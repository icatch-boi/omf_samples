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
static int _width=0;
static int _height=0;
static int _fr = 1;
static int _seconds=0x7fffffff;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfHumanDetect(...): \n"
	 "this demo is to start the HumanDetect service to detect human"
	 ", and post the result message when it detect over. eg..\n"
	 "> omfHumanDetect -w 640 -h 360 -f 1 -d 3600\n"
	},
	{"width"	,'w',_width		,"set the image width."},
	{"height"	,'h',_height	,"set the image height."},
	{"fr"		,'f',_fr		,"set the image framerate(fps)."},
	{"duration"	,'d',_seconds	,"set the process run (*) seconds."},
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
		case Hash("eos"):
			dbgNotePSL("eos");
			_exit=true;
			break;
		case Hash("HumanDetect"):
		{
			const char* data = (ap.Get("info"));
			dbgNotePSL(data);

		}
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(bool _dbg){
		auto layout =(std::string)
		"type=ShmService,index=1,partner={"
			"type=Application,name=app-humandetect,layout={"
				"type=Pipeline,name==pl-human-detect,layout={"
					"pv-yuv-src:w="+_width+",h="+_height+
					"+FrameRateCtrl:fr="+_fr+  
					"+HumanDetect:name=humandetection"
					"+FakeSink:live=false"
				"}"
			"}"
		"}"
	;
	
	dbgTestPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));
	///
	dbgErrPSL(_seconds);
	auto pt0 = Now();
	while(!_exit){
		std::this_thread::sleep_for(100_ms);////wait 1s
		if(Now()-pt0>Seconds(_seconds)) {
			dbgNotePSL("timeout:"<<_seconds<<'s');
			break;
		}
	}
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfHumanDetect\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!_width);
	returnIfErrC(0,!_height);
	returnIfErrC(0,!_fr);
	returnIfErrC(0,!_seconds);
	///
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}
