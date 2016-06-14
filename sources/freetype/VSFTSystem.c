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
VSFTSystem.c
*********************************************************************/

#include "VSFreeType.h"
#include "ft2build.h"
#include "ftconfig.h"

#include FT_INTERNAL_DEBUG_H
#include FT_SYSTEM_H
#include FT_ERRORS_H
#include FT_TYPES_H
#include FT_INTERNAL_OBJECTS_H

#include "VSTypes.h"
#include "VSLibI.h"



void*  vspdf_alloc( FT_Memory  memory, long       size )
{
	PDFLibHandle	Lib = ( PDFLibHandle )memory->user;
	return ( mmalloc( Lib, size ));
}


void*  vspdf_realloc( FT_Memory  memory, long cur_size, long new_size, void* block )  
{
	PDFLibHandle	Lib = ( PDFLibHandle )memory->user;
    FT_UNUSED( cur_size );

    return (mrealloc( Lib, block, new_size ));
}

void vspdf_free( FT_Memory  memory, void*      block )
{
	 PDFLibHandle Lib = ( PDFLibHandle )memory->user;	
     mfree( Lib, block );
}
  
FT_Memory  VSPDF_New_Memory( PDFLibHandle Lib )  
{
    FT_Memory  memory;


    memory = (FT_Memory)mmalloc( Lib, sizeof ( *memory ) );
    if ( memory )
    {
      memory->user    = ( void *)Lib;
      memory->alloc   = vspdf_alloc;
      memory->realloc = vspdf_realloc;
      memory->free    = vspdf_free;
    }

    return memory;
}


  /* documentation is in ftobjs.h */

void  VSPDF_Done_Memory( FT_Memory  memory )
{
    PDFLibHandle	Lib = (PDFLibHandle)memory->user;
    mfree(  Lib, memory );
}

FT_Error  VSPDF_Init_FreeType( PDFLibHandle Lib, FT_Library  *alibrary )
{
    FT_Error   error;
    FT_Memory  memory;


    /* First of all, allocate a new system object -- this function is part */
    /* of the system-specific component, i.e. `ftsystem.c'.                */

    memory = VSPDF_New_Memory( Lib );
    if ( !memory )
    {
      FT_ERROR(( "FT_Init_FreeType: cannot find memory manager\n" ));
      return FT_Err_Unimplemented_Feature;
    }

    /* build a library out of it, then fill it with the set of */
    /* default drivers.                                        */

    error = FT_New_Library( memory, alibrary );
    if ( !error )
    {
      (*alibrary)->version_major = FREETYPE_MAJOR;
      (*alibrary)->version_minor = FREETYPE_MINOR;
      (*alibrary)->version_patch = FREETYPE_PATCH;

      FT_Add_Default_Modules( *alibrary );
    }

    return error;
}

 
FT_Error  VSPDF_Done_FreeType( FT_Library  library )
{
    if ( library )
    {
      FT_Memory  memory = library->memory;

      /* Discard the library object */
      FT_Done_Library( library );

      /* discard memory manager */
      VSPDF_Done_Memory( memory );
    }

    return FT_Err_Ok;
}

  