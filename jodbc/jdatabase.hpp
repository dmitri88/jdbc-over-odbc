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
	RETCODE getConnectionAttr(SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLUINTEGER cbValueMax, SQLUINTEGER *pcbValue);
	RETCODE setConnectionAttr(SQLINTEGER fAttribute, PTR rgbValue, SQLUINTEGER	cbValue);
	RETCODE getInfo(SQLUSMALLINT fInfoType, PTR rgbInfoValue, SQLSMALLINT cbInfoValueMax, SQLSMALLINT * pcbInfoValue);
	RETCODE getNativeSql(ustring sql,SQLWCHAR * out,SQLUINTEGER	cbSqlStrMax,SQLUINTEGER   *pcbSqlStr);

	jclass entrypointClass = NULL;
	jobject entrypointObj = NULL;
private:
	JavaVM* jvm;
	void addPath( const std::string& path);
	RETCODE javaConnect();
};


#endif /* JODBC_JDATABASE_HPP_ */
