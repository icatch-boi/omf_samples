#ifndef __TINYAIOT_INTERFACE_H__
#define __TINYAIOT_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tinyaiot_error_code.h"
#include "tinyaiot_data_def.h"

/**
 * \brief TinyAIoT module initialization.
 *
 * \details A device client want to use TinyAIoT Service, must call it first.
 *
 * \param itransType[in] The connection protocol to TinyAIoT Service, one of tinyaiotTransTypeId_e.
 * \param cfgRoot[in]    The absolute path used to save TinyAIoT device config files,
 *                         it must be a directory and app has permissions to read and write,
 *                         make sure the path real existence.
 *
 * \return SUCCESS or FAIL.
 *
 * \attention (1) This function must be called before any others interface, and should be called once only.
 */
int tinyAIoT_Initialize(tinyaiotTransTypeId_e itransType, char *cfgRoot);
/**
 * \brief TinyAIoT module initialization2.
 *
 * \details A device client want to use TinyAIoT Service, must call it first.
 *  The different between this function and tinyAIoT_Initialize() is this function will not save
 *  device info automatically, developer should provide save and read callback function for using.
 *
 * \param itransType[in]  The connection protocol to TinyAIoT Service, one of tinyaiotTransTypeId_e.
 * \param saveHandler[in] The callback function for save device information by developer's defined.
 * \param readHandler[in] The callback function for get device information by developer's defined.
 *
 * \return SUCCESS or FAIL.
 *
 * \attention (1) This function must be called before any others interface, and should be called once only.
 */
int tinyAIoT_Initialize2(tinyaiotTransTypeId_e itransType, tinyaiotDevInfoUpdateCallbackFunction_t saveHandler, tinyaiotDevInfoGetCallbackFunction_t readHandler);

/**
 * \brief TinyAIoT module deinitialization.
 *
 * \details A device client want to exit TinyAIoT Service, please call it.
 *
 * \return SUCCESS or FAIL.
 *
 * \attention (1) This function will destory any thread create by TinyAIoT Service, and release all resources.
 */
int tinyAIoT_DeInitialize(void);

/**
 * \brief System parameters set.
 *
 * \details This function set client parameters.
 *
 * \param id[in] parameters id, one of tinyaiotParamId_e.
 * \param val[in] data value for parameter id.
 *
 * \return SUCCESS or ErrorCode.
 */
int tinyAIoT_SystemSet(tinyaiotParamId_e id, void *val);

/**
 * \brief get a free uuid from TinyAIoT Service.
 *
 * \details A client want to get new free device uuid by TinyAIoT Service, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ERRROR_CODE.
 *
 * \attention (1) This function can be called anywhere, but the TinyAIoT Service will return the same uuid whenever.
 */
int tinyAIoT_GetFreeUuid(char *authcode, char uuid[]);

/**
 * \brief drop a assigned and unavailable uuid from TinyAIoT Service.
 *
 * \details A client want to drop a unavailable device uuid by TinyAIoT Service, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ERRROR_CODE.
 *
 * \attention (1) This function can be called after you get a free uuid success, the you can get a new free uuid by tinyAIoT_GetFreeUuid();
 *            (2) Please make sure your uuid really unavailable before you call it.
 */
 int tinyAIoT_DropUuid(char *authcode, char* uuid);

/**
 * \brief new TinyAIoT device register on TinyAIoT Service.
 *
 * \details A client want to register new device to TinyAIoT Service, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ERRROR_CODE.
 *
 * \attention (1) This function must be called when connect to TinyAIoT Service first, and should be called once only.
 *            (2) This function shoult not be called after api tinyAIoT_DevUpdate() or tinyAIoT_DevActivation().
 */
int tinyAIoT_DevRegister(char *authcode, tinyaiotDevInfo_t *device, unsigned char isNotify);
/**
 * \brief new TinyAIoT device information save into TinyAIoT SDK.
 *
 * \details A client want to save device information from TinyAIoT Service, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ERRROR_CODE.
 *
 * \attention (1) This function must be called when connect to TinyAIoT Service first, and should be called once only;
 *            (2) This function shoult not be called after api tinyAIoT_DevRegister() or tinyAIoT_DevActivation().
 */
int tinyAIoT_DevUpdate(char *authcode, tinyaiotDevInfo_t *device);
/**
 * \brief new TinyAIoT device information save into TinyAIoT SDK.
 *
 * \details A client want to save device information from TinyAIoT Service, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ERRROR_CODE.
 *
 * \attention (1) This function must be called when connect to TinyAIoT Service first, and should be called once only;
 *            (2) This function shoult not be called after api tinyAIoT_DevRegister() or tinyAIoT_DevUpdate().
 */
int tinyAIoT_DevActivation(void);

/**
 * \brief get setting information from TinyAIoT Service.
 *
 * \details A client want to get current device setting information by TinyAIoT Service, please call it.
 *
 * \param version[out] Optional, the version value for this setting information.
 *
 * \return setting buffer or NULL.
 *
 * \attention (1) This function return buffer will be freed by tinyAIoT_SettingFree or tinyAIoT_SettingGet.
 */
char* tinyAIoT_SettingGet(int *version);
/**
 * \brief free TinyAIoT device setting information buffer.
 *
 * \details A client want to free setting information buffer after each use, please call it.
 *
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 *
 * \attention (1) This function should be call after tinyAIoT_SettingGet.
 */
int tinyAIoT_SettingFree(void);
/**
 * \brief set setting information version TinyAIoT Service.
 *
 * \details A client want to update setting information version, please call it.
 *
 * \param version[in] the version value to set.
 * \param originalVersion[in] Optional, the original version.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_SettingVersionSet(int version, int originalVersion);
/**
 * \brief get setting information version from TinyAIoT Service.
 *
 * \details A client want to get setting information version befare each use, please call it.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_SettingVersionGet(void);

/**
 * \brief report status information to TinyAIoT Service.
 *
 * \details A client want to report status information, please call it.
 *
 * \param status[in] device status, Reference to tinyaiotDevStatus_t.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_StatusReport(tinyaiotDevStatus_t *status);

/**
 * \brief Send Message
 *
 * \details This function send assign message to session by TinyAIoT Service.
 *
 * \param msgInfo[in] message info to push, Reference to struct _tinyaiotFileInfo_t.
 *
 * \return TINYAIOT_SUCCEED or FAIL.
 */
int tinyAIoT_MsgNotify(tinyaiotMessage_t* msgInfo);

/**
 * \brief Upload data.
 *
 * \details This function upload data to remote storage.
 *
 * \param name[in] file handle name on remote storage.
 * \param type[in] file type, one of enum _tinyaiotFileTypeId_e:
 * \param info[in] buffer info to upload, Reference to struct _tinyaiotFileInfo_t.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_Upload(char *name, tinyaiotFileTypeId_e type, tinyaiotFileInfo_t *info);
/**
 * \brief Upload File
 *
 * \details This function upload file to remote storage.
 *
 * \param name[in] file handle name on remote storage.
 * \param path[in] filename to upload.
 * \param type[in] file type, one of enum _tinyaiotFileTypeId_e:
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_UploadFile(char *name, char *path, int type);
/**
 * \brief Upload File Path Get.
 *
 * \details This function is get the recent file path to upload.
 *
 * \param name[in] file handle NOT support now.
 *
 * \return filepath or NULL.
 *
 * \attention (1) This function not be supported for multithreading environment.
 */
char* tinyAIoT_UploadFilePathGet(char *name);

/**
 * \brief Upload File information.
 *
 * \details This function upload file information to TinyAIoT Service.
 *
 * \param info[in] Reference to tinyaiotFileAttrInfo_t.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_UploadFileAttribute(tinyaiotFileAttrInfo_t *info);

/**
 * \brief Download File
 *
 * \details This function download file from designated uri.
 *
 * \param fileuri[in] designated uri.
 * \param filename[in] filename to save.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_DownloadFile(char* fileuri, char* filename);

/**
 * \brief Get File status
 *
 * \details This function get upload or download file status.
 *
 * \param name[in] file name for upload[name] or download[fileUrl].
 * \param status[out] file current upload or download status.
 *
 * \return TINYAIOT_SUCCEED or ErrorCode.
 */
int tinyAIoT_GetFileStatus(const char* name, tinyaiotFileStatus_t* status);

/**
 * \brief Get the version of TinyAIoT device module - Deprecated
 *
 * \details This function returns the version of TinyAIoT module.
 *
 * \return The version of TinyAIoT module from high byte to low byte, for example,
 *			0x01020304 means the version is 1.2.3.4
 *
 * \attention (1) These functions not be maintained from 1.2.0, and will be removed from 1.3.0.
 *
 * \see tinyAIoT_VersionNumberGet, tinyAIoT_VersionStringGet, tinyAIoT_VersionStringFullGet
 */
unsigned int tinyAIoT_GetApiVer(void);
unsigned int tinyAIoT_GetItransApiVer(void);

#ifdef __cplusplus
}
#endif


#endif
