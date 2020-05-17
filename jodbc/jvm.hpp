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
extern jint jinteger_to_jint(JNIEnv *env, jobject intObj);


template<typename... Types> int java_callback(const std::function<int(JNIEnv*,Types... )>& func,Types... arg1){
	if(globalJvm == NULL)
		return SQL_ERROR;
	JavaVMAttachArgs __jvm_args = {JNI_VERSION_10, 0, 0};
	//LOG(5,"jvm_call IN\n");
	//static thread_local JNIEnv* env = nullptr;
	//if (env == nullptr){
	//	printf("aaaaaaaaaaaaaaaaaa");
	//}

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
