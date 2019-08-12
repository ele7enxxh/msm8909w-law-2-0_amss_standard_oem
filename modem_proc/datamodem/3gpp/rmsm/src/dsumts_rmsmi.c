/*===========================================================================
                          D S U M T S _ R M S M I . C 

DESCRIPTION
  The Data Services UMTS specific RM State Machine functions 
  used for pdp-ip/pdp-ppp calls which are used by the state machine(s) 
  for both pdp-ip and pdp-ppp calls.
  
  NOTE:
  This file contains functions which are private to DSUMTS_RMSM and 
  should not be used by the clients of DSUMTS_RMSM directly.
 
EXTERNALIZED FUNCTIONS
Copyright (c) 2002-2016 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/src/dsumts_rmsmi.c#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/03/16    skc    Adding support for ESC SEQUENCE for PS Data call.
12/02/15    sd     Fixed crash issue due to incorrect SM instance for PDP PPP 
                   type DUN call.
11/21/12    ss     Fixed the issue of DUN calls failing in DSDS/DSDA.
08/07/12    nd     SIO port not freed after DUN is ended in entitlement check.
06/19/12    nd     MT PDP pkt call.
07/25/12    nd     Reset ppp_info.valid_fields before filling for new call.
03/22/12    nd     RCINIT changes.
02/15/12    nd     Setting Authentication info in Activate PDP context request.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
09/19/11    ss     Fixed usage of incorrect fc interface to free cmd buffers.
07/14/11    ttv    Removed F3 messages.
05/23/11    asn    Support for new event for active Ifaces
04/01/11    ttv    Support for Q6 free floating.
02/16/11    ttv    Added DSDS changes for consolidated profile family.
08/31/10    sa     Added support for default pdp type as IPv4v6.
08/18/10    ss     Compiler warning fixes.
07/09/10    asn    Support for Arbitration
07/12/10    sch    Added ioctl handler for A2 autonomous mode HDLC and PPP
                   header addition 
06/25/10    vk     Increased rx dne and added support for limiting rx pkt sz 
02/15/10    sa     Added dsumts_rmsm_is_dun_call().
02/05/10    ss     Initialize DSM watermark queues with dsm_queue_init ().
03/08/10    am     meta_info freed in sio_tx_ppp_data().
08/18/09    ars    Capability for run time disabling of data aggregation timer
08/11/09    ca     Fixed Compiler Warnings.
07/10/09    asn    Allow PPP Rx when in WAITING_FOR_IPCP_CFG state
09/27/08    pp     Metainfo Optimizations.
04/22/08    vrk    Flushing Tx only for USB Modem.
04/14/08    vrk    Flush TX queue for small pkts with HS USB aggregation.
04/12/07    vd     Moved init of total_rcv_cnt outside Feature.
02/11/07    vk     Fixed RVCT 2.1 compiler warnings.
02/01/07    vk     Watermark level changes for HSUPA.
11/30/06    ar     Select automatic mode on ds3g_siolib_allocate_port().
09/06/06    rsl    Added 2 new commands PHYS_LINK_FLOW_ENABLE and IFACE_FLOW_ENABLE.
08/15/06    asen   dial str is now picked from NV_DATA_PKT_ORIG_STR_I (if set)
                   for embedded calls and defaults to *98#. For TE calls user input is used.
08/03/06    rsl    Removed unused IOCTL call.
06/09/06    vk      A round of Lint fixes.
06/07/06    aw      Removed the tasklocks since they are called by route_get
                   function.
05/25/06    rsl       Added setting of bring_up in policy_info so that the in_use
                   flag is set correctly. 
08/11/05    rr        Changed signature of dsumts_rmsmi_ppprm_event_handler_cb 
                   as per PS TIP.
06/16/05    mv      Increased the SIO Tx WM counts for HSDPA under FEATURE_HSDPA. 
                   The hi_watermark has been increased from 6000 to 30000 and 
                   don't exceed from 10500 to 35500.
05/11/05    rr        Fixed Lint errors/warnings.
04/13/05    rr        Changed dsumts_rmsmi_handle_atd_on_rm to save ATCOP 
                   profile family in profile_id.
02/18/05    ks       Added call to ps_iface_dpl_set_iface_desc() for logging.
01/12/05    ks       Changed calls to Data Protocol Logging functions.
11/22/04    rr        Set higher order byte of profile number to DS_UMTS_PDP_
                   ATCOP_PROFILE_FAMILY to access profiles from 
                   "pdp_profile/atcop_profiles" directory
11/21/04    rc        Added code to handle the ps phys link event cback  
                   mechanism for Um interface for enable and disable events.
10/28/04    ks/msr Added logging support in dsumts_rmsmi_init_all_instances()
10/14/04    ar       Ensure SIO port allocated for external data call.
09/27/04    jd       Use ppp dependent PPP_RM_RX_SIGNAL instead of PS task
                   dependent PS_PPP_RM_RX_SIGNAL
09/10/04    sb      September PS development changes
06/04/04    rc       Fixed Lint errors/warnings.
05/06/04    vsk     Fix for not setting the ps_ppp_rm_rx_signal signal 
                   when Um interface is coming up
04/19/04    rc       Removing the flow control check before setting the
                   watermark levels. Now The software and hardware flow
                   controls can both have same watermark levels.
04/12/04    rc       Renamed acl_ipfilter_info_type to ip_pkt_info_type and
                   wrapped it around FEATURE_NEW_PKT_INFO_DEF.  
03/19/04    vsk     de-register SIO callbacks if the call origination fails.
02/06/04    vsk     Use profile_id instead of context_id 
02/03/04    aku    Added support for logical ifaces
01/31/04    usb    No need to disable Um iface ACL, routing takes care of 
                   exclusive use of iface in laptop call.
01/27/04    vsk    M-PDP support 
08/15/03    mvl    Updates for separation of phys_link.
07/24/03    vsk    Pass pdp profile number instead of profile ptr & adhere to
                   routing layer interface changes
05/23/03    ttl/rc Changed Tx Watermark counts. 
05/14/03    usb    Modified policy info to request a UMTS iface only while
                   doing a routing lookup for system selection.
05/07/03    vsk    Use the new ps_iface_create interface
04/22/03    vsk    code review cleanup
04/18/03    vsk    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#define FEATURE_DSM_WM_CB

#include "memory.h"
#include "err.h"
#include "msg.h"
#include "queue.h"
#include "sys.h"
#include "rcinit_rex.h"

#include "dstask.h"
#include "dstaski.h"
#include "ps_iface.h"
#include "ds3gsiolib.h"
#include "rdevmap.h"    
#include "ds_flow_control.h"
#include "ds3gcfgmgr.h"
#include "ps_ppp.h"
#include "ps_acl.h"
#include "ps_route.h"
#include "ps_route_iface.h"
#include "nv_items.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)
#include "dsumts_rmsmi.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#include "dsumts_rmsm_ppp.h"
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_nv_manager.h"
#include "dsumts_rmsm_ip.h"
#include "ps_iface_logging.h"
#include "ps_logging_defs.h"
#include "ps_meta_info.h"

#ifdef FEATURE_DATA_TE_MT_PDP

#include <stringl/stringl.h>
#include "dsumtspsmthdlr.h"
#include "dsumtspdpreg.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/

#include "ds_3gppi_utils.h"

#include "dsatctab.h"
#define DEFAULT_RMSM_AGG_TIMER_TIMEOUT 10
#define RMSM_AGG_TIMER_NV_INDEX 1

#ifdef FEATURE_QXDM_WM_TRACING
#include "ps_iface.h"
#include "ps_phys_link_ioctl.h"
#include "fc_wm.h"
#endif

#ifdef FEATURE_DUAL_SIM
#include "dsumts_subs_mgr.h"
#endif /* FEATURE_DUAL_SIM */

//#include "ps_routei.h"

#include "fc.h"
#include "ps_ppp_ipcp.h"
#include "ps_byte.h"
#include "ps_ppp_netmodel.h"
#include "dsumtspdpreg.h"
#include "ps_ppp_fsm.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_3gpp_rmsm_sio_rt_acl.h"
#include "ps_route_iface.h"
#include "ds_system_heap.h"
#include "ds3gsiolib_ex.h"
#include "dsati.h"
#include "ds_3gpp_profile_cache_hdlr.h"

/*===========================================================================

                           EXTERNAL DECLARATIONS

===========================================================================*/
/* RMSM UL watermark levels configurable */
  uint32 DSUMTS_RMSM_UL_LO        = 15000;
  uint32 DSUMTS_RMSM_UL_HI        = 25000;
  uint32 DSUMTS_RMSM_UL_DNE       = 58000;

/* RMSM DL watermark levels configurable */
#ifdef FEATURE_WCDMA_3C_HSDPA
  uint32 DSUMTS_RMSM_DL_LO         = 144000;
  uint32 DSUMTS_RMSM_DL_HI         = 360000;
  uint32 DSUMTS_RMSM_DL_DNE        = 450000;
#else
  uint32 DSUMTS_RMSM_DL_LO         = 50000;
  uint32 DSUMTS_RMSM_DL_HI         = 75000;
  uint32 DSUMTS_RMSM_DL_DNE        = 108000;
#endif /* FEATURE_WCDMA_3C_HSDPA */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/* Global variable set by trace. If the CPU usage is above this value, it
will trigger Data Aggregation*/
uint16 cpu_high_threshold_for_DA = 60; //eg 60
uint16 cpu_low_threshold_for_DA = 50; //eg 50
uint16 cpu_off_threshold =30;  //eg 30

static timer_type smd_notify_delay_timer[DSUMTS_RMSM_MAX_RM_DEV_INSTANCES];
/* Timeout for smd_notify_delay_timer, read from nv in 
   dsumts_rmsmi_setup_sio_watermarks */
static uint8 ds_smem_tx_delay = 0; 

static boolean fc_reg = FALSE;

/* RMSM-FC command Q for commands from FC */
q_type rmsm_fc_cmd_q = {{NULL}};
/*---------------------------------------------------------------------------
  The control block of the Generic RM state machine.
---------------------------------------------------------------------------*/
static dsumts_rmsmi_info_type* dsumts_rmsmi_info[DSUMTS_RMSM_MAX_RM_DEV_INSTANCES] = {NULL};

LOCAL dsat_mode_enum_type dsumts_rmsmi_at_escape_sequence_handler( void );

LOCAL void 
dsumts_rmsmi_at_return_to_online_data_handler
(
  /* Pointer to the watermark for data from SIO */
  dsm_watermark_type     **from_sio_wm_ptr,   

  /* Pointer to the watermark for data to SIO. */
  dsm_watermark_type     **to_sio_wm_ptr,
  
  /* Pointer to SIO RX mode-specific handler function */
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr
);

LOCAL void dsumts_rmsmi_at_return_to_online_data_complete_handler ( void );


LOCAL ds3g_siolib_func_tbl_type dsumts_rmsmi_sio_func_table =
{
  dsumts_rmsmi_at_escape_sequence_handler, /* AT escape seq handler*/
  NULL, /* DTR changed sig handler */
  NULL, /* DTR when AT&D = 0 handler */
  dsumts_rmsmi_at_return_to_online_data_handler,
  /* return to online data handler */
  dsumts_rmsmi_at_return_to_online_data_complete_handler,
  /* return to online data complete handler */
  NULL, /* return to online cmd mode complete handler */
  NULL, /* auto answer handler */
};

#ifdef FEATURE_DATA_WCDMA_PS
/*---------------------------------------------------------------------------
  TCB pointer for BCDB task
---------------------------------------------------------------------------*/
static rex_tcb_type* ds_tcb_ptr = NULL; 
#endif /* FEATURE_DATA_WCDMA_PS */

/*---------------------------------------------------------------------------
  HS_USB AGGREGATION flush if packet < packet size.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#ifdef FEATURE_HS_USB
#define HS_USB_FLUSH_PACKET_SIZE 100
static uint16 hs_usb_flush_packet_size = 0;
#endif /* FEATURE_HS_USB */
#endif/* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

/*---------------------------------------------------------------------------
  Enable chopping of rx packets into bite sized fragments before delivery
  to PPP/HDLC. This ensures an upper bound on the amount of data buffer 
  required on Um side before flow control takes effect.
---------------------------------------------------------------------------*/
#define SPLIT_RX_PACKETS

/*---------------------------------------------------------------------------
  Default maximum size of packet injected into PPP/HDLC
---------------------------------------------------------------------------*/
#define MAX_RX_PACKET_SZ    5000

#ifdef SPLIT_RX_PACKETS

/*---------------------------------------------------------------------------
  Variable to hold maximum allowed rx packet size
---------------------------------------------------------------------------*/
static uint16 dsumts_rmsmi_rx_pkt_sz = MAX_RX_PACKET_SZ;

/*---------------------------------------------------------------------------
  DSM packet pending delivery to PPP/HDLC
---------------------------------------------------------------------------*/
static dsm_item_type * dsumts_rmsmi_pending_rx_pkt_ptr[DSUMTS_RMSM_MAX_RM_DEV_INSTANCES] = {NULL}; 

/*---------------------------------------------------------------------------
  Helper macro to get pending rx packet ptr
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_GET_PENDING_RX_PKT(rm_dev) (dsumts_rmsmi_pending_rx_pkt_ptr[rm_dev])

/*---------------------------------------------------------------------------
  Helper macro to set pending rx packet ptr
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_SET_PENDING_RX_PKT(pkt_ptr,rm_dev) \
            (dsumts_rmsmi_pending_rx_pkt_ptr[rm_dev] = (pkt_ptr))

/*---------------------------------------------------------------------------
  Helper macro to free pending rx packet
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_FREE_PENDING_RX_PKT(rm_dev) \
        { \
          if (dsumts_rmsmi_pending_rx_pkt_ptr[rm_dev]) \
          { \
            dsm_free_packet(&dsumts_rmsmi_pending_rx_pkt_ptr[rm_dev]); \
          } \
        }

/*---------------------------------------------------------------------------
  Helper macro to get max rx packet size
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_GET_RX_PKT_SZ() (dsumts_rmsmi_rx_pkt_sz)

/*---------------------------------------------------------------------------
  Helper macro to set max rx packet size (currently unused)
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_SET_RX_PKT_SZ(pkt_sz) \
            (dsumts_rmsmi_rx_pkt_sz = (pkt_sz))

void dsumts_rmsmi_set_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  DSUMTS_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  rmsmi_info_ptr->rx_data_can_be_pulled = TRUE;
  DSUMTS_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
}

void dsumts_rmsmi_reset_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  DSUMTS_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  rmsmi_info_ptr->rx_data_can_be_pulled = TRUE;
  DSUMTS_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
}

boolean dsumts_rmsmi_get_rx_data_can_be_pulled_flag 
(
  dsumts_rmsmi_info_type* rmsmi_info_ptr
)
{
  boolean flag;

  DSUMTS_ENTER_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);
  flag = rmsmi_info_ptr->rx_data_can_be_pulled;
  DSUMTS_LEAVE_CRIT_SECTION(&rmsmi_info_ptr->rx_data_crit_sec);

  return flag;
}

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_NEXT_RX_PACKET()

  DESCRIPTION
    This function returns a pointer to the next rx packet for delivery to 
    the upper layer (PPP/HDLC). If necessary, packet is chopped to guarantee
    packet does not exceed maximum allowed packet size. 

  PARAMETERS
    rx_wm : DSM watermark ptr of queue holding incoming rx packets
    rm_dev: Rm Device Instance

  RETURN VALUE
    Ptr to dsm packet chain

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsm_item_type * dsumts_rmsmi_next_rx_packet
(
  dsm_watermark_type * rx_wm,
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  dsm_item_type * pkt_ptr;
  dsm_item_type * pkt_tail_ptr = NULL;
  uint16 max_pkt_sz;

  if ((pkt_ptr = DSUMTS_RMSMI_GET_PENDING_RX_PKT(rm_dev)) == NULL) 
  {
    pkt_ptr = dsm_dequeue(rx_wm);
  }

  if ((pkt_ptr) && ((max_pkt_sz = DSUMTS_RMSMI_GET_RX_PKT_SZ()) > 0))
  {
    (void) dsm_split_packet
           (
             &pkt_ptr, 
             &pkt_tail_ptr, 
             max_pkt_sz, 
             DSM_DUP_ITEM_POOL
           );
    DSUMTS_RMSMI_SET_PENDING_RX_PKT(pkt_tail_ptr,rm_dev);
  }

  return pkt_ptr;
}

#else /* SPLIT_RX_PACKETS */

/*---------------------------------------------------------------------------
  Stub definition of macro that does nothing
---------------------------------------------------------------------------*/
#define DSUMTS_RMSMI_FREE_PENDING_RX_PKT()

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_NEXT_RX_PACKET()

  DESCRIPTION
    This function returns a pointer to the next rx packet for delivery to 
    the upper layer (PPP/HDLC).

  PARAMETERS
    rx_wm : DSM watermark ptr of queue holding incoming rx packets
    rm_dev: Rm Device Instance

  RETURN VALUE
    Ptr to dsm packet chain

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsm_item_type * dsumts_rmsmi_next_rx_packet
(
  dsm_watermark_type * rx_wm,
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  return dsm_dequeue(rx_wm);
} /*dsumts_rmsmi_next_rx_packet*/

#endif 


/*===========================================================================
                            Forward Declarations
===========================================================================*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_RM_DEV_FROM_SIO_IFACE

DESCRIPTION
  This function returns the Rm Device Instance for the given SIO Iface.

DEPENDENCIES
   None
  
PARAMETERS
  SIo Iface

RETURN VALUE
   Rm Dev Instance
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL dsumts_rmsm_rm_dev_instance_type dsumts_rmsmi_get_rm_dev_from_sio_iface
(
  ps_iface_type* sio_iface_ptr
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance;
  dsumts_rmsm_sm_instance_type sm_instance;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;


  for (rm_dev_instance = DSUMTS_RMSM_RM_DEV_INSTANCE_0; 
        rm_dev_instance < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; 
          rm_dev_instance++)
  {
    inst.rm_dev_instance = rm_dev_instance;

    for (sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE; 
          sm_instance < DSUMTS_RMSM_MAX_SM_INSTANCES; 
            sm_instance++)
    {
      inst.sm_instance = sm_instance;
      sm_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
      ASSERT(sm_ptr);
      if (sio_iface_ptr == &sm_ptr->sio_iface)
      {
        return rm_dev_instance;
      }
    }
  }

  return DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
}

 /*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_PS_PPP_RM_RX_SIGNAL

  DESCRIPTION
    This function fetches the PPP Rm Rx Signal for the corresponding Rm Device.

  PARAMETERS
    Rm Device Instance

  RETURN VALUE
    PPP Rm Rx Signal

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL ps_sig_enum_type dsumts_rmsmi_get_ps_ppp_rm_rx_signal
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev == DSUMTS_RMSM_RM_DEV_INSTANCE_0)
  {
    return PPP_RM_RX_SIGNAL;
  }
  else if (rm_dev == DSUMTS_RMSM_RM_DEV_INSTANCE_1)
  {
    return PPP_RM_2_RX_SIGNAL;
  }
  else
  {
    ASSERT(0);
    return PS_MAX_SIGNALS;
  }
}
/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_RM_DEV_FROM_PORT_ID

DESCRIPTION
  This function is used to get the Rm Device instance corresponding to a Port Id

DEPENDENCIES
   None
  
PARAMETERS
  Port Id

RETURN VALUE
   Rm Dev Instance
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL dsumts_rmsm_rm_dev_instance_type
dsumts_rmsmi_get_rm_dev_from_port_id(ds3g_siolib_port_e_type port_id)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr; 
  dsumts_rmsm_rm_dev_instance_type rm_dev;

  for (rm_dev=0; rm_dev<DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++)
  {

    rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);

    if (rmsmi_info_ptr != NULL &&
        rmsmi_info_ptr->port_id == port_id)
    {
      return rm_dev;
    }
  }

  return DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;

}/* dsumts_rmsmi_get_sio_portid */


LOCAL void smd_notify_timer_expired(timer_cb_data_type data)
{
  sio_ioctl_param_type wait_param;
  dsumts_rmsmi_info_type* rmsmi_info_ptr =
    (dsumts_rmsmi_info_type*)data;

  //The timer expired, call the foll. ioctl in sio to call notifywrite immediately
  wait_param.wm_wait = FALSE;

  if (rmsmi_info_ptr != NULL)
  {
    ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param,
                         rmsmi_info_ptr->port_id);
  }
} /*smd_notify_timer_expired*/

LOCAL void dsumts_rmsmi_tx_wm_non_empty_func
(
  struct dsm_watermark_type_s *wm,
  void* callback_data
)
{
  sio_ioctl_param_type wait_param;
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    (dsumts_rmsmi_info_type*)callback_data;

  //Call the foll. ioctl in sio to set the wait which will delay the call to notifywrite (to ARM11).
  wait_param.wm_wait = TRUE;

  if (rmsmi_info_ptr != NULL)
  {
    ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param, 
                         rmsmi_info_ptr->port_id);
    timer_set(&smd_notify_delay_timer[rmsmi_info_ptr->instance],
            ds_smem_tx_delay,  // this is the delay in MS
            0,
            T_MSEC);
  }
} /*dsumts_rmsmi_tx_wm_non_empty_func*/

/*===========================================================================
                            Forward Declarations
===========================================================================*/

static void dsumts_rmsmi_um_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

static void dsumts_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,
  void*               user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_RX_WM_NON_EMPTY_FUNC()

  DESCRIPTION
    This function performs the 'Non Empty' event processing for the PACKET
    DATA SERVICES shared queue. This function is called when the SIO RX
    data stream watermark item gets its first item enqueued. It sets its
    idle bit which allows UM_PKT_CTRL to end cta dormancy if needed.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsmi_rx_wm_non_empty_func
( 
  struct dsm_watermark_type_s *wm,
  /*lint -esym(715,wm) */
  /*lint -esym(818,wm) */
  void* callback_data 
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = (dsumts_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   ASSERT(rmsmi_info_ptr != NULL);


  if ( RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr))
  {
    PS_SET_SIGNAL( rmsmi_info_ptr->ps_ppp_rm_rx_signal );
    return;
  }

  return;

} /* dsumts_rmsmi_wm_non_empty_func() */
  /*lint +esym(715,wm) */
  /*lint +esym(818,wm) */

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_RX_WM_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLC TX processing, which occurs in the 
   decode ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsmi_rx_wm_lowater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data 
  /*lint -esym(715,wm,callback_data) */
  /*lint -esym(818,wm,callback_data) */
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    (dsumts_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmsmi_info_ptr != NULL);

  ds3g_siolib_ex_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_ENABLE,
                                   rmsmi_info_ptr->port_id );

} /* dsumts_rmsmi_wm_lowater_func() */
  /*lint +esym(715,wm,callback_data) */
  /*lint +esym(818,wm,callback_data) */


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_IFACE_FLOW_CMD_HDLR()

  DESCRIPTION
    This function handles the IFACE_FLOW_ENABLE_CMD posted
  by RMSM to DS task.

  PARAMETERS
    DS cmd that has information relating to IFACE_FLOW_ENABLE_CB.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_iface_flow_cmd_hdlr
(
  const ds_cmd_type            *cmd_ptr
)
{
  dsumts_rmsm_instance_type   inst;
  dsumts_rmsm_sm_instance_type   sm_inst;
  dsumts_rmsmi_sm_info_type   *sm_info_ptr = NULL;
  dsumts_rmsmi_info_type*        rmsmi_info_ptr = NULL;
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;
  ds_umts_pdp_type_enum_type     pdp_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("NULL cmd_ptr passed, return");
    return;
  }
  
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  inst.rm_dev_instance = (dsumts_rmsm_rm_dev_instance_type)
               call_info_ptr->rmsm_instance.rm_dev_instance;

  inst.sm_instance = (dsumts_rmsm_sm_instance_type)
               call_info_ptr->rmsm_instance.sm_instance;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
  }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  for (sm_inst = DSUMTS_RMSM_V4_SM_INSTANCE; 
        sm_inst < DSUMTS_RMSM_MAX_SM_INSTANCES; sm_inst++)
  {

    inst.sm_instance = sm_inst;
    sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
    ASSERT(sm_info_ptr);

  if (cmd_ptr->hdr.cmd_id == DS_CMD_3GPP_IFACE_FLOW_ENABLE) 
  {
    ps_iface_enable_flow(&sm_info_ptr->sio_iface,
                         (uint32)call_info_ptr->info_ptr);
  }
  else if (cmd_ptr->hdr.cmd_id == DS_CMD_3GPP_IFACE_FLOW_DISABLE) 
  {
    ps_iface_disable_flow(&sm_info_ptr->sio_iface,
                          (uint32)call_info_ptr->info_ptr);
  }
  else
  {
	DS_3GPP_MSG1_ERROR("Unknown cmd_id = %d", cmd_ptr->hdr.cmd_id);
  }
  }
} /* DSUMTS_RMSMI_IFACE_FLOW_CMD_HDLR */		

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_RX_WM_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound serial flow, when the hiwater point in the
   packet watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsmi_rx_wm_hiwater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data 
  /*lint -esym(715,wm,callback_data) */
  /*lint -esym(818,wm,callback_data) */
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    (dsumts_rmsmi_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmsmi_info_ptr != NULL);

  ds3g_siolib_ex_set_inbound_flow( DS_FLOW_PKT_WM_MASK, DS_FLOW_DISABLE,
                                   rmsmi_info_ptr->port_id );
} /* dsumts_rmsmi_wm_hiwater_func() */
  /*lint +esym(715,wm,callback_data) */
  /*lint +esym(818,wm,callback_data) */

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_TX_WM_LOWATER_FUNC()

  DESCRIPTION
   This function enables inbound serial flow, when the lowater point in the
   packet watermark is reached.

   This function is called from RLC TX processing, which occurs in the 
   decode ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsmi_tx_wm_lowater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data
  /*lint -esym(715,wm) */
  /*lint -esym(818,wm) */
)
{
  ds_cmd_type                 *cmd_ptr;               /* DS Task message */
  dsumts_rmsm_instance_type   inst;
  dsumts_rmsmi_info_type      *rmsmi_info_ptr = 
                                         (dsumts_rmsmi_info_type*)callback_data;
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmsmi_info_ptr != NULL);
  /*-------------------------------------------------------------------------
    To support multiple instances later, get the instance from user
    parameter which is passed back in the callback
  -----------------------------------------------------------------------*/
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    We enable the flow on the SIO interface. The rm event handler calls
    a function registered by um controller which signals the base station
    to enable the forward link flow on the Um link.
  -----------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_FLOW_ENABLE;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->rmsm_instance.rm_dev_instance = (int)inst.rm_dev_instance;
  call_info_ptr->rmsm_instance.sm_instance = (int)inst.sm_instance;
  call_info_ptr->info_ptr = (void*)DS_FLOW_PKT_WM_MASK;

  ds_put_cmd(cmd_ptr);
} /* dsumts_rmsmi_wm_lowater_func() */
  /*lint +esym(715,wm) */
  /*lint +esym(818,wm) */

/*===========================================================================
FUNCTION DSUMTS_RMSM_IFACE_IOCTL_HDLR

DESCRIPTION
  This function implements dsumts rmsm iface ioctls needed for Autonomous mode
  PPP header addition and autonomous HDLC framing/deframing using A2.

PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on
  ioctl_name        - Type of request
  argval_ptr        - QoS specified by the application
  *ps_errno         - Ptr to error code (if any)

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure

SIDE EFFECTS

===========================================================================*/
int dsumts_rmsm_iface_ioctl_hdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int ret_val = -1;
  ps_iface_ioctl_ppp_header_type* ppp_header;
  ps_iface_ioctl_vj_result_type* vj_result;
  sio_port_id_type * port_id;
  dsumts_rmsm_rm_dev_instance_type rm_dev;
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  ppp_dev_enum_type ppp_dev;

  if(argval_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("argval_ptr is NULL");
    return ret_val;
  }

  rm_dev = dsumts_rmsmi_get_rm_dev_from_sio_iface(iface_ptr);

  if (dsumts_rmsmi_validate_rm_dev_instance(rm_dev) == FALSE)
  {
    return ret_val;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if (rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("RmSmi info ptr is NULL");
    return -1;
  }

  ppp_dev = dsumts_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return -1;
  }

  switch ( ioctl_name )
  {
    case PS_IFACE_IOCTL_RMSM_PPP_BUILD_HEADER:
      DS_3GPP_MSG0_LOW("processing PS_IFACE_IOCTL_RMSM_PPP_BUILD_HEADER");
      ppp_header = (ps_iface_ioctl_ppp_header_type*)argval_ptr;
      ret_val = 
        ppp_build_header_ip( ppp_dev, 
                             ppp_header->header_ptr, 
                                     ppp_header->header_len_ptr );
      break;

    case  PS_IFACE_IOCTL_RMSM_GET_VJ_RESULT:
      DS_3GPP_MSG0_LOW("processing  PS_IFACE_IOCTL_RMSM_GET_VJ_RESULT");
      vj_result = (ps_iface_ioctl_vj_result_type *)argval_ptr;
      *vj_result = 
        ppp_get_peer_vj_result(ppp_dev);
      ret_val=0;
      break;

    case  PS_IFACE_IOCTL_RM_GET_SIO_PORT:
      DS_3GPP_MSG0_LOW("processing  PS_IFACE_IOCTL_GET_SIO_PORT");
      port_id = (sio_port_id_type *) argval_ptr;
      *port_id = ds3g_siolib_get_sio_port(rmsmi_info_ptr->port_id);
      ret_val=0;
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid request from IOCTL: %d",ioctl_name);
      *ps_errno =  DS_EINVAL;
      ret_val = -1;
      break;
  }
  return ret_val;
} /*dsumts_rmsm_iface_ioctl_hdlr*/

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_TX_WM_HIWATER_FUNC()

  DESCRIPTION
   This function disables inbound Um flow, when the hiwater point in the
   tx watermark is reached.

   This is called from the serial RX ISR.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsumts_rmsmi_tx_wm_hiwater_func
( 
  struct dsm_watermark_type_s *wm,
  void* callback_data 
  /*lint -esym(715,wm) */
  /*lint -esym(818,wm) */
)
{
  ds_cmd_type                 *cmd_ptr;               /* DS Task message */
  sio_ioctl_param_type        wait_param;
  dsumts_rmsm_instance_type   inst;
  dsumts_rmsmi_info_type      *rmsmi_info_ptr = 
                                         (dsumts_rmsmi_info_type*)callback_data;
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(rmsmi_info_ptr != NULL);
  /*-------------------------------------------------------------------------
    To support multiple instances later, get the instance from user 
    parameter which is passed back in the callback
  -------------------------------------------------------------------------*/
  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
	We disable the flow on the SIO interface. The rm event handler calls
	a function registered by um controller which signals the base station
	to disable the forward link flow on the Um link.
  -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_FLOW_DISABLE;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->rmsm_instance.rm_dev_instance = (int)inst.rm_dev_instance;
  call_info_ptr->rmsm_instance.sm_instance = (int)inst.sm_instance;
  call_info_ptr->info_ptr = (void*)DS_FLOW_PKT_WM_MASK;

  ds_put_cmd(cmd_ptr);

  //There is no wait if the timer value is 0. So no need to clear it 
  if(ds_smem_tx_delay != 0) {
    //Since the high watermark has hit, call the foll. ioctl in sio to clear the wait 
    //This will call notifywrite and send data across immediately.
    wait_param.wm_wait = FALSE;
    if (rmsmi_info_ptr != NULL)
    {
      ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param, rmsmi_info_ptr->port_id);
      timer_clr(&smd_notify_delay_timer[inst.rm_dev_instance], T_MSEC);
    }
  }
    
} /* dsumts_rmsmi_tx_wm_hiwater_func() */
  /*lint +esym(715,wm) */
  /*lint +esym(818,wm) */

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_ALLOCATE_RMSM_INSTANCE

  DESCRIPTION
   This function allocates a RmSm instance.

  PARAMETERS
    Rm Device instance which needs to be allocated

  RETURN VALUE
    Pointer to the newly allocated RmSm instance.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsumts_rmsmi_info_type* dsumts_rmsmi_allocate_rmsm_instance
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev > DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE &&
      rm_dev < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    if (dsumts_rmsmi_info[rm_dev] == NULL)
    {
      dsumts_rmsmi_info[rm_dev] = 
        DSUMTS_RMSMI_MEM_ALLOC(sizeof(dsumts_rmsmi_info_type), 
                               MODEM_MEM_CLIENT_DATA);
      return dsumts_rmsmi_info[rm_dev];
    }
  }

  return NULL;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*====================================================================
  FUNCTION dsumts_rmsmi_sio_cleanup ()

  DESCRIPTION
    This is used to perform SIO port related cleanup before call tear down

  PARAMETERS
    Rm Dev Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void dsumts_rmsmi_sio_cleanup ( dsumts_rmsm_rm_dev_instance_type rm_dev )
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);
	
  if(rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("rmsmi_info_ptr is NULL");
    return;
  }

  if (ds3g_siolib_ex_set_cd_state
       ( DS3G_SIOLIB_REMOTE_CARRIER_OFF, rmsmi_info_ptr->port_id ) == 
          DS3G_SIOLIB_NO_PORT_OPEN)
  {
    DS_3GPP_MSG0_ERROR("Error setting CD State");  
  }

  dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
  dsumts_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);

  ds3g_siolib_ex_release_port (rmsmi_info_ptr->port_id);
  dsumts_rmsmi_set_sio_port_id(rm_dev,DS3G_SIOLIB_PORT_NONE);
}

/*===========================================================================
  FUNCTION dsumts_rmsmi_get_and_validate_profile_context

  DESCRIPTION
    This function gets the pdp profile context and validates it

  PARAMETERS
    sm_ptr:         (IN)  Pointer to RM state machine
    result_ptr:    (OUT) Pointer to result

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void dsumts_rmsmi_get_and_validate_profile_context
(
  dsumts_rmsmi_info_type*     rmsmi_info_ptr,
  int*                        result_ptr
)
{
  ds_umts_pdp_profile_status_etype pdp_result;

  /*-------------------------------------------------------------------------
    Get the profile information & see what type of context is defined
  -------------------------------------------------------------------------*/      
  pdp_result=ds_umts_get_pdp_profile_context_info_per_subs(
                         (uint16)rmsmi_info_ptr->profile_id, 
                         dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
                         &rmsmi_info_ptr->profile_context_info);

  if( pdp_result != DS_UMTS_PDP_SUCCESS ) 
  {
    DS_3GPP_MSG1_ERROR("PDP profile access error:%d fail call",pdp_result);
    *result_ptr = -1;
  }

#ifdef FEATURE_SECONDARY_PDP
  else if( rmsmi_info_ptr->profile_context_info.secondary_flag == TRUE )
  {
    DS_3GPP_MSG1_ERROR("Context Specified %d is secondary. Fail call setup",
                       rmsmi_info_ptr->profile_id);
    *result_ptr = -1;
  }
#endif /* FEATURE_SECONDARY_PDP */

  return;
} /*dsumts_rmsmi_get_and_validate_profile_context*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_INIT_ALL_INSTANCES()

  DESCRIPTION
    This function initializes all the instances of UMTS RM state machines. 
    There is one state machine per Rm interface that is enabled for Data.
    
    As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
   IT also calls the pdp-ip and pdp-ppp specific sub state machine initializations
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_init_all_instances( void )
{
  int i = 0;
  int iface_instance;
  dsumts_rmsmi_sm_info_type* sm_info_ptr;
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = 
                                         DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  ppp_dev_enum_type ppp_dev;


  /*-------------------------------------------------------------------------
    DPL iface ID which uniquely identifies each ps_iface
  -------------------------------------------------------------------------*/
  dpl_iid_ifname_enum_type  ifname[DSUMTS_RMSM_MAX_INSTANCES] = 
  {
    DPL_IID_IFNAME_SIO_IFACE_UMTS_1,
    DPL_IID_IFNAME_SIO_IFACE_UMTS_2,
    DPL_IID_IFNAME_SIO_IFACE_UMTS_3,
    DPL_IID_IFNAME_SIO_IFACE_UMTS_4
  };
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsumts_rmsmi_sio_func_table.at_escape_sequence_ex_handler =
    dsumts_rmsmi_process_esc_seq_cb;
  dsumts_rmsmi_sio_func_table.dtr_changed_sig_ex_handler =
    dsumts_rmsmi_process_dtr_sig_cb;
  dsumts_rmsmi_sio_func_table.dtr_changed_sig_ex_handler_ampd0 =
    dsumts_rmsmi_process_dtr_sig_cb;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for (rm_dev_instance = DSUMTS_RMSM_RM_DEV_INSTANCE_0; 
       rm_dev_instance < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; 
       rm_dev_instance++)
  {
    /*----------------------------------------------------------------------- 
      Initialize smd_notify_delay_timers
      ----------------------------------------------------------------------*/
    memset(&smd_notify_delay_timer[rm_dev_instance],0,sizeof(timer_type));

    rmsmi_info_ptr = dsumts_rmsmi_allocate_rmsm_instance(rm_dev_instance);
  
    if (rmsmi_info_ptr == NULL)
    {
      DATA_ERR_FATAL("RmSmi Info Ptr is NULL");
    }

    /*-----------------------------------------------------------------------
      Initialize the watermarks and queues used to hook up to SIO in network 
      model.
    -----------------------------------------------------------------------*/
    memset(&rmsmi_info_ptr->sio_tx_wmk, 0, sizeof(dsm_watermark_type));
    memset(&rmsmi_info_ptr->sio_tx_wmk_q, 0, sizeof(q_type));
    dsm_queue_init(&rmsmi_info_ptr->sio_tx_wmk, 
                   (int)0x7FFFFFFF,
                   &rmsmi_info_ptr->sio_tx_wmk_q);

    memset(&rmsmi_info_ptr->sio_rx_wmk, 0, sizeof(dsm_watermark_type));
    memset(&rmsmi_info_ptr->sio_rx_wmk_q, 0, sizeof(q_type));
    dsm_queue_init(&rmsmi_info_ptr->sio_rx_wmk,
                   (int)0x7FFFFFFF,
                   &rmsmi_info_ptr->sio_rx_wmk_q);

    /*-----------------------------------------------------------------------
      Initialize the signal to indicate to PS that there is data on the RX
      watermark. Currently, we use the only one signal but later on, when we
      support multiple Rm devices, we should have one signal for each RMSM
      instance.
    -----------------------------------------------------------------------*/

    DSUMTS_INIT_CRIT_SECTION( &rmsmi_info_ptr->rx_data_crit_sec );

    RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);

    rmsmi_info_ptr->ps_ppp_rm_rx_signal = 
        dsumts_rmsmi_get_ps_ppp_rm_rx_signal(rm_dev_instance);

    rmsmi_info_ptr->user_cancel = FALSE;
  
    rmsmi_info_ptr->entitlement_check_req = FALSE;

    rmsmi_info_ptr->instance = rm_dev_instance;

    rmsmi_info_ptr->subs_id = SYS_MODEM_AS_ID_1;
  
    rmsmi_info_ptr->port_id = DS3G_SIOLIB_PORT_NONE;

    rmsmi_info_ptr->rm_ppp_config_ptr = NULL;

    ppp_dev = dsumts_rmsmi_get_ppp_dev(rm_dev_instance);

    if (dsumts_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
    {
      ASSERT(0);
      return FALSE;
    }

    rmsmi_info_ptr->ppp_handle = ppp_event_alloc(
                                        ppp_dev,
                                        dsumts_rmsmi_ppprm_event_handler_cb,
                                        (void*)rmsmi_info_ptr,
                                        NULL,
                                        NULL,
                                        dsumts_rmsmi_ppprm_event_handler_cb,
                                        (void*)rmsmi_info_ptr,
                                        dsumts_rmsmi_ppprm_event_handler_cb,
                                        (void*)rmsmi_info_ptr
                                      );
    if( NULL == rmsmi_info_ptr->ppp_handle )
    {
      DATA_ERR_FATAL("Couldn't get PPP event handler");
    }
  
    if (ds_3gpp_rmsm_ip_common_init(rm_dev_instance) == FALSE)
    {
      return FALSE;
    }
  
    rmsmi_info_ptr->sm_type = DSUMTS_RMSM_INVALID_SM_TYPE;
  
    for (sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE; 
          sm_instance < DSUMTS_RMSM_MAX_SM_INSTANCES; 
          sm_instance++, i++)
    {
      sm_info_ptr = rmsmi_info_ptr->sm_info_ptr[sm_instance] = 
        (dsumts_rmsmi_sm_info_type*) 
               DSUMTS_RMSMI_MEM_ALLOC(sizeof(dsumts_rmsmi_sm_info_type), MODEM_MEM_CLIENT_DATA);
  
      if (sm_info_ptr == NULL)
      {
        DS_3GPP_MSG2_ERROR( "Could not allocate memory for sm instance %d, call instance %d", 
                            sm_instance, 
                            rm_dev_instance);
        ASSERT(0);
      }
    
      memset(sm_info_ptr, 0x0, sizeof(dsumts_rmsmi_sm_info_type));
  
      sm_info_ptr->instance = sm_instance;
      sm_info_ptr->um_iface_ptr = NULL;
      sm_info_ptr->flow_enabled = 0x0;
      sm_info_ptr->sio_acl.acl_fptr = ds_3gpp_rmsm_sio_rt_acl;
      sm_info_ptr->sio_acl.post_proc_fptr = ds_3gpp_rmsm_sio_rt_acl_post_proc;
  
      iface_instance = ps_iface_create(&sm_info_ptr->sio_iface, 
                                       SIO_IFACE, 
                                       &sm_info_ptr->sio_acl,
                                       NULL,
                                       &sm_info_ptr->sio_phys_link,
                                       1);
      DS_3GPP_MSG1_HIGH("SIO Iface created :inst:%d",iface_instance);
      /*lint -e718, -e746, -e730 */
      ASSERT(iface_instance >= 0);
      /*lint +e718, +e746, +e730 */
   
      (void) ps_iface_dpl_support_network_logging(&sm_info_ptr->sio_iface,
                                               ifname[i]);
      (void) ps_iface_dpl_set_iface_desc(&sm_info_ptr->sio_iface, NULL);
  
      (void) ps_iface_dpl_support_link_logging(&sm_info_ptr->sio_iface,
                                               ifname[i]);

      (void) ps_iface_dpl_support_flow_logging(&sm_info_ptr->sio_iface,
                                               ifname[i]);
      /*---------------------------------------------------------------------
      Register rmsm ioctl function pointer
      ----------------------------------------------------------------------*/
      sm_info_ptr->sio_iface.iface_ioctl_f_ptr = dsumts_rmsm_iface_ioctl_hdlr;
  
      /*-----------------------------------------------------------------------
      Initialize the sub state machines. They initialize the sm_specific_info
      in the structure
      -----------------------------------------------------------------------*/  
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
     if (dsumts_rmsm_ppp_sm_init(sm_info_ptr) == FALSE)
     {
      return FALSE;
      }
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */

    if (dsumts_rmsm_ip_init(sm_info_ptr) == FALSE)
    {
      return FALSE;
    }

    sm_info_ptr->arb_close_in_progress = FALSE;
  
    sm_info_ptr->vobj_data_ptr = (void*)rmsmi_info_ptr;
  
    sm_info_ptr->inited = TRUE;

  }
  
  /* If Multi PDN is not enabled we allocate only one RmSm Instance. So break from the loop*/
  if (ds_3gpp_nv_manager_get_multi_pdn_dun_nv() == FALSE)
  {
    break;
  }
}

#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#ifdef FEATURE_HS_USB
  /* Use the default value for HS_USB flush threshold */
  hs_usb_flush_packet_size = HS_USB_FLUSH_PACKET_SIZE;  
#endif /* FEATURE_HS_USB */
#endif/* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */
  fc_reg = FALSE;
  (void)q_init(&rmsm_fc_cmd_q);
  return TRUE;
} /* dsumts_rmsmi_init_all_instances() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_HANDLE_ATD_ON_RM

  DESCRIPTION
    This function handles the dial strings for pkt data calls on a 
    specific RM instance and initiates the data call over the appropriate 
    pkt iface.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsmi_handle_atd_on_rm
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance, /* Rm Device inst. on which atd */
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  /*lint -esym(715,dial_string,digit_mode) */
  const dsat_dial_modifier_info_type *modifiers,  /* Dial sting modifiers  */
  sys_modem_as_id_e_type             ps_subs_id
)
{
  ds3g_siolib_port_alloc_info_s_type  alloc_info;
  int  profile_result = 0;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
  ds3g_siolib_port_e_type port_id = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_port_e_type active_port = ds3g_siolib_get_active_port();
  ds_umts_pdp_type_enum_type pdp_type;

#ifdef FEATURE_DUAL_SIM
  byte default_profile_id = 0;
  ds_umts_pdp_subs_e_type ds_subs_id; /* subs id mapping in DS */
#endif /* FEATURE_DUAL_SIM */  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds3gsubsmgr_is_subs_id_valid(ps_subs_id))
  {
    return NULL;
  }
  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("RmSmi info ptr is NULL");

    if (ds3g_siolib_ex_set_cd_state
          (DS3G_SIOLIB_REMOTE_CARRIER_OFF,active_port) ==
            DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DS_3GPP_MSG0_ERROR("Error setting CD State");  
    }

    dsat_send_result_code_ex( DSAT_NO_CARRIER, active_port );

    return NULL;
  }

#ifdef FEATURE_DATA_TE_MT_PDP
  if ( !dsumts_rmsm_lock_rm() ) 
  {
    DS_3GPP_MSG1_HIGH("HANDLE_ATD_ON_RM: failed, RM is locked already,"
                      " Rm Device [%d]", rm_dev_instance);
    if (ds3g_siolib_ex_set_cd_state
          (DS3G_SIOLIB_REMOTE_CARRIER_OFF,active_port) ==
          DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DS_3GPP_MSG0_ERROR("Error setting CD State");  
    }

    dsat_send_result_code_ex( DSAT_NO_CARRIER,active_port );
    dsumts_rmsm_unlock_rm();
    return NULL;
  }
  else
  {
    DS_3GPP_MSG1_HIGH("HANDLE_ATD_ON_RM: success, RM lock acquired, Rm Device [%d]", 
             rm_dev_instance);
  }
#endif /* FEATURE_DATA_TE_MT_PDP */

  DS_3GPP_MSG0_HIGH("HANDLE_ATD_ON_RM: Rcvd dial string for Pkt data call");

  /*-------------------------------------------------------------------------
    Try to allocate SIO port for external data call.  Without successfully
    downing the semaphore, the call cannot proceed.
  -------------------------------------------------------------------------*/
  memset((void*)&alloc_info, 0x0, sizeof(ds3g_siolib_port_alloc_info_s_type));
  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED;

  if ( DS3G_SIOLIB_PORT_NONE !=
       (port_id = ds3g_siolib_ex_allocate_port( &alloc_info, DS_CALL_TYPE_PKT )) )
  {
    /*-------------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
      Otherwise indicate to siolib that we are in call now.
    -------------------------------------------------------------------------*/
    dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, port_id);
    if( FALSE ==
          ds3g_siolib_ex_register_callback_func
            (&dsumts_rmsmi_sio_func_table,rmsmi_info_ptr->port_id ))
    {
      if (ds3g_siolib_ex_set_cd_state
            (DS3G_SIOLIB_REMOTE_CARRIER_OFF,rmsmi_info_ptr->port_id) ==
            DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error setting CD State");  
      }
      dsat_send_result_code_ex( DSAT_NO_CARRIER,port_id );
#ifdef FEATURE_DATA_TE_MT_PDP
      dsumts_rmsm_unlock_rm();
#endif
      return NULL;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Unable to allocate SIOLIB port");

    if (ds3g_siolib_ex_set_cd_state (DS3G_SIOLIB_REMOTE_CARRIER_OFF,port_id) ==
          DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DS_3GPP_MSG0_ERROR("Error setting CD State");  
    }
    dsat_send_result_code_ex( DSAT_NO_CARRIER,port_id );
#ifdef FEATURE_DATA_TE_MT_PDP
    dsumts_rmsm_unlock_rm();
#endif
    return NULL;
  }

  ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,TRUE );

  DS_3GPP_MSG0_HIGH("Pkt dial string recvd, posting RM_WANTS_PKT_CALL_EV");

  if (!ds3gsubsmgr_is_subs_id_valid(ps_subs_id))
  {
    ps_subs_id = ds3gsubsmgr_get_def_data_subs_id();
  }

  /*-------------------------------------------------------------------------
    Get the context id from the dial string, if available.
  -------------------------------------------------------------------------*/
  if ((modifiers->modifier_present == TRUE) && 
      (modifiers->modifier == DSAT_CID_DIAL_MODIFIER))
  {
    rmsmi_info_ptr->profile_id = 
     modifiers->modifier_info.cid.cid_val | 
      (uint32)DS_UMTS_PDP_ATCOP_PROFILE_FAMILY;
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    /*-------------------------------------------------------------------------
      If the profile number was not specified, default to using profile number
      1 so that we are backwards compatible. 
    -------------------------------------------------------------------------*/ 
    ds_subs_id = dsumts_subs_mgr_get_subs_id(ps_subs_id);
    DS_3GPP_MSG1_HIGH("UMTS Data Subscription is: %d", ds_subs_id);
    	  
    if (DS_UMTS_PDP_SUCCESS != 
        ds_umts_get_pdp_profile_num_for_tethered_calls_internal
          (ds_subs_id, &default_profile_id))
    {
      DS_3GPP_MSG0_ERROR("Couldn't get default profile, failing call");
      dsumts_rmsmi_sio_cleanup(rmsmi_info_ptr->instance);
#ifdef FEATURE_DATA_TE_MT_PDP
      dsumts_rmsm_unlock_rm();
#endif /* FEATURE_DATA_TE_MT_PDP */
      dsat_send_result_code( DSAT_NO_CARRIER );
      return NULL;
    }
    else
    {
      rmsmi_info_ptr->profile_id = default_profile_id;
      DS_3GPP_MSG1_HIGH("No ctx in Dial str:default to prof %d",
                         rmsmi_info_ptr->profile_id);
    }
#else
    rmsmi_info_ptr->profile_id = 0x1 | 
                 (uint32)DS_UMTS_PDP_ATCOP_PROFILE_FAMILY;
    DS_3GPP_MSG1_HIGH("No ctx in Dial str:default to prof #%d",
                       rmsmi_info_ptr->profile_id);
#endif
  }

  rmsmi_info_ptr->subs_id = ps_subs_id;

  /*-------------------------------------------------------------------------
  Get the profile context and validate it
  ------------------------------------------------------------------------*/
  dsumts_rmsmi_get_and_validate_profile_context( rmsmi_info_ptr, 
                                                &profile_result);
  if(profile_result == -1)
  {
    DS_3GPP_MSG0_ERROR("Failing call");
    dsumts_rmsmi_sio_cleanup(rmsmi_info_ptr->instance);
#ifdef FEATURE_DATA_TE_MT_PDP
    dsumts_rmsm_unlock_rm();
#endif /* FEATURE_DATA_TE_MT_PDP */

      return NULL;
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Profile:%d valid & primary: using it",
              rmsmi_info_ptr->profile_id);
    }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if(pdp_type == DS_UMTS_PDP_PPP )
  {
    rmsmi_info_ptr->sm_type = DSUMTS_RMSM_SM_TYPE_PDP_PPP;
    inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
    DS_3GPP_MSG0_HIGH("UMTS RMSM : PDP-PPP type call/State machine");
  }

  else if (pdp_type == DS_UMTS_PDP_IPV4 || 
           pdp_type == DS_UMTS_PDP_IPV6 ||
           pdp_type == DS_UMTS_PDP_IPV4V6 )

  {
    rmsmi_info_ptr->sm_type = DSUMTS_RMSM_SM_TYPE_PDP_IP;
    inst.sm_instance = DSUMTS_RMSM_ANY_SM_INSTANCE;
    DS_3GPP_MSG3_HIGH("UMTS RMSM : PDP-IP type call/State machine",0,0,0);
  }

  else
  {
    DS_3GPP_MSG1_ERROR("Invalid pdp type %d",
                        rmsmi_info_ptr->profile_context_info.pdp_type);
    ASSERT(0);
    return NULL;
  }

  /*--------------------------------------------------------------------------
    Capture dial string to internal RMSM info table, 
    NV_PKT_DIAL_STRING_SIZE is size of dial string with ending NULL
  ---------------------------------------------------------------------------*/
  memscpy( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.dial_string, 
           NV_PKT_DIAL_STRING_SIZE-1,
           dial_string, NV_PKT_DIAL_STRING_SIZE - 1 );
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info. 
    dial_string[NV_PKT_DIAL_STRING_SIZE - 1] = '\0';
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info. 
    valid_fields = (uint32) NET_CFG_PRM_DIAL_STR_MASK;

  DS_3GPP_MSG1_LOW("DSUMTS_RMSMI_DIAL_STR_CB: dial string is %s",
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->ppp_info.dial_string);

  /* cache subs_id into rmsm info */
  rmsmi_info_ptr->subs_id = ps_subs_id;
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_WANTS_PKT_CALL_EV );

#ifdef FEATURE_DATA_TE_MT_PDP
  /*--------------------------------------------------------------------
    RMSM was locked and Command to kick-start RMSM has been 
    enqueued to DS Task, RMSM will now lock itself accordingly 
  --------------------------------------------------------------------*/
  dsumts_rmsm_unlock_rm();
#endif

  return (dsumts_rmsmi_call_abort_cb);
} /* dsumts_rmsmi_dial_str_cb() */
  /*lint +esym(715,dial_string,digit_mode) */


#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_FIND_PROFILE_MATCH()

  DESCRIPTION
    This routine is used to find matching profile to MT call info

  PARAMETERS
    instance
   *mt_info_ptr
    num_of_profiles
   *profile_number

  RETURN VALUE
    dsat_call_abort_cb_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsmi_mt_find_profile_match
(
  dsumts_rmsm_rm_dev_instance_type instance,
  const dsumts_rmsm_mt_info_type *mt_info_ptr,
  uint8 num_of_profiles,
  const uint16 *profile_number
)
{
  int i;
  uint16 tmp_profile_id, tmp_num_of_profiles;
  uint16 *tmp_profile_number;
  ds_umts_pdp_profile_status_etype result;
  boolean valid_flag = FALSE;
  dsat_pdp_access_control_e_type access_ctrl_flag;
  const byte *incom_apn_name;
  uint8 encoded_apn_name_len = 0;
  PACKED byte * PACKED_POST apn;
  byte encoded_apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_umts_pdp_type_enum_type  ds_pdp_type, incom_ds_pdp_type;
  ds_umts_pdp_context_type tmp_profile_context_info;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(instance);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
  }

  if ( (mt_info_ptr == NULL) || 
       ( profile_number == NULL ) ||
       ( num_of_profiles > DS_UMTS_MAX_PDP_PROFILE_NUM ) ) 
  {
    DS_3GPP_MSG0_ERROR("RMSMI_PROFILE_MATCH: invalid input params, returning!");
    return FALSE;
  }

  access_ctrl_flag = dsatetsipkt_get_access_control();
  incom_apn_name = mt_info_ptr->apn_name;
  incom_ds_pdp_type = dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt(mt_info_ptr->pdp_type);

  /*-------------------------------------------------------------------------
    If number of profiles passed if 0, use default profile
  -------------------------------------------------------------------------*/
  if ( num_of_profiles == 0 ) 
  {
    DS_3GPP_MSG0_HIGH("RMSMI_PROFILE_MATCH: default to ATCOP prof #1");
    tmp_num_of_profiles = 1;
    tmp_profile_number = (uint16 *)ds_system_heap_mem_alloc(sizeof(uint16));
    tmp_profile_number[0] = (uint16)0x1;
      }
  else
      {
    tmp_num_of_profiles = num_of_profiles;
    tmp_profile_number = (uint16 *)ds_system_heap_mem_alloc(num_of_profiles * sizeof(uint16));
    (void)memscpy(tmp_profile_number, num_of_profiles * sizeof(uint16), 
                  profile_number, num_of_profiles * sizeof(uint16) );
      }

  /*-------------------------------------------------------------------------
    Get the profile information & see what type of context is defined
  -------------------------------------------------------------------------*/  
  for ( i = 0; i < tmp_num_of_profiles; i++ ) 
  {
    tmp_profile_id = tmp_profile_number[i] | 
                     (uint32)DS_UMTS_PDP_ATCOP_PROFILE_FAMILY;

    result = ds_umts_get_pdp_profile_context_info_is_valid_per_subs(
               tmp_profile_id,
               dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
               &valid_flag);
    
    if ( ( result == DS_UMTS_PDP_SUCCESS ) && ( valid_flag ) ) 
    {
      result = ds_umts_get_pdp_profile_context_info_per_subs(
                           tmp_profile_id, 
                           dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
                           &tmp_profile_context_info);

      if( result != DS_UMTS_PDP_SUCCESS ) 
      {
        DS_3GPP_MSG1_HIGH("RMSMI_PROFILE_MATCH: PDP profile access error [%d] fail call",
                 result);
        return FALSE;
      }
#ifdef FEATURE_SECONDARY_PDP
      else if( rmsmi_info_ptr->profile_context_info.secondary_flag == TRUE )
      {
        DS_3GPP_MSG0_HIGH("RMSMI_PROFILE_MATCH: Context Specified is secondary. Fail call setup");
        return FALSE;
      }
#endif
      else
      {
        /*------------------------------------------------------------------------
          Post-Answer Profile match starts here
          (1) Match on [APN, PDP type]
        ------------------------------------------------------------------------*/
        apn = tmp_profile_context_info.apn;
        encoded_apn_name[DS_UMTS_MAX_APN_STRING_LEN] = '\0';
        encoded_apn_name_len = ds_pdn_cntxt_store_encoded_apn(encoded_apn_name, apn);
        encoded_apn_name[encoded_apn_name_len] = '\0';
        /*------------------------------------------------------------------------
          change the pdp_type to IPV4 if incoming pdp_type is IPV4V6 and profile
          also has IPV4V6. We can have the outgoing request(OTA) as same as profile,
          this is will make sure that the APN table will be in line with the profile.
          We may have an NV addition, for configuring different scenario based
          on customer requirement currently we will default the RMSM iface bring up
          to be of V4 type.
        ------------------------------------------------------------------------*/
        DATA_3GPP_MSG_SPRINTF_3(MSG_LEGACY_HIGH,"dsumts_rmsmi_mt_find_profile_match"
                            " apn in profile %s incoming APN: %s profile %d,",
                            (char *) encoded_apn_name,
                            (char*) incom_apn_name, tmp_profile_id);
            
        if(tmp_profile_context_info.pdp_type == DS_UMTS_PDP_IPV4V6 &&
           incom_ds_pdp_type == DS_UMTS_PDP_IPV4V6)
        {
          DS_3GPP_MSG0_HIGH("RMSMI_PROFILE_MATCH: Changing PDP_TYPE as IPV4 as call with IPV4V6 is not possible");
          ds_pdp_type = DS_UMTS_PDP_IPV4;
        }
        else if(tmp_profile_context_info.pdp_type == DS_UMTS_PDP_IPV4V6 &&
           ((incom_ds_pdp_type == DS_UMTS_PDP_IPV4)||
           (incom_ds_pdp_type == DS_UMTS_PDP_IPV6)) )
        {
          ds_pdp_type = incom_ds_pdp_type;
          /* update the pdp type in pdp_context in check call orig in this particular case */
        }
        else /* this is for PDP type non IPV4V6 */
        {
          ds_pdp_type = tmp_profile_context_info.pdp_type;
        }
        DS_3GPP_MSG1_HIGH("Debug MT PDP: ds_pdp_type %d", ds_pdp_type);

        if (( 0 == strlen( (char *)apn ) )|| 
               ( 0 == strcasecmp( (char *)encoded_apn_name, (char *)incom_apn_name ) ))
        {
          if ((incom_ds_pdp_type == ds_pdp_type) ||
             ((incom_ds_pdp_type == DS_UMTS_PDP_IPV4V6) &&
              ((ds_pdp_type == DS_UMTS_PDP_IPV4)||
               (ds_pdp_type == DS_UMTS_PDP_IPV6))))
          {
            rmsmi_info_ptr->profile_id = tmp_profile_id;
            (void)memscpy(&rmsmi_info_ptr->profile_context_info,
		          sizeof(ds_umts_pdp_context_type),
                          &tmp_profile_context_info,
                          sizeof(ds_umts_pdp_context_type) );
            rmsmi_info_ptr->profile_context_info.pdp_type = ds_pdp_type;
            DS_3GPP_MSG1_HIGH( "RMSMI_PROFILE_MATCH: Matched with profile %d",
                               tmp_profile_id);
            return TRUE;
          }/* matched [APN, PDP type] */
        }
      }/* Post answer profile match */
    }/* valid context */
    else
    {
      DS_3GPP_MSG2_HIGH("MT_RING_HDLR: skipping to next profile, result [%d], valid_flag [%d]",
               result, valid_flag);
      continue;
    }
  }/* for(all context ids provided by TE) */

  return FALSE;

}/* dsumts_rmsmi_mt_find_profile_match() */


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_HANDLE_ANS_ON_RM

  DESCRIPTION
    This function handles MT PDP calls on a specific RM instance and 
    initiates the data call over the appropriate pkt iface.
    
  PARAMETERS
    instance: RMSM instance
    dsat_pdp_answer_data_type

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsmi_mt_handle_ans_on_rm
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance,
  dsat_pdp_answer_data_type * ans_cb_data
)
{ 
  ds3g_siolib_port_alloc_info_s_type  alloc_info;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsm_mt_info_type rmsm_mt_info;
  uint8 mt_index;
  ds3g_siolib_port_e_type port_id;

  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  DS_3GPP_MSG1_MED("HANDLE_ANS_ON_RM: Handling ANS on rm_inst [%d]",rm_dev_instance);

  /*-------------------------------------------------------------------------
    Try to allocate SIO port for external data call.  Without successfully
    downing the semaphore, the call cannot proceed.
  -------------------------------------------------------------------------*/
  memset((void*)&alloc_info, 0x0, sizeof(ds3g_siolib_port_alloc_info_s_type));
  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_TERMINATED;

  if ( DS3G_SIOLIB_PORT_NONE !=
       ( port_id = ds3g_siolib_ex_allocate_port( &alloc_info, DS_CALL_TYPE_PKT )) )
  {
    DS_3GPP_MSG1_HIGH("HANDLE_ANS_ON_RM: Alloc SIO port [%d]",
                       port_id);

    dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, port_id);

    /*-------------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
      Otherwise indicate to siolib that we are in call now.
    -------------------------------------------------------------------------*/
    if( FALSE ==
          ds3g_siolib_ex_register_callback_func
            (&dsumts_rmsmi_sio_func_table, port_id))
    {
      if (ds3g_siolib_ex_set_cd_state(DS3G_SIOLIB_REMOTE_CARRIER_OFF,port_id) == 
            DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_3GPP_MSG0_ERROR("Error setting CD State");  
      }

      dsat_send_result_code_ex( DSAT_NO_CARRIER,port_id );
      return NULL;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("HANDLE_ANS_ON_RM: Unable to allocate SIOLIB port");
    
    dsumts_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance, DS3G_SIOLIB_PORT_NONE);

    port_id = ds3g_siolib_determine_port_to_be_allocated(&alloc_info);
    
    if (ds3g_siolib_ex_set_cd_state (DS3G_SIOLIB_REMOTE_CARRIER_OFF,port_id) 
          == DS3G_SIOLIB_NO_PORT_OPEN)
    {
      DS_3GPP_MSG0_ERROR("Error setting CD State");  
    }

    dsat_send_result_code_ex(DSAT_NO_CARRIER, port_id);
    return NULL;
  }
  memset( &rmsm_mt_info, 0, sizeof( dsumts_rmsm_mt_info_type ) );

  if (  !dsumts_rmsmi_get_mt_index_from_instance( inst.rm_dev_instance, &mt_index ) ||
        !dsumtsps_mt_um_get_info( mt_index, &rmsm_mt_info ) ||
        !dsumts_rmsmi_mt_find_profile_match(
             inst.rm_dev_instance,
             &rmsm_mt_info,
             ans_cb_data->number_of_profiles,
             ans_cb_data->profile_number ) )
  {
    /* Fail call setup */
    DS_3GPP_MSG0_HIGH("RMSM_MT_ANS_CB: post REJECT_MT_PKT_CALL_EV to RMSM");
    
    /* Post RM_REJECT_MT_PKT_CALL_EV to RMSM, todo ask ATCOP for callback data */
    dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV );
    return NULL;
  }

  ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,TRUE );

  DS_3GPP_MSG0_MED("HANDLE_ANS_ON_RM: RMSM post event RMSM_RM_WANTS_PKT_CALL_EV");
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_WANTS_PKT_CALL_EV );

  return (dsumts_rmsmi_call_abort_cb);  
}/* dsumts_rmsmi_mt_handle_ans_on_rm() */


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_RING_TE

  DESCRIPTION
    This function handles MT PDP calls on a specific RM instance and 
    initiates the data call over the appropriate pkt iface.
    
  PARAMETERS
   *mt_info_ptr : MT info from Um

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_mt_ring_te
(
  const dsumts_rmsm_mt_info_type *mt_info_ptr
)
{
  ds_umts_pdp_profile_status_etype result = DS_UMTS_PDP_FAIL;
  uint32 profile_id;
  boolean perm_list_match = FALSE;
  dsat_pdp_access_control_e_type  active_access_ctrl, access_ctrl_flag;
  ds_umts_pdp_type_enum_type incom_ds_pdp_type = DS_UMTS_PDP_MAX;
  ds_umts_pdp_type_enum_type ds_pdp_type = DS_UMTS_PDP_MAX;
  uint32 none_list_sz = 0, rej_list_sz = 0, perm_list_sz = 0;
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = 
    DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
  PACKED byte * PACKED_POST apn_name ;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( mt_info_ptr == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("RMSMI_MT_RING_TE: invalid input params, returning!");
    return FALSE;
  }

  inst.rm_dev_instance = rm_dev_instance = mt_info_ptr->rmsm_inst;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  incom_ds_pdp_type = dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt(mt_info_ptr->pdp_type);

  DS_3GPP_MSG3_HIGH("RMSMI_MT_RING_TE: RMSM inst [%d], incom APN [%c], incom_pdp_type [%d]", 
           rm_dev_instance, mt_info_ptr->apn_name[0], mt_info_ptr->pdp_type);

  active_access_ctrl = dsatetsipkt_get_access_control();

  if ( active_access_ctrl == DSAT_PDP_ACCESS_CONTROL_NONE ) 
  {
    DS_3GPP_MSG3_HIGH("RMSMI_MT_RING_TE: no access ctrl is active, RING_TE/AUTO-ANS!", 
           rm_dev_instance, mt_info_ptr->apn_name[0], mt_info_ptr->pdp_type);
    goto RING_TE;
  }

  /*-------------------------------------------------------------------------
    PRE-RING Access Control Starts here. 
    Loop thru all profiles. 
  -------------------------------------------------------------------------*/
  for (profile_id = 1; profile_id <= DS_UMTS_MAX_PDP_PROFILE_NUM; profile_id++) 
  {
    rmsmi_info_ptr->profile_id = 
      ds_3gpp_profile_cache_get_cache_profile_num_from_index(profile_id);

    if(rmsmi_info_ptr->profile_id == -1)
    {
      DS_3GPP_MSG1_HIGH("profile_id:%d is not present in cache",profile_id);
      continue;
    }

    rmsmi_info_ptr->profile_id = profile_id | 
                 (uint32)DS_UMTS_PDP_ATCOP_PROFILE_FAMILY;

    result = ds_umts_get_pdp_profile_context_info_per_subs(
             rmsmi_info_ptr->profile_id, 
             dsumts_subs_mgr_get_subs_id(rmsmi_info_ptr->subs_id),
             &rmsmi_info_ptr->profile_context_info);

    if( result != DS_UMTS_PDP_SUCCESS ) 
    {
      DS_3GPP_MSG1_HIGH("RMSMI_MT_RING_TE: PDP profile access error [%d], fail call",
             result);
      return FALSE;
    }

    apn_name = rmsmi_info_ptr->profile_context_info.apn;

    ds_pdp_type = rmsmi_info_ptr->profile_context_info.pdp_type;

    if (ds_pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      ds_pdp_type = DS_UMTS_PDP_IPV4;
      rmsmi_info_ptr->profile_context_info.pdp_type = DS_UMTS_PDP_IPV4;
    }
    
    /*-------------------------------------------------------------------------
      Fetch value of access_ctrl_flag from Profile
    -------------------------------------------------------------------------*/
    result = ds_umts_get_pdp_profile_access_ctrl_flag(
               rmsmi_info_ptr->profile_id,
               &access_ctrl_flag);

    if( result != DS_UMTS_PDP_SUCCESS ) 
    {
      DS_3GPP_MSG1_HIGH("RMSMI_MT_RING_TE: PDP profile access error [%d], fail call",
               result);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Switch on Active Access Control Method
    -------------------------------------------------------------------------*/
    switch( active_access_ctrl )
    {
    case DSAT_PDP_ACCESS_CONTROL_REJECT:
      if (access_ctrl_flag == DSAT_PDP_ACCESS_CONTROL_REJECT)
      {
        rej_list_sz++;

        /* Check for [APN,PDP type] match, on match return false */
        if ( ( 0 == strncmp( (char *)apn_name, (char *)mt_info_ptr->apn_name, mt_info_ptr->apn_len ) ) &&
             ( incom_ds_pdp_type == ds_pdp_type ) ) 
        {
          return FALSE;
        }
      }
      break; /* DSAT_PDP_ACCESS_CONTROL_REJECT */

    case DSAT_PDP_ACCESS_CONTROL_PERMISSION:
      if (access_ctrl_flag == DSAT_PDP_ACCESS_CONTROL_PERMISSION)
      {
        perm_list_sz++;

        /* Check for [APN,PDP type] match, on match return true */
        if ( strncmp( (char *)apn_name, (char *)mt_info_ptr->apn_name, mt_info_ptr->apn_len ) &&
             ( incom_ds_pdp_type == ds_pdp_type ) ) 
        {
          perm_list_match = TRUE;
          break;
        }
      }
      break; /* DSAT_PDP_ACCESS_CONTROL_PERMISSION */

    case DSAT_PDP_ACCESS_CONTROL_NONE:
      
      break; /* DSAT_PDP_ACCESS_CONTROL_NONE */

    default:
      DS_3GPP_MSG1_ERROR("RMSMI_MT_RING_TE: unknown access control method [%d]",
                access_ctrl_flag);
      ASSERT(0);
      return FALSE;
    }/* switch() */

    if (perm_list_match) 
    {
      DS_3GPP_MSG1_HIGH("RMSMI_MT_RING_TE: incom MT matches profile [%d] in PERM list",
               profile_id);
      break; /* break out of for( all profiles) loop*/
    }
  
  }/* for( all profiles ) */

  /*-------------------------------------------------------------------------
    PRE-RING Access Control: If there is no PERM list match, check for other 
    access control rules. Check if list sizes of active access control methods,
    if they are 0, that means TE will REJECT or ACCEPT all calls
  -------------------------------------------------------------------------*/
  if (!perm_list_match) 
  {
    switch( active_access_ctrl )
    {
      case DSAT_PDP_ACCESS_CONTROL_REJECT:
        if ( rej_list_sz == 0 )
        {
          DS_3GPP_MSG0_HIGH("RMSMI_MT_RING_TE: ACCESS_CTRL_REJ, rej_list_sz = 0, RING TE");
        }
        else
        {
          DS_3GPP_MSG0_HIGH("RMSMI_MT_RING_TE: ACCESS_CTRL_REJ, rej_list_sz <> 0, no match, RING TE");
        }
        break; /* DSAT_PDP_ACCESS_CONTROL_REJECT */

      case DSAT_PDP_ACCESS_CONTROL_PERMISSION:
        if ( perm_list_sz == 0 )
        {
          DS_3GPP_MSG0_HIGH("RMSMI_MT_RING_TE: ACCESS_CTRL_PERM, perm_list_sz = 0, FAIL call!");
          return FALSE;
        }
        else
        {
          DS_3GPP_MSG0_HIGH("MT_HANDLE_RING_IND: ACCESS_CTRL_PERM, perm_list_sz <> 0, no match, RING TE!");
        }
        break; /* DSAT_PDP_ACCESS_CONTROL_PERMISSION */

      case DSAT_PDP_ACCESS_CONTROL_NONE:
        DS_3GPP_MSG1_HIGH("MT_RING_HDLR: ACCESS_CTRL_NONE, none_list_sz [%d], RING TE",
                   none_list_sz);
        break; /* DSAT_PDP_ACCESS_CONTROL_NONE */

      default:
        DS_3GPP_MSG1_ERROR("MT_RING_HDLR: unknown access control method [%d]",
                  access_ctrl_flag);
        ASSERT(0);
        return FALSE;
    }/* switch() */
  }/* if (!perm_list_match) */

  /*-------------------------------------------------------------------------
      PRE-RING Access Control ends here. If execution reaches here that means
      we can proceed with TE RING indication
  -------------------------------------------------------------------------*/
RING_TE:
  /*-----------------------------------------------------------------------
    Store the call type & pdp type & the stack for the pdp type 
    Set the sub state machine type based on the type of call
    pdp-ip or pdp-ppp
  -------------------------------------------------------------------------*/
  if(incom_ds_pdp_type == DS_UMTS_PDP_PPP )
  {
    rmsmi_info_ptr->sm_type = DSUMTS_RMSM_SM_TYPE_PDP_PPP ;
    rmsmi_info_ptr->mt_index = mt_info_ptr->mt_index;
    DS_3GPP_MSG0_HIGH("RMSMI_MT_RING_TE: PDP-PPP type call/State machine");
  }
  else if(  (incom_ds_pdp_type == DS_UMTS_PDP_IP) ||
            (incom_ds_pdp_type == DS_UMTS_PDP_IPV6)||
            (incom_ds_pdp_type == DS_UMTS_PDP_IPV4V6))
  {
    rmsmi_info_ptr->sm_type = DSUMTS_RMSM_SM_TYPE_PDP_IP ;
    rmsmi_info_ptr->mt_index = mt_info_ptr->mt_index;
    DS_3GPP_MSG0_HIGH("RMSMI_MT_RING_TE: PDP-IP type call/State machine");
  }
  else
  {
    DS_3GPP_MSG2_ERROR("RMSMI_MT_RING_TE: Invalid PDP type [%d] incom_ds_pdp_type %d!",
                       rmsmi_info_ptr->profile_context_info.pdp_type,incom_ds_pdp_type);
    return FALSE;
  }
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state = DS_3GPP_RMSM_IP_COMMON_NULL_STATE;

  DS_3GPP_MSG1_HIGH("RMSMI_MT_RING_TE: set mt_index [%d], "
                    "post to RMSM UM_WANTS_MT_PKT_CALL_EV", 
           mt_info_ptr->mt_index);

  /*-------------------------------------------------------------------------
    We currently pass 0 as the instance while posting rm_wants_pkt_call 
    event. When we support multiple Rm devices, we would need to use a 
    different RMSM instance for each Rm device
  -------------------------------------------------------------------------*/
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_WANTS_MT_PKT_CALL_EV );

  return TRUE;
}


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_MT_GET_INDEX_FROM_INSTANCE

  DESCRIPTION
    Given a RMSM instance, get mt_index
    
  PARAMETERS
    instance: RMSM instance
    mt_index: MT index

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_get_mt_index_from_instance
(
  dsumts_rmsm_rm_dev_instance_type instance,
  uint8 *mt_index
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr =
                                 dsumts_rmsmi_get_rmsmi_info_ptr(instance);

  if (rmsmi_info_ptr == NULL)
  {
    return FALSE;
  }
  
  if ( ( instance >= 0 ) && ( instance < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES) ) 
  {
    *mt_index = rmsmi_info_ptr->mt_index;
    DS_3GPP_MSG2_HIGH("RMSMI_MT_IDX_FROM_INST: inst [%d], mt_index [%d]",
             instance, *mt_index);
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_DATA_TE_MT_PDP*/


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_CALL_ABORT_CB()

  DESCRIPTION
    This function is registered as abort handler with ATCOP. If ATCOP
    receives an AT cmd while processing an abortable cmd, this function gets
    called.  It is registered along with the dial string handler. It posts
    appropriate event to DSUMTS_RMSM which initiates the call termination.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_call_abort_cb
(
  void
)
{
  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  dsumts_rmsm_instance_type inst;
  ds3g_siolib_port_e_type port_id = ds3g_siolib_get_active_port();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE && 
      port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_3GPP_MSG3_HIGH( "Invalid Port Id", 0, 0, 0);
    return;
  }

  inst.rm_dev_instance = dsumts_rmsmi_get_rm_dev_from_port_id(port_id);

  if (dsumts_rmsmi_validate_rm_dev_instance(inst.rm_dev_instance) == FALSE)
  {
    return;
  }

  inst.sm_instance = sm_instance;

  DS_3GPP_MSG0_HIGH( "Call aborted");

  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );

  return;
} /* dsumts_rmsmi_call_abort_cb() */


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_PROCESS_DTR_SIG_CB()

  DESCRIPTION
    This function processes the change in dtr state.  If DTR is deasserted
    appropriate event is posted to dsumts_rmsm which initiates the call 
    termination. DTR assert is ignored for now.

  PARAMETERS
    is_asserted: flag which indicates current DTR state -
                 TRUE: DTR asserted, FALSE: DTR deasserted

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_process_dtr_sig_cb
(
  boolean                          is_asserted,
  void*                            cb_data
)
{
  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_ANY_SM_INSTANCE;
  dsumts_rmsm_instance_type inst;
  dsumts_rmsm_rm_dev_instance_type rm_dev;
  ds3g_siolib_port_e_type port_id;
  dsumts_rmsmi_info_type  *rmsm_info_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cb_data != NULL);

  port_id = (ds3g_siolib_port_e_type)cb_data;

  rm_dev = dsumts_rmsmi_get_rm_dev_from_port_id(port_id);

  if (dsumts_rmsmi_validate_rm_dev_instance(rm_dev) == FALSE)
  {
    return;
  }

  rmsm_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);
  if(rmsm_info_ptr != NULL &&
     rmsm_info_ptr->sm_type == DSUMTS_RMSM_SM_TYPE_PDP_PPP)
  {
    sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  }

  inst.rm_dev_instance = rm_dev;
  inst.sm_instance = sm_instance;

  /*-------------------------------------------------------------------------
    Currently only dtr dropped is handled, post appropriate event
------------------------------------------------------------------------*/

  if ( is_asserted == FALSE )
  {
    DS_3GPP_MSG0_HIGH( "DTR dropped");

    dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
  }
  else
  {
    DS_3GPP_MSG0_MED( "DTR assert, ignoring");
  }

  return;

} /* dsumts_rmsmi_process_dtr_sig_cb() */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_UM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on the UM_PKT_IFACE.  Registraion is done for various events based
  upon the call type during call setup. As part of event handling
  appropriate event is posted to DSUMTS_RMSM.

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsumts_rmsmi_um_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
  /*lint -esym(715,user_data_ptr) */
  /*lint -esym(818,user_data_ptr) */
)
{
  dsumts_rmsmi_sm_info_type *sm_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type *companion_sm_info_ptr = NULL;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;

  dsumts_rmsm_instance_type inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG2_MED( "Recd event %d for iface 0x%x", event, this_iface_ptr);

  sm_info_ptr = (dsumts_rmsmi_sm_info_type*)user_data_ptr;

  if (sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("SM info ptr is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_info_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_info_ptr->instance;


  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case IFACE_FLOW_ENABLED_EV:    

      sm_info_ptr->flow_enabled &= 0x1;
      if (sm_info_ptr->flow_enabled)
      {
        SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      }
      /*---------------------------------------------------------------------
        Send signal to PS to resume pulling PPP packets from SIO rx_wm, 
        laptop flow will be enabled by rx_wm lo water function.
      ---------------------------------------------------------------------*/
      /*lint -e641 */
      PS_SET_SIGNAL(rmsmi_info_ptr->ps_ppp_rm_rx_signal );
      /*lint +e641 */ 
       
      break;

    case IFACE_FLOW_DISABLED_EV:

      sm_info_ptr->flow_enabled &= 0x0;
      companion_sm_info_ptr = dsumts_rmsmi_get_companion_sm_ptr(&inst);

      if (companion_sm_info_ptr != NULL && companion_sm_info_ptr->flow_enabled)
      {
        RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      }

      break;

    case IFACE_ACTIVE_OUT_OF_USE_EV:
    {
      DS_3GPP_MSG0_HIGH( "Um iface active out of use ev");
break;
    }
    

    case IFACE_DOWN_EV:
    {
      if ( sm_info_ptr->arb_close_in_progress )
      {
        DS_3GPP_MSG0_HIGH( "In arb, UM iface down ev, posting UM_IFACE_DOWN_ARB_EV");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_ARB_EV );
      }
      else if( event_info.iface_down_info.state != IFACE_DOWN )
      {
        DS_3GPP_MSG0_HIGH( "UM iface down, posting UMTS_RMSM_UM_IFACE_DOWN_EV");
        dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_DOWN_EV );
      }
      else
      {
        DS_3GPP_MSG1_HIGH( "Ignoring event %d", event);
      }

      break;
    }

    case IFACE_UP_EV:
    {
    
      DS_3GPP_MSG0_HIGH( "UMTS iface up, posting UM_IFACE_UP_EV");
  
      dsumts_rmsm_post_event( &inst, UMTS_RMSM_UM_IFACE_UP_EV );  

      break;
    }

    default:
    
      DS_3GPP_MSG1_ERROR( "Unregistered event %d recvd, ignoring", event);
      break;
  } /* switch( event ) */
} /* dsumts_rmsmi_um_event_handler_cb() */
  /*lint +esym(715,user_data_ptr) */
  /*lint +esym(818,user_data_ptr) */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_INSTANCE_NUM_FROM_UM_PHYS_LINK()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  um_phys_link_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  um_phys_link_ptr

RETURN VALUE
  DSUMTS_RMSM_INVALID_SM_INSTANCE, in case of failure
  SM Instance, otherwise

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsm_sm_instance_type dsumts_rmsmi_get_instance_num_from_um_phys_link
(
  const ps_phys_link_type *um_phys_link_ptr 
)
{
  dsumts_rmsm_sm_instance_type i = DSUMTS_RMSM_V4_SM_INSTANCE;  
  dsumts_rmsm_sm_instance_type instance = DSUMTS_RMSM_INVALID_SM_INSTANCE;
  ps_phys_link_type *saved_phys_link_ptr = NULL; /* Ptr to phys link       */
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = 
    DSUMTS_RMSM_RM_DEV_INSTANCE_0;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = 
    dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i= DSUMTS_RMSM_V4_SM_INSTANCE; i<DSUMTS_RMSM_MAX_SM_INSTANCES; i++)
  {
    saved_phys_link_ptr = PS_IFACE_GET_PHYS_LINK
      (rmsmi_info_ptr->sm_info_ptr[i]->um_iface_ptr);

    if (saved_phys_link_ptr == um_phys_link_ptr)
    {
      instance = i;
      break;
    }
  }

  return instance;
} /* dsumts_rmsmi_get_instance_num_from_um_phys_link */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_PHYS_LINK_UM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link 
  events occur on the UM_PKT_IFACE.  Registraion is done for various events 
  based upon the call type during call setup. As part of event handling
  appropriate event is posted to DSUMTS_RMSM.

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsumts_rmsmi_phys_link_um_event_handler_cb
(
  ps_phys_link_type          *this_phys_link_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
  /*lint -esym(715,user_data_ptr,event_info) */
  /*lint -esym(818,user_data_ptr) */
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* companion_sm_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG2_MED("Recd event %d for phys link 0x%x", 
                   event, this_phys_link_ptr);   
  
  sm_info_ptr = (dsumts_rmsmi_sm_info_type*)user_data_ptr;

  if (sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("SM Info ptr is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_info_ptr->vobj_data_ptr;
  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  inst.sm_instance = sm_info_ptr->instance;

  /*-------------------------------------------------------------------------
    Process flow control events first since they may get posted during data
    transfer
  -------------------------------------------------------------------------*/

  switch( event )
  {
    case PHYS_LINK_FLOW_ENABLED_EV:    

      sm_info_ptr->flow_enabled &= 0x1;
      if (sm_info_ptr->flow_enabled)
      {
        SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      }

      /*---------------------------------------------------------------------
        Send signal to PS to resume pulling PPP packets from SIO rx_wm, 
        laptop flow will be enabled by rx_wm lo water function.
      ---------------------------------------------------------------------*/
      /*lint -e641 */
      DS_3GPP_MSG0_MED( "PHYS link flow enabled,"
                        " set PPP_RM_RX_SIGNAL to PS");
      PS_SET_SIGNAL(rmsmi_info_ptr->ps_ppp_rm_rx_signal  );
      /*lint +e641 */ 
       

      break;

    case PHYS_LINK_FLOW_DISABLED_EV:
      /*---------------------------------------------------------------------
        We don't need to do anything when UM flow is disabled, rx function 
        will stop pulling PPP packets from SIO rx_wm. rx_wm hi water 
        function will disable laptop flow.
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG3_MED( "phys link flow DISABLED", 0, 0, 0 );

      sm_info_ptr->flow_enabled &= 0x0;
      companion_sm_info_ptr = dsumts_rmsmi_get_companion_sm_ptr(&inst);

      if (companion_sm_info_ptr != NULL && companion_sm_info_ptr->flow_enabled)
      {
        RESET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      }

      break;

    default:
    
      DS_3GPP_MSG1_ERROR( "Unregistered event %d recvd, ignoring",
                           event);
      break;

  } /* switch( event ) */
} /* dsumts_rmsmi_phys_link_um_event_handler_cb() */
  /*lint +esym(715,user_data_ptr,event_info) */
  /*lint +esym(818,user_data_ptr) */


/*===========================================================================
  FUNCTION DSUMTS_RMSMI_SETUP_SIO_WATERMARKS()

  DESCRIPTION
    This function configures the SIO packet mode watermark items
    to operate for the laptop pkt data calls handled by DSUMTS_RMSM.

  PARAMETERS
    rm_dev_instance   - RM Device Instance 
    *tx_wm_ptr - ptr to sio transmit (to laptop) watermark
    *rx_wm_ptr - ptr to sio receive (from laptop) watermark

  RETURN VALUE
    None

  DEPENDENCIES
    Hi/Lo Watermark Settings: The Lo and Hi watermark values and the don't
    exceed values for each watermark is inter-dependent with the DSM small
    and large item counts (DSM_LARGE_ITEM_CNT and DSM_SMALL_ITEM_CNT) in the
    DSM module. Any change to lo/hi watermark or don't exceed values can
    cause asserts due to insufficient memory items available.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_setup_sio_watermarks
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance,
  dsm_watermark_type *tx_wm_ptr,
  dsm_watermark_type *rx_wm_ptr,
  q_type             *tx_wmk_q_ptr,
  q_type             *rx_wmk_q_ptr 
)
{
  nv_stat_enum_type   nv_status;
  boolean is_device_bt = FALSE;                    /* is device Bluetooth? */
  dsat_num_item_type  ipr_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rdm_get_device( RDM_DATA_SRVC ) == RDM_BT_NA_DEV )
  {
    is_device_bt = TRUE;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  ASSERT(rmsmi_info_ptr != NULL);

  /*-------------------------------------------------------------------------
    Free any pending rx packet.
  -------------------------------------------------------------------------*/
  DSUMTS_RMSMI_FREE_PENDING_RX_PKT(rm_dev_instance);
  
  /*-------------------------------------------------------------------------
                             SETUP SIO TX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This watermark queue is to be used for transmitting data from mobile to 
    TE2 over SIO.
  -------------------------------------------------------------------------*/
  dsm_queue_destroy(tx_wm_ptr);
  dsm_queue_init(tx_wm_ptr,
                 DSUMTS_RMSM_DL_DNE,
                 tx_wmk_q_ptr);
  tx_wm_ptr->lo_watermark          = DSUMTS_RMSM_DL_LO;
  tx_wm_ptr->hi_watermark          = DSUMTS_RMSM_DL_HI;
  tx_wm_ptr->dont_exceed_cnt       = DSUMTS_RMSM_DL_DNE;

  tx_wm_ptr->gone_empty_func_ptr   = NULL;
  tx_wm_ptr->non_empty_func_ptr    = NULL;
  tx_wm_ptr->lowater_func_ptr      = dsumts_rmsmi_tx_wm_lowater_func;
    tx_wm_ptr->lowater_func_data     = (void*) rmsmi_info_ptr;
  tx_wm_ptr->hiwater_func_ptr      = dsumts_rmsmi_tx_wm_hiwater_func;
    tx_wm_ptr->hiwater_func_data     = (void*) rmsmi_info_ptr;
  tx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
                             SETUP SIO RX WATERMARK
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    This watermark queue is to be used for transmitting data from mobile to 
    TE2 over SIO.
    
    If SIO is using high speed serial card and CTS flow control is available
    then use the higher values for waternarks.  Also for bluetooth, provide
    more cushion between hi and don't exceed levels.
  -------------------------------------------------------------------------*/

  dsm_queue_destroy(rx_wm_ptr);
  dsm_queue_init(rx_wm_ptr,
                   15000,
                   rx_wmk_q_ptr);

  /*lint -e64 -e641 -e713 */
  if ( ( ds3g_siolib_is_baud_valid( (sio_bitrate_type)ipr_val ) ) &&
       ( ipr_val >= SIO_BITRATE_115200 ) &&
       ( ipr_val <  SIO_BITRATE_BEST ) 
     )
  {
  /*lint +e64 +e641 +e713*/
    if( is_device_bt == TRUE )
    {
    rx_wm_ptr->lo_watermark        = 1500;
    rx_wm_ptr->hi_watermark        = 5000;
    rx_wm_ptr->dont_exceed_cnt   = 38000;
    }
    else
    {
    /* RMSM UL watermark levels configurable */
    rx_wm_ptr->lo_watermark    = DSUMTS_RMSM_UL_LO;
    rx_wm_ptr->hi_watermark    = DSUMTS_RMSM_UL_HI;
    rx_wm_ptr->dont_exceed_cnt = DSUMTS_RMSM_UL_DNE;
    }
  }
  else
  {
    rx_wm_ptr->lo_watermark        = 400;
    rx_wm_ptr->hi_watermark        = 1300;

    if( is_device_bt == TRUE )
    {
      rx_wm_ptr->dont_exceed_cnt   = 35000;
    }
    else
    {
      rx_wm_ptr->dont_exceed_cnt   = 35000;
    }
  }

  rx_wm_ptr->gone_empty_func_ptr   = NULL;
  rx_wm_ptr->non_empty_func_ptr    = dsumts_rmsmi_rx_wm_non_empty_func;
  rx_wm_ptr->non_empty_func_data   = (void*) rmsmi_info_ptr;
  rx_wm_ptr->lowater_func_ptr      = dsumts_rmsmi_rx_wm_lowater_func;
  rx_wm_ptr->lowater_func_data     = (void*) rmsmi_info_ptr;
  rx_wm_ptr->hiwater_func_ptr      = dsumts_rmsmi_rx_wm_hiwater_func;
  rx_wm_ptr->hiwater_func_data     = (void*) rmsmi_info_ptr;
  rx_wm_ptr->each_enqueue_func_ptr = NULL;

  /*-------------------------------------------------------------------------
    Reset the total_rcvd_cnt, as a packet data call can really set this
    number very high.
  -------------------------------------------------------------------------*/
  tx_wm_ptr->total_rcvd_cnt        = 0;
  rx_wm_ptr->total_rcvd_cnt        = 0;

 //Initialize timer value to 0 before reading from NV
  ds_smem_tx_delay = 0;

  timer_def(&smd_notify_delay_timer[rm_dev_instance], NULL, NULL, 0, 
             smd_notify_timer_expired, (unsigned long)rmsmi_info_ptr);
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DATA_AGG_TIMER_I, &ds_nv_item,rmsmi_info_ptr->subs_id);
  if( nv_status == NV_DONE_S)
  {
     ds_smem_tx_delay = (uint8)ds_nv_item.data_agg_timer[RMSM_AGG_TIMER_NV_INDEX];
     //read the element at index 1, because it corresponds to smd data port 1.
     if(ds_smem_tx_delay == 0) {
         DS_3GPP_MSG0_HIGH("NV read, smd_notify_delay_timer is 0. "
                  "Disabling data aggregation");
     } else {
         DS_3GPP_MSG1_HIGH("NV read, smd_notify_delay_timer : [%d]", 
                           ds_smem_tx_delay);
     }
  }
  else 
  {
    DS_3GPP_MSG1_HIGH("Bad NV read status for smd_notify_delay_timer: %d,"
                      " Disabling timer", nv_status);
    //cannot read nv item for the timeout value for aggregation timer. So disable the timer
    ds_smem_tx_delay = 0;
  }

  tx_wm_ptr->non_empty_func_ptr    = dsumts_rmsmi_tx_wm_non_empty_func;
  tx_wm_ptr->non_empty_func_data   = (void*) rmsmi_info_ptr;

#ifdef FEATURE_DATA_WCDMA_PS
  //Register with FC for enabling/Disabling DA
  DATA_MSG0_HIGH("Registering with FC for CPU stats");
  dsumts_rmsmi_dl_fc_register();
#endif /*  FEATURE_DATA_WCDMA_PS */
} /* dsumts_rmsmi_setup_sio_watermarks() */


/*===========================================================================
FUNCTION DSUMTS_RMSMI_SIO_TX_PPP_DATA

DESCRIPTION
  This function is called to transmit a packet coming on Um PPP
  interface onto Rm interface.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_sio_tx_ppp_data
(
  dsm_item_type **item_head_ptr,
  void           *meta_info_ptr,
  void *user_data_ptr
  /*lint -esym(715,user_data_ptr) */
  /*lint -esym(818,user_data_ptr) */
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_meta_info_type *temp_meta_info_ptr = (ps_meta_info_type *)meta_info_ptr;

  ASSERT(user_data_ptr != NULL);
  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)user_data_ptr;

#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#if defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
  int length;
#endif /* FEATURE_HS_USB && !FEATURE_NO_USB_HW_ON_PROCESSOR*/
#endif /* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

  (void)ds3g_siolib_ex_setup_tx( *item_head_ptr, FALSE, 
                                 rmsmi_info_ptr->port_id );

#ifdef FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH
#if defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)
  if (rmsmi_info_ptr->port_id == DS3G_SIOLIB_SMD_DS_PORT || 
      rmsmi_info_ptr->port_id == DS3G_SIOLIB_SMD_DATA11_PORT)
  {
    length = dsm_length_packet(*item_head_ptr);
    if(length <= hs_usb_flush_packet_size)
    {
      ds3g_siolib_ex_flush_tx(NULL,rmsmi_info_ptr->port_id);
    }
  }  
#endif /* FEATURE_HS_USB && !FEATURE_NO_USB_HW_ON_PROCESSOR*/
#endif /* FEATURE_DATA_USB_AGGRN_IOCTL_FLUSH */

  *item_head_ptr = NULL;

  /*------------------------------------------------------------------------
   Free the meta_info_ptr here. meta_info_ptr would be set if filters were
   executed on Um iface (typically for GPS/LBS apps).
  ------------------------------------------------------------------------*/
  if (NULL != temp_meta_info_ptr)
  {
    PS_META_INFO_FREE(&temp_meta_info_ptr);
  }

} /* dsumts_rmsmi_sio_tx_ppp_data() */
  /*lint +esym(715,user_data_ptr) */
  /*lint +esym(818,user_data_ptr) */

/*===========================================================================
FUNCTION DSUMTS_RMSMI_SIO_RX_PPP_DATA

DESCRIPTION
  This function is called to receive the next PPP packet, coming on the Rm
  interface, for rx processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type* dsumts_rmsmi_sio_rx_ppp_data( void *user_data )
{
  dsumts_rmsm_instance_type inst;
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  uint8                  i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)user_data;

  if (rmsmi_info_ptr == NULL)
  {
    for(i=0; i < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; i++)
    {
      rmsmi_info_ptr = dsumts_rmsmi_info[i];
      if (rmsmi_info_ptr != NULL)
      {
        DS_3GPP_MSG0_LOW("RmSmi Info ptr is NULL "
                          "emptying rx_wm and not doing further rx processing");
        dsm_empty_queue(&rmsmi_info_ptr->sio_rx_wmk);
      }
    }
    return NULL;
  }

  if (rmsmi_info_ptr->instance <= DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE || 
      rmsmi_info_ptr->instance >= DSUMTS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    DS_3GPP_MSG1_HIGH( "rmsmi_info_ptr->instance is invalid: "
                       "rmsmi_info_ptr->instance is %d", rmsmi_info_ptr->instance);
    return NULL;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;;

    if ( RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr) )
  {
    return dsumts_rmsmi_next_rx_packet( &rmsmi_info_ptr->sio_rx_wmk,
                                        inst.rm_dev_instance);
  }

    return NULL;
  
} /* dsumts_rmsmi_sio_rx_ppp_data() */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_PPPRM_EVENT_HANDLER_CB()

DESCRIPTION
  This callback function is called when one of the registered PPP events
  occur on RM PPP device. As part of event handling appropriate event is
  posted to DSMTS_RMSM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsumts_rmsmi_ppprm_event_handler_cb
(
  ppp_dev_enum_type   ppp_dev,
  ppp_protocol_e_type protocol,
  ppp_event_e_type    ppp_event,
  void*               user_data,
  uint32              session_handle,
  ppp_fail_reason_e_type  fail_reason
  /*lint -esym(715,protocol,session_handle, fail_reason) */
)
{
  dsumts_rmsmi_info_type            *rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type *sm_info_ptr = NULL;
  dsumts_rmsm_sm_instance_type sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  dsumts_rmsm_instance_type inst;
  ds_umts_pdp_type_enum_type         pdp_type;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  DS_3GPP_MSG3_MED( "Recd %d ppp event", ppp_event, 0, 0 );
    
  /*lint -e730 */
  ASSERT( ppp_dev == PPP_RM_DEV ||
          ppp_dev == PPP_RM_2_DEV );
  /*lint +e730 */

  inst.rm_dev_instance = dsumts_rmsmi_get_rm_dev_from_ppp_dev(ppp_dev);
  inst.sm_instance = sm_instance;

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL");
    return;
  }
  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (protocol == PPP_PROTOCOL_IPCP || 
      (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_UMTS_PDP_IPV4) ||
      (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_UMTS_PDP_PPP))
  {
    inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  }

  else if (protocol == PPP_PROTOCOL_IPV6CP || 
          (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_UMTS_PDP_IPV6))
  {
    inst.sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;
  }

  else if (protocol == PPP_PROTOCOL_LCP && pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    if (ppp_event != PPP_DOWN_EV)
    {
      DS_3GPP_MSG3_HIGH("LCP Up/Resync event received during dual IP call, ignoring",0,0,0);
      return;
    }
    inst.sm_instance =  DSUMTS_RMSM_ANY_SM_INSTANCE;
  }

  else
  {
    DS_3GPP_MSG3_ERROR("Unrecognized protocol: %d, pdp type: %d",
                        protocol, pdp_type, 0 );
    return;
  }

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
  if (sm_info_ptr == NULL && inst.sm_instance != DSUMTS_RMSM_ANY_SM_INSTANCE)
  {
    DATA_ERR_FATAL("SM Info ptr is NULL");
    return;
  }
  
  switch(ppp_event)
  {
    case PPP_UP_EV:      
      ps_iface_enable_flow( &sm_info_ptr->sio_iface, 
                            DS_FLOW_UMTS_RMSM_MASK );
      dsumts_rmsm_post_event( &inst,  UMTS_RMSM_RM_PPP_UP_EV );
      break;

    case PPP_DOWN_EV:
      dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
      break;

    case PPP_RESYNC_EV:
      ps_iface_disable_flow( &sm_info_ptr->sio_iface, 
                             DS_FLOW_UMTS_RMSM_MASK );
      break;

    default:
      DS_3GPP_MSG1_ERROR( "Unsupported PPP event %d", ppp_event);
      ASSERT(0);
  }
} /* dsumts_rmsmi_ppprm_event_handler_cb */
  /*lint +esym(715,protocol,session_handle, fail_reason) */



/*===========================================================================
  FUNCTION DSUMTS_RMSMI_PROCESS_ESC_SEQ_CB

  DESCRIPTION
    This function is called when DTR goes low and AT&D = 1.  This should do
    the same action as the dtr_sig_cb.
  PARAMETERS
    None
  RETURN VALUE
    Always return DSAT_ONLINE_DATA.
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
 ===========================================================================*/
dsat_mode_enum_type dsumts_rmsmi_process_esc_seq_cb(void* cb_data)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dsumts_rmsmi_process_dtr_sig_cb(FALSE, cb_data);
  return(DSAT_ONLINE_DATA);
} /* dsumts_rmsmi_process_esc_seq_cb() */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_INSTANCE_NUM_FROM_UM_IFACE()

DESCRIPTION
  Returns the instance of the DSUMTS_RMSM corresponding to the supplied 
  um_iface_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  um_iface_ptr

RETURN VALUE
  DSUMTS_RMSM_INVALID_SM_INSTANCE, in case of failure
  Valid SM Instance, otherwise

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsm_sm_instance_type dsumts_rmsmi_get_sm_instance_num_from_um_iface
(
  const ps_iface_type* um_iface_ptr
)
{
  dsumts_rmsm_sm_instance_type i = DSUMTS_RMSM_V4_SM_INSTANCE;
  dsumts_rmsm_rm_dev_instance_type j = DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
  dsumts_rmsmi_info_type* rmsmi_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (j = DSUMTS_RMSM_RM_DEV_INSTANCE_0; j<DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; j++)
{
  for (i = DSUMTS_RMSM_V4_SM_INSTANCE; i<DSUMTS_RMSM_MAX_SM_INSTANCES; i++)
  {
      rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(j);

      if (rmsmi_info_ptr != NULL &&
          rmsmi_info_ptr->sm_info_ptr[i] != NULL &&
          rmsmi_info_ptr->sm_info_ptr[i]->um_iface_ptr == um_iface_ptr)
      {
        return i;
      }
    }
  }

  return DSUMTS_RMSM_INVALID_SM_INSTANCE;
} /* dsumts_rmsmi_get_instance_num_from_um_iface */


/*===========================================================================
FUNCTION   DSUMTS_RMSM_GET_INSTANCE_NUM_FROM_SIO_IFACE()

DESCRIPTION
  Returns the instance of the UMTS_RMSM corresponding to the supplied 
  sio_iface_ptr.

NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  sio_iface_ptr: The sio_iface_ptr which the caller wants to map to RMSM
  instance

RETURN VALUE
  DSUMTS_RMSM_INVALID_SM_INSTANCE: In case of error
  SM instance: The SM instance corr. to the supplied sio_iface_ptr, otherwise


DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsm_sm_instance_type  dsumts_rmsmi_get_instance_num_from_sio_iface
(
  const ps_iface_type* sio_iface_ptr
)
{
  dsumts_rmsm_sm_instance_type i = DSUMTS_RMSM_V4_SM_INSTANCE;
  dsumts_rmsm_sm_instance_type instance = DSUMTS_RMSM_INVALID_SM_INSTANCE;
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance = 
    DSUMTS_RMSM_RM_DEV_INSTANCE_0;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = 
    dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = DSUMTS_RMSM_V4_SM_INSTANCE; i<DSUMTS_RMSM_MAX_SM_INSTANCES; i++)
  {
    if (&rmsmi_info_ptr->sm_info_ptr[i]->sio_iface == sio_iface_ptr)
    {
      instance = i;
      break;
    }
  }

  return instance;
} /* dsumts_rmsmi_get_instance_num_from_sio_iface */

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_RMSMI_INFO_PTR

DESCRIPTION
  Returns pointer to the Rm Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  rm_dev_instance - Rm Device Instance

RETURN VALUE
  Pointer to the RMSMI info structure,
  NULL if invalid Rm Dev instance

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsmi_info_type*  dsumts_rmsmi_get_rmsmi_info_ptr
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance
)
{
  if (rm_dev_instance <= DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE ||
         rm_dev_instance >= DSUMTS_RMSM_MAX_RM_DEV_INSTANCES)
  {
    DS_3GPP_MSG1_ERROR("Invalid Rm Dev instance %d",rm_dev_instance);
    return NULL;
  }

  return dsumts_rmsmi_info[rm_dev_instance];
} /* dsumts_rmsmi_get_info_ptr_from_instance */


/*===========================================================================
FUNCTION   DSUMTS_RMSMI_GET_SM_INFO_PTR

DESCRIPTION
  Returns pointer to the SM specific Control Block.
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS 
  inst_ptr - Instance of the Rm Device and the State Machine Instance

RETURN VALUE
  Pointer to the SM info structure.
  NULL if invalid SM or Rm Dev instance

DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
dsumts_rmsmi_sm_info_type*  dsumts_rmsmi_get_sm_info_ptr
(
  dsumts_rmsm_instance_type* inst_ptr
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Instance ptr is NULL");
    return NULL;
  }

  if (inst_ptr->sm_instance <= DSUMTS_RMSM_INVALID_SM_INSTANCE ||
         inst_ptr->sm_instance >= DSUMTS_RMSM_MAX_SM_INSTANCES)
  {
    DS_3GPP_MSG1_ERROR("Invalid sm instance %d",inst_ptr->sm_instance);
    return NULL;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("RmSm Info ptr is NULL");
    return NULL;
  }

  return rmsmi_info_ptr->sm_info_ptr[inst_ptr->sm_instance];
}

/*===========================================================================
FUNCTION   DSUMTS_RMSMI_SIOLIB_DEREGISTER_CALLBACKS()

DESCRIPTION
  De reregisters the callbacks registered with SIO lib
  
NOTE: THIS IS A RESTRICTED FUNCTION AND IS INTENDED TO BE USED BY 
DSUMTS_RMSM MODULES ONLY

PARAMETERS
  Port Id

RETURN VALUE
  none
  
DEPENDENCIES
  dsumts_rmsm_init() should have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void  dsumts_rmsmi_siolib_deregister_callbacks(ds3g_siolib_port_e_type port_id)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)ds3g_siolib_ex_deregister_callback_func( &dsumts_rmsmi_sio_func_table, 
                                                  port_id );
}

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_UM_IFACE_PTR

  DESCRIPTION
    This function retrieves the pointer to UM iface in the system which can
    be used to originate the packet data call. The iface pointer is obtained
    from the routing lookup. If no policy info is provided, the router 
    returns the default UM interface.
    
    The PDP profile pointer is passed in the acl policy info.
    
    An UM iface is returned only if there is one available and it is not
    in IFACE_DOWN state
  
  PARAMETERS
    None
  
  RETURN VALUE
    Pointer to ps_iface
  
  DEPENDENCIES
    Note that context lock is required because sockets calls originate 
    in PS task context. So we need to make sure that if we decide to 
    originate on a particular iface, IFACE_state should be set to 
    something other than DOWN and DISABLED and the UM packet iface 
    ACLs should be disabled to prevent sockets call before 
    freeing up the locks. This ensures only one type of call originates 
    at a time on each interface.
      
  SIDE EFFECTS
    None
===========================================================================*/
ps_iface_type* dsumts_rmsmi_get_um_iface_ptr
( 
  const dsumts_rmsmi_sm_info_type     *sm_ptr
)
{
  ps_iface_type*            um_iface_ptr = NULL;
  acl_policy_info_type      *acl_policy_info = NULL;
#ifdef FEATURE_NEW_PKT_INFO_DEF
  ip_pkt_info_type          ip_info;
#else
  acl_ipfilter_info_type    ip_info;
#endif
  boolean        arb_candidate = FALSE;
  uint32         iface_prio_mask = 0;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ps_iface_net_down_reason_type   net_down_reason;

  if (sm_ptr == NULL)
  {
    DATA_ERR_FATAL("SM Ptr is NULL");
    return NULL;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Rmsmi Infos Ptr is NULL");
    return NULL;
  }

  acl_policy_info = (acl_policy_info_type *)modem_mem_alloc(
                                    sizeof(acl_policy_info_type),
                                    MODEM_MEM_CLIENT_DATA_CRIT );

  if (acl_policy_info == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for dsumts_rmsmi_get_um_iface_ptr");
    return NULL;
  }

  memset( &ip_info, 0, sizeof(ip_info) );
  memset( acl_policy_info, 0, sizeof(acl_policy_info_type) );

  acl_init_policy_info(acl_policy_info);

  if ( sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE )
  {
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.dest.ps_s_addr = ROUTE_DEFAULT_DST;
    acl_policy_info->ip_family = IPV4_ADDR;
  }
  else if ( sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE)
  {
    ip_info.ip_vsn = IP_V6;
    acl_policy_info->ip_family = IPV6_ADDR;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unsupported PDP type:%d",
                        rmsmi_info_ptr->profile_context_info.pdp_type);
    ASSERT(0);
  }

  acl_policy_info->iface.kind = DSS_IFACE_NAME; 
  /*lint -e64 */
  acl_policy_info->iface.info.name = DSS_IFACE_UMTS;
  /*lint +e64 */
  acl_policy_info->policy_flag = DSS_IFACE_POLICY_SPECIFIED;
  acl_policy_info->pdp_info = (acl_pdp_ctx_cfg_type) (rmsmi_info_ptr->profile_id);

  /*-------------------------------------------------------------------------
    setup proc ID and App ID
  -------------------------------------------------------------------------*/
  acl_policy_info->proc_id        = PROC_ID_TE_DUN;
  acl_policy_info->app_identifier = APP_ID_TE_DUN;

  /*-------------------------------------------------------------------------
    Setup intent flags
  -------------------------------------------------------------------------*/
  acl_policy_info->bring_up    = FALSE;
  acl_policy_info->lookup_only = TRUE;

  /*-------------------------------------------------------------------------
    Do the interface bring-up. This will involve arbitration.
  -------------------------------------------------------------------------*/
  um_iface_ptr =  ps_route_look_up_iface_by_policy( acl_policy_info,
                                                    &arb_candidate,
                                                    &iface_prio_mask,
                                                    &net_down_reason );

  if ( um_iface_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR( "lookup: FAILED route look up by policy!");
    modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
    return NULL;
  }
  else if ( arb_candidate )
  {
    DS_3GPP_MSG3_HIGH( "lookup: ARB, Iface [0x%x:%x], "
                       "arb_cand? [%d], prio_mask [0x%x]", 
                        um_iface_ptr->name, 
                       um_iface_ptr->instance, iface_prio_mask );
  }
  else
  {
    DS_3GPP_MSG3_HIGH( "lookup: NO ARB, Iface [0x%x:%x],"
                       " arb_cand? [%d], prio_mask [0x%x]", 
              0, arb_candidate, iface_prio_mask );
  }

  DS_3GPP_MSG1_HIGH("um iface allocated: %lx",(uint32) um_iface_ptr);
  
  if ( um_iface_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("um_iface_ptr is NULL");
  }
  
  modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
  return um_iface_ptr;
      
} /* dsumts_rmsmi_get_um_iface_ptr */

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_BRING_UP_UM_IFACE_BY_POLICY

  DESCRIPTION
    
  
  PARAMETERS
    None
  
  RETURN VALUE
    0 SUCCESS
   -1 FAILURE
  
  DEPENDENCIES

      
  SIDE EFFECTS
    None
===========================================================================*/
int dsumts_rmsmi_bring_up_um_iface_by_policy
( 
  const dsumts_rmsmi_sm_info_type     *sm_ptr,
  ps_iface_type                   **if_ptr,
  int16                            *ps_errno
)
{
  acl_policy_info_type      *acl_policy_info = NULL;
#ifdef FEATURE_NEW_PKT_INFO_DEF
  ip_pkt_info_type          ip_info;
#else
  acl_ipfilter_info_type    ip_info;
#endif
  int32 result = 0;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ps_iface_net_down_reason_type   net_down_reason;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sm_ptr == NULL)
  {
    DATA_ERR_FATAL("SM Ptr is NULL");
    return -1;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr; 

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Rmsmi Info Ptr is NULL");
    return -1;
  }

  acl_policy_info = (acl_policy_info_type *)modem_mem_alloc(
                                    sizeof(acl_policy_info_type),
                                    MODEM_MEM_CLIENT_DATA_CRIT );

  if (acl_policy_info == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for dsumts_rmsmi_bring_up_um_iface_by_policy");
    return -1;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr);

  memset( &ip_info, 0, sizeof(ip_info) );
  memset( acl_policy_info, 0, sizeof(acl_policy_info_type) );

  acl_init_policy_info(acl_policy_info);

  if ( sm_ptr->instance == DSUMTS_RMSM_V4_SM_INSTANCE )
  {
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.dest.ps_s_addr = ROUTE_DEFAULT_DST;
    acl_policy_info->ip_family = IPV4_ADDR;
  }
  else if ( sm_ptr->instance == DSUMTS_RMSM_V6_SM_INSTANCE )
  {
    ip_info.ip_vsn = IP_V6;
    acl_policy_info->ip_family = IPV6_ADDR;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unsupported PDP type:%d",
                        rmsmi_info_ptr->profile_context_info.pdp_type);
    ASSERT(0);
    modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
    return -1;
  }

  acl_policy_info->iface.kind = DSS_IFACE_NAME; 
  /*lint -e64 */
  acl_policy_info->iface.info.name = DSS_IFACE_UMTS;
  /*lint +e64 */
  acl_policy_info->policy_flag = DSS_IFACE_POLICY_SPECIFIED;
  acl_policy_info->pdp_info = (acl_pdp_ctx_cfg_type)(rmsmi_info_ptr->profile_id);

  /*-------------------------------------------------------------------------
    bring_up has to be set to TRUE here so that the in_use flag is to TRUE
    when the IFACE gets assigned. Set proc ID to TE-DUN
  -------------------------------------------------------------------------*/
  acl_policy_info->bring_up    = TRUE;
  acl_policy_info->lookup_only = FALSE;
  acl_policy_info->proc_id     = PROC_ID_TE_DUN;

  /*-------------------------------------------------------------------------
    Prepare App ID.
  -------------------------------------------------------------------------*/
  acl_policy_info->app_identifier = APP_ID_TE_DUN;


  acl_policy_info->subs_id = (uint32)ds3gsubsmgr_subs_id_cm_to_ds(rmsmi_info_ptr->subs_id);

  /*-------------------------------------------------------------------------
    Do the interface bring-up. This will involve arbitration.
  -------------------------------------------------------------------------*/
  result =  ps_route_bring_up_iface_by_policy_ex
            (
              acl_policy_info,
              if_ptr,
              ps_errno,
              (void *)&(info_ptr->ppp_info),
              &net_down_reason              
            );

  DS_3GPP_MSG2_HIGH("bring_up_iface_by_policy: ret [%d], errno [%d]", 
                    result, *ps_errno);

  if( acl_policy_info != NULL)
  {
    modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
  }

  return result;
        
} /* dsumts_rmsmi_bring_up_um_iface_by_policy() */

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION  DSUMTS_RMSMI_REGISTER_WITH_FC

DESCRIPTION
  Used to register clients that need to report watermarks with FC

PARAMETERS
  Rm Dev instance 

DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_register_with_fc
(
  dsumts_rmsm_instance_type rm_dev_instance
)
{
  uint32 call_id = 0;
  sint15 err_no;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);

  if (sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm Info Ptr is NULL");
    return;
  }

  //Get the call id to register with FC
  if(0 != ps_phys_link_ioctl(PS_IFACE_GET_PHYS_LINK(
                             sm_info_ptr->um_iface_ptr),
                             PS_PHYS_LINK_IOCTL_GET_CALL_ID, 
                             (void*)&call_id,  
                             &err_no))
  {
    DS_3GPP_MSG0_ERROR("Cannot get the Call ID. ");
  }
  //Register with FC
  DS_3GPP_MSG1_HIGH("RMSM Client: Registering with FC for call_id:%d ", call_id);
  fc_post_reg_cmd_wm_trace(UNKNOWN_TECH, //tech id
                          call_id,
                          RMSM_CLIENT,
                          dsumts_rmsmi_report_wm_stats_cb, 
                          (uint32)rm_dev_instance);
} /*dsumts_rmsmi_register_with_fc*/

/*===========================================================================
FUNCTION  DSUMTS_RMSMI_DEREGISTER_WITH_FC

DESCRIPTION
  Used to register clients with FC 

PARAMETERS
  Rm Dev instance 

DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_deregister_with_fc
(
  dsumts_rmsm_instance_type rm_dev_instance
)
{
  uint32 call_id = 0;
  sint15 err_no;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;
  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);

  if (sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm Info Ptr is NULL");
    return;
  }


  //Get the call id to deregister with FC
  if(0 != ps_phys_link_ioctl
          (
            PS_IFACE_GET_PHYS_LINK(
            sm_info_ptr->um_iface_ptr,
            PS_PHYS_LINK_IOCTL_GET_CALL_ID, 
            (void*)&call_id,  
            &err_no
          )
     )
  {
    DS_3GPP_MSG0_ERROR("Cannot get the Call ID. ");
  }
  //Deregister with FC
  DS_3GPP_MSG1_HIGH("RMSM Client: Dergistering with FC for call_id:%d ", 
                    call_id);
  fc_post_deregister_cmd_wm_trace(call_id, RMSM_CLIENT);

} /*dsumts_rmsmi_deregister_with_fc*/

/*===========================================================================
FUNCTION  DSUMTS_RMSMI_REPORT_WM_STATS

DESCRIPTION
  Reports the watermark statistics by collecting relevant information from DSM

PARAMETERS
  Rm Dev Instance
  wm_stat_ptr - Pointer to structure where fc stores watermark info
  wm_id       - Watermark ID for which the stats are required
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dsumts_rmsmi_report_wm_stats
(
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance,
  fc_wm_stat_type *wm_stat_ptr,
  fc_wm_id_enum_type wm_id
)
{
  uint32 call_id = 0;
  sint15 err_no;
  dsumts_rmsmi_sm_info_type* sm_info_ptr = NULL;
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;

  /* Index to wm_info array provided by FC */
  int index = wm_stat_ptr->num_wms;


  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;

  sm_info_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);

  if (sm_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Sm Info Ptr is NULL");
    return;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_info_ptr->vobj_data_ptr;

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi Info Ptr is NULL");
    return;
  }

  if(index >= MAX_WM_CNT) {
      DS_3GPP_MSG0_ERROR("Max WM count reached. Cannot report statistics");
      return;
  }

  //Get the call_id
  if(0 != ps_phys_link_ioctl
         (
           PS_IFACE_GET_PHYS_LINK(
           sm_info_ptr->um_iface_ptr),
           PS_PHYS_LINK_IOCTL_GET_CALL_ID, 
           (void*)&call_id,  
           &err_no
         )
    )
  {
    DS_3GPP_MSG0_ERROR("Cannot get the Call ID. ");
  }

  switch (wm_id) {
  case WM_DIR_PS_SIO_DL:
    DS_3GPP_MSG0_MED("Reporting statistics for WM_DIR_PS_SIO_DL");
    /* Report the technology */
    wm_stat_ptr->wm_info[index].tech_id=UNKNOWN_TECH;

    /* Report the Rab ID */
    wm_stat_ptr->wm_info[index].call_id = call_id;

    /* Specify the WM for which the stats are being reported */
    wm_stat_ptr->wm_info[index].wm_id = WM_DIR_PS_SIO_DL;

    /* query DSM for wm stats */
    dsm_get_wm_stats(&(rmsmi_info_ptr->sio_tx_wmk), 
                     &(wm_stat_ptr->wm_info[index].wm_counts));
    wm_stat_ptr->num_wms++;
    break;

  case WM_DIR_PS_SIO_UL:
    DS_3GPP_MSG0_MED("Reporting statistics for WM_DIR_PS_SIO_UL");
    wm_stat_ptr->wm_info[index].tech_id=UNKNOWN_TECH;

    wm_stat_ptr->wm_info[index].call_id = call_id;
    wm_stat_ptr->wm_info[index].wm_id = WM_DIR_PS_SIO_UL;
    dsm_get_wm_stats(&(rmsmi_info_ptr->sio_rx_wmk), 
                     &(wm_stat_ptr->wm_info[index].wm_counts));
    wm_stat_ptr->num_wms++;
    break;

  default:
    break;
  }
} /*dsumts_rmsmi_report_wm_stats*/

/*===========================================================================
FUNCTION  DSUMTS_RMSMI_REPORT_WM_STATS_CB

DESCRIPTION
  Called by FC to gather watermark statistics

PARAMETERS
  SM Instance
  wm_stat_ptr - Pointer to structure where fc stores watermark info
DEPENDENCIES
  Called in FC TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_report_wm_stats_cb
(
  uint32 instance,
  fc_wm_stat_type *wm_stat_ptr
)
{
  /*------------------------------------------------------------
  Collect statistics for DL and UL watermarks
  --------------------------------------------------------------*/
  dsumts_rmsmi_report_wm_stats((dsumts_rmsm_rm_dev_instance_type)instance, 
                               wm_stat_ptr, WM_DIR_PS_SIO_DL);
  dsumts_rmsmi_report_wm_stats((dsumts_rmsm_rm_dev_instance_type)instance, 
                               wm_stat_ptr, WM_DIR_PS_SIO_UL);

} /*dsumts_rmsmi_report_wm_stats_cb*/
#endif /* FEATURE_QXDM_WM_TRACING */

/*====================================================================
  FUNCTION dsumts_rmsmi_entitlement_check_complete ()

  DESCRIPTION
    This API is called by client once the entitlement check is done. 
    The result of this check would determine whether to bring up the call 
    or not.

  PARAMETERS
    profile_num: (IN) Profile number to be used for the call
    result:      (IN) TRUE if entitlement check was successful
                      FALSE otherwise
    cb_data:          Pointer to Callback Data
    profile_info_changed (IN) Set to TRUE if profile contents changed 
                           during entitlement check
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
====================================================================*/
void dsumts_rmsmi_entitlement_check_complete
( 
  boolean                  result,
  dsumts_rmsm_rm_dev_instance_type  rm_dev_instance
)
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsmi_sm_info_type* sm_ptr = NULL;
  dsumts_rmsm_instance_type inst;

  inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE; 
  // Needs to change during Dual IP scenarios

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Rmsmi Info pointer is NULL. Assert");
  }

  /*---------------------------------------------------------------
   Only if we are waiting on entitlement check result, proceed. 
   Return Otherwise
  -------------------------------------------------------------*/
  switch (rmsmi_info_ptr->sm_type)
  {
    case DSUMTS_RMSM_SM_TYPE_PDP_IP:
      if( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state != 
            DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_ENTITLEMENT_CHECK_STATE )
    {
        DS_3GPP_MSG1_ERROR("Invalid RmSm IP Common state %d to "
                           "receive Entitlement Check Complete",
                            rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      return;
    }
    break;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
    case DSUMTS_RMSM_SM_TYPE_PDP_PPP:
      sm_ptr = dsumts_rmsmi_get_sm_info_ptr(&inst);
      if( sm_ptr != NULL &&
          sm_ptr->pdp_specific_info.pdp_ppp.state != 
          DSUMTS_RMSM_PPP_WAITING_FOR_ENTITLEMENT_CHECK_STATE )
    {
        DS_3GPP_MSG1_ERROR("Invalid RmSM PPP state %d to "
                           "receive Entitlement Check Complete",
                            sm_ptr->pdp_specific_info.pdp_ppp.state);
      return;
    }
    break;
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

  default: 
    DS_3GPP_MSG1_ERROR("Invalid state machine type %d",
                       rmsmi_info_ptr->sm_type);
    ASSERT(0);
    break;
  }

  /*------------------------------------------------------------
  If Entitlement check resulted in Failure, terminate call.
  -------------------------------------------------------------*/
  if( result == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Entitlement check Fail. Terminate call");
    dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_PPP_DOWN_EV );
    return;
  }

  /*-------------------------------------------------------------------
  Here we can safely assume Entitlement check resulted in Success
  ------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Entitlement check success. "
                    "Proceed with call origination");
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_WANTS_PKT_CALL_EV );

  return;
} /*dsumts_rmsmi_entitlement_check_complete*/


/*====================================================================
 FUNCTION dsumts_rmsmi_redial_check_complete ()
 
 DESCRIPTION
 This API is called by DS task after a client has indicated the redial 
 check is done. The result of this check would determine whether to 
 bring up a new call or terminate the RMSM.
 
 PARAMETERS
 profile_num: (IN) Profile number to be used for the call
 result:      (IN) TRUE if entitlement check was successful
 FALSE otherwise
 cb_data:          Pointer to Callback Data
 profile_info_changed (IN) Set to TRUE if profile contents changed 
                           during entitlement redial
 
 RETURN VALUE
 None
 
 DEPENDENCIES
 None
 
 SIDE EFFECTS
 None
 ====================================================================*/
void dsumts_rmsmi_redial_check_complete
( 
 boolean                  result,
  dsumts_rmsm_rm_dev_instance_type  rm_dev_inst
 )
{
  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  dsumts_rmsm_instance_type inst;

  /*------------------------------------------*/
  inst.rm_dev_instance = rm_dev_inst;
  inst.sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE; 
  // Needs to change during Dual IP.

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);

  if(rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Rmsmi Info pointer is NULL. Assert");
  }
  
  /*---------------------------------------------------------------
   Only if we are waiting on redial check result, proceed. 
   Return Otherwise
  ---------------------------------------------------------------*/
  switch (rmsmi_info_ptr->sm_type)
  {
    case DSUMTS_RMSM_SM_TYPE_PDP_IP:
      if( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state != 
            DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_REDIAL_CHECK_STATE )
      {
        DS_3GPP_MSG1_ERROR("Invalid state %d to receive Redial "
                           "Check Complete",
                            rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
        return;
      }
      break;
      
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
    case DSUMTS_RMSM_SM_TYPE_PDP_PPP:
        DS_3GPP_MSG1_ERROR("Unsupported state machine type for redial check %d",
                  rmsmi_info_ptr->sm_type);
        return;
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */
  
      
    default: 
      DS_3GPP_MSG1_ERROR("Invalid state machine type %d",
                         rmsmi_info_ptr->sm_type);
      ASSERT(0);
      break;
  }
  
  /*------------------------------------------------------------
   If Entitlement redial check resulted in failure, terminate call.
   -------------------------------------------------------------*/
  if( result == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Redial check Fail. Terminate call");
    dsumts_rmsm_post_event( &inst,UMTS_RMSM_NO_REDIALS_REMAINING_EV );
    return;
  }

  /*-------------------------------------------------------------------
   Here we can safely assume entitlement redial check resulted in success
   ------------------------------------------------------------------*/
  DS_3GPP_MSG0_HIGH("Redial check success."
                    "Proceed with call origination");
  dsumts_rmsm_post_event( &inst, UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV );
  
  return;
} /*dsumts_rmsmi_redial_check_complete*/

#ifdef FEATURE_DATA_WCDMA_PS
/*===========================================================================

FUNCTION dsumts_rmsmi_dl_fc_register

DESCRIPTION
  RMSM registers with Flow Control module for DL traffic.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_dl_fc_register
(
  void
)
{
  fc_in_cmd_type *fc_ptr = NULL;

/* Never register the FC before */
  if (!fc_reg)
  {
    fc_ptr = fc_get_in_cmd_buf();
    if (fc_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Cant get FC cmd buffer");
      ASSERT(0);
    }
    fc_ptr->cmd_hdr.cmd_id = FC_IN_REGISTER_CMD;
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    ds_tcb_ptr = rcinit_lookup_rextask("ds");

    if ( RCINIT_NULL == ds_tcb_ptr)
    {
      DS_3GPP_MSG0_ERROR( "RCINIT Lookup for ds tcb returned NULL");
      ASSERT(0);
    }
#else
    ds_tcb_ptr = &ds_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

    fc_ptr->cmd_data.fc_reg.tcb = ds_tcb_ptr;

    fc_ptr->cmd_data.fc_reg.q = &rmsm_fc_cmd_q;
    fc_ptr->cmd_data.fc_reg.sig = DS_RMSM_FC_SIG;

    fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.high = (uint8)cpu_high_threshold_for_DA;
    fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.low = (uint8)cpu_low_threshold_for_DA;
    fc_ptr->cmd_data.fc_reg.fc_cpu_threshold.off = (uint8)cpu_off_threshold;

    fc_ptr->cmd_data.fc_reg.period = 5;  // corresponding to cpu_avg_time
    fc_ptr->cmd_data.fc_reg.fc_rpt_cfg.processor_id = FC_MODEM_CPU;
    fc_ptr->cmd_data.fc_reg.fc_rpt_cfg.fc_cpu_reporting = FC_CPU_EVENT_BASED;
    fc_ptr->cmd_data.fc_reg.fc_rpt_cfg.fc_cpu_meas = FC_CPU_RAW_VALUE;
    
    fc_put_in_cmd(fc_ptr);
    DATA_MSG0_HIGH("RMSM Registered with FC");

    fc_reg = TRUE;

  }
} /*dsumts_rmsmi_dl_fc_register*/

/*===========================================================================

FUNCTION dsumts_rmsmi_dl_fc_deregister

DESCRIPTION
  RMSM deregisters with Flow Control module for DL traffic.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_dl_fc_deregister
(
  void
)
{
  fc_out_cmd_type *cmd_ptr;
  fc_in_cmd_type  *fc_ptr = NULL;

  /* Check if entity was registered to FC */
  if (fc_reg)
  {
    /* make sure RLC get cmd buffer before it goes ahead for
       FC de-registration */
    fc_ptr = fc_get_in_cmd_buf();

    if (fc_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Cant get FC cmd buffer");
      ASSERT(0);
    }

    fc_ptr->cmd_hdr.cmd_id = FC_IN_DEREGISTER_CMD;
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    ds_tcb_ptr = rcinit_lookup_rextask("ds");

    if ( RCINIT_NULL == ds_tcb_ptr)
    {
      DS_3GPP_MSG0_ERROR( "RCINIT Lookup for ds tcb returned NULL");
      ASSERT(0);
    }
#else
    ds_tcb_ptr = &ds_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */

    fc_ptr->cmd_data.fc_reg.tcb = ds_tcb_ptr;

    fc_ptr->cmd_data.fc_reg.q = &rmsm_fc_cmd_q;
    fc_ptr->cmd_data.fc_reg.sig = DS_RMSM_FC_SIG;

    fc_put_in_cmd(fc_ptr);

    /* Clean up FC cmd q */
    if (rmsm_fc_cmd_q.cnt)
    {
      while ((cmd_ptr = (fc_out_cmd_type *) (q_get(&rmsm_fc_cmd_q))) != NULL)
      {
        /* put item on queue */
        fc_free_out_cmd(cmd_ptr);
      }
    }

    DATA_MSG0_HIGH("RMSM De-registered with FC");
    fc_reg = FALSE;
  }
} /*dsumts_rmsmi_dl_fc_deregister*/

/*===========================================================================

FUNCTION dsumts_rmsmi_turn_off_data_aggregation

DESCRIPTION
  Turns off Data Aggregation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dsumts_rmsmi_turn_off_data_aggregation ( void )
{
  sio_ioctl_param_type wait_param;
  dsumts_rmsm_rm_dev_instance_type rm_dev;
  ds3g_siolib_port_e_type port_id;

  DS_3GPP_MSG0_HIGH("Turning off data aggregation");
  //If timer was running already, then clear it
  if( ds_smem_tx_delay != 0 )
  {
    //flush data and clear timer
    wait_param.wm_wait = FALSE;
    for (rm_dev=0; rm_dev < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++)
    {
      port_id = dsumts_rmsmi_get_sio_port_id(rm_dev);
      ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param,port_id);
      timer_clr(&smd_notify_delay_timer[rm_dev], T_MSEC);

      if (ds_3gpp_nv_manager_get_multi_pdn_dun_nv() == FALSE)
      {
        break;
      }
    }
  }
  ds_smem_tx_delay = 0;
} /* dsumts_rmsmi_turn_off_data_aggregation */

/*===========================================================================

FUNCTION dsumts_rmsmi_turn_on_data_aggregation

DESCRIPTION
  Turns On Data Aggregation

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dsumts_rmsmi_turn_on_data_aggregation ( void )
{
  sio_ioctl_param_type wait_param;
  dsumts_rmsm_rm_dev_instance_type rm_dev;
  ds3g_siolib_port_e_type port_id;

  DS_3GPP_MSG0_HIGH("Turning on data aggregation");
  //If timer was running already, then clear it
  if( ds_smem_tx_delay != 0 )
  {
    //flush data and clear timer
    wait_param.wm_wait = FALSE;

    for (rm_dev=0; rm_dev < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++)
    {
      port_id = dsumts_rmsmi_get_sio_port_id(rm_dev);
      ds3g_siolib_ex_ioctl(SIO_IOCTL_WM_SET_WAIT, &wait_param, port_id);
      timer_clr(&smd_notify_delay_timer[rm_dev], T_MSEC);

      if (ds_3gpp_nv_manager_get_multi_pdn_dun_nv() == FALSE)
      {
        break;
      }
    }
  }

  ds_smem_tx_delay = DEFAULT_RMSM_AGG_TIMER_TIMEOUT;
} /* dsumts_rmsmi_turn_on_data_aggregation */

/*===========================================================================

FUNCTION dsumts_rmsmi_process_fc_cmd_q

DESCRIPTION
  Handles the FC commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsumts_rmsmi_process_fc_cmd_q ( void )
{
  fc_out_cmd_type  *cmd_ptr = NULL;
  fc_out_cmd_enum_type fc_cmd;


  /* Loop through the commands from FC */
  while ((cmd_ptr = (fc_out_cmd_type *)(q_get(&rmsm_fc_cmd_q))) != NULL)
  {
    /* Store FC CMD locally before releasing the CMD buffer. */
    fc_cmd = cmd_ptr->cmd_hdr.cmd_id;

    /* put item on queue */
    fc_free_out_cmd(cmd_ptr);


    switch (fc_cmd) 
    {
      case FC_OUT_UP_CMD:
        dsumts_rmsmi_turn_off_data_aggregation();
        break;

      case FC_OUT_DN_CMD:
        dsumts_rmsmi_turn_on_data_aggregation();
        break;

      case FC_OUT_OFF_CMD:
      case FC_OUT_FREEZE_CMD:
        DS_3GPP_MSG1_HIGH("Received FC command is not handled: %d",fc_cmd);
        break;

      default:
        break;
    }
  }
} /*dsumts_rmsmi_process_fc_cmd_q*/
#endif /* FEATURE_DATA_WCDMA_PS */

/*===========================================================================
FUNCTION DSUMTS_RMSM_IS_DUN_CALL

DESCRIPTION
   This function is called by Ds3g to notify whether the call 
   is originated or answered by ATCoP.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE -- if it is SUCCESS
   FALSE -- if it is FAILURE
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsm_is_dun_call
(
  cm_call_id_type                 cm_call_id        /* call id of call    */
)
{
  ps_iface_type                 *iface_ptr;  /* Interface ptr for the call */  
  dsumts_rmsm_sm_instance_type     rmsm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
  ds_pdn_context_s     *pdn_context_p;
  ds_bearer_context_s  *bearer_context_p;

  DS_3GPP_MSG1_MED(" CM call id %d",cm_call_id);

  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(cm_call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if(!bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    /* only primary ATD calls can be made, so return FALSE */
    DS_3GPP_MSG0_ERROR("bearer is non-default, so returning FALSE");
    return FALSE;
  }

  pdn_context_p = (ds_pdn_context_s*)ds_bearer_cntxt_get_pdn_cntxt(
                                                           bearer_context_p);
  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG1_ERROR("No PDN context associated with call_id %d",
                       cm_call_id);
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Get the Interface ptr.
  -------------------------------------------------------------------------*/
  iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);
  rmsm_instance = dsumts_rmsmi_get_sm_instance_num_from_um_iface(iface_ptr);
  if(rmsm_instance != -1)
  {
    /* Valid instance denotes the call to be ATCoP call */
    DS_3GPP_MSG0_MED("DUN call ");
    return TRUE;
  }
  else
  {
    DS_3GPP_MSG0_MED("Not a valid RMSM instance, return FALSE");
    return FALSE;
  }
   
}/* dsumts_rmsm_is_dun_call */

/*===========================================================================
FUNCTION DSUMTS_RMSMI_SET_SIO_PORT_ID

DESCRIPTION
  This function is used to set the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev instance
  Port Id
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void 
dsumts_rmsmi_set_sio_port_id
(
  dsumts_rmsm_rm_dev_instance_type rm_dev,
  ds3g_siolib_port_e_type          port_id
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if (rmsmi_info_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  if (port_id < DS3G_SIOLIB_PORT_NONE ||
      port_id > DS3G_SIOLIB_PORT_MAX)
  {
    ASSERT(0);
    return;
  }

  rmsmi_info_ptr->port_id = port_id;

  DS_3GPP_MSG1_HIGH("Port Id is %d",port_id);
}

/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_SIO_PORT_ID

DESCRIPTION
  This function is used to get the port id corresponding to an Rm Device.

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   Port Id
    
SIDE EFFECTS
   None

===========================================================================*/
ds3g_siolib_port_e_type 
dsumts_rmsmi_get_sio_port_id(dsumts_rmsm_rm_dev_instance_type rm_dev)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = 
    dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if (rmsmi_info_ptr == NULL)
  {
    ASSERT(0);
    return DS3G_SIOLIB_PORT_NONE;
  }

  return rmsmi_info_ptr->port_id;
}/* dsumts_rmsmi_get_sio_portid */

/*===========================================================================
FUNCTION DSUMTS_RMSMI_GET_PPP_DEV

DESCRIPTION
  This function is used to map Rm Dev to the PPP Device

DEPENDENCIES
   None
  
PARAMETERS
  Rm Dev Instance

RETURN VALUE
   PPP Device
    
SIDE EFFECTS
   None

===========================================================================*/
ppp_dev_enum_type dsumts_rmsmi_get_ppp_dev
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  if (rm_dev == DSUMTS_RMSM_RM_DEV_INSTANCE_0)
  {
    return PPP_RM_DEV;
  }
  else if (rm_dev == DSUMTS_RMSM_RM_DEV_INSTANCE_1)
  {
    return PPP_RM_2_DEV;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Invalid Rm Dev %d", rm_dev);
    return PPP_INVALID_DEV;
  }
}

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_RM_DEV_FROM_PPP_DEV

  DESCRIPTION
    This function returns the Rm Device instance corresponding to a
    PPP device.
    
  PARAMETERS
    PPP Device

  RETURN VALUE
    Rm Device Instance    

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
dsumts_rmsm_rm_dev_instance_type dsumts_rmsmi_get_rm_dev_from_ppp_dev
(
  ppp_dev_enum_type ppp_dev
)
{
  if (ppp_dev == PPP_RM_DEV)
  {
    return DSUMTS_RMSM_RM_DEV_INSTANCE_0;
  }
  else if (ppp_dev == PPP_RM_2_DEV)
  {
    return DSUMTS_RMSM_RM_DEV_INSTANCE_1;
  }
  else
  {
    ASSERT(0);
    return DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
  }
}

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_REG_UM_EVENTS

  DESCRIPTION
    This function is used to register for Um events.
    
  PARAMETERS
    sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsmi_reg_um_events
(
  dsumts_rmsmi_sm_info_type       *sm_ptr
)
{
  int32 result = -1;
  dsumts_rmsmi_info_type *rmsmi_info_ptr;
  boolean retval = TRUE;

  if ( (sm_ptr == NULL) || (sm_ptr->um_iface_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "invaild input, event reg failed");
    return FALSE;
  }

  rmsmi_info_ptr = (dsumts_rmsmi_info_type*)sm_ptr->vobj_data_ptr;
  ASSERT(rmsmi_info_ptr);

  /*-----------------------------------------------------------------------
    Initialize the ps_iface event cback buffers for Um interface
  -----------------------------------------------------------------------*/
    sm_ptr->um_iface_active_out_of_use_buf_ptr = 
                                          ps_iface_alloc_event_cback_buf  
                                          ( 
                                            dsumts_rmsmi_um_event_handler_cb, 
                                            sm_ptr 
                                          );
    sm_ptr->um_iface_up_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_um_event_handler_cb,
                                          sm_ptr
                                        );

    sm_ptr->um_iface_down_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_um_event_handler_cb,
                                          sm_ptr
                                        );

    sm_ptr->um_flow_enable_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_um_event_handler_cb,
                                          sm_ptr
                                        );

    sm_ptr->um_flow_disable_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_um_event_handler_cb,
                                          sm_ptr
                                        );

  /*-----------------------------------------------------------------------
    Initialize the ps phys link event cback buffers for Um interface
  -----------------------------------------------------------------------*/
    sm_ptr->um_phys_link_flow_disable_buf_ptr = 
                                        ps_phys_link_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_phys_link_um_event_handler_cb,
                                          sm_ptr
                                        );

    sm_ptr->um_phys_link_flow_enable_buf_ptr = 
                                        ps_phys_link_alloc_event_cback_buf
                                        (
                                          dsumts_rmsmi_phys_link_um_event_handler_cb,
                                          sm_ptr
                                        );

    if(
       sm_ptr->um_iface_active_out_of_use_buf_ptr == NULL ||
       sm_ptr->um_iface_up_buf_ptr == NULL ||
       sm_ptr->um_iface_down_buf_ptr == NULL ||
       sm_ptr->um_flow_enable_buf_ptr == NULL ||
       sm_ptr->um_flow_disable_buf_ptr == NULL ||
       sm_ptr->um_phys_link_flow_enable_buf_ptr == NULL ||
       sm_ptr->um_phys_link_flow_disable_buf_ptr == NULL )
  {
      DS_3GPP_MSG1_ERROR("RMSM %d, ev buf alloc failed", sm_ptr->instance);
      ASSERT(0);
      /*lint -e527 */
      return FALSE;
    /*lint +e527 */
  }

  /*---------------------------------------------------------------------
    Register for Iface events.
  ---------------------------------------------------------------------*/
  DS_3GPP_MSG1_MED( "Registering for IFACE_ACTIVE_OUT_OF_USE_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr );

  result=ps_iface_event_cback_reg
         (
           sm_ptr->um_iface_ptr,
           IFACE_ACTIVE_OUT_OF_USE_EV,
           sm_ptr->um_iface_active_out_of_use_buf_ptr
         );
  
  if( result < 0 )
  {
    DS_3GPP_MSG0_ERROR( "IFACE_ACTIVE_OUT_OF_USE_EV Registration Failed");
    retval = FALSE;   
  } 


  DS_3GPP_MSG1_MED( "Registering for IFACE_DOWN_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result=ps_iface_event_cback_reg
         (
           sm_ptr->um_iface_ptr,
           IFACE_DOWN_EV,
           sm_ptr->um_iface_down_buf_ptr
         );
  
  if( result < 0 )
  {
    DS_3GPP_MSG0_ERROR("IFACE_DOWN_EV Registration Failed");
    retval = FALSE;
  } 

  DS_3GPP_MSG1_MED( "Registering for IFACE_UP_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result = ps_iface_event_cback_reg
           (
             sm_ptr->um_iface_ptr,
             IFACE_UP_EV,
             sm_ptr->um_iface_up_buf_ptr
           );

  if( result < 0)
  {
    DS_3GPP_MSG0_ERROR("IFACE_UP_EV Registration Failed");
    retval = FALSE;
  }

  DS_3GPP_MSG1_MED( "Registering for IFACE_FLOW_ENABLED_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result = ps_iface_event_cback_reg
           (
             sm_ptr->um_iface_ptr,
             IFACE_FLOW_ENABLED_EV,
             sm_ptr->um_flow_enable_buf_ptr
           );

  if( result < 0)
  {
    DS_3GPP_MSG0_ERROR("IFACE_FLOW_ENABLED_EV Registration Failed");
    retval = FALSE;
  }

  DS_3GPP_MSG1_MED( "Registering for IFACE_FLOW_DISABLED_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result = ps_iface_event_cback_reg
           (
             sm_ptr->um_iface_ptr,
             IFACE_FLOW_DISABLED_EV,
             sm_ptr->um_flow_disable_buf_ptr
           );

  if( result < 0)
  {
    DS_3GPP_MSG0_ERROR("IFACE_FLOW_DISABLED_EV Registration Failed");
    retval = FALSE;
  }

  /*------------------------------------------------------------------------
    Cache the phys link pointer. This is needed because the phys link and
    ifaces are dynamically bound. During call tear down, we will no longer
    be able to get the phys link from the iface.
  ------------------------------------------------------------------------*/
  sm_ptr->um_phys_lnk_ptr = PS_IFACE_GET_PHYS_LINK(sm_ptr->um_iface_ptr);

  if ( sm_ptr->um_phys_lnk_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("null phys link during ev reg");
  }

  DS_3GPP_MSG1_MED( "Registering for PHYS_LINK_FLOW_ENABLED_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result = ps_phys_link_event_cback_reg
           (
             PS_IFACE_GET_PHYS_LINK(sm_ptr->um_iface_ptr),
             PHYS_LINK_FLOW_ENABLED_EV,
             sm_ptr->um_phys_link_flow_enable_buf_ptr
           );

  if( result < 0)
  {
    DS_3GPP_MSG0_ERROR("PHYS_LINK_FLOW_ENABLED_EV Registration Failed");
    retval = FALSE;
  }

  DS_3GPP_MSG1_MED( "Registering for PHYS_LINK_FLOW_DISABLED_EV, "
                    "um_iface_ptr: %x ", sm_ptr->um_iface_ptr);

  result = ps_phys_link_event_cback_reg
           (
             PS_IFACE_GET_PHYS_LINK(sm_ptr->um_iface_ptr),
             PHYS_LINK_FLOW_DISABLED_EV,
             sm_ptr->um_phys_link_flow_disable_buf_ptr
           );

  if( result < 0)
  {
    DS_3GPP_MSG0_ERROR("PHYS_LINK_FLOW_DISABLED_EV Registration Failed");
    retval = FALSE;
  }

  return retval;

}/* dsumts_rmsm_ip_reg_iface_events() */

/*===========================================================================
  FUNCTION DSUMTS_RMSM_IP_DEREG_UM_EVENTS

  DESCRIPTION
    This function is used to de-register for Um events.
    
  PARAMETERS
   	sm_ptr: Pointer to the RmSm IP State Machine

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_dereg_um_events
(
  dsumts_rmsmi_sm_info_type       *sm_ptr
)
{

  if ( (sm_ptr == NULL) || (sm_ptr->um_iface_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR( "invalid input, event dereg failed" );
    return;
  }
  
  /*---------------------------------------------------------------------
    Deregister all the events with UM packet interface
  ---------------------------------------------------------------------*/
  ps_iface_event_cback_dereg(
                              sm_ptr->um_iface_ptr,
                              IFACE_ACTIVE_OUT_OF_USE_EV,
                              sm_ptr->um_iface_active_out_of_use_buf_ptr
                            );

  ps_iface_event_cback_dereg(
                              sm_ptr->um_iface_ptr,
                              IFACE_UP_EV,
                              sm_ptr->um_iface_up_buf_ptr
                            );

  ps_iface_event_cback_dereg(
                              sm_ptr->um_iface_ptr,
                              IFACE_DOWN_EV,
                              sm_ptr->um_iface_down_buf_ptr
                            );

  ps_iface_event_cback_dereg(
                              sm_ptr->um_iface_ptr,
                              IFACE_FLOW_ENABLED_EV,
                              sm_ptr->um_flow_enable_buf_ptr
                            );

  ps_iface_event_cback_dereg(
                              sm_ptr->um_iface_ptr,
                              IFACE_FLOW_DISABLED_EV,
                              sm_ptr->um_flow_disable_buf_ptr
                            );

  if ( sm_ptr->um_phys_lnk_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR( "null phys link ptr during dereg" );
  }

  ps_phys_link_event_cback_dereg(
                                  sm_ptr->um_phys_lnk_ptr,
                                  PHYS_LINK_FLOW_ENABLED_EV,
                                  sm_ptr->um_phys_link_flow_enable_buf_ptr
                                );
   
  ps_phys_link_event_cback_dereg(
                                  sm_ptr->um_phys_lnk_ptr,
                                  PHYS_LINK_FLOW_DISABLED_EV,
                                  sm_ptr->um_phys_link_flow_disable_buf_ptr
                                );

    /*-----------------------------------------------------------------------
    Initialize the ps_iface event cback buffers for Um interface
  -----------------------------------------------------------------------*/
    ps_iface_free_event_cback_buf  
    ( 
      sm_ptr->um_iface_active_out_of_use_buf_ptr 
    );
  
    ps_iface_free_event_cback_buf
    (
      sm_ptr->um_iface_up_buf_ptr
    );

    ps_iface_free_event_cback_buf
    (
      sm_ptr->um_iface_down_buf_ptr
    );
  
    ps_iface_free_event_cback_buf
    (
      sm_ptr->um_flow_enable_buf_ptr
    );
   
    ps_iface_free_event_cback_buf
    (
      sm_ptr->um_flow_disable_buf_ptr
    );

  /*-----------------------------------------------------------------------
    Initialize the ps phys link event cback buffers for Um interface
  -----------------------------------------------------------------------*/
   ps_phys_link_free_event_cback_buf
   (
     sm_ptr->um_phys_link_flow_disable_buf_ptr
   );

   ps_phys_link_free_event_cback_buf
   (
     sm_ptr->um_phys_link_flow_enable_buf_ptr
   );

   sm_ptr->um_iface_active_out_of_use_buf_ptr =
   sm_ptr->um_iface_up_buf_ptr = 
   sm_ptr->um_iface_down_buf_ptr =
   sm_ptr->um_flow_enable_buf_ptr = 
   sm_ptr->um_flow_disable_buf_ptr = 
   sm_ptr->um_phys_link_flow_enable_buf_ptr = 
   sm_ptr->um_phys_link_flow_disable_buf_ptr = NULL;

  return;
} /* dsumts_rmsm_ip_dereg_iface_events() */

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_GET_NEXT_RM_DEV_INSTANCE

  DESCRIPTION
    This function fetches the next Rm Dev Instance to be use for DUN calls.
    
  PARAMETERS
    void

  RETURN VALUE
    Next Rm Dev Instance

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
dsumts_rmsm_rm_dev_instance_type dsumts_rmsmi_get_next_rm_dev_instance
(
  void
)
{
  dsumts_rmsm_rm_dev_instance_type rm_dev = DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE;
  dsumts_rmsmi_info_type           *rmsmi_info_ptr = NULL;

  DS_3GPP_MSG1_HIGH( "Call being attempted on port %d", 
                     ds3g_siolib_get_active_port());  
  
  for (rm_dev = 0; rm_dev < DSUMTS_RMSM_MAX_RM_DEV_INSTANCES; rm_dev++ )
  {
    rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);
    if (rmsmi_info_ptr == NULL)
    {
      DS_3GPP_MSG1_HIGH( "Cannot allocate Rm Dev Instance: %d ",rm_dev ); 
      continue;
    }

    if (rmsmi_info_ptr->port_id != DS3G_SIOLIB_PORT_NONE)
    {
      DS_3GPP_MSG2_HIGH( "Rm Dev %d has an active data session. Port Id: %d ",
                         rm_dev, rmsmi_info_ptr->port_id ); 
    }
    else
    {
      DS_3GPP_MSG1_HIGH( "Rm Dev %d is free, use this instance for data session ",
                         rm_dev); 
      break;
    }
  } 
  
  DS_3GPP_MSG1_HIGH( "Rm Dev being returned: %d ", rm_dev); 
  return rm_dev;
}

/*===========================================================================
FUNCTION DSUMTS_RMSMI_VALIDATE_RM_DEV_INSTANCE

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether Rm Device instance is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsmi_validate_rm_dev_instance
(
  dsumts_rmsm_rm_dev_instance_type rm_dev
)
{
  if (ds_3gpp_nv_manager_get_multi_pdn_dun_nv() == TRUE)
  {
    if (rm_dev >= DSUMTS_RMSM_MAX_RM_DEV_INSTANCES ||
         rm_dev <= DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE)
    {
      DS_3GPP_MSG1_ERROR( "Invalid Rm Dev %d", rm_dev);
      return FALSE;
    }
  }
  else
  {
    if (rm_dev != DSUMTS_RMSM_RM_DEV_INSTANCE_0)
    {
      DS_3GPP_MSG1_ERROR( "Invalid Rm Dev %d", rm_dev);
      return FALSE;
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DSUMTS_RMSMI_VALIDATE_PPP_DEV

DESCRIPTION
  This function validates the given Rm Dev instance

DEPENDENCIES
   None
  
PARAMETERS
  Rm Device instance

RETURN VALUE
  TRUE or FALSE depending on whether PPP Device is valid
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsmi_validate_ppp_dev
(
  ppp_dev_enum_type ppp_dev
)
{
  if (ppp_dev == PPP_INVALID_DEV )
  {
    DS_3GPP_MSG1_ERROR( "Invalid PPP Dev %d", ppp_dev);
    return FALSE;
  }
  
  return TRUE;
}


dsumts_rmsmi_sm_info_type* dsumts_rmsmi_get_companion_sm_ptr
(
  dsumts_rmsm_instance_type* inst_ptr
)
{

  dsumts_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_umts_pdp_type_enum_type          pdp_type;
  dsumts_rmsm_sm_instance_type        companion_sm_instance = 
    DSUMTS_RMSM_INVALID_SM_INSTANCE;
  dsumts_rmsmi_sm_info_type*          companion_sm_ptr = NULL;
  dsumts_rmsm_instance_type           companion_inst;

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return NULL;
    /*lint +e527 */
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return NULL;
  }

  pdp_type = DSUMTS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    if (inst_ptr->sm_instance == DSUMTS_RMSM_V4_SM_INSTANCE)
    {
      companion_sm_instance = DSUMTS_RMSM_V6_SM_INSTANCE;
    }

    else if (inst_ptr->sm_instance == DSUMTS_RMSM_V6_SM_INSTANCE)
    {
      companion_sm_instance = DSUMTS_RMSM_V4_SM_INSTANCE;
    }

    companion_inst.sm_instance = companion_sm_instance;
    companion_inst.rm_dev_instance = inst_ptr->rm_dev_instance;

    companion_sm_ptr = dsumts_rmsmi_get_sm_info_ptr(&companion_inst);
   
  }

  return companion_sm_ptr;

}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

FUNCTION dsumts_rmsmi_at_escape_sequence_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the AT escape sequence is detected, in
   online data mode. The return value indicates whether online command mode
   should be entered (in this case, data transfer over the air should be
   suspended), or if we should stay in online data mode. 
   
   In our case (UCSD subtask), we simply forward this indication on to the
   UCSD protocol stack task.
         
DEPENDENCIES
   None
  
RETURN VALUE
   An indication of whether or not the MT should transition to online
   command mode or not.  In our case we will always return the indication
   that the MT should go to on-line command mode. 
   
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL dsat_mode_enum_type 
dsumts_rmsmi_at_escape_sequence_handler( void )
{
/*-------------------------------------------------------------------------*/
  //TODO:

  /* Tell 3GDSMGR that we want to go to on-line command mode */
  return (DSAT_ONLINE_CMD);
} /* ds_ucsd_at_escape_sequence_handler() */

/*===========================================================================

FUNCTION dsumts_rmsmi_at_return_to_online_data_handler

DESCRIPTION
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. It is invoked before the transition to
   return the SIO watermarks and callback function.
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
dsumts_rmsmi_at_return_to_online_data_handler
(
  /* Pointer to the watermark for data from SIO */
  dsm_watermark_type     **from_sio_wm_ptr,   

  /* Pointer to the watermark for data to SIO. */
  dsm_watermark_type     **to_sio_wm_ptr,
  
  /* Pointer to SIO RX mode-specific handler function */
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr
)
{
  dsumts_rmsmi_info_type *rmsmi_info_ptr = NULL;  
  dsumts_rmsm_rm_dev_instance_type rm_dev;  
  ds3g_siolib_port_e_type port_id = DS3G_SIOLIB_PORT_NONE;
  ds_call_type_e_type call_type = ds3g_siolib_ex_get_current_call_type();
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(call_type == DS_CALL_TYPE_PKT)
  {
    port_id = ds3g_siolib_ex_get_psd_port();
  }
  else
  {
    DS_SIO_MSG0_ERROR("Invalid call type");
    return;
  }

  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DS_SIO_MSG1_ERROR("Invalid Port Id %d", port_id);
    ASSERT(0);
    return;
  }

  if(NULL == from_sio_wm_ptr || NULL == to_sio_wm_ptr || NULL == mode_hdlr_sio_rx_func_ptr )
  {
    ASSERT(0);
  }

  rm_dev = dsumts_rmsmi_get_rm_dev_from_port_id(port_id);
  if (dsumts_rmsmi_validate_rm_dev_instance(rm_dev) == FALSE)
  {
    DATA_MSG1_HIGH("invalid rm_dev %d", rm_dev);
    return;
  }

  rmsmi_info_ptr = dsumts_rmsmi_get_rmsmi_info_ptr(rm_dev);

  if(rmsmi_info_ptr != NULL)
  {
    /* Switch statapath to that in call instance */
    *from_sio_wm_ptr            = &rmsmi_info_ptr->sio_tx_wmk;
    *to_sio_wm_ptr              = &rmsmi_info_ptr->sio_rx_wmk;
    mode_hdlr_sio_rx_func_ptr   = NULL;
  }
} /* ds_ucsd_at_return_to_online_data_handler() */

/*===========================================================================

FUNCTION dsumts_rmsmi_at_return_to_online_data_complete_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. The mode-specific handler should resume
   data transfer over the air.
   
   In our case, this simply involves sending an event notification down to
   the protocol stack.. 
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
dsumts_rmsmi_at_return_to_online_data_complete_handler ( void )
{
  //TODO:
} /* ds_ucsd_at_return_to_online_data_complete_handler() */

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS || (FEATURE_DATA_LTE)*/

