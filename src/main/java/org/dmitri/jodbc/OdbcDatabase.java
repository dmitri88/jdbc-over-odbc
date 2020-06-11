package org.dmitri.jodbc;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;

import org.dmitri.jodbc.dto.DataTypeInfo;
import org.dmitri.jodbc.enums.OdbcConnectionAttribute;
import org.dmitri.jodbc.enums.OdbcInfoType;

import lombok.Getter;
import lombok.Setter;
import lombok.SneakyThrows;
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
	private Map<Long, OdbcStatement> statements = new HashMap<>();

	private Map<Integer, DataTypeInfo> infos = new HashMap<>();

	public String connect(String dsn) {
		log.debug("JAVA connect {}",dsn);
		try {
			Class.forName(driverclass);
		} catch (ClassNotFoundException e) {
			throw new RuntimeException(e);
		}
		log.debug("JDBC Driver Registered!");
		try {
			connection = DriverManager.getConnection(url, user, password);
			log.debug("SQL Connection to database established!");

		} catch (SQLException e) {
			log.error("Connection Failed! Check output console");
			throw new RuntimeException(e);
		}
		if(dsn.endsWith(";"))
			return dsn;
		else
			return dsn + ";";
	}

	public DataTypeInfo getTypeInfo(int typeId) {
		if (infos.size() > 0) {
			return infos.get(typeId);
		}

		try {

			DatabaseMetaData metaData = this.connection.getMetaData();

			ResultSet set = metaData.getTypeInfo();
			while (set.next()) {
				DataTypeInfo info = new DataTypeInfo();
				info.setName(set.getString("TYPE_NAME"));
				info.setTypeId(set.getInt("DATA_TYPE"));
				info.setUnsigned(set.getBoolean("UNSIGNED_ATTRIBUTE"));
				// info.setSize(set.getInt("COLUMN_SIZE"));

				infos.put(info.getTypeId(), info);

			}
		} catch (SQLException e) {
			throw new RuntimeException(e);
		}
		return infos.get(typeId);
	}

	public void createStatement(long stmtId) {
		log.debug("JAVA createStatement " + stmtId);
		statements.put(stmtId, new OdbcStatement(this, stmtId));
	}

	public OdbcStatement getStatement(long stmtId) {
		if (stmtId == 0) {
			return null;
		}
		return statements.get(stmtId);
	}

	public void setConnectionAttribute(long attr, long cbValue) {
		log.debug("JAVA setConnectionAttribute {} {}", attr, cbValue);

	}

	@SneakyThrows
	public Object[] getConnectionAttribute(long attrLong) {
		OdbcConnectionAttribute attr = OdbcConnectionAttribute.valueOf((int) attrLong);
		log.debug("JAVA getConnectionAttribute {} ", attr != null ? attr : attrLong);

		if (attr == null) {
			log.warn("UNDEFINED connection attibute:" + attrLong);
			return new Object[0];
		}

		Object[] ret;
		ret = new Object[1];

		switch (attr) {
		case SQL_CURRENT_QUALIFIER:
			ret[0] = getCurrentCatalog();
			break;
		case SQL_TXN_ISOLATION:
			ret[0] = connection.getTransactionIsolation();
			break;
		case SQL_CA_SS_VARIANT_SERVER_TYPE:
			ret[0] = Long.valueOf(4);
			break;		
		default:
			log.warn("UNDEFINED connection attibute:" + attr);
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
			if (resultSet.next()) {
				return resultSet.getString(1);
			}

		} catch (SQLException e) {
			throw new RuntimeException(e);
		} finally {
			if (resultSet != null)
				try {
					resultSet.close();
				} catch (SQLException e) {
				}
			if (statement != null)
				try {
					statement.close();
				} catch (SQLException e) {
				}
		}
		return null;
	}

	public Object[] getInfo(int fieldId) {
		OdbcInfoType attr = OdbcInfoType.valueOf(fieldId);
		log.debug("JAVA getInfo {} ", attr != null ? attr : fieldId);

		Object[] ret;
		ret = new Object[1];

		switch (attr) {
		case SQL_DATA_SOURCE_READ_ONLY:
			ret[0] = "N";
			break;
		case SQL_ODBC_SQL_CONFORMANCE:
			ret[0] = Integer.valueOf(1);
			break;		
		case SQL_ODBC_API_CONFORMANCE:
			ret[0] = Integer.valueOf(2);
			break;
		case SQL_DRIVER_NAME:
			ret[0] = "msodbcsql17.dl";
			break;
		case SQL_ODBC_VER:
			ret[0] = "03.80.0000";
			break;
		case SQL_DRIVER_ODBC_VER:
			ret[0] = "03.80";
			break;
		case SQL_NEED_LONG_DATA_LEN:
			ret[0] = "Y";
			break;
		case SQL_MULT_RESULT_SETS:
			ret[0] = "Y";
			break;
		case SQL_OWNER_TERM:
			ret[0] = "owner";
			break;
		case SQL_IDENTIFIER_QUOTE_CHAR:
			ret[0] = "\"";
			break;
		case SQL_QUALIFIER_NAME_SEPARATOR:
			ret[0] = ".";
			break;
		case SQL_DBMS_NAME:
			try {
				DatabaseMetaData metaData = this.connection.getMetaData();
				ret[0] = metaData.getDatabaseProductName();
			} catch (SQLException e) {
				throw new RuntimeException(e);
			}
			break;
		case SQL_DATABASE_NAME:
				ret[0] = getCurrentCatalog();
			break;
		case SQL_DBMS_VER:
			try {
				DatabaseMetaData metaData = this.connection.getMetaData();
				ret[0] = metaData.getDatabaseProductVersion();
			} catch (SQLException e) {
				throw new RuntimeException(e);
			}
			break;
		case SQL_POS_OPERATIONS:
			ret[0] = Integer.valueOf(0x1f);
			break;
		case SQL_POSITIONED_STATEMENTS:
			ret[0] = Integer.valueOf(0x0);  //??? SQL_PS_POSITIONED_DELETE | 			   SQL_PS_POSITIONED_UPDATE | 			   SQL_PS_SELECT_FOR_UPDATE
			break;
		case SQL_STATIC_SENSITIVITY:
			ret[0] = Integer.valueOf(5);
			break;
		case SQL_LOCK_TYPES:
			ret[0] = Integer.valueOf(1);
			break;
		case SQL_GETDATA_EXTENSIONS:
			ret[0] = Integer.valueOf(4);
			break;
		case SQL_TXN_ISOLATION_OPTION:
			ret[0] = Integer.valueOf(0x2f);
			break;
		case SQL_BOOKMARK_PERSISTENCE:
			ret[0] = Integer.valueOf(0x52);
			break;
		case SQL_SCROLL_OPTIONS:
			ret[0] = Integer.valueOf(0x17);
			break;
		case SQL_SCROLL_CONCURRENCY:
			ret[0] = Integer.valueOf(0xf);
			break;
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
			ret[0] = Integer.valueOf(0x0001FE47);
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES1:
			ret[0] = Integer.valueOf(0x000FFE4F);
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES1:
			ret[0] = Integer.valueOf(0x0008124F);
			break;
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
			ret[0] = Integer.valueOf(0x0000E001);
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES2:
			ret[0] = Integer.valueOf(0x00011FDF);
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES2:
			ret[0] = Integer.valueOf(0x00001F81);
			break;
		case SQL_CURSOR_COMMIT_BEHAVIOR:
			ret[0] = Integer.valueOf(1);
			break;
		case SQL_CURSOR_ROLLBACK_BEHAVIOR:
			ret[0] = Integer.valueOf(1);
			break;
		case SQL_TXN_CAPABLE:
			ret[0] = Integer.valueOf(2);
			break;
		case SQL_MAX_CATALOG_NAME_LEN:
			ret[0] = Integer.valueOf(128);
			break;
		case SQL_MAX_COLUMN_NAME_LEN:
			ret[0] = Integer.valueOf(128);
			break;
		case SQL_MAX_SCHEMA_NAME_LEN:
			ret[0] = Integer.valueOf(128);
			break;
		case SQL_MAX_TABLE_NAME_LEN:
			ret[0] = Integer.valueOf(128);
			break;
		case SQL_ACTIVE_STATEMENTS:
			ret[0] = Integer.valueOf(1);
			break;
		case SQL_DEFAULT_TXN_ISOLATION:
			ret[0] = Integer.valueOf(2);
			break;
		default:
			throw new RuntimeException("getInfo missing field " + (attr != null ? attr : fieldId));
		}
		return ret;
	}

	public String getNativeSql(String sql) {
		log.debug("JAVA getNativeSql {} ", sql);
		if (sql == null || sql.trim().isBlank())
			throw new RuntimeException("getNativeSql is blank");
		return sql;
	}

}
