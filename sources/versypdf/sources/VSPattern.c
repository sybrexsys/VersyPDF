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
VSCanvasA.c
*********************************************************************/

#include <math.h>  
#include <string.h>

#include "../VSCanvasA.h"
#include "VSCanvasAI.h"

#include "../VSBaseA.h"
#include "VSCosAI.h"
#include "VSDocAI.h"
#include "VSFilter.h"
#include "VSStdFont.h"
#include "VSAssert.h"
#include "VSFontAI.h"


#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib



PDFCosHandle PDFDocAddPattern( PDFDocHandle Doc, ppUns32 Width, ppUns32 Height, ppUns32 XStep, ppUns32 YStep )
{
	PDFCosHandle Obj, wrk;
	
	Obj = CosStreamNew (Doc, true, 5);
	_CosDictAppend(Obj,Type, _CosNameNew(Pattern) );
	_CosDictAppend(Obj,PatternType, CosIntNew(Doc,false,1) );
	_CosDictAppend(Obj,PaintType, CosIntNew(Doc,false,1) );
	_CosDictAppend(Obj,TilingType, CosIntNew(Doc,false,1) );
	_CosDictAppend(Obj,XStep, CosIntNew(Doc,false,XStep) );
	_CosDictAppend(Obj,YStep, CosIntNew(Doc,false,YStep) );
	_CosDictAppend(Obj, Matrix, wrk= CosArrayNew (Doc, false,6));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 1));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 1));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	_CosDictAppend(Obj, BBox, wrk= CosArrayNew (Doc, false,4));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Width));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Height));
	_CosDictAppend (Obj, Resources, CosDictNew(Doc,false,0));
	return Obj;
}




PBXHandle PDFPatternCreateContent( PDFCosHandle Pattern, ppUns32 Resolution )
{
	PDFDocHandle Doc;
	PDFCosHandle ItemObject, CoordinateObject;
	PPBX         bx;
	TPDFRect     rect;

	Doc = _CosDoc(Pattern);

	bx = (PPBX)mmalloc ( _LIB, sizeof ( TPBX ) );
	bx->Doc = Doc;
	bx->Res = _CosDictValueByName( Pattern, Resources );
	bx->cs  = Pattern;
	bx->FS = 1.0;
	bx->Strm = CosStreamGetValue ( Pattern );
	bx->Resolution = Resolution;
	ItemObject = _CosDictValueByName( Pattern, BBox );
	if ( _IsCosArray( ItemObject ) ) {
		CoordinateObject = CosArrayItem( ItemObject, 0 );
		rect.xl  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 1 );
		rect.yl  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 2 );
		rect.xr  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 3 );
		rect.yr  = CosRealGetValue( CoordinateObject );
		bx->Width  = rect.xr - rect.xl;
		bx->Height = rect.yr - rect.yl;
	}
	bx->BaseLine = false;
	bx->E2I = 72.0 / (ppReal) Resolution;
	bx->I2E = ( ppReal ) Resolution / 72.0;
	bx->CharSpace = 0.0;
	bx->WordSpace = 0.0;
	bx->HorizontalScaling = 100.0;
	bx->CurrentFontIndex = 0;
	bx->CurrentFontSize = 10.0;
	bx->TextInited = false;
	bx->TextUsed = false;
	bx->GrayUsed = false;
	bx->ColorUsed = false;
	bx->TextAngle = 0.0;
	bx->UnderLine = false;
	bx->StrikeOut = false;
	bx->FontInited = false;
	bx->Unicode = false;
	bx->CurrentFontIndex = ppMAXUns32;
	bx->TY = 0.0;
	bx->TX = 0.0;
	bx->ResourceFontIndex = 0;
	bx->ResourceImageIndex = 0;
	bx->ResourcePatternIndex = 0;
	bx->FontIsChanged = true;
	bx->SaveCount = 0;
	bx->WT.YScale = 1.0;
	bx->WT.XScale = 1.0;
	bx->NextOperatorChar = 10;

	bx->Rotate = pra0;
	PBXSetAngle( bx, bx->Rotate );
	CDebugCode (
		bx->DBG = NULL;
	);
	return ( PBXHandle ) bx;
}
