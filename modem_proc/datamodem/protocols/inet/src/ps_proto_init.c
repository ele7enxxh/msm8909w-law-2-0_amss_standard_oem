/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P R O T O _ I N I T . C

GENERAL DESCRIPTION
  Protocols Generic Initialization Functions.

Copyright (c) 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dss_ps.c_v   1.13   24 Feb 2003 18:32:12   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_proto_init.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/14    vl     Created the initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "nv_items.h"
#include "ps_utils.h"
#include "ps_proto_utils.h"
#include "ps_lo.h"
#include "ps_inet_init.h"
#include "ps_icmp.h"
#include "ps_proto_init.h"
#include "ps_iface_defs.h"
#include "ps_svc.h"
#include "ps_utils.h"
#include "ps_socki_defs.h"
#include "ps_tcp_config.h"
#include "ps_comp_logging_helper.h"
#include "ps_ipdc_config_ex.h"

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI */

#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ip6i_sm.h"
#include "ps_icmp6.h"
#include "ps_ifacei_addr_v6.h"
#endif

#ifdef FEATURE_DATA_PS_EAP
#include "ps_eap_init.h"
#endif

#ifdef FEATURE_DATA_PS_PPP
#include "ps_ppp_task.h"
#endif

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_api.h"
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PS_READ_NV()

DESCRIPTION
  This function performs NV read for items featured on the RUIM card.
  Current NVs read:
    RUIM EF name: UIM_CDMA_3GPD_TCPCONFIG
    NV_TCP_GRACEFUL_DORMANT_CLOSE_I
    NV_TCP_KEEPALIVE_IDLE_TIME_I

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_read_nv
(
  void
)
{
  nv_stat_enum_type nv_status;
  nv_item_type     *ps_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
  }

  /*-------------------------------------------------------------------------
    Retrieve the value of tcp_graceful_dormant_close flag from the NV
  -------------------------------------------------------------------------*/
  nv_status =
    ps_get_nv_item(NV_TCP_GRACEFUL_DORMANT_CLOSE_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_1("TCP graceful dormant close flag unavailable, nv_status %d,"
                    " using default", nv_status);
    /*-----------------------------------------------------------------------
      Set the global variable with default value and then set the NV item to
      default value
   ------------------------------------------------------------------------*/
    sock_config_cb.tcp_graceful_dormant_close = FALSE;
    ps_nv_item_ptr->tcp_graceful_dormant_close = FALSE;
  }
  else
  {
    sock_config_cb.tcp_graceful_dormant_close =
      ps_nv_item_ptr->tcp_graceful_dormant_close;
  }

  /*-------------------------------------------------------------------------
    Retrieve the value of tcp_keepalive_idle_time from the NV
  -------------------------------------------------------------------------*/
  nv_status = ps_get_nv_item(NV_TCP_KEEPALIVE_IDLE_TIME_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    LOG_MSG_INFO1_1("TCP keepalive idle time unavailable, nv_status %d,"
                    " using default", nv_status);
    /*-----------------------------------------------------------------------
      set the global variable with default value and then set the NV item
      to default value
     ----------------------------------------------------------------------*/
    sock_config_cb.tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME;
    ps_nv_item_ptr->tcp_keepalive_idle_time = TCP_KEEPALIVE_IDLE_TIME/(60 * 1000);
  }
  else
  {
    /*-----------------------------------------------------------------------
     The NV keepalive time is supplied in minutes. Convert it to milliseconds
    -----------------------------------------------------------------------*/
     sock_config_cb.tcp_keepalive_idle_time =
       ((ps_nv_item_ptr->tcp_keepalive_idle_time) * 60 * 1000);
  }

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
} /* ps_read_nv() */

/*===========================================================================
FUNCTION PS_MMGSDI_CLIENT_PROCESS_EVENT_CMD()

DESCRIPTION
  Called when an NV item read is to be performed.
  This function processes commands to read NV items placed in the SIM Card's
  elementary file system.

  Function is handled within PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ps_mmgsdi_client_process_event_cmd
(
  ps_cmd_enum_type      cmd_id,
  void                  *userdata_ptr
)
{
  mmgsdi_events_enum_type evt_recvd;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmd_id != PS_SIM_EVENT_HANDLER_CMD ||
       NULL == userdata_ptr)
  {
    LOG_MSG_ERROR_1("Invalid params, cmd_id %d", cmd_id);
    ASSERT(0);
    return;
  }

  evt_recvd = (mmgsdi_events_enum_type)userdata_ptr;

  switch(evt_recvd)
  {
#ifdef FEATURE_MMGSDI_SESSION_LIB
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      ps_read_nv();
      break;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

    default:
      LOG_MSG_ERROR_1("Invalid event %d", evt_recvd);
      break;
  }

} /* ps_mmgsdi_client_process_event_cmd() */

#endif /* FEATURE_MMGSDI */

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION PS_MMGSDI_CLIENT_REG_INIT()

DESCRIPTION
  This function registers PS as client to the MMGSDI module.
  If MMGSDI is not enabled, NV read is performed directly.

  Called from psi_init() in PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_mmgsdi_client_reg_init
(
  void
)
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_0("ps_mmgsdi_client_reg_init()");
  /*-------------------------------------------------------------------------
    All NVs are read in dssps_init() at init time. RUIM NVs will be re-read later.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MMGSDI
#if 0 /* This kind of registration is obsolete - DSMgr will act as a client */
  /*-------------------------------------------------------------------------
    Register as a MMGSDI client to read NV items on card power up.
  -------------------------------------------------------------------------*/
  if ( MMGSDI_SUCCESS !=
       mmgsdi_client_id_reg( ps_mmgsdi_client_id_reg_cback, 0) )
  {
    LOG_MSG_ERROR_0("MMGSDI client registration failed");
    return;
  }
#endif /* Obsolete - End */
  /*-------------------------------------------------------------------------
    Register PS cmd callback for concerned MMGSDI events.
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PS_SIM_EVENT_HANDLER_CMD,
                           ps_mmgsdi_client_process_event_cmd);

#else
  #error code not present
#endif /* FEATURE_MMGSDI */

  return;

} /* ps_mmgsdi_client_reg_init() */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION PS_PROTO_REFRESH_NV_EFS()

DESCRIPTION
  This function invokes all NV items of different protocol modules that was 
  changed due to SIM swap. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_nv_efs
(
  void
)
{
/*-------------------------------------------------------------------------*/
  /*------------------------------------------------------------------------
    Read the NV item for ICMP reply rate for both v4 and v6 before 
    initialising the token bucket.
  ------------------------------------------------------------------------*/
  icmp_read_tokens_nv();

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Do NV related initialization of IPv6 state machine.
  -------------------------------------------------------------------------*/
  ip6_sm_read_nv_efs();
#endif

#ifdef FEATURE_DATA_PS_PPP
  /*-------------------------------------------------------------------------
    Initialize PPP devices
  -------------------------------------------------------------------------*/
  ppp_task_nv_init();
#endif
}/* ps_proto_refresh_nv_efs() */

/*===========================================================================
FUNCTION PS_PROTO_REFRRESH_CB()

DESCRIPTION
  Callback function to be invoked when NV refresh event is received from 
  MCFG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_refresh_cb
(
  ps_utils_refresh_notify_info  ps_refresh_info
)
{
/*-------------------------------------------------------------------------*/
  LOG_MSG_INFO1_1("ps_proto_refresh_cb(): refresh_type : %d",
                   ps_refresh_info.refresh_type);

  /*------------------------------------------------------------------------
    Re-read the required NV items as NV refersh event is received. 
    For all the below mentioned 3 types, actions are the same for now.
  ------------------------------------------------------------------------*/  
  if( (PS_UTILS_REFRESH_TYPE_SLOT == ps_refresh_info.refresh_type) ||
      (PS_UTILS_REFRESH_TYPE_SUBS == ps_refresh_info.refresh_type) || 
      (PS_UTILS_REFRESH_TYPE_SLOT_N_SUBS == ps_refresh_info.refresh_type) )
  {
    ps_proto_refresh_nv_efs();
  }
  else
  {
    LOG_MSG_ERROR_1("utils_reg_call_back_fn() : Invalid refresh type : %d",
                    ps_refresh_info.refresh_type);
  }
} /* ps_proto_refresh_cb() */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================
FUNCTION PS_PROTO_INIT()

DESCRIPTION
  This function invokes all protocol initialization functions from all its 
  different modules. It also registers to PS framework for NV refresh 
  notification. In addition this registers with PS for SIM ready event. This 
  function is called from psi_init() in PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_proto_init
(
  void
)
{
#ifdef FEATURE_MODEM_CONFIG_REFRESH  
  ps_utils_refresh_reg_info  proto_reg_info;
  ps_utils_status_e_type     reg_status;
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  /*-------------------------------------------------------------------------*/ 
  LOG_MSG_INFO1_0("ps_proto_init(): "
                  "Protocol Service Initialization");
  
  /*------------------------------------------------------------------------    
    Read the NV item for ICMP reply rate for both v4 and v6 before 
    initialising the token bucket.
  ------------------------------------------------------------------------*/
  icmp_read_tokens_nv();

  /*-------------------------------------------------------------------------
    Initialize token bucket to rate limit icmp messages.
  -------------------------------------------------------------------------*/
  icmp_init_token_bucket();

  /*-------------------------------------------------------------------------
    Initialize inet module
  -------------------------------------------------------------------------*/
  ps_inet_init();

#ifdef FEATURE_DATA_PS_EAP
  /*-------------------------------------------------------------------------
    Initialize EAP services
  -------------------------------------------------------------------------*/
  eap_svc_init();
#endif /* FEATURE_DATA_PS_EAP */

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Do NV related initialization of IPv6 state machine.
  -------------------------------------------------------------------------*/
  ip6_sm_init();

  /*-------------------------------------------------------------------------
    Initialize token bucket to rate limit icmpv6 messages.
  -------------------------------------------------------------------------*/
  icmp6_init_token_bucket();
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Mobile IP Initialization
  -------------------------------------------------------------------------*/
  if(mip_meta_sm_init() != 0)
  {
    LOG_MSG_ERROR_0("psi_init(): "
                    "MIP meta sm init failed");
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Initialize the loopback interface for IPv6
  -------------------------------------------------------------------------*/
  lo_init();

#ifdef FEATURE_DATA_PS_PPP
  /*-------------------------------------------------------------------------
    Initialize PPP devices
  -------------------------------------------------------------------------*/
  ppp_task_init();
#endif /* FEATURE_DATA_PS_PPP */

  /*-------------------------------------------------------------------------
    Initialize PS Compression Logging Module
  -------------------------------------------------------------------------*/
  ps_comp_logging_init();

  /*-------------------------------------------------------------------------
    Initialize IPDC QXDM Config Module
  -------------------------------------------------------------------------*/
  ps_ipdc_config_init_ex();

  /*-------------------------------------------------------------------------
    NV read Initialization and MMGSDI registration.
  -------------------------------------------------------------------------*/
  ps_mmgsdi_client_reg_init();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  proto_reg_info.func_ptr = 
                 (ps_utils_refresh_notify_func_type) ps_proto_refresh_cb;
  proto_reg_info.user_data_ptr = NULL;

  reg_status = ps_utils_refresh_reg(proto_reg_info);
  if(PS_UTILS_REG_SUCCESS != reg_status)
  {
    LOG_MSG_ERROR_1("Registration failure status : %d", reg_status);
  }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
} /* ps_proto_init() */
