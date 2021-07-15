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
static const char* _keywords="dualos-pull";
static int _sensorID = 0;
static int _width=1920;
static int _height=1080;
static int _framerate=0;
static int _bitrate=2000;///kb
static const char* _goptype = "ippp";
static int _gop = 15;
static int _triggerInterval=0;//ms
static bool _dumpFrm=true;
static int _fluency=0;
static bool _showBrc=false;
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
     "  omfH264Src -d5 -w1920 -h1080 -f30 -b 128 -t ippp -g30 -k vbrc -F \n"
	 "  omfH264Src -d5 -w1920 -h1080 -f30 -b 5000 -t ippp -g30 -C 256 -F -n test2048.h264\n"
	 "  omfH264Src -d5 -w1920 -h1080 -f30 -b 5000 -t ippp -g30 -C 256 -F -a1 -n test2048.h264\n"
	 "  omfH264Src -d5 -a 1\n"
	 "  omfH264Src -d5 -r 1\n"
	 "  omfH264Src -d5 -r1 -k \"vbrc-vfrc\" -f10 -b2000 \n"   ///receive prerec streaming & reset prerec params.
	 "  omfH264Src -n vbrcpull.h264 -d30 -w1920 -h1080 -f30 -b 128 -t ippp -g30 \n"
	 "  omfH264Src -n vbrcpush.h264 -d30 -w1080 -h720 -k dualos-vbrc-push \n"
	 "  omfH264Src -n cbrcpush.h264 -d30 -w1920 -h1080 -k dualos-cbrc -w1920 -h1080 -f30 \n"
	 "  omfH264Src -n vbrcPushTrig.h264 -d30 -w1920 -h1080 -k dualos-vbrc-push-trigger -i 1000 \n"
	 "  omfH264Src -d30 -w1920 -h1080 -k dualos-vbrc-push -i 1000 -c roi0={0,0,0,0,0,0,0},roi1={1,1,14,11,105,56,0},roi2={2,1,13,10,106,57,1},roi3={3,1,12,9,107,58,2},roi4={4,1,10,8,109,59,3},roi5={5,1,8,6,111,61,4},roi6={6,1,4,3,115,64,5},roi7={7,1,0,0,119,67,6} \n"
	 "  omfH264Src -d30 -w1280 -h720 -k dualos-vbrc-push -i 1000 -c roi0={0,0,0,0,0,0,0},roi1={1,1,9,7,69,36,0},roi2={2,1,8,6,70,37,1},roi3={3,1,7,6,71,38,2},roi4={4,1,6,5,72,39,3},roi5={5,1,5,4,74,40,4},roi6={6,1,2,2,76,42,5},roi7={7,1,0,0,79,44,6} \n"
     "  omfH264Src -l ./config1.txt \n"   ///load configure file config1.txt
     "  omfH264Src -o keywords=push-vbrc-vfrc,fr=10,br=500000,gop=60 \n"   ///load configure string
	},
	{"fname"	,'n', _fname	,"record filename(*.h264)."},
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
	{"h264"	    ,'c', _h264Param,"the other h264 codec params."},
	{"cache"	,'C', _cache    ,"the vlc cache size,default is no cache."},
	{"prerec"   ,'r', _prerecIdx,"set preRecord vbrc index and enable preRecord."},
	{"shared"   ,'a', _sharedIdx,"set shared h264 encoder group."},
	{"load"     ,'l', _fileConfig,"loading configure file."},
	{"config"   ,'o', _config   ,"set configure streaming."},
	{"misc:"},
	{"dumpfrm"	,'F', [](){_dumpFrm=false;}	,"disable dump the frame."},
	{"interval"	,'i', _triggerInterval	,"the interval(ms) per trigger. Only used for IsSupportSingleFrameTrigger()."},
	{"blockfrm"	,'B', [](){_blocking=true;}	,"blocking to process frame."},
	{"bit rate control:"},
	{"showBrc"	,'S', [](){_showBrc=1;}	,"show the h264 source bitrate control modes."},
	{"fluency"	,'e', _fluency	,"[-7,7]the fluency for streaming."},
	{"keywords"	,'k', _keywords	,"select the IH264Source with keywords:"
							  	"\n <module>-<brc>-<output>-<tigger>,eg.."
		 						"\n dualos-cbrc"
		 						"\n dualos-cbrc-pull"
								"\n dualos-cbrc-push"
		 						"\n dualos-cbrc-pull-trig"
		 						"\n dualos-cbrc-push-trig"
								"\n dualos-vbrc"
								"\n dualos-vbrc-pull"
								"\n dualos-vbrc-push"
								"\n dualos-vbrc-pull-trig"
								"\n dualos-vbrc-push-trig"
								"\n dualos-dbrc"
								"\n dualos-dbrc-pull"
								"\n dualos-dbrc-push"
								"\n dualos-dbrc-pull-trig"
								"\n dualos-dbrc-push-trig"
								"\n dualos-abrc"
								"\n dualos-abrc-pull"
								"\n dualos-abrc-push"
								"\n dualos-abrc-pull-trig"
								"\n dualos-abrc-push-trig"
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
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	while(!_exit) {
		auto frm = src->PullFrame(false);
		if(frm)
			ProcessFrame(frm,fd,__LINE__);
		std::this_thread::sleep_for(10_ms);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IH264Source*src,FILE*fd){dbgTestPL();
	//set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IH264Source::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__);
	});
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming...
	while(!_exit) {
		std::this_thread::sleep_for(10_ms);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool SetParams(IH264Source*src){
	std::string paras;
	if(_h264Param){
		paras=",h264={";
		paras+=_h264Param;
		paras+="}";
	}
	//if(_cache){
	//	paras+=",cache=";
	//	paras+=_cache;//dbgTestPVL(_cache);dbgTestPVL(paras);
	//}
	dbgTestPVL(paras);
	if(!paras.empty())src->Set(paras.c_str()+1);
	return true;
}
static bool ProcessParams(IH264Source*src){
	if(_config){
		src->FromConfig(_config);
		return true;
	}

	if(_fileConfig) {
		src->LoadConfig(_fileConfig);
		return true;
	}
	//set yuv srouce parameters
	src->SelectSensor(_sensorID);//select the sensor0.
	src->SetWidth(_width);
	src->SetHeight(_height);
	//set h264 encoder parameters
	src->SetGop(_gop);
	src->SetGopType(_goptype);
	//set BitRateControl
	if(src->IsSupportBitRateControl())src->SetBitRate(_bitrate*1000);
	if(src->IsSupportPreRecord())src->SetPreRecordGroup(_prerecIdx);
	if(src->IsSupportSharedEncoder())src->SetSharedEncoderGroup(_sharedIdx);
	if(src->IsSupportedCache())src->SetCache(_cache);
	if(_fluency)src->SetFluency(_fluency);
	if(_framerate)src->SetFrameRate(_framerate);
	///streaming params set
	SetParams(src);
	return true;
}
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a h264Source instance with keywords.
	std::string keywords;
	if(_prerecIdx)keywords=std::string("prerec");
	else if(_sharedIdx)keywords=std::string("shared");
	if(_keywords) {
		keywords += "-";
		keywords += _keywords;
	}
	dbgTestPVL(keywords);
	std::unique_ptr<IH264Source> src(IH264Source::CreateNew(keywords.c_str()));
	returnIfErrC(false,!src);
	src->RegisterMessageCallback(&MessageProcess);
	///
	if(_showBrc){
		auto brcs=src->GetBrcModes();
		for(auto&brc:brcs){
			dbgTestPSL(brc.mode<<','<<brc.note);
		}
		auto brc=src->GetBrcMode();
		dbgTestPSL("current brc mode:"<<brc.mode<<','<<brc.note);
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
	if(src->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd));
	}else if(src->IsSupportedOutputFrameCallback()){
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
	///open to see more information
	//returnIfTestC(0,!OmfMain::Globle().LogConfig("all=true"));
	///check the params
	returnIfErrC(0,!Check());
	///process
	Process();
	///
	return 0;
}






