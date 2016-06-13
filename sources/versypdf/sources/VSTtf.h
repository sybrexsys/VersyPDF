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
VSTtf.h
*********************************************************************/

#ifndef VSTtf_H
#define VSTtf_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "../VSTypes.h"

#ifdef WINDOWS_PLATFORM

#include <windows.h>

#endif /* WINDOWS_PLATFORM */

#ifdef __cplusplus
extern "C"{
#endif

	typedef enum {
		ppFontFamilyName            = 1,
		ppFontPSName                = 6
	} pdfTTFSection;

	typedef struct TPDFFontRecord {
		ppUns32 Name;
		ppInt32 CheckSum;
		ppUns32 Offset;
		ppUns32 Length;
	} * PPDFFontRecord, TPDFFontRecord;

	typedef struct TRectangle {
		ppInt16 XMin;
		ppInt16 YMin;
		ppInt16 XMax;
		ppInt16 YMax;
	} * PRectangle, TRectangle;

	typedef PPDFFontRecord *PPDFFontHeader;

	typedef struct TFontMetrics {
		ppInt32     Flags;
		ppInt16     ANSICodes[256];
		TRectangle  FontBBox;
		ppInt16     ItalicAngle;
		ppInt16     Ascent;
		ppInt16     Descent;
		ppInt32     CapHeight;
		ppInt32     StemV;
		ppInt16     UnderlinePosition;
		ppInt16     UnderlineThickness;
	} * PFontMetrics, TFontMetrics;

	typedef struct TTableSegment {
		ppUns16 StartCode;
		ppUns16 EndCode;
		ppUns16 IDRange;
	} * PTableSegment, TTableSegment;

	typedef struct TCodeTableNode {
		ppUns16 Code;
		ppUns16 ID;
		ppUns16 Width;
		ppBool  Used;
	} * PCodeTableNode, TCodeTableNode;

	typedef struct tpdffont {
		void    *pdffontbuffer;
		ppUns32 pdffontsize;
	} * ppdffont, tpdffont;

	typedef struct TCmap4 {
		ppUns16 SegCountX2;
		ppUns16 SearchRange;
		ppUns16 EntrySelector;
		ppUns16 RangeShift;
	} * PCmap4, TCmap4;

	typedef struct CmapHeaderRec {
		ppUns16 Version;
		ppUns16 NumTables;
	} * PCmapHeaderRec, CmapHeaderRec;

	typedef struct TCmap4Segment {
		ppUns16 * EndCount;
		ppUns16 ZeroByte;
		ppUns16 * StartCount;
		ppUns16 * IDDelta;
		ppUns16 * IDRangeOffset;
	} * PCmap4Segment, TCmap4Segment;

	typedef struct TCmap4Head {
		ppUns16 Format;
		ppUns16 Length;
		ppUns16 Version;
	} * PCmap4Head, TCmap4Head;

	typedef struct TGlyph {
		ppUns16         Number;
		ppUns16         Unicode;
		ppUns16         InCode;
		ppUns16         Size;
		ppUns16         GlyphID;
		char            *Glyph;
		struct TGlyph   *Next;
		ppUns16         Tag;
		ppUns16         Index;
	} * PGlyph, TGlyph;

	typedef struct TPDFFontTableCollection {
		CmapHeaderRec   CmapHeader;
		TCmap4Head      Cmap4Head;
		TCmap4          Cmap4;
		TCmap4Segment   Segments;
		PppUns16        GArrayID;
		ppUns16         *Hmtx;
		char            *PHead;
		char            *PHhea;
		char            *PMaxp;
		char            *Prep;
		char            *Fpgm;
		char            *Post;
		char            *Cvt;
		char            *Name;
		unsigned char   *Loca;
		char            *Cmap;
		PGlyph          FGlyph;
		PGlyph          BackgroundGlyph;
	} * PPDFFontTableCollection, TPDFFontTableCollection;

	typedef struct TPDFFontTables {
#ifdef WINDOWS_PLATFORM
		ppBool					DCUsedForLoad;
#endif
		ppUns16                 TableNumber;
		ppUns16                 SearchRange;
		ppUns16                 EntrySelector;
		ppUns16                 RangeShift;
		ppUns16                 NumGlyphs;
		ppUns16                 indexToLocFormat;
		ppBool                  MonoSpace;
		ppInt16                 UnitsPerEm;
		ppUns32                 LocaSize;
		ppUns32                 HmtxSize;
		ppUns32                 FpgmSize;
		ppUns32                 CodeLength;
		PPDFFontHeader          Headers;
		TPDFFontTableCollection Tables;
		PCodeTableNode          CodeTable;
		ppBool                  IsFPGMPresent;
		ppBool					IsCVTPPresent;
		ppBool					IsPREPPresent;
	} * PPDFFontTables, TPDFFontTables;

	typedef struct CmapInfoRec {
		ppUns16 PlatformID;
		ppUns16 EncodingID;
		ppUns32 Offset;
	} * PCmapInfoRec, CmapInfoRec; 

	/********************************************************************
	Description: 
	Loads font stream and parse tables.
	Parameters: 
	PDFLibHandle lib     - [in] Current lib handle.
	PDFStreamHandle Strm - [in] Font stream.
	Returns:
	Temporary font tables.
	*********************************************************************/
	PPDFFontTables PDFTTFLoadFontFromStream(PDFLibHandle Lib, PDFStreamHandle Strm);

	/********************************************************************
	Description: 
	Returns name of the font from name table.
	Parameters: 
	PDFLibHandle lib          - [in] Current lib handle.
	pdfTTFSection NameSection - [in] Type of name section.
	char *NameTable           - [in] Current name table.
	ppUns16 *SLength          - [out] Font name length.
	Returns:
	Font name.
	*********************************************************************/	
	char *PDFTTFGetFontNameFromNameTable(PDFLibHandle Lib, pdfTTFSection NameSection, char *NameTable, ppUns16 *SLength);

	/********************************************************************
	Description: 
	Returns name table.
	Parameters: 
	PDFLibHandle lib          - [in] Current lib handle.
	pdfTTFSection NameSection - [in] Type of name section.
	PPDFFontTables Font       - [in] Temporary font tables.
	ppUns16 *SLength          - [out] Font name table length.
	Returns:
	Font name table.
	*********************************************************************/
	char *PDFTTFGetFontNameFromTables(PDFLibHandle Lib, pdfTTFSection NameSection,PPDFFontTables Font, ppUns16 *SLength);

	/********************************************************************
	Description: 
	Returns name table.
	Parameters: 
	PDFLibHandle lib     - [in] Current lib handle.
	PDFStreamHandle Strm - [in] Font stream.
	ppUns16 *Llen        - [out] Font name table length.
	Returns:
	Font name table.
	*********************************************************************/
	char *PDFTTFGetNameTable(PDFLibHandle Lib, PDFStreamHandle Strm, ppUns16 *Llen);

	/********************************************************************
	Description: 
	Returns stream with ANSI embedded font.
	Parameters: 
	PDFLibHandle lib     - [in] Current lib handle.
	PPDFFontTables Font  - [in] Temporary font tables.
	ppUns16 *Unicodes    - [in] Array with char codes.
	ppUns32 CodeLength   - [in] Length of array with char codes.
	Returns:
	Stream with embedded font.
	*********************************************************************/
	PDFStreamHandle PDFTTFGetANSIEmbeddedFont(PDFLibHandle Lib, PPDFFontTables Font, ppUns16 **Unicodes, ppUns32 CodeLength);

	/********************************************************************
	Description: 
	Returns stream with unicode embedded font.
	Parameters: 
	PDFLibHandle lib     - [in] Current lib handle.
	PPDFFontTables Font  - [in] Temporary font tables.
	Returns:
	Stream with embedded font.
	*********************************************************************/
	PDFStreamHandle PDFTTFGetUnicodeEmbeddedFont(PDFLibHandle Lib, PPDFFontTables Font);

	/********************************************************************
	Description: 
	Returns required metrics for embedded font.
	Parameters: 
	PFontMetrics req_tables - [out] Required metrics for embedded font.
	PPDFFontTables Font     - [in] Temporary font tables.
	Returns:
	None.
	*********************************************************************/
	void PDFTTFGetRequiredMetrics( PFontMetrics req_tables, PPDFFontTables Font);

	/********************************************************************
	Description: 
	Returns ID of current character.
	Parameters: 
	PPDFFontTables Tables - [in] Temporary font tables.
	ppUns16 Character     - [in] Character code.
	ppBool * Modified     - [in] Refer to new characters.
	Returns: 
	Character ID.
	*********************************************************************/
	ppInt32 PDFTTFGetCharID(PPDFFontTables Tables, ppUns16 Character, ppBool *Modified);

	/********************************************************************
	Description: 
	Returns width of current character.
	Parameters: 
	PPDFFontTables Tables - [in] Temporary font tables.
	ppUns16 Character     - [in] Character code.
	Returns: 
	Character width.
	*********************************************************************/
	ppUns16 PDFTTFGetCharWidth(PPDFFontTables Tables, ppUns16 Character);

	/********************************************************************
	Description: 
	Returns pointer to character table by unicode.
	Parameters: 
	PPDFFontTables Font - [in] Temporary font tables.
	ppUns16 Unicode     - [in] Character unicode.
	Returns: 
	Character width.
	*********************************************************************/
	PCodeTableNode PDFTTFGetCharCodeTable(PPDFFontTables Font, ppUns16 Unicode);

	/********************************************************************
	Description: 
	Returns pointer to character table by glyph index.
	Parameters: 
	Font		- [in] Temporary font tables.
	GlyphIndex  - [in] Glyph index.
	Returns: 
	Character width.
	*********************************************************************/
	PCodeTableNode PDFTTFGetCharCodeTableByIndex(PPDFFontTables Font, ppUns16 GlyphIndex);

	/********************************************************************
	Description:
	Free temporary font tables.
	Parameters: 
	PPDFFontTables Font - [in] Type1 temporary font tables.
	Returns:
	None.
	*********************************************************************/
	void PDFTTFFreeFont( PDFLibHandle Lib, PPDFFontTables Font );

#ifdef WINDOWS_PLATFORM

	/********************************************************************
	Description: 
	Loads font stream and parse tables from DC.
	Parameters: 
	PDFLibHandle lib - [ in ] Current lib handle.
	HDC DC           - [ in ] Device context handle 
	Returns:
	Temporary font tables.
	*********************************************************************/
	PPDFFontTables PDFTTFLoadFontFromDC( PDFLibHandle Lib, HDC DC );

#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef VSTtf_H */



