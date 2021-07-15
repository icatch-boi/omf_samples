//
// Created by wang.zhou on 2021/1/5.
//
#include "omf_api.h"
#include "omf_msg_site_user.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "OmfHelper.h"
#include "OmfMain.h"

#define dbgTestPL() printf("[%s(%d)]\n",__FUNCTION__,__LINE__);
#define dbgTestPSL(v) printf("[%s(%d)]:%s\n",__FUNCTION__,__LINE__,v);
#define dbgTestPDL(v) printf("[%s(%d)]:%s=%d\n",__FUNCTION__,__LINE__,#v,v);
#define dbgTestPVL(v) printf("[%s(%d)]:%s=%s\n",__FUNCTION__,__LINE__,#v,v);
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}
#define returnIfErrC0(c) if(c){dbgTestPSL(#c);return;}


#define SHM0_CMD 		"shm0_cmd" /*system cmd ,can not change*/
#define MD_CMD			"md_cmd"  /*must same to iCatOS register*/
#define MD_NN_CMD		"md_nn_cmd"  /*must same to iCatOS register*/
#define MD_MSG_NAME		"mdResult" /*must same to iCatOS sent*/
#define NN_MSG_NAME		"nnresult" /*must same to iCatOS sent*/

#define MD_COORD_HSIZE     65536 /*md coordinate window*/
#define MD_COORD_VSIZE     65536

#define MD_MOTION_BLOCKS	1
#define MD_BLOCK_VALS 	0

#define NN_ROI_CTRL 0 //need nn runing if 1

/**document
 * https://www.yuque.com/docs/share/082b5ec4-ab0b-4b21-8ec6-457a0c276bb7?#
 **/

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

////////////////////////////////////
using namespace omf;
using namespace omf::api;
////////////////////////////////////

///MD result structure
typedef struct omfMdResult_s{
	uint32 x0; /* 1024 domain */
	uint32 y0;
	uint32 x1;
	uint32 y1;
	uint32 block_w;
	uint32 block_h;
	uint32 block_diff_vals[960];
	uint8 motion_blocks[960];
}omfMdResult_t;

///MD config structure
typedef struct omfMdConfig_s{
	uint16 x,y,w,h;
	uint32 fgd_block_size; /* Block size at 320x192 domain. Should be [8, 16, 32, 64]. Default = 16 */
	uint32 fgd_day_th; /* Abs diff with base 100. Defualt = 6.25 * 100 = 625 */
	uint32 fgd_night_th; /* Abs diff with base 100. Defualt = 4.68 * 100 = 468*/
}omfMdConfig_t;
///NN IMG ROI
typedef struct omfNnImgRoi_s {
	uint32 width;	/* width */
	uint32 height;	/* height */
	uint32 roiX;	/* offset x of roi */
	uint32 roiY;	/* offset y of roi */
	uint32 roiW;	/* width of roi */
	uint32 roiH;	/* height of roi */
	uint32 reserved;
} omfNnImgRoi_t;

///MD ctrl operation
enum {
	OMF_MD_OPER_INIT = 0,
	OMF_MD_OPER_CTRL = 1,
	OMF_MD_OPER_PV = 2,
	OMF_MD_OPER_UNINIT = 3,
};

///MD status
enum mdFrameWorkState_s {
	OMF_MD_CTRL_STOP = 0,
	OMF_MD_CTRL_RUN = 1,
	OMF_MD_CTRL_PAUSE,
	OMF_MD_CTRL_RESUME,
};
///NN ctrl
enum appNnPvStreamSrcRoiOpt_e {
	OMF_NN_PV_STREAM_GEN_NORMAL_FLOW,   /* normal flow */
	OMF_NN_PV_STREAM_GEN_EVERY_FRAME,   /* flow 1 */
	OMF_NN_PV_STREAM_GEN_WITH_REQUEST,  /* flow 2 */
	OMF_NN_PV_STREAM_GEN_MAX,
};
typedef enum{
	DATA_INTERACTION_OPERATION_SET = 0,
	DATA_INTERACTION_OPERATION_GET = 1,
	DATA_INTERACTION_OPERATION_CMD = 2,
}DataInteractionOperation_e;

static int _time = 30;//s
static int _expand_ration = 150;  // 150/100
static bool _nnRun = 1;//s
static bool _exit = false;
static bool _nn_doing = false;
static omfMdResult_t* _mdResult = 0; /*get from iCatOS*/
static omfMdConfig_t* _mdConfig = 0; /*get from iCatOS*/
static omfNnImgRoi_t* _nnRoi = 0; /*get from iCatOS*/

int MdNnRoiSet(omfNnImgRoi_t* roi,int opt);

static OmfHelper::Item _options0[]{
		{"omfAacSrc(...): \n"
		 "This demo shows how to get MD result and set to NN."
		 "> omfMotionDetectionNn -d 30\n"
		 "> omfMotionDetectionNn -n 0 -d 30\n"
		 "> omfMotionDetectionNn -n 1 -d 30\n"
		},
#if NN_ROI_CTRL
		{"runNn",'n', _nnRun	,"run nn when md start(0/1), default 1."},
#endif
		{"duration",'d', _time	,"set process execute duration(*s),defaule 30s."},
		{},
};

#if NN_ROI_CTRL
///the NN result will be send to here
static int MsgcbReceiveNN(void* priv_hd, const void *data, int size){
	printf("#received nn msg:%s\n",(const char*)data);
	_nn_doing = false;
	return 1;
}
static int NnDemoInit(void){
	char input[64];
	sprintf(input, "cmd=nn,operation=%d,model=1,msgen=true", DATA_INTERACTION_OPERATION_SET);
	returnIfErrC(0,!omfCommand("shm0_cmd",input, 0));
	printf("nn demo start\n");
	return 1;
}
static int NnDemoExit(void){
	printf("nn demo stop\n");
	char input[64];
	sprintf(input, "cmd=nn,operation=%d,model=0,msgen=false", DATA_INTERACTION_OPERATION_SET);
	returnIfErrC(0,!omfCommand("shm0_cmd",input, 0));
	return 1;
}
#endif

///the MD result will be send to here
static int MsgcbReceiveMD(void* priv_hd, const void *data, int size){
	returnIfErrC(0,!data);
	void *attr = omfCreateAttrSet((const char*)data);
	if(attr){
		if(!_mdResult)
			_mdResult = (omfMdResult_t*)omfAttrGetInt(attr,"presult");
		omfDestroy(attr);
	}
	if(_mdResult){
		printf("[D] Motion ROI (%d, %d, %d, %d)\n",
			   _mdResult->x0, _mdResult->y0, _mdResult->x1, _mdResult->y1);
#if NN_ROI_CTRL
		if(_nnRoi){
			_nnRoi->width = 1024; /*MD virtual coord*/
			_nnRoi->height= 1024;
			_nnRoi->roiX = _mdResult->x0;
			_nnRoi->roiY = _mdResult->y0;
			_nnRoi->roiW = _mdResult->x1-_mdResult->x0;
			_nnRoi->roiH = _mdResult->y1-_mdResult->x0;

			if(!_nn_doing) {
				_nn_doing = true;
				
				/* Motion detect nothing, set NN roi as full image */
				if ( _nnRoi->roiW == 0 || _nnRoi->roiH == 0) {
					_nnRoi->roiX = 0;
					_nnRoi->roiY = 0;
					_nnRoi->roiW = _nnRoi->width;
					_nnRoi->roiH = _nnRoi->height;					
				}
				
				/* expand roi */
				int min_x, min_y, max_x, max_y, new_w, new_h, cen_x, cen_y;
				int expand_ration = _expand_ration;
				if( (int)_nnRoi->roiX < 0 || (int)_nnRoi->roiX > _nnRoi->width)
					_nnRoi->roiX = 0;
				if( (int)_nnRoi->roiY < 0 || (int)_nnRoi->roiY > _nnRoi->height)
					_nnRoi->roiY = 0;
				if( (int)_nnRoi->roiW < 0 || (int)_nnRoi->roiW > _nnRoi->width )
					_nnRoi->roiW = _nnRoi->width;
				if( (int)_nnRoi->roiH < 0 || (int)_nnRoi->roiH > _nnRoi->height )
					_nnRoi->roiH = _nnRoi->height;
				if( ( _nnRoi->roiX + _nnRoi->roiW ) > _nnRoi->width ){
					_nnRoi->roiX = 0;
					_nnRoi->roiW = _nnRoi->width;
				}
				if( ( _nnRoi->roiY + _nnRoi->roiH ) > _nnRoi->height ){
					_nnRoi->roiY = 0;
					_nnRoi->roiH = _nnRoi->height;
				}

				min_x = (int)_nnRoi->roiX;
				min_y = (int)_nnRoi->roiY;
				max_x = (int)_nnRoi->roiX + (int)_nnRoi->roiW;
				max_y = (int)_nnRoi->roiY + (int)_nnRoi->roiH;
				cen_x = (int)_nnRoi->roiX + (int)(_nnRoi->roiW / 2);
				cen_y = (int)_nnRoi->roiY + (int)(_nnRoi->roiH / 2);

				new_w = (int)( ( _nnRoi->roiW * expand_ration ) / 100 );
				new_h = (int)( ( _nnRoi->roiH * expand_ration ) / 100 );
				min_x = (int)cen_x - (new_w / 2);
				min_y = (int)cen_y - (new_h / 2);
				max_x = (int)cen_x + (new_w / 2);
				max_y = (int)cen_y + (new_h / 2);

				 _nnRoi->roiX = ( min_x < 0) ? 0 : (uint32)min_x;
				 _nnRoi->roiY = ( min_y < 0) ? 0 : (uint32)min_y;
				 _nnRoi->roiW = ( max_x > _nnRoi->width ) ? _nnRoi->width  : (uint32)(max_x - min_x);
				 _nnRoi->roiH = ( max_y > _nnRoi->height) ? _nnRoi->height : (uint32)(max_y - min_y);

				if( ( _nnRoi->roiX + _nnRoi->roiW ) > _nnRoi->width ){
					_nnRoi->roiW = _nnRoi->width - _nnRoi->roiX;
				}
				if( ( _nnRoi->roiY + _nnRoi->roiH ) > _nnRoi->height ){
					_nnRoi->roiH = _nnRoi->height - _nnRoi->roiY;
				}
				 _nnRoi->roiX &= ~1;
				 _nnRoi->roiY &= ~1;
				 _nnRoi->roiW &= ~1;
				 _nnRoi->roiH &= ~1;
				printf(" new roi:%d,%d %d,%d, wh(%d,%d)\n",
					_nnRoi->roiX,_nnRoi->roiY,_nnRoi->roiW,_nnRoi->roiH, new_w, new_h);
				
				printf("NN roi ctrl...\n");
				MdNnRoiSet(_nnRoi,OMF_NN_PV_STREAM_GEN_WITH_REQUEST);
			}else{
				printf("NN roi doing...\n");
			}
		}
#else
		{
			uint32 i, j, cnt;
			#if 0
			printf("[D] Motion Blocks=====================\n");
			cnt = 0;
			for(i = 0; i < _mdResult->block_h; i++){
				for(j = 0; j < _mdResult->block_w; j++){
					printf("%d ", _mdResult->motion_blocks[cnt]);
					cnt++;
				}
				printf("\n");
			}
			printf("[D] Motion Blocks=====================\n");
			#endif

			#if 0
			printf("[D] Block Vals=====================\n");
			cnt = 0;
			for(i = 0; i < _mdResult->block_h; i++){
				for(j = 0; j < _mdResult->block_w; j++){
					printf("%d ", _mdResult->block_diff_vals[cnt]);
					cnt++;
				}
				printf("\n");
			}
			printf("[D] Block Vals=====================\n");
			#endif
		}
#endif
	}
	return 1;
}
static int MsgProcess() {
	dbgTestPL();
	///register msg process function to OMF
	returnIfErrC(0,!omfMsgSiteUserRegister(MD_MSG_NAME,&MsgcbReceiveMD,0));
#if NN_ROI_CTRL
	printf("register nn msg\n");
	returnIfErrC(0,!omfMsgSiteUserRegister(NN_MSG_NAME,&MsgcbReceiveNN,0));
#endif
	return 1;
}
static int MsgClose() {
	///delete msg process function from OMF
	returnIfErrC(0,!omfMsgSiteUserUnRegister(MD_MSG_NAME));
#if NN_ROI_CTRL
	returnIfErrC(0,!omfMsgSiteUserUnRegister(NN_MSG_NAME));
#endif
	return 1;
}

static void AppMdDump(){
	if(_mdConfig){
		printf("fgd config(%x):\nparm:%d,x:%d,y:%d,w:%d,h:%d,fgd_block_size:%d,fgd_day_th:%d,fgd_night_th:%d\n"
		,_mdConfig
		,_mdConfig->x
		,_mdConfig->y
		,_mdConfig->w
		,_mdConfig->h
		,_mdConfig->fgd_block_size
		,_mdConfig->fgd_day_th
		,_mdConfig->fgd_night_th);
	}
	printf("fgd result(%x)\n",_mdResult);
}
/** Get two icatos  buffer for linux use
 * @return 1(true)/0(false)
 */
static int AppMdInit(){
	int ret = 0;
	char input[128] = {0};
	char output[1024] = {0};

	std::string cmd = "cmd=%s,operation=%d";
	sprintf(input,cmd.c_str(),MD_CMD,OMF_MD_OPER_INIT);

	///send cmd to iCatOS
	returnIfErrC(0,!omfCommand(SHM0_CMD,input,output));

	void *attr = omfCreateAttrSet((const char*)output);
	if(attr){dbgTestPSL(output);
		///get buffer when init success
		if(omfAttrGetBool(attr,"rst")){
			_mdResult = (omfMdResult_t*)omfAttrGetInt(attr,"presult");
			_mdConfig = (omfMdConfig_t*)omfAttrGetInt(attr,"pconfig");
			_nnRoi = (omfNnImgRoi_t*)omfAttrGetInt(attr,"nnroi");
			ret = 1;
		}else{
			printf("init failed!\n");
		}
	}
	omfDestroy(attr);

	///dump the init defaule config
	AppMdDump();

	return ret;
}
/** Destory buffer
 * @return 1(true)/0(false)
 */
static int AppMdUninit(){
	char input[128] = {0};
	sprintf(input,"cmd=%s,operation=%d,",MD_CMD,OMF_MD_OPER_UNINIT);
	///send cmd to iCatOS
	return omfCommand(SHM0_CMD,input,0);
}
/** Setting PT YUV
 * @param 1(enable)/0(disable)
 * @return 1(true)/0(false)
 */
static int AppMdPV(int en){
	char input[128];
	sprintf(input,"cmd=%s,operation=%d,en=%d",MD_CMD,OMF_MD_OPER_PV,en);
	return omfCommand(SHM0_CMD,input,0);
}
/** Control MD status,before config must set status to stop
 * @param status
 * @param MD config
 * @return 1(true)/0(false)
 */
static int AppMdControl(int ctrl,void* cfg){
	int ret = 0;
	char output[128];
	char input[128];
	std::string cmd = "cmd=%s,operation=%d,ctrl=%d,pconfig=%d";
	sprintf(input,cmd.c_str(),MD_CMD,OMF_MD_OPER_CTRL,ctrl,(int)cfg);

	///send config to iCatOS
	if(omfCommand(SHM0_CMD,input,output)){
		void *attr = omfCreateAttrSet((const char*)output);
		if(attr){dbgTestPSL(output);
			///config success ,get result
			if(omfAttrGetBool(attr,"rst")) {
				_mdConfig = (omfMdConfig_t *) omfAttrGetInt(attr, "pconfig");
				ret = 1;
			}else{
				printf("config failed!\n");
			}
		}
		omfDestroy(attr);
	}
	///dump the config result
	AppMdDump();

	return ret;
}

static int MdProcess(){
	///step1. Init MD fgd buffer
	returnIfErrC(0,!AppMdInit());

	///step2. Setting pt yuv enable
	returnIfErrC(0,!AppMdPV(1));

	///step3. Config param
	_mdConfig->x = 0;
	_mdConfig->y = 0;
	_mdConfig->w = MD_COORD_HSIZE-1;
	_mdConfig->h = MD_COORD_VSIZE-1;
	_mdConfig->fgd_block_size = 16;
	_mdConfig->fgd_day_th = 625;
	_mdConfig->fgd_night_th = 468;

	///step4. Set config and start MD
	AppMdControl(OMF_MD_CTRL_RUN,(void*)_mdConfig);

	while(!_exit && _time--){
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	///step5. Set config and start MD
	AppMdControl(OMF_MD_CTRL_STOP,0);

	///step6. Setting pt yuv disable
	returnIfErrC(0,!AppMdPV(0));

	///step7. Uninit md buffer
	returnIfErrC(0,!AppMdUninit());

	return 1;
}

static int MdNnRoiSet(omfNnImgRoi_t* roi,int opt){
	char input[128];
	sprintf(input,"cmd=%s,nnroi=%d,operation=%d",MD_NN_CMD,roi,opt);
	return omfCommand(SHM0_CMD,input,0);
}

extern "C"
void mysighandler (int a)
{
	printf("Ctrl C ...\n");

	_exit = true;
}

int main(int argc, char *argv[]){
	///Catch ctrl+c
	struct sigaction newhandler;
	newhandler.sa_handler = &mysighandler;
	sigaddset(&newhandler.sa_mask, SIGQUIT);
	newhandler.sa_flags = 0;
	sigaction(SIGINT, &newhandler, NULL);
	///Init omf system
	returnIfErrC(0,!OmfMain::Initialize(_options0,argc,argv));

#if NN_ROI_CTRL
	///Start nn
	if(_nnRun)
		returnIfErrC(0,!NnDemoInit());
#endif

	///Register msg process
	MsgProcess();
	///MD pocess
	MdProcess();
	///Close msg process
	MsgClose();

#if NN_ROI_CTRL
	///Stop nn
	if(_nnRun)
		returnIfErrC(0,!NnDemoExit());
	#endif

	return 0;
}
