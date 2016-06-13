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
VSEmf.h
*********************************************************************/

#ifndef VSEmf_H
#define VSEmf_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSLibI.h"
#include "VSCanvasAI.h"
#include "VSDocAI.h"

#ifdef __cplusplus
extern "C"{
#endif
#ifdef WINDOWS_PLATFORM

#ifndef EMR_SMALLTEXTOUT
#define EMR_SMALLTEXTOUT  108
#endif
#ifndef EMR_SETTEXTJUSTIFICATION
#define EMR_SETTEXTJUSTIFICATION 120
#endif

#undef SMALLTEXT_TYPE_ANSI
#define SMALLTEXT_TYPE_ANSI 0x200

#undef SMALLTEXT_TYPE_WITHOUT_CLIP
#define SMALLTEXT_TYPE_WITHOUT_CLIP 0x100

#ifndef SB_CONST_ALPHA

	#define EMR_ALPHABLEND 114

	typedef struct tagEMRALPHABLEND
	{
		EMR     emr;
		RECTL   rclBounds;
		LONG    xDest;
		LONG    yDest;
		LONG    cxDest;
		LONG    cyDest;
		DWORD   dwRop;
		LONG    xSrc;
		LONG    ySrc;
		XFORM   xformSrc; 
		COLORREF crBkColorSrc; 
		DWORD   iUsageSrc;     
		DWORD   offBmiSrc;     
		DWORD   cbBmiSrc;      
		DWORD   offBitsSrc;    
		DWORD   cbBitsSrc;     
		LONG    cxSrc;
		LONG    cySrc;
	} EMRALPHABLEND, *PEMRALPHABLEND;
#endif

    typedef struct _t_EMRSMALLTEXTOUTClipA{
        EMR         emr;
        POINT       ptlReference;
        DWORD       nChars;
        DWORD       fOptions;
        DWORD       iGraphicsMode;
        float       exScale;
        float       eyScale;
        RECT        rclClip;
        char        cString[1];
    } EMRSMALLTEXTOUTCLIPA, *PEMRSMALLTEXTOUTCLIPA;

    typedef struct _t_EMRSMALLTEXTOUTA{
        EMR         emr;
        POINTL      ptlReference;
        DWORD       nChars;
        DWORD       fOptions;
        DWORD       iGraphicsMode;
        float       exScale;
        float       eyScale;
        char        cString[1];
    } EMRSMALLTEXTOUTA, *PEMRSMALLTEXTOUTA;


	typedef struct _t_EMFPattern{
		PDFCosHandle Pattern;
		void *Bitmap;
	}EMFPattern, *PEMFPattern;


    typedef struct _t_PDFEmf{
		ppUns32			PatternsSize;
		PEMFPattern		Patterns;
        ppInt32         MapMode;
        PBXHandle       PaintBox;
        ppReal          CalX;
        ppReal          CalY;
        ppReal          FontScale;
        HDC             DC;
        POINT           CurVal;
        ppBool          PolyFillMode;
        ppBool          BkMode;
        COLORREF        TextColor;
        COLORREF        BkColor;
        TPDFVertJust    VertMode;
        TPDFHorJust     HorMode;
        ppBool          UpdatePos;
        ppBool          Clipping;
        ppBool          CCW;
        ppBool          Check;
        LOGPEN          CPen;
        LOGBRUSH        CBrush;
        LOGFONT         CFont;
        COLORREF        CurFill;
        RECTL           ClipRect;
        ppBool          IsCR;
        ppBool          InText;
        ppBool          InPath;
        ppInt32         StretchBltMode;
        ppBool          IsNullBrush;
        ppInt32         CurRec;
        ppBool          FontChanged;
        ppBool          NT;
        XFORM           CurrentXF;
        ppBool          XFInited;
        XFORM           *TransformStack;
        ppInt32         TSS;
        ppInt32         TSC;
        ppReal          XOff;
        ppReal          YOff;
        ppReal          XScale;
        ppReal          YScale;
        ppInt32         WOrgX, VOrgX, WExtX, VExtX, WOrgY, VOrgY, WExtY, VExtY;
        ppUns32         HandlesCount;
        HGDIOBJ         *HandleTable;

        CDebugHeader (
            PDFStreamHandle DBG;
            char DBGString[1024];
            )
        } TPDFEmf, *PPDFEmf;

        POINT PDFEMFGetSize ( PBXHandle PaintBox, HGDIOBJ Metafile );
        void PDFEMFExecute(PBXHandle PaintBox, HGDIOBJ Metafile);
		void PBXTextFromBaseLine( PBXHandle PaintBox, ppBool BaseLine );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSEmf_H */
