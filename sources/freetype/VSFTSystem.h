/*********************************************************************

SYBREX SYSTEMS INCORPORATED
Copyright (C) 2005 Sybrex Systems Incorporated
All rights reserved.

NOTICE: Sybrex permits you to use, modify, and distribute this file
in accordance with the terms of the Sybrex license agreement
accompanying it. If you have received this file from a source other
than Sybrex, then your use, modification, or distribution of it
requires the prior written permission of Sybrex.

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
