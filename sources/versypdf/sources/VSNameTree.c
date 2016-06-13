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
VSNameTree.c
*********************************************************************/

#include "VSMisc.h"
#include "VSCosAI.h"
#include "../VSBaseA.h"
#include "VSDocAI.h"
#include "VSAssert.h"
#include <string.h>


#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

PDFCosHandle ULGetDictFromNameTree(PDFDocHandle Doc, PDFCosHandle NameTree, char *String, ppUns32 Length)
{
    PDFCosHandle wrk, w, w1, w2, a, a1;
    ppUns32 i, k, j;

    if ( !Length )
        return CosNullNew( Doc );
    if ( !_IsCosDict ( NameTree ) )
        return CosNullNew( Doc );
    a = _CosDictValueByName ( NameTree, Names );
    if ( _IsCosArray ( a ) ){
        k = _CosArrayCount ( a ) >> 1;
        for ( i = 0; i < k; i++ ){
            w = _CosArrayItem ( a, i << 1 );
            if ( !_IsCosString ( w ) )
                continue;
            j = _CosStringSize ( w );
            if ( !memcmp ( String, _CosStringValue ( w ), min ( Length, j ) ) )
                return _CosArrayItem ( a, ( i << 1 ) + 1 );
        }
        return CosNullNew ( Doc );
    };
    a = _CosDictValueByName ( NameTree, Kids );
nextl:

    if ( _IsCosArray ( a ) ){
        for ( i = 0; i < _CosArrayCount ( a ); i++ ){
            w = CosArrayItem ( a, i );
            if ( !_CosIndirectDict ( w ) )
                continue;
            a1 = _CosDictValueByName ( w, Limits );
            if ( !_IsCosArray ( a1 ) )
                continue;
            if ( _CosArrayCount ( a1 ) < 2 )
                continue;
            w1 = CosArrayItem ( a1, 0 );
            if ( !_IsCosString ( w1 ) )
                continue;
            w2 = CosArrayItem ( a1, 1 );
            if ( !_IsCosString ( w2 ) )
                continue;
            if ( ( memcmp ( _CosStringValue ( w1 ), String, min ( Length, _CosStringSize ( w1 ) ) ) >
                0 ) ||
                ( memcmp ( _CosStringValue ( w2 ), String, min ( Length, _CosStringSize ( w2 ) ) ) <
                0 ) )
                continue;
            wrk = _CosDictValueByName ( w, Names );
            if ( _IsCosArray ( wrk ) ){
                k = _CosArrayCount ( wrk ) >> 1;
                for ( i = 0; i < k; i++ ){
                    w = CosArrayItem ( wrk, i << 1 );
                    if ( _CosGetType ( w ) != CosString )
                        continue;
                    j = _CosStringSize ( w );
                    if ( !memcmp ( String, _CosStringValue ( w ), min ( Length, j ) ) )
                        return CosArrayItem ( wrk, ( i << 1 ) + 1 );
                }
                return CosNullNew( Doc );
            };
            a = _CosDictValueByName ( a1, Kids );
            goto nextl;
        }
        return CosNullNew( Doc );
    };
    return CosNullNew( Doc );
}

void ULSetObjToNameTree(PDFDocHandle Doc, PDFCosHandle NameTree, PDFCosHandle InsObj, char *String,
                        ppUns32 Length)
{
    PDFCosHandle wrk, w, w1, w2, a, a1;
    ppUns32 i, k, j;
	ppInt32 z;
    if ( !Length )
        return;
    a = _CosDictValueByName ( NameTree, Names );
    if ( _IsCosArray ( a ) ){
        k = _CosArrayCount ( a ) >> 1;
        for ( i = 0; i < k; i++ ){
            w = CosArrayItem ( a, i << 1 );
            if ( _CosGetType ( w ) != CosString )
                continue;
            z = memcmp ( String, _CosStringValue ( w ), min ( Length, _CosStringSize ( w ) ) );
			if (!z) {
				if (Length < _CosStringSize ( w )) 
					z= -1;
				if (Length > _CosStringSize ( w )) 
					z= 1;
			}
            if ( !z ){
                a1 = CosArrayItemNRF ( a, ( i << 1 ) + 1 );
                if ( _CosGetType ( a1 ) != CosRef ){
                    CosArrayRemove ( a, ( i << 1 ) + 1 );
                    CosArrayInsert ( a, InsObj , ( i << 1 ) + 1 );
                } else
                    _CosRefValue ( a1 ) = _CosObjFullID ( InsObj );
                return;
            };
            if ( z > 0 ){
                CosArrayInsert ( a, CosStringNew ( Doc, false, String, Length ), i << 1 );
                CosArrayInsert ( a, InsObj , ( i << 1 ) + 1 );
                if ( !i ){
                    wrk = _CosDictValueByName ( NameTree, Limits );
                    if ( _IsCosArray ( wrk ) ){
                        if ( _CosArrayCount ( wrk ) < 2 )
                            return;
                        w1 = _CosArrayItem ( wrk, 0 );
                        if ( _IsCosString ( w1 ) )
                            CosStringSetValue (  w1, String, Length );
                    }
                } 
                return;
            }
        }
        CosArrayInsert ( a, CosStringNew ( Doc, false, String, Length ), i << 1 );
        CosArrayInsert ( a, InsObj, ( i << 1 ) + 1 );
        wrk = _CosDictValueByName ( NameTree, Limits );
        if ( _IsCosArray ( wrk ) ){
            if ( _CosArrayCount ( wrk ) < 2 )
                return;
            w1 = _CosArrayItem ( wrk, 1 );
            if ( _IsCosString ( w1 ) )
                CosStringSetValue ( w1, String, Length );
        }
        return;
    };
    a = _CosDictValueByName ( NameTree, Kids );

nextl:
    if ( _IsCosArray ( a ) ){
        for ( i = 0; i < _CosArrayCount ( a ); i++ ){
            w = CosArrayItem ( a, i );
            if ( !_CosIndirectDict ( w ) )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            a1 = _CosDictValueByName ( w, Limits );
            if ( !_IsCosArray ( a1 ) )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            if ( _CosArrayCount ( a1 ) < 2 )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            w1 = CosArrayItem (  a1, 0 );
            if ( !_IsCosString ( w1 ) )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            w2 = CosArrayItem ( a1, 1 );
            if ( !_IsCosString ( w2 ) )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            z = memcmp ( String, _CosStringValue ( w1 ), min ( Length, _CosStringSize ( w1 ) ) );
            if ( z < 0 )
                CosStringSetValue ( w1, String, Length );
            j = memcmp ( String, _CosStringValue ( w2 ), min ( Length, _CosStringSize ( w2 ) ) );
            if ( j > 0 ){
                if ( i == _CosArrayCount ( a ) - 1 )
                    CosStringSetValue ( w2, String, Length );
                else
                    continue;
            }
            wrk = _CosDictValueByName ( w, Names );
            if ( _IsCosArray ( wrk ) ){
                k = _CosArrayCount ( wrk ) >> 1;
                for ( i = 0; i < k; i++ ){
                    w = CosArrayItem ( wrk, i << 1 );
                    if ( _CosGetType ( w ) != CosString )
                        continue;
                    z = memcmp ( String, _CosStringValue ( w ),
                        min ( Length, _CosStringSize ( w ) ) );
                    if ( !z ){
                        a1 = _CosArrayItem ( wrk, ( i << 1 ) + 1 );
                        if ( !_IsCosRef ( a1 ) ){
                            CosArrayRemove ( wrk, ( i << 1 ) + 1 );
                            CosArrayInsert ( wrk, InsObj, ( i << 1 ) + 1 );
                        } else
                            _CosRefValue ( a1 ) = _CosObjFullID ( InsObj );
                        return;
                    };
                    if ( z > 0 ){
                        CosArrayInsert (  wrk, CosStringNew ( Doc, false, String, Length ), i << 1 );
                        CosArrayInsert (  wrk, InsObj, ( i << 1 ) + 1 );
                        return;
                    }
                }
                CosArrayInsert ( wrk, CosStringNew ( Doc, false, String, Length ), i << 1 );
                CosArrayInsert ( wrk, InsObj, ( i << 1 ) + 1 );
                return;
            };
            a = _CosDictValueByName ( a1, Kids );
            if ( !_IsCosArray ( a ) )
                _RAISE ( ErrDocumentLevel, dleCannotInsertNameToNameTreeError );
            goto nextl;
        }
        return;
    };
    wrk = CosArrayNew ( Doc, false, 8 );
    _CosDictAppend ( NameTree, Names, wrk );
    CosArrayAppend ( wrk, CosStringNew (Doc, false, String, Length ) );
    CosArrayAppend ( wrk, InsObj );
    wrk = CosArrayNew ( Doc, false, 2 );
    _CosDictAppend ( NameTree, Limits, wrk );
    CosArrayAppend ( wrk, CosStringNew ( Doc, false, String, Length) );
    CosArrayAppend ( wrk, CosStringNew ( Doc, false, String, Length ) );
}
