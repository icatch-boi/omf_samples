/**************************************************************************
 *                       			  INCLUDE              	              *
 **************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "ipc_shm.h"


/**************************************************************************
 *                       			   MACROS              	              *
 **************************************************************************/ 
#define IPC_SHM_SIZE		128
#define IPC_SHM_KEY			1880222
/**************************************************************************
 *                          	TYPE DEFINITION   	                       *
 **************************************************************************/ 



/**************************************************************************
 *                       		   VARIABLES              	              *
 **************************************************************************/



/**************************************************************************
 *                           FUNCTION DECLARATION                         *
 **************************************************************************/

/**************************************************************************
 *                           FUNCTION DEFINITION                          *
 **************************************************************************/

int main(int argc, char **argv)
{
	int shm = ipc_shm_create(IPC_SHM_KEY, IPC_SHM_SIZE, IPC_CREAT|0666);

	if(shm <= 0) {
		printf("[%s:%d]ipc_shm_create error\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	char *pBuff = (char*)malloc(IPC_SHM_SIZE);
	if(NULL == pBuff) {
		printf("[%s:%d]no memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	unsigned int count = 0;
	while(count < 50) {
		count++;
		memset(pBuff, 0, IPC_SHM_SIZE);
		if(ipc_shm_read(shm, pBuff, IPC_SHM_SIZE) < 0) {
			printf("[%s:%d]ipc_shm_read error\n", __FUNCTION__, __LINE__);
			return -1;
		}
		printf("read data:%s\n", pBuff);
		sleep(1);
	}

	ipc_shm_delete(&shm, IPC_SHM_DETACH);
	
	return 0;
}
