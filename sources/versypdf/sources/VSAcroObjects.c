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
VSAcroObjects.c
*********************************************************************/

#include "VSAcroObjects.h"
#include "VSCanvasAI.h"
#include "VSDocAI.h"
#include "VSFontAI.h"
#include "VSMisc.h"


#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib
#define _ACRO ((PPDFAcroObject) (_DOC->AcroFormList->FList [ AcroIndex ] ) )

typedef struct TPDFByteRange{
    ppInt32     Offset;
    ppInt32     Length;
} TPDFByteRange, *PPDFByteRange;

typedef struct TPDFByteRanges{
    ppInt32         Size;
    PPDFByteRange   Array;
} TPDFByteRanges, *PPDFByteRanges;
 
typedef struct TPDFDataString{
    ppInt32 Size;
    void   *Data;
} TPDFDataString, *PPDFDataString;

typedef struct TPDFPersonSign{
    TPDFByteRanges  ByteRange; /* Required */
    TPDFDataString  Contents;  /* Required */
    char           *Filter;    /* Required */
    char           *SubFilter;
    TPDFDataString  Cert;
    char           *Person;    /* Optional Name */
    char           *ContactInfo;
    char           *Location;
    char           *SignDate;  /* Optional M */
    char           *Reason;
} TPDFPersonSign, *PPDFPersonSign;


#define SetAcroForm(Obj) Obj = _CosDictValueByName(_DOC->Root, AcroForm ); \
	if (! _IsCosDict(Obj))\
		_CosDictAppend( _DOC->Root,AcroForm,Obj = CosDictNew(Doc,false,5));\
	if( !_IsCosDict(_CosDictValueByName(Obj, DR)))\
		_CosDictAppend(Obj, DR, CosDictNew(Doc,false,5));


PDFCosHandle PDFAddAPXObject( PDFCosHandle Object, TPDFAppearanceStreamType Type, char *Name, TPDFRect Rect )
{
	PDFCosHandle Obj, wrk, AP;
	PDFDocHandle Doc;
	ppAtom		 N;

	Doc = _CosDoc(Object);

	AP = _CosDictValueByName( Object, AP );
	if ( !_IsCosDict( AP ) )
		_CosDictAppend( Object, AP, AP = CosDictNew( Doc, false, 1 ) );

	switch( Type ) {
		case apsRollover :
			N = GetAtomDoc( R );
			break;
		case apsDown :
			N =GetAtomDoc( D );
			break;
		case apsNormal :
		default : N =  GetAtomDoc( N );
	}
	Obj = CosStreamNew (Doc, true, 5);
	if (!Name){
		CosDictAppend(AP,N,Obj);
	} else {
		wrk = CosDictValueByName( AP,N);
		if (!_IsCosDict(wrk))
			CosDictAppend(AP,N,wrk=CosDictNew(Doc, false,2));
		CosDictAppend (wrk,ULStringToAtom (_LIB,Name), Obj );
	}
	_CosDictAppend(Obj,Type, _CosNameNew(XObject) );
	_CosDictAppend(Obj,Subtype, _CosNameNew(Form) );
	_CosDictAppend(Obj, Matrix, wrk= CosArrayNew (Doc, false,6));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 1));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 1));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	CosArrayAppend (wrk, CosRealNew (Doc, false, 0));
	Rect.xr = Rect.xr - Rect.xl;
	Rect.yr = Rect.yr - Rect.yl;
	Rect.xl = 0;
	Rect.yl = 0;
	_CosDictAppend(Obj, BBox, wrk= CosArrayNew (Doc, false,4));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Rect.xl));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Rect.yl));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Rect.xr));
	CosArrayAppend (wrk, CosRealNew (Doc, false, Rect.yr));
	_CosDictAppend (Obj, Resources, CosDictNew(Doc,false,0));
	return Obj;
}

PDFCosHandle PDFAddCosAcroField ( PDFCosHandle pParent, ppUns32 pPage, TPDFAcroFieldFlags pFieldType,
								 TPDFAcroFlags pFieldFlag, char *pFieldName, char *pFieldValue,
								 TPDFRect pFieldRect, TPDFAnnotFlags pAnnotFlag, ppUns8 pAnnotQuadding )
{
	PDFDocHandle Doc;
	PDFCosHandle AcroFieldsObject, AcroFieldObject, ItemFormObject, PageObject;
	ppAtom       AtomName;
	ppBool       IsItKid = true;

	Doc = _CosDoc(pParent);
	AcroFieldObject = CosNullNew( Doc );

	if ( !_DOC->Pages )
		PDFDocLoadPages ( Doc );
	if (pPage >= _DOC->Pages->Size )
		return AcroFieldObject;
	PageObject = ( ( PPDFPageInfo ) _DOC->Pages->FList[ pPage ] )->PO;

	AcroFieldsObject = _CosDictValueByName( pParent, Fields );
	if ( !_IsCosArray( AcroFieldsObject ) ) {
		AcroFieldsObject = _CosDictValueByName( pParent, Kids );
		if ( !_IsCosArray( AcroFieldsObject ) ) {
			AcroFieldsObject = CosArrayNew( Doc, false, 0 );
			_CosDictAppend( pParent, Kids, AcroFieldsObject );
		}
	} else
		IsItKid = false;

	AcroFieldObject = CosDictNew( Doc, true, 16 );

	ItemFormObject = CosNameNew( Doc, false, GetAtomDoc( Annot ) );
	_CosDictAppend( AcroFieldObject, Type, ItemFormObject );

	ItemFormObject = CosNameNew( Doc, false, GetAtomDoc( Widget ) );
	_CosDictAppend( AcroFieldObject, Subtype, ItemFormObject );

	if ( !IsAcroFieldFlagNull( pFieldType ) ) {
		switch( AcroFieldFlagGroup( pFieldType ) ) {
			case AcroField_Flag_Button    : AtomName = GetAtomDoc( Btn ); break;
			case AcroField_Flag_Text      : AtomName = GetAtomDoc( Tx );  break;
			case AcroField_Flag_Choice    : AtomName = GetAtomDoc( Ch );  break;
			case AcroField_Flag_Signature : AtomName = GetAtomDoc( Sig ); break;
			default                       : AtomName = GetAtomDoc( Tx );
		}

		ItemFormObject = CosNameNew( Doc, false, AtomName );
		_CosDictAppend( AcroFieldObject, FT, ItemFormObject );
	}

	if ( IsItKid ) {
		ItemFormObject = CosNewRef( Doc, _CosObjID( pParent ), _CosObjGeneration( pParent ) );
		_CosDictAppend( AcroFieldObject, Parent, ItemFormObject );
	}

	ItemFormObject = CosNewRef( Doc, _CosObjID( PageObject ), _CosObjGeneration( PageObject ) );
	_CosDictAppend( AcroFieldObject, P, ItemFormObject );

	ItemFormObject = _CosDictValueByName( PageObject, Annots );
	if ( !_IsCosArray( ItemFormObject ) ) {
		ItemFormObject = CosArrayNew( Doc, false, 0 );
		_CosDictAppend( PageObject, Annots, ItemFormObject );
	}
	CosArrayAppend( ItemFormObject, CosNewRef( Doc, _CosObjID( AcroFieldObject ), _CosObjGeneration( AcroFieldObject ) ) );

	if ( pFieldName != NULL ) {
		ItemFormObject = CosStringNew( Doc, false, pFieldName, (ppInt32)strlen( pFieldName ) );
		_CosDictAppend( AcroFieldObject, T, ItemFormObject );
	}

	if ( pFieldValue != NULL ) {
		if ( ( IsAcroFieldFlagCheckbox( pFieldType ) ) ||
			( IsAcroFieldFlagRadioButton( pFieldType ) ) ) {
				/* Name value */
				AtomName = ULStringToAtom( _LIB, pFieldValue );
				ItemFormObject = CosNameNew( Doc, false, AtomName );
				_CosDictAppend ( AcroFieldObject, V, ItemFormObject );
				ItemFormObject = CosNameNew( Doc, false, AtomName );
				_CosDictAppend ( AcroFieldObject, DV, ItemFormObject );
				ItemFormObject = CosNameNew( Doc, false, AtomName );
				_CosDictAppend ( AcroFieldObject, AS, ItemFormObject );
			} else
				if ( ( IsAcroFieldFlagText( pFieldType ) ) ||
					( IsAcroFieldFlagComboBox( pFieldType ) ) ||
					( IsAcroFieldFlagListBox( pFieldType ) ) ) {
						/* String value */
						ItemFormObject = CosStringNew( Doc, false, pFieldValue, (ppInt32)strlen( pFieldValue ) );
						_CosDictAppend( AcroFieldObject, V, ItemFormObject );
						ItemFormObject = CosStringNew( Doc, false, pFieldValue, (ppInt32)strlen( pFieldValue ) );
						_CosDictAppend( AcroFieldObject, DV, ItemFormObject );
					}
	}

	pAnnotQuadding &= 3;
	ItemFormObject = CosIntNew( Doc, false, pAnnotQuadding );
	_CosDictAppend( AcroFieldObject, Q, ItemFormObject );

	ItemFormObject = CosIntNew( Doc, false, pFieldFlag );
	_CosDictAppend( AcroFieldObject, Ff, ItemFormObject );


	pAnnotFlag &= 511;
	ItemFormObject = CosIntNew( Doc, false, pAnnotFlag );
	_CosDictAppend( AcroFieldObject, F, ItemFormObject );

	ItemFormObject = CosDictNew( Doc, false, 0 );
	_CosDictAppend( AcroFieldObject, AA, ItemFormObject );

	if ( IsAcroFieldFlagButton( pFieldType ) ) {
		ItemFormObject = CosNameNew( Doc, false, GetAtomDoc( P ) );
		_CosDictAppend( AcroFieldObject, H, ItemFormObject );
	}

	PageObject = CosArrayNew( Doc, false, 4 );
	ItemFormObject = CosRealNew( Doc, false, pFieldRect.xl );
	CosArrayAppend( PageObject, ItemFormObject );
	ItemFormObject = CosRealNew( Doc, false, pFieldRect.yl );
	CosArrayAppend( PageObject, ItemFormObject );
	ItemFormObject = CosRealNew( Doc, false, pFieldRect.xr );
	CosArrayAppend( PageObject, ItemFormObject );
	ItemFormObject = CosRealNew( Doc, false, pFieldRect.yr );
	CosArrayAppend( PageObject, ItemFormObject );
	_CosDictAppend( AcroFieldObject, Rect, PageObject );

	if ( !IsAcroFieldFlagPushButton( pFieldType ) ) {
		ItemFormObject = CosNewRef( Doc, _CosObjID( AcroFieldObject ),
			_CosObjGeneration( AcroFieldObject ) );
		CosArrayAppend( AcroFieldsObject, ItemFormObject );
	}
	return AcroFieldObject;
}

void AddMKDictionaryToVariableTextField( PDFCosHandle VariableTextCosObject, TPDFColor BG, TPDFColor BC )
{
	PDFDocHandle    Doc = _CosDoc(VariableTextCosObject);
	PDFCosHandle    DictObject, ItemObject;

	/* Add MK Dictionary */
	DictObject = CosDictNew( Doc, false, 0 );
    _CosDictAppend ( VariableTextCosObject, MK, DictObject );

	if ( !IsNoPDFColor( BC ) ) {
		ItemObject = CosArrayNew( Doc, false, 0 );
		_CosDictAppend ( DictObject, BC, ItemObject );
		CosArrayAppend( ItemObject, CosRealNew( Doc, false, BC.Color.Gray ));	
		if (( BC.Device == cgCMYK ) || ( BC.Device == cgRGB )) {
			CosArrayAppend( ItemObject, CosRealNew( Doc, false, BC.Color.RGB.G ) );
			CosArrayAppend( ItemObject, CosRealNew( Doc, false, BC.Color.RGB.B ));
			if ( BC.Device == cgCMYK ) {
				CosArrayAppend( ItemObject, CosRealNew( Doc, false, BC.Color.CMYK.K ) );
			}
		}
	}
	if ( !IsNoPDFColor( BG ) ) {
		ItemObject = CosArrayNew( Doc, false, 0 );
		_CosDictAppend ( DictObject, BG, ItemObject );
		CosArrayAppend( ItemObject, CosRealNew( Doc, false, BG.Color.Gray ));
		if (( BG.Device == cgCMYK ) || ( BG.Device == cgRGB )) {
			CosArrayAppend( ItemObject, CosRealNew( Doc, false, BG.Color.RGB.G ));
			CosArrayAppend( ItemObject, CosRealNew( Doc, false, BG.Color.RGB.B ) );
			if ( BG.Device == cgCMYK ) {
				CosArrayAppend( ItemObject, CosRealNew( Doc, false, BG.Color.CMYK.K ));
			}
		}
	}
}

PBXHandle PBXCreateContent( PDFCosHandle CosXObject )
{
	PDFDocHandle Doc;
	PDFCosHandle ItemObject, CoordinateObject;
	PPBX         bx;
	TPDFRect     rect;

	Doc = _CosDoc(CosXObject);

	bx = (PPBX)mmalloc ( _LIB, sizeof ( TPBX ) );
	bx->Doc = Doc;
    bx->FS = 1;
	bx->Res = _CosDictValueByName( CosXObject, Resources );
	bx->cs  = CosXObject;
	bx->Strm = CosStreamGetValue ( CosXObject );
	bx->Resolution = 72;
	ItemObject = _CosDictValueByName( CosXObject, BBox );
	if ( _IsCosArray( ItemObject ) ) {
		CoordinateObject = CosArrayItem( ItemObject, 0 );
		rect.xl  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 1 );
		rect.yl  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 2 );
		rect.xr  = CosRealGetValue( CoordinateObject );
		CoordinateObject = CosArrayItem( ItemObject, 3 );
		rect.yr  = CosRealGetValue( CoordinateObject );
		bx->Width  = rect.xr - rect.xl;
		bx->Height = rect.yr - rect.yl;
	}
	bx->BaseLine = false;
	bx->E2I = 1.0;
	bx->I2E = 1.0;
	bx->CharSpace = 0.0;
	bx->WordSpace = 0.0;
	bx->HorizontalScaling = 100.0;
	bx->CurrentFontIndex = 0;
	bx->CurrentFontSize = 10.0;
	bx->TextInited = false;
	bx->TextUsed = false;
	bx->GrayUsed = false;
	bx->ColorUsed = false;
	bx->TextAngle = 0.0;
	bx->UnderLine = false;
	bx->StrikeOut = false;
	bx->FontInited = false;
	bx->Unicode = false;
	bx->CurrentFontIndex = ppMAXUns32;
	bx->TY = 0.0;
	bx->TX = 0.0;
	bx->ResourceFontIndex = 0;
	bx->ResourceImageIndex = 0;
	bx->ResourcePatternIndex = 0;
	bx->ResourceGStateIndex = 0;
	bx->FontIsChanged = true;
	bx->SaveCount = 0;
	bx->WT.YScale = 1.0;
	bx->WT.XScale = 1.0;
	bx->FS = 1.0;

	_CosNullNew( _DOC, bx->AcroformRes );

	bx->Rotate = pra0;
	PBXSetAngle( bx, bx->Rotate );
	CDebugCode (
		bx->DBG = NULL;
	);
	return ( PBXHandle ) bx;
}

char *AcroStdFontName( PBXHandle PaintBox, TPDFFontID FontID )
{
	if ( FontID.IsStdFont ) {
		switch( FontID.From.StandardFont ) {
		case stdfHelvetica              : return "Helv";
		case stdfHelveticaBold          : return "HeBo";
		case stdfHelveticaOblique       : return "HeOb";
		case stdfHelveticaBoldOblique   : return "HeBO";
		case stdfTimesRoman             : return "TiRo";
		case stdfTimesBold              : return "TiBo";
		case stdfTimesItalic            : return "TiIt";
		case stdfTimesBoldItalic        : return "TiBI";
		case stdfCourier                : return "Cour";
		case stdfCourierBold            : return "CoBo";
		case stdfCourierOblique         : return "CoOb";
		case stdfCourierBoldOblique     : return "CoBO";
		case stdfSymbol                 : return "Symb";
		case stdfZapfDingbats           : return "ZaDb";
		default : return "Helv";
		}
	} else
		return ULAtomToString( _PDOC(PaintBox)->Lib, _PBOX(PaintBox)->CurrentFontName );
}

void AddDAstringToVariableTextField( PDFCosHandle VariableTextCosObject, PBXHandle PaintBox, TPDFFont Font )
{
	PDFDocHandle    Doc = _CosDoc(VariableTextCosObject);
	PDFCosHandle    StringCosObject;
	char            DAstring[255];
	ppInt32         lenDAstring;

	switch( Font.Color.Device ) {
	case cgGray :
		lenDAstring = sprintf( DAstring, "/%s %.2f Tf %.2f g",
			AcroStdFontName( PaintBox, Font.ID ), Font.Size, Font.Color.Color.Gray );
		break;
	case cgRGB :
		lenDAstring = sprintf( DAstring, "/%s %.2f Tf %.2f %.2f %.2f rg", 
			AcroStdFontName( PaintBox, Font.ID ), Font.Size, Font.Color.Color.RGB.R,
			Font.Color.Color.RGB.G, Font.Color.Color.RGB.B );
		break;
	case cgCMYK :
		lenDAstring = sprintf( DAstring, "/%s %.2f Tf %.2f %.2f %.2f %.2f k", 
			AcroStdFontName( PaintBox, Font.ID ), Font.Size, Font.Color.Color.CMYK.C,
			Font.Color.Color.CMYK.M, Font.Color.Color.CMYK.Y, Font.Color.Color.CMYK.K );
		break;
	default:
		lenDAstring = sprintf( DAstring, "/%s %.2f Tf 0 g", 
			AcroStdFontName( PaintBox, Font.ID ), Font.Size, Font.Color.Color.Gray );
	}

	/* Add default appearance text font */
	StringCosObject = CosStringNew ( Doc, false, DAstring, lenDAstring );
	_CosDictAppend ( VariableTextCosObject, DA, StringCosObject );

}




void PDFAcroEditBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{
	PDFCosHandle    AFormObj;
	PDFCosHandle    AS;
	PBXHandle       PaintBox;
	PPDFEditBox	    EditBox;
	PDFCosHandle    Obj, wrk, PO;
	EditBox = & ( _ACRO->ObjectInfo.EditBox );
	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;

	if ( !EditBox->Font.ID.IsStdFont )
		_DOC->Version = max(_DOC->Version, pdfver15);
	SetAcroForm (AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk))
		_CosDictAppend( AFormObj,Fields, wrk = CosArrayNew(Doc, false, 1));
	
	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( wrk, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Tx));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,EditBox->Name, strlen(EditBox->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,EditBox->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,EditBox->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,EditBox->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,EditBox->Border.Rect.yr));
	if (EditBox->Caption){
		_CosDictAppend(Obj, V, CosStringNew(Doc, false,EditBox->Caption, strlen(EditBox->Caption)));
		_CosDictAppend(Obj, DV, CosStringNew(Doc, false,EditBox->Caption, strlen(EditBox->Caption)));
	}else 
		_CosDictAppend(Obj, V, CosStringNew(Doc, false,"", 0));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	_CosDictAppend(Obj, Ff, CosIntNew(Doc, false, EditBox->Flag & ( PDFAcroFlags_Multiline | PDFAcroFlags_Password |
		PDFAcroFlags_FileSelect | PDFAcroFlags_DoNotSpellCheck | PDFAcroFlags_DoNotScroll | PDFAcroFlags_ReadOnly)));
	_CosDictAppend(Obj, Q, CosIntNew(Doc, false,(ppUns8)EditBox->Align % 3));
	if ( EditBox->MaxLen )
		_CosDictAppend(Obj, MaxLen, CosIntNew(Doc, false, EditBox->MaxLen));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, EditBox->Border.FillColor,	EditBox->Border.BorderColor );
	AS = PDFAddAPXObject( Obj, apsNormal, NULL, EditBox->Border.Rect );
	PaintBox = PBXCreateContent( AS );
	_PBOX(PaintBox)->NextOperatorChar = ' ';
	_PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );

	if ( EditBox->PaintContent == NULL ) 
		PBXDrawEditBox( PaintBox, EditBox );
	else {
		//      EditBox.PaintContent( PaintBox, &EditBox );
	}

	AddDAstringToVariableTextField( Obj, PaintBox, EditBox->Font );
	PBXClose( PaintBox, false );
	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
}


/************************************************************************/
/* Active Features API                                                  */
/************************************************************************/

void PDFAcroPushButtonInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{
    PDFCosHandle    AS;
    PBXHandle       PaintBox;
	PPDFPushButton  PushButton;
	PDFCosHandle    Obj, wrk, PO;
	PushButton = &( _ACRO->ObjectInfo.PushButton );
	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;
	Obj = CosDictNew( Doc, true, 12);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Btn));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,PushButton->Name, strlen(PushButton->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, H, _CosNameNew(P));
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,PushButton->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,PushButton->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,PushButton->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,PushButton->Border.Rect.yr));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	_CosDictAppend(Obj, Q, CosIntNew(Doc, false, 0));
	_CosDictAppend(Obj, Ff, CosIntNew(Doc, false, 1 << 16));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, PushButton->Border.FillColor,	PushButton->Border.BorderColor );
    AS = PDFAddAPXObject( Obj, apsNormal, NULL, PushButton->Border.Rect );
    PaintBox = PBXCreateContent( AS );
    _PBOX(PaintBox)->NextOperatorChar = ' ';
    if ( PushButton->PaintContentUp == NULL ) 
        PBXDrawPushButtonUp( PaintBox, PushButton );
	else {
     //   PushButton.PaintContentUp( PaintBox, &PushButton );
	}
    PBXClose( PaintBox, false );
    AS = PDFAddAPXObject( Obj, apsDown, NULL, PushButton->Border.Rect );
    PaintBox = PBXCreateContent( AS );
    _PBOX(PaintBox)->NextOperatorChar = ' ';
    if ( PushButton->PaintContentDown == NULL ) 
        PBXDrawPushButtonDown( PaintBox, PushButton );
	else {
     //   PushButton.PaintContentDown( PaintBox, &PushButton );
	}

    PBXClose( PaintBox, false );
	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
}

void PDFAcroCheckBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{
	PPDFCheckBox	CheckBox;
	PDFCosHandle    AFormObj;
	PDFCosHandle    AS;
	PBXHandle       PaintBox;
	PDFCosHandle    Obj, wrk, PO;
	CheckBox = & (_ACRO->ObjectInfo.CheckBox );

	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;
	SetAcroForm(AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk))
		_CosDictAppend( AFormObj,Fields, wrk = CosArrayNew(Doc, false, 1));

	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( wrk, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Btn));
	_CosDictAppend(Obj, Ff,CosIntNew (Doc, false, 0));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,CheckBox->Name, strlen(CheckBox->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,CheckBox->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,CheckBox->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,CheckBox->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,CheckBox->Border.Rect.yr));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, CheckBox->Border.FillColor,	CheckBox->Border.BorderColor );
	_CosDictAppend(Obj, V, CheckBox->Value? _CosNameNew(Yes):_CosNameNew(Off));
	_CosDictAppend(Obj, DV, CheckBox->Value? _CosNameNew(Yes):_CosNameNew(Off));
	_CosDictAppend(Obj, AS, CheckBox->Value? _CosNameNew(Yes):_CosNameNew(Off));
    AS = PDFAddAPXObject( Obj, apsNormal, "Off", CheckBox->Border.Rect );
    PaintBox = PBXCreateContent( AS );
    _PBOX(PaintBox)->NextOperatorChar = ' ';
    _PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );

    if ( CheckBox->PaintContentOff == NULL ) 
        PBXDrawCheckBox( PaintBox, CheckBox, false );
	else {
     //   CheckBox.PaintContentOff( PaintBox, &CheckBox );
	}

    PBXClose( PaintBox, false );

    AS = PDFAddAPXObject( Obj, apsNormal, "Yes", CheckBox->Border.Rect );
    PaintBox = PBXCreateContent( AS );
    _PBOX(PaintBox)->NextOperatorChar = ' ';
    _PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );

    if ( CheckBox->PaintContentOn == NULL ) 
        PBXDrawCheckBox( PaintBox, CheckBox, true );
	else {
     //   CheckBox->PaintContentOn( PaintBox, &CheckBox );
	}

    PBXClose( PaintBox, false );

	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
}

void PDFAcroRadioButtonInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{
    PDFCosHandle    w1;
	ppUns32			i,l;
	PDFCosHandle    From, t;
	ppBool			UseOpt;
	PPDFRadioButton RadioButton;
	PDFCosHandle    AFormObj;
	PDFCosHandle    AS;
	PBXHandle       PaintBox;
	PDFCosHandle    Obj, wrk, PO;

	RadioButton = & ( _ACRO->ObjectInfo.RadioButton );
	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;	
	SetAcroForm(AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk)){
		_CosDictAppend (AFormObj, Fields, wrk = CosArrayNew (Doc, false,1));
	}
	if (_ACRO->Group){
		l = strlen(_ACRO->Group);
		for (i = 0; i <_CosArrayCount(wrk);i++){
			Obj = CosArrayItem (wrk, i);
			if (!_IsCosDict(Obj))
				continue;
			t = _CosDictValueByName(Obj,T);
			if (!_IsCosString(t) || _CosStringSize(t)!=l || memcmp (_ACRO->Group, _CosStreamValue(t),l))
				continue;
			t = _CosDictValueByName(Obj,FT);
			if (!_IsCosName(t) ||_CosNameValue(t)!=GetAtomDoc(Btn))
				continue;
			t = _CosDictValueByName(Obj,Ff);
			if (_IsCosInt(t) && !(_CosIntValue(t) &(1<<16)) || _CosIntValue(t) &(1<<15))
				break;
		}
		if (i ==_CosArrayCount(wrk)){
			CosArrayAppend (wrk, Obj = CosDictNew (Doc, true,5));
			_CosDictAppend(Obj, T,CosStringNew (Doc, false, _ACRO->Group, strlen(_ACRO->Group)));
			_CosDictAppend(Obj, FT, _CosNameNew(Btn));
			_CosDictAppend(Obj, Ff, CosIntNew(Doc, false, 1 << 15));
			_CosDictAppend(Obj, Kids, From = CosArrayNew(Doc,false, 1));
		} else {
			From = _CosDictValueByName( Obj,Kids);
			if (!_IsCosArray(From))
				_CosDictAppend(Obj, Kids, From = CosArrayNew(Doc,false, 1));
		}
		UseOpt = true;
		if (RadioButton->Value){
			_CosDictAppend(Obj, V, CosNameNew (Doc, false, ULStringToAtom (_LIB, RadioButton->Name)));
			_CosDictAppend(Obj, DV, CosNameNew (Doc, false, ULStringToAtom (_LIB, RadioButton->Name)));
		} else {
			wrk = _CosDictValueByName(Obj, V);
			if (!_IsCosName(wrk))
				_CosDictAppend(Obj, V, _CosNameNew (Off)); 
		}
		w1 = Obj;
	} else {
		UseOpt = false;
		From = wrk;
	}

	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( From, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Btn));
	_CosDictAppend(Obj, Ff,CosIntNew (Doc, false, 1 << 15 | 1 <<14));
	_CosDictAppend(Obj, P, PO);
	if (UseOpt)
		_CosDictAppend(Obj, Parent, w1);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,RadioButton->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,RadioButton->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,RadioButton->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,RadioButton->Border.Rect.yr));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, RadioButton->Border.FillColor, RadioButton->Border.BorderColor );
	if (RadioButton->Value)
		_CosDictAppend(Obj, AS, CosNameNew(Doc, false, ULStringToAtom (_LIB, RadioButton->Name)));
	else 
		_CosDictAppend(Obj, AS, _CosNameNew(Off));
	AS = PDFAddAPXObject( Obj, apsNormal, "Off", RadioButton->Border.Rect );
	PaintBox = PBXCreateContent( AS );
	_PBOX(PaintBox)->NextOperatorChar = ' ';
	_PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );

	if ( RadioButton->PaintContentOff == NULL ) 
		PBXDrawCheckBox( PaintBox, RadioButton, false );
	else {
		//   RadioButton.PaintContentOff( PaintBox, &RadioButton );
	}
	PBXClose( PaintBox, false );

	AS = PDFAddAPXObject( Obj, apsNormal, RadioButton->Name, RadioButton->Border.Rect );
	PaintBox = PBXCreateContent( AS );
	_PBOX(PaintBox)->NextOperatorChar = ' ';
	_PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );

	if ( RadioButton->PaintContentOff == NULL ) 
		PBXDrawCheckBox( PaintBox, RadioButton, true );
	else {
		//   RadioButton.PaintContentOff( PaintBox, &RadioButton );
	}

	PBXClose( PaintBox, false );

	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
}

void PDFAcroComboBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex)
{

	PDFCosHandle    AFormObj;
	PDFCosHandle    AS;
	PBXHandle       PaintBox;
	PPDFComboBox	ComboBox;
	PDFCosHandle    Obj, wrk, PO;
	ppUns32			i;

	ComboBox = & ( _ACRO->ObjectInfo.ComboBox );
	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;

	if ( !ComboBox->Font.ID.IsStdFont )
		_DOC->Version = max(_DOC->Version, pdfver15);
	SetAcroForm (AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk))
		_CosDictAppend( AFormObj,Fields, wrk = CosArrayNew(Doc, false, 1));

	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( wrk, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Ch));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,ComboBox->Name, strlen(ComboBox->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,ComboBox->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ComboBox->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ComboBox->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ComboBox->Border.Rect.yr));
	if (ComboBox->Caption){
		_CosDictAppend(Obj, V, CosStringNew(Doc, false,ComboBox->Caption, strlen(ComboBox->Caption)));
		_CosDictAppend(Obj, DV, CosStringNew(Doc, false,ComboBox->Caption, strlen(ComboBox->Caption)));
	} else 
		_CosDictAppend(Obj, V, CosStringNew(Doc, false,"", 0));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	_CosDictAppend(Obj, Ff, CosIntNew(Doc, false, ComboBox->Flag & 
		( PDFAcroFlags_Edit | PDFAcroFlags_Sort | \
		PDFAcroFlags_DoNotSpellCheck ) | PDFAcroFlags_Combo | PDFAcroFlags_ReadOnly) );
	_CosDictAppend(Obj, Q, CosIntNew(Doc, false,0));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, ComboBox->Border.FillColor, ComboBox->Border.BorderColor );

	_CosDictAppend(Obj, Opt, wrk = CosArrayNew (Doc, false, _ACRO->Size));
	for (i= 0; i < _ACRO->Size;i++){
		CosArrayAppend (wrk, CosStringNew (Doc, false, _ACRO->List[i], strlen(_ACRO->List[i])));
	}
	AS = PDFAddAPXObject( Obj, apsNormal, NULL, ComboBox->Border.Rect );
	PaintBox = PBXCreateContent( AS );
	_PBOX(PaintBox)->NextOperatorChar = ' ';
	_PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );


	if ( ComboBox->PaintContent == NULL ) 
		PBXDrawComboBox( PaintBox, ComboBox );
	else {
		//   ComboBox.PaintContent( PaintBox, &ComboBox );
	}

	AddDAstringToVariableTextField( Obj, PaintBox, ComboBox->Font );
	PBXClose( PaintBox, false );
	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
    
}

void PDFAcroListBoxInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{

	PDFCosHandle    AFormObj;
	PDFCosHandle    AS;
	PBXHandle       PaintBox;
	PDFCosHandle    Obj, wrk, PO, tmp;
	ppUns32			i,ti, idx = ppMAXUns32;
	PPDFListBox		ListBox ;


	ListBox = & (_ACRO->ObjectInfo.ListBox );

	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;

	if ( !ListBox->Font.ID.IsStdFont )
		_DOC->Version = max(_DOC->Version, pdfver15);
	SetAcroForm (AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk))
		_CosDictAppend( AFormObj,Fields, wrk = CosArrayNew(Doc, false, 1));

	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( wrk, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Ch));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,ListBox->Name, strlen(ListBox->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,ListBox->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ListBox->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ListBox->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,ListBox->Border.Rect.yr));
	if (ListBox->Caption){
		_CosDictAppend(Obj, V, CosStringNew(Doc, false,ListBox->Caption, strlen(ListBox->Caption)));
		_CosDictAppend(Obj, DV, CosStringNew(Doc, false,ListBox->Caption, strlen(ListBox->Caption)));
	};
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	_CosDictAppend(Obj, Ff, CosIntNew(Doc, false, ListBox->Flag & ( PDFAcroFlags_Sort | PDFAcroFlags_MultiSelect | PDFAcroFlags_ReadOnly )));
	_CosDictAppend(Obj, Q, CosIntNew(Doc, false,0));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	AddMKDictionaryToVariableTextField( Obj, ListBox->Border.FillColor, ListBox->Border.BorderColor );

	_CosDictAppend(Obj, Opt, wrk = CosArrayNew (Doc, false, _ACRO->Size));
	for (i= 0; i < _ACRO->Size;i++){
		CosArrayAppend (wrk, CosStringNew (Doc, false, _ACRO->List[i], strlen(_ACRO->List[i])));
		if (ListBox->Caption && !strcmp(ListBox->Caption,_ACRO->List[i])){
			idx = i;
		}
	}
	if (ListBox->Caption){
		if (idx == ppMAXUns32){
			idx = i;
			CosArrayAppend (wrk, CosStringNew (Doc, false, ListBox->Caption, strlen(ListBox->Caption)));
		}
		_CosDictAppend (Obj, I, tmp = CosArrayNew (Doc, false, 1));
		CosArrayAppend (tmp, CosIntNew(Doc, false, i));
	}
	AS = PDFAddAPXObject( Obj, apsNormal, NULL, ListBox->Border.Rect );
	PaintBox = PBXCreateContent( AS );
	_PBOX(PaintBox)->NextOperatorChar = ' ';
	_PBOX(PaintBox)->AcroformRes = _CosDictValueByName( AFormObj, DR );
	PBXDrawListBox( PaintBox, ListBox, _ACRO->List, _ACRO->Size, idx, &ti );
	AddDAstringToVariableTextField( Obj, PaintBox, ListBox->Font );
	PBXClose( PaintBox, false );
	if (ti!= ppMAXUns32)
		_CosDictAppend (Obj, TI, CosIntNew (Doc, false, ti));
	_ACRO->Stored = true;
	_ACRO->Handle = Obj;
}


void PDFAcroObjectAddAction( PDFDocHandle Doc, ppUns32 AcroIndex, PDFActionHandle Action, TPDFAcroEventType Type)
{
    PDFCosHandle    AcroFormObject, AddActionObject;
	if ( !_DOC->AcroFormList || _DOC->AcroFormList->Size <= AcroIndex )
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError );
	if ( !_ACRO->Stored )
		PDFAcroObjectDrawIt ( Doc, AcroIndex );
    AcroFormObject = _ACRO->Handle;
    AddActionObject = _CosDictValueByName( AcroFormObject, AA );
	if (!_IsCosDict(AddActionObject))
		_CosDictAppend( AcroFormObject, AA, AddActionObject = CosDictNew(Doc, false, 1) );
	if ( ! _IsCosDict( Action ) )
		return;
    switch( Type ) {
        case PDFAcroEventTypeActivate :
                _CosDictAppend ( AcroFormObject, A, Action );
            break;
        case PDFAcroEventTypeEnter :
                _CosDictAppend ( AddActionObject, E, Action );
            break;
        case PDFAcroEventTypeExit :
                _CosDictAppend ( AddActionObject, X, Action );
            break;
        case PDFAcroEventTypePress :
                _CosDictAppend ( AddActionObject, D, Action );
            break;
        case PDFAcroEventTypeRelease :
                _CosDictAppend ( AddActionObject, U, Action );
            break;
        case PDFAcroEventTypeFocusOn :
                _CosDictAppend ( AddActionObject, Fo, Action );
            break;
        case PDFAcroEventTypeFocusOff :
                _CosDictAppend ( AddActionObject, Bl, Action );
            break;
        case PDFAcroEventTypeKeystroke :
                _CosDictAppend ( AddActionObject, K, Action );
            break;
        case PDFAcroEventTypeFormat :
                _CosDictAppend ( AddActionObject, F, Action );
            break;
        case PDFAcroEventTypeValidate :
                _CosDictAppend ( AddActionObject, V, Action );
            break;
        case PDFAcroEventTypeCalculate :
                _CosDictAppend ( AddActionObject, C, Action );
            break;
        default:;
        }
}
/************************************************************************/
/* Signature Acroform API                                               */
/************************************************************************/
void PDFAcroSignatureInDocument( PDFDocHandle Doc, ppUns32 AcroIndex )
{
	PDFCosHandle    AFormObj;
	PDFCosHandle    Obj, wrk, PO;
	PPDFSignature   Signature;

	Signature = & (_ACRO->ObjectInfo.Signature );

	PO = ( ( PPDFPageInfo ) _DOC->Pages->FList[ _ACRO->PageIndex ] )->PO;
	SetAcroForm(AFormObj);
	wrk = _CosDictValueByName(AFormObj, Fields);
	if (!_IsCosArray(wrk))
		_CosDictAppend( AFormObj,Fields, wrk = CosArrayNew(Doc, false, 1));

	Obj = CosDictNew( Doc, true, 12);
	CosArrayAppend( wrk, Obj);
	_CosDictAppend(Obj, Type, _CosNameNew(Annot));
	_CosDictAppend(Obj, Subtype, _CosNameNew(Widget));
	_CosDictAppend(Obj, FT, _CosNameNew(Sig));
	_CosDictAppend(Obj, Ff,CosIntNew (Doc, false, 0));
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,Signature->Name, strlen(Signature->Name)));
	_CosDictAppend(Obj, P, PO);
	_CosDictAppend(Obj, Rect, wrk= CosArrayNew(Doc, false, 4) );
	CosArrayAppend(wrk,CosRealNew(Doc, false,Signature->Border.Rect.xl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,Signature->Border.Rect.yl));
	CosArrayAppend(wrk,CosRealNew(Doc, false,Signature->Border.Rect.xr));
	CosArrayAppend(wrk,CosRealNew(Doc, false,Signature->Border.Rect.yr));
	_CosDictAppend(Obj, F, CosIntNew(Doc, false, _ACRO->AnnotationFlag));
	wrk = _CosDictValueByName(PO, Annots);
	if (!_IsCosArray(wrk))
		_CosDictAppend(PO, Annots, wrk = CosArrayNew(Doc, false, 1));
	CosArrayAppend(wrk, Obj);
	
	_ACRO->Stored = true;
	_ACRO->Handle = Obj;

}


