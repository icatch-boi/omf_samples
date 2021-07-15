#ifndef __INIFILE_H__
#define __INIFILE_H__

unsigned int iniGet(const char* section, const char* key, const char *defaultStr, char *strBuf, unsigned int strBufSize, const char *fileName);
unsigned int iniSet(const char* section, const char* key, const char* Data, const char *fileName);


#endif /* __CONFIG_H__ */
