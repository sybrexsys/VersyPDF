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
VSFilter.h
*********************************************************************/

#ifndef VSFilter_H
#define VSFilter_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */  

#include "../VSTypes.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C"{
#endif
    PDFStreamHandle ULStrmGetUnFiltered(PDFLibHandle Lib, PDFCosHandle co);
    void CosStreamFlatePack(PDFLibHandle Lib, PDFCosHandle co);


#define  ZBUFFERSIZE 0x4000

#define  CTM_Code    0x0100
#define  EOD_Code    0x0101

#define  PREDICTOR_NO_PREDICT   0x01
#define  PREDICTOR_TIFF_PREDICT 0x02
#define  PREDICTOR_PNG_NONE     0x0A
#define  PREDICTOR_PNG_SUB      0x0B
#define  PREDICTOR_PNG_UP       0x0C
#define  PREDICTOR_PNG_AVERAGE  0x0D
#define  PREDICTOR_PNG_PAETH    0x0E
#define  PREDICTOR_PNG_OPTIMAL  0x0F

    typedef struct ULFlateStrm {
        z_stream        ZLibStream;
        PDFStreamHandle Strm;
        ppUns8          OutBuffer[ZBUFFERSIZE];
        ppInt32         OutIndex;
        ppInt32         OutAvail;
        ppBool          EOFF;
    } ULFlateStrm, *PULFlateStrm;

    PDFStreamHandle ULStrmFlateCompress(PDFStreamHandle Strm);
    void ULStrmFlateInit(PDFLibHandle Lib, PULFlateStrm Flate);
    ppInt32 ULStrmFlateReadToBuffer(PULFlateStrm Flate);
    ppInt32 ULStrmFlateGetChar(PULFlateStrm Flate);
    ppInt32 ULStrmFlateClose(PULFlateStrm Flate);
    void ULStrmFlateDecompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm, ppInt32 Pred,
                               ppInt32 Collumns, ppInt32 Colors, ppInt32 Bits);
    void ULStrmASCIIHexDecompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm);
    void ULStrmASCII85Decompress(PDFStreamHandle Strm, PDFStreamHandle OutStrm);
    void ULStrmLZWDecompress( PDFStreamHandle InStream, PDFStreamHandle OutStream,
        ppInt32 Predictor,  ppInt32 Columns, ppInt32 Colors, ppInt32 BitsPerComponent,
        ppInt32 EarlyChange );
    void ULStrmOneUnFilter(PDFCosHandle FilterName, PDFCosHandle DecodeParams,
                           PDFStreamHandle InStream, PDFStreamHandle OutStream);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSFilter_H */
