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
VSFTSystem.h
*********************************************************************/

#ifndef _VSFTSYSTEM_H 
#define _VSFTSYSTEM_H

#include	"ft2build.h"
#include	"freetype.h"
#include "VSType1.h"

#ifdef __cplusplus
extern "C"{
#endif


    /********************************************************************
    Description:
        Memory allocation function for FreeType library
    Parameters: 
        FT_Memory memory		- [in] Memmory handler pointer.
        long size      			- [in] Size in bytes to allocate.
    Returns:
        Pointer to allocated memory.
    *********************************************************************/
	void*  vspdf_alloc ( FT_Memory memory, long  size );

    /********************************************************************
    Description:
        Memory reallocation function for FreeType library
    Parameters: 
        FT_Memory memory		- [in] Memmory handler pointer.
        long cur_size			- [in] current memory block size.
		long new_size			- [in] new memory block size.
		void * block			- [in] old memory block pointer.         
    Returns:
        Pointer to allocated memory.
    *********************************************************************/
	void*  vspdf_realloc ( FT_Memory  memory, long cur_size, long new_size, void* block );

    /********************************************************************
    Description:
        Memory free function for FreeType library
    Parameters: 
        FT_Memory memory		- [in] Memmory handler pointer.
		void * block			- [in] old memory block pointer.         
    Returns:
       None
    *********************************************************************/
	void vspdf_free ( FT_Memory  memory, void* block );

    /********************************************************************
    Description:
        Create new FreeType memory handler
    Parameters: 
		PDFLibHandle Lib		- [in] PDF Library handle.
    Returns:
       Pointer to new FreeType Memory handler
    *********************************************************************/
	FT_Memory  VSPDF_New_Memory ( PDFLibHandle Lib );

    /********************************************************************
    Description:
        Destroy FreeType memory handler
    Parameters: 
		FT_Memory memory		- [in] Pointer to FreeType memory handler.
    Returns:
       Pointer to new FreeType Memory handler
    *********************************************************************/
	void  VSPDF_Done_Memory ( FT_Memory  memory );
	
    /********************************************************************
    Description:
        Init FreeType Library handler
    Parameters: 
		PDFLibHandle			- [in] PDF Library handle.
		FT_Library * alibrary	- [out] Pointer to new FreeType library handle
    Returns:
       Error code
    *********************************************************************/
	FT_Error  VSPDF_Init_FreeType ( PDFLibHandle Lib, FT_Library  *alibrary );

    /********************************************************************
    Description:
        Destroy FreeType Library handler
    Parameters: 
		FT_Library library		- [in] FreeType library handle
    Returns:
       Error code
    *********************************************************************/
	FT_Error  VSPDF_Done_FreeType( FT_Library  library );
	
#ifdef __cplusplus
}
#endif
	
#endif //_VSFTSYSTEM_H
