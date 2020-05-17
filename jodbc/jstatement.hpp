/*
 * jstatement.hpp
 *
 *  Created on: May 15, 2020
 *      Author: dmitri
 */

#ifndef JODBC_JSTATEMENT_HPP_
#define JODBC_JSTATEMENT_HPP_

#include "jdatabase.hpp"

class JStatement{
public:
	JStatement(JDatabase* connection);
	RETCODE createStatement();
	RETCODE freeStatement(SQLUSMALLINT option);
	RETCODE execDirect(ustring sql);
	RETCODE getRowCount(SQLLEN * retCount);
	RETCODE getResultColumnCount(SQLSMALLINT * retCount);
	RETCODE describeColumn(SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLULEN * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable);
	RETCODE getColumnAttribute(SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLLEN *pfDesc);
private:
	JDatabase* connection;
};




#endif /* JODBC_JSTATEMENT_HPP_ */
