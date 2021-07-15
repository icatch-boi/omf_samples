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
static const char* _fname="test.wav";
static int _seconds=30;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfWavRecord(...): \n"
	 "record the sounds to wave file(*.wav). eg..\n"
	 "> omfWavRecord -n test.wav -d10\n"
	},
	{"fname"	,'n', _fname 		,"set the wav file name."},
	{"duration",'d', _seconds	,"set the wav record duration(*s)."},
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
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static bool Process(){
	auto _dbg=OmfMain::Globle().DebugMode();
	auto layout = (std::string)
		"type=Application,name=appWavRec,layout={"
			"type=Pipeline,name=pl_wav_rec,layout={"
				"ShmSource:connect={"
					"type=pipeline,name=plPcmSource,layout={"
						"mic:name=mic"
						"+RingSink:name=asink,max=16"
					"}"
				"}"
				"+RecordControler:name=rc,duration="+_seconds+"s"
				"+WavMuxer:name=wavmux,url=file://"+_fname+",dbg="+_dbg+
			"}"
		"}"
	;
	dbgTestPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));
	///
	std::cout<<"Recording";
	while(!_exit){
		std::this_thread::sleep_for(1000_ms);////wait 1s
		std::cout<<'.';
	}
	std::cout<<std::endl;
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfWavRecord(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	Process();
	///
	return 0;
}
