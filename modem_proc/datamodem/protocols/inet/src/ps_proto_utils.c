/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 P S_P R O T O_U T I L S . C

GENERAL DESCRIPTION
 Protocols Generic  Utility Functions.


EXTERNALIZED FUNCTIONS
 None
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_proto_utils.c_v   1.5   12 Feb 2003 20:35:30   omichael  $
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_proto_utils.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when          who    what, where, why
--------    ---    ----------------------------------------------------------
11/14/13    ds     Adding support to read the ICMP tokens parameter values for 
                          both IPv4 and IPv6.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "ps_utils.h"
#include "msg.h"
#include "ps_icmp.h"
#include "ps_icmp6.h"
#include "ps_utils_defs.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef PACKED struct
{ 
  uint16 icmp4_tokens_per_second;
  uint16 icmp4_max_tokens;
  uint16 icmp6_tokens_per_second;
  uint16 icmp6_max_tokens;
} icmp_reply_rate_config_s;

/*===========================================================================

                  FUNCTION DEFINITIONS
                  
===========================================================================*/
void icmp_read_tokens_nv( void )
{
  icmp_reply_rate_config_s icmp_reply_rate;
  const char* item_file_path = 
    {PS_NV_CONF_ITEM_INFO(PS_NV_CONF_ICMP_REPLY_RATE)};

  /*-------------------------------------------------------------------------
      Read the ICMP tokens NV item.
    -------------------------------------------------------------------------*/  
  if (0 != ps_read_efs_nv( item_file_path, 
                           &icmp_reply_rate, 
                           sizeof(icmp_reply_rate) ))
  {
    /*-----------------------------------------------------------------------
          Unable to read the NV item , set the values to default 
       -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0( "Unable to read ICMP tokens NV, setting to default values");
    icmp_reply_rate.icmp4_tokens_per_second = ICMP_TOKENS_PER_SECOND;
    icmp_reply_rate.icmp4_max_tokens = ICMP_MAX_TOKENS;
    icmp_reply_rate.icmp6_tokens_per_second = ICMP6_TOKENS_PER_SECOND;
    icmp_reply_rate.icmp6_max_tokens =  ICMP6_MAX_TOKENS;   
  }
  else
  { 
    /*-----------------------------------------------------------------------
          Check the maximum of the two values, if the NV values are less than, 
          then configure it to default.
       -----------------------------------------------------------------------*/
    icmp_reply_rate.icmp4_tokens_per_second = 
                                  MAX(icmp_reply_rate.icmp4_tokens_per_second, 
                                      ICMP_TOKENS_PER_SECOND);
    icmp_reply_rate.icmp4_max_tokens = 
                                  MAX(icmp_reply_rate.icmp4_max_tokens, 
                                      ICMP_MAX_TOKENS);
    icmp_reply_rate.icmp6_tokens_per_second = 
                                  MAX(icmp_reply_rate.icmp6_tokens_per_second, 
                                      ICMP6_TOKENS_PER_SECOND);
    icmp_reply_rate.icmp6_max_tokens = 
                                  MAX(icmp_reply_rate.icmp6_max_tokens,
                                      ICMP6_MAX_TOKENS);
  }

  /*-------------------------------------------------------------------------
      Check if Max Number of tokens is greater than or equal to icmp tokens per second.
    -------------------------------------------------------------------------*/
  if( icmp_reply_rate.icmp4_max_tokens < 
                                    icmp_reply_rate.icmp4_tokens_per_second )                             
  {
    icmp_reply_rate.icmp4_max_tokens =
                                    icmp_reply_rate.icmp4_tokens_per_second;
  }
  if( icmp_reply_rate.icmp6_max_tokens < 
                                    icmp_reply_rate.icmp6_tokens_per_second )
  {
    icmp_reply_rate.icmp6_max_tokens = 
                                    icmp_reply_rate.icmp6_tokens_per_second;
  }
  
  /*-------------------------------------------------------------------------
       Now call the API for individual ICMP modules to set the token values 
    -------------------------------------------------------------------------*/
  
  /* Calling the API to set the values of the ICMP v4 module */
  icmp_set_v4_tokens( icmp_reply_rate.icmp4_tokens_per_second, 
                      icmp_reply_rate.icmp4_max_tokens );

  /* Calling the API to set the values of the ICMP v6 module */
  icmp_set_v6_tokens( icmp_reply_rate.icmp6_tokens_per_second, 
                      icmp_reply_rate.icmp6_max_tokens );
  
  return;
}

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
)
{
  uint8 temp_domain_name_arr[PS_IFACE_MAX_DOMAIN_NAME_SIZE] = {0};
  uint8 read_index = 0;
  uint8 write_index = 1; /* Start writing from first index : first byte is 
                            length of first label. */
  uint8 current_label_len = 0;
  uint8 current_label_len_pos = 0;

  /* Encoding will increment 1 byte in the buffer length */
  #define MAX_LENGTH_INCREMENT 1
/*-------------------------------------------------------------------------*/

  /* Validation checks */  
  if( domain_name_ptr == NULL || domain_name_len == NULL )
  {
    LOG_MSG_ERROR_0( "NULL arguments" );
    return -1;
  }
  /* Ensure we have enough space in input domain name buffer to account for 
     length increment due to encoding. */
  if( *domain_name_len >= 
        ( PS_IFACE_MAX_DOMAIN_NAME_SIZE - MAX_LENGTH_INCREMENT ))
  {
    LOG_MSG_ERROR_1( "Insufficient space in domain name buffer %d", 
                     *domain_name_len );
    return -1;
  }
  /* The domain name must be NULL terminated */
  if( domain_name_ptr[*domain_name_len] != 0 )
  {
    LOG_MSG_ERROR_0("Domain name to encode is not NULL terminated.");
    return -1;
  }

  /* Summary

     The input domain name is in following format:
     label1.label2.label3
     All the ASCII characters except for a [dot] form a label.

     The encoded domain name should have an extra byte for each label length
     inserted immediately before the label.

     Also the length byte should have the 2 MSB as 0 => Max label length can
     be 63 (0x3F)

     The encoded domain name would end with 0x00 byte which indicates the 
     next label length as 0 => no more labels. 

     The processing logic is to parse through the domain name pointer and 
     create the encoded domain name in the temp array. */

  /* Parse the input domain name */
  while( read_index < *domain_name_len )
  {
    /* Write to temp buffer if this is not a [dot] [0x2E] */
    if( domain_name_ptr[read_index] != 0x2E )
    {
      temp_domain_name_arr[write_index] = domain_name_ptr[read_index];
      current_label_len++;
    }
    else
    {
      /* Validate current label length */
      if( current_label_len > 63 )
      {
        LOG_MSG_ERROR_1( "Invalid label length %d", current_label_len );
        return -1;
      }

      /* Set current label length at label beginning and update label length
         byte position for next label */
      temp_domain_name_arr[current_label_len_pos] = current_label_len;
      current_label_len_pos = write_index;

      /* Reset current label length */
      current_label_len = 0;
    }

    read_index++;
    write_index++;
  }

  /* Set last label length : not covered in above while loop */
  temp_domain_name_arr[current_label_len_pos] = current_label_len;

  /* Terminate with NULL character */
  temp_domain_name_arr[write_index] = 0;

  /* Update the domain name len
     Basically the encoded domain name length would just be one more than 
     the original domain name len.
     The encoded domain name length is also excluding the terminating NULL 
     character. */
  *domain_name_len = write_index;

  /* Copy encoded value to input domain name pointer */
  memscpy( domain_name_ptr, *domain_name_len + 1, temp_domain_name_arr, 
           PS_IFACE_MAX_DOMAIN_NAME_SIZE );

  return 0;
}