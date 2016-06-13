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
VSError.h
*********************************************************************/

#ifndef VSError_H
#define VSError_H /* All errors list module */

#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"

#define PDF_ERR_LEVEL_MASK      0x0FFF0000 /* Error level mask in full error code */
#define PDF_ERR_ERROR_MASK      0x0000FFFF /* Error type mask in full error code */
#define PDF_ERR_LEVEL_SHIFT     16         /* Shift of error level in full error code */
#define PDF_ERR_ERROR_SHIFT     0          /* Shift of error type in full error code */

#define PDFBuildErrCode(xlevel,xerror) \
    ((((ppUns32)xlevel) << PDF_ERR_LEVEL_SHIFT ) | \
    ((((ppUns32)xerror) << PDF_ERR_ERROR_SHIFT) & PDF_ERR_ERROR_MASK )) /* Create full error code */

#define PDFGetErrLevel(errcode) (((ppUns32)errcode & PDF_ERR_LEVEL_MASK) >> PDF_ERR_LEVEL_SHIFT) /* Take only error level from full error code */

#define PDFGetErrError(errcode) (((ppUns32)errcode & PDF_ERR_ERROR_MASK) >> PDF_ERR_ERROR_SHIFT) /* Take only error type from full error code */


    /************************************************************************
    Description :
        Error Level Type in fault situations.
    ************************************************************************/
    typedef enum {
        ErrGeneralLevel = 0,    /* General Level of Error in fault situations       */
        ErrCosLevel,            /* Data Object's Level of Error in fault situations */
        ErrFileLevel,           /* File System's Level of Error in fault situations */
        ErrPageLevel,           /* Page's Level of Error in fault situations        */
        ErrDocumentLevel,       /* Document's Level of Error in fault situations    */
        ErrSignLevel,           /* Signature's Level of Error in fault situations   */
    } PDFErrLevels;

    /************************************************************************
    Description :
        Error Type of General Level of fault situations. When we are working
        with any programming code.
    ************************************************************************/
    typedef enum {
        gleNoError = 0,                 /* No Error */
        gleOtherError,                  /* Other Error */
        gleNotEnoughMemoryError,        /* Not Enough Memory */
        gleCannotOpenFileError,         /* Cannot Open File */
		gleCannotCreateFileError,       /* Cannot Create File */
        gleCannotReceiveFileSizeError,  /* Cannot Receive File Size */
        gleCannotCreateMapFileError,    /* Cannot Create Map File */
        gleOutOfBoundError,             /* Out Of Bound */
        gleCannotChangeCapacityError,   /* Cannot Change Capacity */
        gleCannotReceiveFontDataError,  /* Cannot Receive Font Data */
        gleCannotSaveBufferToFileError, /* Cannot Save Buffer To File */
        gleWindowsGDIError,             /* Windows GDI */
		gleInvalidBitmapError,          /* Invalid Bitmap */
        gleInvalidJpegQualityError,     /* Invalid ipeg-image Quality */
		gleLoadPNGFileError,            /* Invalid PNG File */
		gleLoadTIFFFileError,           /* Load TIFF */
        gleLoadJPEGFileError,           /* Invalid JPEG File */
		gleJBIG2Error,		            /* Invalid JBIG2 */
        gleUnsupportedJPEGColorSpaceError,        /* Unsupported JPEG color space */
        gleUnknowImageFormatError,      /* Not supported image format */
        gleLoadBMPFileError,            /* Invalid BMP file */
        gleUnsupportedBitmapCompressionError, /* Unsupported bitmap compression */
		gleIOError,						/* Input/Output Error */		
		gleUnsupportedFeatureError		/* Unsupported feature for current license */
		


    } PDFGeneralLevelErrors;


    /************************************************************************
    Description :
        Error Type of Document Level of fault situations. When we are working
        with PDF Document.
    ************************************************************************/
    typedef enum {
        dleNoError = 0,                     /* No Error */
        dleOtherError,                      /* Other Error */
        dleCannotUseThisColorDeviceError,   /* Cannot Use This Color Device */
        dleCannotUseBookmarkRootError,      /* Cannot Use Bookmark Root */
        dleCannotFindFirstBeadError,        /* Cannot Find First Bead */
        dleCannotFindThreadForBeadError,    /* Cannot Find Thread For Bead */
        dleCannotFindCurrentBeadError,      /* Cannot Find Current Bead */
        dleCannotFindBeadRectError,         /* Cannot Find Bead Rectangle */
        dleCannotInsertNameToNameTreeError, /* Cannot Insert Name To Name Tree */
        dleUncompatibleActionError,         /* Not Compatible Action */
        dleInvalidHandleUsingError,         /* Invalid Handle Using */
        dleUnknowDestinationTypeError,      /* Unknown Destination Type */
        dleIvalidAnnotItemError,            /* Invalid Annotation Item */
        dleNotBWImageAsMaskError,           /* Try append color image as b/w mask */
        dleColorImageWithBWCompressionError,/* Try compress color image with b/w compression */ 
        dleBWImageWithJPEGCompressionError, /* Try compress b/w image with JPEG compression */
        dleNotValidMaskImageError,          /* As mask possible use only mask images */ 
		dleUnsupportedTypeAcroFormError,	/* Cannot perform operation with current AcroForm object */
		dleCannotFindStateForAcroFormError, /* Cannot find state of AcroForm*/
		dleInvalidValueAcroFormError,		/* Invalid value for RadioBox or CheckBox*/
		dleCannotChangeValueForAcroFormError, /* Cannot change value for AcroForm*/
		dleSignatureNotAppendedError,	/* Digital signature not appended to document */
		dleSignatureWasAppendedError,   /* Digital signature was appended to document */
    } PDFDocumentLevelErrors;


    /************************************************************************
    Description :
        Error Type of Page Level of fault situations. When we are working
        with Pages API function of PDF Document.
    ************************************************************************/
    typedef enum {
        pleNoError = 0,                     /* No Error */
        pleOtherError,                      /* Other Error */
        pleInvalidRectangleValueError,      /* Invalid Rectangle Value */
        pleUnicodeAvailableForTrueTypeFontOnlyError,/* Unicode Available For TrueType Font Only */
		pleCannotCreateEMFFileError         /* Cannot Create EMF File */
    } PDFPageLevelErrors;

    /************************************************************************
    Description :
        Error Type of File Level of fault situations. When we are working
        with File System, File API functions. Storage of PDF Document.
    ************************************************************************/
    typedef enum {
        fleNoError = 0,                     /* No Error */
        fleOtherError,                      /* Other Error */
        fleUncryptedFileError,              /* Decrypted File */
		fleCryptedFileError,                /* Crypted File */
        fleCorruptedTrailerError,           /* Corrupted Trailer */
        fleCannotLoadPagesError,            /* Cannot Load Pages */
        fleDocumentNotContainPagesError,    /* Document Not Contain Pages */
        fleCorruptedFileError               /* Corrupted File */
    } PDFFileLevelErrors;

    /************************************************************************
    Description :
        Error Type of Cos Object Level of fault situations. When we are working
        with PDF Data Objects.
    ************************************************************************/
    typedef enum {
        cleNoError = 0,                         /* No Error */
        cleOtherError,                          /* Other Error */
        cleInvalidObjError,                     /* Invalid Object */
        cleNotCosObjError,                      /* Not Cos Object */
        cleArrayBoundsError,                    /* Array Bounds */
        cleUnterminatedStringError,             /* Unterminated String */
        cleIllegalCharacterInHexCharInNameError,/* Illegal Hex Character In Name */
        cleNameTokenTooLongError,               /* Name Token Too Long */
        cleIllegalCharacterError,               /* Illegal Character */
        cleIllegalCharacterInHexStringError,    /* Illegal Character In Hex String */
        cleCommandTokenTooLongError,            /* Command Token Too Long */
        cleEOFWasFoundError,                    /* EOF Was Found */
        cleUnknowTokenWasFoundError,            /* Unknown Token Was Found */
        cleDictKeyNotNameError,                 /* Dictionary Key Have Not Name */
        cleUnknowStreamLength,                  /* Unknown Stream Length */
        cleDataReadError,                       /* Data Read */
        cleCannotFindEndOfStreamError,          /* Cannot Find End Of Stream */
        cleIllegalCosObjectWasFoundError,       /* Illegal Cos Object Was Found */
        cleNoHeaderError,                       /* No Header */
        cleUnsupportedVersionError,             /* Unsupported Version */
        cleNoStartXRefError,                    /* No Start XRef */
        cleNoStartAddressError,                 /* No Start Address */
        cleBadXrefError,                        /* Bad XRef */
        cleBadTrailerError,                     /* Bad Trailer */
        cleUnexpectedEndStreamError,            /* Unexpected End of Stream */
        cleCorruptedXRefStreamError,            /* Corrupted XRef of Stream */
        cleInvalidEncryptInformationError,      /* Invalid Encrypt Information */
        cleUnsupportedErcryptAlgorithmError,    /* Unsupported encrypt Algorithm */
        cleFlateError,                          /* Flate */
        cleASCIIHexError,                       /* ASCII Hex */
        cleASCII85Error,                        /* ASCII 85 */
        cleUnsupporterFilterDecodeError,        /* Unsupported Filter Decode */
        cleUnfilteredStreamWasFoundError,       /* Unfiltered Stream Was Found */
        cleInvalidObjectStreamDictionaryError,  /* Invalid Object Stream Dictionary */
        cleInvalidObjectStreamError,            /* Invalid Object Stream */
        cleInvalidImageFormat,                  /* Invalid Image Format */
		cleInvalidFontFormat,                   /* Invalid Font Format */
        cleCannotInsertObjectWithParrentError,  /* Cannot Insert Object With Parrent */
        cleCannotInsertObjectFromOtherDocumentError,/* Cannot Insert Object From Other Document */
        cleDirectObjectWasFoundError            /* Direct Object Was Found */
    } PDFCosLevelErrors;




    /************************************************************************
    Description :
        Error Type of Digital Signature Level of fault situations. When we are
        working with PDF Signatures.
    ************************************************************************/
    typedef enum {
        sleNoError = 0,         /* No Error */
        sleOtherError,          /* Other Error */
        sleInvalidObjError,     /* Invalid Object */
        sleBadAsn1HeaderError,  /* Bad ASN1 Header */
        sleWrongAsn1TagError,   /* Bad ASN1 Tag */
        sleLengthMismatchError, /* Length Mismatch */
        sleNotImplementedError, /* Not Implemented */
        sleBadPasswordError     /* Bad Password */
    } PDFSignLevelErrors;


#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#ifdef __cplusplus
   extern "C"{
#endif
       /********************************************************************
       Description:
            Returns string value of error code.
       Parameters:
            ErrorCode - [ in ] Error Code.
            Buffer    - [ in ] Buffer where will stored string. Buffer size must be >= 255 chars
       Returns:
            Size of the string.
       *********************************************************************/
       
       LIB_API ppInt32 VSGetErrorStr ( ppUns32 ErrorCode, char * Buffer );

#ifdef __cplusplus
   }
#endif


#endif /* #ifndef VSError_H */
