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
VSExcept.c
*********************************************************************/

#include "VSPlatform.h"



#ifdef MACOSX_PLATFORM
# define _MAC
#endif

#include <stdio.h>


#include "VSAssert.h"
#include "VSLibI.h"
#include "VSMisc.h"

#ifdef _LIB
#undef _LIB
#endif

#define _LIB (( PPDFLib ) Lib)

void PPCallException(PDFLibHandle Lib, ppInt32 err){
	_LIB->LastError=err;
#ifndef USESTDEXCEPTION

#ifdef PDF_EXCEPTION_DEBUG
	assert(!(err & PDF_ERR_ERROR_MASK)); 
#endif

	if (_LIB->ExcCount){
		PDFlongjmp(_LIB->ExcBuffer[--_LIB->ExcCount],1);
	} else {
		if ( _LIB->errorproc ){
			_LIB->errorproc ( Lib, err, _LIB->opaque );
		} else {
#ifdef PDFERROUT
			fprintf(stderr, "PDF Exception stack underflow\n");
#endif
			exit(1);
		}
	}
#endif
}

ppInt32 PPGetLastError(PDFLibHandle Lib){
	if (!Lib) return 0;
	return _LIB->LastError;
}

#ifndef USESTDEXCEPTION


PDFjmp_buf *PPPushExceptionBuffer(PDFLibHandle Lib){
	if (_LIB->ExcCount==_LIB->ExcCapacity){
		_LIB->ExcCapacity += CalculateDelta(_LIB->ExcCapacity);
		_LIB->ExcBuffer = ( PDFjmp_buf * ) mrealloc(Lib,_LIB->ExcBuffer,sizeof(PDFjmp_buf)*_LIB->ExcCapacity);
	};
	return &(_LIB->ExcBuffer[_LIB->ExcCount++]);
}

void PPPopExceptionBuffer(PDFLibHandle Lib){
	if (_LIB->ExcCount) _LIB->ExcCount--;
}




#endif