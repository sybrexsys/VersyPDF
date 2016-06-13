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
VSkPKCS12I.h
*********************************************************************/

#ifndef VSPKCS12IH
#define VSPKCS12IH

#include "VSkPKCS7I.h"

#ifndef NOT_USE_SIGN
typedef ppUns32 DES_LONG;
typedef ppUns8 des_cblock[8];
typedef ppUns8 const_des_cblock[8];

typedef struct des_ks_struct
{
	union
	{
		des_cblock cblock;
		DES_LONG deslong[2];
	} ks;
	ppInt32 weak_key;
} des_key_schedule[16];

#define DES_KEY_SZ 	(sizeof(des_cblock))
#define DES_SCHEDULE_SZ (sizeof(des_key_schedule))

#define DES_ENCRYPT	1
#define DES_DECRYPT	0

#define DES_CBC_MODE	0
#define DES_PCBC_MODE	1

#define ITERATIONS 16

#define c2l(c,l) \
	(l =((DES_LONG)(*((c)++))), \
	 l|=((DES_LONG)(*((c)++)))<< 8L, \
	 l|=((DES_LONG)(*((c)++)))<<16L, \
	 l|=((DES_LONG)(*((c)++)))<<24L)

/*NOTE - c is not incremented as per c2l */
#define c2ln(c,l1,l2,n)	\
{ \
	c+=n; \
	l1=l2=0; \
	switch (n) \
	{ \
		case 8: l2 =((DES_LONG)(*(--(c))))<<24L; \
		case 7: l2|=((DES_LONG)(*(--(c))))<<16L; \
		case 6: l2|=((DES_LONG)(*(--(c))))<< 8L; \
		case 5: l2|=((DES_LONG)(*(--(c))));     \
		case 4: l1 =((DES_LONG)(*(--(c))))<<24L; \
		case 3: l1|=((DES_LONG)(*(--(c))))<<16L; \
		case 2: l1|=((DES_LONG)(*(--(c))))<< 8L; \
		case 1: l1|=((DES_LONG)(*(--(c))));     \
	} \
}

#define l2c(l,c) \
	(*((c)++)=(ppUns8)(((l)     )&0xff), \
	 *((c)++)=(ppUns8)(((l)>> 8L)&0xff), \
	 *((c)++)=(ppUns8)(((l)>>16L)&0xff), \
	 *((c)++)=(ppUns8)(((l)>>24L)&0xff))

/*NOTE - c is not incremented as per l2c */
#define l2cn(l1,l2,c,n)	\
{ \
	c+=n; \
	switch (n) \
	{ \
		case 8: *(--(c))=(ppUns8)(((l2)>>24L)&0xff); \
		case 7: *(--(c))=(ppUns8)(((l2)>>16L)&0xff); \
		case 6: *(--(c))=(ppUns8)(((l2)>> 8L)&0xff); \
		case 5: *(--(c))=(ppUns8)(((l2)     )&0xff); \
		case 4: *(--(c))=(ppUns8)(((l1)>>24L)&0xff); \
		case 3: *(--(c))=(ppUns8)(((l1)>>16L)&0xff); \
		case 2: *(--(c))=(ppUns8)(((l1)>> 8L)&0xff); \
		case 1: *(--(c))=(ppUns8)(((l1)     )&0xff); \
	} \
}

#define	ROTATE(a,n)	(((a)>>(n))+((a)<<(32-(n))))

#ifdef DES_FCRYPT

#define LOAD_DATA_tmp(R,S,u,t,E0,E1) \
	{ DES_LONG tmp; LOAD_DATA(R,S,u,t,E0,E1,tmp); }

#define LOAD_DATA(R,S,u,t,E0,E1,tmp) \
	t=R^(R>>16L); \
	u=t&E0; t&=E1; \
	tmp=(u<<16); u^=R^s[S  ]; u^=tmp; \
	tmp=(t<<16); t^=R^s[S+1]; t^=tmp
#else
#define LOAD_DATA_tmp(a,b,c,d,e,f) LOAD_DATA(a,b,c,d,e,f,g)
#define LOAD_DATA(R,S,u,t,E0,E1,tmp) \
	u=R^s[S  ]; \
	t=R^s[S+1]
#endif

/*The changes to this macro may help or hinder, depending on the
 * compiler and the architecture.  gcc2 always seems to do well :-).
 * DO NOT use the alternative version on machines with 8 byte longs.
 * It does not seem to work on the Alpha, even when DES_LONG is 4
 * bytes, probably an issue of accessing non-word aligned objects :-( */
#ifdef DES_PTR

/*It recently occurred to me that 0^0^0^0^0^0^0 == 0, so there
 * is no reason to not xor all the sub items together.  This potentially
 * saves a register since things can be xored directly into L */

#if defined(DES_RISC1) || defined(DES_RISC2)
#ifdef DES_RISC1
#define D_ENCRYPT(LL,R,S) { \
	ppUns32 u1,u2,u3; \
	LOAD_DATA(R,S,u,t,E0,E1,u1); \
	u2=(ppInt32)u>>8L; \
	u1=(ppInt32)u&0xfc; \
	u2&=0xfc; \
	t=ROTATE(t,4); \
	u>>=16L; \
	LL^= *(const DES_LONG * )(des_SP      +u1); \
	LL^= *(const DES_LONG * )(des_SP+0x200+u2); \
	u3=(ppInt32)(u>>8L); \
	u1=(ppInt32)u&0xfc; \
	u3&=0xfc; \
	LL^= *(const DES_LONG * )(des_SP+0x400+u1); \
	LL^= *(const DES_LONG * )(des_SP+0x600+u3); \
	u2=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0xfc; \
	u2&=0xfc; \
	t>>=16L; \
	LL^= *(const DES_LONG * )(des_SP+0x100+u1); \
	LL^= *(const DES_LONG * )(des_SP+0x300+u2); \
	u3=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0xfc; \
	u3&=0xfc; \
	LL^= *(const DES_LONG * )(des_SP+0x500+u1); \
	LL^= *(const DES_LONG * )(des_SP+0x700+u3); }
#endif
#ifdef DES_RISC2
#define D_ENCRYPT(LL,R,S) { \
	ppUns32 u1,u2,s1,s2; \
	LOAD_DATA(R,S,u,t,E0,E1,u1); \
	u2=(ppInt32)u>>8L; \
	u1=(ppInt32)u&0xfc; \
	u2&=0xfc; \
	t=ROTATE(t,4); \
	LL^= *(const DES_LONG * )(des_SP      +u1); \
	LL^= *(const DES_LONG * )(des_SP+0x200+u2); \
	s1=(ppInt32)(u>>16L); \
	s2=(ppInt32)(u>>24L); \
	s1&=0xfc; \
	s2&=0xfc; \
	LL^= *(const DES_LONG * )(des_SP+0x400+s1); \
	LL^= *(const DES_LONG * )(des_SP+0x600+s2); \
	u2=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0xfc; \
	u2&=0xfc; \
	LL^= *(const DES_LONG * )(des_SP+0x100+u1); \
	LL^= *(const DES_LONG * )(des_SP+0x300+u2); \
	s1=(ppInt32)(t>>16L); \
	s2=(ppInt32)(t>>24L); \
	s1&=0xfc; \
	s2&=0xfc; \
	LL^= *(const DES_LONG * )(des_SP+0x500+s1); \
	LL^= *(const DES_LONG * )(des_SP+0x700+s2); }
#endif
#else
#define D_ENCRYPT(LL,R,S) { \
	LOAD_DATA_tmp(R,S,u,t,E0,E1); \
	t=ROTATE(t,4); \
	LL^= \
	*(const DES_LONG * )(des_SP      +((u     )&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x200+((u>> 8L)&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x400+((u>>16L)&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x600+((u>>24L)&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x100+((t     )&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x300+((t>> 8L)&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x500+((t>>16L)&0xfc))^ \
	*(const DES_LONG * )(des_SP+0x700+((t>>24L)&0xfc)); }
#endif

#else /*original version */

#if defined(DES_RISC1) || defined(DES_RISC2)
#ifdef DES_RISC1
#define D_ENCRYPT(LL,R,S) {\
	ppUns32 u1,u2,u3; \
	LOAD_DATA(R,S,u,t,E0,E1,u1); \
	u>>=2L; \
	t=ROTATE(t,6); \
	u2=(ppInt32)u>>8L; \
	u1=(ppInt32)u&0x3f; \
	u2&=0x3f; \
	u>>=16L; \
	LL^=des_SPtrans[0][u1]; \
	LL^=des_SPtrans[2][u2]; \
	u3=(ppInt32)u>>8L; \
	u1=(ppInt32)u&0x3f; \
	u3&=0x3f; \
	LL^=des_SPtrans[4][u1]; \
	LL^=des_SPtrans[6][u3]; \
	u2=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0x3f; \
	u2&=0x3f; \
	t>>=16L; \
	LL^=des_SPtrans[1][u1]; \
	LL^=des_SPtrans[3][u2]; \
	u3=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0x3f; \
	u3&=0x3f; \
	LL^=des_SPtrans[5][u1]; \
	LL^=des_SPtrans[7][u3]; }
#endif
#ifdef DES_RISC2
#define D_ENCRYPT(LL,R,S) {\
	ppUns32 u1,u2,s1,s2; \
	LOAD_DATA(R,S,u,t,E0,E1,u1); \
	u>>=2L; \
	t=ROTATE(t,6); \
	u2=(ppInt32)u>>8L; \
	u1=(ppInt32)u&0x3f; \
	u2&=0x3f; \
	LL^=des_SPtrans[0][u1]; \
	LL^=des_SPtrans[2][u2]; \
	s1=(ppInt32)u>>16L; \
	s2=(ppInt32)u>>24L; \
	s1&=0x3f; \
	s2&=0x3f; \
	LL^=des_SPtrans[4][s1]; \
	LL^=des_SPtrans[6][s2]; \
	u2=(ppInt32)t>>8L; \
	u1=(ppInt32)t&0x3f; \
	u2&=0x3f; \
	LL^=des_SPtrans[1][u1]; \
	LL^=des_SPtrans[3][u2]; \
	s1=(ppInt32)t>>16; \
	s2=(ppInt32)t>>24L; \
	s1&=0x3f; \
	s2&=0x3f; \
	LL^=des_SPtrans[5][s1]; \
	LL^=des_SPtrans[7][s2]; }
#endif

#else

#define D_ENCRYPT(LL,R,S) {\
	LOAD_DATA_tmp(R,S,u,t,E0,E1); \
	t=ROTATE(t,4); \
	LL^=\
		des_SPtrans[0][(u>> 2L)&0x3f]^ \
		des_SPtrans[2][(u>>10L)&0x3f]^ \
		des_SPtrans[4][(u>>18L)&0x3f]^ \
		des_SPtrans[6][(u>>26L)&0x3f]^ \
		des_SPtrans[1][(t>> 2L)&0x3f]^ \
		des_SPtrans[3][(t>>10L)&0x3f]^ \
		des_SPtrans[5][(t>>18L)&0x3f]^ \
		des_SPtrans[7][(t>>26L)&0x3f]; }
#endif
#endif

	/*IP and FP
	 * The problem is more of a geometric problem that random bit fiddling.
	 0  1  2  3  4  5  6  7      62 54 46 38 30 22 14  6
	 8  9 10 11 12 13 14 15      60 52 44 36 28 20 12  4
	16 17 18 19 20 21 22 23      58 50 42 34 26 18 10  2
	24 25 26 27 28 29 30 31  to  56 48 40 32 24 16  8  0

	32 33 34 35 36 37 38 39      63 55 47 39 31 23 15  7
	40 41 42 43 44 45 46 47      61 53 45 37 29 21 13  5
	48 49 50 51 52 53 54 55      59 51 43 35 27 19 11  3
	56 57 58 59 60 61 62 63      57 49 41 33 25 17  9  1

	The output has been subject to swaps of the form
	0 1 -> 3 1 but the odd and even bits have been put into
	2 3    2 0
	different words.  The main trick is to remember that
	t=((l>>size)^r)&(mask);
	r^=t;
	l^=(t<<size);
	can be used to swap and move bits between words.

	So l =  0  1  2  3  r = 16 17 18 19
	        4  5  6  7      20 21 22 23
	        8  9 10 11      24 25 26 27
	       12 13 14 15      28 29 30 31
	becomes (for size == 2 and mask == 0x3333)
	   t =   2^16  3^17 -- --   l =  0  1 16 17  r =  2  3 18 19
		 6^20  7^21 -- --        4  5 20 21       6  7 22 23
		10^24 11^25 -- --        8  9 24 25      10 11 24 25
		14^28 15^29 -- --       12 13 28 29      14 15 28 29

	IP&FP could be done in 15 xor, 10 shifts and 5 ands.
	When I finally started to think of the problem in 2D
	I first got ~42 operations without xors.  When I remembered
	how to use xors :-) I got it to its final state. */
#define PERM_OP(a,b,t,n,m) \
	((t)=((((a)>>(n))^(b))&(m)), \
	(b)^=(t), \
	(a)^=((t)<<(n)))

#define IP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(r,l,tt, 4,0x0f0f0f0fL); \
	PERM_OP(l,r,tt,16,0x0000ffffL); \
	PERM_OP(r,l,tt, 2,0x33333333L); \
	PERM_OP(l,r,tt, 8,0x00ff00ffL); \
	PERM_OP(r,l,tt, 1,0x55555555L); \
	}

#define FP(l,r) \
	{ \
	register DES_LONG tt; \
	PERM_OP(l,r,tt, 1,0x55555555L); \
	PERM_OP(r,l,tt, 8,0x00ff00ffL); \
	PERM_OP(l,r,tt, 2,0x33333333L); \
	PERM_OP(r,l,tt,16,0x0000ffffL); \
	PERM_OP(l,r,tt, 4,0x0f0f0f0fL); \
	}

#define HPERM_OP(a,t,n,m) \
	((t)=((((a)<<(16-(n)))^(a))&(m)), \
	(a)=(a)^(t)^(t>>(16-(n))))

#define MAX_KEY_LENGTH	24
#define MAX_IV_LENGTH	8

#define RC2_INT ppUns32

#define RC2_BLOCK		8
#define RC2_KEY_LENGTH	16

#define RC2_40_MAGIC	0xa0
#define RC2_64_MAGIC	0x78
#define RC2_128_MAGIC	0x3a

typedef struct {
    ppUns32 subkeys[32];                                             /*subkeys */
    ppUns32 iv[2];                                       /*initializing vector */
    ppUns32 originalIV[2];                        /*for restarting the context */
    ppInt32 encrypt;                                                /*encrypt flag */
} DES_CBC_CTX;

typedef struct {
    ppUns32 subkeys[32];                                             /*subkeys */
    ppUns32 iv[2];                                       /*initializing vector */
    ppUns32 inputWhitener[2];                                 /*input whitener */
    ppUns32 outputWhitener[2];                               /*output whitener */
    ppUns32 originalIV[2];                        /*for restarting the context */
    ppInt32 encrypt;                                              /*encrypt flag */
} DESX_CBC_CTX;

typedef struct {
    ppUns32 subkeys[3][32];                     /*subkeys for three operations */
    ppUns32 iv[2];                                       /*initializing vector */
    ppUns32 originalIV[2];                        /*for restarting the context */
    ppInt32 encrypt;                                              /*encrypt flag */
} DES3_CBC_CTX;


typedef struct
{
    RC2_INT data[64];
} RC2_KEY;

typedef struct
{
	PDFLibHandle lib;
	ppInt32 cipher_id;
	ppInt32 encrypt;		/*encrypt or decrypt */
	ppInt32 buf_len;		/*number we have left */
	ppInt32 block_size;
	ppInt32 key_len;		/*Default value for variable length ciphers */
	ppInt32 iv_len;

	ppUns8  origiv[MAX_IV_LENGTH];	/*original iv */
	ppUns8  workiv[MAX_IV_LENGTH];	/*working iv */
	ppUns8  buf[MAX_IV_LENGTH];		/*saved partial block */

	union
	{
		struct
		{
			ppInt32 bits;
			RC2_KEY key;
		} rc2;

		union
		{
			struct
			{
				des_key_schedule ks;
			} des;
			struct
			{
				des_key_schedule ks;
				des_cblock inw;
				des_cblock outw;
			} des_cbc;
			struct
			{
				des_key_schedule ks1;
				des_key_schedule ks2;
				des_key_schedule ks3;
			} des_ede;
		} des;
	} c;
} cipher_Context;

typedef struct {
    asn1_OctetString *salt;
    asn1_Integer *iter;
} pbe_Param;

#define pkcs12_KeyGen pkcs12_KeyGenAscii

#define PKCS12_KEYGEN_KEYID	1
#define PKCS12_KEYGEN_IVID	2
#define PKCS12_KEYGEN_MACID	3

#define SHA1_BLOCK		16
#define SHA1_BLOCKSIZE	64
#define SHA1_HASHSIZE	20

typedef struct 
{
	ppUns32 Nl,Nh;
	ppUns32 h0,h1,h2,h3,h4;
	ppUns8 data[SHA1_BLOCKSIZE];
	ppInt32 num;
} sha1_Context;

typedef struct {
	x509_Algorithm *digestAlgorithm;
	asn1_OctetString *digest;
} pkcs7_DigestInfo;

#define pkcs7_DI pkcs7_DigestInfo

typedef struct {
    pkcs7_DigestInfo *mac;
    asn1_OctetString *macSalt;
    asn1_Integer *iterations;
} pkcs12_MacData;

typedef struct {
    asn1_Integer *version;
    pkcs12_MacData *macData;
    pkcs7 *authSafe;
} pkcs12;

typedef struct {
    asn1_Object *type;
    union {
        asn1_OctetString *x509cert;
        asn1_OctetString *x509crl;
        asn1_OctetString *octet;
        asn1_IA5String *sdsicert;
        asn1_Type *other;
    } value;
} pkcs12_Bag;

typedef struct {
    asn1_Integer *version;
    x509_Algorithm *pkeyalg;
    asn1_OctetString *pkey;
    VECTOR *attributes;
} pkcs8_PrivateKeyInfo;

#define pkcs8_PKI pkcs8_PrivateKeyInfo

typedef struct {
    asn1_Object *type;
    union {
        pkcs12_Bag *bag;
        pkcs8_PrivateKeyInfo *keybag;
        pkcs7_DigestInfo *shkeybag;
        VECTOR *safes;					/*pkcs12_SafeBag's */
        asn1_Type *other;
    } value;
    VECTOR *attrib;						/*x509_Attribute's */
    asn1_Type *rest;
} pkcs12_SafeBag;

typedef pkcs12_SafeBag *(*pkcs12_SafeBagDecodeFunc)(PDFLibHandle, pkcs12_SafeBag**, ppUns8 **, ppInt64);
typedef void (*pkcs12_SafeBagDisposeFunc)(PDFLibHandle, pkcs12_SafeBag* );
typedef VECTOR *(*pkcs12_SafeBagSeqUnpackFunc)(PDFLibHandle, ppUns8 *, ppInt32, pkcs12_SafeBagDecodeFunc, pkcs12_SafeBagDisposeFunc);
typedef VECTOR *(*p12dad_SafeBags)(PDFLibHandle, x509_Algorithm*, pkcs12_SafeBagDecodeFunc, 
                                   pkcs12_SafeBagDisposeFunc, const char *, ppInt32, asn1_String*, ppInt32);

#ifdef __cplusplus
extern "C" {
#endif

    void des_set_key_unchecked( const_des_cblock *key, des_key_schedule schedule );
    void des_ede3_cbc_encrypt( const ppUns8 *input, ppUns8 *output, 
        ppInt64 length, des_key_schedule ks1, des_key_schedule ks2,
        des_key_schedule ks3, des_cblock *ivec, ppInt32 enc);


    void DES_CBCInit (DES_CBC_CTX *, ppUns8 *, ppUns8 *, ppInt32);
    ppInt32 DES_CBCUpdate (DES_CBC_CTX *, ppUns8 *, ppUns8 *, ppUns32);
    void DES_CBCRestart (DES_CBC_CTX * );

    void DESX_CBCInit (DESX_CBC_CTX *, ppUns8 *, ppUns8 *, ppInt32);
    ppInt32 DESX_CBCUpdate (DESX_CBC_CTX *, ppUns8 *, ppUns8 *, ppUns32);
    void DESX_CBCRestart (DESX_CBC_CTX * );

    void DES3_CBCInit (DES3_CBC_CTX *, ppUns8 *, ppUns8 *, ppInt32);
    ppInt32 DES3_CBCUpdate (DES3_CBC_CTX *, ppUns8 *, ppUns8 *, ppUns32);
    void DES3_CBCRestart (DES3_CBC_CTX * );

    void RC2_set_key(RC2_KEY *key, ppInt32 len, const ppUns8 *data,ppInt32 bits);
    void RC2_encrypt(ppUns32 *data,RC2_KEY *key);
    void RC2_decrypt(ppUns32 *data,RC2_KEY *key);
    void RC2_cbc_encrypt(const ppUns8 *in, ppUns8 *out, ppInt64 length, RC2_KEY *ks, ppUns8 *iv, ppInt32 enc);

    /*RC2 in CBC mode */
    ppInt32 rc2_init_key(cipher_Context *ctx, const ppUns8 *key);
    ppInt32 rc2_cbc_cipher(cipher_Context *ctx, ppUns8 *out, const ppUns8 *in,
        ppUns32 len);

    /*Triple DES in CBC mode */
    ppInt32 des_ede3_init_key(cipher_Context *ctx, const ppUns8 *key);
    ppInt32 des_ede3_cbc_cipher(cipher_Context *ctx, ppUns8 *out, const ppUns8 *in,
        ppUns32 len);

    void cipher_Init(PDFLibHandle, cipher_Context*, ppUns8 *, ppUns8 *, ppInt32);
    void cipher_Update(cipher_Context*, ppUns8 *, ppInt32, ppUns8 *, ppInt32 * );
    void cipher_Finish(cipher_Context*, ppUns8 *, ppInt32 * );

    void pbe_Init( PDFLibHandle lib, asn1_Object *alg, const char *pass, ppInt32 plen,
        asn1_Type *param, cipher_Context *ctx, ppInt32 enc_dec );

    ppUns8 *pkcs12_PBE( PDFLibHandle lib, x509_Algorithm *alg, const char *pwd, ppInt32 plen, 
        ppUns8 *ibuf, ppInt32 ilen, ppUns8 **pobuf, ppInt32 *polen, ppInt32 enc_dec );

    ppInt32 pkcs12_KeyIvGen(PDFLibHandle, cipher_Context*, const char *, ppInt32,
        asn1_Type*, ppInt32);
    ppInt32 pkcs12_KeyGenUnicode(PDFLibHandle, const ppUns8 *, ppInt32, ppUns8 *, ppInt32,
        ppInt32, ppInt32, ppUns8 *, ppInt32);
    ppInt32 pkcs12_KeyGenAscii(PDFLibHandle, const char *, ppInt32, ppUns8 *, ppInt32, ppInt32,
        ppInt32, ppUns8 *, ppInt32);

    void sha1_Init( sha1_Context *c );
    void sha1_Update( sha1_Context *c, const ppUns8 *data, ppUns32 len );
    void sha1_Finish( sha1_Context *c, ppUns8 *h );
    void sha1_Pad(sha1_Context* );
    void sha1_Block(sha1_Context* );

    ppInt32 pkcs7_EncodeDigestInfo( pkcs7_DI *di, ppUns8 **pbuf );
    pkcs7_DI *pkcs7_DecodeDigestInfo( PDFLibHandle, pkcs7_DI **, ppUns8 **, ppInt64 );
    pkcs7_DI *pkcs7_CreateDigestInfo( PDFLibHandle );
    void pkcs7_DisposeDigestInfo(PDFLibHandle lib, pkcs7_DI *di);

    ppInt32 pkcs12_EncodeMacData( pkcs12_MacData *, ppUns8 ** );
    pkcs12_MacData *pkcs12_DecodeMacData( PDFLibHandle, pkcs12_MacData **, ppUns8 **, ppInt64 );
    pkcs12_MacData *pkcs12_CreateMacData( PDFLibHandle );
	void pkcs12_DisposeMacData(PDFLibHandle lib, pkcs12_MacData *dat);

    ppInt32 pkcs12_Encode( pkcs12 *, ppUns8 ** );
    pkcs12 *pkcs12_Decode( PDFLibHandle, pkcs12 **, ppUns8 **, ppInt64 );
    pkcs12 *pkcs12_Create( PDFLibHandle );
    void pkcs12_Dispose(PDFLibHandle lib, pkcs12 *obj);

    ppInt32 pkcs12_EncodeBag( pkcs12_Bag *, ppUns8 ** );
    pkcs12_Bag *pkcs12_DecodeBag( PDFLibHandle, pkcs12_Bag **, ppUns8 **, ppInt64 );
    pkcs12_Bag *pkcs12_CreateBag( PDFLibHandle );
    void pkcs12_DisposeBag(PDFLibHandle lib, pkcs12_Bag *bag);


    ppInt32 pkcs8_EncodePrivateKeyInfo( pkcs8_PKI *, ppUns8 ** );
    pkcs8_PKI *pkcs8_DecodePrivateKeyInfo( PDFLibHandle, pkcs8_PKI **, ppUns8 **, ppInt64 );
    pkcs8_PKI *pkcs8_CreatePrivateKeyInfo( PDFLibHandle );
    void pkcs8_DisposePrivateKeyInfo(PDFLibHandle lib, pkcs8_PrivateKeyInfo *pki);
    rsa_Key *rsa_UnShroudKey( PDFLibHandle, pkcs8_PKI * );

    ppInt32 pkcs12_EncodeSafeBag( pkcs12_SafeBag *, ppUns8 ** );
    pkcs12_SafeBag *pkcs12_DecodeSafeBag( PDFLibHandle, pkcs12_SafeBag **, ppUns8 **, ppInt64 );
    pkcs12_SafeBag *pkcs12_CreateSafeBag( PDFLibHandle );
    void pkcs12_DisposeSafeBag(PDFLibHandle lib, pkcs12_SafeBag *bag);

    VECTOR *pkcs12_DecryptAndDecode( PDFLibHandle, x509_Algorithm *, TDecodeFunction, TDisposeFunction, 
        const char *, ppInt32, asn1_OctetString *, ppInt32 );

    ppInt32 pkcs12_Parse( PDFLibHandle, pkcs12 *, const char *, rsa_Key **, x509 **, VECTOR ** );

#ifdef  __cplusplus
}
#endif

#endif

#endif /*endif VSPKCS12IH */
