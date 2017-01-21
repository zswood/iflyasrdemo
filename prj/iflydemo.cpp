#include "iflyspeech.h"
#include "iflyrecord.h"


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
			bool complete =  false;
			demo->asr_inst()->query_result(asr_result, complete);
			printf("%s\n", asr_result.c_str());
			if (complete){
				demo->asr_inst()->release_inst();
				// end the recorder;
				waveInStop(hwi);
			}
		}
		printf("cap %d \n",capnum++);
		
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