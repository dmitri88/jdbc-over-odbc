/*
 * jvm.cpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */

#include <thread>
#include <future>
#include <string.h>
#include <sqlext.h>
#include <odbcinst.h>
#include "jodbc.hpp"
#include "jvm.hpp"
#include "unicode.hpp"



using namespace std;


JavaVM* globalJvm=NULL;
JavaVM* create_vm() {
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVM *jvm;
    JavaVMOption options[10];
    int opcnt = 0 ;
    if(globalJvm!=NULL){
    	return globalJvm;
    }
    std::string utf8CP("");

    SQLWCHAR buffer[4096];
	auto ret1 = SQLGetPrivateProfileStringW(
			ustring(L"ODBC").c_str(),
			ustring(L"ClassPath").c_str(),ustring(L".").c_str(), buffer, sizeof(buffer),
			ustring(L"odbc.ini").c_str());
	if (ret1 > 0) {
		ustring val(buffer, ret1);
		utf8CP=val.utf8();
	}

    options[opcnt++].optionString = (char*) "-Xcheck:jni" ;
    //options[opcnt++].optionString = (char*) "-Xdebug" ;
    //options[opcnt++].optionString = (char*) "-Xrunjdwp:transport=dt_socket,address=5005,server=y,suspend=y" ;

    options[opcnt++].optionString = (char*) "-Djava.class.path=/app/tools/mssql-jdbc/mssql-jdbc-8.2.2.jre8.jar:/app/git/tools/jdbc-over-odbc/build/libs/jdbc-over-odbc-0.0.1.jar" ;



    //options[opcnt++].optionString = (char*) "-Xdebug" ;
    //options[opcnt++].optionString = "-Xms256m";
    //options[opcnt++].optionString = "-Xmx512m";
    //strcpy(optionString, "-XX:+AllowUserSignalHandlers");

    //Path to the java source code
    vm_args.version = JNI_VERSION_10;
    vm_args.nOptions = opcnt;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = 0;

    LOG(2,"\ncreate_vm JVM starting\n");
    JNI_GetDefaultJavaVMInitArgs(&vm_args);
    int ret = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    if(ret < 0){
    	LOG(1,"\nError: Unable to Launch JVM %d\n",ret);
        return NULL;
    }
    globalJvm = jvm;

    LOG(2,"create_vm JVM created\n");
    jclass classLoaderCls = env->FindClass("java/lang/NoClassDefFoundError");

    return globalJvm;
}

JavaVM* start_vm(){
	std::future<JavaVM*> ret = std::async(&create_vm);
return ret.get();
}

std::string from_jstring(JNIEnv *env,jstring jstr){
    const char *cstr = env->GetStringUTFChars(jstr, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jstr, cstr);
    return str;
}

jint jinteger_to_jint(JNIEnv *env,jobject intObj){
	jclass clz = env->FindClass("java/lang/Integer");
	jmethodID method = env->GetMethodID(clz, "intValue", "()I");
	jint ret = env->CallIntMethod(intObj, method);
	if(env->ExceptionCheck()){
		env->ExceptionDescribe();
		throw std::runtime_error("jinteger_to_jint failed");
	}
	return ret;
}

jlong jlong_to_long(JNIEnv *env, jobject longObj){
	jclass clz = env->FindClass("java/lang/Long");
	jmethodID method = env->GetMethodID(clz, "longValue", "()J");
	jlong ret = env->CallLongMethod(longObj, method);
	if(env->ExceptionCheck()){
		env->ExceptionDescribe();
		throw std::runtime_error("jlong_to_long failed");
	}
	return ret;
}

RETCODE jarrayToString(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLUINTEGER maxStringLength,SQLUINTEGER *retByteSize){
	int ret;
	jstring val=(jstring) env->GetObjectArrayElement(data, pos);
	ustring wcharData = ustring(from_jstring(env,val));

	if(pointer == NULL) {
		*retByteSize = (wcharData.size())* sizeof(SQLWCHAR);
		return SQL_SUCCESS;
	}
	if(wcharData.size()>maxStringLength){
		return SQL_ERROR;
	}
	ret = strcpy((SQLWCHAR*)pointer,maxStringLength,wcharData);
	if(retByteSize!= NULL){
		*retByteSize = sizeof(SQLWCHAR)*wcharData.size();
	}
	return ret;
}

RETCODE jarrayToInt(JNIEnv *env, jobjectArray data, int pos, jint* pointer){
	if(pointer == NULL)
		return SQL_ERROR;
	jobject val=(jobject) env->GetObjectArrayElement(data, pos);
	jint ret = jinteger_to_jint(env,val);
	*pointer=ret;
	return SQL_SUCCESS;
}

RETCODE jarrayToInt(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLUINTEGER maxSize) {
	if(maxSize<4){
		return SQL_ERROR;
	}
	return jarrayToInt(env, data, pos, (jint*)pointer);
}

RETCODE jarrayToShort(JNIEnv *env, jobjectArray data, int pos, jshort* pointer){
	if(pointer == NULL)
		return SQL_ERROR;
	jobject val=(jobject) env->GetObjectArrayElement(data, pos);
	jshort ret = (jshort)jinteger_to_jint(env,val);
	*pointer=ret;
	return SQL_SUCCESS;
}

RETCODE jarrayToShort(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLUINTEGER maxSize) {
	if(maxSize<2){
		return SQL_ERROR;
	}
	return jarrayToShort(env, data, pos, (jshort*)pointer);
}

RETCODE jarrayToLong(JNIEnv *env, jobjectArray data, int pos, jlong* pointer){
	if(pointer == NULL)
		return SQL_ERROR;
	jobject val=(jobject) env->GetObjectArrayElement(data, pos);
	jlong ret = (jlong)jlong_to_long(env,val);
	*pointer=ret;
	return SQL_SUCCESS;
}


RETCODE jarrayToString(JNIEnv* env, jobjectArray data, int pos, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue){
	int ret = SQL_SUCCESS;
	if(rgbDesc != NULL){
		jstring val=(jstring) env->GetObjectArrayElement(data, pos);
		ustring wcharData = ustring(from_jstring(env,val));
		if(wcharData.size()* sizeof(SQLWCHAR)>(SQLUSMALLINT)cbDescMax){
			return SQL_ERROR;
		}
		ret = strcpy((SQLWCHAR*)rgbDesc,cbDescMax/2,wcharData);
		if(pcbDesc)
			*pcbDesc = wcharData.size()* sizeof(SQLWCHAR);
	}
	if(numberValue!= NULL){
		*numberValue = 0;
	}
	return ret;
}

RETCODE jarrayToLong(JNIEnv* env, jobjectArray data,int arrayPos, SQLINTEGER * numberData,SQLPOINTER rawData, SQLSMALLINT rawDataMax, SQLSMALLINT  *rawDataType){
	jobject val=(jobject) env->GetObjectArrayElement(data, arrayPos);
	if(env->ExceptionCheck()){
		env->ExceptionDescribe();
		return SQL_ERROR;
	}


	SQLINTEGER valInt = jlong_to_long(env,val);
	if(numberData) {
		*numberData = valInt;
	}
	if(rawData){
		int dataType=0;
		if(rawDataMax<0)
			dataType =rawDataMax;
		if((int)rawDataType <0 && dataType==0)
			dataType =(int)rawDataType;

		if(dataType == SQL_IS_POINTER){
			*rawDataType = SQL_IS_INTEGER;
			*(SQLINTEGER *)rawData=(SQLINTEGER)valInt;
		}
		else if(rawDataMax ==4 || dataType == SQL_IS_INTEGER){
			*rawDataType = SQL_IS_INTEGER;
			*(SQLINTEGER *)rawData=(SQLINTEGER)valInt;
		}
		else if (rawDataMax ==4 || dataType == SQL_IS_UINTEGER){
			*rawDataType = SQL_IS_UINTEGER;
			*(SQLUINTEGER *)rawData=(SQLUINTEGER)valInt;
		}
		else if(rawDataMax ==2 || dataType == SQL_IS_SMALLINT){
			*rawDataType = SQL_IS_SMALLINT;
			*(SQLSMALLINT *)rawData=(SQLSMALLINT)valInt;
		}
		else if (rawDataMax ==2 || dataType == SQL_IS_USMALLINT){
			*rawDataType = SQL_IS_USMALLINT;
			*(SQLUSMALLINT *)rawData=(SQLUSMALLINT)valInt;
		}
		else {
			LOG(1,"getLongFromArrayObject incorrect size %d",rawDataMax);
			return SQL_ERROR;
		}

	}
	return SQL_SUCCESS;
}

RETCODE jstringToChar(JNIEnv *env, jstring data, SQLCHAR* pointer, SQLUINTEGER maxStringLength,SQLUINTEGER *retByteSize){
	int ret;
	string charData = from_jstring(env,data);

	if(pointer == NULL) {
		*retByteSize = (charData.size())* sizeof(SQLCHAR);
		return SQL_SUCCESS;
	}
	if(charData.size()>maxStringLength){
		return SQL_ERROR;
	}

	strcpy((char*)pointer,charData.c_str());
	if(retByteSize!= NULL){
		*retByteSize = sizeof(SQLCHAR)*charData.size();
	}
	return SQL_SUCCESS;
}

RETCODE jstringToString(JNIEnv *env, jstring data, SQLWCHAR* pointer, SQLUINTEGER maxStringLength,SQLUINTEGER *retByteSize){
	int ret;
	ustring wcharData = ustring(from_jstring(env,data));

	if(pointer == NULL) {
		*retByteSize = (wcharData.size())* sizeof(SQLWCHAR);
		return SQL_SUCCESS;
	}
	if(wcharData.size()>maxStringLength){
		return SQL_ERROR;
	}

	ret = strcpy((SQLWCHAR*)pointer,maxStringLength,wcharData);
	if(retByteSize!= NULL){
		*retByteSize = sizeof(SQLWCHAR)*wcharData.size();
	}
	return ret;
}
