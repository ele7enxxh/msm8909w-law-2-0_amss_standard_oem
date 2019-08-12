/*===========================================================================

                D S S   D N S   U T I L I T I E S   M O D U L E

DESCRIPTION

  The Domain Name Services utility module. Contains definitions of
  DNS address utility functions exported by the DNS API.

EXTERNALIZED FUNCTIONS
  dss_inet_aton()
    Given a string with an IPv4 address in dot format returns the host
    address in network order.

  dss_inet_ntoa()
    Given an IPv4 address returns it in dot string format.

  dss_inet_pton()
    Given an IPv4 or IPv6 address in presentation format, converts
    to ps_in_addr or ps_in6_addr.

  dss_inet_ntop()
    Given an IPv4 or IPv6 address in network format(ps_in_addr or ps_in6_addr)
    converts to dotted decimal(for IPv4) or colon seperated(for IPv6)
    presentation.

Copyright (c) 2007-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/dss_dns_utils.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   hm      Moved from dssdns.c to here.

===========================================================================*/

#include "comdef.h"
#include "customer.h"


#include "dssdns.h"
#include "ps_dnsi_defs.h"

#include <stringl/stringl.h>
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                               EXTERNAL FUNCTIONS

===========================================================================*/
/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*===========================================================================
FUNCTION  DNSI_INET_NTOP4()

DESCRIPTION
  Converts an IPv4 network (binary) format address into its corresponding
  presentation (ASCII or printable) format.  The converted address is
  returned in the dst argument.

DEPENDENCIES
  The dst argument should have enough memory to contain a presentation format
  IPv4 address.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument
  DSS_ERROR   in case of error with the error code returned in the dss_errno
              argument

SIDE EFFECTS
  Returns the converted printable format IPv4 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
static int32 dnsi_inet_ntop4
(
  const char *src,
  char       *dst,
  uint32      size,
  int16      *dss_errno
)
{
  int32 ret;
  int32 retval = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret = snprintf(dst, size, "%u.%u.%u.%u", (unsigned char)src[0],
                                                 (unsigned char)src[1],
                                                 (unsigned char)src[2], 
                                                 (unsigned char)src[3]);

  if (ret >= (int32)size)
  {
    LOG_MSG_INFO2_2("dnsi_inet_ntop4(): Require %d octets for conversion have %d "
                    "octets.  Returning DS_EMSGTRUNC", ret, size );
    *dss_errno = DS_EMSGTRUNC;
    retval = DSS_ERROR;
  }
  else if (ret == -1)
  {
    LOG_MSG_INFO2_0("dnsi_inet_ntop4(): Failed to convert address.  Returning "
                    "DS_ENORECOVERY" );
    *dss_errno = DS_ENORECOVERY;
    retval = DSS_ERROR;
  }

  return retval;
} /* dnsi_inet_ntop4() */


/*===========================================================================
FUNCTION  DNSI_INET_NTOP6()

DESCRIPTION
  Converts an IPv6 network (binary) format address into its corresponding
  presentation (ASCII or printable) format.  The converted address is
  returned in the dst argument.

DEPENDENCIES
  The dst argument should have enough memory to contain a presentation format
  IPv6 address.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument
  DSS_ERROR   in case of error with the error code returned in the dss_errno
              argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
static int32 dnsi_inet_ntop6
(
  const char    *src,
  char          *dst,
  unsigned int   size,
  int16         *dss_errno
)
{

  char   tmp[PS_IN6_ADDRSTRLEN];
  char  *tp;
  char  *endp = tmp + PS_IN6_ADDRSTRLEN;
  struct
  {
    int base;
    int len;
  } best, cur;
  uint16 words[sizeof (struct ps_in6_addr) / sizeof (int16)];
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Preprocess:
      Copy the input (bytewise) array into a wordwise array.
      Find the longest run of 0x00's in src[] for :: shorthanding.
  ------------------------------------------------------------------------*/
  memset(words, '\0', sizeof words);
  for (i = 0; i < (int)sizeof(struct ps_in6_addr); i++)
  {
    words[i / 2] |= (uint16)(src[i] << ((1 - (i % 2)) << 3));
  }
  best.base = -1;
  best.len = 0;
  cur.base = -1;
  cur.len = 0;
  for (i = 0; i < (int)(sizeof(struct ps_in6_addr) / sizeof(int16)); i++)
  {
    if (words[i] == 0)
    {
      if (cur.base == -1)
      {
        cur.base = i;
        cur.len = 1;
      }
      else
      {
        cur.len++;
      }
    }
    else
    {
      if (cur.base != -1)
      {
        if (best.base == -1 || cur.len > best.len)
        {
          best = cur;
        }
        cur.base = -1;
      }
    }
  }

  if (cur.base != -1)
  {
    if (best.base == -1 || cur.len > best.len)
    {
      best = cur;
    }
  }

  if (best.base != -1 && best.len < 2)
  {
    best.base = -1;
  }

  /*-------------------------------------------------------------------------
    Format the result.
  -------------------------------------------------------------------------*/
  tp = tmp;
  for (i = 0; i < (int)(sizeof(struct ps_in6_addr) / sizeof(int16)); i++)
  {
    /*----------------------------------------------------------------------
      Are we inside the best run of 0x00's?
    ----------------------------------------------------------------------*/
    if (best.base != -1 && i >= best.base &&
        i < (best.base + best.len))
    {
      if (i == best.base)
      {
        *tp++ = ':';
      }
      continue;
    }

    /*----------------------------------------------------------------------
      Are we following an initial run of 0x00s or any real hex?
    ----------------------------------------------------------------------*/
    if (i != 0)
    {
      *tp++ = ':';
    }

    /*----------------------------------------------------------------------
      Is this address an encapsulated IPv4?
    ----------------------------------------------------------------------*/
    if (i == 6 && best.base == 0 &&
        (best.len == 6 ||
         (best.len == 7 && words[7] != 0x0001) ||
         (best.len == 5 && words[5] == 0xffff)))
    {
      if (dnsi_inet_ntop4(src+12, tp, sizeof tmp - (uint32)(tp - tmp),
                                                    dss_errno) == DSS_ERROR)
      {
        LOG_MSG_INFO2_0("dnsi_inet_ntop6(): Failed to convert v4-mapped-v6 address." );
        return DSS_ERROR;
      }
      tp += strlen(tp);
      break;
    }

    if ((endp - tp) < (int)sizeof(uint32))
    {
      LOG_MSG_INFO2_2("dnsi_inet_ntop6(): Insufficient memory, need %d octets, have "
                      "%d octets.  Returning DS_EMSGTRUNC",
                      sizeof(uint32), endp - tp );
      *dss_errno = DS_EMSGTRUNC;
      return DSS_ERROR;
    }

    tp += snprintf(tp, (unsigned int)(endp - tp), "%x", words[i]);
  }

  /*------------------------------------------------------------------------
    Was it a trailing run of 0x00's?
  ------------------------------------------------------------------------*/
  if (best.base != -1 && (best.base + best.len) ==
      (int)(sizeof(struct ps_in6_addr) / sizeof(int16)))
  {
    *tp++ = ':';
  }
  *tp++ = '\0';

  /*------------------------------------------------------------------------
    Check for overflow, copy, and we're done.
  ------------------------------------------------------------------------*/
  if ((tp - tmp) > (int)size)
  {
    LOG_MSG_INFO2_2("dnsi_inet_ntop6(): Insufficient memory, need %d octets, have "
                    "%d octets.  Returning DS_EMSGTRUNC", tp - tmp, size );
    *dss_errno = DS_EMSGTRUNC;
    return DSS_ERROR;
  }

  (void) strlcpy(dst, tmp, size );

  return DSS_SUCCESS;
} /* dnsi_inet_ntop6() */


/*===========================================================================
FUNCTION  DNSI_INET_PTON4()

DESCRIPTION
  Converts an IPV4 presentation (ASCII or printable) format address into its
  corresponding network (binary) format version.  The converted address is
  returned in the dst argument which should be large enough for the IPv4
  binary format address

DEPENDENCIES
  dst argument should have enough memory to contain an IPv4 binary format
  address

RETURN VALUE
  DSS_SUCCESS in case of success with the binary format address returned in
              the dst argument
  DSS_ERROR   in case of error with the error code returned in the dss_errno
              argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
static int dnsi_inet_pton4
(
  const char    *src,
  unsigned char *dst,
  int16         *dss_errno
)
{
  const char digits[] = "0123456789";
  int saw_digit = FALSE;
  int octets = 0;
  unsigned char ch = 0;
  unsigned char tmp[sizeof(struct ps_in_addr)];
  unsigned char *tp = tmp;
  const char *pch;
  uint32 new_digit;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *tp = 0;

  while ((ch = *src++) != '\0')
  {
    if ((pch = strchr(digits, ch)) != NULL)
    {
      new_digit = (uint32)(*tp * 10 + (pch - digits));

      if (saw_digit && *tp == 0)
      {
        LOG_MSG_INFO2_1("dnsi_inet_pton4(): For octet %d value started with 0.  "
                        "Return DS_NAMEERR", octets );
        *dss_errno = DS_NAMEERR;
        return DSS_ERROR;
      }
      if (new_digit > 255)
      {
        LOG_MSG_INFO2_2("dnsi_inet_pton4(): Value %d greater then 0xff in octet %d.  "
                        "Return DS_NAMEERR", new_digit, octets );
        *dss_errno = DS_NAMEERR;
        return DSS_ERROR;
      }
      *tp = (unsigned char)new_digit;
      if (!saw_digit)
      {
        if (++octets > 4)
        {
          LOG_MSG_INFO2_1("dnsi_inet_pton4(): Number of octets %d greater then 4.  "
                          "Return DS_NAMEERR", octets );
          *dss_errno = DS_NAMEERR;
          return DSS_ERROR;
        }
        saw_digit = 1;
      }
    }
    else
    {
      if (ch == '.' && saw_digit)
      {
        if (octets == 4)
        {
          LOG_MSG_INFO2_0("dnsi_inet_pton4(): '.' found after 4 octets.  Return "
                          "DS_NAMEERR" );
          *dss_errno = DS_NAMEERR;
          return DSS_ERROR;
        }
        *++tp = 0;
        saw_digit = 0;
      }
      else
      {
        LOG_MSG_INFO2_2("dnsi_inet_pton4(): %c found in octet %d no digits seen.  "
                        "Return DS_NAMEERR", ch, octets );
        *dss_errno = DS_NAMEERR;
        return DSS_ERROR;
      }
    }
  }

  if (octets < 4)
  {
    LOG_MSG_INFO2_1("dnsi_inet_pton4(): Only %d octets found. Return DS_NAMEERR",
                    octets );
    *dss_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  memscpy(dst, sizeof(struct ps_in_addr), tmp, sizeof(struct ps_in_addr));
  return DSS_SUCCESS;
} /* dnsi_inet_pton4() */


/*===========================================================================
FUNCTION  DNSI_INET_PTON6()

DESCRIPTION
  This function is called to convert an IPv6 presentation (ASCII) format
  address into its corresponding network (binary) format address.

DEPENDENCIES
  The dst argument should be large enough for an IPv6 address in network
  address format.

RETURN VALUE
  DSS_SUCCESS in case of success with the address returned in the dst
              argument
  DSS_ERROR   in case of error with the error code returned in the dss_errno
              argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
 (1) does not touch `dst' unless it's returning 1.
 (2) :: in a full address is silently ignored.
===========================================================================*/
static int dnsi_inet_pton6
(
  const char    *src,
  unsigned char *dst,
  int16         *dss_errno
)
{
  static const char xdigits_l[] = "0123456789abcdef",
                    xdigits_u[] = "0123456789ABCDEF";
  unsigned char tmp[sizeof(struct ps_in6_addr)], *tp, *endp, *colonp;
  const char *xdigits, *curtok;
  int ch, seen_xdigits;
  unsigned int val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((tp = tmp), '\0', sizeof(struct ps_in6_addr));
  endp = tp + sizeof(struct ps_in6_addr);
  colonp = NULL;

  /*------------------------------------------------------------------------
    Leading :: requires some special handling.
  ------------------------------------------------------------------------*/
  if (*src == ':')
  {
    if (*++src != ':')
    {
      LOG_MSG_INFO2_0("dnsi_inet_pton4(): Address begins with single ':'.  Return "
                      "DS_NAMEERR" );
      *dss_errno = DS_NAMEERR;
      return DSS_ERROR;
    }
  }

  curtok = src;
  seen_xdigits = 0;
  val = 0;

  while ((ch = *src++) != '\0')
  {
    const char *pch;

    if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
    {
      pch = strchr((xdigits = xdigits_u), ch);
    }

    if (pch != NULL)
    {
      val <<= 4;
      val |= (unsigned int)(pch - xdigits);
      if (++seen_xdigits > 4)
      {
        LOG_MSG_INFO2_1("dnsi_inet_pton4(): %d digits seen.  Return DS_NAMEERR",
                        seen_xdigits );
        *dss_errno = DS_NAMEERR;
        return DSS_ERROR;
      }
      continue;
    }

    if (ch == ':')
    {
      curtok = src;
      if (!seen_xdigits)
      {
        if (colonp)
        {
          LOG_MSG_INFO2_0("dnsi_inet_pton4(): Duplicate :: found.  Return DS_NAMEERR" );
          *dss_errno = DS_NAMEERR;
          return DSS_ERROR;
        }
        colonp = tp;
        continue;
      }
      else
      {
        if (*src == '\0')
        {
          LOG_MSG_INFO2_0("dnsi_inet_pton4(): ':' found at end of address.  "
                          "Return DS_NAMEERR" );
          *dss_errno = DS_NAMEERR;
          return DSS_ERROR;
        }
      }
      if (tp + sizeof(int16) > endp)
      {
        LOG_MSG_INFO2_2("dnsi_inet_pton4(): Insufficient memory.  Need %d octets, "
                        "have %d octets.  Return DS_EMSGTRUNC",
                        sizeof(int16), endp - tp );
        *dss_errno = DS_EMSGTRUNC;
        return DSS_ERROR;
      }

      *tp++ = (unsigned char) (val >> 8) & 0xff;
      *tp++ = (unsigned char) val & 0xff;
      seen_xdigits = 0;
      val = 0;
      continue;
    }
    if (ch == '.' && ((tp + sizeof(struct ps_in_addr)) <= endp) &&
        dnsi_inet_pton4(curtok, tp, dss_errno) == DSS_SUCCESS)
    {
      tp += sizeof(struct ps_in_addr);
      seen_xdigits = 0;
      break;  /* '\0' was seen by inet_pton4(). */
    }

    LOG_MSG_INFO2_3("dnsi_inet_pton4():  Found ch %c, need %d octets, have %d "
                    "octets.  Return DS_NAMEERR",
                    ch, sizeof(struct ps_in_addr), endp - tp );
    *dss_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  if (seen_xdigits)
  {
    if (tp + sizeof(int16) > endp)
    {
      LOG_MSG_INFO2_2("dnsi_inet_pton4(): Insufficient memory.  Need %d octets, "
                      "have %d octets.  Return DS_EMSGTRUNC",
                      sizeof(int16), endp - tp );
      *dss_errno = DS_EMSGTRUNC;
      return DSS_ERROR;
    }

    *tp++ = (unsigned char) (val >> 8) & 0xff;
    *tp++ = (unsigned char) val & 0xff;
  }

  if (colonp != NULL)
  {
    /*----------------------------------------------------------------------
      Since some memsmove()'s erroneously fail to handle overlapping
      regions, we'll do the shift by hand.
    ----------------------------------------------------------------------*/
    const int n = tp - colonp;
    int i;

    if (tp == endp)
    {
      LOG_MSG_INFO2_0("dnsi_inet_pton4(): Insufficient memory.  Return DS_EMSGTRUNC" );
      *dss_errno = DS_EMSGTRUNC;
      return DSS_ERROR;
    }

    for (i = 1; i <= n; i++)
    {
      endp[- i] = colonp[n - i];
      colonp[n - i] = 0;
    }
    tp = endp;
  }

  if (tp != endp)
  {
    LOG_MSG_INFO2_2("dnsi_inet_pton4(): Out of %d octets, processed %d octets.  "
                    "Return DS_NAMEERR", endp - tmp, tp - tmp );
    *dss_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  memscpy(dst, sizeof(struct ps_in6_addr), tmp, sizeof(struct ps_in6_addr));
  return DSS_SUCCESS;
} /* dnsi_inet_pton6() */


/*===========================================================================
FUNCTION  DSS_INET_NTOP()

DESCRIPTION
  This function is called with an IPv4 or IPv6 address in network (binary)
  format to be converted into the presentation (dotted decimal for IPv4
  or colon seperated for IPv6) format.  The af argument can be either
  DSS_AF_INET if the address to be converted is an IPv4 address or 
  DSS_AF_INET6 if the address is an IPv6 address.  In case of error the error 
  code is returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the string address.
  To convert an IPv4 address it is suggested to be PS_IN_ADDRSTRLEN
  in size while to convert an IPv6 address it is suggested to be 
  PS_IN6_ADDRSTRLEN.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument
              DS_ENORECOVERY    Failed to convert

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_ntop
(
  const char *src,       /* Memory containing network form IP address      */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory to return address in presentation format*/
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
)
{
  int32 retval = DSS_ERROR;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_inet_ntop(): "
    "caller 0x%p", caller_ptr);

  if (dss_errno == NULL)
  {
    LOG_MSG_INFO2_0( "dss_inet_ntop(): Called with dss_errno NULL" );
    return DSS_ERROR;
  }

  if (src == NULL || dst == NULL)
  {
    LOG_MSG_INFO2_2( "dss_inet_ntop(): Called with src %p, dst %p", src, dst );
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if (dst_size == 0)
  {
    LOG_MSG_INFO2_1( "dss_inet_ntop(): dst_size 0", dst_size );
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  switch (af)
  {
  case DSS_AF_INET:
    retval = dnsi_inet_ntop4(src, dst, dst_size, dss_errno);
    break;

  case DSS_AF_INET6:
    retval = dnsi_inet_ntop6(src, dst, dst_size, dss_errno);
    break;

  default:
    *dss_errno = DS_EAFNOSUPPORT;
    break;
  }

  return retval;
} /* dss_inet_ntop() */


/*===========================================================================
FUNCTION  DSS_INET_PTON()

DESCRIPTION
  This function is called with a presentation (printable or ASCII) format
  address to be converted to its network address (binary) format.  The af
  argument can be either DSS_AF_INET if the address to be converted is an IPv4
  address or DSS_AF_INET6 if the address is an IPv6 address.  In case of error
  the error code is returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the network address
  of the appropriate family.  For IPv4 it should be at least
  sizeof(struct ps_in_addr) while for IPv6 it should be at least
  sizeof(struct ps_in6_addr).

RETURN VALUE
  DSS_SUCCESS in case of success with the network format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_pton
(
  const char *src,       /* String containing presentation form IP address */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory for returning address in network format */
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
)
{
  int32 retval = DSS_ERROR;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_inet_pton(): "
    "caller 0x%p", caller_ptr);

  if (dss_errno == NULL)
  {
    LOG_MSG_INFO2_0( "dss_inet_pton(): Called with dss_errno NULL" );
    return DSS_ERROR;
  }

  if (src == NULL || dst == NULL)
  {
    LOG_MSG_INFO2_2( "dss_inet_pton(): Called with src %p, dst %p", src, dst );
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if ((af == DSS_AF_INET && dst_size < sizeof(struct ps_in_addr)) ||
      (af == DSS_AF_INET6 && dst_size < sizeof(struct ps_in6_addr)))
  {
    LOG_MSG_INFO2_2( "dss_inet_pton(): dst_size %d too small for af %d",
                     dst_size, af );
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  switch (af)
  {
  case DSS_AF_INET:
    retval = dnsi_inet_pton4(src, dst, dss_errno);
    break;

  case DSS_AF_INET6:
    retval = dnsi_inet_pton6(src, dst, dss_errno);
    break;

  default:
    *dss_errno = DS_EAFNOSUPPORT;
    break;
  }

  return retval;
} /* dss_inet_pton() */


/*===========================================================================
FUNCTION  DSS_INET_ATON()

DESCRIPTION
  Convert internet address from dotted string to network ordered struct
  ps_in_addr.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR    For failure
  DSS_SUCCESS  For success

  The IP address is returned in struct ps_in_addr *addr

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_aton
(
  const  char       *cp,
  struct ps_in_addr *addr
)
{
  enum
  {
    BASE_OCT = 8,
    BASE_DEC = 10,
    BASE_HEX = 16
  }         base        = BASE_DEC;
  uint32    index       = 0;
  int32     number      = 0;
  uint32    value       = 0;
  uint32    ipaddr      = 0;
  uint32    check_value = 0;
  uint8    *presult     = (uint8 *) &ipaddr;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("dss_inet_aton(): "
    "caller 0x%p", caller_ptr);

   if( NULL == cp || NULL == addr || 0 == strlen(cp) )
   {
      return DSS_ERROR;
   }

  /*-------------------------------------------------------------------------
    The ASCII string containing the address could be in one of 4 forms:
    i  ) a.b.c.d - each of a,b,c and d is one octet of IPv4 address
    ii ) a.b.c   - a and b are most significant octets, c is remaining 16
                   bits of address.
    iii) a.b     - a is most significant octet while b is remaining 24 bits
    iv ) a       - a is the complete address

    Each of a,b,c and d can be a decimal number, an octal number or a hex
    number.
  -------------------------------------------------------------------------*/
  for(index = 0; index < sizeof(struct ps_in_addr); index++)
  {
   /*------------------------------------------------------------------------
     Determine the base of the number.  If number starts with 0x or 0X then
     base is 16, if the number starts with 0 the next charachter being
     neither x nor X then base 8 else base 10.
    -----------------------------------------------------------------------*/
    base  = BASE_DEC;
    value = 0;
    if( '0' == *cp )
    {
      /*---------------------------------------------------------------------
        Either a hex or an octal number follows or the number is zero
      ---------------------------------------------------------------------*/
      cp++;
      base = ( 'X' == *cp || 'x' == *cp ) ?  BASE_HEX : BASE_OCT;
      if( BASE_HEX == base )
      {
        /*-------------------------------------------------------------------
          Skip the 'x' or 'X'.
        -------------------------------------------------------------------*/
        cp++;
      }
    }

   /*------------------------------------------------------------------------
     Find the rest of the number.
    -----------------------------------------------------------------------*/
    while( '.' != *cp && 0 != *cp )
    {
      number = *cp - '0';
      switch(base)
      {
        case BASE_DEC:
        case BASE_OCT:
          if(number < 0 || number > ((int32)base - 1))
          {
            /*---------------------------------------------------------------
              Illegal number
            ---------------------------------------------------------------*/
            return DSS_ERROR;
          }
          break;

        case BASE_HEX:
          /*-----------------------------------------------------------------
            Convert to decimal.
          -----------------------------------------------------------------*/
          if(number < 0 || number > 9)
          {
            number = *cp - 'a' + 10;
            if (number < 10 || number > 15)
            {
              number = *cp - 'A' + 10;
              if(number < 10 || number > 15)
              {
                return DSS_ERROR;
              }
            }
          }
          break;

        default:
          LOG_MSG_ERROR_1("Invalid base: %d", base );
          return DSS_ERROR;
      } /* switch */

      cp++;

      /*---------------------------------------------------------------------
        Verify that the number can be contained in a 32 bit quantity.
      ---------------------------------------------------------------------*/
      if( ((int64) value * (int) base) + number > (int64) 0xffffffff )
      {
        return DSS_ERROR;
      }

      value = (value * (int)base) + (uint32)number;
    } /* while */

    /*-----------------------------------------------------------------------
      Each '.' seperated number should be at most 0xff.
    -----------------------------------------------------------------------*/
    if(value > 0xff && *cp != 0)
    {
      return DSS_ERROR;
    }
    else
    {
      if( 0 != *cp )
      {
        *presult++ = (uint8)value;
      }
      else
      {
        /*-------------------------------------------------------------------
          Verify that the last number is in the correct range.
          -------------------------------------------------------------------*/
        switch( index )
        {
          case 0:
            check_value = 0xffffffffU;
            break;

          case 1:
            check_value = 0xffffff;
            break;

          case 2:
            check_value = 0xffff;
            break;

          case 3:
            check_value = 0xff;
            break;

          default:
            return DSS_ERROR;
        } /* switch */

        if( value > check_value )
        {
          return DSS_ERROR;
        }

        ipaddr |= dss_htonl(value);
      }
    }

    /*-----------------------------------------------------------------------
      Break if this was the last number.
    -----------------------------------------------------------------------*/
    if(*cp++ == 0)
    {
      break;
    }
  } /* for */

  addr->ps_s_addr = ipaddr;

  return DSS_SUCCESS;
} /* dss_inet_aton() */


/*===========================================================================
FUNCTION  DSS_INET_NTOA()

DESCRIPTION
  Convert network order internet address to dotted string form.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS   If successful
  DSS_ERROR     If an error occured

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_ntoa
(
  const struct ps_in_addr  in,     /* IPv4 address to be converted         */
  uint8                   *buf,    /* Buffer to hold the converted address */
  int32                    buflen  /* Length of buffer                     */
)
{
  uint8 *paddr  = (uint8*) &in.ps_s_addr;
  int32  retval = DSS_SUCCESS;
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("dss_inet_ntoa(): "
    "caller 0x%p", caller_ptr);

  if( NULL == buf || 0 >= buflen )
  {
    retval = DSS_ERROR;
  }
  else
  {
    if( -1 == snprintf( (char*)buf,
                        (unsigned int)buflen, "%d.%d.%d.%d",
                        paddr[0],
                        paddr[1],
                        paddr[2],
                        paddr[3] )
      )
    {
      retval = DSS_ERROR;
    }
  }

  return retval;
} /* dss_inet_ntoa() */


