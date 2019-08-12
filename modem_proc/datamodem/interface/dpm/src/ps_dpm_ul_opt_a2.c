/*===========================================================================

                     PS_OPT_PATH_HDLR . C

DESCRIPTION

This is the implementation of Uplink Data Optimized handler.

Copyright (c) 2011-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_ul_opt_a2.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/04/15    gk     Removed un-necessary F3s from data path
08/11/14    cx     Remove code for freeing packet if ip4_fragment_optimized()
                   fails 
09/30/13    wc     DPM and IPA filtering support
09/27/13    wc     Support dynamic port configuration
06/10/13    pgm    Added flow control for Legacy Wmk.
01/23/13    pgm    Added check for pushing V6 frag and ICMP packets to
                   legacy path.
01/18/13    tw     v4 Legacy check for RM filters
12/27/12    tw     Fix IPv6 call up check error
11/07/12    tw     Added added offset to pefix packet check
11/05/12    pgm    Checking Flow control before pushing packets to Legacy Wmk
                   to avoid DSM exhaustion/RLF.
09/18/12    tw     Fix for packet cmp when NW assigns new IPv6 prefix
09/20/12    dvk    Made changes for accessing tcp_ack_prio_enabled global
                   variable from a accessor function.
08/08/12    pgm    Flow control changes for supporting different APNs in
                   Dual-IP.
07/10/12    pgm    Fix for routing packets to the right PDN in a multi-PDN
                   call after flow is enabled.
07/02/12    pgm    Freeing dynamically allocated ndp buff in error cases to
                   avoid memory leak issues.
04/12/12    pgm    Moving Legacy path processing to PS task from PS_RM to
                   avoid race condition issues.
03/09/12    pgm    Added check for PS_FLOW_CAPABILITY_DATA_DISALLOWED.
12/08/02    pgm    Fix for deadlock issue in fallback_sig_hdlr()between PS and
                   PSRM task. Also added rmnet_rx_pkt_hdlr_crit_section.
11/01/11    am     Re-orged for NAT support.
10/14/11    pgm    Fixed Klocwork errors.
09/22/11    pgm    Removed NDP Signature Mask.
09/22/11    pgm    Added a new sig hdlr for MBIM support.
09/14/11    SM     Added TCP ACK PRIO NV item check for dynamic enablement
                   of ACK tagging.
09/08/11    pgm    Featurized the TCP ACK tagging with
                   FEATURE_DATA_PS_TCP_ACK_PRIO.
08/19/11    pgm    TCP ACK prioritization support
                   Code cleanup - Added macros and inline functions
08/15/11    AM     created file
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "dsm.h"

#include "ps_ifacei.h"
#include "ps_iface_defs.h"
#include "ps_phys_linki.h"
#include "ps_crit_sect.h"
#include "ps_dpm_ul_opt.h"
#include "ps_dpm_opt_defs.h"
#include "ps_logging.h"
#include "ps_metai_info.h"
#include "ps_flow.h"
#include "ps_tx_meta_info.h"
#include "ps_stat_phys_link.h"
#include "ps_system_heap.h"
#include "ps_dpmi.h"
#include "ps_dpm.h"
#include "ps_dpm_ul_legacy.h"
#include "ps_dpm_ul_opt_defs.h"
#include "ds_qmi_svc.h"
#include "ps_tcp_config.h"
#include "ps_icmp.h"
#include "ps_ip4_hdr.h"
#include "ps_ip4_frag.h"
#include "ps_icmp6_msg.h"
#include "ps_icmp6.h"
#include "ps_pkt_info_utils.h"
#include "ds3gutil.h"

#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_dpm_clat.h"
#include "ps_clat.h"
#endif /* FEATURE_DATA_PS_464XLAT */
#define PS_DPM_UL_OPT_MAX_FRAG_PACKETS 5

ps_tx_meta_info_type  * tx_meta_info_ptr;


extern ps_dl_opt_filter_spec_type *global_ul_opt_fltr_arr;
/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

static dsm_item_type * pkts_array[PS_DPM_UL_OPT_MAX_FRAG_PACKETS];

static int ps_dpm_ul_opt_fragment_packet
(
  dsm_item_type                   * rx_pkt,
  ps_iface_type                   * iface_ptr,
  uint16                            mtu,
  dsm_item_type                  ** pkts_array,
  uint16                            version,
  uint16                            pkt_len,
  int16                           * ds_errno
)
{
  int                               ret_val = 0;
  struct ip                         ipv4_hdr;
  ps_tx_meta_info_type            * frag_tx_meta_info_ptr = NULL;
  errno_enum_type                   ps_errno;
  ps_icmp6_msg_type                 icmp6_msg;
  ip_pkt_info_type                * pkt_info_ptr;
  uint16                            offset;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
    1.	Find size of the pkt and compare against mtu. 
        If pkt is smaller than mtu then skip all below steps
    2.	If its ipv6 pkt then generate the error and drop the pkt 
    3.	Parse the pkt using ps_ip4_hdr_parse 
    4.	If df bit is not set then call into ip4_fragment_ex, 
        - UL opt code would pass array of ptrs as out parameter.
        - Protocols would fill up the array of ptrs with dsm items of generated 
          fragments
        - If in case of error, array of ptrs would all be NULL and ul opt code 
          would skip further processing 
        - We would go through all non-null entries of array and call tx cmd on
          each of them.
    5.	If df bit is set then we would call icmpv4_frag_needed_error
    -----------------------------------------------------------------------*/
    if (PS_DPM_OPT_V4_VERSION == version)
    {
      offset = 0;
      /*-------------------------------------------------------------------
        Parse Ipv4 header
      -------------------------------------------------------------------*/        
      if (FALSE == ps_ip4_hdr_parse_optimized(rx_pkt, &offset, &ipv4_hdr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_fragment_packet(): Dropping pkt"
                        "Cannot parse ip v4 header ");        
        ret_val = -1;
        break;
      } 
      
      /*-------------------------------------------------------------------
        1. If DF bit is not set then fragment the big packet else call
           icmpv4_frag_needed_error to generate ping response back.
        2. Call ps_iface_tx_cmd for each fragment 
      -------------------------------------------------------------------*/         
      if( ipv4_hdr.df == 0 )
      {
        pkts_array[0] = NULL;
        if (0 != ip4_fragment_optimized(&rx_pkt, 
                                        &ipv4_hdr, 
                                         mtu,
                                         pkts_array,
                                         PS_DPM_UL_OPT_MAX_FRAG_PACKETS))
        {
          /*-------------------------------------------------------------------
            Indication not to free the rx pkt
          -------------------------------------------------------------------*/         
          *ds_errno = DS_EINVAL;

          ret_val = -1;
          break;                              
        }                    
      }
      else
      {          
        if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &frag_tx_meta_info_ptr, &ps_errno))
        {
          pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(frag_tx_meta_info_ptr));
          pkt_info_ptr->if_ptr = iface_ptr;
          icmpv4_frag_needed_error(&rx_pkt, mtu, pkt_info_ptr);
          PS_TX_META_INFO_FREE(&frag_tx_meta_info_ptr);

          /*-------------------------------------------------------------------
            Indication not to free the rx pkt
          -------------------------------------------------------------------*/         
          *ds_errno = DS_EINVAL;
        }
        
        else
        {
          LOG_MSG_ERROR_2("ps_dpm_ul_opt_fragment_packet(): Dropping pkt"
                          "Size of pkt %d greater than mtu size %d for v4 iface"
                          " and DF bit is set ", 
                          pkt_len, mtu);      
        }

        ret_val = -1;
        break;                                  
      }        
    }
    else
    {
      if(-1 != ps_tx_pkt_info_generate(&rx_pkt, &frag_tx_meta_info_ptr, &ps_errno))
      {
        memset(&icmp6_msg, 0, sizeof(ps_icmp6_msg_type));
        icmp6_msg.cmn_hdr.type =  ICMP6_PKT_TOO_BIG;  //pkt too big
        icmp6_msg.cmn_hdr.code =  0;
        icmp6_msg.msg_type.mtu = mtu;
        pkt_info_ptr = &(PS_TX_META_GET_PKT_INFO(frag_tx_meta_info_ptr));
        pkt_info_ptr->if_ptr = iface_ptr;
        icmpv6_error_send( &rx_pkt,
                           &icmp6_msg,
                           pkt_info_ptr);
                           
        PS_TX_META_INFO_FREE(&frag_tx_meta_info_ptr);

        /*-------------------------------------------------------------------
          Indication not to free the rx pkt
        -------------------------------------------------------------------*/         
        *ds_errno = DS_EINVAL;
      }
      else
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_fragment_packet(): Dropping pkt"
                        "Size of pkt %d greater than mtu size %d for v6 iface", 
                        pkt_len, mtu);
      }

      ret_val = -1;
      break;        
    }      
  } while(0);

  return ret_val;
} /* ps_dpm_ul_opt_fragment_packet() */
/*===========================================================================
FUNCTION ps_dpm_ul_opt_tcp_ack_pri_check

DESCRIPTION
  This macro returns whether the packet is a TCP ACK and if it needs to be
  prioritized for IPv4 and IPv6.

PARAMETERS
  pkt_buf     : pointer to the pkt buffer
  ip_hdr_len  : IP header length
  total_ip_len: Total IP length

RETURN VALUE
  TRUE  : if packet is a TCP ACK
  FALSE : otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_dpm_ul_opt_tcp_ack_pri_check
(
  uint8        * pkt_buf,
  uint8          ip_hdr_len,
  uint16         total_ip_len,
  uint8          prot_offset
)
{
  uint8  tcp_header_length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    1. Check if it is a TCP packet
    2. Check if TCP ACK prioritization is enabled
    3. Check if it is a TCP ACK packet. If there is no payload for TCP, then
       it is most likely a TCP ACK
  -------------------------------------------------------------------------*/
  if (*(pkt_buf + prot_offset) == PS_IPPROTO_TCP &&
      ps_pkt_info_get_tcp_ack_prio_enabled())
  {
    tcp_header_length =
      (*(pkt_buf + ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_OFFSET) &
         PS_DPM_OPT_TCP_HDR_LEN_MASK) >> PS_DPM_UL_OPT_PATH_WORD_SHIFT;

    if (total_ip_len == (ip_hdr_len + tcp_header_length))
    {
      return TRUE;
    }
  }

  return FALSE;

} /* ps_dpm_ul_opt_tcp_ack_pri_check() */

/*===========================================================================
FUNCTION PS_OPT_PATH_HGET32_UNALIGNED

DESCRIPTION
  This function extracts 32 bits in host order.

PARAMETERS
  cp - Byte value.

RETURN VALUE
  Returns 32 bit algined value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE dword ps_dpm_ul_opt_hget32_unaligned
(
  byte * cp
)
{

  dword rval;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cp += 4;

  rval = *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);
  rval <<= 8;
  rval |= *(--cp);

  return( rval);

} /* ps_dpm_ul_opt_hget32_unaligned() */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_ipv6_prefix_cmp

DESCRIPTION


PARAMETERS
  pkt_buf    : pointer to the pkt buffer
  info       : Rm state machine's information block

RETURN VALUE
  TRUE  : UM and packet prefix match
  FALSE : Prefix does not match

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_dpm_ul_opt_ipv6_prefix_cmp
(
  uint8               	    * pkt_buf,
  ps_iface_type             * rmnet_iface_ptr
)
{
  uint32   src_v6_prefix_addr32[2] ALIGN(4); //__attribute__ ((aligned(4)));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  src_v6_prefix_addr32[0] = ((uint32) *(uint32 *)
                             (pkt_buf + PS_DPM_OPT_V6_SRC_ADDR_OFFSET));
  src_v6_prefix_addr32[1] = ((uint32) *(uint32 *)
                             (pkt_buf + PS_DPM_OPT_V6_SRC_ADDR_OFFSET+4));

  if ( NULL != rmnet_iface_ptr && 
      (*(uint64*)(&src_v6_prefix_addr32[0])) == rmnet_iface_ptr->
          iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix
     )
  {
    return TRUE;
  }

  LOG_MSG_ERROR_1("ps_dpm_ul_opt_ipv6_prefix_cmp() "
                  "Src V6 prefix 0x%llx doesnt match Um prefix, drop pkt!",
                  ps_ntohll(*(uint64*)((&src_v6_prefix_addr32[0]))));
  return FALSE;
} /* ps_dpm_ul_opt_ipv6_prefix_cmp */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_ipv6_prefix_cmp_unaligned

DESCRIPTION


PARAMETERS
  pkt_buf    : pointer to the pkt buffer
  info       : Rm state machine's information block

RETURN VALUE
  TRUE  : UM and packet prefix match
  FALSE : Prefix does not match

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_dpm_ul_opt_ipv6_prefix_cmp_unaligned
(
  uint8               			* const pkt_buf,
  ps_iface_type             * rmnet_iface_ptr
)
{
  uint32   src_v6_prefix_addr32[2] ALIGN(4); //__attribute__ ((aligned(4)));
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  src_v6_prefix_addr32[0] = ps_dpm_ul_opt_hget32_unaligned(
							 pkt_buf + PS_DPM_OPT_V6_SRC_ADDR_OFFSET);
  src_v6_prefix_addr32[1] = ps_dpm_ul_opt_hget32_unaligned(
                             pkt_buf + PS_DPM_OPT_V6_SRC_ADDR_OFFSET+4);

  if ( NULL != rmnet_iface_ptr &&  
      (*(uint64*)(&src_v6_prefix_addr32[0])) == rmnet_iface_ptr->
          iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix
     )
  {
    return TRUE;
  }

  LOG_MSG_ERROR_1("ps_dpm_ul_opt_ipv6_prefix_cmp_unaligned() "
                  "Src V6 prefix 0x%llx doesnt match Um prefix, drop pkt!",
                  ps_ntohll(*(uint64*)((&src_v6_prefix_addr32[0]))));
  return FALSE;
} /* ps_dpm_ul_opt_ipv6_prefix_cmp_unaligned */


INLINE boolean ps_dpm_ul_opt_process_filters
(
  ps_dl_opt_filter_spec_type * filter_ptr,
  uint16                       src_port,
  uint16                       dst_port,
  uint16                       protocol,
  uint16                       max_filter_entries
)
{
  uint16                   fltr_dst_port_host;
  uint16                   fltr_src_port_host;
  uint16                   pkt_dst_port_host;
  uint16                   pkt_src_port_host;
  uint8                    filter_cnt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( filter_ptr != NULL )
  {
    pkt_dst_port_host = ps_ntohs(dst_port);
    pkt_src_port_host = ps_ntohs(src_port);
    for (filter_cnt = 0;
         filter_cnt < max_filter_entries &&
           filter_ptr->fi_handle != 0;
         filter_cnt++, filter_ptr++)
    {
      fltr_dst_port_host = ps_ntohs(filter_ptr->dst_port);
      fltr_src_port_host = ps_ntohs(filter_ptr->src_port);
      if ( ((protocol == filter_ptr->protocol) || 
            (filter_ptr->protocol == PS_IPPROTO_TCP_UDP && 
             (protocol == PS_IPPROTO_TCP || protocol == PS_IPPROTO_UDP)))&&
           ((pkt_dst_port_host >= fltr_dst_port_host &&
             pkt_dst_port_host <= fltr_dst_port_host + filter_ptr->dst_port_range) ||
            (pkt_src_port_host >= fltr_src_port_host &&
             pkt_src_port_host <= fltr_src_port_host + filter_ptr->src_port_range)))
      {
        LOG_MSG_DATA_PATH_INFO_3("ps_dpm_ul_opt_process_filters: "
                                 "Rx filter pass protocol (%d) dst port"
                                 " (%d) src port (%d)",
                                 protocol, pkt_dst_port_host, 
                                 pkt_src_port_host);
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION PS_UL_OPTIMIZED_IPV4_IS_LEGACY

DESCRIPTION
  This function returns whether the packet needs to take the legacy path or should
  be forwarded to the Um iface for IPv4.
  Packets take the legacy path in following cases
  1. If packet is destined to a broadcast address
  2. if packet is destined to a limited broadcast address
  3. If packet is destined to a multicast address
  4. If packet is fragmented
  5. if packet is destined to Rm iface
  6. if IP header has options

PARAMETERS
  pkt_buf    : pointer to the pkt buffer
  temp_info  : Rm state machine's information block
  ip4_hdr_len: IPv4 Header length

RETURN VALUE
  TRUE  : if packet needs to take legacy path
  FALSE : if packet should be forwarded to the Um iface

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean ps_dpm_ul_opt_ipv4_is_legacy
(
  uint8               * const pkt_buf,
  ps_iface_type       * rmnet_iface_ptr,
  uint8                 ip4_hdr_len
)
{
  ps_dl_opt_filter_spec_type * filter_ptr = NULL;
  ps_iface_type              * rm_base_iface_ptr = NULL;
  uint16                       dst_port = 0;
  uint16                       src_port = 0;
  uint8                        protocol = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*---------------------------------------------------------------------
   Extract the src port, dst port and protocol from the pkt_buf.
  ---------------------------------------------------------------------*/
   protocol = *(pkt_buf + PS_DPM_OPT_V4_PROT_FIELD_OFFSET);
   /*--------------------------------------------------------------------------
    This is to allow ICMP/IGMP packets to match optimized filters installed
    by clients for the same 
    -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_464XLAT
   if( (protocol == PS_IPPROTO_TCP) || (protocol == PS_IPPROTO_UDP) )
#endif
   {
     src_port = (uint16)*((uint16*)(pkt_buf + PS_DPM_OPT_V4_HDR_LEN));
     dst_port = (uint16)*((uint16*)(pkt_buf + PS_DPM_OPT_V4_HDR_LEN + 2));
   }

   /*---------------------------------------------------------------------
     Check to see if RM filters are present instead of checking IP addr.
     Only allow packets which match a filters protocol and port range to
     be pass.
   ---------------------------------------------------------------------*/
   rm_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(rmnet_iface_ptr);
   filter_ptr = PS_IFACEI_GET_OPT_FILTER_BASE(rm_base_iface_ptr);
   if (ps_dpm_ul_opt_process_filters(filter_ptr, 
                                     src_port, 
                                     dst_port, 
                                     protocol,
                                     PS_IFACE_MAX_OPT_FILTER_ENTRIES)) 
   {
     return TRUE;
   }


   /* If filters installed on iface fails, then run filters on global filter array for Rm group */
   if (ps_dpm_ul_opt_process_filters(global_ul_opt_fltr_arr, 
                                     src_port, 
                                     dst_port, 
                                     protocol,
                                     MAX_GLOBAL_OPT_FILTER_ENTRIES)) 
   {
     return TRUE;
   }
   
  if ( 
       (((uint16) *((uint16 *) (pkt_buf +
        PS_DPM_OPT_V4_FRAG_OFFSET)) &
        ps_htons(PS_DPM_OPT_V4_FRAG_MASK)) != 0)  ||
       
       PS_IN_IS_ADDR_LIMITED_BROADCAST
       ( (uint32) *(uint32 *)(pkt_buf +
         PS_DPM_OPT_V4_DEST_ADDR_OFFSET)
       )                                                    ||
       PS_IN_IS_ADDR_MULTICAST
       ( (uint32) *(uint32 *)(pkt_buf +
         PS_DPM_OPT_V4_DEST_ADDR_OFFSET)
       )                                                    ||
       
       (ip4_hdr_len > PS_DPM_OPT_V4_HDR_LEN)
     )
   {
     return TRUE;
   }   

   return FALSE;
} /* ps_dpm_ul_opt_ipv4_is_legacy() */

/*===========================================================================
FUNCTION ps_dpm_ul_opt_ipv6_is_legacy

DESCRIPTION
  This macro returns whether the packet needs to take the legacy path or should
  be forwarded to the Um iface for IPv6. It uses optimized function to get 32
  bit values.
  Packets take the legacy path in following cases
  1. If packet is destined to a linklocal address
  2. If packet is destined to a multicast address
  3. If packet has the frag hdr
  4. If packet is ICMPv6

PARAMETERS
  pkt_buf    : pointer to the pkt buffer

RETURN VALUE
  TRUE  : if packet needs to take legacy path
  FALSE : if packet should be forwarded to the Um iface

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_dpm_ul_opt_ipv6_is_legacy
(
  uint8  * const pkt_buf
)
{
  if ( 
       ((uint8) (* (pkt_buf + PS_DPM_OPT_V6_NXT_HDR_OFFSET)) ==
       (uint8)PS_IPPROTO_ICMP6)           ||
  
       ((uint8) (* (pkt_buf + PS_DPM_OPT_V6_NXT_HDR_OFFSET)) ==
       (uint8)PS_IPPROTO_FRAG_HDR)       ||
       
       PS_IN6_IS_PREFIX_LINKLOCAL
       (ps_dpm_ul_opt_hget32_unaligned(pkt_buf +
         PS_DPM_OPT_V6_DEST_ADDR_OFFSET)) ||

       ((uint8) (* (pkt_buf + PS_DPM_OPT_V6_DEST_ADDR_OFFSET)) ==
       PS_DPM_OPT_V6_MCAST_MASK)
     )
  {
    return TRUE;
  }

  return FALSE;
} /* ps_dpm_ul_opt_ipv6_is_legacy() */

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void ps_dpm_ul_opt_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each IP pkt in
    UL optimized path
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);
  if (NULL == tx_meta_info_ptr)
  {
    DATA_ERR_FATAL("ps_dpm_ul_opt_init(): Couldn't alloc meta info");
    return;
  }

  return;
} /* ps_dpm_ul_opt_init() */

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  ps_dpm_ul_opt_path_type           path_type = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val   = FALSE;
  ps_tx_meta_info_type            * qos_tx_meta_info_ptr = NULL;
  uint8                             pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig);
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {

    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;

    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;

      if (NULL == rm_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "rm iface 0x%p is NULL", rm_iface_ptr);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }   

      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }       

      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
    }

    else if (version == PS_DPM_OPT_V6_VERSION)              /* IPv6 packet */
    {
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;

      if (NULL == rm_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "rm iface 0x%p is NULL", rm_iface_ptr);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }       
      
        /*-------------------------------------------------------------------
          Check CR 692508
        -------------------------------------------------------------------*/
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;      
    }
    else
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) or 
           Dual-IP over different RmNet call- Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is, go to step 3 else goto step
           4.
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled, goto step4
        4 If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(dpm_rm_info_ptr->te_flow_control || 
          PS_FLOW_IS_TX_ENABLED(ps_flow_ptr)) ||
        !(dpm_rm_info_ptr->te_flow_control || 
          PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr)) ||
        !(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(dpm_rm_info_ptr->te_flow_control || 
              PS_FLOW_IS_TX_ENABLED(companion_ps_flow_ptr)) ||
            !(dpm_rm_info_ptr->te_flow_control || 
              PS_PHYS_LINK_FLOW_ENABLED(companion_ps_phys_link_ptr)) ||
            !(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    um_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(um_iface_ptr);
    if (NULL == um_base_iface_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                      "um base iface 0x%p is NULL", um_base_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }     
    
    if (version == PS_DPM_OPT_V4_VERSION)
    {
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
          (PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = PS_DPM_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));                                                                                               

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, rm_iface_ptr, ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;  
        break;        
      }   

      /*---------------------------------------------------------------------
        LOW LATENCY TRAFFIC handling
       ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
                           
      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Um's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }   
    }

    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {    
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr), 
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }   
    }      

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS &&
           NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

      if (PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT) > 0)
      {
        //Tagging used for DPM QOS Testing
      #ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      
        path_type = PS_DPM_UL_OPT_QOS_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    path_type = PS_DPM_UL_OPT_OPT_PATH;

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, um_base_iface_ptr);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32) ps_flow_ptr);
                                 
    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_QOS_PATH:
      {
        PS_TX_META_INFO_AND_RT_META_INFO_GET(qos_tx_meta_info_ptr);
        if (qos_tx_meta_info_ptr == NULL ||
            PS_TX_META_GET_RT_META_INFO_PTR(qos_tx_meta_info_ptr) == NULL)
        {
          LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr() Cannot allocate "
                          "meta info tx 0x%p", qos_tx_meta_info_ptr);
          dsm_free_packet(&rx_pkt);
          rx_pkt = NULL;
          break;
        }

          /*-----------------------------------------------------------------------
            Make sure that the power save mode is disabled after recieving the first 
            uplink packet on this path
            -----------------------------------------------------------------------*/
          if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
          {
            if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
            {
              ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
            }
          }
        
        PS_TX_META_SET_ROUTING_CACHE(qos_tx_meta_info_ptr, um_base_iface_ptr);  
        ps_iface_tx_cmd(um_base_iface_ptr, &rx_pkt, qos_tx_meta_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr() */

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  ps_dpm_qos_hdr_type               qos_hdr;
  ps_dpm_qos2_hdr_type              qos2_hdr;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  ip_version_enum_type              ip_version;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  uint8                             qos_hdr_format;
  uint8                             qos_hdr_offset = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;
  qos_hdr_format     = dpm_rm_info_ptr->qos_header_format;
  if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
  {
    qos_hdr_offset = PS_DPM_OPT_QOS2_HEADER_OFFSET;
  }
  else
  {
    qos_hdr_offset = PS_DPM_OPT_QOS_HEADER_OFFSET;
  }

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1( "ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                              "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, qos_hdr_offset, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;

      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }      

      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
    }
    else if (version == PS_DPM_OPT_V6_VERSION)              /* IPv6 packet */
    {
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;
      
      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }      

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
    }
    else
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type   = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    if (version == PS_DPM_OPT_V4_VERSION)
    {
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                      (PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            qos_hdr_offset,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = PS_DPM_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET))); 
        
      ip_version = IP_V4;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE 
          == ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, rm_iface_ptr, ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        LOW LATENCY TRAFFIC handling
       ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            qos_hdr_offset,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Rm's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      ip_version = IP_V6;
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr & assign the rx_pkt back since it might get modified
      by dsm_pullup
    -----------------------------------------------------------------------*/
    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {    
      (void) dsm_pullup(&rx_pkt, &qos2_hdr, sizeof(qos2_hdr));
    }
    else
    {
      (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    }

    pkts_array[num_pkts] = rx_pkt;

    um_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(um_iface_ptr);
    
    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {     
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
          NULL != pkts_array[num_pkts]) 
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);
    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             ip_version,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void *)&qos2_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }
    else
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             ip_version,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void*)&qos_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     


    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_ip_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_ip_qos_rx_sig_hdlr() */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_ETH_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr = NULL;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  uint16                            ether_type;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val     = FALSE;
  ps_tx_meta_info_type            * qos_tx_meta_info_ptr = NULL;
  uint8                             pkt_buf[PS_OPT_PATH_ETH_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint16                            buf_len = 0;
  uint8                             ip_hdr_first_byte; 
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, PS_DPM_OPT_ETH_HEADER_OFFSET, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;

      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }      

      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
    }

    else if (version == PS_DPM_OPT_V6_VERSION)
    {
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                          
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;
      
        /*-------------------------------------------------------------------
          Check CR 692508
        -------------------------------------------------------------------*/
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
    }
    else
    {
      (void) dsm_extract(rx_pkt,
                         PS_DPM_OPT_ETHERTYPE_OFFSET,
                         &ether_type,
                         sizeof(ether_type));

      ether_type = ps_ntohs(ether_type);

      if (LAN_ETHERTYPE_ARP != ether_type)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "Unknown ether type 0x%x for sig %d",
                        ether_type, rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      path_type    = PS_DPM_UL_OPT_LEGACY_PATH;
      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;

      break;      
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(dpm_rm_info_ptr->te_flow_control || 
          PS_FLOW_IS_TX_ENABLED(ps_flow_ptr)) ||
        !(dpm_rm_info_ptr->te_flow_control || 
          PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr)) ||
        !(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(dpm_rm_info_ptr->te_flow_control || 
              PS_FLOW_IS_TX_ENABLED(companion_ps_flow_ptr)) ||
            !(dpm_rm_info_ptr->te_flow_control || 
              PS_PHYS_LINK_FLOW_ENABLED(companion_ps_phys_link_ptr)) ||
            !(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    if (version == PS_DPM_OPT_V4_VERSION)
    {
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                            (PS_OPT_PATH_ETH_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
                            
      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = PS_DPM_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));  
      
      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, rm_iface_ptr, ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        LOW LATENCY TRAFFIC handling
       ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }
    }
    else if(version == PS_DPM_OPT_V6_VERSION)
    {
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Rm's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
    }    
    else
    {
      path_type    = PS_DPM_UL_OPT_LEGACY_PATH;
      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      break;
    }

    /*-----------------------------------------------------------------------
      Pull out Ethernet Header & assign the rx_pkt back since it
      might get modified by dsm_pullup

    -----------------------------------------------------------------------*/
    (void) dsm_pullup(&rx_pkt, NULL, PS_DPM_OPT_ETH_HEADER_OFFSET);
    pkts_array[num_pkts] = rx_pkt;

    um_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(um_iface_ptr);
    
    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {     
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while ( num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
          NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    //MSR TODO validate that this is getting updated for all modes
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      Check if there are any QoS filters on Um iface. 
    -----------------------------------------------------------------------*/
    if (PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT) > 0)
    {
      path_type = PS_DPM_UL_OPT_QOS_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    path_type = PS_DPM_UL_OPT_OPT_PATH;

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, um_base_iface_ptr);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32) ps_flow_ptr);

    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_QOS_PATH:
      {
        PS_TX_META_INFO_AND_RT_META_INFO_GET(qos_tx_meta_info_ptr);
        if (qos_tx_meta_info_ptr == NULL ||
            PS_TX_META_GET_RT_META_INFO_PTR(qos_tx_meta_info_ptr) == NULL)
        {
          LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr() Cannot allocate "
                          "meta info tx 0x%p", qos_tx_meta_info_ptr);
          dsm_free_packet(&rx_pkt);
          rx_pkt = NULL;
          break;
        }

          /*-----------------------------------------------------------------------
            Make sure that the power save mode is disabled after recieving the first 
            uplink packet on this path
           -----------------------------------------------------------------------*/
          if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
          {
            if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
            {
              ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
            }
          }

        PS_TX_META_SET_ROUTING_CACHE(qos_tx_meta_info_ptr, um_base_iface_ptr); 
        ps_iface_tx_cmd(um_base_iface_ptr, &rx_pkt, qos_tx_meta_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  } while (0);

  return ret_val;
} /* ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr() */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_ETH_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_eth_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  ps_dpm_qos_hdr_type               qos_hdr;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  ip_version_enum_type              ip_version;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_ETH_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, PS_DPM_OPT_QOS_HEADER_OFFSET + PS_DPM_OPT_ETH_HEADER_OFFSET, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 and IPv6 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION)
    {
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;

      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }      

      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
    }

    else if (version == PS_DPM_OPT_V6_VERSION)
    {
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;
      
      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
    }
    else
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    if (version == PS_DPM_OPT_V4_VERSION)
    {
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                                  (PS_OPT_PATH_ETH_QOS_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_QOS_HEADER_OFFSET + PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
                            
      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Get the offset of protocol field and also the packet's total length
      ---------------------------------------------------------------------*/
      prot_offset = PS_DPM_OPT_V4_PROT_FIELD_OFFSET;
      total_len   =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET))); 
        
      ip_version = IP_V4;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE ==
              ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, rm_iface_ptr, ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }    

      /*---------------------------------------------------------------------
        LOW LATENCY TRAFFIC handling
       ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_QOS_HEADER_OFFSET + PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Rm's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      ip_version = IP_V6;    
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr, Ethernet Header & assign the rx_pkt back since it
      might get modified by dsm_pullup

    -----------------------------------------------------------------------*/
    (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    (void) dsm_pullup(&rx_pkt, NULL, PS_DPM_OPT_ETH_HEADER_OFFSET);
    pkts_array[num_pkts] = rx_pkt;

    um_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(um_iface_ptr);
    
    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {     
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
           NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    ps_dpm_process_qos_hdr(rx_pkt,
                           um_base_iface_ptr,
                           ip_version,
                           TRUE,
                           dup_tx_meta_info_ptr,
                           (void*)&qos_hdr,
                           PS_DPM_QOS_FORMAT_6_BYTE,
                           &ps_flow_ptr);

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    ---------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_eth_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  } while(0);

  return ret_val;
} /* ps_dpm_ul_opt_eth_qos_rx_sig_hdlr() */

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_EMBMS_RX_SIG_HDLR

DESCRIPTION
  This function drops any data for embms call becuase for embms call
  there is no data expected on uplink path

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_embms_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type * dpm_rm_info_ptr;
  dsm_item_type       * rx_pkt;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  if (NULL != dpm_rm_info_ptr)
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
    LOG_MSG_INFO1_0("ps_dpm_ul_opt_embms_rx_sig_hdlr : Dropping packet for "
                    "embms call as not data is expected on uplink");
    dsm_free_packet(&rx_pkt);
  }
    
  return FALSE;
} /* ps_dpm_ul_opt_embms_rx_sig_hdlr */

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr = NULL;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr = NULL;
  ps_iface_type                   * logical_iface_ptr = NULL;
  dsm_item_type                   * rx_pkt;
  ps_dpm_ul_opt_path_type           path_type = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val   = FALSE;
  ps_tx_meta_info_type            * qos_tx_meta_info_ptr = NULL;
  uint8                             pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  boolean                           is_pkt_trnsltd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {        
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;

    /*-------------------------------------------------------------------------
      1. Setup iface info for v4 packets & for v6 packets
      2. Handle flow control
      3. For v4 packets first fragment if required & then translate them to v6.
         This step is not required for v6 packets
      4. Send out the translated v6 packets
    -------------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv4 packets
      -----------------------------------------------------------------------*/
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;     

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else if(version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr; 

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else 
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type   = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-------------------------------------------------------------------
      To handle packet being sent when call is being brought down
    -------------------------------------------------------------------*/
    if (NULL == um_iface_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "Call in down or going down state um iface 0x%p "
                      "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }
    
    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);

    /*-----------------------------------------------------------------------
      For checking flow control, logical iface need to be used. Hence get
      logical iface info from CLAT um iface

      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) or 
           Dual-IP over different RmNet call- Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is, go to step 3 else goto step
           4.
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled, goto step4
        4 If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    logical_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(um_iface_ptr);
	if(!PS_IFACE_IS_VALID(logical_iface_ptr))
	{
	  LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr():"
	  	              "failed to find associated um_iface_ptr 0x%p",
	  	              um_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
	  break;
	}
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(logical_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(logical_iface_ptr);

    if (!(dpm_rm_info_ptr->te_flow_control || 
          PS_FLOW_IS_TX_ENABLED(ps_flow_ptr)) ||
        !(dpm_rm_info_ptr->te_flow_control || 
          PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr)) ||
        !(PS_IFACEI_FLOW_ENABLED(logical_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(dpm_rm_info_ptr->te_flow_control || 
              PS_FLOW_IS_TX_ENABLED(companion_ps_flow_ptr)) ||
            !(dpm_rm_info_ptr->te_flow_control || 
              PS_PHYS_LINK_FLOW_ENABLED(companion_ps_phys_link_ptr)) ||
            !(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;
          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;
        break;
      }
    }

    if (version == PS_DPM_OPT_V4_VERSION) 
    {    
      if(um_base_iface_ptr->clat_pdn_cntxt == NULL)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "clat pdn cntx NULL for iface 0x%x:%d", 
                         um_base_iface_ptr->name, um_base_iface_ptr->instance);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                (PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            0,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);
      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }    
            
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, 
                                         rm_iface_ptr, 
                                         ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
              LOW LATENCY TRAFFIC handling
             ---------------------------------------------------------------------*/
       if( ds3g_get_config_latency_info())
       {
         ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                    ip_hdr_len,
                                                    prot_offset );
       }

      /*---------------------------------------------------------------------
        Get the packet's total length
      ---------------------------------------------------------------------*/
      total_len   =
           ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));
      
     /*-----------------------------------------------------------------------
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface before it is fragmented/translated
      -----------------------------------------------------------------------*/
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
      
      if (total_len > PS_IFACE_GET_MTU(um_iface_ptr))
      {    
        frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           rm_iface_ptr,
                                           PS_IFACE_GET_MTU(um_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
        if (-1 == frag_result)
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;
          break;
        }   
      }
                
      while ((num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS) && 
             (NULL != pkts_array[num_pkts]))
      {
        /* Translate v4 packet to v6 */
        if (-1 == ps_clat_ip_to_ip6(&(pkts_array[num_pkts]),
                                    rm_iface_ptr,
                                    ((ps_dpm_clat_pdn_cntxt_info_type *)
                                     (um_base_iface_ptr->clat_pdn_cntxt))->client_pdn_handle))
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;

          /*-------------------------------------------------------------------
            To make sure we dont drop the pkt since protcols would have already
            dropped
          -------------------------------------------------------------------*/
          ds_errno = DS_EINVAL;
          break;
        }

        num_pkts++;
      }

      /*---------------------------------------------------------------------
        Reset num_pkts & rx_pkt which will be used when packets are sent out
      ---------------------------------------------------------------------*/	      
      num_pkts = 0;
      rx_pkt = pkts_array[num_pkts];

      is_pkt_trnsltd = TRUE;
    }
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
           NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    
    /*-------------------------------------------------------------------------
      We need to point rx_pkt to point to right fragment when a pkt is 
      fragmented and then they need to be queued to legacy watermark 
    -------------------------------------------------------------------------*/
    rx_pkt = pkts_array[num_pkts]; 
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    /*---------------------------------------------------------------------
      Extract IP header and port information from the IP packet
    ---------------------------------------------------------------------*/
    buf_len = dsm_extract(rx_pkt,
                          0,
                          pkt_buf,
                          PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

    if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
    total_len =
      ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
    total_len  += PS_DPM_OPT_V6_HDR_LEN;
    ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
    prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

    /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    } 

    /*-----------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Rm's prefix.
      Applicable only to regular v6 packets
    -----------------------------------------------------------------------*/
    if ((FALSE  == is_pkt_trnsltd) && 
         (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr)))
    {
      LOG_MSG_ERROR_0("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "IPV6 prefix cmp failed");      
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    if (FALSE == is_pkt_trnsltd)
    {
      /*-----------------------------------------------------------------------
        If the pkt got translated then it is already logged. Hence this is NA.
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
      -----------------------------------------------------------------------*/
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
    }

    if (PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT) > 0)
    {
      path_type = PS_DPM_UL_OPT_QOS_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    path_type = PS_DPM_UL_OPT_OPT_PATH;

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, um_base_iface_ptr);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32) ps_flow_ptr);
                                 
    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_QOS_PATH:
      {
        PS_TX_META_INFO_AND_RT_META_INFO_GET(qos_tx_meta_info_ptr);
        if (qos_tx_meta_info_ptr == NULL ||
            PS_TX_META_GET_RT_META_INFO_PTR(qos_tx_meta_info_ptr) == NULL)
        {
          LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr() Cannot allocate "
                          "meta info tx 0x%p", qos_tx_meta_info_ptr);
          dsm_free_packet(&rx_pkt);
          rx_pkt = NULL;
          break;
        }

          /*-----------------------------------------------------------------------
            Make sure that the power save mode is disabled after recieving the first 
            uplink packet on this path
            -----------------------------------------------------------------------*/
          if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
          {
            if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
            {
              ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
            }
          }
        
        PS_TX_META_SET_ROUTING_CACHE(qos_tx_meta_info_ptr, um_base_iface_ptr);  
        ps_iface_tx_cmd(um_base_iface_ptr, &rx_pkt, qos_tx_meta_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr() */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr = NULL;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr = NULL;
  ps_iface_type                   * logical_iface_ptr = NULL;
  dsm_item_type                   * rx_pkt;
  ps_dpm_qos_hdr_type               qos_hdr;
  ps_dpm_qos2_hdr_type              qos2_hdr;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
  uint8                             qos_hdr_format;
  uint8                             qos_hdr_offset = 0;
  boolean                           is_pkt_trnsltd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;
  qos_hdr_format     = dpm_rm_info_ptr->qos_header_format;
  if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
  {
    qos_hdr_offset = PS_DPM_OPT_QOS2_HEADER_OFFSET;
  }
  else
  {
    qos_hdr_offset = PS_DPM_OPT_QOS_HEADER_OFFSET;
  }
  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
  
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, qos_hdr_offset, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;

    /*-----------------------------------------------------------------------
      Do appropriate processing for IPv4 packets
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {    
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv4 packets
      -----------------------------------------------------------------------*/
      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;  

      um_iface_ptr = dpm_rm_info_ptr->v4_logical_iface_ptr;     

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v6_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v6_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else if(version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr; 

      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr; 

      /*-----------------------------------------------------------------------
        For companion_um_iface_ptr we first get the CLAT iface & then get the 
        associated logical iface
      -----------------------------------------------------------------------*/
      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
      if (PS_IFACE_IS_VALID(companion_um_iface_ptr))
      {
        companion_um_iface_ptr = 
                            PS_IFACEI_GET_ASSOC_IFACE(companion_um_iface_ptr);
      }
    }

    else 
    {
      LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "Unknown IP type for sig %d", rx_sig);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    /*-------------------------------------------------------------------
      To handle packet being sent when call is being brought down
    -------------------------------------------------------------------*/
    if (NULL == um_iface_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "Call in down or going down state um iface 0x%p "
                      "rm iface 0x%p ", um_iface_ptr, rm_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      break;
    }

    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);

    /*-----------------------------------------------------------------------
      For checking flow control, logical iface need to be used. Hence get
      logical iface info from CLAT um iface

      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    logical_iface_ptr = PS_IFACEI_GET_ASSOC_IFACE(um_iface_ptr);
	if(!PS_IFACE_IS_VALID(logical_iface_ptr))
	{
	  LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr():"
	  	              "failed to find associated um_iface_ptr 0x%p",
	  	              um_iface_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
	  break;
	}
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(logical_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(logical_iface_ptr);

    if (!(PS_IFACEI_FLOW_ENABLED(logical_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr & assign the rx_pkt back since it might get modified
      by dsm_pullup

    -----------------------------------------------------------------------*/
    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      (void) dsm_pullup(&rx_pkt, &qos2_hdr, sizeof(qos2_hdr));
    }

    else
    {
      (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    }
    pkts_array[num_pkts] = rx_pkt;

    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      if(um_base_iface_ptr->clat_pdn_cntxt == NULL)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr(): "
                        "clat pdn cntx NULL for iface 0x%x:%d", 
                         um_base_iface_ptr->name, um_base_iface_ptr->instance);
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }
      
      /*---------------------------------------------------------------------
        Fetch IHL and use it to extract IP header and port information from
        the IP packet
      ---------------------------------------------------------------------*/
      ip_hdr_len = (ip_hdr_first_byte & PS_DPM_OPT_IP_HDR_LEN_MASK) << 2;

      if( (ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > 
                            (PS_OPT_PATH_IP_QOS2_HEADER_LEN + 1) )
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_QOS_HEADER_OFFSET,
                            pkt_buf,
                            ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((ip_hdr_len + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }    
            
      if (TRUE ==
            ps_dpm_ul_opt_ipv4_is_legacy(pkt_buf, 
                                         rm_iface_ptr, 
                                         ip_hdr_len))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
              LOW LATENCY TRAFFIC handling
             ---------------------------------------------------------------------*/
      if( ds3g_get_config_latency_info())
      {
        ps_dpm_ul_v4_low_latency_traffic_handler ( pkt_buf,
                                                   ip_hdr_len,
                                                   prot_offset );
      }

      /*---------------------------------------------------------------------
        Get the packet's total length
      ---------------------------------------------------------------------*/
      total_len   =
           ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V4_TOTAL_LEN_OFFSET)));
      
      /*-----------------------------------------------------------------------
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface before it is fragmented/translated
      -----------------------------------------------------------------------*/
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
      
      if (total_len > PS_IFACE_GET_MTU(um_iface_ptr))
      {
        frag_result = ps_dpm_ul_opt_fragment_packet (
                                                 rx_pkt,
                                                 rm_iface_ptr,
                                                 PS_IFACE_GET_MTU(um_iface_ptr),
                                                 pkts_array,
                                                 version,
                                                 total_len,
                                                 &ds_errno);
        if (-1 == frag_result)
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;
          break;
        }    
      }

      while ((num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS) && 
              (NULL != pkts_array[num_pkts]))
      {
        /* Translate the v4 packet to v6 */
        if (-1 == ps_clat_ip_to_ip6(
                      &(pkts_array[num_pkts]),
                      rm_iface_ptr,
                      ((ps_dpm_clat_pdn_cntxt_info_type *)
                      (um_base_iface_ptr->clat_pdn_cntxt))->client_pdn_handle))
        {
          path_type = PS_DPM_UL_OPT_NO_PATH;

          /*-------------------------------------------------------------------
            To make sure we dont drop the pkt since protcols would have already
            dropped
          -------------------------------------------------------------------*/
          ds_errno = DS_EINVAL;
          break;
        }

        num_pkts++;
      }

      /*---------------------------------------------------------------------
        Reset num_pkts & rx_pkt which will be used when packets are sent out
      ---------------------------------------------------------------------*/	      
      num_pkts = 0;
      rx_pkt = pkts_array[num_pkts];

      is_pkt_trnsltd = TRUE;
    }
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS &&
           NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    /*-------------------------------------------------------------------------
      We need to point rx_pkt to point to right fragment when a pkt is 
      fragmented and then they need to be queued to legacy watermark 
    -------------------------------------------------------------------------*/
    rx_pkt = pkts_array[num_pkts]; 
       
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    /*---------------------------------------------------------------------
      Extract IP header and port information from the IP packet
    ---------------------------------------------------------------------*/
    buf_len = dsm_extract(rx_pkt,
                          PS_DPM_OPT_QOS_HEADER_OFFSET,
                          pkt_buf,
                          PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

    if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
      Get the offset of protocol field, IP header length, and also the
      packet's total length
    ---------------------------------------------------------------------*/
    total_len =
      ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
    total_len  += PS_DPM_OPT_V6_HDR_LEN;
    ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
    prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

    /*-----------------------------------------------------------------------
      Check if packet needs to be sent up the legacy path
    -----------------------------------------------------------------------*/
    if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Drop the packet if the packet's source prefix doesn't match Rm's prefix.
      Applicable only to regular v6 packets
    -----------------------------------------------------------------------*/
    if ((FALSE  == is_pkt_trnsltd) && 
         (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, rm_iface_ptr)))
    {
      LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                      "IPV6 prefix cmp failed");      
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    if (FALSE == is_pkt_trnsltd)
    {
      /*-----------------------------------------------------------------------
        If the pkt got translated then it is already logged. Hence this is NA.
        Increment packet stats on Rm iface
      -----------------------------------------------------------------------*/
      rm_iface_ptr->iface_i_stats.pkts_rx++;
      rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

      /*-----------------------------------------------------------------------
        Log the packet on the Rm iface
      -----------------------------------------------------------------------*/
      DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);
    }

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             IP_V6,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void *)&qos2_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }
    else
    {
      ps_dpm_process_qos_hdr(rx_pkt,
                             um_base_iface_ptr,
                             IP_V6,
                             TRUE,
                             dup_tx_meta_info_ptr,
                             (void*)&qos_hdr,
                             qos_hdr_format,
                             &ps_flow_ptr);
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     


    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr() */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_ETH_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  uint16                            ether_type;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  boolean                           ret_val     = FALSE;
  ps_tx_meta_info_type            * qos_tx_meta_info_ptr = NULL;
  uint8                             pkt_buf[PS_OPT_PATH_ETH_NO_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint16                            buf_len = 0;
  uint8                             ip_hdr_first_byte; 
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {    
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, PS_DPM_OPT_ETH_HEADER_OFFSET, &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      For CLAT, Protocols doesnt support converting ethernet packets, so
      DPM takes legacy path and let protocols take care of CLAT
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      break;                                     
    }
    else if (version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;
      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }      

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
                                 
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;

      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Um's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, um_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }                                 
    }
    else
    {
      (void) dsm_extract(rx_pkt,
                         PS_DPM_OPT_ETHERTYPE_OFFSET,
                         &ether_type,
                         sizeof(ether_type));

      ether_type = ps_ntohs(ether_type);

      if (LAN_ETHERTYPE_ARP != ether_type)
      {
        LOG_MSG_ERROR_2("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                        "Unknown ether type 0x%x for sig %d",
                        ether_type, rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      if (NULL == v4_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                        "v4 call not UP for sig %d", rx_sig);
        path_type   = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      path_type    = PS_DPM_UL_OPT_LEGACY_PATH;
      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;

      break;
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(dpm_rm_info_ptr->te_flow_control || 
          PS_FLOW_IS_TX_ENABLED(ps_flow_ptr)) ||
        !(dpm_rm_info_ptr->te_flow_control || 
          PS_PHYS_LINK_FLOW_ENABLED(ps_phys_link_ptr)) ||
        !(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(dpm_rm_info_ptr->te_flow_control || 
              PS_FLOW_IS_TX_ENABLED(companion_ps_flow_ptr)) ||
            !(dpm_rm_info_ptr->te_flow_control || 
              PS_PHYS_LINK_FLOW_ENABLED(companion_ps_phys_link_ptr)) ||
            !(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    /*-----------------------------------------------------------------------
      Pull out Ethernet header & assign the rx_pkt back since it  
      might get modified by dsm_pullup
    -----------------------------------------------------------------------*/
    (void) dsm_pullup(&rx_pkt, NULL, PS_DPM_OPT_ETH_HEADER_OFFSET);
    pkts_array[num_pkts] = rx_pkt;

    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);
    
    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {     
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
          NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts];

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    //MSR TODO validate that this is getting updated for all modes
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      Check if there are any QoS filters on Um iface. 
    -----------------------------------------------------------------------*/
    if (PS_IFACEI_NUM_FILTERS(um_base_iface_ptr, IP_FLTR_CLIENT_QOS_OUTPUT) > 0)
    {
      path_type = PS_DPM_UL_OPT_QOS_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*-----------------------------------------------------------------------
      NOTE: AFTER THIS POINT, THERE MUST NOT BE ANY ERROR AND PACKET MUST BE
      SENT TO Um IFACE VIA OPTIMIZED PATH, ELSE UNDUP THE META INFO
    -----------------------------------------------------------------------*/

    path_type = PS_DPM_UL_OPT_OPT_PATH;

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    /*-----------------------------------------------------------------------
      Update meta info with routing cache and QoS filter result
    -----------------------------------------------------------------------*/
    PS_TX_META_SET_ROUTING_CACHE(dup_tx_meta_info_ptr, um_base_iface_ptr);
    PS_TX_META_SET_FILTER_RESULT(dup_tx_meta_info_ptr,
                                 IP_FLTR_CLIENT_QOS_OUTPUT,
                                 (uint32) ps_flow_ptr);

    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    -----------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
          um_base_iface_ptr->iface_private.tx_cmd
          (
            um_base_iface_ptr,
            &rx_pkt,
            dup_tx_meta_info_ptr,
            um_base_iface_ptr->iface_private.tx_cmd_info
          );
        break;
      }

      case PS_DPM_UL_OPT_QOS_PATH:
      {
        PS_TX_META_INFO_AND_RT_META_INFO_GET(qos_tx_meta_info_ptr);
        if (qos_tx_meta_info_ptr == NULL ||
            PS_TX_META_GET_RT_META_INFO_PTR(qos_tx_meta_info_ptr) == NULL)
        {
          LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr() Cannot allocate "
                          "meta info tx 0x%p", qos_tx_meta_info_ptr);
          dsm_free_packet(&rx_pkt);
          rx_pkt = NULL;
          break;
        }

          /*-----------------------------------------------------------------------
            Make sure that the power save mode is disabled after recieving the first 
            uplink packet on this path
           -----------------------------------------------------------------------*/
          if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
          {
            if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
            {
              ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
            }
          }

        PS_TX_META_SET_ROUTING_CACHE(qos_tx_meta_info_ptr, um_base_iface_ptr); 
        ps_iface_tx_cmd(um_base_iface_ptr, &rx_pkt, qos_tx_meta_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }

    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr() */


/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_ETH_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr      = NULL;
  ps_dpm_um_info_type             * v4_dpm_um_info_ptr   = NULL;
  ps_dpm_um_info_type             * v6_dpm_um_info_ptr   = NULL;
  ps_tx_meta_info_type            * dup_tx_meta_info_ptr = NULL;
  ps_phys_link_type               * ps_phys_link_ptr;
  ps_phys_link_type               * companion_ps_phys_link_ptr;
  ps_flow_type                    * ps_flow_ptr;
  ps_flow_type                    * companion_ps_flow_ptr;
  ps_iface_type                   * rm_iface_ptr = NULL;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * um_base_iface_ptr = NULL;
  ps_iface_type                   * companion_um_iface_ptr;
  dsm_item_type                   * rx_pkt;
  ps_dpm_qos_hdr_type               qos_hdr;
  ps_dpm_ul_opt_path_type           path_type   = PS_DPM_UL_OPT_OPT_PATH;
  ip_version_enum_type              ip_version;
  boolean                           ret_val     = FALSE;
  uint8                             pkt_buf[PS_OPT_PATH_ETH_QOS_HEADER_LEN + 1] ALIGN(4); //__attribute__ ((aligned(4)));
  uint8                             ip_hdr_len  = 0;
  uint8                             prot_offset = 0;
  uint16                            total_len   = 0;
  uint16                            version = 0;
  uint8                             ip_hdr_first_byte;  
  uint16                            buf_len = 0;
  boolean                           enable      = FALSE;
  sint15                            ps_errno;
  uint8                             num_pkts = 0;
  int                               frag_result = -1;
  int16                             ds_errno = DS_ENOERR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data;

  v4_dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
  v6_dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;

  /*-------------------------------------------------------------------------
    If rx_pkt is outstanding from previous time, use it. Otherwise, dequeue
    a new packet
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->rx_pkt)
  {
    rx_pkt                  = dpm_rm_info_ptr->rx_pkt;
    dpm_rm_info_ptr->rx_pkt = NULL;

    LOG_MSG_DATA_PATH_INFO_1("ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr(): "
                             "Using outstanding pkt, sig %d", rx_sig );
  }
  else
  {
    rx_pkt = dsm_dequeue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    if (NULL == rx_pkt)
    {
      return TRUE;
    }
  }

  pkts_array[num_pkts] = rx_pkt;
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {      
    /*-----------------------------------------------------------------------
      Peek in to the first byte to determine version of the IP packet and
      process the packet appropriately
    -----------------------------------------------------------------------*/
    (void) dsm_extract(rx_pkt, 
                       PS_DPM_OPT_QOS_HEADER_OFFSET + PS_DPM_OPT_ETH_HEADER_OFFSET, 
                       &ip_hdr_first_byte, 1);

    version = ip_hdr_first_byte & PS_DPM_OPT_IP_VERSION_MASK;
    
    /*-----------------------------------------------------------------------
      For CLAT, Protocols doesnt support converting ethernet packets, so
      DPM takes legacy path and let protocols take care of CLAT
    -----------------------------------------------------------------------*/
    if (version == PS_DPM_OPT_V4_VERSION) 
    {
      rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      break;                                     
    }
    else if (version == PS_DPM_OPT_V6_VERSION)
    {
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv6 packets
      -----------------------------------------------------------------------*/
      if (NULL == v6_dpm_um_info_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr(): "
                        "v6 call not UP for sig %d", rx_sig);
        path_type = PS_DPM_UL_OPT_NO_PATH;                        
        break;
      }

      rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      
      /*-----------------------------------------------------------------------
        Do appropriate processing for IPv4 and IPv6 packets
      -----------------------------------------------------------------------*/
      um_iface_ptr = dpm_rm_info_ptr->v6_logical_iface_ptr;
      /*-------------------------------------------------------------------
          Check CR 692508
      -------------------------------------------------------------------*/       
      if (NULL == um_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr(): "
                        "Call in down or going down state um iface 0x%p "
                        "rm iface 0x%p ", um_iface_ptr);                        
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;      
      }       

      companion_um_iface_ptr = (NULL == v4_dpm_um_info_ptr)
                                 ? NULL
                                 : dpm_rm_info_ptr->v4_logical_iface_ptr;
                                 
      /*---------------------------------------------------------------------
        Extract IP header and port information from the IP packet
      ---------------------------------------------------------------------*/
      buf_len = dsm_extract(rx_pkt,
                            PS_DPM_OPT_QOS_HEADER_OFFSET + PS_DPM_OPT_ETH_HEADER_OFFSET,
                            pkt_buf,
                            PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT);

      if ((PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_TCP_HDR_LEN_EXTRACT) > buf_len)
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Get the offset of protocol field, IP header length, and also the
        packet's total length
      ---------------------------------------------------------------------*/
      total_len =
        ps_htons(*((uint16 *)(pkt_buf + PS_DPM_OPT_V6_TOTAL_LEN_OFFSET)));
      total_len  += PS_DPM_OPT_V6_HDR_LEN;
      ip_hdr_len  = PS_DPM_OPT_V6_HDR_LEN;
      prot_offset = PS_DPM_OPT_V6_NXT_HDR_OFFSET;


      /*-----------------------------------------------------------------------
        Check if packet needs to be sent up the legacy path
      -----------------------------------------------------------------------*/
      if (TRUE == ps_dpm_ul_opt_ipv6_is_legacy(pkt_buf))
      {
        path_type = PS_DPM_UL_OPT_LEGACY_PATH;
        break;
      }

      /*---------------------------------------------------------------------
        Drop the packet if the packet's source prefix doesn't match Um's prefix
      ---------------------------------------------------------------------*/
      if (FALSE == ps_dpm_ul_opt_ipv6_prefix_cmp(pkt_buf, um_iface_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr(): "
                        "IPV6 prefix cmp failed");      
        path_type = PS_DPM_UL_OPT_NO_PATH;
        break;
      }

      ip_version = IP_V6;
    }
    else
    {
      path_type = PS_DPM_UL_OPT_LEGACY_PATH;
      break;
    }

    /*-----------------------------------------------------------------------
      Check if Um is flow controlled and if so, do not process the packet
      further. There are two scenarios to consider.
        1. Single IP call - Um is flow controlled if corresponding Um iface
           is flow controlled
        2. Dual-IP over single RmNet call (Single APN/Dual APN) - Um is
           flow controlled only if both the corresponding v4 and v6 Um ifaces
           are flow controlled

      Detailed algorithm is
        1. Check if Um is flow controlled. If it is not, then there is
           no flow control. If it is, go to step 2
        2. Check if companion Um iface is registered with DPM. If it is not,
           then Um is flow controlled. If it is not, go to step 3
        3. Check if companion Um is flow controlled. If it is not, then there is
           no flow control. If it is, Um is flow controlled
        4. If Um is flow controlled, cache the packet as outstanding packet
           in DPM and process it later
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Step 1
    -----------------------------------------------------------------------*/
    ps_flow_ptr      = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    ps_phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

    if (!(PS_IFACEI_FLOW_ENABLED(um_iface_ptr)))
    {
      /*---------------------------------------------------------------------
        Step 2
      ---------------------------------------------------------------------*/
      if (NULL != companion_um_iface_ptr)
      {
        /*-------------------------------------------------------------------
          Step 3
        -------------------------------------------------------------------*/
        companion_ps_flow_ptr =
          PS_IFACEI_GET_DEFAULT_FLOW(companion_um_iface_ptr);
        companion_ps_phys_link_ptr =
          PS_IFACEI_GET_PHYS_LINK(companion_um_iface_ptr);

        if (!(PS_IFACEI_FLOW_ENABLED(companion_um_iface_ptr)))
        {
          /*-----------------------------------------------------------------
            Step 4
          -----------------------------------------------------------------*/
          ret_val                 = TRUE;
          path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
          dpm_rm_info_ptr->rx_pkt = rx_pkt;

          break;
        }
      }
      else
      {
        /*-------------------------------------------------------------------
          Step 4
        -------------------------------------------------------------------*/
        ret_val                 = TRUE;
        path_type               = PS_DPM_UL_OPT_FLOW_CONTROL_PATH;
        dpm_rm_info_ptr->rx_pkt = rx_pkt;

        break;
      }
    }

    /*-----------------------------------------------------------------------
      Pull out QoS hdr, Ethernet header & assign the rx_pkt back since it  
      might get modified above by dsm_pullup
    -----------------------------------------------------------------------*/
    (void) dsm_pullup(&rx_pkt, &qos_hdr, sizeof(qos_hdr));
    (void) dsm_pullup(&rx_pkt, NULL, PS_DPM_OPT_ETH_HEADER_OFFSET);
    pkts_array[num_pkts] = rx_pkt;

    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(um_iface_ptr);
    
    if (total_len > PS_IFACE_GET_MTU(um_base_iface_ptr))
    {     
      frag_result = ps_dpm_ul_opt_fragment_packet (
                                           rx_pkt,
                                           um_base_iface_ptr,
                                           PS_IFACE_GET_MTU(um_base_iface_ptr),
                                           pkts_array,
                                           version,
                                           total_len,
                                           &ds_errno);
      if (-1 == frag_result)
      {
        path_type = PS_DPM_UL_OPT_NO_PATH;
      }    
    }

  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  
  /*-------------------------------------------------------------------
    Loop through all the pkts in array and send them one by one
  -------------------------------------------------------------------*/    
  while (num_pkts < PS_DPM_UL_OPT_MAX_FRAG_PACKETS && 
           NULL != pkts_array[num_pkts])
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
   
    if (path_type != PS_DPM_UL_OPT_OPT_PATH)
    {
      goto send_data;
    }
    
    rx_pkt = pkts_array[num_pkts]; 

    /*-----------------------------------------------------------------------
      Increment packet stats on Rm iface
    -----------------------------------------------------------------------*/
    rm_iface_ptr->iface_i_stats.pkts_rx++;
    rm_iface_ptr->iface_i_stats.bytes_rx += total_len;

    /*-----------------------------------------------------------------------
      Log the packet on the Rm iface
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(rm_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    PS_TX_META_INFO_DUP(tx_meta_info_ptr, &dup_tx_meta_info_ptr);

    ps_dpm_process_qos_hdr(rx_pkt,
                           um_base_iface_ptr,
                           ip_version,
                           TRUE,
                           dup_tx_meta_info_ptr,
                           (void*)&qos_hdr,
                           PS_DPM_QOS_FORMAT_6_BYTE,
                           &ps_flow_ptr);

    /*-----------------------------------------------------------------------
      Increment packet stats on Um iface
    -----------------------------------------------------------------------*/
    um_base_iface_ptr->iface_i_stats.pkts_tx++;
    um_base_iface_ptr->iface_i_stats.bytes_tx += total_len;

    /*-----------------------------------------------------------------------
      Log packet on the Um iface
    -----------------------------------------------------------------------*/
    PS_DPM_GLOBAL_STATS_INC(um_base_iface_ptr);
    DPL_LOG_NETWORK_TX_PACKET(um_base_iface_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    /*-----------------------------------------------------------------------
      If flow capability is set to PS_FLOW_CAPABILITY_DATA_DISALLOWED,
      drop the packet
    -----------------------------------------------------------------------*/
    if (PS_FLOW_GET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED))
    {
      LOG_MSG_INFO2_1("ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr(): "
                      "ps_flow 0x%p is PS_FLOW_CAPABILITY_DATA_DISALLOWED",
                      ps_flow_ptr);
      path_type = PS_DPM_UL_OPT_NO_PATH;
      goto send_data;
    }

    /*---------------------------------------------------------------------
        TCP ACK prioritization
    ---------------------------------------------------------------------*/
    if (ps_dpm_ul_opt_tcp_ack_pri_check
          (
            pkt_buf,
            ip_hdr_len,
            total_len,
            prot_offset
          ) == TRUE
       )
    {
      LOG_MSG_INFO3_0("IP_NO_QOS: TCP_ACK_PRIORITY: Tagging DSM_HIGHEST");
      (rx_pkt)->priority = DSM_HIGHEST;
    }     

    /*-----------------------------------------------------------------------
      Log packet on ps_flow and increment phys link stats
    -----------------------------------------------------------------------*/
    DPL_LOG_FLOW_TX_PACKET(ps_flow_ptr, rx_pkt, DPL_IID_NETPROT_IP);

    if (NULL != ps_phys_link_ptr)
    {
      ps_phys_link_ptr->phys_link_i_stats.pkts_tx++;
    }

    /*-----------------------------------------------------------------------
      NOTE: COULD HAVE THIS CODE OUTSIDE THE DO-WHILE LOOP, BUT DOING IT
      THIS WAY SAVES ONE IF CHECK
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Make sure that the power save mode is disabled after recieving the first 
      uplink packet on this path
    ---------------------------------------------------------------------*/
    if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE( rm_iface_ptr ) )
    {
      if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE( rm_iface_ptr ) )
      {
        ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, &ps_errno );
      }
    }

send_data :

    /*-------------------------------------------------------------------------
      Leave CS before tx_cmd to avoid possible deadlock issues
    -------------------------------------------------------------------------*/
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    /*-------------------------------------------------------------------------
      Process the packet based on the 'path_type'
    -------------------------------------------------------------------------*/
    switch (path_type)
    {
      case PS_DPM_UL_OPT_OPT_PATH:
      {
        um_base_iface_ptr->iface_private.tx_cmd
        (
          um_base_iface_ptr,
          &rx_pkt,
          dup_tx_meta_info_ptr,
          um_base_iface_ptr->iface_private.tx_cmd_info
        );
        break;
      }

      case PS_DPM_UL_OPT_LEGACY_PATH:
      {
        ret_val = ps_dpm_ul_legacy_input(rm_iface_ptr, rx_pkt, dpm_rm_info_ptr);
        break;
      }

      case PS_DPM_UL_OPT_FLOW_CONTROL_PATH:
      {
        LOG_MSG_INFO1_1("ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr(): "
                        "Um is flow controlled, sig %d", rx_sig);
        break;
      }

      case PS_DPM_UL_OPT_NO_PATH:
      default:
      {
        /*---------------------------------------------------------------------
          Check if rx packet needs to be free'd or not
        ---------------------------------------------------------------------*/
        if (DS_EINVAL != ds_errno)
        {
          dsm_free_packet(&rx_pkt);
        }
        break;
      }
    }
    
    pkts_array[num_pkts] = NULL;
    num_pkts++;
  }

  return ret_val;
} /* ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr() */
#endif /* FEATURE_DATA_PS_464XLAT */
