#include "iflyrecord.h"

iflyrecorder::iflyrecorder()
{
	// default
	wave_format_.wFormatTag = WAVE_FORMAT_PCM;
	wave_format_.nChannels = 1;
	wave_format_.nSamplesPerSec = 16000;
	wave_format_.nAvgBytesPerSec = 16000*16*1/8;
	wave_format_.nBlockAlign = 2;
	wave_format_.wBitsPerSample = 16;
	wave_format_.cbSize = 0;

	wavein_hdl_ = NULL;
}

iflyrecorder::~iflyrecorder()
{
	if (wavein_hdl_ != NULL) stop();
}

int iflyrecorder::start(int frequency/*ms*/, DWORD_PTR cb, DWORD_PTR uinst/*=NULL*/, int rate/*=16000*/, int bit/*=16*/, int channels/*=1*/)
{
	wave_format_.nSamplesPerSec = rate;
	wave_format_.wBitsPerSample = bit;
	wave_format_.nChannels = channels;
	wave_format_.nAvgBytesPerSec = (rate * bit * channels)/8;
	wave_format_.nBlockAlign = bit/8;

	MMRESULT nRet = waveInOpen(&wavein_hdl_, WAVE_MAPPER, &wave_format_, cb, uinst, CALLBACK_FUNCTION); 
	if(nRet != MMSYSERR_NOERROR){
		printf("recorder::start | waveInOpen fail with err:%d.\n", nRet);
		return nRet;
	}

	WAVEINCAPS wic;
	waveInGetDevCaps((UINT_PTR)wavein_hdl_, &wic, sizeof(WAVEINCAPS));

	buffer_size = wave_format_.nAvgBytesPerSec / 1000 * frequency;
	for(int i = 0; i < buffer_num; i++){
		wave_hdr_[i].lpData = new char[buffer_size];  
		wave_hdr_[i].dwBufferLength = buffer_size;  
		wave_hdr_[i].dwBytesRecorded = 0;  
		wave_hdr_[i].dwUser = NULL;  
		wave_hdr_[i].dwFlags = 0;  
		wave_hdr_[i].dwLoops = 1;  
		wave_hdr_[i].lpNext = NULL;  
		wave_hdr_[i].reserved = 0;  

		// 添加多个录音缓冲区;
		waveInPrepareHeader(wavein_hdl_, &wave_hdr_[i], sizeof(WAVEHDR));  
		waveInAddBuffer(wavein_hdl_, &wave_hdr_[i], sizeof(WAVEHDR)); 
	}
	waveInStart(wavein_hdl_);
	return 0;
}

int iflyrecorder::stop()
{
	waveInStop(wavein_hdl_);
	waveInReset(wavein_hdl_);
	for (int i=0; i < buffer_num; i++){
		waveInUnprepareHeader(wavein_hdl_, &wave_hdr_[i],sizeof(WAVEHDR));
	}
	waveInClose(wavein_hdl_);
	wavein_hdl_ = NULL;
	return 0;
}