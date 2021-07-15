#ifndef __REMOTE_PUSH_IO_H__
#define __REMOTE_PUSH_IO_H__

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
remotePushErrno_e remotePushModuleSendMsg(remotePushGenericMsgParam_t *genericParam);
remotePushErrno_e remotePushModuleSendFile(remotePushGenericFileParam_t *genericParam);
#endif