//
// Created by jg.wang on 2020/4/15.
//

#include <stdio.h>
#include <string.h>

#include "nn_human_detect.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_os_api.h"
#include "omf_msg_site.h"

#define dbgTestPVL(v) printf("%s/%d:%s#%s=%d\n",__FILE__,__LINE__,__FUNCTION__,#v,v)
#define dbgTestPSL(v) printf("%s/%d:%s:%s\n",__FILE__,__LINE__,__FUNCTION__,v)
#define returnIfErrC(v,c) if(c){dbgTestPSL(#c);return v;}

static int _srcID=101;
static int _msgID=100;

#define APP_NN_HUMAN_MAX_NUM 	 5
static nnModelCfg_t nnOdCfg = {
		.bin_name = "A:\\RO_RES\\NN\\RES\\PERSONYOLOV2TINY.BIN",
		.confThreshold = 650, /*Max is 1000*/
};

static void* omfNnInitNnModel(const char*media,void* priv_data){

	/// the function is a callback, that will be register to OMF, and called before streaming start.
	/// the media input argv is the media script string, to give the media attributes, such as codec,width,height and so on.
	/// the priv_data input argv is the multi-instance data, gived by the layout string.

	UINT32 hd = nnHumanDetModeCreate(&nnOdCfg);

	printf("omfNnInitNnModel()\n\tmedia:\t%s\n\tpriv_data:\t%p\n\t return:\t%p",media,priv_data, (void*)hd);
	return (void*)hd;
}
static void omfNnReleaseNnModel(void*hd){
	/// the function is a callback, that will be register to OMF, and called when streaming stop.
	/// the hd input argv is the value returned by xx_open;
	printf("omfNnReleaseNnModel(%p)\n",hd);

	nnHumanDetModeDestory((UINT32)hd);
}
static int omfNnPrecessFrame(void*hd,void*frame,int size,long long pts_ms,unsigned flags,int idx,void(*free)(void*),void* free_data){
	/// the function is a callback, that will be register to OMF, to receive the frame data from OMF.
	/// obj: the object instance, that is returned by xx_open;
	/// data: the frame data pointer;
	/// size: the frame data size;
	/// pts_ms: the timestamp of frame,the unit is milliseconds.
	/// flags: the frame flags, the bit[0] is to sign key frame.
	/// idx: the frame index;
	/// free: the frame free callback, if the frame is no longer used, this callback have to be call.
	/// free_data: the input param of free callback.

	printf("omfNnPrecessFrame(%p) %p, size=%d, pts_ms=%lld\n", hd, frame, size, pts_ms);
#if 0
	char bin_name[128];
	sprintf(bin_name, "D:\\nn_stucture\\n_%p.yuv", frame);
	UINT32 fd = sp5kFsFileOpen((UINT8 *)bin_name, SP5K_FS_OPEN_CREATE);
	sp5kFsFileWrite(fd, (UINT8 *)frame, size);
	sp5kFsFileClose(fd);
#endif
	sp5kNnFrameInfo_t nnImg;
	UINT32 nnOdOutSize = sizeof( sp5kNnModelObjectDetOutput_t ) * APP_NN_HUMAN_MAX_NUM;
	UINT32 nnOutNum = 0, i, humanNum = 0;
	sp5kNnModelObjectDetOutput_t *pnnObjectOut = sp5kMalloc(nnOdOutSize);
	char resultMsg[1024*2] = {0};
	char subMsg[128] = {0};

	memset(&nnImg, 0x0, sizeof(nnImg));
	nnImg.pbuf	  = frame;
	nnImg.width   = 640;
	nnImg.height  = 360;
	nnImg.roiW	  = 640;
	nnImg.roiH	  = 360;
	nnImg.roiX	  = 0;
	nnImg.roiY	  = 0;
	nnImg.fmt	  = SP5K_GFX_FMT_YUV422;

	printf("omfNnPrecessFrame before\n");

	UINT32 ret = nnHumanDetect(&nnImg, (UINT32)hd, pnnObjectOut, nnOdOutSize, &nnOutNum);
	printf("omfNnPrecessFrame ret =%d, nnOutNum=%d\n", ret, nnOutNum);

	if(ret == NN_SUCCESS && nnOutNum > 0){
		for(i = 0; i < nnOutNum && i < APP_NN_HUMAN_MAX_NUM; i ++ ){
			printf("%d : idx:%d, obj:%s, conf:%d%%, (%d,%d,%d,%d)\n",
				   i, pnnObjectOut[i].label_idx, pnnObjectOut[i].label_string, (pnnObjectOut[i].conf * 100) / 1000,
				   pnnObjectOut[i].objectBBox.roiX, pnnObjectOut[i].objectBBox.roiY,
				   pnnObjectOut[i].objectBBox.roiW, pnnObjectOut[i].objectBBox.roiH);
			if( strcmp(pnnObjectOut[i].label_string, "person") == 0  && pnnObjectOut[i].conf >= nnOdCfg.confThreshold) {
				sprintf(subMsg, ",h%d={x=%d,y=%d,w=%d,h=%d,conf=%d}",
						humanNum, pnnObjectOut[i].objectBBox.roiX, pnnObjectOut[i].objectBBox.roiY,
						pnnObjectOut[i].objectBBox.roiW, pnnObjectOut[i].objectBBox.roiH, pnnObjectOut[i].conf);
				strcat(&resultMsg[1024], subMsg);
				printf("msgsize: %d, %d\n", strlen(resultMsg), strlen(subMsg));
				humanNum ++;
			}
		}
		sprintf(resultMsg, "humanNum=%d", humanNum);
		strcat(resultMsg, &resultMsg[1024]);
	}else{
		sprintf(resultMsg, "humanNum=%d", humanNum);
	}

	printf("nn result msg: %s\n", resultMsg);
#if 1
	void* site = (void*)omfMsgSite0Get();
	returnIfErrC(0,!site);
	returnIfErrC(0,!omfMsgSiteIsWorking(site));
	printf("\nSiteId:%d\n",omfMsgSiteGetID(site));
	printf("send message%d->%d:%s\n",_srcID, _msgID, resultMsg);
	int len = resultMsg?(strlen(resultMsg)+1):0;
	returnIfErrC(0,!omfMsgSiteSend1(site,_srcID,_msgID,resultMsg,len,0));
#endif
	///free the frame
	if(free)free(free_data);
	if(pnnObjectOut)
		sp5kFree(pnnObjectOut);
	return 1;
}

void appNnFeatureRegister(void){
	omfRegisterSink("HumanDetectSink"
			,&omfNnInitNnModel
			,&omfNnReleaseNnModel
			,&omfNnPrecessFrame
	);
}