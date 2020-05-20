/*
 * jvm.cpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */

#include <thread>
#include <future>
#include <string.h>
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

RETCODE jarrayToString(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLUINTEGER maxSize,SQLUINTEGER *retSize){
	int ret;
	jstring val=(jstring) env->GetObjectArrayElement(data, pos);
	ustring wcharData = ustring(from_jstring(env,val));

	if(pointer == NULL) {
		*retSize = (wcharData.size())* sizeof(SQLWCHAR);
		return SQL_SUCCESS;
	}
	if(wcharData.size()>maxSize){
		return SQL_ERROR;
	}
	ret = strcpy((SQLWCHAR*)pointer,maxSize,wcharData);
	if(retSize!= NULL){
		*retSize = sizeof(SQLWCHAR)*wcharData.size();
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
