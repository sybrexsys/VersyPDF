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
VSAcroInfoA.c
*********************************************************************/

#include "VSAcroInfoA.h"

#include "VSCanvasAI.h"
#include "VSDocAI.h"
#include "VSFontAI.h"
#include "VSAcroObjects.h"
#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib



ppUns32 PDFAcroGetCount( PDFDocHandle Doc )
{
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	return _DOC->AcroItems->Size;
}
TPDFAcroType PDFAcroGetType (PDFDocHandle Doc, PPDFAcroFormItem Item)
{
	if (Item->Type == GetAtomDoc(Btn)){
		if (Item->Flag & (1 << 16 ) )
			return PDFAcroTypePushButton;
		if (Item->Flag & (1 << 15 ) )
			return PDFAcroTypeRadioButton;
		else
			return PDFAcroTypeCheckBox;
	}
	if (Item->Type == GetAtomDoc(Tx))
		return PDFAcroTypeEditBox;
	if (Item->Type == GetAtomDoc(Ch)){
		if (Item->Flag & (1 << 17 ) )
			return PDFAcroTypeComboBox;
		else
			return PDFAcroTypeListBox;
	} 
	if (Item->Type == GetAtomDoc(Sig))
		return PDFAcroTypeSignature;
	else 
		return PDFAcroTypeUnknown;
}
void * PDFAcroGetNameByIndex (PDFDocHandle Doc, ppUns32 Index, ppBool *IsUnicode, ppUns32 *Length)
{
	PPDFAcroFormItem Item;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]); 
	*Length = Item->Length;
	*IsUnicode = Item->IsUnicode;
	if (Item->IsUnicode)
		return Item->UnicodeName;
	else 
		return Item->StrName;
}


char * PDFAcroGetValueByIndex (PDFDocHandle Doc, ppUns32 Index, ppUns32 *Length)
{
	PDFCosHandle Obj, wrk;
	char *R;
	PPDFAcroFormItem Item;
	ppUns32 i;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]);
	switch (Item->AType) {
		case PDFAcroTypeEditBox:
		case PDFAcroTypeListBox:
		case PDFAcroTypeComboBox:
			Obj = Item->V;
			if (_IsCosNull(Obj))
				Obj = Item->DV;
			if (_IsCosNull(Obj)){
				return NULL;
			}
			if (_IsCosString(Obj)){
				*Length = _CosStringSize( Obj );
				return _CosStringValue (Obj);
			} else 
				return NULL;
			break;
		case PDFAcroTypeCheckBox:
			Obj = _CosDictValueByName(Item->Obj, AS);
			if (_IsCosName(Obj)){
				R = ULAtomToString (_LIB, _CosNameValue(Obj));
				*Length = (ppUns32)strlen(R);
				return R;
			} else {
				*Length = 3;
				return "Off";
			}
			break;
		case PDFAcroTypeRadioButton:
			if (_IsCosName(Item->V)){
				R = ULAtomToString (_LIB, _CosNameValue(Item->V));
				*Length = (ppUns32)strlen(R);
				return R;
			}
			Obj = _CosDictValueByName(Item->Obj, Kids);
			if (!_IsCosArray(Obj)){
				Obj = _CosDictValueByName(Item->Obj, AS);
				if (_IsCosName(Obj)){
					R = ULAtomToString (_LIB, _CosNameValue(Obj));
					*Length = (ppUns32)strlen(R);
					return R;
				} else {
					*Length = 3;
					return "Off";
				}
			}
			for (i= 0; i < _CosArrayCount(Obj);i++){
				wrk = CosArrayItem (Obj, i);
				if (!_IsCosDict(wrk))
					continue;
				wrk = _CosDictValueByName( wrk, AS);
				if (_IsCosName(wrk) && _CosNameValue(wrk)!= GetAtomDoc(Off)){
					R = ULAtomToString (_LIB, _CosNameValue(wrk));
					*Length = (ppUns32)strlen(R);
					return R;
				}
			}
			*Length = 3;
			return "Off";
			break;
		default :
			return NULL;
	}
}

TPDFAcroType PDFAcroGetTypeByIndex( PDFDocHandle Doc, ppUns32 Index )
{
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	return ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index])->AType;
}

PDFCosHandle PDFAcroGetObjByIndex( PDFDocHandle Doc, ppUns32 Index )
{
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	return ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index])->Obj;
}

ppBool PDFAcroChangeNameByName (PDFDocHandle Doc, char *OldName, char *NewName)
{
	PDFCosHandle Obj;
	ppUns32 idx;

	idx = PDFAcroGetIndexByName(Doc, OldName, false);
	if (idx == ppMAXUns32)
		return false;
	Obj = ((PPDFAcroFormItem)_DOC->AcroItems->FList[idx])->Obj;
	_CosDictAppend(Obj, T, CosStringNew(Doc, false,NewName, (ppUns32)strlen(NewName)));
	PDFAcroFreeForms ( Doc );
	PDFAcroLoadForms ( Doc );
	return true;
}

ppUns32 PDFAcroGetIndexByName(PDFDocHandle Doc, char * FieldName, ppBool CheckUnicodeNames)
{
	ppUns32 i, l, j, len;
	PPDFAcroFormItem Item;
	PDFCosHandle Obj;
	unsigned char *t;
	ppUns16 U;

	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	l = (ppUns32)strlen(FieldName);
	for ( i =0; i < _DOC->AcroItems->Size;i++ ){
		Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[i]);
		if (Item->Length != l)
			continue;
		if (!Item->IsUnicode){
			if (!memcmp ( Item->StrName, FieldName, l ))
				return i;
		} else {
			if (!CheckUnicodeNames)
				continue;
			for (j = 0; j<l;j++)
				if (FieldName[j]!=ULUnicodeToANSI (Item->UnicodeName[j]))
					break;
			if (j ==l)
				return i;
		}
	}
	for ( i =0; i < _DOC->AcroItems->Size;i++ ){
		Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[i]);
		Obj = _CosDictValueByName(Item->Obj,TM);
		if (!_IsCosString(Obj))
			continue;
		len = _CosStringSize(Obj);
		t = (unsigned char *)_CosStringValue(Obj);
		if (len >=2 && t[0] == 0xfe && t[1] == 0xff){
			if (!CheckUnicodeNames)
				continue;
			len = (len>>1)-1;
			if (l!=len)
				continue;
			for (j = 0; j <len;j++){
				U = (((ppUns16)t[2+(i<<1)])<<8)+t[3+(j<<1)];
				if (FieldName[j]!=ULUnicodeToANSI (U))
					break;
			}
			if (j ==len)
				return i;
		} else {
			if (l!=len)
				continue;
			if (!memcmp (t, FieldName,l))
				return i;
		}
	}
	return ppMAXUns32;
}
void PDFAcroSetValueByIndex ( PDFDocHandle Doc, ppUns32 Index, char * NewValue)
{
	PPDFAcroFormItem Item;
	ppAtom A;
	PDFCosHandle Obj, wrk, w1;
	ppUns32 i, l, j, idx;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]);
	switch ((ppUns32)Item->AType){
		case PDFAcroTypeListBox:
		case PDFAcroTypeEditBox:
		case PDFAcroTypeComboBox:
			_CosDictAppend(Item->Obj, AP, _CosNull(Doc));
			_CosDictAppend(Item->Obj,V,CosStringNew (Doc, false, NewValue, (ppUns32)strlen(NewValue)));
			CosFree(Item->V);
			Item->V = CosStringNew (Doc, false, NewValue, (ppUns32)strlen(NewValue));
			_DOC->AcroValuesWasModified = true;
			_DOC->NeedAppendReDrawAcroScript = true;
			if (Item->AType !=PDFAcroTypeListBox)
				return;
			Obj = _CosDictValueByName( Item->Obj,Opt);
			if (!_IsCosArray(Obj)){
				_CosDictAppend(Item->Obj,Opt,wrk= CosArrayNew (Doc,false, 1));
				CosArrayAppend (wrk, CosStringNew (Doc, false, NewValue,(ppUns32)strlen (NewValue)));
				return;
			}
			l = (ppUns32)strlen(NewValue);
			for (i = 0;i <_CosArrayCount(Obj);i++ ){
				wrk = CosArrayItem (Obj, i);
				if (_IsCosArray(wrk) && _CosArrayCount(wrk)>0)
					wrk = CosArrayItem (wrk,0);
				if (_IsCosString(wrk) && _CosStringSize(wrk)==l && !memcmp(_CosStringValue(wrk),NewValue,l) )
					break;
			}
			_CosDictAppend( Item->Obj, I,wrk = CosArrayNew (Doc,false,1));
			CosArrayAppend (wrk, CosIntNew (Doc, false, i) );
			if (i <_CosArrayCount(Obj))
				return;
			CosArrayAppend (Obj, CosStringNew (Doc, false, NewValue,(ppUns32)strlen (NewValue)));
			_CosDictAppend( Item->Obj, TI,CosIntNew (Doc, false, i));
			return;
		case PDFAcroTypeCheckBox:
			if (!strcmp(NewValue,"Off")){
				_CosDictAppend(Item->Obj, V, _CosNameNew(Off));
				_CosDictAppend(Item->Obj, AS, _CosNameNew(Off));
				_DOC->AcroValuesWasModified = true;
				CosFree(Item->V);
				Item->V = _CosNameNew (Off);
				return;
			} 
			Obj = _CosDictValueByName(Item->Obj, AS);
			if (_IsCosName(Obj) && _CosNameValue(Obj)!=GetAtomDoc(Off))
				return;
			Obj = _CosDictValueByName(Item->Obj, AP);
			if (!_IsCosDict(Obj))
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			Obj = _CosDictValueByName(Obj, N);
			if (!_IsCosDict(Obj))
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			for (i= 0; i < _CosDictCount(Obj);i++){
				if (_CosDictPair(Obj,i).Key != GetAtomDoc(Off)){
					A = _CosDictPair(Obj,i).Key;
					_CosDictAppend(Item->Obj, V, CosNameNew(Doc, false, A));
					_CosDictAppend(Item->Obj, AS, CosNameNew(Doc, false, A));
					_DOC->AcroValuesWasModified = true;
					CosFree(Item->V);
					Item->V = CosNameNew(Doc, false, A);
					return;
				}
			}
			_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
		case PDFAcroTypeRadioButton:
			if (!strcmp(NewValue,"Off")){
				_CosDictAppend(Item->Obj, V, _CosNameNew(Off));
				_DOC->AcroValuesWasModified = true;
				CosFree(Item->V);
				Item->V = _CosNameNew (Off);
				Obj = _CosDictValueByName( Item->Obj,Kids);
				if (!_IsCosArray(Obj)){
					_CosDictAppend(Item->Obj, AS, _CosNameNew(Off));
					return;
				}
				for (i=0; i < _CosArrayCount(Obj);i++){
					wrk = CosArrayItem (Obj, i);
					if (_IsCosDict(wrk)){
						_CosDictAppend(wrk, AS, _CosNameNew(Off));
						_CosDictAppend(wrk, V, _CosNull(Doc));
					}
				}
				return;
			} 
			A = ULStringToAtom (_LIB, NewValue);
			Obj = _CosDictValueByName( Item->Obj,Kids);
			if (!_IsCosArray(Obj)){
				wrk = _CosDictValueByName(Item->Obj,AS);
				if(_IsCosName(wrk) && _CosNameValue(wrk)== A){
					_CosDictAppend(Item->Obj,V,CosNameNew (Doc, false, A));
					CosFree(Item->V);
					_DOC->AcroValuesWasModified = true;
					Item->V = CosNameNew (Doc, false, A);
					return;
				}
				wrk = _CosDictValueByName(Item->Obj,AP);
				if (!_IsCosDict(wrk))
					_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
				wrk = _CosDictValueByName(wrk,N);
				if (!_IsCosDict(wrk))
					_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
				for (i =0; i < _CosDictCount(wrk); i++ ){
					if (_CosDictPair(wrk,i).Key == A){
						_CosDictAppend(Item->Obj,V,CosNameNew (Doc, false, A));
						_CosDictAppend(Item->Obj,AS,CosNameNew (Doc, false, A));
						CosFree(Item->V);
						_DOC->AcroValuesWasModified = true;
						Item->V = CosNameNew (Doc, false, A);
						return ;
					}
				}
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			}
			idx = ppMAXUns32;
			Obj = _CosDictValueByName( Item->Obj,Kids);
			if (!_IsCosArray(Obj))
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			for (i=0; i < _CosArrayCount(Obj);i++){
				wrk = CosArrayItem (Obj, i);
				if (!_IsCosDict(wrk))
					continue;
				w1 = _CosDictValueByName(wrk, AS);
				if (_IsCosName(w1) && _CosNameValue(w1) == A){
					idx = i;
					break;
				}
				w1 = _CosDictValueByName(wrk, AP);
				if (!_IsCosDict(w1))
					continue;
				w1 = _CosDictValueByName( w1, N);
				if (!_IsCosDict(w1))
					continue;
				for (j = 0; j < _CosDictCount(w1);j++)
					if (_CosDictPair(w1,j).Key == A)
						break;
				if (j !=_CosDictCount(w1)){
					idx = i;
					break;
				}
			}
			if (idx == ppMAXUns32){
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			}
			Obj = _CosDictValueByName( Item->Obj,Kids);
			if (!_IsCosArray(Obj))
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			for (i = 0; i <idx; i++){
				wrk = CosArrayItem (Obj, i);
				if (!_IsCosDict(wrk))
					continue;
				_CosDictAppend( wrk, AS, _CosNameNew(Off));
				_CosDictAppend( wrk, V, _CosNull(Doc));
			}
			for (i = idx+1; i <_CosArrayCount(Obj); i++){
				wrk = CosArrayItem (Obj, i);
				if (!_IsCosDict(wrk))
					continue;
				_CosDictAppend( wrk, AS, _CosNameNew(Off));
				_CosDictAppend( wrk, V, _CosNull(Doc));
			}
			wrk = CosArrayItem (Obj, idx);
			if (!_IsCosDict(wrk))
				_RAISE ( ErrDocumentLevel, dleCannotChangeValueForAcroFormError);
			_CosDictAppend( wrk, AS, CosNameNew(Doc, false, A));
			_CosDictAppend( wrk, V, _CosNull(Doc));
			_CosDictAppend(Item->Obj,V,CosNameNew (Doc, false, A));
			CosFree(Item->V);
			_DOC->AcroValuesWasModified = true;
			Item->V = CosNameNew (Doc, false, A);
			return ;
	}
	_RAISE (ErrDocumentLevel, dleUnsupportedTypeAcroFormError);
}

ppInt32 PDFAcroGetFlagsByIndex( PDFDocHandle Doc, ppUns32 Index )
{
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	return ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index])->Flag;
}

typedef struct AcroInh{
	ppAtom Type;
	ppInt32 Flag;
	PDFCosHandle V;
	PDFCosHandle DV;
}AcroInh;

void PDFAcroLoadItem ( PDFCosHandle Item, AcroInh Inh, void * Name, ppUns32 Length, ppBool IsUnicode )
{
	PDFCosHandle Obj, wrk;
	PDFDocHandle Doc = _CosDoc ( Item );
	ppUns32 i;
	ppBool Mod = false;
	ppUns8 *Str = NULL, *t;
	ppUns16 *Uni = NULL,U;
	ppUns32 Len, l, nl;
	ppBool IsRadio = false;
	PPDFAcroFormItem tmp;
	if ( !_IsCosDict( Item ) )
		return;
	Obj = _CosDictValueByName( Item, FT );
	if ( _IsCosName(Obj))
		Inh.Type = _CosNameValue ( Obj );
	Obj = _CosDictValueByName (Item, Ff);
	if (_IsCosInt(Obj))
		Inh.Flag = _CosIntValue ( Obj );
	Obj = _CosDictValueByName (Item, V);
	Inh.V = CosCopy ( Doc, Obj );
	Obj = _CosDictValueByName (Item, DV);
	Inh.DV = CosCopy ( Doc, Obj );
	Obj = _CosDictValueByName (Item, T);
	if (_IsCosString(Obj)){
		Mod = true;
		t =  (ppUns8 *) _CosStringValue (Obj);
		l = _CosStringSize ( Obj );
		if (Name == NULL){
			IsUnicode = (l >=2 && t[0] == 0xfe && t[1] == 0xff)?true:false;
			if (IsUnicode){
				Uni = (ppUns16 *) mmalloc( _LIB, sizeof(ppUns16)* (Len = (l >> 1) - 1));
				for (i =0; i < Len ; i++ ){
					Uni[i] = (((ppUns16)t[2+(i<<1)])<<8) +t[3+(i<<1)];
				}
			} else {
				Str = (ppUns8 *)mmalloc (_LIB, Len = l);
				memcpy( Str, t, Len );
			}
		} else {
			if (IsUnicode){
				if (l >=2 && t[0] == 0xfe && t[1] == 0xff) {
					Len = Length + (nl = l>>1);
					Uni = (ppUns16 *) mmalloc( _LIB, sizeof(ppUns16)* Len );
					memcpy(Uni, Name, sizeof(ppUns16)*Length);
					Uni[Length] = '.';
					nl --;
					for (i = 0; i < nl; i++)
						Uni[Length+1+i] = (((ppUns16)t[2+(i<<1)])<<8)+t[3+(i<<1)];
				} else {
					Len = Length + 1 + l;
					Uni = (ppUns16 *) mmalloc( _LIB, sizeof(ppUns16)* Len );
					memcpy(Uni, Name, sizeof(ppUns16)*Length);
					Uni[Length] = '.';
					for ( i = 0; i < l; i++)
						Uni[Length+1+i]= ULANSIToUnicode (t[i]);
				}
			} else {
				if (l >=2 && t[0] == 0xfe && t[1] == 0xff) {
					Len = Length + 1 + (nl= (l>>1)-1);
					Str = (ppUns8 *)mmalloc (_LIB, Len );
					memcpy( Str, Name, Length);
					Str[Length] = '.';
					for (i = 0; i < nl;i++){
						U = (((ppUns16)t[2+(i<<1)])<<8)+t[3+(i<<1)];
						Str[Length + 1 +i ]= ULUnicodeToANSI ( U );
					}
				} else {
					Len = Length + 1 + l;
					Str = (ppUns8 *)mmalloc (_LIB, Len );
					memcpy( Str, Name, Length);
					Str[Length] = '.';
					memcpy(&(Str[Length+1]), t, l);
				}
			}
		}
	} else {
		if (IsUnicode)
			Uni = (ppUns16 *) Name;
		else
			Str = (ppUns8*) Name;
		Len = Length;
	}
	Obj = _CosDictValueByName (Item, Kids);
	wrk = _CosDictValueByName( Item, FT);
	if (_IsCosName(wrk) && _CosNameValue(wrk )== GetAtomDoc(Btn)){
		wrk = _CosDictValueByName( Item, Ff);
		if (_IsCosInt(wrk)){
			i = _CosIntValue(wrk);
			if ((i & (1<<16))==0 && (i & (1<<15))!=0 )
				IsRadio = true;
		}
	}
	if (_IsCosArray(Obj) && !IsRadio){
		for (i = 0; i < _CosArrayCount ( Obj ); i++)
			PDFAcroLoadItem ( CosArrayItem (Obj, i), Inh, IsUnicode? (ppUns8*)Uni:Str, Len, IsUnicode );
	} else {
		tmp = (PPDFAcroFormItem)mmalloc (_LIB, sizeof(TPDFAcroFormItem));
		tmp->DV = CosCopy(Doc, Inh.DV);
		tmp->V = CosCopy(Doc, Inh.V);
		tmp->Flag = Inh.Flag;
		tmp->Type = Inh.Type;
		tmp->IsUnicode = IsUnicode;
		tmp->Length = Len;
		if (IsUnicode){
			tmp->UnicodeName = (ppUns16 *)mmalloc( _LIB, sizeof(ppUns16)* Len );
			memcpy(tmp->UnicodeName, Uni, sizeof(ppUns16)* Len );
			tmp->StrName = NULL;
		} else {
			tmp->StrName = (char *)mmalloc( _LIB, Len );
			memcpy(tmp->StrName, Str,  Len );
			tmp->UnicodeName = NULL;
		}
		tmp->Obj = Item;
		tmp->AType = PDFAcroGetType ( Doc, tmp);
		ULListAdd (_DOC->AcroItems, tmp);
	}
	if (Mod )
		mfree( _LIB, IsUnicode? (ppUns8*)Uni:Str);
	CosFree ( Inh.DV );
	CosFree ( Inh.V );
}

void PDFAcroLoadForms (PDFDocHandle Doc)
{
	AcroInh Inh;
	PDFCosHandle Obj;
	ppUns32 i;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	_DOC->AcroIsLoaded = true;
	_DOC->AcroItems = ULListCreate ( _LIB, 0 );
	Obj = _CosDictValueByName( _DOC->Root, AcroForm);
	if ( !_IsCosDict (Obj) )
		return;
	Obj = _CosDictValueByName (Obj, Fields);
	if ( !_IsCosArray (Obj) || !_CosArrayCount(Obj))
		return;

	Inh.Flag = 0;
	Inh.DV = _CosNull ( Doc );
	Inh.V = _CosNull ( Doc );
	Inh.Type = ppMAXUns32;
	for ( i = 0; i < _CosArrayCount(Obj); i++)
		PDFAcroLoadItem ( CosArrayItem (Obj, i), Inh, NULL, 0, false );

}

void PDFAcroFreeForms ( PDFDocHandle Doc )
{
	ppUns32 i;
	if (_DOC->AcroIsLoaded){
		for (i = 0; i < _DOC->AcroItems->Size;i++){
			CosFree(((PPDFAcroFormItem)_DOC->AcroItems->FList[i])->DV);
			CosFree(((PPDFAcroFormItem)_DOC->AcroItems->FList[i])->V);
			if (((PPDFAcroFormItem)_DOC->AcroItems->FList[i])->IsUnicode)
				mfree(_LIB,((PPDFAcroFormItem)_DOC->AcroItems->FList[i])->UnicodeName);
			else
				mfree(_LIB,((PPDFAcroFormItem)_DOC->AcroItems->FList[i])->StrName);
			mfree(_LIB, _DOC->AcroItems->FList[i]);
		}
		ULListFree ( _DOC->AcroItems );
	}
}

void PDFAcroReloadForms (PDFDocHandle Doc)
{
	PDFAcroFreeForms ( Doc );
	PDFAcroLoadForms ( Doc );
}

ppUns32 PDFAcroGetMaxLengthByIndex ( PDFDocHandle Doc, ppUns32 Index)
{
	PPDFAcroFormItem Item ;
	PDFCosHandle Opt;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]);	
	Opt = _CosDictValueByName(Item->Obj,MaxLen);
	if (!_IsCosInt(Opt))
		return 0;
	else
		return _CosIntValue(Opt);
}

ppUns32 PDFAcroGetOptionCountByIndex ( PDFDocHandle Doc, ppUns32 Index)
{
	PPDFAcroFormItem Item ;
	PDFCosHandle Opt;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]);	
	switch (Item->AType){
		case PDFAcroTypeRadioButton:
			Opt = _CosDictValueByName(Item->Obj,Kids);
			if (_IsCosArray(Opt))
				return _CosArrayCount(Opt);
			return 1;
		case PDFAcroTypeComboBox:
		case PDFAcroTypeListBox:
			Opt = _CosDictValueByName(Item->Obj,Opt);
			if (_IsCosArray(Opt))
				return _CosArrayCount(Opt);
			else 
				return 0;
		default:
			return 0;
	}
}
char * PDFAcroGetOptionValueByIndex ( PDFDocHandle Doc, ppUns32 Index, ppUns32 OptionIndex, ppUns32 *Size)
{
	PPDFAcroFormItem Item ;
	PDFCosHandle Opt, wrk, Obj;
	char *R;
	ppUns32 i;
	if ( ! _DOC->AcroIsLoaded )
		PDFAcroLoadForms ( Doc );
	if (Index >= _DOC->AcroItems->Size)
		_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
	Item = ((PPDFAcroFormItem)_DOC->AcroItems->FList[Index]);	
	switch (Item->AType){
		case PDFAcroTypeRadioButton:
			Opt = _CosDictValueByName(Item->Obj,Kids);
			if (_IsCosArray(Opt)){
				if ( _CosArrayCount(Opt) > OptionIndex )
					Obj = CosArrayItem ( Opt, OptionIndex );
				else 
					return NULL;
			} else
                if (OptionIndex == 1)
                    Obj = Item->Obj;
                else
                    return NULL;
			wrk = _CosDictValueByName (Obj, AS);
			if (_IsCosName(wrk) && _CosNameValue(wrk)!= GetAtomDoc(Off)){
				R = ULAtomToString (_LIB, _CosNameValue(wrk));
				*Size = (ppUns32)strlen(R);
				return R;
			}
			wrk = _CosDictValueByName(Obj,AP);
			if (!_IsCosDict(wrk))
				return NULL;
			wrk = _CosDictValueByName(wrk,N);
			if (!_IsCosDict(wrk))
				return NULL;
			for (i = 0; i<_CosDictCount(wrk);i++){
				if (_CosDictPair(wrk,i).Key != GetAtomDoc(Off)){
					R = ULAtomToString (_LIB, _CosDictPair(wrk,i).Key);
					*Size = (ppUns32)strlen(R);
					return R;	
				}
			}
			return NULL;
		case PDFAcroTypeComboBox:
		case PDFAcroTypeListBox:
			Opt = _CosDictValueByName(Item->Obj,Opt);
			if (!_IsCosArray(Opt) && _CosArrayCount(Opt)>=OptionIndex)
				_RAISE ( ErrGeneralLevel, gleOutOfBoundError);
			wrk = CosArrayItem (Opt, OptionIndex );
			if (_IsCosArray(wrk) && _CosArrayCount (wrk)>1 )
				wrk = CosArrayItem (wrk, 0);
			if (!_IsCosString(wrk))
				_RAISE(ErrCosLevel, cleInvalidObjError);
			*Size = _CosStringSize(wrk);
			return _CosStringValue(wrk);
		default:
			return NULL;
	}

}

