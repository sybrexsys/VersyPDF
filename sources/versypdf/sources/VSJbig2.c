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
VSJbig2.c
*********************************************************************/

#include <math.h>

#include "VSMisc.h"
#include "VSFilter.h"

#include "VSDocAI.h"
#include "VSPlatform.h"

#include "VSCosAI.h"
#include "../VSError.h"
#include "../VSExcept.h"
#include "VSImageI.h"

#ifdef _LIB
#undef _LIB
#endif

#define _LIB CD(Doc)->Lib
#define _DOC CD(Doc)


//static const char FHeader [ 13 ] = { '\x97', '\x4A', '\x42', '\x32', '\x0D', '\x0A', '\x1A', '\x0A', '\x01', '\x00', '\x00', '\x00', '\x01' };
static const char PHeader [ 11 ] = { '\x00', '\x00', '\x00', '\x01', '\x30', '\x00', '\x01', '\x00', '\x00', '\x00', '\x13' };
static const char SDHeader [ 7 ] = { '\x00', '\x00','\x00', '\x02','\x00','\x01', '\x01' };
static const char TRHeader [ 8 ] = { '\x00', '\x00', '\x00', '\x03', '\x06', '\x20', '\x02', '\x01' };
static const char BlHeader [ 2 ] = { '\x00', '\x11' }; 
//static const char BlankWord [ 2 ] = { '\x00', '\x00' };
//static const char PEof [ 11 ] = { '\x00', '\x00', '\x00', '\x03', '\x31', '\x00', '\x01', '\x00', '\x00', '\x00', '\x00' };
static const char TRFlags [ 4 ] = { '\x0C', '\x19', '\x00', '\x10' };

static const
ppUns32 b1[4][5] ={
    {0x00, 0x10, 272, 65808, 0xFFFFFFFF},
    {0x00, 0x00, 0x02, 0x06, 0x07},
    {0x00, 0x04, 0x08, 0x10, 0x20},
    {0x00, 0x01, 0x02, 0x03, 0x03}
};

static const
ppInt32 b3[4][8] = {
    {-257,   -1, 0x00, 0x01, 0x02, 0x0A, 74, 0xFFFF},
    {0xFF, 0xFE, 0x00, 0x02, 0x06, 0x0E, 0x1E, 0x7E},
    {0x20, 0x08, 0x00, 0x00, 0x00, 0x03, 0x06, 0x20},
    {0x08, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07}
};

static const
ppUns32 b4[4][6] = {
    {0x01, 0x02, 0x03, 0x0B, 0x4B, 0xFFFF},
    {0x00, 0x02, 0x06, 0x0E, 0x1E, 0x1F},
    {0x00, 0x00, 0x00, 0x03, 0x06, 0x20},
    {0x01, 0x02, 0x03, 0x04, 0x05, 0x05}
};

static const
ppInt32 b6[4][14] = {
    {-2048, -1024, -512, -256, -128, -64, -32, 0x00, 0x80, 0x100, 0x200, 0x400, -2049, 0x800},
    {0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x05, 0x07, 0x07, 0x08, 0x09, 0x0A, 0x20, 0x20},
    {0x1C, 0x08, 0x09, 0x0A, 0x1D, 0x1E, 0x0B, 0x00, 0x02, 0x03, 0x0C, 0x0D, 0x3E, 0x3F},
    {0x05, 0x04, 0x04, 0x04, 0x05, 0x05, 0x04, 0x02, 0x03, 0x03, 0x04, 0x04, 0x06, 0x06}
};

static const
    ppInt32 b8[4][20] = {
        {-15, -7, -5, -3, -2, -1, 0, 2, 3, 4, 20, 22, 38, 70, 134, 262, 390, 646, -16, 1670},
        {3, 1, 1, 0, 0, 0, 1, 0, 0, 4, 1, 4, 5, 6, 7, 7, 8, 10, 32, 32},
        {0xFC, 0x1FC, 0xFD, 0x1FD, 0x7C, 0xA, 0, 0x1A, 0x3A, 4, 0x3B, 0xB, 0xC, 0x1B, 0x1C, 0x3C, 0x7D, 0x3D, 0x1FE, 0x1FF},
        {8, 9, 8, 9, 7, 4, 2, 5, 6, 3, 6, 4, 4, 5, 5, 6, 7, 6, 9, 9}
    };

static const
    ppInt32 b12[4][14] = {
        {1, 2, 3, 5, 6, 8, 10, 11, 13, 17, 25, 41, 73, 0xFFFF},
        {0, 0, 1, 0, 1, 1, 0, 1, 2, 3, 4, 5, 32, 0},
        {0, 2, 6, 0x1C, 0x1D, 0x3C, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0xFE, 0xFF, 0},
        {1, 2, 3, 5, 5, 6, 7, 7, 7, 7, 7, 8, 8, 0}
    };



typedef struct TTreeItem {
    ppInt32 Parent;
    ppInt32 Index;
    ppInt32 Value;
    ppBool  Branch;
} TTreeItem;

typedef struct TPoint {
    ppInt32 x;
    ppInt32 y;
} TPoint;

typedef struct TIndexItem {
    ppUns32 Index;
    ppUns32 Value;
} TIndexItem;

typedef struct TBorder{
    TPoint LeftTop;
    TPoint RightBottom;
} TBorder;

typedef struct BLPoint {
    ppBool ThereIs;
    TPoint Point;
} BLPoint;

typedef struct TSymbol{
    ppUns32 x;
    ppUns32 y;
    ppInt32 Pict;
} TSymbol;

typedef struct TDict {
    ppInt32 Dw;
    ppUns32 Code;
    ppUns8  CodeLen;
    PMImage Pict;
    ppInt32 BMWidth;
    ppInt32 BMHeight;
} TDict;

typedef struct TJBIG2Comp {
    PDFLibHandle Lib;
    ppUns8  Lossy;
    ppUns8  *MatrixPix;
    ppInt32 PrevMatrixI;
    ppUns32 SymbCount;
    TSymbol *Symbols;
    ppUns32 SymbolsCount;
    TDict   *SymbTemplates;
    ppUns32 SymbTemplatesCount;
    ppUns32 STLength;
    TIndexItem *UseTime;
    ppUns32 UseTimeCount;
    ppInt32 MaxH;
    ppInt32 TotalWidth;
    ppInt32 *PrefLen;
    ppUns32 PrefLenCount;
    ppInt32 *Codes;
    ppUns32 CodesCount;
    TPoint  PictRestrict;
} TJBIG2Comp, * PJBIG2Comp;



PJBIG2Comp JBIG2InitCompression ( PDFLibHandle Lib )
{
    PJBIG2Comp Tmp;
    Tmp = ( PJBIG2Comp ) mmalloc ( Lib, sizeof ( TJBIG2Comp ) );
    memset ( Tmp, 0, sizeof (TJBIG2Comp ) );
    Tmp->Lib = Lib;
    return Tmp;
}

void JBIG2DoneCompression ( PJBIG2Comp JBIG2 )
{
    PDFLibHandle Lib = JBIG2->Lib;
    ppUns32 i;
    if ( JBIG2->Symbols )
        mfree ( Lib, JBIG2->Symbols );
    if ( JBIG2->SymbTemplates ){
        for ( i = 0; i < JBIG2->SymbTemplatesCount; i++)
            PDFImageFree( JBIG2->SymbTemplates[i].Pict );
        mfree ( Lib, JBIG2->SymbTemplates );
    }     
    if ( JBIG2->UseTime )
        mfree ( Lib, JBIG2->UseTime );
    if ( JBIG2->Codes )
        mfree ( Lib, JBIG2->Codes );
    if ( JBIG2->PrefLen )
        mfree ( Lib, JBIG2->PrefLen );
    mfree ( Lib, JBIG2 );
}


void JBIG2RectCopyToRect ( PMImage SourceImg, PMImage TargetImg, ppUns32 FromX, ppUns32 FromY,
					 ppUns32 ToX, ppUns32 ToY, ppUns32 Width, ppUns32 Height)
{
	ppUns32 SourceByteOffset =  FromX  >> 3;
	ppUns32 TargetByteOffset = ToX >> 3;
	ppUns32 SBitOff = FromX - ( ( FromX >> 3) << 3 );
	ppUns32 TBitOff = ToX -  ( ( ToX >> 3) << 3 );
	ppUns8 *Target, *Source, BT , BL, BH;
	ppUns8 THiBitsMask = (ppUns8)(( 0xFF >> (8 - TBitOff ) ) << ( 8 - TBitOff ));
	ppUns8 TLowBitsMask = (ppUns8)( (ppUns8)( 0xFF << TBitOff ) >> TBitOff );
	ppUns32 i, Tc, Sc;
	ppInt32 size;
	for ( i = 0; i < Height; i++ ){
		Source = ScanLine ( SourceImg, FromY + i );
		Target = ScanLine ( TargetImg, ToY + i );
		size = Width;
		Sc = SourceByteOffset;
		Tc = TargetByteOffset;
		while ( size > 0 ){
			if ( !SBitOff ){
				BT = Source [ Sc ];
			} else {
				BH = (ppUns8)( Source [ Sc ] << SBitOff );
				BL = (ppUns8) ( Source [ Sc + 1 ] >> ( 8 - SBitOff ));
				BT = BH | BL;
			}
			if ( size < 8 ){
				BT = BT | (ppUns8)( ( ppUns8 ) (0xFF << size ) >> size ); 
			}
			if ( !TBitOff ){
				if ( size < 8 ) {
					Target[Tc] = Target[Tc] & BT;
				} else {
					Target[Tc] = BT;
				}		
			} else {
				Target[Tc] = (ppUns8) ( Target[Tc] & ( THiBitsMask | ( BT >> TBitOff)) );
				Target[Tc+1] = (ppUns8) ( Target[Tc+1] & ( TLowBitsMask | ( BT << ( 8 - TBitOff ) ) ) );
			}
			size -= 8;
			Sc++;
			Tc++;
		}
	}

}

void JBIG2AssignPrefCodes ( PJBIG2Comp JBIG2, ppUns32 NTemp )
{
    ppInt32  CurCode, CurLen, LenMax ;
    ppUns32 CurTemp, i;
    ppInt32 *LenCount, *FirstCode;
    LenMax = 0;
    for ( i = 0; i < NTemp; i++ ){
        if ( JBIG2->PrefLen [ i ] > LenMax )
            LenMax = JBIG2->PrefLen [ i ];
    }
    LenCount = ( ppInt32 *) mmalloc ( JBIG2->Lib, ( LenMax + 1 ) * sizeof ( ppInt32 ) );
    memset ( LenCount, 0, ( LenMax + 1 ) * sizeof ( ppInt32 ) );
    for ( i = 0; i < NTemp; i++){
        LenCount [ JBIG2->PrefLen [ i ] ]++;
    }
    FirstCode = ( ppInt32 *) mmalloc ( JBIG2->Lib, ( LenMax + 1 ) * sizeof ( ppInt32 ) );
    JBIG2->Codes = ( ppInt32 *) mrealloc ( JBIG2->Lib, JBIG2->Codes, NTemp * sizeof ( ppInt32 ) );
    if ( JBIG2->CodesCount < NTemp ){
        memset ( &(JBIG2->Codes[JBIG2->CodesCount]), 0, ( NTemp - JBIG2->CodesCount) *  sizeof ( ppInt32 ) );
    }
    JBIG2->CodesCount = NTemp;
    CurLen = 1;
    FirstCode [ 0 ] = 0;
    LenCount [ 0 ] = 0;
    while ( CurLen <= LenMax ) {
        FirstCode [ CurLen ] = ( FirstCode [ CurLen - 1 ] + LenCount [ CurLen - 1 ] ) * 2;
        CurCode = FirstCode [ CurLen ];
        CurTemp = 0;
        while ( CurTemp < NTemp ) {
            if ( JBIG2->PrefLen [ CurTemp ] == CurLen ){
                JBIG2->Codes [ CurTemp ] = CurCode;
                CurCode++;
            }
            CurTemp++;
        }
        CurLen++;
    }
    mfree ( JBIG2->Lib, LenCount );
    mfree ( JBIG2->Lib, FirstCode );
}

ppBool JBIG22GetPixel( PMImage ImageStream, ppUns32 x, ppUns32 y, ppUns8 **P, ppInt32 *PrevY )
{
    ppBool Tmp = false;
    if (  x >= ImageStream->Width || y >= ImageStream->Height )
        return Tmp;
    if ( *PrevY != (ppInt32)y ){
        *P = ScanLine ( ImageStream, y );
        *PrevY = y;
    }
    if ( ( ( (  ~ ( (*P)[ x >> 3 ] ) ) >> ( 7-( x - ( 8*( x >> 3 ) ) ) ) ) & 1 ) == 1 )
        Tmp = true;
    return Tmp;
}

#define NextPixel \
    switch ( Direct ) {\
        case 1 :\
            x++;\
    	    break;\
        case 2 :\
            y++;\
            break;\
        case 3 :\
            x--;\
            break;\
        case 4 :\
            y--;\
            break;\
        default:;\
    };

#define LoopDirect { if ( Direct < 1 ) Direct = 4; if ( Direct > 4 ) Direct = 1; }

#define GetDirect \
{\
      ppInt32 TX = x;\
      ppInt32 TY = y;\
      Direct++;\
      LoopDirect;\
      NextPixel;\
      if ( JBIG22GetPixel ( Table, x, y, &P, &PrevY ) ){\
          x = TX;\
          y = TY;\
          Direct--;\
          LoopDirect;\
          NextPixel;\
          if ( JBIG22GetPixel ( Table, x, y, &P, &PrevY ) ){\
              x = TX;\
              y = TY;\
              Direct--;\
              LoopDirect;\
              NextPixel;\
              if ( JBIG22GetPixel ( Table, x, y, &P, &PrevY ) ){\
                  x = TX;\
                  y = TY;\
                  Direct--;\
                  LoopDirect;\
                  NextPixel;\
              }\
          }\
      }\
};


TBorder JBIG2GetBorder ( PJBIG2Comp JBIG2, ppInt32 x, ppInt32 y, PMImage Table )
{
    ppUns8 Direct;
    TPoint FirstPoint;
    ppUns8 *P;
    ppInt32 PrevY;
    TBorder Result;
    Result.LeftTop.x = x;
    Result.LeftTop.y = y;
    Result.RightBottom.x = x;
    Result.RightBottom.y = y;
    PrevY = -2;
    Direct = 4;
    GetDirect;

    FirstPoint.x = x;
    FirstPoint.y = y;
    if ( x < Result.LeftTop.x )
        Result.LeftTop.x = x + 1;
    if ( y < Result.LeftTop.y )
        Result.LeftTop.y = y + 1;
    if ( x > Result.RightBottom.x )
        Result.RightBottom.x = x - 1;
    if ( y > Result.RightBottom.y )
        Result.RightBottom.y = y - 1;
    GetDirect;
    
    while ( FirstPoint.x != x || FirstPoint.y != y ) {
        if ( x < Result.LeftTop.x )
            Result.LeftTop.x = x + 1;
        if ( y < Result.LeftTop.y )
            Result.LeftTop.y = y + 1;
        if ( x > Result.RightBottom.x )
            Result.RightBottom.x = x - 1;
        if ( y > Result.RightBottom.y )
            Result.RightBottom.y = y - 1;
        GetDirect;
    }
    return Result;
}

BLPoint JBIG2GetFirstBlackPoint ( PJBIG2Comp JBIG2, ppInt32 PrevY, PMImage Table )
{
    BLPoint Result;
    ppUns32 x, y;
    ppUns8 *P;
    ppBool Black;
    ppUns32 BitWidth;
    ppUns8 cx, shByte;
    Black = false;
    Result.ThereIs = false;
    if ( Table->Width < 8 ){
        BitWidth = 1;
    } else {
        BitWidth = Table->Width >> 3;
    }
    for ( y = PrevY; y < Table->Height; y++ ){
        P = ScanLine ( Table, y );
        for ( x = 0; x < BitWidth; x++ ){
            if ( Table->Width < 8 ){
                shByte = (ppUns8) (0xFF >> ( 8 - Table->Width ));
                if ( P [ x ] != shByte ){
                    Black = true;
                    break;
                }
            } else {
                if ( P [ x ] != 0xFF ){
                    Black = true;
                    break;
                }
            }
        }
        if ( Black )
            break;
    }
    if ( Black ){
        Result.ThereIs = true;
        cx = ~P [ x ];
        Result.Point.x = ( x << 3 ) + ( 8 - ( ppInt32 )( log2 ( (double) cx ) ) ) - 1;
        Result.Point.y = y;
    }
    return Result;
}
ppBool JBIG2PixelIsBlack ( PJBIG2Comp JBIG2, ppInt32 h, ppInt32 i, PMImage matrix)
{
    if ( h < 0 || i < 0 || h >= ( ppInt32 )matrix->Width || i >= ( ppInt32 ) matrix->Height )
        return false;
    if ( JBIG2->PrevMatrixI != i ){
        JBIG2->MatrixPix = ScanLine ( matrix, i );
        JBIG2->PrevMatrixI = i;
    }
    return (((~ ( JBIG2->MatrixPix [  h >> 3 ] ) ) >> ( 7 - ( h - ( ( h >> 3) << 3) ) ) ) & 1 ) ? true : false; 


}

ppBool JBIG2GetFourNeighPixel ( PJBIG2Comp JBIG2, ppInt32 h, ppInt32 i, ppInt32 min, PMImage matrix )
{
    ppInt32 xi, yi;
    ppUns8 Count, Accuracy;
    h--;
    i--;
    Count = 0;
    switch ( JBIG2->Lossy ){
        case 0: 
            Accuracy = 4;
            break;
        case 1:
            Accuracy = 6;
            break;
        default:
            Accuracy = 8;
            break;
    }
    JBIG2->PrevMatrixI = -2;
    if ( min < 13 || matrix->Width < 5 )
        Accuracy = 1;
    for ( xi = h; xi <= h + 2; xi++ ){
        for ( yi = i; yi <= i + 2; yi++ ){
            if ( JBIG2PixelIsBlack ( JBIG2, xi, yi, matrix ) )
                Count++;
            if ( Count >= Accuracy ){
                return true;
            }
        }
    }
    if ( Count >= Accuracy)
        return true;
    return false;
}

void JBIG2ClearRectangle( PMImage Image, TPoint lt, TPoint br )
{
    ppInt32 i;
    ppInt32 h;
    ppUns8 mask;
    ppUns8* ptr;
    for ( i = lt.y; i <= br.y; i++ ){
        ptr = ScanLine( Image, i );
        if ( ( lt.x >> 3 ) == ( br.x >> 3 ) ){
            mask = ( ppUns8 )( ( ( 0xFF << ( 8 - ( lt.x - ( ( lt.x >> 3 ) << 3 ) ) ) ) | 
                ( 0xFF >> ( 1 + ( br.x - ( ( br.x >> 3 ) << 3 ) ) ) ) ) );
            ptr[ lt.x >> 3 ] = ptr[ lt.x >> 3 ] | ~mask;
        } else {
            mask = ( ppUns8 )( 0xFF << ( 8 - ( lt.x - ( ( lt.x >> 3 ) << 3 ) ) ) );
            ptr[ lt.x >> 3 ] = ptr[ lt.x >> 3 ] | ~mask;
            if ( ( ( lt.x >> 3 ) + 1 ) < ( br.x >> 3 ) ){
                for ( h = ( lt.x >> 3 ) + 1 ; h < ( br.x >> 3 ); h++ )
                    ptr[ h ] = ptr[ h ] | 0xFF ;
            }
            mask = (ppUns8)( 0xFF >> ( 1 + ( br.x - ( ( ( br.x >> 3 ) << 3 ) ) ) ) );
            ptr[ br.x >> 3 ] = ptr[ br.x >> 3 ] | ~mask;
        }
    }
}


ppInt32 JBIG2MatchTemplate ( PJBIG2Comp JBIG2, PMImage Image, PMImage Glyph )
{
    ppUns8 * T, * P, * B;
    ppUns32 i, h, g, minh, W, H;
	ppInt32 Result = -1;
    ppUns8 Pxy, Bxy;
    ppBool AllRight;
    PMImage TempBMP;
    
    for ( g = 0; g < JBIG2->STLength; g++){
        if ( JBIG2->SymbTemplates [ g ].BMHeight > ( ppInt32 ) Glyph->Height || 
            JBIG2->SymbTemplates [ g ].BMWidth > ( ppInt32 )Glyph->Width )
            continue;
        if ( JBIG2->SymbTemplates [ g ].Pict->Height > Glyph->Height ){
            H = JBIG2->SymbTemplates [ g ].Pict->Height;
            minh = Glyph->Height;
        } else {
            H = Glyph->Height;
            minh = JBIG2->SymbTemplates [ g ].Pict->Height;
        }
        W = max ( JBIG2->SymbTemplates [ g ].Pict->Width, Glyph->Width );
        TempBMP = ( PMImage )PDFImageCreate( JBIG2->Lib, W, H, it1bit, cgGray );
        PDFTRY ( JBIG2->Lib ){    
            P = ScanLine ( JBIG2->SymbTemplates[ g ].Pict, 0 );
            B = ScanLine ( Glyph, 0 );
            for ( i = 0; i < H; i++ ) {
                if ( i < JBIG2->SymbTemplates[ g ].Pict->Height )
                    P = ScanLine ( JBIG2->SymbTemplates[ g ].Pict, i );
                if ( i < Glyph->Height )
                    B = ScanLine ( Glyph, i );
                T = ScanLine ( TempBMP, i );
                for ( h = 0; h <= ( ( W - 1 ) >> 3); h++ ){
                    if ( i < JBIG2->SymbTemplates[g].Pict->Height && 
                        ( h << 3 ) < JBIG2->SymbTemplates[g].Pict->Width ){
                            Pxy = P [ h ];
                        } else {
                            Pxy = 0xFF;
                        }
                        if ( i < Glyph->Height && ( h << 3) <= Glyph->Width ){
                            Bxy = B [ h ];
                        } else {
                            Bxy = 0xFF;
                        }
                        T [ h ] = ( ppUns8 )(  ~ ( (  ~ ( Pxy ) ) ^ (  ~ ( Bxy ) ) ) );
                }
            }
            JBIG2->PrevMatrixI = -2;
            AllRight = true;
            for ( i = 0; i < H; i++ ) {
                for ( h = 0; h < W ; h++ ){
                    if ( JBIG2PixelIsBlack( JBIG2, h, i, TempBMP ) ){
                        if ( JBIG2GetFourNeighPixel ( JBIG2, h, i, minh, TempBMP ) ){
                            AllRight = false;
                            break;
                        }
                    }
                }
                if ( ! AllRight )
                    break;
            }
        } PDFFINALLY ( JBIG2->Lib ){
            PDFImageFree( TempBMP );
        } PDFFINALLYEND ( JBIG2->Lib );
        if ( AllRight ){
            Result = g;
            break;
        }
    }  
    return Result;
}

void JBIG2GetSymbol ( PJBIG2Comp JBIG2, TBorder Bord, PMImage Table)
{
    PMImage  Symb;
    Symb = ( PMImage )PDFImageCreate ( JBIG2->Lib, Bord.RightBottom.x - Bord.LeftTop.x + 1, 
        Bord.RightBottom.y - Bord.LeftTop.y + 1 , it1bit, cgGray);
    PDFTRY ( JBIG2->Lib ){
		memset( Symb->ImageBuffer, 0xFF, Symb->LineSize * Symb->Height );
		JBIG2RectCopyToRect( Table, Symb, Bord.LeftTop.x, Bord.LeftTop.y, 0,0,
			Symb->Width, Symb->Height );
        JBIG2ClearRectangle ( Table, Bord.LeftTop, Bord.RightBottom );
        JBIG2->Symbols = ( TSymbol * ) mrealloc ( JBIG2->Lib, JBIG2->Symbols, ( ++JBIG2->SymbolsCount ) * sizeof ( TSymbol ) );
        JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].x =Bord.LeftTop.x;
        JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].y =Bord.LeftTop.y;
        if ( JBIG2->STLength > 0 ){
            JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].Pict = JBIG2MatchTemplate( JBIG2, Table, Symb );
        } else {
            JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].Pict = -1;
        }
        if ( JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].Pict == -1 ){
            JBIG2->SymbCount++;
            JBIG2->STLength++;
            JBIG2->SymbTemplates = ( TDict * ) mrealloc ( JBIG2->Lib, JBIG2->SymbTemplates, ( ++JBIG2->SymbTemplatesCount ) * sizeof ( TDict ) );
            JBIG2->UseTime = ( TIndexItem * ) mrealloc ( JBIG2->Lib, JBIG2->UseTime, ( ++JBIG2->UseTimeCount ) * sizeof ( TIndexItem ) );
            JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].Pict = ( PMImage )PDFImageCreate ( JBIG2->Lib, Symb->Width, Symb->Height, it1bit, cgGray );
            JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].BMWidth = Symb->Width;
            JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].BMHeight = Symb->Height;
            if ( ( ppInt32 ) Symb->Height > JBIG2->MaxH )
                JBIG2->MaxH = Symb->Height;
            if ( JBIG2->STLength > 1){
                JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].Dw = Symb->Width - JBIG2->TotalWidth;
                JBIG2->TotalWidth += JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].Dw;
            } else {
                JBIG2->SymbTemplates[ 0 ].Dw = Symb->Width;
                JBIG2->TotalWidth = Symb->Width;
            }
            memcpy( JBIG2->SymbTemplates[ JBIG2->STLength - 1 ].Pict->ImageBuffer, Symb->ImageBuffer, Symb->LineSize * Symb->Height );
            JBIG2->UseTime [JBIG2->UseTimeCount - 1].Value = 1;
            JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].Pict = JBIG2->STLength - 1;
        } else {
            JBIG2->UseTime [ JBIG2->Symbols [ JBIG2->SymbolsCount - 1 ].Pict ].Value ++;            
        }
        JBIG2->SymbCount++;
    } PDFFINALLY ( JBIG2->Lib ){
        PDFImageFree ( Symb );
    } PDFFINALLYEND ( JBIG2->Lib );
}

void JBIG2Sort ( ppBool ByIndex, TTreeItem *HuffTree, ppUns32 TreeSize, TIndexItem * RC, ppUns32 RCSize)
{
    ppInt32 i =  4, h, g;
    ppInt32 CurLen;
    TIndexItem Tmp;
    TTreeItem TrTmp;
    ppBool IsSort;
    while ( i >= 1 ){
        h = 0;
        CurLen = ( ByIndex ?  TreeSize : RCSize ) - i;
        while ( h < i ){
            IsSort = true;
            while ( IsSort ){
                IsSort = false;
                g = h;
                while ( g < CurLen ){
                    if ( ByIndex ){
                        if ( HuffTree [ g ].Index > HuffTree [ g + i ].Index ){
                            IsSort = true;
                            TrTmp = HuffTree [ g + i ];
                            HuffTree [ g + i ] = HuffTree [ g ];
                            HuffTree [ g ] = TrTmp;
                        }
                    } else {
                        if ( RC [ g ].Value > RC [ g + i ].Value ){
                            IsSort = true;
                            Tmp = RC [ g + i ];
                            RC [ g + i ] = RC [ g ];
                            RC [ g ] = Tmp;
                        }
                    }
                    g += i;
                }              
            }
            h++;
        }
        i >>= 1;
    }
}

void JBIG2AddRun ( PDFLibHandle Lib, ppBool CloseStream, ppInt32 * RCodes, ppInt32 ** Runs, ppUns32 *RunsLen,
                     ppInt32* CurrRun, ppInt32* LenCurrRun, ppInt32 CurrInd )
{
    ppInt32 v, a;
    if ( CurrInd == *CurrRun && !CloseStream ){
        (*LenCurrRun) ++;
        return;
    }
    if ( *CurrRun == 0 ){
        if ( *LenCurrRun < 3){
            if ( *LenCurrRun > 0){
                a = *RunsLen;
                *RunsLen += *LenCurrRun;
                *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, ( *RunsLen ) * sizeof ( ppInt32 ) );
            }
            for ( v = 1; v <= *LenCurrRun; v++ ){
                (*Runs) [ a + v - 1 ]= 0;
                ( RCodes [ 0 ] ) ++;
            }
        } else {
            if ( *LenCurrRun < 11 ){
                RCodes [ 33 ] ++ ; 
                *RunsLen += 2;
                *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, ( *RunsLen ) * sizeof ( ppInt32 ) );
                ( *Runs ) [ *RunsLen - 2 ] = 33;
                ( *Runs ) [ *RunsLen - 1 ] =  *LenCurrRun - 3;              
            } else {
                RCodes [ 34 ] ++ ;
                *RunsLen += 2;
                *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, ( *RunsLen ) * sizeof ( ppInt32 ) );
                (*Runs )[ *RunsLen - 2 ] = 34;
                (*Runs )[ *RunsLen - 1 ] =  *LenCurrRun - 11;              
            }
        }
    *LenCurrRun = 1;
    *CurrRun = CurrInd;
    return;
    }
    if ( *LenCurrRun < 4){
        if ( *LenCurrRun > 0){
            a = *RunsLen;
            *RunsLen += *LenCurrRun;
            *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, ( *RunsLen ) * sizeof ( ppInt32 ) );
        }
        for ( v = 1; v <= *LenCurrRun; v++ ){          
            (*Runs) [ a - 1 + v ]= *CurrRun;
            ( RCodes [ *CurrRun ] ) ++;
        }
    } else {
        if ( *LenCurrRun < 8 ){
            RCodes [ *CurrRun ] ++ ;
            *RunsLen += 3 ;
            *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, (*RunsLen ) * sizeof ( ppInt32 ) );
            (*Runs) [ *RunsLen - 3 ] = *CurrRun;
            (*Runs) [ *RunsLen - 2 ] = 32;
            (*Runs) [ *RunsLen - 1 ] =  *LenCurrRun - 4;              
            RCodes [ 32 ] ++ ;
        } else {
            a = ( ppInt32 )( *LenCurrRun / 7 );
            for ( v = 1; v <= a ; v++ ){
                RCodes [ *CurrRun ]++;
                 *RunsLen += 3 ;
                *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, (*RunsLen) * sizeof ( ppInt32 ) );
                (*Runs) [ *RunsLen - 3 ] = *CurrRun;
                (*Runs) [ *RunsLen - 2 ] = 32;
                (*Runs) [ *RunsLen - 1 ] = 3;
                RCodes [ 32 ]++;
                *LenCurrRun -=  ((ppInt32)(*LenCurrRun / 7 )) * 7;
            }
            if ( *LenCurrRun - ( ( *LenCurrRun / 7) * 7 ) > 3){
                RCodes [ *CurrRun ]++;
                *RunsLen += 3;
                *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, ( *RunsLen ) * sizeof ( ppInt32 ) );
                (*Runs) [ *RunsLen - 3 ] = *CurrRun;
                RCodes [ *CurrRun ] ++;
                (*Runs) [ *RunsLen - 2 ] = 32;
                (*Runs) [ *RunsLen - 1 ] = *LenCurrRun - 4;
                RCodes [ 32 ]++;
            } else {
                for ( v = 1; v <= *LenCurrRun; v++ ){
                     *RunsLen += 1 ;
                    *Runs = (ppInt32 * )mrealloc ( Lib, *Runs, (*RunsLen ) * sizeof ( ppInt32 ) );
                    (*Runs) [ *RunsLen - 1 ] = *CurrRun;
                    RCodes [ *CurrRun ]++;
                }
            }
        }
    }
    *LenCurrRun = 1;
    *CurrRun = CurrInd;
}

void JBIG2InitRC ( PJBIG2Comp JBIG2, TIndexItem **RC, ppUns32 *RCLen )
{
    PDFLibHandle Lib = JBIG2->Lib;
    ppUns32 i,a;
    for ( i = 0, a = 0; i < JBIG2->UseTimeCount; i++ ){
        if ( JBIG2->UseTime [ i ].Value > 0 )
            a++;
    }
    *RCLen = a;
    if ( *RC )
        mfree ( Lib, *RC );
    *RC = ( TIndexItem * ) mmalloc ( Lib, a * sizeof ( TIndexItem ) );
    for ( i = 0, a = 0; i < JBIG2->UseTimeCount; i++ )
        if ( JBIG2->UseTime [ i ].Value > 0 ){
            ( (*RC) [ a ] ).Value = JBIG2->UseTime [ i ].Value;
            ( (*RC) [ a ] ).Index = a;
            JBIG2->UseTime [ i ].Index = a++;
        }
}
void JBIG2ReCompose ( TIndexItem *RC, ppInt32 RCLen, ppUns32 SubRoot, ppInt32 StIndex, ppInt32 ParID, ppUns32 SortFlag )
{
    ppInt32 u, i;
    if ( StIndex + 1 < RCLen && RC [ StIndex + 1 ].Value <= SubRoot ){
        u = StIndex + 1;
        while ( u < RCLen && ( RC [ u ].Value < SubRoot || ( RC [ u ].Value == SubRoot && RC [ u ].Index > SortFlag ) ) )
            u++;
        for ( i = StIndex + 1; i < u; i++ )
            RC [ i - 1 ] = RC [ i ];
        RC [ u - 1 ].Value = SubRoot;
        RC [ u - 1 ].Index = ParID;
    }
}

ppInt32 JBIG2GetWayLength ( TTreeItem * HuffTree, ppUns32 TreeLen, ppInt32 Ind)
{
    ppInt32 Result, par;
    Result = 1;
    if ( TreeLen > 1 ){
        par = HuffTree [ Ind ].Parent;
        while ( HuffTree [ par - 1 ].Parent > 0 ){
            Result++;
            par = HuffTree [ par - 1 ].Parent;
        }
    }
    return Result;
}

void JBIG2BuildTree ( PDFLibHandle Lib, TIndexItem *RC, ppUns32 RCLen, TTreeItem **HuffTree, ppUns32 *TreeLen )
{
    ppInt32 rind, ParentInd;
    ppUns32 i, t;
    t = *TreeLen + ( RCLen > 1 ? ( RCLen - 1 ) << 1 : 0 ) + 1 ;
    *HuffTree = (TTreeItem* )mrealloc ( Lib, *HuffTree, t * sizeof ( TTreeItem ) );
    *TreeLen = t;
    for ( i = 0, rind = 1; i < RCLen - 1; i++, rind++ ){
        ParentInd = rind + RCLen;
        t = rind << 1;
        (*HuffTree)[ t - 2 ].Value = RC [ i ].Value;
        (*HuffTree)[ t - 2 ].Index = RC [ i ].Index;
        (*HuffTree)[ t - 2 ].Parent = ParentInd;
        (*HuffTree)[ t - 2 ].Branch = false;
        (*HuffTree)[ t - 1 ].Value = RC [ i + 1 ].Value;
        (*HuffTree)[ t - 1 ].Index = RC [ i + 1 ].Index;
        (*HuffTree)[ t - 1 ].Parent = ParentInd;
        (*HuffTree)[ t - 1 ].Branch = true;
        RC [ i + 1 ].Value = (*HuffTree)[ t - 2 ].Value +(*HuffTree)[ t - 1 ].Value;
        RC [ i + 1 ].Index = ParentInd;
        JBIG2ReCompose( RC, RCLen, RC [ i + 1].Value, i + 1, ParentInd, RCLen );
    }
    t = *TreeLen;
    if ( t == 1 ){
        (*HuffTree)[ 0 ].Value = RC [ 0 ].Value;
        (*HuffTree)[ 0 ].Index = 0;
        (*HuffTree)[ 0 ].Parent = 0;
    } else {
        t--;
        (*HuffTree)[ t ].Value = (*HuffTree)[ t - 1 ].Value + (*HuffTree)[ t - 2 ].Value;
        (*HuffTree)[ t ].Index =  (*HuffTree)[ t - 1 ].Parent;
        (*HuffTree)[ t ].Parent = 0;
    }
    JBIG2Sort ( true, *HuffTree, *TreeLen, RC, RCLen );
}


void JBIG2CodeAndAdd ( ULBitStream * Bits, ppInt32 Number, ppBool First)
{
    ppInt32 s, len ;
    len = First ? 11: 17;
    for ( s = len; s >= 0; s-- ){
        if ( First ){
            if ( Number <= b6 [ 0 ][ 12 ] ){
                ULBitStreamPutBits ( Bits, b6 [ 2 ] [ 12 ], b6 [ 3] [ 12 ]);
                ULBitStreamPutBits ( Bits, -Number + b6 [ 0 ] [ 12 ]  , b6 [ 1 ] [ 12 ] );
                break;
            }
            if ( Number >= b6 [ 0 ] [ 13 ]) {
                ULBitStreamPutBits ( Bits, b6 [ 2 ] [ 13 ], b6 [ 3 ][ 13 ] );
                ULBitStreamPutBits ( Bits, Number - b6 [ 0 ] [ 13 ], b6 [ 1 ] [ 13 ] );
                break;
            }
            if ( Number >= b6 [ 0 ] [ 11 ] && Number < b6 [ 0 ] [ 13 ] ){
                ULBitStreamPutBits ( Bits, b6 [ 2 ] [ 11 ], b6 [ 3 ] [ 11 ] );
                ULBitStreamPutBits ( Bits, Number - b6 [ 0 ] [ 11 ], b6 [ 1 ] [ 11 ] );
                break;
            }
            if ( Number < b6 [ 0 ] [ s + 1 ] && Number >= b6 [ 0 ] [ s ] ) {
                ULBitStreamPutBits ( Bits, b6 [ 2 ] [ s ], b6 [ 3 ] [ s ] );
                ULBitStreamPutBits ( Bits, Number - b6 [ 0 ] [ s ], b6 [ 1 ] [ s ] );
                break;
            }
        } else {
            if ( Number <= b8 [ 0 ] [ 18 ]) {
                ULBitStreamPutBits ( Bits, b8 [ 2 ] [ 18 ], b8 [ 3 ] [ 18 ] );
                ULBitStreamPutBits ( Bits, -Number + b8 [ 0 ] [ 18 ] , b8 [ 1 ] [ 18 ] );
                break;
            }
            if ( Number >= b8 [ 0 ] [ 19 ]) {
                ULBitStreamPutBits ( Bits, b8 [ 2 ] [ 19 ], b8 [ 3 ] [ 19 ] );
                ULBitStreamPutBits ( Bits, Number - b8 [ 0 ] [ 19 ], b8 [ 1 ] [ 19 ] );
                break;
            };
            if ( Number >= b8 [ 0 ] [ 17 ] && Number < b8 [ 0 ] [ 19 ] ) {
                ULBitStreamPutBits ( Bits, b8 [ 2 ] [ 17 ], b8 [ 3 ] [ 17 ] );
                ULBitStreamPutBits ( Bits, Number - b8 [ 0 ] [ 17 ], b8 [ 1 ] [ 17 ] );
                break;
            }
            if ( Number < b8 [ 0 ] [ s + 1 ] && Number >= b8 [ 0 ] [ s ] ){
                ULBitStreamPutBits ( Bits, b8 [ 2 ] [ s ], b8 [ 3 ] [ s ] );
                ULBitStreamPutBits ( Bits, Number - b8[ 0 ] [ s ], b8 [ 1 ] [ s ] );
                break;
            };
        }
    }
}

#define SaveLSW( x ) { TempConsec = x; BSWAPL ( TempConsec ); ULStreamWriteBuffer ( AStream, &TempConsec, 4 ); }
#define SaveLSWStr( AStream, x ) { TempConsec = x; BSWAPL ( TempConsec ); ULStreamWriteBuffer ( AStream, &TempConsec, 4 ); }

void JBIG2CreateOutStream ( PJBIG2Comp JBIG2, PDFStreamHandle AStream, PMImage Pict )
{
    ppInt32 DT;
    ppUns32 i, h;
    ppInt32 j;
    ppUns32 Tc;
    PDFStreamHandle  BmData, DSizeData;
    ULBitStream Bits;
    TIndexItem * RC = NULL;
    ppUns32 RCLen = 0;
    TTreeItem *HuffTree = NULL;
    ppUns32 TreeLen = 0;
    ppInt32 Code, IDS, CurS, SBStripsInd, StripT, SBDSOffset, SBStrips, Firsts, CurrInd, CurrRun, LenCurrRun;
    ppInt32 RCodes [ 35 ];
    ppInt32 *Runs = NULL;
    ppUns32 RunsLen = 0;
    ppBool First, EofStrips;
    ppInt32 NodeParent;
    ppUns32 TempConsec;
    ppInt32 *ALens = NULL;
    ppUns32 ALensLen = 0;
    PDFLibHandle Lib = JBIG2->Lib;
    if ( !Pict->Width || !Pict->Height || Pict->Width > 0xffff || Pict->Height > 0xffff )
        return;
    memset ( RCodes, 0, sizeof ( ppInt32 ) * 35 );
    PDFTRY ( Lib ){
        ULStreamWriteBuffer ( AStream, ( void * ) PHeader, 11 );
        SaveLSW ( JBIG2->PictRestrict.x );
        SaveLSW ( JBIG2->PictRestrict.y );
        for ( i = 0; i < 11; i++)
            ULStreamWriteBuffer ( AStream, ( void * ) PHeader, 1);
        if ( JBIG2->STLength < 1 )
            return;
        ULStreamWriteBuffer ( AStream, ( void * )SDHeader, 7 );
        DSizeData = ULStreamMemNew ( Lib, 0 );
        PDFTRY ( Lib ){
            ULBitStrmInit ( DSizeData, Bits );
            ULStreamWriteBuffer ( DSizeData, ( void * )BlHeader, 2 );
            SaveLSWStr ( DSizeData, JBIG2->STLength );
            SaveLSWStr ( DSizeData, JBIG2->STLength );
            for ( i = 1 ; i < 6 ; i++ ){
                if ( ( ppUns32 ) JBIG2->MaxH <= b4 [ 0 ] [ i ] && ( ppUns32 ) JBIG2->MaxH > b4 [ 0 ] [ i - 1 ] ){
                    ULBitStreamPutBits ( &Bits, b4 [ 1 ] [ i ], b4 [ 3 ] [ i ]);
                    if ( b4 [ 2 ] [ i ] )
                        ULBitStreamPutBits ( &Bits, JBIG2->MaxH - ( b4 [ 0 ] [ i - 1 ] + 1), b4 [ 2 ] [ i ]);
                    break;
                }
            }

            for ( i = 0; i < JBIG2->STLength; i++ ){
                if ( JBIG2->SymbTemplates[i].Dw < -256 ){
                    ULBitStreamPutBits ( &Bits, 0xFF, 8);
                    ULBitStreamPutBits ( &Bits, -257 - JBIG2->SymbTemplates[i].Dw, 32);
                    continue;
                }
                if ( JBIG2->SymbTemplates[i].Dw > -257 && JBIG2->SymbTemplates[i].Dw < 0 ){
                    ULBitStreamPutBits ( &Bits, 0xFE, 8);
                    ULBitStreamPutBits ( &Bits, 256 + JBIG2->SymbTemplates[i].Dw, 0x8);
                    continue;
                }
                for ( h = 2; h < 8; h++ ){
                    if ( JBIG2->SymbTemplates[i].Dw <= b3 [ 0 ] [h] && JBIG2->SymbTemplates[i].Dw > b3[0][h-1] ){
                        ULBitStreamPutBits ( &Bits, b3[1][h], b3[3][h]);
                        if ( b3[2][h])
                            ULBitStreamPutBits ( &Bits, JBIG2->SymbTemplates[i].Dw - ( b3[0][h-1] + 1 ), b3[2][h]);
                        break;
                    }
                }
            }
            ULBitStreamPutBits ( &Bits, 0x3E, 6);
            BmData = ULStreamMemNew ( Lib, 0 );
            PDFTRY ( Lib ){
                PDFImageCompressCCITT( Pict, BmData );
                _StreamPosition ( BmData ) = 0 ;
                for ( i = 1; i < 5; i++ ){
                    if ( (ppUns32) _StreamSize ( BmData ) < b1 [ 0 ][i] &&  ( ppUns32 )_StreamSize ( BmData ) >= b1 [0][i-1] ){
                        ULBitStreamPutBits ( &Bits, b1[1][i], b1[3][i]);
                        if ( b1[1][i] ){
                            ULBitStreamPutBits ( &Bits, _StreamSize ( BmData )-  b1[0][i-1], b1[2][i]);
                        } else {
                            ULBitStreamPutBits ( &Bits, _StreamSize ( BmData ), b1[2][i]);
                        }
                    }
                }
                ULBitStreamFlushWithCheck ( Bits );
                ULStreamCopyToStream( BmData, DSizeData );
                ULBitStreamPutBits ( &Bits, 0, 5);
                for ( i = 1; i < 5; i++){
                    if ( JBIG2->STLength < b1 [0][i] && JBIG2->STLength >= b1[0][i-1]){
                        ULBitStreamPutBits ( &Bits, b1[1][i], b1[3][i]);
                        if ( b1[1][i] ){
                            ULBitStreamPutBits ( &Bits, JBIG2->STLength -   b1[0][i-1], b1[2][i]);
                        } else {
                            ULBitStreamPutBits ( &Bits, JBIG2->STLength, b1[2][i]);
                        }
                    }
                }
                ULBitStreamFlushWithCheck ( Bits );
                SaveLSW ( _StreamSize ( DSizeData ) );
            } PDFFINALLY ( Lib ){
                ULStreamClose ( BmData );
            } PDFFINALLYEND ( Lib );
            _StreamPosition ( DSizeData ) = 0;
            ULStreamCopyToStream( DSizeData, AStream );
			ULStreamClear ( DSizeData, 0 );
            ULStreamWriteBuffer ( AStream, ( void * ) TRHeader, 8 );
            SaveLSWStr ( DSizeData, JBIG2->PictRestrict.x );
            SaveLSWStr ( DSizeData, JBIG2->PictRestrict.y );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRHeader, 2 );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRHeader, 2 );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRHeader, 2 );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRHeader, 2 );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRHeader, 1 );
            ULStreamWriteBuffer ( DSizeData, ( void * ) TRFlags, 4 );
            SaveLSWStr ( DSizeData, JBIG2->SymbolsCount );
            JBIG2InitRC ( JBIG2, &RC, &RCLen );
            JBIG2Sort ( false, HuffTree, TreeLen, RC, RCLen );
            JBIG2BuildTree ( Lib, RC, RCLen, &HuffTree, &TreeLen );
            for ( i = 0; i < JBIG2->UseTimeCount; i++ ){
                NodeParent = HuffTree[i].Parent;
                JBIG2->SymbTemplates[i].Code = HuffTree[i].Branch ? 1 : 0;
                JBIG2->SymbTemplates[i].CodeLen = 1;
                if ( !NodeParent ){
                    JBIG2->SymbTemplates[i].Code =  1;
                    JBIG2->SymbTemplates[i].CodeLen = 1;
                } else {
                    while ( HuffTree[ NodeParent - 1 ].Parent > 0 ){
                        if ( HuffTree[NodeParent-1].Branch )
                            JBIG2->SymbTemplates[i].Code = JBIG2->SymbTemplates[i].Code | ( 1 << JBIG2->SymbTemplates[i].CodeLen );
                        JBIG2->SymbTemplates[i].CodeLen++;
                        NodeParent = HuffTree[ NodeParent - 1].Parent;
                    }
                }
            }
            CurrRun = 0;
            LenCurrRun = 0;
            ALens = (ppInt32 * )mmalloc ( Lib, JBIG2->UseTimeCount * sizeof ( ppInt32 ) );
            ALensLen = JBIG2->UseTimeCount;
            for ( i = 0; i <= JBIG2->UseTimeCount; i++ ){
                if ( i != JBIG2->UseTimeCount ){
                    CurrInd = JBIG2->UseTime[i].Value == 0 ? 0: JBIG2GetWayLength( HuffTree, TreeLen,JBIG2->UseTime[i].Index ); 
                    ALens[i] = CurrInd;
                    JBIG2AddRun( Lib, false, RCodes, &Runs, &RunsLen, &CurrRun, &LenCurrRun, CurrInd );
                } else {
                    JBIG2AddRun( Lib, true, RCodes, &Runs, &RunsLen, &CurrRun, &LenCurrRun, CurrInd );
                }
            }
            JBIG2->UseTime = (TIndexItem* )mrealloc ( Lib, JBIG2->UseTime, 35 * sizeof ( TIndexItem ) );
            JBIG2->UseTimeCount = 35;
            for ( i = 0; i <35; i++ ){
                JBIG2->UseTime[i].Value = RCodes[i];
            }
            JBIG2InitRC( JBIG2, &RC, &RCLen );
            mfree ( Lib, HuffTree );
            HuffTree = NULL;
            TreeLen = 0;
            JBIG2Sort( false, HuffTree, 0, RC, RCLen );
            CurrInd = 1;
            JBIG2BuildTree( Lib, RC, RCLen, &HuffTree, &TreeLen );
            JBIG2->PrefLen = (ppInt32 * )mrealloc ( Lib, JBIG2->PrefLen, sizeof ( ppInt32 ) * 35 );
            JBIG2->PrefLenCount = 35; 
            for ( i = 0; i < JBIG2->UseTimeCount; i++)
                JBIG2->PrefLen[i]= JBIG2->UseTime[i].Value == 0 ? 0: JBIG2GetWayLength( HuffTree, TreeLen,JBIG2->UseTime[i].Index ); 
            JBIG2AssignPrefCodes( JBIG2, 35 );
            for ( i = 0; i <35; i++ ){
                ULBitStreamPutBits ( &Bits, JBIG2->PrefLen[i], 4);
            }
            for ( i = 0; i < RunsLen; i++ ){
                ULBitStreamPutBits ( &Bits, JBIG2->Codes[Runs [ i ]], JBIG2->PrefLen[Runs [ i ]]);
                switch ( Runs [ i ] ){
                    case 32:
                        ULBitStreamPutBits ( &Bits, Runs[ i + 1 ], 2);
                        i++;
                        break;
                    case 33:
                        ULBitStreamPutBits ( &Bits, Runs[ i + 1 ], 3);
                        i++;
                        break;
                    case 34:
                        ULBitStreamPutBits ( &Bits, Runs[ i + 1 ], 7 );
                        i++;
                        break;
                    default:;
                }
            }
            ULBitStreamFlushWithCheck ( Bits );
            JBIG2->PrefLen = (ppInt32 * )mrealloc ( Lib, JBIG2->PrefLen, sizeof ( ppInt32 ) * ALensLen );
            JBIG2->PrefLenCount = ALensLen;
            memcpy( JBIG2->PrefLen, ALens, sizeof ( ppInt32 ) * ALensLen );
            JBIG2AssignPrefCodes( JBIG2, ALensLen );
            for ( i = 0; i < ALensLen; i++){
                JBIG2->SymbTemplates[i].Code = JBIG2->Codes[i];
                JBIG2->SymbTemplates[i].CodeLen = (ppUns8) JBIG2->PrefLen[i];
            }
            SBStrips = 4;
            EofStrips = false;
            SBDSOffset = 3;
            ULBitStreamPutBits ( &Bits, 0, 1);
            StripT = -4;
            Firsts = 0;
            i = 0;
            while ( !EofStrips ){
                Tc = JBIG2->Symbols[i].y;
                DT = Tc - StripT;
                Code = ( ppInt32 ) ( DT / SBStrips );
                StripT += SBStrips * Code;
                for ( j = 12; j >=0; j--){
                    if ( Code < b12 [ 0 ] [ j + 1 ] && Code >=b12 [ 0 ] [ j ] ){
                        ULBitStreamPutBits ( &Bits, b12 [ 2 ] [ j ], b12 [ 3 ] [ j ] );
                        ULBitStreamPutBits ( &Bits, Code - b12 [ 0 ] [ j ], b12 [ 1 ] [ j ] );
                        break;
                    }
                }
                SBStripsInd = ( ppInt32 ) ( JBIG2->Symbols [ i].y / SBStrips );
                First = true;
                IDS = JBIG2->Symbols [ i].x - Firsts;
                Firsts += IDS;
                CurS = Firsts;
                while ( ( ppInt32 )( JBIG2->Symbols [ i].y ) <= (SBStripsInd + 1 ) * SBStrips - 1 ) {
                    if ( !First ){
                        CurS -= SBDSOffset;
                        IDS  = JBIG2->Symbols [ i].x - CurS - SBDSOffset;
                        CurS = JBIG2->Symbols [ i].x;
                    }
                    JBIG2CodeAndAdd( &Bits, IDS, First );
                    First = false;
                    ULBitStreamPutBits ( &Bits, JBIG2->Symbols [ i].y - StripT, ( ppUns32 ) (log2 ( ( double ) SBStrips ) ) );
                    CurS +=  SBDSOffset + JBIG2->SymbTemplates[JBIG2->Symbols[i].Pict].Pict->Width - 1;
                    ULBitStreamPutBits ( &Bits, JBIG2->SymbTemplates[JBIG2->Symbols[i].Pict].Code,
                        JBIG2->SymbTemplates[JBIG2->Symbols[i].Pict].CodeLen );
                    i++;
                    if ( i == JBIG2->SymbolsCount){
                        EofStrips = true;
                        break;
                    }
                }
                ULBitStreamPutBits ( &Bits, 1, 2 );
            }
            ULBitStreamFlushWithCheck ( Bits );
            _StreamPosition ( DSizeData ) = 0;
            SaveLSW ( _StreamSize ( DSizeData ) );
            ULStreamCopyToStream( DSizeData, AStream );       
        } PDFFINALLY ( Lib ){
            ULStreamClose ( DSizeData );
        } PDFFINALLYEND ( Lib );

    } PDFFINALLY ( Lib ){
        if ( ALens )
            mfree ( Lib, ALens );
        if ( Runs )
            mfree ( Lib, Runs );
        if ( RC )
            mfree ( Lib, RC );
        if ( HuffTree )
            mfree ( Lib, HuffTree );
    } PDFFINALLYEND ( Lib );
}


void JBIG2Execute ( PJBIG2Comp JBIG2, PDFStreamHandle AStream, PMImage Image )
{
    BLPoint BlackPix;
    TBorder Border;
    PMImage Pict;
    ppUns32 ComplexWidth, ComplexHeight;
    ppUns32 i,x;
    PDFLibHandle Lib = JBIG2->Lib;
    Pict = ( PMImage )PDFImageCreate( Lib, Image->Width, Image->Height, it1bit, cgGray );
    PDFTRY ( Lib ){
        JBIG2->PrevMatrixI = -2;
        JBIG2->Lossy = 0;
        memcpy ( Pict->ImageBuffer, Image->ImageBuffer, Image->LineSize * Image->Height );
        JBIG2->PictRestrict.x = Image->Width;
        JBIG2->PictRestrict.y = Image->Height;
        BlackPix = JBIG2GetFirstBlackPoint( JBIG2, 0, Pict );
        JBIG2->STLength = 0;
        i = 0;
        while ( BlackPix.ThereIs ){
            Border = JBIG2GetBorder( JBIG2, BlackPix.Point.x, BlackPix.Point.y, Pict );
            JBIG2GetSymbol( JBIG2, Border, Pict );
            BlackPix = JBIG2GetFirstBlackPoint( JBIG2, BlackPix.Point.y, Pict );
            i++;
        }
        ComplexHeight = 0;
        ComplexWidth = 0;
        if ( JBIG2->STLength ){
            for ( i = 0; i < JBIG2->STLength; i++ ){
                ComplexWidth += JBIG2->SymbTemplates[i].Pict->Width;
                if ( JBIG2->SymbTemplates[i].Pict->Height > ComplexHeight )
                    ComplexHeight = JBIG2->SymbTemplates[i].Pict->Height;
            }
            PDFImageChange ( Pict, ComplexWidth, ComplexHeight, it1bit, cgGray );
			memset ( Pict->ImageBuffer, 0xff, Pict->LineSize * Pict->Height );
            for ( x = i = 0; i < JBIG2->STLength; i++ ){
                JBIG2RectCopyToRect( JBIG2->SymbTemplates[i].Pict, Pict, 0, 0, x, 0,
					JBIG2->SymbTemplates[i].Pict->Width, JBIG2->SymbTemplates[i].Pict->Height );
                x += JBIG2->SymbTemplates[i].Pict->Width;
            }
        }
        JBIG2CreateOutStream( JBIG2, AStream, Pict);
    } PDFFINALLY ( Lib ) {
        PDFImageFree( Pict );
    } PDFFINALLYEND ( Lib );
}

void PDFImageCompressJBIG2 ( PMImage Image, PDFStreamHandle  AStream)
{
    PJBIG2Comp JBIG2;
    PDFLibHandle Lib = _StreamLib ( AStream );
    JBIG2 = JBIG2InitCompression(  Lib );
    PDFTRY ( Lib ){
        JBIG2Execute( JBIG2, AStream, Image );
    } PDFFINALLY ( Lib ) {
        JBIG2DoneCompression ( JBIG2 );
    } PDFFINALLYEND ( Lib );
}

