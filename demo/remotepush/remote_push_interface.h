#ifndef __REMOTE_PUSH_INTERFACE_H__
#define __REMOTE_PUSH_INTERFACE_H__

#include "remote_push_errno.h"
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 	
typedef unsigned UINT32;

typedef enum _remotePushCBs_e {
	REMOTE_PUSH_CB_MSG_START_COUNTDOWN,
	REMOTE_PUSH_CB_MSG_STOP_COUNTDOWN,	
	REMOTE_PUSH_CB_FILE_START_COUNTDOWN,
	REMOTE_PUSH_CB_FILE_STOP_COUNTDOWN,	
}remotePushCBs_e;
	
typedef enum _remotePushMsgEvt_e{
	MSG_EVT_MOTION_DET				= 100,
	MSG_EVT_LOW_BAT					= 102,
	MSG_EVT_OTA_DOWNLOAD_DONE		= 106,
	MSG_EVT_OTA_DOWNLOAD_ERR	 	= 107,
	MSG_EVT_OTA_INSTALL_DONE    	= 108,
	MSG_EVT_OTA_INSTALL_ERR    	 	= 109,
	MSG_EVT_EXITPV				 	= 110,
	MSG_EVT_RING					= 301,
	
	/* 900~999 is user-defined message type */
	MSG_EVT_USER_PACKAGE_DETECT		=900,	/* Package Detection */
	MSG_EVT_MAX,
}remotePushMsgEvt_e;

typedef struct _remotePushcoordination_t {
	int left;
	int top;
	int width;
	int height;
	int confidence;
}remotePushcoordination_t;

typedef struct _remotePushMsgBaseParam_t {/* these parameters are needed in common and generic situation */
	int type;
	void* data;
	int size;
	char timeStr[32];
}remotePushMsgBaseParam_t;

typedef struct _remotePushMsgFaceRecogParam_t {/* these parameters are needed by face-recognition */
	int recogResult;		/* the result of face-recognition */
	int faceID[2];			/* faceID array */
	int faceIDNum;			/* the actual number of valid number */
}remotePushMsgFaceRecogParam_t;


typedef struct _remotePushMsgCenterParam_t {/* these parameters are needed by message center */
	int timeStamp;
	int coordinationGrpNum;
	remotePushcoordination_t coordinate[2];
}remotePushMsgCenterParam_t;

typedef struct _remotePushMsgBody_t {
	remotePushMsgBaseParam_t baseParam;
	remotePushMsgCenterParam_t msgCenterParam;
	remotePushMsgFaceRecogParam_t faceRecogParam;
}remotePushMsgBody_t;

typedef struct _remotePushFileBody_t {
	void* data;
	int size;
	int dataType;
	char timeStr[32];
	char destName[32];
}remotePushFileBody_t;

typedef UINT32 (*remotePushCBType)(void*);

typedef struct _remotePushCBList_t{
	UINT32 (*msgStartPwrOffCB)(void*);			/* this Callback function is used to start power countdown flow for msg psuh */
	UINT32 (*msgStopPwrOffCB)(void*);			/* this Callback function is used to stop countdown flow for msg psuh*/
	UINT32 (*fileStartPwrOffCB)(void*);			/* this Callback function is used to start power countdown flow for file psuh*/
	UINT32 (*fileStopPwrOffCB)(void*);			/* this Callback function is used to stop countdown flow for file psuh*/
}remotePushCBList_t;

/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/
void remotePushModuleInit();
void remotePushModuleDeInit();
remotePushErrno_e remotePushModuleMsgPushTrigger(remotePushMsgBody_t *msgBody);
remotePushErrno_e remotePushModuleFilePushTrigger(remotePushFileBody_t *fileBody);
remotePushErrno_e remotePushModuleCBRegister(remotePushCBs_e sel, remotePushCBType cb);
#endif
