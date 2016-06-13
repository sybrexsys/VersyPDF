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
VSFontAI.h
*********************************************************************/

#ifndef VSFontAI_H
#define VSFontAI_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "../VSFontA.h"
#include "VSTtf.h"

#ifdef WINDOWS_PLATFORM
#include <Windows.h>

#endif


#ifdef __cplusplus
extern "C"{
#endif
    
#define PDFSYMBOLENCODE 4
#define PDFZAPFDINGBATSENCODE 5



	typedef struct PDFStdFontMatrix{
		char * FontName;
		ppInt16 Abscent;
		ppInt16 Descent;
		ppInt16 BBox[4];	
	} PDFStdFontMatrix;

    typedef enum PDFFontType {
        PDFStdFont,
        PDFTrueTypeFont,
        PDFType1Font
    } PDFFontType;

	

    typedef struct PDFStdFontInfo {
        TPDFStdandardFont   ID;
        TPDFEncodingType    Encode;
        PDFCosHandle        Handle;
    } PDFStdFontInfo;

    typedef struct PDFTrueTypeANSIInfo {
        ppUns8       FirstChar;
        ppUns8       LastChar;
        ppBool       UsedChars[256];
        ppBool       Modified;
        PDFCosHandle Font;
        PDFCosHandle FontDescriptor;
        PDFCosHandle FontFile;
    }PDFTrueTypeANSIInfo, *PDFTrueTypeANSIInfoP;

    typedef struct PDFTrueTypeUnicodeInfo {
        PDFCosHandle ToUnicode;
        PDFCosHandle Compl;
        PDFCosHandle Font;
        PDFCosHandle FontDescriptor;
        PDFCosHandle FontFile;
        ppBool       Modified;
    }PDFTrueTypeUnicodeInfo, *PDFTrueTypeUnicodeInfoP;


    typedef struct PDFTrueTypeInfo {
        ppAtom                  FontName;
        PDFTrueTypeANSIInfoP    ANSI;
        PDFTrueTypeUnicodeInfoP Unicode;
        PPDFFontTables          Font;
    } PDFTrueTypeInfo, *PPDFTrueTypeInfo;

    typedef struct PDFType1Info {
        ppAtom                  FontName;
        PDFCosHandle            Font;
        PDFCosHandle FontDescriptor;
        PDFCosHandle FontFile;
    } PDFType1Info, *PPDFType1Info;

    typedef struct PDFFontInfo {
        PDFLibHandle    Lib;
        PDFFontType     Type;
        union {
            PDFStdFontInfo      StdInfo;
            PPDFTrueTypeInfo    TTFInfo;
            PPDFType1Info       T1Info;
        } FontInfo;
        TFontMetrics        metrics;
    }PDFFontInfo, *PPDFFontInfo;

#ifdef WINDOWS_PLATFORM
    typedef struct TTFListItem {
        ppAtom  Name;
        ppBool  Bold;
        ppBool  Italic;
        ppInt32 Index;
    }TTFListItem, *TTFListItemP;
#define TTFIt(l,i) (((TTFListItemP *)l->FList)[i])

    ppUns32 PDFFontAppendFromDC ( PDFDocHandle Doc, HDC DC );

#endif

#define FE(x) ((PPDFFontInfo)x)
#define FONTDoc(Doc, x) ((PPDFFontInfo)CD(Doc)->FontList->FList[x])

    void PDFFontFillUsed(PPDFFontInfo Font, ppUns8 *Text);
    void PDFFontTrueTypeClear( PDFLibHandle Lib, PPDFFontInfo Font);
    void PDFTTFFontUpdate(PDFDocHandle Doc);
    

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSFontAI_H */
