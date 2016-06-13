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
VSDraw.c
*********************************************************************/


#include "VSDraw.h"
#include "VSError.h"
#include "VSExcept.h"
#include "VSMisc.h"
#include "VSCosAI.h"
#include "VSDocAI.h"
#include "VSFilter.h"
#include "VSParse.h" 
#include <string.h>
 
#ifdef ERE

//cvgv

void PDStateFree( PPDFDrawManager DrawManager, ppUns32 Index)
{
	CosFree ( DrawManager->State.StateStack[Index].DashPattern );
	CosFree ( DrawManager->State.StateStack[Index].BlendMode );
	CosFree ( DrawManager->State.StateStack[Index].SoftMask );
	CosFree ( DrawManager->State.StateStack[Index].Transfer );
	CosFree ( DrawManager->State.StateStack[Index].Halftone );
}

void PDStatePush( PPDFDrawManager DrawManager )
{
	ppInt32 c;
	c = DrawManager->State.StackSize;
	if ( DrawManager->State.StackSize == DrawManager->State.StackCapacity){
		DrawManager->State.StackCapacity += CalculateDelta(DrawManager->State.StackCapacity);
		DrawManager->State.StateStack = ( PDFDrawState* )mrealloc ( DrawManager->Lib, DrawManager->State.StateStack, 
			DrawManager->State.StackCapacity * sizeof( PDFDrawState ) );
	}
	memmove(&(DrawManager->State.StateStack[c]),&(DrawManager->State.StateStack[c-1]), sizeof(PDFDrawState));
	
	DrawManager->State.StateStack[c].DashPattern = CosCopy(DrawManager->Doc, DrawManager->State.StateStack[c-1].DashPattern) ;
	DrawManager->State.StateStack[c].BlendMode = CosCopy(DrawManager->Doc, DrawManager->State.StateStack[c-1].BlendMode);
	DrawManager->State.StateStack[c].SoftMask = CosCopy(DrawManager->Doc, DrawManager->State.StateStack[c-1].SoftMask);
	DrawManager->State.StateStack[c].Transfer = CosCopy(DrawManager->Doc, DrawManager->State.StateStack[c-1].Transfer) ;
	DrawManager->State.StateStack[c].Halftone = CosCopy(DrawManager->Doc, DrawManager->State.StateStack[c-1].Halftone);
	DrawManager->State.StackSize++;
}

void PDStatePop( PPDFDrawManager DrawManager )
{
	if (DrawManager->State.StackSize != 1 ){
		DrawManager->State.StackSize--;
		PDStateFree ( DrawManager, DrawManager->State.StackSize );
	}
}
#define STATE DrawManager->State.StateStack[DrawManager->State.StackSize]

void PDStateSetDefValue ( PPDFDrawManager DrawManager, ppUns32 Index)
{
	DrawManager->State.StateStack[Index].DashPattern = _CosNull(DrawManager->Doc);
	DrawManager->State.StateStack[Index].BlendMode = CosNameNew(DrawManager->Doc, false, GetAtomLib(DrawManager->Lib,Normal));
	DrawManager->State.StateStack[Index].SoftMask = CosNameNew(DrawManager->Doc, false, GetAtomLib(DrawManager->Lib,None));
	DrawManager->State.StateStack[Index].Transfer = _CosNull(DrawManager->Doc);
	DrawManager->State.StateStack[Index].Halftone = _CosNull(DrawManager->Doc);
	DrawManager->State.StateStack[Index].CTM.a = 1;
	DrawManager->State.StateStack[Index].CTM.b = 0;
	DrawManager->State.StateStack[Index].CTM.c = 0;
	DrawManager->State.StateStack[Index].CTM.d = 1;
	DrawManager->State.StateStack[Index].CTM.e = 0;
	DrawManager->State.StateStack[Index].CTM.f = 0;
	DrawManager->State.StateStack[Index].ClippingPath = NULL;
	DrawManager->State.StateStack[Index].StrokeColorSpace.Device = cgGray;
	DrawManager->State.StateStack[Index].StrokeColorSpace.IsStd = true;
	DrawManager->State.StateStack[Index].FillColorSpace.Device = cgGray;
	DrawManager->State.StateStack[Index].FillColorSpace.IsStd = true;
	DrawManager->State.StateStack[Index].StrokeColor[0] = 0;
	DrawManager->State.StateStack[Index].FillColor[0] = 0;
	DrawManager->State.StateStack[Index].LineWidth = 1;
	DrawManager->State.StateStack[Index].LineCap = 0;
	DrawManager->State.StateStack[Index].LineJoin = 0;
	DrawManager->State.StateStack[Index].MiterLimit = 10.0;
	DrawManager->State.StateStack[Index].RenderingIntent = GetAtomLib(DrawManager->Lib, RelativeColorimetric);
	DrawManager->State.StateStack[Index].StrokeAdjustment = false;
	DrawManager->State.StateStack[Index].AlphaConstant = 1;
	DrawManager->State.StateStack[Index].AlphaSource = false;
	DrawManager->State.StateStack[Index].Overprint = false;
	DrawManager->State.StateStack[Index].OverprintMode = 0;
	DrawManager->State.StateStack[Index].BlackGeneration = ppAtomNull;
	DrawManager->State.StateStack[Index].UndercolorRemoval = ppAtomNull;
	DrawManager->State.StateStack[Index].Flatness = 1;
	DrawManager->State.StateStack[Index].Smoothness = 0;
	DrawManager->State.StateStack[Index].CurX = 0;
	DrawManager->State.StateStack[Index].CurY = 0;

	DrawManager->State.StateStack[Index].TextState.CharacterSpacing = 0;
	DrawManager->State.StateStack[Index].TextState.WordSpacing = 0;
	DrawManager->State.StateStack[Index].TextState.HorizontalScaling = 100;
	DrawManager->State.StateStack[Index].TextState.Leading = 0;
	DrawManager->State.StateStack[Index].TextState.TextRenderingMode = 0;
	DrawManager->State.StateStack[Index].TextState.TextRise = 0;
}

void PDStateSetGState ( PPDFDrawManager DrawManager, ppAtom StateName)
{

}

void PDStandardInit ( PDFLibHandle Lib, PPDFDrawManager DrawManager )
{
	DrawManager->State.StackCapacity = DrawManager->State.StackSize = 1;
	DrawManager->State.StateStack = (PDFDrawState *)mmalloc(Lib,sizeof (PDFDrawState));
	PDStateSetDefValue ( DrawManager, 0);
}

void PDStandardDone ( PPDFDrawManager DrawManager )
{
	ppUns32 i;
	for ( i = 0; i < DrawManager->State.StackSize; i++)
		PDStateFree ( DrawManager, i);
	mfree( DrawManager->Lib, DrawManager->State.StateStack);
}


void PDSetCMYKStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.StrokeColorSpace.IsStd = true;
	STATE.StrokeColorSpace.Device = cgCMYK;
	STATE.StrokeColor[0] = _CosNumGetValue (Operand[0]);
	STATE.StrokeColor[1] = _CosNumGetValue (Operand[1]);
	STATE.StrokeColor[2] = _CosNumGetValue (Operand[2]);
	STATE.StrokeColor[3] = _CosNumGetValue (Operand[3]);
	return false;
}

void PDSetCMYKFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.FillColorSpace.IsStd = true;
	STATE.FillColorSpace.Device = cgCMYK;
	STATE.FillColor[0] = _CosNumGetValue (Operand[0]);
	STATE.FillColor[1] = _CosNumGetValue (Operand[1]);
	STATE.FillColor[2] = _CosNumGetValue (Operand[2]);
	STATE.FillColor[3] = _CosNumGetValue (Operand[3]);
	return false;
}

void PDSetRGBStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.StrokeColorSpace.IsStd = true;
	STATE.StrokeColorSpace.Device = cgRGB;
	STATE.StrokeColor[0] = _CosNumGetValue (Operand[0]);
	STATE.StrokeColor[1] = _CosNumGetValue (Operand[1]);
	STATE.StrokeColor[2] = _CosNumGetValue (Operand[2]);
	return false;
}

void PDSetRGBFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.FillColorSpace.IsStd = true;
	STATE.FillColorSpace.Device = cgRGB;
	STATE.FillColor[0] = _CosNumGetValue (Operand[0]);
	STATE.FillColor[1] = _CosNumGetValue (Operand[1]);
	STATE.FillColor[2] = _CosNumGetValue (Operand[2]);
	return false;
}

void PDSetGrayStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.StrokeColorSpace.IsStd = true;
	STATE.StrokeColorSpace.Device = cgGray;
	STATE.StrokeColor[0] = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetGrayFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.FillColorSpace.IsStd = true;
	STATE.FillColorSpace.Device = cgGray;
	STATE.FillColor[0] = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetLineWidth( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.LineWidth = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetLineJoin( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.LineJoin = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetLineCap( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.LineCap = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetMiterLimit( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.MiterLimit = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetFlatness( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetHorizontalScale( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.HorizontalScaling = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetLeading( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.Leading	= _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetCharacterSpacing( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.CharacterSpacing = _CosNumGetValue (Operand[0]);
	return false;
}


void PDSetTextMatrix( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetRender( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.TextRenderingMode = _CosIntValue (Operand[0]);
	return false;
}

void PDSetRise( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.TextRise = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetWordSpacing( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.WordSpacing = _CosNumGetValue (Operand[0]);
	return false;
}

void PDBeginText( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.TextState.TextMatrix.a = 1;
	STATE.TextState.TextMatrix.b = 0;
	STATE.TextState.TextMatrix.c = 0;
	STATE.TextState.TextMatrix.d = 1;
	STATE.TextState.TextMatrix.e = 0;
	STATE.TextState.TextMatrix.f = 0;
	return false;
}

void PDPopState( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	PDStatePop( DrawManager);
	return false;
}


void PDPushState( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	PDStatePush( DrawManager);
	return false;
}

void PDSetCTM( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.CTM.a = _CosNumGetValue (Operand[0]);
	STATE.CTM.b = _CosNumGetValue (Operand[1]);
	STATE.CTM.c = _CosNumGetValue (Operand[2]);
	STATE.CTM.d = _CosNumGetValue (Operand[3]);
	STATE.CTM.e = _CosNumGetValue (Operand[4]);
	STATE.CTM.f = _CosNumGetValue (Operand[5]);
	return false;
}

/* Unprocessed */


void PDSetDashArray( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetColorSpaceStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetSCStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}


void PDSetColorSpaceFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetSCFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetSCNStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetSCNFill( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetColorSpaceStroke( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}

void PDSetGState( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount )
{
	STATE.Flatness = _CosNumGetValue (Operand[0]);
	return false;
}



static PDOperatorProperty OperatorList[OPERATOR_COUNT] = {
	{ 0x22 /* " */, 3, {otNumber, otNumber, otString}, false, dlText, NULL},
	{ 0x27 /* ' */, 1, {otString}, false, dlText, NULL},
	{ 0x42 /* B */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x46 /* F */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x47 /* G */, 1, {otNumber}, false, dlColorText, NULL},
	{ 0x4a /* J */, 1, {otInteger}, false, dlFullImage, NULL },
	{ 0x4b /* K */, 4, {otNumber, otNumber, otNumber, otNumber}, false, dlColorText, NULL},
	{ 0x4d /* M */, 1, {otNumber}, false, dlFullImage, NULL },
	{ 0x51 /* Q */, 0 ,{otNone}, false , dlFullImage, NULL},
	{ 0x53 /* S */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x57 /* W */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x62 /* b */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x63 /* c */, 6, {otNumber, otNumber, otNumber, otNumber, otNumber, otNumber}, false, dlFullImage, NULL},
	{ 0x64 /* d */, 2, {otArray, otInteger}, false, dlFullImage, NULL },
	{ 0x66 /* f */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x67 /* g */, 1, {otNumber}, false, dlColorText, NULL},
	{ 0x68 /* h */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x69 /* i */, 1, {otNumber }, false, dlFullImage, NULL },
	{ 0x6a /* j */, 1, {otInteger}, false, dlFullImage, NULL },
	{ 0x6b /* k */, 4, {otNumber, otNumber, otNumber, otNumber}, false, dlColorText, NULL},
	{ 0x6c /* l */, 2, {otNumber, otNumber}, false, dlFullImage, NULL},
	{ 0x6d /* m */, 2, {otNumber, otNumber}, false, dlFullImage, NULL}, 
	{ 0x6e /* n */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x71 /* q */, 0, {otNone }, false, dlFullImage, NULL },
	{ 0x73 /* s */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x76 /* v */, 4, {otNumber, otNumber, otNumber, otNumber}, false, dlFullImage, NULL},
	{ 0x77 /* w */, 1, {otNumber}, false, dlFullImage, NULL },
	{ 0x79 /* y */, 4, {otNumber, otNumber, otNumber, otNumber}, false, dlFullImage, NULL},
	{ 0x422a /* B* */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x4249 /* BI */, 0, {otNone}, false, dlText, NULL},
	{ 0x4254 /* BT */, 0, {otNone}, false, dlText, NULL},
	{ 0x4258 /* BX */, 0, {otNone}, false,dlText, NULL},
	{ 0x4353 /* CS */, 1, {otName}, false, dlColorText, NULL },
	{ 0x4450 /* DP */, 2, {otName, otProperty}, false, dlFullImage, NULL},
	{ 0x446f /* Do */, 1, {otName}, false, dlText, NULL},
	{ 0x4549 /* EI */, 0, {otNone}, false, dlText, NULL},
	{ 0x4554 /* ET */, 0, {otNone}, false, dlText, NULL},
	{ 0x4558 /* EX */, 0, {otNone}, false, dlText, NULL},
	{ 0x4944 /* ID */, 0, {otNone}, false, dlText, NULL},
	{ 0x4d50 /* MP */, 1, {otName}, false, dlFullImage, NULL },
	{ 0x5247 /* RG */, 3, {otNumber, otNumber, otNumber}, false, dlColorText, NULL},
	{ 0x5343 /* SC */, 4, {otNumber, otNumber, otNumber, otNumber}, true, dlColorText, NULL},
	{ 0x542a /* T* */, 0, {otNone}, false,dlText, NULL},
	{ 0x5444 /* TD */, 2, {otNumber, otNumber}, false,dlText, NULL},
	{ 0x544a /* TJ */, 1, {otArray}, false,dlText, NULL},
	{ 0x544c /* TL */, 1, {otNumber}, false,dlText, NULL},
	{ 0x5463 /* Tc */, 1, {otNumber}, false,dlText, NULL},
	{ 0x5464 /* Td */, 2, {otNumber, otNumber}, false,dlText, NULL},
	{ 0x5466 /* Tf */, 2, {otName, otNumber}, false,dlText, NULL},
	{ 0x546a /* Tj */, 1, {otString}, false,dlText, NULL},
	{ 0x546d /* Tm */, 6, {otNumber, otNumber, otNumber, otNumber, otNumber, otNumber}, false,dlText, NULL},
	{ 0x5472 /* Tr */, 1, {otInteger}, false,dlText, NULL},
	{ 0x5473 /* Ts */, 1, {otNumber}, false,dlText, NULL},
	{ 0x5477 /* Tw */, 1, {otNumber}, false,dlText, NULL},
	{ 0x547a /* Tz */, 1, {otNumber}, false,dlText, NULL},
	{ 0x572a /* W* */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x622a /* b* */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x636d /* cm */, 6, { otNumber, otNumber, otNumber, otNumber, otNumber, otNumber}, false, dlFullImage, NULL },
	{ 0x6373 /* cs */, 2, {otName}, false, dlColorText, NULL},
	{ 0x6430 /* d0 */, 2, {otNumber, otNumber}, false, dlText, NULL },
	{ 0x6431 /* d1 */, 6, {otNumber, otNumber, otNumber, otNumber, otNumber, otNumber}, false, dlText, NULL},
	{ 0x662a /* f* */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x6773 /* gs */, 1, {otName }, false, dlColorText, NULL},
	{ 0x7265 /* re */, 4, {otNumber, otNumber, otNumber, otNumber}, false, dlFullImage, NULL},
	{ 0x7267 /* rg */, 3, {otNumber, otNumber, otNumber}, false, dlColorText, NULL},
	{ 0x7269 /* ri */, 1 ,{otName}, false, dlFullImage, NULL},
	{ 0x7363 /* sc */, 4, {otNumber, otNumber, otNumber, otNumber}, true, dlColorText, NULL},
	{ 0x7368 /* sh */, 1, {otName}, false, dlFullImage, NULL},
	{ 0x424443 /* BDC */, 2, {otName, otProperty}, false, dlFullImage, NULL},
	{ 0x424d43 /* BMC */, 1, {otName}, false, dlFullImage, NULL},
	{ 0x454d43 /* EMC */, 0, {otNone}, false, dlFullImage, NULL},
	{ 0x53434e /* SCN */, 5, {otSCN, otSCN, otSCN, otSCN, otSCN}, true, dlColorText, NULL},
	{ 0x73636e /* scn */, 5, {otSCN, otSCN, otSCN, otSCN, otSCN}, true, dlColorText, NULL}
};





static TPDFDrawManagerFunctionList PDFStatesManagerList = {
		NULL, /*	APDShowTextWithOptions;	*/	/* " */
		NULL, /*	APDShowTextOnNextLine;	*/	/* ' */
		NULL, /*	APDFillAndStroke;	*/	/* B */
		NULL, /*	APDFillCompatible;	*/	/* F */
		PDSetGrayStroke, /*	APDSetGrayStroke;	*/	/* G */
		PDSetLineJoin, /*	APDSetLineJoin;	*/	/* J */
		PDSetCMYKStroke, /*	APDSetCMYKStroke;	*/	/* K */
		PDSetMiterLimit, /*	APDSetMiterLimit;	*/	/* M */
		PDPopState, /*	APDPopState;	*/	/* Q */
		NULL, /*	APDStroke;	*/	/* S */
		NULL, /*	APDClip;	*/	/* W */
		NULL, /*	APDCloseFillAndStroke;	*/	/* b */
		NULL, /*	APDCurve;	*/	/* c */
		PDSetDashArray, /*	APDSetDashArray;	*/	/* d */
		NULL, /*	APDFill;	*/	/* f */
		PDSetGrayFill, /*	APDSetGrayFill;	*/	/* g */
		NULL, /*	APDCloseSubPath;	*/	/* h */
		PDSetFlatness, /*	APDSetFlatness;	*/	/* i */
		PDSetLineCap, /*	APDSetLineCap;	*/	/* j */
		PDSetCMYKFill, /*	APDSetCMYKFill;	*/	/* k */
		NULL, /*	APDLineTo;	*/	/* l */
		NULL, /*	APDMoveTo;	*/	/* m  */
		NULL, /*	APDEndPath;	*/	/* n */
		PDPushState, /*	APDPushState;	*/	/* q */
		NULL, /*	APDCloseAndStroke;	*/	/* s */
		NULL, /*	APDCurveV;	*/	/* v */
		PDSetLineWidth, /*	APDSetLineWidth;	*/	/* w */
		NULL, /*	APDCurveY;	*/	/* y */
		NULL, /*	APDEvenOddFillAndStroke;	*/	/* B* */
		NULL, /*	APDBeginInline;	*/	/* BI */
		PDBeginText, /*	APDBeginText;	*/	/* BT */
		NULL, /*	APDBeginCompatibleSection;	*/	/* BX */
		PDSetColorSpaceStroke, /*	APDSetColorSpaceStroke;	*/	/* CS */
		NULL, /*	APDDesignMarkContentWithParams;	*/	/* DP */
		NULL, /*	APDPaintXObject;	*/	/* Do */
		NULL, /*	APDEndInline;	*/	/* EI */
		NULL, /*	APDEndText;	*/	/* ET */
		NULL, /*	APDEndCompatibleSection;	*/	/* EX */
		NULL, /*	APDInlineInfo;	*/	/* ID */
		NULL, /*	APDDesignMarkContent;	*/	/* MP */
		PDSetRGBStroke, /*	APDSetRGBStroke;	*/	/* RG */
		PDSetSCStroke, /*	APDSetSCStroke;	*/	/* SC */
		NULL, /*	APDSetTextPosNextLine;	*/	/* T* */
		NULL, /*	APDSetTextPositionTD;	*/	/* TD */
		NULL, /*	APDSetTextStrings;	*/	/* TJ */
		PDSetLeading, /*	APDSetLeading;	*/	/* TL */
		PDSetCharacterSpacing, /*	APDSetCharacterSpacing;	*/	/* Tc */
		NULL, /*	APDSetTextPositionTd;	*/	/* Td */
		NULL, /*	APDFontSize;	*/	/* Tf */
		NULL, /*	APDShowText;	*/	/* Tj */
		PDSetTextMatrix, /*	APDSetTextMatrix;	*/	/* Tm */
		PDSetRender, /*	APDSetRender;	*/	/* Tr */
		PDSetRise, /*	APDSetRise;	*/	/* Ts */
		PDSetWordSpacing, /*	APDSetWordSpacing;	*/	/* Tw */
		PDSetHorizontalScale, /*	APDSetHorizontalScale;	*/	/* Tz */
		NULL, /*	APDEvenOddClip;	*/	/* W* */
		NULL, /*	APDCloseEvenOddFillAndStroke;	*/	/* b* */
		PDSetCTM, /*	APDSetCTM;	*/	/* cm */
		NULL, /*	APDSetColorSpaceFill;	*/	/* cs */
		PDSetColorSpaceFill, /*	APDSetType1Width;	*/	/* d0 */
		NULL, /*	APDSetType1WidthAndBound;	*/	/* d1 */
		NULL, /*	APDEvenOddFill;	*/	/* f* */
		PDSetGState, /*	APDSetGState;	*/	/* gs */
		NULL, /*	APDRectangle;	*/	/* re */
		PDSetRGBFill, /*	APDSetRGBFill;	*/	/* rg */
		NULL, /*	APDSetIntent;	*/	/* ri */
		PDSetSCFill, /*	APDSetSCFill;	*/	/* sc */
		NULL, /*	APDPaintShape;	*/	/* sh */
		NULL, /*	APDBeginMarkContentWithParams;	*/	/* BDC */
		NULL, /*	APDBeginMarkContent;	*/	/* BMC */
		NULL, /*	APDEndMarkContent;	*/	/* EMC */
		PDSetSCNStroke, /*	APDSetSCNStroke;	*/	/* SCN */
		PDSetSCNFill, /*	APDSetSCNFill;	*/	/* scn */
};



void PDProcessOperator ( PPDFDrawManager DrawManager, PPDFParser Parser, PDFCosHandle *OperandStack, ppUns32 OperandCount )
{
	char *Operator;
	ppUns32 Lo,Hi, Mid, i, h;
	ppBool Err = false;
	PDOperatorProperty *OperatorProperty;
	PDExcuteOperatorProc *Procs, *StdProcs;
	Operator = _CosWorkCommand ( OperandStack[OperandCount]);
	for (h = 0;*Operator;Operator++)
		h = ( h << 8 ) | (ppUns8)*Operator;
	Lo = 0;
	Hi = OPERATOR_COUNT - 1;
	while ( Hi - Lo > 1){
		Mid = ( Lo + Hi ) / 2;
		if (  OperatorList[ Mid ].OperatorID == h )
			break;
		if ( OperatorList[ Mid ].OperatorID < h )
			Lo = Mid;
		else 
			Hi = Mid;
	}
	if ( OperatorList[ Mid ].OperatorID != h )
		return;
	OperatorProperty = &OperatorList[ Mid ];
	if ( DrawManager->Level < OperatorProperty->Level )
		return;
	Procs = (PDExcuteOperatorProc *) DrawManager->Funcs ;
	Procs += Mid;
	StdProcs = (PDExcuteOperatorProc *) &PDFStatesManagerList ;
	StdProcs += Mid;
	if ( !*Procs && !*StdProcs )
		return;
	if ( !OperatorProperty->IsRandom ){
		if ( OperandCount != OperatorProperty->OperandCount )
			return;
		for ( i = 0; i < OperandCount; i++ ){
			switch ( OperatorProperty->Operands[i] ){
				case otNumber:
					if ( !_CosIsNumber( OperandStack[i] ) )
						Err = true;
					break;
				case otInteger:
					if ( !_IsCosInt ( OperandStack[i]))
						Err = true;
					break;
				case otString:
					if ( !_IsCosString ( OperandStack[i]))
						Err = true;
					break;
				case otName:
					if ( !_IsCosName ( OperandStack[i]))
						Err = true;
					break;
				case otArray:
					if ( !_IsCosArray ( OperandStack[i]))
						Err = true;
					break;
				case otProperty:
					if ( !(_IsCosName ( OperandStack[i])||_IsCosDict ( OperandStack[i])))
						Err = true;
					break;
				default:;
			}
			if ( Err )
				return;
		}
	} else {
		if ( OperandCount > OperatorProperty->OperandCount )
			return;
		for ( i = 0; i < OperandCount; i++ )
			if ( !(_CosIsNumber ( OperandStack[i])||_IsCosName ( OperandStack[i])))
				return;
	}
/*	if (*StdProcs){
		if ((*StdProcs)( DrawManager, OperandStack, OperandCount -1	))
			if (*Procs)	
				(*Procs)( DrawManager, OperandStack, OperandCount -1);
	} else 
		(*Procs)( DrawManager, OperandStack, OperandCount - 1);*/
}

#define _LIB  CD(Doc)->Lib
#define OPERANDSTACKMAX 10
void PDParseContent( PPDFDrawManager DrawManager , PDFStreamHandle Content )
{	
	PDFCosHandle OperandStack[OPERANDSTACKMAX];
	ppUns32 StackPointer;
	ppUns32 i;
	ppBool Exit = false, IsOperator;
	TPDFParser Parser;
	CosType CT;
	if ( !DrawManager->Funcs)
		return;
	Parser.Doc = DrawManager->Doc;
	Parser.GenerateExceptionOnEOF = false;
	Parser.Stream = Content;
	ULStreamSetPosition( Content, 0 );
	do {
		StackPointer = 0;
		IsOperator = false;
		do {
			OperandStack[ StackPointer ] = ULParseCosObject ( &Parser );
			CT = _CosGetType ( OperandStack [ StackPointer ] );
			if ( CT == CosEOF ){
				Exit = true;
				for ( i = 0; i <= StackPointer; i++ )
					CosFree ( OperandStack[i]);
				IsOperator = true;
				continue;
			}
			if ( CT == CosWork ) {
				IsOperator = true;
				continue;
			}
			if ( StackPointer != OPERANDSTACKMAX -1 )
				StackPointer++;
		} while ( !IsOperator );
		if ( Exit )
			continue;
		PDProcessOperator ( DrawManager, &Parser, OperandStack, StackPointer );
		for ( i = 0; i <= StackPointer; i++ )
			CosFree ( OperandStack[i]);
	} while (!Exit);
}



#endif