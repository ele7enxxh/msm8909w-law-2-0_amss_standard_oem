/*===========================================================================
                                 PS_LAN_LLCI.C


DESCRIPTION

    This file contains implementation of the Internal function of PS LAN LLC
    Module internal functions.

INTERNAL FUNCTIONS

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_lan_llci.c#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   rk      Removed feature mobileap.
06/27/13   cx      Replace memcpy and memmove with memscpy and memsmove
05/08/12   ss      Fix to skip processing of zero payload ethernet frames.
02/28/11   cp      Fixed Q6 exception while printing lle_instance.
11/14/10   rt      Removed Asserts in data path.
10/20/10   pp      FEATURE_DSM_EXHAUSTION_RECOVERY block mainlined.
10/07/10   am      Rm split changes for LLC in SoftAp mode.
09/29/09   ss      Fixed KW warnings.
08/31/09   mga     Merged from eHRPD branch
08/19/09   pg      Added support for WAPI packets forwarding.
08/11/09   am      Modified locks usage. Allowing tandem start->stop->start 
                   calls in caller context.
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
09/12/08   pp      Metainfo optimizations.
07/18/08   rnp     Fixed Compiler warnings
10/01/07   mct     Added support for IPv6 RmNet.
02/07/07   scb     Fixed Klocwork high errors
11/16/06   ks      High Lint error fixes.
07/06/06   scb     Changed the lan_llci_decapsulate() function to take in a
                   lan_lle_cb_type pointer as against the mode and later derive
                   the mode from the same. Added this change to support statistics
                   for the specified function.
06/27/06   scb     Added statistics support for the 802.2 LLC module
06/05/06   ks      fixed compile error seen for 7500
03/13/06   lyr     Added workaround for populating packet chains with Qos MI
02/21/06   lyr     Added QoS support and support for 802.1Q VLAN tags
02/06/06   ks      Replaced tasklocks with critical sections.
10/27/05   ks      Fixed compile warning.
06/08/05   hba     Added support for 802.1X packets forwarding.
03/30/05   hba     Merge various fixes from integration.
03/10/05   jd      Added graceful handling of malformed input packets in
                   lan_llci_decapsulate()
01/13/05   mct     Fixed compilation error.
01/12/05   ks      Removed link_instance parameter from
                   DPL_LOG_LINK_FULL_[TX/RX]_PACKET macro calls.
01/10/05   ks      Added support for Ethernet logging using DPL.
10/07/04   hba     Creation.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "amssassert.h"
#include "task.h"
#include "rex.h"
#include "ps_lan_llci.h"
#include "ps_iface.h"
#include "dsm.h"
#include "ps_arp.h"
#include "ps_in.h"
#include "ps_tx_meta_info.h"
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "ps_rm_svc.h"

#include "ps_stat_llc.h"
#include "ps_stat_logging.h"

#include "ps_logging.h"
#include "ps_logging_defs.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_icmp6_nd.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds_Utils_DebugMsg.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  LLE control block arrays
 --------------------------------------------------------------------------*/
lan_lle_cb_type  lle_array[LAN_LLE_MAX];

/*---------------------------------------------------------------------------
   LLC/SNAP header relative definitions
---------------------------------------------------------------------------*/
#define LAN_LLC_SNAP_SAP_ADDR         0xAA
#define LAN_LLC_SIZE_OF_SNAP_SAP_ADDR 1
#define LAN_LLC_UI_CTRL_VAL           0x03
#define LAN_LLC_UI_SIZE_OF_CTL_VAL    1
#define LAN_LLC_DSAP_FIELD_IND        0
#define LAN_LLC_SSAP_FIELD_IND        1
#define LAN_LLC_UI_FIELD_IND          2
#define LAN_LLC_HDR_LEN               3

#define LAN_LLC_SNAP_OUI_FIELD_IND    3
#define LAN_LLC_SNAP_OUI_FIELD_MASK   0xFFFFFF

#define LAN_LLC_ETHERTYPE_FIELD_IND   6
#define LAN_LLC_HTON_IP_ETHERTYPE     0x0008
#define LAN_LLC_HTON_ARP_ETHERTYPE    0x0608

#define LAN_LLC_SNAP_STD_OUI          0x000000
#define LAN_LLC_SNAP_STD_OUI_TAB      {0x00, 0x00, 0x00}

#define LAN_LLC_SNAP_HDR_LEN (sizeof(lan_llc_snap_hdr))

typedef PACKED struct PACKED_POST
{
  unsigned user_pri :  3;
  unsigned cfi      :  1;
  unsigned vlan_id  : 12;
} lan_llc_vlan_tag;


/*---------------------------------------------------------------------------
  LLC/SNAP Encapsulation relative
 --------------------------------------------------------------------------*/
STATIC lan_llc_snap_hdr llc_snap_hdr =
{
  LAN_LLC_SNAP_SAP_ADDR,        /* DSAP                                    */
  LAN_LLC_SNAP_SAP_ADDR,        /* SSAP                                    */
  LAN_LLC_UI_CTRL_VAL,          /* CTRL Fielf                              */
  LAN_LLC_SNAP_STD_OUI_TAB,     /* OUI                                     */
  0x0000                           /* Ethertype to be set when framing     */
};


typedef enum
{
  LLC_UNKNOWN_FRM,
  LLC_CTRL_FRM,
  LLC_IPV4_FRM,
  LLC_IPV6_FRM,
  LLC_ARP_FRM ,
  LLC_802_1X_FRM,
  LLC_802_1X_FRM_PREAUTH,
  LLC_WAPI_FRM
} llc_frm_body_type;

typedef enum
{
  LLC_UNKNOWN_IP_FRM,
  LLC_BROADCAST_IP_FRM,
  LLC_MULTICAST_IP_FRM,
  LLC_UNICAST_IP_FRM
} ip_frm_type;

/*---------------------------------------------------------------------------
  Decapsulate an LLC PDU based on a
  specific LLC mode
---------------------------------------------------------------------------*/
STATIC llc_frm_body_type lan_llci_decapsulate
(
  dsm_item_type**        rx_pkt,
  lan_lle_enum_type      lle_instance,
  byte*                  src_hw_addr,
  byte*                  dst_hw_addr
);


/*---------------------------------------------------------------------------
  Retrieve the type of an IP frame based on the supplied meta info
---------------------------------------------------------------------------*/
STATIC ip_frm_type lan_llci_get_ip_frm_type
(
  ps_tx_meta_info_type**  meta_info_ptr
);


/*---------------------------------------------------------------------------
  LLC Control frames processing
---------------------------------------------------------------------------*/
STATIC void lan_llci_ctrl_input
(
  dsm_item_type**           rx_pkt,
  lan_llc_mode_enum_type lle_mode
);


/*---------------------------------------------------------------------------
  Retrieve an IP packet destination IP address
---------------------------------------------------------------------------*/
STATIC ps_ip_addr_type * lan_llci_get_dest_ip_addr
(
  ps_tx_meta_info_type**  meta_info_ptr
);


/*---------------------------------------------------------------------------
  Retrieve an IP packet Muticast hardware address.
  Return -1 if cannot get the address
          0 if successful
---------------------------------------------------------------------------*/
STATIC int lan_llci_get_multicast_hw_addr
(
  byte*                  hw_addr_ptr, /* Where to copy the muticast, address  */
  dsm_item_type**        ip_pkt,
  ps_tx_meta_info_type** meta_info_ptr
);

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION LAN_LLCI_START()

DESCRIPTION
  This function start LLC operation for a particular LLE. A consequence of
  starting the LLE is starting with it the associated ARP instance.

PARAMETERS
  lle_instance: The instance of the LLE to start. Note that when this
  function is invoked, the corresponding control block is already correctly
  set up!

RETURN VALUE
  None

DEPENDENCIES
  The PS LAN LLC should have been initialized.

SIDE EFFECTS
  None
===========================================================================*/
void lan_llci_start
(
  lan_lle_enum_type lle_instance
)
{
    lan_lle_cb_type*         ptr_lle_cb;
    lan_llc_start_info_type* ptr_start_info;
    uint8*                   iface_hw_addr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if( lle_instance <= LAN_LLE_MIN ||
        lle_instance >= LAN_LLE_MAX )
    {
      LOG_MSG_ERROR_1("Invalid LLE, value=%d", lle_instance);
      return;
    }
    ptr_lle_cb     = &lle_array[lle_instance];
    ptr_start_info = &ptr_lle_cb->start_info;

    /*-------------------------------------------------------------------------
      Based on the interface type initialize the appropriate Tx functions and
      any additional services needed.
    -------------------------------------------------------------------------*/
    if(ps_iface_addr_family_is_v4(ptr_start_info->iface_ptr))
    {
      /*-----------------------------------------------------------------------
       Setup ARP instance corresponding to this PS LLC instance.
       Note: It is safe to pass pointer between the LLC module and the ARP
       module cause both operate within the PS task context.
      -----------------------------------------------------------------------*/
      (void) arp_start( lle_instance,
                        &ptr_start_info->arp_config,
                        ptr_start_info->iface_ptr,
                        lan_llci_send_pkt );

      /*-----------------------------------------------------------------------
       Register IPv4 Tx fn with ps_iface.
      -----------------------------------------------------------------------*/
      ps_iface_set_tx_function( ptr_start_info->iface_ptr,
                                lan_llci_v4_tx_cback,
                                (void *) lle_instance);
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if(ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
    {
      (void) ps_icmp6_nd_start(lle_instance,
                               &ptr_start_info->nd_config,
                               ptr_start_info->iface_ptr,
                               lan_llci_send_pkt);

      /*-----------------------------------------------------------------------
       Register IPv6 Tx fn with ps_iface.
      -----------------------------------------------------------------------*/
      ps_iface_set_tx_function( ptr_start_info->iface_ptr,
                                lan_llci_v6_tx_cback,
                                (void *) lle_instance);
    }
#endif /* FEATURE_DATA_PS_IPV6 */
    else
    {
      /*-----------------------------------------------------------------------
        Register common Tx fn for PS interfaces which can tx either IPv4 or
        IPv6 packets.
      -----------------------------------------------------------------------*/
      (void) arp_start( lle_instance,
                        &ptr_start_info->arp_config,
                        ptr_start_info->iface_ptr,
                        lan_llci_send_pkt );

#ifdef FEATURE_DATA_PS_IPV6
      (void) ps_icmp6_nd_start(lle_instance,
                               &ptr_start_info->nd_config,
                               ptr_start_info->iface_ptr,
                               lan_llci_send_pkt);
#endif /* FEATURE_DATA_PS_IPV6 */

      ps_iface_set_tx_function( ptr_start_info->iface_ptr,
                                lan_llci_tx_cback,
                                (void *) lle_instance);
    }

#if 0
    if (ptr_lle_cb->softap_mode == TRUE)
    {
      (void)ps_rm_set_sig_handler(
        (ps_rm_sig_enum_type)ptr_start_info->lle_rx_sig,
        (ps_rm_sig_handler_type)lan_llci_rx_sig_hdler,
        (void *) lle_instance);

      ps_rm_enable_sig(
        (ps_rm_sig_enum_type) ptr_start_info->lle_rx_sig );
    }
    else
    {
      /*-----------------------------------------------------------------------
       Register common RX signal handler on provided signal & enable signal
      -----------------------------------------------------------------------*/
      (void) ps_set_sig_handler(
        (ps_sig_enum_type)ptr_start_info->lle_rx_sig,
        (ps_sig_handler_type)lan_llci_rx_sig_hdler,
        (void *) lle_instance);

      ps_enable_sig(
        (ps_sig_enum_type)ptr_start_info->lle_rx_sig );
    }
#endif

    /*-----------------------------------------------------------------------
     Prepare the LLC headers
    -----------------------------------------------------------------------*/
    iface_hw_addr = PS_IFACE_HW_ADDR_PTR(ptr_start_info->iface_ptr);

    if(NULL == iface_hw_addr)
    {
       DATA_ERR_FATAL("The Interface hardware address is NULL");
       return;
    }

    if (ptr_start_info->lle_mode == LAN_LLC_MODE_0)
    {
      memscpy(ptr_lle_cb->snd_pkt.zero_hdr.src_hw_addr,
             LAN_IEEE_MAC_ADDR_LEN,
             iface_hw_addr,
             LAN_IEEE_MAC_ADDR_LEN);
    }
    else if (ptr_start_info->lle_mode == LAN_LLC_MODE_1)
    {
      memscpy(ptr_lle_cb->snd_pkt.one_hdr.src_hw_addr,
             LAN_IEEE_MAC_ADDR_LEN,
             iface_hw_addr,
             LAN_IEEE_MAC_ADDR_LEN);

      memscpy(&ptr_lle_cb->snd_pkt.one_hdr.snap_hdr,
             LAN_LLC_SNAP_HDR_LEN,
             &llc_snap_hdr,
             LAN_LLC_SNAP_HDR_LEN);
    }

    PS_ENTER_CRIT_SECTION(&(ptr_lle_cb->lan_lle_crit_section));
    ptr_lle_cb->started = TRUE;
    PS_LEAVE_CRIT_SECTION(&(ptr_lle_cb->lan_lle_crit_section));

    ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_LLC,
                                     ptr_lle_cb->lle_instance,
                                     PS_STAT_INST_CREATED);

    LOG_MSG_INFO2_1("PS LLC instance %d started !", lle_instance );

} /* lan_llci_start() */

/*===========================================================================
FUNCTION LAN_LLCI_STOP()

DESCRIPTION
  This function stops LLC operation for a particular LLE. All ARP cache
  entries related to this LLE is deleted and ARP operation for this LLE will
  be stopped.

PARAMETERS
  lle_instance: the LLE instance that should be stopped.

RETURN VALUE
  0 in case of succcess
  -1 in case of error

DEPENDENCIES
  The PS LAN LLC should have been initialized.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_stop
(
  lan_lle_enum_type        lle_instance
)
{
  lan_lle_cb_type*          ptr_lle_cb;
  lan_llc_start_info_type*  ptr_start_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lle_instance <= LAN_LLE_MIN ||
      lle_instance >= LAN_LLE_MAX )
  {
    LOG_MSG_ERROR_1("Invalid LLE, value=%d", lle_instance);
    return -1;
  }
  ptr_lle_cb = &lle_array[lle_instance];
  ptr_start_info = &ptr_lle_cb->start_info;

  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_LLC,
                                   ptr_lle_cb->lle_instance,
                                   PS_STAT_INST_DELETED);


#if 0
  /*-------------------------------------------------------------------------
    Disable Rx signal
  -------------------------------------------------------------------------*/
  if (ptr_lle_cb->softap_mode == TRUE)
  {
    ps_rm_disable_sig( (ps_rm_sig_enum_type)ptr_start_info->lle_rx_sig );
  }
  else
  {
    ps_disable_sig( (ps_sig_enum_type)ptr_start_info->lle_rx_sig );
  }
#endif

  /*-------------------------------------------------------------------------
    Deregister common Tx fn with ps_iface.
  -------------------------------------------------------------------------*/
  ps_iface_unset_tx_function(ptr_start_info->iface_ptr);

  /*-------------------------------------------------------------------------
    Stop the ARP or IPv6 ND instance associated with this LLE
  -------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v4(ptr_start_info->iface_ptr))
  {
    (void) arp_stop(lle_instance);
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
  {
    (void) ps_icmp6_nd_stop(lle_instance);
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    (void) arp_stop(lle_instance);

#ifdef FEATURE_DATA_PS_IPV6
    (void) ps_icmp6_nd_stop(lle_instance);
#endif /* FEATURE_DATA_PS_IPV6 */
  }

  /*-------------------------------------------------------------------------
   Clear this LLE control block
  -------------------------------------------------------------------------*/
  memset(ptr_start_info, 0, sizeof(lan_llc_start_info_type));

  ptr_lle_cb->lle_instance = LAN_LLE_MIN;

  /*-------------------------------------------------------------------------
   Explicitly release start and stop locks in an atomic way ...
  -------------------------------------------------------------------------*/

  PS_ENTER_CRIT_SECTION(&(ptr_lle_cb->lan_lle_crit_section));

  ptr_lle_cb->started    = FALSE;

  PS_LEAVE_CRIT_SECTION(&(ptr_lle_cb->lan_lle_crit_section));

  LOG_MSG_INFO2_1("PS LLC instance %d, Stopped!", lle_instance);

  return 0;

} /* lan_llci_stop() */

/*===========================================================================
FUNCTION LAN_LLCI_RX_SIG_HDLER()

DESCRIPTION
  This function is a generic callback that PS LAN LLC regiters with PS task
  as the LLE RX signal handler upon starting the given LLE. When called. LAN
  LLC uses the RX function supplied by Mode Controller when starting up the
  LLE to retrieve the received LLC PDUs. LLC/SNAP header is removed (if
  applicable) and packets are processed and dispacthed to either IP stack or
  ARP module.

PARAMETERS
  rx_sig: the PS signal that causes the execution of this callback.
  user_data: pointer to user data passed by PS task. PS LLC uses this to
             identify the LLC instance it is dealing with.

RETURN VALUE
  Abide by the prototype of a PS Signal handler:
  Returns TRUE if signal should be cleared from the set signal mask,
          FALSE if further signal processing is needed and hence signal
                 should not be cleared

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean lan_llci_rx_sig_hdler
(
  int32            rx_sig,
  void*            user_data
)
{
  lan_lle_cb_type*         ptr_lle_cb;
  lan_lle_enum_type        lle_instance;
  lan_llc_start_info_type* ptr_start_info;
  dsm_item_type*           rx_pkt;
  byte                     dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  byte                     src_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  llc_frm_body_type        frm_type;
  ps_meta_info_type_ex  *  meta_info_ex_ptr = NULL;
  ps_phys_link_type  * ps_phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Recover LLC instance for which Rx LLC PDUs are pending.
  -------------------------------------------------------------------------*/
  lle_instance = (lan_lle_enum_type)(uint32)(user_data);

  /*-------------------------------------------------------------------------
    Make sure lle instance is not corrupted !!!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid lle_instance: %d", lle_instance);
    ASSERT(0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
   Retrieve Start info
  -------------------------------------------------------------------------*/
  ptr_lle_cb     = &lle_array[lle_instance];
  ptr_start_info = &ptr_lle_cb->start_info;

  /*-------------------------------------------------------------------------
    Proceed on only if this instance is still started...
  -------------------------------------------------------------------------*/
  if ( TRUE != ptr_lle_cb->start_lock)
  {
    if ( TRUE != ptr_lle_cb->started )
    {
      LOG_MSG_INFO2_1("Receive RX signal when LLC instance %d stopped",
                      lle_instance);
      LLC_INC_INSTANCE_STATS(lle_instance, instance_stop_rx, 1);
      return TRUE;
    }
    /*-----------------------------------------------------------------------
      Else allow signal to be processed.
    -----------------------------------------------------------------------*/
  }
  else
  {
    if ( TRUE != ptr_lle_cb->started)
    {
      LOG_MSG_INFO2_1("Receive RX signal when LLC instance %d startup is pending",
                      lle_instance);
      /*-----------------------------------------------------------------------
        Keep signal till startup is processed
      -----------------------------------------------------------------------*/
      LLC_INC_INSTANCE_STATS(lle_instance, instance_start_pending_rx, 1);
      return FALSE;
    }
  }

  /*-------------------------------------------------------------------------
   Make sure this is exactly the signal we want to handle
  -------------------------------------------------------------------------*/
  ASSERT(rx_sig == ptr_start_info->lle_rx_sig);

  /*---------------------------------------------
   Get next packet via the registered rx function
  -----------------------------------------------*/
  if ( (rx_pkt =
           ptr_start_info->rx_f_ptr(ptr_start_info->rx_f_user_data,
                                    &meta_info_ex_ptr)
           ) != NULL)
  {
      /*---------------------------------------------------------------------
        Log the received frame
      ---------------------------------------------------------------------*/
      ps_phys_link_ptr = PS_IFACE_GET_PHYS_LINK(ptr_start_info->iface_ptr);
      if(ps_phys_link_ptr != NULL)
      {
        DPL_LOG_LINK_FULL_RX_PACKET(ps_phys_link_ptr,
                                    rx_pkt,
                                    DPL_IID_LINKPROT_ETHERNET);
      }

    /*-----------------------------------------------------------------------
     Retrieve an LLC PDU Process it!
    -----------------------------------------------------------------------*/
    LLC_INC_INSTANCE_STATS(lle_instance, frms_rx, 1);
    frm_type = lan_llci_decapsulate(&rx_pkt,
                                    lle_instance,
                                    src_hw_addr,
                                    dst_hw_addr);

    /*-------------------------------------------------------------------
     Check for Zero payload ethernet frames to skip processing.
    -------------------------------------------------------------------*/
    if ( rx_pkt == NULL )
    {
      LOG_MSG_INFO1_0("Zero payload ethernet frame."
                      "Skip further processing!!");
      LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      if ( meta_info_ex_ptr != NULL )
      {
        PS_RX_META_INFO_FREE(&(meta_info_ex_ptr->rx_meta_info_ptr));
        PS_TX_META_INFO_FREE(&(meta_info_ex_ptr->tx_meta_info_ptr));
        /* Clear meta_info_ex_ptr */
        PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
      }
      return FALSE;
    }

    switch (frm_type)
    {
      case LLC_UNKNOWN_FRM:
      {
        /*-------------------------------------------------------------------
          Just drop frame for now
        -------------------------------------------------------------------*/
        LOG_MSG_INFO2_0("Receive unknown LLC frame, Frame will be dropped!");
        dsm_free_packet(&rx_pkt);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
        break;
      }

      case LLC_CTRL_FRM:
      {
        /*-------------------------------------------------------------------
          LLC Control frames
        -------------------------------------------------------------------*/
        LOG_MSG_INFO2_0("Processing LLC Control frame");
        lan_llci_ctrl_input(&rx_pkt,
                               ptr_start_info->lle_mode);
        LLC_INC_INSTANCE_STATS(lle_instance, llc_control_frms_rx, 1);
        break;
      }


      case LLC_IPV4_FRM:
      {
        /*-------------------------------------------------------------------
         Input to PS_IFACE

         Check if it makes more sense to do the check further up the stack.
        -------------------------------------------------------------------*/
        if(!ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
        {
          LOG_MSG_INFO3_0("Inputting IPv4 frame to PS IFACE");
          (void) ps_iface_input(ptr_start_info->iface_ptr,
                                &rx_pkt,
                                meta_info_ex_ptr);
          meta_info_ex_ptr = NULL; /* ps_iface_input will free this */
        }
        else
        {
          LOG_MSG_INFO2_0("Dropping mismatched IPv4 packet for interface type");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
        }
        break;
      }

      case LLC_IPV6_FRM:
#ifdef FEATURE_DATA_PS_IPV6
      {
        /*-------------------------------------------------------------------
         Input to PS_IFACE
        -------------------------------------------------------------------*/
        if(ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
        {
          LOG_MSG_INFO3_0("Inputting IPv6 frame to PS IFACE");
          (void) ps_iface_input(ptr_start_info->iface_ptr,
                                &rx_pkt,
                                meta_info_ex_ptr);
          meta_info_ex_ptr = NULL; /* ps_iface_input will free this */
        }
        else
        {
          LOG_MSG_INFO2_0("Dropping mismatch IPv6 packet for interface type");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
        }
        break;
      }
#else
      {
        LOG_MSG_INFO1_0("IPv6 not supported. Dropping packet");
        dsm_free_packet(&rx_pkt);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
        break;
      }
#endif /* FEATURE_DATA_PS_IPV6 */

      case LLC_ARP_FRM:
      {
        if(!ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
        {
        /*-------------------------------------------------------------------
          Input to ARP Module
        -------------------------------------------------------------------*/
        LOG_MSG_INFO3_0("LLC Module inputting ARP fram to ARP module");
        arp_input(lle_instance,
                  &rx_pkt,
                  src_hw_addr,
                  dst_hw_addr);
        }
        else
        {
          LOG_MSG_INFO2_0("Dropping ARP frame on non IPv4 interface");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
        }
        break;
      }

      case LLC_802_1X_FRM:
      {
        /*-------------------------------------------------------------------
          Input to 802_1X Module
        -------------------------------------------------------------------*/
        if ( NULL != ptr_lle_cb->lan_1x_input_f_ptr )
        {
          LOG_MSG_INFO3_0( "LLC Module inputting 802 1X frame to 802.1X module");
          ptr_lle_cb->lan_1x_input_f_ptr( lle_instance,
                                          &rx_pkt,
                                          src_hw_addr,
                                          dst_hw_addr);
        }
        else
        {
          LOG_MSG_ERROR_0("Dropping 802_1X packet cause no input func registered");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, llc_802_1x_unsupported, 1);
        }

        break;
      }

      case LLC_802_1X_FRM_PREAUTH:
      {
        /*-------------------------------------------------------------------
          Input to 802_1X Module
        -------------------------------------------------------------------*/
        if ( NULL != ptr_lle_cb->lan_1x_preauth_input_f_ptr )
        {
          LOG_MSG_INFO3_0( "LLC inputting 802 1X Preauth frame to LAN 1X module");
          ptr_lle_cb->lan_1x_preauth_input_f_ptr( lle_instance,
                                                  &rx_pkt,
                                                  src_hw_addr,
                                                  dst_hw_addr);
        }
        else
        {
          LOG_MSG_ERROR_0("Dropping 802 1X Preaut frame: no input func registered");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, llc_802_1x_preauth_unsupported, 1);
        }

        break;
      }

      case LLC_WAPI_FRM:
      {
        /*-------------------------------------------------------------------
          Input to WAPI Module
        -------------------------------------------------------------------*/
        if ( NULL != ptr_lle_cb->lan_wapi_input_f_ptr )
        {
          LOG_MSG_INFO3_0( "LLC Module inputting WAPI frame to WAPI module");
          ptr_lle_cb->lan_wapi_input_f_ptr( lle_instance,
                                            &rx_pkt,
                                            src_hw_addr,
                                            dst_hw_addr);
        }
        else
        {
          LOG_MSG_ERROR_0("Dropping WAPI packet: No input func registered");
          dsm_free_packet(&rx_pkt);
          LLC_INC_INSTANCE_STATS(lle_instance, llc_wapi_unsupported, 1);
        }

        break;
      }

    }// End switch frm_type

    /* Free Metainfo if allocated! */
    if(meta_info_ex_ptr != NULL)
    {
      /* TX Metainfo is allocated in rx_f_ptr, free it */
      PS_TX_META_INFO_FREE(&(meta_info_ex_ptr->tx_meta_info_ptr));
      PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
    }
    
    return FALSE;
  } // End if Rx pkt

  return TRUE;

} /* lan_llci_rx_sig_hdler () */


/*===========================================================================
FUNCTION LAN_LLCI_TX_CBACK()

DESCRIPTION
  This function is a generic callback that is registered with PS_IFACE when
  starting up a given LLE. This is the TX CMD executed by PS_IFACE when
  requesting transmission at the LLC layer.

PARAMETERS
  Abide by tx_cmd prototype of PS IFACE:
     iface_ptr:     Ptr to interface that this pkt was received on - and to
                     operate on.
     pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                    to be transmitted.
     meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
     client_info:   info identifying the LLC instance.

RETURN VALUE
  Abide by the prototype of a PS Signal handler:
     0 on success
    -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_tx_cback
(
  ps_iface_type *      iface_ptr,
  dsm_item_type **     pkt_chain_ptr,
  ps_tx_meta_info_type* meta_info_ptr,
  void*                client_info
)
{
  ip_version_enum_type ip_vsn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (NULL == meta_info_ptr) ||
       (NULL == PS_TX_META_GET_RT_META_INFO_PTR(meta_info_ptr)))
  {
    LOG_MSG_ERROR_0("NULL meta info type, dropping pkt");
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(pkt_chain_ptr);
    LLC_INC_INSTANCE_STATS( ((lan_lle_enum_type)(uint32)(client_info)),
                             frms_fail_tx, 1);
    return -1;
  }

  ip_vsn = PS_TX_META_GET_PKT_INFO(meta_info_ptr).ip_vsn;

  switch(ip_vsn)
  {
    case IP_V4:
      return lan_llci_v4_tx_cback(iface_ptr,
                                  pkt_chain_ptr,
                                  meta_info_ptr,
                                  client_info);
#ifdef FEATURE_DATA_PS_IPV6
    case IP_V6:
      return lan_llci_v6_tx_cback(iface_ptr,
                                  pkt_chain_ptr,
                                  meta_info_ptr,
                                  client_info);
#endif /* FEATURE_DATA_PS_IPV6  */

    default:
      LOG_MSG_ERROR_1("Invalid IP type: %d", ip_vsn);
      PS_TX_META_INFO_FREE(&meta_info_ptr);
      dsm_free_packet(pkt_chain_ptr);
      LLC_INC_INSTANCE_STATS( ((lan_lle_enum_type)(uint32)(client_info)),
                               frms_fail_tx, 1);
  }

  return -1;
} /* lan_llci_tx_cmd()  */

/*===========================================================================
FUNCTION LAN_LLCI_V4_TX_CBACK()

DESCRIPTION
  This function is an IPv4 specific callback that is registered with PS_IFACE
  when starting up an IPv4 LLE instance. This is the TX CMD executed by
  PS_IFACE when requesting transmission at the LLC layer.

PARAMETERS
  Abide by tx_cmd prototype of PS IFACE:
     iface_ptr:     Ptr to interface that this pkt was received on - and to
                     operate on.
     pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                    to be transmitted.
     meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
     client_info:   info identifying the LLC instance.

RETURN VALUE
  Abide by the prototype of a PS Signal handler:
     0 on success
    -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_v4_tx_cback
(
  ps_iface_type *      iface_ptr,
  dsm_item_type **     pkt_chain_ptr,
  ps_tx_meta_info_type*   meta_info_ptr,
  void*                client_info
)
{
  lan_lle_enum_type        lle_instance;
  lan_lle_cb_type*         ptr_lle_cb;
  ip_frm_type              tx_frm_type;
  byte                     dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  ps_tx_meta_info_type**   tmp_ps_mi_ptr;
  ps_ip_addr_type *        ip_addr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Recover LLE instance.
  -------------------------------------------------------------------------*/
  lle_instance = (lan_lle_enum_type)(uint32)(client_info);

  /*-------------------------------------------------------------------------
   Make sure lle instance is not corrupted !!!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid lle_instance: %d", lle_instance);
    ASSERT(0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
    Proceed on only if this instance is still started...
  -------------------------------------------------------------------------*/
  ptr_lle_cb = &lle_array[lle_instance];

  if ( TRUE != ptr_lle_cb->started)
  {
    LOG_MSG_INFO2_1(" LLC instance %d is already stopped\n", lle_instance);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(pkt_chain_ptr);
    LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy the PS meta info into the dsm_item to be passed along with the pkt

    NOTES:
    - This (app_ptr) field should be unused at this point in the stack
    - The order in which dsm item and PS meta info are freed is significant:
      PS meta info must be freed FIRST and then dsm item
  -------------------------------------------------------------------------*/
  if( NULL != (*pkt_chain_ptr)->app_ptr )
  {
    LOG_MSG_ERROR_0("app_ptr in dsm wasn't reset, overwritting it");
  }

  (*pkt_chain_ptr)->app_ptr = (void *) meta_info_ptr;

  /*-------------------------------------------------------------------------
    Use a temp ptr for readability of code
  -------------------------------------------------------------------------*/
  tmp_ps_mi_ptr = ( (ps_tx_meta_info_type**)(&((*pkt_chain_ptr)->app_ptr)) );

  /*-------------------------------------------------------------------------
    Proceed with pkt preparation
  -------------------------------------------------------------------------*/
  tx_frm_type = lan_llci_get_ip_frm_type(&meta_info_ptr);


  switch (tx_frm_type)
  {

    case LLC_UNKNOWN_IP_FRM:
    {
      LOG_MSG_ERROR_0("Can't determine if IP pkt is Unicast, Multicast, Broadcast");
      PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
      dsm_free_packet(pkt_chain_ptr);
      LLC_INC_INSTANCE_STATS(lle_instance, unknown_proto_drop_tx, 1);
      LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
      return -1;
    }

    case LLC_MULTICAST_IP_FRM:
    {
      if ( lan_llci_get_multicast_hw_addr(dst_hw_addr,
                                          pkt_chain_ptr,
                                          &meta_info_ptr) < 0 )
      {
        LOG_MSG_ERROR_0("Can't get Multicast MAC address of Multicast IP packet");
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return -1;
      }

      /*---------------------------------------------------------------------
        Transmit directly packet to MAC without  any ARP resolution
      ---------------------------------------------------------------------*/
      if ( lan_llci_send_pkt(lle_instance,
                             pkt_chain_ptr,
                             LAN_ETHERTYPE_IPV4,
                             dst_hw_addr ) < 0 )
      {
        LOG_MSG_ERROR_1("Error transmitting LLC intance %d packet",
                        lle_instance  );
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return -1;
      }
      break;
    }

    case LLC_BROADCAST_IP_FRM:
    {

      /*-------------------------------------------------------------------------
        Initialize destination hardware address to the broadcast address
      -------------------------------------------------------------------------*/
      memscpy(dst_hw_addr,
             LAN_IEEE_MAC_ADDR_LEN,
             LAN_IEEE_MAC_BROADCAST_ADDR,
             LAN_IEEE_MAC_ADDR_LEN );
      /*---------------------------------------------------------------------
        Transmit directly packet to MAC without  any ARP resolution
      ---------------------------------------------------------------------*/
      if ( lan_llci_send_pkt(lle_instance,
                             pkt_chain_ptr,
                             LAN_ETHERTYPE_IPV4,
                             dst_hw_addr ) < 0 )
      {
        LOG_MSG_ERROR_1("Error transmitting LLC intance %d packet",
                        lle_instance  );
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return -1;
      }
      break;
    }

    case LLC_UNICAST_IP_FRM:
    {
      /*---------------------------------------------------------------------
        Transmit packet after ARP resolution
      ---------------------------------------------------------------------*/
      ip_addr_ptr = lan_llci_get_dest_ip_addr(&meta_info_ptr);

      if ( arp_ipv4_tx_pkt(lle_instance,
                           ip_addr_ptr->addr.v4.ps_s_addr,
                           pkt_chain_ptr) < 0 )
      {
        LOG_MSG_ERROR_0("Error when performing ARP resolution");
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return  -1;
      }
      break;
    }
  }

  return 0;

} /* lan_llci_v4_tx_cback() */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION LAN_LLCI_V6_TX_CBACK()

DESCRIPTION
  This function is an IPv6 specific callback that is registered with PS_IFACE
  when starting up an IPv6 LLE instance. This is the TX CMD executed by
  PS_IFACE when requesting transmission at the LLC layer.

PARAMETERS
  Abide by tx_cmd prototype of PS IFACE:
     iface_ptr:     Ptr to interface that this pkt was received on - and to
                     operate on.
     pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                    to be transmitted.
     meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
     client_info:   info identifying the LLC instance.

RETURN VALUE
  Abide by the prototype of a PS Signal handler:
     0 on success
    -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_v6_tx_cback
(
  ps_iface_type *      iface_ptr,
  dsm_item_type **     pkt_chain_ptr,
  ps_tx_meta_info_type*   meta_info_ptr,
  void*                client_info
)
{
  lan_lle_enum_type        lle_instance;
  lan_lle_cb_type*         ptr_lle_cb;
  ip_frm_type              tx_frm_type;
  byte                     dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  ps_tx_meta_info_type**   tmp_ps_mi_ptr;
  ps_ip_addr_type *        ip_addr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Recover LLE instance.
  -------------------------------------------------------------------------*/
  lle_instance = (lan_lle_enum_type)(uint32)(client_info);

  /*-------------------------------------------------------------------------
   Make sure lle instance is not corrupted !!!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid lle_instance: %d", lle_instance);
    ASSERT(0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
    Proceed on only if this instance is still started...
  -------------------------------------------------------------------------*/
  ptr_lle_cb = &lle_array[lle_instance];

  if ( TRUE != ptr_lle_cb->started)
  {
    LOG_MSG_INFO2_1(" LLC instance %d is already stopped\n", lle_instance);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    dsm_free_packet(pkt_chain_ptr);
    LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Copy the PS meta info into the dsm_item to be passed along with the pkt

    NOTES:
    - This (app_ptr) field should be unused at this point in the stack
    - The order in which dsm item and PS meta info are freed is significant:
      PS meta info must be freed FIRST and then dsm item
  -------------------------------------------------------------------------*/
  if( NULL != (*pkt_chain_ptr)->app_ptr )
  {
    LOG_MSG_ERROR_0("app_ptr in dsm wasn't reset, overwritting it");
  }
  
  (*pkt_chain_ptr)->app_ptr = (void *) meta_info_ptr;

  /*-------------------------------------------------------------------------
    Use a temp ptr for readability of code
  -------------------------------------------------------------------------*/
  tmp_ps_mi_ptr = ( (ps_tx_meta_info_type**)(&((*pkt_chain_ptr)->app_ptr)) );

  /*-------------------------------------------------------------------------
    Proceed with pkt preparation
  -------------------------------------------------------------------------*/
  tx_frm_type = lan_llci_get_ip_frm_type(&meta_info_ptr);


  switch (tx_frm_type)
  {

    case LLC_UNKNOWN_IP_FRM:
    {
      LOG_MSG_ERROR_0("Can't determine if IPv6 pkt is Unicast or Multicast");
      PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
      dsm_free_packet(pkt_chain_ptr);
      LLC_INC_INSTANCE_STATS(lle_instance, unknown_proto_drop_tx, 1);
      LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
      return -1;
    }

    case LLC_MULTICAST_IP_FRM:
    {
      if ( lan_llci_get_multicast_hw_addr(dst_hw_addr,
                                          pkt_chain_ptr,
                                          &meta_info_ptr) < 0 )
      {
        LOG_MSG_ERROR_0("Can't get Multicast MAC address of Multicast IPv6 packet");
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return -1;
      }

      /*---------------------------------------------------------------------
        Transmit directly packet to MAC without any ICMP6 ND resolution
      ---------------------------------------------------------------------*/
      if ( lan_llci_send_pkt(lle_instance,
                             pkt_chain_ptr,
                             LAN_ETHERTYPE_IPV6,
                             dst_hw_addr ) < 0 )
      {
        LOG_MSG_ERROR_1("Error transmitting LLC intance %d packet",
                        lle_instance  );
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return -1;
      }
      break;
    }

    case LLC_UNICAST_IP_FRM:
    {
      /*---------------------------------------------------------------------
        Transmit packet after ICMP6 ND resolution
      ---------------------------------------------------------------------*/
      ip_addr_ptr = lan_llci_get_dest_ip_addr(&meta_info_ptr);

      if ( ps_icmp6_nd_tx_pkt(lle_instance,
                              &ip_addr_ptr->addr.v6,
                              pkt_chain_ptr) < 0 )
      {
        LOG_MSG_ERROR_0("Error sending IPv6 packet");
        PS_TX_META_INFO_FREE(tmp_ps_mi_ptr);
        dsm_free_packet(pkt_chain_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
        return  -1;
      }

      break;
    }

    default:
      break;

  }

  return 0;
} /* lan_llci_v6_tx_cback() */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION LAN_LLCI_SEND_PKT()

DESCRIPTION
  This function is called to send an IP Packet without address resolution in
  (case of broadcast IP packets) or by the ARP module to send either queued
  IP or ARP packets. As such, this function performs the necessary LLC/SNAP
  encapulation based on the LLE mode of operation. All pointers supplied as
  function arguments belong to caller and are not freed by this function.

PARAMETERS
  lle_instance:  The LLE instance transmitting this packet
  pkt_chain_ptr: pointer to chain of dms items that hold the payload of the
                 packet that is to be transmitted.
  ethertype:     packet ethertype.
  src_hw_addr:   pointer to source hardware address.

RETURN VALUE
  Abide by the prototype of a PS IFACE Tx CMD:
     0 on success
    -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_send_pkt
(
  lan_lle_enum_type    lle_instance,
  dsm_item_type**      pkt_chain_ptr,
  uint16               ethertype,
  byte*                dst_hw_addr
)
{
  lan_lle_cb_type*         ptr_lle_cb;
  lan_llc_start_info_type* ptr_start_info;
  llc_frm_packed_mode_zero_type* ptr_zero_hdr;
  llc_frm_packed_mode_one_type*  ptr_one_hdr;
  ps_tx_meta_info_type*    tmp_ps_mi_ptr;
  uint16 bytes_to_write = 0;
  uint16 bytes_written;
  uint16 nwk_ethertype;
  void*  qos_mi_ptr;
  ps_phys_link_type  * ps_phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tmp_ps_mi_ptr = NULL;
  /*-------------------------------------------------------------------------
    Recover the PS meta info from the dsm item app_ptr then reset the app
    ptr for later use

    Note: This MUST be retrieved here as any subsequent pushdowns can cause
    the meta info to be lost

    TODO: Change the mechanism of meta info passing
  -------------------------------------------------------------------------*/
  ASSERT( NULL != pkt_chain_ptr );

  if( NULL != *pkt_chain_ptr )
  {
    tmp_ps_mi_ptr = (ps_tx_meta_info_type*) ((*pkt_chain_ptr)->app_ptr);

    (*pkt_chain_ptr)->app_ptr = NULL;
  }

  /*-------------------------------------------------------------------------
    LLE instance sanity check!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid lle_instance: %d", lle_instance);
    ASSERT(0);
    return FALSE;
  };

  /*-------------------------------------------------------------------------
    Recover LLE Configuration information
  -------------------------------------------------------------------------*/
  ptr_lle_cb = &lle_array[lle_instance]; 
  ptr_start_info = &ptr_lle_cb->start_info;

  /*-------------------------------------------------------------------------
     Pushdwon LLC SNAP header if LLC mode is Type 1
  -------------------------------------------------------------------------*/
  nwk_ethertype = ps_htons(ethertype);

  switch (ptr_start_info->lle_mode)
  {
    case LAN_LLC_MODE_0:
    {
      /*---------------------------------------------------------------------
        No LLC/SNAP encapsulation;
        Only ethertype is pushed on frame
      ---------------------------------------------------------------------*/
      bytes_to_write = sizeof(llc_frm_packed_mode_zero_type);
      ptr_zero_hdr = &ptr_lle_cb->snd_pkt.zero_hdr;
      ptr_zero_hdr->ethertype = nwk_ethertype;

      /* Src Addr already filled in*/
      memscpy(ptr_zero_hdr->dst_hw_addr,
              LAN_IEEE_MAC_ADDR_LEN,
              dst_hw_addr,
              LAN_IEEE_MAC_ADDR_LEN);
      
      /* Pushdown mode zero header                                        */
      bytes_written = dsm_pushdown_packed(pkt_chain_ptr,
                             ptr_zero_hdr,
                             bytes_to_write,
                             DSM_DS_SMALL_ITEM_POOL);

      LLC_INC_INSTANCE_STATS(lle_instance, llc_mode0_tx, 1);
      break;
    }

    case LAN_LLC_MODE_1:
    {
      /*---------------------------------------------------------------------
       LLC/SNAP Encapsulation
      ---------------------------------------------------------------------*/
      ptr_one_hdr = &ptr_lle_cb->snd_pkt.one_hdr;
      ptr_one_hdr->snap_hdr.ethertype = nwk_ethertype;
      bytes_to_write = sizeof(llc_frm_packed_mode_one_type);

      /* Src Addr already filled in */
      memscpy(ptr_one_hdr->dst_hw_addr,
              LAN_IEEE_MAC_ADDR_LEN,
              dst_hw_addr,
              LAN_IEEE_MAC_ADDR_LEN);
      
      /* Pushdown mode zero header                                        */
      bytes_written = dsm_pushdown_packed(pkt_chain_ptr,
                             ptr_one_hdr,
                             bytes_to_write,
                             DSM_DS_SMALL_ITEM_POOL);

      LLC_INC_INSTANCE_STATS(lle_instance, llc_mode1_tx, 1);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Unsupported LLC Mode %d", ptr_start_info->lle_mode );
      LLC_INC_INSTANCE_STATS(lle_instance, invalid_mode_drop_tx, 1);
      LLC_INC_INSTANCE_STATS(lle_instance, frms_fail_tx, 1);
      ASSERT(0);
      return -1;
    }

  }

  /*-------------------------------------------------------------------------
    LLC PDU is ready!
    Sanity check if hdr filled properly
  -------------------------------------------------------------------------*/
  if (bytes_written != bytes_to_write)
  {
    /* Drop the packet. Free the DSM as well as meta info, if any*/
    LOG_MSG_ERROR_2("Invalid number of bytes written %d in dsm."
                    "Reqd = %d.Dropping", bytes_written, bytes_to_write);
    dsm_free_packet( pkt_chain_ptr );
    PS_TX_META_INFO_FREE(&tmp_ps_mi_ptr);
    return 0;
  }

    /*-----------------------------------------------------------------------
      Log the frame to be transmitted
    -----------------------------------------------------------------------*/
  ps_phys_link_ptr = PS_IFACE_GET_PHYS_LINK(ptr_start_info->iface_ptr);
  if(ps_phys_link_ptr != NULL)
  {
    DPL_LOG_LINK_FULL_TX_PACKET(ps_phys_link_ptr,
                                *pkt_chain_ptr,
                                DPL_IID_LINKPROT_ETHERNET);
  }

  /*-------------------------------------------------------------------------
    Note: there should be no further pushdowns/pullups after this:

    Or QoS meta info could be lost!

    TODO: Need to change this mechanism of passing QoS MI
  -------------------------------------------------------------------------*/
  if( ptr_start_info->get_qos_mi_f_ptr )
  {
    /*-----------------------------------------------------------------------
       Retrieve pkt QoS meta info (back into app_ptr)
    -----------------------------------------------------------------------*/
    qos_mi_ptr = ptr_start_info->get_qos_mi_f_ptr( tmp_ps_mi_ptr,
                                                   ethertype,
                                                   ptr_start_info->
                                                   get_qos_mi_f_user_data );

    /*-----------------------------------------------------------------------
      Place the QoS meta info now in the dsm item
    -----------------------------------------------------------------------*/
    if( NULL != (*pkt_chain_ptr)->app_ptr )
    {
      LOG_MSG_ERROR_0("app_ptr in dsm wasn't reset, overwritting it");
    }

    (*pkt_chain_ptr)->app_ptr = qos_mi_ptr;

  }

  /*-------------------------------------------------------------------------
    Free the PS meta information
  -------------------------------------------------------------------------*/
  if( tmp_ps_mi_ptr )
  {
    PS_TX_META_INFO_FREE(&tmp_ps_mi_ptr);
  }


  /*-------------------------------------------------------------------------
     Now we can request the packet transmission at the MAC layer
  -------------------------------------------------------------------------*/
  ptr_start_info->tx_f_ptr(pkt_chain_ptr,
                           ptr_start_info->tx_f_user_data);

  LLC_INC_INSTANCE_STATS(lle_instance, frms_tx, 1);
  return 0;

} /* lan_llci_send_pkt() */

/*===========================================================================
FUNCTION LAN_LLC_RX_PKT()
===========================================================================*/
/** @ingroup lan_llc_rx_pkt
  Processes the received LLC PDU. LLC/SNAP header is removed (if applicable)
  and packets are processed and dispacthed to either IP stack or ARP module.

  @datatypes
  #lan_lle_enum_type\n
  #dsm_item_type\n
  #ps_meta_info_type_ex

  @vertspace
  @param[in] lle_instance     LLE instance receiving this packet.
  @param[in] rx_pkt_ptr       Pointer to the chain of DMS items that hold the 
                              payload of the packet that is received.
  @param[in] meta_info_ex_ptr Meta info for the received packet.

  @return
   0 -- IP or ARP packet received successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_rx_pkt
(
  lan_lle_enum_type      lle_instance,
  dsm_item_type*         rx_pkt_ptr,
  ps_meta_info_type_ex  *meta_info_ex_ptr
)
{
  lan_lle_cb_type*         ptr_lle_cb;
  lan_llc_start_info_type *ptr_start_info;
  byte                     dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  byte                     src_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  llc_frm_body_type        frm_type;
  ps_phys_link_type       *ps_phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Make sure lle instance is not corrupted !!!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR("Invalid lle_instance: %d",lle_instance,0,0);
    return -1;
  };

  /*---------------------------------------------
   Validate input packet
  -----------------------------------------------*/
  if ( rx_pkt_ptr == NULL )
  {
    LOG_MSG_ERROR("NULL rx pkt lle instance: %d", lle_instance, 0, 0);
    return -1;
  }

  /*-------------------------------------------------------------------------
   Retrieve Start info
  -------------------------------------------------------------------------*/
  ptr_lle_cb     = &lle_array[lle_instance];
  ptr_start_info = &ptr_lle_cb->start_info;

  /*-------------------------------------------------------------------------
    Proceed on only if this instance is still started...
  -------------------------------------------------------------------------*/
  if ( TRUE != ptr_lle_cb->start_lock)
  {
    if ( TRUE != ptr_lle_cb->started )
    {
      LOG_MSG_ERROR( "Receive RX pkt when LLC instance %d stopped",
                     lle_instance, 0, 0 );
      LLC_INC_INSTANCE_STATS(lle_instance, instance_stop_rx, 1);
      return -1;
    }
  }
  else
  {
    if ( TRUE != ptr_lle_cb->started)
    {
      LOG_MSG_ERROR( "Receive RX pkt when LLC instance %d startup is pending",
                     lle_instance, 0, 0 );
      /*-----------------------------------------------------------------------
        Caller should re-invoke the RX packet API
      -----------------------------------------------------------------------*/
      LLC_INC_INSTANCE_STATS(lle_instance, instance_start_pending_rx, 1);
      return -1;
    }
  }

  /*---------------------------------------------------------------------
    Log the received frame
  ---------------------------------------------------------------------*/
  ps_phys_link_ptr = PS_IFACE_GET_PHYS_LINK(ptr_start_info->iface_ptr);
  if(ps_phys_link_ptr != NULL)
  {
    DPL_LOG_LINK_FULL_RX_PACKET(ps_phys_link_ptr,
                                rx_pkt_ptr,
                                DPL_IID_LINKPROT_ETHERNET);
  }
  
  /*-----------------------------------------------------------------------
   Retrieve an LLC PDU Process it!
  -----------------------------------------------------------------------*/
  LLC_INC_INSTANCE_STATS(lle_instance, frms_rx, 1);
  frm_type = lan_llci_decapsulate(&rx_pkt_ptr,
                                  lle_instance,
                                  src_hw_addr,
                                  dst_hw_addr);
  
  /*-------------------------------------------------------------------
   Check for Zero payload ethernet frames to skip processing.
  -------------------------------------------------------------------*/
  if ( rx_pkt_ptr == NULL )
  {
    LOG_MSG_INFO1("Zero payload ethernet frame."
                  "Skip further processing!!", 0,0, 0);
    LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
    if ( meta_info_ex_ptr != NULL )
    {
      PS_RX_META_INFO_FREE(&(meta_info_ex_ptr->rx_meta_info_ptr));
      PS_TX_META_INFO_FREE(&(meta_info_ex_ptr->tx_meta_info_ptr));
      /* Clear meta_info_ex_ptr */
      PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
    }
    return -1;
  }
  
  switch (frm_type)
  {
    case LLC_UNKNOWN_FRM:
    {
      /*-------------------------------------------------------------------
        Just drop frame for now
      -------------------------------------------------------------------*/
      LOG_MSG_INFO2("Receive unknown LLC frame, Frame will be dropped!", 0,0, 0);
      dsm_free_packet(&rx_pkt_ptr);
      LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      break;
    }
  
    case LLC_CTRL_FRM:
    {
      /*-------------------------------------------------------------------
        LLC Control frames
      -------------------------------------------------------------------*/
      LOG_MSG_INFO2("Processing LLC Control frame", 0,0, 0);
      lan_llci_ctrl_input(&rx_pkt_ptr,
                             ptr_start_info->lle_mode);
      LLC_INC_INSTANCE_STATS(lle_instance, llc_control_frms_rx, 1);
      break;
    }
  
  
    case LLC_IPV4_FRM:
    {
      /*-------------------------------------------------------------------
       Input to PS_IFACE
  
       Check if it makes more sense to do the check further up the stack.
      -------------------------------------------------------------------*/
      if(!ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
      {
        LOG_MSG_INFO3("Inputting IPv4 frame to PS IFACE", 0, 0, 0);
        (void) ps_iface_input(ptr_start_info->iface_ptr,
                              &rx_pkt_ptr,
                              meta_info_ex_ptr);
        meta_info_ex_ptr = NULL; /* ps_iface_input will free this */
      }
      else
      {
        LOG_MSG_INFO2("Dropping mismatched IPv4 packet for interface type",0,0,0);
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      }
      break;
    }
  
    case LLC_IPV6_FRM:
  #ifdef FEATURE_DATA_PS_IPV6
    {
      /*-------------------------------------------------------------------
       Input to PS_IFACE
      -------------------------------------------------------------------*/
      if(ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
      {
        LOG_MSG_INFO3("Inputting IPv6 frame to PS IFACE", 0, 0, 0);
        (void) ps_iface_input(ptr_start_info->iface_ptr,
                              &rx_pkt_ptr,
                              meta_info_ex_ptr);
        meta_info_ex_ptr = NULL; /* ps_iface_input will free this */
      }
      else
      {
        LOG_MSG_INFO2("Dropping mismatch IPv6 packet for interface type",0,0,0);
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      }
      break;
    }
  #else
    {
      LOG_MSG_INFO1("IPv6 not supported. Dropping packet", 0, 0, 0);
      dsm_free_packet(&rx_pkt_ptr);
      LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      break;
    }
  #endif /* FEATURE_DATA_PS_IPV6 */
  
    case LLC_ARP_FRM:
    {
      if(!ps_iface_addr_family_is_v6(ptr_start_info->iface_ptr))
      {
      /*-------------------------------------------------------------------
        Input to ARP Module
      -------------------------------------------------------------------*/
      LOG_MSG_INFO3("LLC Module inputting ARP fram to ARP module", 0, 0, 0);
      arp_input(lle_instance,
                &rx_pkt_ptr,
                src_hw_addr,
                dst_hw_addr);
      }
      else
      {
        LOG_MSG_INFO2("Dropping ARP frame on non IPv4 interface",0,0,0);
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, frms_drop_rx, 1);
      }
      break;
    }
  
    case LLC_802_1X_FRM:
    {
      /*-------------------------------------------------------------------
        Input to 802_1X Module
      -------------------------------------------------------------------*/
      if ( NULL != ptr_lle_cb->lan_1x_input_f_ptr )
      {
        LOG_MSG_INFO3( "LLC Module inputting 802 1X frame to 802.1X module",
                 0, 0, 0);
        ptr_lle_cb->lan_1x_input_f_ptr( lle_instance,
                                        &rx_pkt_ptr,
                                        src_hw_addr,
                                        dst_hw_addr);
      }
      else
      {
        LOG_MSG_ERROR("Dropping 802_1X packet cause no input func registered",
                  0, 0, 0);
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, llc_802_1x_unsupported, 1);
      }
  
      break;
    }
  
    case LLC_802_1X_FRM_PREAUTH:
    {
      /*-------------------------------------------------------------------
        Input to 802_1X Module
      -------------------------------------------------------------------*/
      if ( NULL != ptr_lle_cb->lan_1x_preauth_input_f_ptr )
      {
        LOG_MSG_INFO3( "LLC inputting 802 1X Preauth frame to LAN 1X module",
                 0, 0, 0);
        ptr_lle_cb->lan_1x_preauth_input_f_ptr( lle_instance,
                                                &rx_pkt_ptr,
                                                src_hw_addr,
                                                dst_hw_addr);
      }
      else
      {
        LOG_MSG_ERROR("Dropping 802 1X Preaut frame: no input func registered",
                  0, 0, 0);
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, llc_802_1x_preauth_unsupported, 1);
      }
  
      break;
    }
  
    case LLC_WAPI_FRM:
    {
      /*-------------------------------------------------------------------
        Input to WAPI Module
      -------------------------------------------------------------------*/
      if ( NULL != ptr_lle_cb->lan_wapi_input_f_ptr )
      {
        LOG_MSG_INFO3( "LLC Module inputting WAPI frame to WAPI module",
                 0, 0, 0);
        ptr_lle_cb->lan_wapi_input_f_ptr( lle_instance,
                                          &rx_pkt_ptr,
                                          src_hw_addr,
                                          dst_hw_addr);
      }
      else
      {
        LOG_MSG_ERROR_0("Dropping WAPI packet: No input func registered");
        dsm_free_packet(&rx_pkt_ptr);
        LLC_INC_INSTANCE_STATS(lle_instance, llc_wapi_unsupported, 1);
      }
  
      break;
    }
  
  }// End switch frm_type
  
  /* Free Metainfo if allocated! */
  if(meta_info_ex_ptr != NULL)
  {
    /* TX Metainfo is allocated in rx_f_ptr, free it */
    PS_TX_META_INFO_FREE(&(meta_info_ex_ptr->tx_meta_info_ptr));
    PS_META_INFO_FREE_EX(&meta_info_ex_ptr);
  }
  
  return 0;
}


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION LAN_LLCI_DECAPSULATE()

DESCRIPTION
  This function decapsulates an LLC PDU, removing any header information.

PARAMETERS
  rx_pkt_ptr: pointer to chain of dms items that hold the payload of the
                 packet that is to be transmitted.
  lan_lle_enum_type:lle_instance

RETURN VALUE
  Return the packet type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC llc_frm_body_type lan_llci_decapsulate
(
  dsm_item_type**           rx_pkt,
  lan_lle_enum_type         lle_instance,
  byte*                     src_hw_addr,
  byte*                     dst_hw_addr
)
{
  uint16               bytes_read;
  uint16               ethertype;
  lan_llc_snap_hdr    *tmp_snap_hdr_ptr;
  uint32               oui;
  unsigned int temp_size;
  lan_lle_cb_type*         ptr_lle_cb;
  lan_llc_start_info_type* ptr_start_info;
  llc_frm_packed_mode_zero_type rcv_zero_hdr;
  llc_frm_packed_mode_one_type rcv_one_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ASSERT(NULL != src_hw_addr);
  ASSERT(NULL != dst_hw_addr);

  /*-------------------------------------------------------------------------
    Recover LLE Configuration information
  -------------------------------------------------------------------------*/
  ptr_lle_cb = &lle_array[lle_instance]; 
  ptr_start_info = &ptr_lle_cb->start_info;

  /*-------------------------------------------------------------------------
    LLE instance sanity check!
  -------------------------------------------------------------------------*/
  if(lle_instance <= LAN_LLE_MIN || lle_instance >= LAN_LLE_MAX)
  {
    LOG_MSG_ERROR_1("Invalid lle_instance: %d", lle_instance);
    ASSERT(0);
    return LLC_UNKNOWN_FRM;
  };

  switch (ptr_start_info->lle_mode)
  {

    case LAN_LLC_MODE_0:
    {
      /*---------------------------------------------------------------------
       LLC PDU has the following structure:

       |----------|---------------|
       | Ethertye | Frame Body    |
       |----------|---------------|
      ---------------------------------------------------------------------*/
      /*-------------------------------------------------------------------------
         Retrieve Dest and Src hardware addresses along with mode 0 hdr
      -------------------------------------------------------------------------*/
      bytes_read = dsm_pullup(rx_pkt, &rcv_zero_hdr, 
                              sizeof(llc_frm_packed_mode_zero_type));
      if(sizeof(llc_frm_packed_mode_zero_type) > bytes_read)
      {
        LOG_MSG_ERROR_1("Short 802.3 hdr! (%d) bytes", bytes_read);
        LLC_INC_INSTANCE_STATS(lle_instance, inv_dest_addr_rx, 1);
        return LLC_UNKNOWN_FRM;
      }

      memscpy(dst_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN, 
              rcv_zero_hdr.dst_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN);
      memscpy(src_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN,
              rcv_zero_hdr.src_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN);
        
      ethertype = rcv_zero_hdr.ethertype;
      bytes_read -= 2;
      
      LLC_INC_INSTANCE_STATS(lle_instance, llc_mode0_rx, 1);
      break;
    }

    case LAN_LLC_MODE_1:
    {
      /*---------------------------------------------------------------------
       Pullup LLC Header first!
      ---------------------------------------------------------------------*/

      /*-------------------------------------------------------------------------
         Retrieve Dest and Src hardware addresses along with mode 1 hdr
      -------------------------------------------------------------------------*/
      bytes_read = dsm_pullup(rx_pkt, &rcv_one_hdr, 
                              sizeof(llc_frm_packed_mode_one_type));
      if(sizeof(llc_frm_packed_mode_one_type) > bytes_read)
      {
        LOG_MSG_ERROR_1("Short 802.3 hdr! (%d) bytes", bytes_read);
        LLC_INC_INSTANCE_STATS(lle_instance, inv_dest_addr_rx, 1);
        return LLC_UNKNOWN_FRM;
      }

      memscpy(dst_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN, 
              rcv_one_hdr.dst_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN);
      memscpy(src_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN, 
              rcv_one_hdr.src_hw_addr, 
              LAN_IEEE_MAC_ADDR_LEN);

      /*---------------------------------------------------------------------
        The LLC frame is an UI one. It should follow the following structure:

               |----|----|----|------|----------|-------------|
               |DSAP|SSAP|CTRL| OUI  | Ethertype|  frame Body |
               |----|----|----|------|----------|-------------|
      ---------------------------------------------------------------------*/
      tmp_snap_hdr_ptr = &rcv_one_hdr.snap_hdr;

      if (LAN_LLC_UI_CTRL_VAL != tmp_snap_hdr_ptr->ctrl)
      {
        LOG_MSG_INFO2_0("Receive probably an  internal LLC Ctrl frame");
        return LLC_CTRL_FRM;
      }

      /* Check FIELD                                                       */
      temp_size = sizeof(uint32);
      if( LAN_LLC_SNAP_OUI_LEN < temp_size )
      {
        temp_size = LAN_LLC_SNAP_OUI_LEN;
      }
      oui = 0;
      memscpy(&oui, sizeof(oui),(void*)tmp_snap_hdr_ptr->oui,temp_size);
     

      if ( LAN_LLC_SNAP_SAP_ADDR == tmp_snap_hdr_ptr->ssap &&
           LAN_LLC_SNAP_SAP_ADDR == tmp_snap_hdr_ptr->dsap  &&
           LAN_LLC_SNAP_STD_OUI  == (oui & LAN_LLC_SNAP_OUI_FIELD_MASK)
         )
      {
        ethertype = *(uint16 *)(&tmp_snap_hdr_ptr->ethertype);
      }
      else
      {
        LOG_MSG_ERROR_1("Error in LLC/SNAP header; OUI: %d", oui);
        LLC_INC_INSTANCE_STATS(lle_instance, mode1_inv_oui_rx, 1);
        return LLC_UNKNOWN_FRM;
      }

      /*---------------------------------------------------------------------
        SNAP header may be followed by 802.1Q VLAN tag. Frame format is then:

          |----|----|----|------|-------|------|-----------|------------|
          |DSAP|SSAP|CTRL| OUI  | 8100H | VLAN | Ethertype | frame Body |
          |----|----|----|------|-------|------|-----------|------------|

        For now, we simply discard the 802.1Q tag and re-read the ethertype
      ---------------------------------------------------------------------*/
      if( LAN_ETHERTYPE_802_1Q_TAG == (ps_ntohs(ethertype)) )
      {
        lan_llc_vlan_tag vlan_tag;

        bytes_read = dsm_pullup(rx_pkt,
                                (void*)&vlan_tag,
                                sizeof(vlan_tag));

        if( sizeof(vlan_tag) != bytes_read )
        {
          LOG_MSG_ERROR_1("Short LLC VLAN TAG! (%d) bytes", bytes_read);
          LLC_INC_INSTANCE_STATS(lle_instance, mode1_inv_vlan_len_rx, 1);
          return LLC_UNKNOWN_FRM;
        }

        /*-------------------------------------------------------------------
          Note that if the CFI bit is non-zero there is an additional ERIF
          field
          Also, the MAC address carried by the frame could be in
          non-canonical form.

          Since we support neither of these, the CFI bit should be reset (0)
        -------------------------------------------------------------------*/
        if( 0 != vlan_tag.cfi )
        {
          LOG_MSG_ERROR_0("Invalid VLAN TAG! CFI non-zero");
          LLC_INC_INSTANCE_STATS(lle_instance, mode1_inv_vlan_frm_rx, 1);
          return LLC_UNKNOWN_FRM;
        }

        /*-------------------------------------------------------------------
          Note that we do not check the VLAN ID as we do not support VLANs
          as such. We just attempt to decode the rest of the frame as if
          the VLAN tag were not present

          Also we do not care about the user priority
        -------------------------------------------------------------------*/

        bytes_read = dsm_pullup(rx_pkt,
                                (void*)&ethertype,
                                sizeof(ethertype));

        if( sizeof(ethertype) != bytes_read )
        {
          LOG_MSG_ERROR_1("Short LLC Ethertype! (%d) bytes", bytes_read);
          LLC_INC_INSTANCE_STATS(lle_instance, mode1_inv_ethlen_rx, 1);
          return LLC_UNKNOWN_FRM;
        }
        LLC_INC_INSTANCE_STATS(lle_instance, mode1_vlan_frms_rx, 1);
      }

      LLC_INC_INSTANCE_STATS(lle_instance, llc_mode1_rx, 1);
      break;
    } // End case.

    default:
    {
      LOG_MSG_ERROR_1("Unsupported LLC mode %d", (&(&lle_array[lle_instance])->start_info)->lle_mode);
      LLC_INC_INSTANCE_STATS(lle_instance, inv_mode_rx, 1);
      return LLC_UNKNOWN_FRM;
    }
  } //End switch

  /*-------------------------------------------------------------------------
     Now determine frame type base on Ethertype
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO3_1("Received %d Frame within an LLC Packet", ps_ntohs(ethertype));
  switch (ps_ntohs(ethertype))
  {
    case LAN_ETHERTYPE_IPV4:
    {
      /*---------------------------------------------------------------------
        This is an IPv4 frame
      ---------------------------------------------------------------------*/
      return LLC_IPV4_FRM;
    }

#ifdef FEATURE_DATA_PS_IPV6
    case LAN_ETHERTYPE_IPV6:
    {
      /*---------------------------------------------------------------------
        This is an IPv6 frame
      ---------------------------------------------------------------------*/
      return LLC_IPV6_FRM;
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    case LAN_ETHERTYPE_ARP:
    {
      /*---------------------------------------------------------------------
        This is an IP frame
      ---------------------------------------------------------------------*/
      return LLC_ARP_FRM;
    }

    case LAN_ETHERTYPE_802_1X:
    {
      /*---------------------------------------------------------------------
        This is an 802.1X frame
      ---------------------------------------------------------------------*/
      return LLC_802_1X_FRM;
    }

    case LAN_ETHERTYPE_802_1X_PREAUTH:
    {
      /*---------------------------------------------------------------------
        This is an 802.1X frame
      ---------------------------------------------------------------------*/
      return LLC_802_1X_FRM_PREAUTH;
    }

    case LAN_ETHERTYPE_WAPI:
    {
      /*---------------------------------------------------------------------
        This is a WAPI frame
      ---------------------------------------------------------------------*/
      return LLC_WAPI_FRM;
    }

  } //End switch

  /*-------------------------------------------------------------------------
   If we arrive here then the ethertype was wrong
  -------------------------------------------------------------------------*/
  LOG_MSG_ERROR_1("Received LLC Packet with unknown ethertype (%d)",
                  ps_ntohs(ethertype));

  return LLC_UNKNOWN_FRM;

}  /* lan_llci_decapsulate() */



/*===========================================================================
FUNCTION LAN_LLCI_CTRL_INPUT

DESCRIPTION
  This function processes internal LLC Control frame. This implementation
  supports only the XID and TEST commands

PARAMETERS
  rx_pkt_ptr: pointer to chain of dms items that hold the payload of the
              LLC PDU.
  lle_mode : LLC mode of operation

RETURN VALUE
  Return the packet type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void lan_llci_ctrl_input
(
  dsm_item_type**           rx_pkt,
  lan_llc_mode_enum_type lle_mode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
   Internal LLC PDU processing only for LLC Mode 1
  -------------------------------------------------------------------------*/
  if ( LAN_LLC_MODE_1 == lle_mode)
  {
    //TODO
    // Decode LLC PDU and respond
    LOG_MSG_INFO2_0("Received an LLC control frame");
  }

  //End processing LLC CTRL frame
  dsm_free_packet(rx_pkt);

} /* lan_llci_ctrl_input() */




/*===========================================================================

                        UTILITIES FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION LAN_LLCI_GET_IP_FRM_TYPE

DESCRIPTION
  This function retrieves the type of an IP frame based on the supplied meta
  information.

PARAMETERS
  ip_pkt_ptr: pointer to chain of dms items that hold the payload of the
              IP PDU.
  meta_info_ptre : Ptr to meta information...

RETURN VALUE
  Return the IP packet type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC ip_frm_type lan_llci_get_ip_frm_type
(
  ps_tx_meta_info_type** meta_info_ptr
)
{
  ip_frm_type type = LLC_UNKNOWN_IP_FRM;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((NULL == meta_info_ptr) || (NULL == *meta_info_ptr) ||
      (NULL == PS_TX_META_GET_RT_META_INFO_PTR(*meta_info_ptr)))
  {
    LOG_MSG_ERROR_0("Rxed NULL meta info, return UNKNOWN_FRM");
    return type;
  }

  switch(PS_TX_META_GET_IP_ADDR_SCOPE(*meta_info_ptr))
  {
    case IP_ADDR_UNICAST:
    {
      type = LLC_UNICAST_IP_FRM;
      break;
    }

    case IP_ADDR_MULTICAST:
    {
      type = LLC_MULTICAST_IP_FRM;
      break;
    }

    case IP_ADDR_BROADCAST:
    {
      type = LLC_BROADCAST_IP_FRM;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_0("Meta info contains unknown next hop addr scope");
    }
  }

  return type;

} /* lan_llci_get_ip_frm_type() */



/*===========================================================================
FUNCTION LAN_LLCI_GET_DEST_IP_TYPE

DESCRIPTION
  This function retrieves an IP packet next hop destination address.

PARAMETERS
  ip_pkt_ptr: pointer to chain of dms items that hold the payload of the
              IP PDU.
  meta_info_ptre : Ptr to meta information...

RETURN VALUE
  Return next hop destination address.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC ps_ip_addr_type * lan_llci_get_dest_ip_addr
(
  ps_tx_meta_info_type** meta_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT ((NULL != meta_info_ptr) && (NULL!= *meta_info_ptr) &&
          (NULL != PS_TX_META_GET_RT_META_INFO_PTR(*meta_info_ptr)));

  return ( &(PS_TX_META_GET_NEXT_HOP_ADDR(*meta_info_ptr)) );

} /* lan_llci_get_dest_ip_addr() */



/*===========================================================================
FUNCTION LAN_LLCI_GET_MULTICATS_HW_ADDR

DESCRIPTION
  This function retrieves the Multicast MAC address of a Multicast IP Packet.

PARAMETERS
  hw_addr_ptr : pointer to location where to copy the MAc address.
  ip_pkt:       pointer to chain of dsm item that holds the IP packet frame
  metat_info_ptr: pointer to meta information.

RETURN VALUE
  Return -1 if error
         0 if successfull.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC int lan_llci_get_multicast_hw_addr
(
  byte* hw_addr_ptr,
  dsm_item_type** ip_pkt,
  ps_tx_meta_info_type** meta_info_ptr
)
{

  ps_ip_addr_type * ip_addr_ptr;
  char            * ip_addr_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ip_addr_ptr = lan_llci_get_dest_ip_addr(meta_info_ptr);

  switch(ip_addr_ptr->type)
  {
    case IPV4_ADDR:

      ip_addr_buf = (char *)&(ip_addr_ptr->addr.v4.ps_s_addr);
      /*---------------------------------------------------------------------
        An IPv4 host group address is mapped to an Ethernet multicast address
        by placing the low-order 23-bits of the IP address into the low-order
        23 bits of the Ethernet multicast address 01-00-5E-XX-YY-ZZ (hex).
      ---------------------------------------------------------------------*/
      hw_addr_ptr[0] = 0x01;
      hw_addr_ptr[1] = 0x00;
      hw_addr_ptr[2] = 0x5E;
      hw_addr_ptr[3] = ip_addr_buf[1] & 0x7F;
      hw_addr_ptr[4] = ip_addr_buf[2];
      hw_addr_ptr[5] = ip_addr_buf[3];

      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IPV6_ADDR:

      ip_addr_buf = (char *)&(ip_addr_ptr->addr.v6.ps_s6_addr[0]);
      /*---------------------------------------------------------------------
        An IPv6 address is mapped to an Ethernet multicast address by placing
        the low-order 32-bits of the 128 bit IPv6 address into the low-order
        32 bits of the Ethernet multicast address 33-33-WW-XX-YY-ZZ (hex).
      ---------------------------------------------------------------------*/
      hw_addr_ptr[0] = 0x33;
      hw_addr_ptr[1] = 0x33;
      hw_addr_ptr[2] = ip_addr_buf[12];
      hw_addr_ptr[3] = ip_addr_buf[13];
      hw_addr_ptr[4] = ip_addr_buf[14];
      hw_addr_ptr[5] = ip_addr_buf[15];

      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:

      LOG_MSG_ERROR_1("Incorrect IP type %d", ip_addr_ptr->type);
      return -1;
  }
  return 0;
} /* lan_llci_get_multicast_hw_addr() */
