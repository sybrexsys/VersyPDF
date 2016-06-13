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
VSActionA.c
*********************************************************************/

#include "../VSActionA.h" 
#include "VSCosAI.h"
#include "VSAssert.h"
#include "VSDocAI.h"
#include "VSMisc.h"
#include <string.h>


#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib


PDFDestinationHandle PDFDestinationFromExplit(PDFDocHandle Doc, PDFExplicitDest Dest, ppBool OtherDocument, ppBool IsIndirect)
{
    PDFCosHandle obj;
    if ( !OtherDocument ){
        if ( !_DOC->Pages )
            PDFDocLoadPages ( Doc );
        if ( Dest.Page >= _DOC->Pages->Size )
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    obj = CosArrayNew ( Doc, IsIndirect, 5 );
    if ( OtherDocument )
        CosArrayAppend ( obj, CosIntNew ( Doc, false, Dest.Page ) );
    else
        CosArrayAppend ( obj, PGO ( Dest.Page  ) );
    switch ( Dest.Type ){
        case edtXYZ:
            CosArrayAppend ( obj, _CosNameNew ( XYZ ) );
			CosArrayAppend ( obj, Dest.a == -1? CosNullNew(Doc): CosRealNew ( Doc, false, Dest.a ) );
            CosArrayAppend ( obj, Dest.b == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.b ) );
            CosArrayAppend ( obj, Dest.c == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.c ) );
            break;
        case edtFit:
            CosArrayAppend ( obj, _CosNameNew ( Fit ) );
            break;
        case edtFitH:
            CosArrayAppend ( obj, _CosNameNew ( FitH ) );
            CosArrayAppend ( obj, Dest.a == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.a ) );
            break;
        case edtFitV:
            CosArrayAppend ( obj, _CosNameNew ( FitV ) );
            CosArrayAppend ( obj, Dest.a == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.a ) );
            break;
        case edtFitR:
            CosArrayAppend ( obj, _CosNameNew ( FitR ) );
            CosArrayAppend ( obj, CosRealNew ( Doc, false, Dest.a ) );
            CosArrayAppend ( obj, CosRealNew ( Doc, false, Dest.b ) );
            CosArrayAppend ( obj, CosRealNew ( Doc, false, Dest.c ) );
            CosArrayAppend ( obj, CosRealNew ( Doc, false, Dest.d ) );
            break;
        case edtFitBH:
            CosArrayAppend ( obj, _CosNameNew ( FitBH ) );
            CosArrayAppend ( obj, Dest.a == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.a ) );
            break;
        case edtFitBV:
            CosArrayAppend ( obj, _CosNameNew ( FitBV ) );
            CosArrayAppend ( obj, Dest.a == -1? CosNullNew(Doc):CosRealNew ( Doc, false, Dest.a ) );
            break;
        case edtFitB:
            ;
        default:
            CosArrayAppend ( obj, _CosNameNew ( FitB ) );
    };
    return obj;
}



void PDFDestinationNamedNew(PDFDocHandle Doc, char *String, ppInt32 Length,
                           PDFExplicitDest Dest)
{
    PDFCosHandle ou, b, obj, wrk;
    ou = PDFDestinationFromExplit ( Doc, Dest, false, false );
    b = CosDictNew ( Doc, true, 1 );
    _CosDictAppend ( b, D, ou );
    obj = _CosDictValueByName ( _DOC->Root, Names );
    if ( !_IsCosDict ( obj ) ){
        obj = CosDictNew (  Doc, true, 1 );
        _CosDictAppend ( _DOC->Root, Names, obj );
         wrk = CosDictNew ( Doc, true, 5 );
        _CosDictAppend ( obj, Dests, wrk );
    } else{
        wrk = _CosDictValueByName ( obj, Dests );
        if ( _CosGetType ( wrk ) != CosDict ){
            wrk = CosDictNew ( Doc, true, 5 );
            _CosDictAppend ( obj, Dests,  wrk );
        };
    }
    ULSetObjToNameTree ( Doc, wrk, b, String, Length );
}

#ifdef DELPHI
void PDFDestinationNamedNewM(PDFDocHandle Doc, char *String, ppInt32 Length, PDFExplicitDest *Dest)
{
	PDFDestinationNamedNew ( Doc, String, Length, *Dest );		
}
PDFDestinationHandle PDFDestinationFromExplitM(PDFDocHandle Doc, PDFExplicitDest *Dest, ppBool OtherDocument, ppBool IsIndirect)
{
	return PDFDestinationFromExplit ( Doc, *Dest, OtherDocument, IsIndirect );
}

#endif

PDFDestinationHandle PDFDestinationFromString( PDFDocHandle Doc, char *String, ppInt32 Length, ppBool IsIndirect ){
	return CosStringNew( Doc, IsIndirect, String, Length );
}

void PDFActionSetNext( PDFActionHandle Action, PDFActionHandle Next)
{
    PDFCosHandle a, ar;
    a = CosDictValueByName ( Action, GetAtomLib(_CosLib ( Action), Next ) );
    if ( _IsCosArray ( a ) ){
        CosArrayAppend ( a, Next );
        return;
    }
    if ( _IsCosDict ( a ) ){
        ar = CosArrayNew ( _CosDoc( Action ), false, 2 );
        CosArrayAppend ( ar, CosCopy ( _CosDoc( Action ),a ) );
        CosArrayAppend ( ar,  Next );
        CosDictAppend ( Action, GetAtomLib( _CosLib ( Action ), Next ), ar );
        return;
    };
    CosDictAppend ( Action, GetAtomLib( _CosLib ( Action ), Next ), Next );
}

/* GoTo Action */

PDFActionHandle PDFActionNewGoToDestination(PDFDocHandle Doc, PDFDestinationHandle Destination )
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 2 );
    _CosDictAppend ( obj, S, _CosNameNew ( GoTo ) );
    _CosDictAppend ( obj, D, Destination );
    return obj;
}


/* GoToRemote Action */

PDFActionHandle PDFActionNewGoToRemote(PDFDocHandle Doc, char *FileName,
                                           ppInt32 FileNameLength, PDFDestinationHandle Destination,
                                           ppBool InNewWindow)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 4 );
    _CosDictAppend ( obj, S, _CosNameNew ( GoToR ) );
    _CosDictAppend ( obj, F, CosStringNew ( Doc, false, FileName, FileNameLength ) );
    _CosDictAppend ( obj, D, Destination );
    _CosDictAppend ( obj, NewWindow, CosBoolNew ( Doc, false, InNewWindow ) );
    return obj;
}



/* Launch Action */

PDFActionHandle PDFActionNewLaunch(PDFDocHandle Doc, char * FileName, char * DefaultDir, 
								   char * Operation, char * Params, ppBool InNewWindow)
{
    PDFCosHandle obj, obj1;
    obj = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( obj, S, _CosNameNew ( Launch ) );
    _CosDictAppend ( obj, NewWindow, CosBoolNew ( Doc, false, InNewWindow ) );
    _CosDictAppend ( obj, Win, ( obj1 = CosDictNew ( Doc, false, 4 ) ) );
    _CosDictAppend ( obj1, F, CosStringNew ( Doc, false, FileName, (ppUns32)strlen( FileName ) ) );
    if ( DefaultDir )
        _CosDictAppend ( obj1, D, CosStringNew ( Doc, false, DefaultDir, (ppUns32)strlen ( DefaultDir ) ) );
    if ( Operation )
        _CosDictAppend ( obj1, O, CosStringNew ( Doc, false, Operation, (ppUns32)strlen ( Operation ) ) );
    if ( Params )
        _CosDictAppend ( obj1, P, CosStringNew ( Doc, false, Params, (ppUns32)strlen ( Params ) ) );
    return obj;
}


/* Thread Action */

PDFActionHandle PDFActionNewThread(PDFDocHandle Doc, char *FileName,
                                   ppInt32 FileNameLength, PDFThreadActionParam Thread)
{
    PDFCosHandle obj;
    if ( FileName &&
         ( Thread.DestThreadType == taThreadHandle ||
           Thread.DestBeadType == taBeadHandle ) )
        _RAISE ( ErrDocumentLevel, dleInvalidHandleUsingError );
    obj = CosDictNew ( Doc, true, 4 );
    _CosDictAppend ( obj, S, _CosNameNew ( Thread ) );
    if ( FileName )
        _CosDictAppend ( obj, F, CosStringNew ( Doc, false, FileName, FileNameLength) );
    switch ( Thread.DestThreadType ){
        case taThreadHandle:
            _CosDictAppend ( obj, D, Thread.DestThread.ThreadHandle );
            break;
        case taThreadIndex:
            _CosDictAppend ( obj, D, CosIntNew ( Doc, false, Thread.DestThread.ThreadIndex ) );
            break;
        case taThreadTitle:
            _CosDictAppend ( obj, D, CosStringNew ( Doc, false,
                Thread.DestThread.ThreadTitle.String, Thread.DestThread.ThreadTitle.Length ) );
            break;
    }
    switch ( Thread.DestBeadType ){
        case taBeadHandle:
            _CosDictAppend ( obj, D, Thread.DestBead.BeadHandle );
            break;
        case taBeadIndex:
            _CosDictAppend ( obj, D, CosIntNew ( Doc, false, Thread.DestBead.BeadIndex ) );
            break;
    }
    return obj;
}

/* URI Action */
PDFActionHandle PDFActionNewURI(PDFDocHandle Doc, char *URI, ppInt32 Length,
                                ppBool IsMap)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( obj, S, _CosNameNew ( URI ) );
    _CosDictAppend ( obj, URI, CosStringNew ( Doc, false, URI, Length ) );
    _CosDictAppend ( obj, IsMap, CosBoolNew ( Doc, false, IsMap ) );
    return obj;
}


/* Hide Action */

PDFActionHandle PDFActionNewHide(PDFDocHandle Doc, ppBool IsHide)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( obj, S, _CosNameNew ( Hide ) );
    _CosDictAppend ( obj, T, CosArrayNew ( Doc, false, 5 ) );
    _CosDictAppend ( obj, H, CosBoolNew ( Doc, false, IsHide ) );
    return obj;
}

#define _DDOC CD(_CosDoc(Action))
#define _LLIB _DDOC->Lib

void PDFActionHideAddAnnotation(PDFActionHandle Action,
                                PDFAnnotationHandle Annotation)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) || _CosNameValue ( obj ) != GetAtomDDoc ( Hide ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, T );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, Annotation );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy ( _DDOC, obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( ar, Annotation );
        __CosDictAppend ( Action, T, ar );
        return;
    }
    __CosDictAppend ( Action, T, (ar = CosArrayNew ( _DDOC, false, 2 )) );
    CosArrayAppend ( ar, Annotation );
}

void PDFActionHideAddAnnotationName(PDFActionHandle Action,
                                    char *AnnotationName, ppInt32 Length)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) || _CosNameValue ( obj ) != GetAtomDDoc ( Hide ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, T );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length) );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy ( _DDOC, obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( obj,
                         CosStringNew ( _DDOC, false, AnnotationName, Length) );
        __CosDictAppend ( Action, T, ar );
        return;
    }
    __CosDictAppend ( Action, T, (ar = CosArrayNew ( _DDOC, false, 2 )) );
    CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
}


/* Named Action */
PDFActionHandle PDFActionNewNamed(PDFDocHandle Doc,
                                  PDFNamedActionType NamedType)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( obj, S, _CosNameNew ( Named ) );
    switch ( NamedType ){
        case naNextPage:
            _CosDictAppend ( obj, N, _CosNameNew ( NextPage ) );
            break;
        case naPrevPage:
            _CosDictAppend ( obj, N, _CosNameNew ( PrevPage ) );
            break;
        case naFirstPage:
            _CosDictAppend ( obj, N, _CosNameNew ( FirstPage ) );
            break;
        case naLastPage:
            _CosDictAppend ( obj, N, _CosNameNew ( LastPage ) );
            break;
    };
    return obj;
}


/* Submit-Form Action */

PDFActionHandle PDFActionNewSubmitForm(PDFDocHandle Doc, char *URI,
                                       ppInt32 Length, ppInt32 Flags)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 4 );
    _CosDictAppend ( obj, S, _CosNameNew ( SubmitForm ) );
    _CosDictAppend ( obj, F, CosStringNew ( Doc, false, URI, Length ) );
    _CosDictAppend ( obj, Fields, CosArrayNew ( Doc, false, 5 ) );
    _CosDictAppend ( obj, Flags, CosIntNew ( Doc, false, Flags ) );
    return obj;
}
void PDFActionSubmitFormAddAnnotation(PDFActionHandle Action, PDFAnnotationHandle Annotation)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) ||
         _CosNameValue ( obj ) != GetAtomDDoc ( SubmitForm ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, Fields );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, Annotation );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy (_DDOC,  obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( ar, Annotation );
        __CosDictAppend ( Action, Fields, ar );
        return;
    }
    __CosDictAppend ( Action, Fields, (ar = CosArrayNew (  _DDOC, false, 2 )) );
    CosArrayAppend ( ar, Annotation );
}

void PDFActionSubmitFormAddAnnotationName(PDFActionHandle Action,
                                          char *AnnotationName, ppInt32 Length)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) ||
         _CosNameValue ( obj ) != GetAtomDDoc ( SubmitForm ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, Fields );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy (_DDOC, obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
        __CosDictAppend ( Action, Fields, ar );
        return;
    }
    __CosDictAppend ( Action, Fields, (ar = CosArrayNew ( _DDOC, false, 2 )) );
    CosArrayAppend ( ar, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
}


/* Reset-Form Action */

PDFActionHandle PDFActionNewResetForm(PDFDocHandle Doc, ppBool Exclude)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 3 );
    _CosDictAppend ( obj, S, _CosNameNew ( ResetForm ) );
    if ( Exclude )
        _CosDictAppend ( obj, Flags, CosIntNew ( Doc, false, 1 ) );
    return obj;
}
void PDFActionResetFormAddAnnotation( PDFActionHandle Action, PDFAnnotationHandle Annotation)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) ||
         _CosNameValue ( obj ) != GetAtomDDoc ( ResetForm ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, Fields );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, Annotation );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy (_DDOC, obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( ar, Annotation );
        __CosDictAppend ( Action, Fields, ar );
        return;
    }
    __CosDictAppend ( Action, Fields, (ar = CosArrayNew ( _DDOC, false, 2 )) );
    CosArrayAppend ( ar, Annotation );
}

void PDFActionResetFormAddAnnotationName(PDFActionHandle Action, char *AnnotationName, ppInt32 Length)
{
    PDFCosHandle obj, ar;
    obj = __CosDictValueByName ( Action, S );
    if ( !_IsCosName ( obj ) ||
         _CosNameValue ( obj ) != GetAtomDDoc ( ResetForm ) )
        __RAISE ( ErrDocumentLevel, dleUncompatibleActionError );
    obj = __CosDictValueByName ( Action, Fields );
    if ( _IsCosArray ( obj ) ){
        CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
        return;
    };
    if ( _IsCosDict ( obj ) || _IsCosString ( obj ) ){
        if ( _IsCosDict ( obj ) )
            if ( !_CosIndirectDict ( obj ) )
                __RAISE ( ErrCosLevel, cleInvalidObjError );
        ar = CosArrayNew ( _DDOC, false, 2 );
        if ( _IsCosString ( obj ) )
            CosArrayAppend ( ar, CosCopy (_DDOC, obj ) );
        else
            CosArrayAppend ( ar, obj );
        CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
        __CosDictAppend ( Action, Fields, ar );
        return;
    }
    __CosDictAppend ( Action, Fields, (ar = CosArrayNew ( _DDOC, false, 2 )) );
    CosArrayAppend ( obj, CosStringNew ( _DDOC, false, AnnotationName, Length ) );
}


/* Import Data Action */
PDFActionHandle PDFActionNewImportData(PDFDocHandle Doc, char *FileName,
                                       ppInt32 Length)
{
    PDFCosHandle obj, t;
    obj = CosDictNew ( Doc, true, 2 );
    _CosDictAppend ( obj, S, _CosNameNew ( ImportData ) );
    _CosDictAppend ( obj, F, t = CosDictNew ( Doc, false, 2 ) );
    _CosDictAppend ( t, Type, _CosNameNew ( Filespec ) );
    _CosDictAppend ( t, F, CosStringNew ( Doc, false, FileName, Length ) );
    return obj;
}

/* JavaScript Action */
PDFActionHandle PDFActionNewJavaScript(PDFDocHandle Doc, char *JavaScript,
                                       ppInt32 Length)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 2 );
    _CosDictAppend ( obj, S, _CosNameNew ( JavaScript ) );
    _CosDictAppend ( obj, JS, CosStringNew ( Doc, false, JavaScript, Length ) );
    return obj;
}
PDFActionHandle PDFActionNewJavaScriptStream(PDFDocHandle Doc,
                                             PDFCosHandle JavaScript)
{
    PDFCosHandle obj;
    obj = CosDictNew ( Doc, true, 2 );
    _CosDictAppend ( obj, S, _CosNameNew ( JavaScript ) );
    _CosDictAppend ( obj, JS, JavaScript );
    return obj;
}

