#ifndef AEESTDINET_H
#define AEESTDINET_H
/*======================================================
        Copyright (c) 2006 - 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential 
========================================================

DESCRIPTION:
   Utility functions for networking-related uses.

======================================================*/
#include "ds_Utils_StdDef.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

extern boolean       std_inet_aton(const char *psz, unsigned long *pulAddr);
extern int           std_inet_ntoa(unsigned long ulAddr, char *pc, int nlen);

extern const char*   std_inet_ntop(int af, const void *src, char *dst, int size);
extern int           std_inet_pton(int af, const char *src, void *dst);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*
=======================================================================
   HELPER DOCUMENTATION
=======================================================================
std_inet_aton()

Description:

   A helper function that parses an IPv4 numeric internet address from a string. 
   The following formats are supported:
===pre>
       8.8.8.8  : four 8-bit numbers
       8.8.16   : three numers, the last of which is 16-bits
       8.24     : two numbers (the last 24-bits)
       32       : one 32-bit number
===/pre>

   Each number must be unsigned, must not overflow its bounds, and can be 
   specified as decimal, hexadecimal (if it begins with "0x" or "0X"), or 
   octal (initial "0" without an "x").

   Valid terminating characters are ASCII null, space, tab, newline, carriage 
   return, and the following punctuation characters:
===pre>
          '/', ':', ',', '#', '?', and '%'
===/pre>

   Any other terminating character is considered an error.

Prototype:

   boolean std_inet_aton(const char *cpsz, unsigned long *pulAddr);

Parameters:

   cpsz: A NULL-terminated string representation.  The std_inet_aton helper 
         function also stops parsing at any character other than "." or 
         numeric characters.
         
   pulAddr [out]: Where to place the parsed address, in network-byte order.

Return Value:
   TRUE if the string could be parsed, FALSE otherwise

Comments:
   The address is placed in the pulAddr parameter in network-byte order.

Side Effects:
   None

See Also:
   std_inet_ntoa()
   std_inet_ntop()
   std_inet_pton()

=======================================================================

std_inet_ntoa()

Description:
   A helper function that converts an IPv4 internet address into a string.

Prototype:

   int std_inet_ntoa(unsigned long ulAddr, char *pc, int nlen);

Parameters:
   ulAddr: The IPv4 address in network-byte order
   pc [out]: The string to write into
   nlen: The size of pc in bytes

Return Value:
   The required length to hold the address.

Comments:
   None

Side Effects:
   None

See Also:
   std_inet_aton()
   std_inet_ntop()
   std_inet_pton()

=======================================================================

std_inet_ntop()

Description:
   A helper function that converts a network format address to presentation 
   format.

Prototype:
   const char * std_inet_ntop(int af, const void *src, char *dst, int size)

Parameters:
   af : Address family of the address being processed.
   src: An address in network format.
   dst: Pointer to a presentation format address.
   size: The size of the src address.

Return Value:
   Pointer to presentation format address (dst), or NULL.

Comments:
   None

Side Effects:
   None

See Also:
   std_inet_aton()
   std_inet_ntoa()
   std_inet_pton()

=======================================================================
std_inet_pton()

Description:
   A helper function that converts from presentation format (which usually means 
   ASCII-printable) to network format (which is usually some kind of 
   binary format).

Prototype:
   int std_inet_pton(int af, const char *src, void *dst)

Parameters:
   af : Address family of the address being processed.
   src: An address in presentation format.
   dst: Pointer to a network format address.

Return Value:
   1: if the address was valid for the specified address family
   0: if the address wasn't valid (dst is untouched in this case)
   (-1): if some other error occurred (dst is untouched in this case, too)

Comments:
   None

Side Effects:
   None

See Also:
   std_inet_aton()
   std_inet_ntoa()
   std_inet_ntop()
   
=======================================================================*/

#endif // AEESTDINET_H
