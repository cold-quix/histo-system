/*
NAME:			semaphores.h
DATE:			2020-05-04
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	Contains a definition of the semaphore structure used in DP-1 and DP-2.
	This file was mostly adapted from the course examples provided in module 6.
*/

// The semaphore structure would be declared multiple times unless we use an 
// if-def-else chain to ensure it is only declared once.
#ifndef __SEMAPHORE_STRUCT_H__
#define __SEMAPHORE_STRUCT_H__


// Operation structures for semaphore control.  acquire_operation will DECREMENT 
// semaphore by 1 and release_operation will INCREMENT semaphore by 1.
// Both structs will work with first semaphore allocated.
struct sembuf acquire_operation = { 0, -1, SEM_UNDO };
struct sembuf release_operation = { 0, 1, SEM_UNDO };

// Array used to assign value to semaphore.  Initially 1 because the semaphore will 
// only allow 1 process to access it at a time.
unsigned short init_values[1] = { 1 };

#else
// If the compiler has already declared the semaphores, then just "tell the source file"
// that these structures exist and have been initialized.  No need to re-initialize or 
// re-declare them.
extern struct sembuf acquire_operation;
extern struct sembuf release_operation;

#endif