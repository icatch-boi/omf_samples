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
static int _seconds=30;//seconds
static const char* _keywords="Kvspb";
static int _width=1920;
static int _height=1080;
static int _framerate=0;
static int _bitrate=2000;///kb
static const char* _goptype = "ippp";
static int _gop = 30;
static int _fluency=0;
static int _showBrc=0;
static const char* _vbrc="vbrc";
static int _push = 0;
////////////////////////////////////////////
static bool _exit = false;
static OmfMain* _main=0;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfKvspb(...): \n"
	 "This demo shows how to get H264 streaming from OMF using IH264Source interface. \n"
     "  omfKvspb -d5 -w1920 -h1080 -f30 -b 128 -t ippp -g30 -v vbrc -p 1 -L \"\" -F 0 -e 3\n"

	},
	{"fname"	,'n', _fname	,"record filename(*.*)."},
	{"duration"	,'d', _seconds	,"record duration(*s)."},
	{"the yuv paramers:"},
	{"w"		,'w', _width	,"width of image."},
	{"h"		,'h', _height	,"height of image."},
	{"fr"		,'f', _framerate,"frame number per seconds."},
	{"the h264 paramers:"},
	{"bitrate"	,'b', _bitrate	,"bitrate(kb) of h264 encoder."},
	{"goptype"	,'t', _goptype	,"gop type(iiii,ippp,ibbp) of h264 encoder."},
	{"gop"		,'g', _gop		,"gop of h264 encoder."},
	{"misc:"},
	{"bit rate control:"},
	{"showBrc"	,'S', [](){_showBrc=1;}	,"show the h264 source bitrate control modes."},
	{"fluency"	,'e', _fluency	,"[-7,7]the fluency for streaming."},
	{"vbrc"	    ,'v', _vbrc	    ,"set vbrc mode:eg..\n"
	                                   "\n cbrc"
	                                   "\n vbrc"
	                                   "\n vbrc-vfrc"
	                                   "\n abrc-afrc"
							  },
	{"push"	    ,'p', _push	    ,"set output push mode,default pull mode:"},
	{"keywords"	,'k', _keywords	,"select the IH264Source/IAacSource with keywords:default is \"kvspb\""

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
static TimePoint _tpEnd(-1_s);
static bool ProcessFrame(std::shared_ptr<frame_t> frm,FILE*fd,int line,const char*codec){
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
	dbgTestPS(codec<<','<<line<<'#'<<frm->index
					  << ',' << frm->data
					  << ',' << frm->size
					  << ',' << frm->iskeyframe
					  << ',' << frm->pts
					  << ','
	);
	dbgTestDL(frm->data, 16);
	return true;
}

static bool ProcessPull(IStreamSource*src,FILE*fd,const char*codec){dbgTestPL();
	//start streaming
	returnIfErrC(false,!src->ChangeUp(State::play));
	//streaming....
	while(!_exit) {
		auto frm = src->PullFrame(false);
		if(frm)
			ProcessFrame(frm,fd,__LINE__,codec);
		std::this_thread::sleep_for(10_ms);
	}
	//stop streaming
	returnIfErrC(false,!src->ChangeDown(State::ready));
	return true;
}
static bool ProcessPush(IStreamSource*src,FILE*fd,const char*codec){dbgTestPL();
	//set push callback
	src->RegisterOutputCallback([&fd,codec](std::shared_ptr<IStreamSource::frame_t>&frm){
		return ProcessFrame(frm,fd,__LINE__,codec);
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
static bool ProcessAac(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a AacSource instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IAacSource> src(IAacSource::CreateNew("Kvspb"));
	returnIfErrC(false,!src);
	//set pcm srouce parameters
	//src->SelectMicrophone(_mic);//select the MIC.
	///set aac codec
	src->SetBitRate(128000);//_bitrate*1000);
	src->SetProfile("LC");
	src->SetTargetChannels(1);//_channels);//encode to 1 channels.
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
		fd = fopen("kvspb.aac", "wb");
		if (!fd) {
			dbgErrPSL("open file fail:" << _fname);
		}
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	//streaming......
	if(src->IsSupportedPullFrame()){
		returnIfErrC(false,!ProcessPull(src.get(),fd,"aac"));
	}else if(src->IsSupportedOutputFrameCallback()){
		returnIfErrC(false,!ProcessPush(src.get(),fd,"aac"));
	}else{
		dbgErrPSL("null support output mode.");
	}
	returnIfErrC(false,!src->ChangeDown(State::null));
	return true;
}
static bool ProcessH264(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	//create a h264Source instance with keywords.
	dbgTestPVL(_keywords);
	std::unique_ptr<IH264Source> src(IH264Source::CreateNew("Kvspb-vbrc-pull"));
	returnIfErrC(false,!src);
	///
	if(_showBrc){
		auto brcs=src->GetBrcModes();
		for(auto&brc:brcs){
			dbgTestPSL(brc.mode<<','<<brc.note);
		}
		auto brc=src->GetBrcMode();
		dbgTestPSL("current brc mode:"<<brc.mode<<','<<brc.note);
	}
	//set yuv srouce parameters
	src->SetWidth(_width);
	src->SetHeight(_height);
	//set h264 encoder parameters
	src->SetGop(_gop);
	src->SetGopType(_goptype);
	//set BitRateControl
	src->SetBitRate(_bitrate*1000);
	if(_fluency)src->SetFluency(_fluency);
	if(_framerate)src->SetFrameRate(_framerate);
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
		fd=fopen("kvspb.h264", "wb");
		if (!fd) {
			dbgErrPSL("open file fail:" << _fname);
		}
	}
	ExitCall ecfd([fd](){if(fd)fclose(fd);});
	//////////////////////////////////
	///streaming......
	_exit = false;
	///process frame
			if(src->IsSupportedPullFrame()){
				returnIfErrC(false,!ProcessPull(src.get(),fd,"h264"));
			}else if(src->IsSupportedOutputFrameCallback()){
				returnIfErrC(false,!ProcessPush(src.get(),fd,"h264"));
			}else{
				dbgErrPSL("null support output mode.");
			}
			returnIfErrC(false,!src->ChangeDown(State::null));
			dbgTestPVL(src.get());
			return true;
}
static bool ProcessCtrl(){

}
static bool Process(){
	std::thread h264([=](){ProcessH264();});
	std::thread aac ([=](){ProcessAac ();});

	h264.join();
	aac.join();
	return true;
}
////////////////////////////////////////////
static bool Check(OmfHelper&helper){
	returnIfTestC(false,!helper);
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("omfKvspb(...)\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	returnIfErrC(0,!Check(helper));
	///
	Process();
	///
	return 0;

}






