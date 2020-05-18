package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcConnectionAttribute {
	/* connection attributes */
	SQL_ACCESS_MODE                 (101),
	SQL_AUTOCOMMIT                  (102),
	SQL_LOGIN_TIMEOUT               (103),
	SQL_OPT_TRACE                   (104),
	SQL_OPT_TRACEFILE               (105),
	SQL_TRANSLATE_DLL               (106),
	SQL_TRANSLATE_OPTION            (107),
	SQL_TXN_ISOLATION               (108),
	SQL_CURRENT_QUALIFIER           (109),
	SQL_ODBC_CURSORS                (110),
	SQL_QUIET_MODE                  (111),
	SQL_PACKET_SIZE                 (112),

	/* connection attributes with new names */
	//#if (ODBCVER >= 0x0300)
	//SQL_ATTR_ACCESS_MODE		SQL_ACCESS_MODE
	//SQL_ATTR_AUTOCOMMIT			SQL_AUTOCOMMIT
	SQL_ATTR_CONNECTION_TIMEOUT	(113),
	//SQL_ATTR_CURRENT_CATALOG	SQL_CURRENT_QUALIFIER
	SQL_ATTR_DISCONNECT_BEHAVIOR	(114),
	SQL_ATTR_ENLIST_IN_DTC		(1207),
	SQL_ATTR_ENLIST_IN_XA		(1208),
	//SQL_ATTR_LOGIN_TIMEOUT		SQL_LOGIN_TIMEOUT
	//SQL_ATTR_ODBC_CURSORS		SQL_ODBC_CURSORS
	//SQL_ATTR_PACKET_SIZE		SQL_PACKET_SIZE
//	SQL_ATTR_QUIET_MODE			SQL_QUIET_MODE
//	SQL_ATTR_TRACE				SQL_OPT_TRACE
//	SQL_ATTR_TRACEFILE			SQL_OPT_TRACEFILE
//	SQL_ATTR_TRANSLATE_LIB		SQL_TRANSLATE_DLL
//	SQL_ATTR_TRANSLATE_OPTION	SQL_TRANSLATE_OPTION
//	SQL_ATTR_TXN_ISOLATION		SQL_TXN_ISOLATION
//	#endif  /* ODBCVER >= 0x0300 */

	SQL_ATTR_CONNECTION_DEAD	(1209),	/* GetConnectAttr only */

	SQL_ATTR_DRIVER_THREADING	(1028),	/* Driver threading level */

	//#if (ODBCVER >= 0x0351)
	/*	ODBC Driver Manager sets this connection attribute to a unicode driver
		(which supports SQLConnectW) when the application is an ANSI application
		(which calls SQLConnect, SQLDriverConnect, or SQLBrowseConnect).
		This is SetConnectAttr only and application does not set this attribute
		This attribute was introduced because some unicode driver's some APIs may
		need to behave differently on ANSI or Unicode applications. A unicode
		driver, which  has same behavior for both ANSI or Unicode applications,
		should return SQL_ERROR when the driver manager sets this connection
		attribute. When a unicode driver returns SQL_SUCCESS on this attribute,
		the driver manager treates ANSI and Unicode connections differently in
		connection pooling.
	*/
	SQL_ATTR_ANSI_APP			(115),
	//#endif

	//#if (ODBCVER >= 0x0380)
	SQL_ATTR_RESET_CONNECTION   (116),
	SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE    (117);
	//#endif
	
	@Getter
	private int type;
	
	private OdbcConnectionAttribute(int type) {
		this.type = type;
	}
	public static OdbcConnectionAttribute valueOf(int e) {
		for(OdbcConnectionAttribute attr:OdbcConnectionAttribute.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}	
}
