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
VSkPKCS7.c
*********************************************************************/

#include "VSkPKCS7I.h"
#include <string.h>

#ifndef NOT_USE_SIGN

ppInt32 pkcs7_DataType(asn1_Object *type)
{
    ppInt32 i;
    if (type == NULL) 
        return ID_undef;
    if (type->length != pkcs7_TypeLength) 
        return ID_undef;
    if (memcmp(pkcs7_TypeBegin, type->data, pkcs7_TypeLength - 1)) 
        return ID_undef;
    for (i = 0; i < pkcs7_TypeCount; i++)
        if (pkcs7_TypeEnds[i] == type->data[pkcs7_TypeLength - 1]) 
            return pkcs7_TypeOIDs[i];
    return ID_undef;
}

/*'I' means indefinite encoding */
ppInt32 pkcs7_EncodeI(pkcs7 *obj, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (obj == NULL) return 0;

    rslt += 4;
    rslt += asn1_EncodeObject(obj->contentType, NULL);
    if (obj->contents.ptr != NULL)
    {
        rslt += 4;
        switch (asn1_GetObjectID(obj->contentType))
        {
        case ID_pkcs7Data:
            rslt += asn1_EncodeOctetString(obj->contents.data, NULL);
            break;
        case ID_pkcs7SignedData:
            rslt += pkcs7_EncodeSignedData(obj->contents.signedData, NULL);
            break;
        case ID_pkcs7EnvelopedData:
            /*rslt += pkcs7_EncodeEnvelopedData(obj->contents.envelopedData, NULL);*/
            break;
        case ID_pkcs7SignedAndEnvelopedData:
            /*rslt += pkcs7_EncodeSignedAndEnvelopedData(obj->contents.signedAndEnvelopedData, NULL);*/
            break;
        case ID_pkcs7DigestedData:
            /*rslt += pkcs7_EncodeDigestedData(obj->contents.digestedData, NULL);*/
            break;
        case ID_pkcs7EncryptedData:
            rslt += pkcs7_EncodeEncryptedData(obj->contents.encryptedData, NULL);
            break;
        default:
            rslt += asn1_EncodeType(obj->contents.other, NULL);
            break;
        }
    }
    r = rslt;
    if (pbuf == NULL) return r;

    p = *pbuf;
    *(p++) = CONSTRUCTED | SEQUENCE_TAG | UNIVERSAL_CLS;
    *(p++) = 0x80;
    asn1_EncodeObject(obj->contentType, &p);

    if (obj->contents.ptr != NULL)
    {
        *(p++) = CONSTRUCTED | CTX_SPECIFIC_CLS;
        *(p++) = 0x80;
        switch (asn1_GetObjectID(obj->contentType))
        {
        case ID_pkcs7Data:
            asn1_EncodeOctetString(obj->contents.data, &p);
            break;
        case ID_pkcs7SignedData:
            pkcs7_EncodeSignedData(obj->contents.signedData, &p);
            break;
        case ID_pkcs7EnvelopedData:
            /*pkcs7_EncodeEnvelopedData(obj->contents.envelopedData, &p);*/
            break;
        case ID_pkcs7SignedAndEnvelopedData:
            /*pkcs7_EncodeSignedAndEnvelopedData(obj->contents.signedAndEnvelopedData, &p);*/
            break;
        case ID_pkcs7DigestedData:
            /*pkcs7_EncodeDigestedData(obj->contents.digestedData, &p);*/
            break;
        case ID_pkcs7EncryptedData:
            pkcs7_EncodeEncryptedData(obj->contents.encryptedData, &p);
            break;
        default:
            asn1_EncodeType(obj->contents.other, &p);
            break;
        }
        *(p++) = 0x00; *(p++) = 0x00;
    }
    *(p++) = 0x00; *(p++) = 0x00;

    (*pbuf) = p;
    return r;
}

/*'D' means definite encoding */
ppInt32 pkcs7_EncodeD(pkcs7 *obj, ppUns8 **pbuf)
{
    ppInt32 explen = 0;
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (obj == NULL) return 0;

    rslt += asn1_EncodeObject(obj->contentType, NULL);
    if (obj->contents.ptr != NULL)
    {
        r = rslt;
        switch (asn1_GetObjectID(obj->contentType))
        {
        case ID_pkcs7Data:
            rslt += asn1_EncodeOctetString(obj->contents.data, NULL);
            break;
        case ID_pkcs7SignedData:
            rslt += pkcs7_EncodeSignedData(obj->contents.signedData, NULL);
            break;
        case ID_pkcs7EnvelopedData:
            /*rslt += pkcs7_EncodeEnvelopedData(obj->contents.envelopedData, NULL);*/
            break;
        case ID_pkcs7SignedAndEnvelopedData:
            /*rslt += pkcs7_EncodeSignedAndEnvelopedData(obj->contents.signedAndEnvelopedData, NULL);*/
            break;
        case ID_pkcs7DigestedData:
            /*rslt += pkcs7_EncodeDigestedData(obj->contents.digestedData, NULL);*/
            break;
        case ID_pkcs7EncryptedData:
            rslt += pkcs7_EncodeEncryptedData(obj->contents.encryptedData, NULL);
            break;
        default:
            rslt += asn1_EncodeType(obj->contents.other, NULL);
            break;
        }
        explen = rslt - r;
        rslt =  r + asn1_ObjectSize(1, explen, 0);
    }

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(obj->contentType, &p);

    if (obj->contents.ptr != NULL)
    {
        asn1_PutObject(&p, 1, explen, 0, CTX_SPECIFIC_CLS);
        switch (asn1_GetObjectID(obj->contentType))
        {
        case ID_pkcs7Data:
            asn1_EncodeOctetString(obj->contents.data, &p);
            break;
        case ID_pkcs7SignedData:
            pkcs7_EncodeSignedData(obj->contents.signedData, &p);
            break;
        case ID_pkcs7EnvelopedData:
            /*pkcs7_EncodeEnvelopedData(obj->contents.envelopedData, &p);*/
            break;
        case ID_pkcs7SignedAndEnvelopedData:
            /*pkcs7_EncodeSignedAndEnvelopedData(obj->contents.signedAndEnvelopedData, &p);*/
            break;
        case ID_pkcs7DigestedData:
            /*pkcs7_EncodeDigestedData(obj->contents.digestedData, &p);*/
            break;
        case ID_pkcs7EncryptedData:
            pkcs7_EncodeEncryptedData(obj->contents.encryptedData, &p);
            break;
        default:
            asn1_EncodeType(obj->contents.other, &p);
            break;
        }
    }

    (*pbuf) = p;
    return r;
}

pkcs7 *pkcs7_Decode(PDFLibHandle lib, pkcs7 **pobj, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7 *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pobj == NULL || (*pobj) == NULL)
            rslt = pkcs7_Create(lib);
        else
            rslt = *pobj;

        if (pobj != NULL && (*pobj) != NULL)
        {
            if ((*pobj)->asn1 != NULL)
            {
                mfree(lib, (*pobj)->asn1);
                (*pobj)->asn1 = NULL;
            }
            (*pobj)->length = 0;
        }

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->contentType), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        rslt->detached = asn1_CheckEndSequence(&ctx);
        if (!rslt->detached)
        {
            ppInt32 ainf, atag, acls;
            ppInt64 alen;

            if ((*ctx.p) != (CONSTRUCTED | CTX_SPECIFIC_CLS))
                PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));

            ctx.q = ctx.p;
            ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, (ctx.inf & 1) ? max + (*pbuf - ctx.q) : ctx.len);
            if (ainf & 0x80) 
                PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
            ctx.len -= (ppInt64)(ctx.p - ctx.q);

            switch (pkcs7_DataType(rslt->contentType))
            {
            case ID_pkcs7Data:
                ctx.q = ctx.p;
                asn1_DecodeOctetString(lib, &(rslt->contents.data), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                break;
            case ID_pkcs7SignedData:
                ctx.q = ctx.p;
                pkcs7_DecodeSignedData(lib, &(rslt->contents.signedData), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                if (rslt->contents.signedData->contentInfo->contents.ptr == NULL) 
                    rslt->detached = 1;
                break;
            case ID_pkcs7EnvelopedData:
                /*
                ctx.q = ctx.p;
                pkcs7_DecodeEnvelopedData(lib, &(rslt->contents.envelopedData), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                */
                break;
            case ID_pkcs7SignedAndEnvelopedData:
                /*
                ctx.q = ctx.p;
                pkcs7_DecodeSignedAndEnvelopedData(lib, &(rslt->contents.signedAndEnvelopedData), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                  */
                break;
            case ID_pkcs7DigestedData:
                /*
                ctx.q = ctx.p;
                pkcs7_DecodeDigestedData(lib, &(rslt->contents.digestedData), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                */
                break;
            case ID_pkcs7EncryptedData:
                ctx.q = ctx.p;
                pkcs7_DecodeEncryptedData(lib, &(rslt->contents.encryptedData), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                break;
            default:
                ctx.q = ctx.p;
                asn1_DecodeType(lib, &(rslt->contents.other), &ctx.p, ctx.len);
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
                break;
            }
            if (ainf == (1 | CONSTRUCTED))
            {
                ctx.q = ctx.p;
                if (!asn1_CheckInfiniteEnd(&ctx.p, ctx.len))
                    PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                ctx.len -= (ppInt64)(ctx.p - ctx.q);
            }
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pobj != NULL) (*pobj) = rslt;
    }PDFEXCEPT(lib) {
        if (rslt != NULL && (pobj == NULL || (*pobj) != rslt))
        {
            pkcs7_Dispose(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

pkcs7 *pkcs7_Create(PDFLibHandle lib)
{
    pkcs7 *rslt;
    rslt = (pkcs7* )mmalloc(lib, sizeof(pkcs7));
    rslt->contentType = NULL; /*undef*/
    rslt->asn1 = NULL;
    rslt->length = 0;
    rslt->detached = 0;
    rslt->contents.ptr = NULL;
    return rslt;
}

void pkcs7_Dispose(PDFLibHandle lib, pkcs7 *obj)
{
    if (obj == NULL) return;
    pkcs7_DisposeContent(lib, obj);
    if (obj->contentType != NULL)
        asn1_DisposeObject(lib, obj->contentType);
    mfree(lib, obj);
}

void pkcs7_DisposeContent(PDFLibHandle lib,pkcs7 *obj)
{
    if (obj == NULL) return;

    if (obj->asn1 != NULL) mfree(lib, obj->asn1);
    if (obj->contentType == NULL) return;   
    if (obj->contents.ptr != NULL)
    {
        switch (pkcs7_DataType(obj->contentType))
        {
        case ID_pkcs7Data:
            asn1_DisposeOctetString( lib, obj->contents.data);
            break;
        case ID_pkcs7SignedData:
            pkcs7_DisposeSignedData(lib, obj->contents.signedData);
            break;
        case ID_pkcs7EnvelopedData:
            /*
            pkcs7_DisposeEnvelopedData(obj->contents.envelopedData);
            */
            break;
        case ID_pkcs7SignedAndEnvelopedData:
            /*
            pkcs7_DisposeSignedAndEnvelopedData(obj->contents.signedAndEnvelopedData);
            */
            break;
        case ID_pkcs7DigestedData:
            /*
            pkcs7_DisposeDigestedData(obj->contents.digestedData);
            */
            break;
        case ID_pkcs7EncryptedData:
            pkcs7_DisposeEncryptedData(lib, obj->contents.encryptedData);
            break;
        default:
            asn1_DisposeType(lib, obj->contents.other);
            break;
        }
    }
    obj->contents.ptr=NULL;
}



ppInt32 pkcs7_EncodeSignedData(pkcs7_SignedData *data, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (data == NULL) return 0;

    rslt += asn1_EncodeInteger(data->version, NULL);
    rslt += asn1_EncodeSet(data->digestAlgorithms, NULL, ( TEncodeFunction )x509_EncodeAlgorithm, SET_TAG, UNIVERSAL_CLS, IS_SET);
    rslt += pkcs7_EncodeD(data->contentInfo, NULL);
    if (data->certificates != NULL && vector_GetCount(data->certificates) != 0)
        rslt += asn1_EncodeSet(data->certificates, NULL, ( TEncodeFunction )x509_Encode, 0, CTX_SPECIFIC_CLS, IS_SEQUENCE);
    if (data->crls != NULL && vector_GetCount(data->crls) != 0)
        rslt += asn1_EncodeSet(data->crls, NULL, ( TEncodeFunction )x509_EncodeCRL, 1, CTX_SPECIFIC_CLS, IS_SET);
    rslt += asn1_EncodeSet(data->signerInfos, NULL, ( TEncodeFunction )pkcs7_EncodeSignerInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(data->version, &p);
    asn1_EncodeSet(data->digestAlgorithms, &p, ( TEncodeFunction )x509_EncodeAlgorithm, SET_TAG, UNIVERSAL_CLS, IS_SET);
    pkcs7_EncodeD(data->contentInfo, &p);
    if (data->certificates != NULL && vector_GetCount(data->certificates) != 0)
        asn1_EncodeSet(data->certificates, &p, ( TEncodeFunction )x509_Encode, 0, CTX_SPECIFIC_CLS, IS_SEQUENCE);
    if (data->crls != NULL && vector_GetCount(data->crls) != 0)
        asn1_EncodeSet(data->crls, &p, ( TEncodeFunction )x509_EncodeCRL, 1, CTX_SPECIFIC_CLS, IS_SET);
    asn1_EncodeSet(data->signerInfos, &p, ( TEncodeFunction )pkcs7_EncodeSignerInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
    (*pbuf) = p;
    return r;
}

pkcs7_SignedData *pkcs7_DecodeSignedData(PDFLibHandle lib, pkcs7_SignedData **pdata, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_SignedData *rslt = NULL;
    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pdata == NULL || (*pdata) == NULL)
            rslt = pkcs7_CreateSignedData(lib); 
        else
            rslt = *pdata;

        ctx.p = (*pbuf);
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeSet(lib, &(rslt->digestAlgorithms), &ctx.p, ctx.len, 
            (TDecodeFunction)x509_DecodeAlgorithm, (TDisposeFunction)x509_DisposeAlgorithm, 
            SET_TAG, UNIVERSAL_CLS);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        pkcs7_Decode(lib, &(rslt->contentInfo), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED/*|0*/))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->certificates), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_Decode, (TDisposeFunction)x509_Dispose, 
                0, CTX_SPECIFIC_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        if (ctx.len != 0 && (*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED/*|1*/))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->crls), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_DecodeCRL, (TDisposeFunction)x509_DisposeCRL, 
                1, CTX_SPECIFIC_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        ctx.q = ctx.p;
        asn1_DecodeSet(lib, &(rslt->signerInfos), &ctx.p, ctx.len, 
            (TDecodeFunction)pkcs7_DecodeSignerInfo, (TDisposeFunction)pkcs7_DisposeSignerInfo, 
            SET_TAG, UNIVERSAL_CLS);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pdata != NULL) (*pdata) = rslt;
    }PDFEXCEPT(lib){
        if (rslt != NULL && (pdata == NULL || (*pdata) != rslt)) 
        {
            pkcs7_DisposeSignedData(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

pkcs7_SignedData *pkcs7_CreateSignedData(PDFLibHandle lib)
{
    pkcs7_SignedData *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_SignedData* )mmalloc(lib, sizeof(pkcs7_SignedData));
        rslt->version = asn1_CreateInteger(lib);
        rslt->digestAlgorithms = vector_Create(lib);
        rslt->contentInfo = pkcs7_Create(lib);
        rslt->certificates = NULL;
        rslt->crls = NULL;
        rslt->signerInfos = vector_Create(lib);
    }PDFEXCEPT(lib){
        if (rslt != NULL) 
        {
            pkcs7_DisposeSignedData(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeSignedData(PDFLibHandle lib, pkcs7_SignedData *data)
{
    if (data == NULL) return;
    asn1_DisposeInteger(lib, data->version);
    vector_Dispose(data->digestAlgorithms, (DisposeFunc)x509_DisposeAlgorithm);
    pkcs7_Dispose(lib, data->contentInfo);
    vector_Dispose(data->certificates, (DisposeFunc)x509_Dispose);
    vector_Dispose(data->crls, (DisposeFunc)x509_DisposeCRL);
    vector_Dispose(data->signerInfos, (DisposeFunc)pkcs7_DisposeSignerInfo);
    mfree(lib, data);
}



ppInt32 pkcs7_EncodeEncryptedContent(pkcs7_EncryptedContent *data, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (data == NULL) return 0;
    rslt += asn1_EncodeObject(data->content_type, NULL);
    rslt += x509_EncodeAlgorithm(data->algorithm, NULL);
    if (data->enc_data != NULL) 
        rslt += asn1_EncodeOctetString(data->enc_data, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(data->content_type, &p);
    x509_EncodeAlgorithm(data->algorithm, &p);
    if (data->enc_data != NULL) 
        asn1_EncodeOctetString(data->enc_data, &p);
    (*pbuf) = p;
    return r;
}

pkcs7_EncryptedContent *pkcs7_DecodeEncryptedContent(PDFLibHandle lib, pkcs7_EncryptedContent **pdata, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_EncryptedContent *rslt = NULL;
    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (pdata == NULL || (*pdata) == NULL)
            rslt = pkcs7_CreateEncryptedContent(lib);
        else
            rslt = *pdata;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->content_type), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->algorithm), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && ((*ctx.p) & (~CONSTRUCTED)) == (CTX_SPECIFIC_CLS | 0))
        {
            ppUns8 t = (*ctx.p);
            (*ctx.p) = (t & CONSTRUCTED) | OCTETSTRING_TAG;
            ctx.q = ctx.p;
            asn1_DecodeOctetString(lib, &(rslt->enc_data), &ctx.p, ctx.len);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
            (*ctx.q) = t;
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pdata != NULL) (*pdata) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pdata == NULL || (*pdata) != rslt)) 
        {
            pkcs7_DisposeEncryptedContent(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib) ;
    return rslt;
}

pkcs7_EncryptedContent *pkcs7_CreateEncryptedContent(PDFLibHandle lib)
{
    pkcs7_EncryptedContent *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_EncryptedContent* )mmalloc(lib, sizeof(pkcs7_EncryptedContent));
        rslt->content_type = asn1_CreateObjectByID(lib, ID_pkcs7Data);
        rslt->algorithm = x509_CreateAlgorithm(lib);
        rslt->enc_data = NULL;
    }PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeEncryptedContent(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeEncryptedContent(PDFLibHandle lib, pkcs7_EncryptedContent *data)
{
    if (data == NULL) return;
    asn1_DisposeObject(lib, data->content_type);
    x509_DisposeAlgorithm(lib, data->algorithm);
    asn1_DisposeOctetString(lib, data->enc_data);
    mfree(lib, data);
}


/*
ppInt32 pkcs7_EncodeEnvelopedData(pkcs7_EnvelopedData *data, ppUns8 **pbuf)
{
ppInt32 r = 0, rslt = 0;
ppUns8 *p;
if (data == NULL) return 0;

rslt += asn1_EncodeInteger(data->version, NULL);
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_RecipientInfo*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->recipient_infos, NULL, pkcs7_EncodeRecipientInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
rslt += pkcs7_EncodeEncryptedContent(data->enc_data, NULL);

r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
if (pbuf == NULL) return r;

p = *pbuf;
asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

asn1_EncodeInteger(data->version, &p);
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_RecipientInfo*, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))asn1_EncodeSet)
(data->recipient_infos, &p, pkcs7_EncodeRecipientInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
pkcs7_EncodeEncryptedContent(data->enc_data, &p);

*pbuf = p;
return r;
}

pkcs7_EnvelopedData *pkcs7_DecodeEnvelopedData(pkcs7_EnvelopedData **pdata, ppUns8 **pbuf, ppInt64 max)
{
asn1_Context ctx;
pkcs7_EnvelopedData *rslt = NULL;
PDFTRY(pdf_lib)
ctx.pp = pbuf;
ctx.q = *pbuf;

if (pdata == NULL || (*pdata) == NULL)
rslt = pkcs7_CreateEnvelopedData();
else
rslt = (*pdata);

ctx.p = *pbuf;
ctx.max = (max == 0 ? 0 : ctx.p + max);
asn1_GetSequence(&ctx, &max);

ctx.q = ctx.p;
asn1_DecodeInteger(&(rslt->version), &ctx.p, ctx.len);
ctx.len -= (ctx.p - ctx.q);

asn1_DecodeSet(&(rslt->recipient_infos), &ctx.p, ctx.len, 
(char *(* )())pkcs7_DecodeRecipientInfo, 
(void (* )(void * ))pkcs7_DisposeRecipientInfo, 
SET_TAG, UNIVERSAL_CLS);

ctx.q = ctx.p;
pkcs7_DecodeEncryptedContent(&(rslt->enc_data), &ctx.p, ctx.len);
ctx.len -= (ctx.p - ctx.q);

asn1_Finish(&ctx);

(*pbuf) = ctx.p;
if (pdata != NULL) (*pdata) = rslt;
PDFEXCEPT(pdf_lib)
if (rslt != NULL && (pdata == NULL || (*pdata) != rslt)) 
{
pkcs7_DisposeEnvelopedData(rslt);
rslt = NULL;
}
PDFRERAISE(pdf_lib)
PDFTRYEND(pdf_lib)
return rslt;
}

pkcs7_EnvelopedData *pkcs7_CreateEnvelopedData(void)
{
pkcs7_EnvelopedData *rslt = NULL;
PDFTRY(pdf_lib)
rslt = (pkcs7_EnvelopedData* )mmalloc(pdf_lib, sizeof(pkcs7_EnvelopedData));
rslt->version = asn1_CreateInteger();
rslt->recipient_infos = vector_Create();
rslt->enc_data = pkcs7_CreateEncryptedContent();
PDFEXCEPT(pdf_lib)
if (rslt != NULL)
{
pkcs7_DisposeEnvelopedData(rslt);
rslt = NULL;
}
PDFRERAISE(pdf_lib)
PDFTRYEND(pdf_lib)
return rslt;
}

void pkcs7_DisposeEnvelopedData(pkcs7_EnvelopedData *data)
{
if (data == NULL) return;
asn1_DisposeInteger(data->version);
vector_Dispose(data->recipient_infos, (void (* )(void * ))pkcs7_DisposeRecipientInfo);
pkcs7_DisposeEncryptedContent(data->enc_data);
mfree(data);
}
*/

/*
ppInt32 pkcs7_EncodeSignedAndEnvelopedData(pkcs7_SignedAndEnvelopedData *data, ppUns8 **pbuf)
{
ppInt32 r = 0, rslt = 0;
ppUns8 *p;
if (data == NULL) return 0;

rslt += asn1_EncodeInteger(data->version, NULL);
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_RecipientInfo*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->recipient_infos, NULL, pkcs7_EncodeRecipientInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Algorithm*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->md_algs, NULL, x509_EncodeAlgorithm, SET_TAG, UNIVERSAL_CLS, IS_SET);
rslt += pkcs7_EncodeEncryptedContent(data->enc_data, NULL);
if (data->certs != NULL && vector_GetCount(data->certs) != 0)
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->certs, NULL, x509_Encode, 0, CTX_SPECIFIC_CLS, IS_SET);
if (data->crls != NULL && vector_GetCount(data->crls) != 0)
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_CRL*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->crls, NULL, x509_EncodeCRL, 1, CTX_SPECIFIC_CLS, IS_SET);
rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_SignerInfo*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->signer_infos, NULL, pkcs7_EncodeSignerInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);

r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
if (pbuf == NULL) return r;

p = *pbuf;
asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

asn1_EncodeInteger(data->version, &p);
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_RecipientInfo*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->recipient_infos, &p, pkcs7_EncodeRecipientInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Algorithm*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->md_algs, &p, x509_EncodeAlgorithm, SET_TAG, UNIVERSAL_CLS, IS_SET);
pkcs7_EncodeEncryptedContent(data->enc_data, &p);
if (data->certs != NULL && vector_GetCount(data->certs) != 0)
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->certs, &p, x509_Encode, 0, CTX_SPECIFIC_CLS, IS_SET);
if (data->crls != NULL && vector_GetCount(data->crls) != 0)
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_CRL*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->crls, &p, x509_EncodeCRL, 1, CTX_SPECIFIC_CLS, IS_SET);
((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs7_SignerInfo*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
(data->signer_infos, &p, pkcs7_EncodeSignerInfo, SET_TAG, UNIVERSAL_CLS, IS_SET);

*pbuf = p;
return r;
}

pkcs7_SignedAndEnvelopedData *pkcs7_DecodeSignedAndEnvelopedData(pkcs7_SignedAndEnvelopedData **pdata, ppUns8 **pbuf, ppInt64 max)
{
asn1_Context ctx;
pkcs7_SignedAndEnvelopedData *rslt = NULL;

ctx.pp = pbuf;
ctx.q = *pbuf;

if ((pdata == NULL) || ((*pdata) == NULL))
{
if ((rslt = pkcs7_CreateSignedAndEnvelopedData()) == NULL) goto err;
}
else
rslt = (*pdata);

ctx.p = *pbuf;
ctx.max = (max == 0 ? 0 : ctx.p + max);

if (!asn1_GetSequence(&ctx, &max)) goto err;

ctx.q = ctx.p;
if (asn1_DecodeInteger(&(rslt->version), &ctx.p, ctx.len) == NULL) goto err;
ctx.len -= (ctx.p - ctx.q);

ctx.q = ctx.p;
if (asn1_DecodeSet(&(rslt->recipient_infos), &ctx.p, ctx.len,
(char *(* )())pkcs7_DecodeRecipientInfo,
(void (* )(void * ))pkcs7_DisposeRecipientInfo, 
SET_TAG, UNIVERSAL_CLS) == NULL) goto err; 
ctx.len -= (ctx.p - ctx.q);

ctx.q = ctx.p;
if (asn1_DecodeSet(&(rslt->md_algs), &ctx.p, ctx.len,
(char *(* )())x509_DecodeAlgorithm,
(void (* )(void * ))x509_DisposeAlgorithm, 
SET_TAG, UNIVERSAL_CLS) == NULL) goto err; 
ctx.len -= (ctx.p - ctx.q);

ctx.q = ctx.p;
if (pkcs7_DecodeEncryptedContent(&(rslt->enc_data), &ctx.p, ctx.len) == NULL) goto err;
ctx.len -= (ctx.p - ctx.q);

if ((ctx.len != 0) && ((*ctx.p) == (CTX_SPECIFIC_CLS | CONSTRUCTED | 0)))
{
ctx.q = ctx.p;
if (asn1_DecodeSet(&(rslt->certs), &ctx.p, ctx.len,
(char *(* )())x509_Decode, (void (* )(void * ))x509_Dispose, 
0, CTX_SPECIFIC_CLS) == NULL) goto err; 
ctx.len -= (ctx.p - ctx.q);
}

if ((ctx.len != 0) && ((*ctx.p) == (CTX_SPECIFIC_CLS | CONSTRUCTED | 1)))
{
ctx.q = ctx.p;
if (asn1_DecodeSet(&(rslt->crls), &ctx.p, ctx.len,
(char *(* )())x509_DecodeCRL, 
(void (* )(void * ))x509_DisposeCRL, 
1, CTX_SPECIFIC_CLS) == NULL) goto err; 
ctx.len -= (ctx.p - ctx.q);
}

ctx.q = ctx.p;
if (asn1_DecodeSet(&(rslt->signer_infos), &ctx.p, ctx.len,
(char *(* )())pkcs7_DecodeSignerInfo,
(void (* )(void * ))pkcs7_DisposeSignerInfo, 
SET_TAG, UNIVERSAL_CLS) == NULL) goto err; 
ctx.len -= (ctx.p - ctx.q);

if (!asn1_Finish(&ctx)) goto err;
*pbuf = ctx.p;
if (pdata != NULL) (*pdata) = rslt;
return rslt;

err:
if ((rslt != NULL) && ((pdata == NULL) || (*pdata != rslt))) 
pkcs7_DisposeSignedAndEnvelopedData(rslt);
return NULL;
}

pkcs7_SignedAndEnvelopedData *pkcs7_CreateSignedAndEnvelopedData(void)
{
pkcs7_SignedAndEnvelopedData *rslt;
rslt = (pkcs7_SignedAndEnvelopedData* )malloc(sizeof(pkcs7_SignedAndEnvelopedData));
if (rslt != NULL)
{
rslt->version = asn1_CreateInteger();
rslt->recipient_infos = vector_Create();
rslt->md_algs = vector_Create();
rslt->enc_data = pkcs7_CreateEncryptedContent();
rslt->certs = NULL;
rslt->crls = NULL;
rslt->signer_infos = vector_Create();
if (rslt->version == NULL || rslt->recipient_infos == NULL ||
rslt->md_algs == NULL || rslt->enc_data == NULL || rslt->signer_infos == NULL)
{
pkcs7_DisposeSignedAndEnvelopedData(rslt);
rslt = NULL;
}
}
return rslt;
}

void pkcs7_DisposeSignedAndEnvelopedData(pkcs7_SignedAndEnvelopedData *data)
{
if (data == NULL) return;
asn1_DisposeInteger(data->version);
vector_Dispose(data->recipient_infos, (void (* )(void * ))pkcs7_DisposeRecipientInfo);
vector_Dispose(data->md_algs, (void (* )(void * ))x509_DisposeAlgorithm);
pkcs7_DisposeEncryptedContent(data->enc_data);
vector_Dispose(data->certs, (void (* )(void * ))x509_Dispose);
vector_Dispose(data->crls, (void (* )(void * ))x509_DisposeCRL);
vector_Dispose(data->signer_infos, (void (* )(void * ))pkcs7_DisposeSignerInfo);
mfree(data);
}
*/


ppInt32 pkcs7_EncodeEncryptedData(pkcs7_EncryptedData *data, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (data == NULL) return 0;

    rslt += asn1_EncodeInteger(data->version, NULL);
    rslt += pkcs7_EncodeEncryptedContent(data->enc_data, NULL);

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);

    asn1_EncodeInteger(data->version, &p);
    pkcs7_EncodeEncryptedContent(data->enc_data, &p);

    *pbuf = p;
    return r;
}

pkcs7_EncryptedData *pkcs7_DecodeEncryptedData(PDFLibHandle lib, pkcs7_EncryptedData **pdata, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_EncryptedData *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pdata == NULL || (*pdata) == NULL)
            rslt = pkcs7_CreateEncryptedData(lib);
        else
            rslt = *pdata;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len); 
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        pkcs7_DecodeEncryptedContent(lib, &(rslt->enc_data), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pdata != NULL) (*pdata) = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pdata == NULL || (*pdata) != rslt)) 
        {
            pkcs7_DisposeEncryptedData(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs7_EncryptedData *pkcs7_CreateEncryptedData(PDFLibHandle lib)
{
    pkcs7_EncryptedData *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_ENCD* )mmalloc(lib, sizeof(pkcs7_ENCD));
        rslt->version = asn1_CreateInteger(lib);
        rslt->enc_data = pkcs7_CreateEncryptedContent(lib);
    } PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeEncryptedData(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeEncryptedData(PDFLibHandle lib, pkcs7_EncryptedData *data)
{
    if (data == NULL) return;
    asn1_DisposeInteger(lib, data->version);
    pkcs7_DisposeEncryptedContent(lib, data->enc_data);
    mfree(lib, data);
}

ppInt32 pkcs7_EncodeSignerInfo(pkcs7_SI *si, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (si == NULL) return 0;
    rslt += asn1_EncodeInteger(si->version, NULL);
    rslt += pkcs7_EncodeIssuerAndSerialNumber(si->issuerAndSerialNumber, NULL);
    rslt += x509_EncodeAlgorithm(si->digestAlgorithm, NULL);
    if (si->authenticatedAttributes != NULL && vector_GetCount(si->authenticatedAttributes) != 0)
        rslt += asn1_EncodeSet(si->authenticatedAttributes, NULL, ( TEncodeFunction )x509_EncodeAttribute, 0, CTX_SPECIFIC_CLS, IS_SET);
    rslt += x509_EncodeAlgorithm(si->digestEncryptionAlgorithm, NULL);
    rslt += asn1_EncodeOctetString(si->encryptedDigest, NULL);
    if (si->unauthenticatedAttributes != NULL && vector_GetCount(si->unauthenticatedAttributes) != 0)
        rslt += asn1_EncodeSet(si->unauthenticatedAttributes, NULL, ( TEncodeFunction )x509_EncodeAttribute, 1, CTX_SPECIFIC_CLS, IS_SET);

    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;
    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(si->version, &p);
    pkcs7_EncodeIssuerAndSerialNumber(si->issuerAndSerialNumber, &p);
    x509_EncodeAlgorithm(si->digestAlgorithm, &p);
    if (si->authenticatedAttributes != NULL && vector_GetCount(si->authenticatedAttributes) != 0)
        asn1_EncodeSet(si->authenticatedAttributes, &p, ( TEncodeFunction )x509_EncodeAttribute, 0, CTX_SPECIFIC_CLS, IS_SET);
    x509_EncodeAlgorithm(si->digestEncryptionAlgorithm, &p);
    asn1_EncodeOctetString(si->encryptedDigest, &p);
    if (si->unauthenticatedAttributes != NULL && vector_GetCount(si->unauthenticatedAttributes) != 0)
        asn1_EncodeSet(si->unauthenticatedAttributes, &p, ( TEncodeFunction )x509_EncodeAttribute, 1, CTX_SPECIFIC_CLS, IS_SET);
    (*pbuf) = p;
    return r;
}

pkcs7_SI *pkcs7_DecodeSignerInfo(PDFLibHandle lib, pkcs7_SI **psi, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_SI *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (psi == NULL || (*psi) == NULL)
            rslt = pkcs7_CreateSignerInfo(lib); 
        else
            rslt = (*psi);

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        pkcs7_DecodeIssuerAndSerialNumber(lib, &(rslt->issuerAndSerialNumber), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->digestAlgorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED/*|0*/))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->authenticatedAttributes), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_DecodeAttribute, (TDisposeFunction)x509_DisposeAttribute, 
                0, CTX_SPECIFIC_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->digestEncryptionAlgorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->encryptedDigest), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED|1))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->unauthenticatedAttributes), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_DecodeAttribute, (TDisposeFunction)x509_DisposeAttribute, 
                1, CTX_SPECIFIC_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx); 
        (*pbuf) = ctx.p;
        if (psi != NULL) (*psi) = rslt;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (psi == NULL || (*psi) != rslt)) 
        {
            pkcs7_DisposeSignerInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }
    PDFTRYEND(lib)
        return rslt;
}

pkcs7_SI *pkcs7_CreateSignerInfo(PDFLibHandle lib)
{
    pkcs7_SI *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_SI* )mmalloc(lib, sizeof(pkcs7_SI));
        rslt->version = asn1_CreateInteger(lib);
        rslt->issuerAndSerialNumber = pkcs7_CreateIssuerAndSerialNumber(lib);
        rslt->digestAlgorithm = x509_CreateAlgorithm(lib);
        rslt->authenticatedAttributes = NULL;
        rslt->digestEncryptionAlgorithm = x509_CreateAlgorithm(lib);
        rslt->encryptedDigest = asn1_CreateOctetString(lib);
        rslt->unauthenticatedAttributes = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeSignerInfo(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeSignerInfo(PDFLibHandle lib, pkcs7_SI *si)
{
    if (si == NULL) return;
    asn1_DisposeInteger(lib, si->version);
    pkcs7_DisposeIssuerAndSerialNumber(lib, si->issuerAndSerialNumber);
    x509_DisposeAlgorithm(lib, si->digestAlgorithm);
    vector_Dispose(si->authenticatedAttributes, (DisposeFunc)x509_DisposeAttribute);
    x509_DisposeAlgorithm(lib, si->digestEncryptionAlgorithm);
    asn1_DisposeOctetString(lib, si->encryptedDigest);
    vector_Dispose(si->unauthenticatedAttributes, (DisposeFunc)x509_DisposeAttribute);
    mfree(lib, si);
}



ppInt32 pkcs7_EncodeRecipientInfo(pkcs7_RI *ri, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (ri == NULL) return 0;
    rslt += asn1_EncodeInteger(ri->version, NULL);
    rslt += pkcs7_EncodeIssuerAndSerialNumber(ri->issuerAndSerialNumber, NULL);
    rslt += x509_EncodeAlgorithm(ri->keyEncryptionAlgorithm, NULL);
    rslt += asn1_EncodeOctetString(ri->encryptedKey, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(ri->version, &p);
    pkcs7_EncodeIssuerAndSerialNumber(ri->issuerAndSerialNumber, &p);
    x509_EncodeAlgorithm(ri->keyEncryptionAlgorithm, &p);
    asn1_EncodeOctetString(ri->encryptedKey, &p);
    (*pbuf) = p;
    return r;
}

pkcs7_RI *pkcs7_DecodeRecipientInfo(PDFLibHandle lib, pkcs7_RI **pri, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_RI *rslt = NULL;
    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pri == NULL || (*pri) == NULL)
            rslt = pkcs7_CreateRecipientInfo(lib); 
        else
            rslt = *pri;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        pkcs7_DecodeIssuerAndSerialNumber(lib, &(rslt->issuerAndSerialNumber), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->keyEncryptionAlgorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->encryptedKey), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pri != NULL) (*pri) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pri == NULL || (*pri) != rslt)) 
        {
            pkcs7_DisposeRecipientInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs7_RI *pkcs7_CreateRecipientInfo(PDFLibHandle lib)
{
    pkcs7_RI *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_RI* )mmalloc(lib, sizeof(pkcs7_RI));
        rslt->version = asn1_CreateInteger(lib);
        rslt->issuerAndSerialNumber = pkcs7_CreateIssuerAndSerialNumber(lib);
        rslt->keyEncryptionAlgorithm = x509_CreateAlgorithm(lib);
        rslt->encryptedKey = asn1_CreateOctetString(lib);
    }PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeRecipientInfo(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeRecipientInfo(PDFLibHandle lib, pkcs7_RI *ri)
{
    if (ri == NULL) return;
    asn1_DisposeInteger(lib, ri->version);
    pkcs7_DisposeIssuerAndSerialNumber(lib, ri->issuerAndSerialNumber);
    x509_DisposeAlgorithm(lib, ri->keyEncryptionAlgorithm);
    asn1_DisposeOctetString(lib, ri->encryptedKey);
    mfree(lib, ri);
}



ppInt32 pkcs7_EncodeIssuerAndSerialNumber(pkcs7_IASN *iasn, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (iasn == NULL) return 0;
    rslt += x509_EncodeName(iasn->issuer, NULL);
    rslt += asn1_EncodeInteger(iasn->serialNumber, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    x509_EncodeName(iasn->issuer, &p);
    asn1_EncodeInteger(iasn->serialNumber, &p);
    (*pbuf) = p;
    return r;
}

pkcs7_IASN *pkcs7_DecodeIssuerAndSerialNumber(PDFLibHandle lib, pkcs7_IASN **piasn, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_IASN *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = (*pbuf);

        if (piasn == NULL || (*piasn) == NULL)
            rslt = pkcs7_CreateIssuerAndSerialNumber(lib);
        else
            rslt = (*piasn);

        ctx.p = (*pbuf);
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx ,&max);

        ctx.q = ctx.p;
        x509_DecodeName(lib, &(rslt->issuer), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->serialNumber), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (piasn != NULL) (*piasn) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (piasn == NULL || (*piasn) != rslt))
        {
            pkcs7_DisposeIssuerAndSerialNumber(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs7_IASN *pkcs7_CreateIssuerAndSerialNumber(PDFLibHandle lib)
{
    pkcs7_IASN *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_IASN* )mmalloc(lib, sizeof(pkcs7_IASN));
        rslt->issuer = x509_CreateName(lib);
        rslt->serialNumber = asn1_CreateInteger(lib);
    }PDFEXCEPT(lib){
        if (rslt != NULL)
        {
            pkcs7_DisposeIssuerAndSerialNumber(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void pkcs7_DisposeIssuerAndSerialNumber(PDFLibHandle lib, pkcs7_IASN *iasn)
{
    if (iasn == NULL) return;
    x509_DisposeName(lib, iasn->issuer);
    asn1_DisposeInteger(lib, iasn->serialNumber);
    mfree(lib, iasn);
}

pkcs7_IASN *pkcs7_IASNFromCertInfo(PDFLibHandle lib, x509_CertInfo *ci)
{
    pkcs7_IASN *rslt = NULL;
    PDFTRY(lib){
        rslt = pkcs7_CreateIssuerAndSerialNumber(lib);
        x509_DisposeName(lib, rslt->issuer);
        rslt->issuer = (x509_Name *)asn1_Clone(lib, ( TEncodeFunction )x509_EncodeName, ( TDecodeFunction )x509_DecodeName, ci->issuer);
        asn1_DisposeInteger(lib, rslt->serialNumber);
        rslt->serialNumber = (asn1_String *) asn1_Clone(lib, ( TEncodeFunction )asn1_EncodeInteger, ( TDecodeFunction )asn1_DecodeInteger, ci->serialNumber);
    }PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeIssuerAndSerialNumber(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

#endif