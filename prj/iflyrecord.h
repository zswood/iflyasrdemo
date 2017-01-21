#ifndef __IFLY_RECORD_H__
#define __IFLY_RECORD_H__

/*
 *  windows mm interface reference: https://msdn.microsoft.com/en-us/library/dd743586(v=vs.85).aspx
 */

#include "string"
#include "windows.h"
#include "mmsystem.h"

const static int buffer_num = 8;
static int buffer_size = 32000;

class iflyrecorder
{
public:
	iflyrecorder();
	~iflyrecorder();

	/*
	 * @brief 开启一个windows录音采样实例;
	 * @param rate[in]		录音采样率;
	 * @param bit[in]		录音采样大小;
	 * @param channels[in]	录音采样通道数;
	 * @param frequency[in]	缓冲取样频率,单位:ms;
	 * @param cb[in]		回调接口,waveInProc;
	 * @param uinst[in]		用户实例,用于waveIn回调传递;
	 * @return errorcode;
	 */
	int start(int frequency, DWORD_PTR cb, DWORD_PTR uinst = NULL,
		int rate = 16000, int bit = 16, int channels = 1);

	/*
	 * @brief 关闭windows录音采样实例;
	 */
	int stop();

	/*
	 * @brief 播放录音;
	 * @note: set wave format;
	 */
	int play(std::string localpath);
	int play(void* audio, unsigned int length);

private:
	HWAVEIN wavein_hdl_;
	WAVEFORMATEX wave_format_;
	WAVEHDR wave_hdr_[buffer_num];
	
	// 本地缺省缓存,用于音频回放等;
};

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);


#endif /* __IFLY_RECORD_H__ */