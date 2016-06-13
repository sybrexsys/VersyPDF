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
VSBaseA.c
*********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../VSBaseA.h"
#include "VSLibI.h"
#include "VSMisc.h"
#include "VSAssert.h"

#ifdef WINDOWS_PLATFORM
#include <windows.h>
#include <winbase.h>
#endif


/* Atom Section*/


ppInt32 ULModFindOrAppendAtom(PDFLibHandle Lib, char *str)
{
	PPDFLib l   =  ( PPDFLib ) Lib;
	ppInt32 i, cmp;
	char *n;
	PppAtomNode  /* T = &( l-> AtomRoot), */
		S,P, Q;
	S = P = l->AtomRoot.Right;
	if ( !S ){
		S =  ( PppAtomNode ) mmalloc ( Lib, sizeof ( ppAtomNode ) );
		i = ( ppInt32 ) strlen ( str );
		n = ( char * ) mmalloc ( Lib, i + 1 );
		memcpy ( n, str, i + 1 );
		S->Balance = 0;
		S->ID = 0;
		S->Left = NULL;
		S->Right = NULL;
		S->String = n;
		S->EscStr = ULGetEscName ( Lib, n );
		S->Length = ULGetNameSize ( n );
		if ( ( l->AtomCount ) == l->AtomCapacity ){
			l->AtomCapacity += CalculateDelta(l->AtomCapacity);
			l->AtomArray = ( PppAtomNode *) mrealloc ( Lib, l->AtomArray, sizeof ( PppAtomNode ) * l->AtomCapacity );
		};
		l->AtomArray[l->AtomCount++] = S;
		l->AtomRoot.Right = S;
		return 0;
	}
A2 : 
	cmp = strcmp ( str, P->String );
	if ( !cmp )
		return P->ID;
	if ( cmp < 0 ){
		Q = P->Left;
		if ( !Q ){
			Q = ( PppAtomNode ) mmalloc ( Lib, sizeof ( ppAtomNode ) );
			P->Left = Q;
			goto A5;
		}
	} else{
		Q = P->Right;
		if ( !Q ){
			Q = ( PppAtomNode )mmalloc ( Lib, sizeof ( ppAtomNode ) );
			P->Right = Q;
			goto A5;
		}
	}
	/*    if ( Q->Balance ){
	T = P;
	S = Q;
	}*/
	P = Q;
	goto A2;
A5 : 
	i = ( ppInt32 ) strlen ( str );
	n = ( char * ) mmalloc ( Lib, i + 1 );
	memcpy ( n, str, i + 1 );
	Q->Balance = 0;
	Q->ID = l->AtomCount;
	Q->Left = NULL;
	Q->Right = NULL;
	Q->String = n;
	Q->EscStr = ULGetEscName ( Lib, n );
	Q->Length = ULGetNameSize ( n );
	if ( ( l->AtomCount ) == l->AtomCapacity ){
		l->AtomCapacity += CalculateDelta ( l->AtomCapacity ) ;
		l->AtomArray = ( PppAtomNode * )mrealloc ( Lib, l->AtomArray, sizeof ( PppAtomNode ) * l->AtomCapacity );
	};
	l->AtomArray[l->AtomCount] = Q;
	return l->AtomCount++;
}

ppAtom ULStringToAtom(PDFLibHandle Lib, char *str)
{
	return ULModFindOrAppendAtom ( Lib, str );
}

char * ULAtomToString(PDFLibHandle Lib, ppAtom atom)
{
    if ( atom >= ( (PPDFLib) Lib )->AtomCount )
        return NULL;
    return ( (PPDFLib) Lib )->AtomArray[atom]->String;
}

ppInt32 ULGetAtomCount(PDFLibHandle Lib)
{
    return ( (PPDFLib) Lib )->AtomCount;
}


void ULFindAtom(PDFLibHandle Lib, char *str, PppAtomNode node)
{
    ppInt32 i;
	if ( !node )
		return;
    i = strcmp ( str, node->String );
    if ( i < 0 )
        ULFindAtom ( Lib, str, node->Left );
    else if ( i > 0 )
        ULFindAtom ( Lib, str, node->Right );
    else
        ( (PPDFLib) Lib )->LastAtom = node;
}

ppBool ULExistsAtomForString(PDFLibHandle Lib, char *str)
{
    ( (PPDFLib) Lib )->LastAtom = NULL;
	if ( ( (PPDFLib) Lib )->AtomRoot.Right )
		ULFindAtom ( Lib, str, ( (PPDFLib) Lib )->AtomRoot.Right );
	else if ( ( (PPDFLib) Lib )->AtomRoot.Left )
		ULFindAtom ( Lib, str, ( (PPDFLib) Lib )->AtomRoot.Left );
    return ( (PPDFLib) Lib )->LastAtom ? true : false;
}

/* File Section*/

ppInt32 ULWriteFileBuffer(PDFFileHandle file, void *buffer, ppInt32 len)
{
#ifdef WINDOWS_PLATFORM
    ppUns32 sz;
    BOOL b;
#endif
    if ( len == 0 )
        return 0;
    if ( ( ( PppFile ) file )->Mode == ppFileReadMode )
        return 0;
#ifdef WINDOWS_PLATFORM
    b = WriteFile ( ( ( PppFile ) file )->File, buffer, len, &sz, NULL );
    if ( !b )
        PDFRAISE ( ( ( PppFile ) file )->Lib,
                   PDFBuildErrCode ( ErrGeneralLevel, gleCannotSaveBufferToFileError ) );
#else
    if ( fwrite ( buffer, 1, len, ( ( PppFile ) file )->File ) != len )
        PDFRAISE ( ( ( PppFile ) file )->Lib,
                   PDFBuildErrCode ( ErrGeneralLevel, gleCannotSaveBufferToFileError ) );
#endif
    return 0;
}

ppUns32 ULFileGetPosition(PDFFileHandle file)
{
    if ( ( ( PppFile ) file )->Mode == ppFileReadMode )
    #ifdef WINDOWS_PLATFORM
        return ( ( PppFile ) file )->Position;
#else
    return ( ppInt32 ) ftell ( ( ( PppFile ) file )->File );
#endif
    else
        return ( ( PppFile ) file )->FileSize + ( ( PppFile ) file )->BufferSize;
}


PDFFileHandle ULFileOpen(PDFLibHandle Lib, char *filename, ppFileOpenMode mode)
{
    PppFile tmp;
#ifdef WINDOWS_PLATFORM
    PVOID map = NULL;
    HANDLE hFile = NULL, hFileMapping;
    ppInt32 sz = 0 ;
#else
    FILE *f;
#endif
    if ( !filename )
        return NULL;
#ifndef WINDOWS_PLATFORM
    switch ( mode ){
        case ppFileReadMode:
            f = fopen ( filename, READMODE );
            break;
        case ppFileWriteMode:
            f = fopen ( filename, WRITEMODE );
            break;
    };
	if ( !f ){
		switch ( mode ){
			case ppFileWriteMode:
				PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotCreateFileError ) );
				break;
			case ppFileReadMode:
			default:
				PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotOpenFileError ) );
		}     
	}
    PDFTRY ( Lib ){
        tmp = mmalloc ( Lib, sizeof ( ppFile ) );
        tmp->Buffer = mmalloc ( Lib, FILE_BUFFER_SIZE );
    } PDFEXCEPT ( Lib ){
        chfreez ( Lib, tmp );
        fclose ( f );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    tmp->File = f;
    tmp->Mode = mode;
    tmp->Lib = Lib;
    tmp->BufferSize = 0;
    tmp->FileSize = 0;
    return( PDFFileHandle ) tmp;
#else
    switch ( mode ){
        case ppFileReadMode:
            hFile = CreateFile ( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0 );
            if ( hFile == INVALID_HANDLE_VALUE )
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotOpenFileError ) );
            sz = GetFileSize ( hFile, NULL );
            if ( sz == 0 ){
                CloseHandle ( hFile );
                PDFRAISE ( Lib,
                           PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFileSizeError ) );
            };
            hFileMapping = CreateFileMapping ( hFile, NULL, PAGE_READONLY, 0, sz, NULL );
            if ( !hFileMapping ){
                CloseHandle ( hFile );
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotCreateMapFileError ) );
            };
            CloseHandle ( hFile );
            map = MapViewOfFile ( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
            CloseHandle ( hFileMapping );
            if ( !map )
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotCreateMapFileError ) );
            break;
        case ppFileWriteMode:
            hFile = CreateFile ( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, 0 );
            if ( hFile == INVALID_HANDLE_VALUE )
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotCreateFileError ) );
        
            break;
    };
    tmp = ( PppFile ) mmalloc ( Lib, sizeof ( ppFile ) );
    if (mode == ppFileWriteMode){
        PDFTRY ( Lib ){
            tmp->Buffer = ( ppUns8 * ) mmalloc ( Lib, FILE_BUFFER_SIZE );
        }PDFEXCEPT ( Lib ){
            chfreez ( Lib, tmp );
            CloseHandle ( hFile );
            PDFRERAISE ( Lib );
        }
        PDFTRYEND ( Lib );
    }
    tmp->Mode = mode;
    tmp->Map = map;
    tmp->Size = sz;
    tmp->File = hFile;
    tmp->Position = 0;
    tmp->BufferSize = 0;
    tmp->FileSize = 0;
    tmp->Lib = Lib;
    return ( PDFFileHandle ) tmp;
#endif
}

ppUns32 ULFileWrite(PDFFileHandle file, void *buffer, ppUns32 len)
{
#define _F ((PppFile)file)
    ppInt32 j;
    if ( len == 0 )
        return 0;
    if ( _F->Mode == ppFileReadMode )
        return 0;
    if ( len > FILE_BUFFER_SIZE ){
        if ( _F->BufferSize )
            ULWriteFileBuffer ( file, _F->Buffer, _F->BufferSize );
        ULWriteFileBuffer ( file, buffer, len );
        _F->FileSize += _F->BufferSize + len;
        _F->BufferSize = 0;
        return len;
    }
    if ( len + _F->BufferSize < FILE_BUFFER_SIZE ){
        memcpy ( &( _F->Buffer[_F->BufferSize] ), buffer, len );
        _F->BufferSize += len;
        return len;
    }
    memcpy ( &( _F->Buffer[_F->BufferSize] ), buffer, j = FILE_BUFFER_SIZE - _F->BufferSize );
    ULWriteFileBuffer ( file, _F->Buffer, FILE_BUFFER_SIZE );
    _F->FileSize += FILE_BUFFER_SIZE;
	_F->BufferSize = len - j;
    if ( _F->BufferSize )
        memcpy ( _F->Buffer, &( ( ( ppUns8 * ) buffer )[j] ), _F->BufferSize );
    return len;
}


 
void ULFileClose(PDFFileHandle file)
{
    if ( ( ( PppFile ) file )->Mode != ppFileReadMode ){
        if ( ( ( PppFile ) file )->BufferSize )
            ULWriteFileBuffer ( file, ( ( PppFile ) file )->Buffer,
                                ( ( PppFile ) file )->BufferSize );
    }
#ifdef WINDOWS_PLATFORM
    if ( ( ( PppFile ) file )->Mode == ppFileReadMode )
        UnmapViewOfFile ( ( ( PppFile ) file )->Map );
    else{
		mfree ( ((PppFile)file)->Lib, ( ( PppFile ) file )->Buffer );
        CloseHandle ( ( ( PppFile ) file )->File );
    }
#else
    mfree ( ((PppFile)file)->Lib, ( ( PppFile ) file )->Buffer );    
    fclose ( ( ( PppFile ) file )->File );
#endif
    mfree ( ((PppFile)file)->Lib, file );
}


ppUns32 ULFileSetPosition(PDFFileHandle file, ppUns32 pos)
{
#ifdef WINDOWS_PLATFORM
    if ( ( ( PppFile ) file )->Mode == ppFileReadMode ){
        ( ( PppFile ) file )->Position = pos;
        return pos;
    } else
        return SetFilePointer ( ( ( PppFile ) file )->File, pos, NULL, FILE_BEGIN );
#else
    fseek ( ( ( PppFile ) file )->File, pos, SEEK_SET );
    return pos;
#endif
}

ppUns32 ULFileGetSize(PDFFileHandle file)
{
#ifdef WINDOWS_PLATFORM
    if ( ( ( PppFile ) file )->Mode == ppFileReadMode )
        return ( ( PppFile ) file )->Size;
    else
        return GetFileSize ( ( ( PppFile ) file )->File, 0 );
#else
    ppInt32 old, ret;
    old = (ppInt32)ftell ( ( ( PppFile ) file )->File );
    fseek ( ( ( PppFile ) file )->File, 0, SEEK_END );
    ret = (ppInt32)ftell ( ( ( PppFile ) file )->File );
    fseek ( ( ( PppFile ) file )->File, old, SEEK_SET );
    return ret;
#endif
}


ppUns32 ULFileRead(PDFFileHandle file, void *buffer, ppUns32 len)
{
#ifdef WINDOWS_PLATFORM
    char *b;
    ppInt32 k;
#endif
    if ( len == 0 )
        return 0;
    if ( ( ( PppFile ) file )->Mode != ppFileWriteMode )
    #ifdef WINDOWS_PLATFORM
    {
        b = ( char * )( ( PppFile ) file )->Map;
        b += ( ( PppFile ) file )->Position;
        if ( ( ( PppFile ) file )->Position + len > ( ( PppFile ) file )->Size )
            k = ( ( PppFile ) file )->Size - ( ( PppFile ) file )->Position;
        else
            k = len;
        if ( k <= 0 )
            return 0;
        memcpy ( buffer, b, k );
        ( ( PppFile ) file )->Position += k;
        return k;
    }
#else
    return ( ppInt32 ) fread ( buffer, 1, len, ( ( PppFile ) file )->File );
#endif
    else
        return 0;
}

ppInt32 ULFileGetChar(PDFFileHandle file)
{
#ifdef WINDOWS_PLATFORM
    unsigned char * b;
    ppInt32 v;
    if ( ( ( PppFile ) file )->Mode != ppFileReadMode )
        return EOF;
    if ( ( ( PppFile ) file )->Size > ( ( PppFile ) file )->Position ){
        b = ( unsigned char * ) ( ( PppFile ) file )->Map;
        b += ( ( PppFile ) file )->Position;
        v = ( ppInt32 ) ( *b );
        ( ( PppFile ) file )->Position++;
        return v;
    } else
        return EOF;
#else
    return fgetc ( ( ( PppFile ) file )->File );
#endif
}

ppInt32 ULFileLookChar(PDFFileHandle file)
{
#ifdef WINDOWS_PLATFORM
    unsigned char * b;
    ppInt32 v;
    if ( ( ( PppFile ) file )->Mode != ppFileReadMode )
        return EOF;
    if ( ( ( PppFile ) file )->Size > ( ( PppFile ) file )->Position ){
        b = ( unsigned char * )( ( PppFile ) file )->Map;
        b += ( ( PppFile ) file )->Position;
        v = ( ppInt32 ) ( *b );
        return v;
    } else
        return EOF;
#else
    ppInt32 pos, r;
    pos = ULFileGetPosition ( file );
    r = ULFileGetChar ( file );
    ULFileSetPosition ( file, pos );
    return r;
#endif
}


/*Stream Section */

#define __Stream(Strm) ((PULStrm) Strm )

#define _Stream __Stream(Strm)

void * ULStrmReAlloc(PULStrm pms, ppUns32 *NewCapacity)
{
    void *t;
    if ( *NewCapacity > 0 && *NewCapacity != pms->Sections.Memory.Size )
        *NewCapacity = *NewCapacity + ( ( MemoryDelta - 1 ) & ( ~( MemoryDelta - 1 ) ) );
    t = pms->Sections.Memory.Buffer;
    if ( *NewCapacity != pms->Sections.Memory.Capacity ){
        if ( *NewCapacity == 0 ){
            mfree ( pms->Lib, pms->Sections.Memory.Buffer );
            t = NULL;
        } else if ( pms->Sections.Memory.Capacity == 0 )
            t = mmalloc ( pms->Lib, *NewCapacity );
        else
            t = mrealloc ( pms->Lib, pms->Sections.Memory.Buffer, *NewCapacity );
    }
    return t;
}



ppInt32 ULStrmSetCapacity(PDFStreamHandle Strm, ppUns32 NewCapacity)
{
    if ( _Stream->Type != strtMemory || _Stream->RO )
        return 0;
    _Stream->Sections.Memory.Buffer = ULStrmReAlloc (_Stream, &NewCapacity );
    _Stream->Sections.Memory.Capacity = NewCapacity;
    return NewCapacity;
}


void ULStreamClear(PDFStreamHandle Strm, ppUns32 InitSize)
{
    if ( _Stream->Type != strtMemory )
        return ;
    if ( !InitSize ){
        ULStrmSetCapacity ( Strm, 0 );
        _Stream->Sections.Memory.Buffer = NULL;
    } else
        ULStrmSetCapacity ( Strm, InitSize );
    _Stream->Sections.Memory.Size = 0;
    _Stream->Sections.Memory.Position = 0;
}


PDFStreamHandle ULStreamFileNew(PDFLibHandle Lib, char *filename, ppFileOpenMode mode)
{
    PDFFileHandle f;
    PULStrm wrk = NULL;
    f = ULFileOpen ( Lib, filename, mode );
    PDFTRY ( Lib ){
        wrk = ( PULStrm ) mmalloc ( Lib, sizeof ( TULStrm ) );
    } PDFEXCEPT ( Lib ){
        ULFileClose ( f );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    wrk->RO = ( ppBool )( mode == ppFileReadMode );
    wrk->Lib = Lib;
    wrk->Sections.File.HandleCreated = true;
    wrk->Type = strtFile;
    wrk->Sections.File.FileHandle = f;
    return ( PDFStreamHandle ) wrk;
}

PDFStreamHandle ULStreamFileHandleNew(PDFLibHandle Lib, PDFFileHandle file)
{
    PULStrm wrk;
    wrk = ( PULStrm ) mmalloc ( Lib, sizeof ( TULStrm ) );
    wrk->RO = ( ppBool ) ( ( ( PppFile ) file )->Mode == ppFileReadMode );
    wrk->Lib = Lib;
    wrk->Sections.File.HandleCreated = false;
    wrk->Type = strtFile;
    wrk->Sections.File.FileHandle = file;
    return ( PDFStreamHandle ) wrk;
}

PDFStreamHandle ULStreamMemNew(PDFLibHandle Lib, ppUns32 newsize)
{
    PULStrm wrk;
    wrk = ( PULStrm ) mmalloc ( Lib, sizeof ( TULStrm ) );
    wrk->RO = false;
    wrk->Lib = Lib;
    wrk->Type = strtMemory;
    wrk->Sections.Memory.Capacity = 0;
    wrk->Sections.Memory.Size = 0;
    wrk->Sections.Memory.Position = 0;
    wrk->Sections.Memory.Buffer = NULL;
    if ( newsize != 0 )
        ULStrmSetCapacity ( wrk, newsize );
    return ( PDFStreamHandle ) wrk;
}

PDFStreamHandle ULStreamCustomNew ( PDFLibHandle Lib, StreamReadBufferProc ReadBuffer, StreamWriteBufferProc WriteBuffer,
									StreamGetPositionProc GetPosition, StreamSetPositionProc SetPosition, StreamGetSizeProc GetSize,
									StreamGetCharProc GetChar, StreamLookCharProc LookChar, void * AStream )
{
	PULStrm wrk;
	wrk = ( PULStrm ) mmalloc ( Lib, sizeof ( TULStrm ) );
	wrk->RO = false;
	wrk->Lib = Lib;
	wrk->Type = strtCustom;
	wrk->Sections.Custom.AStream = AStream;
	wrk->Sections.Custom.ReadBuffer = ReadBuffer;
	wrk->Sections.Custom.WriteBuffer = WriteBuffer;
	wrk->Sections.Custom.GetPosition = GetPosition;
	wrk->Sections.Custom.GetSize = GetSize;
	wrk->Sections.Custom.SetPosition = SetPosition;
	wrk->Sections.Custom.GetChar = GetChar;
	wrk->Sections.Custom.LookChar = LookChar;
	return ( PDFStreamHandle ) wrk;
}

PDFStreamHandle ULStreamMemReadOnlyNew(PDFLibHandle Lib, void *buf, ppUns32 len)
{
    PULStrm wrk;
    wrk = ( PULStrm ) mmalloc ( Lib, sizeof ( TULStrm ) );
    wrk->Sections.Memory.Buffer = buf;
    wrk->Sections.Memory.Capacity = len;
    wrk->Lib = Lib;
    wrk->RO = true;
    wrk->Sections.Memory.Position = 0;
    wrk->Sections.Memory.Size = len;
    wrk->Type = strtMemory;
    return ( PDFStreamHandle ) wrk;
}

void ULStreamClose(PDFStreamHandle Strm)
{
    ULStrmSetCapacity ( Strm, 0 );
    if ( _Stream->Type == strtFile && _Stream->Sections.File.HandleCreated )
        ULFileClose ( _Stream->Sections.File.FileHandle);
	mfree ( _Stream->Lib,  Strm );
}

ppUns32 ULStreamSetSize(PDFStreamHandle Strm, ppUns32 size)
{
    ppUns32 OldPos;
    if ( _Stream->Type != strtMemory || _Stream->RO )
        return 0;
    OldPos =  _Stream->Sections.Memory.Position;
    ULStrmSetCapacity ( Strm, size );
    _Stream->Sections.Memory.Size = size;
    if ( OldPos > size )
        _Stream->Sections.Memory.Position = _Stream->Sections.Memory.Size;
    return size;
}

ppUns32 ULStreamSetPosition(PDFStreamHandle Strm, ppUns32 newposition)
{
    switch ( _Stream->Type ){
        case strtMemory:
            _Stream->Sections.Memory.Position = newposition;
            return newposition;
        case strtFile:
            return ULFileSetPosition ( _Stream->Sections.File.FileHandle, newposition );
		case strtCustom:
			return _Stream->Sections.Custom.SetPosition ( _Stream->Lib, _Stream->Sections.Custom.AStream, newposition );
    } ;
    return 0;
}

ppUns32 ULStreamGetSize(PDFStreamHandle Strm)
{
    switch ( _Stream->Type ){
        case strtMemory:
            return _Stream->Sections.Memory.Size;
        case strtFile:
            return ULFileGetSize ( _Stream->Sections.File.FileHandle );
		case strtCustom:
			return _Stream ->Sections.Custom.GetSize ( _Stream->Lib, _Stream->Sections.Custom.AStream );
    } ;
    return 0;
}

ppUns32 ULStreamGetPosition(PDFStreamHandle Strm)
{
    switch ( _Stream->Type ){
        case strtMemory:
            return _Stream->Sections.Memory.Position;
        case strtFile:
            return ULFileGetPosition ( _Stream->Sections.File.FileHandle );
		case strtCustom :
			return _Stream->Sections.Custom.GetPosition (_Stream->Lib,  _Stream->Sections.Custom.AStream );
    } ;
    return 0;
}
 
ppInt32 ULStreamReadChar(PDFStreamHandle Strm)
{
    unsigned char * k, r;
    switch ( _Stream->Type ){
        case strtMemory:
            if ( _Stream->Sections.Memory.Position >= _Stream->Sections.Memory.Size )
                return EOF;
            k = ( unsigned char * )_Stream->Sections.Memory.Buffer;
            k += _Stream->Sections.Memory.Position;
            r = *k;
            _Stream->Sections.Memory.Position++;
            return r;
        case strtFile:
            return ULFileGetChar ( _Stream->Sections.File.FileHandle );
		case strtCustom:
			return _Stream->Sections.Custom.GetChar (_Stream->Lib,  _Stream->Sections.Custom.AStream );
    } ;
    return 0;
}

ppInt32 ULStreamLookChar(PDFStreamHandle Strm)
{
    unsigned char * k, r;
    switch ( _Stream->Type ){
        case strtMemory:
            if ( _Stream->Sections.Memory.Position >= _Stream->Sections.Memory.Size )
                return EOF;
            k = ( unsigned char * ) _Stream->Sections.Memory.Buffer;
            k += _Stream->Sections.Memory.Position;
            r = *k;
            return r;
        case strtFile:
            return ULFileLookChar ( _Stream->Sections.File.FileHandle );
		case strtCustom:
			return _Stream->Sections.Custom.LookChar ( _Stream->Lib, _Stream->Sections.Custom.AStream );
    } ;
    return 0;
}

char * ULStreamReadLine(PDFStreamHandle Strm)
{
    ppInt32 i, sz, pos;
    ppUns32 a;
    char *st;
    sz = ULStreamGetSize ( Strm );
    pos = ULStreamGetPosition ( Strm );
    for ( i = 0; i + pos < sz && ( a = ULStreamReadChar ( Strm ) ) != '\r' && a != '\n'; i++ ){
    };
    st = ( char * ) mmalloc ( _Stream->Lib, i + 1 );
    ULStreamSetPosition ( Strm, pos );
    if ( i > 0 )
        ULStreamReadBuffer ( Strm, st, i );
    st[i] = '\0';
    if ( i + pos != sz ){
        a = ULStreamReadChar ( Strm );
        if ( a == '\r' )
            if ( 1 + i + pos < sz && ( ppUns32 ) ULStreamLookChar ( Strm ) == '\n' )
                ULStreamReadChar ( Strm );
    };
    return st;
}

ppUns32 ULStreamReadBuffer(PDFStreamHandle Strm, void *Buffer, ppUns32 Count)
{
    ppUns32 rr;
    char *k;
    switch ( _Stream->Type ){
        case strtMemory:
                rr = _Stream->Sections.Memory.Size - _Stream->Sections.Memory.Position;
                if ( rr > 0 ){
                    if ( rr > Count )
                        rr = Count;
                    k = ( char * ) _Stream->Sections.Memory.Buffer;
                    k += _Stream->Sections.Memory.Position;
                    memcpy ( Buffer, k, rr );
                    _Stream->Sections.Memory.Position += rr;
                    return rr;
                };
            return 0;
        case strtFile:
            return ULFileRead ( _Stream->Sections.File.FileHandle, Buffer, Count );
		case strtCustom:
			return _Stream->Sections.Custom.ReadBuffer ( _Stream->Lib, _Stream->Sections.Custom.AStream, Buffer, Count );
    } ;
    return 0;
}

ppUns32 ULStreamWriteBuffer(PDFStreamHandle Strm, void *Buffer, ppUns32 Count)
{
    ppUns32 pos;
    char *k;
    if ( _Stream->RO )
        return 0;
    switch ( _Stream->Type ){
        case strtMemory:
                pos = _Stream->Sections.Memory.Position + Count;
                if ( pos > 0 ){
                    if ( pos > _Stream->Sections.Memory.Size ){
                        if ( pos > _Stream->Sections.Memory.Capacity )
                            ULStrmSetCapacity ( Strm, pos );
                        _Stream->Sections.Memory.Size = pos;
                    };
                    k = ( char * ) _Stream->Sections.Memory.Buffer;
                    k += _Stream->Sections.Memory.Position;
                    memcpy ( k, Buffer, Count );
                    _Stream->Sections.Memory.Position = pos;
                    return Count;
                };
            return _Stream->Sections.Memory.Position;
        case strtFile:
            return ULFileWrite ( _Stream->Sections.File.FileHandle, Buffer, Count );
		case strtCustom:
			return _Stream->Sections.Custom.WriteBuffer ( _Stream->Lib, _Stream->Sections.Custom.AStream, Buffer, Count );
    } ;
    return 0;
}

ppUns32 ULStreamWriteChar(PDFStreamHandle Strm, char ch)
{
    return ULStreamWriteBuffer ( Strm, &ch, 1 );
}

ppInt32 ULStrmReset(PDFStreamHandle Strm)
{
    switch ( _Stream->Type ){
        case strtFile:
            ULFileSetPosition ( _Stream->Sections.File.FileHandle, 0 );
            break;
        case strtMemory:
            _Stream->Sections.Memory.Position = 0;
            break;
		case strtCustom:
			_Stream->Sections.Custom.SetPosition ( _Stream->Lib, _Stream->Sections.Custom.AStream, 0 );
        default:
    ;
    }
    return 0;
}

ppUns32 ULStreamCopyToStream(PDFStreamHandle FromStrm, PDFStreamHandle ToStrm)
{
    void *bf;
    char *b;
    ppInt32 sz, pos, i;
    switch ( __Stream (FromStrm )->Type ){
        case strtMemory:
            return ULStreamWriteBuffer ( ToStrm, __Stream (FromStrm )->Sections.Memory.Buffer,
                                       __Stream (FromStrm )->Sections.Memory.Size );
        case strtFile:
            pos = ULFileGetPosition ( __Stream (FromStrm )->Sections.File.FileHandle );
            ULFileSetPosition ( __Stream (FromStrm )->Sections.File.FileHandle, 0 );
            sz = ULFileGetSize ( __Stream (FromStrm )->Sections.File.FileHandle );
            if ( __Stream (ToStrm )->Type == strtMemory ){
                if ( __Stream (ToStrm )->RO )
                    return 0;
                else{
                    if ( __Stream (ToStrm )->Sections.Memory.Size < __Stream (ToStrm )->Sections.Memory.Position + sz )
                        ULStreamSetSize ( ToStrm, __Stream (ToStrm )->Sections.Memory.Position + sz );
                    b = ( char * ) __Stream (ToStrm )->Sections.Memory.Buffer;
                    b += __Stream (ToStrm )->Sections.Memory.Position;
                    i = ULFileRead ( __Stream (FromStrm )->Sections.File.FileHandle, b, sz );
                    ULFileSetPosition ( __Stream (FromStrm )->Sections.File.FileHandle, pos );
                    return i;
                };
            } else{
                bf = mmalloc ( __Stream (FromStrm )->Lib, sz );
                ULFileRead ( __Stream (FromStrm )->Sections.File.FileHandle, bf, sz );
                ULFileSetPosition ( __Stream (FromStrm )->Sections.File.FileHandle, pos );
                pos = ULStreamWriteBuffer ( ToStrm, bf, sz );
				mfree ( __Stream (FromStrm )->Lib,  bf );
                return pos;
            };
		case strtCustom:
			pos = __Stream( FromStrm )->Sections.Custom.GetPosition ( __Stream (FromStrm )->Lib,  __Stream (FromStrm )->Sections.Custom.AStream );
			__Stream( FromStrm )->Sections.Custom.SetPosition ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream, 0 );
			sz = __Stream ( FromStrm )->Sections.Custom.GetSize ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream );
			if ( __Stream (ToStrm )->Type == strtMemory ){
				if ( __Stream (ToStrm )->RO )
					return 0;
				else{
					if ( __Stream (ToStrm )->Sections.Memory.Size < __Stream (ToStrm )->Sections.Memory.Position + sz )
						ULStreamSetSize ( ToStrm, __Stream (ToStrm )->Sections.Memory.Position + sz );
					b = ( char * ) __Stream (ToStrm )->Sections.Memory.Buffer;
					b += __Stream (ToStrm )->Sections.Memory.Position;
					i = __Stream ( FromStrm )->Sections.Custom.ReadBuffer ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream, b, sz );
					__Stream( FromStrm )->Sections.Custom.SetPosition ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream, pos );
					return i;
				};
			} else{
				bf = mmalloc ( __Stream (FromStrm )->Lib, sz );
				__Stream ( FromStrm )->Sections.Custom.ReadBuffer ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream, bf, sz );
				__Stream( FromStrm )->Sections.Custom.SetPosition ( __Stream (FromStrm )->Lib, __Stream (FromStrm )->Sections.Custom.AStream, pos );
				pos = ULStreamWriteBuffer ( ToStrm, bf, sz );
				mfree ( __Stream (FromStrm )->Lib,  bf );
				return pos;
			};
    } ;
    return 0;
}
