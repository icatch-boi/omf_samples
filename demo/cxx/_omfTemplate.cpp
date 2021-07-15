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
///input paramers
static int _width=0;
static int _height=0;
static int _fr = 1;
static int _seconds=0x7fffffff;
////////////////////////////////////////////
///exit flags
static bool _exit = false;
////////////////////////////////////////////
///input parameters parser table
static OmfHelper::Item _options0[]{
	{"omfTemplate(...): \n"
	 "this demo is to start the fake yuv source"
	 ", and post the result message \n"
	 "> omfTemplate -w 640 -h 360 -f 1 -d 3600\n"
	},
	{"width"	,'w',_width		,"set the image width."},
	{"height"	,'h',_height	,"set the image height."},
	{"fr"		,'f',_fr		,"set the image framerate(fps)."},
	{"duration"	,'d',_seconds	,"set the process run (*) seconds."},
	{},
};
////////////////////////////////////////////
///message callback
static bool MessageProcess(const char* msg0){
	dbgTestPSL(msg0);
	OmfAttrSet ap(msg0);
	returnIfErrC(true,!ap);
	auto msg = ap.Get("msg");
	returnIfErrC(true,!msg);
	switch(Hash(msg)){
		///error message, to exit program
		case Hash("error"):
		case Hash("err"):
			dbgNotePSL("error");
			_exit=true;
			break;
		///end of scan message, to exit programs
		case Hash("eos"):
			dbgNotePSL("eos");
			_exit=true;
			break;
		///the specified message
		case Hash("xxx"):
			dbgNotePSL("xxx");
			break;
		///unknow message
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///the Applicaion Layout string
	auto layout =(std::string)
		"type=ShmService,index=1,partner={"
			"type=Application,layout={"
				"type=Pipeline,layout={"
					"pv-yuv-src:w="+_width+",h="+_height+
					"+FrameRateCtrl:fr="+_fr+
					"+FakeSink:dbg="+_dbg+
				"}"
			"}"
		"}"
	;
	dbgTestPSL(layout);
	///create an instance of application
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	///Register callback to receive messages
	returnIfErrC(false,!obj.Register(&MessageProcess));
	///start the application.
	returnIfErrC(false,!obj.StatusUp("play"));
	///wait the exit flags, or wait for the duration of programs.
	auto pt0 = Now();
	while(!_exit){
		std::this_thread::sleep_for(100_ms);////wait 1s
		if(Now()-pt0>Seconds(_seconds)) {
			dbgNotePSL("timeout:"<<_seconds<<'s');
			break;
		}
	}
	///stop the application
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
static bool CheckParameters(){
	returnIfErrC(false,!_width);
	returnIfErrC(false,!_height);
	returnIfErrC(false,!_fr);
	returnIfErrC(false,!_seconds);
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the parameters
	returnIfErrC(0,!CheckParameters());
	///
	Process();
	///
	return 0;
}
