/*
NAME:			signalhandler.c
DATE:			2020-05-06
CODER:			Jack Lazarski Parkinson
DESCRIPTION:	
	Contains a signal handler for SIGINT which will be used
	by both DP processes.
*/

/*
The purpose of this function is to handle a SIGINT signal gracefully instead 
of crashing/killing the program.  Additionally, because signal handlers are global 
already, we can simply use a global variable to indicate whether or not the signal 
handler was invoked, then check for it at various appropriate points in the program.

Thus, sending the SIGINT signal to one of the programs will mean it exits gracefully 
and at the next appropriate point.

All this function does is set a global flag, then reinstall itself.
*/

void handlerSIGINT (int signal_number) {
	
}


