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
		{"omfMemPool(...): \n"
		 "This demo is used to show how to use OmfMemPool to alloc or free shared memory.\n"
		 "The memory allocated in this process will be released automatically after the process ends.\n"
		},
		{"\nreceive message"
		 "> omfMemPool -s 1024   		 ####alloc shm with size \n"
		 "> omfMemPool -s 1024 -k 32666 ####alloc shm with size and key.  \n"
		 "> omfMemPool -s 1024 -n hahah ####alloc shm with size and name.  \n"
		 "> omfMemPool -s 1024 -f 1 ####alloc shm with size and release memory manual.  \n"
		},
		{"size"		,'s', _size ,"set the shared memory size."},
		{"key"		,'k', _key ,"set the shared memory key."},
		{"name"		,'n', _name ,"set the shared memory name."},
		{"free"		,'f', _free ,"set the shared memory name."},
		{},
};
////////////////////////////////////////////

static bool Process(){
	OmfMain&omf=OmfMain::Globle();
	bool _dbg=omf.DebugMode();
	///
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
	dbgNotePSL("omfMemPool\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///check the params
	returnIfErrC(0,!Check());
	///
	Process();
	///
	return 0;
}
