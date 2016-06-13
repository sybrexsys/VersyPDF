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
VSMisc.h
*********************************************************************/

#ifndef VSMisc_H
#define VSMisc_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSLibI.h"

#ifdef WINDOWS_PLATFORM
#include <Windows.h>
#include <WinGDI.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif


#define CalculateDelta(SZ) ((SZ)>64?(SZ)>>2:(SZ)>8?16:4)

#define ULBitStrmInit(OutStrm, BST) {BST.Bit=8; BST.Data=0; BST.Strm=OutStrm; }
#define ULBitStreamFlush(BST) {ULStreamWriteChar(BST.Strm,BST.Data); BST.Data=0; BST.Bit=8;}
#define ULBitStreamFlushWithCheck(BST) {if (BST.Bit!=8) {ULStreamWriteChar(BST.Strm,BST.Data);BST.Data=0; BST.Bit=8;}}
#define ULBitStrmDone(BST){ if (BST.Bit!=8) ULBitStreamFlush(BST);}
#ifndef log2
#define log2(A) (log10(A)/log10( 2.0 ) )
#endif

#ifndef Pi
#define Pi       3.14159265358979323846
#endif

#define _abs(a) (a<0)?(-a):(a)

 

#define EscToStrm(Strm,a) {if (!a)  ULStrToStrm(Strm,"\\000"); \
    else if (a=='(') ULStrToStrm(Strm,"\\("); \
    else if (a==')') ULStrToStrm(Strm,"\\)"); \
    else if (a=='\\') ULStrToStrm(Strm,"\\\\"); \
    else if (a==13) ULStrToStrm(Strm,"\\\r"); \
    else if (a==10) ULStrToStrm(Strm,"\\\n"); \
    else ULStreamWriteChar(Strm,a);}

#define freez(A, x) {PDFIntFree(A, x);x=NULL;}
#define chfreez(A, x) if (x) freez(A, x);
#define chfree(A, x) if (x) mfree(A, x);

    /* Some misc decrement size of the written code */
#define _RAISE(Lev,Err) PDFRAISE ( _LIB, PDFBuildErrCode ( Lev, Err ) )
#define __RAISE(Lev,Err) PDFRAISE ( _LLIB, PDFBuildErrCode ( Lev, Err ) )
#define _LRAISE(Lib, Lev,Err) PDFRAISE ( Lib, PDFBuildErrCode ( Lev, Err ) )

    typedef struct ULBitStream {
        PDFStreamHandle   Strm;
        ppUns8      Data;
        ppUns8      Bit;
    } ULBitStream;

#ifndef WINDOWS_PLATFORM
	
	typedef struct  _t_XFORM
	{
		float   eM11;
		float   eM12;
		float   eM21;
		float   eM22;
		float   eDx;
		float   eDy;
	} XFORM, *PXFORM;

#endif

    TPDFRect PDFRect( ppReal xl, ppReal yl, ppReal xr, ppReal yr );
    XFORM PDFMatrix( ppReal eM11, ppReal eM12, ppReal eM21, ppReal eM22, ppReal eDx, ppReal eDy );
    void ULBitStreamPutBits(ULBitStream *BST, ppInt32 bits, ppInt32 length);

    
#ifndef MACOSX_PLATFORM
	void ULSwapLong(void *lp);
	void ULSwapShort(void *wp);
#	ifndef PDFBIGENDIAN   
#		define BSWAPL( x ) ULSwapLong ( &x )
#		define BSWAPS( x ) ULSwapShort ( &x )
#		define SSWAPL( x )
#		define SSWAPS( x )
#	else
#		define BSWAPL( x )
#		define BSWAPS( x )
#		define SSWAPL( x ) ULSwapLong ( &x )
#		define SSWAPS( x ) ULSwapShort ( &x )
#	endif
#else
	void MACULSwapLong ( void *lp, ppBool NeedBig);
	void MACULSwapShort ( void *lp, ppBool NeedBig);
#	define BSWAPL( x ) MACULSwapLong ( &x, true )
#	define BSWAPS( x ) MACULSwapShort ( &x, true )
#	define SSWAPL( x ) MACULSwapLong ( &x, false )
#	define SSWAPS( x ) MACULSwapShort ( &x, false )
#endif

    ppInt32 ULStrToStrm(PDFStreamHandle Strm, char *str);
    ppInt32 ULIntToStrm(PDFStreamHandle Strm, ppInt32 val);
    ppInt32 ULRealToStrm(PDFStreamHandle Strm, ppReal val);
    ppInt32 ULAtomToStrm(PDFLibHandle Lib, PDFStreamHandle Strm, ppAtom atom);
    ppInt32 ULHexUns8ToStrm(PDFStreamHandle Strm, ppUns8 val);
    ppInt32 ULHexUns16ToStrm(PDFStreamHandle Strm, ppUns16 val);
    ppUns16 ULANSIToUnicode(ppUns8 ansi_code);
    ppUns8 ULUnicodeToANSI(ppUns16 unicode);
    char *ULitostr(int n, char *str);
    ppInt32 ULGetBitsPerInt(ppInt32 i);
    ppInt32 ULGetIntSize(ppInt32 val);
    ppInt32 ULGetIntByte(ppInt32 val);
    ppInt32 ULGetRealSize(ppReal val);
    ppInt32 ULGetAtomSize(PDFLibHandle Lib, ppAtom atom);
    ppInt32 ULGetStringSize(char *val, ppInt32 len);
    ppInt32 ULGetNameSize(char *val);
    ppBool ULIsNotANSICode(ppUns16 Code);

	XFORM ULMultiplyXFORM(XFORM A, XFORM B);

    ppInt32 ULmemfndsubset(void *buf, void *subs, ppInt32 buflen, ppInt32 subslen);
    char *ULIntToStrWithZero(PDFLibHandle Lib, ppInt32 val, int sz);
    char *ULGetEscString(PDFLibHandle Lib, char *val, ppInt32 len);
    char *ULGetEscName(PDFLibHandle Lib, char *val);
    char *ULTrimStr(char *str);
    ppInt32 ULGetBitsPerInt(ppInt32 i);

    PPDFList ULListCreate(PDFLibHandle Lib, ppUns32 InitSize);
    void ULListFree(PPDFList list);
    void ULListDelete(PPDFList list, ppUns32 index);
    ppUns32 ULListAdd(PPDFList list, PDFHandle item);
    void ULListExchange(PPDFList list, ppUns32 First, ppUns32 Second);
    ppUns32 ULListIndexOf(PPDFList list, PDFHandle Item);
    ppUns32 ULListInsert(PPDFList list, PDFHandle Item, ppUns32 index);
    void ULListRemove(PPDFList list, PDFHandle Item);
    void ULListClear(PPDFList list);
    void ULListSetCapacity(PPDFList list, ppUns32 newcapacity);

	void ULSetNewCryptoData( PDFDocHandle Doc, TPDFCryptoType * Cryptodata);
	PDFCosHandle ULPrepareCryptoDictionary( PDFDocHandle Doc, TPDFCryptoType *Cryptodata);


#define ULExtListItem( List, Type, Index ) ( ( Type ) List->FList ) [Index] 

    PPDFExtList ULExtListCreate ( PDFLibHandle Lib, ppUns32 EntrySize, ppUns32 InitSize );
    void ULExtListFree ( PPDFExtList List );
    ppUns32 ULExtListAdd ( PPDFExtList List, void *Entry );
    ppUns32 ULExtListIndexOf ( PPDFExtList List, void *Entry );
    ppUns32 ULExtListInsert ( PPDFExtList List, void *Entry, ppUns32 Index );
    void ULExtListClear ( PPDFExtList List );


    void *ULStrmGetMem(PDFStreamHandle Strm);

    ppInt32 ULStrmWriteInt(PDFStreamHandle Strm, ppInt32 i, ppInt32 c);

    ppInt32 ULModFindOrAppendAtom(PDFLibHandle Lib, char *str);
    void ULClearAtomNode( PDFLibHandle Lib, PppAtomNode node);
    void ULFindAtom(PDFLibHandle Lib, char *str, PppAtomNode node);
    void *ULStrmReAlloc(PULStrm pms, ppUns32 *NewCapacity);
    ppInt32 ULStrmSetCapacity(PDFStreamHandle Strm, ppUns32 NewCapacity);
    ppInt32 ULStrmReset(PDFStreamHandle Strm);
    void ULListGrow(PPDFList list);
    void ULListSetCount(PPDFList list, ppUns32 newcount);

    PDFCosHandle ULGetDictFromNameTree(PDFDocHandle Doc, PDFCosHandle NameTree, char *str, ppUns32 l);
    void ULSetObjToNameTree(PDFDocHandle Doc, PDFCosHandle NameTree, PDFCosHandle InsObj, char *str,
                            ppUns32 l);

    ppAtom ULGetResourceDictElem(PDFDocHandle Doc, PDFCosHandle Res, char *Pref, ppInt32 *Acc,
                                 PDFCosHandle Search);

#ifdef WINDOWS_PLATFORM
    ppUns8 ULGetDefFontCharSet(void);
    ppInt32 ULGetCodePageByCharset(ppUns8 Charset);
#endif


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSMisc_H */
