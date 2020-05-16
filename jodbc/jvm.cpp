/*
 * jvm.cpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */

#include <thread>
#include <future>
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

