//
// Created by yb.leng on 2020/3/9.
//

#pragma once

typedef enum{
	WIFI_AP_MODE         = 3,
	WIFI_STA_MODE        = 2,
	WIFI_SMARTLINK_MODE  = 1,
	WIFI_NOT_INIT        = 0,
	WIFI_ERROR           = -1,
	WIFI_STA_CFG_ERROR   = -2,
}ENUM_WIFI_STAT;