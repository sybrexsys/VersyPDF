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
VSBaseA.h
*********************************************************************/

#ifndef VSBaseA_H
#define VSBaseA_H /* Common Types Routines Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"

/* \File Open Mode Type */
_EXP

typedef enum {
    ppFileReadMode = 0, /* Read File Mode */
    ppFileWriteMode     /* Write File Mode */
} ppFileOpenMode;

#ifdef __cplusplus
extern "C"{
#endif

/************************************************************************/
/* Atom section                                                         */
/************************************************************************/


    /* Description
       Gets atom key by String in PDF Library.
       Parameters
       Lib :     [in] PDF Library Handle.
       String :  [in] Text String Name.
       Returns
       Atom Key. Name Index in PDF Library
       See Also
       ULAtomToString                          */
    LIB_API ppAtom ULStringToAtom( PDFLibHandle Lib, char *String );



    /* Description
       Gets Text String Name by Atom Key in PDF Library.
       Parameters
       Lib :   [in] PDF Library Handle.
       Atom :  [in] Atom Key.
       Returns
       Text String Name in PDF Library
       See Also
       ULStringToAtom                                    */
    LIB_API char *ULAtomToString( PDFLibHandle Lib, ppAtom Atom );



    /* Description
       Gets atom count in PDF Library.
       Parameters
       Lib :  [in] PDF Library Handle.
       Returns
       Atom count in PDF Library.      */
    LIB_API ppInt32 ULGetAtomCount( PDFLibHandle Lib );



    /* Description
       Clears atoms in PDF Library. Gets free namespace.
       Parameters
       Lib :  [in] PDF Library Handle.
       Returns
       None.                                             */
    LIB_API void ULClearAtoms( PDFLibHandle Lib );



    /* Description
       Tests if atom exists in PDF Library for searching text
       string.
       Parameters
       Lib :     [in] PDF Library Handle.
       String :  [in] Text String Name.
       Returns
       Boolean : true - exists, false - name not found.
       See Also
       ULStringToAtom                                         */
    LIB_API ppBool ULExistsAtomForString( PDFLibHandle Lib, char *String );




/************************************************************************/
/* File section                                                         */
/************************************************************************/



    /* Description
       Sets file cursor to position ( byte offset ).
       Parameters
       FileHandle :  [in] PDF File Handle.
       Position :    [in] File offset in bytes.
       Returns
       Position which is set.
       See Also
       ULFileGetPosition                             */
    LIB_API ppUns32 ULFileSetPosition( PDFFileHandle FileHandle, ppUns32 Position );
    
    
    
    /* Description
       Reads data from file to buffer. Length is in bytes.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Buffer :      [in] Destination buffer in memory for data.
       Length :      [in] Size of read block in bytes.
       Returns
       Size of real read block in bytes.
       See Also
       ULFileWrite                                               */
    LIB_API ppUns32 ULFileRead( PDFFileHandle FileHandle, void *Buffer, ppUns32 Length );
    
    
    
    /* Description
       Writes data from buffer to file. Length is in bytes.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Buffer :      [in] Source data buffer in memory.
       Length :      [in] Size of write block in bytes.
       Returns
       Size of real write block in bytes.
       See Also
       ULFileRead                                           */
    LIB_API ppUns32 ULFileWrite( PDFFileHandle FileHandle, void *Buffer, ppUns32 Length );
    
    
    
    /* Description
       Gets file cursor position ( byte offset from beginning of the
       \file ).
       Parameters
       FileHandle :  [in] PDF File Handle.
       Returns
       \File offset in bytes.
       See Also
       ULFileSetPosition                                            */
    LIB_API ppUns32 ULFileGetPosition( PDFFileHandle FileHandle );
    
    
    
    /* Description
       Gets file size in bytes.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Returns
       \File size in bytes.                */
    LIB_API ppUns32 ULFileGetSize( PDFFileHandle FileHandle );
    
    
    
    /* Description
       Gets one character from file.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Returns
       One character form file. If it returns -1 than it is EOF (
       end of file ).                                             */
    LIB_API ppInt32 ULFileGetChar( PDFFileHandle FileHandle );
    
    
    
    /* Description
       Gets one character from file. Same as ULFileGetChar, only
       \file cursor stays on that place.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Returns
       One character form file. If it returns -1 than it is EOF (
       end of file ).                                             */
    LIB_API ppInt32 ULFileLookChar( PDFFileHandle FileHandle );
    
    
    
    /* Description
       Opens file and returns PDF File Handle.
       Parameters
       Lib :       [in] PDF Library Handle.
       FileName :  [in] Filename, text string.
       OpenMode :  [in] Open Mode \: read or write.
       Returns
       PDF File Handle.
       See Also
       ULFileClose, ppFileOpenMode                  */
    LIB_API PDFFileHandle ULFileOpen( PDFLibHandle Lib, char *FileName, ppFileOpenMode OpenMode );
    
    
    
    /* Description
       Closes PDF File.
       Parameters
       FileHandle :  [in] PDF File Handle.
       Returns
       None.
       See Also
       ULFileOpen                          */
    LIB_API void ULFileClose( PDFFileHandle FileHandle );

/************************************************************************/
/* Stream section                                                       */
/************************************************************************/



    /* Description
       Creates new Memory PDF Stream with initializing size.
       Parameters
       Lib :   [in] PDF Library Handle.
       Size :  [in] Initializing size of memory Stream in bytes.
       Returns
       Memory PDF Stream Handle.
       See Also
       ULStreamClose                                               */
    LIB_API PDFStreamHandle ULStreamMemNew( PDFLibHandle Lib, ppUns32 Size );
    
    
    
    /* Description
       Converts memory buffer to PDF Stream.
       Parameters
       Lib :     [in] PDF Library Handle.
       Buffer :  [in] Existed memory buffer.
       Length :  [in] Size of buffer in bytes.
       Returns
       Memory PDF Stream Handle.
       See Also
       ULStreamClose                             */
    LIB_API PDFStreamHandle ULStreamMemReadOnlyNew( PDFLibHandle Lib, void *Buffer, ppUns32 Length );
    
    
    
    /* Description
       Creates new file PDF Stream by filename and open mode.
       Parameters
       Lib :       [in] PDF Library Handle.
       FileName :  [in] PDF File Name, text string.
       OpenMode :  [in] Open Mode \: read or write.
       Returns
       \File PDF Stream Handle.
       See Also
       ULStreamClose                                            */
    LIB_API PDFStreamHandle ULStreamFileNew( PDFLibHandle Lib, char *FileName, ppFileOpenMode OpenMode );
    
    
	LIB_API PDFStreamHandle ULStreamCustomNew ( PDFLibHandle Lib, StreamReadBufferProc ReadBuffer, StreamWriteBufferProc WriteBuffer,
		StreamGetPositionProc GetPosition, StreamSetPositionProc SetPosition, StreamGetSizeProc GetSize,
		StreamGetCharProc GetChar, StreamLookCharProc LookChar, void * AStream );
    
    /* Description
       Creates new file PDF Stream by PDF File Handle.
       Parameters
       Lib :         [in] PDF Library Handle.
       FileHandle :  [in] PDF File Handle.
       Returns
       \File PDF Stream Handle.
       See Also
       ULStreamClose                                     */
    LIB_API PDFStreamHandle ULStreamFileHandleNew( PDFLibHandle Lib, PDFFileHandle FileHandle );
    
    
    
    /* Description
       Closes PDF Stream.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       None.
       See Also
       ULStreamMemNew, ULStreamFileNew       */
    LIB_API void ULStreamClose( PDFStreamHandle Stream );
    
    
    
    /* Description
       Sets new Stream position.
       Parameters
       Stream :       [in] PDF Stream Handle.
       NewPosition :  [in] New position in bytes ( offset from
                      beginning ).
       Returns
       Stream position after setting.
       See Also
       ULStreamGetPosition                                       */
    LIB_API ppUns32 ULStreamSetPosition( PDFStreamHandle Stream, ppUns32 NewPosition );
    
    
    
    /* Description
       Gets Stream position ( offset from start of stream ).
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       Stream position.
       See Also
       ULStreamSetPosition                                     */
    LIB_API ppUns32 ULStreamGetPosition( PDFStreamHandle Stream );
    
    
    
    /* Description
       Gets Stream size in bytes.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       Stream size in bytes.
       See Also
       ULStreamSetSize                     */
   LIB_API ppUns32 ULStreamGetSize( PDFStreamHandle Stream );
    
    
    
    /* Description
       Sets new Stream size in bytes. Enlarges stream capacity.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Size :    [in] New size of PDF Stream.
       Returns
       Stream size in bytes.
       See Also
       ULStreamGetSize                                            */
    LIB_API ppUns32 ULStreamSetSize( PDFStreamHandle Stream, ppUns32 Size );
    
    
    
    /* Description
       Reads from PDF Stream to memory buffer some count of the
       bytes.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Buffer :  [out] Memory buffer for data.
       Count :   [in] Count of bytes which we want to read from
                 Stream.
       Returns
       Count of bytes which read from Stream.
       See Also
       ULStreamWriteBuffer                                        */
    LIB_API ppUns32 ULStreamReadBuffer( PDFStreamHandle Stream, void *Buffer, ppUns32 Count );
    
    
    
    /* Description
       Writes from memory buffer to PDF Stream some count of the
       bytes.
       Parameters
       Stream :  [out] PDF Stream Handle.
       Buffer :  [in] Memory buffer with data.
       Count :   [in] Count of bytes which we want to write in
                 Stream.
       Returns
       Count of written bytes in Stream.
       See Also
       ULStreamReadBuffer                                          */
   LIB_API ppUns32 ULStreamWriteBuffer( PDFStreamHandle Stream, void *Buffer, ppUns32 Count );
    
    
    
    /* Description
       Clears PDF Stream with initializing size ( maybe zero ).
       Parameters
       Stream :  [in] PDF Stream Handle.
       Size :    [in] Initializing size of PDF Stream in bytes.
       Returns
       None.                                                    */
    LIB_API void ULStreamClear( PDFStreamHandle Stream, ppUns32 Size );
    
    
    
    /* Description
       Copies from one Stream in another Stream.
       Parameters
       FromStream :  [in] PDF Stream Handle.
       ToStream :    [in] PDF Stream Handle.
       Returns
       Size of bytes which is copied.            */
    LIB_API ppUns32 ULStreamCopyToStream( PDFStreamHandle FromStream, PDFStreamHandle ToStream );
    
    
    
    /* Description
       Reads one character from Stream.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       One character form Stream. If it returns -1 than it is EOF (
       end of stream ).
       See Also
       ULStreamWriteChar                                              */
    LIB_API ppInt32 ULStreamReadChar( PDFStreamHandle Stream );
    
    
    
    /* Description
       Reads one character from Stream. Same as ULStreamReadChar, only
       stream position stays on that place.
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       One character form Stream. If it returns -1 than it is EOF (
       end of stream ).
       See Also
       ULStreamWriteChar                                               */
    LIB_API ppInt32 ULStreamLookChar( PDFStreamHandle Stream );
    
    
    
    /* Description
       Reads one line from Stream. Line is text string to character
       EOL ( end of line )
       Parameters
       Stream :  [in] PDF Stream Handle.
       Returns
       Line from Stream. Text string terminated by zero. Must be
       free after use.                                              */
    LIB_API char *ULStreamReadLine( PDFStreamHandle Stream );
    
    
    
    /* Description
       Writes one character to Stream.
       Parameters
       Stream :     [in] PDF Stream Handle.
       Character :  [in] Writing data in size of one byte.
       Returns
       Count of written bytes in Stream.
       See Also
       ULStreamReadChar                                      */
    LIB_API ppUns32 ULStreamWriteChar( PDFStreamHandle Stream, char Character );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSBaseA_H */
