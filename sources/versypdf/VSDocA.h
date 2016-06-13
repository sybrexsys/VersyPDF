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
VSDocA.h
*********************************************************************/

#ifndef VSDocA_H
#define VSDocA_H /* PDF Document API Module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"

#ifdef WINDOWS_PLATFORM
#include <Windows.h>
#include <WinGDI.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

#define PDFLastSupportedVersion 7       /* Last Supported PDF Document Version is 1.7 */

    /*  Password Type of Crypted PDF Document. Password Validity. */
    _EXP
    typedef enum  _t_TKeyValidType {
        kvtInvalid                      = 0, /* Invalid Password */
        kvtUser                         = 1, /* User Password */
        kvtOwner                        = 2  /* Owner Password */
    }  TKeyValidType;

    /*  Protection Key-Length Type of Crypted PDF Document */
    _EXP
    typedef enum _t_TPDFProtectionType {
	ptUncrypted									= 0, /*  Uncrypted file */
        pt40BitProtection                           = 1, /*  40 Bit protection key length */
        pt128BitProtection                          = 2, /* 128 Bit protection key length */
	pt128BitAESProtection						= 3, /* 128 Bit protection key Length with AES cryptography */ 
	pt256BitAESProtection			= 4  /* 256 Bit protection key Length with AES cryptography */ 
    } TPDFProtectionType;

    /*   Type of supported PDF Document Versions */
    typedef enum PDFVersion {
        pdfver10                    = 0,        /* PDF Document Version is 1.0 */
        pdfver11                    = 1,        /* PDF Document Version is 1.1 */
        pdfver12                    = 2,        /* PDF Document Version is 1.2 */
        pdfver13                    = 3,        /* PDF Document Version is 1.3 */
        pdfver14                    = 4,        /* PDF Document Version is 1.4 */
        pdfver15                    = 5,		/* PDF Document Version is 1.5 */
		pdfver16                    = 6, 		/* PDF Document Version is 1.6 */
		pdfver17                    = 7, 		/* PDF Document Version is 1.7 */
		pdfver18                    = 8, 		/* PDF Document Version is 1.8 */
		pdfver19                    = 9, 		/* PDF Document Version is 1.9 */
    } TPDFVersion;

    /*  Type of usual PDF Document's Page Sizes */
    _EXP
    typedef enum _t_TPDFPageSize {
        psLetter = 0,   /* Document's Page Size is 792 x 612 */
        psA4,           /* Document's Page Size is 842 x 595 */
        psA3,           /* Document's Page Size is 1190 x 842 */
        psLegal,        /* Document's Page Size is 1008 x 612 */
        psB5,           /* Document's Page Size is 728 x 516 */
        psC5,           /* Document's Page Size is 649 x 459 */
        ps8x11,         /* Document's Page Size is 792 x 595 */
        psB4,           /* Document's Page Size is 1031 x 728 */
        psA5,           /* Document's Page Size is 595 x 419 */
        psFolio,        /* Document's Page Size is 936 x 612 */
        psExecutive,    /* Document's Page Size is 756 x 522 */
        psEnvB4,        /* Document's Page Size is 1031 x 728 */
        psEnvB5,        /* Document's Page Size is 708 x 499 */
        psEnvC6,        /* Document's Page Size is 459 x 323 */
        psEnvDL,        /* Document's Page Size is 623 x 312 */
        psEnvMonarch,   /* Document's Page Size is 540 x 279 */
        psEnv9,         /* Document's Page Size is 639 x 279 */
        psEnv10,        /* Document's Page Size is 684 x 297 */
        psEnv11         /* Document's Page Size is 747 x 324 */
    }  TPDFPageSize;

    /*  Page Orientation Type  */
    _EXP
    typedef enum _t_TPDFPageOrientation {
        poPagePortrait   = 0, /* Orientation of Page is Portrait */
        poPageLandScape       /* Orientation of Page is Landscape */
    } TPDFPageOrientation;

    /*  Document Description's Item Type  */
    _EXP
    typedef enum _t_TPDFInformation { /* */ 
        piCreator = 0,      /* Information about creator */
        piAuthor,           /* Information about author */
        piDate,             /* Information about date */
        piProducer,         /* Information about producer*/
        piTitle,            /* Information about title */
        piSubject,          /* Information about subject of the PDF Document */
        piKeyWords,         /* Information about keywords */
        piModificationData  /* Information about modification data */
    } TPDFInformation; /* */ 

    /*   Document Restriction's Flags   */
#define prPrint                 1   /* Allowed print the document */
#define prModifyContent         2   /* Allowed modify the content of the document */
#define prCopyInformation       4   /* Allowed copy text and graphics from the document */
#define prModifyAnnotation      8   /* Allowed add or modify annotations */
#define prFillAcroForm          16  /* Allowed fill in interactive form fields */
#define prExtractTextAndImage   32  /* Allowed extract text and graphics from the document */
#define prAssembleDocument      64  /* Allowed assemble the document */
#define prPrintHiResolution     128 /* Allowed print the document with high resolution */



    /************************************************************************
        Description :
            Create New PDF Document.
        Parameters :
            Lib                 - [in] Current loaded PDF Library.
        Returns :
            PDF Document Handle.
        See also :
            PDFDocClose.
    ************************************************************************/

    LIB_API PDFDocHandle PDFDocCreate( PDFLibHandle Lib );

    /* Description
       Checking the PDF Document on Security.
       Parameters
       Doc :  [in] Checking PDF Document Handle.
       Returns
       Boolean value : true - if document is crypted, false is not
       crypted.
       See Also
       PDFDocCheckPassword, PDFDocSetSecurity.                     */
    LIB_API ppBool PDFDocIsCrypted( PDFDocHandle Doc );


	/* Description
       Return the PDF Document Security.
       Parameters
       Doc :  [in] Checking PDF Document Handle.
       Returns
       Method of cryptography 
       See Also
       PDFDocCheckPassword, PDFDocSetSecurity.                     */
    LIB_API TPDFProtectionType PDFDocGetCryptoAlgorithm( PDFDocHandle Doc );

    /************************************************************************
        Description :
            Close PDF Document and free all structures.
        Parameters :
            Doc                 - [in] PDF Document Handle.
        Returns :
            None.
        See also :
            PDFDocCreate.
    ************************************************************************/
    LIB_API void PDFDocClose( PDFDocHandle Doc );

	LIB_API TPDFVersion PDFDocGetVersion ( PDFDocHandle Doc );

	LIB_API void PDFDocSetVersion ( PDFDocHandle Doc, TPDFVersion Version );

    /************************************************************************
        Description :
            Checking the PDF Document on Validity of Password.
        Parameters :
            Doc                 - [in] PDF Document Handle.
            Password            - [in] Password text string.
        Returns :
            Password Validity Type. If kvtInvalid then Password is invalid.
        See also :
            PDFDocIsCrypted, PDFDocSetSecurity, TKeyValidType.
    ************************************************************************/
    LIB_API TKeyValidType PDFDocCheckPassword( PDFDocHandle Doc, char *Password );


    /* Description
       Set PDF Document Security.
       Parameters
       Doc :                [in] PDF Document Handle.
       Permission :         [in] Document Permission's Flags.
       KeyLength :          [in] 40\-bits or 128\-bits Security Key
                            Length.
       UserPassword :       [in] User Password text string.
       OwnerPassword :      [in] Owner Password text string.
       Returns
       None.
       See Also
       PDFDocIsCrypted, PDFDocCheckPassword, TPDFProtectionType, PDFDocRemoveSecurity.         */
    LIB_API void PDFDocSetSecurity( PDFDocHandle Doc, ppInt32 Permission,
                           TPDFProtectionType KeyLength, char *UserPassword, char *OwnerPassword );


	/* Description
       Remove PDF Document Security.
       Parameters
			Doc :                [in] PDF Document Handle.
       Returns
			None.
       See Also
			PDFDocIsCrypted, PDFDocCheckPassword, TPDFProtectionType, PDFDocSetSecurity.     */
    LIB_API void PDFDocRemoveSecurity( PDFDocHandle Doc);

	/* 
	Description
		Remove PDF Document Security.
	Parameters
		Doc :                [in] PDF Document Handle.
		CryptMetadata:		 [in] Boolean flag. If false matadata not will crypted, 
									else metadata streams will crypted as other cos streams
	Returns
		None.
	See Also
	PDFDocIsCrypted, PDFDocCheckPassword, TPDFProtectionType, PDFDocSetSecurity, PDFDocRemoveSecurity.     
	Note:
		Setting of the function will be used only for 128 bit cryptography
	*/
	LIB_API void PDFDocSetEncryptMetaData( PDFDocHandle Doc, ppBool CryptMetadata);


	LIB_API void PDFDocSetAppendRedrawAcroForm(PDFDocHandle Doc);

    /************************************************************************
        Description :
            Get Document Permission's Flags.
        Parameters :
            Doc                 - [in] Checking PDF Document Handle.
        Returns :
            Document Permission's Flags.
        See also :
            PDFDocSetSecurity.
    ************************************************************************/
    LIB_API ppInt32 PDFDocGetPermission( PDFDocHandle Doc );

    /************************************************************************
        Description :
            Add Page in PDF Document.
        Parameters :
            Doc                 - [in] PDF Document Handle.
            Width               - [in] Width of Page.
            Height              - [in] Height of Page.
        Returns :
            Index of New Page in Document.
        See also :
            PDFDocAppendPage2.
    ************************************************************************/
    LIB_API ppInt32 PDFDocAppendPage( PDFDocHandle Doc, ppReal Width, ppReal Height );


/************************************************************************
        Description :
            Add Page in PDF Document.
        Parameters :
            Doc                 - [in] PDF Document Handle.
            Size                - [in] Type of Page Size, see TPDFPageSize.
            Orientation         - [in] Orientation of Page, see TPDFPageOrientation.
        Returns :
            Index of New Page in Document.
        See also :
            PDFDocAppendPage, TPDFPageSize, TPDFPageOrientation.
    ************************************************************************/
    LIB_API ppInt32 PDFDocAppendPage2( PDFDocHandle Doc, TPDFPageSize Size, TPDFPageOrientation Orientation );


    /************************************************************************
        Description :
            Get Page Count of PDF Document.
        Parameters :
            Doc                 - [in] PDF Document Handle.
        Returns :
            Page Count of PDF Document.
    ************************************************************************/
    LIB_API ppInt32 PDFDocGetPageCount( PDFDocHandle Doc );


    /* Description
       Save PDF Document in memory stream or file stream, see PDF
       Streams.
       Parameters
       Doc :     [in] PDF Document Handle.
       Stream :  [out] PDF Stream Handle.
       Returns
       None.
       See Also
       PDFDocLoadFromStream, PDFStreamHandle, PDFDocSaveToBuffer,
       PDFDocSaveToFile.                                          */
    LIB_API void PDFDocSaveToStream( PDFDocHandle Doc, PDFStreamHandle Stream );


    /* Description
       Save PDF Document in memory.
       Parameters
       Doc :   [in] PDF Document Handle.
       Size :  [out] Size of buffer.
       Returns
       Pointer on PDF document beginning.
       See Also
       PDFDocLoadFromBuffer.              */
    LIB_API void *PDFDocSaveToBuffer( PDFDocHandle Doc, ppInt32 *Size );


    /* Description
       Save PDF Document in file.
       Parameters
       Doc :       [in] PDF Document Handle.
       FileName :  [in] Name of output file, text string.
       Returns
       None.
       See Also
       PDFDocLoadFromFile.                                */
    LIB_API void PDFDocSaveToFile( PDFDocHandle Doc, char *FileName );

    /* Description
       Get Information from Document Description.
       Parameters
       Doc :   [in] PDF Document Handle.
       Info :  [in] Type of Description Information.
       Returns
       Handle of Information Object.
       See Also
       PDFDocSetInfo, TPDFInformation.               */
    LIB_API char * PDFDocGetInfo( PDFDocHandle Doc, TPDFInformation Info, ppUns32 *StrLen );


    /* Description
       Save information in Document description.
       Parameters
       Doc :    [in] PDF Document Handle.
       Info :   [in] Type of Description Information.
       Value :  [in] Handle of Information Object.
       Returns
       None.
       See Also
       PDFDocGetInfo, TPDFInformation.                */
    LIB_API void PDFDocSetInfo( PDFDocHandle Doc, TPDFInformation Info, char * Str, ppUns32 StrLen );

    /* Description
       Load PDF Document from memory stream or file stream, see PDF
       Streams.
       Parameters
       Lib :     [in] PDF Library Handle.
       Stream :  [in] PDF Stream Handle.
       Returns
       PDF Document Handle.
       See Also
       PDFDocSaveToStream, PDFStreamHandle, PDFDocLoadFromFile,
       PDFDocLoadFromBuffer.                                        */
    LIB_API PDFDocHandle PDFDocLoadFromStream( PDFLibHandle Lib, PDFStreamHandle Stream );


    /* Description
       Load PDF Document from file.
       Parameters
       Lib :       [in] PDF Library Handle.
       FileName :  [in] Name of input file, text string.
       Returns
       PDF Document Handle.
       See Also
       PDFDocSaveToFile.                                 */
    LIB_API PDFDocHandle PDFDocLoadFromFile( PDFLibHandle Lib, char *FileName );


    /* Description
       Load PDF Document from Memory buffer.
       Parameters
       Lib :     [in] PDF Library Handle.
       Buffer :  [in] Pointer on PDF document beginning.
       Length :  [in] Length of buffer in bytes.
       Returns
       PDF Document Handle.
       See Also
       PDFDocSaveToBuffer.                               */
    LIB_API PDFDocHandle PDFDocLoadFromBuffer( PDFLibHandle Lib, void *Buffer, ppInt32 Length );


    /* Description
       Set whether PDF document is stored as packed document or not.
       Parameters
       Doc :     [in] PDF Document Handle.
       Packed :  [in] if true use packed save method. If false don't
                 use it.
       Returns
       None.                                                         */
    LIB_API void PDFDocSetPacked( PDFDocHandle Doc, ppBool Packed );


    /* Description
       Set whether PDF document is stored as linearized document or
       not.
       Parameters
       Doc :         [in] PDF Document Handle.
       Linearized :  [in] if true use linearized save method. If false
                     non linearized method.
       Returns
       None.                                                           */
    LIB_API void PDFDocSetLinearized( PDFDocHandle Doc, ppBool Linearized );


    /* Description
       Remove all unused objects from PDF Document before save it.
       Parameters
       Doc :     [in] PDF Document Handle.
       Remove :  [in] If true remove unused ( unlink ) objects. If
                 false don't remove. 
       Returns
       None.                                                       */
    LIB_API void PDFDocSetRemoveUnUsed( PDFDocHandle Doc, ppBool Remove );


    /* Description
       Set "Jpeg Images Quality" for jpeg images stored in PDF
       Document.
       Parameters
       Doc :      [in] PDF Document Handle.
       Quality :  [in] Value from 0 to 100\: 0 \- worst quality,
                  smallest size. 100 \- best quality, biggest size.
       Returns
       None.                                                        */
    LIB_API void PDFDocSetJpegImageQuality ( PDFDocHandle Doc, ppInt32 Quality );


	/* Description
       Appends embedded file to PDF document form disk.
       Parameters
		Doc :      [in] PDF Document Handle.
		FileName : [in] Filename of the file on the disk which need embed.
		EmbedFileName : [in] Filename of the file in PDF document.
		Title: [in] 
       Returns
		None.                                                        */
    LIB_API void PDFDocAppendFileFromDisk ( PDFDocHandle Doc, char * FileName, char * EmbedFileName,char * Title, ppUns32 TitleLength, ppBool PackData);

	/* Description
       Appends embedded file to PDF document form memory.
       Parameters
		Doc :      [in] PDF Document Handle.
		Buffer : [in] Pointer to buffer of the data which need embed.
		BufferSize: [in] Size of the buffer
		EmbedFileName : [in] Filename of the file in PDF document.
       Returns
		None.                                                        */
    LIB_API void PDFDocAppendFileFromBuffer ( PDFDocHandle Doc, void * Buffer, ppUns32 BufferSize, char * EmbedFileName, char * Title, ppUns32 TitleLength, ppBool PackData);

	/* Description
       Appends embedded file to PDF document form PDF stream.
       Parameters
		Doc :      [in] PDF Document Handle.
		AStream : [in] Stream where stored data which need embed.
		EmbedFileName : [in] Filename of the file in PDF document.
       Returns
		None.                                                        */
    LIB_API void PDFDocAppendFileFromStream ( PDFDocHandle Doc, PDFStreamHandle AStream, char * EmbedFileName, char * Title, ppUns32 TitleLength, ppBool PackData);

	void PDFDocAppendStartJavaScript ( PDFDocHandle Doc, char * JSName, char * JS);

	/* Description
	Gets PDF Document Root Cos handle.
	Parameters
	Doc :  [in] PDF Document Handle.		
	Returns
	PDF Document Root Cos handle.                                               */
	LIB_API PDFCosHandle PDFDocGetRoot( PDFDocHandle Doc ); 

	/* Description
	Remove hidden layers from PDF documents.
	Parameters
		Doc :  [in] PDF Document Handle.		
	Returns
		None.                                               */
	/*LIB_API void PDFDocRemoveHiddenLayers ( PDFDocHandle Doc ); */

#ifdef WINDOWS_PLATFORM


    /* Description
       Set AutoLaunch option for PDF Document, it launches after
       saving (only Windows Platform).
       Parameters
       Doc :         [in] PDF Document Handle.
       AutoLaunch :  [in] If true launch PDF document, If false don't.
       Returns
       None.                                                           */
	LIB_API void PDFDocSetAutoLaunch( PDFDocHandle Doc, ppBool AutoLaunch );


    /* Description
       Set hDC concerning which EMF images will be parsed.
       Parameters
       Doc :  [in] PDF Document Handle.
       DC :   [in] Handle of Device Context.
       Returns
       None.                                               */
	LIB_API void PDFDocSetUsedDC( PDFDocHandle Doc, HDC DC ); 





    /* Description
       Set "Emf color Images as jpeg" option for PDF Document.
       Parameters
       Doc :        [in] PDF Document Handle.
       EmfAsJpeg :  [in] If value has set to true, all color images stored in the
                    EMF file will be saved as JPEG images.<p/>If value has
                    set to false, all color images stored in the EMF file will be
                    saved with flate compression.
       Returns
       None.                                                                */
    LIB_API void PDFDocSetEMFColorImagesAsJpeg ( PDFDocHandle Doc, ppBool EmfAsJpeg );

    /* Description
        Set "Emf BW Images as JBIG2" option for PDF Document.
    Parameters
        Doc :           [in] PDF Document Handle.
        AsJBIG2 :       [in] If value has set to true, all black/white images stored in the
            EMF file will be saved with JBIG2 compression.<p/>If value has
            set to false, all images stored in the EMF file will be
            saved with CCITT compression.
    Returns
        None.                                                                */
    LIB_API void PDFDocSetEMFBWImagesAsJBIG2 ( PDFDocHandle Doc, ppBool AsJBIG2 );

    /***********************************************************************/
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSDocA_H */
