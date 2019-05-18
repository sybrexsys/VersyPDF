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

#ifndef NOT_USE_PNG

#include "VSDocAI.h"
#include "VSPlatform.h"
#include "VSFilter.h"
#include "VSExcept.h"

png_voidp PDFPNGmalloc_fn(png_structp png_ptr, png_size_t size){
    return mmalloc ( (PDFLibHandle) png_ptr->mem_ptr, size );
}

void PDFPNGfree_fn(png_structp png_ptr, png_voidp ptr){
    mfree ( (PDFLibHandle)png_ptr->mem_ptr, ptr );
}

void CDECL PDFPNGuser_error_fn(png_structp png_ptr, png_const_charp error_msg){
    PDFRAISE ( (PDFLibHandle)png_ptr->error_ptr, PDFBuildErrCode ( ErrGeneralLevel, gleLoadPNGFileError ) );
}

void CDECL PDFPNGuser_warning_fn(png_structp png_ptr, png_const_charp warning_msg){

}

void CDECL PDFPNGuser_read_data(png_structp png_ptr, png_bytep data, png_size_t length){
    ULStreamReadBuffer ( ( PDFStreamHandle ) png_get_io_ptr(png_ptr), data, (ppUns32)length );
}




PMImage PDFImageLoadPNGFromStream ( PDFStreamHandle Stream )
{
    PMImage Tmp;
    png_struct * png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    png_infop end_info;
    ppUns32 i;
    TPDFColorDevice Device;
    ppComponentDepth Depth;
    void * SL;
    PDFLibHandle Lib = _StreamLib ( Stream );
    png_ptr = png_create_read_struct_2( PNG_LIBPNG_VER_STRING, Lib, 
        PDFPNGuser_error_fn, PDFPNGuser_warning_fn, Lib,
        PDFPNGmalloc_fn, PDFPNGfree_fn );
    PDFTRY ( Lib ){
        info_ptr = png_create_info_struct( png_ptr );
    } PDFEXCEPT ( Lib ){
        png_destroy_read_struct(&png_ptr, ( png_infopp )NULL, ( png_infopp )NULL);
        PDFRERAISE ( Lib );
    } PDFTRYEND ( Lib );
    png_set_read_fn ( png_ptr, Stream , PDFPNGuser_read_data );
    end_info = png_create_info_struct(png_ptr);
    PDFTRY ( Lib ){
        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16 , NULL);
        if ( info_ptr->channels == 1 ){
            Device = cgGray;
        } else {
            Device = cgRGB;
        }
        Depth = ( ppComponentDepth ) info_ptr->bit_depth;
        Tmp = ( PMImage )PDFImageCreate ( Lib, (ppUns32)info_ptr->width , (ppUns32)info_ptr->height, Depth, Device );
        PDFTRY(Lib){    
            row_pointers = png_get_rows(png_ptr, info_ptr);
            for ( i = 0; i <  info_ptr->height; i++  ){
                SL = ScanLine ( Tmp, i );
                memcpy ( SL, row_pointers[i], Tmp->LineSize );
            }
        } PDFEXCEPT(Lib){
            PDFImageFree( Tmp );
            PDFRERAISE ( Lib );
        }
        PDFTRYEND(Lib);
    } PDFFINALLY ( Lib ){
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info);
    } PDFFINALLYEND ( Lib );
    return Tmp;
}

#endif