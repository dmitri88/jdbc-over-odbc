package org.dmitri.jodbc;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;

import javax.management.RuntimeErrorException;

import org.dmitri.jodbc.dto.DataTypeInfo;
import org.dmitri.jodbc.enums.OdbcConnectionAttribute;
import org.dmitri.jodbc.enums.OdbcInfoType;

import lombok.Getter;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

@Slf4j
public class OdbcDatabase {
	@Setter
	private String password;
	@Setter
	private String classpath;
	@Setter
	private String user;
	@Setter
	private String url;
	@Setter
	private String driverclass;
	
	@Getter
	private Connection connection;
	private Map<Long,OdbcStatement> statements =  new HashMap<>();
	
	private Map<Integer,DataTypeInfo> infos = new HashMap<>();
	
	
	public void connect() {
        try
        {
            Class.forName(driverclass);
        } 
        catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
        log.debug("JDBC Driver Registered!");
        try {
            connection = DriverManager
                .getConnection(url, user, password);
            log.debug("SQL Connection to database established!");
 
        } catch (SQLException e) {
        	log.error("Connection Failed! Check output console");
        	throw new RuntimeException(e);
        } 		
	}
	public DataTypeInfo getTypeInfo(int typeId) {
		if(infos.size()>0) {
			return infos.get(typeId);
		}
		
		try {
			DatabaseMetaData metaData = this.connection.getMetaData();
			ResultSet set = metaData.getTypeInfo();
			while(set.next()) {
				DataTypeInfo info =  new DataTypeInfo();
				info.setName(set.getString("TYPE_NAME"));
				info.setTypeId(set.getInt("DATA_TYPE"));
				info.setUnsigned(set.getBoolean("UNSIGNED_ATTRIBUTE") );
				
				infos.put(info.getTypeId(), info);
				
			}
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return infos.get(typeId);
	}


	public void createStatement(long stmtId) {
		log.debug("JAVA createStatement "+stmtId);
		statements.put(stmtId,new OdbcStatement(this,stmtId));
	}


	public OdbcStatement getStatement(long stmtId) {
		if(stmtId == 0) {
			return null;
		}
		return statements.get(stmtId);
	}


	public void setConnectionAttribute(long attr, long cbValue) {
		log.debug("JAVA setConnectionAttribute {} {}",attr,cbValue);
		
	}


	public Object[] getConnectionAttribute(long attrLong) {
		OdbcConnectionAttribute attr = OdbcConnectionAttribute.valueOf((int)attrLong);
		log.debug("JAVA getConnectionAttribute {} ", attr!=null?attr:attrLong);
		
		if(attr==null) {
			return new Object[0];
		}
		
		Object[] ret;
		ret = new Object[1];
		
		switch (attr) {
		case SQL_CURRENT_QUALIFIER:
			ret[0] = getCurrentCatalog();
			break;

		default:
			return new Object[0];
		}  	
		return ret;			
	}
	private String getCurrentCatalog() {
		Statement statement = null;
		ResultSet resultSet = null;
		try {
			statement = this.connection.createStatement();
			resultSet = statement.executeQuery("SELECT DB_NAME()");
			if(resultSet.next()) {
				return resultSet.getString(1);
			}
		
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}	finally {
			if(resultSet != null)
				try {
					resultSet.close();
				} catch (SQLException e) {
				}
			if(statement != null)
				try {
					statement.close();
				} catch (SQLException e) {
				}			
		}
		return null;
	}
	public Object[] getInfo(int fieldId) {
		OdbcInfoType attr = OdbcInfoType.valueOf(fieldId);
		log.debug("JAVA getInfo {} ", attr!=null?attr:fieldId);
		
		
		Object[] ret;
		ret = new Object[1];
		
		switch (attr) {
		case SQL_ODBC_VER:
			ret[0] = "03.80.0000";
			break;
		case SQL_DRIVER_ODBC_VER:
			ret[0] = "03.80";
			break;
		case SQL_DBMS_NAME:
			try {
				DatabaseMetaData metaData = this.connection.getMetaData();
				ret[0] = metaData.getDatabaseProductName();
			} catch (SQLException e) {
				throw new RuntimeException(e);
			}
			break;

		default:
			throw new RuntimeException("getInfo missing field "+(attr!=null?attr:fieldId));
		}  	
		return ret;
	}

}