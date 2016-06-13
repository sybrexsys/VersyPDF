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
VSkRSAI.h
*********************************************************************/

#ifndef VSRSAIH
#define VSRSAIH

#include "../VSExcept.h"
#include "VSLibI.h"

#include "VSkASNI.h"




#ifndef NOT_USE_SIGN


#define ppUns64_BITS 32
#define ppUns32_BITS 16
#define ppUns64_LEN (ppUns64_BITS / 8)
#define MAX_ppUns64S \
  ((MAX_RSA_MODULUS_LEN + ppUns64_LEN - 1) / ppUns64_LEN + 1)
#define MAX_ppUns64 0xffffffff
#define MAX_ppUns32 0xffff

#define LOW_HALF(x) ((x) & MAX_ppUns32)
#define HIGH_HALF(x) (((x) >> ppUns32_BITS) & MAX_ppUns32)
#define TO_HIGH_HALF(x) (((ppUns32)(x)) << ppUns32_BITS)
#define DIGIT_2MSB(x) (ppUns32)(((x) >> (ppUns64_BITS - 2)) & 3)

typedef struct
{
	asn1_Integer *version;
	asn1_Integer *modulus;
	asn1_Integer *publExp;
	asn1_Integer *exponent;
	asn1_Integer *prime[2];
	asn1_Integer *prExp[2];
	asn1_Integer *coeff;
} rsa_Key;


/*RSA key lengths.*/
#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 2048
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)


typedef struct {
    ppUns32 bits;                           
    ppUns8 modulus[MAX_RSA_MODULUS_LEN];                    
    ppUns8 publicExponent[MAX_RSA_MODULUS_LEN];   
    ppUns8 exponent[MAX_RSA_MODULUS_LEN];         
    ppUns8 prime[2][MAX_RSA_PRIME_LEN];           
    ppUns8 primeExponent[2][MAX_RSA_PRIME_LEN];   
    ppUns8 coefficient[MAX_RSA_PRIME_LEN];        
} RSA_PRIVATE_KEY;


#ifdef  __cplusplus
extern "C" {
#endif

    void ppUns64Mult (ppUns32 [2], ppUns32, ppUns32);
    void ppUns64Div (ppUns32 *, ppUns32 [2], ppUns32);

    static ppUns32 LargeNumberAddDigitMult (ppUns32 *, ppUns32 *, ppUns32, ppUns32 *, ppUns32);
    static ppUns32 LargeNumberSubDigitMult (ppUns32 *, ppUns32 *, ppUns32, ppUns32 *, ppUns32);

    static ppUns32 ppUns64Bits (ppUns32);

    void LargeNumberDecode (ppUns32 *, ppUns32, ppUns8 *, ppUns32);
    void LargeNumberEncode (ppUns8 *, ppUns32, ppUns32 *, ppUns32);

    void LargeNumberAssign (ppUns32 *, ppUns32 *, ppUns32);
    void LargeNumberAssignZero (ppUns32 *, ppUns32);
    void LargeNumberAssign2Exp (ppUns32 *, ppUns32, ppUns32);

    ppUns32 LargeNumberAdd (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);
    ppUns32 LargeNumberSub (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);
    void LargeNumberMult (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);
    void LargeNumberDiv (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32, ppUns32 *, ppUns32);
    ppUns32 LargeNumberLShift (ppUns32 *, ppUns32 *, ppUns32, ppUns32);
    ppUns32 LargeNumberRShift (ppUns32 *, ppUns32 *, ppUns32, ppUns32);

    void LargeNumberMod (ppUns32 *, ppUns32 *, ppUns32, ppUns32 *, ppUns32);
    void LargeNumberModMult (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);
    void LargeNumberModExp (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32, ppUns32 *, ppUns32);
    void LargeNumberModInv (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);
    void LargeNumberGcd (ppUns32 *, ppUns32 *, ppUns32 *, ppUns32);

    ppInt32 LargeNumberCmp (ppUns32 *, ppUns32 *, ppUns32);
    ppInt32 LargeNumberZero (ppUns32 *, ppUns32);
    ppUns32 LargeNumberBits (ppUns32 *, ppUns32);
    ppUns32 ppUns64s (ppUns32 *, ppUns32);


    rsa_Key *rsa_CreateKey(PDFLibHandle);
	void rsa_DisposeKey(PDFLibHandle lib, rsa_Key *key);
    ppInt32	rsa_KeySize(rsa_Key * );

    ppInt32 rsa_EncodePrivateKey(rsa_Key *, ppUns8 ** );
    rsa_Key *rsa_DecodePrivateKey(PDFLibHandle, rsa_Key **, ppUns8 **, ppInt64);
    rsa_Key *rsa_DecodePublicKey(PDFLibHandle, rsa_Key **, ppUns8 **, ppInt64);


    void rsa_PublicEncrypt(PDFLibHandle, ppUns8 *, ppInt32 *, ppUns8 *, ppInt32, rsa_Key *, ppInt32);
    void rsa_PublicDecrypt(PDFLibHandle, ppUns8 *, ppInt32 *, ppUns8 *, ppInt32, rsa_Key *, ppInt32);
    void rsa_PrivateEncrypt(PDFLibHandle, ppUns8 *, ppInt32 *, ppUns8 *, ppInt32, rsa_Key *, ppInt32);
    
    void rsa_Sign(PDFLibHandle, ppInt32, ppUns8 *, ppInt32, ppUns8 *, ppInt32 *, rsa_Key * );
    ppInt32 RSAPrivateEncrypt (ppUns8 *, ppUns32 *, ppUns8 *, ppUns32,  RSA_PRIVATE_KEY * );
    



#ifdef  __cplusplus
}
#endif

#endif

#endif /*endif VSRSAIH */
