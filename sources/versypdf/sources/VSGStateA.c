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
VSGStateA.c
*********************************************************************/

#include "../VSGStateA.h"
#include "VSAssert.h"
#include "VSLibI.h"
#include "VSMisc.h"
#include "VSCosAI.h"
#include "VSDocAI.h"

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib


ppUns32 PDFExtGraphicStateNew ( PDFDocHandle Doc )
{
    PDFCosHandle obj;
    PDFDocGStateEntry Entry;
	CheckEdition(_LIB, STD);
    if ( !_DOC->GStateList){
        _DOC->GStateList = ULExtListCreate ( _LIB, sizeof ( PDFDocGStateEntry ), 1);
    }
    obj = CosDictNew ( Doc, true, 5 );
    _CosDictAppend ( obj, Type, _CosNameNew ( ExtGState ) );
    Entry.Obj = obj;
    return ULExtListAdd ( _DOC->GStateList, &Entry );
}

void PDFExtGraphicStateSetCTM(PDFDocHandle Doc, ppUns32 GState, ppReal a, ppReal b, ppReal c, ppReal d, ppReal e, ppReal f )
{
	PDFCosHandle A;
	if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	_CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj , CTM, A = CosArrayNew ( Doc, false,6 ) );
	CosArrayAppend ( A, CosRealNew( Doc, false, a) );
	CosArrayAppend ( A, CosRealNew( Doc, false, b) );
	CosArrayAppend ( A, CosRealNew( Doc, false, c) );
	CosArrayAppend ( A, CosRealNew( Doc, false, d) );
	CosArrayAppend ( A, CosRealNew( Doc, false, e) );
	CosArrayAppend ( A, CosRealNew( Doc, false, f) );
}
void PDFExtGraphicStateSetLineWidth ( PDFDocHandle Doc, ppUns32 GState, ppReal LineWidth )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj , LW, CosRealNew ( Doc, false, LineWidth ) );
}

void PDFExtGraphicStateSetLineCap ( PDFDocHandle Doc, ppUns32 GState, TPDFLineCap LineCap )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, LC, CosIntNew ( Doc, false, LineCap ) );
}

void PDFExtGraphicStateSetLineJoin ( PDFDocHandle Doc, ppUns32 GState, TPDFLineJoin LineJoin )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, LJ, CosIntNew ( Doc, false, LineJoin ) );
}

void PDFExtGraphicStateSetMitterLimit ( PDFDocHandle Doc, ppUns32 GState, ppReal MitterLimit )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, ML, CosRealNew ( Doc, false, MitterLimit ) );
}

void PDFExtGraphicStateSetDashPattern ( PDFDocHandle Doc, ppUns32 GState, PDFCosHandle DashPattern )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, D, CosCopy ( Doc, DashPattern ) );
}

void PDFExtGraphicStateSetAlphaIsShape ( PDFDocHandle Doc, ppUns32 GState, ppBool AlphaIsShape )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, AIS, CosBoolNew ( Doc, false, AlphaIsShape ) );
}

void PDFExtGraphicStateSetAlphaStroke ( PDFDocHandle Doc, ppUns32 GState, ppReal AlphaStroke )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, CA, CosRealNew ( Doc, false, AlphaStroke ) );
}

void PDFExtGraphicStateSetAlphaFill ( PDFDocHandle Doc, ppUns32 GState, ppReal AlphaFill )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, ca, CosRealNew ( Doc, false, AlphaFill ) );
}

void PDFExtGraphicStateSetTextKnockout ( PDFDocHandle Doc, ppUns32 GState, ppBool TextKnockout )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, TK, CosBoolNew ( Doc, false, TextKnockout ) );
}

void PDFExtGraphicStateSetOverprintStroke ( PDFDocHandle Doc, ppUns32 GState, ppBool OverprintStroke )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, OP, CosBoolNew ( Doc, false, OverprintStroke ) );
}

void PDFExtGraphicStateSetOverprintFill ( PDFDocHandle Doc, ppUns32 GState, ppBool OverprintFill )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, op, CosBoolNew ( Doc, false, OverprintFill ) );
}

void PDFExtGraphicStateSetOverprintMode ( PDFDocHandle Doc, ppUns32 GState, ppInt32 OverprintMode )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, OPM, CosIntNew ( Doc, false, OverprintMode ) );
}

void PDFExtGraphicStateSetFlatness ( PDFDocHandle Doc, ppUns32 GState, ppReal Flatness )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, FL, CosRealNew ( Doc, false, Flatness ) );
}

void PDFExtGraphicStateSetRenderingIntent ( PDFDocHandle Doc, ppUns32 GState, TPDFRenderingIntents Intent )
{
    PDFCosHandle obj1;
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    switch ( Intent ){
        case ReIntAbsoluteColormetric:
            obj1 = _CosNameNew ( AbsoluteColormetric );
            break;
        case ReIntRelativeColorMetrics:
            obj1 = _CosNameNew ( RelativeColorimetric );
            break;
        case ReIntSaturation:
            obj1 = _CosNameNew ( Saturation );
            break;
        case ReIntPerceptual:
            obj1 = _CosNameNew ( Perceptual );
            break;
        default:

        ;
    };
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, RI, obj1 );
}
void PDFExtGraphicStateSetBlendMode ( PDFDocHandle Doc,ppUns32 GState, TPDFBlendMode BlendMode )
{
    PDFCosHandle obj1;
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    switch ( BlendMode ){
        case blmoNormal:
            obj1 = _CosNameNew ( Normal );
            break;
        case blmoMultiply:
            obj1 = _CosNameNew ( Multiply );
            break;
        case blmoScreen:
            obj1 = _CosNameNew ( Screen );
            break;
        case blmoOverlay:
            obj1 = _CosNameNew ( Overlay );
            break;
        case blmoDarken:
            obj1 = _CosNameNew ( Darken );
            break;
        case blmoLighten:
            obj1 = _CosNameNew ( Lighten );
            break;
        case blmoColorDodge:
            obj1 = _CosNameNew ( ColorDodge );
            break;
        case blmoColorBurn:
            obj1 = _CosNameNew ( ColorBurn );
            break;
        case blmoHardLight:
            obj1 = _CosNameNew ( HardLight );
            break;
        case blmoSoftLight:
            obj1 = _CosNameNew ( SoftLight );
            break;
        case blmoDifference:
            obj1 = _CosNameNew ( Difference );
            break;
        case blmoExclusion:
            obj1 = _CosNameNew ( Exclusion );
            break;
        default:

        ;
    }
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, BM, obj1 );
}

void PDFExtGraphicStateSetSmoothness ( PDFDocHandle Doc, ppUns32 GState, ppReal Smoothness )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, SM, CosRealNew ( Doc, false, Smoothness ) );
}

void PDFExtGraphicStateSetStrokeAdjustment ( PDFDocHandle Doc, ppUns32 GState, ppBool StrokeAdjustment )
{
    if ( !_DOC->GStateList || GState >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    _CosDictAppend ( ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, GState).Obj, ML, CosBoolNew ( Doc, false, StrokeAdjustment ) );
}

