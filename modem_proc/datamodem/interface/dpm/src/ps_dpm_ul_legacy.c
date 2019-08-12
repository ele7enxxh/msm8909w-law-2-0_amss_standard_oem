/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ D P M _ L E G A C Y . C

GENERAL DESCRIPTION
  This is the implementation of  legacy path Data Path Manager module

Copyright (c) 2013-15 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_ul_legacy.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/04/15    gk     Removed un-necessary F3s from data path
09/10/13    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "ps_dpmi.h"
#include "ps_iface.h"
#include "ps_lan_llc.h"
#include "ps_handle_mgr.h"
#include "ds_qmi_svc.h"
#include "ps_dpm_opt_defs.h"
#include "ds3gutil.h"

dsm_watermark_type     ps_dpm_ul_legacy_wm;
q_type                 ps_dpm_ul_legacy_wm_q;

extern ps_crit_sect_type         ps_dpm_crit_section;

static ps_meta_info_type_ex    * meta_info_ex_ptr;
static ps_tx_meta_info_type    * tx_meta_info_ptr;

static boolean legacy_wm_flow_enabled = TRUE;

#define   PS_DPM_UL_LEGACY_WM_LOW   15000
#define   PS_DPM_UL_LEGACY_WM_HIGH  45000
#define   PS_DPM_UL_LEGACY_WM_DNE   800000

#define PS_DPM_VALID_IFACE_STATE_FOR_DATA_PATH                           \
          ~(IFACE_STATE_INVALID | IFACE_DISABLED | IFACE_DOWN)

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief  This function checks whether the packet is ICMPv6 ECHO REQUEST/RESPONSE and 
             start low latency status timer mechanism

  @param[in] pkt_buf        : pointer to the v6 pkt buffer
  @param[in] ip_hdr_len   : IP header length
  @param[in] prot_offset   : Protocol field offset in IP header
  @param[in] rx_pkt         : packet reference pointer
  
  @return None

  @code
  @endcode
*/
INLINE void ps_dpm_ul_legacy_v6_low_latency_traffic_handler
(
  uint8 * const          pkt_buf,
  uint8                  ip_hdr_len,
  uint8                  prot_offset,
  dsm_item_type        * rx_pkt
)
{
  uint8 icmp6_hdr[PS_DPM_OPT_ICMP6_HDR_LEN_EXTRACT + 1] ALIGN(4);
/*---------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------
  call low latency timer handler in below scenarios
  1. If the pkt is ICMP6 ECHO REQ/RESP 
  2. If the pkt is fragmented, check whether next_header  is ICMPv6 and 
     fragment offset is 0
  
  In 2nd scenario we need to extract more bytes to check if it is ECHO REQ/RESP
  pkt as ICMPv6 header is not extracted yet.
  
  protocol offset and ip header length includes any headers present before IP
  header depending on the path it has come from.
  -----------------------------------------------------------------------------*/
  if( ( pkt_buf != NULL) && (*(pkt_buf + prot_offset) == PS_IPPROTO_ICMP6 ) &&
      (( (*(pkt_buf + ip_hdr_len + PS_DPM_OPT_ICMP6_TYPE_OFFSET) == 
            ICMP6_ECHO_REQ) ||
         (*(pkt_buf + ip_hdr_len + PS_DPM_OPT_ICMP6_TYPE_OFFSET) == 
            ICMP6_ECHO_RSP)) &&
         (*(pkt_buf + ip_hdr_len + PS_DPM_OPT_ICMP6_CODE_OFFSET) == 0)) )
  {
    ps_iface_low_latency_timer_start();
  }
  else if ( (pkt_buf != NULL) && 
            (*(pkt_buf + prot_offset) == PS_IPPROTO_FRAG_HDR ) &&
            (*(pkt_buf + ip_hdr_len + PS_DPM_OPT_FRAG_NEXT_HDR_OFFSET) == 
               PS_IPPROTO_ICMP6) &&
            !(*(uint16 *)(pkt_buf + ip_hdr_len + PS_DPM_OPT_FRAG_FRAG_OFFSET ) & 
                PS_DPM_OPT_V6_FRAG_OFFSET_MASK) )
  {
    if( PS_DPM_OPT_ICMP6_HDR_LEN_EXTRACT != 
          dsm_extract( rx_pkt, ip_hdr_len + PS_DPM_OPT_V6_OPT_HDR_LEN, 
                       icmp6_hdr, 
                       PS_DPM_OPT_ICMP6_HDR_LEN_EXTRACT ) )
    {
      return;
    }

    if( (((*(icmp6_hdr + PS_DPM_OPT_ICMP6_TYPE_OFFSET) == ICMP6_ECHO_REQ) ||
            (*(icmp6_hdr + PS_DPM_OPT_ICMP6_TYPE_OFFSET) == ICMP6_ECHO_RSP)) &&
            (*(icmp6_hdr + PS_DPM_OPT_ICMP6_CODE_OFFSET) == 0)) )
    {
      ps_iface_low_latency_timer_start();
    }
  }
  else
  {
    return;
  }
}/*ps_dpm_ul_legacy_v6_low_latency_traffic_handler()*/

static void ps_dpm_ul_legacyi_non_empty_data_cback
(
 dsm_watermark_type  * wm,
 void                * cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set Rx Signal
  -------------------------------------------------------------------------*/
  PS_SET_SIGNAL(PS_RX_RMNET1_SIGNAL);

} /* ps_dpm_ul_legacyi_non_empty_data_cback() */


static void ps_dpm_ul_legacyi_wm_low_cb_func
(
  struct dsm_watermark_type_s * legacy_wm_ptr,
  void                        * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
LOG_MSG_DATA_PATH_INFO_0 ("In ps_dpm_ul_legacyi_wm_low_cb_func");

  legacy_wm_flow_enabled = TRUE;
  
  /*-------------------------------------------------------------------------
    Once legacy wm goes from flow disabled to flow enabled state, it has to 
    flow enable all the wm on DPM RM. In ps_dpm_rm_enable_all_flows, loop is 
    ran through all DPM RMs to enable flow on each of them by setting the
    DPM UL signal
  -------------------------------------------------------------------------*/
  ps_dpm_rm_enable_all_flows();

}/*ps_dpm_ul_legacyi_wm_low_cb_func()*/

static void ps_dpm_ul_legacyi_wm_high_cb_func
(
  struct dsm_watermark_type_s * legacy_wm_ptr,
  void                        * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_DATA_PATH_INFO_0 ("In ps_dpm_ul_legacyi_wm_high_cb_func");

  legacy_wm_flow_enabled = FALSE;

}/*ps_dpm_ul_legacyi_wm_high_cb_func()*/

boolean ps_dpm_ul_legacyi_sig_hdlr
(
  ps_sig_enum_type     sig,
  void               * user_data
)
{
  ps_meta_info_type_ex        *  dup_meta_info_ex_ptr = NULL;
  dsm_item_type               *  legacy_rx_pkt;
  ps_iface_type               *  rm_iface_ptr = NULL;
  ps_iface_type               *  um_iface_ptr = NULL;
  ps_dpm_rm_info_type         *  dpm_rm_info_ptr = NULL;
  uint16                         bytes_read;
  ps_dpm_qos_hdr_type            qos_hdr;
  ps_dpm_qos2_hdr_type           qos2_hdr;
  ip_version_enum_type           ip_ver;
  lan_lle_enum_type              lle_instance;
  rmnet_sm_link_prot_e_type      link_prot;
  uint8                          data_format;
  ps_dpm_qos_format              qos_hdr_format;
  boolean                        enable      = FALSE;
  sint15                         ps_errno;
  uint8                          pkt_buf[PS_OPT_PATH_IP_NO_QOS_HEADER_LEN + 1] ALIGN(4);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  legacy_rx_pkt = dsm_dequeue(&ps_dpm_ul_legacy_wm);
  if (NULL == legacy_rx_pkt)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Validate Rm iface and get it's IP family
  -------------------------------------------------------------------------*/
  rm_iface_ptr = (ps_iface_type *) (legacy_rx_pkt->app_ptr);
  if (!PS_IFACE_IS_VALID(rm_iface_ptr))
  {
    LOG_MSG_ERROR_0("ps_dpm_ul_legacyi_sig_hdlr(): Invalid Rm iface");
    dsm_free_packet(&legacy_rx_pkt);
    return FALSE;
  }

  ip_ver =
    (IPV4_ADDR == ps_iface_get_addr_family(rm_iface_ptr)) ? IP_V4 : IP_V6;

  /*-------------------------------------------------------------------------
    Fetch DPM RM using app_field of DSM item and cache the needed info in
    local variables
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  dpm_rm_info_ptr = ps_dpm_get_dpm_rm_info_by_handle(legacy_rx_pkt->app_field);
  if (NULL == dpm_rm_info_ptr)
  {
    dsm_free_packet(&legacy_rx_pkt);
    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
    return FALSE;
  }

  link_prot        = dpm_rm_info_ptr->link_prot;
  data_format      = dpm_rm_info_ptr->data_format;
  qos_hdr_format   = dpm_rm_info_ptr->qos_header_format;

  lle_instance = (IP_V4 == ip_ver) ? dpm_rm_info_ptr->v4_lan_llc_instance
                                   : dpm_rm_info_ptr->v6_lan_llc_instance;

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

  /*-------------------------------------------------------------------------
    Extract QoS header from DSM item if QoS is negotiated as data format
  -------------------------------------------------------------------------*/
  memset((void *) &qos_hdr, 0, sizeof(ps_dpm_qos_hdr_type));

  if (PS_DPM_QOS_ENABLED == data_format)
  {
    if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
    {
      bytes_read = dsm_pullup(&legacy_rx_pkt, (byte *) &qos2_hdr, sizeof(qos2_hdr));
      if (sizeof(qos2_hdr) != bytes_read)
      {
        dsm_free_packet(&legacy_rx_pkt);
        return FALSE;
      }
    }
    else
    {
      bytes_read = dsm_pullup(&legacy_rx_pkt, (byte *) &qos_hdr, sizeof(qos_hdr));
      if (sizeof(qos_hdr) != bytes_read)
      {
        dsm_free_packet(&legacy_rx_pkt);
        return FALSE;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Update meta info if Um iface is not in DOWN state. Else, just send the
    packet up the stack to Rm iface and let the framework process the packet
  -------------------------------------------------------------------------*/
  um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(rm_iface_ptr);

  if (PS_DPM_VALID_IFACE_STATE_FOR_DATA_PATH & ps_iface_state(um_iface_ptr))
  {
    PS_TX_META_RESET_FILTER_RESULT(tx_meta_info_ptr, IP_FLTR_CLIENT_QOS_OUTPUT);
    meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
    PS_META_INFO_DUP(meta_info_ex_ptr, &dup_meta_info_ex_ptr);

    if (PS_DPM_QOS_ENABLED == data_format)
    {
      if (PS_DPM_QOS_FORMAT_8_BYTE == qos_hdr_format)
      {
        ps_dpm_process_qos_hdr(legacy_rx_pkt,
                               um_iface_ptr,
                               ip_ver,
                               FALSE,
                               dup_meta_info_ex_ptr->tx_meta_info_ptr,
                               (void *)&qos2_hdr,
                               qos_hdr_format,
                               NULL);
      }
      else
      {
        ps_dpm_process_qos_hdr(legacy_rx_pkt,
                               um_iface_ptr,
                               ip_ver,
                               FALSE,
                               dup_meta_info_ex_ptr->tx_meta_info_ptr,
                               (void *)&qos_hdr,
                               PS_DPM_QOS_FORMAT_6_BYTE,
                               NULL);
      }
    }
  }

/*-------------------------------------------------------------------
   Execute powersave filters on incoming packet in legacy path. 
-------------------------------------------------------------------*/
  if( TRUE == PS_IFACE_GET_POWERSAVE_FILTERING_MODE(rm_iface_ptr) )
  {
    if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE(rm_iface_ptr) )
    {
      ps_iface_set_powersave_filtering_mode( rm_iface_ptr, enable, 
                                            &ps_errno );
    }
  }
          
  if (RMNET_IP_MODE == link_prot)
  {
    /*-------------------------------------------------------------------------
        We will be checking and sending the low latency indication only when 
        NV73594 is enabled
    -------------------------------------------------------------------------*/
    if( ds3g_get_config_latency_info())
    {
      switch( ip_ver )
      {
        case IP_V4:
        {
          if( PS_DPM_UL_OPT_V4_IP_ICMP_LEN_CHECK == 
                dsm_extract( 
                        legacy_rx_pkt,
                        0,
                        pkt_buf,
                        PS_DPM_OPT_V4_HDR_LEN + PS_DPM_OPT_ICMP_HDR_LEN_EXTRACT) )
          {
            ps_dpm_ul_v4_low_latency_traffic_handler ( 
                                                pkt_buf,
                                                PS_DPM_OPT_V4_HDR_LEN,
                                                PS_DPM_OPT_V4_PROT_FIELD_OFFSET );
          }
          break;
        }
        case IP_V6:
        {
          if( PS_DPM_UL_OPT_V6_IP_ICMP6_LEN_CHECK == 
                dsm_extract( 
                       legacy_rx_pkt,
                       0,
                       pkt_buf,
                       PS_DPM_OPT_V6_HDR_LEN + PS_DPM_OPT_ICMP6_HDR_LEN_EXTRACT) )
          {
            ps_dpm_ul_legacy_v6_low_latency_traffic_handler(
                                                    pkt_buf,
                                                    PS_DPM_OPT_V6_HDR_LEN,
                                                    PS_DPM_OPT_V6_NXT_HDR_OFFSET,
                                                    legacy_rx_pkt );
          } 
        
          break;
        }
      
        default:
          break;
      }
    }
    
    (void) ps_iface_input(rm_iface_ptr, &legacy_rx_pkt, dup_meta_info_ex_ptr);
  }
  else
  {
    (void) lan_llc_rx_pkt(lle_instance, legacy_rx_pkt, dup_meta_info_ex_ptr);
  }

  return FALSE;
} /* ps_dpm_ul_legacyi_sig_hdlr() */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void ps_dpm_ul_legacy_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initiatlize the WM
  -------------------------------------------------------------------------*/
  dsm_queue_init(&ps_dpm_ul_legacy_wm,
                 PS_DPM_UL_LEGACY_WM_DNE,
                 &ps_dpm_ul_legacy_wm_q);

  dsm_set_low_wm(&ps_dpm_ul_legacy_wm, PS_DPM_UL_LEGACY_WM_LOW);
  dsm_set_hi_wm(&ps_dpm_ul_legacy_wm, PS_DPM_UL_LEGACY_WM_HIGH);
  dsm_set_dne(&ps_dpm_ul_legacy_wm, PS_DPM_UL_LEGACY_WM_DNE);

  ps_dpm_ul_legacy_wm.non_empty_func_data = NULL;
  ps_dpm_ul_legacy_wm.non_empty_func_ptr  =
    ps_dpm_ul_legacyi_non_empty_data_cback;

  ps_dpm_ul_legacy_wm.lowater_func_data = NULL;
  ps_dpm_ul_legacy_wm.lowater_func_ptr  =  
    ps_dpm_ul_legacyi_wm_low_cb_func;
    
  ps_dpm_ul_legacy_wm.hiwater_func_data = NULL;
  ps_dpm_ul_legacy_wm.hiwater_func_ptr  =  
    ps_dpm_ul_legacyi_wm_high_cb_func;
    
  /*-------------------------------------------------------------------------
    Initialize signal used for legacy path
  -------------------------------------------------------------------------*/
  (void) ps_set_sig_handler(PS_RX_RMNET1_SIGNAL,
                            ps_dpm_ul_legacyi_sig_hdlr,
                            NULL);
  ps_enable_sig(PS_RX_RMNET1_SIGNAL);

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each legacy pkt
  -------------------------------------------------------------------------*/
  PS_META_INFO_GET_EX(meta_info_ex_ptr);
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);
  if (NULL == meta_info_ex_ptr || NULL == tx_meta_info_ptr)
  {
    DATA_ERR_FATAL("ps_dpm_ul_legacy_init(): Couldn't alloc meta info");
    return;
  }

  meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
  return;

} /* ps_dpm_ul_legacy_init() */


boolean ps_dpm_ul_legacy_input
(
  ps_iface_type        * rm_iface_ptr,
  dsm_item_type        * rx_pkt,
  ps_dpm_rm_info_type  * dpm_rm_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If legacy watermark is not flow enabled, then return true
  -------------------------------------------------------------------------*/
  if (FALSE == legacy_wm_flow_enabled)
  {
    dpm_rm_info_ptr->rx_pkt = rx_pkt;
    return TRUE;
  }
  
  /*-------------------------------------------------------------------------
    Enqueue packet in to legacy WM. These packets will handled by the
    legacy sig hdlr in PS task
  -------------------------------------------------------------------------*/
  rx_pkt->app_field = dpm_rm_info_ptr->handle;
  rx_pkt->app_ptr   = rm_iface_ptr;
  dsm_enqueue(&ps_dpm_ul_legacy_wm, &rx_pkt);

  return FALSE;
} /* ps_dpm_ul_legacy_input() */