/*
 * unicode.cpp
 *
 *  Created on: May 11, 2020
 *      Author: dmitri
 */
#include <stdio.h>
#include <string>
#include <cstring>
#include <malloc.h>
#include <sqlext.h>
#include "unicode.hpp"

#define DETAIL_LOG_LEVEL 0
#define	PG_CARRIAGE_RETURN				'\r'
#define	PG_LINEFEED					'\n'
#define	surrog_check	0xfc00
#define	surrog1_bits	0xd800
#define	surrog2_bits	0xdc00
#define	surrogate_adjust	(0x10000 >> 10)

#define Int4 int
#define UInt4 unsigned int
#define Int2 short
#define UInt2 unsigned short

#define	byte3_m1	0x0f
#define	byte3_m2	0x3f
#define	byte3_m3	0x3f
#define	byte2_m1	0x1f
#define	byte2_m2	0x3f
#define	byte4_m1	0x07
#define	byte4_m2	0x3f
#define	byte4_m31	0x30
#define	byte4_m32	0x0f
#define	byte4_m4	0x3f

#define	byte3check	0xfffff800
#define	byte2_base	0x80c0
#define	byte2_mask1	0x07c0
#define	byte2_mask2	0x003f
#define	byte3_base	0x8080e0
#define	byte3_mask1	0xf000
#define	byte3_mask2	0x0fc0
#define	byte3_mask3	0x003f
#define	byte4_base	0x808080f0
#define	byte4_sr1_mask1	0x0700
#define	byte4_sr1_mask2	0x00fc
#define	byte4_sr1_mask3	0x0003
#define	byte4_sr2_mask1	0x03c0
#define	byte4_sr2_mask2	0x003f

static int little_endian = -1;

SQLULEN utf8_to_ucs2_lf(const char *utf8str, SQLLEN ilen, BOOL lfconv, SQLWCHAR *ucs2str, SQLULEN bufcount, BOOL errcheck)
{
	int			i;
	SQLULEN		rtn, ocount, wcode;
	const UCHAR *str;

	if (!utf8str)
		return 0;

	if (!bufcount)
		ucs2str = NULL;
	else if (!ucs2str)
		bufcount = 0;
	if (ilen < 0)
		ilen = strlen(utf8str);
	for (i = 0, ocount = 0, str = (SQLCHAR *) utf8str; i < ilen && *str;)
	{
		if ((*str & 0x80) == 0)
		{
			if (lfconv && PG_LINEFEED == *str &&
			    (i == 0 || PG_CARRIAGE_RETURN != str[-1]))
			{
				if (ocount < bufcount)
					ucs2str[ocount] = PG_CARRIAGE_RETURN;
				ocount++;
			}
			if (ocount < bufcount)
				ucs2str[ocount] = *str;
			ocount++;
			i++;
			str++;
		}
		else if (0xf8 == (*str & 0xf8)) /* more than 5 byte code */
		{
			ocount = (SQLULEN) -1;
			goto cleanup;
		}
		else if (0xf0 == (*str & 0xf8)) /* 4 byte code */
		{
			if (errcheck)
			{
				if (i + 4 > ilen ||
				    0 == (str[1] & 0x80) ||
				    0 == (str[2] & 0x80) ||
				    0 == (str[3] & 0x80))
				{
					ocount = (SQLULEN) -1;
					goto cleanup;
				}
			}
			if (ocount < bufcount)
			{
				wcode = (surrog1_bits |
					((((UInt4) *str) & byte4_m1) << 8) |
					((((UInt4) str[1]) & byte4_m2) << 2) |
					((((UInt4) str[2]) & byte4_m31) >> 4))
					- surrogate_adjust;
				ucs2str[ocount] = (SQLWCHAR) wcode;
			}
			ocount++;
			if (ocount < bufcount)
			{
				wcode = surrog2_bits |
					((((UInt4) str[2]) & byte4_m32) << 6) |
					(((UInt4) str[3]) & byte4_m4);
				ucs2str[ocount] = (SQLWCHAR) wcode;
			}
			ocount++;
			i += 4;
			str += 4;
		}
		else if (0xe0 == (*str & 0xf0)) /* 3 byte code */
		{
			if (errcheck)
			{
				if (i + 3 > ilen ||
				    0 == (str[1] & 0x80) ||
				    0 == (str[2] & 0x80))
				{
					ocount = (SQLULEN) -1;
					goto cleanup;
				}
			}
			if (ocount < bufcount)
			{
				wcode = ((((UInt4) *str) & byte3_m1) << 12) |
					((((UInt4) str[1]) & byte3_m2) << 6) |
					(((UInt4) str[2]) & byte3_m3);
				ucs2str[ocount] = (SQLWCHAR) wcode;
			}
			ocount++;
			i += 3;
			str += 3;
		}
		else if (0xc0 == (*str & 0xe0)) /* 2 byte code */
		{
			if (errcheck)
			{
				if (i + 2 > ilen ||
				    0 == (str[1] & 0x80))
				{
					ocount = (SQLULEN) -1;
					goto cleanup;
				}
			}
			if (ocount < bufcount)
			{
				wcode = ((((UInt4) *str) & byte2_m1) << 6) |
					(((UInt4) str[1]) & byte2_m2);
				ucs2str[ocount] = (SQLWCHAR) wcode;
			}
			ocount++;
			i += 2;
			str += 2;
		}
		else
		{
			ocount = (SQLULEN) -1;
			goto cleanup;
		}
	}
cleanup:
	rtn = ocount;
	if (ocount == (SQLULEN) -1)
	{
		if (!errcheck)
			rtn = 0;
		ocount = 0;
	}
	if (ocount < bufcount && ucs2str)
		ucs2str[ocount] = 0;
	return rtn;
}

SQLULEN	ucs2strlen(const SQLWCHAR *ucs2str)
{
	SQLULEN	len;
	for (len = 0; ucs2str[len]; len++)
		;
	return len;
}

SQLWCHAR * utf8_to_ucs2_s(const char *utf8str, SQLLEN ilen, SQLWCHAR *ucs2str, SQLULEN bufcount){

	if(ilen == -1){
		ilen = strlen(utf8str);
	}
	if(ucs2str ==  NULL){
		bufcount = ilen*2+2;
		ucs2str = (SQLWCHAR*)malloc(bufcount);
	}
	int a = utf8_to_ucs2_lf(utf8str,ilen,0,ucs2str,bufcount,0);
	return ucs2str;
}

inline int append_char(char* utf8,int pos,int max,char chr){
	if(pos+1>=max){
		return pos;
	}
	utf8[pos] = chr;
	return pos+1;
}

inline int append_string(char* utf8,const char* src,int pos,int len,int max){
	if(pos+len>=max)
		return pos;
	memcpy(utf8 + pos, src, len);
	return pos+len;
}

char *ucs2_to_utf8(const SQLWCHAR *ucs2str, SQLLEN ilen, SQLLEN *olen,char *buffer, int bufSize, BOOL lower_identifier)
{
	char *	utf8str;
	int	len = 0;

	if (!ucs2str)
	{
		if (olen)
			*olen = SQL_NULL_DATA;
		return NULL;
	}
	if (little_endian < 0)
	{
		int	crt = 1;
		little_endian = (0 != ((char *) &crt)[0]);
	}
	if (ilen < 0)
		ilen = ucs2strlen(ucs2str);
	if(buffer != NULL){
		utf8str = buffer;
	}
	else {
		bufSize = ilen * 4 + 1;
		utf8str = (char *) malloc(bufSize);
	}
	if (utf8str)
	{
		int	i = 0;
		UInt2	byte2code;
		Int4	byte4code, surrd1, surrd2;
		const SQLWCHAR	*wstr;

		for (i = 0, wstr = ucs2str; i < ilen; i++, wstr++)
		{
			if (!*wstr)
				break;
			else if (0 == (*wstr & 0xffffff80)) /* ASCII */
			{
				if (lower_identifier)
					len=append_char(utf8str,len,bufSize,(char) tolower(*wstr));
				else
					len=append_char(utf8str,len,bufSize,(char) *wstr);
			}
			else if ((*wstr & byte3check) == 0)
			{
				byte2code = byte2_base |
					    ((byte2_mask1 & *wstr) >> 6) |
					    ((byte2_mask2 & *wstr) << 8);
				if (little_endian)
					append_string(utf8str,(char *) &byte2code,len,sizeof(byte2code),bufSize);
				else
				{
					append_char(utf8str,len,bufSize,((char *) &byte2code)[1]);
					append_char(utf8str,len,bufSize,((char *) &byte2code)[0]);
				}
				len += sizeof(byte2code);
			}
			/* surrogate pair check for non ucs-2 code */
			else if (surrog1_bits == (*wstr & surrog_check))
			{
				surrd1 = (*wstr & ~surrog_check) + surrogate_adjust;
				wstr++;
				i++;
				surrd2 = (*wstr & ~surrog_check);
				byte4code = byte4_base |
					   ((byte4_sr1_mask1 & surrd1) >> 8) |
					   ((byte4_sr1_mask2 & surrd1) << 6) |
					   ((byte4_sr1_mask3 & surrd1) << 20) |
					   ((byte4_sr2_mask1 & surrd2) << 10) |
					   ((byte4_sr2_mask2 & surrd2) << 24);
				if (little_endian)
					append_string(utf8str,(char *) &byte4code,len,sizeof(byte4code),bufSize);
				else
				{
					append_char(utf8str,len,bufSize,((char *) &byte4code)[3]);
					append_char(utf8str,len+1,bufSize,((char *) &byte4code)[2]);
					append_char(utf8str,len+2,bufSize,((char *) &byte4code)[1]);
					append_char(utf8str,len+3,bufSize,((char *) &byte4code)[0]);
				}
				len += sizeof(byte4code);
			}
			else
			{
				byte4code = byte3_base |
					    ((byte3_mask1 & *wstr) >> 12) |
					    ((byte3_mask2 & *wstr) << 2) |
					    ((byte3_mask3 & *wstr) << 16);
				if (little_endian)
					append_string(utf8str,(char *) &byte4code,len,3,bufSize);
				else
				{
					append_char(utf8str,len,bufSize,((char *) &byte4code)[3]);
					append_char(utf8str,len+1,bufSize,((char *) &byte4code)[2]);
					append_char(utf8str,len+2,bufSize,((char *) &byte4code)[1]);
				}
				len += 3;
			}
		}
		utf8str[len] = '\0';
		if (olen)
			*olen = len;
	}
	return utf8str;
}

int strcpy(SQLWCHAR* dest, SQLULEN max,const SQLWCHAR* src,unsigned int len){
	if(max<len){
		return -1;
	}
	memcpy(dest,src,len*sizeof(unsigned short));
	if(max>len){
		*(dest+len)=0;
	}
	return 0;
}

int strcpy(SQLWCHAR* dest, SQLULEN max,ustring src){
	return strcpy(dest,max,src.c_str(),src.size());
}

