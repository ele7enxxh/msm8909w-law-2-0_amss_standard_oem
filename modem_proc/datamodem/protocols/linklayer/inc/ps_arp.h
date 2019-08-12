#ifndef PS_ARP_H
#define PS_ARP_H
/*===========================================================================
                               P S _ ARP . H

DESCRIPTION
  Header file for the PS ARP protocol suite Interfacing functions.

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

           $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/13    rk     Removed feature mobileap.
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
01/10/11    cp     Added a new API to retrieve the static connected devs info.
07/02/10    cp     Moved arp_ipv4_cache_update API from 
                   ps_arp_ext.h back to here.
06/08/10    cp     Made arp update ipv4 cache API CMI compliant.
01/31/09    pp     CMI updates: arp_flush API moved here from ps_arp.h.
06/26/06    lyr    Added probe_ex API for clients which might wish to
                   configure ARP differently (e.g. link local address mgr)
09/27/04    hba    Creation
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "dsm.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_lan_llc.h"
/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enumeration of the possible probe type:
    ARP_UNICAST_PROBE: used mainly by DHCP client to request the transmission
    of a broadcast ARP Request.
    ARP_UNICAST_PROBE: used internaly to send a point-to-point ARP Request
    for cash refresh.
---------------------------------------------------------------------------*/
typedef enum
{
  ARP_UNICAST_PROBE,
  ARP_BROADCAST_PROBE
} arp_probe_enum_type;

/*---------------------------------------------------------------------------
  ARP Probe STATUS
---------------------------------------------------------------------------*/
typedef enum
{
  ARP_PROBE_FAILURE,
  ARP_PROBE_SUCCESS
} arp_probe_status;

/*---------------------------------------------------------------------------
 Type defintion of ARP calback prototype for ARP probing outcome notification
---------------------------------------------------------------------------*/
typedef void (*arp_cback_ptr_type)(void* user_data, arp_probe_status status);

/*---------------------------------------------------------------------------
 Type defintion of ARP conflict calback prototype for ARP to notify of an IP
 conflict at any point in time
---------------------------------------------------------------------------*/
typedef void (*arp_conflict_cback_ptr_type)(void* user_data);

/*---------------------------------------------------------------------------
Type definition of the function usedby ARP core to request transmission of a
a packet (IP SDU or ARP SDU) by LLC.
---------------------------------------------------------------------------*/
typedef int (*lan_llc_send_pkt_f_ptr_type)( lan_lle_enum_type instance,
                                            dsm_item_type**   pkt,
                                            uint16            ethertype,
                                            byte*             dst_hw_addr);

/*---------------------------------------------------------------------------
  Structure with the parameters required to configure the extended Probe
  Request (probe_ex)

  ip_addr: IP address being probed
  num_retries: number of times to attempt retry before giving up
  not_confident:
   TRUE  -> treat ARP requests for IP address being probed as probe success
            (e.g. if we are not confident that our IP address is unique)
   FALSE -> ignore probe requests during probes
            (e.g. when confident - through DHCP - that IP addr is unique)
  retry_intvl: time duration in msec between successive retries
  randomize_retry_intvl:
   TRUE  -> time between retries is a random value in
            [retry_intvl,retry_intvl+max_random_incr]
   FALSE -> time between retries is fixed at retry_intvl
  max_random_incr: delta from retry_intvl in msec to choose a random interval
                   between successive retries
                   Used ONLY if randomize_retry_intvl is TRUE
  response_wait_time: time to wait after last retry before declaring failure
---------------------------------------------------------------------------*/
typedef struct
{
  arp_probe_enum_type probe_type;
  uint32              ip_addr;
  uint32              num_retries;
  boolean             not_confident;
  uint16              retry_intvl;
  boolean             randomize_retry_intvl;
  uint16              max_random_incr;
  uint16              response_wait_time;
} arp_probe_ex_params_type;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
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
);


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
  dsm_item_type**   pkt,             /* Rx ARP frame                       */
  byte*             src_hw_addr,
  byte*             dst_hw_addr
);



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
  lan_lle_enum_type           lle_instance,
  arp_config_type*            arp_config_ptr,
  ps_iface_type*              iface_ptr,
  lan_llc_send_pkt_f_ptr_type llc_tx_f_ptr
);


/*===========================================================================
FUNCTION ARP_STOP()

DESCRIPTION
  This function stops an ARP instance associated to a particular LLE. All
  entries in the ARP cache related to this LLE are flushed and any subsequent
  ARP resolution originating for this LLE will fail.

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
);



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
  lan_lle_enum_type  lle_instance,
  uint32             dst_ip_addr,
  dsm_item_type**    ip_pkt
);

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
);

/*===========================================================================
FUNCTION ARP_PROBE()

DESCRIPTION
  This function is called to probe an IP address. It forces the generation of
  an ARP request, which may be unicast or brodcast. If a callback is supplied,
  it is called when a response is received for the ARP request.

  arp_cb_ptr() might be invoked before arp_probe returns: it is left to the
  caller to handle this

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

  ARP cache must not contain the multicast entry being probed - this e.g.
  implies that ARP cache was flushed either implicitly (via a stop) or
  explicitly (via arp_flush() before DHCP/link local address manager issue a
  probe


SIDE EFFECTS
  None
===========================================================================*/
int arp_probe
(
  ps_iface_type*      iface_ptr,
  arp_probe_enum_type probe_type,
  uint32              ip_addr,
  arp_cback_ptr_type  arp_cb_ptr,
  void*               user_data
);



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

  ARP cache must not contain the entry being probed - this e.g. implies that
  ARP cache was flushed either implicitly (via a stop) or explicitly (via
  arp_flush() before DHCP/link local address manager issue a probe

SIDE EFFECTS
  None
===========================================================================*/
int arp_probe_ex
(
  ps_iface_type*             iface_ptr,
  arp_probe_ex_params_type*  probe_params_ptr,
  arp_cback_ptr_type         arp_cb_ptr,
  void*                      user_data
);



/*===========================================================================
FUNCTION ARP_CONFLICT_CBACK_REG()

DESCRIPTION
  This function is called by a PS entity to register a callback handler to be
  notified of an IP address conflict during normal operation of ARP

  This can be used e.g. by DHCP or the link local address manager to be
  notified of any conflicts to trigger recovery mechanisms

  A NULL callback deregisters the previously registered one.

  Only one callback may be registered per PS IFACE.

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
);

/*===========================================================================
FUNCTION ARP_GET_DEST_MAC_ADDR

DESCRIPTION
  This function gets the destination hw address by calling 
  arpi_cmgr_lookup().

PARAMETERS
  arp_instance: the ARP instance to which the entry is associated.
  ipv4_addr: IPv4 address.
  dest_mac_addr: OUT parameter. Stores the result

RETURN VALUE
  0  -  on Success
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
);

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
  uint32 : returned number of statically conn devices information.
  -1     : on Failure

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
);


#endif /* PS_ARP_H */

