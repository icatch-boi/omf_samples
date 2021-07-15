#include "omf_api.h"
#include "omf_msg_site_user.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define returnIfErrC(v,c) if(c){printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,#c);return v;}
#define dbgTestPL() printf("%s\%d:%s:\n",__FILE__,__LINE__,__FUNCTION__);
#define dbgTestPSL(v) printf("%s\%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,v);
#define dbgTestPDL(v) printf("%s\%d:%s:%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define dbgTestPVL(v) printf("%s\%d:%s:%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define returnIfErrC0(c) if(c){dbgTestPSL(#c);return;}

static const char* _msgName="NNResult";

static std::string CreateLayoutString(void*hd){
	auto str = new char[2048];
	std::unique_ptr<char[]> ptr_str(str);
	sprintf(str,
		"type=ShmService,index=1,partner={"
			"type=application,layout={"
				"type=pipeline,name=pl-nn-process,layout={"
					"pv-yuv-src-adv:w=640,h=360"
					"+FrameRateCtrl:fr=1"
					"+HumanDetectSink:priv_data=%p,name=HumanDetectionSink"
				"}"
			"}"
		"}"
		,hd
	);
	return str;
}
static int MsgcbReceive(void* priv_hd, const void *data, int size){
	dbgTestPDL((unsigned)priv_hd);
	dbgTestPDL((unsigned)data);
	dbgTestPDL(size);
	if(data){
		dbgTestPVL((const char*)data);
	}
	return 1;
}

static int MsgProcess() {
	dbgTestPL();
	returnIfErrC(0,!omfMsgSiteUserRegister(_msgName,&MsgcbReceive,0));
	return 1;
}
static int MsgClose() {
	returnIfErrC(0,!omfMsgSiteUserUnRegister(_msgName));
	return 1;
}

static int NNprocess(const char*str,int wait) {
	printf("%s\n",str);
	auto app = omfCreate(str);
	returnIfErrC(0,!app);
	returnIfErrC(0,!omfStatusUp(app,"play"));
	///wait
	std::this_thread::sleep_for(std::chrono::seconds(wait));
	returnIfErrC(0,!omfStatusDown(app,"null"));
	returnIfErrC(0,!omfDestroy(app));
	return 1;
}

int main(int argc, char *argv[]){
	omfInit(0);

	MsgProcess();

	void* object = (void*)0x01234567;
	std::string str=CreateLayoutString(object);
	NNprocess(str.c_str(),6);

	MsgClose();

	omfUninit(0);
	return 0;
}