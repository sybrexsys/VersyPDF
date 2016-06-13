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
VSAnnotA.h
*********************************************************************/

#ifndef VSAnnotA_H
#define VSAnnotA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSCosA.h"
#include "VSDocA.h"
#include "VSTypes.h" 
#include "VSExcept.h"
#include "VSError.h"
#include "VSPageA.h"

#ifdef __cplusplus
extern "C"{
#endif

	/* If set, do not display the annotation if it does not belong
	to one of the standard annotation types and no annotation
	handler is available. If clear, display such an unknown
	annotation using an appearance stream specified by its 
	appearance dictionary, if any */
#define PDF_ANNOTATION_INVISIBLE        1

	/* If set, do not display or print the annotation or allow it
	to interact with the user, regardless of its annotation type or
	whether an annotation handler is available. In cases where
	screen space is limited, the ability to hide and show annotations
	selectively can be used in combination with appearance streams
	to display auxiliary pop-up information similar in function to
	online help systems. */
#define PDF_ANNOTATION_HIDDEN           2

	/* If set, print the annotation when the page is printed. If clear,
	never print the annotation, regardless of whether it is displayed
	on the screen. This can be useful, for example, for annotations
	representing interactive pushbuttons, which would serve no
	meaningful purpose on the printed page. */
#define PDF_ANNOTATION_PRINT            4

	/* If set, do not scale the annotation’s appearance to match the
	magnification of the page. The location of the annotation on the
	page (defined by the upper-left corner of its annotation rectangle)
	remains fixed, regardless of the page magnification. See below for
	further discussion. */
#define PDF_ANNOTATION_NO_ZOOM          8

	/* If set, do not rotate the annotation’s appearance to match the
	rotation of the page. The upper-left corner of the annotation
	rectangle remains in a fixed location on the page, regardless of
	the page rotation. See below for further discussion. */
#define PDF_ANNOTATION_NO_ROTATE        16

	/* If set, do not display the annotation on the screen or allow it
	to interact with the user. The annotation may be printed (depending
	on the setting of the Print flag), but should be considered hidden
	for purposes of on-screen display and user interaction. */
#define PDF_ANNOTATION_NO_VIEW          32

	/* If set, do not allow the annotation to interact with the user.
	The annotation may be displayed or printed (depending on the
	settings of the NoView and Print flags), but should not respond to
	mouse clicks or change its appearance in response to mouse motions.*/
#define PDF_ANNOTATION_READONLY         64

	/* If set, do not allow the annotation to be deleted or its
	properties (including position and size) to be modified by the user.
	However, this does not restrict changes to the annotation’s contents,
	such as a form field value. */
#define PDF_ANNOTATION_LOCKED           128

	/* If set, invert the interpretation of the NoView flag for certain
	events. A typical use is to have an annotation that appears only
	when a mouse cursor is held over it. */
#define PDF_ANNOTATION_TOGGLE_NO_VIEW   256


	/* The annotation border styles */
    typedef enum {
        bsSolid,		/* A solid rectangle surrounding the annotation. */
        bsDashed,		/* A dashed rectangle surrounding the annotation. */
		bsBeveled,		/* A simulated embossed rectangle that appears to be raised above the surface of the page. */
		bsInset,		/* A simulated engraved rectangle that appears to be recessed below the	surface of the page. */
        bsUnderline		/* A single line along the bottom of the annotation rectangle. */
    } TBorderStyle;




/* Common Annotation functions */

	/********************************************************************
    Description:
        Sets flag for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set flags.
		   AnnotationFlags      - [ in ] Specifies annotation's flags.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetFlag( PDFAnnotationHandle Annotation, ppUns16 AnnotationFlag );

	/********************************************************************
    Description:
        Sets alpha blending for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set alpha blending.
		   AlphaBlending		- [ in ] Specifies annotation's alpha blending.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetAlphaBlending( PDFAnnotationHandle Annotation, ppReal AlphaBlending );

	/********************************************************************
    Description:
        Sets Title for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set title.
		   Title				- [ in ] Specifies annotation's title.
		   TitleLength			- [ in ] Specifies title's length.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetTitle ( PDFAnnotationHandle Annotation, char * Title, ppUns32 TitleLength );

	/********************************************************************
    Description:
        Sets Name for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set name.
		   Name					- [ in ] Specifies annotation's name.
		   NameLength			- [ in ] Specifies title's name.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetName ( PDFAnnotationHandle Annotation, char * Name, ppUns32 NameLength );

	/********************************************************************
    Description:
        Sets border style for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set border style.
		   Width				- [ in ] Specifies border's width.
		   Style				- [ in ] Specifies border's style.
		   Dash					- [ in ] Specifies dash of the border. Used only with style equal to bsDashed.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetBorderStyle ( PDFAnnotationHandle Annotation, ppReal Width, TBorderStyle Style, PDFCosHandle Dash );

	/********************************************************************
    Description:
        Sets color for PDF Annotation.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set color.
		   Color				- [ in ] Specifies annotation's name.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetColor ( PDFAnnotationHandle Annotation, TPDFColor Color );

	/********************************************************************
    Description:
        Sets action for PDF Annotation which will executed when annotation is activated.
    Parameters:
           Annotaion            - [ in ] PDF annotation for which need set action.
		   Action				- [ in ] Specifies annotation's action.
    Returns:
        None.
    *********************************************************************/
LIB_API void PDFAnnotationSetAction ( PDFAnnotationHandle Annotation, PDFActionHandle Action );

	


/*          Text Annotation         */

/* Available annotations names */
_EXP
typedef enum {
    atnNote ,         /* Note annotation */
    atnComment ,      /* Comment annotation */
    atnHelp ,         /* Help annotation */
    atnInsert ,       /* Insert annotation */
    atnKey ,          /* Key annotation */
    atnNewParagraph , /* New paragraph annotation */
    atnParagraph      /* Paragraph annotation */
} TAnnotationTextName;

	/********************************************************************
    Description:
        Create new text annotation.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created text annotation.
		   Rect					- [ in ] The annotation rectangle, defining the location of the annotation on the page
		   Content				- [ in ] Text to be displayed for the annotation	
		   ContentLength		- [ in ] Length of the content.
		   IsOpened				- [ in ] Specifying whether the annotation should initially be displayed open.
		   Name					- [ in ] The name of an icon to be used in displaying the annotation.
    Returns:
        Text annotation handle.
    *********************************************************************/
LIB_API PDFAnnotationHandle PDFPageAppendAnnotationText ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, char * Content, 
														 ppUns32 ContentLength, ppBool IsOpened, TAnnotationTextName Name );


/*          Link Annotation         */

/* The annotation’s highlighting mode, the visual effect to be
used when the mouse button is pressed or held down inside its active area. */
typedef enum {
    hlmNoHightLight,	/* No highlighting. */
    hlmInvert,			/* Invert the contents of the annotation rectangle. */
    hlmOutline,			/* Invert the annotation's border. */
    hlmPush				/* Display the annotation's down appearance. */
} THighLightMode;

	/********************************************************************
    Description:
        Create new link annotation.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created link annotation.
		   Rect					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		   Destination			- [ in ] A destination to be displayed when the annotation is activated.
		   Mode					- [ in ] The annotation’s highlighting mode.
		   Visible				- [ in ] No border is drawn if set to false.
    Returns:
        Link annotation handle.
    *********************************************************************/

LIB_API PDFAnnotationHandle PDFPageAppendAnnotationLinkWithDest ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, 
														 PDFDestinationHandle Destination, THighLightMode Mode , ppBool Visible );

	/********************************************************************
    Description:
        Create new link annotation.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created link annotation.
		   Rect					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		   Action				- [ in ] An action to be performed when the annotation is activated.
		   Mode					- [ in ] The annotation’s highlighting mode.
		   Visible				- [ in ] No border is drawn if set to false.
    Returns:
        Link annotation handle.
    *********************************************************************/
LIB_API PDFAnnotationHandle PDFPageAppendAnnotationLinkWithAction ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, 
														 PDFActionHandle Action, THighLightMode Mode , ppBool Visible );


/************************************************************************/
/*     Stamp Annotations                                                */
/************************************************************************/

/* Available stamp icon types */
typedef enum {
    asnApproved,				/* Approved icon type */
    asnAsIs,					/* AsIs icon type */
    asnConfidential,			/* Confidential icon type */
    asnDepartmental,			/* Departmental icon type */
    asnDraft,				    /* Draft icon type */
    asnExperimental,			/* Experimental icon type */
    asnExpired,				    /* Expired icon type */
    asnFinal,				    /* Final icon type */
    asnForComment,			    /* For comment icon type */
    asnForPublicRelease,		/* For public release icon type */
    asnNotApproved,			    /* Not approved icon type */
    asnNotForPublicRelease,	    /* Not for public release icon type */
    asnSold,					/* Sold icon type */
    asnTopSecret,			    /* Top secret icon type */
    asnOther                    /* Other icon type */       
}TAnnotationStampName;

	/********************************************************************
    Description:
        Create new stamp annotaion.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created stamp annotation.
		   Rect					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		   Content				- [ in ] Text to be displayed for the annotation	
		   ContentLength		- [ in ] Length of the content.
		   Name					- [ in ] The name of an icon to be used in displaying the annotation.
    Returns:
        Link annotation handle.
    *********************************************************************/
LIB_API PDFAnnotationHandle PDFPageAppendAnnotationStamp ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, char * Content, 
														  ppUns32 ContentLength, TAnnotationStampName Name);

/********************************************************************
    Description:
        Create new stamp annotaion.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created stamp annotation.
		   Rect					- [ in ] The annotation rectangle, defining the location of the annotation on the page.
		   Content				- [ in ] Text to be displayed for the annotation	
		   ContentLength		- [ in ] Length of the content.
		   Name					- [ in ] The name of an icon to be used in displaying the annotation.
    Returns:
        Link annotation handle.
    *********************************************************************/

PDFAnnotationHandle PDFPageAppendAnnotationStampWithDrawBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect,char * Content, 
															 ppUns32 ContentLength, TAnnotationStampName Name, ppUns32 Resolution, PBXHandle *PBX);


#ifdef __cplusplus
}
#endif


#endif /* #ifndef VSAnnotA */

