//
// Created by jg.wang on 2020/6/15.
//

#include <thread>
#include <memory>
#include <atomic>
#include "_call.h"
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "IH264Source.h"
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
using ISource=omf::api::streaming::common::IStreamSource;
////////////////////////////////////////////////////////////
static const char* _fname=0;
static int _seconds=30;//seconds
static const char* _keywords="dualos-pull-prerec";
static int _dumpFrm=16;
///
static int _cache=0;    ///the vlc cache size
static int _prerecIdx=1;	///enable/disable preRecord
///
static int _sleep_ms=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfKvspbSimplest(...): \n"
		 "This demo shows how to get H264/aac streaming with PreRecord from OMF using IH264Source/IAacSource interface. \n"
		 "  omfKvspbSimplest -d5 -C 1024\n"
		 "  omfKvspbSimplest -d5 -C 1024 -n test -F 0\n"
		},
		{"fname"	,'n', _fname	,"record filename(*)."},
		{"duration"	,'d', _seconds	,"record duration(*s)."},
		{"cache"	,'C', _cache    ,"the vlc cache size,default is no cache."},
		{"prerec"   ,'r', _prerecIdx,"set preRecord vbrc index and enable preRecord."},
		{"dumpfrm"	,'F', _dumpFrm	,"dump the frame data of length."},
		{"push"     ,'s', [](){_keywords="dualos-push-prerec";}	,"push mode."},
		{"sleep"    ,'m', _sleep_ms	,"process frame sleep * ms."},
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
		case Hash("warning"):
			dbgNotePSL("warning");
			dbgNotePSL("cache is full:"<<_cache);
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
		dbgNotePSL("the 1st frame:"<<frm->pts);
	}else if(frm->pts>_tpEnd){
		///frame receive is enough, exit profram.
		dbgNotePSL("frame is enough:"<<Seconds(_seconds));
		_exit = true;
		return false;
	}
	///write to file
	if(fd)fwrite(frm->data,1,frm->size,fd);
	///
	if(_dumpFrm) {
		dbgTestPS(line << '#' << frm->pts
				       << ',' << frm->index
		               << ',' << frm->data
		               << ',' << frm->size
		               << ',' << frm->iskeyframe
		               << '#'
		);
		dbgTestDL(frm->data, _dumpFrm);
	}
	///
	if(_sleep_ms){
		std::this_thread::sleep_for(MilliSeconds(_sleep_ms));
	}
	return true;
}
static std::atomic_int _exit_pull_cnt{0};
static bool ProcessPull(ISource* src,FILE*fd){dbgTestPL();
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	std::thread thread([src,fd](){
		_exit_pull_cnt++;
		while(!_exit) {
			auto frm = src->PullFrame(false);
			if(frm)
				ProcessFrame(frm,fd,__LINE__);
			std::this_thread::sleep_for(10_ms);
		}
		_exit_pull_cnt--;
	});
	thread.detach();
	return true;
}
static bool ProcessPush(ISource* src,FILE*fd){dbgTestPL();
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IH264Source::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	return true;
}
static bool SetParams(ISource*src){
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
static std::shared_ptr<IH264Source> CreateH264Src(){
	std::string keywords=_keywords;
	dbgTestPVL(keywords);
	std::shared_ptr<IH264Source> src(IH264Source::CreateNew(keywords.c_str()));
	returnIfErrC(nullptr,!src);
	src->RegisterMessageCallback(&MessageProcess);
	///streaming params set
	SetParams(src.get());
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
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

	return src;
}
static std::shared_ptr<IAacSource> CreateAacSrc(){
	std::string keywords=_keywords;
	dbgTestPVL(keywords);
	std::shared_ptr<IAacSource> src(IAacSource::CreateNew(keywords.c_str()));
	returnIfErrC(nullptr,!src);
	src->RegisterMessageCallback(&MessageProcess);
	///streaming params set
	SetParams(src.get());
	//open streaming
	returnIfErrC(nullptr,!src->ChangeUp(State::ready));
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

	return src;
}
static bool Process(){
	auto h264 =CreateH264Src();
	returnIfErrC(false,!h264);
	auto aac =CreateAacSrc();
	returnIfErrC(false,!aac);
	///////////////////////////////////////
	FILE* fdh264=0;
	FILE* fdaac=0;
	if(_fname) {
		auto h264fname=(std::string)_fname+".h264";
		fdh264=fopen(h264fname.c_str(), "wb");
		if (!fdh264) {
			dbgErrPSL("open file fail:" << h264fname);
		}

		///
		auto aacfname=(std::string)_fname+".aac";
		fdaac=fopen(aacfname.c_str(), "wb");
		if (!fdaac) {
			dbgErrPSL("open file fail:" << aacfname);
		}
	}
	ExitCall ecfd([fdh264,fdaac](){
		if(fdh264)fclose(fdh264);
		if(fdaac)fclose(fdaac);
	});
	//////////////////////////////////
	///streaming......
	_exit = false;
	///process frame
	///h264
	if(h264->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull(h264.get(),fdh264));
	}else if(h264->IsSupportedOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush(h264.get(),fdh264));
	}else{
		dbgErrPSL("null support output mode.");
	}
	///aac
	if(aac->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull( aac.get(),fdaac));
	}else if(aac->IsSupportedOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush( aac.get(),fdaac));
	}else{
		dbgErrPSL("null support output mode.");
	}
	///streaming....
	while(!_exit || (_exit_pull_cnt.load()>0))std::this_thread::sleep_for(1_ms);;
	//stop streaming
	returnIfErrC(false,!h264->ChangeDown(State::null));
	returnIfErrC(false,!aac->ChangeDown(State::null));
	//dbgTestPVL(h264.get());
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfKvspbSimplest(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///process
	Process();
	///
	return 0;
}






