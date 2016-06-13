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
VSPageA.c
*********************************************************************/

#include "../VSPageA.h"
#include "VSLibI.h"
#include "VSFilter.h"
#include "VSMisc.h"
#include "VSDocAI.h"
#include "VSCanvasAI.h"

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

ppUns32 PDFPageGetContentCount(PDFDocHandle Doc, ppUns32 Page)
{
    PDFCosHandle PO, cont;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    cont = _CosDictValueByName ( PO, Contents );
    if ( _CosGetType ( cont ) == CosNull )
        return 0;
    else if ( _CosGetType ( cont ) == CosStream )
        return 1;
    else if ( _CosGetType ( cont ) == CosArray )
        return _CosArrayCount ( cont );
    else
        _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    return 0;
}

void PDFPageSetTumbnail( PDFDocHandle Doc, ppUns32 Page, ppUns32 Image )
{
	PDFCosHandle PO;
	PPDFDocImageEntry Img;
	if ( !_DOC->Pages )
		PDFDocLoadPages ( Doc );
	if ( Page >= _DOC->Pages->Size )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if ( !_DOC->ImageList || Image >= _DOC->ImageList->Size )
		PDFRAISE ( _LIB, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
	Img = ( PPDFDocImageEntry )  _DOC->ImageList->FList[Image];
	_CosDictAppend( PO, Thumb, Img->Obj);
}

PDFCosHandle PDFPageGetCosObject(PDFDocHandle Doc, ppUns32 Page)
{
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    return ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
}

void PDFPageSetRotateAngle(PDFDocHandle Doc, ppUns32 Page, TPDFPageRotateAngle Rotate)
{
    PDFCosHandle PO;
    ppInt32 i;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    i = Rotate * 90;
    _CosDictAppend ( PO, Rotate, CosIntNew ( Doc, false, i ) );
}

TPDFPageRotateAngle PDFPageGetRotateAngle(PDFDocHandle Doc, ppUns32 Page)
{
    PDFCosHandle PO, inh, R;
    ppInt32 i;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    if ( _CosGetType ( PO ) != CosDict )
        _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    inh = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh;
	if (_IsCosDict (inh)){
		R = _CosDictValueByName ( inh, Rotate );
		if ( _CosGetType ( R ) == CosInteger ){
			i = _CosIntValue ( R );
			if ( i > 360 )
				i = i % 360;
			i = i / 90;
			return ( TPDFPageRotateAngle ) i;
		}
	}
    R = _CosDictValueByName ( PO, Rotate );
    if ( _IsCosInt ( R ) ){
        i = _CosIntValue ( R );
        if ( i > 360 )
            i = i % 360;
        i = i / 90;
        return ( TPDFPageRotateAngle ) i;
    } else
        return pra0;
}

TPDFRect PDFPageGetBox(PDFDocHandle Doc, ppUns32 Page, TPDFPageBoxType Type)
{
    PDFCosHandle PO, bx , w, u;
    TPDFRect r;
	ppReal UU;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
	u = _CosDictValueByName ( PO, UserUnit);
	if (_IsCosNumber(u))
		UU = _CosNumGetValue(u);
	else 
		UU = 1.0;
    _CosNullNew ( Doc, bx );
    switch ( Type ){
        case pbnMediaBox:
            if ( _IsCosDict ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh ) )
                bx = _CosDictValueByName ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh,
                                           MediaBox );
            if ( _CosGetType ( bx ) == CosNull )
                bx = _CosDictValueByName ( PO, MediaBox );
            if ( _CosGetType ( bx ) == CosNull ){
                r.xl = 0;
                r.yl = 0;
                r.xr = 595;
                r.yr = 842;
                return r;
            };  
            break;
        case pbnCropBox:
            if ( _IsCosDict ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh ) )
                bx = _CosDictValueByName ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh,
                                           CropBox );
            if ( _CosGetType ( bx ) == CosNull )
                bx = _CosDictValueByName ( PO, CropBox );
            if ( _CosGetType ( bx ) == CosNull )
                return PDFPageGetBox ( Doc, Page, pbnMediaBox );
            break;
        case pbnBleedBox:
            bx = _CosDictValueByName ( PO, BleedBox );
            if ( _CosGetType ( bx ) == CosNull )
                return PDFPageGetBox ( Doc, Page, pbnCropBox );
            break;
        case pbnTrimBox:
            bx = _CosDictValueByName ( PO, TrimBox );
            if ( _CosGetType ( bx ) == CosNull )
                return PDFPageGetBox ( Doc, Page, pbnCropBox );
            break;
        case pbnArtBox:
            bx = _CosDictValueByName ( PO, ArtBox );
            if ( _CosGetType ( bx ) == CosNull )
                return PDFPageGetBox ( Doc, Page, pbnCropBox );
            break;
        default:
    ;
    }
    if ( _CosGetType ( bx ) != CosArray )
        _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    if ( _CosArrayCount ( bx ) != 4 )
        _RAISE ( ErrPageLevel, pleInvalidRectangleValueError );
    w = CosArrayItem ( bx, 0 );
    if ( _CosIsNumber ( w ) )
        r.xl = CosNumberGetValue ( w );
    else
        _RAISE ( ErrPageLevel, pleInvalidRectangleValueError );
    w = CosArrayItem ( bx, 1 );
    if ( _CosIsNumber ( w ) )
        r.yl = CosNumberGetValue ( w );
    else
        _RAISE ( ErrPageLevel, pleInvalidRectangleValueError );
    w = CosArrayItem ( bx, 2 );
    if ( _CosIsNumber ( w ) )
        r.xr = CosNumberGetValue ( w );
    else
        _RAISE ( ErrPageLevel, pleInvalidRectangleValueError );
    w = CosArrayItem ( bx, 3 );
    if ( _CosIsNumber ( w ) )
        r.yr = CosNumberGetValue ( w );
    else
        _RAISE ( ErrPageLevel, pleInvalidRectangleValueError );
	if (UU != 1.0) {
		r.xl = r.xl * UU;
		r.yl = r.yl * UU;
		r.xr = r.xr * UU;
		r.yr = r.yr * UU;
	}
	return r;
}

void PDFPageSetBox(PDFDocHandle Doc, ppUns32 Page, TPDFPageBoxType Type, TPDFRect rect)
{
    PDFCosHandle PO, w;
	ppReal AllMax, UU;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if (Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	AllMax = max(rect.xl,rect.yl);
	AllMax = max(AllMax, rect.xr);
	AllMax = max(AllMax, rect.yr);
	AllMax = _abs(AllMax);
	if (AllMax > 72*200){
		UU = floor( AllMax / (72*200))+1;
	} else {
		UU = 1.0;
	}
	
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
	if (UU == 1.0){
		_CosDictRemoveKey(PO, UserUnit);
	} else {
		_CosDictAppend(PO, UserUnit, CosRealNew(Doc, false, UU));
	}
    w = CosArrayNew ( Doc, false, 4 );
    PDFTRY ( _LIB ){
        CosArrayAppend ( w, CosRealNew ( Doc, false, rect.xl / UU ) );
        CosArrayAppend ( w, CosRealNew ( Doc, false, rect.yl / UU ) );
        CosArrayAppend ( w, CosRealNew ( Doc, false, rect.xr / UU ) );
        CosArrayAppend ( w, CosRealNew ( Doc, false, rect.yr / UU ) );
    } PDFEXCEPT ( _LIB ){
        CosFree ( w );
        PDFRERAISE ( _LIB );
    }PDFTRYEND ( _LIB );
    switch ( Type ){
        case pbnMediaBox:
            if ( _IsCosDict ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh ) )
                _CosDictRemoveKey ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh,
                                    MediaBox );
            _CosDictAppend ( PO, MediaBox, w );
            break;
        case pbnCropBox:
            if ( _IsCosDict ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh ) )
                _CosDictRemoveKey ( ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->Inh,
                                    CropBox );
            _CosDictAppend ( PO, CropBox, w );
            break;
        case pbnBleedBox:
            _CosDictAppend ( PO, BleedBox, w );
            break;
        case pbnTrimBox:
            _CosDictAppend ( PO, TrimBox, w );
            break;
        case pbnArtBox:
            _CosDictAppend ( PO, ArtBox, w );
            break;
        default:
    ;
    }
}
#ifdef DELPHI
void PDFPageSetBoxM(PDFDocHandle Doc, ppUns32 Page, TPDFPageBoxType Type, TPDFRect *rect){
	PDFPageSetBox( Doc, Page, Type, *rect );
}
#endif

ppUns32 PDFPageAddContent(PDFDocHandle Doc, ppUns32 Page)
{
    PDFCosHandle PO, cont, nc, ad, ar;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    nc = CosStreamNew ( Doc, true, 1 );
    _CosDictAppend ( nc, Length, CosIntNew ( Doc, false, 0 ) );
    cont = _CosDictValueByName ( PO, Contents );
    if ( _CosGetType ( cont ) == CosNull ){
        _CosDictAppend ( PO, Contents, nc );
        return 0;
    };
    if ( _CosGetType ( cont ) == CosStream ){
        ad = _CosDictValueByNameNRF ( PO, Contents );
        ar = CosArrayNew ( Doc, false, 2 );
        PDFTRY ( _LIB ){
            CosArrayAppend ( ar, CosCopy ( Doc, ad ) );
            CosArrayAppend ( ar, nc );
        } PDFEXCEPT ( _LIB ){
            CosFree ( ar );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
        _CosDictAppend ( PO, Contents, ar );
        return 1;
    };
    if ( _CosGetType ( cont ) == CosArray ){
        CosArrayAppend ( cont, nc );
        return _CosArrayCount ( cont ) - 1;
    };
    _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    return 0;
}

ppUns32 PDFPageInsertContent(PDFDocHandle Doc, ppUns32 Page, ppUns32 Index)
{
    PDFCosHandle PO, cont, nc, ad, ar;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    if ( _CosGetType ( PO ) != CosDict )
        _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    cont = _CosDictValueByName ( PO, Contents );
    if ( _CosGetType ( cont ) == CosNull ){
        if ( Index == 0 ){
            nc = CosStreamNew ( Doc, true, 1 );
            _CosDictAppend ( nc, Length, CosIntNew ( Doc, false, 0 ) );
            _CosDictAppend ( PO, Contents, nc );
            return 0;
        } else
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    if ( _CosGetType ( cont ) == CosStream ){
        if ( Index == 0 || Index == 1 ){
            nc = CosStreamNew ( Doc, true, 1 );
            _CosDictAppend ( nc, Length, CosIntNew ( Doc, false, 0 ) );
            ad = _CosDictValueByNameNRF ( PO, Contents );
            ar = CosArrayNew ( Doc, false, 2 );
            PDFTRY ( _LIB ){
                if ( Index == 1 ){
                    CosArrayAppend ( ar, CosCopy ( Doc, ad ) );
                    CosArrayAppend ( ar, nc );
                } else{
                    CosArrayAppend ( ar, nc );
                    CosArrayAppend ( ar, CosCopy ( Doc, ad ) );
                }
                _CosDictAppend ( PO, Contents, ar );
            } PDFEXCEPT ( _LIB ){
                CosFree ( ar );
                PDFRERAISE ( _LIB );
            }
            PDFTRYEND ( _LIB )
            return Index;
        } else
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    if ( _CosGetType ( cont ) == CosArray ){
        if ( Index <= _CosArrayCount ( cont ) ){
            nc = CosStreamNew ( Doc, true, 1 );
            _CosDictAppend ( nc, Length, CosIntNew ( Doc, false, 0 ) );
            CosArrayInsert ( cont, nc, Index );
            return Index;
        } else
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    return 0;
}

void PDFPageRemoveContent(PDFDocHandle Doc, ppUns32 Page, ppUns32 Index)
{
    PDFCosHandle PO, cont;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    cont = _CosDictValueByName ( PO, Contents );
    if ( _CosGetType ( cont ) == CosNull )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    if ( _CosGetType ( cont ) == CosStream ){
		if ( Index == 0 ){
            _CosDictRemoveKey ( PO, Contents );
			return;
		} else
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    if ( _CosGetType ( cont ) == CosArray ){
		if ( Index <= _CosArrayCount ( cont ) ){
            CosArrayRemove ( cont, Index );
			return;
		} else
            _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    }
    _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
}

PBXHandle PDFPageCreatePaintBox(PDFDocHandle Doc, ppUns32 Page, ppUns32 XResolution)
{
    ppUns32 i;
    i = PDFPageGetContentCount ( Doc, Page );
    if ( !i )
        i = PDFPageAddContent ( Doc, Page );
    else
        i--;
    return PDFPageCreatePaintBoxFromContent ( Doc, Page, i, XResolution );
}

PBXHandle PDFPageCreatePaintBoxFromContent(PDFDocHandle Doc, ppUns32 Page, ppUns32 ContentIndex,
                                       ppUns32 Resolution)
{
    PDFCosHandle PO, cont, ad, ct = 0, u;
    PDFStreamHandle nst;
    PPBX bx;
	ppReal UU;
    TPDFRect rect;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    if ( Page >= _DOC->Pages->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[Page] )->PO;
    cont = _CosDictValueByName ( PO, Contents );
    switch ( _CosGetType ( cont ) ){
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
	u = _CosDictValueByName ( PO, UserUnit);
	if (_IsCosNumber(u))
		UU = _CosNumGetValue(u);
	else 
		UU = 1.0;
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
    if ( ULStreamGetSize ( nst ) != 0 ){
        ULStreamSetPosition ( nst, ULStreamGetSize ( nst ) );
        ULStreamWriteChar ( nst, '\n' );
    }
    bx->cs = ct;
	bx->FS = UU;
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
	bx->ResourcePatternIndex = 0;
    bx->SaveCount = 0;
    bx->WT.YScale = 1.0;
    bx->WT.XScale = 1.0;
    bx->NextOperatorChar = '\n';
//	bx->EmulateItalic = false;
    _CosNullNew( _DOC, bx->AcroformRes );
    CDebugCode (
        bx->DBG = NULL;
    );
	bx->Rotate = PDFPageGetRotateAngle(Doc,Page);
	PBXSetAngle( bx, bx->Rotate );
    return ( PBXHandle ) bx;
}

PBXHandle PDFPageCreatePaintBoxAndRepackContents( PDFDocHandle Doc, ppUns32 Page, ppUns32 Resolution)
{
	PBXHandle PB, TPB;
	PDFStreamHandle S1, S2;
	ppUns32 L, J;
	PDFPageAddContent( Doc, Page );
	PB = PDFPageCreatePaintBox( Doc, Page, Resolution );
	PBXStateStore( PB );
	S1 = _PBOX(PB)->Strm;
	L = PDFPageGetContentCount( Doc, Page );
	for ( J = 0; J < L -1; J++){
		TPB = PDFPageCreatePaintBoxFromContent( Doc, Page,0, 72 );
		S2 = _PBOX(TPB)->Strm;
		ULStreamSetPosition( S2, 0 );
		ULStreamCopyToStream( S2, S1 );
		PBXClose( TPB, false );
		PDFPageRemoveContent( Doc, Page, 0 );
		PBXAppendLine(PB, "");
	}
	PBXStateRestore( PB );
	return PB;
}