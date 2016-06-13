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
VSkASN.c
*********************************************************************/

#include "VSkASNI.h"
#include <string.h>

#ifndef NOT_USE_SIGN
/************************************************************************/
/*                                                                    */
/************************************************************************/
ppUns8 digest_Begin[digest_Length - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x02};

ppUns8 digest_Ends[digest_Count] = 
{
    0x02,     /*md2 */
        0x04,     /*md4 */
        0x05,     /*md5 */
        0x07      /*hmacWithSHA1 */
};
ppInt32 digest_OIDs[digest_Count] = 
{
    ID_md2,
        ID_md4,
        ID_md5,
        ID_hmacWithSHA1
};

ppUns8 pkcs1_Begin[pkcs1_Length - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01};
ppUns8 pkcs1_Ends[pkcs1_Count] =
{
    0x01, /*rsaEncryption */
        0x02, /*md2WithRSAEncryption */
        0x03, /*md4WithRSAEncryption */
        0x04, /*md5WithRSAEncryption */
        0x05  /*sha1WithRSAEncryption */
};
ppInt32 pkcs1_OIDs[pkcs1_Count] =
{
    ID_rsaEncryption,
        ID_md2WithRSAEncryption,
        ID_md4WithRSAEncryption,
        ID_md5WithRSAEncryption,
        ID_sha1WithRSAEncryption
};

ppUns8 algorithm_Begin[algorithm_Length - 1] = {0x2B,0x0E,0x03,0x02};
ppUns8 algorithm_Ends[algorithm_Count] = 
{
    0x03,   /*md5WithRSA */
        0x06,   /*des_ecb */
        0x07,   /*des_cbc */
        0x08,   /*des_ofb64 */
        0x09,   /*des_cfb64 */
        0x0B,   /*rsaSignature */
        /*
        0x0C,   / * dsa-old * /
        0x0D,   / *dsaWithSHA * /
        */
        0x0F,   /*shaWithRSAEncryption */
        0x11,   /*des_ede */
        0x12,   /*sha */
        0x1A,   /*sha1 */
        /*
        0x1B,   / *dsaWithSHA1-old * /
        */
        0x1D    /*sha1WithRSA */
};
ppInt32 algorithm_OIDs[algorithm_Count] = 
{
    ID_md5WithRSAEncryption,
        ID_des_ecb,
        ID_des_cbc,
        ID_des_ofb64,
        ID_des_cfb64,
        ID_rsaSignature,
        /*
        ID_dsa_2,
        ID_dsaWithSHA,
        */
        ID_shaWithRSAEncryption,
        ID_des_ede,
        ID_sha,
        ID_sha1,
        /*
        ID_dsaWithSHA1_2,
        */
        ID_sha1WithRSAEncryption,
};

ppUns8 pkcs5_Begin[pkcs5_Length - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x05};
ppUns8 pkcs5_Ends[pkcs5_Count] = 
{
    0x01,   /*pbeWithMD2AndDES_CBC */
        0x04,   /*pbeWithMD2AndRC2_CBC */
        0x03,   /*pbeWithMD5AndDES_CBC */
        0x06,   /*pbeWithMD5AndRC2_CBC */
        0x0A,   /*pbeWithSHA1AndDES_CBC */
        0x0B,   /*pbeWithSHA1AndRC2_CBC */
        0x0C,   /*pbkdf2 */
        0x0D,   /*pbes2 */
        0x0E    /*pbmac1 */
};
ppInt32 pkcs5_OIDs[pkcs5_Count] = 
{
    ID_pbeWithMD2AndDES_CBC,
        ID_pbeWithMD2AndRC2_CBC,
        ID_pbeWithMD5AndDES_CBC,
        ID_pbeWithMD5AndRC2_CBC,
        ID_pbeWithSHA1AndDES_CBC,
        ID_pbeWithSHA1AndRC2_CBC,
        ID_pbkdf2,
        ID_pbes2,
        ID_pbmac1
};

ppUns8 pkcs7_TypeBegin[pkcs7_TypeLength - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x07};
ppUns8 pkcs7_TypeEnds[pkcs7_TypeCount] = 
{
    0x01,/*data */
        0x02,/*signedData */
        0x03,/*envelopedData */
        0x04,/*signedAndEnvelopedData */
        0x05,/*digestedData */
        0x06 /*encryptedData */
};
ppInt32 pkcs7_TypeOIDs[pkcs7_TypeCount] = 
{
    ID_pkcs7Data,
        ID_pkcs7SignedData,
        ID_pkcs7EnvelopedData,
        ID_pkcs7SignedAndEnvelopedData,
        ID_pkcs7DigestedData,
        ID_pkcs7EncryptedData
};

ppUns8 pkcs9_Begin[pkcs9_Length - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x09};
ppUns8 pkcs9_Ends[pkcs9_Count] = 
{
    0x01,/*pkcs9EmailAddress */
        0x02,/*pkcs9UnstructuredName */
        0x03,/*pkcs9ContentType */
        0x04,/*pkcs9MessageDigest */
        0x05,/*pkcs9SigningTime */
        0x06,/*pkcs9Countersignature */
        0x07,/*pkcs9ChallengePassword */
        0x08,/*pkcs9UnstructuredAddress */
        0x09,/*pkcs9ExtCertAttributes */
        0x0D,/*pkcs9SigningDescription */
        0x0E,/*pkcs9ExtensionRequest */
        0x0F,/*pkcs9SMIMECapabilities */
        0x14,/*pkcs9FriendlyName */
        0x15 /*pkcs9LocalKeyID */
};
ppInt32 pkcs9_OIDs[pkcs9_Count] = 
{
    ID_pkcs9EmailAddress,
        ID_pkcs9UnstructuredName,
        ID_pkcs9ContentType,
        ID_pkcs9MessageDigest,
        ID_pkcs9SigningTime,
        ID_pkcs9Countersignature,
        ID_pkcs9ChallengePassword,
        ID_pkcs9UnstructuredAddress,
        ID_pkcs9ExtCertAttributes,
        ID_pkcs9SigningDescription,
        ID_pkcs9ExtensionRequest,
        ID_pkcs9SMIMECapabilities,
        ID_pkcs9FriendlyName,
        ID_pkcs9LocalKeyID
};

ppUns8 pkcs12_CertTypeBegin[pkcs12_CertTypeLength - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x09,0x16};
ppUns8 pkcs12_CertTypeEnds[pkcs12_CertTypeCount] = 
{
    0x01,/*pkcs12CertTypeX509 */
        0x02 /*pkcs12CertTypeSDSI */
};
ppInt32 pkcs12_CertTypeOIDs[pkcs12_CertTypeCount] = 
{
    ID_pkcs12CertTypeX509,
        ID_pkcs12CertTypeSDSI
};

ppUns8 pkcs12_CRLTypeBegin[pkcs12_CRLTypeLength - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x09,0x17};
ppUns8 pkcs12_CRLTypeEnds[pkcs12_CRLTypeCount] = 
{
    0x01 /*pkcs12CRLTypeX509 */
};
ppInt32 pkcs12_CRLTypeOIDs[pkcs12_CRLTypeCount] = 
{
    ID_pkcs12CRLTypeX509
};

ppUns8 pkcs12_PbeBegin[pkcs12_PbeLength - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x0C,0x01};
ppUns8 pkcs12_PbeEnds[pkcs12_PbeCount] = 
{
    0x01, /*pbeWithSHA1And128BitRC4 */
        0x02, /*pbeWithSHA1And40BitRC4 */
        0x03, /*pbeWithSHA1And3_Key_TripleDES_CBC */
        0x04, /*pbeWithSHA1And2_Key_TripleDES_CBC */
        0x05, /*pbeWithSHA1And128BitRC2_CBC */
        0x06, /*pbeWithSHA1And40BitRC2_CBC */
};
ppInt32 pkcs12_PbeOIDs[pkcs12_PbeCount] = 
{
    ID_pbeWithSHA1And128BitRC4,
        ID_pbeWithSHA1And40BitRC4,
        ID_pbeWithSHA1And3_Key_TripleDES_CBC,
        ID_pbeWithSHA1And2_Key_TripleDES_CBC,
        ID_pbeWithSHA1And128BitRC2_CBC,
        ID_pbeWithSHA1And40BitRC2_CBC
};

ppUns8 pkcs12_BagTypeBegin[pkcs12_BagTypeLength - 1] = {0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x0C,0x0A,0x01};
ppUns8 pkcs12_BagTypeEnds[pkcs12_BagTypeCount] = 
{
    0x01,/*keyBag */
        0x02,/*pkcs8ShroudedKeyBag */
        0x03,/*certBag */
        0x04,/*crlBag */
        0x05,/*secretBag */
        0x06 /*safeContentsBag */
};
ppInt32 pkcs12_BagTypeOIDs[pkcs12_BagTypeCount] = 
{
    ID_keyBag,
        ID_pkcs8ShroudedKeyBag,
        ID_certBag,
        ID_crlBag,
        ID_secretBag,
        ID_safeContentsBag
};

ppUns8 nameEntry_Begin[nameEntry_Length - 1] = {0x55,0x04};
ppUns8 nameEntry_Ends[nameEntry_Count] = 
{
    0x03,   /*commonName */
        0x04,   /*surname */
        0x05,   /*serialNumber */
        0x06,   /*countryName */
        0x07,   /*localityName */
        0x08,   /*stateOrProvinceName */
        0x0A,   /*organizationName */
        0x0B,   /*organizationalUnitName */
        0x0C,   /*title */
        0x0D,   /*description */
        0x1F,   /*crl_distribution_points */
        0x29,   /*name */
        0x2A,   /*givenName */
        0x2B,   /*initials */
        0x2D,   /*uniqueIdentifier */
        0x2E,   /*dnQualifier */
        0x48,   /*role */
};
ppInt32 nameEntry_OIDs[nameEntry_Count] = 
{
    ID_commonName,
        ID_surname,
        ID_serialNumber,
        ID_countryName,
        ID_localityName,
        ID_stateOrProvinceName,
        ID_organizationName,
        ID_organizationalUnitName,
        ID_title,
        ID_description,
        ID_crl_distribution_points,
        ID_name,
        ID_givenName,
        ID_initials,
        ID_uniqueIdentifier,
        ID_dnQualifier,
        ID_role
};

#define num_Arrays 11   /*KEEP UPDATED! */
ppInt32 pLength[num_Arrays] = 
{
    digest_Length,
        pkcs1_Length,
        algorithm_Length,
        pkcs5_Length,
        pkcs7_TypeLength,
        pkcs9_Length,
        pkcs12_CertTypeLength,
        pkcs12_CRLTypeLength,
        pkcs12_PbeLength,
        pkcs12_BagTypeLength,
        nameEntry_Length
};
ppInt32 pCount[num_Arrays] = 
{
    digest_Count,
        pkcs1_Count,
        algorithm_Count,
        pkcs5_Count,
        pkcs7_TypeCount,
        pkcs9_Count,
        pkcs12_CertTypeCount,
        pkcs12_CRLTypeCount,
        pkcs12_PbeCount,
        pkcs12_BagTypeCount,
        nameEntry_Count
};
ppUns8 *pBegin[num_Arrays] = 
{
    digest_Begin,
        pkcs1_Begin,
        algorithm_Begin,
        pkcs5_Begin,
        pkcs7_TypeBegin,
        pkcs9_Begin,
        pkcs12_CertTypeBegin,
        pkcs12_CRLTypeBegin,
        pkcs12_PbeBegin,
        pkcs12_BagTypeBegin,
        nameEntry_Begin
};
ppUns8 *pEnds[num_Arrays] = 
{
    digest_Ends,
        pkcs1_Ends,
        algorithm_Ends,
        pkcs5_Ends,
        pkcs7_TypeEnds,
        pkcs9_Ends,
        pkcs12_CertTypeEnds,
        pkcs12_CRLTypeEnds,
        pkcs12_PbeEnds,
        pkcs12_BagTypeEnds,
        nameEntry_Ends
};
ppInt32 *pOIDs[num_Arrays] = 
{
    digest_OIDs,
        pkcs1_OIDs,
        algorithm_OIDs,
        pkcs5_OIDs,
        pkcs7_TypeOIDs,
        pkcs9_OIDs,
        pkcs12_CertTypeOIDs,
        pkcs12_CRLTypeOIDs,
        pkcs12_PbeOIDs,
        pkcs12_BagTypeOIDs,
        nameEntry_OIDs
};
/************************************************************************/
/*                                                                    */
/************************************************************************/
asn1_Object *asn1_CreateObjectByID(PDFLibHandle lib, ppInt32 id)
{
    asn1_Object *rslt = NULL;
    ppInt32 i, j, *ids, c, l;
    PDFTRY(lib){
        i = 0; j = 0;
        for (; i < num_Arrays; i++)
        {
            ids = pOIDs[i];
            c = pCount[i]; j = 0;
            for (; j < c; j++)
                if (ids[j] == id) break;
            if (j < c) break;
        }
        if (i < num_Arrays && j < c)
        {
            l = pLength[i];
            rslt = asn1_CreateObject(lib);
            rslt->length = l;
            rslt->data = ( ppUns8 * )mmalloc(lib, l);
            memcpy(rslt->data, pBegin[i], l - 1);
            rslt->data[l - 1] = pEnds[i][j];
        }
    } PDFEXCEPT(lib){
        if (rslt != NULL)
        {
            asn1_DisposeObject(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

ppInt32 asn1_GetObjectID(asn1_Object *obj)
{
    ppInt32 i, j, rslt = ID_undef;
    if (obj == NULL) return rslt;
    for (i = 0; i < num_Arrays; i++)
    {
        if (obj->length != pLength[i]) continue;
        if (memcmp(obj->data, pBegin[i], pLength[i] - 1)) continue; 
        for (j = 0; j < pCount[i]; j++)
            if (obj->data[pLength[i] - 1] == pEnds[i][j]) 
            {
                rslt = pOIDs[i][j];
                break;
            }
            break;
    }
    return rslt;
}

VECTOR *vector_Create(PDFLibHandle lib)
{
    ppInt32 i;
    VECTOR *v = NULL;
    PDFTRY(lib){

        v = (VECTOR* )mmalloc(lib, sizeof(VECTOR)); 
        v->data = (char ** )mmalloc(lib, MIN_ITEMS * sizeof(char * ));
        for (i = 0; i < MIN_ITEMS; i++) v->data[i] = NULL;
        v->lib = lib;
        v->max = MIN_ITEMS;
        v->num = 0;
    } PDFEXCEPT(lib){
        if (v != NULL) 
        {
            vector_Free(v);
            v = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return v;
}

void vector_Dispose(VECTOR *v, void (*cleanup)(PDFLibHandle lib, void * ))
{
    ppInt32 i;
    if (v != NULL)
    {
        for (i = 0; i < v->num; i++)
            if (v->data[i] != NULL) cleanup(v->lib, v->data[i]);
        vector_Free(v);
    }
}

void vector_Free(VECTOR *v)
{
    if (v != NULL)
    {
        if (v->data != NULL) mfree(v->lib, v->data);
        mfree(v->lib, v);
    }
}



ppInt32 vector_GetCount(const VECTOR *v)
{
    return (v == NULL ? -1 : v->num);
}

char *vector_GetValueAt(const VECTOR *v, ppInt32 i)
{
    return (v == NULL ? NULL : v->data[i]);
}

char *vector_SetValueAt(VECTOR *v, ppInt32 i, char *d)
{
    if (v == NULL) return NULL;
    return (v->data[i] = d);
}



ppInt32 vector_Insert(VECTOR *v, ppInt32 i, char *d)
{
    char **s;
    ppInt32 j;

    if (v == NULL) return 0;
    if (v->max <= v->num + 1)
    {
        s = (char ** )mrealloc(v->lib, (char * )v->data, sizeof(char * )*v->max*2);
        v->data = s; v->max *= 2;
    }
    if (i >= v->num || i < 0)
        v->data[v->num] = d;
    else 
    {
        for (j = v->num; j > i; j--)
            v->data[j] = v->data[j - 1];
        v->data[i] = d;
    }
    return ++v->num;
}

char *vector_Delete(VECTOR *v, ppInt32 i)
{
    char *r;
    ppInt32 j;

    if (v == NULL || v->num == 0 || i < 0 || i >= v->num) 
        return NULL;

    r = v->data[i];
    if (i != v->num - 1)
        for (j = i; j < v->num - 1; j++)
            v->data[i] = v->data[i + 1];
    v->num--;
    return r;
}

ppInt32 vector_Push(VECTOR *v, char *d)
{
    return vector_Insert(v, v->num, d);
}

char *vector_Pop(VECTOR *v)
{
    if (v == NULL || v->num <= 0) return NULL;
    return vector_Delete(v, v->num - 1);
}



BUFFER *buffer_Create(PDFLibHandle lib)
{
    BUFFER *rslt;
    rslt = (BUFFER* )mmalloc(lib, sizeof(BUFFER));
    rslt->num = 0;
    rslt->max = 0;
    rslt->data = NULL;
    rslt->lib = lib;
    return rslt;
}

void buffer_Dispose(BUFFER *ptr)
{
    if (ptr == NULL) return;
    if (ptr->data != NULL) 
    {
        memset(ptr->data, 0, ptr->max);
        mfree(ptr->lib, ptr->data);
    }
    mfree(ptr->lib, ptr);
}

ppInt32 buffer_Enlarge(BUFFER *ptr, ppInt32 num)
{
    char *rslt;
    ppUns32 n;

    if (ptr->num >= num) {
        ptr->num = num;
        return num;
    }

    if (ptr->max >= num) {
        memset(&(ptr->data[ptr->num]), 0, num - ptr->num);
        ptr->num = num;
        return num;
    }

    n = (num + 3)*4/3;
    if (ptr->data == NULL)
        rslt = ( char * ) mmalloc(ptr->lib, n);
    else
        rslt = ( char * ) mrealloc(ptr->lib, ptr->data, n);
    ptr->data = (ppUns8 *)rslt;
    ptr->num = num;
    ptr->max = n;
    return num;
}

/*=== ASN.1 basic routines === */

ppInt32 asn1_ObjectSize(ppInt32 constructed, ppInt32 length, ppInt32 tag)
{
    ppInt32 rslt;

    rslt = length;
    rslt++;
    if (tag >= 31)
    {
        while (tag > 0)
        {
            tag >>= 7;
            rslt++;
        }
    }
    if (length == 0 && constructed == 2)
        rslt += 2;
    rslt++;
    if (length > 127)
    {
        while (length > 0)
        {
            length >>= 8;
            rslt++;
        }
    }
    return rslt;
}

void asn1_PutLength(ppUns8 **pbuf, ppInt32 length)
{
    ppUns8 *p = *pbuf;
    ppInt32 i, l;
    if (length <= 127)
        *(p++) = (ppUns8)length;
    else
    {
        l = length;
        for (i = 0; l > 0; i++)
            l >>= 8;
        *(p++) = (ppUns8)( i | 0x80 );
        l = i;
        while (i-- > 0)
        {
            p[i] = (ppUns8)( length & 0xFF );
            length >>= 8;
        }
        p += l;
    }
    *pbuf = p;
}

void asn1_PutObject(ppUns8 **pbuf, ppInt32 constructed, ppInt32 length, ppInt32 tag, ppInt32 cls)
{
    ppUns8 *p = *pbuf;
    ppInt32 i, ttag;

    i = constructed ? CONSTRUCTED : 0;
    i |= (cls & PRIVATE_CLS);
    if (tag < 31)
        *(p++) = (ppUns8)( i | (tag & PRIMITIVE) );
    else
    {
        *(p++) = (ppUns8)( i | PRIMITIVE );
        for (i = 0, ttag = tag; ttag > 0; i++) ttag >>=7;
        ttag = i;
        while (i-- > 0)
        {
            p[i] = (ppUns8)( tag & 0x7F );
            if(i != (ttag - 1)) p[i] |= 0x80;
            tag >>= 7;
        }
        p += ttag;
    }
    if ((constructed == 2) && (length == 0))
        *(p++) = 0x80;
    else
        asn1_PutLength(&p, length);
    *pbuf = p;
}

ppInt32 asn1_GetLength(ppUns8 **pbuf, ppInt32 *pinf, ppInt64 *plen, ppInt32 max)
{
    ppUns8 *p = *pbuf;
    ppUns32 rslt = 0;
    ppInt32 i;

    if (max-- < 1) return 0;
    if (*p == 0x80)
    {
        *pinf = 1; rslt = 0; p++;
    }
    else
    {
        *pinf = 0; i = *p & 0x7f;
        if (*(p++) & 0x80)
        {
            if ( i > (ppInt32)sizeof(ppInt64) )
                return 0;
            if (max-- == 0) return 0;
            while (i-- > 0)
            {
                rslt <<= 8L; rslt |= *(p++);
                if (max-- == 0) return 0;
            }
        }
        else
            rslt = i;
    }
    if (rslt > LONG_MAX) return 0;
    *pbuf = p; *plen = (ppInt64)rslt;
    return 1;
}

ppInt32 asn1_GetObject(ppUns8 **pbuf, ppInt64 *plen, ppInt32 *ptag, ppInt32 *pcls, ppInt64 omax)
{
    ppInt32 i, rslt;
    ppInt64 l, max = omax;
    ppUns8 *p = *pbuf;
    ppInt32 tag, cls, inf;

    if (max == 0) return 0x80;
    rslt = (*p & CONSTRUCTED);
    cls = (*p & PRIVATE_CLS);
    i = *p & PRIMITIVE;
    if (i == PRIMITIVE)
    { 
        p++; if (--max == 0) return 0x80;
        l = 0;
        while (*p & 0x80) 
        {
            l <<= 7L; l |= *(p++) & 0x7f;
            if (--max == 0) return 0x80;
            if (l > (INT_MAX >> 7L)) return 0x80;
        }
        l <<= 7L; l |= *(p++) & 0x7f;
        tag = (ppInt32)l;
        if (--max == 0) return 0x80;
    }
    else
    {
        tag = i;
        p++; if (--max == 0) return 0x80;
    }
    *ptag = tag;
    *pcls = cls;
    if (!asn1_GetLength(&p, &inf, plen, (ppInt32)max)) return 0x80;
    /*
    if (*plen > (omax - (p - *pbuf))) rslt |= 0x80;
    */
    *pbuf = p;
    return rslt | inf;
}



void asn1_GetSequence(PDFLibHandle lib, asn1_Context *ctx, ppInt64 *len)
{
    ppUns8 *q;
    q = ctx->p;
    ctx->inf = asn1_GetObject(&(ctx->p), &(ctx->len), &(ctx->tag), &(ctx->cls), *len);
    if (ctx->inf & 0x80) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
    if (ctx->tag != SEQUENCE_TAG) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
    (*len) -= (ppInt64)(ctx->p - q);
    if (ctx->max && (*len) < 0) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
    if (ctx->inf == (1 | CONSTRUCTED)) 
        ctx->len = (*len);
    ctx->eos = 0;
}

ppInt32 asn1_CheckInfiniteEnd(ppUns8 **pbuf, ppInt64 len)
{
    if (len <= 0) return 1;
    else if (len >= 2 && (*pbuf)[0] == 0 && (*pbuf)[1] == 0)
    {
        (*pbuf) += 2;
        return 1;
    }
    return 0;
}

ppInt32 asn1_CheckEndSequence(asn1_Context *ctx)
{
    if ((ctx->inf & 1) == 0) 
        return (ctx->len <= 0);
    else
    {
        ctx->eos = asn1_CheckInfiniteEnd(&ctx->p, ctx->len);
        return ctx->eos;
    }
}

void asn1_Finish(PDFLibHandle lib, asn1_Context *ctx)
{
    /*TO DO: check error codes */
    if (ctx->inf == (1 | CONSTRUCTED) && !ctx->eos &&
        !asn1_CheckInfiniteEnd(&ctx->p, ctx->len)) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleOtherError));
    if ((ctx->inf & 1) == 0 && ctx->len != 0)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleOtherError));
    if (ctx->len < 0) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleOtherError));
}



ppInt32 asn1_EncodeType(asn1_Type *type, ppUns8 **pbuf)
{
    ppInt32 r;

    if (type == NULL) return 0;
    switch (type->type)
    {
    case NULL_TAG:
        if (pbuf != NULL) asn1_PutObject(pbuf, 0, 0, NULL_TAG, UNIVERSAL_CLS);
        r = 2;
        break;
    case INTEGER_TAG:
        r = asn1_EncodeInteger(type->value.integer, pbuf);
        break;
    case BITSTRING_TAG:
        r = asn1_EncodeBitString(type->value.bit_string, pbuf);
        break;
    case OCTETSTRING_TAG:
        r = asn1_EncodeOctetString(type->value.octet_string, pbuf);
        break;
    case OBJECT_TAG:
        r = asn1_EncodeObject(type->value.object, pbuf);
        break;
    case IA5STRING_TAG:
        r = asn1_EncodeIA5String(type->value.ia5string, pbuf);
        break;
    case BMPSTRING_TAG:
        r = asn1_EncodeBMPString(type->value.bmpstring, pbuf);
        break;
    case UTCTIME_TAG:
        r = asn1_EncodeUTCTime(type->value.utc_time, pbuf);
        break;
    case GENERALIZEDTIME_TAG:
        r = asn1_EncodeGeneralizedTime(type->value.generalized_time, pbuf);
        break;
    case SET_TAG:
    case SEQUENCE_TAG:
    default:
        if (type->value.set == NULL)
            r = 0;
        else
        {
            r = type->value.set->length;
            if (pbuf != NULL)
            {
                memcpy(*pbuf, type->value.set->data, r);
                *pbuf += r;
            }
        }
        break;
    }
    return r;
}

asn1_Type *asn1_DecodeType(PDFLibHandle lib, asn1_Type **ptype, ppUns8 **pbuf, ppInt64 length)
{
    asn1_Type *rslt = NULL;
    ppUns8 *q, *p, *max;
    ppInt32 inf, tag, cls;
    ppInt64 len;

    PDFTRY(lib)
        if (ptype == NULL || (*ptype) == NULL)
            rslt = asn1_CreateType(lib);
        else
            rslt = (*ptype);

    p = *pbuf; q = p;
    max = p + (ppInt32)length;

    inf = asn1_GetObject(&q, &len, &tag, &cls, length);
    if (inf & 0x80) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));

    asn1_DisposeTypeComponent(lib, rslt);

    switch (tag)
    {
    case NULL_TAG:
        p = q;
        rslt->value.ptr = NULL;
        break;
    case INTEGER_TAG:
        rslt->value.integer = asn1_DecodeInteger(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case BITSTRING_TAG:
        rslt->value.bit_string = asn1_DecodeBitString(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case OCTETSTRING_TAG:
        rslt->value.octet_string = asn1_DecodeOctetString(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case OBJECT_TAG:
        rslt->value.object = asn1_DecodeObject(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case IA5STRING_TAG:
        rslt->value.ia5string = asn1_DecodeIA5String(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case BMPSTRING_TAG:
        rslt->value.bmpstring = asn1_DecodeBMPString(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case UTCTIME_TAG:
        rslt->value.utc_time = asn1_DecodeUTCTime(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case GENERALIZEDTIME_TAG:
        rslt->value.generalized_time = asn1_DecodeGeneralizedTime(lib, NULL, &p, (ppInt64)(max - p));
        break;
    case SET_TAG:
    case SEQUENCE_TAG:
    default:
        rslt->value.set = asn1_CreateString(lib);
        rslt->value.set->type = tag;
        len = (ppInt32)len + q - p;
        asn1_SetString(lib, rslt->value.set, p, (ppInt32)len);
        p = p + (ppInt32)len;
        break;
    }

    rslt->type = tag;
    if (ptype != NULL) (*ptype) = rslt;
    (*pbuf) = p;
    PDFEXCEPT(lib)
        if (rslt != NULL && (ptype == NULL || (*ptype) != rslt))
        {
            asn1_DisposeType(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
        PDFTRYEND(lib)
            return rslt;
}

/*=== Any ASN.1 type routines === */

asn1_Type *asn1_CreateType(PDFLibHandle lib)
{
    asn1_Type *rslt;
    rslt = (asn1_Type* )mmalloc(lib, sizeof(asn1_Type));
    rslt->type = -1;
    rslt->value.ptr = NULL;
    return rslt;
}

void asn1_DisposeType(PDFLibHandle Lib, asn1_Type *ptype)
{
    if (ptype == NULL) return;
    asn1_DisposeTypeComponent(Lib, ptype);
    mfree(Lib, ptype);
}

void asn1_DisposeTypeComponent(PDFLibHandle Lib, asn1_Type *ptype)
{
    if (ptype == NULL) return;

    if (ptype->value.ptr != NULL)
    {
        if (ptype->type == OBJECT_TAG)
            asn1_DisposeObject(Lib, ptype->value.object);
        else if (ptype->type != NULL_TAG)
            asn1_DisposeString(Lib, (asn1_String* )ptype->value.ptr);
        ptype->type = 0;
        ptype->value.ptr = NULL;
    }
}

void asn1_SetType(PDFLibHandle Lib, asn1_Type *type, ppInt32 t, void *v)
{
    if (type->value.ptr != NULL)
        asn1_DisposeTypeComponent(Lib, type);
    type->type = t;
    type->value.ptr = ( char * ) v;
}

/*=== Byte sequence routines*/

ppInt32 asn1_EncodeBytes(asn1_String *str, ppUns8 **pbuf, ppInt32 tag, ppInt32 cls)
{
    ppInt32 rslt, r, constructed;
    ppUns8 *p;

    if (str == NULL)  return 0;

    if (tag == BITSTRING_TAG)
        return asn1_EncodeBitString(str, pbuf);

    rslt = str->length;
    r = asn1_ObjectSize(0, rslt, tag);
    if (pbuf == NULL) return r;

    p = *pbuf;
    if (tag == SEQUENCE_TAG || tag == SET_TAG)
        constructed = 1;
    else
        constructed = 0;
    asn1_PutObject(&p, constructed, rslt, tag, cls);
    memcpy(p, str->data, str->length);
    p += str->length;
    *pbuf = p;
    return r;
}

void asn1_CollatePrimitive(PDFLibHandle lib, asn1_String *str, asn1_Context *ctx)
{
    asn1_String *os = NULL;
    BUFFER b;
    ppInt32 num;

    b.num = 0;
    b.max = 0;
    b.data = NULL;

    PDFTRY(lib){
        if (str == NULL) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));

        num = 0;
        for (;;) 
        {
            if (asn1_CheckEndSequence(ctx)) break;

            ctx->q = ctx->p;
            asn1_DecodeBytes(lib, &os, &ctx->p, ctx->len, ctx->tag, ctx->cls);
            buffer_Enlarge(&b, num + os->length);

            memcpy(b.data + num, os->data, os->length);
            ctx->len -= (ppInt64)(ctx->p - ctx->q);
            num += os->length;
        }

        asn1_Finish(lib, ctx);

        str->length=num;
        if (str->data != NULL) mfree(lib, str->data);
        str->data = (ppUns8 * )b.data;
        if (os != NULL) 
        { 
            asn1_DisposeOctetString(lib, os); 
            os = NULL; 
        }
    } PDFEXCEPT(lib){
        if (os != NULL) { 
            asn1_DisposeOctetString(lib, os); os = NULL; 
        }
        if (b.data != NULL) {
            mfree(lib, b.data); b.data = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
}

asn1_String *asn1_DecodeBytes(PDFLibHandle lib, asn1_String **pstr, ppUns8 **pbuf, ppInt64 len, ppInt32 tag, ppInt32 cls)
{
    asn1_String *rslt = NULL;
    ppUns8 *p, *s;
    ppInt32 ainf, atag, acls;
    ppInt64 alen;

    PDFTRY(lib){
        if (pstr == NULL || (*pstr) == NULL)
            rslt = asn1_CreateString(lib);
        else
            rslt = *pstr;

        p = *pbuf;
        ainf = asn1_GetObject(&p, &alen, &atag, &acls, len);
        if (ainf & 0x80)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
        if (atag != tag)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
        if (ainf & CONSTRUCTED)
        {
            asn1_Context ctx;
            ctx.pp = pbuf;
            ctx.p = p;
            ctx.inf = ainf;
            if (ainf & 1)
                ctx.len = len - (p - *pbuf);
            else
                ctx.len = len;
            ctx.tag = tag;
            ctx.cls = cls;
            ctx.max = ! len ? NULL : p + (ppInt32)len;
            asn1_CollatePrimitive(lib, rslt, &ctx);
            p = ctx.p;
        }
        else
        {
            if (alen != 0)
            {
                if (rslt->length < alen || rslt->data == NULL)
                {
                    if (rslt->data != NULL) mfree(lib, rslt->data);
                    s = (ppUns8 * )mmalloc(lib, (ppInt32)alen + 1);
                }
                else
                    s = rslt->data;
                memcpy(s, p, (ppInt32)alen);
                s[(ppInt32)alen] = 0;
                p = p + (ppInt32)alen;
            }
            else
            {
                if (rslt->data != NULL) mfree(lib, rslt->data);
                s = NULL;
            }

            rslt->length = (ppInt32)alen;
            rslt->data = s;
            rslt->type = tag;
        }

        if (pstr != NULL) (*pstr) = rslt;
        (*pbuf) = p;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pstr == NULL || (*pstr) != rslt))
        {
            asn1_DisposeString(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

static ppUns32 tag2bit[32] = 
{
    0,                  0,                      0,                      BITSTRING_BIT,
        OCTETSTRING_BIT,    0,                      0,                      UNKNOWN_BIT,
        UNKNOWN_BIT,        UNKNOWN_BIT,            UNKNOWN_BIT,            UNKNOWN_BIT,
        UTF8STRING_BIT,     UNKNOWN_BIT,            UNKNOWN_BIT,            UNKNOWN_BIT,
        0,                  0,                      NUMERICSTRING_BIT,      PRINTABLESTRING_BIT,
        T61STRING_BIT,      VIDEOTEXSTRING_BIT,     IA5STRING_BIT,          0,
        0,                  GRAPHICSTRING_BIT,      ISO64STRING_BIT,        GENERALSTRING_BIT,
        UNIVERSALSTRING_BIT,UNKNOWN_BIT,            BMPSTRING_BIT,          UNKNOWN_BIT
};

asn1_String *asn1_DecodeTypedBytes(PDFLibHandle lib, asn1_String **pstr, ppUns8 **pbuf, ppInt64 max, ppInt32 type)
{
    asn1_String *rslt = NULL;
    ppUns8 *p, *s;
    ppInt32 inf, tag, cls;
    ppInt64 len;

    PDFTRY(lib){
        p = (*pbuf);
        inf = asn1_GetObject(&p, &len, &tag, &cls, max);
        if ((inf & 0x80) != 0 || tag >= 32 || (tag2bit[tag] & type) == 0)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));

        if (tag == BITSTRING_TAG)
            return asn1_DecodeBitString(lib, pstr, pbuf, max);

        if (pstr == NULL || (*pstr) == NULL)
            rslt = asn1_CreateString(lib);
        else
            rslt = (*pstr);

        if (len != 0)
        {
            s = (ppUns8 * )mmalloc(lib, (size_t)len + 1);
            memcpy(s, p, (ppInt32)len);
            s[(ppInt32)len] = 0;
            p = p + (ppInt32)len;
        }
        else s = NULL;

        if (rslt->data != NULL) mfree(lib, rslt->data);
        rslt->length = (ppInt32)len;
        rslt->data = s;
        rslt->type = tag;

        if (pstr != NULL) (*pstr) = rslt;
        (*pbuf) = p;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pstr == NULL || (*pstr) != rslt))
        {
            asn1_DisposeString(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}



VECTOR *asn1_UnpackSequence(PDFLibHandle lib, ppUns8 *buf, ppInt32 len, TDecodeFunction dec_func, TDisposeFunction free_func)
{
    VECTOR *v;
    ppUns8 *p;
    p = buf;
    v = asn1_DecodeSet(lib, NULL, &p, len, dec_func, free_func, SEQUENCE_TAG, UNIVERSAL_CLS);
    return v;
}

/*
ppUns8 *asn1_PackSequence(VECTOR *v, ppInt32 (*enc_func)(), ppUns8 **pbuf, ppInt32 *plen)
{
ppInt32 outlen;
ppUns8 *outbuf, *p;
outlen = asn1_EncodeSet(v, NULL, enc_func, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE); 
if (!outlen) return NULL; 
if (!(outbuf = malloc(outlen))) return NULL;
p = outbuf;
asn1_EncodeSet(v, &p, enc_func, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE); 
if (plen) *plen = outlen;
if (pbuf) *pbuf = outbuf;
return outbuf;
}
*/

void *asn1_UnpackString(PDFLibHandle lib, asn1_String *s, TDecodeFunction dec_func )
{
    ppUns8 *p = s->data;
    return dec_func(lib, NULL, &p, s->length);
}

/*
asn1_String *asn1_PackString(void *obj, ppInt32 (*enc_func)(), asn1_String **pstr)
{
ppUns8 *p;
asn1_String *tmp;

if (!pstr || !*pstr) 
{
if (!(tmp = asn1_CreateString())) return NULL;
if (pstr) *pstr = tmp;
} 
else 
tmp = *pstr;

if (!(tmp->length = enc_func(obj, NULL))) return NULL;
if (!(p = malloc(tmp->length))) return NULL;
tmp->data = p;
enc_func(obj, &p);
return tmp;
}
*/

/*=== ASN.1 ...STRINGs routines === */

void asn1_SetString(PDFLibHandle lib, asn1_String *str, const void *d, ppInt32 l)
{
    ppUns8 *c;
    const char *data = (char * )d;

    if (l < 0)
    {
        if (data == NULL) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));
        l = (ppInt32)strlen(data);
    }
    if (str->length < l || str->data == NULL)
    {
        c = str->data;
        if (!c) 
            str->data = ( ppUns8 * ) mmalloc(lib, l + 1);
        else 
            str->data = ( ppUns8 * ) mrealloc(lib, c, l + 1);
    }
    str->length = l;
    if (data != NULL)
    {
        memcpy(str->data, data, l);
        str->data[l] = '\0';
    }
}

asn1_String *asn1_CloneString(PDFLibHandle lib, asn1_String *str)
{
    asn1_String *rslt = NULL;
    PDFTRY(lib){
        if (str != NULL) 
        {
            rslt = asn1_CreateTypedString(lib, str->type);
            asn1_SetString(lib, rslt, str->data, str->length);
            rslt->bits_left = str->bits_left;
        }
    }
    PDFEXCEPT(lib){
        if (rslt != NULL)
        {
            asn1_DisposeString(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

asn1_String *asn1_CreateString(PDFLibHandle lib)
{
    return asn1_CreateTypedString(lib, OCTETSTRING_TAG);
}

asn1_String *asn1_CreateTypedString(PDFLibHandle lib, ppInt32 type)
{
    asn1_String *rslt;
    rslt = (asn1_String* )mmalloc(lib, sizeof(asn1_String));
    rslt->length = 0;
    rslt->type = type;
    rslt->data = NULL;
    rslt->bits_left = 0;
    return rslt;
}

void asn1_DisposeString(PDFLibHandle lib, asn1_String *str)
{
    if (str == NULL) return;
    if (str->data != NULL) mfree(lib, str->data);
    mfree(lib, str);
}

ppInt32 asn1_CompareStrings(asn1_String *a, asn1_String *b)
{
    ppInt32 i;

    i = a->length - b->length;
    if (i == 0)
    {
        i = memcmp(a->data, b->data, a->length);
        return i == 0 ? (a->type - b->type) : i;
    }
    else return i;
}



asn1_OctetString *asn1_CreateOctetString(PDFLibHandle lib)
{
    return (asn1_OctetString* )asn1_CreateTypedString(lib, OCTETSTRING_TAG);
}

void asn1_DisposeOctetString(PDFLibHandle lib, asn1_OctetString *str)
{
    asn1_DisposeString(lib, (asn1_String* )str);
}

ppInt32 asn1_EncodeOctetString(asn1_OctetString *str, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )str, pbuf, OCTETSTRING_TAG, UNIVERSAL_CLS);
}

asn1_OctetString *asn1_DecodeOctetString(PDFLibHandle lib, asn1_OctetString **pstr, ppUns8 **pbuf, ppInt64 length)
{
    asn1_OctetString *rslt;
    rslt = (asn1_OctetString* )asn1_DecodeBytes(lib, (asn1_String** )pstr, pbuf, length, OCTETSTRING_TAG, UNIVERSAL_CLS);
    return rslt;
}



ppInt32 asn1_EncodePrintable(asn1_String *str, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )str, pbuf, str->type, UNIVERSAL_CLS);
}

asn1_String *asn1_DecodePrintable(PDFLibHandle lib, asn1_String **pstr, ppUns8 **pbuf, ppInt64 max)
{ 
    return asn1_DecodeTypedBytes(lib, (asn1_String** )pstr, pbuf, max,
        PRINTABLESTRING_BIT|T61STRING_BIT|IA5STRING_BIT|BITSTRING_BIT|
        UNIVERSALSTRING_BIT|BMPSTRING_BIT|UTF8STRING_BIT|UNKNOWN_BIT);
}



asn1_IA5String *asn1_CreateIA5String(PDFLibHandle lib)
{
    return (asn1_IA5String* )asn1_CreateTypedString(lib, IA5STRING_TAG);
}

void asn1_DisposeIA5String(PDFLibHandle lib, asn1_IA5String *str)
{
    asn1_DisposeString(lib, (asn1_String* )str);
}

ppInt32 asn1_EncodeIA5String(asn1_IA5String *str, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )str, pbuf, IA5STRING_TAG, UNIVERSAL_CLS);
}

asn1_IA5String *asn1_DecodeIA5String(PDFLibHandle lib, asn1_IA5String **pstr, ppUns8 **pbuf, ppInt64 length)
{
    asn1_IA5String *rslt;
    rslt = (asn1_IA5String* )asn1_DecodeBytes(lib, (asn1_String** )pstr, pbuf, length, IA5STRING_TAG, UNIVERSAL_CLS);
    return rslt;
}



asn1_BMPString *asn1_CreateBMPString(PDFLibHandle lib)
{ 
    return (asn1_BMPString* )asn1_CreateTypedString(lib, BMPSTRING_TAG);
}

void asn1_DisposeBMPString(PDFLibHandle lib, asn1_BMPString *str)
{
    asn1_DisposeString(lib, (asn1_String* )str);
}

ppInt32 asn1_EncodeBMPString(asn1_BMPString *str, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )str, pbuf, BMPSTRING_TAG, UNIVERSAL_CLS);
}

asn1_BMPString *asn1_DecodeBMPString(PDFLibHandle lib, asn1_BMPString **pstr, ppUns8 **pbuf, ppInt64 length)
{
    asn1_BMPString *rslt;
    rslt = (asn1_BMPString* )asn1_DecodeBytes(lib, (asn1_String** )pstr, pbuf, length, BMPSTRING_TAG, UNIVERSAL_CLS);
    return rslt;
}

asn1_BitString *asn1_CreateBitString(PDFLibHandle lib)
{
    return (asn1_BitString* )asn1_CreateTypedString(lib, BITSTRING_TAG);
}

void asn1_DisposeBitString(PDFLibHandle lib, asn1_BitString *str)
{
    asn1_DisposeString(lib, (asn1_String* )str);
}

ppInt32 asn1_EncodeBitStringContent(asn1_BitString *str, ppUns8 **pbuf)
{
    ppInt32 rslt, j, bits, len;
    ppUns8 *p, *d;

    if (str == NULL)
        return 0;

    bits = 0;
    len = str->length;

    if (len > 0)
    {
        if (str->bits_left & BITS_LEFT_FLAG)
            bits = (ppInt32)str->bits_left & 0x07;
        else
        {
            for ( ; len > 0; len--)
                if (str->data[len - 1]) break;
            j = str->data[len - 1];
            if      (j & 0x01) bits = 0;
            else if (j & 0x02) bits = 1;
            else if (j & 0x04) bits = 2;
            else if (j & 0x08) bits = 3;
            else if (j & 0x10) bits = 4;
            else if (j & 0x20) bits = 5;
            else if (j & 0x40) bits = 6;
            else if (j & 0x80) bits = 7;
        }
    }
    else
        bits = 0;

    rslt = 1 + len;
    if (pbuf == NULL) return rslt;

    p = *pbuf;
    *(p++) = (ppUns8)bits;
    d = str->data;
    memcpy(p, d, len);
    p += len;
    if (len > 0) *(p - 1) &= (0xff << bits);
    *pbuf = p;
    return rslt;
}

ppInt32 asn1_EncodeBitString(asn1_BitString *str, ppUns8 **pbuf)
{
    ppInt32 len, rslt;
    len = asn1_EncodeBitStringContent(str, NULL);
    rslt = asn1_ObjectSize(0, len, BITSTRING_TAG);
    if (pbuf)
    {
        asn1_PutObject(pbuf, 0, len, BITSTRING_TAG, UNIVERSAL_CLS);
        asn1_EncodeBitStringContent(str, pbuf);
    }
    return rslt;
}

asn1_BitString *asn1_DecodeBitStringContent(PDFLibHandle lib, asn1_BitString **pstr, ppUns8 **pbuf, ppInt64 len)
{
    asn1_BitString *rslt = NULL;
    ppUns8 *p, *s;
    ppInt32 i;

    PDFTRY(lib){
        if (pstr == NULL || (*pstr) == NULL)
            rslt = asn1_CreateBitString(lib);
        else
            rslt = (*pstr);

        p = *pbuf; i = *(p++);
        rslt->bits_left &= ~(BITS_LEFT_FLAG | 0x07); 
        rslt->bits_left |= (BITS_LEFT_FLAG | (i & 0x07));

        if (len-- > 1) 
        {
            s = (ppUns8 * )mmalloc(lib, (ppInt32)len);
            memcpy(s, p, (ppInt32)len);
            s[(ppInt32)len - 1] &= (ppUns8)(0xff << i);
            p = p + (ppInt32)len;
        }
        else
            s = NULL;

        rslt->length = (ppInt32)len;
        if (rslt->data != NULL) mfree(lib, rslt->data);
        rslt->data = s;
        rslt->type = BITSTRING_TAG;
        if (pstr != NULL) (*pstr) = rslt;
        (*pbuf) = p;
    }PDFEXCEPT(lib){
        if (rslt != NULL && (pstr == NULL || (*pstr) != rslt))
        {
            asn1_DisposeBitString(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

asn1_BitString *asn1_DecodeBitString(PDFLibHandle lib, asn1_BitString **pstr, ppUns8 **pbuf, ppInt64 max)
{
    ppUns8 *p;
    ppInt64 len;
    ppInt32 inf, tag, cls;
    asn1_BitString *rslt;

    p = *pbuf;
    inf = asn1_GetObject(&p, &len, &tag, &cls, max);
    if (inf & 0x80) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
    if (tag != BITSTRING_TAG)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
    if (len < 1)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
    rslt = asn1_DecodeBitStringContent(lib, pstr, &p, len);
    (*pbuf) = p;

    return rslt;
}

/*=== ASN.1 TIMEs routines === */

asn1_Time *asn1_CreateTime(PDFLibHandle lib)
{
    return (asn1_Time* )asn1_CreateTypedString(lib, UTCTIME_TAG);
}

void asn1_DisposeTime(PDFLibHandle lib, asn1_Time *tm)
{
    asn1_DisposeString(lib, (asn1_String* )tm);
}

ppInt32 asn1_EncodeTime(asn1_Time *tm, ppUns8 **pbuf)
{
    if (tm->type == UTCTIME_TAG || tm->type == GENERALIZEDTIME_TAG)
        return asn1_EncodeBytes((asn1_String* )tm, pbuf, tm->type, UNIVERSAL_CLS);
    return 0;
}

asn1_Time *asn1_DecodeTime(PDFLibHandle lib, asn1_Time **ptm, ppUns8 **pbuf, ppInt64 length)
{
    ppUns8 tag;
    tag = (**pbuf) & (~CONSTRUCTED);
    if (tag == (UTCTIME_TAG | UNIVERSAL_CLS))
        return asn1_DecodeUTCTime(lib, ptm, pbuf, length);
    if (tag == (GENERALIZEDTIME_TAG | UNIVERSAL_CLS))
        return asn1_DecodeGeneralizedTime(lib, ptm, pbuf, length);
    return NULL;
}

ppInt32 asn1_EncodeGeneralizedTime(asn1_GeneralizedTime *tm, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )tm, pbuf, GENERALIZEDTIME_TAG, UNIVERSAL_CLS);
}

ppInt32 asn1_CheckGeneralizedTime(asn1_GeneralizedTime *tm)
{
    static ppInt32 min[9] = {  0,  0,  1,  1,  0,  0,  0,  0,  0 };
    static ppInt32 max[9] = { 99, 99, 12, 31, 23, 59, 59, 12, 59 };
    char *a;
    ppInt32 n, i, l, o;

    if (tm->type != GENERALIZEDTIME_TAG) return 0;
    l = tm->length;
    a = (char * )tm->data;
    o = 0;
    if (l < 13) goto err;
    for (i = 0; i < 7; i++)
    {
        if ((i == 6) && ((a[o] == 'Z') || (a[o] == '+') || (a[o] == '-')))
            break; 
        if ((a[o] < '0') || (a[o] > '9')) goto err;
        n = a[o] - '0';
        if (++o > l) goto err;

        if ((a[o] < '0') || (a[o] > '9')) goto err;
        n = (n * 10) + a[o] - '0';
        if (++o > l) goto err;

        if ((n < min[i]) || (n > max[i])) goto err;
    }
    if (a[o] == 'Z') o++;
    else if ((a[o] == '+') || (a[o] == '-'))
    {
        o++;
        if (o + 4 > l) goto err;
        for (i = 7; i < 9; i++)
        {
            if ((a[o] < '0') || (a[o] > '9')) goto err;
            n = a[o] - '0';
            o++;
            if ((a[o] < '0') || (a[o] > '9')) goto err;
            n = (n * 10) + a[o] - '0';
            if ((n < min[i]) || (n > max[i])) goto err;
            o++;
        }
    }
    return (o == l);
err:
    return 0;
}

asn1_GeneralizedTime *asn1_DecodeGeneralizedTime(PDFLibHandle lib, asn1_GeneralizedTime **ptm, ppUns8 **pbuf, ppInt64 length)
{
    asn1_GeneralizedTime *rslt;
    rslt = (asn1_GeneralizedTime* )asn1_DecodeBytes(lib,
        (asn1_String** )ptm, pbuf, length, GENERALIZEDTIME_TAG, UNIVERSAL_CLS);
    if (!(rslt && asn1_CheckGeneralizedTime(rslt)))
    {
        if (rslt != NULL)
        {
            asn1_DisposeString(lib, (asn1_String* )rslt);
            rslt = NULL;
        }
    }
    return rslt;
}

asn1_GeneralizedTime *asn1_CreateGeneralizedTime(PDFLibHandle lib)
{
    return (asn1_GeneralizedTime* )asn1_CreateTypedString(lib, GENERALIZEDTIME_TAG);
}

void asn1_DisposeGeneralizedTime( PDFLibHandle lib, asn1_GeneralizedTime *tm )
{
    asn1_DisposeString( lib, (asn1_String* )tm);
}



ppInt32 asn1_EncodeUTCTime(asn1_UTCTime *tm, ppUns8 **pbuf)
{
    return asn1_EncodeBytes((asn1_String* )tm, pbuf, UTCTIME_TAG, UNIVERSAL_CLS);
}

ppInt32 asn1_CheckUTCTime(asn1_UTCTime *tm)
{
    static ppInt32 min[8] = {  0,  1,  1,  0,  0,  0,  0,  0 };
    static ppInt32 max[8] = { 99, 12, 31, 23, 59, 59, 12, 59 };
    char *a;
    ppInt32 n, i, l, o;

    if (tm->type != UTCTIME_TAG) return 0;
    l = tm->length;
    a = (char * )tm->data;
    o = 0;

    if (l < 11) goto err;
    for (i = 0; i < 6; i++)
    {
        if ((i == 5) && ((a[o] == 'Z') || (a[o] == '+') || (a[o] == '-')))
            break; 
        if ((a[o] < '0') || (a[o] > '9')) goto err;
        n = a[o] - '0';
        if (++o > l) goto err;

        if ((a[o] < '0') || (a[o] > '9')) goto err;
        n = (n * 10) + a[o] - '0';
        if (++o > l) goto err;

        if ((n < min[i]) || (n > max[i])) goto err;
    }
    if (a[o] == 'Z') o++;
    else if ((a[o] == '+') || (a[o] == '-'))
    {
        o++;
        if (o + 4 > l) goto err;
        for (i = 6; i < 8; i++)
        {
            if ((a[o] < '0') || (a[o] > '9')) goto err;
            n = a[o] - '0';
            o++;
            if ((a[o] < '0') || (a[o] > '9')) goto err;
            n = (n * 10) + a[o] - '0';
            if ((n < min[i]) || (n > max[i])) goto err;
            o++;
        }
    }
    return (o == l);
err:
    return 0;
}

asn1_UTCTime *asn1_DecodeUTCTime(PDFLibHandle lib, asn1_UTCTime **ptm, ppUns8 **pbuf, ppInt64 length)
{
    asn1_UTCTime *rslt;
    rslt = (asn1_UTCTime* )asn1_DecodeBytes(lib, 
        (asn1_String** )ptm, pbuf, length, UTCTIME_TAG, UNIVERSAL_CLS);
    if (!(rslt && asn1_CheckUTCTime(rslt)))
    {
        if (rslt != NULL)
        {
            asn1_DisposeString(lib, (asn1_String* )rslt);
            rslt = NULL;
        }
    }
    return rslt;
}

asn1_UTCTime *asn1_CreateUTCTime(PDFLibHandle lib)
{
    return (asn1_UTCTime* )asn1_CreateTypedString(lib, UTCTIME_TAG);
}

void asn1_DisposeUTCTime( PDFLibHandle lib, asn1_UTCTime *tm)
{
    asn1_DisposeString( lib, (asn1_String* )tm);
}

/*=== ASN.1 OBJECT routines === */

ppInt32 asn1_EncodeObject(asn1_Object *obj, ppUns8 **pbuf)
{
    ppUns8 *p;
    ppInt32 size;

    if ((obj == NULL) || (obj->data == NULL)) return 0;

    size = asn1_ObjectSize(0, obj->length, OBJECT_TAG);
    if (pbuf == NULL) return size;

    p = *pbuf;
    asn1_PutObject(&p, 0, obj->length, OBJECT_TAG, UNIVERSAL_CLS);
    memcpy(p, obj->data, obj->length);
    p += obj->length;

    *pbuf = p;
    return size;
}

asn1_Object *asn1_DecodeObjectContent(PDFLibHandle lib, asn1_Object **pobj, ppUns8 **pbuf, ppInt64 len)
{
    asn1_Object *rslt = NULL;
    ppUns8 *p;
    PDFTRY(lib) {
        if (pobj == NULL || (*pobj) == NULL)
            rslt = asn1_CreateObject(lib);
        else
            rslt = (*pobj);

        p = *pbuf;
        if (rslt->data == NULL || rslt->length < len)
        {
            if (rslt->data != NULL) mfree(lib, rslt->data);
            rslt->data = (ppUns8 * )mmalloc(lib, (len ? (ppInt32)len : 1));
        }
        memcpy(rslt->data, p, (ppInt32)len);
        rslt->length = (ppInt32)len;
        p = p + (ppInt32)len;

        if (pobj != NULL) (*pobj) = rslt;
        (*pbuf) = p;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pobj == NULL || (*pobj) != rslt))
        {
            asn1_DisposeObject(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

asn1_Object *asn1_DecodeObject(PDFLibHandle lib, asn1_Object **pobj, ppUns8 **pbuf, ppInt64 length)
{
    ppUns8 *p;
    ppInt64 len;
    ppInt32 tag, cls, inf;
    asn1_Object *rslt = NULL;

    PDFTRY(lib){
        p = *pbuf;
        inf = asn1_GetObject(&p, &len, &tag, &cls, length);
        if (inf & 0x80) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
        if (tag != OBJECT_TAG) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
        rslt = asn1_DecodeObjectContent(lib, pobj, &p, len);
        *pbuf = p;
    } PDFEXCEPT(lib) {
        if (rslt != NULL && (pobj == NULL || (*pobj) != rslt))
        {
            asn1_DisposeObject(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

asn1_Object *asn1_CreateObject(PDFLibHandle lib)
{
    asn1_Object *rslt;
    rslt = (asn1_Object* )mmalloc(lib, sizeof(asn1_Object));
    rslt->length = 0;
    rslt->data = NULL;
    return rslt;
}

void asn1_DisposeObject(PDFLibHandle lib, asn1_Object *obj)
{
    if (obj == NULL) return;
    if (obj->data != NULL) mfree( lib, obj->data );
    obj->data = NULL;
    obj->length = 0;
    mfree(lib, obj);
}

/*=== ASN.1 INTEGER and BOOLEAN routines === */
/*!!! ТОЛЬКО НЕОТРИЦАТЕЛЬНЫЕ !!! */
ppInt32 asn1_EncodeInteger(asn1_Integer *arg, ppUns8 **pbuf)
{
    ppInt32 l, r = 0;
    if (arg != NULL) 
    {
        l = asn1_EncodeIntegerContent(arg, NULL);
        r = asn1_ObjectSize(0, l, INTEGER_TAG);

        if (pbuf)
        {
            asn1_PutObject(pbuf, 0, l, INTEGER_TAG, UNIVERSAL_CLS);
            asn1_EncodeIntegerContent(arg, pbuf);
        }
    }
    return r;
}

ppInt32 asn1_EncodeIntegerContent(asn1_Integer *arg, ppUns8 **pbuf)
{
    ppInt32 pad = 0, rslt;
    ppUns8 *p, pb = 0;

    rslt = 0;
    if (arg != NULL && arg->data != NULL) 
    {
        if (arg->length == 0) rslt = 1;
        else
        {
            rslt = arg->length;
            if (arg->data[0] > 127) { pad = 1; pb = 0; }
            rslt += pad;
        }
        if (pbuf != NULL)
        {
            p = *pbuf;
            if (pad) *(p++) = pb;
            if (arg->length == 0)
                *p = 0;
            else
                memcpy(p, arg->data, arg->length);
            (*pbuf) += rslt;
        }
    }
    return rslt;
}

asn1_Integer *asn1_DecodeInteger(PDFLibHandle lib, asn1_Integer **parg, ppUns8 **pbuf, ppInt64 max)
{
    ppUns8 *p;
    asn1_Integer *rslt;
    ppInt32 inf, tag, cls;
    ppInt64 len;

    p = *pbuf;
    inf = asn1_GetObject(&p, &len, &tag, &cls, max);
    if (inf & 0x80) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
    if (tag != INTEGER_TAG) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
    rslt = asn1_DecodeIntegerContent(lib, parg, &p, len);
    (*pbuf) = p;
    return rslt;
}

asn1_Integer *asn1_DecodeIntegerContent(PDFLibHandle lib, asn1_Integer **parg, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Integer *rslt = NULL;
    ppUns8 *p, *s, *pend;

    PDFTRY(lib){
        if (parg == NULL || (*parg) == NULL)
            rslt = asn1_CreateInteger(lib);
        else
            rslt = (*parg);

        p = *pbuf;
        pend = p + (ppInt32)max;

        s = (ppUns8 * )mmalloc(lib, (size_t)max + 1);
        if (max > 0)
        {
            if (*p == 0 && max > 1) { p++; max--; }
            memcpy(s, p, (size_t)max);
        }

        if (rslt->data != NULL) mfree(lib, rslt->data);
        rslt->data = s;
        rslt->length = (ppInt32)max;
        if (parg != NULL) (*parg) = rslt;
        *pbuf = pend;
    } PDFEXCEPT(lib) {
        if (rslt != NULL)
        {
            asn1_DisposeInteger(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

asn1_Integer *asn1_CreateInteger(PDFLibHandle lib)
{
    return (asn1_Integer* )asn1_CreateTypedString(lib, INTEGER_TAG);
}

void asn1_DisposeInteger(PDFLibHandle lib, asn1_Integer *arg)
{
    asn1_DisposeString(lib, (asn1_String* )arg);
}

ppInt32 asn1_SetInteger(PDFLibHandle lib, asn1_Integer *arg, ppInt64 val)
{
    ppInt32 i, j, k, r = 1;
    ppUns8 buf[sizeof(ppInt64) + 1];

    arg->type = INTEGER_TAG;
    if (arg->length < (ppInt32)sizeof(ppInt64) + 1)
    {
        if (arg->data != NULL) mfree(lib, arg->data);
        arg->data = (ppUns8 * )mmalloc(lib, sizeof(ppInt64) + 1);
        memset((char * )arg->data, 0, sizeof(ppInt64) + 1);
    }
    if (arg->data == NULL)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));
    for (i = 0; i < (ppInt32)sizeof(ppInt64); i++)
    {
        if (val == 0) break;
        buf[i] = (ppUns8)( val & 0xFF );
        val >>= 8;
    }
    j = 0;
    for (k = i - 1; k >= 0; k--)
        arg->data[j++] = buf[k];
    arg->length = j;

    return r;
}

ppInt64 asn1_GetInteger(asn1_Integer *arg)
{
    ppInt32 i;
    ppInt64 r = 0;

    if (arg == NULL)
        return 0L;
    if (arg->type != INTEGER_TAG)
        return 0L;
    if (arg->length > (ppInt32)sizeof(ppInt64))
        return 0xffffffffL;
    if (arg->data == NULL)
        return 0L;
    for (i = 0; i < arg->length; i++)
    {
        r <<= 8;
        r |= (ppUns8)arg->data[i];
    }
    return r;
}



ppInt32 asn1_EncodeBoolean(ppInt32 val, ppUns8 **pbuf)
{
    ppInt32 r;
    ppUns8 *p;

    r = asn1_ObjectSize(0, 1, BOOLEAN_TAG);
    if (pbuf == NULL) return r;
    p = *pbuf;

    asn1_PutObject(&p, 0, 1, BOOLEAN_TAG, UNIVERSAL_CLS);
    *(p++) = (ppUns8)val;
    *pbuf = p;

    return r;
}

ppInt32 asn1_DecodeBoolean(PDFLibHandle lib, ppInt32 *pval, ppUns8 **pbuf, ppInt64 max)
{
    ppInt32 rslt;
    ppUns8 *p;
    ppInt64 len;
    ppInt32 inf, tag, cls;

    p = (*pbuf);
    inf = asn1_GetObject(&p, &len, &tag, &cls, max);
    if ((inf & 0x80) != 0) 
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
    if (tag != BOOLEAN_TAG)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
    if (len != 1)
        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
    rslt = (ppInt32)*(p++);
    if (pval != NULL) (*pval) = rslt;
    (*pbuf) = p;

    return rslt;
}

/*=== ASN.1 SET OF ... routines === */

typedef struct
{
    ppUns8 *data;
    ppInt32 length;
} SetOfObject;

int CDECL SetOfObjectCmp(const void *item1, const void *item2)
{
    const SetOfObject *b1 = (const SetOfObject* )item1;
    const SetOfObject *b2 = (const SetOfObject* )item2;
    ppInt32 r;

    r = memcmp(b1->data, b2->data,
        b1->length < b2->length ? b1->length : b2->length);
    if (r != 0) return r;
    return b1->length - b2->length;
}

/*Если set, сортировать содержимое вектора (НЕ SEQUENCE) */
/*TO DO: заменить malloc(...) на mmalloc(lib, ...) */
ppInt32 asn1_EncodeSet(VECTOR *v, ppUns8 **pbuf, TEncodeFunction enc_func, ppInt32 tag, ppInt32 cls, ppInt32 set)
{
    ppInt32 rslt = 0, r, i;
    ppUns8 *p, *start, *tmp;
    SetOfObject *setof;
    ppInt32 total;

    if (v == NULL) return 0;
    for (i = vector_GetCount(v) - 1; i >= 0; i--)
        rslt += enc_func(vector_GetValueAt(v, i), NULL);
    r = asn1_ObjectSize(1, rslt, tag);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, tag, cls);

    if (!set || (vector_GetCount(v) < 2))
    {
        for (i = 0; i < vector_GetCount(v); i++)
            enc_func(vector_GetValueAt(v, i), &p);
        *pbuf = p;
        return r;
    }

    start = p; 
    setof = (SetOfObject* )mmalloc( v->lib, vector_GetCount(v)*sizeof(SetOfObject));
    if (!setof)
        return 0;

    for (i = 0; i < vector_GetCount(v); i++)
    {
        setof[i].data = p;  
        enc_func(vector_GetValueAt(v, i), &p);
        setof[i].length = (ppInt32)(p - setof[i].data);
    }
    *pbuf = p;
    total = (ppInt32)(p - start);

    qsort(setof, vector_GetCount(v), sizeof(SetOfObject), SetOfObjectCmp);

    tmp = ( ppUns8 * )mmalloc(v->lib, total);
    if (!tmp)
        return 0;
    p = tmp;
    for (i = 0; i < vector_GetCount(v); i++)
    {
        memcpy(p, setof[i].data, setof[i].length);
        p += setof[i].length;
    }
    memcpy(start, tmp, total);
    mfree(v->lib, tmp);
    mfree(v->lib, setof);

    return r;
}

VECTOR *asn1_DecodeSet(PDFLibHandle lib, VECTOR **pv, ppUns8 **pbuf, ppInt64 max,
                       TDecodeFunction dec_func, TDisposeFunction free_func, ppInt32 tag, ppInt32 cls)
{
    asn1_Context ctx;
    VECTOR *rslt = NULL;
    PDFTRY(lib){
        if (pv == NULL || (*pv) == NULL)
            rslt = vector_Create(lib);
        else
            rslt = (*pv);

        ctx.p = (*pbuf);
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;

        ctx.inf = asn1_GetObject(&ctx.p, &ctx.len, &ctx.tag, &ctx.cls, (ppInt64)(ctx.max - ctx.p));
        if ((ctx.inf & 0x80) != 0 || ctx.cls != cls) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
        if (ctx.tag != tag) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleWrongAsn1TagError));
        if (((ppInt32)ctx.len + ctx.p) > ctx.max) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
        if (ctx.inf == (CONSTRUCTED + 1))
            ctx.len = max + (ppInt64)(*pbuf - ctx.p);
        ctx.max = ctx.p + (ppInt32)ctx.len;

        while (ctx.p < ctx.max)
        {
            void *s;
            if (asn1_CheckEndSequence(&ctx)) break;
            s = dec_func(lib, NULL, &ctx.p, ctx.len);
            vector_Push(rslt, ( char * ) s);
        }

        if (pv != NULL) (*pv) = rslt;
        (*pbuf) = ctx.p;
    }PDFEXCEPT(lib) {
        if (rslt != NULL && (pv == NULL || *pv != rslt))
        {
            if (free_func != NULL)
                vector_Dispose(rslt, free_func);
            else
                vector_Free(rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib) ;
    return rslt;
}



void *asn1_Clone(PDFLibHandle lib, TEncodeFunction enc_func, TDecodeFunction dec_func, void *obj)
{
    ppUns8 *buf = NULL, *p;
    ppInt32 len;
    void *rslt = NULL;
    PDFTRY(lib){
        len = enc_func(obj, NULL);
        buf = ( ppUns8 * ) mmalloc(lib, len + 1);
        p = buf; 
        enc_func(obj, &p);
        p = buf;
        rslt = dec_func(lib, NULL, &p, len);
        mfree(lib, buf);
    } PDFEXCEPT(lib){
        if (buf != NULL) 
            mfree(lib, buf);
        PDFRERAISE(lib);
    }PDFTRYEND(lib)
        return rslt;
}

#endif