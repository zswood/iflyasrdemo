#ifndef __FORMAT_H__
#define __FORMAT_H__

/*
 * @brief �ṩiflyʶ�������ݸ�ʽ��У�鼰����,��json,xml;
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
	 * @brief	ʶ������ʽ���ݵĽ���;
	 * @param	typedata[in]	����������;
	 *			type[in]		���������ݸ�ʽ;
	 *			plain[out]		�����ı����;
	 * @return	errorcode;
	 */
	int parse(std::string& typedata, format_type type, std::string& plain);

	/*
	 * @brief	ʶ������ʽ���ݵĴ��;
	 * @param	plain[in]		���������;
	 *			type[in]		��������ݸ�ʽ;
	 *			typedata[out]	������;
	 */
	int packet(std::string& plain, format_type type, std::string& typedata);

private:

};

#endif /*__FORMAT_H__*/