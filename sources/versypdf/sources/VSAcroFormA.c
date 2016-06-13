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
VSAcroFormA.c
*********************************************************************/

#include "VSLibI.h"
#include "VSDocAI.h"
#include "VSAcroObjects.h"
#include "VSCanvasAI.h"
#include "VSFontAI.h"
#include "VSAcroObjects.h"

#include <string.h>


#define _DOC CD(Doc)
#define _LIB _DOC->Lib
#define _ACRO ((PPDFAcroObject) (_DOC->AcroFormList->FList [ AcroIndex ] ) )




void PDFAcroObjectSetBorder( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFColor BorderColor, TPDFColor FillColor, ppReal BorderWidth )
{
	PPDFBorder B = NULL;
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );


	switch( _ACRO->AcroType){
		case aftEditBox:
			 B = & _ACRO->ObjectInfo.EditBox.Border;
			break;
		case aftComboBox:
			B = & _ACRO->ObjectInfo.ComboBox.Border;
			break;
		case aftListBox:
			B = & _ACRO->ObjectInfo.ListBox.Border;
			break;
		case aftCheckBox:
			B = & _ACRO->ObjectInfo.CheckBox.Border;
			break;
		case aftRadioButton:
			B = & _ACRO->ObjectInfo.RadioButton.Border;
			break;
		case aftPushButton:
			B = & _ACRO->ObjectInfo.PushButton.Border;
			break;
		default:
			break;
		}
	if ( B ){
		B->BorderColor = BorderColor;
		B->FillColor = FillColor;
		B->Width = BorderWidth;
	};

}
#ifdef DELPHI
void PDFAcroObjectSetBorderM( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFColor *BorderColor, TPDFColor *FillColor, ppReal BorderWidth )
{
	PDFAcroObjectSetBorder ( Doc, AcroIndex, *BorderColor, *FillColor,  BorderWidth );
}
#endif

void PDFAcroObjectAppendItem ( PDFDocHandle Doc, ppUns32 AcroIndex, char * Item )
{
	ppUns32 SZ;
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if (  _ACRO->AcroType == aftComboBox || _ACRO->AcroType == aftListBox ){
		SZ = _ACRO->Size + 1;
		_ACRO->List = (char **) mrealloc ( CD(_ACRO->Doc)->Lib, _ACRO->List, SZ * sizeof ( char * ) );
		_ACRO->List[ _ACRO->Size ] = ( char * ) mmalloc ( CD(_ACRO->Doc)->Lib, strlen ( Item ) + 1 );
		strcpy ( _ACRO->List[ _ACRO->Size ], Item );
		_ACRO->Size  = SZ;
	}
}

void PDFAcroObjectSetCaption ( PDFDocHandle Doc, ppUns32 AcroIndex, char * Caption )
{
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	switch( _ACRO->AcroType){
		case aftEditBox:
			if ( _ACRO->ObjectInfo.EditBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.EditBox.Caption );
			_ACRO->ObjectInfo.EditBox.Caption = ( char * ) mmalloc ( CD(_ACRO->Doc)->Lib, strlen ( Caption ) + 1 );
			strcpy ( _ACRO->ObjectInfo.EditBox.Caption, Caption );			
			break;
		case aftComboBox:
			if ( _ACRO->ObjectInfo.ComboBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ComboBox.Caption );
			_ACRO->ObjectInfo.ComboBox.Caption = ( char * ) mmalloc ( CD(_ACRO->Doc)->Lib, strlen ( Caption ) + 1 );
			strcpy ( _ACRO->ObjectInfo.ComboBox.Caption, Caption );			
			break;
		case aftListBox:
			if ( _ACRO->ObjectInfo.ListBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ListBox.Caption );
			_ACRO->ObjectInfo.ListBox.Caption = ( char * ) mmalloc ( CD(_ACRO->Doc)->Lib, strlen ( Caption ) + 1 );
			strcpy ( _ACRO->ObjectInfo.ListBox.Caption, Caption );			
			break;
		case aftPushButton:
			if ( _ACRO->ObjectInfo.PushButton.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.PushButton.Caption );
			_ACRO->ObjectInfo.PushButton.Caption =( char * )  mmalloc ( CD(_ACRO->Doc)->Lib, strlen ( Caption ) + 1 );
			strcpy ( _ACRO->ObjectInfo.PushButton.Caption, Caption );			
			break;
		default:
			break;
	}
}

void PDFAcroObjectSetFont ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 FontIndex, ppReal FontSize, TPDFColor Color )
{
	PPDFFont F = NULL;
	PPDFFontInfo f;
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	switch( _ACRO->AcroType){

		case aftEditBox:
			 F = & _ACRO->ObjectInfo.EditBox.Font;
			break;
		case aftComboBox:
			F = & _ACRO->ObjectInfo.ComboBox.Font;
			break;
		case aftListBox:
			F = & _ACRO->ObjectInfo.ListBox.Font;
			break;
		case aftCheckBox:
			F = & _ACRO->ObjectInfo.CheckBox.Font;
			break;
		case aftRadioButton:
			F = & _ACRO->ObjectInfo.RadioButton.Font;
			break;
		case aftPushButton:
			F = & _ACRO->ObjectInfo.PushButton.Font;
			break;
		default:
			break;
		}
	if ( F ){
		F->Color = Color;
		F->Size = FontSize;
		if ( ! CD ( _ACRO->Doc )->FontList || CD ( _ACRO->Doc )->FontList->Size < FontIndex || FontIndex < 0 )
			return;
		f = FONTDoc ( _ACRO->Doc, FontIndex );
		if ( f->Type == PDFStdFont ){
			F->ID.IsStdFont = true;
			F->ID.From.StandardFont = f->FontInfo.StdInfo.ID;
		} else {
			F->ID.IsStdFont = false;
			F->ID.From.Index = FontIndex;
		}
	}
}

#ifdef DELPHI
void PDFAcroObjectSetFontM ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 FontIndex, ppReal FontSize, TPDFColor *Color )
{
	PDFAcroObjectSetFont ( Doc, AcroIndex, FontIndex, FontSize, *Color );
}
#endif

void PDFAcroObjectSetFlag ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 Flag )
{
	ppUns32 iflag = PDFAcroFlags_Default;
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );

	if ( Flag & PDFACRO_REQUIRED )
		iflag |= PDFAcroFlags_Required;
	if ( Flag & PDFACRO_READONLY )
		iflag |= PDFAcroFlags_ReadOnly;
	if ( Flag & PDFACRO_NOEXPORT )
		iflag |= PDFAcroFlags_NoExport;
	if ( Flag & PDFACRO_MULTILINE )
		iflag |= PDFAcroFlags_Multiline;
	if ( Flag & PDFACRO_PASSWORD )
		iflag |= PDFAcroFlags_Password;
	if ( Flag & PDFACRO_EDIT )
		iflag |= PDFAcroFlags_Edit;
	if ( Flag & PDFACRO_MULTISELECT )
		iflag |= PDFAcroFlags_MultiSelect;
	if ( Flag & PDFACRO_DONOTSPELLCHECK )
		iflag |= PDFAcroFlags_DoNotSpellCheck;
	if ( Flag & PDFACRO_DONOTSCROLL )
		iflag |= PDFAcroFlags_DoNotScroll;
	switch( _ACRO->AcroType){
		case aftEditBox:
			_ACRO->ObjectInfo.EditBox.Flag = iflag;
			break;
		case aftComboBox:
			_ACRO->ObjectInfo.ComboBox.Flag = iflag;
			break;
		case aftListBox:
			_ACRO->ObjectInfo.ListBox.Flag = iflag;
			break;
		case aftCheckBox:
			_ACRO->ObjectInfo.CheckBox.Flag = iflag;
			break;
		case aftRadioButton:
			_ACRO->ObjectInfo.RadioButton.Flag = iflag;
			break;
		case aftPushButton:
			_ACRO->ObjectInfo.PushButton.Flag = iflag;
			break;
		default:
			break;
	}
}

void PDFAcroObjectSetAnnotationFlag ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns16 AnnotationFlag  )
{
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	_ACRO->AnnotationFlag = AnnotationFlag;
}


void PDFAcroEditBoxSetMaxLen ( PDFDocHandle Doc, ppUns32 AcroIndex, ppUns32 MaxLen )
{
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if ( _ACRO->AcroType == aftEditBox ){
		_ACRO->ObjectInfo.EditBox.MaxLen = MaxLen;
	}
}

void PDFAcroEditBoxSetAlign ( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFAcroQuadding Align)
{
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if ( _ACRO->AcroType == aftEditBox ){
		_ACRO->ObjectInfo.EditBox.Align = Align;
	}
}

void PDFAcroObjectSetStyle ( PDFDocHandle Doc, ppUns32 AcroIndex, TPDFCheckBoxSign Sign, TPDFCheckBoxStyle Style )
{
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if ( _ACRO->AcroType == aftCheckBox ){
		_ACRO->ObjectInfo.CheckBox.Sign = Sign;
		_ACRO->ObjectInfo.CheckBox.Style = Style ;
	}

	if ( _ACRO->AcroType == aftRadioButton ){
		_ACRO->ObjectInfo.RadioButton.Sign = Sign;
		_ACRO->ObjectInfo.RadioButton.Style = Style ;
	}
}

ppUns32 PDFPageAppendPushButton ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition(_LIB, STD);
	if ( !_DOC->Pages )
		PDFDocLoadPages ( Doc );
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftPushButton;
		Acro->Doc = Doc;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.PushButton.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->Size = 0;
		Acro->AnnotationFlag = 4;
		Acro->List = NULL;
		Acro->ObjectInfo.PushButton.Flag = 0;
		Acro->ObjectInfo.PushButton.Border.Width = 1;
		Acro->ObjectInfo.PushButton.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.PushButton.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.PushButton.PaintContentDown = NULL;
		Acro->ObjectInfo.PushButton.PaintContentUp = NULL;
		Acro->ObjectInfo.PushButton.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.PushButton.Font.Size = 8;
		Acro->ObjectInfo.PushButton.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.PushButton.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.PushButton.Miter = 1;
		Acro->ObjectInfo.PushButton.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.PushButton.Name, Name );
		Acro->ObjectInfo.PushButton.Caption = NULL;
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}

ppUns32 PDFPageAppendRadioButton ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name,  char *Group, ppBool InitialState )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp, i;
	ppBool SetToFalse = false;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftRadioButton;
		Acro->Doc = Doc;
		Acro->ObjectInfo.RadioButton.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->Size = 0;
		Acro->List = NULL;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.RadioButton.Flag = 0;
		Acro->ObjectInfo.RadioButton.Border.Width = 1;
		Acro->ObjectInfo.RadioButton.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.RadioButton.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.RadioButton.PaintContentOff = NULL;
		Acro->ObjectInfo.RadioButton.PaintContentOn = NULL;
		Acro->ObjectInfo.RadioButton.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.RadioButton.Font.Size = 8;
		Acro->ObjectInfo.RadioButton.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.RadioButton.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.RadioButton.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.RadioButton.Name, Name );
		Acro->Group = ( char * ) mmalloc ( _LIB, strlen ( Group ) + 1 );
		strcpy( Acro->Group, Group );
		Acro->ObjectInfo.RadioButton.Value = InitialState;
		Acro->ObjectInfo.RadioButton.Sign = cbsX;
		Acro->ObjectInfo.RadioButton.Style = cbfRectangle ;
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
		if (InitialState && Group){
			for ( i = 0; i < Tmp; i++){
				if (((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->AcroType ==aftRadioButton){
					if (((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->Group && !strcmp(((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->Group,Group)){
						if (((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->ObjectInfo.RadioButton.Value){
							if (((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->Stored)
								SetToFalse = true;
							else 
								((TPDFAcroObject * )_DOC->AcroFormList->FList[i])->ObjectInfo.RadioButton.Value = false;

						}
					}
				}

			}
			if (SetToFalse)
				Acro->ObjectInfo.RadioButton.Value = false;

		}
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}

ppUns32 PDFPageAppendCheckBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name, ppBool InitialState )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftCheckBox;
		Acro->AnnotationFlag = 4;
		Acro->Doc = Doc;
		Acro->ObjectInfo.CheckBox.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->Size = 0;
		Acro->List = NULL;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.CheckBox.Flag = 0;
		Acro->ObjectInfo.CheckBox.Border.Width = 1;
		Acro->ObjectInfo.CheckBox.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.CheckBox.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.CheckBox.PaintContentOff = NULL;
		Acro->ObjectInfo.CheckBox.PaintContentOn = NULL;
		Acro->ObjectInfo.CheckBox.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.CheckBox.Font.Size = 8;
		Acro->ObjectInfo.CheckBox.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.CheckBox.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.CheckBox.Value = InitialState;
		Acro->ObjectInfo.CheckBox.Sign = cbsX;
		Acro->ObjectInfo.CheckBox.Style = cbfRectangle ;
		Acro->ObjectInfo.CheckBox.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.CheckBox.Name, Name );
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}

ppUns32 PDFPageAppendListBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftListBox;
		Acro->Doc = Doc;
		Acro->ObjectInfo.ListBox.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->Size = 0;
		Acro->AnnotationFlag = 4;
		Acro->List = NULL;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.ListBox.Flag = 0;
		Acro->ObjectInfo.ListBox.Border.Width = 1;
		Acro->ObjectInfo.ListBox.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.ListBox.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.ListBox.Caption = NULL;
		Acro->ObjectInfo.ListBox.PaintContent = NULL;
		Acro->ObjectInfo.ListBox.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.ListBox.Font.Size = 8;
		Acro->ObjectInfo.ListBox.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.ListBox.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.ListBox.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.ListBox.Name, Name );
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}


ppUns32 PDFPageAppendComboBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftComboBox;
		Acro->Doc = Doc;
		Acro->AnnotationFlag = 4;
		Acro->ObjectInfo.ComboBox.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->Size = 0;
		Acro->List = NULL;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.ComboBox.Flag = 0;
		Acro->ObjectInfo.ComboBox.Border.Width = 1;
		Acro->ObjectInfo.ComboBox.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.ComboBox.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.ComboBox.Caption = NULL;
		Acro->ObjectInfo.ComboBox.PaintContent = NULL;
		Acro->ObjectInfo.ComboBox.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.ComboBox.Font.Size = 8;
		Acro->ObjectInfo.ComboBox.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.ComboBox.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.ComboBox.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.ComboBox.Name, Name );
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;

}

ppUns32 PDFPageAppendEditBox (PDFDocHandle Doc,  ppUns32 Page, TPDFRect R, char * Name  )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftEditBox;
		Acro->Doc = Doc;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.EditBox.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->AnnotationFlag = 4;
		Acro->ObjectInfo.EditBox.Flag = 0;
		Acro->ObjectInfo.EditBox.Border.Width = 1;
		Acro->ObjectInfo.EditBox.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.EditBox.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.EditBox.Caption = NULL;
		Acro->ObjectInfo.EditBox.MaxLen = 0;
		Acro->ObjectInfo.EditBox.Align = PDFAcroQuaddingLeftTop;
		Acro->ObjectInfo.EditBox.PaintContent = NULL;
		Acro->ObjectInfo.EditBox.Font.Color = ULGrayToColor( 0 );
		Acro->ObjectInfo.EditBox.Font.Size = 8;
		Acro->ObjectInfo.EditBox.Font.ID.IsStdFont = true;
		Acro->ObjectInfo.EditBox.Font.ID.From.StandardFont = stdfHelvetica;
		Acro->ObjectInfo.EditBox.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.EditBox.Name, Name );
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}



void PDFAcroObjectFree( PDFDocHandle Doc, ppInt32 AcroIndex )
{
	ppUns32 i;
	switch( _ACRO->AcroType){
		case aftEditBox:
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.EditBox.Name );
			if ( _ACRO->ObjectInfo.EditBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.EditBox.Caption );
			break;
		case aftPushButton: 
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.PushButton.Name );
			if ( _ACRO->ObjectInfo.PushButton.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.PushButton.Caption );
			break;
		case aftComboBox:
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ComboBox.Name );
			if ( _ACRO->ObjectInfo.ComboBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ComboBox.Caption );
			for ( i = 0; i < _ACRO->Size; i++ ){
				mfree ( CD(_ACRO->Doc)->Lib, _ACRO->List[i] );
			}
			if ( _ACRO->Size )
				mfree ( CD(_ACRO->Doc)->Lib, _ACRO->List );
			break;
		case aftListBox:
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ListBox.Name );
			if ( _ACRO->ObjectInfo.ListBox.Caption )
				mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.ListBox.Caption );
			for ( i = 0; i < _ACRO->Size; i++ ){
				mfree ( CD(_ACRO->Doc)->Lib, _ACRO->List[i] );
			}
			if ( _ACRO->Size )
				mfree ( CD(_ACRO->Doc)->Lib, _ACRO->List );
			break;
		case aftCheckBox:
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.CheckBox.Name );
			break;
		case aftRadioButton:
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->ObjectInfo.RadioButton.Name );
			mfree( CD(_ACRO->Doc)->Lib, _ACRO->Group );
			break;
		default:
			break;
	}
	mfree( CD(_ACRO->Doc)->Lib, _ACRO );
}

void PDFAcroPushButtonSetMiter ( PDFDocHandle Doc, ppUns32 AcroIndex, ppReal Miter )
{
	if ( _ACRO->AcroType == aftPushButton ){
		_ACRO->ObjectInfo.PushButton.Miter = Miter;
	}
}

void PDFAcroObjectDrawIt ( PDFDocHandle Doc, ppInt32 AcroIndex )
{
	if ( _ACRO->Stored )
		return;
	switch( _ACRO->AcroType){
		case aftEditBox:
			PDFAcroEditBoxInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftPushButton:
			PDFAcroPushButtonInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftRadioButton:
			PDFAcroRadioButtonInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftCheckBox:
			PDFAcroCheckBoxInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftComboBox:
			PDFAcroComboBoxInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftListBox:
			PDFAcroListBoxInDocument ( _ACRO->Doc, AcroIndex );
			break;
		case aftSignatureField:
			PDFAcroSignatureInDocument( _ACRO->Doc, AcroIndex );
			break;
		default:
			break;
	}
	_ACRO->Stored = true;
}

ppUns32 PDFPageAppendSignatureBox ( PDFDocHandle Doc, ppUns32 Page, TPDFRect R, char * Name )
{
	TPDFAcroObject * Acro;
	ppUns32 Tmp;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, STD);
	if (Page >= _DOC->Pages->Size )
		_RAISE( ErrGeneralLevel, gleOutOfBoundError);
	Acro = (TPDFAcroObject * ) mmalloc ( _LIB, sizeof ( TPDFAcroObject ) );
	PDFTRY ( _LIB ){
		Acro->AcroType = aftSignatureField;
		Acro->Doc = Doc;
		Acro->Stored = false;
		_CosNullNew( Doc, Acro->Handle );
		Acro->ObjectInfo.Signature.Border.Rect = R;
		Acro->PageIndex = Page;
		Acro->AnnotationFlag = 4;
		Acro->ObjectInfo.Signature.SigFlags = 0;
		Acro->ObjectInfo.Signature.AnnotFlag = 0;
		Acro->ObjectInfo.Signature.Border.Width = 1;
		Acro->ObjectInfo.Signature.Border.BorderColor = ULGrayToColor( 0 );
		Acro->ObjectInfo.Signature.Border.FillColor = ULGrayToColor ( 1 );
		Acro->ObjectInfo.Signature.Name = ( char * ) mmalloc ( _LIB, strlen ( Name ) + 1 );
		strcpy( Acro->ObjectInfo.Signature.Name, Name );
		if ( !_DOC->AcroFormList )
			_DOC->AcroFormList = ULListCreate( _LIB, 1 );
		Tmp = ULListAdd ( _DOC->AcroFormList, Acro );
	} PDFEXCEPT ( _LIB ) {
		mfree( _LIB, Acro );
	} PDFTRYEND ( _LIB );
	return Tmp ;
}
#ifdef DELPHI
ppUns32 PDFPageAppendPushButtonM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name ){
	return PDFPageAppendPushButton ( Doc, Page, *R, Name );
}
ppUns32 PDFPageAppendSignatureBoxM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name ){
	return PDFPageAppendSignatureBox (Doc, Page, *R, Name);
}
ppUns32 PDFPageAppendEditBoxM (PDFDocHandle Doc,  ppUns32 Page, TPDFRect *R, char * Name  ){
	return PDFPageAppendEditBox ( Doc, Page, *R, Name );
}

ppUns32 PDFPageAppendComboBoxM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name ){
	return PDFPageAppendComboBox ( Doc, Page, *R, Name );
}
ppUns32 PDFPageAppendListBoxM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name ){
	return PDFPageAppendListBox ( Doc, Page, *R, Name );
}

ppUns32 PDFPageAppendCheckBoxM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name, ppBool InitialState ){
	return PDFPageAppendCheckBox ( Doc, Page, *R, Name, InitialState );
}

ppUns32 PDFPageAppendRadioButtonM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect *R, char * Name,  char *Group, ppBool InitialState ){
	return PDFPageAppendRadioButton ( Doc, Page, *R, Name, Group, InitialState );
}
#endif

#ifndef NOT_USE_SIGN

void PDFDocAppendSignatureFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 Length, char * Name, 
											  char * Reason, ppBool PKCS7, char * Password )
{
	PDFPfxHandle    pfxMySignature;
	sig_Context    *SignatureContext;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, PRO);
	SignatureContext = ( sig_Context * ) PDFDocPrepSign( Doc );
	sig_SetSigner( SignatureContext, "" );
	sig_SetReason( SignatureContext, Reason );
	sig_SetField( SignatureContext, Name );
	SignatureContext->isPKCS7 = PKCS7;
	SignatureContext->PageIndex = 0;
	pfxMySignature = PDFPfxLoadFromBuffer( _LIB, Buffer, Length, (ppUns8 *)Password);
	PDFTRY ( _LIB ){
		PDFDocSign( Doc, pfxMySignature);
	} PDFFINALLY ( _LIB ){
		PDFPfxDispose ( _LIB, pfxMySignature );
	} PDFFINALLYEND ( _LIB );	
}

void PDFDocAppendSignatureFromFile ( PDFDocHandle Doc, char * FileName, char * Name, 
											char * Reason, ppBool PKCS7, char * Password )
{
	PDFPfxHandle    pfxMySignature;
	sig_Context    *SignatureContext;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, PRO);
	SignatureContext = ( sig_Context * ) PDFDocPrepSign( Doc );
	sig_SetSigner( SignatureContext, "" );
	sig_SetReason( SignatureContext, Reason );
	sig_SetField( SignatureContext, Name );
	SignatureContext->isPKCS7 = PKCS7;
	SignatureContext->PageIndex = 0;
	pfxMySignature = PDFPfxLoadFromFile( _LIB, FileName, (ppUns8 *)Password);
	PDFTRY ( _LIB ){
		PDFDocSign( Doc, pfxMySignature);
	} PDFFINALLY ( _LIB ){
		PDFPfxDispose ( _LIB, pfxMySignature );
	} PDFFINALLYEND ( _LIB );	
}

void PDFDocAppendSignatureFromStream ( PDFDocHandle Doc, PDFStreamHandle Stream, char * Name, 
											  char * Reason, ppBool PKCS7, char * Password )
{
	PDFPfxHandle    pfxMySignature;
	sig_Context    *SignatureContext;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition (_LIB, PRO);
	SignatureContext = ( sig_Context * ) PDFDocPrepSign( Doc );
	sig_SetSigner( SignatureContext, "" );
	sig_SetReason( SignatureContext, Reason );
	sig_SetField( SignatureContext, Name );
	SignatureContext->isPKCS7 = PKCS7;
	SignatureContext->PageIndex = 0;
	pfxMySignature = PDFPfxLoadFromStream( _LIB,Stream, (ppUns8 *)Password);
	PDFTRY ( _LIB ){
		PDFDocSign( Doc, pfxMySignature);
	} PDFFINALLY ( _LIB ){
		PDFPfxDispose ( _LIB, pfxMySignature );
	} PDFFINALLYEND ( _LIB );	
}
#define SetAcroForm(Obj) Obj = _CosDictValueByName(_DOC->Root, AcroForm ); \
	if (! _IsCosDict(Obj))\
	_CosDictAppend( _DOC->Root,AcroForm,Obj = CosDictNew(Doc,false,5));\
	if( !_IsCosDict(_CosDictValueByName(Obj, DR)))\
	_CosDictAppend(Obj, DR, CosDictNew(Doc,false,5));

PBXHandle PDFPagePlaceSignature ( PDFDocHandle Doc, ppUns32 Page, TPDFRect Rect, ppUns32 Resolution)
{
	PPBX         PaintContent;
	PDFCosHandle StreamObject, ArrayObject, Obj, RS,XO, WRKStrm;
	ppReal          bbx, bby;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	if ( !_DOC->Signed)
		_RAISE( ErrDocumentLevel, dleSignatureNotAppendedError );
	bbx = fabs( ( ppReal ) ( Rect.xr - Rect.xl ) );
	bby = fabs( ( ppReal ) ( Rect.yr - Rect.yl ) );
	StreamObject = CosStreamNew( Doc, true, 0 );
	_CosDictAppend( StreamObject, Type, CosNameNew( Doc, false, GetAtomDoc( XObject ) ) );
	_CosDictAppend( StreamObject, Subtype, CosNameNew( Doc, false, GetAtomDoc( Form ) ) );

	_CosDictAppend( StreamObject, Resources, RS = CosDictNew( Doc, true, 0 ) );
	_CosDictAppend( StreamObject, BBox, ArrayObject = CosArrayNew( Doc, false, 6 ) );
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bbx ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bby ));
	SetAcroForm(Obj);

	WRKStrm = CosStreamNew( Doc, true, 0);
	
	_CosDictAppend(RS,XObject, XO = CosDictNew( Doc, false, 1));
	CosDictAppend(XO, ULStringToAtom(_LIB,"FRM"), WRKStrm);
	ULStrToStrm( _CosStreamValue(StreamObject),"/FRM Do");
	_CosDictAppend(StreamObject,Length, CosIntNew(Doc,false,7));


	_CosDictAppend( WRKStrm, Type, CosNameNew( Doc, false, GetAtomDoc( XObject ) ) );
	_CosDictAppend( WRKStrm, Subtype, CosNameNew( Doc, false, GetAtomDoc( Form ) ) );

	_CosDictAppend( WRKStrm, Resources, RS = CosDictNew( Doc, true, 0 ) );
	_CosDictAppend( WRKStrm, BBox, ArrayObject = CosArrayNew( Doc, false, 6 ) );
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0.0 ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bbx ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, bby ));
	_CosDictAppend( WRKStrm, Matrix, ArrayObject = CosArrayNew( Doc, false, 6 ) );
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 1  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 1  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));
	CosArrayAppend( ArrayObject, CosRealNew( Doc, false, 0  ));

	PaintContent = (PPBX)mmalloc ( _LIB, sizeof ( TPBX ) );
	PaintContent->Doc = Doc;
	PaintContent->Res = RS;
	_CosDictAppend( PaintContent->Res, ProcSet, _CosNameNew(PDF) );
	PaintContent->cs  = WRKStrm;
	Obj= PaintContent->AcroformRes = _CosDictValueByName(Obj, DR);
	PaintContent->Strm = CosStreamGetValue ( WRKStrm );
	PaintContent->Resolution = Resolution;
	PaintContent->Width = bbx;
	PaintContent->Height = bby;
	PaintContent->BaseLine = false;
	PaintContent->E2I = 72.0 / (ppReal) Resolution;
	PaintContent->I2E = ( ppReal ) Resolution / 72.0;
	PaintContent->CharSpace = 0.0;
	PaintContent->WordSpace = 0.0;
	PaintContent->HorizontalScaling = 100.0;
	PaintContent->CurrentFontIndex = 0;
	PaintContent->CurrentFontSize = 10.0;
	PaintContent->TextInited = false;
	PaintContent->TextUsed = false;
	PaintContent->GrayUsed = false;
	PaintContent->ColorUsed = false;
	PaintContent->TextAngle = 0.0;
	PaintContent->UnderLine = false;
	PaintContent->StrikeOut = false;
	PaintContent->FontInited = false;
	PaintContent->Unicode = false;
	PaintContent->CurrentFontIndex = ppMAXUns32;
	PaintContent->TY = 0.0;
	PaintContent->TX = 0.0;
	PaintContent->ResourceFontIndex = 0;
	PaintContent->ResourceImageIndex = 0;
	PaintContent->ResourceGStateIndex = 0;
	PaintContent->FontIsChanged = true;
	PaintContent->SaveCount = 0;
	PaintContent->WT.YScale = 1.0;
	PaintContent->WT.XScale = 1.0;
	PaintContent->NextOperatorChar = ' ';
	PaintContent->Rotate = pra0;
	PaintContent->FS = 1;
	PBXSetAngle( PaintContent, PaintContent->Rotate );
	_CosNullNew( Doc, PaintContent->AcroformRes );
	CDebugCode (
		PaintContent->DBG = NULL;
	);
	_DOC->SigCtx->IsVisible = true;
	_DOC->SigCtx->PageIndex = Page;
	_DOC->SigCtx->Rect = Rect;
	_DOC->SigCtx->VisibleContent = StreamObject;
	return  (PBXHandle)PaintContent;
}
#ifdef DELPHI
PBXHandle PDFPagePlaceSignatureM ( PDFDocHandle Doc, ppUns32 Page, TPDFRect * R, ppUns32 Resolution)
{
	PDFPagePlaceSignature( Doc, Page, *R, Resolution);
}
#endif

#endif

