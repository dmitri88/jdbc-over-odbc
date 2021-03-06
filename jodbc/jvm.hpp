/*
 * jvm.hpp
 *
 *  Created on: May 15, 2020
 *      Author: dmitri
 */

#ifndef JODBC_JVM_HPP_
#define JODBC_JVM_HPP_

#include <jni.h>
#include <functional>
#include <future>
#include <utility>
#include "log.hpp"

extern JavaVM* start_vm();
extern JavaVM* globalJvm;

#define to_jstring(str) env->NewStringUTF(str.utf8().c_str())
extern std::string from_jstring(JNIEnv *env, jstring jstr);
#define jstring_to_ustring(env,jstr)  ustring(from_jstring(env,jstr))

extern jint jinteger_to_jint(JNIEnv *env, jobject intObj);
extern jlong jlong_to_long(JNIEnv *env, jobject longObj);

extern RETCODE jarrayToString(JNIEnv *env, jobjectArray array, int pos, PTR pointer, SQLUINTEGER maxSize,SQLUINTEGER *retSize);
extern RETCODE jarrayToString(JNIEnv* env, jobjectArray data, int pos, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numberValue);
extern RETCODE jarrayToInt(JNIEnv *env, jobjectArray array, int pos, jint* pointer);
extern RETCODE jarrayToInt(JNIEnv *env, jobjectArray array, int pos, PTR pointer, SQLUINTEGER maxSize);
extern RETCODE jarrayToShort(JNIEnv *env, jobjectArray data, int pos, jshort* pointer);
extern RETCODE jarrayToShort(JNIEnv *env, jobjectArray array, int pos, PTR pointer, SQLUINTEGER maxSize);
extern RETCODE jarrayToLong(JNIEnv *env, jobjectArray data, int pos, jlong* pointer);
extern RETCODE jarrayToLong(JNIEnv* env, jobjectArray data,int arrayPos, SQLINTEGER * numberData,SQLPOINTER rawData, SQLSMALLINT rawDataMax, SQLSMALLINT  *rawDataType);
extern RETCODE jlongToLong(jlong data,SQLINTEGER * numberData,SQLPOINTER rawData, SQLSMALLINT rawDataMax, SQLSMALLINT  *rawDataType);
extern RETCODE jstringToString(JNIEnv *env, jstring data, SQLWCHAR* pointer, SQLUINTEGER maxStringLength,SQLUINTEGER *retByteSize);
extern RETCODE jstringToChar(JNIEnv *env, jstring data, SQLCHAR* pointer, SQLUINTEGER maxStringLength,SQLUINTEGER *retByteSize);



template<typename... Types> int java_callback(const std::function<int(JNIEnv*,Types... )>& func,Types... arg1){
	if(globalJvm == NULL)
		return SQL_ERROR;
	JavaVMAttachArgs __jvm_args = {JNI_VERSION_10, 0, 0};

	auto future = async(std::launch::async, [](const std::function<int(JNIEnv*,Types... )>& func,Types... arg2){
			//LOG(5,"jvm_call INSIDE\n");
			JNIEnv* env;
			globalJvm->AttachCurrentThread((void**) &env, NULL);
			int ret = func(env,arg2...);
			if(env->ExceptionCheck()){
				env->ExceptionClear();
				if(!ret){
					ret = SQL_ERROR;
					LOG(1,"Error: java_callback untracked e");
				}
			}
			globalJvm->DetachCurrentThread();
			return ret;

	},func,arg1...);

	int ret = future.get();
	//LOG(5,"jvm_call OUT\n");
	return ret;
}

#endif /* JODBC_JVM_HPP_ */
