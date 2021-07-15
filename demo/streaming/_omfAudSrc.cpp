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
#include "IAudioSource.h"
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
static const char* _codec=0;
static bool _aec=0;
static const char* _aecpara="keys=webrtc,level=2,samples=1600";
static bool _ans=0;
static const char* _anspara="keys=webrtc,ansmode=3,samples=1600";
static int _rate = 16000;
static bool _dumpFrm=true;
static const char* _ioMode="pull";
static int _prerecIdx=0;	///enable/disable preRecord
static int _cache=0;    ///the cache size
///
static const char* _config=0;
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _exit = false;
static bool _writeFrameSize= false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfAudSrc(...): \n"
	 "This demo shows how to get audio streaming from OMF using IAudioSource interface.\n"
  	 "[api]https://www.yuque.com/docs/share/0d4eb2ad-f3dc-4eca-ab3c-151a775f6ef6? \n"
	 "> omfAudSrc -n test.pcm -d 30 -r 16000\n"
	 "> omfAudSrc -n test.pcm -d 30 -r 16000 --pull\n"
	 "> omfAudSrc -n test.pcm -d 30 -r 16000 --push\n"
	 "> omfAudSrc -n test.pcm -d 30 -r 16000 -e 1 -N 1\n"
	 "> omfAudSrc -n test.aac -d 30 -c aac:br=128000\n"
	 "> omfAudSrc -n test.alaw -d 30 -c alaw\n"
	 "> omfAudSrc -n test.ulaw -d 30 -c ulaw\n"
	 "> omfAudSrc -n test.g722 -d 30 -c g722\n"
     ">	omfAudSrc -n test.opus  -d 10 -c opus\n"
	 "> omfAudSrc -n test.pcm -d 30 -e 1 -p keys=webrtc,level=2,samples=1600\n"
	 "> omfAudSrc -n test.pcm -d 30 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	 "> omfAudSrc -n test.pcm -d 30 -e 1 -p keys=webrtc,level=2,samples=1600 -N 1 -s keys=webrtc,ansmode=3,samples=1600\n"
	},
	{"fname"			,'n'	, _fname	,"record filename(*.pcm)."},
	{"duration"			,'d'	, _seconds	,"process execute duration(*s)."},
	{"keywords"			,'k'	, _keywords	,"select the IAudioSource with keywords.Usually use the default values."},
	{"codec"			,'c'	, _codec	,"set the audio codec:eg.. aac:br=128000"},
	{"samplerate"		,'r'	, _rate		,"set the audio samplerate:eg.. aac:rate=16000"},
	{"prerec"   		,'R'	, _prerecIdx	,"set preRecord vbrc index and enable preRecord."},
	{"cache"			,'C'	, _cache    	,"the vlc cache size,default is no cache."},
	{"\naec:"},
	{"aec"				,'e'	, _aec		,"set the pcm aec enable"},
	{"aecpara"			,'p'	, _aecpara	,"aec params.eg.. keys=webrtc,level=2,samples=1600"},
	{"\nans:"},
	{"ans"				,'N'	, _ans		,"set the pcm ans enable"},
	{"anspara"			,'s'	, _anspara	,"ans params.eg.. keys=webrtc,ansmode=3,samples=1600"},
	{"\nio"       				, _ioMode   	,"set frame output mode:pull,push."},
	{"pull"     				, [](){_ioMode="pull";},"set frame output mode:pull"},
	{"push"     				, [](){_ioMode="push";},"set frame output mode:push"},
	{"load"     		,'l'	, _fileConfig	,"loading configure file."},
	{"config"   		,'o'	, _config   	,"set configure streaming."},
	{"dumpfrm"			,'F'	, [](){_dumpFrm=false;}	,"dump the frame."},
	{"WriteFrameSize"	,'S'	, [](){_writeFrameSize=true;}	,"write frame size before frame data."},
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
static TimePoint _tpEnd(_timepoint_min);
static bool ProcessFrame(std::shared_ptr<frame_t> frm,FILE*fd,int line){
	if(!frm->data || !frm->size)
		return false;
	///
	if(_tpEnd==_timepoint_min){
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
static bool ProcessPull(IAudioSource*src,FILE*fd){
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming....
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		std::shared_ptr<IAudioSource::frame_t> frm;
		returnIfErrCS(false, !src->PullFrame(frm), "pull frame fail!");
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		std::this_thread::sleep_for(10_ms);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IAudioSource*src,FILE*fd){
	///set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IAudioSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming...
	auto end = Now()+Seconds(_seconds);
	while(!_exit && Now()<end) {
		std::this_thread::sleep_for(10_ms);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessParams(IAudioSource*src){
	if(_config){
		return src->FromConfig(_config);
	}

	if(_fileConfig) {
		return src->LoadConfig(_fileConfig);
	}
	if(_codec) {
		returnIfErrC(false, !src->SetCodec(_codec));
	}
	if(_rate) {
		returnIfErrC(false, !src->SetSampleRate(_rate));
	}

	if(src->IsSupportAEC()) {
		returnIfErrC(false, !src->SetAEC(_aec, _aecpara));
	}
	if(src->IsSupportANS()) {
		returnIfErrC(false, !src->SetANS(_ans, _anspara));
	}
	if(src->IsSupportPreRecord()){
		returnIfErrC(false, !src->SetPreRecordGroup(_prerecIdx));
	}
	if(src->IsSupportCache()){
		returnIfErrC(false, !src->SetCache(_cache));
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
static bool Process(){dbgTestPL();
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	///create a IAudioSource instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IAudioSource> src(IAudioSource::CreateNew(_keywords));
	returnIfErrC(false,!src);
	///set audio srouce parameters
	returnIfErrC(false,!ProcessParams(src.get()));
	///open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	///get streaming parameters after Open().
	auto info = src->GetAudioMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.media);

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
	if(!_codec && _fname) {
		auto url = std::string("file://") + _fname;
		OmfAttrSet ap(url);
		returnIfErrC(false, !ap);
		auto ext = ap.Get("ext");
		returnIfErrC(false, !ext);
		_codec = _fname + strlen(_fname) - strlen(ext);
	}
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfAudSrc(...)\n");
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

