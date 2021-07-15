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
static int _seconds=0;
static int _rate=0;
static int _channels=0;
static char* _codec = 0;
static int _comp=1;
static int _frameDur_ms = 20;
static int _vbr=1;
static int _bitrate=-1000;
////////////////////////////////////////////
static bool _exit = false;
std::string _muxer;
std::string _encoder; 
static const char* _opusParam=0;    ///the h264 other params;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfOpusEncDec(...): \n"
	 "encode  the sounds to opus and decode to pcm,save decoding data to a file(*.wav)\n"
	 "> omfOpusEncDec -n test.wav -d 10 -r 16000 -c 1 -p bitrate=30000,frame_duration=100ms,application=voip,complexity=1,bandwidth=FB,forcechannels=mono,framesize=20,inbandfec=0,dtx=0,vbr=0,cvbr=0\n"
     "> https://www.yuque.com/tinycloud/linux/nbk58d\n"
	},
	{"fname"	,'n', _fname 		,"set the file name."},
	{"duration",'d', _seconds		,"set the record duration(*s)."},
	{"rate",'r', _rate				,"set the sample rate of audio."},
	{"ch",'c', _channels			,"set the channels of audio."},
	{"opus"	    ,'p', _opusParam, "the other opus codec params."},
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
				"+FifoFilter:name=vfifo0,max=8"
				"+OpusplusAudEncoder:name=opusEnc,"+_opusParam+
				"+FifoFilter:name=vfifo1,max=8"
				"+OpusplusAudDecoder:name=opusDec"
				"+wav-muxer:name=muxer,url=file://"+_fname+",dbg="+_dbg+
			"}"
		"}"
	;
	//dbgErrPSL(layout);
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

////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfOpusEnc(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!_fname);
	///
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}
