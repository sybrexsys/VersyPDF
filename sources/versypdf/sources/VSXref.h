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
VSXref.h
*********************************************************************/

#ifndef VSXref_H
#define VSXref_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSCosAI.h"

#ifdef __cplusplus
extern "C"{
#endif

    /********************************************************************
    Description: 
        Receive position of the last "xref" section in the loading PDF Document.
    Parameters: 
        Doc    - Handle to document where need find "xref" section.
    Returns:
        Position of the last "xref" section in stream where stored loading document.
    *********************************************************************/
    ppInt32 ULGetXRefPosition(PDFDocHandle Doc);


    /********************************************************************
    Description: 
        Loading of the one "xref" section to memory. Here we check it's xref stream or 
    xref table.
    Parameters: 
        Doc     - (in)  Handle to document where need load "xref" section.
        Position- (out) Position of the next stream section.
        trl     - (out) If input value is NULL return trailer section of the PDF document.
        ID      - (out) If this xref section is xref stream here returned ID of this stream.
    Returns:
        None.
    *********************************************************************/
    void ULReadOneXRef(PDFDocHandle Doc, ppInt32 *Position, PDFCosHandle *trl, PDFID *ID);


    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULReadXRefTable(PDFDocHandle Doc, ppInt32 *Position, PDFCosHandle *trl);



    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULReadXRefStream(PDFDocHandle Doc, PDFCosHandle strobj, ppInt32 *Position, PDFCosHandle *trl);



    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULCheckDocumentHeader(PDFDocHandle Doc);



    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULCheckLinearization(PDFDocHandle Doc, PDFID *ID);



    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULSetEncryptOptions(PDFDocHandle Doc, PDFID EncrID, PDFCosHandle trl);



    /********************************************************************
    Description: 
    Creates a new cos object from current position of input stream.
    Position in stream increment on size of the received cos object.
    Parameters: 
    Pars    - PDF parser which set to stream where need receive cos object.
    Returns:
    The newly-created Cos object.
    *********************************************************************/
    void ULReadOneXRefStreamSection(PDFDocHandle Doc, PDFStreamHandle strm, ppInt32 *W, ppUns32 First,
                                    ppUns32 Count);

   

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSXref_H */
