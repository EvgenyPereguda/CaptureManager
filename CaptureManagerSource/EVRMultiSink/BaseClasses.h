#pragma once

#include "../WinmmManager/WinmmManager.h"

#include <windows.h>
#include <windowsx.h>
#include <olectl.h>
#include <ddraw.h>
#include <mmsystem.h>

#ifdef	_MSC_VER
// disable some level-4 warnings, use #pragma warning(enable:###) to re-enable
#pragma warning(disable:4100) // warning C4100: unreferenced formal parameter
#pragma warning(disable:4201) // warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable:4511) // warning C4511: copy constructor could not be generated
#pragma warning(disable:4512) // warning C4512: assignment operator could not be generated
#pragma warning(disable:4514) // warning C4514: "unreferenced inline function has been removed"

#if _MSC_VER>=1100
#define AM_NOVTABLE __declspec(novtable)
#else
#define AM_NOVTABLE
#endif
#endif	// MSC_VER

#ifndef NUMELMS
#if _WIN32_WINNT < 0x0600
#define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#else
#define NUMELMS(aa) ARRAYSIZE(aa)
#endif   
#endif

#pragma warning(push)
#pragma warning(disable: 4312 4244)
// _GetWindowLongPtr
// Templated version of GetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
T _GetWindowLongPtr(HWND hwnd, int nIndex)
{
	return (T)GetWindowLongPtr(hwnd, nIndex);
}

// _SetWindowLongPtr
// Templated version of SetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
LONG_PTR _SetWindowLongPtr(HWND hwnd, int nIndex, T p)
{
	return SetWindowLongPtr(hwnd, nIndex, (LONG_PTR)p);
}
#pragma warning(pop)

#include <Unknwnbase.h>
#include <strmif.h>     // Generated IDL header file for streams interfaces
#include <intsafe.h>    // required by amvideo.h
#include "baseclasses\reftime.h"    // Helper class for REFERENCE_TIME management
#include "baseclasses\wxdebug.h"    // Debug support for logging and ASSERTs
#include <amvideo.h>    // ActiveMovie video interfaces and definitions
#include <control.h>


#include "baseclasses\measure.h"
#include "baseclasses\wxutil.h"     // General helper classes for threads etc
#include "baseclasses\combase.h"    // Base COM classes to support IUnknown
//#include <dllsetup.h>   // Filter registration support functions
//#include "measure.h"    // Performance measurement
//#include <comlite.h>    // Light weight com function prototypes


#include "baseclasses\schedule.h"
//#include <cache.h>      // Simple cache container class
#include "baseclasses\wxlist.h"     // Non MFC generic list class
#include "baseclasses\msgthrd.h"	// CMsgThread
#include "baseclasses\mtype.h"      // Helper class for managing media types
#include "baseclasses\fourcc.h"     // conversions between FOURCCs and GUIDs
#include <control.h>    // generated from control.odl
#include "baseclasses\ctlutil.h"    // control interface utility classes
#include <evcode.h>     // event code definitions
#include "baseclasses\amfilter.h"   // Main streams architecture class hierachy
#include "baseclasses\transfrm.h"   // Generic transform filter
#include "baseclasses\transip.h"    // Generic transform-in-place filter
#include <uuids.h>      // declaration of type GUIDs and well-known clsids
#include "baseclasses\source.h"	// Generic source filter
#include "baseclasses\outputq.h"    // Output pin queueing
#include <errors.h>     // HRESULT status and error definitions
#include "baseclasses\renbase.h"    // Base class for writing ActiveX renderers
#include "baseclasses\winutil.h"    // Helps with filters that manage windows
#include "baseclasses\winctrl.h"    // Implements the IVideoWindow interface
#include "baseclasses\videoctl.h"   // Specifically video related classes
#include "baseclasses\refclock.h"	// Base clock class
#include "baseclasses\sysclock.h"	// System clock
#include "baseclasses\pstream.h"    // IPersistStream helper class
#include "baseclasses\vtrans.h"     // Video Transform Filter base class
#include "baseclasses\amextra.h"
#include "baseclasses\cprop.h"      // Base property page class
#include "baseclasses\strmctl.h"    // IAMStreamControl support
#include <edevdefs.h>   // External device control interface defines
#include <audevcod.h>   // audio filter device error event codes
    // Base COM classes to support IUnknown



