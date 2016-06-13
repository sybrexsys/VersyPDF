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
VSThreadA.c
*********************************************************************/

#include "../VSThreadA.h"
#include "VSCosAI.h"
#include "VSDocAI.h"
#include "VSAssert.h" 
#include "VSMisc.h"
#include <string.h>

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

#define _DDOC CD(_CosDoc(Thread))
#define _LLIB _DDOC->Lib


ppInt32 PDFDocGetThreadCount(PDFDocHandle Doc)
{
    PDFCosHandle list;
    list = _CosDictValueByName ( _DOC->Root, Threads );
    if ( _CosGetType ( list ) != CosArray )
        return 0;
    return _CosArrayCount ( list );
}

PDFThreadHandle PDFDocGetThread(PDFDocHandle Doc, ppInt32 Index)
{
    PDFCosHandle list, ddd;
    list = _CosDictValueByName ( _DOC->Root, Threads );
    if ( _CosGetType ( list ) != CosArray )
        return CosNullNew(Doc);
    ddd = CosArrayItem ( list,  Index );
    if ( _CosIndirectDict ( ddd ) )
        return ddd;
    return CosNullNew( Doc );
}

PDFThreadHandle PDFThreadNew(PDFDocHandle Doc)
{
    PDFCosHandle list, ddd;
	CheckEdition(_LIB, STD);
    list = _CosDictValueByName ( _DOC->Root, Threads );
    if ( _CosGetType ( list ) != CosArray ){
        list = CosArrayNew ( Doc,  false, 1 );
        _CosDictAppend ( _DOC->Root, Threads, list );
    }
    ddd = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( ddd, Type, _CosNameNew ( Thread ) );
    _CosDictAppend ( ddd, I, CosDictNew ( Doc, false, 5 ) );
    CosArrayAppend ( list, ddd );
    return ddd;
}

void PDFThreadDelete(PDFThreadHandle Thread)
{
    PDFCosHandle f, rf, P, B;
    ppInt32 i;
    PDFID ID;
    f = __CosDictValueByName ( Thread, F );
	if ( _IsCosNull(f) ){
		ID = _CosObjFullID ( f );
		PDFDocDeleteObj ( _DDOC, _CosObjID ( Thread ) );
        return;
	}
	if ( !_CosIndirectDict ( f ) ){
		ID = _CosObjFullID ( f );
		PDFDocDeleteObj ( _DDOC, _CosObjID ( Thread ) );
		return;
	}      
    ID = _CosObjFullID ( f );
    PDFDocDeleteObj ( _DDOC, _CosObjID ( Thread ) );
    while ( 1 ){
		P = __CosDictValueByName ( f, P );
		if ( _CosIndirectDict ( P ) ){
			B = __CosDictValueByName ( P, B );
			if ( _IsCosArray ( B ) )
				for ( i = _CosArrayCount ( B ) - 1; i >= 0; i-- ){
					P = CosArrayItem ( B, i );
					if ( CO(P) == CO(f) )
						CosArrayRemove ( B, i );
				}
		}
        rf = __CosDictValueByName ( f, N );
		PDFDocDeleteObj ( _DDOC, _CosObjID ( f ) );
		if ( !_CosIndirectDict ( rf ) )
			break;
		f = rf;
    }
}


PDFBeadHandle PDFThreadGetFirstBead(PDFThreadHandle Thread)
{
    PDFCosHandle out;
    out = __CosDictValueByName ( Thread, F );
    if ( !_CosIndirectDict ( out ) )
        out = CosNullNew ( _DDOC );
    return out;
}

char * PDFThreadGetInfo( PDFThreadHandle Thread, TPDFInformation Info, ppUns32 *StrLen )
{
	PDFCosHandle obj,obj1;
	ppAtom A = ppAtomNull;
	obj=__CosDictValueByName ( Thread, I );
	if (!_IsCosDict(Thread) ) {
		*StrLen = 0;
		return NULL;
	}
	switch(Info) {
		case piProducer:
			if (((PPDFLib)_LLIB)->ADM) 
				A = GetAtomDDoc ( Producer );
			break;
		case piAuthor:
			A = GetAtomDDoc ( Author );
			break;
		case piDate:
			A = GetAtomDDoc ( CreationDate );
			break;
		case piTitle:
			A = GetAtomDDoc ( Title );
			break;
		case piCreator:
			A = GetAtomDDoc ( Creator );
			break;
		case piKeyWords:
			A = GetAtomDDoc ( Keywords );
			break;
		case piSubject:
			A = GetAtomDDoc ( Subject );
			break;
		case piModificationData:
			A = GetAtomDDoc ( ModDate );
			break;
		default:;
	};
	obj1 = CosDictValueByName ( obj, A );
	if ( !_IsCosString ( obj1 )){
		*StrLen = 0;
		return NULL;
	}
	return CosStringGetValue( obj1, StrLen );
}

void PDFThreadSetInfo( PDFThreadHandle Thread, TPDFInformation Info,
					  char *Value, ppUns32 Length )
{
    PDFCosHandle i;
	ppAtom A = ppAtomNull;
    i = __CosDictValueByName ( Thread, I );
    if ( !_IsCosDict ( i ) )
        __CosDictAppend ( Thread, I, i = CosDictNew ( _DDOC, false, 5 ) );
	switch(Info) {
		case piProducer:
				A = GetAtomDDoc ( Producer );
			break;
		case piAuthor:
			A = GetAtomDDoc ( Author );
			break;
		case piDate:
			A = GetAtomDDoc ( CreationDate );
			break;
		case piTitle:
			A = GetAtomDDoc ( Title );
			break;
		case piCreator:
			A = GetAtomDDoc ( Creator );
			break;
		case piKeyWords:
			A = GetAtomDDoc ( Keywords );
			break;
		case piSubject:
			A = GetAtomDDoc ( Subject );
			break;
		case piModificationData:
			A = GetAtomDDoc ( ModDate );
			break;
		default:;
	};
	if ( A != ppAtomNull){
		CosDictAppend( i, A, CosStringNew( _DDOC, false, Value, Length ) );
	}
}

PDFBeadHandle PDFThreadAppendBead( PDFThreadHandle Thread, ppUns32 Page, TPDFRect Rect )
{
	PDFCosHandle nw, B, a;
	PDFCosHandle out;
	if ( !_DDOC->Pages )
		PDFDocLoadPages ( _DDOC );
	if ( Page >= _DDOC->Pages->Size )
		__RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	nw = CosDictNew ( _DDOC, true, 5 );
	__CosDictAppend ( nw, P, PPGO ( Page ) );
	__CosDictAppend ( nw, T, Thread );
	__CosDictAppend ( nw, R, a = CosArrayNew ( _DDOC, false, 4 ) );
	CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.xl ) );
	CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.yl ) );
	CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.xr ) );
	CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.yr ) );
	B = __CosDictValueByName ( PPGO ( Page ), B );
	if ( !_IsCosArray ( B ) )
		__CosDictAppend ( PPGO ( Page ), B, B = CosArrayNew ( _DDOC, false, 5 ) );
	CosArrayAppend ( B, nw );
	out = __CosDictValueByName ( Thread, F );
	if ( !_CosIndirectDict ( out ) ){
		__CosDictAppend ( Thread, F, nw );
		__CosDictAppend ( nw, N, nw );
		__CosDictAppend ( nw, V, nw );
		return nw;
	}
	a = __CosDictValueByName ( out, V );
	if ( !_CosIndirectDict ( a ) ){
		__CosDictAppend ( Thread, F, nw );
		__CosDictAppend ( nw, N, nw );
		__CosDictAppend ( nw, V, nw );
		return nw;
	}
	__CosDictAppend ( a, N, nw );
	__CosDictAppend ( nw, V, a );
	__CosDictAppend ( out, V, nw );
	__CosDictAppend ( nw, N, out );	
	return nw;
}
#ifdef DELPHI
PDFBeadHandle PDFThreadAppendBeadM(PDFThreadHandle Thread, ppUns32 Page, TPDFRect *Rect){
	return  PDFThreadAppendBead( Thread, Page, *Rect);
}
#endif
#undef _DDOC
#undef _LLIB

#define _DDOC CD(_CosDoc(Bead))
#define _LLIB _DDOC->Lib



PDFBeadHandle PDFBeadGetNext(PDFBeadHandle Bead)
{
    PDFCosHandle out;
    out = __CosDictValueByName ( Bead, N );
    if ( !_CosIndirectDict ( out ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    return out;
}

PDFBeadHandle PDFBeadGetPrev(PDFBeadHandle Bead)
{
    PDFCosHandle out;
    out = __CosDictValueByName ( Bead, V );
    if ( !_CosIndirectDict ( out ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    return out;
}

void PDFBeadGetRect(PDFBeadHandle Bead, TPDFRect *Rect)
{
    PDFCosHandle a, l1, l2, l3, l4;
    a = __CosDictValueByName ( Bead, R );
    if ( !_IsCosArray ( a ) )
        __RAISE ( ErrDocumentLevel, dleCannotFindBeadRectError );
    if ( _CosArrayCount ( a ) != 4 )
        __RAISE ( ErrDocumentLevel, dleCannotFindBeadRectError );
    l1 = CosArrayItem ( a, 0 );
    l2 = CosArrayItem ( a, 1 );
    l3 = CosArrayItem ( a, 2 );
    l4 = CosArrayItem ( a, 3 );
    if ( !( _CosIsNumber ( l1 ) &&  _CosIsNumber ( l2 ) &&
            _CosIsNumber ( l3 ) &&  _CosIsNumber ( l4 ) ) )
        __RAISE ( ErrDocumentLevel, dleCannotFindBeadRectError );
    Rect->xl = CosNumberGetValue ( l1 );
    Rect->yl = CosNumberGetValue ( l2 );
    Rect->xr = CosNumberGetValue ( l3 );
    Rect->yr = CosNumberGetValue ( l4 );
}

ppInt32 PDFBeadGetPage( PDFBeadHandle Bead)
{
    ppUns32 i;
    PDFCosHandle pg;
    if ( !_DDOC->Pages )
        PDFDocLoadPages ( _DDOC );
    pg = __CosDictValueByName ( Bead, P );
    if ( !_CosIndirectDict ( pg ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    for ( i = 0; i < _DDOC->Pages->Size; i++ )
        if ( CO(pg) == CO(( ( PPDFPageInfo ) _DDOC->Pages->FList[i] )->PO ) )
            return i;
    return -1;
}

PDFThreadHandle PDFBeadGetThread(PDFBeadHandle Bead)
{
    PDFCosHandle wrk, pr, o;
    wrk = __CosDictValueByName ( Bead, T );
    o = Bead;
    while ( !_CosIndirectDict ( wrk ) ){
        pr = __CosDictValueByName ( o, V );
        if ( !_CosIndirectDict ( pr ) )
            __RAISE ( ErrCosLevel, cleInvalidObjError );
        if ( CO(pr) == CO(Bead ))
            __RAISE ( ErrDocumentLevel, dleCannotFindThreadForBeadError );
        o = pr;
        wrk = __CosDictValueByName ( o, T );
    };
    if ( !_CosIndirectDict ( wrk ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    return wrk;
}

void PDFBeadSetRect(PDFBeadHandle Bead, TPDFRect Rect)
{
    PDFCosHandle a;
    __CosDictAppend ( Bead, R, a = CosArrayNew ( _DDOC, false, 4 ) );
    CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.xl ) );
    CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.yl ) );
    CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.xr ) );
    CosArrayAppend ( a, CosRealNew ( _DDOC, false, Rect.yr ) );
}
#ifdef DELPHI
void PDFBeadSetRectM(PDFBeadHandle Bead, TPDFRect *Rect){
	PDFBeadSetRect( Bead, *Rect);
}
#endif

