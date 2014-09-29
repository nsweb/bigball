


#include "../bigball.h"
#include "profiler.h"
#include "threadingutils.h"

namespace bigball
{

namespace Profiler
{

static Array<CallRoot> g_CallRoots;


/** Start profiling on all threads */
void Start()
{

}
void Stop()
{

}
void Reset()
{

}

void ThreadEntry( const char* szName );
{
	CallNode* newRoot = new CallNode( szName );

	//ThreadTools::


	
}

} /* namespace Profiler */

} /* namespace bigball */

