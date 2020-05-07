/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DC program.  The DC program 
	is launched by the DP-2 program and reads data from a circular memory 
	buffer while printing a histogram to the screen depending on the amount 
	and type of data it has read.
*/

#include <sys/sem.h>
#include "../inc/prototypes.h"
#include "../../common/inc/constants.h"
#include "../../common/inc/semaphores.h"

int main(int argc, char* argv[]) {
	#ifdef DEBUG
	printf("[DC]: Hello world.  This is the DC program being executed from DP-2.\n");
	printf("[DC]: Shared memory ID is - %s.\n", argv[1]);
	printf("[DC]: DP-1 PID is - %s.\n", argv[2]);
	printf("[DC]: DP-2 PID is - %s.\n", argv[3]);
	#endif
	
	
	return 0;
}