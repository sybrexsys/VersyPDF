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
VSSignAI.h
*********************************************************************/

#ifndef VSSignAI_H
#define VSSignAI_H

#include "../VSTypes.h" 
#include "../VSError.h"
#include "VSMisc.h"
#include "VSkPKCS12I.h"


#ifndef NOT_USE_SIGN

#define SULStrToStrm(strm, str) \
	{ \
	char *pc = str; \
	sha1_Context *phctx = &hctx; \
	if (CD(Doc)->Signed) sha1_Update( phctx, (PppUns8)pc, (int)strlen(str)); \
	ULStrToStrm((strm), ( char * ) (str)); \
	}

#define SULStrmWriteChar(strm, c) \
	{ \
	char *pc = &c; \
	sha1_Context *phctx = &hctx; \
	if (CD(Doc)->Signed) sha1_Update( phctx, (PppUns8)pc, 1); \
	ULStreamWriteChar((strm), (c)); \
	}


#ifdef  __cplusplus
extern "C"{
#endif

    typedef struct {
        x509    *certificate;
        rsa_Key *key;
    } PDFPfx;

    typedef struct {
        asn1_OctetString*sig;
        ppUns8          *asn1;
        long            length;
    } pkcs1;

    pkcs1 *pkcs1_Create(PDFLibHandle);
    void pkcs1_Dispose(PDFLibHandle Lib, pkcs1 *obj);

    typedef struct {
        PDFLibHandle    lib;
        char            *signer;
        char            *reason;
        char            *field;
        ppInt32         PageIndex;
        TPDFRect         Rect;
        ppBool          isPKCS7;
        union {
            void    *ptr;
            pkcs7   *p7;
            pkcs1   *p1;
        } contents;
        PDFPfx  *digitalID;
        PDFCosHandle   Annot, Sign;
		PDFPaintContent Drawer;
		ppBool IsVisible;
		PDFCosHandle VisibleContent;
    } sig_Context;

    sig_Context *sig_CreateContext(PDFDocHandle);
	void sig_DisposeContext( PDFLibHandle Lib, sig_Context *ctx);

    void sig_AddSigDict(PDFDocHandle);
    void sig_AddSigAnnot(PDFDocHandle);
    void sig_UpdateAcroForm(PDFDocHandle);
    void sig_UpdatePageAnnots(PDFDocHandle);

    void sig_UpdateByteRange(PDFDocHandle, int, int, int);
    void sig_CreateContents(PDFDocHandle);
    void sig_UpdateContents(PDFDocHandle, ppUns8 *, int);

    typedef void *PDFPfxHandle, *PDFSigHandle;

    PDFPfxHandle PDFPfxCreate(PDFLibHandle);
    void PDFPfxDispose( PDFLibHandle Lib, PDFPfxHandle hpfx);
    PDFPfxHandle PDFPfxLoadFromStream(PDFLibHandle, PDFStreamHandle, const ppUns8 *);
    PDFPfxHandle PDFPfxLoadFromFile(PDFLibHandle, char *, const ppUns8 *);
    PDFPfxHandle PDFPfxLoadFromBuffer(PDFLibHandle, void *, ppInt32, const ppUns8 *);

    PDFSigHandle PDFDocPrepSign(PDFDocHandle);
    void sig_SetSigner(PDFSigHandle, char *);
    void sig_SetReason(PDFSigHandle, char *);
    void sig_SetField(PDFSigHandle, char *);

    void PDFDocSign(PDFDocHandle, PDFPfxHandle);

#ifdef  __cplusplus
}
#endif

#else
#define SULStrToStrm(strm, str)  ULStrToStrm((strm), (str)); 

#define SULStrmWriteChar(strm, c) ULStreamWriteChar((strm), (c)); 

#endif

#endif /* #ifndef VSSignAI_H */

