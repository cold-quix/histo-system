/*
NAME:			constants.h
DATE:			2020-05-06
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DC program.  The DC program 
	is launched by the DP-2 program and reads data from a circular memory 
	buffer while printing a histogram to the screen depending on the amount 
	and type of data it has read.
	
	The legend for the histogram it creates can be found below in 
	printHistogram().
*/

// Includ estatements
#include <sys/sem.h>
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"
#include "../inc/prototypes.h"
#include "../inc/constants.h"

/*
NAME:		SIGINTHandler
PARAMETERS:	int signal_number				- signal ID
RETURN:		None
DESC:
	Signal handler for SIGINT.  Must be declared here because signal handlers need 
	to have global scope to do anything.
	
	Sets a flag indicating the signal was received, which is checked for programmatically, 
	then reinstalls itself.
	
*/
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

/*
NAME:		SIGALRMHandler
PARAMETERS:	int signal_number				- signal ID
RETURN:		None
DESC:
	Signal handler for SIGALRM.  Must be declared here because signal handlers need 
	to have global scope to do anything.
	
	Sets a flag indicating the signal was received, which is checked for programmatically, 
	then reinstalls itself.
	
*/
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

// Main
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
		#ifdef DEBUG
		printf("[DC]: Beginning main loop.\n");
		#endif
		DC_loop(shmID, DP1_PID, DP2_PID, semID, buffer_pointer, charCount, arrayASCII);
		
		// If the process was sent a SIGINT signal, stop the other two processes
		if (signalFlagINT == SIGNAL_FLAG_UP) {
			#ifdef DEBUG
			printf("[DC]: Received a SIGINT signal.  Time to close up shop.\n");
			#endif
			kill(DP1_PID, SIGINT);
			kill(DP2_PID, SIGINT);
			
			// clear screen and display final histogram
			system("clear");
			printHistogram(charCount, arrayASCII);
			// clean up IPC usage - nothing to do here because DP-1 cleans shared memory 
			// and semaphores.  the DC only needs to sent DP-1 and DP-2 SIGINT signals.
			#ifdef DEBUG
			printf("[DC]: Destroying shared memory and exiting.\n");
			#endif
			// Destroy shared memory, destroy semaphore, and exit.
			shmctl(shmID, IPC_RMID, NULL);
			semctl(semID, 0, IPC_RMID, 0);
			// display message "Shazam !!"
			printf("Shazam !!\n");
			// exit
			exit(0);
		}		
	}
	return 0;
}


/*
NAME:		DC_loop
PARAMETERS:	int shmID						- shared memory ID
			int DP1_PID						- DP1 process ID
			int DP2_PID						- DP2 process ID
			SHAREDBUFFER* buffer_pointer	- pointer to shared memory
			int charCount					- array that counts characters
											  in shared memory buffer
			char arrayASCII					- array with char values to 
											  compare against
RETURN:		None
DESC:
	The read loop for the DC program.  Like the "_loop" functions in dc1.c and dc2.c, 
	this is essentially an extension of main(), which it was passed so many parameters.
	
	The general operation of the read loop is:
		- pause() for 2 seconds
		- read up to just before the write index
		- after doing this five times, display a histogram
	
*/
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
	printHistogram(charCount, arrayASCII);
	
}


/*
NAME:		getReadDestination
PARAMETERS:	SHAREDBUFFER* buffer_pointer	- shared memory ID
RETURN:		int readDestination
DESC:
	Takes a pointer to shared memory and returns the index which the DC shoulds 
	read until, which is 1 behind the writePosition.  Putting this in a function 
	makes it easier to account for wrapping.
*/
int getReadDestination(SHAREDBUFFER* buffer_pointer) {
	int readDestination = buffer_pointer->writePosition;
	readDestination--;
	// If the write index is at 0, wrap back to 255
	if (readDestination < SHM_START) {
		readDestination = SHM_END;
	}
	return readDestination;
}


/*
NAME:		printHistogram
PARAMETERS:	SHAREDBUFFER* buffer_pointer	- shared memory ID
RETURN:		int readDestination
DESC:
	Prints the histogram to the screen. The int array parameter will always be ordered from A-T.
	Symbols:
		"-": 1
		"+": 10
		"*": 100
	Format:
		[CHAR]-### [symbols]
	Example:
		[A]-012 +-
*/
void printHistogram(int charCount[], char arrayASCII[]) {
	// Clear the screen.
	system("clear");
	// For each character, print its histogram row, then a new line.
	for (int i = 0; i < DC_ARRAY_LENGTH; i++) {
		// Determine the histogram string and store it
		int hundreds = (charCount[i] / 100);			// Number of "*"
		int tens = (charCount[i] % 100) / 10;			// Number of "+"
		int ones = ( (charCount[i] % 100) % 10);		// Number of "-"
		
		// Print the character
		printf(HISTO_1, arrayASCII[i]);
		// Print the number
		printf(HISTO_2, charCount[i]);
		// Print the symbol string
		for (int count = 0; count < hundreds; count++) {
			printf("*");
		}
		for (int count = 0; count < tens; count++) {
			printf("+");
		}
		for (int count = 0; count < ones; count++) {
			printf("-");
		}
		
		// Print new line
		printf("\n");
	}
}









