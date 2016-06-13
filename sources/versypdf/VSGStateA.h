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
VSGStateA.h
*********************************************************************/

#ifndef VSGStateA_H
#define VSGStateA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"
#include "VSCanvasA.h"
#ifdef __cplusplus
extern "C"{
#endif
	/* Converting CIE-based colors to device colors  */
    typedef enum {
		ReIntAbsoluteColormetric,    /* Colors are represented solely with respect to the light source; no
									 correction is made for the output medium’s white point (such as
									 the color of unprinted paper). */
		ReIntRelativeColorMetrics,   /* Colors are represented with respect to the combination of the
								 	 light source and the output medium’s white point (such as the
									 color of unprinted paper).    */
        ReIntSaturation,             /* Colors are represented in a manner that preserves or 
									 emphasizes saturation. */
        ReIntPerceptual              /* Colors are represented in a manner that provides a pleasing 
									 perceptual appearance. */
    } TPDFRenderingIntents;
    
	/* Blending mode */
    _EXP
    typedef enum {
        blmoNormal,                  /* Selects the source color, ignoring the backdrop */ 
        blmoMultiply,                /* Multiplies the backdrop and source color values */ 
		blmoScreen,                  /* Multiplies the complements of the backdrop and source color
									 values, then complements the result*/
        blmoOverlay,                 /* Multiplies or screens the colors, depending on the backdrop color. */
        blmoDarken,                  /* Selects the darker of the backdrop and source colors. */ 
        blmoLighten,                 /* Selects the lighter of the backdrop and source colors. */
		blmoColorDodge,              /* Brightens the backdrop color to reflect the source color. Painting
									 with black produces no change. */
        blmoColorBurn,               /* Darkens the backdrop color to reflect the source color. 
									 Painting with white produces no change. */
        blmoHardLight,               /* Multiplies or screens the colors, depending on the source 
									 color value. */
        blmoSoftLight,               /* Darkens or lightens the colors, depending on the source color value. */
        blmoDifference,              /* Subtracts the darker of the two constituent colors from the lighter. */
		blmoExclusion                /* Produces an effect similar to that of the Difference mode, but lower
									 in contrast. */
    } TPDFBlendMode;


	/********************************************************************
    Description:
        Creates graphic state in document.
    Parameters:
           Doc                  - [ in ] Current PDF document.
    Returns:
        Graphic state index.
    *********************************************************************/
    LIB_API ppUns32 PDFExtGraphicStateNew(PDFDocHandle Doc);

	/* 
	Description
		Setting CTM ( current transformation matrix ) width for current graphic state.
	Parameters
		Doc :        [ in, out ] Current PDF document.
		GState :     [ in ] Graphic state index.
		a :  [ in ] A part of the CTM.
		b :  [ in ] B part of the CTM.
		c :  [ in ] C part of the CTM.
		d :  [ in ] D part of the CTM.
		e :  [ in ] E part of the CTM.
		f :  [ in ] F part of the CTM.
	Returns
		None.                                          
	*/
	LIB_API void PDFExtGraphicStateSetCTM(PDFDocHandle Doc, ppUns32 GState, ppReal a, ppReal b, ppReal c, ppReal d, ppReal e, ppReal f );

	/* Description
	   Setting line width for current graphic state.
	   Parameters
	   Doc :        [ in, out ] Current PDF document.
	   GState :     [ in ] Graphic state index.
	   LineWidth :  [ in ] Line width.
	   Returns
	   None.                                          */
    LIB_API void PDFExtGraphicStateSetLineWidth(PDFDocHandle Doc, ppUns32 GState, ppReal LineWidth);


	/* Description
	   Setting line cap for current graphic state.
	   Parameters
	   Doc :      [ in, out ] Current PDF document.
	   GState :   [ in ] Graphic state index.
	   LineCap :  [ in ] Line cap.
	   Returns
	   None.                                        */
    LIB_API void PDFExtGraphicStateSetLineCap(PDFDocHandle Doc, ppUns32 GState, TPDFLineCap LineCap);


	/* Description
	   Setting line cap for current graphic state.
	   Parameters
	   Doc :       [ in, out ] Current PDF document.
	   GState :    [ in ] Graphic state index.
	   LineJoin :  [ in ] Line join.
	   Returns
	   None.                                         */
	LIB_API void PDFExtGraphicStateSetLineJoin(PDFDocHandle Doc, ppUns32 GState, TPDFLineJoin LineJoin);


	/* Description
	   Setting mitter limit for current graphic state.
	   Parameters
	   Doc :          [ in, out ] Current PDF document.
	   GState :       [ in ] Graphic state index.
	   MitterLimit :  [ in ] Mitter limit.
	   Returns
	   None.                                            */
	LIB_API void PDFExtGraphicStateSetMitterLimit(PDFDocHandle Doc, ppUns32 GState, ppReal MitterLimit);


	/* Description
	   Setting dash pattern for current graphic state.
	   Parameters
	   Doc :          [ in, out ] Current PDF document.
	   GState :       [ in ] Graphic state index.
	   DashPattern :  [ in ] Dash pattern.
	   Returns
	   None.                                            */
    LIB_API void PDFExtGraphicStateSetDashPattern(PDFDocHandle Doc, ppUns32 GState, PDFCosHandle DashPattern);


/* Description
	   Setting "alpha is shape" for current graphic state.
	   Parameters
	   Doc :           [ in, out ] Current PDF document.
	   GState :        [ in ] Graphic state index.
	   AlphaIsShape :  [ in ] Alpha is shape.
	   Returns
	   None.                                               */
	LIB_API void PDFExtGraphicStateSetAlphaIsShape(PDFDocHandle Doc, ppUns32 GState, ppBool AlphaIsShape);


	/* Description
	   Setting alpha stroke for current graphic state.
	   Parameters
	   Doc :           [ in, out ] Current PDF document.
	   GState :        [ in ] Graphic state index.
	   AlphaIsShape :  [ in ] Alpha stroke.
	   Returns
	   None.                                             */
	LIB_API void PDFExtGraphicStateSetAlphaStroke(PDFDocHandle Doc, ppUns32 GState, ppReal AlphaStroke);


	/* Description
	   Setting alpha fill for current graphic state.
	   Parameters
	   Doc :        [ in, out ] Current PDF document.
	   GState :     [ in ] Graphic state index.
	   AlphaFill :  [ in ] Alpha fill.
	   Returns
	   None.                                          */
	LIB_API void PDFExtGraphicStateSetAlphaFill(PDFDocHandle Doc, ppUns32 GState, ppReal AlphaFill);


	/* Description
	   Setting text knockout for current graphic state.
	   Parameters
	   Doc :           [ in, out ] Current PDF document.
	   GState :        [ in ] Graphic state index.
	   TextKnockout :  [ in ] Text knockout.
	   Returns
	   None.                                             */
	LIB_API void PDFExtGraphicStateSetTextKnockout(PDFDocHandle Doc, ppUns32 GState, ppBool TextKnockout);


	/* Description
	   Setting overprint stroke for current graphic state.
	   Parameters
	   Doc :              [ in, out ] Current PDF document.
	   GState :           [ in ] Graphic state index.
	   OverprintStroke :  [ in ] Overprint stroke.
	   Returns
	   None.                                                */
    LIB_API void PDFExtGraphicStateSetOverprintStroke(PDFDocHandle Doc, ppUns32 GState, ppBool OverprintStroke);


	/* Description
	   Setting overprint fill for current graphic state.
	   Parameters
	   Doc :            [ in, out ] Current PDF document.
	   GState :         [ in ] Graphic state index.
	   OverprintFill :  [ in ] Overprint fill.
	   Returns
	   None.                                              */
    LIB_API void PDFExtGraphicStateSetOverprintFill(PDFDocHandle Doc, ppUns32 GState, ppBool OverprintFill);


	/* Description
	   Setting overprint mode for current graphic state.
	   Parameters
	   Doc :            [ in, out ] Current PDF document.
	   GState :         [ in ] Graphic state index.
	   OverprintMode :  [ in ] Overprint mode.
	   Returns
	   None.                                              */
    LIB_API void PDFExtGraphicStateSetOverprintMode(PDFDocHandle Doc, ppUns32 GState, ppInt32 OverprintMode);


	/* Description
	   Setting flatness for current graphic state.
	   Parameters
	   Doc :       [ in, out ] Current PDF document.
	   GState :    [ in ] Graphic state index.
	   Flatness :  [ in ] Graphic state flatness.
	   Returns
	   None.                                         */
    LIB_API void PDFExtGraphicStateSetFlatness(PDFDocHandle Doc, ppUns32 GState, ppReal Flatness);



	/* Description
	   Setting graphic state rendering intents.
	   Parameters
	   Doc :     [ in, out ] Current PDF document.
	   GState :  [ in ] Graphic state index.
	   Intent :  [ in ] Rendering intents.
	   Returns
	   None.                                       */
    LIB_API void PDFExtGraphicStateSetRenderingIntent(PDFDocHandle Doc, ppUns32 GState, TPDFRenderingIntents Intent);


	/* Description
	   Setting blending mode for current graphic state.
	   Parameters
	   Doc :        [ in, out ] Current PDF document.
	   GState :     [ in ] Graphic state index.
	   BlendMode :  [ in ] Blending mode.
	   Returns
	   None.                                            */
    LIB_API void PDFExtGraphicStateSetBlendMode(PDFDocHandle Doc, ppUns32 GState, TPDFBlendMode BlendMode);


	/* Description
	   Setting smoothness for current graphic state.
	   Parameters
	   Doc :         [ in, out ] Current PDF document.
	   GState :      [ in ] Graphic state index.
	   Smoothness :  [ in ] Smoothness value.
	   Returns
	   None.                                           */
    LIB_API void PDFExtGraphicStateSetSmoothness(PDFDocHandle Doc, ppUns32 GState, ppReal Smoothness);


	/* Description
	   Setting stroke adjustment for current graphic state.
	   Parameters
	   Doc :               [ in, out ] Current PDF document.
	   GState :            [ in ] Graphic state index.
	   StrokeAdjustment :  [ in ] Stroke adjustment.
	   Returns
	   None.                                                 */
    LIB_API void PDFExtGraphicStateSetStrokeAdjustment(PDFDocHandle Doc, ppUns32 GState, ppBool StrokeAdjustment);


#ifdef __cplusplus
}
#endif
#endif /* #ifndef VSGStateA_H */
