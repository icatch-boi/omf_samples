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
#include <sys/time.h>
#include "iotc_console.h"
#include "iotc_debug_print.h"
#include "iotc_receiver.h"
#include "iotc_listen.h"
#include "iotc_queue.h"
#include "jansson.h"
#include "omf_api.h"
#include "omf_msg_site_user.h"
#include "ipc_shm/ipc_shm.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
/********************************************************************************/
/*                                     DECLARATION                              */
/********************************************************************************/
using namespace iot::queue;
/********************************************************************************/
/*                                       MACRO                                  */
/********************************************************************************/

#define IOT_CMD_RX_BUFFER_SIZE				1024
#define IOT_CMD_TX_BUFFER_SIZE				1024
#define IOT_CMD_MAX_NUM						5
#define KVS_BIN_NAME						"smf_start.sh"

#define SHM_PATHNAME		"/rom"
#define SHM_PROJECT_ID		8888
#define IPC_SHM_SIZE			128
#define IPC_SHM_KEY			1880222

/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/
typedef struct{
	pthread_t taskID;
	UINT32 gLoop;
	void *pSess;
}IotLoopTaskCtrl_t;

typedef struct {
	iot::queue::IotBlockingQueue *cmdQueue;
}IotCmdQueue_t;
typedef IotErrNo_e (*IotCmdProcCb)(const IotSession_t *pSess, json_t *jRoot);
typedef enum {
	IOT_CMD_ID_WAKE_UP = 0xFF00,
	IOT_CMD_ID_STREAM_START = 0xFF01,
}IotCmdIDs_e;
typedef struct {
	IotCmdIDs_e cmdID;
	IotCmdProcCb cmdProcCb;
}IotCmdProcCbs_t;
typedef enum {
	KVS_STATUS_NONE = 0,
	KVS_STATUS_BOOT = 1,
	KVS_STATUS_READY = 2,
	KVS_STATUS_DISCONNECT = 3,
	KVS_STATUS_EXIT = 4,
}kvsStatusType_e;
typedef enum {
	IOT_CMD_OPERATION_CMD = 0,
	IOT_CMD_OPERATION_SET = 1,
	IOT_CMD_OPERATION_GET = 2,
	IOT_CMD_OPERATION_SUPPORTED = 5,
	IOT_CMD_OPERATION_NONE,
}IotCmdOper_e;
/********************************************************************************/
/*                                  VARIABLES                                   */
/********************************************************************************/
static pthread_t gIotListenTaskID = 0;
static pthread_t gIotSmfStatusTaskID = 0;
static UINT32 gListenLoop = 1;
static UINT32 gSmfStatusLoop = 1;
static IotLoopTaskCtrl_t gIotCmdRecvTaskCtrl[IOT_SESSION_MAX_NUM] = {0};
static IotLoopTaskCtrl_t gIotCmdProcessTaskCtrl[IOT_SESSION_MAX_NUM] = {0};
static IotCmdQueue_t gIotCmdQue[IOT_SESSION_MAX_NUM] = {0};
static IotCmdProcCbs_t *gIotCmdProcCbList = NULL;
static kvsStatusType_e gKvsStatus = KVS_STATUS_NONE;
static const char *kvsMsgID = "kvs_2_iotc";
/********************************************************************************/
/*                                  FUNCTION DEFINITION                         */
/********************************************************************************/

/*
 * Name:IotCmdProcCbRegister
 * Brief:Register the processing function of command.
 * Args:
 * id[IotCmdIDs_e] command ID
 * cb[IotCmdProcCb]processing function of command.
 * return:[IotErrNo_e]errcode
 */
static IotErrNo_e IotCmdProcCbRegister(IotCmdIDs_e id, IotCmdProcCb cb)
{
	if(NULL == gIotCmdProcCbList) {
		unsigned int cbListSize = sizeof(IotCmdProcCbs_t) * IOT_CMD_MAX_NUM;
		gIotCmdProcCbList = (IotCmdProcCbs_t *)malloc(cbListSize);
		if(NULL == gIotCmdProcCbList) {
			return IOT_ERRNO_MALLOC;
		}
		memset(gIotCmdProcCbList, 0, cbListSize);
	}

	int idx = 0;
	for(idx=0; idx<IOT_CMD_MAX_NUM; idx++) {
		if(NULL == gIotCmdProcCbList[idx].cmdProcCb) {
			break;
		}
	}
	if(IOT_CMD_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}
	gIotCmdProcCbList[idx].cmdID = id;
	gIotCmdProcCbList[idx].cmdProcCb = cb;
	return IOT_ERRNO_SUCCESS;
}
/*
 * Name:IotCmdProcCbListRelease
 * Brief:Release the list which manage the processing function of command.
 * Args:None
 * return:[IotErrNo_e]errcode
 */
static IotErrNo_e IotCmdProcCbListRelease()
{
	if(NULL != gIotCmdProcCbList) {
		memset(gIotCmdProcCbList, 0, sizeof(IotCmdProcCbs_t) * IOT_CMD_MAX_NUM);
		gIotCmdProcCbList = NULL;
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 * Name:IotCmdProcCbFind
 * Brief:search the list which manage the processing function of command for the poniter to the processing function.
 * Args:
 * id[IotCmdIDs_e] command ID
 * return:[IotCmdProcCb]processing function.
 */
static IotCmdProcCb IotCmdProcCbFind(IotCmdIDs_e id)
{
	int idx = 0;
	for(idx=0; idx<IOT_CMD_MAX_NUM; idx++) {
		if(id == gIotCmdProcCbList[idx].cmdID) {
			break;
		}
	}
	if(IOT_CMD_MAX_NUM <= idx) {
		return NULL;
	}
	return gIotCmdProcCbList[idx].cmdProcCb;
}

/*
 *Name: IotCommandProcess
 *Brief:This function is responsible for distributing the received command to the corresponding processing function.
 *Args:
 *[const IotSession_t *]pSess --- the pointer to the session.
 *[const void *]pInputData --- pointer to the received data.
 *[UINT32]inputDataSize --- the size of received data.
 *Return:[IotErrNo_e] errcode
 */

static IotErrNo_e IotCommandProcess(const IotSession_t *pSess, const void *pInputData, int inputDataSize)
{
	dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]common process:cmd(%s)", __FUNCTION__, __LINE__, pInputData);
	if(NULL == pSess) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]pointer to session null", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	if((NULL == pInputData) || (0 == inputDataSize)) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]input data error", __FUNCTION__, __LINE__);
		return IOT_ERRNO_PARAM;
	}
	const char *jsonString = (const char*)pInputData;
	size_t jsonLength = (size_t)inputDataSize;
	json_error_t jsonErr;
	json_t *jroot = json_loadb(jsonString , jsonLength , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jsonErr);
	if(jroot) {
		json_t *jItemCmdID = json_object_get(jroot, "cmd");
		if (NULL != jItemCmdID) {
			IotCmdIDs_e cmdID = (IotCmdIDs_e)json_integer_value(jItemCmdID);
			IotCmdProcCb cmdProcCb = IotCmdProcCbFind(cmdID);
			if(NULL != cmdProcCb) {
				cmdProcCb(pSess, jroot);
			}
		}
	}
	if(NULL != jroot) {
		json_decref(jroot);
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotCommandReceiveTask
 *Brief:The task is to receive the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:void*
 */

void * IotCommandReceiveTask(void *param)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotCommandReceiveTask start", __FUNCTION__,__LINE__);
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]session is NULL", __FUNCTION__,__LINE__);
		return NULL;
	}
	IotSession_t *pSess = (IotSession_t *)param;
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(pSess == gIotCmdRecvTaskCtrl[idx].pSess) {
			break;
		}
	}
	if(IOT_SESSION_MAX_NUM <= idx) {
		return NULL;

	}
	gIotCmdQue[idx].cmdQueue = new IotBlockingQueue;
	if(NULL == gIotCmdQue[idx].cmdQueue) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] create command queue error", __FUNCTION__, __LINE__);
		return NULL;
	}

	while(gIotCmdRecvTaskCtrl[idx].gLoop) {
		UINT32 rxBufSize = IOT_CMD_RX_BUFFER_SIZE;
		char *rxBuf = (char*)malloc(rxBufSize);
		if(NULL == rxBuf) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]malloc cmd buffer fail", __FUNCTION__,__LINE__);
			return NULL;
		}
		memset(rxBuf, 0, rxBufSize);
		UINT32 readdedSize = 0;
		if(IOT_ERRNO_SUCCESS == IotRead(pSess, rxBuf, rxBufSize, &readdedSize, 0xFFFFFFFF)) {
			dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]recev cmd(%s)", __FUNCTION__,__LINE__,rxBuf);
			IotQueueElem cmdQueElem;
			cmdQueElem.DataBufBind(rxBuf);
			cmdQueElem.DataSize(readdedSize);
			std::function<bool(void**)> release = [](void** pBuf) {
				dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]free data(%s)", __FUNCTION__,__LINE__,*pBuf);
				if(!(*pBuf)) {
					return false;
				}
				free(*pBuf);
				*pBuf = NULL;
				return true;
			};
			cmdQueElem.ReleaseMethodBind(release);
			gIotCmdQue[idx].cmdQueue->Enqueue(cmdQueElem);
		}
		else {
			if(NULL != rxBuf) {
				free(rxBuf);
				rxBuf = NULL;
			}
		}
	}

	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotCommandReceiveTask end", __FUNCTION__,__LINE__);
	return NULL;
}

/*
 *Name: IotCommandReceiveTaskCreate
 *Brief:Create a task to receive the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:[IotErrNo_e] errcode
 */

static IotErrNo_e IotCommandReceiveTaskCreate(void *param)
{	
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]param NULL", __FUNCTION__,__LINE__);
		return IOT_ERRNO_PARAM;
	}
	
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(0 == gIotCmdRecvTaskCtrl[idx].taskID) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	if(0 == gIotCmdRecvTaskCtrl[idx].taskID) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotCommandReceiveTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;
		
		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandReceiveTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandReceiveTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandReceiveTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}
		
		/* create task */
		gIotCmdRecvTaskCtrl[idx].gLoop = 1;
		if(0 != pthread_create(&gIotCmdRecvTaskCtrl[idx].taskID, &attr,IotCommandReceiveTask, param)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandReceiveTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			gIotCmdRecvTaskCtrl[idx].gLoop = 0;
			return IOT_ERRNO_CREATE_OBJECT;
		}
		gIotCmdRecvTaskCtrl[idx].pSess = param;
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotCommandReceiveTaskCreate end", __FUNCTION__, __LINE__);
	}	
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotCommandReceiveTaskDestroy
 *Brief:Destroy the task which is to receive the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:[IotErrNo_e]errcode
 */

static IotErrNo_e IotCommandReceiveTaskDestroy(void *param)
{
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(param == gIotCmdRecvTaskCtrl[idx].pSess) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	memset(&gIotCmdRecvTaskCtrl[idx], 0, sizeof(IotLoopTaskCtrl_t));
	return IOT_ERRNO_SUCCESS;
}
/*
 *Name: IotCommandProcessTask
 *Brief:The task is to process the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:void*
 */
void * IotCommandProcessTask(void *param)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotCommandProcessTask start", __FUNCTION__,__LINE__);
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]session is NULL", __FUNCTION__,__LINE__);
		return NULL;
	}
	IotSession_t *pSess = (IotSession_t *)param;
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(pSess == gIotCmdProcessTaskCtrl[idx].pSess) {
			break;
		}
	}
	if(IOT_SESSION_MAX_NUM <= idx) {
		return NULL;

	}
	while(gIotCmdProcessTaskCtrl[idx].gLoop) {
		IotQueueElem cmdQueElem;
		if(NULL == gIotCmdQue[idx].cmdQueue || false == gIotCmdQue[idx].cmdQueue->Wait_dequeue_for(cmdQueElem, 0xFFFFFFFF)) {
			usleep(10*1000);
			continue;
		}
		void *pData = cmdQueElem.DataBuf();
		int dataSize = cmdQueElem.DataSize();
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]process cmd(%s)", __FUNCTION__,__LINE__, pData);
		IotCommandProcess(pSess, pData, dataSize);
	}
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotCommandProcessTask end", __FUNCTION__,__LINE__);
	return NULL;
}

/*
 *Name: IotCommandProcessTaskCreate
 *Brief:Create a task to process the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:[IotErrNo_e] errcode
 */

static IotErrNo_e IotCommandProcessTaskCreate(void *param)
{
	if(NULL == param) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]param NULL", __FUNCTION__,__LINE__);
		return IOT_ERRNO_PARAM;
	}

	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(0 == gIotCmdProcessTaskCtrl[idx].taskID) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	if(0 == gIotCmdProcessTaskCtrl[idx].taskID) {
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotCommandProcessTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;

		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandProcessTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandProcessTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandProcessTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* create task */
		gIotCmdProcessTaskCtrl[idx].gLoop = 1;
		if(0 != pthread_create(&gIotCmdProcessTaskCtrl[idx].taskID, &attr,IotCommandProcessTask, param)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotCommandProcessTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			gIotCmdProcessTaskCtrl[idx].gLoop = 0;
			return IOT_ERRNO_CREATE_OBJECT;
		}
		gIotCmdProcessTaskCtrl[idx].pSess = param;
		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d] IotCommandProcessTaskCreate end", __FUNCTION__, __LINE__);
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotCommandProcessTaskDestroy
 *Brief:Destroy the task which is to process the command from remote app.
 *Args:
 * [void*]param --- the pointer to the corresponding session.
 *Return:[IotErrNo_e]errcode
 */

static IotErrNo_e IotCommandProcessTaskDestroy(void *param)
{
	int idx = 0;
	for(idx=0; idx<IOT_SESSION_MAX_NUM; idx++) {
		if(param == gIotCmdRecvTaskCtrl[idx].pSess) {
			break;
		}
	}

	if(IOT_SESSION_MAX_NUM <= idx) {
		return IOT_ERRNO_EXCEED_MAX;
	}

	memset(&gIotCmdRecvTaskCtrl[idx], 0, sizeof(IotLoopTaskCtrl_t));
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
		IotCommandReceiveTaskCreate(pSess);
		IotCommandProcessTaskCreate(pSess);
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

/*
 *Name: IotProcKvsMsg
 *Brief:this function is a callback function to process the message from KVS.
 *Args:
 *[void*]priv_hd --- reserved param
 *[const void *] data --- msg from kvs.
 *[int]size --- the size of msg from kvs.
 *Return:[int]
 */
int IotProcKvsMsg(void* priv_hd, const void *data, int size)
{
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]ksv msg(%s)", __FUNCTION__,__LINE__, data);
	const char *jsonString = (const char*)data;
	size_t jsonLength = (size_t)size;
	json_error_t jsonErr;
	json_t *jroot = json_loadb(jsonString , jsonLength , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jsonErr);
	if(jroot) {
		json_t *jItemStatus = json_object_get(jroot, "status");
		if (NULL != jItemStatus) {
			gKvsStatus = (kvsStatusType_e)json_integer_value(jItemStatus);
			dbgLogPrint(IOT_LVL_INFO, "[%s:%d]ksv status(%d)", __FUNCTION__,__LINE__, gKvsStatus);
		}
	}
	if(NULL != jroot) {
		json_decref(jroot);
	}
	return 1;
}

/*
 *Name: IotSmfStatusTask
 *Brief:
 *Args:[void *]arg
 *Return:void
 */
void * IotSmfStatusTask(void *arg)
{
	int shm = ipc_shm_create(IPC_SHM_KEY, IPC_SHM_SIZE, IPC_CREAT|0666);
	if(shm <= 0) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]ipc_shm_create error", __FUNCTION__, __LINE__);
		return NULL;
	}

	char *pBuff = (char*)malloc(IPC_SHM_SIZE);
	if(NULL == pBuff) {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]no memory", __FUNCTION__, __LINE__);
		return NULL;
	}
	while(gSmfStatusLoop) {
		memset(pBuff, 0, IPC_SHM_SIZE);
		if(ipc_shm_read(shm, pBuff, IPC_SHM_SIZE) < 0) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]ipc_shm_read error", __FUNCTION__, __LINE__);
			return -1;
		}
		const char *jsonString = (const char*)pBuff;
		size_t jsonLength = (size_t)strlen(jsonString);
		if(0 == jsonLength) {
			usleep(10*1000);
			continue;
		}

		json_error_t jsonErr;
		json_t *jroot = json_loadb(jsonString , jsonLength , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jsonErr);
		if(jroot) {
			json_t *jItemStatus = json_object_get(jroot, "status");
			if (NULL != jItemStatus) {
				gKvsStatus = (kvsStatusType_e)json_integer_value(jItemStatus);
				// dbgLogPrint(IOT_LVL_INFO, "[%s:%d]ksv status(%d)", __FUNCTION__,__LINE__, gKvsStatus);
			}
		}
		if(NULL != jroot) {
			json_decref(jroot);
		}
		usleep(10*1000);
	}
	ipc_shm_delete(&shm, IPC_SHM_DETACH|IPC_SHM_RMID);
	dbgLogPrint(IOT_LVL_INFO, "[%s:%d]IotSmfStatusTask end", __FUNCTION__,__LINE__);
	gIotSmfStatusTaskID = 0;
	return NULL;
}
/*
 *Name: IotSmfStatusTaskCreate
 *Brief:
 *Args:void
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotSmfStatusTaskCreate()
{
	if(0 == gIotSmfStatusTaskID) {
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotSmfStatusTaskCreate start", __FUNCTION__, __LINE__);
		pthread_attr_t attr;

		/* default initialize thread attribute */
		if(0 != pthread_attr_init(&attr)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotSmfStatusTaskCreate::pthread_attr_init", __FUNCTION__, __LINE__);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* set detach attribute */
		if(0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotSmfStatusTaskCreate::pthread_attr_setdetachstate", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* set inherit schedual attribute */
		if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotSmfStatusTaskCreate::pthread_attr_setinheritsched", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* create task */
		if(0 != pthread_create(&gIotSmfStatusTaskID, &attr,&IotSmfStatusTask, NULL)) {
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d] IotSmfStatusTaskCreate::pthread_create", __FUNCTION__, __LINE__);
			pthread_attr_destroy(&attr);
			return IOT_ERRNO_CREATE_OBJECT;
		}

		/* destroy attribute */
		pthread_attr_destroy(&attr);
		dbgLogPrint(IOT_LVL_INFO, "[%s:%d] IotSmfStatusTaskCreate end", __FUNCTION__, __LINE__);
	}
	return IOT_ERRNO_SUCCESS;
}

/*
 *Name: IotListenTaskDestroy
 *Brief:destroy the task which is to listen connection from remote.
 *Args:void
 *Return:[IotErrNo_e]
 */
static IotErrNo_e IotSmfStatusTaskDestroy()
{
	if(0 != gIotSmfStatusTaskID) {
		gSmfStatusLoop = 0;
	}
	return IOT_ERRNO_SUCCESS;
}
/*
 *Name: IotCmdProcWakeup
 *Brief:processing function to process "wakeup" command.
 *Args:
 *[const IotSession_t *]pSess --- the corresponding session.
 *[const char *] pJsonString --- command JSON string.
 *[UINT32]jsonStringLength --- the size of command JSON string.
 *Return:[IotErrNo_e]errcode
 */
IotErrNo_e IotCmdProcWakeup(const IotSession_t *pSess, json_t *jRoot)
{
	/* parse command */
	if(NULL == jRoot) {
		return 	IOT_ERRNO_PARAM;
	}
	unsigned long long transID = 0;
	UINT32 cmdID = 0;
	IotCmdOper_e operation = IOT_CMD_OPERATION_NONE;

	/* transaction ID */
	json_t *jItemTransID = json_object_get(jRoot, "transid");
	if (NULL != jItemTransID) {
		transID = (unsigned long long)json_integer_value(jItemTransID);
	}

	/* command ID */
	json_t *jItemCmdID = json_object_get(jRoot, "cmd");
	if (NULL != jItemCmdID) {
		cmdID = (UINT32)json_integer_value(jItemCmdID);
	}

	/* operation */
	json_t *jItemOper = json_object_get(jRoot, "operation");
	if (NULL != jItemOper) {
		operation = (IotCmdOper_e)json_integer_value(jItemOper);
	}

	dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d](%llu-%u-%u)", __FUNCTION__, __LINE__, transID, cmdID, operation);
	/* process */
	int errcode = 0;

	/* response */
	const char *jsonRespStr = "{\"transid\": 12345,\"cmd\": 123455,\"err\": 0,\"ret\": null}";
	size_t jsonRespLength = strlen(jsonRespStr)+1;
	json_error_t jsonRespErr;
	json_t *jrootResp = json_loadb(jsonRespStr, jsonRespLength , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jsonRespErr);
	if(jrootResp) {
		/* transaction ID */
		json_t *jItemTransID = json_object_get(jrootResp, "transid");
		if (NULL != jItemTransID) {
			json_integer_set(jItemTransID, transID);
		}

		/* command ID */
		json_t *jItemCmdID = json_object_get(jrootResp, "cmd");
		if (NULL != jItemCmdID) {
			json_integer_set(jItemCmdID, cmdID);
		}

		/* errcode */
		json_t *jItemErr = json_object_get(jrootResp, "err");
		if (NULL != jItemErr) {
			json_integer_set(jItemErr, errcode);
		}
		/* value */
	}
	else {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]jerr(column:%d,line:%d, position:%d, source:%s, text:%s)", \
					__FUNCTION__, __LINE__,jsonRespErr.column, jsonRespErr.line, jsonRespErr.position, jsonRespErr.source, jsonRespErr.text);
		return IOT_ERRNO_PARAM;
	}
	/* send out */
	if(jrootResp) {
		char *jsonResp = NULL;
		jsonResp = json_dumps(jrootResp,JSON_ENCODE_ANY);
		if(jsonResp) {
			dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]wakeup response:(%s)", __FUNCTION__, __LINE__, jsonResp);
			IotWrite(pSess, (const void*)jsonResp, strlen(jsonResp)+1, 100);
			free(jsonResp);
			jsonResp = NULL;
		}
		json_decref(jrootResp);
	}
	return IOT_ERRNO_SUCCESS;
}
/*
 *Name: IotCmdProcStreamStart
 *Brief:processing function to process "stream start" command.
 *Args:
 *[const IotSession_t *]pSess --- the corresponding session.
 *[const char *] pJsonString --- command JSON string.
 *[UINT32]jsonStringLength --- the size of command JSON string.
 *Return:[IotErrNo_e]errcode
 */
IotErrNo_e IotCmdProcStreamStart(const IotSession_t *pSess, json_t *jRoot)
{
	/* parse command */
	if(NULL == jRoot) {
		return 	IOT_ERRNO_PARAM;
	}

	unsigned long long transID = 0;
	UINT32 cmdID = 0;
	IotCmdOper_e operation = IOT_CMD_OPERATION_NONE;
	/* transaction ID */
	json_t *jItemTransID = json_object_get(jRoot, "transid");
	if (NULL != jItemTransID) {
		transID = (unsigned long long)json_integer_value(jItemTransID);
	}

	/* command ID */
	json_t *jItemCmdID = json_object_get(jRoot, "cmd");
	if (NULL != jItemCmdID) {
		cmdID = (UINT32)json_integer_value(jItemCmdID);
	}

	/* operation */
	json_t *jItemOper = json_object_get(jRoot, "operation");
	if (NULL != jItemOper) {
		operation = (IotCmdOper_e)json_integer_value(jItemOper);
	}


	/* process */
	int errcode = -1;
	int alreadyExecute = 0;
	int retry = 5000;
	char tp[16];
	while(retry--){
		if(KVS_STATUS_READY == gKvsStatus) {
			errcode = 0;
			dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]kvs ready" , __FUNCTION__, __LINE__);
			break;
		}
		else if(KVS_STATUS_EXIT==gKvsStatus) {
			if(0 != alreadyExecute) {
				usleep(10*1000);
				continue;
			}
			// usleep(2000*1000);

			struct timeval tv;
    		gettimeofday(&tv, NULL);
			int status = system(KVS_BIN_NAME);
			printf("[%lld:%06lld] %s\n", (unsigned long long)tv.tv_sec, (unsigned long long)tv.tv_usec, KVS_BIN_NAME);
			if(-1 == status) {
				errcode = -1;
				dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]execute %s fail" , __FUNCTION__, __LINE__, KVS_BIN_NAME);
				break;
			}
			else {
				if(!WIFEXITED(status)) {
					errcode = -1;
					dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]execute %s failed" , __FUNCTION__, __LINE__, KVS_BIN_NAME);
					break;
				}
				else
				{
					if (0 != WEXITSTATUS(status))
					{
						errcode = -1;
						dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]execute %s failed" , __FUNCTION__, __LINE__, KVS_BIN_NAME);
						break;
					}
					else
					{
						dbgLogPrint(IOT_LVL_INFO, "[%s:%d]execute %s succeed" , __FUNCTION__, __LINE__, KVS_BIN_NAME);
						alreadyExecute = 1;
						continue;
					}
				}
			}
			/* boot KVS application */
		}
		else if(KVS_STATUS_BOOT==gKvsStatus || KVS_STATUS_DISCONNECT==gKvsStatus || KVS_STATUS_NONE==gKvsStatus) {
			usleep(10*1000);
			continue;
			/* wait KVS ready */
		}
		else {
			errcode = -1;
			break;
		}
	}

	/* response */
	const char *jsonRespStr = "{\"transid\": 12345,\"cmd\": 123456,\"err\": 0,\"ret\": null, \"tp\": \"0\"}";
	size_t jsonRespLength = strlen(jsonRespStr)+1;
	json_error_t jsonRespErr;
	json_t *jrootResp = json_loadb(jsonRespStr, jsonRespLength , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jsonRespErr);
	if(jrootResp) {
		/* transaction ID */
		json_t *jItemTransID = json_object_get(jrootResp, "transid");
		if (NULL != jItemTransID) {
			json_integer_set(jItemTransID, transID);
		}

		/* command ID */
		json_t *jItemCmdID = json_object_get(jrootResp, "cmd");
		if (NULL != jItemCmdID) {
			json_integer_set(jItemCmdID, cmdID);
		}

		/* errcode */
		json_t *jItemErr = json_object_get(jrootResp, "err");
		if (NULL != jItemErr) {
			json_integer_set(jItemErr, errcode);
		}
		/* tp */
		json_t *jItemTp = json_object_get(jrootResp, "tp");
		if (NULL != jItemTp) {
			unsigned long long timePointMs;
			struct timeval tv;
    		gettimeofday(&tv, NULL);
			timePointMs = (unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000;
			memset(tp, 0, sizeof(tp));
			sprintf(tp, "%lld", timePointMs);
			json_string_set(jItemTp, tp);
		}
	}
	else {
		dbgLogPrint(IOT_LVL_ERROR, "[%s:%d]jerr(column:%d,line:%d, position:%d, source:%s, text:%s)", \
					__FUNCTION__, __LINE__,jsonRespErr.column, jsonRespErr.line, jsonRespErr.position, jsonRespErr.source, jsonRespErr.text);
		return IOT_ERRNO_PARAM;
	}
	/* send out */
	if(jrootResp) {
		char *jsonResp = NULL;
		jsonResp = json_dumps(jrootResp,JSON_ENCODE_ANY);
		if(jsonResp) {
			dbgLogPrint(IOT_LVL_DEBUG, "[%s:%d]stream start response:(%s)", __FUNCTION__, __LINE__, jsonResp);
			struct timeval tv;
			gettimeofday(&tv, NULL);
			printf("[%lld:%06lld] %s\n", (unsigned long long)tv.tv_sec, (unsigned long long)tv.tv_usec, "IotWrite");
			IotWrite(pSess, (const void*)jsonResp, strlen(jsonResp)+1, 100);
			free(jsonResp);
			jsonResp = NULL;
		}
		json_decref(jrootResp);
	}
	return IOT_ERRNO_SUCCESS;
}

int main(int argc, char **argv)
{
	omfInit(0);
	IotInit();
	IotSmfStatusTaskCreate();
	IotCmdProcCbRegister(IOT_CMD_ID_WAKE_UP, IotCmdProcWakeup);
	IotCmdProcCbRegister(IOT_CMD_ID_STREAM_START, IotCmdProcStreamStart);
	IotListenTaskCreate();
	sleep(10000);
	IotListenTaskDestroy();
	IotDeinit();
	omfUninit(0);
	return 0;
}
