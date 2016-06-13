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
VSCosA.c
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






#define _DDOC CD(Doc)
#define _LLIB _DDOC->Lib

#ifdef _LIB
#undef _LIB
#endif

#define _DOC _CosDoc(CosObject)
#define _LIB CD(_DOC)->Lib

#define SetID0(id) {id.ID=0;id.GenID=0;}

PDFCosHandle CosNullNew( PDFDocHandle Doc )
{
    return _CosNull( Doc );
}


/*  Int Object  */ 
 
PDFCosHandle CosIntNew( PDFDocHandle Doc, ppBool IsIndirect, ppInt32 Value )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosInteger;
    obj->Data.iv.Value = Value;
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp,obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

ppInt32 CosIntGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosInt( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return  _CosIntValue ( CosObject );
}

void CosIntSetValue( PDFCosHandle CosObject, ppInt32 Value )
{
    if ( !_IsCosInt ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    _CosIntValue ( CosObject ) = Value;
}

/* WorkInt objects*/
PDFCosHandle CosNewIntWork( PDFDocHandle Doc, ppInt32 Value )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = (CosType)CosIntWork;
    obj->Doc = Doc;
    obj->Data.iv.Value = Value;
    obj->ObjInfo.Parent = NULL;
    obj->IsIndirect = false;
    SetCO(tmp,obj);
    return tmp;
}

ppInt32 CosGetIntWorkValue( PDFCosHandle CosObject )
{
    if ( !_IsCosIntWork ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosIntWorkValue ( CosObject );
}

/*  Real Object  */

PDFCosHandle CosRealNew( PDFDocHandle Doc, ppBool IsIndirect, ppReal Value )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosReal;
    obj->Data.rv.Value = Value;
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp,obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

ppReal CosRealGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosReal ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosRealValue ( CosObject );
}

void CosRealSetValue( PDFCosHandle CosObject, ppReal Value )
{
    if ( !_IsCosReal ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    _CosRealValue ( CosObject ) = Value;
}

/*  Boolean Object  */


PDFCosHandle CosBoolNew( PDFDocHandle Doc, ppBool IsIndirect, ppBool Value )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosBoolean;
    obj->Data.bv.Value = Value;
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp,obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}


ppBool CosBoolGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosBool ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosBoolValue ( CosObject );
}

void CosBoolSetValue( PDFCosHandle CosObject, ppBool Value )
{
    if ( !_IsCosBool ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    _CosBoolValue ( CosObject ) = Value;
}

/*  Name  */

PDFCosHandle CosNameNew( PDFDocHandle Doc, ppBool IsIndirect, ppAtom Value )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosName;
    obj->Data.nv.Value = Value;
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp,obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

ppAtom CosNameGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosName ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosNameValue ( CosObject );
}

void CosNameSetValue( PDFCosHandle CosObject, ppAtom Value )
{
    if ( !_IsCosName ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    _CosNameValue ( CosObject ) = Value;
}

/*  EOF Cos Object */

PDFCosHandle CosEOFNew( PDFDocHandle Doc)
{
	CosObj obj;
	PDFCosHandle tmp;
	obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
	obj->Type = (CosType)CosEOF;
	obj->Doc = Doc;
	SetCO(tmp,obj);
	return tmp;
}

/*  Inderect Link */

PDFCosHandle CosNewRef( PDFDocHandle Doc, ppInt32 ID, ppUns16 GenID )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = (CosType)CosRef;
    obj->Data.rfv.Value.ID = ID;
    obj->Data.rfv.Value.GenID = GenID;
    obj->ObjInfo.Parent = NULL;
    obj->IsIndirect = false;
    obj->Doc = Doc;
    SetCO(tmp,obj);
    return tmp;
}

/*  Work object  */
PDFCosHandle CosNewWork( PDFDocHandle Doc, char *String )
{
    size_t sz;
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = (CosType)CosWork;
    sz = strlen ( String ) + 1;
    PDFTRY ( _LLIB ){
        obj->Data.sv.Buffer = ( char * ) mmalloc ( _LLIB, sz );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    memcpy ( obj->Data.sv.Buffer, String, sz );
    obj->ObjInfo.Parent = NULL;
    obj->IsIndirect = false;
    obj->Doc = Doc;
    SetCO(tmp, obj);
    return tmp;
}

char * CosGetWorkCommand( PDFCosHandle CosObject )
{
    if ( ! _IsCosWork ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosWorkCommand ( CosObject );
}

/*  String  */

PDFCosHandle CosStringNew( PDFDocHandle Doc, ppBool IsIndirect, char *String, ppUns32 Length)
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosString;
    PDFTRY ( _LLIB ){
        obj->Data.sv.Buffer = ( char * ) mmalloc ( _LLIB, Length );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    memcpy ( obj->Data.sv.Buffer, String, Length );
    obj->Data.sv.IsHex = false;
    obj->Data.sv.Size = Length;
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp,obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

char * CosStringGetValue( PDFCosHandle CosObject, ppUns32 *Length )
{
    if ( !_IsCosString ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    *Length = _CosStringSize ( CosObject );
    return _CosStringValue ( CosObject );
}

void CosStringSetValue( PDFCosHandle CosObject, char *String, ppUns32 Length )
{
    char *t;
    if ( ! _IsCosString ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    t = ( char * ) mmalloc ( _LIB, Length );
    memcpy ( t, String, Length );
    mfree ( _LIB, _CosStringValue( CosObject ) );
    _CosStringValue ( CosObject ) = t;
    _CosStringSize ( CosObject ) = Length;
}

ppBool CosGetStringHex( PDFCosHandle CosObject )
{
    if ( ! _IsCosString ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosStringHex ( CosObject );
}

void CosSetStringHex( PDFCosHandle CosObject, ppBool IsHex )
{
    if ( ! _IsCosString ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    _CosStringHex ( CosObject ) = IsHex;
}

ppInt32 CosGetStringSize( PDFCosHandle CosObject )
{
    if ( ! _IsCosString ( CosObject)  )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosStringSize ( CosObject );
}

/*  Array  */

PDFCosHandle CosArrayNew( PDFDocHandle Doc, ppBool IsIndirect, ppUns32 Entries )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosArray;
    obj->Data.av.Capacity = Entries;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    obj->Data.av.Count = 0;
    obj->Data.av.MinStrLen = 0;
    PDFTRY ( _LLIB ){
        obj->Data.av.Array = ( PDFCosHandle * ) mmalloc ( _LLIB, sizeof ( PDFCosHandle ) * Entries );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO(tmp, obj);
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            chfreez ( _LLIB, obj->Data.av.Array );
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

ppUns32 CosArrayCount( PDFCosHandle CosObject )
{
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosArrayCount ( CosObject );
}

ppInt32 CosArrayInsert( PDFCosHandle CosObject, PDFCosHandle NewCosObject, ppUns32 Position )
{
    if ( !_IsCosArray( CosObject )  )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosDoc ( NewCosObject ) != _DOC )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectFromOtherDocumentError );
    if ( !_CosIsIndirect ( NewCosObject) && _CosParent ( NewCosObject ) )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectWithParrentError );      
    if ( Position >= _CosArrayCount ( CosObject ) )
        return CosArrayAppend ( CosObject, NewCosObject );
    if ( _CosArrayCount ( CosObject ) == _CosArrayCapacity ( CosObject ) ){
            _CosArrayCapacity ( CosObject ) += CalculateDelta ( _CosArrayCapacity ( CosObject ) );
            _CosArrayArray ( CosObject ) = ( PDFCosHandle * )
                mrealloc ( _LIB, _CosArrayArray ( CosObject ),
                _CosArrayCapacity ( CosObject ) * sizeof ( PDFCosHandle ) );
        };
    memmove ( &( _CosArrayItem ( CosObject, Position + 1 ) ),
        &( _CosArrayItem ( CosObject, Position )  ),
        ( _CosArrayCount ( CosObject ) - Position ) * sizeof ( PDFCosHandle ) );
    if ( _CosIsIndirect ( NewCosObject ) )
        NewCosObject = CosNewRef ( _CosDoc( NewCosObject ), _CosObjID ( NewCosObject), 
            _CosObjGeneration ( NewCosObject ) );
    _CosArrayItem ( CosObject, Position ) = NewCosObject;
    _CosArrayCount ( CosObject )++;
    if ( !_IsCosNull ( NewCosObject ) )
        _CosParent ( NewCosObject ) = CO ( CosObject );
    return Position;
}

ppInt32 CosArrayAppend( PDFCosHandle CosObject, PDFCosHandle NewCosObject )
{
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosDoc ( NewCosObject ) != _DOC )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectFromOtherDocumentError );
    if ( !_CosIsIndirect ( NewCosObject) && _CosParent ( NewCosObject ) )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectWithParrentError );
    if ( _CosArrayCount ( CosObject ) == _CosArrayCapacity ( CosObject ) ){
        _CosArrayCapacity ( CosObject ) += CalculateDelta ( _CosArrayCapacity ( CosObject ) );
        _CosArrayArray ( CosObject ) = ( PDFCosHandle * )
            mrealloc ( _LIB, _CosArrayArray ( CosObject ),
            _CosArrayCapacity ( CosObject ) * sizeof ( PDFCosHandle ) );
    };
    if ( _CosIsIndirect ( NewCosObject ) )
        NewCosObject = CosNewRef ( _CosDoc( NewCosObject ), _CosObjID ( NewCosObject), 
            _CosObjGeneration ( NewCosObject ) );
    _CosArrayItem ( CosObject, _CosArrayCount (CosObject ) ) = NewCosObject;
    if ( !_IsCosNull ( NewCosObject ) )
        _CosParent ( NewCosObject ) = CO ( CosObject );
    return _CosArrayCount ( CosObject )++;
}

void CosArrayRemove( PDFCosHandle CosObject, ppUns32 Index )
{
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( Index >= _CosArrayCount (CosObject ) )
        return;
    CosFree ( _CosArrayItem( CosObject, Index ) );
    memmove ( &( _CosArrayItem( CosObject, Index ) ),
        &( _CosArrayItem( CosObject, Index + 1 ) ),
        ( _CosArrayCount (CosObject ) - Index - 1 ) * sizeof ( PDFCosHandle ) );
    _CosArrayCount (CosObject )--;
}

PDFCosHandle CosArrayItem( PDFCosHandle CosObject, ppUns32 Index )
{
    PDFCosHandle obj;
    PDFID ID;
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( Index >= _CosArrayCount ( CosObject ) )
        return CosNullNew ( _DOC );
    obj = _CosArrayItem( CosObject, Index );
    if ( !_IsCosRef ( obj ) )
        return obj;
    ID = _CosRefValue (obj );
    return  CosGetFromDoc ( _DOC, ID.ID);
}

PDFCosHandle CosArrayItemNRF( PDFCosHandle CosObject, ppUns32 Index )
{
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( Index >= _CosArrayCount ( CosObject ) )
        return CosNullNew ( _DOC );
    return _CosArrayItem( CosObject, Index );
}

void CosArrayClear( PDFCosHandle CosObject )
{
    ppUns32 i;
    if ( !_IsCosArray ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    for ( i = 0; i < _CosArrayCount ( CosObject ) ; i++ )
        CosFree ( _CosArrayItem( CosObject, i ) );
    mfree ( _LIB, _CosArrayArray ( CosObject ) );
    _CosArrayCapacity ( CosObject ) = 0;
    _CosArrayArray ( CosObject ) = NULL;
    _CosArrayCount ( CosObject ) = 0;
}

/*  Dictionary  */

PDFCosHandle CosDictNew( PDFDocHandle Doc, ppBool IsIndirect, ppUns32 Entries )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosDict;
    obj->Data.dv.Capacity = Entries;
    obj->Data.dv.Count = 0;
    PDFTRY ( _LLIB ){
        obj->Data.dv.Array = ( DictElem * ) mmalloc ( _LLIB, sizeof ( DictElem ) * Entries );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    obj->ObjInfo.Parent = NULL;
    obj->Doc = Doc;
    obj->IsIndirect = IsIndirect;
    SetCO ( tmp, obj );
    if ( IsIndirect ) {
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            chfreez ( _LLIB, obj->Data.dv.Array );
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}

ppInt32 CosDictCount( PDFCosHandle CosObject )
{
    if ( !_IsCosDictOrStream ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosGetType ( CosObject ) == CosStream )
        CosObject = _CosStreamAttr ( CosObject );
    return _CosDictCount ( CosObject );
}


void CosDictGetPair(PDFCosHandle CosObject, ppUns32 Index, ppAtom *Key, PDFCosHandle *Value){
    if ( !_IsCosDictOrStream ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr ( CosObject );
    if ( Index >= _CosDictCount ( CosObject ) )
        _RAISE ( ErrCosLevel, cleArrayBoundsError);
    *Key =  _CosDictKey ( CosObject, Index );
    *Value = _CosDictValue ( CosObject, Index );
}

void CosDictAppend(PDFCosHandle CosObject, ppAtom Key, PDFCosHandle KeyValue )
{
    ppUns32 i;
    if ( !_IsCosDictOrStream (CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _CosDoc ( KeyValue ) != _DOC )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectFromOtherDocumentError );
    if ( !_CosIsIndirect ( KeyValue) && _CosParent ( KeyValue ) )
        _RAISE ( ErrCosLevel, cleCannotInsertObjectWithParrentError );
    if ( _IsCosNull ( KeyValue ) ){
        CosDictRemoveKey ( CosObject, Key );
        return;
    }
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr (CosObject );

    /* Check on exists key */
    for ( i = 0; i < _CosDictCount ( CosObject ); i++ )
        if ( _CosDictKey ( CosObject, i) == Key ){
            CosFree( _CosDictValue ( CosObject, i) );
            if ( _CosIsIndirect ( KeyValue ) )
                KeyValue = CosNewRef ( _CosDoc( KeyValue ), _CosObjID ( KeyValue), 
                _CosObjGeneration ( KeyValue ) );
            _CosDictValue ( CosObject, i) = KeyValue;
            _CosParent ( KeyValue ) = CO ( CosObject );
            return;
        };

    if ( _CosDictCount ( CosObject ) == _CosDictCapacity (CosObject) ){
        _CosDictCapacity ( CosObject ) += CalculateDelta ( _CosDictCapacity ( CosObject) );
        _CosDictArray ( CosObject ) = ( DictElem * ) mrealloc ( _LIB, _CosDictArray ( CosObject ),
            _CosDictCapacity ( CosObject) * sizeof ( DictElem ) );
    }     
    i = _CosDictCount ( CosObject );
    _CosDictKey ( CosObject, i) = Key;
    if ( _CosIsIndirect ( KeyValue ) )
        KeyValue = CosNewRef ( _CosDoc( KeyValue ), _CosObjID ( KeyValue), 
            _CosObjGeneration ( KeyValue ) );
    _CosDictValue ( CosObject, i) = KeyValue;
    _CosParent ( KeyValue ) = CO ( CosObject );
    _CosDictCount ( CosObject )++;
}

void CosDictRemoveKey(PDFCosHandle CosObject, ppAtom Key )
{
    ppUns32 i;
    if ( !_IsCosDictOrStream (CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr (CosObject );
    for ( i = 0; i < _CosDictCount ( CosObject ); i++ )
        if (  _CosDictKey ( CosObject, i) == Key ){
            CosFree ( _CosDictValue ( CosObject, i) );
            memmove ( &( _CosDictPair ( CosObject, i ) ),
                &( _CosDictPair ( CosObject, i + 1 ) ),
                ( _CosDictCount ( CosObject ) - i - 1 ) * sizeof ( DictElem ) );
            _CosDictCount ( CosObject )--;
            return ;
        };
}

PDFCosHandle CosDictValueByName( PDFCosHandle CosObject, ppAtom Key )
{
    ppUns32 i;
    PDFCosHandle obj;
    PDFID ID;
    if ( !_IsCosDictOrStream (CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr (CosObject );
    for ( i = 0; i < _CosDictCount ( CosObject ); i++ ){
        if ( _CosDictKey ( CosObject, i) == Key ){
            obj = _CosDictValue ( CosObject, i);
            if ( !_IsCosRef ( obj ) )
                return obj;
            ID = _CosRefValue( obj );
            return CosGetFromDoc ( _DOC, ID.ID);
        }
    }
    return CosNullNew ( _DOC );
}

PDFCosHandle CosDictValueByNameNRF( PDFCosHandle CosObject, ppAtom Key )
{
    ppUns32 i;
    if ( !_IsCosDictOrStream (CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr (CosObject );
    for ( i = 0; i < _CosDictCount ( CosObject ); i++ )
        if ( _CosDictKey ( CosObject, i) == Key )
            return _CosDictValue ( CosObject, i);
    return CosNullNew ( _DOC );
}

void CosDictClear( PDFCosHandle CosObject )
{
    ppUns32 i;
    if ( !_IsCosDictOrStream (CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosStream ( CosObject ) )
        CosObject = _CosStreamAttr (CosObject );
    for ( i = 0; i < _CosDictCount ( CosObject ); i++ )
        CosFree ( _CosDictValue ( CosObject, i) );
    _CosDictCount ( CosObject ) = 0;
    _CosDictCapacity ( CosObject ) = 0;
    chfreez ( _LIB, _CosDictArray ( CosObject ) );
    _CosDictArray ( CosObject ) = NULL;
}

/*  Stream  */

PDFCosHandle CosStreamNew( PDFDocHandle Doc, ppBool IsIndirect, ppInt32 Entries )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosStream;
    obj->Doc = Doc;
    obj->Data.smv.Filtered = false;
    PDFTRY ( _LLIB ){
        obj->Data.smv.Attr = CosDictNew ( Doc, false, Entries );
        PDFTRY ( _LLIB ){
            obj->Data.smv.Store = ULStreamMemNew ( _LLIB, 0 );
        } PDFEXCEPT ( _LLIB ){
            CosFree ( obj->Data.smv.Attr );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    obj->ObjInfo.Parent = NULL;
    obj->IsIndirect = IsIndirect;
    SetCO ( tmp, obj );
    if  (IsIndirect){
        PDFTRY ( _LLIB ){
            CosAddToDoc ( Doc, tmp );
        } PDFEXCEPT ( _LLIB ) {
            CosFree ( obj->Data.smv.Attr );
            ULStreamClose ( obj->Data.smv.Store );
            mfree ( _LLIB, obj );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    }
    return tmp;
}


PDFCosHandle CosNewIntStream( PDFDocHandle Doc, ppInt32 Entries )
{
    CosObj obj;
    PDFCosHandle tmp;
    obj = ( CosObj ) mmalloc ( _LLIB, sizeof ( TCosObj ) );
    obj->Type = CosStream;
    obj->Doc = Doc;
    obj->Data.smv.Filtered = false;
    PDFTRY ( _LLIB ){
        obj->Data.smv.Attr = CosDictNew ( Doc, false, Entries );
        PDFTRY ( _LLIB ){
            obj->Data.smv.Store = ULStreamMemNew ( _LLIB, 0 );
        } PDFEXCEPT ( _LLIB ){
            CosFree ( obj->Data.smv.Attr );
            PDFRERAISE ( _LLIB );
        }
        PDFTRYEND ( _LLIB );
    } PDFEXCEPT ( _LLIB ){
        mfree ( _LLIB, obj );
        PDFRERAISE ( _LLIB );
    }
    PDFTRYEND ( _LLIB );
    obj->ObjInfo.Parent = NULL;
    obj->IsIndirect = false;
    SetCO ( tmp, obj );
    return tmp;
}


PDFCosHandle CosStreamGetAttr( PDFCosHandle CosObject )
{
    if ( !_IsCosStream ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosStreamAttr ( CosObject );
}

PDFStreamHandle CosStreamGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosStream ( CosObject ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosStreamValue( CosObject );
}


/*   Other Functions for work with CosObject */
CosType CosGetType( PDFCosHandle CosObject )
{
    return (CosType)_CosGetType( CosObject );
}

void CosFree( PDFCosHandle CosObject )
{
    PDFDocHandle Doc;
    Doc=_CosDoc ( CosObject );
    switch ( _CosGetType( CosObject ) ){
        case CosString:
        case CosWork:
            mfree ( _LIB, _CosStringValue( CosObject ) );
            break;
        case CosDict:
            CosDictClear ( CosObject );   
            break;
        case CosArray:
            CosArrayClear ( CosObject );
            break;
        case CosStream:
            CosFree (  _CosStreamAttr ( CosObject ) );
            ULStreamClose ( _CosStreamValue( CosObject ) );
            break;
        case CosNull:
            return;
        default:
            ;
    }
    mfree ( _LIB, CO ( CosObject ) );
    _CosNullNew ( Doc, CosObject );    
}


ppReal CosNumberGetValue( PDFCosHandle CosObject )
{
    if ( !_IsCosNumber(CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( _IsCosInt ( CosObject ) )
        return ULIntToReal ( _CosIntValue( CosObject ) );
    else
        return _CosRealValue ( CosObject );
}

PDFCosHandle CosCopy( PDFDocHandle Doc, PDFCosHandle CosObject )
{
    PDFCosHandle tmp;
    ppUns32 i;
    switch ( _CosGetType ( CosObject ) ){
        case CosNull:
            return _CosNull ( Doc );
        case CosInteger:
            return CosIntNew ( Doc, false, _CosIntValue ( CosObject ) );
        case CosReal:
            return CosRealNew ( Doc, false, _CosRealValue ( CosObject ) );
        case CosRef:
            return CosNewRef ( Doc, _CosRefValue ( CosObject).ID, 
                _CosRefValue ( CosObject).GenID );
        case CosBoolean:
            return CosBoolNew (Doc, false, _CosBoolValue ( CosObject ) );
        case CosName:
            return CosNameNew ( Doc, false, _CosNameValue ( CosObject ) );
        case CosString:
			tmp = CosStringNew ( Doc, false, _CosStringValue ( CosObject ), 
				_CosStringSize ( CosObject) );
			_CosStringHex(tmp) = _CosStringHex (CosObject);
            return tmp;
        case CosWork:
            return CosNewWork ( Doc, _CosWorkCommand ( CosObject ) );
        case CosArray:
            tmp = CosArrayNew ( Doc, false, _CosArrayCount ( CosObject ) );
            PDFTRY ( _LIB ){
                _CosArrayMinLen ( tmp ) = _CosArrayMinLen ( CosObject );
                for ( i = 0; i < _CosArrayCount ( CosObject ); i++ )
                    CosArrayAppend ( tmp, CosCopy ( Doc, _CosArrayItem ( CosObject, i ) ) );
            } PDFEXCEPT ( _LIB ){
                CosFree ( tmp );
                PDFRERAISE ( _LIB );
            }
            PDFTRYEND ( _LIB );
            return tmp;
        case CosDict:
            tmp = CosDictNew ( Doc, false, _CosDictCount ( CosObject ) );
            PDFTRY ( _LIB ){
                for ( i = 0; i < _CosDictCount ( CosObject ); i++ )
                    CosDictAppend ( tmp, _CosDictKey ( CosObject, i ),
                        CosCopy ( Doc, _CosDictValue ( CosObject, i) ) );
            } PDFEXCEPT ( _LIB ){
                CosFree ( tmp );
                PDFRERAISE ( _LIB );
            }
            PDFTRYEND ( _LIB );
            return tmp;
        case CosStream:
            tmp = CosNewIntStream ( Doc, _CosDictCount ( _CosStreamAttr ( CosObject ) ) ) ;
            PDFTRY ( _LIB ){
                for ( i = 0; i < _CosDictCount ( _CosStreamAttr ( CosObject ) ); i++)
                    CosDictAppend ( tmp, _CosDictKey ( _CosStreamAttr ( CosObject ), i ),
                        CosCopy ( Doc, _CosDictValue ( _CosStreamAttr ( CosObject ), i ) ) );
                ULStreamCopyToStream ( _CosStreamValue ( CosObject ), _CosStreamValue ( tmp ) );
				CO ( tmp )->Data.smv.Filtered = CO ( CosObject )->Data.smv.Filtered;
            } PDFEXCEPT ( _LIB ){
                CosFree ( tmp );
                PDFRERAISE ( _LIB );
            }
            PDFTRYEND ( _LIB );
            return tmp;
    }
    return _CosNull ( _DOC );
}

PDFCosHandle CosDictToStream( PDFCosHandle CosObject )
{
    CosObj obj;
    PDFCosHandle tmp;
    if ( !_IsCosDict ( CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    obj = ( CosObj ) mmalloc ( _LIB, sizeof ( TCosObj ) );
    obj->Type = CosStream;
	obj->Doc = _DOC;
    obj->Data.smv.Filtered = false;
    obj->Data.smv.Attr = CosObject;
    obj->IsIndirect = false;
    obj->ObjInfo.Parent = NULL;
    SetCO(tmp,obj);
    PDFTRY ( _LIB ){
        obj->Data.smv.Store = ULStreamMemNew ( _LIB, 0 );
    } PDFEXCEPT ( _LIB ){
        mfree ( _LIB, obj );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    return tmp;
}

void CosStreamCheckFilters( PDFCosHandle CosObject )
{
    PDFCosHandle attr, fl;
    if ( !_IsCosStream ( CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    attr = _CosStreamAttr( CosObject );
    fl = CosDictValueByName ( attr, GetAtomDoc ( Filter ) );
    if ( _IsCosNull (fl) )
        fl = CosDictValueByName ( attr, GetAtomDoc ( F ) );
    if (  _IsCosArray ( fl ) ){
        if (_CosArrayCount ( fl ) ) _CosStreamIsFiltered(CosObject) =  true;
        else _CosStreamIsFiltered(CosObject) = false;
        return;
    }
    if ( _CosGetType ( fl ) == CosName  ){
            _CosStreamIsFiltered(CosObject) = true;
            return;
    }
    _CosStreamIsFiltered(CosObject) = false;
}

ppBool CosStreamIsFiltered( PDFCosHandle CosObject )
{
    if ( !_IsCosStream ( CosObject) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    return _CosStreamIsFiltered (CosObject );
}


ppUns32 CosGetID( PDFCosHandle CosObject )
{
    if ( !_CosIsIndirect( CosObject ) )
        _RAISE ( ErrCosLevel, cleDirectObjectWasFoundError );
    return _CosObjID( CosObject );
}

ppUns16 CosGetGeneration( PDFCosHandle CosObject )
{
    if ( !_CosIsIndirect( CosObject ) )
        _RAISE ( ErrCosLevel, cleDirectObjectWasFoundError );
    return _CosObjGeneration( CosObject );
}


ppUns32 CosGetObjSize( PDFCosHandle CosObject )
{
    ppUns32 i, k, t;
    switch ( _CosGetType ( CosObject ) ){
        case CosNull:
            return 4;
        case CosInteger:
            return ULGetIntSize ( _CosIntValue(  CosObject ) );
        case CosReal:
            return ULGetRealSize ( _CosRealValue ( CosObject ) );
        case CosRef:
            return ULGetIntSize ( _CosRefValue ( CosObject ).ID ) +
                ULGetIntSize ( _CosRefValue ( CosObject ).GenID ) +
                3;
        case CosBoolean:
            return  _CosBoolValue( CosObject ) ? 4 : 5;
        case CosName:
            return ULGetAtomSize ( _LIB, _CosNameValue ( CosObject ) ) + 1;
        case CosString:
            if ( _CosStringHex( CosObject ) )
                return ( _CosStringSize( CosObject ) + 1 ) << 1;
            else
                return ULGetStringSize ( _CosStringValue (  CosObject ), 
                    _CosStringSize( CosObject ) ) + 2;
        case CosArray:
            k = 3;
            for ( i = 0; i < _CosArrayCount ( CosObject ); i++ ){
                if ( i &&   _CosGetType ( _CosArrayItem ( CosObject, i ) ) < CosName )
                    k++;
                k += CosGetObjSize ( _CosArrayItem ( CosObject, i ) );
            }
            if ( _CosArrayMinLen ( CosObject ) > k )
                k = _CosArrayMinLen ( CosObject );
            return k;
        case CosDict:
            k = 4;
            for ( i = 0; i < _CosDictCount ( CosObject ); i++ ){
                t = 1 +
                    ULGetAtomSize ( _LIB, _CosDictKey ( CosObject, i ) ) +
                    CosGetObjSize ( _CosDictValue ( CosObject, i ) );
                if ( _CosGetType ( _CosDictValue ( CosObject, i ) ) < CosName )
                    t++;
				k += t;
            };
            return k;
        case CosStream:
            return CosGetObjSize ( _CosStreamAttr ( CosObject ) ) +
                ULStreamGetSize ( _CosStreamValue( CosObject ) ) +
                18;
        default:

            ;
    };
    return 0;
}

void CosCopyObjToStream( PDFCosHandle CosObject, PDFStreamHandle Strm )
{
    char *hexchar   = "0123456789abcdef";
    ppUns32 i, k;
    char *ops;
    char hex[3];
    switch ( _CosGetType( CosObject ) ){
        case CosNull:
            ULStrToStrm ( Strm, "null" );
            return ;            
        case CosInteger:
            ULIntToStrm ( Strm, _CosIntValue ( CosObject ) );
            return ;
        case CosReal:
            ULRealToStrm ( Strm, _CosRealValue ( CosObject ) );
            return ;
        case CosRef:
            ULIntToStrm ( Strm, _CosRefValue( CosObject ).ID );
            ULStreamWriteChar ( Strm, ' ' );
            ULIntToStrm ( Strm, _CosRefValue( CosObject ).GenID );
            ULStrToStrm ( Strm, " R" );
            return;
        case CosBoolean:
            _CosBoolValue ( CosObject ) ? ULStrToStrm ( Strm, "true" ) :
                ULStrToStrm ( Strm, "false" );
            return;
        case CosName:
            ULStreamWriteChar ( Strm, '/' );
            ULAtomToStrm ( _LIB, Strm, _CosNameValue ( CosObject ) );
            return;
        case CosString:
            if ( _CosStringHex ( CosObject ) ){
                ULStreamWriteChar ( Strm, '<' );
                hex[2] = '\0';
                for ( i = 0; i < _CosStringSize ( CosObject ); i++ ){
                        hex[0] = hexchar[(_CosStringValue( CosObject )[i] & 0xf0)  >> 4];
                        hex[1] = hexchar[_CosStringValue( CosObject )[i] & 0xf];
                        ULStrToStrm ( Strm, hex );
                    };
                ULStreamWriteChar ( Strm, '>' );
            }else{
                ULStreamWriteChar ( Strm, '(' );
                ops = ULGetEscString ( _LIB, _CosStringValue( CosObject ),
                    _CosStringSize ( CosObject ));
                ULStrToStrm ( Strm, ops );
                mfree ( _LIB, ops );
                ULStreamWriteChar ( Strm, ')' );
            };
            return;
        case CosArray:
            if ( _CosArrayMinLen ( CosObject ) )
                k = ULStreamGetPosition ( Strm );
            ULStrToStrm ( Strm, "[ " );
            for ( i = 0; i < _CosArrayCount ( CosObject ); i++ ){
                if ( i &&  _CosGetType ( _CosArrayItem ( CosObject, i ) ) < CosName )
                    ULStreamWriteChar ( Strm, ' ' );
                CosCopyObjToStream ( _CosArrayItem ( CosObject, i ), Strm );
            };
            ULStreamWriteChar ( Strm, ']' );
            if ( _CosArrayMinLen ( CosObject ) )
                for ( k = ULStreamGetPosition ( Strm ) - k; k < _CosArrayMinLen ( CosObject ); k++ )
                    ULStreamWriteChar ( Strm, ' ' );
            return;
        case CosDict:
            ULStrToStrm ( Strm, "<<" );
            for ( i = 0; i < _CosDictCount ( CosObject ); i++ ){
                ULStreamWriteChar ( Strm, '/' );
                ULAtomToStrm ( _LIB, Strm, _CosDictKey ( CosObject, i ) );
                if ( _CosGetType ( _CosDictValue( CosObject, i ) ) < CosName )
                    ULStreamWriteChar ( Strm, ' ' );              
                CosCopyObjToStream ( _CosDictValue( CosObject, i ), Strm);
            };
            ULStrToStrm ( Strm, ">>" );
            return;
        case CosStream:
            CosCopyObjToStream ( _CosStreamAttr ( CosObject ), Strm );
            ULStrToStrm ( Strm, "stream\r\n" );
            ULStreamCopyToStream ( _CosStreamValue ( CosObject ), Strm );
            ULStrToStrm ( Strm, "\nendstream" );
            return;
        default:
            return;
    };
}

ppBool CosIsIndirect( PDFCosHandle CosObject )
{
    return _CosIsIndirect ( CosObject );
}

