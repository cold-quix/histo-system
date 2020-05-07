/*
NAME:			constants.h
DATE:			2020-05-06
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DC program.  The DC program 
	is launched by the DP-2 program and reads data from a circular memory 
	buffer while printing a histogram to the screen depending on the amount 
	and type of data it has read.
*/

#include <sys/sem.h>
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"
#include "../inc/prototypes.h"
#include "../inc/constants.h"


// Signal handler for SIGINT
	// Send SIGINT signals to DP-1 and DP-2.
	// Do NOT terminate DC program.
sig_atomic_t signalFlagINT = SIGNAL_FLAG_DOWN; // 0
void SIGINTHandler(int signal_number) {
	#ifdef DEBUG
	printf("[DC]: Process was sent SIGINT signal.  Must send SIGINT signal to DP-1 and DP-2.\n");
	#endif
	// Flip global switch.
	signalFlagINT = SIGNAL_FLAG_UP; // 1
	// Reinstall.
	signal(signal_number, SIGINTHandler);
}

// Signal handler for SIGALRM
	// Set an alarm flag that enables the program to break out of a wait-loop.
sig_atomic_t signalFlagALRM = SIGNAL_FLAG_DOWN; // 0
void SIGALRMHandler(int signal_number) {
	#ifdef DEBUG
	printf("[DC]: Process was sent SIGALRM signal.\n");
	#endif
	// Flip global switch.
	signalFlagALRM = SIGNAL_FLAG_UP; // 1
	// Reinstall.
	signal(signal_number, SIGALRMHandler);
}

int main(int argc, char* argv[]) {
	#ifdef DEBUG
	printf("[DC]: Hello world.  This is the DC program being executed from DP-2.\n");
	printf("[DC]: Shared memory ID is - %s.\n", argv[1]);
	printf("[DC]: DP-1 PID is - %s.\n", argv[2]);
	printf("[DC]: DP-2 PID is - %s.\n", argv[3]);
	#endif
	// Convert arguments to ints so we can use them more easily.
	int shmID = strtol(argv[1], NULL, 10);
	int DP1_PID = strtol(argv[2], NULL, 10);
	int DP2_PID = strtol(argv[3], NULL, 10);
	
	/*
	=================
	  SHARED MEMORY
	=================
	*/
	// Check for shared memory existence.  If no, sleep(10).  If yes, continue.
	SHAREDBUFFER* buffer_pointer = NULL;
	buffer_pointer = (SHAREDBUFFER* ) shmat(shmID, NULL, 0);
	// If the buffer is NULL, we couldn't attach to the memory properly.
	// If the buffer was not NULL, then we attached properly.  Continue to main reading loop.
	while (buffer_pointer == NULL) {
		#ifdef DEBUG
		printf("[DC]: Could not attach to shared memory.  Sleeping for 10 seconds.\n");
		#endif
		sleep(10);
	}
	
	
	/*
	==============
	  SEMAPHORES
	==============
	*/
	// We didn't get the semID on the command line, but DP-1 stored it in shared memory.
	int semID = buffer_pointer->semaphoreID;
	
	
	/*
	=================
	     SIGNALS
	=================
	*/
	// Set up signal handler.
	signal(SIGINT, SIGINTHandler);
	signal(SIGALRM, SIGALRMHandler);
	
	
	/*
	==============
	     LOOP
	==============
	*/
	// Set up array to hold a count of each number of characters.
	int charCount[DC_ARRAY_LENGTH] = {}; // 19
	// Enter main reading loop.
	// Loop infinitely.
	while (1) {
		
		
		// If the program received a SIGINT signal, ignore these delays.
		if (signalFlagINT == SIGNAL_FLAG_DOWN) {
			// The program reads every 2 seconds, then writes every 5 read-loops (every 10 seconds).
			for (int countToFive = 0; countToFive < MULTIPLE_10_SEC; countToFive++) {
				// Wait for 2 seconds until a signal is sent.
				alarm(2);
				pause();
				// Read characters from the shared memory buffer into charCount[]
				
			}
			
			
		}
		
		
		
		
		
		
		// READING REQUIRES SEMAPHORE ACCESS
		// Every 2 seconds, get the index just before the write index [make sure to wrap properly], 
		// then read up until that index.  Then sleep [set SIGALRM flag back to 0].
		
		
		
		
		// Every 10 seconds, display the histogram based on what data has been collected.
			// see PDF assignment sheet for full details.  should comment a legend somewhere.
	
		
	
		// If the readPosition reaches the writePosition [should only happen when DP-1 and DP-2 are 
		// stopped by a SIGINT signal], then:
			// clear screen and display final histogram
			// clean up IPC usage
			// display message "Shazam !!"
			// exit
	}
		
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}




// Takes a pointer to shared memory and returns the index which the DC should
// read until, which is 1 behind the writePosition.
int getReadDestination(SHAREDBUFFER* buffer_pointer) {
	int readDestination = buffer_pointer->writePosition;
	readDestination--;
	// If the write index is at 0, wrap back to 255
	if (readDestination < SHM_START) {
		readDestination = SHM_END;
	}
	return readDestination;
}



//void countSHM(SHAREDBUFFER* buffer_pointer





