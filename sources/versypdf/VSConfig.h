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
VSConfig.h
*********************************************************************/

#ifndef VSConfig_H
#define VSConfig_H  /*  */

#ifdef  __cplusplus
extern "C"{
#endif



/* #define CANVASDBG      */       /* Defining debugging*/
/* #define LOGDBG         */       /* Defining logging*/




#define COSOBJISPOINTER 



#ifdef CANVASDBG
#	define CDebugCode( code ) { code }          /* Defining long debugging*/
#else
#	define CDebugCode( code )                   /* Defining short debugging*/
#endif

#ifdef CANVASDBG
#	define CDebugHeader( code ) code            /* Defining long head debugging*/    
#else
#	define CDebugHeader( code )                 /* Defining short debugging*/
#endif

#ifdef LOGDBG
#	define LOGCode( code ) { code }             /* Defining long logging */
#else
#	define LOGCode( code )                      /* Defining short logging */ 
#endif




/* #define NOT_USE_TIFF */


/* #define NOT_USE_FREETYPE */


/* #define NOT_USE_PNG */


/* #define NOT_USE_EMF */


/* #define NOT_USE_SIGN */


#ifdef  __cplusplus
}
#endif

#endif /* #ifndef VSConfig_H */

