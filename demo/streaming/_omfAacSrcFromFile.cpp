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
///
static const char* _fileConfig=0;
////////////////////////////////////////////
static bool _blocking=false;
static bool _dumpFrm=true;
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfAacSrcFromFile(...): \n"
	 "This demo shows how to get Aac streaming from OMF using config file. \n"
     "> omfAacSrcFromFile -l ./_omfAacSrc.cfg \n"   ///load configure file _omfAacSrc.cfg
	},
	{"fname"	,'n'	, _fname	,"record filename(*.aac)."},
	{"duration"	,'d'	, _seconds	,"record duration(*s)."},
	{"load"     ,'l'	, _fileConfig,"loading configure file."},
	{"\nmisc:"},
	{"dumpfrm"	,'F'	, [](){_dumpFrm=false;}	,"disable dump the frame."},
	{"blockfrm"	,'B'	, [](){_blocking=true;}	,"blocking to process frame."},
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
static bool ProcessPull(IAacSource*src,FILE*fd){dbgTestPL();
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
static bool ProcessPush(IAacSource*src,FILE*fd){dbgTestPL();
	///set push callback
	src->RegisterOutputCallback([&fd](std::shared_ptr<IAacSource::frame_t>&frm){
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
static bool Process(){
	bool _dbg=OmfMain::Globle().DebugMode();
	///////////////////////////////////////
	///create a aacSource instance from config file.
	dbgTestPVL(_fileConfig);
	std::unique_ptr<IAacSource> src(IAacSource::CreateNewFromFile(_fileConfig));
	returnIfErrC(false,!src);
	src->RegisterMessageCallback(&MessageProcess);
	///open streaming
	returnIfErrC(false,!src->ChangeUp(State::ready));
	///get streaming parameters after Open().
	auto info = src->GetAacMediaInfo();
	dbgTestPVL(info.rate);
	dbgTestPVL(info.channels);
	dbgTestPVL(info.profile);
	dbgTestPVL(info.version);
	dbgTestPVL(info.rateidx);
	dbgTestPVL(info.bitrate);
	dbgTestPVL(info.hasADTS);
	dbgTestPVL(info.adtsLength);
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






