#include <thread>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static int _seconds=3600*24*30;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfEventService(...): \n"
	 "this demo is to run a event process service, that records video and auoio streaming to the file when the event is triggering.. eg..\n"
	 "> omfEventService -d 3600\n"
	},
	{"duartion",'d', _seconds	,"set the service run duration(*s)."},
	{},
};
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
			_exit=true;
			break;
		case Hash("recordStart"):
			dbgNotePSL("recordStart");
			break;
		case Hash("recordStop"):
			dbgNotePSL("recordStop");
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	auto muxer="fmp4";
	auto layout =(std::string)"type=Application,layout={"
		"type=pipeline,layout={"
			"ShmSource:connect={"
				"type=pipeline,name=plPreRecordAac"
			"},name=asrc"
			"+FifoFilter:name=afifo,max=8"

			"+ShmSource:connect={"
				"type=pipeline,name=plPreRecordH264"
			"},name=vsrc,link=null"
			"+FifoFilter:name=vfifo,max=8"

			"+Sync:name=sync,link={afifo,vfifo}"
			"+RecordEventServiceTriggerShm:name=ctrl,delApp=false,dbg=false"
	            ",shmCreater={type=EventSharedMemoryObject,name=ShmEventTrigger,free=false}"
			"+muxer-"+muxer+":name=muxer,dbg=false"
		"}"
	"},name=appRecordEventService"
    ;
	dbgTestPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));
	///
	while(!_exit){
		std::this_thread::sleep_for(100_ms);////wait 1s
	}
	///
	returnIfErrC(false,!obj.StatusDown("null"));
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfEventService(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///
	Process();
	///
	return 0;
}
