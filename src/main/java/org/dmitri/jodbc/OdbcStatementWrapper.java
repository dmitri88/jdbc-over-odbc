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

}
