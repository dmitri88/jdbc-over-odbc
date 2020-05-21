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
	RETCODE getRowCount(SQLINTEGER * retCount);
	RETCODE getResultColumnCount(SQLSMALLINT * retCount);
	RETCODE describeColumn(SQLUSMALLINT colnum, SQLWCHAR *colName, SQLSMALLINT bufLength, SQLSMALLINT *nameLength, SQLSMALLINT * dataType, SQLUINTEGER * colSize, SQLSMALLINT * decimalDigits, SQLSMALLINT * nullable);
	RETCODE getColumnAttribute(SQLUSMALLINT icol, SQLUSMALLINT fDescType, SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax, SQLSMALLINT  *pcbDesc, SQLINTEGER *numValue);
	RETCODE getStatementAttr(SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax, SQLINTEGER	*stringLength);
	RETCODE setStatementAttr(SQLINTEGER	fAttribute, PTR		rgbValue, SQLINTEGER	cbValueMax);
	RETCODE bindColumn(SQLUSMALLINT column, SQLSMALLINT type, PTR value, SQLUINTEGER bufLength, SQLUINTEGER * strLengthOrIndex);
	RETCODE fetch();
	RETCODE moreResults();
private:
	JDatabase* connection;
};




#endif /* JODBC_JSTATEMENT_HPP_ */
