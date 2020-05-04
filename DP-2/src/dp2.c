/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains the source code for the DP-2 program.  The DP-2 program 
	is launched by the DP-1 program before launching the DC program.
*/

// Include statements
#include "../inc/prototypes.h"
#include "../../common/inc/constants.h"

int main(int argc, const char* argv[]) {
	#ifdef DEBUG
	printf("Hello world.  I am being run as a child process from DP-1 forking and exec-ing into DP-2.\n");
	#endif
	
	
	
	
	
	return 0;
}