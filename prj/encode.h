#ifndef __ENCODE_H__
#define __ENCODE_H__

/*
 * @brief 提供字符编码检测及转换功能支持;
 * @date	2017.01.13;
 * @author	zswood;
 */

#include "stdio.h"
#include "stdlib.h"
#include "string"

enum encode_type{
	et_invalid = 0,
	et_unicode,
	et_utf8,
	et_gb2312,
	et_count,
};

class encode_util{
public:
	encode_util();
	~encode_util();

public:
	encode_type check(std::string& src);
	int convert(std::string& src, std::string& dst,
			encode_type src_encode, encode_type dst_encode);

private:
	int encode_utf8_to_gb2312(std::string& src, std::string& dst);
	int encode_gb2312_to_utf8(std::string& src, std::string& dst);

	int utf8_to_unicode (const char *src, std::wstring &t);
	int unicode_to_utf8 (wchar_t wchar, char *utf8);
	int to_unicode (const std::string &encoding, const std::string &src, std::wstring &dst);
#ifndef WIN32
	int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);
#endif

	/*
		WIN32:
			MultiByteToWideChar
			WideCharToMultiByte
		LINUX:
			iconv_open
			iconv
			iconv_close
	 */

};

#endif /* __ENCODE_H__ */