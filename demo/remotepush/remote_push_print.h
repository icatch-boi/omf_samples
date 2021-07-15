#ifndef __REMOTE_PUSH_PRINT_H__
#define __REMOTE_PUSH_PRINT_H__

/* level setting */
#define REMOTE_PUSH_LVL_ERROR                 0 /*min print info*/
#define REMOTE_PUSH_LVL_WARN                  1
#define REMOTE_PUSH_LVL_INFO                  2
#define REMOTE_PUSH_LVL_DEBUG                 3 /*max print info*/
		
/* color setting */
#define DEBUG_COLOR_NORMAL          "\033[m"
#define DEBUG_COLOR_BLACK           "\033[30m"
#define DEBUG_COLOR_RED             "\033[31m"
#define DEBUG_COLOR_GREEN           "\033[32m"
#define DEBUG_COLOR_YELLOW          "\033[33m"
#define DEBUG_COLOR_BLUE            "\033[34m"
#define DEBUG_COLOR_PURPLE          "\033[35m"
#define DEBUG_COLOR_BKRED           "\033[41;37m"

#ifndef REMOTE_PUSH_PRINT_EN
#define REMOTE_PUSH_PRINT_EN 1
#endif

#ifndef REMOTE_PUSH_DBG_DEF_LEVEL
#define REMOTE_PUSH_DBG_DEF_LEVEL		REMOTE_PUSH_LVL_DEBUG
#endif

#if REMOTE_PUSH_PRINT_EN
	#define dbgLogPrint(level, format, args...)   \
		do { \
			if(level <= REMOTE_PUSH_DBG_DEF_LEVEL ) { \
				if(level == REMOTE_PUSH_LVL_ERROR) { \
	                printf(DEBUG_COLOR_BKRED format DEBUG_COLOR_NORMAL "\n", ##args); \
	            }\
	            else if(level == REMOTE_PUSH_LVL_WARN) {    \
	               printf(DEBUG_COLOR_GREEN format DEBUG_COLOR_NORMAL "\n", ##args); \
				}\
	            else if(level == REMOTE_PUSH_LVL_INFO) {  \
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