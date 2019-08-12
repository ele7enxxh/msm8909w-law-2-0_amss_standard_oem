/*
 * Copyright (c) 1997-2009 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/

/**
 * @file asn1CharSet.h
 */
/*  CHANGE LOG */
/*  Date         Init    Description */
/*  */
/* ////////////////////////////////////////////////////////////////////////// */
/* asn1CharSet.h - ASN.1 character set definitions */

#ifndef _ASN1CHARSET_H_
#define _ASN1CHARSET_H_

/* Canonical character set definitions */

#define NUM_ABITS  4
#define NUM_UBITS  4
#define NUM_CANSET \
" 0123456789"

#define PRN_ABITS  8
#define PRN_UBITS  7
#define PRN_CANSET \
" '()+,-./0123456789:=?ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

#define VIS_ABITS  8
#define VIS_UBITS  7
#define VIS_CANSET \
" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]"\
"^_`abcdefghijklmnopqrstuvwxyz{|}~"

#define T61_ABITS  8
#define T61_UBITS  7
#define T61_CANSET \
" !\"%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]"\
"_abcdefghijklmnopqrstuvwxyz"

#define IA5_ABITS  8
#define IA5_UBITS  7
#define IA5_CANSET \
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"\
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"\
" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]"\
"^_`abcdefghijklmnopqrstuvwxyz{|}~"

#define IA5_RANGE1_LOWER 0
#define IA5_RANGE2_LOWER 0x5f

#define GEN_ABITS  8
#define GEN_UBITS  7
#define GEN_CANSET \
"\000\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017"\
"\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037"\
" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"\
"`abcdefghijklmnopqrstuvwxyz{|}~\177\200\201\202\203\204\205\206\207"\
"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237"\
"\240\241\242\243\244\245\246\247\250\251\252\253\254\255\256\257"\
"\260\261\262\263\264\265\266\267\270\271\272\273\274\275\276\277"\
"\300\301\302\303\304\305\306\307\310\311\312\313\314\315\316\317"\
"\320\321\322\323\324\325\326\327\330\331\332\333\334\335\336\337"\
"\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356\357"\
"\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377"

#define BMP_ABITS  16
#define BMP_UBITS  16
#define BMP_FIRST  0
#define BMP_LAST   0xffff

#define UCS_ABITS  32
#define UCS_UBITS  32
#define UCS_FIRST  0
#define UCS_LAST   0xfffffffful

#endif
