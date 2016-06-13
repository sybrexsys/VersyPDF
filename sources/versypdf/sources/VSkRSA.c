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
VSkRSA.c
*********************************************************************/

#include "VSkPKCS12I.h"
#include "VSkRSAI.h"
#include "VSCrypt.h"
#include <string.h>

#ifndef NOT_USE_SIGN

#define LargeNumberASSIGN_DIGIT(a, b, digits) {LargeNumberAssignZero (a, digits); a[0] = b;}

/*Computes a = b * c, where b and c are digits.  Lengths: a[2]. */
void ppUns64Mult (ppUns32 *a, ppUns32 b, ppUns32 c)
{
    ppUns32 t, u;
    ppUns32 bHigh, bLow, cHigh, cLow;

    bHigh = (ppUns32)HIGH_HALF (b);
    bLow = (ppUns32)LOW_HALF (b);
    cHigh = (ppUns32)HIGH_HALF (c);
    cLow = (ppUns32)LOW_HALF (c);

    a[0] = (ppUns32)bLow * (ppUns32)cLow;
    t = (ppUns32)bLow * (ppUns32)cHigh;
    u = (ppUns32)bHigh * (ppUns32)cLow;
    a[1] = (ppUns32)bHigh * (ppUns32)cHigh;

    if ((t += u) < u)
        a[1] += TO_HIGH_HALF (1);
    u = TO_HIGH_HALF (t);

    if ((a[0] += u) < u)
        a[1]++;
    a[1] += HIGH_HALF (t);
}

void ppUns64Div ( ppUns32 *a, ppUns32 * b, ppUns32 c)
{
    ppUns32 t[2], u, v;
    ppUns32 aHigh, aLow, cHigh, cLow;

    cHigh = (ppUns32)HIGH_HALF (c);
    cLow = (ppUns32)LOW_HALF (c);

    t[0] = b[0];
    t[1] = b[1];

    if (cHigh == MAX_ppUns32)
        aHigh = (ppUns32)HIGH_HALF (t[1]);
    else
        aHigh = (ppUns32)(t[1] / (cHigh + 1));
    u = (ppUns32)aHigh * (ppUns32)cLow;
    v = (ppUns32)aHigh * (ppUns32)cHigh;
    if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_ppUns64 - TO_HIGH_HALF (u)))
        t[1]--;
    t[1] -= HIGH_HALF (u);
    t[1] -= v;

    while ((t[1] > cHigh) ||
        ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
            if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_ppUns64 - TO_HIGH_HALF (cLow))
                t[1]--;
            t[1] -= cHigh;
            aHigh++;
        }

        if (cHigh == MAX_ppUns32)
            aLow = (ppUns32)LOW_HALF (t[1]);
        else
            aLow = 
            (ppUns32)((TO_HIGH_HALF (t[1]) + HIGH_HALF (t[0])) / (cHigh + 1));
        u = (ppUns32)aLow * (ppUns32)cLow;
        v = (ppUns32)aLow * (ppUns32)cHigh;
        if ((t[0] -= u) > (MAX_ppUns64 - u))
            t[1]--;
        if ((t[0] -= TO_HIGH_HALF (v)) > (MAX_ppUns64 - TO_HIGH_HALF (v)))
            t[1]--;
        t[1] -= HIGH_HALF (v);

        while ((t[1] > 0) || ((t[1] == 0) && t[0] >= c)) {
            if ((t[0] -= c) > (MAX_ppUns64 - c))
                t[1]--;
            aLow++;
        }

        * a = TO_HIGH_HALF (aHigh) + aLow;
}

void LargeNumberDecode ( ppUns32 *a, ppUns32 digits, ppUns8 * b, ppUns32 len)
{
    ppUns32 t;
    ppInt32 j;
    ppUns32 i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
        t = 0;
        for (u = 0; j >= 0 && u < ppUns64_BITS; j--, u += 8)
            t |= ((ppUns32)b[j]) << u;
        a[i] = t;
    }

    for (; i < digits; i++)
        a[i] = 0;
}

void LargeNumberEncode (ppUns8 *a, ppUns32 len, ppUns32 * b,  ppUns32 digits)
{
    ppUns32 t;
    ppInt32 j;
    ppUns32 i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
        t = b[i];
        for (u = 0; j >= 0 && u < ppUns64_BITS; j--, u += 8)
            a[j] = (ppUns8)(t >> u);
    }

    for (; j >= 0; j--)
        a[j] = 0;
}

void LargeNumberAssign ( ppUns32 * a, ppUns32 *b, ppUns32 digits)
{
    ppUns32 i;

    for (i = 0; i < digits; i++)
        a[i] = b[i];
}

void LargeNumberAssignZero ( ppUns32 *a, ppUns32 digits)
{
    ppUns32 i;

    for (i = 0; i < digits; i++)
        a[i] = 0;
}

void LargeNumberAssign2Exp ( ppUns32 *a,  ppUns32 b, ppUns32 digits)
{
    LargeNumberAssignZero (a, digits);

    if (b >= digits * ppUns64_BITS)
        return;

    a[b / ppUns64_BITS] = (ppUns32)1 << (b % ppUns64_BITS);
}

ppUns32 LargeNumberAdd ( ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 digits)
{
    ppUns32 ai, carry;
    ppUns32 i;

    carry = 0;

    for (i = 0; i < digits; i++) {
        if ((ai = b[i] + carry) < carry)
            ai = c[i];
        else if ((ai += c[i]) < c[i])
            carry = 1;
        else
            carry = 0;
        a[i] = ai;
    }

    return (carry);
}

ppUns32 LargeNumberSub (ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 digits)
{
    ppUns32 ai, borrow;
    ppUns32 i;

    borrow = 0;

    for (i = 0; i < digits; i++) {
        if ((ai = b[i] - borrow) > (MAX_ppUns64 - borrow))
            ai = MAX_ppUns64 - c[i];
        else if ((ai -= c[i]) > (MAX_ppUns64 - c[i]))
            borrow = 1;
        else
            borrow = 0;
        a[i] = ai;
    }

    return (borrow);
}

void LargeNumberMult (ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 digits)
{
    ppUns32 t[2*MAX_ppUns64S];
    ppUns32 bDigits, cDigits, i;

    LargeNumberAssignZero (t, 2 * digits);

    bDigits = ppUns64s (b, digits);
    cDigits = ppUns64s (c, digits);

    for (i = 0; i < bDigits; i++)
        t[i+cDigits] += LargeNumberAddDigitMult (&t[i], &t[i], b[i], c, cDigits);

    LargeNumberAssign (a, t, 2 * digits);
    memset ((ppUns8 * )t, 0, sizeof (t));
}

ppUns32 LargeNumberLShift (ppUns32 *a, ppUns32 *b, ppUns32 c, ppUns32 digits)
{
    ppUns32 bi, carry;
    ppUns32 i, t;

    if (c >= ppUns64_BITS)
        return (0);
    t = ppUns64_BITS - c;
    carry = 0;
    for (i = 0; i < digits; i++) {
        bi = b[i];
        a[i] = (bi << c) | carry;
        carry = c ? (bi >> t) : 0;
    }
   return (carry);
}

ppUns32 LargeNumberRShift (ppUns32 *a, ppUns32 *b, ppUns32 c, ppUns32 digits)
{
    ppUns32 bi, carry;
    ppInt32 i;
    ppUns32 t;

    if (c >= ppUns64_BITS)
        return (0);

    t = ppUns64_BITS - c;

    carry = 0;

    for (i = digits - 1; i >= 0; i--) {
        bi = b[i];
        a[i] = (bi >> c) | carry;
        carry = c ? (bi << t) : 0;
    }

    return (carry);
}

void LargeNumberDiv ( ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 cDigits, ppUns32 *d, ppUns32 dDigits)
{
    ppUns32 ai, cc[2*MAX_ppUns64S+1], dd[MAX_ppUns64S], t;
    ppInt32 i;
    ppUns32 ddDigits, shift;

    ddDigits = ppUns64s (d, dDigits);
    if (ddDigits == 0)
        return;

    shift = ppUns64_BITS - ppUns64Bits (d[ddDigits-1]);
    LargeNumberAssignZero (cc, ddDigits);
    cc[cDigits] = LargeNumberLShift (cc, c, shift, cDigits);
    LargeNumberLShift (dd, d, shift, ddDigits);
    t = dd[ddDigits-1];

    LargeNumberAssignZero (a, cDigits);

    for (i = cDigits-ddDigits; i >= 0; i--) {
        if (t == MAX_ppUns64)
            ai = cc[i+ddDigits];
        else
            ppUns64Div (&ai, &cc[i+ddDigits-1], t + 1);
        cc[i+ddDigits] -= LargeNumberSubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);

        while (cc[i+ddDigits] || (LargeNumberCmp (&cc[i], dd, ddDigits) >= 0)) {
            ai++;
            cc[i+ddDigits] -= LargeNumberSub (&cc[i], &cc[i], dd, ddDigits);
        }

        a[i] = ai;
    }

    LargeNumberAssignZero (b, dDigits);
    LargeNumberRShift (b, cc, shift, ddDigits);

    memset ((ppUns8 * )cc, 0, sizeof (cc));
    memset ((ppUns8 * )dd, 0, sizeof (dd));
}

void LargeNumberMod (ppUns32 *a, ppUns32 *b, ppUns32 bDigits, ppUns32 *c, ppUns32 cDigits)
{
    ppUns32 t[2 * MAX_ppUns64S];

    LargeNumberDiv (t, a, b, bDigits, c, cDigits);

    memset ((ppUns8 * )t, 0, sizeof (t));
}

void LargeNumberModMult (ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 *d, ppUns32 digits)
{
    ppUns32 t[2*MAX_ppUns64S];

    LargeNumberMult (t, b, c, digits);
    LargeNumberMod (a, t, 2 * digits, d, digits);

    memset ((ppUns8 * )t, 0, sizeof (t));
}

void LargeNumberModExp (ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 cDigits, ppUns32 *d, ppUns32 dDigits)
{
    ppUns32 bPower[3][MAX_ppUns64S], ci, t[MAX_ppUns64S];
    ppInt32 i;
    ppUns32 ciBits, j, s;

    LargeNumberAssign (bPower[0], b, dDigits);
    LargeNumberModMult (bPower[1], bPower[0], b, d, dDigits);
    LargeNumberModMult (bPower[2], bPower[1], b, d, dDigits);

    LargeNumberASSIGN_DIGIT (t, 1, dDigits);

    cDigits = ppUns64s (c, cDigits);
    for (i = cDigits - 1; i >= 0; i--) {
        ci = c[i];
        ciBits = ppUns64_BITS;

        if (i == (ppInt32)(cDigits - 1)) {
            while (! DIGIT_2MSB (ci)) {
                ci <<= 2;
                ciBits -= 2;
            }
        }

        for (j = 0; j < ciBits; j += 2, ci <<= 2) {
            LargeNumberModMult (t, t, t, d, dDigits);
            LargeNumberModMult (t, t, t, d, dDigits);
            if ((s = DIGIT_2MSB (ci)) != 0)
                LargeNumberModMult (t, t, bPower[s-1], d, dDigits);
        }
    }

    LargeNumberAssign (a, t, dDigits);

    memset ((ppUns8 * )bPower, 0, sizeof (bPower));
    memset ((ppUns8 * )t, 0, sizeof (t));
}

void LargeNumberModInv ( ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 digits)
{
    ppUns32 q[MAX_ppUns64S], t1[MAX_ppUns64S], t3[MAX_ppUns64S],
        u1[MAX_ppUns64S], u3[MAX_ppUns64S], v1[MAX_ppUns64S],
        v3[MAX_ppUns64S], w[2*MAX_ppUns64S];
    ppInt32 u1Sign;
    LargeNumberASSIGN_DIGIT (u1, 1, digits);
    LargeNumberAssignZero (v1, digits);
    LargeNumberAssign (u3, b, digits);
    LargeNumberAssign (v3, c, digits);
    u1Sign = 1;

    while (! LargeNumberZero (v3, digits)) {
        LargeNumberDiv (q, t3, u3, digits, v3, digits);
        LargeNumberMult (w, q, v1, digits);
        LargeNumberAdd (t1, u1, w, digits);
        LargeNumberAssign (u1, v1, digits);
        LargeNumberAssign (v1, t1, digits);
        LargeNumberAssign (u3, v3, digits);
        LargeNumberAssign (v3, t3, digits);
        u1Sign = -u1Sign;
    }

    if (u1Sign < 0)
        LargeNumberSub (a, c, u1, digits);
    else
        LargeNumberAssign (a, u1, digits);

    memset ((ppUns8 * )q, 0, sizeof (q));
    memset ((ppUns8 * )t1, 0, sizeof (t1));
    memset ((ppUns8 * )t3, 0, sizeof (t3));
    memset ((ppUns8 * )u1, 0, sizeof (u1));
    memset ((ppUns8 * )u3, 0, sizeof (u3));
    memset ((ppUns8 * )v1, 0, sizeof (v1));
    memset ((ppUns8 * )v3, 0, sizeof (v3));
    memset ((ppUns8 * )w, 0, sizeof (w));
}

void LargeNumberGcd (ppUns32 *a, ppUns32 *b, ppUns32 *c, ppUns32 digits)
{
    ppUns32 t[MAX_ppUns64S], u[MAX_ppUns64S], v[MAX_ppUns64S];

    LargeNumberAssign (u, b, digits);
    LargeNumberAssign (v, c, digits);

    while (! LargeNumberZero (v, digits)) {
        LargeNumberMod (t, u, digits, v, digits);
        LargeNumberAssign (u, v, digits);
        LargeNumberAssign (v, t, digits);
    }

    LargeNumberAssign (a, u, digits);

    memset ((ppUns8 * )t, 0, sizeof (t));
    memset ((ppUns8 * )u, 0, sizeof (u));
    memset ((ppUns8 * )v, 0, sizeof (v));
}

ppInt32 LargeNumberCmp (ppUns32 *a, ppUns32 *b, ppUns32 digits)
{
    ppInt32 i;

    for (i = digits - 1; i >= 0; i--) {
        if (a[i] > b[i])
            return (1);
        if (a[i] < b[i])
            return (-1);
    }
    return (0);
}

ppInt32 LargeNumberZero (ppUns32 *a, ppUns32 digits)
{
    ppUns32 i;

    for (i = 0; i < digits; i++)
        if (a[i])
            return (0);

    return (1);
}

ppUns32 LargeNumberBits (ppUns32 *a, ppUns32 digits)
{
    if ((digits = ppUns64s (a, digits)) == 0)
        return (0);

    return ((digits - 1) * ppUns64_BITS + ppUns64Bits (a[digits-1]));
}

ppUns32 ppUns64s (ppUns32 *a, ppUns32 digits)
{
    ppInt32 i;

    for (i = digits - 1; i >= 0; i--)
        if (a[i])
            break;

    return (i + 1);
}

static ppUns32 LargeNumberAddDigitMult (ppUns32 *a, ppUns32 *b, ppUns32 c, ppUns32 *d, ppUns32 digits)
{
    ppUns32 carry, t[2];
    ppUns32 i;

    if (c == 0)
        return (0);

    carry = 0;
    for (i = 0; i < digits; i++) {
        ppUns64Mult (t, c, d[i]);
        if ((a[i] = b[i] + carry) < carry)
            carry = 1;
        else
            carry = 0;
        if ((a[i] += t[0]) < t[0])
            carry++;
        carry += t[1];
    }
    return (carry);
}

static ppUns32 LargeNumberSubDigitMult (ppUns32 *a, ppUns32 *b, ppUns32 c, ppUns32 *d, ppUns32 digits)
{
    ppUns32 borrow, t[2];
    ppUns32 i;

    if (c == 0)
        return (0);

    borrow = 0;
    for (i = 0; i < digits; i++) {
        ppUns64Mult (t, c, d[i]);
        if ((a[i] = b[i] - borrow) > (MAX_ppUns64 - borrow))
            borrow = 1;
        else
            borrow = 0;
        if ((a[i] -= t[0]) > (MAX_ppUns64 - t[0]))
            borrow++;
        borrow += t[1];
    }

    return (borrow);
}

static ppUns32 ppUns64Bits (ppUns32 a)
{
    ppUns32 i;

    for (i = 0; i < ppUns64_BITS; i++, a >>= 1)
        if (a == 0)
            break;

    return (i);
}


rsa_Key *rsa_CreateKey(PDFLibHandle lib)
{
    rsa_Key *rslt;
    rslt = (rsa_Key * )mmalloc(lib, sizeof(rsa_Key));
    rslt->version = NULL;
    rslt->modulus = NULL;
    rslt->publExp = NULL;
    rslt->exponent = NULL;
    rslt->prime[0] = NULL;
    rslt->prime[1] = NULL;
    rslt->prExp[0] = NULL;
    rslt->prExp[1] = NULL;
    rslt->coeff = NULL;
    return rslt;
}

void rsa_DisposeKey(PDFLibHandle lib, rsa_Key *key)
{
    if (key == NULL) return;
    asn1_DisposeInteger(lib, key->version);
    asn1_DisposeInteger(lib, key->modulus);
    asn1_DisposeInteger(lib, key->publExp);
    asn1_DisposeInteger(lib, key->exponent);
    asn1_DisposeInteger(lib, key->prime[0]);
    asn1_DisposeInteger(lib, key->prime[1]);
    asn1_DisposeInteger(lib, key->prExp[0]);
    asn1_DisposeInteger(lib, key->prExp[1]);
    asn1_DisposeInteger(lib, key->coeff);
    mfree(lib, key);
}

ppInt32 rsa_KeySize(rsa_Key *key)
{
    return key->modulus->length;
}


ppInt32 rsa_EncodePrivateKey(rsa_Key *rsa, ppUns8 **pbuf)
{
    ppInt32 r, rslt = 0;
    ppUns8 *p;
    if (rsa == NULL) return 0;
    rslt += asn1_EncodeInteger(rsa->version, NULL);
    rslt += asn1_EncodeInteger(rsa->modulus, NULL);
    rslt += asn1_EncodeInteger(rsa->publExp, NULL);
    rslt += asn1_EncodeInteger(rsa->exponent, NULL);
    rslt += asn1_EncodeInteger(rsa->prime[0], NULL);
    rslt += asn1_EncodeInteger(rsa->prime[1], NULL);
    rslt += asn1_EncodeInteger(rsa->prExp[0], NULL);
    rslt += asn1_EncodeInteger(rsa->prExp[1], NULL);
    rslt += asn1_EncodeInteger(rsa->coeff, NULL);
    r = asn1_ObjectSize(1, rslt, SEQUENCE_TAG);
    if (pbuf == NULL) return r;

    p = *pbuf;
    asn1_PutObject(&p, 1, rslt, SEQUENCE_TAG, UNIVERSAL_CLS);
    asn1_EncodeInteger(rsa->version, &p);
    asn1_EncodeInteger(rsa->modulus, &p);
    asn1_EncodeInteger(rsa->publExp, &p);
    asn1_EncodeInteger(rsa->exponent, &p);
    asn1_EncodeInteger(rsa->prime[0], &p);
    asn1_EncodeInteger(rsa->prime[1], &p);
    asn1_EncodeInteger(rsa->prExp[0], &p);
    asn1_EncodeInteger(rsa->prExp[1], &p);
    asn1_EncodeInteger(rsa->coeff, &p);
    (*pbuf) = p;
    return r;
}

rsa_Key *rsa_DecodePrivateKey(PDFLibHandle lib, rsa_Key **rsa, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context c;
    rsa_Key *rslt = NULL;

    PDFTRY(lib){
        c.pp = pbuf;
        c.q = (*pbuf);

        if (rsa == NULL || (*rsa) == NULL)
            rslt = rsa_CreateKey(lib); 
        else
            rslt = (*rsa);

        c.p = *pbuf;
        c.max = ! max ? NULL : c.p + (ppInt32)max;
        asn1_GetSequence(lib, &c, &max);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->version), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);
        if (rslt->version->length == 0) asn1_SetInteger(lib, rslt->version, 0);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->modulus), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->publExp), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->exponent), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->prime[0]), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->prime[1]), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->prExp[0]), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->prExp[1]), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->coeff), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        asn1_Finish(lib, &c);
        (*pbuf) = c.p;
        if (rsa != NULL) (*rsa) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (rsa == NULL || (*rsa) != rslt))
        {
            rsa_DisposeKey(lib, rslt);
            rslt = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return rslt;
}

rsa_Key * rsa_UnShroudKey( PDFLibHandle lib, pkcs8_PKI *p8 )
{
    rsa_Key *key = NULL;
    ppUns8 *p;
    ppInt32 l;

    PDFTRY(lib){
        if (asn1_GetObjectID(p8->pkeyalg->algorithm) != ID_rsaEncryption)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleNotImplementedError));
        p = p8->pkey->data;
        l = p8->pkey->length;
        key = rsa_DecodePrivateKey(lib, NULL, &p, l);
    } PDFEXCEPT(lib){ 
        if (key != NULL)
        {
            rsa_DisposeKey(lib, key);
            key = NULL;
        }
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    return key;
}
rsa_Key *rsa_DecodePublicKey(PDFLibHandle lib, rsa_Key **rsa, ppUns8 **pbuf, ppInt64 max)
{
    asn1_Context c;
    rsa_Key *rslt = NULL;

    PDFTRY(lib) {
        c.pp = pbuf;
        c.q = (*pbuf);
        if (rsa == NULL || (*rsa) == NULL)
            rslt = rsa_CreateKey(lib); 
        else
            rslt = (*rsa);

        c.p = (*pbuf);
        c.max = ! max ? NULL : c.p + (ppInt32)max;
        asn1_GetSequence(lib, &c, &max);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->modulus), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        c.q = c.p;
        asn1_DecodeInteger(lib, &(rslt->publExp), &c.p, c.len);
        c.len -= (ppInt64)(c.p - c.q);

        asn1_Finish(lib, &c);
        (*pbuf) = c.p;
        if (rsa != NULL) (*rsa) = rslt;
    } PDFEXCEPT(lib){
        if (rslt != NULL && (rsa == NULL || (*rsa) != rslt)) {
            rsa_DisposeKey(lib, rslt); rslt = NULL;
        }
        PDFRERAISE(lib);
    }
    PDFTRYEND(lib);
    return rslt;
}

void RSASetPrivateKey(rsa_Key *from, RSA_PRIVATE_KEY *to)
{
    ppUns8 t;
    ppInt32  i, p;
    to->bits = 8*(from->modulus->length - 1);
    t = from->modulus->data[0]; 
    while (t) { to->bits++; t >>= 1; };

    p = MAX_RSA_MODULUS_LEN - from->modulus->length;
    for (i = 0; i < p; i++) to->modulus[i] = 0;
    memcpy(&(to->modulus[p]), from->modulus->data, from->modulus->length);

    p = MAX_RSA_MODULUS_LEN - from->publExp->length;
    for (i = 0; i < p; i++) to->publicExponent[i] = 0;
    memcpy(&(to->publicExponent[p]), from->publExp->data, from->publExp->length);

    p = MAX_RSA_MODULUS_LEN - from->exponent->length;
    for (i = 0; i < p; i++) to->exponent[i] = 0;
    memcpy(&(to->exponent[p]), from->exponent->data, from->exponent->length);

    p = MAX_RSA_PRIME_LEN - from->prime[0]->length;
    for (i = 0; i < p; i++) to->prime[0][i] = 0;
    memcpy(&(to->prime[0][p]), from->prime[0]->data, from->prime[0]->length);

    p = MAX_RSA_PRIME_LEN - from->prime[1]->length;
    for (i = 0; i < p; i++) to->prime[1][i] = 0;
    memcpy(&(to->prime[1][p]), from->prime[1]->data, from->prime[1]->length);

    p = MAX_RSA_PRIME_LEN - from->prExp[0]->length;
    for (i = 0; i < p; i++) to->primeExponent[0][i] = 0;
    memcpy(&(to->primeExponent[0][p]), from->prExp[0]->data, from->prExp[0]->length);

    p = MAX_RSA_PRIME_LEN - from->prExp[1]->length;
    for (i = 0; i < p; i++) to->primeExponent[1][i] = 0;
    memcpy(&(to->primeExponent[1][p]), from->prExp[1]->data, from->prExp[1]->length);

    p = MAX_RSA_PRIME_LEN - from->coeff->length;
    for (i = 0; i < p; i++) to->coefficient[i] = 0;
    memcpy(&(to->coefficient[p]), from->coeff->data, from->coeff->length);
}

void rsa_PrivateEncrypt(PDFLibHandle lib, ppUns8 *obuf, ppInt32 *olen, ppUns8 *ibuf, ppInt32 ilen, rsa_Key *rsa, ppInt32 padding)
{
    RSA_PRIVATE_KEY key;
    PDFTRY(lib){
        RSASetPrivateKey(rsa, &key);
        if (RSAPrivateEncrypt(obuf, (ppUns32 *)olen, ibuf, (ppUns32)ilen, &key)) 
        {
            *olen = -1;
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, gleOtherError));
        }
        memset(&key, 0, sizeof(key));
    } PDFEXCEPT(lib){
        memset(&key, 0, sizeof(key));
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
    if ( padding == 0 )
        return;
}


void rsa_Sign(PDFLibHandle lib, ppInt32 type, ppUns8 *mbuf, ppInt32 mlen, ppUns8 *sbuf, ppInt32 *slen, rsa_Key *rsa)
{
    ppInt32 rlen = 0, ilen;
    ppUns8 *p, *ibuf = NULL;
    pkcs7_DI di;
    asn1_Type parameter;
    x509_Algorithm algorithm;
    asn1_OctetString digest;
    asn1_Object *obj = NULL;

    PDFTRY(lib){
        obj = asn1_CreateObjectByID(lib, type);
        if (obj == NULL || obj->length == 0)
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleInvalidObjError));
        algorithm.algorithm = obj;
        parameter.type = NULL_TAG;
        parameter.value.ptr = NULL;
        algorithm.parameter = &parameter;
        di.digestAlgorithm = &algorithm;

        digest.data = mbuf;
        digest.length = mlen;
        di.digest= &digest;

        ilen = pkcs7_EncodeDigestInfo(&di, NULL);
        rlen = rsa_KeySize(rsa);

        if (ilen > (rlen - 11)) 
            PDFRAISE(lib, PDFBuildErrCode(ErrSignLevel, sleOtherError));
        ibuf = (ppUns8 * )mmalloc(lib, rlen + 1);

        p = ibuf;
        pkcs7_EncodeDigestInfo(&di, &p);
        rsa_PrivateEncrypt(lib, sbuf, slen, ibuf, ilen, rsa, 1);
        if (*slen <= 0) *slen = 0;
        if (ibuf != NULL)
        {
            memset(ibuf, 0, rlen + 1);
            mfree(lib, ibuf);
        }
        asn1_DisposeObject(lib, obj);
    } PDFEXCEPT(lib){
        if (ibuf != NULL)
        {
            memset(ibuf, 0, rlen + 1);
            mfree(lib, ibuf);
        }
        if (obj != NULL)
            asn1_DisposeObject(lib, obj);
        PDFRERAISE(lib);
    } PDFTRYEND(lib);
}

static ppInt32 RSAPrivateBlock (ppUns8 *output, ppUns32 *outputLen, ppUns8 *input, ppUns32 inputLen, RSA_PRIVATE_KEY *privateKey)
{
	ppUns32 c[MAX_ppUns64S], cP[MAX_ppUns64S], cQ[MAX_ppUns64S],
		dP[MAX_ppUns64S], dQ[MAX_ppUns64S], mP[MAX_ppUns64S],
		mQ[MAX_ppUns64S], n[MAX_ppUns64S], p[MAX_ppUns64S], q[MAX_ppUns64S],
		qInv[MAX_ppUns64S], t[MAX_ppUns64S];
	ppUns32 cDigits, nDigits, pDigits;

	LargeNumberDecode (c, MAX_ppUns64S, input, inputLen);
	LargeNumberDecode (n, MAX_ppUns64S, privateKey->modulus, MAX_RSA_MODULUS_LEN);
	LargeNumberDecode (p, MAX_ppUns64S, privateKey->prime[0], MAX_RSA_PRIME_LEN);
	LargeNumberDecode (q, MAX_ppUns64S, privateKey->prime[1], MAX_RSA_PRIME_LEN);
	LargeNumberDecode 
		(dP, MAX_ppUns64S, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
	LargeNumberDecode 
		(dQ, MAX_ppUns64S, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
	LargeNumberDecode (qInv, MAX_ppUns64S, privateKey->coefficient, MAX_RSA_PRIME_LEN);
	cDigits = ppUns64s (c, MAX_ppUns64S);
	nDigits = ppUns64s (n, MAX_ppUns64S);
	pDigits = ppUns64s (p, MAX_ppUns64S);

	if (LargeNumberCmp (c, n, nDigits) >= 0)
		return (-1);

	LargeNumberMod (cP, c, cDigits, p, pDigits);
	LargeNumberMod (cQ, c, cDigits, q, pDigits);
	LargeNumberModExp (mP, cP, dP, pDigits, p, pDigits);
	LargeNumberAssignZero (mQ, nDigits);
	LargeNumberModExp (mQ, cQ, dQ, pDigits, q, pDigits);

	if (LargeNumberCmp (mP, mQ, pDigits) >= 0)
		LargeNumberSub (t, mP, mQ, pDigits);
	else {
		LargeNumberSub (t, mQ, mP, pDigits);
		LargeNumberSub (t, p, t, pDigits);
	}
	LargeNumberModMult (t, t, qInv, p, pDigits);
	LargeNumberMult (t, t, q, pDigits);
	LargeNumberAdd (t, t, mQ, nDigits);

	* outputLen = (privateKey->bits + 7) / 8;
	LargeNumberEncode (output, *outputLen, t, nDigits);

	memset ((ppUns8 * )c, 0, sizeof (c));
	memset ((ppUns8 * )cP, 0, sizeof (cP));
	memset ((ppUns8 * )cQ, 0, sizeof (cQ));
	memset ((ppUns8 * )dP, 0, sizeof (dP));
	memset ((ppUns8 * )dQ, 0, sizeof (dQ));
	memset ((ppUns8 * )mP, 0, sizeof (mP));
	memset ((ppUns8 * )mQ, 0, sizeof (mQ));
	memset ((ppUns8 * )p, 0, sizeof (p));
	memset ((ppUns8 * )q, 0, sizeof (q));
	memset ((ppUns8 * )qInv, 0, sizeof (qInv));
	memset ((ppUns8 * )t, 0, sizeof (t));

	return (0);
}

ppInt32 RSAPrivateEncrypt (ppUns8 *output, ppUns32 *outputLen, ppUns8 *input, ppUns32 inputLen, RSA_PRIVATE_KEY *privateKey)
{
    ppInt32 status;
    ppUns8 pkcsBlock[MAX_RSA_MODULUS_LEN];
    ppUns32 i, modulusLen;

    modulusLen = (privateKey->bits + 7) / 8;
    if (inputLen + 11 > modulusLen)
        return (-1);

    pkcsBlock[0] = 0;
    pkcsBlock[1] = 1;

    for (i = 2; i < modulusLen - inputLen - 1; i++)
        pkcsBlock[i] = 0xff;

    pkcsBlock[i++] = 0;

    memcpy ((ppUns8 * )&pkcsBlock[i], (ppUns8 * )input, inputLen);

    status = RSAPrivateBlock
        (output, outputLen, pkcsBlock, modulusLen, privateKey);

    memset ((ppUns8 * )pkcsBlock, 0, sizeof (pkcsBlock));

    return (status);
}


#endif