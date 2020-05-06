/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DP-1 program.  This program initializes shared 
	memory, then launches the DP-2 and DC programs to produce and consume data, respectively.
*/

// Include statements
#include <sys/sem.h>
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"
#include "../inc/prototypes.h"

// Main
int main() {
	/*
	=================
	  SHARED MEMORY
	=================
	*/
	// Variables to hold shared memory connection data
	int shmID = 0;
	key_t shmkey = 0;
	SHAREDBUFFER* buffer_pointer = NULL;
	
	// Get the key to the shared memory block, using the current directory as the seed
	shmkey = ftok(".", 'M');
	
	// If the key can't be allocated, then exit (the scope of solving that is beyond this system)
	if (shmkey == SHM_INVALID) {
		#ifdef DEBUG
		printf("DP-1 shared memory key invalid.  Exiting.\n");
		#endif
		exit(0);
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
			printf("DP-1 shared memory ID invalid - shared memory cannot be allocated.  Exiting.\n");
			#endif
			exit(0);
		}
	}
	
	// If we made it here, then the shared memory was allocated properly.
	// Attach and initialize data just to be safe.
	buffer_pointer = (SHAREDBUFFER* ) shmat(shmID, NULL, 0);
	// If the buffer is NULL, we couldn't attach to the memory properly.
	if (buffer_pointer == NULL) {
		#ifdef DEBUG
		printf("DP-1 could not attach to shared memory.  Exiting.\n");
		#endif
		exit(0);
	}
	
	// Initialize array to 0s and read/write indices to starting locations, just to be safe
	for (int i = 0; i < SHM_SIZE; i++) {
		buffer_pointer->SHM_buffer[i] = 0;
	}
	buffer_pointer->readPosition = SHM_START; // 0
	buffer_pointer->writePosition = SHM_WRITE_START; // 1
	
	/*
	==============
	  SEMAPHORES
	==============
	*/
	// Set up semaphore access
	int semID;
	semID = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
	
	// Check if semaphore was allocated correctly.
	if (semID == SEMAPHORE_FAILURE) {
		#ifdef DEBUG
		printf("[PARENT]: DP-1 could not allocate semaphore.  Exiting.");
		#endif
		exit(0);
	}
	
	#ifdef DEBUG
	printf("[PARENT]: DP-1's semaphore ID is: %d\n", semID);
	printf("[PARENT]: DP-1 will initialize semaphore to known value");
	#endif
	if (semctl(semID, 0, SETALL, init_values) == SEMAPHORE_FAILURE) {
		#ifdef DEBUG
		printf("[PARENT]: DP-1 could not initialize semaphore.  Exiting.");
		#endif
		exit(0);
	}
	
	/*
	==============
	     FORK
	==============
	*/
	// If the program made it here, the semaphore must be allocated and initialized.
	// Thus, we can fork.
	#ifdef DEBUG
	printf("Everything is properly initialized.\n");
	printf("Launching DP-2 program.\n");
	#endif
	// Set up data for forking.
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
		printf("[DP-2]: DP-2 process started successfully.\n");
		printf("[DP-2]: DP-2's PID is %ld and my parent's PID is %ld\n", (long)getpid(), (long)parentPID);
		#endif
		// If DP-2, do that behaviour
		// Exec into the DP-2 program to handle it
		// Pass the shared memory ID to DP-2, and nothing else
		execv("./DP-2", argv);
		
		exit(0);
	}
	
	// The program will only make it here if it is the parent - no need for an else-if block
	// In the parent (DP-1), the return value will be the parent's PID, and not 0
	#ifdef DEBUG
	printf("[DP-1]: DP-1 process continuing as normal.\n");
	printf("[DP-1]: DP-1's PID is %ld .\n", (long)parentPID);
	#endif
	
	/*
	==============
	     LOOP
	==============
	*/
	// Initialize random number seed
	srandom(time(NULL));
		
	// Loop infinitely until the process is interrupted.
	while (1) {
		DP1_loop(semID, buffer_pointer);
	}
	
		
	return 0;
}


// Function that details the DP-1 write loop.
// Called within an infinite loop in main().
void DP1_loop(int semID, SHAREDBUFFER* buffer_pointer) {
	#ifdef DEBUG
	printf("[DP-1]: DP-1 beginning write operation.\n");
	#endif
	// Int to hold return value from semaphore access.
	int semReturn;
	
	// Acquire/decrement semaphore so we can write to circular buffer.
	semReturn = semop (semID, &acquire_operation, 1);
	if (semReturn == SEMAPHORE_FAILURE) {
		#ifdef DEBUG
		printf("[DP-1]: DP-1 cannot access critical region.  Waiting for next loop.\n");
		#endif
		// This is unlikely to happen, which is why it's okay to skip it.
	}
	// If we can write, then write 20 characters at random.
	else if (semReturn == SEMAPHORE_SUCCESS) {
		#ifdef DEBUG
		printf("[DP-1]: DP-1 has semaphore access to critical region.  Beginning 20-character write.\n");
		#endif
		for (int writeCount = 0; writeCount < DP1_WRITE_LIMIT; writeCount++) {
			// If writePosition would go past the end, wrap it around to the start.
			if (buffer_pointer->writePosition > SHM_END) {
				#ifdef DEBUG
				printf("[DP-1]: Wrapping from end to start of circular buffer.\n");
				#endif
				buffer_pointer->writePosition = SHM_START;
			}
			// If writePosition would write into readPosition, stop the writing and 
			// let the reader get further ahead.		
			if (buffer_pointer->writePosition == buffer_pointer->readPosition) {
				#ifdef DEBUG
				printf("[DP-1]: Error - Write index and read index are equal, so buffer is full.  Aborting write operation.\n");
				#endif
				break;
			}
			else {
				char randChar = randomAT();
				#ifdef DEBUG
				printf("[DP-1]: Writing character %c at index %d.\n", randChar, buffer_pointer->writePosition);
				#endif
				// If all is well, actually write random character value to shared memory buffer.			
				buffer_pointer->SHM_buffer[buffer_pointer->writePosition] = randChar;
				buffer_pointer->writePosition++; // Increment the writePosition if it actually wrote something
			}
		}
		
		#ifdef DEBUG
		printf("[DP-1]: DP-1 exiting write operation.  Releasing semaphore and sleeping for 2 seconds.\n");
		#endif
		// Release/increment semaphore so DP-2 can write to circular buffer.
		semReturn = semop (semID, &release_operation, 1);
		// Wait for 2 seconds.
		sleep(2);
	}
}






