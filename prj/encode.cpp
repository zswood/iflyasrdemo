#include "encode.h"

#ifndef WIN32
#include <iconv.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

encode_util::encode_util(){

}

encode_util::~encode_util(){

}

encode_type encode_util::check(std::string& src)
{
	// check src's encode and return encode type;

	return et_invalid;
}

int encode_util::convert(std::string& src, std::string& dst, encode_type src_encode, encode_type dst_encode)
{
	int ret = 0;
	if (src_encode == dst_encode){
		dst = src;
		return ret;
	}

	switch(src_encode)
	{
	case et_gb2312:
		if (dst_encode == et_utf8){
			ret = encode_gb2312_to_utf8(src, dst);
		}
		break;
	case et_utf8:
		if (dst_encode == et_gb2312){
			ret = encode_utf8_to_gb2312(src, dst);
		}
		break;
	default:
		printf("please input valid src encode!\n");
		break;
	}

	return ret;
}

int encode_util::encode_gb2312_to_utf8(std::string& src, std::string& dst)
{
#ifdef WIN32
	std::wstring unicode;
	int ret = 0;
	ret = to_unicode("gb2312", src, unicode);

	char buf[5];
	for (int i = 0; i != unicode.size(); ++i){
		int len = unicode_to_utf8(unicode[i], buf);
		dst.append(buf, len);
	}
#else
	char outbuf[100*1024] = {0}; // limit size;
	int  ret = code_convert("gb18030", "utf-8", src.c_str(), src.length(), outbuf, sizeof(outbuf));
	if (ret != 0){
		return ret;
	}
	dst = outbuf;
#endif

	return ret;
}

#ifndef WIN32
int encode_util::code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen)
{ 
	iconv_t cd; 
	int     rc; 
	char **pin   =   &inbuf; 
	char **pout  =   &outbuf; 

	cd = iconv_open(to_charset, from_charset); 
	if( cd == 0 )
	{
		srlog_error("ATS_Config::code_convert| iconv_open(%s, %s) failed!", to_charset, from_charset);
		return -1; 
	}

	memset(outbuf,0,outlen); 
	size_t in_len = inlen, out_len = outlen;
	if (iconv(cd, pin, &in_len, pout, &out_len) == -1)
	{
		srlog_error("ATS_Config::code_convert| iconv failed ,ret == -1");
		return -1; 
	}

	iconv_close(cd); 
	return 0; 
}
#endif


int encode_util::encode_utf8_to_gb2312(std::string& src, std::string& gb2312)
{
#if defined(WIN32) || defined(WINCE)
	int buf_len = (src.length() + 2) * 2;
	wchar_t *tmp = new wchar_t[buf_len];
	int str_len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), tmp, buf_len);
	tmp[str_len] = 0;
	char* dst = new char[buf_len];
	memset(dst, 0, buf_len);
	str_len = WideCharToMultiByte(CP_ACP, 0, tmp, str_len, dst, buf_len, 0, 0);
	gb2312 = dst;
	delete[] tmp;
	delete[] dst;
	return 0;
#else
	char outbuf[100*1024] = {0}; // limit size;
	int  ret = code_convert("utf-8", "gb18030",src.c_str(), src.length(), outbuf, sizeof(outbuf));
	if (ret != 0){
		return ret;
	}
	gb2312 = outbuf;
	return 0;
#endif
}

int encode_util::utf8_to_unicode(const char *src, std::wstring &t)
{
	if (src == NULL){
		return -1;
	}

	int size_s = strlen(src);
	int size_d = size_s + 10;

	wchar_t *des = new wchar_t[size_d];
	memset(des, 0, size_d * sizeof(wchar_t));

	int s = 0, d = 0;
	bool toomuchbyte = true; //set true to skip error prefix.

	while (s < size_s && d < size_d)
	{
		unsigned char c = src [s];
		if ( (c & 0x80) == 0 )
		{
			des [d++] += src [s++];
		} 
		else if ( (c & 0xE0) == 0xC0 )  ///< 110x-xxxx 10xx-xxxx
		{
			wchar_t &wideChar = des [d++];
			wideChar  = (src [s + 0] & 0x3F) << 6;
			wideChar |= (src [s + 1] & 0x3F);
			s += 2;
		}
		else if ( (c & 0xF0) == 0xE0 )  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
		{
			wchar_t &wideChar = des [d++];
			wideChar  = (src [s + 0] & 0x1F) << 12;
			wideChar |= (src [s + 1] & 0x3F) << 6;
			wideChar |= (src [s + 2] & 0x3F);
			s += 3;
		} 
		else if ( (c & 0xF8) == 0xF0 )  ///< 1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 
		{
			wchar_t &wideChar = des [d++];
			wideChar  = (src [s + 0] & 0x0F) << 18;
			wideChar  = (src [s + 1] & 0x3F) << 12;
			wideChar |= (src [s + 2] & 0x3F) << 6;
			wideChar |= (src [s + 3] & 0x3F);
			s += 4;
		} 
		else 
		{
			wchar_t &wideChar = des [d++]; ///< 1111-10xx 10xx-xxxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 
			wideChar  = (src [s + 0] & 0x07) << 24;
			wideChar  = (src [s + 1] & 0x3F) << 18;
			wideChar  = (src [s + 2] & 0x3F) << 12;
			wideChar |= (src [s + 3] & 0x3F) << 6;
			wideChar |= (src [s + 4] & 0x3F);
			s += 5;
		}
	}

	t = des;
	delete [] des;
	des = NULL;

	return 0;
}

int encode_util::unicode_to_utf8 (wchar_t wchar, char *utf8)
{
	if ( utf8 == NULL )
	{
		return -1;
	}

	int len = 0;
	int size_d = 8;

	if ( wchar < 0x80 )
	{
		// length = 1;
		utf8 [len++] = (char)wchar;
	}
	else if ( wchar < 0x800 )
	{
		//length = 2;
		if ( len + 1 >= size_d )
			return -1;

		utf8 [len++] = 0xc0 | ( wchar >> 6 );
		utf8 [len++] = 0x80 | ( wchar & 0x3f );
	}
	else if ( wchar < 0x10000 )
	{
		//length = 3;
		if ( len + 2 >= size_d )
			return -1;

		utf8 [len++] = 0xe0 | ( wchar >> 12 );
		utf8 [len++] = 0x80 | ( (wchar >> 6) & 0x3f );
		utf8 [len++] = 0x80 | ( wchar & 0x3f );
	}
	else if ( wchar < 0x200000 )
	{
		//length = 4;
		if ( len + 3 >= size_d )
			return -1;

		utf8 [len++] = 0xf0 | ( (int)wchar >> 18 );
		utf8 [len++] = 0x80 | ( (wchar >> 12) & 0x3f );
		utf8 [len++] = 0x80 | ( (wchar >> 6) & 0x3f );
		utf8 [len++] = 0x80 | ( wchar & 0x3f );
	}
	return 0;
}

int encode_util::to_unicode(const std::string &encoding, const std::string &src, std::wstring &dst)
{
#ifdef WIN32
	int ret = 0;
	int offset = 0;
	unsigned int codepage = CP_ACP;

	if (!strcmp(encoding.c_str(), "utf-8")){
		codepage = CP_UTF8;
		if ( (src.size() > 3) &&
			((unsigned char)src[0] == (unsigned char)0xEF) && 
			((unsigned char)src[1] == (unsigned char)0xBB) && 
			((unsigned char)src[2] == (unsigned char)0xBF) )
			offset = 3;
	}else if(!strcmp(encoding.c_str(), "utf-16")){
		wchar_t *src_begin = (wchar_t*)(void*)src.c_str();
		size_t len = src.size() / sizeof(wchar_t);
		dst.assign (src_begin, len);
		return 0;
	}else if(!strcmp(encoding.c_str(), "gb2312")){
		codepage = 936;
	}else if (!strcmp (encoding.c_str(), "gbk")){
		codepage = 936;
	}else if (!strcmp (encoding.c_str(), "gb18030")){
		codepage = 936;
	}else if (!strcmp (encoding.c_str(), "big5")){
		codepage = 950;
	}

	int wide_buf_size = MultiByteToWideChar(
		codepage,				// code page
		0,						// character-type options
		src.c_str()+offset,		// string to map
		src.size()-offset,		// number of bytes in string
		NULL,					// wide-character buffer
		0);						// size of buffer

	wchar_t *wide_char_buf = new wchar_t [ wide_buf_size + 1];
	if (wide_char_buf == NULL)
		return -1;

	ret = MultiByteToWideChar(
		codepage,				// code page
		0,						// character-type options
		src.c_str()+offset,		// string to map
		src.size()-offset,		// number of bytes in string
		wide_char_buf,			// wide-character buffer
		wide_buf_size);			// size of buffer

	wide_char_buf [ret] = L'\0';

	dst = wide_char_buf;
	delete[] wide_char_buf;
#else
	int ret = 0;
	char outbuf[10*1024]; // limit size;
	ret = code_convert(encoding.c_str(), "unicode", src.c_str(), src.length(), outbuf, sizeof(outbuf));
	SP_ASSERT(ret == 0);

	size_t out_ret = 0;
	dst = sp::char2wchar(outbuf, &out_ret);      
#endif // WIN32

	return 0;
}