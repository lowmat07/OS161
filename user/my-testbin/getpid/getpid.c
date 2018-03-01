/*
 * Me writeConsole program.
 */
#include <unistd.h>
#include <stdio.h>

int
main()
{
	pid_t my_pid = getpid();
	printf( "procId: %ld\n", (long int)my_pid );
	return 0; /* avoid compiler warnings */
}
