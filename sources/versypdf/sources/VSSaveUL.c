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
VSSaveUL.c
*********************************************************************/

#include <string.h>

#include "VSDocAI.h"
#include "VSFilter.h"
#include "VSCrypt.h" 
#include "VSCosAI.h"
#include "VSSignAI.h"
#include "VSMisc.h"

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib


void PDFDocSavePackedUnLinearized(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    char str[128];
    ppInt32 xrefoffset, err, IndexPacked=0, j, k, first, maxgen, maxoff;
	ppUns32 i;
    PDFCosHandle obj, ar, z,cr, Pack, attr, wrk;
    PDFStreamHandle packstrm=NULL, addstrm = NULL;
    PDFID ID, EID, PackID, trailID;
    TPDFCryptoType ncr, *UsedCrypto;
    ppBool crypting = false, delID = false;
    _CosNullNew(Doc, cr);
    PackID.ID = 0;
    if ( _DOC->Remove ){
        PDFDocRemoveUnUsed ( Doc, NULL );
        k = 1;
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( ( ( CosDoc ) Doc )->Entries[i].Used != etFree )
                ( ( CosDoc ) Doc )->Entries[i].Additional = k++;
            else
                ( ( CosDoc ) Doc )->Entries[i].Additional = 0x7FFFFFFF;
        };
        PDFDocRenumAllObjects ( Doc );
        PDFDocPackFree ( ( CosDoc ) Doc );
    };
	if ( _DOC->UseOldSecurity ){
		if ( _DOC->Crypted ){
			crypting = true;
			UsedCrypto = &( _DOC->CryptoInfo );
		};
	} else{
		if ( _DOC->NewCrypted ){
			crypting = true;
			ULSetNewCryptoData( Doc, &ncr );
			UsedCrypto = &ncr;
			delID = true;
		};
	};
    PDFTRY ( _LIB ){
        ULStrToStrm ( Strm, "%PDF-1." );
        ULStreamWriteChar ( Strm,(char)(_DOC->Version + '0') );
        ULStrToStrm ( Strm, "\r\n" );
        ULStrToStrm ( Strm, "%\330\302\300\314\r\n" );
        _CosNullNew(Doc, Pack ) ;
        for ( i = 1; i < _DOC->Size; i++ )
            _DOC->Entries[i].SaveInfo = setSave;
        for ( i = 1; i < _DOC->Size; i++ ){
            obj = CosGetFromDoc ( Doc, i );
            if ( _IsCosStream ( obj ) ){
                wrk = _CosDictValueByNameNRF ( obj, Length );
                if ( _IsCosRef( wrk ) ){
                    ID = _CosRefValue ( wrk );
                    if ( ID.ID < _DOC->Size )
                        _DOC->Entries[ID.ID].SaveInfo = setUnPacked;
                }
            }
        }
        if ( crypting )
            _DOC->Entries[_CosObjID ( _DOC->Root )].SaveInfo = setUnPacked;
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used == etFree )
                continue;
            obj = CosGetFromDoc ( Doc, i );
            /*  Проверяем на стрим и на объекты с ненулевым GenID  */
            if ( _IsCosStream( obj ) ||
                 _DOC->Entries[i].Generation > 0 ||
                 _DOC->Entries[i].SaveInfo == setUnPacked ){
                if ( _DOC->Entries[i].SaveInfo == setPacker )
                    continue;
                wrk = crypting ? CosCopy ( Doc, obj ) : obj;
                if ( crypting ){
                    ID.ID = i;
                    ID.GenID = _DOC->Entries[i].Generation;
                    CosCryptObject ( wrk, UsedCrypto, ID, true );
                };
                _DOC->Entries[i].Offset = ULStreamGetPosition ( Strm );
                ULitostr ( i, str );
                ULStrToStrm ( Strm, str );
                ULStreamWriteChar ( Strm, ' ' );
                ULitostr ( _DOC->Entries[i].Generation, str );
                ULStrToStrm ( Strm, str );
                if ( _CosGetType ( wrk ) < CosName )
                    ULStrToStrm ( Strm, " obj " );
                else
                    ULStrToStrm ( Strm, " obj" );
                CosCopyObjToStream ( wrk, Strm );
                if ( crypting )
                    CosFree ( wrk );
                ULStrToStrm ( Strm, "\nendobj\n" );
                _DOC->Entries[i].SaveInfo = setSave;
            } else{
                if ( _IsCosNull( Pack ) || IndexPacked == 200 ){
                    if ( !_IsCosNull ( Pack ) ){
                        first = ULStreamGetPosition ( packstrm );
                        ULStreamCopyToStream ( addstrm, packstrm );
                        ULStreamClose ( addstrm );
                        _DOC->Entries[PackID.ID].Offset = ULStreamGetPosition ( Strm );
                        _CosDictAppend ( Pack, Type, _CosNameNew ( ObjStm ) );
                        _CosDictAppend ( Pack, N, CosIntNew ( Doc, false, IndexPacked ) );
                        _CosDictAppend ( Pack, First, CosIntNew ( Doc, false, first ) );
                        CosStreamFlatePack ( _LIB, Pack );
                        _CosDictAppend ( Pack, Length, CosIntNew ( Doc, false,
                            ULStreamGetSize ( _CosStreamValue ( Pack ) ) ) );
                        wrk = crypting ? CosCopy ( Doc, Pack ) : Pack;
                        if ( crypting )
                            CosCryptObject ( wrk, UsedCrypto, PackID, true );
                        ULitostr ( PackID.ID, str );
                        ULStrToStrm ( Strm, str );
                        ULStrToStrm ( Strm, " 0 obj" );
                        CosCopyObjToStream ( wrk, Strm );
                        if ( crypting )
                            CosFree ( wrk );
                        ULStrToStrm ( Strm, "\nendobj\n" );
                    }
                    Pack = CosStreamNew ( Doc, true, 5 );
					PackID = _CosObjFullID ( Pack );
                    _DOC->Entries[PackID.ID].SaveInfo = setPacker;
                    packstrm = _CosStreamValue ( Pack );
                    IndexPacked = 0;
                    addstrm = ULStreamMemNew ( _LIB, 0 );
                }
                j = ULStreamGetPosition ( addstrm );
                CosCopyObjToStream ( obj, addstrm );
                ULStreamWriteChar ( addstrm, ' ' );
                ULitostr ( i, str );
                ULStrToStrm ( packstrm, str );
                ULStreamWriteChar ( packstrm, ' ' );
                ULitostr ( j, str );
                ULStrToStrm ( packstrm, str );
                ULStreamWriteChar ( packstrm, ' ' );
                _DOC->Entries[i].SaveInfo = setPacked;
                _DOC->Entries[i].Index = IndexPacked;
                _DOC->Entries[i].ObjID = PackID.ID;
                IndexPacked++;
            }
        }
        if ( !_IsCosNull(Pack )){
            first = ULStreamGetPosition ( packstrm );
            ULStreamCopyToStream ( addstrm, packstrm );
            ULStreamClose ( addstrm );
            _DOC->Entries[PackID.ID].Offset = ULStreamGetPosition ( Strm );
            _CosDictAppend ( Pack, Type, _CosNameNew ( ObjStm ) );
            _CosDictAppend ( Pack, N, CosIntNew ( Doc, false, IndexPacked ) );
            _CosDictAppend ( Pack, First, CosIntNew ( Doc, false, first ) );
            CosStreamFlatePack ( _LIB, Pack );
            _CosDictAppend ( Pack, Length, CosIntNew ( Doc, false,
                ULStreamGetSize ( _CosStreamValue ( Pack ) ) ) );
            wrk = crypting ? CosCopy ( Doc, Pack ) : Pack;
            if ( crypting )
                CosCryptObject ( wrk, UsedCrypto, PackID, true );
            ULitostr ( PackID.ID, str );
            ULStrToStrm ( Strm, str );
            ULStrToStrm ( Strm, " 0 obj" );
            CosCopyObjToStream ( wrk, Strm );
            if ( crypting )
                CosFree ( wrk );
            ULStrToStrm ( Strm, "\nendobj\n" );
        }
        if ( crypting ){
			if ( crypting ){
				cr = ULPrepareCryptoDictionary (Doc, UsedCrypto );
				EID= _CosObjFullID (cr);
			};
			if (((CosDoc)Doc)->Version >= 9){
				_CosDictAppend(_DOC->Root,Extensions,obj = CosDictNew(Doc,false,1));
				_CosDictAppend(obj,ADBE,wrk = CosDictNew(Doc,false,2));
				_CosDictAppend(wrk,BaseVersion,CosNameNew(Doc, false,ULStringToAtom(_LIB,"1.7")));
				_CosDictAppend(wrk,ExtensionLevel, CosIntNew(Doc,false,3));
			}
            _DOC->Entries[EID.ID].Offset = ULStreamGetPosition ( Strm );
            ULitostr ( EID.ID, str );
            ULStrToStrm ( Strm, str );
            ULStrToStrm ( Strm, " 0 obj" );
            CosCopyObjToStream ( cr, Strm );
            ULStrToStrm ( Strm, "\nendobj\n" );
            _DOC->Entries[EID.ID].SaveInfo = setPacker;
        }
        xrefoffset = ULStreamGetPosition ( Strm );
        obj = CosStreamNew ( Doc, true, 7 );
        attr = _CosStreamAttr ( obj );
        trailID = _CosObjFullID ( obj );
        _DOC->Entries[trailID.ID].SaveInfo = setPacker;
        _DOC->Entries[trailID.ID].Offset = xrefoffset;
        _CosDictAppend ( attr, Size, CosIntNew ( Doc, false, _DOC->Size ) );
        _CosDictAppend ( attr, Root, _DOC->Root );
        _CosDictAppend ( attr, Info, _DOC->Info );
        _CosDictAppend ( attr, Type, _CosNameNew ( XRef ) );
        if ( crypting )
            _CosDictAppend ( attr, Encrypt, cr);
        _CosDictAppend ( attr, ID, ( ar = CosArrayNew ( Doc,false, 2 ) ) );
        if ( crypting )
            z = CosCopy ( Doc,  UsedCrypto->FileID );
        else
            z = ULCreateFileID ( Doc, PDFEngine );
        _CosStringHex ( z ) = true;
        CosArrayAppend ( ar, z );
        CosArrayAppend ( ar, CosCopy (Doc, z ) );
        maxgen = 1;
        maxoff = ULGetIntByte ( PackID.ID );
        err = 0;
        packstrm = _CosStreamValue ( obj );
        /*  Здесь мы вычисляем размер элементов W array  */
        for ( i = 0; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used == etFree ){
                _DOC->Entries[err].Index = err;
                maxoff = max ( maxoff, ULGetIntByte ( err ) );
                maxgen = max ( maxgen,
                               ULGetIntByte ( _DOC->Entries[i].Generation ) );
                err = i;
                continue;
            }
            if ( _DOC->Entries[i].SaveInfo == setPacked )
                maxgen = max ( maxgen,
                               ULGetIntByte ( _DOC->Entries[i].Index ) );
            else{
                maxgen = max ( maxgen,
                               ULGetIntByte ( _DOC->Entries[i].Generation ) );
                maxoff = max ( maxoff,
                               ULGetIntByte ( _DOC->Entries[i].Offset ) );
            }
        }
        ar = CosArrayNew ( Doc, false, 3 );
        PDFTRY ( _LIB ){
            CosArrayAppend ( ar, CosIntNew ( Doc, false, 1 ) );
            CosArrayAppend ( ar, CosIntNew ( Doc, false, maxoff ) );
            CosArrayAppend ( ar, CosIntNew ( Doc, false, maxgen ) );
            _CosDictAppend ( attr, W, ar );
        } PDFEXCEPT ( _LIB ){
            CosFree ( ar );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
        for ( i = 0; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used == etFree ){
                ULStreamWriteChar ( packstrm, 0 );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].Index, maxoff );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].Generation,
                                 maxgen );
                continue;
            }
            if ( _DOC->Entries[i].SaveInfo == setPacked ){
                ULStreamWriteChar ( packstrm, 2 );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].ObjID, maxoff );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].Index, maxgen );
            } else{
                ULStreamWriteChar ( packstrm, 1 );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].Offset,
                                 maxoff );
                ULStrmWriteInt ( packstrm,
                                 ( ( CosDoc ) Doc )->Entries[i].Generation,
                                 maxgen );
            }
        }
        CosStreamFlatePack ( _LIB, obj );
        _CosDictAppend ( attr, Length, CosIntNew ( Doc, false,
            ULStreamGetSize ( _CosStreamValue ( obj ) ) ) );
        ULitostr ( trailID.ID, str );
        ULStrToStrm ( Strm, str );
        ULStrToStrm ( Strm, " 0 obj" );
        CosCopyObjToStream ( obj, Strm );
        ULStrToStrm ( Strm, "\nendobj\n" );
        ULStrToStrm ( Strm, "startxref\r\n" );
        ULIntToStrm ( Strm, xrefoffset );
        ULStrToStrm ( Strm, "\r\n%%EOF" );
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].SaveInfo == setPacker ){
                if ( _DOC->Entries[i].Loaded )
                    CosFree ( _DOC->Entries[i].Obj );
                _DOC->Entries[i].Loaded = false;
                _DOC->Entries[i].Generation = 0xffff;
                _DOC->Entries[i].Used = etFree;
            }
        }
        PDFDocPackFree ( ( CosDoc ) Doc );
    } PDFEXCEPT ( _LIB ){
        if ( delID )
            CosFree ( ncr.FileID );
        CosFree ( cr );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    if ( delID )
        CosFree ( ncr.FileID );
}
#ifndef NOT_USE_SIGN

void TailPass(PDFDocHandle Doc, int start, PDFCosHandle tobj,
              TPDFCryptoType *crypto, PDFID EID, int *fsize, int *s1, int *o2,
              sha1_Context *ctx)
{
    PDFStreamHandle strm= NULL;
    PDFCosHandle obj    ;
    PDFID ID;
    ppBool crypting;
    char str[128], *pstr, *p, *q;
    ppUns32 i, l;

    _CosNullNew(Doc, obj);

    crypting = (ppBool)( crypto != NULL );
    PDFTRY ( _LIB ){
        strm = ULStreamMemNew ( _LIB, 0 );
        obj = CosGetFromDoc ( Doc, start);
        if ( crypting ){
            obj= CosCopy( Doc, obj );
            ID.ID = start;
            ID.GenID = _DOC->Entries[start].Generation;
            CosCryptObject ( obj, crypto, ID, true );
        }
        _DOC->Entries[start].Offset = *fsize;
        ULitostr ( start, str );
        ULStrToStrm ( strm, str );
        ULStrToStrm ( strm, " " );
        ULitostr ( _DOC->Entries[start].Generation, str );
        ULStrToStrm ( strm, str );
        if ( _CosGetType ( obj ) < CosName )
            ULStrToStrm ( strm, " obj " );
        else
            ULStrToStrm ( strm, " obj" );
        CosCopyObjToStream ( obj, strm );
        if ( crypting )
            CosFree ( obj );
        ULStrToStrm ( strm, "\nendobj\n" );
        p = ( char * ) ULStrmGetMem ( strm );
        q = strstr ( p, "/Contents" );
        q += strlen ( "/Contents" );
        ( *s1 ) = *fsize + ( int ) ( q - p );
        if ( ctx != NULL )
            sha1_Update ( ctx, (ppUns8 *) p, ( int ) ( q - p ) );
        q = strchr ( p, '>' );
        l = ULStreamGetSize ( strm );
        if ( ctx != NULL )
            sha1_Update ( ctx, (ppUns8 *) (q + 1), l - ( int ) ( q - p ) - 1 );
        ( *o2 ) = *fsize + ( int ) ( q - p ) + 1;
        ( *fsize ) += ULStreamGetSize ( strm );
        ULStreamClear ( strm, 0 );

        for ( i = start + 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used != etFree ){
                obj = CosGetFromDoc ( Doc, i );
                if ( crypting ){
                    obj = CosCopy ( Doc, obj );
                    ID.ID = i;
                    ID.GenID = _DOC->Entries[i].Generation;
                    if ( i != EID.ID )
                        CosCryptObject ( obj, crypto, ID, true );
                }
                _DOC->Entries[i].Offset = *fsize;
                ULitostr ( i, str );
                ULStrToStrm ( strm, str );
                ULStrToStrm ( strm, " " );
                ULitostr ( _DOC->Entries[i].Generation, str );
                ULStrToStrm ( strm, str );
                if ( _CosGetType ( obj ) < CosName )
                    ULStrToStrm ( strm, " obj " );
                else
                    ULStrToStrm ( strm, " obj" );
                CosCopyObjToStream ( obj, strm );
                if ( crypting )
                    CosFree ( obj );
                ULStrToStrm ( strm, "\nendobj\n" );
                ( *fsize ) += ULStreamGetSize ( strm );
                if ( ctx != NULL )
                    sha1_Update ( ctx, ( ppUns8 * ) ULStrmGetMem ( strm ),
                                  ULStreamGetSize ( strm ) );
                ULStreamClear ( strm, 0 );
            }
        }

        ULStrToStrm ( strm, "xref\r\n0 " );
        ULitostr ( _DOC->Size, str );
        ULStrToStrm ( strm, str );
        ULStrToStrm ( strm, "\r\n0000000000 65535 f\r\n" );
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used != etFree ){
                pstr = ULIntToStrWithZero ( _LIB, _DOC->Entries[i].Offset, 10 );
                ULStrToStrm ( strm, pstr );
                mfree ( _LIB, pstr );
                ULStreamWriteChar ( strm, ' ' );
            } else
                ULStrToStrm ( strm, "0000000000 " );

            if ( _DOC->Entries[i].Generation == 0xFFFF )
                pstr = ULIntToStrWithZero ( _LIB, 0, 5 );
            else
                pstr = ULIntToStrWithZero ( _LIB, _DOC->Entries[i].Generation, 5 );
            ULStrToStrm ( strm, pstr );
            mfree ( _LIB, pstr );
            if ( _DOC->Entries[i].Used != etFree )
                ULStrToStrm ( strm, " n\r\n" );
            else
                ULStrToStrm ( strm, " f\r\n" );
        }
        ULStrToStrm ( strm, "trailer\r\n" );
        CosCopyObjToStream (tobj, strm );
        ULStrToStrm ( strm, "\r\nstartxref\r\n" );
        ULIntToStrm ( strm, *fsize );
        ULStrToStrm ( strm, "\r\n%%EOF" );
        ( *fsize ) += ULStreamGetSize ( strm );
        if ( ctx != NULL )
            sha1_Update ( ctx, ( ppUns8 * ) ULStrmGetMem ( strm ), ULStreamGetSize ( strm ) );
        ULStreamClose ( strm );
    } PDFEXCEPT ( _LIB ){
        if ( strm )
            ULStreamClose ( strm );
        if ( crypting )
            CosFree ( obj );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
}

#endif

void PDFDocSaveUnPackedUnLinearized(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    char str[128];
    ppUns32 i, xrefoffset;
	ppUns32 k;
    PDFCosHandle obj,wrk, ar, z, cr;
    PDFCosHandle tobj;
    PDFID ID, EID;
    char *pstr;
    TPDFCryptoType ncr, *UsedCrypto = NULL;
    ppBool crypting = false, delID = false;
    char c;

#ifndef NOT_USE_SIGN 
    sha1_Context hctx;
    PDFStreamHandle ms  = NULL;
    int fs, s1, o2, s2, hlen = SHA1_HASHSIZE;
    ppUns8 hash[SHA1_HASHSIZE]; 
#endif
    _CosNullNew (Doc, cr);
    _CosNullNew (Doc, tobj);

#ifndef NOT_USE_SIGN
	if ( _DOC->Signed ){
		sig_AddSigDict ( Doc );
		sig_AddSigAnnot ( Doc );
		sig_UpdateAcroForm ( Doc );
		sig_UpdatePageAnnots ( Doc );
		ms = ULStreamMemNew ( _LIB, 0 );
	}
#endif

	if ( _DOC->Remove ){
		PDFDocRemoveUnUsed ( Doc, NULL );
		k = 1;
		for ( i = 1; i < _DOC->Size; i++ ){
			if ( ( ( CosDoc ) Doc )->Entries[i].Used != etFree )
				( ( CosDoc ) Doc )->Entries[i].Additional = k++;
			else 
				( ( CosDoc ) Doc )->Entries[i].Additional = 0x7FFFFFFF;
		} 
		PDFDocRenumAllObjects ( Doc );
		PDFDocPackFree ( ( CosDoc ) Doc );
	}
    if ( _DOC->UseOldSecurity ){
        if ( _DOC->Crypted ){ 
            crypting = true;
            UsedCrypto = &( _DOC->CryptoInfo );
        };
    } else{
        if ( _DOC->NewCrypted ){
            crypting = true;
			ULSetNewCryptoData( Doc, &ncr );
			UsedCrypto = &ncr;
            delID = true;
        };
    };
    PDFTRY ( _LIB ){
		if ( crypting ){
            cr = ULPrepareCryptoDictionary (Doc, UsedCrypto );
			EID= _CosObjFullID (cr);
        };
		if (((CosDoc)Doc)->Version >= 9){
			_CosDictAppend(_DOC->Root,Extensions,obj = CosDictNew(Doc,false,1));
			_CosDictAppend(obj,ADBE,wrk = CosDictNew(Doc,false,2));
			_CosDictAppend(wrk,BaseVersion,CosNameNew(Doc, false,ULStringToAtom(_LIB,"1.7")));
			_CosDictAppend(wrk,ExtensionLevel, CosIntNew(Doc,false,3));
		}
/*
#ifndef NOT_USE_SIGN
        if ( _DOC->Signed ){
            sig_AddSigDict ( Doc );
            sig_AddSigAnnot ( Doc );
            sig_UpdateAcroForm ( Doc );
            sig_UpdatePageAnnots ( Doc );
            ms = ULStreamMemNew ( _LIB, 0 );
        }
#endif*/
        _CosNullNew(Doc, z);
        _CosNullNew(Doc, ar);
        PDFTRY ( _LIB ){
            tobj = CosDictNew ( Doc, false, 4 );
            _CosDictAppend ( tobj, Size, CosIntNew ( Doc, false, _DOC->Size ) );
            _CosDictAppend ( tobj, Root, _DOC->Root );
            _CosDictAppend ( tobj, Info, _DOC->Info );
            if ( crypting )
                _CosDictAppend ( tobj, Encrypt, cr );
            _CosDictAppend ( tobj, ID, ( ar = CosArrayNew ( Doc, false, 2 ) ) );
            if ( crypting )
                z = CosCopy ( Doc, UsedCrypto->FileID );
            else
                z = ULCreateFileID ( Doc, PDFEngine );
            _CosStringHex(z) =  true;
            CosArrayAppend ( ar, z );
            CosArrayAppend ( ar, CosCopy (Doc, z ) );         
        } PDFEXCEPT ( _LIB ){
            CosFree ( tobj );
            CosFree ( z );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
#ifndef NOT_USE_SIGN
        if ( _DOC->Signed )
            sha1_Init ( &hctx );
#endif
        SULStrToStrm ( Strm, "%PDF-1." );
        c =(ppInt8)( _DOC->Version + '0');
        SULStrmWriteChar ( Strm, c );
        SULStrToStrm ( Strm, "\r\n" );
        SULStrToStrm ( Strm, "%\330\302\300\314\r\n" );
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used != etFree ){
#ifndef NOT_USE_SIGN 
                if ( _DOC->Signed && _CosObjID( _DOC->SigCtx->Sign) == i )
                    break;
#endif
                obj = CosGetFromDoc ( Doc, i );
                if ( crypting ){
                    obj = CosCopy( Doc, obj ); 
                    ID.ID = i;
                    ID.GenID = _DOC->Entries[i].Generation;
                    if ( i != EID.ID )
                        CosCryptObject ( obj, UsedCrypto, ID, true );
                };
                _DOC->Entries[i].Offset = ULStreamGetPosition ( Strm );
                ULitostr ( i, str );
                SULStrToStrm ( Strm, str );
                SULStrToStrm ( Strm, " " );
                ULitostr ( _DOC->Entries[i].Generation, str );
                SULStrToStrm ( Strm, str );
                if ( _CosGetType ( obj ) < CosName )
                    SULStrToStrm ( Strm, " obj " )
                else
                    SULStrToStrm ( Strm, " obj" )
#ifndef NOT_USE_SIGN 
                if ( _DOC->Signed ){
                    CosCopyObjToStream (obj, ms );
                    sha1_Update ( &hctx, ( ppUns8 *) ULStrmGetMem ( ms ),
                                  ULStreamGetSize ( ms ) );
                    ULStreamCopyToStream ( ms, Strm );
                    ULStreamClear ( ms, 0 );
                } else
#endif
                    CosCopyObjToStream (obj, Strm );
                if ( crypting )
                    CosFree ( obj );
                SULStrToStrm ( Strm, "\nendobj\n" );
            };
        };
#ifndef NOT_USE_SIGN

        if ( _DOC->Signed ){
            fs = ULStreamGetPosition ( Strm );
            TailPass ( Doc, i, tobj, UsedCrypto, EID, &fs, &s1, &o2, NULL );
            s2 = fs - o2; sig_UpdateByteRange ( Doc, s1, o2, s2 );
            fs = ULStreamGetPosition ( Strm );
            TailPass ( Doc, i, tobj, UsedCrypto, EID, &fs, &s1, &o2, &hctx );
            sha1_Finish ( &hctx, hash );
            sig_UpdateContents ( Doc, hash, hlen );

            for ( ; i < _DOC->Size; i++ ){
                if ( _DOC->Entries[i].Used != etFree ){
                    obj = CosGetFromDoc ( Doc, i );
                    if ( crypting ){
                        obj = CosCopy( Doc, obj );
                        ID.ID = i; ID.GenID = _DOC->Entries[i].Generation;
                        if ( i != EID.ID )
                            CosCryptObject ( obj, UsedCrypto, ID, true );
                    }
                    _DOC->Entries[i].Offset = ULStreamGetPosition ( Strm );
                    ULitostr ( i, str );
                    ULStrToStrm ( Strm, str );
                    ULStrToStrm ( Strm, " " );
                    ULitostr ( _DOC->Entries[i].Generation, str );
                    ULStrToStrm ( Strm, str );
                    if ( _CosGetType ( obj ) < CosName )
                        ULStrToStrm ( Strm, " obj " );
                    else
                        ULStrToStrm ( Strm, " obj" );
                    CosCopyObjToStream ( obj, Strm );
                    if ( crypting )
                        CosFree ( obj );
                    ULStrToStrm ( Strm, "\nendobj\n" );
                }
            }
        }

#endif        
        xrefoffset = ULStreamGetPosition ( Strm );
        ULStrToStrm ( Strm, "xref\r\n0 " );
        ULitostr ( _DOC->Size, str );
        ULStrToStrm ( Strm, str );
        ULStrToStrm ( Strm, "\r\n0000000000 65535 f\r\n" );
        for ( i = 1; i < _DOC->Size; i++ ){
            if ( _DOC->Entries[i].Used != etFree ){
                pstr = ULIntToStrWithZero ( _LIB, _DOC->Entries[i].Offset, 10 );
                ULStrToStrm ( Strm, pstr );
                mfree (  _LIB, pstr );
                ULStreamWriteChar ( Strm, ' ' );
            } else
                ULStrToStrm ( Strm, "0000000000 " );

            if ( _DOC->Entries[i].Generation == 0xFFFF )
                pstr = ULIntToStrWithZero ( _LIB, 0, 5 );
            else
                pstr = ULIntToStrWithZero ( _LIB, _DOC->Entries[i].Generation,
                                            5 );
            ULStrToStrm ( Strm, pstr );
            mfree ( _LIB, pstr );
            _DOC->Entries[i].Used != etFree ? ULStrToStrm ( Strm, " n\r\n" ) :
            ULStrToStrm ( Strm, " f\r\n" );
        }
        ULStrToStrm ( Strm, "trailer\r\n" );
        CosCopyObjToStream ( tobj, Strm );
        CosFree ( tobj );
        ULStrToStrm ( Strm, "\r\nstartxref\r\n" );
        ULIntToStrm ( Strm, xrefoffset );
        ULStrToStrm ( Strm, "\r\n%%EOF" );
        if ( crypting )
            PDFDocDeleteObjEx ( Doc, EID.ID);
#ifndef NOT_USE_SIGN
        if ( _DOC->Signed )
            ULStreamClose ( ms );
#endif
    } PDFEXCEPT ( _LIB ){
        CosFree ( ncr.FileID );
#ifndef NOT_USE_SIGN
        if ( ms )
            ULStreamClose ( ms );
#endif
        CosFree ( tobj );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    if ( delID )
        CosFree ( ncr.FileID );
}
