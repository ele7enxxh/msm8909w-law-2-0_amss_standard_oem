#ifndef _PS_DNSI_H
#define _PS_DNSI_H
/*===========================================================================

                           P S _ D N S I . H
                   
DESCRIPTION
  This file contains internal definitions of data structures and utilities
  for manipulating domain names.

Copyright (c) 2005-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/inc/ps_dnsi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/15   ds      Fix to handle expansion of DNS names accordingly as both v4 
                   v6 have different encoding techniques.
07/25/13   cx      Changed prototype of ps_dnsi_get_bytes_f_type due to 
                   replacement of memcpy
05/10/06   es      Added necessary defines for max lengths, ptr_val_mask.
09/15/05   ifk     Created module.
===========================================================================*/
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


/* The following defines are defined in RFC 1035 */
#define DNS_MAX_LABEL_LEN          (63)      /* Max labellen */
#define DNS_MAX_DOMAIN_NAME_OCTETS (255)     /* Max octets */

/* mask to obtain pointer value from word */
#define DNS_LABEL_PTR_VAL_MASK     ((uint32)~0xffffc000) 
/* mask to check top two bits in byte for domain name ptr format */
#define DNS_LABEL_PTR_MASK         ((uint8)0xc0)    

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION TYPEDEF PS_DNSI_GET_BYTES_F_TYPE 

DESCRIPTION
  Function pointer to copy num_bytes at a particular offset from an opaque
  source buffer to a passed buffer of contiguous memory.

DEPENDENCIES
  None.

RETURN VALUE
   0 in case of success, with the copied bytes in dst_buf
  -1 in case of error 

SIDE EFFECTS
  None.
===========================================================================*/
typedef int (*ps_dnsi_get_bytes_f_type)
(
  void   *src_buf,           /* Buffer to get bytes from                   */
  uint32  offset_src_buf,    /* Offset into the source buffer              */
  char   *dst_buf,           /* Buffer to copy bytes to                    */
  uint16  dst_buf_size,       /* Destination Buffer size                   */
  uint32  num_bytes          /* Number of bytes to get                     */
);


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION PS_DNSI_DOMAIN_NAME_EXPAND

DESCRIPTION
  The function expands a passed domain name encoded in the compressed domain
  name format (RFC 1035 Section 4.1.4). It is passed an opaque buffer, buf, 
  which contains the compressed domain name along with an offset argument 
  giving offset of the compressed name in buf. 
  A function pointer, get_f, is passed to get bytes from the opaque buffer.  
  This assumes the DNS message format defined in RFC 1035 Section 4.1, but 
  may be used to expand compressed domain names messages with pointers if 
  offset values are accurate.
  
  The expanded name is copied into the dst_buf argument if non-NULL.
  The length of the expanded name is returned in name_len if non-NULL. 
  dst_buf_len is used to pass in the length of the destination buffer.  
  The offset in the opaque buffer past the compressed domain name is 
  returned as the return value of the function.  
  A return value of -1 indicates an error.

DEPENDENCIES
  dst_buf and name_len must not both be NULL.

PARAMETERS
  buf          - start of buffer holding compressed domain name format. 
  offset       - offset of domain name in buf. RFC 1035 defines this 
                 offset to be from the ID field in the DNS header.
  dst_buf      - buffer for storing expanded name
  dst_buf_len  - length of destination buffer for space check
  name_len     - pointer to var stored with length of expanded domain name
  get_f        - function to extract bytes from buf
  
RETURN VALUE
  retval   - Offset into the opaque buffer after the domain name, -1 on error
  dst_buf  - Holds the expanded domain name.
  name_len - Set to the length of the expanded domain name.

SIDE EFFECTS
  Expands the passed compressed name copying it into the passed dst_buf
  argument with the length of the expanded name returned in name_len if
  name_len argument is non-NULL.
===========================================================================*/
int ps_dnsi_domain_name_expand
(
  void                     *buf,     /* Buffer with compressed domain name */
  uint32                    offset,  /* Offset of domain name in buf       */
  char                     *dst_buf, /* Buffer for storing expanded name   */
  uint32                    buf_len, /* Length of destination buffer       */
  uint32                   *name_len,/* Length of expanded domain name     */
  ps_dnsi_get_bytes_f_type  get_f,   /* Function to get bytes from buf     */
  boolean                   ip_ver_flag /* IP version flag 0-v4, 1-v6     */
);

/*===========================================================================
FUNCTION PS_DNSI_ADD_TRAILING_DOT_TO_HOSTNAME

DESCRIPTION
  The function adds a trailing dot to a hostname string unless the hostname
  matches one of the following conditions:

  1. is an IPv4 or IPv6 numerical address

  2. already has a trailing dot 

DEPENDENCIES
  hostname must not be NULL.
  hostname must not be of length DSS_DNS_MAX_DOMAIN_NAME_LEN (validate using,
    for example, dss_dnsi_validate_hostname_query())
  hostname must be allocated with space for an extra character - change is
    done in place.

PARAMETERS
  hostname - the hostname string to check.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_dnsi_add_trailing_dot_to_hostname
(
  char *hostname
);

#endif  /* _PS_DNSI_H */
