#ifndef __IPC_SHM_H__
#define __IPC_SHM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                       			  INCLUDE              	              *
 **************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

/**************************************************************************
 *                       			   MACROS              	              *
 **************************************************************************/ 
#define dbgPrintf		printf
/**************************************************************************
 *                          	TYPE DEFINITION   	                       *
 **************************************************************************/ 
typedef enum {
	IPC_SHM_DETACH = 0x1<<0,
	IPC_SHM_RMID   = 0x1<<1,
}ipc_shm_delete_mode;
/**************************************************************************
 *                           FUNCTION DECLARATION                         *
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
int ipc_shm_create(key_t shmKey, size_t shmSize, int shmFlag);

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
int ipc_shm_delete(int *pShm, int mode);
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
int ipc_shm_read(int shm, void *pBuffer, size_t buffSize);

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
int ipc_shm_write(int shm, const void *pData, size_t dataSize);

#ifdef __cplusplus
}
#endif

#endif
