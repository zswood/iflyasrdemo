#include "iflyspeech.h"
#include "iflyrecord.h"
#include "encode.h"
#include "format.h"

encode_util encode_tool;
iflydata_parser	parser;

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD_PTR dwParam2)
{
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;
	iflyspeech* demo = (iflyspeech*) dwInstance;
	static int capnum = 0;
	bool finish_stream = false;
	bool result_exist = false;

	if (uMsg == WIM_DATA){
		demo->asr_inst()->write_audio(pwh->lpData, pwh->dwBytesRecorded, finish_stream, result_exist);
		waveInAddBuffer(hwi, pwh, sizeof(WAVEHDR));

		if (result_exist){
			std::string asr_result;
			std::string gb_result;
			bool complete =  false;
			demo->asr_inst()->query_result(asr_result, complete);
			if (!asr_result.empty()){

				std::string plain_result;
				if (encode_tool.convert(asr_result, gb_result, et_utf8, et_gb2312)){
					printf("character encode convert fail, utf8:%s\n", asr_result.c_str());
				}else if (parser.parse(gb_result, ft_json, plain_result)){
					printf(" parse format data fail, data:%s\n", gb_result.c_str());
				}else {
					printf("%s", plain_result.c_str());
				}
			}
			if (complete){
				printf("\n");
				printf("recv final asr result and finish record!\n");
				demo->asr_inst()->release_inst();
				// end the recorder;
				waveInStop(hwi);
			}
		}
		
	}else if (uMsg == WIM_CLOSE){
		finish_stream = true;
		demo->asr_inst()->write_audio(pwh->lpData, pwh->dwBytesRecorded, finish_stream, result_exist);
	}

}

int main(int argc, char* argv[]){
	int ret = 0;

	iflyrecorder recorder;
	iflyspeech demo;
	ret = demo.init();
	demo.asr_inst()->create_inst();
	recorder.start(100, (DWORD_PTR)waveInProc, (DWORD_PTR)&demo);

	while(1){
		Sleep(40);
	}


	ret = demo.fini();
	system("pause");
	return ret;
}