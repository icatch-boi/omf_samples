//
// Created by yb.leng on 2020/3/8.
//

#pragma once

/*                                 MACROS                                       */
/********************************************************************************/
#define DATA_INTERACTION_OPTION_SYS_REBOOT							"appSysReboot"
#define DATA_INTERACTION_OPTION_SYS_FIRST_BOOT						"appSysFirstBoot"
#define DATA_INTERACTION_OPTION_GPIO_STATUS_PIR						"appGpioStatusPir"
#define DATA_INTERACTION_OPTION_POWER_ON_SRC						"appPwrOnSrc"
#define DATA_INTERACTION_OPTION_SYS_PARAMS							"appSysParams"
#define DATA_INTERACTION_OPTION_GPIO_INIT_PIR						"appGpioInitPir"
#define DATA_INTERACTION_OPTION_WIFI_MODULE_RESET_FLAG				"appWifiModuleRstFlag"
#define DATA_INTERACTION_OPTION_SD_MOUNTED							"appSDMounted"
#define DATA_INTERACTION_OPTION_SETUP_STATUS						"appSetupSts"
#define DATA_INTERACTION_OPTION_PWR_ON_SOUND						"appPwrOnSoundTrigger"
#define DATA_INTERACTION_OPTION_GPIO_STATUS_PWR_ON_SRC				"appGpioStsPwrOnSrc"
#define DATA_INTERACTION_OPTION_GPIO_STATUS_RING					"appGpioStatusRing"
#define DATA_INTERACTION_OPTION_GPIO_STATUS_SYNC					"appGpioStatusSync"
#define DATA_INTERACTION_OPTION_GPIO_PWR_CTRL						"appPwrCtrl"
#define DATA_INTERACTION_OPTION_SYNC_KEY_TRIGGER_SETUP				"appSyncKeyTriggerSetup"
#define DATA_INTERACTION_OPTION_PUSH_MSG_VIA_WIFI_MODULE			"appPushMsgViaWifiModule"
#define DATA_INTERACTION_OPTION_WIFI_STATUS							"appWifiSts"
#define DATA_INTERACTION_OPTION_SYS_PWROFF_COUNT					"appPwrOffCount"
#define DATA_INTERACTION_OPTION_VOICE_PROMPT_PLAY					"appPlayVoicePrompt"
#define DATA_INTERACTION_OPTION_TRANSMIT_UUID_TO_WIFI_MODULE		"appSendUuid2WifiModule"
#define DATA_INTERACTION_OPTION_WIFI_MODULE_PUSH_MSG_TASK_INIT		"appMsgTaskInit"
#define DATA_INTERACTION_OPTION_WIFI_MODULE_PUSH_MSG_ENABLE			"appMsgPushEnable"
#define DATA_INTERACTION_OPTION_AI_FEATURE_STATUS					"appAIFeatureSts"
#define DATA_INTERACTION_OPTION_PIR_ENABLE_STATUS					"appPirEnSts"
#define DATA_INTERACTION_OPTION_PIR_RECORD_ENABLE_STATUS			"appPirRecEnSts"
#define DATA_INTERACTION_OPTION_PIR_MSG_ENABLE_STATUS				"appPirMsgEnSts"
#define DATA_INTERACTION_OPTION_PIR_SENSITIVITY_VALUE				"appPirSensiVal"
#define DATA_INTERACTION_OPTION_PIR_SENSITIVITY_SPEC				"appPirSensiSpec"
#define DATA_INTERACTION_OPTION_SD_TOTAL_SIZE						"appSDTotalSz"
#define DATA_INTERACTION_OPTION_SD_CARD_INFO						"appSDInfo"
#define DATA_INTERACTION_OPTION_SD_PLUG_IN_OUT						"appSDPlugInOut"
#define DATA_INTERACTION_OPTION_DISK_FORMAT							"appDiskFormat"
#define DATA_INTERACTION_OPTION_DEVICE_VERSION						"appDevVer"
#define DATA_INTERACTION_OPTION_OTA_CFG								"appOTACfg"
#define DATA_INTERACTION_OPTION_REBOOT_DEVICES						"appRebootDev"
#define DATA_INTERACTION_OPTION_RTC_RESET							"appRtcRst"
#define DATA_INTERACTION_OPTION_WIFI_MODULE_VERSION					"appWifiModuleVer"
#define DATA_INTERACTION_OPTION_FACTORY_SETTING						"appFactorySetting"
#define DATA_INTERACTION_OPTION_TINYAIOT_SERVER_ADDRESS				"appTinyaiotServAddr"
#define DATA_INTERACTION_OPTION_MSG_CENTER_STATUS					"appMsgCenterSts"
#define DATA_INTERACTION_OPTION_MSG_CENTER_SWITCH					"appMsgCenterSw"
#define DATA_INTERACTION_OPTION_MSG_CENTER_PARAMS					"appMsgCenterParams"
#define DATA_INTERACTION_OPTION_DEVINFO_SAVE_CB						"appDevInfoSaveCB"
#define DATA_INTERACTION_OPTION_DEVINFO_READ_CB						"appDevInfoReadCB"
#define DATA_INTERACTION_OPTION_AE_STATBLE_STATUS					"appAEStableSts"
#define DATA_INTERACTION_OPTION_USB_PLUG_IN_OUT						"appUSBInOut"
#define DATA_INTERACTION_OPTION_BATTERY_CHARGING_STATUS				"appBattChargingSts"
#define DATA_INTERACTION_OPTION_BATTERY_CURRENT_LVL					"appBattCurrLvl"
#define DATA_INTERACTION_OPTION_DEVICE_NAME							"appDevName"
#define DATA_INTERACTION_OPTION_TIME_ZONE							"appTimeZone"
#define DATA_INTERACTION_OPTION_THUMBNAIL							"appThumbnail"
#define DATA_INTERACTION_OPTION_SETUP_FLAG							"appSetupFlag"
#define DATA_INTERACTION_OPTION_WIFI_PARAM							"appWifiParams"
#define DATA_INTERACTION_OPTION_SYS_UID								"appSysUID"
#define DATA_INTERACTION_OPTION_CLIENT_NUMBER						"appClientNum"
#define DATA_INTERACTION_OPTION_LOGIN_RESULT						"appLoginResult"
#define DATA_INTERACTION_OPTION_LOGIN_STATUS						"appLoginIsOver"
#define DATA_INTERACTION_OPTION_WAKEUP_PATTERN_STATUS				"appWakeupPtnIsOver"
#define DATA_INTERACTION_OPTION_MSG_PUSH_STATUS						"appMsgPushIsOver"
#define DATA_INTERACTION_OPTION_FILE_PUSH_STATUS					"appFilePushIsOver"
#define DATA_INTERACTION_OPTION_LINUX_BOOT_START					"appLinuxBootStart"
#define DATA_INTERACTION_OPTION_WAKEUP_PATTERN						"appWakeupPtn"
#define DATA_INTERACTION_OPTION_PASS_ONE_DAY						"appPassOneDay"
#define DATA_INTERACTION_OPTION_CRYPTO_CIPHER_INFO					"appCryptoCipHerInfo"
#define DATA_INTERACTION_OPTION_SDIO_SWITCH_LINUX					"appSdioSwLinux"
#define DATA_INTERACTION_OPTION_SDIO_SWITCH_RTOS					"appSdioSwRtos"
#define DATA_INTERACTION_OPTION_SDIO_XMIT_RECEIVE					"appSdioXmitRecv"
#define DATA_INTERACTION_OPTION_SDIO_TRANSMIT_METHOD				"appSdioTransmitMethod"
#define DATA_INTERACTION_OPTION_PACKAGE_STATUS						"appPackageStatus"
#define DATA_INTERACTION_OPTION_LINUX_THREAD_STATUS					"appLinuxThreadStatus"
/********************************************************************************/
/*                                 TYPES                                        */
/********************************************************************************/

typedef enum{
	DATA_INTERACTION_OPERATION_SET = 0,
	DATA_INTERACTION_OPERATION_GET = 1,
	DATA_INTERACTION_OPERATION_CMD = 2,
}DataInteractionOperation_e;

