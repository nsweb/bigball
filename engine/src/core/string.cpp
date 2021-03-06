
//#include <cstdio>
//
//#ifdef WIN32
//#   define WIN32_LEAN_AND_MEAN
//#   include <windows.h>
//#endif
//
//#include <cstdarg>
//
//#include "core.h"

#include "../bigball.h"

namespace bigball
{

String String::Printf(char const *format, ...)
{
    String ret;

    va_list ap;
    va_start(ap, format);
    ret = vaPrintf(format, ap);
    va_end(ap);

    return ret;
}

String String::vaPrintf(char const *format, va_list ap)
{
    String ret;

    va_list ap2;
#if !defined _MSC_VER
    /* Visual Studio 2010 does not support va_copy. */
    va_copy(ap2, ap);
#else
    ap2 = ap;
#endif

    /* vsnprintf() tells us how many character we need, and we need to
     * add one for the terminating null byte. */
    int needed = vsnprintf(nullptr, 0, format, ap2) + 1;
    va_end(ap2);

    ((Super &)ret).reserve(needed);
    ret.m_count = needed;

    vsnprintf(&ret[0], needed, format, ap);

    return ret;
}

} /* namespace bigball */
