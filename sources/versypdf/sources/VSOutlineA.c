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
VSOutlineA.c
*********************************************************************/

#include "../VSOutlineA.h"
#include "VSLibI.h"
#include "VSDocAI.h"
#include "VSCosAI.h"
#include "VSAssert.h"
#include "VSMisc.h"
#include <string.h>

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

#define _DDOC CD(_CosDoc(Outline))
#define _LLIB _DDOC->Lib

PDFOutlineHandle PDFDocGetOutlineRoot(PDFDocHandle Doc)
{
    return _CosDictValueByName ( _DOC->Root, Outlines );
}

void PDFOutlinesOneNew(PDFCosHandle Outline, ppBool IsNew)
{
    PDFCosHandle obj, Count, Parent;
    ppInt32 Inc;
    obj = Outline;
    if ( !_IsCosDict ( obj ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( IsNew )
        Inc = 0;
    else{
        Count = __CosDictValueByName ( obj, Count );
        if ( !_IsCosInt ( Count ) )
            Inc = 0;
        else if ( _CosIntValue ( Count ) < 0 )
            Inc = 0;
        else
            Inc = _CosIntValue ( Count );
    }
    while ( 1 ){
        Parent = __CosDictValueByName ( obj, Parent );
        if ( !_IsCosDict ( Parent ) )
            return;
        Count = __CosDictValueByName ( Parent, Count );
        obj = Parent;
        if ( !_IsCosInt ( Count )  )
            __CosDictAppend ( Parent, Count, Count = CosIntNew ( _DDOC, false, 0 ) );
        if ( _CosIntValue ( Count ) < 0 ){
            CosIntSetValue ( Count, _CosIntValue ( Count ) - Inc - 1 );
            return;
        } else
            CosIntSetValue ( Count, _CosIntValue ( Count ) + Inc + 1 );
    }
}

PDFOutlineHandle PDFOutlineAddNewSibling(PDFOutlineHandle Outline)
{
    PDFCosHandle wrk, parent, last;
    if ( CO(PDFDocGetOutlineRoot ( _DDOC )) == CO(Outline ))
        __RAISE ( ErrDocumentLevel, dleCannotUseBookmarkRootError );
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    parent = __CosDictValueByName ( Outline, Parent );
    if ( !_CosIndirectDict ( parent ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    last = __CosDictValueByName ( parent, Last );
    if ( !_CosIndirectDict ( last ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    wrk = CosDictNew ( _DDOC, true, 5 );
    __CosDictAppend ( wrk, Parent, parent );
    __CosDictAppend ( wrk, Prev, last );
    __CosDictAppend ( wrk, Title, CosStringNew ( _DDOC, false, "Untitled", 8 ) );
    __CosDictAppend ( last, Next, wrk  );
    __CosDictAppend ( parent, Last, wrk  );
    PDFOutlinesOneNew ( wrk, true );
    return wrk;
}

void PDFOutlineAddSibling(PDFOutlineHandle Outline, PDFOutlineHandle Sibling)
{
    PDFCosHandle wrk, parent, last;
    if ( CO(PDFDocGetOutlineRoot ( _DDOC )) == CO(Outline ))
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    parent = __CosDictValueByName ( Outline, Parent );
    if ( !_CosIndirectDict ( parent ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    last = __CosDictValueByName ( parent, Last );
    if ( !_CosIndirectDict ( last ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    wrk = Sibling;
    if ( !_CosIndirectDict ( wrk ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    __CosDictAppend ( wrk, Parent,  parent  );
    __CosDictAppend ( wrk, Prev,  last  );
    __CosDictAppend ( last, Next,  Sibling  );
    __CosDictAppend ( parent, Last,  Sibling  );
    PDFOutlinesOneNew (wrk, false );
}

PDFOutlineHandle PDFOutlineAddNewNext(PDFOutlineHandle Outline)
{
    PDFCosHandle wrk, parent, next, last;
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( CO(PDFDocGetOutlineRoot ( _DDOC )) == CO(Outline) )
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    parent = __CosDictValueByName ( Outline, Parent );
    if ( !_CosIndirectDict ( parent ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    last = __CosDictValueByName ( parent, Last );
    if ( !_CosIndirectDict ( last ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    wrk = CosDictNew ( _DDOC, true, 5 );
    __CosDictAppend ( wrk, Title, CosStringNew ( _DDOC, false, "Untitled", 8 ) );
    __CosDictAppend ( wrk, Parent,  parent  );
    __CosDictAppend ( Outline, Next, wrk );
    __CosDictAppend ( wrk, Prev, Outline );
    if ( CO(last) == CO(Outline) )
        __CosDictAppend ( parent, Last, wrk );
    else{
        next = __CosDictValueByName ( Outline, Next );
        if ( !_CosIndirectDict ( next ) )
            __RAISE ( ErrCosLevel, cleInvalidObjError );
        __CosDictAppend ( wrk, Next, next );
        __CosDictAppend ( next, Prev, wrk );
    };
    PDFOutlinesOneNew (wrk, true );
    return wrk;
}

PDFOutlineHandle PDFOutlineAddNewPrev(PDFOutlineHandle Outline)
{
    PDFCosHandle wrk, parent, prev, first;
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( CO(PDFDocGetOutlineRoot ( _DDOC )) == CO(Outline) )
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    parent = __CosDictValueByName ( Outline, Parent );
    if ( !_CosIndirectDict ( parent ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    first = __CosDictValueByName ( parent, First );
    if ( !_CosIndirectDict ( first ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    wrk = CosDictNew ( _DDOC, true, 5 );
    __CosDictAppend ( wrk, Title, CosStringNew ( _DDOC, false, "Untitled", 8 ) );
    __CosDictAppend ( wrk, Parent, parent );
    __CosDictAppend ( Outline, Prev, wrk );
    __CosDictAppend ( wrk, Next, Outline );
    if ( _CosEqual(first,Outline) )
        __CosDictAppend ( parent, First, wrk );
    else{
        prev = __CosDictValueByName ( Outline, Prev );
        if ( !_CosIndirectDict ( prev ) )
            __RAISE ( ErrCosLevel, cleInvalidObjError );
        __CosDictAppend ( wrk, Prev, prev );
        __CosDictAppend ( prev, Next, wrk );
    };
    PDFOutlinesOneNew ( wrk, true );
    return wrk;
}


PDFOutlineHandle PDFOutlineAddNewChild(PDFOutlineHandle Outline)
{
    PDFOutlineHandle l;
    PDFCosHandle obj, wrk;
    if ( _IsCosNull( Outline ) )
        Outline = PDFDocGetOutlineRoot ( _DDOC );
    if ( _IsCosNull ( Outline ) ){
        obj = CosDictNew ( _DDOC,  true, 4 );
        __CosDictAppend ( _DDOC->Root, Outlines, obj );
        __CosDictAppend ( obj, Type, __CosNameNew ( Outlines ) );
        __CosDictAppend ( obj, Count, CosIntNew ( _DDOC, false, 1 ) );
        wrk = CosDictNew ( _DDOC, true, 5 );
        __CosDictAppend ( wrk, Title, CosStringNew ( _DDOC, false, "Untitled", 8 ) );
        __CosDictAppend ( wrk, Parent, obj );
        __CosDictAppend ( obj, First,  wrk );
        __CosDictAppend ( obj, Last,  wrk );
        return wrk;
    }
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    l = PDFOutlineGetLastChild ( Outline );
    if ( !_IsCosNull(l) )
        return PDFOutlineAddNewSibling ( l );
    else{
        wrk = CosDictNew ( _DDOC, true, 5 );
        __CosDictAppend ( wrk, Parent, Outline );
        __CosDictAppend ( wrk, Title, CosStringNew ( _DDOC, false, "Untitled", 8) );
        __CosDictAppend ( Outline, First, wrk );
        __CosDictAppend ( Outline, Last, wrk );
        PDFOutlinesOneNew ( wrk, true );
        return wrk;
    }
}


PDFOutlineHandle PDFOutlineGetFirstChild( PDFOutlineHandle Outline)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    return __CosDictValueByName ( Outline, First );
}

PDFOutlineHandle PDFOutlineGetLastChild( PDFOutlineHandle Outline)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    return __CosDictValueByName ( Outline, Last );
}

PDFOutlineHandle PDFOutlineGetNext( PDFOutlineHandle Outline)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosEqual(PDFDocGetOutlineRoot ( _DDOC ), Outline ))
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    return __CosDictValueByName ( Outline, Next );
}

PDFOutlineHandle PDFOutlineGetPrev(  PDFOutlineHandle Outline)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosEqual(PDFDocGetOutlineRoot ( _DDOC ), Outline ))
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    return __CosDictValueByName ( Outline, Prev );
}

PDFOutlineHandle PDFOutlineGetParent(  PDFOutlineHandle Outline)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosEqual(PDFDocGetOutlineRoot ( _DDOC ), Outline ))
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    return __CosDictValueByName ( Outline, Parent );
}

void PDFOutlineUnLink(  PDFOutlineHandle Outline)
{
    PDFCosHandle obj, next, prev, parent, wrk, Count;
    ppInt32 inc;
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosEqual(PDFDocGetOutlineRoot ( _DDOC ), Outline ))
        __RAISE ( ErrGeneralLevel, dleCannotUseBookmarkRootError );
    next = __CosDictValueByName ( Outline, Next );
    prev = __CosDictValueByName ( Outline, Prev );
    parent = __CosDictValueByName ( Outline, Parent );
    if ( !_CosIndirectDict ( parent ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( !_CosIndirectDict ( next ) ){
        if ( _CosIndirectDict ( prev ) ){
            __CosDictAppend ( parent, Last, prev );
            __CosDictRemoveKey ( prev, Next );
        } else{
            __CosDictRemoveKey ( parent, First );
            __CosDictRemoveKey ( parent, Last );
        }
    } else{
        if ( !_CosIndirectDict ( prev ) ){
            __CosDictRemoveKey ( next, Prev );
            __CosDictAppend ( parent, First, next );
        } else{
            __CosDictAppend ( next, Prev, prev );
            __CosDictAppend ( prev, Next, next );
        }
    }
    __CosDictRemoveKey ( Outline, Next );
    __CosDictRemoveKey ( Outline, Prev );
    wrk = __CosDictValueByName ( Outline, Count );
    if ( _CosGetType ( wrk ) != CosInteger )
        inc = 0;
    else
        inc = _CosIntValue ( wrk ) >= 0 ? _CosIntValue ( wrk ) : 0;
    obj = Outline;
    while ( 1 ){
        parent = __CosDictValueByName ( obj, Parent );
        if ( !_CosIndirectDict ( parent ) )
            return;
        Count = __CosDictValueByName ( parent, Count );
        obj = parent;
        if ( _CosGetType ( Count ) != CosInteger )
            __CosDictAppend ( parent, Count, Count = CosIntNew ( _DDOC, false, 0 ) );
        else{
            if ( _CosIntValue ( Count ) < 0 ){
                _CosIntValue ( Count ) += inc + 1;
                return;
            } else
                _CosIntValue ( Count ) -= inc + 1;
        }
    }
}

void PDFOutlineFree(PDFOutlineHandle Outline)
{
    PDFCosHandle wrkNext, wrk;
    PDFID ID;
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    wrk = __CosDictValueByName ( Outline, First );
    while ( _CosIndirectDict ( wrk ) ){
        if ( _CosGetType ( wrk ) != CosDict )
            __RAISE ( ErrCosLevel, cleInvalidObjError );
        wrkNext = __CosDictValueByNameNRF ( wrk, Next );
        PDFOutlineFree ( wrk );
        wrk = wrkNext;
    }
    ID = _CosObjFullID ( Outline );
    PDFDocDeleteObj ( _DDOC, ID.ID );
}

void PDFOutlineDelete(PDFOutlineHandle Outline)
{
    PDFOutlineUnLink ( Outline );
    PDFOutlineFree ( Outline );
}


ppBool PDFOutlineHasChildren( PDFOutlineHandle Outline)
{
    PDFCosHandle obj;
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    obj = __CosDictValueByNameNRF ( Outline, First );
    return (ppBool)( _CosGetType ( obj ) == CosRef );
}

ppInt32 PDFOutlineGetCount( PDFOutlineHandle Outline)
{
    PDFCosHandle obj;
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    obj = __CosDictValueByName ( Outline, Count );
    if ( _CosGetType ( obj ) != CosInteger )
        return 0;
    else
        return _CosIntValue ( obj ) >= 0 ? _CosIntValue ( obj ) : 0;
}

void PDFOutlineSetExpanded( PDFOutlineHandle Outline, ppBool Expanded)
{
    PDFCosHandle obj, Count, parent;
    ppInt32 inc;
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    obj = __CosDictValueByName ( Outline, Count );
    if ( _CosGetType ( obj ) != CosInteger )
        return;
    if ( _CosIntValue ( obj ) == 0 )
        return;
    if ( _CosIntValue ( obj ) > 0 ){
        if ( Expanded )
            return;
        inc = _CosIntValue ( obj );
        _CosIntValue ( obj ) = -_CosIntValue ( obj );
        obj = Outline;
        while ( 1 ){
            parent = __CosDictValueByName ( obj, Parent );
            if ( !_CosIndirectDict ( parent ) )
                return;
            Count = __CosDictValueByName ( parent, Count );
            obj = parent;
            if ( _CosGetType ( Count ) != CosInteger )
                __CosDictAppend ( parent, Count, Count = CosIntNew ( _DDOC, false, 0 ) );
            else{
                if ( _CosIntValue ( Count ) < 0 ){
                    _CosIntValue ( Count ) += inc;
                    return;
                } else
                    _CosIntValue ( Count ) -= inc;
            }
        }
    } else{
        if ( !Expanded )
            return;
        inc = _CosIntValue ( obj );
        _CosIntValue ( obj ) = -_CosIntValue ( obj );
        obj = Outline;
        while ( 1 ){
            parent = __CosDictValueByName ( obj, Parent );
            if ( !_CosIndirectDict ( parent ) )
                return;
            Count = __CosDictValueByName ( parent, Count );
            obj = parent;
            if ( _CosGetType ( Count ) != CosInteger )
                __CosDictAppend ( parent, Count, Count = CosIntNew ( _DDOC, false, 0 ) );
            else{
                if ( _CosIntValue ( Count ) < 0 ){
                    _CosIntValue ( Count ) -= inc;
                    return;
                } else
                    _CosIntValue ( Count ) += inc;
            }
        }
    }
}


void PDFOutlineSetColor(PDFOutlineHandle Outline, PPDFColor Color)
{
    PDFCosHandle obj;
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( Color->Device != cgRGB )
        __RAISE ( ErrDocumentLevel, dleCannotUseThisColorDeviceError );
    __CosDictAppend ( Outline, C, obj = CosArrayNew (  _DDOC, false, 3 ) );
    CosArrayAppend ( obj, CosRealNew ( _DDOC, false, Color->Color.RGB.R ) );
    CosArrayAppend ( obj, CosRealNew ( _DDOC, false, Color->Color.RGB.G ) );
    CosArrayAppend ( obj, CosRealNew ( _DDOC, false, Color->Color.RGB.B ) );
    
}


void PDFOutlineSetFlags(PDFOutlineHandle Outline, ppInt32 Flags)
{
    if ( _CosGetType ( Outline ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    __CosDictAppend ( Outline, F, CosIntNew ( _DDOC, false, Flags ) );
}


void PDFOutlineSetTitle( PDFOutlineHandle Outline, char *String, ppInt32 Length)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    __CosDictAppend ( Outline, Title, CosStringNew ( _DDOC, false, String, Length ) );
} 

void PDFOutlineSetAction( PDFOutlineHandle Outline, PDFActionHandle Action)
{
    if ( !_CosIndirectDict ( Outline ) )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    __CosDictAppend ( Outline, A, Action );
}

void PDFOutlineSetDestination(PDFOutlineHandle Outline, PDFDestinationHandle Destination)
{
	if ( !_CosIndirectDict ( Outline ) )
		__RAISE ( ErrCosLevel, cleInvalidObjError );
	__CosDictAppend ( Outline, Dest, Destination );
}