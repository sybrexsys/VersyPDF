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
VSCosDocA.c
*********************************************************************/

#include "VSPlatform.h"


#if !defined(MACOSX_PLATFORM)
#include <stdlib.h>
#endif

#include <string.h>

#include "VSCosAI.h"
#include "VSAssert.h"
#include "VSMisc.h"
#include "VSLibI.h"
#include "VSDocAI.h"
#include "VSParse.h"
#include "VSCrypt.h"
#include "VSFilter.h"


#ifdef _LIB
#undef _LIB
#endif

#define _LIB _DOC->Lib
#define _DOC CD(Doc)


void ULUnPackObjStrm( PDFDocHandle Doc, PDFCosHandle ObjStrm )
{
    PDFStreamHandle strm;
    ppUns32 i, j;
    ppBool Release;
    PDFCosHandle attr, obj;
    PDFCosHandle oo;
    ppUns32 ObjCount;
    ppUns32 First;
    ppUns32 *objs   = NULL;
    TLexeme lex;
    TPDFParser Parser;
    if ( !_IsCosStream ( ObjStrm ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    attr = _CosStreamAttr (ObjStrm );
    obj = _CosDictValueByName ( attr,  N );
    if ( !_IsCosInt ( obj ) || ( ObjCount = _CosIntValue ( obj ) ) <= 0 )
        _RAISE (ErrCosLevel, cleInvalidObjectStreamDictionaryError );
    obj = _CosDictValueByName ( attr, First );
    if ( !_IsCosInt ( obj ) || ( First = _CosIntValue ( obj ) ) <= 0 )
        _RAISE ( ErrCosLevel, cleInvalidObjectStreamDictionaryError );
    if ( _CosStreamIsFiltered ( ObjStrm ) ){
        Release = true;
        strm = ULStrmGetUnFiltered ( _LIB, ObjStrm );
    }else{
        Release = false;
        strm = _CosStreamValue ( ObjStrm );
    };
    Parser.Stream = strm;
    Parser.Doc = Doc;
	Parser.GenerateExceptionOnEOF = true;
    PDFTRY ( _LIB ){
        objs = ( ppUns32 * ) mmalloc ( _LIB, ( sizeof ( ppUns32 ) * ObjCount ) << 1 );
        ULStreamSetPosition ( strm, 0 );
        j = ObjCount << 1;
        for ( i = 0; i < j; i += 2 ){
            ULGetLexeme ( &Parser, &lex );
            if ( lex.Type != ltInteger ){
                ULClearLexeme( _LIB, &lex);
                _RAISE ( ErrCosLevel, cleInvalidObjectStreamError );
            }
            objs[i] = lex.IntValue;
            ULGetLexeme ( &Parser, &lex );
            if ( lex.Type != ltInteger ){
                ULClearLexeme( _LIB, &lex);
                _RAISE ( ErrCosLevel, cleInvalidObjectStreamError );
            }
            objs[i + 1] = lex.IntValue;
        }
        for ( i = 0; i < j; i += 2 ){
            if ( objs[i] < _DOC->Size ){
                ULStreamSetPosition ( strm, objs[i + 1] + First );            
                oo = ULParseCosObject ( &Parser );
                if ( _DOC->Entries[objs[i]].Loaded )
                    CosFree ( _DOC->Entries[objs[i]].Obj );
                _DOC->Entries[objs[i]].Loaded = true;
                _DOC->Entries[objs[i]].Generation = 0;
                if ( !_IsCosNull ( oo ) ){
                    _CosObjID ( oo ) = objs[i];
                    _CosObjGeneration ( oo ) = 0;
                    _CosIsIndirect ( oo ) = true;
                }
                _DOC->Entries[objs[i]].Obj = oo;
                _DOC->Entries[objs[i]].Used = etUnCompressed;
            }
        }
    } PDFFINALLY ( _LIB ){
        chfree ( _LIB, objs );
        if ( Release )
            ULStreamClose ( strm );
    }
    PDFFINALLYEND ( _LIB )
}



PDFCosHandle CosGetFromDoc( PDFDocHandle Doc, ppUns32 ID)
{
	TLexeme lex;
    PDFCosHandle obj, o1;
    TPDFParser Parser;
    PDFID IDA;
    if ( ID >= _DOC->Size || ID <= 0 )
        return CosNullNew ( Doc );
    if ( _DOC->Entries[ID].Loaded )
            return _DOC->Entries[ID].Obj;
    if ( _DOC->Entries[ID].Used == etFree )
        return CosNullNew ( Doc );
    if ( _DOC->Entries[ID].Used == etCompressed ){
        CosGetFromDoc ( Doc, _DOC->Entries[ID].Offset );
        if ( _DOC->Entries[ID].Loaded )
                return  _DOC->Entries[ID].Obj ;    
        else
            return CosNullNew ( Doc );
    };
	if (_DOC->Check && !_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
    Parser.Doc = Doc;
    Parser.Stream = _DOC->Strm;
	Parser.GenerateExceptionOnEOF = true;
    ULStreamSetPosition ( _DOC->Strm, _DOC->Entries[ID].Offset );
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltInteger || (ppUns32)lex.IntValue != ID ){
        ULClearLexeme ( _LIB, &lex );
        return CosNullNew ( Doc );
    };
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltInteger ){
        ULClearLexeme ( _LIB, &lex );
        return CosNullNew ( Doc );
    };
    IDA.GenID = ( ppUns16 ) lex.IntValue;
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltCommand || strcmp ( lex.Data, "obj" ) ){
        ULClearLexeme ( _LIB, &lex );
        return CosNullNew ( Doc );
    };
    ULClearLexeme ( _LIB, &lex );
    obj = ULParseCosObject ( &Parser );
    if ( _DOC->Crypted ){
        IDA.ID = ID;
        CosCryptObject ( obj, &( _DOC->CryptoInfo ), IDA, false );
    };
    _DOC->Entries[ID].Loaded = true;
    if ( !_IsCosNull ( obj ) ){
        _CosObjID ( obj ) = ID;
        _CosObjGeneration ( obj ) = IDA.GenID;
        _CosIsIndirect ( obj ) = true;
    }
    _DOC->Entries[ID].Obj = obj;
    if ( _IsCosStream ( obj ) &&
        _IsCosName( (o1 = _CosDictValueByName (obj, Type)))  &&
        _CosNameValue ( o1 ) == GetAtomDoc ( ObjStm ) ){
            ULUnPackObjStrm ( Doc, obj );
            PDFDocDeleteObjEx ( Doc, ID );
            return CosNullNew ( Doc );
        }
     return _DOC->Entries[ID].Obj;
}



void PDFDocDeleteObj( PDFDocHandle Doc, ppUns32 ID )
{
    if ( ID < 1 || ID >= _DOC->Size )
        return ;
    if ( _DOC->Entries[ID].Loaded )
        CosFree ( _DOC->Entries[ID].Obj );
    _DOC->Entries[ID].Offset = ppMAXUns32;
    _DOC->Entries[ID].Used = etFree;
    _DOC->Entries[ID].Loaded = false;
    _CosNullNew(Doc, _DOC->Entries[ID].Obj );
    while ( ID == _DOC->Size - 1 &&
        _DOC->Entries[ID].Used == etFree ){
            --_DOC->Size;
            --( ID );
        };
}

void PDFDocDeleteObjEx( PDFDocHandle Doc, ppUns32 ID )
{
    if ( ID < 1 || ID >= _DOC->Size )
        return ;
    if ( _DOC->Entries[ID].Loaded )
        CosFree ( _DOC->Entries[ID].Obj );
    _DOC->Entries[ID].Offset = ppMAXUns32;
    _DOC->Entries[ID].Used = etFree;
    _DOC->Entries[ID].Loaded = false;
    _CosNullNew ( Doc, _DOC->Entries[ID].Obj );
    _DOC->Entries[ID].Generation = 0xFFFF;
    while ( ID == _DOC->Size - 1 &&
        _DOC->Entries[ID].Used == etFree ){
            --_DOC->Size;
            --( ID );
        };
}

void CosAddToDoc( PDFDocHandle Doc, PDFCosHandle CosObject)
{
    ppUns32 i,ID;
	if (_DOC->Check && !_DOC->UnCrypted)
		_RAISE(ErrFileLevel,fleCryptedFileError);
    ID=_DOC->Size;
    if ( _DOC->Size == _DOC->Capacity ){
        if ( _DOC->Capacity > 64 )
            i = _DOC->Capacity / 4;
        else if ( _DOC->Capacity > 8 )
            i = 16;
        else
            i = 4;
        _DOC->Capacity += i;
        _DOC->Entries = ( TXREFEntry * ) mrealloc ( _DOC->Lib, _DOC->Entries,
             sizeof ( TXREFEntry ) * _DOC->Capacity );
        for ( i = _DOC->Size; i < _DOC->Capacity; i++ ){
            _DOC->Entries[i].Offset = ppMAXUns32;
            _DOC->Entries[i].Generation = 0xFFFF;
            _DOC->Entries[i].Used = etFree;
            _DOC->Entries[i].Loaded = false;
            _CosNullNew( Doc, _DOC->Entries[i].Obj );
        };
    };
    _DOC->Size++;
    _DOC->Entries[ID].Offset = ppMAXUns32;
    _DOC->Entries[ID].Used = etUnCompressed;
    _DOC->Entries[ID].Loaded = true;
    _DOC->Entries[ID].Obj = CosObject;
    _DOC->Entries[ID].Generation = 0;
    if ( !_IsCosNull ( CosObject ) ){
        _CosObjID ( CosObject ) = ID;
        _CosObjGeneration ( CosObject ) = 0;
        _CosIsIndirect ( CosObject ) = true;
    }
}

void PDFDocMarkUsedObjects(PDFDocHandle Doc, PDFCosHandle wobj,ppBool Go, ppBool *Mark){
	PDFCosHandle wrk;
	PDFID WID;
	ppUns32 i;
	switch((ppUns32)_CosGetType(wobj)) {
	case CosRef:
		if (Go){
			WID=_CosRefValue(wobj);
			if (!(Mark[WID.ID])){
				Mark[WID.ID]=true;
				if (_DOC->Entries[WID.ID].Generation!=WID.GenID) 
					WID.GenID=_DOC->Entries[WID.ID].Generation;
				wrk=CosGetFromDoc(Doc,WID.ID);
				if (_IsCosInclude(wrk)) 
					PDFDocMarkUsedObjects(Doc,wrk,Go, Mark);
			}
		}
		_CosRefValue(wobj).GenID = 0;
		break;
	case CosArray:
		for (i=0;i<_CosArrayCount (wobj );i++)
			if (_IsCosIncludeR(_CosArrayItem (wobj, i)))
				PDFDocMarkUsedObjects(Doc,_CosArrayItem (wobj, i),Go,Mark);
		break;
	case CosDict:
		for (i=0;i<_CosDictCount(wobj);i++)
			if (_IsCosIncludeR(_CosDictValue (wobj, i)))
				PDFDocMarkUsedObjects(Doc,_CosDictValue (wobj, i),Go,Mark);
		break;
	case CosStream:
		wobj=_CosStreamAttr(wobj);
		for (i=0;i<_CosDictCount(wobj);i++)
			if (_IsCosIncludeR(_CosDictValue (wobj, i)))
				PDFDocMarkUsedObjects(Doc,_CosDictValue (wobj, i),Go,Mark);
		break;
	default:;
	}

}


void PDFDocRemoveUnUsed(PDFDocHandle Doc, ppInt32 *UsedCount){
	ppUns32 i, tot;
	ppBool *mark;
	mark = (ppBool * ) mmalloc(_LIB,sizeof(ppBool)*_DOC->Size);
	PDFTRY(_LIB){
		memset(mark,0,sizeof(ppBool)*_DOC->Size);
		mark[_CosObjID(_DOC->Root)]=true;
		PDFDocMarkUsedObjects(Doc,_DOC->Root,true, mark);
		if (!_IsCosNull( _DOC->Info ) ){
			mark[_CosObjID(_DOC->Info)]=true;
			PDFDocMarkUsedObjects(Doc, _DOC->Info,true, mark);
		}
#ifndef NOT_USE_SIGN
		if (_DOC->Signed && _DOC->SigCtx->IsVisible ){
			mark[_CosObjID(_DOC->SigCtx->VisibleContent)]=true;
			PDFDocMarkUsedObjects(Doc, _DOC->SigCtx->VisibleContent,true, mark);
		}
#endif
		if (_DOC->Pages){
			for (i=1;i<_DOC->Pages->Size;i++){
				if (!_IsCosNull(((PPDFPageInfo)_DOC->Pages->FList[i])->Inh))
					PDFDocMarkUsedObjects(Doc,((PPDFPageInfo)_DOC->Pages->FList[i])->Inh, false, mark);
			}
		}
		for (i=1, tot=0;i<_DOC->Size;i++){
			if (!mark[i]){
				if (_DOC->Entries[i].Loaded) 
					CosFree(_DOC->Entries[i].Obj);
				_CosNullNew(Doc,_DOC->Entries[i].Obj);
				_DOC->Entries[i].Used=etFree;
				_DOC->Entries[i].Loaded=false;
			} else tot++;
			_DOC->Entries[i].Generation=0;
			if (!_IsCosNull(_DOC->Entries[i].Obj))
				_CosObjGeneration(_DOC->Entries[i].Obj)=0;
		}
		if (UsedCount) *UsedCount=tot;
	}PDFFINALLY(_LIB){
		mfree(_LIB, mark);
	}PDFFINALLYEND(_LIB)
}

void PDFDocRenumObj(PDFDocHandle Doc, PDFCosHandle wobj,ppBool Go){
	PDFCosHandle wrk;
	PDFID WID;
	ppUns32 i;
	switch((ppUns32)_CosGetType(wobj)) {
		case CosRef:
			if (Go){
				WID=_CosRefValue(wobj);
				if (_DOC->Entries[WID.ID].SaveInfo==setSave){
					_DOC->Entries[WID.ID].SaveInfo=setPacked;
					wrk=CosGetFromDoc(Doc,WID.ID);
					if (_IsCosInclude(wrk)) 
						PDFDocRenumObj(Doc,wrk,Go);
				}
			}
			i=_CosRefValue(wobj).ID;
			if (i>0 && i<_DOC->Size){
				_CosRefValue(wobj).ID=_DOC->Entries[i].Additional;
				_CosRefValue(wobj).GenID=0;
			}
			break;
		case CosArray:
			for (i=0;i<_CosArrayCount (wobj );i++)
				if (_IsCosIncludeR(_CosArrayItem (wobj, i)))
					PDFDocRenumObj(Doc,_CosArrayItem (wobj, i),Go);
			break;
		case CosDict:
			for (i=0;i<_CosDictCount(wobj);i++)
				if (_IsCosIncludeR(_CosDictValue (wobj, i)))
					PDFDocRenumObj(Doc,_CosDictValue (wobj, i),Go);
			break;
		case CosStream:
			wobj=_CosStreamAttr(wobj);
			for (i=0;i<_CosDictCount(wobj);i++)
				if (_IsCosIncludeR(_CosDictValue (wobj, i)))
					PDFDocRenumObj(Doc,_CosDictValue (wobj, i),Go);
			break;
		default:;
	}
}



void RenumQuickSort(TXREFEntry *A, ppUns32 iLo, ppUns32 iHi)
{
	ppUns32 Lo, Hi, Mid;
	TXREFEntry T;

	Lo = iLo;
	Hi = iHi;
	Mid = A[(Lo+Hi)/2].Additional;
	do{
		while (A[Lo].Additional < Mid)
			Lo++;
		while (A[Hi].Additional > Mid) 
			Hi--;
		if (Lo <= Hi){
			T = A[Lo];
			A[Lo] = A[Hi];
			A[Hi] = T;
			Lo++;
			Hi--;
		}
	} while (Lo <= Hi);

	if (Hi > iLo) 
		RenumQuickSort(A, iLo, Hi);
	if (Lo < iHi) 
		RenumQuickSort(A, Lo, iHi);
}

int CDECL XREFEntryCMP( const void * FST, const void * SCND)
{
	if ( ((TXREFEntry*)FST)->Additional < ((TXREFEntry*)SCND)->Additional)
		return -1;
	if ( ((TXREFEntry*)FST)->Additional > ((TXREFEntry*)SCND)->Additional)
		return 1;
	return 0;
}

ppInt32 PDFDocRenumAllObjects(PDFDocHandle Doc){
	ppUns32 i;
	PDFCosHandle obj;
	for ( i = 1;i < _DOC->Size; i++){ 
		_DOC->Entries[i].SaveInfo=setSave;
	}
	_DOC->Entries[_CosObjID(_DOC->Root)].SaveInfo=setPacked;
	obj=_DOC->Root;
	PDFDocRenumObj(Doc, obj, true);
	if (!_IsCosNull( _DOC->Info ) ){
		_DOC->Entries[_CosObjID(_DOC->Info)].SaveInfo=setPacked;
		obj=_DOC->Info;
		PDFDocRenumObj(Doc, obj, true);
	}
	if (_DOC->Pages){
		for (i=0;i<_DOC->Pages->Size;i++){
			if (!_IsCosNull(((PPDFPageInfo)_DOC->Pages->FList[i])->Inh))
				PDFDocRenumObj(Doc,((PPDFPageInfo)_DOC->Pages->FList[i])->Inh, false);
		}
	}
	_DOC->Entries[0].Additional = 0;
	RenumQuickSort(_DOC->Entries,1,_DOC->Size-1);
	
	for (i=1;i<_DOC->Size;i++) {
		if (_DOC->Entries[i].Used!=etFree && !_IsCosNull(_DOC->Entries[i].Obj)){
			_CosObjID(_DOC->Entries[i].Obj)=i;
			_CosObjGeneration(_DOC->Entries[i].Obj)=0;
		}
	}
	return 0;
}


void PDFDocCalcer(PDFDocHandle Doc, PDFCosHandle wobj, PDFID ID,ppInt32 *Count){
	PDFCosHandle wrk;
	PDFID WID;
	ppUns32 i;
	switch((ppUns32)_CosGetType(wobj)) {
		case CosRef:
			WID=_CosRefValue(wobj);
			if (ID.ID==WID.ID) (*Count)++;
			wrk=CosGetFromDoc(Doc,WID.ID);
			if (_IsCosInclude(wrk)) 
				PDFDocCalcer(Doc,wrk,ID,Count);
			break;
		case CosArray:
			for (i=0;i<_CosArrayCount (wobj );i++)
				if (_IsCosIncludeR(_CosArrayItem (wobj, i)))
					PDFDocCalcer(Doc,_CosArrayItem (wobj, i),ID,Count);
			break;
		case CosDict:
			for (i=0;i<_CosDictCount(wobj);i++)
				if (_IsCosIncludeR(_CosDictValue (wobj, i)))
					PDFDocCalcer(Doc,_CosDictValue (wobj, i),ID,Count);
			break;
		case CosStream:
			wobj=_CosStreamAttr(wobj);
			for (i=0;i<_CosDictCount(wobj);i++)
				if (_IsCosIncludeR(_CosDictValue (wobj, i)))
					PDFDocCalcer(Doc,_CosDictValue (wobj, i),ID,Count);
			break;
		default:;
	}
}



void PDFDocCalcIDCountUsed(PDFDocHandle Doc, PDFID id, ppInt32 *Count){
	PDFCosHandle root;
	root=_DOC->Root;
	PDFDocCalcer(Doc,root,id,Count);
}

