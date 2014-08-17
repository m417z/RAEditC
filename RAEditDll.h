#ifndef RAEDITDLL_H_INCLUDED
#define RAEDITDLL_H_INCLUDED

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#endif // RAEDITDLL_H_INCLUDED
