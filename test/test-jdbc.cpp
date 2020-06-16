/*
 * test-jdbc.cpp
 *
 *  Created on: May 7, 2020
 *      Author: dmitri
 */
#include <sqlext.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <assert.h>
#include "test-jdbc.h"

void test_java_callback() {
	int ret;

	JavaVM *jvm = start_vm();

	std::function<int(JNIEnv*)> func1;
	func1 = [](JNIEnv *env) {
		return 7;
	};
	ret = java_callback(func1);
	assert(ret == 7);

	std::function<int(JNIEnv*, int)> func2;
	func2 = [](JNIEnv *env, int a) {
		return 17;
	};
	ret = java_callback(func2, 14);
	assert(ret == 17);

	std::function<int(JNIEnv*, int, char *str)> func3;
	func3 = [](JNIEnv *env, int a, char *str) {
		return 19;
	};
	ret = java_callback(func3, 15, (char*) "asd");
	assert(ret == 19);

	std::function<int(JNIEnv*)> func4;
	func4 = [](JNIEnv *env) {
		int ret=0;
		jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
		return ret;
	};
	ret = java_callback(func4);
	assert(ret == 0);
}

void test_attributes2(SQLHANDLE hDbc, SQLHANDLE hStmt){
	int ret;
	char data[1024];
	SQLINTEGER size;
	//SQLGetConnectAttrW(SQLHDBC hdbc, SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue);
	//SQLSetConnectAttrW(HDBC hdbc, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValue);

	ret = SQLSetConnectAttrW(hDbc, 0, NULL, -6);
	assert(ret == 0);

	ret = SQLGetConnectAttrW(hDbc, 0, NULL, 514, NULL);
	assert(ret == 0);

	ret = SQLGetConnectAttrW(hDbc, SQL_ATTR_CURRENT_CATALOG, data, 1024, &size);
	assert(ret == 0);
	assert(ustring((SQLWCHAR*)data).compare(ustring(L"DEV_EnterpriseArchitect"))==0);

	ret = SQLGetConnectAttrW(hDbc, 1202, data, SQL_IS_UINTEGER,0);
	assert(ret == 0);

	ret = SQLGetConnectAttrW(hDbc, 1217, data, 4,&size);
	assert(ret == 0);
	assert(size == 4);


	ret = SQLSetConnectAttrW(hDbc, SQL_ATTR_MAX_ROWS, NULL, -6);
	assert(ret == 0);

	ret = SQLSetConnectAttrW(hDbc, SQL_ATTR_QUERY_TIMEOUT, NULL, -6);
	assert(ret == 0);

	ret = SQLSetConnectAttrW(hDbc, SQL_ROWSET_SIZE, (PTR)1, -6);
	assert(ret == 0);

	ret = SQLSetConnectAttrW(hDbc, SQL_ATTR_ROW_ARRAY_SIZE, (PTR)1, -6);
	assert(ret == 0);

	ret = SQLGetConnectAttrW(hDbc, SQL_ATTR_TXN_ISOLATION, data, 4,0);
	assert(ret == 0);
	assert(*((short*)data)==2);//SQL_TXN_READ_COMMITTED>

	ret = SQLParamOptions(hStmt, 1, NULL);
	assert(ret == 0);

}

SQLHANDLE create_database(){
	SQLHANDLE hEnv;
	SQLHANDLE hDbc;
	HSTMT  hstmt;
	int ret;

	SQLWCHAR data[200];
	short int retConnStrLength;
	SQLSMALLINT retLen;

	ret = SQLAllocHandle( SQL_HANDLE_ENV, 0, &hEnv);
	printf("SQLAllocHandle result %d %p\n", ret, hEnv);
	ret = SQLAllocHandle( SQL_HANDLE_ENV, 0, &hEnv);
	printf("SQLAllocHandle result %d %p\n", ret, hEnv);

	ret = SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc);
	printf("SQLAllocHandle result %d %p\n", ret, hDbc);

	ret = SQLGetInfoW(hDbc, SQL_ODBC_VER, &data, 255, NULL);
	assert(ustring(data).compare(ustring(L"03.80.0000"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DRIVER_ODBC_VER, &data, 255, NULL);
	assert(ustring(data).compare(ustring(L"03.80"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DATA_SOURCE_READ_ONLY, &data, 255, NULL);
	assert(ustring(data).compare(ustring(L"N"))==0);

	ret = SQLGetInfoW(hDbc, SQL_IDENTIFIER_QUOTE_CHAR, &data, 255, NULL);
	assert(ustring(data).compare(ustring(L"\""))==0);


	ret = SQLGetInfoW(hDbc, SQL_ODBC_API_CONFORMANCE, &data, 255, NULL);
	assert(ret == 0);
	assert(*((short*)data)==2);

	ret = SQLGetInfoW(hDbc, SQL_ODBC_SQL_CONFORMANCE, &data, 255, NULL);
	assert(ret == 0);
	assert(*((short*)data)==SQL_OSC_CORE);





	short unsigned int *enabledFuncs = (short unsigned int*) malloc(
			sizeof(UWORD) * SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);
	ret = SQLGetFunctions(hDbc, 999, enabledFuncs);
	printf("SQLGetFunctions result %d %d \n", ret,enabledFuncs);
	//assert(enabledFuncs == 0);
	/*
	Out:			Function                                                    SQL_FUNC_EXISTS(SupportedPtr,API Constant)
								SQL_API_SQLALLOCCONNECT = 1                                 SQL_TRUE
								SQL_API_SQLALLOCENV = 2                                     SQL_TRUE
								SQL_API_SQLALLOCSTMT = 3                                    SQL_TRUE
								SQL_API_SQLBINDCOL = 4                                      SQL_TRUE
								SQL_API_SQLCANCEL = 5                                       SQL_TRUE
								SQL_API_SQLCOLATTRIBUTES = 6                                SQL_TRUE
								SQL_API_SQLCONNECT = 7                                      SQL_TRUE
								SQL_API_SQLDESCRIBECOL = 8                                  SQL_TRUE
								SQL_API_SQLDISCONNECT = 9                                   SQL_TRUE
								SQL_API_SQLERROR = 10                                       SQL_TRUE
								SQL_API_SQLEXECDIRECT = 11                                  SQL_TRUE
								SQL_API_SQLEXECUTE = 12                                     SQL_TRUE
								SQL_API_SQLFETCH = 13                                       SQL_TRUE
								SQL_API_SQLFREECONNECT = 14                                 SQL_TRUE
								SQL_API_SQLFREEENV = 15                                     SQL_TRUE
								SQL_API_SQLFREESTMT = 16                                    SQL_TRUE
								SQL_API_SQLGETCURSORNAME = 17                               SQL_TRUE
								SQL_API_SQLNUMRESULTCOLS = 18                               SQL_TRUE
								SQL_API_SQLPREPARE = 19                                     SQL_TRUE
								SQL_API_SQLROWCOUNT = 20                                    SQL_TRUE
								SQL_API_SQLSETCURSORNAME = 21                               SQL_TRUE
								SQL_API_SQLSETPARAM = 22                                    SQL_TRUE
								SQL_API_SQLTRANSACT = 23                                    SQL_TRUE
								SQL_API_SQLBULKOPERATIONS = 24                              SQL_TRUE
								SQL_API_SQLCOLUMNS = 40                                     SQL_TRUE
								SQL_API_SQLDRIVERCONNECT = 41                               SQL_TRUE
								SQL_API_SQLGETCONNECTOPTION = 42                            SQL_TRUE
								SQL_API_SQLGETDATA = 43                                     SQL_TRUE
								SQL_API_SQLGETFUNCTIONS = 44                                SQL_TRUE
								SQL_API_SQLGETINFO = 45                                     SQL_TRUE
								SQL_API_SQLGETSTMTOPTION = 46                               SQL_TRUE
								SQL_API_SQLGETTYPEINFO = 47                                 SQL_TRUE
								SQL_API_SQLPARAMDATA = 48                                   SQL_TRUE
								SQL_API_SQLPUTDATA = 49                                     SQL_TRUE
								SQL_API_SQLSETCONNECTOPTION = 50                            SQL_TRUE
								SQL_API_SQLSETSTMTOPTION = 51                               SQL_TRUE
								SQL_API_SQLSPECIALCOLUMNS = 52                              SQL_TRUE
								SQL_API_SQLSTATISTICS = 53                                  SQL_TRUE
								SQL_API_SQLTABLES = 54                                      SQL_TRUE
								SQL_API_SQLBROWSECONNECT = 55                               SQL_TRUE
								SQL_API_SQLCOLUMNPRIVILEGES = 56                            SQL_TRUE
								SQL_API_SQLDATASOURCES = 57                                 SQL_TRUE
								SQL_API_SQLDESCRIBEPARAM = 58                               SQL_TRUE
								SQL_API_SQLEXTENDEDFETCH = 59                               SQL_TRUE
								SQL_API_SQLFOREIGNKEYS = 60                                 SQL_TRUE
								SQL_API_SQLMORERESULTS = 61                                 SQL_TRUE
								SQL_API_SQLNATIVESQL = 62                                   SQL_TRUE
								SQL_API_SQLNUMPARAMS = 63                                   SQL_TRUE
								SQL_API_SQLPARAMOPTIONS = 64                                SQL_TRUE
								SQL_API_SQLPRIMARYKEYS = 65                                 SQL_TRUE
								SQL_API_SQLPROCEDURECOLUMNS = 66                            SQL_TRUE
								SQL_API_SQLPROCEDURES = 67                                  SQL_TRUE
								SQL_API_SQLSETPOS = 68                                      SQL_TRUE
								SQL_API_SQLSETSCROLLOPTIONS = 69                            SQL_TRUE
								SQL_API_SQLTABLEPRIVILEGES = 70                             SQL_TRUE
								SQL_API_SQLDRIVERS = 71                                     SQL_TRUE
								SQL_API_SQLBINDPARAMETER = 72                               SQL_TRUE
								SQL_API_SQLALLOCHANDLE = 1001                               SQL_TRUE
								SQL_API_SQLBINDPARAM = 1002                                 SQL_TRUE
								SQL_API_SQLCLOSECURSOR = 1003                               SQL_TRUE
								SQL_API_SQLCOLATTRIBUTE = 6                                 SQL_TRUE
								SQL_API_SQLCOPYDESC = 1004                                  SQL_TRUE
								SQL_API_SQLENDTRAN = 1005                                   SQL_TRUE
								SQL_API_SQLFETCHSCROLL = 1021                               SQL_TRUE
								SQL_API_SQLFREEHANDLE = 1006                                SQL_TRUE
								SQL_API_SQLGETCONNECTATTR = 1007                            SQL_TRUE
								SQL_API_SQLGETDESCFIELD = 1008                              SQL_TRUE
								SQL_API_SQLGETDESCREC = 1009                                SQL_TRUE
								SQL_API_SQLGETDIAGFIELD = 1010                              SQL_TRUE
								SQL_API_SQLGETDIAGREC = 1011                                SQL_TRUE
								SQL_API_SQLGETENVATTR = 1012                                SQL_TRUE
								SQL_API_SQLGETSTMTATTR = 1014                               SQL_TRUE
								SQL_API_SQLSETCONNECTATTR = 1016                            SQL_TRUE
								SQL_API_SQLSETDESCFIELD = 1017                              SQL_TRUE
								SQL_API_SQLSETDESCREC = 1018                                SQL_TRUE
								SQL_API_SQLSETENVATTR = 1019                                SQL_TRUE
								SQL_API_SQLSETSTMTATTR = 1020                               SQL_TRUE


	*/




	ustring dsn = ustring(L"Simba");
	ret = SQLConnectW(hDbc,(SQLWCHAR*)dsn.c_str(),dsn.size(),NULL,0,NULL,0);
	assert(ret == 0);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode("DSN=Simba;"), 0, NULL,
			0, &retConnStrLength, 0);
	assert(ret == 0);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode("DSN=Simba;"), 0, data, 1024, &retConnStrLength, 0);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"DSN=Simba;"))==0);
	assert(retConnStrLength == 10);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode("DSN=Simba"), 0, data, 1024, &retConnStrLength, 0);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"DSN=Simba;"))==0);
	assert(retConnStrLength == 10);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode(""), 0, NULL,
			0, &retConnStrLength, 0);
	assert(ret == 0);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_NAME, NULL, 0, &retLen);
	assert(ret == 0);
	assert(retLen == 40);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_NAME, &data, 42, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Microsoft SQL Server"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_NAME, &data, 42, NULL);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Microsoft SQL Server"))==0);
	//assert(retLen == 0);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_VER, NULL, 0, &retLen);
	assert(ret == 0);
	assert(retLen == 20);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_VER, &data, 22, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"14.00.3238"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DRIVER_NAME, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"msodbcsql17.dl"))==0);

	ret = SQLGetInfoW(hDbc, SQL_OWNER_TERM, &data, 12, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"owner"))==0);

	ret = SQLGetInfoW(hDbc, SQL_OWNER_TERM, NULL, 0, &retLen);
	assert(ret == 0);
	assert(retLen == 10);
	//assert(ustring(data).compare(ustring(L"owner"))==0);

	ret = SQLGetInfoW(hDbc, SQL_NEED_LONG_DATA_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_MULT_RESULT_SETS, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_MULT_RESULT_SETS, &data,4, NULL);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_IDENTIFIER_QUOTE_CHAR, NULL, 255, &retLen);
	assert(ret == 0);
	assert(retLen == 2);

	ret = SQLGetInfoW(hDbc, SQL_IDENTIFIER_QUOTE_CHAR, &data, 4, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"\""))==0);

	ret = SQLGetInfoW(hDbc, SQL_CATALOG_NAME_SEPARATOR, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"."))==0);

	ret = SQLGetInfoW(hDbc, SQL_POS_OPERATIONS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x1f);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_POSITIONED_STATEMENTS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0);
	assert(retLen==4);



	ret = SQLGetInfoW(hDbc, SQL_STATIC_SENSITIVITY, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==5);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_LOCK_TYPES, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==1);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_GETDATA_EXTENSIONS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==4);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_TXN_ISOLATION_OPTION, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x2f);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_BOOKMARK_PERSISTENCE, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x52);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_SCROLL_OPTIONS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x17);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_SCROLL_CONCURRENCY, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0xf);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_DYNAMIC_CURSOR_ATTRIBUTES1, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x0001FE47);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_KEYSET_CURSOR_ATTRIBUTES1, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x000FFE4F);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_STATIC_CURSOR_ATTRIBUTES1, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x0008124F);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x0000E001);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_KEYSET_CURSOR_ATTRIBUTES2, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x00011FDF);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_STATIC_CURSOR_ATTRIBUTES2, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x00001F81);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_NEED_LONG_DATA_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DATABASE_NAME, NULL, 0, &retLen);
	assert(ret == 0);
	assert(retLen==46);




	ret = SQLGetInfoW(hDbc, SQL_CURSOR_COMMIT_BEHAVIOR, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==1);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_CURSOR_ROLLBACK_BEHAVIOR, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==1);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_TXN_CAPABLE, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==2);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_MAX_CATALOG_NAME_LEN, &data, 4, NULL);
	assert(ret == 0);
	assert(*((short*)data)==128);
	//assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_COLUMN_NAME_LEN, &data, 2, NULL);
	assert(ret == 0);
	assert(*((short*)data)==128);
	//assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_SCHEMA_NAME_LEN, &data, 2, NULL);
	assert(ret == 0);
	assert(*((short*)data)==128);
	//assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_TABLE_NAME_LEN, &data, 2, NULL);
	assert(ret == 0);
	assert(*((short*)data)==128);
	//assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_ACTIVE_STATEMENTS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==1);
	assert(retLen==4);

	ret = SQLGetInfoW(hDbc, SQL_DEFAULT_TXN_ISOLATION, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==2);
	assert(retLen==4);


	return hDbc;
}

SQLHANDLE create_statement(SQLHANDLE hDbc){
	int ret;
	SQLHANDLE hStmt;

	ret = SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt);
	printf("SQLAllocHandle result %d %p\n", ret, hStmt);

	ret = SQLFreeStmt(hStmt, 0);
	assert(ret == 0);

	return hStmt;
}

void test_queries(SQLHANDLE hDbc){
	int ret;
	SQLINTEGER valInt;
	SQLINTEGER retCount;
	SQLHANDLE hStmt;
	SQLHANDLE hStmt2;
	SQLSMALLINT smallInt;
	char data[1024];
	char retdata[1024];

	ret = SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt);
	assert(ret == 0);

	ret = SQLNativeSqlW(hDbc, (SQLWCHAR *)ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package").c_str(),SQL_VARBINARY, NULL, 0, &retCount);
	assert(ret == 0);
	assert(retCount == 45);

	ret = SQLNativeSqlW(hDbc, (SQLWCHAR *)ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package").c_str(),SQL_VARBINARY, (SQLWCHAR *)data, 46, NULL);
	assert(ret == 0);
	assert(ustring((SQLWCHAR *)data).compare(ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package"))==0);


	ustring sql = ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package");
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLExecute(hStmt);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_ARRAY_SIZE, (PTR)0x1, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ROWSET_SIZE, (PTR)0x2, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLGetStmtAttrW(hStmt, SQL_ROWSET_SIZE, data, SQL_IS_UINTEGER, NULL);
	assert(ret == 0);
	assert(*(SQLUINTEGER*)data == 2);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_BIND_TYPE, (PTR)0x88, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_BIND_OFFSET_PTR, (PTR)data, SQL_IS_POINTER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_BIND_OFFSET_PTR, (PTR)0, SQL_IS_POINTER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_RETRIEVE_DATA, (PTR)0, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_RETRIEVE_DATA, (PTR)1, SQL_IS_INTEGER);
	assert(ret == 0);


	retCount = 0;
	valInt = 0;
	ret = SQLBindCol(hStmt, 1, SQL_C_SLONG, (PTR)&valInt, /*SQLUINTEGER bufLength*/4, /*SQLUINTEGER * strLengthOrIndex*/&retCount);
	assert(ret == 0);
	assert(valInt == 0);

	ret = SQLFetch(hStmt); //valInt is a result of prev bindCol
	assert(ret == 0);
	assert(valInt == 3);


	ret = SQLGetData(hStmt, 1, SQL_C_SLONG, (PTR)&valInt, /*SQLUINTEGER bufLength*/4, /*SQLUINTEGER * strLengthOrIndex*/&retCount);
	assert(ret == 0);
	assert(valInt == 3);

	ret = SQLGetData(hStmt, 1, SQL_C_CHAR, (PTR)data, /*SQLUINTEGER bufLength*/1024, /*SQLUINTEGER * strLengthOrIndex*/&retCount);
	assert(ret == 0);
	assert(retCount == 1);
	assert(std::string(data).compare("3")==0);

	ret = SQLFreeStmt(hStmt, SQL_UNBIND);
	assert(ret == 0);

	ret = SQLMoreResults(hStmt);
	assert(ret == SQL_NO_DATA);

	ret = SQLGetDiagFieldW(SQL_HANDLE_STMT,hStmt,0,SQL_DIAG_CURSOR_ROW_COUNT,(SQLPOINTER)data,1024,&smallInt);
	assert(ret == 0);

	sql = ustring(L"SELECT * FROM t_package");
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLNumResultCols (hStmt, &smallInt);
	assert(ret == 0);
	assert(smallInt == 23);

}
void test_queries_row_binding(SQLHANDLE hDbc){
	int ret;
	SQLINTEGER valInt;
	SQLINTEGER retCount;
	SQLHANDLE hStmt;
	SQLSMALLINT smallInt;
	char data[1024];
	char retdata[1024];

	SQLULEN rows;
	SQLUSMALLINT statuses[200];

	ret = SQLAllocStmt(hDbc, &hStmt);
	assert(ret == 0);

	ustring sql = ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package");
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLExecute(hStmt);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_BIND_OFFSET_PTR, (PTR)data, SQL_IS_POINTER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ROWSET_SIZE, (PTR)1, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLBindCol(hStmt, 1, SQL_C_SLONG, (PTR)0x4, /*SQLUINTEGER bufLength*/4, /*SQLUINTEGER * strLengthOrIndex*/(SQLINTEGER *)(0x8));
	assert(ret == 0);

	rows=-1;
	statuses[0]=-1;
	ret = SQLExtendedFetch(hStmt, SQL_FETCH_NEXT, /*irow*/0, &rows, statuses);
	assert(ret == 0);
	assert(rows == 1);
	assert(statuses[0] == SQL_ROW_SUCCESS);
	assert(*(SQLINTEGER*)(data+0x4) == 3);

	ret = SQLExtendedFetch(hStmt, SQL_FETCH_NEXT, /*irow*/0, &rows, statuses);
	assert(ret == 100);

}

void test_imp(SQLHANDLE hDbc){
	int ret;
	SQLINTEGER valInt;
	SQLINTEGER retCount;
	SQLHANDLE hStmt;
	SQLHANDLE ird;
	char data[1024];
	char retdata[1024];

	ret = SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt);
	assert(ret == 0);

	ret = SQLExecDirectW(hStmt, (SQLWCHAR*)ustring(L"select * from t_package").c_str(), -1);
	assert(ret == 0);

	/* get the ird descriptors */
	ret = SQLGetStmtAttrW(hStmt, SQL_ATTR_APP_PARAM_DESC, &ird, 0, NULL);
	assert(ret == 0);
	assert(ird == hStmt);

	ret = SQLGetStmtAttrW(hStmt, SQL_ATTR_IMP_PARAM_DESC, &ird, 0, NULL);
	assert(ret == 0);
	assert(ird == hStmt);

	/* get the ird descriptors */
	ret = SQLGetStmtAttrW(hStmt, SQL_ATTR_IMP_ROW_DESC, &ird, SQL_IS_POINTER, NULL);
	assert(ret == 0);
	assert((long)ird > 99);
	assert(ird == hStmt);

	/* get the number of columns */
	ret = SQLGetDescFieldW(ird, 0, SQL_DESC_COUNT, &retCount, 0, NULL);
	assert(ret == 0);
	assert(retCount ==23);

	ret = SQLGetDescFieldW(ird, 1, SQL_DESC_NAME, data, 512, &retCount);
	assert(ret == 0);
	assert(retCount == 20);
	assert(ustring((SQLWCHAR*)data).compare(ustring(L"Package_ID"))==0);

	ret = SQLGetDescFieldW(ird, 1, SQL_DESC_TYPE_NAME, data, 512, &retCount);
	assert(ret == 0);
	assert(retCount == 6);
	assert(ustring((SQLWCHAR*)data).compare(ustring(L"int"))==0);

	ret = SQLGetDescFieldW(ird, 1, SQL_DESC_PRECISION, &retCount,0, NULL);
	assert(ret == 0);
	assert(retCount == 10);

	ret = SQLGetDescFieldW(ird, 1, SQL_DESC_SCALE, &retCount,0, NULL);
	assert(ret == 0);
	assert(retCount == 0);

	ret = SQLGetDescFieldW(ird, 1, SQL_DESC_LENGTH, &retCount,0, NULL);
	assert(ret == 0);
	assert(retCount == 10);

	//for (int index = 1; index <= retCount; index++) {
	//	printf("asdasdas\n");
		//ret = SQLGetDescFieldW(ird, index, SQL_DESC_NAME, colName, 512, NULL);
		///assert(ret == 0);
//		SQLGetDescField(ird, index, SQL_DESC_TYPE_NAME, typeName, LEN, NULL);
//		SQLGetDescField(ird, index, SQL_DESC_PRECISION, &prec, 0, NULL);
//
//		SQLGetDescField(ird, index, SQL_DESC_SCALE, &scale, 0, NULL);
//		SQLGetDescField(ird, index, SQL_DESC_LENGTH, &length, 0, NULL);
//	printf(“Column No : %d, Name : %s, Type : %s, Length : %d,
//			Precision : %d, Scale : %d \n”, index, colName, typeName, len, length, prec,
//			scale);
//}

}



void test_attributes_3(SQLHANDLE hStmt){
	int ret;
	SQLLEN retCount;
	SQLSMALLINT colCount;

	ustring sql = ustring(L"SELECT *  FROM t_package");
	ret = SQLExecDirectW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLRowCount(hStmt, &retCount);
	assert(ret == 0);
	assert(retCount >2);
	ret = SQLNumResultCols(hStmt, &colCount);
	assert(ret == 0);
	assert(colCount == 23);

	SQLWCHAR colName[1024];
	SQLSMALLINT bufLength;
	SQLSMALLINT nameLength;
	SQLSMALLINT dataType;
	SQLULEN colSize;
	SQLSMALLINT decimalDigits;
	SQLSMALLINT nullable;
	ret = SQLDescribeColW(hStmt, 1, colName, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(nameLength == 10);
	assert(dataType == SQL_INTEGER);
	assert(colSize == 10);
	assert(decimalDigits == 0);
	assert(nullable == 0);

	ret = SQLDescribeColW(hStmt, 2, colName, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(nameLength == 4);
	assert(dataType == SQL_WVARCHAR);
	assert(colSize == 255);
	assert(decimalDigits == 0);
	assert(nullable == 1);

	ret = SQLDescribeColW(hStmt, 6, NULL, 0, NULL, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(dataType == SQL_WVARCHAR);
	assert(colSize == 0);
	assert(decimalDigits == 0);
	assert(nullable == 1);

	ret = SQLDescribeColW(hStmt, 9, NULL, 0, NULL, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(dataType == SQL_INTEGER);
	assert(colSize == 10);
	assert(decimalDigits == 0);
	assert(nullable == 0);

	ret = SQLDescribeColW(hStmt, 11, NULL, 0, NULL, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(dataType == SQL_TYPE_TIMESTAMP);//SQL_TIMESTAMP
	assert(colSize == 23);
	assert(decimalDigits == 3);
	assert(nullable == 1);


	sql = ustring(L"SELECT * FROM t_package");
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLDescribeColW(hStmt, 1, colName, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(nameLength == 10);
	assert(dataType == 4);
	assert(colSize == 10);
	assert(decimalDigits == 0);
	assert(nullable == 0);


}

void test_attributes(SQLHANDLE hStmt){
	int ret;
	SQLLEN retCount;
	SQLSMALLINT colCount;

	ustring sql = ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package");
	ret = SQLExecDirectW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);

	ret = SQLRowCount(hStmt, &retCount);
	assert(ret == 0);
	assert(retCount == 1);
	ret = SQLNumResultCols(hStmt, &colCount);
	assert(ret == 0);
	assert(colCount == 1);

	SQLWCHAR colName[1024];
	SQLSMALLINT bufLength;
	SQLSMALLINT nameLength;
	SQLSMALLINT dataType;
	SQLULEN colSize;
	SQLSMALLINT decimalDigits;
	SQLSMALLINT nullable;

	ret = SQLDescribeColW(hStmt, 1, NULL, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(dataType == 4);
	assert(colSize == 10);
	assert(decimalDigits == 0);
	assert(nullable == 1);

	ret = SQLDescribeColW(hStmt, 1, colName, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(nameLength == 11);
	assert(dataType == 4);
	assert(colSize == 10);
	assert(decimalDigits == 0);
	assert(nullable == 1);


	char data[1024];
	SQLINTEGER numberValue;
	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_LABEL, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 22);
	assert(ustring((SQLWCHAR*)data).compare(ustring(L"RecordCount"))==0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_LABEL, data, 1024, NULL, NULL);
	assert(ret == 0);
	assert(bufLength == 22);
	assert(ustring((SQLWCHAR*)data).compare(ustring(L"RecordCount"))==0);


	ret = SQLColAttributeW(hStmt, 1, SQL_COLUMN_DISPLAY_SIZE, data, 1024, &bufLength, NULL);
	assert(ret == 0);
	assert(bufLength == SQL_IS_INTEGER);
	assert(*(SQLINTEGER*)data == 11);

	ret = SQLColAttributeW(hStmt, 1, SQL_COLUMN_DISPLAY_SIZE, NULL, 0, NULL, &numberValue);
	assert(ret == 0);
	assert(numberValue == 11);

	ret = SQLColAttributeW(hStmt, 1, SQL_COLUMN_LENGTH, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 10);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_UNSIGNED, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 1);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_UPDATABLE, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 4);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_COLUMN_NAME, data, 258, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_COLUMN_NAME, data, 258, &bufLength, NULL);
	assert(ret == 0);
	assert(bufLength == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_TABLE_NAME, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_TABLE_NAME, data, 1024, &bufLength, NULL);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_CATALOG_NAME, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_SCHEMA_NAME, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, 1212, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, 1212, NULL,0,NULL, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_COLUMN_TABLE_NAME, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_AUTO_UNIQUE_VALUE, NULL, 0, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_AUTO_UNIQUE_VALUE, NULL, 0, NULL, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);



	// 11 27 1227 1228 0
	ret = SQLSetStmtAttrW(hStmt, 11, data, 1024);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_PARAM_BIND_TYPE, (PTR)10, 0);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_PARAM_BIND_TYPE, (PTR)0, 0);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_ROW_ARRAY_SIZE, (PTR)0x1, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_QUERY_TIMEOUT, (PTR)0x12c, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_PARAM_BIND_OFFSET_PTR, data, 0);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_ATTR_PARAM_BIND_OFFSET_PTR, NULL, 0);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_SOPT_SS_HIDDEN_COLUMNS, NULL, SQL_IS_INTEGER);
	assert(ret == 0);

	ret = SQLSetStmtAttrW(hStmt, SQL_SOPT_SS_NOBROWSETABLE, NULL, SQL_IS_INTEGER);
	assert(ret == 0);


	//ustring sql = ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package");
	//ret = SQLExecDirectW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	//assert(ret == 0);

	ret = SQLSetDescFieldW(hStmt, 1,SQL_DESC_OCTET_LENGTH_PTR, &numberValue, 0);
	assert(ret == 0);

	ret = SQLSetDescFieldW(hStmt, 1,SQL_DESC_OCTET_LENGTH_PTR, NULL, 0);
	assert(ret == 0);



	//6 SQL_CURSOR_TYPE  7 SQL_CONCURRENCY
	ret = SQLGetStmtAttrW(hStmt, SQL_CURSOR_TYPE, data, SQL_IS_SMALLINT, NULL);
	assert(ret == 0);
	assert(numberValue == 0);
	assert((*(SQLSMALLINT*)data) == 0);

	ret = SQLGetStmtAttrW(hStmt, SQL_CONCURRENCY, data, SQL_IS_INTEGER, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);
	assert((*(SQLINTEGER*)data) == 1);



	//ret = SQLGetStmtAttrW(hStmt, SQL_DESC_BASE_COLUMN_NAME, data, 258, &numberValue);
	//assert(ret == 0);
	//assert(numberValue == 0);
	//assert((*(SQLWCHAR*)data) == 0);


}

int main(int argc, char **argv) {
	printf(" ");
	test_java_callback();

	int ret;
	SQLHANDLE hDbc;
	SQLHANDLE hStmt;

	//printf("\n%s ", unicode_to_utf8(utf8_to_unicode("testing")));
	//printf("%s\n", unicode_to_utf8(utf8_to_unicode("DSN=Simba;")));

	hDbc = create_database();
	hStmt = create_statement(hDbc);

	test_attributes2(hDbc,hStmt);
	test_attributes(hStmt);
	test_attributes_3(hStmt);
	test_queries(hDbc);
	test_queries_row_binding(hDbc);
	test_imp(hDbc);

	printf("\ntesting-done\n");
}
