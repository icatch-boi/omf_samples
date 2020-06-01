#include <thread>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
static int _seconds=3600*24*30;
///
static const char* _shmname = "dualos_shm_stamp";
static int _shmmax = 16;
static int _shmidx = 0;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfStampService(...): \n"
	 "start a stamp service that generates the date stamp image and draw it to video image. eg..\n"
	 "> omfStampService -d 3600\n"
	},
	{"duration"	,'d',_seconds	,"the process run duration(seconds)."},
	{"the follow paramer is for shm. usually, using default.\n"},
	{"shmname"	,'N',_shmname	,"name of shm."},
	{"shmmax"	,'M',_shmmax	,"item max of shm."},
	{"shmidx"	,'I',_shmidx	,"item idx of shm."},
	{},
};
////////////////////////////////////////////
static bool Process(bool _dbg){
	auto layout = (std::string)
		"type=StampService,name=stamp,ShmName="+_shmname
	;
	dbgTestPSL(layout);
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	returnIfErrC(false,!obj.StatusUp("play"));
	///wait
	std::this_thread::sleep_for(std::chrono::seconds(_seconds));
	returnIfErrC(false,!obj.StatusDown("null"));
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc, char *argv[]){
	dbgNotePSL("omfStampService(...)\n");
	///parse the input params
	OmfHelper helper(_options0,argc,argv);
	///--help
	returnIfTestC(0,!helper);
	///output the params list
	helper.Print();
	///check the params
	returnIfErrC(0,!Check());
	///
	OmfMain omf;
	omf.ShowModules();
	omf.Debug(helper.Debug());
	if(helper.Log())omf.LogConfig(helper.Log());
	///
	Process(helper.Debug());
	///
	return 0;
}