/*===========================================================================
              Internet Group Management Protocol (IGMP)

            I N T E R N A L   A P I   S O U R C E   F I L E

DESCRIPTION
 The IGMP internal API file.


Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_igmp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
05/13/13    ss     Updates header parse API to input DSM double pointer
03/02/12    ss     Fixed response time calculation logic.
04/22/10    ss     Compiler warnings fixed.
05/22/09    kk     Q6 compiler warning fixes.
03/26/09    pp     CMI De-featurization.
26/06/07    rs     Created module.

===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "dsm.h"
#include "err.h"
#include "ps_mem.h"

#include "ps_in.h"
#include "ps_svc.h"
#include "ps_mcasti.h"
#include "ps_igmp.h"
#include "amssassert.h"
#include "ps_iface.h"
#include "ps_ip.h"
#include "ps_ip4.h"
#include "ps_iputil.h"
#include "ps_byte.h"
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_pkt_info.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_IGMP_HDR_PARSE

DESCRIPTION
  This function will update the appropriate fields in passed IGMP header
  after extracting information from dsm item.

PARAMETERS
  dsm_item_type **      - DSM item ptr containing the packet.
  uint16 *             - Offset where the IGMP header is in the DSM item.
  struct ps_igmp_hdr * - IGMP header to be filled.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success and FALSE on failure.

SIDE EFFECTS
  Sets the value of offset to after the IGMP header in dsm item.
===========================================================================*/
boolean ps_igmp_hdr_parse
(
  dsm_item_type     ** bpp,
  uint16             * hdrlen,
  struct ps_igmp_hdr * igmph,
  uint16               offset,
  ip_pkt_info_type    *ip_pkt_info_ptr
)
{
  (void)ip_pkt_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( NULL == bpp || NULL == *bpp || NULL == igmph || NULL == hdrlen )
  {
    LOG_MSG_ERROR_0("NULL DSM item or IGMP header or offset passed to ps_igmp_hdr_parse()");
    return FALSE;
  }

  if(dsm_pullup(bpp, NULL, offset) != offset )
  {
    LOG_MSG_ERROR_0("Error in dsm_pullup of IGMP header in ps_igmp_hdr_parse()");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
      Calculate IGMP checksum.
   -----------------------------------------------------------------------*/
  if(cksum(NULL, *bpp, 
        IGMP_LEN, 0) != 0)
  {
    /* Checksum failed, ignore packet completely */
    LOG_MSG_ERROR_0("IGMP Checksum Error");
    return FALSE;
  }

  if( IGMP_LEN != (dsm_extract(*bpp, 0, (uint8*)igmph, IGMP_LEN)) )
  {
    LOG_MSG_ERROR_0("Error in dsm_extract of IGMP header in ps_igmp_hdr_parse()");
    return FALSE;
  }

  *hdrlen += IGMP_LEN;

  return TRUE;
} /* ps_igmp_hdr_parse() */


/*===========================================================================

FUNCTION PS_IGMP_INPUT

DESCRIPTION
  This function is called by the function ip_receive() and is used to
  process an incoming IGMP packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_input
(
  ip_pkt_info_type    *ip_pkt_info_ptr, /* IP Packet Info            */
  struct dsm_item_s   *igmp_data_ptr,   /* data field                */
  uint16               offset           /* offset of the IGMP packet */
)
{
  struct ps_igmp_hdr    igmp_header;      /* IGMP header fields     */
  uint16                length;           /* length of data portion */
  uint16                hdrlen = 0;       /* length of IGMP header   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( igmp_data_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Parse the IGMP header.
  -------------------------------------------------------------------------*/
  if(ps_igmp_hdr_parse(&igmp_data_ptr, &hdrlen, &igmp_header, offset,
            ip_pkt_info_ptr) ==   FALSE)
  {
    /* IGMP header is too small */
    LOG_MSG_ERROR_0( "Bad IGMP Header, Ignoring packet");
    dsm_free_packet(&igmp_data_ptr);
    return;
  }

  length = ip_pkt_info_ptr->pseudo_hdr.length;

  if(length > dsm_length_packet( igmp_data_ptr))
  /* Sanity check to match IP header length to actual payload length */
  {
    LOG_MSG_ERROR_0("Actual length of segment doesnt match  header len");
    dsm_free_packet(&igmp_data_ptr);
    return;
  }
  else if ( length < dsm_length_packet(igmp_data_ptr))
  {
    dsm_trim_packet( &igmp_data_ptr, length);
  }

  /*-------------------------------------------------------------------------
    Discard the packet if message is not sent to a multicast group or if
    message is not addressed to the group identified within the message.
  -------------------------------------------------------------------------*/
  if (igmp_header.igmp_mcast_grp.ps_s_addr != 0) /* packet is group specific */
  {
    /* the following check is specific to IPv4 since igmp_mcast_grp in the
       IGMP header is IPv4 */
    if (!PS_IN_IS_ADDR_MULTICAST(igmp_header.igmp_mcast_grp.ps_s_addr) ||
        igmp_header.igmp_mcast_grp.ps_s_addr !=
                           ip_pkt_info_ptr->ip_hdr.v4.dest.ps_s_addr)
    {
      LOG_MSG_ERROR_0( "Bad IGMP Header, Ignoring packet");
      dsm_free_packet(&igmp_data_ptr);
      return;
    }
  }

  switch (igmp_header.igmp_type)
  {
    case IGMP_MEMBERSHIP_QUERY_MSG_TYPE:
      ps_igmp_start_timers((ps_iface_type *)ip_pkt_info_ptr->if_ptr,
                                  &igmp_header);
      break;
    case IGMP_MEMBERSHIP_REPORT_V1_MSG_TYPE:
    case IGMP_MEMBERSHIP_REPORT_V2_MSG_TYPE:
      ps_igmp_stop_timer((ps_iface_type *)ip_pkt_info_ptr->if_ptr,
                                igmp_header.igmp_mcast_grp);
      break;
  } /* switch (type) */

  dsm_free_packet(&igmp_data_ptr);
} /* ps_igmp_input() */


/*===========================================================================

FUNCTION PS_IGMP_HDR_CREATE

DESCRIPTION
  This function prepares an IGMP header that is later used by the function
  ps_igmp_ouput() to send the IGMP packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_hdr_create
(
  ps_igmp_msg_enum_type    igmp_msg_type,
  ps_iface_type          * iface_ptr,
  struct ps_in_addr      * igmp_grp_ptr,
  dsm_item_type         ** item_ptr
)
{
  struct ps_igmp_hdr  igmp_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------

                               IGMP header format

       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |  Type         |Max Resp Time  |          Checksumm            |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                       Group Address                           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  -------------------------------------------------------------------------*/
  *item_ptr = dsm_offset_new_buffer(DSM_DS_LARGE_ITEM_POOL, 0);
  if (*item_ptr == NULL)
  {
    LOG_MSG_ERROR_0("No memory to send the packet in ps_igmp_hdr_create()");
    return;
  }

  /*-----------------------------------------------------------------------
     Fill the IGMP header fields.
  -----------------------------------------------------------------------*/
  memset(&igmp_hdr, 0, sizeof(struct ps_igmp_hdr));
  switch(igmp_msg_type)
  {
    case PS_IGMP_JOIN:
      {
        if (ps_timer_is_running(
            ps_iface_mcast_get_querier_v1_timer(iface_ptr)))
        {
          igmp_hdr.igmp_type = (uint8)IGMP_MEMBERSHIP_REPORT_V1_MSG_TYPE;
        }
        else
        {
          igmp_hdr.igmp_type = (uint8)IGMP_MEMBERSHIP_REPORT_V2_MSG_TYPE;
        }
      }
      break;
    case PS_IGMP_LEAVE:
      {
        igmp_hdr.igmp_type = (uint8)IGMP_LEAVE_GROUP_MSG_TYPE;
      }
      break;
    default:
      LOG_MSG_ERROR_0("Invalid IGMP TYPE");
      return;
  }

  igmp_hdr.igmp_resp_time = 0;
  igmp_hdr.igmp_checksum  = 0;
  memscpy(&igmp_hdr.igmp_mcast_grp,
          sizeof(igmp_hdr.igmp_mcast_grp), 
          igmp_grp_ptr, 
          sizeof(struct ps_in_addr));

  /*-----------------------------------------------------------------------
     Pushdown the IGMP header (payload)
  -----------------------------------------------------------------------*/
  if (dsm_pushdown_packed( item_ptr,
                    (void *)&igmp_hdr,
                    sizeof(struct ps_igmp_hdr),
                    DSM_DS_SMALL_ITEM_POOL) < sizeof(struct ps_igmp_hdr))
  {
    LOG_MSG_ERROR_0("Running out of items");
    dsm_free_packet(item_ptr);
    return;
  }

  (void)put16((*item_ptr)->data_ptr + 2, cksum(NULL, *item_ptr, IGMP_LEN, 0));

  return;
} /*ps_igmp_hdr_create()*/


/*===========================================================================

FUNCTION PS_IGMP_OUTPUT

DESCRIPTION
  This function calls the function ip_send() to send an IGMP message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ps_igmp_output
(
  ps_igmp_msg_enum_type    igmp_msg_type,
  ps_iface_type          * iface_ptr,
  struct ps_in_addr      * igmp_grp_ptr
)
{
  ip_cb_type          temp_ipcb;
  struct ps_in_addr   dest_ip;
  int16               ps_errno;
  dsm_item_type       * item_ptr = NULL;
  int                 ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&temp_ipcb, 0, sizeof(ip_cb_type));

  switch(igmp_msg_type)
  {
    case PS_IGMP_JOIN:
      {
        dest_ip = *igmp_grp_ptr;
      }
      break;
    case PS_IGMP_LEAVE:
      {
        dest_ip.ps_s_addr = PS_MCAST_ALL_ROUTER_IPV4;
      }
      break;
    default:
      LOG_MSG_ERROR_0("Invalid IGMP TYPE");
      return;
  }

  /*-----------------------------------------------------------------------
    Create an IP control block using which IGMP message would be sent out.
  -----------------------------------------------------------------------*/
  temp_ipcb.routing_cache = iface_ptr;
  temp_ipcb.ttl = 1;
  temp_ipcb.ip_cb_router_alert = TRUE;

  ps_igmp_hdr_create(igmp_msg_type, iface_ptr, &dest_ip, &item_ptr);
  ret = ip4_send( &temp_ipcb,
                  dest_ip,
                  PS_IPPROTO_IGMP,
                  item_ptr,
                  IGMP_LEN,
                  NULL,
                  &ps_errno);
  if (ret != 0)
  {
    LOG_MSG_ERROR_0("Unable to send packet, ip4_send() failed");
  }
  return;  

} /*ps_igmp_output()*/


/*===========================================================================
FUNCTION PS_IGMP_STOP_TIMER

DESCRIPTION
  This function stops timer for the membership group for which a membership
  report is received if the interface is a member of that group. This
  function is specific to IGMP as it expects an IPV4 address as parameter.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  group_address: IPV4 group address.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_stop_timer
(
  ps_iface_type               *this_iface_ptr,
  struct ps_in_addr           group_address
)
{
  struct ps_mcast_cb  *if_mcast_grp;
  int timer_ret_val;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface 0x%p", this_iface_ptr);
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

 /*-------------------------------------------------------------------------
    Determine if this interface is a member of the group being reported.
    If so, stop timer for this group.
  -------------------------------------------------------------------------*/
  if_mcast_grp = ps_igmp_find_v4_group(this_iface_ptr,
                                              group_address);
  if (if_mcast_grp != NULL)
  {
    if (ps_timer_is_running(if_mcast_grp->mcast_report_timer))
    {
      timer_ret_val = ps_timer_cancel(if_mcast_grp->
                                      mcast_report_timer);
      if (timer_ret_val != PS_TIMER_SUCCESS)
      {
        LOG_MSG_ERROR_0("Error cancelling the timer");
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return;
      }
      if_mcast_grp->mcast_state = PS_MCAST_STATE_IDLE_MEMBER;
      if_mcast_grp->send_leave_msg = FALSE;
    } /* if (ps_timer_is_running() */
  } /* if (mcast_grp != NULL) */

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_igmp_stop_timer() */


/*===========================================================================
FUNCTION PS_IGMP_START_TIMERS

DESCRIPTION
  This function starts timers for the membership groups associated with the
  given interface. This function is specific to IGMP as it works on IGMP
  header.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  igmph: pointer to igmp header structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_start_timers
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface 0x%p", this_iface_ptr);
    return;
  }

  if (igmph == NULL)
  {
    LOG_MSG_ERROR_0("NULL parameter is passed");
    return;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

 /*-------------------------------------------------------------------------
    Determine if the membership query was general query i.e. has group 
    addresss field of zero. If so, start timer for every group. If query 
    was group specific, start timer only for this group.
  -------------------------------------------------------------------------*/
  if (igmph->igmp_mcast_grp.ps_s_addr == 0 
      || igmph->igmp_resp_time == 0) /* query is general or it is sent by
                                        IGMPv1 querier ; query can't be
                                        group specific and sent
                                        by IGMPv1 querier at the same time */
  {
    ps_igmp_process_gen_query(this_iface_ptr, igmph);
  }

  else /* query is group specific */
  {
    ps_igmp_process_grp_query(this_iface_ptr, igmph);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_igmp_start_timers() */


/*===========================================================================
FUNCTION PS_IGMP_PROCESS_GEN_QUERY

DESCRIPTION
  This function processes IGMP general queries.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  igmph: pointer to igmp header structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_process_gen_query
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
)
{
  int response_time, ret_value;
  struct ps_mcast_cb   * if_mcast_grp, ** if_mcast_grp_ptr;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

 /*-------------------------------------------------------------------------
    If the querier was IGMPv1, set the response time to 10 seconds and start
    Version 1 router present timer
  -------------------------------------------------------------------------*/
  if (igmph->igmp_resp_time == 0)
  {
    response_time = MCAST_V1_RESPONSE_INTERVAL;
    ret_value = ps_mcast_start_v1_timer(this_iface_ptr);
    if (ret_value == -1)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return ;
    }
  }
  else
  {
 /*-------------------------------------------------------------------------
    Max response time in the IGMP header is in units of 1/10 seconds.
    Calculating in milliseconds: 
    => [value_in_pkt] * 1/10 * 1000
    => [value_in_pkt] * 100
  -------------------------------------------------------------------------*/
    response_time = (igmph->igmp_resp_time) * 100;
  }

  if_mcast_grp_ptr = ps_iface_mcast_get_group_list(this_iface_ptr);
  if (if_mcast_grp_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Error accessing multicast group list");
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }
  if_mcast_grp = *if_mcast_grp_ptr;
  while (if_mcast_grp != NULL)
  {
    ps_igmp_set_report_timer(if_mcast_grp, response_time);
    if_mcast_grp = if_mcast_grp->next_mcast_grp;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_igmp_process_gen_query() */


/*===========================================================================
FUNCTION PS_IGMP_PROCESS_GRP_QUERY

DESCRIPTION
  This function processes IGMP group specific queries.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  igmph: pointer to igmp header structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_process_grp_query
(
  ps_iface_type              * this_iface_ptr,
  struct ps_igmp_hdr         * igmph
)
{
  struct ps_mcast_cb   * if_mcast_grp;
  int                    response_time;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if_mcast_grp = ps_igmp_find_v4_group(this_iface_ptr,
                                              igmph->igmp_mcast_grp);
  if (if_mcast_grp != NULL)
  {
/*-------------------------------------------------------------------------
    Max response time in the IGMP header is in units of 1/10 seconds.
    Calculating in milliseconds: 
    => [value_in_pkt] * 1/10 * 1000
    => [value_in_pkt] * 100
  -------------------------------------------------------------------------*/
    response_time = igmph->igmp_resp_time * 100;
    ps_igmp_set_report_timer(if_mcast_grp, response_time);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_igmp_process_grp_query() */


/*===========================================================================
FUNCTION PS_IGMP_SET_REPORT_TIMER

DESCRIPTION
  This function sets multicast report timer corresponding to the given
  multicast group.

PARAMETERS
  if_mcast_grp: multicast group whose timer needs to be set
  response_time: response time within which report needs to be sent

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_igmp_set_report_timer
(
  struct ps_mcast_cb   * if_mcast_grp,
  int                    response_time
)
{
  ps_timer_handle_type  timer_handle;
  int64 remaining_time;
  ps_timer_error_type timer_ret_val;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  timer_handle = if_mcast_grp->mcast_report_timer;
  if (ps_timer_is_running(timer_handle))
  {
    remaining_time = ps_timer_remaining(timer_handle);

/*-------------------------------------------------------------------------
    If timer is already running and query response time is less than the
    remaining time, cancel the timer.
  -------------------------------------------------------------------------*/
    if (remaining_time > response_time)
    {
      timer_ret_val = ps_timer_cancel(if_mcast_grp->
                                      mcast_report_timer);
      if (timer_ret_val != PS_TIMER_SUCCESS)
      {
        LOG_MSG_ERROR_0("Error cancelling the timer");
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return;
      }
    }
    else
    {

/*-------------------------------------------------------------------------
    If timer is already running and query response time is more than the
    remaining time, do nothing and return.
  -------------------------------------------------------------------------*/
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return;
    }
  } /* if (ps_timer_is_running(timer_handle)) ends */

/*-------------------------------------------------------------------------
    If timer is not already running or query response time is less than the
    remaining time of timer, start timer.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_start(timer_handle,
  	                   (int64)ps_utils_generate_16bit_rand_num_by_range(0, 
                                                               response_time));
  if( timer_ret_val == PS_TIMER_FAILURE)
  {
    LOG_MSG_ERROR_0("Error starting timer");
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }
  if_mcast_grp->mcast_state = PS_MCAST_STATE_DELAYING_MEMBER;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_igmp_set_report_timer() */


/*===========================================================================
FUNCTION PS_IGMP_FIND_V4_GROUP

DESCRIPTION
  This function is used to find a multicast group in the list of groups
  associated with the interface. This function is specific to IPV4 addresses.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  igmp_grp: multicast group

RETURN VALUE
  Pointer to multicast group in case the multicast group was found, NULL
  otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_mcast_cb * ps_igmp_find_v4_group
(
  ps_iface_type               *this_iface_ptr,
  struct ps_in_addr           igmp_grp
)
{
  struct ps_mcast_cb  * if_mcast_grp, ** if_mcast_grp_ptr;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    LOG_MSG_ERROR_1("Invalid iface 0x%p", this_iface_ptr);
        return NULL;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
 /*-------------------------------------------------------------------------
    Loop through the list of multicast groups associated with this interface
    looking for the given multicast group.
  -------------------------------------------------------------------------*/
  if_mcast_grp_ptr = ps_iface_mcast_get_group_list(this_iface_ptr);
  if (if_mcast_grp_ptr == NULL)
  {
    LOG_MSG_ERROR_0("Error accessing multicast group list");
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return NULL;
  }
  if_mcast_grp = *if_mcast_grp_ptr;
  while (if_mcast_grp != NULL)
  {
      if (if_mcast_grp->mcast_grp.addr.v4.ps_s_addr == igmp_grp.ps_s_addr)
      {
        PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return if_mcast_grp;
      }
    if_mcast_grp = if_mcast_grp->next_mcast_grp;
  } /* while (if_mcast_grp != NULL) ends */

 /*-------------------------------------------------------------------------
    Group was not found, return NULL.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return NULL;
} /* ps_igmp_find_v4_group() */


