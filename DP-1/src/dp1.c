/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DP-1 program.  This program initializes shared 
	memory, then launches the DP-2 and DC programs to produce and consume data, respectively.
*/

// Include statements
#include "../inc/prototypes.h"
#include "../../common/inc/constants.h"

// Main
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
	printf("Launching DP-2 program.\n");
	#endif
	
	
	// Set up data for forking
	pid_t	forkReturn;	// Return value from fork().  Indicates whether process is
						// parent or child.
	pid_t	childPID;	// PID of DP-2
	pid_t	parentPID;	// PID of DP-1
	parentPID = getpid(); // Save current PID
	
	// Shared memory ID that will be passed to DP-2 if forking is successful.
	// This is located here because it must be done before the fork.
	char shmIDString[SHM_SIZE]; // Size doesn't matter; just reducing magic numbers
	sprintf(shmIDString, "%d", shmID); // Convert to string
	char* argv[3] = {"Command-line", shmIDString, NULL};
	
	// Fork process, then check to see if it worked
	forkReturn = fork(); 
	
	// If it failed, exit
	if (forkReturn == FORK_FAILURE) {
		#ifdef DEBUG
		printf("Error - fork() call failed.  Exiting.\n");
		#endif
		exit(0);
	} 
	
	// If it worked, go on
	// Determine whether this is the DP-1 (parent) or DP-2 (child)
	else if (forkReturn == IS_CHILD) {
		#ifdef DEBUG
		printf("[CHILD]: DP-2 process started successfully.\n");
		printf("[CHILD]: DP-2's PID is %ld and my parent's PID is %ld\n", (long)getpid(), (long)parentPID);
		#endif
		// If DP-2, do that behaviour
		// Exec into the DP-2 program to handle it
		// Pass the shared memory ID to DP-2, and nothing else
		execv("./DP-2", argv);
		
		exit(0);
	}
	
	// In the parent (DP-1), the return value will be the parent's PID, and not 0
	else if (forkReturn > 0) {
		#ifdef DEBUG
		printf("[PARENT]: DP-1 process continuing as normal.\n");
		printf("[PARENT]: DP-1's PID is %ld .\n", (long)parentPID);
		sleep(5); // Give the child time to work before the parent exits.
		#endif
		// If DP-1, do that behaviour
		
		
		exit(0);
	}
	
	
	
	
	
	
	
	
	
	
	return 0;
}


	// Function that details the DP-1 program loop.
	void DP1_loop() {
		
	}






