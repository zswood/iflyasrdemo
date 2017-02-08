#include "iflyspeech.h"

iflyasr::iflyasr()
	: asr_lib_hdl_ (NULL)
	, asrfunc_create_ (NULL)
	, asrfunc_audio_ (NULL)
	, asrfunc_result_ (NULL)
	, asrfunc_release_ (NULL)
	, asr_sess_hdl_ (NULL)
	, audio_status_ (MSP_AUDIO_SAMPLE_FIRST){
}

iflyasr::~iflyasr(){
}

int iflyasr::init(){

	Proc_MSPLogin func_login = NULL;
	if (load("msc_x64.dll")){
		printf("iflyasr::init | load asr lib fail!\n");	// errinfo打印规则:当步异常错误时打印,调用接口异常时不打印;即仅在错误发生处打印;
		return -1;
	}else if (proc_addr("QISRSessionBegin", (void *&)asrfunc_create_) || proc_addr("QISRAudioWrite", (void *&)asrfunc_audio_) 
		|| proc_addr("QISRGetResult", (void *&)asrfunc_result_) || proc_addr("QISRSessionEnd", (void *&)asrfunc_release_)
		|| proc_addr("MSPLogin", (void *&)func_login)){
			printf("iflyasr::init | proc asr func addr fail!\n");
			return -2;
	}

	int ret = func_login(NULL, NULL, "appid=58738b53");
	if (ret != 0){
		printf("iflyasr::init | msplogin fail with err:%d.\n", ret);
	}
	return ret;
}

int iflyasr::fini(){
	Proc_MSPLogout func_logout = NULL;
	if (proc_addr("MSPLogout", (void *&)func_logout)){
		printf("iflyasr::fini | proc asr func addr fail!\n");
		return -1;
	}

	int ret = func_logout();
	if (ret != 0){
		printf("iflyasr::fini | msplogout fail with err:%d.\n", ret);
	}
	unload();
	return ret;
}

int iflyasr::create_inst(){
	if (asr_sess_hdl_ != NULL){
		release_inst();
	}
	int ret = 0;
	std::string asr_params = "sub=iat,domain=iat,language=zh_cn,accent=mandarin,sample_rate=16000,aue=speex-wb";
	//std::string asr_params = "sub=iat,domain=cmn-hans-cn,language=zh_cn,accent=mandarin,sample_rate=16000,aue=speex-wb,svad=0";
	asr_sess_hdl_ = asrfunc_create_(NULL, asr_params.c_str(), &ret);
	if (asr_sess_hdl_ == NULL){
		printf("iflyasr::create_inst | call asr sessionbegin fail with err:%d\n", ret);
		return ret;
	}

	return 0;
}

int iflyasr::write_audio(const void* waveData, unsigned int waveLen, bool& finish, bool& result){
	if (asr_sess_hdl_ == NULL){
		printf("iflyasr::query_result | sess hdl not initialize\n");
		return -1;
	}else if (audio_status_ == MSP_AUDIO_SAMPLE_LAST){
		printf("iflyasr::query_result | last audio has written\n");
		return -2;
	}

	int eps = 0, rcs = 0;
	audio_status_ = finish ? MSP_AUDIO_SAMPLE_LAST : audio_status_;
	int ret = asrfunc_audio_(asr_sess_hdl_, waveData, waveLen, audio_status_, &eps, &rcs);
	if (ret != 0){
		printf("iflyasr::write_audio | call asr audiowrite fail with err:%d\n", ret);
		return ret;
	}

	if (eps >= MSP_EP_AFTER_SPEECH || rcs == MSP_REC_STATUS_COMPLETE){ // 异常|检测到后端点|识别结束;
		audio_status_ = MSP_AUDIO_SAMPLE_LAST;
		finish = true;
	}else if (audio_status_ == MSP_AUDIO_SAMPLE_FIRST){
		audio_status_ = MSP_DATA_SAMPLE_CONTINUE;
		finish = false;
	}
	
	result = (rcs == MSP_REC_STATUS_SUCCESS || rcs == MSP_REC_STATUS_COMPLETE) ? true : false;
	return ret;
}

int iflyasr::query_result(std::string& result, bool& finish){
	if (asr_sess_hdl_ == NULL){
		printf("iflyasr::query_result | sess hdl not initialize\n");
		return -1;
	}

	int ret = 0, rcs = 0;
	const char* asr_result = asrfunc_result_(asr_sess_hdl_, &rcs, 5000, &ret);
	if (ret != 0){
		printf("iflyasr::query_result | call asr getresult fail with err:%d\n", ret);
		return ret;
	}else if(asr_result != NULL){
		result = asr_result;
	}

	finish = false;
	if (rcs == MSP_REC_STATUS_NO_MATCH || rcs == MSP_REC_STATUS_COMPLETE){
		audio_status_ = MSP_AUDIO_SAMPLE_LAST;
		finish = true;
	}
	return ret;
}

int iflyasr::release_inst(){
	if (asr_sess_hdl_ == NULL){
		return 0;
	}
	int ret = asrfunc_release_(asr_sess_hdl_, NULL);
	audio_status_ = MSP_AUDIO_SAMPLE_FIRST;
	asr_sess_hdl_ = NULL;
	return ret;
}

int iflyasr::load(char* lib){
	asr_lib_hdl_ = LoadLibraryA(lib);
	if (asr_lib_hdl_ == NULL)
		return GetLastError();
	return 0;
}

int iflyasr::unload(){
	if (asr_lib_hdl_ && !FreeLibrary(asr_lib_hdl_))
		return GetLastError();
	return 0;
}

int iflyasr::proc_addr(char* sym, void*& func){
	func = GetProcAddress(asr_lib_hdl_, sym);
	if (func == NULL)
		return GetLastError();
	return 0;
}




iflyspeech::iflyspeech()
	: asr_ (NULL){
};

iflyspeech::~iflyspeech(){
	if (asr_ != NULL) delete asr_; asr_ = NULL;
}

int iflyspeech::init(){
	int ret = 0;
	if (asr_ == NULL){
		asr_ = new iflyasr;
		ret = asr_->init();
	}
	return ret;
}

int iflyspeech::fini(){
	int ret = 0;
	if (asr_ != NULL){
		ret = asr_->fini();
		delete asr_; asr_ = NULL;
	}
	return ret;
}