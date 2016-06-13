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
VSAes.h
*********************************************************************/

#ifndef VSAes_H
#define VSAes_H
#ifndef __GNUC__ /* suppress gcc warning */ 
#pragma once
#endif /* __GNUC__ */


#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "../VSTypes.h"
#include "VSLibI.h"

    typedef struct SHA256Context {
		ppUns32 total[2];
		ppUns32 state[8];
		ppUns8 buffer[64];
	} SHA256Context;

	typedef struct
	{
		ppUns32 erk[64];     /* encryption round keys */
		ppUns32 drk[64];     /* decryption round keys */
		ppUns32 nr;           /* number of rounds      */
	}
	aes_context;

    
    void ULSHA256Init (SHA256Context *sc);
	void ULSHA256Update(SHA256Context *ctx, ppUns8 *input, ppUns32 length );
	void ULSHA256Final (SHA256Context *sc, ppUns8 hash[32]);

    
	/*
	* AES key schedule (keysize can be 128, 192 or 256)
	*/
	void ULaes_set_key( PPDFLib Lib, aes_context *ctx, ppUns8 *key, ppUns32 keysize );

	/*
	* AES 128-bit block encryption (ECB)
	*/
	void ULaes_encrypt( aes_context *ctx, ppUns8 input[16], ppUns8 output[16] );

	/*
	* AES 128-bit block decryption (ECB)
	*/
	void ULaes_decrypt( aes_context *ctx, ppUns8 input[16], ppUns8 output[16] );

	/*
	* AES-CBC encryption
	*/
	void ULaes_cbc_encrypt( aes_context *ctx, ppUns8 iv[16],
		ppUns8 *input, ppUns8 *output, ppUns32 len );

	/*
	* AES-CBC decryption
	*/
	void ULaes_cbc_decrypt( aes_context *ctx, ppUns8 iv[16],
		ppUns8 *input, ppUns8 *output, ppUns32 len );



#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSAes_H */
