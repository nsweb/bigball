
//
#include <cstdio>
//
//#ifdef WIN32
//#   define WIN32_LEAN_AND_MEAN
//#   include <windows.h>
//#endif
//
//#if defined __ANDROID__
//#   include <android/log.h>
//#   include <unistd.h> /* for gettid() */
//#else
//#   include <cstdarg>
//#endif

#include "../bigball.h"
#include "log.h"

namespace bigball
{

static int PrintInArray( Array<char>& OutBuffer, char const* Format, ... )
{
	va_list ap;
	va_start( ap, Format );

	int needed = vsnprintf(nullptr, 0, Format, ap);
	OutBuffer.resize( needed );
	vsnprintf( &OutBuffer[0], needed, Format, ap );
	OutBuffer.push_back( '\0' );

	va_end(ap);

	return needed+1;
}

void BBLog::Print( char const* File, int Line, char const* Category, Type type, char const* Format, ... )
{
    va_list ap;
    va_start( ap, Format );

	char const* LogPrefix[] =
	{
		"Info",
		"Warning",
		"ERROR",
	};

#if defined _WIN32 || _WIN64

	Array<char> Buffer, TmpBuffer;
	int needed = vsnprintf(nullptr, 0, Format, ap);
	TmpBuffer.resize( needed );
	vsnprintf( &TmpBuffer[0], needed, Format, ap );
	TmpBuffer.push_back( '\n' );
	TmpBuffer.push_back( '\0' );

	PrintInArray( Buffer, "%s(%d) [%s] %s : %s", File, Line, LogPrefix[type], Category, &TmpBuffer[0] );

	OutputDebugString( &Buffer[0] );
	printf( &TmpBuffer[0] ); 
#else
	fprintf( stderr, "%s: ", LogPrefix[type] );
	vfprintf( stderr, format, ap );
#endif

    va_end(ap);
}

} /* namespace lol */

