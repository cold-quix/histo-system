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
#include "../inc/prototypes.h"
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"

// Signal handle for SIGINT.
// Must be declared here because signal handlers need to have global scope in order to do anything.
sig_atomic_t signalFlag = SIGINT_FLAG_DOWN;
void SIGINTHandler(int signal_number) {
	#ifdef DEBUG
	printf("[SIGNAL HANDLER]: Process was interrupted by SIGINT.\n");
	#endif
	// Flip global switch.
	signalFlag = SIGINT_FLAG_UP;	
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
	// Set up signal handler.
	signal(SIGINT, SIGINTHandler);
	
	
	/*
	==============
	  SEMAPHORES
	==============
	*/
	// We didn't get the semID on the command line, but DP-1 stored it in shared memory.
	int semID = buffer_pointer->semaphoreID;
	
	
	/*
	==============
	     FORK
	==============
	*/
	/*
	If all is well, launch the DC.  The DC requires:
		- DP-2 PID
		- DP-1 PID
		- shmID
	*/
	// DP-2 PID
	int DP2_PID = getpid();
	
	
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
	
	
	
	
	
	// Initialize random number seed
	srandom(time(NULL));
	
	// Go into the DP-2 write loop
	
	
	
	
	
	
	
	
	return 0;
}




// Function that details the DP-2 write loop.
	void DP2_loop() {
		
	}


