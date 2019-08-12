#ifndef PS_LAN_LLCI_H
#define PS_LAN_LLCI_H
/*===========================================================================
                               P S _ LAN_LLCI . H

DESCRIPTION
  Header file for the PS 802 LLC protocol suite Internal Operaions.

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_lan_llci.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
           Part of it Derived from Jeff Dyck's original Ethernet implementation

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/17/14    cx     Added API to de-init llc module
11/11/13    rk     Removed feature mobileap.
11/01/13    rk     Removed feature mobileap.
08/19/09    pg     Added support for WAPI in LLE Config buffer.
08/11/09   am      Modified locks usage. Allowing tandem start->stop->start 
                   calls in caller context.
03/13/08    pp     Metainfo optimizations.
06/27/06    scb    Added statistics support for the lan_lle module.
02/06/05    ks     Added critical section to lan_lle control block.
06/05/05    hba    Added pointers to 802.1X input functions to LLE CB
09/16/04    hba    Creation
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "dsm.h"
#include "rex.h"
#include "ps_iface_defs.h"
#include "ps_svc.h"
#include "ps_tx_meta_info.h"
#include "ps_lan_llc.h"
#include "ps_crit_sect.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Define utilities macro that is used by the PS LAN LLC module
---------------------------------------------------------------------------*/
#define LAN_LLC_SET_SIG_HANDLER( signal, cback, user_data ) \
  ps_set_sig_handler((signal),(cback), (user_data))

#define LAN_LLC_SEND_CMD( cmd, data_ptr )                   \
  ps_send_cmd((cmd), (void *) (data_ptr))

#define LAN_LLC_ENABLE_SIG( signal )                        \
  ps_enable_sig( (signal) )

#define LAN_LLC_DISABLE_SIG( signal )                       \
  ps_disable_sig( (signal) )

#define LAN_LLC_IS_CURR_TASK_OWNER()                        \
  (rex_self() == &ps_tcb)

#define LAN_LLC_SNAP_OUI_LEN          3

typedef PACKED struct PACKED_POST
{
  uint8  dsap;
  uint8  ssap;
  uint8  ctrl;
  uint8  oui[LAN_LLC_SNAP_OUI_LEN];
  uint16 ethertype;
} lan_llc_snap_hdr;

typedef PACKED struct PACKED_POST
{
  byte              dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  byte              src_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  lan_llc_snap_hdr  snap_hdr;
} llc_frm_packed_mode_one_type;

typedef PACKED struct PACKED_POST
{
  byte              dst_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  byte              src_hw_addr[LAN_IEEE_MAC_ADDR_LEN];

  uint16            ethertype;
} llc_frm_packed_mode_zero_type;

/*---------------------------------------------------------------------------
    LLE Configuration buffer:
        This datat structure corresponds to an LLC instance control block.
    start_lock: if available, is acquired @ startup and released @ stop
                initiation.
    started   : if available, is acquired @ startup complete and released @
                stop complete.
 --------------------------------------------------------------------------*/
typedef struct
{
  ps_crit_sect_type        lan_lle_crit_section;
  boolean                  start_lock; // Use to lock a START Request      */
  boolean                  started;
  lan_lle_enum_type        lle_instance;
  lan_llc_start_info_type  start_info;
  lan_llc_input_f_ptr_type lan_1x_input_f_ptr;
  lan_llc_input_f_ptr_type lan_1x_preauth_input_f_ptr;
  lan_llc_input_f_ptr_type lan_wapi_input_f_ptr;
  boolean                  softap_mode;

  union
  {
    llc_frm_packed_mode_zero_type  zero_hdr;
    llc_frm_packed_mode_one_type   one_hdr;
  } snd_pkt;

  /*--------------------------------------------------------------------
                    Statistics support
  --------------------------------------------------------------------*/
  struct llc_cb_stats
  {
    uint32  frms_tx                         ;
    uint32  llc_mode0_tx                    ;
    uint32  llc_mode1_tx                    ;
    uint32  frms_fail_tx                    ;
    uint32  unknown_proto_drop_tx           ;
    uint32  invalid_mode_drop_tx            ;
    uint32  frms_rx                         ;
    uint32  llc_mode0_rx                    ;
    uint32  llc_mode1_rx                    ;
    uint32  llc_control_frms_rx             ;
    uint32  frms_drop_rx                    ;
    uint32  instance_stop_rx                ;
    uint32  instance_start_pending_rx       ;
    uint32  inv_dest_addr_rx                ;
    uint32  inv_src_addr_rx                 ;
    uint32  inv_mode_rx                     ;
    uint32  mode0_inv_ethlen_rx             ;
    uint32  mode1_inv_snaphdr_len_rx        ;
    uint32  mode1_inv_snaphdr_rx            ;
    uint32  mode1_inv_oui_rx                ;
    uint32  mode1_vlan_frms_rx              ;
    uint32  mode1_inv_vlan_len_rx           ;
    uint32  mode1_inv_vlan_frm_rx           ;
    uint32  mode1_inv_ethlen_rx             ;
    uint32  llc_802_1x_unsupported          ;
    uint32  llc_802_1x_preauth_unsupported  ;
    uint32  llc_wapi_unsupported            ;
    uint32  queried                     ;
  } llc_i_stats;
} lan_lle_cb_type;

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/


extern lan_lle_cb_type  lle_array[LAN_LLE_MAX];



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION LAN_LLC_INIT()

DESCRIPTION
  This function initializes the LLC module main data structures. It should be
  called only one, at power up, before any LLC service is invoked. It also
  called indirectly the initialization function of the ARP module.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void lan_llc_init
(
  void
);

/*===========================================================================
FUNCTION LAN_LLCI_START()

DESCRIPTION
  This function start LLC operation for a particular LLE. A consequence of
  starting the LLE is starting with it the associated ARP instance.

PARAMETERS
  lle_instance: ThE Instance of the LLE to start. Note that when this
  functionis invoked, the corresponding control block is already correctly
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
);


/*===========================================================================
FUNCTION LAN_LLCI_STOP()

DESCRIPTION
  This function stops LLC operation for a particular LLE. All ARP cache
  entries related to this LLE is deleted and ARP operation for this LLE will
  be stopped.

PARAMETERS
  lle_instance: the LLE instance that should be stopped.

RETURN VALUE
  None

DEPENDENCIES
  The PS LAN LLC should have been initialized.

SIDE EFFECTS
  None
===========================================================================*/
int lan_llci_stop
(
  lan_lle_enum_type        lle_instance
);


/*===========================================================================
FUNCTION LAN_LLCI_RX_SIG_HDLER()

DESCRIPTION
  This function is a generic callback that PS LAN LLC regiters with PS task
  at the LLE RX signal handler upon starting the given LLE. When called. LAN
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
);


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
     pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that
                    needs to be transmitted.
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
  ps_iface_type*         iface_ptr,
  dsm_item_type**        pkt_chain_ptr,
  ps_tx_meta_info_type*  meta_info_ptr,
  void*                  client_info
);

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
  ps_iface_type*         iface_ptr,
  dsm_item_type**        pkt_chain_ptr,
  ps_tx_meta_info_type*  meta_info_ptr,
  void*                  client_info
);

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
  ps_iface_type*         iface_ptr,
  dsm_item_type**        pkt_chain_ptr,
  ps_tx_meta_info_type*  meta_info_ptr,
  void*                  client_info
);

/*===========================================================================
FUNCTION LAN_LLC_DEINIT()

DESCRIPTION
  This function free the resources allocated in init function

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void lan_llc_deinit
(
  void
);

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
  ethertype:     Packet Ethertype.
  dst_hw_addr:   pointer to destination hardware address.

RETURN VALUE
  Abide by the prototype of a PS IFACE Tx CMD:
     0 on success
    -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define lan_llci_send_pkt lan_llc_send_pkt

#endif /* PS_LAN_LLCI_H */

