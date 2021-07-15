#ifndef __TINYAIOT_ERROR_CODE_H__
#define __TINYAIOT_ERROR_CODE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _tinyaiotErrorCode_e {
	TINYAIOT_SUCCEEDED        = 0,

	/* For global module APIs */
	TINYAIOT_UNKNOWN_ERROR       = 1,  /**< Unknown error */
	TINYAIOT_ERROR_CHECK_PRECOND = 2,  /**< Precondition not met */
	TINYAIOT_ERROR_INVALID_PARAM = 3,  /**< Invalid handle or possible bad pointer was passed to a function */
	TINYAIOT_ERROR_NO_MEMORY     = 4,  /**< Buffer too small or a failure while in memory allocation */
	TINYAIOT_ERROR_REPEATED      = 5,  /**< api call repeat */
	TINYAIOT_ERROR_NONEXISTENT   = 6,  /**< Resource non-existent */
	TINYAIOT_ERROR_EXISTENT      = 7,  /**< Resources already existent */
	TINYAIOT_ERROR_LIMITED       = 8,  /**< Resources limited */
	TINYAIOT_ERROR_INVALID_DATA  = 9,  /**< Invalid data, parse data fail */
	TINYAIOT_ERROR_SIZE_INVALID  = 10, /**< Resources/File size invaild */
	TINYAIOT_ERROR_OPERATION       = 11, /**< Resources/File operation fail */
	TINYAIOT_ERROR_UNSUPPORTED     = 12, /**< Function not supported */
	TINYAIOT_ERROR_TIMEOUT         = 13, /**< Timeout */
	TINYAIOT_ERROR_ENCRYPTION      = 14, /**< Data encryption error. */
	TINYAIOT_ERROR_DECRYPTION      = 15, /**< Data decryption error. */
	TINYAIOT_ERROR_SERVER_RESPONSE = 16, /**< Response error from server, more error code reference to https://www.yuque.com/tinycloud/cloud/xnc3u3 */

	TINYAIOT_ERROR_SERVICES_GET_TYPE     = 21, /**< Service type get error, reference to emnu _servicesTypeId_e. */
	TINYAIOT_ERROR_SERVICES_GET_INFO,          /**< Service information get error, reference to union _serviceInfo_u. */
	TINYAIOT_ERROR_SERVICES_LIVE_INITIALIZE,   /**< Service Initialize error. */

} tinyaiotErrorCode_e;

/**< Response error code from server RESTFul API, more error code reference to https://www.yuque.com/tinycloud/cloud/xnc3u3 */
typedef enum _tinyaiotServiceErrorCode_e {
	/* For Client module APIs, reference to tinyaiot_interface.h */
	/* universal error */
	TINYAIOT_NOT_SUPPORTED		   = 10001,  /**< Function not supported */
	TINYAIOT_CALL_REPEATED		   = 10002,  /**< api call repeat */
	TINYAIOT_MEMORY_FAILED		   = 10003,  /**< Memory operate fail */
	TINYAIOT_TIMEOUT			   = 10004,
	TINYAIOT_DATA_INVALID		   = 10005,  /**< response data invaild */
	TINYAIOT_FILE_TYPE_INVALID	   = 10011,  /**< file type invaild */
	TINYAIOT_FILE_SIZE_INVALID	   = 10012,  /**< file size invaild */
	TINYAIOT_FILE_OPERATION_FAILED = 10013,  /**< file size invaild */
	TINYAIOT_FILE_NONEXISTENT	   = 10014,  /**< file non-existent */
	TINYAIOT_FILE_EXISTENT		   = 10015,  /**< file already existent */
	TINYAIOT_PARAM_MISSING		   = 10041,  /**< parameter missing */
	TINYAIOT_PARAM_INVALID		   = 10042,  /**< parameter invaild */
	TINYAIOT_PARAM_TYPE_INVALID    = 10043,  /**< parameter type invaild */
	TINYAIOT_PERMISSION_DENIED	   = 20031,
	/* tinyaiot device error */
	TINYAIOT_DEV_NONEXISTENT	   = 50002,  /**< Device not existing */
	TINYAIOT_DEV_EXISTENT		   = 50034,  /**< Device already existed */
	TINYAIOT_DEV_PERMISSION_DENIED = 50201,
	TINYAIOT_UUID_NONEXISTENT	   = 50204,  /**< Uuid not existing */
	TINYAIOT_UUID_EXISTENT		   = 50234,  /**< Device already existed */
	TINYAIOT_THING_NONEXISTENT	   = 50302,  /**< THING not existing */
	TINYAIOT_THING_EXISTENT 	   = 50334,  /**< THING already existed */
	TINYAIOT_THING_EXTENSION_NONEXISTENT = 50341,  /**< extensions THING not existing */
	TINYAIOT_DEV_RESP_FAILED	   = 50000,  /**< return fail by device server. */

	TINYAIOT_FAILED  = -1,
} tinyaiotServiceErrorCode_e;

#ifdef __cplusplus
}
#endif


#endif
