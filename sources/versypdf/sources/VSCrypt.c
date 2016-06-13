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
VSCrypt.c
*********************************************************************/

#include "VSCrypt.h"
#include <time.h>
#include <string.h>
#include "VSAes.h"

/*ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))


/*FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation. */
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (ppUns32)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (ppUns32)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (ppUns32)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (ppUns32)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}


#define _DOC CD(Doc)
#define _LIB _DOC->Lib
void ULprepare_key(unsigned char *key_data_ptr, int key_data_len, ULrc4_key *key)
{
    ppUns8 swapByte;
    ppUns8 index1;
    ppUns8 index2;
    ppUns8 *state;
    ppInt32 counter;

    state = &key->state[0];
    for ( counter = 0; counter < 256; counter++ )
        state[counter] = ( ppUns8 ) counter;
    key->x = 0;
    key->y = 0;
    index1 = 0;
    index2 = 0;
    for ( counter = 0; counter < 256; counter++ ){
        index2 =(ppUns8) (( key_data_ptr[index1] + state[counter] + index2 ) % 256);
        swapByte = state[counter];
        state[counter] = state[index2];
        state[index2] = swapByte;
        index1 =(ppUns8) (( index1 + 1 ) % key_data_len);
    }
}

void ULrc4(ppUns8 *s, ppUns8 *d, ppInt32 buffer_len, ULrc4_key *key)
{
    ppUns8 x;
    ppUns8 y;
    ppUns8 *state;
    ppUns8 xorIndex;
    ppInt32 counter;
    ppUns8 swapByte;

    x = key->x;
    y = key->y;

    state = &key->state[0];
    for ( counter = 0; counter < buffer_len; counter ++ ){
        x = (ppUns8)(( x + 1 ) % 256);
        y = (ppUns8)(( state[x] + y ) % 256);
        swapByte = state[x];
        state[x] = state[y];
        state[y] = swapByte;
        xorIndex = (ppUns8)(( state[x] + state[y] ) % 256);
        d[counter] = s[counter] ^ state[xorIndex];
    }
    key->x = x;
    key->y = y;
}






static ppUns8 ULPADDING[64] ={0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))


static void ULMD5Transform(ppUns32 *buf, ppUns32 *in)
{
    ppUns32 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

    /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
    FF ( a, b, c, d, in[0], S11, 0xd76aa478 ); /* 1 */
    FF ( d, a, b, c, in[1], S12, 0xe8c7b756 ); /* 2 */
    FF ( c, d, a, b, in[2], S13, 0x242070db ); /* 3 */
    FF ( b, c, d, a, in[3], S14, 0xc1bdceee ); /* 4 */
    FF ( a, b, c, d, in[4], S11, 0xf57c0faf ); /* 5 */
    FF ( d, a, b, c, in[5], S12, 0x4787c62a ); /* 6 */
    FF ( c, d, a, b, in[6], S13, 0xa8304613 ); /* 7 */
    FF ( b, c, d, a, in[7], S14, 0xfd469501 ); /* 8 */
    FF ( a, b, c, d, in[8], S11, 0x698098d8 ); /* 9 */
    FF ( d, a, b, c, in[9], S12, 0x8b44f7af ); /* 10 */
    FF ( c, d, a, b, in[10], S13, 0xffff5bb1 ); /* 11 */
    FF ( b, c, d, a, in[11], S14, 0x895cd7be ); /* 12 */
    FF ( a, b, c, d, in[12], S11, 0x6b901122 ); /* 13 */
    FF ( d, a, b, c, in[13], S12, 0xfd987193 ); /* 14 */
    FF ( c, d, a, b, in[14], S13, 0xa679438e ); /* 15 */
    FF ( b, c, d, a, in[15], S14, 0x49b40821 ); /* 16 */

    /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
    GG ( a, b, c, d, in[1], S21, 0xf61e2562 ); /* 17 */
    GG ( d, a, b, c, in[6], S22, 0xc040b340 ); /* 18 */
    GG ( c, d, a, b, in[11], S23, 0x265e5a51 ); /* 19 */
    GG ( b, c, d, a, in[0], S24, 0xe9b6c7aa ); /* 20 */
    GG ( a, b, c, d, in[5], S21, 0xd62f105d ); /* 21 */
    GG ( d, a, b, c, in[10], S22, 0x2441453 ); /* 22 */
    GG ( c, d, a, b, in[15], S23, 0xd8a1e681 ); /* 23 */
    GG ( b, c, d, a, in[4], S24, 0xe7d3fbc8 ); /* 24 */
    GG ( a, b, c, d, in[9], S21, 0x21e1cde6 ); /* 25 */
    GG ( d, a, b, c, in[14], S22, 0xc33707d6 ); /* 26 */
    GG ( c, d, a, b, in[3], S23, 0xf4d50d87 ); /* 27 */
    GG ( b, c, d, a, in[8], S24, 0x455a14ed ); /* 28 */
    GG ( a, b, c, d, in[13], S21, 0xa9e3e905 ); /* 29 */
    GG ( d, a, b, c, in[2], S22, 0xfcefa3f8 ); /* 30 */
    GG ( c, d, a, b, in[7], S23, 0x676f02d9 ); /* 31 */
    GG ( b, c, d, a, in[12], S24, 0x8d2a4c8a ); /* 32 */

    /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
    HH ( a, b, c, d, in[5], S31, 0xfffa3942 ); /* 33 */
    HH ( d, a, b, c, in[8], S32, 0x8771f681 ); /* 34 */
    HH ( c, d, a, b, in[11], S33, 0x6d9d6122 ); /* 35 */
    HH ( b, c, d, a, in[14], S34, 0xfde5380c ); /* 36 */
    HH ( a, b, c, d, in[1], S31, 0xa4beea44 ); /* 37 */
    HH ( d, a, b, c, in[4], S32, 0x4bdecfa9 ); /* 38 */
    HH ( c, d, a, b, in[7], S33, 0xf6bb4b60 ); /* 39 */
    HH ( b, c, d, a, in[10], S34, 0xbebfbc70 ); /* 40 */
    HH ( a, b, c, d, in[13], S31, 0x289b7ec6 ); /* 41 */
    HH ( d, a, b, c, in[0], S32, 0xeaa127fa ); /* 42 */
    HH ( c, d, a, b, in[3], S33, 0xd4ef3085 ); /* 43 */
    HH ( b, c, d, a, in[6], S34, 0x4881d05 ); /* 44 */
    HH ( a, b, c, d, in[9], S31, 0xd9d4d039 ); /* 45 */
    HH ( d, a, b, c, in[12], S32, 0xe6db99e5 ); /* 46 */
    HH ( c, d, a, b, in[15], S33, 0x1fa27cf8 ); /* 47 */
    HH ( b, c, d, a, in[2], S34, 0xc4ac5665 ); /* 48 */



    /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
    II ( a, b, c, d, in[0], S41, 0xf4292244 ); /* 49 */
    II ( d, a, b, c, in[7], S42, 0x432aff97 ); /* 50 */
    II ( c, d, a, b, in[14], S43, 0xab9423a7 ); /* 51 */
    II ( b, c, d, a, in[5], S44, 0xfc93a039 ); /* 52 */
    II ( a, b, c, d, in[12], S41, 0x655b59c3 ); /* 53 */
    II ( d, a, b, c, in[3], S42, 0x8f0ccc92 ); /* 54 */
    II ( c, d, a, b, in[10], S43, 0xffeff47d ); /* 55 */
    II ( b, c, d, a, in[1], S44, 0x85845dd1 ); /* 56 */
    II ( a, b, c, d, in[8], S41, 0x6fa87e4f ); /* 57 */
    II ( d, a, b, c, in[15], S42, 0xfe2ce6e0 ); /* 58 */
    II ( c, d, a, b, in[6], S43, 0xa3014314 ); /* 59 */
    II ( b, c, d, a, in[13], S44, 0x4e0811a1 ); /* 60 */
    II ( a, b, c, d, in[4], S41, 0xf7537e82 ); /* 61 */
    II ( d, a, b, c, in[11], S42, 0xbd3af235 ); /* 62 */
    II ( c, d, a, b, in[2], S43, 0x2ad7d2bb ); /* 63 */
    II ( b, c, d, a, in[9], S44, 0xeb86d391 ); /* 64 */

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


void ULMD5Init(ULMD5_CTX *mdContext)
{
    mdContext->i[0] = mdContext->i[1] = ( ppUns32 ) 0;
    mdContext->buf[0] = ( ppUns32 ) 0x67452301;
    mdContext->buf[1] = ( ppUns32 ) 0xefcdab89;
    mdContext->buf[2] = ( ppUns32 ) 0x98badcfe;
    mdContext->buf[3] = ( ppUns32 ) 0x10325476;
}

void ULMD5Update(ULMD5_CTX *mdContext, ppUns8 *inBuf, ppInt32 inLen)
{
    ppUns32 in[16];
    ppInt32 mdi;
    ppUns32 i, ii;

    /* compute number of bytes mod 64 */
    mdi = ( ppInt32 ) ( ( mdContext->i[0] >> 3 ) & 0x3F );

    /* update number of bits */
    if ( ( mdContext->i[0] + ( ( ppUns32 ) inLen << 3 ) ) < mdContext->i[0] )
        mdContext->i[1]++;
    mdContext->i[0] += ( ( ppUns32 ) inLen << 3 );
    mdContext->i[1] += ( ( ppUns32 ) inLen >> 29 );

    while ( inLen-- ){ 
        /* add new character to buffer, increment mdi */
        mdContext->in[mdi++] = *inBuf++;

        /* transform if necessary */
        if ( mdi == 0x40 ){
            for ( i = 0, ii = 0; i < 16; i++, ii += 4 )
                in[i] = ( ( ( ppUns32 ) mdContext->in[ii + 3] ) << 24 ) |
                        ( ( ( ppUns32 ) mdContext->in[ii + 2] ) << 16 ) |
                        ( ( ( ppUns32 ) mdContext->in[ii + 1] ) << 8 ) |
                        ( ( ppUns32 ) mdContext->in[ii] );
            ULMD5Transform ( mdContext->buf, in );
            mdi = 0;
        }
    }
}

void ULMD5Final(ULMD5_CTX *mdContext, ULTMD5Digest *Dig)
{
    ppUns32 in[16];
    ppInt32 mdi;
    ppInt32 i, ii;
    ppUns32 padLen;

    /* save number of bits */
    in[14] = mdContext->i[0];
    in[15] = mdContext->i[1];

    /* compute number of bytes mod 64 */
    mdi = ( ppInt32 ) ( ( mdContext->i[0] >> 3 ) & 0x3F );

    /* pad out to 56 mod 64 */
    padLen = ( mdi < 56 ) ? ( 56 - mdi ) : ( 120 - mdi );
    ULMD5Update ( mdContext, ULPADDING, padLen );

    /* append length in bits and transform */
    for ( i = 0, ii = 0; i < 14; i++, ii += 4 )
        in[i] = ( ( ( ppUns32 ) mdContext->in[ii + 3] ) << 24 ) |
                ( ( ( ppUns32 ) mdContext->in[ii + 2] ) << 16 ) |
                ( ( ( ppUns32 ) mdContext->in[ii + 1] ) << 8 ) |
                ( ( ppUns32 ) mdContext->in[ii] );
    ULMD5Transform ( mdContext->buf, in );

    /* store buffer in digest */
    for ( i = 0, ii = 0; i < 4; i++, ii += 4 ){
        ( *Dig )[ii] = ( ppUns8 ) ( mdContext->buf[i] & 0xFF );
        ( *Dig )[ii + 1] = ( ppUns8 ) ( ( mdContext->buf[i] >> 8 ) & 0xFF );
        ( *Dig )[ii + 2] = ( ppUns8 ) ( ( mdContext->buf[i] >> 16 ) & 0xFF );
        ( *Dig )[ii + 3] = ( ppUns8 ) ( ( mdContext->buf[i] >> 24 ) & 0xFF );
    }
}

void ULxCalcMD5(ppUns8 *data, ppInt32 len, ULTMD5Digest *dig)
{
    ULMD5_CTX Context;
    ULMD5Init ( &Context );
    ULMD5Update ( &Context, data, len );
    ULMD5Final ( &Context, dig );
}


ppUns8 ULPassKey[32]={0x28, 0xBF, 0x4E, 0x5E, 0x4E, 0x75, 0x8A, 0x41, 0x64, 0x00, 0x4E, 0x56,
                      0xFF, 0xFA, 0x01, 0x08, 0x2E, 0x2E, 0x00, 0xB6, 0xD0, 0x68, 0x3E, 0x80,
                      0x2F, 0x0C, 0xA9, 0xFE, 0x64, 0x53, 0x69, 0x7A};

#define CR(x) ((PPDFCryptoType)x)

void ULInitCryptoData(PDFDocHandle Doc, PDFCryptoHandle Data, ppInt32 Version, ppInt32 Revision,
                      ppInt32 KeyLength, PDFCosHandle FileID)
{
    CR ( Data )->KeyLength = KeyLength;
    CR ( Data )->Version = Version;
    CR ( Data )->Revision = Revision;
    CR ( Data )->FileID = CosCopy ( Doc, FileID );
}

void ULPadTrunc(char *s, ppInt32 sl, char *d)
{
    ppInt32 i, j;
    for ( i = 0; i < 32 && i < sl; i++ )
        d[i] = s[i];
    for ( j = 0; i < 32; j++,i++ )
        d[i] = ULPassKey[j];
}

void ULCreateKey( PDFCryptoHandle Data, char *Owner, char *User)
{
    unsigned char pass[32];
    ppInt32 i, j;
    ppInt32 ol = 0, ul = 0;
    ULrc4_key key;
    ULTMD5Digest digest, dg1;
    ULMD5_CTX Cont;
    char *flid;
    ppUns32 sz;
    ppUns32 BK[9];
	ppUns8 UVS[8], UKS[8], OVS[8], OKS[8];
	SHA256Context Sh;
	ppUns8 Dig[32];
	ppUns8 zz[16], zv[16];
	aes_context aes;

	if ( CR ( Data )->Version < 5 ){
		if ( Owner )
			ol = ( ppInt32 ) strlen ( Owner );
		if ( User )
			ul = ( ppInt32 ) strlen ( User );
		if ( ol == 0 )
			ULPadTrunc ( User, ul, ( char * ) pass );
		else
			ULPadTrunc ( Owner, ol, ( char * ) pass );
		ULxCalcMD5 ( pass, 32, &digest );
		if ( CR ( Data )->Revision >= 3 )
			for ( i = 0; i < 50; i++ )
				ULxCalcMD5 ( digest, CR ( Data )->KeyLength, &digest );
		else
			CR ( Data )->KeyLength = 5;
		ULprepare_key ( digest, CR ( Data )->KeyLength, &key );
		ULPadTrunc ( User, ul, ( char * ) pass );
		ULrc4 ( pass, CR ( Data )->COP, 32, &key );
		if ( CR ( Data )->Revision >= 3 )
			for ( i = 1; i < 20; i++ ){
				for ( j = 0; j < CR ( Data )->KeyLength; j++ )
					dg1[j] = ( ppUns8 ) ( digest[j] ^ i );
				ULprepare_key ( dg1, CR ( Data )->KeyLength, &key );
				ULrc4 ( CR ( Data )->COP, CR ( Data )->COP, 32, &key );
			};
		ULPadTrunc ( User, ul, ( char * ) pass );
		ULMD5Init ( &Cont );
		ULMD5Update ( &Cont, pass, 32 );
		ULMD5Update ( &Cont, CR ( Data )->COP, 32 );
		i = CR ( Data )->Permission;
		SSWAPL ( i );
		ULMD5Update ( &Cont, ( unsigned char * ) &( i ), 4 );
		flid = CosStringGetValue ( CR ( Data )->FileID, &sz );
		ULMD5Update ( &Cont, ( ppUns8 * ) flid, sz );
		if ( CR(Data)->Revision >=4 && !CR(Data)->EncryptMetadata){
			i = 0xFFFFFFFF;
			ULMD5Update ( &Cont, ( unsigned char * ) &( i ), 4 );
		}
		ULMD5Final ( &Cont, &digest );
		if ( CR ( Data )->Revision >= 3 )
			for ( i = 0; i < 50; i++ )
				ULxCalcMD5 ( digest, CR ( Data )->KeyLength, &digest );
		memcpy ( CR ( Data )->key, digest, CR ( Data )->KeyLength );
		if ( CR ( Data )->Revision == 2 ){
			ULprepare_key ( CR ( Data )->key, CR ( Data )->KeyLength, &key );
			ULrc4 ( ULPassKey, CR ( Data )->CUP, 32, &key );
		} else{
			ULMD5Init ( &Cont );
			ULMD5Update ( &Cont, ULPassKey, 32 );
			ULMD5Update ( &Cont, ( ppUns8 * ) flid, sz );
			ULMD5Final ( &Cont, &digest );
			ULprepare_key ( CR ( Data )->key, CR ( Data )->KeyLength, &key );
			ULrc4 ( digest, digest, 16, &key );
			for ( i = 1; i < 20; i++ ){
				for ( j = 0; j < CR ( Data )->KeyLength; j++ )
					dg1[j] = ( ppUns8 ) ( CR ( Data )->key[j] ^ i );
				ULprepare_key ( dg1, CR ( Data )->KeyLength, &key );
				ULrc4 ( digest, digest, 16, &key );
			};
			memcpy ( CR ( Data )->CUP, digest, 16 );
			for ( i = 16; i < 32; i++ )
				CR ( Data )->CUP[i] = ' ';
		}
	} else {
		srand ((unsigned int)time(NULL));
		for (i= 0;i <= 7;i++){
			BK[i]= ((rand() & 0x7fff) << 16) + (rand() & 0x7fff);
			UVS[i]= rand() & 0xFF;
			UKS[i]= rand() & 0xFF;
			OVS[i]= rand() & 0xFF;
			OKS[i]= rand() & 0xFF;
		}

		ULSHA256Init(&Sh);
		ULSHA256Update(&Sh,(ppUns8 *)User, min(127,(ppUns32)strlen(User)));
		ULSHA256Update(&Sh,UVS,8);
		ULSHA256Final(&Sh, Dig);
		memcpy(CR ( Data )->CUP, Dig,32);
		memcpy(CR ( Data )->CUP+32, UVS,8);
		memcpy(CR ( Data )->CUP+40, UKS,8);
	
		memset(zv,0,16);
		ULSHA256Init(&Sh);
		ULSHA256Update(&Sh,(ppUns8 *)User, min(127,(ppUns32)strlen(User)));
		ULSHA256Update(&Sh,UKS,8);
		ULSHA256Final(&Sh, Dig);
		ULaes_set_key(NULL,&aes,Dig,256);
		ULaes_cbc_encrypt( &aes,zv,(ppUns8*)&BK[0], CR ( Data )->CUPE, 32);

		ULSHA256Init(&Sh);
		ULSHA256Update(&Sh,(ppUns8 *)Owner, min(127,(ppUns32)strlen(Owner)));
		ULSHA256Update(&Sh,OVS,8);
		ULSHA256Update(&Sh,CR ( Data )->CUP,48);
		ULSHA256Final(&Sh,Dig);
	
		memcpy(CR ( Data )->COP,Dig,32);
		memcpy(CR ( Data )->COP+32,OVS,8);
		memcpy(CR ( Data )->COP+40,OKS,8);

		ULSHA256Init(&Sh);
		ULSHA256Update(&Sh,(ppUns8 *)Owner, min(127,(ppUns32)strlen(Owner)));
		ULSHA256Update(&Sh,OKS,8);
		ULSHA256Update(&Sh,CR ( Data )->CUP,48);
		ULSHA256Final(&Sh,Dig);

		memset(zv,0,16);
		ULaes_set_key(NULL,&aes,Dig,256);
		ULaes_cbc_encrypt( &aes,zv,(ppUns8*)&BK[0], CR ( Data )->COPE, 32);
		
		memset(zv,0,16);
		zz[0] = (ppUns8)CR ( Data )->Permission;
		zz[1] = (ppUns8)(CR ( Data )->Permission >> 8);
		zz[2] = (ppUns8)(CR ( Data )->Permission >> 16);
		zz[3] = (ppUns8)(CR ( Data )->Permission >>24);
		zz[4] = 0xff;
		zz[5] = 0xff;
		zz[6] = 0xff;
		zz[7] = 0xff;
		if (CR ( Data )->EncryptMetadata)
			zz[8] = 'T';
		else
			zz[8] = 'F';
		zz[9] = 'a';
		zz[10] = 'd';
		zz[11] = 'b';
		zz[12] = 0;
		zz[13] = 0;
		zz[14] = 0;
		zz[15] = 0;

		ULaes_set_key(NULL,&aes,(ppUns8*)&BK[0],256);
		ULaes_cbc_encrypt(&aes,zv,zz, CR ( Data )->Perm, 16);
		memcpy(CR ( Data )->key, (ppUns8*)&BK[0], 32);
	}
}
TPDFCheckPasword ULCheckUserPassword( PDFCryptoHandle Data, char *Password, ppInt32 pl)
{
    unsigned char pass[32], calcp[32];
    ppInt32 i, j, Perm;
    ULrc4_key key;
    ULTMD5Digest digest, dg1;
    ULMD5_CTX Cont;
    char *flid;
    ppUns32 sz;
	SHA256Context Sh;
	ppUns8 Dig[32];
	ppUns8 zz[16], zv[16];
	aes_context aes;
	ppInt32 P;
	if (!(CR ( Data )->Version == 5 && CR ( Data )->Revision == 5)){
		ULPadTrunc ( Password, pl, ( char * ) pass );
		ULMD5Init ( &Cont );
		ULMD5Update ( &Cont, pass, 32 );
		ULMD5Update ( &Cont, CR ( Data )->COP, 32 );
		Perm = CR ( Data )->Permission;
		SSWAPL(Perm);
		ULMD5Update ( &Cont, ( unsigned char * ) &Perm, 4 );
		flid = CosStringGetValue ( CR ( Data )->FileID, &sz );
		ULMD5Update ( &Cont, ( ppUns8 * ) flid, sz );
		if ( CR(Data)->Revision >=4 && !CR(Data)->EncryptMetadata){
			Perm = 0xFFFFFFFF;
			ULMD5Update ( &Cont, ( unsigned char * ) &Perm, 4 );
		}
		ULMD5Final ( &Cont, &digest );
		if ( CR ( Data )->Revision >= 3 )
			for ( i = 0; i < 50; i++ )
				ULxCalcMD5 ( digest, CR ( Data )->KeyLength, &digest );
		memcpy ( CR ( Data )->key, digest, CR ( Data )->KeyLength );
		if ( CR ( Data )->Revision == 2 ){
			ULprepare_key ( CR ( Data )->key, CR ( Data )->KeyLength, &key );
			ULrc4 ( ULPassKey, calcp, 32, &key );
			for ( i = 0; i < 32; i++ )
				if ( calcp[i] != CR ( Data )->CUP[i] )
					return cpInvalid;
			return cpValid;
		} else{
			ULMD5Init ( &Cont );
			ULMD5Update ( &Cont, ULPassKey, 32 );
			ULMD5Update ( &Cont, ( ppUns8 * ) flid, sz );
			ULMD5Final ( &Cont, &digest );
			for ( i = 0; i < 20; i++ ){
				for ( j = 0; j < CR ( Data )->KeyLength; j++ )
					dg1[j] = ( ppUns8 ) ( CR ( Data )->key[j] ^ i );
				ULprepare_key ( dg1, CR ( Data )->KeyLength, &key );
				ULrc4 ( digest, digest, 16, &key );
			};
			for ( i = 0; i < 16; i++ )
				if ( digest[i] != ( ( PPDFCryptoType ) Data )->CUP[i] )
					return cpInvalid;
			return cpValid;
		};
	} else {
		ULSHA256Init(&Sh);
		if (Password)
			ULSHA256Update(&Sh,(ppUns8 *)Password, min(127,(ppUns32)strlen(Password)));
		ULSHA256Update(&Sh,CR ( Data )->CUP+32,8);
		ULSHA256Final(&Sh, Dig);
		if (memcmp(CR ( Data )->CUP,Dig,32))
			return cpInvalid;
		ULSHA256Init(&Sh);
		if (Password)
			ULSHA256Update(&Sh,(ppUns8 *)Password, min(127,(ppUns32)strlen(Password)));
		ULSHA256Update(&Sh,CR ( Data )->CUP+40,8);
		ULSHA256Final(&Sh, Dig);

		memset(zv,0,16);
		ULaes_set_key(NULL,&aes,Dig,256);
		ULaes_cbc_decrypt( &aes,zv,CR ( Data )->CUPE,CR ( Data )->key,  32);


		memset(zv,0,16);
		ULaes_set_key(NULL,&aes,CR ( Data )->key,256);
		ULaes_cbc_decrypt( &aes,zv,CR ( Data )->Perm,zz, 16);
		P =  zz[0] + (zz[1] << 8) + (zz[2] << 16) + (zz[2] << 24);
		if ((CR ( Data )->Permission != P) || (zz[9]!=(ppUns8) 'a') || (zz[10] != (ppUns8)'d') || 
			(zz[11] != (ppUns8)'b') || (CR ( Data )->EncryptMetadata != (zz[8]==(ppUns8)'T')?true:false))
			return cpCorrupted;
		return cpValid;
	}
}

TPDFCheckPasword ULCheckOwnerPassword( PDFCryptoHandle Data, char *Password, ppInt32 pl)
{
    ULrc4_key key;
    unsigned char pass[32];
    ppInt32 i, j;
    ULTMD5Digest digest, dg1;
	SHA256Context Sh;
	ppUns8 Dig[32];
	ppUns8 zz[16], zv[16];
	aes_context aes;
	ppInt32 P;
	if (!(CR ( Data )->Version == 5 && CR ( Data )->Revision == 5)){
		ULPadTrunc ( Password, pl, ( char * ) pass );
		ULxCalcMD5 ( pass, 32, &digest );
		if ( CR ( Data )->Revision >= 3 )
			for ( i = 0; i < 50; i++ )
				ULxCalcMD5 ( digest, CR ( Data )->KeyLength, &digest );
		if ( CR ( Data )->Revision == 2 ){
			ULprepare_key ( digest, CR ( Data )->KeyLength, &key );
			ULrc4 ( CR ( Data )->COP, pass, 32, &key );
		} else{
			memcpy ( pass, CR ( Data )->COP, 32 );
			for ( i = 19; i >= 0; i-- ){
				for ( j = 0; j < CR ( Data )->KeyLength; j++ )
					dg1[j] = ( ppUns8 ) ( digest[j] ^ i );
				ULprepare_key ( dg1, CR ( Data )->KeyLength, &key );
				ULrc4 ( pass, pass, 32, &key );
			};
		};
		return ULCheckUserPassword ( Data, ( char * ) pass, 32 );
	} else {		
		ULSHA256Init(&Sh);
		if (Password)
		  ULSHA256Update(&Sh,(ppUns8 *)Password, min(127,(ppUns32)strlen(Password)));
		ULSHA256Update(&Sh,CR ( Data )->COP+32,8);
		ULSHA256Update(&Sh,CR ( Data )->CUP,48);
		ULSHA256Final(&Sh, Dig);
		if (memcmp(CR ( Data )->COP,Dig,32))
			return cpInvalid;
		memset(zv,0,16);
		ULSHA256Init(&Sh);
		if (Password)
			ULSHA256Update(&Sh,(ppUns8 *)Password, min(127,(ppUns32)strlen(Password)));
		ULSHA256Update(&Sh,CR ( Data )->COP+40,8);
		ULSHA256Update(&Sh,CR ( Data )->CUP,48);
		ULSHA256Final(&Sh, Dig);
		ULaes_set_key(NULL,&aes,Dig,256);
		ULaes_cbc_decrypt( &aes,zv,CR ( Data )->COPE,CR ( Data )->key,  32);
		
		memset(zv,0,16);
		ULaes_set_key(NULL,&aes,CR ( Data )->key,256);
		ULaes_cbc_decrypt( &aes,zv,CR ( Data )->Perm,zz, 16);
		P =  zz[0] + (zz[1] << 8) + (zz[2] << 16) + (zz[2] << 24);
		if (CR ( Data )->Permission != P || zz[9]!=(ppUns8) 'a' || zz[10] != (ppUns8)'d' || 
			zz[11] != (ppUns8)'b' || (CR ( Data )->EncryptMetadata != (zz[8]==(ppUns8)'T')?true:false))
			return cpCorrupted;
		return cpValid;

	}
}

TPDFCheckPasword ULCheckPassword(PDFCryptoHandle Data, char *Password, ppInt32 pl)
{
	TPDFCheckPasword ch;
	ch = ULCheckOwnerPassword ( Data, Password, pl );
    if ( ch == cpValid )
        return cpValidOwner;
	if (ch == cpCorrupted)
		return ch;
    ch = ULCheckUserPassword ( Data, Password, pl );        
	if ( ch == cpValid )
		return cpValidUser;
	return ch;
}

PDFCosHandle ULCreateFileID(PDFDocHandle Doc, char *instr)
{
    ULMD5_CTX Dig;
    ULTMD5Digest d;
    time_t t;
    void *ch;
    ULMD5Init ( &Dig ); 
    ULMD5Update ( &Dig, ( unsigned char * ) instr, ( ppInt32 ) strlen ( instr ) );
	
    t = time ( NULL );
    ch = &t;
    ULMD5Update ( &Dig, (ppUns8 *) ch, sizeof ( t ) );
    ULMD5Final ( &Dig, &d );
    ch = &d;
    return CosStringNew ( Doc, false ,( char * ) ch, 16 );
}

ppBool CryptFilterCheck( PDFCosHandle StreamAttr, ppAtom * Filter)
{
	PDFCosHandle fl, dp, wrk;
	ppUns32 i;
	PDFDocHandle Doc = _CosDoc ( StreamAttr );
	ppAtom Identity = GetAtomDoc( Identity );
	fl = _CosDictValueByName ( StreamAttr, Filter );
	if ( _IsCosNull ( fl ) )
		fl = _CosDictValueByName ( StreamAttr, F );
	if ( _IsCosName ( fl ) && _CosNameValue ( fl ) == GetAtomDoc(Crypt) ){
		dp = _CosDictValueByName ( StreamAttr, DecodeParms );
		if ( _IsCosNull ( dp ) )
			dp = _CosDictValueByName ( StreamAttr, DP );
		if ( _IsCosArray( dp ))
			dp = CosArrayItem ( dp, 0 );
		if (_IsCosDict(dp)){
			wrk = _CosDictValueByName( dp, Name );
			if ( _IsCosName ( wrk ))
				*Filter = _CosNameValue ( wrk );
			else 
				*Filter = Identity;
		} else {
			*Filter = Identity;
		}
		return true;
	}
	if ( _IsCosArray( fl )){
		for ( i = 0; i < _CosArrayCount( fl ); i++ ){
			wrk = CosArrayItem ( fl, i );
			if ( _IsCosName ( wrk ) && _CosNameValue ( wrk ) == GetAtomDoc(Crypt) ){
				dp = _CosDictValueByName ( StreamAttr, DecodeParms );
				if ( _IsCosNull ( dp ) )
					dp = _CosDictValueByName ( StreamAttr, DP );
				if ( i != 0 && _IsCosDict( dp ) )
					dp = _CosNull ( Doc );
				if ( _IsCosArray( dp ))
					dp = CosArrayItem ( dp, i );
				if (_IsCosDict(dp)){
					wrk = _CosDictValueByName( dp, Name );
					if ( _IsCosName ( wrk ))
						*Filter = _CosNameValue ( wrk );
					else 
						*Filter = Identity;
				} else {
					*Filter = Identity;
				}
				return true;
			}
		}
	}
	return false;
}


void ULCryptWithAES ( PPDFLib Lib, TPDFCryptoType *Data, PDFID ID, ppUns8 *Buffer, ppUns32 Size, ppBool IsEncrypt )
{
	unsigned char Fullkey[25];
	ULTMD5Digest d;
	aes_context ctx;
	ppUns8 iv[16];
	ppUns32 i;
	ppUns16 *R;
	if (Data->Version == 4){
		memset ( Fullkey, 0, 21 );
		memcpy ( Fullkey, Data->key, 16 );
		Fullkey[16] = ( unsigned char ) ID.ID ;
		Fullkey[17] = ( unsigned char ) ( ID.ID >> 8 );
		Fullkey[18] = ( unsigned char ) ( ID.ID >> 16 ); 	
		Fullkey[19] = ( unsigned char ) ID.GenID;
		Fullkey[20] = ( unsigned char ) ( ID.GenID >> 8 );
		Fullkey[21] = 0x73;
		Fullkey[22] = 0x41;
		Fullkey[23] = 0x6C;
		Fullkey[24] = 0x54;
		ULxCalcMD5 ( Fullkey, 25, &d );
		ULaes_set_key ( Lib, &ctx, (ppUns8 *)&d, 128 );
	} else {
		ULaes_set_key ( Lib, &ctx, Data->key, 256 );
	}
	if (IsEncrypt){
		R = (ppUns16*)iv;
		for ( i = 0; i < 8;i++, R++)
			*R = (ppUns16)rand ();
		memcpy(Buffer, iv, 16 );
		ULaes_cbc_encrypt( &ctx, Buffer, &(Buffer[16]), &(Buffer[16]), Size-16 );
		memcpy(Buffer, iv, 16 );
	} else
		ULaes_cbc_decrypt( &ctx, Buffer, &(Buffer[16]), &(Buffer[16]), Size-16 );

	
}

void ULCryptData( PDFCryptoHandle Data, PDFID ID, void *Buffer, ppInt32 Size )
{
	unsigned char Fullkey[21];
	ULTMD5Digest d;
	ULrc4_key AKey;
	if ( Size == 0 )
		return;
	memset ( Fullkey, 0, 21 );
	memcpy ( Fullkey, CR ( Data )->key, CR ( Data )->KeyLength );
	Fullkey[CR ( Data )->KeyLength + 2] = ( unsigned char ) ( ID.ID >> 16 ); 
	Fullkey[CR ( Data )->KeyLength + 1] = ( unsigned char ) ( ID.ID >> 8 );
	Fullkey[CR ( Data )->KeyLength] = ( unsigned char ) ID.ID ;
	Fullkey[CR ( Data )->KeyLength + 4] = ( unsigned char ) ( ID.GenID >> 8 );
	Fullkey[CR ( Data )->KeyLength + 3] = ( unsigned char ) ID.GenID;
	ULxCalcMD5 ( Fullkey, CR ( Data )->KeyLength + 5, &d );
	ULprepare_key ( d, min ( 16, CR ( Data )->KeyLength + 5 ), &AKey );
	ULrc4 ( (ppUns8 *)Buffer, (ppUns8 *)Buffer, Size, &AKey );
}

void ULRemoveCryptFilter (PDFCosHandle CosObject)
{
	PDFCosHandle wrk, wrk1;
	ppUns32 i;
	wrk = CosDictValueByName ( CosObject, GetAtomLib( _CosLib(CosObject), Filter ) ) ;
	if (_IsCosName( wrk )){
		if ( _CosNameValue( wrk ) == GetAtomLib( _CosLib(CosObject), Crypt ) ){
			CosDictAppend ( CosObject, GetAtomLib( _CosLib(CosObject), Filter ),  _CosNull( _CosDoc ( CosObject)));
			CosDictAppend ( CosObject, GetAtomLib( _CosLib(CosObject), DecodeParms ),  _CosNull( _CosDoc ( CosObject)));
		} 
		return;
	}
	if ( _IsCosArray ( wrk ) ){
		for ( i = 0; i < _CosArrayCount( wrk ); i++){
			wrk1 = CosArrayItem ( wrk, i );
			if ( _IsCosName ( wrk1 ) && _CosNameValue( wrk1 ) == GetAtomLib( _CosLib(CosObject), Crypt ) ){
				CosArrayRemove ( wrk, i );
				wrk1 = CosDictValueByName ( CosObject, GetAtomLib( _CosLib(CosObject), DecodeParms ) ) ;
				if (_IsCosName( wrk1 ) && !i){
					CosDictAppend ( CosObject, GetAtomLib( _CosLib(CosObject), DecodeParms ),  _CosNull( _CosDoc ( CosObject)));
					return;
				}
				if ( _IsCosArray ( wrk1 ) ){
					CosArrayRemove ( wrk1, i );		
				}
				return;
			}
		}
	}
}


void ULEncryptObjectR4orR5( PPDFCryptoType Data, PDFCosHandle CosObject, PDFID ID )
{
	PDFCosHandle w;
	ppUns32 i, kap, sz;
	PDFStreamHandle WStream;
	ppUns32 NewStringSize;
	ppAtom Dec;
	PDFDocHandle Doc = _CosDoc ( CosObject );
	if ( Data->RemoveOldFilters ){
		if ( _IsCosStream( CosObject ) ){
			ULRemoveCryptFilter ( CosObject );
			if ( !CR(Data)->EncryptMetadata ) {
				w = _CosDictValueByName ( CosObject, Type);
				if (_IsCosName ( w ) && _CosNameValue ( w ) == GetAtomDoc ( Metadata ) )
					return;
			}
			WStream = _CosStreamValue( CosObject );
			if (CR( Data)->nkl == pt128BitProtection ){
				ULCryptData ( Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream )  );
			} else {
				sz = _StreamSize( WStream );
				kap = sz & 0xf;
				NewStringSize = sz + 16 + ( kap = kap ? 16-kap:16 );
				if ( _StreamCapacity( WStream) < NewStringSize )
					ULStrmSetCapacity (WStream, NewStringSize);
				memset( &(_StreamBuffer(WStream)[sz+16]), kap, kap );
				memmove( &( _StreamBuffer(WStream)[16]), _StreamBuffer(WStream), sz );
				_StreamSize( WStream ) = NewStringSize;
				_StreamPosition ( WStream ) = 0;
				ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream ), true );
				CosDictAppend ( CosObject, GetAtomLib( _StreamLib( WStream ),Length ),
					CosIntNew( _CosDoc ( CosObject ), false, NewStringSize ));
			}
		} else {
			if (CR( Data)->nkl == pt128BitProtection ){
				ULCryptData ( Data, ID,  _CosStringValue ( CosObject ), _CosStringSize ( CosObject ) );
			} else {
				sz = _CosStringSize( CosObject );
				kap = sz & 0xf;
				NewStringSize = sz + 16 + ( kap = kap ? 16- kap:16 );
				_CosStringValue( CosObject ) = (char *)mrealloc ( _LIB, _CosStringValue( CosObject ), NewStringSize );
				_CosStringSize( CosObject ) = NewStringSize;
				memset( &(_CosStringValue( CosObject )[sz+16]), kap, kap );
				memmove( &( _CosStringValue( CosObject )[16]), _CosStringValue( CosObject ), sz );
				ULCryptWithAES ( (PPDFLib) _CosLib ( CosObject ), Data, ID,(ppUns8 *) _CosStringValue( CosObject ), NewStringSize, true );
			}
		}
	} else {
		if ( _IsCosStream( CosObject ) ){
			w = _CosDictValueByName ( CosObject, Type);
			if (_IsCosName ( w ) && _CosNameValue ( w ) == GetAtomDoc ( Metadata ) && !_DOC->CryptoInfo.EncryptMetadata )
				return;
			WStream = _CosStreamValue( CosObject );
			if ( ! CryptFilterCheck( CosObject, &Dec)){
				switch ( _DOC->CryptoInfo.DefStreamAlg ){
					case eaRC4:
						ULCryptData ( Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream )  );
						break;
					case eaAES:
						sz = _StreamSize( WStream );
						kap = sz & 0xf;
						NewStringSize = sz + 16 + ( kap = kap ? 16-kap:16 );
						if ( _StreamCapacity( WStream) < NewStringSize )
							ULStrmSetCapacity (WStream, NewStringSize);
						memset( &(_StreamBuffer(WStream)[sz+16]), kap, kap );
						memmove( &( _StreamBuffer(WStream)[16]), _StreamBuffer(WStream), sz );
						_StreamSize( WStream ) = NewStringSize;
						_StreamPosition ( WStream ) = 0;
						ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ),Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream ), true );
						CosDictAppend ( CosObject, GetAtomLib( _StreamLib( WStream ),Length ),
							CosIntNew( _CosDoc ( CosObject ), false, NewStringSize ));
						break;
					default:;
				}
			} else {
				if ( Dec == GetAtomDoc(Identity))
					return;
				for ( i = 0; i < _DOC->CryptoInfo.AlgListCount; i++){
					if (_DOC->CryptoInfo.AlgList[i].Name == Dec ){
						switch ( _DOC->CryptoInfo.AlgList[i].Algorithm ){
							case eaRC4:
								ULCryptData ( Data, ID, _StreamBuffer( _CosStreamValue( CosObject ) ), _StreamSize( _CosStreamValue( CosObject ) )  );
								break;
							case eaAES:
								sz = _StreamSize( WStream );
								kap = sz & 0xf;
								NewStringSize = sz + 16 + ( kap = kap ? 16 - kap:16 );
								if ( _StreamCapacity( WStream) < NewStringSize )
									ULStrmSetCapacity (WStream, NewStringSize);
								memset( &(_StreamBuffer(WStream)[sz+16]), kap, kap );
								memmove( &( _StreamBuffer(WStream)[16]), _StreamBuffer(WStream), sz );
								_StreamSize( WStream ) = NewStringSize;
								_StreamPosition ( WStream ) = 0;
								ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream ), true );
								CosDictAppend ( CosObject, GetAtomLib( _StreamLib( WStream ),Length ),
									CosIntNew( _CosDoc ( CosObject ), false, NewStringSize ));
								break;
							default:;
						}
						break;
					}
				}
			}
		} else {
			switch ( _DOC->CryptoInfo.DefStringAlg ){
				case eaRC4:
					ULCryptData ( Data, ID,  _CosStringValue ( CosObject ), _CosStringSize ( CosObject ) );
					break;
				case eaAES:
					sz = _CosStringSize( CosObject );
					kap = sz & 0xf;
					NewStringSize = sz + 16 + ( kap = kap ? 16-kap:16 );
					_CosStringValue( CosObject ) = (char *) mrealloc ( _LIB, _CosStringValue( CosObject ), NewStringSize );
					_CosStringSize( CosObject ) = NewStringSize;
					memset( &(_CosStringValue( CosObject )[sz+16]), kap, kap );
					memmove( &( _CosStringValue( CosObject )[16]), _CosStringValue( CosObject ), sz );
					ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), Data, ID,(ppUns8 *) _CosStringValue( CosObject ), NewStringSize, true );
					break;
				default:;
			}
		}
	}
}

void ULDecryptObjectR4orR5( PDFCryptoHandle Data, PDFCosHandle CosObject, PDFID ID)
{
	ppAtom Dec, Identity;
	PDFCosHandle w;
	ppUns32 i;
	PDFStreamHandle WStream;
	ppUns8 * NewString;
	ppUns32 NewStringSize;
	PDFDocHandle Doc = _CosDoc ( CosObject );
	Identity = GetAtomDoc(Identity);
	if ( _IsCosStream( CosObject ) ){
		w = _CosDictValueByName ( CosObject, Type);
		if (_IsCosName ( w ) && _CosNameValue ( w ) == GetAtomDoc ( Metadata ) && !_DOC->CryptoInfo.EncryptMetadata )
			return;
		WStream = _CosStreamValue( CosObject );
		if ( ! CryptFilterCheck( CosObject, &Dec)){
			switch ( _DOC->CryptoInfo.DefStreamAlg ){
				case eaRC4:
					ULCryptData ( Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream )  );
					break;
				case eaAES:
					ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), (TPDFCryptoType * )Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream ), false );
					NewString = _StreamBuffer( WStream );
					NewStringSize = _StreamSize( WStream ) - 16 - NewString[_StreamSize( WStream )- 1];
					memmove ( _StreamBuffer( WStream ), &(_StreamBuffer( WStream )[16]),NewStringSize );
					_StreamSize( WStream ) = NewStringSize;
					_StreamPosition ( WStream ) = 0;
					CosDictAppend ( CosObject, GetAtomLib( _StreamLib( WStream ),Length ),
						CosIntNew( _CosDoc ( CosObject ), false, NewStringSize ));
					break;
				default:;
			}
		} else {
			if ( Dec != Identity){
				for ( i = 0; i < _DOC->CryptoInfo.AlgListCount; i++){
					if (_DOC->CryptoInfo.AlgList[i].Name == Dec ){
						switch ( _DOC->CryptoInfo.AlgList[i].Algorithm ){
							case eaRC4:
								ULCryptData ( Data, ID, _StreamBuffer( _CosStreamValue( CosObject ) ), _StreamSize( _CosStreamValue( CosObject ) )  );
								break;
							case eaAES:
								ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), (TPDFCryptoType * )Data, ID, _StreamBuffer( WStream ), _StreamSize( WStream ), false );
								NewString = _StreamBuffer( WStream );
								NewStringSize = _StreamSize( WStream ) - 16 - NewString[_StreamSize( WStream )- 1];
								memmove ( _StreamBuffer( WStream ), &(_StreamBuffer( WStream )[16]),NewStringSize );
								_StreamSize( WStream ) = NewStringSize;
								_StreamPosition ( WStream ) = 0;
								CosDictAppend ( CosObject, GetAtomLib( _StreamLib( WStream ),Length ),
									CosIntNew( _CosDoc ( CosObject ), false, NewStringSize ));									
								break;
							default:;
						}
						break;
					}
				}
			}
		}
	} else {
		switch ( _DOC->CryptoInfo.DefStringAlg ){
				case eaRC4:
					ULCryptData ( Data, ID,  _CosStringValue ( CosObject ), _CosStringSize ( CosObject ) );
					break;
				case eaAES:
					ULCryptWithAES ( (PPDFLib)_CosLib ( CosObject ), (TPDFCryptoType * )Data, ID, (ppUns8 *)_CosStringValue( CosObject ), _CosStringSize( CosObject ), false );
					NewString = (ppUns8 *)_CosStringValue( CosObject );
					NewStringSize = _CosStringSize( CosObject ) - 16 - NewString[_CosStringSize( CosObject ) - 1];
					memmove ( _CosStringValue( CosObject ), &(_CosStringValue( CosObject )[16]),NewStringSize );
					_CosStringValue( CosObject ) = (char *)mrealloc ( _LIB, _CosStringValue( CosObject ), NewStringSize );
					_CosStringSize( CosObject ) = NewStringSize;
					break;
				default:;
		}
	}

}



void CosCryptObject(PDFCosHandle co, PDFCryptoHandle Data, PDFID ID, ppBool IsEncrypt)
{
    ppUns32 i;
    switch ( _CosGetType( co ) ){
        case CosString:
			if ( CR(Data)->Revision < 4){
				ULCryptData ( Data, ID,  _CosStringValue ( co ), _CosStringSize ( co ) );
			} else {
				if (IsEncrypt )
					ULEncryptObjectR4orR5 ( (TPDFCryptoType * )Data, co, ID );
				else
					ULDecryptObjectR4orR5( (TPDFCryptoType * )Data, co, ID );
			}
            break;
        case CosArray:
            for ( i = 0; i < _CosArrayCount ( co ); i++ )
                CosCryptObject ( _CosArrayItem ( co ,i), Data, ID, IsEncrypt );
            break;
        case CosDict:
            for ( i = 0; i < _CosDictCount( co ); i++ )
                CosCryptObject ( _CosDictValue (co , i ), Data, ID, IsEncrypt );
            break;
        case CosStream:
            CosCryptObject ( _CosStreamAttr ( co ), Data, ID, IsEncrypt );
			if ( CR(Data)->Revision < 4 ){
				ULCryptData ( Data, ID, _StreamBuffer( _CosStreamValue( co ) ), _StreamSize( _CosStreamValue( co ) )  );
			} else {
				if (IsEncrypt )
					ULEncryptObjectR4orR5 ( (TPDFCryptoType * )Data, co, ID );
				else
					ULDecryptObjectR4orR5( (TPDFCryptoType * )Data, co, ID );
			}
            break;
        default:
            return;
    }
}

