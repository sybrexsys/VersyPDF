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
VSMisc.c
*********************************************************************/


#include <string.h>
#include <math.h>
#include <time.h>

#ifdef WINDOWS_PLATFORM
#include <windows.h>
#endif





#include "VSMisc.h"
#include "VSCosAI.h"
#include "VSAssert.h"
#include "VSDocAI.h"
#include "VSCrypt.h"
static char *pdfhexchar         = "0123456789abcdef";

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD (Doc)
#define _LIB  _DOC->Lib

static const int _pdfbmask[9]   ={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};




XFORM ULMultiplyXFORM(XFORM A, XFORM B){
	XFORM R;
	R.eM11 = A.eM11 * B.eM11 + A.eM12 * B.eM21;
	R.eM12 = A.eM11 * B.eM12 + A.eM12 * B.eM22;
	R.eM21 = A.eM21 * B.eM11 + A.eM22 * B.eM21;
	R.eM22 = A.eM21 * B.eM12 + A.eM22 * B.eM22;
	R.eDx = A.eDx * B.eM11 + A.eDy * B.eM21 + B.eDx;
	R.eDy = A.eDx * B.eM12 + A.eDy * B.eM22 + B.eDy;
	return R;
}

ppInt32 ULAtomToStrm(PDFLibHandle Lib, PDFStreamHandle Strm, ppAtom Atom)
{
    if ( Atom >= ( ( PPDFLib ) Lib )->AtomCount )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    return ULStreamWriteBuffer ( Strm, ( ( PPDFLib ) Lib )->AtomArray[Atom]->EscStr,
		( ( PPDFLib ) Lib )->AtomArray[Atom]->Length );
}

ppInt32 ULGetAtomSize(PDFLibHandle Lib, ppAtom Atom)
{
    if ( Atom >= ( ( PPDFLib ) Lib )->AtomCount )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    return ( ( PPDFLib ) Lib )->AtomArray[Atom]->Length;
}

ppInt32 ULStrToStrm(PDFStreamHandle Strm, char *String)
{
    return ULStreamWriteBuffer ( Strm, String, ( ppInt32 ) strlen ( String ) );
}

ppInt32 ULIntToStrm(PDFStreamHandle Strm, ppInt32 Value)
{
    char Buffer[20];
    ULitostr ( Value, Buffer );
    return ULStreamWriteBuffer ( Strm, Buffer, ( ppInt32 ) strlen ( Buffer ) );
}

ppInt32 ULHexUns8ToStrm(PDFStreamHandle Strm, ppUns8 Value)
{
    char Buffer[2];
    Buffer[0] = pdfhexchar[( Value & 0xf0 ) >> 4];
    Buffer[1] = pdfhexchar[Value & 0xf];
    return ULStreamWriteBuffer ( Strm, Buffer, 2 );
}

ppInt32 ULHexUns16ToStrm(PDFStreamHandle Strm, ppUns16 Value)
{
    char Buffer[4];
    Buffer[0] = pdfhexchar[( Value & 0xf000 ) >> 12];
    Buffer[1] = pdfhexchar[( Value & 0xf00 ) >> 8];
    Buffer[2] = pdfhexchar[( Value & 0xf0 ) >> 4];
    Buffer[3] = pdfhexchar[Value & 0xf];
    return ULStreamWriteBuffer ( Strm, Buffer, 4 );
}


ppInt32 ULRealToStrm(PDFStreamHandle Strm, ppReal Value)
{
    char Buffer[20], *buf;
    ppInt32 i, l, r;
    if ( fabs ( Value ) > 32767 ){
        return ULIntToStrm ( Strm, ROUND ( Value ) );
    };
    r = ( int ) ( Value * 10000.0 );
	if ( ! r ){
		return  ULStreamWriteBuffer ( Strm,"0", 1 );
	}
	buf = Buffer;
	if (r <0 )
		buf++;
	//buf = r < 0 ? b1 : 0;
	ULitostr( r , Buffer );
	l = ( ppInt32 ) strlen ( buf ) - 1;
	if ( l > 3 ){
		for ( i = 0; i < 4; i++ )
			buf [ l - i + 1 ] = buf [ l - i ];
		buf [ l - 3 ] = '.';
		buf [ l + 2 ] = 0;
		i = l + 1; 
	} else { 
		for ( i = 0; i < l + 1; i++)
			buf [ 5 - i ] = buf [ l - i] ;
		for ( i = 0; i < 3 - l; i++)
			buf [ 2 + i ] = '0';
		buf [ 0 ] = '0';
		buf [ 1 ] = '.';
		buf [ 6 ] = 0;
        i = 5;
	}
    
    while ( true ){
        if ( buf[i] == '0' )
            buf[i--] = '\0';
        else if ( buf[i] == '.' ){
            buf[i] = '\0';
            break;
        } else 
			 break;
    };
    i = ULStreamWriteBuffer ( Strm, Buffer, ( ppInt32 ) strlen ( Buffer ) );
    return i;
}

char * ULitostr(int n, char *String)
{
    int i, c, j, sign;

    if ( ( sign = n ) < 0 )
        n = -n;    
    i = 0;
    do
        String[i++] =(char)( n % 10 + '0' );
    while ( ( n /= 10 ) > 0 );
    if ( sign < 0 )
        String[i++] = '-';
    String[i] = '\0';
    for ( j = i - 1, i = 0; i < j; i++, j-- ){
        c = String[i];
        String[i] = String[j];
        String[j] = (char) c;
    };
    return String;
}

ppInt32 ULGetIntSize(ppInt32 Value)
{
    if ( Value == 0 )
        return 1;
    else if ( Value < 0 )
        return ( ppInt32 ) log10 ( ( ppReal ) - Value ) + 2;
    else
        return ( ppInt32 ) log10 ( ( ppReal ) Value ) + 1;
}

ppInt32 ULGetIntByte(ppInt32 Value)
{
    if ( Value < 0x100 )
        return 1;
    else if ( Value < 0x10000 )
        return 2;
    else if ( Value < 0x1000000 )
        return 3;
    else
        return 4;
}

ppInt32 ULGetNameSize(char *Value)
{
    ppInt32 i, r = 0;
    for ( i = 0; Value[i]; i++ )
        if ( Value[i] < 33 ||
             Value[i] > 126 ||
             Value[i] == '%' ||
             Value[i] == '(' ||
             Value[i] == ')' ||
             Value[i] == '<' ||
             Value[i] == '>' ||
             Value[i] == '[' ||
             Value[i] == ']' ||
             Value[i] == '{' ||
             Value[i] == '}' ||
             Value[i] == '/' ||
             Value[i] == '#' )
            r += 3;
        else
            r++;
    return r;
}

ppInt32 ULGetRealSize(ppReal Value)
{
    char Buffer[20];
    ppInt32 i, l, r;
    if ( fabs ( Value ) > 32767 )
        return ULGetIntSize ( ROUND ( Value ) );
	r  = ( int ) ( Value * 10000 );
	if ( !r )
		return 1;
	ULitostr( r, Buffer );
	l = ( ppInt32 ) strlen ( Buffer ) - 1;
	if ( l > 3 ){
		for ( i = 0; i < 4; i++ )
			Buffer [ l - i + 1 ] = Buffer [ l - i ];
		Buffer [ l - 3 ] = '.';
		Buffer [ l + 2 ] = 0;
		i = l + 1; 
	} else { 
		for ( i = 0; i < l + 1; i++)
			Buffer [ 5 - i ] = Buffer [ l - i] ;
		for ( i = 0; i < 3 - l; i++)
			Buffer [ 2 + i ] = '0';
		Buffer [ 0 ] = '0';
		Buffer [ 1 ] = '.';
		Buffer [ 6 ] = 0;
		i = 5;
	}
    while ( true ){
        if ( Buffer[i] == '0' )
            i--;
        else if ( Buffer[i] == '.' ){
            return i;
        } else
            break;
    };
    return i + 1;
}

ppInt32 ULGetStringSize(char *Value, ppInt32 len)
{
    ppInt32 i, r = 0;
    for ( i = 0; i < len; i++ )
        if ( Value[i] == 0 )
            r += 4;
        else if ( Value[i] == '(' ||
                  Value[i] == ')' ||
                  Value[i] == '\\' ||
                  Value[i] == 13 ||
                  Value[i] == 10 )
            r += 2;
        else
            r++;
    return r;
}



char * ULGetEscName(PDFLibHandle Lib, char *Value)
{
    char *tmp;
    ppInt32 i, j;
    tmp = ( char * ) mmalloc ( Lib, ULGetNameSize ( Value ) + 1 );
    for ( i = 0,j = 0; Value[i]; i++ )
        if ( Value[i] < 33 ||
             Value[i] > 126 ||
             Value[i] == '%' ||
             Value[i] == '(' ||
             Value[i] == ')' ||
             Value[i] == '<' ||
             Value[i] == '>' ||
             Value[i] == '[' ||
             Value[i] == ']' ||
             Value[i] == '{' ||
             Value[i] == '}' ||
             Value[i] == '/' ||
             Value[i] == '#' ){
            tmp[j] = '#';
            tmp[j + 1] = pdfhexchar[( Value[i] & 0xf0 ) >> 4];
            tmp[j + 2] = pdfhexchar[( Value[i] & 0xf )];
            j += 3;
        } else{
            tmp[j] = Value[i]; j++;
        };

    tmp[j] = '\0';
    return tmp;
}

char * ULGetEscString(PDFLibHandle Lib, char *Value, ppInt32 len)
{
    char *tmp;
    ppInt32 i, j;
    tmp = ( char * ) mmalloc ( Lib, ULGetStringSize ( Value, len ) + 1 );
    for ( i = 0,j = 0; i < len; i++ )
        if ( Value[i] == 0 ){
            tmp[j] = '\\';
            tmp[j + 1] = '0';
            tmp[j + 2] = '0';
            tmp[j + 3] = '0';
            j += 4;
        } else if ( Value[i] == '(' ||
                    Value[i] == ')' ||
                    Value[i] == '\\' ||
                    Value[i] == 13 ||
                    Value[i] == 10 ){
            tmp[j] = '\\';
            if ( Value[i] == '(' )
                tmp[j + 1] = '(';

            if ( Value[i] == ')' )
                tmp[j + 1] = ')';
            if ( Value[i] == '\\' )
                tmp[j + 1] = '\\';
            if ( Value[i] == 13 )
                tmp[j + 1] = 'r';
            if ( Value[i] == 10 )
                tmp[j + 1] = 'n';
            j += 2;
        } else{
            tmp[j] = Value[i]; j++;
        };
    tmp[j] = '\0';
    return tmp;
}

ppUns16 ULANSIToUnicode(ppUns8 ansi_code)
{
    if ( ( ansi_code < 128 ) || ( ( ansi_code < 255 ) && ( ansi_code > 159 ) ) )
        return ansi_code;
	switch (ansi_code){
		case 128: return 8364;
		case 129: return 129;
		case 130: return 8218;
		case 131: return 402;
		case 132: return 8222;
		case 133: return 8230;
		case 134: return 8224;
		case 135: return 8225;
		case 136: return 710;
		case 137: return 8240;
		case 138: return 352;
		case 139: return 8249;
		case 140: return 338;
		case 141: return 141;
		case 142: return 381;
		case 143: return 143;
		case 144: return 144;
		case 145: return 8216;
		case 146: return 8217;
		case 147: return 8220;
		case 148: return 8221;
		case 149: return 8226;
		case 150: return 8211;
		case 151: return 8212;
		case 152: return 732;
		case 153: return 8482;
		case 154: return 353;
		case 155: return 8250;
		case 156: return 339;
		case 157: return 157;
		case 158: return 382;
		case 159: return 376;
	}
    return 0;
}

ppUns8 ULUnicodeToANSI(ppUns16 unicode)
{
    if ( ( unicode < 128 ) || ( ( unicode < 255 ) && ( unicode > 159 ) ) )
        return ( ppUns8 ) unicode;
	switch (unicode){
		case 8364: return 128;
		case 129 : return 129;
		case 8218: return 130;
		case 402 : return 131;
		case 8222: return 132;
		case 8230: return 133;
		case 8224: return 134;
		case 8225: return 135;
		case 710 : return 136;
		case 8240: return 137;
		case 352 : return 138;
		case 8249: return 139;
		case 338 : return 140;
		case 141 : return 141;
		case 381 : return 142;
		case 143 : return 143;
		case 144 : return 144;
		case 8216: return 145;
		case 8217: return 146;
		case 8220: return 147;
		case 8221: return 148;
		case 8226: return 149;
		case 8211: return 150;
		case 8212: return 151;
		case 732 : return 152;
		case 8482: return 153;
		case 353 : return 154;
		case 8250: return 155;
		case 339 : return 156;
		case 157 : return 157;
		case 382 : return 158;
		case 376 : return 159;
	}
    return 0;
}


char * ULIntToStrWithZero(PDFLibHandle Lib, ppInt32 Value, int Size)
{
    char *tmp;
    char Buffer[15];
    tmp = ( char * ) mmalloc ( Lib, Size + 1 );
    memset ( tmp, '0', Size );
    tmp[Size] = '\0';
    ULitostr ( Value, Buffer );
    memcpy ( &tmp[Size - strlen ( Buffer )], Buffer, strlen ( Buffer ) );
    return tmp;
}

ppInt32 ULmemfndsubset(void *Buffer, void *SubSting, ppInt32 BufferLength, ppInt32 SubStringLength)
{
    ppInt32 i, j;
    char *p1, *p2;
    if ( BufferLength < SubStringLength || SubStringLength == 0 )
        return -1;
    p1 = ( char * ) Buffer;
    p2 = ( char * ) SubSting;
    for ( i = 0; i <= BufferLength - SubStringLength; i++ ){
        for ( j = 0; j < SubStringLength; j++ )
            if ( p1[i + j] != p2[j] )
                break;
        if ( j == SubStringLength )
            return i;
    };
    return -1;
}

char * ULTrimStr(char *String)
{
    ppInt32 i;
    if ( String[0] == '\0' )
        return String;
    for ( i = 0; String[i] == ' ' || String[i] == '\t'; i++ ){
    };
    if ( i != 0 )
        memmove ( String, &String[i], strlen ( String ) - i + 1 );
    for ( i = ( ppInt32 ) strlen ( String ) - 1;
          ( i >= 0 && ( String[i] == ' ' || String[i] == '\t' ) );
          i-- )
        String[i] = '\0';
    return String;
}

PPDFList ULListCreate(PDFLibHandle Lib, ppUns32 InitSize)
{
    PPDFList tmp;
    tmp = ( PPDFList ) mmalloc ( Lib, sizeof ( TPDFList ) );
    tmp->Size = 0;
    tmp->Lib = Lib;
    tmp->Capacity = InitSize;
    PDFTRY ( Lib ){
        tmp->FList = ( PDFHandle * ) mmalloc ( Lib, InitSize * sizeof ( PDFHandle ) );
    } PDFEXCEPT ( Lib ){
        mfree ( Lib, tmp );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    return tmp;
}

void ULListFree(PPDFList List)
{
	mfree ( List->Lib , List->FList );
    mfree ( List->Lib, List );
}

void ULListGrow(PPDFList List)
{
    ULListSetCapacity ( List, List->Capacity + CalculateDelta ( List->Capacity ) );
}

void ULListDelete(PPDFList List, ppUns32 Index)
{
    if ( Index >= List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    if ( Index != List->Size - 1 )
        memmove ( &( List->FList[Index] ), &( List->FList[Index + 1] ),
                  ( List->Size - Index - 1 ) * sizeof ( PDFHandle ) );
    List->Size--;
}

void ULListSetCount(PPDFList List, ppUns32 NewCount)
{
    ppUns32 i;
    if ( List->Capacity < NewCount )
        ULListSetCapacity ( List, NewCount );
    if ( List->Size < NewCount ){
        for ( i = List->Size; i < NewCount; i++ )
            List->FList[i] = NULL;
    };
    List->Size = NewCount;
}


ppUns32 ULListAdd(PPDFList List, PDFHandle Item)
{
    ppUns32 r;
    r = List->Size;
    if ( r == List->Capacity )
        ULListGrow ( List );
    List->FList[r] = Item;
    List->Size++;
    return r;
}

void ULListExchange(PPDFList List, ppUns32 First, ppUns32 Second)
{
    PDFHandle tmp;
    if ( First >= List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    if ( Second >= List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    if ( First == Second )
        return;
    tmp = List->FList[First];
    List->FList[First] = List->FList[Second];
    List->FList[Second] = tmp;
}

ppUns32 ULListIndexOf(PPDFList List, PDFHandle Item)
{
    ppUns32 i;
    for ( i = 0; i < List->Size; i++ )
        if ( Item == List->FList[i] )
            return i;
    return ppMAXUns32;
}

ppUns32 ULListInsert(PPDFList List, PDFHandle Item, ppUns32 Index)
{
    if ( Index>List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    if ( List->Size == List->Capacity )
        ULListGrow ( List );
    if ( Index < List->Size )
        memmove ( &( List->FList[Index + 1] ), &( List->FList[Index] ),
                  ( List->Size - Index ) * sizeof ( PDFHandle ) );
    List->FList[Index] = Item;
    return Index;
}

void ULListRemove(PPDFList List, PDFHandle Item)
{
    ppInt32 r   = ULListIndexOf ( List, Item );
    if ( r >= 0 )
        ULListDelete ( List, r );
}

void ULListClear(PPDFList List)
{
    mfree ( List->Lib, List->FList );
    List->FList = NULL;
    List->Capacity = 0;
    List->Size = 0;
}

void ULListSetCapacity(PPDFList List, ppUns32 NewCapacity)
{
    if ( NewCapacity < List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotChangeCapacityError ) );
    if ( NewCapacity != List->Capacity ){
        if ( !NewCapacity )
            mfree ( List->Lib, List->FList );
        else
            List->FList = ( PDFHandle * ) mrealloc ( List->Lib, List->FList, NewCapacity * sizeof ( PDFHandle ) );
        List->Capacity = NewCapacity;
    }
}

void ULExtListSetCapacity(PPDFExtList List, ppUns32 NewCapacity)
{
    if ( NewCapacity < List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleCannotChangeCapacityError ) );
    if ( NewCapacity != List->Capacity ){
        if ( !NewCapacity )
            mfree ( List->Lib, List->FList );
        else
            List->FList = mrealloc ( List->Lib, List->FList, NewCapacity * List->EntrySize );
        List->Capacity = NewCapacity;
    }
}

PPDFExtList ULExtListCreate ( PDFLibHandle Lib, ppUns32 EntrySize, ppUns32 InitSize )
{
    PPDFExtList List;
    List = ( PPDFExtList ) mmalloc ( Lib, sizeof ( TPDFExtList ) );
    List->Lib =  Lib;
    List->EntrySize = EntrySize;
    List->Capacity = InitSize;
    List->Size = 0;
    if ( InitSize ){
        PDFTRY ( Lib ){
            List->FList = mmalloc ( Lib, InitSize * EntrySize );
        } PDFEXCEPT ( Lib ) {
            mfree ( Lib, List );
            PDFRERAISE ( Lib );
        } PDFTRYEND ( Lib );
    } else 
        List->FList = NULL;
    return List;
}

void ULExtListFree ( PPDFExtList List )
{
    mfree ( List->Lib, List->FList);
    mfree ( List->Lib, List );
}

ppUns32 ULExtListAdd ( PPDFExtList List, void *Entry )
{
    ppUns32 r;
    r = List->Size;
    if ( r == List->Capacity )
        ULExtListSetCapacity ( List, List->Capacity + CalculateDelta ( List->Capacity ) );
    memmove ( ( PppUns8 )( List->FList ) + r * List->EntrySize, Entry, List->EntrySize );
    List->Size++;
    return r;
}
ppUns32 ULExtListIndexOf ( PPDFExtList List, void *Entry )
{
    ppUns32 i;
    for ( i = 0; i < List->Size; i++ )
        if ( !memcmp ( ( PppUns8 )( List->FList ) + i * List->EntrySize, Entry, List->EntrySize ) )
            return i;
    return ppMAXUns32;
}

ppUns32 ULExtListInsert ( PPDFExtList List, void *Entry, ppUns32 Index )
{
    if ( Index>List->Size )
        PDFRAISE ( List->Lib, PDFBuildErrCode ( ErrGeneralLevel, gleOutOfBoundError ) );
    if ( List->Size == List->Capacity )
        ULExtListSetCapacity ( List, List->Capacity + CalculateDelta ( List->Capacity ) );
    if ( Index < List->Size )
        memmove ( ( PppUns8 )( List->FList ) + ( Index + 1) * List->EntrySize,
            ( PppUns8 )( List->FList ) + Index * List->EntrySize,
            ( List->Size - Index ) * List->EntrySize );
    memmove ( ( PppUns8 )( List->FList ) + Index * List->EntrySize, Entry, List->EntrySize );
    return Index;
}

void ULExtListClear ( PPDFExtList List )
{
    mfree ( List->Lib, List->FList );
    List->FList = NULL;
    List->Capacity = 0;
    List->Size = 0;
}

void * ULStrmGetMem(PDFStreamHandle Strm)
{
    if ( ( ( PULStrm ) Strm )->Type == strtMemory )
        return ( ( PULStrm ) Strm )->Sections.Memory.Buffer;
    else
        return NULL;
}

ppInt32 ULStrmWriteInt(PDFStreamHandle Strm, ppInt32 Integer, ppInt32 CountBytes)
{
    switch ( CountBytes ){
        case 4:
            ULStreamWriteChar ( Strm, ( ppUns8 ) ( ( Integer & 0xff000000 ) >> 24 ) );
        case 3:
            ULStreamWriteChar ( Strm, ( ppUns8 ) ( ( Integer & 0xff0000 ) >> 16 ) );
        case 2:
            ULStreamWriteChar ( Strm, ( ppUns8 ) ( ( Integer & 0xff00 ) >> 8 ) );
        default:
            ULStreamWriteChar ( Strm, ( ppUns8 ) ( Integer & 0xff ) );
    }
    return 0;
}

void ULBitStreamPutBits(ULBitStream *BST, ppInt32 Bits, ppInt32 Length)
{
    while ( Length > BST->Bit ){
        BST->Data |= (ppUns8) ( Bits >> ( Length - BST->Bit ) );
        Length -= BST->Bit;
        ULStreamWriteChar ( BST->Strm, BST->Data ); 
        BST->Data = 0; 
        BST->Bit = 8;
    } 
    BST->Data |= (ppUns8)(( Bits & _pdfbmask[Length] ) << ( BST->Bit - Length )); 
    BST->Bit = (ppUns8)(BST->Bit - ( ppUns8 ) Length); 
    if ( BST->Bit == 0 ){
        ULStreamWriteChar ( BST->Strm, BST->Data ); 
        BST->Data = 0; 
        BST->Bit = 8;
    }
}

ppInt32 ULGetBitsPerInt(ppInt32 Integer)
{
    if ( !Integer )
        return 0;
    if ( Integer > 0 )
        return ( ppInt32 ) log2 ( ( double ) Integer ) + 1;
    return 0;
}


ppAtom ULGetResourceDictElem(PDFDocHandle Doc, PDFCosHandle Res, char *Pref, ppInt32 *Acc,
                             PDFCosHandle Search)
{
    char nm[10];
    ppUns32 i, k;
    PDFID ID;
    PDFCosHandle obj;
    ppAtom at;
    if ( !_IsCosDictOrStream ( Search ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    if ( !_CosIsIndirect ( Search ) )
        _RAISE ( ErrCosLevel, cleInvalidObjError );
    ID = _CosObjFullID ( Search );
    for ( i = 0; i < _CosDictCount (Res); i++ ){
        obj = _CosDictValue(Res, i);
        if ( !_IsCosRef ( obj ) )
            continue;
        if ( _CosRefValue ( obj ).ID == ID.ID && _CosRefValue ( obj ).GenID == ID.GenID )
            return _CosDictKey(Res, i);
    }
    memset ( nm, 0, 10 );
    strcpy ( nm, Pref );
    k = ( ppInt32 ) strlen ( Pref );
    while ( true ){
        ULitostr ( *Acc, &( nm[k] ) );
        ( *Acc )++;
        at = ULStringToAtom ( _LIB, nm );
        for ( i = 0; i < _CosDictCount(Res); i++ ){
            if ( at == _CosDictKey(Res,i) )
                break;
        };
        if ( i == _CosDictCount(Res) ){
            CosDictAppend ( Res, at, Search );
            return at;
        };
    };
}

#ifdef WINDOWS_PLATFORM
ppUns8 ULGetDefFontCharSet(void)
{
    HDC DisplayDC;
    TEXTMETRIC TxtMetric;
    ppUns8 ooo  = DEFAULT_CHARSET;
    DisplayDC = GetDC ( 0 );
    if ( DisplayDC ){
        if ( SelectObject ( DisplayDC, GetStockObject ( SYSTEM_FONT ) ) )
            if ( GetTextMetrics ( DisplayDC, &TxtMetric ) )
                ooo = TxtMetric.tmCharSet;
        ReleaseDC ( 0, DisplayDC );
    }
    return ooo;
}

ppInt32 ULGetCodePageByCharset(ppUns8 Charset)
{
    if ( Charset == DEFAULT_CHARSET )
        Charset = ULGetDefFontCharSet ();
    switch ( Charset ){
        case RUSSIAN_CHARSET:
            return 1251;
        case TURKISH_CHARSET:
            return 1254;
        case SHIFTJIS_CHARSET:
            return 932;
        case HANGEUL_CHARSET:
            return 949;
        case CHINESEBIG5_CHARSET:
            return 950;
        case GB2312_CHARSET:
            return 936;
        case JOHAB_CHARSET:
            return 1361;
        case HEBREW_CHARSET:
            return 1255;
        case ARABIC_CHARSET:
            return 1256;
        case GREEK_CHARSET:
            return 1253;
        case THAI_CHARSET:
            return 874;
        case EASTEUROPE_CHARSET:
            return 1250;
        case MAC_CHARSET:
            return 10000;
        case BALTIC_CHARSET:
            return 1257;
        case VIETNAMESE_CHARSET:
            return 1258;
        default:
            return 1252;
    }
}
#endif



ppBool ULIsNotANSICode(ppUns16 Code)
{
    if ( Code < 128 )
        return false;
    if ( ( Code < 255 ) && ( Code > 159 ) )
        return false;
    if ( ( Code == 8364 ) ||
         ( Code == 129 )  ||
         ( Code == 8218 ) ||
         ( Code == 402 )  ||
         ( Code == 8222 ) ||
         ( Code == 8230 ) ||
         ( Code == 8224 ) ||
         ( Code == 8225 ) ||
         ( Code == 710 )  ||
         ( Code == 8240 ) ||
         ( Code == 352 )  ||
         ( Code == 8249 ) ||
         ( Code == 338 )  ||
         ( Code == 141 )  ||
         ( Code == 381 )  ||
         ( Code == 143 )  ||
         ( Code == 144 )  ||
         ( Code == 8216 ) ||
         ( Code == 8217 ) ||
         ( Code == 8220 ) ||
         ( Code == 8221 ) ||
         ( Code == 8226 ) ||
         ( Code == 8211 ) ||
         ( Code == 8212 ) ||
         ( Code == 732 )  ||
         ( Code == 8482 ) ||
         ( Code == 353 )  ||
         ( Code == 8250 ) ||
         ( Code == 339 )  ||
         ( Code == 157 )  ||
         ( Code == 382 )  ||
         ( Code == 376 ) )
        return false;
    return true;
}


#ifdef MACOSX_PLATFORM
static ppBool PlatformWasChecked = false;
static ppBool IsIntelPlatform = true;

void ULCheckPlatform ()
{
	ppUns16 T1 = 0xAABB;
	ppUns8 *T2;
	T2 = (ppUns8 *)&T1;
	if ( T2[0] != 0xBB )
		IsIntelPlatform = false;
	PlatformWasChecked = true;
}

void MACULSwapLong ( void *lp, ppBool NeedBig)
{
	if ( !PlatformWasChecked )
		ULCheckPlatform ();
	if ( IsIntelPlatform == NeedBig ){
		ppUns32 *l = (ppUns32*) lp;
		*l = (( *l & 0xFF000000)>>24) | ((*l&0x000000FF)<<24) | 
			((*l&0x00FF0000)>>8 ) | ((*l&0x0000FF00)<<8 );
	}

}
void MACULSwapShort ( void *lp, ppBool NeedBig)
{
	if ( !PlatformWasChecked )
		ULCheckPlatform ();
	if ( IsIntelPlatform == NeedBig ){
		ppUns16 *l = (ppUns16 *) lp;
		*l = ((*l) >> 8 ) | ((*l) << 8 );
	}

}
#else 
void ULSwapLong(void *lp)
{
	ppUns32 *l = (ppUns32*) lp;
	*l = (( *l & 0xFF000000)>>24) | ((*l&0x000000FF)<<24) | 
		((*l&0x00FF0000)>>8 ) | ((*l&0x0000FF00)<<8 )   ;
}
void ULSwapShort(void *wp)
{
	ppUns16 *l = (ppUns16 *) wp;
	*l = ((*l) >> 8 ) | ((*l) << 8 );
}
#endif

TPDFRect PDFRect( ppReal xl, ppReal yl, ppReal xr, ppReal yr )
{
    TPDFRect Rect;
    Rect.xl = xl;
    Rect.yl = yl;
    Rect.xr = xr;
    Rect.yr = yr;
    return Rect;
}

XFORM PDFMatrix( ppReal eM11, ppReal eM12, ppReal eM21, ppReal eM22, ppReal eDx, ppReal eDy )
{
    XFORM Matrix;
    Matrix.eM11 = ( float ) eM11;
    Matrix.eM12 = ( float ) eM12;
    Matrix.eM21 = ( float ) eM21;
    Matrix.eM22 = ( float ) eM22;
    Matrix.eDx = ( float ) eDx;
    Matrix.eDy = ( float ) eDy;
    return Matrix;
}

void ULSetNewCryptoData( PDFDocHandle Doc, TPDFCryptoType * Cryptodata)
{
	Cryptodata->nkl= _DOC->nkl;
	if (_DOC->nkl){
		Cryptodata->Permission = 0x7FFFF860 << 1;
		Cryptodata->Permission += 1024;
		if ( prPrint & _DOC->NewPermission )
			Cryptodata->Permission |= 4;
		if ( prModifyContent & _DOC->NewPermission )
			Cryptodata->Permission |= 8;
		if ( prCopyInformation & _DOC->NewPermission )
			Cryptodata->Permission |= 16;
		if ( prModifyAnnotation & _DOC->NewPermission )
			Cryptodata->Permission |= 32;
		if ( prPrintHiResolution & _DOC->NewPermission )
			Cryptodata->Permission |= 2048;
		if ( prFillAcroForm & _DOC->NewPermission )
			Cryptodata->Permission |= 256;
		if ( prAssembleDocument & _DOC->NewPermission )
			Cryptodata->Permission &= ~1024;
		if ( prExtractTextAndImage & _DOC->NewPermission )
			Cryptodata->Permission |= 512;
		if ( _DOC->CryptMetadata && _DOC->nkl == 1){
			Cryptodata->Version = 2;
			Cryptodata->Revision = 3;
			Cryptodata->KeyLength = 16;
		} else {
			if (_DOC->nkl != pt256BitAESProtection){
				Cryptodata->Version = 4;
				Cryptodata->Revision = 4;
				Cryptodata->KeyLength = 16;
			} else {
				Cryptodata->Version = 5;
				Cryptodata->Revision = 5;
				Cryptodata->KeyLength = 32;
			}
		}
	} else {
		Cryptodata->Version = 1;
		Cryptodata->Revision = 2;
		Cryptodata->KeyLength = 5;
		Cryptodata->Permission = 0x7FFFFFE0 << 1;
		if ( prPrint & _DOC->NewPermission )
			Cryptodata->Permission |= 4;
		if ( prModifyContent & _DOC->NewPermission )
			Cryptodata->Permission |= 8;
		if ( prCopyInformation & _DOC->NewPermission )
			Cryptodata->Permission |= 16;
		if ( prModifyAnnotation & _DOC->NewPermission )
			Cryptodata->Permission |= 32;
	}
	Cryptodata->EncryptMetadata = _DOC->CryptMetadata;
	Cryptodata->RemoveOldFilters = true;
	Cryptodata->FileID = ULCreateFileID ( Doc, PDFEngine );
	ULCreateKey ( Cryptodata, _DOC->OPassword, _DOC->UPassword );
}

PDFCosHandle ULPrepareCryptoDictionary( PDFDocHandle Doc, TPDFCryptoType *Cryptodata)
{
	PDFCosHandle Result;
	PDFCosHandle Wrk, Wrk1;
	if ( Cryptodata->Revision < 4  ){
		Result = CosDictNew ( Doc, true, 8 );
		PDFTRY ( _LIB ){
			_CosDictAppend ( Result, Filter, _CosNameNew ( Standard ) );
			_CosDictAppend ( Result, V,
				CosIntNew ( Doc, false, Cryptodata->Version ) );
			_CosDictAppend ( Result, Length,
				CosIntNew ( Doc, false, Cryptodata->KeyLength * 8 ) );
			_CosDictAppend ( Result, R,
				CosIntNew ( Doc, false, Cryptodata->Revision ) );
			_CosDictAppend ( Result, P,
				CosIntNew ( Doc, false, Cryptodata->Permission ) );
			_CosDictAppend ( Result, U,
				CosStringNew ( Doc, false, (char *)Cryptodata->CUP, 32 ) );
			_CosDictAppend ( Result, O,
				CosStringNew ( Doc, false, (char *)Cryptodata->COP, 32 ) );
			if ( Cryptodata->KeyLength == 5 )
				CheckPDFVersion ( Doc, 2 );
			else 
				CheckPDFVersion ( Doc, 4 );

		} PDFEXCEPT ( _LIB ){
			CosFree ( Result );
			PDFRERAISE ( _LIB );
		} PDFTRYEND ( _LIB );
	} else {
		srand( (unsigned)time( NULL ) );
		Result = CosDictNew ( Doc, true, 10 );
		PDFTRY ( _LIB ){
			_CosDictAppend ( Result, Filter, _CosNameNew ( Standard ) );
			_CosDictAppend ( Result, V,
				CosIntNew ( Doc, false, Cryptodata->Version ) );
			_CosDictAppend ( Result, Length,
				CosIntNew ( Doc, false, Cryptodata->KeyLength * 8 ) );
			_CosDictAppend ( Result, R,
				CosIntNew ( Doc, false, Cryptodata->Revision ) );
			_CosDictAppend ( Result, P,
				CosIntNew ( Doc, false, Cryptodata->Permission ) );
			if ( Cryptodata->nkl < pt256BitAESProtection ){
				_CosDictAppend ( Result, U,
					CosStringNew ( Doc, false, (char *)Cryptodata->CUP, 32 ) );
				_CosDictAppend ( Result, O,
					CosStringNew ( Doc, false, (char *)Cryptodata->COP, 32 ) );
			} else{
				_CosDictAppend ( Result, UE,
					CosStringNew ( Doc, false, (char *)Cryptodata->CUPE, 32 ) );
				_CosDictAppend ( Result, OE,
					CosStringNew ( Doc, false, (char *)Cryptodata->COPE, 32 ) );
				_CosDictAppend ( Result, U,
					CosStringNew ( Doc, false, (char *)Cryptodata->CUP, 48 ) );
				_CosDictAppend ( Result, O,
					CosStringNew ( Doc, false, (char *)Cryptodata->COP, 48 ) );
				_CosDictAppend ( Result, Perms,
					CosStringNew ( Doc, false, (char *)Cryptodata->Perm, 16 ) );
			}

			if ( !Cryptodata->EncryptMetadata)
				_CosDictAppend ( Result, EncryptMetadata,
				CosBoolNew ( Doc, false, false ) );
			if ( Cryptodata->RemoveOldFilters ){
				_CosDictAppend ( Result, CF , Wrk = CosDictNew (Doc, false, 1 ) );
				_CosDictAppend ( Wrk, StdCF, Wrk1 = CosDictNew ( Doc,false, 3));
				switch (Cryptodata->nkl) {
				case pt128BitProtection:
					_CosDictAppend ( Wrk1, CFM, _CosNameNew(V2));
					_CosDictAppend ( Wrk1, Length,CosIntNew ( Doc, false, 16 ));
					CheckPDFVersion ( Doc, 5 );
					break;									
				case pt128BitAESProtection:
					_CosDictAppend ( Wrk1, CFM, _CosNameNew(AESV2));
					_CosDictAppend ( Wrk1, Length,CosIntNew ( Doc, false, 16 ));
					CheckPDFVersion ( Doc, 7 );
					break;
				case pt256BitAESProtection:
					_CosDictAppend ( Wrk1, CFM, _CosNameNew(AESV3));
					_CosDictAppend ( Wrk1, Length,CosIntNew ( Doc, false, 32 ));
					CheckPDFVersion ( Doc, 9 );										   
					break;
				default:;
				}				
				_CosDictAppend ( Wrk1, AuthEvent, _CosNameNew(DocOpen));
				_CosDictAppend ( Result, StmF , _CosNameNew ( StdCF ) );
				_CosDictAppend ( Result, StrF , _CosNameNew ( StdCF ) );
			} else {
				_CosDictAppend ( Result, CF , CosCopy( Doc, Cryptodata->Algorithms) );
				_CosDictAppend ( Result, StmF , CosNameNew ( Doc, false, Cryptodata->DefStream ) );
				_CosDictAppend ( Result, StrF , CosNameNew ( Doc, false, Cryptodata->DefString ) );
			}
		} PDFEXCEPT ( _LIB ){
			CosFree ( Result );
			PDFRERAISE ( _LIB );
		} PDFTRYEND ( _LIB );	
	}		
	return Result;
}
