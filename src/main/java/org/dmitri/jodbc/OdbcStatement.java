package org.dmitri.jodbc;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import org.dmitri.jodbc.dto.DataTypeInfo;
import org.dmitri.jodbc.enums.OdbcColumnAttribute;
import org.dmitri.jodbc.enums.OdbcStatementAttribute;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@RequiredArgsConstructor
public class OdbcStatement {
	
	private final OdbcDatabase database;
	private final long statementId;
	
	private PreparedStatement statement;
	private ResultSet result;

	public void freeResource(long option) {
		log.debug("JAVA freeResource {} {}",statementId,option);
	}

	public void execDirect(String sql) {
		log.debug("JAVA execDirect {} {}",statementId,sql);
		try {
			statement = database.getConnection().prepareStatement(sql, ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
			result = statement.executeQuery();
		} catch (SQLException e) {
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
			ret[5] = 1;
			switch(Integer.valueOf(rsmd.getColumnType(colNum))) {
			case 4:
				ret[3] = 10; // colSize
				break;
			default:
				ret[3] = Integer.valueOf(0); // colSize
			}
			
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return ret;
	}
	
	public Object[]  getColumnAttribute(int colNum,int descType) {
		OdbcColumnAttribute attr = OdbcColumnAttribute.valueOf(descType);
		log.debug("JAVA getColumnAttribute {} {} {}",statementId, colNum, attr!=null?attr:descType);
		switch (attr) {
		case SQL_COLUMN_UNSIGNED:
			return getColumnAttributeByUnsigned(colNum);
		case SQL_COLUMN_LENGTH:
			return getColumnAttributeByLength(colNum);
		case SQL_COLUMN_UPDATABLE:
			return getColumnAttributeByUpdatable(colNum);
		case SQL_COLUMN_LABEL:
			return getColumnAttributeByLabel(colNum);
		case SQL_COLUMN_AUTO_INCREMENT:
		case SQL_DESC_BASE_COLUMN_NAME:
		case SQL_DESC_BASE_TABLE_NAME:
		case SQL_COLUMN_OWNER_NAME:
		case SQL_COLUMN_QUALIFIER_NAME:
		case UNKNOWN_SHIT_1:
			return new Object[] {};
		default:
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
			int typeid = rsmd.getColumnType(colNum);
			DataTypeInfo typeInfo = this.database.getTypeInfo(typeid);
			ret[0]=Long.valueOf(typeInfo.isUnsigned()?1:0);
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return ret;	
	}

	private Object[] getColumnAttributeByLength(int colNum) {
		Object[] ret = new Object[1];
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			ret[0]=Long.valueOf(rsmd.getColumnDisplaySize(colNum));
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return ret;	
	}

	private Object[]  getColumnAttributeByLabel(int colNum) {
		Object[] ret = new Object[1];
		try {
			ResultSetMetaData rsmd = result.getMetaData();
			ret[0]=rsmd.getColumnLabel(colNum);
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return ret;	
	}

	public Object[] getStatementAttribute(int attrInt) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA getStatementAttribute {} {}",statementId, attr!=null?attr:attrInt);
		switch(attr) {
		case SQL_CURSOR_TYPE:
			return new Object[] { Long.valueOf(0)};//SQL_CURSOR_FORWARD_ONLY
		case SQL_CONCURRENCY:
			return new Object[] { Long.valueOf(1)};//SQL_CONCUR_READ_ONLY
		default:
				return null;
		}
	}

	public void setStatementAttribute(int attrInt, long data) {
		OdbcStatementAttribute attr = OdbcStatementAttribute.valueOf(attrInt);
		log.debug("JAVA setStatementAttribute {} {} {}",statementId, attr!=null?attr:attrInt,data);
		
	}

}
