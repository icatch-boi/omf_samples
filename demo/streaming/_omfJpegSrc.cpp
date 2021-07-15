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
#include "IJpegSource.h"
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
static const char* _fname=0;
static int _seconds=30;//seconds
static const char* _keywords="dualos-pull";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _framerate=30;
static int _qp=90;//0~100
static int _triggerInterval=0;//seconds
static int _prerecIdx=0;	///enable/disable preRecord
static const char* _ioMode="pull";
///
static const char* _config=0;
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfH264Src(...): \n"
	 "This demo shows how to get JPEG streaming from OMF using IJpegSource interface.\n"
  	 "[api]https://www.yuque.com/docs/share/0746d93e-d248-42d1-b846-8337683a6255? \n"
	 "> omfJpegSrc -n test.jpeg -d30 -w 1920 -h1080 -fr 30 -q 90\n"
	 "> omfJpegSrc -n test.jpeg -d30 -w 1920 -h1080 -fr 30 -q 90 --push\n"
	},
	{"fname"	,'n'	, _fname		,"record filename(*.jpeg)."},
	{"duration"	,'d'	, _seconds		,"record duration(*s)."},
	{"\nthe yuv paramers:"},
	{"sid"		,'s'	, _sensorID		,"select the sensor with the id."},
	{"w"		,'w'	, _width		,"set the width for yuv source."},
	{"h"		,'h'	, _height		,"set the height for yuv source."},
	{"fr"		,'f'	, _framerate	,"set the framerate for yuv source."},
	{"prerec"   ,'r'	, _prerecIdx	,"set preRecord vbrc index and enable preRecord."},
	{"\nthe jpeg paramers:"},
	{"qp"		,'q'	, _qp			,"set the QP(0~100) to JPEG codec."},
	{"\nio"       		, _ioMode   	,"set frame output mode:pull,push."},
	{"pull"     		, [](){_ioMode="pull";}	,"set frame output mode:pull"},
	{"push"     		, [](){_ioMode="push";},"set frame output mode:push"},
	{"load"     ,'l'	, _fileConfig	,"loading configure file."},
	{"config"   ,'o'	, _config   	,"set configure streaming."},
	{"\nmisc:"},
	{"interval"	,'i'	, _triggerInterval	,"set the interval(seconds) per trigger. Only used for IsSupportSingleFrameTrigger()."},
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
static TimePoint _tpEnd(-1_s);
static bool ProcessFrame(std::shared_ptr<frame_t> frm,FILE*fd,int line){
	if(!frm->data || !frm->size)
		return false;
	///
	if(_tpEnd==-1_s){
		_tpEnd=frm->pts+Seconds(_seconds);
	}else if(frm->pts>_tpEnd){
		///frame receive is enough, exit profram.
		_exit = true;
		return false;
	}
	dbgTestPSL(frm->index
					   <<','<<frm->data
					   <<','<<frm->size
					   <<','<<frm->iskeyframe
					   <<','<<frm->pts
	);
	dbgTestDL(frm->data,16);
	///write to file
	if(fd)fwrite(frm->data,1,frm->size,fd);
	///

	return true;
}
static bool ProcessPull(IJpegSource*src,FILE*fd){
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		auto frm = src->PullFrame(false);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		///sleep & trigger
		auto interval = 10_ms;
		if(_triggerInterval&&src->IsSupportSingleFrameTrigger()) {
			interval = Seconds(_triggerInterval);
			src->Trigger();
		}
		std::this_thread::sleep_for(interval);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IJpegSource*src,FILE*fd){
	///set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IJpegSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming...
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		///sleep & trigger
		auto interval = 10_ms;
		if(_triggerInterval&&src->IsSupportSingleFrameTrigger()) {
			interval = Seconds(_triggerInterval);
			src->Trigger();
		}
		std::this_thread::sleep_for(interval);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessParams(IJpegSource*src){
	if(_config){
		return src->FromConfig(_config);
	}

	if(_fileConfig) {
		return src->LoadConfig(_fileConfig);
	}
	///set jpeg codec
	///set yuv srouce parameters
	returnIfErrC(false,!src->SelectSensor(_sensorID));//select the sensor0.
	if(_width){
		returnIfErrC(false,!src->SetWidth(_width));
	}
	if(_height){
		returnIfErrC(false,!src->SetHeight(_height));
	}
	///set BitRateControl
	if(src->IsSupportQPControl() && _qp){
		returnIfErrC(false,!src->SetQP(_qp));
	}
	///set prerecord
	if(src->IsSupportPreRecord()){
		returnIfErrC(false,!src->SetPreRecordGroup(_prerecIdx));
	}
	///
	if(_ioMode){
		OmfIOMode io(_ioMode);
		returnIfErrCS(false,!io,"unknowniomode:"<<_ioMode);
		returnIfErrC(false,!src->IsSupportIOMode(io));
		returnIfErrC(false,!src->SetFrameIOMode(io));
	}
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	///create a JpegSource instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IJpegSource> src(IJpegSource::CreateNew(_keywords));
	returnIfErrC(false,!src);
	returnIfErrC(false,!ProcessParams(src.get()));
	///open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	///get streaming parameters after Open().
	FILE* fd=0;
	if(_fname) {
		fd = fopen(_fname, "wb");
		if (!fd) {
			dbgErrPSL("open file fail:" << _fname);
		}
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	///streaming......
	if(src->IsSupportPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd));
	}else if(src->IsSupportOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush(src.get(),fd));
	}else{
		dbgErrPSL("null support output mode.");
	}
	returnIfErrC(false,!src->ChangeDown(State::null));
	return true;
}
////////////////////////////////////////////
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfJpegSrc(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///process
	Process();
	///
	return 0;
}






