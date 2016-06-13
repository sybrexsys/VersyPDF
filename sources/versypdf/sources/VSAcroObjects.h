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
VSAcroObjects.h
*********************************************************************/

#ifndef _H_VSACROOBJECTS
#define _H_VSACROOBJECTS /* Acro Form Interactive Objects Module */

#include "time.h"

#include "VSTypes.h"
#include "VSFontA.h"
#include "VSCanvasA.h"
#include "VSActionA.h"
#include "VSAcroFormA.h"
#include "VSAcroInfoA.h"



#define AcroField_Flag_None            0
#define AcroField_Flag_TypeMask        7
#define AcroField_Flag_Button          8
#define AcroField_Flag_PushButton      9
#define AcroField_Flag_Checkbox       10
#define AcroField_Flag_RadioButton    11
#define AcroField_Flag_Text           16
#define AcroField_Flag_Choice         32
#define AcroField_Flag_ListBox        33
#define AcroField_Flag_ComboBox       34
#define AcroField_Flag_Signature      64
#define AcroField_Flag_MultiplyMask  127
#define AcroField_Flag_Multiply      128
typedef ppUns8 TPDFAcroFieldFlags, *PPDFAcroFieldFlags;


#define IsAcroFieldFlagNull(a)          (AcroFieldFlagWOMultiply(a)==AcroField_Flag_None)
#define IsAcroFieldFlagTyped(a)         ((a&AcroField_Flag_TypeMask)>0)
#define IsAcroFieldFlagButton(a)        ((a&AcroField_Flag_Button)>0)
#define IsAcroFieldFlagPushButton(a)    (AcroFieldFlagWOMultiply(a)==AcroField_Flag_PushButton)
#define IsAcroFieldFlagCheckbox(a)      (AcroFieldFlagWOMultiply(a)==AcroField_Flag_Checkbox)
#define IsAcroFieldFlagRadioButton(a)   (AcroFieldFlagWOMultiply(a)==AcroField_Flag_RadioButton)
#define IsAcroFieldFlagText(a)          (AcroFieldFlagWOMultiply(a)==AcroField_Flag_Text)
#define IsAcroFieldFlagChoice(a)        ((a&AcroField_Flag_Choice)>0)
#define IsAcroFieldFlagListBox(a)       (AcroFieldFlagWOMultiply(a)==AcroField_Flag_ListBox)
#define IsAcroFieldFlagComboBox(a)      (AcroFieldFlagWOMultiply(a)==AcroField_Flag_ComboBox)
#define IsAcroFieldFlagSignature(a)     (AcroFieldFlagWOMultiply(a)==AcroField_Flag_Signature)
#define IsAcroFieldFlagMultiply(a)      ((a&AcroField_Flag_Multiply)>0)
#define AcroFieldFlagWOMultiply(a)      (a&AcroField_Flag_MultiplyMask)
#define AcroFieldFlagGroup(a)           (a&(AcroField_Flag_MultiplyMask-AcroField_Flag_TypeMask))

typedef struct TPDFAcroFormItem{
	char * StrName;
	ppUns16 *UnicodeName;
	ppUns32 Length;
	ppBool IsUnicode;
	ppInt32 Flag;
	ppAtom Type;
	PDFCosHandle V;
	PDFCosHandle DV;
	PDFCosHandle Obj;
	TPDFAcroType AType; 
} TPDFAcroFormItem, * PPDFAcroFormItem;

typedef ppUns32 TPDFAcroFlags;  /* Acro Object's set of Field's Flags */
typedef TPDFAcroFlags *PPDFAcroFlags; /* Pointer to Field's Flags */

/* Acro Form Object Annotation's Flags. Property of controls. */
#define PDFAnnotFlags_Default         0 /* Default flag without any features */
#define PDFAnnotFlags_Invisible       1 /* If set, do not display the annotation if it does not belong
                                        to one of the standard annotation types and no annotation
                                        handler is available. If clear, display such an unknown
                                        annotation using an appearance stream specified by its 
                                        appearance dictionary, if any */
#define PDFAnnotFlags_Hidden          2 /* If set, do not display or print the annotation or allow it
                                        to interact with the user, regardless of its annotation type or
                                        whether an annotation handler is available. In cases where
                                        screen space is limited, the ability to hide and show annotations
                                        selectively can be used in combination with appearance streams
                                        to display auxiliary pop-up information similar in function to
                                        online help systems. */
#define PDFAnnotFlags_Print           4 /* If set, print the annotation when the page is printed. If clear,
                                        never print the annotation, regardless of whether it is displayed
                                        on the screen. This can be useful, for example, for annotations
                                        representing interactive pushbuttons, which would serve no
                                        meaningful purpose on the printed page. */
#define PDFAnnotFlags_NoZoom          8 /* If set, do not scale the annotation’s appearance to match the
                                        magnification of the page. The location of the annotation on the
                                        page (defined by the upper-left corner of its annotation rectangle)
                                        remains fixed, regardless of the page magnification. See below for
                                        further discussion. */
#define PDFAnnotFlags_NoRotate       16 /* If set, do not rotate the annotation’s appearance to match the
                                        rotation of the page. The upper-left corner of the annotation
                                        rectangle remains in a fixed location on the page, regardless of
                                        the page rotation. See below for further discussion. */
#define PDFAnnotFlags_NoView         32 /* If set, do not display the annotation on the screen or allow it
                                        to interact with the user. The annotation may be printed (depending
                                        on the setting of the Print flag), but should be considered hidden
                                        for purposes of on-screen display and user interaction. */
#define PDFAnnotFlags_ReadOnly       64 /* If set, do not allow the annotation to interact with the user.
                                        The annotation may be displayed or printed (depending on the
                                        settings of the NoView and Print flags), but should not respond to
                                        mouse clicks or change its appearance in response to mouse motions.*/
#define PDFAnnotFlags_Locked        128 /* If set, do not allow the annotation to be deleted or its
                                        properties (including position and size) to be modified by the user.
                                        However, this does not restrict changes to the annotation’s contents,
                                        such as a form field value. */
#define PDFAnnotFlags_ToggleNoView  256 /* If set, invert the interpretation of the NoView flag for certain
                                        events. A typical use is to have an annotation that appears only
                                        when a mouse cursor is held over it. */
typedef ppUns16 TPDFAnnotFlags;  /* Acro Object's set of Annotation's Flags */
typedef TPDFAnnotFlags *PPDFAnnotFlags;  /* Pointer to Annotation's Flags */


/* Acro Form Object Appearance Type. For point to appearance type. */
typedef enum {
    apsNormal = 0,    /* The normal appearance is used when the annotation
                                    is not interacting with the user. This is also the 
                                    appearance that is used for printing the annotation. */
    apsRollover,      /* The rollover appearance is used when the user moves
                                    the cursor into the annotation’s active area without
                                    pressing the mouse button. */
    apsDown           /* The down appearance is used when the mouse button is
                                    pressed or held down within the annotation’s active area. */
} TPDFAppearanceStreamType;

/* Acro Form Signature's Flags. Document Digital Signature's properties. */
#define PDFAcroSigFlags_Default         0   /* Default flag - document without any signatures */
#define PDFAcroSigFlags_SignaturesExist 1   /* If set, the document contains at least one signature
                                            field. This flag allows a viewer application to enable
                                            user interface items (such as menu items or pushbuttons)
                                            related to signature processing without having to scan
                                            the entire document for the presence of signature fields.*/
#define PDFAcroSigFlags_AppendOnly      2   /* If set, the document contains signatures that may be
                                            invalidated if the file is saved (written) in a way that
                                            alters its previous contents, such as with the “optimize”
                                            option. Merely updating the file by appending new
                                            information to the end of the previous version is safe.
                                            Viewer applications can use this flag to present a user
                                            requesting an optimized save with an additional alert box
                                            warning that signatures will be invalidated and requiring
                                            explicit confirmation before continuing with the operation.*/
typedef ppUns8 TPDFAcroSigFlags; /* Digital Signature's set of Flags */
typedef TPDFAcroSigFlags *PPDFAcroSigFlags; /* Pointer to Siggnature Flags */



/* Font Index Structure - to support early PDF versions ( 1.2
   and below )                                                */
typedef struct {
    ppBool IsStdFont;                   /* Boolean flag an accessories to 14 standard fonts */
    union {                             /* Font source */
        ppUns32           Index;        /* Index of loaded font in Document */
        TPDFStdandardFont StandardFont; /* If standard font then it must be named, see TPDFStdandardFont */
    } From;                             /* Font source */
} TPDFFontID, *PPDFFontID;

/* Font Type. Specifies text font properties of the annotation on the page.
( Acroform Object Characteristic ) */
typedef struct {
    TPDFFontID  ID;     /* Identifier of font type, see TPDFFontID      */
    ppReal      Size;   /* Size of font in points                       */
    TPDFColor   Color;  /* Color of font for displaying, see TPDFColor  */
} TPDFFont, *PPDFFont;

/* Border Type. Specifies position of the annotation on the page. ( Acroform Object Characteristic )*/
typedef struct {
    TPDFRect    Rect;           /* Rectangle region for active area of Acro Form Object.
                                Four coordinates - left, top, right and bottom of border.
                                See TPDFRect */
    TPDFColor   BorderColor;    /* Color of border for displaying, see TPDFColor */
    TPDFColor   FillColor;      /* Color for filling inside area, background color */
    ppReal      Width;          /* Width of border line in points */
} TPDFBorder, *PPDFBorder;

/* Determination of Text object for setting on Page or Acroform Content */
typedef struct {
    char           *Caption;        /* Text string of label */
    TPDFFont        Font;           /* Font for displaying text, see TPDFFont */
    TPDFBorder      Border;         /* Border of text label, see TPDFBorder */
    TPDFAcroQuadding Align;         /* Alignment text label option, see TPDFAcroQuadding */
    ppReal          Orientation;    /* Incline level, angle in degrees */
} TPDFTextBox, *PPDFTextBox;

/* Determination of Variable Text object for setting on Acroform Content. For text entering from document.*/
typedef struct {
    char           *Name;       /* Name(text string) of Acroform object, Name (text string) of
                                   Acroform field is used to export when the PDF document is
                                   submitted.                                                  */
    ppUns32         Flag;       /* Specify the behavior of the annotation when is printed,
                                   rotated etc. AcroField Type ( see TPDFAcroFlags )       */
    char           *Caption;    /* Default text string for appearance in EditBox. Text is
                                   displayed in EditBox when control is created.          */
    TPDFFont        Font;       /* Text font for appearance text label. Attributes of text
                                written on or in the control. */
    TPDFBorder      Border;     /* EditBox rectangle specifies position of the annotation
                                on the page. Border width and colors. */
    ppUns32         MaxLen;     /* The maximum length of the field's text, in characters. */
    TPDFAcroQuadding Align;     /* Text alignment in edit box, justification of input text. */
    PDFPaintContent PaintContent; /* Pointer to overload function to repaint edit box, instead of
                                     default ( optional )                                         */
} TPDFEditBox, *PPDFEditBox;

/* Determination of Pushbutton object for setting on Acroform Content. Action selection. Submit action. */
typedef struct {
    char           *Name;               /* Name(text string) of Acroform object, Name (text string) of
                                           Acroform field is used to export when the PDF document is
                                           submitted.                                                  */
    ppUns32         Flag;               /* Specify the behavior of the annotation when is printed,
                                           rotated etc. AcroField Type ( see TPDFAcroFlags )       */
    char           *Caption;            /* Specifies a text string that identifies the control to
                                        the user. Text label for appearance on button. */
    TPDFFont        Font;               /* Text font for appearance text label. Attributes of text
                                        written on or in the control. */
    TPDFBorder      Border;             /* Pushbutton rectangle specifies position of the
                                        annotation on the page. Border width and colors. */
    ppReal          Miter;              /* Miter of pushbutton, bevel size. */
    PDFPaintContent PaintContentUp;     /* Pointer to overload function to repaint pushbutton in normal
                                           state ( optional ), instead of default appearance.           */
    PDFPaintContent PaintContentDown;   /* Pointer to overload function to repaint pushbutton in pressed
                                           state ( optional ), instead of default appearance             */
} TPDFPushButton, *PPDFPushButton;


/* Determination of CheckBox object and RadioButton object for setting on Acroform Content. Item selection. */
typedef struct {
    char           *Name;           /* Name(text string) of Acroform object, Name (text string) of
                                       Acroform field is used to export when the PDF document
                                       submitted.                                                  */
    ppUns32         Flag;           /* Specify the behavior of the annotation when is printed,
                                       rotated etc. AcroField Type ( see TPDFAcroFlags )       */
    TPDFFont        Font;           /* Text font for appearance text label. Attributes of
                                    text written on or in the control. Use only Font's color
                                    for displaying Mark character. */
    TPDFBorder      Border;         /* CheckBox or RadioButton rectangle specifies position
                                    of the annotation on the page. Border width and colors */
    TPDFCheckBoxStyle Style;        /* Style of CheckBox or RadioButton - rectangle or
                                    circle ( see TPDFCheckBoxStyle ). */
    TPDFCheckBoxSign Sign;          /* Code of Mark character in CheckBox or RadioButton
                                    ( see TPDFCheckBoxSign ). */
    ppBool          Value;          /* Value of CheckBox or RadioButton. Variable interactive
                                    value on Acroform ( see VSAcroForm.h ). */
    PDFPaintContent PaintContentOn; /* Pointer to overload function to repaint CheckBox in checked
                                       state ( optional ), instead of default appearance.          */
    PDFPaintContent PaintContentOff;/* Pointer to overload function to repaint CheckBox in unchecked
                                       state ( optional ), instead of default appearance.            */
} TPDFCheckBox, *PPDFCheckBox, TPDFRadioButton, *PPDFRadioButton;

/* Determination of ComboBox object and ListBox object for setting on Acroform Content.
Item(s) selection from items list. */
typedef struct {
    char           *Name;           /* Name(text string) of Acroform object, Name (text string)
                                    of Acroform field used for export when the PDF document
                                    submitted. */
    ppUns32         Flag;           /* Specify the behavior of the annotation when is printed,
                                       rotated etc. AcroField Type ( see TPDFAcroFlags )       */
    char           *Caption;        /* Default text string for appearance in ComboBox. Wasted in
                                       ListBox. Text is displayed in ComboBox when control is
                                       created.                                                  */
    TPDFFont        Font;           /* Text font for appearance text label. Attributes of
                                    text written on or in the control. */
    TPDFBorder      Border;         /* ComboBox or ListBox rectangle specifies position of
                                    the annotation on the page. Border width and colors. */
    PDFPaintContent PaintContent;   /* Pointer to overload function to repaint ComboBox ( optional
                                       ), instead of default.                                      */
} TPDFComboBox, *PPDFComboBox, TPDFListBox, *PPDFListBox, TPDFItemsBox;

/* Determination of Empty Signature object for setting on
   Acroform Content. Item to sign document                */
typedef struct {
    char           *Name;       /* Name(text string) of Acroform object, Name (text string) of
                                   Acroform field is used to export when the PDF document is
                                   submitted.                                                  */
    TPDFBorder      Border;     /* Signature specifies position of the annotation
                                on the page. */
    TPDFAcroSigFlags SigFlags;  /* A set of flags specifying various
                                document-level characteristics related to signature
                                fields. See TPDFAcroSigFlags */
    TPDFAnnotFlags  AnnotFlag;  /* Specify the behavior of the annotation when is printed,
                                   rotated etc. See TPDFAnnotFlags                         */
} TPDFSignature, *PPDFSignature;

/* Determination of Personal Invisible Signature object to sign
   Document                                                     */
typedef struct {
    char       *Name;       /* Name(text string) of Acroform object, Name (text string) of
                               Acroform field is used to export when the PDF document is
                               submitted.                                                  */
    char       *Owner;      /* Owner of Signature, Person Name. Text string,
                            for example "Ted Thompson" */
    char       *Reason;     /* Reason of Sign this document. Text string,
                            for example "I agree..." */
    ppBool      PKCS7;      /* Boolean flag of coding type :
                                    true    - 'Adobe.PPKMS' and 'adbe.pkcs7.sha1' crypt
                                            system sub filter
                                    false   - 'Adobe.PPKLite' and 'adbe.x509.rsa_sha1'
                                            crypt system sub filter */
    char       *FileName;   /* PFX Personal Signature FileName. Text string. */
    char       *Password;   /* Owner Password for Personal Signature. Text string. */
} TPDFDocumentSignature, *PPDFDocumentSignature;



/* Acro Form Object Field's Flags. Property of interactive object. */
#define PDFAcroFlags_Default                  0 /* Default flag without any features */
#define PDFAcroFlags_ReadOnly                 1 /* If set, the user may not change the value of the field.
                                                Any associated widget annotations will not interact with
                                                the user; that is, they will not respond to mouse clicks
                                                or change their appearance in response to mouse motions.
                                                This flag is useful for fields whose values are computed
                                                or imported from a database.*/
#define PDFAcroFlags_Required                 2 /* If set, the field must have a value at the time it is
                                                exported by a submit-form action */
#define PDFAcroFlags_NoExport                 4 /* If set, the field must not be exported by a submit-form
                                                action */
#define PDFAcroFlags_Multiline             4096 /* If set, the field may contain multiple lines of text;
                                                if clear, the field’s text is restricted to a single line.*/
#define PDFAcroFlags_Password              8192 /* If set, the field is intended for entering a secure
                                                password that should not be echoed visibly to the screen.
                                                Characters typed from the keyboard should instead be echoed
                                                in some unreadable form, such as asterisks or bullet
                                                characters. To protect password confidentiality, viewer
                                                applications should never store the value of the text field
                                                in the PDF file if this flag is set. */
#define PDFAcroFlags_NoToggleToOff        16384 /* For Radio buttons only. If set, exactly one radio button
                                                must be selected at all times; clicking the currently
                                                selected button has no effect. If clear, clicking the
                                                selected button deselects it, leaving no button selected.*/
#define PDFAcroFlags_Radio                32768 /* If set, the field is a set of radio buttons; if clear,
                                                the field is a check box. This flag is meaningful only if
                                                the Pushbutton flag is clear. */
#define PDFAcroFlags_Pushbutton           65536 /* If set, the field is a pushbutton that does not retain
                                                a permanent value. */
#define PDFAcroFlags_Combo               131072 /* If set, the field is a combo box; if clear, the field is
                                                a list box. */
#define PDFAcroFlags_Edit                262144 /* If set, the combo box includes an editable text box as
                                                well as a drop list; if clear, it includes only a drop list.
                                                This flag is meaningful only if the Combo flag is set. */
#define PDFAcroFlags_Sort                524288 /* If set, the field’s option items should be sorted
                                                alphabetically. This flag is intended for use by form
                                                authoring tools, not by PDF viewer applications; viewers
                                                should simply display the options in the order in which
                                                they occur in the items array */
#define PDFAcroFlags_FileSelect         1048576 /* If set, the text entered in the field represents
                                                the pathname of a file whose contents are to be submitted
                                                as the value of the field. */
#define PDFAcroFlags_MultiSelect        2097152 /* If set, more than one of the field’s option items may
                                                be selected simultaneously; if clear, no more than one
                                                item at a time may be selected. */
#define PDFAcroFlags_DoNotSpellCheck    4194304 /* If set, the text entered in the field will not be
                                                spell-checked. */
#define PDFAcroFlags_DoNotScroll        8388608 /* If set, the field will not scroll (horizontally for 
                                                single-line fields, vertically for multiple-line fields)
                                                to accommodate more text than will fit within its annotation
                                                rectangle. Once the field is full, no further text will be
                                                accepted. */
#define PDFAcroFlags_Comb              16777216 /* Meaningful only if the MaxLen entry is present in the
                                                text field dictionary and if the Multiline, Password, and
                                                FileSelect flags are clear. If set, the field is
                                                automatically divided up into as many equally spaced
                                                positions, or combs, as the value of MaxLen, and the text
                                                is laid out into those combs. */
#define PDFAcroFlags_RadiosInUnison    33554432 /* If set, a group of radio buttons within a radio button
                                                field that use the same value for the on state will turn on
                                                and off in unison; that is if one is checked, they are all
                                                checked. If clear, the buttons are mutually exclusive (the
                                                same behavior as HTML radio buttons). */
#define PDFAcroFlags_CommitOnSelChange 67108864 /* If set, the new value is committed as soon as
                                                a selection is made with the pointing device. This allows
                                                applications to perform an action once a selection is made,
                                                without requiring the user to exit the field. If clear,
                                                the new value is not committed until the user exits the
                                                field. */
#define PDFAcroFlags_RichText         134217728 /* If set, the value of this field should be represented
                                                as a rich text string. If the field has a value, the RV
                                                entry of the field dictionary specifies the rich text
                                                string. */


typedef enum {
	aftPushButton,
	aftEditBox,
	aftCheckBox,
	aftRadioButton,
	aftListBox,
	aftComboBox,
	aftSignatureField,
	aftInvisibleSignature
} TPDFAcroFieldType;


typedef struct {
	TPDFAcroFieldType AcroType;
	PDFDocHandle Doc;
	ppBool Stored;
	PDFCosHandle Handle;
	ppInt32 PageIndex;
	ppUns16 AnnotationFlag;
	union  {
		TPDFEditBox EditBox;
		TPDFPushButton PushButton;
		TPDFCheckBox CheckBox;
		TPDFRadioButton RadioButton; 
		TPDFListBox ListBox;
		TPDFComboBox ComboBox;
		TPDFSignature Signature;
	} ObjectInfo ;
	char **List;
	ppUns32 Size;
	char * Group;
} TPDFAcroObject, *PPDFAcroObject;




/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
    Active Features API
************************************************************************/



/* Description
   Sets Digital Signature Blank Field on Page Content for
   further sign (filling)
   Parameters
   Doc :        [in] Destination Document
   PageIndex :  [in] Page index ( as from 0 ) where Digital Signature
                will be stated.
   Signature :  [in] Digital Signature Structure which describes all
                needed fields ( see TPDFSignature ).
   Returns
   Index of acroform object in document.                              */
void PDFAcroSignatureInDocument( PDFDocHandle Doc, ppUns32 AcroIndex);


/* Description
   Sets Invisible Filled Digital Signature on first page. This
   signature is in PFX Digital Signature File.
   Parameters
   Doc :                [in] Destination Document
   DocumentSignature :  [in] Digital Signature Structure which describes
                        owner, reason and information about PFX file ( see
                        TPDFDocumentSignature ).
   Returns
   None.                                                                   */
void PDFAcroSignDocument( PDFDocHandle Doc, TPDFDocumentSignature DocumentSignature );



/* Description
   Sets EditBox on Page Content for text entering
   Parameters
   Doc :        [in] Destination Document.
   PageIndex :  [in] Page index ( as from 0 ) where EditBox will be
                stated.
   EditBox :    [in] EditBox Structure which describes all needed
                fields ( see TPDFEditBox ).
   Returns
   Index of acroform object in document.                            */
void PDFAcroEditBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Sets PushButton on Page Content for action control.
   Parameters
   Doc :         [in] Destination Document.
   PageIndex :   [in] Page index ( as from 0 ) where PushButton will
                 be stated.
   PushButton :  [in] PushButton Structure which describes all needed
                 fields ( see TPDFPushButton ).
   Returns
   Index of acroform object in document.                              */
void PDFAcroPushButtonInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Sets CheckBox on Page Content for item selection.
   Parameters
   Doc :        [in] Destination Document.
   PageIndex :  [in] Page index ( as from 0 ) where CheckBox will be
                stated.
   CheckBox :   [in] CheckBox Structure which describes all needed
                fields ( see TPDFCheckBox ).
   Returns
   Index of acroform object in document.                             */
void PDFAcroCheckBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Sets RadioButton on Page Content for item selection.
   Parameters
   Doc :          [in] Destination Document.
   PageIndex :    [in] Page index ( as from 0 ) where RadioButton will
                  be stated.
   GroupName :    [in] Name of Radio Buttons Group to which
                  RadioButton will be linked
   RadioButton :  [in] RadioButton Structure which describes all
                  needed fields ( see TPDFRadioButton ).
   Returns
   Index of acroform object in document.                               */
void PDFAcroRadioButtonInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Sets ComboBox on Page Content for item selection.
   Parameters
   Doc :        [in] Destination Document.
   PageIndex :  [in] Page index ( as from 0 ) where ComboBox will be
                stated.
   ComboBox :   [in] ComboBox Structure which describes all needed
                fields ( see TPDFComboBox ).
   See Also
   Array of text items(Opt) in ComboBox structure will be filled
   with function PDFAcroItemsBoxAdd.
   Returns
   Index of acroform object in document.                             */
void PDFAcroComboBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Sets ListBox on Page Content for item selection.
   Parameters
   Doc :        [in] Destination Document.
   PageIndex :  [in] Page index ( as from 0 ) where ListBox will be
                stated.
   ListBox :    [in] ListBox Structure which describes all needed
                fields ( see TPDFListBox ).
   See Also
   Array of text items(Opt) in ListBox structure will be filled
   with function PDFAcroItemsBoxAdd.
   Returns
   Index of acroform object in document.                            */
void PDFAcroListBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex );


/* Description
   Adds Text Item to items array of ComboBox or ListBox.
   Parameters
   Doc :              [in] Destination document.
   AcroObjectIndex :  [in] Index of acroform object in document ComboBox
                      or ListBox.
   String :           [in] Text string for addition in items array
   ItemsBox :         [in] ComboBox or ListBox Structure which describes
                      all needed fields ( see TPDFItemsBox ).
   See Also
   PDFAcroComboBoxInDocument, PDFAcroListBoxInDocument
   Returns
   None.                                                                 */
void PDFAcroItemsBoxAdd( PDFDocHandle Doc, ppUns32 AcroIndex, char *String);


void PDFAcroObjectDrawIt ( PDFDocHandle Doc, ppInt32 AcroIndex );

void PDFAcroObjectFree( PDFDocHandle Doc, ppInt32 AcroIndex );



void PBXDrawEditBox( PBXHandle PaintBox, PPDFEditBox EditBox );
void PBXDrawListBox( PBXHandle PaintBox, PPDFListBox ListBox, char **Opt, ppUns32 Size, ppUns32 Index, ppUns32 *TOff );
void PBXDrawCheckBox( PBXHandle PaintBox, PPDFCheckBox CheckBox, ppBool CheckBoxStatus );
void PBXDrawPushButtonDown( PBXHandle PaintBox, PPDFPushButton PushButton );
void PBXDrawPushButtonUp( PBXHandle PaintBox, PPDFPushButton PushButton );
void PBXDrawComboBox( PBXHandle PaintBox, PPDFComboBox ComboBox );



#ifdef __cplusplus
}
#endif


#endif /* #ifndef _H_VSACROOBJECTS */
