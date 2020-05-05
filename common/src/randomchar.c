/*
NAME:			randomchar.c
DATE:			2020-05-04
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	Contains functions used for generating random characters
*/

#include <time.h>

// Generates a random character from A-T inclusive, then returns it
// Generate a random number between 1-20 inclusive
// Return the corresponding character: A = 0, T = 19
char randomAT() {
	char randChar;
	int range = 19;
	
	
	randChar = random() % range;
	randChar += 'A'; //Convert int value to ASCII value, starting at A
	return randChar;
}




