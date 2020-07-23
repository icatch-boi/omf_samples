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
#include "IAacSource.h"
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
static int _seconds=30;
static const char* _keywords="dualos";
static const char* _mic = "dualos";
static int _bitrate=128;///KB
static int _channels=0;
static int _triggerInterval=100;//seconds
static int _aec = 0;
static const char* _aecpara="level=2,ansmode=3";
static bool _dumpFrm=true;
static int _cache=0;    ///the vlc cache size
static int _prerecIdx=0;	///enable/disable preRecord
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfAacSrc(...): \n"
  	"This demo shows how to get AAC streaming from OMF using IAacSource interface."
	 "> omfAacSrc -n test.aac -d30 -b 128 -c 1\n"
	 "> omfAacSrc -n test.aac -d30 -b 128\n"
	 "> omfAacSrc -d30 -r 1\n"
	 "> omfAacSrc -d30 -C 2048\n"
	},
	{"fname",'n', _fname	,"set the record filename(*.aac)."},
	{"duration",'d', _seconds	,"set process execute duration(*s)."},
	{"\naac codec paramters:"},
	{"bitrate",'b', _bitrate	,"set the bitrate(kb) to aac codec."},
	{"channels",'c', _channels	,"set the target channels to aac codec.0:follow the channels of pcm src,other is force to rechannels."},
	{"cache"	,'C', _cache    ,"the vlc cache size,default is no cache."},
	{"prerec"   ,'r', _prerecIdx,"set preRecord vbrc index and enable preRecord."},
	{"\nmisc:"},
	{"dumpfrm"	,'F', [](){_dumpFrm=false;}	,"dump the frame."},
	{"mic"		,'m', _mic	,"select the mic with the keywords.Usually use the default values."},
	{"keywords",'k', _keywords	,"select the IAacSource with keywords.Usually use the default values."},
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
	if(_dumpFrm) {
		dbgTestPS(line << '#' << frm->index
		               << ',' << frm->data
		               << ',' << frm->size
		               << ',' << frm->iskeyframe
		               << ',' << frm->pts
		               << ','
		);
		dbgTestDL(frm->data, 8);
	}
	return true;
}
static bool ProcessPull(IAacSource*src,FILE*fd){
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		auto frm = src->PullFrame(true);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		std::this_thread::sleep_for(10_ms);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IAacSource*src,FILE*fd){
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IAacSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
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
static bool SetParams(IAacSource*src){
	std::string paras;
	if(_cache){
		paras+=",cache=";
		paras+=_cache;//dbgTestPVL(_cache);dbgTestPVL(paras);
	}
	if(_prerecIdx){
		paras+=",prerec=";
		paras+=_prerecIdx;
	}
	dbgTestPVL(paras);
	if(!paras.empty())src->Set(paras.c_str()+1);
	return true;
}
static bool Process(bool _dbg){
	///////////////////////////////////////
	//create a AacSource instance with keywords.
	std::string keywords=_keywords;
	if(_prerecIdx)keywords="prerec-"+keywords;
	dbgTestPVL(keywords);
	std::unique_ptr<IAacSource> src(IAacSource::CreateNew(keywords.c_str()));
	returnIfErrC(false,!src);
	src->RegisterMessageCallback(&MessageProcess);
	//set pcm srouce parameters
	//src->SelectMicrophone(_mic);//select the MIC.
	//set aec
	//src->SetAEC(_aec,_aecpara);
	///set aac codec
	src->SetBitRate(_bitrate*1000);
	src->SetProfile("LC");
	src->SetTargetChannels(_channels);//encode to 1 channels.
	src->SetAEC(_aec,_aecpara);
	SetParams(src.get());
	//open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	//get streaming parameters after Open().
	auto info = src->GetAacMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.hasADTS);
	dbgTestPVL(info.profile);
	dbgTestPVL(info.version);
	dbgTestPVL(info.rateidx);
	dbgTestPVL(info.adtsLength);
	dbgTestPVL(info.bitrate);
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
	//streaming......
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
	dbgNotePSL("omfAacSrc(...)\n");
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






