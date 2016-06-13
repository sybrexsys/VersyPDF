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
VSDocA.c
*********************************************************************/ 

#include <string.h>


#include "VSDocAI.h"
#include "VSAssert.h"
#include "VSCrypt.h"  
#include "VSCosAI.h"
#include "VSMisc.h"
#include "VSParse.h"
#include "VSXref.h"
#include "VSFontAI.h"
#include "VSCanvasAI.h"
#include "VSCanvasA.h"
#include "VSGStateA.h"
#include "VSCosA.h"
#include "VSPageA.h"
#include "VSActionA.h"
#include "VSAnnotA.h"
#include "VSAcroObjects.h"
#include "VSFilter.h"

#ifdef WINDOWS_PLATFORM

#include <shellapi.h>

#endif
#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

static char AcroRedrawScript1[]="try{ for (var i = 0; i < this.numFields; i++){var f = this.getField(this.getNthFieldName(i));";
static char AcroRedrawScript2[]="if (typeof f != \"undefined\"){try {f.delay = true;f.delay = false;}catch(e) { continue; }}}}catch(e){}";
static char RedrawJSName[] = "PDFAcroFormRedraw";

static char PDFProducer[]= "VersyPDF Engine 3.x\0";

typedef struct TRenum{
    ppBool Mark;  
    ppInt32 Old;
    ppInt32 New;
} TRenum, *PRenum;

void PDFDocPackFree(PDFDocHandle Doc){
	ppUns32 i;
	i=_DOC->Size-1;
	while (i==_DOC->Size-1 && _DOC->Entries[i].Used==etFree && i){
		--_DOC->Size;
		--i;
	};
}

TPDFVersion PDFDocGetVersion ( PDFDocHandle Doc )
{
	return _DOC->Version;
}

void PDFDocSetVersion ( PDFDocHandle Doc, TPDFVersion Version )
{
	_DOC->Version = max( _DOC->Version, Version );
}


PDFDocHandle PDFDocCreate(PDFLibHandle Lib){
	CosDoc tmp;
	PDFCosHandle obj,pgs;
	tmp= ( CosDoc ) mmalloc(Lib,sizeof(TPDFDoc));
	tmp->Size=1;
	tmp->Lib=Lib;
    tmp->NullObject.Doc=tmp;
    tmp->NullObject.Type=CosNull;
    tmp->NullObject.IsIndirect=false;
    tmp->NullObject.ObjInfo.Parent=NULL;
	tmp->Capacity=1;
	_CosNullNew(tmp, tmp->Root);
    _CosNullNew(tmp, tmp->Info);
    _CosNullNew(tmp, tmp->CryptoInfo.FileID);
	tmp->Crypted=false;
	tmp->Pages=NULL;
	tmp->Remove=true;
	tmp->Packed=false;
	if (IsEdition(Lib, STD))
		tmp->Linearized = true;
	else 
		tmp->Linearized = false;
	tmp->CryptMetadata = true;
	tmp->FontList=NULL; 
    tmp->ImageList = NULL;
    tmp->GStateList = NULL;
    tmp->AcroFormList = NULL;
	tmp->ActionList = NULL;
	tmp->Strm=NULL;
	tmp->Pages=ULListCreate(Lib,0);
	tmp->NewCrypted=false;
	tmp->UnCrypted = true;
	tmp->Check = false;
	tmp->UseOldSecurity=true;
	tmp->UPassword=NULL;
	tmp->OPassword=NULL;
	tmp->Entries=NULL;
	tmp->AutoRemove = false;
    tmp->JpegQuality = 65;
	tmp->CryptoInfo.AlgList = NULL;
	tmp->CryptoInfo.Algorithms = _CosNull ( tmp );
	tmp->CryptoInfo.DefStream = 0;
	tmp->CryptoInfo.DefString = 0;
	tmp->CryptoInfo.DefStreamAlg = eaNone;
	tmp->CryptoInfo.DefStringAlg = eaNone;
#ifndef NOT_USE_SIGN	
	tmp->Signed=false;
	tmp->SigCtx=NULL;
#endif
	tmp->AcroIsLoaded = false;
	tmp->AcroItems = NULL;
	tmp->AcroValuesWasModified = false;
	tmp->NeedAppendReDrawAcroScript = false;
#ifdef WINDOWS_PLATFORM
	tmp->TTFList=NULL;
	tmp->AutoLaunch =  false;
	tmp->DC = GetDC(0);
	tmp->UseScreen = true;
    tmp->EmfImagesAsJpeg = false;
    tmp->BWImagesAsJBIG2 = false;
#endif
	PDFTRY(Lib)
		tmp->Entries=( TXREFEntry * )mmalloc(Lib,sizeof(TXREFEntry));
		tmp->Entries[0].Offset=ppMAXUns32;
		tmp->Entries[0].Used=etFree;
		tmp->Entries[0].Generation=0;
		_CosNullNew(tmp, tmp->Entries[0].Obj);
		tmp->Entries[0].Loaded=false;
		tmp->Version=pdfver13;
		obj= CosDictNew(tmp,true,4);
		CosDictAppend(obj,GetAtomLib(Lib,Type),CosNameNew(tmp,false, GetAtomLib(Lib,Catalog)));
		tmp->Root=obj;
		pgs=CosDictNew(tmp,true,3);
		CosDictAppend(pgs,GetAtomLib(Lib,Type),CosNameNew(tmp,false,GetAtomLib(Lib,Pages)));
		CosDictAppend(pgs,GetAtomLib(Lib,Kids),CosArrayNew(tmp,false,1));
		CosDictAppend(pgs,GetAtomLib(Lib,Count), CosIntNew(tmp,false,0));
		CosDictAppend(obj,GetAtomLib(Lib,Pages),pgs);
		pgs=CosDictNew(tmp,true,3);
		CosDictAppend(pgs,GetAtomLib(Lib,Producer),CosStringNew(tmp,false,PDFProducer,(ppUns32)strlen(PDFProducer)));
		tmp->Info=pgs;
	PDFEXCEPT(Lib)
	    chfreez(Lib, tmp->Entries);
		mfree(Lib, tmp);
	PDFTRYEND(Lib)
	return (PDFDocHandle) tmp;
}


ppBool PDFDocIsCrypted(PDFDocHandle Doc){
	if (!Doc) return false;
	return _DOC->Crypted;
}


TPDFProtectionType PDFDocGetCryptoAlgorithm( PDFDocHandle Doc ){
	if (! _DOC->Crypted) return ptUncrypted;
	if (_DOC->CryptoInfo.Version == 5 && _DOC->CryptoInfo.Version == 5 ) return pt256BitAESProtection;
	if (_DOC->CryptoInfo.Version == 4 && _DOC->CryptoInfo.Version == 4 ) 
		return  _DOC->CryptoInfo.nkl == pt128BitProtection? pt128BitProtection : pt128BitAESProtection;
	if (_DOC->CryptoInfo.KeyLength == 16 ) return pt128BitProtection;
	if (_DOC->CryptoInfo.KeyLength == 5 ) return pt40BitProtection;
	return ptUncrypted;
}

void PDFDocClose ( PDFDocHandle Doc )
{
	ppUns32 i;


#ifdef WINDOWS_PLATFORM
	ReleaseDC(0,_DOC->DC);
#endif


	for ( i = 1; i < _DOC->Size; i++){
		if (_DOC->Entries[i].Loaded) 
			CosFree(_DOC->Entries[i].Obj);
	};
	mfree(_LIB, _DOC->Entries);
	if (!_IsCosNull(_DOC->CryptoInfo.FileID ) ) 
        CosFree( _DOC->CryptoInfo.FileID);
	chfree(_LIB, _DOC->CryptoInfo.AlgList );
	CosFree ( _DOC->CryptoInfo.Algorithms);
	if (_DOC->UPassword) 
        mfree(_LIB, _DOC->UPassword);
	if (_DOC->OPassword) 
        mfree(_LIB, _DOC->OPassword);
	if (_DOC->AutoRemove)
		if (_DOC->Strm) ULStreamClose(_DOC->Strm);
	if (_DOC->Pages){
		for (i=0;i<_DOC->Pages->Size;i++){ 
			CosFree ( ( ( PPDFPageInfo ) _DOC->Pages->FList[i] )->Inh );
			mfree(_LIB, _DOC->Pages->FList[i]);
		}
		ULListFree(_DOC->Pages);
	}
	if ( _DOC->FontList ){
		for ( i = 0; i < _DOC->FontList->Size; i++ ){ 
			switch ((ppUns32)(( PPDFFontInfo )_DOC->FontList->FList[i])->Type ){
				case PDFTrueTypeFont:
					PDFFontTrueTypeClear(_LIB,  (( PPDFFontInfo )_DOC->FontList->FList[i]));
					break;
				case PDFType1Font:
					mfree(_LIB,  (( PPDFFontInfo )_DOC->FontList->FList[i])->FontInfo.T1Info );
					break;
			}
			mfree(_LIB, _DOC->FontList->FList[i]);
		}
		ULListFree(_DOC->FontList);
	}

    if ( _DOC->ImageList ){
        for (i=0;i<_DOC->ImageList->Size;i++){ 
            mfree(_LIB, _DOC->ImageList->FList[i]);
        }
        ULListFree(_DOC->ImageList);
    }
    if ( _DOC->GStateList ){
        ULExtListFree ( _DOC->GStateList );
    }

    if ( _DOC->AcroFormList ){
		for ( i = 0; i < _DOC->AcroFormList->Size;i++ ){
			PDFAcroObjectFree ( Doc, i );
		}
        ULListFree ( _DOC->AcroFormList );
    }

    if ( _DOC->ActionList ){
        ULExtListFree ( _DOC->ActionList );
    }

#ifdef WINDOWS_PLATFORM
	if (_DOC->TTFList){
		for (i=0;i<_DOC->TTFList->Size;i++)
			mfree(_LIB, _DOC->TTFList->FList[i]);
		ULListFree(_DOC->TTFList); 
	}
	if (_DOC->UseScreen)
		ReleaseDC(0, _DOC->DC);
	else
		DeleteDC(_DOC->DC);
#endif
#ifndef NOT_USE_SIGN
	sig_DisposeContext(_LIB, _DOC->SigCtx);
	_DOC->SigCtx=NULL;
#endif
	PDFAcroFreeForms ( Doc );
	mfree(_LIB, Doc);
}

TKeyValidType PDFDocCheckPassword(PDFDocHandle Doc, char *Password){
	TPDFCheckPasword RV;
	if (!_DOC->Crypted)
		return kvtOwner;
	if (_DOC->UnCrypted) 
        _RAISE(ErrFileLevel,fleUncryptedFileError);
	RV = ULCheckPassword ( & ( _DOC->CryptoInfo ), Password, ( ppInt32 ) strlen ( Password ) );
	if (RV == cpCorrupted)
		PDFRAISE(_LIB,PDFBuildErrCode(ErrFileLevel,fleCorruptedFileError));
	if ( RV != cpInvalid ){
		_DOC->UnCrypted = true;
		_DOC->Root=CosGetFromDoc(Doc,_DOC->RootID.ID);
		if (!_IsCosDict(_DOC->Root)) 
			PDFRAISE(_LIB,PDFBuildErrCode(ErrFileLevel,fleCorruptedTrailerError));
		if (_DOC->InfoID.ID) {
			_DOC->Info=CosGetFromDoc(Doc,_DOC->InfoID.ID);
			if (!_IsCosDict(_DOC->Info)) _DOC->Info=_CosNull ( Doc );
		}
	}
	switch (RV){
		case cpValidOwner: return kvtOwner;
		case cpValidUser: return kvtUser;
		default: return kvtInvalid;
	}
}

LIB_API void PDFDocSetEncryptMetaData( PDFDocHandle Doc, ppBool CryptMetadata)
{
	_DOC->CryptMetadata = CryptMetadata;
}


LIB_API void PDFDocRemoveSecurity( PDFDocHandle Doc)
{
	_DOC->UseOldSecurity=false;
	_DOC->NewCrypted=false;
}

void PDFDocSetSecurity(PDFDocHandle Doc, ppInt32 Permission, TPDFProtectionType KeyLength, char *UserPassword, char *OwnerPassword){
	ppInt32 i;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	if (KeyLength >= pt128BitAESProtection)
		CheckEdition(_LIB, STD);
	_DOC->UseOldSecurity=false;
	if (!(_DOC->OPassword)) mfree(_LIB, _DOC->OPassword);
	if (!(_DOC->UPassword)) mfree(_LIB, _DOC->UPassword);
	_DOC->OPassword=NULL;
	_DOC->UPassword=NULL;
	_DOC->NewCrypted=true;
	_DOC->NewPermission=Permission;
	_DOC->nkl=KeyLength;
	/*if ( KeyLength > pt128BitProtection )
		_DOC->nkl= pt128BitProtection;*/
	if (OwnerPassword){
		i=(ppInt32)strlen(OwnerPassword)+1;
		_DOC->OPassword= (char *) mmalloc(_LIB,i);
		memcpy(_DOC->OPassword,OwnerPassword,i);
	};
	if (UserPassword){
		i=(ppInt32)strlen(UserPassword)+1;
		_DOC->UPassword= (char *) mmalloc(_LIB,i);
		memcpy(_DOC->UPassword,UserPassword,i);
	};
}


ppInt32 PDFDocGetPermission (PDFDocHandle Doc){
	ppInt32 i=0;
	if (!_DOC->Crypted) return (ppInt32)0xFFFFFFFF;
	if (_DOC->CryptoInfo.Permission & 4) i|=prPrint;
	if (_DOC->CryptoInfo.Permission & 8) i|=prModifyContent;
	if (_DOC->CryptoInfo.Permission & 16) i|=prCopyInformation;
	if (_DOC->CryptoInfo.Permission & 32) i|=prModifyAnnotation;
	if (_DOC->CryptoInfo.Revision==3){
		if (_DOC->CryptoInfo.Permission & 2048) i|=prPrintHiResolution;
		if (_DOC->CryptoInfo.Permission & 256) i|=prFillAcroForm;
		if (_DOC->CryptoInfo.Permission & 1024) i|=prAssembleDocument;
		if (_DOC->CryptoInfo.Permission & 512) i|=prExtractTextAndImage;
	};
	return i;
}

void PDFDocLoadPageCatalog(PDFDocHandle Doc, PDFCosHandle Catalog,ppInt32 *Counter, PDFCosHandle InhPageAttr){
	PDFCosHandle obj, inh, w, w1;
	PPDFPageInfo page;
	ppUns32 i;
	inh=CosCopy( Doc, InhPageAttr);
	PDFTRY(_LIB){
        w=_CosDictValueByName(Catalog,Resources);
        if (_IsCosDict ( w ) ) 
            _CosDictAppend ( inh, Resources, CosCopy ( Doc, w ) );
        w=_CosDictValueByName(Catalog,MediaBox);
        if (_IsCosArray ( w ) && _CosArrayCount (w) == 4 ) 
            _CosDictAppend (inh, MediaBox, CosCopy ( Doc, w ) );

        w=_CosDictValueByName(Catalog,CropBox);
        if (_IsCosArray ( w ) && _CosArrayCount (w) == 4 ) 
            _CosDictAppend (inh, CropBox, CosCopy ( Doc, w ) );

        w=_CosDictValueByName(Catalog,Rotate);
        if (_IsCosNumber ( w ) ) 
            _CosDictAppend (inh, Rotate, CosCopy ( Doc, w ) );

        obj=_CosDictValueByName(Catalog,Kids);
        if (!_IsCosArray(obj))
            _RAISE(ErrFileLevel,fleCannotLoadPagesError);
        for (i=0;i<_CosArrayCount ( obj ); i++){
            w=CosArrayItem(obj,i);
            if (!_CosIndirectDict(w)) _RAISE(ErrFileLevel,fleCannotLoadPagesError);
            if (!_IsCosName((w1=_CosDictValueByName(w,Type))))
                _RAISE(ErrFileLevel,fleCannotLoadPagesError);
            if (GetAtomDoc(Page)==_CosNameValue(w1)){
                page = ( PPDFPageInfo ) mmalloc(_LIB,sizeof(TPDFPageInfo));
                PDFTRY(_LIB){
                    page->PO=w;
                    page->Doc=Doc;
                    if (_CosDictCount ( inh ) ){
                        page->Inh =  CosCopy ( Doc, inh );
                        if (!_IsCosNull( _CosDictValueByName(w,MediaBox)))
                            _CosDictRemoveKey(page->Inh,MediaBox);
                        if (!_IsCosNull( _CosDictValueByName(w,CropBox)))
                            _CosDictRemoveKey(page->Inh,CropBox);
                        if (!_IsCosNull( _CosDictValueByName(w,Rotate)))
                            _CosDictRemoveKey(page->Inh,Rotate);
                        if (!_IsCosNull( _CosDictValueByName(w,Resources)))
                            _CosDictRemoveKey(page->Inh,Resources);
                        if (!_CosDictCount ( inh )) {
                            CosFree(page->Inh);
                            page->Inh=_CosNull( Doc );
                        }
                    } else 
                        page->Inh=_CosNull( Doc );
                    ULListAdd(_DOC->Pages,(PDFHandle)page);
                    (*Counter)++;
                } PDFEXCEPT(_LIB){
                    mfree(_LIB, page);
                    PDFRERAISE(_LIB);
                }
                PDFTRYEND(_LIB);
            } else if (GetAtomDoc(Pages)==_CosNameValue(w1))
                PDFDocLoadPageCatalog(Doc,w,Counter,inh);
            else _RAISE(ErrFileLevel,fleCannotLoadPagesError);
        }
    } PDFFINALLY(_LIB){
            CosFree(inh);
        }
	PDFFINALLYEND(_LIB);
}


void PDFDocLoadPages(PDFDocHandle Doc){
	PDFCosHandle wrk,w;
	ppInt32 k, si;
	ppUns32 i;
	if (_DOC->Check && !_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	if (!_IsCosDict((wrk=_CosDictValueByName(_DOC->Root,Pages)))) 
		_RAISE(ErrFileLevel,fleCannotLoadPagesError);
	if (!_IsCosInt((w=_CosDictValueByName(wrk,Count)))) 
		_RAISE(ErrFileLevel,fleCannotLoadPagesError);
	k=_CosIntValue(w);
	if (_DOC->Pages){
		for (i=0;i<_DOC->Pages->Size;i++){ 
			CosFree ( ( ( PPDFPageInfo ) _DOC->Pages->FList[i] )->Inh );
			mfree(_LIB, _DOC->Pages->FList[i]);
		}
		ULListFree(_DOC->Pages);
	}
	_DOC->Pages=ULListCreate(_LIB,k);
	w=CosDictNew(Doc,false,0);
	PDFTRY(_LIB)
		si=0;
	PDFDocLoadPageCatalog(Doc,wrk,&si,w);
	PDFFINALLY(_LIB)
		CosFree ( w );
	PDFFINALLYEND(_LIB)
}


ppInt32 PDFDocGetPageCount(PDFDocHandle Doc){
	if (!_DOC->Pages) PDFDocLoadPages(Doc);
	return _DOC->Pages->Size;
}



void PDFDocRedrawAcroformAppend( PDFDocHandle Doc){
	PDFCosHandle wrk, w1, obj;

	_DOC->NeedAppendReDrawAcroScript = false;
	wrk = CosStreamNew (Doc, true,2);
	ULStreamWriteBuffer (_CosStreamValue(wrk),AcroRedrawScript1, (ppUns32)strlen(AcroRedrawScript1));
	ULStreamWriteBuffer (_CosStreamValue(wrk),AcroRedrawScript2, (ppUns32)strlen(AcroRedrawScript2));
	CosStreamFlatePack ( _LIB, wrk );
	_CosDictAppend ( wrk, Length , CosIntNew ( Doc, false,	ULStreamGetSize ( _CosStreamValue ( wrk ) ) ) );
	w1 = CosDictNew( Doc, true, 2);
	_CosDictAppend (w1, S, _CosNameNew(JavaScript) );
	_CosDictAppend (w1, JS, wrk);
	obj = _CosDictValueByName ( _DOC->Root, Names );
	if ( !_IsCosDict ( obj ) ){
		obj = CosDictNew (  Doc, true, 1 );
		_CosDictAppend ( _DOC->Root, Names, obj );
		wrk = CosDictNew ( Doc, true, 1 );
		_CosDictAppend ( obj, JavaScript, wrk );
	} else{
		wrk = _CosDictValueByName ( obj, JavaScript );
		if ( _CosGetType ( wrk ) != CosDict ){
			wrk = CosDictNew ( Doc, true, 1 );
			_CosDictAppend ( obj, JavaScript,  wrk );
		};
	}
	ULSetObjToNameTree ( Doc, wrk, w1, RedrawJSName, (ppUns32)strlen(RedrawJSName) );
}

void PDFDocSetAppendRedrawAcroForm(PDFDocHandle Doc){
	_DOC->NeedAppendReDrawAcroScript=true;
}

void PDFDocSaveToStream(PDFDocHandle Doc, PDFStreamHandle Strm){
	PDFCosHandle obj;
	ppUns32 i;

	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);


	if ( _DOC->AcroFormList ){
		for ( i = 0; i < _DOC->AcroFormList->Size; i++ ){
			if ( !(((PPDFAcroObject) (_DOC->AcroFormList->FList [ i ] ) )->Stored )){
				PDFAcroObjectDrawIt ( Doc, i );
			}
		}
	}

	PDFTTFFontUpdate(Doc);


	if (_IsCosNull(_DOC->Info)){
		obj=CosDictNew(Doc,true,2);
		_CosDictAppend(obj,Producer,CosStringNew(Doc,false,PDFProducer,
            (ppInt32)strlen(PDFProducer)));
		_DOC->Info=obj;
	}
	if (_DOC->AcroValuesWasModified){
		obj = _CosDictValueByName(_DOC->Root, AcroForm);
		if (_IsCosDict(obj))
			_CosDictAppend( obj, XFA, _CosNull(Doc));
		_DOC->AcroValuesWasModified = false;
	}
	if (_DOC->NeedAppendReDrawAcroScript){
		PDFDocRedrawAcroformAppend(Doc);
		PDFAcroFreeForms ( Doc );
		PDFAcroLoadForms ( Doc );
	}
#ifndef NOT_USE_SIGN
    if (_DOC->Signed)
        PDFDocSaveUnPackedUnLinearized(Doc, Strm);
    else
#endif
	if (_DOC->Linearized)
		if (_DOC->Packed) PDFDocSavePackedLinearized(Doc, Strm);
		else PDFDocSaveUnPackedLinearized(Doc, Strm);
	else
		if (_DOC->Packed) PDFDocSavePackedUnLinearized(Doc, Strm);
		else PDFDocSaveUnPackedUnLinearized(Doc, Strm);
#ifndef NOT_USE_SIGN
	if (_DOC->Signed){
		_DOC->Signed=false;
		sig_DisposeContext(_LIB, _DOC->SigCtx);
		_DOC->SigCtx=NULL;
	}
#endif
}


void *PDFDocSaveToBuffer(PDFDocHandle Doc, ppInt32 *size){
	PDFStreamHandle fl;
    PDFHandle buf;
	fl=ULStreamMemNew(_LIB,0);
    PDFTRY(_LIB){
        PDFDocSaveToStream(Doc,fl);
    } PDFEXCEPT(_LIB){
        ULStreamClose(fl);
        PDFRERAISE(_LIB);
    }
	PDFTRYEND(_LIB);
	buf=ULStrmGetMem(fl);
	*size=ULStreamGetSize(fl);
	mfree(_LIB, fl);
	return buf;
}

void PDFDocSaveToFile(PDFDocHandle Doc, char *filename){
	PDFStreamHandle fl;
	fl=ULStreamFileNew(_LIB,filename,ppFileWriteMode);
    PDFTRY(_LIB){
        PDFDocSaveToStream(Doc,fl);
    } PDFFINALLY(_LIB){
        ULStreamClose(fl);
    }
	PDFFINALLYEND(_LIB);
#ifdef WINDOWS_PLATFORM
	if (_DOC->AutoLaunch)
		ShellExecute (0,"open", filename,NULL,NULL,SW_NORMAL);
#endif

}

char * PDFDocGetInfo( PDFDocHandle Doc, TPDFInformation Info, ppUns32 *StrLen ){
	PDFCosHandle obj,obj1;
	ppAtom A = ppAtomNull;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	obj=_DOC->Info;
	if (!_IsCosDict(obj) ) {
		*StrLen = 0;
		return NULL;
	}
	switch(Info) {
		case piProducer:
			if (((PPDFLib)_LIB)->ADM) 
				A = GetAtomDoc ( Producer );
			break;
		case piAuthor:
			A = GetAtomDoc ( Author );
			break;
		case piDate:
			A = GetAtomDoc ( CreationDate );
			break;
		case piTitle:
			A = GetAtomDoc ( Title );
			break;
		case piCreator:
			A = GetAtomDoc ( Creator );
			break;
		case piKeyWords:
			A = GetAtomDoc ( Keywords );
			break;
		case piSubject:
			A = GetAtomDoc ( Subject );
			break;
		case piModificationData:
			A = GetAtomDoc ( ModDate );
			break;
		default:;
	};
	obj1 = CosDictValueByName ( obj, A );
	if ( !_IsCosString ( obj1 )){
		*StrLen = 0;
		return NULL;
	}
	return CosStringGetValue( obj1, StrLen );
}

void PDFDocSetInfo( PDFDocHandle Doc, TPDFInformation Info, char * Str, ppUns32 StrLen )
{
	PDFCosHandle obj, Value;
	ppAtom A = ppAtomNull;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	obj=_DOC->Info;
	if (! _IsCosDict ( obj )){
		obj=CosDictNew(Doc,true,2);
		_CosDictAppend(obj,Producer,CosStringNew(Doc,false, PDFProducer,(ppInt32)strlen(PDFProducer)));
		_DOC->Info=obj;
	};
	switch(Info) {
		case piProducer:
			if (((PPDFLib)_LIB)->ADM) 
				A = GetAtomDoc ( Producer );
			break;
		case piAuthor:
			A = GetAtomDoc ( Author );
			break;
		case piDate:
			A = GetAtomDoc ( CreationDate );
			break;
		case piTitle:
			A = GetAtomDoc ( Title );
			break;
		case piCreator:
			A = GetAtomDoc ( Creator );
			break;
		case piKeyWords:
			A = GetAtomDoc ( Keywords );
			break;
		case piSubject:
			A = GetAtomDoc ( Subject );
			break;
		case piModificationData:
			A = GetAtomDoc ( ModDate );
			break;
		default:;
	};
	if ( A != ppAtomNull){
		Value = CosStringNew( Doc, false, Str, StrLen );
		CosDictAppend( obj, A, Value );
	}
}


PDFDocHandle PDFDocLoadFromStream(PDFLibHandle Lib, PDFStreamHandle Strm){
	PDFCosHandle obj1;
	PDFCosHandle trl;
	ppInt32 XREFPointer;
	PDFID EncrID, lenID;
	CosDoc tmp;
	PDFID RID, RootID, InfoID;
	CheckEdition(Lib, PRO);
	tmp = ( CosDoc ) mmalloc(Lib,sizeof(TPDFDoc));
	tmp->Size=1;
	tmp->Check = false;
	tmp->Lib=Lib;
    tmp->NullObject.Doc=tmp;
    tmp->NullObject.Type=CosNull;
    tmp->NullObject.IsIndirect=false;
    tmp->NullObject.ObjInfo.Parent=NULL;
	tmp->AutoRemove=false;
    _CosNullNew (tmp, trl);
    _CosNullNew (tmp, tmp->Root);
    _CosNullNew (tmp, tmp->Info);
    _CosNullNew (tmp, tmp->CryptoInfo.FileID);
	tmp->Capacity=1;
	tmp->Pages=NULL;
	tmp->FontList = NULL;
    tmp->ImageList = NULL;
	tmp->GStateList = NULL;
    tmp->AcroFormList = NULL;
    tmp->ActionList = NULL;
	tmp->Strm = Strm;
	tmp->Crypted = false;
	tmp->NewCrypted = false;
	tmp->UseOldSecurity = true;
	tmp->UPassword = NULL;
	tmp->OPassword = NULL;
	tmp->Entries = NULL;
	tmp->Packed = false;
	tmp->Linearized =  true;
	tmp->Remove = true;
	tmp->AutoRemove = false;
    tmp->JpegQuality = 65;
#ifndef NOT_USE_SIGN
	tmp->Signed = false;
	tmp->SigCtx = NULL;
#endif
	tmp->CryptMetadata = true;
	tmp->AcroIsLoaded = false;
	tmp->AcroItems = NULL;
	tmp->AcroValuesWasModified = false;
	tmp->NeedAppendReDrawAcroScript = false;

#ifdef WINDOWS_PLATFORM
    tmp->AutoLaunch = false;
	tmp->TTFList = NULL;
	tmp->DC = GetDC(0);
	tmp->UseScreen = true;
    tmp->EmfImagesAsJpeg = false;
    tmp->BWImagesAsJBIG2 = false;
#endif
	PDFTRY(Lib)
		tmp->Entries=( TXREFEntry * )mmalloc(Lib,sizeof(TXREFEntry));
		tmp->Entries[0].Offset=ppMAXUns32;
		tmp->Entries[0].Used=etFree;
		tmp->Entries[0].Generation=0;
		_CosNullNew (tmp, tmp->Entries[0].Obj);
		tmp->Entries[0].Loaded=false;
		tmp->Crypted=false;
		ULCheckDocumentHeader((PDFDocHandle)tmp);
		ULCheckLinearization((PDFDocHandle)tmp,&lenID);
		XREFPointer=ULGetXRefPosition((PDFDocHandle)tmp);
		do {
			ULReadOneXRef((PDFDocHandle)tmp, &XREFPointer,&trl, &RID);
			if (RID.ID!=ppMAXUns32){
				if (tmp->Entries[RID.ID].Loaded) PDFDocDeleteObjEx((PDFDocHandle)tmp,RID.ID);
				else tmp->Entries[RID.ID].Used=etFree;
			}

		} while(XREFPointer!=-1 );
		obj1=CosDictValueByNameNRF(trl,GetAtomLib(Lib,Root));
		if (!_IsCosRef ( obj1 ) ) 
            PDFRAISE(Lib,PDFBuildErrCode(ErrFileLevel,fleCorruptedTrailerError));
		RootID=_CosRefValue(obj1);
		obj1=CosDictValueByNameNRF(trl,GetAtomLib(Lib,Info));
		if (!_IsCosRef(obj1)) InfoID.ID=0;
		else InfoID=_CosRefValue(obj1);
		obj1=CosDictValueByNameNRF(trl,GetAtomLib(Lib,Encrypt));
		if (!_IsCosRef(obj1)) EncrID.ID=0;
		else EncrID=_CosRefValue(obj1);
		tmp->CryptoInfo.Algorithms = _CosNull(tmp);
		tmp->CryptoInfo.AlgList = NULL;
		tmp->UnCrypted = true;
		ULSetEncryptOptions((PDFDocHandle)tmp,EncrID, trl);
		if (lenID.ID!=ppMAXUns32) PDFDocDeleteObjEx(tmp,lenID.ID);	
		PDFDocPackFree(tmp);
		CosFree(trl);
		if ( ! tmp->Crypted ){
			tmp->Root=CosGetFromDoc(tmp,RootID.ID);
			if (!_IsCosDict(tmp->Root)) 
				PDFRAISE(Lib,PDFBuildErrCode(ErrFileLevel,fleCorruptedTrailerError));
			if (InfoID.ID) {
				tmp->Info=CosGetFromDoc(tmp,InfoID.ID);
				if (!_IsCosDict(tmp->Info)) tmp->Info=CosNullNew ( tmp );
			}
		} else {
			tmp->RootID = RootID;
			tmp->InfoID = InfoID;
			tmp->Root = _CosNull(tmp);
			tmp->Info = _CosNull(tmp);
		}
    PDFEXCEPT(Lib)
		CosFree(trl);
        CosFree(tmp->CryptoInfo.FileID);
		if (tmp->Entries) mfree(Lib, tmp->Entries);
		CosFree (tmp->CryptoInfo.Algorithms );
		chfree( Lib, tmp->CryptoInfo.AlgList);
		mfree(Lib, tmp);
		PDFRERAISE(Lib);
	PDFTRYEND(Lib);
	tmp->Check = true;
	return tmp;
}


PDFDocHandle PDFDocLoadFromFile(PDFLibHandle Lib,char *filename){
	PDFStreamHandle ms;
    PDFDocHandle Doc;
	ms=ULStreamFileNew(Lib,filename,ppFileReadMode);
	PDFTRY(Lib){
		Doc=PDFDocLoadFromStream(Lib,ms);
		_DOC->AutoRemove=true;
	}PDFEXCEPT(Lib){
		ULStreamClose(ms);
		PDFRERAISE(Lib);
	}PDFTRYEND(Lib)
	return Doc;
}

PDFDocHandle PDFDocLoadFromBuffer(PDFLibHandle Lib, void *buffer, ppInt32 bufferlen){
	PDFStreamHandle ms;
    PDFDocHandle Doc;
	ms=ULStreamMemReadOnlyNew(Lib,buffer,bufferlen);
    PDFTRY(Lib){
		Doc=PDFDocLoadFromStream(Lib,ms);
		_DOC->AutoRemove=true;
    } PDFEXCEPT(Lib){
		ULStreamClose(ms);
		PDFRERAISE(Lib);
    } PDFTRYEND(Lib)
	return Doc;
}


void PDFDocSetPacked(PDFDocHandle Doc, ppBool Packed){
	if (Packed){
		if (_DOC->Version<pdfver15) _DOC->Version=pdfver15;
		_DOC->Packed=true;
	} else	_DOC->Packed=false;
}
void PDFDocSetRemoveUnUsed(PDFDocHandle Doc, ppBool Remove){
	CheckEdition(_LIB, STD);
	_DOC->Remove = Remove;
}

void PDFDocSetLinearized(PDFDocHandle Doc, ppBool Linearized){
	CheckEdition(_LIB, STD);
	_DOC->Linearized = Linearized;
}


void PDFDocRemoveInheritedFromPages(PDFDocHandle Doc){
	ppUns32 i,j;
	PDFCosHandle obj,t;
	for (i=0;i<_DOC->Pages->Size;i++){
		if (_IsCosDict((t=((PPDFPageInfo)_DOC->Pages->FList[i])->Inh))){
			obj=((PPDFPageInfo)_DOC->Pages->FList[i])->PO;
			for (j=0; j<_CosDictCount(t);j++){
				CosDictAppend(obj,_CosDictKey(t,j),CosCopy( Doc, _CosDictValue(t,j)));
			}
			CosFree(((PPDFPageInfo)_DOC->Pages->FList[i])->Inh);
			((PPDFPageInfo)_DOC->Pages->FList[i])->Inh=CosNullNew ( Doc );
		}
	}
}

LIB_API PDFCosHandle PDFDocGetRoot( PDFDocHandle Doc ){
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	CheckEdition(_LIB, STD);
	return _DOC->Root;
}

ppInt32 PDFDocAppendPage(PDFDocHandle Doc, ppReal Width, ppReal Height){
	PDFCosHandle obj,obj1,page,ar;
	PPDFPageInfo pg;
	ppReal UU, AllMax;
    ppInt32 i;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	if (!_DOC->Pages) PDFDocLoadPages(Doc);
	obj=_CosDictValueByName(_DOC->Root,Pages);
	obj1=_CosDictValueByName(obj,Count);
    if (!_IsCosInt(obj1)) 
        _RAISE(ErrCosLevel,cleIllegalCosObjectWasFoundError);
	CosIntSetValue(obj1,_CosIntValue(obj1)+1);
	page = CosDictNew(Doc,true,5);
	_CosDictAppend(page,Type,_CosNameNew(Page));
	_CosDictAppend(page,Parent, obj);

	AllMax = max(Width,Height);
	AllMax = _abs(AllMax);
	if (AllMax > 72*200){
		UU = floor( AllMax / (72*200))+1;
	} else {
		UU = 1.0;
	}
	if (UU != 1.0){
		_CosDictAppend(page, UserUnit, CosRealNew(Doc, false, UU));
	}
	_CosDictAppend(page,MediaBox,ar=CosArrayNew(Doc, false, 4));
	CosArrayAppend(ar,CosIntNew(Doc,false,0));
	CosArrayAppend(ar,CosIntNew(Doc,false,0));
	CosArrayAppend(ar,CosRealNew(Doc,false,Width/UU));
	CosArrayAppend(ar,CosRealNew(Doc,false,Height/UU));
	_CosDictAppend(page, Resources, CosDictNew (Doc, false, 4) );
	obj1 = _CosDictValueByName(obj,Kids);
	if (!_IsCosArray(obj1)) 
        _RAISE(ErrCosLevel,cleIllegalCosObjectWasFoundError);
	CosArrayAppend(obj1,page);
	pg= ( PPDFPageInfo ) mmalloc(_LIB,sizeof(TPDFPageInfo));
	pg->Doc=Doc;
	pg->PO=page;
	pg->Inh=CosNullNew(Doc);
    PDFTRY (_LIB){
        i = ULListAdd(_DOC->Pages,pg);
    } PDFEXCEPT ( _LIB ){
        mfree(_LIB, pg);
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
	return i;
}
static ppInt32 PDFPageSize[]={792,612,842,595,1190,842,1008,612,728,516,649,459,792,595,1031,728,595,419,936,612,756,522,
1031,728,708,499,459,323,623,312,540,279,639,279,684,297,747,324};

ppInt32 PDFDocAppendPage2(PDFDocHandle Doc, TPDFPageSize Size, TPDFPageOrientation Orientation)
{
	ppReal Width, Height;

	Height = ( ppReal )PDFPageSize[ 2 * (ppInt32) Size ];
	Width = ( ppReal )PDFPageSize[ 2 * (ppInt32) Size + 1 ];

	if ( Orientation == poPageLandScape )
        return PDFDocAppendPage( Doc, Height, Width );
	else
        return PDFDocAppendPage( Doc, Width, Height );
}

void PDFDocSetJpegImageQuality (PDFDocHandle Doc, ppInt32 Quality)
{
    if ( Quality < 0 || Quality > 100 )
        _RAISE ( ErrGeneralLevel, gleInvalidJpegQualityError );
    CD(Doc)->JpegQuality = ( ppUns8 ) Quality;
}

#ifdef WINDOWS_PLATFORM
void PDFDocSetAutoLaunch(PDFDocHandle Doc, ppBool AutoLaunch){
	_DOC->AutoLaunch =  AutoLaunch;
}

void PDFDocSetUsedDC(PDFDocHandle Doc, HDC DC){
	if (_DOC->UseScreen)
		ReleaseDC(0,_DOC->DC);
	else 
		DeleteDC(_DOC->DC);
	if (!DC){
		_DOC->DC = GetDC(0);
		_DOC->UseScreen = true;
	} else {
		_DOC->DC = CreateCompatibleDC (DC);
		_DOC->UseScreen = false;
	}
}

void PDFDocSetEMFColorImagesAsJpeg ( PDFDocHandle Doc, ppBool AsJpeg )
{
    _DOC->EmfImagesAsJpeg = AsJpeg;
}

void PDFDocSetEMFBWImagesAsJBIG2 ( PDFDocHandle Doc, ppBool AsJBIG2 )
{
     _DOC->BWImagesAsJBIG2 = AsJBIG2;
}


#endif


void PDFDocAppendFileFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 BufferSize, char * EmbedFileName,char * Title, ppUns32 TitleLength, ppBool PackData){
	PDFStreamHandle ms;
	ms = ULStreamMemReadOnlyNew ( _LIB, Buffer, BufferSize );
	PDFTRY(_LIB){
		PDFDocAppendFileFromStream(Doc,ms, EmbedFileName,  Title, TitleLength, PackData);
	}PDFFINALLY ( _LIB ){
		ULStreamClose(ms);
	}PDFFINALLYEND ( _LIB );
}

void PDFDocAppendFileFromDisk ( PDFDocHandle Doc, char * FileName, char * EmbedFileName, char * Title, ppUns32 TitleLength,ppBool PackData){
	PDFStreamHandle fl;
	fl=ULStreamFileNew(_LIB,FileName,ppFileReadMode);
	PDFTRY(_LIB){
		PDFDocAppendFileFromStream(Doc,fl, EmbedFileName, Title, TitleLength, PackData);
	} PDFFINALLY(_LIB){
		ULStreamClose(fl);
	}PDFFINALLYEND(_LIB);
}

void PDFDocAppendFileFromStream ( PDFDocHandle Doc, PDFStreamHandle AStream, char * EmbedFileName,char * Title, ppUns32 TitleLength, ppBool PackData){
	PDFStreamHandle St;
	PDFCosHandle Obj, w1, w2, FS, arr, tmp;
	ppUns32 Len, i, k;
	ppInt32 z;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	Obj = CosStreamNew( Doc, true, 3);
	St = _CosStreamValue(Obj);
	ULStreamCopyToStream(AStream, St);
	Len = _StreamSize(St);
	if (PackData){
		CosStreamFlatePack( _LIB, Obj);
		St = _CosStreamValue(Obj);
	}
	tmp = _CosStreamAttr(Obj);
	_CosDictAppend(tmp, Length, CosIntNew(Doc, false, _StreamSize(St)));
	_CosDictAppend(tmp,Params, w1 = CosDictNew(Doc, false, 1));
	_CosDictAppend(w1, Size, CosIntNew(Doc, false, Len));
	FS = CosDictNew( Doc, true, 3);
	_CosDictAppend( FS, Type, _CosNameNew(Filespec));
	CosDictAppend( FS, ULStringToAtom(_LIB, "Desc"), CosStringNew(Doc,false,Title, TitleLength));
	_CosDictAppend( FS, F, CosStringNew(Doc, false, EmbedFileName, (ppUns32)strlen(EmbedFileName)));
	_CosDictAppend( FS, EF, w1 = CosDictNew(Doc, false, 1));
	_CosDictAppend(w1, F, Obj);
	w1 = _CosDictValueByName( _DOC->Root, Names);
	if (!_IsCosDict(w1))
		_CosDictAppend(_DOC->Root, Names, w1 = CosDictNew(Doc, true, 1));
	w2 = _CosDictValueByName(w1, EmbeddedFiles);
	if (!_IsCosDict(w2))
		_CosDictAppend( w1, EmbeddedFiles, w2 = CosDictNew(Doc,true,1 ));
	arr = _CosDictValueByName( w2, Names);
	if (!_IsCosArray(arr))
		_CosDictAppend(w2, Names, arr = CosArrayNew(Doc, true, 1));
	k = _CosArrayCount(arr);
	for (i = 0; i< k; i+=2 ){
		w1 = CosArrayItem(arr,i);
		if (!_IsCosString(w1))
			continue;
		z = memcmp(Title, _CosStringValue(w1), min(TitleLength, _CosStringSize(w1)));
		if (!z) {
			if (TitleLength < _CosStringSize ( w1 )) 
				z = -1;
			if (TitleLength > _CosStringSize ( w1 )) 
				z = 1;
		}
		if (z < 0 )
			break;
	}
	CosArrayInsert(arr,FS,i);
	CosArrayInsert(arr, CosStringNew(Doc, false, Title,TitleLength),i);
}

void PDFDocAppendStartJavaScript ( PDFDocHandle Doc, char * JSName, char * JS)
{
	PDFCosHandle w1, obj, wrk;
	if (!_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
	w1 = CosDictNew( Doc, true, 2);
	_CosDictAppend (w1, S, _CosNameNew(JavaScript) );
	_CosDictAppend (w1, JS, CosStringNew(Doc, false, JS, (ppUns32)strlen(JS)));
	obj = _CosDictValueByName ( _DOC->Root, Names );
	if ( !_IsCosDict ( obj ) ){
		obj = CosDictNew (  Doc, true, 1 );
		_CosDictAppend ( _DOC->Root, Names, obj );
		wrk = CosDictNew ( Doc, true, 1 );
		_CosDictAppend ( obj, JavaScript, wrk );
	} else{
		wrk = _CosDictValueByName ( obj, JavaScript );
		if ( _CosGetType ( wrk ) != CosDict ){
			wrk = CosDictNew ( Doc, true, 1 );
			_CosDictAppend ( obj, JavaScript,  wrk );
		};
	}
	ULSetObjToNameTree ( Doc, wrk, w1, JSName, (ppUns32)strlen(JSName) );
}


void PDFDocRemoveHiddenLayers ( PDFDocHandle Doc )
{

}