#include "omf_msg_site_user.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OmfMain.h"
#include "OmfObject.h"
#include "OmfDbg.h"
#include "OmfAttrSet.h"
#include "OmfHelper.h"
/////////////////////////////////////////////
using namespace omf;
using namespace omf::api;
////////////////////////////////////////////
#undef dbgEntryTest
#define dbgEntryTest(s) dbgEntrySky(s)
///////////////////////////////////////////
//#define returnIfErrC(v,c) if(c){printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,#c);return v;}

#define YUV_COLOR(y,u,v)   (((u)<<16)|((y)<<8)|(v))
static const char* _msgName="FaceDetectResult";
static int _drawEn = 0;
static int _color = YUV_COLOR(0xff, 0x00, 0x80);
////////////////////////////////////////////
static OmfHelper::Item _options0[]{
		{"omfNnFaceDetectProcess(...): \n"
		 "Detect  faces.\n"
		 "> omfNnFaceDetectProcess -d 0 \n"
		},
		{"draw",'d', _drawEn		,"draw the line."},
		{"color",'c', _color		,"set the line color."},
		{},
};

////////////////////////////////
static std::string CreateLayoutString(void*hd){
	dbgErrPSL(hd);
	auto str = new char[2048];
	std::unique_ptr<char[]> ptr_str(str);
	sprintf(str,
		"type=ShmService,index=1,partner={"
			"type=application,layout={"
				"type=pipeline,name=pl-nn-process,layout={"
					"pv-yuv-src-adv:w=640,h=360"
					"+FrameRateCtrl:fr=1"
					"+FaceDetectFilter:priv_data=%p,name=FaceDetectFilter"
	 				"+FakeSink:live=false"
				"}"
			"}"
		"}"
		,hd
	);
	return str;
}

static int MsgSitecbReceive(void* priv_hd, const void *data, int size){
	dbgTestPSL((unsigned)priv_hd);
	dbgTestPSL((unsigned)data);
	dbgErrPSL(size);
	if(data){
		dbgErrPVL((const char*)data);
	}
	return 1;
}

static int MsgProcess() {
	dbgTestPL();
	returnIfErrC(0,!omfMsgSiteUserRegister(_msgName,&MsgSitecbReceive,0));
	return 1;
}
static int MsgClose() {
	returnIfErrC(0,!omfMsgSiteUserUnRegister(_msgName));
	return 1;
}

static int NNprocess(const char*str,int wait) {
	printf("%s\n",str);

	OmfObject obj(str);
	returnIfErrC(false,!obj);
	//returnIfErrC(false,!obj.Register(&MessageProcess));
	returnIfErrC(false,!obj.StatusUp("play"));

	///wait
	std::this_thread::sleep_for(std::chrono::seconds(wait));
	///
	returnIfErrC(false,!obj.StatusUp("null"));
	return 1;
}

int main(int argc, char *argv[]){



	returnIfTestC(0,!OmfMain::Initialize(_options0,argc,argv));

	char output[512] ;//= nullptr;
	OmfAttrSet ap;
	ap.SetOrAdd("name","nnAttr");
	ap.SetOrAdd("size",1024);
	dbgErrPL();
	OmfMain::Globle().Command("shm_alloc",ap.ToString().c_str(),output);
	dbgErrPL();

	OmfAttrSet apout(output);
	//memset(paramstr,0,512);
	void* ptr = nullptr;
	apout.Get("ptr",ptr);
	dbgErrPSL(ptr);
	sprintf(ptr,"draw=%d,color=%d",_drawEn,_color);
	printf("\n@@paramstr:%s,%p\n",ptr,ptr);
	dbgErrPSL(ptr);
	std::string str=CreateLayoutString(ptr);
	MsgProcess();
	NNprocess(str.c_str(),600);

	MsgClose();

	OmfAttrSet ap0;
	ap.SetOrAdd("name","nnAttr");
	ap.SetOrAdd("size",1024);
	ap.SetOrAdd("ptr",ptr);
	dbgErrPL();
	OmfMain::Globle().Command("shm_free",ap.ToString().c_str(),0);
	dbgErrPL();
	return 0;
}