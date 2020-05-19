/*
 * unicode.hpp
 *
 *  Created on: May 14, 2020
 *      Author: dmitri
 */

#ifndef JODBC_UNICODE_HPP_
#define JODBC_UNICODE_HPP_

#include <string>
#include <codecvt>
#include <locale>

#define FALSE 0
#define WCLEN	sizeof(SQLWCHAR)
#define	SPRINTF_FIXED(to, ...) snprintf((to), sizeof(to), __VA_ARGS__)


SQLULEN	utf8_to_ucs2_lf(const char * utf8str, SQLLEN ilen, BOOL lfconv, SQLWCHAR *ucs2str, SQLULEN buflen, BOOL errcheck);
#define	utf8_to_ucs2(utf8str, ilen, ucs2str, buflen) utf8_to_ucs2_lf(utf8str, ilen, FALSE, ucs2str, buflen, FALSE)

extern char *ucs2_to_utf8(const SQLWCHAR *ucs2str, SQLLEN ilen, SQLLEN *olen, char *buffer, int bufSize, BOOL lower_identifier);
extern SQLWCHAR * utf8_to_ucs2_s(const char *utf8str, SQLLEN ilen, SQLWCHAR *ucs2str, SQLULEN bufcount);

typedef std::basic_string<wchar_t> wstring;

class ustring: public std::basic_string<SQLWCHAR>
{
public:
	ustring(): std::basic_string<SQLWCHAR>() { }
	ustring(const SQLWCHAR *wstr): std::basic_string<SQLWCHAR>( wstr ) { }
	ustring(const SQLWCHAR *wstr,int len): std::basic_string<SQLWCHAR>( wstr ) { }
    ustring(const wchar_t *wstr): std::basic_string<SQLWCHAR>(convert(wstr) ) { }
    ustring(std::string utf8): std::basic_string<SQLWCHAR>(convert_utf8(utf8).c_str() ) { }

    std::string utf8(){
    	int buf_len=this->size()*4 +1;
    	char buffer[buf_len];
    	ucs2_to_utf8(this->c_str(),this->size(),NULL,buffer,buf_len,FALSE);
    	return std::string(buffer);
    }

private:
    ustring convert_utf8(std::string utf8){
    	int len = utf8.size();
    	SQLWCHAR buf[len+1];
    	utf8_to_ucs2_lf(utf8.c_str(),len,FALSE,buf,len+1,FALSE);
    	return ustring(buf);
    }
    ustring convert(const wchar_t* wstr){
    	SQLWCHAR null_str[1] = {0};
		if(wstr == NULL)
			return ustring(null_str);
		std::wstring_convert<std::codecvt_utf16<wchar_t>, wchar_t> conv;
		std::string bytes = conv.to_bytes(wstr);
		char* pointer = (char*)bytes.c_str();
 		int len = bytes.length()/sizeof(SQLWCHAR);
 		SQLWCHAR buf[len+1];
 		buf[len]=0;
		for(int i=0;i<len;i++){
			//swap bytes to micro$oft
			char* first = pointer+i*2;
			buf[i] = (*(first) <<8) + *(first+1);
		}
		return ustring(buf);
    }
};

extern int strcpy(SQLWCHAR* dest, SQLULEN max,const SQLWCHAR* src,int len);
extern int strcpy(SQLWCHAR* dest, SQLULEN max,ustring stc);

//debug functions
#define unicode_to_utf8(str) (str!=NULL?ustring(str).utf8().c_str():"")
#define utf8_to_unicode(str) utf8_to_ucs2_s(str, -1, NULL, -1)


#endif /* JODBC_UNICODE_HPP_ */
