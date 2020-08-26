#include "_string_base.h"
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
////////////////////////////////////////////
static const char* _shmname = "dualos_shm_stamp";
static int _shmmax = 16;
static int _shmidx = 0;
///
static int _x = 0;//48;
static int _y = 0;//48;
static unsigned _color = 0;//0x00ff0000;
static unsigned _bgcolor = 0;
static const char* _pattern = "%04d-%02d-%02d %02d-%02d-%02d";
static int _draw = -1;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfStampSet(...): \n"
	 "set the follow paramers to stamp service. eg..\n"
	 "> omfStampSet -x 48 -y 48 -c 0x0000ff00 -d 1 -\n"
	},
	{"x"	,'x',  	_x		,"the stamp x position."},
	{"y"	,'y',  	_y		,"the stamp y position."},
	{"color",'c',  	_color	,"the stamp color(0xAABBGGRR)."},
	{"bgcolor",'b', _bgcolor,"the stamp background color(0xAABBGGRR)."},
	{"pattern",'p',	_pattern,"the stamp pattern(\"%04d-%02d-%02d %02d-%02d-%02d\")."},
	{"draw"	,'d',	_draw	,"draw the stamp enable or not."},
	{"the follow paramer is for shm. usually, using default.\n"},
	{"shmname"	,'N',_shmname	,"the shm name for stamp."},
	{"shmmax"	,'M',_shmmax	,"the shm item max for stamp."},
	{"shmidx"	,'I',_shmidx	,"the shm item idx for stamp."},
	{},
};
////////////////////////////////////////////
bool Process(int x,int y,unsigned color,const char* pattern,int draw,unsigned bgcolor){
	auto layout = (std::string)
		"type=StampParam,name=stampparam"
   			",ShmMax="+_shmmax+
	  		",ShmIdx="+_shmidx+
	 		",ShmName="+_shmname
	;
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	OmfAttrSet ap;
	if(pattern)ap.Add("pattern",pattern);///set
	//ap.Add("refw",1920);
	//ap.Add("refh",1080);
	if(x)ap.Add("x",x);///set
	if(y)ap.Add("y",y);///set
	//unsigned colortmp = color.r<<16|color.g<<8|color.b;
	if(color)ap.Add("color",(void*)color);
	if(draw>=0)ap.Add("draw", (draw>0));
	if(bgcolor) ap.Add("bgcolor", (void*)bgcolor);
	///
	auto para = ap.ToString();
	dbgTestPSL("set stamp params:");
	dbgTestPSL(para);
	dbgTestSL("");
	///
	returnIfErrC(false,!obj.Set("change",para));
	return true;
}

int main(int argc,char* argv[]) {
	dbgNotePSL("omfStampSet()\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	Process(_x,_y,_color,_pattern,_draw,_bgcolor);
	///
	return 0;
}