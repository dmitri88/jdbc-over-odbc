/*
 * jdatabase.hpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */

#ifndef JODBC_JDATABASE_HPP_
#define JODBC_JDATABASE_HPP_

#include <jni.h>
#include <sqlext.h>
#include "unicode.hpp"

class JDatabase{
public:
	JDatabase(JavaVM* jvm);
	RETCODE preInitConnection();
	RETCODE connect(SQLWCHAR *szConnStrIn, SQLSMALLINT cbConnStrIn);
	RETCODE setConnectionParameter(ustring prop, ustring val);
	RETCODE getConnectionAttr(SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER *pcbValue);
	RETCODE setConnectionAttr(SQLINTEGER fAttribute, PTR rgbValue, SQLINTEGER	cbValue);

	jclass entrypointClass = NULL;
	jobject entrypointObj = NULL;
private:
	JavaVM* jvm;
	void addPath( const std::string& path);
	RETCODE javaConnect();
};


#endif /* JODBC_JDATABASE_HPP_ */
