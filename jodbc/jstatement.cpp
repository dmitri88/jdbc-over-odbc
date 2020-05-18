/*
 * jstatement.cpp
 *
 *  Created on: May 15, 2020
 *      Author: dmitri
 */
#include <jni.h>
#include <memory.h>
#include "jstatement.hpp"
#include "jvm.hpp"

JStatement::JStatement(JDatabase* connection) {
	this->connection = connection;
}

RETCODE JStatement::createStatement(){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement)> func1;
	func1 = [](JNIEnv *env,JStatement* statement) {
		jlong val = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "createStatement", "(J)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, val);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::createStatement");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this);
	return ret;
}

RETCODE JStatement::freeStatement(SQLUSMALLINT option)
{
	int ret;
	std::function<int(JNIEnv*,JStatement* statement,SQLUSMALLINT option)> func1;
	func1 = [](JNIEnv *env,JStatement* statement,SQLUSMALLINT option) {
		jlong stmt = (long long)statement;
		jlong opt = (long long)option;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "freeStatement", "(JJ)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt,opt);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::createStatement");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,option);
	return ret;
}

RETCODE JStatement::execDirect(ustring sql){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement,ustring sql)> func1;
	func1 = [](JNIEnv *env,JStatement* statement,ustring sql) {
		jlong stmt = (long long)statement;
		jstring jstr = to_jstring(sql);
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "execDirect", "(JLjava/lang/String;)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt, jstr);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::execDirect\n");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,sql);
	return ret;
}

RETCODE JStatement::getRowCount(SQLINTEGER * retCount){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement, SQLINTEGER * retCount)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLINTEGER * retCount) {
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getRowCount", "(J)J");
		*retCount = env->CallLongMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::getRowCount\n");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,retCount);
	return ret;
}

RETCODE JStatement::getResultColumnCount(SQLSMALLINT * retCount){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement, SQLSMALLINT * retCount)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLSMALLINT * retCount) {
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getResultColumnCount", "(J)I");
		*retCount = env->CallLongMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::getRowCount\n");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,retCount);
	return ret;
}

RETCODE JStatement::describeColumn(SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLUINTEGER * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement, SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLUINTEGER * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLUINTEGER * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable) {
		jobject val;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "describeColumn", "(JI)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,colnum);

		if(colName != NULL) {
			jstring jcolName=(jstring) env->GetObjectArrayElement(data, 0);
			ustring colName2 = ustring(from_jstring(env,jcolName));
			SQLSMALLINT len1  = ((colName2.size()+1)*sizeof(SQLWCHAR));
			if(bufLength<len1){
				//LOG(1,"Error: JStatement::describeColumn buffer is too small %d/%d",bufLength,len1);
				//return SQL_ERROR;
				*nameLength = 0;
			} else {
				*nameLength = colName2.size();
				memcpy(colName,colName2.c_str(),len1);
			}
		} else {
			*nameLength = 0;
		}
		if(dataType != NULL) {
			val = env->GetObjectArrayElement(data, 2);
			*dataType = jinteger_to_jint(env,val);
		}
		if(colSize != NULL) {
			val = env->GetObjectArrayElement(data, 3);
			*colSize = jinteger_to_jint(env,val);
		}
		if(decimalDigits != NULL) {
			val = env->GetObjectArrayElement(data, 3);
			*decimalDigits = jinteger_to_jint(env,val);
		}
		if(nullable != NULL) {
			val = env->GetObjectArrayElement(data, 3);
			*nullable = jinteger_to_jint(env,val);
		}
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::describeColumn\n");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,colnum,colName,bufLength,nameLength,dataType, colSize, decimalDigits, nullable);
	return ret;
}

RETCODE getColumnAttribute_18(JNIEnv* env, jobjectArray data, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue){
	if(rgbDesc != NULL){
		jstring val=(jstring) env->GetObjectArrayElement(data, 0);
		ustring wcharData = ustring(from_jstring(env,val));
		if(wcharData.size()>cbDescMax){
			return SQL_ERROR;
		}
		memcpy(rgbDesc,wcharData.c_str(),(wcharData.size()+1)* sizeof(SQLWCHAR));
		*pcbDesc = wcharData.size()* sizeof(SQLWCHAR);

	}
	if(numberValue!= NULL){
		*numberValue = 0;
	}
	return SQL_SUCCESS;
}

RETCODE getLongFromArrayObject(JNIEnv* env, jobjectArray data,int arrayPos, SQLINTEGER * pointer){
	if(pointer == NULL)
		return SQL_ERROR;
	jobject val=(jobject) env->GetObjectArrayElement(data, arrayPos);
	*pointer = jlong_to_long(env,val);
	return SQL_SUCCESS;
}


RETCODE JStatement::getColumnAttribute(SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement, SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getColumnAttribute", "(JII)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,icol,fDescType);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::getColumnAttribute");
			return SQL_ERROR;
		}
		switch(fDescType){
		case SQL_COLUMN_LABEL://label
			ret = getColumnAttribute_18(env,data,rgbDesc,cbDescMax,pcbDesc, numberValue);
			break;
		case SQL_COLUMN_UPDATABLE:
		case SQL_COLUMN_LENGTH://col size
		case SQL_COLUMN_UNSIGNED://unsigned
			ret = getLongFromArrayObject(env,data,0, numberValue);
			break;
		default:
			ret = SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,icol,fDescType,rgbDesc,cbDescMax,pcbDesc, numberValue);
	return ret;
}


RETCODE JStatement::getStatementAttr(SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax, SQLINTEGER	*stringLength){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement, SQLINTEGER fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax, SQLINTEGER	*stringLength)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLINTEGER fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax, SQLINTEGER	*stringLength) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getStatementAttribute", "(JI)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,fAttribute);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::getStatementAttr");
			return SQL_ERROR;
		}
		switch(fAttribute){
		case SQL_CURSOR_TYPE:
		case SQL_CONCURRENCY:
			ret = getLongFromArrayObject(env,data,0, (SQLINTEGER *)rgbValue);
			break;
		default:
			ret = SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,fAttribute,rgbValue,cbValueMax,stringLength);
	return ret;
}
RETCODE JStatement::setStatementAttr(SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jlong data = 0;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "setStatementAttribute", "(JIJ)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt,fAttribute,data);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::setStatementAttr");
			return SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,fAttribute,rgbValue,cbValueMax);
	return ret;
}

