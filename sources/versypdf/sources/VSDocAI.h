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
VSDocAI.h
*********************************************************************/

#ifndef VSDocAI_H
#define VSDocAI_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSLibI.h"
#include "../VSDocA.h"
#include "VSSignAI.h"
#include "VSCosAI.h"

#define PDFEngine "VersyPDF Engine Sybrex Systems"



#ifdef __cplusplus
extern "C"{
#endif
#define CD(x) ((CosDoc)x)


    typedef enum EntryType {
        etFree,
        etUnCompressed,
        etCompressed
    } EntryType;

    typedef enum SaveEntryType {
        setSave                 = 0,
        setPacked,
        setPacker,
        setUnPacked
    } SaveEntryType;

    /*      XREF Entry Structure            */
    typedef struct TXREFEntry {
        ppUns32         Offset;
        ppUns16         Generation;
        EntryType       Used;
        ppBool          Loaded;
        PDFCosHandle    Obj;
        ppUns32         SaveInfo;
        ppUns32         Additional;
        ppUns32         Index;
        ppUns32         ObjID;
		ppUns32			NewOffset;
    } TXREFEntry;

    typedef struct _t_PDFDocImageEntry {
        PDFCosHandle Obj;
        ppBool IsMask;
    } PDFDocImageEntry, *PPDFDocImageEntry;

    typedef struct _t_PDFDocGStateEntry {
        PDFCosHandle Obj;
    } PDFDocGStateEntry, *PPDFDocGStateEntry;

    typedef struct _t_PDFDocAcroFormEntry {
        PDFCosHandle Obj;
    } PDFDocAcroFormEntry, * PPDFDocAcroFormEntry;

    typedef struct _t_PDFDocActionEntry {
        PDFCosHandle Obj;
    } PDFDocActionEntry, * PPDFDocActionEntry;


    typedef struct _t_PDFDoc {
        PDFLibHandle        Lib;
        TCosObj             NullObject;
		ppUns32				HeaderOffset;
        ppUns32             Size;
        ppUns32             Capacity;
        PDFCosHandle        Root;
        PDFCosHandle        Info;
		PDFID				RootID;
		PDFID				InfoID;
        ppBool              Crypted;
		ppBool				UnCrypted;
		ppBool				Check;
        ppBool              UseOldSecurity;
        ppBool              NewCrypted;
        char                *UPassword;
        char                *OPassword;
        ppInt32             NewPermission;
        TPDFProtectionType  nkl;
        ppBool              Linearized;
        ppBool              Remove;
		ppBool              Packed;
		ppBool				CryptMetadata;
        TXREFEntry          *Entries; 
        TPDFVersion         Version;
        TPDFCryptoType      CryptoInfo;
        PDFStreamHandle     Strm;
        ppBool              AutoRemove;
        PPDFList            Pages;
        PPDFList            FontList;
        PPDFList            ImageList;
        PPDFList            PatternList;
        ppUns8              JpegQuality;
        PPDFExtList         GStateList;
        PPDFExtList         ActionList;
        PPDFList	        AcroFormList;
#ifndef NOT_USE_SIGN
        ppBool              Signed;
        sig_Context         *SigCtx;
#endif
		ppBool				AcroIsLoaded;
		PPDFList			AcroItems;
		ppBool				AcroValuesWasModified;
		ppBool				NeedAppendReDrawAcroScript;

#ifdef WINDOWS_PLATFORM
        PPDFList            TTFList;
		ppBool				AutoLaunch;
        ppBool              EmfImagesAsJpeg;
		ppInt32             DocFileLength;
		HDC					DC;
        ppBool              BWImagesAsJBIG2;
		ppBool				UseScreen;
#endif
    } TPDFDoc, *CosDoc;

#define CheckPDFVersion(Doc, version) ((CosDoc)Doc)->Version = max (((CosDoc)Doc)->Version, pdfver1 ## version )

    typedef struct linpageaddinfo {
        ppUns32 ObjectCount;
        ppUns32 PageSize;
        ppUns32 SharedObjectsCount;
        ppUns32 *SharedObjectList;
    } linpageaddinfo, *plinpageaddinfo;


    typedef struct TPDFPageInfo {
        PDFCosHandle    PO;
        PDFCosHandle    Inh;
        PDFDocHandle    Doc;
    } TPDFPageInfo, *PPDFPageInfo;



    void PDFDocPackFree(PDFDocHandle Doc);
    void PDFDocSavePackedLinearized(PDFDocHandle Doc, PDFStreamHandle Strm);
    void PDFDocSaveUnPackedLinearized(PDFDocHandle Doc, PDFStreamHandle Strm);
    void PDFDocSavePackedUnLinearized(PDFDocHandle Doc, PDFStreamHandle Strm);
    void PDFDocSaveUnPackedUnLinearized(PDFDocHandle Doc, PDFStreamHandle Strm);
    void PDFDocRemoveUnUsed(PDFDocHandle Doc, ppInt32 *UsedCount);
    ppInt32 PDFDocRenumAllObjects(PDFDocHandle Doc);
    void PDFDocLoadPages(PDFDocHandle Doc);
    void PDFDocRemoveInheritedFromPages(PDFDocHandle Doc);
    PBXHandle PDFCreateDemoContent( PDFDocHandle Doc, TPDFRect Rect );
	void PDFAcroLoadForms (PDFDocHandle Doc);
	void PDFAcroFreeForms ( PDFDocHandle Doc );


	/* The annotation appearance states */
    typedef enum {
		asNormal,		/* The normal appearance is used when the annotation is not interacting with the
						user. This is also the appearance that is used for printing the annotation. */
        asRollover,		/* The rollover appearance is used when the user moves the cursor into the annotation’s
						active area without pressing the mouse button.*/
        asDown			/* The down appearance is used when the mouse button is pressed or held down
						within the annotation’s active area. */
    } TAppearanceState;
	LIB_API void PDFAnnotationSetAppearance ( PDFAnnotationHandle Annotation, TAppearanceState State,  PDFCosHandle Appearance );

#define _DocPage(Doc,PageIndex) ((PPDFPageInfo)CD(Doc)->Pages->FList[PageIndex])
#define PGO(PageIndex) (((PPDFPageInfo)CD(Doc)->Pages->FList[PageIndex])->PO)
#define PPGO(PageIndex) (((PPDFPageInfo)_DDOC->Pages->FList[PageIndex])->PO)

#define names (((PPDFLib)_LIB)->Names)
#define GetAtomDoc(x) (names[pdfnidx ## x]!= ppAtomNull ? names[pdfnidx ## x] : (names[pdfnidx ## x] =ULStringToAtom(_LIB,# x)))

#define nnames (((PPDFLib)_LLIB)->Names)
#define GetAtomDDoc(x) (nnames[pdfnidx ## x] != ppAtomNull ? nnames[pdfnidx ## x] : (nnames[pdfnidx ## x] =ULStringToAtom(_LLIB,# x)))

#define namez(lib) (((PPDFLib)lib)->Names)
#define GetAtomLib(Lib,x) (namez(Lib)[pdfnidx ## x]!= ppAtomNull ? namez(Lib)[pdfnidx ## x] : (namez(Lib)[pdfnidx ## x] =ULStringToAtom(Lib,# x)))

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSDocAI_H */
