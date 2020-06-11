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


RETCODE JStatement::execute(){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement)> func1;
	func1 = [](JNIEnv *env,JStatement* statement) {
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "execute", "(J)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::execute\n");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this);
	return ret;
}

RETCODE JStatement::prepareStatement(ustring sql){
	int ret;
	std::function<int(JNIEnv*,JStatement* statement,ustring sql)> func1;
	func1 = [](JNIEnv *env,JStatement* statement,ustring sql) {
		jlong stmt = (long long)statement;
		jstring jstr = to_jstring(sql);
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "prepareStatement", "(JLjava/lang/String;)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt, jstr);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::prepareStatement\n");
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
		*retCount = env->CallIntMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JStatement::getResultColumnCount\n");
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
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "describeColumn", "(JI)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,colnum);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::describeColumn\n");
			return SQL_ERROR;
		}
		if(colName != NULL) {
			jstring jcolName=(jstring) env->GetObjectArrayElement(data, 0);
			printf("asdasdasd %p\n",jcolName);
			ustring colName2 = ustring(from_jstring(env,jcolName));
			SQLSMALLINT len1  = ((colName2.size()+1)*sizeof(SQLWCHAR));
			if(bufLength<len1){
				//LOG(1,"Error: JStatement::describeColumn buffer is too small %d/%d",bufLength,len1);
				//return SQL_ERROR;
				*nameLength = 0;
			} else {
				*nameLength = colName2.size();
				//memcpy(colName,colName2.c_str(),len1);
				ret = strcpy(colName,bufLength,colName2);
			}
		} else {
			if(nameLength)
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
			val = env->GetObjectArrayElement(data, 4);
			*decimalDigits = jinteger_to_jint(env,val);
		}
		if(nullable != NULL) {
			val = env->GetObjectArrayElement(data, 5);
			*nullable = jinteger_to_jint(env,val);
		}
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::describeColumn\n");
			return SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,colnum,colName,bufLength,nameLength,dataType, colSize, decimalDigits, nullable);
	return ret;
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
		case SQL_COLUMN_UPDATABLE:
			if(pcbDesc != NULL)
				*pcbDesc  =4;
			if(numberValue !=NULL)
				*numberValue = 0;
			break;
		case SQL_COLUMN_DISPLAY_SIZE:
		case SQL_COLUMN_LENGTH://col size
		case SQL_COLUMN_UNSIGNED://unsigned
		case SQL_COLUMN_AUTO_INCREMENT:
			ret = jarrayToLong(env,data,0, numberValue,rgbDesc,4,pcbDesc);
			break;
		case SQL_COLUMN_LABEL:
		case SQL_DESC_BASE_TABLE_NAME:
		case SQL_DESC_BASE_COLUMN_NAME:
		case SQL_DESC_CATALOG_NAME:
		case SQL_DESC_SCHEMA_NAME:
		case SQL_COLUMN_TABLE_NAME:
			ret = jarrayToString(env,data,0,rgbDesc,cbDescMax,pcbDesc, numberValue);
			break;


		case 1212:
			if(pcbDesc != NULL)
				*pcbDesc  =0;
			if(numberValue !=NULL)
				*numberValue = 0;
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
		SQLSMALLINT retType=0;
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::getStatementAttr");
			return SQL_ERROR;
		}
		switch(fAttribute){
		case SQL_CURSOR_TYPE:
		case SQL_CONCURRENCY:
			ret = jarrayToLong(env,data,0,NULL, (SQLPOINTER)rgbValue,cbValueMax,&retType);
			if(stringLength)
				*stringLength = 0;
			break;
		case SQL_ATTR_IMP_PARAM_DESC:
		case SQL_ATTR_APP_PARAM_DESC:
		case SQL_ATTR_APP_ROW_DESC:
			ret = jarrayToLong(env,data,0,NULL, (SQLPOINTER)rgbValue,4,NULL);
			if(stringLength)
				*stringLength = 0;
			*(JStatement**)rgbValue = statement;
			break;
		case SQL_ATTR_IMP_ROW_DESC:
			ret = jarrayToLong(env,data,0,NULL, (SQLPOINTER)rgbValue,cbValueMax,&retType);
			if(stringLength)
				*stringLength = 0;
			*(JStatement**)rgbValue = statement;
			break;
//		case SQL_ATTR_PARAMSET_SIZE:
//			if(stringLength != NULL)
//				*stringLength  =0;
//			break;
		default:
			ret = SQL_SUCCESS;
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
		jlong data = (long)rgbValue;
		jlong reval = cbValueMax;

		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "setStatementAttribute", "(JIJJ)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt,fAttribute,data,reval);
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

RETCODE JStatement::bindColumn(SQLUSMALLINT column, SQLSMALLINT type, PTR value, SQLUINTEGER bufLength, SQLUINTEGER * strLengthOrIndex){
	int ret;

	std::function<int(JNIEnv* env,JStatement* statement, SQLUSMALLINT column, SQLSMALLINT type, PTR value, SQLUINTEGER bufLength, SQLUINTEGER * strLengthOrIndex)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLUSMALLINT column, SQLSMALLINT type, PTR value, SQLUINTEGER bufLength, SQLUINTEGER * strLengthOrIndex) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jlong data = (unsigned long)value;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "bindColumn", "(JIIJJJ)V");
		env->CallVoidMethod(statement->connection->entrypointObj, method, stmt,column,type,(jlong)((SQLUINTEGER)value),(jlong)((SQLINTEGER)(bufLength)),(jlong)((SQLUINTEGER)strLengthOrIndex));
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::bindColumn\n");
			return SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this,column,type,value,bufLength,strLengthOrIndex);
	return ret;
}

RETCODE bindResultToVariable(JNIEnv* env,jobject val, SQLSMALLINT type, PTR valuePtr,SQLUINTEGER bufLength,SQLUINTEGER * strLengthOrIndex){

	if(val == NULL){
		*strLengthOrIndex = SQL_NULL_DATA;
		return SQL_SUCCESS;
	}
	*strLengthOrIndex = 0;

	jlong tt;
	SQLCHAR tt2;
	switch(type){

	case SQL_C_CHAR:// char* string
		jstringToChar(env,(jstring)val, (SQLCHAR*) valuePtr, bufLength,strLengthOrIndex);
		break;
	case SQL_C_SLONG://32 bit signed
		if(bufLength<4){
			LOG(1,"bind variable buffer is too small %lu needed 4",bufLength);
			return SQL_SUCCESS;
		}
		*((SQLINTEGER*)valuePtr)=(SQLINTEGER)jlong_to_long(env, val);
		break;
	case SQL_C_ULONG:
		if(bufLength<4){
			LOG(1,"bind variable buffer is too small %lu needed 4",bufLength);
			return SQL_SUCCESS;
		}
		*((SQLUINTEGER*)valuePtr)=(SQLUINTEGER)jlong_to_long(env, val);
		break;
	default:
		LOG(1,"type not supported %d",type);
		throw std::runtime_error("type not supported");
	}
	LOG(5,"bindResultToVariable %p %li\n",val,*(long *)valuePtr);
	return SQL_SUCCESS;
}

RETCODE JStatement::fetch(){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement)> func1;
	func1 = [](JNIEnv *env,JStatement* statement) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "fetch", "(J)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::fetch");
			return SQL_ERROR;
		}
		jint fetchRet;
		ret = jarrayToInt(env,data,0,&fetchRet);
		if(ret)
			return ret;
		if(!SQL_SUCCEEDED(fetchRet))
			return fetchRet;
		jint paramCount;
		ret = jarrayToInt(env,data,1,&paramCount);
		if(ret)
			return ret;
		if(paramCount == 0)
			return ret;

		for(int i=0;i<paramCount;i++){
			SQLSMALLINT type;
			PTR valuePtr;
			SQLUINTEGER bufLength;
			SQLUINTEGER * strLengthOrIndex;
			jarrayToShort(env, data, 2+5*i, &type);
			jarrayToLong(env, data, 2+5*i+1, (jlong*)&valuePtr);
			jarrayToLong(env, data, 2+5*i+2, (jlong*)&bufLength);
			jarrayToLong(env, data, 2+5*i+3, (jlong*)&strLengthOrIndex);
			jobject val=(jobject) env->GetObjectArrayElement(data, 2+5*i+4);
			ret = bindResultToVariable(env, val, type, valuePtr,bufLength, strLengthOrIndex);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				LOG(1,"Error: JStatement::fetch");
				return SQL_ERROR;
			}
			LOG(5, "fetch param (%d,%d,%p,%li,%p)\n",i,type,valuePtr,bufLength,strLengthOrIndex);
		}
		return ret;
	};
	ret = java_callback(func1,this);
	return ret;
}

RETCODE JStatement::moreResults(){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement)> func1;
	func1 = [](JNIEnv *env,JStatement* statement) {
		int ret = SQL_SUCCESS;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "moreResults", "(J)I");
		ret = (jint)env->CallIntMethod(statement->connection->entrypointObj, method, stmt);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::fetch");
			return SQL_ERROR;
		}
		return ret;
	};
	ret = java_callback(func1,this);
	return ret;
}

RETCODE JStatement::getData(SQLUSMALLINT column, SQLSMALLINT targetType, PTR pointer, SQLLEN bufferLength, SQLLEN *strLenOrInd){
	int ret;

	std::function<int(JNIEnv* env,JStatement* statement, SQLUSMALLINT column, SQLSMALLINT targetType, PTR pointer, SQLLEN bufferLength, SQLLEN *strLenOrInd)> func1;
	func1 = [](JNIEnv *env,JStatement* statement, SQLUSMALLINT column, SQLSMALLINT targetType, PTR pointer, SQLLEN bufferLength, SQLLEN *strLenOrInd) {
		int ret = SQL_SUCCESS;
		jobject val;
		jlong stmt = (long long)statement;
		//jlong data = (unsigned long)value;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getData", "(JII)[Ljava/lang/Object;");
		jobjectArray data = (jobjectArray)env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,column,targetType);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::getData\n");
			return SQL_ERROR;
		}
		jint fetchRet;
		ret = jarrayToInt(env,data,0,&fetchRet);
		if(ret)
			return ret;
		if(!SQL_SUCCEEDED(fetchRet))
			return fetchRet;
		jint paramCount;
		ret = jarrayToInt(env,data,1,&paramCount);
		if(ret)
			return ret;
		if(paramCount == 0)
			return ret;

		for(int i=0;i<paramCount;i++){
			jobject val=(jobject) env->GetObjectArrayElement(data, 2+5*i+4);
			ret = bindResultToVariable(env, val, targetType, pointer,bufferLength, (SQLUINTEGER*)strLenOrInd);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				LOG(1,"Error: JStatement::getData");
				return SQL_ERROR;
			}
			LOG(5, "fetch param (%d,%d,%p,%li,%p)\n",i,targetType,pointer,bufferLength,strLenOrInd);
		}
		return ret;
		return ret;
	};
	ret = java_callback(func1,this,column,targetType,pointer,bufferLength,strLenOrInd);
	return ret;
}

RETCODE JStatement::getDiagField(SQLSMALLINT	iRecord, SQLSMALLINT	fDiagField, SQLPOINTER		rgbDiagInfo, SQLSMALLINT	cbDiagInfoMax, SQLSMALLINT   *pcbDiagInfo){
	int ret;
	std::function<int(JNIEnv* env,JStatement* statement,SQLSMALLINT	iRecord, SQLSMALLINT	fDiagField, SQLPOINTER		rgbDiagInfo, SQLSMALLINT	cbDiagInfoMax, SQLSMALLINT   *pcbDiagInfo)> func1;
	func1 = [](JNIEnv *env,JStatement* statement,SQLSMALLINT	iRecord, SQLSMALLINT	fDiagField, SQLPOINTER		rgbDiagInfo, SQLSMALLINT	cbDiagInfoMax, SQLSMALLINT   *pcbDiagInfo) {
		int ret = SQL_SUCCESS;
		jlong stmt = (long long)statement;
		jmethodID method = env->GetMethodID(statement->connection->entrypointClass, "getDiagFieldStatement", "(JII)Ljava/lang/Object;");
		jobject response = env->CallObjectMethod(statement->connection->entrypointObj, method, stmt,(int)iRecord,(int)fDiagField);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			LOG(1,"Error: JStatement::fetch");
			return SQL_ERROR;
		}
		jlong longVal = jlong_to_long(env,response);
		ret = jlongToLong(longVal,NULL,rgbDiagInfo,4,NULL);
		return ret;
	};
	ret = java_callback(func1,this,iRecord,fDiagField,rgbDiagInfo,cbDiagInfoMax,pcbDiagInfo);
	return ret;

	return SQL_ERROR;
}
