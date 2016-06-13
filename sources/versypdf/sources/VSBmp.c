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
VSPng.c
*********************************************************************/

#include "VSImageI.h"
#include "VSDocAI.h"
#include "VSFilter.h"
#include "VSExcept.h"
#ifdef WINDOWS_PLATFORM
#include <WinGDI.h>
#else
#define BI_RGB          0
#define BI_RLE8         1
#define BI_RLE4         2
#define BI_BITFIELDS    3

#pragma pack ( 1 )

typedef struct tagBITMAPFILEHEADER {
    ppUns16 bfType;
    ppUns32 bfSize;
    ppUns16 bfReserved1;
    ppUns16 bfReserved2;
    ppUns32 bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    ppUns32 biSize;
    ppUns32 biWidth;
    ppUns32 biHeight;
    ppUns16 biPlanes;
    ppUns16 biBitCount;
    ppUns32 biCompression;
    ppUns32 biSizeImage;
    ppUns32 biXPelsPerMeter;
    ppUns32 biYPelsPerMeter;
    ppUns32 biClrUsed;
    ppUns32 biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBTRIPLE {
    ppUns8 rgbRed;
    ppUns8 rgbGreen;
    ppUns8 rgbBlue;
    ppUns8 rgbReserved;
} RGBTRIPLE;

typedef struct tagRGBQUAD {
    ppUns8 rgbRed;
    ppUns8 rgbGreen;
    ppUns8 rgbBlue;
    ppUns8 rgbReserved;
} RGBQUAD;

#pragma  pack ()
#endif

#define RLE_COMMAND     0
#define RLE_ENDOFLINE   0
#define RLE_ENDOFBITMAP 1
#define RLE_DELTA       2


#ifdef PDFBIGENDIAN
#define SWAPHEAD {\
    SSWAPL (Header.bfSize );\
    SSWAPS (Header.bfReserved1  );\
    SSWAPS (Header.bfReserved2 );\
    SSWAPL (Header.bfOffBits ) ;\
    SSWAPL (Info.biSize );\
    SSWAPL (Info.biWidth );\
    SSWAPL (Info.biHeight  );\
    SSWAPS (Info.biPlanes  );\
    SSWAPS (Info.biBitCount );\
    SSWAPL (Info.biCompression   );\
    SSWAPL (Info.biSizeImage );\
    SSWAPL (Info.biXPelsPerMeter  );\
    SSWAPL (Info.biYPelsPerMeter );\
    SSWAPL (Info.biClrUsed );\
    SSWAPL (Info.biClrImportant );\
}
#else
#define SWAPHEAD
#endif

#define I16_555_RED_MASK		0x7C00
#define I16_555_GREEN_MASK		0x03E0
#define I16_555_BLUE_MASK		0x001F
#define I16_555_RED_SHIFT		10
#define I16_555_GREEN_SHIFT	    5
#define I16_555_BLUE_SHIFT		0
#define I16_565_RED_MASK		0xF800
#define I16_565_GREEN_MASK		0x07E0
#define I16_565_BLUE_MASK		0x001F
#define I16_565_RED_SHIFT		11
#define I16_565_GREEN_SHIFT	    5
#define I16_565_BLUE_SHIFT		0

PMImage PDFBMPLoadFromStream ( PDFStreamHandle AStream )
{
    BITMAPFILEHEADER Header;
    BITMAPINFOHEADER Info;
    PDFLibHandle Lib = _StreamLib ( AStream );
    PMImage Tmp;
    ppInt32 Width, Height, BitCount,UsedColors, Compression;
    ppBool BottomUp;
    TPDFColorDevice Device;
    ppComponentDepth Depth;
    RGBQUAD *Pal;
    ppBool ZeroIsBlack;
    ppUns8 *P, *SL, off,*Img;
    ppBool Hi;
    ppUns16 Color;
    ppUns32 bitfields[3], RMask, GMask, BMask, x;
    ppBool B565;
    ppUns8 status_byte = 0;
    ppUns8 delta_x, delta_y, CR;
    ppInt32 scanline = 0;
    ppUns32 bits = 0;
    ppBool eof;

    ppUns32 i,j, SLSize, a, SOFF;

//    ppUns8 DIBPalSizes[2] = { sizeof ( TRGBQuad ), sizeof ( TRGBTriple )};
    ULStreamReadBuffer( AStream, &Header , sizeof ( Header ) );
    SSWAPS ( Header.bfType );
    if ( Header.bfType != 0x4D42 ){
        _LRAISE ( Lib, ErrGeneralLevel, gleLoadBMPFileError );
    }
    ULStreamReadBuffer ( AStream, &Info , sizeof ( Info ) );
    SWAPHEAD;   
    Width = Info.biWidth;
    Height = Info.biHeight;
    BitCount = Info.biBitCount;
    UsedColors = Info.biClrUsed;
    Compression = Info.biCompression;
    SLSize = (( Width * BitCount + 31 ) & ~31) >>3 ;
    if ( Height < 0 ){
        Height = - Height;
        BottomUp = false;
    } else {
        BottomUp = true;
    }
    if ( BitCount == 1 ){
        Device = cgGray;
        Depth = it1bit;
    } else {
        Device = cgRGB;
        Depth = it8bit;
    }
    Tmp = ( PMImage )PDFImageCreate( Lib, Width, Height, Depth, Device );
    PDFTRY ( Lib ){
        switch ( BitCount) {
            case 1 :
            case 4 :
            case 8 :{
                i = CalculateUsedPaletteEntries ( BitCount );
                if ( UsedColors <= 0 || UsedColors > (ppInt32)i )
                    UsedColors = i;
                ULStreamSetPosition( AStream, sizeof ( Header ) + Info.biSize );
                Pal = (RGBQUAD *)mmalloc ( Lib, UsedColors * sizeof(RGBQUAD) );
                PDFTRY ( Lib ){
                    ULStreamReadBuffer( AStream,Pal, UsedColors * sizeof (RGBQUAD ) );
                    if (BitCount == 1){
                        ZeroIsBlack = ( Pal[0].rgbBlue == 0 );
                    }
                    ULStreamSetPosition( AStream, Header.bfOffBits );
                    switch ( Compression ){
                        case BI_RGB:{
                            SL = (ppUns8 *)mmalloc ( Lib, SLSize );
                            PDFTRY ( Lib ){
                                for ( i = 0; i < ( ppUns32 ) Height; i++ ){
                                    P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                                    ULStreamReadBuffer( AStream, SL, SLSize );
                                    if (BitCount == 1){
                                        memcpy( P, SL, Tmp->LineSize );
                                    } else if ( BitCount == 4 ) {
                                        Hi = true;
                                        for ( SOFF = 0, a= 0, j = 0; j < ( ppUns32 )Width; j++ ){
                                            off = Hi? SL[a]>>4: SL[a]& 0x0F;
                                            P[SOFF ] = Pal[off].rgbRed;
                                            P[SOFF+1 ] = Pal[off].rgbGreen;
                                            P[SOFF+2 ] = Pal[off].rgbBlue;
                                            SOFF += 3;
                                            if ( !Hi )
                                                a++;
                                            Hi = !Hi;
                                        }
                                    } else {
                                        for ( SOFF = 0, j = 0; j < ( ppUns32 )Width; j++ ){
                                            P[SOFF ] = Pal[SL[j]].rgbRed;
                                            P[SOFF+1 ] = Pal[SL[j]].rgbGreen;
                                            P[SOFF+2 ] = Pal[SL[j]].rgbBlue;
                                            SOFF += 3;
                                        }
                                    }
                                }                                      
                            } PDFFINALLY ( Lib ){
                                mfree ( Lib, SL);
                            } PDFFINALLYEND ( Lib );
                            if ( BitCount == 1 ){
                                if ( ! ZeroIsBlack ){
                                    P = (ppUns8 *)Tmp->ImageBuffer;
                                    for ( i = 0; i < Height * Tmp->LineSize; i++)
                                        P[i] = ~P[i];
                                }
                            }   

                            break;
                        }
                        case BI_RLE4:{
                             _LRAISE ( Lib, ErrGeneralLevel, gleUnsupportedBitmapCompressionError );
                             /*
                             Img = mmalloc ( Lib, SLSize * Height );
                             PDFTRY ( Lib ) {
                                 SL = Img;
                                 status_byte = 0;
                                 second_byte = 0;
                                 scanline = 0;
                                 bits = 0;
                                 align = 0;
                                 low_nibble = false;
                                 eof = false;
                                 for ( ;; ){
                                     status_byte = ( ppUns8)ULStreamReadChar( AStream );
                                     if ( status_byte == RLE_COMMAND) {
                                         status_byte = (ppUns8) ULStreamReadChar( AStream );
                                         switch( status_byte ) {
                                            case RLE_ENDOFLINE:
                                                scanline++;
                                                bits = 0;
                                                low_nibble = false;
                                         	    break;
                                            case RLE_ENDOFBITMAP:
                                                eof = true;
                                                break;
                                            case RLE_DELTA:
                                                delta_x = (ppUns8) ULStreamReadChar( AStream );
                                                delta_y = (ppUns8) ULStreamReadChar( AStream );
                                                scanline += delta_y;
                                                bits += delta_x /2;
                                                break;
                                            default:
                                                SOFF = min ( status_byte, Width - ( bits << 1 ) );
                                                SL = Img + scanline  * SLSize;
                                                if ( ! low_nibble ){
                                                    P = SL + bits;
                                                    ULStreamReadBuffer( AStream, P, SOFF >> 1);
                                                    bits += SOFF >>1; 
                                                } else {
                                                    for ( i = 0; i < SOFF; i++){
                                                        if ( low_nibble ){
                                                            SL[bits] &= 0xf0;
                                                            SL[bits] |= second_byte & 0x0f;
                                                            if ( i != SOFF - 1){
                                                                second_byte  = (ppUns8)ULStreamReadChar( AStream );
                                                            }
                                                            bits++;
                                                        } else {
                                                            SL[bits] &= 0x0f;
                                                            SL[bits] |= second_byte & 0xf0;
                                                        }
                                                        low_nibble = !low_nibble;
                                                    }
                                                }
                                                align = status_byte % 4;
                                                if ( align == 1 || align == 2)
                                                   status_byte = (ppUns8) ULStreamReadChar( AStream );
                                         }
                                     } else {
                                         SOFF = min ( status_byte, Width - ( bits << 2 ) );
                                         second_byte = (ppUns8)ULStreamReadChar( AStream );
                                         SL = Img + scanline  * SLSize;
                                         for ( i = 0; i < SOFF; i++){
                                             if ( low_nibble ){
                                                 SL[bits] &= 0xf0;
                                                 SL[bits] |= second_byte & 0x0f;
                                                 bits++;
                                             } else {
                                                 SL[bits] &= 0x0f;
                                                 SL[bits] |= second_byte & 0xf0;
                                             }
                                             low_nibble = !low_nibble;
                                         }
                                     }
                                     if ( scanline >= Height )
                                         break;
                                     if ( eof )
                                         break;
                                 }
                                 for ( i = 0; i < ( ppUns32 ) Height; i++ ){
                                    Hi = true;
                                    P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                                    for ( SOFF = 0, a= 0, j = 0; j < ( ppUns32 )Width; j++ ){
                                        off = Hi? SL[a]>>4: SL[a]& 0x0F;
                                        P[SOFF ] = Pal[off].rgbRed;
                                        P[SOFF+1 ] = Pal[off].rgbGreen;
                                        P[SOFF+2 ] = Pal[off].rgbBlue;
                                        SOFF += 3;
                                        if ( !Hi )
                                            a++;
                                        Hi = !Hi;
                                    }
                                 }
                             } PDFFINALLY ( Lib ){
                                 mfree ( Lib, Img );
                             } PDFFINALLYEND ( Lib );
                            break;
                            */
                        }
                        case BI_RLE8:{
                             Img = (ppUns8 *)mmalloc ( Lib, SLSize * Height );
                             PDFTRY ( Lib ) {
                                 SL = Img;
                                 scanline = 0;
                                 x = 0;
                                 eof = false;
                                 bits = 0;
                                 for ( ;; ){
                                     bits ++;
                                     status_byte = ( ppUns8)ULStreamReadChar( AStream );
                                     if ( status_byte == RLE_COMMAND) {
                                         status_byte = (ppUns8) ULStreamReadChar( AStream );
                                         switch( status_byte ) {
                                            case RLE_ENDOFLINE:
                                                scanline++;
                                                SL = Img + scanline * SLSize;
                                                x = 0;
                                         	    break;
                                            case RLE_ENDOFBITMAP:
                                                eof = true;
                                                break;
                                            case RLE_DELTA:
                                                delta_x = (ppUns8) ULStreamReadChar( AStream );
                                                delta_y = (ppUns8) ULStreamReadChar( AStream );
                                                scanline += delta_y;
                                                x += delta_x;
                                                SL = Img + scanline*SLSize + x;
                                                break;
                                            default:
                                                SOFF = min ( status_byte, Width - x );
                                                ULStreamReadBuffer( AStream, SL,  SOFF );
                                                SL+=SOFF;
                                                x+=SOFF;
                                                if ( ( status_byte & 1 ) == 1)
                                                    status_byte = (ppUns8) ULStreamReadChar( AStream );
                                         }
                                     } else {
                                         SOFF = min ( status_byte, Width - x );
                                         CR = (ppUns8) ULStreamReadChar( AStream );
                                         memset ( SL, CR, SOFF );
                                         SL += SOFF;
                                         x += SOFF;                                       
                                     }
                                     if ( scanline >= Height )
                                         break;
                                     if ( eof )
                                         break;
                                 }
                                 for ( i = 0; i < ( ppUns32 ) Height; i++ ){

                                     P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                                     SL = Img + SLSize * i;                                   
                                     for ( SOFF = 0, j = 0; j < ( ppUns32 )Width; j++ ){
                                        P[SOFF ] = Pal[SL[j]].rgbRed;
                                        P[SOFF+1 ] = Pal[SL[j]].rgbGreen;
                                        P[SOFF+2 ] = Pal[SL[j]].rgbBlue;
                                        SOFF += 3;
                                    }
                                 }
                             } PDFFINALLY ( Lib ){
                                 mfree ( Lib, Img );
                             } PDFFINALLYEND ( Lib );
                            break;
                        }
                        default:
                            _LRAISE ( Lib, ErrGeneralLevel, gleUnsupportedBitmapCompressionError );

                    }

                } PDFFINALLY ( Lib ){
                    mfree ( Lib, Pal );
                } PDFFINALLYEND ( Lib );
                

                break;
            };
            case 16:{
                if ( Info.biCompression == BI_BITFIELDS){
                    ULStreamSetPosition( AStream, sizeof ( Header ) + Info.biSize );
                    ULStreamReadBuffer( AStream, bitfields, sizeof ( ppUns32 ) * 3 );
                    RMask = bitfields[0];
                    GMask = bitfields[1];
                    BMask = bitfields[2];
                } else {
                    RMask = I16_555_RED_MASK;
                    GMask = I16_555_GREEN_MASK;
                    BMask = I16_555_BLUE_MASK;
                }
                B565 = ( RMask == I16_565_RED_MASK && GMask == I16_565_GREEN_MASK && BMask == I16_565_BLUE_MASK )? true : false;
                ULStreamSetPosition( AStream, Header.bfOffBits );
                SL = (ppUns8 *)mmalloc ( Lib, SLSize );
                PDFTRY ( Lib ){
                    for ( i = 0; i < ( ppUns32 ) Height; i++ ){
                        P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                        ULStreamReadBuffer( AStream, SL, SLSize );
                        for ( SOFF = 0, a = 0, j = 0; j < ( ppUns32 )Width; j++ ){
                            Color = SL[ a + 1 ] << 8 | SL [ a ]; 
                            if ( B565 ){
                                P[SOFF +2]	= (ppUns8)((((Color & I16_565_BLUE_MASK) >> I16_565_BLUE_SHIFT) * 0xFF) / 0x1F);
                                P[SOFF +1]	= (ppUns8)((((Color & I16_565_GREEN_MASK) >> I16_565_GREEN_SHIFT) * 0xFF) / 0x1F);
                                P[SOFF ]	= (ppUns8)((((Color & I16_565_RED_MASK) >> I16_565_RED_SHIFT) * 0xFF) / 0x1F);
                            } else {
                                P[SOFF +2]	= (ppUns8)((((Color & I16_555_BLUE_MASK) >> I16_555_BLUE_SHIFT) * 0xFF) / 0x1F);
                                P[SOFF +1]	= (ppUns8)((((Color & I16_555_GREEN_MASK) >> I16_555_GREEN_SHIFT) * 0xFF) / 0x1F);
                                P[SOFF ]	= (ppUns8)((((Color & I16_555_RED_MASK) >> I16_555_RED_SHIFT) * 0xFF) / 0x1F);
                            }
                            SOFF += 3;
                            a +=2;
                        }
                    }                                      
                } PDFFINALLY ( Lib ){
                    mfree ( Lib, SL);
                } PDFFINALLYEND ( Lib );
                break;
            }
            case 24: {
                ULStreamSetPosition( AStream, Header.bfOffBits  );
                SL = (ppUns8 *)mmalloc ( Lib, SLSize );
                PDFTRY ( Lib ){
                    for ( i = 0; i < ( ppUns32 ) Height; i++ ){
                        P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                        ULStreamReadBuffer( AStream, SL, SLSize );
                        for ( SOFF = 0, j = 0; j < ( ppUns32 )Width; j++ ){
                            P[SOFF ] = SL[SOFF +2 ];
                            P[SOFF+1 ] = SL[SOFF + 1];
                            P[SOFF+2 ] = SL[SOFF];
                            SOFF += 3;
                        }
                    }                                      
                } PDFFINALLY ( Lib ){
                    mfree ( Lib, SL);
                } PDFFINALLYEND ( Lib );
                break;
            }
            case 32:{
                ULStreamSetPosition( AStream, Header.bfOffBits );
                SL = (ppUns8 *)mmalloc ( Lib, SLSize );
                PDFTRY ( Lib ){
                    for ( i = 0; i < ( ppUns32 ) Height; i++ ){
                        P = ScanLine ( Tmp, BottomUp ? Height - 1 - i: i );
                        ULStreamReadBuffer( AStream, SL, SLSize );
                        for ( SOFF = 0, a = 0,  j = 0; j < ( ppUns32 )Width; j++ ){
                            P[SOFF ] = SL[a +2 ];
                            P[SOFF+1 ] = SL[a + 1];
                            P[SOFF+2 ] = SL[a];
                            SOFF += 3;
                            a += 4; 
                        }
                    }                                      
                } PDFFINALLY ( Lib ){
                    mfree ( Lib, SL);
                } PDFFINALLYEND ( Lib );
                break;
            }


            default:;

        }
    } PDFEXCEPT ( Lib ) {
        PDFImageFree( Tmp );
        PDFRERAISE ( Lib );
    } PDFTRYEND ( Lib );
    return Tmp;
}



