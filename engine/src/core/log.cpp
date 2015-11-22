
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

#if defined _MS_VER

	String TmpString = String::Printf( Format, ap );
	TmpString += "\n";

	String BufferString = String::Printf( "%s(%d) [%s] %s : %s", File, Line, LogPrefix[type], Category, TmpString.c_str() );

	OutputDebugString( BufferString.c_str() );
	printf( TmpString.c_str() ); 
#else
	fprintf( stderr, "%s: ", LogPrefix[type] );
	vfprintf( stderr, Format, ap );
#endif

    va_end(ap);
}

} /* namespace bigball */

