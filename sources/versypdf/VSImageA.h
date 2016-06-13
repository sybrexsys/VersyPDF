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
VSImageA.h
*********************************************************************/

#ifndef VSImageA_H
#define VSImageA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#ifdef __cplusplus
extern "C"{
#endif

#include "VSTypes.h"
#ifdef WINDOWS_PLATFORM
#include <windows.h>
#endif


/* Available image compression types */
_EXP
typedef enum _t_TImageCompressionType {
	itcFlate,            /* FLATE compression */
	itcJPEG,             /* JPEG compression */
	itcJBIG2,            /* JBIG2 compression */
	itcCCITT42D          /* CCITT 4 compression */
}TImageCompressionType;


typedef enum ppComponentDepth{
    it1bit  = 1,
    it2bit  = 2, 
    it4bit  = 4,
    it8bit  = 8,
    it16bit = 16,
} ppComponentDepth;


/********************************************************************
    Description:
        Create PDF image.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   Width                - [ in ] Specifies the width in pixels of the image.
           Height               - [ in ] Specifies the height in pixels of the image.
           Depth                - [ in ] Indicates the bit count per component of the image. 
           Device               - [ in ] Indicates the color space in which image samples are specified.
    Returns:
        PDF Image handle.
    *********************************************************************/

LIB_API PDFImageHandle PDFImageCreate ( PDFLibHandle Lib, ppUns32 Width, ppUns32 Height, ppComponentDepth Depth, TPDFColorDevice Device );

    /********************************************************************
    Description:
        Create PDF image from image file opened with PDF Stream.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   AStream             	- [ in ] Stream handle.
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
    Returns:
        PDF Image handle.
    *********************************************************************/
LIB_API PDFImageHandle PDFImageLoadFromStream ( PDFLibHandle Lib, PDFStreamHandle AStream, ppUns32 Index );

    /********************************************************************
    Description:
        Create PDF image from image file.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   FileName            	- [ in ] Filename of the file where stored image
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
    Returns:
        PDF Image handle.
    *********************************************************************/
LIB_API PDFImageHandle PDFImageLoadFromFile ( PDFLibHandle Lib, char * FileName, ppUns32 Index );

    /********************************************************************
    Description:
        Create PDF image from image file stored in memory buffer.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   Buffer            	- [ in ] Storage location for data.
           BufferSize           - [ in ] Size of the storage.
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
    Returns:
        PDF Image handle.
    *********************************************************************/

LIB_API PDFImageHandle PDFImageLoadFromBuffer ( PDFLibHandle Lib, void * Buffer, ppUns32 BufferSize, ppUns32 Index );

#ifdef WINDOWS_PLATFORM
    /********************************************************************
    Description:
        Create image from bitmap handle.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   Handle             	- [ in ] Bitmap handle.
    Returns:
        PDF Image handle.
    *********************************************************************/
LIB_API PDFImageHandle PDFImageLoadFromHandle ( PDFLibHandle Lib, HBITMAP Handle ); 

#endif 


    /********************************************************************
    Description:
        Free PDF image.
    Parameters:
           Image                - [ in ] PDF Image handle.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFImageFree ( PDFImageHandle Image );


    /********************************************************************
    Description:
        Gets the bit count per component of the image.
    Parameters:
           Image                - [ in ] PDF Image handle.
    Returns:
        The bit count.
    *********************************************************************/
LIB_API ppComponentDepth PDFImageGetDepth ( PDFImageHandle Image );


    /********************************************************************
    Description:
        Gets the color space in which image samples are specified.
    Parameters:
           Image                - [ in ] PDF Image handle.
    Returns:
        Color space.
    *********************************************************************/
LIB_API TPDFColorDevice PDFImageGetColorDevice ( PDFImageHandle Image );

    /********************************************************************
    Description:
        Gets the width of the image.
    Parameters:
           Image                - [ in ] PDF Image handle.
    Returns:
        Width in samples of the bitmap.
    *********************************************************************/
LIB_API ppUns32 PDFImageGetWidth ( PDFImageHandle Image );

    /********************************************************************
    Description:
        Gets the height of the image.
    Parameters:
           Image                - [ in ] PDF Image handle.
    Returns:
        Height in samples of the bitmap.
    *********************************************************************/
LIB_API ppUns32 PDFImageGetHeight ( PDFImageHandle Image );


    /********************************************************************
    Description:
        Provides indexed access to each line of samples.
    Parameters:
           Image                - [ in ] PDF Image handle.
           ScanLineIndex        - [ in ] Index of line of the pixels.
    Returns:
        Pointer to line of the samples.
    *********************************************************************/
LIB_API void * PDFImageGetScanLine ( PDFImageHandle Image, ppUns32 ScanLineIndex );


    /********************************************************************
    Description:
        Appends image to document with specified compression.
    Parameters:
           Image                - [ in ] PDF Image handle.
           Doc                  - [ in ] PDF document handle.
           CompressionType	    - [ in ] Image compression type.
    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDoc ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType );


    /********************************************************************
    Description:
        Appends image to document with specified compression from file.
    Parameters:
           Doc                  - [ in ] PDF document handle.
           FileName             - [ in ] Filename of file where stored image
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
           CompressionType	    - [ in ] Image compression type.
    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDocFromFile ( PDFDocHandle Doc, char * FileName, ppUns32 Index, TImageCompressionType CompressionType );

    /********************************************************************
    Description:
        Appends image to document with specified compression from memory buffer.
    Parameters:
           Doc                  - [ in ] PDF document handle.
           Buffer            	- [ in ] Storage location for data.
           BufferSize           - [ in ] Size of the storage.
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
           CompressionType	    - [ in ] Image compression type.
    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDocFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 BufferSize, ppUns32 Index, TImageCompressionType CompressionType );

    /********************************************************************
    Description:
        Appends image to document with specified compression from stream.
    Parameters:
           Doc                  - [ in ] PDF document handle.
           AStream              - [ in ] Stream where stored image
           Index                - [ in ] Index of the image if file ( Used only for Tiff files )
           CompressionType	    - [ in ] Image compression type.
    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDocFromStream ( PDFDocHandle Doc, PDFStreamHandle AStream, ppUns32 Index, TImageCompressionType CompressionType );


    /********************************************************************
    Description:
        Appends b/w image to document with specified compression as transparent mask.
    Parameters:
           Image                - [ in ] PDF Image handle.
           Doc                  - [ in ] PDF document handle.
           CompressionType	    - [ in ] Image compression type.
    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDocAsMask ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType );

    /********************************************************************
    Description:
        Appends image to document with specified compression with transparent mask.
    Parameters:
           Image                - [ in ] PDF Image handle.
           Doc                  - [ in ] PDF document handle.
           CompressionType	    - [ in ] Image compression type.

    Returns:
        Index of the image in PDF document.
    *********************************************************************/

LIB_API ppUns32 PDFImageAppendToDocWithMask ( PDFImageHandle Image, PDFDocHandle Doc, TImageCompressionType CompressionType, ppUns32 MaskImageIndex );


#ifndef NOT_USE_TIFF
    /********************************************************************
    Description:
        Gets count of the images in the tiff file opened with PDF Stream.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   AStream             	- [ in ] Stream handle.
    Returns:
        Count of the images.
    *********************************************************************/
LIB_API ppUns32 PDFImageGetTIFFCountFromStream ( PDFLibHandle Lib, PDFStreamHandle AStream );

    /********************************************************************
    Description:
        Gets count of the images in the tiff file.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
		   FileName            	- [ in ] Filename of the file where stored image.
    Returns:
        Count of the images.
    *********************************************************************/
LIB_API ppUns32 PDFImageGetTIFFCountFromFile ( PDFLibHandle Lib, char * FileName );

    /********************************************************************
    Description:
        Gets count of the images in the tiff file stored in memory buffer.
    Parameters:
           Lib                  - [ in ] PDF Library handle.
           Buffer            	- [ in ] Storage location for data.
           BufferSize           - [ in ] Size of the storage.
    Returns:
        Count of the images.
    *********************************************************************/
LIB_API ppUns32 PDFImageGetTIFFCountFromBuffer ( PDFLibHandle Lib, void * Buffer, ppUns32 BufferSize );

#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSImageA_H */
