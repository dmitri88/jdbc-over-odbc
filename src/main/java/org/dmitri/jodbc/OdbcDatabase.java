package org.dmitri.jodbc;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import org.dmitri.jodbc.dto.DataTypeInfo;

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
		log.debug("JAVA getStatement "+stmtId);
		if(stmtId == 0) {
			return null;
		}
		return statements.get(stmtId);
	}


	public void setConnectionAttribute(long attr, long cbValue) {
		log.debug("JAVA setConnectionAttribute {} {}",attr,cbValue);
		
	}


	public void getConnectionAttribute(long attr, long cbMax) {
		log.debug("JAVA getConnectionAttribute {} {}",attr,cbMax);
		
	}

}
