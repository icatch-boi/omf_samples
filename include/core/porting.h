#ifndef _PORTING_H_
#define _PORTING_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
///
EXTERNC unsigned int thread_priority_set(void *handle, unsigned int pri);
EXTERNC unsigned int thread_name_set(void *handle, const char *name);
///
EXTERNC unsigned enter_critical();
EXTERNC unsigned exit_critical();
EXTERNC void vfp_enable(void);
EXTERNC void vfp_disable(void);
EXTERNC void dsp_enable(void);
EXTERNC void dsp_disable(void);
EXTERNC void cmdExec(char *cmd);
///
EXTERNC int fileConcate(void* srcFd,void* dstFd, int opt);
EXTERNC unsigned int GetClusterSize(char *diskname);
///
EXTERNC unsigned int pfLogInit(unsigned int id, unsigned int size);
EXTERNC unsigned int pfLogModeSet(unsigned int id, unsigned int mode);
EXTERNC unsigned int pfLogPrintf(unsigned int id, const char *format, ...);
EXTERNC unsigned int pfLogMemdump(unsigned int id, const void *addr, unsigned int size);
EXTERNC unsigned int pfLogTerm(unsigned int id);
EXTERNC unsigned int pfLogCallStack(unsigned int id);
///
EXTERNC void dbgAssert(unsigned int expr);
EXTERNC void callStack(void);
EXTERNC void osDeadMediaDumpSet(void (*fp)(void));
///
EXTERNC void getLocalTime(char*);
///
EXTERNC void cxxPoolsInfo();

#endif  /* _PORTING_H_ */