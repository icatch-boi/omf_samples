#ifndef __IOT_IO_H__
#define __IOT_IO_H__

/********************************************************************************/
/*                                  HEADER FILES                                */
/********************************************************************************/

#include "iotc_console.h"
/********************************************************************************/
/*                                     MACROS                                   */
/********************************************************************************/

/* Iot state */
#define IOT_READY_STR			"IoT_ready"
#define IOT_NOT_READY_STR		"IoT_not_ready"

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif


/********************************************************************************/
/*                                  TYPE DECLARATION                            */
/********************************************************************************/



/********************************************************************************/
/*                                 FUNCTION DECLARATION                         */
/********************************************************************************/
EXTERNC IotErrNo_e IotIoInit();
EXTERNC IotErrNo_e IotIoSend(void *pData, UINT32 dataSize);
EXTERNC IotErrNo_e IotIoReceive(void *pBuf, UINT32 bufSize, UINT32 *pBytesRecv, UINT32 timeoutMs);
EXTERNC IotErrNo_e IotIoDeinit();




#endif

