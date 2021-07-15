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
#include "IYuvSource.h"
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
static const char* _fname="source.yuv";
static int _seconds=30;//seconds
static const char* _keywords="dualos-pull";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _framerate=30;
static const char* _ioMode="pull";
///
static const char* _config=0;
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfH264Src(...): \n"
	 "This demo shows how to get YUV streaming from OMF using IYuvSource interface.\n"
  	 "[api]https://www.yuque.com/docs/share/8e62c718-b57a-40e7-80e7-bfd241f2866c? \n"
	 "> omfYuvSrc -n test.yuv -d 30 -w1920 -h1080\n"
	 "> omfYuvSrc -n test.yuv -d 30 -w1920 -h1080 --push\n"
	},
	{"fname"	,'n'	, _fname						,"record filename(*.aac)."},
	{"duration"	,'d'	, _seconds						,"process execute duration(*s)."},
	{"\nthe yuv paramers:"},
	{"sid"		,'s'	, _sensorID						,"select the sensor with the id."},
	{"w"		,'w'	, _width						,"width of image."},
	{"h"		,'h'	, _height						,"height of image."},
	{"fr"		,'f'	, _framerate					,"the frame number per seconds."},
	{"\nio"           	, _ioMode   					,"set frame output mode:pull,push."},
	{"pull"         	, [](){_ioMode="pull";}			,"set frame output mode:pull"},
	{"push"         	, [](){_ioMode="push";}			,"set frame output mode:push"},
	{"load"     ,'l'	, _fileConfig					,"loading configure file."},
	{"config"   ,'o'	, _config   					,"set configure streaming."},
	{"\nmisc:"},
	{"keywords"	,'k'	, _keywords		,"select the IYuvSource with keywords."},
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
	///write to file
	if(fd)fwrite(frm->data,1,frm->size,fd);
	///
	dbgTestPSL(frm->index
					   <<','<<frm->data
					   <<','<<frm->size
					   <<','<<frm->iskeyframe
					   <<','<<frm->pts
	);
	dbgTestDL(frm->data,16);
	return true;
}
static bool ProcessPull(IYuvSource*src,FILE*fd){
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		auto frm=src->PullFrame(false);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);

		auto interval = 10_ms;
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IYuvSource*src,FILE*fd){
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IYuvSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming...
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		///sleep & trigger
		auto interval = 10_ms;
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessParams(IYuvSource*src) {
	if (_config) {
		return src->FromConfig(_config);
	}

	if (_fileConfig) {
		return src->LoadConfig(_fileConfig);
	}
	returnIfErrC(false,!src->SelectSensor(_sensorID));//select the sensor0.
	if(_width){
		returnIfErrC(false,!src->SetWidth(_width));
	}
	if(_height){
		returnIfErrC(false,!src->SetHeight(_height));
	}
	//src->SetFormat("422NV16");
	src->SetInterlaced(false);
	if(_ioMode){
		OmfIOMode io(_ioMode);
		returnIfErrCS(false,!io,"unknowniomode:"<<_ioMode);
		returnIfErrC(false,!src->IsSupportIOMode(io));
		returnIfErrC(false,!src->SetFrameIOMode(io));
	}
	return true;
}
static bool Process(bool _dbg){
	///////////////////////////////////////
	///create a YuvSource instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IYuvSource> src(IYuvSource::CreateNew(_keywords));
	returnIfErrC(false,!src);
	///set yuv srouce parameters
	returnIfErrC(false,!ProcessParams(src.get()));
	///open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	///get streaming parameters after Open().
	auto info = src->GetYuvMediaInfo();
	dbgTestPVL(info.width);
	dbgTestPVL(info.height);
	dbgTestPVL(info.format);
	dbgTestPVL(info.isInterlaced);
	dbgTestPVL(info.frameSize);
	///////////////////////////////////////

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
	dbgNotePSL("omfYuvSrc(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///process
	Process(OmfMain::Globle().DebugMode());
	///
	return 0;
}






