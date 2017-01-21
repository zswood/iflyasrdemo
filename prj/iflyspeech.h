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
	 * @brief 开启会话之后持续进行音频流的写操作,以便云端实时解码;
	 * @param waveData[in]	音频流片段二进制数据;
	 * @param waveLen[in]	音频流片段数据大小;
	 * @param finish[in/out] 是否结束写音频操作, in时主动结束, out时被动结束;
	 * @param query[out]	是否存在缓存识别结果[调用query_result获取];
	 * @return 返回错误码;
	 */
	int write_audio(const void* waveData, unsigned int waveLen, bool& finish, bool& query);

	/*
	 * @brief 实时识别结果查询操作,在音频流过程中及音频流结束后调用[依赖write_audio:query值];
	 * @param result[out] 语音识别结果内容[可能存在空结果,即无结果];
	 * @param finish[out] 识别结果是否获取完全;
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

	// libmsc asr接口地址;
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