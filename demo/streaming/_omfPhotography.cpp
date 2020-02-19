#include <thread>
#include <memory>
#include "_call.h"
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "IPhotography.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::chrono;
using namespace omf::api;
using namespace omf::api::streaming;
using namespace omf::api::streaming::common;
////////////////////////////////////////////////////////////
static const char* _frtcpattern="SNAP_%04u-%02u-%02u_%02u-%02u-%02u.jpg";
static const char* _fidxpattern=0;
static const char* _fname=0;
static const char* _keywords="dualos";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _qp=90;
static int _snapCount=1;
static int _snapInterval=1;//seconds
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfPhotography(...): \n"
	 "This demo shows how to use the IPhotography interface to snap the image from OMF ,and save it as the JPG file.\n"
	 "> omfPhotography -x SNAP_%03u.jpg -c 30 -i 1 -w1920 -h1080 -q80\n"
	 "> omfPhotography -c 30 -i 1 -w1920 -h1080 -q80\n"
	 "> omfPhotography -n AAA.jpg -c 1 -w1920 -h1080 -q80\n"
	},
	{"fname"	,'n', _fname	,"filename(*.jpg)."},
	{"findex"	,'x', _fidxpattern	,"file pattern with index.(SNAP%04u.jpg)."},
	{"frtc"		,'r', _frtcpattern	,"file pattern with rtc.(SNAP_%04u-%02u-%02u_%02u-%02u-%02u.jpg)."},
	{"count"	,'c', _snapCount	,"snap count."},
	{"interval"	,'i', _snapInterval	,"the interval(seconds) per snap."},
	{"the yuv paramers:"},
	{"sid"		,'s', _sensorID	,"select the sensor with the id."},
	{"w"		,'w', _width	,"set the width for yuv source."},
	{"h"		,'h', _height	,"set the height for yuv source."},
	{"the jpeg paramers:"},
	{"qp"		,'q', _qp	,"set the QP(0~100) to jpeg codec."},
	{"misc:"},
	{"keywords"	,'k', _keywords	,"select the IH264Source with keywords." },
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
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
////////////////////////////////////////////////////////////
static std::string createFileNameRtc(){
	time_t lt = time(NULL);///https://www.cnblogs.com/kingos/p/4553202.html
	struct tm* tm = localtime(&lt);
	const char* pattern=_frtcpattern;
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
	const char* pattern=_fidxpattern;
	char url[1024];
	sprintf(url, pattern
			,index++
	);//dbgTestPSL(url);
	return url;
}
static std::string createUrl(){
	if(_fname)
		return std::string("file://")+_fname;
	else if(_fidxpattern)
		return std::string("file://")+createFileNameIndex();
	else if(_frtcpattern)
		return std::string("file://")+createFileNameRtc();
	return "test.jpg";
}
static bool Process(bool _dbg){
	///////////////////////////////////////
	//create a IPhotography instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IPhotography> src(IPhotography::CreateNew(_keywords));
	returnIfErrC(false,!src);
	//set yuv srouce parameters
	src->SelectSensor(_sensorID);//select the sensor0.
	src->SetWidth(_width);
	src->SetHeight(_height);
	//register streaming callback
	src->RegisterMessageCallback(&MessageProcess);

	//open streaming
	//returnIfErrC(false,!src->ChangeUp(State::ready));

	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));

	auto count = _snapCount;
	while(!_exit && count--){
		//snap one frame
		src->SetQP(_qp);
		src->Snap(createUrl().c_str(),false);
		std::this_thread::sleep_for(std::chrono::seconds(_snapInterval));
	}
	//stop streaming
	//returnIfErrC(false,!src->ChangeDown(State::ready));
	//close streaming
	returnIfErrC(false,!src->ChangeDown(State::null));
	return true;
}
////////////////////////////////////////////
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfPhotography(...)\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(0,!Check());
	///init the omf module
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(helper.Debug());
	if(helper.Log())omf.LogConfig(helper.Log());
	///process
	Process(helper.Debug());
	///
	return 0;
}






