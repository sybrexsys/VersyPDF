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
VSLibI.h
*********************************************************************/

#ifndef VSLibI_H
#define VSLibI_H
#ifndef __GNUC__ /* suppress gcc warning */ 
#pragma once
#endif /* __GNUC__ */


#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "../VSTypes.h"
#include "../VSBaseA.h"
#include "VSAssert.h"
#include "VSName.h"


#ifdef WINDOWS_PLATFORM
#include <windows.h>
#include <winbase.h>
#endif




    void *PDFIntMalloc(PDFLibHandle Lib, size_t Size);
    void *PDFIntRealloc(PDFLibHandle Lib, void *Buffer, size_t Size);
	void PDFIntFree(PDFLibHandle Lib, void *buf);

#define MemoryDelta 0x2000
#define mmalloc PDFIntMalloc
#define mrealloc PDFIntRealloc
#define mfree PDFIntFree
#define FILE_BUFFER_SIZE 0x4000



#define CosMagicValue 0x0F17

#undef  ROUND
#define ROUND(x)  (((x) < 0) ? (ppInt32)((x) - 0.5) : (ppInt32)((x) + 0.5))

#ifndef max
#define max(x,y) ((x>y)?x:y)
#endif
#ifndef min
#define min(x,y) ((x<y)?x:y)
#endif

#define PRO 2
#define STD 1

#ifdef LICENSED
#define CheckEdition( Lib, E) if (((PPDFLib)Lib)->Edition < E ) PDFRAISE (Lib, PDFBuildErrCode(ErrGeneralLevel, gleUnsupportedFeatureError));
#define IsEdition(Lib, E) (((PPDFLib)Lib)->Edition < E ? false:true )
#else
#define CheckEdition( Lib, E) {};
#define IsEdition(Lib, E) true
#endif

    enum StrmType {
        strtFile,
        strtMemory,
		strtCustom
    };

    typedef struct ppAtomNode {
        struct ppAtomNode   *Left, *Right;
        ppUns32             ID;
        char                *String;
        char                *EscStr;
        ppInt32             Length;
        ppInt8              Balance;
    } ppAtomNode, *PppAtomNode;

    typedef struct _t_PDFLib{
        /* Atom array management*/
        ppUns32         AtomCount;
        PppAtomNode     *AtomArray;
        ppUns32         AtomCapacity;
        ppAtomNode      AtomRoot;
        PppAtomNode     LastAtom;
        /*Error Management*/
        ppUns32         LastError;
        /* Exception management*/
        ppUns32         ExcCount;
        ppUns32         ExcCapacity;
        PDFjmp_buf      *ExcBuffer;
        ppBool          ADM;
        ppUns32		*Names;
		/* User defined procedures */
		PDFAllocProc	allocproc; 
		PDFReAllocProc  reallocproc;
		PDFFreeProc		freeproc; 
		PDFErrorProc	errorproc; 
		void			*opaque;
        ppBool          MemRaiseOnNULL;

#ifdef LICENSED
		ppUns8		   Edition;
#endif  
		
		/* 
		* Decryption key schedule tables for AES
		*/
		int				KT_init ;
		ppUns32			KT0[256];
		ppUns32			KT1[256];
		ppUns32			KT2[256];
		ppUns32			KT3[256];

CDebugHeader (	ppInt32			DBGIndex; )
    } PDFLib, *PPDFLib;

    typedef struct _t_PDFFile{
        PDFLibHandle    Lib;
        char            *FileName;
        ppFileOpenMode  Mode;
        ppUns8          *Buffer;
        ppUns32         FileSize;
        ppUns32         BufferSize;
#ifdef  WINDOWS_PLATFORM
        PVOID           Map;
        ppUns32         Size;
        ppUns32         Position;
        HANDLE          File;
#else
        FILE            *File;
#endif
    } ppFile, *PppFile;

	struct PDFStreamFileSection {
		PDFFileHandle	FileHandle;
		ppBool			HandleCreated;
	};

	struct PDFStreamMemorySection {
		void            *Buffer;
		ppUns32         Size;
		ppUns32         Capacity;
		ppUns32         Position;
	};

	struct PDFStreamCustomSection {
		void					*AStream;
		StreamSetPositionProc	SetPosition;
		StreamGetPositionProc	GetPosition;
		StreamGetSizeProc		GetSize;
		StreamReadBufferProc	ReadBuffer;
		StreamWriteBufferProc	WriteBuffer;
		StreamGetCharProc		GetChar;
		StreamLookCharProc		LookChar;
	};

    typedef struct _t_PDFStrm {
        enum StrmType   Type;
        PDFLibHandle    Lib;
        ppBool          RO;
		union  {
			struct PDFStreamFileSection File;
			struct PDFStreamMemorySection Memory;
			struct PDFStreamCustomSection Custom;
		} Sections;
    } * PULStrm, TULStrm;
#define _StreamPosition( Stream ) ( ( ( PULStrm ) Stream )->Sections.Memory.Position )
#define _StreamSize( Stream ) ( ( ( PULStrm ) Stream )->Sections.Memory.Size )
#define _StreamCapacity( Stream ) ( ( ( PULStrm ) Stream )->Sections.Memory.Capacity )
#define _StreamBuffer( Stream ) ( (ppUns8 *)(( ( PULStrm ) Stream )->Sections.Memory.Buffer) )
#define _StreamLib( Stream ) ( ( ( PULStrm ) Stream )->Lib )


	typedef enum PDFEncryptAlgorithm {
		eaNone,
		eaRC4,
		eaAES
	} PDFEncryptAlgorithm;

	typedef struct PDFAlgorithmItem {
		ppAtom Name;
		PDFEncryptAlgorithm Algorithm;
	} PDFAlgorithmItem;

    typedef struct _t_PDFCrypto {
        ppInt32         KeyLength;
        ppUns8          key[32];
        ppUns8          COP[48];
        ppUns8          CUP[48];
		ppUns8          COPE[32];
		ppUns8          CUPE[32];
		ppUns8			Perm[16];
        ppInt32         Permission;
        ppInt32         Version;
        ppInt32         Revision;
        PDFCosHandle    FileID;
        PDFAlgorithmItem *   AlgList;
		ppUns32			AlgListCount;
		PDFCosHandle    Algorithms;
		PDFEncryptAlgorithm DefStringAlg;
		PDFEncryptAlgorithm DefStreamAlg;
		ppAtom			DefString;
		ppAtom			DefStream;
		ppBool			EncryptMetadata;
		ppBool			RemoveOldFilters;
		ppUns32			nkl;

    } TPDFCryptoType, *PPDFCryptoType;

    typedef struct PDFList {
        PDFHandle         *FList;
        ppUns32         Size;
        ppUns32         Capacity;
        PDFLibHandle    Lib;
    } TPDFList, *PPDFList;
    
    typedef struct PDFExtList {
        void            *FList;
        ppUns32         Size;
        ppUns32         Capacity;
        ppUns32         EntrySize;
        PDFLibHandle    Lib;
    } TPDFExtList, *PPDFExtList;


    typedef struct TPDFParser{
        PDFDocHandle    Doc;
        PDFStreamHandle Stream;
		ppBool			GenerateExceptionOnEOF;
    } TPDFParser, * PPDFParser;

#define _STREAM(s) ((PULStrm)s)
#define _LIBRARY(l) ((PPDFLib)l)


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSLibI_H */
