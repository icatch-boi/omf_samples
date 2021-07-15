#pragma once
#ifndef __STREAM_OMF_CMD__H__
#define __STREAM_OMF_CMD__H__

#ifdef EXTERNC
#else
	#ifdef __cplusplus
		#define EXTERNC extern "C"
	#else
		#define EXTERNC
	#endif
#endif

/*Activate plugin*/
typedef void (*CbRegister)();
#define ACTIVATE(m) extern CbRegister __cb_register_##m;sum|=(unsigned)(void*)__cb_register_##m;
#define ACTIVATE_PLUGIN(m) ACTIVATE(Plugin##m)
#define ACTIVATE_APP(m) ACTIVATE(App##m)

/*Activate command*/
#define ACTIVATE_CMD(f) ACTIVATE(f)//extern void* __register_cmd_##f;sum|=(unsigned)(void*)__register_cmd_##f;

#endif

