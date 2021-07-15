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
static const char* _msgSiteKeys="event";
static int _prerec=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfEventService(...): \n"
		 "this demo is to run a event process service, that records video and auoio streaming to the file when the event is triggering.. eg..\n"
		 "> omfEventService -m event\n"
		 "> omfEventService -m event -r 1\n"
		},
		{"msgSiteKeys"  ,'m', _msgSiteKeys	,"set the msg site keys."},
		{"prerec"       ,'r', _prerec	    ,"enable PreRecord."},
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
	bool _dbg = OmfMain::Globle().DebugMode();
	auto layout = (std::string)
			"type=RecordEventService,msgkey="+_msgSiteKeys+",prerec="+_prerec
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
	///
	Process();
	///
	return 0;
}
