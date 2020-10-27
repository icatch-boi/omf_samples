//
// Created by jg.wang on 2020/10/21.
//
#include <thread>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "omf_api.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
////////////////////////////////////////////
///input paramers
static const char* _cmd=0;
static const char* _para=0;
////////////////////////////////////////////
///input parameters parser table
static OmfHelper::Item _options0[]{
		{"omfCmd(...): \n"
		 "this demo is to send a command to omf."
		 "> omfcmd -c shm0_cmd -p cmd=flip,hflip=true,vflip=true,sendid=0,w=1920,h=1080\n"
		},
		{"cmd"	,'c',_cmd	,"command name."},
		{"para"	,'p',_para	,"command paras."},
		{},
};

static bool Process(){
	char output[2048];
	output[0]=0;
	dbgTestPVL(_cmd);
	dbgTestPVL(_para);
	auto tp=Now();
	dbgTestPVL(Now());
	returnIfErrC(false,!omfCommand(_cmd,_para,output));
	dbgTestPVL(Now());
	auto duration=Now()-tp;
	dbgTestPVL(output);
	dbgTestPVL(duration);
	return true;
}
static bool CheckParameters(){
	returnIfErrC(false,!_cmd);
	returnIfErrC(false,!_para);
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the parameters
	returnIfErrC(0,!CheckParameters());
	///
	Process();
	///
	return 0;
}
