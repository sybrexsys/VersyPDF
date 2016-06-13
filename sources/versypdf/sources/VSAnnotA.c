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
VSAnnotA.c
*********************************************************************/

#include "VSName.h"
#include "VSDocAI.h"
#include "VSAnnotA.h"
#include "VSFilter.h"
#include "VSCanvasAI.h"


#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib 

#define _DDOC CD(CO(Annotation)->Doc)
#define _LLIB _DDOC->Lib 





/* Common Annotation functions */

void PDFAnnotationSetFlag( PDFAnnotationHandle Annotation, ppUns16 AnnotationFlag )
{
    __CosDictAppend ( Annotation, F, CosIntNew ( _DDOC, false, AnnotationFlag ) );
}

void PDFAnnotationSetAlphaBlending( PDFAnnotationHandle Annotation, ppReal AlphaBlending )
{
    __CosDictAppend ( Annotation, F, CosRealNew ( _DDOC, false, AlphaBlending ) );
}

void PDFAnnotationSetAppearance ( PDFAnnotationHandle Annotation, TAppearanceState State,  PDFCosHandle Appearance )
{
    PDFCosHandle wrk;
    ppAtom t;
    wrk = __CosDictValueByName ( Annotation, AP );
    if ( ! _IsCosDict ( wrk ) )
         __CosDictAppend ( Annotation, AP, wrk = CosDictNew ( _DDOC, false, 1 ) );
    switch ( State ){
        case asRollover:
            t = GetAtomDDoc ( R );
            break;
        case asDown:
            t = GetAtomDDoc ( D );
            break;
        default:
            t = GetAtomDDoc ( N );
    }
    CosDictAppend ( wrk, t, Appearance );
}



void PDFAnnotationSetTitle ( PDFAnnotationHandle Annotation, char * Title, ppUns32 TitleLength )
{
    __CosDictAppend ( Annotation, T, CosStringNew ( _DDOC, false, Title, TitleLength ) ) ;
}


void PDFAnnotationSetName ( PDFAnnotationHandle Annotation, char * Name, ppUns32 NameLength )
{
    __CosDictAppend ( Annotation, NM, CosStringNew ( _DDOC, false, Name, NameLength ) ) ;
}


void PDFAnnotationSetBorderStyle ( PDFAnnotationHandle Annotation, ppReal Width, TBorderStyle Style, PDFCosHandle Dash )
{
	PDFCosHandle wrk;
	ppAtom st;
	PDFDocHandle Doc = _CosDoc ( Annotation );
	wrk = CosDictNew( Doc, false, 4 );
	_CosDictAppend ( wrk, Type, _CosNameNew ( Border ) );
	_CosDictAppend ( wrk, W, CosRealNew ( Doc, false, Width ) );
	switch (Style ){
		case bsDashed:
			st = GetAtomDoc ( D );
			_CosDictAppend ( wrk, D, Dash );
			break;
		case bsBeveled:
			st = GetAtomDoc ( B );
			break;
		case bsInset:
			st = GetAtomDoc ( I );
			break;
		case bsUnderline:
			st = GetAtomDoc ( U );
			break;
		case bsSolid:
		default:
			st = GetAtomDoc ( S );
			break;
	}
	_CosDictAppend( wrk, S, CosNameNew ( Doc, false, st ) );
    _CosDictAppend ( Annotation, BS, wrk ) ;
}

void PDFAnnotationSetColor ( PDFAnnotationHandle Annotation, TPDFColor Color )
{
    PDFCosHandle obj;
    PDFDocHandle Doc;
    if ( _CosGetType ( Annotation ) != CosDict )
        __RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( Color.Device != cgRGB )
        __RAISE ( ErrDocumentLevel, dleCannotUseThisColorDeviceError );
    __CosDictAppend ( Annotation, C, obj = CosArrayNew ( Doc = _CosDoc ( Annotation ), false, 3 ) );
    CosArrayAppend ( obj, CosRealNew ( Doc, false, Color.Color.RGB.R ) );
    CosArrayAppend ( obj, CosRealNew ( Doc, false, Color.Color.RGB.G ) );
    CosArrayAppend ( obj, CosRealNew ( Doc, false, Color.Color.RGB.B ) );
}

#ifdef DELPHI
void PDFAnnotationSetColorM ( PDFAnnotationHandle Annotation, TPDFColor *Color )
{
	PDFAnnotationSetColor ( Annotation, *Color );
}
#endif

void PDFAnnotationSetAction ( PDFAnnotationHandle Annotation, PDFActionHandle Action )
{
    __CosDictAppend ( Annotation, A, Action );
}


/*          Text Annotation         */


PDFAnnotationHandle PDFPageAppendAnnotationText (PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, char * Content, 
												 ppUns32 ContentLength, ppBool IsOpened, TAnnotationTextName Name )
{
    PDFCosHandle Annot, R, PO, POAnnots;
	ppAtom N ;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = PGO(Page);
    Annot = CosDictNew( Doc, true, 5 );
    _CosDictAppend ( Annot,Type, _CosNameNew ( Annot ) );
    _CosDictAppend ( Annot,Subtype, _CosNameNew ( Text ) );
    _CosDictAppend ( Annot, Contents, CosStringNew ( Doc, false, Content, ContentLength ) );
	_CosDictAppend ( Annot, Open, CosBoolNew ( Doc, false, IsOpened ) );
	switch ( Name ){
		case atnComment:
			N = GetAtomDoc ( Comment );
			break;
		case atnKey:
			N = GetAtomDoc ( Key );
			break;
		case atnHelp:
			N = GetAtomDoc ( Help );
			break;
		case atnNewParagraph:
			N = GetAtomDoc ( NewParagraph );
			break;
		case atnParagraph:
			N = GetAtomDoc ( Paragraph );
			break;
		case atnInsert:
			N = GetAtomDoc ( Insert );
			break;
		case atnNote:
		default:
			N = GetAtomDoc ( Note );
			break;
	}
	_CosDictAppend ( Annot, Name, CosNameNew ( Doc, false, N ) );
    _CosDictAppend ( Annot, Rect, R =CosArrayNew ( Doc, false, 4 ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xr ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yr ) );   
    POAnnots = _CosDictValueByName( PO, Annots );
    if ( ! _IsCosArray(POAnnots) ){
        _CosDictAppend ( PO, Annots, POAnnots =  CosArrayNew( Doc,false, 1) );          
    };
    CosArrayAppend( POAnnots, Annot );      

    return Annot;
}


/*          Link Annotation         */


PDFAnnotationHandle PDFPageAppendAnnotationLinkWithDest ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, 
												 PDFDestinationHandle Destination, THighLightMode Mode, ppBool Visible)
{
    PDFCosHandle Annot, R, PO, POAnnots;
	ppAtom wrk;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if (Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = PGO(Page);
    Annot = CosDictNew( Doc, true, 5 );
    _CosDictAppend ( Annot, Type, _CosNameNew ( Annot ) );
    _CosDictAppend ( Annot, Subtype, _CosNameNew ( Link ) );
	if ( ! Visible )
		_CosDictAppend ( Annot, Border, CosArrayNew( Doc, false,0 ) );
    _CosDictAppend ( Annot, Dest, Destination );
	switch ( Mode ) {
		case hlmNoHightLight:
			wrk = GetAtomDoc ( N );
			break;
		case hlmOutline:
			wrk = GetAtomDoc ( O );
			break;
		case hlmPush:
			wrk = GetAtomDoc ( P );
			break;
		case hlmInvert:
		default:
			wrk = GetAtomDoc ( I );
			break;
	}
	_CosDictAppend ( Annot, H, CosNameNew ( Doc, false, wrk ) );
    _CosDictAppend ( Annot, Rect, R = CosArrayNew ( Doc, false, 4 ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xr ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yr ) );
    POAnnots = _CosDictValueByName( PO, Annots );
    if ( ! _IsCosArray(POAnnots) ){
        _CosDictAppend ( PO, Annots, POAnnots =  CosArrayNew( Doc,false, 1) );          
    };
    CosArrayAppend( POAnnots, Annot );    

    return Annot;
}

PDFAnnotationHandle PDFPageAppendAnnotationLinkWithAction ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, 
												 PDFActionHandle Action, THighLightMode Mode, ppBool Visible)
{
	PDFCosHandle Annot, R, PO, POAnnots;
	ppAtom wrk;
	if ( !_DOC->Pages )
		PDFDocLoadPages ( Doc );
	if (Page >= _DOC->Pages->Size )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	PO = PGO(Page);
	Annot = CosDictNew( Doc, true, 5 );
	_CosDictAppend ( Annot, Type, _CosNameNew ( Annot ) );
	_CosDictAppend ( Annot, Subtype, _CosNameNew ( Link ) );
	if ( ! Visible )
		_CosDictAppend ( Annot, Border, CosArrayNew( Doc, false,0 ) ); 
	_CosDictAppend ( Annot, A, Action );
	switch ( Mode ) {
		case hlmNoHightLight:
			wrk = GetAtomDoc ( N );
			break;
		case hlmOutline:
			wrk = GetAtomDoc ( O );
			break;
		case hlmPush:
			wrk = GetAtomDoc ( P );
			break;
		case hlmInvert:
		default:
			wrk = GetAtomDoc ( I );
			break;
	}
	_CosDictAppend ( Annot, H, CosNameNew ( Doc, false, wrk ) );
	_CosDictAppend ( Annot, Rect, R = CosArrayNew ( Doc, false, 4 ) );
	CosArrayAppend( R, CosRealNew( Doc, false, Rect.xl ) );
	CosArrayAppend( R, CosRealNew( Doc, false, Rect.yl ) );
	CosArrayAppend( R, CosRealNew( Doc, false, Rect.xr ) );
	CosArrayAppend( R, CosRealNew( Doc, false, Rect.yr ) );
	POAnnots = _CosDictValueByName( PO, Annots );
	if ( ! _IsCosArray(POAnnots) ){
		_CosDictAppend ( PO, Annots, POAnnots =  CosArrayNew( Doc,false, 1) );          
	};
	CosArrayAppend( POAnnots, Annot );    

	return Annot;
}

PDFAnnotationHandle PDFPageAppendAnnotationStamp ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect,char * Content, 
												  ppUns32 ContentLength, TAnnotationStampName Name)
{
    PDFCosHandle Annot, R, PO, POAnnots;
	ppAtom N = ppAtomNull;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = PGO(Page);
    Annot = CosDictNew( Doc, true, 5 );
    _CosDictAppend ( Annot,Type, _CosNameNew ( Annot ) );
    _CosDictAppend ( Annot,Subtype, _CosNameNew ( Stamp ) );
    _CosDictAppend ( Annot, Contents, CosStringNew ( Doc, false, Content, ContentLength ) );
	switch ( Name ){
		case asnApproved:
			N = GetAtomDoc ( Approved );
			break;
		case asnExperimental:
			N = GetAtomDoc ( Experimental );
			break;
		case asnNotApproved:
			N = GetAtomDoc ( NotApproved );
			break;
		case asnAsIs:
			N = GetAtomDoc ( AsIs );
			break;
		case asnExpired:
			N = GetAtomDoc ( Expired );
			break;
		case asnNotForPublicRelease:
			N = GetAtomDoc ( NotForPublicRelease );
			break;
		case asnConfidential:
			N = GetAtomDoc ( Confidential );
			break;
		case asnFinal:
			N = GetAtomDoc ( Final );
			break;
		case asnSold:
			N = GetAtomDoc ( Sold );
			break;
		case asnDepartmental:
			N = GetAtomDoc ( Departmental );
			break;
		case asnForComment:
			N = GetAtomDoc ( ForComment );
			break;
		case asnTopSecret:
			N = GetAtomDoc ( TopSecret );
			break;
		case asnForPublicRelease:
			N = GetAtomDoc ( ForPublicRelease );
			break;
		case asnOther:
			break;
		case asnDraft:
		default:
			N = GetAtomDoc ( Draft );
			break;
	}
	if ( N != ppAtomNull)
		_CosDictAppend ( Annot, Name, CosNameNew ( Doc, false, N ) );
    _CosDictAppend ( Annot, Rect, R =CosArrayNew ( Doc, false, 4 ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yl ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.xr ) );
    CosArrayAppend( R, CosRealNew( Doc, false, Rect.yr ) );
    
    POAnnots = _CosDictValueByName( PO, Annots );
    if ( ! _IsCosArray(POAnnots) ){
        _CosDictAppend ( PO, Annots, POAnnots =  CosArrayNew( Doc,false, 1) );          
    };
    CosArrayAppend( POAnnots, Annot );      
    return Annot;
}

PDFAnnotationHandle PDFPageAppendAnnotationStampWithDrawBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect,char * Content, 
												  ppUns32 ContentLength, TAnnotationStampName Name, ppUns32 Resolution, PBXHandle *PBX)
{
	PDFAnnotationHandle r;
	PPBX         PaintContent;
	PDFCosHandle StreamObject, ArrayObject;
	ppReal          bbx, bby;


	r = PDFPageAppendAnnotationStamp ( Doc, Page, Rect, Content, ContentLength, Name );
	bbx = fabs( ( ppReal ) ( Rect.xr - Rect.xl ) );
	bby = fabs( ( ppReal ) ( Rect.yr - Rect.yl ) );
	StreamObject = CosStreamNew( Doc, true, 0 );
	_CosDictAppend( StreamObject, Type, CosNameNew( Doc, false, GetAtomDoc( XObject ) ) );
	_CosDictAppend( StreamObject, Subtype, CosNameNew( Doc, false, GetAtomDoc( Form ) ) );
	_CosDictAppend( StreamObject, Matrix, ArrayObject = CosArrayNew( Doc, false, 6 ) );
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 1  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 1  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	_CosDictAppend( StreamObject, Resources, CosDictNew( Doc, true, 0 ) );
	_CosDictAppend( StreamObject, BBox, ArrayObject = CosArrayNew( Doc, false, 6 ) );
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bbx ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bby ));

	PaintContent = (PPBX)mmalloc ( _LIB, sizeof ( TPBX ) );
	PaintContent->Doc = Doc;
	PaintContent->Res = _CosDictValueByName( StreamObject, Resources );
	PaintContent->cs  = StreamObject;
	PaintContent->Strm = CosStreamGetValue ( StreamObject );
	PaintContent->Resolution = Resolution;
	PaintContent->Width = bbx;
	PaintContent->Height = bby;
	PaintContent->BaseLine = false;
	PaintContent->E2I = 72.0 / (ppReal) Resolution;
	PaintContent->I2E = ( ppReal ) Resolution / 72.0;
	PaintContent->CharSpace = 0.0;
	PaintContent->WordSpace = 0.0;
	PaintContent->HorizontalScaling = 100.0;
	PaintContent->CurrentFontIndex = 0;
	PaintContent->CurrentFontSize = 10.0;
	PaintContent->TextInited = false;
	PaintContent->TextUsed = false;
	PaintContent->GrayUsed = false;
	PaintContent->ColorUsed = false;
	PaintContent->TextAngle = 0.0;
	PaintContent->UnderLine = false;
	PaintContent->StrikeOut = false;
	PaintContent->FontInited = false;
	PaintContent->Unicode = false;
	PaintContent->CurrentFontIndex = ppMAXUns32;
	PaintContent->TY = 0.0;
	PaintContent->TX = 0.0;
	PaintContent->ResourceFontIndex = 0;
	PaintContent->ResourceImageIndex = 0;
	PaintContent->ResourceGStateIndex = 0;
	PaintContent->FontIsChanged = true;
	PaintContent->SaveCount = 0;
	PaintContent->WT.YScale = 1.0;
	PaintContent->WT.XScale = 1.0;
	PaintContent->NextOperatorChar = ' ';
	PaintContent->Rotate = pra0;
	PBXSetAngle( PaintContent, PaintContent->Rotate );
	_CosNullNew( Doc, PaintContent->AcroformRes );
	CDebugCode (
		PaintContent->DBG = NULL;
	);
	PDFAnnotationSetAppearance( r, asNormal, PaintContent->cs );
	*PBX = (PBXHandle)PaintContent;
	return r;
}


#ifdef DELPHI
PDFAnnotationHandle PDFPageAppendAnnotationTextM (PDFDocHandle Doc, ppUns32 Page, TPDFRect *Rect, char * Content, 
												 ppUns32 ContentLength, ppBool IsOpened, TAnnotationTextName Name )
{
	return PDFPageAppendAnnotationText ( Doc, Page, *Rect, Content, ContentLength, IsOpened, Name );
}

PDFAnnotationHandle PDFPageAppendAnnotationStampM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *Rect,char * Content, 
												  ppUns32 ContentLength, TAnnotationStampName Name)
{
	return PDFPageAppendAnnotationStamp ( Doc, Page, *Rect, Content, ContentLength, Name );
}

PDFAnnotationHandle PDFPageAppendAnnotationLinkWithActionM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *Rect, 
												  PDFActionHandle Action, THighLightMode Mode, ppBool Visible)
{
	return PDFPageAppendAnnotationLinkWithAction ( Doc, Page, *Rect, Action, Mode, Visible );
}
PDFAnnotationHandle PDFPageAppendAnnotationLinkWithDestM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *Rect, 
												 PDFDestinationHandle Destination, THighLightMode Mode, ppBool Visible)
{
	return PDFPageAppendAnnotationLinkWithDest ( Doc, Page, *Rect, Destination, Mode, Visible );
}

PDFAnnotationHandle PDFPageAppendAnnotationStampWithDrawBoxM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *Rect,char * Content, 
															 ppUns32 ContentLength, TAnnotationStampName Name, ppUns32 Resolution, PBXHandle *PBX)
{
	return PDFPageAppendAnnotationStampWithDrawBox ( Doc, Page, *Rect, Content, ContentLength, Name, Resolution, PBX );
}

#endif
/*

PBXHandle PDFAnnotationStampDrawContent ( PDFAnnotationHandle Annot, ppUns32 Resolution)
{
	PDFCosHandle PO, cont, ad, ct;
	PDFStreamHandle nst;
	PPBX bx;
	TPDFRect rect;
		case CosNull:
			_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
			break;
		case CosStream:
			if ( ContentIndex == 0 )
				ct = cont;
			else
				_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
			break;
		case CosArray:
			if ( ContentIndex <= _CosArrayCount ( cont ) ){
				ct = CosArrayItem ( cont, ContentIndex );
				if ( _CosGetType ( ct ) != CosStream )
					_RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
			} else
				_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
			break;
		default:
			_RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
	}
	rect = PDFPageGetBox ( Doc, Page, pbnMediaBox );
	if ( CosGetType ( ad = _CosDictValueByName ( PO, Resources ) ) != CosDict ){
		if (_IsCosDict(( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh )) 
			ad = CosCopy ( Doc, _CosDictValueByName( (( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh, Resources));
		else 
			ad = CosDictNew ( Doc, false, 0 );
		_CosDictAppend ( PO, Resources, ad );
	}
	if ( _CosStreamIsFiltered ( ct ) ){
		nst = ULStrmGetUnFiltered ( _LIB, ct );
		ULStreamClose ( _CosStreamValue ( ct ) );
		_CosStreamValue(ct ) = nst;
		_CosDictAppend ( ct, Length, CosIntNew ( Doc, false, ULStreamGetSize ( nst ) ) );
		_CosStreamIsFiltered( ct ) = false;
		_CosDictRemoveKey ( ct, Fl);
		_CosDictRemoveKey ( ct, Filter);
	} else
		nst = _CosStreamValue ( ct );
	bx = ( PPBX )mmalloc ( _LIB, sizeof ( TPBX ) );
	bx->Doc = Doc;
	bx->Res = ad;
	bx->FS = 1;
	if ( ULStreamGetSize ( nst ) != 0 ){
		ULStreamSetPosition ( nst, ULStreamGetSize ( nst ) );
		ULStreamWriteChar ( nst, '\n' );
	}
	bx->cs = ct;
	bx->Strm = nst;
	bx->Resolution = Resolution;
	bx->BaseLine = false;
	bx->Width = rect.xr;
	bx->Height = rect.yr;
	bx->E2I = 72.0 / (ppReal) Resolution;
	bx->I2E = ( ppReal ) Resolution / 72.0;
	bx->CharSpace = 0.0;
	bx->WordSpace = 0.0;
	bx->HorizontalScaling = 100.0;
	bx->CurrentFontSize = 10.0;
	bx->TextInited = false;
	bx->TextUsed = false;
	bx->GrayUsed = false;
	bx->ColorUsed = false;
	bx->TextAngle = 0.0;
	bx->UnderLine = false;
	bx->StrikeOut = false;
	bx->FontIsChanged = true;
	bx->FontInited = false;
	bx->Unicode = false;
	bx->CurrentFontIndex = ppMAXUns32;
	bx->TY = 0.0;
	bx->TX = 0.0;
	bx->ResourceFontIndex = 0;
	bx->ResourceImageIndex = 0;
	bx->ResourceGStateIndex = 0;
	bx->SaveCount = 0;
	bx->WT.YScale = 1.0;
	bx->WT.XScale = 1.0;
	bx->NextOperatorChar = '\n';
	_CosNullNew( _DOC, bx->AcroformRes );
	CDebugCode (
		bx->DBG = NULL;
	);
	bx->Rotate = PDFPageGetRotateAngle(Doc,Page);
	PBXSetAngle( bx, bx->Rotate );
	return ( PBXHandle ) bx;
}*/