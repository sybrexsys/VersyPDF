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
VSFontA.c
*********************************************************************/

#include <string.h>

#include "VSFontAI.h"
#include "VSLibI.h"
#include "VSAssert.h"
#include "VSDocAI.h"
#include "VSCosAI.h"
#include "VSMisc.h"
#include "VSTtf.h" 
#include "VSFilter.h"
#include "VSStdFont.h"
#include "VSType1.h"
#include <ctype.h>
#ifdef WINDOWS_PLATFORM
#include <windows.h>
#endif

#ifdef _LIB
#undef _LIB
#endif

#define _DOC CD(Doc)
#define _LIB _DOC->Lib



static  ppUns16 PDFEncodeToPDFChar[][256]   ={
        {/* PDF Encode */
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_breve , 
            PDFChar_caron , PDFChar_circumflex , PDFChar_dotaccent , PDFChar_hungarumlaut , 
            PDFChar_ogonek , PDFChar_ring , PDFChar_tilde , PDFChar_space, PDFChar_exclam , 
            PDFChar_quotedbl , PDFChar_numbersign , PDFChar_dollar , PDFChar_percent , 
            PDFChar_ampersand , PDFChar_quotesingle , PDFChar_parenleft , PDFChar_parenright , 
            PDFChar_asterisk , PDFChar_plus , PDFChar_comma , PDFChar_hyphen, PDFChar_period , 
            PDFChar_slash , PDFChar_zero , PDFChar_one , PDFChar_two , PDFChar_three , 
            PDFChar_four , PDFChar_five , PDFChar_six , PDFChar_seven , PDFChar_eight , 
            PDFChar_nine , PDFChar_colon , PDFChar_semicolon , PDFChar_less , PDFChar_equal , 
            PDFChar_greater , PDFChar_question , PDFChar_at , PDFChar_A , PDFChar_B , PDFChar_C , 
            PDFChar_D , PDFChar_E , PDFChar_F , PDFChar_G , PDFChar_H , PDFChar_I , PDFChar_J , 
            PDFChar_K , PDFChar_L , PDFChar_M , PDFChar_N , PDFChar_O , PDFChar_P , 
            PDFChar_Q , PDFChar_R , PDFChar_S , PDFChar_T , PDFChar_U , PDFChar_V , 
            PDFChar_W , PDFChar_X , PDFChar_Y , PDFChar_Z , PDFChar_bracketleft , 
            PDFChar_backslash , PDFChar_bracketright , PDFChar_asciicircum , PDFChar_underscore , 
            PDFChar_grave , PDFChar_a , PDFChar_b , PDFChar_c , PDFChar_d , PDFChar_e , 
            PDFChar_f , PDFChar_g , PDFChar_h , PDFChar_i , PDFChar_j , PDFChar_k , PDFChar_l , 
            PDFChar_m , PDFChar_n , PDFChar_o , PDFChar_p , PDFChar_q , PDFChar_r , PDFChar_s , 
            PDFChar_t , PDFChar_u , PDFChar_v , PDFChar_w , PDFChar_x , PDFChar_y, PDFChar_z , 
            PDFChar_braceleft , PDFChar_bar , PDFChar_braceright , PDFChar_asciitilde , 
            PDFChar_NULL, PDFChar_bullet, PDFChar_dagger , PDFChar_daggerdbl , PDFChar_ellipsis , 
            PDFChar_emdash , PDFChar_endash , PDFChar_florin , PDFChar_fraction , 
            PDFChar_guilsinglleft , PDFChar_guilsinglright , PDFChar_minus , PDFChar_perthousand , 
            PDFChar_quotedblbase , PDFChar_quotedblleft , PDFChar_quotedblright , PDFChar_quoteleft , 
            PDFChar_quoteright , PDFChar_quotesinglbase , PDFChar_trademark , PDFChar_fi , 
            PDFChar_fl , PDFChar_Lslash , PDFChar_OE , PDFChar_Scaron , PDFChar_Ydieresis , 
            PDFChar_Zcaron, PDFChar_dotlessi , PDFChar_lslash , PDFChar_oe , PDFChar_scaron , 
            PDFChar_zcaron, PDFChar_NULL, PDFChar_Euro, PDFChar_exclamdown , PDFChar_cent , 
            PDFChar_sterling , PDFChar_currency, PDFChar_yen , PDFChar_brokenbar , 
            PDFChar_section , PDFChar_dieresis , PDFChar_copyright , PDFChar_ordfeminine , 
            PDFChar_guillemotleft, PDFChar_logicalnot , PDFChar_NULL, PDFChar_registered ,
            PDFChar_macron , PDFChar_degree , PDFChar_plusminus , PDFChar_twosuperior , 
            PDFChar_threesuperior , PDFChar_acute , PDFChar_mu , PDFChar_paragraph ,
            PDFChar_periodcentered , PDFChar_cedilla , PDFChar_onesuperior , PDFChar_ordmasculine , 
            PDFChar_guillemotright, PDFChar_onequarter , PDFChar_onehalf , PDFChar_threequarters , 
            PDFChar_questiondown , PDFChar_Agrave , PDFChar_Aacute , PDFChar_Acircumflex , 
            PDFChar_Atilde , PDFChar_Adieresis , PDFChar_Aring , PDFChar_AE , PDFChar_Ccedilla , 
            PDFChar_Egrave , PDFChar_Eacute , PDFChar_Ecircumflex , PDFChar_Edieresis , 
            PDFChar_Igrave , PDFChar_Iacute , PDFChar_Icircumflex , PDFChar_Idieresis , 
            PDFChar_Eth , PDFChar_Ntilde , PDFChar_Ograve , PDFChar_Oacute , 
            PDFChar_Ocircumflex , PDFChar_Otilde , PDFChar_Odieresis , PDFChar_multiply , 
            PDFChar_Oslash , PDFChar_Ugrave , PDFChar_Uacute , PDFChar_Ucircumflex , 
            PDFChar_Udieresis , PDFChar_Yacute , PDFChar_Thorn , PDFChar_germandbls , 
            PDFChar_agrave , PDFChar_aacute , PDFChar_acircumflex , PDFChar_atilde , 
            PDFChar_adieresis , PDFChar_aring , PDFChar_ae , PDFChar_ccedilla , PDFChar_egrave , 
            PDFChar_eacute , PDFChar_ecircumflex , PDFChar_edieresis , PDFChar_igrave , 
            PDFChar_iacute , PDFChar_icircumflex , PDFChar_idieresis , PDFChar_eth , PDFChar_ntilde , 
            PDFChar_ograve , PDFChar_oacute , PDFChar_ocircumflex , PDFChar_otilde , 
            PDFChar_odieresis , PDFChar_divide , PDFChar_oslash , PDFChar_ugrave , PDFChar_uacute , 
            PDFChar_ucircumflex , PDFChar_udieresis , PDFChar_yacute, PDFChar_thorn , PDFChar_ydieresis
        }, 
        {/* Win Encoding*/
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_space, PDFChar_exclam  , PDFChar_quotedbl  , 
            PDFChar_numbersign  , PDFChar_dollar  , PDFChar_percent  , PDFChar_ampersand  , 
            PDFChar_quotesingle  , PDFChar_parenleft  , PDFChar_parenright  , PDFChar_asterisk  , 
            PDFChar_plus  , PDFChar_comma  , PDFChar_hyphen, PDFChar_period  , PDFChar_slash  , 
            PDFChar_zero  , PDFChar_one  , PDFChar_two  , PDFChar_three  , PDFChar_four  , 
            PDFChar_five  , PDFChar_six  , PDFChar_seven  , PDFChar_eight  , PDFChar_nine  , 
            PDFChar_colon  , PDFChar_semicolon  , PDFChar_less  , PDFChar_equal  , PDFChar_greater  , 
            PDFChar_question  , PDFChar_at  , PDFChar_A  , PDFChar_B  , PDFChar_C  , PDFChar_D  , 
            PDFChar_E  , PDFChar_F  , PDFChar_G  , PDFChar_H  , PDFChar_I  , PDFChar_J  , 
            PDFChar_K  , PDFChar_L  , PDFChar_M  , PDFChar_N  , PDFChar_O  , PDFChar_P  , 
            PDFChar_Q  , PDFChar_R  , PDFChar_S  , PDFChar_T  , PDFChar_U  , PDFChar_V  , 
            PDFChar_W  , PDFChar_X  , PDFChar_Y  , PDFChar_Z  , PDFChar_bracketleft  , 
            PDFChar_backslash  , PDFChar_bracketright  , PDFChar_asciicircum  , PDFChar_underscore  , 
            PDFChar_grave  , PDFChar_a  , PDFChar_b  , PDFChar_c  , PDFChar_d  , PDFChar_e  , 
            PDFChar_f  , PDFChar_g  , PDFChar_h  , PDFChar_i  , PDFChar_j  , PDFChar_k  , 
            PDFChar_l  , PDFChar_m  , PDFChar_n  , PDFChar_o  , PDFChar_p  , PDFChar_q  , 
            PDFChar_r  , PDFChar_s  , PDFChar_t  , PDFChar_u  , PDFChar_v  , PDFChar_w  , 
            PDFChar_x  , PDFChar_y, PDFChar_z  , PDFChar_braceleft  , PDFChar_bar  , 
            PDFChar_braceright  , PDFChar_asciitilde  , PDFChar_NULL, PDFChar_Euro, 
            PDFChar_NULL, PDFChar_quotesinglbase  , PDFChar_florin  , PDFChar_quotedblbase  , 
            PDFChar_ellipsis  , PDFChar_dagger  , PDFChar_daggerdbl  , PDFChar_circumflex  , 
            PDFChar_perthousand  , PDFChar_Scaron , PDFChar_guilsinglleft  , PDFChar_OE  , 
            PDFChar_NULL, PDFChar_Zcaron, PDFChar_NULL, PDFChar_NULL, PDFChar_quoteleft  , 
            PDFChar_quoteright  , PDFChar_quotedblleft  , PDFChar_quotedblright  , PDFChar_bullet, 
            PDFChar_endash  , PDFChar_emdash  , PDFChar_tilde  , PDFChar_trademark , 
            PDFChar_scaron , PDFChar_guilsinglright  , PDFChar_oe  , PDFChar_NULL, PDFChar_zcaron, 
            PDFChar_Ydieresis , PDFChar_space, PDFChar_exclamdown  , PDFChar_cent  , 
            PDFChar_sterling  , PDFChar_currency, PDFChar_yen  , PDFChar_brokenbar , 
            PDFChar_section  , PDFChar_dieresis  , PDFChar_copyright , PDFChar_ordfeminine  , 
            PDFChar_guillemotleft, PDFChar_logicalnot , PDFChar_hyphen, PDFChar_registered , 
            PDFChar_macron  , PDFChar_degree , PDFChar_plusminus , PDFChar_twosuperior , 
            PDFChar_threesuperior , PDFChar_acute  , PDFChar_mu , PDFChar_paragraph  , 
            PDFChar_periodcentered  , PDFChar_cedilla  , PDFChar_onesuperior , 
            PDFChar_ordmasculine  , PDFChar_guillemotright, PDFChar_onequarter , PDFChar_onehalf , 
            PDFChar_threequarters , PDFChar_questiondown  , PDFChar_Agrave , PDFChar_Aacute , 
            PDFChar_Acircumflex , PDFChar_Atilde , PDFChar_Adieresis , PDFChar_Aring , 
            PDFChar_AE  , PDFChar_Ccedilla , PDFChar_Egrave , PDFChar_Eacute , 
            PDFChar_Ecircumflex , PDFChar_Edieresis , PDFChar_Igrave , PDFChar_Iacute , 
            PDFChar_Icircumflex , PDFChar_Idieresis , PDFChar_Eth , PDFChar_Ntilde , 
            PDFChar_Ograve , PDFChar_Oacute , PDFChar_Ocircumflex , PDFChar_Otilde , 
            PDFChar_Odieresis , PDFChar_multiply , PDFChar_Oslash  , PDFChar_Ugrave , 
            PDFChar_Uacute , PDFChar_Ucircumflex , PDFChar_Udieresis , PDFChar_Yacute , 
            PDFChar_Thorn , PDFChar_germandbls  , PDFChar_agrave , PDFChar_aacute , 
            PDFChar_acircumflex , PDFChar_atilde , PDFChar_adieresis , PDFChar_aring , 
            PDFChar_ae  , PDFChar_ccedilla , PDFChar_egrave , PDFChar_eacute , PDFChar_ecircumflex , 
            PDFChar_edieresis , PDFChar_igrave , PDFChar_iacute , PDFChar_icircumflex , 
            PDFChar_idieresis , PDFChar_eth , PDFChar_ntilde , PDFChar_ograve , PDFChar_oacute , 
            PDFChar_ocircumflex , PDFChar_otilde , PDFChar_odieresis , PDFChar_divide , 
            PDFChar_oslash  , PDFChar_ugrave , PDFChar_uacute , PDFChar_ucircumflex , 
            PDFChar_udieresis , PDFChar_yacute, PDFChar_thorn , PDFChar_ydieresis 
        }, 
        { /* Mac Encoding*/
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_space,PDFChar_exclam,PDFChar_quotedbl,PDFChar_numbersign,
            PDFChar_dollar,PDFChar_percent,PDFChar_ampersand,PDFChar_quotesingle,
            PDFChar_parenleft,PDFChar_parenright,PDFChar_asterisk,PDFChar_plus,PDFChar_comma,
            PDFChar_hyphen,PDFChar_period,PDFChar_slash,PDFChar_zero,PDFChar_one,
            PDFChar_two,PDFChar_three,PDFChar_four,PDFChar_five,PDFChar_six,PDFChar_seven,
            PDFChar_eight,PDFChar_nine,PDFChar_colon,PDFChar_semicolon,PDFChar_less,
            PDFChar_equal,PDFChar_greater,PDFChar_question,PDFChar_at,PDFChar_A,PDFChar_B,
            PDFChar_C,PDFChar_D,PDFChar_E,PDFChar_F,PDFChar_G,PDFChar_H,PDFChar_I,
            PDFChar_J,PDFChar_K,PDFChar_L,PDFChar_M,PDFChar_N,PDFChar_O,PDFChar_P,
            PDFChar_Q,PDFChar_R,PDFChar_S,PDFChar_T,PDFChar_U,PDFChar_V,PDFChar_W,
            PDFChar_X,PDFChar_Y,PDFChar_Z,PDFChar_bracketleft,PDFChar_backslash,
            PDFChar_bracketright,PDFChar_asciicircum,PDFChar_underscore,PDFChar_grave,
            PDFChar_a,PDFChar_b,PDFChar_c,PDFChar_d,PDFChar_e,PDFChar_f,PDFChar_g,
            PDFChar_h,PDFChar_i,PDFChar_j,PDFChar_k,PDFChar_l,PDFChar_m,PDFChar_n,
            PDFChar_o,PDFChar_p,PDFChar_q,PDFChar_r,PDFChar_s,PDFChar_t,PDFChar_u,
            PDFChar_v,PDFChar_w,PDFChar_x,PDFChar_y,PDFChar_z,PDFChar_braceleft,
            PDFChar_bar,PDFChar_braceright,PDFChar_asciitilde,PDFChar_NULL,PDFChar_Adieresis,
            PDFChar_Aring,PDFChar_Ccedilla,PDFChar_Eacute,PDFChar_Ntilde,PDFChar_Odieresis,
            PDFChar_Udieresis,PDFChar_aacute,PDFChar_agrave,PDFChar_acircumflex,
            PDFChar_adieresis,PDFChar_atilde,PDFChar_aring,PDFChar_ccedilla,PDFChar_eacute,
            PDFChar_egrave,PDFChar_ecircumflex,PDFChar_edieresis,PDFChar_iacute,PDFChar_igrave,
            PDFChar_icircumflex,PDFChar_idieresis,PDFChar_ntilde,PDFChar_oacute,PDFChar_ograve,
            PDFChar_ocircumflex,PDFChar_odieresis,PDFChar_otilde,PDFChar_uacute,PDFChar_ugrave,
            PDFChar_ucircumflex,PDFChar_udieresis,PDFChar_dagger,PDFChar_degree,PDFChar_cent,
            PDFChar_sterling,PDFChar_section,PDFChar_bullet,PDFChar_paragraph,
            PDFChar_germandbls,PDFChar_registered,PDFChar_copyright,PDFChar_trademark,
            PDFChar_acute,PDFChar_dieresis,PDFChar_NULL,PDFChar_AE,PDFChar_Oslash,
            PDFChar_NULL,PDFChar_plusminus,PDFChar_NULL,PDFChar_NULL,PDFChar_yen,
            PDFChar_mu,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,PDFChar_NULL,
            PDFChar_NULL,PDFChar_ordfeminine,PDFChar_ordmasculine,PDFChar_NULL,
            PDFChar_ae,PDFChar_oslash,PDFChar_questiondown,PDFChar_exclamdown,
            PDFChar_logicalnot,PDFChar_NULL,PDFChar_florin,PDFChar_NULL,PDFChar_NULL,
            PDFChar_guillemotleft,PDFChar_guillemotright,PDFChar_ellipsis,PDFChar_space,
            PDFChar_Agrave,PDFChar_Atilde,PDFChar_Otilde,PDFChar_OE,PDFChar_oe,
            PDFChar_endash,PDFChar_emdash,PDFChar_quotedblleft,PDFChar_quotedblright,
            PDFChar_quoteleft,PDFChar_quoteright,PDFChar_divide,PDFChar_NULL,
            PDFChar_ydieresis,PDFChar_Ydieresis,PDFChar_fraction,PDFChar_currency,
            PDFChar_guilsinglleft,PDFChar_guilsinglright,PDFChar_fi,PDFChar_fl,PDFChar_daggerdbl,
            PDFChar_periodcentered,PDFChar_quotesinglbase,PDFChar_quotedblbase,
            PDFChar_perthousand,PDFChar_Acircumflex,PDFChar_Ecircumflex,PDFChar_Aacute,
            PDFChar_Edieresis,PDFChar_Egrave,PDFChar_Iacute,PDFChar_Icircumflex,
            PDFChar_Idieresis,PDFChar_Igrave,PDFChar_Oacute,PDFChar_Ocircumflex,
            PDFChar_NULL,PDFChar_Ograve,PDFChar_Uacute,PDFChar_Ucircumflex,
            PDFChar_Ugrave,PDFChar_dotlessi,PDFChar_circumflex,PDFChar_tilde,
            PDFChar_macron,PDFChar_breve,PDFChar_dotaccent,PDFChar_ring,PDFChar_cedilla,
            PDFChar_hungarumlaut,PDFChar_ogonek,PDFChar_caron
        },
        { /* Standard encoding*/
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_space, PDFChar_exclam  , PDFChar_quotedbl  , 
            PDFChar_numbersign  , PDFChar_dollar  , PDFChar_percent  , PDFChar_ampersand  , 
            PDFChar_quoteright  , PDFChar_parenleft  , PDFChar_parenright  , PDFChar_asterisk  , 
            PDFChar_plus  , PDFChar_comma  , PDFChar_hyphen, PDFChar_period  , PDFChar_slash  , 
            PDFChar_zero  , PDFChar_one  , PDFChar_two  , PDFChar_three  , PDFChar_four  , 
            PDFChar_five  , PDFChar_six  , PDFChar_seven  , PDFChar_eight  , PDFChar_nine  , 
            PDFChar_colon  , PDFChar_semicolon  , PDFChar_less  , PDFChar_equal  , PDFChar_greater  , 
            PDFChar_question  , PDFChar_at  , PDFChar_A  , PDFChar_B  , PDFChar_C  , PDFChar_D  , 
            PDFChar_E  , PDFChar_F  , PDFChar_G  , PDFChar_H  , PDFChar_I  , PDFChar_J  , 
            PDFChar_K  , PDFChar_L  , PDFChar_M  , PDFChar_N  , PDFChar_O  , PDFChar_P  , 
            PDFChar_Q  , PDFChar_R  , PDFChar_S  , PDFChar_T  , PDFChar_U  , PDFChar_V  , 
            PDFChar_W  , PDFChar_X  , PDFChar_Y  , PDFChar_Z  , PDFChar_bracketleft  , 
            PDFChar_backslash  , PDFChar_bracketright  , PDFChar_asciicircum  , PDFChar_underscore  , 
            PDFChar_quoteleft  , PDFChar_a  , PDFChar_b  , PDFChar_c  , PDFChar_d  , PDFChar_e  , 
            PDFChar_f  , PDFChar_g  , PDFChar_h  , PDFChar_i  , PDFChar_j  , PDFChar_k  , 
            PDFChar_l  , PDFChar_m  , PDFChar_n  , PDFChar_o  , PDFChar_p  , PDFChar_q  , 
            PDFChar_r  , PDFChar_s  , PDFChar_t  , PDFChar_u  , PDFChar_v  , PDFChar_w  , 
            PDFChar_x  , PDFChar_y, PDFChar_z  , PDFChar_braceleft  , PDFChar_bar  , 
            PDFChar_braceright  , PDFChar_asciitilde  , PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_exclamdown  , PDFChar_cent  , 
            PDFChar_sterling  , PDFChar_fraction  , PDFChar_yen  , PDFChar_florin  , PDFChar_section  , 
            PDFChar_currency, PDFChar_quotesingle  , PDFChar_quotedblleft  , PDFChar_guillemotleft, 
            PDFChar_guilsinglleft  , PDFChar_guilsinglright  , PDFChar_fi  , PDFChar_fl  , 
            PDFChar_NULL, PDFChar_endash  , PDFChar_dagger  , PDFChar_daggerdbl  , PDFChar_periodcentered  , 
            PDFChar_NULL, PDFChar_paragraph  , PDFChar_bullet, PDFChar_quotesinglbase  , PDFChar_quotedblbase  , 
            PDFChar_quotedblright  , PDFChar_guillemotright, PDFChar_ellipsis  , PDFChar_perthousand  , 
            PDFChar_NULL, PDFChar_questiondown  , PDFChar_NULL, PDFChar_grave  , PDFChar_acute  , 
            PDFChar_circumflex  , PDFChar_tilde  , PDFChar_macron  , PDFChar_breve  , PDFChar_dotaccent  , 
            PDFChar_dieresis  , PDFChar_NULL, PDFChar_ring  , PDFChar_cedilla  , PDFChar_NULL, PDFChar_hungarumlaut  , 
            PDFChar_ogonek  , PDFChar_caron  , PDFChar_emdash  , PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_AE  , PDFChar_NULL, PDFChar_ordfeminine  , 
            PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, PDFChar_Lslash  , PDFChar_Oslash  , 
            PDFChar_OE  , PDFChar_ordmasculine  , PDFChar_NULL, PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL, PDFChar_ae  , PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_dotlessi  , PDFChar_NULL, PDFChar_NULL, PDFChar_lslash  , 
            PDFChar_oslash  , PDFChar_oe  , PDFChar_germandbls  , PDFChar_NULL, PDFChar_NULL, 
            PDFChar_NULL, PDFChar_NULL
        },
        { /* Symbol Encode*/
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_space, 
            PDFSymbolChar_exclam, PDFSymbolChar_universal, PDFSymbolChar_numbersign, 
            PDFSymbolChar_existential, PDFSymbolChar_percent, PDFSymbolChar_ampersand, 
            PDFSymbolChar_suchthat, PDFSymbolChar_parenleft, PDFSymbolChar_parenright, 
            PDFSymbolChar_asteriskmath, PDFSymbolChar_plus, PDFSymbolChar_comma, 
            PDFSymbolChar_minus, PDFSymbolChar_period, PDFSymbolChar_slash, 
            PDFSymbolChar_zero, PDFSymbolChar_one, PDFSymbolChar_two, PDFSymbolChar_three, 
            PDFSymbolChar_four, PDFSymbolChar_five, PDFSymbolChar_six, PDFSymbolChar_seven, 
            PDFSymbolChar_eight, PDFSymbolChar_nine, PDFSymbolChar_colon, 
            PDFSymbolChar_semicolon, PDFSymbolChar_less, PDFSymbolChar_equal, 
            PDFSymbolChar_greater, PDFSymbolChar_question, PDFSymbolChar_congruent, 
            PDFSymbolChar_Alpha, PDFSymbolChar_Beta, PDFSymbolChar_Chi, PDFSymbolChar_Delta, 
            PDFSymbolChar_Epsilon, PDFSymbolChar_Phi, PDFSymbolChar_Gamma, 
            PDFSymbolChar_Eta, PDFSymbolChar_Iota, PDFSymbolChar_theta1, 
            PDFSymbolChar_Kappa, PDFSymbolChar_Lambda, PDFSymbolChar_Mu, 
            PDFSymbolChar_Nu, PDFSymbolChar_Omicron, PDFSymbolChar_Pi, PDFSymbolChar_Theta, 
            PDFSymbolChar_Rho, PDFSymbolChar_Sigma, PDFSymbolChar_Tau, 
            PDFSymbolChar_Upsilon, PDFSymbolChar_sigma1, PDFSymbolChar_Omega, 
            PDFSymbolChar_Xi, PDFSymbolChar_Psi, PDFSymbolChar_Zeta, 
            PDFSymbolChar_bracketleft, PDFSymbolChar_therefore, PDFSymbolChar_bracketright, 
            PDFSymbolChar_perpendicular, PDFSymbolChar_underscore, PDFSymbolChar_radicalex, 
            PDFSymbolChar_alpha, PDFSymbolChar_beta, PDFSymbolChar_chi, PDFSymbolChar_delta, 
            PDFSymbolChar_epsilon, PDFSymbolChar_phi, PDFSymbolChar_gamma, 
            PDFSymbolChar_eta, PDFSymbolChar_iota, PDFSymbolChar_phi1, PDFSymbolChar_kappa, 
            PDFSymbolChar_lambda, PDFSymbolChar_mu, PDFSymbolChar_nu, 
            PDFSymbolChar_omicron, PDFSymbolChar_pi, PDFSymbolChar_theta, PDFSymbolChar_rho, 
            PDFSymbolChar_sigma, PDFSymbolChar_tau, PDFSymbolChar_upsilon, 
            PDFSymbolChar_omega1, PDFSymbolChar_omega, PDFSymbolChar_xi, PDFSymbolChar_psi, 
            PDFSymbolChar_zeta, PDFSymbolChar_braceleft, PDFSymbolChar_bar, 
            PDFSymbolChar_braceright, PDFSymbolChar_similar, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, PDFSymbolChar_NULL, 
            PDFSymbolChar_Euro, PDFSymbolChar_Upsilon1, PDFSymbolChar_minute, PDFSymbolChar_lessequal, 
            PDFSymbolChar_fraction, PDFSymbolChar_infinity, PDFSymbolChar_florin, 
            PDFSymbolChar_club, PDFSymbolChar_diamond, PDFSymbolChar_heart, 
            PDFSymbolChar_spade, PDFSymbolChar_arrowboth, PDFSymbolChar_arrowleft, PDFSymbolChar_arrowup, 
            PDFSymbolChar_arrowright, PDFSymbolChar_arrowdown, 
            PDFSymbolChar_degree, PDFSymbolChar_plusminus, PDFSymbolChar_second, PDFSymbolChar_greaterequal, 
            PDFSymbolChar_multiply, PDFSymbolChar_proportional, PDFSymbolChar_partialdiff, 
            PDFSymbolChar_bullet, PDFSymbolChar_divide, PDFSymbolChar_notequal, PDFSymbolChar_equivalence, 
            PDFSymbolChar_approxequal, PDFSymbolChar_ellipsis, PDFSymbolChar_arrowvertex, 
            PDFSymbolChar_arrowhorizex, PDFSymbolChar_carriagereturn, PDFSymbolChar_aleph, 
            PDFSymbolChar_Ifraktur, PDFSymbolChar_Rfraktur, PDFSymbolChar_weierstrass, 
            PDFSymbolChar_circlemultiply, PDFSymbolChar_circleplus, PDFSymbolChar_emptyset, PDFSymbolChar_intersection, 
            PDFSymbolChar_union, PDFSymbolChar_propersuperset, PDFSymbolChar_reflexsuperset, 
            PDFSymbolChar_notsubset, PDFSymbolChar_propersubset, PDFSymbolChar_reflexsubset, 
            PDFSymbolChar_element, PDFSymbolChar_notelement, PDFSymbolChar_angle, 
            PDFSymbolChar_gradient, PDFSymbolChar_registerserif, PDFSymbolChar_copyrightserif, 
            PDFSymbolChar_trademarkserif, PDFSymbolChar_product, PDFSymbolChar_radical, 
            PDFSymbolChar_dotmath, PDFSymbolChar_logicalnot, PDFSymbolChar_logicaland, 
            PDFSymbolChar_logicalor, PDFSymbolChar_arrowdblboth, PDFSymbolChar_arrowdblleft, 
            PDFSymbolChar_arrowdblup, PDFSymbolChar_arrowdblright, PDFSymbolChar_arrowdbldown, 
            PDFSymbolChar_lozenge, PDFSymbolChar_angleleft, PDFSymbolChar_registersans, 
            PDFSymbolChar_copyrightsans, PDFSymbolChar_trademarksans, PDFSymbolChar_summation, 
            PDFSymbolChar_parenlefttp, PDFSymbolChar_parenleftex, PDFSymbolChar_parenleftbt, 
            PDFSymbolChar_bracketlefttp, PDFSymbolChar_bracketleftex, PDFSymbolChar_bracketleftbt, 
            PDFSymbolChar_bracelefttp, PDFSymbolChar_braceleftmid, PDFSymbolChar_braceleftbt, 
            PDFSymbolChar_braceex, PDFSymbolChar_NULL, PDFSymbolChar_angleright, PDFSymbolChar_integral, 
            PDFSymbolChar_integraltp, PDFSymbolChar_integralex, PDFSymbolChar_integralbt, 
            PDFSymbolChar_parenrighttp, PDFSymbolChar_parenrightex, PDFSymbolChar_parenrightbt, 
            PDFSymbolChar_bracketrighttp, PDFSymbolChar_bracketrightex, 
            PDFSymbolChar_bracketrightbt, PDFSymbolChar_bracerighttp, 
            PDFSymbolChar_bracerightmid, PDFSymbolChar_bracerightbt, PDFSymbolChar_NULL
        },
        {
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_space, 
            PDFZapfDingbatsChar_a1, PDFZapfDingbatsChar_a2, PDFZapfDingbatsChar_a202, PDFZapfDingbatsChar_a3, 
            PDFZapfDingbatsChar_a4, PDFZapfDingbatsChar_a5, PDFZapfDingbatsChar_a119, PDFZapfDingbatsChar_a118, 
            PDFZapfDingbatsChar_a117, PDFZapfDingbatsChar_a11, PDFZapfDingbatsChar_a12, 
            PDFZapfDingbatsChar_a13, PDFZapfDingbatsChar_a14, PDFZapfDingbatsChar_a15, PDFZapfDingbatsChar_a16, 
            PDFZapfDingbatsChar_a105, PDFZapfDingbatsChar_a17, PDFZapfDingbatsChar_a18, 
            PDFZapfDingbatsChar_a19, PDFZapfDingbatsChar_a20, PDFZapfDingbatsChar_a21, PDFZapfDingbatsChar_a22, 
            PDFZapfDingbatsChar_a23, PDFZapfDingbatsChar_a24, PDFZapfDingbatsChar_a25, PDFZapfDingbatsChar_a26, 
            PDFZapfDingbatsChar_a27, PDFZapfDingbatsChar_a28, PDFZapfDingbatsChar_a6, PDFZapfDingbatsChar_a7, 
            PDFZapfDingbatsChar_a8, PDFZapfDingbatsChar_a9, PDFZapfDingbatsChar_a10, PDFZapfDingbatsChar_a29, 
            PDFZapfDingbatsChar_a30, PDFZapfDingbatsChar_a31, PDFZapfDingbatsChar_a32, PDFZapfDingbatsChar_a33, 
            PDFZapfDingbatsChar_a34, PDFZapfDingbatsChar_a35, PDFZapfDingbatsChar_a36, PDFZapfDingbatsChar_a37, 
            PDFZapfDingbatsChar_a38, PDFZapfDingbatsChar_a39, PDFZapfDingbatsChar_a40, PDFZapfDingbatsChar_a41, 
            PDFZapfDingbatsChar_a42, PDFZapfDingbatsChar_a43, PDFZapfDingbatsChar_a44, PDFZapfDingbatsChar_a45, 
            PDFZapfDingbatsChar_a46, PDFZapfDingbatsChar_a47, PDFZapfDingbatsChar_a48, PDFZapfDingbatsChar_a49, 
            PDFZapfDingbatsChar_a50, PDFZapfDingbatsChar_a51, PDFZapfDingbatsChar_a52, PDFZapfDingbatsChar_a53, 
            PDFZapfDingbatsChar_a54, PDFZapfDingbatsChar_a55, PDFZapfDingbatsChar_a56, PDFZapfDingbatsChar_a57, 
            PDFZapfDingbatsChar_a58, PDFZapfDingbatsChar_a59, PDFZapfDingbatsChar_a60, PDFZapfDingbatsChar_a61, 
            PDFZapfDingbatsChar_a62, PDFZapfDingbatsChar_a63, PDFZapfDingbatsChar_a64, PDFZapfDingbatsChar_a65, 
            PDFZapfDingbatsChar_a66, PDFZapfDingbatsChar_a67, PDFZapfDingbatsChar_a68, PDFZapfDingbatsChar_a69, 
            PDFZapfDingbatsChar_a70, PDFZapfDingbatsChar_a71, PDFZapfDingbatsChar_a72, PDFZapfDingbatsChar_a73, 
            PDFZapfDingbatsChar_a74, PDFZapfDingbatsChar_a203, PDFZapfDingbatsChar_a75, 
            PDFZapfDingbatsChar_a204, PDFZapfDingbatsChar_a76, PDFZapfDingbatsChar_a77, 
            PDFZapfDingbatsChar_a78, PDFZapfDingbatsChar_a79, PDFZapfDingbatsChar_a81, PDFZapfDingbatsChar_a82, 
            PDFZapfDingbatsChar_a83, PDFZapfDingbatsChar_a84, PDFZapfDingbatsChar_a97, PDFZapfDingbatsChar_a98, 
            PDFZapfDingbatsChar_a99, PDFZapfDingbatsChar_a100, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_NULL, 
            PDFZapfDingbatsChar_a101, PDFZapfDingbatsChar_a102, PDFZapfDingbatsChar_a103, 
            PDFZapfDingbatsChar_a104, PDFZapfDingbatsChar_a106, PDFZapfDingbatsChar_a107, 
            PDFZapfDingbatsChar_a108, PDFZapfDingbatsChar_a112, PDFZapfDingbatsChar_a111, 
            PDFZapfDingbatsChar_a110, PDFZapfDingbatsChar_a109, PDFZapfDingbatsChar_a120, 
            PDFZapfDingbatsChar_a121, PDFZapfDingbatsChar_a122, PDFZapfDingbatsChar_a123, 
            PDFZapfDingbatsChar_a124, PDFZapfDingbatsChar_a125, PDFZapfDingbatsChar_a126, 
            PDFZapfDingbatsChar_a127, PDFZapfDingbatsChar_a128, PDFZapfDingbatsChar_a129, 
            PDFZapfDingbatsChar_a130, PDFZapfDingbatsChar_a131, PDFZapfDingbatsChar_a132, 
            PDFZapfDingbatsChar_a133, PDFZapfDingbatsChar_a134, PDFZapfDingbatsChar_a135, 
            PDFZapfDingbatsChar_a136, PDFZapfDingbatsChar_a137, PDFZapfDingbatsChar_a138, 
            PDFZapfDingbatsChar_a139, PDFZapfDingbatsChar_a140, PDFZapfDingbatsChar_a141, 
            PDFZapfDingbatsChar_a142, PDFZapfDingbatsChar_a143, PDFZapfDingbatsChar_a144, 
            PDFZapfDingbatsChar_a145, PDFZapfDingbatsChar_a146, PDFZapfDingbatsChar_a147, 
            PDFZapfDingbatsChar_a148, PDFZapfDingbatsChar_a149, PDFZapfDingbatsChar_a150, 
            PDFZapfDingbatsChar_a151, PDFZapfDingbatsChar_a152, PDFZapfDingbatsChar_a153, 
            PDFZapfDingbatsChar_a154, PDFZapfDingbatsChar_a155, PDFZapfDingbatsChar_a156, 
            PDFZapfDingbatsChar_a157, PDFZapfDingbatsChar_a158, PDFZapfDingbatsChar_a159, 
            PDFZapfDingbatsChar_a160, PDFZapfDingbatsChar_a161, PDFZapfDingbatsChar_a163, 
            PDFZapfDingbatsChar_a164, PDFZapfDingbatsChar_a196, PDFZapfDingbatsChar_a165, 
            PDFZapfDingbatsChar_a192, PDFZapfDingbatsChar_a166, PDFZapfDingbatsChar_a167, 
            PDFZapfDingbatsChar_a168, PDFZapfDingbatsChar_a169, PDFZapfDingbatsChar_a170, 
            PDFZapfDingbatsChar_a171, PDFZapfDingbatsChar_a172, PDFZapfDingbatsChar_a173, 
            PDFZapfDingbatsChar_a162, PDFZapfDingbatsChar_a174, PDFZapfDingbatsChar_a175, 
            PDFZapfDingbatsChar_a176, PDFZapfDingbatsChar_a177, PDFZapfDingbatsChar_a178, 
            PDFZapfDingbatsChar_a179, PDFZapfDingbatsChar_a193, PDFZapfDingbatsChar_a180, 
            PDFZapfDingbatsChar_a199, PDFZapfDingbatsChar_a181, PDFZapfDingbatsChar_a200, 
            PDFZapfDingbatsChar_a182, PDFZapfDingbatsChar_NULL, PDFZapfDingbatsChar_a201, 
            PDFZapfDingbatsChar_a183, PDFZapfDingbatsChar_a184, PDFZapfDingbatsChar_a197, 
            PDFZapfDingbatsChar_a185, PDFZapfDingbatsChar_a194, PDFZapfDingbatsChar_a198, 
            PDFZapfDingbatsChar_a186, PDFZapfDingbatsChar_a195, PDFZapfDingbatsChar_a187, 
            PDFZapfDingbatsChar_a188, PDFZapfDingbatsChar_a189, PDFZapfDingbatsChar_a190, 
            PDFZapfDingbatsChar_a191, PDFZapfDingbatsChar_NULL
        }
    };

    static  ppUns16 PDFStdCharWidth[][230]      ={{
    /* Helvetica */
    278,667, 1000, 667, 667, 667, 667, 667, 667, 667, 722, 722, 722, 667, 667, 667, 667, 667, 722, 
    556, 611, 778, 722, 278, 278, 278, 278, 278, 500, 667, 556, 556, 833, 722, 722, 778, 1000, 
    778, 778, 778, 778, 778, 778, 667, 778, 722, 667, 667, 611, 667, 722, 722, 722, 722, 722, 
    667, 944, 667, 667, 667, 667, 611, 611, 556, 556, 556, 333, 556, 889, 556, 667, 556, 469, 
    584, 389, 1015, 556, 556, 278, 260, 334, 334, 278, 278, 333, 260, 350, 500, 333, 500, 333, 
    556, 333, 278, 278, 737, 556, 556, 556, 556, 400, 333, 584, 556, 333, 278, 556, 556, 556, 556, 
    556, 556, 1000, 1000, 556, 584, 556, 278, 333, 278, 500, 556, 500, 556, 556, 167, 556, 611, 333, 
    584, 556, 556, 333, 333, 556, 333, 333, 222, 278, 278, 278, 278, 222, 500, 222, 584, 584, 222, 833, 
    333, 584, 556, 584, 556, 556, 556, 556, 556, 556, 556, 556, 944, 333, 556, 556, 834, 834, 333, 370, 
    365, 611, 556, 556, 537, 333, 333, 889, 278, 278, 1000, 584, 584, 556, 556, 611, 355, 333, 333, 333, 
    222, 222, 222, 191, 333, 737, 333, 500, 500, 556, 278, 556, 556, 278, 278, 556, 278, 556, 556, 834, 
    333, 333, 1000, 556, 333, 556, 556, 556, 556, 556, 556, 500, 722, 500, 500, 500, 500, 556, 500, 500, 556
    },
                                                  {
    /* Helvetica Bold*/
    278,722, 1000, 722, 722, 722, 722, 722, 722, 722, 722, 722, 722, 667, 667, 667, 667, 667, 722, 556, 
    611, 778, 722, 278, 278, 278, 278, 278, 556, 722, 611, 611, 833, 722, 722, 778, 1000, 778, 778, 
    778, 778, 778, 778, 667, 778, 722, 667, 667, 611, 667, 722, 722, 722, 722, 722, 667, 944, 667, 
    667, 667, 667, 611, 611, 556, 556, 556, 333, 556, 889, 556, 722, 556, 584, 584, 389, 975, 556, 
    611, 278, 280, 389, 389, 333, 333, 333, 280, 350, 556, 333, 556, 333, 556, 333, 333, 278, 737, 
    556, 611, 556, 556, 400, 333, 584, 556, 333, 278, 556, 556, 556, 556, 556, 556, 1000, 1000, 556,
    584, 611, 333, 333, 333, 611, 556, 611, 556, 556, 167, 611, 611, 333, 584, 556, 556, 333, 333,
    611, 333, 333, 278, 278, 278, 278, 278, 278, 556, 278, 584, 584, 278, 889, 333, 584, 611, 584, 
    611, 556, 611, 556, 611, 611, 611, 611, 944, 333, 611, 556, 834, 834, 333, 370, 365, 611, 611, 
    611, 556, 333, 333, 889, 278, 278, 1000, 584, 584, 611, 611, 611, 474, 500, 500, 500, 278, 278, 
    278, 238, 389, 737, 333, 556, 556, 556, 333, 556, 556, 278, 278, 556, 333, 611, 556, 834, 333, 
    333, 1000, 556, 333, 611, 611, 611, 611, 611, 556, 556, 778, 556, 556, 556, 556, 556, 500, 500, 556

    },
                                                  {
    /* Helvetica Oblique */
    278, 667, 1000, 667, 667, 667, 667, 667, 667, 667, 722, 722, 722, 667, 667, 667, 667, 667, 722, 556, 
    611, 778, 722, 278, 278, 278, 278, 278, 500, 667, 556, 556, 833, 722, 722, 778, 1000, 778, 778, 
    778, 778, 778, 778, 667, 778, 722, 667, 667, 611, 667, 722, 722, 722, 722, 722, 667, 944, 667, 
    667, 667, 667, 611, 611, 556, 556, 556, 333, 556, 889, 556, 667, 556, 469, 584, 389, 1015, 556, 
    556, 278, 260, 334, 334, 278, 278, 333, 260, 350, 500, 333, 500, 333, 556, 333, 278, 278, 737, 
    556, 556, 556, 556, 400, 333, 584, 556, 333, 278, 556, 556, 556, 556, 556, 556, 1000, 1000, 556, 
    584, 556, 278, 333, 278, 500, 556, 500, 556, 556, 167, 556, 611, 333, 584, 556, 556, 333, 333, 
    556, 333, 333, 222, 278, 278, 278, 278, 222, 500, 222, 584, 584, 222, 833, 333, 584, 556, 584, 
    556, 556, 556, 556, 556, 556, 556, 556, 944, 333, 556, 556, 834, 834, 333, 370, 365, 611, 556, 
    556, 537, 333, 333, 889, 278, 278, 1000, 584, 584, 556, 556, 611, 355, 333, 333, 333, 222, 222, 
    222, 191, 333, 737, 333, 500, 500, 556, 278, 556, 556, 278, 278, 556, 278, 556, 556, 834, 333, 
    333, 1000, 556, 333, 556, 556, 556, 556, 556, 556, 500, 722, 500, 500, 500, 500, 556, 500, 500, 556
    },
                                                  {
    /* Helvetica  BoldOblique*/
    278,722, 1000, 722, 722, 722, 722, 722, 722, 722, 722, 722, 722, 667, 667, 667, 667, 667, 722, 556, 
    611, 778, 722, 278, 278, 278, 278, 278, 556, 722, 611, 611, 833, 722, 722, 778, 1000, 778, 778, 
    778, 778, 778, 778, 667, 778, 722, 667, 667, 611, 667, 722, 722, 722, 722, 722, 667, 944, 667, 
    667, 667, 667, 611, 611, 556, 556, 556, 333, 556, 889, 556, 722, 556, 584, 584, 389, 975, 556, 
    611, 278, 280, 389, 389, 333, 333, 333, 280, 350, 556, 333, 556, 333, 556, 333, 333, 278, 737, 
    556, 611, 556, 556, 400, 333, 584, 556, 333, 278, 556, 556, 556, 556, 556, 556, 1000, 1000, 556, 
    584, 611, 333, 333, 333, 611, 556, 611, 556, 556, 167, 611, 611, 333, 584, 556, 556, 333, 333, 
    611, 333, 333, 278, 278, 278, 278, 278, 278, 556, 278, 584, 584, 278, 889, 333, 584, 611, 584, 
    611, 556, 611, 556, 611, 611, 611, 611, 944, 333, 611, 556, 834, 834, 333, 370, 365, 611, 611, 
    611, 556, 333, 333, 889, 278, 278, 1000, 584, 584, 611, 611, 611, 474, 500, 500, 500, 278, 278, 
    278, 238, 389, 737, 333, 556, 556, 556, 333, 556, 556, 278, 278, 556, 333, 611, 556, 834, 333, 
    333, 1000, 556, 333, 611, 611, 611, 611, 611, 556, 556, 778, 556, 556, 556, 556, 556, 500, 500, 556
    },
                                                  {
    /* Times Roman*/
    250, 722, 889, 722, 722, 722, 722, 722, 722, 667, 667, 667, 722, 611, 611, 611, 611, 611, 722, 500, 
    556, 722, 722, 333, 333, 333, 333, 333, 389, 722, 611, 611, 889, 722, 722, 722, 889, 722, 722, 
    722, 722, 722, 722, 556, 722, 667, 556, 556, 611, 556, 722, 722, 722, 722, 722, 722, 944, 722, 
    722, 722, 722, 611, 611, 444, 444, 444, 333, 444, 667, 444, 778, 444, 469, 541, 500, 921, 444, 
    500, 278, 200, 480, 480, 333, 333, 333, 200, 350, 444, 333, 444, 333, 500, 333, 278, 250, 760, 
    500, 500, 500, 500, 400, 333, 564, 500, 333, 278, 444, 444, 444, 444, 444, 500, 1000, 1000, 500,
    564, 500, 333, 333, 333, 556, 500, 556, 500, 500, 167, 500, 500, 333, 564, 500, 500, 333, 333, 
    500, 333, 333, 278, 278, 278, 278, 278, 278, 500, 278, 564, 564, 278, 778, 333, 564, 500, 564, 
    500, 500, 500, 500, 500, 500, 500, 500, 722, 333, 500, 500, 750, 750, 300, 276, 310, 500, 500, 
    500, 453, 333, 333, 833, 250, 250, 1000, 564, 564, 500, 444, 444, 408, 444, 444, 444, 333, 333, 
    333, 180, 333, 760, 333, 389, 389, 500, 278, 500, 500, 278, 250, 500, 278, 500, 500, 750, 300, 
    333, 980, 500, 300, 500, 500, 500, 500, 500, 500, 500, 722, 500, 500, 500, 500, 500, 444, 444, 500
    },
                                                  {
    /* Times Bold*/
    250, 722, 1000, 722, 722, 722, 722, 722, 722, 667, 722, 722, 722, 667, 667, 667, 667, 667, 722, 500, 
    611, 778, 778, 389, 389, 389, 389, 389, 500, 778, 667, 667, 944, 722, 722, 778, 1000, 778, 778, 
    778, 778, 778, 778, 611, 778, 722, 556, 556, 667, 611, 722, 722, 722, 722, 722, 722, 1000, 722, 
    722, 722, 722, 667, 667, 500, 500, 500, 333, 500, 722, 500, 833, 500, 581, 520, 500, 930, 500, 
    556, 278, 220, 394, 394, 333, 333, 333, 220, 350, 444, 333, 444, 333, 500, 333, 333, 250, 747, 
    500, 556, 500, 500, 400, 333, 570, 500, 333, 278, 444, 444, 444, 444, 444, 500, 1000, 1000, 500, 
    570, 500, 333, 333, 333, 556, 500, 556, 500, 500, 167, 500, 556, 333, 570, 500, 500, 333, 333, 
    556, 333, 333, 278, 278, 278, 278, 278, 333, 556, 278, 570, 570, 278, 833, 333, 570, 556, 570, 
    556, 500, 556, 500, 500, 500, 500, 500, 722, 333, 500, 500, 750, 750, 300, 300, 330, 500, 500, 
    556, 540, 333, 333, 1000, 250, 250, 1000, 570, 570, 556, 500, 500, 555, 500, 500, 500, 333, 333, 
    333, 278, 444, 747, 333, 389, 389, 500, 333, 500, 500, 278, 250, 500, 333, 556, 500, 750, 300, 
    333, 1000, 500, 300, 556, 556, 556, 556, 556, 500, 500, 722, 500, 500, 500, 500, 500, 444, 444, 500
    },
                                                  {
    /* Times Italic*/
    250, 611, 889, 611, 611, 611, 611, 611, 611, 611, 667, 667, 722, 611, 611, 611, 611, 611, 722, 500, 
    611, 722, 722, 333, 333, 333, 333, 333, 444, 667, 556, 556, 833, 667, 667, 722, 944, 722, 722, 
    722, 722, 722, 722, 611, 722, 611, 500, 500, 556, 611, 722, 722, 722, 722, 722, 611, 833, 611, 
    556, 556, 556, 556, 556, 500, 500, 500, 333, 500, 667, 500, 778, 500, 422, 541, 500, 920, 500, 
    500, 278, 275, 400, 400, 389, 389, 333, 275, 350, 444, 333, 444, 333, 500, 333, 333, 250, 760, 
    500, 500, 500, 500, 400, 333, 675, 500, 333, 278, 444, 444, 444, 444, 444, 500, 889, 889, 500, 
    675, 500, 333, 389, 278, 500, 500, 500, 500, 500, 167, 500, 500, 333, 675, 500, 500, 333, 333, 
    500, 333, 333, 278, 278, 278, 278, 278, 278, 444, 278, 675, 675, 278, 722, 333, 675, 500, 675, 
    500, 500, 500, 500, 500, 500, 500, 500, 667, 333, 500, 500, 750, 750, 300, 276, 310, 500, 500, 
    500, 523, 333, 333, 833, 250, 250, 1000, 675, 675, 500, 500, 500, 420, 556, 556, 556, 333, 333, 
    333, 214, 389, 760, 333, 389, 389, 500, 333, 500, 500, 278, 250, 500, 278, 500, 500, 750, 300, 
    333, 980, 500, 300, 500, 500, 500, 500, 500, 500, 444, 667, 444, 444, 444, 444, 500, 389, 389, 500
    },
                                                  {
    /* Times Bold Italic*/
    250, 667, 944, 667, 667, 667, 667, 667, 667, 667, 667, 667, 722, 667, 667, 667, 667, 667, 722, 500,
    667, 722, 778, 389, 389, 389, 389, 389, 500, 667, 611, 611, 889, 722, 722, 722, 944, 722, 722, 
    722, 722, 722, 722, 611, 722, 667, 556, 556, 611, 611, 722, 722, 722, 722, 722, 667, 889, 667, 
    611, 611, 611, 611, 611, 500, 500, 500, 333, 500, 722, 500, 778, 500, 570, 570, 500, 832, 500, 
    500, 278, 220, 348, 348, 333, 333, 333, 220, 350, 444, 333, 444, 333, 500, 333, 333, 250, 747, 
    500, 500, 500, 500, 400, 333, 570, 500, 333, 278, 444, 444, 444, 444, 444, 500, 1000, 1000, 500, 
    570, 500, 389, 389, 333, 556, 500, 556, 500, 500, 167, 500, 500, 333, 570, 500, 500, 333, 333, 
    556, 333, 333, 278, 278, 278, 278, 278, 278, 500, 278, 570, 606, 278, 778, 333, 606, 576, 570, 
    556, 500, 556, 500, 500, 500, 500, 500, 722, 333, 500, 500, 750, 750, 300, 266, 300, 500, 500, 
    500, 500, 333, 333, 833, 250, 250, 1000, 570, 570, 500, 500, 500, 555, 500, 500, 500, 333, 333, 
    333, 278, 389, 747, 333, 389, 389, 500, 333, 500, 500, 278, 250, 500, 278, 500, 500, 750, 300, 
    333, 1000, 500, 300, 556, 556, 556, 556, 556, 500, 444, 667, 500, 444, 444, 444, 500, 389, 389, 500
    },
                                                  {
    /* Courier*/
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600
    },
                                                  {
    /* Courier Bold*/
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600 
    },
                                                  {
    /* Courier Oblique*/
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600
    },
                                                  {
    /* Courier Bold Oblique*/
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 
    600, 600
    }};

    static ppUns16 PDFSymbolCharWidth[190]      ={250, 667, 722, 612, 611, 722, 750, 603, 686,
                                                  333, 722, 686, 889, 722, 768, 722, 763, 768,
                                                  795, 795, 556, 592, 611, 741, 690, 620, 645,
                                                  611, 823, 631, 778, 768, 329, 329, 549, 1042,
                                                  1042, 603, 987, 987, 603, 603, 1000, 987, 987,
                                                  603, 603, 500, 200, 549, 494, 480, 494, 494,
                                                  494, 480, 494, 494, 494, 333, 384, 384, 384,
                                                  333, 384, 384, 384, 460, 658, 549, 768, 768,
                                                  753, 278, 250, 549, 790, 790, 400, 494, 753,
                                                  549, 250, 500, 713, 1000, 823, 439, 549, 549,
                                                  603, 333, 549, 500, 500, 500, 167, 411, 713,
                                                  549, 549, 753, 713, 274, 686, 686, 686, 768,
                                                  329, 549, 549, 549, 549, 603, 713, 603, 494,
                                                  549, 247, 576, 549, 500, 713, 549, 713, 521,
                                                  500, 686, 713, 549, 500, 333, 384, 384, 384,
                                                  333, 384, 384, 384, 494, 833, 250, 658, 521,
                                                  603, 549, 549, 549, 823, 713, 713, 713, 686,
                                                  444, 549, 500, 713, 713, 790, 790, 549, 411,
                                                  278, 500, 603, 439, 549, 500, 278, 250, 753,
                                                  439, 713, 439, 863, 521, 631, 500, 786, 890,
                                                  500, 500, 768, 713, 576, 987, 493, 500, 494};

    static ppUns16 PDFZapfDingbatsCharWidth[189]={278, 974, 692, 668, 732, 544, 544, 910, 911,
                                                  667, 760, 760, 626, 960, 694, 595, 776, 690,
                                                  791, 790, 939, 788, 788, 788, 788, 788, 788,
                                                  788, 788, 788, 788, 549, 788, 788, 788, 788,
                                                  788, 788, 788, 788, 788, 788, 855, 788, 788,
                                                  788, 788, 788, 788, 788, 788, 788, 788, 911,
                                                  788, 788, 788, 788, 788, 788, 788, 788, 788,
                                                  788, 933, 894, 838, 924, 1016, 458, 924, 918,
                                                  927, 928, 928, 945, 834, 873, 828, 924, 917,
                                                  930, 931, 463, 883, 836, 974, 867, 696, 874,
                                                  760, 946, 865, 967, 831, 873, 927, 755, 970,
                                                  918, 748, 836, 771, 888, 748, 771, 888, 867,
                                                  961, 846, 696, 874, 974, 762, 759, 762, 761,
                                                  571, 677, 763, 760, 759, 754, 786, 980, 788,
                                                  788, 790, 793, 794, 816, 823, 789, 841, 823,
                                                  719, 833, 816, 831, 923, 744, 723, 749, 790,
                                                  792, 695, 789, 776, 768, 792, 759, 707, 708,
                                                  682, 701, 826, 815, 494, 789, 789, 707, 687,
                                                  696, 689, 786, 787, 713, 791, 552, 785, 791,
                                                  873, 761, 762, 759, 892, 892, 788, 784, 537,
                                                  438, 138, 277, 415, 577, 392, 392, 668, 278};


	static char *ULStdEncodeList[]  ={"PDFDocEncoding", "WinAnsiEncoding", "MacRomanEncoding",
		"StandardEncoding"};
	


	static PDFStdFontMatrix StdFontInfoMatrix[14] = {
		{ "Helvetica",				728,	-210,	{-166, -225, 1000,  931} },
		{ "Helvetica-Bold",			728,	-210,	{-170, -228, 1003,  962} },
		{ "Helvetica-Oblique",		728,	-208,	{-170, -225, 1116,  931} },
		{ "Helvetica-BoldOblique",	728,	-210,	{-174, -228, 1114,  962} },
		{ "Times-Roman",			613,	-188,	{-168, -218, 1000,  898} },
		{ "Times-Bold",				633,	-209,	{-168, -218, 1000,  935} },
		{ "Times-Italic",			613,	-188,	{-169, -217, 1010,  883} },
		{ "Times-BoldItalic",		633,	-216,	{-200, -218,  996,  921} },
		{ "Courier",				693,	-216,	{ -23, -250,  715,  805} },
		{ "Courier-Bold",			677,	-216,	{-113, -250,  749,  801} },
		{ "Courier-Oblique",		694,	-216,	{ -27, -250,  849,  805} },
		{ "Courier-BoldOblique",	677,	-216,	{ -57, -250,  869,  801} },
		{ "Symbol",					1000,	   0,	{-180, -293, 1090, 1010} },
		{ "ZapfDingbats",			1000,	   0,	{  -1, -143,  981,  820} }
	};


ppUns32 PDFFontStandardAppend(PDFDocHandle Doc, TPDFStdandardFont font,
                                TPDFEncodingType encode)
{
    ppUns32 i, j;
    PPDFFontInfo inf;
    PDFCosHandle obj;
    if ( !_DOC->FontList )
        _DOC->FontList = ULListCreate ( _LIB, 1 );
    for ( i = 0; i < _DOC->FontList->Size; i++ )
        if ( FE ( _DOC->FontList->FList[i] )->Type == PDFStdFont &&
             FE ( _DOC->FontList->FList[i] )->FontInfo.StdInfo.ID == font &&
             ( font > stdfCourierBoldOblique ||
               FE ( _DOC->FontList->FList[i] )->FontInfo.StdInfo.Encode == encode ) )
            return i;
    inf = ( PDFFontInfo * ) mmalloc ( _LIB, sizeof ( PDFFontInfo ) );
    PDFTRY ( _LIB ){
        i = ULListAdd ( _DOC->FontList, inf );
    } PDFEXCEPT ( _LIB ){
        mfree ( _LIB, inf );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB )
    inf->Lib = _LIB;
    inf->FontInfo.StdInfo.ID = font;
    inf->FontInfo.StdInfo.Encode = encode;
    inf->Type = PDFStdFont;
    inf->metrics.Ascent = StdFontInfoMatrix[ font ].Abscent;
    inf->metrics.Descent = StdFontInfoMatrix[ font ].Descent;
    obj = CosDictNew ( Doc, true, 4 );
    inf->FontInfo.StdInfo.Handle = obj;
    _CosDictAppend ( obj, Type, _CosNameNew ( Font ) );
    _CosDictAppend ( obj, Subtype, _CosNameNew ( Type1 ) );
    _CosDictAppend ( obj, BaseFont, CosNameNew ( Doc, false,
        ULStringToAtom ( _LIB, StdFontInfoMatrix[font].FontName ) ) );
    if ( font <= stdfCourierBoldOblique ){
        _CosDictAppend ( obj, Encoding, CosNameNew ( Doc, false,
            ULStringToAtom ( _LIB, ULStdEncodeList[encode] ) ) );
        for ( j = 0; j < 256; j++ )
            inf->metrics.ANSICodes[j] = PDFStdCharWidth[font][PDFEncodeToPDFChar[encode][j]];
    } else 
        if ( font == stdfSymbol ){
            for ( j = 0; j < 256; j++ )
                inf->metrics.ANSICodes[j] = PDFSymbolCharWidth[PDFEncodeToPDFChar[PDFSYMBOLENCODE][j]];
        } else 
            if ( font == stdfZapfDingbats ){
                for ( j = 0; j < 256; j++ )
                    inf->metrics.ANSICodes[j] = PDFZapfDingbatsCharWidth[PDFEncodeToPDFChar[PDFZAPFDINGBATSENCODE][j]];
            }

    return i;
}

void PDFFontFillUsed(PPDFFontInfo Font, ppUns8 *Text)
{
    ppInt32 i, k;
    k = ( ppInt32 ) strlen ( (const char *)Text );
    if ( Font->Type == PDFTrueTypeFont )
        for ( i = 0; i < k; i++ )
            if ( !( Font->FontInfo.TTFInfo->ANSI->UsedChars[Text[i]] ) ){
                Font->FontInfo.TTFInfo->ANSI->UsedChars[Text[i]] = true;
                Font->FontInfo.TTFInfo->ANSI->Modified = true;
                if ( Text[i] > Font->FontInfo.TTFInfo->ANSI->LastChar )
                    Font->FontInfo.TTFInfo->ANSI->LastChar = Text[i];
                if ( Text[i] < Font->FontInfo.TTFInfo->ANSI->FirstChar )
                    Font->FontInfo.TTFInfo->ANSI->FirstChar = Text[i];
            }
}


ppUns32 PDFFontTrueTypeAppendFromFile(PDFDocHandle Doc, char *fontfilename)
{
    PDFStreamHandle ms;
    ppInt32 i=0;   

    ms = ULStreamFileNew ( _LIB, fontfilename, ppFileReadMode );
    PDFTRY ( _LIB ){
        i = PDFFontTrueTypeAppendFromStream ( Doc, ms );
    } PDFEXCEPT ( _LIB ){
        ULStreamClose ( ms );
	    PDFRERAISE( _LIB );
    }
    PDFTRYEND ( _LIB )
    ULStreamClose ( ms );
    return i;
}

ppUns32 PDFFontTrueTypeAppendFromStream(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    ppAtom psn;
    PPDFFontTables tables;
    char *n = NULL;
    ppUns32 i;
    ppUns16 ln;
    PPDFFontInfo inf;
    
    tables = PDFTTFLoadFontFromStream ( _LIB, Strm );
    if ( !tables )
        _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );
    PDFTRY ( _LIB ){
        n = PDFTTFGetFontNameFromTables ( _LIB, ppFontPSName, tables, &ln );
        if ( !n )
            _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );  
        psn = ULStringToAtom ( _LIB, n );
        mfree ( _LIB, n );
    } PDFEXCEPT ( _LIB ){
        chfree ( _LIB, n );
        PDFTTFFreeFont ( _LIB, tables );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    if ( !( _DOC->FontList ) )
        _DOC->FontList = ULListCreate ( _LIB, 2 );
    else
        for ( i = 0; i < _DOC->FontList->Size; i++ )
            if ( FE ( _DOC->FontList->FList[i] )->Type == PDFTrueTypeFont &&
                FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->FontName == psn ){
                PDFTTFFreeFont ( _LIB, tables );
                return i;
            };
    inf = ( PDFFontInfo * ) mmalloc ( _LIB, sizeof ( PDFFontInfo ) );
    PDFTRY ( _LIB ){
        i = ULListAdd ( _DOC->FontList, inf );
    } PDFEXCEPT ( _LIB ){
        mfree ( _LIB, inf );
        PDFTTFFreeFont ( _LIB, tables );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    inf->Lib = _LIB;
    inf->Type = PDFTrueTypeFont;
    PDFTRY ( _LIB ){
        inf->FontInfo.TTFInfo = ( PDFTrueTypeInfo * ) mmalloc ( _LIB, sizeof ( PDFTrueTypeInfo ) );
    } PDFEXCEPT ( _LIB ){
        PDFTTFFreeFont ( _LIB, tables );
        PDFRERAISE ( _LIB );
    }
    PDFTRYEND ( _LIB );
    inf->FontInfo.TTFInfo->Font = tables;
    inf->FontInfo.TTFInfo->ANSI = NULL;
    inf->FontInfo.TTFInfo->Unicode = NULL;
    inf->FontInfo.TTFInfo->FontName = psn;
    PDFTTFGetRequiredMetrics ( &( inf->metrics ), inf->FontInfo.TTFInfo->Font );
    return i;
}

#ifdef WINDOWS_PLATFORM

#define til(i) TTFIt(CD(Doc)->TTFList,i)

static int CALLBACK ULFontTestTrueTypeBack(const LOGFONT *lf,
                                           const TEXTMETRIC *tm,
                                           ppUns32 FontType, LPARAM valid)
{
    *( ppBool * ) ( valid ) = (ppBool)( FontType == TRUETYPE_FONTTYPE );
    return 0;
}

static char *PDFPanoseAskFontName[3] = {"HELVETICA", "COURIER", "TIMES ROMAN"};
static char *PDFPanoseReturnFontName[3] = {"Arial", "Courier New", "Times New Roman"};


char * PDFFontFindEqual( char * fontname)
{
    char FN[LF_FACESIZE];
    ppInt32 i, l;
    strcpy( FN, fontname );
    l = ( ppInt32 ) strlen ( fontname );
    for ( i = 0; i < l; i++ )
        FN[i] = ( char ) toupper ( FN[i] );
    for (i = 0; i < 3; i++ ){
        if ( ! strcmp ( PDFPanoseAskFontName[i], FN) )
            return PDFPanoseReturnFontName[i];
    }
    return PDFPanoseReturnFontName[0];
}

ppUns32 PDFFontAppendFromDC (PDFDocHandle Doc, HDC DC)
{
    ppUns32 SZ, i, k;
    ppAtom psn;
    void * buf;
    char * n;
    ppUns16 t;
    PPDFFontInfo inf;
	ABC Widths[256];
	TEXTMETRICA TM;
	ppBool Mono;

    SZ = GetFontData ( DC, 0x656d616e, 0, NULL, 0 );
    if ( SZ == GDI_ERROR )
        _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );
    buf = mmalloc ( _LIB, SZ );
    PDFTRY ( _LIB ){
        SZ = GetFontData ( DC, 0x656d616e, 0, buf, SZ );
        if ( SZ == GDI_ERROR )
            _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );
        n = PDFTTFGetFontNameFromNameTable ( _LIB, ppFontPSName, ( char * ) buf, &t );
        if ( !n )
            _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );
        psn = ULStringToAtom ( _LIB, n );
        mfree ( _LIB, n );
    } PDFFINALLY ( _LIB ){
        mfree ( _LIB, buf );
    }
    PDFFINALLYEND ( _LIB );
    if ( !_DOC->FontList )
        _DOC->FontList = ULListCreate ( _LIB, 1 );
    for ( i = 0; i < _DOC->FontList->Size; i++ )
        if ( FE ( _DOC->FontList->FList[i] )->Type == PDFTrueTypeFont &&
            FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->FontName == psn )
                break;
    if ( i == _DOC->FontList->Size ){
        inf = ( PDFFontInfo * ) mmalloc ( _LIB, sizeof ( PDFFontInfo ) );
        PDFTRY ( _LIB ){
            i = ULListAdd ( _DOC->FontList, inf );
        } PDFEXCEPT ( _LIB ){
            mfree ( _LIB, inf );
            PDFRERAISE ( _LIB );
        }
        PDFTRYEND ( _LIB );
        inf->Lib = _LIB;
        inf->Type = PDFTrueTypeFont;
        inf->FontInfo.TTFInfo = ( PDFTrueTypeInfo * ) mmalloc ( _LIB, sizeof ( PDFTrueTypeInfo ) );
        inf->FontInfo.TTFInfo->Font = PDFTTFLoadFontFromDC ( _LIB, DC );
        PDFTTFGetRequiredMetrics ( &( inf->metrics ), inf->FontInfo.TTFInfo->Font );
		GetTextMetrics(DC,&TM);
		Mono = TM.tmPitchAndFamily & TMPF_FIXED_PITCH ? false: true;
		GetCharABCWidths( DC, 0, 255,Widths);
		if (Mono) {
			for (k =0; k<256;k++)
				inf->metrics.ANSICodes[k] = (ppInt16)(Widths[0].abcA + Widths[0].abcB + Widths[0].abcC);
		} else {
			for (k =0; k<256;k++)
				inf->metrics.ANSICodes[k] = (ppInt16)(Widths[k].abcA + Widths[k].abcB + Widths[k].abcC);
		}
        inf->FontInfo.TTFInfo->ANSI = NULL;
        inf->FontInfo.TTFInfo->Unicode = NULL;
        inf->FontInfo.TTFInfo->FontName = psn;
		
    }
    return i;
}

ppUns32 PDFFontTrueTypeAppend(PDFDocHandle Doc, char *fontname, ppBool Bold,
                              ppBool Italic)
{
    ppUns32 i;
    LOGFONT lf;
    ppBool FND;
    TTFListItemP pp;
    HDC DC;
    HGDIOBJ Obj;
    char *fn;
    ppAtom nm = ULStringToAtom ( _LIB, fontname);
    if ( !( _DOC->TTFList ) )
        _DOC->TTFList = ULListCreate ( _LIB, 2 );
    else
        for ( i = 0; i < _DOC->TTFList->Size; i++ ){
            if ( til ( i )->Name == nm &&
                 til ( i )->Bold == Bold &&
                 til ( i )->Italic == Italic )
                return til ( i )->Index;
        }
    memset ( &lf, 0, sizeof ( lf ) );
    lf.lfCharSet = DEFAULT_CHARSET;
    memmove ( &( lf.lfFaceName ), fontname, strlen ( fontname ) );
    DC = GetDC ( 0 );
    PDFTRY ( _LIB ){
        EnumFontFamiliesEx ( DC, &lf, &ULFontTestTrueTypeBack, ( LPARAM ) ( &FND ), 0 );
        if ( !FND ){
            fn = PDFFontFindEqual ( fontname );
            memset ( &( lf.lfFaceName ), 0, sizeof ( lf.lfFaceName ) );
            memmove ( &( lf.lfFaceName ), fn, strlen ( fn ) );
        };  
        lf.lfWeight = Bold ? FW_BOLD : FW_NORMAL;
        lf.lfItalic = ( ppUns8 ) Italic;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfQuality = DEFAULT_QUALITY;
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        lf.lfPitchAndFamily = DEFAULT_PITCH;
		lf.lfWidth = 0;
		lf.lfHeight = -1000;
        Obj = CreateFontIndirect ( &lf );
        SelectObject ( DC, Obj );
        PDFTRY ( _LIB ){
            i = PDFFontAppendFromDC (Doc, DC );
        } PDFFINALLY ( _LIB ){
            SelectObject ( DC, GetStockObject ( SYSTEM_FONT ) );
            DeleteObject ( Obj );
        }
        PDFFINALLYEND ( _LIB );
    } PDFFINALLY ( _LIB ){
        ReleaseDC ( 0, DC );
    }
    PDFFINALLYEND ( _LIB );
    pp = ( TTFListItem * ) mmalloc ( _LIB, sizeof ( TTFListItem ) );
    pp->Bold = Bold;
    pp->Italic = Italic;
    pp->Name = nm;
    pp->Index = i;
    ULListAdd ( _DOC->TTFList, pp );
    return i;
}
#endif


void PDFFontTrueTypeClear( PDFLibHandle Lib, PPDFFontInfo Font)
{
    if ( Font->Type != PDFTrueTypeFont )
        return;
    if ( Font->FontInfo.TTFInfo ){
        if ( Font->FontInfo.TTFInfo->ANSI )
            mfree ( Lib, Font->FontInfo.TTFInfo->ANSI );
        if ( Font->FontInfo.TTFInfo->Unicode )
            mfree ( Lib, Font->FontInfo.TTFInfo->Unicode );
        PDFTTFFreeFont ( Lib, Font->FontInfo.TTFInfo->Font );
        mfree ( Lib, Font->FontInfo.TTFInfo );
    }
}

void PDFTTFFontUpdate(PDFDocHandle Doc)
{
    ppUns32 i, k;
    ppUns16 first, last, count;
    PDFStreamHandle strm;
    PDFCosHandle obj, ar, wrk;
    ppUns16 *used, a;
    ppUns32 j;
    PDFTrueTypeANSIInfoP an;
    PDFTrueTypeUnicodeInfoP un;
    PPDFTrueTypeInfo ttf;
    PDFStreamHandle emb;
    if ( !( _DOC->FontList ) )
        return;
    for ( i = 0; i < _DOC->FontList->Size; i++ )
        if ( FE ( _DOC->FontList->FList[i] )->Type == PDFTrueTypeFont ){
            if ( FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->ANSI &&
                 FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->ANSI->Modified ){
                an = FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->ANSI;
                if ( !_IsCosNull(an->FontFile )){
                    for ( j = 0,a = 0; j < 256; j++ ){
                        if ( an->UsedChars[j] )
                            a++;
                    }
                    used = ( ppUns16 * ) mmalloc ( _LIB, sizeof ( ppUns16 ) * a );
                    PDFTRY ( _LIB ){
                        for ( j = 0,a = 0; j < 256; j++ ){
                            if ( an->UsedChars[j] )
                                used[a++] = ULANSIToUnicode ( ( ppUns8 ) j );
                        }
                        emb = PDFTTFGetANSIEmbeddedFont ( _LIB, FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->Font,
							&used, a );
                    } PDFFINALLY ( _LIB ){
                        mfree ( _LIB, used );
                    }
                    PDFFINALLYEND ( _LIB );
                    if ( !emb )
                        _RAISE ( ErrGeneralLevel,
                                 gleCannotReceiveFontDataError );
                    PDFTRY ( _LIB ){
                        strm = _CosStreamValue ( an->FontFile );
                        _CosDictRemoveKey ( an->FontFile, Filter );
                        ULStreamClear ( strm, 0 );
                        ULStreamSetPosition ( emb, 0 );
                        ULStreamCopyToStream ( emb, strm );
                        CosStreamFlatePack ( _LIB, an->FontFile );
                        _CosDictAppend ( an->FontFile, Length, CosIntNew ( Doc, false,
                            ULStreamGetSize ( _CosStreamValue ( an->FontFile ) ) ) );
                        _CosDictAppend ( an->FontFile, Length1, CosIntNew ( Doc, false,
                            ULStreamGetSize ( emb ) ) );
                    } PDFFINALLY ( _LIB ){
                        ULStreamClose ( emb );
                    }
                    PDFFINALLYEND ( _LIB );
                };
				_CosDictAppend ( an->Font, Encoding, _CosNameNew ( WinAnsiEncoding ) );
                _CosDictAppend ( an->Font, FirstChar, CosIntNew ( Doc, false, an->FirstChar ) );
                _CosDictAppend ( an->Font, LastChar, CosIntNew ( Doc, false, an->LastChar ) );
                _CosDictAppend ( an->Font, Widths,
                    obj = CosArrayNew ( Doc,false,  an->LastChar - an->FirstChar + 1 ) );
                for ( j = an->FirstChar; j <= ( ppUns32 )an->LastChar; j++ )
                    CosArrayAppend ( obj, CosIntNew ( Doc, false, an->UsedChars[j] ? 
                        FE ( _DOC->FontList->FList[i] )->metrics.ANSICodes[j] : 0 ) );
                FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->ANSI->Modified = false;
            };
            if ( FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->Unicode &&
                 FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->Unicode->Modified ){
                un = FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->Unicode;
                ttf = FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo;

                /* Widths array */
                _CosDictAppend ( un->Font, W, ar = CosArrayNew ( Doc, false, 20 ) );
                for ( j = 0,count = 0,first = 0xFFFF,last = 0;
                      j < ttf->Font->CodeLength;
                      j++ ){
                    if ( ttf->Font->CodeTable[j].Used ){
                        count++;
                        if ( first > ttf->Font->CodeTable[j].ID )
                            first = ttf->Font->CodeTable[j].ID;
                        if ( last < ttf->Font->CodeTable[j].ID )
                            last = ttf->Font->CodeTable[j].ID;
                        CosArrayAppend ( ar, CosIntNew ( Doc, false, ttf->Font->CodeTable[j].ID ) );
                        CosArrayAppend ( ar, wrk = CosArrayNew ( Doc, false, 1 ) );
                        CosArrayAppend ( wrk,CosIntNew ( Doc, false,
                            ttf->Font->CodeTable[j].Width ) );
                    }
					  };
                if ( !_IsCosNull(un->FontFile )){
                    emb = PDFTTFGetUnicodeEmbeddedFont ( _LIB, ttf->Font );
                    if ( !emb )
                        _RAISE ( ErrGeneralLevel, gleCannotReceiveFontDataError );
                    PDFTRY ( _LIB ){
                        strm = _CosStreamValue ( un->FontFile );
                        _CosDictRemoveKey ( un->FontFile, Filter );
                        ULStreamClear ( strm, 0 );
                        ULStreamSetPosition ( emb, 0 );
                        ULStreamCopyToStream ( emb, strm );
                        CosStreamFlatePack ( _LIB, un->FontFile );
                        _CosDictAppend ( un->FontFile, Length, CosIntNew ( Doc, false,
                            ULStreamGetSize ( _CosStreamValue ( un->FontFile ) ) ) );
                        _CosDictAppend ( un->FontFile, Length1, CosIntNew ( Doc, false,
                            ULStreamGetSize ( emb ) ) );
                    } PDFFINALLY ( _LIB ){
                        ULStreamClose ( emb );
                    }
                    PDFFINALLYEND ( _LIB );
                };
                strm = _CosStreamValue ( un->ToUnicode );
                _CosDictRemoveKey ( un->ToUnicode, Filter );
                ULStreamClear ( strm, 0 );
                ULStrToStrm ( strm,
                              "/CIDInit /ProcSet findresource begin 12 dict begin begincmap /CIDSystemInfo << \n" );
                ULStrToStrm ( strm, "/Registry (" );
                ULStrToStrm ( strm, ULAtomToString ( _LIB, ttf->FontName ) );
                ULStrToStrm ( strm,
                              "+0) /Ordering (T42UV) /Supplement 0 >> def\n" );
                ULStrToStrm ( strm, "/CMapName /" );
                ULStrToStrm ( strm, ULAtomToString ( _LIB, ttf->FontName ) );
                ULStrToStrm ( strm, "+0 def \n" );
                ULStrToStrm ( strm, "/CMapType 2 def\n" );
                ULStrToStrm ( strm, "1 begincodespacerange <" );
                ULHexUns16ToStrm ( strm, first );
                ULStrToStrm ( strm, "> <" );
                ULHexUns16ToStrm ( strm, last );
                ULStrToStrm ( strm, "> endcodespacerange\n" );
                j = 0;
                while ( count > 100 ){
                    ULStrToStrm ( strm, "100 beginbfchar\n" );
                    k = 0;
                    while ( k < 100 ){
                        if ( ttf->Font->CodeTable[j].Used ){
                            ULStrToStrm ( strm, "<" );
                            ULHexUns16ToStrm ( strm,
                                               ttf->Font->CodeTable[j].ID );
                            ULStrToStrm ( strm, "> <" );
                            ULHexUns16ToStrm ( strm,
                                               ttf->Font->CodeTable[j].Code );
                            ULStrToStrm ( strm, ">\n" );
                            k++;
                        }
                        j++;
                    }
                    count -= 100;
                    ULStrToStrm ( strm, "endbfchar\n" );
                }
                ULIntToStrm ( strm, count );
                ULStrToStrm ( strm, " beginbfchar\n" );
                k = 0;
                while ( k < count ){
                    if ( ttf->Font->CodeTable[j].Used ){
                        ULStrToStrm ( strm, "<" );
                        ULHexUns16ToStrm ( strm, ttf->Font->CodeTable[j].ID );
                        ULStrToStrm ( strm, "> <" );
                        ULHexUns16ToStrm ( strm, ttf->Font->CodeTable[j].Code );
                        ULStrToStrm ( strm, ">\n" );
                        k++;
                    }
                    j++;
                }
                ULStrToStrm ( strm, "endbfchar\n" );
                ULStrToStrm ( strm,
                              "endcmap CMapName currentdict /CMap defineresource pop end end" );
                CosStreamFlatePack ( _LIB, un->ToUnicode );
                _CosDictAppend ( un->ToUnicode, Length, CosIntNew ( Doc, false,
                    ULStreamGetSize ( _CosStreamValue ( un->ToUnicode ) ) ) );
                FE ( _DOC->FontList->FList[i] )->FontInfo.TTFInfo->Unicode->Modified = false;
            };
        }
}


#ifndef NOT_USE_FREETYPE 

ppUns32 PDFFontType1AppendFromFile(PDFDocHandle Doc, char * fontfilename)
{
    PDFStreamHandle ms;
    ppInt32 i = 0;
    ms = ULStreamFileNew ( _LIB, fontfilename, ppFileReadMode );
    PDFTRY ( _LIB ){
        i = PDFFontType1AppendFromStream ( Doc, ms );
    } PDFEXCEPT ( _LIB ){
        ULStreamClose ( ms );
	    PDFRERAISE( _LIB );
    }
    PDFTRYEND ( _LIB )
    ULStreamClose ( ms );
    return i;
}

ppUns32 PDFFontType1AppendFromStream(PDFDocHandle Doc, PDFStreamHandle Strm)
{
    ppAtom psn;
    char *n = NULL;
    ppUns32 i, idx;
	ppInt32 l1, l2, l3;
    PPDFFontInfo inf;
    PDFCosHandle obj,w;
	CheckEdition(_LIB, STD);
    i = PDFType1GetFontName ( Strm, &n );
    psn = ULStringToAtom ( _LIB, n );
    mfree( _LIB, n);
    if ( !( _DOC->FontList ) )
        _DOC->FontList = ULListCreate ( _LIB, 2 );
    else
        for ( i = 0; i < _DOC->FontList->Size; i++ )
            if ( FE ( _DOC->FontList->FList[i] )->Type == PDFType1Font &&
                FE ( _DOC->FontList->FList[i] )->FontInfo.T1Info->FontName == psn ){
                    return i;
                };
    inf = ( PDFFontInfo * ) mmalloc ( _LIB, sizeof ( PDFFontInfo ) );
    memset ( inf, 0, sizeof ( PDFFontInfo ) );
    PDFTRY ( _LIB ){
        idx = ULListAdd ( _DOC->FontList, inf );
        inf->Lib = _LIB;
        inf->Type = PDFType1Font;
        inf->FontInfo.T1Info = ( PDFType1Info * ) mmalloc ( _LIB, sizeof ( PDFType1Info ) );
        inf->FontInfo.T1Info->FontName = psn;
        PDFType1GetFontInfo (_LIB, Strm, & ( inf->metrics ), &l1, &l2, &l3 );
	} PDFEXCEPT ( _LIB ){
		mfree ( _LIB, inf );
		PDFRERAISE ( _LIB );
	}
	PDFTRYEND ( _LIB );

    inf->FontInfo.T1Info->FontFile = obj = CosStreamNew ( Doc, true, 3 );
    ULStreamSetPosition ( Strm, 0 );
    ULStreamCopyToStream ( Strm, _CosStreamValue ( obj ) );
    CosStreamFlatePack ( _LIB, obj );
 /*   _CosDictAppend ( obj, Length, CosIntNew ( Doc, false,  ULStreamGetSize ( _CosStreamValue ( obj ) ) ) );*/
    _CosDictAppend ( obj, Length, CosIntNew ( Doc, false,  ULStreamGetSize ( Strm ) ) );
    _CosDictAppend ( obj, Length1, CosIntNew ( Doc, false, l1 ) );
    _CosDictAppend ( obj, Length2, CosIntNew ( Doc, false, l2 ) );
    _CosDictAppend ( obj, Length3, CosIntNew ( Doc, false, l3 ) );
    inf->FontInfo.T1Info->FontDescriptor = obj = CosDictNew(Doc, true, 10);
    _CosDictAppend ( obj, Type, _CosNameNew ( FontDescriptor ) );
    _CosDictAppend ( obj, FontName, CosNameNew ( Doc, false, psn ) );
    _CosDictAppend ( obj, Flags, CosIntNew ( Doc, false, inf->metrics.Flags ) );
    _CosDictAppend ( obj, FontBBox, w = CosArrayNew (Doc, false, 4 ) );
    CosArrayAppend ( w, CosIntNew (Doc, false, inf->metrics.FontBBox.XMin ) ) ;
    CosArrayAppend ( w, CosIntNew (Doc, false, inf->metrics.FontBBox.YMin ) ) ;
    CosArrayAppend ( w, CosIntNew (Doc, false, inf->metrics.FontBBox.XMax ) ) ;
    CosArrayAppend ( w, CosIntNew (Doc, false, inf->metrics.FontBBox.YMax ) ) ;
    _CosDictAppend ( obj, ItalicAngle, CosIntNew ( Doc, false, inf->metrics.ItalicAngle ) );
    _CosDictAppend ( obj, Ascent, CosIntNew ( Doc, false, inf->metrics.Ascent ) );
    _CosDictAppend ( obj, Descent, CosIntNew ( Doc, false, inf->metrics.Descent ) );
    _CosDictAppend ( obj, CapHeight, CosIntNew ( Doc, false, inf->metrics.CapHeight ) );
    _CosDictAppend ( obj, StemV, CosIntNew ( Doc, false, inf->metrics.StemV ) );
    if ( !_IsCosNull ( inf->FontInfo.T1Info->FontFile ) )
        _CosDictAppend ( obj, FontFile, inf->FontInfo.T1Info->FontFile );
    
    
    inf->FontInfo.T1Info->Font= obj = CosDictNew ( Doc, true, 7 );
    _CosDictAppend ( obj, Type, _CosNameNew ( Font ) );
    _CosDictAppend ( obj, Subtype, _CosNameNew ( Type1 ) );
    _CosDictAppend ( obj, BaseFont, CosNameNew ( Doc, false, psn ) );
    _CosDictAppend ( obj, FirstChar, CosIntNew ( Doc, false, 0 ) );
    _CosDictAppend ( obj, LastChar, CosIntNew ( Doc, false, 255 ) );
    _CosDictAppend ( obj, FontDescriptor, inf->FontInfo.T1Info->FontDescriptor );
    _CosDictAppend ( obj, Widths, w = CosArrayNew ( Doc, false, 256 ) );
    for ( i=0; i<256; i++ )
        CosArrayAppend( w, CosIntNew ( Doc, false, inf->metrics.ANSICodes[i] ) );
    return idx;
}
#endif
