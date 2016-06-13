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
VSTextOut.c
*********************************************************************/


#include "VSTextOut.h"
#include "VSMisc.h"
#include "VSDocAI.h"
#include "VSCosAI.h"
#include "VSPageA.h"
#include "VSFilter.h"
#define _LIB _DOC->Lib
#define _DM ((PPDFDrawManager)DrawManager) 
#define _DATA ((PPDFTextOutData)_DM->Data)

ppBool PDTOBeginText( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount)
{
	return true;
}
ppBool PDTOEndText( PPDFDrawManager DrawManager, PDFCosHandle *Operand, ppUns32 OperandCount)
{
	return true;
}

static TPDFDrawManagerFunctionList PDFTextOutManagerList = {
	NULL, /*	APDShowTextWithOptions;	*/	/* " */
	NULL, /*	APDShowTextOnNextLine;	*/	/* ' */
	NULL, /*	APDFillAndStroke;	*/	/* B */
	NULL, /*	APDFillCompatible;	*/	/* F */
	NULL, /*	APDSetGrayStroke;	*/	/* G */
	NULL, /*	APDSetLineJoin;	*/	/* J */
	NULL, /*	APDSetCMYKStroke;	*/	/* K */
	NULL, /*	APDSetMiterLimit;	*/	/* M */
	NULL, /*	APDPopState;	*/	/* Q */
	NULL, /*	APDStroke;	*/	/* S */
	NULL, /*	APDClip;	*/	/* W */
	NULL, /*	APDCloseFillAndStroke;	*/	/* b */
	NULL, /*	APDCurve;	*/	/* c */
	NULL, /*	APDSetDashArray;	*/	/* d */
	NULL, /*	APDFill;	*/	/* f */
	NULL, /*	APDSetGrayFill;	*/	/* g */
	NULL, /*	APDCloseSubPath;	*/	/* h */
	NULL, /*	APDSetFlatness;	*/	/* i */
	NULL, /*	APDSetLineCap;	*/	/* j */
	NULL, /*	APDSetCMYKFill;	*/	/* k */
	NULL, /*	APDLineTo;	*/	/* l */
	NULL, /*	APDMoveTo;	*/	/* m  */
	NULL, /*	APDEndPath;	*/	/* n */
	NULL, /*	APDPushState;	*/	/* q */
	NULL, /*	APDCloseAndStroke;	*/	/* s */
	NULL, /*	APDCurveV;	*/	/* v */
	NULL, /*	APDSetLineWidth;	*/	/* w */
	NULL, /*	APDCurveY;	*/	/* y */
	NULL, /*	APDEvenOddFillAndStroke;	*/	/* B* */
	NULL, /*	APDBeginInline;	*/	/* BI */
	PDTOBeginText, /*	APDBeginText;	*/	/* BT */
	NULL, /*	APDBeginCompatibleSection;	*/	/* BX */
	NULL, /*	APDSetColorSpaceStroke;	*/	/* CS */
	NULL, /*	APDDesignMarkContentWithParams;	*/	/* DP */
	NULL, /*	APDPaintXObject;	*/	/* Do */
	NULL, /*	APDEndInline;	*/	/* EI */
	PDTOEndText, /*	APDEndText;	*/	/* ET */
	NULL, /*	APDEndCompatibleSection;	*/	/* EX */
	NULL, /*	APDInlineInfo;	*/	/* ID */
	NULL, /*	APDDesignMarkContent;	*/	/* MP */
	NULL, /*	APDSetRGBStroke;	*/	/* RG */
	NULL, /*	APDSetSCStroke;	*/	/* SC */
	NULL, /*	APDSetTextPosNextLine;	*/	/* T* */
	NULL, /*	APDSetTextPositionTD;	*/	/* TD */
	NULL, /*	APDSetTextStrings;	*/	/* TJ */
	NULL, /*	APDSetLeading;	*/	/* TL */
	NULL, /*	APDSetCharacterSpacing;	*/	/* Tc */
	NULL, /*	APDSetTextPositionTd;	*/	/* Td */
	NULL, /*	APDFontSize;	*/	/* Tf */
	NULL, /*	APDShowText;	*/	/* Tj */
	NULL, /*	APDSetTextMatrix;	*/	/* Tm */
	NULL, /*	APDSetRender;	*/	/* Tr */
	NULL, /*	APDSetRise;	*/	/* Ts */
	NULL, /*	APDSetWordSpacing;	*/	/* Tw */
	NULL, /*	APDSetHorizontalScale;	*/	/* Tz */
	NULL, /*	APDEvenOddClip;	*/	/* W* */
	NULL, /*	APDCloseEvenOddFillAndStroke;	*/	/* b* */
	NULL, /*	APDSetCTM;	*/	/* cm */
	NULL, /*	APDSetColorSpaceFill;	*/	/* cs */
	NULL, /*	APDSetType1Width;	*/	/* d0 */
	NULL, /*	APDSetType1WidthAndBound;	*/	/* d1 */
	NULL, /*	APDEvenOddFill;	*/	/* f* */
	NULL, /*	APDSetGState;	*/	/* gs */
	NULL, /*	APDRectangle;	*/	/* re */
	NULL, /*	APDSetRGBFill;	*/	/* rg */
	NULL, /*	APDSetIntent;	*/	/* ri */
	NULL, /*	APDSetSCFill;	*/	/* sc */
	NULL, /*	APDPaintShape;	*/	/* sh */
	NULL, /*	APDBeginMarkContentWithParams;	*/	/* BDC */
	NULL, /*	APDBeginMarkContent;	*/	/* BMC */
	NULL, /*	APDEndMarkContent;	*/	/* EMC */
	NULL, /*	APDSetSCNStroke;	*/	/* SCN */
	NULL, /*	APDSetSCNFill;	*/	/* scn */
} ;


void 	PDFTextOutDone ( PPDFDrawManager DrawManager )
{
	ULStreamClose ( _DATA->OutStream );
	mfree ( CD( DrawManager->Doc)->Lib, DrawManager->Data );
}

#ifdef _DOC
#	undef _DOC
#endif

#define  _DOC CD(Doc)

void	PDFTextOutInit ( PDFDocHandle Doc, PDFCosHandle Resurces, PPDFDrawManager DrawManager )
{
	PDFTextOutData * tmp;
	tmp = (PDFTextOutData *)mmalloc ( _LIB, sizeof ( PDFTextOutData ));
	DrawManager->Data = tmp;
	DrawManager->Funcs = & PDFTextOutManagerList;
	DrawManager->Doc = Doc;
	DrawManager->Resource = Resurces;
	DrawManager->Level = dlText;
	tmp->OutStream = ULStreamMemNew ( _LIB, 0 );
}




PDFStreamHandle PDFPageExtractText ( PDFDocHandle Doc, ppUns32 PageIndex )
{
	PDFCosHandle Resources, Page, Content, C1;
	TPDFDrawManager Manager;
	ppUns32 i;
	PDFStreamHandle Stream;
	ppBool UnPacked;
	Page = PDFPageGetCosObject ( Doc, PageIndex );
	Resources = _CosDictValueByName ( Page, Resources );
	Content = _CosDictValueByName ( Page, Contents );
	PDFTextOutInit ( Doc, Resources, &Manager );
	PDFTRY ( _LIB ){
		if ( CosGetType ( Content) == CosArray ){
			for ( i = 0; i < _CosArrayCount ( Content ); i++ ){
				C1 = CosArrayItem ( Content, i );
				if (! _IsCosStream ( C1 ) )
					_RAISE ( ErrCosLevel, cleInvalidObjError );
				if ( _CosStreamIsFiltered ( C1 ) ){
					Stream = ULStrmGetUnFiltered ( _LIB, C1 );
					UnPacked = true;
				} else {
					Stream = _CosStreamValue( C1 );
					UnPacked = false;
				}
				PDFTRY ( _LIB ){
		//			PDParseContent ( &Manager, Stream );
				} PDFFINALLY ( _LIB ){
					if ( UnPacked )
						ULStreamClose ( Stream );
				} PDFFINALLYEND ( _LIB );
			}
		} else 	
		if ( CosGetType (Content) == CosStream ){
			if ( _CosStreamIsFiltered ( Content ) ){
				Stream = ULStrmGetUnFiltered ( _LIB, Content );
				UnPacked = true;
			} else {
				Stream = _CosStreamValue( Content );
				UnPacked = false;
			}
			PDFTRY ( _LIB ){
		//		PDParseContent ( &Manager, Stream );
			} PDFFINALLY ( _LIB ){
				if ( UnPacked )
					ULStreamClose ( Stream );
			} PDFFINALLYEND ( _LIB );
		}
		//Stream = 
	} PDFFINALLY ( _LIB ) {
		PDFTextOutDone ( &Manager );
	} PDFFINALLYEND ( _LIB );
	return Stream;
}