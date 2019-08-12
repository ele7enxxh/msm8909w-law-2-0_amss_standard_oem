/*===========================================================================
                   
                             P S _ D N S I . C

DESCRIPTION
 Contains utility functions to manipulate domain names.
 
EXTERNALIZED FUNCTIONS
  ps_dnsi_domain_name_expand()
    Function to expand a compressed encoded domain name.

Copyright (c) 2005-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/ps_dnsi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/15   ds      Fix to handle expansion of DNS names accordingly as both v4 
                   v6 have different encoding techniques.
05/12/15   pg      Fixed QCMAP PCSCF FQDN Address is not getting updated issue.
07/25/09   cx      Change get_f prototype in ps_dnsi_domain_name_expand due to
                   replacement of memcpy
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
05/10/06   es      Fixed bugs in ps_dnsi_domain_name_expand.
09/15/05   ifk     Added module.
===========================================================================*/
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"


#include <stringl/stringl.h>
#include "ps_dnsi.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_in.h"
#include "dssdns.h"
#include "ds_Utils_DebugMsg.h"

#define DNS_CLIENT_SIP_OPTION_DATA_OFFSET (1)
#define DNS_CLIENT_MAX_OPTION_HEADER_LEN (2)

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
.  ip_ver_flag  - IP version flag 0 - v4 and 1 - v6
  
RETURN VALUE
  retval   - Offset into the opaque buffer after the domain name, -1 on error
  dst_buf  - Holds the expanded domain name.
  name_len - Set to the length of the expanded domain name.

SIDE EFFECTS
  Expands the passed compressed name copying it into the passed dst_buf
  argument with the length of the expanded name returned in name_len if
  name_len argument is non-NULL.
===========================================================================*/
int 
ps_dnsi_domain_name_expand
(
  void                       * buf, 
  uint32                       offset, 
  char                       * dst_buf, 
  uint32                       dst_buf_len, 
  uint32                     * name_len, 
  ps_dnsi_get_bytes_f_type     get_f,
  boolean                      ip_ver_flag
)
{
  uint8  label_len = 0;             /* Length of domain name label */
  int    retval = 0;                /* Return value, offset of next domain 
                                     * name in buffer */
  int    octet_count = 0;           /* Count how many label and label len
                                     * octets seen, must not exceed 
                                     * DNS_MAX_DOMAIN_NAME_OCTETS */
    
  /* Used to detect pointer loops */
  uint32 chase_ptr_offset = offset;
  int chase_ptr_toggle = FALSE;
  
  uint8 chase_label_len = 0;
  
  
  /* Sanity check */
  if ( NULL == buf )
  {
    LOG_MSG_INFO1_0("source buf NULL");
    return -1;
  }
  
  if ( NULL == get_f )
  {
    LOG_MSG_INFO1_2("get_f func ptr NULL", buf, get_f);
    return -1;
  }
  
  if ( NULL == dst_buf && NULL == name_len )
  {
    LOG_MSG_INFO1_0("Both dst_buf and name_len NULL");
    return -1;
  }

  /* initialize name_len to 0 */
  if ( NULL != name_len )
  {
    *name_len = 0;
  }

  /* Loop over the domain name till have a valid name. */
  for(;;)
  {
    /* Get the label length */
    if ( -1 == get_f( buf, offset, (char *)&label_len,sizeof(label_len), sizeof(uint8)) )
    {
      LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): label length missing" );
      retval = -1;
      break;
    }

    /* Read a zero label len, end of domain-name condition */
    if ( 0 == label_len )
    {
      break;
    }

    /* Pointer case. A pointer is detected if the most significant
     * two bits of the label_len are one. The remaining fourteen bits 
     * are the pointer. Only one pointer is allowed in a domain name,
     * see RFC 1035 4.1.4 */
    if ((label_len & DNS_LABEL_PTR_MASK) == DNS_LABEL_PTR_MASK ) 
    
    {
      /* Save offset past the domain name as location of next domain name.  
       * We need to save offset before following the label pointer. */
      if ( 0 == retval )
      {
        retval = offset + sizeof(uint16);
      }
      
      /* get label len pointer */
      if ( -1 == get_f( buf, offset, (char *)&offset,sizeof(offset), sizeof(uint16)) )
      {
        LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): label ptr missing");
        retval = -1;
        break;
      }
      /* set current offset to pointer value */
      offset = ps_ntohs(offset);
      offset &= DNS_LABEL_PTR_VAL_MASK; 
      /* Update the offset in case of v4 only. */
      if(!ip_ver_flag)
      {
        offset += DNS_CLIENT_SIP_OPTION_DATA_OFFSET + DNS_CLIENT_MAX_OPTION_HEADER_LEN;       
      }
      /* move chase_pointer to next pointer */ 
      if ( TRUE == chase_ptr_toggle )
      {
        for (;;)
        {
          if ( -1 == get_f(buf, chase_ptr_offset, 
                           (char *)&chase_label_len,sizeof(chase_label_len), sizeof(uint8)) )
          {
            LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): chase label length missing" );
            return -1;
          }
          /* chase pointer should not reach end of domain_name */
          if ( 0 == chase_label_len )
          {
            LOG_MSG_ERROR_0("ps_dnsi_domain_name_expand(): chase label length is 0" );
            retval = -1;
            break;
          }
          /* skip labels */
          if ((chase_label_len & DNS_LABEL_PTR_MASK) != DNS_LABEL_PTR_MASK )
          {
            chase_ptr_offset += chase_label_len + sizeof(uint8);
          }
          else
          {
            /* follow chase pointer */
            if ( -1 == get_f( buf, chase_ptr_offset, 
                             (char *)&chase_ptr_offset,sizeof(chase_ptr_offset), sizeof(uint16)) )
            {
              LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): label ptr missing");
              retval = -1;
              break;
            }
            chase_ptr_offset = ps_ntohs(chase_ptr_offset);
            chase_ptr_offset &= DNS_LABEL_PTR_VAL_MASK;
            break;
          }
        }

        if ( offset == chase_ptr_offset )
        {
          LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): pointer loop detected "
                          "in domain name list");
          retval = -1;
          break;
        }
      }
      /* only move chase pointer every other pointer seen */
      chase_ptr_toggle = !chase_ptr_toggle;
    } 
    else /* Non-pointer label len case */
    {
      /* verify label len if not pointer */
      if ( DNS_MAX_LABEL_LEN < label_len )
      {
        LOG_MSG_INFO1_2("label_len %d > %d", label_len, DNS_MAX_LABEL_LEN );
        retval = -1;
        break;
      }
      
      /* move offset past labellen byte */
      offset++;

      if ( NULL != dst_buf )
      {
        /* Copy label to destination buffer after checking for space
         * for label + terminating '.' and 0. */
        if ( dst_buf_len < ((uint32)label_len) + (2 * sizeof(uint8)) )
        {
          LOG_MSG_INFO1_1("dst_buf doesn't have room "
                          "for label of length %d", label_len);
          retval = -1;
          break;
        }

        if ( -1 == get_f( buf, offset, dst_buf, (uint16)dst_buf_len,label_len) )
        {
          LOG_MSG_INFO1_0("get_f failed ");
          retval = -1;
          break;
        }
        dst_buf += label_len;
        *dst_buf++  = '.';
        dst_buf_len -= label_len + sizeof(uint8); /* size of '.' char */
      }
      
      offset += label_len;
      octet_count += label_len + sizeof(uint8);
      
      /* Add the label length + space for '.' to the total name length. */
      if ( NULL != name_len )
      {
        *name_len += label_len + sizeof(uint8);
      }
    } /* else - non-pointer case */
  } /* read domain name from buffer loop */

  if ( -1 != retval )
  {
    /* Fill in terminating characters */
    if ( NULL != dst_buf )
    {
      if ( 0 >= dst_buf_len )
      {
        LOG_MSG_ERROR_1("ps_dnsi_domain_name_expand(): dst_buf_len = %d",
                        dst_buf_len );
        retval = -1;
      }
      *dst_buf  = '\0';
    }
/* Removed here since we don't want to count NULL char in string length 
    if (name_len != NULL)
    {
      (*name_len)++;
    }
*/  
    /* Count the 'zero' length byte */
    octet_count++;

    if ( DNS_MAX_DOMAIN_NAME_OCTETS < octet_count )
    {
      LOG_MSG_INFO1_0("ps_dnsi_domain_name_expand(): message exceeds max domain "
                      "name octets" );
      retval = -1;
    }
    else
    {
      /* if retval hasn't already been set in case of ptr */
      if ( 0 == retval ) 
      {
        retval = offset + sizeof(uint8);
      }
    }
  } /* if (-1 != retval ) */

  return retval;
} /* ps_dnsi_domain_name_expand() */

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
)
{
  struct ps_in_addr  temp_in_addr;
  struct ps_in6_addr temp_in6_addr;
  int32              retval = DSS_ERROR;
  int16              dss_errno = 0;
  int16              query_len = 0;

  retval = dss_inet_pton( hostname,
                          DSS_AF_INET,
                          &temp_in_addr,
                          sizeof(struct ps_in_addr),
                          &dss_errno );
  if( DSS_ERROR == retval )
  {
    retval = dss_inet_pton( hostname,
                            DSS_AF_INET6,
                            &temp_in6_addr,
                            sizeof(struct ps_in6_addr),
                            &dss_errno );
  }
  
  if( DSS_ERROR == retval )
  {
    query_len = strlen(hostname) ;
    if(0 == query_len)
    {
      return;
    }
    if( '.' != hostname[ query_len - 1 ] )
    {
      hostname[ query_len ] = '.';
      hostname[ query_len + 1 ] = '\0';
    }
  }
}

