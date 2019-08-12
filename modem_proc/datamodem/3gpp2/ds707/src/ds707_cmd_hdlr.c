/** 
  @file ds707_cmd_hdlr.c 
  @brief 
   Handles 3GPP2 MH specific commands
*/
/** Contains API's to handle commands specific to 3GPP2 MH */

/*===========================================================================

                         D S 7 0 7 _ C M D _ H D L R
GENERAL DESCRIPTION
  This file contains the 3GPP2 MH command handling functionality.

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2014 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_cmd_hdlr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ------------------------------------------------------- 
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
03/14/14   jee     Created Module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "data_msg.h"
#include "dstask_v.h"
#include "ds707_cmd_hdlr.h"
#include "ps_rsvpi.h"
#include "ds707_epzid_mgr.h"
#include "ds707_pkt_mgr.h"

#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
#include "ds707_pdn_context.h"
#endif /* defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP) */

#include "ds707_pkt_mgr_iface.h"
#include "dsrlpi.h"
#include "ds707_dorm_timer.h"
#include "ds707_rmsm.h"

#ifdef FEATURE_EHRPD
#include "ds707_data_session_profile.h"
#include "ds707_ps_evt_hdlr.h"
#include "ds707_devmgr.h"
#endif /* FEATURE_EHRPD */

#include "ds707_iface4_hdlr.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dsgen_iface6_hdlr.h"
#include "ds707_ifaceAny_hdlr.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr.h"
#ifdef FEATURE_NIQ_EHRPD
#include "ds707_nw_init_qos_mgr.h"
#endif /* FEATURE_NIQ_EHRPD */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_HDR
#include "hdrds.h"
#include "dshdr_an_mgr.h"
#ifdef FEATURE_DS_CAM
#include "dscam.h"
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */

#include "ds707_async_mgr.h"
#include "dsrdud.h"

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
void ds707_process_cmds
(
  ds_cmd_type *cmd_ptr  
);

/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_CMD_HDLR_INIT

DESCRIPTION   Called once at power-up to init 3GPP2 MH cmd hdlr
 
DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_cmd_hdlr_init
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Register callback to process 3GPP2 MH commands
  -------------------------------------------------------------------------*/
  if(FALSE == ds_cmd_process_register(DS_MODULE_3GPP2_MH, ds707_process_cmds))
  {
    DATA_ERR_FATAL("3GPP2 MH cmd reg failed");
    return;
  }
} /* ds707_cmd_hdlr_init() */

/*===========================================================================
FUNCTION      DS707_PROCESS_CMDS

DESCRIPTION   To process 3gpp2 MH specific commands
 
DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_process_cmds
(
  ds_cmd_type *cmd_ptr  
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(NULL == cmd_ptr)
  {
    DATA_ERR_FATAL("cmd_ptr is NULL");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Processing command %d", 
                  cmd_ptr->hdr.cmd_id);

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_EPZID_CHANGE:
    case DS_CMD_707_PKT_EPZID_SOCM:
    case DS_CMD_707_PKT_EPZID_DELAY_TIMER_EXPIRED:
    case DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED:
    case DS_CMD_707_PKT_EPZID_HYS_DATA_READY:
    case DS_CMD_707_PKT_EPZID_HYS_SDB_DATA:
    case DS_CMD_707_PKT_MGR_SET_HAT_TIMER:
      ds707_epzid_mgr_process_cmd(cmd_ptr);
      break;

    /*-----------------------------------------------------------------------
      IS707-PKT Commands
    -----------------------------------------------------------------------*/
    case DS_CMD_707_PKT_PHYS_LINK_UP:
    case DS_CMD_707_PKT_PHYS_LINK_DOWN:
    case DS_CMD_707_PKT_IDM_CHANGE:
    case DS_CMD_707_PKT_SYS_CHG:
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    case DS_CMD_707_PKT_AUTO_REDIAL_TIMER_EXPIRED:
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
    case DS_CMD_707_TOGGLE_QNC_ENABLE:
    case DS_CMD_707_INITIATE_ALN:
    case DS_CMD_707_ABORT_DORMANT_PPP_TIMER_EXPIRED:
    case DS_CMD_707_EXIT_TC:
    case DS_CMD_707_UM_TX_HI_WM:
    case DS_CMD_707_UM_TX_LO_WM:
    case DS_CMD_707_UM_RX_HI_WM:
    case DS_CMD_707_UM_RX_LO_WM:
    case DS_CMD_707_RETRY_DELAY_ORIG:
    case DS_CMD_707_PDN_INACTIVITY_TIMER_CMD:
#if defined(FEATURE_HDR) && defined(FEATURE_DS_CAM)
    case DS_CMD_707_COLOC_INFO_CHANGED:
#endif /* FEATURE_HDR && FEATURE_DS_CAM */
    case DS_CMD_707_HDR_LN_STATUS:
    case DS_CMD_707_CM_NOTIFY:
    case DS_CMD_3GPP2_TM_NOTIFY:
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  case DS_CMD_707_MCFG_NOTIFY:
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
    case DS_CMD_707_NEW_CALL_ATTEMPT_TIMER:
    case DS_CMD_707_THROTTLE_INFO_UPDATE:

#ifdef FEATURE_EHRPD      
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
    case DS_CMD_707_LSD_TIMEOUT:
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
#ifdef FEATURE_DATA_3GPP2_VS
    case DS_CMD_707_MPIT_TIMEOUT:
#endif /* FEATURE_DATA_3GPP2_VS */
#endif /* FEATURE_EHRPD */
    case DS_CMD_707_ROAM_CHG:
    case DS_CMD_707_WAIT_FOR_EMERG_CALL_TIMEOUT:
    case DS_CMD_707_POST_GRACEFUL_PPP_TERM:

    case DS_CMD_707_PKT_CTA_TIMER_EXPIRED:
    case DS_CMD_707_PKT_HOLDDOWN_TIMER_EXPIRED:
    case DS_CMD_707_MANDATORY_APN_UPDATE:
      ds707_pkt_process_cmd(cmd_ptr);
      break;

#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_EHRPD_DUAL_IP)
    case DS_CMD_707_IFACE_OUT_USE:
    case DS_CMD_707_IFACE_IN_USE:
      ds707_pdncntx_process_cmd(cmd_ptr);
      break;
#endif /* FEATURE_EHRPD && FEATURE_DATA_EHRPD_DUAL_IP */

    case DS_CMD_707_PKT_DELAY_CONNECTION_CLOSE_TIMER:
      ds707_pkt_mgr_iface_process_cmd(cmd_ptr);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      1x RLP Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_RLP_INIT:
      dsrlp_process_cmd(cmd_ptr);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      Dorm Timer Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_707_PARSE_DORM_TIMER_SOCM:
      ds707_dorm_timer_process_cmd(cmd_ptr);
      break;

    /*-------------------------------------------------------------------------
      Cmds used for 3GPP2 iface bringup and teardown
    -------------------------------------------------------------------------*/
    case DS_CMD_707_RMSM_BRING_UP:
      ds707_rmsm_pkt_bring_up_cmd(cmd_ptr);
      break;

    case DS_CMD_707_SOCKETS_BRING_UP:
      ds707_socket_bring_up_cmd(cmd_ptr);
      break;

    case DS_CMD_707_RMSM_TEAR_DOWN:
      ds707_rmsm_tear_down_cmd(cmd_ptr);
      break;

    case DS_CMD_707_SOCKETS_TEAR_DOWN:
      ds707_socket_tear_down_cmd(cmd_ptr);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      RM IFACE Flow Control Specific Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_707_RM_IFACE_FLOW_ENABLED:
    case DS_CMD_707_RM_IFACE_FLOW_DISABLED:
      ds707_rmsm_process_rm_iface_flow_cmd(cmd_ptr);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      RMSM IS707-PKT Commands
    - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case DS_CMD_707_RMSM_RM_WANTS_PKT_CALL:
    case DS_CMD_707_RMSM_RM_IFACE_DOWN:
    case DS_CMD_707_RMSM_UM_IFACE_DOWN:
    case DS_CMD_707_RMSM_UM_PHYS_LINK_UP:
    case DS_CMD_707_RMSM_UM_PPP_DOWN:
    case DS_CMD_707_RMSM_UM_PHYS_LINK_GONE:
    case DS_CMD_707_RMSM_E2E_PPP_UP:
    case DS_CMD_707_RMSM_UM_RESYNCING:
    case DS_CMD_707_RMSM_RM_RESYNCING:
    case DS_CMD_707_RMSM_ENTITLEMENT_CHECK_COMPLETE:
#ifdef FEATURE_DS_MOBILE_IP
    case DS_CMD_707_RMSM_RM_PPP_UP:
    case DS_CMD_707_RMSM_UM_IFACE_UP:
    case DS_CMD_707_RMSM_UM_MIP_IP_ADDR_CHANGED:
#endif /* FEATURE_DS_MOBILE_IP */
      ds707_rmsm_process_cmd(cmd_ptr);
      break;

#ifdef FEATURE_EHRPD
    case DS_CMD_707_MANDATORY_APN_CHECK:
      ds707_data_session_check_mandatory_apns();
      break;
#endif /* FEATURE_EHRPD */

      /*---------------------------------------------------------------------
        DSSNET4 commands
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
      case DS_CMD_DSSNET4_PHY_LINK_UP_EV:
      case DS_CMD_DSSNET4_PHY_LINK_GONE_EV:
      case DS_CMD_DSSNET4_PHY_LINK_DOWN_EV:
      case DS_CMD_DSSNET4_LINK_UP_WITH_SIP_EV:
      case DS_CMD_DSSNET4_LINK_DOWN_EV:
      case DS_CMD_DSSNET4_LINK_RESYNC_EV:
#ifdef FEATURE_DS_MOBILE_IP
      case DS_CMD_DSSNET4_LINK_UP_WITH_MIP_EV:
      case DS_CMD_DSSNET4_MIP_UP_SUCCESS_EV:
      case DS_CMD_DSSNET4_MIP_FAILURE_EV:
#ifdef FEATURE_DS_MOBILE_IP_DEREG
      case DS_CMD_DSSNET4_MIP_DEREGED_EV:
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#endif /* FEATURE_DS_MOBILE_IP */
      case DS_CMD_DSSNET4_PPP_CLOSE_EV:
      case DS_CMD_DSSNET4_FLOW_DELETED_EV:
      case DS_CMD_DSSNET4_AUTH_STARTED_EV:
        dssnet4_sm_process_event(cmd_ptr);
        break;
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_PS_IPV6
#ifdef  FEATURE_DATA_IS707
    case DS_CMD_DSSNETANY_IFACE_DOWN_EV:
    case DS_CMD_DSSNETANY_IFACE_UP_EV:
    case DS_CMD_DSSNETANY_CONFIGURING_EV:
    case DS_CMD_DSSNETANY_OPEN_EV:
    case DS_CMD_DSSNETANY_TEARDOWN_EV:
    case DS_CMD_DSSNETANY_CLOSE_EV:
      dssnetAny_sm_process_event(cmd_ptr);
      break;
#endif
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR_QOS
    /*---------------------------------------------------------------------
    Secondary Packet manager related commands
    ---------------------------------------------------------------------*/
    case DS_CMD_707_QOS_FLOW_MGR_RESV_ON_NOTIFY_CMD:
    case DS_CMD_707_QOS_FLOW_MGR_RESV_OFF_NOTIFY_CMD:
    case DS_CMD_707_QOS_FLOW_MGR_RESV_ON_SENT_NOTIFY_CMD:
    case DS_CMD_707_QOS_FLOW_MGR_CONSOLIDATED_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_CONFIGURE_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_ACTIVATE_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_RESUME_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_SUSPEND_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_GO_NULL_CMD:
    case DS_CMD_707_SEC_PKT_FLOW_MODIFY_CMD:
    case DS_CMD_DQOS_MGR_PROC_RSVP_MSG_CMD:
    case DS_CMD_DQOS_MGR_REXMIT_TIMER_CMD:
    case DS_CMD_707_SEC_NETWORK_INITIATED_PKT_PPP_DOWN_CMD:
    case DS_CMD_707_SEC_PKT_GRANT_TIMER_EXPIRE_CMD:
    case DS_CMD_707_SEC_PKT_MGR_IFACE_CMD:
       ds707_sec_pkt_process_cmd( cmd_ptr );
       break;
              
#ifdef FEATURE_NIQ_EHRPD
    /*---------------------------------------------------------------------
    Network initiated QOS manager related commands
    ---------------------------------------------------------------------*/
    case DS_CMD_707_NW_INIT_QOS_PPP_RESYNC_TIMER_EXP_CMD:
    case DS_CMD_707_NW_INIT_QOS_INACTIVITY_TIMER_EXP_CMD:
      ds707_nw_init_qos_mgr_ds_cmd_processing(cmd_ptr);
      break;
#endif /*FEATURE_NIQ_EHRPD*/

#endif /*FEATURE_HDR_QOS*/

#ifdef FEATURE_HDR
      case DS_CMD_HDR_EVENT:
        hdrds_process_hdr_event_cmd( cmd_ptr );
        break;
#endif /* FEATURE_HDR */
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
    case DS_CMD_HDR_AN_PPP_EV:
    case DS_CMD_HDR_AN_PPP_START:
    case DS_CMD_HDR_CDMA_SUBS_AVAIL_EVENT:
       dshdr_an_process_cmd(cmd_ptr);
       break;

#ifdef FEATURE_DS_CAM
      /*------------------------------------------------------------------------
        Process CAM commands
      ------------------------------------------------------------------------*/
    case DS_CMD_CAM_SO33_CALL_CONNECTED:
    case DS_CMD_CAM_TIMER_EXPIRED:
    case DS_CMD_CAM_CALL_GONE_NULL:
    case DS_CMD_CAM_CALL_DORMANT:
    case DS_CMD_CAM_EVDO_COLOCATED:
	case DS_CMD_CAM_SRLTE_ENABLED:
    case DS_CMD_CAM_NO_EVDO_COLOCATED:
    case DS_CMD_CAM_FLOW_CTRL_TIMER_EXPIRED:
    case DS_CMD_CAM_NON_SO33_CALL_CONNECTED:
    {
      ds_process_cam_cmd( cmd_ptr->hdr.cmd_id );
      break;
    }
#endif /* FEATURE_DS_CAM */
#endif /* FEATURE_HDR */
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_EHRPD
    case DS_CMD_707_PS_EVT_PROC:
    ds707_ps_evt_hdlr_ds_cmd_process(cmd_ptr);
    break;
#endif /* FEATURE_EHRPD && FEATURE_DATA_IS707 */

#ifdef FEATURE_DATA_IS707_ASYNC  
    /*-----------------------------------------------------------------------
      IS707-Async Commands
    -----------------------------------------------------------------------*/
    case DS_CMD_707_ASYNC_IFACE_BRING_UP:
    case DS_CMD_707_ASYNC_IFACE_TEAR_DOWN:
    case DS_CMD_707_ASYNC_PHYS_LINK_TEAR_DOWN:
    case DS_CMD_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED:
    case DS_CMD_707_ASYNC_PTCL_OPENED:
    case DS_CMD_707_ASYNC_PTCL_CLOSED:
    case DS_CMD_707_ASYNC_ATZ:
    ds707_async_process_cmd(cmd_ptr);
    break;
#endif /* FEATURE_DATA_IS707_ASYNC */

   /*---------------------------------------------------------------------
     Following cmds added for RDUD module that runs in DS context.
     The purpose of this module is to enforce quick idle_timeout dormancy
     if TC came up due to an incoming call, to avoid keepign TC alive
     unncessarily long.
   ---------------------------------------------------------------------*/

    case DS_CMD_RDUD_INC_CALL_ACCEPTED:
    case DS_CMD_RDUD_PHYS_LINK_DOWN:
    case DS_CMD_RDUD_TIMER_EXPIRED:
    {
      ds_process_rdud_cmd( cmd_ptr->hdr.cmd_id );
      break;
    }

    
#ifdef FEATURE_DATA_OPTHO
    case DS_CMD_707_S101_COMPLETE_PREREG_HANDOFF:
      ds707_s101_process_cmd(cmd_ptr);
      break;
#endif /* FEATURE_DATA_OPTHO */

#if defined(FEATURE_EHRPD) && defined(FEATURE_DATA_IS707)
    case DS_CMD_707_DEVMGR_PCMT_TIMEOUT:
    case DS_CMD_707_DEVMGR_PPP_EV:
      ds707_devmgr_process_cmd(cmd_ptr);
      break;
#endif /* FEATURE_EHRPD */

    case DS_CMD_RSVP_SOCKET_EVENT:
      ps_rsvpi_process_socket_event(cmd_ptr);
      break;

     default:
      /*---------------------------------------------------------------------
      Invalid command, ignore.
      ---------------------------------------------------------------------*/
      break;
  }

  return;
} /* ds707_process_cmds() */

#endif /* FEATURE_DATA_IS707 */
