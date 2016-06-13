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
VSFilter.c
*********************************************************************/

#include "VSPlatform.h"




#include <memory.h>
#include <ctype.h>


#include <string.h>
#include "VSFilter.h"
#include "VSMisc.h"
#include "VSDocAI.h"


PDFStreamHandle ULStrmFlateCompress(PDFStreamHandle Strm)
{
    z_stream c_stream; /* compression stream */
    void *bf, *inbf;
    ppInt32 err, sz;    
    PDFStreamHandle tmp;
    PDFLibHandle Lib= ( ( PULStrm ) Strm )->Lib;
    c_stream.zalloc = Z_NULL;
    c_stream.zfree = Z_NULL;
    c_stream.opaque = ( voidpf ) 0;
    inbf = ULStrmGetMem ( Strm );
    sz = ULStreamGetSize ( Strm );
    if ( deflateInit ( &c_stream, Z_DEFAULT_COMPRESSION ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
    PDFTRY ( Lib ){
        tmp = ULStreamMemNew ( Lib, 0 );
        PDFTRY ( Lib ){
            bf = mmalloc ( Lib, ZBUFFERSIZE );
            c_stream.next_out = ( Bytef * ) bf;
            c_stream.avail_out = ZBUFFERSIZE;
            c_stream.next_in = ( Bytef * ) inbf;
            c_stream.avail_in = sz;
            err = deflate ( &c_stream, Z_FINISH );
            if ( err < 0 ){
                mfree ( Lib, bf );
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
            }
            while ( err != Z_STREAM_END ){
                ULStreamWriteBuffer ( tmp, bf, ZBUFFERSIZE );
                c_stream.next_out = ( Bytef * ) bf;
                c_stream.avail_out = ZBUFFERSIZE;
                err = deflate ( &c_stream, Z_FINISH );
                if ( err < 0 ){
                    mfree ( Lib, bf );
                    PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
                };
            };
            err = c_stream.total_out - ULStreamGetSize ( tmp );
            if ( err )
                ULStreamWriteBuffer ( tmp, bf, err );
            deflateEnd ( &c_stream );
            mfree ( Lib, bf );
        } PDFEXCEPT ( Lib ){
            ULStreamClose ( tmp );
            PDFRERAISE ( Lib );
        }
        PDFTRYEND ( Lib );
    } PDFEXCEPT ( Lib ){
        deflateEnd ( &c_stream );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    return tmp;
}



void ULStrmFlateInit(PDFLibHandle Lib, PULFlateStrm Flate)
{
    ppUns8 *inbf;
    ppInt32 sz;
    Flate->ZLibStream.zalloc = Z_NULL;
    Flate->ZLibStream.zfree = Z_NULL;
    Flate->ZLibStream.opaque = ( voidpf ) 0;
    inbf = ( ppUns8 * ) ULStrmGetMem ( Flate->Strm );
    if ( !inbf )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, gleOtherError ) );
    sz = ULStreamGetSize ( Flate->Strm );
    inflateInit ( &( Flate->ZLibStream ) );
    Flate->OutAvail = 0;
    Flate->OutIndex = 0;
    Flate->ZLibStream.avail_in = sz;
    Flate->ZLibStream.next_in = inbf;
    Flate->EOFF = false;
    return ;
}
ppInt32 ULStrmFlateReadToBuffer(PULFlateStrm Flate)
{
    ppInt32 err;
    Flate->ZLibStream.avail_out = ZBUFFERSIZE;
    Flate->ZLibStream.next_out = Flate->OutBuffer;
    do{
        err = inflate ( &( Flate->ZLibStream ), Z_NO_FLUSH );
        if ( err < 0 )
            return err;
        if ( err == Z_STREAM_END )
            Flate->EOFF = true;
    } while ( Flate->ZLibStream.avail_out == ZBUFFERSIZE );
    Flate->OutIndex = 0;
    Flate->OutAvail = ZBUFFERSIZE - Flate->ZLibStream.avail_out;
    return 0;
}



ppInt32 ULStrmFlateGetChar(PULFlateStrm Flate)
{
    ppInt32 err;
    if ( Flate->OutIndex >= Flate->OutAvail ){
        if ( Flate->EOFF )
            return EOF;
        if ( ( err = ULStrmFlateReadToBuffer ( Flate ) ) != 0 )
            return err;
        if ( !Flate->OutAvail )
            return EOF;
    }
    return ( ppUns8 ) Flate->OutBuffer[Flate->OutIndex++];
}



ppInt32 ULStrmFlateClose(PULFlateStrm Flate)
{
    inflateEnd ( &( Flate->ZLibStream ) );
    return 0;
}


void ULStrmFlateDecompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm, ppInt32 Pred,
                           ppInt32 Collumns, ppInt32 Colors, ppInt32 Bits)
{
    z_stream c_stream; /* compression stream */
    void *bf, *inbf;
    PDFLibHandle Lib= ( ( PULStrm ) Strm )->Lib;
    ppInt32 err, sz, i, ch, nc, j, k, s;   
    ppInt32 left, up, upLeft, p, pa, pb, pc;
    ppUns32 inBuf, outBuf, bitMask;
    ppUns8 lb[4];
    ppInt32 Val, PixBytes, RowBytes, inBits, outBits;
    ppUns8 *line;
    ppBool hr;
	PDFStreamHandle WS;
	if ( Pred == 1 ){
		WS = OutStrm;
	} else {
		WS = ULStreamMemNew( Lib,0);
	}
    c_stream.zalloc = Z_NULL;
    c_stream.zfree = Z_NULL;
    c_stream.opaque = ( voidpf ) 0;
    inbf = ULStrmGetMem ( Strm );
	if ( !inbf ){
		if (Pred != 1) ULStreamClose( WS);
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, gleOtherError ) );
	}
    sz = ULStreamGetSize ( Strm );
    bf = mmalloc ( Lib, ZBUFFERSIZE );
	if ( inflateInit ( &c_stream ) ){
		if (Pred != 1) ULStreamClose( WS);
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
	}
	s = 0;
    c_stream.next_in = ( Bytef * ) inbf;
    c_stream.avail_in = sz;
    c_stream.next_out = ( Bytef * ) bf;
    c_stream.avail_out = ZBUFFERSIZE;
    err = inflate ( &c_stream, Z_NO_FLUSH );
    if ( err < 0 ){
        inflateEnd ( &c_stream );
        mfree ( Lib, bf );
		if (Pred != 1) ULStreamClose( WS);
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
    };
    while ( err != Z_STREAM_END ){
        ULStreamWriteBuffer ( WS, bf, ZBUFFERSIZE );
		s += ZBUFFERSIZE;
        c_stream.next_out = ( Bytef * ) bf;
        c_stream.avail_out = ZBUFFERSIZE;
        err = inflate ( &c_stream, Z_NO_FLUSH );
        if ( err < 0 ){
            inflateEnd ( &c_stream );
            mfree ( Lib, bf );
			if (Pred != 1) ULStreamClose( WS);
            PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleFlateError ) );
        };
    };
    err = c_stream.total_out - s;
    if ( err )
        ULStreamWriteBuffer ( WS, bf, err );
    inflateEnd ( &c_stream );
    mfree ( Lib, bf );
    if (Pred == 1)
		 return;
	ULStreamSetPosition( WS, 0);
	PDFTRY (Lib){
		Val = Collumns * Colors;
		PixBytes = ( Colors * Bits + 7 ) >> 3;
		RowBytes = ( ( Val * Bits + 7 ) >> 3 ) + PixBytes;
		line = ( ppUns8 * ) mmalloc ( Lib, RowBytes );
		PDFTRY ( Lib ){
			memset ( line, 0, RowBytes );
			hr = false;
			while ( true ){
				if ( Pred >= 10 ){
					ch = ULStreamReadChar(WS );
					if ( ch == EOF )
						break;
					ch += 10;
				} else
					ch = Pred;
				lb[0] = lb[1] = lb[2] = lb[3] = 0;
				for ( i = PixBytes; i < RowBytes; i++ ){
					lb[3] = lb[2];
					lb[2] = lb[1];
					lb[1] = lb[0];
					lb[0] = line[i];
					nc = ULStreamReadChar ( WS );
					if ( nc == EOF ){
						hr = true;
						break;
					}
					switch ( ch ){
							case 11:
								line[i] = ( ppUns8 )(line[i - PixBytes] + ( ppUns8 ) nc);
								break;
							case 12:
								line[i] = ( ppUns8 )(line[i] + ( ppUns8 ) nc);
								break;
							case 13:
								line[i] = ( ppUns8 )(( ( line[i - PixBytes] + line[i] ) >> 1 ) + ( ppUns8 ) nc);
								break;
							case 14:
								left = line[i - PixBytes];
								up = line[i];
								upLeft = lb[PixBytes];
								p = left + up - upLeft;
								if ( ( pa = p - left ) < 0 )
									pa = -pa;
								if ( ( pb = p - up ) < 0 )
									pb = -pb;
								if ( ( pc = p - upLeft ) < 0 )
									pc = -pc;
								if ( pa <= pb && pa <= pc )
									line[i] = ( ppUns8 )(( ppUns8 ) left + ( ppUns8 ) nc);
								else if ( pb <= pc )
									line[i] = ( ppUns8 )(( ppUns8 ) up + ( ppUns8 ) nc);
								else
									line[i] = ( ppUns8 )(( ppUns8 ) upLeft + ( ppUns8 ) nc);
								break;
							case 10:
							default:
								line[i] = ( ppUns8 ) nc;
								break;
					}
				}
				if ( hr )
					break;
				if ( ch == 2 ){
					if ( Bits == 1 ){
						inBuf = line[PixBytes - 1];
						for ( i = PixBytes; i < RowBytes; i += 8 ){
							inBuf = ( inBuf << 8 ) | line[i];
							line[i] ^= ( ppUns8 )(inBuf >> Colors);
						}
					} else if ( Bits == 8 )
						for ( i = PixBytes; i < RowBytes; ++i )
							line[i] = (ppUns8)(line[i] + line[i - Colors]);
					else{
						lb[0] = lb[1] = lb[2] = lb[3] = 0;
						bitMask = ( 1 << Bits ) - 1;
						inBuf = outBuf = 0;
						inBits = outBits = 0;
						j = k = PixBytes;
						for ( i = 0; i < Val; ++i ){
							if ( inBits < Bits ){
								inBuf = ( inBuf << 8 ) | ( line[j++] & 0xff );
								inBits += 8;
							}
							lb[3] = lb[2];
							lb[2] = lb[1];
							lb[1] = lb[0];
							lb[0] = ( ppUns8 )
								( ( lb[Colors] + ( inBuf >> ( inBits - Bits ) ) ) & bitMask );
							outBuf = ( outBuf << Bits ) | lb[0];
							inBits -= Bits;
							outBits += Bits;
							if ( outBits > 8 )
								line[k++] = ( ppUns8 ) ( outBuf >> ( outBits - 8 ) );
						}
						if ( outBits > 0 )
							line[k] = ( ppUns8 ) ( outBuf << ( 8 - outBits ) );
					}
				}
				ULStreamWriteBuffer ( OutStrm, &( line[PixBytes] ), RowBytes - PixBytes );
			}
		} PDFFINALLY ( Lib ){
			mfree ( Lib, line );
		}PDFFINALLYEND ( Lib );
	} PDFFINALLY( Lib) {
		ULStreamClose( WS);
	} PDFFINALLYEND (Lib);
}
void ULStrmASCIIHexDecompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm)
{
    ppInt32 pos;
    ppInt32 ch, ch1;
    ppBool ex;
    unsigned char b;
    PDFLibHandle Lib= ( ( PULStrm ) Strm )->Lib;
    ex = false;
    pos = ULStreamGetPosition ( Strm );
    ULStreamSetPosition ( Strm, 0 );
    for ( ; ; ){
        do
        ch = ULStreamReadChar ( Strm );
        while ( isspace ( ch ) );
        if ( ch == EOF || ch == ( ppInt32 ) '>' )
            break;
        do
        ch1 = ULStreamReadChar ( Strm );
        while ( isspace ( ch1 ) );
        if ( ch1 == EOF || ch1 == ( ppInt32 ) '>' ){
            ex = true;  
            ch1 = '0';
        };
        if ( ch >= ( ppInt32 ) '0' && ch <= ( ppInt32 ) '9' )
            ch = ch - '0';
        else if ( ch >= ( ppInt32 ) 'A' && ch <= ( ppInt32 ) 'F' )
            ch = ch - ( ppInt32 ) 'A' + 10;
        else if ( ch >= ( ppInt32 ) 'a' && ch <= ( ppInt32 ) 'f' )
            ch = ch - ( ppInt32 ) 'a' + 10;
        else
            PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleASCIIHexError ) );
        if ( ch1 >= ( ppInt32 ) '0' && ch1 <= ( ppInt32 ) '9' )
            ch1 = ch1 - ( ppInt32 ) '0';
        else if ( ch1 >= ( ppInt32 ) 'A' && ch1 <= ( ppInt32 ) 'F' )
            ch1 = ch1 - ( ppInt32 ) 'A' + 10;
        else if ( ch1 >= ( ppInt32 ) 'a' && ch1 <= ( ppInt32 ) 'f' )
            ch1 = ch1 - ( ppInt32 ) 'a' + 10;
        else
            PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleASCIIHexError ) );
        b = ( unsigned char ) ( ( ch << 4 ) + ch1 );
        ULStreamWriteChar ( OutStrm, b );
        if ( ex )
            break;
    };
    ULStreamSetPosition ( Strm, pos );
}

void ULStrmASCII85Decompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm)
{
    ppInt32 pos, i, j,k,t;
    ppInt32 ch;
    ppBool ex;
    unsigned char b[5];
    PDFLibHandle Lib= ( ( PULStrm ) Strm )->Lib;
    ex = false;
    pos = ULStreamGetPosition ( Strm );
    ULStreamSetPosition ( Strm, 0 );
    do {
		do{
			ch = ULStreamReadChar ( Strm );
		} while ( ch =='\r' || ch == '\n' );
        if ( ch == EOF || ch == 126 )
            break;
        if ( ch == 122 ){
            ULStreamWriteChar ( OutStrm, 0 );
            ULStreamWriteChar ( OutStrm, 0 );
            ULStreamWriteChar ( OutStrm, 0 );
            ULStreamWriteChar ( OutStrm, 0 );
            continue;
        };
        if ( ch<33 || ch>117 )
            PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleASCII85Error ) );
        b[0] = ( unsigned char ) ch;
        for ( i = 1; i < 5; i++ ){
			do { 
				ch = ULStreamReadChar ( Strm );
			} while ( ch =='\r' || ch == '\n' );
            if ( ch == EOF || ch == 126 ){
                ex = true;
                break;
            };
            if ( ch<33 || ch>117 )
                PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleASCII85Error ) );
            b[i] = ( unsigned char ) ch;
        };
		for ( j = i; j < 5; j++ ){
            b[j] = 117;
		}
        t = 0;
		for ( k = 0; k <= 4; k++ ){
            t = t * 85 + ( b[k] - 33 );
		}
           
        ULStreamWriteChar ( OutStrm, ( ppUns8 ) ( t >> 24 ) );
		ULStreamWriteChar ( OutStrm, ( ppUns8 ) ( t >> 16 ) );
		ULStreamWriteChar ( OutStrm, ( ppUns8 ) ( t >> 8 ) );
		ULStreamWriteChar ( OutStrm, ( ppUns8 ) ( t ) );
    } while (!ex);
    ULStreamSetPosition ( Strm, pos );
}


void ULStrmOneUnFilter(PDFCosHandle FilterName, PDFCosHandle DecodeParams,
                       PDFStreamHandle InStream, PDFStreamHandle OutStream)
{
    ppAtom at;
    ppInt32 pred, bits, color, columns, EarlyChange;
    PDFCosHandle obj;
    PDFLibHandle Lib= ( ( PULStrm ) InStream )->Lib;
    if ( !_IsCosName ( FilterName ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleIllegalCosObjectWasFoundError ) );
    at = _CosNameValue ( FilterName );
	if ( at == GetAtomLib ( Lib,Crypt))
		return;
    if ( at == GetAtomLib ( Lib, FlateDecode ) || at == GetAtomLib ( Lib, Fl ) ){
        pred = 1;
        columns = 1;
        bits = 8;
        color = 1;
        if ( _IsCosDict ( DecodeParams ) ){
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Predictor ) );
            if ( _IsCosInt ( obj ) )
                pred = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Colors ) );
            if ( _IsCosInt ( obj ) )
                color = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Columns ) );
            if ( _IsCosInt ( obj ) )
                columns = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, BitsPerComponent ) );
            if ( _IsCosInt ( obj ) )
                bits = _CosIntValue ( obj );
        }
        ULStrmFlateDecompress ( InStream, OutStream, pred, columns, color, bits );
        return ;
    } else if ( at == GetAtomLib ( Lib, DCTDecode ) || at == GetAtomLib ( Lib, DCT ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
    else if ( at == GetAtomLib ( Lib, LZWDecode ) || at == GetAtomLib ( Lib, LZW ) ) {
        pred = 1;
        columns = 1;
        color = 1;
        bits = 8;
        EarlyChange = 1;
        if ( _IsCosDict ( DecodeParams ) ){
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Predictor ) );
            if ( _IsCosInt ( obj ) )
                pred = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Colors ) );
            if ( _IsCosInt ( obj ) )
                color = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, Columns ) );
            if ( _IsCosInt ( obj ) )
                columns = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, BitsPerComponent ) );
            if ( _IsCosInt ( obj ) )
                bits = _CosIntValue ( obj );
            obj = CosDictValueByName ( DecodeParams, GetAtomLib ( Lib, EarlyChange ) );
            if ( _IsCosInt ( obj ) )
                EarlyChange = _CosIntValue ( obj );
        }
        _StreamPosition ( InStream ) = 0;
        _StreamPosition ( OutStream ) = 0;
        ULStrmLZWDecompress ( InStream, OutStream, pred, columns, color, bits, EarlyChange );
        _StreamPosition (  InStream ) = 0;
        _StreamPosition ( OutStream ) = 0;
        return ;
    }
    else if ( at == GetAtomLib ( Lib, ASCII85Decode ) || at == GetAtomLib ( Lib, A85 ) ){
        ULStrmASCII85Decompress ( InStream, OutStream );
        return;
    } else if ( at == GetAtomLib ( Lib, ASCIIHexDecode ) || at == GetAtomLib ( Lib, AHx ) ){
        ULStrmASCIIHexDecompress ( InStream, OutStream );
        return;
    } else if ( at == GetAtomLib ( Lib, RunLengthDecode ) || at == GetAtomLib ( Lib, RL ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
    else if ( at == GetAtomLib ( Lib, CCITTFaxDecode ) || at == GetAtomLib ( Lib, CCF ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
    else if ( at == GetAtomLib ( Lib, JBIG2Decode ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
    else if ( at == GetAtomLib ( Lib, JPGXDecode ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
    else
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnsupporterFilterDecodeError ) )
}

#ifdef _LIB
#undef _LIB
#endif
#define _LIB Lib

PDFStreamHandle ULStrmGetUnFiltered(PDFLibHandle Lib, PDFCosHandle CosObject)
{
    PDFCosHandle attr, fl, dp, cdp, cfl;
    PDFStreamHandle tmp1, tmp2;
    ppUns32 i;
    if ( !_IsCosStream ( CosObject ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleInvalidObjError ) );
    if ( !_CosStreamIsFiltered ( CosObject ) )
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleUnfilteredStreamWasFoundError ) );;
    attr = _CosStreamAttr ( CosObject );
    fl = _CosDictValueByName ( attr, Filter );
    if ( _IsCosNull ( fl ) )
        fl = _CosDictValueByName ( attr, F );
    dp = _CosDictValueByName ( attr, DecodeParms );
    if ( _IsCosNull ( dp ) )
        dp = _CosDictValueByName ( attr, DP );
    if ( _IsCosName ( fl ) ){
        if ( _IsCosArray ( dp ) )
            dp = CosArrayItem ( dp, 0 );
        tmp1 = ULStreamMemNew ( Lib, 0 );
        PDFTRY ( Lib ){
            ULStrmOneUnFilter ( fl, dp, _CosStreamValue ( CosObject ), tmp1 );
        } PDFEXCEPT ( Lib ){
            ULStreamClose ( tmp1 );
            PDFRERAISE ( Lib );
        }
        PDFTRYEND ( Lib );
        ULStreamSetPosition ( tmp1, 0 );
        return tmp1;
    } else if ( _IsCosArray ( fl ) ){
        tmp1 = _CosStreamValue ( CosObject );
        for ( i = 0; i < _CosArrayCount ( fl ); i++ ){
            cfl = _CosArrayItem ( fl, i );
            if ( _IsCosArray ( dp ) )
                cdp = CosArrayItem ( dp, i );
            else
                cdp = CosNullNew ( _CosDoc (fl) );
            tmp2 = ULStreamMemNew ( Lib, 0 );
            PDFTRY ( Lib ){
                ULStrmOneUnFilter ( cfl, cdp, tmp1, tmp2 );
            }
            PDFEXCEPT ( Lib ){
                ULStreamClose ( tmp2 );
                PDFRERAISE ( Lib );
            }
            PDFTRYEND ( Lib );
            if ( i )
                ULStreamClose ( tmp1 );
            tmp1 = tmp2;
        }
        ULStreamSetPosition ( tmp1, 0 );
        return tmp1;
    } else
        PDFRAISE ( Lib, PDFBuildErrCode ( ErrCosLevel, cleIllegalCosObjectWasFoundError ) );
    return NULL;
}

#define _DOC _CosDoc ( CosObject )

void CosStreamFlatePack(PDFLibHandle Lib, PDFCosHandle CosObject)
{
    PDFCosHandle attr, obj, add;
    PDFStreamHandle packStrm, frstrm;
    ppBool fl;
    if ( _CosGetType ( CosObject ) != CosStream )
        return ;
    attr = _CosStreamAttr ( CosObject );
    frstrm = _CosStreamValue ( CosObject );
    packStrm = ULStrmFlateCompress ( frstrm );
    PDFTRY ( Lib ){
        fl = false;
        obj = _CosDictValueByName ( attr, Filter );
        if ( _IsCosNull ( obj ) ){
            fl = true;
            obj = _CosDictValueByName ( attr, F );
        }
        if ( _IsCosArray ( obj ) )
            CosArrayAppend ( obj, _CosNameNew ( FlateDecode ) );
        else if ( _IsCosName ( obj )  ){
            add = CosArrayNew (_DOC, false, 2 );
            fl ? _CosDictAppend ( attr, F, add ) : _CosDictAppend ( attr, Filter, add );
            CosArrayAppend ( add, CosCopy ( _CosDoc ( obj ), obj ) );
            CosArrayAppend ( add, _CosNameNew ( FlateDecode ) );
            
            fl = false;
            obj = _CosDictValueByName ( attr, DecodeParms );
            if ( _IsCosNull ( obj ) ){
                fl = true;
                obj = _CosDictValueByName ( attr, DP );
            }
            if ( _IsCosName ( obj ) ){
                add = CosArrayNew ( _DOC, false, 2 );
                fl ? _CosDictAppend ( attr, DP, add ) : _CosDictAppend (attr, DecodeParms, add );
                CosArrayAppend ( add, CosCopy ( _CosDoc ( obj ), obj ) );
                CosArrayAppend ( add, CosNullNew ( _DOC ) );
            };
        } else if ( _IsCosNull ( obj ) )
            _CosDictAppend ( attr, Filter, _CosNameNew ( FlateDecode ) );
        else
            fl ? _CosDictAppend ( attr, F, _CosNameNew ( FlateDecode ) ) :
        _CosDictAppend ( attr, Filter, _CosNameNew ( FlateDecode ) );
        _CosStreamIsFiltered( CosObject ) = true;
    } PDFEXCEPT ( Lib ){
        ULStreamClose ( packStrm );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    ULStreamClose ( frstrm );
    _CosStreamValue ( CosObject ) = packStrm;
}

void ULStrmLZWDecompress( PDFStreamHandle InStream, PDFStreamHandle OutStream,  ppInt32 Predictor,  ppInt32 Columns, ppInt32 Colors, ppInt32 BitsPerComponent, ppInt32 EarlyChange )
{
    PDFLibHandle Lib = ( ( PULStrm ) InStream )->Lib;

    ppUns8  byte_from_stream;   /* Next read byte */
    ppUns32 big_number = 0;     /* 32-Bit buffer */
    ppUns16 new_code;           /* 9 - 12 bit code from LZW sequence */

    PppUns8 dictionary_bytes[4096];  /* LZW sequence dictionary */
    ppUns8  dictionary_size[4096];  /* size of word from dictionary */

    ppUns16 dictionary_items_count = 256;  /* size of dictionary */
    ppUns16 dictionary_item;               /* dictionary counter */

    ppUns8  idxBIT       = 0;   /* remain of bits from bit buffer */
    ppUns8  lzwBIT       = 9;   /* usefully bit from new_code */

    PppUns8  prev = NULL;        /* prev lexeme */
    PppUns8  curr = NULL;        /* curr lexeme */
    ppUns8   size_prev = 0;      /* size of... */
    ppUns8   size_curr = 0;
    ppInt16  add_new;            /* flag looking for addition into dictionary */
    ppUns8   i;                  

    if ( EarlyChange != 1)
        return;

    if ( Predictor == PREDICTOR_NO_PREDICT )
    {
        /* Default dictionary */
        for ( dictionary_item = 0; dictionary_item < dictionary_items_count; dictionary_item++ )
        {
            dictionary_size[ dictionary_item ]   = 1;
            dictionary_bytes[ dictionary_item ] = (PppUns8) mmalloc( Lib, dictionary_size[ dictionary_item ] );
            *dictionary_bytes[ dictionary_item ]   = (ppUns8) dictionary_item;
        }

        /* ... and 2 codes Clear-Table Marker (CTM) and End-Of-Dictionary Marker (EOD) */
        dictionary_items_count = 258;
        /* CTM_Code */
        dictionary_size[ CTM_Code ] = 0;
        dictionary_bytes[ CTM_Code ] = (PppUns8) mmalloc( Lib, 0 );
        /* EOD_Code */
        dictionary_size[ EOD_Code ] = 0;
        dictionary_bytes[ EOD_Code ] = (PppUns8) mmalloc( Lib, 0 );

        /* Reading per 1 byte */
        while ( ULStreamReadBuffer( InStream, &byte_from_stream, 1 ) != 0 )
        {
            /* .. insert to buffer */
            big_number = ( big_number << 8 ) + ( ppUns32 ) byte_from_stream;
            /* .. total idxBIT bits in buffer */
            idxBIT = idxBIT + 8;
            /* .. if we can decode */
            if ( idxBIT > lzwBIT )
            {
                /* check new code from LZW sequence */
                new_code = ( ppUns16 )( big_number >> ( idxBIT - lzwBIT ) );
                /* ... and clear it from buffer */
                big_number = big_number & ( ppUns32 ) ( ( 1 << ( idxBIT - lzwBIT ) ) - 1 );
                idxBIT = idxBIT - lzwBIT;

                /* looking for new code in dictionary */
                add_new = -1;
                for ( dictionary_item = 258; ( dictionary_item < dictionary_items_count ) && ( add_new < 0 ); dictionary_item++ )
                {
                    add_new = dictionary_item;
                    if ( ( size_prev + 1 ) == dictionary_size[dictionary_item] )
                    {
                        for ( i = 0; ( ( i < size_prev ) && ( add_new > 0 ) ); i++ )
                            if ( prev[ i ] != dictionary_bytes[ dictionary_item ][ i ] ) add_new = -1;
                        if ( curr[ 0 ] != dictionary_bytes[ dictionary_item ][ size_prev ] ) add_new = -1;
                    } else add_new = -1;
                }

                /* if we meet Clear-Table Marker => clear dictionary */
                if ( new_code == CTM_Code )
                {
                    for (dictionary_item = 258;dictionary_item < dictionary_items_count;dictionary_item++)
                        mfree( Lib, dictionary_bytes[dictionary_item] );
                    dictionary_items_count = 258;
                    lzwBIT = 9;
                }
                /* if we meet End-Of-Dictionary Marker => leftover bits in the final byte are set to 0 and stop */
                if ( new_code == EOD_Code )
                {
                    for (dictionary_item = 0;dictionary_item < dictionary_items_count;dictionary_item++)
                        mfree( Lib, dictionary_bytes[dictionary_item] );

                    mfree( Lib, prev );
                    mfree( Lib, curr );
                    return;
                }

                /* copy curr string into prev string */
                mfree( Lib, prev );
                size_prev = size_curr;
                prev = (PppUns8) mmalloc( Lib, size_prev );
                for ( i = 0; i < size_prev ; i++ )
                    prev[ i ] = curr [i];

                if ( new_code < dictionary_items_count )
                {
                    /* copy dictionary_char[new_code] string into curr string */
                    mfree( Lib, curr );
                    size_curr = dictionary_size[new_code];
                    curr = (PppUns8) mmalloc( Lib, size_curr );
                    for ( i = 0; i < size_curr ; i++ )
                        curr[ i ] = dictionary_bytes[new_code][i];

                    if ( ( size_prev > 0 ) && ( size_curr > 0 ) )
                    {
                        add_new = -1;
                        for ( dictionary_item = 258; ( dictionary_item < dictionary_items_count ) && ( add_new < 0 ); dictionary_item++ )
                        {
                            add_new = dictionary_item;
                            if ( ( size_prev + 1 ) == dictionary_size[dictionary_item] )
                            {
                                for ( i = 0; ( ( i < size_prev ) && ( add_new > 0 ) ); i++ )
                                    if ( prev[ i ] != dictionary_bytes[dictionary_item][i] ) add_new = -1;
                                if ( curr[ 0 ] != dictionary_bytes[dictionary_item][size_prev] ) add_new = -1;
                            } else add_new = -1;
                        }

                        if ( add_new < 0 )
                        {
                            dictionary_item = dictionary_items_count;
                            dictionary_items_count++;

                            /* Dictionary overflow => !!! NO ADDITION */
                            if ( dictionary_items_count > 4095 ) /* PDF Reference ver 1.5 ( page 47 ) */
                            {
                                /* Illegal situation */
                                dictionary_items_count--;
                                dictionary_item--;
                            }
                            /* Switch in most bit codes */
                            if ( dictionary_items_count > 510  )         /* PDF Reference ver 1.5 ( page 47 ) */
                                if ( dictionary_items_count > 1022 )     /* ... as so */
                                    if ( dictionary_items_count > 2046 ) /* ... as so */
                                        lzwBIT = 12;
                                    else lzwBIT = 11;
                                else lzwBIT = 10;

                                dictionary_size[ dictionary_item ] = size_prev + 1;
                                dictionary_bytes[ dictionary_item ] = (PppUns8) mmalloc( Lib, dictionary_size[ dictionary_item ] );
                                for ( i = 0; i < size_prev ; i++ )
                                    dictionary_bytes[ dictionary_item ][ i ] = prev[ i ];
                                dictionary_bytes[ dictionary_item ][ size_prev ] = curr[ 0 ];
                        }
                    }
                }
                else
                    if ( ( size_prev > 0 ) && ( size_curr > 0 ) )
                    {
                        /* copy prev string and first character into curr string */
                        mfree( Lib, curr );
                        size_curr++;
                        curr = (PppUns8) mmalloc( Lib, size_curr );
                        for ( i = 0; i < size_prev ; i++ )
                            curr[ i ] = prev[ i ];
                        curr[ size_prev ] = prev[ 0 ];

                        /* addition new lexeme to dictionary : prev.lexxama + 1st char from curr.lexeme */
                        dictionary_item = dictionary_items_count;
                        dictionary_items_count++;
                        if ( dictionary_items_count > 4095 ) /* PDF Reference ver 1.5 ( page 47 ) */
                        {
                            /* Illegal situation */
                            dictionary_items_count--;
                            dictionary_item--;
                        }

                        /* Switch in most bit codes */
                        if ( dictionary_items_count > 510  )
                            if ( dictionary_items_count > 1022 )
                                if ( dictionary_items_count > 2046 )
                                    lzwBIT = 12;
                                else lzwBIT = 11;
                            else lzwBIT = 10;

                            dictionary_size[ dictionary_item ] = size_curr;
                            dictionary_bytes[ dictionary_item ] = (PppUns8) mmalloc( Lib, dictionary_size[ dictionary_item ] );
                            for ( i = 0; i < size_curr ; i++ )
                                dictionary_bytes[ dictionary_item ][ i ] = curr[ i ];
                    }

                    /* Write sequence of NEW_CODE in file */
                    if ( new_code < dictionary_items_count )
                        ULStreamWriteBuffer( OutStream, dictionary_bytes[ new_code ], dictionary_size[ new_code ] );
                    else
                        PDFRAISE( Lib, PDFBuildErrCode( ErrCosLevel, cleUnfilteredStreamWasFoundError ) );

            }
        }

        /* Let's normal stop LZW decoding */
        for ( dictionary_item = 0; dictionary_item < dictionary_items_count; dictionary_item++ )
            mfree( Lib, dictionary_bytes[ dictionary_item ] );

        mfree( Lib, prev );
        mfree( Lib, curr );
        return;
    } else
    {
        ppInt32  BytesPerPixel, BytesPerRow, Val;
        ppUns8  *Row;        /* line */
        ppBool   BreakFlag;  /* hr */
        ppInt32  PNG_Predictor, NextChar, ColumnCount, Offset1, Offset2; /* ch, nc, i, j, k */
        ppUns8   lb[ 4 ];   /* Leftover bytes */
        ppInt32  Left, Upper, UpperLeft; /* left, up, upLeft */
        ppInt32  Paeth, Paeth1, Paeth2, Paeth3; /* Paeth parameters p, pa, pb, pc */
        PULFlateStrm Flate = NULL;
        ppUns32 inBuf, outBuf, bitMask;
        ppInt32 inBits,outBits;

        Val = Columns * Colors;
        BytesPerPixel = ( Colors * BitsPerComponent + 7 ) >> 3;
        BytesPerRow   = ( ( Val * BitsPerComponent + 7 ) >> 3 ) + BytesPerPixel;

        Row = ( ppUns8 * ) mmalloc( Lib, BytesPerRow );
        PDFTRY( Lib )
            memset( Row, 0, BytesPerRow );

        BreakFlag = false;

        for (;;) {

            if ( Predictor >= 10 ){
                PNG_Predictor = ULStrmFlateGetChar( Flate );
                if ( PNG_Predictor == EOF ) break;
                PNG_Predictor += 10;
            } else PNG_Predictor = Predictor;

            lb[ 0 ] = lb[ 1 ] = lb[ 2 ] = lb[ 3 ] = 0;

            for ( ColumnCount = BytesPerPixel; ColumnCount < BytesPerRow; ColumnCount++ ){
                lb[ 3 ] = lb[ 2 ];
                lb[ 2 ] = lb[ 1 ];
                lb[ 1 ] = lb[ 0 ];
                lb[ 0 ] = Row[ ColumnCount ];
                NextChar = ULStrmFlateGetChar( Flate );
                if ( NextChar == EOF ) {
                    BreakFlag = true;
                    break;
                }
                switch( PNG_Predictor ) {
                        case PREDICTOR_PNG_NONE:
                        case PREDICTOR_PNG_SUB:
                            Row[ ColumnCount ] = Row[ ColumnCount - BytesPerPixel ] + (ppUns8) NextChar;
                            break;
                        case PREDICTOR_PNG_UP:
                            Row[ ColumnCount ] = Row[ ColumnCount ] + (ppUns8) NextChar;
                            break;
                        case PREDICTOR_PNG_AVERAGE:
                            Row[ ColumnCount ]=( ( Row[ ColumnCount - BytesPerPixel ] + Row[ ColumnCount ] ) >> 1 ) + (ppUns8) NextChar;
                            break;
                        case PREDICTOR_PNG_PAETH:
                            Left      = Row[ ColumnCount - BytesPerPixel ];
                            Upper     = Row[ ColumnCount ];
                            UpperLeft = lb[ BytesPerPixel ];

                            Paeth = Left + Upper - UpperLeft;
                            if ( ( Paeth1 = Paeth - Left  ) < 0 )     Paeth1 = -Paeth1;
                            if ( ( Paeth2 = Paeth - Upper ) < 0 )     Paeth2 = -Paeth2;
                            if ( ( Paeth3 = Paeth - UpperLeft ) < 0 ) Paeth3 = -Paeth3;

                            if ( Paeth1 <= Paeth2 && Paeth1 <= Paeth3 )
                                Row[ ColumnCount ] = (ppUns8) Left + (ppUns8) NextChar;
                            else
                                if ( Paeth2 <= Paeth3 )
                                    Row[ ColumnCount ] = (ppUns8) Upper + (ppUns8) NextChar;
                                else
                                    Row[ ColumnCount ] = (ppUns8) UpperLeft + (ppUns8) NextChar;

                            break;

                        default:
                            Row[ ColumnCount ] = (ppUns8) NextChar;
                            break;
                }
            }

            if ( BreakFlag ) break;

            if ( PNG_Predictor == PREDICTOR_TIFF_PREDICT ){
                if ( BitsPerComponent == 1 ) {
                    inBuf = Row[ BytesPerPixel - 1 ];
                    for ( ColumnCount = BytesPerPixel; ColumnCount < BytesPerRow; ColumnCount += 8 )
                    {
                        inBuf = ( inBuf << 8 ) | Row[ ColumnCount ];
                        Row[ ColumnCount ] ^= inBuf >> Colors;
                    }
                } else if ( BitsPerComponent == 8 ) {
                    for ( ColumnCount = BytesPerPixel; ColumnCount < BytesPerRow; ++ColumnCount ) {
                        Row[ ColumnCount ] = Row[ ColumnCount ] + Row[ ColumnCount - Colors ];
                    }
                } else
                {
                    lb[ 0 ] = lb[ 1 ] = lb[ 2 ] = lb[ 3 ] = 0;
                    bitMask = ( 1 << BitsPerComponent ) - 1;
                    inBuf = outBuf = 0;
                    inBits = outBits = 0;
                    Offset1 = Offset2 = BytesPerPixel;
                    for ( ColumnCount = 0; ColumnCount < Val; ++ColumnCount ) {
                        if ( inBits < BitsPerComponent ) {
                            inBuf = ( inBuf << 8 ) | ( Row[ Offset1++ ] & 0xff );
                            inBits += 8;
                        }
                        lb[ 3 ] = lb[ 2 ];
                        lb[ 2 ] = lb[ 1 ];
                        lb[ 1 ] = lb[ 0 ];
                        lb[ 0 ] = (ppUns8)( ( lb[ Colors ] + ( inBuf >> ( inBits - BitsPerComponent ) ) ) & bitMask );
                        outBuf  = ( outBuf << BitsPerComponent ) | lb[ 0 ];
                        inBits  -= BitsPerComponent;
                        outBits += BitsPerComponent;
                        if ( outBits > 8 )
                            Row[ Offset2++ ] = (ppUns8)( outBuf >> ( outBits - 8 ) );
                    }
                    if ( outBits > 0 )
                        Row[ Offset2 ] = (ppUns8)( outBuf << ( 8 - outBits ) );
                }
            }
            ULStreamWriteBuffer( OutStream, &Row[ BytesPerPixel ], BytesPerRow - BytesPerPixel );	
        }
        ULStrmFlateClose( Flate );

        PDFEXCEPT( Lib )
            mfree( Lib, Row );
        PDFRERAISE( Lib );
        PDFTRYEND( Lib )
            mfree( Lib, Row );
    }

}
