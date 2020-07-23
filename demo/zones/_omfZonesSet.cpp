#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
static const char* _shmname = "dualos_shm_rects";
static int _shmmax = 16;
static int _shmidx = 0;
///
static int _idx = 0;
static int _x = 0;//48;
static int _y = 0;//48;
static int _w = 0;
static int _h = 0;
static unsigned _color = 0;//0x00ff0000;
static int _draw = -1;
////////////////////////////////////////////
static bool _exit = false;
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
	{"omfZoneSet(...): \n"
	 "set the follow paramers to zones service. eg..\n"
	 "> omfZonesSet -i 0 -x 48 -y 48 -w 128 -h 512 -c 0x0000ff00 -d 1 -D 1\n"
	},
	{"idx"	,'i',  _idx		,"set the zone index."},
	{"x"	,'x',  _x		,"set the zone x position."},
	{"y"	,'y',  _y		,"set the zone y position."},
	{"w"	,'w',  _w		,"set the zone width."},
	{"h"	,'h',  _h		,"set the zone height."},
	{"color",'c',  _color	,"set the zone color(0xAABBGGRR)."},
	{"draw"	,'d',  _draw	,"set the zone draw enable or not."},
	{"the follow paramer is for shm. usually, using default.\n"},
	{"shmname"	,'N',_shmname	,"set the shm name for stamp."},
	{"shmmax"	,'M',_shmmax	,"set the shm item max for stamp."},
	{"shmidx"	,'I',_shmidx	,"set the shm item idx for stamp."},
	{},
};
////////////////////////////////////////////////////////////
#define YUV_COLOR(y,u,v)   (((v)<<16)|((y)<<8)|(u))
typedef struct{
	char b;char g;char r;char alpha;
}Res_Color;
unsigned ColorConvert(unsigned color0){
	Res_Color& color=*(Res_Color*)&color0;
	char R = color.r;
	char G = color.g;
	char B = color.b;
	printf("R=%x,g=%x,b=%x\n",R,G,B);
	unsigned char y = (+2990 * R + 5870 * G + 1140 * B )/10000;
    unsigned char u = (-1687 * R - 3313 * G + 5000 * B + 1280000)/10000;
    unsigned char v = (5000 * R - 4187 * G - 813 * B + 1280000)/10000;
	return YUV_COLOR(y,u,v);
}
static bool Process(int idx,int x,int y,int w,int h,unsigned color,int draw){
	auto layout = (std::string)
		"type=ZoneParam,name=zoneparam"
			",ShmMax="+_shmmax+
			",ShmIdx="+_shmidx+
			",ShmName="+_shmname
	;
	OmfObject obj(layout);
	returnIfErrC(false,!obj);
	OmfAttrSet ap;
	ap.Add("idx",idx);
	if(x)ap.Add("x",x);
	if(y)ap.Add("y",y);
	if(w)ap.Add("w",w);
	if(h)ap.Add("h",h);
	if(color)ap.Add("color",(void*) ColorConvert(color));
	if(draw>=0)ap.Add("draw",(draw>0));
	///
	auto para = ap.ToString();
	dbgTestPSL("set zones params:");
	dbgTestPSL(para);
	dbgTestSL("");
	///
	returnIfErrC(false,!obj.Set("change",para));
	return true;
}
int main(int argc,char* argv[]) {
	dbgNotePSL("omfZonesSet()\n");
	///parse the input parameters with the parser table,
	///and initialize omf system.
	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));
	///
	Process(_idx,_x,_y,_w,_h,_color,_draw);
	///
	return 0;
}