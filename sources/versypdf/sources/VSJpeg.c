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
VSJpeg.c
*********************************************************************/
#include "VSImageI.h"


#include "VSDocAI.h"

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "jmemsys.h"		/* import the system-dependent declarations */

#define _DOC CD ( Doc )
#define _LIB _DOC->Lib

void PDFJPEGerror_exit (j_common_ptr cinfo){
    jpeg_abort(cinfo);
    PDFRAISE ( (PDFLibHandle)cinfo->client_data, PDFBuildErrCode (ErrGeneralLevel, gleLoadJPEGFileError ) );
}

void PDFJPEGemit_message (j_common_ptr cinfo, int msg_level){
}

void PDFJPEGfill_input_buffer (j_decompress_ptr cinfo){
    
}

void PDFJPEGskip_input_data (j_decompress_ptr cinfo, long num_bytes){

}
void PDFJPEGresync_to_restart (j_decompress_ptr cinfo, int desired){

}
void PDFImageCompressJPEG ( PMImage Image, PDFStreamHandle Stream, ppUns8 Quality )
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	ppUns8 * scline;
	JSAMPARRAY buffer;
	cinfo.err = jpeg_std_error ( &jerr );
    cinfo.err->error_exit = PDFJPEGerror_exit;
    cinfo.err->emit_message = PDFJPEGemit_message;
    cinfo.client_data = _StreamLib ( Stream );
	jpeg_create_compress ( &cinfo ) ;
    cinfo.add_data = Stream;
	jpeg_stdio_dest ( &cinfo, NULL );
	cinfo.image_width = Image->Width;
	cinfo.image_height = Image->Height; 
	cinfo.input_components = Image->Device;
	switch ( Image->Device ){
		case 1:
			cinfo.in_color_space = JCS_GRAYSCALE;
			break;
		case 3:
			cinfo.in_color_space = JCS_RGB;
			break;
		case 4:
			cinfo.in_color_space = JCS_CMYK;
			break;
	}
	jpeg_set_defaults ( &cinfo );
	jpeg_set_quality (&cinfo, ( int )Quality, false );
	jpeg_start_compress ( &cinfo, true );   
	while ( cinfo.next_scanline < cinfo.image_height ){	
		scline = ScanLine ( Image, cinfo.next_scanline );
		buffer = &scline;
		jpeg_write_scanlines ( &cinfo, buffer, 1, NULL );
	}
	jpeg_finish_compress ( &cinfo, NULL ); 
	jpeg_destroy_compress ( &cinfo );
}


PMImage PDFImageLoadJPEGFromStream( PDFStreamHandle Stream ){
    PMImage Tmp;
    TPDFColorDevice CurrentCG = cgRGB;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    ppInt32 i;
    ppUns8 * SL;
    int row_stride;
    PDFLibHandle Lib = _StreamLib( Stream );
    cinfo.err = jpeg_std_error ( &jerr );                    
    cinfo.err->error_exit = PDFJPEGerror_exit;
    cinfo.err->emit_message = PDFJPEGemit_message;
    cinfo.client_data = Lib;
    jpeg_create_decompress ( &cinfo );
    PDFTRY ( Lib ){
        cinfo.add_data = Stream;
        //if ( ( hFile= fopen( filename, "rb" ) )==NULL )  exit( 1 );
        jpeg_stdio_src ( &cinfo, NULL, 0 );
        jpeg_read_header ( &cinfo, true, NULL );
        jpeg_start_decompress ( &cinfo );
        PDFTRY ( Lib ) {
            switch ( cinfo.out_color_space ){
                case JCS_GRAYSCALE:
                    CurrentCG = cgGray;
                    break;
                case JCS_RGB:
                    CurrentCG = cgRGB;
                    break;
                case JCS_CMYK:
                    CurrentCG = cgCMYK;
                    break;
                default:
                    PDFRAISE ( Lib, PDFBuildErrCode (ErrGeneralLevel, gleLoadJPEGFileError ) );
            }
            Tmp = ( PMImage )PDFImageCreate ( Lib, cinfo.image_width, cinfo.image_height, it8bit, CurrentCG );
            PDFTRY ( Lib ){
                row_stride = cinfo.output_width * CurrentCG;
                buffer = ( *cinfo.mem->alloc_sarray )( ( j_common_ptr ) &cinfo, JPOOL_IMAGE, row_stride, 1 );
                while ( cinfo.output_scanline < cinfo.output_height ){
                    jpeg_read_scanlines ( &cinfo, buffer, 1, NULL );
                    i = cinfo.output_scanline - 1;
                    SL = ScanLine ( Tmp, i );
                    memcpy ( SL,buffer[0], Tmp->LineSize );
                    if ( cinfo.out_color_space == JCS_CMYK ){                      
                        for ( i = 0; i< row_stride; i ++)
                            SL [ i ] = 255 - SL [ i ];
                    }
                }
            } PDFEXCEPT ( Lib ) {
                PDFImageFree( Tmp );
                PDFRERAISE ( Lib );
            } PDFTRYEND ( Lib );
        } PDFFINALLY ( Lib ) {
            jpeg_finish_decompress(&cinfo);
        } PDFFINALLYEND ( Lib );
    } PDFFINALLY ( Lib ) {
        jpeg_destroy_decompress(&cinfo);
    } PDFFINALLYEND ( Lib );
    return Tmp;
}


/*
* Memory allocation and freeing are controlled by the regular library
* routines malloc() and free().
*/

GLOBAL(void *)
jpeg_get_small (j_common_ptr cinfo, size_t sizeofobject)
{
    return cinfo->client_data ? (void *) mmalloc( (PDFLibHandle)cinfo->client_data, sizeofobject): 
        (void *) malloc( sizeofobject);
}

GLOBAL(void)
jpeg_free_small (j_common_ptr cinfo, void * object, size_t sizeofobject)
{
    if ( cinfo->client_data ){
        mfree((PDFLibHandle) cinfo->client_data, object);
    } else {
        free ( object );
    }
            
    
}


/*
* "Large" objects are treated the same as "small" ones.
* NB: although we include FAR keywords in the routine declarations,
* this file won't actually work in 80x86 small/medium model; at least,
* you probably won't be able to process useful-size images in only 64KB.
*/

GLOBAL(void FAR *)
jpeg_get_large (j_common_ptr cinfo, size_t sizeofobject)
{
    return cinfo->client_data ? (void FAR *) mmalloc( (PDFLibHandle)cinfo->client_data, sizeofobject): 
        ( void FAR * ) malloc( sizeofobject);
}

GLOBAL(void)
jpeg_free_large (j_common_ptr cinfo, void FAR * object, size_t sizeofobject)
{
    if ( cinfo->client_data ){
        mfree( (PDFLibHandle)cinfo->client_data, object);
    } else {
        free ( object );
    }
}


/*
* This routine computes the total memory space available for allocation.
* Here we always say, "we got all you want bud!"
*/

GLOBAL(long)
jpeg_mem_available (j_common_ptr cinfo, long min_bytes_needed,
                    long max_bytes_needed, long already_allocated)
{
    return max_bytes_needed;
}


/*
* Backing store (temporary file) management.
* Since jpeg_mem_available always promised the moon,
* this should never be called and we can just error out.
*/

GLOBAL(void)
jpeg_open_backing_store (j_common_ptr cinfo, backing_store_ptr info,
                         long total_bytes_needed)
{
    ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}


/*
* These routines take care of any system-dependent initialization and
* cleanup required.  Here, there isn't any.
*/

GLOBAL(long)
jpeg_mem_init (j_common_ptr cinfo)
{
    return 0;			/* just set max_memory_to_use to 0 */
}

GLOBAL(void)
jpeg_mem_term (j_common_ptr cinfo)
{
    /* no work */
}


/* Expanded data source object for stdio input */

typedef struct {
    struct jpeg_source_mgr pub;	/* public fields */

    FILE * infile;		/* source stream */
    JOCTET * buffer;		/* start of buffer */
    boolean start_of_file;	/* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */


/*
* Initialize source --- called by jpeg_read_header
* before any data is actually read.
*/

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    /* We reset the empty-input-file flag for each image,
    * but we don't clear the input buffer.
    * This is correct behavior for reading a series of images from one source.
    */
    src->start_of_file = true;
}


METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo, j_stream_ptr from_memory)
{
    char * tmp_src;
    my_src_ptr src = (my_src_ptr) cinfo->src;
    size_t nbytes;
    
    if ( cinfo->add_data ){
        nbytes = ULStreamReadBuffer( ( PDFStreamHandle )cinfo->add_data, src->buffer, INPUT_BUF_SIZE );
    } else {
        if (from_memory == NULL)
        {
            nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
        }
        else
        {
            tmp_src = from_memory->buffer + from_memory->offset;
            if ((from_memory->offset + INPUT_BUF_SIZE)<= from_memory->size)
            {
                memcpy(src->buffer, tmp_src, INPUT_BUF_SIZE);
                nbytes = INPUT_BUF_SIZE;
            }
            else
            {
                nbytes = from_memory->size - from_memory->offset;
                memcpy(src->buffer, tmp_src, nbytes);
            }
            from_memory->s_pointer = tmp_src + nbytes;
            from_memory->offset = (int)(from_memory->offset + nbytes);        
        }  
    }

    if (nbytes <= 0) {
        if (src->start_of_file)	/* Treat empty input file as fatal error */
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        WARNMS(cinfo, JWRN_JPEG_EOF);
        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;

    return true;
}


/*
* Skip data --- used to skip over a potentially large amount of
* uninteresting data (such as an APPn marker).
*
* Writers of suspendable-input applications must note that skip_input_data
* is not granted the right to give a suspension return.  If the skip extends
* beyond the data currently in the buffer, the buffer can be marked empty so
* that the next read will cause a fill_input_buffer call that can suspend.
* Arranging for additional bytes to be discarded before reloading the input
* buffer is the application writer's problem.
*/

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes, j_stream_ptr from_memory)
{
    my_src_ptr src = (my_src_ptr) cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
    * it doesn't work on pipes.  Not clear that being smart is worth
    * any trouble anyway --- large skips are infrequent.
    */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->pub.bytes_in_buffer) {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) fill_input_buffer(cinfo, from_memory);
            /* note we assume that fill_input_buffer will never return FALSE,
            * so suspension need not be handled.
            */
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}


/*
* An additional method that can be provided by data source modules is the
* resync_to_restart method for error recovery in the presence of RST markers.
* For the moment, this source module just uses the default resync method
* provided by the JPEG library.  That method assumes that no backtracking
* is possible.
*/


/*
* Terminate source --- called by jpeg_finish_decompress
* after all data has been read.  Often a no-op.
*
* NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
* application must deal with any cleanup that should happen even
* for error exit.
*/

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
    /* no work necessary here */
}


/*
* Prepare for input from a stdio stream.
* The caller must have already opened the stream, and is responsible
* for closing it after finishing decompression.
*/

GLOBAL(void)
jpeg_stdio_src (j_decompress_ptr cinfo, FILE * infile, int from_file)
{
    my_src_ptr src;

    /* 
    The source object and input buffer are made permanent so that a series
    of JPEG images can be read from the same file by calling jpeg_stdio_src
    only before the first one.  (If we discarded the buffer at the end of
    one image, we'd likely lose the start of the next one.)
    This makes it unsafe to use this manager and a different source
    manager serially with the same JPEG object.  Caveat programmer.
    */
    if (cinfo->src == NULL) 
    {	/* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, SIZEOF(my_source_mgr));

        src = (my_src_ptr) cinfo->src;

        src->buffer = (JOCTET *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, INPUT_BUF_SIZE * SIZEOF(JOCTET));
    }

    src = (my_src_ptr) cinfo->src;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = term_source;
    if (from_file != 0)
        src->infile = infile;
    src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = NULL; /* until buffer loaded */
}

/* Expanded data destination object for stdio output */

typedef struct {
    struct jpeg_destination_mgr pub; /* public fields */

    FILE * outfile;		/* target stream */
    JOCTET * buffer;		/* start of buffer */
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */


/*
* Initialize destination --- called by jpeg_start_compress
* before any data is actually written.
*/

METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

    /* Allocate the output buffer --- it will be released when done with image */
    dest->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
        OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
* Empty the output buffer --- called whenever buffer fills up.
*
* In typical applications, this should write the entire output buffer
* (ignoring the current state of next_output_byte & free_in_buffer),
* reset the pointer & count to the start of the buffer, and return true
* indicating that the buffer has been dumped.
*
* In applications that need to be able to suspend compression due to output
* overrun, a FALSE return indicates that the buffer cannot be emptied now.
* In this situation, the compressor will return to its caller (possibly with
* an indication that it has not accepted all the supplied scanlines).  The
* application should resume compression after it has made more room in the
* output buffer.  Note that there are substantial restrictions on the use of
* suspension --- see the documentation.
*
* When suspending, the compressor will back up to a convenient restart point
* (typically the start of the current MCU). next_output_byte & free_in_buffer
* indicate where the restart point will be if the current call returns FALSE.
* Data beyond this point will be regenerated after resumption, so do not
* write it out when emptying the buffer externally.
*/

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo, j_stream_ptr mem_stream)
{
    char * tmp_ptr;
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
    if ( cinfo->add_data ){
        if ( ULStreamWriteBuffer (( PDFStreamHandle ) cinfo->add_data , dest->buffer, OUTPUT_BUF_SIZE ) != OUTPUT_BUF_SIZE)
            ERREXIT(cinfo, JERR_FILE_WRITE);
    } else {
        if (mem_stream == NULL)
        {
            if (JFWRITE(dest->outfile, dest->buffer, OUTPUT_BUF_SIZE) !=
                (size_t) OUTPUT_BUF_SIZE)
                ERREXIT(cinfo, JERR_FILE_WRITE);
        }
        else
        {
            mem_stream->buffer = (char *)realloc((char *)(mem_stream->buffer), mem_stream->size + OUTPUT_BUF_SIZE);
            tmp_ptr = mem_stream->buffer + mem_stream->size;
            memcpy(tmp_ptr, dest->buffer, OUTPUT_BUF_SIZE);
            mem_stream->size = mem_stream->size + OUTPUT_BUF_SIZE;
            mem_stream->offset = mem_stream->size;
            mem_stream->s_pointer = tmp_ptr;
        }

    }
    
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

    return true;
}


/*
* Terminate destination --- called by jpeg_finish_compress
* after all data has been written.  Usually needs to flush buffer.
*
* NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
* application must deal with any cleanup that should happen even
* for error exit.
*/

METHODDEF(void)
term_destination (j_compress_ptr cinfo, j_stream_ptr mem_stream)
{
    char * tmp_ptr;
    my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
    size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

    /* Write any data remaining in the buffer */
    if ( cinfo->add_data ){
        if ( ULStreamWriteBuffer (( PDFStreamHandle ) cinfo->add_data , dest->buffer, (ppUns32)datacount ) != datacount)
            ERREXIT(cinfo, JERR_FILE_WRITE);
    } else {
        if (mem_stream == NULL)
        {
            if (datacount > 0) 
            {
                if (JFWRITE(dest->outfile, dest->buffer, datacount) != datacount)
                    ERREXIT(cinfo, JERR_FILE_WRITE);
            }
            fflush(dest->outfile);
            /* Make sure we wrote the output file OK */
            if (ferror(dest->outfile))
                ERREXIT(cinfo, JERR_FILE_WRITE);
        }
        else
        {
            if (datacount > 0) 
            {
                mem_stream->buffer = (char *)realloc((char *)mem_stream->buffer, mem_stream->size + datacount);
                tmp_ptr = mem_stream->buffer + mem_stream->size;
                memcpy(tmp_ptr, dest->buffer, datacount);
                mem_stream->size = (int)(mem_stream->size + datacount);
                mem_stream->offset = mem_stream->size;
                mem_stream->s_pointer = tmp_ptr;
            }

        }
    }
}


/*
* Prepare for output to a stdio stream.
* The caller must have already opened the stream, and is responsible
* for closing it after finishing compression.
*/

GLOBAL(void)
jpeg_stdio_dest (j_compress_ptr cinfo, FILE * outfile)
{
    my_dest_ptr dest;

    /* The destination object is made permanent so that multiple JPEG images
    * can be written to the same file without re-executing jpeg_stdio_dest.
    * This makes it dangerous to use this manager and a different destination
    * manager serially with the same JPEG object, because their private object
    * sizes may be different.  Caveat programmer.
    */

    if (cinfo->dest == NULL) 	/* first time for this JPEG object? */
    {
        cinfo->dest = (struct jpeg_destination_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            SIZEOF(my_destination_mgr));
    }

    dest = (my_dest_ptr) cinfo->dest;
    dest->pub.init_destination = init_destination;
    dest->pub.empty_output_buffer = empty_output_buffer;
    dest->pub.term_destination = term_destination;
    dest->outfile = outfile;
}




