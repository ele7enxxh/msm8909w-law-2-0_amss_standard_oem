/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file jtypes.h

Holds definitions for general types used by Jungo's USB stack.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jtypes.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.

    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
**************************************************************************/ 

#ifndef _JTYPES_H_
#define _JTYPES_H_


/** @ingroup hsusb_jungo_api 
@{ */

/* Generic flags */
#define BUFFER_IS_DMA_ABLE      0x1

/* Basic types definitions */
typedef unsigned long           juint32_t;
typedef signed long             jint32_t;
typedef unsigned short          juint16_t;
typedef signed short            jint16_t;
typedef unsigned char           juint8_t;

/* HSU TempFix START */
/* This fix will be included in the next distribution of the stack.
 * It is needed because in ARM the default of char is unsigned.
 * It solves an issue in HID where negative logical_min values are not reported
 * correctly. */
typedef signed char             jint8_t;
/* HSU TempFix END */

typedef unsigned int            juint_t;
typedef signed int              jint_t;
typedef signed int              jresult_t;
typedef int                     jbool_t;

/** Implementation for the general byte order swapping function (for word).
*/
juint16_t __bswap16(juint16_t _x);

/** Implementation for the general byte order swapping function (for dword).
*/
juint32_t __bswap32(juint32_t _x);

/** LSB is first: i386, vax.
*/
#define CPU_LITTLE_ENDIAN       1234

/** MSB is first: 68000, ibm, net.
*/
#define CPU_BIG_ENDIAN          4321

/** LSB is first in a word; MSW is first in long.
*/
#define CPU_PDP_ENDIAN          3412

/** API for the general byte order swapping function (for word).
*/
#define bswap16(x)      __bswap16(x)

/** API for the general byte order swapping function (for dword).
*/
#define bswap32(x)      __bswap32(x)


/*  Unaligned buffer access macros.
 *  
 *  The following macros perform ** BOTH ** unaligned access and
 *  endianity conversion where necessary.
 *  For unaligned access WITHOUT affecting endianity, use the
 *  ua_get() and ua_set() macros below.
 *  
 *  the 'le' and 'be' notation indicates the endianity of
 *  the BUFFER being READ from ('xxxtoh') or
 *  the BUFFER being WRITTEN to ('htoxxx').
 *  The 'h' (i.e. host order) is implied and is automatically
 *  adjusted for by the macros.
 *
 *  e.g, reading a uint16 from a little endian packed buffer
 *  use the ua_le16toh() macro. for writing a uint32 to a big
 *  endian packed buffer, use the ua_htobe32() macro.
 */

/** Little endian unaligned buffers: READ a value from an unaligned buffer, and 
    CONVERT it to host order.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_le16toh(ptr)   \
    (((juint8_t *)(ptr))[0] | (((juint8_t *)(ptr))[1] << 8))

/** Little endian unaligned buffers: CONVERT a value to little endian, and
    WRITE it to an unaligned buffer.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_htole16(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)(v), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 8))

/** Little endian unaligned buffers: READ a value from an unaligned buffer, and 
    CONVERT it to host order.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_le32toh(ptr) \
    ((juint32_t)(((juint8_t *)(ptr))[0]) | \
    ((juint32_t)(((juint8_t *)(ptr))[1]) << 8) | \
    ((juint32_t)(((juint8_t *)(ptr))[2]) << 16) | \
    ((juint32_t)(((juint8_t *)(ptr))[3]) << 24))

/** Little endian unaligned buffers: CONVERT a value to little endian, and
    WRITE it to an unaligned buffer.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_htole32(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)(v), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[2] = (juint8_t)((v) >> 16), \
    ((juint8_t *)(ptr))[3] = (juint8_t)((v) >> 24))

/** Big endian unaligned buffer: READ a value from an unaligned buffer, and 
    CONVERT it to host order.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_be16toh(ptr)     \
    ((((juint8_t *)(ptr))[0] << 8) | ((juint8_t *)(ptr))[1])

/** Big endian unaligned buffer: CONVERT a value to big endian, and WRITE it to
    an unaligned buffer.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_htobe16(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[1] = (juint8_t)(v))

/** Big endian unaligned buffer: READ a value from an unaligned buffer, and 
    CONVERT it to host order.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_be32toh(ptr) \
    (((juint32_t)(((juint8_t *)(ptr))[0]) << 24) | \
    ((juint32_t)(((juint8_t *)(ptr))[1]) << 16) | \
    ((juint32_t)(((juint8_t *)(ptr))[2]) << 8) | \
    ((juint32_t)(((juint8_t *)(ptr))[3])))

/** Big endian unaligned buffer: CONVERT a value to big endian, and WRITE it to
    an unaligned buffer.
@note The endianity conversion to and from host order is automatic, i.e., CPU
      ENDIANITY IS IRRELEVANT. It is the BUFFER's endianity that is important.
*/
#define ua_htobe32(ptr,v) \
    (((juint8_t *)(ptr))[0] = (juint8_t)((v) >> 24), \
    ((juint8_t *)(ptr))[1] = (juint8_t)((v) >> 16), \
    ((juint8_t *)(ptr))[2] = (juint8_t)((v) >> 8), \
    ((juint8_t *)(ptr))[3] = (juint8_t)(v))


/* 
* Host to big endian, host to little endian, big endian to host, and little
* endian to host byte order functions as detailed in byteorder(9).
*/

#define UGETW(ptr)        ua_le16toh(ptr)    /**< Backward compatibility macro. */
#define USETW(ptr,v)      ua_htole16(ptr,v)  /**< Backward compatibility macro. */
#define UGETDW(ptr)       ua_le32toh(ptr)    /**< Backward compatibility macro. */
#define USETDW(ptr,v)     ua_htole32(ptr,v)  /**< Backward compatibility macro. */

/** Convert a value to (void *).
*/
#define htobe16_t(ptr, x) \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  0 & 0xff);

/** Convert a value to (void *).
*/
#define htobe32_t(ptr, x) \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >> 24 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >> 16 & 0xff); \
        ((juint8_t *)(ptr))[2] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[3] = (juint8_t)((x) >>  0 & 0xff);

/** Convert a value to (void *).
*/
#define htole16_t(ptr, x) \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  0 & 0xff);

/** Convert a value to (void *).
*/
#define htole32_t(ptr, x) \
        ((juint8_t *)(ptr))[3] = (juint8_t)((x) >> 24 & 0xff); \
        ((juint8_t *)(ptr))[2] = (juint8_t)((x) >> 16 & 0xff); \
        ((juint8_t *)(ptr))[1] = (juint8_t)((x) >>  8 & 0xff); \
        ((juint8_t *)(ptr))[0] = (juint8_t)((x) >>  0 & 0xff);

/** Convert (void *) to a value.
*/
#define be16toh_t(ptr) be16toh(ua_get16(ptr))

/** Convert (void *) to a value.
*/
#define be32toh_t(ptr) be32toh(ua_get32(ptr))

/** Convert (void *) to a value.
*/
#define le16toh_t(ptr) le16toh(*(juint16_t *)(ptr))

/** Convert (void *) to a value.
*/
#define le32toh_t(ptr) le32toh(*(juint32_t *)(ptr))

/** Add milliseconds (ms) to the time structure.
*/
#define TIME_ADD_MS(time, ms) \
        do { \
        (time)->tv_usec += ms * 1000; \
        (time)->tv_sec  += ((time)->tv_usec / (1000 * 1000)); \
        (time)->tv_usec %= (1000 * 1000); \
        } while (0);

/** Check if time a is greater than time b.
*/
#define TIME_OVER(a, b) \
        (((a).tv_sec > (b).tv_sec) || \
        (((a).tv_sec == (b).tv_sec) && \
        ((a).tv_usec >= (b).tv_usec)) ? \
        1 : 0)

/** Check if time a is greater than time b with a resolution in milliseconds.
*/
#define TIME_OVER_MS(a, b) \
        ((TIME_DIFF(a, b) > 0) ? 0 : 1) 

/** Return the time difference between start and end in milliseconds. 
*/
#define TIME_DIFF(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000 + \
        ((end).tv_usec - (start).tv_usec) / 1000)

/** Return value in milliseconds of a given time structure.
*/
#define TIME_GET_MS(time) \
        (((time).tv_sec) * 1000 + ((time).tv_usec) / 1000)
   
/** Greater than the operator for the time structure.
*/
#define jtime_gt(t1, t2) \
    ((t1)->tv_sec > (t2)->tv_sec || \
        ((t1)->tv_sec == (t2)->tv_sec && (t1)->tv_usec > (t2)->tv_usec))

/** Less than the operator for the time structure.
*/
#define jtime_lt(t1, t2) \
    ((t1)->tv_sec < (t2)->tv_sec || \
        ((t1)->tv_sec == (t2)->tv_sec && (t1)->tv_usec < (t2)->tv_usec))

/** Greater than or equal to the operator for the time structure.
*/
#define jtime_egt(t1, t2) (!jtime_lt(t1, t2))

/** Less than or equal to the operator for the time structure.
*/
#define jtime_elt(t1, t2) (!jtime_gt(t1, t2))

/** @} */ /* end_group hsusb_jungo_api */

#endif
