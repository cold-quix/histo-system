/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DP-1 program.  This program initializes shared 
	memory, then launches the DP-2 and DC programs to produce and consume data, respectively.
*/

// Include statements
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include "../inc/prototypes.h"
#include "../../common/inc/constants.h"


// Definition of shared memory struct.  Common across all programs.
typedef struct tagSHAREDBUFFER {
	// Read and write positions will always loop from 255 back to 0
	int readPosition;
	int writePosition;
	char SHM_buffer[SHM_SIZE]; //256 characters
} SHAREDBUFFER;

int main() {
	// Variables to hold shared memory connection data
	int shmID = 0;
	key_t shmkey = 0;
	SHAREDBUFFER* buffer_pointer = NULL;
	
	// Get the key to the shared memory block, using the current directory as the seed
	shmkey = ftok(".", 'M');
	
	// If the key can't be allocated, then exit (the scope of solving that is beyond this system)
	if (shmkey == SHM_INVALID) {
		#ifdef DEBUG
		printf("DP-1 shared memory key invalid.  Exiting with return code 1.\n");
		#endif
		return 1;
	}
	
	// If the key is valid, then use it to connect
	shmID = shmget(shmkey, sizeof (SHAREDBUFFER), 0);
	// If the shared memory ID isn't valid, then we need to allocate shared memory
	if (shmID == SHM_INVALID) {
		#ifdef DEBUG
		printf("Allocating shared memory.\n");
		#endif
		shmID = shmget(shmkey, sizeof (SHAREDBUFFER), IPC_CREAT | 0660);
		
		// If the shared memory can't be allocated, then exit (the scope of solving that is beyond this system)
		if (shmID == SHM_INVALID) {
			#ifdef DEBUG
			printf("DP-1 shared memory ID invalid - shared memory cannot be allocated.  Exiting with return code 2.\n");
			#endif
			return 2;
		}
	}
	
	// If we made it here, then the shared memory was allocated properly.
	// Attach and initialize data just to be safe.
	buffer_pointer = (SHAREDBUFFER* ) shmat(shmID, NULL, 0);
	// If the buffer is NULL, we couldn't attach to the memory properly.
	if (buffer_pointer == NULL) {
		#ifdef DEBUG
		printf("DP-1 could not attach to shared memory.  Exiting with return code 3.\n");
		#endif
		return 3;
	}
	
	// Initialize array to 0s and read/write indices to starting locations, just to be safe
	for (int i = 0; i < SHM_SIZE; i++) {
		buffer_pointer->SHM_buffer[i] = 0;
	}
	buffer_pointer->readPosition = 0;
	buffer_pointer->writePosition = 0;
	
	#ifdef DEBUG
		printf("Everything is properly initialized.\n");
		#endif
	
	
	
	
	
	
	
	
	return 0;
}