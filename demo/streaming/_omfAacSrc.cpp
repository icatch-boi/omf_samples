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
static const char* _keywords="dualos-push";
static int _bitrate=128;///KB
static int _channels=1;
static int _triggerInterval=100;//seconds
static bool _aec=0;
static const char* _aecpara="keys=webrtc,level=2,samples=1600";
static bool _ans=0;
static const char* _anspara="keys=webrtc,ansmode=3,samples=1600";
static bool _dumpFrm=true;
static int _cache=0;    ///the vlc cache size
static int _prerecIdx=0;	///enable/disable preRecord
static int _sharedIdx=0;
static const char* _ioMode="pull";
static const char* _aacParam=0;    ///the aac other params;
///
static const char* _config=0;
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfAacSrc(...): \n"
  	"This demo shows how to get AAC streaming from OMF using IAacSource interface.\n"
   	"[api]https://www.yuque.com/docs/share/f5aa1b98-767e-477d-abf1-b3e950696c39\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -c 1\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -c 1 -a1\n"
	"> omfAacSrc -n test.aac -d30 -b 128\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -e 1 -N 1\n"
	"> omfAacSrc -d30 -a 1\n"
	"> omfAacSrc -d30 -r 1\n"
	"> omfAacSrc -d30 -r 1 --push\n"
	"> omfAacSrc -d30 -r 1 --pull\n"
	"> omfAacSrc -d30 -C 2048\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -e 1 -p keys=webrtc,level=2,samples=1600\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	"> omfAacSrc -n test.aac -d30 -b 128 -e 1 -p keys=webrtc,level=2,samples=1600 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	},
	{"fname"	,'n'	, _fname		,"set the record filename(*.aac)."},
	{"duration"	,'d'	, _seconds		,"set process execute duration(*s)."},
	{"\naac codec paramters:"},
	{"bitrate"	,'b'	, _bitrate		,"set the bitrate(kb) to aac codec."},
	{"channels"	,'c'	, _channels		,"set the target channels to aac codec.0:follow the channels of pcm src,other is force to rechannels."},
	{"cache"	,'C'	, _cache    	,"the vlc cache size,default is no cache."},
	{"prerec"   ,'r'	, _prerecIdx	,"set preRecord vbrc index and enable preRecord."},
	{"shared"   ,'a'	, _sharedIdx	,"set shared aac encoder group."},
	{"\naec:"}	,
	{"aec"		,'e'	,_aec			,"set the pcm aec enable"},
	{"aecpara"	,'p'	, _aecpara		,"aec params.eg.. keys=webrtc,level=2,samples=1600"},
	{"\nans:"}	,
	{"ans"		,'N'	, _ans			,"set the pcm ans enable"},
	{"anspara"	,'s'	, _anspara		,"ans params.eg.. keys=webrtc,ansmode=3,samples=1600"},
	{"\nio"       		, _ioMode   	,"set frame output mode:pull,push."},
	{"pull"     		, [](){_ioMode="pull";},"set frame output mode:pull"},
	{"push"     		, [](){_ioMode="push";},"set frame output mode:push"},
	{"load"     ,'l'	, _fileConfig	,"loading configure file."},
	{"config"   ,'o'	, _config   	,"set configure streaming."},
	{"\nmisc:"}	,
	{"dumpfrm"	,'F'	, [](){_dumpFrm=false;}	,"dump the frame."},
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
		dbgTestPS(line << '#' << frm->pkt<< ',' << frm->index
				       << ',' << frm->pts<< '/' << Now()
		               << ',' << frm->data
		               << ',' << frm->size
		               << ',' << frm->iskeyframe
		               << ','
		);
		dbgTestDL(frm->data, 8);
	}
	return true;
}
static bool ProcessPull(IAacSource*src,FILE*fd){
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		auto frm = src->PullFrame(true);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		std::this_thread::sleep_for(10_ms);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IAacSource*src,FILE*fd){
	///set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IAacSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming...
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		///sleep & trigger
		auto interval = 10_ms;
		std::this_thread::sleep_for(interval);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool SetParams(IAacSource*src){
	std::string paras;
	dbgTestPVL(paras);
	if(!paras.empty())src->Set(paras.c_str()+1);
	return true;
}
static bool ProcessParams(IAacSource*src){
	if(_config){
		return src->FromConfig(_config);
	}

	if(_fileConfig) {
		return src->LoadConfig(_fileConfig);
	}
	///set aac codec
	returnIfErrC(false,!src->SetProfile("LC"));

	if(_bitrate){
		returnIfErrC(false,!src->SetBitRate(_bitrate*1000));
	}

	if(_channels){
		returnIfErrC(false,!src->SetTargetChannels(_channels));//encode to 1 channels.
	}
	///
	if(_ioMode){
		OmfIOMode io(_ioMode);
		returnIfErrCS(false,!io,"unknowniomode:"<<_ioMode);
		returnIfErrC(false,!src->IsSupportIOMode(io));
		returnIfErrC(false,!src->SetFrameIOMode(io));
	}
	/////set aec
	if(src->IsSupportAEC()){
		returnIfErrC(false,!src->SetAEC(_aec,_aecpara));
	}
	///set ans
	if(src->IsSupportANS()){
		returnIfErrC(false,!src->SetANS(_ans,_anspara));
	}
	///set prerec
	if(src->IsSupportPreRecord()){
		returnIfErrC(false,!src->SetPreRecordGroup(_prerecIdx));
	}
	///set share encoder
	if(src->IsSupportSharedEncoder()){
		returnIfErrC(false,!src->SetSharedEncoderGroup(_sharedIdx));
	}
	///set linux cache
	if(src->IsSupportCache()){
		returnIfErrC(false,!src->SetCache(_cache));
	}
	SetParams(src);
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	std::unique_ptr<IAacSource> src(IAacSource::CreateNew(_keywords));
	returnIfErrC(false,!src);
	src->RegisterMessageCallback(&MessageProcess);
	returnIfErrC(false,!ProcessParams(src.get()));
	///open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	///get streaming parameters after Open().
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
	dbgNotePSL("omfAacSrc(...)\n");
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






