#include "omf_api.h"
#include "omf_msg_site.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define returnIfErrC(v,c) if(c){printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,#c);return v;}
#define dbgTestPL() printf("%s\%d:%s:\n",__FILE__,__LINE__,__FUNCTION__);
#define dbgTestPSL(v) printf("%s\%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,v);
#define dbgTestPDL(v) printf("%s\%d:%s:%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define dbgTestPVL(v) printf("%s\%d:%s:%s=%s\n",__FILE__,__LINE__,__FUNCTION__,#v,v);
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define returnIfErrC0(c) if(c){dbgTestPSL(#c);return;}

static int _srcID=0;
static int _msgID=100;

static std::string CreateLayoutString(void*hd){
	auto str = new char[2048];
	std::unique_ptr<char[]> ptr_str(str);
	sprintf(str,
		"type=ShmService,partner={"
			"type=application,layout={"
				"type=pipeline,name=pl-nn-process,layout={"
					"pv-yuv-src-adv:w=1280,h=720"
					"+FrameRateCtrl:fr=1"
					"+HumanDetectSink:priv_data=%p,name=HumanDetectionSink"
				"}"
			"}"
		"}"
		,hd
	);
	return str;
}

static int MsgcbReceive(void*hd,const void *data, int size, int sender, int target, unsigned flags){
	dbgTestPDL((unsigned)data);
	dbgTestPDL(size);
	dbgTestPDL(sender);
	dbgTestPDL(target);
	dbgTestPDL(flags);
	if(data){
		dbgTestPVL((const char*)data);
	}
	return 1;
}

static int MsgProcess() {
	dbgTestPL();
	void* site=omfMsgSite0Get();
	returnIfErrC(false,!site);
	returnIfErrC(false,!omfMsgSiteIsWorking(site));
	dbgTestPDL(omfMsgSiteGetID(site));
	returnIfErrC(false,!omfMsgSiteRegister1(site,_msgID,&MsgcbReceive,0));
}
static int MsgClose() {
	void* site=omfMsgSite0Get();
	returnIfErrC(false,!omfMsgSiteUnRegister1(site,_msgID));
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