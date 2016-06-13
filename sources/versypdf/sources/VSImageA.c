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
VSImageA.c
*********************************************************************/

#include "../VSImageA.h"
#include "VSImageI.h"
#include "VSDocAI.h"
#include "VSMisc.h"
#include "VSFilter.h"



#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD ( Doc )
#define _LIB _DOC->Lib


#ifdef WINDOWS_PLATFORM

#include "Windows.h"



ppInt32 PDFDocAppendImageFromBitmapHandle ( PDFDocHandle Doc, HBITMAP Bitmap)
{
    PMImage Img;
    ppInt32 R;
    Img = ( PMImage )PDFImageLoadFromHandle ( _LIB, Bitmap );
    if ( Img->Depth == it1bit ){
		if ( IsEdition(_LIB, STD)){
			if ( _DOC->BWImagesAsJBIG2 ){
				R =  PDFImageAppendToDoc( Img, Doc, itcJBIG2 );
			} else 
			{
				R =  PDFImageAppendToDoc( Img, Doc, itcCCITT42D );
			}
		} else 
			R =  PDFImageAppendToDoc( Img, Doc, itcFlate );
    } else {
        if ( _DOC->EmfImagesAsJpeg ){
            R =  PDFImageAppendToDoc( Img, Doc, itcJPEG );
        } else 
        {
            R =  PDFImageAppendToDoc( Img, Doc, itcFlate );
        }
    }
    PDFImageFree ( Img );
    return R;
}

#endif


PDFImageHandle PDFImageCreate ( PDFLibHandle Lib, ppUns32 Width, ppUns32 Height, ppComponentDepth Depth, TPDFColorDevice Device )
{
    PMImage Tmp;
    Tmp = ( PMImage ) mmalloc ( Lib, sizeof ( TMImage ) );
    Tmp->Lib = Lib;
    Tmp->Width = Width;
    Tmp->Height = Height;
    Tmp->Depth = Depth;
    Tmp->LineSize = GetScanlineSize ( Width, Depth, Device );
    Tmp->Device = Device;
    Tmp->ImageBuffer = ( char * ) mmalloc ( Lib, Tmp->LineSize * Height );
    return ( PDFImageHandle ) Tmp;
}

PDFImageHandle PDFImageLoadFromStream ( PDFLibHandle Lib, PDFStreamHandle AStream, ppUns32 Index )
{
    PDFImageHandle Tmp = NULL;
    ppUns8 b;
    b = ( ppUns8 ) ULStreamLookChar( AStream );
    switch ( b ){
#ifndef NOT_USE_TIFF
        case 'M':
        case 'I':
			if (IsEdition(Lib, STD ))
				Tmp = PDFImageLoadTIFFFromStream( AStream, Index );
			else 
				_LRAISE ( Lib, ErrGeneralLevel, gleUnknowImageFormatError );

            break;
#endif
#ifndef NOT_USE_PNG
        case 137:
			if (IsEdition(Lib, STD ))
				Tmp = PDFImageLoadPNGFromStream( AStream );
			else
				_LRAISE ( Lib, ErrGeneralLevel, gleUnknowImageFormatError );
            break;
#endif

        case 0xFF:
            Tmp = PDFImageLoadJPEGFromStream( AStream );
            break;
        case 'B':
            Tmp = PDFBMPLoadFromStream ( AStream );
            break;
        default:
            _LRAISE ( Lib, ErrGeneralLevel, gleUnknowImageFormatError );
            ;
    }  
    return Tmp;
}

PDFImageHandle PDFImageLoadFromFile ( PDFLibHandle Lib, char * FileName, ppUns32 Index )
{
    PDFStreamHandle ms;
    PDFImageHandle Tmp;
    ms = ULStreamFileNew(Lib,FileName,ppFileReadMode);
    PDFTRY(Lib){
        Tmp = PDFImageLoadFromStream ( Lib, ms, Index );
    }PDFFINALLY ( Lib ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( Lib );
    return Tmp;
}

PDFImageHandle PDFImageLoadFromBuffer ( PDFLibHandle Lib, void * Buffer, ppUns32 BufferSize, ppUns32 Index )
{
    PDFStreamHandle ms;
    PDFImageHandle Tmp;
    ms = ULStreamMemReadOnlyNew ( Lib, Buffer, BufferSize );
    PDFTRY(Lib){
        Tmp = PDFImageLoadFromStream ( Lib, ms, Index );
    }PDFFINALLY ( Lib ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( Lib );
    return Tmp;
}

#ifdef WINDOWS_PLATFORM
LIB_API PDFImageHandle PDFImageLoadFromHandle ( PDFLibHandle Lib, HBITMAP Handle )
{
    HDC ScreenDC, DC ;
    DIBSECTION DIB;
    BITMAPINFO *BI;
    ppComponentDepth PF;
    TPDFColorDevice CG;
    ppUns8 BC;
    HGDIOBJ BM;
    ppUns8 *Buffer = NULL; 
    PMImage Tmp;
    ppInt32 Row, SLSize;  
    ppUns8 * SCLine, * SCL;
    ppUns32 Width, Height, i, j, k;
    ppBool BottomUp, DCIsBlack, BufIsBlack;
    void *D;
    DIB.dsBmih.biSize = 0;
    if ( GetObject ( Handle, sizeof ( DIB ), &DIB ) < sizeof ( DIB.dsBm ) ) 
        _LRAISE ( Lib, ErrGeneralLevel, gleInvalidBitmapError );

    if ( DIB.dsBm.bmBitsPixel == 1 ){
        PF =  it1bit;
        CG = cgGray;
        BC = 1;
    } else {
        PF = it8bit;
        CG = cgRGB;
        BC = 24;
    }
    Width = DIB.dsBm.bmWidth;
    if ( DIB.dsBm.bmHeight < 0){
        BottomUp = false;
        Height = - DIB.dsBm.bmHeight;
    } else {
        BottomUp = true;
        Height = DIB.dsBm.bmHeight;
    }

    ScreenDC = GetDC ( 0 );
    DC = CreateCompatibleDC ( ScreenDC );
    ReleaseDC( 0, ScreenDC );
    PDFTRY ( Lib ){
        BI = ( BITMAPINFO * ) mmalloc ( Lib, sizeof ( BITMAPINFO ) + 256 * sizeof ( RGBQUAD ) );
        PDFTRY ( Lib ){
            BI->bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
            BI->bmiHeader.biWidth = DIB.dsBm.bmWidth;
            BI->bmiHeader.biHeight = DIB.dsBm.bmHeight;
            BI->bmiHeader.biPlanes = 1;
            BI->bmiHeader.biBitCount = BC;
            BI->bmiHeader.biCompression = BI_RGB;
            BM = CreateDIBSection ( DC, BI, DIB_RGB_COLORS, &D, 0, 0);
            Buffer = (ppUns8 * )D;
            if ( ! ( BM || Buffer ) )
                _LRAISE ( Lib, ErrGeneralLevel, gleWindowsGDIError );
            PDFTRY ( Lib ){
                GetDIBits ( DC, Handle, 0, Height, Buffer, BI, DIB_RGB_COLORS );
                SLSize = (( Width * BC + 31 ) & ~31) >>3;
                Tmp = ( PMImage )PDFImageCreate ( Lib, Width, Height, PF, CG );
                if ( BC == 1 ){
                    for ( i = 0; i < Height; i++ ){
                        Row = BottomUp? Height - i - 1: i;
                        SCLine = Buffer + Row * SLSize;
                        memcpy( ScanLine ( Tmp, i), SCLine, Tmp->LineSize );
                    }
                    SelectObject( DC, Handle );
                    DCIsBlack = GetPixel ( DC, 0, 0) & 0x0FFF?false:true;
                    BufIsBlack = Buffer[0] & 0x80 ? false: true; 
                    if ( DCIsBlack != BufIsBlack ){
                        for ( i=0; i < Height; i++ ){
                            SCLine = ScanLine ( Tmp, i);
                            for ( j = 0; j < Tmp->LineSize; j++)
                                SCLine[j] = ~SCLine[j];
                        }
                    }                                    
                } else {
                    for ( k = 0 ; k <  Height ; k++){
                        Row = BottomUp? Height - k - 1: k;
                        SCLine = Buffer + Row * SLSize;
                        SCL = ScanLine ( Tmp, k );
                        for ( i = 0, j = 0; i< Width; i++, j+=3){
                            SCL[j] = SCLine [ j+2];
                            SCL[ j+1] = SCLine [ j+1];
                            SCL [ j +2 ]= SCLine [ j ];
                        }
                    }
                }    
            } PDFFINALLY ( Lib ){
                DeleteObject ( BM );
            } PDFFINALLYEND ( Lib );
        } PDFFINALLY ( Lib ){
            mfree ( Lib, BI );
        } PDFFINALLYEND ( Lib );
    } PDFFINALLY ( Lib ){
        DeleteDC ( DC );
    } PDFFINALLYEND ( Lib );
    return Tmp;
}
#endif 

void PDFImageFree ( PDFImageHandle Image )
{
    if ( TImage(Image)->ImageBuffer )
        mfree ( TImage( Image )->Lib, TImage(Image)->ImageBuffer );
	mfree( TImage( Image )->Lib, Image );
}

ppComponentDepth PDFImageGetDepth ( PDFImageHandle Image )
{
    return TImage ( Image )->Depth;
    
}

TPDFColorDevice PDFImageGetColorDevice ( PDFImageHandle Image )
{
    return TImage ( Image )->Device;
}

ppUns32 PDFImageGetWidth ( PDFImageHandle Image )
{
    return TImage ( Image )->Width;
}

ppUns32 PDFImageGetHeight ( PDFImageHandle Image )
{
    return TImage ( Image )->Height;
}

void * PDFImageGetScanLine ( PDFImageHandle Image, ppUns32 ScanLineIndex )
{
    if ( ScanLineIndex >= TImage(Image)->Height )
        _LRAISE ( TImage(Image)->Lib, ErrGeneralLevel, gleOutOfBoundError );
    return ScanLine( TImage ( Image ), ScanLineIndex ); 
}

PDFCosHandle PDFDocAppendImage ( PDFDocHandle Doc, PMImage Image, TImageCompressionType ImageCompressionType )
{
    ppUns8 * ScanLineBuffer;
    ppUns32 i;
    PDFCosHandle obj;
	PDFCosHandle ar, di;
    PDFStreamHandle ImageStream;
    /*
    PDFStreamHandle ms;
    */
	if (  ImageCompressionType == itcJBIG2 || ImageCompressionType == itcCCITT42D )
		CheckEdition (_LIB, STD );
    if ( ( ImageCompressionType == itcJBIG2 || ImageCompressionType == itcCCITT42D ) && ( Image->Device != cgGray || Image->Depth != it1bit ) ){ 
        _LRAISE ( CD( Doc )->Lib, ErrDocumentLevel, dleColorImageWithBWCompressionError );
    }
    if ( ImageCompressionType == itcJPEG && Image->Device == cgGray && Image->Depth == it1bit ){ 
        _LRAISE ( CD( Doc )->Lib, ErrDocumentLevel, dleBWImageWithJPEGCompressionError );
    }
    obj = CosStreamNew ( Doc, true, 5 );
    ImageStream = CosStreamGetValue ( obj );
    switch ( ImageCompressionType ) {
            case itcJBIG2:
                PDFImageCompressJBIG2 ( Image, ImageStream );
                _StreamPosition ( ImageStream ) = 0;
                //////////////////////////////////////////////////////////////////////////
                /*
                ms=ULStreamFileNew(_LIB,"e:/3.bin",ppFileWriteMode);
                PDFTRY(_LIB){
                ULStreamCopyToStream( ImageStream, ms );
                }    PDFFINALLY ( _LIB ){
                ULStreamClose(ms);
                } PDFFINALLYEND ( _LIB );
                */
                //////////////////////////////////////////////////////////////////////////
                _CosDictAppend ( obj, Filter, _CosNameNew( JBIG2Decode ) );
              //  add = CosStreamNew( Doc, true, 1 );
              //  _CosDictAppend ( add, Length, CosIntNew( Doc, false, 0 ) );
              //  _CosDictAppend ( obj, DecodeParms, di = CosDictNew ( Doc, false, 1 ) );
              //  _CosDictAppend ( di, JBIG2Globals, add );
                break;
			case itcCCITT42D:
				PDFImageCompressCCITT ( Image, ImageStream );
				ar = CosArrayNew ( Doc, false, 1 ); 
				_CosDictAppend ( obj, Filter,ar );                                 
				CosArrayAppend ( ar, _CosNameNew ( CCITTFaxDecode ) );
				ar =  CosArrayNew( Doc, false, 1 );
				_CosDictAppend ( obj, DecodeParms, ar );
				di = CosDictNew ( Doc, false, 4 );
				CosArrayAppend ( ar, di );
				_CosDictAppend( di, K, CosIntNew ( Doc, false, -1 ) );
				_CosDictAppend( di, Columns,  CosIntNew ( Doc, false, Image->Width ) );
				_CosDictAppend( di, Rows,  CosIntNew ( Doc, false, Image->Height ) );
				_CosDictAppend( di, BlackIs1, CosBoolNew ( Doc, false, false ) );
				break;
            case itcFlate:
                ULStreamSetSize( ImageStream, Image->Height * Image->LineSize );
                _StreamPosition ( ImageStream ) = 0;
                for ( i=0; i < Image->Height; i++  ){
                    ScanLineBuffer = ScanLine ( Image, i );
                    ULStreamWriteBuffer ( ImageStream, ScanLineBuffer, Image->LineSize);
                }
                CosStreamFlatePack ( _LIB, obj );
                ImageStream = _CosStreamValue ( obj );
                _CosDictAppend ( obj, Filter,_CosNameNew( FlateDecode ) );
                break;
            case itcJPEG:
                PDFImageCompressJPEG ( Image, ImageStream, (ppUns8)CD(Doc)->JpegQuality);
                _CosDictAppend ( obj, Filter,_CosNameNew( DCTDecode ) );
                break;
    }; 
    _CosDictAppend ( obj, BitsPerComponent, CosIntNew( Doc, false, Image->Depth ) );
    _CosDictAppend( obj,Type,_CosNameNew ( XObject ) );
    _CosDictAppend ( obj, Length, CosIntNew( Doc, false,  _StreamSize( ImageStream ) ) );
    switch( Image->Device ){
        case cgGray:
            _CosDictAppend( obj,ColorSpace,_CosNameNew( DeviceGray ) );
            break;
        case cgRGB:
            _CosDictAppend( obj,ColorSpace,_CosNameNew( DeviceRGB ) );
            break;
        case cgCMYK:
            _CosDictAppend( obj,ColorSpace,_CosNameNew( DeviceCMYK ) );
            break;
    }
    _CosDictAppend ( obj, Subtype, _CosNameNew ( Image ) );
    _CosDictAppend ( obj, Width, CosIntNew ( Doc, false, Image->Width ) );
    _CosDictAppend ( obj, Height, CosIntNew ( Doc, false,Image->Height ) );
    return obj;
}


ppUns32 PDFImageAppendToDoc ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType )
{
    ppInt32 R;
    PDFCosHandle Obj;
    PPDFDocImageEntry IE;		
    Obj = PDFDocAppendImage ( Doc, ( PMImage )Image, CompressionType );
    IE = ( PDFDocImageEntry *) mmalloc ( _LIB, sizeof ( PDFDocImageEntry ) );
    PDFTRY ( _LIB ){
        if ( !_DOC->ImageList ){
            _DOC->ImageList = ULListCreate ( _LIB, 1 );
        };
        R = ULListAdd ( _DOC->ImageList, IE );
    } PDFEXCEPT ( _LIB ){
        mfree ( _LIB, IE );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    IE->Obj = Obj;
    IE->IsMask = false;
    return R;
}

#ifndef NOT_USE_TIFF

ppUns32 PDFImageGetTIFFCountFromFile ( PDFLibHandle Lib, char * FileName )
{
    PDFStreamHandle ms;
    ppInt32 Count;
	CheckEdition( Lib, STD);
    ms = ULStreamFileNew(Lib,FileName,ppFileReadMode);
    PDFTRY(Lib){
        Count = PDFImageGetTIFFCountFromStream ( Lib, ms );
    }PDFFINALLY ( Lib ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( Lib );
    return Count;
}

ppUns32 PDFImageGetTIFFCountFromBuffer ( PDFLibHandle Lib , void * Buffer, ppUns32 BufferSize )
{
    PDFStreamHandle ms;
    ppInt32 Count;
	CheckEdition( Lib, STD);
    ms = ULStreamMemReadOnlyNew ( Lib, Buffer, BufferSize );
    PDFTRY(Lib){
        Count = PDFImageGetTIFFCountFromStream ( Lib, ms );
    }PDFFINALLY ( Lib ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( Lib );
    return Count;
}

#endif


ppUns32 PDFImageAppendToDocAsMask ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType )
{
    ppUns32 Idx;
    PDFCosHandle obj;
    PPDFDocImageEntry IE;	
	CheckEdition( _LIB, STD);
    if ( TImage(Image)->Depth != it1bit || TImage ( Image )->Device != cgGray )
        _LRAISE ( TImage ( Image )->Lib, ErrDocumentLevel, dleNotBWImageAsMaskError );
    Idx = PDFImageAppendToDoc( Image, Doc, CompressionType );
    IE = ( PPDFDocImageEntry )( _DOC->ImageList->FList[ _DOC->ImageList->Size - 1 ] );
    obj = IE->Obj;
	IE ->IsMask = true;
    _CosDictAppend ( obj, ImageMask, CosBoolNew ( Doc, false, true ) );    
    return Idx;
}


ppUns32 PDFImageAppendToDocWithMask ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType, ppUns32 MaskImageIndex )
{
    ppUns32 Idx;
    PDFCosHandle obj;
    PPDFDocImageEntry IE;	
    if ( !_DOC->ImageList || MaskImageIndex >= (ppUns32)_DOC->ImageList->Size )
        _RAISE ( ErrGeneralLevel, gleOutOfBoundError );
    if ( ! ( (PPDFDocImageEntry ) _DOC->ImageList->FList[ MaskImageIndex ])->IsMask )
        _RAISE ( ErrDocumentLevel, dleNotValidMaskImageError );
    Idx = PDFImageAppendToDoc( Image, Doc, CompressionType );
    IE = (PPDFDocImageEntry )( _DOC->ImageList->FList[ _DOC->ImageList->Size - 1 ]);
    obj = IE->Obj;
    IE = (PPDFDocImageEntry )( _DOC->ImageList->FList[ MaskImageIndex ] );
    _CosDictAppend ( obj, Mask, IE->Obj );    
    return Idx;
}


void PDFImageChange ( PMImage Image, ppUns32 NewWidth, ppUns32 NewHeight, ppComponentDepth NewDepth, TPDFColorDevice NewDevice )
{
    ppInt32 SL;
    if ( Image->ImageBuffer ){
        mfree ( Image->Lib, Image->ImageBuffer );
    }
    SL = GetScanlineSize ( NewWidth, NewDepth, NewDevice );
    Image->ImageBuffer = mmalloc ( Image->Lib, SL * NewHeight );
    Image->Width = NewWidth;
    Image->Height = NewHeight;
    Image->Depth = NewDepth;
    Image->LineSize = SL;
    Image->Device = NewDevice;
}

ppUns32 PDFImageAppendToDocFromFile ( PDFDocHandle Doc, char * FileName, ppUns32 Index, TImageCompressionType CompressionType )
{
    PDFStreamHandle ms;
    ppUns32 Tmp;
    ms = ULStreamFileNew(_LIB,FileName,ppFileReadMode);
    PDFTRY(_LIB){
        Tmp = PDFImageAppendToDocFromStream( Doc, ms, Index, CompressionType );
    }PDFFINALLY ( _LIB ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( _LIB );
    return Tmp;
}


ppUns32 PDFImageAppendToDocFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 BufferSize, ppUns32 Index, TImageCompressionType CompressionType )
{
    PDFStreamHandle ms;
    ppUns32 Tmp;
    ms = ULStreamMemReadOnlyNew ( _LIB, Buffer, BufferSize );
    PDFTRY(_LIB){
        Tmp = PDFImageAppendToDocFromStream( Doc, ms, Index, CompressionType );
    }PDFFINALLY ( _LIB ){
        ULStreamClose(ms);
    }PDFFINALLYEND ( _LIB );
    return Tmp;
}


ppUns32 PDFImageAppendToDocFromStream ( PDFDocHandle Doc, PDFStreamHandle AStream, ppUns32 Index, TImageCompressionType CompressionType )
{
    PDFImageHandle Img;
    ppUns32 R;
    Img = PDFImageLoadFromStream( _LIB, AStream, Index );
    R = PDFImageAppendToDoc ( Img, Doc, CompressionType );
    PDFImageFree( Img );
    return R;
}

