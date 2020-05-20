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
import org.dmitri.jodbc.enums.OdbcFreeStatement;
import org.dmitri.jodbc.enums.OdbcStatementAttribute;
import org.dmitri.jodbc.enums.OdbcStatus;

import lombok.RequiredArgsConstructor;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@RequiredArgsConstructor
public class OdbcStatement {
	
	private final OdbcDatabase database;
	private final long statementId;
	
	private PreparedStatement statement;
	private ResultSet result;
	private List<BoundParameter> bindParameters =  new ArrayList<>();
	
	//private int fetchSize = 1;

	public void freeResource(long option) {
		log.debug("JAVA freeResource {} {}",statementId,option);
		OdbcFreeStatement attr = OdbcFreeStatement.valueOf((int)option);
		switch(attr) {
		case SQL_CLOSE:
			freeResourceClose();
			break;
		//case SQL_DROP:
		case SQL_UNBIND:
			freeResourceUnbind();
			break;
		case SQL_RESET_PARAMS:
			freeResourceReset();
			break;
		}
		
	}
	private void freeResourceClose() {
		if(result != null) {
			try {
				result.close();
			} catch (SQLException e) {
			}
			result = null;
		}
	}
	
	private void freeResourceUnbind() {
		bindParameters.clear();
	}
	
	private void freeResourceReset() {
		
	}

	public void execDirect(String sql) {
		log.debug("JAVA execDirect {} {}",statementId,sql);
		try {
			if(result!=null)
				result.close();
			if(statement !=null)
				statement.close();
		} catch (Exception e) {
		}
		
		try {
			
			statement = database.getConnection().prepareStatement(sql, ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
			result = statement.executeQuery();
		} catch (SQLException e) {
			log.error("execDirect error",e);
			throw new RuntimeException(e);
		}
		
	}

	public long getRowCount() {
		log.debug("JAVA getRowCount {}",statementId);
		if(statement == null || result == null) {
			throw new RuntimeException("no statement");
		}
		
		int size = 0;
		try {
			result.last();
		    size = result.getRow();
		    result.beforeFirst();
		}
		catch(Exception ex) {
			log.error("getRowCount error",ex);
		    return 0;
		}
		return size;		
	}

	public int getResultColumnCount() {
		int columnsNumber;
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			columnsNumber = rsmd.getColumnCount();
		} catch (SQLException e) {
			log.error("getResultColumnCount error",e);
			throw new RuntimeException(e);
		}
		return columnsNumber;
	}

	public Object[] describeColumn(int colNum) {
		log.debug("JAVA describeColumn {} {}",statementId, colNum);
		Object[] ret = new Object[6];
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			ret[0]=rsmd.getColumnName(colNum);
			ret[1]=rsmd.getColumnLabel(colNum);
			ret[2]=Integer.valueOf(rsmd.getColumnType(colNum));
			
			ret[4] = Integer.valueOf(0); //decimalDigits
			//ret[5] = Integer.valueOf(rsmd.isNullable(colNum));
			ret[5] = rsmd.isNullable(colNum);
			ret[3] = Integer.valueOf(getColumnAttributeByLength(colNum).intValue());
			
		} catch (SQLException e) {
			log.error("describeColumn error",e);
			throw new RuntimeException(e);
		}
		return ret;
	}
	
	@SneakyThrows
	public Object[]  getColumnAttribute(int colNum,int descType) {
		OdbcColumnAttribute attr = OdbcColumnAttribute.valueOf(descType);
		log.debug("JAVA getColumnAttribute {} {} {}",statementId, colNum, attr!=null?attr:descType);
		switch (attr) {
		case SQL_COLUMN_UNSIGNED:
			return getColumnAttributeByUnsigned(colNum);
		case SQL_COLUMN_LENGTH:
			return new Object[] {getColumnAttributeByLength(colNum)};
		case SQL_COLUMN_UPDATABLE:
			return getColumnAttributeByUpdatable(colNum);
		case SQL_COLUMN_LABEL:
			return getColumnAttributeByLabel(colNum);
		case SQL_COLUMN_AUTO_INCREMENT:
			ResultSetMetaData rsmd = result.getMetaData();
			return new Object[] {rsmd.isAutoIncrement(colNum) ? 1L:0L };
		case SQL_DESC_BASE_COLUMN_NAME:
		case SQL_DESC_BASE_TABLE_NAME:
		case SQL_COLUMN_OWNER_NAME:
		case SQL_COLUMN_QUALIFIER_NAME:
		default:
			log.warn("UNDEFINED column attibute:"+attr);
			//throw new RuntimeException("undefined attr: "+attr);
			return new Object[] {};
		}
		//return null;
	}
	
	
	private Object[] getColumnAttributeByUpdatable(int colNum) {
		Object[] ret = new Object[1];
			ret[0]=Long.valueOf(4);
		return ret;	
	}

	private Object[] getColumnAttributeByUnsigned(int colNum) {
		Object[] ret = new Object[1];
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			ret[0]=Long.valueOf(rsmd.isSigned(colNum)?1:0);
		} catch (SQLException e) {
			log.error("getColumnAttributeByUnsigned error",e);
			throw new RuntimeException(e);
		}
		return ret;	
	}

	private Long getColumnAttributeByLength(int colNum) {
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			return Long.valueOf(rsmd.getPrecision(colNum));
		} catch (SQLException e) {
			log.error("getColumnAttributeByLength error",e);
			throw new RuntimeException(e);
		}
	}

	private Object[]  getColumnAttributeByLabel(int colNum) {
		Object[] ret = new Object[1];
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			ret[0]=rsmd.getColumnLabel(colNum);
		} catch (SQLException e) {
			log.error("getColumnAttributeByLabel error",e);
			throw new RuntimeException(e);
		}
		return ret;	
	}

	@SneakyThrows
	public Object[] getStatementAttribute(int attrInt) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA getStatementAttribute {} {}",statementId, attr!=null?attr:attrInt);
		switch(attr) {
		case SQL_CURSOR_TYPE:
			return new Object[] { Long.valueOf(0)};//SQL_CURSOR_FORWARD_ONLY
		case SQL_CONCURRENCY:
			return new Object[] { Long.valueOf(1)};//SQL_CONCUR_READ_ONLY
		case SQL_ATTR_ROW_ARRAY_SIZE:
			return new Object[] { Long.valueOf(statement.getFetchSize())};
		default:
			log.warn("UNDEFINED statement attibute:"+attr);
				return null;
		}
	}

	@SneakyThrows
	public void setStatementAttribute(int attrInt, long data) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA setStatementAttribute {} {} {}",statementId, attr!=null?attr:attrInt,data);
		switch (attr) {
		case SQL_ATTR_ROW_ARRAY_SIZE:
			statement.setFetchSize((int)data);
			//fetchSize = (int)data;
			break;

		default:
			log.warn("setStatementAttribute NOT FOUND "+attr);
			break;
		}
	}
	
	public void bindColumn(int column,OdbcBindType type, long dataPtr,long size, long retLength) {
		log.debug("JAVA bindColumn {} {} {} {} {} {}",statementId, column,type,dataPtr,size,retLength);
		bindParameters.add(new BoundParameter(column, type,dataPtr, size, retLength));
	}
	@SneakyThrows
	public Object[] fetch() {
		log.debug("JAVA fetch {}}",statementId);
			if(statement == null || statement.isClosed()) {
				log.error("fetch: invalid statement");
				return fetchError(OdbcStatus.SQL_ERROR);
			}
		if(result == null) {
			log.error("fetch: invalid result");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if(result.isClosed()) {
			log.error("fetch: result is closed");
			return fetchError(OdbcStatus.SQL_ERROR);
		}
		if(!result.next()) {
			return fetchError(OdbcStatus.SQL_NO_DATA);
		}
		if(bindParameters.size() ==0) {
			return fetchError(OdbcStatus.SQL_SUCCESS);
		}
		
		int ELEMENTS_PER_PARAMETER = 5;
		int paramCount = bindParameters.size();
		Object[] ret = new Object[2+paramCount*ELEMENTS_PER_PARAMETER];
		ret[0] = Integer.valueOf(OdbcStatus.SQL_SUCCESS.getType());
		ret[1] = Integer.valueOf(paramCount);
		
		int offset = 2;
		for(int i=0;i<paramCount;i++) {
			BoundParameter parameter = bindParameters.get(i);
			ret[offset] = Integer.valueOf(parameter.getType().getType());
			ret[offset+1] = Long.valueOf(parameter.getBuffer());
			ret[offset+2] = Long.valueOf(parameter.getBufLen());
			ret[offset+3] = Long.valueOf(parameter.getRetBuffer());
			ret[offset+4] = fetchColumn(result,parameter.getColumn(),parameter.getType());
			offset +=ELEMENTS_PER_PARAMETER;
		}
		
		return ret;
	}
	
	private Object fetchColumn(ResultSet rs, int column, OdbcBindType type) throws SQLException {
		switch(type) {
		case SQL_C_CHAR:
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
			if(rs.wasNull())
				return null;
			return Long.valueOf(long1);
			
		//case SQL_C_DOUBLE:
		//case SQL_C_FLOAT:
		
			default:
				throw new RuntimeException("fetch type not supported: "+type);
				
		}
		//return null;
	}
	
	private Object[] fetchError(OdbcStatus error) {
		Object[] ret =  new Object[2];
		ret[0] = Integer.valueOf(error.getType());
		ret[1] = Integer.valueOf(0);
		return ret;
	}
	

}
