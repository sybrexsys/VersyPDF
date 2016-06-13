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
VSTiff.c
*********************************************************************/
#include "VSImageI.h"


#ifndef NOT_USE_TIFF

#include "VSDocAI.h"
#include "VSFilter.h"
#include "../VSExcept.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffiop.h"

#define _DOC CD ( Doc )
#define _LIB _DOC->Lib

void
TIFFError(const char* module, const char* fmt, ...)
{
    //
}

void TIFFWarning( const char* module, const char* fmt, ...)
{
    //
}

tdata_t PDFTIFFMalloc ( TIFF * tif, tsize_t size ){
    tdata_t tmp;
    PPDFLib Lib = ( PPDFLib )( tif->lib );
    Lib->MemRaiseOnNULL = false;
    tmp = mmalloc ( Lib, size );
    Lib->MemRaiseOnNULL = true;
    return tmp;
}

tdata_t PDFTIFFRealloc ( TIFF * tif, tdata_t Buffer, tsize_t size ){
    tdata_t tmp;
    PPDFLib Lib = ( PPDFLib )( tif->lib );
    Lib->MemRaiseOnNULL = false;
    tmp = mrealloc ( Lib, Buffer, size );
    Lib->MemRaiseOnNULL = true;
    return tmp;
}

void PDFTIFFFree ( TIFF * tif, tdata_t Buffer){
    PPDFLib Lib = ( PPDFLib )( tif->lib );
    mfree ( Lib, Buffer);
}


tsize_t PDFTIFFReadStream(thandle_t Stream, tdata_t Buffer, tsize_t Size){
    return ULStreamReadBuffer ( (PDFStreamHandle)Stream, Buffer, Size );
}
tsize_t PDFTIFFWriteStream(thandle_t Stream , tdata_t Buffer , tsize_t Size ){
    return ULStreamWriteBuffer ( (PDFStreamHandle)Stream, Buffer, Size );
}

toff_t PDFTIFFSeekStream(thandle_t Stream , toff_t off , int whence){
    ppInt32 Pos;

    /* we use this as a special code, so avoid accepting it */
    if( off == 0xFFFFFFFF )
        return 0xFFFFFFFF;

    switch ( whence ){
        case SEEK_SET:
            Pos = ULStreamSetPosition ( (PDFStreamHandle)Stream, off );
            break;
        case SEEK_CUR:
            Pos = ULStreamSetPosition ( (PDFStreamHandle)Stream, ULStreamGetPosition( ( PDFStreamHandle )Stream ) + off );
            break;
        case SEEK_END:
            Pos = ULStreamSetPosition ( (PDFStreamHandle)Stream, ULStreamGetSize( ( PDFStreamHandle )Stream ) + off );
            break;
        default:
            Pos = ULStreamSetPosition ( (PDFStreamHandle)Stream, off );
            break;
    }
    return (toff_t) Pos;
}
toff_t PDFTIFFSizeStream(thandle_t Stream){
    return ULStreamGetSize( (PDFStreamHandle)Stream );
}


 
ppUns32 PDFImageGetTIFFCountFromStream( PDFLibHandle Lib, PDFStreamHandle AStream)
{
    TIFF * tif;  
    ppInt32 DirCount;
    tif = TIFFClientOpen ( Lib, "PDFStream", "rb", ( thandle_t ) AStream , PDFTIFFReadStream,
        PDFTIFFWriteStream, PDFTIFFSeekStream, PDFTIFFSizeStream, NULL, 
        PDFTIFFMalloc, PDFTIFFRealloc, PDFTIFFFree );
    if ( !tif) {
        PDFRAISE ( Lib,  PDFBuildErrCode( ErrGeneralLevel, gleLoadTIFFFileError ) );
    }
    PDFTRY ( Lib ){
        DirCount = TIFFNumberOfDirectories( tif );
    } PDFFINALLY ( Lib ){
         TIFFClose ( tif ) ;
    } PDFFINALLYEND ( Lib);
    return DirCount;
}


PMImage PDFImageLoadTIFFFromStream (  PDFStreamHandle AStream , ppUns32 ImageIndex ){
    PMImage Tmp;
    ppUns32 row, SLSize, i;
    ppUns16 PixDepth, Device, biw;
    ppUns32 ImageLength, ImageWidth;
    TPDFColorDevice CurrentCG ;
    ppComponentDepth Depth;
    tdata_t buf;
    ppUns32 DirCount = 0;
    TIFF * tif;  
    ppUns8 *SL;
	ppBool bi1 = false; 

    PDFLibHandle Lib = _StreamLib ( AStream );

    tif = TIFFClientOpen ( Lib, "PDFStream", "rb", ( thandle_t ) AStream , PDFTIFFReadStream,
        PDFTIFFWriteStream, PDFTIFFSeekStream, PDFTIFFSizeStream, NULL, 
        PDFTIFFMalloc, PDFTIFFRealloc, PDFTIFFFree );
    if ( !tif) {
        PDFRAISE ( Lib,  PDFBuildErrCode( ErrGeneralLevel, gleLoadTIFFFileError ) );
    }
    DirCount = TIFFNumberOfDirectories( tif );
    if ( DirCount <= ImageIndex ){
        TIFFClose ( tif ) ;
        PDFRAISE ( Lib,  PDFBuildErrCode( ErrGeneralLevel, gleOutOfBoundError ) );
    }
    PDFTRY ( Lib ){
        TIFFSetDirectory ( tif, (tdir_t)ImageIndex );
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &ImageWidth);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &ImageLength );
		PixDepth = 0xFF;
		Device = 0xFF;
        TIFFGetField ( tif, TIFFTAG_BITSPERSAMPLE, &PixDepth );
        TIFFGetField ( tif, TIFFTAG_SAMPLESPERPIXEL, &Device );
		if ( PixDepth == 0xFF && Device == 0xFF ){
			PixDepth = 1;
			Device = 1;
		}
		biw = 0xff;
		if ( PixDepth == 1 && Device == 1 ) {
			TIFFGetField ( tif, 262, &biw );
			if ( biw == 0 || biw ==0xff )
				bi1 = true;
		}
        switch ( Device ) {
            case 1:
                CurrentCG = cgGray;
                break;
            case 3:
                CurrentCG = cgRGB;
                break;
            case 4:
                CurrentCG = cgCMYK;
                break;
            default:
                PDFRAISE ( Lib,  PDFBuildErrCode( ErrGeneralLevel, gleLoadTIFFFileError ) );
        }
        Depth = ( ppComponentDepth ) PixDepth;
        Tmp = ( PMImage )PDFImageCreate ( Lib, ImageWidth , ImageLength, Depth, CurrentCG );
        PDFTRY ( Lib ){
            SLSize = TIFFScanlineSize(tif);
            buf = mmalloc ( Lib, SLSize );
            for (row = 0; row < ImageLength; row++){
                TIFFReadScanline(tif, buf, row, 0);
                SL = ( ppUns8 *)ScanLine ( Tmp, row );
                memcpy ( SL, buf, Tmp->LineSize );
				if ( bi1 ) 
					for ( i = 0; i < Tmp->LineSize; i++ )
						SL[i] = ~SL[i];				
            }
            mfree( Lib, buf);
        } PDFEXCEPT ( Lib ) {
            PDFImageFree ( Tmp );
            PDFRERAISE ( Lib );
        } PDFTRYEND ( Lib );

    } PDFFINALLY ( Lib ){
        TIFFClose ( tif ) ;
    } PDFFINALLYEND ( Lib );
    return Tmp;
}

#endif

