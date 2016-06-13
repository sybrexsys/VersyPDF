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
VSParse.c
*********************************************************************/



#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

#include "VSParse.h"
#include "VSAssert.h"
#include "VSLibI.h"
#include "VSMisc.h"

static char specialChars[256]   ={1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 2,
                                  0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0};

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Parser->Doc)
#define _LIB _DOC->Lib
#define  RChar ULStreamReadChar(Parser->Stream)
#define  LChar ULStreamLookChar(Parser->Stream)


void ULGetLexeme(PPDFParser Parser, PLexeme Lexeme)
{
    char *p;
    ppInt32 c, c2;
    ppBool comment, neg, done;
    ppInt32 numParen;
    ppInt32 xi;
    ppReal xf, scale;
    ppInt32 n, m;
    PDFStreamHandle ms;
    char tb[NAMEMAXSIZE];

    /* skip whitespace and comments*/
    comment = false;
    for ( ; ; ){
        if ( ( c = RChar ) == EOF ){
            Lexeme->Type = ltEOF;
            return;
        }
        if ( comment ){
            if ( c == ( ppInt32 ) '\r' || c == ( ppInt32 ) '\n' )
                comment = false;
        } else if ( c == ( ppInt32 ) '%' ){
            comment = true;
        } else if ( specialChars[c] != 1 ){
            break;
        }
    }

    /* start reading token*/
    switch ( c ){
            /* number */
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
        case '.':
            neg = false;
            xi = 0;
            if ( c == ( ppInt32 ) '-' )
                neg = true;
            else if ( c == ( ppInt32 ) '.' )
                goto doReal;
            else
                xi = c - ( ppInt32 ) '0';
            for ( ; ; ){
                c = LChar;
                if ( isdigit ( c ) ){
                    RChar;
                    xi = xi * 10 + ( c - ( ppInt32 ) '0' );
                } else if ( c == ( ppInt32 ) '.' ){
                    RChar;
                    goto doReal;
                } else
                    break;
            };
            if ( neg )
                xi = -xi;
            Lexeme->Type = ltInteger;
            Lexeme->IntValue = xi;
            return;

            doReal : xf = ( ppReal ) xi;
            scale = ( ppReal ) 0.1;
            for ( ; ; ){
                c = LChar;
                if ( !isdigit ( c ) )
                    break;
                RChar;
                xf = xf + scale * ( c - ( ppInt32 ) '0' );
                scale *= ( ppReal ) 0.1;
            };
            if ( neg )
                xf = -xf;
            Lexeme->Type = ltReal;
            Lexeme->RealValue = xf;
            return;
            /* string */
        case '(':
            p = tb;
            ms = NULL;
            n = 0;
            numParen = 1;
            done = false;
            do{
                c2 = EOF;
                switch ( c = RChar ){
                    case EOF:
                        if ( ms )
                            ULStreamClose ( ms );
                        _RAISE ( ErrCosLevel, cleUnterminatedStringError );
                    case '(':
                        ++numParen;
                        c2 = c;
                        break;
                    case ')':
                        if ( --numParen == 0 )
                            done = true;
                        else
                            c2 = c;
                        break;
                    case '\\':
                        switch ( c = RChar ){
                            case 'n':
                                c2 = ( ppInt32 ) '\n';
                                break;
                            case 'r':
                                c2 = ( ppInt32 ) '\r';
                                break;
                            case 't':
                                c2 = ( ppInt32 ) '\t';
                                break;
                            case 'b':
                                c2 = ( ppInt32 ) '\b';
                                break;
                            case 'f':
                                c2 = ( ppInt32 ) '\f';
                                break;
                            case '\\':
                            case '(':
                            case ')':
                                c2 = c;
                                break;
                            case '0':
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                                c2 = c - ( ppInt32 ) '0';
                                c = LChar;
                                if ( c >= ( ppInt32 ) '0' && c <= ( ppInt32 ) '7' ){
                                    RChar;
                                    c2 = ( c2 << 3 ) + ( c - ( ppInt32 ) '0' );
                                    c = LChar;
                                    if ( c >= ( ppInt32 ) '0' && c <= ( ppInt32 ) '7' ){
                                        RChar;
                                        c2 = ( c2 << 3 ) + ( c - ( ppInt32 ) '0' );
                                    }
                                }
                                break;
                            case '\r':
                                c = LChar;
                                if ( c == ( ppInt32 ) '\n' )
                                    RChar;
                                break;
                            case '\n':
                                break;
                            case EOF:
                                if ( ms )
                                    ULStreamClose ( ms );
                                _RAISE ( ErrCosLevel, cleUnterminatedStringError );
                            default:
                                c2 = c;
                                break;
                        };
                        break;
                    default:
                        c2 = c;
                        break;
                };
                if ( c2 != EOF ){
                    if ( n == NAMEMAXSIZE ){
                        if ( !ms )
                            ms = ULStreamMemNew ( _LIB, 0 );
                        ULStreamWriteBuffer ( ms, tb, NAMEMAXSIZE );
                        p = tb;
                        n = 0;
                    };
                    *p++ = ( char ) c2;
                    ++n;
                };
            } while ( !done );
            Lexeme->Type = ltString;
            if ( !ms ){
                Lexeme->IntValue = n;
                if ( n ){
                    Lexeme->Data = (char *) mmalloc ( _LIB, n );
                    memcpy ( Lexeme->Data, tb, n );
                } else
                    Lexeme->Data = NULL;
            } else{
                if ( n )
                    ULStreamWriteBuffer ( ms, tb, n );
                Lexeme->IntValue = ULStreamGetSize ( ms );
                Lexeme->Data = (char *) mmalloc ( _LIB, Lexeme->IntValue );
                ULStreamSetPosition ( ms, 0 );
                ULStreamReadBuffer ( ms, Lexeme->Data, Lexeme->IntValue );
                ULStreamClose ( ms );
            };
            return;
            /* name */
        case '/':
            p = tb;
            n = 0;
            while ( ( c = LChar ) != EOF && !specialChars[c] ){
                RChar;
                if ( c == ( ppInt32 ) '#' ){
                    c2 = LChar;
                    if ( c2 >= ( ppInt32 ) '0' && c2 <= ( ppInt32 ) '9' ){
                        c = c2 - ( ppInt32 ) '0';
                    } else if ( c2 >= ( ppInt32 ) 'A' && c2 <= ( ppInt32 ) 'F' ){
                        c = c2 - ( ppInt32 ) 'A' + 10;
                    } else if ( c2 >= ( ppInt32 ) 'a' && c2 <= ( ppInt32 ) 'f' ){
                        c = c2 - ( ppInt32 ) 'a' + 10;
                    } else{
                        goto notEscChar;
                    };
                    RChar;
                    c <<= 4;
                    c2 = RChar;
                    if ( c2 >= ( ppInt32 ) '0' && c2 <= ( ppInt32 ) '9' ){
                        c += c2 - ( ppInt32 ) '0';
                    } else if ( c2 >= ( ppInt32 ) 'A' && c2 <= ( ppInt32 ) 'F' ){
                        c += c2 - ( ppInt32 ) 'A' + 10;
                    } else if ( c2 >= ( ppInt32 ) 'a' && c2 <= ( ppInt32 ) 'f' ){
                        c += c2 - ( ppInt32 ) 'a' + 10;
                    } else
                        _RAISE ( ErrCosLevel, cleIllegalCharacterInHexCharInNameError );
                };
                notEscChar:
                if ( ++n == NAMEMAXSIZE )
                    _RAISE ( ErrCosLevel, cleNameTokenTooLongError );
                *p++ = ( char ) c;
            };
            *p = ( ppInt32 ) '\0';
            Lexeme->Type = ltName;
            n = ( ppInt32 ) strlen ( tb );
            n++;
            Lexeme->Data = (char *) mmalloc ( _LIB, n );
            memcpy ( Lexeme->Data, tb, n );
            return;
            /* array punctuation */
        case '[':
            Lexeme->Type = ltSmCommand;
            Lexeme->IntValue = smOpenArray;
            return;
        case ']':
            Lexeme->Type = ltSmCommand;
            Lexeme->IntValue = smCloseArray;
            return;
            /* hex string or dict punctuation */
        case '<':
            c = LChar;
            /* dict punctuation */
            if ( c == ( ppInt32 ) '<' ){
                RChar;
                Lexeme->Type = ltSmCommand;
                Lexeme->IntValue = smOpenDictionary;
                return;
                /* hex string */
            } else{
                p = tb;
                m = n = 0;
                c2 = 0;
                ms = NULL;
                for ( ; ; ){
                    c = RChar;
                    if ( c == ( ppInt32 ) '>' )
                        break;
                    else if ( c == EOF )
                        _RAISE ( ErrCosLevel, cleUnterminatedStringError )
                    else if ( specialChars[c] != 1 ){
                        c2 = c2 << 4;
                        if ( c >= ( ppInt32 ) '0' && c <= ( ppInt32 ) '9' )
                            c2 += c - ( ppInt32 ) '0';
                        else if ( c >= ( ppInt32 ) 'A' && c <= ( ppInt32 ) 'F' )
                            c2 += c - ( ppInt32 ) 'A' + 10;
                        else if ( c >= ( ppInt32 ) 'a' && c <= ( ppInt32 ) 'f' )
                            c2 += c - ( ppInt32 ) 'a' + 10;
                        else{
                            if ( !ms )
                                ULStreamClose ( ms );
                            _RAISE ( ErrCosLevel, cleIllegalCharacterInHexStringError );
                        };
                        if ( ++m == 2 ){
                            if ( n == NAMEMAXSIZE ){
                                if ( !ms )
                                    ms = ULStreamMemNew ( _LIB, 0 );
                                ULStreamWriteBuffer ( ms, tb, NAMEMAXSIZE );
                                p = tb;
                                n = 0;
                            };
                            *p++ = ( char ) c2;
                            ++n;
                            c2 = m = 0;
                        };
                    };
                };
                if ( m == 1 ){
                    c2 = c2 << 4;
                    if ( n == NAMEMAXSIZE ){
                        if ( !ms )
                            ms = ULStreamMemNew ( _LIB, 0 );
                        ULStreamWriteBuffer ( ms, tb, NAMEMAXSIZE );
                        p = tb;
                        n = 0;
                    };
                    *p = ( char ) c2;
                    ++n;
                };
                Lexeme->Type = ltStringHex;
                if ( !ms ){
                    Lexeme->IntValue = n;
                    if ( n ){
                        Lexeme->Data = ( char * ) mmalloc ( _LIB, n );
                        memcpy ( Lexeme->Data, tb, n );
                    } else
                        Lexeme->Data = NULL;
                } else{
                    if ( n )
                        ULStreamWriteBuffer ( ms, tb, n );
                    Lexeme->IntValue = ULStreamGetSize ( ms );
                    Lexeme->Data = ( char * ) mmalloc ( _LIB, Lexeme->IntValue );
                    ULStreamSetPosition ( ms, 0 );
                    ULStreamReadBuffer ( ms, Lexeme->Data, Lexeme->IntValue );
                    ULStreamClose ( ms );
                };
                return;
            };
            /* dict punctuation */
        case '>':
            c = LChar;
            if ( c == ( ppInt32 ) '>' ){
                RChar;
                Lexeme->Type = ltSmCommand;
                Lexeme->IntValue = smCloseDictionary;
                return;
            } else
                _RAISE ( ErrCosLevel, cleIllegalCharacterError );
            /* error */
        case ')':
        case '{':
        case '}':
            _RAISE ( ErrCosLevel, cleIllegalCharacterError );
            /* command */
        default:
            p = tb;
            *p++ = ( char ) c;
            n = 1;
            while ( ( c = LChar ) != EOF && !specialChars[c] ){
                RChar;
                if ( ++n == NAMEMAXSIZE )
                    _RAISE ( ErrCosLevel, cleCommandTokenTooLongError );
                *p++ = ( char ) c;
            };
            *p = '\0';
            if ( tb[0] == 't' && !strcmp ( tb, "true" ) ){
                Lexeme->Type = ltBoolean;
                Lexeme->IntValue = true;
                return;
            } else if ( tb[0] == 'f' && !strcmp ( tb, "false" ) ){
                Lexeme->Type = ltBoolean;
                Lexeme->IntValue = false;
                return;
            } else if ( tb[0] == 'n' && !strcmp ( tb, "null" ) ){
                Lexeme->Type = ltNull;
                Lexeme->IntValue = true;
                return;
            } else{
                Lexeme->Type = ltCommand;
                n = ( ppInt32 ) strlen ( tb );
                n++;
                Lexeme->Data = ( char * ) mmalloc ( _LIB, n );
                memcpy ( Lexeme->Data, tb, n );
                return;
            };
    }
}

void ULClearLexeme(PDFLibHandle Lib, PLexeme Lexeme)
{
    if ( Lexeme->Type >= ltName ){
        mfree ( Lib, Lexeme->Data );
        Lexeme->Data = NULL;
    };
}


void ULParseToNextLine(PPDFParser Parser)
{
    ppInt32 c;
    for ( ; ; ){
        c = RChar;
        if ( c == EOF || c == ( ppInt32 ) '\n' )
            return;
        if ( c == ( ppInt32 ) '\r' ){
            if ( ( c = LChar ) == ( ppInt32 ) '\n' )
                RChar;
            return;
        };
    };
}


PDFCosHandle ULParseCosObject(PPDFParser Parser)
{
    TLexeme Lexeme;
    ppUns32 p2;
    ppUns32 pos;
    ppInt32 p1;
    PDFStreamHandle ms;
    PDFCosHandle co;

    void *mm;
    char *end   = "endstream";     
    PDFCosHandle tmp, wrk, wrk1;
    ULGetLexeme ( Parser, &Lexeme );
    switch ( Lexeme.Type ){
        case ltBoolean:
            return CosBoolNew ( Parser->Doc, false, Lexeme.IntValue ? true: false );
        case ltReal:
            return CosRealNew ( Parser->Doc, false, Lexeme.RealValue );
        case ltNull:
            return CosNullNew ( Parser->Doc );
        case ltEOF:
			if (Parser->GenerateExceptionOnEOF){
				_RAISE ( ErrCosLevel, cleEOFWasFoundError );
			} else {
				return CosEOFNew ( Parser->Doc );				
			}
        case ltString:
            co = CosStringNew ( Parser->Doc, false, ( char * ) Lexeme.Data, Lexeme.IntValue);
            _CosStringHex ( co ) = false;
            ULClearLexeme ( _LIB, &Lexeme );
            return co;
        case ltStringHex:
            co = CosStringNew ( Parser->Doc, false, ( char * ) Lexeme.Data, Lexeme.IntValue);
            _CosStringHex ( co ) = true;
            ULClearLexeme ( _LIB, &Lexeme );
            return co;
        case ltName:
            co = CosNameNew (Parser->Doc, false, ULStringToAtom ( _LIB, ( char * ) Lexeme.Data ) );
            ULClearLexeme ( _LIB, &Lexeme );
            return co;
        case ltInteger:
            p1 = Lexeme.IntValue;
            pos = ULStreamGetPosition ( Parser->Stream );
            ULGetLexeme ( Parser, &Lexeme );
            if ( Lexeme.Type == ltInteger ){
                p2 = Lexeme.IntValue;
                ULGetLexeme ( Parser, &Lexeme );
                if ( Lexeme.Type == ltCommand &&
                     ( ( char * ) Lexeme.Data )[0] == 'R' &&
                     ( ( char * ) Lexeme.Data )[1] == '\0' ){
                    ULClearLexeme ( _LIB, &Lexeme );
                    return CosNewRef ( Parser->Doc, p1, ( ppUns16 ) p2 );
                } else{
                    ULClearLexeme ( _LIB, &Lexeme );
                    ULStreamSetPosition ( Parser->Stream, pos );
                    return CosIntNew ( Parser->Doc, false, p1 );
                };
            } else{
                ULClearLexeme ( _LIB, &Lexeme );
                ULStreamSetPosition ( Parser->Stream, pos );
                return CosIntNew ( Parser->Doc, false, p1 );
            };
        case ltSmCommand:
            if ( Lexeme.IntValue == smOpenArray ){
                tmp = CosArrayNew ( Parser->Doc, false, 0 );
                PDFTRY ( _LIB ){
                    wrk = ULParseCosObject ( Parser );
                    while ( !_IsCosIntWork ( wrk ) ){
                        CosArrayAppend ( tmp, wrk );
                        wrk = ULParseCosObject ( Parser );
                    };
                } PDFEXCEPT ( _LIB ){
                    CosFree ( tmp );
                    PDFRERAISE ( _LIB );
                } PDFTRYEND ( _LIB );
                if ( _CosIntWorkValue ( wrk ) != smCloseArray ){
                    CosFree ( wrk );
                    CosFree ( tmp );
                    _RAISE ( ErrCosLevel, cleUnknowTokenWasFoundError );
                };
                CosFree ( wrk );
                return tmp;
            };
            if ( Lexeme.IntValue == smOpenDictionary ){
                tmp = CosDictNew ( Parser->Doc, false, 0 );
                PDFTRY ( _LIB ){
                    wrk = ULParseCosObject ( Parser );
                    while ( !_IsCosIntWork ( wrk ) ){
                        if ( !_IsCosName ( wrk ) ){
                            CosFree ( wrk );
                            _RAISE ( ErrCosLevel, cleDictKeyNotNameError );
                        }
                        PDFTRY ( _LIB ){
                            wrk1 = ULParseCosObject ( Parser );
                        } PDFEXCEPT ( _LIB ){
                            CosFree (  wrk );
                            PDFRERAISE ( _LIB );
                        } PDFTRYEND ( _LIB );
                        CosDictAppend ( tmp, _CosNameValue ( wrk ), wrk1 );
                        CosFree ( wrk );
                        wrk = ULParseCosObject ( Parser );
                    }
                } PDFEXCEPT ( _LIB ){
                    CosFree ( tmp );
                    PDFRERAISE ( _LIB );
                } PDFTRYEND ( _LIB );
                if ( _CosIntWorkValue ( wrk ) != smCloseDictionary ){
                    CosFree ( wrk );
                    CosFree ( tmp );
                    _RAISE ( ErrCosLevel, cleUnknowTokenWasFoundError );
                }
                CosFree ( wrk );
                pos = ULStreamGetPosition ( Parser->Stream );
                ULGetLexeme ( Parser, &Lexeme );
                if ( Lexeme.Type != ltCommand || strcmp ( "stream", ( char * ) Lexeme.Data ) ){
                    ULClearLexeme ( _LIB, &Lexeme );
                    ULStreamSetPosition ( Parser->Stream, pos );
                    return tmp;
                }
                ULClearLexeme ( _LIB, &Lexeme );
                PDFTRY ( _LIB ){
                    tmp = CosDictToStream ( tmp );
                    ULParseToNextLine ( Parser );
                    p2 = ULStreamGetPosition ( Parser->Stream );
                    wrk1 = _CosStreamAttr ( tmp );
                    wrk = _CosDictValueByNameNRF ( wrk1, Length );
					if ( _DOC->Size == 1 && _IsCosRef( wrk ) )
						PDFRETURN ( _LIB, tmp );
					if ( _IsCosRef ( wrk ))
						wrk = _CosDictValueByName ( wrk1, Length );
                    if ( !_IsCosInt ( wrk ) )
                        _RAISE ( ErrCosLevel, cleUnknowStreamLength );
                    p1 = _CosIntValue ( wrk );
                    if ( p1 < 0 )
                        PDFRETURN ( _LIB, tmp );
                    ULStreamSetPosition ( Parser->Stream, p2 );
                    if ( p1 + p2 < ULStreamGetSize ( Parser->Stream ) ){
                        mm = mmalloc ( _LIB, p1 );
                        if ( ULStreamReadBuffer ( Parser->Stream, mm, p1 ) != p1 ){
                            mfree ( _LIB, mm );
                            _RAISE ( ErrCosLevel, cleDataReadError );
                        };
                        ms = _CosStreamValue ( tmp );
                        ULStreamWriteBuffer ( ms, mm, p1 );
                        mfree ( _LIB, mm );
                        CosStreamCheckFilters ( tmp );
                    } else{
                        pos = ULStreamGetSize ( Parser->Stream ) - p2;
                        mm = mmalloc ( _LIB, pos );
                        if ( ULStreamReadBuffer ( Parser->Stream, mm, pos ) != pos ){
                            mfree ( _LIB, mm );
                            _RAISE ( ErrCosLevel, cleDataReadError );
                        };
                        p1 = ULmemfndsubset ( mm, end, pos, ( ppInt32 ) strlen ( end ) );
                        if ( p1 < 0 ){
                            mfree ( _LIB, mm );
                            _RAISE ( ErrCosLevel, cleCannotFindEndOfStreamError );
                        }
                        ms = _CosStreamValue ( tmp );
                        ULStreamWriteBuffer ( ms, mm, p1 );
                        mfree ( _LIB, mm );
                        CosStreamCheckFilters ( tmp );
                    }
                } PDFEXCEPT ( _LIB ){
                    CosFree ( tmp );
                    PDFRERAISE ( _LIB );
                }
                PDFTRYEND ( _LIB );
                return tmp;
            };
            return CosNewIntWork ( Parser->Doc, Lexeme.IntValue );
        case ltCommand:
            co = CosNewWork ( Parser->Doc, ( char * ) Lexeme.Data );
            ULClearLexeme ( _LIB, &Lexeme );
            return co;
        default:
            return CosNullNew ( Parser->Doc );
    };
}

