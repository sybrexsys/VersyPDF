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
VSTtf.c
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 

#include "VSTtf.h"  
#include "VSDocAI.h"
#include "VSMisc.h"
#include "VSAssert.h"

#define CMAP_TABLE 1668112752
#define CVT_TABLE 1668707360
#define FPGM_TABLE 1718642541
#define GLYPH_TABLE 1735162214
#define HEAD_TABLE 1751474532
#define HHEA_TABLE 1751672161

#define HMTX_TABLE 1752003704
#define LOCA_TABLE 1819239265
#define MAXP_TABLE 1835104368
#define NAME_TABLE 1851878757
#define PREP_TABLE 1886545264
#define POST_TABLE 1886352244



#ifdef WINDOWS_PLATFORM



static ppUns32 InTables[12]      ={ 
	CMAP_TABLE, CVT_TABLE , FPGM_TABLE, GLYPH_TABLE ,HEAD_TABLE , HHEA_TABLE,
	HMTX_TABLE, LOCA_TABLE, MAXP_TABLE, NAME_TABLE, PREP_TABLE, POST_TABLE
};
#endif /* WINDOWS_PLATFORM */

static char CmHeaher[59]      ={
    '\x00', '\x00', '\x00', '\x02', '\x00', '\x01', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x14', '\x00', '\x03', '\x00', '\x01', '\x00', '\x00',
    '\x01', '\x1A', '\x00', '\x00', '\x01', '\x06', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x02'
};
/*
static char SpaceGlyph[4]     ={
	'\x98', '\x01', '\x4B', '\x02'
};
*/
static char OutHeader[16]     ={
    '\x00', '\x01', '\x00', '\x00', '\x00', '\x0C', '\x00', '\x80', '\x00',
    '\x03', '\x00', '\x10', '\x63', '\x6D', '\x61', '\x70'
};

static char OutFOHeader[16]     ={
	'\x00', '\x01', '\x00', '\x00', '\x00', '\x0B', '\x00', '\x80', '\x00',
	'\x03', '\x00', '\x10', '\x63', '\x6D', '\x61', '\x70'
};

static char PostTable[52]           ={
	'\x00', '\x02', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\xFF',
	'\x27', '\x00', '\x96', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
	'\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
	'\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x04', '\x00', '\x00',
	'\x00', '\x00', '\x01', '\x02', '\x00', '\x44', '\x09', '\x61', '\x66',
	'\x69', '\x69', '\x31', '\x30', '\x30', '\x36', '\x35'
};

static char NameTable[260]          ={
    '\x00', '\x00', '\x00', '\x0A', '\x00', '\x7E', '\x00', '\x01', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x01', '\x00', '\x05', '\x00', '\x00',
    '\x00', '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x02', '\x00',
    '\x07', '\x00', '\x05', '\x00', '\x01', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x04', '\x00', '\x05', '\x00', '\x0C', '\x00', '\x01', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x06', '\x00', '\x07', '\x00', '\x11',
    '\x00', '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x03', '\x00',
    '\x1F', '\x00', '\x18', '\x00', '\x03', '\x00', '\x01', '\x04', '\x09',
    '\x00', '\x01', '\x00', '\x0A', '\x00', '\x37', '\x00', '\x03', '\x00',
    '\x01', '\x04', '\x09', '\x00', '\x02', '\x00', '\x0E', '\x00', '\x41',
    '\x00', '\x03', '\x00', '\x01', '\x04', '\x09', '\x00', '\x04', '\x00',
    '\x0A', '\x00', '\x4F', '\x00', '\x03', '\x00', '\x01', '\x04', '\x09',
    '\x00', '\x06', '\x00', '\x0E', '\x00', '\x59', '\x00', '\x03', '\x00',
    '\x01', '\x04', '\x09', '\x00', '\x03', '\x00', '\x1E', '\x00', '\x67',
    '\x41', '\x72', '\x69', '\x61', '\x6C', '\x52', '\x65', '\x67', '\x75',
    '\x6C', '\x61', '\x72', '\x41', '\x72', '\x69', '\x61', '\x6C', '\x41',
    '\x72', '\x69', '\x61', '\x6C', '\x4D', '\x54', '\x6C', '\x6C', '\x50',
    '\x44', '\x46', '\x6C', '\x69', '\x62', '\x3A', '\x20', '\x41', '\x72',
    '\x69', '\x61', '\x6C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x41', '\x00', '\x72', '\x00', '\x69', '\x00', '\x61',
    '\x00', '\x6C', '\x00', '\x52', '\x00', '\x65', '\x00', '\x67', '\x00',
    '\x75', '\x00', '\x6C', '\x00', '\x61', '\x00', '\x72', '\x00', '\x41',
    '\x00', '\x72', '\x00', '\x69', '\x00', '\x61', '\x00', '\x6C', '\x00',
    '\x41', '\x00', '\x72', '\x00', '\x69', '\x00', '\x61', '\x00', '\x6C',
    '\x00', '\x4D', '\x00', '\x54', '\x00', '\x6C', '\x00', '\x6C', '\x00',
    '\x50', '\x00', '\x44', '\x00', '\x46', '\x00', '\x6C', '\x00', '\x69',
    '\x00', '\x62', '\x00', '\x3A', '\x00', '\x20', '\x00', '\x41', '\x00',
    '\x72', '\x00', '\x69', '\x00', '\x61', '\x00', '\x6C', '\x00'
};

static const ppUns16 colnames       = 12;

#define read_buffer_from_stream_long(A) {                                       \
    ULStreamReadBuffer ( Strm, &A, 4 );                                            \
    BSWAPL ( A );                                                           \
}

#define read_buffer_from_stream_short(A) {                                      \
    ULStreamReadBuffer ( Strm, &A, 2 );                                            \
    BSWAPS ( A );                                                          \
}

#define get_lrecord  {                                                          \
    PlatformID = Mark;                                                          \
    EncodingID = Mark + 1;                                                      \
    LanguageID = Mark + 2;                                                      \
    NameID =  Mark + 3;                                                         \
    StringLength =  Mark + 4;                                                   \
    StringOffset =  Mark + 5;                                                   \
    Mark = Mark + 6;                                                            \
}

#define get_word(A) {                                                           \
    A = ( ppUns16 ) ( *Mark );                                                  \
    BSWAPS ( A );                                                         \
    Mark ++ ;                                                                   \
}

#define get_word1(A) {                                                          \
    A = ( ppUns16 ) ( *Mark );                                                  \
    BSWAPS ( A );                                                         \
}

#define load_st_dt(A,B) PDFTTFLoadStreamDinamicTable ( Lib, tmp, Strm, A, (char **)(B) );

#define enlarge_segment {                                                       \
    SegCount++;                                                                 \
    Segments = ( PTableSegment ) mrealloc ( Lib, Segments, SegCount * sizeof ( TTableSegment ) ); \
    Segments[SegCount - 1].IDRange = 0;                                         \
}

#ifdef __BORLANDC__

#define get_head_element(A) {                                                   \
    A = ( *FullName );                                                          \
    BSWAPS ( A) );                                             \
    A = ( ( ppUns16 ) ( A/k_em ) );                                             \
    FullName++;                                                                 \
}

#define get_head_element1(A) {                                                  \
    A = ( *FullName );                                                          \
    BSWAPS ( A );                                              \
    A = ( ( ppUns16 ) ( A/k_em ) );                                             \
}

#else

#define get_head_element(A) {                                                   \
    A = ( *FullName );                                                          \
    BSWAPS ( A );                                                         \
    A = ( ( ppUns16 ) ( A/k_em ) );                                             \
    FullName++;                                                                 \
}

#define get_head_element1(A) {                                                  \
    A = ( *FullName );                                                          \
    BSWAPS ( A);                                                          \
    A = ( ( ppUns16 ) ( A/k_em ) );                                             \
}

#endif

#define send_buffer(A, B) {                                                     \
    A = B;                                                                      \
    BSWAPS ( A );                                                         \
    ULStreamWriteBuffer ( InclStream, &A, 2 );                                    \
}

#define conv_size(A) ((A + 3) & ~3)

#define update_offset PrevOffset = ((PrevOffset + 3) & ~3);

#define paste_table_tail {                                                      \
    PrevSize = 32;                                                              \
    PDFTTFPasteTableHeader( MAXP_TABLE, Font->Headers, Font->TableNumber,       \
        temp, &PrevOffset, &PrevSize, Font->Tables.PMaxp );                     \
    PrevSize = 260;                                                             \
    PDFTTFPasteTableHeader( NAME_TABLE, Font->Headers, Font->TableNumber,       \
        temp, &PrevOffset, &PrevSize, &NameTable[0] );                          \
    PrevSize = 52;                                                              \
    PDFTTFPasteTableHeader( POST_TABLE, Font->Headers, Font->TableNumber,       \
        temp, &PrevOffset, &PrevSize, Font->Tables.Post );                      \
    PrevSize = 0;                                                               \
    if ( Font->IsPREPPresent)													\
		PDFTTFPasteTableHeader( PREP_TABLE, Font->Headers, Font->TableNumber,   \
        temp, &PrevOffset, &PrevSize, Font->Tables.Prep );                      \
}

#define get_post_offset(A,B) {                                                  \
    FullName=(ppInt16 *)(Font->Tables.Post + B*2);                     \
    SLength = (*FullName);                                                      \
    BSWAPS(SLength);                                                      \
    A = SLength;                                                                \
    A = ((ppUns16)(A/k_em));                                                    \
}

#define create_glyph_table {                                                    \
    for ( h=1; h <= (ppUns32)Font->NumGlyphs; h++ ){                            \
        if ( GlyphNext->Size > 0 ){                                             \
            GlyphCount++;                                                       \
            ULStreamWriteBuffer ( GlyphTable, GlyphNext->Glyph , GlyphNext->Size ); \
            GlyphList = ( PGlyph * ) mrealloc( Lib, GlyphList, GlyphCount * sizeof ( PGlyph ) ); \
            GlyphList[GlyphCount - 1] = GlyphNext;                              \
        }                                                                       \
        GlyphNext = GlyphNext->Next;                                            \
    }                                                                           \
}

#define PDFTTFEditLinkCompression(glyph_links, j) ((ppUns16)(((ppInt32 *)glyph_links)[j]+3))

ppUns16 PDFTTFFindTable( ppUns32 Name, PPDFFontHeader Headers, ppUns16 Length )
{
    ppUns16 h;
    for ( h = 0; h < Length; h++ ){
        if ( Headers[h]->Name == Name )
            return h;
    }       
    return ppMAXUns16;
}

void PDFTTFLoadStreamMaxpTable( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm )
{
    ppUns16 TableIndex;
    ppUns16 *MaxpBlock;
    ppUns32 CurrOffset;
    TableIndex = PDFTTFFindTable ( MAXP_TABLE, tmp->Headers, tmp->TableNumber );
    CurrOffset = tmp->Headers[TableIndex]->Offset;
    ULStreamSetPosition ( Strm, CurrOffset );
    tmp->Tables.PMaxp = ( char * ) mmalloc ( Lib, 32 );
    ULStreamReadBuffer ( Strm, tmp->Tables.PMaxp, 32 );
    MaxpBlock = ( ppUns16 * ) ( tmp->Tables.PMaxp + 4 );
    tmp->NumGlyphs = *MaxpBlock;
    BSWAPS ( tmp->NumGlyphs );
}

void PDFTTFLoadStreamHheaTable( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm )
{
    ppUns16 TableIndex;
    ppUns32 CurrOffset;
    TableIndex = PDFTTFFindTable ( HHEA_TABLE, tmp->Headers, tmp->TableNumber );
    CurrOffset = tmp->Headers[TableIndex]->Offset;
    ULStreamSetPosition ( Strm, CurrOffset );
    tmp->Tables.PHhea = ( char * ) mmalloc ( Lib, 36 );
    ULStreamReadBuffer ( Strm, tmp->Tables.PHhea, 36 );
}

void PDFTTFLoadStreamHeadTable( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm )
{
    ppUns16 TableIndex;
    ppUns32 CurrOffset;
    ppUns16 *FontBuffer;
    TableIndex = PDFTTFFindTable ( HEAD_TABLE, tmp->Headers, tmp->TableNumber );
    CurrOffset = tmp->Headers[TableIndex]->Offset;
    ULStreamSetPosition ( Strm, CurrOffset );
    tmp->Tables.PHead = ( char * ) mmalloc ( Lib, 54 );
    ULStreamReadBuffer ( Strm, tmp->Tables.PHead, 54 );
    FontBuffer = ( ppUns16 * ) ( tmp->Tables.PHead );
    FontBuffer += 9;
    tmp->UnitsPerEm = *FontBuffer;
    FontBuffer += 16;
    tmp->indexToLocFormat = *FontBuffer;
    BSWAPS ( tmp->indexToLocFormat );
    BSWAPS ( tmp->UnitsPerEm );
}

void PDFTTFGetStreamMonoSpace( PPDFFontTables tmp, PDFStreamHandle Strm )
{
    ppUns16 TableIndex;
    ppUns32 CurrOffset;
    tmp->MonoSpace = 0;
    TableIndex = PDFTTFFindTable ( POST_TABLE, tmp->Headers, tmp->TableNumber );
    CurrOffset = tmp->Headers[TableIndex]->Offset;
    ULStreamSetPosition ( Strm, CurrOffset + 12 );
    ULStreamReadBuffer ( Strm, &CurrOffset, 4 );
    BSWAPL ( CurrOffset );
    if ( CurrOffset != 0 )
        tmp->MonoSpace = 1;
}

ppUns32 PDFTTFLoadStreamDinamicTable( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm, 
                                     ppUns32 Name, char **ptable )
{
    ppInt16 TableIndex;
    ppUns32 CurrOffset, CurrLength;
    TableIndex = ( ppInt16 )PDFTTFFindTable ( Name, tmp->Headers, tmp->TableNumber );
	if ( TableIndex != -1){
		CurrOffset = tmp->Headers[TableIndex]->Offset;
		CurrLength = tmp->Headers[TableIndex]->Length;
		ULStreamSetPosition ( Strm, CurrOffset );
		*ptable = ( char * ) mmalloc ( Lib, CurrLength );
		ULStreamReadBuffer ( Strm, *( ptable ), CurrLength );   
        return CurrLength;
	}
	return 0;
}

void PDFTTFLoadStreamAuxTables( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm )
{
	int sz;
    sz = load_st_dt ( PREP_TABLE, &tmp->Tables.Prep );
	if ( sz ==0 ) 
		tmp->IsPREPPresent = false;
	else 
		tmp->IsPREPPresent = true;

    tmp->FpgmSize = load_st_dt ( FPGM_TABLE, &tmp->Tables.Fpgm );
    if ( tmp->FpgmSize ==0 ) 
		tmp->IsFPGMPresent = false;
	else 
		tmp->IsFPGMPresent = true;
    tmp->HmtxSize = load_st_dt ( HMTX_TABLE, ( char * * ) &tmp->Tables.Hmtx );
    load_st_dt ( NAME_TABLE, &tmp->Tables.Name );
    load_st_dt ( POST_TABLE, &tmp->Tables.Post );
    sz = load_st_dt ( CVT_TABLE, &tmp->Tables.Cvt );
	if ( sz ==0 ) 
		tmp->IsCVTPPresent = false;
	else 
		tmp->IsCVTPPresent = true;
    tmp->LocaSize = load_st_dt ( LOCA_TABLE, &tmp->Tables.Loca );
    PDFTTFLoadStreamMaxpTable ( Lib, tmp, Strm );
    PDFTTFLoadStreamHheaTable ( Lib, tmp, Strm );
    PDFTTFLoadStreamHeadTable ( Lib, tmp, Strm );
    PDFTTFGetStreamMonoSpace ( tmp, Strm );
}

void PDFTTFTablesGetFontHeader( PDFLibHandle Lib, PPDFFontTables tmp, PDFStreamHandle Strm )
{
    ppInt32 h, k;
    ULStreamSetPosition ( Strm, ULStreamGetPosition ( Strm ) + 4 );
    read_buffer_from_stream_short ( tmp->TableNumber );
    read_buffer_from_stream_short ( tmp->SearchRange );
    read_buffer_from_stream_short ( tmp->EntrySelector );
    read_buffer_from_stream_short ( tmp->EntrySelector );
    tmp->Headers = ( PPDFFontHeader ) mmalloc ( Lib, k = tmp->TableNumber * 
        sizeof ( PPDFFontRecord ) );
    memset ( tmp->Headers, 0, k );
    for ( h = 0; h < tmp->TableNumber; h++ ){
        tmp->Headers[h] = ( PPDFFontRecord ) mmalloc ( Lib, sizeof ( TPDFFontRecord ) );
        read_buffer_from_stream_long ( tmp->Headers[h]->Name );
        read_buffer_from_stream_long ( tmp->Headers[h]->CheckSum );
        read_buffer_from_stream_long ( tmp->Headers[h]->Offset );
        read_buffer_from_stream_long ( tmp->Headers[h]->Length );
    }
}

ppUns16 PDFTTFGetSymbolID( PPDFFontTables tmp, ppUns16 Unicode, ppInt32 i )
{
    ppUns32 COffset;
    if ( ( Unicode >= tmp->Tables.Segments.StartCount[i] ) &&
         ( Unicode <= tmp->Tables.Segments.EndCount[i] ) ){
        if ( tmp->Tables.Segments.IDRangeOffset[i] == 0 )
            return Unicode + tmp->Tables.Segments.IDDelta[i];
        else{
            COffset = i + ( tmp->Tables.Segments.IDRangeOffset[i] >> 1 ) -
                ( tmp->Tables.Cmap4.SegCountX2 >> 1 ) + Unicode -
                tmp->Tables.Segments.StartCount[i];
            return ( ( ppUns16 ) ( tmp->Tables.GArrayID[COffset] +
                tmp->Tables.Segments.IDDelta[i] ) );
        }
    }else
        return 0;
}

ppUns16 PDFTTFGetSymbolWidth( PPDFFontTables tmp, ppUns16 SymbolID )
{
    char *TableLink;
    ppUns16 *FLink;
    ppUns16 *LinkMetrics;
    ppUns16 FWidth;
    ppInt32 HmtxCount;
    ppUns16 HheaHMetrics;
    float k_em;
    LinkMetrics = ( ppUns16 * )((char * )( tmp->Tables.PHhea ) + 34);
    HheaHMetrics = ( *LinkMetrics );
    BSWAPS ( HheaHMetrics );
    HmtxCount = ( ppInt32 ) ( tmp->HmtxSize / HheaHMetrics );
    if ( ( tmp->MonoSpace != 0 ) && ( HmtxCount != 4 ) )
        FLink = ( ppUns16 * ) ( tmp->Tables.Hmtx );
    else{
        TableLink = ( char * ) tmp->Tables.Hmtx;
        TableLink += ( SymbolID * 4 );
        FLink = ( ppUns16 * ) ( TableLink );
    }
    FWidth = *FLink;
    BSWAPS ( FWidth ); 
    k_em = ( float ) ( tmp->UnitsPerEm  / 1000.0 );
    return ( ( ppUns16 ) ( FWidth / k_em ) );
}

void PDFTTFBuildCodeTable( PDFLibHandle Lib, PPDFFontTables Font )
{
    ppUns16 SerialNumber= 0;
    ppInt32 f, i;
    Font->CodeTable = NULL;
    PDFTRY ( Lib ){
        for ( i = 0; i < ( Font->Tables.Cmap4.SegCountX2 / 2 ); i++ )
            for ( f = Font->Tables.Segments.StartCount[i]; f <= Font->Tables.Segments.EndCount[i]; f++ ){
				if ( f == 0xFFFF)
					break;
                Font->CodeTable = ( PCodeTableNode ) mrealloc ( Lib, Font->CodeTable,
                    ( SerialNumber + 1 ) * sizeof ( TCodeTableNode ) );
                Font->CodeTable[SerialNumber].Code = (ppInt16)f;
                Font->CodeTable[SerialNumber].ID = PDFTTFGetSymbolID ( Font, (ppUns16)f, i );
                Font->CodeTable[SerialNumber].Width = PDFTTFGetSymbolWidth ( Font,
                    Font->CodeTable[SerialNumber].ID );
                Font->CodeTable[SerialNumber].Used = 0;
                SerialNumber++;
            }
    }
    PDFEXCEPT ( Lib ){
        mfree ( Lib, Font->CodeTable );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    Font->CodeLength = SerialNumber;
}

#ifdef WINDOWS_PLATFORM
PPDFFontTables PDFTTFLoadFontFromDC( PDFLibHandle Lib, HDC DC )
{
   void * buf;
   PPDFFontTables tmp;
   ppInt32 SZ, i, h, off, K;
   ppUns32 TName, LongX;
   ppUns16 PHeadLoc, GlyphLength;
   ppUns16 *PHeadBlock, ShortX, SMBuff;
   ppUns16 *FontBuffer;
   PDFStreamHandle Strm;
   CmapInfoRec CmapInfo;
   ppUns16 *MaxpBlock;
   PGlyph GlyphNext, GlyphTmp;
   tmp = ( PPDFFontTables ) mmalloc ( Lib, sizeof ( TPDFFontTables ) );
   PDFTRY ( Lib ){
        memset ( tmp, 0, sizeof ( TPDFFontTables ) );
		tmp->DCUsedForLoad = true;
		tmp->IsCVTPPresent = true;
		tmp->IsFPGMPresent = true;
		tmp->IsPREPPresent = true;
		tmp->TableNumber = 12;
		tmp->Headers = ( PPDFFontHeader ) mmalloc ( Lib, 12 * 
			sizeof ( PPDFFontRecord ) );
		for ( i=0; i < 12; i++ ){
			tmp->Headers [ i ] = ( PPDFFontRecord ) mmalloc ( Lib, sizeof ( TPDFFontRecord ) );
			tmp->Headers [ i ]->Name = InTables [ i ];
			tmp->Headers [ i ]->CheckSum = ppMAXUns32;
			tmp->Headers [ i ]->Offset = ppMAXUns32;
			if ( i == 3 ) {
				tmp->Headers [ i ]->Length = ppMAXUns32;
				continue;
			}
			TName = InTables[ i ];
			BSWAPL( TName );
			SZ = GetFontData ( DC, TName, 0, NULL, 0 );
			if ( SZ == -1 ){
				if ( i == 2 ){
                    tmp->IsFPGMPresent = false;
                    tmp->FpgmSize = 0;
					tmp->Headers [ i ]->Length = ppMAXUns32;
					continue;
				}
				if ( i == 1 ){
					tmp->IsCVTPPresent = false;
					//tmp->FpgmSize = 0;
					tmp->Headers [ i ]->Length = ppMAXUns32;
					continue;
				}
				if ( i == 10 ){
					tmp->IsPREPPresent = false;
					//tmp->FpgmSize = 0;
					tmp->Headers [ i ]->Length = ppMAXUns32;
					continue;
				}

                PDFRAISE( Lib, cleInvalidFontFormat );
			}
			buf = mmalloc ( Lib, SZ );
			PDFTRY ( Lib )
			{
				SZ = GetFontData ( DC, TName, 0, buf, SZ );
				tmp->Headers [ i ]->Length = SZ;
				if ( SZ == -1 ){
                    PDFRAISE( Lib, cleInvalidFontFormat );
				}
                Strm = ULStreamMemReadOnlyNew( Lib, buf, SZ) ;
				ULStreamSetPosition( Strm, 0);
				switch ( i ) 
				{
					case 0:{ /* CMAP */
                        tmp->Tables.Cmap = ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Cmap, SZ );
						ULStreamSetPosition ( Strm, 0 );
						read_buffer_from_stream_short ( tmp->Tables.CmapHeader.Version );
						read_buffer_from_stream_short ( tmp->Tables.CmapHeader.NumTables );
						for ( h = 1; h <= tmp->Tables.CmapHeader.NumTables; h++ ){
							read_buffer_from_stream_short ( CmapInfo.PlatformID );
							read_buffer_from_stream_short ( CmapInfo.EncodingID );
							read_buffer_from_stream_long ( CmapInfo.Offset );
							off = CmapInfo.Offset;
							if ( ( CmapInfo.PlatformID == 3 ) && ( CmapInfo.EncodingID == 1 ) )
								break;
						}
						ULStreamSetPosition ( Strm, off );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Format );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Length );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Version );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4.SegCountX2 );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4.SearchRange );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4.EntrySelector );
						read_buffer_from_stream_short ( tmp->Tables.Cmap4.RangeShift );
						K = ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 );
						tmp->Tables.Segments.EndCount = ( ppUns16 * ) mmalloc ( Lib, K * sizeof( ppUns16 ) );
						tmp->Tables.Segments.StartCount = ( ppUns16 * ) mmalloc ( Lib, K * sizeof( ppUns16 ) );
						tmp->Tables.Segments.IDDelta = ( ppUns16 * ) mmalloc ( Lib, K * sizeof( ppUns16 ) );
                        tmp->Tables.Segments.IDRangeOffset = ( ppUns16 * ) mmalloc ( Lib, K * sizeof( ppUns16 ) );
						for ( h = 0; h < K; h++ )
						{
                       	    ULStreamReadBuffer ( Strm, &SMBuff, 2 );
	                        BSWAPS ( SMBuff );
							tmp->Tables.Segments.EndCount[h] = SMBuff;
						}
						ULStreamReadBuffer ( Strm, &SMBuff, 2 );
						BSWAPS ( SMBuff );
						tmp->Tables.Segments.ZeroByte = SMBuff;
						for ( h = 0; h < K; h++ )
						{
							ULStreamReadBuffer ( Strm, &SMBuff, 2 );
							BSWAPS ( SMBuff );
							tmp->Tables.Segments.StartCount[h] = SMBuff;
						}
						for ( h = 0; h < K; h++ )
						{
							ULStreamReadBuffer ( Strm, &SMBuff, 2 );
							BSWAPS ( SMBuff );
							tmp->Tables.Segments.IDDelta[h] = SMBuff;
						}
						for ( h = 0; h < K;	h++ )
						{
							ULStreamReadBuffer ( Strm, &SMBuff, 2 );
							BSWAPS ( SMBuff );
							tmp->Tables.Segments.IDRangeOffset[h] = SMBuff;
						}
						off = ( ( ppInt32 ) ( tmp->Tables.Cmap4Head.Length / 2 ) - 8 -
							4 * ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
						tmp->Tables.GArrayID = ( ppUns16 * ) mmalloc ( Lib,
							( off + 1 ) * sizeof ( ppUns16 ) );
						for ( h = 0; h <= off; h++ )
							read_buffer_from_stream_short ( tmp->Tables.GArrayID[h] );
						break;
					}
					case 1:{ /* CVT */
						tmp->Tables.Cvt = ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Cvt, SZ );   
						break;
					}
					case 2:{ /* FPGM */
                        tmp->FpgmSize = SZ;
						tmp->Tables.Fpgm = ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Fpgm, SZ );   
						break;
					}
					case 3:{ /* GLYPH */
						break;
					}
					case 4:{ /* HEAD */
						tmp->Tables.PHead = ( char * ) mmalloc ( Lib, 54 );
						ULStreamReadBuffer ( Strm, tmp->Tables.PHead, 54 );
						FontBuffer = ( ppUns16 * ) ( tmp->Tables.PHead );
						FontBuffer += 9;
						tmp->UnitsPerEm = *FontBuffer;
						FontBuffer += 16;
						tmp->indexToLocFormat = *FontBuffer;
						BSWAPS ( tmp->indexToLocFormat );
						BSWAPS ( tmp->UnitsPerEm );
						break;
					}
					case 5:{ /* HHEA */
						tmp->Tables.PHhea = ( char * ) mmalloc ( Lib, 36 );
						ULStreamReadBuffer ( Strm, tmp->Tables.PHhea, 36 );
						break;
					}
					case 6: { /* HMTX */
                        tmp->HmtxSize = SZ;
						tmp->Tables.Hmtx = ( ppUns16 * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Hmtx, SZ );   
						break;
					}
					case 7: { /* LOCA */
                        tmp->LocaSize =  SZ;
						tmp->Tables.Loca = ( unsigned char *) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Loca, SZ );   
						break;
					}
					case 8: { /* MAXP */
						tmp->Tables.PMaxp = ( char * ) mmalloc ( Lib, 32 );
						ULStreamReadBuffer ( Strm, tmp->Tables.PMaxp, 32 );
						MaxpBlock = ( ppUns16 * ) ( tmp->Tables.PMaxp + 4 );
						tmp->NumGlyphs = *MaxpBlock;
						BSWAPS ( tmp->NumGlyphs );
						break;
					}
					case 9:{ /* NAME */
                        tmp->Tables.Name =  ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Name, SZ );
                        break;
					}
					case 10:{ /* PREP */
						tmp->Tables.Prep =  ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Prep, SZ );
						break;
					}
					case 11:{ /* POST */
						ppUns32 CurrOffset;
                        ppUns32 * PCurrOffset;
                        tmp->MonoSpace = 0;
						tmp->Tables.Post =  ( char * ) mmalloc ( Lib, SZ );
						ULStreamReadBuffer ( Strm, tmp->Tables.Post, SZ );
                        PCurrOffset =  (ppUns32 *)tmp->Tables.Post + 3;
						CurrOffset = *PCurrOffset;
						BSWAPL ( CurrOffset );
						if ( CurrOffset != 0 )
							tmp->MonoSpace = 1;
                        break;
					}
					default:{
                        PDFRAISE( Lib, cleInvalidFontFormat );
					}
				}
			} PDFFINALLY ( Lib ){
                 ULStreamClose( Strm );			
 				 mfree ( Lib, buf );
			}
			PDFFINALLYEND ( Lib );
		}
		tmp->Tables.BackgroundGlyph = ( PGlyph ) mmalloc ( Lib, sizeof ( TGlyph ) );
		PDFTRY ( Lib ){
			tmp->Tables.BackgroundGlyph->Next = NULL;
			tmp->Tables.BackgroundGlyph->Glyph = NULL;
			TName = InTables[ 3 ];
			BSWAPL( TName );
			SZ = GetFontData ( DC, TName, 0, NULL, 0 );
			if ( SZ == -1 ){
				PDFRAISE( Lib, cleInvalidFontFormat );
			}				
			buf = mmalloc ( Lib, SZ );
			PDFTRY ( Lib ){
				SZ = GetFontData ( DC, TName, 0, buf, SZ );
				if ( SZ == -1 ){
					PDFRAISE( Lib, cleInvalidFontFormat );
				}
				Strm = ULStreamMemReadOnlyNew( Lib, buf, SZ) ;
				ULStreamSetPosition( Strm, 0);
				GlyphNext = tmp->Tables.BackgroundGlyph;
				PHeadBlock = ( ppUns16 * ) ( tmp->Tables.PHead + 50 );
				PHeadLoc = *PHeadBlock;
				BSWAPS ( PHeadLoc );
				for ( i = 1; i <= tmp->NumGlyphs; i++ ){
					if ( PHeadLoc == 1 ){
						LongX = ( ( ppUns8 ) ( tmp->Tables.Loca[i * 4] )     << 24 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 1] ) << 16 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 2] ) << 8 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 3] ) );
						GlyphLength = ( ppUns16 ) 
							( LongX - 
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4] )     << 24 ) -
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 1] ) << 16 ) -
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 2] ) << 8 ) -
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 3] ) ) );
					}else
						GlyphLength = ( ppUns16 )(2 * ((  ( 
						( ( ppUns8 ) ( tmp->Tables.Loca[i * 2] ) ) ) <<  8 ) +
						( ( ppUns8 ) ( tmp->Tables.Loca[i * 2 + 1] ) ) -
						( ( ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 2] ) ) ) << 8 ) -
						( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 2 + 1] ) )));
					if ( GlyphLength )
						GlyphNext->Glyph = ( char * ) mmalloc ( Lib, GlyphLength );
					else
						GlyphNext->Glyph = NULL;
					GlyphNext->Number = (ppUns16)i;
					GlyphNext->Size = GlyphLength;
					GlyphNext->Tag = 0;
					if ( PHeadLoc == 1 ){
						LongX = ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4] )     << 24 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 1] ) << 16 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 2] ) << 8 ) +
							( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 3] ) );
						ULStreamSetPosition ( Strm, LongX );
					}else{
						ShortX = ( tmp->Tables.Loca[( i - 1 ) * 2] << 8 ) +
							( tmp->Tables.Loca[( i - 1 ) * 2 + 1] );        
						ULStreamSetPosition ( Strm, ShortX * 2 );
					}
					ULStreamReadBuffer ( Strm, GlyphNext->Glyph, GlyphLength );
					if ( i != tmp->NumGlyphs ){
						GlyphTmp = ( PGlyph ) mmalloc ( Lib, sizeof ( TGlyph ) );
						GlyphTmp->Next = NULL;
						GlyphTmp->Glyph = NULL;
						GlyphNext->Next = GlyphTmp;
						GlyphNext = GlyphTmp;
					}else
						GlyphNext = NULL;
				}
			} PDFFINALLY ( Lib ){
                ULStreamClose( Strm );			
				mfree ( Lib, buf );
			}
			PDFFINALLYEND ( Lib );
			PDFTTFBuildCodeTable ( Lib, tmp );
		}
		PDFEXCEPT ( Lib ){
			GlyphNext = tmp->Tables.BackgroundGlyph; 
			for ( i = 1; i <= tmp->NumGlyphs; i++ ){
				if ( !GlyphNext )
					break;
				GlyphTmp = GlyphNext->Next;
				chfreez ( Lib, GlyphNext->Glyph );
				mfree ( Lib, GlyphNext );
				GlyphNext = GlyphTmp;
			}
			PDFRERAISE ( Lib );
		}
		PDFTRYEND ( Lib );
   }
   PDFEXCEPT ( Lib ){
	   chfreez ( Lib, tmp->Tables.Prep );
	   chfreez ( Lib, tmp->Tables.Fpgm );
	   chfreez ( Lib, tmp->Tables.Hmtx );
	   chfreez ( Lib, tmp->Tables.Name );
	   chfreez ( Lib, tmp->Tables.Post );
	   chfreez ( Lib, tmp->Tables.Cvt );
	   chfreez ( Lib, tmp->Tables.Loca );
	   chfreez ( Lib, tmp->Tables.PMaxp );
	   chfreez ( Lib, tmp->Tables.PHhea );
	   chfreez ( Lib, tmp->Tables.PHead );
	   chfreez ( Lib, tmp->Tables.GArrayID );
	   chfreez ( Lib, tmp->Tables.Cmap );
	   
	   for ( h = 0; h < tmp->TableNumber; h++ )
		   chfreez ( Lib, tmp->Headers[h] );
	   chfreez ( Lib, tmp->Headers );
	   mfree ( Lib, tmp );
	   PDFRERAISE ( Lib );
   }
   PDFTRYEND ( Lib );
   return tmp;

}
#endif
 
PPDFFontTables PDFTTFLoadFontFromStream( PDFLibHandle Lib, PDFStreamHandle Strm )
{
    ppInt32 i, h, off;  
    PGlyph GlyphNext, GlyphTmp;
    ppUns16 PHeadLoc;
    ppUns16 *PHeadBlock, ShortX;
    ppUns32 CurrOffset, LongX;
    ppUns16 TableIndex, GlyphLength;
    PPDFFontTables tmp;
    CmapInfoRec CmapInfo;
    tmp = ( PPDFFontTables ) mmalloc ( Lib, sizeof ( TPDFFontTables ) );
    memset ( tmp, 0, sizeof ( TPDFFontTables ) );
    PDFTRY ( Lib ){
#ifdef WINDOWS_PLATFORM
		tmp->DCUsedForLoad = false;
#endif
        PDFTTFTablesGetFontHeader ( Lib, tmp, Strm );
        TableIndex = PDFTTFFindTable ( CMAP_TABLE, tmp->Headers,
            tmp->TableNumber );
        CurrOffset = tmp->Headers[TableIndex]->Offset;
        tmp->Tables.Cmap = ( char * ) mmalloc ( Lib, tmp->Headers[TableIndex]->Length );
        ULStreamSetPosition ( Strm, CurrOffset );
        ULStreamReadBuffer ( Strm, tmp->Tables.Cmap,
            tmp->Headers[TableIndex]->Length );
        ULStreamSetPosition ( Strm, CurrOffset );
        read_buffer_from_stream_short ( tmp->Tables.CmapHeader.Version );
        read_buffer_from_stream_short ( tmp->Tables.CmapHeader.NumTables );
        for ( h = 1; h <= tmp->Tables.CmapHeader.NumTables; h++ ){
            read_buffer_from_stream_short ( CmapInfo.PlatformID );
            read_buffer_from_stream_short ( CmapInfo.EncodingID );
            read_buffer_from_stream_long ( CmapInfo.Offset );
            off = CmapInfo.Offset;
            if ( ( CmapInfo.PlatformID == 3 ) && ( CmapInfo.EncodingID == 1 ) )
                break;
        }
        ULStreamSetPosition ( Strm, CurrOffset + off );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Format );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Length );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4Head.Version );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4.SegCountX2 );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4.SearchRange );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4.EntrySelector );
        read_buffer_from_stream_short ( tmp->Tables.Cmap4.RangeShift );
        tmp->Tables.Segments.EndCount = ( ppUns16 * ) mmalloc( Lib, sizeof(ppUns16) * ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
        for ( h = 0; h < ( ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) ); h++ ){
            read_buffer_from_stream_short ( tmp->Tables.Segments.EndCount[h] );
	    }
        read_buffer_from_stream_short ( tmp->Tables.Segments.ZeroByte );
		tmp->Tables.Segments.StartCount = ( ppUns16 * ) mmalloc( Lib, sizeof(ppUns16) * ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
        for ( h = 0; h < ( ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) ); h++ ){
            read_buffer_from_stream_short ( tmp->Tables.Segments.StartCount[h] );
		}
		tmp->Tables.Segments.IDDelta = ( ppUns16 * ) mmalloc( Lib, sizeof(ppUns16) * ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
        for ( h = 0; h < ( ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) ); h++ ){
            read_buffer_from_stream_short ( tmp->Tables.Segments.IDDelta[h] );
		}
        tmp->Tables.Segments.IDRangeOffset = ( ppUns16 * ) mmalloc( Lib, sizeof(ppUns16) * ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
        for ( h = 0; h < ( ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) ); h++ ){
            read_buffer_from_stream_short ( tmp->Tables.Segments.IDRangeOffset[h] );
	    }
        off = ( ( ppInt32 ) ( tmp->Tables.Cmap4Head.Length / 2 ) -
                8 -
                4 * ( ppInt32 ) ( tmp->Tables.Cmap4.SegCountX2 / 2 ) );
        tmp->Tables.GArrayID = ( ppUns16 * ) mmalloc ( Lib,
                                         ( off + 1 ) * sizeof ( ppUns16 ) );
        for ( h = 0; h <= off; h++ )
            read_buffer_from_stream_short ( tmp->Tables.GArrayID[h] );
        ULStreamSetPosition ( Strm, 0 );
        PDFTTFLoadStreamAuxTables ( Lib, tmp, Strm );
        tmp->Tables.BackgroundGlyph = ( PGlyph ) mmalloc ( Lib, sizeof ( TGlyph ) );
        PDFTRY ( Lib ){
            tmp->Tables.BackgroundGlyph->Next = NULL;
            tmp->Tables.BackgroundGlyph->Glyph = NULL;
            TableIndex = PDFTTFFindTable ( GLYPH_TABLE, tmp->Headers,
                tmp->TableNumber );
            CurrOffset = tmp->Headers[TableIndex]->Offset;
            ULStreamSetPosition ( Strm, CurrOffset );
            GlyphNext = tmp->Tables.BackgroundGlyph;
            PHeadBlock = ( ppUns16 * ) ( tmp->Tables.PHead + 50 );
            PHeadLoc = *PHeadBlock;
            BSWAPS ( PHeadLoc );
            for ( i = 1; i <= tmp->NumGlyphs; i++ ){
                if ( PHeadLoc == 1 ){
                    LongX = ( ( ppUns8 ) ( tmp->Tables.Loca[i * 4] )     << 24 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 1] ) << 16 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 2] ) << 8 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[i * 4 + 3] ) );
                    GlyphLength = ( ppUns16 ) 
                        ( LongX - 
                        ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4] )     << 24 ) -
                        ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 1] ) << 16 ) -
                        ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 2] ) << 8 ) -
                        ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 3] ) ) );
                }else
                    GlyphLength = ( ppUns16 )(2 * ((  ( 
                        ( ( ppUns8 ) ( tmp->Tables.Loca[i * 2] ) ) ) <<  8 ) +
                        ( ( ppUns8 ) ( tmp->Tables.Loca[i * 2 + 1] ) ) -
                        ( ( ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 2] ) ) ) << 8 ) -
                        ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 2 + 1] ) )));
                if ( GlyphLength )
                    GlyphNext->Glyph = ( char * ) mmalloc ( Lib, GlyphLength );
                else
                    GlyphNext->Glyph = NULL;
                GlyphNext->Number = (ppUns16)i;
                GlyphNext->Size = GlyphLength;
                GlyphNext->Tag = 0;
                if ( PHeadLoc == 1 ){
                    LongX = ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4] )     << 24 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 1] ) << 16 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 2] ) << 8 ) +
                            ( ( ppUns8 ) ( tmp->Tables.Loca[( i - 1 ) * 4 + 3] ) );
                    ULStreamSetPosition ( Strm, CurrOffset + LongX );
                }else{
                    ShortX = ( tmp->Tables.Loca[( i - 1 ) * 2] << 8 ) +
                             ( tmp->Tables.Loca[( i - 1 ) * 2 + 1] );        
                    ULStreamSetPosition ( Strm, CurrOffset + ShortX * 2 );
                }
                ULStreamReadBuffer ( Strm, GlyphNext->Glyph, GlyphLength );


				if ( i != tmp->NumGlyphs ){
                    GlyphTmp = ( PGlyph ) mmalloc ( Lib, sizeof ( TGlyph ) );
                    GlyphTmp->Next = NULL;
                    GlyphTmp->Glyph = NULL;
                    GlyphNext->Next = GlyphTmp;
                    GlyphNext = GlyphTmp;
                }else
                    GlyphNext = NULL;
            }
            PDFTTFBuildCodeTable ( Lib, tmp );
        }
        PDFEXCEPT ( Lib ){
            GlyphNext = tmp->Tables.BackgroundGlyph; 
            for ( i = 1; i <= tmp->NumGlyphs; i++ ){
                if ( !GlyphNext )
                    break;
                GlyphTmp = GlyphNext->Next;
                chfreez ( Lib, GlyphNext->Glyph );
                mfree ( Lib, GlyphNext );
                GlyphNext = GlyphTmp;
            }
            PDFRERAISE ( Lib );
        } PDFTRYEND ( Lib );
    }
    PDFEXCEPT ( Lib ){
        chfreez ( Lib, tmp->Tables.Prep );
        chfreez ( Lib, tmp->Tables.Fpgm );
        chfreez ( Lib, tmp->Tables.Hmtx );
        chfreez ( Lib, tmp->Tables.Name );
        chfreez ( Lib, tmp->Tables.Post );
        chfreez ( Lib, tmp->Tables.Cvt );
        chfreez ( Lib, tmp->Tables.Loca );
        chfreez ( Lib, tmp->Tables.PMaxp );
        chfreez ( Lib, tmp->Tables.PHhea );
        chfreez ( Lib, tmp->Tables.PHead );
        chfreez ( Lib, tmp->Tables.GArrayID );
        chfreez ( Lib, tmp->Tables.Cmap );
        for ( h = 0; h < tmp->TableNumber; h++ )
            chfreez ( Lib, tmp->Headers[h] );
        chfreez ( Lib, tmp->Headers );
        mfree (  Lib, tmp );
        PDFRERAISE ( Lib );
    } PDFTRYEND ( Lib );
    return tmp;
}

ppUns16 * PDFTTFGetNTableRecord( ppUns16 fName, ppUns16 *SLength, char *Table, ppUns16 NameWindowsUnicode)
{
    char *ResultName = NULL;
    char *Storage;
    ppUns16 NameWindows         = 0x0003;
    ppUns16 NameWindowsEnglish  = 0x0409; 
    ppUns16 *Mark; 
    ppUns16 FormatSelector;
    ppUns16 NumRecords;
    ppUns16 StringsOffset;
    ppUns16 *PlatformID;
    ppUns16 *EncodingID;
    ppUns16 *LanguageID;
    ppUns16 *NameID;
    ppUns16 *StringLength;
    ppUns16 *StringOffset;
    ppUns16 SymbolicOffset;
    Mark = ( ppUns16 * ) ( Table );
    get_word ( FormatSelector );
    get_word ( NumRecords );
    get_word1 ( StringsOffset );
    BSWAPS ( NameWindows );
    BSWAPS ( NameWindowsUnicode );
    BSWAPS ( NameWindowsEnglish );
    BSWAPS ( fName );
    Storage = Table + StringsOffset;
    Mark = ( ppUns16 * ) ( Table ) + 3;
    PlatformID = Mark; 
    EncodingID = Mark + 1; 
    LanguageID = Mark + 2;
    NameID = Mark + 3; 
    StringLength = Mark + 4; 
    StringOffset = Mark + 5; 
    Mark = Mark + 6;
    *SLength = 0;
    while ( NumRecords > 0 ){
        if ( ( ( *PlatformID ) == NameWindows ) &&
             ( ( *EncodingID ) == NameWindowsUnicode ) &&
             ( ( *LanguageID ) == NameWindowsEnglish ) &&
             ( ( *NameID ) == fName ) ){
            SymbolicOffset = ( *StringOffset );
            *SLength = *StringLength;
            BSWAPS ( SymbolicOffset );
            BSWAPS ( *SLength );
            ResultName = Storage + SymbolicOffset;     
            break;
        }
        PlatformID = Mark; 
        EncodingID = Mark + 1; 
        LanguageID = Mark + 2;
        NameID = Mark + 3; 
        StringLength = Mark + 4; 
        StringOffset = Mark + 5; 
        Mark = Mark + 6;
        NumRecords --;
    }
    return ( ppUns16 * ) ResultName;
}

char * PDFTTFGetFontName( PDFLibHandle Lib, ppUns16 *SLength, ppUns16 *FullName )
{
    ppInt32 h;
    char *ReturnName, *Temp;
    ( *SLength ) = ( *SLength ) / 2;
    Temp = ( char * ) FullName + 1;
    ReturnName = ( char * ) mmalloc ( Lib, ( *SLength ) + 1 );
    for ( h = 0; h < ( *SLength ); h++ ){
        ReturnName[h] = *Temp;
        Temp = Temp + 2;
    }
    ReturnName[( *SLength )] = '\0';
    return ReturnName;
}

char * PDFTTFGetFontNameFromTables( PDFLibHandle Lib, pdfTTFSection NameSection, PPDFFontTables Font, 
                                   ppUns16 *SLength )
{
    char *Temp;
    ppUns16 *FullName;
    FullName = PDFTTFGetNTableRecord ( (ppUns16)NameSection, SLength, Font->Tables.Name, 0 );
	if ( *SLength == 0 )
		FullName = PDFTTFGetNTableRecord ( (ppUns16)NameSection, SLength, Font->Tables.Name, 1 );
    Temp = PDFTTFGetFontName ( Lib, SLength, FullName );
    return Temp;
}

char * PDFTTFGetFontNameFromNameTable( PDFLibHandle Lib, pdfTTFSection NameSection, char *NameTable, 
                                      ppUns16 *SLength )
{
    char *Temp;
    ppUns16 *FullName;
    FullName = PDFTTFGetNTableRecord ( (ppUns16)NameSection, SLength, NameTable, 0 );
	if ( *SLength == 0 )
		FullName = PDFTTFGetNTableRecord ( (ppUns16)NameSection, SLength, NameTable, 1 );
    Temp = PDFTTFGetFontName ( Lib, SLength, FullName );   
    return Temp;
}

char * PDFTTFGetNameTable( PDFLibHandle Lib, PDFStreamHandle Strm, ppUns16 *Llen )
{
    PPDFFontTables tmp;
    char *FontBuffer;
    ppUns32 CurrOffset;
    ppUns16 TableIndex;
    tmp = ( PPDFFontTables ) mmalloc ( Lib, sizeof ( TPDFFontTables ) );
    PDFTRY ( Lib ){
        PDFTTFTablesGetFontHeader ( Lib, tmp, Strm );
        TableIndex = PDFTTFFindTable ( NAME_TABLE, tmp->Headers,
                                       tmp->TableNumber );
        CurrOffset = tmp->Headers[TableIndex]->Offset;
        ( *Llen ) = ( ppUns16 ) ( tmp->Headers[TableIndex]->Length );
        ULStreamSetPosition ( Strm, CurrOffset );
        FontBuffer = ( char * ) mmalloc ( Lib, tmp->Headers[TableIndex]->Length );
        PDFTRY ( Lib ){
            ULStreamReadBuffer ( Strm, FontBuffer, tmp->Headers[TableIndex]->Length );
        }
        PDFEXCEPT ( Lib ){
            mfree (  Lib, FontBuffer );
            PDFRERAISE ( Lib );
        }
        PDFTRYEND ( Lib );
        ULStreamSetPosition ( Strm, 0 );
    }
    PDFEXCEPT ( Lib ){
        mfree (  Lib, tmp );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    mfree (  Lib, tmp );
    return FontBuffer;
}

void PDFTTFSetWorkGlyphs( PDFLibHandle Lib, PPDFFontTables Font )
{
    PGlyph GlyphNext, GlyphTmp;
    ppInt32 i;
    PDFTRY(Lib){
        PGlyph GlyphCurr, GlyphTmp, GlyphNext;
        Font->Tables.FGlyph = ( PGlyph ) mmalloc (Lib, sizeof ( TGlyph ) );
        Font->Tables.FGlyph->Next = NULL;
        Font->Tables.FGlyph->Glyph = NULL;
        GlyphTmp = Font->Tables.FGlyph;
        GlyphNext = Font->Tables.BackgroundGlyph;
        GlyphTmp->Glyph = ( char * ) mmalloc ( Lib, GlyphNext->Size );
        memcpy ( GlyphTmp->Glyph, GlyphNext->Glyph, GlyphNext->Size );
        GlyphTmp->Number = GlyphNext->Number;
        GlyphTmp->Size = GlyphNext->Size;
        GlyphTmp->Tag = 0;
        GlyphNext = GlyphNext->Next;
        while( GlyphNext ){
            GlyphCurr = ( PGlyph ) mmalloc ( Lib, sizeof ( TGlyph ) );
            GlyphTmp->Next = GlyphCurr;
            GlyphTmp = GlyphCurr;
            GlyphTmp->Glyph = ( char * ) mmalloc ( Lib, GlyphNext->Size );
            memcpy ( GlyphTmp->Glyph, GlyphNext->Glyph, GlyphNext->Size );
            GlyphTmp->Number = GlyphNext->Number;
            GlyphTmp->Size = GlyphNext->Size;
            GlyphTmp->Tag = 0;
            GlyphNext = GlyphNext->Next;
        }
    }
    PDFEXCEPT(Lib){
        GlyphNext = Font->Tables.FGlyph; 
        for ( i = 1; i <= Font->NumGlyphs; i++ ){
            if ( !GlyphNext )
                break;
            GlyphTmp = GlyphNext->Next;
            chfreez ( Lib, GlyphNext->Glyph );
            mfree ( Lib, GlyphNext );
            GlyphNext = GlyphTmp;
        }
        PDFRERAISE(Lib);
    }
    PDFTRYEND(Lib);
}

void PDFTTFFreeGlyphBuffer( PDFLibHandle Lib, PPDFFontTables Font )
{
    ppInt32 i;
    PGlyph GlyphNext, aux_glyph;
    GlyphNext = Font->Tables.FGlyph; 
    for ( i = 1; i <= Font->NumGlyphs; i++ ){
        aux_glyph = GlyphNext->Next;
        if (!aux_glyph) break;
        chfreez ( Lib, GlyphNext->Glyph );
        mfree ( Lib, GlyphNext );
        GlyphNext = aux_glyph;
    }
}

void PDFTTFGetGlyph( PPDFFontTables Font, ppUns16 SymbolCode)
{
    ppInt32 i;
    long Offset;
    ppUns16 UnicodeGlyph= 0;
    PGlyph GlyphNext;
    for ( i = 0; i < ( Font->Tables.Cmap4.SegCountX2 / 2 ); i++ )
        if ( ( SymbolCode <= Font->Tables.Segments.EndCount[i] ) &&
            ( SymbolCode >= Font->Tables.Segments.StartCount[i] ) ){
                if ( Font->Tables.Segments.IDRangeOffset[i] == 0 )
                    UnicodeGlyph = SymbolCode + Font->Tables.Segments.IDDelta[i];
                else{
                    Offset = i + ( ppInt32 ) ( Font->Tables.Segments.IDRangeOffset[i] / 2 ) -
                        ( ppInt32 ) ( Font->Tables.Cmap4.SegCountX2 / 2 ) +
                        SymbolCode -  Font->Tables.Segments.StartCount[i];
                    UnicodeGlyph = ( ppUns16 ) ( Font->Tables.GArrayID[Offset] ) +
                        Font->Tables.Segments.IDDelta[i];
                }
                break;
            }else
                UnicodeGlyph = 0;
            GlyphNext = Font->Tables.FGlyph;
            for ( i = 1; i <= Font->NumGlyphs; i++ ){
                if ( i == UnicodeGlyph + 1 ){
                    GlyphNext->Tag = 1;
                    GlyphNext->Index = SymbolCode;
                    break;
                }
                GlyphNext = GlyphNext->Next;
            }
}

void PDFTTFCompositGlyph( PPDFFontTables Font, PGlyph Glyph )
{
    ppInt32 j, x;
    PGlyph GlyphNext;
    ppInt32 Index   = 11;
    GlyphNext = Font->Tables.FGlyph;
    j = ( ( ppUns8 ) Glyph->Glyph[Index + 1] << 8 ) +
        ( ( ppUns8 ) Glyph->Glyph[Index + 2] );
    for ( x = 1; x <= j; x++ )
        GlyphNext = GlyphNext->Next;
    GlyphNext->Tag = 1;
    if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
        ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
        PDFTTFCompositGlyph ( Font, GlyphNext );
    while ( true ){
        if ( ( ( ppUns8 ) Glyph->Glyph[Index] & 33 ) == 33 )
            Index = Index + 8;
        else if ( ( ( ppUns8 ) Glyph->Glyph[Index] & 32 ) == 32 )
            Index = Index + 6;
        else
            break;
        j = ( ( ppUns8 ) Glyph->Glyph[Index + 1] << 8 ) +
            ( ( ppUns8 ) Glyph->Glyph[Index + 2] );
        GlyphNext = Font->Tables.FGlyph;
        for ( x = 1; x <= j; x++ )
            GlyphNext = GlyphNext->Next;
        GlyphNext->Tag = 1;
        if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
            ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
            PDFTTFCompositGlyph ( Font, GlyphNext );
    }
    return;
}

void PDFTTFEditGlyphLinks( PPDFFontTables Font, TGlyph *Glyph, ppInt32 *GlyphLinks )
{
    ppInt32 j;
    long x;
    TGlyph *GlyphNext;
    ppInt32 Index;
    ppUns16 *GlyphNewInd, gni;
    GlyphNext = Font->Tables.FGlyph;
    Index = 11;
    j = ( ( ppUns8 ) Glyph->Glyph[Index + 1] << 8 ) +
        ( ( ppUns8 ) Glyph->Glyph[Index + 2] );
    GlyphNewInd = ( ppUns16 * ) ( Glyph->Glyph + Index + 1 );
    gni = PDFTTFEditLinkCompression ( GlyphLinks, j );
    BSWAPS ( gni );
    *GlyphNewInd = gni;
    for ( x = 1; x <= j; x++ )
        GlyphNext = GlyphNext->Next;
    if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
        ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
        PDFTTFEditGlyphLinks ( Font, Glyph, GlyphLinks );
    while ( true ){
        if ( ( ( ppUns8 ) Glyph->Glyph[Index] & 33 ) == 33 )
            Index = Index + 8;
        else if ( ( ( ppUns8 ) Glyph->Glyph[Index] & 32 ) == 32 )
            Index = Index + 6;
        else
            break;
        j = ( ( ppUns8 ) Glyph->Glyph[Index + 1] << 8 ) +
            ( ( ppUns8 ) Glyph->Glyph[Index + 2] );
        GlyphNewInd = ( ppUns16 * ) ( Glyph->Glyph + Index + 1 );
        gni = PDFTTFEditLinkCompression ( GlyphLinks, j );
        BSWAPS ( gni );
        *GlyphNewInd = gni;
        GlyphNext = Font->Tables.FGlyph;
        for ( x = 1; x <= j; x++ )
            GlyphNext = GlyphNext->Next;
        if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
            ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
            PDFTTFEditGlyphLinks ( Font, Glyph, GlyphLinks );
    }
}

ppUns32 PDFTTFCutSelectedGlyphs( PDFLibHandle Lib, PPDFFontTables Font, ppBool Check )
{
    ppInt32 i;
    char *ModLoca;
    ppUns32 GlyphCount, LongOffset;
    ppUns16 ShortOffset;
    PGlyph GlyphNext;
    ppInt32 *GlyphLinks;
    ppUns32 LocaOffset = 0;
    ppUns16 SLocaOffset= 0;
    GlyphCount = 0;
    GlyphNext = Font->Tables.FGlyph;
    for ( i = 1; i < Font->NumGlyphs; i++ ){
        if ( ( GlyphNext->Tag == 1 ) && ( GlyphNext->Size != 0 ) )
            if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
                ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
                PDFTTFCompositGlyph ( Font, GlyphNext );         
        GlyphNext = GlyphNext->Next;
    }
    GlyphNext = Font->Tables.FGlyph;
    if ( Check ){
        GlyphLinks = ( ppInt32 * ) mmalloc ( Lib,
            Font->NumGlyphs * sizeof ( ppInt32 ) );
        for ( i = 1; i <= Font->NumGlyphs; i++ ){
            if ( GlyphNext->Tag == 1 ){
                GlyphLinks[i - 1] = GlyphCount;
                GlyphCount++;
            }else
                GlyphNext->Size = 0;
            GlyphNext = GlyphNext->Next;
        }
        GlyphNext = Font->Tables.FGlyph;
        if ( Check == 1 )
            for ( i = 1; i <= Font->NumGlyphs; i++ ){
                if ( ( GlyphNext->Tag == 1 ) && ( GlyphNext->Size != 0 ) )
                    if ( ( ( ppUns8 ) GlyphNext->Glyph[0] == 255 ) &&
                        ( ( ppUns8 ) GlyphNext->Glyph[1] == 255 ) )
                        PDFTTFEditGlyphLinks ( Font, GlyphNext, GlyphLinks );
                GlyphNext = GlyphNext->Next;
            }
            mfree ( Lib, GlyphLinks );
            return GlyphCount;
    }else{
        for ( i = 1; i <= Font->NumGlyphs + 1; i++ ){
            if ( i <= Font->NumGlyphs )
                if ( GlyphNext->Tag == 0 )
                    GlyphNext->Size = 0;
            if ( Font->indexToLocFormat == 1 ){
                LongOffset = LocaOffset;
                BSWAPL ( LongOffset );
                ModLoca = ( char * ) &LongOffset;
                Font->Tables.Loca[( i - 1 ) * 4] = ModLoca[0];
                Font->Tables.Loca[( i - 1 ) * 4 + 1] = ModLoca[1];
                Font->Tables.Loca[( i - 1 ) * 4 + 2] = ModLoca[2];
                Font->Tables.Loca[( i - 1 ) * 4 + 3] = ModLoca[3];
                if ( i <= Font->NumGlyphs )
                    LocaOffset = LocaOffset + GlyphNext->Size;
            }else{
                ShortOffset = ( ppInt16 ) ( SLocaOffset / 2 );
                BSWAPS ( ShortOffset );
                ModLoca = ( char * ) &ShortOffset;
                Font->Tables.Loca[( i - 1 ) * 2] = ModLoca[0]; 
                Font->Tables.Loca[( i - 1 ) * 2 + 1] = ModLoca[1];
                if ( i <= Font->NumGlyphs )
                    SLocaOffset = SLocaOffset + GlyphNext->Size;
            }
            if ( i <= Font->NumGlyphs )
                GlyphNext = GlyphNext->Next;
        }
        return 0;
    }
}

void PDFTTFQuickSort( PGlyph *A, ppInt32 iLo, ppInt32 iHi, ppInt32 from_unicode )
{
    ppInt32 Lo, Hi, Mid;
    PGlyph T;
    Lo = iLo;
    Hi = iHi;
    if ( from_unicode == 1 )
        Mid = A[( Lo + Hi ) / 2]->Unicode;
    else
        Mid = A[( Lo + Hi ) / 2]->InCode;
    do{
        if ( from_unicode == 1 ){
            while ( A[Lo]->Unicode < Mid ){
                Lo++;
            }
            while ( A[Hi]->Unicode > Mid ){
                Hi--;
            }
        }else{
            while ( A[Lo]->InCode < Mid ){
                Lo++;
            }
            while ( A[Hi]->InCode > Mid ){
                Hi--;
            }
        }
        if ( Lo <= Hi ){
            T = A[Lo];
            A[Lo] = A[Hi];
            A[Hi] = T;
            Lo++;
            Hi--;
        }
    }while ( Lo <= Hi );
    if ( Hi > iLo )
        PDFTTFQuickSort ( A, iLo, Hi, from_unicode );
    if ( Lo < iHi )
        PDFTTFQuickSort ( A, Lo, iHi, from_unicode );
}

ppUns32 PDFTTFCreateCMapTemplate( PDFLibHandle Lib, char **out_buffer, ppUns16 GlyphCount,
								 PGlyph * GlyphList)
{
	PDFStreamHandle TempStream, InclStream;
	ppUns8 cmapinc, temp_b;
	ppUns16 PrevCode, t_short;
	ppUns32 h,CmapSize, SearchRange, Selector;
	ppUns16 TmpW, SegCount, x;
	ppUns16 stmp_w;
	ppUns16 CurrentCode;
	PTableSegment Segments;
	TempStream = ULStreamMemNew ( Lib, 0 );
	PDFTRY ( Lib ){
		ULStreamWriteBuffer( TempStream, &CmHeaher[0], 59 );   
		PrevCode = 32;
		cmapinc = 2;
		for ( x = 0; x < GlyphCount; x++ ){
			if ( GlyphList[x]->InCode != 0 ){
				CurrentCode = GlyphList[x]->InCode;
				for ( h = 1; ( ppInt16 ) h < ( CurrentCode - PrevCode ); h++ )
					ULStreamWriteChar ( TempStream, 0);   
				PrevCode = CurrentCode;
				cmapinc++;
				ULStreamWriteChar( TempStream, cmapinc );
			}
		}
		PrevCode = 32;
		if ( GlyphCount > 1 )
			PDFTTFQuickSort ( GlyphList, 0, GlyphCount - 1, 1 ); 
		SegCount = 1;
		Segments = ( PTableSegment ) mmalloc ( Lib, SegCount * sizeof ( TTableSegment ) );
		PDFTRY ( Lib )
			Segments[0].StartCode = 32;
		Segments[0].IDRange = ( 2 - Segments[0].StartCode );
		for ( x = 0; x < GlyphCount; x++ ) {
			if ( GlyphList[x]->Index != 0 ) { 
				CurrentCode = GlyphList[x]->Unicode;
				if ( ( CurrentCode - PrevCode ) >= 2 ) {
					Segments[SegCount - 1].EndCode = ( ppUns16 ) PrevCode;
					enlarge_segment;
					Segments[SegCount - 1].StartCode = ( ppUns16 ) CurrentCode; 
					Segments[SegCount - 1].IDRange = GlyphList[x]->Number -
						GlyphList[x]->Unicode - 1;
				}
				else {
					if ( x > 0 ) {
						if ( ( GlyphList[x]->GlyphID < GlyphList[x - 1]->GlyphID ) || 
							( GlyphList[x]->GlyphID - GlyphList[x - 1]->GlyphID >= 2 ) ) {
								Segments[SegCount - 1].EndCode = ( ppUns16 ) PrevCode;
								enlarge_segment;
								Segments[SegCount - 1].StartCode = ( ppUns16 ) CurrentCode; 
								Segments[SegCount - 1].IDRange = GlyphList[x]->Number -
									GlyphList[x]->Unicode - 1;
							}
					}
				}
				PrevCode = CurrentCode;
			}
		}
		Segments[SegCount - 1].EndCode = PrevCode;
		enlarge_segment;
		Segments[SegCount - 1].EndCode = 0xFFFF;
		Segments[SegCount - 1].StartCode = 0xFFFF;
		TmpW = ( ppUns16 ) ULStreamGetSize ( TempStream );
		while ( TmpW < 283 ){
			ULStreamWriteChar( TempStream, 0 );
			TmpW++;
		}
		temp_b = 4;
		ULStreamWriteChar ( TempStream, temp_b );
		InclStream = ULStreamMemNew ( Lib, 0 );
		PDFTRY ( Lib ){
			TmpW = 0;
			ULStreamWriteBuffer ( InclStream, &TmpW, 2 );
			send_buffer ( TmpW, SegCount * 2 );
			SearchRange = ( 2 * ( ppUns32 ) ( pow ( ( ppReal )2, log2 ( ( ppReal ) SegCount ) ) + 0.5 ) );
			send_buffer ( TmpW, ( ppUns16 ) SearchRange );
			Selector = ( ppUns32 ) ( log2 ( ( ppReal )SearchRange ) / 2 );
			send_buffer ( TmpW, ( ppUns16 ) Selector );
			send_buffer ( TmpW, ( ppUns16 ) ( 2 * SegCount - SearchRange ) );
			for ( x = 0; x < SegCount; x++ )
				send_buffer ( TmpW, Segments[x].EndCode );
			send_buffer ( TmpW, 0 );
			for ( x = 0; x < SegCount; x++ )
				send_buffer ( TmpW, Segments[x].StartCode );
			for ( x = 0; ( ppInt16 ) x < SegCount - 1; x++ )
				send_buffer ( stmp_w, Segments[x].IDRange );
			send_buffer ( TmpW, 1 );
			TmpW = 0;
			for ( x = 0; x < SegCount; x++ )
				ULStreamWriteBuffer ( InclStream, &TmpW, 2 );
			send_buffer ( TmpW, 2 );
			cmapinc = 2; x = 1;
			while ( x < GlyphCount ){
				if ( GlyphList[x]->Index != 0 ){
					cmapinc++;
					send_buffer ( TmpW, cmapinc );
				}
				x++;
			}
			cmapinc++;
			send_buffer ( TmpW, cmapinc );
			t_short = ( ppUns16 ) ( ULStreamGetSize ( InclStream ) + 2 );
			BSWAPS ( t_short );
			ULStreamWriteBuffer ( TempStream, &t_short, 2 );
			ULStreamWriteBuffer ( TempStream, ULStrmGetMem ( InclStream ),
				ULStreamGetSize ( InclStream ) );
			CmapSize = ULStreamGetSize ( TempStream );
			*out_buffer = ( char * ) mmalloc ( Lib, CmapSize );
			memcpy ( *( out_buffer ), ULStrmGetMem ( TempStream ), CmapSize );

		}
		PDFFINALLY ( Lib){
			ULStreamClose(InclStream);
		}
		PDFFINALLYEND ( Lib );
		PDFFINALLY ( Lib ){
			mfree ( Lib, Segments );
		}
		PDFFINALLYEND ( Lib );
	}
	PDFFINALLY ( Lib ){
		ULStreamClose ( TempStream );
	}
	PDFFINALLYEND ( Lib );
	return CmapSize;
}  

ppUns16 PDFTTFGetSymbolUnicode( PPDFFontTables tmp, ppUns16 SymbolID )
{
    ppInt32 i, h;
    ppUns32 off;
    ppUns16 UnicodeGlyph;
    for ( i = 0; i < ( tmp->Tables.Cmap4.SegCountX2 >> 1 ); i++ )
        for ( h = tmp->Tables.Segments.StartCount[i];
            h <= tmp->Tables.Segments.EndCount[i];
            h++ ){
                if ( tmp->Tables.Segments.IDRangeOffset[i] == 0 )
                    UnicodeGlyph = (ppUns16)(h + tmp->Tables.Segments.IDDelta[i]);
                else{
                    off = i +
                        ( tmp->Tables.Segments.IDRangeOffset[i] >> 1 ) -
                        ( tmp->Tables.Cmap4.SegCountX2 >> 1 ) +
                        h -
                        tmp->Tables.Segments.StartCount[i];
                    UnicodeGlyph = ( tmp->Tables.GArrayID[off] ) +
                        tmp->Tables.Segments.IDDelta[i];
                }
                if ( SymbolID == UnicodeGlyph )
                    return (ppUns16)h;
            }
            return 0;
}

void PDFTTFSwapTableHeader( PPDFFontRecord header )
{
    BSWAPL ( header->Name );
    BSWAPL ( header->CheckSum );
    BSWAPL ( header->Offset );
    BSWAPL ( header->Length );
}

ppUns32 PDFTTFCalcTableCheckSum( char *Table, ppUns32 Length )
{
    ppUns32 Sum = 0L;
    ppUns32 gain;
    char *table_stream;
    char *Endptr;
	if ( !Length )
		return 0;
    table_stream = Table;
    gain = ( ( ( Length + 3 ) & ~3 ) - 3 );
    Endptr = table_stream + gain;
    while ( table_stream < Endptr ){
        Sum += *table_stream++;
    }
    return Sum;
}

ppUns32 CalcTableChecksum(ppUns32 *Table, ppUns32 Length)
{
	ppUns32 Sum = 0L;
	ppUns32 *EndPtr = Table+((Length+3) & ~3) / sizeof(ppUns32);

	while (Table < EndPtr)
		Sum += *Table++;
	return Sum;
}


ppUns32 PDFTTFPasteTableHeader( ppUns32 TableName, PPDFFontHeader Headers, ppUns16 CountOfTables,
                               PDFStreamHandle temp, ppUns32 *PrevOffset, ppUns32 *PrevSize, 
                               char *Table )
{
    ppInt32 TableIndex;
	ppUns32 * CHS;
    TableIndex = PDFTTFFindTable ( TableName, Headers, CountOfTables );  
	Headers[TableIndex]->Offset = (*PrevOffset);
    if ( *PrevSize == 0 )
        *PrevSize = Headers[TableIndex]->Length;
    else
        Headers[TableIndex]->Length = *PrevSize;
	CHS = ( ppUns32 *) Table;
    Headers[TableIndex]->CheckSum = //PDFTTF
		CalcTableChecksum ( CHS, //Table,
        *PrevSize );
    PDFTTFSwapTableHeader ( Headers[TableIndex] );
    ULStreamWriteBuffer ( temp, Headers[TableIndex], sizeof ( TPDFFontRecord ) );
    *PrevOffset = *PrevOffset + *PrevSize;
    PDFTTFSwapTableHeader ( Headers[TableIndex] );
    return ( *PrevSize );
}

PDFStreamHandle PDFTTFGetANSIEmbeddedFont( PDFLibHandle Lib, PPDFFontTables Font, ppUns16 **Unicodes, 
                                          ppUns32 CodeLength )
{
    ppUns32 x, CvtSize, FpgmSize, CmapSize;
    PDFStreamHandle temp = NULL, TempStream, GlyphTable, HmtxTable;
    char *OutCmap        = NULL;
    char *OutLoca        = NULL;
	char *Hhea;
	char MikBuffer[2];
	char HheaBuffer[2];
	ppUns16 Blockd;
    PGlyph *GlyphList    = NULL;
    ppUns16 XLoc;
    ppUns32 PrevOffset, PrevSize, LocaOff;
    PGlyph GlyphNext;
    ppUns16 tmpHeadLoc, GlyphCount, Jump, InCode, *tmpHheaTab, HHMins;
    ppUns16 *PHeadBlock, h;
    ppInt32 cm_off, cm_len;
    ppInt32 TableIndex, hmtx_count;
	if ( ( CodeLength == 1 ) && ( *Unicodes[ 0 ] == 32 ) ){
		h = 1;
		Jump = PDFTTFGetSymbolUnicode ( Font, ( ppUns16 )( ( h ) - 1 ) );
		while ( ( Jump <= 32 ) && ( ULUnicodeToANSI ( Jump ) <= 32 ) ){
			h++;
			Jump = PDFTTFGetSymbolUnicode ( Font, ( ppUns16 ) ( ( h ) - 1 ) );
		}
        (*Unicodes) = ( ppUns16 * ) mrealloc ( Lib, (*Unicodes), sizeof ( ppUns16 ) * 2 );
        (*Unicodes)[1] = Jump;
        CodeLength = 2;
	}
    GlyphTable =ULStreamMemNew(Lib,0);
    PDFTRY ( Lib ){
        PDFTTFSetWorkGlyphs ( Lib, Font );
		for ( h = 0; h < (ppUns16)CodeLength; h++ ){
			if ( (*Unicodes)[h] != 32 )
			    PDFTTFGetGlyph ( Font, (*Unicodes)[ h ] );
		}
        PDFTTFCutSelectedGlyphs ( Lib, Font, 1 ); 
        GlyphNext = Font->Tables.FGlyph; 
        GlyphCount = 0;
		for ( h = 1; h <= Font->NumGlyphs; h++ ){
			if ( GlyphNext->Size > 0 ){
				GlyphCount++; 
				ULStreamWriteBuffer ( GlyphTable, GlyphNext->Glyph, GlyphNext->Size );
				GlyphList = ( PGlyph * ) mrealloc ( Lib, GlyphList,
					GlyphCount * sizeof ( TGlyph ) ); 
				GlyphList[GlyphCount - 1] = GlyphNext;
				GlyphList[GlyphCount - 1]->GlyphID = ( ppUns16 ) ( h );
				Jump = PDFTTFGetSymbolUnicode ( Font, ( ppUns16 ) ( ( h ) - 1 ) );
				InCode = ULUnicodeToANSI ( Jump );
				if ( InCode == 0 ){
					GlyphList[GlyphCount - 1]->InCode = 0;
					GlyphList[GlyphCount - 1]->Unicode = 0;
				}else{
					GlyphList[GlyphCount - 1]->InCode = InCode;
					GlyphList[GlyphCount - 1]->Unicode = ULANSIToUnicode ( ( ppUns8 )
						InCode );
				}
				GlyphList[GlyphCount - 1]->Number = GlyphCount + 3;
			} 
        GlyphNext = GlyphNext->Next;
        }
        if ( GlyphCount > 1 )
            PDFTTFQuickSort ( GlyphList, 0, GlyphCount - 1, 0 ); 
        CmapSize = PDFTTFCreateCMapTemplate ( Lib, &OutCmap, GlyphCount, GlyphList);
		PrevOffset = 12;
        if ( ! Font->IsFPGMPresent )
			PrevOffset--;
		if ( ! Font->IsCVTPPresent )
			PrevOffset--;
		if ( ! Font->IsPREPPresent )
			PrevOffset--;
        PrevOffset = 12 + ( 16 * PrevOffset ); 
        PrevSize = CmapSize;
        temp = ULStreamMemNew ( Lib, 0 );
        PDFTRY ( Lib ){
			ULStreamWriteBuffer ( temp, &OutHeader[0], 12 );
			/*  CMAP */
            TableIndex = PDFTTFFindTable ( CMAP_TABLE, Font->Headers,
                Font->TableNumber );
            cm_off = Font->Headers[TableIndex]->Offset;
            cm_len = Font->Headers[TableIndex]->Length;
            PDFTTFPasteTableHeader ( CMAP_TABLE, Font->Headers,
                Font->TableNumber, temp, &PrevOffset,
                &PrevSize, OutCmap ); 
            Font->Headers[TableIndex]->Offset = cm_off;
            Font->Headers[TableIndex]->Length = cm_len;
            PrevSize = 0; 
			if ( Font->IsCVTPPresent ){
				PDFTTFPasteTableHeader ( CVT_TABLE, Font->Headers,
					Font->TableNumber, temp, &PrevOffset, &PrevSize,
					Font->Tables.Cvt ); 
				CvtSize = PrevSize; 
				PrevSize = 0;
			}
			if ( Font->IsFPGMPresent ){
				PDFTTFPasteTableHeader ( FPGM_TABLE, Font->Headers,
					Font->TableNumber, temp, &PrevOffset, &PrevSize,
					Font->Tables.Fpgm );
				FpgmSize = PrevSize;
			}
		    PrevSize = ULStreamGetSize ( GlyphTable );
			PDFTTFPasteTableHeader ( GLYPH_TABLE, Font->Headers, Font->TableNumber,
				temp, &PrevOffset, &PrevSize, ( char * ) ULStrmGetMem ( GlyphTable ) );
			MikBuffer[0] = Font->Tables.PHead[50];
			MikBuffer[1] = Font->Tables.PHead[51];
			Font->Tables.PHead[50] = 0;
			Font->Tables.PHead[51] = 0;
			PrevSize = 0;
			PDFTTFPasteTableHeader ( HEAD_TABLE, Font->Headers, Font->TableNumber,
				temp, &PrevOffset, &PrevSize, Font->Tables.PHead );
			PrevSize = 0;
			PDFTTFPasteTableHeader ( HHEA_TABLE, Font->Headers, Font->TableNumber,
				temp, &PrevOffset, &PrevSize, Font->Tables.PHhea );
			x = 0;
            HmtxTable = ULStreamMemNew ( Lib, 0 );
            PDFTRY ( Lib ){
                x = 0;
                for ( h = 0; h < 2; h++ )
                    ULStreamWriteBuffer ( HmtxTable, &x, 4 );
                GlyphNext = Font->Tables.FGlyph;
				TableIndex = PDFTTFFindTable ( HMTX_TABLE, Font->Headers, Font->TableNumber );
				tmpHheaTab = (ppUns16 *)(Font->Tables.PHhea + 34);
				HHMins = *tmpHheaTab;
                BSWAPS ( HHMins );
				hmtx_count = ( Font->Headers[TableIndex]->Length / HHMins);
                ULStreamWriteBuffer ( HmtxTable, &Font->Tables.Hmtx[2], 4 );
                for ( h = 1; h <= Font->NumGlyphs; h++ ){
                    if ( GlyphNext->Size > 0 ){
						if ( ( Font->MonoSpace ) && ( hmtx_count != 4 ) ){
							ULStreamWriteBuffer ( HmtxTable,
								&Font->Tables.Hmtx[ 0 ], 2 ); 
							ULStreamWriteBuffer ( HmtxTable,
								&Font->Tables.Hmtx[ h + 2 ], 2 );
						}
						else{
							ULStreamWriteBuffer ( HmtxTable,
								&Font->Tables.Hmtx[( h * 2 ) - 2], 2 ); 
							ULStreamWriteBuffer ( HmtxTable,
								&Font->Tables.Hmtx[( h * 2 ) - 1], 2 );
						}
                    }
                    GlyphNext = GlyphNext->Next;
                } 
                PrevSize = ( ( GlyphCount + 3 ) * 4 );
                TempStream = ULStreamMemNew ( Lib, 0 );
                PDFTRY ( Lib){
                    PDFTTFPasteTableHeader ( HMTX_TABLE, Font->Headers,
                        Font->TableNumber, temp, &PrevOffset, &PrevSize,
                        ( char * ) ULStrmGetMem ( HmtxTable ) );
                    for ( h = 0; h <= 3; h++ )
                        ULStreamWriteBuffer ( TempStream, &CmHeaher[0], 2 ); 
                    GlyphNext = Font->Tables.FGlyph; 
                    LocaOff = 0; 
                    for ( h = 0; h < Font->NumGlyphs; h++ ){
                        if ( GlyphNext->Size > 0 ){
                            LocaOff = LocaOff + GlyphNext->Size; 
                            XLoc = ( ppUns16 ) ( LocaOff >> 1 ); 
                            BSWAPS ( XLoc ); 
                            ULStreamWriteBuffer ( TempStream, &XLoc, 2 );
                        }
                        GlyphNext = GlyphNext->Next;
                    } 
                    PrevSize = ( ( GlyphCount + 4 ) * 2 ); 
                    OutLoca = ( char * ) mmalloc ( Lib, PrevSize );
                    memcpy ( OutLoca, ULStrmGetMem ( TempStream ), PrevSize );
                    PDFTTFPasteTableHeader ( LOCA_TABLE, Font->Headers,
                        Font->TableNumber, temp, &PrevOffset, &PrevSize,
                        OutLoca );
                    PHeadBlock = ( ppUns16 * ) ( Font->Tables.PMaxp + 4 ); 
                    tmpHeadLoc = *PHeadBlock;
                    *PHeadBlock = ( ppUns16 ) ( GlyphCount + 3 );
                    BSWAPS ( *PHeadBlock );
                    paste_table_tail;
					ULStreamWriteBuffer ( temp, OutCmap, CmapSize );
					if ( Font->IsCVTPPresent )
						ULStreamWriteBuffer ( temp, Font->Tables.Cvt, CvtSize );
					if ( Font->IsFPGMPresent )
					    ULStreamWriteBuffer ( temp, Font->Tables.Fpgm, FpgmSize );
					ULStreamWriteBuffer ( temp, ULStrmGetMem ( GlyphTable ), ULStreamGetSize ( GlyphTable ) );
					ULStreamWriteBuffer ( temp, Font->Tables.PHead, 54 );
                    HheaBuffer[ 0 ] = Font->Tables.PHhea [ 34 ];
                    HheaBuffer[ 1 ] = Font->Tables.PHhea [ 35 ];
					Blockd = GlyphCount + 3;
                    Hhea = (char *)&Blockd;
                    Font->Tables.PHhea [ 34 ] = *(Hhea + 1);
                    Font->Tables.PHhea [ 35 ] = *Hhea;
					ULStreamWriteBuffer ( temp, Font->Tables.PHhea, 36 );
					Font->Tables.PHhea [ 34 ] = HheaBuffer[ 0 ];
					Font->Tables.PHhea [ 35 ] = HheaBuffer[ 1 ];
					ULStreamWriteBuffer ( temp, ULStrmGetMem ( HmtxTable ), ((GlyphCount+3)*4) );
					ULStreamWriteBuffer ( temp, OutLoca, ((GlyphCount+4)*2) );
					ULStreamWriteBuffer ( temp, Font->Tables.PMaxp, 32 );
					ULStreamWriteBuffer ( temp, &NameTable[0], 260 );
					ULStreamWriteBuffer ( temp, &PostTable[0], 52 );
					if ( Font->IsPREPPresent )
						ULStreamWriteBuffer ( temp, Font->Tables.Prep, PrevSize );
                    *PHeadBlock = tmpHeadLoc;
                    Font->Tables.PHead[50] = MikBuffer[0];
                    Font->Tables.PHead[51] = MikBuffer[1];
                }
                PDFFINALLY ( Lib ){
                    chfree ( Lib, OutLoca );
                    ULStreamClose ( TempStream );
                }
                PDFFINALLYEND ( Lib );
            }
            PDFFINALLY ( Lib ){
                ULStreamClose ( HmtxTable );
            }
            PDFFINALLYEND ( Lib );
        }
        PDFEXCEPT ( Lib ){
            ULStreamClose( temp );
            PDFRERAISE ( Lib );
        }
        PDFFINALLYEND ( Lib );  
    }
    PDFFINALLY ( Lib ){
        chfree ( Lib, OutCmap );
        chfree ( Lib, GlyphList );
        ULStreamClose ( GlyphTable );
        PDFTTFFreeGlyphBuffer ( Lib, Font );
    }
    PDFFINALLYEND ( Lib );
    return temp;
}

void PDFTTFAddToStream( PDFStreamHandle Dest, void *Source, ppInt32 Length)
{
    ppUns32 i,j;
    j = conv_size ( Length ) - Length;
    ULStreamWriteBuffer ( Dest, Source, Length );
    for ( i=0; i<j; i++ )
        ULStreamWriteChar ( Dest, 0 );
}

PDFStreamHandle PDFTTFGetUnicodeEmbeddedFont( PDFLibHandle Lib, PPDFFontTables Font)
{
    ppUns32 h;
    PDFStreamHandle temp;
    ppUns32 PrevOffset, PrevSize, GlyphCount = 0;
    ppUns32 CmapSize, head_size, CvtSize, prep_size;
    PDFStreamHandle GlyphTable;
    PGlyph GlyphNext;
    PGlyph *GlyphList   = NULL;
    temp = ULStreamMemNew ( Lib, 0 );
    PDFTRY ( Lib ){
        PDFTTFSetWorkGlyphs ( Lib, Font );
        for ( h = 0; h < Font->CodeLength; h++ ){
            if ( Font->CodeTable[h].Used == 1 )
                PDFTTFGetGlyph ( Font, Font->CodeTable[h].Code);
        }
        PDFTTFCutSelectedGlyphs ( Lib, Font, 0 );
        ULStreamWriteBuffer ( temp, &OutHeader[0], 5 );
        ULStreamWriteChar ( temp, 10 );
        ULStreamWriteBuffer ( temp, &OutHeader[6], 6 );
        PrevOffset = 12 + 160;
        PrevSize = 36;
        PDFTTFPasteTableHeader ( HHEA_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset,
            &PrevSize, Font->Tables.PHhea );
        PrevSize = Font->LocaSize;
        PDFTTFPasteTableHeader ( LOCA_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset,
            &PrevSize, (char *)Font->Tables.Loca );
        update_offset;
        PrevSize = 0;
        CmapSize = PDFTTFPasteTableHeader ( CMAP_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset, &PrevSize,
            Font->Tables.Cmap );
		update_offset;
        PrevSize = 0; 
        head_size = PDFTTFPasteTableHeader ( HEAD_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset, &PrevSize,
            Font->Tables.PHead );
        update_offset;
        PrevSize = 32; 
        PDFTTFPasteTableHeader ( MAXP_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset,
            &PrevSize, Font->Tables.PMaxp );
        update_offset;
        PrevSize = 0;
        CvtSize = PDFTTFPasteTableHeader ( CVT_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset, &PrevSize,
            Font->Tables.Cvt );
        update_offset;
        PrevSize = 0;
        prep_size = PDFTTFPasteTableHeader ( PREP_TABLE, Font->Headers,
            Font->TableNumber, temp, &PrevOffset, &PrevSize,
            Font->Tables.Prep );
        GlyphTable = ULStreamMemNew ( Lib, 0 );
        PDFTRY ( Lib ){
            GlyphNext = Font->Tables.FGlyph; 
            create_glyph_table; 
            update_offset;
            PrevSize = ULStreamGetSize ( GlyphTable );
            PDFTTFPasteTableHeader ( GLYPH_TABLE, Font->Headers,
                Font->TableNumber, temp, &PrevOffset,
                &PrevSize, ( char * ) ULStrmGetMem ( GlyphTable ) );
            update_offset;
            PrevSize = Font->HmtxSize;
            PDFTTFPasteTableHeader ( HMTX_TABLE, Font->Headers,
                Font->TableNumber, temp, &PrevOffset,
                &PrevSize, ( char * ) Font->Tables.Hmtx );
            update_offset;
			if ( Font->IsFPGMPresent ){
				PrevSize = Font->FpgmSize;
				PDFTTFPasteTableHeader ( FPGM_TABLE, Font->Headers,
					Font->TableNumber, temp, &PrevOffset,
					&PrevSize, Font->Tables.Fpgm );
			}
            PDFTTFAddToStream ( temp, Font->Tables.PHhea, 36 );
            PDFTTFAddToStream ( temp, Font->Tables.Loca, Font->LocaSize ); 
            PDFTTFAddToStream ( temp, Font->Tables.Cmap, CmapSize );
            PDFTTFAddToStream ( temp, Font->Tables.PHead, head_size ); 
            PDFTTFAddToStream ( temp, Font->Tables.PMaxp, 32 );
            PDFTTFAddToStream ( temp, Font->Tables.Cvt, CvtSize );
            PDFTTFAddToStream ( temp, Font->Tables.Prep, prep_size ); 
            PDFTTFAddToStream ( temp, ULStrmGetMem ( GlyphTable ),
                ULStreamGetSize ( GlyphTable ) );
            PDFTTFAddToStream ( temp, Font->Tables.Hmtx, Font->HmtxSize );
			if ( Font->IsFPGMPresent )
                PDFTTFAddToStream ( temp, Font->Tables.Fpgm, Font->FpgmSize ); 
        }
        PDFFINALLY ( Lib ){
            chfree ( Lib, GlyphList );
            ULStreamClose( GlyphTable );
        }
        PDFFINALLYEND ( Lib );
        PDFTTFFreeGlyphBuffer ( Lib, Font );
    }
    PDFEXCEPT ( Lib ){
        PDFTTFFreeGlyphBuffer ( Lib, Font );
        ULStreamClose ( temp );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    return temp;
}

void PDFTTFGetRequiredMetrics( PFontMetrics req_tables, PPDFFontTables Font )
{
    ppInt32 f, i;
    ppInt16 *FullName;
    ppUns16 SLength;
    float k_em;
    k_em = ( ( float ) ( Font->UnitsPerEm ) / 1000 );
    req_tables->Flags = 32;
    FullName = ( ppInt16 * ) ( Font->Tables.PHead + 36 ); 

    req_tables->FontBBox.XMin = ( *FullName );                                                      
    BSWAPS ( req_tables->FontBBox.XMin );                                                        
    req_tables->FontBBox.XMin = ( ( ppUns16 ) ( req_tables->FontBBox.XMin/k_em ) );                                            
    FullName++;                                                               

    req_tables->FontBBox.YMin = ( *FullName );                                                      
    BSWAPS ( req_tables->FontBBox.YMin );                                                        
    req_tables->FontBBox.YMin = ( ( ppUns16 ) ( req_tables->FontBBox.YMin/k_em ) );                                            
    FullName++;                                                               

    req_tables->FontBBox.XMax = ( *FullName );                                                      
    BSWAPS ( req_tables->FontBBox.XMax );                                                        
    req_tables->FontBBox.XMax = ( ( ppUns16 ) ( req_tables->FontBBox.XMax/k_em ) );                                            
    FullName++;                                                               

    req_tables->FontBBox.YMax = ( *FullName );                                                      
    BSWAPS ( req_tables->FontBBox.YMax );                                                        
    req_tables->FontBBox.YMax = ( ( ppUns16 ) ( req_tables->FontBBox.YMax/k_em ) );                                            
    FullName++;                                                               
/*    get_head_element ( (req_tables->FontBBox.XMin ) );
    get_head_element ( (req_tables->FontBBox.YMin ) );
    get_head_element ( (req_tables->FontBBox.XMax ) );
    get_head_element1 ( (req_tables->FontBBox.YMax ) );
*/    
    
    get_post_offset ( req_tables->ItalicAngle, 2 );
    get_post_offset ( req_tables->UnderlinePosition, 4 );
    get_post_offset ( req_tables->UnderlineThickness, 5 );
    FullName = ( ppInt16 * ) ( Font->Tables.PHhea + 4 );
    req_tables->Ascent = ( *FullName );
    FullName = FullName + 1;
    req_tables->Descent = ( *FullName );
	BSWAPS ( req_tables->Ascent );
	BSWAPS ( req_tables->Descent);
    req_tables->Ascent = ( ( ppUns16 ) ( req_tables->Ascent / k_em ) );
    req_tables->Descent = ( ( ppUns16 ) ( req_tables->Descent / k_em ) );
    req_tables->CapHeight = 667;
    req_tables->StemV = 87;
    memset ( req_tables->ANSICodes, 0, ( 256 * sizeof ( ppInt16 ) ) );
    for ( i = 0; i < ( Font->Tables.Cmap4.SegCountX2 / 2 ); i++ )
        for ( f = Font->Tables.Segments.StartCount[i];
            f <= Font->Tables.Segments.EndCount[i];
            f++ ){
                if ( ( f > 0 ) && ( f < 255 ) )
                    req_tables->ANSICodes[f] = PDFTTFGetSymbolWidth ( Font,
                    PDFTTFGetSymbolID ( Font, (ppUns16)f, i ));
            }
}

PCodeTableNode PDFTTFGetCharCodeTableByIndex(PPDFFontTables Font, ppUns16 GlyphIndex)
{
	ppUns32 i;
	for ( i = 0; i < Font->CodeLength; i++)
		if ( Font->CodeTable [ i ].ID == GlyphIndex ){
			return &Font->CodeTable[i];
		}
	return &Font->CodeTable[0];
}

PCodeTableNode PDFTTFGetCharCodeTable( PPDFFontTables Font, ppUns16 Unicode )
{
    ppUns32 aprox_center;
    ppUns32 half_length;
    ppUns32 table_begin = 0, table_len = Font->CodeLength;
    while ( table_len > 1 ){
        half_length = table_len >> 1;
        aprox_center = table_begin + half_length;
        if ( Unicode > Font->CodeTable[aprox_center].Code ){
            table_begin = aprox_center;
            table_len = table_len - half_length;
            continue;
        };
        if ( Unicode < Font->CodeTable[aprox_center].Code ){
            table_len = half_length;
            continue;
        };
        return &Font->CodeTable[aprox_center];
    };
    if ( Unicode == Font->CodeTable[table_begin].Code )
        return &Font->CodeTable[table_begin];
    return &Font->CodeTable[0];
}

ppInt32 PDFTTFGetCharID( PPDFFontTables Tables, ppUns16 Character, ppBool *Modified )
{
    PCodeTableNode cc;
    cc = PDFTTFGetCharCodeTable ( Tables, Character );
    if ( !( cc->Used ) ){
        cc->Used = 1;
        if ( Modified )
            *Modified = true;
    }
    return cc->ID;
}

ppUns16 PDFTTFGetCharWidth( PPDFFontTables Tables, ppUns16 Character )
{
    PCodeTableNode cc;
    cc = PDFTTFGetCharCodeTable ( Tables, Character );
    return cc->Width;
}

void PDFTTFFreeFont( PDFLibHandle Lib, PPDFFontTables Font )
{
    ppInt32 i;
    PGlyph GlyphNext, aux_glyph;
    GlyphNext = Font->Tables.BackgroundGlyph; 
    for ( i = 1; i <= Font->NumGlyphs; i++ ){
        aux_glyph = GlyphNext->Next;
        mfree ( Lib, GlyphNext->Glyph );
        mfree ( Lib, GlyphNext );
        GlyphNext = aux_glyph;
    }
    for ( i = 0; i < Font->TableNumber; i++ )
        mfree ( Lib, Font->Headers[i] );
	mfree ( Lib, Font->Tables.Segments.EndCount );
    mfree ( Lib, Font->Tables.Segments.StartCount );
    mfree ( Lib, Font->Tables.Segments.IDDelta );
    mfree ( Lib, Font->Tables.Segments.IDRangeOffset );
    mfree ( Lib, Font->Tables.PHead );
    mfree ( Lib, Font->Tables.PHhea );
    mfree ( Lib, Font->Tables.PMaxp );
    mfree ( Lib, Font->Headers );
    mfree ( Lib, Font->Tables.Prep );
    mfree ( Lib, Font->Tables.Fpgm );
    mfree ( Lib, Font->Tables.Hmtx );
    mfree ( Lib, Font->Tables.Name );
    mfree ( Lib, Font->Tables.Post );
    mfree ( Lib, Font->Tables.Cvt );
    mfree ( Lib, Font->Tables.Loca );
    mfree ( Lib, Font->Tables.Cmap );
    mfree ( Lib, Font->Tables.GArrayID );
    mfree ( Lib, Font->CodeTable );
    mfree ( Lib, Font );
}
