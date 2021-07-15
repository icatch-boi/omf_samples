#ifndef __IOT_CONSOLE_H__
#define __IOT_CONSOLE_H__

/********************************************************************************/
/*                                    HEADERS                                   */
/********************************************************************************/
#include <pthread.h>
#include "iotc_queue.h"

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


#define IOT_SESSION_RX_QUEUE_MAX_ELEM			3
#define IOT_SESSION_TX_QUEUE_MAX_ELEM			3
/* session */
#define IOT_SESSION_MAX_NUM					4
#define IOT_REMOTE_ID_MAX_LENGTH			64

typedef enum {
	IOT_NOT_READY=0, 
	IOT_READY=1
}IotReady_t;

/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/
typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef enum {
	IOT_ERRNO_SUCCESS 		= 0,
	IOT_ERRNO_PARAM	  		= 1,
	IOT_ERRNO_MALLOC		= 2,
	IOT_ERRNO_CONDITION		= 3,	/* some necessary condition not met */
	IOT_ERRNO_TIMEOUT		= 4,
	IOT_ERRNO_EXCEED_MAX	= 5,
	IOT_ERRNO_CREATE_OBJECT	= 6,
	IOT_ERRNO_DESTROY_OBJECT= 7,
	IOT_ERRNO_SEND          = 8,
	IOT_ERRNO_READ          = 9,
	IOT_ERRNO_UNKNOWN,
}IotErrNo_e;
	
typedef struct {
	iot::queue::IotBlockingQueue *rxQueue;
	iot::queue::IotBlockingQueue *txQueue;
}IotSessQueueAttr_t;

typedef struct IotClient {
	char remoteID[IOT_REMOTE_ID_MAX_LENGTH];
	IotSessQueueAttr_t queueAttr;
}IotSession_t;

/********************************************************************************/
/*                                 FUNCTION DECLARATION                         */
/********************************************************************************/
/*
 *Name: IotInit
 *Brief:Initialize IOT communication mechanism.Only initialize once.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotInit();

/*
 *Name: IotListen
 *Brief:Listen connection from remote.
 *Args:
 *[char *] pRemoteID --- pointer to the remote ID.
 *[UINT32] timeOutMs --- timeout in ms.
 *Return:IotErrNo_e
 */

EXTERNC IotErrNo_e IotListen(char *pRemoteID, UINT32 timeOutMs);


/*
 *Name: IotCreateSession
 *Brief:Create an object of IotSession_t specified by the remote ID.
 *Args:
 *[const char*]remoteID --- ID of remote client.
 *[IotSession_t **]ppSess --- pointer to the destination object of IotSession_t.
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotCreateSession(const char *remoteID, IotSession_t **ppSess);

/*
 *Name: IotRead
 *Brief:Read data via the specific session.
 *Args:
 *[const IotSession_t *]pSess --- the pointer to the owner of session.
 *[void*]pBuffer --- buffer to receive data.
 *[UINT32]bufSize --- the size of buffer.
 *[UINT32 *]pReadSize --- the actual readded size of data.
 *[UINT32]timeoutMs --- timeout in ms.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotRead(const IotSession_t *pSess, void *pBuffer, UINT32 bufSize, UINT32 *pReadSize, UINT32 timeoutMs);

/*
 *Name: IotWrite
 *Brief:Write data via the specific session.
 *Args:[const IotSession_t *]pSess --- the pointer to the object of session. 
 *[void*]pData --- pointer to the data.
 *[UINT32]dataSize --- the size of data.
 *[UINT32]timeoutMs --- timeout in ms.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotWrite(const IotSession_t *pSess, const void *pData, UINT32 dataSize, UINT32 timeoutMs);


/*
 *Name: IotDestroySession
 *Brief:Destroy the session object specified by the pointer.
 *Args:[IotSession_t *]pSess --- pointer to the object which is to be destroied.
 *Return:[IotErrNo_e]
 */

EXTERNC IotErrNo_e IotDestroySession(IotSession_t *pSess);

/*
 *Name: IotDeinit
 *Brief:Denitialize IOT communication mechanism.Only deinitialize once.
 *Args:void
 *Return:[IotErrNo_e]
 */
EXTERNC IotErrNo_e IotDeinit();

/*
 *Name: IotSessionList
 *Brief:Get the pointer to session list.
 *Args:void.
 *Return:[IotSession_t *]pointer to session list.
 */

EXTERNC IotSession_t * IotSessionList();


#endif
