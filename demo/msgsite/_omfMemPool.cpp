//
// Created by jg.wang on 2020/2/25.
//
#include <thread>
#include "_hash.h"
#include "_chrono_base.h"
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
#include "OmfMemPool.h"
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
using namespace omf::chrono;
using namespace omf::mem;
////////////////////////////////////////////
static int _size=1024;
static unsigned _key=0;
static const char* _name="test";
static bool _free=true;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"demoMemPool(...): \n"
		 "This demo is used to show how to use OmfMemPool to alloc or free shared memory.\n"
		 "The memory allocated in this process will be released automatically after the process ends.\n"
		},
		{"size"		,'s', _size ,"set the shared memory size."},
		{"key"		,'k', _key ,"set the shared memory key."},
		{"name"		,'n', _name ,"set the shared memory name."},
		{"free"		,'f', _free ,"set the shared memory name."},
		{"receive message"
		 "> demoMemPool -s 1024   		 ####alloc shm with size \n"
		 "> demoMsgSite -s 1024 -k 32666 ####alloc shm with size and key.  \n"
		 "> demoMsgSite -s 1024 -n hahah ####alloc shm with size and name.  \n"
		 "> demoMsgSite -s 1024 -f 1 ####alloc shm with size and release memory manual.  \n"
  		 },
		{},
};
////////////////////////////////////////////

static bool Process(OmfMain&omf,bool _dbg){
	dbgTestPVL(Now());
	auto pool = OmfMemPool::ShmPool();
	returnIfErrC(false,!pool);
	dbgTestPVL(Now());
	void* shm=0;
	if(_key) shm= pool->Alloc(_size,_key);
	else if(_name) shm= pool->Alloc(_size,_name);
	else if(_size) shm= pool->Alloc(_size);
	else dbgErrPSL("error input paramer");

	dbgTestPVL(shm);
	dbgTestPVL(Now());
	if(_free) {
		if (_key) {returnIfErrC(false, !pool->Free(_key));}
		else if (_name) {returnIfErrC(false, !pool->Free(_name));}
		else{ returnIfErrC(false, !pool->Free(shm,_size));}
	}
	dbgTestPVL(Now());
	return true;
}
static bool Check(){
	return true;
}
////////////////////////////////
int main(int argc,char* argv[]){
	dbgNotePSL("demoMemPool\n");
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
	//if(helper.Log())omf.LogConfig(helper.Log());
	///
	Process(omf,helper.Debug());
	///
	return 0;
}
