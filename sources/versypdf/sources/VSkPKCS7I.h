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
VSkPKCS7I.h
*********************************************************************/

#ifndef VSPKCS7IH
#define VSPKCS7IH

#include "VSkX509I.h"

#ifndef NOT_USE_SIGN

typedef struct
{
    x509_Name *issuer;
    asn1_Integer *serialNumber;
} pkcs7_IssuerAndSerialNumber;

#define pkcs7_IASN pkcs7_IssuerAndSerialNumber

typedef struct
{
    asn1_Integer *version;
    pkcs7_IASN *issuerAndSerialNumber;
    x509_Algorithm *digestAlgorithm;
    VECTOR *authenticatedAttributes;
    x509_Algorithm *digestEncryptionAlgorithm;
    asn1_OctetString *encryptedDigest;
    VECTOR *unauthenticatedAttributes;
} pkcs7_SignerInfo;

#define pkcs7_SI pkcs7_SignerInfo

typedef struct
{
    asn1_Integer *version;
    pkcs7_IASN *issuerAndSerialNumber;
    x509_Algorithm *keyEncryptionAlgorithm;
    asn1_OctetString *encryptedKey;
    x509 *cert;
} pkcs7_RecipientInfo;

#define pkcs7_RI pkcs7_RecipientInfo

typedef struct
{
    asn1_Integer *version;
    VECTOR *digestAlgorithms;	/*x509_Algorithm's */
    VECTOR *certificates;		/*x509's [ 0 ] */
    VECTOR *crls;				/*x509_CRL's[ 1 ] */
    VECTOR *signerInfos;		/*pkcs7_SingerInfo's */
    struct pkcs7_tag *contentInfo;
} pkcs7_SignedData;

#define pkcs7_SIGD pkcs7_SignedData

typedef struct
{
    asn1_Object *content_type;
    x509_Algorithm *algorithm;
    asn1_OctetString *enc_data;
} pkcs7_EncryptedContent;

#define pkcs7_ENCC pkcs7_EncryptedContent

typedef struct
{
    asn1_Integer *version;
    VECTOR *recipient_infos;	/*pkcs7_RecipientInfo's */
    pkcs7_EncryptedContent *enc_data;
} pkcs7_EnvelopedData;

#define pkcs7_ENVD pkcs7_EnvelopedData

typedef struct
{
    asn1_Integer *version;	
    VECTOR *md_algs;				/*x509_Algorithm's */
    VECTOR *certs;					/*x509's [ 0 ] */
    VECTOR *crls;					/*x509_CRL's [ 1 ] */
    VECTOR *signer_infos;			/*pkcs7_SignerInfo's */
    pkcs7_EncryptedContent *enc_data;
    VECTOR *recipient_infos;		/*pkcs7_RecipientInfo's */
} pkcs7_SignedAndEnvelopedData;

#define pkcs7_SAED pkcs7_SignedAndEnvelopedData

typedef struct
{
    asn1_Integer *version;	
    x509_Algorithm *md;
    struct pkcs7_tag *contents;
    asn1_OctetString *digest;
} pkcs7_DigestedData;

#define pkcs7_DIGD pkcs7_DigestedData

typedef struct
{
    asn1_Integer *version;
    pkcs7_EncryptedContent *enc_data;
} pkcs7_EncryptedData;

#define pkcs7_ENCD pkcs7_EncryptedData

typedef struct pkcs7_tag
{
    ppUns8 *asn1;
    ppInt64 length;
    ppInt32 detached;
    asn1_Object *contentType;
    union
    {
        char *ptr;
        asn1_OctetString *data;
        pkcs7_SIGD *signedData;
        pkcs7_ENVD *envelopedData;
        pkcs7_SAED *signedAndEnvelopedData;
        pkcs7_DIGD *digestedData;
        pkcs7_ENCD *encryptedData;
        asn1_Type *other;
    } contents;
} pkcs7;

#define pkcs7_Encode pkcs7_EncodeD

typedef pkcs7 *( *pkcs7_DecodeFunc )( PDFLibHandle, pkcs7 **, ppUns8 **, ppInt64 );
typedef void ( *pkcs7_DisposeFunc )( PDFLibHandle, pkcs7 * );
typedef VECTOR *( *pkcs7_SeqUnpackFunc )( PDFLibHandle, ppUns8 *, ppInt32, pkcs7_DecodeFunc, pkcs7_DisposeFunc );


#ifdef  __cplusplus
extern "C" {
#endif

    pkcs7_IASN *pkcs7_CreateIssuerAndSerialNumber( PDFLibHandle );
	void pkcs7_DisposeIssuerAndSerialNumber(PDFLibHandle lib, pkcs7_IASN *iasn);
    ppInt32 pkcs7_EncodeIssuerAndSerialNumber( pkcs7_IASN *, ppUns8 ** );
    pkcs7_IASN *pkcs7_DecodeIssuerAndSerialNumber( PDFLibHandle, pkcs7_IASN **, ppUns8 **, ppInt64 );
    pkcs7_IASN *pkcs7_IASNFromCertInfo( PDFLibHandle, x509_CertInfo * );

    pkcs7_SI *pkcs7_CreateSignerInfo( PDFLibHandle );
	void pkcs7_DisposeSignerInfo(PDFLibHandle lib, pkcs7_SI *si);
    ppInt32 pkcs7_EncodeSignerInfo( pkcs7_SI *, ppUns8 ** );
    pkcs7_SI *pkcs7_DecodeSignerInfo( PDFLibHandle, pkcs7_SI **, ppUns8 **, ppInt64 );

    pkcs7_RI *pkcs7_CreateRecipientInfo( PDFLibHandle );
	void pkcs7_DisposeRecipientInfo(PDFLibHandle lib, pkcs7_RI *ri);
    ppInt32 pkcs7_EncodeRecipientInfo( pkcs7_RI *, ppUns8 ** );
    pkcs7_RI *pkcs7_DecodeRecipientInfo( PDFLibHandle, pkcs7_RI **, ppUns8 **, ppInt64 );

    ppInt32 pkcs7_DataType( asn1_Object * );

    pkcs7_SIGD *pkcs7_CreateSignedData( PDFLibHandle );
	void pkcs7_DisposeSignedData(PDFLibHandle lib, pkcs7_SignedData *data);
    ppInt32 pkcs7_EncodeSignedData( pkcs7_SIGD *, ppUns8 ** );
    pkcs7_SIGD *pkcs7_DecodeSignedData( PDFLibHandle, pkcs7_SIGD **, ppUns8 **, ppInt64 );

    pkcs7_ENCC *pkcs7_CreateEncryptedContent( PDFLibHandle);
    void pkcs7_DisposeEncryptedContent(PDFLibHandle lib, pkcs7_EncryptedContent *data);
    ppInt32 pkcs7_EncodeEncryptedContent( pkcs7_ENCC *, ppUns8 ** );
    pkcs7_ENCC *pkcs7_DecodeEncryptedContent( PDFLibHandle, pkcs7_ENCC **, ppUns8 **, ppInt64 );

    pkcs7_ENVD *pkcs7_CreateEnvelopedData( PDFLibHandle );
    void pkcs7_DisposeEnvelopedData( pkcs7_ENVD * );
    ppInt32 pkcs7_EncodeEnvelopedData( pkcs7_ENVD *, ppUns8 ** );
    pkcs7_ENVD *pkcs7_DecodeEnvelopedData( PDFLibHandle, pkcs7_ENVD **, ppUns8 **, ppInt64 );

    pkcs7_SAED *pkcs7_CreateSignedAndEnvelopedData( PDFLibHandle );
    void pkcs7_DisposeSignedAndEnvelopedData( pkcs7_SAED * );
    ppInt32 pkcs7_EncodeSignedAndEnvelopedData( pkcs7_SAED *, ppUns8 ** );
    pkcs7_SAED *pkcs7_DecodeSignedAndEnvelopedData( PDFLibHandle, pkcs7_SAED **, ppUns8 **, ppInt64 );

    pkcs7_DIGD *pkcs7_CreateDigestedData( PDFLibHandle );
    void pkcs7_DisposeDigestedData( pkcs7_DIGD * );
    ppInt32 pkcs7_EncodeDigestedData( pkcs7_DIGD *, ppUns8 ** );
    pkcs7_DIGD *pkcs7_DecodeDigestedData( PDFLibHandle, pkcs7_DIGD **, ppUns8 **, ppInt64 );

    pkcs7_ENCD *pkcs7_CreateEncryptedData( PDFLibHandle);
	void pkcs7_DisposeEncryptedData(PDFLibHandle lib, pkcs7_EncryptedData *data);
    ppInt32	pkcs7_EncodeEncryptedData( pkcs7_ENCD *, ppUns8 ** );
    pkcs7_ENCD *pkcs7_DecodeEncryptedData( PDFLibHandle, pkcs7_ENCD **, ppUns8 **, ppInt64 );

    pkcs7 *pkcs7_Create( PDFLibHandle );
	void pkcs7_Dispose(PDFLibHandle lib, pkcs7 *obj);
	void pkcs7_DisposeContent(PDFLibHandle lib,pkcs7 *obj);
    ppInt32 pkcs7_EncodeD( pkcs7 *, ppUns8 ** );
    ppInt32 pkcs7_EncodeI( pkcs7 *, ppUns8 ** );
    pkcs7 *pkcs7_Decode( PDFLibHandle, pkcs7 **, ppUns8 **, ppInt64 );

#ifdef  __cplusplus
}
#endif

#endif

#endif /*endif VSPKCS7IH */
