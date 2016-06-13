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
VSCanvasA.c
*********************************************************************/

#include <math.h>  
#include <time.h>

#include <string.h>

#include "../VSCanvasA.h"
#include "VSCanvasAI.h"

#include "../VSBaseA.h"
#include "VSCosAI.h"
#include "VSDocAI.h"
#include "VSFilter.h"
#include "VSStdFont.h"
#include "VSAssert.h"
#include "VSFontAI.h"
#include "VSEmf.h"



#ifdef _LIB
#undef _LIB
#endif
 
#define _BOX ((PPBX)PaintBox)
#define _DOC CD(_BOX->Doc)
#define _LIB _DOC->Lib






#define R2S(x) { ULRealToStrm ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULRealToStrm ( _BOX->DBG, x ); ); }
#define I2S(x) { ULIntToStrm ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULIntToStrm ( _BOX->DBG, x ); );}
#define SPS { ULStreamWriteChar ( _BOX->Strm, ' ' ); CDebugCode ( if ( _BOX->DBG ) ULStreamWriteChar ( _BOX->DBG, ' ' ); );}
#define C2S(x) { ULStreamWriteChar ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULStreamWriteChar ( _BOX->DBG, x ); );}
#define S2S(x) { ULStrToStrm ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULStrToStrm ( _BOX->DBG, x ); );}
#define A2S(x) { ULAtomToStrm ( _LIB, _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULAtomToStrm ( _LIB, _BOX->DBG, x ); );}
#define St2S(x) { ULStreamCopyToStream ( x, _BOX->Strm ); CDebugCode ( if ( _BOX->DBG ) ULStreamCopyToStream ( x, _BOX->DBG ); );}
#define H2S(x) { ULHexUns16ToStrm ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) ULHexUns16ToStrm ( _BOX->DBG, x ); );}
#define E2S(x) { EscToStrm ( _BOX->Strm, x ); CDebugCode ( if ( _BOX->DBG ) EscToStrm ( _BOX->DBG, x ); );}

PDFStreamHandle PBXGetStreamHandle( PBXHandle PaintBox )
{
	return _BOX->Strm;
}

TPDFColor ULRGBToColor(ppReal r, ppReal g, ppReal b){
    TPDFColor c;
    c.Device = cgRGB;
    c.Color.RGB.R = r;
    c.Color.RGB.G = g;
    c.Color.RGB.B = b;
    return c;
}

TPDFColor ULGrayToColor(ppReal g){
    TPDFColor c;
    c.Device = cgGray;
    c.Color.Gray = g;
    return c;
}

TPDFColor ULCMYKToColor(ppReal c, ppReal m, ppReal y, ppReal k){
    TPDFColor cl;
    cl.Device = cgCMYK;
    cl.Color.CMYK.C = c;
    cl.Color.CMYK.M = m;
    cl.Color.CMYK.Y = y;
    cl.Color.CMYK.K = k;
    return cl;
}


void PBXExt2Int(PPBX PB, ppReal *x, ppReal *y){
    ppReal tx, ty;
	XFORM *C;
#ifdef WINDOWS_PLATFORM
    if ( PB->WT.UseEMF && PB->WT.UseSkip )
        C = & ( PB->WT.Skip );
    else 
        C = & ( PB->WT.STDMatrix );
#else 
    C = & ( PB->WT.BaseMatrix );
#endif
	tx= C->eM11*(*x)+C->eM21*(*y)+C->eDx;
	ty= C->eM12*(*x)+C->eM22*(*y)+C->eDy;
	*x=tx;
	*y=ty; 
}

void ULRotateCoordinate( ppReal X, ppReal Y, ppReal Angle, ppReal *XO,
                         ppReal *YO )
{
    ppReal rcos, rsin;
    Angle = ( ppReal ) ( Angle * ( Pi / 180 ) );
    rcos = ( ppReal ) cos ( Angle );
    rsin = ( ppReal ) sin ( Angle );
    *XO = rcos * X - rsin * Y;
    *YO = rsin * X + rcos * Y;
}

void PBXBeginText( PBXHandle PaintBox )
{
    if ( _BOX->TextInited )
        return;
    S2S ( "BT" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->TextInited = true;
    _BOX->TX = 0;
    _BOX->TY = 0;
    _BOX->TextAngle = 0;
}

void PBXSetAngle( PPBX PB, TPDFPageRotateAngle Rotate )
{
	XFORM R;
	switch (Rotate) {
		case 1:
			R.eM12 =(float) PB->E2I;
			R.eM11 = 0.0;
			R.eM22 = 0.0;
			R.eM21 =(float) PB->E2I;
			R.eDx = 0.0;
			R.eDy = 0.0;
			break;
		case 2:
			R.eM11 =(float) -PB->E2I;
			R.eM12 = 0.0;
			R.eM21 = 0.0;
			R.eM22 =(float) PB->E2I;
			R.eDx = (float) PB->Width;
			R.eDy = 0.0;
			break;
		case 3:
			R.eM12 =(float) -PB->E2I;
			R.eM11 = 0.0;
			R.eM22 = 0.0;
			R.eM21 =(float) -PB->E2I;
			R.eDx = (float) PB->Width;
			R.eDy = (float) PB->Height;
			break;
		default:
			R.eM11 =(float) PB->E2I;
			R.eM12 = 0.0;
			R.eM21 = 0.0;
			R.eM22 =(float) -PB->E2I;
			R.eDx = 0.0;
			R.eDy = (float) PB->Height;
	}
	PB->WT.BaseMatrix = R;
#ifdef WINDOWS_PLATFORM
	PB->WT.UseEMF =  false;
    PB->WT.UseScale = false;
    PB->WT.STDMatrix = R;
    PB->WT.XScale = PB->E2I;
    PB->WT.YScale = PB->E2I;
#endif
}

void PBXEndText( PBXHandle PaintBox )
{
    if ( !_BOX->TextInited )
        return;
    S2S ( "ET" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->TextInited = false;
}

void PBXNewPath( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'n' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXClosePath( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'h' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXClip( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'W' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXEoClip( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    S2S ( "W*" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXEoFill( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    S2S ( "f*" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXFill( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'f' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXEoFillAndStroke( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    S2S ( "B*" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXFillAndStroke( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'B' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXStroke( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    C2S ( 'S' );
    C2S ( _BOX->NextOperatorChar );
}



/*Graphic primitives*/
void PBXMoveTo( PBXHandle PaintBox, ppReal x, ppReal y )
{   
    PBXEndText ( PaintBox );
    PBXExt2Int( _BOX, &x, &y );
    R2S ( x / _BOX->FS );
    SPS;
    R2S ( y / _BOX->FS );
    S2S ( " m" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->X = x;
    _BOX->Y = y;
}

void PBXLineTo( PBXHandle PaintBox, ppReal x, ppReal y )
{
    PBXEndText ( PaintBox );
    PBXExt2Int ( _BOX, &x, &y );
    R2S ( x / _BOX->FS );
    SPS;
    R2S ( y / _BOX->FS);
    S2S ( " l" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->X = x;
    _BOX->Y = y;
}

void PBXCurveTo( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                    ppReal y2, ppReal x3, ppReal y3 )
{
    PBXEndText ( PaintBox );
    PBXExt2Int ( _BOX, &x1, &y1 );
    PBXExt2Int ( _BOX, &x2, &y2 );
    PBXExt2Int ( _BOX, &x3, &y3 );
    R2S ( x1 / _BOX->FS );
    SPS;
    R2S ( y1 / _BOX->FS);
    SPS;
    R2S ( x2 / _BOX->FS);
    SPS;
    R2S ( y2 / _BOX->FS);
    SPS;
    R2S ( x3 / _BOX->FS);
    SPS;
    R2S ( y3 / _BOX->FS);
    S2S ( " c" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->X = x3;
    _BOX->Y = y3;
}



void PBXRectangle( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2 )
{
    ppReal w, h, rx, ry;
#ifdef WINDOWS_PLATFORM
    if ( ( _BOX->WT.UseEMF && ( _BOX->WT.EMFMatrix.eM12 || _BOX->WT.EMFMatrix.eM12 ) ) ){
        PBXEndText ( PaintBox );
        PBXMoveTo ( PaintBox, x1, y1 );
        PBXLineTo ( PaintBox, x1, y2 );
        PBXLineTo ( PaintBox, x2, y2 );
        PBXLineTo ( PaintBox, x2, y1 );
        PBXLineTo ( PaintBox, x1, y1 );  
    } else {
#endif
        PBXEndText ( PaintBox );
        PBXExt2Int ( _BOX, &x1, &y1 );
        PBXExt2Int ( _BOX, &x2, &y2 );
        w = fabs ( x2 - x1 );
        h = fabs ( y2 - y1 );
        rx = min ( x1, x2 );
        R2S ( rx / _BOX->FS );
        SPS;
        ry = min ( y1, y2 );
        R2S ( ry / _BOX->FS );
        SPS;
        R2S ( w / _BOX->FS);
        SPS;
        R2S ( h / _BOX->FS);
        S2S ( " re" );
        C2S ( _BOX->NextOperatorChar );
        _BOX->X = rx;
        _BOX->Y = ry;
#ifdef WINDOWS_PLATFORM
    }   
#endif
}



/* Advanced graphic operations*/
void PBXCircle( PBXHandle PaintBox, ppReal x, ppReal y, ppReal r )
{
    ppReal b= ( ppReal ) 0.5522847498;
    PBXMoveTo ( PaintBox, x + r, y );
    PBXCurveTo ( PaintBox, x + r, y + b * r, x + b * r, y + r, x, y + r );
    PBXCurveTo ( PaintBox, x - b * r, y + r, x - r, y + b * r, x - r, y );
    PBXCurveTo ( PaintBox, x - r, y - b * r, x - b * r, y - r, x, y - r );
    PBXCurveTo ( PaintBox, x + b * r, y - r, x + r, y - b * r, x + r, y );
}

void PBXEllipse( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                    ppReal y2 )
{
    ppReal b = ( ppReal) 0.5522847498, x,y,rx,ry;
    rx = ( x2 - x1 ) / 2;
    ry = ( y2 - y1 ) / 2;
    x = x1 + rx;
    y = y1 + ry;
    PBXMoveTo ( PaintBox, x + rx, y );
    PBXCurveTo ( PaintBox, x + rx, y + b * ry, x + b * rx, y + ry, x,
        y + ry );
    PBXCurveTo ( PaintBox, x - b * rx, y + ry, x - rx, y + b * ry, x - rx,
        y );
    PBXCurveTo ( PaintBox, x - rx, y - b * ry, x - b * rx, y - ry, x,
        y - ry );
    PBXCurveTo ( PaintBox, x + b * rx, y - ry, x + rx, y - b * ry, x + rx,
        y );
}

void PBXDrawArcWithBezier( PBXHandle PaintBox, ppReal CenterX, ppReal CenterY,
                           ppReal RadiusX, ppReal RadiusY, ppReal StartAngle,
                           ppReal SweepRange, ppBool UseMoveTo )
{
    TPDFRealPoint Coord[4], C2[4];
    ppReal a, b, c, x, y, ss, cc;
    ppInt32 i;
    if ( !SweepRange ){
        if ( UseMoveTo )
            PBXMoveTo ( PaintBox, ( ppReal ) ( CenterX + RadiusX * cos ( StartAngle ) ),
                ( ppReal ) ( CenterY - RadiusY * sin ( StartAngle ) ) );
        PBXLineTo ( PaintBox, ( ppReal ) ( CenterX + RadiusX * cos ( StartAngle ) ),
                ( ppReal ) ( CenterY - RadiusY * sin ( StartAngle ) ) );
        return;
    }

    b = ( ppReal ) sin ( SweepRange / 2 );
    c = ( ppReal ) cos ( SweepRange / 2 );
    a = 1 - c;
    x = a * 4 / 3;
    y = b - x * c / b;
    ss = ( ppReal ) sin ( StartAngle + SweepRange / 2 );
    cc = ( ppReal ) cos ( StartAngle + SweepRange / 2 );
    Coord[0].x = c;
    Coord[0].y = b;
    Coord[1].x = c + x;
    Coord[1].y = y;
    Coord[2].x = c + x;
    Coord[2].y = -y;
    Coord[3].x = c;
    Coord[3].y = -b;
    for ( i = 0; i < 4; i++ ){
        C2[i].x = ( ppReal ) ( CenterX + RadiusX * ( Coord[i].x * cc + Coord[i].y * ss ) -
                    0.0001 );
        C2[i].y = ( ppReal ) ( CenterY + RadiusY * ( -Coord[i].x * ss + Coord[i].y * cc ) -
                    0.0001 );
    }
    if ( UseMoveTo )
        PBXMoveTo ( PaintBox, C2[0].x, C2[0].y );
    PBXCurveTo ( PaintBox, C2[1].x, C2[1].y, C2[2].x, C2[2].y, C2[3].x, C2[3].y );
}

TPDFRealPoint PBXIntArc( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                         ppReal y2, ppReal x3, ppReal y3, ppReal x4, ppReal y4 )
{
    ppReal CenterX, CenterY, RadiusX, RadiusY, StartAngle, EndAngle,
           SweepRange;
    ppBool UseMoveTo;
    TPDFRealPoint r;
    CenterX = ( x1 + x2 ) / 2;
    CenterY = ( y1 + y2 ) / 2;
    RadiusX = ( ppReal ) ( ( fabs ( x1 - x2 ) - 1 ) / 2 );
    RadiusY = ( ppReal ) ( ( fabs ( y1 - y2 ) - 1 ) / 2 );
    if ( RadiusX < 0 )
        RadiusX = 0;
    if ( RadiusY < 0 )
        RadiusY = 0;
    StartAngle = ( ppReal ) atan2 ( -( y3 - CenterY ) * RadiusX,
                                    ( x3 - CenterX ) * RadiusY );
    EndAngle = ( ppReal ) atan2 ( -( y4 - CenterY ) * RadiusX,
                                  ( x4 - CenterX ) * RadiusY );
    SweepRange = EndAngle - StartAngle;
    if ( SweepRange <= 0 )
        SweepRange = SweepRange + ( ppReal ) ( 2 * Pi );

    r.x = ( ppReal ) ( CenterX + RadiusX * cos ( StartAngle ) );
    r.y = ( ppReal ) ( CenterY - RadiusY * sin ( StartAngle ) );

    UseMoveTo = true;
    while ( SweepRange > Pi / 2 ){
        PBXDrawArcWithBezier ( PaintBox, CenterX, CenterY, RadiusX, RadiusY,
                               StartAngle, ( ppReal ) ( Pi / 2 ), UseMoveTo );
        SweepRange = SweepRange - ( ppReal ) ( Pi / 2 );
        StartAngle = StartAngle + ( ppReal ) ( Pi / 2 );
        UseMoveTo = false;
    }
    if ( SweepRange >= 0 )
        PBXDrawArcWithBezier ( PaintBox, CenterX, CenterY, RadiusX, RadiusY,
                               StartAngle, SweepRange, UseMoveTo );
    return r;
}

TPDFRealPoint PBXIntArc2( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                          ppReal y2, ppReal BeginAngle, ppReal EndAngle )
{
    ppReal CenterX, CenterY, RadiusX, RadiusY, StartAngle, EndsAngle,
           SweepRange;
    ppBool UseMoveTo;
    TPDFRealPoint r;
    CenterX = ( x1 + x2 ) / 2;
    CenterY = ( y1 + y2 ) / 2;
    RadiusX = ( ppReal ) ( ( fabs ( x1 - x2 ) - 1 ) / 2 );
    RadiusY = ( ppReal ) ( ( fabs ( y1 - y2 ) - 1 ) / 2 );
    if ( RadiusX < 0 )
        RadiusX = 0;
    if ( RadiusY < 0 )
        RadiusY = 0;
    StartAngle = ( ppReal ) ( BeginAngle * Pi / 180 );
    EndsAngle = ( ppReal ) ( EndAngle * Pi / 180 );
    SweepRange = EndsAngle - StartAngle;
    if ( SweepRange <= 0 )
        SweepRange = SweepRange + ( ppReal ) ( 2 * Pi );
    r.x = ( ppReal ) ( CenterX + RadiusX * cos ( StartAngle ) );
    r.y = ( ppReal ) ( CenterY - RadiusY * sin ( StartAngle ) );

    UseMoveTo = true;
    while ( SweepRange > Pi / 2 ){
        PBXDrawArcWithBezier ( PaintBox, CenterX, CenterY, RadiusX, RadiusY,
                               StartAngle, ( ppReal ) ( Pi / 2 ), UseMoveTo );
        SweepRange = SweepRange - ( ppReal ) ( Pi / 2 );
        StartAngle = StartAngle + ( ppReal ) ( Pi / 2 );
        UseMoveTo = false;
    }
    if ( SweepRange >= 0 )
        PBXDrawArcWithBezier ( PaintBox, CenterX, CenterY, RadiusX, RadiusY,
                               StartAngle, SweepRange, UseMoveTo );
    return r;
}


void PBXArc( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                        ppReal y2, ppReal x3, ppReal y3, ppReal x4, ppReal y4 )
{
    PBXIntArc ( PaintBox, x1, y1, x2, y2, x3, y3, x4, y4 );
}


void PBXArc2( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                         ppReal y2, ppReal BeginAngle, ppReal EndAngle )
{
    PBXIntArc2 ( PaintBox, x1, y1, x2, y2, BeginAngle, EndAngle );
}

void PBXPie( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                ppReal y2, ppReal x3, ppReal y3, ppReal x4, ppReal y4 )
{
    ppReal CX, CY;
    TPDFRealPoint dp;
    dp = PBXIntArc ( PaintBox, x1, y1, x2, y2, x3, y3, x4, y4 );
    CX = x1 + ( x2 - x1 ) / 2;
    CY = y1 + ( y2 - y1 ) / 2;
    PBXLineTo ( PaintBox, CX, CY );
    PBXMoveTo ( PaintBox, dp.x, dp.y );
    PBXLineTo ( PaintBox, CX, CY );
}

void PBXPie2( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                 ppReal y2, ppReal BeginAngle, ppReal EndAngle )
{
    ppReal CX, CY;
    TPDFRealPoint dp;
    dp = PBXIntArc2 ( PaintBox, x1, y1, x2, y2, BeginAngle, EndAngle );
    CX = x1 + ( x2 - x1 ) / 2;
    CY = y1 + ( y2 - y1 ) / 2;
    PBXLineTo ( PaintBox, CX, CY );
    PBXMoveTo ( PaintBox, dp.x, dp.y );
    PBXLineTo ( PaintBox, CX, CY );
}




void PBXRoundRect( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
                   ppReal y2, ppReal x3, ppReal y3 )
{
    ppReal b= ( ppReal ) 0.5522847498;
    ppReal RX, RY, t;
    if ( x1 > x2 ){
        t = x2;
        x2 = x1;
        x1 = t;
    }
    if ( y1 > y2 ){
        t = y2;
        y2 = y1;
        y1 = t;
    }
    RX = x3 / 2;
    RY = y3 / 2;
    PBXMoveTo ( PaintBox, x1 + RX, y1 );
    PBXLineTo ( PaintBox, x2 - RX, y1 );
    PBXCurveTo ( PaintBox, x2 - RX + b * RX, y1, x2, y1 + RY - b * RY, x2,
                 y1 + RY );
    PBXLineTo ( PaintBox, x2, y2 - RY );
    PBXCurveTo ( PaintBox, x2, y2 - RY + b * RY, x2 - RX + b * RX, y2,
                 x2 - RX, y2 );
    PBXLineTo ( PaintBox, x1 + RX, y2 );
    PBXCurveTo ( PaintBox, x1 + RX - b * RX, y2, x1, y2 - RY + b * RY, x1,
                 y2 - RY );
    PBXLineTo ( PaintBox, x1, y1 + RY );
    PBXCurveTo ( PaintBox, x1, y1 + RY - b * RY, x1 + RX - b * RX, y1,
                 x1 + RX, y1 );
    PBXClosePath ( PaintBox );
}


/* Graphic state operators*/

void PBXStateStore( PBXHandle PaintBox )
{
    PBXEndText ( PaintBox );
    _BOX->SaveCount++;
    C2S ( 'q' );
    C2S ( _BOX->NextOperatorChar );
}

void PBXStateRestore( PBXHandle PaintBox )
{
    if ( _BOX->SaveCount != 0 ){
        PBXEndText ( PaintBox );
        _BOX->SaveCount--;
        C2S ( 'Q' );
        C2S ( _BOX->NextOperatorChar );
        if ( _BOX->FontWasInitedLevel > _BOX->SaveCount )
            _BOX->FontIsChanged = true;
    }
}

void PBXNoDash( PBXHandle PaintBox )
{
    PBXSetDash ( PaintBox, "[] 0" );
}

void PBXSetDash( PBXHandle PaintBox, char *Dash )
{
    PBXEndText ( PaintBox );
    S2S ( Dash );
    S2S ( " d" );
    C2S ( _BOX->NextOperatorChar );
}

#define chk01(x) {if(x<0)x=0;else if(x>1)x=1;}
#define chk0(x,y) {if(x<0)x=0;else if(x>y)x=y;}


void PBXSetFlatness( PBXHandle PaintBox, ppInt32 Flatness )
{
    PBXEndText ( PaintBox );
    chk0 ( Flatness, 100 )
    I2S (  Flatness );
    S2S ( " i" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXSetFillColor(PBXHandle PaintBox, TPDFColor Color){
    switch( Color.Device ) {
    case cgGray:
        chk01 ( Color.Color.Gray );
        R2S ( Color.Color.Gray );
        S2S ( " g" );
        C2S ( _BOX->NextOperatorChar );
        return;
    case cgRGB:
        chk01 ( Color.Color.RGB.R );
        chk01 ( Color.Color.RGB.G );
        chk01 ( Color.Color.RGB.B );
        R2S ( Color.Color.RGB.R );
        SPS;
        R2S ( Color.Color.RGB.G );
        SPS;
        R2S ( Color.Color.RGB.B );
        S2S ( " rg" );
        C2S ( _BOX->NextOperatorChar );
        return;
    case cgCMYK:
        chk01 ( Color.Color.CMYK.C );
        chk01 ( Color.Color.CMYK.M );
        chk01 ( Color.Color.CMYK.Y );
        chk01 ( Color.Color.CMYK.K );
        R2S ( Color.Color.CMYK.C );
        SPS;
        R2S ( Color.Color.CMYK.M );
        SPS;
        R2S ( Color.Color.CMYK.Y );
        SPS;
        R2S ( Color.Color.CMYK.K );
        S2S ( " k" );
        C2S ( _BOX->NextOperatorChar );
        return;
    }
}

void PBXSetStrokeColor(PBXHandle PaintBox, TPDFColor Color){
    switch( Color.Device ) {
    case cgGray:
        chk01 ( Color.Color.Gray );
        R2S ( Color.Color.Gray );
        S2S ( " G" );
        C2S ( _BOX->NextOperatorChar );
        return;
    case cgRGB:
        chk01 ( Color.Color.RGB.R );
        chk01 ( Color.Color.RGB.G );
        chk01 ( Color.Color.RGB.B );
        R2S ( Color.Color.RGB.R );
        SPS;
        R2S ( Color.Color.RGB.G );
        SPS;
        R2S ( Color.Color.RGB.B );
        S2S ( " RG" );
        C2S ( _BOX->NextOperatorChar );
        return;
    case cgCMYK:
        chk01 ( Color.Color.CMYK.C );
        chk01 ( Color.Color.CMYK.M );
        chk01 ( Color.Color.CMYK.Y );
        chk01 ( Color.Color.CMYK.K );
        R2S ( Color.Color.CMYK.C );
        SPS;
        R2S ( Color.Color.CMYK.M );
        SPS;
        R2S ( Color.Color.CMYK.Y );
        SPS;
        R2S ( Color.Color.CMYK.K );
        S2S ( " K" );
        C2S ( _BOX->NextOperatorChar );
        return;
    }
}

void PBXSetColor(PBXHandle PaintBox, TPDFColor Color){
    PBXSetFillColor( PaintBox, Color);
    PBXSetStrokeColor( PaintBox, Color);
}

#ifdef DELPHI
void PBXSetColorM(PBXHandle PaintBox, TPDFColor *Color){
	PBXSetFillColor( PaintBox, *Color);
	PBXSetStrokeColor( PaintBox, *Color);
}
void PBXSetStrokeColorM(PBXHandle PaintBox, TPDFColor *Color){
	PBXSetStrokeColor( PaintBox, *Color);
}
void PBXSetFillColorM(PBXHandle PaintBox, TPDFColor *Color){
	PBXSetFillColor( PaintBox, *Color);
}
#endif

void PBXSetLineWidth( PBXHandle PaintBox, ppReal LineWidth )
{
	if (LineWidth < 0 ) 
		return;
    R2S ( LineWidth * _BOX->WT.XScale / _BOX->FS );
    S2S ( " w" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXSetLineCap(PBXHandle PaintBox, TPDFLineCap LineCap)
{
    I2S (  LineCap );
    S2S ( " j" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXSetLineJoin(PBXHandle PaintBox, TPDFLineJoin LineJoin)
{
    I2S (  LineJoin );
    S2S ( " J" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXSetMiterLimit( PBXHandle PaintBox, ppReal MiterLimit )
{
    R2S ( PBXEXT2INT ( MiterLimit ) /_BOX->FS );
    S2S ( " M" );
    C2S ( _BOX->NextOperatorChar );
}

void PBXAppendLine(PBXHandle PaintBox, char * LineCode)
{
	S2S ( LineCode );
	C2S ( _BOX->NextOperatorChar );
}

void PBXSetGState ( PBXHandle PaintBox, ppUns32 Index )
{
    PDFCosHandle obj, gs;
    ppAtom ap;
    if ( !_DOC->GStateList || Index >= _DOC->GStateList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    obj = _CosDictValueByName ( _BOX->Res, ExtGState );
    if ( !_IsCosDict ( obj ) ){
        obj = CosDictNew ( _BOX->Doc, false, 5 );
        _CosDictAppend ( _BOX->Res, ExtGState, obj );
    }
    gs = ULExtListItem( _DOC->GStateList, PPDFDocGStateEntry, Index).Obj;
    ap = ULGetResourceDictElem ( _BOX->Doc, obj, "GS", &( _BOX->ResourceGStateIndex ), gs );
    C2S ( '/' );
    A2S ( ap );
    S2S ( " gs" );   
    C2S ( _BOX->NextOperatorChar );
}

void PBXClose( PBXHandle PaintBox, ppBool Pack )
{
    PDFCosHandle dk, ad;
    ppAtom str;
    ppUns32 i;
    ppBool pp, te, co, gr;
    PBXEndText ( PaintBox );
    if ( ( _BOX->TextUsed || _BOX->GrayUsed || _BOX->ColorUsed ) &&
         _CosGetType ( _BOX->Res ) == CosDict ){
        pp = true;
        te = _BOX->TextUsed;
        co = _BOX->ColorUsed;
        gr = _BOX->GrayUsed;
        dk = _CosDictValueByName ( _BOX->Res,  ProcSet ) ;
        if ( !_IsCosArray ( dk ) )
            _CosDictAppend ( _BOX->Res, ProcSet, dk = CosArrayNew ( _BOX->Doc, false, 3 ) );
        else
            for ( i = 0; i < _CosArrayCount ( dk ); i++ ){
                ad = _CosArrayItem ( dk, i );
                if ( _IsCosName ( ad ) ){
                    str = _CosNameValue ( ad );
                    if ( str == GetAtomDoc ( PDF ) )
                        pp = false;
                    else if ( str == GetAtomDoc ( Text ) )
                        te = false;
                    else if ( str == GetAtomDoc (  ImageB ) )
                        gr = false;
                    else if ( str == GetAtomDoc (  ImageC ) )
                        co = false;
                }
            }
        if ( pp )
            CosArrayAppend ( dk, _CosNameNew ( PDF ) );
        if ( te )
            CosArrayAppend ( dk, _CosNameNew ( Text ) );
        if ( co )
            CosArrayAppend ( dk, _CosNameNew ( ImageC ) );
        if ( gr )
            CosArrayAppend ( dk, _CosNameNew ( ImageB ) );
    };
    if ( Pack )
        CosStreamFlatePack ( _LIB, _BOX->cs );
    _CosDictAppend ( _BOX->cs, Length , CosIntNew ( _BOX->Doc, false,
        ULStreamGetSize ( _CosStreamValue ( _BOX->cs ) ) ) );
    mfree ( _LIB, PaintBox );
}

ppReal PBXGetWidth( PBXHandle PaintBox )
{
    if ( !_BOX->Rotate || _BOX->Rotate==2 )  return PBXINT2EXT(_BOX->Width);
    else return PBXINT2EXT(_BOX->Height);
}

ppReal PBXGetHeight( PBXHandle PaintBox )
{
    if ( !_BOX->Rotate || _BOX->Rotate==2 )  return PBXINT2EXT(_BOX->Height);
    else return PBXINT2EXT(_BOX->Width);
}


void PBXSetPattern( PBXHandle PaintBox, PDFCosHandle Pattern)
{

	PDFCosHandle obj;
	ppAtom ap;
	
	obj = _CosDictValueByName ( _BOX->Res, Pattern );
	
	if ( !_IsCosDict ( obj ) ){
		obj = CosDictNew ( _BOX->Doc, false, 5 );
		_CosDictAppend ( _BOX->Res, Pattern, obj );
	};

	ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Ptn", &( _BOX->ResourceImageIndex ), Pattern );
	S2S ( "/Pattern cs /" );
	A2S ( ap );
	S2S ( " scn" );   
	C2S ( _BOX->NextOperatorChar );

}


/* Image Options */
void PBXShowImage ( PBXHandle PaintBox, ppUns32 Index, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Angle)
{
    XFORM Scale, Total;
    float c, s, rad;
    PDFCosHandle obj;
    ppAtom ap;
    PPDFDocImageEntry Img;
    if ( !CD ( _BOX->Doc )->ImageList || Index >= CD ( _BOX->Doc )->ImageList->Size )
        PDFRAISE ( _LIB, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    PBXEndText ( PaintBox );
    Y = Y + Height;
    PBXExt2Int( _BOX, &X, &Y );
    Total.eDx = 0.0;
    Total.eDy = 0.0;
    if ( fabs ( Angle ) > 0.001 ){
        c = ( float ) cos ( rad = ( float ) ( Angle * Pi / 180.0 ) );
        s = ( float ) sin ( rad );
        Total.eM11 = c;
        Total.eM21 = s;
        Total.eM12 = -s;
        Total.eM22 = c;   
    } else {
        Total.eM11 = 1.0;
        Total.eM12 = 0.0;
        Total.eM21 = 0.0;
        Total.eM22 = 1.0;
    }
    Scale.eDx = 0.0;
    Scale.eDy = 0.0;
    Scale.eM11 = ( float ) ( Width * _BOX->E2I / _BOX->FS);
    Scale.eM22 = ( float ) ( Height * _BOX->E2I / _BOX->FS);
    Scale.eM12 = 0.0;
    Scale.eM21 = 0.0;
    Total = ULMultiplyXFORM ( Total, Scale );
#ifdef WINDOWS_PLATFORM
    if ( _BOX->WT.UseEMF || _BOX->WT.UseScale ){
        Scale = _BOX->WT.TextMatrix;
        Scale.eDx = 0.0;
        Scale.eDy = 0.0;
        Total = ULMultiplyXFORM ( Scale, Total );
    }
#endif
    obj = _CosDictValueByName ( _BOX->Res, XObject );
    Img = ( PPDFDocImageEntry )  _DOC->ImageList->FList[Index];
    if ( !_IsCosDict ( obj ) ){
        obj = CosDictNew ( _BOX->Doc, false, 5 );
        _CosDictAppend ( _BOX->Res, XObject, obj );
    };
    ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Img", &( _BOX->ResourceImageIndex ), Img->Obj );
    PBXStateStore ( PaintBox );
    R2S ( Total.eM11 );
    SPS;
    R2S ( Total.eM21 );
    SPS;
    R2S ( Total.eM12 );
    SPS;
    R2S ( Total.eM22 );
    SPS;
    R2S ( X / _BOX->FS);
    SPS;
    R2S ( Y / _BOX->FS);
    S2S ( " cm" ); 
    C2S ( _BOX->NextOperatorChar );
    C2S ( '/' );
    A2S ( ap );
    S2S ( " Do" );   
    C2S ( _BOX->NextOperatorChar );
    PBXStateRestore ( PaintBox );
}

/* Text Options */
#define FONT(x) ((PPDFFontInfo)CD(_BOX->Doc)->FontList->FList[x])

#ifdef WINDOWS_PLATFORM
void PBXSetActiveFontWithCharset( PBXHandle PaintBox, ppUns32 Index,
                                  ppReal FontSize, ppUns8 Charset,
                                  ppBool UnderLine, ppBool StrikeOut )
{
    if ( !CD ( _BOX->Doc )->FontList || Index >= CD ( _BOX->Doc )->FontList->Size )
        PDFRAISE ( _LIB, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    _BOX->UnderLine = UnderLine;
    _BOX->StrikeOut = StrikeOut;
    _BOX->CurrentFontSize = FontSize;
    _BOX->Charset = Charset;
    if ( _BOX->CurrentFontIndex == Index )
        return;
    _BOX->CurrentFontIndex = Index;
	_BOX->EmulateItalic = false;
    _BOX->Unicode = false;
    _BOX->FontInited = true;
    _BOX->FontIsChanged = true;
}
#endif



void PBXSetActiveFont( PBXHandle PaintBox, ppUns32 Index, ppReal FontSize,
                       ppBool UnderLine, ppBool StrikeOut )
{
    if ( !CD ( _BOX->Doc )->FontList || Index >= CD ( _BOX->Doc )->FontList->Size )
        PDFRAISE ( _LIB, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    _BOX->UnderLine = UnderLine;
    _BOX->StrikeOut = StrikeOut;
    _BOX->CurrentFontSize = FontSize;
#ifdef WINDOWS_PLATFORM
    _BOX->Charset = 0;
#endif
    if ( _BOX->CurrentFontIndex == Index )
        return;
	_BOX->EmulateItalic = false;
    _BOX->CurrentFontIndex = Index;
    _BOX->FontInited = true;
    _BOX->FontIsChanged = true;
    _BOX->Unicode = false;
	
    

}
void PBXSetIntCharacterSpacing( PBXHandle PaintBox, ppReal Spacing )
{
	if ( Spacing == _BOX->CharSpace )
		return;
	PBXBeginText ( PaintBox );
	R2S ( Spacing / _BOX->FS);
	S2S ( " Tc" );
	C2S ( _BOX->NextOperatorChar );
	_BOX->CharSpace = Spacing;
}

void PBXSetCharacterSpacing( PBXHandle PaintBox, ppReal Spacing )
{
    Spacing = PBXEXT2INT ( Spacing );
    if ( Spacing == _BOX->CharSpace )
        return;
    PBXBeginText ( PaintBox );
    R2S ( Spacing /_BOX->FS );
    S2S ( " Tc" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->CharSpace = Spacing;
}

void PBXSetHorizontalScaling( PBXHandle PaintBox, ppReal Scale )
{
    if ( Scale == _BOX->HorizontalScaling )
        return;
    PBXBeginText ( PaintBox );
    R2S ( Scale );
    S2S ( " Tz" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->HorizontalScaling = Scale;
}

void PBXSetTextRenderingMode( PBXHandle PaintBox, ppInt32 Mode )
{
    PBXBeginText ( PaintBox );
    I2S (  Mode );
    S2S ( " Tr" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->Render = Mode;
}

void PBXSetWordSpacing( PBXHandle PaintBox, ppReal Spacing )
{
    Spacing = PBXEXT2INT ( Spacing );
    if ( Spacing == _BOX->WordSpace )
        return;
    PBXBeginText ( PaintBox );
    R2S ( Spacing / _BOX->FS );
    S2S ( " Tw" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->WordSpace = Spacing;
}

#define PBXGetIntTextHeight(z) ( _BOX->BaseLine ? 0 : FONT ( _BOX->CurrentFontIndex )->metrics.Ascent * _BOX->CurrentFontSize / 1000 ) 

void PBXSetTextMatrix( PBXHandle PaintBox, ppReal X, ppReal Y,
                          ppReal Degrees )
{
    ppReal angle, vcos, vsin;
    ppReal it;
#ifdef WINDOWS_PLATFORM
    XFORM Calc, C;
#endif

    PBXBeginText ( PaintBox );
    angle = Pi * Degrees / 180;
    _BOX->TextAngle = Degrees;
    vcos = cos ( angle ) * _BOX->CurrentFontSize / _BOX->FS;
    vsin = sin ( angle ) * _BOX->CurrentFontSize / _BOX->FS;
#ifdef WINDOWS_PLATFORM
    if ( _BOX->WT.UseEMF || _BOX->WT.UseScale ){
        C = _BOX->WT.TextMatrix;
		if ( _BOX->EmulateItalic ){
			it = tan(Pi/20);
			Calc.eM11 =  ( FLOAT ) vcos;
			Calc.eM21 = ( FLOAT ) vsin;
			Calc.eM12 = ( FLOAT ) (vcos*it-vsin);
			Calc.eM22 = ( FLOAT ) (vcos+vsin*it);
		} else {
			Calc.eM11 =  ( FLOAT ) vcos;
			Calc.eM21 = ( FLOAT ) vsin;
			Calc.eM12 = - ( FLOAT ) vsin;
			Calc.eM22 = ( FLOAT ) vcos;
		}
        Calc.eDx = 0.0;
        Calc.eDy = 0.0;
        C.eDx = 0.0;
        C.eDy = 0.0;
        Calc = ULMultiplyXFORM ( Calc, C );
        R2S ( Calc.eM11 );
        SPS;
        R2S ( Calc.eM21 );
        SPS;
        R2S ( Calc.eM12 );
        SPS;
        R2S ( Calc.eM22 );
        SPS;
        R2S ( X /_BOX->FS);
        SPS;
        R2S ( Y / _BOX->FS);
        S2S ( " Tm" );   
        C2S ( _BOX->NextOperatorChar );
    } else
#endif
    {
		if (_BOX->EmulateItalic){
			it = tan(Pi/20);
			R2S ( vcos );
			SPS;
			R2S ( vsin );
			SPS;
			R2S ( vcos*it-vsin );
			SPS;
			R2S ( vcos+vsin*it );
		} else {
			R2S ( vcos );
			SPS;
			R2S ( vsin );
			SPS;
			R2S ( - vsin );
			SPS;
			R2S ( vcos );
		}
        SPS;
        R2S ( X / _BOX->FS);
        SPS;
        R2S ( Y / _BOX->FS);
        S2S ( " Tm" );
        C2S ( _BOX->NextOperatorChar );
    }
    _BOX->TX = X;
    _BOX->TY = Y;

}

ppReal PBXGetIntUnicodeWidth( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len )
{
    PPDFFontInfo f;
    ppInt32 i, l, sp;
    ppReal sz;
    PPDFTrueTypeInfo Font;
    PCodeTableNode Node;
    char a;
    if ( !Len )
        return 0;
    f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
    if ( f->Type != PDFTrueTypeFont )
        PDFRAISE ( _LIB,PDFBuildErrCode ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError ) );
    Font = f->FontInfo.TTFInfo;
    for ( sp = i = l = 0; i < Len; i++ ){
        if ( ULIsNotANSICode ( Text[i] ) ){
            Node = PDFTTFGetCharCodeTable ( Font->Font, Text[i] );
            l += Node->Width;
        }else{
            a = ( char ) ULUnicodeToANSI ( Text[i] );
            l += f->metrics.ANSICodes[a];
            if ( a == 32 )
                sp++;
        };
    }
    sz = l * _BOX->CurrentFontSize / 1000;
    if ( _BOX->WordSpace > 0 )
        sz += sp * _BOX->WordSpace;
    if ( _BOX->CharSpace > 0 )
        sz += Len * _BOX->CharSpace;
    if ( _BOX->HorizontalScaling != 100.0 )
        sz = sz * _BOX->HorizontalScaling / 100.0;
    return ( ppReal ) sz;
}


ppReal PBXGetIntTextWidth( PBXHandle PaintBox, char *Text )
{
    ppInt32 i, l, k, sp;
    ppReal o = 0;
    PPDFFontInfo font;
#ifdef WINDOWS_PLATFORM
    ppUns16 *MB;
    ppInt32 sz;
    ppInt32 is, CP;
#endif

#ifdef WINDOWS_PLATFORM
    if ( !_BOX->Charset || ( _BOX->Charset == DEFAULT_CHARSET && !ULGetDefFontCharSet () ) ){
#endif
        font = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
        l = ( ppInt32 ) strlen ( Text );
        for ( sp = k = i = 0; i < l; i++ ){
            k += font->metrics.ANSICodes[(ppUns8)Text[i]];
            if ( Text[i] == 32 )
                sp++;
        };
        o = k * _BOX->CurrentFontSize / 1000;
        if ( _BOX->WordSpace > 0 )
            o += sp * _BOX->WordSpace;
        if ( _BOX->CharSpace > 0 )
            o += l * _BOX->CharSpace;
        if ( _BOX->HorizontalScaling != 100.0 )
            o = o * _BOX->HorizontalScaling / 100.0;
        return o;
#ifdef WINDOWS_PLATFORM
    }else{
        CP = ULGetCodePageByCharset ( _BOX->Charset );
        is = ( ppInt32 ) strlen (  ( char * ) Text );
        sz = MultiByteToWideChar ( CP, 0, ( char * ) Text, is, NULL, 0 );
        if ( sz ){
            MB = ( ppUns16 * ) mmalloc ( _LIB, sizeof ( ppUns16 ) * sz );
            PDFTRY ( _LIB ){
                sz = MultiByteToWideChar ( CP, 0, ( char * ) Text, is, (LPWSTR)MB, sz );
                o = PBXGetIntUnicodeWidth ( PaintBox, MB, sz );
            } PDFFINALLY ( _LIB ){
                mfree ( _LIB, MB );
            }
            PDFFINALLYEND ( _LIB );
        }
    }
#endif
    return o;
}



void PBXSetCurrentFont( PBXHandle PaintBox, ppBool IsUnicode )
{
    ppAtom ap;
    PDFCosHandle obj, ff, fd, f, ar, f0, tu, AcroFontDict;
    PPDFFontInfo fnt;
    ppInt32 ind = 0, i;
	char fubs[256];


    if ( !_BOX->FontIsChanged && IsUnicode == _BOX->Unicode )
        return;
    if ( _IsCosDict ( _BOX->AcroformRes ) ){
        AcroFontDict = _CosDictValueByName ( _BOX->AcroformRes, Font );
        if ( !_IsCosDict ( AcroFontDict ) ){
            AcroFontDict = CosDictNew ( _BOX->Doc, false, 5 );
            _CosDictAppend ( _BOX->AcroformRes, Font, AcroFontDict );
        };
    }
    obj = _CosDictValueByName ( _BOX->Res, Font );
    if ( !_IsCosDict ( obj ) ){
        obj = CosDictNew ( _BOX->Doc, false, 5 );
        _CosDictAppend ( _BOX->Res, Font, obj );
    };

    fnt = FONT ( _BOX->CurrentFontIndex );
    switch ( fnt->Type ) {
        case PDFStdFont:
            if ( IsUnicode )
                _RAISE ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError ) ;
            ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Fnt", &( _BOX->ResourceFontIndex ),
                fnt->FontInfo.StdInfo.Handle );
            if ( _IsCosDict ( _BOX->AcroformRes ) ){
                ULGetResourceDictElem ( _BOX->Doc, AcroFontDict, "Fnt", &ind,
                    fnt->FontInfo.StdInfo.Handle );
            }
    	    break;
        case PDFTrueTypeFont:
			srand( (unsigned)time( NULL ) );
			for (i=0;i <6;i++ ){
				fubs[i]= (char)((rand() % 26)+'A');
			}
			fubs[6]='+';
			sprintf(fubs+7,"%s",ULAtomToString(_LIB,fnt->FontInfo.TTFInfo->FontName));
            if ( IsUnicode ){
                if ( !( fnt->FontInfo.TTFInfo->Unicode ) ){
                    fnt->FontInfo.TTFInfo->Unicode = ( PDFTrueTypeUnicodeInfo * ) mmalloc ( _LIB, sizeof ( PDFTrueTypeUnicodeInfo ) );
                    memset ( fnt->FontInfo.TTFInfo->Unicode, 0,
                        sizeof ( PDFTrueTypeUnicodeInfo ) );
                    fnt->FontInfo.TTFInfo->Unicode->Modified = true;
                    fnt->FontInfo.TTFInfo->Unicode->FontFile = ff = 
                        /* CosNullNew (_BOX->Doc); */
                        CosStreamNew ( _BOX->Doc, true, 4 ); 
                    /*Font descriptor information*/
                    fnt->FontInfo.TTFInfo->Unicode->FontDescriptor = fd = 
                        CosDictNew ( _BOX->Doc, true, 11 );
                    _CosDictAppend ( fd,Type, _CosNameNew ( FontDescriptor ) );
                    _CosDictAppend ( fd, FontBBox, ar = CosArrayNew ( _BOX->Doc, false,4 ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.XMin ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.YMin ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.XMax ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.YMax ) );
                    _CosDictAppend ( fd, FontName, CosNameNew ( _BOX->Doc, false,
                        ULStringToAtom(_LIB, fubs) ) );
                    _CosDictAppend ( fd, Flags, CosIntNew ( _BOX->Doc, false, fnt->metrics.Flags ) );
                    _CosDictAppend ( fd, StemV ,CosIntNew ( _BOX->Doc, false, fnt->metrics.StemV ) );
                    _CosDictAppend ( fd, CapHeight, CosIntNew ( _BOX->Doc, false, fnt->metrics.CapHeight ) );
                    _CosDictAppend ( fd, Ascent , CosIntNew ( _BOX->Doc, false, fnt->metrics.Ascent ) );
                    _CosDictAppend ( fd, Descent, CosIntNew ( _BOX->Doc, false, fnt->metrics.Descent ) );
                    _CosDictAppend ( fd, ItalicAngle, CosIntNew ( _BOX->Doc, false, fnt->metrics.ItalicAngle ) );
                    if ( !_IsCosNull( ff ) ) 
                        _CosDictAppend ( fd, FontFile2, ff );
                    /* Font information */
                    fnt->FontInfo.TTFInfo->Unicode->Font = f = 
                        CosDictNew ( _BOX->Doc, true, 8 );
                    _CosDictAppend ( f, Type, _CosNameNew ( Font ) );
                    _CosDictAppend ( f, BaseFont , 
                        CosNameNew ( _BOX->Doc, false, fnt->FontInfo.TTFInfo->FontName ) );
                    _CosDictAppend ( f, Subtype , _CosNameNew ( CIDFontType2 ) );
                    _CosDictAppend ( f, CIDSystemInfo , 
                        ar = CosDictNew ( _BOX->Doc,false, 3 ) );
                    _CosDictAppend ( ar, Ordering , 
                        CosStringNew ( _BOX->Doc, false, "Identity", 8 ) );
                    _CosDictAppend ( ar, Registry, 
                        CosStringNew ( _BOX->Doc, false, "Adobe", 5 ) );
                    _CosDictAppend ( ar, Supplement, CosIntNew ( _BOX->Doc, false, 0 ) );
                    _CosDictAppend ( f, FontDescriptor , fd );
                    _CosDictAppend ( f, DW, CosIntNew ( _BOX->Doc, false, 1000 ) );
                    /* ToUnicode Section */
                    fnt->FontInfo.TTFInfo->Unicode->ToUnicode = 
                        tu = CosStreamNew ( _BOX->Doc, true, 4 );

                    /* Main Font information */
                    fnt->FontInfo.TTFInfo->Unicode->Compl = 
                        f0 = CosDictNew ( _BOX->Doc, true, 6 );
                    _CosDictAppend ( f0, Type, _CosNameNew ( Font ) );
                    _CosDictAppend ( f0, Subtype, _CosNameNew ( Type0  ) );
                    _CosDictAppend ( f0,  BaseFont, 
                        CosNameNew ( _BOX->Doc, false, fnt->FontInfo.TTFInfo->FontName ) );
                    _CosDictAppend ( f0, Encoding,CosNameNew ( _BOX->Doc, false,
                        ULStringToAtom ( _LIB, "Identity-H" ) ) );
                    _CosDictAppend ( f0, DescendantFonts,
                        ar = CosArrayNew ( _BOX->Doc, false, 1 ) );
                    CosArrayAppend ( ar, f );
                    _CosDictAppend ( f0, ToUnicode , tu );
                };
                ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Fnt",
                    &( _BOX->ResourceFontIndex ), fnt->FontInfo.TTFInfo->Unicode->Compl );
                if ( _IsCosDict ( _BOX->AcroformRes ) ){
                    ULGetResourceDictElem ( _BOX->Doc, AcroFontDict, "Fnt", &ind,
                        fnt->FontInfo.TTFInfo->Unicode->Compl );
                }
            }else{
                if ( !( fnt->FontInfo.TTFInfo->ANSI ) ){
                    fnt->FontInfo.TTFInfo->ANSI = ( PDFTrueTypeANSIInfo * ) mmalloc ( _LIB,
                        sizeof ( PDFTrueTypeANSIInfo ) );
                    memset ( fnt->FontInfo.TTFInfo->ANSI, 0,
                        sizeof ( PDFTrueTypeANSIInfo ) );
                    fnt->FontInfo.TTFInfo->ANSI->FirstChar = 255;
                    fnt->FontInfo.TTFInfo->ANSI->LastChar = 32;
                    fnt->FontInfo.TTFInfo->ANSI->Modified = true;
                    /* Here append check to nonembedding */
                    fnt->FontInfo.TTFInfo->ANSI->FontFile =  ff = 
                            /*CosNullNew( _BOX->Doc);*/
                    CosStreamNew ( _BOX->Doc, true, 4 ); 
                    /*Font descriptor information*/
                    fnt->FontInfo.TTFInfo->ANSI->FontDescriptor = 
                        fd = CosDictNew ( _BOX->Doc, true, 11 );
                    _CosDictAppend ( fd, Type,_CosNameNew ( FontDescriptor ) );
                    _CosDictAppend ( fd,FontBBox , ar = 
                        CosArrayNew ( _BOX->Doc, false, 4 ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.XMin ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.YMin ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.XMax ) );
                    CosArrayAppend ( ar, CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.FontBBox.YMax ) );
                    _CosDictAppend ( fd, FontName , CosNameNew ( _BOX->Doc, false,
                        ULStringToAtom(_LIB, fubs) ) );
                    _CosDictAppend ( fd, Flags , CosIntNew ( _BOX->Doc, false, 
                        fnt->metrics.Flags ) );
                    _CosDictAppend ( fd, StemV , CosIntNew ( _BOX->Doc, false,
                        fnt->metrics.StemV ) );
                    _CosDictAppend ( fd, CapHeight, CosIntNew ( _BOX->Doc, false, 
                        fnt->metrics.CapHeight ) );
                    _CosDictAppend ( fd, Ascent,  CosIntNew ( _BOX->Doc, false, 
                        fnt->metrics.Ascent ) );
                    _CosDictAppend ( fd, Descent, CosIntNew ( _BOX->Doc, false, 
                        fnt->metrics.Descent ) );
                    _CosDictAppend ( fd, ItalicAngle, CosIntNew ( _BOX->Doc, false, 
                        fnt->metrics.ItalicAngle ) );
                    if ( !_IsCosNull ( ff ) )
                        _CosDictAppend ( fd, FontFile2, ff );
                    /* Font information */
                    fnt->FontInfo.TTFInfo->ANSI->Font = 
                        f = CosDictNew ( _BOX->Doc, true, 8 );
                    _CosDictAppend ( f, Type, _CosNameNew ( Font ) );
                    _CosDictAppend ( f, Subtype,_CosNameNew ( TrueType ) );
                    _CosDictAppend ( f, FontDescriptor, fd );
                    _CosDictAppend ( f, Encoding, _CosNameNew ( WinAnsiEncoding ) );
                    _CosDictAppend ( f, BaseFont, CosNameNew ( _BOX->Doc, false,
                        fnt->FontInfo.TTFInfo->FontName ) );
                };
                ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Fnt",
                    &( _BOX->ResourceFontIndex ), fnt->FontInfo.TTFInfo->ANSI->Font );
                if ( _IsCosDict ( _BOX->AcroformRes ) ){
                    ULGetResourceDictElem ( _BOX->Doc, AcroFontDict, "Fnt", &ind,
                        fnt->FontInfo.TTFInfo->ANSI->Font );
                }
            }
            break;
        case PDFType1Font:
            ap = ULGetResourceDictElem ( _BOX->Doc, obj, "Fnt",
                &( _BOX->ResourceFontIndex ), fnt->FontInfo.T1Info->Font );
            if ( _IsCosDict ( _BOX->AcroformRes ) ){
                ULGetResourceDictElem ( _BOX->Doc, AcroFontDict, "Fnt", &ind,
                    fnt->FontInfo.T1Info->Font );
            }
            break;
    }
    
    C2S ( '/' );
    A2S ( ap );
    S2S ( " 1 Tf" );
    C2S ( _BOX->NextOperatorChar );
    _BOX->FontIsChanged = false;
    _BOX->FontWasInitedLevel = _BOX->SaveCount;
    _BOX->Unicode = IsUnicode;
    _BOX->CurrentFontName = ap;
}

void PBXRectRotated(PBXHandle PaintBox, ppReal x, ppReal y, ppReal w, ppReal h, ppReal Angle){
    ppReal  xo, yo;
    PBXEndText (PaintBox);
    if (Angle){
        R2S ( x / _BOX->FS );
        SPS;
        R2S ( y / _BOX->FS);
        S2S ( " m" );
        C2S ( _BOX->NextOperatorChar );
        ULRotateCoordinate(w, 0, Angle, &xo, &yo);
        R2S ( (x+xo)/_BOX->FS );
        SPS;
        R2S ( (y+yo)/_BOX->FS );
        S2S ( " l" );
        C2S ( _BOX->NextOperatorChar );
        ULRotateCoordinate(w, h, Angle, &xo, &yo);
        R2S ( (x+xo)/_BOX->FS );
        SPS;
        R2S ( (y+yo)/_BOX->FS );
        S2S ( " l" );
        C2S ( _BOX->NextOperatorChar );
        ULRotateCoordinate(0, h, Angle, &xo, &yo);
        R2S ( (x+xo)/_BOX->FS );
        SPS;
        R2S ( (y+yo)/_BOX->FS );
        S2S ( " l" );
        C2S ( _BOX->NextOperatorChar );
        R2S ( x/_BOX->FS );
        SPS;
        R2S ( y/_BOX->FS );
        S2S ( " l" );
        C2S ( _BOX->NextOperatorChar );
    } else {
        R2S ( x/_BOX->FS );
        SPS;
        R2S ( y/_BOX->FS );
        SPS;
        R2S ( w/_BOX->FS );
        SPS;
        R2S ( (-h)/_BOX->FS );
        S2S ( " re" );
        C2S ( _BOX->NextOperatorChar );
    }
}


void PBXDrawUnderAndStrike(PBXHandle PaintBox, ppReal Size){

    if ( ! ( _BOX->UnderLine || _BOX->StrikeOut ) )
        return;
    if (_BOX->UnderLine){
            PBXRectRotated ( PaintBox, _BOX->TX + _BOX->WT.XScale / _BOX->E2I * 3 * sin ( Pi / 180.0 * _BOX->TextAngle ),
                _BOX->TY - _BOX->WT.XScale / _BOX->E2I * 3 * cos ( Pi / 180.0 * _BOX->TextAngle ), 
            Size,-(_BOX->CurrentFontSize)*0.05*_BOX->WT.XScale,_BOX->TextAngle);
    };
    if (_BOX->StrikeOut){
            PBXRectRotated(PaintBox,_BOX->TX-_BOX->WT.XScale / _BOX->E2I * _BOX->CurrentFontSize/4*sin(Pi/180.0*_BOX->TextAngle), 
            _BOX->TY+_BOX->WT.XScale / _BOX->E2I * _BOX->CurrentFontSize/4*cos(Pi/180.0*_BOX->TextAngle)*_BOX->WT.XScale, 
            Size,-(_BOX->CurrentFontSize)*0.05*_BOX->WT.XScale,_BOX->TextAngle);
    };
    if (_BOX->Render==2) 
        PBXFillAndStroke(PaintBox);
    else if (_BOX->Render==1)
        PBXStroke(PaintBox);
    else PBXFill(PaintBox);
}

void PBXUnicodeTextShow( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len )
{
    ppBool Calc, un;
    ppInt32 i, sp = 0;
    ppUns8 a;
    ppReal sz   = 0.0;
    PPDFTrueTypeInfo font;
    PCodeTableNode node;
    PPDFFontInfo f;
    if ( !Len )
        return;
    f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
    if ( f->Type != PDFTrueTypeFont )
        _RAISE ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError );
    Calc = _BOX->UnderLine || _BOX->StrikeOut;
    PBXSetCurrentFont ( PaintBox, un = ULIsNotANSICode ( Text[0] ) );
    font = f->FontInfo.TTFInfo;
	if ( un ){
        C2S ('<');
	} else {
		C2S ('(');
	}
    for ( i = 0; i < Len; i++ ){
        un = ULIsNotANSICode ( Text[i] );
        if ( un != _BOX->Unicode ){
            S2S ( _BOX->Unicode ? "> Tj" : ") Tj" );
            C2S ( _BOX->NextOperatorChar );
            PBXSetCurrentFont ( PaintBox, un );
			if ( un ) {
				C2S ('<');
			} else {
				C2S ('(');
			}
        }
        if ( un ){
            node = PDFTTFGetCharCodeTable ( font->Font, Text[i] );
            H2S ( node->ID );
            if ( !( node->Used ) ){
                node->Used = true;
                font->Unicode->Modified = true;
            };
            if ( Calc )
                sz += node->Width;
        }else{
            a = ( ppUns8 ) ULUnicodeToANSI ( Text[i] );
            E2S ( a );
            if ( !( font->ANSI->UsedChars[a] ) ){
                font->ANSI->UsedChars[a] = true;
                font->ANSI->Modified = true;
                if ( font->ANSI->FirstChar > a )
                    font->ANSI->FirstChar = a;
                if ( font->ANSI->LastChar < a )
                    font->ANSI->LastChar = a;
            }
            if ( Calc ){
                sz += f->metrics.ANSICodes[a];
                if ( a == 32 )
                    sp++;
            };
        };
    }
    S2S ( _BOX->Unicode ? "> Tj" : ") Tj" );
    C2S ( _BOX->NextOperatorChar );
    if ( !Calc )
        return;
    sz = sz * _BOX->CurrentFontSize / 1000;
    if ( _BOX->WordSpace > 0 )
        sz += sp * _BOX->WordSpace;
    if ( _BOX->CharSpace > 0 )
        sz += Len * _BOX->CharSpace;
    if ( _BOX->HorizontalScaling != 100.0 )
        sz = sz * _BOX->HorizontalScaling / 100.0;
    sz *= _BOX->WT.XScale / _BOX->E2I;
    PBXDrawUnderAndStrike( PaintBox, sz);
}

void PBXGlyphTextShow( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len )
{
	ppBool Calc, un;
	ppInt32 i, sp = 0;
	ppUns8 a;
	ppReal sz   = 0.0;
	PPDFTrueTypeInfo font;
	PCodeTableNode node;
	PPDFFontInfo f;
	if ( !Len )
		return;
	f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
	if ( f->Type != PDFTrueTypeFont )
		_RAISE ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError );
	Calc = _BOX->UnderLine || _BOX->StrikeOut;
	font = f->FontInfo.TTFInfo;
	node = PDFTTFGetCharCodeTableByIndex ( font->Font, Text[ 0 ] );
	PBXSetCurrentFont ( PaintBox, un = ULIsNotANSICode ( node->Code ) );
	
	if ( un ){
		C2S ('<');
	} else {
		C2S ('(');
	}
	for ( i = 0; i < Len; i++ ){
		node = PDFTTFGetCharCodeTableByIndex ( font->Font, Text [ i ] );
		un = ULIsNotANSICode ( node->Code );
		if ( un != _BOX->Unicode ){
			S2S ( _BOX->Unicode ? "> Tj" : ") Tj" );
			C2S ( _BOX->NextOperatorChar );
			PBXSetCurrentFont ( PaintBox, un );
			if ( un ) {
				C2S ('<');
			} else {
				C2S ('(');
			}
		}
		if ( un ){
			H2S ( Text [ i ] );
			if ( !( node->Used ) ){
				node->Used = true;
				font->Unicode->Modified = true;
			};
			if ( Calc )
				sz += node->Width;
		}else{
			a = ( ppUns8 ) ULUnicodeToANSI ( node->Code );
			E2S ( a );
			if ( !( font->ANSI->UsedChars[a] ) ){
				font->ANSI->UsedChars[a] = true;
				font->ANSI->Modified = true;
				if ( font->ANSI->FirstChar > a )
					font->ANSI->FirstChar = a;
				if ( font->ANSI->LastChar < a )
					font->ANSI->LastChar = a;
			}
			if ( Calc ){
				sz += f->metrics.ANSICodes[a];
				if ( a == 32 )
					sp++;
			};
		};
	}
	S2S ( _BOX->Unicode ? "> Tj" : ") Tj" );
	C2S ( _BOX->NextOperatorChar );
	if ( !Calc )
		return;
	sz = sz * _BOX->CurrentFontSize / 1000;
	if ( _BOX->WordSpace > 0 )
		sz += sp * _BOX->WordSpace;
	if ( _BOX->CharSpace > 0 )
		sz += Len * _BOX->CharSpace;
	if ( _BOX->HorizontalScaling != 100.0 )
		sz = sz * _BOX->HorizontalScaling / 100.0;
	sz *= _BOX->WT.XScale / _BOX->E2I ;
	PBXDrawUnderAndStrike( PaintBox, sz);
}



void PBXTextShow( PBXHandle PaintBox, char *TextStr )
{
    char *s;
    ppReal r ;
#ifdef WINDOWS_PLATFORM
    ppUns16 *MB;
    ppInt32 sz;
    ppInt32 is, CP;

#endif
    _BOX->TextUsed = true;
#ifdef WINDOWS_PLATFORM
    if ( !_BOX->Charset || ( _BOX->Charset == DEFAULT_CHARSET && !ULGetDefFontCharSet () ) ){
#endif
            PBXSetCurrentFont ( PaintBox, false );
            PDFFontFillUsed ( FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex ),
                ( ppUns8 * ) TextStr );
            s = ULGetEscString ( _LIB, TextStr,
                ( ppInt32 ) strlen ( TextStr ) );
            C2S ( '(' );
            S2S ( s );
            S2S ( ") Tj" );
            C2S ( _BOX->NextOperatorChar );
            mfree ( _LIB, s );
            r = PBXGetIntTextWidth ( PaintBox, TextStr );
            r *= _BOX->WT.XScale / _BOX->E2I ;
            PBXDrawUnderAndStrike( PaintBox, r);
#ifdef WINDOWS_PLATFORM
        }else{
            CP = ULGetCodePageByCharset ( _BOX->Charset );
            is = ( ppInt32 ) strlen ( TextStr );
            sz = MultiByteToWideChar ( CP, 0, TextStr, is, NULL, 0 );
            if ( sz ){
                MB = ( ppUns16 * ) mmalloc ( _LIB, sizeof ( ppUns16 ) * sz );
                PDFTRY ( _LIB ){
                    sz = MultiByteToWideChar ( CP, 0, TextStr, is, (LPWSTR)MB, sz );
                    PBXUnicodeTextShow ( PaintBox, MB, sz );
                }
                PDFFINALLY ( _LIB ){
                    mfree ( _LIB, MB );
                }
                PDFFINALLYEND ( _LIB );            
            }
        }
#endif
}

ppReal PBXGetTextHeight(PBXHandle PaintBox)
{
	return PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
}

void PBXTextOut( PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation,
                char *TextStr )
{
    ppReal O, Angle, XI, YI;
    if ( !TextStr )
        return;
    Angle =  _BOX->Rotate*90 + Orientation;
    O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
    if ( !Angle ){
        XI=X;
        YI=Y + O;
    } else {
        XI = X + O * sin ( Angle * Pi / 180 );
        YI = Y + O * cos ( Angle * Pi / 180 );
    };
    PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
    PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
    PBXTextShow ( PaintBox, TextStr );
    _BOX->TextAngle = Orientation;
}


void PBXUnicodeTextOut( PBXHandle PaintBox, ppReal X, ppReal Y,
                       ppReal Orientation, PppUns16 Text, ppInt32 Len )
{
    ppReal O, Angle, XI, YI;
    if ( !Text )
        return;
    Angle =  _BOX->Rotate*90 + Orientation;
    O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
    if ( !Angle ){
        XI=X;
        YI=Y + O;
    } else {
        XI = X + O * sin ( Angle * Pi / 180 );
        YI = Y + O * cos ( Angle * Pi / 180 );
    };
    PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
    PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
    PBXUnicodeTextShow (PaintBox, Text, Len);
    _BOX->TextAngle = Orientation;
}

void PBXGlyphTextOut( PBXHandle PaintBox, ppReal X, ppReal Y,
					   ppReal Orientation, PppUns16 Text, ppInt32 Len )
{
	ppReal O, Angle, XI, YI;
	if ( !Text )
		return;
	Angle =  _BOX->Rotate*90 + Orientation;
	O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
	if ( !Angle ){
		XI=X;
		YI=Y + O;
	} else {
		XI = X + O * sin ( Angle * Pi / 180 );
		YI = Y + O * cos ( Angle * Pi / 180 );
	};
	PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
	PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
	PBXGlyphTextShow (PaintBox, Text, Len);
	_BOX->TextAngle = Orientation;
}


void PBXExtGlyphTextShow( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len,
						   ppReal *Dx )
{
	PDFStreamHandle strm;
	PPDFFontInfo f;
	ppInt32 i, l, k;
	PPDFTrueTypeInfo font;
	PCodeTableNode node;
	ppReal RS, CS, D;
	ppUns8 a;
	ppBool un;
	if ( !Len )
		return ;
	if ( !Dx ){
		PBXUnicodeTextShow ( PaintBox, Text, Len );
		return;
	}
	f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
	if ( f->Type != PDFTrueTypeFont )
		_RAISE ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError );
	strm = ULStreamMemNew ( _LIB, 4096 );
	font = f->FontInfo.TTFInfo;
	PDFTRY ( _LIB ){
		PBXSetWordSpacing ( PaintBox, 0 );
		for ( l = 0; ; ){
			RS = CS = 0.0;
			k = 0;
			node = PDFTTFGetCharCodeTableByIndex ( font->Font, Text [ l ] );
			un = ULIsNotANSICode ( node->Code );
			PBXSetCurrentFont ( PaintBox, un );
			ULStreamClear ( strm, 4096 );
			for ( ; ; ){
				if ( l >= Len ){
					if (k > 0 )
						PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
					break;
				}             
				node = PDFTTFGetCharCodeTableByIndex ( font->Font, Text [ l ] );
				if ( un != ULIsNotANSICode ( node->Code ) ) {
					if (k > 0 )
						PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
					break;
				}
				if ( un ){
					D = node->Width / 1000.0;
					ULHexUns16ToStrm ( strm, node->ID );
					if ( !( node->Used ) ){
						node->Used = true;
						font->Unicode->Modified = true;
					};
				}else{
					a = ULUnicodeToANSI ( node->Code );
					D = f->metrics.ANSICodes[a] / 1000.0;
					if ( a < 32 )
						a = 32;
					if ( !( font->ANSI->UsedChars[a] ) ){
						font->ANSI->UsedChars[a] = true;
						font->ANSI->Modified = true;
						if ( font->ANSI->FirstChar > a )
							font->ANSI->FirstChar = a;
						if ( font->ANSI->LastChar < a )
							font->ANSI->LastChar = a;
					}
					EscToStrm ( strm, a );
				}
				if ( _BOX->HorizontalScaling != 100.0 )
					D *= _BOX->HorizontalScaling / 100.0;

				if ( fabs ( D - Dx[l] ) > 0.1 ){
					if (k > 0 )
						PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
					CS += Dx[l];            
					l++;
					k++;
					break;
				}
				RS += D;
				CS += Dx[l];            
				l++;
				k++;
			}
			if ( k ){             
				if ( un ){
					C2S ( '<' );
					ULStreamSetPosition ( strm, 0 );
					St2S ( strm );
					S2S ( "> Tj" );
					C2S ( _BOX->NextOperatorChar );
				}else{
					C2S ( '(' );
					ULStreamSetPosition ( strm, 0 );
					St2S ( strm );
					S2S ( ") Tj" );
					C2S ( _BOX->NextOperatorChar );
				}
			}
			if ( l < Len ){
				R2S ( CS / _BOX->FS);
				S2S ( " 0 Td" );
				C2S ( _BOX->NextOperatorChar );
			}else
				break;
		}
	}
	PDFFINALLY ( _LIB ){
		ULStreamClose ( strm );
	}
	PDFFINALLYEND ( _LIB );
	if ( !( _BOX->UnderLine || _BOX->StrikeOut ) )
		return;
	for ( i = 0,RS = 0.0; i < Len; i++ )
		RS += Dx[i];
	RS *= _BOX->CurrentFontSize;
	RS *= _BOX->WT.XScale / _BOX->E2I ;
	PBXDrawUnderAndStrike( PaintBox, RS);
}

void PBXExtGlyphTextOut( PBXHandle PaintBox, ppReal X, ppReal Y,
						  ppReal Orientation, PppUns16 Text, ppInt32 Len,
						  ppReal *Dx )
{
	ppReal *ara, O, Angle, XI, YI;
	ppInt32 i;
	if ( !Len )
		return;
	if ( !Dx )
		PBXGlyphTextOut ( PaintBox, X, Y, Orientation, Text, Len );
	ara = ( ppReal * ) mmalloc ( _LIB, sizeof ( ppReal ) * Len );
	PDFTRY ( _LIB ){
		for ( i = 0; i < Len; i++ )
			ara[i] = Dx[i] * _BOX->E2I / _BOX->CurrentFontSize;
		Angle =  _BOX->Rotate*90 + Orientation;
		O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
		if ( !Angle ){
			XI=X;
			YI=Y + O;
		} else {
			XI = X + O * sin ( Angle * Pi / 180 );
			YI = Y + O * cos ( Angle * Pi / 180 );
		};
		PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
		PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
		PBXExtGlyphTextShow ( PaintBox, Text, Len, ara);
		_BOX->TextAngle = Orientation;
	} PDFFINALLY ( _LIB ){
		mfree ( _LIB, ara );
	}
	PDFFINALLYEND ( _LIB );
}


void PBXExtUnicodeTextShow( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len,
                            ppReal *Dx )
{
    PDFStreamHandle strm;
    PPDFFontInfo f;
    ppInt32 i, l, k;
    PPDFTrueTypeInfo font;
    PCodeTableNode node;
    ppReal RS, CS, D;
    ppUns8 a;
    ppBool un;
    if ( !Len )
        return ;
    if ( !Dx ){
        PBXUnicodeTextShow ( PaintBox, Text, Len );
        return;
    }
    f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
    strm = ULStreamMemNew ( _LIB, 4096 );
    font = f->FontInfo.TTFInfo;
    PDFTRY ( _LIB ){
        PBXSetWordSpacing ( PaintBox, 0 );
        for ( l = 0; ; ){
            RS = CS = 0.0;
            k = 0;
            un = ULIsNotANSICode ( Text[l] );
            PBXSetCurrentFont ( PaintBox, un );
            ULStreamClear ( strm, 4096 );
            for ( ; ; ){
                if ( l >= Len ){
                    if (k > 0 )
                        PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
                    break;
                }             
                if ( un != ULIsNotANSICode ( Text[l] ) ) {
                    if (k > 0 )
                        PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
                    break;
                }
                if ( un ){
					if ( f->Type != PDFTrueTypeFont )
						_RAISE ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError );
                    node = PDFTTFGetCharCodeTable ( font->Font, Text[l] );
                    D = node->Width / 1000.0;
                    ULHexUns16ToStrm ( strm, node->ID );
                    if ( !( node->Used ) ){
                        node->Used = true;
                        font->Unicode->Modified = true;
                    };
                }else{
                    a = ULUnicodeToANSI ( Text[l] );
					D = f->metrics.ANSICodes[a] / 1000.0;
					if ( f->Type == PDFTrueTypeFont ){
						if ( a < 32 )
							a = 32;
						if ( !( font->ANSI->UsedChars[a] ) ){
							font->ANSI->UsedChars[a] = true;
							font->ANSI->Modified = true;
							if ( font->ANSI->FirstChar > a )
								font->ANSI->FirstChar = a;
							if ( font->ANSI->LastChar < a )
								font->ANSI->LastChar = a;
						}
					};
                    EscToStrm ( strm, a );
                }
                if ( _BOX->HorizontalScaling != 100.0 )
                    D *= _BOX->HorizontalScaling / 100.0;

                if ( fabs ( D - Dx[l] ) > 0.1 ){
                    if (k > 0 )
                        PBXSetIntCharacterSpacing ( PaintBox, ( CS - RS ) / k );
                    CS += Dx[l];            
                    l++;
                    k++;
                    break;
                }
                RS += D;
                CS += Dx[l];            
                l++;
                k++;
            }
            if ( k ){             
                if ( un ){
                    C2S ( '<' );
                    ULStreamSetPosition ( strm, 0 );
                    St2S ( strm );
                    S2S ( "> Tj" );
                    C2S ( _BOX->NextOperatorChar );
                }else{
                    C2S ( '(' );
                    ULStreamSetPosition ( strm, 0 );
                    St2S ( strm );
                    S2S ( ") Tj" );
                    C2S ( _BOX->NextOperatorChar );
                }
            }
            if ( l < Len ){
                R2S ( CS );
                S2S ( " 0 Td" );
                C2S ( _BOX->NextOperatorChar );
            }else
                break;
        }
    }
    PDFFINALLY ( _LIB ){
        ULStreamClose ( strm );
    }
    PDFFINALLYEND ( _LIB );
    if ( !( _BOX->UnderLine || _BOX->StrikeOut ) )
        return;
    for ( i = 0,RS = 0.0; i < Len; i++ )
        RS += Dx[i];
    RS *= _BOX->CurrentFontSize;
    RS *= _BOX->WT.XScale / _BOX->E2I ;
    PBXDrawUnderAndStrike( PaintBox, RS);
}

void PBXExtTextShow( PBXHandle PaintBox, char *TextStr, ppReal *Dx )
{
#ifdef WINDOWS_PLATFORM
    ppUns16 *MB;
    ppInt32 sz;
    ppInt32 is, CP;
#endif
    PDFStreamHandle strm;
    PPDFFontInfo f;
    ppInt32 Len, l, k, i;
    ppUns8 a;
    ppReal RS, CS, D;
#ifdef WINDOWS_PLATFORM
    if ( !_BOX->Charset || ( _BOX->Charset == DEFAULT_CHARSET && !ULGetDefFontCharSet () ) ){
#endif
            Len = ( ppInt32 ) strlen ( TextStr );
            if ( !Len )
                return ;
            if ( !Dx ){
                PBXTextShow ( PaintBox, TextStr );
                return;
            }
            PBXSetCurrentFont ( PaintBox, false );
            f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
            strm = ULStreamMemNew ( _LIB, 4096 );
            PDFTRY ( _LIB ){
                PBXSetWordSpacing ( PaintBox, 0 );
                for ( l = 0; ; ){
                    RS = CS = 0.0;
                    k = 0;
                    ULStreamClear ( strm, 4096 );
                    for ( ; ; ){
                        if ( l >= Len )
                            break;
                        a = ( ppUns8 ) TextStr[l];
                        D = f->metrics.ANSICodes[a];
                        if ( a < 32 ){
                            CS += Dx[l++];
                            break;
                        }
                        if ( f->Type == PDFTrueTypeFont ){
                            if ( !( f->FontInfo.TTFInfo->ANSI->UsedChars[a] ) ){
                                f->FontInfo.TTFInfo->ANSI->UsedChars[a] = true;
                                f->FontInfo.TTFInfo->ANSI->Modified = true;
                                if ( f->FontInfo.TTFInfo->ANSI->FirstChar > a )
                                    f->FontInfo.TTFInfo->ANSI->FirstChar = a;
                                if ( f->FontInfo.TTFInfo->ANSI->LastChar < a )
                                    f->FontInfo.TTFInfo->ANSI->LastChar = a;
                            }
                        }
                        EscToStrm ( strm, a );
                        if ( _BOX->HorizontalScaling != 100.0 )
                            D *= _BOX->HorizontalScaling / 100.0;
                        RS += D;
                        CS += Dx[l];
                        if ( fabs ( D - Dx[l] ) > 0.1 ){
                            l++;
                            k++;
                            break;
                        }
                        l++;
                        k++;
                    }
                    if ( k ){
                        if ( k > 1 )
                            PBXSetCharacterSpacing ( PaintBox, ( CS - RS ) / k );
                        C2S ( '(' );
                        ULStreamSetPosition ( strm, 0 );
                        St2S ( strm );
                        S2S ( ") Tj" );
                        C2S ( _BOX->NextOperatorChar );
                    }
                    if ( l < Len ){
                        R2S ( CS );
                        S2S ( " 0 Td" );
                        C2S ( _BOX->NextOperatorChar );
                    }else
                        break;
                }
            }
            PDFFINALLY ( _LIB ){
                ULStreamClose ( strm );
            }
            PDFFINALLYEND ( _LIB );
			for ( i = 0,RS = 0.0; i < Len; i++ )
				RS += Dx[i];
			RS *= _BOX->CurrentFontSize;
            RS *= _BOX->WT.XScale / _BOX->E2I ;
			PBXDrawUnderAndStrike( PaintBox, RS);
#ifdef WINDOWS_PLATFORM
        }else{
            CP = ULGetCodePageByCharset ( _BOX->Charset );
            is = ( ppInt32 ) strlen ( TextStr );
            sz = MultiByteToWideChar ( CP, 0, TextStr, is, NULL, 0 );
            if ( sz ){
                MB = ( ppUns16 * ) mmalloc ( _LIB, sizeof ( ppUns16 ) * sz );
                PDFTRY ( _LIB ){
                    sz = MultiByteToWideChar ( CP, 0, TextStr, is, (LPWSTR)MB, sz );
                    PBXExtUnicodeTextShow ( PaintBox, MB, sz, Dx );
                }
                PDFFINALLY ( _LIB ){
                    mfree ( _LIB, MB );
                }
                PDFFINALLYEND ( _LIB );
            }
        }
#endif
}
void PBXExtTextOut( PBXHandle PaintBox, ppReal X, ppReal Y,
				   ppReal Orientation, char *TextStr, ppReal *Dx )
{
	ppReal *ara, O, Angle, XI, YI;
	ppInt32 i, k;
	if ( !TextStr )
		return;
	if ( !Dx )
		PBXTextOut ( PaintBox, X, Y, Orientation, TextStr );
	k = ( ppInt32 ) strlen ( TextStr );
	ara = ( ppReal * ) mmalloc ( _LIB, sizeof ( ppReal ) * k );
	PDFTRY ( _LIB ){
		for ( i = 0; i < k; i++ )
			ara[i] = Dx[i] / _BOX->E2I / _BOX->CurrentFontSize;
		Angle =  _BOX->Rotate*90 + Orientation;
        O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
		if ( !Angle ){
			XI=X;
			YI=Y + O;
		} else {
			XI = X + O * sin ( Angle * Pi / 180 );
			YI = Y + O * cos ( Angle * Pi / 180 );
		};
		PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
		PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
		PBXExtTextShow ( PaintBox,TextStr, ara);
		_BOX->TextAngle = Orientation;
	}
	PDFFINALLY ( _LIB ){
		mfree ( _LIB, ara );
	}
	PDFFINALLYEND ( _LIB );    
}

void PBXExtUnicodeTextOut( PBXHandle PaintBox, ppReal X, ppReal Y,
						  ppReal Orientation, PppUns16 Text, ppInt32 Len,
						  ppReal *Dx )
{
	ppReal *ara, O, Angle, XI, YI;
	ppInt32 i;
	if ( !Len )
		return;
	if ( !Dx )
		PBXUnicodeTextOut ( PaintBox, X, Y, Orientation, Text, Len );
	ara = ( ppReal * ) mmalloc ( _LIB, sizeof ( ppReal ) * Len );
	PDFTRY ( _LIB ){
		for ( i = 0; i < Len; i++ )
			ara[i] = Dx[i] * _BOX->E2I / _BOX->CurrentFontSize;
		Angle =  _BOX->Rotate*90 + Orientation;
        O = PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
		if ( !Angle ){
			XI=X;
			YI=Y + O;
		} else {
			XI = X + O * sin ( Angle * Pi / 180 );
			YI = Y + O * cos ( Angle * Pi / 180 );
		};
		PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
		PBXSetTextMatrix ( PaintBox, XI, YI, Angle );
		PBXExtUnicodeTextShow ( PaintBox, Text, Len, ara);
		_BOX->TextAngle = Orientation;
	} PDFFINALLY ( _LIB ){
		mfree ( _LIB, ara );
	}
	PDFFINALLYEND ( _LIB );
}



void PBXTextFromBaseLine( PBXHandle PaintBox, ppBool BaseLine )
{
    _BOX->BaseLine = BaseLine;
}

ppReal PBXGetTextWidth( PBXHandle PaintBox, char *Text )
{
    return PBXGetIntTextWidth ( PaintBox, Text ) * _BOX->I2E;
}

ppReal PBXGetUnicodeWidth( PBXHandle PaintBox, PppUns16 Text, ppInt32 Len )
{
    return PBXGetIntUnicodeWidth ( PaintBox, Text, Len ) * _BOX->I2E;
}

void PBXTextBox( PBXHandle PaintBox, TPDFRect Rect, char *Text,
                TPDFHorJust Hor, TPDFVertJust Vert ); 

void PBXAddCommand ( PBXHandle PaintBox, char * Command )
{
	S2S ( Command );
}

#ifdef WINDOWS_PLATFORM

void PBXSetTransform ( PBXHandle PaintBox, XFORM XF)
{
    ppReal x1, x2, y1, y2;
    _BOX->WT.UseSkip = false;
    if ( XF.eDx == _BOX->WT.EMFMatrix.eDx && XF.eDy == _BOX->WT.EMFMatrix.eDy && 
        XF.eM12 == _BOX->WT.EMFMatrix.eM12 && _BOX->WT.EMFMatrix.eM11 == XF.eM11 && 
        _BOX->WT.EMFMatrix.eM21 == XF.eM21 && _BOX->WT.EMFMatrix.eM22 == XF.eM22)
        return;
    _BOX->WT.EMFMatrix = XF;
    if ( XF.eDx == 0.0 && XF.eDy == 0.0 && XF.eM12 == 0.0 &&
        XF.eM21 == 0.0 && XF.eM11 == 1.0 && XF.eM22 == 1.0){
            _BOX->WT.UseEMF = false;
            if ( _BOX->WT.UseScale ){
                _BOX->WT.STDMatrix =  ULMultiplyXFORM ( _BOX->WT.ScaleMatrix, _BOX->WT.BaseMatrix);
                _BOX->WT.TextMatrix =  _BOX->WT.ScaleMatrix;    
                _BOX->WT.XScale =  _BOX->WT.ScaleMatrix.eM11 * _BOX->E2I ;
                _BOX->WT.YScale =  _BOX->WT.ScaleMatrix.eM22 * _BOX->E2I ; 
            } else {
                _BOX->WT.STDMatrix = _BOX->WT.BaseMatrix;
                _BOX->WT.XScale = _BOX->WT.YScale =  _BOX->E2I;
                return;
            }
        } else {
            _BOX->WT.UseEMF = true;
            if ( _BOX->WT.UseScale ){
                _BOX->WT.STDMatrix =  ULMultiplyXFORM ( XF, ULMultiplyXFORM( _BOX->WT.ScaleMatrix, _BOX->WT.BaseMatrix ) );
                _BOX->WT.TextMatrix = ULMultiplyXFORM ( _BOX->WT.ScaleMatrix, XF );    
                _BOX->WT.Skip =  ULMultiplyXFORM ( _BOX->WT.ScaleMatrix, _BOX->WT.BaseMatrix );
            } else {
                _BOX->WT.STDMatrix = ULMultiplyXFORM ( XF, _BOX->WT.BaseMatrix );
                _BOX->WT.Skip = _BOX->WT.BaseMatrix; 
                _BOX->WT.TextMatrix = XF;
            }
            x1 = 0.0;
            y1 = 0.0;
            x2 = CALCSCALESOURCE;
            y2 = CALCSCALESOURCE;
            PBXExt2Int ( ( PPBX ) PaintBox, &x1, &y1 );
            PBXExt2Int ( ( PPBX ) PaintBox, &x2, &y2 );
            _BOX->WT.XScale =  fabs ( x2- x1 ) / CALCSCALESOURCE;
            _BOX->WT.YScale =  fabs ( y2- y1 ) / CALCSCALESOURCE;
        }

}

void PBXPlayMetaFile(PBXHandle PaintBox, HGDIOBJ Metafile, ppReal X, ppReal Y, ppReal XScale,
                     ppReal YScale)
{
	CheckEdition (_LIB, STD);
    _BOX->WT.ScaleMatrix.eDx = (float)X;
    _BOX->WT.ScaleMatrix.eDy = (float)Y;
    _BOX->WT.ScaleMatrix.eM11 = (float)XScale;
    _BOX->WT.ScaleMatrix.eM22 = (float)YScale;
    _BOX->WT.ScaleMatrix.eM12 = _BOX->WT.ScaleMatrix.eM21 = 0.0;
    _BOX->WT.TextMatrix =  _BOX->WT.ScaleMatrix;    
    _BOX->WT.UseEMF = false;
    _BOX->WT.UseSkip = false;
    _BOX->WT.EMFMatrix.eDx = _BOX->WT.EMFMatrix.eDy = 
        _BOX->WT.EMFMatrix.eM12 = _BOX->WT.EMFMatrix.eM21 = 0.0;
    _BOX->WT.EMFMatrix.eM11 = _BOX->WT.EMFMatrix.eM22 = 1.0;
    if ( X == 0.0 && Y == 0.0 && XScale == 1.0 && YScale == 1.0 ){
        _BOX->WT.UseScale = false;
        _BOX->WT.STDMatrix = _BOX->WT.BaseMatrix;
    } else {
        _BOX->WT.STDMatrix = ULMultiplyXFORM ( _BOX->WT.ScaleMatrix, _BOX->WT.BaseMatrix);
        _BOX->WT.UseScale = true;
    }
    _BOX->WT.XScale = XScale * _BOX->E2I * _BOX->FS;
    _BOX->WT.YScale = YScale * _BOX->E2I * _BOX->FS;
    
    PDFTRY(_LIB){
        PDFEMFExecute(PaintBox,Metafile);
    }PDFFINALLY (_LIB){
        _BOX->WT.UseEMF = _BOX->WT.UseScale = _BOX->WT.UseSkip = false;
        _BOX->WT.XScale = _BOX->E2I * _BOX->FS;
        _BOX->WT.YScale = _BOX->E2I * _BOX->FS;
        _BOX->WT.STDMatrix = _BOX->WT.BaseMatrix;
    }
    PDFFINALLYEND (_LIB);

}
#endif
ppUns32 PBXUnicodeOutBox(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Interval, PppUns16 Text, ppUns32 Size)
{
	ppUns32 Result;
	ppUns32 Off, Len, Total, Space; 
	ppUns32 Lines, CurLine;
	ppUns16 Ch;
	ppReal CWidth, LWidth, SWidth;
	unsigned char a;
	PPDFFontInfo f;
	PPDFTrueTypeInfo font;
	PCodeTableNode Node;


	if ( !Size)
		return 0;
	Result = 0;

	LWidth = PBXEXT2INT(Width);
	Lines = ( ppUns32 ) (Height / Interval);
	f = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
	if ( f->Type != PDFTrueTypeFont )
		PDFRAISE ( _LIB,PDFBuildErrCode ( ErrPageLevel, pleUnicodeAvailableForTrueTypeFontOnlyError ) );
	font = f->FontInfo.TTFInfo;

	PBXBeginText(PaintBox);
	Off = 0;
	Len = 0;
	Total = 0;
	CurLine = 0;
	SWidth = 0;
	while ( Total < Size ){
		Ch = Text[Total];
		if ( Ch == 13 || Ch == 10 ){
			if ( Ch == 13 &&  Total < Size && Text[Total + 1] == 10 )
				Total++;
			PBXUnicodeTextOut(PaintBox, X,Y +CurLine*Interval,0,Text + Off, Len);
			Total++;
			Off = Total;
			Len = 0;
			SWidth = 0;
			CurLine++;
			Space = 0;
			if (CurLine>Lines)
				break;
			continue;
		}
		if ( ULIsNotANSICode ( Ch ) ){
			Node = PDFTTFGetCharCodeTable ( font->Font, Ch );
			CWidth = Node->Width;
		}else{
			a = ( unsigned char ) ULUnicodeToANSI ( Ch );
			CWidth = f->metrics.ANSICodes[a];
		};
		CWidth *= _BOX->CurrentFontSize / 1000;
		if (Ch == 32 && _BOX->WordSpace > 0 )
			CWidth += _BOX->WordSpace;
		if ( _BOX->CharSpace > 0 )
			CWidth += _BOX->CharSpace;
		if ( _BOX->HorizontalScaling != 100.0 )
			CWidth *= _BOX->HorizontalScaling / 100.0;
		if (SWidth + CWidth <= LWidth){
			Total++;
			Len++;
			SWidth += CWidth;
			if (Ch == 32) 
				Space = Len;
		} else {
			if (Space){
				Total = Off + Space;
				Len = Space;
			}
			PBXUnicodeTextOut(PaintBox, X,Y +CurLine*Interval,0,Text + Off, Len);
			
			Off = Total;
			Len = 0;
			SWidth = 0;
			CurLine++;
			Space = 0;
			if (CurLine>=Lines)
				break;
		}
	}
	if (Len){
		PBXUnicodeTextOut(PaintBox, X,Y +CurLine*Interval,0,Text + Off, Len);
	}
	return Total;
}

ppUns32 PBXTextOutBox(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Interval, char * TextStr)
{
#ifdef WINDOWS_PLATFORM
	ppUns16 *MB;
	ppInt32 sz;
	ppInt32 is, CP;
#endif
	ppUns32 Length, i;
	ppUns32 Result;
	ppUns32 Off, Len, Total, Space; 
	ppUns32 Lines, CurLine;
	ppUns8 Ch;
	ppReal XI, YI, CWidth, LWidth, SWidth;
	char *s;
	PPDFFontInfo font;

	if (!TextStr ) 
		return 0;
	Length = strlen(TextStr);
	if ( !Length)
		return 0;
	Result = 0;
	
#ifdef WINDOWS_PLATFORM
	if ( !_BOX->Charset || ( _BOX->Charset == DEFAULT_CHARSET && !ULGetDefFontCharSet () ) ){
#endif
		
		Lines = ( ppUns32 ) (Height / Interval);
		
		PBXBeginText(PaintBox);
		PBXSetCurrentFont ( PaintBox, false );
		font = FONTDoc ( _BOX->Doc, _BOX->CurrentFontIndex );
		if ( font->Type == PDFTrueTypeFont ){
			for (i=0;i < Length;i++){
				Ch = TextStr[i];
				if (Ch < 32)
					continue;
				if ( !( font->FontInfo.TTFInfo->ANSI->UsedChars[Ch] ) ){
					font->FontInfo.TTFInfo->ANSI->UsedChars[Ch] = true;
					font->FontInfo.TTFInfo->ANSI->Modified = true;
					if ( font->FontInfo.TTFInfo->ANSI->FirstChar > Ch )
						font->FontInfo.TTFInfo->ANSI->FirstChar = Ch;
					if ( font->FontInfo.TTFInfo->ANSI->LastChar < Ch )
						font->FontInfo.TTFInfo->ANSI->LastChar = Ch;
				}
			}
		}
		XI = X;
		YI = Y + PBXINT2EXT ( PBXGetIntTextHeight ( PaintBox ) );
		LWidth = PBXEXT2INT(Width);
		PBXExt2Int( ( PPBX ) PaintBox,&XI,&YI);
		PBXSetTextMatrix(PaintBox, XI, YI, 0);
		R2S(PBXEXT2INT(Interval) / _BOX->CurrentFontSize );
		S2S(" TL");
		C2S ( _BOX->NextOperatorChar );
		Off = 0;
		Len = 0;
		Total = 0;
		CurLine = 0;
		SWidth = 0;
		while ( Total < Length ){
			Ch = TextStr[Total];
			if ( Ch == 13 || Ch == 10 ){
				if ( Ch == 13 &&  Total < Len && TextStr[Total + 1] == 10 )
					Total++;
				s = ULGetEscString(_LIB, TextStr + Off, Len );
				C2S('(');
				S2S(s);
				if (CurLine)
					S2S(") '")
				else
					S2S(") Tj");
				C2S ( _BOX->NextOperatorChar );
				mfree ( _LIB, s );
				Total++;
				Off = Total;
				Len = 0;
				SWidth = 0;
				CurLine++;
				Space = 0;
				if (CurLine>Lines)
					break;
				continue;
			}
			CWidth = font->metrics.ANSICodes[(ppUns8)Ch];
			CWidth *= _BOX->CurrentFontSize / 1000;
			if (Ch == 32 && _BOX->WordSpace > 0 )
				CWidth += _BOX->WordSpace;
			if ( _BOX->CharSpace > 0 )
				CWidth += _BOX->CharSpace;
			if ( _BOX->HorizontalScaling != 100.0 )
				CWidth *= _BOX->HorizontalScaling / 100.0;
			if (SWidth + CWidth <= LWidth){
				Total++;
				Len++;
				SWidth += CWidth;
				if (Ch == 32) 
					Space = Len;
			} else {
				if (Space){
					Total = Off + Space;
					Len = Space;
				}
				s = ULGetEscString(_LIB, TextStr + Off, Len );
				C2S('(');
				S2S(s);
				if (CurLine)
					S2S(") '")
				else
				S2S(") Tj");
				C2S ( _BOX->NextOperatorChar );
				mfree ( _LIB, s );
				Off = Total;
				Len = 0;
				SWidth = 0;
				CurLine++;
				Space = 0;
				if (CurLine>=Lines)
					break;
			}
		}
		if (Len){
			s = ULGetEscString(_LIB, TextStr + Off, Len );
			C2S('(');
			S2S(s);
			if (CurLine)
				S2S(") '")
			else
			S2S(") Tj");
			C2S ( _BOX->NextOperatorChar );
			mfree ( _LIB, s );
		}
		return Total;

#ifdef WINDOWS_PLATFORM
	}else{
		CP = ULGetCodePageByCharset ( _BOX->Charset );
		is = ( ppInt32 ) strlen ( TextStr );
		sz = MultiByteToWideChar ( CP, 0, TextStr, is, NULL, 0 );
		if ( sz ){
			MB = ( ppUns16 * ) mmalloc ( _LIB, sizeof ( ppUns16 ) * sz );
			PDFTRY ( _LIB ){
				sz = MultiByteToWideChar ( CP, 0, TextStr, is, (LPWSTR)MB, sz );
				Result = PBXUnicodeOutBox ( PaintBox, X,Y, Width, Height, Interval, MB, sz );
			}
			PDFFINALLY ( _LIB ){
				mfree ( _LIB, MB );
			}
			PDFFINALLYEND ( _LIB );
		}
		return Result;
	}
#endif
 

}




