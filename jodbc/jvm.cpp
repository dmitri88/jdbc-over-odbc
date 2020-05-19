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
	return env->CallIntMethod(intObj, method);
}

jlong jlong_to_long(JNIEnv *env, jobject longObj){
	jclass clz = env->FindClass("java/lang/Long");
	jmethodID method = env->GetMethodID(clz, "longValue", "()J");
	return env->CallLongMethod(longObj, method);
}

int jarrayToString(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLINTEGER maxSize,SQLINTEGER *retSize){
	jstring val=(jstring) env->GetObjectArrayElement(data, pos);
	ustring wcharData = ustring(from_jstring(env,val));

	if(pointer == NULL) {
		*retSize = (wcharData.size())* sizeof(SQLWCHAR);
		return SQL_SUCCESS;
	}
	if((long)wcharData.size()>maxSize){
		return SQL_ERROR;
	}
	memcpy(pointer,wcharData.c_str(),(wcharData.size()+1)* sizeof(SQLWCHAR));
	*(((SQLWCHAR*)pointer)+wcharData.size())=0;
	if(retSize!= NULL){
		*retSize = sizeof(SQLWCHAR)*wcharData.size();
	}
	return SQL_SUCCESS;
}

int jarrayToInt(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLINTEGER maxSize) {
	if(pointer == NULL)
		return SQL_ERROR;
	if(maxSize<4){
		return SQL_ERROR;
	}
	jobject val=(jobject) env->GetObjectArrayElement(data, pos);
	jint ret = jinteger_to_jint(env,val);
	*((jint*)pointer)=ret;
	return SQL_SUCCESS;
}

int jarrayToShort(JNIEnv *env, jobjectArray data, int pos, PTR pointer, SQLINTEGER maxSize) {
	if(pointer == NULL)
		return SQL_ERROR;
	if(maxSize<2){
		return SQL_ERROR;
	}
	jobject val=(jobject) env->GetObjectArrayElement(data, pos);
	short  ret = (short)jinteger_to_jint(env,val);

	*((short*)pointer)=ret;
	return SQL_SUCCESS;
}
