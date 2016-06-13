/*********************************************************************

This file is part of the VersyPDF project.
Copyright (C) 2005 - 2016 Sybrex Systems Ltd. All rights reserved.
Authors: Vadzim Shakun , et al.

VersyPDF is free software: you can redistribute it and/or modify it 
under the terms of the GNU Affero General Public License as published 
by the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version, with the addition of the following 
permission added to Section 15 as permitted in Section 7(a):
FOR ANY PART OF THE COVERED WORK IN WHICH THE COPYRIGHT IS OWNED BY 
SYBREX SYSTEMS. SYBREX SYSTEMS DISCLAIMS THE WARRANTY OF NON 
INFRINGEMENT OF THIRD PARTY RIGHTS.
                    
VersyPDF is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU Affero General Public License for more details.
                                                                   
In accordance with Section 7(b) of the GNU Affero General Public License, 
a covered work must retain the producer line in every PDF that is created 
or manipulated using VersyPDF.

You can be released from the requirements of the license by purchasing 
a commercial license. Buying such a license is mandatory as soon as you 
develop commercial activities involving VersyPDF without disclosing 
the source code of your own applications, offering paid services to customers 
as an ASP, serving PDFs on the fly in a commerce web application, 
or shipping VersyPDF with a closed source product.

For more information, please contact Sybrex Systems at http://www.sybrex.com

----------------------------------------------------------------------
VSPlatform.h
*********************************************************************/

#ifndef VSPlatform_H
#define VSPlatform_H /* Hardware and OS Platform definitions */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#ifdef _mips
#   define SGI_PLATFORM
#   define UNIX_PLATFORM            /* Unix platform ftype */
#   define PDFPLATFORM "SGI MIPS"   /* SGI Irix for MIPS CPU Operation system */
#	define PDFPLATFORMID 4
#endif 


#if defined(MACOS) || defined(TARGET_OS_MAC) || defined(__APPLE__) || defined(__MACOS__)
#   define READMODE    "rb"         /* Read mode parameters */
#   define WRITEMODE   "wb"         /* Write mode parameters */
#   define APPENDMODE  "ab"         /* Append mode parameters */
#   define UNIX_PLATFORM
#   if defined(__POWERPC_) || defined (__ppc__)
#       define MACOSX_PLATFORM          /* MacOS Platform */
#       define PDFPLATFORM "MacOS X"    /* MacOS X Operation System */
#   endif
#	define PDFPLATFORMID 5
#endif

#ifdef __sun
#   define UNIX_PLATFORM                /* Unix Platform Type */
#   ifdef __sparc
#       define SUN_SPARC_PLATFORM 
#       define PDFPLATFORM "SunOS Sparc"  /* SunOS for Sparc Operation System */
#		define PDFPLATFORMID 6
#   endif
#   ifdef __i386
#       define SUN_I386_PLATFORM
#       define PDFPLATFORM "SunOS i386"   /* SunOS for i386 Operating System */
#		define PDFPLATFORMID 7
#   endif
#endif

#ifdef _AIX
#   define AIX_PLATFORM 
#   define PDFPLATFORM "AIX"        /* AIX Operation System */
#   define UNIX_PLATFORM            /* Unix Platform Type */
#	define PDFPLATFORMID 8
#endif

#ifdef __hpux
#   define HPUX_PLATFORM 
#   define PDFPLATFORM "HP-UX"      /* HP-UX Operation System */
#   define UNIX_PLATFORM            /* Unix Platform Type */
#	define PDFPLATFORMID 9
#endif

#if defined(__digital__) && defined(__unix__) /* Tru64 alpha CPU cc compiler defoult definitions */
#   define TRU64_PLATFORM           
#   define UNIX_PLATFORM
#   define PDFPLATFORM "TRU64"      /* Tru64 (OSF/1) Operating System */
#	define PDFPLATFORMID 10
#endif

#if __linux 
#   define UNIX_PLATFORM   /* Unix Platform Type */
#   ifdef __i386
#       define LINUX_I386_PLATFORM
#       define PDFPLATFORM "Linux i386" /* Linux i386 Operation System */
#		define PDFPLATFORMID 11
#   endif
#   ifdef __ia64
#       define LINUX_IA64_PLATFORM
#       define PDFPLATFORM "Linux ia64" /* Linux ia64 Operating System */
#		define PDFPLATFORMID 12
#   endif
#   ifdef __x86_64
#       define LINUX_X86_64_PLATFORM
#       define PDFPLATFORM "Linux x86_64" /* Linux x86_64 Operating System */
#       define PDFPLATFORMID 20
#   endif
#endif

#ifdef __FreeBSD__
#   define UNIX_PLATFORM
#   ifdef __i386
#       define FREEBSD_I386_PLATFORM
#       define PDFPLATFORM "FreeBSD i386" /* FreeBSD i386 Operating System */
#		define PDFPLATFORMID 13
#   endif
#   ifdef __ia64
#       define FREEBSD_IA64_PLATFORM
#       define PDFPLATFORM "FreeBSD ia64" /* FreeBSD ia64 Operating System */
#		define PDFPLATFORMID 14
#   endif
#endif

#ifndef UNIX_PLATFORM
#	ifdef __unix
#		define UNIX_PLATFORM
#		ifdef __i386
#			define OTHERUNIX_I386_PLATFORM
#			define PDFPLATFORM "Other UNIX i386" /* Other UNIX i386 Operating System */
#			define PDFPLATFORMID 19
#		endif
#	endif
#endif


#if defined(_WIN32) || defined(WIN32)
#   define PDFPLATFORM "Windows 9x/NT/2000/XP" /* Windows 9x/NT/2000/XP Operation System */
#   define WINDOWS_PLATFORM    /* Windows platform */
#   define READMODE "rb"       /* Read mode parameters */
#   define WRITEMODE "wb"      /* Write mode parameters */
#   define APPENDMODE "ab"     /* Append mode parameters */
#   define PATH_SEPARATOR "\\" /* Path separator by Windows OS */
#   define CDECL _cdecl
#   ifdef PDFX
#       define _EXP [ export ]
#	define PDFPLATFORMID 15
#   endif
#   ifdef PDFDLL
#       define LIB_API __declspec(dllexport) /* PDF Library API Enable */
#   endif  
#   ifdef DOTNET
#		define PDFPLATFORMID 16
#       define USESTDEXCEPTION
#       pragma warning ( disable: 4297) 
#       define _EXP 
#		define COSOBJISPOINTER	
#   endif
#	ifdef DELPHI
#		define COSOBJISPOINTER
#		define PDFPLATFORMID 17
#	endif
#	ifdef _M_IA64
#		define PDFPLATFORMID 21
#	endif
#	ifdef _M_AMD64
#       ifdef PDFPLATFORMID
#           undef PDFPLATFORMID
#       endif
#		define PDFPLATFORMID 22
#	endif
#	ifndef PDFPLATFORMID
#		define PDFPLATFORMID 18
#	endif
#endif

/* Default Values */
#ifndef READMODE
#   define READMODE "r" /* Read mode parameters */
#endif

#ifndef WRITEMODE
#   define WRITEMODE "w" /* Write mode parameters */
#endif

#ifndef APPENDMODE
#   define APPENDMODE "a" /* Append mode parameters */
#endif


#ifdef UNIX_PLATFORM
#   if defined(__i386) || defined(__alpha) || defined ( __ia64 ) || defined(__x86_64 )
#       ifdef  PDFBIGENDIAN  /* Little Endian Number Data Store Type*/
#           undef PDFBIGENDIAN
#       endif
#   else
#       ifndef PDFBIGENDIAN /* Big Endian Number Data Store Type*/
#           define PDFBIGENDIAN
#       endif
#   endif
#   if defined(__alpha) || defined(__ia64) || defined(__x86_64)
#       define PDFLARGEINT /* Enable Large Integer Definition */
#   endif
#   if defined(_AIX) && defined(__64BIT__)
#       define PDFLARGEINT
#   endif
#endif


#ifndef LIB_API
#   define LIB_API 
#endif

#ifndef CDECL
#   define CDECL 
#endif

#ifndef _EXP
#   define _EXP 
#endif


#endif /* #ifndef VSPlatform_H */
