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
VSType1.c
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "VSMisc.h"

#ifndef NOT_USE_FREETYPE

#include "VSType1.h"

#include	"ft2build.h"
#include	"freetype.h"
#include	"t1tables.h"
#include	"ftsnames.h"
#include	"ftsystem.h"
#include	"ftobjs.h"
#include	"VSFTSystem.h"
#include "../VSExcept.h"

#include "VSFreeType.h"
#include "ft2build.h"
#include "ftconfig.h"

#include "VSTypes.h"
#include "VSLibI.h"



#include FT_INTERNAL_DEBUG_H
#include FT_SYSTEM_H
#include FT_ERRORS_H
#include FT_TYPES_H
#include FT_INTERNAL_OBJECTS_H

/* Check End of Stream 											*/
#define	EoS(x)		( ULStreamLookChar(x) == -1 ) ? true : false

/* Type1 flags	*/

#define	T1FLAG_FIXEDPITCH		0x0001L
#define	T1FLAG_SERIF			0x0002L
#define	T1FLAG_SYMBOLIC			0x0004L
#define	T1FLAG_SCRIPT			0x0008L
#define	T1FLAG_NOSYMBOLIC		0x0020L
#define	T1FLAG_ITALIC			0x0040L
#define	T1FLAG_ALLCAPS			0x0100L
#define	T1FLAG_SMALLCAP			0x0200L
#define	T1FLAG_FORCEBOLD		0x0400L

 


void*  vspdf_alloc( FT_Memory  memory, long       size )
{
	PDFLibHandle	Lib = ( PDFLibHandle )memory->user;
	return ( mmalloc( Lib, size ));
}


void*  vspdf_realloc( FT_Memory  memory, long cur_size, long new_size, void* block )  
{
	PDFLibHandle	Lib = ( PDFLibHandle )memory->user;
	FT_UNUSED( cur_size );

	return (mrealloc( Lib, block, new_size ));
}

void vspdf_free( FT_Memory  memory, void*      block )
{
	PDFLibHandle Lib = ( PDFLibHandle )memory->user;	
	mfree( Lib, block );
}

FT_Memory  VSPDF_New_Memory( PDFLibHandle Lib )  
{
	FT_Memory  memory;


	memory = (FT_Memory)mmalloc( Lib, sizeof ( *memory ) );
	if ( memory )
	{
		memory->user    = ( void *)Lib;
		memory->alloc   = vspdf_alloc;
		memory->realloc = vspdf_realloc;
		memory->free    = vspdf_free;
	}

	return memory;
}


/* documentation is in ftobjs.h */

void  VSPDF_Done_Memory( FT_Memory  memory )
{
	PDFLibHandle	Lib = (PDFLibHandle)memory->user;
	mfree(  Lib, memory );
}

FT_Error  VSPDF_Init_FreeType( PDFLibHandle Lib, FT_Library  *alibrary )
{
	FT_Error   error;
	FT_Memory  memory;


	/* First of all, allocate a new system object -- this function is part */
	/* of the system-specific component, i.e. `ftsystem.c'.                */

	memory = VSPDF_New_Memory( Lib );
	if ( !memory )
	{
		FT_ERROR(( "FT_Init_FreeType: cannot find memory manager\n" ));
		return FT_Err_Unimplemented_Feature;
	}

	/* build a library out of it, then fill it with the set of */
	/* default drivers.                                        */

	error = FT_New_Library( memory, alibrary );
	if ( !error )
	{
		(*alibrary)->version_major = FREETYPE_MAJOR;
		(*alibrary)->version_minor = FREETYPE_MINOR;
		(*alibrary)->version_patch = FREETYPE_PATCH;

		FT_Add_Default_Modules( *alibrary );
	}

	return error;
}


FT_Error  VSPDF_Done_FreeType( FT_Library  library )
{
	if ( library )
	{
		FT_Memory  memory = library->memory;

		/* Discard the library object */
		FT_Done_Library( library );

		/* discard memory manager */
		VSPDF_Done_Memory( memory );
	}

	return FT_Err_Ok;
}



 
ppInt32 PDFType1GetFontName ( PDFStreamHandle Strm, char ** FontName )
{

	PDFLibHandle	Lib = _StreamLib ( Strm );  /* PDF Library handle	  */
	
	FT_Library	flib;				/* Font Library Handle				  */
	FT_Face		fface;				/* Font Face Structure				  */
	FT_Byte		*fbuff;				/* Memory byffer for file			  */
	ppInt32		got = 0;			/* Bytes readed from memory stream	  */
	ppInt32		pos = 0;			/* Curret stream position			  */
	ppInt32		len = 0;			/* Lenght of Type1 Stream 			  */
	PS_FontInfoRec		T1info;			// Type1 font info record
	ppInt32 			nlen = 0;		// Full Name string's lenght	 

    len = ULStreamGetSize ( Strm );
    pos = ULStreamSetPosition ( Strm, pos );
    if ( VSPDF_Init_FreeType ( Lib, &flib ) ) 
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel,  gleNotEnoughMemoryError ) );
    PDFTRY ( Lib ) {
        fbuff = ( FT_Byte * )mmalloc ( Lib, len );
        PDFTRY ( Lib ){
            /*	Read from PDF Stream to Memory Buffer */	
            got = ULStreamReadBuffer( Strm, ( void * )fbuff, len);
            /* Init Font Face from Memory Stream		*/
            if ( FT_New_Memory_Face ( flib, fbuff, got,	0, &fface ) )
                PDFRAISE(Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );					
            PDFTRY ( Lib ){
                if ( ! ( fface->face_flags & FT_FACE_FLAG_SFNT) ) {
                    if ( FT_Get_PS_Font_Info ( fface , &T1info ) )
                        PDFRAISE(Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );
                    nlen = (ppInt32)strlen ( T1info.full_name );
                    *FontName = (char *)mmalloc ( Lib, nlen + 1);
                    strcpy ( *FontName, ( char *)T1info.full_name );
                }
                else
                    /* Not a Type1 font					 */
                    PDFRAISE(Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );
            } PDFFINALLY ( Lib ){
                FT_Done_Face ( fface );
            } PDFFINALLYEND ( Lib );       
        } PDFFINALLY ( Lib ){
            mfree ( Lib, fbuff );
        } PDFFINALLYEND ( Lib );
    } PDFFINALLY ( Lib ) {
        VSPDF_Done_FreeType ( flib );
    } PDFFINALLYEND ( Lib );
     return 1;
}
/*
 *  Get Type1 Metrics from a font stream
 */
void PDFType1GetFontInfo ( PDFLibHandle Lib, PDFStreamHandle Strm, PFontMetrics T1Metrics, ppInt32 * Len1, ppInt32 * Len2, ppInt32 * Len3)
{
	FT_Library	flib;				/* Font Library Handle */
	FT_Face		fface;				/* Font Face Structure */
	FT_Byte		*fbuff;				/* Memory byffer for file */
	ppInt32		got = 0;			/* Bytes readed from memory stream */
	ppInt32		pos = 0;			/* Curret stream position */
	ppInt32		len = 0;			/* Lenght of Type1 Stream */			
	FT_UInt	glid	 = 0;			/* Index of Next Glyph in file */
	ppBool found = false;			/* found indicato */
	char	*line;					/* One Line from Sream */
	PS_FontInfoRec		T1info;		/* Type1 PostScript info */
	FT_ULong chcode;				/* Character code					 	*/
	
	
	/* Get Stream info & Set Stream Pointer to begin */
	len = ULStreamGetSize ( Strm );
	pos = ULStreamSetPosition ( Strm, pos );
	/* Init Font Library Handle	*/
	if ( VSPDF_Init_FreeType ( Lib, &flib )) 
		PDFRAISE( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleNotEnoughMemoryError ) );
	PDFTRY ( Lib ){
		/* Allocate memory buffer for font */
		fbuff = ( FT_Byte * )mmalloc ( Lib, len );
		PDFTRY ( Lib ){
			/*	Read from PDF Stream to Memory Buffer */	
			got = ULStreamReadBuffer( Strm, ( void * ) fbuff, len );
			
			/* Init Font Face from Memory Stream		*/
			if ( FT_New_Memory_Face ( flib, fbuff, got,	0, &fface ) )
				PDFRAISE(Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );	

			PDFTRY ( Lib ){

				/* Read Font Metrics from Font_Face Records	*/
				T1Metrics->Flags = 32;
				T1Metrics->Ascent = ( ppInt16 )fface->ascender;
				T1Metrics->Descent = ( ppInt16 )fface->descender;
				T1Metrics->UnderlinePosition = ( ppInt16 )fface->underline_position;
				T1Metrics->UnderlineThickness = ( ppInt16 )fface->underline_thickness;
				T1Metrics->FontBBox.XMin = ( ppInt16 )fface->bbox.xMin;
				T1Metrics->FontBBox.YMin = ( ppInt16 )fface->bbox.yMin;
				T1Metrics->FontBBox.XMax = ( ppInt16 )fface->bbox.xMax;
				T1Metrics->FontBBox.YMax = ( ppInt16 )fface->bbox.yMax;
				T1Metrics->StemV = 50;
				T1Metrics->CapHeight = ( ppInt32 ) ( fface->bbox.yMax - fface->bbox.yMin );
				/* If Loaded Font is Type1 font	Get ItalicAngle from PS Related table */
				if ( !( fface->face_flags & FT_FACE_FLAG_SFNT ) ){
					if ( FT_Get_PS_Font_Info ( fface , &T1info ) )
						PDFRAISE(Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );
					T1Metrics->ItalicAngle = (ppUns16)T1info.italic_angle;
				}
				else /* Not a Type1 font */
					PDFRAISE( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotReceiveFontDataError ) );

				/* Fill ANSICodes Array with Width of first 256 Glyphs */
				chcode = FT_Get_First_Char ( fface, &glid ); /* Get First Char Code and Glyph Index from table */
				while ( glid != 0 )	{
					if ( chcode > 255 ) break; /* Table is full */
					/* Load no scale Glyph to FontFace Structure */
					if ( FT_Load_Glyph( fface,  glid, FT_LOAD_NO_SCALE ))
						PDFRAISE( Lib, PDFBuildErrCode(ErrGeneralLevel, gleCannotReceiveFontDataError ) );
					/* Get Glyph width from Glyph's metrics				*/
					T1Metrics->ANSICodes[ chcode ] = (ppUns16)fface->glyph->metrics.horiAdvance;
					/* Get Next Char Code and Glyph Index				*/
					chcode = FT_Get_Next_Char ( fface, chcode, &glid );
				}  

			} PDFFINALLY ( Lib ){
				FT_Done_Face ( fface );	/* close font face */
			} PDFFINALLYEND ( Lib );		
		} PDFFINALLY ( Lib ){
			mfree ( Lib, fbuff );
		} PDFFINALLYEND ( Lib ); 	
	} PDFFINALLY ( Lib ){
		VSPDF_Done_FreeType ( flib );	/* close FreeType library */
	} PDFFINALLYEND ( Lib );	

	/* Calculate lenght1 , lenght2, lenght3	*/
		/* Clear-text portion */			
		/* Set stream pointer to begin position */
		ULStreamSetPosition ( Strm, 0 );
		/* read strings from stream	*/		

		while ( !EoS ( Strm ) ){
			line = ULStreamReadLine ( Strm );
			PDFTRY ( Lib ){
				/* Find <eexec> Postscript command */ 	
				if ( strstr ( line, "eexec" ) ){
					/* Set Lenght1 - Clear Text Portion Lenght	*/
					*Len1 = ULStreamGetPosition ( Strm ) - 1;
					/*Find fixed portion & Set current position on the 3/4 of stream size */
					ULStreamSetPosition ( Strm, len * 3 / 4 );
				}
				if ( strstr ( line, "cleartomark" ) ){
					found = true;
					pos = ULStreamGetPosition(Strm);
					/* Stream size = Current position + 520 "0" + 8 EndOfLine + Lenght of line with cleartomark + EoL */
					*Len3 = (ppInt32)(len - pos + 520 + strlen ( line ) + 1);
				}								
			} PDFFINALLY ( Lib ){
				mfree ( Lib, line );
			} PDFFINALLYEND ( Lib );		
		} 
								
		if ( !found )  *Len3 = 0;  /* Possible value, font filter must add Fixed portion */

		/* Encrypted portion */
		/* Encrypted portion = entire stream lenght - Fixed - ClearText	*/
		*Len2 = len - (*Len1 + *Len3);
}	



  
#endif
