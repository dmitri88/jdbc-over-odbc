/*
 * jdatabase.cpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */
#include <odbcinst.h>
#include <string>
#include <functional>
#include "jodbc.hpp"
#include "jdatabase.hpp"
#include "unicode.hpp"
#include "jvm.hpp"

using namespace std;

JDatabase::JDatabase(JavaVM *env) {
	this->jvm = env;
}

void JDatabase::addPath(const std::string &path) {
	JNIEnv *env;
	const std::string urlPath = "file:///*.jar";	//+ path;

	//using myFunc_i = decltype(std::function<int(JNIEnv*)));
	//std::function<int(JNIEnv* env)> f_display_42;
	//f_display_42 = [](JNIEnv* env) { int a=5; return testf(env); };
	//int ret = callFunction(f_display_42);
	//printf("ret %d",ret);
	int ret;

	std::function<int(JNIEnv*,const std::string path)> func1;
	func1 = [](JNIEnv *env,const std::string path) {
		jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
		jmethodID getSystemClassLoaderMethod = env->GetStaticMethodID(classLoaderCls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
		jobject classLoaderInstance = env->CallStaticObjectMethod(classLoaderCls, getSystemClassLoaderMethod);
		if(env->ExceptionCheck() || classLoaderInstance == NULL){
			env->ExceptionClear();
			LOG(1,"Error: JDatabase::addPath 1\n");
			return SQL_ERROR;
		}

		jclass urlClassLoaderCls = env->FindClass("java/net/URLClassLoader");
		jclass urlClassPathCls = env->FindClass("jdk/internal/loader/URLClassPath");

		jfieldID f = env->GetFieldID(urlClassLoaderCls, "ucp", "Ljdk/internal/loader/URLClassPath;");
		jobject ucp = env->GetObjectField(classLoaderInstance, f);

		jmethodID addUrlMethod = env->GetMethodID(urlClassPathCls, "addURL", "(Ljava/net/URL;)V");

		jclass urlCls = env->FindClass("java/net/URL");
		jmethodID urlConstructor = env->GetMethodID(urlCls, "<init>", "(Ljava/lang/String;)V");

		jobject urlInstance = env->NewObject(urlCls, urlConstructor, env->NewStringUTF(path.c_str()));
		jclass jniClass = env->FindClass("org/dmitri/jodbc/JniEntrypoint");
		printf("url %p",jniClass);


		//env->CallVoidMethod(ucp, addUrlMethod, env->NewStringUTF(path.c_str()));
		if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
					LOG(1,"Error: JDatabase::addPath 2\n");
					return SQL_ERROR;
				}

		//jclass classLoaderCls = env->FindClass("java/lang/NoClassDefFoundError");
		printf("coooll\n\n");
		return 0;
	};
	ret = java_callback(func1,path);
	printf("java_callback result %d\n", ret);


	// env = create_vm();
	//jclass classLoaderCls = env->FindClass("java/lang/ClassLoader");
	/*
	 jmethodID getSystemClassLoaderMethod = env->GetStaticMethodID(classLoaderCls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
	 jobject classLoaderInstance = env->CallStaticObjectMethod(classLoaderCls, getSystemClassLoaderMethod);

	 jclass urlClassLoaderCls = env->FindClass("java/net/URLClassLoader");
	 jmethodID addUrlMethod = env->GetMethodID(urlClassLoaderCls, "addURL", "(Ljava/net/URL;)V");
	 jclass urlCls = env->FindClass("java/net/URL");
	 jmethodID urlConstructor = env->GetMethodID(urlCls, "<init>", "(Ljava/lang/String;)V");

	 jobject urlInstance = env->NewObject(urlCls, urlConstructor, env->NewStringUTF(urlPath.c_str()));
	 */
	//env->CallVoidMethod(classLoaderInstance, addUrlMethod, urlInstance);
}

RETCODE JDatabase::connect(SQLWCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn) {
	SQLWCHAR buffer[4096];
	ustring inputstring(szConnStrIn);
	int ret=SQL_SUCCESS;

	int pos_start = inputstring.find(ustring(L"DSN=").c_str());
	if (pos_start == -1)
		return SQL_ERROR;

	int pos_end = inputstring.find(ustring(L";").c_str(), pos_start + 4);
	if (pos_end == -1)
		return SQL_ERROR;




	pos_start += 4;
	auto dsn = inputstring.substr(pos_start, pos_end - pos_start);

	int optlen=6;
	ustring *props = new ustring[optlen];
	props[0] = ustring(L"ClassPath");
	props[1] = ustring(L"URL");
	props[2] = ustring(L"DriverClass");
	props[3] = ustring(L"User");
	props[4] = ustring(L"Password");
	props[5] = ustring(L"Debug");

	for (int i = 0; i < optlen; i++) {
		ustring prop = props[i];
		auto ret1 = SQLGetPrivateProfileStringW(
				dsn.c_str(),
				prop.c_str(),prop.c_str(), buffer, sizeof(buffer),
				ustring(L"odbc.ini").c_str());
		if (ret1 > 0) {
			ustring val(buffer, ret1);
//			if (i == 0) {
//				this->addPath(val.utf8());
//			}
			ret = this->setConnectionParameter(prop, val);
			if(ret)
				return SQL_ERROR;
		}
	}

	return this->javaConnect();
}

RETCODE JDatabase::setConnectionParameter(ustring prop, ustring val) {
	int ret;
	std::function<int(JNIEnv*,JDatabase* database,ustring prop, ustring val)> func1;
	func1 = [](JNIEnv *env,JDatabase* database,ustring prop, ustring val) {
		jmethodID method = env->GetMethodID(database->entrypointClass, "setConnectionParameter", "(Ljava/lang/String;Ljava/lang/String;)V");
		env->CallVoidMethod(database->entrypointObj, method,to_jstring(prop),to_jstring(val));
		if(env->ExceptionCheck()){
			LOG(1,"Error: JDatabase::setConnectionParameter");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this, prop, val);
	return ret;
}

RETCODE JDatabase::getConnectionAttr(SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue)
{
	int ret;
	std::function<int(JNIEnv*,JDatabase* connection,SQLINTEGER fAttribute, SQLINTEGER	cbValueMax)> func1;
	func1 = [](JNIEnv *env,JDatabase* connection, SQLINTEGER fAttribute, SQLINTEGER	cbValueMax) {
		jlong attr = (long long)fAttribute;
		jlong val = (long long)cbValueMax;
		jmethodID method = env->GetMethodID(connection->entrypointClass, "getConnectionAttr", "(JJ)V");
		env->CallVoidMethod(connection->entrypointObj, method, attr,val);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JDatabase::getConnectionAttr");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,fAttribute,cbValueMax);
	return ret;
}

RETCODE JDatabase::setConnectionAttr(SQLINTEGER fAttribute, PTR rgbValue, SQLINTEGER	cbValue)
{
	int ret;
	std::function<int(JNIEnv*,JDatabase* connection,SQLINTEGER fAttribute, SQLINTEGER	cbValue)> func1;
	func1 = [](JNIEnv *env,JDatabase* connection, SQLINTEGER fAttribute, SQLINTEGER	cbValue) {
		jlong attr = (long long)fAttribute;
		jlong val = (long long)cbValue;
		jmethodID method = env->GetMethodID(connection->entrypointClass, "setConnectionAttr", "(JJ)V");
		env->CallVoidMethod(connection->entrypointObj, method, attr,val);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JDatabase::setConnectionAttr");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this,fAttribute,cbValue);
	return ret;
}


RETCODE JDatabase::javaConnect() {
	int ret;
	std::function<int(JNIEnv*,JDatabase* database)> func1;
	func1 = [](JNIEnv *env,JDatabase* database) {
		jmethodID method = env->GetMethodID(database->entrypointClass, "connect", "()V");
		env->CallVoidMethod(database->entrypointObj, method);
		if(env->ExceptionCheck()){
			LOG(1,"Error: JDatabase::javaConnect");
			return SQL_ERROR;
		}
		return SQL_SUCCESS;
	};
	ret = java_callback(func1,this);
	return ret;
}

RETCODE JDatabase::preInitConnection(){
	int ret;
	std::function<int(JNIEnv*,JDatabase* database)> func1;
	func1 = [](JNIEnv *env,JDatabase* database) {

		jclass cls = (jclass) env->NewGlobalRef(env->FindClass("org/dmitri/jodbc/JniEntrypoint"));
		database->entrypointClass = cls;
		if(env->ExceptionCheck() || database->entrypointClass == NULL){
			LOG(1,"Error: JDatabase::libraryInit 1");
			return SQL_ERROR;RETCODE libraryInit(SQLWCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn);
		}
		jmethodID createInstanceMethod = env->GetStaticMethodID(cls, "getInstance", "()Lorg/dmitri/jodbc/JniEntrypoint;");
		jobject obj = env->NewGlobalRef(env->CallStaticObjectMethod(cls, createInstanceMethod));
		if(obj == NULL || env->ExceptionCheck()){
			LOG(1,"Error: JDatabase::libraryInit 2\n");
			return SQL_ERROR;
		}
		database->entrypointObj = obj;
		return 0;
	};
	ret = java_callback(func1,this);
	if(ret){
		LOG(1,"Error: JDatabase::libraryInit 3\n");
		return SQL_ERROR;
	}
	return ret;
}