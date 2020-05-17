package org.dmitri.jodbc;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ch.qos.logback.classic.Level;


public class JniEntrypoint {
	
	private OdbcDatabaseWrapper database;
	
	private JniEntrypoint() {
		database = new OdbcDatabaseWrapper();
	}
	
	public static JniEntrypoint getInstance() {
		ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger)LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
		root.setLevel(Level.OFF);
		return new JniEntrypoint();
	}
	
	public void setConnectionParameter(String prop, String val) throws  SecurityException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		if(prop == null || prop.trim().isBlank()) {
			return;
		}
		prop = prop.trim().toLowerCase();
		if(prop.equals("debug")) {
			Level level = Level.valueOf(val.trim().toUpperCase());
			ch.qos.logback.classic.Logger root = (ch.qos.logback.classic.Logger)LoggerFactory.getLogger(Logger.ROOT_LOGGER_NAME);
			root.setLevel(level);
			return;
		}
		
		String propSetter = prop.substring(0,1).toUpperCase()+prop.substring(1);
		Method method;
		try {
			method = OdbcDatabaseWrapper.class.getMethod("set"+propSetter, new Class<?>[]{String.class});
		} catch (NoSuchMethodException | SecurityException e) {
			return;
		}
		method.invoke(database, val);
	}
	
	public void connect() {
		database.connect();
	}
	
	public void createStatement(long stmtId) {
		database.createStatement(stmtId);
	}
	
	public void freeStatement(long stmtId, long option) {
		OdbcStatementWrapper stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.freeResource(option);
	}
	
	
	public void getConnectionAttr(long attr, long cbMax) {
		database.getConnectionAttribute(attr,cbMax);
	}
	
	public void setConnectionAttr(long attr, long cbValue) {
		//database.setConnectionAttribute(attr,cbValue);
	}
	
	public void execDirect(long stmtId,String sql) {
		OdbcStatementWrapper stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.execDirect(sql);
	}
	
	public long getRowCount(long stmtId) {
		OdbcStatementWrapper stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getRowCount();
	}
	
	public int getResultColumnCount(long stmtId) {
		OdbcStatementWrapper stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getResultColumnCount();
	}
	
	public Object[] describeColumn(long stmtId, int colNum) {
		OdbcStatementWrapper stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.describeColumn(colNum);
	}

}
