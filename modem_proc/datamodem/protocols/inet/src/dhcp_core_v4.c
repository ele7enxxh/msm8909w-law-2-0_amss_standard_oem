/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D H C P _ C O R E _ V 4 . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol Core IPV4 message parser
  and message generatator.

  For details on the requirements for the parser, see the following:
  - RFC 2131 (DHCP)
  - RFC 2132 (DHCP Options)
  - RFC 3396 (DHCP long option encoding)
  (and potentially others)

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Initializations for single IP address, PS address manager:
  - dhcp_core_new must be called first to initialize a handle with the
    internal configuration.
  - dhcp_core_start must be called second to configure the handle with
    external interdependancies.

  Cleanup for any mp address manager:
  - dhcp_core_stop must be called to free the handle when the session
    is finished.  This will clean up the lower layer AM.

  All external functions of this file must be called from the same
  context.

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_core_v4.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/06/14    vl     BSS optimization changes
01/10/14    vl     Optimization of F3 messages
01/03/14    mp     Added the support for SIP server option.
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
06/11/12    ss     Client ID option should not be sent back to client.
08/29/11    ag     Added the support for NBNS option.
01/10/11    cp     Added few log messages.
11/26/10    cp     Fix for ignoring the MESSAGE option recieved in Parameter
                   Request List.
11/18/10    cp     Fix for handling the MESSAGE option with incorrect
                   length gracefully.
06/17/10    cp     Fix for not sending any packet back when server recieves a
                   DHCP RELEASE message.
05/03/10    cp     DHCP Soft AP changes.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
01/11/10    ss     Lint warning fixes.
01/07/09    am     Handle 0 length DHCP option cleanly.
10/23/08    am     Dont ASSERT if cookie_ptr check fails in core_stop().
10/22/08    am     Changes to ARP IP cache update if dst IP addr is bcast.
09/26/08    am     Changed malloc/free names for off target support.
05/03/07    ks     Disarding invalid Inform msg. Check to see if Dest addr is
                   not zero before sending a DHCP reply.
02/07/07    scb    Fixed Klocwork High errors
11/27/06    jd     Added DHCP message callbacks
11/06/06    jd     Fix DHCP broadcast flag
04/13/04    clp    Initial development work done.  Added comments.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include <string.h>
#include "dhcpi.h"
#include "ps_in.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_proto_utils.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* bootp core magic number (see RFC 2131, section 3) */
#define DHCP_MAGIC_NUM (dss_htonl(0x63825363))

/* Bootp flags field: Broadcast bit and the rest Must Be Zero */
#define DHCP_BOOTP_FLAG_BROADCAST (dss_htons(0x8000))
#define DHCP_BOOTP_FLAG_MBZ_MASK (~DHCP_BOOTP_FLAG_BROADCAST)

/* Max option len that we will attempt to send without splitting */
#define DHCP_MAX_OPT_LEN (254)

/* The range of max message lengths that we will attempt to send */
#define DHCP_MAX_MAX_REPLY_LEN (1500)
#define DHCP_MIN_MAX_REPLY_LEN (576)

/* Error checking macro */
#define DHCP_SET_IF_FAIL( pass, expr ) \
  ((pass) = (expr) ? (pass) : FALSE)

/* INPUT convienence macros */
#define DHCP_COPY_INPUT_OPTION_N_BYTES(info, field, N) \
  do { \
  (void)(info)->extract( (info)->core.field, (info)->msg, (info)->offset, (N) ); \
  (info)->offset += (N); } while (0)

#define DHCP_COPY_INPUT_OPTION_4_BYTES(info, field) \
  do { \
  (void)(info)->extract( &(info)->core.field, (info)->msg, (info)->offset, 4 ); \
  (info)->offset += 4; } while (0)

#define DHCP_COPY_INPUT_OPTION_2_BYTES(info, field) \
  do { \
  (void)(info)->extract( &(info)->core.field, (info)->msg, (info)->offset, 2 ); \
  (info)->offset += 2; } while (0)

#define DHCP_COPY_INPUT_OPTION_1_BYTES(info, field) \
  (void)(info)->extract( &(info)->core.field, (info)->msg, (info)->offset++, 1 )

/* Output convienence macros */
#define DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, field, N) \
  DHCP_SET_IF_FAIL( (pass), \
                    (info)->config.server_reply_pushdown_tail \
                    ( \
                      (info)->config.server_reply_handle, \
                      &(msg), \
                      (field), \
                      (N) \
                    ) )

#define DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 4)

#define DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 2)

#define DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, field) \
  DHCP_COPY_OUTPUT_OPTION_N_BYTES((pass), (msg), (info), &(field), 1)

#define DHCP_MAX_OPTION_HEADER_LEN (2)

/* These values per RFC 2132, 9.3 */
#define DHCP_OPTION_OVERLOAD_FILE (1)
#define DHCP_OPTION_OVERLOAD_SNAME (2)
#define DHCP_OPTION_OVERLOAD_FILE_SNAME (3)

/* Option values we need to know about.  See RFC 2132 */
typedef enum {
  DHCP_OPTION_PAD           = 0,
  DHCP_OPTION_END           = 255,
  DHCP_OPTION_SUBNET_MASK   = 1,
  DHCP_OPTION_ROUTER        = 3,
  DHCP_OPTION_DNS           = 6,
  DHCP_OPTION_HOSTNAME      = 12,
  DHCP_OPTION_DOMAIN_NAME   = 15,
  DHCP_OPTION_MTU           = 26,
  DHCP_OPTION_NBNS          = 44,
  DHCP_OPTION_REQUESTED_IP  = 50,
  DHCP_OPTION_IP_LEASE      = 51,
  DHCP_OPTION_OVERLOAD      = 52,
  DHCP_OPTION_MSG_TYPE      = 53,
  DHCP_OPTION_SERVER_ID     = 54,
  DHCP_OPTION_PARAMETER_REQ = 55,
  DHCP_OPTION_MESSAGE       = 56,
  DHCP_OPTION_MAX_SIZE      = 57,
  DHCP_OPTION_RENEW_TIME    = 58,
  DHCP_OPTION_REBIND_TIME   = 59,
  DHCP_OPTION_CLIENT_ID     = 61,
  DHCP_OPTION_SIP           = 120
} dhcp_option_enum;

/* States for parsing options */
typedef enum {
  DHCP_OPTION_RET_SUCCESS = 0,
  DHCP_OPTION_RET_FAIL    = 1,
  DHCP_OPTION_RET_DONE    = 2
} dhcp_option_ret_enum;

#if 0
/* DNS IP list structure */
typedef struct dhcp_core_dns {
  struct dhcp_core_dns * next;
  uint32                 dns_server;
} dhcp_core_dns;
#endif /* 0 */

/* Option bookkeeping info */
typedef struct dhcp_meta_option {
  struct dhcp_meta_option * next;
  dhcp_option_enum option;
  uint32 len;
  uint8 * option_txt;
} dhcp_meta_option;

/* Core bookkeeping info */
typedef struct dhcp_core_info {
  /* Parsed and collected info: */
  dhcp_core_msg_info core;
  /* init info */
  dhcp_core_init init;
  /* config info */
  dhcp_core_config config;
  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_core_info * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_CORE_COOKIE ((uint32)0xC03EC03E)
  uint32 cookie;
  /* Input message */
  void * msg;
  boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len);
  uint32 offset;
  /* Option pointers */
  dhcp_meta_option * gather_options;
  dhcp_meta_option * current_option;
  dhcp_meta_option * output_options;
  /* The total length of the output options. */
  uint32 output_option_size;
  /* The maximum length of the output options, as requested by the client */
  uint32 output_option_max_size;

  /* Per RFC2131 4.1, handle overloads after processing the option field,
   * and in a particular order.
   */
  uint8 overload;

#if 0
  /* DNS server addresses */
  dhcp_core_dns * dns_servers;
#endif /* 0 */

  /* LEASE options allow */
  boolean          lease_option_allow;
} dhcp_core_info;

typedef struct
{
  dhcp_option_enum op;
  dhcp_option_ret_enum (*handler)(dhcp_core_info *);
} dhcp_option_lookup_table_type;

/* Max number of SIP servers supported. */
#define DHCP_MAX_SIP_SERVERS  (0x04)
/*---------------------------------------------------------------------------
  Temporary global memory (scratch pad for CORE Module)
---------------------------------------------------------------------------*/
typedef union
{
  ip_addr_type sip_addr[DHCP_MAX_SIP_SERVERS];
  ps_iface_domain_name_type sip_domain_name[DHCP_MAX_SIP_SERVERS];
} dhcp_core_v4_temp_info_type;

/*===========================================================================

                           FORWARD REFERENCES

===========================================================================*/
static dhcp_option_ret_enum
  dhcpi_opt_parse_bootp_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_opt_parse_overload_handler( dhcp_core_info * info, uint8 optlen );

static dhcp_option_ret_enum
  dhcpi_option_bootp_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_subnet_mask_router_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_dns_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_nbns_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_hostname_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_client_id_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_domain_name_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_ip_lease_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_msg_type_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_server_id_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_parameter_req_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_max_size_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_renew_time_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_rebind_time_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_message_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_mtu_handler( dhcp_core_info * );
static dhcp_option_ret_enum
  dhcpi_option_sip_handler( dhcp_core_info * );


/*===========================================================================

                           STATIC DATA

===========================================================================*/
static const dhcp_option_lookup_table_type dhcp_option_lookup[] =
{
#define DHCP_OPTION_LOOKUP_ELEMENT(u,l) \
  { DHCP_OPTION_## u, dhcpi_option_## l ##_handler }
  DHCP_OPTION_LOOKUP_ELEMENT(SUBNET_MASK, subnet_mask_router),
  DHCP_OPTION_LOOKUP_ELEMENT(ROUTER, subnet_mask_router),
  DHCP_OPTION_LOOKUP_ELEMENT(DNS, dns),
  DHCP_OPTION_LOOKUP_ELEMENT(HOSTNAME, hostname),
  DHCP_OPTION_LOOKUP_ELEMENT(DOMAIN_NAME, domain_name),
  DHCP_OPTION_LOOKUP_ELEMENT(IP_LEASE, ip_lease),
  DHCP_OPTION_LOOKUP_ELEMENT(MSG_TYPE, msg_type),
  DHCP_OPTION_LOOKUP_ELEMENT(SERVER_ID, server_id),
  DHCP_OPTION_LOOKUP_ELEMENT(PARAMETER_REQ, parameter_req),
  DHCP_OPTION_LOOKUP_ELEMENT(MAX_SIZE, max_size),
  DHCP_OPTION_LOOKUP_ELEMENT(RENEW_TIME, renew_time),
  DHCP_OPTION_LOOKUP_ELEMENT(REBIND_TIME, rebind_time),
  DHCP_OPTION_LOOKUP_ELEMENT(MESSAGE, message),
  DHCP_OPTION_LOOKUP_ELEMENT(CLIENT_ID, client_id),
  DHCP_OPTION_LOOKUP_ELEMENT(MTU,mtu),
  DHCP_OPTION_LOOKUP_ELEMENT(NBNS,nbns),
  DHCP_OPTION_LOOKUP_ELEMENT(SIP,sip)
}; /* dhcp_option_lookup[] */
#define DHCP_OPTION_TABLE_SIZE \
  (sizeof(dhcp_option_lookup)/sizeof(dhcp_option_lookup_table_type))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                           UTILITY FUNCTIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_ALLOC_LINK()

DESCRIPTION
  This function allocates an option meta pointer thingy, initializes
  it and inserts it into the gather options list.

DEPENDENCIES
  parameters correct

PARAMETERS
  dhcp_core_info * - The handle associated with the current handle.
  dhcp_option_enum - The option number that we are handling.
  uint32 len - The extra length for the option text that we should
               allocate, or 0 if we shouldn't allocate anything extra
               for this option.
RETURN VALUE
  dhcp_option_ret_enum to indicate success/fail.

SIDE EFFECTS
  Adds the option to the info->gather options list.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_alloc_link
(
  dhcp_core_info   * info,
  dhcp_option_enum   option_num,
  uint32             len   /* of alloc and option */
)
{
  dhcp_meta_option * opt;
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  ASSERT(NULL != info);
  opt = info->init.d_malloc(sizeof(dhcp_meta_option) + len);
  if ( NULL == opt )
  {
    LOG_MSG_ERROR_0("Insufficent memory to alloc option link");
    ret = DHCP_OPTION_RET_FAIL;
  }
  else
  {
    /* Caution, pointer magic ahead */
    opt->option_txt = ( 0 != len ) ? (byte*)&opt[1] : NULL;
    opt->option = option_num;
    opt->len = len;
    opt->next = info->gather_options;
    info->gather_options = opt;
  }
  return ret;
} /* dhcpi_option_alloc_link */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_UNLINK()

DESCRIPTION
  This function unlinks the first specified option from the
  gather_options list and returns it.

DEPENDENCIES
  Input parameters correct.

PARAMETERS
  dhcp_core_info * - The handle to associated with the message.
  dhcp_option_enum - The option to look for.

RETURN VALUE
  The meta option for the requested option if found, or NULL is not.

SIDE EFFECTS
  Removes the specified option from the gather_options list.
===========================================================================*/
static dhcp_meta_option *
dhcpi_option_unlink
(
  dhcp_core_info * info,
  dhcp_option_enum option
)
{
  dhcp_meta_option ** optptr = &info->gather_options;
  dhcp_meta_option *  optret = NULL;

  ASSERT(NULL != info);
  while ( NULL != *optptr )
  {
    if ( (*optptr)->option == option )
    {
      optret = *optptr;
      *optptr = optret->next;
      optret->next = NULL;
      break;
    }
    optptr = &(*optptr)->next;
  }

  return optret;
} /* dhcpi_option_unlink */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_REALLOC_CURRENT_OPTION()

DESCRIPTION
  This function "reallocs" the current option to the specified length.
  This is accomplished by mallocing an option of the correct length,
  then copying the option over, including the optional val, if passed
  in.  The old option is freed.

DEPENDENCIES
  Parameters correct.

PARAMETERS
  dhcp_core_info - The handle associated with the current message.
  uint32 len - The length of the option text to allocate.

RETURN VALUE
  dhcp_option_ret_enum - Inidacates success or failure.

SIDE EFFECTS
  None
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_realloc_current_option
(
  dhcp_core_info   * info,
  uint32             len
)
{
  dhcp_meta_option * opt;
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  ASSERT(NULL != info);
  opt = info->init.d_malloc(sizeof(dhcp_meta_option) + len);
  if ( NULL == opt )
  {
    LOG_MSG_ERROR_0("Insufficent memory to alloc option link");
    ret = DHCP_OPTION_RET_FAIL;
  }
  else
  {
    if ( 0 != len )
    {
      /* Caution, pointer magic ahead */
      opt->option_txt = (byte*)&opt[1];
      memscpy(opt->option_txt,len,
             info->current_option->option_txt,
             MIN(info->current_option->len, len));
    }
    opt->len = len;
    opt->option = info->current_option->option;
    opt->next = NULL;
    info->init.d_free(info->current_option);
    info->current_option = opt;
  }
  return ret;
} /* dhcpi_option_realloc_current_option */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_PARSE()

DESCRIPTION
  Parse through the options that appear in a field in a recieved
  message.  This function will extract the info from each option and
  add it to a new meta_option and add this to the list of options
  found.

DEPENDENCIES
  Info valid.

PARAMETERS
  dhcp_core_info * - handle to instance message recieved on.
  int32 - Size of fixed length in option field, or 0 for unbound
          option field.

RETURN VALUE
  dhcp_option_ret_enum - state of message parsing, indicating done or
                         failure.

SIDE EFFECTS
  Offset field of info changed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_parse
(
  dhcp_core_info * info,
  int32 field_size
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;
  dhcp_option_enum     option;
  uint32               optlen = 0;
  dhcp_meta_option   * opt;
  boolean              bound = field_size?TRUE:FALSE;

  ASSERT(NULL != info);
  while ( DHCP_OPTION_RET_SUCCESS == ret )
  {
    option = DHCP_OPTION_PAD; /* Zero out any high bits */
    /* Get bytes and verify that we succeeded */
    if ( !(
           (! bound || 1 <= field_size) &&
           info->extract( &option, info->msg, info->offset++, 1)
          )
       )
    {
      ret = DHCP_OPTION_RET_FAIL;
      LOG_MSG_INFO2_0( "FAIL: End of options without END option" );
      break;
    }
    if ( DHCP_OPTION_END != option && DHCP_OPTION_PAD != option )
    {
      optlen = 0;
      if ( !( ( ! bound || DHCP_MAX_OPTION_HEADER_LEN <= field_size ) &&
           info->extract( &optlen, info->msg, info->offset++, 1) ) )
      {
        ret = DHCP_OPTION_RET_FAIL;
        LOG_MSG_INFO2_0( "FAIL: End of options midoption without END option");
        break;
      }
      field_size -= DHCP_MAX_OPTION_HEADER_LEN + (int32)optlen;
      LOG_MSG_INFO3_2( "INFO: Parse option %d len %lu", option, optlen );
    }
    else
    {
      field_size--;
      if( DHCP_OPTION_END == option )
      {
        LOG_MSG_INFO3_0( "INFO: Parse END" );
        ret = DHCP_OPTION_RET_DONE;
        /* We are done with this option field */
        break;
      }
      LOG_MSG_INFO3_0( "INFO: Parse PAD" );
      /* nothing more to process with a PAD */
      continue;
    }
    if ( bound && 0 > field_size )
    {
      LOG_MSG_INFO2_0( "FAIL: End of message mid-option" );
      ret = DHCP_OPTION_RET_FAIL;
      break;
    }
    if ( DHCP_OPTION_OVERLOAD == option )
    {
      ret = dhcpi_opt_parse_overload_handler(info, (uint8)optlen);
    }
    else
    {
      opt = dhcpi_option_unlink(info, option);
      if ( NULL == opt )
      {
        /* Common case */
        ret = dhcpi_option_alloc_link(info, option, optlen);
        if (ret != DHCP_OPTION_RET_SUCCESS)
        {
          LOG_MSG_ERROR_1("Out of mem for option %d", option);
          break;
        }
        else if (NULL != info->gather_options &&
                 NULL != info->gather_options->option_txt)
        {
          (void) info->extract(info->gather_options->option_txt,
                        info->msg, info->offset, optlen);
        }
      }
      else
      {
        /* Spanning options */
        LOG_MSG_INFO3_1( "INFO: Found spanning opt %d", option );
        ret = dhcpi_option_alloc_link(info, option, optlen + opt->len);
        if (ret != DHCP_OPTION_RET_SUCCESS)
        {
          LOG_MSG_ERROR_1("Out of mem for option %d", option);
          break;
        }
        else if (NULL != info->gather_options &&
                 NULL != info->gather_options->option_txt)
        {
          memscpy(info->gather_options->option_txt,
                  opt->len, 
                  opt->option_txt, 
                  opt->len);
          (void) info->extract(&info->gather_options->option_txt[opt->len],
                        info->msg, info->offset, optlen);
          info->init.d_free( opt );
        }
      }
    }
    info->offset += optlen;
  }
  return ret;
} /* dhcpi_option_parse */

/*===========================================================================
LOCAL FUNCTION DHCPI_OUTPUT_OPTIONS()

DESCRIPTION
  This function formats and "outputs" options.  The options to output
  are in info->output_options.

  This function seems overly complicated due to handling the case that
  the reply may be longer than the max size (perhaps as requested). In
  this case we overload other bootp field with options.  Another
  complicating factor is that "long" options can be split, not only
  into multiple options, but across multiple fields in the DHCP
  message.

  If there is an option that will not fit in all of the
  remaining room, that will be skipped since it can't be sent anyway,
  and following shorter options may be output in place.

DEPENDENCIES
  Parameters correct.

PARAMETERS
  dhcp_core_info - the handle for the instance we are generating a
                     message for.
  dhcp_meta_option - the head of a linked list of options that are to
                     be output.  This is a pointer into the
                     info->output_options list.
  void * msg - The handle of the output object.
  uint32 size - The size of the field that we are populating.
  boolean * pass - output parameter of function.  Set to FALSE if
                   there is any problem "writing" the "output"
  uint32 future - This is how many bytes are available in future
                  fields that can be populated with options.

RETURN VALUE
  dhcp_meta_option * - The next option that we are to output when we
                       ran out of room in this field, including
                       possibly the remainder of a split long option.

SIDE EFFECTS
  The option lengths in the dhcp_meta_option list are mucked with.
===========================================================================*/
static dhcp_meta_option *
dhcpi_output_options
(
 dhcp_core_info * info,
 dhcp_meta_option * opt,
 void * msg,
 uint32 size,
 boolean *pass,
 uint32 future
)
{
  dhcp_meta_option * seen;
  uint8 end = (uint8)DHCP_OPTION_END;
  uint32 len;
  uint32 used = 0;

  ASSERT(NULL != info);
#define DHCP_MIN_SPLIT_LEN ((0 == future)?opt->len:(3))
#define DHCP_WORST_CASE_HEADERS (2 * (3 + 1))
  /* We will maximally pack and split across field boundaries */
  while ( NULL != opt && used + DHCP_MIN_SPLIT_LEN <= size - 1 )
  {
    seen = info->output_options;
    while ( NULL != seen && seen != opt )
    {
      if ( seen->option == opt->option )
      {
        LOG_MSG_INFO3_1( "INFO: Not sending duplicate option %d",
                         opt->option);
        break;
      }
      seen = seen->next;
    }
    if ( seen == opt )
    {
      /* Check to see if the option exceeds the total remaining space,
       * and if so, skip this option
       */
      if ( size - used - 1 + future - DHCP_WORST_CASE_HEADERS >= opt->len )
      {
        do
        {
          len = MIN( opt->len, DHCP_MAX_OPT_LEN );
          /* 3 == 2 for the header + 1 for the END option */
          len = MIN( len, size - used - 3 );
          LOG_MSG_INFO3_3( "INFO: output option %u len %lu of %lu",
                           opt->option, len, opt->len );
          DHCP_COPY_OUTPUT_OPTION_1_BYTES(*pass, msg, info, opt->option);
          used++;
          if ( DHCP_OPTION_PAD != opt->option &&
               DHCP_OPTION_END != opt->option )
          {
            DHCP_COPY_OUTPUT_OPTION_1_BYTES(*pass, msg, info, len);
            if ( 0 != len )
            {
              DHCP_COPY_OUTPUT_OPTION_N_BYTES(*pass, msg, info,
                                              opt->option_txt, len);
              opt->option_txt += len;
            }
            opt->len -= len;
            used += len + 1;
          }
        } while ( 0 < opt->len && used + DHCP_MIN_SPLIT_LEN < size - 1 );
        /* If the len is not zero, we ran out of space for this split
         * option
         */
        if ( 0 != opt->len )
        {
          break;
        }
      }
      else
      {
        LOG_MSG_INFO2_2( "INFO: Dropping option too long %u len %lu",
                         opt->option, opt->len );
      }
    }
    opt = opt->next;
  }
  /* Add end option */
  LOG_MSG_INFO3_1( "INFO: output option %d", end );
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(*pass, msg, info, end);
  return opt;
} /* dhcpi_output_options */

/*===========================================================================
LOCAL FUNCTION DHCPI_CORE_PUSHDOWN_BUFFER()

DESCRIPTION
  This function has the same prototype as the
  server_reply_pushdown_tail in core_config.  This function is used to
  put the options into buffers in the unlikely case that we overflow
  into the overloaded fields.

DEPENDENCIES
  Buffer has sufficent space for the data being copied into it.

PARAMETERS
  void * handle - This would be the object handle.  In this case, this
                  is a pointer to the cumulative offset into the
                  buffer so far.
  void ** msg - This would be the object item that are are
                manipulating.  In this case, this is a pointer to the
                beginning of the buffer we are filling
  void * data - The data we are adding at offset into msg.
  uin32 size - The size of the data we are adding.

RETURN VALUE
  boolean - TRUE if successful, FALSE on error.  In this case, always
            successful.

SIDE EFFECTS
  Updates offset (handle)
===========================================================================*/
static boolean
dhcpi_core_pushdown_buffer
(
  void * handle,
  void ** msg,
  void * data,
  uint32 size
)
{
  uint32 * offset = handle;
  uint8 **buffer = (uint8**)msg;

  ASSERT(NULL != handle);
  ASSERT(NULL != buffer);
  ASSERT(NULL != *buffer);
  ASSERT(NULL != data);
  memscpy( *buffer + *offset,size, data, size );
  *offset += size;

  return TRUE;
} /* dhcpi_core_pushdown_buffer */

/*===========================================================================
LOCAL FUNCTION DHCPI_OUTPUT()

DESCRIPTION
  This function outputs the current message associated with info.

DEPENDENCIES
  info has a message to output.

PARAMETERS
  dhcp_core_info * - The handle with the message to send.

RETURN VALUE
  None

SIDE EFFECTS
  May mangle the options.
===========================================================================*/
static void
dhcpi_output
(
  dhcp_core_info * info
)
{
  void * msg;
  dhcp_meta_option * opt;
  uint32 ip_addr = 0;
  uint32 pushdown_len;
  void * pushdown_handle_save;
  boolean (* pushdown_func_save)(void * , void ** ,void * , uint32);
  boolean pass;
  uint8 * optionfield = NULL;
  uint8 overloadval = 0, overloadop;
  uint32 size;

  ASSERT(NULL != info);
  /* Add 1 for the end option */
  size = MIN( info->output_option_max_size,
              DHCP_BOOTP_SIZE + info->output_option_size + 1 );
  LOG_MSG_INFO3_1( "INFO: output bootp len %lu", size );
  pass = info->config.server_reply_new( info->config.server_reply_handle,
                                        &msg, size );

  size -= DHCP_BOOTP_SIZE; /* length of options field */

  if ( info->output_option_max_size <
       DHCP_BOOTP_SIZE + info->output_option_size + 1 )
  {
    size -= 3; /* For the OVERLOAD option */
    opt = info->output_options;
    optionfield = info->init.d_malloc( size );
    if ( NULL != optionfield )
    {
      /* set up to put the options in buffers */
      memset( optionfield, 0, size );
      pushdown_handle_save = info->config.server_reply_handle;
      pushdown_func_save = info->config.server_reply_pushdown_tail;
      info->config.server_reply_handle = &pushdown_len;
      info->config.server_reply_pushdown_tail = dhcpi_core_pushdown_buffer;
      /* build buffers */
      pushdown_len = 0;
      LOG_MSG_INFO3_1("INFO: populating option field len %ld", size);
      opt = dhcpi_output_options( info, info->output_options,
                                 optionfield, size, &pass,
                                 DHCP_FILE_LEN + DHCP_SNAME_LEN );
      if ( NULL != opt )
      {
        LOG_MSG_INFO3_1("INFO: populating sname field len %d", DHCP_SNAME_LEN);
        memset( info->core.sname, 0, DHCP_SNAME_LEN );
        pushdown_len = 0;
        opt = dhcpi_output_options( info, opt, info->core.sname,
                                   DHCP_SNAME_LEN, &pass, DHCP_FILE_LEN );
        overloadval = 2;
      }
      if ( NULL != opt )
      {
        LOG_MSG_INFO3_1("INFO: populating file field len %d", DHCP_FILE_LEN);
        memset( info->core.file, 0, DHCP_FILE_LEN );
        pushdown_len = 0;
        opt = dhcpi_output_options( info, opt, info->core.file,
                                   DHCP_FILE_LEN, &pass, 0 );
        overloadval = 3;
      }
      info->config.server_reply_handle = pushdown_handle_save;
      info->config.server_reply_pushdown_tail = pushdown_func_save;
    }
    else
    {
      LOG_MSG_ERROR_0("INFO: unable to malloc memory for option field");
    }
  }

  /* Order and sizes here are important.  See RFC2131 */
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.op);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.htype);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.hlen);
  DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, info->core.hops);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.xid);
  DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, info->core.secs);
  DHCP_COPY_OUTPUT_OPTION_2_BYTES(pass, msg, info, info->core.flags);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.ciaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.yiaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.siaddr);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.giaddr);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->core.chaddr,
                                  DHCP_CHADDR_LEN);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->core.sname,
                                  DHCP_SNAME_LEN);
  DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, info->core.file,
                                  DHCP_FILE_LEN);
  DHCP_COPY_OUTPUT_OPTION_4_BYTES(pass, msg, info, info->core.magic);
  if ( NULL == optionfield )
  {
    (void) dhcpi_output_options(info, info->output_options,
                                msg, size, &pass, 0);
  }
  else
  {
    /* Check to see that we actually used the overload fields.  If
     * not, we don't want to output the overload option.  This can
     * happen if we have a really long option that gets dropped, and
     * the rest of the options fit into the option field.
     */
    if ( 0 != overloadval )
    {
      /* Build the overload option here by hand */
      LOG_MSG_INFO3_1("INFO: output option DHCP_OPTION_OVERLOAD %d",
                      overloadval);
      overloadop = (uint8)DHCP_OPTION_OVERLOAD;
      DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, overloadop );
      overloadop = 1; /* len */
      DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, overloadop );
      DHCP_COPY_OUTPUT_OPTION_1_BYTES(pass, msg, info, overloadval );
    }
    /* now output the rest of the options */
    DHCP_COPY_OUTPUT_OPTION_N_BYTES(pass, msg, info, optionfield, size);
    info->init.d_free(optionfield);
  }
  if ( TRUE == pass )
  {
    /* The following reply address logic from RFC2131, 4,1:
     *   If the 'giaddr' field in a DHCP message from a client is
     *   non-zero, the server sends any return messages to the 'DHCP
     *   server' port on the BOOTP relay agent whose address appears
     *   in 'giaddr'. If the 'giaddr' field is zero and the 'ciaddr'
     *   field is nonzero, then the server unicasts DHCPOFFER and
     *   DHCPACK messages to the address in 'ciaddr'.  If 'giaddr' is
     *   zero and 'ciaddr' is zero, and the broadcast bit is set, then
     *   the server broadcasts DHCPOFFER and DHCPACK messages to
     *   0xffffffff. If the broadcast bit is not set and 'giaddr' is
     *   zero and 'ciaddr' is zero, then the server unicasts DHCPOFFER
     *   and DHCPACK messages to the client's hardware address and
     *   'yiaddr' address.  In all cases, when 'giaddr' is zero, the
     *   server broadcasts any DHCPNAK messages to 0xffffffff.
     */
    if ( 0 != info->core.giaddr )
    {
      ip_addr = info->core.giaddr;
    }
    else if ( DHCP_DHCPNACK == info->core.dhcp_msg_reply )
    {
      ip_addr = (uint32)0xFFFFFFFF;
    }
    else if ( 0 != info->core.ciaddr )
    {
      ip_addr = info->core.ciaddr;
    }
    else if ( DHCP_BOOTP_FLAG_BROADCAST & info->core.flags )
    {
      ip_addr = (uint32)0xFFFFFFFF;
    }
    else if ( 0 != info->core.yiaddr )
    {
      ip_addr = info->core.yiaddr;
    }
    else
    {
      LOG_MSG_ERROR_0("Invalid destination IP addr for server reply");
      pass = FALSE;
    }
  }

  if(pass == TRUE)
  {
    uint32 arp_ip_addr = 0;
    arp_ip_addr = (info->core.yiaddr) ? (info->core.yiaddr) :
                  (info->core.ciaddr) ? (info->core.ciaddr) : 0;

    /* Send the message along */
    info->config.server_reply_send( info->config.server_reply_handle,
                                    &msg,
                                    ip_addr,
                                    info->core.chaddr,
                                    info->core.hlen,
                                    arp_ip_addr );
  }
  else
  {
    LOG_MSG_INFO3_0( "FAIL: Failed to alloc reply" );
    info->config.server_reply_free( info->config.server_reply_handle,
                                    &msg );
  }
} /* dhcpi_output */

/*===========================================================================
LOCAL FUNCTION DHCPI_PARSE_MSG()

DESCRIPTION
  This function deals with parsing a message. Specifically, it calls
  the approprate parsers (bootp handle parser, option parser) for the
  message.  Also deals with option overloads.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_core_info * info - handle associated with the incoming
                            message.

RETURN VALUE
  dhcp_option_ret_enum - indicates success/done or failure.
  dhcp_core_info * - info parsed.

SIDE EFFECTS
  None
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_parse_msg
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret;

  ASSERT(NULL != info);
  info->overload = 0;
  info->offset = DHCP_BOOTP_OFFSET;
  ret = dhcpi_opt_parse_bootp_handler( info );
  if ( DHCP_OPTION_RET_SUCCESS == ret )
  {
    /* Per RFC 2131 4.1, handle option field, then file, then sname */
    info->offset = DHCP_OPTION_OFFSET;
    ret = dhcpi_option_parse( info, 0 );
    if( DHCP_OPTION_RET_DONE == ret &&
        ( DHCP_OPTION_OVERLOAD_FILE == info->overload ||
          DHCP_OPTION_OVERLOAD_FILE_SNAME == info->overload ) )
    {
      info->offset = DHCP_FILE_OFFSET;
      ret = dhcpi_option_parse( info, DHCP_FILE_LEN );
    }
    if( DHCP_OPTION_RET_DONE == ret &&
        ( DHCP_OPTION_OVERLOAD_SNAME  == info->overload ||
          DHCP_OPTION_OVERLOAD_FILE_SNAME == info->overload ) )
    {
      info->offset = DHCP_SNAME_OFFSET;
      ret = dhcpi_option_parse( info, DHCP_SNAME_LEN );
    }
  }
  return ret;
} /* dhcpi_parse_msg */

/*===========================================================================
LOCAL FUNCTION DHCPI_GATHER()

DESCRIPTION
  This function walks the list of options that was created when the
  incoming message was parsed, and calls the option specific handler
  for each of the known options to gather option specific information.
  Also handles the bootp message similarly by calling the bootp
  handler.

DEPENDENCIES
  input paramaters correct.

PARAMETERS
  dhcp_core_info - handle associated with current message.

RETURN VALUE
  dhcp_option_ret_enum - Success/done/Fail to indicate future action
                         on this message.
  dhcp_core_info * info - may be updated by option specific handlers
                            with info needed to reply to message.

SIDE EFFECTS
  - The option specific handlers may modify the option as needed for
    output.
  - The option specific handlers may remove an option from the list if
    not needed for output.
  - Options not understood will be removed from the list and freed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_gather
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret;
  uint32 i;
  dhcp_meta_option * tmp;

  ASSERT(NULL != info);
  ret = dhcpi_option_bootp_handler( info );
  while ( DHCP_OPTION_RET_SUCCESS == ret && NULL != info->gather_options)
  {
    /* Grab the next gather option */
    info->current_option = info->gather_options;
    info->gather_options = info->gather_options->next;
    info->current_option->next = NULL;
    /* find handler for current_option */
    for ( i = 0; DHCP_OPTION_TABLE_SIZE > i; i++ )
    {
      if ( info->current_option->option == dhcp_option_lookup[i].op ) {
        ret = dhcp_option_lookup[i].handler( info );
        break;
      }
    }
    if ( DHCP_OPTION_TABLE_SIZE == i )
    {
      info->init.d_free(info->current_option);
      info->current_option = NULL;
    }
    else
    {
      /* An option handler may output multiple options to handle corner cases
       * such as RFC3132, submask and router options.
       */
      while ( NULL != info->current_option )
      {
        /* Update the output size */
        info->output_option_size += info->current_option->len +
          ((info->current_option->len / DHCP_MAX_OPT_LEN) + 1 ) *
          DHCP_MAX_OPTION_HEADER_LEN;
        /* save off any additional "current" options */
        tmp = info->current_option->next;
        /* add current option to output option list */
        info->current_option->next = info->output_options;
        info->output_options = info->current_option;
        /* update current option with next current option */
        info->current_option = tmp;
      }
    }
  }

  return ret;
} /* dhcpi_gather */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                           OPTION PARSE HANDLERS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
LOCAL FUNCTION DHCPI_OPT_PARSE_BOOTP_HANDLER()

DESCRIPTION
  This function extracts the bootp header information from the message
  and puts it into the core handle.

DEPENDENCIES
  None.

PARAMETERS
  dhcp_core_infor * - The handle associated with the current messaage.

RETURN VALUE
  dhcp_option_ret_enum - indicated success or failure.

SIDE EFFECTS
  core handle becomes populated with bootp infomation.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_opt_parse_bootp_handler
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: Parsing bootp 'option'" );

  /* Order and sizes here is important.  See RFC2131 */
  DHCP_COPY_INPUT_OPTION_1_BYTES(info, op);
  DHCP_COPY_INPUT_OPTION_1_BYTES(info, htype);
  DHCP_COPY_INPUT_OPTION_1_BYTES(info, hlen);
  DHCP_COPY_INPUT_OPTION_1_BYTES(info, hops);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, xid);
  DHCP_COPY_INPUT_OPTION_2_BYTES(info, secs);
  DHCP_COPY_INPUT_OPTION_2_BYTES(info, flags);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, ciaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, yiaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, siaddr);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, giaddr);
  DHCP_COPY_INPUT_OPTION_N_BYTES(info, chaddr, DHCP_CHADDR_LEN);
  DHCP_COPY_INPUT_OPTION_N_BYTES(info, sname, DHCP_SNAME_LEN);
  DHCP_COPY_INPUT_OPTION_N_BYTES(info, file, DHCP_FILE_LEN);
  DHCP_COPY_INPUT_OPTION_4_BYTES(info, magic);

  /* Validate what we can */
  if ( DHCP_BOOTP_OP_BOOTREQUEST != info->core.op )
  {
    LOG_MSG_INFO2_1( "FAIL: Invalid op field 0x%x", info->core.op );
    ret = DHCP_OPTION_RET_FAIL;
  }
  info->core.op = DHCP_BOOTP_OP_BOOTREPLY; /* If we reply, we reply! */
  if ( info->core.flags & DHCP_BOOTP_FLAG_MBZ_MASK )
  {
    LOG_MSG_INFO2_1( "FAIL: Non-zero MBZ in flags field 0x%x",
                     info->core.flags );
    ret = DHCP_OPTION_RET_FAIL;
  }
  if ( DHCP_MAGIC_NUM != info->core.magic )
  {
    LOG_MSG_INFO2_1( "FAIL: bad magic number 0x%lx", info->core.magic );
    ret = DHCP_OPTION_RET_FAIL;
  }

  return ret;
} /* dhcpi_opt_parse_bootp_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPT_PARSE_OVERLOAD_HANDLER()

DESCRIPTION
  This function handles the overload option.  From RFC-2132:

  9.3. Option Overload

     This option is used to indicate that the DHCP 'sname' or 'file'
     fields are being overloaded by using them to carry DHCP options. A
     DHCP server inserts this option if the returned parameters will
     exceed the usual space allotted for options.

     If this option is present, the client interprets the specified
     additional fields after it concludes interpretation of the standard
     option fields.

     The code for this option is 52, and its length is 1.  Legal values
     for this option are:

             Value   Meaning
             -----   --------
               1     the 'file' field is used to hold options
               2     the 'sname' field is used to hold options
               3     both fields are used to hold options

      Code   Len  Value
     +-----+-----+-----+
     |  52 |  1  |1/2/3|
     +-----+-----+-----+

DEPENDENCIES
  None.

PARAMETERS
  dhcp_core_info * info - Handle associated with current message.
  uint8 optlen - The reported length of the option.

RETURN VALUE
  dhcp_option_ret_enum - Indicates success/failure.

SIDE EFFECTS
  dhcp_core_infor * gets updated with overload info.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_opt_parse_overload_handler
(
  dhcp_core_info * info,
  uint8 optlen
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: OVERLOAD option" );
  if ( 1 != optlen )
  {
    LOG_MSG_INFO2_0( "FAIL: Corrupt option len" );
    ret = DHCP_OPTION_RET_FAIL;
  }
  if ( 0 != info->overload )
  {
    LOG_MSG_INFO2_0( "FAIL: multiple overload options?" );
    ret = DHCP_OPTION_RET_FAIL;
  }
  if( ! info->extract( &info->overload, info->msg, info->offset, 1 ) )
  {
    LOG_MSG_INFO2_0( "FAIL: sort read in overload option" );
    ret = DHCP_OPTION_RET_FAIL;
  }
  if ( DHCP_OPTION_OVERLOAD_FILE > info->overload ||
       DHCP_OPTION_OVERLOAD_FILE_SNAME < info->overload )
  {
    LOG_MSG_INFO2_1( "FAIL: overload value out of range %d", info->overload );
    ret = DHCP_OPTION_RET_FAIL;
  }

  return ret;
} /* dhcpi_opt_parse_overload_handler */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                           OPTION GATHER HANDLERS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_BOOTP_HANDLER()

DESCRIPTION
  This function gathers information needed to properly fill out the
  bootp header of the reply message.  This includes riffling through
  the gather options for information, and making calls to the address
  manager based on this information.

DEPENDENCIES
  Incoming message already parsed.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum to indicate success/fail.  A fail means that we
  will not be sending a reply back.

SIDE EFFECTS
  AM Lease may get allocated.  AM Lease may get released.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_bootp_handler
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;
  dhcp_meta_option * opt;
  boolean ret1 = FALSE;

  ASSERT(NULL != info);
  /* Figure out what type of message, and any other relevant info */
  opt = info->gather_options;
  while ( NULL != opt )
  {
    switch( opt->option )
    {
    case DHCP_OPTION_MSG_TYPE:
      info->core.dhcp_msg_request = (dhcp_msg_type)(*opt->option_txt);
      break;
    case DHCP_OPTION_HOSTNAME:
      info->core.hostname = opt->option_txt;
      info->core.hostnamelen = opt->len;
      break;
    case DHCP_OPTION_CLIENT_ID:
      info->core.client_id = (opt->option_txt + 1);
      info->core.client_id_len = opt->len - 1;
      break;
    case DHCP_OPTION_REQUESTED_IP:
      /* length per RFC 2132 */
      memscpy(&info->core.requested_ip, 
              sizeof(info->core.requested_ip),
              opt->option_txt, 4);
      break;
    case DHCP_OPTION_SERVER_ID:
      /* length per RFC 2132 */
      memscpy(&info->core.server_id,
              sizeof(info->core.server_id), 
              opt->option_txt, 4);
      break;
    case DHCP_OPTION_IP_LEASE:
      memscpy(&info->core.ip_lease,
              sizeof(info->core.ip_lease), 
              opt->option_txt, 4);
      break;
    default:
      /* Do nothing */
      break;
    }
    opt = opt->next;
  }
  /* Gather the relevant info */
  switch( info->core.dhcp_msg_request )
  {
  case DHCP_DHCPDISCOVER:
    LOG_MSG_INFO1_6("DHCP DISCOVER received from client %x:%x:%x:%x:%x:%x",
                    info->core.chaddr[0], info->core.chaddr[1],
                    info->core.chaddr[2], info->core.chaddr[3],
                    info->core.chaddr[4], info->core.chaddr[5]);
    if ( FALSE ==
         info->config.lease_get( info->config.lease_handle, &info->core ) )
    {
      ret = DHCP_OPTION_RET_FAIL;
    }
    else
    {
      LOG_MSG_INFO1_6("DHCP sending OFFER to client %x:%x:%x:%x:%x:%x",
                      info->core.chaddr[0], info->core.chaddr[1],
                      info->core.chaddr[2], info->core.chaddr[3],
                      info->core.chaddr[4], info->core.chaddr[5]);
      LOG_MSG_INFO1("Offered IP address 0x%x", info->core.yiaddr, 0, 0 );
      info->core.dhcp_msg_reply = DHCP_DHCPOFFER;
      info->lease_option_allow = TRUE;
      ret = dhcpi_option_alloc_link( info, DHCP_OPTION_IP_LEASE, 0 );
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SERVER_ID, 0 );
      }
      /* Force send SIP server option always. */
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SIP, 0 );
      }
    }
    break;
  case DHCP_DHCPREQUEST:
    LOG_MSG_INFO1_6("DHCP REQUEST received from client %x:%x:%x:%x:%x:%x",
                    info->core.chaddr[0],
                    info->core.chaddr[1],
                    info->core.chaddr[2],
                    info->core.chaddr[3],
                    info->core.chaddr[4],
                    info->core.chaddr[5]);
    LOG_MSG_INFO1_2("DHCP REQUEST received with ciaddr 0x%x and "
                    "requested ip addr 0x%x",
                    info->core.ciaddr, info->core.requested_ip );
    /* We really have the possibility of a tri-state in this case:
     * ACK, NAK, or ignore
     */
    if ( FALSE ==
         info->config.lease_get( info->config.lease_handle, &info->core ) )
    {
      if ( DHCP_DHCPNACK == info->core.dhcp_msg_reply )
      {
        LOG_MSG_INFO1_6("DHCP sending NACK to client %x:%x:%x:%x:%x:%x",
                        info->core.chaddr[0], info->core.chaddr[1],
                        info->core.chaddr[2], info->core.chaddr[3],
                        info->core.chaddr[4], info->core.chaddr[5]);

        /* A NAK requires that only certain options go out, so go
         * through and remove all options!
         */
        while ( NULL != info->gather_options )
        {
          opt = info->gather_options;
          info->gather_options = opt->next;
          info->init.d_free( opt );
        }
        /* Add back the options we need */
        info->lease_option_allow = FALSE;
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_MSG_TYPE, 1 );
        if ( DHCP_OPTION_RET_SUCCESS == ret )
        {
          ret = dhcpi_option_alloc_link(info, DHCP_OPTION_SERVER_ID, 0);
        }
      }
      else
      {
        LOG_MSG_INFO2_0( "FAIL: Lease alloc failed, ignoring request");
        ret = DHCP_OPTION_RET_FAIL;
      }
    }
    else
    {
      LOG_MSG_INFO1_6("DHCP sending ACK to client %x:%x:%x:%x:%x:%x",
                      info->core.chaddr[0],
                      info->core.chaddr[1],
                      info->core.chaddr[2],
                      info->core.chaddr[3],
                      info->core.chaddr[4],
                      info->core.chaddr[5]);
      LOG_MSG_INFO1_1("Allocated IP address 0x%x", info->core.yiaddr );

      info->core.dhcp_msg_reply = DHCP_DHCPACK;
      info->lease_option_allow = TRUE;
      ret = dhcpi_option_alloc_link( info, DHCP_OPTION_IP_LEASE, 0 );
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SERVER_ID, 0 );
      }
      /* Force send SIP server option always. */
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SIP, 0 );
      }
      if (info->config.msg_cback)
      {
        info->config.msg_cback( info->config.msg_cback_user_data,
                                info->core.dhcp_msg_reply,
                                info->core.yiaddr );
      }
    }
    break;
  case DHCP_DHCPINFORM:
    /* RFC 2131 has the following to say about DHCPINFORM:
       3.4 Obtaining parameters with externally configured network address

       If a client has obtained a network address through some other
       means (e.g., manual configuration), it may use a DHCPINFORM
       request message to obtain other local configuration parameters.
       Servers receiving a DHCPINFORM message construct a DHCPACK
       message with any local configuration parameters appropriate for
       the client without: allocating a new address, checking for an
       existing binding, filling in 'yiaddr' or including lease time
       parameters.  The servers SHOULD unicast the DHCPACK reply to
       the address given in the 'ciaddr' field of the DHCPINFORM
       message.

       The server SHOULD check the network address in a DHCPINFORM
       message for consistency, but MUST NOT check for an existing
       lease.  The server forms a DHCPACK message containing the
       configuration parameters for the requesting client and sends
       the DHCPACK message directly to the client.
   */
    ASSERT(NULL != info->config.addr_info_server_id );
    LOG_MSG_INFO1_6("DHCP INFORM received from client %x:%x:%x:%x:%x:%x",
                    info->core.chaddr[0],
                    info->core.chaddr[1],
                    info->core.chaddr[2],
                    info->core.chaddr[3],
                    info->core.chaddr[4],
                    info->core.chaddr[5]);
    if ( 0 == info->core.ciaddr )
    {
      LOG_MSG_INFO1_0("DHCP dropping INFORM with 0 ciaddr" );
      ret = DHCP_OPTION_RET_FAIL;
    }
    else
    {
      LOG_MSG_INFO1_1("DHCP INFORM received with ciaddr 0x%x",
                      info->core.ciaddr );
      info->core.siaddr = info->core.server_id =
      info->config.addr_info_server_id(info->config.addr_info_handle);
      LOG_MSG_INFO1_0("DHCP sending ACK");
      info->core.dhcp_msg_reply = DHCP_DHCPACK;
      info->lease_option_allow = FALSE;
      ret = dhcpi_option_alloc_link( info, DHCP_OPTION_IP_LEASE, 0 );
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SERVER_ID, 0 );
      }
      /* Force send SIP server option always. */
      if ( DHCP_OPTION_RET_SUCCESS == ret )
      {
        ret = dhcpi_option_alloc_link( info, DHCP_OPTION_SIP, 0 );
      }
      if (info->config.msg_cback)
      {
        info->config.msg_cback( info->config.msg_cback_user_data,
                                info->core.dhcp_msg_request,
                                info->core.yiaddr );
      }
    }
    break;
  case DHCP_DHCPRELEASE:
    LOG_MSG_INFO1_6("DHCP RELEASE received from client %x:%x:%x:%x:%x:%x",
                    info->core.chaddr[0],
                    info->core.chaddr[1],
                    info->core.chaddr[2],
                    info->core.chaddr[3],
                    info->core.chaddr[4],
                    info->core.chaddr[5]);

    ret1 = info->config.lease_free( info->config.lease_handle, &info->core );
    ret = DHCP_OPTION_RET_FAIL;
    if (TRUE == ret1 && info->config.msg_cback)
    {
      info->config.msg_cback( info->config.msg_cback_user_data,
                              info->core.dhcp_msg_request,
                              info->core.yiaddr );
    }
    break;
  case DHCP_DHCPDECLINE:
    LOG_MSG_INFO1_6("DHCP DECLINE received from client %x:%x:%x:%x:%x:%x",
                    info->core.chaddr[0],
                    info->core.chaddr[1],
                    info->core.chaddr[2],
                    info->core.chaddr[3],
                    info->core.chaddr[4],
                    info->core.chaddr[5]);

    ret1 = info->config.lease_err( info->config.lease_handle, &info->core );
    ret = DHCP_OPTION_RET_FAIL;
    if (TRUE == ret1 && info->config.msg_cback)
    {
      info->config.msg_cback( info->config.msg_cback_user_data,
                              info->core.dhcp_msg_request,
                              info->core.yiaddr );
    }
    break;
  case DHCP_DHCPOFFER:
  case DHCP_DHCPACK:
  case DHCP_DHCPNACK:
    LOG_MSG_INFO2_1( "FAIL: Invalid DHCP server MSG type from client 0x%x",
                     info->core.dhcp_msg_request );
    ret = DHCP_OPTION_RET_FAIL;
    break;
  default:
    LOG_MSG_INFO2_1( "FAIL: Invalid DHCP MSG type 0x%x",
                     info->core.dhcp_msg_request );
    ret = DHCP_OPTION_RET_FAIL;
  }
  return ret;
} /* dhcpi_option_bootp_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_SUBNET_MASK_ROUTER_HANDLER()

DESCRIPTION
  This function called for both subnet mask option and for router
  option.  We always provide both.  If we are asked to provide both, we
  output multiple copies at this stage, but we will cowardly refuse to
  issue both in the output stage

  Relevant portions of RFC2132:
  3.3. Subnet Mask

   The subnet mask option specifies the client's subnet mask as per RFC
   950 [5].

   If both the subnet mask and the router option are specified in a DHCP
   reply, the subnet mask option MUST be first.

   The code for the subnet mask option is 1, and its length is 4 octets.

    Code   Len        Subnet Mask
   +-----+-----+-----+-----+-----+-----+
   |  1  |  4  |  m1 |  m2 |  m3 |  m4 |
   +-----+-----+-----+-----+-----+-----+

  3.5. Router Option

   The router option specifies a list of IP addresses for routers on the
   client's subnet.  Routers SHOULD be listed in order of preference.

   The code for the router option is 3.  The minimum length for the
   router option is 4 octets, and the length MUST always be a multiple
   of 4.

    Code   Len         Address 1               Address 2
   +-----+-----+-----+-----+-----+-----+-----+-----+--
   |  3  |  n  |  a1 |  a2 |  a3 |  a4 |  a1 |  a2 |  ...
   +-----+-----+-----+-----+-----+-----+-----+-----+--


DEPENDENCIES
  The current option must be either a router option or subnetmask
  option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  May overwrite the current option.
  Inserts additional option to ensure ordering of netmask/router
  options.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_subnet_mask_router_handler
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_FAIL;
  uint32 netmask;

  ASSERT(NULL != info);
  ASSERT(NULL != info->config.addr_info_netmask );
  /* The order of the options will be reversed pulling them off the list */
  LOG_MSG_INFO3_0( "INFO: ROUTER option" );
  info->current_option->option = DHCP_OPTION_ROUTER;
  info->current_option->option_txt = (uint8*)&info->core.server_id;
  info->current_option->len = 4; /* Per RFC2132 */
  LOG_MSG_INFO3_0( "INFO: SUBNET MASK option" );
  info->current_option->next =
    info->init.d_malloc(sizeof(dhcp_meta_option) + 4);
  if ( NULL != info->current_option->next )
  {
    netmask = info->config.addr_info_netmask(info->config.addr_info_handle);
    info->current_option->next->next = NULL;
    info->current_option->next->option = DHCP_OPTION_SUBNET_MASK;
    info->current_option->next->option_txt =
      (uint8*)&info->current_option->next[1];
    memscpy( info->current_option->next->option_txt,4, &netmask, 4);
    info->current_option->next->len = 4; /* Per RFC2132 */
    ret = DHCP_OPTION_RET_SUCCESS;
  }
  else
  {
    LOG_MSG_ERROR_0("Failed to alloc memory for subnetmask option");
  }
  return ret;
} /* dhcpi_option_subnet_mask_router_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_DNS_HANDLER()

DESCRIPTION
  This function will call the AM to get the DNS addresses that need to
  be returned to the client.

  Relevant portions of RFC2132:
  3.8. Domain Name Server Option

   The domain name server option specifies a list of Domain Name System
   (STD 13, RFC 1035 [8]) name servers available to the client.  Servers
   SHOULD be listed in order of preference.

   The code for the domain name server option is 6.  The minimum length
   for this option is 4 octets, and the length MUST always be a multiple
   of 4.

    Code   Len         Address 1               Address 2
   +-----+-----+-----+-----+-----+-----+-----+-----+--
   |  6  |  n  |  a1 |  a2 |  a3 |  a4 |  a1 |  a2 |  ...
   +-----+-----+-----+-----+-----+-----+-----+-----+--

DEPENDENCIES
  Current option DNS.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Formulates DNS option for reply.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_dns_handler
(
  dhcp_core_info * info
)
{
  uint32    size = 0;
  uint32    ip_addr;
  void    * handle;
  boolean   more;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: DNS option" );
  if ( NULL != info->config.dns_first && NULL != info->config.dns_next )
  {
    more = info->config.dns_first(info->config.dns_handle, &handle,
                                  &ip_addr);
    while ( TRUE == more )
    {
      /* Yeah, ok, this is sub-optimial... */
      if ( DHCP_OPTION_RET_SUCCESS ==
           dhcpi_option_realloc_current_option(info, size + 4))
      {
        /* size of 4 per RFC 2132 */
        memscpy( &info->current_option->option_txt[size],4, &ip_addr, 4 );
        size += 4;
      }
      more = info->config.dns_next(info->config.dns_handle, &handle,
                                   &ip_addr);
    }
  }
  if ( 0 == size )
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_dns_handler */


/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_NBNS_HANDLER()

DESCRIPTION
  This function will call the AM to get the NBNS addresses that need to
  be returned to the client.

  Relevant portions of RFC2132:
  8.5. NetBIOS over TCP/IP Name Server Option

   The NetBIOS name server (NBNS) option specifies a list of RFC
   1001/1002 [19] [20] NBNS name servers listed in order of preference.

   The code for this option is 44.  The minimum length of the option is
   4 octets, and the length must always be a multiple of 4.

    Code   Len         Address 1               Address 2
   +-----+-----+-----+-----+-----+-----+-----+-----+--
   |  44 |  n  |  a1 |  a2 |  a3 |  a4 |  a1 |  a2 |  ...
   +-----+-----+-----+-----+-----+-----+-----+-----+--

DEPENDENCIES
  Current option NBNS.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Formulates NBNS option for reply.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_nbns_handler
(
  dhcp_core_info * info
)
{
  uint32    size = 0;
  uint32    ip_addr;
  void    * handle;
  boolean   more;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: NBNS option" );
  if ( NULL != info->config.nbns_first && NULL != info->config.nbns_next )
  {
    more = info->config.nbns_first(info->config.nbns_handle, &handle,
                                  &ip_addr);
    while ( TRUE == more )
    {
      /* Yeah, ok, this is sub-optimial... */
      if ( DHCP_OPTION_RET_SUCCESS ==
           dhcpi_option_realloc_current_option(info, size + 4))
      {
        /* size of 4 per RFC 2132 */
        memscpy( &info->current_option->option_txt[size],4, &ip_addr, 4 );
        size += 4;
      }
      more = info->config.nbns_next(info->config.nbns_handle, &handle,
                                   &ip_addr);
    }
  }
  if ( 0 == size )
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_nbns_handler */


/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_HOSTNAME_HANDLER()

DESCRIPTION
  This function processes the hostname option.

  Relevent portions of RFC2132:
  3.14. Host Name Option

   This option specifies the name of the client.  The name may or may
   not be qualified with the local domain name (see section 3.17 for the
   preferred way to retrieve the domain name).  See RFC 1035 for
   character set restrictions.

   The code for this option is 12, and its minimum length is 1.

    Code   Len                 Host Name
   +-----+-----+-----+-----+-----+-----+-----+-----+--
   |  12 |  n  |  h1 |  h2 |  h3 |  h4 |  h5 |  h6 |  ...
   +-----+-----+-----+-----+-----+-----+-----+-----+--

DEPENDENCIES
  The current option must be the hostname option from the request.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Frees the current option iff the length is zero.  This catches the
  case that the remote requests a hostname, which we can't provide.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_hostname_handler
(
  dhcp_core_info * info
)
{
  LOG_MSG_INFO3_0( "INFO: HOSTNAME option" );
  ASSERT(NULL != info);
  /* Nothing to do here, just pass back the option we got from the
   * client.  If the length is zero, then we should send nothing.
   */
  if ( 0 == info->current_option->len )
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_hostname_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_CLIENT_ID_HANDLER()

DESCRIPTION
  This function processes the client identifier option.

  Relevent portions of RFC2132:
  9.14. Client Identifier Option

   This option is used by DHCP clients to specify their unique
   identifier.  DHCP servers use this value to index their database of
   address bindings.  This value is expected to be unique for all
   clients in an administrative domain.

   Identifiers SHOULD be treated as opaque objects by DHCP servers.

   The client identifier MAY consist of type-value pairs similar to the
   'htype'/'chaddr' fields defined in [3]. For instance, it MAY consist
   of a hardware type and hardware address. In this case the type field
   SHOULD be one of the ARP hardware types defined in STD2 [22].  A
   hardware type of 0 (zero) should be used when the value field
   contains an identifier other than a hardware address (e.g. a fully
   qualified domain name).

   For correct identification of clients, each client's client-
   identifier MUST be unique among the client-identifiers used on the
   subnet to which the client is attached.  Vendors and system
   administrators are responsible for choosing client-identifiers that
   meet this requirement for uniqueness.

   The code for this option is 61, and its minimum length is 2.

 
   Code   Len   Type  Client-Identifier
   +-----+-----+-----+-----+-----+---
   |  61 |  n  |  t1 |  i1 |  i2 | ...
   +-----+-----+-----+-----+-----+---


DEPENDENCIES
  The current option must be the client identifier option from the request.

PARAMETERS
  dhcp_core_info * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Frees the current option.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_client_id_handler
(
  dhcp_core_info * info
)
{
  LOG_MSG_INFO3_0( "INFO: Client Identifier option" );
  ASSERT(NULL != info);
  /* Remove this option from the response.
   * This option should not be sent back to the client.
   */
  info->init.d_free(info->current_option);
  info->current_option = NULL;

  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_client_id_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_DOMAIN_NAME_HANDLER()

DESCRIPTION
  This function provides the domain name to the remote side iff we
  know it.  This may or may not be provided by the address manager. If
  the domain name is not known, then the current option is freed.

  Relevent portion of RFC2132:
  3.17. Domain Name

   This option specifies the domain name that client should use when
   resolving hostnames via the Domain Name System.

   The code for this option is 15.  Its minimum length is 1.

    Code   Len        Domain Name
   +-----+-----+-----+-----+-----+-----+--
   |  15 |  n  |  d1 |  d2 |  d3 |  d4 |  ...
   +-----+-----+-----+-----+-----+-----+--

DEPENDENCIES
  The current option must be the domain name option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  The current option will be freed if there is not a domain name.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_domain_name_handler
(
  dhcp_core_info * info
)
{
  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: DOMAIN NAME option" );
  if ( NULL != info->core.domainname )
  {
    info->current_option->option_txt = info->core.domainname;
    info->current_option->len = info->core.domainnamelen;
  }
  else
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_domain_name_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_IP_LEASE_HANDLER()

DESCRIPTION
  This function handles the ip lease option.  This option is allowed
  for some replies, but not others, per RFC2131.  If allowed, then it
  will be provided as specified by the core handle.

  Relevent portions of RFC2132:
  9.2. IP Address Lease Time

   This option is used in a client request (DHCPDISCOVER or DHCPREQUEST)
   to allow the client to request a lease time for the IP address.  In a
   server reply (DHCPOFFER), a DHCP server uses this option to specify
   the lease time it is willing to offer.

   The time is in units of seconds, and is specified as a 32-bit
   unsigned integer.

   The code for this option is 51, and its length is 4.

    Code   Len         Lease Time
   +-----+-----+-----+-----+-----+-----+
   |  51 |  4  |  t1 |  t2 |  t3 |  t4 |
   +-----+-----+-----+-----+-----+-----+

DEPENDENCIES
  The current option must be the ip lease option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Frees the current option if option not allowed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_ip_lease_handler
(
  dhcp_core_info * info
)
{
  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: IP LEASE option" );
  if ( TRUE == info->lease_option_allow )
  {
    info->current_option->option_txt = (uint8 *)&info->core.ip_lease;
    info->current_option->len = 4; /* per RFC 2132 */
  }
  else
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_ip_lease_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_MSG_TYPE_HANDLER()

DESCRIPTION
  This function handles the DHCP msg type option.  If the current
  option is the option recieved from the remote side, then replace the
  request value with the reply value, otherwise free the current
  option.  The latter can happen if the remote side erronously
  requests the msg type option.

  Releavent portions of RFC2132:
  9.6. DHCP Message Type

   This option is used to convey the type of the DHCP message.  The code
   for this option is 53, and its length is 1.  Legal values for this
   option are:

           Value   Message Type
           -----   ------------
             1     DHCPDISCOVER
             2     DHCPOFFER
             3     DHCPREQUEST
             4     DHCPDECLINE
             5     DHCPACK
             6     DHCPNAK
             7     DHCPRELEASE
             8     DHCPINFORM

    Code   Len  Type
   +-----+-----+-----+
   |  53 |  1  | 1-9 |
   +-----+-----+-----+

DEPENDENCIES
  The current option must the dhcp msg type option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  May scrible on option txt of current option.
  May free current option.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_msg_type_handler
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_1( "INFO: DHCP MSG TYPE option %d",
                   info->core.dhcp_msg_reply );
  if ( 1 == info->current_option->len )
  {
    *info->current_option->option_txt = (uint8)info->core.dhcp_msg_reply;
  }
  else
  {
    /* This case is harmless, we ignore this */
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return ret;
} /* dhcpi_option_msg_type_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_SERVER_ID_HANDLER()

DESCRIPTION
  This function handles the server ID option.  The server ID is the
  servers IPV4 address.

  Releavent portions of RFC2132:
  9.7. Server Identifier

   This option is used in DHCPOFFER and DHCPREQUEST messages, and may
   optionally be included in the DHCPACK and DHCPNAK messages.  DHCP
   servers include this option in the DHCPOFFER in order to allow the
   client to distinguish between lease offers.  DHCP clients use the
   contents of the 'server identifier' field as the destination address
   for any DHCP messages unicast to the DHCP server.  DHCP clients also
   indicate which of several lease offers is being accepted by including
   this option in a DHCPREQUEST message.

   The identifier is the IP address of the selected server.

   The code for this option is 54, and its length is 4.

    Code   Len            Address
   +-----+-----+-----+-----+-----+-----+
   |  54 |  4  |  a1 |  a2 |  a3 |  a4 |
   +-----+-----+-----+-----+-----+-----+

DEPENDENCIES
  The current option must the server id.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Sets the option_txt of the current option to point at the siaddr.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_server_id_handler
(
  dhcp_core_info * info
)
{
  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: SERVER ID option" );
  info->current_option->len = 4; /* per RFC2132 */
  info->current_option->option_txt = (uint8*)&info->core.server_id;
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_server_id_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_PARAMETER_REQ_HANDLER()

DESCRIPTION
  This function handles the parameter request message.  This message
  consists of an array of option values for which the client is
  requesting information back from.  To handle this, we walk this
  array and for each option, a new meta_option will be added to the
  list.  This will cause these to be processed by there respective
  handlers.  If a requested option isn't handled, it will be freed
  later.

  Releavent portions of RFC2132:
  9.8. Parameter Request List

   This option is used by a DHCP client to request values for specified
   configuration parameters.  The list of requested parameters is
   specified as n octets, where each octet is a valid DHCP option code
   as defined in this document.

   The client MAY list the options in order of preference.  The DHCP
   server is not required to return the options in the requested order,
   but MUST try to insert the requested options in the order requested
   by the client.

   The code for this option is 55.  Its minimum length is 1.

    Code   Len   Option Codes
   +-----+-----+-----+-----+---
   |  55 |  n  |  c1 |  c2 | ...
   +-----+-----+-----+-----+---

DEPENDENCIES
  The current option must the parameter request message.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  May allocate new options.
  The parameter request message itself will be freed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_parameter_req_handler
(
  dhcp_core_info * info
)
{
  /* The option is a 8 bit quantity over the wire.  We use 8 bits here
   * to enusre that we only pull 8 bits from the wire and cast the
   * result below to expand to whatever size the enum actually is.
   */
  uint8 opt;
  /* How long */
  uint32 optsize = 0;
  /* pointer to option txt */
  uint8 * optptr = NULL;
  dhcp_option_ret_enum ret = DHCP_OPTION_RET_SUCCESS;

  if(NULL == info)
  {
    DATA_ERR_FATAL("dhcp_option_parameter_req_handler(): Info pointer is NULL");
    return DHCP_OPTION_RET_FAIL;
  }



  if(NULL == info->current_option)

  {
    DATA_ERR_FATAL("dhcp_option_parameter_req_handler(): current_option pointer is NULL");
    return DHCP_OPTION_RET_FAIL;
  }

  optsize = info->current_option->len;

  if(NULL == (optptr = info->current_option->option_txt))
  {
    DATA_ERR_FATAL("dhcp_option_parameter_req_handler(): current_option text is NULL");
    info->init.d_free(info->current_option);
    info->current_option = NULL;
    return DHCP_OPTION_RET_FAIL;
  }

  LOG_MSG_INFO3_0( "INFO: PARAMETER REQ option" );

  /* Since we need to gather info for each of the options that are
   * present in the parameter request, we put a link for each option.
   * Since we want to have the options in the order requested, put
   * them in the correct order here.  They will reversed as they are
   * added to the gather linked list, and again when added to the
   * output_options list.
   */

  while ( optsize-- )
  {
    opt = *optptr++;
    /* Strip out requests for end's and pads. */
    if ( (uint8)DHCP_OPTION_PAD != opt && (uint8)DHCP_OPTION_END != opt &&
         (uint8)DHCP_OPTION_MESSAGE != opt )
    {
      LOG_MSG_INFO3_1( "INFO: Adding parameter req for %d", opt );
      ret = dhcpi_option_alloc_link( info, (dhcp_option_enum)opt, 0 );
      if ( DHCP_OPTION_RET_SUCCESS != ret )
      {
        break;
      }
    }
  }
  /* We don't want to send one of these back.. */
  info->init.d_free(info->current_option);
  info->current_option = NULL;
  return ret;
} /* dhcpi_option_parameter_req_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_MAX_SIZE_HANDLER()

DESCRIPTION
  This function handles the max size option.  This work to be done
  here is to pull the requested size out of the option, verify that it
  is within our (and the RFC's) bounds and the free the message.

  Releavent portions of RFC2132:
  9.10. Maximum DHCP Message Size

   This option specifies the maximum length DHCP message that it is
   willing to accept.  The length is specified as an unsigned 16-bit
   integer.  A client may use the maximum DHCP message size option in
   DHCPDISCOVER or DHCPREQUEST messages, but should not use the option
   in DHCPDECLINE messages.

   The code for this option is 57, and its length is 2.  The minimum
   legal value is 576 octets.

    Code   Len     Length
   +-----+-----+-----+-----+
   |  57 |  2  |  l1 |  l2 |
   +-----+-----+-----+-----+

DEPENDENCIES
  The current option must the max size handler option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  The core_handle will be updated.
  The current message will be freed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_max_size_handler
(
  dhcp_core_info * info
)
{
  uint16 size;

  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: MAX SIZE option" );
  /* Size per RFC 2132 */
  /* Memcpy for alignment */
  memscpy( &size,sizeof(size), info->current_option->option_txt, 2);
  info->output_option_max_size = dss_ntohs(size);
  if ( DHCP_MIN_MAX_REPLY_LEN > info->output_option_max_size )
  {
    info->output_option_max_size = DHCP_MIN_MAX_REPLY_LEN;
  }
  if ( DHCP_MAX_MAX_REPLY_LEN < info->output_option_max_size )
  {
    info->output_option_max_size = DHCP_MAX_MAX_REPLY_LEN;
  }
  info->init.d_free(info->current_option);
  info->current_option = NULL;
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_max_size_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_RENEW_TIME_HANDLER()

DESCRIPTION
  This function handles the renew time option.  This option is sent
  from the server to the client with the renew time, if the client
  requests it or if the server cares to change it away from the
  default.  This option is not allowed for some replies.

  Releavent portions of RFC2132:
  9.11. Renewal (T1) Time Value

   This option specifies the time interval from address assignment until
   the client transitions to the RENEWING state.

   The value is in units of seconds, and is specified as a 32-bit
   unsigned integer.

   The code for this option is 58, and its length is 4.

    Code   Len         T1 Interval
   +-----+-----+-----+-----+-----+-----+
   |  58 |  4  |  t1 |  t2 |  t3 |  t4 |
   +-----+-----+-----+-----+-----+-----+

DEPENDENCIES
  The current option must the renew time option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  The option_txt will be changed to point to the renew_time if this
  option is allowed.
  The current option will be freed if this option is not allowed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_renew_time_handler
(
  dhcp_core_info * info
)
{
  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: RENEW TIME option" );
  if ( TRUE == info->lease_option_allow )
  {
    info->current_option->option_txt = (uint8*)&info->core.renew_time;
    info->current_option->len = 4; /* per RFC 2132 */
  }
  else
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_renew_time_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_REBIND_TIME_HANDLER()

DESCRIPTION
  This function handles the rebind option.  This option is sent from
  the server to the client with the rebind time, if the client
  requests it or if the server cares to change it away from the
  default.  This option is not allowed for some replies.

  Releavent portions of RFC2132:
  9.12. Rebinding (T2) Time Value

   This option specifies the time interval from address assignment until
   the client transitions to the REBINDING state.

   The value is in units of seconds, and is specified as a 32-bit
   unsigned integer.

   The code for this option is 59, and its length is 4.

    Code   Len         T2 Interval
   +-----+-----+-----+-----+-----+-----+
   |  59 |  4  |  t1 |  t2 |  t3 |  t4 |
   +-----+-----+-----+-----+-----+-----+

DEPENDENCIES
  The current option must the rebind time option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  The option_txt will be changed to point to the rebind_time if this
  option is allowed.
  The current option will be freed if this option is not allowed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_rebind_time_handler
(
  dhcp_core_info * info
)
{
  ASSERT(NULL != info);
  LOG_MSG_INFO3_0( "INFO: REBIND TIME option" );
  if ( TRUE == info->lease_option_allow )
  {
    info->current_option->option_txt = (uint8*)&info->core.rebind_time;
    info->current_option->len = 4; /* per RFC 2132 */
  }
  else
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_rebind_time_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_MESSAGE_HANDLER()

DESCRIPTION
  This option handles the message option.  This option is used to
  display problems the other side sees.  We put this in a diagnostic
  message and free the option.

  Releavent portions of RFC2132:
  9.9. Message

   This option is used by a DHCP server to provide an error message to a
   DHCP client in a DHCPNAK message in the event of a failure. A client
   may use this option in a DHCPDECLINE message to indicate the why the
   client declined the offered parameters.  The message consists of n
   octets of NVT ASCII text, which the client may display on an
   available output device.

   The code for this option is 56 and its minimum length is 1.

    Code   Len     Text
   +-----+-----+-----+-----+---
   |  56 |  n  |  c1 |  c2 | ...
   +-----+-----+-----+-----+---

DEPENDENCIES
  The current option must the message option.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  The incoming message will be output.
  The current message will be freed.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_message_handler
(
  dhcp_core_info * info
)
{
  dhcp_option_ret_enum   ret = DHCP_OPTION_RET_SUCCESS;
  /*-------------------------------------------------------------------------*/
  
  ASSERT(NULL != info);
  if ( info->current_option->len < 1 )
  {
    LOG_MSG_ERROR_0("INFO: MESSAGE Option incorrect length");
  }
  else
  {
    info->current_option->option_txt[info->current_option->len-1] = '\0';
    LOG_MSG_INFO1_1( "INFO: MESSAGE option %s",
                     info->current_option->option_txt);
  }
  info->init.d_free(info->current_option);
  info->current_option = NULL;
  
  return ret;
} /* dhcpi_option_message_handler */

/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_MTU_HANDLER()
    
DESCRIPTION
  This function processes the MTU option.
    
  Relevent portions of RFC2132:
  5.1. Interface MTU Option
      
   This option specifies the MTU to use on this interface.  The MTU is
   specified as a 16-bit unsigned integer.  The minimum legal value for
   the MTU is 68.
      
   The code for this option is 26, and its length is 2.
      
   Code   Len      MTU
   +-----+-----+-----+-----+
   |  26 |  2  |  m1 |  m2 |
   +-----+-----+-----+-----+

DEPENDENCIES
  The current option must be the interface mtu option.

PARAMETERS
  dhcp_core_info * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - DHCP_OPTION_RET_SUCCESS indicates success.

SIDE EFFECTS
  No Side effects
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_mtu_handler
(
  dhcp_core_info * info
)
{
  uint16 mtu = 0;

  ASSERT(NULL != info);
  LOG_MSG_INFO1_0( "INFO: MTU option" );
  /* Pass back the option we got from the client with the mtu value filled. 
   * if we dont support mtu option, then we should send nothing.
   */
  if ( ( NULL != info->config.interface_mtu ) && ( DHCP_OPTION_RET_SUCCESS
       == dhcpi_option_realloc_current_option(info, 2)))
  {
        mtu = (uint16)
            info->config.interface_mtu(info->config.interface_mtu_handle);
        #define DHCP_MIN_MTU_VALUE 68
        if(mtu < DHCP_MIN_MTU_VALUE ) {
           info->init.d_free(info->current_option);
           info->current_option = NULL;
        }
        else {
           mtu = ps_htons(mtu);
           /* size of 2 per RFC 2132 */
           memscpy( info->current_option->option_txt,2, (uint8 *)&mtu, 2 );
        }
  }
  else {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_mtu_handler */
/*===========================================================================
LOCAL FUNCTION DHCPI_OPTION_SIP_HANDLER()

DESCRIPTION
  This function will call the AM to get the SIP addresses that need to
  be returned to the client.

  Relevant portions of RFC3361:
  3. SIP Server DHCP Option

   The SIP server DHCP option carries either a 32-bit (binary) IPv4
   address or, preferably, a DNS (RFC 1035 [6]) fully-qualified domain
   name to be used by the SIP client to locate a SIP server.

   The option has two encodings, specified by the encoding byte ('enc')
   that follows the code byte.  If the encoding byte has the value 0, it
   is followed by a list of domain names, as described below (Section
   3.1).  If the encoding byte has the value 1, it is followed by one or
   more IPv4 addresses (Section 3.2).  All implementations MUST support
   both encodings.  The 'Len' field indicates the total number of octets
   in the option following the 'Len' field, including the encoding byte.

   A DHCP server MUST NOT mix the two encodings in the same DHCP
   message, even if it sends two different instances of the same option.
   Attempts to do so would result in incorrect client behavior as DHCP
   processing rules call for the concatenation of multiple instances of
   an option into a single option prior to processing the option.

   The code for this option is 120.

    Code  Len   enc   Address 1                  Address 2
    +-----+-----+-----+-----+-----+-----+-----+-----+--
    | 120 |  n  |  1  | a1  | a2  | a3  | a4  | a1  |  ...
    +-----+-----+-----+-----+-----+-----+-----+-----+--

    Code  Len   enc   DNS name of SIP server
    +-----+-----+-----+-----+-----+-----+-----+-----+--
    | 120 |  n  |  0  |  s1 |  s2 |  s3 |  s4 | s5  |  ...
    +-----+-----+-----+-----+-----+-----+-----+-----+--

DEPENDENCIES
  Current option SIP.

PARAMETERS
  dhcp_core_infor * - The handle associated with the incoming message.

RETURN VALUE
  dhcp_option_ret_enum - indicates Success/fail.

SIDE EFFECTS
  Formulates DNS option for reply.
===========================================================================*/
static dhcp_option_ret_enum
dhcpi_option_sip_handler
(
  dhcp_core_info * info
)
{
  uint32    size = 0, sip_dname_len = 0;
  uint32 sip_addr_count = DHCP_MAX_SIP_SERVERS;
  dhcp_core_v4_temp_info_type *dhcp_core_v4_temp = NULL;
  uint8 i = 0;


  ASSERT(NULL != info);
  LOG_MSG_INFO3( "INFO: SIP option", 0, 0, 0 );

  dhcp_core_v4_temp = info->init.d_malloc( sizeof(dhcp_core_v4_temp_info_type) );

  if(NULL == dhcp_core_v4_temp)
  {
    LOG_MSG_ERROR_0( "Allocation of dhcp_core_v4_temp failed" );
    return DHCP_OPTION_RET_FAIL;
  }
  /* By default we try to send addresses. */
  memset(dhcp_core_v4_temp,0,sizeof(dhcp_core_v4_temp_info_type));
  if ( info->config.sip_addr_list != NULL )
  {
    info->config.sip_addr_list(info->config.sip_addr_handle,
                               dhcp_core_v4_temp->sip_addr, &sip_addr_count);

    if ( sip_addr_count != 0 && ( DHCP_OPTION_RET_SUCCESS == 
         dhcpi_option_realloc_current_option(info,
         (sip_addr_count * sizeof(struct ps_in_addr)) + 1)))
   {
     /* Update the Encoding bit. */
     info->current_option->option_txt[size] = 1;
     size = 1;
     for ( i = 0; i < sip_addr_count; i++ )
     {
        /* size of 4 per RFC 2132 */
        memscpy( &info->current_option->option_txt[size],
                 sizeof(struct ps_in_addr),
                 &dhcp_core_v4_temp->sip_addr[i].addr.v4,
                 sizeof(dhcp_core_v4_temp->sip_addr[i].addr.v4) );
        size += sizeof(dhcp_core_v4_temp->sip_addr[i].addr.v4);
     }
   }
  }
  /* Check domain names if we don't have addresses. */
  if ( sip_addr_count == 0 )
  {
    sip_addr_count = DHCP_MAX_SIP_SERVERS;

    if ( info->config.sip_domain_name_list != NULL )
    {
      info->config.sip_domain_name_list(info->config.sip_domain_name_handle,
                            dhcp_core_v4_temp->sip_domain_name,&sip_addr_count);
      if ( sip_addr_count != 0 && ( DHCP_OPTION_RET_SUCCESS == 
           dhcpi_option_realloc_current_option(info,1)))
      {
        /* Update the Encoding bit. */
        info->current_option->option_txt[size] = 0;
        size = 1;
        for ( i = 0; i < sip_addr_count ; i++ )
        {
           sip_dname_len = strlen(dhcp_core_v4_temp->sip_domain_name[i].domain_name);
           LOG_MSG_INFO1("Sip Domain Name length %d", sip_dname_len, 0, 0);
           /* Encode domain name */
           if( ps_proto_utils_encode_domain_name(
                 (uint8 *)dhcp_core_v4_temp->sip_domain_name[i].domain_name,
                 &sip_dname_len ) < 0 )
           {
             LOG_MSG_ERROR("Domain name encoding failed.", 0, 0, 0);
             info->init.d_free(dhcp_core_v4_temp);
             return DHCP_OPTION_RET_FAIL;
           }
           
           if ( ( sip_dname_len <= PS_IFACE_MAX_DOMAIN_NAME_SIZE ) && 
                ( DHCP_OPTION_RET_SUCCESS == 
                dhcpi_option_realloc_current_option(info, size+sip_dname_len)))
           {
             memscpy( &info->current_option->option_txt[size],
                      sip_dname_len,
                      dhcp_core_v4_temp->sip_domain_name[i].domain_name,
                      sip_dname_len);
             size += sip_dname_len;
           }
        }
      }
    }
  }
  if ( 0 == size )
  {
    info->init.d_free(info->current_option);
    info->current_option = NULL;
  }
  info->init.d_free(dhcp_core_v4_temp);
  return DHCP_OPTION_RET_SUCCESS;
} /* dhcpi_option_sip_handler */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CORE_NEW()

DESCRIPTION
  This function initializes a handle and returns it.

DEPENDENCIES
  Good parameters.

PARAMETERS
  dhcp_core_init * : Pointer to a structure of functions used to connect to
                       other systems.

RETURN VALUE
  void * - handle for dhcp_core functions.

SIDE EFFECTS
  Handle will be allocated.
===========================================================================*/
void *
dhcp_core_new
(
  dhcp_core_init * init
)
{
  dhcp_core_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CORE INIT ---" );

  ASSERT(NULL != init);
  ASSERT(NULL != init->d_malloc);
  ASSERT(NULL != init->d_free);

  handle = init->d_malloc(sizeof(dhcp_core_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP core");
    return NULL;
  }
  memset( handle, 0, sizeof(dhcp_core_info) );
  handle->init = *init; /* struct copy */
  handle->cookie = DHCP_CORE_COOKIE;

  return handle;
} /* dhcp_core_new */

/*===========================================================================
FUNCTION DHCP_CORE_START()

DESCRIPTION
  This function registers the functions used for this interface handle and
  does any per connection initialization needed for the DHCP Core system.

DEPENDENCIES
  The handle must be a valid handle allocated by dhcp_core_new.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_config * - Pointer to a structure of functions used to connect to
                       other systems.

RETURN VALUE
  Boolean - TRUE if ok, FALSE if failure.

SIDE EFFECTS
  Handle is finised initializing.
===========================================================================*/
boolean
dhcp_core_start
(
  void * handle,
  dhcp_core_config * config
)
{
  dhcp_core_info * core = handle;

  LOG_MSG_INFO3_0( "INFO: --- DHCP CORE START ---" );
  /* sanity check info */
  ASSERT(NULL != core);
  ASSERT(DHCP_CORE_COOKIE == core->cookie);
  ASSERT(NULL != config);

  /* Initialize "config" info */
  core->config = *config; /* struct copy */
  core->cookie_ptr = core;

  return TRUE;
} /* dhcp_core_start */

/*===========================================================================
FUNCTION DHCP_CORE_STOP()

DESCRIPTION
  This function unregisters the functions used for this interface
  handle and does any per connection cleanup needed for the DHCP Core
  system.

DEPENDENCIES

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  None

SIDE EFFECTS
  The handle passed in is freed.
===========================================================================*/
void
dhcp_core_stop
(
  void ** handle
)
{
  dhcp_core_info *core = NULL;

  if(NULL == handle || NULL == (core = *handle))
  {
    DATA_ERR_FATAL("dhcp_core_stop(): handle or *handle ptr is NULL");
    return ;
  }

  ASSERT(DHCP_CORE_COOKIE == core->cookie);

  /* Dont ASSERT */
  if(core->cookie_ptr != core)
  {
    LOG_MSG_INFO1_0("core_stop() called before  core_start()");
  }

  core->init.d_free(core);
  LOG_MSG_INFO3_0( "INFO: --- DHCP CORE STOP ---" );
  *handle = NULL;
} /* dhcp_core_stop */

/*===========================================================================
FUNCTION DHCP_CORE_INPUT()

DESCRIPTION
  This function processes the message passed in, and sends a reply if
  needed.  This is the core of this system.

DEPENDENCIES
  Handle must be valid.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  char * msg - Handle to message to process.
  extract() - Function to call to extract message.

RETURN VALUE
  None.

SIDE EFFECTS
  Message is processed.
  Reply may (or may not) be sent.
  State of leases may change.
  State in handle may change.
===========================================================================*/
void
dhcp_core_input
(
  void * handle,
  void * msg,
  boolean (*extract)(void * dest, void * src, uint32 offset, uint32 len)
)
{
  dhcp_core_info * info = handle;
  dhcp_option_ret_enum ret;
  dhcp_meta_option * opt;

  ASSERT(NULL != info);
  ASSERT(info->cookie_ptr == info);
  ASSERT(DHCP_CORE_COOKIE == info->cookie);
  ASSERT(NULL != msg);
  ASSERT(NULL != extract);
  /* Zero the message info to clear any previous info */
  memset( &info->core, 0, sizeof( dhcp_core_msg_info ) );
  info->output_option_size = 0;
  info->output_option_max_size = DHCP_MAX_MAX_REPLY_LEN;
  ASSERT( NULL == info->gather_options );
  ASSERT( NULL == info->current_option );
  ASSERT( NULL == info->output_options );
  info->gather_options = NULL;
  info->current_option = NULL;
  info->output_options = NULL;
  info->msg = msg;
  info->extract = extract;

  /* Parse the incoming message */
  ret = dhcpi_parse_msg( info );

  /* Use that info to gather info for the reply */
  if( DHCP_OPTION_RET_DONE == ret )
  {
    ret = dhcpi_gather( info );
  }
  else
  {
    ret = DHCP_OPTION_RET_FAIL;
  }

  /* Actually generate the reply */
  if( DHCP_OPTION_RET_SUCCESS == ret )
  {
    dhcpi_output(info);
  }

  /* Cleanup */
  while( NULL != info->output_options )
  {
    opt = info->output_options->next;
    info->init.d_free(info->output_options);
    info->output_options = opt;
  }
  while( NULL != info->gather_options )
  {
    opt = info->gather_options->next;
    info->init.d_free(info->gather_options);
    info->gather_options = opt;
  }
  ASSERT(NULL == info->current_option);
} /* dhcp_core_input */
