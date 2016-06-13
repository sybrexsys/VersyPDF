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
VSCrypt.h
*********************************************************************/

#ifndef VSCrypt_H
#define VSCrypt_H
#ifndef __GNUC__ /* suppress gcc warning */ 
#pragma once
#endif /* __GNUC__ */

#include "VSLibI.h"
#include "VSCosAI.h"
#include "VSDocAI.h"


#ifdef __cplusplus
extern "C"{
#endif

    void ULCryptData( PDFCryptoHandle Data, PDFID ID, void *Buffer, ppInt32 Size );
    PDFCosHandle ULCreateFileID(PDFDocHandle Doc, char *instr);

    void CosCryptObject(PDFCosHandle co, PDFCryptoHandle Data, PDFID ID, ppBool IsEncrypt);
    void ULStrmCrypt(PDFDocHandle Doc, PDFStreamHandle Strm, PDFCosHandle StreamAttr, PDFCryptoHandle Data, PDFID ID);
    void ULCreateKey( PDFCryptoHandle Data, char *Owner, char *User);

    typedef struct ULrc4_key {
        ppUns8  state[256];
        ppUns8  x;
        ppUns8  y;
    } ULrc4_key;

    typedef struct { 
        ppUns32 i[2];                 
        ppUns32 buf[4];               
        ppUns8  in[64];
    } ULMD5_CTX;

    typedef ppUns8 ULTMD5Digest[16];



	typedef enum _t_TPDFCheckPasword  {cpValid, cpValidOwner,cpValidUser, cpInvalid, cpCorrupted } TPDFCheckPasword;
	
	TPDFCheckPasword ULCheckPassword(PDFCryptoHandle Data, char *Password,ppInt32 pl);
    void ULprepare_key(unsigned char *key_data_ptr, int key_data_len, ULrc4_key *key);
    void ULrc4(ppUns8 *s, ppUns8 *d, ppInt32 buffer_len, ULrc4_key *key);
    void ULMD5Init(ULMD5_CTX *mdContext);
    void ULMD5Update(ULMD5_CTX *mdContext, ppUns8 *inBuf, ppInt32 inLen);
    void ULMD5Final(ULMD5_CTX *mdContext, ULTMD5Digest *Dig);
    void ULxCalcMD5(ppUns8 *data, ppInt32 len, ULTMD5Digest *dig);



    void ULInitCryptoData(PDFDocHandle Doc, PDFCryptoHandle Data, ppInt32 Version,
                          ppInt32 Revision, ppInt32 KeyLength, PDFCosHandle FileID);
    void ULPadTrunc(char *s, ppInt32 sl, char *d);
    TPDFCheckPasword ULCheckUserPassword(PDFCryptoHandle Data, char *Password, ppInt32 pl);
    TPDFCheckPasword ULCheckOwnerPassword(PDFCryptoHandle Data, char *Password, ppInt32 pl);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSCrypt_H */
