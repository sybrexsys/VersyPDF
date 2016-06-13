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
VSTextOut.h
*********************************************************************/

#ifndef VSTextOut_H
#define VSTextOut_H /* Base Type Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */
#include "VSDraw.h"

typedef struct PDFTextOutData{
	ppUns32 i;
	PDFStreamHandle OutStream;
}PDFTextOutData, *PPDFTextOutData;


#ifdef __cplusplus
extern "C"{
#endif
void	PDFTextOutInit ( PDFDocHandle Doc, PDFCosHandle Resurces, PPDFDrawManager DrawManager );
void 	PDFTextOutDone ( PPDFDrawManager DrawManager );

PDFStreamHandle PDFPageExtractText ( PDFDocHandle Doc, ppUns32 PageIndex );

#ifdef __cplusplus
}
#endif


#endif /* #ifndef VSTextOut_H */
