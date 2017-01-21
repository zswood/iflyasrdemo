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
	 * @brief ����һ��windows¼������ʵ��;
	 * @param rate[in]		¼��������;
	 * @param bit[in]		¼��������С;
	 * @param channels[in]	¼������ͨ����;
	 * @param frequency[in]	����ȡ��Ƶ��,��λ:ms;
	 * @param cb[in]		�ص��ӿ�,waveInProc;
	 * @param uinst[in]		�û�ʵ��,����waveIn�ص�����;
	 * @return errorcode;
	 */
	int start(int frequency, DWORD_PTR cb, DWORD_PTR uinst = NULL,
		int rate = 16000, int bit = 16, int channels = 1);

	/*
	 * @brief �ر�windows¼������ʵ��;
	 */
	int stop();

	/*
	 * @brief ����¼��;
	 * @note: set wave format;
	 */
	int play(std::string localpath);
	int play(void* audio, unsigned int length);

private:
	HWAVEIN wavein_hdl_;
	WAVEFORMATEX wave_format_;
	WAVEHDR wave_hdr_[buffer_num];
	
	// ����ȱʡ����,������Ƶ�طŵ�;
};

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);


#endif /* __IFLY_RECORD_H__ */