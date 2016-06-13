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
VSkPKCS12.c
*********************************************************************/

#include "VSkPKCS12I.h"
#include <string.h>

#ifndef NOT_USE_SIGN

/*ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))


const DES_LONG des_skb[8][64]={
    {
    /*for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
    0x00000000L,0x00000010L,0x20000000L,0x20000010L,
    0x00010000L,0x00010010L,0x20010000L,0x20010010L,
    0x00000800L,0x00000810L,0x20000800L,0x20000810L,
    0x00010800L,0x00010810L,0x20010800L,0x20010810L,
    0x00000020L,0x00000030L,0x20000020L,0x20000030L,
    0x00010020L,0x00010030L,0x20010020L,0x20010030L,
    0x00000820L,0x00000830L,0x20000820L,0x20000830L,
    0x00010820L,0x00010830L,0x20010820L,0x20010830L,
    0x00080000L,0x00080010L,0x20080000L,0x20080010L,
    0x00090000L,0x00090010L,0x20090000L,0x20090010L,
    0x00080800L,0x00080810L,0x20080800L,0x20080810L,
    0x00090800L,0x00090810L,0x20090800L,0x20090810L,
    0x00080020L,0x00080030L,0x20080020L,0x20080030L,
    0x00090020L,0x00090030L,0x20090020L,0x20090030L,
    0x00080820L,0x00080830L,0x20080820L,0x20080830L,
    0x00090820L,0x00090830L,0x20090820L,0x20090830L,
    },{
    /*for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
    0x00000000L,0x02000000L,0x00002000L,0x02002000L,
    0x00200000L,0x02200000L,0x00202000L,0x02202000L,
    0x00000004L,0x02000004L,0x00002004L,0x02002004L,
    0x00200004L,0x02200004L,0x00202004L,0x02202004L,
    0x00000400L,0x02000400L,0x00002400L,0x02002400L,
    0x00200400L,0x02200400L,0x00202400L,0x02202400L,
    0x00000404L,0x02000404L,0x00002404L,0x02002404L,
    0x00200404L,0x02200404L,0x00202404L,0x02202404L,
    0x10000000L,0x12000000L,0x10002000L,0x12002000L,
    0x10200000L,0x12200000L,0x10202000L,0x12202000L,
    0x10000004L,0x12000004L,0x10002004L,0x12002004L,
    0x10200004L,0x12200004L,0x10202004L,0x12202004L,
    0x10000400L,0x12000400L,0x10002400L,0x12002400L,
    0x10200400L,0x12200400L,0x10202400L,0x12202400L,
    0x10000404L,0x12000404L,0x10002404L,0x12002404L,
    0x10200404L,0x12200404L,0x10202404L,0x12202404L,
    },{
    /*for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
    0x00000000L,0x00000001L,0x00040000L,0x00040001L,
    0x01000000L,0x01000001L,0x01040000L,0x01040001L,
    0x00000002L,0x00000003L,0x00040002L,0x00040003L,
    0x01000002L,0x01000003L,0x01040002L,0x01040003L,
    0x00000200L,0x00000201L,0x00040200L,0x00040201L,
    0x01000200L,0x01000201L,0x01040200L,0x01040201L,
    0x00000202L,0x00000203L,0x00040202L,0x00040203L,
    0x01000202L,0x01000203L,0x01040202L,0x01040203L,
    0x08000000L,0x08000001L,0x08040000L,0x08040001L,
    0x09000000L,0x09000001L,0x09040000L,0x09040001L,
    0x08000002L,0x08000003L,0x08040002L,0x08040003L,
    0x09000002L,0x09000003L,0x09040002L,0x09040003L,
    0x08000200L,0x08000201L,0x08040200L,0x08040201L,
    0x09000200L,0x09000201L,0x09040200L,0x09040201L,
    0x08000202L,0x08000203L,0x08040202L,0x08040203L,
    0x09000202L,0x09000203L,0x09040202L,0x09040203L,
    },{
    /*for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
    0x00000000L,0x00100000L,0x00000100L,0x00100100L,
    0x00000008L,0x00100008L,0x00000108L,0x00100108L,
    0x00001000L,0x00101000L,0x00001100L,0x00101100L,
    0x00001008L,0x00101008L,0x00001108L,0x00101108L,
    0x04000000L,0x04100000L,0x04000100L,0x04100100L,
    0x04000008L,0x04100008L,0x04000108L,0x04100108L,
    0x04001000L,0x04101000L,0x04001100L,0x04101100L,
    0x04001008L,0x04101008L,0x04001108L,0x04101108L,
    0x00020000L,0x00120000L,0x00020100L,0x00120100L,
    0x00020008L,0x00120008L,0x00020108L,0x00120108L,
    0x00021000L,0x00121000L,0x00021100L,0x00121100L,
    0x00021008L,0x00121008L,0x00021108L,0x00121108L,
    0x04020000L,0x04120000L,0x04020100L,0x04120100L,
    0x04020008L,0x04120008L,0x04020108L,0x04120108L,
    0x04021000L,0x04121000L,0x04021100L,0x04121100L,
    0x04021008L,0x04121008L,0x04021108L,0x04121108L,
    },{
    /*for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
    0x00000000L,0x10000000L,0x00010000L,0x10010000L,
    0x00000004L,0x10000004L,0x00010004L,0x10010004L,
    0x20000000L,0x30000000L,0x20010000L,0x30010000L,
    0x20000004L,0x30000004L,0x20010004L,0x30010004L,
    0x00100000L,0x10100000L,0x00110000L,0x10110000L,
    0x00100004L,0x10100004L,0x00110004L,0x10110004L,
    0x20100000L,0x30100000L,0x20110000L,0x30110000L,
    0x20100004L,0x30100004L,0x20110004L,0x30110004L,
    0x00001000L,0x10001000L,0x00011000L,0x10011000L,
    0x00001004L,0x10001004L,0x00011004L,0x10011004L,
    0x20001000L,0x30001000L,0x20011000L,0x30011000L,
    0x20001004L,0x30001004L,0x20011004L,0x30011004L,
    0x00101000L,0x10101000L,0x00111000L,0x10111000L,
    0x00101004L,0x10101004L,0x00111004L,0x10111004L,
    0x20101000L,0x30101000L,0x20111000L,0x30111000L,
    0x20101004L,0x30101004L,0x20111004L,0x30111004L,
    },{
    /*for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
    0x00000000L,0x08000000L,0x00000008L,0x08000008L,
    0x00000400L,0x08000400L,0x00000408L,0x08000408L,
    0x00020000L,0x08020000L,0x00020008L,0x08020008L,
    0x00020400L,0x08020400L,0x00020408L,0x08020408L,
    0x00000001L,0x08000001L,0x00000009L,0x08000009L,
    0x00000401L,0x08000401L,0x00000409L,0x08000409L,
    0x00020001L,0x08020001L,0x00020009L,0x08020009L,
    0x00020401L,0x08020401L,0x00020409L,0x08020409L,
    0x02000000L,0x0A000000L,0x02000008L,0x0A000008L,
    0x02000400L,0x0A000400L,0x02000408L,0x0A000408L,
    0x02020000L,0x0A020000L,0x02020008L,0x0A020008L,
    0x02020400L,0x0A020400L,0x02020408L,0x0A020408L,
    0x02000001L,0x0A000001L,0x02000009L,0x0A000009L,
    0x02000401L,0x0A000401L,0x02000409L,0x0A000409L,
    0x02020001L,0x0A020001L,0x02020009L,0x0A020009L,
    0x02020401L,0x0A020401L,0x02020409L,0x0A020409L,
    },{
    /*for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
    0x00000000L,0x00000100L,0x00080000L,0x00080100L,
    0x01000000L,0x01000100L,0x01080000L,0x01080100L,
    0x00000010L,0x00000110L,0x00080010L,0x00080110L,
    0x01000010L,0x01000110L,0x01080010L,0x01080110L,
    0x00200000L,0x00200100L,0x00280000L,0x00280100L,
    0x01200000L,0x01200100L,0x01280000L,0x01280100L,
    0x00200010L,0x00200110L,0x00280010L,0x00280110L,
    0x01200010L,0x01200110L,0x01280010L,0x01280110L,
    0x00000200L,0x00000300L,0x00080200L,0x00080300L,
    0x01000200L,0x01000300L,0x01080200L,0x01080300L,
    0x00000210L,0x00000310L,0x00080210L,0x00080310L,
    0x01000210L,0x01000310L,0x01080210L,0x01080310L,
    0x00200200L,0x00200300L,0x00280200L,0x00280300L,
    0x01200200L,0x01200300L,0x01280200L,0x01280300L,
    0x00200210L,0x00200310L,0x00280210L,0x00280310L,
    0x01200210L,0x01200310L,0x01280210L,0x01280310L,
    },{
    /*for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
    0x00000000L,0x04000000L,0x00040000L,0x04040000L,
    0x00000002L,0x04000002L,0x00040002L,0x04040002L,
    0x00002000L,0x04002000L,0x00042000L,0x04042000L,
    0x00002002L,0x04002002L,0x00042002L,0x04042002L,
    0x00000020L,0x04000020L,0x00040020L,0x04040020L,
    0x00000022L,0x04000022L,0x00040022L,0x04040022L,
    0x00002020L,0x04002020L,0x00042020L,0x04042020L,
    0x00002022L,0x04002022L,0x00042022L,0x04042022L,
    0x00000800L,0x04000800L,0x00040800L,0x04040800L,
    0x00000802L,0x04000802L,0x00040802L,0x04040802L,
    0x00002800L,0x04002800L,0x00042800L,0x04042800L,
    0x00002802L,0x04002802L,0x00042802L,0x04042802L,
    0x00000820L,0x04000820L,0x00040820L,0x04040820L,
    0x00000822L,0x04000822L,0x00040822L,0x04040822L,
    0x00002820L,0x04002820L,0x00042820L,0x04042820L,
    0x00002822L,0x04002822L,0x00042822L,0x04042822L,
    }};

const DES_LONG des_SPtrans[8][64]={
{
/*nibble 0 */
0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
},{
/*nibble 1 */
0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
},{
/*nibble 2 */
0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
},{
/*nibble 3 */
0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
},{
/*nibble 4 */
0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
},{
/*nibble 5 */
0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
},{
/*nibble 6 */
0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
},{
/*nibble 7 */
0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
}};

static ppUns8 key_table[256]={
    0xd9,0x78,0xf9,0xc4,0x19,0xdd,0xb5,0xed,0x28,0xe9,0xfd,0x79,
        0x4a,0xa0,0xd8,0x9d,0xc6,0x7e,0x37,0x83,0x2b,0x76,0x53,0x8e,
        0x62,0x4c,0x64,0x88,0x44,0x8b,0xfb,0xa2,0x17,0x9a,0x59,0xf5,
        0x87,0xb3,0x4f,0x13,0x61,0x45,0x6d,0x8d,0x09,0x81,0x7d,0x32,
        0xbd,0x8f,0x40,0xeb,0x86,0xb7,0x7b,0x0b,0xf0,0x95,0x21,0x22,
        0x5c,0x6b,0x4e,0x82,0x54,0xd6,0x65,0x93,0xce,0x60,0xb2,0x1c,
        0x73,0x56,0xc0,0x14,0xa7,0x8c,0xf1,0xdc,0x12,0x75,0xca,0x1f,
        0x3b,0xbe,0xe4,0xd1,0x42,0x3d,0xd4,0x30,0xa3,0x3c,0xb6,0x26,
        0x6f,0xbf,0x0e,0xda,0x46,0x69,0x07,0x57,0x27,0xf2,0x1d,0x9b,
        0xbc,0x94,0x43,0x03,0xf8,0x11,0xc7,0xf6,0x90,0xef,0x3e,0xe7,
        0x06,0xc3,0xd5,0x2f,0xc8,0x66,0x1e,0xd7,0x08,0xe8,0xea,0xde,
        0x80,0x52,0xee,0xf7,0x84,0xaa,0x72,0xac,0x35,0x4d,0x6a,0x2a,
        0x96,0x1a,0xd2,0x71,0x5a,0x15,0x49,0x74,0x4b,0x9f,0xd0,0x5e,
        0x04,0x18,0xa4,0xec,0xc2,0xe0,0x41,0x6e,0x0f,0x51,0xcb,0xcc,
        0x24,0x91,0xaf,0x50,0xa1,0xf4,0x70,0x39,0x99,0x7c,0x3a,0x85,
        0x23,0xb8,0xb4,0x7a,0xfc,0x02,0x36,0x5b,0x25,0x55,0x97,0x31,
        0x2d,0x5d,0xfa,0x98,0xe3,0x8a,0x92,0xae,0x05,0xdf,0x29,0x10,
        0x67,0x6c,0xba,0xc9,0xd3,0x00,0xe6,0xcf,0xe1,0x9e,0xa8,0x2c,
        0x63,0x16,0x01,0x3f,0x58,0xe2,0x89,0xa9,0x0d,0x38,0x34,0x1b,
        0xab,0x33,0xff,0xb0,0xbb,0x48,0x0c,0x5f,0xb9,0xb1,0xcd,0x2e,
        0xc5,0xf3,0xdb,0x47,0xe5,0xa5,0x9c,0x77,0x0a,0xa6,0x20,0x68,
        0xfe,0x7f,0xc1,0xad,
};

void des_set_key_unchecked(const_des_cblock *key, des_key_schedule schedule)
{
    static ppInt32 shifts2[16]={0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};
    register DES_LONG c,d,t,s,t2;
    register const ppUns8 *in;
    register DES_LONG *k;
    register ppInt32 i;

    k = &schedule->ks.deslong[0];
    in = &(*key)[0];

    c2l(in,c);
    d =((DES_LONG)(*((in)++))), \
    d|=((DES_LONG)(*((in)++)))<< 8L, \
    d|=((DES_LONG)(*((in)++)))<<16L, \
    d|=((DES_LONG)(*in))<<24L;

    /*do PC1 in 47 simple operations :-) */
    PERM_OP (d,c,t,4,0x0f0f0f0fL);
    HPERM_OP(c,t,-2,0xcccc0000L);
    HPERM_OP(d,t,-2,0xcccc0000L);
    PERM_OP (d,c,t,1,0x55555555L);
    PERM_OP (c,d,t,8,0x00ff00ffL);
    PERM_OP (d,c,t,1,0x55555555L);
    d=  (((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
         ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
    c&=0x0fffffffL;

    for (i=0; i<ITERATIONS; i++)
    {
        if (shifts2[i])
            { c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
        else
            { c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
        c&=0x0fffffffL;
        d&=0x0fffffffL;
        /*could be a few less shifts but I am to lazy at this
         * point in time to investigate */
        s=  des_skb[0][ (c    )&0x3f                ]|
            des_skb[1][((c>> 6L)&0x03)|((c>> 7L)&0x3c)]|
            des_skb[2][((c>>13L)&0x0f)|((c>>14L)&0x30)]|
            des_skb[3][((c>>20L)&0x01)|((c>>21L)&0x06) |
                          ((c>>22L)&0x38)];
        t=  des_skb[4][ (d    )&0x3f                ]|
            des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
            des_skb[6][ (d>>15L)&0x3f                ]|
            des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

        /*table contained 0213 4657 */
        t2=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
        *(k++)=ROTATE(t2,30)&0xffffffffL;

        t2=((s>>16L)|(t&0xffff0000L));
        *(k++)=ROTATE(t2,26)&0xffffffffL;
    }
}



void des_encrypt2(DES_LONG *data, des_key_schedule ks, ppInt32 enc)
    {
    register DES_LONG l,r,t,u;
#ifdef DES_PTR
    register const ppUns8 *des_SP=(const ppUns8 * )des_SPtrans;
#endif
#ifndef DES_UNROLL
    register ppInt32 i;
#endif
    register DES_LONG *s;

    r=data[0];
    l=data[1];

    /*Things have been modified so that the initial rotate is
     * done outside the loop.  This required the
     * des_SPtrans values in sp.h to be rotated 1 bit to the right.
     * One perl script later and things have a 5% speed up on a sparc2.
     * clear the top bits on machines with 8byte longs */
    r=ROTATE(r,29)&0xffffffffL;
    l=ROTATE(l,29)&0xffffffffL;

    s=ks->ks.deslong;
    /*I don't know if it is worth the effort of loop unrolling the
     * inner loop */
    if (enc)
        {
#ifdef DES_UNROLL
        D_ENCRYPT(l,r, 0); /* 1 */
        D_ENCRYPT(r,l, 2); /* 2 */
        D_ENCRYPT(l,r, 4); /* 3 */
        D_ENCRYPT(r,l, 6); /* 4 */
        D_ENCRYPT(l,r, 8); /* 5 */
        D_ENCRYPT(r,l,10); /* 6 */
        D_ENCRYPT(l,r,12); /* 7 */
        D_ENCRYPT(r,l,14); /* 8 */
        D_ENCRYPT(l,r,16); /* 9 */
        D_ENCRYPT(r,l,18); /* 10 */
        D_ENCRYPT(l,r,20); /* 11 */
        D_ENCRYPT(r,l,22); /* 12 */
        D_ENCRYPT(l,r,24); /* 13 */
        D_ENCRYPT(r,l,26); /* 14 */
        D_ENCRYPT(l,r,28); /* 15 */
        D_ENCRYPT(r,l,30); /* 16 */
#else
        for (i=0; i<32; i+=8)
            {
            D_ENCRYPT(l,r,i+0); /* 1 */
            D_ENCRYPT(r,l,i+2); /* 2 */
            D_ENCRYPT(l,r,i+4); /* 3 */
            D_ENCRYPT(r,l,i+6); /* 4 */
            }
#endif
        }
    else
        {
#ifdef DES_UNROLL
        D_ENCRYPT(l,r,30); /*16 */
        D_ENCRYPT(r,l,28); /*15 */
        D_ENCRYPT(l,r,26); /*14 */
        D_ENCRYPT(r,l,24); /*13 */
        D_ENCRYPT(l,r,22); /*12 */
        D_ENCRYPT(r,l,20); /*11 */
        D_ENCRYPT(l,r,18); /*10 */
        D_ENCRYPT(r,l,16); /* 9 */
        D_ENCRYPT(l,r,14); /* 8 */
        D_ENCRYPT(r,l,12); /* 7 */
        D_ENCRYPT(l,r,10); /* 6 */
        D_ENCRYPT(r,l, 8); /* 5 */
        D_ENCRYPT(l,r, 6); /* 4 */
        D_ENCRYPT(r,l, 4); /* 3 */
        D_ENCRYPT(l,r, 2); /* 2 */
        D_ENCRYPT(r,l, 0); /* 1 */
#else
        for (i=30; i>0; i-=8)
            {
            D_ENCRYPT(l,r,i-0); /*16 */
            D_ENCRYPT(r,l,i-2); /*15 */
            D_ENCRYPT(l,r,i-4); /*14 */
            D_ENCRYPT(r,l,i-6); /*13 */
            }
#endif
        }
    /*rotate and clear the top bits on machines with 8byte longs */
    data[0]=ROTATE(l,3)&0xffffffffL;
    data[1]=ROTATE(r,3)&0xffffffffL;
    /* l=r=t=u=0; */
    }

void des_encrypt3(DES_LONG *data, des_key_schedule ks1, des_key_schedule ks2,
         des_key_schedule ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG * )data,ks1,DES_ENCRYPT);
    des_encrypt2((DES_LONG * )data,ks2,DES_DECRYPT);
    des_encrypt2((DES_LONG * )data,ks3,DES_ENCRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
    }

void des_decrypt3(DES_LONG *data, des_key_schedule ks1, des_key_schedule ks2,
         des_key_schedule ks3)
    {
    register DES_LONG l,r;

    l=data[0];
    r=data[1];
    IP(l,r);
    data[0]=l;
    data[1]=r;
    des_encrypt2((DES_LONG * )data,ks3,DES_DECRYPT);
    des_encrypt2((DES_LONG * )data,ks2,DES_ENCRYPT);
    des_encrypt2((DES_LONG * )data,ks1,DES_DECRYPT);
    l=data[0];
    r=data[1];
    FP(r,l);
    data[0]=l;
    data[1]=r;
    }

void des_ede3_cbc_encrypt(const ppUns8 *input, ppUns8 *output,
         ppInt64 length, des_key_schedule ks1, des_key_schedule ks2,
         des_key_schedule ks3, des_cblock *ivec, ppInt32 enc)
    {
    register DES_LONG tin0,tin1;
    register DES_LONG tout0,tout1,xor0,xor1;
    register const ppUns8 *in;
    ppUns8 *out;
    register ppInt64 l=length;
    DES_LONG tin[2];
    ppUns8 *iv;

    in=input;
    out=output;
    iv = &(*ivec)[0];

    if (enc)
        {
        c2l(iv,tout0);
        tout1 =((DES_LONG)(*((iv)++))), \
        tout1|=((DES_LONG)(*((iv)++)))<< 8L, \
        tout1|=((DES_LONG)(*((iv)++)))<<16L, \
        tout1|=((DES_LONG)(*((iv))))<<24L;

        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            des_encrypt3((DES_LONG * )tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            l2c(tout1,out);
            }
        if (l != -8)
            {
            c2ln(in,tin0,tin1,(ppUns8)l+8);
            tin0^=tout0;
            tin1^=tout1;

            tin[0]=tin0;
            tin[1]=tin1;
            des_encrypt3((DES_LONG * )tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            l2c(tout0,out);
            *(out++)=(ppUns8)((tout1     )&0xff), \
            *(out++)=(ppUns8)((tout1>> 8L)&0xff), \
            *(out++)=(ppUns8)((tout1>>16L)&0xff), \
            *out=(ppUns8)((tout1>>24L)&0xff);

            }
        iv = &(*ivec)[0];
        l2c(tout0,iv);
        *((iv)++)=(ppUns8)(((tout1)     )&0xff), \
        *((iv)++)=(ppUns8)(((tout1)>> 8L)&0xff), \
        *((iv)++)=(ppUns8)(((tout1)>>16L)&0xff), \
        *iv=(ppUns8)(((tout1)>>24L)&0xff);

        }
    else
        {
        register DES_LONG t0,t1;

        c2l(iv,xor0);
        xor1 =((DES_LONG)(*((iv)++))), \
        xor1|=((DES_LONG)(*((iv)++)))<< 8L, \
        xor1|=((DES_LONG)(*((iv)++)))<<16L, \
        xor1|=((DES_LONG)(*(iv)))<<24L;

        for (l-=8; l>=0; l-=8)
            {
            c2l(in,tin0);
            c2l(in,tin1);

            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            des_decrypt3((DES_LONG * )tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];

            tout0^=xor0;
            tout1^=xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=t0;
            xor1=t1;
            }
        if (l != -8)
            {
            c2l(in,tin0);
            tin1 =((DES_LONG)(*((in)++))), \
            tin1|=((DES_LONG)(*((in)++)))<< 8L, \
            tin1|=((DES_LONG)(*((in)++)))<<16L, \
            tin1|=((DES_LONG)(*in))<<24L;

            
            t0=tin0;
            t1=tin1;

            tin[0]=tin0;
            tin[1]=tin1;
            des_decrypt3((DES_LONG * )tin,ks1,ks2,ks3);
            tout0=tin[0];
            tout1=tin[1];
        
            tout0^=xor0;
            tout1^=xor1;
            l2cn(tout0,tout1,out,(ppUns8)l+8);
            xor0=t0;
            xor1=t1;
            }

        iv = &(*ivec)[0];
        l2c(xor0,iv);
        *(iv++)=(ppUns8)((xor1     )&0xff), \
        *(iv++)=(ppUns8)((xor1>> 8L)&0xff), \
        *(iv++)=(ppUns8)((xor1>>16L)&0xff), \
        *iv=(ppUns8)((xor1>>24L)&0xff);

        }
    }

/*NOTE - c is not incremented as per l2c */
#ifndef l2cn 
#define l2cn(l1,l2,c,n) \
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
#endif
/*It has come to my attention that there are 2 versions of the RC2
 *key schedule.  One which is normal, and anther which has a hook to
 *use a reduced key length.
 *BSAFE uses the 'retarded' version.  What I previously shipped is
 *the same as specifying 1024 for the 'bits' parameter.  Bsafe uses
 *a version where the bits parameter is the same as len*8 */
void RC2_set_key(RC2_KEY *key, ppInt32 len, const ppUns8 *data, ppInt32 bits)
{
    ppInt32 i,j;
    ppUns8 *k;
    RC2_INT *ki;
    ppUns32 c,d;

    k= (ppUns8 * )&(key->data[0]);
   * k=0; /*for if there is a zero length key */

    if (len > 128) len=128;
    if (bits <= 0) bits=1024;
    if (bits > 1024) bits=1024;

    for (i=0; i<len; i++)
        k[i]=data[i];

    /*expand table */
    d=k[len-1];
    j=0;
    for (i=len; i < 128; i++,j++)
    {
        d=key_table[(k[j]+d)&0xff];
        k[i]=(ppUns8)d;
    }

    /*hmm.... key reduction to 'bits' bits */

    j=(bits+7)>>3;
    i=128-j;
    c= (0xff>>(-bits & 0x07));

    d=key_table[k[i]&c];
    k[i]=(ppUns8)d;
    while (i--)
    {
        d=key_table[k[i+j]^d];
        k[i]=(ppUns8)d;
    }

    /*copy from bytes into RC2_INT's */

    ki= &(key->data[63]);
    for (i=127; i>=0; i-=2)
       * (ki--)=((k[i]<<8)|k[i-1])&0xffff;

}

void RC2_encrypt(ppUns32 *d, RC2_KEY *key)
{
    ppInt32 i,n;
    register RC2_INT *p0,*p1;
    register RC2_INT x0,x1,x2,x3,t;
    ppUns32 l;

    l=d[0];
    x0=(RC2_INT)l&0xffff;
    x1=(RC2_INT)(l>>16L);
    l=d[1];
    x2=(RC2_INT)l&0xffff;
    x3=(RC2_INT)(l>>16L);

    n=3;
    i=5;

    p0=p1= &(key->data[0]);
    for (;;)
    {
        t=(x0+(x1& ~x3)+(x2&x3)+ *(p0++))&0xffff;
        x0=(t<<1)|(t>>15);
        t=(x1+(x2& ~x0)+(x3&x0)+ *(p0++))&0xffff;
        x1=(t<<2)|(t>>14);
        t=(x2+(x3& ~x1)+(x0&x1)+ *(p0++))&0xffff;
        x2=(t<<3)|(t>>13);
        t=(x3+(x0& ~x2)+(x1&x2)+ *(p0++))&0xffff;
        x3=(t<<5)|(t>>11);

        if (--i == 0)
        {
            if (--n == 0) break;
            i=(n == 2)?6:5;

            x0+=p1[x3&0x3f];
            x1+=p1[x0&0x3f];
            x2+=p1[x1&0x3f];
            x3+=p1[x2&0x3f];
        }
    }

    d[0]=(ppUns32)(x0&0xffff)|((ppUns32)(x1&0xffff)<<16L);
    d[1]=(ppUns32)(x2&0xffff)|((ppUns32)(x3&0xffff)<<16L);
}

void RC2_decrypt(ppUns32 *d, RC2_KEY *key)
{
    ppInt32 i,n;
    register RC2_INT *p0,*p1;
    register RC2_INT x0,x1,x2,x3,t;
    ppUns32 l;

    l=d[0];
    x0=(RC2_INT)l&0xffff;
    x1=(RC2_INT)(l>>16L);
    l=d[1];
    x2=(RC2_INT)l&0xffff;
    x3=(RC2_INT)(l>>16L);

    n=3;
    i=5;

    p0= &(key->data[63]);
    p1= &(key->data[0]);
    for (;;)
    {
        t=((x3<<11)|(x3>>5))&0xffff;
        x3=(t-(x0& ~x2)-(x1&x2)- *(p0--))&0xffff;
        t=((x2<<13)|(x2>>3))&0xffff;
        x2=(t-(x3& ~x1)-(x0&x1)- *(p0--))&0xffff;
        t=((x1<<14)|(x1>>2))&0xffff;
        x1=(t-(x2& ~x0)-(x3&x0)- *(p0--))&0xffff;
        t=((x0<<15)|(x0>>1))&0xffff;
        x0=(t-(x1& ~x3)-(x2&x3)- *(p0--))&0xffff;

        if (--i == 0)
        {
            if (--n == 0) break;
            i=(n == 2)?6:5;

            x3=(x3-p1[x2&0x3f])&0xffff;
            x2=(x2-p1[x1&0x3f])&0xffff;
            x1=(x1-p1[x0&0x3f])&0xffff;
            x0=(x0-p1[x3&0x3f])&0xffff;
        }
    }

    d[0]=(ppUns32)(x0&0xffff)|((ppUns32)(x1&0xffff)<<16L);
    d[1]=(ppUns32)(x2&0xffff)|((ppUns32)(x3&0xffff)<<16L);
}

void RC2_cbc_encrypt(const ppUns8 *in, ppUns8 *out, ppInt64 length, RC2_KEY *ks, ppUns8 *iv, ppInt32 encrypt)
{
    register ppUns32 tin0,tin1;
    register ppUns32 tout0,tout1,xor0,xor1;
    register ppInt64 l=length;
    ppUns32 tin[2];

    if (encrypt)
    {
        c2l(iv,tout0);
        c2l(iv,tout1);
        iv-=8;
        for (l-=8; l>=0; l-=8)
        {
            c2l(in,tin0);
            c2l(in,tin1);
            tin0^=tout0;
            tin1^=tout1;
            tin[0]=tin0;
            tin[1]=tin1;
            RC2_encrypt(tin,ks);
            tout0=tin[0];
            l2c(tout0,out);
            tout1=tin[1];
            l2c(tout1,out);
        }
        if (l != -8)
        {
            c2ln(in,tin0,tin1,(ppUns8)l+8);
            tin0^=tout0;
            tin1^=tout1;
            tin[0]=tin0;
            tin[1]=tin1;
            RC2_encrypt(tin,ks);
            tout0=tin[0];
            l2c(tout0,out);
            tout1=tin[1];
            *(out++)=(ppUns8)((tout1     )&0xff), \
            *(out++)=(ppUns8)((tout1>> 8L)&0xff), \
            *(out++)=(ppUns8)((tout1>>16L)&0xff), \
            *out=(ppUns8)((tout1>>24L)&0xff);


        }
        l2c(tout0,iv);
        *(iv++)=(ppUns8)((tout1     )&0xff), \
        *(iv++)=(ppUns8)((tout1>> 8L)&0xff), \
        *(iv++)=(ppUns8)((tout1>>16L)&0xff), \
        *iv=(ppUns8)((tout1>>24L)&0xff);
    }
    else
    {
        c2l(iv,xor0);
        c2l(iv,xor1);
        iv-=8;
        for (l-=8; l>=0; l-=8)
        {
            c2l(in,tin0); tin[0]=tin0;
            c2l(in,tin1); tin[1]=tin1;
            RC2_decrypt(tin,ks);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2c(tout0,out);
            l2c(tout1,out);
            xor0=tin0;
            xor1=tin1;
        }
        if (l != -8)
        {
            c2l(in,tin0);
            tin[0]=tin0;
            tin1 =((DES_LONG)(*((in)++))), \
            tin1|=((DES_LONG)(*((in)++)))<< 8L, \
            tin1|=((DES_LONG)(*((in)++)))<<16L, \
            tin1|=((DES_LONG)(*(in)))<<24L;

            tin[1]=tin1;
            RC2_decrypt(tin,ks);
            tout0=tin[0]^xor0;
            tout1=tin[1]^xor1;
            l2cn(tout0,tout1,out,(ppUns8)l+8);
            xor0=tin0;
            xor1=tin1;
        }
        l2c(xor0,iv);
        *((iv)++)=(ppUns8)(((xor1)     )&0xff), \
        *((iv)++)=(ppUns8)(((xor1)>> 8L)&0xff), \
        *((iv)++)=(ppUns8)(((xor1)>>16L)&0xff), \
        *iv=(ppUns8)(((xor1)>>24L)&0xff);

    }
    /*
    tin0=tin1=tout0=tout1=xor0=xor1=0;
    tin[0]=tin[1]=0; */
}

ppInt32 cipher_PbeType(asn1_Object *type)
{
    ppInt32 i, rslt = ID_undef;
    if (type == NULL) return rslt;
    if (type->length != pkcs5_Length && 
        type->length != pkcs12_PbeLength) return rslt;
    if (type->length == pkcs5_Length)
    {
        if (memcmp(pkcs5_Begin, type->data, pkcs5_Length - 1)) 
            return rslt;
        for (i = 0; i < pkcs5_Count; i++)
            if (pkcs5_Ends[i] == type->data[pkcs5_Length - 1]) 
            {
                rslt = pkcs5_OIDs[i];
                break;
            }
        return rslt;
    }
    else
    {
        if (memcmp(pkcs12_PbeBegin, type->data, pkcs12_PbeLength - 1)) 
            return rslt;
        for (i = 0; i < 6; i++)
            if (pkcs12_PbeEnds[i] == type->data[pkcs12_PbeLength - 1]) 
            {
                rslt = pkcs12_PbeOIDs[i];
                break;
            }
        return rslt;
    }
}

void pbe_Init(PDFLibHandle lib, asn1_Object *alg, const char *pass, ppInt32 plen,
              asn1_Type *param, cipher_Context *ctx, ppInt32 enc_dec)
{
    ppInt32 id = cipher_PbeType(alg);
    
    switch (id)
    {
        case ID_pbeWithMD2AndDES_CBC:
        case ID_pbeWithMD5AndDES_CBC:
        case ID_pbeWithSHA1AndDES_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithMD2AndRC2_CBC:
        case ID_pbeWithMD5AndRC2_CBC:
        case ID_pbeWithSHA1AndRC2_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithSHA1And128BitRC4:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithSHA1And40BitRC4:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;
            
        case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
            ctx->block_size = 8;
            ctx->key_len = 24;
            ctx->iv_len = 8;
            ctx->cipher_id = id;
            break;
            
        case ID_pbeWithSHA1And2_Key_TripleDES_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithSHA1And128BitRC2_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithSHA1And40BitRC2_CBC:
            ctx->block_size = 8;
            ctx->key_len = 5;
            ctx->iv_len = 8;
            ctx->cipher_id = id;
            break;
    }
    switch (id)
    {
        case ID_pbeWithMD2AndDES_CBC:
        case ID_pbeWithMD2AndRC2_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithMD5AndDES_CBC:
        case ID_pbeWithMD5AndRC2_CBC:
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
            break;

        case ID_pbeWithSHA1AndDES_CBC:
        case ID_pbeWithSHA1AndRC2_CBC:
        case ID_pbeWithSHA1And128BitRC4:
        case ID_pbeWithSHA1And40BitRC4:
        case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
        case ID_pbeWithSHA1And2_Key_TripleDES_CBC:
        case ID_pbeWithSHA1And128BitRC2_CBC:
        case ID_pbeWithSHA1And40BitRC2_CBC:
            /*everything's ok */
            break;
    }

    if (pass == NULL) plen = 0;
    else if (plen == -1) plen = (ppInt32)strlen(pass);
    pkcs12_KeyIvGen(lib, ctx, pass, plen, param, enc_dec);
}



void cipher_Init(PDFLibHandle lib, cipher_Context *ctx, ppUns8 *key, ppUns8 *iv, ppInt32 enc)
{
    ppInt32 rslt = 0;
    if (enc && (enc != -1)) enc = 1;
    if (ctx == NULL) PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));
    
    ctx->lib = lib;
    if (ctx->cipher_id == ID_pbeWithSHA1And40BitRC2_CBC) 
        ctx->c.rc2.bits = ctx->key_len * 8;

    if (iv) memcpy(ctx->origiv, iv, ctx->iv_len);
    memcpy(ctx->workiv, ctx->origiv, ctx->iv_len);

    if (key)
    {
        switch (ctx->cipher_id)
        {
            case ID_pbeWithSHA1And40BitRC2_CBC:
                rslt = rc2_init_key(ctx, key);
                break;
            case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
                rslt = des_ede3_init_key(ctx, key);
                break;
            default:
                break;
        }
    }
    if (rslt == 0) PDFRAISE(lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
    if (enc != -1) ctx->encrypt = enc;
    ctx->buf_len = 0;
}

void cipher_UpdateEncrypt(cipher_Context *ctx, ppUns8 *ibuf, ppInt32 ilen, ppUns8 *obuf, ppInt32 *olen)
{
    ppInt32 i, j, bl, rslt;

    i = ctx->buf_len;
    bl = ctx->block_size;
    *olen = 0;
    if (ilen == 0 && i != bl) return;
    if (i != 0 && i + ilen < bl)
    {
        memcpy(ctx->buf + i, ibuf, ilen);
        ctx->buf_len += ilen;
        return;
    }
    if (i != 0)
    {
        j = bl - i;
        if (j != 0) memcpy(ctx->buf + i, ibuf, j);
        rslt = 0;
        switch (ctx->cipher_id)
        {
            case ID_pbeWithSHA1And40BitRC2_CBC:
                rslt = rc2_cbc_cipher(ctx, obuf, ctx->buf, bl);
                break;
            case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
                rslt = des_ede3_cbc_cipher(ctx, obuf, ctx->buf, bl);
                break;
            default:
                break;
        }
        if (rslt == 0) PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        ilen -= j; ibuf += j;
        obuf += bl; (*olen) += bl;
    }
    i = ilen % bl;
    ilen -= i;
    if (ilen > 0)
    {
        rslt = 0;
        switch (ctx->cipher_id)
        {
            case ID_pbeWithSHA1And40BitRC2_CBC:
                rslt = rc2_cbc_cipher(ctx, obuf, ibuf, ilen);
                break;
            case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
                rslt = des_ede3_cbc_cipher(ctx, obuf, ibuf, ilen);
                break;
            default:
                break;
        }
        if (rslt == 0) PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        *olen += ilen;
    }

    if (i != 0) memcpy(ctx->buf, ibuf + ilen, i);
    ctx->buf_len = i;
}

void cipher_UpdateDecrypt(cipher_Context *ctx, ppUns8 *ibuf, ppInt32 ilen, ppUns8 *obuf, ppInt32 *olen)
{
    ppInt32 b, bl, n;
    ppInt32 keep_last = 0;
    *olen = 0;
    if (ilen == 0) return;

    b = ctx->block_size;
    if (b > 1) 
    {
        bl = ctx->buf_len;
        n = ilen + bl;
        if (n % b == 0) 
        {
            if (ilen < b) 
            {
                memcpy(ctx->buf + bl, ibuf, ilen);
                ctx->buf_len = b;
                *olen = 0;
                return;
            }
            keep_last = 1;
            ilen -= b;
        }
    }

    cipher_UpdateEncrypt(ctx, ibuf, ilen, obuf, olen);

    if (keep_last) 
    {
        memcpy(ctx->buf, ibuf + ilen, b);
        ctx->buf_len = b;
    }
}

void cipher_Update(cipher_Context *ctx, ppUns8 *ibuf, ppInt32 ilen, ppUns8 *obuf, ppInt32 *olen)
{
    if (ctx->encrypt)
        cipher_UpdateEncrypt(ctx, ibuf, ilen, obuf, olen);
    else    
        cipher_UpdateDecrypt(ctx, ibuf, ilen, obuf, olen);
}

void cipher_FinishEncrypt(cipher_Context *ctx, ppUns8 *obuf, ppInt32 *olen)
{
    ppInt32 i, n, b, bl, rslt;
    
    if (ctx == NULL || ctx->lib == NULL) return;
    b = ctx->block_size;
    if (b == 1) { (*olen) = 0; return; }
    
    bl = ctx->buf_len; 
    n = b - bl;
    for (i = bl; i < b; i++) 
        *(ctx->buf + i) = (ppUns8)n;
    rslt = 0;
    switch (ctx->cipher_id)
    {
        case ID_pbeWithSHA1And40BitRC2_CBC:
            rslt = rc2_cbc_cipher(ctx, obuf, ctx->buf, b);
            break;
        case ID_pbeWithSHA1And3_Key_TripleDES_CBC:
            rslt = des_ede3_cbc_cipher(ctx, obuf, ctx->buf, b);
            break;
        default:
            break;
    }
    if (rslt == 0) PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
    (*olen) = b;
}

void cipher_FinishDecrypt(cipher_Context *ctx, ppUns8 *obuf, ppInt32 *olen)
{
    ppInt32 i, b;
    ppInt32 n;

    (*olen) = 0;
    b = ctx->block_size;
    if (b > 1) 
    {
        if (ctx->buf_len != b) 
            PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        cipher_UpdateEncrypt(ctx, ctx->buf, 0, ctx->buf, &n);
        if (n != b) 
            PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        n = ctx->buf[b - 1];
        if (n > b) 
            PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        for (i = 0; i < n; i++)
            if (ctx->buf[--b] != n) 
                PDFRAISE(ctx->lib, PDFBuildErrCode(ErrGeneralLevel, gleOtherError));
        n = ctx->block_size - n;
        for (i = 0; i < n; i++) 
            obuf[i] = ctx->buf[i];
        (*olen) = n;
    }
}

void cipher_Finish(cipher_Context *ctx, ppUns8 *obuf, ppInt32 *olen)
{
    if (ctx->encrypt)
        cipher_FinishEncrypt(ctx, obuf, olen);
    else
        cipher_FinishDecrypt(ctx, obuf, olen);
}

/*--- Password-based encryption according to PKCS#12 --- */

ppUns8 *pkcs12_PBE(PDFLibHandle lib, x509_Algorithm *alg, const char *pwd, ppInt32 plen, 
                 ppUns8 *ibuf, ppInt32 ilen, ppUns8 **pobuf, ppInt32 *polen, ppInt32 enc_dec)
{
    cipher_Context ctx;
    ppUns8 *obuf = NULL;
    ppInt32 olen, otmp;

    PDFTRY(lib){
        pbe_Init(lib, alg->algorithm, pwd, plen, alg->parameter, &ctx, enc_dec);
        obuf = ( ppUns8 * )mmalloc(lib, ilen + ctx.block_size);
        cipher_Update(&ctx, ibuf, ilen, obuf, &otmp); olen = otmp;
        cipher_Finish(&ctx, obuf + otmp, &otmp); olen += otmp;
    
        if (polen) (*polen) = olen;
        if (pobuf) (*pobuf) = obuf;
    } PDFEXCEPT(lib){
        if (obuf) 
        {
            mfree(lib, obuf);
            obuf = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return obuf;
}


ppInt32 pkcs12_KeyGenUnicode(PDFLibHandle lib, const ppUns8 *pass, ppInt32 plen, ppUns8 *salt, ppInt32 slen, 
                         ppInt32 id, ppInt32 iter, ppUns8 *obuf, ppInt32 olen)
{
    ppUns8 *A, *B, *D, *I, *p;
    ppInt32 Slen, Plen, Ilen, bnlen;
    ppInt32 i, j, u, v, t, nB, nD;
    ppUns8 str1[MAX_RSA_MODULUS_LEN], 
         strB[MAX_RSA_MODULUS_LEN], 
         strI[MAX_RSA_MODULUS_LEN], *ptrB;
    ppUns32 dig1[MAX_ppUns64S], 
             digB[MAX_ppUns64S], 
             digI[MAX_ppUns64S];
    sha1_Context ctx;

    nB = MAX_RSA_MODULUS_LEN;
    nD = MAX_ppUns64S;
    v = SHA1_BLOCKSIZE;
    u = SHA1_HASHSIZE;
    D = ( ppUns8 * ) mmalloc(lib, v);
    for (i = 0; i < v; i++) D[i] = (ppUns8)id;

    A = ( ppUns8 * ) mmalloc(lib, u);
    B = ( ppUns8 * ) mmalloc(lib, v + 1);
    Slen = v*((slen + v - 1)/v);
    Plen = (plen ? v*((plen + v - 1)/v) : 0);
    Ilen = Slen + Plen;
    I = ( ppUns8 * ) mmalloc(lib, Ilen);
    p = I;
    for (i = 0; i < Slen; i++) *p++ = salt[i % slen];
    for (i = 0; i < Plen; i++) *p++ = pass[i % plen];

    for (;;)
    {
        sha1_Init(&ctx);
        sha1_Update(&ctx, D, v);
        sha1_Update(&ctx, I, Ilen);
        sha1_Finish(&ctx, A);
        for (j = 1; j < iter; j++)
        {
            sha1_Init(&ctx);
            sha1_Update(&ctx, A, u);
            sha1_Finish(&ctx, A);
        }
        memcpy (obuf, A, min (olen, u));
        if (u >= olen)
        {
            mfree(lib, A);
            mfree(lib, B);
            mfree(lib, D);
            mfree(lib, I);
            return 1;
        }
        olen -= u; obuf += u;
        for (j = 0; j < v; j++) B[j] = A[j % u];

        for (i = 0; i < nB - 1; i++) str1[i] = 0; str1[nB - 1] = 1;
        LargeNumberDecode(dig1, nD, str1, nB);
        t = nB - v; /*if (t < 0) then exception! */
        for (i = 0; i < t; i++) strB[i] = 0; memcpy(&(strB[t]), B, v);
        LargeNumberDecode(digB, nD, strB, nB);
        LargeNumberAdd(digB, digB, dig1, nD);

        for (j = 0; j < Ilen; j += v) 
        {
            t = nB - v; /* if (t < 0) then exception! */
            for (i = 0; i < t; i++) strI[i] = 0;
            memcpy(&(strI[t]), I + j, v);
            LargeNumberDecode(digI, nD, strI, nB);
            LargeNumberAdd(digI, digI, digB, nD);
            LargeNumberEncode(strB, nB, digI, nD);
            bnlen = nB;
            for (i = 0; i < nB; i++) 
                if (!strB[i])
                    bnlen--;
                else
                    break;
            ptrB = &(strB[i]);

            if (bnlen > v)
                memcpy(I + j, ptrB + 1, v);
            else if (bnlen < v)
            {
                memset(I + j, 0, v - bnlen);
                memcpy(I + j + v - bnlen, ptrB, bnlen);
            } 
            else 
                memcpy(I + j, ptrB, v);
        }
    }
}

ppUns8 *asc2uni(PDFLibHandle lib, const char *asc, ppInt32 asclen, ppUns8 **uni, ppInt32 *unilen)
{
    ppInt32 len, i;
    ppUns8 *tmp;
    if (asclen == -1) asclen = (ppInt32)strlen(asc);
    len = asclen * 2  + 2;
    tmp = ( ppUns8 * )mmalloc(lib, len);
    for (i = 0; i < len - 2; i += 2) 
    {
        tmp[i] = 0; tmp[i + 1] = asc[i >> 1];
    }
    tmp[len - 2] = 0; tmp[len - 1] = 0;
    if (unilen != NULL) (*unilen) = len;
    if (uni != NULL) (*uni) = tmp;
    return tmp;
}

ppInt32 pkcs12_KeyGenAscii(PDFLibHandle lib, const char *pass, ppInt32 plen, ppUns8 *salt, ppInt32 slen, 
                       ppInt32 id, ppInt32 iter, ppUns8 *obuf, ppInt32 olen)
{
    ppInt32   rslt = 0;
    ppUns8   *unipass;
    ppInt32   uniplen;
    if (pass != NULL) 
    {
        asc2uni(lib, pass, plen, (ppUns8 **)&unipass, &uniplen); 
        rslt = pkcs12_KeyGenUnicode(lib, unipass, uniplen, salt, slen, id, iter, obuf, olen);
        if (unipass != NULL) 
        {
            memset(unipass, 0, uniplen);
            mfree(lib, unipass);
        }
    }
    return rslt;
}

/*--- PBE Parameters --- */

void pbe_DisposeParam(PDFLibHandle lib, pbe_Param *par)
{
    if (par == NULL) return;
    asn1_DisposeOctetString(lib, par->salt);
    asn1_DisposeInteger(lib, par->iter);
    mfree(lib, par);
}

pbe_Param *pbe_CreateParam(PDFLibHandle lib)
{
    pbe_Param *rslt = NULL;
    PDFTRY(lib){
        rslt = (pbe_Param* )mmalloc(lib, sizeof(pbe_Param));
        rslt->iter = asn1_CreateInteger(lib);
        rslt->salt = asn1_CreateOctetString(lib);
    } PDFEXCEPT(lib){
        if (rslt != NULL) pbe_DisposeParam(lib, rslt); 
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pbe_Param *pbe_DecodeParam(PDFLibHandle lib, pbe_Param **ppar, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context c;
    pbe_Param *rslt = NULL;
    PDFTRY(lib){
        c.pp = pbuf;
        c.q = *pbuf;
        if (ppar == NULL || (*ppar) == NULL) 
            rslt = pbe_CreateParam(lib);
        else
            rslt = *ppar;

        c.p = *pbuf;
        c.max = ! max ? NULL : c.p + (ppInt32)max;
        asn1_GetSequence(lib, &c, &max);

        c.q = c.p;
        asn1_DecodeOctetString(lib, &(rslt->salt), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->iter), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        asn1_Finish(lib, &c);
        (*pbuf) = c.p;
        if (ppar != NULL) (*ppar) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (ppar == NULL || (*ppar) != rslt)) 
        {
            pbe_DisposeParam(lib, rslt); 
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

/*--- PBE Parameters END --- */

ppInt32 pkcs12_KeyIvGen(PDFLibHandle lib, cipher_Context *ctx, const char *pass, ppInt32 plen, 
                    asn1_Type *param, ppInt32 en_de)
{
    pbe_Param *pbe = NULL;
    ppInt32 slen, iter;
    ppUns8 *salt, *pbuf;
    ppUns8 key[MAX_KEY_LENGTH], iv[MAX_IV_LENGTH];
    PDFTRY(lib){
        pbuf = param->value.sequence->data;
        if (param == NULL || param->type != SEQUENCE_TAG) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));
        pbe = pbe_DecodeParam(lib, NULL, &pbuf, param->value.sequence->length);
        if (pbe->iter == 0) iter = 1;
        else iter = (ppInt32)asn1_GetInteger(pbe->iter);
        salt = pbe->salt->data;
        slen = pbe->salt->length;

        pkcs12_KeyGen(lib, pass, plen, salt, slen, PKCS12_KEYGEN_KEYID, iter, key, ctx->key_len); 
        pkcs12_KeyGen(lib, pass, plen, salt, slen, PKCS12_KEYGEN_IVID, iter, iv, ctx->iv_len); 

        cipher_Init(lib, ctx, key, iv, en_de);
        
        if (pbe != NULL)
            pbe_DisposeParam(lib, pbe);
        memset(key, 0, MAX_KEY_LENGTH);
        memset(iv, 0, MAX_IV_LENGTH);
    } PDFEXCEPT(lib){
        if (pbe != NULL)
            pbe_DisposeParam(lib, pbe);
        memset(key, 0, MAX_KEY_LENGTH);
        memset(iv, 0, MAX_IV_LENGTH);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return 1;
}



ppInt32 rc2_init_key(cipher_Context *ctx, const ppUns8 *key)
{
    RC2_set_key(&(ctx->c.rc2.key), ctx->key_len, key, ctx->c.rc2.bits);
    return 1;
}

ppInt32 rc2_cbc_cipher(cipher_Context *ctx, ppUns8 *out, const ppUns8 *in, ppUns32 inl) 
{
    RC2_cbc_encrypt(in, out, (ppInt64)inl, &ctx->c.rc2.key, ctx->workiv, ctx->encrypt);
    return 1;
}



ppInt32 des_ede3_init_key(cipher_Context *ctx, const ppUns8 *key)
{
    des_cblock *deskey = (des_cblock* )key;
    des_set_key_unchecked(&deskey[0],ctx->c.des.des_ede.ks1);
    des_set_key_unchecked(&deskey[1],ctx->c.des.des_ede.ks2);
    des_set_key_unchecked(&deskey[2],ctx->c.des.des_ede.ks3);

    return 1;
}

ppInt32 des_ede3_cbc_cipher(cipher_Context *ctx, ppUns8 *out, const ppUns8 *in, ppUns32 inl)
{
    des_ede3_cbc_encrypt(in, out, (ppInt64)inl, ctx->c.des.des_ede.ks1, ctx->c.des.des_ede.ks2, ctx->c.des.des_ede.ks3, (des_cblock* )ctx->workiv, ctx->encrypt);
    return 1;
}

/*SHA */
void sha1_Init(sha1_Context *ctx)
{
    if (!ctx) return;

    ctx->Nl = ctx->Nh = 0;
    ctx->num = 0;

    ctx->h0 = 0x67452301;
    ctx->h1 = 0xEFCDAB89;
    ctx->h2 = 0x98BADCFE;
    ctx->h3 = 0x10325476;
    ctx->h4 = 0xC3D2E1F0;
}

void sha1_Finish(sha1_Context *ctx, ppUns8 hash[SHA1_HASHSIZE])
{
    ppInt32 i;
    if (!ctx || !hash) return;

    sha1_Pad(ctx);
    for(i = 0; i < 64; ++i) ctx->data[i] = 0;
    ctx->Nh = ctx->Nl = 0;
    
    hash[0] = (ppUns8)((ctx->h0 >> 24) & 0xff);
    hash[1] = (ppUns8)((ctx->h0 >> 16) & 0xff);
    hash[2] = (ppUns8)((ctx->h0 >> 8) & 0xff);
    hash[3] = (ppUns8)(ctx->h0 & 0xff);
    hash[4] = (ppUns8)((ctx->h1 >> 24) & 0xff);
    hash[5] = (ppUns8)((ctx->h1 >> 16) & 0xff);
    hash[6] = (ppUns8)((ctx->h1 >> 8) & 0xff);
    hash[7] = (ppUns8)(ctx->h1 & 0xff);
    hash[8] = (ppUns8)((ctx->h2 >> 24) & 0xff);
    hash[9] = (ppUns8)((ctx->h2 >> 16) & 0xff);
    hash[10] = (ppUns8)((ctx->h2 >> 8) & 0xff);
    hash[11] = (ppUns8)(ctx->h2 & 0xff);
    hash[12] = (ppUns8)((ctx->h3 >> 24) & 0xff);
    hash[13] = (ppUns8)((ctx->h3 >> 16) & 0xff);
    hash[14] = (ppUns8)((ctx->h3 >> 8) & 0xff);
    hash[15] = (ppUns8)(ctx->h3 & 0xff);
    hash[16] = (ppUns8)((ctx->h4 >> 24) & 0xff);
    hash[17] = (ppUns8)((ctx->h4 >> 16) & 0xff);
    hash[18] = (ppUns8)((ctx->h4 >> 8) & 0xff);
    hash[19] = (ppUns8)(ctx->h4 & 0xff);
}

void sha1_Update(sha1_Context *ctx, const ppUns8 *data, ppUns32 len)
{
    if (!ctx || !data || len == 0) return;

    while(len--)
    {
        ctx->data[ctx->num++] = (*data & 0xff);
        ctx->Nl += 8;
        if (ctx->Nl == 0)
            ctx->Nh++;
        /* if if (ctx->Nh == 0) then exception: message is too ppInt64 */;
        if (ctx->num == SHA1_BLOCKSIZE)
            sha1_Block(ctx);
        data++;
    }
}

void sha1_Block(sha1_Context *ctx)
{
    const ppUns32 K[] = {0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6};
    ppInt32 i;
    ppUns32 temp;
    ppUns32 W[80];
    ppUns32 A, B, C, D, E;

    for(i = 0; i < 16; i++)
    {
        W[i] = ctx->data[i * 4] << 24;
        W[i] |= ctx->data[i * 4 + 1] << 16;
        W[i] |= ctx->data[i * 4 + 2] << 8;
        W[i] |= ctx->data[i * 4 + 3];
    }

    for(i = 16; i < 80; i++)
       W[i] = ROTATE_LEFT(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);

    A = ctx->h0; B = ctx->h1; C = ctx->h2; D = ctx->h3; E = ctx->h4;

    for(i = 0; i < 20; i++)
    {
        temp =  ROTATE_LEFT(A, 5) + ((B & C) | ((~B) & D)) + E + W[i] + K[0];
        E = D; D = C;
        C = ROTATE_LEFT(B, 30);
        B = A; A = temp;
    }

    for(i = 20; i < 40; i++)
    {
        temp = ROTATE_LEFT(A, 5) + (B ^ C ^ D) + E + W[i] + K[1];
        E = D; D = C;
        C = ROTATE_LEFT(B, 30);
        B = A; A = temp;
    }

    for(i = 40; i < 60; i++)
    {
        temp = ROTATE_LEFT(A, 5) + ((B & C) | (B & D) | (C & D)) + E + W[i] + K[2];
        E = D; D = C;
        C = ROTATE_LEFT(B, 30);
        B = A; A = temp;
    }

    for(i = 60; i < 80; i++)
    {
        temp = ROTATE_LEFT(A, 5) + (B ^ C ^ D) + E + W[i] + K[3];
        E = D; D = C;
        C = ROTATE_LEFT(B, 30);
        B = A; A = temp;
    }

    ctx->h0 += A; ctx->h1 += B; ctx->h2 += C; ctx->h3 += D; ctx->h4 += E;
    ctx->num = 0;
}

void sha1_Pad(sha1_Context *ctx)
{
    if (ctx->num > 55)
    {
        ctx->data[ctx->num++] = 0x80;
        while(ctx->num < 64)
            ctx->data[ctx->num++] = 0;
        sha1_Block(ctx);
        while(ctx->num < 56)
            ctx->data[ctx->num++] = 0;
    }
    else
    {
        ctx->data[ctx->num++] = 0x80;
        while(ctx->num < 56)
            ctx->data[ctx->num++] = 0;
    }

    ctx->data[56] = (ppUns8)((ctx->Nh >> 24) & 0xff);
    ctx->data[57] = (ppUns8)((ctx->Nh >> 16) & 0xff);
    ctx->data[58] = (ppUns8)((ctx->Nh >> 8) & 0xff);
    ctx->data[59] = (ppUns8)(ctx->Nh & 0xff);
    ctx->data[60] = (ppUns8)((ctx->Nl >> 24) & 0xff);
    ctx->data[61] = (ppUns8)((ctx->Nl >> 16) & 0xff);
    ctx->data[62] = (ppUns8)((ctx->Nl >> 8) & 0xff);
    ctx->data[63] = (ppUns8)(ctx->Nl & 0xff);

    sha1_Block(ctx);
}

ppInt32 pkcs7_EncodeDigestInfo(pkcs7_DI *di, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (di == NULL) return 0;
    rslt += x509_EncodeAlgorithm(di->digestAlgorithm, NULL);
    rslt += asn1_EncodeOctetString(di->digest, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    x509_EncodeAlgorithm(di->digestAlgorithm, &p);
    asn1_EncodeOctetString(di->digest, &p);
    (*pbuf) = p;
    return r;
}

pkcs7_DI *pkcs7_DecodeDigestInfo(PDFLibHandle lib, pkcs7_DI **pdi, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs7_DI *rslt = NULL;
    
    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (pdi == NULL || (*pdi) == NULL)
            rslt = pkcs7_CreateDigestInfo(lib);
        else
            rslt = *pdi;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);
    
        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->digestAlgorithm), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);
    
        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->digest), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pdi != NULL) (*pdi) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pdi == NULL || (*pdi) != rslt))
        {
            pkcs7_DisposeDigestInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs7_DI *pkcs7_CreateDigestInfo(PDFLibHandle lib)
{
    pkcs7_DI *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs7_DI* )mmalloc(lib, sizeof(pkcs7_DI));
        rslt->digestAlgorithm = x509_CreateAlgorithm(lib);
        rslt->digest = asn1_CreateOctetString(lib);
    }PDFEXCEPT(lib){
        if (rslt != NULL) pkcs7_DisposeDigestInfo(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib) ;
    return rslt;
}

void pkcs7_DisposeDigestInfo(PDFLibHandle lib, pkcs7_DI *di)
{
    if (di == NULL) return;
    x509_DisposeAlgorithm(lib, di->digestAlgorithm);
    asn1_DisposeOctetString(lib, di->digest);
    mfree(lib, di);
}



pkcs12_MacData *pkcs12_DecodeMacData(PDFLibHandle lib, pkcs12_MacData **pdat, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs12_MacData *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
    
        if (pdat == NULL || (*pdat) == NULL)
            rslt = pkcs12_CreateMacData(lib);
        else
            rslt = *pdat;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        pkcs7_DecodeDigestInfo(lib, &(rslt->mac), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->macSalt), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*(ctx.p) & (~CONSTRUCTED)) == (UNIVERSAL_CLS|INTEGER_TAG))
        {
            ctx.q = ctx.p;
            asn1_DecodeInteger(lib, &(rslt->iterations), &ctx.p, ctx.len);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pdat != NULL) (*pdat) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pdat == NULL || (*pdat) != rslt)) 
        {
            pkcs12_DisposeMacData(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs12_MacData *pkcs12_CreateMacData(PDFLibHandle lib)
{
    pkcs12_MacData *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs12_MacData* )mmalloc(lib, sizeof(pkcs12_MacData));
        rslt->mac = pkcs7_CreateDigestInfo(lib);
        rslt->macSalt = asn1_CreateOctetString(lib);
        rslt->iterations = NULL;
    }PDFEXCEPT(lib){
        if (rslt != NULL) pkcs12_DisposeMacData(lib, rslt);
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

void pkcs12_DisposeMacData(PDFLibHandle lib, pkcs12_MacData *dat)
{
    if (dat == NULL) return;
    pkcs7_DisposeDigestInfo(lib, dat->mac);
    asn1_DisposeOctetString(lib, dat->macSalt);
    asn1_DisposeInteger(lib, dat->iterations);
    mfree(lib, dat);
}



pkcs12 *pkcs12_Decode(PDFLibHandle lib, pkcs12 **pobj, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs12 *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (pobj == NULL || (*pobj) == NULL)
            rslt = pkcs12_Create(lib);
        else 
            rslt = *pobj;
    
        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        pkcs7_Decode(lib, &(rslt->authSafe), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if ((ctx.len != 0) && (((*ctx.p) & (~CONSTRUCTED)) == (UNIVERSAL_CLS | SEQUENCE_TAG)))
        {
            ctx.q = ctx.p;
            pkcs12_DecodeMacData(lib, &(rslt->macData), &ctx.p, ctx.len);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pobj != NULL) (*pobj) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pobj == NULL || (*pobj) != rslt))
        {
            pkcs12_Dispose(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs12 *pkcs12_Create(PDFLibHandle lib)
{
    pkcs12 *rslt;
    rslt = (pkcs12* )mmalloc(lib, sizeof(pkcs12)); 
    rslt->version = NULL;
    rslt->macData = NULL;
    rslt->authSafe = NULL;
    return rslt;
}

void pkcs12_Dispose(PDFLibHandle lib, pkcs12 *obj)
{
    if (obj == NULL) return;
    asn1_DisposeInteger(lib, obj->version);
    pkcs12_DisposeMacData(lib, obj->macData);
    pkcs7_Dispose(lib, obj->authSafe);
    mfree(lib, obj);
}



ppInt32 pkcs12_BagContentType(asn1_Object *type)
{
    /*TO DO: optimize */
    return asn1_GetObjectID(type);
}

pkcs12_Bag *pkcs12_DecodeBag(PDFLibHandle lib, pkcs12_Bag **pbag, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs12_Bag *rslt = NULL;
    ppInt32 id, ainf, atag, acls;
    ppInt64 alen;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;

        if (pbag == NULL || (*pbag) == NULL)
            rslt = pkcs12_CreateBag(lib);
        else
            rslt = *pbag;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);
    
        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->type), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        id = pkcs12_BagContentType(rslt->type);
        switch (id) 
        {
            case ID_pkcs12CertTypeX509:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeOctetString(lib, &(rslt->value.x509cert), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;

            case ID_pkcs12CertTypeSDSI:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80)
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeIA5String(lib, &(rslt->value.sdsicert), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
                
            case ID_pkcs12CRLTypeX509:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80)
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeOctetString(lib, &(rslt->value.x509crl), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
            
            default:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80)
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeType(lib, &(rslt->value.other), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pbag != NULL) (*pbag) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (pbag == NULL || (*pbag) != rslt))
        {
            pkcs12_DisposeBag(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

pkcs12_Bag *pkcs12_CreateBag(PDFLibHandle lib)
{
    pkcs12_Bag *rslt;
    rslt = (pkcs12_Bag* )mmalloc(lib, sizeof(pkcs12_Bag));
    rslt->type = NULL;
    rslt->value.other = NULL;
    return rslt;
}

void pkcs12_DisposeBag(PDFLibHandle lib, pkcs12_Bag *bag)
{
    if (bag == NULL) return;
    switch (pkcs12_BagContentType(bag->type))
    {
        case ID_pkcs12CertTypeX509:
            asn1_DisposeOctetString(lib, bag->value.x509cert);
            break;

        case ID_pkcs12CertTypeSDSI:
            asn1_DisposeIA5String(lib, bag->value.sdsicert);
            break;

        case ID_pkcs12CRLTypeX509:
            asn1_DisposeOctetString(lib, bag->value.x509crl);
            break;

        default:
            asn1_DisposeType(lib, bag->value.other);
            break;
    }
    asn1_DisposeObject(lib, bag->type);
    mfree(lib, bag);
}




pkcs8_PKI *pkcs8_DecodePrivateKeyInfo(PDFLibHandle lib, pkcs8_PKI **ppki, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs8_PrivateKeyInfo *rslt = NULL;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (ppki == NULL || (*ppki) == NULL)
            rslt = pkcs8_CreatePrivateKeyInfo(lib);
        else
            rslt = *ppki;
    
        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeInteger(lib, &(rslt->version), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);
        
        ctx.q = ctx.p;
        x509_DecodeAlgorithm(lib, &(rslt->pkeyalg), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        ctx.q = ctx.p;
        asn1_DecodeOctetString(lib, &(rslt->pkey), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        if (ctx.len != 0 && (*ctx.p) == (CTX_SPECIFIC_CLS|CONSTRUCTED/*|0*/))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->attributes), &ctx.p, ctx.len,
                (TDecodeFunction)x509_DecodeAttribute, (TDisposeFunction)x509_DisposeAttribute, 
                0, CTX_SPECIFIC_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }

        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (ppki != NULL) (*ppki) = rslt;
    }PDFEXCEPT(lib) {
        if (rslt != NULL && (ppki == NULL || (*ppki) != rslt))
        {
            pkcs8_DisposePrivateKeyInfo(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

pkcs8_PrivateKeyInfo *pkcs8_CreatePrivateKeyInfo(PDFLibHandle lib)
{
    pkcs8_PrivateKeyInfo *rslt = NULL;
    PDFTRY(lib) {
        rslt = (pkcs8_PKI* )mmalloc(lib, sizeof(pkcs8_PKI));
        rslt->version = asn1_CreateInteger(lib);
        rslt->pkeyalg = x509_CreateAlgorithm(lib);
        rslt->pkey = asn1_CreateOctetString(lib);
        rslt->attributes = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) pkcs8_DisposePrivateKeyInfo(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void pkcs8_DisposePrivateKeyInfo(PDFLibHandle lib, pkcs8_PrivateKeyInfo *pki)
{
    if (pki == NULL) return;
    asn1_DisposeInteger(lib, pki->version);
    x509_DisposeAlgorithm(lib, pki->pkeyalg);
    asn1_DisposeOctetString(lib, pki->pkey);
    vector_Dispose(pki->attributes, (void (* )(PDFLibHandle, void * ))x509_DisposeAttribute);
    mfree(lib, pki);
}



ppInt32 pkcs12_BagType(asn1_Object *type)
{
    /*TO DO: optimize */
    return asn1_GetObjectID(type);
}

pkcs12_SafeBag *pkcs12_DecodeSafeBag(PDFLibHandle lib, pkcs12_SafeBag **pbag, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context ctx;
    pkcs12_SafeBag *rslt=NULL;
    ppInt32 ainf, atag, acls;
    ppInt64 alen;

    PDFTRY(lib){
        ctx.pp = pbuf;
        ctx.q = *pbuf;
        if (pbag == NULL || (*pbag) == NULL)
            rslt = pkcs12_CreateSafeBag(lib);
        else
            rslt = *pbag;

        ctx.p = *pbuf;
        ctx.max = ! max ? NULL : ctx.p + (ppInt32)max;
        asn1_GetSequence(lib, &ctx, &max);

        ctx.q = ctx.p;
        asn1_DecodeObject(lib, &(rslt->type), &ctx.p, ctx.len);
        ctx.len -= (ppInt64)(ctx.p - ctx.q);

        switch (pkcs12_BagType(rslt->type)) 
        {
            case ID_keyBag:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    pkcs8_DecodePrivateKeyInfo(lib, &(rslt->value.keybag), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
            case ID_pkcs8ShroudedKeyBag:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    pkcs7_DecodeDigestInfo(lib, &(rslt->value.shkeybag), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
            case ID_certBag:
            case ID_crlBag:
            case ID_secretBag:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    pkcs12_DecodeBag(lib, &(rslt->value.bag), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
            case ID_safeContentsBag:
                if (ctx.len != 0 && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80) 
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeSet(lib, &(rslt->value.safes), &ctx.p, alen, 
                        (TDecodeFunction)pkcs12_DecodeSafeBag, (TDisposeFunction)pkcs12_DisposeSafeBag, 
                        SEQUENCE_TAG, UNIVERSAL_CLS);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen)) 
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;

            default:
                if (ctx.len != 0L && *(ctx.p) == (CONSTRUCTED|CTX_SPECIFIC_CLS/*|0*/))
                {
                    ctx.q = ctx.p;
                    ainf = asn1_GetObject(&ctx.p, &alen, &atag, &acls, ctx.len);
                    if (ainf & 0x80)
                        PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadAsn1HeaderError));
                    if (ainf == (CONSTRUCTED + 1))
                        alen = ctx.len - (ctx.p - ctx.q) - 2;
                    asn1_DecodeType(lib, &(rslt->value.other), &ctx.p, alen);
                    if (ainf == (CONSTRUCTED + 1)) 
                    {
                        alen = ctx.len - (ctx.p - ctx.q);
                        if (!asn1_CheckInfiniteEnd(&ctx.p, alen))
                            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleLengthMismatchError));
                    }
                    ctx.len -= (ppInt64)(ctx.p - ctx.q);
                }
                break;
        }
        if (ctx.len != 0 && (*ctx.p) == (UNIVERSAL_CLS|CONSTRUCTED|SET_TAG))
        {
            ctx.q = ctx.p;
            asn1_DecodeSet(lib, &(rslt->attrib), &ctx.p, ctx.len, 
                (TDecodeFunction)x509_DecodeAttribute, (TDisposeFunction)x509_DisposeAttribute, 
                SET_TAG, UNIVERSAL_CLS);
            ctx.len -= (ppInt64)(ctx.p - ctx.q);
        }
    
        asn1_Finish(lib, &ctx);
        (*pbuf) = ctx.p;
        if (pbag != NULL) (*pbag) = rslt;
    }PDFEXCEPT(lib) {
        if (rslt != NULL && (pbag == NULL || (*pbag) != rslt)) 
        {
            pkcs12_DisposeSafeBag(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

pkcs12_SafeBag *pkcs12_CreateSafeBag(PDFLibHandle lib)
{
    pkcs12_SafeBag *rslt = NULL;
    PDFTRY(lib){
        rslt = (pkcs12_SafeBag* )mmalloc(lib, sizeof(pkcs12_SafeBag));
        rslt->type = NULL;
        rslt->value.other = NULL;
        rslt->attrib = vector_Create(lib);
        rslt->rest = NULL;
    } PDFEXCEPT(lib){
        if (rslt != NULL) pkcs12_DisposeSafeBag(lib, rslt);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

void pkcs12_DisposeSafeBag(PDFLibHandle lib, pkcs12_SafeBag *bag)
{
    if (bag == NULL) return;
    switch (pkcs12_BagType(bag->type)) 
    {
        case ID_keyBag:
            pkcs8_DisposePrivateKeyInfo(lib, bag->value.keybag);
            break;
        case ID_pkcs8ShroudedKeyBag:
            pkcs7_DisposeDigestInfo(lib, bag->value.shkeybag);
            break;
        case ID_certBag:
        case ID_crlBag:
        case ID_secretBag:
            pkcs12_DisposeBag(lib, bag->value.bag);
            break;
        case ID_safeContentsBag:
            vector_Dispose(bag->value.safes, (void (* )(PDFLibHandle, void * ))pkcs12_DisposeSafeBag);
            break;
        default:
            asn1_DisposeType(lib, bag->value.other);
            break;
    }
    asn1_DisposeObject(lib, bag->type);
    vector_Dispose(bag->attrib, (void (* )(PDFLibHandle, void * ))x509_DisposeAttribute);
    mfree(lib, bag);
}

/*--- Verifying of PKCS#12 content --- */

typedef struct
{
    sha1_Context hctx;
    sha1_Context ictx;
    sha1_Context octx;
    ppUns32 keyLength;
    ppUns8 key[SHA1_BLOCKSIZE];
} hmac_Context;

void hmac_Init(hmac_Context *ctx, const void *key, ppInt32 len)
{
    ppInt32 i, j;
    ppUns8 pad[SHA1_BLOCKSIZE];

    if (key != NULL)
    {
        j = SHA1_BLOCKSIZE; /*| ...HASHSIZE ? */
        if (j < len)
        {
            sha1_Init(&ctx->hctx);
            sha1_Update(&ctx->hctx, ( ppUns8 * )key, len);
            sha1_Finish(&ctx->hctx, ctx->key ) ;
            ctx->keyLength = SHA1_HASHSIZE;
        }
        else
        {
            memcpy(ctx->key, key, len);
            ctx->keyLength = len;
        }
        if (ctx->keyLength != SHA1_BLOCKSIZE)
            memset(ctx->key + ctx->keyLength, 0, SHA1_BLOCKSIZE - ctx->keyLength);
    }

    for (i = 0; i < SHA1_BLOCKSIZE; i++)
        pad[i] = 0x36^ctx->key[i];
    sha1_Init(&ctx->ictx);
    sha1_Update(&ctx->ictx, pad, SHA1_BLOCKSIZE);
    for (i = 0; i < SHA1_BLOCKSIZE; i++)
        pad[i] = 0x5c^ctx->key[i];
    sha1_Init(&ctx->octx);
    sha1_Update(&ctx->octx, pad, SHA1_BLOCKSIZE);

    memcpy(&ctx->hctx, &ctx->ictx, sizeof(ctx->ictx));
}

void hmac_Update(hmac_Context *ctx, const ppUns8 *data, ppInt32 len)
{
    sha1_Update(&(ctx->hctx), data, len);
}

void hmac_Finish(hmac_Context *ctx, ppUns8 *hash)
{
    ppUns8 buf[SHA1_HASHSIZE];
    sha1_Finish(&(ctx->hctx), buf);
    memcpy(&(ctx->hctx), &(ctx->octx), sizeof(ctx->octx));
    sha1_Update(&(ctx->hctx), buf, SHA1_HASHSIZE);
    sha1_Finish(&(ctx->hctx), hash);
}

void pkcs12_GenerateMac(PDFLibHandle lib, pkcs12 *p12, const char *pwd, ppInt32 plen, ppUns8 *mac, ppUns32 *mlen)
{
    hmac_Context hmac;
    ppUns8 key[SHA1_HASHSIZE], *salt;
    ppInt32 slen, klen = SHA1_HASHSIZE, iter, id;
    salt = p12->macData->macSalt->data;
    slen = p12->macData->macSalt->length;
    if (!p12->macData->iterations) iter = 1;
    else iter = (ppInt32)asn1_GetInteger(p12->macData->iterations);
    id = asn1_GetObjectID(p12->macData->mac->digestAlgorithm->algorithm);
    if (id != ID_sha1) PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
    pkcs12_KeyGen(lib, pwd, plen, salt, slen, PKCS12_KEYGEN_MACID, iter, key, klen);

    hmac_Init(&hmac, key, klen);
    hmac_Update(&hmac, p12->authSafe->contents.data->data, p12->authSafe->contents.data->length);
    hmac_Finish(&hmac, mac); 
    *mlen = klen; /*both are SHA1_HASHSIZE */
}

ppInt32 pkcs12_VerifyMac(PDFLibHandle lib, pkcs12 *p12, const char *pwd, ppInt32 plen)
{
    ppUns8 mac[SHA1_HASHSIZE];
    ppUns32 mlen;
    
    if (p12->macData == NULL) return 0;
    pkcs12_GenerateMac(lib, p12, pwd, plen, mac, &mlen);
    if ((ppInt32)mlen != p12->macData->mac->digest->length) return 0;
    if (memcmp(mac, p12->macData->mac->digest->data, mlen)) return 0;
    return 1;
}

/*--- Parsing of PKCS#12 content --- */

void parse(PDFLibHandle, pkcs12*, const char *, ppInt32, rsa_Key **, x509**, VECTOR** );
void parse_bags(PDFLibHandle, VECTOR*, const char *, ppInt32, rsa_Key **, x509**, VECTOR**, asn1_OctetString**, char * );
void parse_bag(PDFLibHandle, pkcs12_SafeBag*, const char *, ppInt32, rsa_Key **, x509**, VECTOR**, asn1_OctetString**, char * );

ppInt32 pkcs12_Parse(PDFLibHandle lib, pkcs12 *p12, const char *pwd, rsa_Key **ppkey, x509 **pcert, VECTOR **pcas)
{
    ppInt32 rslt = 1;
    PDFTRY(lib){
        if(p12 == NULL) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));

        if (pcas != NULL && (*pcas) == NULL) 
            (*pcas) = vector_Create(lib);
        if (ppkey != NULL) (*ppkey) = NULL;
        if (pcert != NULL) (*pcert) = NULL;

        if (!pkcs12_VerifyMac(lib, p12, pwd, -1)) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleBadPasswordError));
        
        parse(lib, p12, pwd, -1, ppkey, pcert, pcas);
    } PDFEXCEPT(lib){
        if (ppkey != NULL && (*ppkey) != NULL) 
        {
            rsa_DisposeKey(lib, *ppkey);
            (*ppkey) = NULL;
        }
        if (pcert != NULL && (*pcert) != NULL) 
        {
            x509_Dispose(lib, *pcert);
            (*pcert) = NULL;
        }
        if (pcas != NULL) 
        {
            vector_Dispose(*pcas, (void (* )(PDFLibHandle, void * ))x509_Dispose);
            (*pcas) = NULL;
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}

asn1_Type *pkcs12_GetAttribute(VECTOR *attrs, ppInt32 id)
{
    x509_Attribute *attr;
    ppInt32 i;
    if (attrs == NULL) return NULL;
    for (i = 0; i < vector_GetCount(attrs); i++) 
    {
        attr = (x509_Attribute* )vector_GetValueAt(attrs, i);
        if (asn1_GetObjectID(attr->object) == id) 
        {
            if (vector_GetCount(attr->value.set) > 0)
                return (asn1_Type* )vector_GetValueAt(attr->value.set, 0);
            else
                return NULL;
        }
    }
    return NULL;
}

void parse(PDFLibHandle lib, pkcs12 *p12, const char *pwd, ppInt32 len,
           rsa_Key **ppkey, x509 **pcert, VECTOR **pcas)
{
    VECTOR *safebags = NULL;    /*pkcs7's */
    VECTOR *bags = NULL;        /*pkcs12_Bag's */
    asn1_OctetString *keyid = NULL;
    char keymatch = 0;
    ppInt32 i, bagid;
    pkcs7 *p7;

    PDFTRY(lib){
        safebags = asn1_UnpackSequence(lib,
            p12->authSafe->contents.data->data, 
            p12->authSafe->contents.data->length, 
            (TDecodeFunction)pkcs7_Decode, (TDisposeFunction) pkcs7_Dispose);

        for (i = 0; i < vector_GetCount(safebags); i++) 
        {
            p7 = (pkcs7* )vector_GetValueAt(safebags, i);
            bagid = pkcs7_DataType(p7->contentType);
            if (bagid == ID_pkcs7Data)
            {
                bags = asn1_UnpackSequence(lib,
                    p7->contents.data->data, p7->contents.data->length, 
                    (TDecodeFunction)pkcs12_DecodeSafeBag, (TDisposeFunction)pkcs12_DisposeSafeBag);
            } 
            else if (bagid == ID_pkcs7EncryptedData) 
            {
                bags = pkcs12_DecryptAndDecode(lib,
                    p7->contents.encryptedData->enc_data->algorithm,
                    (TDecodeFunction)pkcs12_DecodeSafeBag, (TDisposeFunction)pkcs12_DisposeSafeBag, 
                    pwd, len, p7->contents.encryptedData->enc_data->enc_data, 3);
            } 
            else continue;

            if (bags == NULL) 
                PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));

            parse_bags(lib, bags, pwd, len, ppkey, pcert, pcas, &keyid, &keymatch);
            vector_Dispose(bags, (void (* )(PDFLibHandle, void * ))pkcs12_DisposeSafeBag);
            bags = NULL;
        }
        vector_Dispose(safebags, (void (* )(PDFLibHandle, void * ))pkcs7_Dispose);
        if (keyid) 
            asn1_DisposeOctetString(lib, keyid);
    } PDFEXCEPT(lib){
        if (safebags != NULL) 
            vector_Dispose(safebags, (void (* )(PDFLibHandle, void * ))pkcs7_Dispose);
        if (bags != NULL) 
            vector_Dispose(bags, (void (* )(PDFLibHandle, void * ))pkcs12_DisposeSafeBag);
        if (keyid) 
            asn1_DisposeOctetString(lib, keyid);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
}

void parse_bags(PDFLibHandle lib, VECTOR *bags, const char *pwd, ppInt32 len, 
                rsa_Key **ppkey, x509 **pcert, VECTOR **pcas, 
                asn1_OctetString **keyid, char *keymatch)
{
    ppInt32 i;
    for (i = 0; i < vector_GetCount(bags); i++) 
        parse_bag(lib, (pkcs12_SafeBag* )vector_GetValueAt(bags, i), pwd, len, 
            ppkey, pcert, pcas, keyid, keymatch);
}

#define MATCH_KEY  0x1
#define MATCH_CERT 0x2
#define MATCH_ALL  0x3

void parse_bag(PDFLibHandle lib, pkcs12_SafeBag *bag, const char *pwd, ppInt32 len, 
               rsa_Key **ppkey, x509 **pcert, VECTOR **pcas, 
               asn1_OctetString **keyid, char *keymatch)
{
    pkcs8_PrivateKeyInfo *p8;
    x509 *cert;
    asn1_OctetString *lkey = NULL;
    asn1_Type *attr;

    pkcs12_GetAttribute(bag->attrib, ID_pkcs9FriendlyName);
    /*
    attr = ...
    if (attr) fname = attr->value.bmpstring; */

    attr = pkcs12_GetAttribute(bag->attrib, ID_pkcs9LocalKeyID);
    if (attr)
        lkey = attr->value.octet_string;

    if (lkey && ((*keymatch) & MATCH_ALL) != MATCH_ALL) 
    {
        if (*keyid) 
        {
            if (asn1_CompareStrings(*keyid, lkey)) lkey = NULL;
        } 
        else 
            (*keyid) = (asn1_OctetString* )asn1_CloneString(lib, lkey);
    }
    
    switch (asn1_GetObjectID(bag->type))
    {
        case ID_keyBag:
            if (lkey == NULL || ppkey == NULL) return;
            (*ppkey) = rsa_UnShroudKey(lib, bag->value.keybag);
            (*keymatch) |= MATCH_KEY;
            break;
            
        case ID_pkcs8ShroudedKeyBag:
            if (lkey == NULL || ppkey == NULL) return;
            p8 = (pkcs8_PrivateKeyInfo* )pkcs12_DecryptAndDecode(lib,
                bag->value.shkeybag->digestAlgorithm, 
                (TDecodeFunction)pkcs8_DecodePrivateKeyInfo, 
                (TDisposeFunction)pkcs8_DisposePrivateKeyInfo,
                pwd, len, bag->value.shkeybag->digest, 2);
            (*ppkey) = rsa_UnShroudKey(lib, p8);
            (*keymatch) |= MATCH_KEY;
            pkcs8_DisposePrivateKeyInfo(lib, p8);
            break;

        case ID_certBag:
            if (asn1_GetObjectID(bag->value.bag->type) != ID_pkcs12CertTypeX509) return;
            cert = (x509* )asn1_UnpackString(lib, bag->value.bag->value.octet, (TDecodeFunction)x509_Decode);
            if (lkey) 
            {
                (*keymatch) |= MATCH_CERT;
                if (pcert) (*pcert) = cert;
                else x509_Dispose(lib, cert);
            }
            else
            {
                if (pcas != NULL && *pcas != NULL) 
                    vector_Push(*pcas, (char * )cert);
                else 
                    x509_Dispose(lib, cert);
            }
            break;

        case ID_safeContentsBag:
            parse_bags(lib, bag->value.safes, pwd, len, ppkey, pcert, pcas, keyid, keymatch);
            break;

        default:
            break;
    }
}

VECTOR *pkcs12_DecryptAndDecode(PDFLibHandle lib, x509_Algorithm *alg, 
                              TDecodeFunction dec_func, TDisposeFunction free_func, 
                              const char *pwd, ppInt32 plen, asn1_OctetString *in, ppInt32 seq)
{
    ppUns8 *out = NULL, *p;
    ppInt32 olen = 0;
    VECTOR *rslt = NULL;

    PDFTRY(lib) {
        pkcs12_PBE(lib, alg, pwd, plen, in->data, in->length, &out, &olen, 0);

        p = out;
        if (seq & 1) 
            rslt = asn1_DecodeSet(lib, NULL, &p, olen, dec_func, free_func, SEQUENCE_TAG, UNIVERSAL_CLS);
        else
            rslt = (VECTOR *)dec_func(lib, NULL, &p, olen);
    
        if (out != NULL) 
        {
            if (seq & 2) memset(out, 0, olen);
            mfree(lib, out);
        }
    } PDFEXCEPT(lib){
        if (out) 
        {
            if (seq & 2) memset(out, 0, olen);
            mfree(lib, out);
        }
        PDFRERAISE(lib);
    }PDFTRYEND(lib);
    return rslt;
}


/*
ppInt32 pkcs12_EncodeMacData(pkcs12_MacData *dat, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    
    if (dat == NULL) return 0;
    rslt += pkcs7_EncodeDigestInfo(dat->mac, NULL);
    rslt += asn1_EncodeOctetString(dat->macSalt, NULL);
    rslt += asn1_EncodeInteger(dat->iterations, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    pkcs7_EncodeDigestInfo(dat->mac, &p);
    asn1_EncodeOctetString(dat->macSalt, &p);
    asn1_EncodeInteger(dat->iterations, &p);
    (*pbuf) = p;
    return r;
}

ppInt32 pkcs12_Encode(pkcs12 *obj, ppUns8 **pbuf)
{
    ppInt32 r = 0, rslt = 0;
    ppUns8 *p;

    if (obj == NULL) return 0;
    rslt += asn1_EncodeInteger(obj->version, NULL);
    rslt += pkcs7_Encode(obj->authSafe, NULL);
    rslt += pkcs12_EncodeMacData(obj->macData, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(obj->version, &p);
    pkcs7_Encode(obj->authSafe, &p);
    pkcs12_EncodeMacData(obj->macData, &p);
    (*pbuf) = p;
    return r;
}

ppInt32 pkcs12_EncodeBag(pkcs12_Bag *bag, ppUns8 **pbuf)
{
    ppInt32 id, v = 0;
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (bag == NULL) return 0;

    id = pkcs12_BagContentType(bag->type);
    rslt += asn1_EncodeObject(bag->type, NULL);
    switch (id)
    {
        case ID_pkcs12CertTypeX509:
            if (bag->value.x509cert != NULL)
            {
                v = asn1_EncodeOctetString(bag->value.x509cert, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        case ID_pkcs12CRLTypeX509:
            if (bag->value.x509crl != NULL)
            {
                v = asn1_EncodeOctetString(bag->value.x509crl, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        case ID_pkcs12CertTypeSDSI:
            if (bag->value.sdsicert != NULL)
            {
                v = asn1_EncodeIA5String(bag->value.sdsicert, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        default:
            if (bag->value.other != NULL)
            {
                v = asn1_EncodeType(bag->value.other, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
    }
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;
    
    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(bag->type, &p);
    switch (id) 
    {
        case ID_pkcs12CertTypeX509:
            if (bag->value.x509cert != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                asn1_EncodeOctetString(bag->value.x509cert, &p);
            }
            break;
        case ID_pkcs12CRLTypeX509:
            if (bag->value.x509crl != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                asn1_EncodeOctetString(bag->value.x509crl, &p);
            }
            break;
        case ID_pkcs12CertTypeSDSI:
            if (bag->value.sdsicert != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                asn1_EncodeIA5String(bag->value.sdsicert, &p);
            }
            break;
        default:
            if (bag->value.other != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                asn1_EncodeType(bag->value.other, &p);
            }
            break;
    }
    (*pbuf) = p;
    return r;
}

ppInt32 pkcs8_EncodePrivateKeyInfo(pkcs8_PKI *pki, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (pki == NULL) return 0;
    rslt += asn1_EncodeInteger(pki->version, NULL);
    rslt += x509_EncodeAlgorithm(pki->pkeyalg, NULL);
    rslt += asn1_EncodeType(pki->pkey, NULL);
    if (pki->attributes && vector_GetCount(pki->attributes)) 
        rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Attribute*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))
            asn1_EncodeSet)(pki->attributes, NULL, x509_EncodeAttribute, 0, CTX_SPECIFIC_CLS, IS_SET);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;
    
    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(pki->version, &p);
    x509_EncodeAlgorithm(pki->pkeyalg, &p);
    asn1_EncodeType(pki->pkey, &p);
    if (pki->attributes && vector_GetCount(pki->attributes)) 
        ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Attribute*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))
            asn1_EncodeSet)(pki->attributes, &p, x509_EncodeAttribute, 0, CTX_SPECIFIC_CLS, IS_SET);
    (*pbuf) = p;
    return r;
}

ppInt32 pkcs12_EncodeSafeBag(pkcs12_SafeBag *bag, ppUns8 **pbuf)
{
    ppInt32 id, v = 0;
    ppInt32 r, rslt = 0;
    ppUns8 *p;

    if (bag == NULL) return 0;
    id = pkcs12_BagType(bag->type);
    rslt += asn1_EncodeObject(bag->type, NULL);
    switch (id) 
    {
        case ID_keyBag:
            if (bag->value.keybag != NULL)
            {
                v = pkcs8_EncodePrivateKeyInfo(bag->value.keybag, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        case ID_pkcs8ShroudedKeyBag:
            if (bag->value.shkeybag != NULL)
            {
                v = pkcs7_EncodeDigestInfo(bag->value.shkeybag, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        case ID_safeContentsBag:
            if (bag->value.safes != NULL && vector_GetCount(bag->value.safes) != 0)
            {
                v = ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs12_SafeBag*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
                    (bag->value.safes, NULL, pkcs12_EncodeSafeBag, 
                    SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        case ID_certBag:
        case ID_crlBag:
        case ID_secretBag:
            if (bag->value.bag != NULL)
            {
                v = pkcs12_EncodeBag(bag->value.bag, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
        default:
            if (bag->value.bag != NULL)
            {
                v = asn1_EncodeType(bag->value.other, NULL);
                rslt += asn1_ObjectSize(1, v, 0);
            }
            break;
    }
    rslt += ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Attribute*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
        (bag->attrib, NULL, x509_EncodeAttribute, SET_TAG, UNIVERSAL_CLS, IS_SET);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;
    
    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeObject(bag->type, &p);
    switch (id) 
    {
        case ID_keyBag:
            if (bag->value.keybag != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                pkcs8_EncodePrivateKeyInfo(bag->value.keybag, &p);
            }
            break;
        case ID_pkcs8ShroudedKeyBag:
            if (bag->value.shkeybag != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                pkcs7_EncodeDigestInfo(bag->value.shkeybag, &p);
            }
            break;
        case ID_safeContentsBag:
            if (bag->value.safes != NULL && vector_GetCount(bag->value.safes) != 0)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(pkcs12_SafeBag*, ppUns8 ** ), ppInt32 , ppInt32 , ppInt32))asn1_EncodeSet)
                    (bag->value.safes, &p, pkcs12_EncodeSafeBag, SEQUENCE_TAG, UNIVERSAL_CLS, IS_SEQUENCE);
            }
            break;
        case ID_certBag:
        case ID_crlBag:
        case ID_secretBag:
            if (bag->value.bag != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                pkcs12_EncodeBag(bag->value.bag, &p);
            }
            break;
        default:
            if (bag->value.other != NULL)
            {
                asn1_PutObject(&p, 1, v, 0, CTX_SPECIFIC_CLS);
                asn1_EncodeType(bag->value.other, &p);
            }
            break;
    }
    ((ppInt32 (* )(VECTOR*, ppUns8 **, ppInt32 (* )(x509_Attribute*, ppUns8 ** ), ppInt32, ppInt32, ppInt32))asn1_EncodeSet)
        (bag->attrib, &p, x509_EncodeAttribute, SET_TAG, UNIVERSAL_CLS, IS_SET);
    (*pbuf) = p;
    return r;
}
*/
#endif