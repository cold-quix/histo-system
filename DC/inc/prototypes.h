/*
NAME:			prototypes.h
DATE:			2020-05-03
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	Function prototypes used in DC.
*/

void DC_loop(int shmID, int DP1_PID, int DP2_PID, int semID, 
			 SHAREDBUFFER* buffer_pointer, int charCount[], char arrayASCII[]);
int getReadDestination(SHAREDBUFFER* buffer_pointer);






