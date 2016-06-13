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
VSPageA.h
*********************************************************************/

#ifndef VSPageA_H
#define VSPageA_H /* Page API routines module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */ 

#include "VSTypes.h"

/* Page Rotation Angle. The number of degrees by which the page should
be rotated clockwise when displayed or printed. The value must be a multiple
of 90. Default value: 0. */
_EXP
typedef enum {
    pra0 = 0,   /*   0 deg. - rotation angle */
    pra90,      /*  90 deg. - rotation angle */
    pra180,     /* 180 deg. - rotation angle */
    pra270      /* 270 deg. - rotation angle */
} TPDFPageRotateAngle;

/* Page Box Type */
_EXP
typedef enum {
    pbnMediaBox, /* A rectangle, expressed in default user space units, defining the boundaries
                 of the physical medium on which the page is intended to be displayed or printed */
    pbnCropBox,  /* A rectangle, expressed in default user space units, defining the visible region
                 of default user space. When the page is displayed or printed, its contents are to
                 be clipped (cropped) to this rectangle and then imposed on the output medium in
                 some implementation defined manner. */
    pbnBleedBox, /* A rectangle, expressed in default user space units, defining the region to which
                 the contents of the page should be clipped when output in a production environment */
    pbnTrimBox,  /* A rectangle, expressed in default user space units, defining the intended
                 dimensions of the finished page after trimming */
    pbnArtBox    /* A rectangle, expressed in default user space units, defining the extent of the
                 page’s meaningful content (including potential white space) as intended by the
                 page’s creator */
} TPDFPageBoxType;

#ifdef __cplusplus
    extern "C"{
#endif


    /************************************************************************
    Description :
        Get Page Content Streams Count.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
    Returns :
        Page Content Streams Count.
    ************************************************************************/
    LIB_API ppUns32 PDFPageGetContentCount( PDFDocHandle Doc, ppUns32 Page );


    /************************************************************************
    Description :
        Get Page Object Handle.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
    Returns :
        Page Object Handle.
    ************************************************************************/
    LIB_API PDFCosHandle PDFPageGetCosObject( PDFDocHandle Doc, ppUns32 Page );

	/************************************************************************
    Description :
        Set tumbnail for Page.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
		Image    -[in] ImageIndex in PDF Document.
    Returns :
        Page Object Handle.
    ************************************************************************/
    LIB_API void PDFPageSetTumbnail( PDFDocHandle Doc, ppUns32 Page, ppUns32 Image );

    /* Description
       Set Rotation Angle of Page.
       Parameters
       Doc :     [in] PDF Document Handle.
       Page :    [in] Page Index in PDF Document.
       Rotate :  [in] Page Rotation Angle by which the page displayed
                 or printed should be rotated clockwise 
       Returns
       None.
       See Also
       PDFPageGetRotateAngle                                          */
    LIB_API void PDFPageSetRotateAngle( PDFDocHandle Doc, ppUns32 Page, TPDFPageRotateAngle Rotate );


    /* Description
       Get Rotation Angle of Page.
       Parameters
       Doc :   [in] PDF Document Handle.
       Page :  [in] Page Index in PDF Document.
       Returns
       Page Rotation Angle by which the page displayed or printed
       should be rotated clockwise.
       See Also
       PDFPageSetRotateAngle                                      */
    LIB_API TPDFPageRotateAngle PDFPageGetRotateAngle( PDFDocHandle Doc, ppUns32 Page );


	/* Description
       Get Typed Rectangle PageBox.
       Parameters
       Doc :   [in] PDF Document Handle.
       Page :  [in] Page Index in PDF Document.
       Type :  [in] PageBox Type for request.
       Returns
       PageBox Typed Rectangle.
       See Also
       PDFPageSetBox                            */
    LIB_API TPDFRect PDFPageGetBox( PDFDocHandle Doc, ppUns32 Page, TPDFPageBoxType Type );


    /* Description
       Set Typed Rectangle PageBox.
       Parameters
       Doc :   [in] PDF Document Handle.
       Page :  [in] Page Index in PDF Document.
       Type :  [in] PageBox Type for setting.
       Rect :  [in] PageBox Rectangle.
       Returns
       None.
       See Also
       PDFPageGetBox                            */
    LIB_API void PDFPageSetBox( PDFDocHandle Doc, ppUns32 Page, TPDFPageBoxType Type, TPDFRect Rect );


    /************************************************************************
    Description :
        Add void Content to Page Content Streams.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
    Returns :
        Page Content Streams Count..
    See also :
        PDFPageGetContentCount
    ************************************************************************/
    LIB_API ppUns32 PDFPageAddContent( PDFDocHandle Doc, ppUns32 Page );


    /************************************************************************
    Description :
        Insert void Content to Page Content Streams in Indexed Site.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
        Index   - [in] Content Index for inserting.
    Returns :
        Index of void Page Content in Content Streams.
    See also :
        PDFPageRemoveContent
    ************************************************************************/
    LIB_API ppUns32 PDFPageInsertContent( PDFDocHandle Doc, ppUns32 Page, ppUns32 Index );


    /* Description
       Remove Content from Page Content Streams according to index.
       Parameters
       Doc :    [in] PDF Document Handle.
       Page :   [in] Page Index in PDF Document.
       Index :  [in] Content Index for deleting.
       Returns
       None.
       See Also
       PDFPageInsertContent                                         */
    LIB_API void PDFPageRemoveContent( PDFDocHandle Doc, ppUns32 Page, ppUns32 Index );


    /************************************************************************
    Description :
        Create Paint Box for Last Page Content with defined resolution.
    Parameters :
        Doc     - [in] PDF Document Handle.
        Page    - [in] Page Index in PDF Document.
        Resolution - [in] Points per inch (dpi).
    Returns :
        Paint Box Handle.
    See also :
        PDFPageCreatePaintBoxFromContent
    ************************************************************************/
    LIB_API PBXHandle PDFPageCreatePaintBox( PDFDocHandle Doc, ppUns32 Page, ppUns32 Resolution );


    /* Description
       Create Paint Box for Page Content according to index with
       defined resolution.
       Parameters
       Doc :         [in] PDF Document Handle.
       Page :        [in] Page Index in PDF Document.
       Index :       [in] Content's Index in Page Content Streams.
       Resolution :  [in] Points per inch (dpi).
       Returns
       Paint Box Handle.
       See Also
       PDFPageCreatePaintBox                                       */
    LIB_API PBXHandle PDFPageCreatePaintBoxFromContent( PDFDocHandle Doc, ppUns32 Page, ppUns32 Index,
                                           ppUns32 Resolution );

	/* Description
		Repack  al contents of the page and wrap it with push and pop garaphic state after it create PaintBox.
	Parameters
		Doc :    [in] PDF Document Handle.
		Page :   [in] Page Index in PDF Document.
		Resolution :  [in] Points per inch (dpi).
	Returns
		Paint Box Handle..*/
	LIB_API PBXHandle PDFPageCreatePaintBoxAndRepackContents( PDFDocHandle Doc, ppUns32 Page, ppUns32 Resolution);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSPageA_H */
