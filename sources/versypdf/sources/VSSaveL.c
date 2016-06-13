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
VSSaveL.c
*********************************************************************/


#include "VSDocAI.h"
#include "VSMisc.h"
#include "VSAssert.h"
#include "VSCrypt.h"
#include "VSCosAI.h"
#include "VSFilter.h" 

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD ( Doc )
#define _LIB _DOC->Lib


void PDFDocMarkSharedThumbnail(PDFDocHandle Doc)
{
    ppUns32 i;
    PDFID id;
    PDFCosHandle page, th;
    if ( !( _DOC->Pages ) )
        return;
    if ( !( _DOC->Pages->Size ) )
        return;
    for ( i = 1; i < _DOC->Size; i++ )
        _DOC->Entries[i].SaveInfo = 0;
    for ( i = 0; i < _DOC->Pages->Size; i++ ){
        page = _DocPage(Doc,i)->PO;
        th = _CosDictValueByNameNRF ( page, Thumb );
        if ( _CosGetType ( th ) != CosRef )
            continue;
        id = _CosRefValue ( th );
        _DOC->Entries[id.ID].SaveInfo++;
    }
}
/********************************************************************
Description: 
    Before linearization check validity of the beads in the document.
Parameters:
    Doc         - [in]  The document in which need check beads.
Returns: 
    None.
*********************************************************************/
void PDFDocCheckPageBeads(PDFDocHandle Doc)
{
    PDFCosHandle Page, Threads, wrk, first, curr, B, Thread;
    ppUns32 i, j;
    Threads = _CosDictValueByName ( _DOC->Root, Threads );
    if ( !_IsCosArray( Threads ) )
        return;
    for ( i = 0; i < _CosArrayCount ( Threads ); i++ ){
        Thread = CosArrayItem ( Threads, i );
        if ( !_CosIndirectDict( Thread ) )
            continue;
        curr = _CosDictValueByName ( Thread, F );
        if ( !_CosIndirectDict( curr ) )
            continue;
        first = curr;
        while ( true ){
            wrk = _CosDictValueByName ( curr, T );
            if ( !_CosIndirectDict( wrk ) || CO(wrk)!=CO(Thread) )
                _CosDictAppend ( curr, T, Thread);
            Page = _CosDictValueByName ( curr, P );
            if (_CosIndirectDict( Page )){
                B=_CosDictValueByName(Page, B);
                if (!_IsCosArray(B))
                    _CosDictAppend(Page,B,B=CosArrayNew(Doc,false,5));
                for (j=0;j<_CosArrayCount(B);j++){
                    wrk=_CosArrayItem (B, j);
                    if (_IsCosRef(wrk)){
                        if (_CosObjID(curr)== _CosRefValue(wrk).ID && 
                            _CosObjGeneration(curr)== _CosRefValue(wrk).GenID)
                            break;
                    }
                }
                if (j==_CosArrayCount(B))
                    CosArrayAppend(B,curr);
            }        
            curr = _CosDictValueByName ( curr, N );
            if (!_CosIndirectDict( curr )) break;       
            if ( CO(curr) == CO(first) )
                break;
        }
    }
}

/********************************************************************
Description: 
    Mark value of the dictionary as used if this value is indirect object.
Parameters:
    Doc         - [in]  The document in enty stored.
    obj         - [in]  The dictionary where need search key
    key         - [in]  The key for which need receive value
    counter     - [in]  The counter of the markers
Returns: 
    None.
*********************************************************************/

void PDFDocMarkOneEntry(PDFDocHandle Doc, PDFCosHandle obj, ppAtom key,
                        ppUns32 *counter)
{
    PDFCosHandle h;
    PDFID id;
    if ( !_IsCosDict ( obj ) )
        return;
    h = CosDictValueByNameNRF ( obj, key );
    if ( !_IsCosRef( h ) )
        return;
    id = _CosRefValue ( h );
    if ( id.ID > 0 && id.ID < _DOC->Size && !_DOC->Entries[id.ID].Additional)
        _DOC->Entries[id.ID].Additional = ( *counter )++;
}


void PDFDocMarkDict(PDFDocHandle Doc, PDFCosHandle obj, ppUns32 *counter,
                    ppBool Exclude, ppAtom *list, ppInt32 ListSize,
                    ppUns32 Index)
{
    ppUns32 i;
	ppInt32 j;
    PDFID WID;
    PDFCosHandle wrk;
    ppAtom wc;
    switch ( _CosGetType ( obj ) ){
        case CosRef:
            WID = _CosRefValue(obj);
			if (WID.ID<=0 || WID.ID >= _DOC->Size) 
				return;
            if ( !_DOC->Entries[WID.ID].Additional ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                wrk = CosGetFromDoc ( Doc, WID.ID );
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkDict ( Doc, wrk, counter, Exclude, list,
                                     ListSize, Index );
            } else if ( Index ){
                if ( _DOC->Entries[WID.ID].SaveInfo != Index ) {
                    wrk = CosGetFromDoc ( Doc, WID.ID );
                    if ( _IsCosInclude ( wrk ) ){
                        _DOC->Entries[WID.ID].SaveInfo = Index ;
                        PDFDocMarkDict ( Doc, wrk, counter, Exclude, list,
                        ListSize, Index );
                    }
                }              
            }
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount ( obj ); i++ )
                if ( _IsCosIncludeR ( _CosArrayItem( obj, i ) ) )
                    PDFDocMarkDict ( Doc, _CosArrayItem(obj, i ), counter,
                                     Exclude, list, ListSize, Index );
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                wc = _CosDictKey(obj,i);
                if ( Exclude ){
                    for ( j = 0; j < ListSize; j++ ){
                        if ( wc == list[j] )
                            break;
                    }
                    if ( j == ListSize )
                        if ( _IsCosIncludeR ( _CosDictValue ( obj,i ) ) )
                            PDFDocMarkDict ( Doc, _CosDictValue ( obj,i ),
                                counter, Exclude, list, ListSize, Index );
                } else
                    for ( j = 0; j < ListSize; j++ )
                        if ( wc == list[j] ){
                            if ( _IsCosIncludeR ( _CosDictValue ( obj,i ) ) )
                                PDFDocMarkDict ( Doc,_CosDictValue ( obj,i ),
                                    counter, Exclude, list, ListSize, Index );
                            break;
                        };
            }
            break;
        case CosStream:
            obj = _CosStreamAttr(obj);
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                wc = _CosDictKey(obj,i);
                if ( Exclude ){
                    for ( j = 0; j < ListSize; j++ ){
                        if ( wc == list[j] )
                            break;
                    }
                    if ( j == ListSize )
                        if ( _IsCosIncludeR ( _CosDictValue ( obj,i ) ) )
                            PDFDocMarkDict ( Doc, _CosDictValue ( obj,i ),
                            counter, Exclude, list, ListSize, Index );
                } else
                    for ( j = 0; j < ListSize; j++ )
                        if ( wc == list[j] ){
                            if ( _IsCosIncludeR ( _CosDictValue ( obj,i ) ) )
                                PDFDocMarkDict ( Doc,_CosDictValue ( obj,i ),
                                counter, Exclude, list, ListSize, Index );
                            break;
                        };
            }
            break;
        default:
        ;
    }
}

void PDFDocMarkDictWithCheck(PDFDocHandle Doc, PDFCosHandle obj, ppUns32 *counter,
                             ppBool Exclude, ppAtom *list, ppInt32 ListSize,
                             ppAtom *typelist, ppInt32 tls)
{
    ppUns32 i;
	ppInt32 j;
    PDFID WID;
    PDFCosHandle wrk, d;
    ppAtom wc;
    switch ( _CosGetType ( obj ) ){
        case CosRef:
            WID = _CosRefValue(obj);
			if (WID.ID<=0 || WID.ID >= _DOC->Size) 
				return;
            if ( _DOC->Entries[WID.ID].Additional )
                return;
            wrk = CosGetFromDoc ( Doc, WID.ID );
            if ( !_IsCosDict ( wrk ) ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkDictWithCheck ( Doc, wrk, counter, Exclude,
                                              list, ListSize, typelist, tls );
                return;
            }
            d = _CosDictValueByName ( wrk, Type );
            if ( !_IsCosName ( d ) ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkDictWithCheck ( Doc, wrk, counter, Exclude,
                        list, ListSize, typelist, tls );
                return;
            }
            wc = _CosNameValue ( d );
            for ( j = 0; j < tls; j++ )
                if ( wc == typelist[j] )
                    break;
            if ( j == tls ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkDictWithCheck ( Doc, wrk, counter, Exclude,
                        list, ListSize, typelist, tls );
            }
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount ( obj ); i++ )
                if ( _IsCosIncludeR ( _CosArrayItem(obj,i) ) )
                    PDFDocMarkDictWithCheck ( Doc, _CosArrayItem(obj,i),
                        counter, Exclude, list, ListSize, typelist, tls );
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                wc = _CosDictKey (obj, i);
                if ( Exclude ){
                    for ( j = 0; j < ListSize; j++ ){
                        if ( wc == list[j] )
                            break;
                    }
                    if ( j == ListSize )
                        if ( _IsCosIncludeR ( _CosDictValue(obj,i ) ) )
                            PDFDocMarkDictWithCheck ( Doc,_CosDictValue(obj,i ),
                                counter, Exclude, list, ListSize, typelist, tls );
                } else
                    for ( j = 0; j < ListSize; j++ )
                        if ( wc == list[j] ){
                            if ( _IsCosIncludeR ( _CosDictValue(obj,i ) ) )
                                PDFDocMarkDictWithCheck ( Doc,_CosDictValue(obj,i ),
                                    counter, Exclude, list, ListSize, typelist, tls );
                            break;
                        };
            }
            break;
        case CosStream:
            obj = _CosStreamAttr ( obj );
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                wc = _CosDictKey (obj, i);
                if ( Exclude ){
                    for ( j = 0; j < ListSize; j++ ){
                        if ( wc == list[j] )
                            break;
                    }
                    if ( j == ListSize )
                        if ( _IsCosIncludeR ( _CosDictValue(obj,i ) ) )
                            PDFDocMarkDictWithCheck ( Doc,_CosDictValue(obj,i ),
                            counter, Exclude, list, ListSize, typelist, tls );
                } else
                    for ( j = 0; j < ListSize; j++ )
                        if ( wc == list[j] ){
                            if ( _IsCosIncludeR ( _CosDictValue(obj,i ) ) )
                                PDFDocMarkDictWithCheck ( Doc,_CosDictValue(obj,i ),
                                counter, Exclude, list, ListSize, typelist, tls );
                            break;
                        };
            }
            break;
        default:

        ;
    }
}

void MarkSharedObjectForPage(PDFDocHandle Doc, PDFCosHandle obj, plinpageaddinfo pageinfo,
                             ppBool Check, ppUns32 Index)
{
    PDFCosHandle wrk, obj1;
    ppUns32 i;
    PDFID WID;
    switch ( _CosGetType ( obj ) ){
        case CosRef:
            WID = _CosRefValue ( obj );
			if (WID.ID<=0 || WID.ID >= _DOC->Size) 
				return;
            if ( _DOC->Entries[WID.ID].Additional == Index )
                return;
            _DOC->Entries[WID.ID].Additional = Index;
            wrk = CosGetFromDoc ( Doc, WID.ID);
            if ( Check ){
                if ( _IsCosDict ( wrk ) ){
                    obj1 = _CosDictValueByName ( wrk, Type );
                    if ( _IsCosName( obj1 ) && (_CosNameValue(obj1)==GetAtomDoc(Page) || 
                        _CosNameValue(obj1)==GetAtomDoc(Annots)))
                        return;
                }
            }
            for ( i = 0; i < pageinfo->SharedObjectsCount; i++ ){
                if ( pageinfo->SharedObjectList[i] == WID.ID )
                    break;
            }
            if ( i == pageinfo->SharedObjectsCount ){
                pageinfo->SharedObjectList[pageinfo->SharedObjectsCount++] = WID.ID;
                if ( _IsCosInclude ( wrk ) )
                    MarkSharedObjectForPage ( Doc, wrk, pageinfo, Check, Index );
            }
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount ( obj ); i++ ){
                if ( _IsCosIncludeR ( _CosArrayItem(obj,i) ) )
                    MarkSharedObjectForPage ( Doc, _CosArrayItem(obj,i), pageinfo, Check, Index );
            }
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                if ( _IsCosIncludeR ( _CosDictValue(obj,i) ) )
                    MarkSharedObjectForPage ( Doc,_CosDictValue(obj,i), pageinfo, Check, Index );
            }
            break;
        case CosStream:
            obj = _CosStreamAttr ( obj );
            for ( i = 0; i < _CosDictCount ( obj ); i++ ){
                if ( _IsCosIncludeR ( _CosDictValue(obj,i) ) )
                    MarkSharedObjectForPage ( Doc,_CosDictValue(obj,i), pageinfo, Check, Index );
            }
            break;
        default:

        ;
    }
}

void PDFDocFindResourcesForMark(PDFDocHandle Doc, PDFCosHandle obj, plinpageaddinfo pageinfo, 
                                ppUns32 Index)
{
    PDFCosHandle wrk, obj1, res;
    ppUns32 i, j;
    PDFID WID;
    ppAtom wc;
    switch ( _CosGetType ( obj ) ){
        case CosRef:
            WID = _CosRefValue ( obj );
			if (WID.ID<=0 || WID.ID >= _DOC->Size) 
				return;
            if ( _DOC->Entries[WID.ID].Additional == Index )
                return;
            _DOC->Entries[WID.ID].Additional = Index;
            wrk = CosGetFromDoc ( Doc, WID.ID);
            if ( _IsCosDict ( wrk ) ){
                obj1 = _CosDictValueByName ( wrk, Type );
                if ( _IsCosName( obj1 ) && (_CosNameValue(obj1)==GetAtomDoc(Page) || 
                    _CosNameValue(obj1)==GetAtomDoc(Annots)))
                    return;            }
            if ( _IsCosInclude ( wrk ) )
                PDFDocFindResourcesForMark ( Doc, wrk, pageinfo, Index );
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount ( obj ); i++ ){
                if ( _IsCosIncludeR ( _CosArrayItem(obj,i) ) )
                    PDFDocFindResourcesForMark ( Doc, _CosArrayItem(obj,i), pageinfo, Index );
            }
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount(obj); i++ ){
                wc = _CosDictKey(obj,i);
                if ( wc == GetAtomDoc(Resources) ){
                    res = _CosDictValue(obj,i);
                    if ( _IsCosRef ( res ) ){
                        WID = _CosRefValue ( res );
                        res = CosGetFromDoc ( Doc, WID.ID);
                    }
                    if ( _IsCosDict ( res )  )
                        for ( j = 0; j < _CosDictCount ( res ); j++ ){
                            wrk = _CosDictValue(res,j);
                            if ( _IsCosRef ( wrk ) ){
                                WID = _CosRefValue ( wrk );
                                wrk = CosGetFromDoc ( Doc, WID.ID );
                            };
                            if ( !_IsCosDict ( wrk ) )
                                continue;
                            MarkSharedObjectForPage ( Doc, wrk, pageinfo, false, Index );
                        }
                } else if ( _IsCosIncludeR ( _CosDictValue(obj,i) ) )
                    PDFDocFindResourcesForMark ( Doc,_CosDictValue(obj,i), pageinfo, Index );
            }
            break;
        case CosStream:
            obj = _CosStreamAttr ( obj );
            for ( i = 0; i < _CosDictCount(obj); i++ ){
                wc = _CosDictKey(obj,i);
                if ( wc == GetAtomDoc(Resources) ){
                    res = _CosDictValue(obj,i);
                    if ( _IsCosRef ( res ) ){
                        WID = _CosRefValue ( res );
                        res = CosGetFromDoc ( Doc, WID.ID);
                    }
                    if ( _IsCosDict ( res )  )
                        for ( j = 0; j < _CosDictCount ( res ); j++ ){
                            wrk = _CosDictValue(res,j);
                            if ( _IsCosRef ( wrk ) ){
                                WID = _CosRefValue ( wrk );
                                wrk = CosGetFromDoc ( Doc, WID.ID );
                            };
                            if ( !_IsCosDict ( wrk ) )
                                continue;
                            MarkSharedObjectForPage ( Doc, wrk, pageinfo, false, Index );
                        }
                } else if ( _IsCosIncludeR ( _CosDictValue(obj,i) ) )
                    PDFDocFindResourcesForMark ( Doc,_CosDictValue(obj,i), pageinfo, Index );
            }
            break;
        default:
        ;
    }
}

void PDFDocGetSharedObjectPerPage(PDFDocHandle Doc, ppInt32 Index, plinpageaddinfo pageinfo,
                                  ppInt32 totalsharedcount)
{
    ppUns32 i;
    PDFCosHandle page, res, wrk, obj;
    PDFID ID;
    if ( !totalsharedcount )
        return;
    pageinfo[Index].SharedObjectList = ( ppUns32 *) mmalloc ( _LIB, totalsharedcount * sizeof ( ppUns32 ) );
    pageinfo[Index].SharedObjectsCount = 0;
    page = ( ( PPDFPageInfo ) _DOC->Pages->FList[Index] )->PO;
    res = _CosDictValueByName ( page, Resources );
    if ( _IsCosDict ( res ) )
        for ( i = 0; i < _CosDictCount ( res ); i++ ){
            wrk = _CosDictValue( res, i );
            if ( _IsCosRef ( wrk )){
                ID = _CosRefValue ( wrk );
                wrk = CosGetFromDoc ( Doc, ID.ID );
            };
            if ( !_IsCosDict ( wrk ) )
                continue;
            MarkSharedObjectForPage ( Doc, wrk, pageinfo + Index,false, Index );
        }
    obj = _CosDictValueByName ( page, Annots );
    PDFDocFindResourcesForMark ( Doc, obj, pageinfo + Index, Index );
    pageinfo[Index].SharedObjectList = ( ppUns32 * ) mrealloc ( _LIB,
                                                  pageinfo[Index].SharedObjectList,
                                                  pageinfo[Index].SharedObjectsCount * sizeof ( ppUns32 ) );
}


void PDFDocMarkAnnotsOnPageWithoutResources(PDFDocHandle Doc, PDFCosHandle Annot,
                                            ppUns32 *counter, ppAtom *Exclude, ppInt32 EL)
{
    ppUns32 i, j;
	ppInt32 is;
    PDFID WID;
    PDFCosHandle wrk, d, res;
    ppAtom wc;
    if ( _IsCosNull(Annot) )
        return;
    switch ( _CosGetType ( Annot ) ){
        case CosRef:
            WID = _CosRefValue (Annot);
			if (WID.ID<=0 || WID.ID >= _DOC->Size) 
				return;
            if ( _DOC->Entries[WID.ID].Additional )
                return;
            wrk = CosGetFromDoc ( Doc, WID.ID);
            if ( !_IsCosDict ( wrk ) ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc, wrk, counter, Exclude, EL );
                return;
            }
            d = _CosDictValueByName ( wrk, Type );
            if ( !_IsCosName( d ) ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc, wrk, counter, Exclude, EL );
                return;
            }
            wc = _CosNameValue ( d );
            for ( is = 2; is < EL; is++ )
                if ( wc == Exclude[is] )
                    break;
            if ( is == EL ){
                _DOC->Entries[WID.ID].Additional = ( *counter )++;
                if ( _IsCosInclude ( wrk ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc, wrk, counter, Exclude, EL );
            }
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount(Annot); i++ )
                if ( _IsCosIncludeR ( _CosArrayItem(Annot,i) ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc,_CosArrayItem(Annot,i),
                        counter, Exclude, EL );
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount( Annot ); i++ ){
                wc = _CosDictKey( Annot, i );
                if ( wc == GetAtomDoc (Resources) ){
                    res = _CosDictValue( Annot, i );
                    if ( _IsCosRef( res ) ){
                        WID = _CosRefValue ( res );
                        res = CosGetFromDoc ( Doc, WID.ID );
                    }
                    if ( _IsCosDict ( res ) )
                        for ( j = 0; j < _CosDictCount ( res ); j++ ){
                            wrk = _CosDictValue(res,j);
                            if ( _IsCosRef ( wrk ) ){
                                WID = _CosRefValue ( wrk );
                                if ( WID.ID > 0 && WID.ID < _DOC->Size )
                                    _DOC->Entries[WID.ID].Additional = ( *counter )++;
                            }
                        }
                } else if ( _IsCosIncludeR ( _CosDictValue(Annot,i) ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc,_CosDictValue(Annot,i),
                        counter, Exclude, EL );
            }
            break;
        case CosStream:
            Annot = _CosStreamAttr(Annot);
            for ( i = 0; i < _CosDictCount( Annot ); i++ ){
                wc = _CosDictKey( Annot, i );
                if ( wc == GetAtomDoc (Resources) ){
                    res = _CosDictValue( Annot, i );
                    if ( _IsCosRef( res ) ){
                        WID = _CosRefValue ( res );
                        res = CosGetFromDoc ( Doc, WID.ID );
                    }
                    if ( _IsCosDict ( res ) )
                        for ( j = 0; j < _CosDictCount ( res ); j++ ){
                            wrk = _CosDictValue(res,j);
                            if ( _IsCosRef ( wrk ) ){
                                WID = _CosRefValue ( wrk );
                                if ( WID.ID > 0 && WID.ID < _DOC->Size )
                                    _DOC->Entries[WID.ID].Additional = ( *counter )++;
                            }
                        }
                } else if ( _IsCosIncludeR ( _CosDictValue(Annot,i) ) )
                    PDFDocMarkAnnotsOnPageWithoutResources ( Doc,_CosDictValue(Annot,i),
                    counter, Exclude, EL );
            }            
            break;
        default:

        ;
    }
}




void PDFDocSavePackedLinearized(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    PDFDocSaveUnPackedLinearized ( Doc, Strm );
}

void PDFDocSaveUnPackedLinearized(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    PDFCosHandle wrk, hintarray, Linearized, cr, hs, page, res, obj,
              FirstTrailer , ar, z;
    PDFStreamHandle TMPStrm, sharedobjecthintstream, hstr;
    ULBitStream bs, sharedobjectbithintstream;
    PDFID ID;
    ppUns32 hh, fpa,  ps, minPgObj, maxPgObj, bitsperpage, minPgSize,
            maxPgSize, bitsperpagesize, bsz, bitssharedref, bitsoffset;
    ppUns32 i, en, MainHeaderSize, fsec, tsec, j, rescount, resstart,
            mi, ma, b, zz, fullsize;
	ppUns32 a, pgs, k, hit;
    ppBool outlin;
    TPDFCryptoType ncr, *UsedCrypto;
    ppBool crypting = false, delID = false;
    char str[128];
    plinpageaddinfo pageinfo;
    char *pstr;
    ppAtom Threads[2], AnnotExclude[1], Beads[1], PagesExclude[2],
           Thumbnail[3], PagesInclude[1], ThreadsInclude[1], outlinDict[5],
           resDictWithoutFontFile[3], ResExcl[2]; 
    Threads[0] = GetAtomDoc ( F );
    Threads[1] = ThreadsInclude[0] = GetAtomDoc ( I );
    AnnotExclude[0] = PagesExclude[1] = ResExcl[0] = GetAtomDoc ( Page );
    Beads[0] = GetAtomDoc ( P );
    PagesExclude[1] = PagesInclude[0] = ResExcl[1] = GetAtomDoc ( Pages );
    Thumbnail[0] = GetAtomDoc ( Thumb );
    Thumbnail[1] = GetAtomDoc ( Parent );
    Thumbnail[2] = GetAtomDoc ( Resources );
    outlinDict[0] = GetAtomDoc ( First );
    outlinDict[1] = GetAtomDoc ( Last );
    outlinDict[2] = GetAtomDoc ( Next );
    outlinDict[3] = GetAtomDoc ( Prev );
    outlinDict[4] = GetAtomDoc ( A );
    resDictWithoutFontFile[0] = GetAtomDoc ( FontFile );
    resDictWithoutFontFile[1] = GetAtomDoc ( FontFile2 );
    resDictWithoutFontFile[2] = GetAtomDoc ( FontFile3 );


	/* Remove all unused objects and cut on last */
    PDFDocRemoveUnUsed ( Doc, NULL );
    PDFDocPackFree ( Doc );
    PDFDocCheckPageBeads ( Doc );
    _CosNullNew(Doc, ncr.FileID);
    _CosNullNew(Doc, FirstTrailer);
    
	if ( !_DOC->Pages )
		PDFDocLoadPages ( Doc );
	if ( !_DOC->Pages->Size )
		_RAISE ( ErrFileLevel, fleDocumentNotContainPagesError );

    /*  modify all inherited properties for pages  */
    PDFDocRemoveInheritedFromPages ( Doc );

    /*UnMark all objects*/
    for ( i = 0; i < _DOC->Size; i++ ){
        _DOC->Entries[i].Additional = 0;
        _DOC->Entries[i].SaveInfo = 0;
    }

    /* Create Linearize dictionary*/
    Linearized = CosDictNew ( Doc, true, 7 );
    _CosDictAppend ( Linearized, Linearized, CosIntNew ( Doc, false, 1 ) );
    _CosDictAppend ( Linearized, H, hintarray = CosArrayNew ( Doc, false, 2 ) );
    CosArrayAppend ( hintarray, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    CosArrayAppend ( hintarray, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    _CosDictAppend ( Linearized, L, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    _CosDictAppend ( Linearized, O, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    _CosDictAppend ( Linearized, E, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    _CosDictAppend ( Linearized, T, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    _CosDictAppend ( Linearized, N, CosIntNew ( Doc, false, _DOC->Pages->Size ) );

    /* If need Create Encryption dictionary */
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
	if ( crypting ){
		cr = ULPrepareCryptoDictionary (Doc, UsedCrypto );
	};
	if (((CosDoc)Doc)->Version >= 9){
		_CosDictAppend(_DOC->Root,Extensions,obj = CosDictNew(Doc,false,1));
		_CosDictAppend(obj,ADBE,wrk = CosDictNew(Doc,false,2));
		_CosDictAppend(wrk,BaseVersion,CosNameNew(Doc, false,ULStringToAtom(_LIB,"1.7")));
		_CosDictAppend(wrk,ExtensionLevel, CosIntNew(Doc,false,3));
	}
	if (((CosDoc)Doc)->Version >= 9){
		_CosDictAppend(_DOC->Root,Extensions,obj = CosDictNew(Doc,false,1));
		_CosDictAppend(obj,ADBE,wrk = CosDictNew(Doc,false,2));
		_CosDictAppend(wrk,BaseVersion,CosNameNew(Doc, false,ULStringToAtom(_LIB,"1.7")));
		_CosDictAppend(wrk,ExtensionLevel, CosIntNew(Doc,false,3));
	}
    /************************************************************************/
    /* Document Catalog and Document-Level Objects                          */
    /************************************************************************/

    a = 1;
    /* Append liniarization dictionary*/
    _DOC->Entries[_CosObjID(Linearized)].Additional = a++;

    /* Append root */
    _DOC->Entries[_CosObjID ( _DOC->Root )].Additional = a++;

    /*append VewerPreferences*/
    PDFDocMarkOneEntry ( Doc, _DOC->Root, GetAtomDoc ( ViewerPreferences ), &a );
    
    /* Append PageMode*/
    PDFDocMarkOneEntry ( Doc, _DOC->Root, GetAtomDoc ( PageMode ), &a );

    wrk = _CosDictValueByName ( _DOC->Root, PageMode );
    outlin = false;
    if ( _IsCosNameValue ( wrk, GetAtomDoc ( UseOutlines )) )
        outlin = true;

    /*  append threads  */
    PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Threads ), &a, true,
                     Threads, 2, 0 );
    /*  append Open Actions  */
    PDFDocMarkOneEntry ( Doc, _DOC->Root, GetAtomDoc ( OpenAction ), &a );
    /*  append AcroForm  */
    PDFDocMarkOneEntry ( Doc, _DOC->Root, GetAtomDoc ( AcroForm ), &a );
    /* append Encryption*/
    if ( crypting ){
        en = a;
        _DOC->Entries[_CosObjID ( cr )].Additional = a++;
    }

    /************************************************************************/
    /* Hint Stream Section                                                  */
    /************************************************************************/
    /*  Append Hint Stream  */
    hs = CosStreamNew ( Doc, true, 3 );
    hit = a;
    _DOC->Entries[_CosObjID ( hs )].Additional = a++;

    pageinfo = ( linpageaddinfo *) mmalloc ( _LIB, sizeof ( linpageaddinfo ) * _DOC->Pages->Size );

    /************************************************************************/
    /* First Page Section                                                   */
    /************************************************************************/
    ps = a;
    /*  Append First Page  */
    page = ((PPDFPageInfo)_DOC->Pages->FList[0])->PO;
    _DOC->Entries[_CosObjID(page)].Additional = a++;
    /*  Append outlines   */
    if ( outlin )
        PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Outlines ), &a,
                         false, outlinDict, 5, 0 );
    /*  Append all object that the page object refer to  */
    /*Append Annotations*/
    PDFDocMarkDictWithCheck ( Doc, _CosDictValueByNameNRF ( page, Annots ),
                              &a, true, NULL, 0, AnnotExclude, 1 );
    /*Append Beads*/
    PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( page, B ), &a, true, Beads,
                     1, 0 );
    /* Append Resources*/
    res = _CosDictValueByNameNRF ( page, Resources );
    PDFDocMarkDict ( Doc, res, &a, true, resDictWithoutFontFile, 3, 0 );
    PDFDocMarkDictWithCheck ( Doc, page, &a, true, Thumbnail, 3, PagesExclude,
                              2 );
    PDFDocMarkDict ( Doc, res, &a, true, NULL, 0, a );
    fpa = a - 1;
    ps = a - ps;
    minPgObj = ps;
    maxPgObj = ps;
    pageinfo[0].ObjectCount = ps;
    /*  First Page Section Finished  */

    /*Other Pages Sections*/
    for ( pgs = 1; pgs < _DOC->Pages->Size; pgs++ ){
        page = ( ( PPDFPageInfo ) _DOC->Pages->FList[pgs] )->PO;
        ps = a;
        if ( !_DOC->Entries[_CosObjID(page)].Additional )
            _DOC->Entries[_CosObjID(page)].Additional = a++;
        /*  Append all object that the page object refer to  */
        
        /*Append Beads*/
        PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( page, B ), &a, true,
                         Beads, 1, 0 );
        /*Append Annotations*/
        PDFDocMarkOneEntry ( Doc, page, GetAtomDoc ( Annots ), &a );
        obj = _CosDictValueByName ( page, Annots );
        PDFDocMarkAnnotsOnPageWithoutResources ( Doc, obj, &a, ResExcl, 2 );
        /* Append Resources*/
        PDFDocMarkOneEntry ( Doc, page, GetAtomDoc ( Resources ), &a );
        res = _CosDictValueByName ( page, Resources );
        if ( _CosGetType ( res ) == CosDict )
            for ( i = 0; i < _CosDictCount ( res ); i++ ){
                wrk = _CosDictValue( res, i );
                if ( _IsCosRef( wrk ) ){
                    ID = _CosRefValue ( wrk );
                    if ( ID.ID > 0 && ID.ID < _DOC->Size )
						if ( !_DOC->Entries[ID.ID].Additional )
							_DOC->Entries[ID.ID].Additional = a++;
                }
            }

        /*Other objects*/
        PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( page, Contents ), &a,
                         true, NULL, 0, 0 );
        PDFDocMarkDictWithCheck ( Doc, page, &a, true, Thumbnail, 3,
                                  PagesExclude, 2 );
        pageinfo[pgs].ObjectCount = ps = a - ps;
        if ( ps < minPgObj )
            minPgObj = ps;
        if ( ps > maxPgObj )
            maxPgObj = ps;
    }

    /************************************************************************/
    /* Shared Object Section                                                */
    /************************************************************************/
    resstart = a;
    for ( pgs = 1; pgs < _DOC->Pages->Size; pgs++ ){
        page = _DocPage(Doc,pgs)->PO;
        PDFDocMarkDict ( Doc, _CosDictValueByName ( page, Resources ), &a, true, NULL, 0, a );
        obj = _CosDictValueByName ( page, Annots );
        if ( _CosGetType ( obj ) == CosArray ){
            j = _CosArrayCount ( obj );
            for ( i = 0; i < j; i++ )
                PDFDocMarkDictWithCheck ( Doc, _CosArrayItem ( obj, i ), &a, true, NULL, 0, AnnotExclude, 1 );
        }
    }
    rescount = a - resstart;
    /************************************************************************/
    /*  Other Objects Section                                               */
    /************************************************************************/
    /* Pages Tree List*/
    PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Pages ), &a, false,
                     PagesInclude, 1, 0 );
    /* Thumbnail Images*/
    PDFDocMarkSharedThumbnail ( Doc );
    for ( pgs = 1; pgs < _DOC->Pages->Size; pgs++ ){
        page = _DocPage(Doc,pgs)->PO;
        PDFDocMarkDict ( Doc, _CosDictValueByName ( page, Thumb ), &a, true,
                         NULL, 0, 0 );
    }
    /* Thumbnail Shared Images */
    for ( pgs = 1; pgs < _DOC->Pages->Size; pgs++ ){
        page = _DocPage(Doc,pgs)->PO;
        PDFDocMarkDict ( Doc, _CosDictValueByName ( page, Thumb ), &a, true,
                         NULL, 0, a );
    }
    /*Outlines*/
    if ( !outlin )
        PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Outlines ), &a,
                         false, outlinDict, 5, 0 );
    /* Threads information*/
    PDFDocMarkDict ( Doc, _CosDictValueByName ( _DOC->Root, Threads ), &a, false,
                     ThreadsInclude, 1, 0 );
    /* Named Destionations*/
    PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Dests ), &a, true,
                     NULL, 0, 0 );
    PDFDocMarkDict ( Doc, _CosDictValueByNameNRF ( _DOC->Root, Names ), &a, true,
                     NULL, 0, 0 );
    /* Information Dictionary*/
    if ( !_IsCosNull(_DOC->Info ))
        if ( !_DOC->Entries[_CosObjID ( _DOC->Info )].Additional )
            _DOC->Entries[_CosObjID ( _DOC->Info )].Additional = a++;
    /* Acroform Hierarchy */
    PDFDocMarkDict ( Doc, _CosDictValueByName ( _DOC->Root, AcroForm ), &a, true,
                     NULL, 0, false );
    /* Other Objects*/
    for ( i = 1; i < _DOC->Size; i++ ){
        if ( _DOC->Entries[i].Used != etFree ){
            if ( !_DOC->Entries[i].Additional )
                _DOC->Entries[i].Additional = a++;
        } else
            _DOC->Entries[i].Additional = 0x7FFFFFFF;
    }
    /* At now we have count*/
    a--;
    k = hh = a - fpa;
    hit += k;
    if ( crypting )
        en += k;

    resstart -= fpa;
/*	
	printf("-------------------------------------------------\n");
	for ( i = 1; i < _DOC->Size; i++ ){
		printf("%i    %i\n", i, _DOC->Entries[i].Additional);		
	}
	printf("-------------------------------------------------\n");
*/

    for ( i = 1; i < _DOC->Size; i++ ){
        if ( _DOC->Entries[i].Used != etFree ){
            if ( _DOC->Entries[i].Additional > fpa )
                _DOC->Entries[i].Additional -= fpa;
            else
                _DOC->Entries[i].Additional += k;
        }
    };

    PDFDocRenumAllObjects ( Doc );
    PDFDocPackFree (Doc);

    /************************************************************************/
    /* Store PDF objects ToFile                                             */
    /************************************************************************/


    FirstTrailer = CosDictNew ( Doc, false, 4 );
    _CosDictAppend ( FirstTrailer, Size, CosIntNew ( Doc, false, _DOC->Size ) );
    _CosDictAppend ( FirstTrailer, Root, _DOC->Root );
    _CosDictAppend ( FirstTrailer, Info, _DOC->Info );
    _CosDictAppend ( FirstTrailer, Prev, CosIntNew ( Doc, false, 0x7FFFFFFF ) );
    if ( crypting )
        _CosDictAppend ( FirstTrailer, Encrypt, CosNewRef ( Doc, en, 0 ) );
    ar = CosArrayNew ( Doc, false, 2 );
    if ( crypting )
        z = CosCopy ( Doc, UsedCrypto->FileID );
    else
        z = ULCreateFileID ( Doc, PDFEngine );
    CosArrayAppend ( ar, z );
    CosArrayAppend ( ar, CosCopy ( Doc, z ) );
    _CosDictAppend ( FirstTrailer, ID, ar );

    MainHeaderSize = ( 
                        fsec = CosGetObjSize ( Linearized )         +   /* Size of Linearized object */ 
                                17                                  +   /* Size of header of PDF Document ("%PDF-1.z\r\n%ШВАМ\r\n")*/ 
                                ULGetIntSize ( k  + 1 )             +   /* Size of ID part of the Linearized Dictionary */ 
                                14 
                     ) +
                     (                   /* Other part of the Linearized Dictionary (" 0 obj\nendobj\n")  */ 
                                6                                   +   /* HREF word size ("xref\r\n")*/ 
                                9                                   +   /* Trailer word size ("trailer\r\n)*/ 
                                CosGetObjSize ( FirstTrailer )      +   /* Size of first Trailer */ 
                                23                                  +   /* Size of the end of the PDF Document ("\r\nstartxref\r\n0\r\n%%EOF\r\n") */ 
                                ULGetIntSize ( k + 1 )+ 1           + 
                                ULGetIntSize ( fpa )+ 2             +   /* XREF first record */
                                fpa * 20                                /* HREF other records */
                     );

    ULStrToStrm ( Strm, "%PDF-1." );
    ULitostr ( _DOC->Version, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, "\r\n" );
    ULStrToStrm ( Strm, "%\330\302\300\314\r\n" );
    _DOC->Entries[k + 1].Offset = ULStreamGetPosition ( Strm );

    /* Calculate size of the memory stream where will store all object need for store*/
    fullsize = 0;
    for ( i = k + 2; i <= a; i++ ){
        if ( i == hit )
            continue;
        if ( _DOC->Entries[i].Used != etFree ){
            obj = CosGetFromDoc ( Doc, i);
            if (crypting) {
                obj= CosCopy(Doc,obj);
                if ( i != en ){
                    ID.ID = i;
                    ID.GenID = 0;
                    CosCryptObject ( obj, UsedCrypto, ID, true );
                }
            }
			fullsize += ULGetIntSize ( i ) +  10 +
				( _CosGetType ( obj ) < CosName ? 5 : 4 ) + CosGetObjSize ( obj );
            if ( crypting )
                CosFree ( obj );
        };
    }
    for ( i = 1; i <= k; i++ ){
        if ( _DOC->Entries[i].Used != etFree ){
            obj = CosGetFromDoc ( Doc, i);
            if ( crypting ){
                obj= CosCopy(Doc,obj);
                ID.ID = i;
                ID.GenID = 0;
                CosCryptObject ( obj, UsedCrypto, ID, true );
            };
            fullsize += ULGetIntSize ( i ) + 10 +
                        ( _CosGetType ( obj ) < CosName ? 5 : 4 ) + CosGetObjSize ( obj );
            if ( crypting )
                CosFree ( obj );
        };
    }
    TMPStrm = ULStreamMemNew ( _LIB, fullsize );
    for ( i = k + 2; i <= a; i++ ){
        if ( i == hit )
            continue;
        if ( _DOC->Entries[i].Used != etFree ){
            obj = CosGetFromDoc ( Doc, i );
            if (crypting) {
                obj= CosCopy(Doc,obj);
                if ( i != en ){
                    ID.ID = i;
                    ID.GenID = 0;
                    CosCryptObject ( obj, UsedCrypto, ID, true );
                }
            }
            _DOC->Entries[i].Offset = ULStreamGetPosition ( TMPStrm ) + MainHeaderSize;
            ULitostr ( i, str );
            ULStrToStrm ( TMPStrm, str );
            ULStrToStrm ( TMPStrm, " 0" );
            if ( _CosGetType ( obj ) < CosName )
                ULStrToStrm ( TMPStrm, " obj " );
            else
                ULStrToStrm ( TMPStrm, " obj" );
            CosCopyObjToStream (obj, TMPStrm );
            if ( crypting )
                CosFree ( obj );
            ULStrToStrm ( TMPStrm, "\nendobj\n" );
        };
    }
    for ( i = 1; i <= k; i++ ){
        if ( _DOC->Entries[i].Used != etFree ){
            obj = CosGetFromDoc ( Doc, i);
            if ( crypting ){
                obj= CosCopy(Doc,obj);
                ID.ID = i;
                ID.GenID = 0;
                CosCryptObject ( obj, UsedCrypto, ID, true );
            };
            _DOC->Entries[i].Offset = ULStreamGetPosition ( TMPStrm ) +
                                      MainHeaderSize;
            ULitostr ( i, str );
            ULStrToStrm ( TMPStrm, str );
            ULStrToStrm ( TMPStrm, " 0" );
            if ( _CosGetType ( obj ) < CosName )
                ULStrToStrm ( TMPStrm, " obj " );
            else
                ULStrToStrm ( TMPStrm, " obj" );
            CosCopyObjToStream ( obj, TMPStrm );
            if ( crypting )
                CosFree ( obj );
            ULStrToStrm ( TMPStrm, "\nendobj\n" );
        };
    }

    for ( i = 1; i < _DOC->Size; i++ )
        _DOC->Entries[i].Additional = 0;
    for ( i = 1; i < _DOC->Pages->Size; i++ )
        PDFDocGetSharedObjectPerPage ( Doc, i, pageinfo, pageinfo[0].ObjectCount + rescount );


    sharedobjecthintstream = ULStreamMemNew ( _LIB, 0 );
    /************************************************************************/
    /* Shared Object Hint Table                                             */
    /************************************************************************/

    /*Header Section*/
    ULBitStrmInit ( sharedobjecthintstream, sharedobjectbithintstream );                        
    rescount > 0 ?  ULBitStreamPutBits ( &sharedobjectbithintstream, resstart,32 ) :
                    ULBitStreamPutBits ( &sharedobjectbithintstream, 0, 32 );                   /*Item 1*/
    rescount > 0 ?
                    ULBitStreamPutBits ( &sharedobjectbithintstream, _DOC->Entries[resstart].Offset, 32 ) :
                    ULBitStreamPutBits ( &sharedobjectbithintstream, 0, 32 );                   /*Item 2*/
    ULBitStreamPutBits ( &sharedobjectbithintstream, pageinfo[0].ObjectCount, 32 );             /*Item 3*/
    ULBitStreamPutBits ( &sharedobjectbithintstream, pageinfo[0].ObjectCount + rescount, 32 );  /*Item 4 */
    ULBitStreamPutBits ( &sharedobjectbithintstream, 0, 16 );                                   /*Item 5*/
    mi = 0xFFFF;
    ma = 0;
    for ( i = _CosObjID ( _DocPage(Doc,0)->PO ); i < _DOC->Size; i++ ){
        b = _DOC->Entries[i != _DOC->Size - 1 ? i + 1 : 1].Offset - _DOC->Entries[i].Offset;
        if ( b > ma )
            ma = b;
        if ( b < mi )
            mi = b;
    }
    for ( i = 0; i < rescount; i++ ){
        b = _DOC->Entries[i + 1 + resstart].Offset - _DOC->Entries[i + resstart].Offset;
        if ( b > ma )
            ma = b;
        if ( b < mi )
            mi = b;
    }
    ULBitStreamPutBits ( &sharedobjectbithintstream, mi, 32 );                                  /*Item 6*/
    ULBitStreamPutBits ( &sharedobjectbithintstream,
                         bsz = ULGetBitsPerInt ( ma - mi ), 16 );                               /*Item 7*/
    /*Items section*/
    for ( i = _CosObjID ( _DocPage(Doc,0)->PO ); i < _DOC->Size; i++ ){
        b = _DOC->Entries[i != _DOC->Size - 1 ? i + 1 : 1].Offset - _DOC->Entries[i].Offset;
        ULBitStreamPutBits ( &sharedobjectbithintstream, b - mi, bsz );
        ULBitStreamPutBits ( &sharedobjectbithintstream, 0, 1 );
    }
    for ( i = 0; i < rescount; i++ ){
        b = _DOC->Entries[i + 1 + resstart].Offset - _DOC->Entries[i + resstart].Offset;
        ULBitStreamPutBits ( &sharedobjectbithintstream, b - mi, bsz );
        ULBitStreamPutBits ( &sharedobjectbithintstream, 0, 1 );
    }
    ULBitStrmDone ( sharedobjectbithintstream );
    /************************************************************************/
    /* Shared Object Hint Table End                                         */
    /************************************************************************/

    /************************************************************************/
    /* Page Objects Hint Table                                              */
    /************************************************************************/
    hstr = _CosStreamValue ( hs );
    ULBitStrmInit ( hstr, bs );
    ULBitStreamPutBits ( &bs, minPgObj, 32 );                                                   /* Item 1 */
    ULBitStreamPutBits ( &bs, _DOC->Entries[_CosObjID ( _DocPage(Doc,0)->PO )].Offset,
       32 );                                                                                    /* Item 2*/
    bitsperpage = ULGetBitsPerInt ( maxPgObj - minPgObj );
    ULBitStreamPutBits ( &bs, bitsperpage, 16 );                                                /* Item 3 */
    maxPgSize = minPgSize = pageinfo[0].PageSize = _DOC->Entries[1].Offset -
                _DOC->Entries[_CosObjID ( _DocPage(Doc,0)->PO )].Offset;
    for ( i = 1; i < _DOC->Pages->Size; i++ ){
        k = _CosObjID ( _DocPage(Doc,i)->PO );
        j = pageinfo[i].PageSize = _DOC->Entries[k + pageinfo[0].ObjectCount].Offset -
            _DOC->Entries[k].Offset;
        if ( maxPgSize < j )
            maxPgSize = j;
        if ( minPgSize > j )
            minPgSize = j;
    }

    ULBitStreamPutBits ( &bs, minPgSize, 32 );                                                  /* Item 4 */
    bitsperpagesize = ULGetBitsPerInt ( maxPgSize - minPgSize );
    ULBitStreamPutBits ( &bs, bitsperpagesize, 16 );                                            /* Item 5 */
    ULBitStreamPutBits ( &bs, 0, 32 );                                                          /* Item 6 */
    ULBitStreamPutBits ( &bs, 0, 16 );                                                          /* Item 7 */
    ULBitStreamPutBits ( &bs, 0, 32 );                                                          /* Item 8 */
    ULBitStreamPutBits ( &bs, bitsperpagesize, 16 );                                            /* Item 9 */
    for ( i = 1,j = 0; i < _DOC->Pages->Size; i++ )
        if ( pageinfo[i].SharedObjectsCount > j )
            j = pageinfo[i].SharedObjectsCount;
    ULBitStreamPutBits ( &bs, bitssharedref = ULGetBitsPerInt ( j ), 16 );                      /* Item 10 */
    ULBitStreamPutBits ( &bs, bitsoffset = ULGetBitsPerInt ( pageinfo[0].ObjectCount +
        rescount ), 16 );                                                                       /* Item 11 */
    ULBitStreamPutBits ( &bs, 4, 16 );                                                          /* Item 12 */
    ULBitStreamPutBits ( &bs, 8, 16 );                                                          /* Item 13 */
    
    
    /* Table F4 */
    k = _CosObjID ( _DocPage(Doc,0)->PO );
    zz = pageinfo[0].ObjectCount - resstart;
    for ( i = 0; i < _DOC->Pages->Size; i++ ){
        ULBitStreamPutBits ( &bs, pageinfo[i].ObjectCount - minPgObj, bitsperpage );            /* Item 1*/
        ULBitStreamPutBits ( &bs, pageinfo[i].PageSize - minPgSize, bitsperpagesize );          /* Item 2 */
        if ( i == 0 )
            ULBitStreamPutBits ( &bs, 0, bitssharedref );                                       /* Item 3 */
        else{
            ULBitStreamPutBits ( &bs, pageinfo[i].SharedObjectsCount, bitssharedref );          /* Item 3 */
            for ( j = 0; j < pageinfo[i].SharedObjectsCount; j++ ){
                if ( pageinfo[i].SharedObjectList[j] > k )
                    ULBitStreamPutBits ( &bs, pageinfo[i].SharedObjectList[j] - k,
                        bitsoffset );                                                           /* Item 4 */
                else
                    ULBitStreamPutBits ( &bs, pageinfo[i].SharedObjectList[j] + zz,
                        bitsoffset );                                                           /* Item 4 */
                ULBitStreamPutBits ( &bs, 0, 4 );                                               /* Item 5 */
            }
        };
                                                                                                /* Item 6 absent */
        ULBitStreamPutBits ( &bs, pageinfo[i].PageSize - minPgSize, bitsperpagesize );          /* Item 7 */
    }
    ULBitStrmDone ( bs );
    _CosDictAppend ( hs, S, CosIntNew ( Doc, false, ULStreamGetPosition ( hstr ) ) );
    ULStreamCopyToStream ( sharedobjecthintstream, hstr );
    ULStreamClose ( sharedobjecthintstream );
    CosStreamFlatePack ( _LIB, hs );
    _CosDictAppend ( hs, Length, CosIntNew ( Doc, false,
         ULStreamGetSize ( _CosStreamValue ( hs ) ) ) );

    /* Вычисляем размер всего файла*/
    k = hh;
	if ( crypting && UsedCrypto->nkl >= pt128BitAESProtection ){
		obj = CosCopy ( Doc, hs );
		ID.ID = hit;
		ID.GenID = 0;
		CosCryptObject ( obj, UsedCrypto, ID, true );
		tsec = ULGetIntSize ( hit ) + CosGetObjSize (  obj ) + 14;
		CosFree ( obj );
	} else
    tsec = ULGetIntSize ( hit ) + CosGetObjSize (  hs ) + 14;
    fullsize = ULStreamGetPosition ( TMPStrm ) +
               MainHeaderSize +
               6 /*Xref size */ +
               4 +
               ULGetIntSize ( k + 1 ) * 2 +
               ( k + 1 ) * 20 +
               9 +
               12 +
               18 +
               ULGetIntSize ( fsec ) +
               tsec;
    _CosDictAppend ( Linearized, L, CosIntNew ( Doc, false, fullsize ) );
    _CosDictAppend ( Linearized, O, CosIntNew ( Doc, false, _CosObjID ( _DocPage(Doc,0 )->PO ) ) );
    _CosDictAppend ( Linearized, E, CosIntNew ( Doc, false, _DOC->Entries[1].Offset + tsec ) );
    _CosDictAppend ( Linearized, T, CosIntNew ( Doc, false, ULStreamGetPosition ( TMPStrm ) +
                                         MainHeaderSize +
                                         tsec +
                                         6 +
                                         4 +
                                         ULGetIntSize ( k + 1 ) ) );
    CosArrayAppend ( hintarray, CosIntNew ( Doc, false, MainHeaderSize ) );
    CosArrayAppend ( hintarray, CosIntNew ( Doc, false, tsec ) );
    CosArrayRemove ( hintarray, 0 );
    CosArrayRemove ( hintarray, 0 );
    ULitostr ( k + 1, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, " 0 obj" );
    CosCopyObjToStream ( Linearized, Strm );
    ULStrToStrm ( Strm, "\nendobj\n" );
    for ( i = ULStreamGetPosition ( Strm ); i < fsec - 1; i++ )
        ULStreamWriteChar ( Strm, 32 );
    ULStreamWriteChar ( Strm, '\n' );
    _CosDictAppend ( FirstTrailer, Prev, CosIntNew ( Doc, false,
        ULStreamGetPosition ( TMPStrm ) + MainHeaderSize + tsec ) );
    ULStrToStrm ( Strm, "xref\r\n" );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, " " );
    ULitostr ( fpa, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, "\r\n" );
    for ( i = 0,j = k + 1; i < fpa; i++,j++ ){
        if ( j == k + 1 )
            zz = _DOC->Entries[k + 1].Offset;
        else if ( j == hit )
            zz = MainHeaderSize;
        else
            zz = _DOC->Entries[j].Offset + tsec;
        pstr = ULIntToStrWithZero ( _LIB, zz, 10 );
        ULStrToStrm ( Strm, pstr );
        mfree ( _LIB, pstr );
        ULStrToStrm ( Strm, " 00000 n\r\n" );
    };
    ULStrToStrm ( Strm, "trailer\r\n" );
    CosCopyObjToStream ( FirstTrailer, Strm );
    ULStrToStrm ( Strm, "\r\nstartxref\r\n0\r\n%%EOF" );
    for ( i = ULStreamGetPosition ( Strm ); i < MainHeaderSize - 2; i++ )
        ULStreamWriteChar ( Strm, 32 );
    ULStrToStrm ( Strm, "\r\n" );

    ULitostr ( hit, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, " 0 obj" );
    if ( crypting ){
        obj = CosCopy ( Doc, hs );
        ID.ID = hit;
        ID.GenID = 0;
        CosCryptObject ( obj, UsedCrypto, ID, true );
        CosCopyObjToStream ( obj, Strm );
        CosFree ( obj );
    } else
        CosCopyObjToStream ( hs, Strm );
    ULStrToStrm ( Strm, "\nendobj\n" );
    ULStreamCopyToStream ( TMPStrm, Strm );
    ULStreamClose ( TMPStrm );
    ULStrToStrm ( Strm, "xref\r\n0 " );
    ULitostr ( k + 1, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, "\r\n0000000000 65535 f\r\n" );
    for ( i = 1; i <= k; i++ ){
        pstr = ULIntToStrWithZero ( _LIB, _DOC->Entries[i].Offset + tsec, 10 );
        ULStrToStrm ( Strm, pstr );
        mfree ( _LIB, pstr );
        ULStrToStrm ( Strm, " 00000 n\r\n" );
    }
    ULStrToStrm ( Strm, "trailer\r\n<</Size " );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, ">>\r\nstartxref\r\n" );
    ULitostr ( fsec, str );
    ULStrToStrm ( Strm, str );
    ULStrToStrm ( Strm, "\r\n%%EOF" );
    if ( pageinfo ){
        for ( i = 1; i < _DOC->Pages->Size; i++ )
            if ( pageinfo[i].SharedObjectsCount )
                mfree ( _LIB, pageinfo[i].SharedObjectList );
        mfree ( _LIB, pageinfo );
    }
    if ( delID )
        CosFree ( ncr.FileID );
    CosFree ( FirstTrailer );
}
