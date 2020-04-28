#pragma once
#ifndef __STREAM_OMF_API__H__
#define __STREAM_OMF_API__H__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

/** get omf version
 * @return version string
 **/
EXTERNC const char*omfVersion();

/*********************************************************************
 *init/uninit
 *******************************************************************/
/** init omf system, called firstly
 * @param para
 * @return 1(true)/0(false)
 **/
EXTERNC int omfInit(const char* para);

/** uninit omf system, called lastly
 * @param para
 * @return 1(true)/0(false)
 **/
EXTERNC int omfUninit(const char* para);

/*********************************************************************
 *crearte/destory object
 *******************************************************************/
/** create a object
 * @param serialize
 * 				object's serialize param.
 * 				note: must contain "type"
 *   			ex: "type=xxx,name=xxx,cfgs=xxx,serialize={FakeSource+FinkSInk}"...
 * @return the object pointer,0 is fail
 **/
EXTERNC void* omfCreate(const char *serialize);

/** create a appliction
 * @param cfgs appliction's serialize param,
 * 			ex: pv:h264-0,br=1000000+aac-1,br=128000
 * @return the appliction pointer,0 is fail
 **/
EXTERNC void* omfCreateApp(const char *serialize);

/** create a appliction
 * @param cfgs appliction's serialize param,
 * 			ex: pv:h264-0,br=1000000+aac-1,br=128000
 * @return the appliction pointer,0 is fail
 **/
EXTERNC void* omfCreateAttrSet(const char *serialize);

/** destroy a object
 * @param obj the destroyed object pointer
 * @return 1(true)/0(false)
 **/
EXTERNC int  omfDestroy(void* obj);

/**  send message to the obj
 * @param obj the object pointer
 * @param msg the message string
 * @return 1(true)/0(false)
 **/
EXTERNC int omfSendMessage(void* obj,const char* msg);

typedef int(*omfRegisterMessageProcess_cb)(const char* msg);
/** register message process callback to object
 * @param obj the object pointer
 * @param cb the message process callback
 * @return 1(true)/0(false)
 */
EXTERNC int omfRegisterMessageProcess(void* obj,omfRegisterMessageProcess_cb cb);
/*********************************************************************
 *status
 *******************************************************************/
/**  change up the obj's status
 * @param obj the object pointer
 * @param sts the target status,
 * 			include:null,ready,play,pause
 * @return 1(true)/0(false)
 **/
EXTERNC int omfStatusUp(void* obj,const char* sts);

/**  change down the obj's status
 * @param obj the object pointer
 * @param sts the target status,
 * 			include:null,ready,play,pause
 * @return 1(true)/0(false)
 **/
EXTERNC int omfStatusDown(void* obj,const char* sts);

/**  change up/down the obj's status
 * @param obj the object pointer
 * @param sts the target status,
 * 			include:null,ready,play,pause
 * @return 1(true)/0(false)
 **/
EXTERNC int omfStatusChange(void* obj,const char* sts);

/**  get obj's status
 * @param obj the object pointer
 * @return the status pointer
 **/
EXTERNC const char* omfStatusGet(void* obj);

/**  attribute from string
 * @param obj the object pointer
 * @param vs the attribute string
 * @return 1(true)/0(false)
 **/
EXTERNC int	omfAttrFrom(void* obj,const char*vs);

/**  attribute to string
 * @param obj the object pointer
 * @param buff the attribute string buffer
 * @param max the attribute buffer max size
 * @return 1(true)/0(false)
 **/
EXTERNC int	omfAttrTo(void* obj,char* buff,int max);

/*********************************************************************
 *set
 *******************************************************************/
/**  set int attribute to the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @param v the attribute value
 * @return 1(true)/0(false)
 **/
EXTERNC int omfAttrSetInt(void* obj,const char* key,long long v);

/**  set bool attribute to the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :is_sink,is_source...
 * @param v the attribute value:
 * 		1: true
 * 		0: false
 * @return 1(true)/0(false)
 **/
EXTERNC int omfAttrSetBool(void* obj,const char* key,int v);

/**  set string attribute to the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @param v the attribute value
 * @return 1(true)/0(false)
 **/
EXTERNC int omfAttrSetStr(void* obj,const char* key,const char* v);

/**  set pointer attribute to the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @param v the attribute value
 * @return 1(true)/0(false)
 **/
EXTERNC int omfAttrSetPtr(void* obj,const char* key,void* v);

/*********************************************************************
 *Get
 *******************************************************************/
/**  get int attribute form the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @return attribute value
 **/
EXTERNC long long 	omfAttrGetInt(void* obj,const char* key);

/**  get bool attribute form the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :is_sink,is_source...
 * @return attribute value:
 * 			1: true
 * 			0: false
 **/
EXTERNC int	omfAttrGetBool(void* obj,const char* key);

/**  get string attribute form the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @return attribute value
 **/
EXTERNC const char* omfAttrGetStr(void* obj,const char* key);

/**  get pointer attribute form the obj
 * @param obj the object pointer
 * @param key the attribute keys,
 * 			ex :br,fr,url...
 * @return attribute value
 **/
EXTERNC void* 		omfAttrGetPtr(void* obj,const char* key);

/*********************************************************************
 *Register
 *******************************************************************/

typedef void* 		(*omfRegisterIO_open)(const char*path,const char* direct);
typedef void 		(*omfRegisterIO_close)(void*hd);
typedef int 		(*omfRegisterIO_write)(void*hd,void*data,int size);
typedef int 		(*omfRegisterIO_read)(void*hd,void*data,int size);
typedef int 		(*omfRegisterIO_seek)(void*hd,long long offset);
typedef int 		(*omfRegisterIO_flush)(void*hd);
typedef long long 	(*omfRegisterIO_getoffset)(void*hd);
typedef long long 	(*omfRegisterIO_getsize)(void*hd);
typedef int 		(*omfRegisterIO_remove)(const char*url);

/**  register a group callback to a new IO
 * @param name the new IO's type
 * @param open
 * @param close
 * @param write
 * @param read
 * @param seek
 * @param flush
 * @param getoffset
 * @param getsize
 * @param remove
 * @return 1(true)/0(false)
 **/
EXTERNC int omfRegisterIO(const char*name
		,omfRegisterIO_open open
		,omfRegisterIO_close close
		,omfRegisterIO_write write
		,omfRegisterIO_read read
		,omfRegisterIO_seek seek
		,omfRegisterIO_flush flush
		,omfRegisterIO_getoffset getoffset
		,omfRegisterIO_getsize getsize
		,omfRegisterIO_remove remove
);


typedef void	(*omfRegisterSink_free)(void*free_data);
typedef void* 	(*omfRegisterSink_open)(const char*media,void*priv_data);
typedef void 	(*omfRegisterSink_close)(void*hd);
typedef int 	(*omfRegisterSink_sendpkt)(void*hd,void*frame,int size
		,long long pts_ms,unsigned flags,int idx,omfRegisterSink_free free,void* free_data);

/**  register a group callback to a new sink element
 * @param name the new sink element's type
 * @param open
 * @param close
 * @param sendpkt
 * @return 1(true)/0(false)
 **/
EXTERNC int omfRegisterSink(const char*name
		,omfRegisterSink_open open
		,omfRegisterSink_close close
		,omfRegisterSink_sendpkt sendpkt
);

typedef void 	(*omfRegisterSink_msg)(void*hd,const char*msg);
EXTERNC int omfRegisterSink4(const char*name
		,omfRegisterSink_open open
		,omfRegisterSink_close close
		,omfRegisterSink_sendpkt sendpkt
		,omfRegisterSink_msg msg
);

typedef void	(*omfRegisterSource_free)(void*free_data);
typedef void* 	(*omfRegisterSource_open)(char* media,void* priv_data);
typedef void 	(*omfRegisterSource_close)(void*hd);
typedef int 	(*omfRegisterSource_getpkt)(void*hd,void**frame,long long* pts_ms,unsigned* flags,int* idx,omfRegisterSource_free*free,void** free_data);

/**  register a group callback to a new source element
 * @param name the new source element's type
 * @param open
 * @param close
 * @param getpkt
 * @param fillpkt
 * @return 1(true)/0(false)
 **/
EXTERNC int omfRegisterSource(const char*name
		,omfRegisterSource_open open
		,omfRegisterSource_close close
		,omfRegisterSource_getpkt getpkt
		,void* reserve
);

typedef void 	(*omfRegisterSource_msg)(void*hd,const char*msg);
EXTERNC int omfRegisterSource4(const char*name
		,omfRegisterSource_open open
		,omfRegisterSource_close close
		,omfRegisterSource_getpkt getpkt
		,omfRegisterSource_msg msg
);

typedef struct omf_frame {
	int index;
	void*data;
	int size;
	int iskeyframe;
	long long pts_ms;
	void*free_data;
	void(*free)(void*free_data);
	void*pkt;
}omf_frame;

typedef void* 	(*omfRegisterFilter_open3)(char* media,void* priv_data,void*obj);
typedef void 	(*omfRegisterFilter_close)(void*hd);
typedef int 	(*omfRegisterFilter_proc)(void*hd,omf_frame*frm);
typedef int 	(*omfRegisterFilter_msg)(void*hd,const char*msg);
/**  register a group callback to a new filter element
 * @param name the new source element's type
 * @param open
 * @param close
 * @param proc
 * @param msg
 * @return 1(true)/0(false)
 **/
EXTERNC int omfRegisterFilter(const char*name
		,omfRegisterFilter_open3 open
		,omfRegisterFilter_close close
		,omfRegisterFilter_proc proc
		,omfRegisterFilter_msg msg
);

/**  register a pipeline with the configure string
 * @param name the pipeline name
 * @param cfgs the configure string, ex: RingSoure~H264Encoder~RingSink
 * @return 1(true)/0(false)
 **/
EXTERNC int omfRegisterPipeline(const char*name,const char* serialize);

/**
 * register a callback to omf system.
 * @param name the command name
 * @param func the callback
 * @return 1(true)/0(false)
 */
EXTERNC int omfRegisterCommand(const char*name,int(*func)(const char*input,char*output));

/**  call the function registered in factory.
 * @param cmd  	the cmd id.
 * @param input the cmd paras string, set 0 if no para input.
 * @param output  output the result string, set 0 if no request output.
 * @return 1(true)/0(false)
 * */
EXTERNC int omfCommand(const char* cmd,const char*input,char*output);

/**  call the function registered in factory.
 * @param cmds[in]: the cmd sequence.\n
 * 		eg.. cmd0=param0,cmd1=param1,cmd2=param2... *
 */
EXTERNC void omfCommands(const char* cmds);

/** called when abnormal exit of the process.\n
 * Usually write the first line of main(),as follow:\n
 *    int mian(){ \n
 * 		signal(SIGINT, omfExit); \n
 * 		... \n
 * 		return 0; \n
 * 	  }
 * @param signo
 * */
EXTERNC void omfExit(int signo);

#endif /**STREAM_OMF_API_H_H*/
