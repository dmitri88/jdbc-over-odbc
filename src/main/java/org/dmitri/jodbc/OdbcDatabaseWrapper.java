package org.dmitri.jodbc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

@Slf4j
public class OdbcDatabaseWrapper {
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
	
	private Connection connection;
	private Map<Long,OdbcStatementWrapper> statements =  new HashMap<>();
	
	
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


	public void createStatement(long stmtId) {
		log.debug("JAVA createStatement "+stmtId);
		statements.put(stmtId,new OdbcStatementWrapper(stmtId));
	}


	public OdbcStatementWrapper getStatement(long stmtId) {
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
