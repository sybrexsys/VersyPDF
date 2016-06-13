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
VSAcroInfoA.h
*********************************************************************/

#ifndef _H_VSACROINFOA
#define _H_VSACROINFOA 

#include "time.h"

#include "VSTypes.h"
#include "VSFontA.h"
#include "VSCanvasA.h"
#include "VSActionA.h"


/* Acro Form Object Type. Interactive Control Type. */

_EXP
typedef enum {
	PDFAcroTypeUnknown = 0,     /*  Unknown Type, in failure case                           */
	PDFAcroTypePushButton,      /*  Button for select single action                         */
	PDFAcroTypeCheckBox,        /*  Button for check single item                            */
	PDFAcroTypeRadioButton,     /*  Button from group for select only one item from ensemble*/
	PDFAcroTypeEditBox,         /*  Variable text edit field for change text item           */
	PDFAcroTypeComboBox,        /*  Field for select one text item from list                */
	PDFAcroTypeListBox,         /*  Box for select item(s) from list                        */
	PDFAcroTypeSignature        /*  Field for sign in document, maybe invisible             */
} TPDFAcroType; 



#ifdef __cplusplus
extern "C" {
#endif

/* 
	Description
		Gets number of Acro Objects in Document.
	Parameters
		Doc :  [in] Source Document.
	Returns
		Number of Acro Objects in Document, integer number. 
*/

LIB_API ppUns32     PDFAcroGetCount( PDFDocHandle Doc );

/* 
	Description
		Gets Full name of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
		IsUnicode : [out] True if result of the function id unicode
		Length: [out] Length of the result (bytes for ASCII and words for unicode result)
	Returns
		Full name of Acro Form Object ( where partial names are separated by point ).
*/
LIB_API void * PDFAcroGetNameByIndex (PDFDocHandle Doc, ppUns32 Index, ppBool *IsUnicode, ppUns32 *Length);

/* 
	Description
		Gets Value of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Text string
	See Also
		PDFAcroSetValueByIndex                                    
*/
LIB_API char * PDFAcroGetValueByIndex (PDFDocHandle Doc, ppUns32 Index, ppUns32 *Length);


/* 
	Description
		Gets Type of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Typed value, for example PushButton, ComboBox, etc.
	See Also
		PDFAcroType                                            
*/
LIB_API TPDFAcroType PDFAcroGetTypeByIndex( PDFDocHandle Doc, ppUns32 Index );

/* 
	Description
		Gets Cos Object of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Cos Object Handle.
	See Also
		PDFAcroType                                            
*/
LIB_API PDFCosHandle PDFAcroGetObjByIndex( PDFDocHandle Doc, ppUns32 Index );

/* 
	Description
		Gets flag of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Flag of the acroform object. More information about this value you can find in PDF Reference.
	See Also
		PDFAcroType                                            
*/
LIB_API ppInt32 PDFAcroGetFlagsByIndex( PDFDocHandle Doc, ppUns32 Index );

/* 
	Description
		Gets index of Acro Form Object in Document according to name.
	Parameters
		Doc			:  [in] Source Document.
		FieldName	:  [in] Name of the acroform object.
		CheckUnicodeNames : [in] if true function will check acrofields with unicode names
	Returns
		Index of the acroform object.
*/
LIB_API ppUns32 PDFAcroGetIndexByName(PDFDocHandle Doc, char * FieldName, ppBool CheckUnicodeNames);

/* 
	Description
		Sets value of Acro Form Object in Document according to Index.
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
		NewValue : [in] New value of the acroform object.
	Returns
		None.
	Note
		Value "Off" turn off checkbox and radiobutton group fields.
		Any other value turn on checkbox field.
		For radiobutton group you must use only values received with PDFAcroGetOptionValueByIndex function
*/
LIB_API void PDFAcroSetValueByIndex ( PDFDocHandle Doc, ppUns32 Index, char * NewValue);


/* 
	Description
		Gets count of acroform object's options. Can be used only for combobox, listbox and radiobutton group. 
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		Count of acroform object's options.
*/
LIB_API ppUns32 PDFAcroGetOptionCountByIndex ( PDFDocHandle Doc, ppUns32 Index);


/* 
	Description
		Gets MaxLength value of acroform object. 
		Can be used only for combobox and EditBox. 
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
	Returns
		MaxLength value of acroform object. If not present sets to 0;
*/
 
ppUns32 PDFAcroGetMaxLengthByIndex ( PDFDocHandle Doc, ppUns32 Index);

/* 
	Description
		Gets option value of acroform object. 
		Can be used only for combobox, listbox and radiobutton group. 
	Parameters
		Doc :  [in] Source Document.
		Index :  [in] Index as from 0. Object number in AcroForm.
		OptionIndex : [in]  Index of the option in acroform field.
		Size: [out] Size of the option value.
	Returns
		Option value of acroform object.
*/
LIB_API char * PDFAcroGetOptionValueByIndex ( PDFDocHandle Doc, ppUns32 Index, ppUns32 OptionIndex, ppUns32 *Size);


LIB_API ppBool PDFAcroChangeNameByName (PDFDocHandle Doc, char *OldName, char *NewName);



#ifdef __cplusplus
}
#endif


#endif /* #ifndef _H_VSACROINFOA */
