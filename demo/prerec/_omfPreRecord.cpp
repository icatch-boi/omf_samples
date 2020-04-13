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
static const char* _codec="h264";
static int _seconds=30;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfTapeRecorder(...): \n"
	 "record the sounds to a file(*.wav/*.pcm/*.aac). eg..\n"
	 "  omfPreRecord -D1\n"
	 "  omfPreRecord -D1 -c h264\n"
	 "  omfPreRecord -D1 -c aac\n"
	 "  omfPreRecord -d10 -D 1\n"
	 "  omfPreRecord -n test.h264 -d10\n"
	},
	{"fname"	,'n', _fname 		,"set the file name."},
	{"duration",'d', _seconds		,"set the record duration(*s)."},
	{"codec",'c', _codec			,"select the streaming codec."},
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
	auto sink=(std::string)"+FakeSink:name=sink,dbg="+_dbg+",live=false";
	if(_fname)sink=(std::string)"+IOSink:name=sink,dbg="+_dbg+",url=file://_fname";
	auto layout = (std::string)
		"type=Application,layout={"
			"type=Pipeline,layout={"
				"ShmSource:connect={"
					"type=Pipeline,name=plPreRecord"+_codec+
				"}"
				"+PlayUntil:duration="+_seconds+"s"
				+sink+
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
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfPreRecord(...)\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(0,!_fname);
	///
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(helper.Debug());
	if(helper.Log())omf.LogConfig(helper.Log());
	///
	Process(helper.Debug());
	///
	return 0;
}
