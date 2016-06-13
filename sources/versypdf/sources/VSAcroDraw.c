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
VSAcroDraw.c
*********************************************************************/




#include "VSCanvasAI.h"
#include "VSAcroObjects.h"
#include "VSMisc.h"
#include "VSDocAI.h"
#include "VSFontAI.h"



void PBXDrawStrechTextBox( PBXHandle PaintBox, TPDFTextBox TextBox, ppBool Stretch )
{
	ppInt32 FontIndex;
	ppReal    TextWidth, TextHeight;
	ppReal    x, y;

	//if ( TextBox.Caption == NULL )
	//	return;

	if ( TextBox.Font.ID.IsStdFont )
		FontIndex = PDFFontStandardAppend( _PDOC(PaintBox), TextBox.Font.ID.From.StandardFont,
		etWinAnsiEncoding );
	else
		FontIndex = TextBox.Font.ID.From.Index;

	PBXSetActiveFont( PaintBox, FontIndex, TextBox.Font.Size, false, false );
    if (TextBox.Caption)
		TextWidth = PBXGetTextWidth( PaintBox, TextBox.Caption );
	else 
		TextWidth  = 0;
	TextHeight = ((PPDFFontInfo)_PDOC(PaintBox)->FontList->FList[FontIndex])->metrics.Ascent *
		_PBOX(PaintBox)->CurrentFontSize / 1000;

	if ( Stretch ){
		while ( ( TextBox.Font.Size > 0 ) && 
			( ( TextWidth > ( TextBox.Border.Rect.xr - TextBox.Border.Rect.xl ) ) ||
			( TextHeight > ( ( TextBox.Border.Rect.yr - TextBox.Border.Rect.yl ) ) ) ) ) {
				TextBox.Font.Size = TextBox.Font.Size - 0.1;
				PBXSetActiveFont( PaintBox, FontIndex, TextBox.Font.Size, false, false );
				if (TextBox.Caption)
					TextWidth = PBXGetTextWidth( PaintBox, TextBox.Caption );
				else 
					TextWidth  = 0;
				TextWidth = PBXGetTextWidth( PaintBox, TextBox.Caption );
				TextHeight = ((PPDFFontInfo)_PDOC(PaintBox)->FontList->FList[FontIndex])->metrics.Ascent *
					_PBOX(PaintBox)->CurrentFontSize / 1000;
			}
	}

	PBXSetFillColor( PaintBox, TextBox.Font.Color );

	switch( TextBox.Align ) {
		case PDFAcroQuaddingLeftTop :
			x = TextBox.Border.Rect.xl;
			y = TextBox.Border.Rect.yl;
			break;
		case PDFAcroQuaddingTop :
			x = ( TextBox.Border.Rect.xr + TextBox.Border.Rect.xl - TextWidth  ) / 2;
			y = TextBox.Border.Rect.yl;
			break;
		case PDFAcroQuaddingRightTop :
			x = TextBox.Border.Rect.xr - TextWidth;
			y = TextBox.Border.Rect.yl;
			break;
		case PDFAcroQuaddingLeft :
			x = TextBox.Border.Rect.xl;
			y = ( TextBox.Border.Rect.yr + TextBox.Border.Rect.yl - TextHeight ) / 2;
			break;
		case PDFAcroQuaddingCenter :
			x = ( TextBox.Border.Rect.xr + TextBox.Border.Rect.xl - TextWidth  ) / 2;
			y = ( TextBox.Border.Rect.yr + TextBox.Border.Rect.yl - TextHeight ) / 2;
			break;
		case PDFAcroQuaddingRight :
			x = TextBox.Border.Rect.xr - TextWidth;
			y = ( TextBox.Border.Rect.yr + TextBox.Border.Rect.yl - TextHeight ) / 2;
			break;
		case PDFAcroQuaddingLeftBottom :
			x = TextBox.Border.Rect.xl;
			y = TextBox.Border.Rect.yr - TextHeight;
			break;
		case PDFAcroQuaddingBottom :
			x = ( TextBox.Border.Rect.xr + TextBox.Border.Rect.xl - TextWidth  ) / 2;
			y = TextBox.Border.Rect.yr - TextHeight;
			break;
		case PDFAcroQuaddingRightBottom :
			x = TextBox.Border.Rect.xr - TextWidth;
			y = TextBox.Border.Rect.yr - TextHeight;
			break;
		default:
			x = TextBox.Border.Rect.xl;
			y = TextBox.Border.Rect.yl;
	}
	PBXTextOut( PaintBox, x, y, TextBox.Orientation, TextBox.Caption?TextBox.Caption:"" );
}

void PBXDrawEditBox( PBXHandle PaintBox, PPDFEditBox EditBox )
{
	TPDFTextBox  TextBox;
	int i, l;
	char p[256]; 
	TPDFRect  Rect = PDFRect( 0, 0, _PBOX(PaintBox)->Width, _PBOX(PaintBox)->Height );

	PBXSetStrokeColor( PaintBox, EditBox->Border.BorderColor );
	PBXSetLineWidth( PaintBox, EditBox->Border.Width );
	PBXSetFillColor( PaintBox, EditBox->Border.FillColor );
	PBXRectangle( PaintBox, Rect.xl, Rect.yl, Rect.xr, Rect.yr );
	PBXFillAndStroke( PaintBox );

	ULStrToStrm ( _PBOX(PaintBox)->Strm, "/Tx BMC " );
	PBXStateStore( PaintBox );
	Rect = PDFRect( Rect.xl + 2, Rect.yl + 2, Rect.xr - 2, Rect.yr - 2 );
	PBXRectangle( PaintBox, Rect.xl, Rect.yl, Rect.xr, Rect.yr );
	PBXBeginText( PaintBox );

	TextBox.Align = EditBox->Align;
	TextBox.Font = EditBox->Font;

	TextBox.Border.Rect = Rect;
	TextBox.Orientation = 0;
	if ( EditBox->Flag & PDFAcroFlags_Password){
		l = (int)strlen(EditBox->Caption);
		for (i=0; i <l ; i++ )
			p[i] = '*';
		p[i] = '\0';
		TextBox.Caption = p;
	} else {
		TextBox.Caption = EditBox->Caption;
	}
	PBXDrawStrechTextBox( PaintBox, TextBox, false );	

	PBXEndText( PaintBox );
	PBXStateRestore( PaintBox );
	ULStrToStrm ( _PBOX(PaintBox)->Strm, "EMC " );
	if (!TextBox.Font.ID.IsStdFont){
		for (i = 32;i <=127;i++)
			FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[i]= true;
		for (i = 130;i <=140;i++)
			FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[i]= true;
		for (i = 145;i <=156;i++)
			FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[i]= true;
		for (i = 161;i <256;i++)
			FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[i]= true;
		
		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[128]= true;
		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[142]= true;
		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[158]= true;
		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->UsedChars[159]= true;

		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->FirstChar = 32;
		FONTDoc(_PBOX(PaintBox)->Doc, TextBox.Font.ID.From.Index)->FontInfo.TTFInfo->ANSI->LastChar = 255;
	}
}


void PBXDrawCheckBox( PBXHandle PaintBox, PPDFCheckBox CheckBox, ppBool CheckBoxStatus )
{
	TPDFTextBox    TextBox;
	TPDFRect    Rect = PDFRect( 0, 0, _PBOX(PaintBox)->Width, _PBOX(PaintBox)->Height );

	PBXStateStore( PaintBox );
	PBXSetLineWidth( PaintBox, CheckBox->Border.Width );
	PBXSetStrokeColor( PaintBox, CheckBox->Border.BorderColor );
	PBXSetFillColor( PaintBox, CheckBox->Border.FillColor );

	switch( CheckBox->Style ) {
	case cbfRectangle:
		PBXRectangle( PaintBox, Rect.xl, Rect.yl, Rect.xr, Rect.yr );
		break;
	case cbfCircle:
		PBXCircle( PaintBox, ( Rect.xl + Rect.xr ) / 2, ( Rect.yl + Rect.yr ) / 2,
			((( Rect.xl + Rect.xr ) > ( Rect.yl + Rect.yr ))?
			(( Rect.yl + Rect.yr ) / 2 - 2 ):(( Rect.xl + Rect.xr ) / 2 - 2 )));
		break;
	default:
		PBXRectangle( PaintBox, Rect.xl, Rect.yl, Rect.xr, Rect.yr );
	}
	PBXFillAndStroke( PaintBox );

	if ( CheckBoxStatus ) {
		TextBox.Align = PDFAcroQuaddingCenter;
		TextBox.Orientation = 0;
		TextBox.Border.Rect = PDFRect( 0, 0, _PBOX(PaintBox)->Width - 1, 0.78 * _PBOX(PaintBox)->Height );
		TextBox.Border.FillColor = SetNoPDFColor;
		TextBox.Border.BorderColor = SetNoPDFColor;
		TextBox.Font.ID.IsStdFont = true;
		TextBox.Font.ID.From.StandardFont = stdfZapfDingbats;
		TextBox.Font.Size = 0.75 * _PBOX(PaintBox)->Height;
		TextBox.Font.Color = CheckBox->Font.Color;
		TextBox.Caption = (char *)mmalloc( _PDOC(PaintBox)->Lib, 2 );
		TextBox.Caption[0] = ( char ) CheckBox->Sign;
		TextBox.Caption[1] = '\0';
		PBXDrawStrechTextBox( PaintBox, TextBox, true );
		mfree( _PDOC(PaintBox)->Lib, TextBox.Caption);
	}
	PBXStateRestore( PaintBox );
}

TPDFColor ULLightColor( TPDFColor SourceColor )
{
	TPDFColor ResultColor;

	ResultColor.Device = SourceColor.Device;
	switch( ResultColor.Device ) {
	case cgGray :
		ResultColor.Color.Gray = ( 1 + SourceColor.Color.Gray ) / 2;
		break;
	case cgRGB :
		ResultColor.Color.RGB.R = ( 1 + SourceColor.Color.RGB.R ) / 2;
		ResultColor.Color.RGB.G = ( 1 + SourceColor.Color.RGB.G ) / 2;
		ResultColor.Color.RGB.B = ( 1 + SourceColor.Color.RGB.B ) / 2;
		break;
	case cgCMYK :
		ResultColor.Color.CMYK.C = ( 1 + SourceColor.Color.CMYK.C ) / 2;
		ResultColor.Color.CMYK.M = ( 1 + SourceColor.Color.CMYK.M ) / 2;
		ResultColor.Color.CMYK.Y = ( 1 + SourceColor.Color.CMYK.Y ) / 2;
		ResultColor.Color.CMYK.K = ( 1 + SourceColor.Color.CMYK.K ) / 2;
		break;
	default:
		ResultColor = SetNoPDFColor;
	}
	return ResultColor;
}

TPDFColor ULDarkColor( TPDFColor SourceColor )
{
	TPDFColor ResultColor;

	ResultColor.Device = SourceColor.Device;
	switch( ResultColor.Device ) {
	case cgGray :
		ResultColor.Color.Gray = SourceColor.Color.Gray / 2;
		break;
	case cgRGB :
		ResultColor.Color.RGB.R = SourceColor.Color.RGB.R / 2;
		ResultColor.Color.RGB.G = SourceColor.Color.RGB.G / 2;
		ResultColor.Color.RGB.B = SourceColor.Color.RGB.B / 2;
		break;
	case cgCMYK :
		ResultColor.Color.CMYK.C = SourceColor.Color.CMYK.C / 2;
		ResultColor.Color.CMYK.M = SourceColor.Color.CMYK.M / 2;
		ResultColor.Color.CMYK.Y = SourceColor.Color.CMYK.Y / 2;
		ResultColor.Color.CMYK.K = SourceColor.Color.CMYK.K / 2;
		break;
	default:
		ResultColor = SetNoPDFColor;
	}
	return ResultColor;
}

void PBXDrawPushButton( PBXHandle PaintBox, PPDFPushButton PushButton, TPDFColor LeftTopColor, TPDFColor RightBottomColor )
{
	TPDFRect  Rect = PDFRect( 0, 0, _PBOX(PaintBox)->Width, _PBOX(PaintBox)->Height );

	PBXStateStore( PaintBox );

	PBXSetLineWidth( PaintBox, PushButton->Border.Width );

	PBXSetFillColor( PaintBox, LeftTopColor );
	PBXMoveTo( PaintBox, Rect.xl, Rect.yr );
	PBXLineTo( PaintBox, Rect.xl, Rect.yl );
	PBXLineTo( PaintBox, Rect.xr, Rect.yl );
	PBXLineTo( PaintBox, Rect.xr - PushButton->Miter, Rect.yl + PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xl + PushButton->Miter, Rect.yl + PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xl + PushButton->Miter, Rect.yr - PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xl, Rect.yr );
	PBXFill( PaintBox );

	PBXSetFillColor( PaintBox, RightBottomColor );
	PBXMoveTo( PaintBox, Rect.xl, Rect.yr );
	PBXLineTo( PaintBox, Rect.xr, Rect.yr );
	PBXLineTo( PaintBox, Rect.xr, Rect.yl );
	PBXLineTo( PaintBox, Rect.xr - PushButton->Miter, Rect.yl + PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xr - PushButton->Miter, Rect.yr - PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xl + PushButton->Miter, Rect.yr - PushButton->Miter );
	PBXLineTo( PaintBox, Rect.xl, Rect.yr );
	PBXFill( PaintBox );

	PBXSetFillColor( PaintBox, PushButton->Border.FillColor );
	PBXRectangle( PaintBox, Rect.xl + PushButton->Miter, Rect.yl + PushButton->Miter,
		Rect.xr - PushButton->Miter, Rect.yr - PushButton->Miter );
	PBXFill( PaintBox );

	if ( !IsNoPDFColor( PushButton->Border.BorderColor ) ) {
		PBXSetStrokeColor( PaintBox, PushButton->Border.BorderColor );
		PBXRectangle( PaintBox, Rect.xl, Rect.yl, Rect.xr, Rect.yr );
		PBXStroke( PaintBox );
	}
	PBXStateRestore( PaintBox );
}


void PBXDrawPushButtonUp( PBXHandle PaintBox, PPDFPushButton PushButton )
{
	TPDFColor       LeftTopColor, RightBottomColor;
	TPDFTextBox        TextBox;

	LeftTopColor = ULLightColor( PushButton->Border.FillColor );
	RightBottomColor = ULDarkColor( PushButton->Border.FillColor );
	PBXDrawPushButton( PaintBox, PushButton, LeftTopColor, RightBottomColor );
	TextBox.Border.Rect = PDFRect( PushButton->Miter, PushButton->Miter, 
		_PBOX(PaintBox)->Width - PushButton->Miter, _PBOX(PaintBox)->Height - PushButton->Miter );
	TextBox.Font = PushButton->Font;
	TextBox.Align = PDFAcroQuaddingCenter;
	TextBox.Orientation = 0;
	TextBox.Caption = PushButton->Caption;
	PBXDrawStrechTextBox( PaintBox, TextBox, true );
}

void PBXDrawPushButtonDown( PBXHandle PaintBox, PPDFPushButton PushButton )
{
	TPDFColor       LeftTopColor, RightBottomColor;
	TPDFTextBox        TextBox;

	LeftTopColor = ULDarkColor( PushButton->Border.FillColor );
	RightBottomColor = ULLightColor( PushButton->Border.FillColor );
	PushButton->Miter = PushButton->Miter * 2 / 3;
	PBXDrawPushButton( PaintBox, PushButton, LeftTopColor, RightBottomColor );
	TextBox.Font = PushButton->Font;
	TextBox.Align = PDFAcroQuaddingCenter;
	TextBox.Orientation = 0;
	TextBox.Caption = PushButton->Caption;
	TextBox.Border.Rect = PDFRect( PushButton->Miter, PushButton->Miter - 1, 
		_PBOX(PaintBox)->Width - PushButton->Miter, _PBOX(PaintBox)->Height - PushButton->Miter - 1 );
	PBXDrawStrechTextBox( PaintBox, TextBox, true );
}


void PBXDrawComboBox( PBXHandle PaintBox, PPDFComboBox ComboBox )
{
	TPDFEditBox        EditBox;

	EditBox.Align = PDFAcroQuaddingLeft;
	EditBox.Border = ComboBox->Border;
	EditBox.Caption = ComboBox->Caption;
	EditBox.Font = ComboBox->Font;
	PBXDrawEditBox( PaintBox, &EditBox );
}


ppReal ULStdListBoxLineHeight( TPDFFontID FontID )
{
	if ( FontID.IsStdFont ) {
		switch( FontID.From.StandardFont ) {
		case stdfHelvetica              : return 1.588;
		case stdfHelveticaBold          : return 1.635;
		case stdfHelveticaOblique       : return 1.588;
		case stdfHelveticaBoldOblique   : return 1.635;
		case stdfTimesRoman             : return 1.82;
		case stdfTimesBold              : return 1.82;
		case stdfTimesItalic            : return 1.794;
		case stdfTimesBoldItalic        : return 1.8;
		case stdfCourier                : return 1.518;
		case stdfCourierBold            : return 1.566;
		case stdfCourierOblique         : return 1.516;
		case stdfCourierBoldOblique     : return 1.566;
		case stdfSymbol                 : return 1.303;
		case stdfZapfDingbats           : return 0.963;
		default : return 1.6;
		}
	} else
		return 1.6;
}


void PBXDrawListBox( PBXHandle PaintBox, PPDFListBox ListBox, char **Opt, ppUns32 Size, ppUns32 Index, ppUns32 *TOff )
{
	ppInt32 FontIndex;
	ppUns32 i, lc, cnt, t;
	ppReal  FontHeight, LineHeight, TH;
	TPDFColor clr;

	PBXSetStrokeColor( PaintBox, ListBox->Border.BorderColor );
	PBXSetLineWidth( PaintBox, ListBox->Border.Width );
	PBXSetFillColor( PaintBox, ListBox->Border.FillColor );
	PBXRectangle( PaintBox, 0, 0, _PBOX(PaintBox)->Width, _PBOX(PaintBox)->Height );
	PBXFillAndStroke( PaintBox );

	if ( ListBox->Font.ID.IsStdFont )
		FontIndex = PDFFontStandardAppend( _PDOC(PaintBox), ListBox->Font.ID.From.StandardFont,
		etStandardEncoding );
	else
		FontIndex = ListBox->Font.ID.From.Index;

	PBXSetActiveFont( PaintBox, FontIndex, ListBox->Font.Size, false, false );
	FontHeight = ((PPDFFontInfo)_PDOC(PaintBox)->FontList->FList[FontIndex])->metrics.Ascent *
		_PBOX(PaintBox)->CurrentFontSize / 1000;
	LineHeight = FontHeight * ( ULStdListBoxLineHeight( ListBox->Font.ID ) - 1 );
	TH = LineHeight + FontHeight;
	lc = (ppUns32)(PBXGetHeight (PaintBox) / TH);

	ULStrToStrm ( _PBOX(PaintBox)->Strm, "/Tx BMC " );
	PBXStateStore( PaintBox );
	PBXSetFillColor( PaintBox, ListBox->Font.Color );
	if (Index == ppMAXUns32 || Index >= Size){
		cnt = min(Size, lc );
		PBXSetFillColor( PaintBox, ListBox->Font.Color );
		for ( i = 0; i < cnt; i++ ) {
			PBXTextOut (PaintBox, 1, TH*i+LineHeight/2,0, Opt[i]);
		}
		*TOff = ppMAXUns32;
	} else {
		t = 0;
		while ( Index >= t + lc) t+=lc;
		cnt=min(lc, Size-t);
		for ( i = 0; i < cnt; i++ ) {
			if (Index != i+t){
				PBXSetFillColor( PaintBox, ListBox->Font.Color );
				PBXTextOut (PaintBox, 1, TH*i+LineHeight/2,0, Opt[i+t]);
			} else {
				PBXEndText (PaintBox);
				PBXSetFillColor( PaintBox, ListBox->Font.Color );
				PBXNewPath (PaintBox);
				PBXRectangle (PaintBox, 0,TH * i, PBXGetWidth (PaintBox), (i+1)* TH+ LineHeight/2);
				PBXFill (PaintBox);
				clr.Device = ListBox->Font.Color.Device;
				clr.Color.CMYK.C = 1 - ListBox->Font.Color.Color.CMYK.C;
				clr.Color.CMYK.M = 1 - ListBox->Font.Color.Color.CMYK.M;
				clr.Color.CMYK.Y = 1 - ListBox->Font.Color.Color.CMYK.Y;
				clr.Color.CMYK.K = 1 - ListBox->Font.Color.Color.CMYK.K;
				PBXSetFillColor( PaintBox, clr );
				PBXTextOut (PaintBox, 1, TH*i+LineHeight/2,0, Opt[i+t]);
			}
		}
		*TOff = t;
	}
	PBXStateRestore( PaintBox );
	ULStrToStrm ( _PBOX(PaintBox)->Strm, " EMC\n" );
}
