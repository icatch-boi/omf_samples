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
static const char* _fpattern="EVT_%04u-%02u-%02u_%02u-%02u-%02u.mp4";
static const char* _findex=0;
static const char* _fname=0;
static int _triggerCount=1;
static int _recordIntervalSeconds=60;
static int _recordPreSeconds=3;
static int _recordPostSeconds=7;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"appEventServiceTrigger(...): \n"
	 "send one tigger signal(with params) to the event service . eg..\n"
	 "> omfWavRecord -n test.mp4 -i 30 -f 3 -b 7\n"
	 "> omfWavRecord -R EVT%04u%02u%02u%02u%02u%02u.mp4 -c 10 -i 30 -f 3 -b 7\n"
	 "> omfWavRecord -I EVT%05u.mp4 -c 10 -i 30 -f 3 -b 7\n"
	 "> omfWavRecord -c 10 -i 30 -f 3 -b 7\n"
	},
	{"fname"		,'n',_fname					,"set the file name for record."},
	{"frtc"			,'R',_fpattern				,"set the file name pattern(rtc) for record."},
	{"findex"		,'I',_fpattern				,"set the file name pattern(index) for record."},
	{"count"		,'c',_triggerCount			,"set the count of trigger."},
	{"interval"		,'i',_recordIntervalSeconds	,"set the interval(seconds) per trigger."},
	{"pre"			,'f',_recordPreSeconds		,"set the record duration(seconds) before trigger."},
	{"post"			,'b',_recordPostSeconds		,"set the record duration(seconds) after grigger."},

	{},
};
////////////////////////////////////////////////////////////
static std::string createFileName(){
	time_t lt = time(NULL);///https://www.cnblogs.com/kingos/p/4553202.html
	struct tm* tm = localtime(&lt);
	const char* pattern=_fpattern;
	char url[1024];
	sprintf(url, pattern
			, tm->tm_year+1900
			, tm->tm_mon+1
			, tm->tm_mday
			, tm->tm_hour
			, tm->tm_min
			, tm->tm_sec
	);//dbgTestPSL(url);
	return url;
}
static std::string createFileNameIndex(){
	static int index=0;
	const char* pattern=_findex;
	char url[1024];
	sprintf(url, pattern
			,index++
	);//dbgTestPSL(url);
	return url;
}
static std::string createUrl(){
	if(_fname)
		return std::string("file://")+_fname;
	if(_findex)
		return std::string("file://")+createFileNameIndex();
	else
		return std::string("file://")+createFileName();
}
static std::string createParam(){
	auto tp0=Now();
	auto tp=toMs(tp0);
	///
	int pre = _recordPreSeconds;
	int post = _recordPostSeconds;
	///
	auto url = createUrl();
	///
	char params[1024];
	sprintf(params,"pre=%ds,post=%ds,tp=%lldms,url=%s"
			,pre,post,tp,url.c_str()
	);
	return params;
}
static bool Trigger(OmfObject&obj){
	auto params=createParam();
	dbgTestPSL(params);
	///
	returnIfErrC(false,!obj.Set("trigger",params));
	return true;
}
static bool Process(bool _dbg){
	///"type=SharedMemory,name=shm,BufferName=evt,size=4096,free=true"
	auto layout =(std::string)
		"type=EventSharedMemoryObject,name=shm,free=false";
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	///
	auto count = _triggerCount;
	while(!_exit && count--) {
		Trigger(obj);
		if(count) {
			std::this_thread::sleep_for(1_s*_recordIntervalSeconds);
		}
	}
	return true;
}
static bool Check(){
	returnIfErrC(false,!_fname && !_fpattern);
	returnIfErrC(false,!_triggerCount);
	returnIfErrC(false,!_recordPreSeconds + _recordPostSeconds<=0);
	if(_recordIntervalSeconds<_recordPreSeconds+_recordPostSeconds) {
		dbgErrPSL(_recordIntervalSeconds<<"<"<<_recordPreSeconds<<"+"<<_recordPostSeconds);
		_recordIntervalSeconds = _recordPreSeconds + _recordPostSeconds;
	}
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("appEventServiceTrigger(...)");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(false,!Check());
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
