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
VSTypes.h
*********************************************************************/

#ifndef VSTypes_H
#define VSTypes_H /* Base Type Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include <stdio.h>
#include "VSConfig.h"
#include "VSPlatform.h"

#define _CRT_SECURE_NO_DEPRECATE


typedef signed char ppInt8; /* standard signed one byte type,
                            range is –128 to 127 */
typedef ppInt8 *PppInt8; /* pointer to ppInt8 */

typedef unsigned char ppUns8; /* standard signed one byte type,
                              range is 0 to 255 */
typedef ppUns8 *PppUns8;    /* pointer to ppUns8 */

typedef short int ppInt16;  /* standard signed two byte type,
                            range is –32,768 to 32,767 */
typedef ppInt16 *PppInt16;  /* pointer to ppInt16 */

typedef unsigned short int ppUns16; /* standard unsigned two byte type,
                                    range is 0 to 65,535 */
typedef ppUns16 *PppUns16;  /* pointer to ppUns16 */

#ifdef PDFLARGEINT

    typedef int ppInt32;       /* standard signed four byte type,
                           range is –2,147,483,648 to 2,147,483,647 */
    typedef ppInt32 *PppInt32; /* pointer to ppInt32 */

    typedef unsigned int ppUns32; /* standard signed four byte type,
                              range is 0 to 4,294,967,295 */
    typedef ppUns32 *PppUns32; /* pointer to ppUns32 */

#else

    typedef long int ppInt32;  /* standard signed four byte type,
                           range is –2,147,483,648 to 2,147,483,647 */
    typedef ppInt32 *PppInt32; /* pointer to ppInt32 */

    typedef unsigned long int ppUns32; /* standard signed four byte type,
                                   range is 0 to 4,294,967,295 */
    typedef ppUns32 *PppUns32; /* pointer to ppUns32 */

#endif

#ifdef WINDOWS_PLATFORM
    typedef signed __int64 ppInt64; /* standard signed eight byte type,
                                range is –9,223,372,036,854,775,808
                                to 9,223,372,036,854,775,807 */
    typedef unsigned __int64 ppUns64; /* standard signed eight byte type,
                                  range is 0 to 2^64-1 */
#else
    typedef long long ppInt64;   /* standard signed eight byte type,
                             range is –9,223,372,036,854,775,808
                             to 9,223,372,036,854,775,807 */
    typedef unsigned long long ppUns64; /* standard signed eight byte type,
                                    range is 0 to 2^64-1 */
#endif

typedef ppInt64 *PppInt64;   /* pointer to ppInt64 */
typedef ppUns64 *PppUns64;          /* pointer to ppUns64 */

typedef double ppReal;   /* standard signed eight byte type,
                         range is 1.7E +/- 308 (15 digits) */
typedef ppReal *PppReal; /* pointer to ppReal */

#define ULIntToReal(x)  ((ppReal)x)                 /* Type conversion from integer to real */
#define ULRealRoundToInt(x) ((ppInt32)(x+ 0.5f))    /* Type conversion from real to integer with rounding */
#define ULRealTruncToInt(x) ((ppInt32)x)            /* Type conversion from real to integer without rounding */
#define ULFloatToReal(x) ((ppReal)x)                /* Type conversion from float to real */
#define ULRealToFloat(x) ((double)x)                /* Type conversion from real to float */

#define ppMAXInt8           ((ppInt8)0x7F)          /* Maximum of ppInt8 range is 127 */
#define ppMINInt8           ((ppInt8)0x80)          /* Minimum of ppInt8 range is -128 */
#define ppMAXInt16          ((ppInt16)0x7FFF)       /* Maximum of ppInt8 range is 32,767 */
#define ppMINInt16          ((ppInt16)0x8000)       /* Minimum of ppInt8 range is –32,768 */
#define ppMAXInt32          ((ppInt32)0x7FFFFFFF)   /* Maximum of ppInt8 range is 2,147,483,647 */
#define ppMINInt32          ((ppInt32)0x80000000)   /* Minimum of ppInt8 range is –2,147,483,648 */
#define ppMAXUns8           ((ppUns8)0xFF)          /* Maximum of ppInt8 range is 255 */
#define ppMINUns8           ((ppUns8)0x00)          /* Minimum of ppInt8 range is 0 */
#define ppMAXUns16          ((ppUns16)0xFFFF)       /* Maximum of ppInt8 range is 65,535 */
#define ppMINUns16          ((ppUns16)0x0000)       /* Minimum of ppInt8 range is 0 */
#define ppMAXUns32          ((ppUns32)0xFFFFFFFF)   /* Maximum of ppInt8 range is 4,294,967,295 */
#define ppMINUns32          ((ppUns32)0x00000000)   /* Minimum of ppInt8 range is 0 */

#if defined( WINDOWS_PLATFORM ) && defined(__BOOL_DEFINED)
    typedef bool ppBool;
#else
    typedef ppUns8 ppBool;     /* Boolean Type : true != 0, false == 0 */
#endif

#if (!HAS_BOOL_SUPPORT) || !defined(__BOOL_DEFINED)
#   ifndef true
#       define true    1 /* default value of true is 1 */
#   endif
#   ifndef false
#       define false   0 /* default value of false is 0 */
#   endif
#endif

#ifndef NULL
#   ifdef __cplusplus
#       define NULL    0 /* default value of NULL is 0 ( zero ) */
#   else
#       define NULL   ((void *)0)  /* default value of NULL is pointer to 0 ( zero ) */
#   endif
#endif

/* Primary Data Object Type */

typedef void *PDFHandle;      /* Pointer Type */

#ifdef DBG
typedef struct _t_CosObj *PCosObj;    /* pointer to Data Object */
#endif	

typedef 
#ifndef COSOBJISPOINTER 
    struct _t_PDFCosHandle{
#   ifdef DBG
        PCosObj a;    /* pointer to Data Object */
#   else
        void *a;
#   endif
    ppInt32 b;              /* Cos Object Identifier  */
} 
#else 
#   ifdef DBG
	PCosObj
#   else
	PDFHandle
#   endif	
    
#endif
    PDFCosHandle,               /* Cos Object Handle    */
    PDFActionHandle,            /* Action Handle        */
    PDFAnnotationHandle,        /* Annotation Handle    */
    PDFOutlineHandle,           /* Outline Handle       */
    PDFDestinationHandle,       /* Destination Handle   */
    PDFThreadHandle,            /* Thread Handle        */
    PDFBeadHandle;              /* Bead Handle          */


#if DBG
    typedef struct _t_PDFStrm *PDFStreamHandle;     /* Stream Handle */
    typedef struct _t_PDFDoc *PDFDocHandle;         /* PDF Document Handle */
    typedef struct _t_PDFLib *PDFLibHandle;         /* PDF Library Handle */
    typedef struct _t_PDFFile *PDFFileHandle;       /* PDF File Handle */
    typedef struct _t_PDFCrypto *PDFCryptoHandle;   /* Encryption Handle */
    typedef struct _t_PBX *PBXHandle;               /* Paint Box Handle */
	typedef struct TMImage *PDFImageHandle;
#else
    typedef void *PDFStreamHandle;                  /* Stream Handle */
    typedef void *PDFDocHandle;                     /* PDF Document Handle */
    typedef void *PDFLibHandle;                     /* PDF Library Handle */
    typedef void *PDFFileHandle;                    /* PDF File Handle */
    typedef void *PDFCryptoHandle;                  /* Encryption Handle */
    typedef void *PBXHandle;                        /* Paint Box Handle */
    typedef void *PDFImageHandle;                   /* PDF Image Handle */ 
#endif

    typedef ppUns32 ppAtom;                         /* Names Space Key */
#define ppAtomNull ppMAXUns32						/* Bad Atom Value - No name */

/* Rectangle Type */


typedef struct  {
    ppReal  xl;     /* Left border coordinate */
    ppReal  yl;     /* Top border coordinate */
    ppReal  xr;     /* Right border coordinate */
    ppReal  yr;     /* Bottom border coordinate */
} TPDFRect;         /* Rectangle Type */

/* Point Type */
typedef struct  {
    ppReal  x;      /* horizontal axis coordinate */
    ppReal  y;      /* vertical axis coordinate */
} TPDFRealPoint;    /* Point Type */

/* RGB Color Type */
typedef struct  {
    ppReal  R;      /* Red component of Color */
    ppReal  G;      /* Green component of Color */
    ppReal  B;      /* Blue component of Color */
} TPDFRGBColor;     /* RGB Color Type */

/* CMYK Color Type */
typedef struct  {
    ppReal  C;      /* Cyan component of Color */
    ppReal  M;      /* Magenta component of Color */
    ppReal  Y;      /* Yellow component of Color */
    ppReal  K;      /* Black component of Color */
}TPDFCMYKColor;     /* CMYK Color Type */

/* Color Device Type */
typedef enum  {
    cgGray = 1,
    cgRGB = 3,
    cgCMYK = 4
} TPDFColorDevice;      /* Color Device Type */


/* PDF Color Type */
typedef struct  {
    TPDFColorDevice Device;         /* Color Device */
    union {                     /* Color Value */
        ppReal          Gray;   /* Gray scale value */
        TPDFRGBColor    RGB;    /* RGB color value */
        TPDFCMYKColor   CMYK;   /* CMYK color value */
    } Color;                    /* Color Value */
} TPDFColor;                    /* PDF Color Type */

typedef TPDFColor *PPDFColor;   /* Pointer to PDF Color Type */


typedef void ( CDECL *PDFErrorProc ) (PDFLibHandle p, ppInt32 ErrorCode, void *Opaque); /* Fuction type for Pointer to a user-supplied error-handling function. */

typedef void * ( CDECL *PDFAllocProc ) ( ppUns32 size );			/* Fuction type for allocate memory on library */
typedef void * ( CDECL *PDFReAllocProc ) ( void *mem, ppUns32 size );	/* Fuction type for reallocate memory on library */
typedef void ( CDECL *PDFFreeProc ) ( void *mem );					/* Fuction type for free memory on library */

typedef void ( CDECL * PDFPaintContent ) ( PBXHandle PaintBox, void * Data ); /* Functional Type for Repaint Acroform
																		Objects Routines - reload drawing */


typedef ppUns32 ( CDECL * StreamSetPositionProc ) ( PDFLibHandle Lib, void * AStream, ppUns32 Position ); /* Function type for set position of the stream */
typedef ppUns32 ( CDECL *StreamGetPositionProc ) ( PDFLibHandle Lib, void * AStream ); /* Function type for get position of the stream */
typedef ppUns32 ( CDECL * StreamReadBufferProc ) ( PDFLibHandle Lib, void * AStream, void * Buffer, ppUns32 Size ); /* Function type for read data from stream */
typedef ppUns32 ( CDECL * StreamWriteBufferProc ) ( PDFLibHandle Lib, void * AStream, void * Buffer, ppUns32 Size ); /* Function type for write data to stream */
typedef ppUns32 ( CDECL *StreamGetSizeProc ) ( PDFLibHandle Lib, void * AStream ); /* Function type for get size of the stream */
typedef ppUns32 ( CDECL *StreamGetCharProc ) ( PDFLibHandle Lib, void * AStream ); /* Function type for get char from current position of the stream */
typedef ppUns32 ( CDECL *StreamLookCharProc ) ( PDFLibHandle Lib, void * AStream ); /* Function type for look char from current position of the stream */



#endif /* #ifndef VSTypes_H */




