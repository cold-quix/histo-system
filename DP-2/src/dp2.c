/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DP-2 program.  The DP-2 program 
	is launched by the DP-1 program before launching the DC program.
*/

// Include statements
#include <sys/sem.h>
#include <time.h>
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"
#include "../inc/prototypes.h"

// Signal handle for SIGINT.
// Must be declared here because signal handlers need to have global scope in order to do anything.
sig_atomic_t signalFlag = SIGNAL_FLAG_DOWN;
void SIGINTHandler(int signal_number) {
	#ifdef DEBUG
	printf("[SIGNAL HANDLER]: Process was interrupted by SIGINT.\n");
	#endif
	// Flip global switch.
	signalFlag = SIGNAL_FLAG_UP;	
	// Reinstall.
	signal(signal_number, SIGINTHandler);
}


int main(int argc, const char* argv[]) {
	#ifdef DEBUG
	printf("[DP-2]: Hello world.  I am being run as a child process from DP-1 forking and exec-ing into DP-2.\n");
	printf("[DP-2]: The shmID I was given is: %s.  Converting to an int.\n", argv[1]);
	#endif
	
	// The only command line argument given is the shmID - convert it to an int and use it to reattach.
	int shmID = strtol(argv[1], NULL, 10);
	#ifdef DEBUG
	printf("[DP-2]: The shmID I was given is: %d.  It has been converted to an int.\n", shmID);
	#endif
	
	
	/*
	=================
	     SIGNALS
	=================
	*/
	// Set up signal handler.  Doing this before the fork because signals can be input at any time.
	signal(SIGINT, SIGINTHandler);
	
	
	/*
	==============
	     FORK
	==============
	*/
	/*
	If all is well, launch the DC.  The DC requires:
		- shmID
		- DP-1 PID
		- DP-2 PID
		
	*/
	// shmID was already stored above.
	int DP1_PID = getppid();
	int DP2_PID = getpid();
	
	
	#ifdef DEBUG
	printf("[DP-2]: Launching DC program.\n");
	#endif
	// Fork and exec into DC binary located in same directory.
	// Set up data for forking.
	pid_t	forkReturn;	// Return value from fork().  Indicates whether process is
						// parent or child.
	// String size doesn't matter; just reducing magic numbers.
	char shmIDString[SHM_SIZE]; 
	char DP1_PIDString[SHM_SIZE];
	char DP2_PIDString[SHM_SIZE];
	// Convert to strings.
	sprintf(shmIDString, "%d", shmID); 
	sprintf(DP1_PIDString, "%d", DP1_PID); 
	sprintf(DP2_PIDString, "%d", DP2_PID); 
	// Put into array for DC's command line.
	char* DC_argv[5] = {"Command-line", shmIDString, DP1_PIDString, DP2_PIDString, NULL};
	
	// Fork and pass strings as arguments.
	forkReturn = fork();
	// If it failed, exit
	if (forkReturn == FORK_FAILURE) {
		#ifdef DEBUG
		printf("[DP-2]: Error - fork() call failed.  Exiting.\n");
		#endif
		exit(0);
	}
	// If it worked, continue and determine whether DP-2 (parent) or DC (child)
	if (forkReturn == IS_CHILD) {
		#ifdef DEBUG
		printf("[DC]: DC process started successfully.\n");
		printf("[DC]: DC's PID is %ld and my parent's PID is %ld\n", (long)getpid(), (long)DP1_PID);
		#endif
		execv("./DC", DC_argv);
	}
	
	// The rest of the code will only be accessed by DP-2.
	
	
	/*
	=================
	  SHARED MEMORY
	=================
	*/
	// Verify that the shared memory exists and is accessble.
	// This pointer and the shmID (above) grant access to shared memory.
	// If we made it here, then DP-1 must have initialized data, so don't do that.
	SHAREDBUFFER* buffer_pointer = NULL;
	buffer_pointer = (SHAREDBUFFER* ) shmat(shmID, NULL, 0);
	// If the buffer is NULL, we couldn't attach to the memory properly.
	if (buffer_pointer == NULL) {
		#ifdef DEBUG
		printf("[DP-2]: Could not attach to shared memory.  Exiting.\n");
		#endif
		exit(0);
	}
	
	
	/*
	==============
	  SEMAPHORES
	==============
	*/
	// We didn't get the semID on the command line, but DP-1 stored it in shared memory.
	int semID = buffer_pointer->semaphoreID;
	
	
	/*
	==============
	     LOOP
	==============
	*/
	// Initialize random number seed
	srandom(time(NULL));
	
	// Go into the DP-2 write loop
	while (1) {
		// Write data to shared memory.
		DP2_loop(semID, buffer_pointer);
		// Check to see if a SIGINT was sent.
		if (signalFlag == SIGNAL_FLAG_UP) {
			#ifdef DEBUG
			printf("[DP-1]: Destroying shared memory and exiting.\n");
			#endif
			// Destroy shared memory, destroy semaphore, and exit.
			shmctl(shmID, IPC_RMID, NULL);
			semctl(semID, 0, IPC_RMID, 0);
			exit(0);
		}
	}
	
	
	
	
	
	
	
	return 0;
}




// Function that details the DP-2 write loop.
// Called within an infinite loop in main().
// Similar to DP-1 write loop, but only writes one character at a time instead 
// of up to 20.
	void DP2_loop(int semID, SHAREDBUFFER* buffer_pointer) {
		#ifdef DEBUG
		printf("[DP-2]: DP-2 beginning write operation.\n");
		#endif
		// Int to hold return value from semaphore access.
		int semReturn;
		
		// Acquire/decrement semaphore so we can write to circular buffer.
		semReturn = semop (semID, &acquire_operation, 1);
		if (semReturn == SEMAPHORE_FAILURE) {
			#ifdef DEBUG
			printf("[DP-2]: DP-2 cannot access critical region.  Waiting for next loop.\n");
			#endif
			// This is unlikely to happen, which is why it's okay to skip it.
		}
		
		// If we can write, then write a single character.
		else if (semReturn == SEMAPHORE_SUCCESS) {
			// If writePosition would go past the end, wrap it around to the start.
			if (buffer_pointer->writePosition > SHM_END) {
				#ifdef DEBUG
				printf("[DP-2]: Wrapping from end to start of circular buffer.\n");
				#endif
				buffer_pointer->writePosition = SHM_START;
			}
			// If writePosition would write into readPosition, stop the writing and 
			// let the reader get further ahead.		
			if (buffer_pointer->writePosition == buffer_pointer->readPosition) {
				#ifdef DEBUG
				printf("[DP-2]: Error - Write index and read index are equal, so buffer is full.  Aborting write operation.\n");
				#endif
				// Do nothing.
			}
			// If all is well, actually write random character value to shared memory buffer.	
			else {
				char randChar = randomAT();
				#ifdef DEBUG
				printf("[DP-2]: Writing character %c at index %d.\n", randChar, buffer_pointer->writePosition);
				#endif
				buffer_pointer->SHM_buffer[buffer_pointer->writePosition] = randChar;
				buffer_pointer->writePosition++; // Increment the writePosition if it actually wrote something
			}
		}
		
		#ifdef DEBUG
		printf("[DP-2]: DP-2 exiting write operation.  Releasing semaphore and sleeping for 1/20th of a second.\n");
		#endif
		// Release/increment semaphore.
		semReturn = semop (semID, &release_operation, 1);
		// Wait for 1/20th of a second, or 50 000 000 nanoseconds.
		// Struct to hold time value to enable sleeping for 1/20th of a second.
		struct timespec delay = {DP2_WAIT_SEC, DP2_WAIT_NSEC}; // 0, 50 000 000
		nanosleep(&delay, NULL);
		return;
	}







