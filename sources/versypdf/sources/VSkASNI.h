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
VSkASNI.h
*********************************************************************/

#ifndef VSASNIH
#define VSASNIH

#include "../VSTypes.h"
#include "../VSExcept.h"
#include "VSLibI.h"

#include <limits.h>
#ifndef NOT_USE_SIGN
#ifndef INT_MAX
#define INT_MAX       2147483647    /*maximum (signed) ppInt32 value */
#endif

#ifndef LONG_MAX
#define LONG_MAX      2147483647L   /*maximum (signed) ppInt64 value */
#endif

/************************************************************************/
/* Redefine RSA routines                                               */
/************************************************************************/
#ifndef _DEBUG
/************************************************************************/
/* ASN routines                                                        */
/************************************************************************/
#define asn1_CreateObjectByID       VS_ASN_Crypt_CreateObjectByID
#define asn1_GetObjectID            VS_ASN_Crypt_GetObjectID
#define vector_Create               VS_ASN_Crypt_Vector_Create
#define vector_Dispose              VS_ASN_Crypt_Vector_Dispose
#define vector_Free                 VS_ASN_Crypt_Vector_Free
#define vector_GetCount             VS_ASN_Crypt_Vector_GetCount
#define vector_GetValueAt           VS_ASN_Crypt_Vector_GetValueAt
#define vector_SetValueAt           VS_ASN_Crypt_Vector_SetValueAt
#define vector_Insert               VS_ASN_Crypt_Vector_Insert
#define vector_Delete               VS_ASN_Crypt_Vector_Delete
#define vector_Push                 VS_ASN_Crypt_Vector_Push
#define vector_Pop                  VS_ASN_Crypt_Vector_Pop
#define buffer_Create               VS_ASN_Crypt_Buffer_Create
#define buffer_Dispose              VS_ASN_Crypt_Buffer_Dispose
#define buffer_Enlarge              VS_ASN_Crypt_Buffer_Enlarge
#define asn1_ObjectSize             VS_ASN_Crypt_ObjectSize
#define asn1_PutLength              VS_ASN_Crypt_PutLength
#define asn1_PutObject              VS_ASN_Crypt_PutObject
#define asn1_GetLength              VS_ASN_Crypt_GetLength
#define asn1_GetObject              VS_ASN_Crypt_GetObject
#define asn1_GetSequence            VS_ASN_Crypt_GetSequence
#define asn1_CheckInfiniteEnd       VS_ASN_Crypt_CheckInfiniteEnd
#define asn1_CheckEndSequence       VS_ASN_Crypt_CheckEndSequence
#define asn1_Finish                 VS_ASN_Crypt_Finish
#define asn1_EncodeType             VS_ASN_Crypt_EncodeType
#define asn1_DecodeType             VS_ASN_Crypt_DecodeType
#define asn1_CreateType             VS_ASN_Crypt_CreateType
#define asn1_DisposeType            VS_ASN_Crypt_DisposeType
#define asn1_DisposeTypeComponent   VS_ASN_Crypt_DisposeTypeComponent
#define asn1_SetType                VS_ASN_Crypt_SetType
#define asn1_EncodeBytes            VS_ASN_Crypt_EncodeBytes
#define asn1_CollatePrimitive       VS_ASN_Crypt_CollatePrimitive
#define asn1_DecodeBytes            VS_ASN_Crypt_DecodeBytes
#define asn1_DecodeTypedBytes       VS_ASN_Crypt_DecodeTypedBytes
#define asn1_UnpackSequence         VS_ASN_Crypt_UnpackSequence
#define asn1_UnpackString           VS_ASN_Crypt_UnpackString
#define asn1_SetString              VS_ASN_Crypt_SetString
#define asn1_CloneString            VS_ASN_Crypt_CloneString
#define asn1_CreateString           VS_ASN_Crypt_CreateString
#define asn1_CreateTypedString      VS_ASN_Crypt_CreateTypedString
#define asn1_DisposeString          VS_ASN_Crypt_DisposeString
#define asn1_CompareStrings         VS_ASN_Crypt_CompareStrings
#define asn1_CreateOctetString      VS_ASN_Crypt_CreateOctetString
#define asn1_DisposeOctetString     VS_ASN_Crypt_DisposeOctetString
#define asn1_EncodeOctetString      VS_ASN_Crypt_EncodeOctetString
#define asn1_DecodeOctetString      VS_ASN_Crypt_DecodeOctetString
#define asn1_EncodePrintable        VS_ASN_Crypt_EncodePrintable
#define asn1_DecodePrintable        VS_ASN_Crypt_DecodePrintable
#define asn1_CreateIA5String        VS_ASN_Crypt_CreateIA5String
#define asn1_DisposeIA5String       VS_ASN_Crypt_DisposeIA5String
#define asn1_EncodeIA5String        VS_ASN_Crypt_EncodeIA5String
#define asn1_DecodeIA5String        VS_ASN_Crypt_DecodeIA5String
#define asn1_CreateBMPString        VS_ASN_Crypt_CreateBMPString
#define asn1_DisposeBMPString       VS_ASN_Crypt_DisposeBMPString
#define asn1_EncodeBMPString        VS_ASN_Crypt_EncodeBMPString
#define asn1_DecodeBMPString        VS_ASN_Crypt_DecodeBMPString
#define asn1_CreateBitString        VS_ASN_Crypt_CreateBitString
#define asn1_DisposeBitString       VS_ASN_Crypt_DisposeBitString
#define asn1_EncodeBitStringContent VS_ASN_Crypt_EncodeBitStringContent
#define asn1_EncodeBitString        VS_ASN_Crypt_EncodeBitString
#define asn1_DecodeBitStringContent VS_ASN_Crypt_DecodeBitStringContent
#define asn1_DecodeBitString        VS_ASN_Crypt_DecodeBitString
#define asn1_CreateTime             VS_ASN_Crypt_CreateTime
#define asn1_DisposeTime            VS_ASN_Crypt_DisposeTime
#define asn1_EncodeTime             VS_ASN_Crypt_EncodeTime
#define asn1_DecodeTime             VS_ASN_Crypt_DecodeTime
#define asn1_EncodeGeneralizedTime  VS_ASN_Crypt_EncodeGeneralizedTime
#define asn1_CheckGeneralizedTime   VS_ASN_Crypt_CheckGeneralizedTime
#define asn1_DecodeGeneralizedTime  VS_ASN_Crypt_DecodeGeneralizedTime
#define asn1_CreateGeneralizedTime  VS_ASN_Crypt_CreateGeneralizedTime
#define asn1_DisposeGeneralizedTime VS_ASN_Crypt_DisposeGeneralizedTime
#define asn1_EncodeUTCTime          VS_ASN_Crypt_EncodeUTCTime
#define asn1_CheckUTCTime           VS_ASN_Crypt_CheckUTCTime
#define asn1_DecodeUTCTime          VS_ASN_Crypt_DecodeUTCTime
#define asn1_CreateUTCTime          VS_ASN_Crypt_CreateUTCTime
#define asn1_DisposeUTCTime         VS_ASN_Crypt_DisposeUTCTime
#define asn1_EncodeObject           VS_ASN_Crypt_EncodeObject
#define asn1_DecodeObjectContent    VS_ASN_Crypt_DecodeObjectContent
#define asn1_DecodeObject           VS_ASN_Crypt_DecodeObject
#define asn1_CreateObject           VS_ASN_Crypt_CreateObject
#define asn1_DisposeObject          VS_ASN_Crypt_DisposeObject
#define asn1_EncodeInteger          VS_ASN_Crypt_EncodeInteger
#define asn1_EncodeIntegerContent   VS_ASN_Crypt_EncodeIntegerContent
#define asn1_DecodeInteger          VS_ASN_Crypt_DecodeInteger
#define asn1_DecodeIntegerContent   VS_ASN_Crypt_DecodeIntegerContent
#define asn1_CreateInteger          VS_ASN_Crypt_CreateInteger
#define asn1_DisposeInteger         VS_ASN_Crypt_DisposeInteger
#define asn1_SetInteger             VS_ASN_Crypt_SetInteger
#define asn1_GetInteger             VS_ASN_Crypt_GetInteger
#define asn1_EncodeBoolean          VS_ASN_Crypt_EncodeBoolean
#define asn1_DecodeBoolean          VS_ASN_Crypt_DecodeBoolean
#define SetOfObjectCmp              VS_ASN_Crypt_SetOfObjectCmp
#define asn1_EncodeSet              VS_ASN_Crypt_EncodeSet
#define asn1_DecodeSet              VS_ASN_Crypt_DecodeSet
#define asn1_Clone                  VS_ASN_Crypt_Clone
/************************************************************************/
/* RSA routines                                                        */
/************************************************************************/
#define ppUns64Mult                 VS_Crypt_ppUns64Mult
#define ppUns64Div                  VS_Crypt_ppUns64Div
#define LargeNumberDecode                   VS_Crypt_NaturalNumbers_Decode
#define LargeNumberEncode                   VS_Crypt_NaturalNumbers_Encode
#define LargeNumberAssign                   VS_Crypt_NaturalNumbers_Assign
#define LargeNumberAssignZero               VS_Crypt_NaturalNumbers_AssignZero
#define LargeNumberAssign2Exp               VS_Crypt_NaturalNumbers_Assign2Exp
#define LargeNumberAdd                      VS_Crypt_NaturalNumbers_Add
#define LargeNumberSub                      VS_Crypt_NaturalNumbers_Sub
#define LargeNumberMult                     VS_Crypt_NaturalNumbers_Mult
#define LargeNumberLShift                   VS_Crypt_NaturalNumbers_LShift
#define LargeNumberRShift                   VS_Crypt_NaturalNumbers_RShift
#define LargeNumberDiv                      VS_Crypt_NaturalNumbers_Div
#define LargeNumberMod                      VS_Crypt_NaturalNumbers_Mod
#define LargeNumberModMult                  VS_Crypt_NaturalNumbers_ModMult
#define LargeNumberModExp                   VS_Crypt_NaturalNumbers_ModExp
#define LargeNumberModInv                   VS_Crypt_NaturalNumbers_ModInv
#define LargeNumberGcd                      VS_Crypt_NaturalNumbers_Gcd
#define LargeNumberCmp                      VS_Crypt_NaturalNumbers_Cmp
#define LargeNumberZero                     VS_Crypt_NaturalNumbers_Zero
#define LargeNumberBits                     VS_Crypt_NaturalNumbers_Bits
#define ppUns64s                    VS_Crypt_NaturalNumbers_ppUns64s
#define LargeNumberAddDigitMult             VS_Crypt_NaturalNumbers_AddDigitMult
#define LargeNumberSubDigitMult             VS_Crypt_NaturalNumbers_SubDigitMult
#define ppUns64Bits                 VS_Crypt_RSA_ppUns64Bits
#define R_RandomInit                VS_Crypt_RSA_RandomInit
#define R_RandomUpdate              VS_Crypt_RSA_RandomUpdate
#define R_GetRandomBytesNeeded      VS_Crypt_RSA_GetRandomBytesNeeded
#define R_GenerateBytes             VS_Crypt_RSA_GenerateBytes
#define R_RandomFinal               VS_Crypt_RSA_RandomFinal
#define rsa_CreateKey               VS_Crypt_RSA_CreateKey
#define rsa_DisposeKey              VS_Crypt_RSA_DisposeKey
#define rsa_KeySize                 VS_Crypt_RSA_KeySize
#define rsa_EncodePrivateKey        VS_Crypt_RSA_EncodePrivateKey
#define rsa_DecodePrivateKey        VS_Crypt_RSA_DecodePrivateKey
#define rsa_UnShroudKey             VS_Crypt_RSA_UnShroudKey
#define rsa_DecodePublicKey         VS_Crypt_RSA_DecodePublicKey
#define rsaref_SetPublicKey         VS_Crypt_RSA_SetPublicKey
#define rsaref_SetPrivateKey        VS_Crypt_RSA_SetPrivateKey
#define rsa_PublicEncrypt           VS_Crypt_RSA_PublicEncrypt_
#define rsa_PrivateEncrypt          VS_Crypt_RSA_PrivateEncrypt_
#define rsa_PrivateDecrypt          VS_Crypt_RSA_PrivateDecrypt_
#define rsa_PublicDecrypt           VS_Crypt_RSA_PublicDecrypt_
#define rsa_Sign                    VS_Crypt_RSA_Sign
#define rsa_Verify                  VS_Crypt_RSA_Verify
#define RSAPublicBlock              VS_Crypt_RSA_PublicBlock
#define RSAPrivateBlock             VS_Crypt_RSA_PrivateBlock
#define RSAPublicEncrypt            VS_Crypt_RSA_PublicEncrypt
#define RSAPublicDecrypt            VS_Crypt_RSA_PublicDecrypt
#define RSAPrivateEncrypt           VS_Crypt_RSA_PrivateEncrypt
#define RSAPrivateDecrypt           VS_Crypt_RSA_PrivateDecrypt
#define RSAPublicBlock              VS_Crypt_RSA_PublicBlock
#define RSAPrivateBlock             VS_Crypt_RSA_PrivateBlock
#define MD5Init                     VS_Crypt_MD5_Init
#define MD5Update                   VS_Crypt_MD5_Update
#define MD5Final                    VS_Crypt_MD5_Final
#define MD5Transform                VS_Crypt_MD5_Transform
#define Encode1                     VS_Crypt_MD5_Encode
#define Decode1                     VS_Crypt_MD5_Decode
#define MD5_memcpy                  VS_Crypt_MD5_memcpy
#define MD5_memset                  VS_Crypt_MD5_memset
/************************************************************************/
/* x509 routines                                                       */
/************************************************************************/
#define x509_Encode                 VS_x509_Crypt_Encode
#define x509_Decode                 VS_x509_Crypt_Decode
#define x509_Create                 VS_x509_Crypt_Create
#define x509_Dispose                VS_x509_Crypt_Dispose
#define x509_EncodeCertInfo         VS_x509_Crypt_EncodeCertInfo
#define x509_DecodeCertInfo         VS_x509_Crypt_DecodeCertInfo
#define x509_EncodeAlgorithm        VS_x509_Crypt_EncodeAlgorithm
#define x509_DecodeAlgorithm        VS_x509_Crypt_DecodeAlgorithm
#define x509_CreateAlgorithm        VS_x509_Crypt_CreateAlgorithm
#define x509_DisposeAlgorithm       VS_x509_Crypt_DisposeAlgorithm
#define x509_EncodeNameEntry        VS_x509_Crypt_EncodeNameEntry
#define x509_EncodeNameEntries      VS_x509_Crypt_EncodeNameEntries
#define x509_EncodeName             VS_x509_Crypt_EncodeName
#define x509_DecodeNameEntry        VS_x509_Crypt_DecodeNameEntry
#define x509_DecodeName             VS_x509_Crypt_DecodeName
#define x509_CreateNameEntry        VS_x509_Crypt_CreateNameEntry
#define x509_CreateName             VS_x509_Crypt_CreateName
#define x509_DisposeNameEntry       VS_x509_Crypt_DisposeNameEntry
#define x509_DisposeName            VS_x509_Crypt_DisposeName
#define x509_EncodeValidity         VS_x509_Crypt_EncodeValidity
#define x509_DecodeValidity         VS_x509_Crypt_DecodeValidity
#define x509_CreateValidity         VS_x509_Crypt_CreateValidity
#define x509_DisposeValidity        VS_x509_Crypt_DisposeValidity
#define x509_EncodePublicKey        VS_x509_Crypt_EncodePublicKey
#define x509_DecodePublicKey        VS_x509_Crypt_DecodePublicKey
#define x509_CreatePublicKey        VS_x509_Crypt_CreatePublicKey
#define x509_DisposePublicKey       VS_x509_Crypt_DisposePublicKey
#define x509_EncodeExtension        VS_x509_Crypt_EncodeExtension
#define x509_DecodeExtension        VS_x509_Crypt_DecodeExtension
#define x509_CreateExtension        VS_x509_Crypt_CreateExtension
#define x509_DisposeExtension       VS_x509_Crypt_DisposeExtension
#define x509_EncodeAttribute        VS_x509_Crypt_EncodeAttribute
#define x509_DecodeAttribute        VS_x509_Crypt_DecodeAttribute
#define x509_CreateAttribute        VS_x509_Crypt_CreateAttribute
#define x509_DisposeAttribute       VS_x509_Crypt_DisposeAttribute
#define x509_GetPublicKey           VS_x509_Crypt_GetPublicKey
#define x509_GetSubjectCommonName   VS_x509_Crypt_GetSubjectCommonName
#define x509_EncodeRevoked          VS_x509_Crypt_EncodeRevoked
#define x509_DecodeRevoked          VS_x509_Crypt_DecodeRevoked
#define x509_CreateRevoked          VS_x509_Crypt_CreateRevoked
#define x509_DisposeRevoked         VS_x509_Crypt_DisposeRevoked
#define x509_EncodeCRLInfo          VS_x509_Crypt_EncodeCRLInfo
#define x509_DecodeCRLInfo          VS_x509_Crypt_DecodeCRLInfo
#define x509_CreateCRLInfo          VS_x509_Crypt_CreateCRLInfo
#define x509_DisposeCRLInfo         VS_x509_Crypt_DisposeCRLInfo
#define x509_EncodeCRL              VS_x509_Crypt_EncodeCRL
#define x509_DecodeCRL              VS_x509_Crypt_DecodeCRL
#define x509_CreateCRL              VS_x509_Crypt_CreateCRL
#define x509_DisposeCRL             VS_x509_Crypt_DisposeCRL
/************************************************************************/
/* PKCS7 routines                                                      */
/************************************************************************/
#define pkcs7_DataType              VS_PKCS7_Crypt_DataType
#define pkcs7_EncodeI               VS_PKCS7_Crypt_EncodeI
#define pkcs7_EncodeD               VS_PKCS7_Crypt_EncodeD
#define pkcs7_Decode                VS_PKCS7_Crypt_Decode
#define pkcs7_Create                VS_PKCS7_Crypt_Create
#define pkcs7_Dispose               VS_PKCS7_Crypt_Dispose
#define pkcs7_DisposeContent        VS_PKCS7_Crypt_DisposeContent
#define pkcs7_EncodeSignedData      VS_PKCS7_Crypt_EncodeSignedData
#define pkcs7_DecodeSignedData      VS_PKCS7_Crypt_DecodeSignedData
#define pkcs7_CreateSignedData      VS_PKCS7_Crypt_CreateSignedData
#define pkcs7_DisposeSignedData     VS_PKCS7_Crypt_DisposeSignedData
#define pkcs7_EncodeEncryptedContent VS_PKCS7_Crypt_EncodeEncryptedContent
#define pkcs7_DecodeEncryptedContent VS_PKCS7_Crypt_DecodeEncryptedContent
#define pkcs7_CreateEncryptedContent VS_PKCS7_Crypt_CreateEncryptedContent
#define pkcs7_DisposeEncryptedContent VS_PKCS7_Crypt_DisposeEncryptedContent
#define pkcs7_EncodeEncryptedData   VS_PKCS7_Crypt_EncodeEncryptedData
#define pkcs7_DecodeEncryptedData   VS_PKCS7_Crypt_DecodeEncryptedData
#define pkcs7_CreateEncryptedData   VS_PKCS7_Crypt_CreateEncryptedData
#define pkcs7_DisposeEncryptedData  VS_PKCS7_Crypt_DisposeEncryptedData
#define pkcs7_EncodeSignerInfo      VS_PKCS7_Crypt_EncodeSignerInfo
#define pkcs7_DecodeSignerInfo      VS_PKCS7_Crypt_DecodeSignerInfo
#define pkcs7_CreateSignerInfo      VS_PKCS7_Crypt_CreateSignerInfo
#define pkcs7_DisposeSignerInfo     VS_PKCS7_Crypt_DisposeSignerInfo
#define pkcs7_EncodeRecipientInfo   VS_PKCS7_Crypt_EncodeRecipientInfo
#define pkcs7_DecodeRecipientInfo   VS_PKCS7_Crypt_DecodeRecipientInfo
#define pkcs7_CreateRecipientInfo   VS_PKCS7_Crypt_CreateRecipientInfo
#define pkcs7_DisposeRecipientInfo  VS_PKCS7_Crypt_DisposeRecipientInfo
#define pkcs7_EncodeIssuerAndSerialNumber VS_PKCS7_Crypt_EncodeIssuerAndSerialNumber
#define pkcs7_DecodeIssuerAndSerialNumber VS_PKCS7_Crypt_DecodeIssuerAndSerialNumber
#define pkcs7_CreateIssuerAndSerialNumber VS_PKCS7_Crypt_CreateIssuerAndSerialNumber
#define pkcs7_DisposeIssuerAndSerialNumber VS_PKCS7_Crypt_DisposeIssuerAndSerialNumber
#define pkcs7_IASNFromCertInfo      VS_PKCS7_Crypt_IASNFromCertInfo
/************************************************************************/
/* PKCS12 routines                                                     */
/************************************************************************/
#define des_set_key_unchecked       VS_DES_Crypt_SET_KEY_unchecked
#define des_encrypt2                VS_DES_Crypt_encrypt2
#define des_encrypt3                VS_DES_Crypt_encrypt3
#define des_decrypt3                VS_DES_Crypt_decrypt3
#define des_ede3_cbc_encrypt        VS_DES_Crypt_EDE3_CBC_encrypt
#define des_ede3_init_key           VS_DES_Crypt_EDE3_INIT_KEY
#define des_ede3_cbc_cipher         VS_DES_Crypt_EDE3_CBC_CIPHER
#define RC2_set_key                 VS_RC2_Crypt_SET_KEY
#define RC2_encrypt                 VS_RC2_Crypt_encrypt
#define RC2_decrypt                 VS_RC2_Crypt_decrypt
#define RC2_cbc_encrypt             VS_RC2_Crypt_CBC_encrypt
#define rc2_init_key                VS_RC2_Crypt_INIT_KEY
#define rc2_cbc_cipher              VS_RC2_Crypt_CBC_CIPHER
#define cipher_PbeType              VS_CIPHER_Crypt_PbeType
#define cipher_Init                 VS_CIPHER_Crypt_Init
#define cipher_UpdateEncrypt        VS_CIPHER_Crypt_UpdateEncrypt
#define cipher_UpdateDecrypt        VS_CIPHER_Crypt_UpdateDecrypt
#define cipher_Update               VS_CIPHER_Crypt_Update
#define cipher_FinishEncrypt        VS_CIPHER_Crypt_FinishEncrypt
#define cipher_FinishDecrypt        VS_CIPHER_Crypt_FinishDecrypt
#define cipher_Finish               VS_CIPHER_Crypt_Finish
#define asc2uni                     VS_PKCS12_Crypt_asc2uni
#define pkcs12_PBE                  VS_PKCS12_Crypt_PBE
#define pkcs12_KeyGenUnicode        VS_PKCS12_Crypt_KeyGenUnicode
#define pkcs12_KeyGenAscii          VS_PKCS12_Crypt_KeyGenAscii
#define pkcs12_KeyIvGen             VS_PKCS12_Crypt_KeyIvGen
#define pbe_Init                    VS_PBE_Crypt_Init
#define pbe_DisposeParam            VS_PBE_Crypt_DisposeParam
#define pbe_CreateParam             VS_PBE_Crypt_CreateParam
#define pbe_DecodeParam             VS_PBE_Crypt_DecodeParam
#define sha1_Init                   VS_SHA_Crypt_Init
#define sha1_Finish                 VS_SHA_Crypt_Finish
#define sha1_Update                 VS_SHA_Crypt_Update
#define sha1_Block                  VS_SHA_Crypt_Block
#define sha1_Pad                    VS_SHA_Crypt_Pad
#define pkcs7_EncodeDigestInfo      VS_PKCS7_Crypt_EncodeDigestInfo
#define pkcs7_DecodeDigestInfo      VS_PKCS7_Crypt_DecodeDigestInfo
#define pkcs7_CreateDigestInfo      VS_PKCS7_Crypt_CreateDigestInfo
#define pkcs7_DisposeDigestInfo     VS_PKCS7_Crypt_DisposeDigestInfo
#define pkcs12_DecodeMacData        VS_PKCS12_Crypt_DecodeMacData
#define pkcs12_CreateMacData        VS_PKCS12_Crypt_CreateMacData
#define pkcs12_DisposeMacData       VS_PKCS12_Crypt_DisposeMacData
#define pkcs12_Decode               VS_PKCS12_Crypt_Decode
#define pkcs12_Create               VS_PKCS12_Crypt_Create
#define pkcs12_Dispose              VS_PKCS12_Crypt_Dispose
#define pkcs12_BagContentType       VS_PKCS12_Crypt_BagContentType
#define pkcs12_DecodeBag            VS_PKCS12_Crypt_DecodeBag
#define pkcs12_CreateBag            VS_PKCS12_Crypt_CreateBag
#define pkcs12_DisposeBag           VS_PKCS12_Crypt_DisposeBag
#define pkcs8_DecodePrivateKeyInfo  VS_PKCS8_Crypt_DecodePrivateKeyInfo
#define pkcs8_CreatePrivateKeyInfo  VS_PKCS8_Crypt_CreatePrivateKeyInfo
#define pkcs8_DisposePrivateKeyInfo VS_PKCS8_Crypt_DisposePrivateKeyInfo
#define pkcs12_BagType              VS_PKCS12_Crypt_BagType
#define pkcs12_DecodeSafeBag        VS_PKCS12_Crypt_DecodeSafeBag
#define pkcs12_CreateSafeBag        VS_PKCS12_Crypt_CreateSafeBag
#define pkcs12_DisposeSafeBag       VS_PKCS12_Crypt_DisposeSafeBag
#define pkcs12_DecryptAndDecode     VS_PKCS12_Crypt_DecryptAndDecode
#define hmac_Init                   VS_MAC_Crypt_Init
#define hmac_Update                 VS_MAC_Crypt_Update
#define hmac_Finish                 VS_MAC_Crypt_Finish
#define pkcs12_GenerateMac          VS_MAC_Crypt_GenerateMac
#define pkcs12_VerifyMac            VS_MAC_Crypt_VerifyMac
#define pkcs12_Parse                VS_MAC_Crypt_Parse
#define pkcs12_GetAttribute         VS_MAC_Crypt_GetAttribute
#define parse                       VS_Crypt_parse
#define parse_bags                  VS_Crypt_bags
#define parse_bag                   VS_Crypt_bag
/************************************************************************/
/*                                                                    */
/************************************************************************/
#endif /*end Release definitions */
/************************************************************************/
/*                                                                     */
/************************************************************************/
typedef void *(* TDecodeFunction)( PDFLibHandle, void *, ppUns8 **, ppInt64);
typedef ppInt32(* TEncodeFunction)( void *, ppUns8 ** );
typedef void(* TDisposeFunction)( PDFLibHandle, void * );

/************************************************************************/
/*                                                                    */
/************************************************************************/
#define digest_Length		8
#define digest_Count		4
extern ppUns8 digest_Begin[];
extern ppUns8 digest_Ends[];
extern ppInt32 digest_OIDs[];

#define algorithm_Length	5
#define algorithm_Count		11
extern ppUns8 algorithm_Begin[];
extern ppUns8 algorithm_Ends[];
extern ppInt32 algorithm_OIDs[];

#define pkcs1_Length		9
#define pkcs1_Count			5

#define pkcs5_Length		9
#define pkcs5_Count			9
extern ppUns8 pkcs5_Begin[];
extern ppUns8 pkcs5_Ends[];
extern ppInt32 pkcs5_OIDs[];

#define pkcs7_TypeLength	9
#define pkcs7_TypeCount		6
extern ppUns8 pkcs7_TypeBegin[];
extern ppUns8 pkcs7_TypeEnds[];
extern ppInt32 pkcs7_TypeOIDs[];

#define pkcs9_Length	9
#define pkcs9_Count		14
extern ppUns8 pkcs9_Begin[];
extern ppUns8 pkcs9_Ends[];
extern ppInt32 pkcs9_OIDs[];

#define pkcs12_CertTypeLength	10
#define pkcs12_CertTypeCount	2
extern ppUns8 pkcs12_CertTypeBegin[];
extern ppUns8 pkcs12_CertTypeEnds[];
extern ppInt32 pkcs12_CertTypeOIDs[];

#define pkcs12_CRLTypeLength	10
#define pkcs12_CRLTypeCount		1
extern ppUns8 pkcs12_CRLTypeBegin[];
extern ppUns8 pkcs12_CRLTypeEnds[];
extern ppInt32 pkcs12_CRLTypeOIDs[];

#define pkcs12_PbeLength	10
#define pkcs12_PbeCount		6
extern ppUns8 pkcs12_PbeBegin[];
extern ppUns8 pkcs12_PbeEnds[];
extern ppInt32 pkcs12_PbeOIDs[];

#define pkcs12_BagTypeLength	11
#define pkcs12_BagTypeCount		6
extern ppUns8 pkcs12_BagTypeBegin[];
extern ppUns8 pkcs12_BagTypeEnds[];
extern ppInt32 pkcs12_BagTypeOIDs[];

#define nameEntry_Length		3
#define nameEntry_Count			17
extern ppUns8 nameEntry_Begin[];
extern ppUns8 nameEntry_Ends[];
extern ppInt32 nameEntry_OIDs[];

#define	ID_undef								0

#define	ID_md2									1
#define	ID_md4									2
#define	ID_md5									3
#define	ID_sha									4
#define	ID_sha1									5
#define	ID_hmacWithSHA1							7

#define	ID_rsaEncryption						11
#define ID_md2WithRSAEncryption					12
#define	ID_md4WithRSAEncryption					13
#define ID_md5WithRSAEncryption					14
#define	ID_shaWithRSAEncryption					15
#define	ID_sha1WithRSAEncryption				16

#define	ID_des_cbc								21
#define	ID_des_ecb								22
#define	ID_des_ede								23
#define	ID_des_ofb64							24
#define	ID_des_cfb64							25

#define	ID_rsaSignature							31

#define	ID_pbeWithMD2AndDES_CBC					41
#define	ID_pbeWithMD2AndRC2_CBC					42
#define	ID_pbeWithMD5AndDES_CBC					43
#define	ID_pbeWithMD5AndRC2_CBC					44
#define	ID_pbeWithSHA1AndDES_CBC				45
#define	ID_pbeWithSHA1AndRC2_CBC				46
#define	ID_pbkdf2								47
#define	ID_pbes2								48
#define	ID_pbmac1								49

#define	ID_pkcs7Data							61
#define	ID_pkcs7SignedData						62
#define	ID_pkcs7EnvelopedData					63
#define	ID_pkcs7SignedAndEnvelopedData			64
#define	ID_pkcs7DigestedData					65
#define	ID_pkcs7EncryptedData					66

#define	ID_pkcs9EmailAddress					71
#define	ID_pkcs9UnstructuredName				72
#define	ID_pkcs9ContentType						73
#define	ID_pkcs9MessageDigest					74
#define	ID_pkcs9SigningTime						75
#define	ID_pkcs9Countersignature				76
#define	ID_pkcs9ChallengePassword				77
#define	ID_pkcs9UnstructuredAddress				78
#define	ID_pkcs9ExtCertAttributes				79
#define	ID_pkcs9SigningDescription				80
#define	ID_pkcs9ExtensionRequest				81
#define	ID_pkcs9SMIMECapabilities				82
#define	ID_pkcs9FriendlyName					83
#define	ID_pkcs9LocalKeyID						84

#define	ID_pkcs12CertTypeX509					91
#define	ID_pkcs12CertTypeSDSI					92

#define	ID_pkcs12CRLTypeX509					96

#define	ID_pbeWithSHA1And128BitRC4				101
#define	ID_pbeWithSHA1And40BitRC4				102
#define	ID_pbeWithSHA1And3_Key_TripleDES_CBC	103
#define	ID_pbeWithSHA1And2_Key_TripleDES_CBC	104
#define	ID_pbeWithSHA1And128BitRC2_CBC			105
#define	ID_pbeWithSHA1And40BitRC2_CBC			106

#define	ID_keyBag								111
#define	ID_pkcs8ShroudedKeyBag					112
#define	ID_certBag								113
#define	ID_crlBag								114
#define	ID_secretBag							115
#define	ID_safeContentsBag						116

#define ID_commonName							121
#define ID_surname								122
#define ID_serialNumber							123
#define ID_countryName							124
#define ID_localityName							125
#define ID_stateOrProvinceName					126
#define ID_organizationName						127
#define ID_organizationalUnitName				128
#define ID_title								129
#define ID_description							130
#define ID_crl_distribution_points				131
#define ID_name									132
#define ID_givenName							133
#define ID_initials								134
#define ID_uniqueIdentifier						135
#define ID_dnQualifier							136
#define ID_role									137
/************************************************************************/
/*                                                                    */
/************************************************************************/
#define MIN_ITEMS 4

typedef struct
{
    PDFLibHandle lib;
    ppInt32		max;
    ppInt32		num;
    char **	data;
} VECTOR;

typedef struct 
{
    PDFLibHandle lib;
    ppInt32		max;
    ppInt32		num;
    ppUns8 *	data;
} BUFFER;


typedef struct
{
	ppUns8 *	p;		
	ppInt32		eos;
	ppInt32		inf;
	ppInt32		tag;
	ppInt32		cls;
	ppInt64	len;
	ppUns8 *	max;
	ppUns8 *	q;
	ppUns8 **	pp;
} asn1_Context;

typedef struct
{
	ppUns8 *	data;
	ppInt32		length;
	ppInt32		flags;
} asn1_Object;

#define UNIVERSAL_CLS		0x00
#define APPLICATION_CLS		0x40
#define CTX_SPECIFIC_CLS	0x80
#define PRIVATE_CLS			0xC0

#define CONSTRUCTED			0x20
#define PRIMITIVE			0x1f

#define IS_SEQUENCE			0
#define IS_SET				1

#define BOOLEAN_TAG					1
#define INTEGER_TAG					2
#define BITSTRING_TAG				3
#define OCTETSTRING_TAG				4
#define NULL_TAG					5
#define OBJECT_TAG					6
#define UTF8STRING_TAG				12
#define SEQUENCE_TAG				16
#define SET_TAG						17
#define IA5STRING_TAG				22
#define UTCTIME_TAG					23
#define GENERALIZEDTIME_TAG			24
#define BMPSTRING_TAG				30

/*asn1_DecodeTypedBytes() */
#define NUMERICSTRING_BIT		0x0001
#define PRINTABLESTRING_BIT		0x0002
#define T61STRING_BIT			0x0004
#define TELETEXSTRING_BIT		0x0004
#define VIDEOTEXSTRING_BIT		0x0008
#define IA5STRING_BIT			0x0010
#define GRAPHICSTRING_BIT		0x0020
#define ISO64STRING_BIT			0x0040
#define VISIBLESTRING_BIT		0x0040
#define GENERALSTRING_BIT		0x0080
#define UNIVERSALSTRING_BIT		0x0100
#define OCTETSTRING_BIT			0x0200
#define BITSTRING_BIT			0x0400
#define BMPSTRING_BIT			0x0800
#define UNKNOWN_BIT				0x1000
#define UTF8STRING_BIT			0x2000

#define BITS_LEFT_FLAG			0x08 

typedef struct
{
	ppInt32		type;
	ppUns8 *	data;
	ppInt32		length;
	ppInt64	bits_left;
} asn1_String;

#define asn1_Integer asn1_String
#define asn1_BitString asn1_String
#define asn1_OctetString asn1_String
#define asn1_UTCTime asn1_String
#define asn1_GeneralizedTime asn1_String
#define asn1_Time asn1_String
#define asn1_UTF8String asn1_String
#define asn1_IA5String asn1_String
#define asn1_BMPString asn1_String
#define asn1_Boolean ppInt32

typedef struct
{
	ppInt32 type;
	union
	{
		char *ptr;
		asn1_Boolean boolean;
		asn1_String *string;
		asn1_Object *object;
		asn1_Integer *integer;
		asn1_BitString *bit_string;
		asn1_OctetString *octet_string;
		asn1_IA5String *ia5string;
		asn1_BMPString *bmpstring;
		asn1_UTCTime *utc_time;
		asn1_GeneralizedTime *generalized_time;
		asn1_UTF8String *utf8_string;
		asn1_String *set;
		asn1_String *sequence;
	} value;
} asn1_Type;
typedef  void (*DisposeFunc )(PDFLibHandle, void * );

#ifdef  __cplusplus
extern "C" {
#endif

    VECTOR *vector_Create(PDFLibHandle);
    void    vector_Free(VECTOR* );
    void    vector_Dispose(VECTOR*, DisposeFunc);

    ppInt32		vector_GetCount(const VECTOR* );
    char *	vector_GetValueAt(const VECTOR*, ppInt32);
    char *	vector_SetValueAt(VECTOR*, ppInt32, char * );

    ppInt32		vector_Insert(VECTOR*, ppInt32, char * );
    char *	vector_Delete(VECTOR*, ppInt32);

    ppInt32		vector_Push(VECTOR*, char * );
    char *	vector_Pop(VECTOR* );

    BUFFER *buffer_Create(PDFLibHandle);
    void buffer_Dispose(BUFFER* );
    ppInt32 buffer_Enlarge(BUFFER*, ppInt32);


    void *asn1_Clone(PDFLibHandle, TEncodeFunction, TDecodeFunction, void * );

    asn1_Type *asn1_CreateType(PDFLibHandle);
	void asn1_DisposeType(PDFLibHandle Lib, asn1_Type *ptype);
	void asn1_DisposeTypeComponent(PDFLibHandle Lib, asn1_Type *ptype);
    ppInt32 asn1_EncodeType(asn1_Type*, ppUns8 ** );
    asn1_Type *asn1_DecodeType(PDFLibHandle, asn1_Type**, ppUns8 **, ppInt64);
    void asn1_SetType(PDFLibHandle Lib, asn1_Type *type, ppInt32 t, void *v);

    asn1_Object *asn1_CreateObject(PDFLibHandle);
    asn1_Object *asn1_CreateObjectByID(PDFLibHandle, ppInt32);
	void asn1_DisposeObject(PDFLibHandle lib, asn1_Object *obj);
    ppInt32 asn1_EncodeObject(asn1_Object*, ppUns8 ** );
    asn1_Object *asn1_DecodeObject(PDFLibHandle, asn1_Object**, ppUns8 **, ppInt64);
    ppInt32 asn1_GetObjectID(asn1_Object* );

    asn1_String *asn1_CreateString(PDFLibHandle);
    asn1_String *asn1_CreateTypedString(PDFLibHandle, ppInt32);
	void asn1_DisposeString(PDFLibHandle lib, asn1_String *str);
    void asn1_SetString(PDFLibHandle, asn1_String*, const void *, ppInt32);
    asn1_String *asn1_CloneString(PDFLibHandle, asn1_String* );
    ppInt32 asn1_CompareStrings(asn1_String *a, asn1_String *b);

    asn1_BitString *asn1_CreateBitString(PDFLibHandle);
    void asn1_DisposeBitString(PDFLibHandle lib, asn1_BitString *str);
    ppInt32 asn1_EncodeBitString(asn1_BitString*, ppUns8 ** );
    asn1_BitString *asn1_DecodeBitString(PDFLibHandle, asn1_BitString**, ppUns8 **, ppInt64);

    asn1_OctetString *asn1_CreateOctetString(PDFLibHandle);
    void asn1_DisposeOctetString(PDFLibHandle lib, asn1_OctetString *str);
    ppInt32 asn1_EncodeOctetString(asn1_OctetString*, ppUns8 ** );
    asn1_OctetString *asn1_DecodeOctetString(PDFLibHandle, asn1_OctetString**, ppUns8 **, ppInt64);

    asn1_IA5String *asn1_CreateIA5String(PDFLibHandle);
	void asn1_DisposeIA5String(PDFLibHandle lib, asn1_IA5String *str);
    ppInt32 asn1_EncodeIA5String(asn1_IA5String*, ppUns8 ** );
    asn1_IA5String *asn1_DecodeIA5String(PDFLibHandle, asn1_IA5String**, ppUns8 **, ppInt64);

    asn1_BMPString *asn1_CreateBMPString(PDFLibHandle);
	void asn1_DisposeBMPString(PDFLibHandle lib, asn1_BMPString *str);
    ppInt32 asn1_EncodeBMPString(asn1_BMPString*, ppUns8 ** );
    asn1_BMPString *asn1_DecodeBMPString(PDFLibHandle, asn1_BMPString**, ppUns8 **, ppInt64);

    ppInt32 asn1_EncodeBoolean(ppInt32, ppUns8 ** );
    ppInt32 asn1_DecodeBoolean(PDFLibHandle, ppInt32 *, ppUns8 **, ppInt64);

    asn1_Integer *asn1_CreateInteger(PDFLibHandle);
	void asn1_DisposeInteger(PDFLibHandle lib, asn1_Integer *arg);
    ppInt32 asn1_EncodeInteger(asn1_Integer*, ppUns8 ** );
    ppInt32 asn1_EncodeIntegerContent(asn1_Integer*, ppUns8 ** );
    asn1_Integer *asn1_DecodeInteger(PDFLibHandle, asn1_Integer**, ppUns8 **, ppInt64);
    asn1_Integer *asn1_DecodeIntegerContent(PDFLibHandle, asn1_Integer**, ppUns8 **, ppInt64);
    ppInt32 asn1_SetInteger(PDFLibHandle, asn1_Integer*, ppInt64);
    ppInt64 asn1_GetInteger(asn1_Integer* );

    typedef ppInt32 (*asn1_EncodeIntegerFunc)(asn1_Integer*, ppUns8 ** );
    typedef asn1_Integer *(*asn1_DecodeIntegerFunc)(PDFLibHandle, asn1_Integer**, ppUns8 **, ppInt64);
    typedef asn1_Integer *(*asn1_CloneIntegerFunc)(PDFLibHandle, asn1_EncodeIntegerFunc, asn1_DecodeIntegerFunc, asn1_Integer* );

    ppInt32 asn1_EncodeBytes(asn1_String*, ppUns8 **, ppInt32, ppInt32);
    asn1_String *asn1_DecodeBytes(PDFLibHandle, asn1_String**, ppUns8 **, ppInt64, ppInt32, ppInt32);
    asn1_String *asn1_DecodeTypedBytes(PDFLibHandle, asn1_String**, ppUns8 **, ppInt64, ppInt32);

    ppInt32 asn1_EncodePrintable(asn1_String*, ppUns8 ** );
    asn1_String *asn1_DecodePrintable(PDFLibHandle, asn1_String**, ppUns8 **, ppInt64);

    asn1_UTCTime *asn1_CreateUTCTime(PDFLibHandle);
	void asn1_DisposeUTCTime( PDFLibHandle lib, asn1_UTCTime *tm);
    ppInt32 asn1_EncodeUTCTime(asn1_UTCTime*, ppUns8 ** );
    asn1_UTCTime *asn1_DecodeUTCTime(PDFLibHandle, asn1_UTCTime**, ppUns8 **, ppInt64);

    asn1_GeneralizedTime *asn1_CreateGeneralizedTime(PDFLibHandle);
	void asn1_DisposeGeneralizedTime( PDFLibHandle lib, asn1_GeneralizedTime *tm );
    ppInt32 asn1_EncodeGeneralizedTime(asn1_GeneralizedTime*, ppUns8 ** );
    asn1_GeneralizedTime *asn1_DecodeGeneralizedTime(PDFLibHandle, asn1_GeneralizedTime**, ppUns8 **, ppInt64);

    asn1_Time *asn1_CreateTime(PDFLibHandle);
	void asn1_DisposeTime(PDFLibHandle lib, asn1_Time *tm);
    ppInt32 asn1_EncodeTime(asn1_Time*, ppUns8 ** );
    asn1_Time *asn1_DecodeTime(PDFLibHandle, asn1_Time**, ppUns8 **, ppInt64);

    void asn1_GetSequence(PDFLibHandle, asn1_Context*, ppInt64 * );
    ppInt32  asn1_CheckEndSequence(asn1_Context* );
    ppInt32  asn1_CheckInfiniteEnd(ppUns8 **, ppInt64);
    void asn1_Finish(PDFLibHandle, asn1_Context* );

    ppInt32 asn1_GetObject(ppUns8 **, ppInt64 *, ppInt32 *, ppInt32 *, ppInt64);
    void asn1_PutObject(ppUns8 **, ppInt32, ppInt32, ppInt32, ppInt32);
    ppInt32 asn1_ObjectSize(ppInt32, ppInt32, ppInt32);

    ppInt32 asn1_EncodeSet(VECTOR*, ppUns8 **, TEncodeFunction, ppInt32, ppInt32, ppInt32);
    VECTOR *asn1_DecodeSet(PDFLibHandle, VECTOR**, ppUns8 **, ppInt64, TDecodeFunction, TDisposeFunction, ppInt32, ppInt32);

    VECTOR *asn1_UnpackSequence(PDFLibHandle, ppUns8 *, ppInt32, TDecodeFunction, TDisposeFunction );
    ppUns8 *asn1_PackSequence(VECTOR*, TEncodeFunction, ppUns8 **, ppInt32 * );

    void *asn1_UnpackString(PDFLibHandle, asn1_String*, TDecodeFunction );
    asn1_String *asn1_PackString(void *, TDecodeFunction, asn1_String** );

#ifdef  __cplusplus
}
#endif

#endif

#endif  /*#endif VSASNIH */
