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
VSCCITT.c
*********************************************************************/

#include "VSTypes.h"
#include "VSImageI.h"
#include "VSMisc.h"

typedef struct tableentry {
    unsigned short length;
    unsigned short code;
    short	runlen;
} tableentry;



static const tableentry horizcode =
    { 3, 0x1 };
static const tableentry passcode =
    { 4, 0x1 };
static const tableentry vcodes [ 7 ] = {
    { 7, 0x03 },
    { 6, 0x03 },
    { 3, 0x03 },
    { 1, 0x1 },
    { 3, 0x2 },
    { 6, 0x02 },
    { 7, 0x02 }
}; 


static const ppUns8 zeroruns[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	/* 0x00 - 0x0f */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0x10 - 0x1f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x20 - 0x2f */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x30 - 0x3f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x40 - 0x4f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x50 - 0x5f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x60 - 0x6f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x70 - 0x7f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 - 0x8f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 - 0x9f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 - 0xaf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 - 0xbf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 - 0xcf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 - 0xdf */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 - 0xef */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xf0 - 0xff */
};
static const ppUns8 oneruns[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00 - 0x0f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 - 0x1f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x20 - 0x2f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x30 - 0x3f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 - 0x4f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x50 - 0x5f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 - 0x6f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 - 0x7f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x80 - 0x8f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x90 - 0x9f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xa0 - 0xaf */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xb0 - 0xbf */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xc0 - 0xcf */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xd0 - 0xdf */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0xe0 - 0xef */
        4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	/* 0xf0 - 0xff */
};


#define	EOL	0x001
#define	G3CODE_EOL	-1
#define	G3CODE_INVALID	-2
#define	G3CODE_EOF	-3
#define	G3CODE_INCOMP	-4

static tableentry TIFFWhiteCodes[] = {
    { 8, 0x35, 0 },	/* 0011 0101 */
    { 6, 0x7, 1 },	/* 0001 11 */
    { 4, 0x7, 2 },	/* 0111 */
    { 4, 0x8, 3 },	/* 1000 */
    { 4, 0xB, 4 },	/* 1011 */
    { 4, 0xC, 5 },	/* 1100 */
    { 4, 0xE, 6 },	/* 1110 */
    { 4, 0xF, 7 },	/* 1111 */
    { 5, 0x13, 8 },	/* 1001 1 */
    { 5, 0x14, 9 },	/* 1010 0 */
    { 5, 0x7, 10 },	/* 0011 1 */
    { 5, 0x8, 11 },	/* 0100 0 */
    { 6, 0x8, 12 },	/* 0010 00 */
    { 6, 0x3, 13 },	/* 0000 11 */
    { 6, 0x34, 14 },	/* 1101 00 */
    { 6, 0x35, 15 },	/* 1101 01 */
    { 6, 0x2A, 16 },	/* 1010 10 */
    { 6, 0x2B, 17 },	/* 1010 11 */
    { 7, 0x27, 18 },	/* 0100 111 */
    { 7, 0xC, 19 },	/* 0001 100 */
    { 7, 0x8, 20 },	/* 0001 000 */
    { 7, 0x17, 21 },	/* 0010 111 */
    { 7, 0x3, 22 },	/* 0000 011 */
    { 7, 0x4, 23 },	/* 0000 100 */
    { 7, 0x28, 24 },	/* 0101 000 */
    { 7, 0x2B, 25 },	/* 0101 011 */
    { 7, 0x13, 26 },	/* 0010 011 */
    { 7, 0x24, 27 },	/* 0100 100 */
    { 7, 0x18, 28 },	/* 0011 000 */
    { 8, 0x2, 29 },	/* 0000 0010 */
    { 8, 0x3, 30 },	/* 0000 0011 */
    { 8, 0x1A, 31 },	/* 0001 1010 */
    { 8, 0x1B, 32 },	/* 0001 1011 */
    { 8, 0x12, 33 },	/* 0001 0010 */
    { 8, 0x13, 34 },	/* 0001 0011 */
    { 8, 0x14, 35 },	/* 0001 0100 */
    { 8, 0x15, 36 },	/* 0001 0101 */
    { 8, 0x16, 37 },	/* 0001 0110 */
    { 8, 0x17, 38 },	/* 0001 0111 */
    { 8, 0x28, 39 },	/* 0010 1000 */
    { 8, 0x29, 40 },	/* 0010 1001 */
    { 8, 0x2A, 41 },	/* 0010 1010 */
    { 8, 0x2B, 42 },	/* 0010 1011 */
    { 8, 0x2C, 43 },	/* 0010 1100 */
    { 8, 0x2D, 44 },	/* 0010 1101 */
    { 8, 0x4, 45 },	/* 0000 0100 */
    { 8, 0x5, 46 },	/* 0000 0101 */
    { 8, 0xA, 47 },	/* 0000 1010 */
    { 8, 0xB, 48 },	/* 0000 1011 */
    { 8, 0x52, 49 },	/* 0101 0010 */
    { 8, 0x53, 50 },	/* 0101 0011 */
    { 8, 0x54, 51 },	/* 0101 0100 */
    { 8, 0x55, 52 },	/* 0101 0101 */
    { 8, 0x24, 53 },	/* 0010 0100 */
    { 8, 0x25, 54 },	/* 0010 0101 */
    { 8, 0x58, 55 },	/* 0101 1000 */
    { 8, 0x59, 56 },	/* 0101 1001 */
    { 8, 0x5A, 57 },	/* 0101 1010 */
    { 8, 0x5B, 58 },	/* 0101 1011 */
    { 8, 0x4A, 59 },	/* 0100 1010 */
    { 8, 0x4B, 60 },	/* 0100 1011 */
    { 8, 0x32, 61 },	/* 0011 0010 */
    { 8, 0x33, 62 },	/* 0011 0011 */
    { 8, 0x34, 63 },	/* 0011 0100 */
    { 5, 0x1B, 64 },	/* 1101 1 */
    { 5, 0x12, 128 },	/* 1001 0 */
    { 6, 0x17, 192 },	/* 0101 11 */
    { 7, 0x37, 256 },	/* 0110 111 */
    { 8, 0x36, 320 },	/* 0011 0110 */
    { 8, 0x37, 384 },	/* 0011 0111 */
    { 8, 0x64, 448 },	/* 0110 0100 */
    { 8, 0x65, 512 },	/* 0110 0101 */
    { 8, 0x68, 576 },	/* 0110 1000 */
    { 8, 0x67, 640 },	/* 0110 0111 */
    { 9, 0xCC, 704 },	/* 0110 0110 0 */
    { 9, 0xCD, 768 },	/* 0110 0110 1 */
    { 9, 0xD2, 832 },	/* 0110 1001 0 */
    { 9, 0xD3, 896 },	/* 0110 1001 1 */
    { 9, 0xD4, 960 },	/* 0110 1010 0 */
    { 9, 0xD5, 1024 },	/* 0110 1010 1 */
    { 9, 0xD6, 1088 },	/* 0110 1011 0 */
    { 9, 0xD7, 1152 },	/* 0110 1011 1 */
    { 9, 0xD8, 1216 },	/* 0110 1100 0 */
    { 9, 0xD9, 1280 },	/* 0110 1100 1 */
    { 9, 0xDA, 1344 },	/* 0110 1101 0 */
    { 9, 0xDB, 1408 },	/* 0110 1101 1 */
    { 9, 0x98, 1472 },	/* 0100 1100 0 */
    { 9, 0x99, 1536 },	/* 0100 1100 1 */
    { 9, 0x9A, 1600 },	/* 0100 1101 0 */
    { 6, 0x18, 1664 },	/* 0110 00 */
    { 9, 0x9B, 1728 },	/* 0100 1101 1 */
    { 11, 0x8, 1792 },	/* 0000 0001 000 */
    { 11, 0xC, 1856 },	/* 0000 0001 100 */
    { 11, 0xD, 1920 },	/* 0000 0001 101 */
    { 12, 0x12, 1984 },	/* 0000 0001 0010 */
    { 12, 0x13, 2048 },	/* 0000 0001 0011 */
    { 12, 0x14, 2112 },	/* 0000 0001 0100 */
    { 12, 0x15, 2176 },	/* 0000 0001 0101 */
    { 12, 0x16, 2240 },	/* 0000 0001 0110 */
    { 12, 0x17, 2304 },	/* 0000 0001 0111 */
    { 12, 0x1C, 2368 },	/* 0000 0001 1100 */
    { 12, 0x1D, 2432 },	/* 0000 0001 1101 */
    { 12, 0x1E, 2496 },	/* 0000 0001 1110 */
    { 12, 0x1F, 2560 },	/* 0000 0001 1111 */
    { 12, 0x1, G3CODE_EOL },	/* 0000 0000 0001 */
    { 9, 0x1, G3CODE_INVALID },	/* 0000 0000 1 */
    { 10, 0x1, G3CODE_INVALID },	/* 0000 0000 01 */
    { 11, 0x1, G3CODE_INVALID },	/* 0000 0000 001 */
    { 12, 0x0, G3CODE_INVALID },	/* 0000 0000 0000 */
};

static tableentry TIFFBlackCodes[] = {
    { 10, 0x37, 0 },	/* 0000 1101 11 */
    { 3, 0x2, 1 },	/* 010 */
    { 2, 0x3, 2 },	/* 11 */
    { 2, 0x2, 3 },	/* 10 */
    { 3, 0x3, 4 },	/* 011 */
    { 4, 0x3, 5 },	/* 0011 */
    { 4, 0x2, 6 },	/* 0010 */
    { 5, 0x3, 7 },	/* 0001 1 */
    { 6, 0x5, 8 },	/* 0001 01 */
    { 6, 0x4, 9 },	/* 0001 00 */
    { 7, 0x4, 10 },	/* 0000 100 */
    { 7, 0x5, 11 },	/* 0000 101 */
    { 7, 0x7, 12 },	/* 0000 111 */
    { 8, 0x4, 13 },	/* 0000 0100 */
    { 8, 0x7, 14 },	/* 0000 0111 */
    { 9, 0x18, 15 },	/* 0000 1100 0 */
    { 10, 0x17, 16 },	/* 0000 0101 11 */
    { 10, 0x18, 17 },	/* 0000 0110 00 */
    { 10, 0x8, 18 },	/* 0000 0010 00 */
    { 11, 0x67, 19 },	/* 0000 1100 111 */
    { 11, 0x68, 20 },	/* 0000 1101 000 */
    { 11, 0x6C, 21 },	/* 0000 1101 100 */
    { 11, 0x37, 22 },	/* 0000 0110 111 */
    { 11, 0x28, 23 },	/* 0000 0101 000 */
    { 11, 0x17, 24 },	/* 0000 0010 111 */
    { 11, 0x18, 25 },	/* 0000 0011 000 */
    { 12, 0xCA, 26 },	/* 0000 1100 1010 */
    { 12, 0xCB, 27 },	/* 0000 1100 1011 */
    { 12, 0xCC, 28 },	/* 0000 1100 1100 */
    { 12, 0xCD, 29 },	/* 0000 1100 1101 */
    { 12, 0x68, 30 },	/* 0000 0110 1000 */
    { 12, 0x69, 31 },	/* 0000 0110 1001 */
    { 12, 0x6A, 32 },	/* 0000 0110 1010 */
    { 12, 0x6B, 33 },	/* 0000 0110 1011 */
    { 12, 0xD2, 34 },	/* 0000 1101 0010 */
    { 12, 0xD3, 35 },	/* 0000 1101 0011 */
    { 12, 0xD4, 36 },	/* 0000 1101 0100 */
    { 12, 0xD5, 37 },	/* 0000 1101 0101 */
    { 12, 0xD6, 38 },	/* 0000 1101 0110 */
    { 12, 0xD7, 39 },	/* 0000 1101 0111 */
    { 12, 0x6C, 40 },	/* 0000 0110 1100 */
    { 12, 0x6D, 41 },	/* 0000 0110 1101 */
    { 12, 0xDA, 42 },	/* 0000 1101 1010 */
    { 12, 0xDB, 43 },	/* 0000 1101 1011 */
    { 12, 0x54, 44 },	/* 0000 0101 0100 */
    { 12, 0x55, 45 },	/* 0000 0101 0101 */
    { 12, 0x56, 46 },	/* 0000 0101 0110 */
    { 12, 0x57, 47 },	/* 0000 0101 0111 */
    { 12, 0x64, 48 },	/* 0000 0110 0100 */
    { 12, 0x65, 49 },	/* 0000 0110 0101 */
    { 12, 0x52, 50 },	/* 0000 0101 0010 */
    { 12, 0x53, 51 },	/* 0000 0101 0011 */
    { 12, 0x24, 52 },	/* 0000 0010 0100 */
    { 12, 0x37, 53 },	/* 0000 0011 0111 */
    { 12, 0x38, 54 },	/* 0000 0011 1000 */
    { 12, 0x27, 55 },	/* 0000 0010 0111 */
    { 12, 0x28, 56 },	/* 0000 0010 1000 */
    { 12, 0x58, 57 },	/* 0000 0101 1000 */
    { 12, 0x59, 58 },	/* 0000 0101 1001 */
    { 12, 0x2B, 59 },	/* 0000 0010 1011 */
    { 12, 0x2C, 60 },	/* 0000 0010 1100 */
    { 12, 0x5A, 61 },	/* 0000 0101 1010 */
    { 12, 0x66, 62 },	/* 0000 0110 0110 */
    { 12, 0x67, 63 },	/* 0000 0110 0111 */
    { 10, 0xF, 64 },	/* 0000 0011 11 */
    { 12, 0xC8, 128 },	/* 0000 1100 1000 */
    { 12, 0xC9, 192 },	/* 0000 1100 1001 */
    { 12, 0x5B, 256 },	/* 0000 0101 1011 */
    { 12, 0x33, 320 },	/* 0000 0011 0011 */
    { 12, 0x34, 384 },	/* 0000 0011 0100 */
    { 12, 0x35, 448 },	/* 0000 0011 0101 */
    { 13, 0x6C, 512 },	/* 0000 0011 0110 0 */
    { 13, 0x6D, 576 },	/* 0000 0011 0110 1 */
    { 13, 0x4A, 640 },	/* 0000 0010 0101 0 */
    { 13, 0x4B, 704 },	/* 0000 0010 0101 1 */
    { 13, 0x4C, 768 },	/* 0000 0010 0110 0 */
    { 13, 0x4D, 832 },	/* 0000 0010 0110 1 */
    { 13, 0x72, 896 },	/* 0000 0011 1001 0 */
    { 13, 0x73, 960 },	/* 0000 0011 1001 1 */
    { 13, 0x74, 1024 },	/* 0000 0011 1010 0 */
    { 13, 0x75, 1088 },	/* 0000 0011 1010 1 */
    { 13, 0x76, 1152 },	/* 0000 0011 1011 0 */
    { 13, 0x77, 1216 },	/* 0000 0011 1011 1 */
    { 13, 0x52, 1280 },	/* 0000 0010 1001 0 */
    { 13, 0x53, 1344 },	/* 0000 0010 1001 1 */
    { 13, 0x54, 1408 },	/* 0000 0010 1010 0 */
    { 13, 0x55, 1472 },	/* 0000 0010 1010 1 */
    { 13, 0x5A, 1536 },	/* 0000 0010 1101 0 */
    { 13, 0x5B, 1600 },	/* 0000 0010 1101 1 */
    { 13, 0x64, 1664 },	/* 0000 0011 0010 0 */
    { 13, 0x65, 1728 },	/* 0000 0011 0010 1 */
    { 11, 0x8, 1792 },	/* 0000 0001 000 */
    { 11, 0xC, 1856 },	/* 0000 0001 100 */
    { 11, 0xD, 1920 },	/* 0000 0001 101 */
    { 12, 0x12, 1984 },	/* 0000 0001 0010 */
    { 12, 0x13, 2048 },	/* 0000 0001 0011 */
    { 12, 0x14, 2112 },	/* 0000 0001 0100 */
    { 12, 0x15, 2176 },	/* 0000 0001 0101 */
    { 12, 0x16, 2240 },	/* 0000 0001 0110 */
    { 12, 0x17, 2304 },	/* 0000 0001 0111 */
    { 12, 0x1C, 2368 },	/* 0000 0001 1100 */
    { 12, 0x1D, 2432 },	/* 0000 0001 1101 */
    { 12, 0x1E, 2496 },	/* 0000 0001 1110 */
    { 12, 0x1F, 2560 },	/* 0000 0001 1111 */
    { 12, 0x1, G3CODE_EOL },	/* 0000 0000 0001 */
    { 9, 0x1, G3CODE_INVALID },	/* 0000 0000 1 */
    { 10, 0x1, G3CODE_INVALID },	/* 0000 0000 01 */
    { 11, 0x1, G3CODE_INVALID },	/* 0000 0000 001 */
    { 12, 0x0, G3CODE_INVALID },	/* 0000 0000 0000 */
};


#define putcode( te ) ULBitStreamPutBits( AStream, ( te )->code, ( te )->length )

#define	isAligned( p,t ) ( ( ( ( unsigned long )( p )) & ( sizeof ( t )-1 ) ) == 0 )

#define	finddiff( _cp, _bs, _be, _color )	\
	( _bs + ( _color ? PDFCCITTFind1Span( _cp,_bs,_be ) : PDFCCITTFind0Span( _cp,_bs,_be ) ) )

#define	finddiff2( _cp, _bs, _be, _color ) \
	( _bs < _be ? finddiff( _cp,_bs,_be,_color ) : _be )



ppInt32 PDFCCITTFind0Span( ppUns8* bp, ppInt32 bs, ppInt32 be )
{
    ppInt32 bits = be - bs;
    ppInt32 n, span;
    bp += bs >> 3;
	n = ( bs & 7 );
    if ( ( bits > 0 ) && n ) {
        span = zeroruns[ ( *bp << n ) & 0xff];
        if ( span > 8-n )
            span = 8-n;
        if ( span > bits )
            span = bits;
        if ( n+span < 8 )
            return ( span );
        bits -= span;
        bp++;
    } else
        span = 0;
    if ( bits >= (ppInt32)(2*8*sizeof ( long )) ) {
        long* lp;
        while ( !isAligned( bp, long ) ) {
            if ( *bp != 0x00 )
                return ( span + zeroruns[ *bp ] );
            span += 8, bits -= 8;
            bp++;
        }
        lp = ( long* ) bp;
        while ( ( bits >= (ppInt32)(8*sizeof ( long )) ) && ( *lp == 0 ) ) {
            span += 8*sizeof ( long ), bits -= 8*sizeof ( long );
            lp++;
        }
        bp = ( ppUns8* ) lp;
    }
    while ( bits >= 8 ) {
        if ( *bp != 0x00 )
            return ( span + zeroruns[ *bp ] );
        span += 8, bits -= 8;
        bp++;
    }
    if ( bits > 0 ) {
        n = zeroruns[ *bp ];
        span += ( n > bits ? bits : n );
    }
    return ( span );
}



ppInt32 PDFCCITTFind1Span( ppUns8* bp, ppInt32 bs, ppInt32 be )
{
    ppInt32 bits = be - bs;
    ppInt32 n, span;
    bp += bs>>3;
	n = ( bs & 7 );
    if (bits > 0 &&  n  )
    {
        span = oneruns[ ( *bp << n ) & 0xff ];
        if ( span > 8-n )
            span = 8-n;
        if ( span > bits )
            span = bits;
        if ( n+span < 8 )
            return ( span );
        bits -= span;
        bp++;
    } else
        span = 0;
    if ( bits >= (ppInt32)(2*8*sizeof ( long )) ) {
        long* lp;
        while ( !isAligned( bp, long ) ) {
            if ( *bp != 0xff )
                return ( span + oneruns[ *bp ] );
            span += 8, bits -= 8;
            bp++;
        }
        lp = ( long* ) bp;
        while ( (bits >= (ppInt32)(8*sizeof ( long ))) && (*lp == ~0) ) {
            span += 8*sizeof ( long ), bits -= 8*sizeof ( long );
            lp++;
        }
        bp = ( ppUns8* ) lp;
    }
    while ( bits >= 8 ) {
        if ( *bp != 0xff )
            return ( span + oneruns[*bp] );
        span += 8, bits -= 8;
        bp++;
    }
    if ( bits > 0 ) {
        n = oneruns[ *bp ];
        span += ( n > bits ? bits : n );
    }
    return ( span );
}

void PDFCCITTPutSpan ( ppInt32 span, tableentry* tab, ULBitStream * AStream )
{
    ppUns32 code, length;
    while ( ((ppUns32)span) >= ((ppUns32)2624) ) {
        tableentry* te = &tab[ 63 + ( 2560 >> 6 ) ];
        code = te->code, length = te->length;
        ULBitStreamPutBits( AStream, code, length );
        span -= te->runlen;
    }
    if ( ((ppUns32)span) >= ((ppUns32)64) ) {
        tableentry* te = &tab[ 63 + ( span >> 6 ) ];
        code = te->code, length = te->length;
        ULBitStreamPutBits( AStream, code, length );
        span -= te->runlen;
    }
    code = tab[ span ].code, length = tab[ span ].length;
    ULBitStreamPutBits( AStream, code, length );
}


void PDFCCITTEncode2DRow( ULBitStream * AStream, ppUns8* bp, ppUns8* rp, ppUns32 bits )
{
#define	PIXEL( buf,ix )	( ( ( ( buf )[( ix ) >> 3 ]) >> ( 7 - (( ix ) & 7 ))) & 1)
        ppUns32 a0 = 0;
	ppUns32 a1 = ( PIXEL( bp, 0 ) != 0 ? 0 : finddiff( bp, 0, bits, 0 ) );
	ppUns32 b1 = ( PIXEL( rp, 0 ) != 0 ? 0 : finddiff( rp, 0, bits, 0 ) );
	ppUns32 a2, b2;
	for ( ;; ) {
		b2 = finddiff2( rp, b1, bits, PIXEL( rp,b1 ) );
		if ( b2 >= a1 ) {
			ppInt32 d = b1 - a1;
			if ( !( -1 <= d && d <= 3 ) ) {
				a2 = finddiff2( bp, a1, bits, PIXEL(bp,a1));
				putcode( &horizcode );
                if ( a0+a1 == 0 || PIXEL( bp, a0 ) == 0 ) {
                    PDFCCITTPutSpan( a1-a0, TIFFWhiteCodes, AStream );
                    PDFCCITTPutSpan( a2-a1, TIFFBlackCodes, AStream );
                } else {
                    PDFCCITTPutSpan( a1-a0, TIFFBlackCodes, AStream );
                    PDFCCITTPutSpan( a2-a1, TIFFWhiteCodes, AStream );
                }
				a0 = a2;
			} else {
				putcode( &vcodes[ d+3 ] );
				a0 = a1;
			}
		} else {
			putcode( &passcode );
			a0 = b2;
		}
		if ( a0 >= bits )
			break;
		a1 = finddiff( bp, a0, bits, PIXEL( bp,a0 ) );
		b1 = finddiff( rp, a0, bits, !PIXEL( bp,a0 ) );
		b1 = finddiff( rp, b1, bits, PIXEL( bp,a0 ) );
	}
#undef PIXEL
}


void PDFImageCompressCCITT (  PMImage Image, PDFStreamHandle AStream)
{
    PppUns8 P, L, M;
	PppUns8 Refline; 
    PDFLibHandle Lib = _StreamLib ( AStream );
	int RowPixels;
	ppUns32 RowBytes;
	ppUns32 i, j;
    ULBitStream Bits;
    if ( ( Image->Width == 0 ) || ( Image->Height == 0 ) )
	    return;
    RowPixels = Image->Width;
    RowBytes = ( Image->Width + 7 ) >> 3;
    P = ( PppUns8 )mmalloc( Lib, RowBytes );
    Refline = ( ppUns8 * ) mmalloc( Lib, RowBytes );
    memset( Refline, 0, RowBytes);  
    ULBitStrmInit ( AStream, Bits );
    for ( i=0;i<Image->Height;i++ )
    {
       L = P;
       M = ScanLine( Image, i );
       for ( j=0; j<RowBytes; j++ )
       {
         *L = ~( *M );
         L++;
         M++;
       }
       PDFCCITTEncode2DRow( &Bits, P, Refline, RowPixels );
            memcpy( Refline, P, RowBytes );
    }
    ULBitStreamFlushWithCheck ( Bits );
    if ( Refline )
        mfree(Lib, Refline );
    if ( P )
        mfree( Lib, P );
}

