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
VSAcroFormA.h
*********************************************************************/

#ifndef _H_VSACROFORMA
#define _H_VSACROFORMA
#include "VSTypes.h"


#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/*    Common function for acroform objects                              */
/************************************************************************/


	/* Macro Definitions of NOCOLOR for PDF Color Type - Transparent, Invisible Color. */
#define NoPDFColor      0xDAB                           /* Nonexistent Color Code   */
#define SetNoPDFColor   ( ULGrayToColor( NoPDFColor ))  /* Transparent PDFColor     */
#define IsNoPDFColor(c) ( c.Color.Gray == NoPDFColor )  /* Check on invisibility    */


	/* Acro Form Object Quadding Type. Text justification style. */
	_EXP
		typedef enum {
			PDFAcroQuaddingLeftTop = 0, /* attach text to left top corner of field */
			PDFAcroQuaddingTop,         /* attach text to top central site of field */
			PDFAcroQuaddingRightTop,    /* attach text to right top corner of field */
			PDFAcroQuaddingLeft,        /* attach text to left central site of field */
			PDFAcroQuaddingCenter,      /* attach text to center of field */
			PDFAcroQuaddingRight,       /* attach text to right central site of field */
			PDFAcroQuaddingLeftBottom,  /* attach text to left bottom corner of field */
			PDFAcroQuaddingBottom,      /* attach text to bottom central site of field */
			PDFAcroQuaddingRightBottom  /* attach text to right bottom corner of field */
		} TPDFAcroQuadding;


	/* Type of CheckBox Style. */
	_EXP
		typedef enum {
			cbfRectangle = 0,           /* Rectangle style */
			cbfCircle                   /* Circle style */
		} TPDFCheckBoxStyle;


	/* Action Event Type for Acro Form Objects. Set of events. */
_EXP
typedef enum {
    PDFAcroEventTypeActivate = 0,   /* event on activate, primary action      */
    PDFAcroEventTypeEnter,          /* event on enter in the active area      */
    PDFAcroEventTypeExit,           /* event on exit from the active area     */
    PDFAcroEventTypePress,          /* event on press mouse button inside it  */
    PDFAcroEventTypeRelease,        /* event on release mouse button inside   */
    PDFAcroEventTypeFocusOn,        /* event on receive the input focus       */
    PDFAcroEventTypeFocusOff,       /* event on lose the input focus          */
    PDFAcroEventTypeKeystroke,      /* event on change text value in field    */
    PDFAcroEventTypeFormat,         /* event on format value in the field     */
    PDFAcroEventTypeValidate,       /* event on change field’s value in field */
    PDFAcroEventTypeCalculate       /* event on recalculate value             */
} TPDFAcroEventType;

#define PDFACRO_DEFAULT				0
#define PDFACRO_REQUIRED			1
#define PDFACRO_READONLY			2
#define PDFACRO_NOEXPORT			4
#define PDFACRO_MULTILINE			8
#define PDFACRO_PASSWORD			16
#define PDFACRO_EDIT				32
#define PDFACRO_MULTISELECT			64
#define PDFACRO_DONOTSPELLCHECK		128
#define PDFACRO_DONOTSCROLL			256



	/* Type of CheckBox Mark - code of Zapf Dingbats sign ( 32 - 255 ). Here is definition in octal system */
typedef enum {
	cbsFinger     = 0053, /* 'Finger' mark */
	cbsPen        = 0062, /* 'Pen' mark */
	cbsVmark      = 0063, /* V-style mark */
	cbsNike       = 0064, /* V-style mark */
	cbsCross      = 0065, /* Cross-style mark */
	cbsX          = 0066, /* Cross-style mark */
	cbsCheck      = 0070, /* Cross-style mark */
	cbsPlus       = 0072, /* Cross-style mark */
	cbsDiamond    = 0106, /* Rhombus-style mark */
	cbsStar       = 0110, /* 'Star' mark */
	cbsFlower     = 0137, /* 'Flower' mark */
	cbsSnow       = 0144, /* 'Snowflake' mark */
	cbsCircle     = 0154, /* 'Circle' mark */
	cbsRectangle  = 0156, /* 'Rectangle' mark */
	cbsRhombus    = 0165, /* Rhombus-style mark */
} TPDFCheckBoxSign;


	
/*	
	Description
		Sets Acroobject border
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		BorderColor :       [in] Color of the border line.
		FillColor :			[in] Color of the background.
		BorderWidth:		[in] Width of the border line.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectSetBorder( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFColor BorderColor, TPDFColor FillColor, ppReal BorderWidth );

/*	
	Description
		Sets Acroobject drawing font
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		FontIndex :			[in] Index of the font stored in PDF document.
		FontSize :			[in] Size of the used font.
		Color :				[in] Color of the used font.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectSetFont ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 FontIndex, ppReal FontSize, TPDFColor Color );

/*	
	Description
		Sets flags specifying various characteristics (as annotation) of the acroform object.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Flag :				[in] Characteristics of the acroform object .
	Returns
		None.                                                            
*/

LIB_API void PDFAcroObjectSetAnnotationFlag ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns16 AnnotationFlag  );
/*	
	Description
		Sets flags specifying various characteristics of the acroform object.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Flag :				[in] Characteristics of the acroform object .
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectSetFlag ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 Flag );

/*	
	Description
		Sets caption of the acroform object.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Caption :			[in] Caption which need set .
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectSetCaption ( PDFDocHandle Doc, ppUns32 AcroIndex, char * Caption );

/*	
	Description
		Appends items to ListBox or to ComboBox.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Item :				[in] Item which need append to list.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectAppendItem ( PDFDocHandle Doc, ppUns32 AcroIndex, char * Item );


	/********************************************************************
    Description:
        Create new acro edit box.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created edit box.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendEditBox (PDFDocHandle Doc,  ppUns32 Page, TPDFRect R, char * Name  );

/*	
	Description
		Sets maximum length of the edit box text.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		MaxLen :			[in] Maximum length of the edit box text.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroEditBoxSetMaxLen ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 MaxLen );

/*	
	Description
		Sets text justification of the edit box.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Align :				[in] Text justification which need set.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroEditBoxSetAlign ( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFAcroQuadding Align);

	/********************************************************************
    Description:
        Create new acro combo box.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created combo box.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendComboBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name );

	/********************************************************************
    Description:
        Create new acro list box.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created list box.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendListBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name );

	/********************************************************************
    Description:
        Create new acro empty signature box.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created signature box.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendSignatureBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name );

/*	
	Description
		Sets display style for radiobutton and checkbox.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Sign :				[in] Code of Mark character in CheckBox or RadioButton.
		Style :				[in] Style of CheckBox or RadioButton - rectangle or circle.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroObjectSetStyle ( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFCheckBoxSign Sign, TPDFCheckBoxStyle Style );

/*	
	Description
		Sets miter of the pushbutton.
	Parameters
		Doc :				[in] Document object handle.
		AcroIndex :			[in] Index of acroform object in document for which need set parameters.
		Miter :				[in] Miter of pushbutton, bevel size.
	Returns
		None.                                                            
*/
LIB_API void PDFAcroPushButtonSetMiter ( PDFDocHandle Doc, ppUns32 AcroIndex, ppReal Miter );

	/********************************************************************
    Description:
        Create new acro push button.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created push button.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendPushButton ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name);

	/********************************************************************
    Description:
        Create new acro check box.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created check box.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
			InitialState		- [ in ] Value of the check box.
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendCheckBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name, ppBool InitialState );
    /********************************************************************
    Description:
        Create new acro radio button.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created radio button.
		   Rect					- [ in ] The annotation rectangle, defining the location of the acro field on the page
		   Name					- [ in ] Name of Acroform object, Name of Acroform field is used to export when the PDF document is
										submitted	
			Group				- [ in ] Name of Radio Buttons Group to which RadioButton will be linked.
			InitialState		- [ in ] Value of the check box.
    Returns:
        Index of acroform object in document.
    *********************************************************************/
LIB_API ppUns32 PDFPageAppendRadioButton ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name, char *Group, ppBool InitialState );

/* Description
   Adds Action OnEvent in Acro Form Object (Control).
   Parameters
   Doc :              [in] Destination document.
   AcroObjectIndex :  [in] Index of acroform object in document.
   Action :           [in] Handle on action for include to trigger.
   Type :             [in] Type of event on Control for Action
   See Also
   TPDFAcroEventType, Actions
   Returns
   None.                                                            */
LIB_API void PDFAcroObjectAddAction( PDFDocHandle Doc, ppUns32 AcroIndex,
                             PDFActionHandle Action, TPDFAcroEventType Type);


/* Description
	Appends digital signature to PDF document stored in memory buffer.
   Parameters
   Doc :				[in] Destination document handle.
   Buffer :				[in] Pointer to memory buffer where stored PFX Personal Signature file.
   Length :				[in] Size of the memory buffer.
   Name :				[in] Name of the digital signature acro object field.
   Reason:				[in] Reason of Sign this document. Text string,  for example "I agree..."
   PKCS7:				[in] Boolean flag of coding type :
								true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
								false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
   Password:			[in] Owner Password for Personal Signature. Text string	
   Returns
   None.                                                            */
LIB_API void PDFDocAppendSignatureFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 Length, char * Name, 
											  char * Reason, ppBool PKCS7, char * Password );
/* Description
	Appends digital signature to PDF document stored in file.
   Parameters
   Doc :				[in] Destination document handle.
   FileName :			[in] Filename of the file where stored PFX Personal Signature.
   Name :				[in] Name of the digital signature acro object field.
   Reason:				[in] Reason of Sign this document. Text string,  for example "I agree..."
   PKCS7:				[in] Boolean flag of coding type :
								true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
								false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
   Password:			[in] Owner Password for Personal Signature. Text string	
   Returns
   None.                                                            */

LIB_API void PDFDocAppendSignatureFromFile ( PDFDocHandle Doc, char * FileName, char * Name, 
											char * Reason, ppBool PKCS7, char * Password );

/* Description
	Appends digital signature to PDF document stored in memory buffer.
   Parameters
   Doc :				[in] Destination document handle.
   Stream :				[in] Stream where stored PFX Personal Signature file.
   Length :				[in] Size of the memory buffer.
   Name :				[in] Name of the digital signature acro object field.
   Reason:				[in] Reason of Sign this document. Text string,  for example "I agree..."
   PKCS7:				[in] Boolean flag of coding type :
								true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt system sub filter
								false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1' crypt system sub filter
   Password:			[in] Owner Password for Personal Signature. Text string	
   Returns
   None.                                                            */

LIB_API void PDFDocAppendSignatureFromStream ( PDFDocHandle Doc, PDFStreamHandle Stream, char * Name, 
											  char * Reason, ppBool PKCS7, char * Password );

	/********************************************************************
    Description:
        Place appended digital signature to page.
    Parameters:
           Doc		            - [ in ] PDF Document handle.
		   Page					- [ in ] Index of the page where will located created signature box.
		   Rect					- [ in ] The signature rectangle, defining the location on the page.
		   Resolution			- [ in ] The resolution of the paint box 
    Returns:
        Paint box handle for drawing signature.
    *********************************************************************/
LIB_API PBXHandle PDFPagePlaceSignature ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, ppUns32 Resolution);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _H_VSACROFORMA */