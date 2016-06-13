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
VSSignA.c
*********************************************************************/

#include <time.h>
#include <string.h>

#include "VSSignAI.h"
#include "VSDocAI.h"
#include "VSMisc.h"

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib

#ifndef NOT_USE_SIGN

PDFPfxHandle PDFPfxCreate(PDFLibHandle Lib)
{
    PDFPfx *rslt;
    rslt = ( PDFPfx * ) mmalloc ( Lib, sizeof ( PDFPfx ) );
    rslt->certificate = NULL; /*x509_Create(_LIB);*/ 
    rslt->key = NULL; /*cipher_CreatePrivateKey(_LIB);*/
    return ( PDFPfxHandle ) rslt;
}

void PDFPfxDispose( PDFLibHandle Lib, PDFPfxHandle hpfx)
{
    PDFPfx *pfx;
    if ( hpfx == NULL )
        return;
    pfx = ( PDFPfx * ) hpfx;
    if ( pfx->certificate ){
        x509_Dispose ( Lib, pfx->certificate );
        pfx->certificate = NULL;
    }
    if ( pfx->key ){
        rsa_DisposeKey ( Lib, pfx->key );
        pfx->key = NULL;
    }
    mfree ( Lib, pfx );
}

PDFPfxHandle PDFPfxLoadFromFile(PDFLibHandle Lib, char *fn, const ppUns8 *pwd)
{
    PDFPfxHandle p;
    PDFStreamHandle ms;
    ms = ULStreamFileNew ( Lib, fn, ppFileReadMode );
    PDFTRY ( Lib ){
        p = PDFPfxLoadFromStream ( Lib, ms, pwd );
    } PDFEXCEPT ( Lib ){
        ULStreamClose ( ms );
	    PDFRERAISE( Lib );
    }
    PDFTRYEND ( Lib )
    ULStreamClose ( ms );

    return p;
}

PDFPfxHandle PDFPfxLoadFromBuffer(PDFLibHandle Lib, void *buf, ppInt32 len, const ppUns8 *pwd)
{
    PDFPfxHandle p;
    PDFStreamHandle ms;
    ms = ULStreamMemReadOnlyNew ( Lib, buf, len );
    PDFTRY ( Lib ){
        p = PDFPfxLoadFromStream ( Lib, ms, pwd );
    } PDFEXCEPT ( Lib ){
        ULStreamClose ( ms );
	    PDFRERAISE( Lib );
    }
    PDFTRYEND ( Lib )
    ULStreamClose ( ms );
    return p;
}

PDFPfxHandle PDFPfxLoadFromStream(PDFLibHandle Lib, PDFStreamHandle strm, const ppUns8 *pwd)
{
    pkcs12 *p12 = NULL;
    ppUns8 *buf, *sbuf;
    ppInt32 size;
    PDFPfx *rslt= NULL;

    PDFTRY ( Lib ){
        size = ULStreamGetSize ( strm );
        buf = ( ppUns8 * ) mmalloc ( Lib, size );
        sbuf = buf;
        ULStreamReadBuffer ( strm, buf, size );
        pkcs12_Decode ( Lib, &p12, &buf, size );
        mfree ( Lib, sbuf ); 
        rslt = ( PDFPfx * )PDFPfxCreate ( Lib );
        pkcs12_Parse ( Lib, p12, (const char *)pwd, ( &rslt->key ), (&rslt->certificate ), NULL );
        pkcs12_Dispose ( Lib, p12 ); p12 = NULL;
    } PDFEXCEPT ( Lib ){
        if ( p12 )
            pkcs12_Dispose ( Lib, p12 );
        if ( rslt )
            PDFPfxDispose ( Lib, rslt );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    return ( PDFPfxHandle ) rslt;
}

pkcs1 * pkcs1_Create(PDFLibHandle Lib)
{
    pkcs1 *rslt;
    rslt = ( pkcs1 * ) mmalloc ( Lib, sizeof ( pkcs1 ) );
    PDFTRY ( Lib ){
        rslt->sig = asn1_CreateOctetString ( Lib );
        rslt->asn1 = NULL; rslt->length = 0;
    } PDFEXCEPT ( Lib ){
        pkcs1_Dispose ( Lib, rslt );
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
    return rslt;
}

void pkcs1_Dispose(PDFLibHandle Lib, pkcs1 *obj)
{
    if ( obj == NULL )
        return;
    asn1_DisposeOctetString ( Lib,  obj->sig );
    if ( obj->asn1 ){
        mfree ( Lib, obj->asn1 );
        obj->asn1 = NULL;
        obj->length = 0;
    }
    mfree ( Lib, obj );
}

/* --- */

sig_Context * sig_CreateContext(PDFDocHandle Doc)
{
    sig_Context *rslt;
    rslt = ( sig_Context * ) mmalloc ( _LIB, sizeof ( sig_Context ) );
    PDFTRY ( _LIB ){
        rslt->lib = _LIB;
        rslt->reason = NULL;
        rslt->signer = NULL;
        rslt->field = NULL;
        rslt->PageIndex = 0;
        rslt->Rect = PDFRect(0,0,0,0);
		rslt->VisibleContent = _CosNull(Doc);
        rslt->isPKCS7 = false;
		rslt->IsVisible = false;
        rslt->contents.ptr = NULL;
        rslt->digitalID = ( PDFPfx * )PDFPfxCreate ( _LIB );
    } PDFEXCEPT ( _LIB ){
        sig_DisposeContext ( _LIB, rslt );
        PDFRERAISE ( _LIB );
    }PDFTRYEND ( _LIB );
    return rslt;
}

void sig_DisposeContext( PDFLibHandle Lib, sig_Context *ctx)
{
    if ( ctx == NULL )
        return;
    chfreez ( Lib, ctx->signer );
    chfreez ( Lib, ctx->reason );
    chfreez ( Lib, ctx->field );
    if ( ctx->isPKCS7 )
        pkcs7_Dispose ( Lib, ctx->contents.p7 );
    else
        pkcs1_Dispose ( Lib, ctx->contents.p1 );
    PDFPfxDispose ( Lib, ctx->digitalID );
    mfree ( Lib, ctx );
}

void sig_SetSigner( PDFSigHandle hctx, char *name)
{
    sig_Context *ctx= ( sig_Context * ) hctx;
    if ( !ctx || !name )
        return;
    chfreez ( ctx->lib, ctx->signer );
    ctx->signer = ( char * ) mmalloc ( ctx->lib, strlen ( name ) + 1 );
    strcpy ( ctx->signer, name );
}

void sig_SetReason( PDFSigHandle hctx, char *reason)
{
    sig_Context *ctx= ( sig_Context * ) hctx;
    if ( !ctx || !reason )
        return;
    chfreez ( ctx->lib, ctx->reason );
    ctx->reason = ( char * ) mmalloc ( ctx->lib, strlen ( reason ) + 1 );
    strcpy ( ctx->reason, reason );
}

void sig_SetField( PDFSigHandle hctx, char *field)
{
    sig_Context *ctx= ( sig_Context * ) hctx;
    if ( !ctx || !field )
        return;
    chfreez ( ctx->lib, ctx->field );
    ctx->field = ( char * ) mmalloc ( ctx->lib, strlen ( field ) + 1 );
    strcpy ( ctx->field, field );
}

void sig_AssignDigitalID(PDFLibHandle Lib, sig_Context *ctx, PDFPfxHandle hpfx)
{
    char *buf = NULL;
    PDFTRY ( Lib ){
        PDFPfxDispose ( Lib, ( PDFPfxHandle ) ctx->digitalID );
        ctx->digitalID = ( PDFPfx * ) PDFPfxCreate ( Lib );
        ctx->digitalID->certificate = (x509 *) asn1_Clone ( Lib, (TEncodeFunction)x509_Encode, (TDecodeFunction) x509_Decode,
            ( ( PDFPfx * ) hpfx )->certificate );
        ctx->digitalID->key = (rsa_Key *) asn1_Clone ( Lib, (TEncodeFunction)rsa_EncodePrivateKey, (TDecodeFunction)rsa_DecodePrivateKey,
            ( ( PDFPfx * ) hpfx )->key );
        buf = x509_GetSubjectCommonName ( ctx->lib, ctx->digitalID->certificate );
        sig_SetSigner ( ctx, buf );
        mfree ( Lib, buf );
    } PDFEXCEPT ( Lib ){
        mfree ( Lib, buf );
        if ( ctx->digitalID ){
            PDFPfxDispose ( Lib, ctx->digitalID );
            ctx->digitalID = NULL;
        }
        PDFRERAISE ( Lib );
    }
    PDFTRYEND ( Lib );
}

void sig_AddSigDict(PDFDocHandle Doc)
{
    PDFCosHandle byteRange, sigDict,l;
    sig_Context *c;

    char SignDate[24];
    struct tm  *lcDateTime, *gmDateTime;
    time_t      DateTime;
    ppInt32     lH, lM, dGMTH, dGMTM, dGMT;

    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;

    byteRange = CosArrayNew ( Doc, false, 4 );
    CosArrayAppend ( byteRange, CosIntNew ( Doc, false, 0 ) );
    CosArrayAppend ( byteRange, CosIntNew ( Doc, false, ppMAXInt32 ) );
    CosArrayAppend ( byteRange, CosIntNew ( Doc, false, ppMAXInt32 ) );
    CosArrayAppend ( byteRange, CosIntNew ( Doc, false, ppMAXInt32 ) );

    sigDict = CosDictNew ( Doc, true, 10 );
    _CosDictAppend ( sigDict, Type, _CosNameNew ( Sig ) );
    c = _DOC->SigCtx;
    if ( c->isPKCS7 ){
        _CosDictAppend ( sigDict, Filter, CosNameNew ( Doc, false, 
            ULStringToAtom ( _LIB, "Adobe.PPKMS" ) ) );
        _CosDictAppend ( sigDict, SubFilter, CosNameNew ( Doc, false, 
            ULStringToAtom ( _LIB, "adbe.pkcs7.sha1" ) ) );
        _CosDictAppend ( sigDict, Contents, l = CosStringNew ( Doc, false, (char *)c->contents.p7->asn1, 
            (ppInt32)c->contents.p7->length ) );
        _CosStringHex(l) = true;
    } else{
        _CosDictAppend ( sigDict, Filter, CosNameNew ( Doc, false, 
            ULStringToAtom ( _LIB, "Adobe.PPKLite" ) ) );
        _CosDictAppend ( sigDict, SubFilter, CosNameNew ( Doc, false, ULStringToAtom ( _LIB, 
            "adbe.x509.rsa_sha1" ) ) );
        _CosDictAppend ( sigDict, Contents, l=CosStringNew ( Doc, false, (char *)c->contents.p1->asn1, 
            c->contents.p1->length) );
        _CosStringHex(l) = true;
        _CosDictAppend ( sigDict, Cert, CosStringNew ( Doc, false, (char *)c->digitalID->certificate->asn1,
            (ppInt32)c->digitalID->certificate->length) );
    }
    _CosDictAppend ( sigDict, Name, CosStringNew ( Doc, false, c->signer, 
        ( ppInt32 ) strlen ( c->signer ) ) );


    /*M*/
    DateTime = time( NULL );
    lcDateTime = localtime( &DateTime );
    lH = lcDateTime->tm_hour;
    lM = lcDateTime->tm_min;
    gmDateTime = gmtime( &DateTime );
    dGMTH = lH - gmDateTime->tm_hour;
    dGMTM = lM - gmDateTime->tm_min;
    dGMT = dGMTH * 60 + dGMTM;
    lcDateTime->tm_year = lcDateTime->tm_year + 1900;
    lcDateTime->tm_mon = lcDateTime->tm_mon + 1;
    sprintf( SignDate, "D:%4d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%c%1d%1d'%1d%1d'",
        lcDateTime->tm_year, lcDateTime->tm_mon/10, lcDateTime->tm_mon%10, lcDateTime->tm_mday/10,
        lcDateTime->tm_mday%10, lH/10, lH%10, lM/10, lM%10, lcDateTime->tm_sec/10,
        lcDateTime->tm_sec%10, ( dGMT < 0 )?'-':'+', _abs(dGMTH/10), _abs(dGMTH%10), _abs(dGMTM/10), _abs(dGMTM%10) );
    SignDate[23] = '\0';

    _CosDictAppend ( sigDict, M, CosStringNew ( Doc, false, SignDate, ( ppInt32 ) strlen ( SignDate ) ) );
    _CosDictAppend ( sigDict, Reason, CosStringNew ( Doc, false,  c->reason, 
        ( ppInt32 ) strlen ( c->reason ) ) );
    _CosDictAppend ( sigDict, ByteRange, byteRange );

    _DOC->SigCtx->Sign = sigDict;
}



void sig_AddSigAnnot(PDFDocHandle Doc)
{
    PPDFPageInfo pageInfo;
    PDFCosHandle rect, annot, AS;

    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;
    _CosNullNew(Doc, rect);
    _CosNullNew(Doc, annot);

    PDFTRY ( _LIB ){
        if ( !_DOC->Pages )
            PDFDocLoadPages ( Doc );
        /* ASSERT(0 < page <= npages) */
        pageInfo = _DocPage( Doc,_DOC->SigCtx->PageIndex);
        rect = CosArrayNew ( Doc, false, 4 );
        CosArrayAppend ( rect, CosRealNew ( Doc, false, _DOC->SigCtx->Rect.xl ) );
        CosArrayAppend ( rect, CosRealNew ( Doc, false, _DOC->SigCtx->Rect.yl ) );
        CosArrayAppend ( rect, CosRealNew ( Doc, false, _DOC->SigCtx->Rect.xr ) );
        CosArrayAppend ( rect, CosRealNew ( Doc, false, _DOC->SigCtx->Rect.yr ) );
        annot = CosDictNew ( Doc,true, 8 );
        _DOC->SigCtx->Annot=annot;
        _CosDictAppend ( annot, F, CosIntNew ( Doc, false, 132 ) );
        _CosDictAppend ( annot, Type, _CosNameNew ( Annot ) );
        _CosDictAppend ( annot, Rect, rect );
        _CosDictAppend ( annot, FT, _CosNameNew ( Sig ) );
        _CosDictAppend ( annot, Subtype, _CosNameNew ( Widget ) );
        _CosDictAppend ( annot, P, pageInfo->PO ); 
		if (_DOC->SigCtx->IsVisible){
			_CosDictAppend ( annot, AP, AS = CosDictNew (Doc, false, 1 ) );
			_CosDictAppend ( AS, N, _DOC->SigCtx->VisibleContent);
		}
        /* ASSERT(field) */
        _CosDictAppend ( annot, T, CosStringNew ( Doc, false, _DOC->SigCtx->field,
            ( ppInt32 ) strlen ( _DOC->SigCtx->field )) );
        /* ASSERT(SID) */
        _CosDictAppend ( annot, V, _DOC->SigCtx->Sign );
    } PDFEXCEPT ( _LIB ){
        CosFree ( rect );
        CosFree ( annot );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
}

void sig_UpdateAcroForm(PDFDocHandle Doc)
{
    PDFCosHandle acroform;
    PDFCosHandle fldArray, sigFlags;

    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;
    _CosNullNew(Doc, acroform);
    acroform = _CosDictValueByName ( _DOC->Root, AcroForm );
    if ( !_IsCosDict( acroform ) ){
        PDFTRY ( _LIB ){
            acroform = CosDictNew ( Doc, true, 4 );
            _CosDictAppend ( _DOC->Root, AcroForm, acroform );
        } PDFEXCEPT ( _LIB ){
            CosFree ( acroform );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB )
    }

    /* Fields */
    fldArray = _CosDictValueByName ( acroform, Fields );
    if ( !_IsCosArray(fldArray) )
        _CosDictAppend ( acroform, Fields, fldArray = CosArrayNew ( _DOC, false, 4 ) );
    CosArrayAppend ( fldArray, _DOC->SigCtx->Annot );

    /*SigFlags*/
    sigFlags = _CosDictValueByName ( acroform, SigFlags );
    if ( !_IsCosInt ( sigFlags ) )
        _CosDictAppend ( acroform, SigFlags, CosIntNew ( Doc, false, 3 ) );
    else
        CosIntSetValue ( sigFlags, 3 );

    /*DR (optional)*/
    /*DA (optional)*/
}

void sig_UpdatePageAnnots(PDFDocHandle Doc)
{
    PDFCosHandle fldArray, pageDict;
    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;
    if ( !_DOC->Pages )
        PDFDocLoadPages ( Doc );
    pageDict = _DocPage(Doc, _DOC->SigCtx->PageIndex )->PO;
    fldArray = _CosDictValueByNameNRF ( pageDict, Annots );
    if ( !_IsCosArray (fldArray))
        _CosDictAppend ( pageDict, Annots, fldArray = CosArrayNew ( _DOC, false, 4 ) );
    CosArrayAppend ( fldArray, _DOC->SigCtx->Annot );
}

void sig_CreateContents(PDFDocHandle Doc)
{
    pkcs1 *p1 = NULL;
    pkcs7 *p7 = NULL, *ci = NULL;
    pkcs7_IASN *iasn = NULL;
    pkcs7_SignedData *sd = NULL;
    pkcs7_SignerInfo *si = NULL;
    x509 *ct;
    x509_Algorithm *da = NULL;
    ppUns8 hbuf[SHA1_HASHSIZE], *buf = NULL, *p;
    int size, hlen = SHA1_HASHSIZE;

    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;

    if ( _DOC->SigCtx->contents.ptr != NULL ){
        if ( _DOC->SigCtx->isPKCS7 )
            pkcs7_Dispose ( _LIB, _DOC->SigCtx->contents.p7 );
        else
            pkcs1_Dispose ( _LIB, _DOC->SigCtx->contents.p1 );
    }
    if ( _DOC->SigCtx->isPKCS7 ){
        PDFTRY ( _LIB ){
            p7 = pkcs7_Create ( _LIB );
            p7->contentType = asn1_CreateObjectByID ( _LIB, ID_pkcs7SignedData );

            sd = pkcs7_CreateSignedData ( _LIB );
            asn1_SetInteger ( _LIB, sd->version, 1 );

            da = x509_CreateAlgorithm ( _LIB ); 
            da->algorithm = asn1_CreateObjectByID ( _LIB, ID_sha1WithRSAEncryption );
            da->parameter = asn1_CreateType ( _LIB );
            asn1_SetType ( _LIB, da->parameter, NULL_TAG, NULL );
            vector_Push ( sd->digestAlgorithms, ( char * ) da );

            ci = pkcs7_Create ( _LIB );
            ci->contentType = asn1_CreateObjectByID ( _LIB, ID_pkcs7Data );
            ci->contents.data = asn1_CreateOctetString ( _LIB );
            memset ( hbuf, 0xff, hlen ); /*placeholder*/
            asn1_SetString ( _LIB, ci->contents.data, hbuf, hlen );
            pkcs7_Dispose ( _LIB, sd->contentInfo );
            sd->contentInfo = ci;

            sd->certificates = vector_Create ( _LIB );
            ct = (x509*)asn1_Clone ( _LIB, (TEncodeFunction)x509_Encode, (TDecodeFunction)x509_Decode, _DOC->SigCtx->digitalID->certificate );
            vector_Push ( sd->certificates, ( char * ) ct );

            si = pkcs7_CreateSignerInfo ( _LIB );
            asn1_SetInteger ( _LIB, si->version, 1 );

            iasn = pkcs7_IASNFromCertInfo ( _LIB, ct->cert_info );
            pkcs7_DisposeIssuerAndSerialNumber ( _LIB, si->issuerAndSerialNumber );
            si->issuerAndSerialNumber = iasn;

            da = x509_CreateAlgorithm ( _LIB ); 
            da->algorithm = asn1_CreateObjectByID ( _LIB, ID_sha1WithRSAEncryption );
            da->parameter = asn1_CreateType ( _LIB );
            asn1_SetType ( _LIB, da->parameter, NULL_TAG, NULL );
            x509_DisposeAlgorithm ( _LIB, si->digestAlgorithm );
            si->digestAlgorithm = da;

            da = x509_CreateAlgorithm ( _LIB ); 
            da->algorithm = asn1_CreateObjectByID ( _LIB, ID_rsaEncryption );
            da->parameter = asn1_CreateType ( _LIB );
            asn1_SetType ( _LIB, da->parameter, NULL_TAG, NULL );
            x509_DisposeAlgorithm ( _LIB, si->digestEncryptionAlgorithm );
            si->digestEncryptionAlgorithm = da;

            size = rsa_KeySize ( _DOC->SigCtx->digitalID->key );
            buf = ( ppUns8 * ) mmalloc ( _LIB, size + 1 ); memset ( buf, 0xff, size );/*placeholder*/
            asn1_SetString ( _LIB, si->encryptedDigest, buf, size );
            mfree ( _LIB, buf );

            vector_Push ( sd->signerInfos, ( char * ) si );

            p7->contents.signedData = sd;
            p7->length = pkcs7_Encode ( p7, NULL ) + 4; /* pad with four zero bytes */
            p7->asn1 = ( ppUns8 * ) mmalloc ( _LIB, (size_t)p7->length );
            memset ( p7->asn1, 0, (size_t)p7->length );
            p = p7->asn1; pkcs7_Encode ( p7, &p );

            _DOC->SigCtx->contents.p7 = p7;
        } PDFEXCEPT ( _LIB ){
            if ( p7 )
                pkcs7_Dispose ( _LIB, p7 );
            if ( da )
                x509_DisposeAlgorithm ( _LIB, da );
            if ( sd )
                pkcs7_DisposeSignedData ( _LIB, sd );
            if ( ci )
                pkcs7_Dispose ( _LIB, ci );
            if ( iasn )
                pkcs7_DisposeIssuerAndSerialNumber ( _LIB, iasn );
            if ( si )
                pkcs7_DisposeSignerInfo ( _LIB,  si );
            mfree( _LIB, buf );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
    } else{
        PDFTRY ( _LIB ){
            p1 = pkcs1_Create ( _LIB );
            size = rsa_KeySize ( _DOC->SigCtx->digitalID->key );
            buf = ( ppUns8 * ) mmalloc ( _LIB, size + 1 ); memset ( buf, 0xff, size );/*placeholder*/
            asn1_SetString ( _LIB, p1->sig, buf, size );
            mfree ( _LIB, buf );
            p1->length = asn1_EncodeOctetString ( p1->sig, NULL ) + 1; /* pad with one zero byte */
            p1->asn1 = ( ppUns8 * ) mmalloc ( _LIB, p1->length );
            memset ( p1->asn1, 0, p1->length );
            p = p1->asn1; asn1_EncodeOctetString ( p1->sig, &p );
            _DOC->SigCtx->contents.p1 = p1;

            ct = _DOC->SigCtx->digitalID->certificate;
            if ( ct->asn1 )
                mfree ( _LIB, ct->asn1 );
            ct->length = x509_Encode ( ct, NULL );
            ct->asn1 = ( ppUns8 * ) mmalloc ( _LIB, (size_t)ct->length );
            p = ct->asn1; x509_Encode ( ct, &p );
        } PDFEXCEPT ( _LIB ){
            if ( p1 )
                pkcs1_Dispose ( _LIB, p1 );
            mfree( _LIB, buf );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
    }
}

void sig_UpdateByteRange(PDFDocHandle Doc, int s1, int o2, int s2)
{
    PDFCosHandle sigDict, byteRange;
    ppAtom aKey, aByteRange;
    ppUns32 i;
    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;

    sigDict = _DOC->SigCtx->Sign;
    if ( CosGetType ( sigDict ) != CosDict )
        return;

    aByteRange = GetAtomLib ( _LIB, ByteRange );
    for ( i = 0; i < _CosDictCount( sigDict ); i++ ){
        aKey = _CosDictKey( sigDict,i );
        if ( aKey == aByteRange )
            break;
    }
    if ( aKey == aByteRange ){
        byteRange = _CosDictValue( sigDict,i );
        if (!_IsCosArray(byteRange))
            return;
        if ( !_CosArrayMinLen(  byteRange ) )
            _CosArrayMinLen(  byteRange )=CosGetObjSize(byteRange);
        CosIntSetValue ( _CosArrayItem ( byteRange, 1 ), s1 );
        CosIntSetValue ( _CosArrayItem ( byteRange, 2 ), o2 );
        CosIntSetValue ( _CosArrayItem ( byteRange, 3 ), s2 );
    }
}

void sig_UpdateContents(PDFDocHandle Doc, ppUns8 *hash, int hlen)
{
    PDFCosHandle sigDict, l;
    ppAtom aKey, aContents;
    pkcs1 *p1;
    pkcs7 *p7, *ci;
    pkcs7_SignedData *sd;
    pkcs7_SignerInfo *si;
    rsa_Key *key;
    sha1_Context hctx;
    ppUns8 *buf, *p;
    ppUns32 i, len;
	ppInt32 slen;

    if ( !_DOC->Signed || _DOC->SigCtx == NULL ) /**/
        return;

    sigDict =  _DOC->SigCtx->Sign;
    if ( CosGetType ( sigDict ) != CosDict )
        return;

    aContents = GetAtomLib ( _LIB, Contents );
    for ( i = 0; i < _CosDictCount( sigDict ); i++ ){
        aKey = _CosDictKey( sigDict, i );
        if ( aKey == aContents )
            break;
    }

    /* if ( aKey != aContents ) then exception: we don't have contents */

    if ( _DOC->SigCtx->isPKCS7 ){
        p7 = _DOC->SigCtx->contents.p7;
        sd = p7->contents.signedData;
        ci = sd->contentInfo;
        asn1_SetString ( _LIB, ci->contents.data, hash, hlen );
        sha1_Init ( &hctx );
        sha1_Update ( &hctx, hash, hlen );
        sha1_Finish ( &hctx, hash );
    }

    key = _DOC->SigCtx->digitalID->key;
    slen = rsa_KeySize ( key );
    buf = ( ppUns8 * ) mmalloc ( _LIB, slen + 1 );
    rsa_Sign ( _LIB, ID_sha1, hash, hlen, buf, &slen, key );
	len = slen;
    if ( _DOC->SigCtx->isPKCS7 ){
        si = ( pkcs7_SI * ) vector_GetValueAt ( sd->signerInfos, 0 );
        asn1_SetString ( _LIB, si->encryptedDigest, buf, len );
    } else{
        p1 = _DOC->SigCtx->contents.p1;
        asn1_SetString ( _LIB, p1->sig, buf, len );
    }
    memset ( buf, 0, len + 1 ); mfree ( _LIB, buf );

    if ( _DOC->SigCtx->isPKCS7 ){
        p = _DOC->SigCtx->contents.p7->asn1; 
        pkcs7_Encode ( _DOC->SigCtx->contents.p7, &p );
        _CosDictAppend ( sigDict, Contents,l = CosStringNew ( Doc, false, 
            (char *)_DOC->SigCtx->contents.p7->asn1, (ppInt32)_DOC->SigCtx->contents.p7->length ) );
        _CosStringHex(l)= true;
    } else{
        p = _DOC->SigCtx->contents.p1->asn1; 
        asn1_EncodeOctetString ( _DOC->SigCtx->contents.p1->sig, &p );
        _CosDictAppend ( sigDict, Contents,l= CosStringNew (Doc, false, (char *)_DOC->SigCtx->contents.p1->asn1,
            _DOC->SigCtx->contents.p1->length ) );
        _CosStringHex(l)= true;
    }
}

PDFSigHandle PDFDocPrepSign(PDFDocHandle Doc)
{
    /* if ( _DOC->Signed )then exception: too late... */
    sig_DisposeContext ( _LIB, _DOC->SigCtx );
    _DOC->SigCtx = sig_CreateContext ( Doc );
    return ( PDFSigHandle ) _DOC->SigCtx;
}

void PDFDocSign(PDFDocHandle Doc, PDFPfxHandle hpfx)
{
    /* if ( _DOC->Signed ) then exception: can't sign twice */
    /* if ( _DOC->SigCtx == NULL ) then exception: not prepared */
    _DOC->Signed = true;
    sig_AssignDigitalID ( _LIB, _DOC->SigCtx, hpfx );
    sig_CreateContents ( Doc );
}

#endif
