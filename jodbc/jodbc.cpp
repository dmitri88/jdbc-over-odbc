/*
 * jodbc.cpp
 *
 *  Created on: May 11, 2020
 *      Author: dmitri
 */

#include "jodbc.hpp"
#include "jdatabase.hpp"
#include "jstatement.hpp"
#include "unicode.hpp"

#include <sqltypes.h>
#include <cstring>

#include <jni.h>
#include <sqlext.h>
#include "jvm.hpp"
#include "log.hpp"





RETCODE SQL_API SQLAllocHandle(SQLSMALLINT handleType, SQLHANDLE parentHandle, SQLHANDLE * outputHandle){
	RETCODE ret = SQL_SUCCESS;
	LOG(5, "Entering SQLAllocHandle %hi %p\n",handleType,parentHandle);
		switch (handleType)
		{
			case SQL_HANDLE_ENV:
				*outputHandle = start_vm();
				break;
			case SQL_HANDLE_DBC:
				*outputHandle = new JDatabase((JavaVM*)parentHandle);
				((JDatabase*)( *outputHandle))->preInitConnection();
				break;
			case SQL_HANDLE_STMT:
				*outputHandle = new JStatement((JDatabase*)parentHandle);
				ret =((JStatement*)( *outputHandle))->createStatement();
				if(ret){
					delete ((JStatement*)( *outputHandle));
					*outputHandle = NULL;
				}
				break;
				/*

			case SQL_HANDLE_DESC:
				conn = (ConnectionClass *) InputHandle;
				CC_examine_global_transaction(conn);
				ENTER_CONN_CS(conn);
				ret = PGAPI_AllocDesc(InputHandle, OutputHandle);
				LEAVE_CONN_CS(conn);
	MYLOG(DETAIL_LOG_LEVEL, "OutputHandle=%p\n", *OutputHandle);
				break;
				*/
			default:

				ret = SQL_ERROR;
				break;
		}
	LOG(5, "Exiting SQLAllocHandle (%hi) %hi %p %p %p\n",ret,handleType,parentHandle,outputHandle,*outputHandle);
	return ret;
}

RETCODE SQL_API SQLColAttributeW(HSTMT hstmt,SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLLEN *pfDesc){
	int ret;
	LOG(5, "Entering SQLColAttributeW (%p,%d,%d)\n",hstmt,icol,fDescType);
	if(hstmt == NULL )
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstmt;
	ret = stmt->getColumnAttribute(icol,fDescType,rgbDesc,cbDescMax,pcbDesc,pfDesc);
	LOG(5, "Exiting SQLColAttributeW %d (%p,%d,%d,%d,%d)\n",ret,hstmt,icol,fDescType,POINTER_VAL(pcbDesc),POINTER_VAL(pfDesc));
	return ret;
}

RETCODE  SQL_API
SQLColumnsW(HSTMT StatementHandle,
			SQLWCHAR *CatalogName, SQLSMALLINT NameLength1,
			SQLWCHAR *SchemaName, SQLSMALLINT NameLength2,
			SQLWCHAR *TableName, SQLSMALLINT NameLength3,
			SQLWCHAR *ColumnName, SQLSMALLINT NameLength4)
{
	LOG(5, "Entering SQLColumnsW\n");
	return SQL_ERROR;
}

RETCODE		SQL_API
SQLConnectW(HDBC ConnectionHandle,
		   SQLWCHAR *ServerName, SQLSMALLINT NameLength1,
		   SQLWCHAR *UserName, SQLSMALLINT NameLength2,
		   SQLWCHAR *Authentication, SQLSMALLINT NameLength3){
	LOG(5, "Entering SQLConnect (%p,%s,%s,%s)\n",ConnectionHandle,unicode_to_utf8(ServerName),unicode_to_utf8(UserName),unicode_to_utf8(Authentication));
	return SQL_SUCCESS;

}

RETCODE SQL_API SQLDescribeColW(HSTMT hstm, SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength,
		SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLUINTEGER * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable){
	RETCODE	ret;

	LOG(5, "Entering SQLDescribeColW (%p,%d,%d)\n",hstm,colnum,bufLength);
	if(!hstm)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstm;

	ret = stmt->describeColumn(colnum, colName, bufLength, nameLength, dataType, colSize, decimalDigits , nullable);

	LOG(5, "Exiting SQLDescribeColW %d (%p,%d,%d,%s,%d,%d,%lu,%d,%d)\n",ret,hstm,colnum,bufLength,unicode_to_utf8(colName),*nameLength,*dataType,*colSize,*decimalDigits,*nullable);
	return ret;
}

RETCODE	SQL_API SQLDisconnect(HDBC ConnectionHandle){
	LOG(5, "Entering SQLDisconnect %p\n",ConnectionHandle);
	return SQL_SUCCESS;
}

RETCODE SQL_API SQLDriverConnectW(HDBC hdbc, HWND hwnd, SQLWCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn, SQLWCHAR *szConnStrOut, SQLSMALLINT cbConnStrOutMax, SQLSMALLINT *pcbConnStrOut, SQLUSMALLINT fDriverCompletion)
{
	RETCODE	ret;
	JDatabase	*db = (JDatabase *) hdbc;

	LOG(5, "Entering SQLDriverConnectW (%p,%p,%s,%i,%i)\n",hdbc,hwnd,unicode_to_utf8(szConnStrIn),cbConnStrIn,fDriverCompletion);
	if(pcbConnStrOut!=NULL)
		*pcbConnStrOut = 0;
	ret = db->connect(szConnStrIn, cbConnStrIn);
	if(ret)
		return ret;
	LOG(5, "Exiting SQLDriverConnectW %d:(%p,%p,%s,%i,%i)\n",ret,hdbc,hwnd,unicode_to_utf8(szConnStrIn),cbConnStrIn,fDriverCompletion);
	return ret;
}

RETCODE  SQL_API SQLExecDirectW(HSTMT handleStatement, SQLWCHAR * sql, SQLINTEGER length){
	//char buf[length*sizeof(SQLWCHAR)+4];
	int ret;
	LOG(5, "Entering SQLExecDirectW (%p,%s)\n",handleStatement,unicode_to_utf8(sql));
	if(handleStatement == NULL )
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)handleStatement;
	ret = stmt->execDirect(ustring(sql));
	LOG(5, "Exiting SQLExecDirectW %d (%p,%s)\n",ret,handleStatement,unicode_to_utf8(sql));
	return ret;
}

RETCODE		SQL_API SQLFreeHandle(SQLSMALLINT handleType, SQLHANDLE handle) {
	LOG(5, "Entering SQLFreeHandle (%hi,%p)\n",handleType,handle);
	return SQL_SUCCESS;
}

RETCODE		SQL_API SQLFreeStmt(HSTMT handleStatement, SQLUSMALLINT option)
{
	int ret;
	LOG(5, "Entering SQLFreeStmt (%p,%hi)\n",handleStatement,option);
	if(handleStatement == NULL )
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)handleStatement;
	ret = stmt->freeStatement(option);
	LOG(5, "Exiting SQLFreeStmt %d (%p,%hi)\n",ret,handleStatement,option);
	return ret;
}


#define SQL_FUNC_ESET(pfExists, uwAPI) (*(((UWORD*) (pfExists)) + ((uwAPI) >> 4)) |= (1 << ((uwAPI) & 0x000F)) )
RETCODE SQL_API SQLGetFunctions(HDBC ConnectionHandle, SQLUSMALLINT FunctionId, SQLUSMALLINT *pfExists)
{
	LOG(5, "Entering GetFunctions (%p,%hi)\n",ConnectionHandle,FunctionId);
	////ConnectionClass	*conn = (ConnectionClass *) hdbc;
	////ConnInfo	*ci = &(conn->connInfo);


	////CC_examine_global_transaction(conn);
	////CC_clear_error(conn);
	if (FunctionId != SQL_API_ODBC3_ALL_FUNCTIONS)
		return SQL_ERROR;
	memset(pfExists, 0, sizeof(UWORD) * SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);


	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLALLOCCONNECT); 1 deprecated */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLALLOCENV); 2 deprecated */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLALLOCSTMT); 3 deprecated */

	/*
	 * for (i = SQL_API_SQLBINDCOL; i <= 23; i++) SQL_FUNC_ESET(pfExists,
	 * i);
	 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLBINDCOL);		/* 4 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCANCEL);		/* 5 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCOLATTRIBUTE);	/* 6 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCONNECT);		/* 7 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLDESCRIBECOL);	/* 8 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLDISCONNECT);		/* 9 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLERROR);  10 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLEXECDIRECT);		/* 11 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLEXECUTE);		/* 12 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLFETCH);	/* 13 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLFREECONNECT); 14 deprecated */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLFREEENV); 15 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLFREESTMT);		/* 16 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETCURSORNAME);	/* 17 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLNUMRESULTCOLS);	/* 18 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPREPARE);		/* 19 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLROWCOUNT);		/* 20 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETCURSORNAME);	/* 21 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLSETPARAM); 22 deprecated */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLTRANSACT); 23 deprecated */

	/*
	 * for (i = 40; i < SQL_API_SQLEXTENDEDFETCH; i++)
	 * SQL_FUNC_ESET(pfExists, i);
	 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCOLUMNS);		/* 40 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLDRIVERCONNECT);	/* 41 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLGETCONNECTOPTION); 42 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETDATA);		/* 43 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETFUNCTIONS);	/* 44 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETINFO);		/* 45 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLGETSTMTOPTION); 46 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETTYPEINFO);	/* 47 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPARAMDATA);		/* 48 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPUTDATA);		/* 49 */

	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLSETCONNECTIONOPTION); 50 deprecated */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLSETSTMTOPTION); 51 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSPECIALCOLUMNS);	/* 52 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSTATISTICS);		/* 53 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLTABLES);		/* 54 */
	//if (ci->drivers.lie)
		SQL_FUNC_ESET(pfExists, SQL_API_SQLBROWSECONNECT);	/* 55 */
	//if (ci->drivers.lie)
		SQL_FUNC_ESET(pfExists, SQL_API_SQLCOLUMNPRIVILEGES);	/* 56 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLDATASOURCES);	/* 57 */
	//if (SUPPORT_DESCRIBE_PARAM(ci) || ci->drivers.lie)
		SQL_FUNC_ESET(pfExists, SQL_API_SQLDESCRIBEPARAM); /* 58 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLEXTENDEDFETCH); /* 59 deprecated ? */

	/*
	 * for (++i; i < SQL_API_SQLBINDPARAMETER; i++)
	 * SQL_FUNC_ESET(pfExists, i);
	 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLFOREIGNKEYS);	/* 60 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLMORERESULTS);	/* 61 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLNATIVESQL);		/* 62 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLNUMPARAMS);		/* 63 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLPARAMOPTIONS); 64 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPRIMARYKEYS);	/* 65 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPROCEDURECOLUMNS);	/* 66 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLPROCEDURES);		/* 67 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETPOS);		/* 68 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLSETSCROLLOPTIONS); 69 deprecated */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLTABLEPRIVILEGES);	/* 70 */
	/* SQL_FUNC_ESET(pfExists, SQL_API_SQLDRIVERS); */	/* 71 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLBINDPARAMETER);	/* 72 */

	SQL_FUNC_ESET(pfExists, SQL_API_SQLALLOCHANDLE);	/* 1001 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLBINDPARAM);		/* 1002 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCLOSECURSOR);	/* 1003 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLCOPYDESC);		/* 1004 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLENDTRAN);		/* 1005 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLFREEHANDLE);		/* 1006 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETCONNECTATTR);	/* 1007 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETDESCFIELD);	/* 1008 */
	//if (ci->drivers.lie)
	{
		SQL_FUNC_ESET(pfExists, SQL_API_SQLGETDESCREC); /* 1009 not implemented yet */
	}
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETDIAGFIELD); /* 1010 minimal implementation */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETDIAGREC);		/* 1011 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETENVATTR);		/* 1012 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLGETSTMTATTR);	/* 1014 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETCONNECTATTR);	/* 1016 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETDESCFIELD);	/* 1017 */
	//if (ci->drivers.lie)
	{
		SQL_FUNC_ESET(pfExists, SQL_API_SQLSETDESCREC); /* 1018 not implemented yet */
	}
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETENVATTR);		/* 1019 */
	SQL_FUNC_ESET(pfExists, SQL_API_SQLSETSTMTATTR);	/* 1020 */
	//SQL_FUNC_ESET(pfExists, SQL_API_SQLFETCHSCROLL);	/* 1021 */
	//if (0 != (ALLOW_BULK_OPERATIONS & ci->updatable_cursors))
	SQL_FUNC_ESET(pfExists, SQL_API_SQLBULKOPERATIONS);	/* 24 */


	LOG(5, "Exiting GetFunctions 0 (%p,%hi)\n",ConnectionHandle,FunctionId);

	return SQL_SUCCESS;
}



RETCODE SQL_API
SQLGetDiagRecW(SQLSMALLINT fHandleType,
			   SQLHANDLE	handle,
			   SQLSMALLINT	iRecord,
			   SQLWCHAR	*szSqlState,
			   SQLINTEGER	*pfNativeError,
			   SQLWCHAR	*szErrorMsg,
			   SQLSMALLINT	cbErrorMsgMax,
			   SQLSMALLINT	*pcbErrorMsg) {
	LOG(5, "Entering SQLGetDiagRecW %hi,%p,%hi\n",fHandleType,handle,iRecord);
	return SQL_SUCCESS;
}

RETCODE  SQL_API SQLGetInfoW(HDBC hdbc,SQLUSMALLINT fInfoType, PTR rgbInfoValue, SQLSMALLINT cbInfoValueMax, SQLSMALLINT * pcbInfoValue){
	char		data[1000];
	const char   *p = NULL;
	SQLUINTEGER			len = 0;
	SQLUINTEGER			value = 0;
	RETCODE		ret = SQL_ERROR;

	LOG(5, "Entering SQLGetInfoW %p,%hhi,%p,%hi\n",hdbc,fInfoType,rgbInfoValue,cbInfoValueMax);
	if(!hdbc)
		return SQL_INVALID_HANDLE;
	JDatabase* database = (JDatabase*)hdbc;

	//try java version
	ret = database->getInfo(fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue);
	if(ret == SQL_SUCCESS){
		LOG(5, "Exiting SQLGetInfoW %d (%p,%hhi,%p,%hi,%d)\n",ret,hdbc,fInfoType,rgbInfoValue,cbInfoValueMax,POINTER_VAL(pcbInfoValue));
		return ret;
	}
	//local fallback
	switch(fInfoType){
		case SQL_POS_OPERATIONS:		/* ODBC 2.0 */
			len = 4;
			value = (SQL_POS_POSITION | SQL_POS_REFRESH);
			//if (0 != ci->updatable_cursors)
				value |= (SQL_POS_UPDATE | SQL_POS_DELETE | SQL_POS_ADD);
			break;
		case SQL_STATIC_SENSITIVITY:	/* ODBC 2.0 */
			len = 4;
			value = 0;
			//if (0 != ci->updatable_cursors)
				value |= (SQL_SS_ADDITIONS | SQL_SS_DELETIONS | SQL_SS_UPDATES);
			break;
		case SQL_BOOKMARK_PERSISTENCE:	/* ODBC 2.0 */
			/* very simple bookmark support */
			len = 4;
			value = SQL_BP_SCROLL | SQL_BP_DELETE | SQL_BP_UPDATE | SQL_BP_TRANSACTION;
			break;

		case SQL_SCROLL_OPTIONS:		/* ODBC 1.0 */
			len = 4;
			value = SQL_SO_FORWARD_ONLY | SQL_SO_STATIC;
			//if (0 != (ci->updatable_cursors & ALLOW_KEYSET_DRIVEN_CURSORS))
				value |= SQL_SO_KEYSET_DRIVEN;
			//if (ci->drivers.lie)
				value |= (SQL_SO_DYNAMIC | SQL_SO_MIXED);
			break;
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
			len = 4;
			value = 0;
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES1:
			len = 4;
			value = SQL_CA1_NEXT | SQL_CA1_ABSOLUTE
				| SQL_CA1_RELATIVE | SQL_CA1_BOOKMARK
				| SQL_CA1_LOCK_NO_CHANGE | SQL_CA1_POS_POSITION
				| SQL_CA1_POS_REFRESH;
			//if (0 != (ci->updatable_cursors & ALLOW_KEYSET_DRIVEN_CURSORS))
				value |= (SQL_CA1_POS_UPDATE | SQL_CA1_POS_DELETE
				| SQL_CA1_BULK_ADD
				| SQL_CA1_BULK_UPDATE_BY_BOOKMARK
				| SQL_CA1_BULK_DELETE_BY_BOOKMARK
				| SQL_CA1_BULK_FETCH_BY_BOOKMARK
				);
			//if (ci->drivers.lie)
				value |= (SQL_CA1_LOCK_EXCLUSIVE
				| SQL_CA1_LOCK_UNLOCK
				| SQL_CA1_POSITIONED_UPDATE
				| SQL_CA1_POSITIONED_DELETE
				| SQL_CA1_SELECT_FOR_UPDATE
				);
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES1:
			len = 4;
			value = SQL_CA1_NEXT | SQL_CA1_ABSOLUTE
				| SQL_CA1_RELATIVE | SQL_CA1_BOOKMARK
				| SQL_CA1_LOCK_NO_CHANGE | SQL_CA1_POS_POSITION
				| SQL_CA1_POS_REFRESH;
			//if (0 != (ci->updatable_cursors & ALLOW_STATIC_CURSORS))
				value |= (SQL_CA1_POS_UPDATE | SQL_CA1_POS_DELETE
				);
			//if (0 != (ci->updatable_cursors & ALLOW_BULK_OPERATIONS))
				value |= (SQL_CA1_BULK_ADD
				| SQL_CA1_BULK_UPDATE_BY_BOOKMARK
				| SQL_CA1_BULK_DELETE_BY_BOOKMARK
				| SQL_CA1_BULK_FETCH_BY_BOOKMARK
				);
			break;
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
			len = 4;
			value = SQL_CA1_NEXT; /* others aren't allowed in ODBC spec */
			break;
		case SQL_KEYSET_CURSOR_ATTRIBUTES2:
			len = 4;
			value = SQL_CA2_READ_ONLY_CONCURRENCY;
			//if (0 != (ci->updatable_cursors & ALLOW_KEYSET_DRIVEN_CURSORS))
				value |= (SQL_CA2_OPT_ROWVER_CONCURRENCY
				/*| SQL_CA2_CRC_APPROXIMATE*/
				);
			//if (0 != (ci->updatable_cursors & SENSE_SELF_OPERATIONS))
				value |= (SQL_CA2_SENSITIVITY_DELETIONS
				| SQL_CA2_SENSITIVITY_UPDATES
				| SQL_CA2_SENSITIVITY_ADDITIONS
				);
			//if (!ci->drivers.use_declarefetch || ci->drivers.lie)
				value |= SQL_CA2_CRC_EXACT;
			//if (ci->drivers.lie)
				value |= (SQL_CA2_LOCK_CONCURRENCY
				| SQL_CA2_OPT_VALUES_CONCURRENCY
				| SQL_CA2_MAX_ROWS_SELECT
				| SQL_CA2_MAX_ROWS_INSERT
				| SQL_CA2_MAX_ROWS_DELETE
				| SQL_CA2_MAX_ROWS_UPDATE
				| SQL_CA2_MAX_ROWS_CATALOG
				| SQL_CA2_MAX_ROWS_AFFECTS_ALL
				| SQL_CA2_SIMULATE_NON_UNIQUE
				| SQL_CA2_SIMULATE_TRY_UNIQUE
				| SQL_CA2_SIMULATE_UNIQUE
				);
			break;
		case SQL_STATIC_CURSOR_ATTRIBUTES2:
			len = 4;
			value = SQL_CA2_READ_ONLY_CONCURRENCY;
			//if (0 != (ci->updatable_cursors & ALLOW_STATIC_CURSORS))
				value |= (SQL_CA2_OPT_ROWVER_CONCURRENCY
				);
			//if (0 != (ci->updatable_cursors & SENSE_SELF_OPERATIONS))
				value |= (SQL_CA2_SENSITIVITY_DELETIONS
				| SQL_CA2_SENSITIVITY_UPDATES
				| SQL_CA2_SENSITIVITY_ADDITIONS
				);
			//if (!ci->drivers.use_declarefetch || ci->drivers.lie)
				value |= (SQL_CA2_CRC_EXACT
				);
			break;
		case SQL_CURSOR_ROLLBACK_BEHAVIOR:		/* ODBC 1.0 */
			len = 2;
//			if (!ci->drivers.use_declarefetch)
				value = SQL_CB_PRESERVE;
//			else
//				value = SQL_CB_CLOSE;
			break;

		case SQL_ACTIVE_CONNECTIONS:	/* ODBC 1.0 */
			len = 2;
			value = 0;
			break;

		case SQL_ACTIVE_STATEMENTS:		/* ODBC 1.0 */
			len = 2;
			value = 0;
			break;
	}


		/* numeric data */
		if (rgbInfoValue)
		{
			if (len == sizeof(SQLSMALLINT))
				*((SQLUSMALLINT *) rgbInfoValue) = (SQLUSMALLINT) value;
			else if (len == sizeof(SQLINTEGER))
				*((SQLUINTEGER *) rgbInfoValue) = (SQLUINTEGER) value;
		}
	LOG(5, "Exiting SQLGetInfoW %d (%p,%hhi,%p,%hi)\n",ret,hdbc,fInfoType,rgbInfoValue,cbInfoValueMax);
	return SQL_SUCCESS;
}

RETCODE SQL_API SQLGetConnectAttrW(
    SQLHDBC            hdbc,
    SQLINTEGER         fAttribute,
    SQLPOINTER         rgbValue,
	SQLINTEGER         cbValueMax,
	SQLINTEGER     *pcbValue)
{
	RETCODE	ret;

	LOG(5, "Entering SQLGetConnectAttrW (%p,%li,%p,%li)\n",hdbc,fAttribute,rgbValue,cbValueMax);
	if(!hdbc)
		return SQL_INVALID_HANDLE;
	JDatabase* conn = (JDatabase*)hdbc;

	ret = conn->getConnectionAttr(fAttribute,rgbValue,(SQLUINTEGER)cbValueMax,(SQLUINTEGER*)pcbValue);

	LOG(5, "Exiting SQLGetConnectAttrW %d (%p,%li,%p,%li,%d)\n",ret,hdbc,fAttribute,rgbValue,cbValueMax,POINTER_VAL(pcbValue));
	return ret;

}

RETCODE SQL_API
SQLSetConnectAttrW(HDBC hdbc,
				   SQLINTEGER	fAttribute,
				   PTR		rgbValue,
				   SQLINTEGER	cbValue)
{
	RETCODE	ret;
	LOG(5, "Entering SQLSetConnectAttrW (%p,%li,%p,%li)\n",hdbc,fAttribute,rgbValue,cbValue);
	if(!hdbc)
		return SQL_INVALID_HANDLE;
	JDatabase* conn = (JDatabase*)hdbc;

	ret = conn->setConnectionAttr(fAttribute,rgbValue,cbValue);

	LOG(5, "Exiting SQLSetConnectAttrW %d (%p,%li,%p,%li)\n",ret,hdbc,fAttribute,rgbValue,cbValue);
	return ret;
}

RETCODE SQL_API SQLGetStmtAttrW(SQLHSTMT hstmt, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax, SQLINTEGER	*numValue){
	RETCODE	ret;

	LOG(5, "Entering SQLGetStmtAttrW (%p,%li,%p,%li,%p)\n",hstmt,fAttribute,rgbValue,cbValueMax,numValue);
	if(!hstmt)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstmt;

	ret = stmt->getStatementAttr(fAttribute,rgbValue,cbValueMax,numValue);

	LOG(5, "Exiting SQLGetStmtAttrW %d (%p,%li,%p,%li,%p)\n",ret,hstmt,fAttribute,rgbValue,cbValueMax,numValue);
	return ret;
}

RETCODE SQL_API SQLSetStmtAttrW(SQLHSTMT hstmt, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax){
	RETCODE	ret;
	LOG(5, "Entering SQLSetStmtAttrW (%p,%li,%p,%li)\n",hstmt,fAttribute,rgbValue,cbValueMax);
	if(!hstmt)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstmt;

	ret = stmt->setStatementAttr(fAttribute,rgbValue,cbValueMax);

	LOG(5, "Exiting SQLSetStmtAttrW %d (%p,%li,%p,%li)\n",ret,hstmt,fAttribute,rgbValue,cbValueMax);
	return ret;
}


RETCODE		SQL_API
SQLTablesW(HSTMT StatementHandle,
		SQLWCHAR *CatalogName, SQLSMALLINT NameLength1,
		SQLWCHAR *SchemaName, SQLSMALLINT NameLength2,
		SQLWCHAR *TableName, SQLSMALLINT NameLength3,
		SQLWCHAR *TableType, SQLSMALLINT NameLength4)
{
	LOG(5, "Entering SQLTables\n");
	return SQL_ERROR;
}

RETCODE	SQL_API SQLRowCount(HSTMT stmtHandle, SQLLEN * retCount){
	RETCODE	ret;

	LOG(5, "Entering SQLRowCount (%p)\n",stmtHandle);
	if(!stmtHandle)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)stmtHandle;

	ret = stmt->getRowCount(retCount);

	LOG(5, "Exiting SQLRowCount %d (%p,%li)\n",ret,stmt,*retCount);
	return ret;
}

RETCODE SQL_API SQLNumResultCols(HSTMT stmtHandle, SQLSMALLINT * retCount){
	RETCODE	ret;

	LOG(5, "Entering SQLNumResultCols (%p)\n",stmtHandle);
	if(!stmtHandle)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)stmtHandle;

	ret = stmt->getResultColumnCount(retCount);

	LOG(5, "Exiting SQLNumResultCols %d (%p,%i)\n",ret,stmt,*retCount);
	return ret;
}

RETCODE SQL_API
SQLGetTypeInfoW(SQLHSTMT	stmt,
				SQLSMALLINT	dataType)
{
	LOG(5, "Entering SQLGetTypeInfoW\n");
	return SQL_ERROR;
}

RETCODE SQL_API SQLNativeSqlW(HDBC hdbc,SQLWCHAR *szSqlStrIn,SQLINTEGER	cbSqlStrIn,SQLWCHAR *szSqlStr,SQLINTEGER	cbSqlStrMax,SQLINTEGER   *pcbSqlStr){
	RETCODE	ret;

	LOG(5, "Entering SQLNativeSqlW (%s,%li,%p,%li,%p)\n",unicode_to_utf8(szSqlStrIn),cbSqlStrIn,szSqlStr,cbSqlStrMax,pcbSqlStr);
	if(!hdbc)
		return SQL_INVALID_HANDLE;
	JDatabase* conn = (JDatabase*)hdbc;

	ustring str(szSqlStrIn,cbSqlStrIn);
	ret = conn->getNativeSql(str,szSqlStr,(SQLUINTEGER)cbSqlStrMax,(SQLUINTEGER*)pcbSqlStr);

	LOG(5, "Exiting SQLNativeSqlW %d (%s,%li,%s,%li,%d)\n",ret,unicode_to_utf8(szSqlStrIn),cbSqlStrIn,unicode_to_utf8(szSqlStr),cbSqlStrMax,POINTER_VAL(pcbSqlStr));
	return ret;
}

RETCODE	SQL_API SQLBindCol(HSTMT hstmt, SQLUSMALLINT column, SQLSMALLINT type, PTR value, SQLINTEGER bufLength, SQLINTEGER * strLengthOrIndex){
	RETCODE	ret;

	LOG(5, "Entering SQLBindCol (%d,%d,%p,%li,%p)\n",column,type,value,bufLength,strLengthOrIndex);
	if(!hstmt)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstmt;

	ret = stmt->bindColumn(column,type,value,(SQLUINTEGER)bufLength,(SQLUINTEGER*)strLengthOrIndex);

	LOG(5, "Exiting SQLBindCol %d (%d,%d,%p,%li,%d)\n",ret,column,type,value,bufLength,POINTER_VAL(strLengthOrIndex));
	return ret;
}
RETCODE SQL_API SQLFetch(HSTMT hstmt) {
	RETCODE	ret;

	LOG(5, "Entering SQLFetch (%p)\n",hstmt);
	if(!hstmt)
		return SQL_INVALID_HANDLE;
	JStatement* stmt = (JStatement*)hstmt;

	ret = stmt->fetch();

	LOG(5, "Exiting SQLFetch %d (%p)\n",ret,hstmt);
	return ret;
}
