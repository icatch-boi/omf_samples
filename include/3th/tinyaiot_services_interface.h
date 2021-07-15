#ifndef __TINYAIOT_SERVICES_INTERFACE_H__
#define __TINYAIOT_SERVICES_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tinyaiot_error_code.h"
#include "tinyaiot_services_def.h"

/**
 * \brief get cloud services credentials information for device from TinyAIoT Service.
 *
 * \details A client want to get device credentials information by TinyAIoT Service, please call it.
 *
 * \param credentials[in/out] the credentials information, type is required.
 * \param credentials.type[in] the credentials type information, one of enum _tinyaiotCloudServicesCredentialId_e.
 *
 * \return TINYAIOT_SUCCEEDED or tinyaiotErrorCode_e.
 *
 * \attention (1) Return credentials buffer will be freed by tinyAIoT_ServicesCredentialsFree or oneself.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesCredentialsGet(tinyaiotCloudServicesCredentials_t *credentials);

/**
 * \brief free TinyAIoT device credentials information buffer.
 *
 * \details A client want to free credentials information buffer after each use, please call it.
 *
 *
 * \return TINYAIOT_SUCCEEDED or tinyaiotErrorCode_e.
 *
 * \attention (1) This function should be call after tinyAIoT_ServicesCredentialsGet.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesCredentialsFree(void);

/**
 * \brief get cloud services type name by functionid from TinyAIoT Service.
 *
 * \details A client want to get cloud services type from TinyAIoT Service, please call it.
 *
 * \param id[in] one of enum _tinyaiotCloudServicesFunctionId_e.
 *
 * \return services type one of enum _tinyaiotCloudServicesTypeId_e, fail is TINYAIOT_CLOUDSERVICES_TID_MAX.
 *
 * \attention (1) Return services type buffer will be cache by sdk.
 */
tinyaiotCloudServicesTypeId_e tinyAIoT_ServicesTypeGet(tinyaiotCloudServicesFunctionId_e id);

/**
 * \brief get cloud services information from TinyAIoT Service.
 *
 * \details A client want to get cloud services information by TinyAIoT Service, please call it.
 *
 * \param services[in/out] the services information, type is required.
 * \param services.type[in] the services type information, one of enum tinyaiotCloudServicesTypeId_e.
 *
 * \return TINYAIOT_SUCCEEDED or tinyaiotErrorCode_e.
 *
 * \attention (1) Return services information buffer will be freed by tinyAIoT_ServicesInfoFree or oneself.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesInfoGet(tinyaiotCloudServicesInfo_t *services);

/**
 * \brief free TinyAIoT device services information buffer.
 *
 * \details A client want to free services information buffer after each use, please call it.
 *
 *
 * \return TINYAIOT_SUCCEEDED or tinyaiotErrorCode_e.
 *
 * \attention (1) This function should be call after tinyAIoT_ServicesInfoGet.
 */
tinyaiotErrorCode_e tinyAIoT_ServicesInfoFree(void);

#ifdef __cplusplus
}
#endif


#endif
