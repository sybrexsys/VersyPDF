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
VSXref.c
*********************************************************************/


#include "VSXref.h"
#include "../VSError.h"
#include "../VSExcept.h"
#include "VSDocAI.h"
#include "../VSBaseA.h"
#include "VSParse.h"
#include "VSCosAI.h"
#include "VSMisc.h"
#include "VSFilter.h"
#include <string.h>

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib


void ULCheckDocumentHeader(PDFDocHandle Doc)
{
    char *st, *st1;
    ppInt32 ver;
    ULStreamSetPosition ( _DOC->Strm, 0 );
    st = ULStreamReadLine ( _DOC->Strm );
	st1 = st;
	st = strchr(st,'%');
	_DOC->HeaderOffset = (ppUns32)(st - st1);
    if ( strlen ( st ) < 8 || memcmp ( st, "%PDF-1.", 7 ) != 0 ){
        mfree ( _LIB, st );
        _RAISE ( ErrCosLevel, cleNoHeaderError );
    };
    ver = st[7] - '0';
    mfree ( _LIB, st );
    _DOC->Version = ( TPDFVersion ) ver;
}
void ULCheckLinearization(PDFDocHandle Doc, PDFID *ID)
{
    PDFCosHandle obj, obj1;
    ppInt32 pos, i, gi;
    TPDFParser Parser;
	ID->ID = ppMAXUns32;
    Parser.Doc = Doc;
    Parser.Stream = _DOC->Strm;
	Parser.GenerateExceptionOnEOF = true;
    pos = ULStreamGetPosition ( _DOC->Strm );
    PDFTRY ( _LIB ){
        ULStreamSetPosition ( _DOC->Strm, 0 );
        obj = ULParseCosObject ( &Parser );
        if ( !_IsCosInt ( obj ) ){
            CosFree (obj );
            goto ex;
        }
        i = _CosIntValue ( obj );
        CosFree ( obj );
        obj = ULParseCosObject ( &Parser );
        if ( !_IsCosInt ( obj ) ){
            CosFree ( obj );
            goto ex;
        }
        gi = _CosIntValue ( obj );
        CosFree ( obj );
        obj = ULParseCosObject ( &Parser );
        if ( !_IsCosWork ( obj ) &&  strcmp ( _CosWorkCommand ( obj ), "obj" ) != 0 ){
            CosFree ( obj );
            goto ex;
        }
        CosFree ( obj );
        obj = ULParseCosObject ( &Parser );
        if ( !_IsCosDict ( obj ) ){
            CosFree ( obj );
            goto ex;
        }
        obj1 = _CosDictValueByName ( obj, Linearized );
		_DOC->Linearized = ( _CosIsNumber ( obj1 ) && CosNumberGetValue ( obj1 ) > 0 )? true:false;
        if ( _DOC->Linearized ){
            ID->ID = i;
            ID->GenID = ( ppUns16 ) gi;
        };
          
        CosFree ( obj );
        ex:;
    } PDFFINALLY ( _LIB ){
        ULStreamSetPosition ( _DOC->Strm, pos );
    }
    PDFFINALLYEND ( _LIB );
}

ppInt32 ULGetXRefPosition(PDFDocHandle Doc)
{
    ppUns32 po, sz, XRefPointer = ppMAXUns32;
    char *st;
    TLexeme lex;
    TPDFParser Parser;
	ppBool Second = false;
    Parser.Doc = Doc;
    Parser.Stream = _DOC->Strm;
	Parser.GenerateExceptionOnEOF = true;
    po = ULStreamGetPosition ( _DOC->Strm );
    sz = ULStreamGetSize ( _DOC->Strm );
    ULStreamSetPosition ( _DOC->Strm, sz > 1024 ? sz - 1024 : 0 );
Seconds:
    while ( ULStreamGetPosition ( _DOC->Strm ) < sz ){
        st = ULStreamReadLine ( _DOC->Strm );
        ULTrimStr ( st );
        if ( !strcmp ( st, "startxref" ) )
            XRefPointer = ULStreamGetPosition ( _DOC->Strm );
        mfree ( _LIB, st );
    }
	if ( XRefPointer == ppMAXUns32 ){
		if ( Second){
			_RAISE ( ErrCosLevel, cleNoStartXRefError );
		}
		else{
			ULStreamSetPosition ( _DOC->Strm, 0 );
			Second = true;
			goto Seconds;
		}
	}
    ULStreamSetPosition ( _DOC->Strm, XRefPointer );
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltInteger ){
        ULClearLexeme ( _LIB, &lex );
        _RAISE ( ErrCosLevel, cleNoStartAddressError );
    }
    ULStreamSetPosition ( _DOC->Strm, po );
    return lex.IntValue;
}
void ULReadOneXRef(PDFDocHandle Doc, ppInt32 *pos, PDFCosHandle *trl, PDFID *ID)
{
    PDFCosHandle obj1;
    PDFCosHandle obj;
    TLexeme lex;
    TPDFParser Parser;
    Parser.Doc = Doc;
    Parser.Stream = _DOC->Strm;
	Parser.GenerateExceptionOnEOF = true;
    ULStreamSetPosition ( _DOC->Strm, *pos + _DOC->HeaderOffset  );
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type == ltCommand && !strcmp ( ( char * ) lex.Data, "xref" ) ){
        ULClearLexeme ( _LIB, &lex );
        ID->ID = ppMAXUns32;
        ULReadXRefTable ( Doc, pos, trl );
        return;
    };
    if ( lex.Type != ltInteger ){
        ULClearLexeme ( _LIB, &lex );
        _RAISE ( ErrCosLevel, cleBadXrefError );
    };
    ID->ID = lex.IntValue;
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltInteger ){
        ULClearLexeme ( _LIB, &lex );
        _RAISE ( ErrCosLevel, cleBadXrefError );
    };
    ID->GenID = ( ppUns16 ) lex.IntValue;
    ULGetLexeme ( &Parser, &lex );
    if ( lex.Type != ltCommand && !strcmp ( ( char * ) lex.Data, "obj" ) ){
        ULClearLexeme ( _LIB, &lex );
        _RAISE ( ErrCosLevel, cleBadXrefError );
    };
    ULClearLexeme ( _LIB, &lex );
    obj = ULParseCosObject ( &Parser );
    if ( !_IsCosStream( obj ) ){
        CosFree ( obj );
        _RAISE ( ErrCosLevel, cleBadXrefError );
    }
    obj1 = _CosDictValueByNameNRF ( obj, Type );
    if ( _IsCosName ( obj1 ) && _CosNameValue ( obj1 ) == GetAtomDoc ( XRef ) ){
        PDFTRY ( _LIB ){
            ULReadXRefStream ( Doc, obj, pos, trl );
        } PDFFINALLY ( _LIB ){
            CosFree ( obj );
        }
        PDFFINALLYEND ( _LIB );
        return;
    };
    CosFree ( obj );
    _RAISE ( ErrCosLevel, cleBadXrefError );
}
void ULReadXRefTable(PDFDocHandle Doc, ppInt32 *pos, PDFCosHandle *trl)
{
    TLexeme lex;
    ppUns32 i;
	ppInt32 si;
    PDFCosHandle obj, obj1, wk;
    PDFID l, RID;
    ppUns32 First, Count;
    TPDFParser Parser;
    Parser.Doc = Doc;
    Parser.Stream = _DOC->Strm;
	Parser.GenerateExceptionOnEOF = true;
    while ( true ){
        ULGetLexeme ( &Parser, &lex );
        if ( lex.Type == ltCommand && !strcmp ( ( char * ) lex.Data, "trailer" ) ){
            ULClearLexeme ( _LIB, &lex );
            break;
        };
        if ( lex.Type != ltInteger ){
            ULClearLexeme ( _LIB, &lex );
            _RAISE ( ErrCosLevel, cleBadXrefError );
        };
        First = lex.IntValue;
        ULGetLexeme ( &Parser, &lex );
        if ( lex.Type != ltInteger ){
            ULClearLexeme ( _LIB, &lex );
            _RAISE ( ErrCosLevel, cleBadXrefError );
        };
        Count = lex.IntValue;
        if ( _DOC->Size < First + Count ){
            _DOC->Entries = ( TXREFEntry * )mrealloc ( _LIB, _DOC->Entries, 
                sizeof ( TXREFEntry ) * ( First + Count ) );
            for ( i = _DOC->Size; i < First + Count; i++ ){
                _DOC->Entries[i].Offset = ppMAXUns32;
                _DOC->Entries[i].Generation = 0xffff;
                _CosNullNew ( Doc,_DOC->Entries[i].Obj );
                _DOC->Entries[i].Used = etFree;
                _DOC->Entries[i].Loaded = false;
            };
            _DOC->Size = First + Count;
            _DOC->Capacity = First + Count;
        };
        for ( i = First; i < First + Count; i++ ){
            if ( _DOC->Entries[i].Offset == ppMAXUns32 ){
                ULGetLexeme ( &Parser, &lex );
                if ( lex.Type != ltInteger ){
                    ULClearLexeme ( _LIB, &lex );
                    _RAISE ( ErrCosLevel, cleBadXrefError );
                };
                _DOC->Entries[i].Offset = lex.IntValue + _DOC->HeaderOffset ;
                ULGetLexeme ( &Parser, &lex );
                if ( lex.Type != ltInteger ){
                    ULClearLexeme ( _LIB, &lex );
                    _RAISE ( ErrCosLevel, cleBadXrefError );
                };
                _DOC->Entries[i].Generation = ( ppUns16 ) lex.IntValue;
                ULGetLexeme ( &Parser, &lex );
                if ( lex.Type != ltCommand ){
                    ULClearLexeme ( _LIB, &lex );
                    _RAISE ( ErrCosLevel, cleBadXrefError );
                };
                if ( lex.Data[0] == 'n' )
                    _DOC->Entries[i].Used = etUnCompressed;
                else if ( lex.Data[0] == 'f' ) 
                    _DOC->Entries[i].Used = etFree;
                else{
                    ULClearLexeme ( _LIB, &lex );
                    _RAISE ( ErrCosLevel, cleBadXrefError );
                };
                ULClearLexeme ( _LIB, &lex );
            } else{
                ULGetLexeme ( &Parser, &lex );
                ULClearLexeme ( _LIB, &lex );
                ULGetLexeme ( &Parser, &lex );
                ULClearLexeme ( _LIB, &lex );
                ULGetLexeme ( &Parser, &lex );
                ULClearLexeme ( _LIB, &lex );
            };
        };
    };
    obj = ULParseCosObject ( &Parser );
    PDFTRY ( _LIB ){
        if ( !_IsCosDict ( obj ) )
            _RAISE ( ErrCosLevel, cleBadTrailerError );
        obj1 = _CosDictValueByNameNRF ( obj, Prev );
        if ( _IsCosInt ( obj1 ) )
            *pos = _CosIntValue ( obj1 );
        else if ( _IsCosRef ( obj1 ) ){
            l = _CosRefValue ( obj1 );
            *pos = l.ID;
        } else
            *pos = -1;
        wk = *trl;
        if (_IsCosNull( wk ) )
            *trl = CosCopy (_CosDoc ( obj ), obj );
        obj1 = _CosDictValueByName ( obj, XRefStm );
        if ( _IsCosInt ( obj1 ) ){
            si = _CosIntValue ( obj1 );
            ULReadOneXRef ( Doc, &si, trl, &RID );
            if ( RID.ID > 0 ){
                if ( !_DOC->Entries[RID.ID].Loaded )
                    _DOC->Entries[RID.ID].Used = etFree;
            }
        };
    } PDFFINALLY ( _LIB ){
        CosFree ( obj );
    }
    PDFFINALLYEND ( _LIB );
}

void ULReadOneXRefStreamSection(PDFDocHandle Doc, PDFStreamHandle strm, ppInt32 *W, ppUns32 First,
                                ppUns32 Count)
{
    ppInt32 type,j , ch, off, genid;
	ppUns32 i;
    if ( _DOC->Size < First + Count ){
        _DOC->Entries = ( TXREFEntry * )mrealloc ( _LIB, _DOC->Entries, 
            sizeof ( TXREFEntry ) * ( First + Count ) );
        for ( i = _DOC->Size; i < First + Count; i++ ){
            _DOC->Entries[i].Offset = ppMAXUns32;
            _DOC->Entries[i].Generation = 0xffff;
            _CosNullNew ( Doc, _DOC->Entries[i].Obj );
            _DOC->Entries[i].Used = etFree;
            _DOC->Entries[i].Loaded = false;
        };
        _DOC->Size = First + Count;
        _DOC->Capacity = First + Count;
    };
    for ( i = First; i < First + Count; i++ ){
        if ( !W[0] )
            type = 1;
        else{
            type = 0;
            for ( j = 0; j < W[0]; j++ ){
                ch = ULStreamReadChar ( strm );
                if ( ch == EOF )
                    _RAISE ( ErrCosLevel, cleUnexpectedEndStreamError );
                type = ( type << 8 ) + ( ppUns8 ) ch;
            }
        }
        off = 0;
        for ( j = 0; j < W[1]; j++ ){
            ch = ULStreamReadChar ( strm );
            if ( ch == EOF )
                _RAISE ( ErrCosLevel, cleUnexpectedEndStreamError );
            off = ( off << 8 ) + ( ppUns8 ) ch;
        }
        genid = 0;
        for ( j = 0; j < W[2]; j++ ){
            ch = ULStreamReadChar ( strm );
            if ( ch == EOF )
                _RAISE ( ErrCosLevel, cleUnexpectedEndStreamError );
            genid = ( genid << 8 ) + ( ppUns8 ) ch;
        }
        if ( type == 0 ){
            _DOC->Entries[i].Generation = ( ppUns16 ) genid;
            _DOC->Entries[i].Used = etFree;
        } else if ( type == 1 ){
            _DOC->Entries[i].Generation = ( ppUns16 ) genid;
            _DOC->Entries[i].Offset = off + _DOC->HeaderOffset ;
            _DOC->Entries[i].Used = etUnCompressed;
        } else if ( type == 2 ){
            _DOC->Entries[i].Generation = 0;
            _DOC->Entries[i].Offset = off ;
            _DOC->Entries[i].Used = etCompressed;
        } else
            _RAISE ( ErrCosLevel, cleUnexpectedEndStreamError );
    };
}

void ULReadXRefStream(PDFDocHandle Doc, PDFCosHandle strobj, ppInt32 *pos, PDFCosHandle *trl)
{
    PDFStreamHandle strm;
    PDFCosHandle obj, wk;
    ppUns32 i, sz, first;
    ppInt32 W[3];
    ppBool Release  = false;
    if ( ! _IsCosStream ( strobj ) )
        _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
    obj = _CosDictValueByName ( strobj, Size );
    if ( ! _IsCosInt ( obj ) )
        _RAISE ( ErrCosLevel, cleCorruptedXRefStreamError );
    sz = _CosIntValue ( obj );
    if ( sz > _DOC->Size ){
        _DOC->Entries = ( TXREFEntry * )mrealloc ( _LIB, _DOC->Entries, sizeof ( TXREFEntry ) * ( sz ) );
        for ( i = _DOC->Size; i < sz; i++ ){
            _DOC->Entries[i].Offset = ppMAXUns32;
            _DOC->Entries[i].Generation = 0xffff;
            _CosNullNew ( Doc, _DOC->Entries[i].Obj );
            _DOC->Entries[i].Used = etFree;
            _DOC->Entries[i].Loaded = false;
        };
        _DOC->Size = sz;
        _DOC->Capacity = sz;
    };
    _DOC->Packed = true;
    obj = _CosDictValueByName ( strobj, W );
    if ( !_IsCosArray ( obj ) || _CosArrayCount ( obj ) < 3 )
        _RAISE ( ErrCosLevel, cleCorruptedXRefStreamError );
    for ( i = 0; i < 3; i++ ){
        wk = CosArrayItem ( obj, i );
        if ( !_IsCosInt ( wk ) )
            _RAISE ( ErrCosLevel, cleCorruptedXRefStreamError );
        W[i] = _CosIntValue ( wk );
    }
    if ( _CosStreamIsFiltered ( strobj ) ){
        Release = true;
        strm = ULStrmGetUnFiltered ( _LIB, strobj );
    } else
        strm = _CosStreamValue ( strobj );

    PDFTRY ( _LIB ){
        obj = _CosDictValueByName ( strobj, Index );
        if ( _IsCosArray ( obj ) && _CosArrayCount ( obj ) >= 2 ){
            for ( i = 0; i < _CosArrayCount ( obj ); i += 2 ){
                wk = _CosArrayItem ( obj, i );
                if ( !_IsCosInt ( wk ) )
                    _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
                first = _CosIntValue ( wk );
                wk = _CosArrayItem ( obj, i + 1 );
                if ( !_IsCosInt ( wk )  )
                    _RAISE ( ErrCosLevel, cleIllegalCosObjectWasFoundError );
                sz = _CosIntValue (  wk );
                ULReadOneXRefStreamSection ( Doc, strm, W, first, sz );
            }
        } else
            ULReadOneXRefStreamSection ( Doc, strm, W, 0, sz );
        obj = _CosDictValueByName ( strobj, Prev );
        if ( _IsCosInt ( obj ) )
            *pos = _CosIntValue ( obj );
        else
            *pos = -1;
        wk = *trl;
        if ( _IsCosNull( wk ) )
            *trl = CosCopy ( _CosDoc ( strobj ), _CosStreamAttr ( strobj ) );
    } PDFFINALLY ( _LIB ){
        if ( Release )
            ULStreamClose ( strm );
    }
    PDFFINALLYEND ( _LIB );
}

void ULSetEncryptOptions(PDFDocHandle Doc, PDFID EncrID, PDFCosHandle trl)
{
    PDFCosHandle obj, obj1, w, cf,sf,smf;
	ppUns32 i;
	ppAtom FN;
    if ( !EncrID.ID )
        return;
    obj = CosGetFromDoc ( Doc, EncrID.ID);
    if ( !_IsCosDict ( obj ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    obj1 = _CosDictValueByName ( obj, Filter );
    if ( !_IsCosName ( obj1 )  )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    if ( GetAtomDoc ( Standard ) != _CosNameValue ( obj1 ) )
        _RAISE ( ErrCosLevel, cleUnsupportedErcryptAlgorithmError );
    obj1 = _CosDictValueByName ( obj, V );
    if ( !_IsCosInt ( obj1 ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    _DOC->CryptoInfo.Version = _CosIntValue ( obj1 );
    obj1 = _CosDictValueByName ( obj, R );
    if ( !_IsCosInt ( obj1 ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    _DOC->CryptoInfo.Revision = _CosIntValue ( obj1 );
    obj1 = _CosDictValueByName ( obj, P );
    if ( !_IsCosInt( obj1 ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    _DOC->CryptoInfo.Permission = _CosIntValue ( obj1 );
    obj1 = _CosDictValueByName ( obj, Length );
    if ( _IsCosNull ( obj1 ) )
        _DOC->CryptoInfo.KeyLength = 5;
    else{
        if ( !_IsCosInt ( obj1 ) )
            _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
        _DOC->CryptoInfo.KeyLength = ( ppInt32 ) _CosIntValue ( obj1 ) / 8;
    };
    if ( !( ( _DOC->CryptoInfo.Version >= 1 && _DOC->CryptoInfo.Version <= 5) &&
            ( _DOC->CryptoInfo.Revision >= 2 && _DOC->CryptoInfo.Revision <= 5 ) ) )
        _RAISE ( ErrCosLevel, cleUnsupportedErcryptAlgorithmError );
	if ( _DOC->CryptoInfo.Revision == 4 || _DOC->CryptoInfo.Revision == 5){
		obj1 = _CosDictValueByName ( obj, SubFilter );
		if ( !_IsCosNull ( obj1 ) )
			_RAISE ( ErrCosLevel, cleUnsupportedErcryptAlgorithmError );
	}
    obj1 = _CosDictValueByName ( obj, O );
    if ( !_IsCosString ( obj1 )  )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    if ( _CosStringSize ( obj1 ) < 32 )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
	if (_CosStringSize(obj1) == 48)
		memcpy ( _DOC->CryptoInfo.COP, _CosStringValue ( obj1 ), 48 );
	else
		memcpy ( _DOC->CryptoInfo.COP, _CosStringValue ( obj1 ), 32 );
    obj1 = _CosDictValueByName ( obj, U );
    if ( !_IsCosString ( obj1 ))
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    if ( _CosStringSize ( obj1 ) < 32 )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
	if (_CosStringSize(obj1) == 48)
		memcpy ( _DOC->CryptoInfo.CUP, _CosStringValue ( obj1 ), 48 );
	else
		memcpy ( _DOC->CryptoInfo.CUP, _CosStringValue ( obj1 ), 32 );
	obj1 = _CosDictValueByName ( obj, UE );
	if ( _IsCosString ( obj1 )){
		memcpy ( _DOC->CryptoInfo.CUPE, _CosStringValue ( obj1 ), 32 );
	}
	obj1 = _CosDictValueByName ( obj, OE );
	if ( _IsCosString ( obj1 )){
		memcpy ( _DOC->CryptoInfo.COPE, _CosStringValue ( obj1 ), 32 );
	}
	obj1 = _CosDictValueByName ( obj, Perms );
	if ( _IsCosString ( obj1 )){
		memcpy ( _DOC->CryptoInfo.Perm, _CosStringValue ( obj1 ), 16 );
	}
    w = _CosDictValueByName ( trl, ID );
    if ( !_IsCosArray ( w ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    obj1 = CosArrayItemNRF ( w, 0 );
    if ( !_IsCosString ( obj1 ) )
        _RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
    _DOC->CryptoInfo.FileID = CosCopy ( _CosDoc ( obj1 ), obj1 );
	if ( _DOC->CryptoInfo.Version == 4 || _DOC->CryptoInfo.Version == 5){
		cf = _CosDictValueByName( obj, CF );
		if ( !_IsCosDict(cf) && !_IsCosNull( cf ))
			_RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
		sf = _CosDictValueByName (obj, StrF);
		if ( !_IsCosName(sf) && !_IsCosNull(sf))
			_RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
		smf = _CosDictValueByName (obj, StmF);
		if ( !_IsCosName(smf) && !_IsCosNull( smf) )
			_RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
		w = _CosDictValueByName ( obj, EncryptMetadata );
		if ( _IsCosBool ( w ) )
			_DOC->CryptoInfo.EncryptMetadata = _CosBoolValue ( w );
		else 
			_DOC->CryptoInfo.EncryptMetadata = true;
		_DOC->CryptoInfo.RemoveOldFilters = false;
		_DOC->CryptoInfo.DefStringAlg =eaNone;
		_DOC->CryptoInfo.DefStreamAlg =eaNone;
		_DOC->CryptoInfo.Algorithms = CosCopy ( Doc, cf );
		_DOC->CryptoInfo.DefString = _IsCosName ( sf )? _CosNameValue( sf ) : GetAtomDoc( Identity );
		_DOC->CryptoInfo.DefStream = _IsCosName ( smf )? _CosNameValue ( smf ) : GetAtomDoc( Identity );
		if ( _IsCosDict (cf) && _CosDictCount ( cf ) ){
			_DOC->CryptoInfo.AlgList = (PDFAlgorithmItem *)mmalloc ( _LIB, _CosDictCount ( cf )* sizeof( PDFAlgorithmItem ) );
			_DOC->CryptoInfo.AlgListCount = _CosDictCount ( cf );
			for ( i = 0; i < _CosDictCount ( cf ); i++){
				_DOC->CryptoInfo.AlgList[i].Name = _CosDictPair( cf, i ).Key;
				w = _CosDictPair( cf, i ).Value;
				if ( _IsCosRef ( w ))
					w = CosGetFromDoc ( Doc, _CosRefValue(w).ID);
				if (! _IsCosDict ( w ) )
					_RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
				w = _CosDictValueByName ( w, CFM );
				if ( _IsCosName( w ) ){
					FN = _CosNameValue ( w );
					if ( FN == GetAtomDoc ( V2) )
						_DOC->CryptoInfo.AlgList[i].Algorithm = eaRC4;
					else if ( FN == GetAtomDoc ( AESV2) )
						_DOC->CryptoInfo.AlgList[i].Algorithm = eaAES;
					else if ( FN == GetAtomDoc ( AESV3) )
						_DOC->CryptoInfo.AlgList[i].Algorithm = eaAES;
					else if ( FN == GetAtomDoc ( None ) )
						_DOC->CryptoInfo.AlgList[i].Algorithm = eaNone;
					else 
						_RAISE ( ErrCosLevel, cleInvalidEncryptInformationError );
				} else 
					_DOC->CryptoInfo.AlgList[i].Algorithm = eaNone;
				if ( _CosDictPair( cf, i ).Key == _DOC->CryptoInfo.DefString )
					_DOC->CryptoInfo.DefStringAlg = _DOC->CryptoInfo.AlgList[i].Algorithm;
				if ( _CosDictPair( cf, i ).Key == _DOC->CryptoInfo.DefStream )
					_DOC->CryptoInfo.DefStreamAlg = _DOC->CryptoInfo.AlgList[i].Algorithm;
			}
		}
		/* */
	}
    _DOC->Crypted = true;
	_DOC->UnCrypted = false;
    PDFDocDeleteObj ( Doc, EncrID.ID );
}

