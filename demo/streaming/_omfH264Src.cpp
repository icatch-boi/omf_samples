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
#include "OmfBitRateStatistics.h"
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
static const char* _keywords="dualos-vbrc-vfrc-pull";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _framerate=30;
static int _fr_min=0;
static int _fr_max=0;
static int _bitrate=2000;///kb
static int _br_min=0;
static int _br_max=0;
static const char* _goptype = "ippp";
static int _gop = 15;
static int _triggerInterval=0;//ms
static bool _dumpFrm=true;
static int _fluency=0;
static bool _showBrc=false;
static const char* _bitRateCtrl="vbrc";
static const char* _frameRateCtrl="vfrc";
static const char* _ioMode="pull";
///
static const char* _h264Param=0;    ///the h264 other params;
static int _cache=0;    ///the vlc cache size
static int _prerecIdx=0;	///enable/disable preRecord
static int _sharedIdx=0;
///
static bool _blocking=false;
///
static const char* _config=0;
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfH264Src(...): \n"
	 "This demo shows how to get H264 streaming from OMF using IH264Source interface. \n"
     "[api]https://www.yuque.com/docs/share/bb23e87e-fc96-41a4-b20b-ebe33fed6163  \n"
	 "> omfH264Src -d5 -w1920 -h1080 --vfrc -f30 --vbrc -b128 -t ippp -g30 --pull -F \n"
     "> omfH264Src -d5 -w1920 -h1080 -f30 -b 128 -t ippp -g30 -k vbrc -F \n"
	 "> omfH264Src -d5 -w1920 -h1080 -f30 -b 5000 -t ippp -g30 -C 256 -F -n test2048.h264\n"
	 "> omfH264Src -d5 -w1920 -h1080 -f30 -b 5000 -t ippp -g30 -C 256 -F -a1 -n test2048.h264\n"
	 "> omfH264Src -d5 -a 1\n"
	 "> omfH264Src -d5 -r 1\n"
	 "> omfH264Src -d5 -r1 -k \"vbrc-vfrc\" -f10 -b2000 \n"   ///receive prerec streaming & reset prerec params.
	 "> omfH264Src -n vbrcpull.h264 -d30 -w1920 -h1080 -f30 -b 128 -t ippp -g30 \n"
	 "> omfH264Src -n vbrcpush.h264 -d30 -w1080 -h720 -k dualos-vbrc-push \n"
	 "> omfH264Src -n cbrcpush.h264 -d30 -w1920 -h1080 -k dualos-cbrc -w1920 -h1080 -f30 \n"
	 "> omfH264Src -n vbrcPushTrig.h264 -d30 -w1920 -h1080 -k dualos-vbrc-push-trigger -i 1000 \n"
     "> omfH264Src -l ./config1.txt \n"   ///load configure file config1.txt
     "> omfH264Src -o keywords=push-vbrc-vfrc,fr=10,br=500000,gop=60 \n"   ///load configure string
	},
	{"fname"		,'n'	, _fname		,"record filename(*.h264)."},
	{"duration"		,'d'	, _seconds		,"record duration(*s)."},
	{"\nthe yuv paramers:"},
	{"sid"			,'s'	, _sensorID		,"select the sensor with the id."},
	{"w"			,'w'	, _width		,"width of image."},
	{"h"			,'h'	, _height		,"height of image."},
	{"fr"			,'f'	, _framerate	,"frame number per seconds,need support framerate control."},
	{"frmin"				, _fr_min		,"min frame number per seconds,need support framerate range."},
	{"frmax"				, _fr_max		,"max frame number per seconds,need support framerate range."},
	{"\nthe h264 paramers:"},
	{"bitrate"		,'b'	, _bitrate		,"bitrate(kb) of h264 encoder,need support bitrate control."},
	{"brmin"				, _br_min		,"min bitrate(kb) of h264 encoder,need support bitrate range."},
	{"brmax"				, _br_max		,"max bitrate(kb) of h264 encoder,need support bitrate range.."},
	{"goptype"		,'t'	, _goptype		,"gop type(iiii,ippp,ibbp) of h264 encoder."},
	{"gop"			,'g'	, _gop			,"gop of h264 encoder."},
	{"h264"	    	,'c'	, _h264Param	,"the other h264 codec params."},
	{"cache"		,'C'	, _cache    	,"the vlc cache size,default is no cache."},
	{"prerec"   	,'r'	, _prerecIdx	,"set preRecord vbrc index and enable preRecord."},
	{"shared"   	,'a'	, _sharedIdx	,"set shared h264 encoder group."},
	{"brc"       			, _bitRateCtrl	,"set h264 encoder brc mode:mbrc,cbrc,vbrc,abrc"},
	{"mbrc"         		, [](){_bitRateCtrl="mbrc";}	,"set h264 encoder brc mode:mbrc"},
	{"cbrc"         		, [](){_bitRateCtrl="cbrc";}	,"set h264 encoder brc mode:cbrc"},
	{"vbrc"         		, [](){_bitRateCtrl="vbrc";}	,"set h264 encoder brc mode:vbrc"},
	{"abrc"         		, [](){_bitRateCtrl="abrc";}	,"set h264 encoder brc mode:abrc"},
	{"\nfrc"          		, _frameRateCtrl				,"set h264 encoder frc mode:vfrc,afrc,trig"},
	{"mfrc"        			, [](){_frameRateCtrl="trig";}	,"set h264 encoder frc mode:trig"},
	{"cfrc"         		, [](){_frameRateCtrl="cfrc";}	,"set h264 encoder frc mode:cfrc"},
	{"vfrc"         		, [](){_frameRateCtrl="vfrc";}	,"set h264 encoder frc mode:vfrc"},
	{"afrc"         		, [](){_frameRateCtrl="afrc";}	,"set h264 encoder frc mode:afrc"},
	{"\nio"           		, _ioMode   					,"set frame output mode:pull,push."},
	{"pull"         		, [](){_ioMode="pull";}			,"set frame output mode:pull"},
	{"push"         		, [](){_ioMode="push";}			,"set frame output mode:push"},
	{"load"     	,'l'	, _fileConfig					,"loading configure file."},
	{"config"   	,'o'	, _config   					,"set configure streaming."},
	{"\nmisc:"},
	{"dumpfrm"		,'F'	, [](){_dumpFrm=false;}			,"disable dump the frame."},
	{"interval"		,'i'	, _triggerInterval				,"the interval(ms) per trigger. Only used for IsSupportSingleFrameTrigger()."},
	{"blockfrm"		,'B'	, [](){_blocking=true;}			,"blocking to process frame."},
	{"\nbit rate control:"},
	{"showBrc"		,'S'	, [](){_showBrc=1;}				,"show the h264 source bitrate control modes."},
	{"fluency"		,'e'	, _fluency						,"[-7,7]the fluency for streaming."},
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
			//_exit=true;
			_blocking=false;
			break;
		default:
			dbgTestPSL("unknow message:"<<msg);
			break;
	}
	return true;
}
static TimePoint _tpEnd(-1_s);
static OmfBitRateStatistics _brs{2_s};
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
		_brs.PushFrame(frm->pts,frm->size);
		dbgTestPS(line << '#' << frm->pkt<< ',' << frm->index
				       << ',' << frm->pts<< '/' << Now()
		               << ',' << frm->data
		               << ',' << frm->size
		               << ',' << frm->iskeyframe
		               << ',' << _brs.GetFrameRate()<<"fps"
				       << '/' << _brs.GetBitRate()/1000<<"kbps"
						<<','
		);
		dbgTestDL(frm->data, 16);
	}
	while(_blocking)std::this_thread::sleep_for(1000_ms);
	return true;
}
static void ProcessTrigger(IH264Source*src){dbgTestPL();
	while(!_exit){
		if(src->IsSupportSingleFrameTrigger()) {
			if(src->CurrentState()==IH264Source::State::play)
				src->Trigger();
			std::this_thread::sleep_for(MilliSeconds(_triggerInterval));
		}

	}
}
static bool ProcessPull(IH264Source*src,FILE*fd){dbgTestPL();
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming....
	while(!_exit) {
		auto frm = src->PullFrame(false);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		std::this_thread::sleep_for(10_ms);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IH264Source*src,FILE*fd){dbgTestPL();
	///set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IH264Source::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	///start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	///streaming...
	while(!_exit) {
		std::this_thread::sleep_for(10_ms);
	}
	///stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool SetParams(IH264Source*src){
	std::string paras;
	dbgTestPVL(paras);
	if(!paras.empty())src->Set(paras.c_str()+1);
	return true;
}
static bool ProcessParams(IH264Source*src){
	if(_config){
		return src->FromConfig(_config);
	}

	if(_fileConfig) {
		return src->LoadConfig(_fileConfig);
	}
	///set yuv srouce parameters
	returnIfErrC(false, !src->SelectSensor(_sensorID));//select the sensor0.
	if(_width){
		returnIfErrC(false, !src->SetWidth(_width));
	}
	if(_height){
		returnIfErrC(false, !src->SetHeight(_height));
	}

	///set h264 encoder parameters
	if(_gop){
		returnIfErrC(false, !src->SetGop(_gop));
	}
	if(_goptype){
		returnIfErrC(false, !src->SetGopType(_goptype));
	}
	///set BitRateControl
	if(_bitRateCtrl){
		OmfBRCMode brc(_bitRateCtrl);
		returnIfErrCS(false,!brc,"unknown brcmode:"<<_bitRateCtrl);
		returnIfErrC(false,!src->IsSupportBRCMode(brc));
		returnIfErrC(false,!src->SetBRCMode(brc));
	}
	if(src->IsSupportBitRateControl()&&_bitrate){
		returnIfErrC(false,!src->SetBitRate(_bitrate*1000));
	}else if(src->IsSupportBitRateRange()){
		if(_br_min) returnIfErrC(false,!src->SetBitRateMinimum(_br_min*1000));
		if(_br_max) returnIfErrC(false,!src->SetBitRateMaximum(_br_max*1000));
	}
	///set FrameRateControl
	if(_frameRateCtrl){
		OmfFRCMode frc(_frameRateCtrl);
		returnIfErrCS(false,!frc,"unknown frcmode:"<<_frameRateCtrl);
		returnIfErrC(false,!src->IsSupportFRCMode(frc));
		returnIfErrC(false,!src->SetFRCMode(frc));
	}
	if(src->IsSupportFrameRateControl()&&_framerate){
		returnIfErrC(false,!src->SetFrameRate(_framerate));
	}else if(src->IsSupportFrameRateRange()){
		if(_fr_min) returnIfErrC(false,!src->SetFrameRateMinimum(_fr_min));
		if(_fr_max) returnIfErrC(false,!src->SetFrameRateMaximum(_fr_max));
	}
	///set IoMode:push or pull
	if(_ioMode){
		OmfIOMode io(_ioMode);
		returnIfErrCS(false,!io,"unknowniomode:"<<_ioMode);
		returnIfErrC(false,!src->IsSupportIOMode(io));
		returnIfErrC(false,!src->SetFrameIOMode(io));
	}
	if(_h264Param){
		returnIfErrC(false,!src->SetCodec(_h264Param));
	}
	if(src->IsSupportPreRecord()){
		returnIfErrC(false,!src->SetPreRecordGroup(_prerecIdx));
	}
	if(src->IsSupportSharedEncoder()){
		returnIfErrC(false,!src->SetSharedEncoderGroup(_sharedIdx));
	}
	if(src->IsSupportCache()){
		returnIfErrC(false,!src->SetCache(_cache));
	}
	if(_fluency){
		returnIfErrC(false,!src->SetFluency(_fluency));
	}
	///streaming params set
	SetParams(src);
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	///create a h264Source instance with keywords.
	std::unique_ptr<IH264Source> src(IH264Source::CreateNew(_keywords));
	returnIfErrC(false,!src);
	src->RegisterMessageCallback(&MessageProcess);
	///
	if(_showBrc){
		dbgTestPSL("support brcs:");
		auto brcs=src->GetSupportedBRCModes();
		for(auto&brc:brcs){
			dbgTestPSL(to_string(brc));
		}
		auto brc=src->GetCurrentBitRate();
		dbgTestPSL("current brc mode:"<<brc);

		dbgTestPSL("support frcs:");
		auto frcs=src->GetSupportedFRCModes();
		for(auto&frc:frcs){
			dbgTestPSL(to_string(frc));
		}
		auto frc=src->GetCurrentFrameRate();
		dbgTestPSL("current frc mode:"<<frc);
	}
	///
	returnIfErrC(false,!ProcessParams(src.get()));
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
	FILE* fd=0;
	if(_fname) {
		fd=fopen(_fname, "wb");
		if (!fd) {
			dbgErrPSL("open file fail:" << _fname);
		}
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	///streaming......
	_exit = false;
	///trigger
	std::unique_ptr<std::thread> thread;
	if(_triggerInterval && src->IsSupportSingleFrameTrigger()) {
		thread.reset(new std::thread(&ProcessTrigger,src.get()));
	}
	///process frame
	if(src->IsSupportPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd));
	}else if(src->IsSupportOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush(src.get(),fd));
	}else{
		dbgErrPSL("null support output mode.");
	}
	returnIfErrC(false,!src->ChangeDown(State::null));
	dbgTestPVL(src.get());
	return true;
}
////////////////////////////////////////////
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfH264Src(...)\n");
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






