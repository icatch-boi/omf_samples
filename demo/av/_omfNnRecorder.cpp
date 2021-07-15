#include <thread>
#include <list>
#include <mutex>
#include <sys/statfs.h>
#include <unistd.h>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "omf_msg_site_user.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static const char* _fname="/tmp/REC_%04u-%02u-%02u_%02u-%02u-%02u.mp4";
static bool _fnameWithRtc = true;
///
static int _recordSeconds=60;
static bool _powerFailProtection=true;
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
static bool _exitstart = false;
static std::string _path="./";
static const char* _msgName="humanoidDetFail";

#define FILE_PATH "/tmp/userThread"

/////////////////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfTriggerRecorder(...): \n"
	 "record the video and audio streaming to a container file(*.mp4/*.mov). eg..\n"
	 "> omfTriggerRecorder  -d300 \n"
	},
	{"fname"	,'n', _fname			,"set the record file name."},
	{"duration"	,'d', _recordSeconds	,"set the record duration."},
	{"protection",'P', _powerFailProtection	,"enable the power fail protection.Currently, "
										  "only fragment mp4 file format can be used on this mode."},
	{},
};
////////////////////////////////////////////
static void RegisterUserPid(){
	if(access(FILE_PATH,F_OK)){
        printf("[Error]directory %s no exist!!\n",FILE_PATH);
		return;
	}
	FILE* fp = NULL;
	char fname[32]={0};
	sprintf(fname,"%s/%d",FILE_PATH,getpid());
	if((fp = fopen(fname,"w+")) == NULL){
			printf("[Error]pid file create failed!\n");
			return;
	}
	fclose(fp);
}
///////////////////////////////////////////
static int MsgSitecbReceive(void* priv_hd, const void *data, int size){
	dbgTestPSL((unsigned)priv_hd);
	dbgTestPSL((unsigned)data);
	dbgTestPSL(size);
	if(data){
		dbgTestPVL((const char*)data);
	}
	_exitstart = true;
	return 1;
}
////////////////////////////////////////////
static bool MessageProcess(const char* msg0){
	dbgTestPSL(msg0);
	OmfAttrSet ap(msg0);
	returnIfErrC(true,!ap);
	auto msg = ap.Get("msg");
	returnIfErrC(true,!msg);
	switch(Hash(msg)){
		case Hash("error"):
		case Hash("err"):
			dbgNotePSL("error");
			_exitstart=true;
			break;
		case Hash("eos"):
			dbgNotePSL("eos");
			_exitstart=true;
			break;
		case Hash("recordStart"):
			dbgTestPSL(ap.Get("url"));
			break;
		case Hash("recordStop"):
			dbgNotePSL("stop");
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(bool _dbg){
	auto fname = std::string("file://")+_fname;
	OmfAttrSet pat(fname);
	auto muxer = pat.Get("ext");
	returnIfErrC(false,!muxer);
	auto path = pat.Get("path");
	returnIfErrC(false,!path);
	_path=path;
	if(_powerFailProtection){
		returnIfErrC(false,::Hash(muxer)!=::Hash("mp4"));
		muxer = "fmp4";
	}
	///
	auto layout =(std::string)
		"type=Application,layout={"
			"type=Pipeline,layout={"
				"ShmSource:connect={"
					"type=pipeline,name=plPreRecordAac"	
				"},name=asrc,sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}}"
				"+FifoFilter:name=afifo,max=64"
				//
				"+ShmSource:connect={"
					"type=Pipeline,name=plPreRecordH264"
				"},name=vsrc,sendmsg={preRecOutput={oper=AfterStart,send={forward}},preRecOutputDisable={oper=BeforeStop,send={forward}}},link=null"
				"+FifoFilter:name=vfifo,max=64"
				//
				"+Sync:name=sync,link={afifo,vfifo}"
				"+RecordControler:name=rc,duration="+_recordSeconds+"s"
				"+muxer-"+muxer+":name=muxer,flush="+_powerFailProtection+
					",urlcreaterlayout={type=UrlRtc,pattern="+fname+"}"
					",sendmsg={recordStart={oper=AfterOpen,send={upward}},recordStop={oper=AfterClose,send={upward}}}"
			"}"
		"}"
	;
	dbgTestPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));
	///
	while(!_exitstart){
		std::this_thread::sleep_for(100_ms);////wait 1s
	}
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
/////////////////////////////////////////////////

static bool Check(){
	returnIfErrC(false,!_fname);
	return true;
}
/////////////////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfTriggerRecorder(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	////reg pid
	RegisterUserPid();
	/////////
	//msgsiet
	returnIfErrC(0, !omfMsgSiteUserRegister(_msgName, &MsgSitecbReceive, 0));
	///process
	Process(OmfMain::Globle().DebugMode());
	returnIfErrC(0,!omfMsgSiteUserUnRegister(_msgName));
	///
	return 0;
}
