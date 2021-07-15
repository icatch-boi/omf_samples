#ifndef __IOT_RECEIVER_H__
#define __IOT_RECEIVER_H__

/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/

#include "iotc_console.h"

/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif



/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/
typedef struct __attribute__ ((packed, aligned(4))){
	UINT32 size;	/* headerSize + dataSize */
	char remoteID[IOT_REMOTE_ID_MAX_LENGTH];
}IotRxDataHeader_t;






/********************************************************************************/
/*                                 FUNCTION DECLARATION                         */
/********************************************************************************/
EXTERNC IotErrNo_e IotIOEvt(void *pData, UINT32 dataSize, UINT32 timeoutMs);
EXTERNC IotErrNo_e IotRxTaskCreate();




#endif

