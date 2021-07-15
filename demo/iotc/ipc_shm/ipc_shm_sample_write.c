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
	
	char *pData = (char*)malloc(IPC_SHM_SIZE);
	if(NULL == pData) {
		printf("[%s:%d]no memory\n", __FUNCTION__, __LINE__);
		return -1;
	}
	unsigned int count = 0;
	while(count < 10) {
		memset(pData, 0, IPC_SHM_SIZE);
		sprintf(pData, "test data(%s), count%d", argv[1], count++);
		if(ipc_shm_write(shm, pData, IPC_SHM_SIZE) < 0) {
			printf("[%s:%d]ipc_shm_write error\n", __FUNCTION__, __LINE__);
			return -1;
		}
		printf("write data:%s\n", pData);
		sleep(3);
	}

	ipc_shm_delete(&shm, IPC_SHM_DETACH);

	return 0;
}
