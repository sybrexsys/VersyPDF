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
VSkX509I.h
*********************************************************************/

#ifndef VSX509IH
#define VSX509IH

#include "../VSExcept.h"
#include "VSLibI.h"

#include "VSkRSAI.h"

#ifndef NOT_USE_SIGN

typedef struct
{
	asn1_Object *algorithm;
	asn1_Type *parameter;
} x509_Algorithm;

typedef struct
{
	asn1_Object *object;
	asn1_String *value;
	ppInt32 set;
	ppInt32 size;
} x509_NameEntry;

typedef struct
{
	VECTOR *entries;
	BUFFER *bytes;
	ppInt32 modified;
} x509_Name;

typedef struct
{
	asn1_Time *notBefore;
	asn1_Time *notAfter;
} x509_Validity;

typedef struct
{
	x509_Algorithm *algorithm;
	asn1_BitString *publicKey;
	rsa_Key *rsaKey;
} x509_PublicKey;

typedef struct
{
	asn1_Object *object;
	ppInt16 critical;
	asn1_OctetString *value;
} x509_Extension;

typedef struct
{
	asn1_Integer *version;			/*[ 0 ] default of v1 */
	asn1_Integer *serialNumber;
	x509_Algorithm *signature;
	x509_Name *issuer;
	x509_Validity *validity;
	x509_Name *subject;
	x509_PublicKey *key;
	asn1_BitString *issuerUID;		/*[ 1 ] optional in v2 */
	asn1_BitString *subjectUID;		/*[ 2 ] optional in v2 */
	VECTOR *extensions;				/*[ 3 ] optional in v3 */
} x509_CertInfo;

typedef struct
{
	x509_CertInfo *cert_info;
	x509_Algorithm *sig_alg;
	asn1_BitString *signature;
	ppUns8 *asn1;
	ppInt64 length;
} x509;

typedef struct
{
	asn1_Integer *serialNumber;
	asn1_Time *revocationDate;
	VECTOR *extensions;						/*x509_Extension's ( opt ) */
	ppInt32 sequence;							/*load sequence ( ? ) */
} x509_Revoked;

typedef struct 
{
	asn1_Integer *version;
	x509_Algorithm *signature;
	x509_Name *issuer;
	asn1_Time *thisUpdate;
	asn1_Time *nextUpdate;
	VECTOR *revokedCertificates;			/*x509_Revoked's */ 
	VECTOR *crlExtensions;						/*x509_Extension's [0] */
} x509_CRLInfo;

typedef struct
{
	x509_CRLInfo *certList;
	x509_Algorithm *signatureAlgorithm;
	asn1_BitString *signatureValue;
} x509_CRL;

typedef struct
{
	asn1_Object *object;
	ppInt32 set; /*1 для множества */
	union	
	{
		char *ptr;
		VECTOR *set;			/*( 1 ) asn_Type's */
		asn1_Type *single;		/*( 0 ) не д.быть */
	} value;
} x509_Attribute;


#ifdef  __cplusplus
extern "C" {
#endif

    x509_Algorithm *x509_CreateAlgorithm( PDFLibHandle );
    void x509_DisposeAlgorithm(PDFLibHandle lib, x509_Algorithm *alg);
    ppInt32 x509_EncodeAlgorithm( x509_Algorithm *, ppUns8 ** );
    x509_Algorithm*	x509_DecodeAlgorithm( PDFLibHandle, x509_Algorithm **, ppUns8 **, ppInt64 );

    x509_Validity *x509_CreateValidity( PDFLibHandle );
	void x509_DisposeValidity(PDFLibHandle lib, x509_Validity *val);
    ppInt32 x509_EncodeValidity( x509_Validity *, ppUns8 ** );
    x509_Validity *x509_DecodeValidity( PDFLibHandle, x509_Validity **, ppUns8 **, ppInt64 );

    x509_PublicKey *x509_CreatePublicKey( PDFLibHandle );
	void x509_DisposePublicKey(PDFLibHandle lib, x509_PublicKey *key);
    ppInt32 x509_EncodePublicKey( x509_PublicKey *, ppUns8 ** );
    x509_PublicKey *x509_DecodePublicKey( PDFLibHandle, x509_PublicKey **, ppUns8 **, ppInt64 );

    x509_Extension *x509_CreateExtension( PDFLibHandle );
    void x509_DisposeExtension(PDFLibHandle lib, x509_Extension *ext);
    ppInt32 x509_EncodeExtension( void *, ppUns8 ** );
    x509_Extension *x509_DecodeExtension( PDFLibHandle, x509_Extension **, ppUns8 **, ppInt64 );

    x509_NameEntry *x509_CreateNameEntry( PDFLibHandle );
	void x509_DisposeNameEntry(PDFLibHandle lib, x509_NameEntry *entry);
    ppInt32 x509_EncodeNameEntry( x509_NameEntry *, ppUns8 ** );
    x509_NameEntry *x509_DecodeNameEntry( PDFLibHandle, x509_NameEntry **, ppUns8 **, ppInt64 );

    x509_Name *x509_CreateName( PDFLibHandle );
    void x509_DisposeName(PDFLibHandle lib, x509_Name *name);
    ppInt32 x509_EncodeName( x509_Name *, ppUns8 ** );
    x509_Name *x509_DecodeName( PDFLibHandle, x509_Name **, ppUns8 **, ppInt64 );

    typedef ppInt32 ( *x509_EncodeNameFunc )( x509_Name *, ppUns8 ** );
    typedef x509_Name *( *x509_DecodeNameFunc )( PDFLibHandle, x509_Name **, ppUns8 **, ppInt64 );
    typedef x509_Name *( *x509_CloneNameFunc )( PDFLibHandle, x509_EncodeNameFunc, x509_DecodeNameFunc, x509_Name * );
    		
    x509_CertInfo *x509_CreateCertInfo( PDFLibHandle );
	void x509_DisposeCertInfo(PDFLibHandle lib, x509_CertInfo *info);
    ppInt32 x509_EncodeCertInfo( x509_CertInfo *, ppUns8 ** );
    x509_CertInfo *x509_DecodeCertInfo( PDFLibHandle, x509_CertInfo **, ppUns8 **, ppInt64 );

    x509 *x509_Create( PDFLibHandle );
	void x509_Dispose(PDFLibHandle lib, x509 *cert);
    ppInt32 x509_Encode( x509 *, ppUns8 ** );
    x509 *x509_Decode( PDFLibHandle, x509 **, ppUns8 **, ppInt64 );

    typedef ppInt32 ( *x509_EncodeFunc )( x509 *, ppUns8 ** );
    typedef x509 *( *x509_DecodeFunc )( PDFLibHandle, x509 **, ppUns8 **, ppInt64 );
    typedef x509 *( *x509_CloneFunc )( PDFLibHandle, x509_EncodeFunc, x509_DecodeFunc, x509 * );

    rsa_Key *x509_GetPublicKey( PDFLibHandle, x509 * );
    char *x509_GetSubjectCommonName( PDFLibHandle, x509 * );

    x509_Revoked *x509_CreateRevoked( PDFLibHandle );
    void x509_DisposeRevoked(PDFLibHandle lib, x509_Revoked *cert);
    ppInt32 x509_EncodeRevoked( x509_Revoked *, ppUns8 ** );
    x509_Revoked *x509_DecodeRevoked( PDFLibHandle, x509_Revoked **, ppUns8 **, ppInt64 );

    x509_CRLInfo *x509_CreateCRLInfo( PDFLibHandle );
    void x509_DisposeCRLInfo(PDFLibHandle lib, x509_CRLInfo *info);
    ppInt32 x509_EncodeCRLInfo( x509_CRLInfo *, ppUns8 ** );
    x509_CRLInfo *x509_DecodeCRLInfo( PDFLibHandle, x509_CRLInfo **, ppUns8 **, ppInt64 );

    x509_CRL *x509_CreateCRL( PDFLibHandle );
    void x509_DisposeCRL(PDFLibHandle lib, x509_CRL *crl);
    ppInt32 x509_EncodeCRL( x509_CRL *, ppUns8 ** );
    x509_CRL *x509_DecodeCRL( PDFLibHandle, x509_CRL **, ppUns8 **, ppInt64 );

    x509_Attribute *x509_CreateAttribute( PDFLibHandle );
	void x509_DisposeAttribute(PDFLibHandle lib, x509_Attribute *attr);
    ppInt32 x509_EncodeAttribute( x509_Attribute *, ppUns8 ** );
    x509_Attribute *x509_DecodeAttribute( PDFLibHandle, x509_Attribute **, ppUns8 **, ppInt64 );

#ifdef  __cplusplus
}
#endif

#endif

#endif /*endif VSX509IH */
