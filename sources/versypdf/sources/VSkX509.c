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
VSkX509.c
*********************************************************************/

#include "VSkX509I.h"
#include <string.h>

#ifndef NOT_USE_SIGN

ppInt32 x509_Encode(x509 *cert, ppUns8 **pbuf)
{
    ppInt32 r, rslt=0;
    ppUns8 *p;
    if (cert == NULL) return 0;

    rslt += x509_EncodeCertInfo(cert->cert_info, NULL);
    rslt += x509_EncodeAlgorithm(cert->sig_alg, NULL);
    rslt += asn1_EncodeBitString(cert->signature, NULL);

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

    x509_EncodeCertInfo(cert->cert_info, &p);
    x509_EncodeAlgorithm(cert->sig_alg, &p);
    asn1_EncodeBitString(cert->signature, &p);

    *pbuf = p;
    return r;
}

x509 *x509_Decode(PDFLibHandle lib, x509 **pcert, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509 *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (pcert == NULL || (*pcert) == NULL)
            rslt = x509_Create(lib);
        else
            rslt = *pcert;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max); 

        ctx.q = ctx.p;
        x509_DecodeCertInfo(lib, &(rslt->cert_info), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->sig_alg), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeBitString(lib, &(rslt->signature), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pcert != NULL) (*pcert) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pcert == NULL || (*pcert) != rslt))
        {
            x509_Dispose(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509 *x509_Create(PDFLibHandle lib)
{
    x509 *rslt = NULL;
    PDFTRY(lib) {
        rslt = (x509* )mmalloc(lib, sizeof(x509));
        rslt->cert_info = x509_CreateCertInfo(lib);
        rslt->sig_alg = x509_CreateAlgorithm(lib);
        rslt->signature = asn1_CreateBitString(lib);
        rslt->asn1 = NULL; 
        rslt->length = 0;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_Dispose(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_Dispose(PDFLibHandle lib, x509 *cert)
{
    if (cert == NULL) return;
    x509_DisposeCertInfo(lib, cert->cert_info);
    x509_DisposeAlgorithm(lib, cert->sig_alg);
    asn1_DisposeString(lib, (asn1_String* )cert->signature);
    if (cert->asn1 != NULL) {
        mfree(lib, cert->asn1);
        cert->asn1 = NULL;
        cert->length = 0;
    }
    mfree(lib, cert);
}



ppInt32 x509_EncodeCertInfo(x509_CertInfo *info, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppInt32 v1 = 0, v2 = 0;
    ppUns8 *p;
    if (info == NULL) return 0;

    if (info->version != NULL)
    {
        v1 = asn1_EncodeInteger(info->version, NULL);
        rslt += asn1_ObjectSize(1, v1, 0);
    }
    rslt += asn1_EncodeInteger(info->serialNumber, NULL);
    rslt += x509_EncodeAlgorithm(info->signature, NULL);
    rslt += x509_EncodeName(info->issuer, NULL);
    rslt += x509_EncodeValidity(info->validity, NULL);
    rslt += x509_EncodeName(info->subject, NULL);
    rslt += x509_EncodePublicKey(info->key, NULL);
    if (info->issuerUID != NULL)
        rslt += asn1_EncodeBitString(info->issuerUID, NULL);
    if (info->subjectUID != 0)
        rslt += asn1_EncodeBitString(info->subjectUID, NULL);

    if (info->extensions != NULL && vector_GetCount(info->extensions) != 0)
    {
        v2 = asn1_EncodeSet(info->extensions, NULL, x509_EncodeExtension,
            SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
        rslt += asn1_ObjectSize(1, v2, 3);
    }

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;
    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

    if (info->version != NULL)
    {
        asn1_PutObject(&p, 1, v1, 0, CTX_SPECIFIC_CLS);
        asn1_EncodeInteger(info->version, &p);
    }
    asn1_EncodeInteger(info->serialNumber, &p);
    x509_EncodeAlgorithm(info->signature, &p);
    x509_EncodeName(info->issuer, &p);
    x509_EncodeValidity(info->validity, &p);
    x509_EncodeName(info->subject, &p);
    x509_EncodePublicKey(info->key, &p);
    if (info->issuerUID != NULL)
    {
        ppUns8 *q = p;
        asn1_EncodeBitString(info->issuerUID, &p);
        (*q) = (CTX_SPECIFIC_CLS | 1 | ((*q) & CONSTRUCTED));
    }
    if (info->subjectUID != NULL)
    {
        ppUns8 *q = p;
        asn1_EncodeBitString(info->subjectUID, &p);
        (*q) = (CTX_SPECIFIC_CLS | 2 | ((*q) & CONSTRUCTED));
    }
    if ((info->extensions != NULL) && (vector_GetCount(info->extensions) != 0))
    {
        asn1_PutObject(&p, 1, v2, 3, CTX_SPECIFIC_CLS);
        asn1_EncodeSet(info->extensions, &p, x509_EncodeExtension,
            SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
    }

    (*pbuf) = p;
    return r;
}

x509_CertInfo *x509_DecodeCertInfo(PDFLibHandle lib, x509_CertInfo **pinfo, ppUns8 **pbuf, ppInt64 max)
{
    ppInt32 ver = 0;
    asn1_Context ctx;
    x509_CertInfo *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pinfo == NULL || (*pinfo) == NULL)
            rslt = x509_CreateCertInfo(lib);
        else
            rslt = *pinfo;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        if ((*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED))
        {
            if (ctx.len != 0L)
            {
                ppInt32 ainf, atag, acls;
                ppInt64 alen;

                ctx.q = ctx.p;
                ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                if (ainf & 0x80) 
                    PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                if (ainf == (CONSTRUCTED + 1))
                    alen = ctx.len - (ctx.p - ctx.q) - 2;
                asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, alen);
                if (ainf == (CONSTRUCTED + 1)) 
                {
                    alen = ctx.len - (ctx.p - ctx.q);
                    if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                }
            }
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
            if (rslt->version->data != NULL)
                ver = rslt->version->data[0];
        }
        else
        {
            if (rslt->version != NULL)
            {
                asn1_DisposeInteger(lib, rslt->version);
                rslt->version = NULL;
            }
        }

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->serialNumber), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->signature), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeName(lib, &(rslt->issuer), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeValidity(lib, &(rslt->validity), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeName(lib, &(rslt->subject), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodePublicKey(lib, &(rslt->key), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ver >= 1) /*version 2 extensions */
        {
            if (rslt->issuerUID != NULL)
            {
                asn1_DisposeString(lib, (asn1_String* )rslt->issuerUID);
                rslt->issuerUID = NULL;
            }
            if (rslt->subjectUID != NULL)
            {
                asn1_DisposeString(lib, (asn1_String* )rslt->subjectUID);
                rslt->subjectUID = NULL;
            }

            if (ctx.len != 0 && ((*ctx.p)&(~CONSTRUCTED)) == (CTX_SPECIFIC_CLS|1))
            {
                ppUns8 t = (*ctx.p);
                (*ctx.p) = (t & CONSTRUCTED) | BITSTRING_TAG;
                ctx.q = ctx.p;
                asn1_DecodeBitString(lib, &(rslt->issuerUID), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                (*ctx.q) = t;
            }

            if (ctx.len != 0 && ((*ctx.p)&(~CONSTRUCTED)) == (CTX_SPECIFIC_CLS|1))
            {
                ppUns8 t = (*ctx.p);
                (*ctx.p) = (t & CONSTRUCTED) | BITSTRING_TAG;
                ctx.q = ctx.p;
                asn1_DecodeBitString(lib, &(rslt->subjectUID), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                (*ctx.q) = t;
            }
        }

        if (ver >= 2) 
        {
            if (rslt->extensions != NULL)
                while (vector_GetCount(rslt->extensions))
                    x509_DisposeExtension(lib, (x509_Extension* )vector_Pop(rslt->extensions));

            if (ctx.len != 0 && (*ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS|3))
            {
                ppInt32 ainf, atag, acls;
                ppInt64 alen;

                ctx.q = ctx.p;
                ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                if (ainf & 0x80) 
                    PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                if (ainf == (CONSTRUCTED + 1))
                    alen = ctx.len - (ctx.p - ctx.q) - 2;
                asn1_DecodeSet(lib, &(rslt->extensions), &ctx.p, alen,
                    (TDecodeFunction)x509_DecodeExtension,
                    (TDisposeFunction)x509_DisposeExtension,
                    SEQUENCE_TAG, UNIVERSAL_CLS);
                if (ainf == (CONSTRUCTED + 1))
                {
                    alen = ctx.len - (ctx.p - ctx.q);
                    if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                }
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
            }
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pinfo != NULL) (*pinfo) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pinfo == NULL || (*pinfo) != rslt))
        {
            x509_DisposeCertInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_CertInfo *x509_CreateCertInfo(PDFLibHandle lib)
{
    x509_CertInfo *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_CertInfo* )mmalloc(lib, sizeof(x509_CertInfo));
        rslt->version = NULL;
        rslt->serialNumber = asn1_CreateInteger(lib);
        rslt->signature = x509_CreateAlgorithm(lib);
        rslt->issuer = x509_CreateName(lib);
        rslt->validity = x509_CreateValidity(lib);
        rslt->subject = x509_CreateName(lib);
        rslt->key = x509_CreatePublicKey(lib);
        rslt->issuerUID = NULL;
        rslt->subjectUID = NULL;
        rslt->extensions = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeCertInfo(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposeCertInfo(PDFLibHandle lib, x509_CertInfo *info)
{
    if (info == NULL) return;
    asn1_DisposeInteger(lib, info->version);
    asn1_DisposeInteger(lib, info->serialNumber);
    x509_DisposeAlgorithm(lib, info->signature);
    x509_DisposeName(lib, info->issuer);
    x509_DisposeValidity(lib, info->validity);
    x509_DisposeName(lib, info->subject);
    x509_DisposePublicKey(lib, info->key);
    asn1_DisposeBitString(lib, info->issuerUID);
    asn1_DisposeBitString(lib, info->subjectUID);
    vector_Dispose(info->extensions, (void (* )(PDFLibHandle, void * ))x509_DisposeExtension);
    mfree(lib, info);
}



ppInt32 x509_EncodeAlgorithm(x509_Algorithm *alg, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (alg == NULL) return 0;

    rslt += asn1_EncodeObject(alg->algorithm, NULL);
    if (alg->parameter != NULL)
        rslt += asn1_EncodeType(alg->parameter, NULL);

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

    asn1_EncodeObject(alg->algorithm, &p);
    if (alg->parameter != NULL)
        asn1_EncodeType(alg->parameter, &p);

    *pbuf = p;
    return r;
}

x509_Algorithm *x509_DecodeAlgorithm(PDFLibHandle lib, x509_Algorithm **palg, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_Algorithm *rslt = NULL;

    PDFTRY(lib) {
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (palg == NULL || (*palg) == NULL)
            rslt = x509_CreateAlgorithm(lib);
        else
            rslt = *palg;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->algorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (!asn1_CheckEndSequence(&ctx))
        {
            ctx.q = ctx.p;
            asn1_DecodeType(lib, &(rslt->parameter), &ctx.p, ctx.len);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }
        else
        {
            asn1_DisposeType(lib, rslt->parameter);
            rslt->parameter = NULL;
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (palg != NULL) (*palg) = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (palg == NULL || (*palg) != rslt))
        {
            x509_DisposeAlgorithm(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Algorithm *x509_CreateAlgorithm(PDFLibHandle lib)
{
    x509_Algorithm *rslt;
    rslt = (x509_Algorithm* )mmalloc(lib, sizeof(x509_Algorithm));
    rslt->algorithm = NULL; 
    rslt->parameter = NULL;
    return rslt;
}

void x509_DisposeAlgorithm(PDFLibHandle lib, x509_Algorithm *alg)
{
    if (alg == NULL) return;
    asn1_DisposeObject(lib, alg->algorithm);
    asn1_DisposeType(lib, alg->parameter);
    mfree(lib, alg);
}

ppInt32 x509_EncodeNameEntry(x509_NameEntry *entry, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (entry == NULL) return 0;

    rslt += asn1_EncodeObject(entry->object, NULL);
    rslt += asn1_EncodePrintable(entry->value, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(entry->object, &p);
    asn1_EncodePrintable(entry->value, &p);
    (*pbuf) = p;

    return r;
}

ppInt32 x509_EncodeNameEntries(x509_Name *name)
{
    x509_NameEntry *ne, *fe = NULL;
    VECTOR *v;
    BUFFER *buf;
    ppInt32 set = 0, r, rslt = 0;
    ppInt32 i;
    ppUns8 *p;
    ppInt32 size = 0;

    v = name->entries;
    for (i = 0; i < vector_GetCount(v); i++)
    {
        ne = (x509_NameEntry* )vector_GetValueAt(v, i);
        if (fe == NULL)
        {
            fe = ne;
            size = 0;
        }

        if (ne->set != set)
        {
            rslt += asn1_ObjectSize(1, size, SET_TAG);
            fe->size = size;
            fe = ne;
            size = 0;
            set = ne->set;
        }
        size += x509_EncodeNameEntry(ne, NULL);
    }

    if (fe != NULL)
    {
        /*SET OF нужен только если не пустой */
        rslt += asn1_ObjectSize(1, size, SET_TAG);
        fe->size = size;
    }

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);

    buf = name->bytes;
    if (!buffer_Enlarge(buf, r)) goto err;
    p = buf->data;

    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

    set = -1;
    for (i = 0; i < vector_GetCount(v); i++)
    {
        ne = (x509_NameEntry* )vector_GetValueAt(v, i);
        if (set != ne->set)
        {
            set = ne->set;
            asn1_PutObject(&p, 1, ne->size, SET_TAG, UNIVERSAL_CLS);
        }
        x509_EncodeNameEntry(ne, &p);
    }
    name->modified = 0;
    return r;

err:
    return -1;
}

ppInt32 x509_EncodeName(x509_Name *name, ppUns8 **pbuf)
{
    ppInt32 rslt;

    if (name == NULL) return 0;
    if (name->modified)
    {
        rslt = x509_EncodeNameEntries(name);
        if (rslt < 0) return rslt;
    }

    rslt = name->bytes->num;
    if (pbuf != NULL)
    {
        memcpy(*pbuf, name->bytes->data, rslt);
        *pbuf += rslt;
    }
    return rslt;
}

x509_NameEntry *x509_DecodeNameEntry(PDFLibHandle lib, x509_NameEntry **pentry, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_NameEntry *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pentry == NULL || (*pentry) == NULL)
            rslt = x509_CreateNameEntry(lib);
        else
            rslt = *pentry;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->object), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodePrintable(lib, &(rslt->value), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        rslt->set = 0;
        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pentry != NULL) (*pentry) = rslt;
    } PDFEXCEPT(lib){ 
        if (rslt != NULL && (pentry == NULL || (*pentry) != rslt))
        {
            x509_DisposeNameEntry(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Name *x509_DecodeName(PDFLibHandle lib, x509_Name **pname, ppUns8 **pbuf, ppInt64 max)
{
    ppInt32 set = 0, i;
    ppInt32 idx = 0;
    ppUns8 *orig;
    asn1_Context ctx;
    x509_Name *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pname == NULL || (*pname) == NULL)
            rslt = x509_CreateName(lib);
        else
            rslt = *pname;

        orig = *pbuf;
        while (vector_GetCount(rslt->entries) > 0)
            x509_DisposeNameEntry(lib, (x509_NameEntry* )vector_Pop(rslt->entries));

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        for (;;)
        {
            if (asn1_CheckEndSequence(&ctx)) break;
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->entries), &ctx.p, ctx.len,
                (TDecodeFunction)x509_DecodeNameEntry,
                (TDisposeFunction)x509_DisposeNameEntry,
                SET_TAG, UNIVERSAL_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);

            for (; idx < vector_GetCount(rslt->entries); idx++)
                ((x509_NameEntry* )vector_GetValueAt(rslt->entries, idx))->set = set;
            set++;
        }

        i = (ppInt32)(ctx.p - orig);
        buffer_Enlarge(rslt->bytes, i);
        memcpy(rslt->bytes->data, orig, i);
        rslt->bytes->num = i;
        rslt->modified = 0;

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pname != NULL) (*pname) = rslt;

    }PDFEXCEPT(lib){
        if (rslt != NULL && (pname == NULL || (*pname) != rslt))
        {
            x509_DisposeName(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_NameEntry *x509_CreateNameEntry(PDFLibHandle lib)
{
    x509_NameEntry *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_NameEntry* )mmalloc(lib, sizeof(x509_NameEntry));
        rslt->object = NULL;
        rslt->set = 0;
        rslt->value = asn1_CreateString(lib);
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeNameEntry(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Name *x509_CreateName(PDFLibHandle lib)
{
    x509_Name *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_Name* )mmalloc(lib, sizeof(x509_Name));
        rslt->entries = vector_Create(lib);
        rslt->bytes = buffer_Create(lib);
        rslt->modified = 1;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeName(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposeNameEntry(PDFLibHandle lib, x509_NameEntry *entry)
{
    if (entry == NULL) return;
    asn1_DisposeObject(lib, entry->object);
    asn1_DisposeString(lib, entry->value);
    mfree(lib, entry);
}

void x509_DisposeName(PDFLibHandle lib, x509_Name *name)
{
    if (name == NULL) return;
    buffer_Dispose(name->bytes);
    vector_Dispose(name->entries, (void (* )(PDFLibHandle, void * ))x509_DisposeNameEntry);
    mfree(lib, name);
}



ppInt32 x509_EncodeValidity(x509_Validity *val, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (val == NULL) return 0;
    rslt += asn1_EncodeTime(val->notBefore, NULL);
    rslt += asn1_EncodeTime(val->notAfter, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeTime(val->notBefore, &p);
    asn1_EncodeTime(val->notAfter, &p);
    (*pbuf) = p;
    return r;
}

x509_Validity *x509_DecodeValidity(PDFLibHandle lib, x509_Validity **pval, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_Validity *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q= *pbuf;

        if (pval == NULL || (*pval) == NULL)
            rslt = x509_CreateValidity(lib);
        else
            rslt = *pval;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeTime(lib, &(rslt->notBefore), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeTime(lib, &(rslt->notAfter), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pval != NULL) (*pval) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pval == NULL || (*pval) != rslt))
        {
            x509_DisposeValidity(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Validity *x509_CreateValidity(PDFLibHandle lib)
{
    x509_Validity *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_Validity* )mmalloc(lib, sizeof(x509_Validity));
        rslt->notBefore = asn1_CreateUTCTime(lib);
        rslt->notAfter = asn1_CreateUTCTime(lib);
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeValidity(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return  rslt;
}

void x509_DisposeValidity(PDFLibHandle lib, x509_Validity *val)
{
    if (val == NULL) return;
    asn1_DisposeString(lib, (asn1_String* )val->notBefore);
    asn1_DisposeString(lib, (asn1_String* )val->notAfter);
    mfree(lib, val);
}



ppInt32 x509_EncodePublicKey(x509_PublicKey *key, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (key == NULL) return 0;
    rslt += x509_EncodeAlgorithm(key->algorithm, NULL);
    rslt += asn1_EncodeBitString(key->publicKey, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    x509_EncodeAlgorithm(key->algorithm, &p);
    asn1_EncodeBitString(key->publicKey, &p);
    (*pbuf) = p;
    return r;
}

x509_PublicKey *x509_DecodePublicKey(PDFLibHandle lib, x509_PublicKey **pkey, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_PublicKey *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pkey == NULL || (*pkey) == NULL)
            rslt = x509_CreatePublicKey(lib);
        else
            rslt = *pkey;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->algorithm), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeBitString(lib, &(rslt->publicKey), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pkey != NULL) (*pkey) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pkey == NULL || (*pkey) != rslt))
        {
            x509_DisposePublicKey(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_PublicKey *x509_CreatePublicKey(PDFLibHandle lib)
{
    x509_PublicKey *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_PublicKey* )mmalloc(lib, sizeof(x509_PublicKey));
        rslt->algorithm = x509_CreateAlgorithm(lib);
        rslt->publicKey = asn1_CreateBitString(lib);
        rslt->rsaKey = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposePublicKey(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposePublicKey(PDFLibHandle lib, x509_PublicKey *key)
{
    if (key == NULL) return;
    x509_DisposeAlgorithm(lib, key->algorithm);
    asn1_DisposeString(lib, (asn1_String* )key->publicKey);
    mfree(lib, key);
}



ppInt32 x509_EncodeExtension( void *vext, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    x509_Extension *ext = ( x509_Extension * ) vext;

    if (ext == NULL) return 0;
    rslt += asn1_EncodeObject(ext->object, NULL);
    if (ext->critical)
        rslt += asn1_EncodeBoolean(ext->critical, NULL);
    rslt += asn1_EncodeOctetString(ext->value, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(ext->object, &p);
    if (ext->critical)
        asn1_EncodeBoolean(ext->critical, &p);
    asn1_EncodeOctetString(ext->value, &p);
    (*pbuf) = p;
    return r;
}

x509_Extension *x509_DecodeExtension(PDFLibHandle lib, x509_Extension **pext, ppUns8 **pbuf, ppInt64 max)
{
    ppInt32 i;
    asn1_Context ctx;
    x509_Extension *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pext == NULL || (*pext) == NULL)
            rslt = x509_CreateExtension(lib);
        else
            rslt = *pext;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->object), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (BOOLEAN_TAG|UNIVERSAL_CLS))
        {
            ctx.q = ctx.p;
            asn1_DecodeBoolean(lib, &i, &ctx.p, ctx.len);
            rslt->critical = (ppInt16)i;
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->value), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pext != NULL) (*pext) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pext == NULL || (*pext) != rslt))
        {
            x509_DisposeExtension(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib)
        return rslt;
}

x509_Extension *x509_CreateExtension(PDFLibHandle lib)
{
    x509_Extension *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_Extension* )mmalloc(lib, sizeof(x509_Extension));
        rslt->object = NULL;
        rslt->value = asn1_CreateOctetString(lib);
        rslt->critical = 0;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeExtension(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposeExtension(PDFLibHandle lib, x509_Extension *ext)
{
    if (ext == NULL) return;
    asn1_DisposeObject(lib, ext->object);
    asn1_DisposeOctetString(lib, ext->value);
    mfree(lib, ext);
}



ppInt32 x509_EncodeAttribute(x509_Attribute *attr, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p = NULL;

    if (attr == NULL) return 0;
    rslt += asn1_EncodeObject(attr->object, NULL);
    if (attr->set)
        rslt += /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(asn1_Type*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))*/asn1_EncodeSet
        (attr->value.set, NULL, (ppInt32 (* )(void *, ppUns8 ** ))asn1_EncodeType, SET_TAG, UNIVERSAL_CLS, IS_SET);
    else
        rslt += asn1_EncodeType(attr->value.single, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(attr->object, &p);
    if (attr->set)
        /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(asn1_Type*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))*/asn1_EncodeSet
        (attr->value.set, &p, (ppInt32 (* )(void *, ppUns8 ** ))asn1_EncodeType, SET_TAG, UNIVERSAL_CLS, IS_SET);
    else
        asn1_EncodeType(attr->value.single, &p);
    (*pbuf) = p;
    return r;
}

x509_Attribute *x509_DecodeAttribute(PDFLibHandle lib, x509_Attribute **pattr, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_Attribute *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pattr == NULL || (*pattr) == NULL)
            rslt = x509_CreateAttribute(lib);
        else
            rslt = *pattr;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->object), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && *(ctx.p) == (CONSTRUCTED|UNIVERSAL_CLS|SET_TAG))
        {
            rslt->set = 1;
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->value.set), &ctx.p, ctx.len,
                (TDecodeFunction)asn1_DecodeType,
                (TDisposeFunction)asn1_DisposeType,
                SET_TAG, UNIVERSAL_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }
        else
        {
            rslt->set = 0;
            ctx.q = ctx.p;
            asn1_DecodeType(lib, &(rslt->value.single), &ctx.p, ctx.len);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx); 
        (*pbuf) = ctx.p;
        if (pattr != NULL) (*pattr) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pattr == NULL || (*pattr) != rslt))
        {
            x509_DisposeAttribute(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Attribute *x509_CreateAttribute(PDFLibHandle lib)
{
    x509_Attribute *rslt;
    rslt = (x509_Attribute* )mmalloc(lib, sizeof(x509_Attribute));
    rslt->object = NULL;
    rslt->set = 0;
    rslt->value.ptr = NULL;
    return rslt;
}

void x509_DisposeAttribute(PDFLibHandle lib, x509_Attribute *attr)
{
    if (attr == NULL) return;
    asn1_DisposeObject(lib, attr->object);
    if (attr->set)
        vector_Dispose(attr->value.set, (void (* )(PDFLibHandle, void * ))asn1_DisposeType);
    else
        asn1_DisposeType(lib, attr->value.single);
    mfree(lib, attr);
}

rsa_Key *x509_GetPublicKey(PDFLibHandle lib, x509 *cert)
{
    rsa_Key *rslt = NULL;
    ppInt64 j;
    ppInt32 type;
    ppUns8 *p;

    if (cert == NULL || cert->cert_info == NULL ||
        cert->cert_info->key == NULL || cert->cert_info->key->publicKey == NULL) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));

    if (cert->cert_info->key->rsaKey != NULL)
        return cert->cert_info->key->rsaKey;

    PDFTRY(lib){
        type = asn1_GetObjectID(cert->cert_info->key->algorithm->algorithm);
        if (type != ID_rsaEncryption)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
        p = cert->cert_info->key->publicKey->data;
        j = cert->cert_info->key->publicKey->length;
        rslt = rsa_DecodePublicKey(lib, NULL, &p, j);
        cert->cert_info->key->rsaKey = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL) rsa_DisposeKey(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

char *x509_GetSubjectCommonName(PDFLibHandle lib, x509 *cert)
{
    x509_Name *name;
    x509_NameEntry *entry = NULL;
    ppInt32 i;
    char *rslt = NULL;

    name = cert->cert_info->subject;
    for(i = 0; i < vector_GetCount(name->entries); i++)
    {
        entry = (x509_NameEntry* )vector_GetValueAt(name->entries, i);
        if (asn1_GetObjectID(entry->object) == ID_commonName) break;
    }
    if (asn1_GetObjectID(entry->object) == ID_commonName)
    {
        rslt = ( char * ) mmalloc(lib, entry->value->length + 1);
        memset(rslt, 0, entry->value->length + 1);
        memcpy(rslt, entry->value->data, entry->value->length);
    }
    return rslt;
}
ppInt32 x509_EncodeRevoked(x509_Revoked *cert, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (cert == NULL) return 0;

    rslt += asn1_EncodeInteger(cert->serialNumber, NULL);
    rslt += asn1_EncodeTime(cert->revocationDate, NULL);
    if (cert->extensions) 
        rslt += /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(void *, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))*/asn1_EncodeSet
        (cert->extensions, NULL, (ppInt32 (* )(void *, ppUns8 ** ))x509_EncodeExtension, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(cert->serialNumber, &p);
    asn1_EncodeTime(cert->revocationDate, &p);
    if (cert->extensions) 
        /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(void *, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))*/asn1_EncodeSet
        (cert->extensions, &p, (ppInt32 (* )(void *, ppUns8 ** ))x509_EncodeExtension, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
    (*pbuf) = p;
    return r;
}

x509_Revoked *x509_DecodeRevoked(PDFLibHandle lib, x509_Revoked **pcert, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_Revoked *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pcert == NULL || (*pcert) == NULL)
            rslt = x509_CreateRevoked(lib); 
        else
            rslt = *pcert;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->serialNumber), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeTime(lib, &(rslt->revocationDate), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (UNIVERSAL_CLS|CONSTRUCTED|SEQUENCE_TAG))
        { 
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->extensions), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_DecodeExtension, 
                (TDisposeFunction)x509_DisposeExtension, 
                SEQUENCE_TAG, UNIVERSAL_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pcert != NULL) (*pcert) = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pcert == NULL || (*pcert) != rslt)) 
        {
            x509_DisposeRevoked(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_Revoked *x509_CreateRevoked(PDFLibHandle lib)
{
    x509_Revoked *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_Revoked* )mmalloc(lib, sizeof(x509_Revoked));
        rslt->serialNumber = asn1_CreateInteger(lib);
        rslt->revocationDate = asn1_CreateUTCTime(lib);
        rslt->extensions = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeRevoked(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposeRevoked(PDFLibHandle lib, x509_Revoked *cert)
{
    if (cert == NULL) return;
    asn1_DisposeInteger(lib, cert->serialNumber);
    asn1_DisposeUTCTime(lib, cert->revocationDate);
    vector_Dispose(cert->extensions, (void (* )(PDFLibHandle, void * ))x509_DisposeExtension);
    mfree(lib, cert);
}

/*
!!! WE ARE NOT PLANNING TO STORE X.509 CRLS WHERE THIS MAY BE USED !!!

static ppInt32 x509_CompareRevoked(const x509_Revoked *const *a, const x509_Revoked *const *b)
{
return asn1_CompareString((asn1_String* )(*a)->serialNumber, (asn1_String* )(*b)->serialNumber));
}

static ppInt32 x509_CompareRevokedSequence(const x509_Revoked *const *a, const x509_Revoked *const *b)
{
return ((*a)->sequence - (*b)->sequence);
}
*/

ppInt32 x509_EncodeCRLInfo(x509_CRLInfo *info, ppUns8 **pbuf)
{
    ppInt32 v1 = 0;
    ppInt64 l = 0;

    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (info == NULL) return 0;

    /*
    !!! WE ARE NOT PLANNING TO STORE X.509 CRLS WHERE THIS MAY BE USED !!!
    ppInt32 (*old_cmp)(const x509_Revoked *const*, const x509_Revoked *const* );
    old_cmp = SetStackCmpFunc(info->revoked, x509_CompareRevokedSequence);
    StackSort(info->revoked);
    SetStackCmpFunc(info->revoked, old_cmp);
    */
    if ((info->version != NULL) && ((l = asn1_GetInteger(info->version)) != 0))
        rslt += asn1_EncodeInteger(info->version, NULL);
    rslt += x509_EncodeAlgorithm(info->signature, NULL);
    rslt += x509_EncodeName(info->issuer, NULL);
    rslt += asn1_EncodeTime(info->thisUpdate, NULL);
    if (info->nextUpdate != NULL)
        rslt += asn1_EncodeTime(info->nextUpdate, NULL);
    if (info->revokedCertificates != NULL && vector_GetCount(info->revokedCertificates) != 0)
        rslt += /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Revoked*, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))*/asn1_EncodeSet
        (info->revokedCertificates, NULL, (ppInt32 (* )(void *, ppUns8 ** ))x509_EncodeRevoked, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
    if (info->crlExtensions)
    {
        v1 = ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(void *, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))asn1_EncodeSet)
            (info->crlExtensions, NULL, x509_EncodeExtension, 0, UNIVERSAL_CLS, IS_SEQUENCE);
        rslt += asn1_ObjectSize(1, v1, SEQUENCE_TAG);
    }
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    if ((info->version != NULL) && (l != 0))
        asn1_EncodeInteger(info->version, &p);
    x509_EncodeAlgorithm(info->signature, &p);
    x509_EncodeName(info->issuer, &p);
    asn1_EncodeTime(info->thisUpdate, &p);
    if (info->nextUpdate != NULL)
        asn1_EncodeTime(info->nextUpdate, &p);
    if (info->revokedCertificates != NULL && vector_GetCount(info->revokedCertificates) != 0)
        /*((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Revoked*, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))*/asn1_EncodeSet
        (info->revokedCertificates, &p, (ppInt32 (* )(void *, ppUns8 ** ))x509_EncodeRevoked, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
    if (info->crlExtensions)
    {
        asn1_PutObject(&p, 1, v1, SEQUENCE_TAG, CTX_SPECIFIC_CLS);
        ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(void *, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))asn1_EncodeSet)
            (info->crlExtensions, &p, x509_EncodeExtension, 0, UNIVERSAL_CLS, IS_SEQUENCE);
    }
    (*pbuf) = p;
    return r;
}

x509_CRLInfo *x509_DecodeCRLInfo(PDFLibHandle lib, x509_CRLInfo **pinfo, ppUns8 **pbuf, ppInt64 max)
{
    ppInt32 i, ver = 0;
    asn1_Context ctx;
    x509_CRLInfo *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pinfo == NULL || (*pinfo) == NULL)
            rslt = x509_CreateCRLInfo(lib); 
        else
            rslt = *pinfo;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        if (ctx.len != 0 && ((*ctx.p) & (~CONSTRUCTED)) == (UNIVERSAL_CLS|INTEGER_TAG))
        {
            ctx.q = ctx.p;
            asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len); 
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }
        if (rslt->version != NULL)
            ver = rslt->version->data[0];
        if (ver == 0 && rslt->version != NULL)
        {
            asn1_DisposeInteger(lib, rslt->version);
            rslt->version = NULL;
        }

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->signature), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeName(lib, &(rslt->issuer), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeTime(lib, &(rslt->thisUpdate), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && ((*ctx.p) & (~CONSTRUCTED)) == (UNIVERSAL_CLS|UTCTIME_TAG))
        {
            ctx.q = ctx.p;
            asn1_DecodeUTCTime(lib, &(rslt->nextUpdate), &ctx.p, ctx.len); 
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }
        if(!rslt->nextUpdate) 
            if (ctx.len != 0 && ((*ctx.p) & (~CONSTRUCTED)) == (UNIVERSAL_CLS|GENERALIZEDTIME_TAG))
            {
                ctx.q = ctx.p;
                asn1_DecodeGeneralizedTime(lib, &(rslt->nextUpdate), &ctx.p, ctx.len); 
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
            }

            if (rslt->revokedCertificates != NULL)
                while (vector_GetCount(rslt->revokedCertificates))
                    x509_DisposeRevoked(lib, (x509_Revoked* )vector_Pop(rslt->revokedCertificates));
            if (ctx.len != 0 && (*ctx.p) == (UNIVERSAL_CLS|CONSTRUCTED|SEQUENCE_TAG))
            { 
                ctx.q = ctx.p;
                asn1_DecodeSet(lib, &(rslt->revokedCertificates), &ctx.p, ctx.len, 
                    (TDecodeFunction)x509_DecodeRevoked, (TDisposeFunction)x509_DisposeRevoked, 
                    SEQUENCE_TAG, UNIVERSAL_CLS);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
            }
            if (rslt->revokedCertificates != NULL)
                for (i = 0; i < vector_GetCount(rslt->revokedCertificates); i++)
                    ((x509_Revoked* )vector_GetValueAt(rslt->revokedCertificates, i))->sequence = i;

            if (rslt->crlExtensions != NULL)
                while (vector_GetCount(rslt->crlExtensions) > 0)
                    x509_DisposeExtension(lib, (x509_Extension* )vector_Pop(rslt->crlExtensions));
            if (ctx.len != 0 && (*ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/)) 
            {
                ppInt32 ainf,atag,acls;
                ppInt64 alen;

                ctx.q = ctx.p;
                ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                if (ainf & 0x80)
                    PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                if (ainf == (CONSTRUCTED + 1))
                    alen = ctx.len - (ctx.p - ctx.q) - 2;

                asn1_DecodeSet(lib, &(rslt->crlExtensions), &ctx.p, ctx.len, 
                    (TDecodeFunction)x509_DecodeExtension, (TDisposeFunction)x509_DisposeExtension, 
                    SEQUENCE_TAG, UNIVERSAL_CLS);

                if (ainf == (CONSTRUCTED | 1)) 
                {
                    alen = ctx.len - (ctx.p - ctx.q);
                    if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                }
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
            }

            asn1_Finish(lib, &ctx);
            (*pbuf) = ctx.p;
            if (pinfo != NULL) (*pinfo) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pinfo == NULL || (*pinfo) != rslt)) 
        {
            x509_DisposeCRLInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_CRLInfo *x509_CreateCRLInfo(PDFLibHandle lib)
{
    x509_CRLInfo *rslt = NULL;
    PDFTRY(lib) {
        rslt = (x509_CRLInfo* )mmalloc(lib, sizeof(x509_CRLInfo));
        rslt->version = NULL;
        rslt->signature = x509_CreateAlgorithm(lib);
        rslt->issuer = x509_CreateName(lib);
        rslt->thisUpdate = asn1_CreateUTCTime(lib);
        rslt->nextUpdate = NULL;
        rslt->revokedCertificates = vector_Create(lib);
        rslt->crlExtensions = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeCRLInfo(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void x509_DisposeCRLInfo(PDFLibHandle lib, x509_CRLInfo *info)
{
    if (info == NULL) return;
    asn1_DisposeInteger(lib, info->version);
    x509_DisposeAlgorithm(lib, info->signature);
    x509_DisposeName(lib, info->issuer);
    asn1_DisposeUTCTime(lib, info->thisUpdate);
    if (info->nextUpdate)
        asn1_DisposeUTCTime(lib, info->nextUpdate);
    vector_Dispose(info->revokedCertificates, (void (* )(PDFLibHandle, void * ))x509_DisposeRevoked);
    vector_Dispose(info->crlExtensions, (void (* )(PDFLibHandle, void * ))x509_DisposeExtension);
    mfree(lib, info);
}



ppInt32 x509_EncodeCRL(x509_CRL *crl, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (crl == NULL) return 0;

    rslt += x509_EncodeCRLInfo(crl->certList, NULL);
    rslt += x509_EncodeAlgorithm(crl->signatureAlgorithm, NULL);
    rslt += asn1_EncodeBitString(crl->signatureValue, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    x509_EncodeCRLInfo(crl->certList, &p);
    x509_EncodeAlgorithm(crl->signatureAlgorithm, &p);
    asn1_EncodeBitString(crl->signatureValue, &p);
    (*pbuf) = p;
    return r;
}

x509_CRL *x509_DecodeCRL(PDFLibHandle lib, x509_CRL **pcrl, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    x509_CRL *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pcrl == NULL || (*pcrl) == NULL)
            rslt = x509_CreateCRL(lib);
        else
            rslt = *pcrl;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        x509_DecodeCRLInfo(lib, &(rslt->certList), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->signatureAlgorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeBitString(lib, &(rslt->signatureValue), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pcrl != NULL) (*pcrl) = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pcrl == NULL || (*pcrl) != rslt))
        {
            x509_DisposeCRL(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

x509_CRL *x509_CreateCRL(PDFLibHandle lib)
{
    x509_CRL *rslt = NULL;
    PDFTRY(lib){
        rslt = (x509_CRL* )mmalloc(lib, sizeof(x509_CRL));
        rslt->certList = x509_CreateCRLInfo(lib);
        rslt->signatureAlgorithm = x509_CreateAlgorithm(lib);
        rslt->signatureValue = asn1_CreateBitString(lib);
    } PDFEXCEPT(lib){
        if (rslt != NULL) x509_DisposeCRL(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib)
        return rslt;
}

void x509_DisposeCRL(PDFLibHandle lib, x509_CRL *crl)
{
    if (crl == NULL) return;
    x509_DisposeCRLInfo(lib, crl->certList);
    x509_DisposeAlgorithm(lib, crl->signatureAlgorithm);
    asn1_DisposeBitString(lib, crl->signatureValue);
    mfree(lib, crl);
}

#endif