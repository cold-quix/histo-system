/*
NAME:			constants.h
DATE:			2020-05-06
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DC program.  The DC program 
	is launched by the DP-2 program and reads data from a circular memory 
	buffer while printing a histogram to the screen depending on the amount 
	and type of data it has read.
	
	=====
	ADD LEGEND FOR HISTOGRAM EITHER HERE OR IN printHistogram().
	=====
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
	exit(0);
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
	// Set up arrays to hold a count of each number of characters, and reference values 
	// for each character.
	int charCount[DC_ARRAY_LENGTH] = {}; // 19
	char arrayASCII[DC_ARRAY_LENGTH] = {
		'A', 'B', 'C', 'D', 'E', 
		'F', 'G', 'H', 'I', 'J', 
		'K', 'L', 'M', 'O', 'P', 
		'Q', 'R', 'S', 'T',
	};
		
		
	// Enter main reading loop.
	// Loop infinitely.
	while (1) {
		DC_loop(shmID, DP1_PID, DP2_PID, semID, buffer_pointer, charCount, arrayASCII);
		
		
		
		// Every 10 seconds, display the histogram based on what data has been collected.
			
	
		
	
		// If the readPosition reaches the writePosition [should only happen when DP-1 and DP-2 are 
		// stopped by a SIGINT signal], then:
			// clear screen and display final histogram
			// clean up IPC usage
			// display message "Shazam !!"
			// exit
	}
		
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}


// Put the main reading loop in a separate function because it's cleaner that way.
void DC_loop(int shmID, int DP1_PID, int DP2_PID, int semID, 
			 SHAREDBUFFER* buffer_pointer, int charCount[], char arrayASCII[]) {
	// Every loop, the function pauses for 2 seconds.  Every 5 pauses is 10 seconds, so 
	// print the histogram every 5 loops [see below].
	int pauseCount = 0;
	for (pauseCount; pauseCount < 5; pauseCount++) {
		// Wait for 2 seconds using SIGALRM
		alarm(2);
		pause();
		// Get/decrement semaphore.
		int semReturn = semop(semID, &acquire_operation, 1);
		
		// If we can't get semaphore access, don't do anything.
		if (semReturn == SEMAPHORE_FAILURE) {
			#ifdef DEBUG
			printf("[DC]: DC cannot access critical region.  Waiting for next loop.\n");
			#endif
			// This is unlikely to happen, which is why it's okay to skip it.
		}
		// If we have semaphore access
		else if (semReturn == SEMAPHORE_SUCCESS) {
			// Find the destination index.
			int destPosition = getReadDestination(buffer_pointer);
			#ifdef DEBUG
			printf("[DC]: Beginning read operation.\n");
			printf("[DC]: Read index is currently %d.\n", buffer_pointer->readPosition);
			printf("[DC]: Destination index is %d, which is 1 less than the write index of %d.\n", 
				   destPosition, buffer_pointer->writePosition);
			#endif
			// Iterate through shared memory until you reach the destination index.
			for (buffer_pointer->readPosition; 
				 
				 
				 
				 
				 
				 buffer_pointer->readPosition != destPosition; 
				 
				 
				 
				 
				 
				 buffer_pointer->readPosition++) {
				// Wrap back around if necessary
				if (buffer_pointer->readPosition > SHM_END) {
					#ifdef DEBUG
					printf("[DC]: Wrapping around to start of shared buffer.\n");
					#endif
					buffer_pointer->readPosition = SHM_START;
				}
				char tempChar = buffer_pointer->SHM_buffer[buffer_pointer->readPosition];
				#ifdef DEBUG
				printf("[DC]: Reading character %c from shared memory index %d.\n", 
					   tempChar, buffer_pointer->readPosition);
				#endif
				// for each character you read, compare it against the ASCII array
				for (int i = 0; i < DC_ARRAY_LENGTH; i++) {
					
					// if the character you read matches something in the ASCII array, increment 
					// the equivalent element in the count array
					if (tempChar == arrayASCII[i]) {
						charCount[i]++;
					}
				}
			}
		}
		#ifdef DEBUG
		printf("[DC]: Releasing/incrementing semaphore.\n");
		#endif
		// Release/increment semaphore.
		semReturn = semop (semID, &release_operation, 1);
	}
	
	#ifdef DEBUG
	printf("[DC]: Printing histogram.\n");
	#endif
	// Print histogram.  Happens every 5 read-loops, which is about every 10 seconds.
	// See PDF assignment sheet for full details.  should comment a legend somewhere.
	
	
}


// Takes a pointer to shared memory and returns the index which the DC should
// read until, which is 1 behind the writePosition.
// Putting this in a function makes it easier to account for wrapping.
int getReadDestination(SHAREDBUFFER* buffer_pointer) {
	int readDestination = buffer_pointer->writePosition;
	readDestination--;
	// If the write index is at 0, wrap back to 255
	if (readDestination < SHM_START) {
		readDestination = SHM_END;
	}
	return readDestination;
}




// Read characters from shared memory.  For each character read, count it in charCount[].
void readToCharCount(SHAREDBUFFER* buffer_pointer, int charCount[], int readDestination) {
	for (buffer_pointer->readPosition;
		 buffer_pointer->readPosition < readDestination;
		 buffer_pointer->readPosition++) {
		
		char tempChar = buffer_pointer->SHM_buffer[buffer_pointer->readPosition];
		// If the value is decimal 0, then it's uninitialized, so ignore it.  This will happen 
		// only with the first character read.
		if (tempChar != 0) {
			
		}
		
		
		
	}
	
}


// Prints the histogram to the screen. The int array will always be ordered from A-T.
void printHistogram(int charCount[]){
	
	
	
	
	
	
}









