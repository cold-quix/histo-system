/*
NAME:			constants.h
DATE:			2020-04-18
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	This file contains global constants used in the HISTO-SYSTEM system
	and #include statements for code common to all three programs.
	
*/

// Debug toggle
#define DEBUG			0 // Value doesn't matter


// Constants used by all programs
#define SHM_START			0
#define SHM_WRITE_START		1
#define SHM_END				255
#define SHM_SIZE			256
#define SHM_INVALID			-1
#define IS_CHILD	 		0
#define FORK_FAILURE		-1
#define SEMAPHORE_FAILURE	-1
#define SEMAPHORE_SUCCESS	0
#define DP1_WRITE_LIMIT		20
#define SIGINT_FLAG_DOWN	0
#define SIGINT_FLAG_UP		1


// Include statments for all programs
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "../src/randomchar.c"


// Definition of shared memory struct.  Common across all programs.
typedef struct tagSHAREDBUFFER {
	// Read and write positions will always loop from 255 back to 0
	int readPosition;
	int writePosition;
	char SHM_buffer[SHM_SIZE]; //256 characters
} SHAREDBUFFER;


