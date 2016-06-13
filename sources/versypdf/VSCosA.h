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
VSCosA.h
*********************************************************************/

#ifndef VSCosA_H
#define VSCosA_H  /*  */
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */
#include "VSTypes.h"

#ifdef __cplusplus
extern "C"{
#endif
         
#define ppRealNULL -32000 /* Cos real NULL */ 

	/* Cos objects */ 

    /* Available Cos objects */ 
    _EXP
    typedef enum {
        CosNull                                 = 2, /* NULL Cos object         */
        CosInteger                              = 4, /* Integer Cos Object      */
        CosReal                                 = 8, /* Real Cos Object         */
        CosBoolean                              = 16, /* Boolean Cos Object      */
        CosName                                 = 32, /* Name Cos Object         */
        CosString                               = 64, /* String Cos Object       */
        CosDict                                 = 128, /* Dictionary Cos Object   */
        CosArray                                = 256, /* Array Cos Object        */
        CosStream                               = 512  /* Stream Cos Object       */
    } CosType;

    /*  Null Object  */

    /********************************************************************
    Description: 
        Creates a new direct null object.
    Parameters:
        Doc         - [in]  The document in which the null object is used.
    Returns: 
        The newly-created null Cos object.
    See Also:
        CosFree
        CosGetType
    *********************************************************************/
    LIB_API PDFCosHandle CosNullNew( PDFDocHandle Doc );



    /*  Int Object  */

    /* Description
       Creates a new integer object and sets the specified value.
       Parameters
       Doc :         [in] The document in which the integer is used.
       IsIndirect :  [in] If true, creates the integer object as an
                     indirect object.
       Value :       [in] The value the new integer will have.
       Returns
       The newly-created integer Cos object.
       See Also
       CosFree CosGetType CosIntGetValue CosIntSetValue           */
    LIB_API PDFCosHandle CosIntNew(PDFDocHandle Doc, ppBool IsIndirect, ppInt32 Value);



    /********************************************************************
    Description: 
        Gets the value of the specified integer object.
    Parameters: 
        CosObject   - [in]  The integer Cos object whose value is obtained.
    Returns:
        Value of the integer Cos object.
    See Also:
        CosIntNew
        CosIntSetValue
    *********************************************************************/
    LIB_API ppInt32 CosIntGetValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Sets the value of the specified integer object.
    Parameters: 
        CosObject   - [in]  The integer Cos object whose value is assigned.
        Value       - [in]  New value of the integer Cos object.
    Returns: 
        None
    See Also:
        CosIntNew
        CosIntGetValue
    *********************************************************************/
    LIB_API void CosIntSetValue(PDFCosHandle CosObject, ppInt32 Value);



    /*  Real Object  */


    /* Description
       Creates a new real object and sets the specified value.
       Parameters
       Doc :         [in] The document in which the real is used.
       IsIndirect :  [in] If true, creates the real object as an indirect
                     object.
       Value :       [in] The value the new real will have.
       Returns
       The newly-created real Cos object.
       See Also
       CosFree CosGetType CosRealGetValue CosRealSetValue              */
    LIB_API PDFCosHandle CosRealNew(PDFDocHandle Doc, ppBool IsIndirect,  ppReal Value);



    /********************************************************************
    Description: 
        Gets the value of the specified real object.
    Parameters: 
        CosObject   - [in]  The real Cos object whose value is obtained.
    Returns:
        Value of the real Cos object.
    See Also:
        CosRealNew
        CosRealSetValue
    *********************************************************************/
    LIB_API ppReal CosRealGetValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Sets the value of the specified real object.
    Parameters: 
        CosObject   - [in]  The real Cos object whose value is assigned.
        Value       - [in]  New value of the real Cos object.
    Returns: 
        None
    See Also:
        CosRealNew
        CosRealGetValue
    *********************************************************************/
    LIB_API void CosRealSetValue(PDFCosHandle CosObject, ppReal Value);



    /*  Boolean Object  */


    /* Description
       Creates a new boolean object and sets the specified value.
       Parameters
       Doc :         [in] The document in which the boolean is used.
       IsIndirect :  [in] If true, creates the boolean object as an
                     indirect object.
       Value :       [in] The value which new boolean will have.
       Returns
       The newly-created boolean Cos object.
       See Also
       CosFree CosGetType CosBoolGetValue CosBoolSetValue         */
    LIB_API PDFCosHandle CosBoolNew(PDFDocHandle Doc, ppBool IsIndirect, ppBool Value);



    /********************************************************************
    Description: 
        Gets the value of the specified boolean object.
    Parameters: 
        CosObject   - [in]  The boolean Cos object whose value is obtained.
    Returns:    
        Value of the boolean Cos object.
    See Also:
        CosBoolNew
        CosBoolSetValue
    *********************************************************************/
    LIB_API ppBool CosBoolGetValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Sets the value of the specified boolean object.
    Parameters: 
        CosObject   - [in]  The boolean Cos object whose value is assigned.
        Value       - [in]  New value of the Cos boolean object.
    Returns: 
        None
    See Also:
        CosBoolNew
        CosBoolGetValue
    *********************************************************************/
    LIB_API void CosBoolSetValue(PDFCosHandle CosObject, ppBool Value);



    /*  String  Object */


    /* Description
       Creates a new string object and sets the specified value.
       Parameters
       Doc :         [in] The document in which the string is used.
       IsIndirect :  [in] If true, creates the string object as an indirect
                     object.
       String :      [in] The value that the new string will have. It is not
                     a C string, since Cos strings can contain NULL
                     characters. The data in String is copied, that is, if
                     String was dynamically allocated, it can be free after
                     this call.
       Length :      [in] The length of String.
       Returns
       The newly-created string Cos object.
       See Also
       CosFree CosGetType CosStringGetValue CosStringSetValue             */
    LIB_API PDFCosHandle CosStringNew(PDFDocHandle Doc, ppBool IsIndirect, char *String, ppUns32 Length);



    /* Description
       Gets the value of string Cos object and the string's length.
       Parameters
       CosObject :  [in] The string Cos object whose value is
                    obtained.
       Length :     [out] Length of the value in bytes.
       Returns
       The value of string Cos object.
       See Also
       CosStringNew CosStringSetValue                               */
    LIB_API char *CosStringGetValue(PDFCosHandle CosObject, ppUns32 *Length);



    /* Description
       Sets the new value for string Cos object.
       Parameters
       CosObject :  [in] The string Cos object whose value is assigned.
       String :     [in] The new value that the string Cos object will have.
                    It is not a C string, since Cos strings can contain NULL
                    characters. The data in String is copied, that is, if
                    String was dynamically allocated, it can be free after
                    this call.
       Length :     [in] The new length of String.
       Returns
       None.
       See Also
       CosStringNew CosStringGetValue                                        */
    LIB_API void CosStringSetValue(PDFCosHandle CosObject, char *String, ppUns32 Length);



    /*  Name Object */


    /* Description
       Creates a new name object and sets the specified value.
       Parameters
       Doc :         [in] The document in which the name object is used.
       IsIndirect :  [in] If true, creates the name object as an indirect
                     object.
       Value :       [in] The value the new name will have.
       Returns
       The newly-created name Cos object.
       See Also
       CosFree CosGetType CosNameGetValue CosNameSetValue              */
    LIB_API PDFCosHandle CosNameNew(PDFDocHandle Doc, ppBool IsIndirect, ppAtom Value);



    /********************************************************************
    Description: 
        Gets the value of the specified name object.
    Parameters: 
        CosObject   - [in]  The name Cos object whose value is obtained.
    Returns:    
        Value of the name Cos object.
    See Also:
        CosNameNew
        CosNameSetValue
    *********************************************************************/
    LIB_API ppAtom CosNameGetValue(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Sets the value of the specified name object.
    Parameters: 
        CosObject   - [in]  The boolean Cos object whose value is assigned.
        Value       - [in]  New value of the name Cos object.
    Returns: 
        None
    See Also:
        CosNameNew
        CosNameGetValue
    *********************************************************************/
    LIB_API void CosNameSetValue(PDFCosHandle CosObject, ppAtom Value);



    /*  Array  */


    /********************************************************************
    Description: 
        Creates and returns a new array Cos object.
    Parameters: 
        Doc         - [in]  The document in which the array is used.
        IsIndirect  - [in]  If true, creates the array object 
                            as an indirect object.  
        Entries     - [in]  The number of elements that will be in the array. This value
                            only a hint; Cos arrays grow dynamically as needed.
    Returns:    
        The newly-created array Cos object.
    See Also:
        CosFree
        CosGetType
        CosArrayCount
        CosArrayInsert
        CosArrayAppend
        CosArrayRemove
        CosArrayClear
    *********************************************************************/
    LIB_API PDFCosHandle CosArrayNew(PDFDocHandle Doc, ppBool IsIndirect, ppUns32 Entries);



    /* Description
       Gets the number of elements in array.
       Parameters
       CosObject :  [in] The array for which the number of elements are
                    determined.
       Returns
       The number of elements in array.
       See Also
       CosArrayNew CosArrayInsert CosArrayAppend CosArrayRemove
       CosArrayClear                                                    */
    LIB_API ppUns32 CosArrayCount(PDFCosHandle CosObject);



    /* Description
       Inserts an cos object into an array.
       Parameters
       CosObject :     [in] The array into which the object is inserted.
       NewCosObject :  [in] The object to insert.
       Position :      [in] The location in the array to insert the cos
                       object. The cos object is inserted before the specified
                       location. The first element in an array has a pos of
                       zero. If pos \>= CosArrayCount ( CosObject ), it
                       appends obj to array (increasing the array count by 1).
       Returns
       Position in which Cos object was inserted.
       See Also
       CosArrayNew CosArrayCount CosArrayInsert CosArrayAppend
       CosArrayRemove CosArrayClear                                            */
    LIB_API ppInt32 CosArrayInsert(PDFCosHandle CosObject, PDFCosHandle NewCosObject, ppUns32 pos);



    /* Description
       Appends an cos object into an array.
       Parameters
       CosObject :     [in] The array into which the object is
                       appended.
       NewCosObject :  [in] The object to append.
       Returns
       Position in which Cos object was inserted.
       See Also
       CosArrayNew CosArrayCount CosArrayInsert CosArrayAppend
       CosArrayRemove CosArrayClear                            */
    LIB_API ppInt32 CosArrayAppend(PDFCosHandle CosObject, PDFCosHandle NewCosObject);



    /* Summary
       Removes element from array.
       Description
       Checks whether the position is within the array bounds and
       then removes it from the array and moves each subsequent
       element to the slot with the next smaller Index and
       decrements the array's length by 1. Removed element will be
       free.
       Parameters
       CosObject :  [in] The array Cos object to remove the member from
                    it.
       Index :      [in] The Index for the array member to remove. Array
                    indices start at 0.
       Returns
       None
       See Also
       CosArrayNew CosArrayCount CosArrayInsert CosArrayAppend
       CosArrayRemove CosArrayClear                                      */
    LIB_API void CosArrayRemove(PDFCosHandle CosObject, ppUns32 Index);



    /* Description
       Gets the specified element from an array.
       Parameters
       CosObject :  [in] The array from which an element is obtained.
       Index :      [in] The Index for the array member to obtain. Array
                    indices start at 0.
       Returns
       The Cos object occupying the index element of array. Returns
       a null Cos object if Index is outside the array bounds. If
       specified element is referenced Cos object function returns
       Cos object with ID equal to value of referenced Cos object.
       See Also
       CosArrayNew CosArrayCount CosArrayInsert CosArrayAppend
       CosArrayRemove CosArrayClear                                      */
    LIB_API PDFCosHandle CosArrayItem(PDFCosHandle CosObject, ppUns32 Index);



    /* Description
       Clears and gets free all elements from an array.
       Parameters
       CosObject :  [in] The array from which an elements are removed.
       Returns
       None
       See Also
       CosArrayNew CosArrayCount CosArrayInsert CosArrayAppend
       CosArrayRemove CosArrayClear                                    */
    LIB_API void CosArrayClear(PDFCosHandle CosObject);



    /*  Dictionary  */


    /********************************************************************
    Description: 
        Creates a new dictionary.
    Parameters: 
        Doc         - [in]  The document in which the dictionary is used.
        IsIndirect  - [in]  If true, creates the dictionary object 
                            as an indirect object.  
        Entries     - [in]  Number of entries in the dictionary. This value is 
                            only a hint - Cos dictionaries grow dynamically as needed.
    Returns:
        The newly-created dictionary Cos object.
    See Also:
        CosGetType
        CosFree
        CosDictCount
        CosDictGetPair
        CosDictAppend
        CosDictRemoveKey
        CosDictValueByName
        CosDictClear
    *********************************************************************/
    LIB_API PDFCosHandle CosDictNew(PDFDocHandle Doc, ppBool IsIndirect, ppUns32 Entries);



    /* Description
       Gets the number of key-value pair in the dictionary. This
       method can also be used with a stream object. In that case,
       \returns number the key-value pair from the stream's
       attributes dictionary.
       Parameters
       CosObject :  [in] The dictionary or stream for which the number of
                    key\-value pair is determined.
       Returns
       The number of key-value pair in the dictionary.
       See Also
       CosDictNew CosDictGetPair CosDictAppend CosDictRemoveKey
       CosDictValueByName CosDictClear                                    */
    LIB_API ppInt32 CosDictCount(PDFCosHandle CosObject);


    /* Description
       Gets the key-value pair in the dictionary. This method can
       also be used with a stream object. In that case, returns the
       key-value pair from the stream's attributes dictionary.
       Parameters
       CosObject :  [in] The dictionary or stream for which the
                    key\-value pair is determined.
       Index :      [in] Index of the pair for which is needed to obtain
                    key and value.
       Key :        [out] Key from pair.
       Value :      [out] Value from pair.
       See Also
       CosDictNew CosDictCount CosDictAppend CosDictRemoveKey
       CosDictValueByName CosDictClear                                   */
    LIB_API void CosDictGetPair(PDFCosHandle CosObject, ppUns32 Index, ppAtom *Key, PDFCosHandle *Value);


    /* Description
       Sets the value of a dictionary key, adding the key to the
       dictionary. This method can also be used with a stream
       \object. In that case, the key-value pair is added to the
       stream's attributes dictionary.
       Parameters
       CosObject :  [in] The dictionary or stream in which a value is
                    set.
       Key :        [in] The key which value is set.
       Value :      [in] The value to set.
       Returns
       None
       See Also
       CosDictNew CosDictCount CosDictGetPair CosDictRemoveKey
       CosDictValueByName CosDictClear                                */
    LIB_API void CosDictAppend(PDFCosHandle CosObject, ppAtom Key, PDFCosHandle KeyValue);



    /* Description
       Removes and gets free a key-value pair from a dictionary.
       This method can also be used with a stream object. In that
       case, the key-value pair is removed from the stream's
       attributes dictionary.
       Parameters
       CosObject :  [in] The dictionary from which the key\-value pair
                    is removed.
       Key :        [in] The key to remove.
       Returns
       None
       See Also
       CosDictNew CosDictCount CosDictGetPair CosDictAppend
       CosDictValueByName CosDictClear                                 */
    LIB_API void CosDictRemoveKey(PDFCosHandle CosObject, ppAtom Key);



    /* Description
       Gets the value of the specified key in the specified
       dictionary. If it's called with a stream object instead of a
       dictionary object, this method gets the value of the
       specified key from the stream's attributes dictionary.
       Parameters
       CosObject :  [in] The dictionary or stream from which a value is
                    obtained.
       Key :        [in] The key whose value is obtained.
       Returns
       The object associated with the specified key. Returns a null
       Cos object if key is not present. If value is referenced Cos
       \object returns Cos object with ID equal to value of
       referenced Cos object.
       Example
       <code>
           PDFCosHandle dict, obj;
           \obj = CosDicValueByName ( dict, ULStringToAtom ( Lib, "Pages" ) );
       </code>
       See Also
       CosDictNew CosDictCount CosDictGetPair CosDictAppend
       CosDictRemoveKey CosDictClear                                           */
    LIB_API PDFCosHandle CosDictValueByName(PDFCosHandle CosObject, ppAtom Key);



    /* Description
       Clears and gets free all keys and values from the dictionary
       \or stream.
       Parameters
       CosObject :  [in] The dictionary or stream from which elements
                    are removed.
       Returns
       None
       See Also
       CosDictNew CosDictCount CosDictGetPair CosDictAppend
       CosDictRemoveKey CosDictValueByName                            */
    LIB_API void CosDictClear( PDFCosHandle CosObject);

    /*  Stream  */


    /********************************************************************
    Description: 
        Creates a new stream.
    Parameters: 
        Doc         - [in]  The document in which the dictionary is used.
        IsIndirect  - [in]  Must always be true, specifying that the Cos stream is
                            created as an indirect object.
        Entries     - [in]  Number of entries in the attribute dictionary. This value 
                            is only a hint - Cos dictionaries grow dynamically as needed.
    Returns:
        The newly-created stream Cos object.
    See Also:
        CosFree
        CosGetType
        CosSteamGetAttr
        CosStreamGetValue
    *********************************************************************/
    LIB_API PDFCosHandle CosStreamNew(PDFDocHandle Doc, ppBool IsIndirect, ppInt32 Entries);



    /********************************************************************
    Description: 
        Gets a stream's attributes dictionary.
    Parameters: 
        CosObject   - [in]  The stream whose attributes dictionary is obtained.
    Returns:
        The stream's attributes dictionary Cos object.
    See Also:
        CosStreamNew
        CosStreamGetValue
    *********************************************************************/
    LIB_API PDFCosHandle CosStreamGetAttr(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Gets a stream's value.
    Parameters: 
        CosObject   - [in]  The stream whose attributes value is obtained.
    Returns:    
        The value of the stream Cos object.
    See Also:
        CosStreamNew
        CosStreamGetAttr
    *********************************************************************/
    LIB_API PDFStreamHandle CosStreamGetValue(PDFCosHandle CosObject);



    /*   Other Functions for work with CosObject */


    /********************************************************************
    Description: 
        Gets an object’s type.
    Parameters: 
        CosObject   - [in]  The object whose type is obtained.
    Returns:    
        The object’s type.
    *********************************************************************/
    LIB_API CosType CosGetType(PDFCosHandle CosObject);



    /* Description
       Gets free Cos object. If it's a composite object (array,
       dictionary or stream) :
         * all the direct objects in it will be automatically
           destroyed
         * the indirect objects in it will be not destroyed
       Parameters
       CosObject :  [in] The object to free.
       Returns
       None.                                                    */
    LIB_API void CosFree(PDFCosHandle CosObject);



    /********************************************************************
    Description: 
        Gets the value of the specified real or integer object.
    Parameters: 
        CosObject   - [in]  The real or integer Cos object whose value is obtained.
    Returns:
        Value of the real or integer Cos object.
    *********************************************************************/    
    LIB_API ppReal CosNumberGetValue(PDFCosHandle CosObject);


    /* Description
       Creates new Cos object and copies all data from source Cos
       \object excluding indirect information.
       Parameters
       PDFDoc    :  [in] Document for which need create new cos object.
       CosObject :  [in] The Cos object from which information will be
                    received for copy.
       Returns
       New Cos object which has all infomation from source Cos
       \object.                                                        */    
    LIB_API PDFCosHandle CosCopy(PDFDocHandle Doc, PDFCosHandle CosObject);


    /* Description
       Gets the indirect Cos object from document.
       Parameters
       Doc :  [in] The document from which Cos object will be loaded.
       ID :   [in] The index of the indirect Cos object which is to be
              returned.
       Returns
       Either Cos object or the null object returns if there is no
       \object with this ID.                                           */ 
    LIB_API PDFCosHandle CosGetFromDoc(PDFDocHandle Doc, ppUns32 ID);


    /* Description
       Tests object if it's indirect or direct.
       Parameters
       CosObject :  [in] The object to test.
       Returns
       true if Cos Object is indirect, false if Cos Object is
       direct.
       See Also
       CosGetID CosGetGeneration                        */
    LIB_API ppBool CosIsIndirect(PDFCosHandle CosObject);



    /* Description
       Gets the index for an indirect object.
       Parameters
       CosObject :  [in] The indirect CosObj for which the ID is obtained. A
                    CosObj can be determined as indirect using
                    CosIsIndirect function.
       Returns
       The ID of CosObj.
       See Also
       CosIsIndirect CosGetGeneration                                  */
    LIB_API ppUns32 CosGetID( PDFCosHandle CosObject);



    /* Description
       Gets the generation number of an indirect Cos object.
       Parameters
       CosObject :  [in] The indirect CosObj for which the generation number
                    is obtained. A CosObj can be determined as indirect
                    using CosIsIndirect function.
       Returns
       The generation number of CosObj.
       See Also
       CosIsIndirect CosGetID                                          */
    LIB_API ppUns16 CosGetGeneration( PDFCosHandle CosObject);


    


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSCosA_H */
