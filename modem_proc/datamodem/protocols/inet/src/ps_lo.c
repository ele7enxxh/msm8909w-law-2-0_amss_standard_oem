/*===========================================================================

                                  P S _ L O . C

GENERAL DESCRIPTION
  The DMSS loopback interface allows applications on the phone to
  communicate with each other via DS sockets.
  The loopback interface is implemented as a ps_iface, i.e. the
  loopback traffic traverses the complete IP stack.

EXTERNALIZED FUNCTIONS
  lo_powerup_init()
  lo_init()
  lo_tx_cmd()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  lo_powerup_init() and lo_init() should be called as part of the PS initialization.

Copyright (c) 2002-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_lo.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/12   ds      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
11/05/12   fn      Optimizing NV read for NV IPV6 ENABLED item   
02/22/11   ss      Compiler warning fixes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/01/10   sa      Added appropriate log messages before ASSERT(0).
06/24/09   am      Renamed hton/ntoh macros with ps_ prefix.
05/22/09   kk      Q6 compiler warning fixes.
09/12/08   pp      Metainfo optimizations.
05/07/07   mct     IPv6 Neighbor Discovery
02/07/07   scb     Fixed Klocworks High errors
01/29/07   hm      Split lo_init into two functions to do Q initializations
                   before PS task starts.
11/02/06   mct     Added support for dynamic IPv6 addresses.
08/18/06   mjp     Fixed IPHC and ROHC Logging
07/28/06   ks      Setting dsm item's app_ptr to NULL after using it in
                   lo_process_tx_data_q().
07/31/06   rt      Route Scope modifications.
04/16/06   mct     Loopback optimizations for IPv4/IPv6.
04/25/06    msr    L4/Tasklock code review changes
02/06/06   msr     Updated for L4 tasklock/crit sections.
12/05/05   msr     Lint fixes
11/07/05   rt      Fixed Coverity Errors.
10/12/05   mct     Don't create the v6 lo iface if v6 nv item is not enabled.
08/16/05   msr     Fixed PS_BRANCH_TASKFREE()
08/15/05   mct     Fixed naming causing some issues w/C++ compilers.
08/15/05   msr     Removed FEATURE_DATA_PS_DATA_LOGGING_FLOW
05/11/05   rt      Fixed lint errors.
05/04/05   msr     Created separate phys link for V6 loopback.
05/02/05   msr     Made default flow transparent to loopback iface.
04/26/05   vp      Fixed DST_ADDR_IS_IN rule in lo4_rt_acl.
04/18/05   mct     Added support for v6 loopback.
04/17/05   msr     Added support for default flow for loopback iface.
03/02/05   ks      Added call to ps_iface_dpl_set_iface_desc() to set the
                   iface description.
01/12/05   ks      Put call to ps_iface_dpl_support_flow_logging within
                   Flow Logging feature.
01/10/05   ks      Added  calls to flow and link logging functions.
11/11/04   msr/ks  Added support for logging IP packets.
10/31/04   msr/ks  Added call to ps_iface_support_logging() in lo_init() to
                   support data protocol logging on the loopback interface.
10/30/04   ks      Changed ptr to ptr of ps_meta_info_type to single pointer
                   in lo_tx_cmd().
10/13/04   vp      Network order conversion of IP address before being set to
                   LO_IFACE. Representation of netmask in aclrules as IP addrs.
08/10/04   mct     Fixed lint errors.
06/11/04   vp      Changes for representation of IP addresses as struct ps_in_addr.
04/30/04   vp      Code for correct handling of packets destined to itself.
03/15/04   mct     Added check to deny v6 and pass v4 for new routing updates
02/27/04   mct     Added ACL check for V4
02/10/04   rc      Changed call ip_input() to ip4_input().
02/03/04    aku    Added support for logical ifaces
01/31/04   usb     Adding Lo ACL during iface creation and not in ps init
08/15/03   aku     Added support for separating phys_link from ps_iface.
06/18/03   usb     Using new priority class ACL_LOOPBACK_CLASS for loopback
                   ACLs, so as to always prefer loopback if required.
05/27/03   om      Changed iface_create call to register ACL separately.
05/13/03   usb     Update to ACL rule names
05/02/03   usb     ps_iface_create() prototype change.
04/15/03   ss      Made the loop-back behavior asynchronous by putting the
                   data in a queue and signalling PS
02/12/03   om      Fixed lo ACL to include all local addresses.
11/01/02   om      Created Module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_tx_meta_info.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_ifacei_utils.h"
#include "ps_route.h"
#include "ps_routei_lo.h"
#include "ps_ip4.h"
#include "ps_svc.h"
#include "ps_in.h"

#include "ps_lo.h"
#include "ps_flowi.h"
#include "ps_utils.h"

#include "ps_iface_logging.h"
#include "ps_logging.h"
#include "ps_logging_defs.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "dss_ps.h"
#include "ps_ip6i_sm.h"

#include "ds_Utils_DebugMsg.h"


/*===========================================================================

                     DATA TYPES AND DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Create the v4/v6 loopback interface and required physlink.
  Create the v4/v6 loopback interface ACLs.
---------------------------------------------------------------------------*/
static ps_iface_type      lo_iface4;
static acl_type           lo4_acl;
static ps_phys_link_type  lo_phys_link;

#ifdef FEATURE_DATA_PS_IPV6
static acl_type       lo6_acl;
static ps_iface_type  lo_iface6;

/*---------------------------------------------------------------------------
  V6 loopback can't share V4 loopback's phys link because when a phys link is
  FLOW_COUPLED at most only one flow can be bound to it. If phys link is
  shared, default flows of both V4 loopback and V6 loopback ifaces will be
  bound to same phys link and we don't want to make phys link FLOW_DECOUPLED
  as loopback ifaces need not be aware of ps_flows
---------------------------------------------------------------------------*/
static ps_phys_link_type lo_phys_link_v6;
#endif /* FEATURE_DATA_PS_IPV6 */

/*---------------------------------------------------------------------------
  The loopback interface's packet counter
---------------------------------------------------------------------------*/
static uint32 lo_pkt_cnt = 0;

/*---------------------------------------------------------------------------
  Queue to put the tx data to be processed by PS
---------------------------------------------------------------------------*/
static q_type lo_tx_data_q;

/*---------------------------------------------------------------------------
  Events used for updating the loopback routing tables.
---------------------------------------------------------------------------*/
static void *ps_lo_iface_up_buf;
static void *ps_lo_iface_down_buf;
static void *ps_lo_iface_addr_changed_buf;

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION LO_PROCESS_TX_DATA_Q()

DESCRIPTION
  This function processes the protocol packets that are sent over the local
  loop-back interface. Each item of the lo_tx_data_q is an IP packet. This
  function gets a packet from the queue and loops it back by calling
  ps_iface_input() in so doing it is IPversion agnostic.

PARAMETERS
  sig: the signal that caused this functions to be called.
  user_data_ptr: not used in this case.

RETURN VALUE
  TRUE: When lo_tx_data_q is empty, clear signal from set signal mask
  FALSE: Don't clear signal yet - wait for lo_tx_data_q to be emptied out

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean lo_process_tx_data_q
(
  ps_sig_enum_type sig,
  void* user_data_ptr
)
{
  dsm_item_type* item_ptr;
  ps_iface_type* in_if_ptr;
  (void)sig;
  (void)user_data_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Get the item from the lo_tx_data_q. If the queue is empty then then ask
    for the signal to be removed.
  -------------------------------------------------------------------------*/
  item_ptr = (dsm_item_type *) q_get(&lo_tx_data_q);
  if (item_ptr == NULL)
  {
    return (TRUE);
  }

  /*-------------------------------------------------------------------------
    Check to see whether we have some interface address passed in the app_ptr
    field of the incoming item_ptr. If so then call the ip4_input on that
    interface.
  -------------------------------------------------------------------------*/
  in_if_ptr = (ps_iface_type *)(item_ptr->app_ptr);
  item_ptr->app_ptr = NULL;

  if(in_if_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL iface ptr ");
    dsm_free_packet(&item_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Log the packet on the Rx side
    -----------------------------------------------------------------------*/
    DPL_LOG_NETWORK_RX_PACKET(in_if_ptr, item_ptr, DPL_IID_NETPROT_IP);

    (void)ps_iface_input(in_if_ptr, &item_ptr, NULL);
  }

  return FALSE;
} /* lo_process_tx_data_q() */



/*===========================================================================
FUNCTION PS_LO_IFACE_EVENT_CBACK()

DESCRIPTION
  This event callback is registered by ps_lo as a global event
  callback with ps_iface. This function will add/clear entries from the
  loopback routing table based on interface state and address changes.

PARAMETERS
  this_iface_ptr: ptr to the interface on which we are operating
  event:          the event for which callbacks need to be called
  event_info:     event information that needs to be passed into callback.
  user_data_ptr:  ptr to user data info

RETURN VALUE
  None

DEPENDENCIES
  Must be called in a critical section

SIDE EFFECTS
  None
===========================================================================*/
static void ps_lo_iface_event_cback
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  ps_ip_addr_type            ip_addr;
  (void)user_data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_lo_iface_event_cback(): Invalid iface ptr %p",
                    this_iface_ptr);
    ASSERT(0);
    return;
  }

  /*------------------------------------------------------------------------
    Only ifaces with their own unique IP address are supported for
    loopback. Logical ifaces that inherit IP info from a physical iface,
    IPSEC_IFACEs, and MCAST ifaces that are only in the bcast/mcast group
    are not supported for loopback.
  ------------------------------------------------------------------------*/
  if( (PS_IFACE_IS_LOGICAL(this_iface_ptr) &&
       PS_IFACE_IP_INFO_IS_INHERITED(this_iface_ptr)) ||
      (this_iface_ptr->name == IPSEC_IFACE) ||
      (this_iface_ptr->group_mask == BCAST_MCAST_GROUP))
  {
    LOG_MSG_INFO3_1("ps_lo_iface_event_cback(): Loopback not supported on this "
                    "logical iface: 0x%p", this_iface_ptr);
    return;
  }

  LOG_MSG_INFO3_1("ps_lo_iface_event_cback(): Event passed "
                      "to loopback iface event cback: %d", event);
  switch(event)
  {
    case IFACE_UP_EV:
      /*---------------------------------------------------------------------
        An interface has just come up. Verify the iface has obtained a valid
        IP address.
      ---------------------------------------------------------------------*/
      if(ps_iface_addr_family_is_v4(this_iface_ptr))
      {
        ip_addr.type = IPV4_ADDR;
      }
      else
      {
        ip_addr.type = IPV6_ADDR;
      }

      ps_iface_get_addr(this_iface_ptr, &ip_addr);

      if( (ip_addr.type == IP_ADDR_INVALID) ||
          ((ip_addr.type == IPV4_ADDR) &&
           (ip_addr.addr.v4.ps_s_addr == 0)) ||
          ((ip_addr.type == IPV6_ADDR) &&
           (ip_addr.addr.v6.ps_s6_addr64[0] == 0) &&
           (ip_addr.addr.v6.ps_s6_addr64[1]== 0)) )
      {
        break;
      }

      ps_route_lo_update(this_iface_ptr, &ip_addr);
      break;

    case IFACE_DOWN_EV:
      /*---------------------------------------------------------------------
        The interface has gone down. Remove the iface entry from the
        appropriate loopback routing table.
      ---------------------------------------------------------------------*/
      ps_route_lo_delete(this_iface_ptr);
      break;

    case IFACE_ADDR_CHANGED_EV:
      /*---------------------------------------------------------------------
        This event only occurs for IPv4 interfaces
      ---------------------------------------------------------------------*/
      if(event_info.ip_addr.addr.v4.ps_s_addr == 0)
      {
        ps_route_lo_delete(this_iface_ptr);
        break;
      }

      ps_route_lo_update(this_iface_ptr, &event_info.ip_addr);
      break;

    default:

      break;
  }

  return;
} /* ps_lo_iface_event_cback() */



/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION LO_POWERUP_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface. This function does all the independent
  initializations without having to read NV.

DEPENDENCIES
  PS Mem, logging, PS iface and Q subsystems must be initialized.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register for iface events used to update loopback routing tables.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Alloc and register for IFACE_UP_EV
  -------------------------------------------------------------------------*/
  ps_lo_iface_up_buf =
    ps_iface_alloc_event_cback_buf(ps_lo_iface_event_cback, NULL);

  ASSERT(ps_lo_iface_up_buf != NULL);
  if(0 != ps_iface_event_cback_reg(NULL,
                                   IFACE_UP_EV,
                                   ps_lo_iface_up_buf))
  {
    LOG_MSG_ERROR_1("lo_powerup_init(): Could not reg global LO cback for evt %d",
                  IFACE_UP_EV);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Alloc and register for IFACE_DOWN_EV
  -------------------------------------------------------------------------*/
  ps_lo_iface_down_buf =
    ps_iface_alloc_event_cback_buf(ps_lo_iface_event_cback, NULL);

  ASSERT(ps_lo_iface_down_buf != NULL);
  if(0 != ps_iface_event_cback_reg(NULL,
                                   IFACE_DOWN_EV,
                                   ps_lo_iface_down_buf))
  {
    DATA_ERR_FATAL("lo_powerup_init(): Could not alloc event buf");
    return;
  }

  /*-------------------------------------------------------------------------
    Alloc and register for IFACE_ADDR_CHANGED_EV
  -------------------------------------------------------------------------*/
  ps_lo_iface_addr_changed_buf =
    ps_iface_alloc_event_cback_buf(ps_lo_iface_event_cback, NULL);

  ASSERT(ps_lo_iface_addr_changed_buf != NULL);
  if(0 != ps_iface_event_cback_reg(NULL,
                                   IFACE_ADDR_CHANGED_EV,
                                   ps_lo_iface_addr_changed_buf))
  {
    DATA_ERR_FATAL("lo_powerup_init(): Could not subscribe global LO cback for IFACE events");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize the loopback tx data queue. The tx data is put in this queue
    and processed by the PS task.
  -------------------------------------------------------------------------*/
  (void)q_init( &lo_tx_data_q );

  /*-------------------------------------------------------------------------
    Set the signal handler and enable the signal to process the tx data
  -------------------------------------------------------------------------*/
  (void) ps_set_sig_handler(PS_LO_TX_DATA_Q_SIGNAL,
                            lo_process_tx_data_q,
                            NULL);
  ps_enable_sig( PS_LO_TX_DATA_Q_SIGNAL );

  /*-------------------------------------------------------------------------
    Initialize the loopback ACL
  -------------------------------------------------------------------------*/
  lo4_acl.acl_fptr       = lo4_rt_acl;
  lo4_acl.post_proc_fptr = NULL;

  /*-------------------------------------------------------------------------
    Create the loopback interfaces. As there will never be multiple
    channels for the lookback interface only one physlink is needed.
  -------------------------------------------------------------------------*/
  (void) ps_iface_create(&lo_iface4,
                         LO_IFACE,
                         &lo4_acl,
                         NULL,
                         &lo_phys_link,
                         1);

  (void) ps_iface_dpl_support_network_logging(&lo_iface4,
                                              DPL_IID_IFNAME_LO_V4_IFACE);
  (void) ps_iface_dpl_set_iface_desc(&lo_iface4, NULL);

  (void) ps_iface_dpl_support_flow_logging(&lo_iface4,
                                           DPL_IID_IFNAME_LO_V4_IFACE);

  LOG_MSG_INFO1_2("IPv4 LO:%d iface created (0x%x)",
           lo_iface4.instance, &lo_iface4);

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Initialize the V6 loopback ACL
  -------------------------------------------------------------------------*/
  lo6_acl.acl_fptr       = lo6_rt_acl;
  lo6_acl.post_proc_fptr = NULL;

  if(-1 == ps_iface_create(&lo_iface6,
                           LO_IFACE,
                           &lo6_acl,
                           NULL,
                           &lo_phys_link_v6,
                           1))
  {
    LOG_MSG_ERROR_0("LO6 iface creation failed!");
    return;
  }

  (void) ps_iface_dpl_support_network_logging(&lo_iface6,
                                              DPL_IID_IFNAME_LO_V6_IFACE);
  (void) ps_iface_dpl_set_iface_desc(&lo_iface6, NULL);
  (void) ps_iface_dpl_support_flow_logging(&lo_iface6,
                                           DPL_IID_IFNAME_LO_V6_IFACE);

  LOG_MSG_INFO1_2("IPv6 LO:%d iface created (0x%x)",
                  lo_iface6.instance, &lo_iface6);
#endif /* FEATURE_DATA_PS_IPV6 */

  return;
} /* lo_powerup_init() */

/*===========================================================================
FUNCTION LO_INIT()

DESCRIPTION
  This function is called at powerup (PS initialization) to create
  the loopback interface. This function reads the NV to do IPv6 loopback
  iface initializations.

DEPENDENCIES
  lo_powerup_init() should be called before.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lo_init( void )
{
  ps_ip_addr_type          my_addr;
  ps_ip_addr_type          my_netmask;
  ps_ip_addr_type          my_gateway;
  ps_flow_type           * flow_ptr_v4;
  ps_flow_type           * flow_ptr_v6;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the lo iface's transmit function to loop back to IP
  -------------------------------------------------------------------------*/
  ps_iface_set_tx_function(&lo_iface4, lo_tx_cmd, NULL);
  (void) ps_iface_set_addr_family(&lo_iface4, IFACE_IPV4_ADDR_FAMILY);

  /*-------------------------------------------------------------------------
    Set the IPv4 lo iface's IP address to 127.0.0.1 and bring it up
  -------------------------------------------------------------------------*/
  my_addr.type       = IPV4_ADDR;
  my_addr.addr.v4.ps_s_addr     = ps_htonl(0x7f000001);
  if (ps_iface_set_v4_addr(&lo_iface4, &my_addr) != 0)
  {
    LOG_MSG_ERROR_0( "LO4: Set address to 127.0.0.1 failed" );
    return;
  }

  my_netmask.type    = IPV4_ADDR;
  my_netmask.addr.v4.ps_s_addr  = ps_htonl( 0xfffffff0 );
  my_gateway.type    = IPV4_ADDR;
  my_gateway.addr.v4.ps_s_addr  = ps_htonl( 0x00000000 );

  if (route_add(ROUTE_HOST,
                my_addr,
                my_netmask,
                my_gateway,
                &lo_iface4,
                1,
                NULL) != 0)
  {
    LOG_MSG_ERROR_0( "LO4: Route add failed" );
  }

  /*-------------------------------------------------------------------------
    Send flow activate, physlink up and iface up indication for lo_iface4
  -------------------------------------------------------------------------*/
  flow_ptr_v4 = PS_IFACEI_GET_DEFAULT_FLOW(&lo_iface4);

  ps_flow_bind_phys_link(flow_ptr_v4, &lo_phys_link);
  ps_flow_activate_ind(flow_ptr_v4, PS_EIC_NOT_SPECIFIED);
  ps_phys_link_up_ind(&lo_phys_link);
  ps_iface_up_ind(&lo_iface4);

  /*-------------------------------------------------------------------------
    Setup this iface for IPV6
  -------------------------------------------------------------------------*/
  ps_iface_set_tx_function(&lo_iface6, lo_tx_cmd, NULL);
  (void) ps_iface_set_addr_family(&lo_iface6, IFACE_IPV6_ADDR_FAMILY);

  /*-------------------------------------------------------------------------
    Set the IPv6 lo iface's IP address to ::1 and bring it up.
  -------------------------------------------------------------------------*/
  lo_iface6.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->prefix = 0;
  lo_iface6.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->iid =
    ps_in6addr_loopback.ps_s6_addr64[1];
  lo_iface6.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->addr_state =
    IPV6_ADDR_STATE_VALID;
  lo_iface6.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->addr_type =
    IPV6_ADDR_TYPE_PUBLIC;

  /*-------------------------------------------------------------------------
    Send flow activate, physlink up and iface up indication for lo_iface6
  -------------------------------------------------------------------------*/
  flow_ptr_v6 = PS_IFACEI_GET_DEFAULT_FLOW(&lo_iface6);

  ps_flow_bind_phys_link(flow_ptr_v6, &lo_phys_link_v6);
  ps_flow_activate_ind(flow_ptr_v6, PS_EIC_NOT_SPECIFIED);
  ps_phys_link_up_ind(&lo_phys_link_v6);
  ps_iface_up_ind(&lo_iface6);

  /*--------------------------------------------------------------------
    lo_init must be called after dssps_init in order for iPv6_enabled
    flag to be valid. The default value is TRUE
  --------------------------------------------------------------------*/
  if(FALSE == dssps_get_ipv6_enabled_setting())
  {
    ps_iface_disabled_ind(&lo_iface6);
  }

  return;
} /* lo_init() */


/*===========================================================================
FUNCTION LO_TX_CMD()

DESCRIPTION
  This function is used as transmit command for the loopback interface.
  It discards any meta info and send the IP datagramm back to the stack.

DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr     - Pointer to transmitting interface
  pkt_chain_ptr      - Reference pointer to IP datagramm
  meta_info_ptr      - Pointer to packet's meta info
  tx_cmd_info        - User data pointer (not used)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int lo_tx_cmd
(
  ps_iface_type*       this_iface_ptr,
  dsm_item_type**      pkt_chain_ptr,
  ps_tx_meta_info_type*   meta_info_ptr,
  void*                tx_cmd_info
)
{
  ps_iface_state_enum_type  iface_state;
  (void)tx_cmd_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    No meta info is defined for the loopback interface, discard it.
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_FREE( &meta_info_ptr );

  if(NULL == pkt_chain_ptr || NULL == *pkt_chain_ptr)
  {
    LOG_MSG_ERROR_0("lo_tx_cmd(): DSM packet pointer null");
    return -1;
  }

  if(!(PS_IFACE_IS_VALID(this_iface_ptr)))
  {
    LOG_MSG_ERROR_1("Pkt loopback failed, invalid iface 0x%x",
                        this_iface_ptr);
    ASSERT(0);
    dsm_free_packet(pkt_chain_ptr);
    return -1;
  }

  iface_state = ps_iface_state(this_iface_ptr);

  if(iface_state != IFACE_UP && iface_state != IFACE_CONFIGURING &&
     iface_state != IFACE_GOING_DOWN && iface_state != IFACE_ROUTEABLE)
  {
    LOG_MSG_INFO1_1("Invalid iface state %d", iface_state);
    dsm_free_packet(pkt_chain_ptr);
    return -1;
  }

  lo_pkt_cnt++;

  /*-------------------------------------------------------------------------
    Log packet on the Tx side
  -------------------------------------------------------------------------*/
  DPL_LOG_NETWORK_TX_PACKET(this_iface_ptr, *pkt_chain_ptr, DPL_IID_NETPROT_IP);

  /*-------------------------------------------------------------------------
    Put the data in the queue and signal PS to process it.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO3_2( "LO: pkt in queue 0x%x (%d)", *pkt_chain_ptr, lo_pkt_cnt );

  (*pkt_chain_ptr)->app_ptr = (void*)this_iface_ptr;
  q_put( &lo_tx_data_q, &((*pkt_chain_ptr)->link) );
  PS_SET_SIGNAL( PS_LO_TX_DATA_Q_SIGNAL );

  /*-------------------------------------------------------------------------
    Make sure the sender thinks the data is gone ...
  -------------------------------------------------------------------------*/
  *pkt_chain_ptr = NULL;

  return 0;
} /* lo_tx_cmd() */

/*===========================================================================
FUNCTION PS_GET_V4_LO_RSCOPE_IDX()

DESCRIPTION
  This function returns the V4 loopback route scope index.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The V4 loopback route scope index.

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_get_v4_lo_rscope_idx(void)
{

  return lo_iface4.iface_private.iface_index;
}

/*===========================================================================
FUNCTION PS_GET_V6_LO_RSCOPE_IDX()

DESCRIPTION
  This function returns the V6 loopback route scope index.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The V6 loopback route scope index.

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_get_v6_lo_rscope_idx(void)
{

  return lo_iface6.iface_private.iface_index;
}


/*===========================================================================

                    ACCESS CONTROL LIST DEFINITION

===========================================================================*/

ACL_DEF( lo4_rt_acl )
ACL_START
{
  DENY( IP_POL_FAMILY_IS( IFACE_IPV6_ADDR_FAMILY ) );
  /* DENY( NOT(IS_PKT_INFO_NULL() OR IP_PKT_FAMILY_IS( IFACE_IPV4_ADDR_FAMILY)));*/
  PASS( ACL_LOOPBACK_CLASS,  REQUIRED_IFACE_TYPE_IS( LO_IFACE ) );
  DENY( DST_ADDR_IS_IN( IPV4(0,0,0,0), IPV4(255,255,255,255) ) );
  PASS( ACL_LOOPBACK_CLASS,
        DST_ADDR_IS_IN(IPV4( 127, 0, 0, 0 ), IPV4(255, 255, 255, 0)));
  PASS_DEFAULT( ACL_DEFAULT_CLASS );
}
ACL_END

#ifdef FEATURE_DATA_PS_IPV6
ACL_DEF( lo6_rt_acl )
ACL_START
{
  DENY( IP_POL_FAMILY_IS( IFACE_IPV4_ADDR_FAMILY ) );
  PASS( ACL_LOOPBACK_CLASS,  REQUIRED_IFACE_TYPE_IS( LO_IFACE ) );
  PASS( ACL_LOOPBACK_CLASS, DST_ADDR6_IS_LOOPBACK() );
  PASS_DEFAULT( ACL_DEFAULT_CLASS );
}
ACL_END
#endif /* FEATURE_DATA_PS_IPV6 */
