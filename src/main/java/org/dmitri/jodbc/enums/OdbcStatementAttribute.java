package org.dmitri.jodbc.enums;

import lombok.Getter;

public enum OdbcStatementAttribute {
	SQL_QUERY_TIMEOUT(0),
	SQL_MAX_ROWS			(1),
	SQL_NOSCAN	(2),
	SQL_MAX_LENGTH			(3),
	SQL_ASYNC_ENABLE		(4),	
	SQL_BIND_TYPE			(5),
	SQL_CURSOR_TYPE			(6),
	SQL_CONCURRENCY			(7),
	SQL_KEYSET_SIZE			(8),
	SQL_ROWSET_SIZE			(9),
	SQL_SIMULATE_CURSOR		(10),
	SQL_RETRIEVE_DATA		(11),
	SQL_USE_BOOKMARKS		(12),
	SQL_GET_BOOKMARK		(13),      /*      GetStmtOption Only */
	SQL_ROW_NUMBER			(14) ,     /*      GetStmtOption Only */
	
	/* statement attributes for ODBC 3.0 */
	
	SQL_ATTR_ASYNC_ENABLE				(4),
	//SQL_ATTR_CONCURRENCY				(7), //SQL_CONCURRENCY
	//SQL_ATTR_CURSOR_TYPE				(6), //SQL_CURSOR_TYPE
	SQL_ATTR_ENABLE_AUTO_IPD			(15),
	SQL_ATTR_FETCH_BOOKMARK_PTR			(16),
	//SQL_ATTR_KEYSET_SIZE				(8), //SQL_KEYSET_SIZE
	//SQL_ATTR_MAX_LENGTH					(3), //SQL_MAX_LENGTH
	//SQL_ATTR_MAX_ROWS					(1), //SQL_MAX_ROWS
	//SQL_ATTR_NOSCAN						(2), //SQL_NOSCAN
	SQL_ATTR_PARAM_BIND_OFFSET_PTR		(17),
	SQL_ATTR_PARAM_BIND_TYPE			(18),
	SQL_ATTR_PARAM_OPERATION_PTR		(19),
	SQL_ATTR_PARAM_STATUS_PTR			(20),
	SQL_ATTR_PARAMS_PROCESSED_PTR		(21),
	SQL_ATTR_PARAMSET_SIZE				(22),
	//SQL_ATTR_QUERY_TIMEOUT				(0), //SQL_QUERY_TIMEOUT
	//SQL_ATTR_RETRIEVE_DATA				(11),//SQL_RETRIEVE_DATA
	SQL_ATTR_ROW_BIND_OFFSET_PTR		(23),
	//SQL_ATTR_ROW_BIND_TYPE				(5), //SQL_BIND_TYPE
	//SQL_ATTR_ROW_NUMBER					(14) ,  //SQL_ROW_NUMBER	  	/*GetStmtAttr*/
	SQL_ATTR_ROW_OPERATION_PTR			(24),
	SQL_ATTR_ROW_STATUS_PTR				(25),
	SQL_ATTR_ROWS_FETCHED_PTR			(26),
	SQL_ATTR_ROW_ARRAY_SIZE				(27),	
	//SQL_ATTR_SIMULATE_CURSOR			(10), //SQL_SIMULATE_CURSOR
	//SQL_ATTR_USE_BOOKMARKS				(12), //SQL_USE_BOOKMARKS	


	//if (ODBCVER >= 0x0380)
	SQL_ATTR_ASYNC_STMT_EVENT           (29);
	//endif /* ODBCVER >= 0x0380 */

	// (ODBCVER < 0x0300)
	//SQL_STMT_OPT_MAX               (14) , //SQL_ROW_NUMBER
	//SQL_STMT_OPT_MIN	(0), //SQL_QUERY_TIMEOUT
	
	
	
	@Getter
	private int type;
	
	private OdbcStatementAttribute(int type) {
		this.type = type;
	}
	public static OdbcStatementAttribute valueOf(int e) {
		for(OdbcStatementAttribute attr:OdbcStatementAttribute.values()) {
			if(attr.getType() == e) {
				return attr;
			}
		}
		return null;
	}
}
