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
	printf("javavm result: %p\n", jvm);

	std::function<int(JNIEnv*)> func1;
	func1 = [](JNIEnv *env) {
		return 7;
	};
	ret = java_callback(func1);
	printf("java_callback result %d\n", ret);
	assert(ret == 7);

	std::function<int(JNIEnv*, int)> func2;
	func2 = [](JNIEnv *env, int a) {
		return 17;
	};
	ret = java_callback(func2, 14);
	printf("java_callback result %d\n", ret);
	assert(ret == 17);

	std::function<int(JNIEnv*, int, char *str)> func3;
	func3 = [](JNIEnv *env, int a, char *str) {
		return 19;
	};
	ret = java_callback(func3, 15, (char*) "asd");
	printf("java_callback result %d\n", ret);
	assert(ret == 19);

	std::function<int(JNIEnv*)> func4;
	func4 = [](JNIEnv *env) {
		int ret=0;
		jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
		return ret;
	};
	ret = java_callback(func4);
	printf("java_callback result %d\n", ret);
	assert(ret == 0);
}

void test_attributes(SQLHANDLE hDbc, SQLHANDLE hStmt){
	int ret;
	//SQLGetConnectAttrW(SQLHDBC hdbc, SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue);
	//SQLSetConnectAttrW(HDBC hdbc, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValue);

	ret = SQLSetConnectAttrW(hDbc, 0, NULL, -6);
	assert(ret == 0);

	ret = SQLGetConnectAttrW(hDbc, 0, NULL, 514, NULL);
	assert(ret == 0);
}

SQLHANDLE create_database(){
	SQLHANDLE hEnv;
	SQLHANDLE hDbc;
	int ret;

	SQLWCHAR data[200];
	short int retConnStrLength;

	ret = SQLAllocHandle( SQL_HANDLE_ENV, 0, &hEnv);
	printf("SQLAllocHandle result %d %p\n", ret, hEnv);
	ret = SQLAllocHandle( SQL_HANDLE_ENV, 0, &hEnv);
	printf("SQLAllocHandle result %d %p\n", ret, hEnv);

	ret = SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc);
	printf("SQLAllocHandle result %d %p\n", ret, hDbc);

	ret = SQLGetInfoW(NULL, SQL_ODBC_VER, &data, 255, NULL);
	printf("SQLGetInfoW SQL_ODBC_VER result %d %s\n", ret,
			unicode_to_utf8(data));
	ret = SQLGetInfoW(NULL, SQL_DRIVER_ODBC_VER, &data, 255, NULL);
	printf("SQLGetInfoW SQL_DRIVER_ODBC_VER result %d %s\n", ret,
			unicode_to_utf8(data));

	printf("\n");

	short unsigned int *enabledFuncs = (short unsigned int*) malloc(
			sizeof(UWORD) * SQL_API_ODBC3_ALL_FUNCTIONS_SIZE);
	ret = SQLGetFunctions(hDbc, 999, enabledFuncs);
	printf("SQLGetFunctions result %d\n", ret);

	ret = SQLDriverConnectW(hDbc, NULL, utf8_to_unicode("DSN=Simba;"), 0, NULL,
			0, &retConnStrLength, 0);
	printf("SQLDriverConnectW result %d\n", ret);

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

void test_queries(SQLHANDLE hStmt){
	int ret;
	SQLLEN retCount;
	SQLSMALLINT colCount;

	ustring sql = ustring(L"select count(*) from usys_system");
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
	assert(dataType == 4);

	char data[1024];
	SQLINTEGER numberValue;
	ret = SQLColAttributeW(hStmt, 1, 18, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);

	ret = SQLColAttributeW(hStmt, 1, 3, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);
	assert(numberValue == 11);

	ret = SQLColAttributeW(hStmt, 1, 8, data, 1024, &bufLength, &numberValue);
	assert(ret == 0);


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

	test_attributes(hDbc,hStmt);
	test_queries(hStmt);

	printf("\ntesting-done\n");
}
