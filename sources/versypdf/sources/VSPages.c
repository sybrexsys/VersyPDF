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
VSPages.c
*********************************************************************/

#include "../VSPagesA.h"
#include "VSPagesAI.h"

#define _DOC CD(Doc)
#define _LIB _DOC->Lib



/*  Document Connection Structure, Page Objects Container */
typedef struct TPageAndObject {
	ppUns32 PageIndex;
	PDFCosHandle PageObject;
	ppBool MoreOnePage;
}TPageAndObject, * PPageAndObject;

typedef struct _t_TPDFDocumentConnection {
	PDFDocHandle    OldDocument;    /* Source Document where pages are taken from */
	PDFDocHandle    NewDocument;    /* Destination Document where to put pages */
	ppUns32         Size;           /* Size of Queue of Page Numbers */
	PPageAndObject  Pages;          /* Queue Page Numbers, with repeated
									numbers possibility */
} TPDFDocumentConnection/*, *PPDFDocumentConnection*/;




void PDFPagesCreateContainer( PDFDocHandle OldDoc, PDFDocHandle NewDoc,
							 PPDFPagesObjectContainer ObjectContainer )
{
	ObjectContainer->OldDoc = OldDoc;
	ObjectContainer->NewDoc = NewDoc;
	ObjectContainer->List = ULExtListCreate( CD(OldDoc)->Lib,sizeof( TPDFPagesReincarnation ), 0);  
}

/* Container destructor - free objects array and unlink documents */
void PDFPagesFreeContainer( PPDFPagesObjectContainer ObjectContainer )
{
	ULExtListFree( ObjectContainer->List);
	ObjectContainer->OldDoc = NULL;
	ObjectContainer->NewDoc = NULL;
}

void PDFPagesAddObjectsToContainer( PPDFPagesObjectContainer ObjectContainer,
									  PDFCosHandle NewObject, ppBool IsPage, PPDFDocumentConnection DocumentConnection, ppUns32 PageIndex )
{
	PDFDocHandle Doc = CO(NewObject)->Doc;
	PDFCosHandle ChildObject, NW;
	PDFID RefValue;
	ppUns32 i;
	ppAtom v;
	TPDFPagesReincarnation Rec;

	if ( _CosIsIndirect( NewObject ) ) {
		if (IsPage || !CD(ObjectContainer->OldDoc)->Entries[ _CosObjID( NewObject ) ].Additional ){
			CD(ObjectContainer->OldDoc)->Entries[ _CosObjID( NewObject ) ].Additional++;
			NW = CosCopy( ObjectContainer->NewDoc, NewObject);
			CosAddToDoc( ObjectContainer->NewDoc,NW );
			Rec.CosObject = NW;
			Rec.NewID = _CosObjFullID( NW );
			Rec.OldID = _CosObjFullID( NewObject );
			ULExtListAdd(ObjectContainer->List, &Rec);
			if ( IsPage ){
				DocumentConnection->Pages[PageIndex].PageObject = NW;
				DocumentConnection->Pages[PageIndex].MoreOnePage = 
					CD(ObjectContainer->OldDoc)->Entries[ _CosObjID( NewObject ) ].Additional > 1 ? true : false;
			}
		} else
			return;
	}
	switch ((ppUns32) _CosGetType( NewObject ) ) {
		case CosRef :
			RefValue = _CosRefValue( NewObject );
			ChildObject = CosGetFromDoc( ObjectContainer->OldDoc, RefValue.ID );
			if (_IsCosDict(ChildObject) && _IsCosName(NW=_CosDictValueByName(ChildObject,Type)) 
				&& _CosNameValue(NW)==GetAtomDoc(Page) && CD(ObjectContainer->OldDoc)->Entries[ _CosObjID( ChildObject ) ].SaveInfo == 0){
				break;
			}
			PDFPagesAddObjectsToContainer( ObjectContainer, ChildObject, false, NULL,0 );
			break;
		case CosDict :
			for ( i = 0; i < _CosDictCount( NewObject ); i++ ) {
				v = _CosDictKey( NewObject, i );
				if (!(v == GetAtomDoc(Parent) /*||v == GetAtomDoc(Annots)*/ || v == GetAtomDoc(B))){
					ChildObject = _CosDictValue( NewObject, i );
					PDFPagesAddObjectsToContainer( ObjectContainer, ChildObject, false, NULL,0 );
				}
			}
			break;
		case CosArray :
			for ( i = 0; i < _CosArrayCount( NewObject ); i++ ) {
				ChildObject = _CosArrayItem( NewObject, i );
				PDFPagesAddObjectsToContainer( ObjectContainer, ChildObject, false, NULL,0 );
			}
			break;

		case CosStream :
			NewObject = _CosStreamAttr( NewObject );
			for ( i = 0; i < _CosDictCount( NewObject ); i++ ) {
				v = _CosDictKey( NewObject, i );
				if (!(v == GetAtomDoc(Parent) || v == GetAtomDoc(B))){
					ChildObject = _CosDictValue( NewObject, i );
					PDFPagesAddObjectsToContainer( ObjectContainer, ChildObject, false, NULL,0 );
				}
			}
			break;

		default :;
	}
}



PPDFDocumentConnection PDFCreateDocumentConnection( PDFDocHandle OldDocument,
                                  PDFDocHandle NewDocument )
{
	PPDFDocumentConnection DocumentConnection ;
    if (( !OldDocument ) || ( !NewDocument ))
        return NULL;
	DocumentConnection = (TPDFDocumentConnection *)mmalloc(CD(OldDocument)->Lib, sizeof(TPDFDocumentConnection));
    DocumentConnection->NewDocument = NewDocument;
    DocumentConnection->OldDocument = OldDocument;
    DocumentConnection->Size = 0;
    DocumentConnection->Pages = NULL;
	return DocumentConnection;
}

void PDFSelectPageFromSourceDocument( PPDFDocumentConnection DocumentConnection,
                                      ppUns32 PageIndex )
{
    if ( !DocumentConnection )
        return;
    DocumentConnection->Size++;
	DocumentConnection->Pages = (PPageAndObject)mrealloc( CD(DocumentConnection->OldDocument)->Lib, DocumentConnection->Pages,
        DocumentConnection->Size * sizeof(TPageAndObject) );
    DocumentConnection->Pages[ DocumentConnection->Size - 1 ].PageIndex = PageIndex;
}

PDFID PDFPagesGetNewID( PPDFPagesObjectContainer ObjectContainer, PDFID RefValue)
{
	ppInt32 LookingIndex, Step;

	LookingIndex = 0;
	Step = ObjectContainer->List->Size;

	while  ( ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[LookingIndex].OldID.ID < RefValue.ID ) {

		if ( Step > 1 )
			Step >>= 1;

		while (( LookingIndex + Step < (ppInt32)ObjectContainer->List->Size ) && 
			( ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[ LookingIndex + Step ].OldID.ID
			<= RefValue.ID ))
			LookingIndex = LookingIndex + Step; 

		if (( Step == 1 ) &&
			( ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[ LookingIndex ].OldID.ID != RefValue.ID ) &&
			(( LookingIndex + Step >= (ppInt32)ObjectContainer->List->Size ) ||
			( ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[ LookingIndex + Step ].OldID.ID > RefValue.ID )))
			PDFIDNULL( RefValue );
	}

	if ( ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[ LookingIndex ].OldID.ID == RefValue.ID )
		RefValue = ((TPDFPagesReincarnation*)ObjectContainer->List->FList)[ LookingIndex ].NewID;
	return RefValue;
}

void PDFPagesRenumerateObject( PPDFPagesObjectContainer ObjectContainer, PDFCosHandle CosObject)
{
	PDFDocHandle Doc = CO(CosObject)->Doc;
	ppUns32 i;

	switch ((ppUns32)_CosGetType(CosObject)) {
		case CosRef :
			CO(CosObject)->Data.rfv.Value = PDFPagesGetNewID( ObjectContainer, _CosRefValue( CosObject ) );
			break;

		case CosDict :
			for ( i = 0; i < _CosDictCount( CosObject ); i++ ){
				if (_CosDictKey( CosObject, i ) == GetAtomDoc( Annot )){
					CosDictAppend( CosObject, _CosDictKey( CosObject, i ),CosNullNew( Doc));
					i--;
					continue;
				}
				if (_CosDictKey( CosObject, i ) == GetAtomDoc( B )){
					CosDictAppend( CosObject, _CosDictKey( CosObject, i ),CosNullNew( Doc));
					i--;
					continue;
				}
				if (_CosDictKey( CosObject, i ) != GetAtomDoc( Parent ))
					PDFPagesRenumerateObject( ObjectContainer,	_CosDictValue( CosObject, i ) );
			}			
			break;
		case CosArray :
			for ( i = 0; i < _CosArrayCount( CosObject ); i++ )
				PDFPagesRenumerateObject( ObjectContainer,
				_CosArrayItem( CosObject, i ) );
			break;

		case CosStream :
			CosObject = _CosStreamAttr( CosObject );
			for ( i = 0; i < _CosDictCount( CosObject ); i++ ){
				if (_CosDictKey( CosObject, i ) == GetAtomDoc( Annot )){
					CosDictAppend( CosObject, _CosDictKey( CosObject, i ),CosNullNew( Doc));
					i--;
					continue;
				}
				if (_CosDictKey( CosObject, i ) == GetAtomDoc( B )){
					CosDictAppend( CosObject, _CosDictKey( CosObject, i ),CosNullNew( Doc));
					i--;
					continue;
				}
				if (_CosDictKey( CosObject, i ) != GetAtomDoc( Parent ))
					PDFPagesRenumerateObject( ObjectContainer,	_CosDictValue( CosObject, i ) );
			}			
			break;
		default : ;
	}
	return;
}

int CDECL PDFPagesQCompareObject( const void *S1, const void *S2 )
{
	return ((PPDFPagesReincarnation)S1)->OldID.ID - ((PPDFPagesReincarnation)S2)->OldID.ID;
}

void ULRenumerateContainer( PPDFPagesObjectContainer ObjectContainer)
{
	ppUns32 i;

	qsort( (void *)ObjectContainer->List->FList, (size_t)ObjectContainer->List->Size,
		sizeof( TPDFPagesReincarnation ), PDFPagesQCompareObject );

	for ( i = 0; i < ObjectContainer->List->Size; i++ ) 
		PDFPagesRenumerateObject( ObjectContainer,
		((TPDFPagesReincarnation*)ObjectContainer->List->FList)[i].CosObject );
}



void ULPlacePageToDestinationDocument( PDFDocHandle Doc,  PDFCosHandle Obj )
{
	ppUns32 i;
	PDFCosHandle w1,w2, Pages;
	PPDFPageInfo pg;
	Pages = _CosDictValueByName(_DOC->Root, Pages);
	if (!_IsCosDict(Pages))
		_CosDictAppend(_DOC->Root,Pages, Pages=CosDictNew(Doc, true,2));
    w1 = _CosDictValueByName(Pages, Count);
	if ( !_IsCosInt(w1))
		_CosDictAppend(Pages,Count,w1 = CosIntNew(Doc, false, 0));
	w2 = _CosDictValueByName( Pages, Kids);
	if (!_IsCosArray(w2))
		_CosDictAppend(Pages, Kids, w2 = CosArrayNew(Doc, false, 1)); 
	_CosDictAppend(Obj, Parent,Pages);
	_CosIntValue(w1)++;
	CosArrayAppend(w2, Obj);
	pg= ( PPDFPageInfo ) mmalloc(_LIB,sizeof(TPDFPageInfo));
	pg->Doc=Doc;
	pg->PO=Obj;
	pg->Inh=CosNullNew(Doc);
	PDFTRY (_LIB){
		i = ULListAdd(_DOC->Pages,pg);
	} PDFEXCEPT ( _LIB ){
		mfree(_LIB, pg);
		PDFRERAISE ( _LIB );
	} PDFTRYEND ( _LIB );
}

void PDFCopyPagesToDestinationDocument( PPDFDocumentConnection DocumentConnection )
{
    TPDFPagesObjectContainer ObjectContainer;
    ppUns32 PageIndex;
    ppUns32 i;
	PDFID FID, PID;
	PDFCosHandle w1, w2, w, arr, s1, s2;
	PDFDocHandle Doc;
	ppBool Added;

    if ( !DocumentConnection )
        return;

    if (( !DocumentConnection->Pages ) || ( DocumentConnection->Size == 0 ))
        return;

    if ( !CD(DocumentConnection->OldDocument)->Pages )
        PDFDocLoadPages ( DocumentConnection->OldDocument );
	if ( !CD(DocumentConnection->NewDocument)->Pages )
		PDFDocLoadPages( DocumentConnection->NewDocument ); 
	Doc = DocumentConnection->OldDocument;

	PDFDocRemoveInheritedFromPages(DocumentConnection->OldDocument);

	for ( i = 1; i < CD(DocumentConnection->OldDocument)->Size;i++){
		CD(DocumentConnection->OldDocument)->Entries[i].Additional = 0;
		CD(DocumentConnection->OldDocument)->Entries[i].SaveInfo = 0;
	}


    PDFPagesCreateContainer( DocumentConnection->OldDocument, DocumentConnection->NewDocument,
        &ObjectContainer);

	for ( i = 0 ; i < DocumentConnection->Size; i++ ) {
		PageIndex = DocumentConnection->Pages[ i ].PageIndex;
		if (PageIndex < CD(DocumentConnection->OldDocument)->Pages->Size ){
			w1 = ((PPDFPageInfo)CD(DocumentConnection->OldDocument)->Pages->FList[ PageIndex ] )->PO;
			CD(DocumentConnection->OldDocument)->Entries[_CosObjID(w1)].SaveInfo = 1;
		}
	}

    for ( i = 0 ; i < DocumentConnection->Size; i++ ) {
        PageIndex = DocumentConnection->Pages[ i ].PageIndex;
        if (PageIndex < CD(DocumentConnection->OldDocument)->Pages->Size )
            PDFPagesAddObjectsToContainer( &ObjectContainer,
            ((PPDFPageInfo)CD(DocumentConnection->OldDocument)->Pages->FList[ PageIndex ] )->PO, true, DocumentConnection, i );
    }

    ULRenumerateContainer( &ObjectContainer );
	for ( i = 0 ; i < DocumentConnection->Size; i++ ) {
		if ( DocumentConnection->Pages[i].MoreOnePage){
			w1 = _CosDictValueByName( DocumentConnection->Pages[i].PageObject, Contents);
			if (_IsCosArray(w1) && _CosIsIndirect(w1)){
				w2 = CosCopy(DocumentConnection->NewDocument, w1);
				_CosDictAppend(DocumentConnection->Pages[i].PageObject, Contents, w2);
			}
		};
	}
	for ( i = 0 ; i < DocumentConnection->Size; i++ ) {
		ULPlacePageToDestinationDocument( DocumentConnection->NewDocument, DocumentConnection->Pages[i].PageObject);
	}


	w1 = PDFDocGetRoot(DocumentConnection->OldDocument );
	w2 = _CosDictValueByName(w1,AcroForm);
	if (_IsCosDict(w2)){
		w1 = _CosDictValueByName(w2, Fields);
		if (_IsCosArray(w1)){
			Added = false;
			for (i = 0; i < _CosArrayCount(w1); i++){
				w = _CosArrayItem( w1, i );
				if ( !_IsCosRef(w) ) 
					continue;
				FID = _CosRefValue(w);
				PID = PDFPagesGetNewID(&ObjectContainer, FID);
				if ( PID.ID == 0 &&  PID.GenID == ppMAXUns16)
					continue;
				if ( !Added ){
					Added = true;
					s1 = PDFDocGetRoot(DocumentConnection->NewDocument );
					s2 = _CosDictValueByName(s1,AcroForm);
					if ( !_IsCosDict(s2)){
						_CosDictAppend( s1, AcroForm, s2 = CosDictNew(DocumentConnection->NewDocument, true, 5 ) );
						_CosDictAppend(s2, Fields, arr = CosArrayNew(DocumentConnection->NewDocument, false, 5 ) );
					} else {
						arr = _CosDictValueByName(s2, Fields);
						if (!_IsCosArray(arr)){
							_CosDictAppend(s2, Fields, arr = CosArrayNew(DocumentConnection->NewDocument, false, 5 ) );
						}
					}
				}
				CosArrayAppend(arr, CosNewRef(DocumentConnection->NewDocument,PID.ID, PID.GenID));
			}
		}
	}
    PDFPagesFreeContainer( &ObjectContainer );
	mfree( CD(DocumentConnection->OldDocument)->Lib,  DocumentConnection->Pages );
    DocumentConnection->Pages = NULL;
    DocumentConnection->Size = 0;
}
 
void PDFFreeDocumentConnection( PPDFDocumentConnection DocumentConnection )
{
    if ( !DocumentConnection )
        return;
	mfree( CD(DocumentConnection->OldDocument)->Lib, DocumentConnection->Pages );
	mfree(CD(DocumentConnection->OldDocument)->Lib,DocumentConnection);
}
