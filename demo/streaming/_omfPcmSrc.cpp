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
#include "IPcmSource.h"
////////////////////////////////////////////////////////////
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
static int _seconds=30;
static const char* _keywords="dualos";
static const char* _mic = "dualos";
static int _triggerInterval=100;//seconds
static bool _aec=0;
static const char* _aecpara="keys=webrtc,level=2,samples=1600";
static bool _ans=0;
static const char* _anspara="keys=webrtc,ansmode=3,samples=1600";
static int _rate = 16000;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfPcmSrc(...): \n"
	 "This demo shows how to get PCM streaming from OMF using IPcmSource interface.\n"
	 "  omfPcmSrc -n test.pcm -d 30\n"
	 "  omfPcmSrc -n test.pcm -d 30 -e 1 -N 1\n"
	 "  omfPcmSrc -n test.pcm -d 30 -e 1 -p keys=webrtc,level=2,samples=1600\n"
	 "  omfPcmSrc -n test.pcm -d 30 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	 "  omfPcmSrc -n test.pcm -d 30 -e 1 -p keys=webrtc,level=2,samples=1600 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	},
	{"fname",'n', _fname		,"record filename(*.pcm)."},
	{"duration",'d', _seconds	,"process execute duration(*s)."},
	{"mic"		,'m', _mic		,"select the mic with the keywords.Usually use the default values."},
	{"keywords",'k', _keywords	,"select the IPcmSource with keywords.Usually use the default values."},
	{"samplerate",'r', _rate	,"set the audio samplerate:eg.. rate=16000"},
	{"\naec:"},
	{"aec",'e', _aec		,"set the pcm aec enable"},
	{"aecpara",'p', _aecpara	,"aec params.eg.. keys=webrtc,level=2,samples=1600"},
	{"\nans:"},
	{"ans",'N', _ans		,"set the pcm ans enable"},
	{"anspara",'s', _anspara	,"ans params.eg.. keys=webrtc,ansmode=3,samples=1600"},
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
static bool ProcessPull(IPcmSource*src,FILE*fd){
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		std::shared_ptr<IPcmSource::frame_t> frm;
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
		//if(src->IsSupportSingleFrameTrigger()) {
		//	interval = Seconds(_triggerInterval);
		//	src->Trigger();
		//}
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IPcmSource*src,FILE*fd){
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IPcmSource::frame_t>&frm){
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
		//if(src->IsSupportSingleFrameTrigger()) {
		//	interval = Seconds(_triggerInterval);
		//	src->Trigger();
		//}
		std::this_thread::sleep_for(interval);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool Process(bool _dbg){
	///////////////////////////////////////
	//create a PcmSource instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IPcmSource> src(IPcmSource::CreateNew(_keywords));
	returnIfErrC(false,!src);
	//set pcm srouce parameters
	//src->SelectMicrophone(_mic);//select the MIC.
	src->SetSampleRate(_rate);//set the samplerate.
	src->SetAEC(_aec,_aecpara);
	src->SetAEC(_ans,_anspara);
	//open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetPcmMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.isSigned);
	dbgTestPVL(info.bitWidth);
	dbgTestPVL(info.byteWidth());
	dbgTestPVL(info.bytePerSample());

	////////////////////////////////////////////////////////
	FILE* fd=0;
	if(_fname) {
		fd = fopen(_fname, "wb");
		if (!fd) {
			dbgErrPSL("open file fail:" << _fname);
		}
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	//streaming......
	//ProcessMic(src.get());
	if(src->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd));
	}else if(src->IsSupportedOutputFrameCallback()){
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
	dbgNotePSL("omfPcmSrc(...)\n");
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

