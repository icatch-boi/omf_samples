/********************************************************************************/
/*                                 DEBUG LOG CONTROL                            */
/********************************************************************************/

#define IOT_PRINT_LOG_EN	1
#define IOT_DEBUG_DEF_LEVEL		IOT_LVL_DEBUG

/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "iotc_console.h"
#include "iotc_debug_print.h"
#include "iotc_receiver.h"
#include "iotc_listen.h"

/********************************************************************************/
/*                                       MACRO                                  */
/********************************************************************************/

#define LOOP_TASK_RX_BUFFER_SIZE				1024
#define LOOP_TASK_TX_BUFFER_SIZE				1024

/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/
typedef struct{
	pthread_t taskID;
	UINT32 gLoop;
	void *pSess;
}IotLoopTaskCtrl_t;

/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static pthread_t gIotListenTaskID = 0;
static UINT32 gListenLoop = 1;
static UINT32 gLoopCtrl = 1;
static IotLoopTaskCtrl_t gIotLoopTaskCtrl[IOT_SESSION_MAX_NUM] = {0};

/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/

/*
 *Name: IotLoopTaskProcess
 *Brief:Process the received data.
 *Args:
 *[const IotSession_t *]pSess --- the pointer to the session.
 *[const void *]pInputData --- pointer to the received data.
 *[UINT32]inputDataSize --- the size of received data.
 *[void *]pOutputBuf --- the pointer to the response buffer.
 *[UINT32]outputBufSize --- the size of response buffer.
 *[UINT32 *]pOutputDataSize --- the size of response data.
 *Return:[IotErrNo_e]
 */

static IotErrNo_e IotLoopTaskProcess(const IotSession_t *pSess, const void *pInputData, UINT32 inputDataSize, void *pOutputBuf, UINT32 outputBufSize, UINT32 *pOutputDataSize)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]cmd(%s)", __FUNCTION__,__LINE__, (char*)pInputData);

	sprintf((char*)pOutputBuf, "%s:%s", "Device got command", (char*)pInputData);
	*pOutputDataSize = strlen((char*)pOutputBuf)+1;
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]resp(%s)", __FUNCTION__,__LINE__, (char*)pOutputBuf);
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotLoopTask
 *Brief:The task is to receive and process the data from remote app.
 *Args:[void*]param
 *Return:void*
 */

void * IotLoopTask(void *param)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotLoopTask start", __FUNCTION__,__LINE__);
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]session is NULL", __FUNCTION__,__LINE__);
		return NULL;
	}
	IotSession_t *pSess = (IotSession_t *)param;
	UINT32 rxBufSize = LOOP_TASK_RX_BUFFER_SIZE;
	UINT32 txBufSize = LOOP_TASK_TX_BUFFER_SIZE;
	char *rxBuf = (char*)malloc(rxBufSize);
	char *txBuf = (char*)malloc(txBufSize);
	if(NULL == rxBuf) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]malloc cmd buffer fail", __FUNCTION__,__LINE__);
		return NULL;
	}
	if(NULL == txBuf) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]malloc response buffer fail", __FUNCTION__,__LINE__);
		return NULL;
	}
	while(gLoopCtrl) {
		memset(rxBuf, 0, rxBufSize);
		memset(txBuf, 0, txBufSize);
		UINT32 readdedSize = 0;
		if(IOT_ERRNO_SUCCESS == IotRead(pSess, rxBuf, rxBufSize, &readdedSize, 0xFFFFFFFF)) {
			UINT32 txDataSize = 0;
			IotLoopTaskProcess(pSess, rxBuf, readdedSize, txBuf, txBufSize, &txDataSize);

			if(0 < txDataSize) {
				IotWrite(pSess, txBuf, txDataSize, 100);
			}
		}
		usleep(50*1000);
	}
	if(NULL != rxBuf) {
		free(rxBuf);
		rxBuf = NULL;
	}
	if(NULL != txBuf) {
		free(txBuf);
		txBuf = NULL;
	}
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotLoopTask end", __FUNCTION__,__LINE__);
	return NULL;
}

/*
 *Name: IotLoopTaskCreate
 *Brief:Create a task to receive and process the data from remote app.
 *Args:[void*]param --- the pointer to the session.
 *Return:[IotErrNo_e]
 */

static IotErrNo_e IotLoopTaskCreate(void *param)
{	
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]param NULL", __FUNCTION__,__LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(0 == gIotLoopTaskCtrl[idx].taskID) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	if(0 == gIotLoopTaskCtrl[idx].taskID) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotLoopTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotLoopTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotLoopTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotLoopTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* create task */
		gIotLoopTaskCtrl[idx].gLoop = 1;
		if(0 != pthread_create(&gIotLoopTaskCtrl[idx].taskID, &attr,IotLoopTask, param)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotLoopTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			gIotLoopTaskCtrl[idx].gLoop = 0;
			return IOT_ERRNO_CREATE_OBJECT;
		}
		gIotLoopTaskCtrl[idx].pSess = param;
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotLoopTaskCreate end", __FUNCTION__, __LINE__);
	}	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotLoopTaskDestroy
 *Brief:Destroy the task which is to receive and process the command from remote app.
 *Args:[void*]param --- the pointer to the session.
 *Return:[IotErrNo_e]
 */

static IotErrNo_e IotLoopTaskDestroy(void *param)
{
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(param == gIotLoopTaskCtrl[idx].pSess) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	memset(&gIotLoopTaskCtrl[idx], 0, sizeof(IotLoopTaskCtrl_t));
	return IOT_ERRNO_SUCCESS;
}



/*
 *Name: IotListenTask
 *Brief:task to listen connection from remote.
 *Args:[void *]arg
 *Return:void
 */
void * IotListenTask(void *arg)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotListenTask start", __FUNCTION__,__LINE__);
	while(gListenLoop) {
		char remoteID[IOT_REMOTE_ID_MAX_LENGTH] = {0};
		if(IOT_ERRNO_SUCCESS != IotListen(remoteID, 0xFFFFFFFF)) {
			continue;
		}
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d]listen a connection", __FUNCTION__,__LINE__);
		/* create session */
		IotSession_t *pSess = NULL;
		if(IOT_ERRNO_SUCCESS != IotCreateSession(remoteID, &pSess)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]create session fail", __FUNCTION__,__LINE__);
			continue;
		}
		/* create loop task */
		IotLoopTaskCreate(pSess);
		usleep(20*1000);	
	}
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotListenTask end", __FUNCTION__,__LINE__);
	gIotListenTaskID = 0;
	return NULL;
}

/*
 *Name: IotListenTaskCreate
 *Brief:create a task to listen connection from remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotListenTaskCreate()
{
	if(0 == gIotListenTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotListenTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotListenTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotListenTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotListenTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* create task */
		if(0 != pthread_create(&gIotListenTaskID, &attr,&IotListenTask, NULL)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotListenTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotListenTaskCreate end", __FUNCTION__, __LINE__);
	}	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotListenTaskDestroy
 *Brief:destroy the task which is to listen connection from remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotListenTaskDestroy()
{
	if(0 != gIotListenTaskID) {
		gListenLoop = 0;
	}	
	return IOT_ERRNO_SUCCESS;
}


int main(int argc, char **argv)
{
	IotInit();
	IotListenTaskCreate();
	sleep(100);
	IotListenTaskDestroy();
	IotDeinit();
	return 0;
}



















