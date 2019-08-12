/*===========================================================================

                       P S _ D N S I _ M S G _ M G R . C

DESCRIPTION

  The Data Services DNS Subsystem Message manager module. Contains
  declarations of functions, variables, macros, structs and enums
  used by DNS message manager. This module is internal to the
  DNS subsystem.

EXTERNALISED FUNCTIONS

  ps_dnsi_msg_mgr_init()
    Initializes the msg manager module during power-up.

  ps_dnsi_msg_mgr_form_query()
    Using query type, query class and query data constructs a DSM item chain
    corresponding to the DNS query message to be sent.

  ps_dnsi_msg_mgr_parse_response()
    Parse the DSM item chain reply into set of RRs.

INTIALIZATIONS AND SEQUENCING REQUIREMENTS
  This module should be run only in PS task context.

Copyright (c) 2007-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
//TODO Use dsm_pullup instead of dsm_extract
/*===========================================================================

                             EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Note that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssvc/src/ps_dnsi_msg_mgr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/15   pg      Fixed recursive DNS query.
12/27/09   ts      In case of receiving unknown or invalid RR data update 
                   offset to skip that RR node.
10/08/09   ts      Fixed crash in ps_dnsi_msg_mgr_parse_response. 
                   Replace temp_rr_q which was on stack with static global
                   to reuse and initialize mutex inside the queue.
07/13/09   mt      Added wildcard query resolving when AI_PASSIVE is set.
07/08/09   mt      Tightened numerical query resolving rules.
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
03/02/08   hm      Fixes for DNS cache poisoning.
12/05/08   hm      Randomizing DNS transaction ID.
02/15/08   dm      Fixed a crash that occurs when domain name 
                   validation fails
08/23/07   hm      Fixed parsing of compressed domain names. 
06/11/07   hm      Added function to resolve numerical hostnames
05/19/07   hm      Bug fixes found during testing.
03/26/07   hm      Initial development work.

===========================================================================*/
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ps_dnsi_msg_mgr.h"
#include "ps_mem.h"
#include "ps_dnsi_mem_mgr.h"
#include "dssocket.h"  //needed for ps_sockaddr used by DSM functions.
#include "dss_config.h"
#include "dssinternal.h"
#include "dsm.h"
#include "ps_utils.h" // For random msg ID generation

#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                                DATA STRUCTURES

===========================================================================*/
/*---------------------------------------------------------------------------
  Q of RR entries used on msg parsing.
---------------------------------------------------------------------------*/
static q_type ps_dnsi_msg_mgr_rr_q;

#define  PS_DNSI_MSG_MGR_MIN_MSG_ID  1
#define  PS_DNSI_MSG_MGR_MAX_MSG_ID  0x7FFF

#define PS_DNSI_MSG_MGR_REASONABLE_RDATA_LEN  300
#define PS_DNSI_MSG_MGR_DNS_HDR_SIZE   12

/* mask to obtain pointer value from word */
#define DNS_LABEL_PTR_VAL_MASK     ((uint32)~0xffffc000)
/* mask to check top two bits in byte for domain name ptr format */
#define DNS_LABEL_PTR_MASK         ((uint8)0xc0)

/*---------------------------------------------------------------------------
  Enum for supported IANA Resource Record types
  Please refer to RFC 2929 for the comprehensive list
---------------------------------------------------------------------------*/
typedef enum
{
  TYPE_A          = 1,        /*   a host address               [RFC 1035] */
  TYPE_CNAME      = 5,        /*   canonical name for an alias  [RFC 1035] */
  TYPE_PTR        = 12,       /*   domain name pointer          [RFC 1035] */
  TYPE_AAAA       = 28        /*   IP6 Address                  [RFC 3596] */
} ps_dnsi_msg_mgr_rr_type_enum_type;

/*---------------------------------------------------------------------------
  Enum for supported IANA Resource Record classes
  Please refer to RFC 1035 for the comprehensive list
---------------------------------------------------------------------------*/
typedef enum
{
  CLASS_IN       = 1          /*   Internet class               [RFC 1035] */
} ps_dnsi_msg_mgr_rr_class_enum_type;

/*---------------------------------------------------------------------------
  Format of DNS message header.  RFC 1035.

   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                       ID                      |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                     QDCOUNT                   |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                     ANCOUNT                   |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                     NSCOUNT                   |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                     ARCOUNT                   |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

  ID      - Identifier. Used to match response with query
  QR      - Is this a query or response?
  Opcode  - Operation code
            0 - Standard query
            1 - Inverse query
            2 - Server status request
            3-15 - Future use
  AA      - Is this an authoritative answer?
  TC      - Is the message truncated?
  RD      - Is recursion desired?
  RA      - Is recursion available?
  Z       - Reserved (must be 0)
  RCODE   - Error code in answer
            0 - No error condition
            1 - Format error
            2 - Server failure
            3 - Name Error
            4 - Not Implemented
            5 - Refused
            6-15 -  Reserved for future use.
  QDCOUNT - Number of entries in question section
  ANCOUNT - Number of RRs in answer section
  NSCOUNT - Number of name server RRs in authority section
  ARCOUNT - Number of RRs in additional records section
---------------------------------------------------------------------------*/

typedef struct
{
  uint16        msg_id;
  union
  {
    struct
    {
      unsigned char response_code          : 4;
      unsigned char reserved_bits          : 3;
      unsigned char is_recursion_available : 1;
      unsigned char is_recursion_desired   : 1;
      unsigned char is_truncated           : 1;
      unsigned char is_authorative_answer  : 1;
      unsigned char opcode                 : 4;
      unsigned char query_or_response      : 1;
    } flag_bits;
    uint16  flag_param;
  }flags;

  uint16        num_questions;
  uint16        num_answer_rr;
  uint16        num_authority_rr;
  uint16        num_additional_rr;
} ps_dnsi_msg_mgr_dns_hdr_type;

/*
#if sizeof(ps_dnsi_msg_mgr_dns_hdr_type) != 12
  #error "Incorrect size";
#endif
*/

/*---------------------------------------------------------------------------
  Format for DNS Question Section. RFC 1035.

     0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    QNAME  - query name
    QTYPE  - Type of the query
    QCLASS - Class of the query
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_msg_mgr_question_s_type
{
  char    * query;
  uint16    qtype;
  uint16    qclass;
} ps_dnsi_msg_mgr_question_type;

/*---------------------------------------------------------------------------
  Tuning the number of ps dnsi msg rr buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_DNSI_MSG_RR_BUF_SIZE  ((sizeof(ps_dnsi_generic_rr_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define PS_DNSI_MSG_RR_BUF_NUM        ((3) * (DSS_MAX_UDP_SOCKS))
#define PS_DNSI_MSG_RR_BUF_HIGH_WM    (PS_DNSI_MSG_RR_BUF_NUM - 3)
#define PS_DNSI_MSG_RR_BUF_LOW_WM      2

#else

#define PS_DNSI_MSG_RR_BUF_NUM        5
#define PS_DNSI_MSG_RR_BUF_HIGH_WM    5
#define PS_DNSI_MSG_RR_BUF_LOW_WM     1
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_dnsi_generic_rr along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_dnsi_msg_rr_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                  (
                                    PS_DNSI_MSG_RR_BUF_NUM,
                                    PS_DNSI_MSG_RR_BUF_SIZE
                                  )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter points to ps_dnsi_generic_rr_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * ps_dnsi_msg_rr_buf_hdr[PS_DNSI_MSG_RR_BUF_NUM];
static ps_dnsi_generic_rr_type * ps_dnsi_msg_rr_buf_ptr[PS_DNSI_MSG_RR_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*===========================================================================

                         INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_GET_QNAME()

DESCRIPTION
  Checks if the query is correctly formed or not and constructs a domain
  name in the DNS message format. Does not use any pointers in messages.

  Each label is preceded by the length of the label. At the end of the
  constructed name, a 0 is appended to indicate a label of 0 length.
  Valid characters allowed in the constructed domain name are a-z, A-Z, 0-9
  and '-'(hyphen). The beginning character of the domain name and of labels
  should be an aplphabet.

  The length of the labels should not exceed DSS_DNS_MAX_LABEL_LEN.
  The length of the entire domain name should not exceed
  DSS_DNS_MAX_DOMAIN_NAME_LEN.

PARAMETERS
  domain_name_ptr  - Contents of DNS query (in FQDN format)
  qname_ptr        - Buffer where formed domain name will be stored.
  errno            - error passed back in case of failure

DEPENDENCIES
  domain_name should allocated memory previously.

RETURN VALUE
  Length of the formed qname on success
  DSS_ERROR on error.

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_msg_mgri_get_qname
(
  char                              * domain_name_ptr,
  char                              * qname_ptr,
  ps_dnsi_query_type_enum_type        query_type,
  ps_dnsi_query_class_enum_type       query_class,
  int16                             * ps_errno
)
{
  uint32  domain_name_len;
  uint16  label_len_index;
  uint16  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == domain_name_ptr )
  {
    LOG_MSG_ERROR_0("NULL FQDN");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*---------------------------------------------------------------------
   It has been assured in DNS code that domain_name_len doesn't 
   exceed DSS_DNS_MAX_DOMAIN_NAME_LEN and that domain_name_ptr is NULL
   terminated.
  ---------------------------------------------------------------------*/
  domain_name_len = strlen( domain_name_ptr );
  
  for( index = 0; index < domain_name_len; index++ )
  {
    label_len_index = index;
    
    /*---------------------------------------------------------------------
      qname_ptr has been allocated to max size of DSS_DNS_MAX_DOMAIN_NAME_LEN + 1
      therefore, the 'index+1' is safe.
    ---------------------------------------------------------------------*/
    qname_ptr[index + 1] = domain_name_ptr[index];

    /*---------------------------------------------------------------------
      Iterating over the label until the separating '.' character in order
      to construct the domain name according to the DNS message format.
    ---------------------------------------------------------------------*/
    while( domain_name_ptr[++index] != '.' && index < domain_name_len )
    {
      qname_ptr[index + 1] = domain_name_ptr[index];
    }

    /*---------------------------------------------------------------------
      Putting the length/size of the label before the label itself.
    ---------------------------------------------------------------------*/
    qname_ptr[label_len_index] = index - label_len_index;
  }

  qname_ptr[index] = 0;
  return index + 1;

} /* ps_dnsi_msg_mgri_get_qname() */


/*===========================================================================
FUNCTION PS_DNSI_MSG_MGRI_EXTRACT_DOMAIN_NAME

DESCRIPTION
  The function expands a passed domain name encoded in the compressed domain
  name format (RFC 1035 Section 4.1.4). It is passed an DSM item chain,
  dns_response_ptr, which contains the compressed domain name along with
  an offset argument giving offset of the compressed name in dns_response_ptr.

  This assumes the DNS message format defined in RFC 1035 Section 4.1, but
  may be used to expand compressed domain names messages with pointers if
  offset values are accurate.

  The expanded name is copied into dst_buf_ptr argument if non-NULL.

PARAMETERS
  dns_response_ptr - DSM item with the message
  offset           - Offset into the DNS item
  buf_ptr          - Buffer for storing the DNS name

RETURN VALUE
  The offset in the opaque buffer past the compressed domain name is
  returned as the return value of the function.
  A return value of DSS_ERROR indicates an error.

DEPENDENCIES
  Memory (upto DSS_DNS_MAX_DOMAIN_NAME_LEN) should be previously
  allocated for buf_ptr.

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_msg_mgri_extract_domain_name
(
  dsm_item_type    * dns_response_ptr,
  int16              offset,
  char             * dst_buf_ptr
)
{
  uint8   label_len = 0;             /* Length of domain name label        */
  int16   retval = 0;                /* Return value                       */
  uint32  chase_ptr_offset = offset; /* Used to detect pointer loops       */
  int16   chase_ptr_toggle = FALSE;
  uint8   chase_label_len = 0;
  char  * temp_dst_buf = NULL;  //TODO Remove
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == dns_response_ptr || NULL == dst_buf_ptr)
  {
    LOG_MSG_ERROR_0("ps_dnsi_msg_mgri_extract_domain_name(): NULL parameters");
    return DSS_ERROR;
  }

  temp_dst_buf = dst_buf_ptr;
  /*-------------------------------------------------------------------------
    Loop over the domain name till have a valid name.
  -------------------------------------------------------------------------*/

  for(;;)
  {
    /*-----------------------------------------------------------------------
      Get the label length (or pointer indicator)
    -----------------------------------------------------------------------*/
    if( sizeof(uint8) != dsm_extract( dns_response_ptr,
                                      offset,
                                      &label_len,
                                      sizeof(uint8) ) )
    {
      LOG_MSG_ERROR_1("Cant extract label length from DSM chain 0x%p",
                      dns_response_ptr);
      retval = DSS_ERROR;
      break;
    }

    /*-----------------------------------------------------------------------
      A zero label length indicates end of domain name.
    -----------------------------------------------------------------------*/
    if( 0 == label_len )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Pointer case. A pointer is detected if the most significant
      two bits of the label_len are one. The remaining fourteen bits
      are the pointer. Only one pointer is allowed in a domain name,
      see RFC 1035 4.1.4
    -----------------------------------------------------------------------*/
    if( (label_len & DNS_LABEL_PTR_MASK) == DNS_LABEL_PTR_MASK )
    {
      /*---------------------------------------------------------------------
        Save offset past the domain name as location of next domain name.
        We need to save offset before following the label pointer.
      ---------------------------------------------------------------------*/
      if( 0 == retval )
      {
        retval = offset + sizeof(uint16);
      }

      /*---------------------------------------------------------------------
        Extract the pointer to the domain name. This is a 16 bits as opposed
        to the label length which is 8 bits.
      ---------------------------------------------------------------------*/
      if( sizeof(uint16) != dsm_extract( dns_response_ptr,
                                         offset,
                                         &offset,
                                         sizeof(uint16) ) )
      {
        LOG_MSG_ERROR_1("Cant extract domain name ptr offset from DSM chain 0x%p",
                        dns_response_ptr);
        retval = DSS_ERROR;
        break;
      }
      offset = ps_ntohs(offset);
      offset &= DNS_LABEL_PTR_VAL_MASK;

      /*---------------------------------------------------------------------
        Move chase_pointer to next pointer
      ---------------------------------------------------------------------*/
      if( TRUE == chase_ptr_toggle )
      {
        for (;;)
        {
          if( sizeof(uint8) != dsm_extract( dns_response_ptr,
                                            (uint16)chase_ptr_offset,
                                            &chase_label_len,
                                            (uint16)sizeof(uint8) ) )
          {
            LOG_MSG_ERROR_1("Cant extract chase label len from DSM chain 0x%p",
                            dns_response_ptr);
            retval = DSS_ERROR;
            break;
          }

          /*-----------------------------------------------------------------
            chase pointer should not reach end of domain_name
          -----------------------------------------------------------------*/
          if ( 0 == chase_label_len )
          {
            LOG_MSG_ERROR_0("ps_dnsi_msg_mgri_extract_domain_name(): chase label length is 0" );
            retval = DSS_ERROR;
            break;
          }

          if((chase_label_len & DNS_LABEL_PTR_MASK) != DNS_LABEL_PTR_MASK )
          {
            chase_ptr_offset += chase_label_len + sizeof(uint8);
          }
          else
          {
            /*---------------------------------------------------------------
              Follow chase pointer.
            ---------------------------------------------------------------*/
            if( sizeof(uint16) != dsm_extract( dns_response_ptr,
                                               (uint16)chase_ptr_offset,
                                               &chase_ptr_offset,
                                               (uint16)sizeof(uint16) ) )
            {
              LOG_MSG_ERROR_1("Cant extract chase label ptr from DSM chain 0x%p",
                              dns_response_ptr);
              retval = DSS_ERROR;
              break;
            }
            chase_ptr_offset = ps_ntohs(chase_ptr_offset);
            chase_ptr_offset &= DNS_LABEL_PTR_VAL_MASK;
            break;
          }
        }

        /*-------------------------------------------------------------------
          Error in inner loop.
        -------------------------------------------------------------------*/
        if( DSS_ERROR == retval )
        {
          LOG_MSG_ERROR_0("Error in parsing Pointer response");
          break;
        }

        /*-------------------------------------------------------------------
          Loop detected in DNS response.
        -------------------------------------------------------------------*/
        if( offset == (int16)chase_ptr_offset )
        {
          LOG_MSG_ERROR_0("ps_dnsi_domain_name_expand(): pointer loop detected "
                          "in domain name list");
          retval = DSS_ERROR;
          break;
        }
      }
      /*---------------------------------------------------------------------
        only move chase pointer every other pointer seen
      ---------------------------------------------------------------------*/
      chase_ptr_toggle = !chase_ptr_toggle;
    } /* if pointer case */

    /*-----------------------------------------------------------------------
      Non pointer case. Here the extracted byte indicates the label length.
      The domain name data starts just after the label length byte.
    -----------------------------------------------------------------------*/
    else
    {
      if( DSS_DNS_MAX_LABEL_LEN < label_len )
      {
        LOG_MSG_ERROR_2("label_len %d > %d",
                        label_len, DSS_DNS_MAX_LABEL_LEN );
        retval = DSS_ERROR;
        break;
      }

      if( DSS_DNS_MAX_DOMAIN_NAME_LEN <= offset + label_len )
      {
        LOG_MSG_ERROR_0("Domain name len is exceeds max length");
        retval = DSS_ERROR;
        break;
      }

      /*---------------------------------------------------------------------
        Move offset past label len byte and expand the domain name into
        destination buffer.
      ---------------------------------------------------------------------*/
      offset += sizeof(uint8);
      if( label_len != dsm_extract( dns_response_ptr,
                                    offset,
                                    dst_buf_ptr,
                                    label_len ) )
      {
        LOG_MSG_ERROR_2("Cant extract label name from DSM chain 0x%p into buffer 0x%p",
                        dns_response_ptr, dst_buf_ptr);
        retval = DSS_ERROR;
        break;
      }
      dst_buf_ptr += label_len;
      *dst_buf_ptr++  = '.';
      offset += label_len;
    } /* else - non-pointer case */

  } /* read domain name from buffer loop */

  if( DSS_ERROR != retval )
  {
    if( DSS_DNS_MAX_DOMAIN_NAME_LEN <= dst_buf_ptr - temp_dst_buf )
    {
      LOG_MSG_ERROR_1("ps_dnsi_msg_mgri_extract_domain_name(): Too long domain name: %d",
                      dst_buf_ptr - temp_dst_buf);
      retval = DSS_ERROR;
    }
    /*-----------------------------------------------------------------------
      NULL terminate the destination buffer.
    -----------------------------------------------------------------------*/
    *dst_buf_ptr  = '\0';

    /*-----------------------------------------------------------------------
      If retval hasn't already been set in case of ptr
    -----------------------------------------------------------------------*/
    if( 0 == retval )
    {
      retval = offset + sizeof(uint8);
    }
  } /* if( DSS_ERROR != retval ) */

  return retval;
} /* ps_dnsi_msg_mgri_extract_domain_name() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_ADD_DNS_HDR()

DESCRIPTION
  Adds a DNS message header to the DSM item chain.

PARAMETERS
  dns_query_ptr        - Pointer to the DSM item chain.
  is_recursion_desired - Should the request indicate DNS recursion
  errno                - Error code in case of error.

RETURN VALUE
  Msg Id of the DNS message pushed down on the DSM chain.
  DSS_ERROR on error and places error code in errno.

  errno values
  ------------
  DS_EFAULT   - If the arguments are wrong.
  DS_ESYSTEM  - System failure. Unable to write DSM Chain.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_msg_mgri_add_dns_hdr
(
  dsm_item_type  * dns_query_ptr,
  boolean          is_recursion_desired,
  int16          * ps_errno
)
{
  ps_dnsi_msg_mgr_dns_hdr_type  dns_hdr;
  uint16                        msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == dns_query_ptr )
  {
    LOG_MSG_ERROR_0("NULL DSM ptr");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( PS_DNSI_MSG_MGR_DNS_HDR_SIZE != sizeof(dns_hdr) )
  {
    LOG_MSG_ERROR_1("wrong sizeof(dns_hdr): %d", sizeof(dns_hdr));
    *ps_errno = DS_ESYSTEM;
    return DSS_ERROR;
  }

  msg_id = ps_utils_generate_16bit_rand_num_by_range(PS_DNSI_MSG_MGR_MIN_MSG_ID,
                                                     PS_DNSI_MSG_MGR_MAX_MSG_ID);

  memset(&dns_hdr, 0, sizeof(dns_hdr));
  dns_hdr.msg_id            = dss_htons(msg_id);
  dns_hdr.num_questions     = dss_htons(1);
  dns_hdr.num_answer_rr     = 0;
  dns_hdr.num_authority_rr  = 0;
  dns_hdr.num_additional_rr = 0;
  /*-------------------------------------------------------------------------
    Flags are always set to 0 in DNS query except for RD flag, which has
    default of 1 but can be set by the application.RFC 1035
  -------------------------------------------------------------------------*/
  dns_hdr.flags.flag_bits.is_recursion_desired = (is_recursion_desired)?1:0;

  dns_hdr.flags.flag_param = dss_htons(dns_hdr.flags.flag_param);

  if( sizeof(dns_hdr) != dsm_pushdown_tail( &dns_query_ptr,
                                            &dns_hdr,
                                            sizeof(dns_hdr),
                                            DSM_DS_LARGE_ITEM_POOL ))
  {
    LOG_MSG_ERROR_0("Failed to push header on to DSM");
    *ps_errno = DS_ESYSTEM;
    /* Dont free the DSM chain here, free it where we call this func. */
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Return the message id.
  -------------------------------------------------------------------------*/
  return msg_id;

} /* ps_dnsi_msg_mgri_add_dns_hdr() */



/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_EXTRACT_DNS_HDR()

DESCRIPTION
  Extracts the DNS Header section from the DSM item chain

PARAMETERS
  msg_id              - Message identifier of the message sent to
                        DNS server.
  dns_response_ptr    - Pointer to dsm chain of the DNS reply message.
  dns_hdr_ptr         - Pointer to message header buffer.
  errno               - Error code in case of error.

RETURN VALUE
  Msg id of the extracted DNS message header.
  DSS_ERROR on error or on recving malformed packets and
  places error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.
  DS_EHOSTNOTFOUND  - Host not found.
  DS_EOPNOTSUPP     - Option not supported.
  DS_EINVAL         - Invalid Operation.
  DS_ECONNREFUSED   - Connection refused by peer.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgri_extract_dns_hdr
(
  dsm_item_type                 * dns_response_ptr,
  ps_dnsi_msg_mgr_dns_hdr_type  * dns_hdr_ptr,
  int16                         * ps_errno
)
{
  uint32  num_bytes;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == dns_response_ptr || NULL == dns_hdr_ptr )
  {
    LOG_MSG_ERROR_2("NULL args 0x%p, 0x%p", dns_response_ptr, dns_hdr_ptr);
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  num_bytes = dsm_extract( dns_response_ptr,
                           0,
                           dns_hdr_ptr,
                           sizeof(ps_dnsi_msg_mgr_dns_hdr_type) );

  if( sizeof(ps_dnsi_msg_mgr_dns_hdr_type) != num_bytes )
  {
    LOG_MSG_ERROR_1("Response has %d bytes only", num_bytes);
    *ps_errno = DS_ESYSTEM;
    return DSS_ERROR;
  }

  dns_hdr_ptr->msg_id            = dss_ntohs(dns_hdr_ptr->msg_id);
  dns_hdr_ptr->flags.flag_param  = dss_ntohs(dns_hdr_ptr->flags.flag_param);
  dns_hdr_ptr->num_questions     = dss_ntohs(dns_hdr_ptr->num_questions);
  dns_hdr_ptr->num_answer_rr     = dss_ntohs(dns_hdr_ptr->num_answer_rr);
  dns_hdr_ptr->num_authority_rr  = dss_ntohs(dns_hdr_ptr->num_authority_rr);
  dns_hdr_ptr->num_additional_rr = dss_ntohs(dns_hdr_ptr->num_additional_rr);

  if( dns_hdr_ptr->flags.flag_bits.query_or_response != 1 ||
      dns_hdr_ptr->flags.flag_bits.opcode != 0            ||
      dns_hdr_ptr->flags.flag_bits.reserved_bits  != 0 )
  {
    LOG_MSG_ERROR_1("Invalid flags 0x%x in DNS response message",
                    dns_hdr_ptr->flags.flag_param);
    *ps_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  if( dns_hdr_ptr->flags.flag_bits.is_truncated == 1 )
  {
    LOG_MSG_INFO1_0("DNS Response is truncated");
    *ps_errno = DS_ETRUNCATED; //MSR what else do we do if response is truncated? RFC 1035 suggests to use TCP
    //HM not sure what can be done here. Using TCP would be overly complicated.
    //we are anyways returning ETRUNCATED if we run out of memory while copying the response
  }

  if( 0 == dns_hdr_ptr->flags.flag_bits.is_authorative_answer )
  {
    LOG_MSG_INFO1_0("DNS Response is NOT authorative");
  }

  if( dns_hdr_ptr->flags.flag_bits.response_code != 0 )
  {
    LOG_MSG_ERROR_1("Error in response %d",
                    dns_hdr_ptr->flags.flag_bits.response_code);

    switch( dns_hdr_ptr->flags.flag_bits.response_code )
    {
      case 1:
        *ps_errno = DS_NAMEERR;
        break;

      case 2:
        *ps_errno = DS_ESYSTEM;
        break;

      case 3:
        *ps_errno = DS_EHOSTNOTFOUND;
        break;

      case 4:
        *ps_errno = DS_EOPNOTSUPP;
        break;

      case 5:
        *ps_errno = DS_ECONNREFUSED;
        break;

      default:
        *ps_errno = DS_EINVAL;
        break;
    } /* switch( response code ) */

    return DSS_ERROR;
  } /* if( response_code != 0 ) */

  return dns_hdr_ptr->msg_id;

} /* ps_dnsi_msg_mgri_extract_dns_hdr() */



/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_ADD_QUESTION()

DESCRIPTION
  Adds a DNS question message section to the DSM item chain.

PARAMETERS

  dns_query_ptr      - DSM item pointer to the DNS query message.
  domain_name_ptr    - The domain name for the query.
  query_type         - Type of query
  query_class        - Class of query
  errno              - Error code in case of error.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on error and places error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.

DEPENDENCIES
  The DNS header should be added before adding question.

SIDE EFFECTS
  None
===========================================================================*/
static int16 ps_dnsi_msg_mgri_add_question
(
  dsm_item_type                     * dns_query_ptr,
  char                              * domain_name_ptr,
  ps_dnsi_query_type_enum_type        query_type,
  ps_dnsi_query_class_enum_type       query_class,
  int16                             * ps_errno
)
{
  char                              * qname_ptr;
  int16                               qname_len;
  uint16                              temp;
  ps_dnsi_msg_mgr_rr_type_enum_type   rr_type;
  ps_dnsi_msg_mgr_rr_class_enum_type  rr_class;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == dns_query_ptr )
  {
    LOG_MSG_ERROR_0("NULL DSM ptr");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  switch( query_type )
  {
    case PS_DNSI_QUERY_TYPE_A:
      rr_type = TYPE_A;
      break;

    case PS_DNSI_QUERY_TYPE_AAAA:
      rr_type = TYPE_AAAA;
      break;

    case PS_DNSI_QUERY_TYPE_PTR:
      rr_type = TYPE_PTR;
      break;

    case PS_DNSI_QUERY_TYPE_CNAME:
      rr_type = TYPE_CNAME;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid query type %d ", query_type);
      *ps_errno = DS_EFAULT;
      return DSS_ERROR;
  } /* switch( query_type ) */

  switch( query_class )
  {
    case PS_DNSI_QUERY_CLASS_IN:
      rr_class = CLASS_IN;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid query class %d", query_class);
      *ps_errno = DS_EFAULT;
      return DSS_ERROR;
  } /* switch( query_class ) */

  /*-------------------------------------------------------------------------
    Depending upon the query type create the question.
    If the query type is A/AAAA the question will be a DNS domain name
    If the query type is PTR/CNAME the question will be a PTR domain name
      For Addr family DSS_AF_INET a.b.c.d.in-addr.arpa.
      For Addr family DSS_AF_INET6 a.b.c.d.e.f....ip6.arpa.
  -------------------------------------------------------------------------*/
//TODO  qname_ptr = ps_dnsi_mem_alloc(strlen(domain_name_ptr) + 1);
  qname_ptr = ps_dnsi_mem_alloc(DSS_DNS_MAX_DOMAIN_NAME_LEN + 1);
  if( NULL == qname_ptr )
  {
    LOG_MSG_ERROR_0("Out of memory");
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  qname_len = ps_dnsi_msg_mgri_get_qname( domain_name_ptr,
                                          qname_ptr,
                                          query_type,
                                          query_class,
                                          ps_errno );
  if( DSS_ERROR == qname_len )
  {
    LOG_MSG_ERROR_0("Malformed FQDN");
    /* Dont free the DSM chain here, free it where we call this func. */
    PS_DNSI_MEM_FREE(qname_ptr);
    *ps_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Add qname to the message.
  -------------------------------------------------------------------------*/
  if( qname_len != dsm_pushdown_tail (  &dns_query_ptr,
                                        qname_ptr,
                                        qname_len,
                                        DSM_DS_LARGE_ITEM_POOL ) )
  {
    LOG_MSG_ERROR_0("DSM pushdown failed");
    /* Dont free the DSM chain here, free it where we call this func. */
    PS_DNSI_MEM_FREE(qname_ptr);
    *ps_errno = DS_ESYSTEM;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Free qname data
  -------------------------------------------------------------------------*/
  PS_DNSI_MEM_FREE(qname_ptr);


  /*-------------------------------------------------------------------------
    Add RR type and RR class.
  -------------------------------------------------------------------------*/
  temp = dss_htons( rr_type );
  if( sizeof(uint16) != dsm_pushdown_tail( &dns_query_ptr,
                                           &temp,
                                           sizeof(uint16),
                                           DSM_DS_LARGE_ITEM_POOL ) )
  {
    LOG_MSG_ERROR_0("DSM pushdown failed");
    *ps_errno = DS_ESYSTEM;
    /* Dont free the DSM chain here, free it where we call this func. */
    return DSS_ERROR;
  }

  temp = dss_htons( rr_class );
  if( sizeof(uint16) != dsm_pushdown_tail( &dns_query_ptr,
                                           &temp,
                                           sizeof(uint16),
                                           DSM_DS_LARGE_ITEM_POOL ) )
  {
    LOG_MSG_ERROR_0("DSM pushdown failed");
    *ps_errno = DS_ESYSTEM;
    /* Dont free the DSM chain here, free it where we call this func. */
    return DSS_ERROR;
  }

  return DSS_SUCCESS;
} /* ps_dnsi_msg_mgri_add_question() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_EXTRACT_QUESTION

DESCRIPTION
  Extracts a DNS Question from the DSM item corresponding to the message.

PARAMETERS
  dns_response_ptr - DSM item pointer from where the resply has to extracted.
  msg_question_ptr - Question buffer where the reply has to be extracted to.
  errno            - Error code in case of error.

DEPENDENCIES
  If the caller wants the domain name to be returned in a msg_question_ptr
  argument, it must allocate memory before calling this function.
  If the caller is only interested in the offset, it must call
  using NULL as msg_question_ptr.

RETURN VALUE
  On success, returns offset after question in the DSM item chain.
  On error, returns DSS_ERROR and places error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - No memory for extracted domain name.

SIDE EFFECTS
  None.
===========================================================================*/
static int16 ps_dnsi_msg_mgri_extract_question
(
  dsm_item_type                     * dns_response_ptr,
  ps_dnsi_msg_mgr_question_type     * msg_question_ptr,
  int16                             * ps_errno
)
{
  uint16           rr_type;
  uint16           rr_class;
  char           * domain_name_ptr = NULL;
  int16            retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == dns_response_ptr )
  {
    LOG_MSG_ERROR_0("NULL DSM pointer");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  domain_name_ptr = (char *) ps_dnsi_mem_alloc( DSS_DNS_MAX_DOMAIN_NAME_LEN );
  if( NULL == domain_name_ptr )
  {
    LOG_MSG_ERROR_0("No memory for extracted domain name");
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    The question section always begins after 12 bytes of DNS header
    in the response message.
  ------------------------------------------------------------------------*/
  retval = ps_dnsi_msg_mgri_extract_domain_name( dns_response_ptr,
                                                 sizeof(ps_dnsi_msg_mgr_dns_hdr_type),
                                                 domain_name_ptr );
  if( DSS_ERROR == retval )
  {
    LOG_MSG_ERROR_0("Invalid domain name in question section");
    *ps_errno = DS_NAMEERR;
    PS_DNSI_MEM_FREE(domain_name_ptr);
    return DSS_ERROR;
  }

  if( sizeof(uint16) != dsm_extract( dns_response_ptr,
                                     retval,
                                     &rr_type,
                                     sizeof(uint16) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_DNSI_MEM_FREE(domain_name_ptr);
    return DSS_ERROR;
  }
  retval += sizeof(uint16);
  if( sizeof(uint16) != dsm_extract( dns_response_ptr,
                                     retval,
                                     &rr_class,
                                     sizeof(uint16) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_DNSI_MEM_FREE(domain_name_ptr);
    return DSS_ERROR;
  }
  retval += sizeof(uint16);

  /*-------------------------------------------------------------------------
    NULL question pointer is a valid argument if we are planning to ignore
    the response.
  -------------------------------------------------------------------------*/
  if( NULL != msg_question_ptr )
  {
    if( NULL == msg_question_ptr->query )
    {
      LOG_MSG_ERROR_0("ps_dnsi_msg_mgri_extract_question(): msg_question_ptr->query is NULL");
      *ps_errno = DS_EFAULT;
      PS_DNSI_MEM_FREE(domain_name_ptr);
      return DSS_ERROR;
    }

    (void) strlcpy (msg_question_ptr->query,
                        domain_name_ptr,
                        DSS_DNS_MAX_DOMAIN_NAME_LEN);

    msg_question_ptr->qtype  = dss_ntohs( rr_type );
    msg_question_ptr->qclass = dss_ntohs( rr_class );
  }

  /*-------------------------------------------------------------------------
    Free the memory allocated for domain name pointer.
  -------------------------------------------------------------------------*/
  PS_DNSI_MEM_FREE(domain_name_ptr);
  return retval;

}/* ps_dnsi_msg_mgri_extract_question() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGRI_GET_GENERIC_RR

DESCRIPTION
  Extracts the answer section from the DSM item chain corresponding
  to the DNS response message and forms a generic RR of type
  ps_dnsi_generic_rr_type.

  DNS Answer/Authority/Additional info message format:
                                  1  1  1  1  1  1
    0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                                               |
  /                                               /
  /                      NAME                     /
  |                                               |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                      TYPE                     |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                     CLASS                     |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                      TTL                      |
  |                                               |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
  |                   RDLENGTH                    |
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
  /                     RDATA                     /
  /                                               /
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

  NAME      - domain name, that can possibly contain pointers. (Variable length)
  TYPE      - Query Type (2 bytes)
  CLASS     - Query class (2 bytes)
  TTL       - Time to Live (4 bytes)
  RDLENGTH  - RDATA length (2 bytes)
  RDATA     - Additional info (Variable length)

PARAMETERS

  dns_response_ptr - DSM item pointer from where the resply has to extracted.
  offset_ptr       - Offset into DSM chain to read from(also return value)
  errno            - Error code in case of error.

DEPENDENCIES
  Memory is allocated for the RR node.

RETURN VALUE
  On success, returns pointer to the Node created
  On error, returns NULL and places error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ENOMEM         - Out of memory

SIDE EFFECTS
  None.
===========================================================================*/
ps_dnsi_generic_rr_type  * ps_dnsi_msg_mgri_get_generic_rr
(
  dsm_item_type               * dns_response_ptr,
  int16                       * offset_ptr,
  int16                       * ps_errno
)
{
  uint16                        rr_type;
  uint16                        rr_class;
  uint32                        ttl;
  ps_dnsi_generic_rr_type     * rr_node;
  uint16                        rdata_len;
  boolean                       error_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return NULL;
  }

  if( NULL == dns_response_ptr || NULL == offset_ptr )
  {
    LOG_MSG_ERROR_0("NULL arguments");
    *ps_errno = DS_EFAULT;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate memory for RR node from PS mem.
  -------------------------------------------------------------------------*/
  rr_node = ( ps_dnsi_generic_rr_type * )
    ps_mem_get_buf( PS_MEM_DNSI_MSG_RR_TYPE );
  if( NULL == rr_node )
  {
    *ps_errno = DS_ENOMEM;
    return NULL;
  }
  (void) q_link( rr_node, &(rr_node->link) );

  /*-------------------------------------------------------------------------
    Extract domain name, rr_type, rr_class, TTL and RDATA from the message.
  -------------------------------------------------------------------------*/
  *offset_ptr = ps_dnsi_msg_mgri_extract_domain_name( dns_response_ptr,
                                                      *offset_ptr,
                                                      rr_node->domain_name );
  if( DSS_ERROR == *offset_ptr )
  {
    LOG_MSG_ERROR_0("Error Extracting domain name");
    PS_MEM_FREE(rr_node);
    return NULL;
  }

  if( sizeof(uint16) != dsm_extract(  dns_response_ptr,
                                      *offset_ptr,
                                      &rr_type,
                                      sizeof(uint16) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_MEM_FREE(rr_node);
    return NULL;
  }
  rr_type = dss_ntohs( rr_type );

  *offset_ptr += sizeof(uint16);
  if( sizeof(uint16) != dsm_extract(  dns_response_ptr,
                                      *offset_ptr,
                                      &rr_class,
                                      sizeof(uint16) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_MEM_FREE(rr_node);
    return NULL;
  }
  rr_class = dss_ntohs( rr_class );

  *offset_ptr += sizeof(uint16);
  if( sizeof(uint32) != dsm_extract(  dns_response_ptr,
                                      *offset_ptr,
                                      &ttl,
                                      sizeof(uint32) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_MEM_FREE(rr_node);
    return NULL;
  }
  *offset_ptr += sizeof(uint32);
  ttl = dss_ntohl( ttl );

  if( sizeof(uint16) != dsm_extract(  dns_response_ptr,
                                      *offset_ptr,
                                      &rdata_len,
                                      sizeof(uint16) ) )
  {
    LOG_MSG_ERROR_0("DSM extract failed");
    PS_MEM_FREE(rr_node);
    return NULL;
  }
  *offset_ptr += sizeof(uint16);
  rdata_len = dss_ntohs( rdata_len );

  /*-------------------------------------------------------------------------
    Validate rdata_len here. We dont want to allocate memory
    for a random amount.
    For TYPE_A RR, rdata_len should be 4 bytes.
    For TYPE_AAAA RR, rdata_len should be 16 bytes.
    For TYPE_CNAME or TYPE_PTR, rdata_len should not exceed
    DSS_DNS_MAX_DOMAIN_NAME_LEN
  -------------------------------------------------------------------------*/
  if( (uint16)CLASS_IN != rr_class )
  {
    LOG_MSG_ERROR_1("Ignoring RR with unsupported class %d", rr_class);
    PS_MEM_FREE(rr_node);
    *offset_ptr += rdata_len;
    return NULL;
  }

  if( 0 == rdata_len ||
      !( ((uint16)TYPE_A == rr_type && sizeof(uint32) == rdata_len) ||
         ((uint16)TYPE_AAAA == rr_type && 2*sizeof(uint64) == rdata_len) ||
         ((uint16)TYPE_CNAME == rr_type && DSS_DNS_MAX_DOMAIN_NAME_LEN >= rdata_len) ||
         ((uint16)TYPE_PTR == rr_type && DSS_DNS_MAX_DOMAIN_NAME_LEN >= rdata_len) ) )
  {
    LOG_MSG_ERROR_2("Invalid RR leng %d for rr type %d", rdata_len, rr_type);
    PS_MEM_FREE(rr_node);
    *offset_ptr += rdata_len;
    return NULL;
  }

  if( (uint16)TYPE_A == rr_type || (uint16)TYPE_AAAA == rr_type )
  {
    rr_node->rdata = ps_dnsi_mem_alloc(rdata_len);
  }
  else if( (uint16)TYPE_CNAME == rr_type || (uint16)TYPE_PTR == rr_type )
  {
    /*-----------------------------------------------------------------------
      In case of compressed CNAME RRs the actual length of the CNAME
      can be more than rdata_len.
    -----------------------------------------------------------------------*/
    rr_node->rdata = ps_dnsi_mem_alloc( DSS_DNS_MAX_DOMAIN_NAME_LEN );
  }
  else
  {
    LOG_MSG_ERROR_1("Error invalid rr_type %d", rr_type);
    *ps_errno = DS_EFAULT;
    PS_MEM_FREE(rr_node);
    *offset_ptr += rdata_len;
    return NULL;
  }

  if( NULL == rr_node->rdata )
  {
    LOG_MSG_ERROR_0("Mem alloc for RDATA failed");
    *ps_errno = DS_ENOMEM;
    PS_MEM_FREE(rr_node);
    *offset_ptr += rdata_len;
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Store the extracted values in Generic RR node.
  -------------------------------------------------------------------------*/
  rr_node->rr_class = PS_DNSI_QUERY_CLASS_IN;
  rr_node->ttl = ttl;
  rr_node->rdata_len = rdata_len;

  /*-------------------------------------------------------------------------
    No need to store the RR if it is not of known type.
    Do further error checks for malformed packets and ignore them.
  -------------------------------------------------------------------------*/
  switch( rr_type )
  {
    case TYPE_A:
      rr_node->rr_type = PS_DNSI_QUERY_TYPE_A;
      /*---------------------------------------------------------------------
        rdata should contain a valid IPv4 address in network byte order.
      ---------------------------------------------------------------------*/
      if( rdata_len != dsm_extract(  dns_response_ptr,
                                     *offset_ptr,
                                     rr_node->rdata,
                                     rdata_len ) )
      {
        LOG_MSG_ERROR_0("DSM extract failed");
        error_flag = TRUE;
      }
      *offset_ptr += rdata_len;
      break;

    case TYPE_AAAA:
      rr_node->rr_type = PS_DNSI_QUERY_TYPE_AAAA;
      /*---------------------------------------------------------------------
        rdata should contain a valid IPv6 address in network byte order.
      ---------------------------------------------------------------------*/
      if( rdata_len != dsm_extract(  dns_response_ptr,
                                     *offset_ptr,
                                     rr_node->rdata,
                                     rdata_len ) )
      {
        LOG_MSG_ERROR_0("DSM extract failed");
        error_flag = TRUE;
      }
      *offset_ptr += rdata_len;
      break;

    case TYPE_PTR:
      rr_node->rr_type = PS_DNSI_QUERY_TYPE_PTR;
      /*---------------------------------------------------------------------
        rdata should contain a valid qname string.
      ---------------------------------------------------------------------*/
      if( DSS_ERROR ==
            ps_dnsi_msg_mgri_extract_domain_name( dns_response_ptr,
                                                  *offset_ptr,
                                                  (char *)rr_node->rdata ) )
      {
        LOG_MSG_ERROR_0("Error extracting PTR RR");
        error_flag = TRUE;
      }
      *offset_ptr += rdata_len;
      break;

    case TYPE_CNAME:
      rr_node->rr_type = PS_DNSI_QUERY_TYPE_CNAME;
      /*---------------------------------------------------------------------
        rdata should contain a valid qname string.
      ---------------------------------------------------------------------*/
      if( DSS_ERROR ==
            ps_dnsi_msg_mgri_extract_domain_name( dns_response_ptr,
                                                  *offset_ptr,
                                                  (char *)rr_node->rdata ) )
      {
        LOG_MSG_ERROR_0("Error extracting PTR RR");
        error_flag = TRUE;
      }
      *offset_ptr += rdata_len;
      break;

    default:
      error_flag = TRUE;
      break;
  }

  if( error_flag )
  {
    LOG_MSG_INFO1_1("Ignoring Malformed or unsupported %d reply msg", rr_type);
    PS_DNSI_MEM_FREE(rr_node->rdata);
    PS_MEM_FREE(rr_node);
    return NULL;
  }

  return ( rr_node );
} /* ps_dnsi_msg_mgri_get_generic_rr() */


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_INIT()

DESCRIPTION
  Initializes the msg manager module during power-up.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  The Queue utils subsystem must be initialzed before this.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_dnsi_msg_mgr_init
(
 void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pool
  -------------------------------------------------------------------------*/
  if (PS_MEM_POOL_INIT_OPT(PS_MEM_DNSI_MSG_RR_TYPE,
                           ps_dnsi_msg_rr_buf_mem,
                           PS_DNSI_MSG_RR_BUF_SIZE,
                           PS_DNSI_MSG_RR_BUF_NUM,
                           PS_DNSI_MSG_RR_BUF_HIGH_WM,
                           PS_DNSI_MSG_RR_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ps_dnsi_msg_rr_buf_hdr,
                           (int *) ps_dnsi_msg_rr_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    LOG_MSG_ERROR_0("Can't init the module");
  }

  (void) q_init( &ps_dnsi_msg_mgr_rr_q );
}

/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_RESOLVE_WILDCARD_QUERY()

DESCRIPTION
  Check if the query is a "wildcard query": a NULL hostname with 
  AI_PASSIVE flag set. If so returns the result as a generic RR Q node 
  in the Q parameter passed in.

IMPLEMENTATION NOTES
  A wildcard query is translated to "\0" by dss_dnsi_construct_queries(). 
  A hostname with "\0" would not pass hostname validation, thus "\0" is 
  sufficient to positively identify a wildcard query.

  If AI_PASSIVE was not set and hostname was NULL, it would have been 
  translated to "localhost." which is resolved in the usual way and is not
  covered by this function.

PARAMETERS
  query_data_ptr       - Pointer to the query data
  query_type           - Type of query
  query_class          - Class of query
  rr_q                 - Pointer to the Q where the results should be
                         returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return DSS_SUCCESS and set the rr_q parameter
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_resolve_wildcard_query
(
  ps_dnsi_query_type_enum_type    query_type,
  ps_dnsi_query_class_enum_type   query_class,
  char                          * query_data_ptr,
  q_type                        * rr_q,
  int16                         * ps_errno
)
{
  struct ps_in_addr               temp_in_addr;
  struct ps_in6_addr              temp_in6_addr = PS_IN6ADDR_ANY_INIT;
  ps_dnsi_generic_rr_type       * rr_node = NULL;

  temp_in_addr.ps_s_addr = PS_INADDR_ANY;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ps_errno)
  {
    LOG_MSG_ERROR_0("ps_dnsi_msg_mgr_resolve_wildcard_query(): ps_errno is NULL");
    return DSS_ERROR;
  }
  
  if( NULL == rr_q || NULL == query_data_ptr )
  {
    LOG_MSG_ERROR_0("ps_dnsi_msg_mgr_resolve_wildcard_query(): NULL parameters");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( ( PS_DNSI_QUERY_TYPE_A != query_type &&
        PS_DNSI_QUERY_TYPE_AAAA != query_type ) ||
      PS_DNSI_QUERY_CLASS_IN != query_class )
  {
    LOG_MSG_INFO1_0("Query type/class are not relevant to wildcard query check.");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  // wildcard query check - see "Implementation notes" above
  if (0 != strlen(query_data_ptr)) 
  {
    LOG_MSG_INFO1_0("Not a wildcard query.");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  rr_node = ( ps_dnsi_generic_rr_type * )
    ps_mem_get_buf( PS_MEM_DNSI_MSG_RR_TYPE );
  if( NULL == rr_node )
  {
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  memset( rr_node, 0, sizeof(ps_dnsi_generic_rr_type) );
  (void) q_link( rr_node, &(rr_node->link) );

  rr_node->rr_class = PS_DNSI_QUERY_CLASS_IN;
  rr_node->ttl = 0;
  if( PS_DNSI_QUERY_TYPE_A == query_type)
  {
    rr_node->rr_type = PS_DNSI_QUERY_TYPE_A;
    rr_node->rdata_len = sizeof(struct ps_in_addr);
    rr_node->rdata = ps_dnsi_mem_alloc( rr_node->rdata_len );
    if( NULL == rr_node->rdata )
    {
      LOG_MSG_ERROR_0("Not enough memory for getting rdata");
      PS_MEM_FREE(rr_node);
      *ps_errno = DS_ENOMEM;
      return DSS_ERROR;
    }
    memscpy( rr_node->rdata, sizeof(struct ps_in_addr),
      &(temp_in_addr), sizeof(struct ps_in_addr) );
  }
  else // PS_DNSI_QUERY_TYPE_AAAA
  {
    rr_node->rr_type = PS_DNSI_QUERY_TYPE_AAAA;
    rr_node->rdata_len = sizeof(struct ps_in6_addr);
    rr_node->rdata = ps_dnsi_mem_alloc( rr_node->rdata_len );
    if( NULL == rr_node->rdata )
    {
      LOG_MSG_ERROR_0("Not enough memory for getting rdata");
      PS_MEM_FREE(rr_node);
      *ps_errno = DS_ENOMEM;
      return DSS_ERROR;
    }
    memscpy( rr_node->rdata, sizeof(struct ps_in6_addr),
      &(temp_in6_addr), sizeof(struct ps_in6_addr) );
  }

  q_put( rr_q, &(rr_node->link) );
  return DSS_SUCCESS;

} /* ps_dnsi_msg_mgr_resolve_wildcard_query() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_RESOLVE_NUMERICAL_QUERY()

DESCRIPTION
  Checks if the query passed in IPv4 dotted decimal or IPv6 colon seperated
  format. If so returns the result as a generic RR Q node in the Q parameter
  passed in.

PARAMETERS
  query_data_ptr       - Pointer to the query data
  query_type           - Type of query
  query_class          - Class of query
  rr_q                 - Pointer to the Q where the results should be
                         returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return DSS_SUCCESS and set the rr_q parameter
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_resolve_numerical_query
(
  ps_dnsi_query_type_enum_type         query_type,
  ps_dnsi_query_class_enum_type        query_class,
  char                               * query_data_ptr,
  q_type                             * rr_q,
  int16                              * ps_errno
)
{
  int32                                retval = DSS_ERROR;
  boolean                              is_query_numerical = FALSE;
  struct ps_in_addr                    temp_in_addr;
  struct ps_in6_addr                   temp_in6_addr;
  ps_dnsi_generic_rr_type            * rr_node = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ps_errno)
  {
    LOG_MSG_ERROR_0("ps_dnsi_msg_mgr_resolve_wildcard_query(): ps_errno is NULL");
    return DSS_ERROR;
  }

  if( ( PS_DNSI_QUERY_TYPE_A != query_type &&
        PS_DNSI_QUERY_TYPE_AAAA != query_type ) ||
      PS_DNSI_QUERY_CLASS_IN != query_class )
  {
    LOG_MSG_INFO1_0("Query is not numerical");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( NULL == rr_q || NULL == query_data_ptr )
  {
    LOG_MSG_ERROR_0("ps_dnsi_msg_mgr_resolve_numerical_query(): NULL parameters");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  rr_node = ( ps_dnsi_generic_rr_type * )
    ps_mem_get_buf( PS_MEM_DNSI_MSG_RR_TYPE );
  if( NULL == rr_node )
  {
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  memset( rr_node, 0, sizeof(ps_dnsi_generic_rr_type) );
  (void) q_link( rr_node, &(rr_node->link) );

  if( PS_DNSI_QUERY_TYPE_A == query_type)
  {
     retval = dss_inet_pton( query_data_ptr,
                             DSS_AF_INET,
                             &temp_in_addr,
                             sizeof(struct ps_in_addr),
                             ps_errno );
     if( DSS_SUCCESS == retval )
     {
       is_query_numerical = TRUE;
       rr_node->rr_type = PS_DNSI_QUERY_TYPE_A;
       rr_node->rr_class = PS_DNSI_QUERY_CLASS_IN;
       rr_node->ttl = 0;
       (void) strlcpy (rr_node->domain_name,
                           query_data_ptr,
                           DSS_DNS_MAX_DOMAIN_NAME_LEN);
       rr_node->rdata_len = sizeof(struct ps_in_addr);
       rr_node->rdata = ps_dnsi_mem_alloc( rr_node->rdata_len );
       if( NULL == rr_node->rdata )
       {
         LOG_MSG_ERROR_0("Not enough memory for getting rdata");
         PS_MEM_FREE(rr_node);
         return DSS_ERROR;
       }
       memscpy( rr_node->rdata, sizeof(struct ps_in_addr),
         &(temp_in_addr), sizeof(struct ps_in_addr) );
     }
     else // check if it isn't a numerical address of a wrong address family
     {    // otherwise will get stuck when forming a query (no terminating dot)
        retval = dss_inet_pton( query_data_ptr,
                                DSS_AF_INET6,
                                &temp_in6_addr,
                                sizeof(struct ps_in6_addr),
                                ps_errno );
        if( DSS_SUCCESS == retval )
        {
           PS_MEM_FREE(rr_node);
           // return zero results
           return DSS_SUCCESS;
        }
     }
  }
  else if ( PS_DNSI_QUERY_TYPE_AAAA == query_type)
  {
    retval = dss_inet_pton( query_data_ptr,
                            DSS_AF_INET6,
                            &temp_in6_addr,
                            sizeof(struct ps_in6_addr),
                            ps_errno );
    if( DSS_SUCCESS == retval )
    {
      is_query_numerical = TRUE;
      rr_node->rr_type = PS_DNSI_QUERY_TYPE_AAAA;
      rr_node->rr_class = PS_DNSI_QUERY_CLASS_IN;
      rr_node->ttl = 0;
      (void) strlcpy (rr_node->domain_name,
                          query_data_ptr,
                          DSS_DNS_MAX_DOMAIN_NAME_LEN);
      rr_node->rdata_len = sizeof(struct ps_in6_addr);
      rr_node->rdata = ps_dnsi_mem_alloc( rr_node->rdata_len );
      if( NULL == rr_node->rdata )
      {
        LOG_MSG_ERROR_0("Not enough memory for getting rdata");
        PS_MEM_FREE(rr_node);
        return DSS_ERROR;
      }
      memscpy( rr_node->rdata, sizeof(struct ps_in6_addr),
        &(temp_in6_addr), sizeof(struct ps_in6_addr) );
    }
    else // check if it isn't a numerical address of a wrong address family
    {    // otherwise will get stuck when forming a query (no terminating dot)
      retval = dss_inet_pton( query_data_ptr,
                             DSS_AF_INET,
                             &temp_in_addr,
                             sizeof(struct ps_in_addr),
                             ps_errno );
      if( DSS_SUCCESS == retval )
      {
        PS_MEM_FREE(rr_node);
        // return zero results
        return DSS_SUCCESS;
      }
    }
  }

  if( FALSE == is_query_numerical )
  {
     PS_MEM_FREE(rr_node);
     return DSS_ERROR;
  }

  q_put( rr_q, &(rr_node->link) );
  return DSS_SUCCESS;

} /* ps_dnsi_msg_mgr_resolve_numerical_query() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_FORM_QUERY()

DESCRIPTION
  Using query type, query class and query data; construct a DSM item chain
  corresponding to the DNS query message to be sent.

PARAMETERS
  domain_name_ptr      - Hostname argument(for A/AAAA/CNAME query) or
                         a.b.c.d.in-addr.arpa(for PTR query)
  query_type           - Type of query
  query_class          - Class of query
  is_recursion_desired - Should the request indicate DNS recursion
  dns_query_ptr_ptr    - DSM item pointer where the resultant query
                         would be returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return messag id for the formed message
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_form_query
(
  char                               * domain_name_ptr,
  ps_dnsi_query_type_enum_type         query_type,
  ps_dnsi_query_class_enum_type        query_class,
  boolean                              is_recursion_desired,
  dsm_item_type                     ** dns_query_ptr_ptr,
  int16                             *  ps_errno
)
{
  int16  msg_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("Form Query called for domain name 0x%p, qtype %d, qclass %d",
                  domain_name_ptr, query_type, query_class);

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }

  if( NULL == domain_name_ptr )
  {
    LOG_MSG_ERROR_0("NULL FQDN");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    We should make sure that we are not leaking any DSM items here.
    If *dsm_query_ptr_ptr is non-NULL, it may be a pointer to a DSM item.
  -------------------------------------------------------------------------*/
  if( NULL == dns_query_ptr_ptr || NULL != *dns_query_ptr_ptr )
  {
    LOG_MSG_ERROR_0("Invalid ptr");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Asks for a large dsm item to buffer the prepared DNS query packet.
  -------------------------------------------------------------------------*/
  if( NULL == (*dns_query_ptr_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) )
  {
    LOG_MSG_ERROR_0("No large DSM items");
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  msg_id = ps_dnsi_msg_mgri_add_dns_hdr( *dns_query_ptr_ptr, 
                                         is_recursion_desired,
                                         ps_errno );
  if( DSS_ERROR == msg_id )
  {
    LOG_MSG_ERROR_0("Can't build DNS header");
    dsm_free_packet( dns_query_ptr_ptr );
    return DSS_ERROR;
  }

  if( DSS_ERROR == ps_dnsi_msg_mgri_add_question( *dns_query_ptr_ptr,
                                                  domain_name_ptr,
                                                  query_type,
                                                  query_class,
                                                  ps_errno ) )
  {
    LOG_MSG_ERROR_0("Can't add question to DNS message");
    dsm_free_packet( dns_query_ptr_ptr );
    return DSS_ERROR;
  }

  return msg_id;
} /* ps_dnsi_msg_mgr_form_query() */


/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_PARSE_RESPONSE

DESCRIPTION
  Parse the DSM item chain response into set of RRs.
  A generic RR is a node for holding information from Answer,
  authority, and additional info RRs. The RR Queue is returned
  in rr_q variable, which should be initialized before calling
  this function.

PARAMETERS
  dns_response_ptr  - DSM item pointer to DNS response msg
  rr_q              - Out parameter. Q of generic RRs to be returned.
  errno             - Error code in case of error.

RETURN VALUE
  On success returns the msg_id of the parsed message.
  DSS_ERROR   - On error and puts the error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.
  DS_EHOSTNOTFOUND  - Host not found.
  DS_EOPNOTSUPP     - Option not supported.
  DS_EINVAL         - Invalid Operation.
  DS_ECONNREFUSED   - Connection refused by peer.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_parse_response
(
  ps_dnsi_query_type_enum_type          query_type,
  ps_dnsi_query_class_enum_type         query_class,
  char                                * query_data_ptr,
  dsm_item_type                       * dns_response_ptr,
  q_type                              * rr_q,
  int16                               * ps_errno
)
{
  ps_dnsi_generic_rr_type                * rr_node = NULL;
  ps_dnsi_msg_mgr_dns_hdr_type             dns_hdr;
  int16                                    offset = 0;
  uint16                                   index;
  int16                                    msg_id = 0;
  char                                   * cname_ptr = NULL;
  ps_dnsi_msg_mgr_question_type            response_question;
  ps_dnsi_msg_mgr_rr_type_enum_type        rr_type;
  ps_dnsi_msg_mgr_rr_class_enum_type       rr_class;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Parse response called for DSM item 0x%p", dns_response_ptr);

  if( NULL == ps_errno )
  {
    LOG_MSG_ERROR_0("NULL errno");
    return DSS_ERROR;
  }
  *ps_errno = DSS_SUCCESS;

  if( NULL == dns_response_ptr || NULL == rr_q || NULL == query_data_ptr)
  {
    LOG_MSG_ERROR_0("NULL args");
    *ps_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  if( PS_DNSI_MSG_MGR_DNS_HDR_SIZE != sizeof(dns_hdr) )
  {
    LOG_MSG_ERROR_1("wrong sizeof(dns_hdr): %d", sizeof(dns_hdr));
    *ps_errno = DS_ESYSTEM;
    return DSS_ERROR;
  }
  memset(&dns_hdr, 0, sizeof(dns_hdr));

  /*-------------------------------------------------------------------------
    Extract the DSM message header and validate message header.
  -------------------------------------------------------------------------*/
  msg_id = ps_dnsi_msg_mgri_extract_dns_hdr( dns_response_ptr,
                                             &dns_hdr,
                                             ps_errno) ;

  if( DSS_ERROR == msg_id )
  {
    LOG_MSG_ERROR_1("Error in DNS response message %d", *ps_errno);
    return DSS_ERROR;
  }
  
  LOG_MSG_INFO1_1("Parsing DNS Response msg_id 0x%x", msg_id);

  if( 0 == dns_hdr.num_answer_rr )
  {
    LOG_MSG_INFO1_0("DNS Response has 0 RRs in answer section");
    *ps_errno = DS_EHOSTNOTFOUND;
    return DSS_ERROR;
  }


  //MSR response should always copy back the question section from query..
  //HM can there be more than one questions?
  if( 1 != dns_hdr.num_questions )
  {
    LOG_MSG_ERROR_1("Invalid # of questions (%d) in response",
                    dns_hdr.num_questions);
    *ps_errno = DS_NAMEERR;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Extract the question section
  -------------------------------------------------------------------------*/
  memset (&response_question, 0, sizeof(response_question));
  response_question.query = ps_dnsi_mem_alloc (DSS_DNS_MAX_DOMAIN_NAME_LEN);
  if (NULL == response_question.query)
  {
    LOG_MSG_ERROR_0 ("No memory for comparing question attributes");
    *ps_errno = DS_ENOMEM;
    return DSS_ERROR;
  }
  
  offset  = ps_dnsi_msg_mgri_extract_question( dns_response_ptr,
                                               &response_question,
                                               ps_errno );
  if( DSS_ERROR == offset )
  {
    LOG_MSG_ERROR_0("malformed questions in response");
    *ps_errno = DS_NAMEERR;
    PS_DNSI_MEM_FREE(response_question.query);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Compare the query type, query class and question name.
  -------------------------------------------------------------------------*/
  switch (query_type)
  {
    case PS_DNSI_QUERY_TYPE_A:
      rr_type = TYPE_A;
      break;

    case PS_DNSI_QUERY_TYPE_AAAA:
      rr_type = TYPE_AAAA;
      break;

    case PS_DNSI_QUERY_TYPE_PTR:
      rr_type = TYPE_PTR;
      break;

    case PS_DNSI_QUERY_TYPE_CNAME:
      rr_type = TYPE_CNAME;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid query type %d ", query_type);
      *ps_errno = DS_EFAULT;
      return DSS_ERROR;
  } /* switch( query_type ) */

  switch( query_class )
  {
    case PS_DNSI_QUERY_CLASS_IN:
      rr_class = CLASS_IN;
      break;

    default:
      LOG_MSG_ERROR_1("Invalid query class %d", query_class);
      *ps_errno = DS_EFAULT;
      return DSS_ERROR;
  } /* switch( query_class ) */

  if ((uint16)rr_type != response_question.qtype ||
      (uint16)rr_class != response_question.qclass ||
      0 != strcasecmp (query_data_ptr, 
                        response_question.query))
  {
    LOG_MSG_ERROR_0 ("Malformed response");
    *ps_errno = DS_NAMEERR;
    PS_DNSI_MEM_FREE(response_question.query);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Free the memory allocated for response question.
  -------------------------------------------------------------------------*/
  PS_DNSI_MEM_FREE(response_question.query);
  
  /*-------------------------------------------------------------------------
    Sanity check to make sure we start with a clean queue.
   -------------------------------------------------------------------------*/
  if (0 != q_cnt( &ps_dnsi_msg_mgr_rr_q ))
  {
    LOG_MSG_ERROR_0 ("ps_dnsi_msg_mgr_parse_response(): ps_dnsi_msg_mgr_rr_q is not empty");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Fetch RRs in Answer section
  -------------------------------------------------------------------------*/
  //MSR we need not parse RRs in authoritative and additional sections. They
  //are used when DNS server is redirecting the resolver to some other
  //server. In this case we should perform another query to the new server. I
  //dont think we support this today. Let me ping Satish about what we want
  //to do here..
  for( index = 0; index < dns_hdr.num_answer_rr; index++ )
  {
    rr_node =
      ps_dnsi_msg_mgri_get_generic_rr( dns_response_ptr, &offset, ps_errno );
    if( NULL == rr_node )
    {
      LOG_MSG_ERROR_0("Generic RR not created");
      if ( DSS_ERROR == offset )
      {
        /*-----------------------------------------------------
          Lost the offset can't decode next RRs so break
          -----------------------------------------------------*/
        break;
      }
      /*-----------------------------------------------------
        Ignore this RR and try decode next RR
      -----------------------------------------------------*/
      continue; 
    }

    /*-----------------------------------------------------------------------
      Add the generic RR node to the R.
    -----------------------------------------------------------------------*/
    q_put( &ps_dnsi_msg_mgr_rr_q, &(rr_node->link) );

  } /* end for */

  /*-------------------------------------------------------------------------
    Consolidate the RR Q so that CNAMEs can go together with A/AAAA records.
  -------------------------------------------------------------------------*/
  while( NULL != (rr_node = q_get(&ps_dnsi_msg_mgr_rr_q)) )
  {
    if( PS_DNSI_QUERY_TYPE_CNAME != rr_node->rr_type )
    {
      q_put( rr_q, &(rr_node->link) );
    }
    else
    {
      if( cname_ptr == NULL )
      {
        cname_ptr = ps_dnsi_mem_alloc( DSS_DNS_MAX_DOMAIN_NAME_LEN );
        if( NULL == cname_ptr )
        {
          LOG_MSG_ERROR_0 ("ps_dnsi_msg_mgr_parse_response(): Can't allocate cname_ptr");
          *ps_errno = DS_ENOMEM;
          return DSS_ERROR;
        }
        (void) strlcpy (cname_ptr, 
                            (char *)rr_node->rdata, 
                            DSS_DNS_MAX_DOMAIN_NAME_LEN);
        if( NULL != query_data_ptr )
        {

          LOG_MSG_INFO2_0("ps_dnsi_msg_mgr_parse_response(): updating query ptr with Alias");
          (void) strlcpy (query_data_ptr, 
                              (char *)rr_node->rdata, 
                              DSS_DNS_MAX_DOMAIN_NAME_LEN);
        }
      }
      /* Free the RR node containing just the CNAME */
      PS_DNSI_MEM_FREE(rr_node->rdata);
      PS_MEM_FREE(rr_node);
    }
  }

  rr_node = q_check(rr_q);
  if( NULL != rr_node && NULL != cname_ptr )
  {
    (void) strlcpy( rr_node->domain_name, cname_ptr, DSS_DNS_MAX_DOMAIN_NAME_LEN );
  }
  PS_DNSI_MEM_FREE(cname_ptr);

  return msg_id;

} /* ps_dnsi_msg_mgr_parse_response() */

