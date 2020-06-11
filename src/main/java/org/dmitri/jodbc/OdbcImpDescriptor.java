package org.dmitri.jodbc;

import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import org.dmitri.jodbc.enums.OdbcImpDescriptorField;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

//@RequiredArgsConstructor
@Slf4j
public class OdbcImpDescriptor {

	@Getter
	private static volatile long global_id = 100;

	private static Map<Long, OdbcImpDescriptor> descriptors = new HashMap<>();

	@Getter
	private final long id;
	private final OdbcStatement statement;

	private OdbcImpDescriptor(OdbcStatement statement) {
		this.statement = statement;
		this.id = global_id++;
	}

	public static void remove(OdbcImpDescriptor impDescriptor) {
		if (impDescriptor != null)
			descriptors.remove(impDescriptor.getId());

	}

	public static OdbcImpDescriptor register(OdbcStatement odbcStatement) {
		return new OdbcImpDescriptor(odbcStatement);
	}
	
//	private ResultSet getResult() {
//		return statement.getResult();
//	}
	
	private ResultSetMetaData getMetaData() throws SQLException {
		return statement.getMetaData();
	}

	public Object getDescriptorField(int iRecord, int iField) {
		OdbcImpDescriptorField attr = OdbcImpDescriptorField.valueOf(iField);
		log.debug("JAVA getDescriptorField {} {} {}", statement.getStatementId(), attr != null ? attr : iField,
				iRecord);
		Object ret;
		switch (attr) {
		
		case SQL_DESC_SCALE:
			ret= Long.valueOf(getColumnScale(iRecord));
			break;
		case SQL_DESC_LENGTH:
		case SQL_DESC_PRECISION:
			ret= Long.valueOf(getColumnAttributeByLength(iRecord));
			break;
		case SQL_DESC_COUNT:
			ret= Long.valueOf(getColumnCount());
			break;
		case SQL_DESC_NAME:
			ret= getColumnLabel(iRecord);
			break;
		case SQL_DESC_TYPE_NAME:
			ret= getColumnTypeName(iRecord);
			break;
		default:
			log.warn("getDescriptorField not used {}",attr);
			return null;
		}
		log.trace("{}: {}",attr,ret);
		return ret;
	}
	
	public Long getColumnScale(int iRecord) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			return  Long.valueOf(rsmd.getScale(iRecord));
		} catch (SQLException e) {
			log.error("getColumnScale error", e);
			throw new RuntimeException(e);
		}
	}

	public int getColumnCount() {
		int columnsNumber;
		try {
			ResultSetMetaData rsmd = getMetaData();
			columnsNumber = rsmd.getColumnCount();
		} catch (SQLException e) {
			log.error("getResultColumnCount error", e);
			throw new RuntimeException(e);
		}
		return columnsNumber;
	}
	
	public String getColumnLabel(int colNum) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			String ret =  rsmd.getColumnLabel(colNum);
			return ret;
		} catch (SQLException e) {
			log.error("getColumnLabel error", e);
			throw new RuntimeException(e);
		}
	}
	
	public Long getColumnAttributeByLength(int colNum) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			int length = rsmd.getPrecision(colNum);
			return Long.valueOf(length<100000000?length:0);
		} catch (SQLException e) {
			log.error("getColumnAttributeByLength error", e);
			throw new RuntimeException(e);
		}
	}
	
	public String getColumnTypeName(int colNum) {
		try {
			ResultSetMetaData rsmd = getMetaData();
			String ret =  rsmd.getColumnTypeName(colNum);
			return ret;
		} catch (SQLException e) {
			log.error("getColumnTypeName error", e);
			throw new RuntimeException(e);
		}
	}

}
