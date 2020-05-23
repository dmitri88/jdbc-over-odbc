package org.dmitri.jodbc;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import org.dmitri.jodbc.enums.OdbcBindType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import ch.qos.logback.classic.Level;


public class JniEntrypoint {
	
	private OdbcDatabase database;
	
	private JniEntrypoint() {
		database = new OdbcDatabase();
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
			//root.setLevel(Level.OFF);
			return;
		}
		
		String propSetter = prop.substring(0,1).toUpperCase()+prop.substring(1);
		Method method;
		try {
			method = OdbcDatabase.class.getMethod("set"+propSetter, new Class<?>[]{String.class});
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
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.freeResource(option);
	}
	
	
	public Object[] getConnectionAttr(long attr) {
		return database.getConnectionAttribute(attr);
	}
	
	public void setConnectionAttr(long attr, long cbValue) {
		//database.setConnectionAttribute(attr,cbValue);
	}
	
	public void execDirect(long stmtId,String sql) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.execDirect(sql);
	}
	
	public void execute(long stmtId) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.execute();
	}	
	
	public void prepareStatement(long stmtId,String sql) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.prepareStatement(sql);
	}
	
	public long getRowCount(long stmtId) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getRowCount();
	}
	
	public int getResultColumnCount(long stmtId) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getImpDescriptor().getColumnCount();
	}
	
	public Object[] describeColumn(long stmtId, int colNum) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.describeColumn(colNum);
	}
	
	public Object[]  getColumnAttribute(long stmtId, int colNum,int descType) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getColumnAttribute(colNum,descType);
	}

	
	public Object[] getStatementAttribute(long stmtId,int attr) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getStatementAttribute(attr);
	}
	
	
	public void setStatementAttribute(long stmtId,int attr, long data) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.setStatementAttribute(attr,data);	
	}
	
	public Object[] getInfo(int fieldId) {
		return database.getInfo(fieldId);
	}
	
	public String getNativeSql(String sql) {
		return database.getNativeSql(sql);
	}
	
	public void bindColumn(long stmtId,int column,int type, long dataPtr,long size, long retLength) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		stmt.bindColumn(column, OdbcBindType.valueOf(type), dataPtr, size, retLength);
	}
	
	public Object[] fetch(long stmtId) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.fetch();
	}
	
	public int moreResults(long stmtId) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.moreResults();
	}
	
	public Object getDescriptorField(long stmtId , int iRecord , int iField) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}	
		
		OdbcImpDescriptor descriptor = stmt.getImpDescriptor();
		if(descriptor == null) {
			throw new RuntimeException("descriptor not found "+ stmtId);
		}	
		return descriptor.getDescriptorField(iRecord , iField);
	}
	
	public Object[] getData(long stmtId,int column,int type) {
		OdbcStatement stmt = database.getStatement(stmtId);
		if(stmt == null) {
			throw new RuntimeException("statement not found "+ stmtId);
		}
		return stmt.getData(column, OdbcBindType.valueOf(type));
	}
}
