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
}

SQLHANDLE create_database(){
	SQLHANDLE hEnv;
	SQLHANDLE hDbc;
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

	short unsigned int *enabledFuncs = (short unsigned int*) malloc(
			sizeof(UWORD) * SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);
	ret = SQLGetFunctions(hDbc, 999, enabledFuncs);
	printf("SQLGetFunctions result %d\n", ret);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode("DSN=Simba;"), 0, NULL,
			0, &retConnStrLength, 0);
	printf("SQLDriverConnectW result %d\n", ret);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_NAME, NULL, 0, &retLen);
	assert(ret == 0);
	assert(retLen == 40);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_NAME, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Microsoft SQL Server"))==0);
	//assert(retLen == 0);

	ret = SQLGetInfoW(hDbc, SQL_DBMS_VER, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"14.00.3238"))==0);

	ret = SQLGetInfoW(hDbc, SQL_DRIVER_NAME, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"msodbcsql17.dl"))==0);

	ret = SQLGetInfoW(hDbc, SQL_OWNER_TERM, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"owner"))==0);

	ret = SQLGetInfoW(hDbc, SQL_NEED_LONG_DATA_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_MULT_RESULT_SETS, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"Y"))==0);

	ret = SQLGetInfoW(hDbc, SQL_IDENTIFIER_QUOTE_CHAR, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"\""))==0);

	ret = SQLGetInfoW(hDbc, SQL_CATALOG_NAME_SEPARATOR, &data, 255, &retLen);
	assert(ret == 0);
	assert(ustring(data).compare(ustring(L"."))==0);

	ret = SQLGetInfoW(hDbc, SQL_POS_OPERATIONS, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==0x1f);
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

	ret = SQLGetInfoW(hDbc, SQL_MAX_CATALOG_NAME_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==128);
	assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_COLUMN_NAME_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==128);
	assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_SCHEMA_NAME_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==128);
	assert(retLen==2);

	ret = SQLGetInfoW(hDbc, SQL_MAX_TABLE_NAME_LEN, &data, 255, &retLen);
	assert(ret == 0);
	assert(*((int*)data)==128);
	assert(retLen==2);

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
	SQLINTEGER retCount;
	SQLHANDLE hStmt;
	char data[1024];
	char retdata[1024];

	ret = SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt);
	assert(ret == 0);

	ret = SQLNativeSqlW(hDbc, (SQLWCHAR *)ustring(L"").c_str(),512, NULL, 0, &retCount);
	assert(ret == 0);

	ustring sql = ustring(L"SELECT COUNT(*) AS RecordCount FROM t_package");
	ret = SQLExecDirectW(hStmt, (SQLWCHAR*)sql.c_str(),sql.size());
	assert(ret == 0);


	SQLBindCol(hStmt, 1, SQL_C_WCHAR, (PTR)data, /*SQLUINTEGER bufLength*/100, /*SQLUINTEGER * strLengthOrIndex*/&retCount);
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
	ret = SQLDescribeColW(hStmt, 1, colName, 1024, &nameLength, &dataType, &colSize, &decimalDigits, &nullable);
	assert(ret == 0);
	assert(nameLength == 22);
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

	ret = SQLColAttributeW(hStmt, 1, 3, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 11);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_UNSIGNED, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_UPDATABLE, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 4);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_COLUMN_NAME, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_BASE_TABLE_NAME, data, 1024, &bufLength, &numberValue);
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
	assert(bufLength == 0);
	assert(numberValue == 0);

	ret = SQLColAttributeW(hStmt, 1, SQL_DESC_AUTO_UNIQUE_VALUE, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(bufLength == 0);
	assert(numberValue == 0);



	// 11 27 1227 1228 0
	ret = SQLSetStmtAttrW(hStmt, 11, data, 1024);
	assert(ret == 0);


	//6 SQL_CURSOR_TYPE  7 SQL_CONCURRENCY
	ret = SQLGetStmtAttrW(hStmt, SQL_CURSOR_TYPE, data, 1024, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);
	assert((*(SQLINTEGER*)data) == 0);
	ret = SQLGetStmtAttrW(hStmt, SQL_CONCURRENCY, data, 1024, &numberValue);
	assert(ret == 0);
	assert(numberValue == 0);
	assert((*(SQLINTEGER*)data) == 1);
//	ret = SQLGetStmtAttrW(hStmt, SQL_DESC_BASE_COLUMN_NAME, data, 258, &numberValue);
//	assert(ret == 0);
//	assert(numberValue == 0);
//	assert((*(SQLINTEGER*)data) == 0);


}

int main(int argc, char **argv) {
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
	test_queries(hDbc);

	printf("\ntesting-done\n");
}
