/*
 * jdescriptor.cpp
 *
 *  Created on: May 21, 2020
 *      Author: dmitri
 */

#include <jni.h>
#include "jstatement.hpp"
#include "jvm.hpp"
#include "unicode.hpp"

RETCODE JStatement::descriptorField(SQLSMALLINT iRecord, SQLSMALLINT iField, PTR rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement,SQLSMALLINT iRecord, SQLSMALLINT iField, PTR rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue)> func1;
	func1 = [](JNIEnv *env,JStatement* statement,SQLSMALLINT iRecord, SQLSMALLINT iField, PTR rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue) {
		jlong stmt = (long long)statement;
		int ret = SQL_SUCCESS;
		jlong longVal;
		ustring ustringVal;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getDescriptorField", "(JII)Ljava/lang/Object;");
		jobject retobj = env->CallObjectMethod(statement->connection->entrypointObj, method, stmt, iRecord,iField);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::descriptorField\n");
			return SQL_ERROR;
		}
		switch(iField){
		case SQL_DESC_COUNT:
			longVal = jlong_to_long(env,retobj);
			*(SQLINTEGER*)rgbValue = longVal;
			break;
		case SQL_DESC_NAME:
			ret |= jstringToString(env, (jstring)retobj, (SQLWCHAR*) rgbValue, cbValueMax,(SQLUINTEGER*)pcbValue);
			break;
		default:
			ret = SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,iRecord,iField,rgbValue,cbValueMax,pcbValue);
	return ret;
}


