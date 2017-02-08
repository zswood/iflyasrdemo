#ifndef __FORMAT_H__
#define __FORMAT_H__

/*
 * @brief 提供ifly识别结果数据格式的校验及解析,如json,xml;
 * @date	2017.01.13;
 * @author	zswood;
 */

#include "stdio.h"
#include "stdlib.h"
#include "string"

enum format_type{
	ft_invalid = 0,
	ft_plain,
	ft_json,
	ft_xml,
	ft_count,
};


class iflydata_parser
{
public:
	iflydata_parser();
	~iflydata_parser();

public:
	/*
	 * @brief	识别结果格式数据的解析;
	 * @param	typedata[in]	待解析数据;
	 *			type[in]		待解析数据格式;
	 *			plain[out]		解析文本结果;
	 * @return	errorcode;
	 */
	int parse(std::string& typedata, format_type type, std::string& plain);

	/*
	 * @brief	识别结果格式数据的打包;
	 * @param	plain[in]		待打包数据;
	 *			type[in]		待打包数据格式;
	 *			typedata[out]	打包结果;
	 */
	int packet(std::string& plain, format_type type, std::string& typedata);

private:

};

#endif /*__FORMAT_H__*/