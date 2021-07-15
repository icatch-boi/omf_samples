#ifndef __TINYAIOT_DEBUG_H__
#define __TINYAIOT_DEBUG_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
/*********************************************************************
		S t r u c t u r e D e f i n e
**********************************************************************/
typedef enum _tinyaiot_dbg_prority_e {
   TINYAIOT_LOG_EMERG = 0,
   TINYAIOT_LOG_ALERT,
   TINYAIOT_LOG_CRIT,
   TINYAIOT_LOG_ERR,
   TINYAIOT_LOG_WARNING,
   TINYAIOT_LOG_NOTICE = 5,
   TINYAIOT_LOG_INFO,
   TINYAIOT_LOG_DEBUG,
} tinyaiot_dbg_prority_e;

/*********************************************************************
 		F u n c t i o n s					D e f i n e
**********************************************************************/
/**
 * \brief System debug log priority set.
 *
 * \details This function set system debug level.
 * set the console level, messages will be print on console when
 * the level less than console barrier
 *
 * \param id[in] parameters id, one of tinyaiotParamId_e.
 * \param val[in] data value for parameter id.
 *
 * \return SUCCESS or ErrorCode.
 */
int tinyAIoT_DbgPrioritySet(tinyaiot_dbg_prority_e level);
/*
* get the console priority, messages will be print on console when
* the level less than console barrier
*/
int tinyAIoT_DbgPriorityGet(void);
/**
 * \brief Get the version of TinyAIoT Debug module
 *
 * \details This function returns the version of debug module.
 *
 * \return The version of dmgr module from high byte to low byte, for example,
 *			0x01020304 means the version is 1.2.3.4
 *
 * \attention (1) These functions not be maintained from 1.2.0, and will be removed from 1.3.0.
 *
 * \see tinyAIoT_VersionNumberGet, tinyAIoT_VersionStringGet, tinyAIoT_VersionStringFullGet
 */
unsigned int tinyAIoT_GetDbgApiVer(void);

int tinyAIoT_DbgFuncDepthGet(void);
void tinyAIoT_DbgFuncEnter(const int priority, const char *funcName, unsigned int lineNum, const char *strFmt, ...);
void tinyAIoT_DbgFuncReturn(const int priority, const char *funcName, unsigned int lineNum, const char *strFmt, ...);
void tinyAIoT_DbgPrint(const char *module, const int priority, const char *strFmt, ...);
void tinyAIoT_DbgLinePrint(const char *module, const int priority, const char *fileName, unsigned int lineNum, const char *strFmt,...);
void tinyAIoT_DbgMemoryDump(const char *module, const int priority, const char *text, FILE *stream, unsigned char *ptr, size_t size);
int tinyAIoT_DbgTimeGet(void);
void* tinyAIoT_DbgMalloc(const char *module, int size, char *file, int line);
void tinyAIoT_DbgFree(const char *module, void *p, char *file, int line);
int tinyAIoT_DbgAllocNumGet(const char *module, char *special, char *file, int line);
int tinyAIoT_DbgAllocInfoPrint(const char *module);

/*********************************************************************
		M a c r o s 						D e f i n e
**********************************************************************/
#ifndef SYSLOG_MODULE
#define SYSLOG_MODULE "SYSLOG"
#endif

#define TINYAIOT_DBG_MALLOC(sz) tinyAIoT_DbgMalloc(NULL, sz, __FILE__, __LINE__)
#define TINYAIOT_DBG_FREE(p)    tinyAIoT_DbgFree(NULL, p, __FILE__, __LINE__)
#define TINYAIOT_DBG_PRINT_ALLOC_NUM(sp)   tinyAIoT_DbgAllocNumGet(NULL, sp, __FILE__, __LINE__)
#define TINYAIOT_DBG_PRINT_ALLOC_INFO(sp)   tinyAIoT_DbgAllocInfoPrint(NULL)
#define TINYAIOT_DBG_MALLOC_MOD(module, sz) tinyAIoT_DbgMalloc(module, sz, __FILE__, __LINE__)
#define TINYAIOT_DBG_FREE_MOD(module, p)    tinyAIoT_DbgFree(module, p, __FILE__, __LINE__)
#define TINYAIOT_DBG_PRINT_ALLOC_NUM_MOD(module, sp)   tinyAIoT_DbgAllocNumGet(module, sp, __FILE__, __LINE__)
#define TINYAIOT_DBG_PRINT_ALLOC_INFO_MOD(module, sp)   tinyAIoT_DbgAllocInfoPrint(module)

/*
  function tracing.

  macros:
	TINYAIOT_DBG_FUNC_ENTER(),
	TINYAIOT_DBG_FUNC_ENTERn), n=0~7,
	TINYAIOT_DBG_FUNC_RETURN(),
	TINYAIOT_DBG_FUNC_RETURNn(), n=0~7,
	TINYAIOT_DBG_FUNC_RETURN_VOID(),
	TINYAIOT_DBG_FUNC_RETURN_VOIDn(), n=0~7.
*/
#define TINYAIOT_DBG_FUNC_ENTER(priority, strFmt, args...) \
	tinyAIoT_DbgFuncEnter(priority, __func__, __LINE__, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER0(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_EMERG, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER1(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_ALERT, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER2(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_CRIT, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER3(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_ERR, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER4(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_WARNING, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER5(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_NOTICE, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER6(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_INFO, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_ENTER7(strFmt, args...)  TINYAIOT_DBG_FUNC_ENTER(TINYAIOT_LOG_DEBUG, strFmt, ##args)

#define TINYAIOT_DBG_FUNC_RETURN(priority, retVal, strFmt, args...) \
	do { \
		tinyAIoT_DbgFuncReturn(priority, __func__, __LINE__, strFmt, ##args); \
		return (retVal); \
	} while (0)
#define TINYAIOT_DBG_FUNC_RETURN0(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_EMERG, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN1(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_ALERT, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN2(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_CRIT, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN3(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_ERR, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN4(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_WARNING, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN5(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_NOTICE, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN6(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_INFO, retVal, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN7(retVal, strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN(TINYAIOT_LOG_DEBUG, retVal, strFmt, ##args)

#define TINYAIOT_DBG_FUNC_RETURN_VOID(priority, strFmt, args...) \
	do { \
		tinyAIoT_DbgFuncReturn(priority, __func__, __LINE__, strFmt, ##args); \
		return; \
	} while (0)
#define TINYAIOT_DBG_FUNC_RETURN_VOID0(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_EMERG, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID1(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_ALERT, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID2(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_CRIT, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID3(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_ERR, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID4(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_WARNING, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID5(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_NOTICE, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID6(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_INFO, strFmt, ##args)
#define TINYAIOT_DBG_FUNC_RETURN_VOID7(strFmt, args...)  TINYAIOT_DBG_FUNC_RETURN_VOID(TINYAIOT_LOG_DEBUG, strFmt, ##args)

/*
  printing.

  macros:
	TINYAIOT_DBG_TAG(),
	TINYAIOT_DBG_TAG_PAUSE(),
	TINYAIOT_DBG_TAG_TIME(),
	TINYAIOT_DBG_PRINT(),
	TINYAIOT_DBG_PRINTn(), n=0~7,
	TINYAIOT_DBG_PRINT_PAUSE(),
	TINYAIOT_DBG_PRINT_PAUSEn(), n=0~7,
	TINYAIOT_DBG_LINE_PRINT(),
	TINYAIOT_DBG_LINE_PRINTn(), n=0~7,
*/
#if 1 /* __func__ takes too much space for debugging strings */
#define TAG_STR  __func__
#else
#define TAG_STR  __FILE__
#endif
#undef TINYAIOT_DBG_TAG
#define TINYAIOT_DBG_TAG() \
		do { \
			printf("%s[%d]\n", TAG_STR, __LINE__); \
		} while (0)

#undef TINYAIOT_DBG_TAG_PAUSE
#define TINYAIOT_DBG_TAG_PAUSE() \
		do { \
			printf("%s[%d]\n", TAG_STR, __LINE__); \
			getch(); \
		} while (0)

#undef TINYAIOT_DBG_TAG_TIME
#define TINYAIOT_DBG_TAG_TIME() \
		do { \
			printf("%s[%d] %d us\n", TAG_STR, __LINE__, tinyAIoT_DbgTimeGet()); \
		} while (0)


#define TINYAIOT_DBG_LINE_PRINT(priority, strFmt, args...) \
		tinyAIoT_DbgLinePrint(SYSLOG_MODULE, priority, __FILE__, __LINE__, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT0(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_EMERG, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT1(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_ALERT, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT2(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_CRIT, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT3(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_ERR, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT4(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_WARNING, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT5(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_NOTICE, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT6(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_INFO, strFmt, ##args)
#define TINYAIOT_DBG_LINE_PRINT7(strFmt, args...) TINYAIOT_DBG_LINE_PRINT(TINYAIOT_LOG_DEBUG, strFmt, ##args)

#define TINYAIOT_DBG_PRINT(priority, strFmt, args...) \
	tinyAIoT_DbgPrint(SYSLOG_MODULE, priority, strFmt, ##args)
#define TINYAIOT_DBG_PRINT0(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_EMERG, strFmt, ##args)
#define TINYAIOT_DBG_PRINT1(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_ALERT, strFmt, ##args)
#define TINYAIOT_DBG_PRINT2(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_CRIT, strFmt, ##args)
#define TINYAIOT_DBG_PRINT3(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_ERR, strFmt, ##args)
#define TINYAIOT_DBG_PRINT4(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_WARNING, strFmt, ##args)
#define TINYAIOT_DBG_PRINT5(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_NOTICE, strFmt, ##args)
#define TINYAIOT_DBG_PRINT6(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_INFO, strFmt, ##args)
#define TINYAIOT_DBG_PRINT7(strFmt, args...) TINYAIOT_DBG_PRINT(TINYAIOT_LOG_DEBUG, strFmt, ##args)

#define TINYAIOT_DBG_DUMP(priority, text, stream, ptr, size) \
		tinyAIoT_DbgMemoryDump(SYSLOG_MODULE, priority, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP0(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_EMERG, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP1(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_ALERT, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP2(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_CRIT, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP3(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_ERR, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP4(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_WARNING, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP5(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_NOTICE, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP6(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_INFO, text, stream, ptr, size)
#define TINYAIOT_DBG_DUMP7(text, stream, ptr, size) TINYAIOT_DBG_DUMP(TINYAIOT_LOG_DEBUG, text, stream, ptr, size)

#define TINYAIOT_DBG_PRINT_PAUSE(priority, strFmt, args...) \
	do { \
		tinyAIoT_DbgPrint(SYSLOG_MODULE, priority, strFmt, ##args); \
		tinyAIoT_DbgPrint(SYSLOG_MODULE, priority, "  ... press any key to continue ...\n"); \
		getch(); \
	} while (0)
#define TINYAIOT_DBG_PRINT_PAUSE0(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_EMERG, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE1(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_ALERT, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE2(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_CRIT, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE3(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_ERR, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE4(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_WARNING, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE5(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_NOTICE, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE6(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_INFO, strFmt, ##args)
#define TINYAIOT_DBG_PRINT_PAUSE7(strFmt, args...)  TINYAIOT_DBG_PRINT_PAUSE(TINYAIOT_LOG_DEBUG, strFmt, ##args)

#ifdef __cplusplus
}
#endif
#endif /* __TINYAIOT_DEBUG_H__ */
