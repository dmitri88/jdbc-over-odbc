package org.dmitri.jodbc;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@RequiredArgsConstructor
public class OdbcStatementWrapper {
	
	private final OdbcDatabaseWrapper database;
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
			ret[3] = Integer.valueOf(0); // colSize
			ret[4] = Integer.valueOf(0); //decimalDigits
			ret[5] = Integer.valueOf(rsmd.isNullable(colNum));
			
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return ret;
	}

}
