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
VSExcept.h
*********************************************************************/

#ifndef VSExcept_H
#define VSExcept_H /*  */
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#ifdef __cplusplus
#ifdef DOTNET
#using <mscorlib.dll> // added by mironichev
#endif
extern "C"{
#endif
#include <setjmp.h>
#include "VSTypes.h"

#define PDFsetjmp setjmp      /* Sets jump */
#define PDFlongjmp longjmp    /* Sets long jump in PDF Library */ 
#define PDFjmp_buf jmp_buf    /* Sets jump buffer */ 

		/********************************************************************
		Description:
		Call the exception from error code.
		Parameters:
		Lib - [ in ] Current PDF library.
		err - [ in ] Error code.
		Returns:
		None.
		*********************************************************************/
		LIB_API void PPCallException(PDFLibHandle Lib, ppInt32 err);

	   
		/********************************************************************
		Description:
		Returns code of last error.
		Parameters:
		Lib - [ in ] Current PDF library.
		Returns:
		Code of last error.
		*********************************************************************/
		LIB_API ppInt32 PPGetLastError(PDFLibHandle Lib);


#ifndef USESTDEXCEPTION
	/********************************************************************
    Description:
        Push exception the buffer.
    Parameters:
           Lib - [ in ] Current PDF library.
    Returns:
        PDF jump buffer.
    *********************************************************************/
    LIB_API PDFjmp_buf *PPPushExceptionBuffer(PDFLibHandle Lib);

	/********************************************************************
    Description:
        Pop exception code from buffer.
    Parameters:
           Lib - [ in ] Current PDF library.
    Returns:
        None.
    *********************************************************************/
    LIB_API void PPPopExceptionBuffer(PDFLibHandle Lib);


#define PDFTRY(lib) { if (!PDFsetjmp(*PPPushExceptionBuffer(lib))){ /* Begin of try expression */
#define PDFBUFRELEASE(lib) PPPopExceptionBuffer(lib)          /* Pop buffer exception into stack */
#define PDFEXCEPT(lib) PDFBUFRELEASE(lib);} else {            /* Release buffer exceptions */
#define PDFTRYEND(lib) }}                                     /* End of try expression */
#define PDFRETURN(lib,x) {PDFBUFRELEASE(lib);return x;}       /* Return value */
#define PDFRETURNVOID(lib) {PDFBUFRELEASE(lib);return;}       /* Return nothing */
#define PDFRAISE(lib,err) PPCallException(lib,(ppUns32)err);  /* Raise PDF exceptions */

#define PDFLASTERROR(lib) PPGetLastError(lib)                 /* Return last PDF error */
#define PDFRERAISE(lib) PDFRAISE(lib,PDFLASTERROR(lib))       /* Reraise PDF exceptions */
#define PDFFINALLY(lib) PDFRAISE(lib,0);}else {               /* Part of PDFTRY...PDFFINALLY...PDFFINALLYEND expression */     
#define PDFFINALLYEND(lib) if (PDFLASTERROR(lib)) PDFRERAISE(lib);}}  /* End of PDFTRY...PDFFINALLY...PDFFINALLYEND */     

#else

#define PDFTRY(lib) try{										/* Begin of try expression */
#define PDFEXCEPT(lib) } catch ( ... ){							/* Release buffer exceptions */
#define PDFTRYEND(lib) }									    /* End of try expression */
#define PDFRETURN(lib,x) return x;						        /* Return value */
#define PDFRETURNVOID(lib) return;						        /* Return nothing */
#define PDFLASTERROR(lib) PPGetLastError(lib)                   /* Return last PDF error */
#define PDFRERAISE(lib) throw                                   /* Reraise PDF exceptions */

#ifndef DOTNET 
#define PDFRAISE(lib,err) { PPCallException( lib, ( ppUns32 )err ); throw (ppUns32) err; }					/* Raise PDF exceptions */
#define PDFFINALLY(lib) PDFRAISE(lib,0);} catch ( ... ){               /* Part of PDFTRY...PDFFINALLY...PDFFINALLYEND expression */     
#define PDFFINALLYEND(lib) if (PDFLASTERROR(lib)) PDFRERAISE(lib);}  /* End of PDFTRY...PDFFINALLY...PDFFINALLYEND */     
#else
	public __gc class  PDFException: public System::Exception{
	public:
		PDFException( System::String *msg):Exception(msg){
		};


	};
#define PDFRAISE(lib,err) { \
		PPCallException( lib, ( ppUns32 )err ); \
		char ErrANSIStr[255];\
		System::String * ErrStr;\
		VSGetErrorStr( err, ErrANSIStr );\
		ErrStr = new System::String ( ErrANSIStr );\
		throw new PDFException ( ErrStr );\
	}					/* Raise PDF exceptions */
#define PDFFINALLY(lib) }__finally {               /* Part of PDFTRY...PDFFINALLY...PDFFINALLYEND expression */     
#define PDFFINALLYEND(lib) }  /* End of PDFTRY...PDFFINALLY...PDFFINALLYEND */     
#endif

#endif
#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSExcept_H */
