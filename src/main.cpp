#include "CFA.h"

// forward declarations
void runTest( int argc, char** argv);

int main(int argc, char **argv)
{
	//parse command line arguments
	parseOptions(argc, argv);

	runTest();

	return 0;
}

void ParseOptions(int argc, char **argv)
{
	for(int i=1; i<argc; ++i)
	{
		char *arg = argv[i];

		//parse option
	}
}

////////////////////////////////////////////////////////////////////////////////
//	Parse input file and run test
////////////////////////////////////////////////////////////////////////////////
void runTest()
{
	//Init CFA
	//filename?
    CFA Analysis();

    StopWatchInterface *timer = 0;
    sdkCreateTimer( &timer );
    sdkStartTimer( &timer );

    //CFA tests

    sdkStopTimer( &timer );
    printf( "Processing time: %f (ms)\n", sdkGetTimerValue( &timer ) );
    sdkDeleteTimer( &timer );
}