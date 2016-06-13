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
VSFontA.h
*********************************************************************/

#ifndef VSFontA_H
#define VSFontA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"

#ifdef __cplusplus
extern "C"{
#endif



    /* Standard 14 fonts enum*/
_EXP
    typedef enum _t_TPDFStdandardFont {
        stdfHelvetica,                /* Helvetica font */
        stdfHelveticaBold,            /* Helvetica Bold font */
        stdfHelveticaOblique,         /* Helvetica Oblique font */
        stdfHelveticaBoldOblique,     /* Helvetica Bold Obliquefont */
        stdfTimesRoman,               /* Times Roman font */
        stdfTimesBold,                /* Times Bold font */
        stdfTimesItalic,              /* Times Italic font */
        stdfTimesBoldItalic,          /* Times Bold Italicfont */
        stdfCourier,                  /* Courier font */
        stdfCourierBold,              /* Courier Bold font */
        stdfCourierOblique,           /* Courier Oblique font */
        stdfCourierBoldOblique,       /* Courier BoldOblique font */
        stdfSymbol,                   /* Symbol font */
        stdfZapfDingbats              /* Zapf Dingbats font */
    } TPDFStdandardFont;



    /* Font encoding*/
_EXP
    typedef enum _t_TPDFEncodingType {
        etPDFDocEncoding,            /* PDF Document encoding */
        etWinAnsiEncoding,           /* ANSI windows encoding */
        etMacRomanEncoding,          /* Apple encoding */
        etStandardEncoding           /* Standard encoding */
    } TPDFEncodingType;


	/* Description
	   Append one of 14 standard fonts to document.
	   Parameters
	   Doc :     [ in ] Current PDF document.
	   font :    [ in ] One of standart font\: stdfHelvetica,
	             stdfHelveticaBold, stdfHelveticaOblique,
	             stdfHelveticaBoldOblique, stdfTimesRoman, stdfTimesBold,
	             stdfTimesItalic, stdfTimesBoldItalic, stdfCourier,
	             stdfCourierBold, stdfCourierOblique,
	             stdfCourierBoldOblique, stdfSymbol, stdfZapfDingbats<p/>
	   encode :  [ in ] Font encoding type.
	   Returns
	   Font index in PDF document.                                        */
    LIB_API ppUns32 PDFFontStandardAppend(PDFDocHandle Doc, TPDFStdandardFont font, TPDFEncodingType encode);

#ifdef WINDOWS_PLATFORM

/* Description
	   Append true type font to document by name.
	   Parameters
	   Doc :       [ in ] Current PDF document.
	   fontname :  [ in ] Font name.
	   Bold :      [ in ] Is bold style.
	   Italic :    [ in ] Is italic style.
	   Returns
	   Font index in PDF document.                */
    LIB_API ppUns32 PDFFontTrueTypeAppend(PDFDocHandle Doc, char *fontname, ppBool Bold, ppBool Italic);
#endif


	/* Description
	   Append true type font to document from file.
	   Parameters
	   Doc :       [ in ] Current PDF document.
	   fontname :  [ in ] Font filename.
	   Returns
	   Font index in PDF document.                  */
    LIB_API ppUns32 PDFFontTrueTypeAppendFromFile(PDFDocHandle Doc, char *fontfilename);


	/* Description
	   Append true type font to document from stream.
	   Parameters
	   Doc :   [ in ] Current PDF document.
	   Strm :  [ in ] Font stream.
	   Returns
	   Font index in PDF document.                    */
    LIB_API ppUns32 PDFFontTrueTypeAppendFromStream(PDFDocHandle Doc, PDFStreamHandle Strm);

#ifndef NOT_USE_FREETYPE 
	/* Description
	   Append Type1 font to document from file.
	   Parameters
	   Doc :       [ in ] Current PDF document.
	   fontname :  [ in ] Font filename.
	   Returns
	   Font index in PDF document.              */
    LIB_API ppUns32 PDFFontType1AppendFromFile(PDFDocHandle Doc, char * fontfilename);


	/* Description
	   Append Type1 font to document from stream.
	   Parameters
	   Doc :   [ in ] Current PDF document.
	   Strm :  [ in ] Font stream.
	   Returns
	   Font index in PDF document.                */
    LIB_API ppUns32 PDFFontType1AppendFromStream(PDFDocHandle Doc, PDFStreamHandle Strm);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSFontA_H */
