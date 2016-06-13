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
VSDraw.h
*********************************************************************/

#ifndef VSDraw_H
#define VSDraw_H /* Base Type Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */
#include "VSTypes.h"
#include "VSCanvasA.h"



typedef struct TPDFDrawManager *PPDFDrawManager;

typedef void ( *PDExcuteOperatorProc) ( PPDFDrawManager manager, PDFCosHandle *Operand, ppUns32 OperandCount);

#define  MAXCOLORCOUNT 10
#define MAX_OPERAND_COUNT 6
#define OPERATOR_COUNT 73

typedef enum PDFOperandType {
	otNone,
	otNumber,
	otString,
	otName,
	otInteger,
	otArray, 
	otSCN,
	otProperty
} PDFOperandType;

typedef enum PDFDrawingLevel {
	dlText,
	dlColorText,
	dlFullImage
} PDFDrawingLevel;

typedef struct PDOperatorProperty{
	ppUns32 OperatorID;
	ppUns32 OperandCount;
	PDFOperandType Operands[6];
	ppBool IsRandom;
	PDFDrawingLevel Level;
	PDExcuteOperatorProc Operation;
} PDOperatorProperty;

#pragma pack ( 1 )
typedef struct TPDFDrawManagerFunctionList{
	PDExcuteOperatorProc	APDShowTextWithOptions;	/* " */
	PDExcuteOperatorProc	APDShowTextOnNextLine;	/* ' */
	PDExcuteOperatorProc	APDFillAndStroke;	/* B */
	PDExcuteOperatorProc	APDFillCompatible;	/* F */
	PDExcuteOperatorProc	APDSetGrayStroke;	/* G */
	PDExcuteOperatorProc	APDSetLineJoin;	/* J */
	PDExcuteOperatorProc	APDSetCMYKStroke;	/* K */
	PDExcuteOperatorProc	APDSetMiterLimit;	/* M */
	PDExcuteOperatorProc	APDPopState;	/* Q */
	PDExcuteOperatorProc	APDStroke;	/* S */
	PDExcuteOperatorProc	APDClip;	/* W */
	PDExcuteOperatorProc	APDCloseFillAndStroke;	/* b */
	PDExcuteOperatorProc	APDCurve;	/* c */
	PDExcuteOperatorProc	APDSetDashArray;	/* d */
	PDExcuteOperatorProc	APDFill;	/* f */
	PDExcuteOperatorProc	APDSetGrayFill;	/* g */
	PDExcuteOperatorProc	APDCloseSubPath;	/* h */
	PDExcuteOperatorProc	APDSetFlatness;	/* i */
	PDExcuteOperatorProc	APDSetLineCap;	/* j */
	PDExcuteOperatorProc	APDSetCMYKFill;	/* k */
	PDExcuteOperatorProc	APDLineTo;	/* l */
	PDExcuteOperatorProc	APDMoveTo;	/* m  */
	PDExcuteOperatorProc	APDEndPath;	/* n */
	PDExcuteOperatorProc	APDPushState;	/* q */
	PDExcuteOperatorProc	APDCloseAndStroke;	/* s */
	PDExcuteOperatorProc	APDCurveV;	/* v */
	PDExcuteOperatorProc	APDSetLineWidth;	/* w */
	PDExcuteOperatorProc	APDCurveY;	/* y */
	PDExcuteOperatorProc	APDEvenOddFillAndStroke;	/* B* */
	PDExcuteOperatorProc	APDBeginInline;	/* BI */
	PDExcuteOperatorProc	APDBeginText;	/* BT */
	PDExcuteOperatorProc	APDBeginCompatibleSection;	/* BX */
	PDExcuteOperatorProc	APDSetColorSpaceStroke;	/* CS */
	PDExcuteOperatorProc	APDDesignMarkContentWithParams;	/* DP */
	PDExcuteOperatorProc	APDPaintXObject;	/* Do */
	PDExcuteOperatorProc	APDEndInline;	/* EI */
	PDExcuteOperatorProc	APDEndText;	/* ET */
	PDExcuteOperatorProc	APDEndCompatibleSection;	/* EX */
	PDExcuteOperatorProc	APDInlineInfo;	/* ID */
	PDExcuteOperatorProc	APDDesignMarkContent;	/* MP */
	PDExcuteOperatorProc	APDSetRGBStroke;	/* RG */
	PDExcuteOperatorProc	APDSetSCStroke;	/* SC */
	PDExcuteOperatorProc	APDSetTextPosNextLine;	/* T* */
	PDExcuteOperatorProc	APDSetTextPositionTD;	/* TD */
	PDExcuteOperatorProc	APDSetTextStrings;	/* TJ */
	PDExcuteOperatorProc	APDSetLeading;	/* TL */
	PDExcuteOperatorProc	APDSetCharacterSpacing;	/* Tc */
	PDExcuteOperatorProc	APDSetTextPositionTd;	/* Td */
	PDExcuteOperatorProc	APDFontSize;	/* Tf */
	PDExcuteOperatorProc	APDShowText;	/* Tj */
	PDExcuteOperatorProc	APDSetTextMatrix;	/* Tm */
	PDExcuteOperatorProc	APDSetRender;	/* Tr */
	PDExcuteOperatorProc	APDSetRise;	/* Ts */
	PDExcuteOperatorProc	APDSetWordSpacing;	/* Tw */
	PDExcuteOperatorProc	APDSetHorizontalScale;	/* Tz */
	PDExcuteOperatorProc	APDEvenOddClip;	/* W* */
	PDExcuteOperatorProc	APDCloseEvenOddFillAndStroke;	/* b* */
	PDExcuteOperatorProc	APDSetCTM;	/* cm */
	PDExcuteOperatorProc	APDSetColorSpaceFill;	/* cs */
	PDExcuteOperatorProc	APDSetType1Width;	/* d0 */
	PDExcuteOperatorProc	APDSetType1WidthAndBound;	/* d1 */
	PDExcuteOperatorProc	APDEvenOddFill;	/* f* */
	PDExcuteOperatorProc	APDSetGState;	/* gs */
	PDExcuteOperatorProc	APDRectangle;	/* re */
	PDExcuteOperatorProc	APDSetRGBFill;	/* rg */
	PDExcuteOperatorProc	APDSetIntent;	/* ri */
	PDExcuteOperatorProc	APDSetSCFill;	/* sc */
	PDExcuteOperatorProc	APDPaintShape;	/* sh */
	PDExcuteOperatorProc	APDBeginMarkContentWithParams;	/* BDC */
	PDExcuteOperatorProc	APDBeginMarkContent;	/* BMC */
	PDExcuteOperatorProc	APDEndMarkContent;	/* EMC */
	PDExcuteOperatorProc	APDSetSCNStroke;	/* SCN */
	PDExcuteOperatorProc	APDSetSCNFill;	/* scn */
} TPDFDrawManagerFunctionList;
#pragma pack ( )

typedef struct TPDFTM {
	ppReal a;
	ppReal b;
	ppReal c;
	ppReal d;
	ppReal e;
	ppReal f;
} TPDFTM;

typedef struct PDFDrawTextState {
	ppReal CharacterSpacing;
	ppReal WordSpacing;
	ppReal HorizontalScaling;
	ppReal Leading;
	ppAtom TextFont;
	ppReal TextFontSize;
	ppUns32 TextRenderingMode;
	ppReal TextRise;
	ppBool TextKnockout;
	TPDFTM TextMatrix;
	TPDFTM TextLineMatrix;
	TPDFTM TextRenderingMatix;
} PDFDrawTextState;

typedef struct PDFDrawColorSpace {
	ppBool IsStd;
	TPDFColorDevice Device;
} PDFDrawColorSpace;

typedef struct PDFDrawState {
	/* Device independed graphic state parameters*/
	TPDFTM CTM;
	void * ClippingPath;
	PDFDrawColorSpace StrokeColorSpace;
	PDFDrawColorSpace FillColorSpace;
	ppReal StrokeColor[MAXCOLORCOUNT];
	ppReal FillColor[MAXCOLORCOUNT];
	PDFDrawTextState TextState;
	ppReal LineWidth;
	TPDFLineCap LineCap;
	TPDFLineJoin LineJoin;
	ppReal MiterLimit;
	PDFCosHandle DashPattern;
	ppAtom RenderingIntent;
	ppBool StrokeAdjustment;
	PDFCosHandle BlendMode;
	PDFCosHandle SoftMask;
	ppReal AlphaConstant;
	ppBool AlphaSource;
	/* Device depended graphic state parameters*/
	ppBool Overprint;
	ppReal OverprintMode;
	ppAtom BlackGeneration;
	ppAtom UndercolorRemoval;
	PDFCosHandle Transfer;
	PDFCosHandle Halftone;
	ppReal Flatness;
	ppReal Smoothness;
	ppReal CurX, CurY;
} PDFDrawState, *PPDFDrawState;

typedef struct PDFDrawStateManager{
	PDFDrawState *StateStack;
	ppUns32 StackSize;
	ppUns32 StackCapacity;
} PDFDrawStateManager;


typedef struct TPDFDrawManager{
	PDFLibHandle Lib;
	PDFDocHandle Doc;
	PDFCosHandle Resource;
	PDFDrawingLevel Level;
	TPDFDrawManagerFunctionList *Funcs;
	PDFDrawStateManager State;
	void *Data;
} TPDFDrawManager;

#ifdef __cplusplus
extern "C"{
#endif

void PDParseContent( PPDFDrawManager DrawManager , PDFStreamHandle Content );
void PDStandardInit ( PDFLibHandle Lib, PPDFDrawManager DrawManager );
void PDStandardDone ( PPDFDrawManager DrawManager );

#ifdef __cplusplus
}
#endif
#endif /* #ifndef VSDraw_H */
