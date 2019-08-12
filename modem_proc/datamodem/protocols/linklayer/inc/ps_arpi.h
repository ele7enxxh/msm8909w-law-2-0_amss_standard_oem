#ifndef PS_ARPI_H
#define PS_ARPI_H
/*===========================================================================
                               PS_ARPI . H

DESCRIPTION
  Header file for the PS ARP protocol Internal data structures.

Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_arpi.h#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/29/11    cp     Fix to check the validity of Iface before we get the ip
                   address from it.
10/14/11    ss     Fixed ARP_PS_IFACE_IP_V4_ADDR to return address in 
                   routeable state as well.
09/20/11    cp     Added the support for Broadcasting GARP in periodic
                   intervals. 
01/10/11    cp     Added a new API to retrieve the static connected devs info.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
06/30/06    lyr    Added support for probe_ex() and conflict_cb()
06/26/06    scb    Added statistics support to the ARP functionality.
11/08/05    ks     fixed compile error.
11/08/05    hba    Changed PS_IFACE_IP_V4_ADDR  into ARP_PS_IFACE_IP_V4_ADDR
                   to return true IFACE IP address only if iface is up.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
09/27/04    hba    Creation
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#include "dsm.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_lan_llc.h"
#include "ps_arp.h"
#include "pstimer.h"


/*---------------------------------------------------------------------------
  Note: this was added as a replacement to directly invoking the PS_IFACE
  macro to deal with an RMNET issue
  i.e. when using RMNET, returning the IP address when the iface was not in
  UP state was causing ARPs to fail and breaking RMNET
---------------------------------------------------------------------------*/
#define ARP_PS_IFACE_IP_V4_ADDR(_iface_ptr)                                \
        ((PS_IFACE_IS_VALID(_iface_ptr) &&                                 \
        ((_iface_ptr)->iface_private.state == IFACE_UP ||                  \
        (_iface_ptr)->iface_private.state == IFACE_ROUTEABLE )) ?          \
                                 (PS_IFACE_IP_V4_ADDR(_iface_ptr)) : 0)


/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
 Standard NULL Hardware address
---------------------------------------------------------------------------*/
#define ARP_HLN_DOD_INTERNET       6   /* Hardware Address lenght for IPV4 */
extern const byte  ARP_NULL_HW_ADDR[ARP_HLN_DOD_INTERNET];

/*---------------------------------------------------------------------------
STRUCT ARPI_LLE_CB_TYPE:
    There is an one-to-one mapping between an LLE and an ARP instance. The
    following data structure describes the configuration info of an ARP
    instance.
-----------------------------------------------------------------------------*/
typedef lan_lle_enum_type arpi_instance_type;


#define ARP_PROXY_ENABLE          0x01
#define ARP_CACHE_REFRESH_ENABLE  0x02
#define ARP_BCAST_GARP_ENABLE  0x04

#define DEF_GARP_BCAST_INTERVAL 60*1000 /* 60 seconds */

typedef struct
{
  arpi_instance_type             arp_instance;
  ps_iface_type*                 iface_ptr;
  uint32                         flags;
  uint32                         garp_bcast_interval;
  uint32                         garp_timer_handle;
  arp_query_proxy_f_ptr_type     proxy_query_f_ptr;
  lan_llc_send_pkt_f_ptr_type    llc_tx_f_ptr;
  arp_conflict_cback_ptr_type    conflict_cback;
  void*                          conflict_cback_user_data;

  struct  arp_cb_stats
  {
    uint32  ipv4_req_tx                 ;
    uint32  ipv4_drop_new_req_tx        ;
    uint32  ipv4_drop_ce_flush          ;
    uint32  pkts_tx                     ;
    uint32  req_bcast_tx                ;
    uint32  req_bcast_prb_tx            ;
    uint32  req_ucast_prb_tx            ;
    uint32  req_bcast_rexmit            ;
    uint32  req_bcast_prb_rexmit        ;
    uint32  req_ucast_prb_rexmit        ;
    uint32  arp_req_fail                ;
    uint32  arp_probe_timeout           ;
    uint32  resp_tx                     ;
    uint32  req_bcast_fail_tx           ;
    uint32  req_bcast_prb_fail_tx       ;
    uint32  req_ucast_prb_fail_tx       ;
    uint32  req_bcast_fail_rexmit       ;
    uint32  req_bcast_prb_fail_rexmit   ;
    uint32  req_ucast_prb_fail_rexmit   ;
    uint32  resp_fail_tx                ;
    uint32  pkts_rx                     ;
    uint32  req_rx                      ;
    uint32  resp_rx                     ;
    uint32  succ_resp_pending_rx        ;
    uint32  unknown_pkts_rx             ;
    uint32  not_for_us_rx               ;
    uint32  no_req_pending_rx           ;
    uint32  cache_hits                  ;
    uint32  cache_misses                ;
    uint32  arp_req_cache_hits          ;
    uint32  arp_req_cache_misses        ;
    uint32  ce_evict_timeout            ;
    uint32  ce_evict_new_req            ;
    uint32  ce_evict_prb_succ           ;
    uint32  ce_evict_ps_ent_clear       ;
    uint32  ce_update_arp_req           ;
    uint32  ce_update_check_succ        ;
    uint32  ce_update_ps_ent_set        ;
    uint32  queried                     ;
  } arp_i_stats;

} arpi_lle_cb_type;


extern arpi_lle_cb_type  arp_array[LAN_LLE_MAX];

/*---------------------------------------------------------------------------
  Possible flags for arp entry status
---------------------------------------------------------------------------*/
#define ARP_STATUS_INIT        0x01
#define ARP_STATUS_RES_PEND    0x02
#define ARP_STATUS_RES_DONE    0x04
#define ARP_STATUS_MONITORABLE 0x08
#define ARP_STATUS_CHECKING    0x10
#define ARP_STATUS_PROBING     0x20

/*---------------------------------------------------------------------------
STRUCT ARPI_ENTRY_TYPE
   This structure defines an entry in the ARP cache.
---------------------------------------------------------------------------*/
typedef struct
{
  arpi_instance_type  arp_instance;
  uint32              used_timestamp;
  uint32              update_timestamp;
  uint32              status;
  byte                hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  uint32              ip_addr;
  dsm_item_type*      pending_pkt;
  arp_cback_ptr_type  arp_cback;
  void*               arp_cback_data;
  ps_timer_handle_type  timer_handle;/* PS Timer handle to timeout the entry */
  uint32              nb_retries;
  boolean             not_confident;
  uint16              retry_intvl;
  boolean             randomize_retry_intvl;
  uint16              max_random_incr;
  uint16              response_wait_time;
  boolean             internal_entry;

} arpi_entry_type;


/*---------------------------------------------------------------------------
  Type definition of possible ARP frame types
---------------------------------------------------------------------------*/
typedef enum
{
  ARP_UNKNOWN_FRM,
  ARP_REQUEST_FRM,
  ARP_RESPONSE_FRM
} arpi_frm_type_enum;


/*---------------------------------------------------------------------------
  Type definition of the body of an ARP frame
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte   sender_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  uint32 sender_ip_addr;
  byte   target_hw_addr[LAN_IEEE_MAC_ADDR_LEN];
  uint32 target_ip_addr;
} arpi_frm_body_type;


/*---------------------------------------------------------------------------
 ARP Operational Parameters: Should later be read from NV
---------------------------------------------------------------------------*/
extern const uint16   arpi_default_num_arp_retries;
extern const uint16   arpi_default_req_duration;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION ARPI_DECODE()

DESCRIPTION
  This function decodes an ARP Frame.

PARAMETERS
  arp_pkt: pointer to chain of DSM items containing ARP packet.
  arp_body: data struture that should be filled with the ARP acket
            decoded IE.

RETURN VALUE
    The type of ARP frame that has been decode (REQUEST or RESPONSE)

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

arpi_frm_type_enum arpi_decode
(
  dsm_item_type**      arp_pkt,
  arpi_frm_body_type*  arp_body
);


/*===========================================================================
FUNCTION ARPI_IPV4_RESOLVE()

DESCRIPTION
  This function is called internally by the ARP Core to start ARP resolution
  for an IP address which is missing from cache.

PARAMETERS
  ptr_arp_entry: pointer to the ARP entry that needs to be resolved
  dst_hw_addr:  Hardware adress of destination where to send packet.

RETURN VALUE
    -1 if error
     0 if sucessful.
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
int arpi_ipv4_resolve
(
  arpi_entry_type* ptr_arp_entry,
  byte*            dst_hw_addr
);



/*===========================================================================
FUNCTION ARPI_IPV4_RESOLVE()

DESCRIPTION
  This function to stop an timer started within the ARP module providing
  its handle.

PARAMETERS
  timer_handle: self_explanatory.

RETURN VALUE
  None

DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
void arpi_stop_timer
(
  ps_timer_handle_type timer_handle
);


/*===========================================================================
FUNCTION ARPI_ENTRY_TIMEOUT()

DESCRIPTION
  This function is called when the timer associated with an ARP entry times
  out.

PARAMETERS
  arp_entry_ptr: pointer to the ARP entry.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_entry_timeout
(
  arpi_entry_type* arp_entry_ptr
);


/*===========================================================================
FUNCTION ARPI_BUILD_REQUEST()

DESCRIPTION
  This function is called by the ARP Core to build an ARP Request.

PARAMETERS
  src_hw_addr: pointer to source hardware address.
  src_ip_addr: pointer to source IP address.
  target_hw_addr: pointer to target hardware address. If null assume target
                  is of broadcast type.
  target_ip_addr: target Ip address.

RETURN VALUE
  Pointer to dsm_item_type containing ARP frame.

DEPENDENCIES
  arp_init should habe been called.
  The dsm item containing the ARP frame is build based on arpi_src_req_pkt
  will have been properly initialized.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* arpi_build_request
(
  byte*  src_hw_addr,
  uint32 src_ip_adr,
  byte*  target_hw_addr,
  uint32 target_ip_addr
);



/*===========================================================================
FUNCTION ARPI_BUILD_RESPONSE()

DESCRIPTION
  This function is called by the ARP Core to build an ARP response.


PARAMETERS
  rsp_hw_addr: pointer to ARP response hardware address
  rsp_ip_addr: ARP response IP address.
  req_hw_addr: pointer to source hardware address.
  req_ip_addr: source IP address

RETURN VALUE
  Pointer to dsm_item_type containing ARP frame.

DEPENDENCIES
  arp_init should habe been called.
  The dsm item containing the ARP farme is build based on arpi_src_rsp_pkt
  will should have been properly initialized.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* arpi_build_response
(
  byte*            rsp_hw_addr,
  uint32           rsp_ip_addr,
  byte*            req_hw_addr,
  uint32           req_ip_addr
);




/*===========================================================================
FUNCTION ARPI_PREP_ENTRY_DEACTIVATION()

DESCRIPTION
  This function is called to prepare an entry before deactivation. It is only
  called for entry for which a specific ARP procedure has been started such
  as ARP Request, ARP probing or ARP Checking...

PARAMETERS
  ptr_arp_entry: pointer to ARP entry to deactivate.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_prep_entry_deactivation
(
  arpi_entry_type* ptr_arp_entry
);

/*===========================================================================

                              ARP Cache Manager

===========================================================================*/


/*===========================================================================
FUNCTION ARPI_CMGR_INIT()

DESCRIPTION
  This function is called by the ARP Core to initialize the ARP Cache
  Manager.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_cmgr_init
(
   void
);



/*===========================================================================
FUNCTION ARPI_CMGR_RELEASE_ENTRTY()

DESCRIPTION
  This function is called by the ARP Core to deactivate a particular entry.
  The entry i re-initialize and re-cycled within the ARP cache.

PARAMETERS
  ptr_arp_entry: pointer to ARP entry to deactivate.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_cmgr_release_entry
(
  arpi_entry_type* ptr_arp_entry
);



/*===========================================================================
FUNCTION ARPI_CMGR_NEW_ENTRY()

DESCRIPTION
  This function is called by the ARP Core to retrieve a new entry. Currently
  the ARP Cache Manager is using the LR: if the cache is full, the oldest
  entry is intrnally deactivated and delivered to the calle as the new entry.

PARAMETERS
  force: Force the deactivation of an entry if Cache is full

RETURN VALUE
  Pointer to ARP new entry

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
arpi_entry_type* arpi_cmgr_new_entry
(
  boolean force
);



/*===========================================================================
FUNCTION ARPI_CMGR_PROCESS_TIMER()

DESCRIPTION
  This function is called when the cache consistency timer expires. The cache
  consistency maintenance algorithm is then performed by Cache Manager.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_cmgr_process_timer
(
   void
);



/*===========================================================================
FUNCTION ARPI_CMGR_LOOKUP

DESCRIPTION
  This function is called by the ARP Core to lookup an entry.

PARAMETERS
  arp_instance: the ARP instance to which the entry is associated.
  ip_addr: the entry IP address.

RETURN VALUE
  Pointer to looked up entry or NULL if no entry is found in cache.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
arpi_entry_type* arpi_cmgr_lookup
(
  arpi_instance_type arp_instance,
  uint32             ip_addr
);

/*===========================================================================
FUNCTION ARPI_CMGR_FLUSH

DESCRIPTION
  This function is called by the ARP Core to deactivate all te entries
  associated to a particular ARP instance.

PARAMETERS
  arp_instance: the ARP instance to which the entry is associated.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_cmgr_flush
(
  arpi_instance_type arp_instance
);



/*===========================================================================
FUNCTION ARPI_CMGR_MONITOR

DESCRIPTION
  This function is called by the ARP Core to request cache consistency
  monitoring for a particular entry. The ARP Cache performs cache consistence
  only for entries for which it has been requested.

PARAMETERS
  Pointer to ARP entry which should be monitored.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void arpi_cmgr_monitor
(
  arpi_entry_type* ptr_arp_entry
);

/*===========================================================================
FUNCTION ARPI_CMGR_MONITOR

DESCRIPTION
  This function is called to submit the result of an prior checking request
  to the Cache Manager.

PARAMETERS
  ptr_arp_entry: Pointer to ARP entry which should be monitored.
  rsp_hw_addr: hardware address of the responding node.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void arpi_cmgr_refresh_result
(
  arpi_entry_type* ptr_arpi_entry,
  byte*            rsp_hw_addr
);


/*===========================================================================
FUNCTION ARPI_GET_SYS_TIME

DESCRIPTION
  Retrive the lower 32 bits of the absolute system time for timestamp.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 arpi_get_sys_time
(
  void
);

/*===========================================================================
FUNCTION ARPI_GET_STATIC_CONN_DEVICES_INFO()

DESCRIPTION
  This function gets the devices information which are statically configured.

PARAMETERS
  lan_llc_instance       - the ARP instance to which the devices are 
                           associated.
  conn_devs              - Contains information about the connected devices.
  num_conn_devs          - contains the total required number of connected 
                           devices information.
  num_dhcp_alloc_devices - contains the number of conn devices allocated 
                           through DHCP.

RETURN VALUE
  uint32: returned number of statically conn devices information.

DEPENDENCIES
  arp_cmgr_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
uint32 arpi_get_static_conn_devices_info
(
  arpi_instance_type                arp_instance,
  dhcp_server_conn_devices_info_s * conn_devs,
  uint32                            num_conn_devs,
  uint32                            num_dhcp_alloc_devices

);
#endif /* PS_ARPI_H */
