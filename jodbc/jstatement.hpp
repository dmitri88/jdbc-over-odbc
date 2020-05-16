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
private:
	JDatabase* connection;
};




#endif /* JODBC_JSTATEMENT_HPP_ */
