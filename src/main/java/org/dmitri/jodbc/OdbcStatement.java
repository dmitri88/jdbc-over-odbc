package org.dmitri.jodbc;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import org.dmitri.jodbc.dto.BoundParameter;
import org.dmitri.jodbc.enums.OdbcBindType;
import org.dmitri.jodbc.enums.OdbcColumnAttribute;
import org.dmitri.jodbc.enums.OdbcDiagField;
import org.dmitri.jodbc.enums.OdbcFetchScrollType;
import org.dmitri.jodbc.enums.OdbcFreeStatement;
import org.dmitri.jodbc.enums.OdbcStatementAttribute;
import org.dmitri.jodbc.enums.OdbcStatus;

import lombok.Getter;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;

@Slf4j
public class OdbcStatement {
	private static int DESCRIPTOR_IMP_ID = 1;

	private final OdbcDatabase database;
	@Getter
	private final long statementId;

	@Getter
	private PreparedStatement statement;
	@Getter
	private ResultSet result;
	private List<BoundParameter> bindParameters = new ArrayList<>();

	@Getter
	private OdbcImpDescriptor impDescriptor = null;

	private Long bindDataRowSize = 0L;// SQL_ATTR_ROW_BIND_TYPE size of wide-arrow element.
										// https://docs.microsoft.com/en-us/sql/odbc/reference/develop-app/binding-arrays-of-parameters?view=sql-server-ver15
	private Long bindDataArraySize = 0L;// SQL_ATTR_ROW_ARRAY_SIZE
	private Long bindDataOffset = 0L;// SQL_ATTR_ROW_BIND_OFFSET_PTR
	private Long bindDataStatus = 0L;// SQL_ATTR_ROW_STATUS_PTR row status array
	private Long bindDataRowsFetched = 0L;// SQL_ATTR_ROWS_FETCHED_PTR number of rows fetched

	private Long statementQueryTimeout;
	// private Long bindDataRowSize2; // SQL_BIND_TYPE
	// private Long sqlRowSetSize; //SQL_ROWSET_SIZE same as SQL_ATTR_ROW_ARRAY_SIZE

	public OdbcStatement(OdbcDatabase database, long statementId) {
		this.database = database;
		this.statementId = statementId;
		impDescriptor = OdbcImpDescriptor.register(this);
	}

	public ResultSetMetaData getMetaData() throws SQLException {
		return result != null ? result.getMetaData() : statement.getMetaData();
	}

	public void freeResource(long option) {
		log.debug("JAVA freeResource {} {}", Utils.toHex(statementId), option);
		OdbcFreeStatement attr = OdbcFreeStatement.valueOf((int) option);
		switch (attr) {
		case SQL_CLOSE:
			freeResourceClose();
			break;
		// case SQL_DROP:
		case SQL_UNBIND:
			freeResourceUnbind();
			break;
		case SQL_RESET_PARAMS:
			freeResourceReset();
			break;
		}

	}

	private void freeResourceClose() {
		if (result != null) {
			try {
				result.close();
			} catch (SQLException e) {
			}
			result = null;
			OdbcImpDescriptor.remove(impDescriptor);
			impDescriptor = null;

		}
	}

	private void freeResourceUnbind() {
		bindParameters.clear();
	}

	private void freeResourceReset() {

	}

	public void execDirect(String sql) {
		log.debug("JAVA execDirect {} {}", Utils.toHex(statementId), sql);
		prepareStatement(sql);
		execute();

	}

	public void execute() {
		log.debug("JAVA execute {}", Utils.toHex(statementId));
		try {
			if (result != null)
				result.close();
		} catch (Exception e) {
		}

		try {
			result = statement.executeQuery();
		} catch (SQLException e) {
			log.error("execDirect error", e);
			throw new RuntimeException(e);
		}
	}

	@SneakyThrows
	public void prepareStatement(String sql) {
		log.debug("JAVA prepareStatement {} {}", Utils.toHex(statementId), sql);
		try {
			if (result != null)
				result.close();
			if (statement != null)
				statement.close();
		} catch (Exception e) {
		}

		try {
			result = null;
			statement = database.getConnection().prepareStatement(sql, ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY);
			OdbcImpDescriptor.remove(impDescriptor);
			impDescriptor = OdbcImpDescriptor.register(this);
		} catch (SQLException e) {
			log.error("prepareStatement error", e);
			throw new RuntimeException(e);
		}

		if (statementQueryTimeout != null)
			statement.setQueryTimeout(statementQueryTimeout.intValue());
	}

	public long getRowCount() {
		log.debug("JAVA getRowCount {}", Utils.toHex(statementId));
		if (statement == null || result == null) {
			throw new RuntimeException("no statement");
		}

		int size = 0;
		try {
			result.last();
			size = result.getRow();
			result.beforeFirst();
		} catch (Exception ex) {
			// log.error("getRowCount error", ex);
			throw new RuntimeException(ex);
		}
		return size;
	}

	private long getRowCountCursor() {
		try {
			return getRowCount();
		} catch (Exception e) {
			return 0;
		}
	}

	public Object[] describeColumn(int colNum) {
		log.debug("JAVA describeColumn {} {}", Utils.toHex(statementId), colNum);
		Object[] ret = new Object[6];
		try {
			ResultSetMetaData rsmd = getMetaData();
			ret[0] = rsmd.getColumnName(colNum);
			ret[1] = rsmd.getColumnLabel(colNum);
			ret[2] = Integer.valueOf(rsmd.getColumnType(colNum));

			ret[4] = Integer.valueOf(getImpDescriptor().getColumnScale(colNum).intValue());
			// ret[5] = Integer.valueOf(rsmd.isNullable(colNum));
			ret[5] = rsmd.isNullable(colNum);
			ret[3] = Integer.valueOf(getImpDescriptor().getColumnAttributeByLength(colNum).intValue());

		} catch (SQLException e) {
			log.error("describeColumn error", e);
			throw new RuntimeException(e);
		}
		return ret;
	}

	@SneakyThrows
	public Object[] getColumnAttribute(int colNum, int descType) {
		OdbcColumnAttribute attr = OdbcColumnAttribute.valueOf(descType);
		Object[] ret;
		log.debug("JAVA getColumnAttribute {} {} {}", Utils.toHex(statementId), colNum, attr != null ? attr : descType);
		switch (attr) {
		case SQL_COLUMN_UNSIGNED:
			ret = new Object[] { getColumnAttributeByUnsigned(colNum) };
			break;
		case SQL_COLUMN_DISPLAY_SIZE:
			ret = new Object[] { getColumnAttributeByDisplaySize(colNum) };
			break;
		case SQL_COLUMN_LENGTH:
			ret = new Object[] { getImpDescriptor().getColumnAttributeByLength(colNum) };
			break;
		case SQL_COLUMN_UPDATABLE:
			ret = new Object[] { getColumnAttributeByUpdatable(colNum) };
			break;
		case SQL_COLUMN_LABEL:
			ret = new Object[] { impDescriptor.getColumnLabel(colNum) };
			break;
		case SQL_COLUMN_AUTO_INCREMENT:
			ResultSetMetaData rsmd = getMetaData();
			ret = new Object[] { rsmd.isAutoIncrement(colNum) ? 1L : 0L };
			break;
		case SQL_COLUMN_TABLE_NAME:
		case SQL_DESC_BASE_COLUMN_NAME:
		case SQL_DESC_BASE_TABLE_NAME:
		case SQL_COLUMN_OWNER_NAME:
		case SQL_COLUMN_QUALIFIER_NAME:
			ret = new Object[] { new String("") };
			break;
		case UNKNOWN_SHIT_1:
			ret = new Object[] { Long.valueOf(0) };
			break;
		default:
			log.warn("UNDEFINED column attibute:" + attr);
			throw new RuntimeException("undefined attr: " + attr);
		// ret = new Object[] {};
		// break;
		}
		return ret;
	}

	private Long getColumnAttributeByDisplaySize(int colNum) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			return Long.valueOf(rsmd.getColumnDisplaySize(colNum));
		} catch (SQLException e) {
			log.error("getColumnAttributeByUnsigned error", e);
			throw new RuntimeException(e);
		}
	}

	private Long getColumnAttributeByUpdatable(int colNum) {
		log.warn("dummy return for getColumnAttributeByUpdatable");
		return Long.valueOf(4);
	}

	private Long getColumnAttributeByUnsigned(int colNum) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			return Long.valueOf(rsmd.isSigned(colNum) ? 1 : 0);
		} catch (SQLException e) {
			log.error("getColumnAttributeByUnsigned error", e);
			throw new RuntimeException(e);
		}
	}

	@SneakyThrows
	public Object[] getStatementAttribute(int attrInt) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA getStatementAttribute {} {}", Utils.toHex(statementId), attr != null ? attr : attrInt);
		switch (attr) {
		case SQL_CURSOR_TYPE:
			return new Object[] { Long.valueOf(0) };// SQL_CURSOR_FORWARD_ONLY
		case SQL_CONCURRENCY:
			return new Object[] { Long.valueOf(1) };// SQL_CONCUR_READ_ONLY
		case SQL_ATTR_ROW_ARRAY_SIZE:
			return new Object[] { bindDataArraySize };
		case SQL_ATTR_APP_ROW_DESC:
		case SQL_ATTR_APP_PARAM_DESC:
		case SQL_ATTR_IMP_PARAM_DESC:
		case SQL_ATTR_IMP_ROW_DESC:
			return new Object[] { /* statementId */0x100 };
		case SQL_ROWSET_SIZE:
			return new Object[] { bindDataArraySize };
		default:
			log.warn("UNDEFINED statement attibute:" + attr);
			throw new RuntimeException("UNDEFINED statement attibute:" + attr);
		// return null;
		}
	}

	private String representValue(OdbcStatementAttribute attr, Long data) {
		if(attr==null)
			return data.toString();
		
		switch (attr) {
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
		case SQL_ATTR_ROW_STATUS_PTR:
		case SQL_ATTR_ROWS_FETCHED_PTR:
			return Utils.toHex(data);
		default:
			return data.toString();
		}

	}

	@SneakyThrows
	public void setStatementAttribute(int attrInt, long data, long index) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA setStatementAttribute {} {} {} {}", Utils.toHex(statementId), attr != null ? attr : attrInt, representValue(attr,data), index);
		switch (attr) {
		case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
			bindDataOffset = data;
			break;
		case SQL_QUERY_TIMEOUT:
			statementQueryTimeout = data;
			break;
		case SQL_ATTR_ROW_ARRAY_SIZE:
			bindDataArraySize = data;
			break;
		case SQL_ATTR_ROW_BIND_OFFSET_PTR:
			bindDataOffset = data;
			break;
		case SQL_ATTR_PARAM_BIND_TYPE:
			this.bindDataRowSize = data;
			break;
		case SQL_ATTR_ROW_STATUS_PTR:
			bindDataStatus = data;
			break;
		case SQL_ATTR_ROWS_FETCHED_PTR:
			bindDataRowsFetched = data;
			break;
		case SQL_BIND_TYPE:
			this.bindDataRowSize = data;
			break;

		case SQL_RETRIEVE_DATA:
			break;
		case SQL_SOPT_SS_HIDDEN_COLUMNS:
		case SQL_SOPT_SS_NOBROWSETABLE:
			break;
		case SQL_ROWSET_SIZE:
			bindDataArraySize = data;
			break;
		default:
			log.warn("setStatementAttribute NOT FOUND " + attr);
			throw new RuntimeException("UNDEFINED statement attibute:" + attr);
		// break;
		}
	}

	public void bindColumn(int column, OdbcBindType type, long dataPtr, long size, long retLength) {
		log.debug("JAVA bindColumn {} {} {} {} {} {}", Utils.toHex(statementId), column, type, dataPtr, size, retLength);
		bindParameters.add(new BoundParameter(column, type, dataPtr, size, retLength));
	}

	@SneakyThrows
	public Object[] fetch() {
		log.debug("JAVA fetch {}}", Utils.toHex(statementId));
		if (statement == null || statement.isClosed()) {
			log.error("fetch: invalid statement");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result == null) {
			log.error("fetch: invalid result");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result.isClosed()) {
			log.error("fetch: result is closed");
			return fetchError(OdbcStatus.SQL_ERROR);
		}

		if (!result.next()) {
			return fetchError(OdbcStatus.SQL_NO_DATA);
		}
		if (bindParameters.size() == 0) {
			return fetchError(OdbcStatus.SQL_SUCCESS);
		}

		int ELEMENTS_PER_PARAMETER = 5;
		int paramCount = bindParameters.size();
		Object[] ret = new Object[2 + paramCount * ELEMENTS_PER_PARAMETER];
		ret[0] = Integer.valueOf(OdbcStatus.SQL_SUCCESS.getType());
		ret[1] = Integer.valueOf(paramCount);

		int offset = 2;
		for (int i = 0; i < paramCount; i++) {
			BoundParameter parameter = bindParameters.get(i);
			ret[offset] = Integer.valueOf(parameter.getType().getType());
			// row-wise binding
			if (bindDataOffset != null) {
				ret[offset + 1] = Long.valueOf(bindDataOffset.longValue() + parameter.getBuffer());
			} else {
				ret[offset + 1] = Long.valueOf(parameter.getBuffer());
			}

			ret[offset + 2] = Long.valueOf(parameter.getBufLen());
			if (bindDataOffset != null) {
				ret[offset + 3] = Long.valueOf(bindDataOffset.longValue() + parameter.getRetBuffer());
			} else {
				ret[offset + 3] = Long.valueOf(parameter.getRetBuffer());
			}
			ret[offset + 4] = fetchColumn(result, parameter.getColumn(), parameter.getType());
			offset += ELEMENTS_PER_PARAMETER;
		}

		return ret;
	}

	private Object fetchColumn(ResultSet rs, int column, OdbcBindType type) throws SQLException {
		Object ret;
		switch (type) {
		case SQL_C_LONG:
		case SQL_C_SHORT:
		case SQL_C_TINYINT:
		case SQL_C_SSHORT:
		case SQL_C_SLONG:
		case SQL_C_STINYINT:
		case SQL_C_USHORT:
		case SQL_C_ULONG:
		case SQL_C_UTINYINT:
			long long1 = rs.getLong(column);
			if (rs.wasNull())
				return null;
			ret = Long.valueOf(long1);
			break;
		case SQL_C_CHAR:
			String str = rs.getString(column);
			if (rs.wasNull())
				return null;
			ret = str;
			break;
		// case SQL_C_DOUBLE:
		// case SQL_C_FLOAT:

		default:
			throw new RuntimeException("fetch type not supported: " + type);

		}
		// return null;
		log.trace("getData: column:{} -> {} ({})", column, ret, ret.getClass().getSimpleName());
		return ret;
	}

	private Object[] fetchError(OdbcStatus error) {
		Object[] ret = new Object[2];
		ret[0] = Integer.valueOf(error.getType());
		ret[1] = Integer.valueOf(0);
		return ret;
	}

	@SneakyThrows
	public int moreResults() {
		log.debug("JAVA moreResults {}}", Utils.toHex(statementId));
		if (statement == null || statement.isClosed()) {
			log.error("moreResults: invalid statement");
			return OdbcStatus.SQL_ERROR.getType();
		}
		if (statement.getMoreResults()) {
			result = statement.getResultSet();
			return OdbcStatus.SQL_SUCCESS.getType();
		}
		return OdbcStatus.SQL_NO_DATA.getType();
	}

	@SneakyThrows
	public Object[] getData(int column, OdbcBindType bindType) {
		log.debug("JAVA getData {} {}", Utils.toHex(statementId), bindType);
		if (statement == null || statement.isClosed()) {
			log.error("fetch: invalid statement");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result == null) {
			log.error("fetch: invalid result");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result.isClosed()) {
			log.error("fetch: result is closed");
			return fetchError(OdbcStatus.SQL_ERROR);
		}

		int ELEMENTS_PER_PARAMETER = 5;
		int paramCount = 1;
		Object[] ret = new Object[2 + paramCount * ELEMENTS_PER_PARAMETER];
		ret[0] = Integer.valueOf(OdbcStatus.SQL_SUCCESS.getType());
		ret[1] = Integer.valueOf(paramCount);

		int offset = 2;
		for (int i = 0; i < paramCount; i++) {
			ret[offset] = Integer.valueOf(bindType.getType());
			ret[offset + 1] = Long.valueOf(0L);
			ret[offset + 2] = Long.valueOf(0L);
			ret[offset + 3] = Long.valueOf(0L);
			ret[offset + 4] = fetchColumn(result, column, bindType);
			offset += ELEMENTS_PER_PARAMETER;
		}
		return ret;

	}

	public Object getDiagField(int iRecord, int fDiagField) {
		OdbcDiagField attr = OdbcDiagField.valueOf(fDiagField);
		Object ret;
		log.debug("JAVA getDiagField {} {} {}", Utils.toHex(statementId), attr != null ? attr : fDiagField, iRecord);
		switch (attr) {
		case SQL_DIAG_CURSOR_ROW_COUNT:
			ret = Long.valueOf(getRowCountCursor());
			break;
		default:
			log.warn("getDiagField NOT FOUND " + attr);
			throw new RuntimeException("UNDEFINED statement attibute:" + attr);
		// break;
		}
		return ret;
	}

	@SneakyThrows
	public Object[] fetchScroll(OdbcFetchScrollType type, int rowOffset) {
		log.debug("JAVA fetchScroll {} {} {}", Utils.toHex(statementId), type, rowOffset);

		if (statement == null || statement.isClosed()) {
			log.error("fetch: invalid statement");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result == null) {
			log.error("fetch: invalid result");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if (result.isClosed()) {
			log.error("fetch: result is closed");
			return fetchError(OdbcStatus.SQL_ERROR);
		}

		if (type != OdbcFetchScrollType.SQL_FETCH_NEXT) {
			log.error("fetch type  not supported : {}", type);
			return fetchError(OdbcStatus.SQL_ERROR);
		}

		if (rowOffset != 0L) {
			log.error("rowOffset  not supported : {}", rowOffset);
			return fetchError(OdbcStatus.SQL_ERROR);
		}

		int ELEMENTS_PER_PARAMETER = 6;
		int offset = 5;
		int paramCount = bindParameters.size();
		Object[] ret = new Object[offset + paramCount * ELEMENTS_PER_PARAMETER * bindDataArraySize.intValue()];
		ret[0] = Integer.valueOf(OdbcStatus.SQL_SUCCESS.getType());
		ret[1] = Integer.valueOf(paramCount);
		ret[2] = 0; // ret row counter
		ret[3] = bindDataStatus; // SQL_ATTR_ROW_STATUS_PTR row status array
		ret[4] = bindDataRowsFetched; // SQL_ATTR_ROWS_FETCHED_PTR row status array

		long dataOffset = 0L;
		long rowSize = bindDataRowSize.longValue();
		if (bindDataOffset != null) {
			dataOffset = bindDataOffset.longValue();
		}

		int row = 0;
		for (row = 0; row < bindDataArraySize.intValue(); row++) {
			if (!result.next()) {
				if (row == 0) { // no data
					ret[0] = Integer.valueOf(OdbcStatus.SQL_NO_DATA.getType());
					return ret;
				}
				break;
			}

			for (int i = 0; i < paramCount; i++) {
				BoundParameter parameter = bindParameters.get(i);
				ret[offset] = Integer.valueOf(parameter.getType().getType());
				ret[offset + 1] = Long.valueOf(dataOffset + parameter.getBuffer() + row * rowSize);
				ret[offset + 2] = Long.valueOf(parameter.getBufLen());
				ret[offset + 3] = Long.valueOf(dataOffset + parameter.getRetBuffer() + row * rowSize);
				ret[offset + 4] = fetchColumn(result, parameter.getColumn(), parameter.getType());
				ret[offset + 5] = Long.valueOf(0L);// status????
				offset += ELEMENTS_PER_PARAMETER;
			}

		}
		ret[2] = Integer.valueOf(row);
		return ret;
	}

}
