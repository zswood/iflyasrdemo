#include "format.h"
#include "third/jsoncpp/json.h"
#include "third/jsoncpp/value.h"
#include "third/jsoncpp/reader.h"

iflydata_parser::iflydata_parser(){

}

iflydata_parser::~iflydata_parser(){

}

int iflydata_parser::parse(std::string& typedata, format_type type, std::string& plain)
{
	int ret = 0;

	switch(type)
	{
	case ft_json:
		{
			Json::Value	data;
			Json::Reader reader;
			if (!reader.parse(typedata, data, 0)){
				ret = -1;
				printf("parse json data fail with %d\n", ret);
				break;
			}

			int size = data["ws"].size();
			for(int i = 0; i < size; i++){
				Json::Value cw = data["ws"][i]["cw"];
				int word_num = cw.size();
				for (int j = 0; j < word_num; j++){
					plain += cw[j]["w"].asCString();
				}
			}
		}
		break;
	case ft_xml:
		// none opt
		break;
	default:
		break;
	}
	

	return ret;
}

int iflydata_parser::packet(std::string& plain, format_type type, std::string& typedata)
{
	int ret = 0;

	return ret;
}