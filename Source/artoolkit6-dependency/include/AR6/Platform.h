/*
 *  Platform.h
 *  ARToolKit6
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015-2016 Daqri, LLC.
 *  Copyright 2011-2015 ARToolworks, Inc.
 *
 *  Author(s): Julian Looser, Philip Lamb
 *
 */

#ifndef __AR6_Platform_h__
#define __AR6_Platform_h__

// Determine the platform on which the code is being built

#if TARGET_PLATFORM_WINDOWS
// Include Windows API.
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#  endif
#  include <sdkddkver.h> // Minimum supported version. See http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745.aspx
#  include <windows.h>
#  if defined(WINAPI_FAMILY)
#    if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) // Windows Phone 8.1 and later.
#      if (_WIN32_WINNT >= 0x0603) // (_WIN32_WINNT_WINBLUE)
#        define TARGET_PLATFORM_WINRT 1
#      else
#        error ARToolKit for Windows Phone requires Windows Phone 8.1 or later. Please compile with Visual Studio 2013 or later with Windows Phone 8.1 SDK installed and with _WIN32_WINNT=0x0603 in your project compiler settings (setting /D_WIN32_WINNT=0x0603).
#      endif
#    elif (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP) // Windows Store 8.1 and later.
#      if (_WIN32_WINNT >= 0x0603) // (_WIN32_WINNT_WINBLUE)
#        define TARGET_PLATFORM_WINRT 1
#      else
#        error ARToolKit for Windows Store requires Windows 8.1 or later. Please compile with Visual Studio 2013 or later with Windows 8.1 SDK installed and with _WIN32_WINNT=0x0603 in your project compiler settings (setting /D_WIN32_WINNT=0x0603).
#      endif
#    endif
#  endif
#endif

// Configure preprocessor definitions for current platform

#if TARGET_PLATFORM_WINDOWS

#  define EXPORT_API __declspec(dllexport) 
#  define CALL_CONV __stdcall

#elif TARGET_PLATFORM_WINRT

#  ifndef LIBARCONTROLLER_STATIC
#    ifdef LIBARCONTROLLER_EXPORTS
#      define EXPORT_API __declspec(dllexport)
#    else
#      define EXPORT_API __declspec(dllimport)
#    endif
#  else
#    define EXPORT_API extern
#  endif
#  define CALL_CONV __stdcall

#elif TARGET_PLATFORM_MACOS || TARGET_PLATFORM_IOS || TARGET_PLATFORM_LINUX

#  define EXPORT_API
#  define CALL_CONV

#elif TARGET_PLATFORM_ANDROID

#  define EXPORT_API
#  define CALL_CONV

// Utility preprocessor directive so only one change needed if Java class name changes
#  define JNIFUNCTION(sig) Java_org_artoolkit_ar6_base_NativeInterface_##sig

#else
#  error Must define one of: TARGET_PLATFORM_MACOS TARGET_PLATFORM_IOS TARGET_PLATFORM_LINUX TARGET_PLATFORM_ANDROID TARGET_PLATFORM_WINDOWS TARGET_PLATFORM_WINRT
#endif

typedef void (CALL_CONV *PFN_LOGCALLBACK)(const char* msg);


#endif // !__AR6_Platform_h__
