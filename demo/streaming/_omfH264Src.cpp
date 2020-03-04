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
#include "IH264Source.h"
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
static const char* _keywords="dualos-vbrc-pull";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _framerate=30;
static int _bitrate=2000;///kb
static const char* _goptype = "ippp";
static int _gop = 15;
static int _triggerInterval=100;//ms
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfH264Src(...): \n"
	 "This demo shows how to get H264 streaming from OMF using IH264Source interface. \n"
	 "> omfH264Src -n vbrcpull.h264 -d30 -w1920 -h1080 -f30 -b 128 -t ippp -g30 \n"
	 "> omfH264Src -n vbrcpush.h264 -d30 -w1080 -h720 -k dualos-vbrc-push \n"
	 "> omfH264Src -n cbrcpush.h264 -d30 -w1920 -h1080 -k dualos-cbrc -w1920 -h1080 -f30 \n"
	 "> omfH264Src -n vbrcPushTrig.h264 -d30 -w1920 -h1080 -k dualos-vbrc-push-trigger -i 1000 \n"
	},
	{"fname"	,'n', _fname	,"record filename(*.aac)."},
	{"duration"	,'d', _seconds	,"record duration(*s)."},
	{"the yuv paramers:"},
	{"sid"		,'s', _sensorID	,"select the sensor with the id."},
	{"w"		,'w', _width	,"width of image."},
	{"h"		,'h', _height	,"height of image."},
	{"fr"		,'f', _framerate,"frame number per seconds."},
	{"the h264 paramers:"},
	{"bitrate"	,'b', _bitrate	,"bitrate(kb) of h264 encoder."},
	{"goptype"	,'t', _goptype	,"gop type(iiii,ippp,ibbp) of h264 encoder."},
	{"gop"		,'g', _gop		,"gop of h264 encoder."},
	{"misc:"},
	{"interval"	,'i', _triggerInterval	,"the interval(ms) per trigger. Only used for IsSupportSingleFrameTrigger()."},
	{"keywords"	,'k', _keywords	,"select the IH264Source with keywords:"
							  	"\n <module>-<brc>-<output>-<tigger>,eg.."
		 						"\n dualos-cbrc"
		 						"\n dualos-cbrc-pull"
		 						"\n dualos-cbrc-pull-trigger"
		 						"\n dualos-cbrc-push-trigger"
								"\n dualos-vbrc"
								"\n dualos-vbrc-pull"
								"\n dualos-vbrc-pull-trigger"
								"\n dualos-vbrc-push-trigger"
   },
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
static bool ProcessPull(IH264Source*src,FILE*fd){dbgTestPL();
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		std::shared_ptr<frame_t> frm;
		returnIfErrCS(false, !src->PullFrame(frm), "pull frame fail!");
		if(!frm->data || !frm->size)
			continue;
		///
		dbgTestPSL(frm->index
						   <<','<<frm->data
						   <<','<<frm->size
						   <<','<<frm->iskeyframe
						   <<','<<frm->pts
		);
		dbgTestDL(frm->data,16);
		///write to file
		if(fd)fwrite(frm->data,1,frm->size,fd);
		///sleep & trigger
		auto interval = 10_ms;
		if(src->IsSupportSingleFrameTrigger()) {
			interval = MilliSeconds(_triggerInterval);
			src->Trigger();
		}
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IH264Source*src,FILE*fd){dbgTestPL();
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IH264Source::frame_t>&frm){
		dbgTestPSL(frm->index
						   <<','<<frm->data
						   <<','<<frm->size
						   <<','<<frm->iskeyframe
						   <<','<<frm->pts
		);
		dbgTestDL(frm->data,16);
		///
		if(fd)fwrite(frm->data,1,frm->size,fd);
		return true;
	});
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming...
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		///sleep & trigger
		auto interval = 10_ms;
		if(src->IsSupportSingleFrameTrigger()) {
			interval = MilliSeconds(_triggerInterval);
			src->Trigger();
		}
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool Process(bool _dbg){
	///////////////////////////////////////
	//create a h264Source instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IH264Source> src(IH264Source::CreateNew(_keywords));
	returnIfErrC(false,!src);
	//set yuv srouce parameters
	src->SelectSensor(_sensorID);//select the sensor0.
	src->SetWidth(_width);
	src->SetHeight(_height);
	//set h264 encoder parameters
	src->SetGop(_gop);
	src->SetGopType(_goptype);
	//set BitRateControl
	src->SetBitRate(_bitrate*1000);
	//open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetH264MediaInfo();
	dbgTestPVL(info.width);
	dbgTestPVL(info.height);
	dbgTestPVL(info.gop);
	dbgTestPVL(info.gopType);
	dbgTestPVL(info.sps);
	dbgTestPVL(info.spsLength);
	dbgTestPVL(info.pps);
	dbgTestPVL(info.ppsLength);
	dbgTestPVL(info.iframe.spsOffset);
	dbgTestPVL(info.iframe.spsLenght);
	dbgTestPVL(info.iframe.ppsOffset);
	dbgTestPVL(info.iframe.ppsLenght);
	dbgTestPVL(info.iframe.dataOffset);
	dbgTestPVL(info.pframe.dataOffset);
	dbgTestPVL((void*)info.extraData);
	dbgTestPVL(info.extraSize);
	dbgTestDL(info.extraData,info.extraSize);
	dbgTestPL();
	///////////////////////////////////////
	FILE* fd=fopen(_fname,"wb");
	if(!fd){
		dbgErrPSL("open file fail:"<<_fname);
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	//streaming......
	if(src->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd));
	}else if(src->IsSupportedOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush(src.get(),fd));
	}else{
		dbgErrPSL("null support output mode.");
	}
	return true;
}
////////////////////////////////////////////
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfH264Src(...)\n");
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






