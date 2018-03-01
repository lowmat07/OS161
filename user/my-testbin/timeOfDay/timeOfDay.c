/*
 * timeOfDay program.
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int
main()
{
	time_t cur_ts;
	time( &cur_ts );

	if( cur_ts == ((time_t)-1))
	{
		printf( "Failure to obtain current time.\n" );
		return 1; //exit_failure
	}
	
	int h = ((cur_ts / 3600) - 8) % 24;
	int m = cur_ts / 60 % 60;
	int s = cur_ts % 60;

	printf( "formatted time: %02d:%02d:%02d\n", h, m, s );

	return 0; /* avoid compiler warnings */
}
