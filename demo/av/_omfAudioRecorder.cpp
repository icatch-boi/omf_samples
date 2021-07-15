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
static const char* _fname=0;
static int _seconds=30;
static int _rate=0;
static int _channels=0;
////////////////////////////////////////////
static bool _exit = false;
std::string _muxer;
std::string _encoder;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfTapeRecorder(...): \n"
	 "record the sounds to a file(*.wav/*.pcm/*.aac). eg..\n"
	 "> omfTapeRecorder -n test.wav -d10\n"
	 "> omfTapeRecorder -n test.aac -d10\n"
	 "> omfTapeRecorder -n test.pcm -r 16000 -c 1 -d10\n"
	},
	{"fname"	,'n', _fname 		,"set the file name."},
	{"duration",'d', _seconds		,"set the record duration(*s)."},
	{"rate",'r', _rate				,"set the sample rate of audio."},
	{"ch",'c', _channels			,"set the channels of audio."},
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
static bool Process(bool _dbg){
	auto layout = (std::string)
		"type=Application,layout={"
			"type=Pipeline,layout={"
				"ShmSource:connect={"
					"type=pipeline,name=plPcmSrc,layout={"
						"mic:name=mic"
						"+RingSink:name=asink,max=16"
					"}"
				"}"
				"+RecordControler:name=rc,duration="+_seconds+"s"
				"+PcmFilter:ch="+_channels+",rate="+_rate+
				_encoder+
				"+"+_muxer+":name=muxer,url=file://"+_fname+",dbg="+_dbg+
			"}"
		"}"
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
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
static bool CheckParamers(){
	returnIfErrC(0,!_fname);
	///
	auto url = std::string("file://")+_fname;
	OmfAttrSet ap(url);
	returnIfErrC(false,!ap);
	auto ext = ap.Get("ext");
	returnIfErrC(false,!ext);
	///
	switch(::Hash(ext)){
		case ::Hash("wav"):
			_muxer = "wav-muxer";
			_encoder = "";
			break;
		case ::Hash("pcm"):
			_muxer = "IOSink";
			_encoder = "";
			break;
		case ::Hash("aac"):
			_muxer = "IOSink";
			_encoder = "+aac-encoder";
			break;
		default:
			dbgErrPSL("unkonw audio file format:"<< _fname);
			return false;
	}
	returnIfErrC(false,_muxer.empty());
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfTapeRecorder(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!CheckParamers());
	///
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}
