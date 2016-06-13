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
VSThreadA.h
*********************************************************************/

#ifndef VSThreadA_H
#define VSThreadA_H /* PDF Document's Threads Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSDocA.h"

#ifdef __cplusplus
extern "C"{
#endif


    /* Description
       Gets Threads Count in PDF Document.
       Parameters
       Doc :  [in] PDF Document Handle.
       Returns
       Number of Threads Items.            */
    LIB_API ppInt32 PDFDocGetThreadCount( PDFDocHandle Doc );


    /* Description
       Gets Thread of PDF Document according to Index.
       Parameters
       Doc :    [in] PDF Document Handle.
       Index :  [in] Index of Thread in PDF Document.
       Returns
       Thread Handle.
       See Also
       PDFThreadHandle                                 */
    LIB_API PDFThreadHandle PDFDocGetThread( PDFDocHandle Doc, ppInt32 Index );


    /* Description
       Creates New Thread in PDF Document.
       Parameters
       Doc :    [in] PDF Document Handle.
       Returns
       New Thread Handle in PDF Document.
       See Also
       PDFThreadDelete                                */
    LIB_API PDFThreadHandle PDFThreadNew( PDFDocHandle Doc );


    /* Description
       Deletes Thread from PDF Document.
       Parameters
       Doc :     [in] PDF Document Handle.
       Thread :  [in] Thread Handle in PDF Document for Deleting.
       Returns
       None.
       See Also
       PDFThreadNew                                               */
    LIB_API void PDFThreadDelete( PDFThreadHandle Thread );



    /* Description
       Gets information from Thread according to Name.
       Parameters
       Doc :        [in] PDF Document Handle.
       Thread :     [in] Thread Handle in PDF Document.
       InfoKey :    [in] Name of requesting information.
       Value :      [out] Text value of requesting information.
       MaxLength :  [in] Maximum length of value in bytes.
       Returns
       Length of value in bytes ( not longer than MaxLength ).
       See Also
       PDFThreadSetInfo                                         */
    LIB_API char * PDFThreadGetInfo( PDFThreadHandle Thread, TPDFInformation Info,
                              ppUns32 *StrLen );


    /* Description
       Sets information to Thread according to property name.
       Parameters
       Doc :      [in] PDF Document Handle.
       Thread :   [in] Thread Handle in PDF Document.
       InfoKey :  [in] Name of setting information.
       Value :    [in] Text string information value.
       Length :   [in] Length of value in bytes.
       Returns
       None.
       See Also
       PDFThreadGetInfo                                       */
    LIB_API void PDFThreadSetInfo( PDFThreadHandle Thread, TPDFInformation Info,
                           char *Value, ppUns32 Length );

	/* Description
	Gets Bead Handle on First Bead in Thread.
	Parameters
	Doc :     [in] PDF Document Handle.
	Thread :  [in] Thread Handle in PDF Document for founding the
	first bead.
	Returns
	Bead Handle on the First Bead in Thread.
	See Also
	PDFThreadSetFirstBead                                         */
	LIB_API PDFBeadHandle PDFThreadGetFirstBead( PDFThreadHandle Thread );


    /* Description
       Creates new bead on the page for Thread.
       Parameters
       Thread :   [in] Thread Handle.
       Page :  [in] Index of Page on which this bead will be
               appeared.
       Rect :  [in] A rectangle specifying the location of this bead
               on the page.
       Returns
       Bead Handle.
    */
    LIB_API PDFBeadHandle PDFThreadAppendBead( PDFThreadHandle Thread, ppUns32 Page, TPDFRect Rect );


    /* Description
       Navigates to the next bead item.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead whence we go onward.
       Returns
       Handle of next bead.
       See Also
       PDFBeadGetPrev                         */
    LIB_API PDFBeadHandle PDFBeadGetNext( PDFBeadHandle Bead );


    /* Description
       Navigates to previous bead item.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead whence we go back.
       Returns
       Handle of previous bead.
       See Also
       PDFBeadGetNext                       */
    LIB_API PDFBeadHandle PDFBeadGetPrev( PDFBeadHandle Bead );


    
    /* Description
       Gets a rectangle specifying the location of this bead on the
       page.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead Handle.
       Rect :  [out] Rectangle specifying the location of this bead
               on the page.
       Returns
       None.
       See Also
       PDFBeadSetRect                                               */
    LIB_API void PDFBeadGetRect( PDFBeadHandle Bead, TPDFRect *Rect );


    /* Description
       Gets an Index of Page on which this bead appears.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead Handle.
       Returns
       Index of Page on which this bead appears.         */
    LIB_API ppInt32 PDFBeadGetPage( PDFBeadHandle Bead );


    /* Description
       Gets a Handle of Bead's Thread.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead Handle.
       Returns
       Handle of Bead's Thread.          */
    LIB_API PDFThreadHandle PDFBeadGetThread( PDFBeadHandle Bead );


    /* Description
       Sets a rectangle specifying the location of this bead on the
       page.
       Parameters
       Doc :   [in] PDF Document Handle.
       Bead :  [in] Bead Handle.
       Rect :  [out] Rectangle specifying the location of this bead
               on the page.
       Returns
       None.
       See Also
       PDFBeadGetRect                                               */
    LIB_API void PDFBeadSetRect( PDFBeadHandle Bead, TPDFRect Rect );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSThreadA_H */
