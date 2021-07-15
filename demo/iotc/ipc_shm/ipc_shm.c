/**************************************************************************
 *                       			  INCLUDE              	              *
 **************************************************************************/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "ipc_shm.h"
/**************************************************************************
 *                       			   MACROS              	              *
 **************************************************************************/ 


/**************************************************************************
 *                          	TYPE DEFINITION   	                       *
 **************************************************************************/ 
typedef struct {
	int ipcShmId;
	int ipcSemId;
	int shmSize;
	void *pShmAddr;
}ipc_shm_t;

union semun {
  int			   val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
							  (Linux-specific) */
};

/**************************************************************************
 *                       		   VARIABLES              	              *
 **************************************************************************/



/**************************************************************************
 *                           FUNCTION DECLARATION                         *
 **************************************************************************/

/**************************************************************************
 *                           FUNCTION DEFINITION                          *
 **************************************************************************/
 
/*
 *Name: ipc_shm_create
 *Brief:create ipc shared memory identifier.
 *Args:
 *[key_t] shmKey --- a value to be assocatetd with the specified shared memory.
 *[size_t] shmSize --- the size of the specified shared memory.
 *[int] shmFlag 
 *The value shmFlag is composed of:
 *     IPC_CREAT   --- to create a new segment.  If this flag is not used, then ipc_shm_create() will find the segment associated with shmKey and check to see if the user has permission to access the segment.
 *     IPC_EXCL    --- used with IPC_CREAT to ensure failure if the segment already exists.
 *     mode_flags  --- (least significant 9 bits) specifying the permissions granted to the owner, group, and world.  These bits have the same format, and the same meaning, as the mode argument of open(2).
                   Presently, the execute permissions are not used by the system.
 *Return:
 *On success, a valid shared memory identifier is returned.  On error, -1 is returned.
 */

int ipc_shm_create(key_t shmKey, size_t shmSize, int shmFlag)
{
	if(0 == shmKey) {
		dbgPrintf("shmKey is zero\n");
		return -1;
	}

	if(0 == shmSize) {
		dbgPrintf("shmSize is zero\n");
		return -1;
	}

	ipc_shm_t *pShm = (ipc_shm_t*)malloc(sizeof(ipc_shm_t)); 
	if(NULL == pShm) {
		dbgPrintf("no memory\n");
		return -1;
	}
	memset(pShm, 0, sizeof(ipc_shm_t));
	
	
	int semId = semget(shmKey, 1, shmFlag);
	if(semId < 0) {
		free(pShm);
		pShm = NULL;
		dbgPrintf("semget error\n");
		return -1;
	}
	
	union semun semUnion;
    semUnion.val = 1;
    if (-1 == semctl(semId, 0, SETVAL, semUnion)) {
		free(pShm);
		pShm = NULL;
		dbgPrintf("semctl set value error\n");
		perror("semctl");
		return -1;
    }
    
	int shmId = shmget(shmKey, shmSize, shmFlag);
	if(shmId < 0) {
		free(pShm);
		pShm = NULL;
		dbgPrintf("shmget error\n");
		return -1;
	}
	
	void *pShmAddr = shmat(shmId, NULL, 0);
	if(NULL == pShmAddr) {
		free(pShm);
		pShm = NULL;
		dbgPrintf("shmat error\n");
		return -1;
	}
	
	pShm->ipcSemId = semId;
	pShm->ipcShmId = shmId;
	pShm->pShmAddr = pShmAddr;
	pShm->shmSize = shmSize;
	
	dbgPrintf("semId(%d)\n", semId);
	dbgPrintf("shmId(%d)\n", shmId);
	return (int)pShm;
}

/*
 *Name: ipc_shm_delete
 *Brief:Detaches the shared memory from the address space of the calling process. Or Mark the shared memory segment to be destroyed.
 *Args:
 *[int *] pShm --- pointer to the variable which is returned by ipc_shm_create.
 *[int]mode is composed of :
 *		IPC_SHM_DETACH --- Detaches the shared memory from the address space of the calling process.
 *		IPC_SHM_RMID   --- Mark the shared memory segment to be destroyed.
 *Return:
 *On success, 0 is returned.  On error, -1 is returned.
 */

int ipc_shm_delete(int *pShm, int mode)
{
	ipc_shm_t *shm = (ipc_shm_t*)(*pShm);
	int ipcShmId = shm->ipcShmId;
	int ipcSemId = shm->ipcSemId;
	int ipcShmSize = shm->shmSize;
	void *pShmAddr = shm->pShmAddr;
	
	if(ipcShmId < 0 || ipcSemId < 0) {
		dbgPrintf("ipcShmId or ipcSemId is less than zero\n");
		return -1;
	}
	
	if(NULL == pShmAddr) {
		dbgPrintf("shm addr error\n");
		return -1;
	}
	
	
	if(mode & IPC_SHM_DETACH) {
		if(-1 == shmdt(pShmAddr)) {
			dbgPrintf("shmdt error\n");
			return -1;
		}
	}
	
	if(mode & IPC_SHM_RMID) {
		union semun semUnion;
		if (-1 == semctl(ipcSemId, 0, IPC_RMID, semUnion)) {
			dbgPrintf("Failed to delete semaphore\n");
			return -1;
		}
		
		if(-1 == shmctl(ipcShmId, IPC_RMID, 0)) {
			dbgPrintf("Failed to delete share memory\n");
			return -1;
		}
	}
	
	free(*pShm);
	*pShm = NULL;
	return 0;
}

/*
 *Name: ipc_shm_read
 *Brief:Read data from the shared memory segment.
 *Args:
 *[int] shm --- the variable which is returned by ipc_shm_create.
 *[void *]pBuffer --- the buffer to copy the data in shared memory segment.
 *[size_t]buffSize --- the size of buffer.
 *Return:
 *On success, 0 is returned.  On error, -1 is returned.
 */

int ipc_shm_read(int shm, void *pBuffer, size_t buffSize)
{
	/* parse parameter */
	ipc_shm_t *pShm = (ipc_shm_t*)shm;
	if(NULL == pShm) {
		dbgPrintf("shm error\n");
		return -1;
	}
	
	int ipcShmId = pShm->ipcShmId;
	int ipcSemId = pShm->ipcSemId;
	int ipcShmSize = pShm->shmSize;
	void *pShmAddr = pShm->pShmAddr;
	
	if(ipcShmId < 0 || ipcSemId < 0) {
		dbgPrintf("ipcShmId or ipcSemId is less than zero\n");
		return -1;
	}
	
	if(NULL == pShmAddr) {
		dbgPrintf("shm adderss is null\n");
		return -1;
	}

	/* do read action */
	if(buffSize < ipcShmSize) {
		memcpy(pBuffer, pShmAddr, buffSize);
	}
	else {
		memcpy(pBuffer, pShmAddr, buffSize);
	}
	
	return 0;
}

/*
 *Name: ipc_shm_write
 *Brief:Write data to the shared memory segment.
 *Args:
 *[int] shm --- the variable which is returned by ipc_shm_create.
 *[void *]pData --- the data to be copied the shared memory segment.
 *[size_t]dataSize --- the size of data.
 *Return:
 *On success, 0 is returned.  On error, -1 is returned.
 */
int ipc_shm_write(int shm, const void *pData, size_t dataSize)
{
	/* parse parameter */
	ipc_shm_t *pShm = (ipc_shm_t*)shm;
	if(NULL == pShm) {
		dbgPrintf("shm error\n");
		return -1;
	}
	
	int ipcShmId = pShm->ipcShmId;
	int ipcSemId = pShm->ipcSemId;
	int ipcShmSize = pShm->shmSize;
	void *pShmAddr = pShm->pShmAddr;
	
	if(ipcShmId < 0 || ipcSemId < 0) {
		dbgPrintf("ipcShmId or ipcSemId is less than zero\n");
		return -1;
	}
	
	if(NULL == pShmAddr) {
		dbgPrintf("shm adderss is null\n");
		return -1;
	}
	
	if(ipcShmSize < dataSize) {
		dbgPrintf("datasize is bigger than shmsize\n");
		return -1;
	}
	
	/* wait ipc semaphore */
	struct sembuf ipcSemBuf;
    ipcSemBuf.sem_num = 0;
    ipcSemBuf.sem_op = -1;
    ipcSemBuf.sem_flg = SEM_UNDO;
    if (-1 == semop(ipcSemId, &ipcSemBuf, 1)) {
        dbgPrintf("wait semaphore fail\n");
        return -1;
    }

	/* do write action */
	memcpy(pShmAddr, pData, dataSize);
	
	/* release ipc semaphore */
	ipcSemBuf.sem_num = 0;
    ipcSemBuf.sem_op = 1;
    ipcSemBuf.sem_flg = SEM_UNDO;
    if (-1 == semop(ipcSemId, &ipcSemBuf, 1)) {
        dbgPrintf("release semaphore fail\n");
        return -1;
    }
	return 0;
}

#ifdef __cplusplus
}
#endif

