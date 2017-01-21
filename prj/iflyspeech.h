#include "include/qisr.h"
#include "include/msp_cmn.h"
#include "include/msp_errors.h"
#include "common.h"

class iflyasr{
public:
	iflyasr();
	~iflyasr();

	int init();
	int fini();


	int create_inst();

	/*
	 * @brief �����Ự֮�����������Ƶ����д����,�Ա��ƶ�ʵʱ����;
	 * @param waveData[in]	��Ƶ��Ƭ�ζ���������;
	 * @param waveLen[in]	��Ƶ��Ƭ�����ݴ�С;
	 * @param finish[in/out] �Ƿ����д��Ƶ����, inʱ��������, outʱ��������;
	 * @param query[out]	�Ƿ���ڻ���ʶ����[����query_result��ȡ];
	 * @return ���ش�����;
	 */
	int write_audio(const void* waveData, unsigned int waveLen, bool& finish, bool& query);

	/*
	 * @brief ʵʱʶ������ѯ����,����Ƶ�������м���Ƶ�����������[����write_audio:queryֵ];
	 * @param result[out] ����ʶ��������[���ܴ��ڿս��,���޽��];
	 * @param finish[out] ʶ�����Ƿ��ȡ��ȫ;
	 */
	int query_result(std::string& result, bool& finish);

	int release_inst();

protected:
	int load(char* lib);
	int unload();
	int proc_addr(char* sym, void*& func);

private:
	HMODULE asr_lib_hdl_;
	const char* asr_sess_hdl_;

	int audio_status_; // 1|2|4;

	// libmsc asr�ӿڵ�ַ;
	Proc_QISRSessionBegin asrfunc_create_;
	Proc_QISRAudioWrite asrfunc_audio_;
	Proc_QISRGetResult asrfunc_result_;
	Proc_QISRSessionEnd asrfunc_release_;
};


class iflyspeech{
public:
	iflyspeech();
	~iflyspeech();

public:
	int init();
	int fini();

	iflyasr* asr_inst(){
		return asr_;
	}

private:
	iflyasr *asr_;
};