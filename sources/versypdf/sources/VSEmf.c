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
VSEmf.c
*********************************************************************/

#ifdef WINDOWS_PLATFORM
#include  <WinGDI.h>
#endif

#include "VSEmf.h"
#include "../VSFontA.h"
#include "../VSImageA.h"
#include "VSImageI.h"
#include "VSFontAI.h"

#ifdef WINDOWS_PLATFORM

#ifdef _LIB
#undef _LIB
#endif

#define _BOX ((PPBX)Parser->PaintBox)
#define _DOC CD(_BOX->Doc)
#define _LIB _DOC->Lib
#define  PB Parser->PaintBox




POINT PDFEMFGetSize ( PBXHandle PaintBox, HGDIOBJ Metafile ){
    ENHMETAHEADER Header;
    POINT r;
    GetEnhMetaFileHeader( ( HENHMETAFILE ) Metafile, sizeof(Header),&Header);
    r.x = Header.rclBounds.right+1;
    r.y = Header.rclBounds.bottom+1;
    return r;
}

#define IS( X ) ( X )

#define CalcX(X) IS ( ( X-Parser->WOrgX ) * Parser->VExtX / Parser->WExtX + Parser->VOrgX )
#define CalcY(Y) IS ( ( Y-Parser->WOrgY ) * Parser->VExtY / Parser->WExtY + Parser->VOrgY )
#define CalcSizeX(X) IS ( X * Parser->VExtX / Parser->WExtX )
#define CalcSizeY(Y) IS ( Y * Parser->VExtY / Parser->WExtY )



#define PStroke {                                                       \
    if ( Parser->CPen.lopnStyle != PS_NULL)                             \
        PBXStroke ( PB );                                               \
    else                                                                \
        PBXNewPath ( PB);                                               \
}

#define PFillAndStroke {                                                    \
    if (!Parser->IsNullBrush){                                              \
        if ( Parser->CPen.lopnStyle != PS_NULL){                            \
            if (Parser->PolyFillMode)                                       \
                PBXEoFillAndStroke  (PB);                                   \
            else                                                            \
                PBXFillAndStroke ( PB);                                   \
        } else {                                                            \
            if (Parser->PolyFillMode)                                       \
                PBXEoFill (PB);                                             \
            else                                                            \
                PBXFill ( PB);                                              \
        }                                                                   \
    }                                                                       \
    else                                                                    \
        PStroke ;                                                           \
}

void PDFEMFSetXF ( PPDFEmf Parser, XFORM XF )
{
    Parser->CurrentXF = XF;
    PBXSetTransform ( Parser->PaintBox, XF);
}



void PDFEMFSetPenColor(PPDFEmf Parser)
{
    PALETTEENTRY e[20];
    ppUns8 b;
    ppInt32 c;
    HGDIOBJ h;
    if (Parser->CPen.lopnColor > 0xFFFFFF  && (Parser->CPen.lopnColor - 0xFFFFFF) < 21 ){
        h = GetStockObject ( DEFAULT_PALETTE );
        c =  GetPaletteEntries ( ( HPALETTE ) h, 0, 20,e);
        b =(ppUns8) ( Parser->CPen.lopnColor & 0xFF );
        if (c)
            Parser->CPen.lopnColor= RGB(e[b].peRed, e[b].peGreen, e[b].peBlue);
    }
    PBXSetStrokeColor ( PB,ULRGBToColor ( GetRValue ( Parser->CPen.lopnColor ) / 255.0,
        GetGValue ( Parser->CPen.lopnColor ) / 255.0, GetBValue ( Parser->CPen.lopnColor ) / 255.0 ) );
}


void PDFEMFSetInPath ( PPDFEmf Parser, ppBool Value)
{
    Parser->InPath = Value;
    if (Value)
        PBXMoveTo ( PB, CalcX ( Parser->CurVal.x ) ,CalcY ( Parser->CurVal.y ) );
}

static int CALLBACK ULFontTestItalicBack(const LOGFONT *lf,
										   const TEXTMETRIC *tm,
										   ppUns32 FontType, LPARAM IsItalic)
{
	if (lf->lfItalic){
		*( ppBool * ) ( IsItalic ) = true;
		return 0;
	}
	return 1;
}

void PDFEMFSetCurrentFont ( PPDFEmf Parser )
{
    ppBool Bold, Italic, StrikeOut, UnderLine;
    ppInt32 idx;
    ppReal FS;
    HDC DC;
    LOGFONT FNT;
    HGDIOBJ Old, F;
    SIZE SZ1, SZ2;
    ppInt32 i, len;
    TEXTMETRIC tm;
	ppReal DCY;
	LOGFONT lf;
	ppBool IsItalic, CheckItalic;
	char Sym[7] = "SYMBOL";
    if ( !Parser->FontChanged )
        return;
    Parser->FontChanged = false;
	_BOX->EmulateItalic = false;
    UnderLine = Parser->CFont.lfUnderline? true: false;
    StrikeOut = Parser->CFont.lfStrikeOut? true: false;
    len = ( ppInt32 ) strlen ( Parser->CFont.lfFaceName );
    for ( i = 0; i < len; i++ ){
        if ( Parser->CFont.lfFaceName[i] =='?' )
            break;
    }
    if ( i == len ){
		idx = -1;
        Bold = Parser->CFont.lfWeight >= 600 ? true : false;
        Italic = Parser->CFont.lfItalic? true:false; 
		if (len == 6 ){
			for (i=0;i <6;i++){
				if (toupper(Parser->CFont.lfFaceName[i])!= Sym[i])
					break;
			}
			if (i == 6){
				idx = PDFFontStandardAppend( _BOX->Doc,stdfSymbol,etStandardEncoding );
			}
		}
		if (idx < 0 )
			idx = PDFFontTrueTypeAppend ( _BOX->Doc, Parser->CFont.lfFaceName, Bold, Italic );
		CheckItalic = Italic;	
    } else {
        PDFTRY ( _LIB ){
            idx = PDFFontAppendFromDC ( _BOX->Doc, Parser->DC );
        } PDFEXCEPT ( _LIB ){
            Bold = Parser->CFont.lfWeight >= 600 ? true : false;
            Italic = Parser->CFont.lfItalic? true : false;
            idx = PDFFontTrueTypeAppend ( _BOX->Doc, Parser->CFont.lfFaceName, Bold, Italic );
        } PDFTRYEND (_LIB );
		CheckItalic = false;
    }
	DCY = ( ppReal ) GetDeviceCaps ( Parser->DC, LOGPIXELSY );
    if ( Parser->CFont.lfHeight < 0 ){
        FS = - Parser->CFont.lfHeight * 72 / DCY ;
    } else {
		GetTextMetrics ( Parser->DC, &tm );
        FS = tm.tmHeight - tm.tmInternalLeading;
        FS = FS * 72 / DCY ;
    }	
	FS = fabs ( CalcSizeY ( FS ) );
	FS *= DCY/_BOX->Resolution;
    PBXSetActiveFontWithCharset ( PB, idx, FS, Parser->CFont.lfCharSet, UnderLine, StrikeOut );
	if (CheckItalic ){
		DC = CreateCompatibleDC ( Parser->DC );
		memset ( &lf, 0, sizeof ( lf ) );
		lf.lfCharSet = DEFAULT_CHARSET;
		memmove ( &( lf.lfFaceName ), Parser->CFont.lfFaceName, strlen ( Parser->CFont.lfFaceName ) );
		IsItalic = false;
		EnumFontFamiliesEx ( DC, &lf, &ULFontTestItalicBack, ( LPARAM ) ( &IsItalic ), 0 );
		if ( !IsItalic ){
			_BOX->EmulateItalic = true;
		}
		DeleteDC ( DC );
	}
    if ( Parser->CFont.lfWidth ){
        DC = CreateCompatibleDC ( Parser->DC );
        memcpy ( &FNT, &Parser->CFont, sizeof ( FNT ) );
        F = CreateFontIndirect ( &FNT );
        Old = SelectObject ( DC, F );
        GetTextExtentPoint32 ( DC, "A", 1, &SZ1 );
        SelectObject ( DC, Old );
        DeleteObject ( F );
        FNT.lfWidth =  0;
        F = CreateFontIndirect ( &FNT );
        Old = SelectObject ( DC, F );
        GetTextExtentPoint32 ( DC, "A", 1, &SZ2 );
        SelectObject ( DC, Old );
        DeleteObject ( F );
        DeleteDC ( DC );
        PBXSetHorizontalScaling ( PB, ( float ) SZ1.cx / ( float )SZ2.cx * 100.0 );
    } else {
        PBXSetHorizontalScaling ( PB, 100.0 );
    }

}

void PDFEMFSetFontColor(PPDFEmf Parser)
{
    if (Parser->CurFill != Parser->TextColor || Parser->FontChanged){
        Parser->CurFill = Parser->TextColor;
        PBXSetFillColor (PB,ULRGBToColor (GetRValue(Parser->TextColor) / 255.0,
            GetGValue(Parser->TextColor) / 255.0, GetBValue(Parser->TextColor) / 255.0));
    }
}
void PDFEMFSetDash ( PPDFEmf Parser )
{
    if ( Parser->InPath )
        return;
    switch(Parser->CPen.lopnStyle) {
        case PS_SOLID:
        case PS_INSIDEFRAME:
            PBXNoDash(PB);
            break;
        case PS_DASH:
            PBXSetDash(PB,"[4 4] 0" );
            break;
        case PS_DOT:
            PBXSetDash(PB,"[1 1] 0" );
            break;
        case PS_DASHDOT:
            PBXSetDash(PB,"[4 1 1 1] 0" );
            break;
        case PS_DASHDOTDOT:
            PBXSetDash(PB,"[4 1 1 1 1 1] 0" );
            break;
    }
}

void PDFEMFSetMapMode(PPDFEmf Parser, ppInt32 Mode)
{
    switch ( Mode ) {
        case MM_TEXT:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = 1;
            Parser->WExtY = 1;
            Parser->VExtX = 1;
            Parser->VExtY = 1;
            Parser->MapMode = Mode;
            break;
        case MM_HIENGLISH:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = GetDeviceCaps ( _DOC->DC, HORZSIZE ) * 10000 / 254;
            Parser->WExtY = GetDeviceCaps ( _DOC->DC, VERTSIZE ) * 10000 / 254;
            Parser->VExtX = GetDeviceCaps ( _DOC->DC, HORZRES );
            Parser->VExtY = - GetDeviceCaps ( _DOC->DC, VERTRES );
            Parser->MapMode = Mode;
            break;
        case MM_LOENGLISH:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = GetDeviceCaps ( _DOC->DC, HORZSIZE ) * 1000 / 254;
            Parser->WExtY = GetDeviceCaps ( _DOC->DC, VERTSIZE ) * 1000 / 254;
            Parser->VExtX = GetDeviceCaps ( _DOC->DC, HORZRES );
            Parser->VExtY = - GetDeviceCaps ( _DOC->DC, VERTRES );
            Parser->MapMode = Mode;
            break;
        case MM_HIMETRIC:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = GetDeviceCaps ( _DOC->DC, HORZSIZE ) * 100;
            Parser->WExtY = GetDeviceCaps ( _DOC->DC, VERTSIZE ) * 100;
            Parser->VExtX = GetDeviceCaps ( _DOC->DC, HORZRES );
            Parser->VExtY = - GetDeviceCaps ( _DOC->DC, VERTRES );
            Parser->MapMode = Mode;
            break;
        case MM_LOMETRIC:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = GetDeviceCaps ( _DOC->DC, HORZSIZE ) * 10;
            Parser->WExtY = GetDeviceCaps ( _DOC->DC, VERTSIZE ) * 10;
            Parser->VExtX = GetDeviceCaps ( _DOC->DC, HORZRES );
            Parser->VExtY = - GetDeviceCaps ( _DOC->DC, VERTRES );
            Parser->MapMode = Mode;
            break;
        case MM_TWIPS:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = GetDeviceCaps ( _DOC->DC, HORZSIZE ) * 14400 / 254;
            Parser->WExtY = GetDeviceCaps ( _DOC->DC, VERTSIZE ) * 14400 / 254;
            Parser->VExtX = GetDeviceCaps ( _DOC->DC, HORZRES );
            Parser->VExtY = -GetDeviceCaps ( _DOC->DC, VERTRES );
            Parser->MapMode = Mode;
            break;
        case MM_ISOTROPIC:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = 1;
            Parser->WExtY = 1;
            Parser->VExtX = 1;
            Parser->VExtY = 1;
            Parser->MapMode = Mode;
            break;
        case MM_ANISOTROPIC:
            Parser->WOrgX = 0;
            Parser->WOrgY = 0;
            Parser->VOrgX = 0;
            Parser->VOrgY = 0;
            Parser->WExtX = 1;
            Parser->WExtY = 1;
            Parser->VExtX = 1;
            Parser->VExtY = 1;
            Parser->MapMode = Mode;
            break;
        default:;
    }
}

void PDFEMFSetBkColor ( PPDFEmf Parser ){
    if ( Parser->CurFill != Parser->BkColor){
        Parser->CurFill = Parser->BkColor;
        PBXSetFillColor ( PB,ULRGBToColor ( GetRValue ( Parser->CurFill ) / 255.0,
            GetGValue ( Parser->CurFill ) / 255.0, GetBValue ( Parser->CurFill ) / 255.0 ) );
    }
}


void PDFEMFSetBrushColor(PPDFEmf Parser, ppBool Check){
    PALETTEENTRY e[20];
    ppUns8 b;
    ppInt32 c;
    HGDIOBJ h;
    if ( Parser->CBrush.lbColor > 0xFFFFFF  && ( Parser->CBrush.lbColor - 0xFFFFFF ) < 21 ){
        h = GetStockObject ( DEFAULT_PALETTE );
        c =  GetPaletteEntries ( ( HPALETTE ) h, 0, 20,e);
        b =(ppUns8) ( Parser->CBrush.lbColor & 0xFF );
        if (c)
            Parser->CBrush.lbColor= RGB(e[b].peRed, e[b].peGreen, e[b].peBlue);
    }
    if (Parser->CurFill!=Parser->CBrush.lbColor || !Check){
        Parser->CurFill = Parser ->CBrush.lbColor;
        PBXSetFillColor (PB,ULRGBToColor (GetRValue(Parser->CBrush.lbColor) / 255.0,
            GetGValue(Parser->CBrush.lbColor) / 255.0, GetBValue(Parser->CBrush.lbColor) / 255.0));
    }
}

void PDFEMFSetInText(PPDFEmf Parser, ppBool InText){
    if (Parser->InText == InText) 
        return;
    Parser->InText = InText;
    if (! InText){
        PDFEMFSetBrushColor(Parser, true);
    }

}

void PDFEMFDoSetWindowExtEx(PPDFEmf Parser,PEMRSETVIEWPORTEXTEX Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetWindowExtEx \n\tData->szlExtent.cx = %i\n\tData->szlExtent.cy = %i\n",
            Parser->CurRec, Data->szlExtent.cx, Data->szlExtent.cy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->WExtX = Data->szlExtent.cx;
    Parser->WExtY = Data->szlExtent.cy;
    SetWindowExtEx ( Parser->DC, Data->szlExtent.cx, Data->szlExtent.cy, NULL);
}

void PDFEMFDoSetWindowOrgEx( PPDFEmf Parser, PEMRSETVIEWPORTORGEX Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetWindowOrgEx\n\tData->ptlOrigin.x = %i\n\tData->ptlOrigin.y = %i\n",
            Parser->CurRec, Data->ptlOrigin.x, Data->ptlOrigin.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->WOrgX = Data->ptlOrigin.x;
    Parser->WOrgY = Data->ptlOrigin.y;
    SetWindowOrgEx ( Parser->DC, Data->ptlOrigin.x, Data->ptlOrigin.y, NULL);
}
void PDFEMFDoSetViewPortExtEx( PPDFEmf Parser,PEMRSETVIEWPORTEXTEX Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetViewPortExtEx\n\tData->szlExtent.cx = %i\n\tData->szlExtent.cy = %i\n",
          Parser->CurRec, Data->szlExtent.cx, Data->szlExtent.cy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->VExtX = Data->szlExtent.cx;
    Parser->VExtY = Data->szlExtent.cy;
    SetViewportExtEx ( Parser->DC, Data->szlExtent.cx, Data->szlExtent.cy, NULL);
}
void PDFEMFDoSetViewPortOrgEx( PPDFEmf Parser,PEMRSETVIEWPORTORGEX Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetViewPortOrgEx\n\tData->ptlOrigin.x = %i\n\tData->ptlOrigin.y = %i\n",
            Parser->CurRec, Data->ptlOrigin.x, Data->ptlOrigin.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->VOrgX = Data->ptlOrigin.x;
    Parser->VOrgY = Data->ptlOrigin.y;
    SetViewportOrgEx ( Parser->DC, Data->ptlOrigin.x, Data->ptlOrigin.y, NULL);
}

void PDFEMFDoSetMapMode( PPDFEmf Parser,PEMRSETMAPMODE Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetMapMode\n\tData->iMode = ",
            Parser->CurRec, Data->iMode );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        switch ( Data->iMode) {
            case MM_TEXT:
                ULStrToStrm ( Parser->DBG, "MM_TEXT\n" );
                break;
            case MM_HIENGLISH:
                ULStrToStrm ( Parser->DBG, "MM_HIENGLISH\n" );
                break;
            case MM_LOENGLISH:
                ULStrToStrm ( Parser->DBG, "MM_LOENGLISH\n" );
                break;
            case MM_HIMETRIC:
                ULStrToStrm ( Parser->DBG, "MM_HIMETRIC\n" );
                break;
            case MM_LOMETRIC:
                ULStrToStrm ( Parser->DBG, "MM_LOMETRIC\n" );
                break;
            case MM_TWIPS:
                ULStrToStrm ( Parser->DBG, "MM_TWIPS\n" );
                break;
            case MM_ISOTROPIC:
                ULStrToStrm ( Parser->DBG, "MM_ISOTROPIC\n" );
                break;
            case MM_ANISOTROPIC:
                ULStrToStrm ( Parser->DBG, "MM_ANISOTROPIC\n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "\n" );;
        }
    );
    Parser->FontChanged = true;
    PDFEMFSetMapMode ( Parser, Data->iMode );
    SetMapMode ( Parser->DC, Data->iMode );
}
void PDFEMFDoPolyBezier( PPDFEmf Parser,PEMRPOLYLINE Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyBezier\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i, Data->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cptl >= 4 ){
        PBXMoveTo ( PB, CalcX ( Data->aptl[0].x ), CalcY ( Data->aptl[0].y ) );
        for ( i=1; i<= ( Data->cptl - 1 ) / 3; i++  ){
            PBXCurveTo ( PB, CalcX ( Data->aptl[1 + (i - 1) * 3].x ), CalcY ( Data->aptl[1 + (i - 1) * 3].y ),
                CalcX ( Data->aptl[1 + (i - 1) * 3 + 1].x ), CalcY ( Data->aptl[1 + (i - 1) * 3 + 1].y ),
                CalcX ( Data->aptl[1 + (i - 1) * 3 + 2].x ), CalcY ( Data->aptl[1 + (i - 1) * 3 + 2].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i:\n\t\t( %i, %i)\n\t\t( %i, %i)\n\t\t( %i, %i) \n",
                    i, Data->aptl[1 + (i - 1) * 3].x , Data->aptl[1 + (i - 1) * 3].y,
                    Data->aptl[1 + (i - 1) * 3 + 1].x , Data->aptl[1 + (i - 1) * 3 + 1].y,
                    Data->aptl[1 + (i - 1) * 3 + 2].x , Data->aptl[1 + (i - 1) * 3 + 2].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        if ( !Parser->InPath ) 
            PStroke;
    }
}

void PDFEMFDoPolygon( PPDFEmf Parser,PEMRPOLYLINE Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Polygon:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cptl){
        PBXNewPath ( PB );
        PBXMoveTo ( PB, CalcX ( Data->aptl[0].x ), CalcY ( Data->aptl[0].y ) );
        for (i=1; i< Data->cptl; i++ ){
            PBXLineTo ( PB, CalcX ( Data->aptl[i].x ), CalcY ( Data->aptl[i].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->aptl[i].x , Data->aptl[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        if ( !Parser->InPath ) {
            PBXClosePath ( PB );
            PFillAndStroke;
        }
    }
}

void PDFEMFDoPolyLine( PPDFEmf Parser,PEMRPOLYLINE Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyLine:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i, Data->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cptl){
        PBXNewPath ( PB );
        PBXMoveTo ( PB, CalcX ( Data->aptl[0].x ), CalcY ( Data->aptl[0].y ) );
        for (i=1; i< Data->cptl; i++ ){
            PBXLineTo ( PB, CalcX ( Data->aptl[i].x ), CalcY ( Data->aptl[i].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->aptl[i].x , Data->aptl[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        if ( !Parser->InPath ) 
            PFillAndStroke;
    }
}
void PDFEMFDoPolyBezierTo( PPDFEmf Parser,PEMRPOLYLINE Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyBezierTo:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cptl = %i\n",
        Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
        Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl );
    ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cptl >= 3 ){
        if ( !Parser->InPath ) 
            PBXMoveTo ( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );
        for ( i=1; i<=  Data->cptl / 3; i++  ){
            PBXCurveTo ( PB, CalcX ( Data->aptl[(i - 1) * 3].x ), CalcY ( Data->aptl[(i - 1) * 3].y ),
            CalcX ( Data->aptl[(i - 1) * 3 + 1].x ), CalcY ( Data->aptl[(i - 1) * 3 + 1].y ),
            CalcX ( Data->aptl[(i - 1) * 3 + 2].x ), CalcY ( Data->aptl[(i - 1) * 3 + 2].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: \n\t\t( %i, %i)\n\t\t( %i, %i)\n\t\t( %i, %i) \n",
                  i, Data->aptl[1 + (i - 1) * 3].x , Data->aptl[1 + (i - 1) * 3].y,
                    Data->aptl[1 + (i - 1) * 3 + 1].x , Data->aptl[1 + (i - 1) * 3 + 1].y,
                    Data->aptl[1 + (i - 1) * 3 + 2].x , Data->aptl[1 + (i - 1) * 3 + 2].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        Parser->CurVal.x = Data->aptl[i-1].x;
        Parser->CurVal.y = Data->aptl[i-1].y;
        if ( !Parser->InPath ) 
            PStroke;
    }
}

void PDFEMFDoPolyLineTo( PPDFEmf Parser,PEMRPOLYLINE Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyLineTo:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cptl){
        if ( !Parser->InPath ){
            PBXNewPath ( PB );
            PBXMoveTo ( PB, CalcX(Parser->CurVal.x),CalcY(Parser->CurVal.y));
        }       
        for (i=0; i< Data->cptl; i++ ){
            PBXLineTo ( PB, CalcX ( Data->aptl[i].x ), CalcY ( Data->aptl[i].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->aptl[i].x , Data->aptl[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        if ( !Parser->InPath ) 
            PStroke;
        Parser->CurVal.x = Data->aptl[i-1].x;
        Parser->CurVal.y = Data->aptl[i-1].y;

    }
}

void PDFEMFDoPolyPolyLine( PPDFEmf Parser,PEMRPOLYPOLYLINE Data)
{
    ppUns32 i, j, k = 0;
    PPOINTL P;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyPolyLine:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->nPolys = %i, Data->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->nPolys, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    P = ( PPOINTL ) ( & ( Data->aPolyCounts[ Data->nPolys ] ) );
    for ( i = 0; i < Data->nPolys; i++){
        PBXMoveTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
        CDebugCode (
            sprintf ( Parser->DBGString, "\tPolygon %i\n", i );
            ULStrToStrm ( Parser->DBG, Parser->DBGString );
        );
        k++;
        for (j = 1 ; j < Data->aPolyCounts[i]; j++){
            PBXLineTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\tIteration %i: ( %i, %i) \n",
                    j, P[k].x , P[k].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        }
    }
    if ( !Parser->InPath )
        PStroke;
}

void PDFEMFDoPolyPolyGon( PPDFEmf Parser,PEMRPOLYPOLYLINE Data)
{
    ppUns32 i, j, k = 0, l;
    PPOINTL P;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyPolyGon:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->nPolys = %i\n\tData->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->nPolys, Data->cptl );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    P = ( PPOINTL ) ( & ( Data->aPolyCounts[ Data->nPolys ] ) );
    for ( i = 0; i < Data->nPolys; i++){
        PBXMoveTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
        l = k;
        k++;
        CDebugCode (
            sprintf ( Parser->DBGString, "\tPolygon %i\n", i );
            ULStrToStrm ( Parser->DBG, Parser->DBGString );
        );
        for (j = 1 ; j < Data->aPolyCounts[i]; j++){
            PBXLineTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\tIteration %i: ( %i, %i) \n",
                    j, P[k].x , P[k].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        }
        PBXLineTo ( PB, CalcX ( P[l].x ), CalcY ( P[l].y ) );
    }
    if ( !Parser->InPath )
        PFillAndStroke;
}

void PDFEMFDoSetPixelV( PPDFEmf Parser,PEMRSETPIXELV Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetPixelV:\n\tColor (Red: %i; Green: %i, Blue: %i)\n\tData->ptlPixel.x = %i Data->ptlPixel.y = %i\n",
            Parser->CurRec, GetRValue ( Data->crColor ), GetGValue ( Data->crColor ), 
            GetBValue ( Data->crColor ), Data->ptlPixel.x, Data->ptlPixel.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXNewPath ( PB );
    if ( Data->crColor != Parser->CPen.lopnColor )
        PBXSetStrokeColor ( PB,  ULRGBToColor ( GetRValue ( Data->crColor ) / 255.0,
        GetGValue( Data->crColor ) / 255.0, GetBValue( Data->crColor ) / 255.0 ) );
    if ( Parser->CPen.lopnWidth.x != 1)
        PBXSetLineWidth( PB, CalcSizeX ( 1 ) );
    PBXMoveTo ( PB, CalcX ( Data->ptlPixel.x ), CalcY( Data->ptlPixel.y ) );
    PBXLineTo ( PB, CalcX (Data->ptlPixel.x ) + 0.01, CalcY ( Data->ptlPixel.y ) + 0.01 );
    PStroke;
    if ( Parser->CPen.lopnWidth.x != 1 ){
        if ( ! Parser->CPen.lopnWidth.x ) 
            PBXSetLineWidth ( PB, 1.0 );
        else
            PBXSetLineWidth( PB, CalcSizeX ( Parser->CPen.lopnWidth.x ) );
    }
    if ( Data->crColor != Parser->CPen.lopnColor )
        PBXSetStrokeColor ( PB,  ULRGBToColor ( GetRValue ( Parser->CPen.lopnColor ) / 255.0,
        GetGValue( Parser->CPen.lopnColor ) / 255.0, GetBValue( Parser->CPen.lopnColor ) / 255.0 ) );
}

void PDFEMFDoSetBKMode( PPDFEmf Parser,PEMRSELECTCLIPPATH Data)
{
    CDebugCode (
        Data->iMode == OPAQUE ?
            sprintf ( Parser->DBGString, "%i: SetBkMode:\n\tMode = %s\n",
                Parser->CurRec, "Opaque" ):
            sprintf ( Parser->DBGString, "%i: SetBkMode:\n\tMode = %s\n",
                Parser->CurRec, "Transparent" );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    SetBkMode (Parser->DC, Data->iMode);
    Parser->BkMode = Data->iMode== TRANSPARENT ? false : true;
}

void PDFEMFDoSetPolyFillMode( PPDFEmf Parser,PEMRSELECTCLIPPATH Data)
{
    CDebugCode (
        Data->iMode == ALTERNATE ?
            sprintf ( Parser->DBGString, "%i: SetPolyFillMode:\n\tMode = %s \n",
                Parser->CurRec, "Alternate" ):
            sprintf ( Parser->DBGString, "%i: SetPolyFillMode:\n\tMode = %s \n",
                Parser->CurRec, "Winding" );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->PolyFillMode =  Data->iMode == ALTERNATE ? true : false;
    SetPolyFillMode ( Parser->DC, Data->iMode );
}

void PDFEMFDoSetTextAlign( PPDFEmf Parser,PEMRSELECTCLIPPATH Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetTextAlign:\n\t",
            Parser->CurRec );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    SetTextAlign ( Parser->DC, Data->iMode);
    switch(Data->iMode &(TA_LEFT | TA_RIGHT | TA_CENTER)) {
        case TA_LEFT:
            Parser->HorMode = hjLeft;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Left\n\t" );
            );
            break;
        case TA_RIGHT:
            Parser->HorMode = hjRight;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Right\n\t" );
            );
            break;
        case TA_CENTER:
            Parser->HorMode = hjCenter;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Center\n\t" );
            );
            break;
    }
    switch(Data->iMode &(TA_TOP | TA_BASELINE | TA_BOTTOM)) {
        case TA_TOP:
            Parser->VertMode = vjTop;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Top\n\t" );
            );
            break;
        case TA_BASELINE:
            Parser->VertMode = vjCenter;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "BaseLine\n\t" );
            );
            break;
        case TA_BOTTOM:
            Parser->VertMode = vjBottom;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Botton\n\t" );
            );
            break;
    }
    Parser->UpdatePos = (Data->iMode & TA_UPDATECP) == TA_UPDATECP ? true: false;
    CDebugCode (
        (Data->iMode & TA_UPDATECP) == TA_UPDATECP ?
            ULStrToStrm ( Parser->DBG, "UpdatePosition \n" ):
            ULStrToStrm ( Parser->DBG, " \n" );
    );

}

void PDFEMFDoSetTextColor( PPDFEmf Parser,PEMRSETTEXTCOLOR Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetTextColor:\n\tColor (Red: %i; Green: %i, Blue: %i) \n",
            Parser->CurRec, GetRValue ( Data->crColor ), GetGValue ( Data->crColor ), 
            GetBValue ( Data->crColor ));
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->TextColor = Data->crColor;
    SetTextColor ( Parser->DC, Data->crColor);
    if (Parser->InText)
        PDFEMFSetFontColor(Parser);
}

void PDFEMFDoSetBKColor( PPDFEmf Parser,PEMRSETTEXTCOLOR Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetBkColor:\n\tColor (Red: %i; Green: %i, Blue: %i) \n",
            Parser->CurRec, GetRValue ( Data->crColor ), GetGValue ( Data->crColor ), 
            GetBValue ( Data->crColor ));
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    SetBkColor ( Parser->DC, Data->crColor);
    Parser->BkColor = Data->crColor;
}

void PDFEMFDoMoveToEx( PPDFEmf Parser,PEMRLINETO Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: MoveToEx:\n\tData->ptl.x = %i\n\tData->ptl.y = %i \n",
            Parser->CurRec, Data->ptl.x, Data->ptl.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->CurVal.x = Data->ptl.x;
    Parser->CurVal.y = Data->ptl.y;
    if (Parser->InPath){
        if (Parser->InText)
            PDFEMFSetInText(Parser, false);
        PBXMoveTo(PB,CalcX(Data->ptl.x), CalcY(Data->ptl.y));
    }
}                            

void PDFEMFDoInterSectClipRect( PPDFEmf Parser,PEMRINTERSECTCLIPRECT Data)
{
    TPDFRect r;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: InterSectClipRect:\n\tData->rclClip.left = %i\n\tData->rclClip.top = %i\n\tData->rclClip.right = %i\n\tData->rclClip.bottom = %i\n",
            Parser->CurRec, Data->rclClip.left, Data->rclClip.top, Data->rclClip.right, Data->rclClip.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Parser->Clipping ){
        PBXStateRestore ( PB );
        Parser->FontChanged = true;
        PDFEMFSetPenColor ( Parser );
        PDFEMFSetBrushColor ( Parser, false );
        PDFEMFSetDash ( Parser );
        if ( ! Parser->CPen.lopnWidth.x ) 
            PBXSetLineWidth ( PB, 1.0 );
        else
            PBXSetLineWidth ( PB, CalcSizeX ( Parser->CPen.lopnWidth.x ) );
    }
    PBXStateStore ( PB );
    Parser->Clipping = true;
    r.xl = CalcX ( Data->rclClip.left );
    r.xr = CalcX ( Data->rclClip.right );
    r.yl = CalcY ( Data->rclClip.top );
    r.yr = CalcY ( Data->rclClip.bottom );
    PBXNewPath ( PB );
    PBXRectangle ( PB, r.xl, r.yl, r.xr, r.yr );
    Parser->IsCR = true;
    Parser->ClipRect = Data->rclClip;
    PBXClip ( PB );
    PBXNewPath ( PB );
}

void PDFEMFDoExcludeClipRect( PPDFEmf Parser,PEMREXCLUDECLIPRECT Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: ExcludeClipRect:\n\tData->rclClip.left = %i\n\tData->rclClip.top = %i\n\tData->rclClip.right = %i\n\tData->rclClip.bottom = %i\n",
            Parser->CurRec, Data->rclClip.left, Data->rclClip.top, Data->rclClip.right, Data->rclClip.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    return;
}

void PDFEMFDoExtSelectClipRGN( PPDFEmf Parser,PEMREXTSELECTCLIPRGN Data)
{
    PRGNDATA RGNs;
    ppUns32 i;
    PRECT RCT;

    CDebugCode (
        sprintf ( Parser->DBGString, "%i: ExtSelectClipRGN\n\tMode = ", Parser->CurRec );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        switch ( Data->iMode ){
            case RGN_AND:
                ULStrToStrm ( Parser->DBG, "RGN_AND\n" );
                break;
            case RGN_COPY:
                ULStrToStrm ( Parser->DBG, "RGN_COPY\n" );
                break;
            case RGN_DIFF:
                ULStrToStrm ( Parser->DBG, "RGN_DIFF\n" );
                break;
            case RGN_OR:
                ULStrToStrm ( Parser->DBG, "RGN_OR\n" );
                break;
            case RGN_XOR:
                ULStrToStrm ( Parser->DBG, "RGN_XOR\n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "\n" );          
        }
    );
    if ( Parser->Clipping ){
        PBXStateRestore ( PB );
        Parser->FontChanged = true;
        PDFEMFSetPenColor ( Parser );
        PDFEMFSetBrushColor ( Parser, false );
        PDFEMFSetDash ( Parser );
        if ( ! Parser->CPen.lopnWidth.x ) 
            PBXSetLineWidth ( PB, 1.0 );
        else
            PBXSetLineWidth ( PB, CalcSizeX ( Parser->CPen.lopnWidth.x ) );
    }
    if ( Data->cbRgnData){
        PBXStateStore ( PB );
        Parser->Clipping = true;
        PBXNewPath ( PB );
        RGNs = ( PRGNDATA ) ( & ( Data->RgnData[0] ) );
        _BOX->WT.UseSkip = true;
        for ( i = 0; i < RGNs->rdh.nCount; i++ ){
            RCT = ( PRECT ) ( & ( RGNs->Buffer[sizeof ( RECT ) * i]) );
            PBXRectangle ( PB, IS ( RCT->left ), IS (RCT->top ), IS ( RCT->right ), IS ( RCT->bottom ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tRegion %i:\n\t\t( %i %i %i %i ) \n",
                    i, RCT->left, RCT->top , RCT->right, RCT->bottom);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        };           
        _BOX->WT.UseSkip = false; 
        Parser->IsCR = false;
        PBXClip ( PB );
        PBXNewPath ( PB );
    }
}

void PDFEMFDoSaveDC( PPDFEmf Parser)
{
    SaveDC ( Parser->DC );
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SaveDC\n ", Parser->CurRec );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    )
    if ( !Parser->NT ){
        if ( Parser->TSS == Parser->TSC ){
            Parser->TSC += CalculateDelta ( Parser->TSC );
            Parser->TransformStack = ( XFORM * ) mrealloc ( _LIB, Parser->TransformStack,
                sizeof ( XFORM ) * Parser->TSC  );
        }
        Parser->TransformStack[Parser->TSS++] = Parser->CurrentXF;
    }
}

void PDFEMFDoRestoreDC( PPDFEmf Parser,PEMRRESTOREDC Data)
{
    ppInt32 i;
    DWORD l;
	ppUns32 I;

    POINT p;
    SIZE sz;
    LOGBRUSH NBrush;
    LOGPEN NPen;
    LOGFONT NFont;
    HGDIOBJ H;
    XFORM XF;
    RestoreDC ( Parser->DC, Data->iRelative );
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: RestoreDC:\n\tData->iRelative = %i\n ",
            Parser->CurRec, Data->iRelative );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    )
/* WorldTransform */
    if ( Parser->NT ){
        GetWorldTransform ( Parser->DC, &XF );
        PDFEMFSetXF ( Parser, XF );
    } else {
        if ( Data->iRelative < 0 ){
            i = Parser->TSS + Data->iRelative;
            if ( i >=0 ){
                PDFEMFSetXF ( Parser, Parser->TransformStack[i] );
                Parser->TSS = i;
            } else {
                Parser->TSS = 0;
            }
        } else {
            if ( Parser->TSS > Data->iRelative ){
                PDFEMFSetXF ( Parser, Parser->TransformStack[Data->iRelative - 1] );
                Parser->TSS = Data->iRelative - 1;
            } else 
                Parser->TSS = 0;
        }
    }
    CDebugCode (
        sprintf ( Parser->DBGString, "\tWorldTransform: xform.eM11 = %f, xform.eM12 = %f, xform.eM21 = %f, xform.eM22 = %f, xform.eDx = %f , xform.eDy = %f\n ",
            Parser->CurrentXF.eM11, Parser->CurrentXF.eM12, Parser->CurrentXF.eM21, 
            Parser->CurrentXF.eM22, Parser->CurrentXF.eDx, Parser->CurrentXF.eDy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    )
    if ( Parser->Clipping ){
        Parser->Clipping = false;
        Parser->FontChanged = true;
        PDFEMFSetInText ( Parser, false );
        PBXStateRestore ( PB );
    }
    PDFEMFSetMapMode ( Parser, GetMapMode ( Parser->DC ) );
    CDebugCode (
        ULStrToStrm ( Parser->DBG, "\tMapMode = " );
        switch ( Parser->MapMode ) {
            case MM_TEXT:
                ULStrToStrm ( Parser->DBG, "MM_TEXT\n" );
                break;
            case MM_HIENGLISH:
                ULStrToStrm ( Parser->DBG, "MM_HIENGLISH\n" );
                break;
            case MM_LOENGLISH:
                ULStrToStrm ( Parser->DBG, "MM_LOENGLISH\n" );
                break;
            case MM_HIMETRIC:
                ULStrToStrm ( Parser->DBG, "MM_HIMETRIC\n" );
                break;
            case MM_LOMETRIC:
                ULStrToStrm ( Parser->DBG, "MM_LOMETRIC\n" );
                break;
            case MM_TWIPS:
                ULStrToStrm ( Parser->DBG, "MM_TWIPS\n" );
                break;
            case MM_ISOTROPIC:
                ULStrToStrm ( Parser->DBG, "MM_ISOTROPIC\n" );
                break;
            case MM_ANISOTROPIC:
                ULStrToStrm ( Parser->DBG, "MM_ANISOTROPIC\n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "\n" );;
        }
    );
    Parser->StretchBltMode = GetStretchBltMode ( Parser->DC );
    Parser->BkMode = GetBkMode ( Parser->DC )== TRANSPARENT ? false : true;
    CDebugCode (
        !Parser->BkMode ?
            sprintf ( Parser->DBGString, "\tBkMode = %s \n", "Opaque" ):
            sprintf ( Parser->DBGString, "\tBkMode =  %s\n", "Transparent" );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    Parser->CCW = GetArcDirection ( Parser->DC) == AD_COUNTERCLOCKWISE ? true : false;
    CDebugCode (
        sprintf ( Parser->DBGString, "\tArcDirection  = %s\n",
            Parser->CCW ? "AD_COUNTERCLOCKWISE" : "AD_CLOCKWISE");
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->TextColor = GetTextColor ( Parser->DC );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tTextColor (Red: %i; Green: %i, Blue: %i) \n",
            GetRValue ( Parser->TextColor ), GetGValue ( Parser->TextColor ), 
            GetBValue ( Parser->TextColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Parser->InText )
        PDFEMFSetFontColor ( Parser );
    Parser->BkColor = GetBkColor ( Parser->DC );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tBkColor (Red: %i; Green: %i, Blue: %i) \n",
            GetRValue ( Parser->BkColor ), GetGValue ( Parser->BkColor ), 
            GetBValue ( Parser->BkColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    Parser->PolyFillMode = GetPolyFillMode ( Parser-> DC ) == ALTERNATE ? true : false;
    CDebugCode (
        Parser->PolyFillMode ?
            sprintf ( Parser->DBGString, "\tPolyFillMode = %s \n", "Alternate" ):
            sprintf ( Parser->DBGString, "\tPolyFillMode = %s \n", "Winding" );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    GetViewportExtEx ( Parser->DC, &sz );
    Parser->VExtX = sz.cx;
    Parser->VExtY = sz.cy;
    CDebugCode (
        sprintf ( Parser->DBGString, "\tViewPortExtEx:\n\t\tcx = %i\n\t\tcy = %i\n", sz.cx, sz.cy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    GetWindowExtEx ( Parser->DC, &sz );
    Parser->WExtX = sz.cx;
    Parser->WExtY = sz.cy;
    CDebugCode (
        sprintf ( Parser->DBGString, "\tWindowExtEx:\n\t\tcx = %i\n\t\tcy = %i\n", sz.cx, sz.cy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    GetViewportOrgEx ( Parser->DC, &p );
    Parser->VOrgX = p.x;
    Parser->VOrgY = p.y;
    CDebugCode (
        sprintf ( Parser->DBGString, "\tViewPortOrgEx:\n\t\t x = %i\n\t\ty = %i\n", p.x, p.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    GetWindowOrgEx ( Parser->DC, &p );
    Parser->WOrgX = p.x;
    Parser->WOrgY = p.y;
    CDebugCode (
        sprintf ( Parser->DBGString, "\tWindowOrgEx:\n\t\tx = %i\n\t\ty = %i\n", p.x, p.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    /* Check Pen */
    H = GetCurrentObject ( Parser->DC, OBJ_PEN );
    GetObject ( H, sizeof ( NPen ), &NPen );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tPen :\n\t\tWidth = %i\n\t\tColor (Red: %i; Green: %i, Blue: %i)\n\t\tStyle = ",
            NPen.lopnWidth, GetRValue ( NPen.lopnColor ), GetGValue ( NPen.lopnColor ), 
            GetBValue ( NPen.lopnColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        switch ( NPen.lopnStyle){
            case PS_SOLID:
                ULStrToStrm ( Parser->DBG, "PS_SOLID \n" );
                break;
            case PS_DASH:
                ULStrToStrm ( Parser->DBG, "PS_DASH \n" );
                break;
            case PS_DOT:
                ULStrToStrm ( Parser->DBG, "PS_DOT \n" );
                break;
            case PS_DASHDOT:
                ULStrToStrm ( Parser->DBG, "PS_DASHDOT \n" );
                break;
            case PS_DASHDOTDOT:
                ULStrToStrm ( Parser->DBG, "PS_DASHDOTDOT \n" );
                break;
            case PS_NULL:
                ULStrToStrm ( Parser->DBG, "PS_NULL \n" );
                break;
            case PS_INSIDEFRAME:
                ULStrToStrm ( Parser->DBG, "PS_INSIDEFRAME \n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "\n" );
    }
    );
    if ( NPen.lopnColor != Parser->CPen.lopnColor ){
        Parser->CPen.lopnColor = NPen.lopnColor;
        PDFEMFSetPenColor ( Parser );
    }
    if (NPen.lopnStyle != Parser->CPen.lopnStyle){
		if ((NPen.lopnStyle & PS_JOIN_MASK) != (Parser->CPen.lopnStyle & PS_JOIN_MASK)){
			switch (NPen.lopnStyle & PS_JOIN_MASK) {
				case PS_JOIN_ROUND:
					PBXSetLineJoin(PB,ljRound);
					break;
				case PS_JOIN_BEVEL:
					PBXSetLineJoin(PB,ljBevel);
					break;
				case PS_JOIN_MITER:
					PBXSetLineJoin(PB,ljMiter);
					break;
			}
		}
		if ((NPen.lopnStyle & PS_ENDCAP_MASK) != (Parser->CPen.lopnStyle & PS_ENDCAP_MASK)){
			switch (NPen.lopnStyle & PS_ENDCAP_MASK) {
				case PS_ENDCAP_ROUND:
					PBXSetLineCap(PB,lcRound);
					break;
				case PS_ENDCAP_FLAT:
					PBXSetLineCap(PB,lcButtEnd);
					break;
				case PS_ENDCAP_SQUARE:
					PBXSetLineCap(PB,lcProjectingSquare);
					break;
			}
		}
		if ((NPen.lopnStyle & PS_STYLE_MASK) != (Parser->CPen.lopnStyle & PS_STYLE_MASK)){
			Parser->CPen.lopnStyle = NPen.lopnStyle;
			PDFEMFSetDash ( Parser );
		}
		Parser->CPen.lopnStyle = NPen.lopnStyle;
	}
    if ( NPen.lopnWidth.x != Parser->CPen.lopnWidth.x ){
        Parser->CPen.lopnWidth.x = NPen.lopnWidth.x;
        if ( ! NPen.lopnWidth.x ) 
            PBXSetLineWidth ( PB, 1.0 );
        else
            PBXSetLineWidth ( PB, CalcSizeX(NPen.lopnWidth.x ) );
    }  
    /* Check Brush */
    H = GetCurrentObject ( Parser->DC, OBJ_BRUSH );
    GetObject ( H, sizeof ( NBrush ),&NBrush );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tBrush :\n\t\t Color (Red: %i; Green: %i, Blue: %i) \n",
            GetRValue ( NBrush.lbColor ), GetGValue ( NBrush.lbColor ), GetBValue ( NBrush.lbColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        ULStrToStrm ( Parser->DBG, "\tStyle = " );
        switch ( NBrush.lbStyle){
            case BS_DIBPATTERN:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERN \n" );
                break;
            case BS_DIBPATTERN8X8:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERN8X8 \n" );
                break;
            case BS_DIBPATTERNPT:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERNPT \n" );
                break;
            case BS_HATCHED:
                ULStrToStrm ( Parser->DBG, "BS_HATCHED \n" );
                break;
            case BS_NULL:
                ULStrToStrm ( Parser->DBG, "BS_NULL \n" );
                break;
            case BS_PATTERN:
                ULStrToStrm ( Parser->DBG, "BS_PATTERN \n" );
                break;
            case BS_PATTERN8X8:
                ULStrToStrm ( Parser->DBG, "BS_PATTERN8X8 \n" );
                break;
            case BS_SOLID:
                ULStrToStrm ( Parser->DBG, "BS_SOLID \n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "Other \n" );
        }
        ULStrToStrm ( Parser->DBG, "\tHatch = " );
        switch ( NBrush.lbHatch){
            case HS_BDIAGONAL:
                ULStrToStrm ( Parser->DBG, "HS_BDIAGONAL \n" );
                break;
            case HS_CROSS:
                ULStrToStrm ( Parser->DBG, "HS_CROSS \n" );
                break;
            case HS_DIAGCROSS:
                ULStrToStrm ( Parser->DBG, "HS_DIAGCROSS \n" );
                break;
            case HS_FDIAGONAL:
                ULStrToStrm ( Parser->DBG, "HS_FDIAGONAL \n" );
                break;
            case HS_HORIZONTAL:
                ULStrToStrm ( Parser->DBG, "HS_HORIZONTAL \n" );
                break;
            case HS_VERTICAL:
                ULStrToStrm ( Parser->DBG, "HS_VERTICAL \n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "Other \n" );
        }
    );

	if ((NBrush.lbStyle==BS_DIBPATTERN) || (NBrush.lbStyle==BS_DIBPATTERN8X8)){
		for (I=0; I < Parser->PatternsSize;I++){
			if (Parser->Patterns[I].Bitmap == (PVOID)NBrush.lbHatch){
				PBXEndText(PB);
				PBXSetPattern(PB,Parser->Patterns[I].Pattern);
				break;
			}
		}
	} else {
		if ( Parser->CBrush.lbColor != NBrush.lbColor ){
			Parser->CBrush.lbColor = NBrush.lbColor;
			if ( !Parser->InPath )
				PDFEMFSetBrushColor ( Parser, true );
		}
		Parser->IsNullBrush = NBrush.lbStyle == 1 ? true : false;
	}
    /* Check Font */
    H = GetCurrentObject ( Parser->DC, OBJ_FONT );
    GetObject ( H, sizeof ( NFont ), &NFont );
    CDebugCode (
        ULStrToStrm ( Parser->DBG, "\tFont :\n" );
        sprintf ( Parser->DBGString, "\t\tLogFont.lfHeight = %i\n\t\tLogFont.lfWidth = %i\n\t\tLogFont.lfEscapement = %i\n\t\tLogFont.lfOrientation = %i\n\t\tLogFont.lfWeight = %i\n\t\tLogFont.lfItalic = %i\n", 
            NFont.lfHeight, NFont.lfWidth,
            NFont.lfEscapement, NFont.lfOrientation,
            NFont.lfWeight, NFont.lfItalic);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString,"\t\tLogFont.lfUnderline = %i\n\t\tLogFont.lfStrikeOut = %i\n\t\tLogFont.lfCharSet = %i\n\t\tLogFont.lfOutPrecision = %i\n\t\tLogFont.lfClipPrecision = %i\n\t\tLogFont.lfQuality =%i\n\t\tLogFont.lfPitchAndFamily = %i\n",
            NFont.lfUnderline, NFont.lfStrikeOut,
            NFont.lfCharSet, NFont.lfOutPrecision,
            NFont.lfClipPrecision, NFont.lfQuality,
            NFont.lfPitchAndFamily );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString,"\t\tLogFont.lfFaceName = %s\n", NFont.lfFaceName);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( strcmp ( NFont.lfFaceName, Parser->CFont.lfFaceName) ||
        NFont.lfItalic != Parser->CFont.lfItalic ||
        NFont.lfWeight != Parser->CFont.lfWeight ||
        NFont.lfCharSet != Parser->CFont.lfCharSet ||
        NFont.lfStrikeOut != Parser->CFont.lfStrikeOut ||
        NFont.lfUnderline, Parser->CFont.lfUnderline ||
        NFont.lfHeight, Parser->CFont.lfHeight){
            memmove ( &Parser->CFont, &NFont, sizeof ( NFont ) );
            Parser->FontChanged =  true;
        } else {
            if ( NFont.lfOrientation != Parser->CFont.lfOrientation ||
                NFont.lfEscapement != Parser->CFont.lfEscapement )
                memmove ( &Parser->CFont, &NFont, sizeof ( NFont ) );
        }

    /* Text allign */
    l = GetTextAlign ( Parser->DC );
    CDebugCode (
        ULStrToStrm ( Parser->DBG, "\tTextAlign: (" );
    );
    switch ( l &(TA_LEFT | TA_RIGHT | TA_CENTER)) {
        case TA_LEFT:
            Parser->HorMode = hjLeft;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Left, " );
            );
            break;
        case TA_RIGHT:
            Parser->HorMode = hjRight;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Right, " );
            );
            break;
        case TA_CENTER:
            Parser->HorMode = hjCenter;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Center, " );
            );
            break;
    }
    switch ( l &(TA_TOP | TA_BASELINE | TA_BOTTOM)) {
        case TA_TOP:
            Parser->VertMode = vjTop;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Top " );
            );
            break;
        case TA_BASELINE:
            Parser->VertMode = vjCenter;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "BaseLine " );
            );
            break;
        case TA_BOTTOM:
            Parser->VertMode = vjBottom;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "Botton " );
            );
            break;
    }
    Parser->UpdatePos = ( l & TA_UPDATECP ) == TA_UPDATECP ? true: false;
    CDebugCode (
        ( l & TA_UPDATECP) == TA_UPDATECP ?
            ULStrToStrm ( Parser->DBG, ", UpdatePosition )\n" ):
            ULStrToStrm ( Parser->DBG, ") \n" );
    );
}

void PDFEMFDoSetWorldTransform( PPDFEmf Parser,PEMRSETWORLDTRANSFORM Data)
{
    PDFEMFSetXF (Parser, Data->xform ); 
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetWorldTransform:\n\tData->xform.eM11 = %f\n\tData->xform.eM12 = %f\n\tData->xform.eM21 = %f\n\tData->xform.eM22 = %f\n\tData->xform.eDx = %f\n\tData->xform.eDy = %f\n ",
            Parser->CurRec, Data->xform.eM11, Data->xform.eM12, Data->xform.eM21, Data->xform.eM22, Data->xform.eDx, Data->xform.eDy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    )
    if ( Parser->NT )
        SetWorldTransform ( Parser->DC, & Data->xform );
 
}

void PDFEMFDoModifyWorldTransform( PPDFEmf Parser,PEMRMODIFYWORLDTRANSFORM Data)
{
    XFORM XF;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: ModifyWorldTransform:\n\tData->xform.eM11 = %f\n\tData->xform.eM12 = %f\n\tData->xform.eM21 = %f\n\tData->xform.eM22 = %f\n\tData->xform.eDx = %f\n\tData->xform.eDy = %f\n ",
        Parser->CurRec, Data->xform.eM11, Data->xform.eM12, Data->xform.eM21, Data->xform.eM22, Data->xform.eDx, Data->xform.eDy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        switch ( Data->iMode ){
            case MWT_LEFTMULTIPLY:
                ULStrToStrm ( Parser->DBG, "\tMode: MWT_LEFTMULTIPLY\n" );
                break;
            case MWT_RIGHTMULTIPLY:
                ULStrToStrm ( Parser->DBG, "\tMode: MWT_RIGHTMULTIPLY\n" );
                break;
            case MWT_IDENTITY:
                ULStrToStrm ( Parser->DBG, "\tMode: MWT_IDENTITY\n" );
                break;
            default:
                sprintf ( Parser->DBGString, "\tMode: Other (%i)\n ", Data->iMode );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
        }    
    )
    if ( Parser->NT ){
        ModifyWorldTransform ( Parser->DC, & Data->xform, Data->iMode );
        GetWorldTransform ( Parser->DC, &XF );
        PDFEMFSetXF ( Parser, XF );

    } else {
        switch ( Data->iMode ){
            case MWT_LEFTMULTIPLY:
                PDFEMFSetXF (Parser, ULMultiplyXFORM ( Data->xform, Parser->CurrentXF ) );
                break;
            case MWT_RIGHTMULTIPLY:
                PDFEMFSetXF (Parser, ULMultiplyXFORM (  Parser->CurrentXF, Data->xform ) );
                break;
            case 4:
                PDFEMFSetXF (Parser, Data->xform ); 
                break;
        }
    }
}

void PDFEMFDoSelectObject( PPDFEmf Parser,PEMRSELECTOBJECT Data)
{
    DWORD Type, I;
    LOGPEN NPen;
    LOGBRUSH NBrush;
    LOGFONT NFont;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SelectObject :\n\tData->ihObject = %i\n",
            Parser->CurRec, Data->ihObject );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( ! ( Data->ihObject & 0x80000000 ) ){
        if ( Data->ihObject >= Parser->HandlesCount )
            return;
        SelectObject ( Parser->DC, Parser->HandleTable[Data->ihObject] );
        Type = GetObjectType ( Parser->HandleTable[Data->ihObject] );
        switch ( Type ) {
            case OBJ_PEN:
                GetObject ( Parser->HandleTable[Data->ihObject],sizeof ( NPen ),&NPen );
                if (NPen.lopnColor != Parser->CPen.lopnColor ){
                    Parser->CPen.lopnColor = NPen.lopnColor;
                    PDFEMFSetPenColor(Parser);
                }
                if (NPen.lopnStyle != Parser->CPen.lopnStyle){
					if ((NPen.lopnStyle & PS_JOIN_MASK) != (Parser->CPen.lopnStyle & PS_JOIN_MASK)){
						switch (NPen.lopnStyle & PS_JOIN_MASK) {
							case PS_JOIN_ROUND:
								PBXSetLineJoin(PB,ljRound);
								break;
							case PS_JOIN_BEVEL:
								PBXSetLineJoin(PB,ljBevel);
								break;
							case PS_JOIN_MITER:
								PBXSetLineJoin(PB,ljMiter);
								break;
						}
					}
					if ((NPen.lopnStyle & PS_ENDCAP_MASK) != (Parser->CPen.lopnStyle & PS_ENDCAP_MASK)){
						switch (NPen.lopnStyle & PS_ENDCAP_MASK) {
							case PS_ENDCAP_ROUND:
								PBXSetLineCap(PB,lcRound);
								break;
							case PS_ENDCAP_FLAT:
								PBXSetLineCap(PB,lcButtEnd);
								break;
							case PS_ENDCAP_SQUARE:
								PBXSetLineCap(PB,lcProjectingSquare);
								break;
						}
					}
					if ((NPen.lopnStyle & PS_STYLE_MASK) != (Parser->CPen.lopnStyle & PS_STYLE_MASK)){
						Parser->CPen.lopnStyle = NPen.lopnStyle;
						PDFEMFSetDash ( Parser );
					}
					Parser->CPen.lopnStyle = NPen.lopnStyle;			
                }
                if ( NPen.lopnWidth.x != Parser->CPen.lopnWidth.x ){
                    Parser->CPen.lopnWidth.x = NPen.lopnWidth.x;
                    if ( ! NPen.lopnWidth.x ) 
                        PBXSetLineWidth ( PB, 1.0 );
                    else
                        PBXSetLineWidth ( PB, CalcSizeX(NPen.lopnWidth.x ) );
                }
                break;
            case OBJ_BRUSH:
				Parser->IsNullBrush = false;
                GetObject ( Parser->HandleTable[Data->ihObject], sizeof ( NBrush ),&NBrush );
				if ((NBrush.lbStyle==BS_DIBPATTERN) || (NBrush.lbStyle==BS_DIBPATTERN8X8)){
					for (I=0; I < Parser->PatternsSize;I++){
						if (Parser->Patterns[I].Bitmap == (PVOID)NBrush.lbHatch){
							PBXEndText(PB);
							PBXSetPattern(PB,Parser->Patterns[I].Pattern);
							break;
						}
					}
				} else {
					if ( Parser->CBrush.lbColor != NBrush.lbColor ){
						Parser->CBrush.lbColor = NBrush.lbColor;
						if ( !Parser->InPath )
							PDFEMFSetBrushColor ( Parser, true );
					}
					Parser->IsNullBrush = NBrush.lbStyle == 1 ? true : false;
				}
                
                break;
            case OBJ_FONT:
                GetObject ( Parser->HandleTable[Data->ihObject], sizeof ( NFont ), &NFont );
                if ( strcmp ( NFont.lfFaceName, Parser->CFont.lfFaceName) ||
                    NFont.lfItalic != Parser->CFont.lfItalic ||
                    NFont.lfWeight != Parser->CFont.lfWeight ||
                    NFont.lfCharSet != Parser->CFont.lfCharSet ||
                    NFont.lfStrikeOut != Parser->CFont.lfStrikeOut ||
                    NFont.lfUnderline != Parser->CFont.lfUnderline ||
                    NFont.lfHeight != Parser->CFont.lfHeight){
                        memmove ( &Parser->CFont, &NFont, sizeof ( NFont ) );
                        Parser->FontChanged =  true;
                    } else {
                        if ( NFont.lfOrientation != Parser->CFont.lfOrientation ||
                            NFont.lfEscapement != Parser->CFont.lfEscapement )
                            memmove ( &Parser->CFont, &NFont, sizeof ( NFont ) );
                    }
                break;
        }
    } else {
        I = Data->ihObject & 0x7FFFFFFF;
        SelectObject ( Parser->DC, GetStockObject ( I ) );
        switch ( I ) {
            case WHITE_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: WHITE_BRUSH\n" );
                );
                Parser->IsNullBrush = false;
                Parser->CBrush.lbColor = 0xFFFFFF;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case LTGRAY_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: LTGRAY_BRUSH\n" );
                );
                Parser->IsNullBrush = false;
                Parser->CBrush.lbColor = 0xAAAAAA;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case GRAY_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: GRAY_BRUSH\n" );
                );
                Parser->IsNullBrush = false;
                Parser->CBrush.lbColor = 0x808080;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case DKGRAY_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: DKGRAY_BRUSH\n" );
                );
                Parser->IsNullBrush = false;
                Parser->CBrush.lbColor = 0x666666;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case BLACK_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: BLACK_BRUSH\n" );
                );
                Parser->IsNullBrush = false;
                Parser->CBrush.lbColor = 0;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case NULL_BRUSH:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: NULL_BRUSH\n" );
                );
                Parser->IsNullBrush = true;
                Parser->CBrush.lbColor = 0xFFFFFF;
                if ( !Parser->InText )
                    PDFEMFSetBrushColor ( Parser, true );
                break;
            case WHITE_PEN:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: WHITE_PEN\n" );
                );
                Parser->CPen.lopnColor =  0xFFFFFF;
                Parser->CPen.lopnStyle = PS_SOLID;
                Parser->CPen.lopnWidth.x = 1;
                PBXSetLineWidth ( PB, CalcSizeX( 1 ) );
                PDFEMFSetDash ( Parser );
                PBXSetStrokeColor ( PB, ULRGBToColor ( 1.0, 1.0, 1.0 ) );
                break;
            case BLACK_PEN:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: BLACK_PEN\n" );
                );
                Parser->CPen.lopnColor =  0;
                Parser->CPen.lopnStyle = PS_SOLID;
                Parser->CPen.lopnWidth.x = 1;
                PBXSetLineWidth ( PB, CalcSizeX( 1 ) );
                PDFEMFSetDash ( Parser );
                PBXSetStrokeColor ( PB, ULRGBToColor ( 0.0, 0.0, 0.0 ) );
                break;
            case NULL_PEN:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: NULL_PEN\n" );
                );
                Parser->CPen.lopnStyle = PS_NULL;
                Parser->CPen.lopnWidth.x = 1;
                PBXSetLineWidth ( PB, CalcSizeX( 1 ) );
                break;
            case OEM_FIXED_FONT:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: OEM_FIXED_FONT\n" );
                );
                memmove ( Parser->CFont.lfFaceName, "Arial", 5 );
                Parser->FontChanged =  true;
                break;
            case ANSI_FIXED_FONT:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: ANSI_FIXED_FONT\n" );
                );
                memmove ( Parser->CFont.lfFaceName, "Arial", 5 );
                Parser->FontChanged =  true;
                break;
            case ANSI_VAR_FONT:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: ANSI_VAR_FONT\n" );
                );
                memmove ( Parser->CFont.lfFaceName, "Arial", 5 );
                Parser->FontChanged =  true;
                break;
            case SYSTEM_FONT:
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\tStandard style: SYSTEM_FONT\n" );
                );
                memmove ( Parser->CFont.lfFaceName, "Arial", 5 );
                Parser->FontChanged =  true;
                break;
        }
    }
}

void PDFEMFDoCreatePen( PPDFEmf Parser,PEMRCREATEPEN Data)
{
    if ( Data->ihPen>=Parser->HandlesCount ) 
        return;
    if ( Parser->HandleTable[Data->ihPen] ) 
        DeleteObject(Parser->HandleTable[Data->ihPen]);
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: CreatePen : \n\tData->ihPen = %i\n\tWidth = %i\n",
            Parser->CurRec, Data->ihPen, Data->lopn.lopnWidth );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tColor (Red: %i Green: %i Blue: %i)\n\tStyle = ",
            GetRValue ( Data->lopn.lopnColor ), GetGValue ( Data->lopn.lopnColor ), GetBValue ( Data->lopn.lopnColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        switch ( Data->lopn.lopnStyle){
            case PS_SOLID:
                ULStrToStrm ( Parser->DBG, "PS_SOLID \n" );
                break;
            case PS_DASH:
                ULStrToStrm ( Parser->DBG, "PS_DASH \n" );
                break;
            case PS_DOT:
                ULStrToStrm ( Parser->DBG, "PS_DOT \n" );
                break;
            case PS_DASHDOT:
                ULStrToStrm ( Parser->DBG, "PS_DASHDOT \n" );
                break;
            case PS_DASHDOTDOT:
                ULStrToStrm ( Parser->DBG, "PS_DASHDOTDOT \n" );
                break;
            case PS_NULL:
                ULStrToStrm ( Parser->DBG, "PS_NULL \n" );
                break;
            case PS_INSIDEFRAME:
                ULStrToStrm ( Parser->DBG, "PS_INSIDEFRAME \n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "\n" );
        }
    );
    Parser->HandleTable[Data->ihPen]= CreatePen(Data->lopn.lopnStyle, Data->lopn.lopnWidth.x,
        Data->lopn.lopnColor);
}

void PDFEMFDoCreateBrushInDirect( PPDFEmf Parser,PEMRCREATEBRUSHINDIRECT Data)
{
    LOGBRUSH lb;
    if ( Data->ihBrush>=Parser->HandlesCount ) 
        return;
    if ( Parser->HandleTable[Data->ihBrush] ) 
        DeleteObject ( Parser->HandleTable[Data->ihBrush] );
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: CreateBrushInDirect :\n\tData->ihBrush = %i\n\tColor (Red: %i; Green: %i, Blue: %i)\n\t",
            Parser->CurRec, Data->ihBrush, GetRValue ( Data->lb.lbColor ), GetGValue ( Data->lb.lbColor ), GetBValue ( Data->lb.lbColor ) );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        ULStrToStrm ( Parser->DBG, "Style = " );
        switch ( Data->lb.lbStyle){
            case BS_DIBPATTERN:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERN " );
                break;
            case BS_DIBPATTERN8X8:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERN8X8 " );
                break;
            case BS_DIBPATTERNPT:
                ULStrToStrm ( Parser->DBG, "BS_DIBPATTERNPT " );
                break;
            case BS_HATCHED:
                ULStrToStrm ( Parser->DBG, "BS_HATCHED " );
                break;
            case BS_NULL:
                ULStrToStrm ( Parser->DBG, "BS_NULL " );
                break;
            case BS_PATTERN:
                ULStrToStrm ( Parser->DBG, "BS_PATTERN " );
                break;
            case BS_PATTERN8X8:
                ULStrToStrm ( Parser->DBG, "BS_PATTERN8X8 " );
                break;
            case BS_SOLID:
                ULStrToStrm ( Parser->DBG, "BS_SOLID " );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "Other " );
        }
        ULStrToStrm ( Parser->DBG, "\n\tHatch = " );
        switch ( Data->lb.lbHatch){
            case HS_BDIAGONAL:
                ULStrToStrm ( Parser->DBG, "HS_BDIAGONAL \n" );
                break;
            case HS_CROSS:
                ULStrToStrm ( Parser->DBG, "HS_CROSS \n" );
                break;
            case HS_DIAGCROSS:
                ULStrToStrm ( Parser->DBG, "HS_DIAGCROSS \n" );
                break;
            case HS_FDIAGONAL:
                ULStrToStrm ( Parser->DBG, "HS_FDIAGONAL \n" );
                break;
            case HS_HORIZONTAL:
                ULStrToStrm ( Parser->DBG, "HS_HORIZONTAL \n" );
                break;
            case HS_VERTICAL:
                ULStrToStrm ( Parser->DBG, "HS_VERTICAL \n" );
                break;
            default:
                ULStrToStrm ( Parser->DBG, "Other \n" );
        }
    );
    if (Data->lb.lbStyle==BS_SOLID)
        Parser->HandleTable[Data->ihBrush]= CreateSolidBrush(Data->lb.lbColor);
    else{
        lb.lbColor = Data->lb.lbColor;
        lb.lbHatch = Data->lb.lbHatch;
        lb.lbStyle = Data->lb.lbStyle;
        Parser->HandleTable[Data->ihBrush] = CreateBrushIndirect(&lb);
    }
     
}
void PDFEMFDoDeleteObject( PPDFEmf Parser,PEMRDELETEOBJECT Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: DeleteObject :\n\tData->ihObject = %i \n",
            Parser->CurRec, Data->ihObject);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->ihObject>=Parser->HandlesCount ) 
        return;
    if ( Parser->HandleTable[Data->ihObject] ) 
        DeleteObject(Parser->HandleTable[Data->ihObject]);
    Parser->HandleTable[Data->ihObject] = NULL;
}

void PDFEMFDoAngleArc( PPDFEmf Parser,PEMRANGLEARC Data)
{
    float AN = Data->eStartAngle + Data->eSweepAngle;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: AngleArc :\n\tData->ptlCenter.x = %i\n\tData->ptlCenter.y = %i\n\tData->nRadius = %i\n\tData->eStartAngle = %f\n\tData->eSweepAngle = %f\n",
            Parser->CurRec, Data->ptlCenter.x, Data->ptlCenter.y, Data->nRadius, Data->eStartAngle, Data->eSweepAngle);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXMoveTo( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );
    PBXLineTo( PB, 
        CalcX ( Data->ptlCenter.x + cos ( Data ->eStartAngle * Pi / 180 ) * Data->nRadius ),
        CalcY( Data->ptlCenter.y - sin ( Data->eStartAngle * Pi / 180 ) * Data->nRadius ) );
    if ( fabs ( Data->eSweepAngle ) >= 360 ){
        PBXEllipse ( PB, 
            CalcX ( Data->ptlCenter.x - Data->nRadius ), CalcY ( Data->ptlCenter.y - Data->nRadius ),
            CalcX ( Data->ptlCenter.x + Data->nRadius ), CalcY ( Data->ptlCenter.y + Data->nRadius ) );
    } else {
        PBXArc2( PB, 
            CalcX ( Data->ptlCenter.x - Data->nRadius ), CalcY ( Data->ptlCenter.y - Data->nRadius ),
            CalcX ( Data->ptlCenter.x + Data->nRadius ), CalcY ( Data->ptlCenter.y + Data->nRadius ),
            Data->eStartAngle, AN );
    }      
    Parser->CurVal.x = ( LONG ) ( Data->ptlCenter.x + cos ( AN * Pi / 180 ) * Data->nRadius );
    Parser->CurVal.y = ( LONG ) ( Data->ptlCenter.y - sin ( AN * Pi / 180 ) * Data->nRadius );
    if ( !Parser->InPath )
        PStroke;
}

void PDFEMFDoEllipse( PPDFEmf Parser,PEMRELLIPSE Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Ellipse :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, 
            Data->rclBox.right, Data->rclBox.bottom );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXEllipse ( PB, CalcX ( Data->rclBox.left ), CalcY ( Data->rclBox.top ),
        CalcX ( Data->rclBox.right ), CalcY ( Data->rclBox.bottom ) );
    if ( !Parser->InPath )
        PFillAndStroke;
}

void PDFEMFDoRectangle( PPDFEmf Parser,PEMRELLIPSE Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Rectangle :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, 
            Data->rclBox.right, Data->rclBox.bottom );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if (Data->rclBox.bottom == Data->rclBox.top || Data->rclBox.left == Data->rclBox.right){
        PBXNewPath( PB );
        return;
    }
    PBXRectangle( PB, CalcX ( Data->rclBox.left ),
        CalcY ( Data->rclBox.top ), CalcX ( Data->rclBox.right ),
        CalcY ( Data->rclBox.bottom ) );
    if ( !Parser->InPath )
        PFillAndStroke;
}

void PDFEMFDoRoundRect( PPDFEmf Parser,PEMRROUNDRECT Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: RoundRect :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n\tData->szlCorner.cx = %i\n\tData->szlCorner.cy = %i \n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, 
            Data->rclBox.right, Data->rclBox.bottom, Data->szlCorner.cx, Data->szlCorner.cy );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXRoundRect ( PB, CalcX ( Data->rclBox.left ),
        CalcY ( Data->rclBox.top ), CalcX ( Data->rclBox.right ),
        CalcY ( Data->rclBox.bottom ), CalcSizeX ( Data->szlCorner.cx ),
        CalcSizeY ( Data->szlCorner.cy ) );
    if ( !Parser->InPath )
        PFillAndStroke;
}

void PDFEMFDoFillRGN( PPDFEmf Parser,PEMRFILLRGN Data)
{
    PRGNDATA RGNs;
    ppUns32 i;
    PRECT RCT;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: FillRGN :\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cbRgnData = %i\n",
        Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
        Data->rclBounds.right, Data->rclBounds.bottom, Data->cbRgnData );
    ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cbRgnData){
        PBXNewPath ( PB );
        RGNs = ( PRGNDATA ) ( & ( Data->RgnData[0] ) );
        for ( i = 0; i < RGNs->rdh.nCount; i++ ){
            RCT = ( PRECT ) ( & ( RGNs->Buffer[sizeof ( RECT ) * i]) );
            PBXRectangle ( PB, CalcX ( RCT->left ), CalcY ( RCT->top ), 
                CalcX ( RCT->right ), CalcY ( RCT->bottom ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tRectangle %i:\n\t\tBox.left = %i\n\t\tBox.top = %i\n\t\tBox.right = %i\n\t\tBox.bottom = %i\n",
                    i, RCT->left, RCT->top, RCT->right, RCT->bottom );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        };           
        if (!Parser->IsNullBrush) 
            PBXFill(PB);
        else
            PBXNewPath(PB);
    }
}

void PDFEMFDoArc( PPDFEmf Parser,PEMRARC Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Arc :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, Data->rclBox.right, Data->rclBox.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tData->ptlStart.x = %i\n\tData->ptlStart.y = %i\n\tData->ptlEnd.x = %i\n\tData->ptlEnd.y = %i\n",
            Data->ptlStart.x, Data->ptlStart.y, Data->ptlEnd.x, Data->ptlEnd.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    PBXArc ( PB, CalcX ( Data->rclBox.left ), CalcY ( Data->rclBox.top ), 
        CalcX ( Data->rclBox.right ), CalcY ( Data->rclBox.bottom ),
        CalcX ( Data->ptlStart.x ), CalcY ( Data->ptlStart.y ),
        CalcX ( Data->ptlEnd.x ), CalcY ( Data->ptlEnd.y ) );
    if ( !Parser->InPath ) 
        PBXStroke ( PB );
}

void PDFEMFDoChord( PPDFEmf Parser,PEMRCHORD Data)
{
    TPDFRealPoint P;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Chord :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, Data->rclBox.right, Data->rclBox.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tData->ptlStart.x = %i\n\tData->ptlStart.y = %i\n\tData->ptlEnd.x = %i\n\tData->ptlEnd.y = %i\n",
            Data->ptlStart.x, Data->ptlStart.y, Data->ptlEnd.x, Data->ptlEnd.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    P = PBXIntArc ( PB, CalcX ( Data->rclBox.left ), CalcY ( Data->rclBox.top ), 
        CalcX ( Data->rclBox.right ), CalcY ( Data->rclBox.bottom ),
        CalcX ( Data->ptlStart.x ), CalcY ( Data->ptlStart.y ),
        CalcX ( Data->ptlEnd.x ), CalcY ( Data->ptlEnd.y ) );
    
    PBXLineTo ( PB, CalcX ( P.x ) , CalcY ( P.y ) );
    if ( !Parser->InPath ) 
        PBXStroke ( PB );

}

void PDFEMFDoPie( PPDFEmf Parser,PEMRPIE Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Pie :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, Data->rclBox.right, Data->rclBox.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tData->ptlStart.x = %i\n\tData->ptlStart.y = %i\n\tData->ptlEnd.x = %i\n\tData->ptlEnd.y = %i\n",
            Data->ptlStart.x, Data->ptlStart.y, Data->ptlEnd.x, Data->ptlEnd.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXPie ( PB, CalcX ( Data->rclBox.left ), CalcY ( Data->rclBox.top ),
        CalcX ( Data->rclBox.right ), CalcY ( Data->rclBox.bottom ),
        CalcX ( Data->ptlStart.x ), CalcY ( Data->ptlStart.y ),
        CalcX ( Data->ptlEnd.x ), CalcY ( Data->ptlEnd.y ) );
    if ( !Parser->InPath )
        PFillAndStroke;
}

void PDFEMFDoLineTo( PPDFEmf Parser,PEMRLINETO Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: LineToEx:\n\tData->ptl.x = %i\n\tData->ptl.y = %i \n",
           Parser->CurRec, Data->ptl.x, Data->ptl.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if (!Parser->InPath)
        PBXMoveTo(PB,CalcX(Parser->CurVal.x), CalcY(Parser->CurVal.y));
    Parser->CurVal.x =  Data->ptl.x;
    Parser->CurVal.y =  Data->ptl.y;
    PBXLineTo(PB,CalcX(Parser->CurVal.x), CalcY(Parser->CurVal.y));
    if ( ! Parser->InPath )
        PBXStroke(PB);
}

void PDFEMFDoArcTo( PPDFEmf Parser,PEMRARC Data)
{
    float CenterX, CenterY, RadiusX, RadiusY;
    float StartAngle, EndAngle;
    LONG t;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: ArcTo :\n\tData->rclBox.left = %i\n\tData->rclBox.top = %i\n\tData->rclBox.right = %i\n\tData->rclBox.bottom = %i\n",
            Parser->CurRec, Data->rclBox.left, Data->rclBox.top, Data->rclBox.right, Data->rclBox.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tData->ptlStart.x = %i\n\tData->ptlStart.y = %i\n\tData->ptlEnd.x = %i\n\tData->ptlEnd.y = %i\n",
            Data->ptlStart.x, Data->ptlStart.y, Data->ptlEnd.x, Data->ptlEnd.y);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXMoveTo ( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );
    if (! Parser->CCW ){
        t = Data->ptlStart.x;
        Data->ptlStart.x = Data->ptlEnd.x;
        Data->ptlEnd.x = t;
        t = Data->ptlStart.y;
        Data->ptlStart.y = Data->ptlEnd.y;
        Data->ptlEnd.y = t;
    }
    CenterX = ( Data->rclBox.left + Data->rclBox.right) / ( float ) 2;
    CenterY = ( Data->rclBox.top + Data->rclBox.bottom) / ( float ) 2;
    RadiusX = ( float ) fabs ( Data->rclBox.left - Data->rclBox.right  / ( float ) 2 );
    RadiusY = ( float ) fabs ( Data->rclBox.top - Data->rclBox.bottom  / ( float ) 2 ) ;
    StartAngle = ( float ) atan2( - ( Data->ptlStart.y - CenterY ) * RadiusX, 
        ( Data->ptlStart.x - CenterX ) * RadiusY );
    EndAngle = ( float ) atan2 ( - ( Data->ptlEnd.y - CenterY ) * RadiusX,
        ( Data->ptlEnd.x - CenterX ) * RadiusY );
    PBXLineTo( PB, CalcX ( CenterX + RadiusX * cos ( StartAngle ) ),
        CalcY ( CenterY - RadiusY * sin ( StartAngle ) ) );
    PBXArc ( PB, CalcX ( Data->rclBox.left ), CalcY ( Data->rclBox.top ), 
        CalcX ( Data->rclBox.right ), CalcY ( Data->rclBox.bottom ),
        CalcX ( Data->ptlStart.x ), CalcY ( Data->ptlStart.y ),
        CalcX ( Data->ptlEnd.x ), CalcY ( Data->ptlEnd.y ) );
    Parser->CurVal.x = ( LONG ) ( CenterX + RadiusX * cos ( EndAngle ) );
    Parser->CurVal.y = ( LONG ) ( CenterY - RadiusY * sin ( StartAngle ) );
    if ( !Parser->InPath ) 
        PBXStroke ( PB );

}

void PDFEMFDoPolyDraw( PPDFEmf Parser,PEMRPOLYDRAW Data)
{
    PBYTE Types;
    POINT TV;
    ppUns32 k = 0;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyDraw :\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cptl = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, Data->rclBounds.right, Data->rclBounds.bottom, Data->cptl);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    TV = Parser->CurVal;
    if ( !Parser->InPath )
        PBXNewPath ( PB );
    Types = ( PBYTE ) ( & ( Data->aptl[Data->cptl] ) );
    while ( k < Data->cptl ){
        if ( Types[k] == PT_MOVETO ){
            TV.x = Data->aptl[k].x;
            TV.y = Data->aptl[k].y;
            Parser->CurVal = TV;
            PBXMoveTo ( PB, CalcX ( TV.x ) , CalcY ( TV.y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\t MoveTo ( %i %i)\n", TV.x, TV.y);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
            continue;
        }
        if ( Types[k] & PT_LINETO ){
            Parser->CurVal.x = Data->aptl[k].x;
            Parser->CurVal.y = Data->aptl[k].y;
            PBXLineTo ( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );          
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\t LineTo ( %i %i)\n", Parser->CurVal.x, Parser->CurVal.y);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        } else 
        if ( Types[k] & PT_BEZIERTO ){
            PBXCurveTo ( PB, CalcX ( Data->aptl[k].x ), CalcY ( Data->aptl[k].y ),
                CalcX ( Data->aptl[k+1].x ), CalcY ( Data->aptl[k+1].y ),
                CalcX ( Data->aptl[k+2].x ), CalcY ( Data->aptl[k+2].y ));
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\t BezierTo ( %i %i %i %i %i %i)\n",
                    Data->aptl[k].x, Data->aptl[k].y,
                    Data->aptl[k+1].x, Data->aptl[k+1].y,
                    Data->aptl[k+2].x, Data->aptl[k+2].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            Parser->CurVal.x = Data->aptl[k+2].x;
            Parser->CurVal.y = Data->aptl[k+2].y;
            k+=3;
        }
        if ( Types[k] & PT_CLOSEFIGURE ){
            PBXLineTo ( PB, CalcX ( TV.x ), CalcY ( TV.y ) );
            Parser->CurVal =  TV;
            CDebugCode (
                ULStrToStrm ( Parser->DBG, "\t\tCloseFigure\n" );
            );
        }
    }
}
void PDFEMFDoSetArcDirection( PPDFEmf Parser,PEMRSETARCDIRECTION Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetArcDirection :\n\tMode = %s\n",
            Parser->CurRec, Data->iArcDirection == AD_COUNTERCLOCKWISE ? "AD_COUNTERCLOCKWISE" : "AD_CLOCKWISE");
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->CCW =  Data->iArcDirection == AD_COUNTERCLOCKWISE ? true : false;
    SetArcDirection ( Parser->DC, Data->iArcDirection );
}

void PDFEMFDoSetMiterLimit( PPDFEmf Parser,PEMRSETMITERLIMIT Data)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SetMiterLimit :\n\tMode = %f\n",
            Parser->CurRec, Data->eMiterLimit);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    return;
}

void PDFEMFDoBeginPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: BeginPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXNewPath ( PB);
    PDFEMFSetInPath ( Parser, true);
}

void PDFEMFDoEndPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: EndPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    ); 
}

void PDFEMFDoCloseFigure( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: CloseFigure\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXClosePath (PB);
}

void PDFEMFDoFillPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: FillPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->InPath = false;
    if (!Parser->IsNullBrush)
        PBXFill (PB);
    PBXNewPath ( PB);
}

void PDFEMFDoStrokeAndFillPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: StrokeAndFillPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->InPath = false;
    PFillAndStroke;
    PBXNewPath ( PB);
}

void PDFEMFDoStrokePath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: StrokePath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Parser->InPath = false;
    PStroke;
    PBXNewPath ( PB);
}

void PDFEMFDoSelectClipPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: SelectClipPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if (Parser->Clipping){
        PBXStateRestore (PB);
        if ( ! Parser->CPen.lopnWidth.x ) 
            PBXSetLineWidth ( PB, 1.0 );
        else
            PBXSetLineWidth (PB, CalcSizeX ( Parser->CPen.lopnWidth.x ) );
        PDFEMFSetBrushColor (Parser,false);
        PDFEMFSetDash ( Parser );
        PDFEMFSetPenColor (Parser);
        Parser->FontChanged = true;
    }
    PBXStateStore ( PB);
    Parser->Clipping = true;
    Parser->IsCR = false;
    PBXClip (PB);
    Parser->InPath = false;
    PBXNewPath (PB);
}

void PDFEMFDoAbortPath( PPDFEmf Parser)
{
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: AbortPath\n", Parser->CurRec);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    PBXNewPath ( PB);
    Parser->InPath =  false;
}

void PDFEMFDoSetDibitsToDevice( PPDFEmf Parser,PEMRSETDIBITSTODEVICE Data)
{
    HBITMAP B;
    HGDIOBJ old;
    HDC DC;
    PBITMAPINFO P;
    PVOID O;
    ppInt32 i;
    O = ( (char *) Data  + Data->offBitsSrc );
    P = ( PBITMAPINFO ) ( (char *) Data  + Data->offBmiSrc );
    if ( P->bmiHeader.biBitCount == 1)
        B = CreateBitmap ( Data->cxSrc, Data->cySrc, 1, 1, NULL );
    else 
        B = CreateCompatibleBitmap ( Parser->DC, Data->cxSrc, Data->cySrc );
    if ( !B )
        _RAISE ( ErrGeneralLevel, gleWindowsGDIError );
    DC = CreateCompatibleDC ( Parser-> DC );
    PDFTRY ( _LIB ){
        old = SelectObject ( DC, B );
        SetDIBitsToDevice ( DC, 0, 0, Data->cxSrc, Data->cySrc,
            Data->xSrc, Data->ySrc, Data->iStartScan, Data->cScans,
            O, P, Data->iUsageSrc );
        GdiFlush ( );
        i = PDFDocAppendImageFromBitmapHandle ( _DOC, B);
        PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
            CalcSizeX ( Data->cxSrc ), CalcSizeY ( Data->cySrc ), 0.0 );
    } PDFFINALLY ( _LIB ){
        SelectObject ( DC, old );
        DeleteDC ( DC );
        DeleteObject ( B );
    }
    PDFFINALLYEND ( _LIB );

}

void PDFEMFDoStretchDiBits( PPDFEmf Parser,PEMRSTRETCHDIBITS Data)
{
    HBITMAP B;
    HGDIOBJ old;
    HDC DC;
    PBITMAPINFO P;
    PVOID O;
    ppInt32 i;
    O = ( (char *) Data  + Data->offBitsSrc );
    P = ( PBITMAPINFO ) ( (char *) Data  + Data->offBmiSrc );
    if ( Data->rclBounds.right - Data->rclBounds.left > 0 && 
        Data->rclBounds.bottom - Data->rclBounds.top > 0 ){
/*        B = CreateCompatibleBitmap ( Parser->DC, Data->cxDest, Data->cyDest ); */
        B = CreateCompatibleBitmap ( Parser->DC, P->bmiHeader.biWidth, P->bmiHeader.biHeight );
        if ( !B )
            _RAISE ( ErrGeneralLevel, gleWindowsGDIError );
        DC = CreateCompatibleDC ( Parser-> DC );
        PDFTRY ( _LIB ){
            old = SelectObject ( DC, B );
            PatBlt ( DC, 0, 0, P->bmiHeader.biWidth, P->bmiHeader.biHeight , WHITENESS);

/*            StretchDIBits ( DC, 0, 0, Data->cxDest, Data->cyDest,
                Data->xSrc, Data->ySrc, Data->cxSrc, Data->cySrc,
                O, P, Data->iUsageSrc, Data->dwRop );*/
            StretchDIBits ( DC, 0, 0, P->bmiHeader.biWidth, P->bmiHeader.biHeight,
                0,0, P->bmiHeader.biWidth, P->bmiHeader.biHeight,
                O, P, Data->iUsageSrc, Data->dwRop );

            GdiFlush ( );
            i = PDFDocAppendImageFromBitmapHandle ( _DOC, B );
            PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
                CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
        } PDFFINALLY ( _LIB ){
            SelectObject ( DC, old );
            DeleteDC ( DC );
            DeleteObject ( B );
        }
        PDFFINALLYEND ( _LIB );
    }
}

void PDFEMFDoCreateFontInDirectW( PPDFEmf Parser,PEMREXTCREATEFONTINDIRECTW Data)
{
    LOGFONTA F;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: CreateFontIndirectW :\n\tData->ihFont = %i\n", 
            Parser->CurRec, Data->ihFont );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tLogFont.lfHeight = %i\n\tLogFont.lfWidth = %i\n\tLogFont.lfEscapement = %i\n\tLogFont.lfOrientation = %i\n\tLogFont.lfWeight = %i\n\tLogFont.lfItalic = %i\n", 
            Data->elfw.elfLogFont.lfHeight, Data->elfw.elfLogFont.lfWidth,
            Data->elfw.elfLogFont.lfEscapement, Data->elfw.elfLogFont.lfOrientation,
            Data->elfw.elfLogFont.lfWeight, Data->elfw.elfLogFont.lfItalic);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString,"\tLogFont.lfUnderline = %i\n\tLogFont.lfStrikeOut = %i\n\tLogFont.lfCharSet = %i\n\tLogFont.lfOutPrecision = %i\n\tLogFont.lfClipPrecision = %i\n\tLogFont.lfQuality =%i\n\tLogFont.lfPitchAndFamily = %i\n",
            Data->elfw.elfLogFont.lfUnderline, Data->elfw.elfLogFont.lfStrikeOut,
            Data->elfw.elfLogFont.lfCharSet, Data->elfw.elfLogFont.lfOutPrecision,
            Data->elfw.elfLogFont.lfClipPrecision, Data->elfw.elfLogFont.lfQuality,
            Data->elfw.elfLogFont.lfPitchAndFamily );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        WideCharToMultiByte (CP_ACP, 0, Data->elfw.elfLogFont.lfFaceName, LF_FACESIZE,
            F.lfFaceName, LF_FACESIZE, NULL, NULL);
        sprintf ( Parser->DBGString,"\tLogFont.lfFaceName = %s\n", F.lfFaceName);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->ihFont>=Parser->HandlesCount ) 
        return;
    if ( Parser->HandleTable[Data->ihFont] ) 
        DeleteObject ( Parser->HandleTable[Data->ihFont] );
    Parser->HandleTable[Data->ihFont] = CreateFontIndirectW ( &Data->elfw.elfLogFont );
    if ( !Parser->HandleTable[Data->ihFont] ){
        memmove ( &F, &Data->elfw.elfLogFont, sizeof ( F ) );
        WideCharToMultiByte (CP_ACP, 0, Data->elfw.elfLogFont.lfFaceName, LF_FACESIZE,
            F.lfFaceName, LF_FACESIZE, NULL, NULL);
        Parser->HandleTable[Data->ihFont] = CreateFontIndirectA ( &F );
    }
}

void PDFEMFDoExtTextOut( PPDFEmf Parser,PEMREXTTEXTOUTA Data)
{
    ppBool  RestoreClip = false;
    ppUns32 i;
    ppReal *Ext =  NULL, RSZ, X, Y, DX, DY, Gipot, Angle, ApAngle;
    INT *OFF;
    SIZE HY;
    char *Str;
    ppBool Opaque, Clip, IsGlyph;
    if ( Parser->InPath ){
        /* TODO Append work with convert characters to curves for InPath use */
        return;
    }
    /* if ( Parser->CurRec == 610 || Parser->CurRec == 528 ) return; */
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: ExtTextOut\n\tData->emrtext.nChars = %i\n", 
            Parser->CurRec, Data->emrtext.nChars);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
        sprintf ( Parser->DBGString, "\tOptions ( %s %s )\n", 
            Data->emrtext.fOptions & ETO_CLIPPED ? "Cliped": "",
            Data->emrtext.fOptions & ETO_OPAQUE ? "Opaque": "");
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n",
            Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tData->exScale = %f\n\tData->eyScaleData = %f\n",
            Data->exScale, Data->eyScale );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tData->emrtext.ptlReference.x = %i\n\tData->emrtext.ptlReference.y = %i\n",
            Data->emrtext.ptlReference.x, Data->emrtext.ptlReference.y );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    CDebugCode (
        sprintf ( Parser->DBGString, "\tData->emrtext.rcl.left = %i\n\tData->emrtext.rcl.top = %i\n\tData->emrtext.rcl.right = %i\n\tData->emrtext.rcl.bottom = %i\n",
            Data->emrtext.rcl.left, Data->emrtext.rcl.top, 
            Data->emrtext.rcl.right, Data->emrtext.rcl.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    Clip = Data->emrtext.fOptions &  ETO_CLIPPED ? true : false;
    Opaque = Data->emrtext.fOptions &  ETO_OPAQUE ? true : false;
	IsGlyph = Data->emrtext.fOptions &  ETO_GLYPH_INDEX ? true : false;
    if ( ( Clip || Opaque || Parser->BkMode ) && ( ! Parser->CFont.lfEscapement ) ) {
        if ( ( Clip || Opaque ) && Data->emrtext.rcl.right - Data->emrtext.rcl.left > 0 &&
            Data->emrtext.rcl.bottom - Data->emrtext.rcl.top > 0){
            if ( Clip && Data->emrtext.nChars ){
                if ( Parser->Clipping ){
                    RestoreClip = true;
                    PBXStateRestore ( PB );
                } else {
                    RestoreClip = false;
                }
                PBXStateStore ( PB );
            }
			if ( Opaque )
				PDFEMFSetBkColor ( Parser );
			PBXNewPath ( PB );
            PBXRectangle ( PB,
                CalcX ( Data->emrtext.rcl.left ), CalcY ( Data->emrtext.rcl.top ), 
                CalcX ( Data->emrtext.rcl.right + 1 ), CalcY ( Data->emrtext.rcl.bottom + 1 ) );
            if ( Clip && Data->emrtext.nChars )
                PBXClip ( PB );
            if ( Opaque ){
                PBXFill ( PB );
            } else {
                PBXNewPath ( PB );
            }
        } else {
            _BOX->WT.UseSkip = true;
            PBXNewPath ( PB );
            PDFEMFSetBkColor ( Parser );
            PBXRectangle ( PB, 
                IS ( Data->rclBounds.left ), IS ( Data->rclBounds.top ), 
                IS ( Data->rclBounds.right ), IS ( Data->rclBounds.bottom ) );
            PBXFill ( PB );
        }
    }
    _BOX->WT.UseSkip = false;

    if ( !Data->emrtext.nChars )
        return;
    _BOX->BaseLine = false;
    PDFEMFSetFontColor ( Parser );
    PDFEMFSetCurrentFont ( Parser );
    /* Calculate size of the text */
    CDebugCode (
        ULStrToStrm ( Parser->DBG, "\tData->emrtext.offDx : " );
    );
    if ( Data->emrtext.offDx ){
        
        Ext = ( ppReal * ) mmalloc ( _LIB, sizeof ( ppReal ) * Data->emrtext.nChars );    
        RSZ = 0.0;
        OFF = ( INT *) ( ( char *) Data + Data->emrtext.offDx );
        for ( i = 0; i < Data->emrtext.nChars; i++ ){
            CDebugCode (
                sprintf ( Parser->DBGString, "%i ", OFF[i]);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            Ext[i] =  CalcSizeX ( OFF[i] );
            RSZ += OFF[i];
        }
        CDebugCode (
            ULStrToStrm ( Parser->DBG, "\n" );
        );
    } else {
        RSZ = Data->emrtext.rcl.right - Data->emrtext.rcl.left;
        CDebugCode (
            ULStrToStrm ( Parser->DBG, "None\n" );
        );
    }
    /* Calculate start position of the text */
    if ( Parser->UpdatePos ){
        X = Parser->CurVal.x;
        Y = Parser->CurVal.y;
        if ( Parser->CFont.lfEscapement ){
            Parser->CurVal.x += ( LONG ) ( RSZ * cos ( Parser->CFont.lfEscapement * Pi / 1800.0 ) ) ;
            Parser->CurVal.y -= ( LONG ) ( RSZ * sin ( Parser->CFont.lfEscapement * Pi / 1800.0 ) ) ;
        } else 
            Parser->CurVal.x += ( LONG ) RSZ;
    } else {
        X = Data->emrtext.ptlReference.x;
        Y = Data->emrtext.ptlReference.y;
    }
    /* Calculate real position of the text out with align*/
    Angle = Parser->CFont.lfEscapement * Pi / 1800.0;
    if ( !Angle ){
        switch ( Parser->VertMode ){
            case vjCenter:
                Y = CalcY ( Y );
                _BOX->BaseLine = true;
                break;
            case vjBottom:
                GetTextExtentPoint32 ( Parser->DC, "Wg",2, &HY);
                Y = CalcY ( Y - HY.cy);
                break;
            default:
                Y = CalcY ( Y );
        }
        switch ( Parser->HorMode ){
            case hjCenter:
                X = CalcX (  X - RSZ  / 2.0 );
                break;
            case hjRight:
                X = CalcX ( X - RSZ );
                break;
            default:
                X = CalcX ( X ) ;
        }
    } else {
        if ( Parser->VertMode ==  vjTop && Parser->HorMode == hjLeft ){
            X = CalcX ( X );
            Y = CalcY ( Y );
        } else {
            switch ( Parser->VertMode ){
                case vjCenter:
                    GetTextExtentPoint32 ( Parser->DC, "W",2, &HY);
                    DY = HY.cy;
                    break;
                case vjBottom:
                    GetTextExtentPoint32 ( Parser->DC, "Wg",2, &HY);
                    DY = HY.cy;
                    break;
                default:
                    DY =  0.0;
            }        
            switch ( Parser->HorMode ){
                case hjCenter:
                    DX = RSZ / 2.0;
                    break;
                case hjRight:
                    DX = RSZ ;
                    break;
                default:
                    DX = 0.0;
            }
            if ( !DY ){
                X -= DX * cos ( Angle );
                Y += DX * sin ( Angle );
            } else {
                Gipot = sqrt ( DX * DX + DY * DY );
                ApAngle = Angle - asin ( DY / Gipot );
                X -= Gipot * cos ( ApAngle  );
                Y += Gipot * sin ( ApAngle );
            }           
            X = CalcX ( X );
            Y = CalcY ( Y );
        }        
    }
    PDFTRY ( _LIB ){
        if ( Data->emr.iType == EMR_EXTTEXTOUTW){
			if ( !IsGlyph ){
				CDebugCode (
					ppUns32 i;
					ppUns16 u;
					char *s;
					ULStrToStrm ( Parser->DBG, "\tUnicode info: " );
					for ( i = 0; i < Data->emrtext.nChars; i++ ){
						u = (( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ))[i];
						sprintf ( Parser->DBGString, u < 0xF ? "000%X " : u < 0xFF ? "00%X " : u < 0xFFF ? "0%X " : "%X ", u);
						ULStrToStrm ( Parser->DBG, Parser->DBGString );
					}
					ULStrToStrm ( Parser->DBG, "\n" );
					i = WideCharToMultiByte ( ULGetCodePageByCharset ( Parser->CFont.lfCharSet ), 0, 
						( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ), Data->emrtext.nChars,
						NULL, 0, NULL, NULL);
					s = (char *)mmalloc ( _LIB, i + 1);
					PDFTRY ( _LIB ){
						s[i] = '\0';
						WideCharToMultiByte ( ULGetCodePageByCharset ( Parser->CFont.lfCharSet ), 0, 
							( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ), Data->emrtext.nChars,
							s, i, NULL, NULL);
						sprintf ( Parser->DBGString, "\tString: %s \n", s);
						ULStrToStrm ( Parser->DBG, Parser->DBGString );
					} PDFFINALLY ( _LIB ){
						mfree ( _LIB, s );
					}
					PDFFINALLYEND ( _LIB );             
				);
				if ( Ext ) 
					PBXExtUnicodeTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0 , 
						( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ),
						Data->emrtext.nChars, Ext);
				else 
					PBXUnicodeTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0 , 
						( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ),
						Data->emrtext.nChars);
			}else {
				if ( Ext )
					PBXExtGlyphTextOut ( PB, X,Y, Parser->CFont.lfEscapement / 10.0, 
						( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ),
						Data->emrtext.nChars, Ext );
				else 
					PBXExtGlyphTextOut ( PB, X,Y, Parser->CFont.lfEscapement / 10.0, 
						( PppUns16 ) ( ( char *) Data + Data->emrtext.offString ),
						Data->emrtext.nChars, NULL );
			};
        } else {
            Str = ( char * ) mmalloc ( _LIB, Data->emrtext.nChars + 1 );
            Str[Data->emrtext.nChars] = '\0';
            memcpy ( Str, ( ( char *) Data + Data->emrtext.offString ), Data->emrtext.nChars );
            PDFTRY ( _LIB ){
                CDebugCode (
                    sprintf ( Parser->DBGString, "\tText: %s ", Str);
                    ULStrToStrm ( Parser->DBG, Parser->DBGString );
                );
                if ( Ext ) 
                    PBXExtTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0 , 
                    Str, Ext);
                else 
                    PBXTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0 , Str );
            } PDFFINALLY ( _LIB ){
                mfree ( _LIB, Str );
            }
            PDFFINALLYEND ( _LIB );         
        }
    } PDFFINALLY ( _LIB ){
        chfree ( _LIB, Ext );
    }
    PDFFINALLYEND ( _LIB );
    if ( Data->emrtext.fOptions & ETO_CLIPPED && Data->emrtext.nChars ){
        Parser->FontChanged = true;
        PBXStateRestore ( PB );
        if ( RestoreClip ){
            if ( Parser->IsCR ){
                PBXStateStore ( PB );
                PBXNewPath ( PB );
                PBXRectangle ( PB,
                    CalcX ( Parser->ClipRect.left ), CalcY ( Parser->ClipRect.top ), 
                    CalcX ( Parser->ClipRect.right ), CalcY ( Parser->ClipRect.bottom ) );
                PBXClip ( PB );
                PBXNewPath ( PB );
            } else {
                Parser->Clipping = false;
            }
        }     
    }  
}


void PDFEMFDoPolyBezier16( PPDFEmf Parser,PEMRPOLYLINE16 Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyBezier16\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i, Data->cpts = %i\n",
        Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
        Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts );
    ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cpts >= 4 ){
        PBXMoveTo ( PB, CalcX ( Data->apts[0].x ), CalcY ( Data->apts[0].y ) );
        for ( i=1; i<= ( Data->cpts -1  ) / 3; i++  ){
            PBXCurveTo ( PB, CalcX ( Data->apts[1 + (i - 1) * 3].x ), CalcY ( Data->apts[1 + (i - 1) * 3].y ),
                CalcX ( Data->apts[1 + (i - 1) * 3 + 1].x ), CalcY ( Data->apts[1 + (i - 1) * 3 + 1].y ),
                CalcX ( Data->apts[1 + (i - 1) * 3 + 2].x ), CalcY ( Data->apts[1 + (i - 1) * 3 + 2].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i:\n\t\t( %i, %i)\n\t\t( %i, %i)\n\t\t( %i, %i) \n",
                    i, Data->apts[1 + (i - 1) * 3].x , Data->apts[1 + (i - 1) * 3].y,
                    Data->apts[1 + (i - 1) * 3 + 1].x , Data->apts[1 + (i - 1) * 3 + 1].y,
                    Data->apts[1 + (i - 1) * 3 + 2].x , Data->apts[1 + (i - 1) * 3 + 2].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );

        }
        if ( !Parser->InPath ) 
            PStroke;
    }
}


void PDFEMFDoPolygon16( PPDFEmf Parser,PEMRPOLYLINE16 Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: Polygon:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    if ( Data->cpts){
        PBXNewPath ( PB );
        PBXMoveTo ( PB, CalcX ( Data->apts[0].x ), CalcY ( Data->apts[0].y ) );
        for (i=1; i< Data->cpts; i++ ){
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->apts[i].x , Data->apts[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            PBXLineTo ( PB, CalcX ( Data->apts[i].x ), CalcY ( Data->apts[i].y ) );
        }
        if ( !Parser->InPath ) {
            PBXClosePath ( PB );
            PFillAndStroke;
        }
    }
}


void PDFEMFDoPolyLine16( PPDFEmf Parser,PEMRPOLYLINE16 Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyLine:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i, Data->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    if ( Data->cpts){
        PBXNewPath ( PB );
        PBXMoveTo ( PB, CalcX ( Data->apts[0].x ), CalcY ( Data->apts[0].y ) );
        for (i=1; i< Data->cpts; i++ ){
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->apts[i].x , Data->apts[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            PBXLineTo ( PB, CalcX ( Data->apts[i].x ), CalcY ( Data->apts[i].y ) );
        }
        if ( !Parser->InPath ) 
            PStroke;
    }
}



void PDFEMFDoPolyBezierTo16( PPDFEmf Parser,PEMRPOLYLINE16 Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyBezierTo:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    if ( Data->cpts >= 3 ){
        if ( !Parser->InPath ) 
            PBXMoveTo ( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );
        for ( i=1; i<=  Data->cpts / 3; i++  ){
            PBXCurveTo ( PB, CalcX ( Data->apts[(i - 1) * 3].x ), CalcY ( Data->apts[(i - 1) * 3].y ),
               CalcX ( Data->apts[(i - 1) * 3 + 1].x ), CalcY ( Data->apts[(i - 1) * 3 + 1].y ),
                CalcX ( Data->apts[(i - 1) * 3 + 2].x ), CalcY ( Data->apts[(i - 1) * 3 + 2].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: \n\t\t( %i, %i)\n\t\t( %i, %i)\n\t\t( %i, %i) \n",
                    i, Data->apts[1 + (i - 1) * 3].x , Data->apts[1 + (i - 1) * 3].y,
                    Data->apts[1 + (i - 1) * 3 + 1].x , Data->apts[1 + (i - 1) * 3 + 1].y,
                    Data->apts[1 + (i - 1) * 3 + 2].x , Data->apts[1 + (i - 1) * 3 + 2].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );

        }
        Parser->CurVal.x = Data->apts[i-1].x;
        Parser->CurVal.y = Data->apts[i-1].y;
        if ( !Parser->InPath ) 
            PStroke;
    }
}

void PDFEMFDoPolyLineTo16( PPDFEmf Parser,PEMRPOLYLINE16 Data)
{
    ppUns32 i;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyLineTo:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( Data->cpts){
        if ( !Parser->InPath ){
            PBXNewPath ( PB );
            PBXMoveTo ( PB, CalcX(Parser->CurVal.x),CalcY(Parser->CurVal.y));
        }       
        for (i=0; i< Data->cpts; i++ ){
            PBXLineTo ( PB, CalcX ( Data->apts[i].x ), CalcY ( Data->apts[i].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\tIteration %i: ( %i, %i) \n",
                    i, Data->apts[i].x , Data->apts[i].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
        }
        if ( !Parser->InPath ) 
            PStroke;
        Parser->CurVal.x = Data->apts[i-1].x;
        Parser->CurVal.y = Data->apts[i-1].y;
    }
}
 
void PDFEMFDoPolyPolyLine16( PPDFEmf Parser,PEMRPOLYPOLYLINE16 Data)
{
    ppUns32 i, j, k = 0;
    PPOINTS P;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyPolyLine:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->nPolys = %i, Data->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->nPolys, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    P = ( PPOINTS ) ( & ( Data->aPolyCounts[ Data->nPolys ] ) );
    for ( i = 0; i < Data->nPolys; i++){
        PBXMoveTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
        CDebugCode (
            sprintf ( Parser->DBGString, "\tPolygon %i\n", i );
            ULStrToStrm ( Parser->DBG, Parser->DBGString );
        );
        k++;
        for (j = 1 ; j < Data->aPolyCounts[i]; j++){
            PBXLineTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\tIteration %i: ( %i, %i) \n",
                    j, P[k].x , P[k].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        }
    }
    if ( !Parser->InPath )
        PStroke;

}
     

void PDFEMFDoPolyPolygon16( PPDFEmf Parser,PEMRPOLYPOLYLINE16 Data)
{
    ppUns32 i, j, k = 0, l;
    PPOINTS P;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyPolyGon:\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->nPolys = %i\n\tData->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, 
            Data->rclBounds.right, Data->rclBounds.bottom, Data->nPolys, Data->cpts );
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    P = ( PPOINTS ) ( & ( Data->aPolyCounts[ Data->nPolys ] ) );
    for ( i = 0; i < Data->nPolys; i++){
        PBXMoveTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
        CDebugCode (
            sprintf ( Parser->DBGString, "\tPolygon %i\n", i );
            ULStrToStrm ( Parser->DBG, Parser->DBGString );
        );
        l = k;
        k++;
        for (j = 1 ; j < Data->aPolyCounts[i]; j++){
            PBXLineTo ( PB, CalcX ( P[k].x ), CalcY ( P[k].y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\tIteration %i: ( %i, %i) \n",
                    j, P[k].x , P[k].y );
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        }
        PBXLineTo ( PB, CalcX ( P[l].x ), CalcY ( P[l].y ) );
    }
    if ( !Parser->InPath )
        PFillAndStroke;
}


void PDFEMFDoPolyDraw16( PPDFEmf Parser,PEMRPOLYDRAW16 Data)
{
    PBYTE Types;
    POINT TV;
    ppUns32 k = 0;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: PolyDraw :\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n\tData->cpts = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, Data->rclBounds.right, Data->rclBounds.bottom, Data->cpts);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );

    TV = Parser->CurVal;
    if ( !Parser->InPath )
        PBXNewPath ( PB );
    Types = ( PBYTE ) ( & ( Data->apts[Data->cpts] ) );
    while ( k < Data->cpts ){
        if ( Types[k] == PT_MOVETO ){
            TV.x = Data->apts[k].x;
            TV.y = Data->apts[k].y;
            Parser->CurVal = TV;
            PBXMoveTo ( PB, CalcX ( TV.x ) , CalcY ( TV.y ) );
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\tMoveTo ( %i %i)\n", TV.x, TV.y);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
            continue;
        }
        if ( Types[k] & PT_LINETO ){
            Parser->CurVal.x = Data->apts[k].x;
            Parser->CurVal.y = Data->apts[k].y;
            PBXLineTo ( PB, CalcX ( Parser->CurVal.x ), CalcY ( Parser->CurVal.y ) );          
            CDebugCode (
                sprintf ( Parser->DBGString, "\t\t LineTo ( %i %i)\n", Parser->CurVal.x, Parser->CurVal.y);
                ULStrToStrm ( Parser->DBG, Parser->DBGString );
            );
            k++;
        } else 
            if ( Types[k] & PT_BEZIERTO ){
                PBXCurveTo ( PB, CalcX ( Data->apts[k].x ), CalcY ( Data->apts[k].y ),
                    CalcX ( Data->apts[k+1].x ), CalcY ( Data->apts[k+1].y ),
                    CalcX ( Data->apts[k+2].x ), CalcY ( Data->apts[k+2].y ));
                CDebugCode (
                    sprintf ( Parser->DBGString, "\t\t BezierTo ( %i %i %i %i %i %i)\n",
                        Data->apts[k].x, Data->apts[k].y,
                        Data->apts[k+1].x, Data->apts[k+1].y,
                        Data->apts[k+2].x, Data->apts[k+2].y );
                    ULStrToStrm ( Parser->DBG, Parser->DBGString );
                );

                Parser->CurVal.x = Data->apts[k+2].x;
                Parser->CurVal.y = Data->apts[k+2].y;
                k+=3;
            }
            if ( Types[k] & PT_CLOSEFIGURE ){
                PBXLineTo ( PB, CalcX ( TV.x ), CalcY ( TV.y ) );
                CDebugCode (
                    ULStrToStrm ( Parser->DBG, "\t\tCloseFigure\n" );
                );
                Parser->CurVal =  TV;
            }
    }
}
void PDFEMFDoExtCreatePen( PPDFEmf Parser,PEMREXTCREATEPEN Data)
{
    if ( Data->ihPen >= Parser->HandlesCount ) 
        return;
    if ( Parser->HandleTable[Data->ihPen] ) 
        DeleteObject( Parser->HandleTable[Data->ihPen] );
    Parser->HandleTable[Data->ihPen]= CreatePen ( Data->elp.elpPenStyle & (PS_STYLE_MASK + PS_ENDCAP_MASK + PS_JOIN_MASK),
        Data->elp.elpWidth,  Data->elp.elpColor );
}
void PDFEMFDoSetTextJustification( PPDFEmf Parser,PEMRLINETO Data)
{
    return;
}
void PDFEMFDoBitBLT( PPDFEmf Parser,PEMRBITBLT Data)
{
    DWORD C;
    HBITMAP B;
    HGDIOBJ old;
    HDC DC;
    PBITMAPINFO P;
    PVOID O;
    ppInt32 i;
    ppBool it;
    C = Data->dwRop;
    CDebugCode (
        sprintf ( Parser->DBGString, "%i: BitBLT :\n\tData->rclBounds.left = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.bottom = %i\n",
            Parser->CurRec, Data->rclBounds.left, Data->rclBounds.top, Data->rclBounds.right, Data->rclBounds.bottom);
        ULStrToStrm ( Parser->DBG, Parser->DBGString );
    );
    if ( !(
        C == SRCCOPY ||
        C == BLACKNESS ||
        C == DSTINVERT ||
        C == MERGECOPY ||
        C == MERGEPAINT ||
        C == NOTSRCCOPY ||
        C == NOTSRCERASE ||
        C == PATCOPY ||
        C == PATPAINT ||
        C == SRCAND ||
        C == SRCERASE ||
        C == SRCINVERT ||
        C == SRCPAINT ||
        C == WHITENESS 
        ) )
        return;
    if ( Parser->InPath )
        return;
    if ( Parser->InText){
        PDFEMFSetInText ( Parser, false );
        it = true;
    } else
        it = false;
    

    if ( !Data->cbBmiSrc || !Data->cbBitsSrc ){
        if ( Data->cxDest <= 0 || Data->cyDest <= 0 ){
            PBXNewPath ( PB );
        } else {
            PDFEMFSetBrushColor ( Parser, false );
            _BOX->WT.UseSkip = true;
            PBXRectangle ( PB, IS ( Data->rclBounds.left ), IS ( Data->rclBounds.top ),
                IS ( Data->rclBounds.right ), IS ( Data->rclBounds.bottom ) );
            _BOX->WT.UseSkip = false;
            PBXFill ( PB );
        }
    } else {
        O = ( (char *) Data  + Data->offBitsSrc );
        P = ( PBITMAPINFO ) ( (char *) Data  + Data->offBmiSrc );
        if ( Data->cxDest <= 0 || Data->cyDest <= 0 )
            return;
        if ( P->bmiHeader.biBitCount == 1)
            B = CreateBitmap ( Data->cxDest, Data->cyDest, 1, 1, NULL );
        else 
            B = CreateCompatibleBitmap ( Parser->DC, Data->cxDest, Data->cyDest );
        if ( !B )
            _RAISE ( ErrGeneralLevel, gleWindowsGDIError );
        DC = CreateCompatibleDC ( Parser-> DC );
        PDFTRY ( _LIB ){
            old = SelectObject ( DC, B );
            StretchDIBits ( DC, 0, 0, Data->cxDest, Data->cyDest,
                Data->xSrc, Data->ySrc, Data->cxDest, Data->cyDest,
                O, P, Data->iUsageSrc, C );
            GdiFlush ( );
            i = PDFDocAppendImageFromBitmapHandle ( _DOC, B);
            PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
                CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
        } PDFFINALLY ( _LIB ){
            SelectObject ( DC, old );
            DeleteDC ( DC );
            DeleteObject ( B );
        }
        PDFFINALLYEND ( _LIB );
    }
    if ( it )
        PDFEMFSetInText ( Parser, true );
}

void PDFEMFDoSetStretchBltMode( PPDFEmf Parser,PEMRSETSTRETCHBLTMODE Data)
{
    Parser->StretchBltMode =  Data->iMode;
    SetStretchBltMode ( Parser->DC, Data->iMode );
}

void PDFEMFDoStretchBlt( PPDFEmf Parser,PEMRSTRETCHBLT Data)
{
    HBITMAP B, B1;
    HGDIOBJ old, old1;
    HDC DC, DC1;
    PBITMAPINFO P;
    PVOID O;
    ppInt32 i;
	CDebugCode (
		sprintf ( Parser->DBGString, "%i: StretchBlt\n\tData->rclBounds.left = %i\n\tData->rclBounds.right = %i\n\tData->rclBounds.top = %i\n\tData->rclBounds.bottom = %i\n",
		Parser->CurRec, Data->rclBounds.left, Data->rclBounds.right, Data->rclBounds.top, Data->rclBounds.bottom );
		ULStrToStrm ( Parser->DBG, Parser->DBGString );
	);
	if ( Data->rclBounds.right > Data->rclBounds.left && Data->rclBounds.bottom > Data->rclBounds.top ){
		if ( Data->offBitsSrc && Data->offBmiSrc ){
			O = ( (char *) Data  + Data->offBitsSrc );
			P = ( PBITMAPINFO ) ( (char *) Data  + Data->offBmiSrc );
			if ( P->bmiHeader.biBitCount == 1)
				B = CreateBitmap ( P->bmiHeader.biWidth, P->bmiHeader.biHeight, 1, 1, NULL );
			else 
				B = CreateCompatibleBitmap ( Parser->DC, P->bmiHeader.biWidth, P->bmiHeader.biHeight);
			if ( !B )
				_RAISE ( ErrGeneralLevel, gleWindowsGDIError );
			DC = CreateCompatibleDC ( Parser-> DC );
			PDFTRY ( _LIB ){
				old = SelectObject ( DC, B );
				StretchDIBits ( DC, 0, 0, P->bmiHeader.biWidth, P->bmiHeader.biHeight,
					0, 0, P->bmiHeader.biWidth, P->bmiHeader.biHeight,
					O, P, Data->iUsageSrc, SRCCOPY);
				if (Data->dwRop != SRCCOPY){
					if ( P->bmiHeader.biBitCount == 1)
						B1 = CreateBitmap ( Data->cxDest, Data->cyDest, 1, 1, NULL );
					else 
						B1 = CreateCompatibleBitmap ( Parser->DC, _abs(Data->cxDest), abs(Data->cyDest));
					if ( !B1 )
						_RAISE ( ErrGeneralLevel, gleWindowsGDIError );
					DC1 = CreateCompatibleDC ( Parser-> DC );
					PDFTRY ( _LIB ){
						old1 = SelectObject ( DC1, B1 );
						StretchBlt ( DC1, 0, 0, abs(Data->cxDest), _abs(Data->cyDest),
							DC, 0, 0, P->bmiHeader.biWidth, P->bmiHeader.biHeight,
							Data->dwRop);
						GdiFlush ( );
						i = PDFDocAppendImageFromBitmapHandle ( _DOC, B1);
						PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
							CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
					} PDFFINALLY ( _LIB ){
						SelectObject ( DC1, old1 );
						DeleteDC ( DC1 );
						DeleteObject ( B1 );
					} PDFFINALLYEND ( _LIB );
				} else {
					i = PDFDocAppendImageFromBitmapHandle ( _DOC, B);
					
						PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
											CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
				}           
			} PDFFINALLY ( _LIB ){
				SelectObject ( DC, old );
				DeleteDC ( DC );
				DeleteObject ( B ); 
			}
			PDFFINALLYEND ( _LIB );
		} else {
			if ( Data->cxDest <= 0 || Data->cyDest <= 0 ){
				PBXNewPath ( PB );
			} else {
				PDFEMFSetBrushColor ( Parser, false );
				PBXRectangle ( PB,
					CalcX ( Data->xDest ), CalcY ( Data->yDest ),
					CalcX ( Data->xDest + Data->cxDest ), CalcY ( Data->yDest + Data->cyDest ) );
				PBXFill ( PB );
			}
		}
	}
}
void PDFEMFDoSmallTextOut( PPDFEmf Parser,PEMRSMALLTEXTOUTA Data)
{
    ppBool RC;
    ppReal X, Y;
    PEMRSMALLTEXTOUTCLIPA D1;
    char *str;
	SIZE SZ;
    if ( !Data->nChars )
        return;
	PDFEMFSetFontColor ( Parser );
	PDFEMFSetCurrentFont ( Parser );
    if ( !( Data->fOptions & SMALLTEXT_TYPE_WITHOUT_CLIP ) ){
        if ( Parser->Clipping ){
            RC = true;
            PBXStateRestore ( PB );
        } else {
            RC = false;
        }
        D1 = ( PEMRSMALLTEXTOUTCLIPA ) Data;
        PBXStateStore ( PB );
        PBXNewPath ( PB );
        PBXRectangle ( PB,
            CalcX ( D1->rclClip.left ), CalcY ( D1->rclClip.top ), 
            CalcX ( D1->rclClip.right ), CalcY ( D1->rclClip.bottom ) );
        PBXClip ( PB );
        PBXNewPath ( PB );
        X = CalcX ( Data->ptlReference.x );
        Y = CalcY ( Data->ptlReference.y );
        if ( Data->fOptions & SMALLTEXT_TYPE_ANSI ){
            str = ( char * ) mmalloc ( _LIB, Data->nChars + 1 );
            PDFTRY ( _LIB ){
                str[Data->nChars] = '\0';
                memcpy ( str, &D1->cString, D1->nChars);
                PBXTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0, str );
            } PDFFINALLY ( _LIB ){
                mfree ( _LIB, str );
            }
            PDFFINALLYEND ( _LIB );
        } else {
            PBXUnicodeTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0, 
                (PppUns16)D1->cString, D1->nChars );
        }
        PBXStateRestore ( PB );
        if ( RC ){
            if ( Parser->IsCR ){
                PBXStateStore ( PB );
                PBXNewPath ( PB );
                PBXRectangle ( PB,
                    CalcX ( Parser->ClipRect.left ), CalcY ( Parser->ClipRect.top ), 
                    CalcX ( Parser->ClipRect.right ), CalcY ( Parser->ClipRect.bottom ) );
                PBXClip ( PB );
                PBXNewPath ( PB );
            } else {
                Parser->Clipping = false;
            }
        }     
    } else {
        X = CalcX ( Data->ptlReference.x );
        Y = Data->ptlReference.y ;
		switch (Parser->VertMode ){
			case vjCenter:
				Y = CalcY ( Y );
				PBXTextFromBaseLine ( PB, true );
				break;
			case vjBottom:
				GetTextExtentPoint32 ( Parser->DC, "Wg", 2, &SZ );
				Y = CalcY ( Y - SZ.cy );
				break;
			default:
				Y = CalcY ( Y );
		}
        PDFEMFSetFontColor ( Parser );
        PDFEMFSetCurrentFont ( Parser );
        if ( Data->fOptions & SMALLTEXT_TYPE_ANSI ){
            str = ( char * ) mmalloc ( _LIB, Data->nChars + 1 );
            PDFTRY ( _LIB ){
                str[Data->nChars] = '\0';
                memcpy ( str, &Data->cString, Data->nChars);
                PBXTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0, str );
            } PDFFINALLY ( _LIB ){
                mfree ( _LIB, str );
            }
            PDFFINALLYEND ( _LIB );
        } else {
            PBXUnicodeTextOut ( PB, X, Y, Parser->CFont.lfEscapement / 10.0, 
                (PppUns16)Data->cString, Data->nChars );
        }
    }
}

void PDFEMFDoAlphaBlend(PPDFEmf Parser, PEMRALPHABLEND Data)
{
    HBITMAP B, B1;
    HGDIOBJ old, old1;
    HDC DC, DC1;
    PBITMAPINFO P;
    PVOID O;
    ppInt32 i;
    BLENDFUNCTION A;
    HINSTANCE hInst;
    ppBool IsAlpha ;
    FARPROC proc;
    O = ( (char *) Data  + Data->offBitsSrc );
    P = ( PBITMAPINFO ) ( (char *) Data  + Data->offBmiSrc );
    if ( Data->cySrc > 0 && Data->cxSrc > 0 ){
        B = CreateCompatibleBitmap ( Parser->DC, Data->cxDest, Data->cyDest );
        if ( !B )
            _RAISE ( ErrGeneralLevel, gleWindowsGDIError );
        DC = CreateCompatibleDC ( Parser-> DC );
        PDFTRY ( _LIB ){
            old = SelectObject ( DC, B );
            PatBlt(DC, 0, 0, Data->cxDest, Data->cyDest, WHITENESS); 
            StretchDIBits ( DC, 0, 0, Data->cxDest, Data->cyDest,
                Data->xSrc, Data->ySrc, Data->cxDest, Data->cyDest,
                O, P, Data->iUsageSrc, SRCCOPY );
            GdiFlush ( );
            hInst = LoadLibrary ( "Msimg32.dll" );
            if ( !hInst )
                IsAlpha = false;
            else {
                proc = GetProcAddress ( hInst, "AlphaBlend");
                if ( !proc ){
                    FreeLibrary ( hInst );
                    IsAlpha = false;
                } else {
                    IsAlpha = true;
                }

            }
            if ( IsAlpha ){
                B1 = CreateCompatibleBitmap ( Parser->DC, Data->cxDest, Data->cyDest );
                if ( !B1 )
                    _RAISE ( ErrGeneralLevel, gleWindowsGDIError );
                DC1 = CreateCompatibleDC ( Parser->DC );
                PDFTRY ( _LIB ){
                    old1 = SelectObject ( DC1, B1 );
                    PatBlt(DC1, 0, 0, Data->cxDest, Data->cyDest, WHITENESS); 
                    memcpy ( &A, & ( Data->dwRop ), sizeof ( A ) );
#ifndef DOTNET
                    proc ( DC1, 0, 0, Data->cxDest, Data->cyDest,
                        DC, 0, 0, Data->cxDest, Data->cyDest, A );
#endif
					GdiFlush ( );
                    SelectObject ( DC1, old1 );
                    DeleteDC ( DC1 );
                    i = PDFDocAppendImageFromBitmapHandle ( _DOC, B1 );
                    PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
                        CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
                } PDFFINALLY ( _LIB ){
                    DeleteObject ( B1 );
                    FreeLibrary ( hInst );
                }
                PDFFINALLYEND ( _LIB );
            } else {
                i = PDFDocAppendImageFromBitmapHandle ( _DOC, B );
                PBXShowImage ( PB, i, CalcX ( Data->xDest ), CalcY ( Data->yDest ),
                    CalcSizeX ( Data->cxDest ), CalcSizeY ( Data->cyDest ), 0.0 );
            }
        } PDFFINALLY ( _LIB ){
            SelectObject ( DC, old );
            DeleteDC ( DC );
            DeleteObject ( B );
        }
        PDFFINALLYEND ( _LIB );
    } else {
        if ( Data->cxDest <= 0 || Data->cyDest <= 0 ){
            PBXNewPath ( PB );
        } else {
            PDFEMFSetBrushColor ( Parser, false );
            PBXRectangle ( PB,
                CalcX ( Data->xDest ), CalcY ( Data->yDest ),
                CalcX ( Data->xDest + Data->cxDest ), CalcY ( Data->yDest + Data->cyDest ) );
            PBXFill ( PB );
        }
    }
}

void PDFEMFCreateDIBPatternBrush(PPDFEmf Parser, PEMRCREATEDIBPATTERNBRUSHPT Data){
	BITMAPINFO *P, *BMI;
	ppUns32 Size, i;
	ppUns32 Width, Height;
	HBITMAP B;
	HDC DC;
	HGDIOBJ old;
	void *O; 
	PDFCosHandle H;
	PBXHandle PBX;

	if ( Data->ihBrush>=Parser->HandlesCount ) 
		return;
	if ( Parser->HandleTable[Data->ihBrush] ) 
		DeleteObject ( Parser->HandleTable[Data->ihBrush] );
	O = ( (char *) Data  + Data->offBits );
	P = (BITMAPINFO *) ((BYTE *)Data + Data->offBmi);
	Width = P->bmiHeader.biWidth;
	Height = P->bmiHeader.biWidth;
	if ( P->bmiHeader.biBitCount == 1)
		B = CreateBitmap ( Width, Height, 1, 1, NULL );
	else 
		B = CreateCompatibleBitmap ( Parser->DC, Width, Height );
	if ( !B )
		_RAISE ( ErrGeneralLevel, gleWindowsGDIError );
	DC = CreateCompatibleDC ( Parser-> DC );
	PDFTRY ( _LIB ){
		old = SelectObject ( DC, B );
		StretchDIBits ( DC, 0, 0, Width, Height,
			0, 0, Width, Height,O, P, Data->iUsage, SRCCOPY );
		GdiFlush ( );
		i = PDFDocAppendImageFromBitmapHandle ( _DOC, B);
		H = PDFDocAddPattern( _DOC, Width, Height, Width, Height);
		PBX = PDFPatternCreateContent(H, 72);
		PBXShowImage ( PBX, i, 0,0,Width,Height,0);
		PBXClose(PBX,false);
	} PDFFINALLY ( _LIB ){
		SelectObject ( DC, old );
		DeleteDC ( DC );
		DeleteObject ( B );
	}PDFFINALLYEND ( _LIB );
	
	Size = Data->cbBits + Data->cbBmi;
	BMI = (BITMAPINFO*)mmalloc(_LIB,Size);
	memcpy(BMI, P, Size);
	i = Parser->PatternsSize;
	Parser->PatternsSize ++;
	Parser->Patterns = (EMFPattern*)mrealloc( _LIB, Parser->Patterns, Parser->PatternsSize * sizeof(EMFPattern));
	Parser->Patterns[i].Bitmap = BMI;
	Parser->Patterns[i].Pattern = H;
	Parser->HandleTable[Data->ihBrush]=CreateDIBPatternBrushPt(BMI,Data->iUsage);
}


void  PDFEMFExecuteRecord(PPDFEmf Parser, ENHMETARECORD *Data)
{
    if (Data->iType == EMR_EXTTEXTOUTA ||
        Data->iType == EMR_EXTTEXTOUTW ||
        Data->iType == EMR_SMALLTEXTOUT){
            if (Parser->InPath)
                return;
            if (!Parser->InText)
                PDFEMFSetInText ( Parser, true);
    } else 
    if (Parser->InText){
        if (!(
            Data->iType == EMR_EXTTEXTOUTA ||
            Data->iType == EMR_EXTTEXTOUTW ||
            Data->iType == EMR_SELECTOBJECT ||
            Data->iType == EMR_BITBLT ||
            Data->iType == EMR_CREATEBRUSHINDIRECT ||
            Data->iType == EMR_CREATEPEN ||
            Data->iType == EMR_SAVEDC ||
            Data->iType == EMR_RESTOREDC ||
            Data->iType == EMR_SETTEXTALIGN ||
            Data->iType == EMR_SETBKMODE ||
            Data->iType == EMR_EXTCREATEFONTINDIRECTW ||
            Data->iType == EMR_SMALLTEXTOUT ||
            Data->iType == EMR_DELETEOBJECT ||
            Data->iType == EMR_SETTEXTCOLOR ||
            Data->iType == EMR_MOVETOEX ||
            Data->iType == EMR_SETBKCOLOR )
            )
            PDFEMFSetInText ( Parser, false);
    }
    switch (Data->iType){
        case EMR_SETWINDOWEXTEX: 
            PDFEMFDoSetWindowExtEx(Parser,( PEMRSETVIEWPORTEXTEX ) Data);
            break;
        case EMR_SETWINDOWORGEX: 
            PDFEMFDoSetWindowOrgEx(Parser,( PEMRSETVIEWPORTORGEX) Data);
            break;
        case EMR_SETVIEWPORTEXTEX: 
            PDFEMFDoSetViewPortExtEx(Parser,(PEMRSETVIEWPORTEXTEX)Data);
            break;
        case EMR_SETVIEWPORTORGEX: 
            PDFEMFDoSetViewPortOrgEx(Parser,(PEMRSETVIEWPORTORGEX)Data);
            break;
        case EMR_SETMAPMODE: 
            PDFEMFDoSetMapMode(Parser,(PEMRSETMAPMODE)Data);
            break;
        case EMR_POLYBEZIER: 
            PDFEMFDoPolyBezier(Parser,(PEMRPOLYLINE)Data);
            break;
        case EMR_POLYGON: 
            PDFEMFDoPolygon(Parser,(PEMRPOLYLINE)Data);
            break;
        case EMR_POLYLINE: 
            PDFEMFDoPolyLine(Parser,(PEMRPOLYLINE)Data);
            break;
        case EMR_POLYBEZIERTO: 
            PDFEMFDoPolyBezierTo(Parser,(PEMRPOLYLINE)Data);
            break;
        case EMR_POLYLINETO: 
            PDFEMFDoPolyLineTo(Parser,(PEMRPOLYLINE)Data);
            break;
        case EMR_POLYPOLYLINE: 
            PDFEMFDoPolyPolyLine(Parser,(PEMRPOLYPOLYLINE)Data);
            break;
        case EMR_POLYPOLYGON: 
            PDFEMFDoPolyPolyGon(Parser,(PEMRPOLYPOLYLINE)Data);
            break;
        case EMR_SETPIXELV: 
            PDFEMFDoSetPixelV(Parser,(PEMRSETPIXELV)Data);
            break;
        case EMR_SETBKMODE: 
            PDFEMFDoSetBKMode(Parser,(PEMRSELECTCLIPPATH)Data);
            break;
        case EMR_SETPOLYFILLMODE: 
            PDFEMFDoSetPolyFillMode(Parser,(PEMRSELECTCLIPPATH)Data);
            break;
        case EMR_SETTEXTALIGN: 
            PDFEMFDoSetTextAlign(Parser,(PEMRSELECTCLIPPATH)Data);
            break;
        case EMR_SETTEXTCOLOR: 
            PDFEMFDoSetTextColor(Parser,(PEMRSETTEXTCOLOR)Data);
            break;
        case EMR_SETBKCOLOR: 
            PDFEMFDoSetBKColor(Parser,(PEMRSETTEXTCOLOR)Data);
            break;
        case EMR_MOVETOEX: 
            PDFEMFDoMoveToEx(Parser,(PEMRLINETO)Data);
            break;
        case EMR_INTERSECTCLIPRECT: 
            PDFEMFDoInterSectClipRect(Parser,(PEMRINTERSECTCLIPRECT)Data);
            break;
        case EMR_EXCLUDECLIPRECT: 
            PDFEMFDoExcludeClipRect(Parser,(PEMREXCLUDECLIPRECT)Data);
            break;
        case EMR_EXTSELECTCLIPRGN: 
            PDFEMFDoExtSelectClipRGN(Parser,(PEMREXTSELECTCLIPRGN)Data);
            break;
        case EMR_SAVEDC: 
            PDFEMFDoSaveDC(Parser);
            break;
        case EMR_RESTOREDC: 
            PDFEMFDoRestoreDC(Parser,(PEMRRESTOREDC)Data);
            break;
        case EMR_SETWORLDTRANSFORM: 
            PDFEMFDoSetWorldTransform(Parser,(PEMRSETWORLDTRANSFORM)Data);
            break;
        case EMR_MODIFYWORLDTRANSFORM: 
            PDFEMFDoModifyWorldTransform(Parser,(PEMRMODIFYWORLDTRANSFORM)Data);
            break;
        case EMR_SELECTOBJECT: 
            PDFEMFDoSelectObject(Parser,(PEMRSELECTOBJECT)Data);
            break;
        case EMR_CREATEPEN: 
            PDFEMFDoCreatePen(Parser,(PEMRCREATEPEN)Data);
            break;
        case EMR_CREATEBRUSHINDIRECT: 
            PDFEMFDoCreateBrushInDirect(Parser,(PEMRCREATEBRUSHINDIRECT)Data);
            break;
        case EMR_DELETEOBJECT: 
            PDFEMFDoDeleteObject(Parser,(PEMRDELETEOBJECT)Data);
            break;
        case EMR_ANGLEARC:
            PDFEMFDoAngleArc(Parser,(PEMRANGLEARC)Data);
            break;
        case EMR_ELLIPSE: 
            PDFEMFDoEllipse(Parser,(PEMRELLIPSE)Data);
            break;
        case EMR_RECTANGLE: 
            PDFEMFDoRectangle(Parser,(PEMRELLIPSE)Data);
            break;
        case EMR_ROUNDRECT: 
            PDFEMFDoRoundRect(Parser,(PEMRROUNDRECT)Data);
            break;
        case EMR_FILLRGN: 
            PDFEMFDoFillRGN(Parser,(PEMRFILLRGN)Data);
            break;
        case EMR_ARC: 
            PDFEMFDoArc(Parser,(PEMRARC)Data);
            break;
        case EMR_CHORD: 
            PDFEMFDoChord(Parser,(PEMRCHORD)Data);
            break;
        case EMR_PIE: 
            PDFEMFDoPie(Parser,(PEMRPIE)Data);
            break;
        case EMR_LINETO: 
            PDFEMFDoLineTo(Parser,(PEMRLINETO)Data);
            break;
        case EMR_ARCTO: 
            PDFEMFDoArcTo(Parser,(PEMRARC)Data);
            break;
        case EMR_POLYDRAW: 
            PDFEMFDoPolyDraw(Parser,(PEMRPOLYDRAW)Data);
            break;
        case EMR_SETARCDIRECTION: 
            PDFEMFDoSetArcDirection(Parser,(PEMRSETARCDIRECTION)Data);
            break;
        case EMR_SETMITERLIMIT: 
            PDFEMFDoSetMiterLimit(Parser,(PEMRSETMITERLIMIT)Data);
            break;
        case EMR_BEGINPATH: 
            PDFEMFDoBeginPath(Parser);
            break;
        case EMR_ENDPATH: 
            PDFEMFDoEndPath(Parser);
            break;
        case EMR_CLOSEFIGURE: 
            PDFEMFDoCloseFigure(Parser);
            break;
        case EMR_FILLPATH: 
            PDFEMFDoFillPath(Parser);
            break;
        case EMR_STROKEANDFILLPATH: 
            PDFEMFDoStrokeAndFillPath(Parser);
            break;
        case EMR_STROKEPATH:
            PDFEMFDoStrokePath(Parser);
            break;
        case EMR_SELECTCLIPPATH: 
            PDFEMFDoSelectClipPath(Parser);
            break;
        case EMR_ABORTPATH: 
            PDFEMFDoAbortPath(Parser);
            break;
        case EMR_SETDIBITSTODEVICE: 
            PDFEMFDoSetDibitsToDevice(Parser,(PEMRSETDIBITSTODEVICE)Data);
            break;
        case EMR_STRETCHDIBITS: 
            PDFEMFDoStretchDiBits(Parser,(PEMRSTRETCHDIBITS)Data);
            break;
        case EMR_EXTCREATEFONTINDIRECTW: 
            PDFEMFDoCreateFontInDirectW(Parser,(PEMREXTCREATEFONTINDIRECTW)Data);
            break;
        case EMR_EXTTEXTOUTA:
        case EMR_EXTTEXTOUTW: 
            PDFEMFDoExtTextOut(Parser,(PEMREXTTEXTOUTA)Data);
            break;
        case EMR_POLYBEZIER16: 
            PDFEMFDoPolyBezier16(Parser,(PEMRPOLYLINE16)Data);
            break;
        case EMR_POLYGON16: 
            PDFEMFDoPolygon16(Parser,(PEMRPOLYLINE16)Data);
            break;
        case EMR_POLYLINE16: 
            PDFEMFDoPolyLine16(Parser,(PEMRPOLYLINE16)Data);
            break;
        case EMR_POLYBEZIERTO16: 
            PDFEMFDoPolyBezierTo16(Parser,(PEMRPOLYLINE16)Data);
            break;
        case EMR_POLYLINETO16: 
            PDFEMFDoPolyLineTo16(Parser,(PEMRPOLYLINE16)Data);
            break;
        case EMR_POLYPOLYLINE16: 
            PDFEMFDoPolyPolyLine16(Parser,(PEMRPOLYPOLYLINE16)Data);
            break;
        case EMR_POLYPOLYGON16: 
            PDFEMFDoPolyPolygon16(Parser,(PEMRPOLYPOLYLINE16)Data);
            break;
        case EMR_POLYDRAW16: 
            PDFEMFDoPolyDraw16(Parser,(PEMRPOLYDRAW16)Data);
            break;
        case EMR_EXTCREATEPEN: 
            PDFEMFDoExtCreatePen(Parser,(PEMREXTCREATEPEN)Data);
            break;
        case EMR_SETTEXTJUSTIFICATION: 
            PDFEMFDoSetTextJustification(Parser,(PEMRLINETO)Data);
            break;
        case EMR_BITBLT: 
            PDFEMFDoBitBLT(Parser,(PEMRBITBLT)Data);
            break;
        case EMR_SETSTRETCHBLTMODE: 
            PDFEMFDoSetStretchBltMode(Parser,(PEMRSETSTRETCHBLTMODE)Data);
            break;
        case EMR_STRETCHBLT: 
            PDFEMFDoStretchBlt(Parser,(PEMRSTRETCHBLT)Data);
            break;
        case EMR_SMALLTEXTOUT:
            PDFEMFDoSmallTextOut(Parser,(PEMRSMALLTEXTOUTA)Data);
            break;
        case EMR_ALPHABLEND:
            PDFEMFDoAlphaBlend(Parser,(PEMRALPHABLEND)Data);
            break;
		case EMR_CREATEDIBPATTERNBRUSHPT:
			PDFEMFCreateDIBPatternBrush(Parser, (PEMRCREATEDIBPATTERNBRUSHPT)Data);
			break;   
	}
}

int CALLBACK PDFEMFRecordsProc(
                             HDC hDC,                      /* handle to DC */
                             HANDLETABLE *lpHTable,        /* metafile handle table */
                             CONST ENHMETARECORD *lpEMFR,  /* metafile record */
                             int nObj,                     /* count of objects */
                             LPARAM lpData                 /* optional data */
                             )
{
    PPDFEmf Parser ;
    Parser = ( PPDFEmf ) lpData;
    Parser->CurRec ++;
    PDFEMFExecuteRecord(Parser, (ENHMETARECORD *) lpEMFR);
    CDebugCode (
        ULStrToStrm ( Parser->DBG, "\n---------------------------------------------------\n" );
    );

	return lpEMFR->iType==EMR_EOF?0:1;
}


#undef _BOX 
#undef _DOC 
#undef _LIB 

#define _BOX ((PPBX)PaintBox)
#define _DOC CD(_BOX->Doc)
#define _LIB _DOC->Lib


void PDFEMFExecute(PBXHandle PaintBox, HGDIOBJ Metafile){
    RECT RC;
    PPDFEmf Parser ;
    ppInt32 i;
	ppUns32 L;
    ENHMETAHEADER Header;
    HGDIOBJ H;
    CDebugHeader ( char filename[FILENAME_MAX]; );


    Parser =( PPDFEmf ) mmalloc(_LIB,sizeof(TPDFEmf));
    PDFTRY(_LIB){
        PB = PaintBox;
		memset( &Parser->CBrush, 0xff, sizeof( Parser->CBrush ) );
		memset( &Parser->CPen, 0xff, sizeof( Parser->CPen ) );
		memset( &Parser->CFont, 0xff, sizeof( Parser->CFont ) );
        Parser->BkMode = true;
        Parser->PolyFillMode = true;
        Parser->VertMode = vjTop;
        Parser->HorMode = hjLeft;
        Parser->UpdatePos = false;
        Parser->Clipping = false;
        Parser->InPath = false;
        Parser->CCW = true;
        Parser->CurRec = 0;
        Parser->CurVal.x = 0;
        Parser->CurVal.y = 0;
        Parser->XFInited =   false;
        Parser->BkColor = GetBkColor (_DOC->DC);
        Parser->TextColor = 0;
		Parser->CurFill =0xFFFFFFFF; 
        Parser->IsNullBrush = false;
        Parser->FontChanged = true;
		Parser->Patterns = NULL;
		Parser->PatternsSize = 0;
        PDFEMFSetMapMode(Parser, MM_TEXT);
		PBXSetLineJoin(PB,ljRound);
		PBXSetLineCap(PB,lcRound);
        RC.left = 0;
        RC.top = 0;
        RC.right = GetDeviceCaps(_DOC->DC, HORZSIZE)*100;
        RC.bottom = GetDeviceCaps(_DOC->DC, VERTSIZE)*100;
        Parser->DC = CreateEnhMetaFile (_DOC->DC,NULL, &RC,NULL);
        if (!Parser->DC) 
            _RAISE(ErrPageLevel,pleCannotCreateEMFFileError);
        PDFTRY(_LIB){
            H = GetCurrentObject ( Parser->DC, OBJ_FONT );
            GetObject ( H, sizeof ( Parser->CFont ), & ( Parser->CFont ) );
            GetEnhMetaFileHeader ( ( HENHMETAFILE )Metafile, sizeof(Header),&Header);
            Parser->HandlesCount = Header.nHandles;
            Parser->HandleTable = ( HGDIOBJ * ) mmalloc(_LIB,sizeof(HGDIOBJ)*Header.nHandles);
            for (i=0;i<Header.nHandles;i++)
                Parser->HandleTable[i]=NULL;
            Parser->CurrentXF.eM11 = 1.0;
            Parser->CurrentXF.eM12 = 0.0;
            Parser->CurrentXF.eM21 = 0.0;
            Parser->CurrentXF.eM22 = 1.0;
            Parser->CurrentXF.eDx = 0.0;
            Parser->CurrentXF.eDy = 0.0;
            Parser->NT = SetWorldTransform (Parser->DC,&(Parser->CurrentXF))? true: false;
            SetGraphicsMode (Parser->DC,GM_ADVANCED);
            Parser->XOff = 0.0;
            Parser->YOff = 0.0;
            Parser->TransformStack = NULL;
            Parser->TSC = 0;
            Parser->TSS = 0; 
            CDebugCode ( 
                /*sprintf (filename,"MFile %i .emf",_LIB->DBGIndex++);
                CopyEnhMetaFile (Metafile,filename);*/
                sprintf (filename,"MFile %i .txt",( ( PPDFLib ) _LIB )->DBGIndex++);
                Parser->DBG =  ULStreamFileNew ( _LIB, filename, ppFileWriteMode );
            _BOX->DBG = Parser->DBG;
            );
            PDFTRY(_LIB){
                EnumEnhMetaFile (0, ( HENHMETAFILE )Metafile,PDFEMFRecordsProc, Parser, NULL);               
            } PDFFINALLY(_LIB){
                CDebugCode ( 
                _BOX->DBG = NULL; )
                if ( Parser->TransformStack ){            
                    mfree( _LIB, Parser->TransformStack);
                }
				for (L=0;L<Parser->PatternsSize;L++)
					mfree(_LIB, Parser->Patterns[L].Bitmap);
				mfree(_LIB, Parser->Patterns);
                for (i=0;i<Header.nHandles;i++)
                    if ( Parser->HandleTable[i])
                        DeleteObject ( Parser->HandleTable[i]);
                mfree( _LIB, Parser->HandleTable);
            } PDFFINALLYEND(_LIB);
            CDebugCode ( 
                ULStreamClose ( Parser->DBG );
            );           
        } PDFFINALLY (_LIB){
			DeleteEnhMetaFile ( ( HENHMETAFILE )Metafile );
            DeleteEnhMetaFile(CloseEnhMetaFile(Parser->DC));
        }
        PDFFINALLYEND (_LIB);
    }PDFFINALLY(_LIB){
        mfree( _LIB, Parser);
    }
    PDFFINALLYEND(_LIB);
}

#endif
