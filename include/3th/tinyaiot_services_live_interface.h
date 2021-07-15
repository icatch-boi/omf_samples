#ifndef __TINYAIOT_SERVICES_LIVE_INTERFACE_H__
#define __TINYAIOT_SERVICES_LIVE_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tinyaiot_error_code.h"

/**
 * \brief Initialize Live Services module.
 *
 * \details
 *
 * \return TINYAIOT_SUCCEEDED or ErrorCode, reference to tinyaiotErrorCode_e.
 *
 * \attention (1) This function must be called before any others interface, and should be called once only.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesLiveInitialize(void);

/**
 * \brief DeInitialize Live Services module.
 *
 * \details
 *
 * \return TINYAIOT_SUCCEEDED or ErrorCode, reference to tinyaiotErrorCode_e.
 *
 * \attention (1) This function will destory any thread create by TinyAIoT Live Service, and release all resources.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesLiveDeInitialize(void);

#ifdef __cplusplus
}
#endif


#endif
