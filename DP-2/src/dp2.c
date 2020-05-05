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

int main(int argc, const char* argv[]) {
	#ifdef DEBUG
	printf("Hello world.  I am being run as a child process from DP-1 forking and exec-ing into DP-2.\n");
	#endif
	// The only command line argument given is the shmID - convert it to an int and use it to reattach
	
	// Verify that the shared memory exists and is accessble
	
	// If all is well, launch the DC
	
	// Initialize random number seed
	srandom(time(NULL));
	
	// Go into the DP-2 write loop
	
	
	
	
	
	
	
	
	return 0;
}




// Function that details the DP-2 write loop.
	void DP2_loop() {
		
	}


