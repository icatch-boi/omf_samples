#ifndef __REMOTE_PUSH_QUEUE_H__
#define __REMOTE_PUSH_QUEUE_H__

/**************************************************************************
 *                           I N C L U D E S	                           *
 **************************************************************************/
#include "remote_push_errno.h"
#include <sys/queue.h>







/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/ 
typedef unsigned int UINT32;
typedef UINT32 remotePushQueue;

typedef struct queNode{
	TAILQ_ENTRY(queNode) next;
	void *data;
	UINT32 dataSize;
}remotePushQueueNode_t;

/**************************************************************************
 *                           FUNCTION DECLARATION                           *
 **************************************************************************/
remotePushErrno_e remotePushQueueCreate(remotePushQueue **ppQueue);
remotePushErrno_e remotePushQueueSend(const remotePushQueue *pQueue, remotePushQueueNode_t *pNode);
remotePushErrno_e remotePushQueueReceive(const remotePushQueue *pQueue, remotePushQueueNode_t **ppNode, UINT32 timeoutMs);
remotePushErrno_e remotePushQueueDestroy(remotePushQueue **ppQueue);

#endif
