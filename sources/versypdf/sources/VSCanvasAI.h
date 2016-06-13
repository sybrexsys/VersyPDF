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
VSCanvasAI.h
*********************************************************************/

#ifndef VSCanvasAI_H
#define VSCanvasAI_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "../VSPageA.h"
#include "../VSCanvasA.h"
#include "VSMisc.h"

#ifdef __cplusplus
extern "C"{
#endif


    typedef struct _t_PDFWorldTransform{
        XFORM BaseMatrix;
#ifdef WINDOWS_PLATFORM
        XFORM ScaleMatrix;
        XFORM EMFMatrix;
        XFORM STDMatrix;
        XFORM TextMatrix;
        XFORM Skip;
        ppBool UseSkip;
        ppBool UseEMF;
        ppBool UseScale;
#endif
        ppReal XScale;
        ppReal YScale;
    } PDFWorldTransform, *PDFWorldTransformP;

#define CALCSCALESOURCE 1000.0

    typedef struct {
        PDFDocHandle        Doc;
        PDFStreamHandle     Strm;
        PDFCosHandle        cs;
        PDFCosHandle        Res;
        PDFCosHandle        AcroformRes;
        ppReal              Width;
        ppReal              Height;
        ppInt32             Resolution;
        ppReal              E2I;
        ppReal              I2E;
        TPDFPageRotateAngle Rotate;
        ppBool              TextUsed;
        ppBool              ColorUsed;
        ppBool              GrayUsed;
        ppReal              CurrentFontSize;
        ppReal              HorizontalScaling;
        ppInt32             Render;
        ppReal              CharSpace;
        ppReal              WordSpace;
        ppBool              TextInited;
        ppReal              TextAngle;
        ppReal              X, Y;
        ppInt32             SaveCount;
        ppUns32             CurrentFontIndex;
        ppInt32             FontWasInitedLevel;
        ppBool              FontIsChanged;
        ppBool              UnderLine;
        ppBool              StrikeOut;
        ppInt32             FontSubIndex;
        ppInt32             FontInited;
        ppBool              BaseLine;
        ppBool              Unicode;
        ppReal              TX, TY;
		ppReal				FS;
        ppInt32             ResourceFontIndex;
        ppInt32             ResourceImageIndex;
        ppInt32             ResourceGStateIndex;
		ppInt32             ResourcePatternIndex;
        ppAtom              CurrentFontName;
		ppBool				EmulateItalic;
        PDFWorldTransform   WT;
        char                NextOperatorChar;
        CDebugHeader ( 
            PDFStreamHandle   DBG;
            )

#ifdef WINDOWS_PLATFORM
        ppUns8              Charset;
#endif
    } TPBX, *PPBX;

#define _PBOX(p) ((PPBX)p)
#define _PDOC(p) (CD(_PBOX(p)->Doc))

	PBXHandle PDFPatternCreateContent( PDFCosHandle Pattern, ppUns32 Resolution );
	PDFCosHandle PDFDocAddPattern( PDFDocHandle Doc, ppUns32 Width, ppUns32 Height, ppUns32 XStep, ppUns32 YStep );
	void PBXSetPattern( PBXHandle PaintBox, PDFCosHandle Pattern);

    void PBXBeginText( PBXHandle PaintBox );
    void PBXEndText( PBXHandle PaintBox );
    void PBXSetTextMatrix( PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Degrees );
    void PBXTextShow( PBXHandle PaintBox, char *TextStr );

	void PBXExt2Int(PPBX PB, ppReal *x, ppReal *y);
	void PBXSetAngle( PPBX PB, TPDFPageRotateAngle Rotate );
    void PBXSetTransform ( PBXHandle PaintBox, XFORM XF);

    void PBXExtTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation, char *TextStr,
        ppReal *Dx);
 
    void PBXExtUnicodeTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation,
        PppUns16 Text, ppInt32 Len, ppReal *Dx);
	void PBXExtGlyphTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation,
		PppUns16 Text, ppInt32 Len, ppReal *Dx);
	void PBXGlyphTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation,
		PppUns16 Text, ppInt32 Len);
	PDFStreamHandle PBXGetStreamHandle( PBXHandle PaintBox );



#define PBXEXT2INT(X) ((X)*_BOX->E2I)
#define PBXINT2EXT(X) ((X)*_BOX->I2E)

    TPDFRealPoint PBXIntArc( PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2,
        ppReal y2, ppReal x3, ppReal y3, ppReal x4, ppReal y4 );
    


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSCanvasAI_H */
