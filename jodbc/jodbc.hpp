#ifndef JODBC_H_
#define JODBC_H_

#include <stdio.h>
#include <sql.h>
#include <sqlspi.h>
#include <stdarg.h>

extern "C" RETCODE SQL_API SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE * OutputHandle);
extern "C" RETCODE SQL_API SQLColumnsW(HSTMT hstm, SQLWCHAR *CatalogName, SQLSMALLINT NameLength1, SQLWCHAR *SchemaName, SQLSMALLINT NameLength2, SQLWCHAR *TableName, SQLSMALLINT NameLength3, SQLWCHAR *ColumnName, SQLSMALLINT NameLength4);
extern "C" RETCODE SQL_API SQLConnectW(HDBC hdbc, SQLWCHAR *ServerName, SQLSMALLINT NameLength1, SQLWCHAR *UserName, SQLSMALLINT NameLength2, SQLWCHAR *Authentication, SQLSMALLINT NameLength3);
extern "C" RETCODE SQL_API SQLDescribeColW(HSTMT hstm, SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLULEN * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable);
extern "C" RETCODE SQL_API SQLDisconnect(HDBC hdbc);
extern "C" RETCODE SQL_API SQLDriverConnectW(HDBC hdbc, HWND hwnd, SQLWCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn, SQLWCHAR *szConnStrOut, SQLSMALLINT cbConnStrOutMax, SQLSMALLINT *pcbConnStrOut, SQLUSMALLINT fDriverCompletion);
extern "C" RETCODE SQL_API SQLExecDirectW(HSTMT hstm, SQLWCHAR * sql, SQLINTEGER length) ;
extern "C" RETCODE SQL_API SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle);
extern "C" RETCODE SQL_API SQLFreeStmt(HSTMT hstm, SQLUSMALLINT Option);
extern "C" RETCODE SQL_API SQLGetConnectAttrW(SQLHDBC hdbc, SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue);
extern "C" RETCODE SQL_API SQLGetDiagRecW(SQLSMALLINT fHandleType, SQLHANDLE	handle, SQLSMALLINT	iRecord, SQLWCHAR	*szSqlState, SQLINTEGER	*pfNativeError, SQLWCHAR	*szErrorMsg, SQLSMALLINT	cbErrorMsgMax, SQLSMALLINT	*pcbErrorMsg);
extern "C" RETCODE SQL_API SQLGetFunctions(HDBC hdbc, SQLUSMALLINT FunctionId, SQLUSMALLINT *Supported);
extern "C" RETCODE SQL_API SQLGetInfoW(HDBC hdbc, SQLUSMALLINT InfoType, PTR InfoValue, SQLSMALLINT BufferLength, SQLSMALLINT *StringLength);
extern "C" RETCODE SQL_API SQLGetTypeInfoW(HSTMT	hstm, SQLSMALLINT	DataType);
extern "C" RETCODE SQL_API SQLNumResultCols(HSTMT hstm, SQLSMALLINT * retCount);
extern "C" RETCODE SQL_API SQLRowCount(HSTMT hstm, SQLLEN * retCount);
extern "C" RETCODE SQL_API SQLSetConnectAttrW(HDBC hdbc, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValue);
extern "C" RETCODE SQL_API SQLTablesW(HSTMT hstm,SQLWCHAR *CatalogName, SQLSMALLINT NameLength1, SQLWCHAR *SchemaName, SQLSMALLINT NameLength2, SQLWCHAR *TableName, SQLSMALLINT NameLength3, SQLWCHAR *TableType, SQLSMALLINT NameLength4);

#endif /* JODBC_H_ */
