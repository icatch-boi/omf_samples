//
// Created by yb.leng on 2020/3/8.
//

#pragma once
typedef enum {
	MSG_CENTER_SETTING_PIR = 0,
	MSG_CENTER_SETTING_RING = 1,
	MSG_CENTER_SETTING_MAX,
}msg_center_sel_e;

typedef struct msgCenterIOParam_s {
	char fileName[32];
	char isoTime[32];
	int fileType;
}msgCenterIOParam_t;

typedef struct msgCenterParam_s {
	msgCenterIOParam_t ioParams;
}msgCenterParam_t;