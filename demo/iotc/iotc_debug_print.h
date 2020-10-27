#ifndef __IOT_DEBUG_PRINT_H__
#define __IOT_DEBUG_PRINT_H__

/* level setting */
#define IOT_LVL_ERROR                 0 /*min print info*/
#define IOT_LVL_WARN                  1
#define IOT_LVL_INFO                  2
#define IOT_LVL_DEBUG                 3 /*max print info*/
		
/* color setting */
#define DEBUG_COLOR_NORMAL          "\033[m"
#define DEBUG_COLOR_BLACK           "\033[30m"
#define DEBUG_COLOR_RED             "\033[31m"
#define DEBUG_COLOR_GREEN           "\033[32m"
#define DEBUG_COLOR_YELLOW          "\033[33m"
#define DEBUG_COLOR_BLUE            "\033[34m"
#define DEBUG_COLOR_PURPLE          "\033[35m"
#define DEBUG_COLOR_BKRED           "\033[41;37m"

#ifndef IOT_PRINT_LOG_EN
#define IOT_PRINT_LOG_EN 1
#endif

#ifndef IOT_DEBUG_DEF_LEVEL
#define IOT_DEBUG_DEF_LEVEL		IOT_LVL_DEBUG
#endif

#if IOT_PRINT_LOG_EN
	#define dbgLogPrint(level, format, args...)   \
		do { \
			if(level <= IOT_DEBUG_DEF_LEVEL ) { \
				if(level == IOT_LVL_ERROR) { \
	                printf(DEBUG_COLOR_BKRED format DEBUG_COLOR_NORMAL "\n", ##args); \
	            }\
	            else if(level == IOT_LVL_WARN) {    \
	               printf(DEBUG_COLOR_GREEN format DEBUG_COLOR_NORMAL "\n", ##args); \
				}\
	            else if(level == IOT_LVL_INFO) {  \
	                printf(DEBUG_COLOR_YELLOW format DEBUG_COLOR_NORMAL "\n", ##args);   \
				}\
	            else {\
	                printf(DEBUG_COLOR_PURPLE format DEBUG_COLOR_NORMAL "\n", ##args);\
				}\
			}\
		}while(0)
#else
	#define dbgLogPrint
#endif




#endif
