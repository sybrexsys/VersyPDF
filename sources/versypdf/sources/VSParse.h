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
VSParse.h
*********************************************************************/

#ifndef VSParse_H
#define VSParse_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSDocAI.h"
#ifdef __cplusplus
extern "C"{
#endif

    /* Lexeme type */
    typedef enum ppLexType {
        ltInteger               = 0,    /* Integer Lexeme       */
        ltReal                  = 1,    /* Real Lexeme          */
        ltEOF                   = 2,    /* End of File Lexeme   */
        ltBoolean               = 3,    /* Boolean Lexeme       */
        ltNull                  = 4,    /* NULL Lexeme          */
        ltSmCommand             = 5,    /* Small Command Lexeme */
        ltName                  = 6,    /* Name Lexeme          */
        ltCommand               = 7,    /* Command Lexeme       */
        ltString                = 8,    /* String Lexeme        */
        ltStringHex             = 9     /* Hex String Lexeme    */ 
    } ppLexType;

    /* Kind of small command lexeme */
    typedef enum ppsmCommandType {
        smOpenArray,                    /* Small Command Lexeme Open Array      */
        smCloseArray,                   /* Small Command Lexeme Close Array     */
        smOpenDictionary,               /* Small Command Lexeme Open Dictionary */
        smCloseDictionary               /* Small Command Lexeme CloseDictionary */
    } ppsmCommandType;


    typedef struct TLexeme {
        ppLexType   Type;               /* Lexemå type                              */
        char        *Data;              /* Pointer where stored data after parse    */
        ppInt32     IntValue;           /* Return value for Integer lexemes        */
        ppReal      RealValue;          /* Return value for real lexemes           */
    } TLexeme, *PLexeme;

    /* Maximum size of Name lexeme. See PDF Reference */
#define NAMEMAXSIZE     128


    /********************************************************************
    Description: 
        Receive lexeme from current position of the stream.
        Position in stream increment on size of the received lexeme.
    Parameters: 
        pars    - PDF parser which set to stream where need receive lexeme.
        lex     - Pointer to lexeme structure where will stored received data.
    Returns:
        None.
    *********************************************************************/
    void ULGetLexeme(PPDFParser Parser, PLexeme Lexeme);



    /********************************************************************
    Description: 
        Clear all buffers which can be used for store lexema.
    Parameters: 
        lex     - Pointer to lexema which buffers need clear.
    Returns:
        None.
    *********************************************************************/
	void ULClearLexeme(PDFLibHandle Lib, PLexeme Lexeme);
    


    /********************************************************************
    Description: 
        Skip all data until next line in input stream.
    Parameters: 
        pars    - PDF parser which set to stream where need skip data.
    Returns:
        None.
    *********************************************************************/
    void ULParseToNextLine(PPDFParser Parser);



    /********************************************************************
    Description: 
        Creates a new cos object from current position of input stream.
        Position in stream increment on size of the received cos object.
    Parameters: 
        Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
        The newly-created Cos object.
    *********************************************************************/
    PDFCosHandle ULParseCosObject(PPDFParser Parser);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSParse_H */
