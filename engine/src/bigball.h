
// bigball.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef BB_BIGBALL_H
#define BB_BIGBALL_H

// defined with this macro as being exported.
#ifdef BIGBALL_EXPORTS
#define BIGBALL_API __declspec(dllexport)
#elif defined( BIGBALL_IMPORTS )
#define BIGBALL_API __declspec(dllimport)
#else
#define BIGBALL_API
#endif


#if _WIN32 || _WIN64
	#include "targetver.h"

	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>
#endif


#include "core/core.h"
#include "core/log.h"
#include "core/templates.h"
#include "core/array.h"
#include "core/string.h"
#include "math/constants.h"
#include "math/functions.h"
#include "math/vector.h"


#endif // BB_BIGBALL_H