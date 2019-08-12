#ifndef PS_PROTO_UTILS_H
#define PS_PROTO_UTILS_H

/*===========================================================================

                   P S _ P R O T O _ U T I L S . H

DESCRIPTION
  Protocols Generic Utility Functions.

EXTERNALIZED FUNCTIONS
 NONE

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_proto_utils.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_proto_utils.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when          who     what, where, why
--------    ---     ----------------------------------------------------------
11/14/13     ds     Adding support to read the ICMP tokens parameter values for 
                           both IPv4 and IPv6.
===========================================================================*/

#include "customer.h"       /* Customer Specific Features */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*==========================================================================
   Function Prototypes
===========================================================================*/
void icmp_read_tokens_nv
(
  void
);

/*===========================================================================
FUNCTION PS_PROTO_UTILS_ENCODE_DOMAIN_NAME()

DESCRIPTION
  This function encodes a domain name as per RFC 1035 Section 3.1

  3. DOMAIN NAME SPACE AND RR DEFINITIONS

  3.1. Name space definitions
  
    Domain names in messages are expressed in terms of a sequence of labels.
    Each label is represented as a one octet length field followed by that
    number of octets.  Since every domain name ends with the null label of
    the root, a domain name is terminated by a length byte of zero.  The
    high order two bits of every length octet must be zero, and the
    remaining six bits of the length field limit the label to 63 octets or
    less.
    
    To simplify implementations, the total length of a domain name (i.e.,
    label octets and label length octets) is restricted to 255 octets or
    less.
    
    Although labels can contain any 8 bit values in octets that make up a
    label, it is strongly recommended that labels follow the preferred
    syntax described elsewhere in this memo, which is compatible with
    existing host naming conventions.  Name servers and resolvers must
    compare labels in a case-insensitive manner (i.e., A=a), assuming ASCII
    with zero parity.  Non-alphabetic codes must match exactly.

DEPENDENCIES
  None.

SIDE EFFECTS
  One extra byte would be used beyond the domain_name_len specified and the 
  corresponding memory location beyond the domain_name_ptr would be updated.

PARAMETERS
  domain_name_ptr[in/out] : Character buffer pointer holding the domain name.
  domain_name_len[in/out] : Length of the null terminated domain name, 
                            excluding the NULL character. This will be 
                            updated with length of encoded domain name.

RETURN VALUE
  0  : for success
  -1 : for failure
===========================================================================*/
int32
ps_proto_utils_encode_domain_name
(
  uint8  *domain_name_ptr, /* NULL terminated domain name. */
  uint32 *domain_name_len  /* Excluding the NULL character at end of domain 
                              name. Will be updated with encoded domain name
                              length. */
);

#endif  /* PS_PROTO_UTILS_H */
