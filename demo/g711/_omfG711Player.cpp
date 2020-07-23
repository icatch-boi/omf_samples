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
static int _rate=0;
static int _channels=0;
////////////////////////////////////////////
static bool _exit = false;
std::string _demuxer;
std::string _decoder;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfG711Player(...): \n"
	 "play the audio file(*.wav). eg..\n"
	 "> omfG711Player -n test.wav \n"
	},
	{"fname"	,'n', _fname 		,"set the file name."},
	{"rate"		,'r', _rate			,"set the sample rate of pcm."},
	{"ch"		,'c', _channels		,"set the channels of pcm."},
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
		"type=Application,layout={"
			"type=Pipeline,layout={"
   				+_demuxer+"url=file://"+_fname+",dbg="+_dbg+
				_decoder+
				"+PcmSink:name=player,live=true,dbg="+_dbg+
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
		std::this_thread::sleep_for(10_ms);////wait 1s
	}
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return true;
}
static bool CheckParamers(){
	returnIfErrC(false,!_fname);
	///
	auto url = std::string("file://")+_fname;
	OmfAttrSet ap(url);
	returnIfErrC(false,!ap);
	auto ext = ap.Get("ext");
	returnIfErrC(false,!ext);
	///
	switch(::Hash(ext)){
		case ::Hash("wav"):
			_demuxer = "wav-demuxer:";
			_decoder = "+g711-decoder:name=decoder";
			break;
		case ::Hash("alaw"):
			_demuxer = (std::string)"IOSource:media_str={type=media-alaw,name=src,ch="+_channels+",rate="+_rate+"},";
			_decoder = "+alaw-decoder:name=decoder";
			break;
    	case ::Hash("ulaw"):
			_demuxer = (std::string)"IOSource:media_str={type=media-ulaw,name=src,ch="+_channels+",rate="+_rate+"},";
			_decoder = "+ulaw-decoder:name=decoder";
			break;
		default:
			dbgErrPSL("unkonw audio file format:"<< _fname);
			return false;
	}
	returnIfErrC(false,_demuxer.empty());
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfG711Player(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	returnIfErrC(0,!CheckParamers());
	///
	Process();
	///
	return 0;
}
