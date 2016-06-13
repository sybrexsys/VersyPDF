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
VSCosAI.h
*********************************************************************/

#ifndef VSCosAI_H
#define VSCosAI_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */



#include "../VSTypes.h"
#include "../VSCosA.h"

#ifdef __cplusplus
extern "C"{
#endif

/* Internal Object types */

#define     CosRef          1
#define     CosWork         1024 
#define     CosIntWork      2048
#define		CosEOF			4096


    typedef struct _t_PDFID {
        ppUns32 ID;             /* Master index of the indirect object      */
        ppUns16 GenID;          /* Generation number of the indirect object */
    } PDFID, *PPDFID;

    typedef struct _t_DictElem {
        ppAtom          Key;    /* Key part of the dictionary pair          */
        PDFCosHandle    Value;  /* Value part of the dictionary pair        */
    } DictElem, *PDictElem;


    /*  Int Object  */
    typedef struct _t_CosInt {
        ppInt32 Value;          /* Integer value of the integer cos object  */
    } TCosInt;

    /*  Real Object  */
    typedef struct _t_CosReal {
        ppReal  Value;          /* Real value of the real cos object        */
    } TCosReal;

    /* Boolean Object */
    typedef struct _t_CosBool {
        ppBool  Value;          /* Boolean value of the boolean cos object  */
    } TCosBool;

    /*  Array  */
    typedef struct _t_CosArray {
        ppUns32         Count;      /* Count of the elements in the array       */
        ppUns32         Capacity;   /* Capacity of the elements in the array    */
        ppUns32         MinStrLen;  /* Minimal size of the array which stored in stream */
        PDFCosHandle    *Array;     /* Array of the pointers to Cos elements    */
    } TCosArray;

    /*  String  */
    typedef struct _t_CosString {
        ppBool          IsHex;      /* If true data stored to stream in hex else in escaped ansi string */
        char            *Buffer;    /* Buffer where stored value                */
        ppUns32         Size;       /* Size of the value data                   */
    } TCosString;

    /*  Name  */
    typedef struct _t_CosName {
        ppAtom  Value;              /* Name value of the name cos object        */
    } TCosName;


    /*  Dictionary  */
    typedef struct _t_CosDict {
        ppUns32         Count;      /* Count of the pair (key-value) in the dict        */
        ppUns32         Capacity;   /* Capacity of the pair (key-value) in the array    */
        DictElem        *Array;     /* Array of the pointers to the pair (key-value)    */
    } TCosDict;

    /*  Stream  */
    typedef struct _t_CosStream {
        PDFCosHandle    Attr;       /* Attribute part of the stream                     */
        PDFStreamHandle Store;      /* Data part of the stream                          */
        ppBool          Filtered;   /* Flag meaning is filtered this stream or not      */
    } TCosStream;

    /*  Inderect Link */
    typedef struct _t_CosRef {
        PDFID   Value;              /* ID value of the reference object                 */
    } TCosRef;

    typedef  struct _t_CosWork {
        char    *command;           /* Value of the work command object                 */
    } TCosWork;

    typedef union _t_CosObjData {
        TCosInt     iv;             /* Cos Integer value    */
        TCosBool    bv;             /* Cos Bool value       */
        TCosReal    rv;             /* Cos Real value       */
        TCosRef     rfv;            /* Cos Reference value  */
        TCosName    nv;             /* Cos Name value       */
        TCosString  sv;             /* Cos String value     */
        TCosStream  smv;            /* Cos Stream value     */
        TCosDict    dv;             /* Cos Dict value       */
        TCosArray   av;             /* Cos Array value      */
        TCosWork    wv;             /* Cos Work value       */
    } CosObjData;

    typedef struct _t_CosObj {
        PDFDocHandle    Doc;            /* Doc where this cos object will used                              */
        CosType         Type;           /* Type of the cos object                                           */
        ppBool          IsIndirect;     /* Boolean value detect is Indirect this object or not              */
        union {
            struct _t_CosObj  *Parent;  /* If IsIndirect set to false this field link to parent cos object  */
            PDFID           ID;         /* If IsIndirect set to true this field store index and generation  */
        }               ObjInfo;
        CosObjData      Data;           /* Here stored all info of the cos object                           */
    } TCosObj, *CosObj;

	/********************************************************************
    Description: 
        Creates a new EOF object.
    Parameters: 
        Doc         - [in]  Document for which will create a work object.
    Returns:    
        The newly-created EOF Cos object.
    *********************************************************************/

	PDFCosHandle CosEOFNew( PDFDocHandle Doc);

    /********************************************************************
    Description: 
        Creates a new work object and having the specified value.
    Parameters: 
        Doc         - [in]  Document for which will create a work object.
        String      - [in]  Value of the newly created work object.
    Returns:    
        The newly-created work Cos object.
    *********************************************************************/
    PDFCosHandle CosNewWork(PDFDocHandle Doc, char *String);



    /********************************************************************
    Description: 
        Receive value of the work cos object.
    Parameters: 
        CosObject   - [in]  The cos object for which need receive value.
    Returns:    
        The value of the work cos object.
    *********************************************************************/
    char *CosGetWorkCommand(PDFCosHandle CosObject);


    /********************************************************************
    Description: 
        Creates a new int work object and having the specified value.
    Parameters: 
        Doc         - [in]  Document for which will create a int work object.
        Value       - [in]  Value of the newly created int work object.
    Returns:    
        The newly-created int work Cos object.
    *********************************************************************/
    PDFCosHandle CosNewIntWork(PDFDocHandle Doc, ppInt32 Value);



    /********************************************************************
    Description: 
        Receive value of the int work cos object.
    Parameters: 
        CosObject   - [in]  The cos object for which need receive value.
    Returns:    
        The value of the int work cos object.
    *********************************************************************/
    ppInt32 CosGetIntWorkValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Convert dict cos object to stream cos object.
    Parameters: 
        CosObject   - [in]  The dict cos object which need convert to stream.
    Returns:    
        The newly-created stream cos object.
    *********************************************************************/
    PDFCosHandle CosDictToStream(PDFCosHandle CosObject);


    /********************************************************************
    Description: 
        Check cos stream object have it any fiters and set Filtered property of the 
        cos stream object.
    Parameters: 
        CosObject   - [in]  The cos object which need check filters.
    Returns:    
        None.
    *********************************************************************/
    void CosStreamCheckFilters( PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Return is filtered cos stream or not .
    Parameters: 
        CosObject   - [in]  The cos stream object for which need return Filtered parameter.
    Returns:    
        True if cos stream is filtered and false, if cosstream not filtered.
    *********************************************************************/
    ppBool CosStreamIsFiltered( PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Delete indirect object with specified index from document.
    Parameters: 
        Doc         - [in]  The document from which need delete indirect cos object.
        ID          - [in]  The index of the cos object which need delete.
    Returns:    
        None.
    *********************************************************************/
    void PDFDocDeleteObj( PDFDocHandle Doc, ppUns32 ID );



    /********************************************************************
    Description: 
        Delete indirect object with specified index from document and mark this object as newer used.
    Parameters: 
        Doc         - [in]  The document from which need delete indirect cos object.
        ID          - [in]  The index of the cos object which need delete.
    Returns:    
        None.
    *********************************************************************/
    void PDFDocDeleteObjEx( PDFDocHandle Doc, ppUns32 ID );

    /********************************************************************
    Description: 
        Calculate size of the cos object need for store this cos object to stream.
    Parameters: 
        CosObject   - [in]  Cos object size of which need receive.
    Returns:    
        Size of the cos object in bytes.
    *********************************************************************/
    ppUns32 CosGetObjSize( PDFCosHandle CosObject);

    /********************************************************************
    Description: 
        Copy cos object to stream.
    Parameters: 
        Doc         - [in]  The cos object which need copy.
        Strm        - [in]  The stream to which will stored cos object.
    Returns:    
        None.
    *********************************************************************/
    void CosCopyObjToStream( PDFCosHandle CosObject, PDFStreamHandle Strm);

    /*  Inderect Link Object */

    /********************************************************************
    Description: 
        Creates a new reference object and having the specified value.
    Parameters: 
        Doc         - [in]  Document in which this object will used.
        ID          - [in]  The ID part of value the new reference will have.
        GenID       - [in]  The GenID part of value the new reference will have.
    Returns:    
        The newly-created reference Cos object.
    *********************************************************************/
    PDFCosHandle CosNewRef(PDFDocHandle Doc, ppInt32 ID, ppUns16 GenID);


    /********************************************************************
    Description: 
        Gets the value of the specified reference object.
    Parameters: 
        CosObject   - [in]  The reference Cos object whose value is obtained.
    Returns:    
        Value of the reference Cos object.
    *********************************************************************/
    ppInt32 CosGetRefValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Sets the value of the specified reference object.
    Parameters: 
        CosObject   - [in]  The reference Cos object whose value is assigned.
        NewID       - [in]  New ID part of value of the reference Cos object
        NewGenID    - [in]  New GenID part of value of the reference Cos object
    Returns: 
        None
    *********************************************************************/
    void CosSetRefValue(PDFCosHandle CosObject, ppInt32 NewID, ppUns16 NewGenID);


    /********************************************************************
    Description: 
        Gets the specified element from an array. 
    Parameters: 
        CosObject   - [in]  The array from which an element is obtained.
        Index       - [in]  The Index for the array member to obtain. Array indices start at 0.
    Returns:    
        The Cos object occupying the indexth element of array. Returns a null Cos object
        if Index is outside the array bounds. 
    *********************************************************************/
    PDFCosHandle CosArrayItemNRF(PDFCosHandle CosObject, ppUns32 Index);


    /********************************************************************
    Summary: 
        Gets the value of the specified key in the specified dictionary.
    Description: 
        Gets the value of the specified key in the specified dictionary. If called with a stream
        object instead of a dictionary object, this method gets the value of the specified key
        from the stream's attributes dictionary.
    Parameters: 
        CosObject   - [in]  The dictionary or stream from which a value is obtained.
        Key         - [in]  The key whose value is obtained.
    Returns:    
        The object associated with the specified key. Returns a null Cos object if key is not present.
    Example:
        <CODE>
        PDFCosHandle dict, obj;
        obj = CosDicValueByNameNRF ( Lib, dict, ULStringToAtom ( Lib, "Pages" ) );
        </CODE>
    *********************************************************************/
    PDFCosHandle CosDictValueByNameNRF( PDFCosHandle CosObject, ppAtom Key);



    /********************************************************************
    Description: 
        Add cos object to pdf document, mark this cos object as indirect.
    Parameters: 
        Doc         - [in]  The document to which need append cos object..
        CosObject   - [in]  The cos object which will appended to document.
    Returns:    
        None.
    *********************************************************************/
    void CosAddToDoc(PDFDocHandle Doc, PDFCosHandle CosObject);
#ifndef COSOBJISPOINTER
#   define CO(x) ((CosObj)(x.a))
#   define SetCO(c, x) {c.a = x; }
#   define _CosNullNew(Doc, x) {x.a=( & ( CD ( Doc )->NullObject ) );}
#else
#   define CO(x) ((CosObj)(x))
#   define SetCO(c, x) {c = x; }
#   define _CosNullNew(Doc, x) {x=( & ( CD ( Doc )->NullObject ) );}
#	define _CosNull(Doc) ( & ( CD ( Doc )->NullObject ) )
#endif 

/*  Macros for incapsulate  Cos structure */ 



    /* Macros for CosInt */
#define _CosIntValue(CosObject) ( CO ( CosObject )->Data.iv.Value )

    /* Macros for CosIntWork */
#define _CosIntWorkValue(CosObject)  ( CO ( CosObject )->Data.iv.Value )

    /* Macros for CosReal */
#define _CosRealValue(CosObject) ( CO ( CosObject )->Data.rv.Value )

    /* Macros for CosBool */
#define _CosBoolValue(CosObject) ( CO ( CosObject )->Data.bv.Value )

    /* Macros for CosName */
#define _CosNameValue(CosObject) ( CO ( CosObject )->Data.nv.Value )

    /* Macros for CosRef */
#define _CosRefValue(CosObject) ( CO ( CosObject )->Data.rfv.Value )


    /* Macros for CosWork*/
#define _CosWorkCommand(CosObject) ( CO ( CosObject )->Data.sv.Buffer )

    /* MAcros for strings */
#define _CosStringHex(CosObject) ( CO ( CosObject )->Data.sv.IsHex )
#define _CosStringSize(CosObject) ( CO ( CosObject )->Data.sv.Size )
#define _CosStringValue(CosObject) ( CO ( CosObject )->Data.sv.Buffer )


    /* Macros for array */
#define _CosArrayCount(CosObject) ( CO ( CosObject )->Data.av.Count )
#define _CosArrayCapacity(CosObject) ( CO ( CosObject )->Data.av.Capacity )
#define _CosArrayItem(CosObject,Index)   ( CO ( CosObject )->Data.av.Array[Index] )
#define _CosArrayArray(CosObject)   ( CO ( CosObject )->Data.av.Array )
#define _CosArrayMinLen(CosObject)   ( CO ( CosObject )->Data.av.MinStrLen )

    /* *Macros for Dictionary */
#define _CosDictCount(CosObject) ( CO ( CosObject )->Data.dv.Count )
#define _CosDictKey(CosObject, Index)  ( CO ( CosObject )->Data.dv.Array[Index].Key )
#define _CosDictValue(CosObject, Index)  ( CO ( CosObject )->Data.dv.Array[Index].Value )
#define _CosDictCapacity(CosObject) ( CO ( CosObject )->Data.dv.Capacity )
#define _CosDictPair(CosObject,Index)   ( CO ( CosObject )->Data.dv.Array[Index] )
#define _CosDictArray(CosObject)   ( CO ( CosObject )->Data.dv.Array )

    /* Macros for stream */
#define _CosStreamAttr(CosObject) ( CO ( CosObject )->Data.smv.Attr )
#define _CosStreamValue(CosObject) ( CO ( CosObject )->Data.smv.Store )
#define _CosStreamIsFiltered(CosObject) ( CO ( CosObject )->Data.smv.Filtered )



#define _CosDictAppend(obj,name,newobj) CosDictAppend ( obj, GetAtomDoc ( name ), newobj )
#define _CosDictValueByName(obj,name) CosDictValueByName ( obj, GetAtomDoc (name) )
#define _CosDictValueByNameNRF(obj,name) CosDictValueByNameNRF( obj, GetAtomDoc ( name ) )
#define _CosDictRemoveKey(obj,key) CosDictRemoveKey ( obj, GetAtomDoc ( key ) )
#define _CosNameNew(name) CosNameNew ( _DOC, false, GetAtomDoc ( name ) )

#define __CosDictAppend(obj,name,newobj) CosDictAppend ( obj, GetAtomDDoc ( name ), newobj )
#define __CosDictValueByName(obj,name) CosDictValueByName ( obj, GetAtomDDoc (name) )
#define __CosDictValueByNameNRF(obj,name) CosDictValueByNameNRF( obj, GetAtomDDoc ( name ) )
#define __CosDictRemoveKey(obj,key) CosDictRemoveKey ( obj, GetAtomDDoc ( key ) )
#define __CosNameNew(name) CosNameNew ( _DDOC, false, GetAtomDDoc ( name ) )



    


    /* Other working macros */
#define _CosGetType(CosObject) ( (ppUns32)CO ( CosObject )->Type )

#define _CosParent(CosObject) ( CO ( CosObject )->ObjInfo.Parent )

#define _CosDoc(CosObject) ( CO ( CosObject )->Doc )

#define _CosLib(CosObject) ( CD( _CosDoc( CosObject ) ) ->Lib )

#define _CosIsNumber(CosObject) ( _CosGetType ( CosObject ) & ( CosReal | CosInteger ) )

#define _IsCosIncludeR(CosObject) ( _CosGetType (CosObject ) & (CosDict | CosArray | CosStream | CosRef ))

#define _IsCosInclude(CosObject) ( _CosGetType (CosObject ) & (CosDict | CosArray | CosStream  ))


/* Macros for CosTypes */
#define _IsCosArray(CosObject) ( (ppUns32)CO ( CosObject )->Type == CosArray )

#define _IsCosDict(CosObject) ( (ppUns32)CO ( CosObject )->Type == CosDict )

#define _IsCosStream(CosObject) ( (ppUns32)CO( CosObject )->Type == CosStream )

#define _IsCosDictOrStream(CosObject) ( _CosGetType ( CosObject ) & ( CosDict | CosStream ) )

#define _IsCosString(CosObject) ( (ppUns32)CO (CosObject )->Type == CosString )

#define _IsCosName(CosObject) ( (ppUns32)CO (CosObject )->Type == CosName )

#define _IsCosBool(CosObject) ( (ppUns32)CO (CosObject )->Type == CosBoolean )

#define _IsCosInt(CosObject) ( (ppUns32)CO( CosObject )->Type == CosInteger )

#define _IsCosIntWork(CosObject) ( (ppUns32)CO( CosObject )->Type == CosIntWork )

#define _IsCosReal(CosObject) ( (ppUns32)CO( CosObject )->Type == CosReal )

#define _IsCosNumber(CosObject) ( _IsCosReal(CosObject) || _IsCosInt( CosObject ) )

#define _IsCosRef(CosObject) ( (ppUns32)CO ( CosObject )->Type == CosRef )

#define _IsCosWork(CosObject) ( (ppUns32)CO ( CosObject )->Type == CosWork )

#define _IsCosNull(CosObject) ( (ppUns32)CO ( CosObject )->Type==CosNull)




#define _CosIsIndirect(CosObject) ( CO ( CosObject )->IsIndirect )

#define _CosIndirectDict(CosObject) ( _IsCosDict ( CosObject ) && _CosIsIndirect ( CosObject ) )

#define _IsCosNameValue(CosObject, Key) (_IsCosName(CosObject) && _CosNameValue(CosObject)==Key )

#define _CosObjID(CosObject) ( CO ( CosObject )->ObjInfo.ID.ID )

#define _CosObjGeneration(CosObject) ( CO( CosObject )->ObjInfo.ID.GenID )

#define _CosObjFullID(CosObject) ( CO (CosObject )->ObjInfo.ID )



#define _CosEqual(a,b) (CO(a)==CO(b))

#define _CosNumGetValue(CosObject) ( _IsCosInt(CosObject)?_CosIntValue(CosObject):_CosRealValue(CosObject))

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSCosAI_H */
