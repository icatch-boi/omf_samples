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
//#include "omf_api.h"
#include "OmfMsgSiteUser.h"
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
		{"omfmsg(...): \n"
		 "this demo is to send a msg to omf.\n"
		 "> omfmsg -m event -p pre=5s,post=10s\n"
		},
		{"msg"	,'m',_cmd	,"message name."},
		{"para"	,'p',_para	,"message paras."},
		{},
};

static bool Process(){
	char output[2048];
	output[0]=0;
	dbgTestPVL(_cmd);
	dbgTestPVL(_para);
	auto tp=Now();
	dbgTestPVL(Now());
	returnIfErrC(false,!OmfMsgSiteUser::Send(_cmd,_para,strlen(_para)));
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
