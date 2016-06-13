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
VSLibA.c
*********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../VSLibA.h"
#include "VSLibI.h"
#include "VSAssert.h"
#include "../VSBaseA.h"

#ifdef LICENSED
#include <time.h>
#define RELEASEDATE "22.02.2008"
typedef unsigned char ppUns8;

extern int HFE_MsgLength;
extern int HFE_SigLength;
extern ppUns8 HFE_PubKey[];

#ifdef PHP
#	define KEYINITED
#	include "hfekey_php.h"
#endif
#ifdef PERL
#	define KEYINITED
#	include "hfekey_perl.h"
#endif
#ifndef KEYINITED
#	include "hfekey.h"
#endif

#ifndef PDFPLATFORMID
#define PDFPLATFORMID 1
#endif

typedef struct TIntDig{
	ppUns8 *Int2Dig;
	int NumDig;
	int Dig2Int [256 ];
} TIntDig, * PIntDig;

typedef struct TRC4Hash{
	ppUns8 RC4_State[256];
	int  RC4_I, RC4_J;
} TRC4Hash, *PRC4Hash;

int StrToDate(char* str)
{
	int i;
	time_t tval;
	struct tm val, off;

	if (str[0] == '\0') return 0;

	if (strlen(str) != 10) return -1;
	val.tm_hour = val.tm_min = val.tm_sec = 0;
	val.tm_mday = val.tm_mon = val.tm_year = 0;
	val.tm_isdst = 0;
	for (i = 0; i < 10; i++)
	{
		if (i == 2 || i == 5)
		{
			if (str[i] != '.') return -1;
		}
		else
		{
			if (str[i] < '0' || str[i] > '9') return -1;
			if (i < 2) val.tm_mday = val.tm_mday*10 + (str[i] - '0');
			else if (i < 5) val.tm_mon = val.tm_mon*10 + (str[i] - '0');
			else val.tm_year = val.tm_year*10 + (str[i] - '0');
		}
	}

	val.tm_mon--; val.tm_year -= 1900;
	if ((tval = mktime(&val)) == -1) return -1;

	off.tm_year = 104; 
	off.tm_mon  = 11;
	off.tm_mday = 31;
	off.tm_hour = 0;
	off.tm_min  = 0;
	off.tm_sec  = 0;
	off.tm_isdst = 0;
	return (int)((tval - mktime(&off))/86400L);
}

int UnpackDate(ppUns8* vector, int pos)
{
	int i, dt, rpos;
	ppUns8 mask;
	/*vector.length >= pos + 12 && pos >= 0*/
	dt = 0;
	for (i = 0; i < 12; i++)
	{
		mask = (ppUns8)(1 << ((int)(pos + 11 - i) % 8));
		rpos = (int)((pos + 11 - i) / 8);
		dt = (dt << 1) | ((vector[rpos] & mask) != 0 ? 1 : 0);
	}
	return dt;
}

unsigned long UnpackDWord(ppUns8* vector, int pos)
{
	int i, rpos;
	unsigned long dw;
	ppUns8 mask;
	/*vector.length >= pos + 32 && pos >= 0*/
	dw = 0L;
	for (i = 0; i < 32; i++)
	{
		mask = (ppUns8)(1 << ((int)(pos + 31 - i) % 8));
		rpos = (int)((pos + 31 - i) / 8);
		dw = (dw << 1) | ((vector[rpos] & mask) != 0 ? 1 : 0);
	}
	return dw;
}

unsigned char UnpackUChar(ppUns8* vector, int pos)
{
	int i, rpos;
	unsigned char uc;
	ppUns8 mask;
	/*pos >= 0 && pos <= vector.length - 7*/
	uc = 0;
	for (i = 0; i < 7; i++)
	{
		mask = (ppUns8)(1 << ((int)(pos + 6 - i) % 8));
		rpos = (int)((pos + 6 - i) / 8);
		uc = (uc << 1) | ((vector[rpos] & mask) != 0 ? 1 : 0);
	}
	return uc;
}

/* === RC4 Hash === */


#define RC4_Init LLRC4_Init 
void RC4_Init( ppUns8 *str, int len, PRC4Hash H)
{
	int i, j;
	ppUns8 k[256], t;
	ppUns8 *p = str;
	if (len < 0) len = (int)strlen((char *) str);
	for (i = 0, j = len; i < 256; i++)
	{
		H->RC4_State[i] = (ppUns8)i;
		k[i] =(ppUns8)( (i + 13) & 0xFF );
		if (len > 0)
		{
			k[i] =(ppUns8)( k[i] ^ (len + (ppUns8)*p));
			if (--j == 0)
			{
				p = str;
				j = len;
			}
			else
				p++;
		}
	}

	for (H->RC4_I = H->RC4_J = i = j = 0; i < 256; i++)
	{
		j = (j + H->RC4_State[i] + k[i]) & 0xFF;
		t = H->RC4_State[i];
		H->RC4_State[i] = H->RC4_State[j];
		H->RC4_State[j] = t;
	}
}
#define RC4_GetppUns8 LLRC4_GetppUns8 
ppUns8 RC4_GetppUns8(PRC4Hash H )
{
	ppUns8 t;
	H->RC4_I = (H->RC4_I + 1) & 0xFF;
	H->RC4_J = (H->RC4_J + H->RC4_State[H->RC4_I]) & 0xFF;
	t = H->RC4_State[H->RC4_I];
	H->RC4_State[H->RC4_I] = H->RC4_State[H->RC4_J];
	H->RC4_State[H->RC4_J] = t;
	return H->RC4_State[(H->RC4_State[H->RC4_I] + H->RC4_State[H->RC4_J]) & 0xFF];
}

#define TrimString LLTrimString 
void TrimString( ppUns8 *src, ppUns8 *dest)
{
	while (*src)
	{
		if (*src > ' ') *dest++ = (ppUns8)toupper(*src);
		src++;
	}
	*dest = '\0';
}

#define RC4_ComputeHash LLRC4_ComputeHash 
void RC4_ComputeHash(ppUns8* message, ppUns8* hash, PRC4Hash H)
{
	ppUns8 msg[256];
	ppUns8 hsh[256];
	int i, j;

	TrimString(message, msg);

	RC4_Init(msg, -1, H);
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
			hsh[j] = RC4_GetppUns8(H);
		RC4_Init(hsh, 256, H);
	}

	for (j = 0; j < 7; j++)
		hash[j] = RC4_GetppUns8( H );
	hash[6] &= 0x0F;
}

/* === === */


#define InitDigits LLInitDigits 
void InitDigits( PIntDig D)
{
	int j;
	ppUns8* p;
	if (D->NumDig < 0)
	{
		for (j = 0; j < 256; j++)
		{
			p = D->Int2Dig;
			for (; *p && *p != j; p++);
			D->Dig2Int[j] = (*p == 0 ? -1 : (int)(p - D->Int2Dig));
		}
		D->NumDig = (int)strlen((char*)D->Int2Dig);
	}
}

#define IsDigit LLIsDigit 
int IsDigit(ppUns8 ch, PIntDig D)
{
	ppUns8* p;
	for (p = D->Int2Dig; *p != 0; p++)
		if (*p == ch) return 1;
	return 0;
}

#define CvtDigits LLCvtDigits 
void CvtDigits(char* src, ppUns8* dest, PIntDig D)
{
	int cpos, cval, dpos;
	char *p;
	ppUns8 mask;

	p = src; dpos = 5;
	while (*p)
	{
		if (IsDigit(*p, D))
		{
			cval = D->Dig2Int[(ppUns8)*p];
			for (cpos = 0; cpos < 5; cpos++)
			{
				if ((cval & 1) == 1)
				{
					mask = (ppUns8)(1 << ((dpos - cpos - 1)%8));
					dest[(dpos - cpos - 1)/8] |= mask;
				}
				cval >>= 1;
			}
			dpos += 5;
		}
		p++;
	}
}

#define HFE_RevSign LLHFE_RevSign 
void HFE_RevSign(ppUns8* message, ppUns8* signature)
{
	int i, j, k, xj, res, rpos;
	ppUns8 *pb, mask, rmask;

	pb = HFE_PubKey;
	mask = 1;
	for (i = 0; i < HFE_MsgLength; i++)
	{
		res = ((*pb & mask) != 0 ? 1 : 0);
		mask <<= 1;
		if (mask == 0)
		{
			pb++;
			mask = 1;
		}

		/* single terms */
		for (j = 0; j < HFE_SigLength; j++)
		{
			if ((*pb & mask) != 0)
			{
				rmask = (ppUns8)(1 << (j % 8));
				rpos = (int)(j / 8);
				if ((signature[rpos] & rmask) != 0)
					res = 1 - res;
			}

			mask <<= 1;
			if (mask == 0) 
			{
				pb++; 
				mask = 1;
			}
		}

		/* quadratic terms */
		for (j = 0; j < HFE_SigLength; j++)
		{
			rmask = (ppUns8)(1 << (j % 8));
			rpos = (int)(j / 8);
			xj = (signature[rpos] & rmask) != 0 ? 1 : 0;
			for  (k = j; k < HFE_SigLength; k++)
			{
				if (xj == 1)
				{
					rmask = (ppUns8)(1 << (k % 8));
					rpos = (int)(k / 8);
					if ((signature[rpos] & rmask) != 0 && (*pb & mask) != 0)
						res = 1 - res;
				}

				mask <<= 1;
				if (mask == 0) 
				{
					pb++; 
					mask = 1;
				}
			}
		}

		rmask = (ppUns8)(1 << ((int)i % 8));
		rpos = (int)(i / 8);
		if (res == 1)
			message[rpos] |= rmask;
		else
			message[rpos] &= ~rmask;
	}
}

#endif



#define _LIB ( (PPDFLib) Lib )

static const char * PDFErrLevelStr[]= {
	"General Level",
		"Cos Object Level",
		"File Level",
		"Page Level",
		"Document Level",
		"Signature Level"
};

static const char * PDFGenLevelStr [] = {
	"No Error",
		"Other Error",
		"Not Enough Memory",
		"Cannot Open File",
		"Cannot Create File",
		"Cannot Receive File Size",
		"Cannot Create Map File",
		"Out Of Bound",
		"Cannot Change Capacity",
		"Cannot Receive Font Data",
		"Cannot Save Buffer To File",
		"Windows GDI Error",
		"Invalid Bitmap",
		"Invalid ipeg-image Quality",
		"Load PNG File Error",
		"Load TIFF File Error",
		"JBIG2 Error",
		"Load JPEG File Error",
		"Unsupported JPEG color space",
		"Not supported image format",
		"Load BMP File Error",
		"Unsupported bitmap compression",
		"I/O Error",
		"Unsupported feature for current license"
};

static const char * PDFDocLevelStr [] = {
	"No Error",
		"Other Error",
		"Cannot Use This Color Device",
		"Cannot Use Bookmark Root",
		"Cannot Find First Bead",
		"Cannot Find Thread For Bead",
		"Cannot Find Current Bead",
		"Cannot Find Bead Rectangle",
		"Cannot Insert Name To Name Tree",
		"Not Compatible Action",
		"Invalid Handle Using",
		"Unknown Destination Type",
		"Invalid Annotation Item",
		"Cannot append color image as b/w mask",
		"Cannot compress color image with b/w compression",
		"Cannot compress b/w image with JPEG compression",
		"Not valid mask image ",
		"Cannot perform operation with current AcroForm object",
		"Cannot find state of AcroForm",
		"Invalid value for RadioBox or CheckBox",
		"Cannot change value for AcroForm",
		"Digital signature not appended to document",
		"Digital signature was appended to document"
};

static const char * PDFPageLevelStr [] = {
	"No Error",
		"Other Error",
		"Invalid Rectangle Value",
		"Unicode Available For TrueType Font Only",
		"Cannot Create EMF File"
};

static const char * PDFFileLevelStr [] = {
	"No Error",
		"Other Error",
		"Decrypted File",
		"Crypted File",
		"Corrupted Trailer",
		"Cannot Load Pages",
		"Document Not Contain Pages",
		"Corrupted File"
};

static const char * PDFCosLevelStr [] = {
	"No Error",
		"Other Error",
		"Invalid Object",
		"Not Cos Object",
		"Array Bounds",
		"Unterminated String",
		"Illegal Hex Character In Name",
		"Name Token Too Long",
		"Illegal Character",
		"Illegal Character In Hex String",
		"Command Token Too Long",
		"EOF Was Found",
		"Unknown Token Was Found",
		"Dictionary Key Have Not Name",
		"Unknown Stream Length",
		"Data Read",
		"Cannot Find End Of Stream",
		"Illegal Cos Object Was Found",
		"No Header",
		"Unsupported Version",
		"No Start XRef",
		"No Start Address",
		"Bad XRef",
		"Bad Trailer",
		"Unexpected End of Stream",
		"Corrupted XRef of Stream",
		"Invalid Encrypt Information",
		"Unsupported encrypt Algorithm",
		"Flate",
		"ASCII Hex",
		"ASCII 85",
		"Unsupported Filter Decode",
		"Unfiltered Stream Was Found",
		"Invalid Object Stream Dictionary",
		"Invalid Object Stream",
		"Invalid Image Format",
		"Invalid Font Format",
		"Cannot Insert Object With Parrent",
		"Cannot Insert Object From Other Document",
		"Direct Object Was Found"
};

static const char * PDFSignLevelStr [] = {
	"No Error",
		"Other Error",
		"Invalid Object",
		"Bad ASN1 Header",
		"Bad ASN1 Tag",
		"Length Mismatch",
		"Not Implemented",
		"Bad Password"
};

ppInt32 VSGetErrorStr ( ppUns32 ErrorCode, char * Buffer )
{
    ppUns32 Level, Code;
    char * l, * e = NULL;
    ppInt32 sz;
    Level = ErrorCode >> PDF_ERR_LEVEL_SHIFT;
    Code = (ErrorCode  >> PDF_ERR_ERROR_SHIFT ) & PDF_ERR_ERROR_MASK;
    l = ( char * ) PDFErrLevelStr [ Level ];
    switch ( Level ) {
        case ErrGeneralLevel:
            e = ( char * ) PDFGenLevelStr [ Code ];
            break;
        case ErrCosLevel:
            e = ( char * ) PDFCosLevelStr [ Code ];
            break;
        case ErrFileLevel:
            e = ( char * ) PDFFileLevelStr [ Code ];
            break;
        case ErrPageLevel:
            e = ( char * ) PDFPageLevelStr [ Code ];
            break;
        case ErrDocumentLevel:
            e = ( char * ) PDFDocLevelStr [ Code ];
            break;
        case ErrSignLevel:
            e = ( char * ) PDFSignLevelStr [ Code ]; 
            break;
    }
    sz = ( ppInt32 ) strlen ( l ) + 5 + ( ppInt32 ) strlen ( e );
    if ( Buffer ) {
        strcpy( Buffer, l);
        strcat( Buffer, ( const char * )" ( ");
        strcat( Buffer, e );
		strcat( Buffer, ( const char * )" )");
    }
    return sz;
}


void * PDFIntMalloc(PDFLibHandle Lib, size_t size){
	void *p;
	if (size == 0)	return NULL;
	if ( _LIB->allocproc){
		p = _LIB->allocproc ((ppUns32) size );
	} else {
		p = malloc ( size );
	}
	if ( !p && _LIB->MemRaiseOnNULL ) 
		PDFRAISE(Lib,PDFBuildErrCode(ErrGeneralLevel,gleNotEnoughMemoryError));
	return p;
}

void *PDFIntRealloc(PDFLibHandle Lib, void *buf, size_t size){
	void *q;
	if (size == 0) {
		if (buf) 
			mfree(Lib, buf);
		return NULL;
	}
	if (buf) {
		if ( _LIB->reallocproc ){
			q = _LIB->reallocproc ( buf, (ppUns32)size );
		} else {
			q = realloc( buf, size );
		}
	} else {
		q = mmalloc(Lib, size);
	}
	if (!q && _LIB->MemRaiseOnNULL ) 
		PDFRAISE(Lib,PDFBuildErrCode(ErrGeneralLevel,gleNotEnoughMemoryError));
	return q;
}

void PDFIntFree(PDFLibHandle Lib, void *buf)
{
	if ( !buf )
		return;
	if ( _LIB->freeproc ){
		_LIB->freeproc ( buf );
	} else {
		free( buf );
	}
}

void ULClearAtomNode( PDFLibHandle Lib, PppAtomNode node)
{
	if ( node->Left )
		ULClearAtomNode ( Lib, node->Left );
	if ( node->Right )
		ULClearAtomNode ( Lib, node->Right );
	mfree ( Lib, node->String );
	mfree ( Lib, node->EscStr );
	mfree ( Lib, node );
}

void ULClearAtoms(PDFLibHandle Lib)
{
	if ( ( (PPDFLib) Lib )->AtomCount > 0 ){
		ULClearAtomNode ( Lib, ( (PPDFLib) Lib )->AtomRoot.Right );
		( (PPDFLib) Lib )->AtomCapacity = 0;
		( (PPDFLib) Lib )->AtomCount = 0;
		mfree ( Lib, ( (PPDFLib) Lib )->AtomArray );
		( (PPDFLib) Lib )->AtomArray = NULL;
		( (PPDFLib) Lib )->AtomRoot.Right = NULL;
	}
}

PDFLibHandle InitPDFLibrary ( char *LicenseName, char *LicenseKey ){
	PPDFLib Lib;
#ifdef LICENSED
	int i, pos, halt;
	int sigSize, msgSize;
	ppUns8 *hash, *sig, *msg, mask;
	char *ptr;
	int from, till;
	unsigned long hwid;
	ppUns8 ltype;
	char * RD = RELEASEDATE;
	TRC4Hash H;
	TIntDig D;
	D.Int2Dig = (ppUns8*)"23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
	D.NumDig = -1;
	hash = (ppUns8*)malloc(7*sizeof(ppUns8)); /*52 bits used*/
	RC4_ComputeHash((ppUns8*)LicenseName, hash, &H);

	sigSize = 0; ptr = LicenseKey;
	while (*ptr)
	{
		if (IsDigit(*ptr, &D) )
			sigSize++;
		ptr++;
	}
	sigSize *= 5;
	/* here sigSize should be equal HFE_SigLength */

	if (sigSize%8 != 0)
		sigSize += (8 - sigSize%8);
	sigSize /= 8;
	sig = (ppUns8*)malloc(sigSize*sizeof(ppUns8));
	memset(sig, 0, sigSize);
	InitDigits( &D);
	CvtDigits(LicenseKey, sig, &D);

	msgSize = HFE_MsgLength;
	if (msgSize%8 != 0)
		msgSize += (8 - msgSize%8);
	msgSize /= 8;
	msg = (ppUns8*)malloc(msgSize*sizeof(ppUns8));
	memset(msg, 0, msgSize);
	HFE_RevSign(msg, sig);

	halt = 0;
	for (i = 0; i < 52; i++)
	{
		mask = (ppUns8)(1 << (i % 8));
		pos = (int)(i / 8);
		if ((hash[pos] & mask) != (msg[pos] & mask))
		{
			halt = 1; break;
		}
	}

	if (!halt)
	{
		if (HFE_MsgLength > 52)
			from = UnpackDate(msg, 52);
		if (HFE_MsgLength > 64)
			till = UnpackDate(msg, 64);
		if (HFE_MsgLength > 76)
			hwid = UnpackDWord(msg, 76);
		if (HFE_MsgLength > 108)
			ltype = UnpackUChar(msg, 108);
	}
	free(hash);
	free(sig);
	free(msg);

	if ( halt )
		return NULL;
	if ( till > 0 ){
		if ( from & 8 ){
			time_t  t;
			char tv[15];
			struct tm *tt;
			time( &t );
			tt = localtime(&t);
			tt->tm_mon++;
			tt->tm_year+=1900;
			sprintf(tv,"%02i.%02i.%04i",tt->tm_mday,tt->tm_mon,tt->tm_year);
			if ( till < StrToDate ( tv ) )
				return NULL;
		} else {
			if ( till < StrToDate ( RD ) )
				return NULL;
		}
	}
	if ( ltype > 0 ){
		if (( ltype & 0x3F) != PDFPLATFORMID )
			return NULL;		
	}
	from = (ppUns8)(from & -9 ); 

#endif

	Lib = (PPDFLib ) malloc(sizeof(PDFLib));
	if (!Lib) return NULL;
	Lib->ExcBuffer=NULL;
	Lib->ExcCapacity=0;
	Lib->ExcCount=0;
	Lib->AtomArray=NULL;
	Lib->AtomCapacity=0;
	Lib->AtomCount=0;
#ifdef LICENSED
	Lib->Edition = (ppUns8)from;
#endif
	Lib->AtomRoot.Left=NULL;
	Lib->AtomRoot.Right=NULL;
	Lib->AtomRoot.Balance=0;
	Lib->AtomRoot.ID=ppMAXUns32;
	Lib->AtomRoot.EscStr=NULL;
	Lib->AtomRoot.String=NULL;
	Lib->allocproc = NULL;
	Lib->reallocproc = NULL;
	Lib->freeproc = NULL;
	Lib->errorproc = NULL;
	Lib->opaque = NULL;
    Lib->MemRaiseOnNULL = true;
	Lib-> KT_init = 1;
#ifdef CANVASDBG
	Lib->DBGIndex = 0;
#endif
	Lib->Names = (ppUns32*)mmalloc( Lib, sizeof(ppUns32)*pdfnamescount);
	memset( Lib->Names, 0xff, sizeof(ppUns32)*pdfnamescount );	
	return Lib;
}


PDFLibHandle InitPDFLibraryWithParams (char *LicenseName, char *LicenseKey,
				PDFAllocProc allocproc, PDFReAllocProc reallocproc, PDFFreeProc freeproc, 
				PDFErrorProc errorproc, void *opaque)
{
	PPDFLib Lib;
	PDFAllocProc Al;
	PDFFreeProc Fr;
#ifdef LICENSED
	int i, pos, halt;
	int sigSize, msgSize;
	ppUns8 *hash, *sig, *msg, mask;
	char *ptr;
	int from, till;
	unsigned long hwid;
	ppUns8 ltype;
	TRC4Hash H;
	TIntDig D;
	char * RD = RELEASEDATE;
#endif
	if ( allocproc ) {
		Al = allocproc;
	} else {
		Al = (PDFAllocProc) malloc;
	}
	if ( freeproc ) {
		Fr = freeproc;
	} else {
		Fr = free;
	}

#ifdef LICENSED
	
	D.Int2Dig = (ppUns8*)"23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
	D.NumDig = -1;

	hash = (ppUns8*)Al(7*sizeof(ppUns8)); /*52 bits used*/
	RC4_ComputeHash((ppUns8*)LicenseName, hash, &H);

	sigSize = 0; ptr = LicenseKey;
	while (*ptr)
	{
		if (IsDigit(*ptr, &D) )
			sigSize++;
		ptr++;
	}
	sigSize *= 5;
	/* here sigSize should be equal HFE_SigLength */

	if (sigSize%8 != 0)
		sigSize += (8 - sigSize%8);
	sigSize /= 8;
	sig = (ppUns8*)Al(sigSize*sizeof(ppUns8));
	memset(sig, 0, sigSize);
	InitDigits( &D);
	CvtDigits(LicenseKey, sig, &D);

	msgSize = HFE_MsgLength;
	if (msgSize%8 != 0)
		msgSize += (8 - msgSize%8);
	msgSize /= 8;
	msg = (ppUns8*)Al(msgSize*sizeof(ppUns8));
	memset(msg, 0, msgSize);
	HFE_RevSign(msg, sig);

	halt = 0;
	for (i = 0; i < 52; i++)
	{
		mask = (ppUns8)(1 << (i % 8));
		pos = (int)(i / 8);
		if ((hash[pos] & mask) != (msg[pos] & mask))
		{
			halt = 1; break;
		}
	}
	if (!halt)
	{
		if (HFE_MsgLength > 52)
			from = UnpackDate(msg, 52);
		if (HFE_MsgLength > 64)
			till = UnpackDate(msg, 64);
		if (HFE_MsgLength > 76)
			hwid = UnpackDWord(msg, 76);
		if (HFE_MsgLength > 108)
			ltype = UnpackUChar(msg, 108);
	}
	Fr(hash);
	Fr(sig);
	Fr(msg);
	if ( halt )
		return NULL;
	if ( till > 0 ){
		if ( from & 8 ){
			time_t  t;
			char tv[15];
			struct tm *tt;
			time( &t );
			tt = localtime(&t);
			tt->tm_mon++;
			tt->tm_year+=1900;
			sprintf(tv,"%02i.%02i.%04i",tt->tm_mday,tt->tm_mon,tt->tm_year);
			if ( till < StrToDate ( tv ) )
				return NULL;
		} else {
			if ( till < StrToDate ( RD ) )
				return NULL;
		}
	}
	if ( ltype > 0 ){
		if (( ltype & 0x3F) != PDFPLATFORMID )
			return NULL;		
	}
	from = (ppUns8)(from & -9 ); 
#endif

	Lib = ( PPDFLib ) Al ( sizeof ( PDFLib ) );
	if (!Lib) return NULL;
	Lib->ExcBuffer=NULL;
	Lib->ExcCapacity=0;
	Lib->ExcCount=0;
#ifdef LICENSED
	Lib->Edition = (ppUns8)from;
#endif
	Lib->AtomArray=NULL;
	Lib->AtomCapacity=0;
	Lib->AtomCount=0;
	Lib->AtomRoot.Left=NULL;
	Lib->AtomRoot.Right=NULL;
	Lib->AtomRoot.Balance=0;
	Lib->AtomRoot.ID=ppMAXUns32;
	Lib->AtomRoot.EscStr=NULL;
	Lib->AtomRoot.String=NULL;
	Lib-> KT_init = 1;
#ifdef CANVASDBG
	Lib->DBGIndex = 0;
#endif
	/*memset( & ( Lib->Names ), 0xff, sizeof ( Lib->Names ) );*/
	Lib->allocproc = allocproc;
	Lib->reallocproc = reallocproc;
	Lib->freeproc = freeproc;
	Lib->errorproc = errorproc;
	Lib->opaque = opaque;
	Lib->Names = (ppUns32*)mmalloc( Lib, sizeof(ppUns32)*pdfnamescount);
	memset( Lib->Names, 0xff, sizeof(ppUns32)*pdfnamescount );	
	return Lib;	
}
void DonePDFLibrary ( PDFLibHandle *lib ){
	mfree ( *lib, ( ( PPDFLib ) *lib )->Names );	
	mfree ( *lib, ( ( PPDFLib ) *lib )->ExcBuffer );
	ULClearAtoms ( *lib );
	mfree ( *lib, ( PPDFLib ) *lib ); 
	*lib = NULL;
}

