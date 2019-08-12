/*===========================================================================
                                 PS_ARP.C
                       

DESCRIPTION

    This file contains implementation of the PS ARP public interface. This
    interface should only be accessible by PS entities.

EXTERNALIZED FUNCTIONS
    arp_init()
    arp_start()
    arp_stop()
    arp_input()
    arp_ipv4_tx_pkt()
    arp_ipv4_cache_update()
    arp_flush()
    arp_probe()
    arp_start_bcast_garp()

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_arp.c#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/01/13   rk      Removed feature mobileap.
06/27/13   cx      Replace memcpy and memmove with memscpy and memsmove
05/07/13   cx      Fix compiler errors  
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
10/14/11   ss      Added check in arp_input to prevent reply to gratuitous 
                   arp packet.
09/20/11   cp      Added the support for Broadcasting GARP in periodic
                   intervals. 
05/17/11   cp      Fix to mark the existing ARP entry as internal
                   entry when it is updated through DHCP module.
01/10/11   cp      Added a new API to retrieve the static connected devs info.
10/19/10   sa      Made changes for graceful exit in case of dsm exhaustion.
10/15/10   pp      Removed ASSERT if deleting ARP entry fails.
10/07/10   am      Serializing cache update IOCTLs for SoftAp in PS_RM task.
10/01/10   sa      Added appropriate log messages before ASSERT(0).
07/07/10   cp      Fix for ignoring the probe packet sent by us is routed
                   back to us [in case the MAC layer is buggy].
07/02/10   cp      DHCP Soft AP changes - defined separate interfaces to update
                   and clear the entries from the ARP cache.
09/24/09   ss      KW warning fixes.
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
09/12/08   pp      Metainfo optimizations.
02/06/07   scb     Fixed Klockwork Critical and High errors
11/16/06   ks      High Lint error fixes.
06/30/06   lyr     Added probe_ex() API and IP conflict handling
06/26/06   scb     Added statistics support for the ARP module 
06/05/06   ks      fixed compile error seen for 7500
02/21/06   lyr     Added QoS support (freeing meta info where necessary)
11/08/05   hba     Used ARP_PS_IFACE_IP_V4_ADDR in lieu of PS_IFACE_IP_V4_ADDR
09/09/05   hba     Added Support for Diag EVENT_ARP
09/09/05   jd      Force ARP entries via arp_ipv4_cache_update into cache 
07/27/05   jd      Made ARP flush command task indepedent
04/26/05   jd/hba  F3's print IP addresses in host order
03/31/05   hba     Remove network/host byte order conversion.
03/27/05   hba/aku Fixed the hardware address check when processing ARP 
                   responses.
03/15/05   hba/jd  Temporarily convert input ip addr from network to host
                   order to account for ipv4_route change
10/07/04   hba     Creation.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "ps_lan_llc.h"
#include "ps_arp.h"
#include "ps_arpi.h"
#include "ps_stat_arp.h"
#include "ps_stat_logging.h"

#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_lan_event.h"
#include "event.h"
#include "event_defs.h"

#include "ps_tx_meta_info.h"
#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define ARP_REPORT_EVENT( _event_payload, _status, _ip_addr, _ptr_mac_addr) \
        do                                                                  \
        {                                                                   \
          _event_payload.status     = (uint8)_status ;                             \
          _event_payload.ip_addr    = _ip_addr;                             \
          memscpy( _event_payload.mac_addr,LAN_IEEE_MAC_ADDR_LEN,            \
                  _ptr_mac_addr,                                            \
                  LAN_IEEE_MAC_ADDR_LEN );                                  \
          event_report_payload( EVENT_ARP,                                  \
                                sizeof(_event_payload),                     \
                                &_event_payload);                           \
        } while (0)
        

static int arpi_flush
(
  lan_lle_enum_type  lle_instance
);

static void arpi_process_flush_cmd
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

static void arpi_bcast_garp_timeout_cback
(
  void* user_data
);

static void arpi_start_bcast_garp
(
  lan_lle_enum_type  lle_instance
);

static void arpi_process_bcast_garp_cmd
(
  ps_cmd_enum_type  cmd,
  void *            user_data_ptr
);

static int arpi_stop_bcast_garp
(
  lan_lle_enum_type  lle_instance
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION ARP_INIT()

DESCRIPTION
  This function initializes the ARP module main data structures. It should be
  called only one, at power up, typically when PS LLC is beeing initialized.  

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arp_init
(
  void 
)
{
  static boolean init_flag = FALSE;
  uint8          loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( TRUE == init_flag)
  {
    DATA_ERR_FATAL("PS ARP Module already initialized");
    return;
  }

  /*------------------------------------------------------------------------
   Initialization of all LLE control blocks 
  -------------------------------------------------------------------------*/
  memset( arp_array, 0, LAN_LLE_MAX * sizeof(arpi_lle_cb_type));

  for ( loop= 0; loop < LAN_LLE_MAX; ++loop)
  {
    /*-----------------------------------------------------------------------
     Set the arp_instance to an invalid one. This will indicate later on 
     validity of ARP instance control block.
    -----------------------------------------------------------------------*/
    arp_array[loop].arp_instance = LAN_LLE_MIN;
  }

  /*-------------------------------------------------------------------------
   Initialize ARP Cache Manager
  -------------------------------------------------------------------------*/
  arpi_cmgr_init();

  /*-------------------------------------------------------------------------
    Register command handlers with serving task (PS)
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PS_ARP_FLUSH_CMD, arpi_process_flush_cmd);
  (void)ps_set_cmd_handler(PS_START_BCAST_GARP, 
                           arpi_process_bcast_garp_cmd);

  init_flag = TRUE;

  LOG_MSG_INFO1_0("ARP Module Initialized");

}  /* arp_init() */



/*===========================================================================
FUNCTION ARP_START()

DESCRIPTION
  This function starts an ARP instance for a particular Logical Link Entity.

PARAMETERS
  lle_instance: the LLE instance that should be started
  arp_config_ptr: pointer to an ARP configuration buffer.
  iface_ptr: pointer to ps_iface associated with this LLE.
  llc_tx_f_ptr: pointer to a generic LLC transmission function.

RETURN VALUE
  -1 in case of error
   0 if ARP instance has been started successfully.

DEPENDENCIES
  arp_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_start
(
  lan_lle_enum_type              lle_instance,
  arp_config_type*                  arp_config_ptr,
  ps_iface_type*                    iface_ptr,
  lan_llc_send_pkt_f_ptr_type    llc_tx_f_ptr
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
  ps_timer_handle_type   timer_res;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(lle_instance >= LAN_LLE_MAX || lle_instance <= LAN_LLE_MIN)
  {
    DATA_ERR_FATAL("arp_start(): Invalid LLE ");
    return -1;
  }

  ASSERT(NULL != llc_tx_f_ptr);
  ASSERT(NULL != arp_config_ptr);

  /*-------------------------------------------------------------------------
  Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  if ( ptr_arpi_cb->arp_instance == lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance already started for LLE %d", lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set up ARP configuration info
  -------------------------------------------------------------------------*/
  ptr_arpi_cb->arp_instance      = lle_instance;
  ptr_arpi_cb->iface_ptr         = iface_ptr;
  ptr_arpi_cb->proxy_query_f_ptr = arp_config_ptr->arp_proxy_query_f_ptr;
  ptr_arpi_cb->llc_tx_f_ptr = llc_tx_f_ptr;
  ptr_arpi_cb->garp_bcast_interval = 0;
  ptr_arpi_cb->flags = 0;

  if ( TRUE == arp_config_ptr->enable_proxy )
  {
    ASSERT(NULL != ptr_arpi_cb->proxy_query_f_ptr);
    ptr_arpi_cb->flags |= ARP_PROXY_ENABLE;
  }

  if ( TRUE == arp_config_ptr->enable_cache_maintenance)
  {
    ptr_arpi_cb->flags |= ARP_CACHE_REFRESH_ENABLE;
  }

  if ( TRUE == arp_config_ptr->enable_bcast_gratuitous_arp)
  {
    ptr_arpi_cb->flags |= ARP_BCAST_GARP_ENABLE;
    ptr_arpi_cb->garp_bcast_interval = ( arp_config_ptr->bcast_gratuitous_arp_interval != 0 ) ?
               arp_config_ptr->bcast_gratuitous_arp_interval * 1000: DEF_GARP_BCAST_INTERVAL;
      /*---------------------------------------------------------------------
        Allocate a new PS Timer
      ---------------------------------------------------------------------*/
    timer_res = ps_timer_alloc(arpi_bcast_garp_timeout_cback,
                               (void*) lle_instance);

    if (PS_TIMER_INVALID_HANDLE == timer_res)
    {
      DATA_ERR_FATAL("Failed to allocate PS timer for Gratuitous ARP");
      return -1;
    }

    ptr_arpi_cb->garp_timer_handle = timer_res;

  }

  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_ARP,
                                   ptr_arpi_cb->arp_instance,
                                   PS_STAT_INST_CREATED);

  LOG_MSG_INFO1_1("ARP instance %d started!", lle_instance);

  return 0;

}  /* arp_start() */


/*===========================================================================
FUNCTION ARP_STOP()

DESCRIPTION
  This function stops an ARP instance associated to a particular LLE. All 
  entries in the ARP cache related to this LLE are flushed and any  
  subsequent ARP resolution originating for this LLE will fail.

PARAMETERS
  lle_instance: the LLE instance to stop.

RETURN VALUE
  -1 in case of error
   0 in case of success

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_stop
(
  lan_lle_enum_type  lle_instance
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(lle_instance >= LAN_LLE_MAX || lle_instance <= LAN_LLE_MIN)
  {
    DATA_ERR_FATAL("arp_stop(): Invalid LLE ");
    return -1;
  }

  /*-------------------------------------------------------------------------
   Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    DATA_ERR_FATAL("Cannot stop ARP instance");
    return -1;
  }

  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_ARP,
                                   ptr_arpi_cb->arp_instance,
                                   PS_STAT_INST_DELETED);

  /*-------------------------------------------------------------------------
   Flush all entry in cache associated with this LLE
  -------------------------------------------------------------------------*/
  if ( arpi_flush(lle_instance) < 0)
  {
    DATA_ERR_FATAL("Error flushing ARP cache associated with LLE instane");
    return -1;
  }

  /* If the broadcasting of gratuitous arp is started, stop it. */
  if ( ( ptr_arpi_cb->flags & ARP_BCAST_GARP_ENABLE ) && 
       ( ptr_arpi_cb->garp_bcast_interval > 0 ) )
  {
    /*-------------------------------------------------------------------------
     Stop b'casting gratuitous arp on the interface associated with this LLE
    -------------------------------------------------------------------------*/
    if ( arpi_stop_bcast_garp(lle_instance) < 0)
    {
      DATA_ERR_FATAL("Error stopping bcast gratuitous ARP cache associated with LLE instane");
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
   Reset this ARP instance control block 
  -------------------------------------------------------------------------*/
  memset(ptr_arpi_cb, 0, sizeof(arpi_lle_cb_type) );

  ptr_arpi_cb->arp_instance = LAN_LLE_MIN;

  return 0;

} /* arp_stop() */


/*===========================================================================
FUNCTION ARP_IPV4_TX_PKT()

DESCRIPTION
  This function is called to request transmission of an IP packet needing 
  either destination hardware address resolution. The caller provides a 
  pointer to dsm_item_type containing the packet which should be transmitted 
  once resolution is successfull. It is up to the ARP module to effectively 
  transmit, queue or trash the packet based on the outcome of the ARP 
  resolution. This function should only be called for unicast packets.


PARAMETERS
  lle_instance: the LLE instance to stop.
  dsp_ip_addr: destination Ip address.
  ip_pkt: refers to dsm_item_type containing chain of IP Packet.

RETURN VALUE
   -1 if error
   0 in case request is taken into account.

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_ipv4_tx_pkt
(
  lan_lle_enum_type lle_instance,
  uint32               dst_ip_addr,
  dsm_item_type**      ip_pkt
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
  arpi_entry_type*       ptr_arpi_entry;
  int                    ret;
  arp_event_payload_type event_payload;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(lle_instance >= LAN_LLE_MAX || lle_instance <= LAN_LLE_MIN)
  {
    DATA_ERR_FATAL("arp_ipv4_tx_pkt(): Invalid LLE ");
    return -1;
  }

  /*-------------------------------------------------------------------------
   Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];


  /*-------------------------------------------------------------------------
    Check if ARP instance has been started
  -------------------------------------------------------------------------*/ 
  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance %d was not started: Cannot Tx Unicast Pkt",
                    lle_instance);
    ASSERT(0);
    return -1;
  }

  ARP_INC_INSTANCE_STATS(lle_instance, ipv4_req_tx, 1); 
  /*-------------------------------------------------------------------------
    Lookup ARP entry for this IP address
  -------------------------------------------------------------------------*/
  ptr_arpi_entry = arpi_cmgr_lookup(lle_instance, dst_ip_addr);

  if ( NULL != ptr_arpi_entry)
  {

    LOG_MSG_INFO3_1("Found ARP entry in cache for IP addr 0x%x",
                    ps_ntohl(dst_ip_addr));

    /*-----------------------------------------------------------------------
     Check if ARP resolution is done! If we're waiting for an ARP response,
     then we've enqueued( queue sixe is on for now) the packet that has 
     triggered the ARP procedure. This packet is freed and we replace it by
     the newer packet for which this function was invoked.
    -----------------------------------------------------------------------*/
    if ( !(ptr_arpi_entry->status & ARP_STATUS_RES_DONE))
    {
      ps_tx_meta_info_type** tmp_ps_mi_ptr;

      LOG_MSG_INFO2_1("ARP Resolution pending for IP 0x%x also "
	  "Freeing current pending packet waiting for ARP", ps_ntohl(dst_ip_addr));
      
      if(NULL == ptr_arpi_entry->pending_pkt || 
         NULL == ptr_arpi_entry->pending_pkt->app_ptr)
      {

        DATA_ERR_FATAL("arp_ipv4_tx_pkt(): ptr_arpi_entry->pending_pkt or ptr_arpi_entry->pending_pkt->app_ptr is null");
        return -1;
      }

      tmp_ps_mi_ptr = 
        ( (ps_tx_meta_info_type **) (&(ptr_arpi_entry->pending_pkt->app_ptr)) );

      PS_TX_META_INFO_FREE( tmp_ps_mi_ptr );
      dsm_free_packet(&ptr_arpi_entry->pending_pkt);
      ARP_INC_INSTANCE_STATS(lle_instance, ipv4_drop_new_req_tx, 1);  
    }
    else
    {
      ARP_INC_INSTANCE_STATS(lle_instance, cache_hits, 1);  
      ret = ptr_arpi_cb->llc_tx_f_ptr(lle_instance,
                                      ip_pkt,
                                      LAN_ETHERTYPE_IPV4,
                                      ptr_arpi_entry->hw_addr);

      /*--------------------------------------------------------------------- 
       Timestamp entry
      ---------------------------------------------------------------------*/        
      ptr_arpi_entry->used_timestamp = arpi_get_sys_time();

      return ret;
    }
  }
  else
  {
    LOG_MSG_INFO2_1("No ARP entry found in cache for IP addr: 0x%x",
                   ps_ntohl(dst_ip_addr));
    ARP_INC_INSTANCE_STATS(lle_instance, cache_misses, 1);  
    /*-----------------------------------------------------------------------
      Request New entry in cache
    -----------------------------------------------------------------------*/
    ptr_arpi_entry = arpi_cmgr_new_entry(TRUE);

    if ( NULL == ptr_arpi_entry)
    {
      LOG_MSG_ERROR_1("Cannot perform ARP resolution for IP addr 0x%x",
                      ps_ntohl(dst_ip_addr));
      ASSERT(0);
      return -1;
    }
    else
    {
      ptr_arpi_entry->arp_instance = lle_instance;
      ptr_arpi_entry->ip_addr      = dst_ip_addr;
      ptr_arpi_entry->status      |= ARP_STATUS_RES_PEND;

      /*---------------------------------------------------------------------
        Request ARP resolution
      ---------------------------------------------------------------------*/
      if ( arpi_ipv4_resolve(ptr_arpi_entry, 
                             (byte*)LAN_IEEE_MAC_BROADCAST_ADDR) < 0)
      {
        LOG_MSG_ERROR_1("Failed to start ARP resolution for IP addr %x",
                        ps_ntohl(dst_ip_addr));

        /*-------------------------------------------------------------------
         Release newly created entry
        -------------------------------------------------------------------*/
        arpi_cmgr_release_entry(ptr_arpi_entry);

        ARP_INC_INSTANCE_STATS(lle_instance, req_bcast_fail_tx, 1); 
        return -1;
      }

      /*-------------------------------------------------------------------
          Report ARP Request here ...
      -------------------------------------------------------------------*/
      ARP_INC_INSTANCE_STATS(lle_instance, pkts_tx, 1); 
      ARP_INC_INSTANCE_STATS(lle_instance, req_bcast_tx, 1);  
      event_payload.event_type = ARP_REQUEST_OUTGOING_EVENT;
        
      ARP_REPORT_EVENT( event_payload,
                        ptr_arpi_entry->status,
                        ptr_arpi_entry->ip_addr,
                        ptr_arpi_entry->hw_addr);

    }// endif (NULL == ptr_arpi_entry)

  }// endif (NULL!= ptr_arpi_entry)

  /*-------------------------------------------------------------------------
    If we arrive here we're waiting for an ARP response. Save current 
    packet. It'll get transmitted when we receive response.
  -------------------------------------------------------------------------*/
  ptr_arpi_entry->pending_pkt = *ip_pkt;

  return 0;

} /* arpi_ipv4_tx_pkt() */




/*===========================================================================
FUNCTION ARP_INPUT()

DESCRIPTION
  Process an incomming ARP frame.  

PARAMETERS
  lle_instance: The LLE instance that received the ARP packet.
  pkt: pointer to chain of DSM ITEM containing ARP packet
  src_hw_addr: pointer Source hardware address
  dst_hw_addr: pointer destination hardware address

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arp_input
(
  lan_lle_enum_type lle_instance,
  dsm_item_type **     pkt,          /* Rx ARP frame                       */
  byte*                src_hw_addr,
  byte*                dst_hw_addr
)
{
  arpi_lle_cb_type*   ptr_arpi_cb;
  arpi_frm_type_enum  frm_type;
  arpi_frm_body_type  frm_body;
  arpi_entry_type*    ptr_arp_entry;
  dsm_item_type*      rsp_pkt;
  dsm_item_type*      pend_pkt;
  ps_iface_type*      iface_ptr;
  uint32              our_ip_addr;
  boolean             should_respond;
  boolean             should_proxy;
  arp_event_payload_type event_payload;
  uint8*              iface_hw_addr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (lle_instance <= LAN_LLE_MIN) || (lle_instance >= LAN_LLE_MAX) )
  {
     LOG_MSG_ERROR_1("arp_input(): lle_instance %d is out of bounds",
                     lle_instance);
     dsm_free_packet(pkt);
     ASSERT(0);
     return;
  }

  /*-------------------------------------------------------------------------
    Retrieve ARP instance control block
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  iface_ptr = ptr_arpi_cb->iface_ptr;

  our_ip_addr    = ARP_PS_IFACE_IP_V4_ADDR(ptr_arpi_cb->iface_ptr);

  iface_hw_addr  = PS_IFACE_HW_ADDR_PTR( iface_ptr );

  if(NULL == iface_hw_addr)
  {
     LOG_MSG_ERROR_0("arp_input(): Interface Hardware Address is NULL");
     dsm_free_packet(pkt);
     return;
  }

  /*-------------------------------------------------------------------------
   Decode the ARP frame and invoke the proper action based on the frame type
  -------------------------------------------------------------------------*/
  frm_type = arpi_decode(pkt, &frm_body);

  ARP_INC_INSTANCE_STATS(lle_instance, pkts_rx, 1); 
  switch (frm_type)
  {
  
    case ARP_UNKNOWN_FRM:
    {
      LOG_MSG_INFO1_0("Received Unknown ARP frame, dropping it");

      /*---------------------------------------------------------------------
       break and move to instruction destroying input ARP packet  
      ---------------------------------------------------------------------*/
      ARP_INC_INSTANCE_STATS(lle_instance, unknown_pkts_rx, 1); 
      break;
    }  // End case ARP_UNKNOWN_FRAME

    case ARP_REQUEST_FRM:
    {
      /*---------------------------------------------------------------------
        If this frame is sent by an entity using our IP address,this is an IP
        conflict:
        Notify any client registered for this; note that ARP *should not*
        take any action - it is left to the client to deal with a conflict
      ---------------------------------------------------------------------*/

      if((0 != our_ip_addr) &&
         (frm_body.sender_ip_addr == our_ip_addr) &&
              (memcmp((void*)frm_body.sender_hw_addr,
                  iface_hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN)) &&
          (NULL != ptr_arpi_cb->conflict_cback) )
      {
        ptr_arpi_cb->conflict_cback( ptr_arpi_cb->conflict_cback_user_data );
      }

      /*---------------------------------------------------------------------
       According to RFC 826 do the following:
         - Perform a merge if sender IP is already in cache 
         - Otherwise insert it in cache.
         - If we're target of ARP request then respond
      ---------------------------------------------------------------------*/
      ARP_INC_INSTANCE_STATS(lle_instance, req_rx, 1);  
      ptr_arp_entry = arpi_cmgr_lookup(lle_instance,
                                       frm_body.sender_ip_addr);

      if ( NULL != ptr_arp_entry )
      {
        /*-------------------------------------------------------------------
          Little optimization:
            Instead of comparing the MAC addresses before copying it, copy it 
            in any case thus saving an extra memcmp instruction.
            But we only do so if resolution has been alread performed 
            successfully.
        -------------------------------------------------------------------*/
        if ( ptr_arp_entry->status & ARP_STATUS_RES_DONE )
        {
          LOG_MSG_INFO1_1("Updating ARP entry(IP: 0x%x) with incomming ARP Request",
                          ps_ntohl(ptr_arp_entry->ip_addr));

          ARP_INC_INSTANCE_STATS(lle_instance, arp_req_cache_hits, 1);  
          ARP_INC_INSTANCE_STATS(lle_instance, ce_update_arp_req, 1); 
          memscpy(ptr_arp_entry->hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN,
                  (void*)frm_body.sender_hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN); 

          ptr_arp_entry->update_timestamp = arpi_get_sys_time();
        }
        else if ( ptr_arp_entry->status & ARP_STATUS_PROBING)
        {
          if( (TRUE == ptr_arp_entry->not_confident) &&
             /* Ignore if it is sent by us [in case the MAC layer is buggy] */
             (memcmp((void*)frm_body.sender_hw_addr,
                      iface_hw_addr,
                      LAN_IEEE_MAC_ADDR_LEN)) )
          {
            /*---------------------------------------------------------------
             This is an ARP announce from someone:
             Let's not make any assumptions about the IP address being used
             by the entity sending this request: clear the entry from the 
             cache and resolve it later if needed.
            ---------------------------------------------------------------*/
            ASSERT(NULL != ptr_arp_entry->arp_cback);
  
            ptr_arp_entry->arp_cback(ptr_arp_entry->arp_cback_data,
                                     ARP_PROBE_SUCCESS);
  
            arpi_cmgr_release_entry(ptr_arp_entry);
          }
        }
      }
      else if( 0 == frm_body.sender_ip_addr )
      {
        /*-------------------------------------------------------------------
          We received an ARP request from another entity attempting to probe
          the same IP address that we are probing

          Since we are not confident, we give up using this IP address
          
          Perform a check to see if we are probing for the target addr &
          if so see if this needs to be treated as a probe success...
        -------------------------------------------------------------------*/
        ptr_arp_entry = arpi_cmgr_lookup(lle_instance,
                                         frm_body.target_ip_addr);

        if( NULL != ptr_arp_entry )
        {
          if( ptr_arp_entry->status & ARP_STATUS_PROBING )
          {
            if( (TRUE == ptr_arp_entry->not_confident) &&
                (memcmp((void*)frm_body.sender_hw_addr,
                        iface_hw_addr,
                        LAN_IEEE_MAC_ADDR_LEN)) )
            {
              /*-------------------------------------------------------------
               This is an ARP probe from someone on the network
               We found an ARP entry for the target IP address
              -------------------------------------------------------------*/
              ASSERT(NULL != ptr_arp_entry->arp_cback);
              
              ptr_arp_entry->arp_cback(ptr_arp_entry->arp_cback_data,
                                       ARP_PROBE_SUCCESS);
              
              arpi_cmgr_release_entry(ptr_arp_entry);
            }
          }

          ptr_arp_entry = NULL;

        } // else ignore the ARP probe from another entity

      }
      else
      {
        /*-------------------------------------------------------------------
          High probability that we will be talking pretty soon to this node. 
          Merge it in cache! 
        -------------------------------------------------------------------*/
        ARP_INC_INSTANCE_STATS(lle_instance, arp_req_cache_misses, 1);  
        ptr_arp_entry = arpi_cmgr_new_entry(FALSE);

        if ( NULL != ptr_arp_entry)
        {
          /*-----------------------------------------------------------------
            Update ARP entry
          -----------------------------------------------------------------*/ 
          ptr_arp_entry->arp_instance = lle_instance;
          ptr_arp_entry->ip_addr      = frm_body.sender_ip_addr;
          ptr_arp_entry->status      |= ARP_STATUS_RES_DONE;

          memscpy(ptr_arp_entry->hw_addr,
                 LAN_IEEE_MAC_ADDR_LEN,
                 (void*)frm_body.sender_hw_addr,
                 LAN_IEEE_MAC_ADDR_LEN); 


          if ( ptr_arpi_cb->flags & ARP_CACHE_REFRESH_ENABLE)
          {
            /*---------------------------------------------------------------
              Start Monitoring this entry
            ---------------------------------------------------------------*/ 
            LOG_MSG_INFO2_1("Start monitoring entry of IP 0x%x",
                            ptr_arp_entry->ip_addr);

            arpi_cmgr_monitor(ptr_arp_entry);

          } //else do nothing 

        } // else ptr_arp_entry = NULL so do nothing... 

      }  // endif outer NULL != ptr_arp_entry

      /*---------------------------------------------------------------------
        Report ARP request event here
      ---------------------------------------------------------------------*/
      if ( NULL != ptr_arp_entry)
      {
        event_payload.event_type = ARP_REQUEST_INCOMING_EVENT;

        ARP_REPORT_EVENT(event_payload,
                         ptr_arp_entry->status,
                         ptr_arp_entry->ip_addr,
                         ptr_arp_entry->hw_addr);
      }

      /*---------------------------------------------------------------------
       Now check if we're target of request and respond appropriately
      ---------------------------------------------------------------------*/

      our_ip_addr    =   ARP_PS_IFACE_IP_V4_ADDR(ptr_arpi_cb->iface_ptr);

      should_proxy   =  (ptr_arpi_cb->proxy_query_f_ptr == NULL) ?
                        FALSE : TRUE;

      /*---------------------------------------------------------------------
        Respond if the target ip is our ip, do not respond to gratuitous arp
        i.e. if the target ip is same as sender ip.
      ---------------------------------------------------------------------*/
      should_respond =  ( frm_body.target_ip_addr == our_ip_addr ) ? TRUE :
                        ( should_proxy == FALSE || 
                          frm_body.target_ip_addr == 
                            frm_body.sender_ip_addr) ? FALSE :
                        ptr_arpi_cb->proxy_query_f_ptr(
                                                      lle_instance,
                                                      frm_body.sender_ip_addr,
                                                      frm_body.target_ip_addr); 

      if ( TRUE == should_respond )
      {
        /*-------------------------------------------------------------------
         We should respond to this request...
        -------------------------------------------------------------------*/

        rsp_pkt = arpi_build_response(iface_hw_addr,
                                      frm_body.target_ip_addr,
                                      (byte*)frm_body.sender_hw_addr,
                                      frm_body.sender_ip_addr);

        if (NULL == rsp_pkt)
        {
          LOG_MSG_ERROR_0("Could not build ARP Response packet");
          break;
        }


        /*-------------------------------------------------------------------
         Transmit paket using LLC service
        -------------------------------------------------------------------*/
        LOG_MSG_INFO2_0("Transmitting ARP response");

        rsp_pkt->app_ptr = NULL;

        if (ptr_arpi_cb->llc_tx_f_ptr(lle_instance,
                                      &rsp_pkt,
                                      LAN_ETHERTYPE_ARP,
                                      (byte*)frm_body.sender_hw_addr) < 0)
        {
          DATA_ERR_FATAL("Fail to transmit ARP response packet");
          dsm_free_packet(&rsp_pkt);
          ARP_INC_INSTANCE_STATS(lle_instance, resp_fail_tx, 1);  
        }

        ARP_INC_INSTANCE_STATS(lle_instance, pkts_tx, 1); 
        ARP_INC_INSTANCE_STATS(lle_instance, resp_tx, 1); 
        event_payload.event_type = ARP_RESPONSE_OUTGOING_EVENT;

        ARP_REPORT_EVENT(event_payload,
                         ARP_STATUS_RES_DONE,
                         frm_body.sender_ip_addr,
                         (byte*)frm_body.sender_hw_addr);
      }

      /*---------------------------------------------------------------------
        Break and move to instruction destroying input ARP packet!
      ---------------------------------------------------------------------*/
      break;

    }// End case ARP_REQ_FRM

    case ARP_RESPONSE_FRM:
    {
      /*---------------------------------------------------------------------
        If this is not an unicast packet and is not directed to us, beware  a 
        potential man in the middle attack!!! Check to make sure that the 
        destination hardware address in the ethernet body is the same as the
        target hardware address in the ARP header.
      ---------------------------------------------------------------------*/
      ARP_INC_INSTANCE_STATS(lle_instance, resp_rx, 1); 
      if ( (memcmp((void*)dst_hw_addr,
                    iface_hw_addr,
                    LAN_IEEE_MAC_ADDR_LEN)) ||
           (memcmp((void*)frm_body.target_hw_addr,
                    iface_hw_addr,
                    LAN_IEEE_MAC_ADDR_LEN))
         )
      {
        LOG_MSG_INFO2_0("ARP Response not directed to us");

        ARP_INC_INSTANCE_STATS(lle_instance, not_for_us_rx, 1); 
        /*-------------------------------------------------------------------
         Break and move to instruction destroying input ARP packet
        -------------------------------------------------------------------*/
        break;
      }

      /*---------------------------------------------------------------------
        If this unicast packet is sent by an entity using our IP address,
        this is an IP conflict:
        Notify any client registered for this; note that ARP *should not*
        take any action - it is left to the client to deal with a conflict
        
        Our current policy is to ensure the reply is directed towards us
        (hw addr check above); but we could be more aggressive and do IP
        check before hw addr check
        
        Also, the extra check on 0 allows us to avoid responses which are
        incorrect (response should always have sender IP addr):
        the order of if checks minimizes the checks during normal operation
      ---------------------------------------------------------------------*/
      if( (frm_body.sender_ip_addr == our_ip_addr) &&
          (0 != our_ip_addr) &&
          (memcmp((void*)frm_body.sender_hw_addr,
                  iface_hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN)) &&
          (NULL != ptr_arpi_cb->conflict_cback) )
      {
        ptr_arpi_cb->conflict_cback( ptr_arpi_cb->conflict_cback_user_data );
      }

      /*---------------------------------------------------------------------
       We're the  target of this ARP response...
       It should correspond to a pending Request
      ---------------------------------------------------------------------*/
      ptr_arp_entry = arpi_cmgr_lookup(lle_instance,
                                       frm_body.sender_ip_addr);

      if ( NULL != ptr_arp_entry )
      {

        /*-------------------------------------------------------------------
          Report ARP Response here ...
        -------------------------------------------------------------------*/
        event_payload.event_type = ARP_RESPONSE_INCOMING_EVENT;
        

        ARP_REPORT_EVENT(event_payload,
                         ptr_arp_entry->status,
                         frm_body.sender_ip_addr,
                         (byte*)frm_body.sender_hw_addr);
        
        /*-------------------------------------------------------------------
         Stop ARP timer if still running.
        -------------------------------------------------------------------*/
        arpi_stop_timer(ptr_arp_entry->timer_handle);

        if ( ptr_arp_entry->status & ARP_STATUS_RES_PEND)
        {
          memscpy(ptr_arp_entry->hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN,
                  (void*)frm_body.sender_hw_addr,
                  LAN_IEEE_MAC_ADDR_LEN);

          /*-----------------------------------------------------------------
           Clear resolution pending flag
          -----------------------------------------------------------------*/
          ptr_arp_entry->status &= (~ARP_STATUS_RES_PEND);
          ptr_arp_entry->status |= ARP_STATUS_RES_DONE;

          pend_pkt = ptr_arp_entry->pending_pkt;

          /*-----------------------------------------------------------------
            There should be a pending packet!
          -----------------------------------------------------------------*/ 
          ASSERT(NULL != pend_pkt);

          /*-----------------------------------------------------------------
            Transmit packet using LLC service
          -----------------------------------------------------------------*/ 
          LOG_MSG_INFO2_1("Transmitting IP Packet(IP: 0x%x) after successful ARP",
                          ptr_arp_entry->ip_addr);

          if (ptr_arpi_cb->llc_tx_f_ptr(lle_instance,
                                        &pend_pkt,
                                        LAN_ETHERTYPE_IPV4,
                                        ptr_arp_entry->hw_addr) < 0)
          {
            ps_tx_meta_info_type** tmp_ps_mi_ptr;

            DATA_ERR_FATAL("Fail to transmit pkt after successful ARP resolution");
      
            ASSERT(NULL != pend_pkt->app_ptr);

            tmp_ps_mi_ptr = ( (ps_tx_meta_info_type**) (&(pend_pkt->app_ptr)) );
            
            PS_TX_META_INFO_FREE( tmp_ps_mi_ptr );
            dsm_free_packet(&pend_pkt);
          }


          ptr_arp_entry->used_timestamp  = arpi_get_sys_time();

          if ( ptr_arpi_cb->flags & ARP_CACHE_REFRESH_ENABLE)
          {
            /*---------------------------------------------------------------
             Start Monitoring this entry
            ---------------------------------------------------------------*/
            LOG_MSG_INFO2_1("Start monitoring entry of IP 0x%x",
                            ptr_arp_entry->ip_addr);

            arpi_cmgr_monitor(ptr_arp_entry);
          }

          ARP_INC_INSTANCE_STATS(lle_instance, succ_resp_pending_rx, 1); 
        }  //endif status & ARP_STATUS_RES_PEND

        else if ( ptr_arp_entry->status & ARP_STATUS_PROBING)
        {
          /*-----------------------------------------------------------------
           This is a reponse to a broadcast probe, invoke the probe callback 
           and deactivate this entry.
          -----------------------------------------------------------------*/
          ASSERT(NULL != ptr_arp_entry->arp_cback);

          ptr_arp_entry->arp_cback(ptr_arp_entry->arp_cback_data,
                                   ARP_PROBE_SUCCESS);

          arpi_cmgr_release_entry(ptr_arp_entry);
          ARP_INC_INSTANCE_STATS(lle_instance, ce_evict_prb_succ, 1); 
        }

        else if ( ptr_arp_entry->status & ARP_STATUS_CHECKING)
        {
          /*-----------------------------------------------------------------
            Notify the result to cache manager
          -----------------------------------------------------------------*/ 
          LOG_MSG_INFO2_0("Receive response to unicast probe");

          arpi_cmgr_refresh_result(ptr_arp_entry,
                                   (byte*)frm_body.sender_hw_addr);
          ARP_INC_INSTANCE_STATS(lle_instance, ce_update_check_succ, 1);  
        }

      }// End NULL!= ptr_arp_entry 
      else
      {
        LOG_MSG_INFO2_0("ARP Response directly to us with no request pending");
        ARP_INC_INSTANCE_STATS(lle_instance, no_req_pending_rx, 1); 
        LOG_MSG_INFO2_2("ARP Response info: sender IP(0x%x), target IP(0x%x)",
                        ps_ntohl(frm_body.sender_ip_addr),
                        ps_ntohl(frm_body.target_ip_addr));

      }

      /*---------------------------------------------------------------------
        Break and move to instruction destroying input ARP packet
      ---------------------------------------------------------------------*/ 
      break;

    }//End case ARP_RESPONSE_FRM 

  }  // End switch

  /*-------------------------------------------------------------------------
   If arrive here, we have to free ARP packet
  -------------------------------------------------------------------------*/
  dsm_free_packet(pkt);

} /* arp_input() */

/*===========================================================================
FUNCTION ARP_START_BCAST_GARP()
===========================================================================*/
/**
  This function is called to start broadcasting gratuitous arp's on the
  interface related to a particular LLE. This function will be called by
  the WLAN module.

  @params
  lle_instance[in] the LLE instance on which grautious arp's are broadcasted.

  @return
  0  - on Success
  -1 - on Failure

  @dependencies
  arp_init should habe been called.

@sideeffects
  None.
*/
int arp_start_bcast_garp
(
  lan_lle_enum_type lle_instance
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the specified arp instance exists
  -------------------------------------------------------------------------*/ 
  if( (lle_instance <= LAN_LLE_MIN) || (lle_instance >= LAN_LLE_MAX) )
  {
    LOG_MSG_ERROR_1("Attempted to flush cache of invalid ARP instance %d!",
              lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
  Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  /*-------------------------------------------------------------------------
    Check if the target ARP instance has been started
  -------------------------------------------------------------------------*/
  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance %d was not started", lle_instance);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Dispatch command to PS RM to start broadcasting gratuitous arp
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_START_BCAST_GARP, (void*) lle_instance);

  return 0;
}

/*===========================================================================
FUNCTION ARP_FLUSH()

DESCRIPTION
  This function will dispatch a command to PS to flush all ARP entries
  related to a particular LLE.

  The caller may be the ARP module itself or Mode Controller when it is 
  determined that the ARP cache needs to be flushed.  The cache will be
  flushed in the PS task context.

PARAMETERS
  lle_instance: the LLE instance to flush.
 
RETURN VALUE
 None.

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_flush
(
  lan_lle_enum_type lle_instance
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if the specified arp instance exists
  -------------------------------------------------------------------------*/ 
  if( (lle_instance <= LAN_LLE_MIN) || (lle_instance >= LAN_LLE_MAX) )
  {
    LOG_MSG_ERROR_1("Attempted to flush cache of invalid ARP instance %d!",
              lle_instance);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
  Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  /*-------------------------------------------------------------------------
    Check if the target ARP instance has been started
  -------------------------------------------------------------------------*/
  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance %d was not started", lle_instance);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Dispatch command to PS to flush the specified cache
  -------------------------------------------------------------------------*/
  ps_send_cmd(PS_ARP_FLUSH_CMD, (void*) lle_instance);

  return 0;

}  /* arp_flush() */



/*===========================================================================
FUNCTION ARP_IPV4_CACHE_UPDATE()

DESCRIPTION
  This function is called by a PS entity (mainly DHCP server) to set or clear
  AR entries related to a particular LLE. If there is no LLE associated with 
  this PS_IFACE, no action is taken.

PARAMETERS
    ifaceptr: PS_IFACE for which an entry is been updated.
    set_or clear: boolean indicating the action to perform:
        true: entry should be update acording to pair (hw addr, ip addr),
        false: entry should be cleared.
    ip_addr: IP address entry to clear
    hw_addr: Hardware address with which entry should be updated.

RETURN VALUE
 None.

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
void arp_ipv4_cache_update
(
  ps_iface_type*  iface_ptr,
  boolean         set_or_clear,
  uint32          ip_addr,
  byte*           hw_addr
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
  arpi_entry_type*       ptr_arpi_entry;
  uint8                  loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Recover the ARP instance associated with this PS IFACE 
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = NULL;

  for ( loop = 0; loop < LAN_LLE_MAX; ++loop)
  {
    if ( iface_ptr == arp_array[loop].iface_ptr)
    {
      ptr_arpi_cb = &arp_array[loop];
      break;
    }
  }

  if (loop == LAN_LLE_MAX)
  {
    DATA_ERR_FATAL("Cannot find ARP instance attached to PS IFACE for update");
    return;
  }

  ASSERT(ptr_arpi_cb);
  ASSERT ( (ptr_arpi_cb->arp_instance > LAN_LLE_MIN) &&
           (ptr_arpi_cb->arp_instance < LAN_LLE_MAX));

  ptr_arpi_entry = arpi_cmgr_lookup(ptr_arpi_cb->arp_instance,
                                    ip_addr);

  if ( FALSE == set_or_clear)
  {
    /*-----------------------------------------------------------------------
      Client is asking us to clear entry
    -----------------------------------------------------------------------*/

    if (NULL == ptr_arpi_entry)
    {
      /*---------------------------------------------------------------------
       Entry doesn't exist! So nothing to clear
       This is possible, if this entry got re-used; so silently return!
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("ARP entry of IP 0x%x to clear doesn't actually exist!",
                      ip_addr);
      return;
    }
    else
    {
      /*---------------------------------------------------------------------
        Prepare entry for deactivation
      ---------------------------------------------------------------------*/ 
      arpi_prep_entry_deactivation(ptr_arpi_entry);

      /*---------------------------------------------------------------------
        Deactivate entry
      ---------------------------------------------------------------------*/ 
      arpi_cmgr_release_entry(ptr_arpi_entry);
      ARP_INC_INSTANCE_STATS(loop, ce_evict_ps_ent_clear, 1); 
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
     Client is asking us for an update:
      - perform a modification if entry exist
      - perform a merge if entry doesn't exist
     ----------------------------------------------------------------------*/
    ASSERT(hw_addr);

    if ( NULL != ptr_arpi_entry)
    {
      /*---------------------------------------------------------------------
        This is a modification of the MAC address.
        We only perform update if resolution is done;
        in any other case, wait for peer response...
      ---------------------------------------------------------------------*/
      if ( ptr_arpi_entry->status & ARP_STATUS_RES_DONE)
      {
        LOG_MSG_INFO2_1("Update ARP entry of IP 0x%x", ip_addr);

        memscpy(ptr_arpi_entry->hw_addr, 
               LAN_IEEE_MAC_ADDR_LEN,
               hw_addr,
               LAN_IEEE_MAC_ADDR_LEN);
        ARP_INC_INSTANCE_STATS(loop, ce_update_ps_ent_set, 1);  
      }
      ptr_arpi_entry->internal_entry   = TRUE;
    }
    else
    {
      /*---------------------------------------------------------------------
       This is a merge, probably asked by DHCP server
       Create a new entry in cache and update it !
      ---------------------------------------------------------------------*/
      ptr_arpi_entry = arpi_cmgr_new_entry(TRUE);


      if ( NULL == ptr_arpi_entry)
      {
        /*-------------------------------------------------------------------
          We're dead here
        -------------------------------------------------------------------*/ 
        DATA_ERR_FATAL("Cannot insert new entry for ARP_IPV4_UPDATE");
      }
      else
      {
        ptr_arpi_entry->arp_instance = ptr_arpi_cb->arp_instance;
        ptr_arpi_entry->ip_addr      = ip_addr;

        memscpy(ptr_arpi_entry->hw_addr,
               LAN_IEEE_MAC_ADDR_LEN,
               hw_addr,
               LAN_IEEE_MAC_ADDR_LEN); 

        ptr_arpi_entry->update_timestamp = arpi_get_sys_time();
        ptr_arpi_entry->internal_entry   = TRUE;
        if ( ptr_arpi_cb->flags & ARP_CACHE_REFRESH_ENABLE)
        {
          /*-----------------------------------------------------------------
            Start Monitoring this entry 
          -----------------------------------------------------------------*/ 
          LOG_MSG_INFO2_1("Start monitoring entry of IP 0x%x",
                          ptr_arpi_entry->ip_addr);

          arpi_cmgr_monitor(ptr_arpi_entry);
        }

        ptr_arpi_entry->status |= ARP_STATUS_RES_DONE;

      }  // end NULL == ptr_arpi_entry

    }  // end NULL != ptr_arpi_entry

  }  // end false == set_or_clear

} /* arp_ipv4_cache_update() */

/*===========================================================================
FUNCTION ARP_PROBE()

DESCRIPTION
  This function is called to probe an IP address. It forces the generation of 
  an ARP request, which may be unicast or brodcast. If a callback is supplied,
  it is called when a response is received for the ARP request.
  If a broadcast probe is requested for an entry which is already in cache, 
  the	request is dismissed. If this happens, it is a possible sign that we've 
  started a DHCP procedure without making sure that the ARP cache is empty..

PARAMETERS
  iface_ptr: PS_IFACE for which an entry is been updated.
  probe_type: probe type
  ip_addr: IP address to probe.
  cb_ptr: callback function taking two parameters.

RETURN VALUE
  -1 if an error occurs
   0 if the probe request has been taken into account

DEPENDENCIES
  arp_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_probe
(
  ps_iface_type*      iface_ptr,
  arp_probe_enum_type probe_type,
  uint32              ip_addr,
  arp_cback_ptr_type  arp_cback_ptr,
  void*               user_data
)
{
  arp_probe_ex_params_type  probe_params;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( &probe_params,
          0,
          sizeof(arp_probe_ex_params_type) );

  /*-------------------------------------------------------------------------
    Map the legacy probe into the new probe_ex by using the earlier
    parameters
  -------------------------------------------------------------------------*/
  probe_params.ip_addr = ip_addr;

  probe_params.retry_intvl = arpi_default_req_duration;

  probe_params.num_retries = arpi_default_num_arp_retries;

  probe_params.probe_type = probe_type;

  probe_params.randomize_retry_intvl = FALSE;

  probe_params.not_confident = FALSE;

  probe_params.response_wait_time = arpi_default_req_duration;

  return arp_probe_ex( iface_ptr,
                       &probe_params,
                       arp_cback_ptr,
                       user_data);

} /* arp_probe() */



/*===========================================================================
FUNCTION ARP_PROBE_EX()

DESCRIPTION
  A configurable probe request: this function is similar in operation to 
  the basic arp_probe(); it provides greater ability to configure the
  behavior as necessary
  

PARAMETERS
  iface_ptr: PS_IFACE for which an entry is been updated.
  probe_type: probe type
  ip_addr: IP address to probe.
  cb_ptr: callback function taking two parameters.

RETURN VALUE
  -1 if an error occurs
   0 if the probe request has been taken into account


DEPENDENCIES
  arp_init should have been called.

SIDE EFFECTS
  None
===========================================================================*/
int arp_probe_ex
(
  ps_iface_type*             iface_ptr,
  arp_probe_ex_params_type*  probe_params_ptr,
  arp_cback_ptr_type         arp_cback_ptr,
  void*                      user_data
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
  arpi_entry_type*       ptr_arpi_entry;
  uint8                  loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( NULL == probe_params_ptr)
  {
    DATA_ERR_FATAL("NULL probe parameters!");
    return -1;
  }

  /*-------------------------------------------------------------------------
  Recover the ARP CB instance associated with this PS IFACE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = NULL;

  for ( loop = 0; loop < LAN_LLE_MAX; loop +=1)
  {
    if ( iface_ptr == arp_array[loop].iface_ptr)
    {
      ptr_arpi_cb = &arp_array[loop];
      break;
    }
  }

  if (loop == LAN_LLE_MAX)
  {
    DATA_ERR_FATAL("Cannot find ARP instance attached to PS IFACE!");
    return -1;
  }

  ASSERT(ptr_arpi_cb);
  ASSERT ( (ptr_arpi_cb->arp_instance > LAN_LLE_MIN) &&
           (ptr_arpi_cb->arp_instance < LAN_LLE_MAX));

  ptr_arpi_entry = arpi_cmgr_lookup(ptr_arpi_cb->arp_instance,
                                    probe_params_ptr->ip_addr);

  switch (probe_params_ptr->probe_type)
  {
    case ARP_BROADCAST_PROBE:
    {

      /*---------------------------------------------------------------------
       This is probably a request from DHCP client or link local addr mgr
         - Don't waist time if there is no callback
         - if entry exists in cache, give up probing 
       --------------------------------------------------------------------*/
      if (NULL == ptr_arpi_entry && 
          NULL != arp_cback_ptr    )
      {
        ARP_INC_INSTANCE_STATS(loop, cache_misses, 1);  
        ptr_arpi_entry = arpi_cmgr_new_entry(TRUE);

        if ( NULL != ptr_arpi_entry)
        {
          ptr_arpi_entry->arp_instance   = ptr_arpi_cb->arp_instance;
          ptr_arpi_entry->ip_addr        = probe_params_ptr->ip_addr;
          ptr_arpi_entry->arp_cback      = arp_cback_ptr;
          ptr_arpi_entry->arp_cback_data = user_data;

          ptr_arpi_entry->nb_retries     = probe_params_ptr->num_retries;
          
          ptr_arpi_entry->not_confident = 
            probe_params_ptr->not_confident;
          
          ptr_arpi_entry->randomize_retry_intvl = 
            probe_params_ptr->randomize_retry_intvl;

          ptr_arpi_entry->retry_intvl       = 
            probe_params_ptr->retry_intvl;

          ptr_arpi_entry->max_random_incr       = 
            probe_params_ptr->max_random_incr;

          ptr_arpi_entry->response_wait_time    = 
            probe_params_ptr->response_wait_time;

          ptr_arpi_entry->status |= ARP_STATUS_PROBING;


          if (arpi_ipv4_resolve(ptr_arpi_entry,
                                (byte*)LAN_IEEE_MAC_BROADCAST_ADDR) < 0)
          {
            /*---------------------------------------------------------------
              Need here to prepare entry deactivation: this is potentially
              dangerous cause DHCP client will believe this as an failure
              of the ARP Probe. But I choose to be optimistic here... 
            ---------------------------------------------------------------*/ 
            arpi_prep_entry_deactivation(ptr_arpi_entry);

            /*---------------------------------------------------------------
             Deactivate the newly created entry
            ---------------------------------------------------------------*/
            ARP_INC_INSTANCE_STATS(loop, req_bcast_prb_fail_tx, 1); 
            arpi_cmgr_release_entry(ptr_arpi_entry);
          }
          else
          {
            /*---------------------------------------------------------------
             ARP Request has been sent for probing! We're fine
            ---------------------------------------------------------------*/
            ARP_INC_INSTANCE_STATS(loop, pkts_tx, 1); 
            ARP_INC_INSTANCE_STATS(loop, req_bcast_prb_tx, 1);  
            break;
          }
        }
      }
      else if(NULL != ptr_arpi_entry)
      {
        ARP_INC_INSTANCE_STATS(loop, cache_hits, 1);

        /*-------------------------------------------------------------------
          ARP entry exists in cache: probe should succeed immediately

          Since we succeed a probe without checking status, this means that
          two probes for the same IP address will mean second probe will
          always succeed if the first probe is still pending!!

          This is defensive, but okay, since it shows intent on someone 
          else attempting to resolve the same IP address: avoid conflicts
        -------------------------------------------------------------------*/
        if( NULL != arp_cback_ptr )
        {
          arp_cback_ptr(user_data,
                        ARP_PROBE_SUCCESS);

          break;
        }
      }

      /*---------------------------------------------------------------------
       If arrive, we cannot perform the probe because:
        - either the cache manager fails to return us an ARP entry
          to start the probing procedure.
        - either no ARP CALLBBACK was supplied
      ---------------------------------------------------------------------*/
      DATA_ERR_FATAL("Error perfoming broadcast ARP probing");
      return -1;      
    }

    case ARP_UNICAST_PROBE:
    {
      /*---------------------------------------------------------------------
       This is a probing for cache consitency maintenance:
         - Entry should be in cache
         - Resolution has once been completed for this entry.
       --------------------------------------------------------------------*/
      if(NULL == ptr_arpi_entry)
      {
        DATA_ERR_FATAL("Unicast ARP probe for an entry which is not in cache!");
        return -1;
      }

      ARP_INC_INSTANCE_STATS(loop, cache_hits, 1);  

      /*---------------------------------------------------------------------
        This may be used to reconfigure the ARP checking parameters
      ---------------------------------------------------------------------*/
      ptr_arpi_entry->nb_retries            = probe_params_ptr->num_retries;

      ptr_arpi_entry->not_confident = 
        probe_params_ptr->not_confident;

      ptr_arpi_entry->randomize_retry_intvl = 
        probe_params_ptr->randomize_retry_intvl;

      ptr_arpi_entry->retry_intvl       = 
        probe_params_ptr->retry_intvl;

      ptr_arpi_entry->max_random_incr       = 
        probe_params_ptr->max_random_incr;

      ptr_arpi_entry->response_wait_time    = 
        probe_params_ptr->response_wait_time;

      if (arpi_ipv4_resolve(ptr_arpi_entry,
                            ptr_arpi_entry->hw_addr) < 0 )
      {
        LOG_MSG_ERROR_0("Error perfoming Unicast ARP probing");
        ARP_INC_INSTANCE_STATS(loop, req_ucast_prb_fail_tx, 1); 
        return -1;  
      }

      ptr_arpi_entry->status |= ARP_STATUS_CHECKING;

      ARP_INC_INSTANCE_STATS(loop, pkts_tx, 1); 
      ARP_INC_INSTANCE_STATS(loop, req_ucast_prb_tx, 1);  
      break;
    }//End case ARP_PROBE_UNICAST

  }// End switch

  return 0;

} /* arp_probe_ex() */



/*===========================================================================
FUNCTION ARP_CONFLICT_CBACK_REG()

DESCRIPTION
  This function is called by a PS entity to register a callback handler to be
  notified of an IP address conflict during normal operation of ARP
  
  This can be used e.g. by DHCP or the link local address manager to be
  notified of any conflicts to trigger recovery mechanisms

PARAMETERS
  iface_ptr: PS_IFACE for which conflict cback is registered.
  cb_ptr: callback function
  
RETURN VALUE
  -1 if an error occurs
   0 if the probe request has been taken into account

DEPENDENCIES
  arp_init should have been called.

SIDE EFFECTS
  None.
===========================================================================*/
int arp_conflict_cback_reg
(
  ps_iface_type*                iface_ptr,
  arp_conflict_cback_ptr_type   arp_conflict_cb_ptr,
  void*                         user_data
)
{
  arpi_lle_cb_type*      ptr_arpi_cb;
  uint8                  loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Recover the ARP instance associated with this PS IFACE 
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = NULL;

  for ( loop = 0; loop < LAN_LLE_MAX; ++loop)
  {
    if ( iface_ptr == arp_array[loop].iface_ptr)
    {
      ptr_arpi_cb = &arp_array[loop];
      break;
    }
  }

  if ( NULL == ptr_arpi_cb)
  {
    DATA_ERR_FATAL("Cannot find ARP instance attached to PS IFACE for update");
    return -1;
  }

  ASSERT ( (ptr_arpi_cb->arp_instance > LAN_LLE_MIN) &&
           (ptr_arpi_cb->arp_instance < LAN_LLE_MAX));

  /*-------------------------------------------------------------------------
    Check that NULL-> non-NULL and non-NULL->NULL are the only cases allowed
    for the conflict cback;
    i.e. only new registration or de-registration are allowed
  -------------------------------------------------------------------------*/
  if( ((NULL == ptr_arpi_cb->conflict_cback) &&
       (NULL == arp_conflict_cb_ptr)) ||
      ((NULL != ptr_arpi_cb->conflict_cback) &&
       (NULL != arp_conflict_cb_ptr)) )
  {
    DATA_ERR_FATAL("Invalid registration of conflict cback");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Install the conflict cback for reporting IP conflicts
  -------------------------------------------------------------------------*/
  ptr_arpi_cb->conflict_cback = arp_conflict_cb_ptr;
  ptr_arpi_cb->conflict_cback_user_data = user_data;

  return 0;
}

/*===========================================================================
FUNCTION ARP_GET_DEST_MAC_ADDR

DESCRIPTION
  This function gets the destination hw address by calling 
  arpi_cmgr_lookup().

PARAMETERS
  lan_llc_instance: the ARP instance to which the entry is associated.
  ip_addr_ptr: Pointer to IP address.
  dest_mac_addr: OUT parameter. Stores the result

RETURN VALUE
  0  - on Success
  -1 - on Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int arp_get_dest_mac_addr
(
  lan_lle_enum_type lan_llc_instance,
  ps_ip_addr_type   *ip_addr_ptr,
  uint8             *dest_mac_addr,
  uint16             dest_buf_size
)
{
  int                 ret_val = -1;
  arpi_entry_type     *arpi_entry_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ip_addr_ptr == NULL || dest_mac_addr == NULL)
  {
    LOG_MSG_ERROR_0("Invalid arguments to arp_get_dest_mac_addr");
    return ret_val;
  }

  arpi_entry_ptr = arpi_cmgr_lookup(lan_llc_instance, 
                                    ip_addr_ptr->addr.v4.ps_s_addr );
  
  if(arpi_entry_ptr == NULL) 
  {
    LOG_MSG_ERROR_0("arpi_entry_ptr is NULL. returning NULL MAC addr");
  }
  else
  {
    memscpy( dest_mac_addr,
             dest_buf_size,
             arpi_entry_ptr->hw_addr, 
             LAN_IEEE_MAC_ADDR_LEN);
    ret_val = 0;
  }
  return ret_val;
} /* arp_get_dest_mac_addr */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        LOCAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
static int arpi_flush
(
  lan_lle_enum_type  lle_instance
)
{
  arpi_lle_cb_type*  ptr_arpi_cb;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lle_instance <= LAN_LLE_MIN ||
      lle_instance >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_0("Invalid LLE");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  /*-------------------------------------------------------------------------
    Check if the target ARP instance has been started
  -------------------------------------------------------------------------*/
  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance %d stopped before flush command was processed",
                    lle_instance);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Delegate the internal entries flushing to cache manager
  -------------------------------------------------------------------------*/
  arpi_cmgr_flush(lle_instance);

  return 0;

} /* arpi_flush() */



/*===========================================================================
FUNCTION ARPI_PROCESS_FLUSH_CMD()

DESCRIPTION
  PS command handler function which will flush all ARP entries related to 
  the particular LLC instance specified in the command user data.

  This function will execute in the PS task context.

PARAMETERS
  cmd:            must be PS_ARP_FLUSH_CMD
  user_data_ptr:  the arp instance for which the arp cache must be flushed

RETURN VALUE
 None.

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
static void arpi_process_flush_cmd
(
  ps_cmd_enum_type  cmd,
  void *            user_data_ptr
)
{
  lan_lle_enum_type  lle_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  lle_instance = (lan_lle_enum_type) (uint32) user_data_ptr;

  if( lle_instance <= LAN_LLE_MIN ||
      lle_instance >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_0("Invalid LLE");
    return;
  }
  
  (void) arpi_flush( lle_instance );

} /* arpi_process_flush_cmd() */

/*===========================================================================
FUNCTION ARPI_BCAST_GARP_TIMEOUT_CBACK()

DESCRIPTION
  This function is called when the timer associated with Gratuitous ARP times
  out.

PARAMETERS
  user_data: pointer to user_data.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void arpi_bcast_garp_timeout_cback
(
  void* user_data
)
 {
  lan_lle_enum_type  lle_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  lle_instance = (lan_lle_enum_type) (uint32) user_data;
  /*-------------------------------------------------------------------------
    Make sure this a valid entry!
  -------------------------------------------------------------------------*/
  if(lle_instance >= LAN_LLE_MAX || lle_instance <= LAN_LLE_MIN)
  {
    LOG_MSG_ERROR_0("arpi_entry_timeout(): Invalid LLE");
    return ;
  }

  arpi_start_bcast_garp(lle_instance);
} /* arpi_bcast_garp_timeout_cback() */

/*===========================================================================
FUNCTION ARPI_START_BCAST_GARP()

DESCRIPTION
  This function stars broadcasting gratuitous arp
  on the interface corresponding to the particular LLC instance.

  This function will execute in the PS task context.

PARAMETERS
  lle_instance - arp_instance associated.

RETURN VALUE
  None

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
static void arpi_start_bcast_garp
(
  lan_lle_enum_type  lle_instance
)
{
  arpi_lle_cb_type*  ptr_arpi_cb;
  ps_ip_addr_type   our_ip_addr;
  dsm_item_type*    arp_pkt;
  uint32            req_duration;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  if (ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance %d is already stopped", lle_instance);
    return;
  }

  if ( !(ptr_arpi_cb->flags & ARP_BCAST_GARP_ENABLE) )
  {
    LOG_MSG_ERROR_1("Broadcasting of Gratuitous ARP is not enabled for ARP instance %d.",
                    lle_instance);
    return;
  }

  memset(&our_ip_addr,0,sizeof(ps_ip_addr_type));
  our_ip_addr.type = IPV4_ADDR;
  ps_iface_get_addr(ptr_arpi_cb->iface_ptr,&our_ip_addr);

  /*-------------------------------------------------------------------------
   Build ARP Request and send it
  -------------------------------------------------------------------------*/
  arp_pkt = arpi_build_request(
                              PS_IFACE_HW_ADDR_PTR(ptr_arpi_cb->iface_ptr),
                              our_ip_addr.addr.v4.ps_s_addr,
                              (byte*)ARP_NULL_HW_ADDR,
                              our_ip_addr.addr.v4.ps_s_addr );

  if (NULL == arp_pkt)
  {
    LOG_MSG_ERROR_0("Fail to build ARP Request");
    return;
  }

  req_duration = ptr_arpi_cb->garp_bcast_interval;

  if( (0 < req_duration) &&
      !(ps_timer_is_running(ptr_arpi_cb->garp_timer_handle)) &&
      (PS_TIMER_FAILURE == ps_timer_start(ptr_arpi_cb->garp_timer_handle,
                                          req_duration)) )
  {
    /*-----------------------------------------------------------------------
      Should not happen: lack of resource to properly perform ARP Resolution!
      We shouldn't even bother to send the Gratutious ARP request!
    -----------------------------------------------------------------------*/
    DATA_ERR_FATAL("Impossible to start Gratuitous ARP Request timer");
    dsm_free_packet(&arp_pkt);
    return;
  }

  /*-------------------------------------------------------------------------
   Prepare for ARP packet transmission
  -------------------------------------------------------------------------*/
  arp_pkt->app_ptr = NULL;
  if ( ptr_arpi_cb->llc_tx_f_ptr( ptr_arpi_cb->arp_instance,
                                  &arp_pkt,
                                  LAN_ETHERTYPE_ARP,
                                  (byte*)LAN_IEEE_MAC_BROADCAST_ADDR) < 0 )
  {
    LOG_MSG_ERROR_0("Error transmitting Gratuitous ARP frame");
    dsm_free_packet(&arp_pkt);
    return;
  }

  /*-------------------------------------------------------------------------
   We're done... Gratuitous ARP Request was sent.
  -------------------------------------------------------------------------*/
  return;
} /* arpi_start_bcast_garp() */


/*===========================================================================
FUNCTION ARPI_PROCESS_BCAST_GARP_CMD()

DESCRIPTION
  PS command handler function to start broadcasting gratuitous arp
  on the interface corresponding to the particular LLC instance specified 
  in the command user data.

  This function will execute in the PS task context.

PARAMETERS
  cmd:            must be PS_START_BCAST_GARP.
  user_data_ptr:  the arp instance.

RETURN VALUE
 None.

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
static void arpi_process_bcast_garp_cmd
(
  ps_cmd_enum_type  cmd,
  void *            user_data_ptr
)
{
  lan_lle_enum_type  lle_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cmd == PS_START_BCAST_GARP );

  lle_instance = (lan_lle_enum_type) (uint32) user_data_ptr;

  if( lle_instance <= LAN_LLE_MIN ||
      lle_instance >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_0("Invalid LLE");
    return;
  }
  
  arpi_start_bcast_garp( lle_instance );

} /* arpi_process_bcast_garp_cmd() */

/*===========================================================================
FUNCTION ARPI_STOP_BCAST_GARP()

DESCRIPTION
  This function stops broadcasting gratuitous arp
  on the interface corresponding to the particular LLC instance.

  This function will execute in the PS task context.

PARAMETERS
  lle_instance - arp_instance associated.

RETURN VALUE
  0  - on Success
  -1 - on Failure

DEPENDENCIES
  arp_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
static int arpi_stop_bcast_garp
(
  lan_lle_enum_type  lle_instance
)
{
  arpi_lle_cb_type*  ptr_arpi_cb;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lle_instance <= LAN_LLE_MIN ||
      lle_instance >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_0("Invalid LLE");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Recover the ARP instance CB corresponding to this LLE
  -------------------------------------------------------------------------*/
  ptr_arpi_cb = &arp_array[lle_instance];

  /*-------------------------------------------------------------------------
    Check if the target ARP instance has been started
  -------------------------------------------------------------------------*/
  if ( ptr_arpi_cb->arp_instance != lle_instance )
  {
    LOG_MSG_ERROR_1("ARP instance is already %d stopped.", lle_instance);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Free associated timer
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_free(ptr_arpi_cb->garp_timer_handle);
  if (PS_TIMER_SUCCESS != timer_ret_val)
  {
    LOG_MSG_ERROR_0("Releasing associated timer failed.");
    return -1;
  }

  return 0;

}/* arpi_stop_bcast_garp */

/*===========================================================================
FUNCTION ARP_GET_STATIC_CONN_DEVICES

DESCRIPTION
  This function gets the devices information which are statically configured.

PARAMETERS
  lle_instance           : the ARP instance to which the devices are 
                           associated.
  conn_devs              : Contains information about the connected devices.
  num_conn_devs          : contains the required number of connected devices 
                           information.
  num_dhcp_alloc_devices : contains the number of conn devices allocated 
                           through DHCP.

RETURN VALUE
  uint32: returned number of statically conn devices information.
  -1    : on Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int arp_get_static_conn_devices
(
  lan_lle_enum_type                 lle_instance,
  dhcp_server_conn_devices_info_s * conn_devs,
  uint32                            num_conn_devs,
  uint32                            num_dhcp_alloc_devices
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (lle_instance <= LAN_LLE_MIN) || (lle_instance >= LAN_LLE_MAX) )
  {
     LOG_MSG_ERROR_1("arp_get_static_conn_devices(): "
                     "lle_instance %d is out of bounds", lle_instance);
     return -1;
  }

  if( conn_devs == NULL || num_conn_devs == 0 ||
      ( num_dhcp_alloc_devices >= num_conn_devs ) )
  {
    LOG_MSG_ERROR_0("arp_get_static_conn_devices(): Invalid arguments");
    return -1;
  }

  return arpi_get_static_conn_devices_info(lle_instance,
                                           conn_devs,
                                           num_conn_devs,
                                           num_dhcp_alloc_devices
                                           );

} /* arp_get_static_conn_devices */

