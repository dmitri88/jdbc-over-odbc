/*
 * jstatement.cpp
 *
 *  Created on: May 15, 2020
 *      Author: dmitri
 */
#include <jni.h>
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
			LOG(1,"Error: JStatement::execDirect");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,sql);
	return ret;
}

RETCODE JStatement::getRowCount(SQLLEN * retCount){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement, SQLLEN * retCount)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLLEN * retCount) {
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getRowCount", "(J)J");
		*retCount = env->CallLongMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::getRowCount");
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
			LOG(1,"Error: JStatement::getRowCount");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,retCount);
	return ret;
}
