/*
 * Me writeConsole program.
 */
#include <unistd.h>
#include <stdio.h>

int
main()
{
	pid_t fpid, gpid, wpid;
	int dest;
	//void *dest = (void *)sizeof(void *);
	
	printf("Test call for fork stub\n");
	fpid = fork();
	printf("fpid: %d\n", fpid);

	printf("Test call for getpid stub\n");
	gpid = getpid();
	printf("gpid: %d\n", gpid);
	
	printf("Test call for waitpid stub\n");
	wpid = waitpid(fpid, &dest, 0);
	printf("wpid: %d\n", wpid);


	return 0; /* avoid compiler warnings */
}
