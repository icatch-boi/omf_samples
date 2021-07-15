#ifndef __REMOTE_PUSH_IO_H__
#define __REMOTE_PUSH_IO_H__
/**************************************************************************
 *                          I N C L U D E S                               *
 **************************************************************************/
#include "tinyaiot_data_def.h"
#include "tinyaiot_interface.h"
#include "tinyaiot_dbg_api.h"
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SERVER_URL_MAX_LENGTH 256
#define INI_SECTION_SYS				"SYSTEM"
#define INI_KEY_PUSHSERVER			"PUSHSERVER"
#define INI_KEY_PUSHSERVPORT		"PUSHSERVPORT"
#define INI_KEY_UUID				"UUID"
#define INI_KEY_DEVID				"DEVID"
#define INI_KEY_DEVAKEY				"DEVAKEY" /* authorization key between device and server */
#define INI_KEY_TUTK_COMPATIBLE				"TUTKCOMPATIBLE"	
#define INI_KEY_OAUTH2_CLIENT_ID	"OAUTH2CLIENTID"
#define INI_KEY_OAUTH2_CLIENT_SECRET	"OAUTH2CLIENTSECRET"
#define INI_KEY_OAUTH2_CLIENT_URI	"OAUTH2CLIENTURI"

#define INI_CFG_FILENAME 			"/mnt/diskb/UDF/DOORBELL.CFG"
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _remotePushGenericMsgParam_t{
	int  msgType;		/* [necessary] message type,eg: 100 indicates motion detection message */
	char time[32];		/* [necessary] IOS time when message occurrs */
	int msgId;			/* [necessary] sequence number of message */
	char *msgParam;		/* [optional] */
	char *attachment;	/* [optional] */
	int  errCode;		/* [optional] */
	char* extraParam;  	/*[optional] json format string */
	int format;		 	/* [necessary]  4: tutk push compatible, others:standard tinyaiot push;  */
}remotePushGenericMsgParam_t;

typedef struct _remotePushGenericFileParam_t{
	void* data;	/* data-pointer */
	int size; /* data size */
	int dataType; /* type of data */
	char timeStr[32]; /* time when file-pusing */
	char destName[32]; /* filename when file is saved in server */
}remotePushGenericFileParam_t;

/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/
int remotepushModuleTutkCompatibleModeGet();
remotePushErrno_e remotePushModuleIOInit();
remotePushErrno_e remotePushModuleIODeinit(void);
remotePushErrno_e remotePushModuleSendMsg(remotePushGenericMsgParam_t *genericParam);
remotePushErrno_e remotePushModuleSendFile(remotePushGenericFileParam_t *genericParam);
#endif