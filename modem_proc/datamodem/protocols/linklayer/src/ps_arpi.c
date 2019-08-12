/*===========================================================================
                                 PS_ARPI.C


DESCRIPTION

    This file contains implementation of the PS ARP Internal functions.

EXTERNALIZED FUNCTIONS (But only to ARP Core)
    arpi_decode()
    arpi_ipv4_resolve()
    arpi_entry_timeout()
    arpi_stop_timer()
    arpi_build_request()
    arpi_build_response()

    arpi_cmgr_init()
    arpi_cmgr_new_entry()
    arpi_cmg_release_entry()
    arpi_cmgr_lookup()
    arpi_cmgr_flush()
    arpi_cmgr_monitor()
    arpi_cmgr_refresh_result()
    arpi_cmgr_process_timer()

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_arpi.c#1 $
$Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   rk      Removed feature mobileap.
09/12/13   sd      Cryptographic vulnerability - replacing ran_next APIs.
06/27/13   cx      Replace memcpy and memmove with memscpy and memsmove
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
09/20/11    cp     Added the support for Broadcasting GARP in periodic
                   intervals. 
05/17/11   cp      Fix to populate the static conn dev info ip addresses in host
                   byte order.
04/08/11   cp      Fix to get our ip address directly using
                   ps_iface_get_addr() API.
01/10/11   cp      Added a new API to retrieve the static connected devs info.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/19/10   sa      Made changes for graceful exit in case of dsm exhaustion.
10/14/10   am      Serialized ARP timeouts in PS_RM context for SoftAp.
10/13/10   cp      Increased maximum number of ARP cache entries to 10.
10/01/10   sa      Added appropriate log messages before ASSERT(0).
09/10/10   jee     Fixed KW warnings
09/24/09   ss      Fixed KW warnings.
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
09/12/08   pp      Metainfo optimizations.
02/07/07   scb     Fixed Klocwork High errors
11/16/06   ks      High Lint error fixes.
06/30/06   lyr     Added support for probe_ex() and conflict_cb()
06/27/06   scb     Added statistics support for the ARP module
02/21/06   lyr     Added QoS support (freeing meta info where necessary)
11/08/05   hba     Used ARP_PS_IFACE_IP_V4_ADDR in lieu of PS_IFACE_IP_V4_ADDR
11/05/05    rt     Fixed coverity errors.
04/26/05   jd      Remove invalid ASSERT at top of arpi_build_pkt()
04/26/05   jd/hba  F3's print IP addresses in host order
03/30/05   hba     Change code as to remove any network/host byte order
                   conversion.
03/21/05   ssh     Changes pursuant to the new typedef ps_timer_handle_type
10/13/04   hba     Creation.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "amssassert.h"

#include "qw.h"           /* Because I am aware of time_type being a qw    */
#include "time_svc.h"     /* For timestamp                                 */

#include "ps_lan_llc.h"
#include "ps_arp.h"
#include "ps_arpi.h"

#include "ps_stat_arp.h"

#include "ps_in.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "pstimer.h"

#include "ps_tx_meta_info.h"
#include "ran.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"

/*==========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of various constant values used by the ARP protocol
---------------------------------------------------------------------------*/
#define ARP_HRD_ETHERNET           1   /* Ethernet Hardware address space  */

#define ARP_PRO_TYPE_DOD_INTERNET  0x0800

#define ARP_OP_REQUEST             1
#define ARP_OP_RESPONSE            2

#define ARP_PLN_DOD_INTERNET       4   /* Protocol Adrress length for IPV4 */

/*--------------------------------------------------------------------------
 Standard NULL Hardware address
---------------------------------------------------------------------------*/
const byte  ARP_NULL_HW_ADDR[ARP_HLN_DOD_INTERNET] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/*---------------------------------------------------------------------------
 ARP Operational Parameters: Should latter be read from NV
---------------------------------------------------------------------------*/
STATIC uint32   arpi_num_check_retries     = 3;
STATIC sint31   arpi_cmgr_refresh_duration = 2*60*1000; /* 2 minutes       */

const uint16   arpi_default_num_arp_retries = 3;
const uint16   arpi_default_req_duration    = 2*1000 ; /* 2 seconds        */


/*---------------------------------------------------------------------------
  Type definition of an ARP frame header
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint16    hrd;      /* Hardware address space                            */
  uint16    pro;      /* Protocol address space(hardware space dependent)  */
  byte      hln;      /* byte length of each hardware address              */
  byte      pln;      /* byte length of each protocol address              */
  uint16    op;       /* opcode (ares_op$REQUEST | ares_op$REPLY)          */
}  arpi_frm_hdr_type;

/*--------------------------------------------------------------------------
  Type definition of an ARP frame
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  arpi_frm_hdr_type  arp_hdr;
  arpi_frm_body_type arp_body;
}  arpi_frm_type;


/*---------------------------------------------------------------------------
  Definition of array of ARP CB data type
---------------------------------------------------------------------------*/
arpi_lle_cb_type  arp_array[LAN_LLE_MAX];


/*---------------------------------------------------------------------------
  Standard function to build an ARP packet
---------------------------------------------------------------------------*/
STATIC dsm_item_type* arpi_build_pkt
(
  byte*  src_hw_addr,
  uint32 src_ip_adr,
  byte*  target_hw_addr,
  uint32 target_ip_addr,
  uint16 op_code
);


/*---------------------------------------------------------------------------
 Declaration of callback for ARP Request timer
---------------------------------------------------------------------------*/
STATIC void arpi_entry_timeout_cback
(
  void * user_data
);


/*---------------------------------------------------------------------------
  Local function to retrieve the delta difference between two systeme times
---------------------------------------------------------------------------*/
STATIC sint31 arpi_get_delta_time
(
  uint32 start_time,
  uint32 end_time
);

/*---------------------------------------------------------------------------
 ARP CACHE MAXIMUM NUMBE OF ENTRIES
---------------------------------------------------------------------------*/
#define ARP_CMGR_MAX_NUM_ENTRIES        10

/*---------------------------------------------------------------------------
 Type definition of an ARP entry envelop:
 WARNING: Do not change order of fields, ARP cache manager implementation is
          aware of fields order...
---------------------------------------------------------------------------*/
typedef struct
{
  arpi_entry_type entry;
  boolean         is_free;
} arpi_entry_env_type;

/*---------------------------------------------------------------------------
 The ARP cache is currently implemented as an array of entries for all
 instances
---------------------------------------------------------------------------*/
STATIC arpi_entry_env_type arp_cache[ARP_CMGR_MAX_NUM_ENTRIES];

/*---------------------------------------------------------------------------
Internal ARP entry initialization function
---------------------------------------------------------------------------*/
STATIC void arpi_init_entry(arpi_entry_type*);

/*---------------------------------------------------------------------------
 Internal function to retrieve the oldest entry with a given status
---------------------------------------------------------------------------*/
STATIC arpi_entry_type* arpi_get_oldest_entry(uint32 status);


/*---------------------------------------------------------------------------
 Definition Of ARP CMGR Timer used to periodically timeout entries
---------------------------------------------------------------------------*/
STATIC ps_timer_handle_type arpi_cmgr_timer;

/*---------------------------------------------------------------------------
 Declaration of calback for ARP Cache Manager timer
---------------------------------------------------------------------------*/
STATIC void arpi_cmgr_timeout_cback
(
  void * user_data
);


/*---------------------------------------------------------------------------
 Internal function to retrieve the timeout for the next ARP request, based on
 the parameters of the ARP entry

 timeout duration is returned in ms
---------------------------------------------------------------------------*/
STATIC uint16 arpi_get_timeout_duration(arpi_entry_type* ptr_arp_entry);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        ARP INTERNAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

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
)
{
  arpi_frm_hdr_type   arp_hdr;
  uint16              bytes_read;
  boolean             good_frm;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != arp_pkt);

  ASSERT( NULL != arp_body);

  /*-------------------------------------------------------------------------
  Retrieve and analyze the ARP header
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup(arp_pkt,
                          (void*)&arp_hdr,
                          sizeof(arpi_frm_hdr_type));


  good_frm = ( sizeof(arpi_frm_hdr_type) == bytes_read )            ?
             ( ps_ntohs(arp_hdr.hrd) == ARP_HRD_ETHERNET          &&
               ps_ntohs(arp_hdr.pro) == ARP_PRO_TYPE_DOD_INTERNET &&
               arp_hdr.hln        == ARP_HLN_DOD_INTERNET         &&
               arp_hdr.pln        == ARP_PLN_DOD_INTERNET)          : FALSE;


  if ( FALSE == good_frm)
  {
    LOG_MSG_INFO1_0("Unknown ARP header received within ARP frame");

    return ARP_UNKNOWN_FRM;
  }

  /*-------------------------------------------------------------------------
    Retrieve now the ARP body
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup(arp_pkt,
                          (void*)arp_body,
                          sizeof(arpi_frm_body_type));


  /*-------------------------------------------------------------------------
    If packet contains extra bytes then this is a bogus ARP frame !!!
  -------------------------------------------------------------------------*/
  good_frm  = ( ( sizeof(arpi_frm_body_type) == bytes_read )  ||
                (NULL != *arp_pkt) );


  if ( TRUE == good_frm )
  {
    /*-----------------------------------------------------------------------
      Return the ARP message type based on ARP operation field
    -----------------------------------------------------------------------*/
    switch (ps_ntohs(arp_hdr.op))
    {
    case ARP_OP_REQUEST:
      {
        LOG_MSG_INFO3_0("Receive an ARP Request frame");
        return ARP_REQUEST_FRM;
      }
    case ARP_OP_RESPONSE:
      {
        return ARP_RESPONSE_FRM;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Frame is bad
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_0("Receive an ARP frame with bad body content");

  return ARP_UNKNOWN_FRM;

} /* arpi_decode() */



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
)
{
  dsm_item_type*    arp_pkt;
  arpi_lle_cb_type* ptr_arpi_cb;
  uint32            req_duration;
  ps_ip_addr_type   our_ip_addr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != ptr_arp_entry);

  ASSERT(NULL != dst_hw_addr);

  if( ptr_arp_entry->arp_instance <= LAN_LLE_MIN ||
      ptr_arp_entry->arp_instance >= LAN_LLE_MAX )
  {
    DATA_ERR_FATAL("Invalid LLE");
	return -1;
  }

  ptr_arpi_cb = &arp_array[ptr_arp_entry->arp_instance];

  if (NULL == ptr_arpi_cb || NULL == ptr_arpi_cb->iface_ptr)
  {
    DATA_ERR_FATAL("ARP control block pointer is NULL or its iface pointer is NULL");
    return -1;
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
                              ptr_arp_entry->ip_addr );

  if (NULL == arp_pkt)
  {
    LOG_MSG_ERROR_0("Fail to build ARP Request");
    return -1;
  }


  /*-------------------------------------------------------------------------
    - Decrement the number of retries
    - Start the ARP request timer...
  -------------------------------------------------------------------------*/
  ptr_arp_entry->nb_retries -= 1;

  req_duration = (uint32) arpi_get_timeout_duration( ptr_arp_entry );

  if( (0 < req_duration) &&
      (PS_TIMER_FAILURE == ps_timer_start(ptr_arp_entry->timer_handle,
                                          req_duration)) )
  {
    /*-----------------------------------------------------------------------
      Should not happen: lack of resource to properly perform ARP Resolution!
      We shouldn't even bother to send the ARP request!
    -----------------------------------------------------------------------*/
    DATA_ERR_FATAL("Impossible to start ARP Request timer");
    dsm_free_packet(&arp_pkt);
    return -1;
  }

  /*-------------------------------------------------------------------------
   Prepare for ARP packet transmission
  -------------------------------------------------------------------------*/
  arp_pkt->app_ptr = NULL;
  if ( ptr_arpi_cb->llc_tx_f_ptr( ptr_arp_entry->arp_instance,
                                  &arp_pkt,
                                  LAN_ETHERTYPE_ARP,
                                  dst_hw_addr) < 0 )
  {
    DATA_ERR_FATAL("Error transmitting ARP frame");
    dsm_free_packet(&arp_pkt);
    return -1;
  }

  /*-------------------------------------------------------------------------
   We're done... ARP Request was sent... Now wait for response!
  -------------------------------------------------------------------------*/
  return 0;

} /* arpi_ipv4_resolve() */



/*===========================================================================
FUNCTION ARPI_STOP_TIMER()

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Use PS Timer service to stop the timer
   ------------------------------------------------------------------------*/
  if ( TRUE == ps_timer_is_running(timer_handle) )
  {
    if( PS_TIMER_SUCCESS == ps_timer_cancel(timer_handle) )
    {
      return;
    }
    else
    {
      LOG_MSG_ERROR_1("Cannot stop PS Timer %d", timer_handle);
    }
  }
  else
  {
    LOG_MSG_INFO3_1("PS Timer %d not running", timer_handle);
  }
} /* arpi_stop_timer() */


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
  arpi_entry_type* ptr_arp_entry
)
{
  byte*              dst_hw_addr;
  arpi_instance_type arp_instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != ptr_arp_entry);


  arp_instance = ptr_arp_entry->arp_instance;

  /*-------------------------------------------------------------------------
    Make sure this a valid entry!
  -------------------------------------------------------------------------*/
  if(arp_instance >= LAN_LLE_MAX || arp_instance <= LAN_LLE_MIN)
  {
    DATA_ERR_FATAL("arpi_entry_timeout(): Invalid LLE");
    return ;
  }


  /*-------------------------------------------------------------------------
    An ARP entry times out in three cases:
     - Either because there is a pending resolution, in which case, based on
       the number of retries a new request is sent or the entry invalidated

     -Either because there is an on-going probing, in which case based on the
      number of retries a new request is sent or the entry invalidated and
      the arp callback invoked.

     - Either because there is an on-going checking in which case, based on
      the number of retries the a new request is sent or the entry is
      invalidated
  -------------------------------------------------------------------------*/

  if ( 0 < ptr_arp_entry->nb_retries)
  {
    /*-----------------------------------------------------------------------
      We should post a new  ARP request.
      But destination address depend on the purpose of the request:
        - If Initial resolution, this should be a broadcast request.
        - if broadcast probing this is also a broadcast request
        - if checking this is a unicast request
    -----------------------------------------------------------------------*/
    dst_hw_addr = (byte*)LAN_IEEE_MAC_BROADCAST_ADDR;

    if ( ptr_arp_entry->status & ARP_STATUS_CHECKING)
    {
      /*---------------------------------------------------------------------
        Point-to-point ARP Request
      ---------------------------------------------------------------------*/
      dst_hw_addr = ptr_arp_entry->hw_addr;
    }

    if ( arpi_ipv4_resolve(ptr_arp_entry, dst_hw_addr) < 0)
    {
      LOG_MSG_ERROR_0("Failed to retry ARP Request");
      /*---------------------------------------------------------------------
       Proceed on behaving as if the entry times out for the maximum number
       of retries...
      ---------------------------------------------------------------------*/
      if(ptr_arp_entry->status & ARP_STATUS_CHECKING)
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_ucast_prb_fail_rexmit, 1);
      }
      else if(ptr_arp_entry->status & ARP_STATUS_PROBING)
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_bcast_prb_fail_rexmit, 1);
      }
      else
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_bcast_fail_rexmit, 1);
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        We're fine: ARP Request retransmitted successfully!
      ---------------------------------------------------------------------*/
      if(ptr_arp_entry->status & ARP_STATUS_CHECKING)
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_ucast_prb_rexmit, 1);
      }
      else if(ptr_arp_entry->status & ARP_STATUS_PROBING)
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_bcast_prb_rexmit, 1);
      }
      else
      {
        ARP_INC_INSTANCE_STATS(arp_instance, req_bcast_rexmit, 1);
      }
      ARP_INC_INSTANCE_STATS(arp_instance, pkts_tx, 1);
      LOG_MSG_INFO2_1("ARP request retransmitted for entry of IP (0x%x)",
                      ps_ntohl(ptr_arp_entry->ip_addr));

      /*---------------------------------------------------------------------
        In case entry is not required to wait for ARP response, we should
        cleanup the ARP entry and notify clients

        Currently only expected on an ARP ANNOUNCE
      ---------------------------------------------------------------------*/
      if ( ( 0 == ptr_arp_entry->nb_retries ) &&
           ( 0 == ptr_arp_entry->response_wait_time ) )
      {
        ASSERT( ARP_STATUS_PROBING & ptr_arp_entry->status );
        arpi_prep_entry_deactivation( ptr_arp_entry );
        arpi_cmgr_release_entry(ptr_arp_entry);
      }

      return;
    }
  }

  /*-------------------------------------------------------------------------
    We're done with the retries;
    Prepare to invalide the ARP entry
   ------------------------------------------------------------------------*/

  if ( ptr_arp_entry->status & ARP_STATUS_RES_PEND)
  {
    ps_tx_meta_info_type** tmp_ps_mi_ptr;

    LOG_MSG_INFO1_1("Failed to receive ARP Response after maximum retries"
                    "Freeing pending packet (next hop IP is 0x%x)",
                    ps_ntohl(ptr_arp_entry->ip_addr));
    /*-----------------------------------------------------------------------
      There should be a pending packet
    -----------------------------------------------------------------------*/

    ASSERT(NULL != ptr_arp_entry->pending_pkt);
    ASSERT(NULL != ptr_arp_entry->pending_pkt->app_ptr);

    tmp_ps_mi_ptr =
      ( (ps_tx_meta_info_type**) (&(ptr_arp_entry->pending_pkt->app_ptr)) );

    PS_TX_META_INFO_FREE( tmp_ps_mi_ptr );

    dsm_free_packet(&ptr_arp_entry->pending_pkt);
    ARP_INC_INSTANCE_STATS(arp_instance, arp_req_fail, 1);
  }

  if ( ptr_arp_entry->status & ARP_STATUS_PROBING)
  {
    /*-----------------------------------------------------------------------
      Invoke Call back
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO3_1("ARP Probe failed for IP(0x%x)! Notifying upper layer",
                    ps_ntohl(ptr_arp_entry->ip_addr));


    ptr_arp_entry->arp_cback(ptr_arp_entry->arp_cback_data,
                             ARP_PROBE_FAILURE);
    ARP_INC_INSTANCE_STATS(arp_instance, arp_probe_timeout, 1);
  }

  if ( ptr_arp_entry->status & ARP_STATUS_CHECKING)
  {
    /*-----------------------------------------------------------------------
      Peer Node is not reachable anymore
      We just have to get rid of this entry
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_1("Peer node of IP(0x%x) failed to repond to Unicast ARP request"
                    " and Corresponding entry will be deactivated in ARP Cache"
					,ps_ntohl(ptr_arp_entry->ip_addr));

    ARP_INC_INSTANCE_STATS(arp_instance, ce_evict_timeout, 1);
  }

  /*-------------------------------------------------------------------------
   Deactivate this entry now
  -------------------------------------------------------------------------*/
  arpi_cmgr_release_entry(ptr_arp_entry);

} /* arp_entry_timeout() */


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
  The dsm item containing the ARP frame is build based on arpi_src_pkt.
SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* arpi_build_request
(
  byte*  src_hw_addr,
  uint32 src_ip_addr,
  byte*  target_hw_addr,
  uint32 target_ip_addr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return arpi_build_pkt(src_hw_addr,
                        src_ip_addr,
                        target_hw_addr,
                        target_ip_addr,
                        ARP_OP_REQUEST);
} /* arpi_build_request() */



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
SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* arpi_build_response
(
  byte*            rsp_hw_addr,
  uint32           rsp_ip_addr,
  byte*            req_hw_addr,
  uint32           req_ip_addr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return arpi_build_pkt(rsp_hw_addr,
                        rsp_ip_addr,
                        req_hw_addr,
                        req_ip_addr,
                        ARP_OP_RESPONSE);

} /* arpi_build_response() */



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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != ptr_arp_entry);

  if ( ptr_arp_entry->status & ARP_STATUS_RES_PEND)
  {
    ps_tx_meta_info_type** tmp_ps_mi_ptr;

    /*-----------------------------------------------------------------------
      There should be a pending packet
    -----------------------------------------------------------------------*/
    ASSERT(NULL != ptr_arp_entry->pending_pkt);
    ASSERT(NULL != ptr_arp_entry->pending_pkt->app_ptr);

    tmp_ps_mi_ptr =
      ( (ps_tx_meta_info_type**) (&(ptr_arp_entry->pending_pkt->app_ptr)) );

    PS_TX_META_INFO_FREE( tmp_ps_mi_ptr );

    dsm_free_packet(&ptr_arp_entry->pending_pkt);

    /*-----------------------------------------------------------------------
      Stop ARP request timer
    -----------------------------------------------------------------------*/
    arpi_stop_timer(ptr_arp_entry->timer_handle);
  }

  if ( ptr_arp_entry->status & ARP_STATUS_PROBING)
  {
    /*-----------------------------------------------------------------------
      Stop probing timer and invoke callback
    -----------------------------------------------------------------------*/
    arpi_stop_timer(ptr_arp_entry->timer_handle);

    ptr_arp_entry->arp_cback(ptr_arp_entry->arp_cback_data,
                             ARP_PROBE_FAILURE);
  }

  if ( ptr_arp_entry->status & ARP_STATUS_CHECKING)
  {
    /*-----------------------------------------------------------------------
      Stop ARP request timer
    -----------------------------------------------------------------------*/
    arpi_stop_timer(ptr_arp_entry->timer_handle);
  }

} /* arpi_prep_entry_deactivation() */

/*===========================================================================
FUNCTION ARPI_GET_SYS_TIME

DESCRIPTION
  This function is called to retieve the lower 32 bits of the current system
  time which is on 64 bits

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  time service module should have been initialized.

SIDE EFFECTS
  None
===========================================================================*/
uint32 arpi_get_sys_time
(
  void
)
{
  qword time;      /* time is a qword                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(time, 0, sizeof(time));
  /*-------------------------------------------------------------------------
    Retrieve the current system time in millisecondes!
  -------------------------------------------------------------------------*/
  time_get_ms(time);


  /*-------------------------------------------------------------------------
   Time comparison is in the order of 2 minutes = (2*60*1000ms) = 120000 ms
   so only the lower 32 bits of the qword is of interest for me!
  -------------------------------------------------------------------------*/
  return qw_lo(time);

} /* arpi_get_sys_time() */

/*===========================================================================
FUNCTION ARPI_GET_DELTA_TIME

DESCRIPTION
  This function is called to compute the duration between a start_time and
  an end_time.

PARAMETERS
  start_time: lower 32 bits of a system time.
  end_time: upper 32 bits of a system time.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define  ARP_HBA_MAX_DELTA_TIME     120000
#define  ARP_HBA_B                  0xFFFFFFFF

STATIC sint31 arpi_get_delta_time
(
  uint32 start_time,
  uint32 end_time
)
{
  uint32 abs_delta;
  sint31 delta;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Theorem (see HBAHINI for proof).
      Let's t1, t2 be positive integers so that |t2-t1| <= DELTA_MAX
      By choosing B such that B > DELTA_MAX, we only need
      Lo(t1) and Lo(t2) respectively the remainders of the euclidien
      division of t1 and t2 by B to compute t2-t1.
      In such conditions:
        -  if |Lo(t2) - Lo(t1)| <= DELTA_MAX then
              t2-t1 = Lo(t2) - Lo(t1)

        - if |Lo(t2) - Lo(t1)| > DELTA_MAX then
              - if Lo(t2) < Lo(t1) t2-t1 = B - [Lo(t1) - Lo(t2)]

              - if Lo(t2) > Lo(t1) t2-t1 = [Lo(t2) - Lo(t1)] - B

    The following implementation applied the theorem for
        DELTA_MAX = 120000
        B = 0xFFFFFFFF + 1
        This choice of B allows t1 and t2 to be on 32 bits...
  -------------------------------------------------------------------------*/

  abs_delta = (end_time > start_time) ? end_time   - start_time
              : start_time - end_time  ;

  if ( abs_delta <= ARP_HBA_MAX_DELTA_TIME)
  {
    /*-----------------------------------------------------------------------
      No overflow between the two time
    -----------------------------------------------------------------------*/
    delta = end_time - start_time;
  }
  else
  {
    /*-----------------------------------------------------------------------
      There is overflow
     ----------------------------------------------------------------------*/
    delta = (end_time > start_time) ? (end_time - start_time - 1) - ARP_HBA_B
            : ARP_HBA_B - (start_time - end_time + 1);
  }

  return delta;

} /* arpi_get_delta_time() */

/*===========================================================================

                              ARP Cache Manager

===========================================================================*/


/*===========================================================================
FUNCTION ARPI_CMGR_INIT()

DESCRIPTION
  This function is called by the ARP Core to initialize the ARP Cache Manager

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
)
{
  uint8 loop;
  arpi_entry_env_type * ptr_entry_env;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    - Initialize all entries to free
   ------------------------------------------------------------------------*/
  memset(arp_cache, 0, ARP_CMGR_MAX_NUM_ENTRIES*sizeof(arpi_entry_env_type));

  for (loop =0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];
    ptr_entry_env->is_free = TRUE;
  }

  LOG_MSG_INFO2_0("ARP CMGR properly initialized");

} /* arpi_cmgr_init() */


/*===========================================================================
FUNCTION ARPI_CMGR_RELEASE_ENTRY()

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
)
{
  arpi_entry_env_type * ptr_entry_env;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != ptr_arp_entry);

  /*-------------------------------------------------------------------------
   Recover entry envelop
   ------------------------------------------------------------------------*/
  ptr_entry_env = (arpi_entry_env_type*)( ptr_arp_entry) ;

  if ( TRUE == ptr_entry_env->is_free)
  {
    DATA_ERR_FATAL("Attempt to deactivate a free ARP entry");
    return;
  }

  LOG_MSG_INFO3_1("Deallocating ARP entry (0x%x)", ptr_arp_entry);

  /*-------------------------------------------------------------------------
    Free associated timer
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_free(ptr_arp_entry->timer_handle);
  if (PS_TIMER_SUCCESS != timer_ret_val)
  {
    LOG_MSG_ERROR_0("arpi_cmgr_release_entry: Releasing associated timer failed.");
  }

  ptr_entry_env->is_free = TRUE;

  memset(ptr_arp_entry, 0, sizeof(arpi_entry_type));

} /* arpi_cmgr_release_entry() */


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
)
{
  uint8 loop;
  ps_timer_handle_type  timer_res;
  arpi_entry_env_type * ptr_entry_env;
  arpi_entry_type*      ptr_arp_entry;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (loop =0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];

    if (TRUE == ptr_entry_env->is_free)
    {
      LOG_MSG_INFO2_0("Allocating new ARP entry");

      ptr_arp_entry = &ptr_entry_env->entry;

      /*---------------------------------------------------------------------
        Allocate a new PS Timer
      ---------------------------------------------------------------------*/
      timer_res = ps_timer_alloc(arpi_entry_timeout_cback,
                                 (void*) ptr_arp_entry);

      if (PS_TIMER_INVALID_HANDLE == timer_res)
      {
        DATA_ERR_FATAL("Failed to allocate PS timer for ARP entry");
        return NULL;
      }

      ptr_arp_entry->timer_handle = timer_res;

      arpi_init_entry(ptr_arp_entry);

      return ptr_arp_entry;
    }
  }

  /*-------------------------------------------------------------------------
    If arrive here no entry is free; if the caller force the new entry
    allocation then we deactivate the oldest entry and returns it with the
    following constraints:
     - An entry in probing state is never force
       to deactivate.
     - the better candidate for deactivation is an
       entry which is being cheched.
     - the next one is the oldest for which resolution
       has been successfull
     - the last one is the oldest for which resolution
      is pending
  -------------------------------------------------------------------------*/
  if (TRUE == force)
  {
    /*-----------------------------------------------------------------------
     Look up the oldest entry in checking status
    -----------------------------------------------------------------------*/
    ptr_arp_entry = arpi_get_oldest_entry(ARP_STATUS_CHECKING);

    /*-----------------------------------------------------------------------
      Did we find an entry?
    -----------------------------------------------------------------------*/
    if ( NULL != ptr_arp_entry)
    {
      /*---------------------------------------------------------------------
        Entry found! Stop associated timer!
      ---------------------------------------------------------------------*/
      arpi_stop_timer(ptr_arp_entry->timer_handle);

      /*---------------------------------------------------------------------
        No need to reallocate a new timer!
        Just re-initialize entry
      ---------------------------------------------------------------------*/
      arpi_init_entry(ptr_arp_entry);

      ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance, ce_evict_new_req, 1);
      return ptr_arp_entry;
    }

    /*-----------------------------------------------------------------------
     Lookup the oldest entry for which resolution is done
    -----------------------------------------------------------------------*/
    ptr_arp_entry = arpi_get_oldest_entry(ARP_STATUS_RES_DONE);

    /*-----------------------------------------------------------------------
      Did we find an entry?
    -----------------------------------------------------------------------*/
    if ( NULL != ptr_arp_entry)
    {
      /*---------------------------------------------------------------------
        Entry found! No need to reallocate a new timer
        Just Re-initialize entry
      ---------------------------------------------------------------------*/
      arpi_init_entry(ptr_arp_entry);

      ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance, ce_evict_new_req, 1);
      return ptr_arp_entry;
    }

    /*-----------------------------------------------------------------------
     Lookup the oldest entry for which resolution is pending
    -----------------------------------------------------------------------*/
    ptr_arp_entry = arpi_get_oldest_entry(ARP_STATUS_RES_PEND);

    /*-----------------------------------------------------------------------
      Did we find an entry?
    -----------------------------------------------------------------------*/
    if ( NULL != ptr_arp_entry)
    {
      ps_tx_meta_info_type** tmp_ps_mi_ptr;

      /*---------------------------------------------------------------------
        Entry found
        There should be a pending packet...
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO2_1("Freeing waiting packet (next hop IP 0x%x)",
                      ps_ntohl(ptr_arp_entry->ip_addr));

      ASSERT(NULL != ptr_arp_entry->pending_pkt);
      ASSERT(NULL != ptr_arp_entry->pending_pkt->app_ptr);

      tmp_ps_mi_ptr =
        ( (ps_tx_meta_info_type**) (&(ptr_arp_entry->pending_pkt->app_ptr)) );

      PS_TX_META_INFO_FREE( tmp_ps_mi_ptr );

      dsm_free_packet(&ptr_arp_entry->pending_pkt);

      ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance, ipv4_drop_ce_flush, 1);
      arpi_stop_timer(ptr_arp_entry->timer_handle);

      arpi_init_entry(ptr_arp_entry);

      ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance, ce_evict_new_req, 1);
      return ptr_arp_entry;
    }

  } // End if force

  return NULL;

} /* arpi_cmgr_new_entry() */



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
)
{
  arpi_lle_cb_type*    ptr_arpi_cb;
  uint8                loop;
  arpi_entry_env_type* ptr_entry_env;
  arpi_entry_type*     ptr_arp_entry;
  boolean              restart_timer;
  uint32               cur_time;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Loop through the Cache manager table:
   For each entry do:
    If cache refresh feature is enabled for the ARP
    instance associated with entry do:
     if entry is currenty been checked
      then timeout entry and deactivate it
     else
      start point to point checking for entry
   If at least one entry is beeing checked, restart
   ARP Cache Manager timer
  -------------------------------------------------------------------------*/
  restart_timer = FALSE;

  for (loop = 0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];
    ptr_arp_entry = &ptr_entry_env->entry;

    if (FALSE ==  ptr_entry_env->is_free)
    {
      /*---------------------------------------------------------------------
        This is an active entry
        Lookup ARP CB associated with entry
      ---------------------------------------------------------------------*/
      ptr_arpi_cb = &arp_array[ptr_arp_entry->arp_instance];

      if ( ptr_arpi_cb->flags & ARP_CACHE_REFRESH_ENABLE &&
           ptr_arp_entry->status & ARP_STATUS_RES_DONE)
      {
        restart_timer = TRUE;

        /*-------------------------------------------------------------------
          Start point to point checking for this entry if the entry has not
          been updated for more than arp_cmgr_refresh_duration
        -------------------------------------------------------------------*/
        cur_time = arpi_get_sys_time();

        if ( arpi_get_delta_time(ptr_arp_entry->update_timestamp,
                                 cur_time) > arpi_cmgr_refresh_duration )
        {
          ptr_arp_entry->nb_retries = arpi_num_check_retries;

          if ( arpi_ipv4_resolve(ptr_arp_entry,
                                 ptr_arp_entry->hw_addr) >= 0)
          {
            ptr_arp_entry->update_timestamp = cur_time;
            ptr_arp_entry->status |= ARP_STATUS_CHECKING;
            ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance, pkts_tx, 1);
            ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance,
                                   req_ucast_prb_tx,
                                   1);
          }
          else
          {
            ARP_INC_INSTANCE_STATS(ptr_arp_entry->arp_instance,
                                   req_ucast_prb_fail_tx,
                                   1);
          }

        }

      }// End ARP_CACHE_REFRESH_ENABLE

    } // End ptr_entry_env->is_free == FALSE

  } // End for loop

  /*-------------------------------------------------------------------------
    Restart the cache refresh timer but only if resolution is done for at
    least one entry for which Cache refresh feature is enable...
  -------------------------------------------------------------------------*/
  if ( TRUE == restart_timer)
  {
    if (PS_TIMER_FAILURE == ps_timer_start(arpi_cmgr_timer,
                                           arpi_cmgr_refresh_duration))
    {
      DATA_ERR_FATAL("Failed to start ARP Cache Manager timer");
      return;
    }
  }
} /* arpi_cmgr_process_timer() */



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
)
{
  uint8                loop;
  arpi_entry_env_type* ptr_entry_env;
  arpi_entry_type*     ptr_arp_entry;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ptr_arp_entry = NULL;

  for ( loop = 0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];

    if ( FALSE == ptr_entry_env->is_free)
    {
      ptr_arp_entry = &ptr_entry_env->entry;

      if ( ptr_arp_entry->arp_instance == arp_instance &&
           ptr_arp_entry->ip_addr == ip_addr)
      {
        LOG_MSG_INFO3_1("Found in cache entry of IP (0x%x)", ip_addr);
        return ptr_arp_entry;
      }
    }
  } // end loop for

  return NULL;

} /* arpi_cmgr_lookup() */



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
)
{
  uint8                loop;
  arpi_entry_env_type* ptr_entry_env;
  arpi_entry_type*     ptr_arp_entry;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ptr_arp_entry = NULL;

  for ( loop = 0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];

    if ( FALSE == ptr_entry_env->is_free)
    {
      ptr_arp_entry = &ptr_entry_env->entry;

      if ( ptr_arp_entry->arp_instance == arp_instance )
      {
        /*-------------------------------------------------------------------
          Found the entry to flush
          Prepare to deactivate entry
        -------------------------------------------------------------------*/
        arpi_prep_entry_deactivation(ptr_arp_entry);

        /*-------------------------------------------------------------------
          Deactivate this entry now
        -------------------------------------------------------------------*/
        arpi_cmgr_release_entry(ptr_arp_entry);

      } // End If ptr-arp_entry->arp_instance == arp_instance

    } // End if ptr_entry_env->is_free == FALSE

  } // End loop for

} /* arp_cmg_flush() */




/*===========================================================================
FUNCTION ARPI_CMGR_MONITOR

DESCRIPTION
  This function is called by the ARP Core to request cache consistency
  monitoring for a particular entry. The ARP Cache performs cache consistency
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
)
{
  static boolean cmgr_timer_allocated = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != ptr_arp_entry);

  ptr_arp_entry->update_timestamp = arpi_get_sys_time();

  /*-------------------------------------------------------------------------
    Allocate and start cache manager refresh timer if we've not already done
    so.
  -------------------------------------------------------------------------*/
  if ( FALSE == cmgr_timer_allocated)
  {
    arpi_cmgr_timer = ps_timer_alloc(arpi_cmgr_timeout_cback,
                                     (void*)arpi_cmgr_refresh_duration );

    if ( PS_TIMER_INVALID_HANDLE == arpi_cmgr_timer)
    {
      DATA_ERR_FATAL("Failed to allocate ARP Cache Manager timer");
      return;
    }

    cmgr_timer_allocated = TRUE;
  }

  /*-------------------------------------------------------------------------
   If we're here then cache manager timer has been allocated Start it now if
   not running...
  -------------------------------------------------------------------------*/
  if ( FALSE == ps_timer_is_running(arpi_cmgr_timer))
  {
    LOG_MSG_INFO1_0("Starting Cache Manager timer for cache refresh");

    if (PS_TIMER_FAILURE == ps_timer_start(arpi_cmgr_timer,
                                           arpi_cmgr_refresh_duration))
    {
      DATA_ERR_FATAL("Failed to start ARP Cache Manager timer");
      return;
    }
  }

} /* arpi_cmgr_monitor() */



/*===========================================================================
FUNCTION ARPI_CMGR_REFRESH_RESULT

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
  arpi_entry_type* ptr_arp_entry,
  byte*            rsp_hw_addr
)
{
  /*-------------------------------------------------------------------------
    Request timeout timer was already stopped see arp_input()
  -------------------------------------------------------------------------*/
  ASSERT( NULL != ptr_arp_entry);

  ASSERT( NULL != rsp_hw_addr );

  memscpy(ptr_arp_entry->hw_addr,
         LAN_IEEE_MAC_ADDR_LEN,
         rsp_hw_addr,
         ARP_HLN_DOD_INTERNET);

  /*-------------------------------------------------------------------------
    Remove checking flag...
  -------------------------------------------------------------------------*/
  ptr_arp_entry->status &= (~ARP_STATUS_CHECKING);

  ptr_arp_entry->update_timestamp = arpi_get_sys_time();

} /* arpi_cmgr_refresh_result() */



/*===========================================================================

                              LOCAL FUNCTIONS DEFINITON

===========================================================================*/
/*===========================================================================
FUNCTION ARPI_BUILD_PKT()

DESCRIPTION
  This function is called by the ARP Core to build an ARP Request.

PARAMETERS
  src_hw_addr: pointer to source hardware address.
  src_ip_addr: pointer to source IP address.
  target_hw_addr: pointer to target hardware address. If null assume target
                  is of broadcast type.
  target_ip_addr: target Ip address.
  op_code       : The ARP opcode

RETURN VALUE
  Pointer to dsm_item_type containing ARP frame.

DEPENDENCIES
  arp_init should habe been called.
  The dsm item containing the ARP frame is build based on arpi_src_pkt.
SIDE EFFECTS
  None
============================================================================*/
STATIC dsm_item_type* arpi_build_pkt
(
  byte*  src_hw_addr,
  uint32 src_ip_addr,
  byte*  target_hw_addr,
  uint32 target_ip_addr,
  uint16 op_code
)
{
  arpi_frm_type  arp_frm;
  uint16         bytes_write;
  dsm_item_type*  arpi_src_pkt = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(NULL != src_hw_addr);
  ASSERT(NULL != target_hw_addr);

  /*-------------------------------------------------------------------------
    Construct the ARP FRAME
  -------------------------------------------------------------------------*/
  arp_frm.arp_hdr.hrd = ps_htons(ARP_HRD_ETHERNET);
  arp_frm.arp_hdr.pro = ps_htons(ARP_PRO_TYPE_DOD_INTERNET);
  arp_frm.arp_hdr.hln = ARP_HLN_DOD_INTERNET;
  arp_frm.arp_hdr.pln = ARP_PLN_DOD_INTERNET;
  arp_frm.arp_hdr.op  = ps_htons(op_code);

  memscpy((void*)arp_frm.arp_body.sender_hw_addr,
         LAN_IEEE_MAC_ADDR_LEN,
         src_hw_addr,
         ARP_HLN_DOD_INTERNET);

  arp_frm.arp_body.sender_ip_addr = src_ip_addr;

  memscpy((void*) arp_frm.arp_body.target_hw_addr,
         LAN_IEEE_MAC_ADDR_LEN,
         target_hw_addr,
         ARP_HLN_DOD_INTERNET);
  arp_frm.arp_body.target_ip_addr = target_ip_addr;

  /*-------------------------------------------------------------------------
   Push now ARP content in dsm item
  -------------------------------------------------------------------------*/
  bytes_write = dsm_pushdown_packed(&arpi_src_pkt,
                                    (void*)&arp_frm,
                                    sizeof(arpi_frm_type),
                                    DSM_DS_SMALL_ITEM_POOL);

  if ( sizeof(arpi_frm_type) == bytes_write)
  {
    return arpi_src_pkt;
  }

  LOG_MSG_ERROR_0("Failed to push down ARP request frame content in dsm item");
  dsm_free_packet(&arpi_src_pkt);

  return NULL;

} /* arpi_build_pkt() */



/*===========================================================================
FUNCTION ARPI_INIT_ENTRY()

DESCRIPTION
  This function internally initialize an ARP entry.

PARAMETERS
  ptr_arp_entry: pointer to ARP entry to initialize.

RETURN VALUE
  None.

DEPENDENCIES
  arp_cmgr_init should habe been called.

SIDE EFFECTS
  None
===========================================================================*/
void arpi_init_entry
(
  arpi_entry_type* ptr_arp_entry
)
{
  arpi_entry_env_type * ptr_entry_env;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ptr_entry_env = (arpi_entry_env_type*)ptr_arp_entry;

  ptr_entry_env->is_free = FALSE;

  ptr_arp_entry->used_timestamp = arpi_get_sys_time();

  /*-------------------------------------------------------------------------
   Initialize by default the number of retries to the one for a standart ARP
   Request.
  --------------------------------------------------------------------------*/
  ptr_arp_entry->nb_retries = arpi_default_num_arp_retries;

  ptr_arp_entry->randomize_retry_intvl = FALSE;

  ptr_arp_entry->retry_intvl = arpi_default_req_duration;

  ptr_arp_entry->max_random_incr = 0;

  ptr_arp_entry->response_wait_time = arpi_default_req_duration;

  ptr_arp_entry->status = ARP_STATUS_INIT;
} /* arpi_init_entry() */



/*===========================================================================
FUNCTION ARPI_GET_OLDEST_ENTRY()

DESCRIPTION
  This function returns the oldest entry in ARP cache with a given status

PARAMETERS
  status: uint32 holding the status that the entry should match.

RETURN VALUE
  Pointer to ARP entry

DEPENDENCIES
  arp_cmgr_init should habe been called.
  SIDE EFFECTS
  None
===========================================================================*/
STATIC arpi_entry_type* arpi_get_oldest_entry
(
  uint32 status
)
{
  uint8                loop;
  arpi_entry_env_type* ptr_entry_env;
  arpi_entry_type*     ptr_arp_entry;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ptr_arp_entry = NULL;

  for ( loop = 0; loop < ARP_CMGR_MAX_NUM_ENTRIES; loop +=1)
  {
    ptr_entry_env = &arp_cache[loop];

    if ( ptr_entry_env->entry.status & status)
    {
      if (NULL == ptr_arp_entry)
      {
        ptr_arp_entry = &ptr_entry_env->entry;
      }
      else
      {
        if ( arpi_get_delta_time(ptr_entry_env->entry.used_timestamp,
                                 ptr_arp_entry->used_timestamp) > 0 )
        {
          ptr_arp_entry = &ptr_entry_env->entry;
        }
      }
    }
  } // end loop for

  return ptr_arp_entry;

} /* arpi_get_oldest_entry() */


/*===========================================================================
FUNCTION ARPI_ENTRY_TIMEOUT()

DESCRIPTION
  This function is called when the timer associated with an ARP entry times
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
STATIC void arpi_entry_timeout_cback
(
  void* user_data
)
{
  /*-------------------------------------------------------------------------
   This is just a trampoline to arpi_entry_timeout! Invoke it !!!
  -------------------------------------------------------------------------*/
  arpi_entry_timeout((arpi_entry_type*)user_data);
} /* arpi_entry_timeout_cback() */




/*===========================================================================
FUNCTION ARPI_CMGR_TIMEOUT_CBACK()

DESCRIPTION
  This function is called when the Cache Manager refresh timer times out.

PARAMETERS
  arp_entry_ptr: pointer to the ARP entry.

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
STATIC void arpi_cmgr_timeout_cback
(
  void* user_data
)
{
  /*-------------------------------------------------------------------------
   I've chosen user data to be the duration of the cache refresh timer to
   check in case PS Timer SVC returns garbage...
  -------------------------------------------------------------------------*/
  sint31 data = (sint31)(user_data);

  ASSERT(data == arpi_cmgr_refresh_duration);

  arpi_cmgr_process_timer();

} /* arpi_cmgr_timeout_cback() */

/*===========================================================================
FUNCTION ARPI_GET_TIMEOUT_DURATION()

DESCRIPTION
  This function returns the duration to be used for the next ARP request
  transmitted

  If the last retry, the response_wait_time is returned

  If not last retry:
    - If not randomized, the min_retry_interval of the ARP entry is returned

    - If randomized, a random number in the min and max retry intervals is
      returned

PARAMETERS
  ptr_arp_entry: pointer to the ARP entry on which the timeout is to be
  computed

RETURN VALUE
  uint32 containing the duration for which to set the timeout in ms

DEPENDENCIES
  arp_cmgr_init should habe been called.
  SIDE EFFECTS
  None
===========================================================================*/
STATIC uint16 arpi_get_timeout_duration
(
  arpi_entry_type* ptr_arp_entry
)
{
  uint16               duration;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  duration = 0;

  if( 0 == ptr_arp_entry->nb_retries )
  {
    duration = ptr_arp_entry->response_wait_time;
  }
  else if( FALSE == ptr_arp_entry->randomize_retry_intvl )
  {
    duration = ptr_arp_entry->retry_intvl;
  }
  else
  {
    ASSERT(0 < ptr_arp_entry->max_random_incr);

    duration = (uint16) ps_utils_generate_16bit_rand_num_by_range(
                         ptr_arp_entry->retry_intvl,
                         ptr_arp_entry->retry_intvl +
                         ptr_arp_entry->max_random_incr);
  }

  return duration;

} /* arpi_get_timeout_duration() */

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

)
{
  uint32                loop;
  uint32               static_devs_count = 0;
  arpi_entry_env_type* ptr_entry_env;
  arpi_entry_type*     ptr_arp_entry;
  uint32               max_static_conn_devs;
  dhcp_server_conn_devices_info_s *static_conn_dev = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  max_static_conn_devs = num_conn_devs - num_dhcp_alloc_devices;
  ptr_arp_entry = NULL;

  /* Starting point to add the static conn devices */
  static_conn_dev = conn_devs + num_dhcp_alloc_devices;

  /* Go through the ip addresses related to particular arp instance in the
   * arp cache and add all the statically allocated addresses to the
   * conn_devs list. LOGIC here is for all the static devices internal_entry 
   * variable will be set to FALSE.
   */

  for ( loop = 0;
        ( loop < ARP_CMGR_MAX_NUM_ENTRIES ) && 
        ( static_devs_count < max_static_conn_devs ); /* Check whether memory is available or not */
        loop +=1)
  {
     ptr_entry_env = &arp_cache[loop];
  
     if ( FALSE == ptr_entry_env->is_free)
     {
       ptr_arp_entry = &ptr_entry_env->entry;
  
       if ( ptr_arp_entry->arp_instance == arp_instance && 
            ptr_arp_entry->internal_entry == FALSE )
       {
         /* Add the entry to conn_devs list as it is statically allocated ip address */
         memset( static_conn_dev, 0,
                sizeof(dhcp_server_conn_devices_info_s));
         static_conn_dev->client_ip.ps_s_addr = ps_ntohl(ptr_arp_entry->ip_addr);
         static_conn_dev->client_hw_len = LAN_IEEE_MAC_ADDR_LEN;
         memscpy( static_conn_dev->client_hw,
                  DHCP_SERVER_MGR_CLIENT_CHADDR_LEN,
                  ptr_arp_entry->hw_addr, 
                  LAN_IEEE_MAC_ADDR_LEN);
         static_conn_dev->is_static = TRUE;
         static_devs_count++;
         static_conn_dev++;
       }
     }
  } 
  return static_devs_count;
} /* arpi_get_static_conn_devices_info */


