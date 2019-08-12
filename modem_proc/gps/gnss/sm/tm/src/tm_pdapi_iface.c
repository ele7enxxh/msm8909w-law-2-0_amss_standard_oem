/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM - PD API Interface

General Description
  This file contains implementations for TM Diag Interface
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_pdapi_iface.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/08/15   gk      Allow pdsm_get_best_pos during a E911 to support VoWiFi Calling
09/16/15   skm     Xtra Version check fix 
07/03/15   jv      Added QZSS Support
06/29/15   rk      Remove unwanted protocol callback upon PDSM_PA_ASST_GLONASS_POS_PROTOCOL PA event change. 
05/28/15   skm     Allow SET command for Premium services during ongoing session
05/26/15   jv      SBAS user preference changed from bool to uint8
03/14/15   rh      Added support for MultiSIM NV 
01/14/15   rk      Added LPP RSTD >24 measurement processing support 
01/08/15   skm     Return End event (NO_SESS) when session end called, when there is not active session.  
05/12/14   ah      Added support for Set XTRA Version Check
04/28/14   ah      Added support for handling premium service SAP disabled
04/17/14   skm     NV Async Read changes 
03/10/14   ah      Added support for Set Premium Services Cfg
01/30/14   ss      Allowing Set Param when Geofencing Session is running
11/13/13   rk      Missing initialization of pd_event_type causes sending garbage data to LBS.
10/03/12   rh      Added a few PA commands for SUPL configuration  
06/15/12   rk      Added NV set parameter support for PDSM_PA_ASST_GLONASS_POS_PROTOCOL PA event.
05/29/12   ssu     Removed tm_core_handle_best_avail_pos  
04/18/12   ssu     Added tm_core_handle_best_avail_pos  
06/02/12   gk      LPP config support
06/03/11   gk      Clear cached position  when delete param or gps lock is sent
06/02/11   rh      Added PDSM_LCS_CMD_SET_VX_CONFIG and Get/Set SUPL security
12/17/10   rc      Changed PDSM_PA_EFS_DATA to support root certificate injection w/o reboot
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
04/09/10   atien   enable standalone in E911 when camped on G/W for Ecall
04/09/10   atien   Correct possible null pointer dereference
03/03/10   ns      DSDS Support. Force supl cert read from efs if pdapi injects
                   new certificate
1/12/10    gk      send the efs write pa event to pdapi
09/30/09   gk     Support ODP 2.0
09/03/09   gk      validate the EFS file name length etc
08/11/09   gk      set mt flag correctly
07/26/09   atien   debug message update


07/23/09   gk      Support ODP 2.0
07/13/09   gk      process end session command even when no TM session
06/30/09   atien   Allow PDSM_CLIENT_TYPE_OEM to abort active session
6/29/09    gk      Send PD_EVENT call back when supl certificate is written.
2/18/09    gk      Client Id is getting reset in some cases.
03/06/09   atien   add missing case for NV write callback for MOLR vs AD Call Flow Select mask update
03/04/09   ns      Modified input param to tm_core_get_gps_state
03/02/09   jd      Added support for Wiper logging over RPC.
02/18/09   gk      Client Id is getting reset in some cases.
02/04/09   atien   MOLR vs AD Call Flow Select support
01/13/08   atien   XTRA-T Support
11/07/08   ank     For QoS=0, reject MS-B and Standalone only fix requests.
09/18/08   cl      Only allow dload/fix session overlapping 1x systems
09/3/08    gk      Fixed compiler warning
08/26/08   gk      On-Demand GPS Lock changes
08/13/08   gk      On-Demand force receiver off changes
08/08/08   cl      Enable non-encrypted lat/long when security is activated
07/25/08   gk      On-Demand Changes
04/25/08   gk      On-Demand Changes
04/09/08   cl      Allow Opt mode when phone is out of services
03/14/08   lt      END and DONE events added if protocol modules reject START.
03/13/08   cl      allow standalone to bypass security check
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
11/13/07   gk      Add support for SUPL ceritificate injection thorugh PDAPI
11/12/07   cl      Reject AGPS request while MS is OoS
09/14/07   gk      NMEA CONFIG set parameters support
09/06/07   gk      undo previous checkin. not valid for this branch
09/06/07   gk      Save the MO method when set via PDAPI
08/30/07   cl      Allow optimal mode when MS is in OoS
08/01/07   rw      Add support of XTRA feature
07/30/07   gk      Add handling for last position
05/27/07   rw      Add handling for app info from client
05/23/07   gk      added checks for end session
04/25/07   gk      Fixed the pa event type to match the new structure definition
04/03/07   gk      Add function for Time injection handling
01/31/07   cl      Modified function for LCS handling
12/07/06   cl      Added set/get parameter handling
12/06/06   gk      renamed set parm struct
09/15/06   cl      Initail Release
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <fs_errno.h>
#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include <math.h>
#include <string.h>
#include "msg.h"
#include "aries_os_api.h"
#include "tm_pdapi_iface.h"
#include "tm_pdapi_client.h"
#include "tm_cm_iface.h"
#include "tm_data.h"
#include "sm_nv.h"
#include "tm_nmea.h"
#include "aries_os_globals.h"
#include "gps_fs_api.h"
#include "pdapibuf.h"
#include "mgp_api.h"
#include "tm_on_demand.h"
#include "tm_nv.h"
#include "lbs_sdp.h"
#include "lsmp_api.h"
#include "sm_oemdre.h"

#ifdef FEATURE_TM_SECURITY
#include "tm_security.h"
#endif /* FEATURE_TM_SECURITY */

#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */
#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#include "tm_xtra_t.h"
#endif /* FEATURE_CGPS_XTRA_T */

extern void tm_wiper_core_position_report(t_wiper_position_report_ex_struct_type * position_report_ptr);

#include "tm_xspi.h"
#include "gm_api.h"

#ifdef FEATURE_CGPS_XTRA_T
#define XTRA_T_HANDSHAKE_KEY_0 0x41
#define XTRA_T_HANDSHAKE_KEY_1 0x52
#define XTRA_T_HANDSHAKE_KEY_2 0x54
#define XTRA_T_HANDSHAKE_KEY_3 0x58

uint8   xtra_t_key_idx = 0;
#define XTRA_T_RCVD_KEY_MAX (4)
uint8   xtra_t_rcvd_keys[XTRA_T_RCVD_KEY_MAX+1];
#endif /* FEATURE_CGPS_XTRA_T */

/*===========================================================================
                 Data Definition
===========================================================================*/

/* Flag to check if SM-NV init has been called */
boolean tm_nv_initialized = FALSE;

/* Handle used for SM NV write */
uint32  tm_nv_rdrw_handle = SM_NV_CLIENT_HANDLE_ERROR;
nv_item_type  tm_pa_nv_write_buffer;
nv_item_type  tm_pa_nv_read_buffer;

#ifdef FEATURE_UIM_SUPPORT_LBS
tm_core_ruim_read_buffer tm_core_ruim_nv_buffer;
#ifdef FEATURE_CGPS_XTRA
tm_xtra_ruim_nv_buffer ruim_xtra_nv_buffer;
#endif /* FEATURE_CGPS_XTRA */
#endif /* FEATURE_UIM_SUPPORT_LBS */
static prtl_event_u_type  ruim_event_payload;
static gps_fs_u_cmd_type z_GpsFsCmd;
#ifdef FEATURE_TM_SECURITY
tm_sec_enable_data_s_type  sec_enable_data;
#endif /* FEATURE_TM_SECURITY */

/*===========================================================================

FUNCTION tm_core_efs_file_resp_handler

DESCRIPTION
  This function is called by the GPS_FS task to send back the result of
  file operation (write/append/delete) performed. This function updates
  V2 delayed response.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void tm_core_efs_file_resp_handler (uint32 q_FileOperationResult)
{
  pdsm_pa_info_s_type pa_info_data;
  char   *input_fname    = (char*)(&z_GpsFsCmd.p_FileName[0]);

  MSG_HIGH("TM_CORE EFS FILE RESP HANDLER result = %d", q_FileOperationResult, 0, 0);
  if (z_GpsFsCmd.p_DataPtr != NULL)
  {
    (void)pdsm_freebuf((char*) z_GpsFsCmd.p_DataPtr);
  }

  
    memset(&pa_info_data, 0, sizeof(pa_info_data));
    pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
    pa_info_data.pa_event         = PDSM_PA_EFS_DATA;

    pa_info_data.pa_info.efs_data.efs_data_file_operation = z_GpsFsCmd.u_FileOP;

  if(ENOERR == q_FileOperationResult)
  {

    tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_PA_CMD_ERR_NOERR);
  }
  else
  {
    MSG_ERROR("File Operation error %d", q_FileOperationResult, 0, 0);
    tm_pdapi_pa_event_callback_ex(&pa_info_data, PDSM_PA_CMD_ERR_OTHER);
  }

  
  /* If a SUPL Cert is being updated, send an update to SUPL */
  if( (strlen(input_fname) >= TM_UMTS_SUPL_ROOT_CERT_DIR_PATH_LEN)  &&
      (strncmp(input_fname,
               TM_UMTS_SUPL_ROOT_CERT_DIR, 
               TM_UMTS_SUPL_ROOT_CERT_DIR_PATH_LEN) == 0) )
  {    
    int     prtlTableIndex = 0;
    prtl_event_u_type event_payload;
    
    /* Sending a callback to SUPL to replace the certificate in the cache with 
       the one sent in the set_parameters request */
    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);
    if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
    {
      if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
      {
        event_payload.pa_event_payload.pa_event = PDSM_PA_EFS_DATA;
        event_payload.pa_event_payload.pa_info_ptr =  &pa_info_data.pa_info;
        (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                 PRTL_EVENT_TYPE_PA,
                                                                 &event_payload);
      }
      else
      {
        MSG_MED("SUPL: Callback not registered. Cannot update cache.",0,0,0);
      }
    }
  }
}


/*===========================================================================

FUNCTION tm_core_get_new_sess_handle

DESCRIPTION
  This function generates a new MO session handle for incoming MO fix request
  and return reserved session handle for NI/MT fix request.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static tm_sess_handle_type tm_core_get_new_sess_handle(pdsm_mt_initiator_e_type mt_initiator, tm_prtl_type active_prtl)
{
  tm_sess_handle_type sess_handle = TM_SESS_HANDLE_NONE;

  if (mt_initiator == MT_UP)
  {
    if (active_prtl == TM_PRTL_TYPE_1X_UP)
    {
      sess_handle = TM_SESS_HANDLE_1X_UP_NI;
    }
    else if (active_prtl == TM_PRTL_TYPE_UMTS_UP)
    {
      sess_handle = TM_SESS_HANDLE_UMTS_UP_NI;
    }
  }
  else if (mt_initiator == MT_CP)
  {
    if (active_prtl == TM_PRTL_TYPE_1X_CP)
    {
      sess_handle = TM_SESS_HANDLE_1X_CP_MT;
    }
    else if (active_prtl == TM_PRTL_TYPE_UMTS_CP)
    {
      sess_handle = TM_SESS_HANDLE_UMTS_CP_NI;
    }
  }

  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    sess_handle = tm_core_new_mo_sess_handle();
  }
  return sess_handle;
}


/*===========================================================================

FUNCTION tm_core_get_pos_para_check 
  This function is called before starting a standalone or a UP session to get
  a GPS position. We need to check whether the get_position request can be
  handled by the position engine by checking the parameters passed and the 
  state of the GPS engine.

DESCRIPTION

DEPENDENCIES

RETURN VALUE  Return error code or success

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_get_pos_para_check
(
  const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr
)
{
  tm_cm_phone_state_info_s_type phone_state_info;

  /* Perform necessary check to see if incoming get pos request should be granted */

  /* First check if the task is active */
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    return PDSM_PD_CMD_ERR_OFFLINE_S;
  }

  /***************************************************************************
   * Perform E911 related checks (E911 call orgination, E911 CBW, ...etc     *
   ***************************************************************************/
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    #ifdef FEATURE_MO_GPS_FOR_ECALL
    /* If serving system if CDMA/HDR, and we are in an E911 state, reject all
     * MO GPS requests */
    if (((phone_state_info.srv_system == CGPS_SRV_SYS_CDMA) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_HDR) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_NONE) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_OTHER)))
    {
      MSG_HIGH("Phone in E911 state in %d. cant handle get_position()",
          phone_state_info.srv_system,
          0, 0);
      return PDSM_PD_CMD_ERR_STATE_S;
    }

    /* if phone is E911 state in GSM/W, check the emergency services support NV.
       If set to low priority, accept any MO incoming calls even in emergency */
    if ((get_pos_param_ptr != NULL) && (get_pos_param_ptr->option_ptr != NULL))
    {
      MSG_HIGH("Phone in E911 state in %d, operation mode = %d, Emerg serv support %d",
                phone_state_info.srv_system,
                get_pos_param_ptr->option_ptr->operation_mode, 
                tm_core_info.config_info.aagps_emergency_services_spprt);
      /* accept MO session in E911 only if NV item is set to accept low priority */
      if (tm_core_info.config_info.aagps_emergency_services_spprt != AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY)
      {
        MSG_HIGH("Phone in E911 state in UMTS. Reject MO request ",0,0,0);
        return PDSM_PD_CMD_ERR_STATE_S;
      }
      else
      {
        MSG_HIGH("Phone in E911 state in UMTS. Accept MO request ",0,0,0);
      }
    } /* if get_pos_param_ptr != NULL */
    else
    {
      MSG_HIGH("Phone in E911 state in %d , defer request to modules",
        phone_state_info.srv_system, 0, 0);
    }
    #else
    /* if phone is E911 state in GSM/W, we check the emergency services support NV item. If
       set to low priority, accept any AGPS incoming calls even in emergency */
    if (((phone_state_info.srv_system == CGPS_SRV_SYS_CDMA) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_HDR) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_NONE) ||
        (phone_state_info.srv_system == CGPS_SRV_SYS_OTHER)))
    {
      MSG_HIGH("Phone in E911 state in %d.",
          phone_state_info.srv_system,
          0, 0);
      return PDSM_PD_CMD_ERR_STATE_S;
    }
    else
    {
      if ((get_pos_param_ptr != NULL) && (get_pos_param_ptr->option_ptr != NULL))
      {
        MSG_HIGH("Phone in E911 state in %d, operation mode = %d, Emergency services support %d. ",
        phone_state_info.srv_system,
          get_pos_param_ptr->option_ptr->operation_mode, tm_core_info.config_info.aagps_emergency_services_spprt);
        /* accept standalone session in E911 only if NV item is set to accept low priority */
        if (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY)
        {
          if (tm_core_info.config_info.aagps_emergency_services_spprt != AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY)
          {
            MSG_HIGH("Phone in E911 state in UMTS. Reject Standalone request ",0,0,0);
            return PDSM_PD_CMD_ERR_STATE_S;
          }
          else
          {
            MSG_HIGH("Phone in E911 state in UMTS. Accept Standalone request ",0,0,0);
          }
        } /* if standalone */
      } /* if get_pos_param_ptr != NULL */
      else
      {
        MSG_HIGH("Phone in E911 state in %d , defer request to modules",
          phone_state_info.srv_system, 0, 0);
      }
    }
    #endif /* FEATURE_MO_GPS_FOR_ECALL */
  } /* If E911 */

  if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_MI)
    ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_ALL))
  {
    MSG_HIGH("MI GPS is locked", 0, 0, 0);
    return PDSM_PD_CMD_ERR_GPS_LOCK_S;
  }

  /* ayt_todo - make sure no code change is need to prevent MT-LR in LPP CP */

  /* [LPP CP FRS] LPP CP supports only NI-LR but not MO-LR 
   * 
   * If in LTE, LPP CP feature enabled then
   *    Block MO Get Postion request with the following conditions:
   *      1. if MO Method is Control Plane
   *      2. if Operation Mode NOT Standalone
   *
   *   (ayt_todo - skip this restriction for now)
   *    If MO-CP and Standalone and
   *       Make sure of all GNSSs , only GPS is supported. 
   * Endif
   */
  if ((phone_state_info.srv_system == CGPS_SRV_SYS_LTE) &&
      (tm_core_get_lpp_cp_enable() == TRUE))
  {
    if ((tm_core_info.config_info.u_mo_method == MO_CP) && 
        (get_pos_param_ptr != NULL) &&
        (get_pos_param_ptr->option_ptr != NULL))
    {    
        if (get_pos_param_ptr->option_ptr->operation_mode != PDSM_SESSION_OPERATION_STANDALONE_ONLY)
        {
          return PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED;
        }
/*  skip this restriction for now
        if (get_pos_param_ptr->option_ptr->mode_info.mode != PDSM_SESS_JGPS_TYPE_GPS)
        {
          return PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED;
        }
*/
    }
  }

  /* MO service interaction checking */
  if( (get_pos_param_ptr != NULL)
    &&(get_pos_param_ptr->option_ptr != NULL)
    &&(get_pos_param_ptr->qos_ptr != NULL))
  {
    if( ( (phone_state_info.srv_system == CGPS_SRV_SYS_NONE) ||
          (phone_state_info.srv_system == CGPS_SRV_SYS_HDR))
      &&( (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSASSISTED)
        ||(get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_REF_POSITION))
      )
    {
      MSG_HIGH("Phone is out of service, can not dispatch request to proper protocol module", 0, 0, 0);
      return PDSM_PD_CMD_ERR_NO_CONNECT_S;
    }

    if( phone_state_info.srv_system == CGPS_SRV_SYS_CDMA
      ||phone_state_info.srv_system == CGPS_SRV_SYS_HDR)
    {

      /* Then check if any there is any active PD session */

      if((get_pos_param_ptr->option_ptr->session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
         &&(tm_core_session_active())
         &&(tm_core_is_internal_session_on()  == FALSE)
         &&(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD))
      {
        MSG_HIGH("Dload session is in progress", 0, 0, 0);
        return PDSM_PD_CMD_ERR_BUSY_S;
      }


      if((get_pos_param_ptr->option_ptr->session != PDSM_PD_SESS_TYPE_DATA_DLOAD)
         &&(tm_core_session_active())
         &&(tm_core_is_internal_session_on()  == FALSE)
         &&(tm_core_info.session_info.pd_param.pd_option.session != PDSM_PD_SESS_TYPE_DATA_DLOAD))
      {
        MSG_HIGH("Fix session is in progress", 0, 0, 0);
        return PDSM_PD_CMD_ERR_BUSY_S;
      }

      /* disable till nvitems.h is updated */
      /* If operation mode is anything other than standalone/msbased/ref-posn,
       * and up-1x_msa is turned off, return mode not supported */
      if (  ! ((get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY) ||
               (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED)         ||
               (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_REF_POSITION))
           && (tm_core_info.config_info.up_1x_msa_disable != FALSE)
         )
      {
        MSG_ERROR("Operation mode %d turned off in 1x/HDR",
          get_pos_param_ptr->option_ptr->operation_mode, 0, 0);
        return PDSM_PD_CMD_ERR_MODE_NOT_SUPPORTED_S;
      }
    } /* srv_system == CGPS_SRV_SYS_CDMA || srv_system == CGPS_SRV_SYS_HDR */
    else
    {
      if( tm_core_session_active()
          &&(tm_core_is_internal_session_on()  == FALSE)
        )
      {
        MSG_HIGH("Fix session is in progress", 0, 0, 0);
        return PDSM_PD_CMD_ERR_BUSY_S;
      }
    }

    if((get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED ||
        get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY) &&
       (get_pos_param_ptr->qos_ptr->gps_session_timeout == 0))
    {
      MSG_HIGH("QoS timeout 0 for MS-B/Standalone", 0, 0, 0);
      return PDSM_PD_CMD_ERR_QOS_P;
    }
  }
  else /* NI service interaction checking */
  {
    if(tm_core_session_active()
      &&(tm_core_info.internal_session == FALSE)
      )
    {
      return PDSM_PD_CMD_ERR_BUSY_S;
    }
  }

#ifdef FEATURE_TM_SECURITY
  if ((get_pos_param_ptr != NULL) && (get_pos_param_ptr->option_ptr != NULL))
  {
#ifdef FEATURE_GPSONE_SECURITY_OPEN_STANDALONE
    if(get_pos_param_ptr->option_ptr->operation_mode != PDSM_SESSION_OPERATION_STANDALONE_ONLY)
    {
#endif
      if(tm_sec_get_pos_check(get_pos_param_ptr) == FALSE)
      {
        MSG_HIGH("Security Check Failed, Access Denied", 0, 0, 0);
        return PDSM_PD_CMD_ERR_AUTH_FAIL_S;
      }
#ifdef FEATURE_GPSONE_SECURITY_OPEN_STANDALONE
    }
#endif
  }
#endif /* FEATURE_TM_SECURITY */

  /* Need CM interface to complete this check, for now always allow incoming request */
  return PDSM_PD_CMD_ERR_NOERR;
}

/*===========================================================================

FUNCTION tm_core_resume_get_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_resume_get_pos(void)
{
  int                               prtl_tbl_idx = -1;
  tm_prtl_type                      active_prtl;
  tm_prtl_type                      orig_active_prtl;
  prtl_start_sess_req_result_e_type reqResult;
  tm_sess_req_start_param_s_type    start_actions;
  boolean                           retVal = FALSE;
  tm_sess_handle_type               mo_sess_handle;
  tm_pdapi_pd_event_s_type          pd_event_type;
  tm_core_cached_pos_s_type         cached_pos_info;
  prtl_ans_u_type                   prtl_answer;
  tm_pdapi_get_pos_param_s_type     get_pos_param;

  /* Init */
  memset(&pd_event_type, 0, sizeof(pd_event_type));

  tm_core_info.session_info.queued_request = FALSE;
  get_pos_param.client_id   = tm_core_info.session_info.queued_get_pos_param.client_id;
  get_pos_param.client_type = tm_core_info.session_info.queued_get_pos_param.client_type;
  get_pos_param.option_ptr  = &(tm_core_info.session_info.queued_get_pos_param.option);
  get_pos_param.qos_ptr     = &(tm_core_info.session_info.queued_get_pos_param.qos);
  get_pos_param.mt_initiator = tm_core_info.session_info.queued_get_pos_param.mt_initiator;
  tm_core_info.internal_session = tm_core_info.session_info.queued_get_pos_param.internal_session;

  /* if the get pos request is last position, return a fix if availalbe.
    otherwise return error
    */
  if (get_pos_param.option_ptr->session == PDSM_PD_SESS_TYPE_LATEST)
  {
    retVal = tm_core_get_last_position(&cached_pos_info);
    if (retVal == TRUE)
    {
      pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param.client_id;

      tm_pdapi_pos_log_generation(cached_pos_info.pd_event_type,
                                  &cached_pos_info.pd_info,
                                  cached_pos_info.z_pos_flags.b_IsSft,
                                  cached_pos_info.z_pos_flags.b_IsKf,
                                  TM_PDAPI_SESS_TYPE_NONE);

      tm_pdapi_pos_event_callback(cached_pos_info.pd_event_type,
                                  &cached_pos_info.pd_info,
                                  TM_PDAPI_SESS_TYPE_NONE);
    }
    else
    {
      pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
      pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_PD_END_FIX_ERROR;
      tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

    }
    pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param.client_id;
    pd_event_type.e_pd_event = PDSM_PD_EVENT_DONE;
    tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

    return retVal;
  }


  /* If a get pos request is
     1. Fix session
     2. MSB or Standalone
     Let standalone handles it first */
  if( (get_pos_param.option_ptr->session != PDSM_PD_SESS_TYPE_DATA_DLOAD)
    &&( (get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY)
      ||(get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED)))
  {
    /* Default allow smart MSB */
    prtl_answer.ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;

    /*
       For MSB fix request, we need to query protocol sub-modules to see if they want standalone module to
       handle the MSB fix request
    */
    if(get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED)
    {
      active_prtl = tm_core_get_active_prtl_module(get_pos_param.mt_initiator);
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

      if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].general_query_fp)
        {
          (void)tm_core_info.prtl_func_cb_table[prtl_tbl_idx].general_query_fp(PRTL_QUERY_SMART_MSB,
                                                                               &prtl_answer);
        }
      }
    }
    else
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }

    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */
    if(prtl_answer.ans_smart_msb == PRTL_ANS_SMART_MSB_ALLOWED)      
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }
  }
  else
  {
    active_prtl = tm_core_get_active_prtl_module(get_pos_param.mt_initiator);
    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */

    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

    /* If fix request is of any optimal mode and MS is OoS, have standalone handle the request since we know
       for sure that no data connection can be brought up */
    if( (prtl_tbl_idx < 0)
      &&( (get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED)
        ||(get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY)
        ||(get_pos_param.option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA)))
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }
  }

  MSG_MED("tm_core dispatches get_pos req to [%d]", active_prtl, 0, 0);

  if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    mo_sess_handle = tm_core_get_new_sess_handle(get_pos_param.mt_initiator, orig_active_prtl);

    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp)
    {
#ifdef FEATURE_TM_SECURITY
      tm_sec_update_random_challenge(TM_SEC_RND_UPDATE_SOURCE_GET_POS);
#endif /* FEATURE_TM_SECURITY */


      tm_core_info.session_info.pd_param.client_id   = get_pos_param.client_id;
      tm_core_info.session_info.pd_param.client_type = get_pos_param.client_type;
      memscpy((void *)&(tm_core_info.session_info.pd_param.pd_option), 
              sizeof(tm_core_info.session_info.pd_param.pd_option),
              (void *)get_pos_param.option_ptr, sizeof(*get_pos_param.option_ptr));
      memscpy((void *)&(tm_core_info.session_info.pd_param.pd_qos), sizeof(tm_core_info.session_info.pd_param.pd_qos),
              (void *)get_pos_param.qos_ptr, sizeof(*get_pos_param.qos_ptr));

      reqResult = tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp(
                     mo_sess_handle,
                    &(tm_core_info.session_info.pd_param),
                     &start_actions);

      if(reqResult == PRTL_START_SESS_REQ_ALLOWED)
      {
        /* If prototocol return PRTL_START_SESS_REQ_ALLOWED, call tmCore_SessReqStart */
        retVal = tm_core_sess_req_start(active_prtl,
                                        mo_sess_handle,
                                        start_actions);
      }
      else if(reqResult == PRTL_START_SESS_REQ_WAIT)
      {
        /* If prototocol return PRTL_START_SESS_REQ_WAIT, set tm-core session state to wait */
        /* Protocol sub-modules should call session continue to continue this session */
        tm_core_info.session_info.active_sess_handle = mo_sess_handle;
        tm_core_info.session_info.from_protocol      = active_prtl;
        tm_core_update_session_state(TM_CORE_SESS_STATE_WAIT_AUTH);

        retVal = TRUE; /*lint !e506 */
      }
      else
      {
        /* If protocol return PRTL_START_SESS_REQ_NOT_ALLOWED, report to PD API */
        MSG_ERROR("Protocol sub-module rejects start session request", 0, 0, 0);
        retVal = FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
      retVal = FALSE;
    }
  }
  else
  {

    MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
    retVal = FALSE;
  }

  if(retVal == FALSE)
  {
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506  !e730 */
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_get_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_pos(const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr)
{
  int                               prtl_tbl_idx = -1;
  tm_prtl_type                      active_prtl;
  tm_prtl_type                      orig_active_prtl;
  prtl_start_sess_req_result_e_type reqResult;
  tm_sess_req_start_param_s_type    start_actions;
  boolean                           retVal = FALSE;
  tm_sess_handle_type               mo_sess_handle;
  tm_pdapi_pd_event_s_type          pd_event_type;
  prtl_ans_u_type                   prtl_answer;
  tm_core_cached_pos_s_type         cached_pos_info;
  tm_cm_phone_state_info_s_type     phone_state_info;

  /* Init */
  memset(&pd_event_type, 0, sizeof(pd_event_type));

  tm_cm_iface_get_phone_state(&phone_state_info);
  if( (get_pos_param_ptr == NULL)
    ||(get_pos_param_ptr->option_ptr == NULL)
    ||(get_pos_param_ptr->qos_ptr == NULL))
  {
    MSG_ERROR("Null get_pos_param_ptr", 0, 0, 0);
    return FALSE;
  }

  if (phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    /* if we are in emergency state, then only a SUPL emergency or a CP session can overwrite
       existing sessions */
    if ((get_pos_param_ptr->mt_initiator == MT_UP) &&
         (tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_E911))
    {
      MSG_ERROR("IN E911, UP session cancelling ongoing Emergency session %d.", 
                tm_core_info.session_info.active_sess_handle,0,0);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911);
    }
    if ((phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) &&
        (get_pos_param_ptr->mt_initiator == MT_UP) &&
        (tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911))
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_E911_OVERRIDE);
    }
  }
  /* For on demand and periodic PPM, when entering this function
     1. On-demand session is active
        - Do abort ongoing session and handle incoming fix request
     2. Periodic PPM session is active
        - Do abort ongoing PPM session and handle incoming fix request
  */
  else if (tm_core_is_on_demand_session_on() == TRUE)
  {
    MSG_MED("Aborting on demand session", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_ON_DEMAND_OVERRIDE);
  }
  else if(tm_core_is_internal_session_on() == TRUE)
  {
    /*Abort the ongoing internal session*/
    MSG_MED("Aborting on-going internal session", 0, 0, 0);
    tm_core_abort_recover(TRUE, TM_STOP_REASON_ABORT_INTERNAL_SESSION);
  }
  else if(tm_core_info.session_info.session_state != TM_CORE_SESS_STATE_INIT)
  {
    if( (get_pos_param_ptr->option_ptr->session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
      &&(tm_core_info.session_info.pd_param.pd_option.session != PDSM_PD_SESS_TYPE_DATA_DLOAD))
    {
      MSG_HIGH("Queue Data Dload Request", 0, 0, 0);

      tm_core_info.session_info.queued_request = TRUE;

      tm_core_info.session_info.queued_get_pos_param.client_type  = get_pos_param_ptr->client_type;
      tm_core_info.session_info.queued_get_pos_param.client_id    = get_pos_param_ptr->client_id;
      tm_core_info.session_info.queued_get_pos_param.mt_initiator = get_pos_param_ptr->mt_initiator;

      tm_core_info.session_info.queued_get_pos_param.internal_session = get_pos_param_ptr->internal_session;

      memscpy((void *)&(tm_core_info.session_info.queued_get_pos_param.option),
             sizeof(tm_core_info.session_info.queued_get_pos_param.option),
             (void *)get_pos_param_ptr->option_ptr,
             sizeof(*get_pos_param_ptr->option_ptr));

      memscpy((void *)&(tm_core_info.session_info.queued_get_pos_param.qos),
             sizeof(tm_core_info.session_info.queued_get_pos_param.qos),
             (void *)get_pos_param_ptr->qos_ptr,
             sizeof(*get_pos_param_ptr->qos_ptr));

      pd_event_type.e_pd_event = PDSM_PD_EVENT_DLOAD_BEGIN;
      pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
      tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
      return TRUE;
    }
    else if( (get_pos_param_ptr->option_ptr->session != PDSM_PD_SESS_TYPE_DATA_DLOAD)
      &&(tm_core_info.session_info.pd_param.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD))
    {
      MSG_HIGH("Queue Fix Request", 0, 0, 0);

      tm_core_info.session_info.queued_request = TRUE;

      tm_core_info.session_info.queued_get_pos_param.client_type = get_pos_param_ptr->client_type;
      tm_core_info.session_info.queued_get_pos_param.client_id   = get_pos_param_ptr->client_id;

      tm_core_info.session_info.queued_get_pos_param.internal_session = get_pos_param_ptr->internal_session;

      memscpy((void *)&(tm_core_info.session_info.queued_get_pos_param.option),
             sizeof(tm_core_info.session_info.queued_get_pos_param.option),
             (void *)get_pos_param_ptr->option_ptr,
             sizeof(*get_pos_param_ptr->option_ptr));

      memscpy((void *)&(tm_core_info.session_info.queued_get_pos_param.qos),
             sizeof(tm_core_info.session_info.queued_get_pos_param.qos),
             (void *)get_pos_param_ptr->qos_ptr,
             sizeof(*get_pos_param_ptr->qos_ptr));

      pd_event_type.e_pd_event = PDSM_PD_EVENT_BEGIN;
      pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
      pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
      tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
      return TRUE;
    }
    else if ((phone_state_info.e911_state == TM_CM_E911_STATE_ORIG) ||
           (phone_state_info.e911_state == TM_CM_E911_STATE_CB) )
    {
      if (get_pos_param_ptr->mt_initiator == MT_UP &&
          tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911)
      {
        MSG_HIGH("ONGOING EMERGENCY SUPL SEssion", 0,0,0);
        /* if SUPL session is an MSA, then the ongoing emergency MSB session needs to be stopped */   
        if (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSASSISTED)
        {
          tm_core_abort_recover(TRUE, TM_STOP_REASON_ABORT_INTERNAL_SESSION);
        }
      }
    }
    else
    {
      MSG_HIGH("Overlapping session", 0, 0, 0);
      return FALSE;
    }
  }

   /* if the get pos request is last position, return a fix if availalbe.
    otherwise return error
    */
  if (get_pos_param_ptr->option_ptr->session == PDSM_PD_SESS_TYPE_LATEST)
  {
    pd_event_type.e_pd_event = PDSM_PD_EVENT_BEGIN;
    pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
    pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
    tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
    retVal = tm_core_get_last_position(&cached_pos_info);
    if (retVal == TRUE)
    {
      pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;

      tm_pdapi_pos_log_generation(cached_pos_info.pd_event_type,
                                  &cached_pos_info.pd_info,
                                  cached_pos_info.z_pos_flags.b_IsSft,
                                  cached_pos_info.z_pos_flags.b_IsKf,
                                  TM_PDAPI_SESS_TYPE_NONE);

      tm_pdapi_pos_event_callback(cached_pos_info.pd_event_type,
                                  &cached_pos_info.pd_info,
                                  TM_PDAPI_SESS_TYPE_NONE);
    }
    else
    {
      pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
      pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_PD_END_FIX_ERROR;
      tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

    }
    pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
    pd_event_type.e_pd_event = PDSM_PD_EVENT_DONE;
    tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);

    return retVal;
  }


  if (get_pos_param_ptr->mt_initiator == MT_UP)
  {
    if ((phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA) &&
        (phone_state_info.e911_state != TM_CM_E911_STATE_NONE))
    {
      tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_UMTS_UP);
    }
    else if ((phone_state_info.srv_system == CGPS_SRV_SYS_LTE) &&
        (phone_state_info.e911_state != TM_CM_E911_STATE_NONE))
    {
      tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_LTE_UP);
    }
  }
  /* If a get pos request is
     1. Fix session
     2. MSB or Standalone
     Let standalone handles it first */
  if( (get_pos_param_ptr->option_ptr->session != PDSM_PD_SESS_TYPE_DATA_DLOAD)
    &&( (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY)
      ||(get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED)))
  {
    /* Default allow smart MSB */
    prtl_answer.ans_smart_msb = PRTL_ANS_SMART_MSB_ALLOWED;

    /*
       For MSB fix request, we need to query protocol sub-modules to see if they want standalone module to
       handle the MSB fix request
    */
    if(get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSBASED)
    {
      active_prtl = tm_core_get_active_prtl_module(get_pos_param_ptr->mt_initiator);
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

      if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].general_query_fp)
        {
          (void)tm_core_info.prtl_func_cb_table[prtl_tbl_idx].general_query_fp(PRTL_QUERY_SMART_MSB,
                                                                               &prtl_answer);
        }
      }
    }
    else
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }
    
    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */
    if(prtl_answer.ans_smart_msb == PRTL_ANS_SMART_MSB_ALLOWED)      
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }
  }
  else
  {
    active_prtl = tm_core_get_active_prtl_module(get_pos_param_ptr->mt_initiator);
    orig_active_prtl = active_prtl; /* Save original prtl as it may get overwritten later. */

    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);

    /* If fix request is of any optimal mode and MS is OoS, have standalone handle the request since we know
       for sure that no data connection can be brought up */
    if( (prtl_tbl_idx < 0)
      &&( (get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED)
        ||(get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY)
        ||(get_pos_param_ptr->option_ptr->operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA)))
    {
      active_prtl = TM_PRTL_TYPE_STANDALONE;
      prtl_tbl_idx = tm_core_get_prtl_idx_by_type(active_prtl);
    }
  }

  MSG_MED("tm_core dispatches get_pos req to [%d]", active_prtl, 0, 0);

  if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    if (get_pos_param_ptr->client_id == 3333)
    {
      mo_sess_handle = TM_SESS_HANDLE_ON_DEMAND;
    }
    else
    {
      mo_sess_handle = tm_core_get_new_sess_handle(get_pos_param_ptr->mt_initiator, orig_active_prtl);
    }

    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp)
    {
      if(get_pos_param_ptr->internal_session == FALSE)
      {
        /* Fire Begin event */
        if(get_pos_param_ptr->option_ptr->session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
        {
          pd_event_type.e_pd_event = PDSM_PD_EVENT_DLOAD_BEGIN;
          pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
          tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
        }
        else
        {
          pd_event_type.e_pd_event = PDSM_PD_EVENT_BEGIN;
          pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
          pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
          tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);
        }
      }

#ifdef FEATURE_TM_SECURITY
      tm_sec_update_random_challenge(TM_SEC_RND_UPDATE_SOURCE_GET_POS);
#endif /* FEATURE_TM_SECURITY */


      tm_core_info.session_info.pd_param.client_id   = get_pos_param_ptr->client_id;
      tm_core_info.session_info.pd_param.client_type = get_pos_param_ptr->client_type;
      memscpy((void *)&(tm_core_info.session_info.pd_param.pd_option), 
              sizeof(tm_core_info.session_info.pd_param.pd_option),
              (void *)get_pos_param_ptr->option_ptr, sizeof(*get_pos_param_ptr->option_ptr));
      memscpy((void *)&(tm_core_info.session_info.pd_param.pd_qos),
              sizeof(tm_core_info.session_info.pd_param.pd_qos),
             (void *)get_pos_param_ptr->qos_ptr, sizeof(*get_pos_param_ptr->qos_ptr));

      reqResult = tm_core_info.prtl_func_cb_table[prtl_tbl_idx].start_sess_req_fp(
                     mo_sess_handle,
                    &(tm_core_info.session_info.pd_param),
                     &start_actions);

      if(reqResult == PRTL_START_SESS_REQ_ALLOWED)
      {
        /* If prototocol return PRTL_START_SESS_REQ_ALLOWED, call tmCore_SessReqStart */
        retVal = tm_core_sess_req_start(active_prtl,
                                        mo_sess_handle,
                                        start_actions);
        tm_core_info.session_info.pd_param.client_id   = get_pos_param_ptr->client_id;
        tm_core_info.session_info.pd_param.client_type = get_pos_param_ptr->client_type;
        tm_core_info.internal_session = get_pos_param_ptr->internal_session;

        /* Call SDP to send #fixes and tbf information */
        MSG_HIGH("SDP_TEMP_DEBUG: SDP send fix rate info. NumFixes %d, TBF %d", 
          tm_core_info.session_info.pd_param.pd_option.fix_rate.num_fixes, 
          tm_core_info.session_info.pd_param.pd_option.fix_rate.time_between_fixes_ms, 0);
        (void) sdp_send_fix_rate_info_proxy(tm_core_info.session_info.pd_param.pd_option.fix_rate.num_fixes,
          tm_core_info.session_info.pd_param.pd_option.fix_rate.time_between_fixes_ms);

        if(tm_core_info.internal_session)
        {
          MSG_HIGH("Internal Session",0,0,0);
        }
      }
      else if(reqResult == PRTL_START_SESS_REQ_WAIT)
      {
        /* If prototocol return PRTL_START_SESS_REQ_WAIT, set tm-core session state to wait */
        /* Protocol sub-modules should call session continue to continue this session */
        tm_core_info.session_info.active_sess_handle = mo_sess_handle;
        tm_core_info.session_info.from_protocol      = active_prtl;
        tm_core_info.internal_session = get_pos_param_ptr->internal_session;

        if(tm_core_info.internal_session)
        {
          MSG_HIGH("Internal Session",0,0,0);
        }

        tm_core_update_session_state(TM_CORE_SESS_STATE_WAIT_AUTH);

        retVal = TRUE; /*lint !e506 */
      }
      else
      {
        /* If protocol return PRTL_START_SESS_REQ_NOT_ALLOWED, report to PD API */
        MSG_HIGH("Protocol sub-module rejects start session request", 0, 0, 0);

        pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
        pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
        pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
        tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);

        pd_event_type.e_pd_event = PDSM_PD_EVENT_DONE;
        pd_event_type.pd_event_data.pd_info_type.client_id = get_pos_param_ptr->client_id;
        pd_event_type.pd_event_data.pd_get_pos_data = *get_pos_param_ptr;
        tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_MI);

        retVal = FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
      retVal = FALSE;
    }
  }
  else
  {

    MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
    retVal = FALSE;
  }

  if(retVal == FALSE)
  {
    tm_core_abort_recover(TRUE, TM_STOP_REASON_GENERAL_ERROR); /*lint !e506  !e730 */
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_use_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_POSITION_SHOPPING
#error code not present
#endif

/*===========================================================================

FUNCTION tm_core_end_session_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_end_session_para_check
(
  const tm_pdapi_end_session_param_s_type  *end_session_param_ptr
)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  (void)end_session_param_ptr;

  /* Perform necessary check to see if incoming end pos request should be granted */

  /* First check if the task is active */
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    return PDSM_PD_CMD_ERR_OFFLINE_S;
  }

  /* Perform E911 related checks (E911 call orgination, E911 CBW, ...etc */
  tm_cm_iface_get_phone_state(&phone_state_info);

   if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    MSG_HIGH("Phone in E911 state", 0, 0, 0);
    return PDSM_PD_CMD_ERR_STATE_S;
  } 
  /* Then check if any there is any active PD session */
  if(!tm_core_session_active())
  {
    MSG_HIGH("No Fix session is in progress. Send cancellation to the protocols.", 0, 0, 0);
    //return PDSM_PD_CMD_ERR_NO_SESS_S;
  }

  return PDSM_PD_CMD_ERR_NOERR;
}


/*===========================================================================

FUNCTION tm_core_handle_end_session

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_end_session
(
  const tm_pdapi_end_session_param_s_type  *end_session_param_ptr
)
{
  boolean                           retVal = FALSE;
  pdsm_client_type_e_type           client_type = PDSM_CLIENT_TYPE_NONE;
  tm_pdapi_pd_event_s_type          pd_event_type = {0};

  if(end_session_param_ptr == NULL)
  {
    MSG_ERROR("Null end_session_param_ptr", 0, 0, 0);
    return FALSE;
  }

  
  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
      "End Session Request Client_id = %d, Session type = %d Receiver off = %d",
      end_session_param_ptr->client_id,
      end_session_param_ptr->session_type,
      end_session_param_ptr->receiver_off);

  client_type = pdsmclient_get_client_type_map(end_session_param_ptr->client_id);

    
  if((end_session_param_ptr->client_id == tm_core_info.session_info.pd_param.client_id) ||
     (client_type == PDSM_CLIENT_TYPE_OEM) ||
     (end_session_param_ptr->receiver_off == TRUE))
  {
    /* Check if the receiver off is needed */
    if(end_session_param_ptr->receiver_off == TRUE)
    {
      tm_core_info.session_info.receiver_off_needed = TRUE;
    }
    tm_core_info.session_info.pd_param.client_id = end_session_param_ptr->client_id;
    if(end_session_param_ptr->session_type == PDSM_PD_END_SESSION_FIX_REQ)
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_USER_TERMINATED); /*lint !e506  !e730 */
    }
    else if(end_session_param_ptr->session_type == PDSM_PD_END_SESSION_DLOAD_REQ)
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_USER_TERMINATED); /*lint !e506  !e730 */
    }
    retVal = TRUE; /*lint !e506 */
  }
  else
  {
    MSG_HIGH("Client (%d) does not have an Active session, Send END Event",end_session_param_ptr->client_id,0, 0);
    pd_event_type.pd_event_data.pd_info_type.client_id = end_session_param_ptr->client_id ;
    pd_event_type.e_pd_event = PDSM_PD_EVENT_END;
    pd_event_type.pd_event_data.pd_info_type.pd_info.end_status = PDSM_PD_END_CLIENT_HAS_NO_SESS;
    tm_pdapi_pd_event_callback(&pd_event_type, TM_PDAPI_SESS_TYPE_NONE);
    retVal = FALSE;
  }
  return retVal;
}

/*===========================================================================

FUNCTION tm_core_handle_inject_ext_time

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_ext_time
(
  pdsm_pd_external_time_info_type *ext_time_ptr
)
{
  if(ext_time_ptr == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return FALSE;
  }

  tm_core_inject_ext_time(ext_time_ptr);
  return TRUE; /*lint !e506 */
}


/*===========================================================================

FUNCTION tm_core_handle_inject_ext_pos

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_inject_ext_pos
(
  pdsm_cmd_s_type *p_cmd_type
)
{
  if( p_cmd_type == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return FALSE;
  }
  tm_core_inject_ext_pos(p_cmd_type);

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_get_gps_state_para_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_get_gps_state_err_check
(
)
{
  tm_cm_phone_state_info_s_type phone_state_info;

  /* Perform necessary check to see if incoming end pos request should be granted */

  /* First check if the task is active */
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    return PDSM_PD_CMD_ERR_OFFLINE_S;
  }

  /* Perform E911 related checks (E911 call orgination, E911 CBW, ...etc */
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    MSG_HIGH("Phone in E911 state", 0, 0, 0);
    return PDSM_PD_CMD_ERR_STATE_S;
  }
  /* Then check if any there is any active PD session */
  if(tm_core_session_active())
  {
    MSG_HIGH("Fix session is in progress", 0, 0, 0);
#ifndef FEATURE_CGPS_SENSOR_TIME_GET // FEATURE_CGPS_SENSOR_TIME_GET is enabled for testing only for now
    return PDSM_PD_CMD_ERR_SESS_ACT_S;
#endif

  }


  if (tm_core_info.e_cached_rcvr_state != C_GPS_RCVR_STATE_OFF)
  {
    MSG_HIGH("Fix session is in progress. RCVR is not OFF", 0, 0, 0);
#ifndef FEATURE_CGPS_SENSOR_TIME_GET // FEATURE_CGPS_SENSOR_TIME_GET is enabled for testing only for now
    return PDSM_PD_CMD_ERR_SESS_ACT_S;
#endif
  }
  return PDSM_PD_CMD_ERR_NOERR;
}



/*===========================================================================

FUNCTION tm_core_get_best_avail_pos_err_check

DESCRIPTION 
This function does error checks to see if the command can be processed 

DEPENDENCIES
None 
 
RETURN VALUE
PD CMD ERROR if the command cannot be processed. NO_ERR otherwise. 
 
SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_get_best_avail_pos_err_check(void)
{
  /* First check if the task is active */
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    return PDSM_PD_CMD_ERR_OFFLINE_S;
  }
  return PDSM_PD_CMD_ERR_NOERR;
}


/*===========================================================================

FUNCTION tm_core_handle_get_gps_state

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_gps_state
(
  pdsm_cmd_s_type *p_cmd_type
)
{
  tm_core_get_gps_state_param_s_type get_gps_state_param;

  if( p_cmd_type == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return FALSE;
  }

  get_gps_state_param.source = SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE;
  get_gps_state_param.cmd.p_cmd_type = p_cmd_type;
  tm_core_get_gps_state(&get_gps_state_param);
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_handle_lcs_response

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_lcs_response
(
  pdsm_lcs_notification_resp_s_type *resp_info_ptr
)
{
  int                               prtl_tbl_idx = -1;
  prtl_event_u_type                 app_resp_info;

  if(resp_info_ptr == NULL)
  {
    MSG_ERROR("Null resp_info_ptr", 0, 0, 0);
    return FALSE;
  }
  else
  {
    /* First, figure out what protol sub-module to forward the resp from app */
    prtl_tbl_idx = tm_core_get_prtl_idx_by_lcs_cmd_type(resp_info_ptr->lcs_info_cmd_type);

    /* if this is auto-looped, attempt to terminate any ongoing session */
    if ( ( tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_4_SUPL_NI_NOTIF_LOOPBK ) ||
         ( tm_core_info.config_info.q_gnss_oem_feature_mask & NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK ) 
       )
    {
      MSG_MED("Auto-loop notif/verif arrives, terminate ongoing session if any", 0, 0, 0);
      tm_core_abort_recover(TRUE, TM_STOP_REASON_USER_TERMINATED); /*lint !e506 !e730 */
    }

    if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
    {
      if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp != NULL)
      {
        app_resp_info.lcs_event_payload.lcs_event                               = PRTL_LCS_EVENT_TYPE_RESP;

        memscpy((void *)&(app_resp_info.lcs_event_payload.lcs_event_payload.clnt_resp_payload),
                sizeof(app_resp_info.lcs_event_payload.lcs_event_payload.clnt_resp_payload),
                (void *)resp_info_ptr, sizeof(*resp_info_ptr));

        (void) tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp(
                                                         PRTL_EVENT_TYPE_LCS,
                                                         &app_resp_info);
        return TRUE;
      }
      else
      {
        MSG_ERROR("No protocol event callback installed", 0, 0, 0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid prtlTableIndex", 0, 0, 0);
      return FALSE;
    }
  }
}

/*===========================================================================

FUNCTION tm_core_handle_lcs_client_init

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_lcs_client_init
(
  pdsm_lcs_agent_client_init_s_type *clnt_init_info_ptr
)
{
  int                               prtl_tbl_idx = -1;
  prtl_event_u_type                 clnt_init_info;

  if(clnt_init_info_ptr == NULL)
  {
    MSG_ERROR("Null clnt_init_info_ptr", 0, 0, 0);
    return FALSE;
  }
  else
  {
    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

    if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
    {
      /* LCS client init is only used for V1/V2 now, so always forward to 1x UP */
      if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp != NULL)
      {
        clnt_init_info.lcs_event_payload.lcs_event = PRTL_LCS_EVENT_TYPE_CLINET_INIT;

        memscpy((void *)&(clnt_init_info.lcs_event_payload.lcs_event_payload.clnt_init_payload),
               sizeof(clnt_init_info.lcs_event_payload.lcs_event_payload.clnt_init_payload),
               (void *)clnt_init_info_ptr, sizeof(*clnt_init_info_ptr));

        return tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp(
                                                                 PRTL_EVENT_TYPE_LCS,
                                                                 &clnt_init_info);
      }
      else
      {
        MSG_ERROR("No protocol event callback installed", 0, 0, 0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid protocol table index", 0, 0, 0);
      return FALSE;
    }
  }
}

/*===========================================================================

FUNCTION tm_core_handle_lcs_client_release

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_lcs_client_release
(
  pdsm_lcs_agent_client_release_s_type *clnt_release_info_ptr
)
{
  int                               prtl_tbl_idx = -1;
  prtl_event_u_type                 clnt_relese_info;

  if(clnt_release_info_ptr == NULL)
  {
    MSG_ERROR("Null clnt_release_info_ptr", 0, 0, 0);
    return FALSE;
  }
  else
  {
    prtl_tbl_idx = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

    if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
    {
      /* LCS client release is only used for V1/V2 now, so always forward to 1x UP */
      if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp != NULL)
      {
        clnt_relese_info.lcs_event_payload.lcs_event = PRTL_LCS_EVENT_TYPE_CLINET_RELEASE;

        memscpy((void *)&(clnt_relese_info.lcs_event_payload.lcs_event_payload.clnt_release_payload),
               sizeof(clnt_relese_info.lcs_event_payload.lcs_event_payload.clnt_release_payload),
               (void *)clnt_release_info_ptr, sizeof(*clnt_release_info_ptr));

        (void) tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp(
                                                           PRTL_EVENT_TYPE_LCS,
                                                           &clnt_relese_info);
        return TRUE;
      }
      else
      {
        MSG_ERROR("No protocol event callback installed", 0, 0, 0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Invalid protocol table index", 0, 0, 0);
      return FALSE;
    }
  }
}


/*===========================================================================

FUNCTION tm_core_handle_lcs_query_vx_config

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_lcs_query_vx_config
(
  pdsm_client_type_e_type client_type
)
{
  int                               prtl_tbl_idx = -1;
  prtl_event_u_type                 vx_config_info;

  /* LCS client release is only used for V1/V2 now, so always forward to 1x UP */
  prtl_tbl_idx = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

  if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp != NULL)
    {
      vx_config_info.lcs_event_payload.lcs_event = PRTL_LCS_EVENT_TYPE_QUERY_CONFIG;

      vx_config_info.lcs_event_payload.lcs_event_payload.client_type = client_type;

      return tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp(
                                                         PRTL_EVENT_TYPE_LCS,
                                                         &vx_config_info);
    }
    else
    {
      MSG_ERROR("No protocol event callback installed", 0, 0, 0);
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR("Invalid protocol table index", 0, 0, 0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_core_handle_lcs_set_vx_config

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_lcs_set_vx_config
(
  pdsm_lcs_agent_set_vx_config_s_type *vx_config_info_ptr
)
{
  int                               prtl_tbl_idx = -1;
  prtl_event_u_type                 vx_config_info;

  // make sure VxVersion parameter is valid
  if (vx_config_info_ptr->vxVersion != PDSM_LCS_AGENT_DISABLED &&
      vx_config_info_ptr->vxVersion != PDSM_LCS_AGENT_VERSION_V1_ONLY &&
      vx_config_info_ptr->vxVersion != PDSM_LCS_AGENT_VERSION_V2_ONLY)
  {
    MSG_MED("Invalid LCS Agent Vx version: 0x%x!", (uint8)vx_config_info_ptr->vxVersion, 0, 0); /*lint !e571 */
    return FALSE;
  }

  MSG_MED("Writing Vx Config Setting (%x) to NV", (uint8)vx_config_info_ptr->vxVersion, 0, 0); /*lint !e571 */

  /* Write to NV */
  tm_pa_nv_write_buffer.gps1_vx_lcs_agent = (uint8)vx_config_info_ptr->vxVersion;
  (void)sm_nv_write(NV_GPS1_VX_LCS_AGENT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

  /* LCS Set VX Config is only used for V1/V2 now, so always forward to 1x UP */
  prtl_tbl_idx = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

  if((prtl_tbl_idx >= 0) && (prtl_tbl_idx < TM_PRTL_NUM))
  {
    if(tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp != NULL)
    {
      vx_config_info.lcs_event_payload.lcs_event = PRTL_LCS_EVENT_TYPE_SET_CONFIG;

      memscpy((void *)&(vx_config_info.lcs_event_payload.lcs_event_payload.vx_config_payload),
             sizeof(vx_config_info.lcs_event_payload.lcs_event_payload.vx_config_payload),
             (void *)vx_config_info_ptr, sizeof(*vx_config_info_ptr));

      return tm_core_info.prtl_func_cb_table[prtl_tbl_idx].event_cb_fp(
                                                         PRTL_EVENT_TYPE_LCS,
                                                         &vx_config_info);
    }
    else
    {
      MSG_ERROR("No protocol event callback installed", 0, 0, 0);
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR("Invalid protocol table index", 0, 0, 0);
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_core_set_param_always_allowed

DESCRIPTION:  Whether setting of the parameter is allowed regardless of session state
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static boolean tm_core_set_param_always_allowed(pdsm_pa_e_type e_pa_type)
{
  /* Allow following set command to go through regardless session state */
  if ((e_pa_type == PDSM_PA_XTRAT_CLIENT_TOKEN) || 
      (e_pa_type == PDSM_PA_XTRAT_USER_SESSION_CONTROL) ||
      (e_pa_type == PDSM_PA_DPO_CONTROL) ||
      (e_pa_type == PDSM_PA_LBS_APN_PROFILE) ||
      (e_pa_type == PDSM_PA_XTRA_APN_PROFILE) ||
      (e_pa_type == PDSM_PA_PREMIUM_SERVICES_CONFIG)||
      (e_pa_type == PDSM_PA_DATA_ENABLE)
      )
  {
    MSG_MED("Always allow Set parameter for %d", e_pa_type,0,0);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION tm_core_set_param_check

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_set_param_check(tm_pdapi_param_s_type *set_param_ptr)
{
  /* First check if the task is active */
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    return FALSE;
  }

  if (tm_core_set_param_always_allowed(set_param_ptr->e_pa_type))
  {
    return TRUE;
  }

  if( (tm_core_session_active()==TRUE) &&
      (tm_core_is_internal_session_on() == FALSE)
    ) 
  {
    MSG_ERROR("Get/Set Parameter %d is not allowed during an active session",
      set_param_ptr->e_pa_type, 0, 0);
    return FALSE;
  }
  else
  {
    return TRUE; /*lint !e506 */
  }
}

/*===========================================================================

FUNCTION tm_core_handle_get_param

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_get_param(tm_pdapi_param_s_type *get_param_ptr)
{
  pdsm_pa_nmea_type  temp_sentence_type;
  uint8 uSbasUserPref = 0;
  boolean bSbasEnabled = FALSE;
  dword portID = 0;
  uint32 uConfigInfo = 0;
  uint8  bConfigByte = 0;

  if ( get_param_ptr == NULL )
  {
    MSG_ERROR("Null set_param_ptr", 0, 0, 0);
    return FALSE;
  }
  else
  {
    switch ( get_param_ptr->e_pa_type )
    {
    case PDSM_PA_1X_PDE_NETWORK_ADRS:

      MSG_MED("Getting 1x PDE Network Address Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_CGPS_1X_PDE_SERVER_PORT_I, (void *)&tm_pa_nv_read_buffer);
      portID = tm_pa_nv_read_buffer.cgps_1x_pde_server_port;

      (void)sm_nv_read_ex(NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_ipv4 != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_ipv4;
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id = portID;
        return TRUE;
      }

      (void)sm_nv_read_ex(NV_CGPS_1X_PDE_SERVER_ADDR_IPV6_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_ipv6[0] != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_IPV6_ADDRESS;
        memscpy(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.server_adrs,
			   sizeof(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.server_adrs),
               (void *)tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_ipv6,
               sizeof(tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_ipv6));
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.port_id = portID;
        return TRUE;
      }
      
      (void)sm_nv_read_ex(NV_CGPS_1X_PDE_SERVER_ADDR_URL_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_url[0] != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
        memscpy(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url, 
			   sizeof(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url),
               tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_url,
               sizeof(tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_url));
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length = (uint8)strlen((char *)tm_pa_nv_read_buffer.cgps_1x_pde_server_addr_url);
        return TRUE;
      }

      /* If nothing is set, default to empty IPV4 */
      get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs = 0;
      get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id = portID;

      return TRUE; /*lint !e506 */

    case PDSM_PA_1X_MPC_NETWORK_ADRS:

      MSG_MED("Getting MPC Network Address Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_read_buffer.cgps_1x_mpc_server_addr_ipv4;

      (void)sm_nv_read_ex(NV_CGPS_1X_MPC_SERVER_PORT_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id = tm_pa_nv_read_buffer.cgps_1x_mpc_server_port;

      return TRUE; /*lint !e506 */

    case PDSM_PA_UMTS_PDE_NETWORK_ADRS:

      MSG_MED("Getting UMTS PDE Address Setting", 0, 0, 0);

      (void)sm_nv_read_ex(NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_url[0] != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
        memscpy(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url, 
			   sizeof(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url),
               tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_url,
               sizeof(tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_url));
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length = (uint8)strlen((char *)tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_url);
        return TRUE;
      }
      (void)sm_nv_read_ex(NV_CGPS_UMTS_PDE_SERVER_PORT_I, (void *)&tm_pa_nv_read_buffer);
      portID = tm_pa_nv_read_buffer.cgps_umts_pde_server_port;

      (void)sm_nv_read_ex(NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_ipv4 != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_ipv4;
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id = portID;
        return TRUE;
      }

      (void)sm_nv_read_ex(NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV6_I, (void *)&tm_pa_nv_read_buffer);
      if ( tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_ipv6[0] != 0 )
      {
        get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_IPV6_ADDRESS;
        memscpy(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.server_adrs,
			   sizeof(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.server_adrs),
               (void *)tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_ipv6,
               sizeof(tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_ipv6));
        get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v6.port_id = portID;
        return TRUE;
      }

      /* If nothing is set Default to empty URL */
      get_param_ptr->pa_info_type_ptr->server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
      memset(get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url, 
             0,
             sizeof(tm_pa_nv_read_buffer.cgps_umts_pde_server_addr_url));
      get_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length = 0;

      return TRUE; /*lint !e506 */

    case PDSM_PA_GPS_LOCK:
      MSG_MED("Getting GPS Lock Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_GPS1_LOCK_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->gps_lock = (pdsm_gps_lock_e_type)tm_pa_nv_read_buffer.gps1_lock;
      return TRUE; /*lint !e506 */

    case PDSM_PA_KEEP_WARM:
      MSG_MED("Getting GPS Lock Setting", 0, 0, 0);
      return TRUE; /*lint !e506 */

    case PDSM_PA_POSITION_MODE:
      MSG_MED("Getting Position Mode Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_AAGPS_POSITIONING_MODES_SUPPORTED_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->position_mode = (pdsm_position_mode_type)tm_pa_nv_read_buffer.aagps_positioning_modes_supported;
      return TRUE; /*lint !e506 */

    case PDSM_PA_MT_LR_SUPPORT:
      MSG_MED("Getting MT LR Support Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_AAGPS_MT_LRSUPPORT_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->mt_lr_support = (pdsm_pa_mt_lr_support_e_type)tm_pa_nv_read_buffer.aagps_mt_lrsupport;
      return TRUE; /*lint !e506 */

    case PDSM_PA_MO_METHOD:
      MSG_MED("Getting MO Method Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_CGPS_MO_METHOD_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->mo_method = (pdsm_pa_mo_method_e_type)tm_pa_nv_read_buffer.cgps_mo_method;
      return TRUE; /*lint !e506 */

    case PDSM_PA_NMEA_SENTENCE_TYPE:
      MSG_MED("Getting NMEA Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_GNSS_NMEA_SENTENCE_TYPE_I, (void *)&tm_pa_nv_read_buffer);
      temp_sentence_type = (pdsm_pa_nmea_type)tm_pa_nv_read_buffer.gnss_nmea_sentence_type;

      MSG_MED("Getting Extended NMEA Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->nmea_sentence_type = temp_sentence_type | ((pdsm_pa_nmea_type)tm_pa_nv_read_buffer.gnss_nmea_extended_sentence_type << 6);
      return TRUE; /*lint !e506 */

    case PDSM_PA_APP_INFO:
      MSG_MED("Getting App Info Setting", 0, 0, 0);
      return TRUE; /*lint !e506 */

    case PDSM_PA_NMEA_CONFIG_INFO:
      MSG_MED("Getting NMEA Port Type", 0, 0, 0);
      (void)sm_nv_read_ex(NV_CGPS_NMEA_CONFIG_INFO_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->nmea_config_info.pd_nmea_port_type = (pdsm_pa_nmea_port_e_type)tm_pa_nv_read_buffer.cgps_nmea_config_info.nmea_port_type;
      get_param_ptr->pa_info_type_ptr->nmea_config_info.pd_nmea_reporting_type = (pdsm_pa_nmea_reporting_e_type)tm_pa_nv_read_buffer.cgps_nmea_config_info.nmea_reporting_type;
      return TRUE;

    case PDSM_PA_DPO_CONTROL:
      {
        uint8 u_DpoControl = 0;
        MSG_MED("Getting DPO Control Setting", 0, 0, 0);
        mgp_GetDpoControlStatus( &u_DpoControl, THREAD_ID_SM_TM );
        get_param_ptr->pa_info_type_ptr->dpo_control = u_DpoControl;
        return TRUE; /*lint !e506 */
      }

    case PDSM_PA_SBAS_STATUS:
      MSG_MED("Getting SBAS Status", 0, 0, 0);

      /* Get SBAS USER PREFERENCE NV item */
      (void)sm_nv_read_ex(NV_CGPS_SBAS_USER_PREFERENCE_I, (void *)&tm_pa_nv_read_buffer);
      uSbasUserPref = tm_pa_nv_read_buffer.cgps_sbas_user_preference;

      /* Get SBAS_ENABLED NV item */
      (void)sm_nv_read_ex(NV_CGPS_SBAS_ENABLED_I, (void *)&tm_pa_nv_read_buffer);
      bSbasEnabled = ((tm_pa_nv_read_buffer.cgps_sbas_enabled & 0x01) == 0x01);

      /* determine and return SBAS STATUS */
      if ( bSbasEnabled == FALSE )  /* SBAS_ENABLED == 0 */
      {
        get_param_ptr->pa_info_type_ptr->sbas_status = PDSM_PA_SBAS_STATUS_NOT_AVAILABLE;
      }
      else if ( uSbasUserPref != 0 ) 
      {
        get_param_ptr->pa_info_type_ptr->sbas_status = PDSM_PA_SBAS_STATUS_ENABLED;
      }
      else /* SBAS_ENABLED == 1, SBAS_USER_PREF == 0 */
      {
        get_param_ptr->pa_info_type_ptr->sbas_status = PDSM_PA_SBAS_STATUS_DISABLED;
      }

      return TRUE; /*lint !e506 */


    case PDSM_PA_ON_DEMAND_LOW_POWER_MODE:
      MSG_MED("Getting ON Demand LPM Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_CGPS_ON_DEMAND_ENABLED_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->on_demand_lpm = (boolean)tm_pa_nv_read_buffer.cgps_on_demand_enabled;
      return TRUE; /*lint !e506 */


    case PDSM_PA_ON_DEMAND_READY_MODE:
      MSG_MED("Getting ON Demand Ready Setting", 0, 0, 0);
      get_param_ptr->pa_info_type_ptr->on_demand_ready = tm_core_get_on_demand_ready_state();
      return TRUE; /*lint !e506 */

    case PDSM_PA_CALL_FLOW_SELECT:
      MSG_MED("Getting MOLR-AD Call Flow Select Setting", 0,0,0);
      get_param_ptr->pa_info_type_ptr->call_flow_select_mask = tm_core_get_call_flow_select_mask();
      return TRUE; /*lint !e506 */

    case PDSM_PA_LBS_APN_PROFILE:
    case PDSM_PA_XTRA_APN_PROFILE:
      case PDSM_PA_EMERGENCY_LBS_APN_PROFILE:
      MSG_MED("Getting APN profile", 0,0,0);
#ifndef FEATURE_GNSS_SA
      pd_comms_get_apn_profiles(get_param_ptr->e_pa_type,&get_param_ptr->pa_info_type_ptr->apn_profiles[0]);
#endif /* FEATURE_GNSS_SA */
      return TRUE;

    case PDSM_PA_SUPL_VERSION:
      MSG_MED("Getting SUPL Version Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_GNSS_SUPL_VERSION_I, (void *)&tm_pa_nv_read_buffer);
      get_param_ptr->pa_info_type_ptr->supl_version = (uint32)tm_pa_nv_read_buffer.gnss_supl_version;
      return TRUE; /*lint !e506 */

    case PDSM_PA_SUPL_SECURITY:
      MSG_MED("Getting SUPL Security Setting", 0, 0, 0);
      (void)sm_nv_read_ex(NV_AAGPS_USE_TRANSPORT_SECURITY_I, (void *)&tm_pa_nv_read_buffer);
      // Bit 0 is for SUPL Security, if bit is set, security enabled.
      get_param_ptr->pa_info_type_ptr->supl_security = (boolean)(tm_pa_nv_read_buffer.aagps_use_transport_security & 0x01);
      return TRUE; /*lint !e506 */

    case PDSM_PA_PE_CONFIG_PARAM:
      MSG_MED("Getting PE config param not supported", 0, 0, 0);
      get_param_ptr->pa_info_type_ptr->pe_config_info.peConfigMask = 0;  // mark all as failure
      return TRUE; /*lint !e506 */

    case PDSM_PA_SUPL_IS801:
      MSG_MED("Getting SUPL IS801 setting",0,0,0);
      if ( sm_nv_efs_reg_item_read_ex(GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE, sizeof(uint8), &bConfigByte) )
      {
        get_param_ptr->pa_info_type_ptr->supl_is801 = (boolean)(bConfigByte);  
        return TRUE;
      }
      break;

    case PDSM_PA_SUPL_TLS_VERSION:
      MSG_MED("Getting SUPL TLS version setting",0,0,0);
      (void)sm_nv_read_ex(NV_AAGPS_USE_TRANSPORT_SECURITY_I, (void *)&tm_pa_nv_read_buffer);
      // Bit 1 is for SUPL2 TLS version, if bit is not set(default), TLS1.1; otherwise TLS1.0
      if ((tm_pa_nv_read_buffer.aagps_use_transport_security & 0x02) == 0)
      {
        get_param_ptr->pa_info_type_ptr->supl_tls_version = PDSM_PA_SUPL_TLS_VERSION_1_1;  
      }
      else
      {
        get_param_ptr->pa_info_type_ptr->supl_tls_version = PDSM_PA_SUPL_TLS_VERSION_1_0;  
      }
      return TRUE;

    case PDSM_PA_SUPL_HASH_ALGORITHM:
      MSG_MED("Getting SUPL hash algorithm setting",0,0,0);
      (void)sm_nv_read_ex(NV_AAGPS_USE_TRANSPORT_SECURITY_I, (void *)&tm_pa_nv_read_buffer);
      // Bit 2 is for SUPL2 hash algorithm, if bit is not set(default), SHA256; otherwise SHA1
      if ((tm_pa_nv_read_buffer.aagps_use_transport_security & 0x04) == 0)
      {
        get_param_ptr->pa_info_type_ptr->supl_hash_algorithm = PDSM_PA_SUPL_HASH_ALGO_SHA256;  
      }
      else
      {
        get_param_ptr->pa_info_type_ptr->supl_hash_algorithm = PDSM_PA_SUPL_HASH_ALGO_SHA1;  
      }
      return TRUE;

    case PDSM_PA_LPP_CONFIG_INFO:
      MSG_MED("Getting LPP Config ",0,0,0);
      if ( sm_nv_efs_reg_item_read_ex(GNSS_NV_EFS_SM_TM_LPP_ENABLE, sizeof(uint8), &uConfigInfo) )
      {
        get_param_ptr->pa_info_type_ptr->lpp_config_info = (pdsm_pa_lpp_config_info_type)uConfigInfo;  
        return TRUE;
      }
      break;

    case PDSM_PA_ASST_GLONASS_POS_PROTOCOL:
      MSG_MED("Getting A-Glonass protocol selection",0,0,0);
      if ( sm_nv_efs_reg_item_read_ex(GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT, sizeof(uint32), &uConfigInfo) )
      {
        get_param_ptr->pa_info_type_ptr->asst_glo_protocol = (uint32)uConfigInfo;  
        return TRUE;
      }
      break;

    case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
      MSG_MED("Getting Emergency protocol selection",0,0,0);
      if ( sm_nv_efs_reg_item_read_ex(GNSS_NV_EFS_SM_TM_EMERGENCY_PROTOCOL_SELECT, sizeof(uint32), &uConfigInfo) )
      {
        get_param_ptr->pa_info_type_ptr->gnss_emergency_protocol_select = (pdsm_pa_emergency_protocol_e_type)uConfigInfo;  
        return TRUE;
      }
      break;

    case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
      MSG_MED("Getting WiFi Scan Timeout value",0,0,0);
      if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC, sizeof(uint8), &uConfigInfo) )
      {
        get_param_ptr->pa_info_type_ptr->wifi_wait_timeout_sec = (uint8)uConfigInfo;  
        return TRUE;
      }
      break;

    case PDSM_PA_LPP_GRT_24_OTDOA_AD_SUPPORT:
      MSG_MED("Getting LPP >24 AD support value",0,0,0);
      if ( sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_SUPPORT, sizeof(uint8), &uConfigInfo) )
      {
        get_param_ptr->pa_info_type_ptr->u_gnss_lpp_grt_24_otdoa = (uint8)uConfigInfo;  
        return TRUE;
      }
      break;


    case PDSM_PA_XTRA_VERSION_CHECK:
    {
      get_param_ptr->pa_info_type_ptr->xtra_version_check = tm_xtra_get_version_check();
      if (PDSM_PA_XTRA_VERSION_CHECK_MAX == get_param_ptr->pa_info_type_ptr->xtra_version_check) 
      {
         MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,"GetParam: PDSM_PA_XTRA_VERSION_CHECK %u",(uint32)get_param_ptr->pa_info_type_ptr->xtra_version_check);
         return FALSE;
      }
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED,"GetParam: PDSM_PA_XTRA_VERSION_CHECK %u",(uint32)get_param_ptr->pa_info_type_ptr->xtra_version_check);
      return TRUE; 
    }
    break;

    default:
      MSG_ERROR("Unknown PA get type %d", get_param_ptr->e_pa_type, 0, 0);
      return FALSE;
    }

    MSG_MED("EFS NV Item %d get failure", get_param_ptr->e_pa_type, 0, 0);
    return FALSE;
  }
} /* tm_core_handle_get_param */

/*===========================================================================

FUNCTION tm_core_nv_write_cb

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_nv_write_cb
(
  nv_items_enum_type    item,
  void*                 data,
  nv_stat_enum_type     status
)
{
  pdsm_pa_info_s_type pa_info_data;

  if(status != NV_DONE_S)
  {
    MSG_ERROR("NV Write Failed", 0, 0, 0);
    return;
  }

  switch(item)
  {
    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
      MSG_MED("1x PDE Server Addr NV Write Status %d", status, 0, 0);
    break;

    case NV_CGPS_1X_PDE_SERVER_PORT_I:
      MSG_MED("1x PDE Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_1X_PDE_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_write_buffer.cgps_1x_pde_server_addr_ipv4;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.port_id     = tm_pa_nv_write_buffer.cgps_1x_pde_server_port;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
      MSG_MED("1x PDE Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_1X_PDE_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
      MSG_MED("1x MPC Server Addr NV Write Status %d", status, 0, 0);
    break;

    case NV_CGPS_1X_MPC_SERVER_PORT_I:
      MSG_MED("1x MPC Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_1X_MPC_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_write_buffer.cgps_1x_mpc_server_addr_ipv4;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.port_id     = tm_pa_nv_write_buffer.cgps_1x_mpc_server_port;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_URL_I:
      MSG_MED("1x MPC Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_1X_MPC_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I:
      MSG_MED("UMTS PDE Server Addr NV Write Status %d", status, 0, 0);
    break;

    case NV_CGPS_UMTS_PDE_SERVER_PORT_I:
      MSG_MED("UMTS PDE Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_UMTS_PDE_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.server_adrs = tm_pa_nv_write_buffer.cgps_umts_pde_server_addr_ipv4;
      pa_info_data.pa_info.server_address.server_adrs.server_adrs_v4.port_id     = tm_pa_nv_write_buffer.cgps_umts_pde_server_port;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I:
      MSG_MED("UMTS PDE Server Addr NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_UMTS_PDE_NETWORK_ADRS;
      pa_info_data.pa_info.server_address.server_addr_type = PDSM_SERVER_TYPE_URL_ADDRESS;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_GPS1_LOCK_I:
      MSG_MED("GPS LOCK NV Write Status %d", status, 0, 0);
      pa_info_data.client_id        = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event         = PDSM_PA_GPS_LOCK;
      pa_info_data.pa_info.gps_lock = TM_CORE_CONFIG_DD(gps_lock);
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_AAGPS_POSITIONING_MODES_SUPPORTED_I:
      MSG_MED("Position Mode NV Write Status %d", status, 0, 0);
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_POSITION_MODE;
      pa_info_data.pa_info.position_mode      = tm_pa_nv_write_buffer.aagps_positioning_modes_supported;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_AAGPS_MT_LRSUPPORT_I:
      MSG_MED("MT LR NV Write Status %d", status, 0, 0);
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_MT_LR_SUPPORT;
      pa_info_data.pa_info.mt_lr_support      = (pdsm_pa_mt_lr_support_e_type)tm_pa_nv_write_buffer.aagps_mt_lrsupport;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_CGPS_MO_METHOD_I:
      MSG_MED("MO Method NV Write Status %d", status, 0, 0);
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_MO_METHOD;
      pa_info_data.pa_info.mo_method          = (pdsm_pa_mo_method_e_type)tm_pa_nv_write_buffer.cgps_mo_method;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_GNSS_NMEA_SENTENCE_TYPE_I:
      MSG_MED( "NMEA Output NV Write Status %d.  CB Count %d",
        status, tm_core_info.config_info.nmea_cb_count, 0);

      tm_core_info.config_info.nmea_cb_count--;
      if (tm_core_info.config_info.nmea_cb_count == 0) {
        // Only if the lower nmea bits are set
        pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
        pa_info_data.pa_event                   = PDSM_PA_NMEA_SENTENCE_TYPE;
        pa_info_data.pa_info.nmea_sentence_type = TM_CORE_CONFIG_DD(nmea_output_format);
        tm_pdapi_pa_event_callback(&pa_info_data);
      }
      break;

    case NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I:
      MSG_MED( "NMEA Extended Output NV Write Status %d.  CB Count %d",
        status, tm_core_info.config_info.nmea_cb_count, 0);

      tm_core_info.config_info.nmea_cb_count--;
      if (tm_core_info.config_info.nmea_cb_count == 0) {
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_NMEA_SENTENCE_TYPE;
      pa_info_data.pa_info.nmea_sentence_type = TM_CORE_CONFIG_DD(nmea_output_format);
      tm_pdapi_pa_event_callback(&pa_info_data);
      }
      break;

    case NV_CGPS_SBAS_USER_PREFERENCE_I:
      MSG_MED("SBAS User Preference NV Write Status %d", status, 0, 0);
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_SBAS_USER_PREFERENCE;
      pa_info_data.pa_info.sbas_user_preference = tm_pa_nv_write_buffer.cgps_sbas_user_preference;
      tm_pdapi_pa_event_callback(&pa_info_data);
      break;

#ifdef FEATURE_TM_SECURITY

    case NV_GPSONE_PASSWORD_I:

      MSG_MED("SSD NV Write Status %d", status, 0, 0);

      /* Indicate to client only the size of the SSD that is written to NV and
      ** not the actual SSD data (ssd_data should be NULL)
      */
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_SSD_DATA;

      memset((void *)pa_info_data.pa_info.ssd_info.ssd_data,
             0,
             PDSM_MAX_SSD_LEN);

      pa_info_data.pa_info.ssd_info.ssd_length = sec_enable_data.base_ssd.ssd_length;

      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

    case NV_GPS1_SEC_UPDATE_RATE_I:

      MSG_MED("Sec update NV Write Status %d", status, 0, 0);

      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_SEC_UPDATE_RATE;

      pa_info_data.pa_info.sec_rate.rate = sec_enable_data.rand_update_rate.rate;

      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

#endif /* FEATURE_TM_SECURITY */

    case NV_CGPS_NMEA_CONFIG_INFO_I:
      MSG_MED("NMEA Port Type NV Write Status %d", status, 0, 0);
      pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
      pa_info_data.pa_event                   = PDSM_PA_NMEA_CONFIG_INFO;
      pa_info_data.pa_info.nmea_config_info.pd_nmea_port_type = tm_core_info.config_info.nmea_config_type.pd_nmea_port_type;
      pa_info_data.pa_info.nmea_config_info.pd_nmea_reporting_type = tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type;
      tm_pdapi_pa_event_callback(&pa_info_data);
    break;

  case NV_CGPS_ON_DEMAND_ENABLED_I:
    MSG_MED("On Demand Enabled status NV Write Status %d", status, 0, 0);
    pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
    pa_info_data.pa_event                   = PDSM_PA_ON_DEMAND_LOW_POWER_MODE;
    pa_info_data.pa_info.on_demand_lpm = tm_core_info.config_info.on_demand_lpm;
    tm_pdapi_pa_event_callback(&pa_info_data);
  break;

  case NV_AAGPS_2G_MO_LRSUPPORT_I:
  case NV_AAGPS_3G_MO_LRSUPPORT_I:
    MSG_MED("MOLR vs AD Call Select NV 0x%x Write Status %d", item, status, 0);
    pa_info_data.client_id                     = tm_core_info.pa_client_info.client_id;
    pa_info_data.pa_event                      = PDSM_PA_CALL_FLOW_SELECT;
    pa_info_data.pa_info.call_flow_select_mask = tm_core_info.config_info.call_flow_select_mask;
    tm_pdapi_pa_event_callback(&pa_info_data);
    break;

  case NV_GNSS_SUPL_VERSION_I:
    MSG_MED("Supl Version NV Write Status %d", status, 0, 0);
    pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
    pa_info_data.pa_event                   = PDSM_PA_SUPL_VERSION;
    pa_info_data.pa_info.supl_version = tm_pa_nv_write_buffer.gnss_supl_version;
    tm_pdapi_pa_event_callback(&pa_info_data);
    break;

  case NV_AAGPS_USE_TRANSPORT_SECURITY_I:
    /* This NV will be used by 3 PA commands: Supl Security/Hash Algorithm/TLS version
       PDSM_PA_SUPL_SECURITY event has been sent ealier (not after NV is written */
    MSG_MED("Supl Security NV Write Status %d", status, 0, 0);
    break;

  case NV_GPS1_VX_LCS_AGENT_I:
    MSG_MED("LCS Agent Vx Version NV Write Status %d", status, 0, 0);
    break;

  case NV_CGPS_UTC_GPS_TIME_OFFSET_I:
    MSG_MED("UTC Time Offset write status %d", status, 0, 0);
    break;

  default:
    MSG_ERROR("Unknown nv write callback", 0, 0, 0);
    break;
  }
}


/*===========================================================================

FUNCTION tm_core_nv_async_rd_cb

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_nv_async_rd_cb
(
  nv_items_enum_type    item,
  void*                 data,
  nv_stat_enum_type     status
)
{
  nv_item_type* tm_nv_read_buffer; 
  int prtlTableIndex;

  if(status != NV_DONE_S)
  {
    MSG_ERROR("NV Async READ Failed for Item: %d", (int)item, 0, 0);
    return;
  }

  tm_nv_read_buffer = ((nv_item_type*)data);
  MSG_MED("NV Async Read Status: %d Item: %d", status, (int)item, 0);

  switch(item)
  {
    case NV_CURR_NAM_I:
      ruim_event_payload.ruim_read_nv_payload.tm_1x_up_curr_nam = (tm_nv_read_buffer)->curr_nam;
      break;
     
    case NV_MIN1_I:
      ruim_event_payload.ruim_read_nv_payload.tm_1x_up_imsi_s1 = (tm_nv_read_buffer)->min1.min1[NV_CDMA_MIN_INDEX];
      break;

    case NV_MIN2_I:
      ruim_event_payload.ruim_read_nv_payload.tm_1x_up_imsi_s2 = (tm_nv_read_buffer)->min2.min2[NV_CDMA_MIN_INDEX];
      break;

    case NV_IMSI_MCC_I:
      ruim_event_payload.ruim_read_nv_payload.mcc = (tm_nv_read_buffer)->imsi_mcc.imsi_mcc;
      break;

    case NV_IMSI_11_12_I:
      ruim_event_payload.ruim_read_nv_payload.imsi_11_12 = (tm_nv_read_buffer)->imsi_11_12.imsi_11_12;

      /*Send the PRTL_EVENT_TYPE_RUIM_NV_READ for 1X module*/
      ruim_event_payload.ruim_read_nv_payload.ruim_item_group = PRTL_EVENT_RUIM_MSID_NV_READ;
      prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);
      if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
        {
          (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_RUIM_NV_READ,
                                                                          &ruim_event_payload);
        }
      }
      break;

#ifdef FEATURE_UIM_SUPPORT_LBS

    case NV_GPS1_DYNAMIC_MODE_I:
      tm_core_ruim_nv_buffer.dynamic_mode = (boolean)(tm_nv_read_buffer)->gps1_dynamic_mode;
      break;

    case NV_GPS1_SEEDPOS_OPTION_I:
      tm_core_ruim_nv_buffer.seed_pos_options = (uint16)(tm_nv_read_buffer)->gps1_seedpos_option;
      tm_core_ruim_read_complete(&tm_core_ruim_nv_buffer);     
      break;

      
#if defined (FEATURE_CGPS_USES_CDMA) && !defined (FEATURE_GNSS_LOW_MEMORY)

    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
      ruim_event_payload.ruim_read_nv_payload.pde_adrs = (tm_nv_read_buffer)->cgps_1x_pde_server_addr_ipv4;
      break;

    case NV_CGPS_1X_PDE_SERVER_PORT_I:
      ruim_event_payload.ruim_read_nv_payload.pde_port = (tm_nv_read_buffer)->cgps_1x_pde_server_port;      
      break;

    case NV_GPS1_CAPABILITIES_I:
      ruim_event_payload.ruim_read_nv_payload.coarse_location_enabled = (tm_nv_read_buffer)->gps1_capabilities;
      break;

  case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
       memscpy((void *) &ruim_event_payload.ruim_read_nv_payload.pde_addr_url[0],
       sizeof(ruim_event_payload.ruim_read_nv_payload.pde_addr_url),
           (const void *)&(tm_nv_read_buffer->cgps_1x_pde_server_addr_url[0]),
               sizeof(tm_nv_read_buffer->cgps_1x_pde_server_addr_url));
             
      /*Send the PRTL_EVENT_TYPE_RUIM_NV_READ for 1X_UP module*/
      ruim_event_payload.ruim_read_nv_payload.ruim_item_group = PRTL_EVENT_RUIM_1X_NV_READ;
      prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);
      if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
        {
          (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_RUIM_NV_READ,
                                                                          &ruim_event_payload);
        }
      }
      break;
#endif /* FEATURE_CGPS_USES_CDMA && !FEATURE_GNSS_LOW_MEMORY*/

#if defined(FEATURE_GPSONE_VX_LCS_AGENT) && !defined(FEATURE_GNSS_LOW_MEMORY)
    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
      ruim_event_payload.ruim_read_nv_payload.mpc_adrs = (tm_nv_read_buffer)->cgps_1x_mpc_server_addr_ipv4;
      break;

    case NV_CGPS_1X_MPC_SERVER_PORT_I:
      ruim_event_payload.ruim_read_nv_payload.mpc_port = (tm_nv_read_buffer)->cgps_1x_mpc_server_port;

      /*Send the PRTL_EVENT_TYPE_RUIM_NV_READ for 1X_UP module*/
      ruim_event_payload.ruim_read_nv_payload.ruim_item_group = PRTL_EVENT_RUIM_VX_NV_READ;
      prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);
      if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
      {
        if(tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
        {
          (void) tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_RUIM_NV_READ,
                                                                          &ruim_event_payload);
        }
      }
      break;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT  & !FEATURE_GNSS_LOW_MEMORY */
          
#if defined(FEATURE_UIM_SUPPORT_LBS) && defined(FEATURE_CGPS_XTRA)
    case NV_GPS1_XTRA_ENABLED_I:
      ruim_xtra_nv_buffer.xtra_enabled =(boolean)((tm_nv_read_buffer)->gps1_xtra_enabled);
      break;             
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:
      ruim_xtra_nv_buffer.download_interval = (uint16)((tm_nv_read_buffer)->gps1_xtra_download_interval);
      break;

    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:
      ruim_xtra_nv_buffer.num_download_attempts = (uint8)((tm_nv_read_buffer)->gps1_xtra_num_download_attempts);
      break;

    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:
      ruim_xtra_nv_buffer.time_between_attempts = (uint8)((tm_nv_read_buffer)->gps1_xtra_time_between_attempts);
      break;

    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:
      ruim_xtra_nv_buffer.auto_download_enabled = (boolean)((tm_nv_read_buffer)->gps1_xtra_auto_download_enabled);
      break;

    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:
      memscpy((void *) &(ruim_xtra_nv_buffer.xtra_server_primary[0]),
       sizeof(ruim_xtra_nv_buffer.xtra_server_primary),
           (const void *)&(tm_nv_read_buffer->gps1_xtra_primary_server_url[0]),
           sizeof((tm_nv_read_buffer)->gps1_xtra_primary_server_url));
      break;

    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:
      memscpy((void *)&(ruim_xtra_nv_buffer.xtra_server_secondary[0]),
       sizeof(ruim_xtra_nv_buffer.xtra_server_secondary),
           (const void *)&(tm_nv_read_buffer->gps1_xtra_secondary_server_url[0]),
           sizeof(tm_nv_read_buffer->gps1_xtra_secondary_server_url));
      break; 

    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:
      memscpy((void *)&(ruim_xtra_nv_buffer.xtra_server_tertiary[0]),
       sizeof(ruim_xtra_nv_buffer.xtra_server_tertiary),
           (const void *)&(tm_nv_read_buffer->gps1_xtra_tertiary_server_url[0]),
           sizeof(tm_nv_read_buffer->gps1_xtra_tertiary_server_url));

      /*ReInit Xtra once we have read all NV's from RUIM*/
      tm_xtra_ruim_read_complete(&ruim_xtra_nv_buffer);

      break;
#endif /* FEATURE_UIM_SUPPORT_LBS && FEATURE_CGPS_XTRA */
#endif /* FEATURE_UIM_SUPPORT_LBS*/
  default:
    MSG_ERROR("Unknown NV Async Read Status callback", 0, 0, 0);
    break;
  }
}

/*===========================================================================

FUNCTION tm_core_handle_access_efs_data

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_access_efs_data
(
  tm_pdapi_param_s_type *p_set_param_ptr,
  prtl_event_u_type     *p_event_payload
)
{
  os_IpcMsgType *p_Msg;

  /* copy the data to create the IPC */
  z_GpsFsCmd.u_FileOP = p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_file_operation;
  z_GpsFsCmd.u_FileNameLen =  p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_filename_len;
  /* copy the file name to the buffer */
  if (GNSS_STRLCPY((char *) z_GpsFsCmd.p_FileName,
                  p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_filename,
                  sizeof(z_GpsFsCmd.p_FileName) ) > sizeof(z_GpsFsCmd.p_FileName))
  {
    MSG_ERROR("EFS File path was truncated.  Destination Length %u",
                   sizeof(z_GpsFsCmd.p_FileName), 0, 0);
    return FALSE;
  }

  /* send the IPC to write/delete the file */
  z_GpsFsCmd.q_DataSize = p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_len;
  p_Msg = os_IpcCreate( sizeof(gps_fs_u_cmd_type), IPC_ALLOCATION_DYNAMIC, THREAD_ID_GPS_FS );
  if (p_Msg)
  {
    p_Msg->q_MsgId = C_GPS_FS_MSG_DIAG_CMD;
    p_Msg->q_DestThreadId = THREAD_ID_GPS_FS;
    p_Msg->q_Size = sizeof(gps_fs_u_cmd_type);
    z_GpsFsCmd.p_DataPtr = NULL;
    z_GpsFsCmd.u_Reserved[0] = 0;
    z_GpsFsCmd.u_Reserved[1] = 0;
    z_GpsFsCmd.u_Reserved[2] = 0;
    z_GpsFsCmd.u_Reserved[3] = 1; /* do a force sync of the file so the file is written
                                    immediately to EFS not at shutdown */
    if (p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_file_operation != PDSM_FS_DELETE_FILE &&
        p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_file_operation != PDSM_FS_PURGE_DIR
       )
    {
      z_GpsFsCmd.p_DataPtr = (unsigned char*)pdsm_getbuf(p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_len);
      if (z_GpsFsCmd.p_DataPtr)
      {
        memscpy(z_GpsFsCmd.p_DataPtr, z_GpsFsCmd.q_DataSize, p_set_param_ptr->pa_info_type_ptr->efs_data.efs_data_ptr,
               z_GpsFsCmd.q_DataSize);
      }
    }
    z_GpsFsCmd.gps_fs_efs_status_get = tm_core_efs_file_resp_handler;
    memscpy( (gps_fs_u_cmd_type *)p_Msg->p_Data, sizeof(z_GpsFsCmd), &z_GpsFsCmd, sizeof(z_GpsFsCmd) );
    if (!os_IpcSend( p_Msg, THREAD_ID_GPS_FS ))
      (void) os_IpcDelete(p_Msg);
  }
  else
  {
    MSG_ERROR("could not allocate IPC", 0, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_core_inform_protocol_param_change

DESCRIPTION

DEPENDENCIES

RETURN VALUE  TRUE if protocol submodule has been notified about param change 

SIDE EFFECTS

===========================================================================*/
boolean tm_core_inform_protocol_param_change(
  tm_prtl_type prtl_type, 
  pdsm_pa_e_type  e_pa_type,
  tm_pdapi_param_s_type *set_param_ptr
)
{
  boolean           retVal = FALSE;
  int               prtlTableIndex;
  prtl_event_u_type event_payload;

  /* Inform protocol sub-modules for PA changes */
  prtlTableIndex = tm_core_get_prtl_idx_by_type(prtl_type);

  if((prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM))
  {
    if (tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL)
    {
      event_payload.pa_event_payload.pa_event = e_pa_type;
      event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
      retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                  &event_payload);
      retVal = TRUE;
    }
  }

  return retVal;
}


/*===========================================================================

FUNCTION tm_core_handle_set_param

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_set_param(tm_pdapi_param_s_type *set_param_ptr)
{
  boolean             retVal         = FALSE;
  int                 prtlTableIndex;
  prtl_event_u_type   event_payload;
  uint16              i;
  pdsm_pa_info_s_type pa_info_data;
  boolean             force_rcvr_off = FALSE;

  if ( (set_param_ptr == NULL) || (set_param_ptr->pa_info_type_ptr == NULL) )
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return ( FALSE );
  }

  /* PDSM_PA_DELETE_PARAMS is deprecated. use new PDAPI command PDSM_PA_DELETE_PARAMS_EX to
     handle deletion of eph / alm for individual SVs based on SV mask.
     Handling of PDSM_PA_DELETE_PARAMS has a bug in tm_core.c:tm_core_delete_inject_aiding_data()
     that deletes eph / alm for all SV, irrespective of eph / alm mask. The "feature" is retained for
     backwards compatibility and instead PDSM_PA_DELETE_PARAMS_EX should be used (when defined).
     */
  if ( (set_param_ptr->e_pa_type == PDSM_PA_DELETE_PARAMS) ||
       (PDSM_PA_DELETE_PARAMS_EX == set_param_ptr->e_pa_type) ||
       (PDSM_PA_DELETE_PARAMS_EX1 == set_param_ptr->e_pa_type) )
  {
    /* PDSM_PA_DELETE_PARAMS_EX and PDSM_PA_DELETE_PARAMS are a special case, no NV write is required */

    /* cancel any ongoing on-demand session */
    if ( tm_core_get_on_demand_state() == TRUE )
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_ON_DEMAND_DELETE_PARAMS);
      force_rcvr_off = TRUE;
    }
    else if ( tm_core_is_internal_session_on() == TRUE )
    {
      tm_core_abort_recover(TRUE, TM_STOP_REASON_INTERNAL_SESSION_DELETE_PARAM);
      force_rcvr_off = TRUE;
    }

    event_payload.pa_event_payload.pa_event = set_param_ptr->e_pa_type;
    event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;

    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_CP);
    if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
    {
      if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
      {
        (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_PA,
                                                                          &event_payload);
      }
    }

    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);
    if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
    {
      if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
      {
        (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_PA,
                                                                          &event_payload);
      }
    }

    prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);
    if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
    {
      if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
      {
        (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                          PRTL_EVENT_TYPE_PA,
                                                                          &event_payload);
      }
    }

    /* send the pdapi event call back since we dont need to write to NV */
    pa_info_data.client_id                  = set_param_ptr->client_id;
    pa_info_data.pa_event                   = set_param_ptr->e_pa_type;
    pa_info_data.pa_info.delete_params      = set_param_ptr->pa_info_type_ptr->delete_params;
    tm_pdapi_pa_event_callback(&pa_info_data);

    /* Following check emulates the behavior for PDSM_PA_DELETE_PARAMS,
    using the newer implemenation for PDSM_PA_DELETE_PARAMS_EX */
    if ( PDSM_PA_DELETE_PARAMS == set_param_ptr->e_pa_type )
    {
      if ( set_param_ptr->pa_info_type_ptr->delete_params.pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH )
       {
          MSG_HIGH("Deleting ALL EPH", 0, 0, 0);
          set_param_ptr->pa_info_type_ptr->delete_params.eph_sv_mask = (uint32)C_DELETE_INFO_EPH_INFO;
       }

      if ( set_param_ptr->pa_info_type_ptr->delete_params.pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM )
       {
          MSG_HIGH("Deleting ALL ALM", 0, 0, 0);
          set_param_ptr->pa_info_type_ptr->delete_params.alm_sv_mask = (uint32)C_DELETE_INFO_ALM_INFO;
       }
    }
    tm_core_delete_inject_aiding_data(&(set_param_ptr->pa_info_type_ptr->delete_params), force_rcvr_off, FALSE, set_param_ptr->e_pa_type);

    return ( TRUE ); /*lint !e506 */
  }
  else if ( set_param_ptr->e_pa_type == PDSM_PA_DPO_CONTROL )
  {
    MSG_MED("Changing DPO Control Setting %ld", set_param_ptr->pa_info_type_ptr->dpo_control, 0, 0);
    /* Update DPO Control Status */
    mgp_UpdateDpoControl(set_param_ptr->pa_info_type_ptr->dpo_control);

    pa_info_data.client_id           = set_param_ptr->client_id;
    pa_info_data.pa_event            = set_param_ptr->e_pa_type;
    pa_info_data.pa_info.dpo_control = set_param_ptr->pa_info_type_ptr->dpo_control;

    tm_pdapi_pa_event_callback(&pa_info_data);
    return ( TRUE ); /*lint !e506 */
  }
  else
  {
    if ( tm_nv_initialized == FALSE )
    {
      tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM, tm_core_nv_write_cb,
                                         tm_core_nv_async_rd_cb);

      if ( tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR )
      {
        tm_nv_initialized = TRUE; /*lint !e506 */
      }
    }

    if ( tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR )
    {
      tm_core_info.pa_client_info.client_id = set_param_ptr->client_id;

      switch ( set_param_ptr->e_pa_type )
      {
         case PDSM_PA_1X_PDE_NETWORK_ADRS:

           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_1X_PDE_NETWORK_ADRS;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }

           MSG_MED("Writing 1x PDE Network Address Setting", 0, 0, 0);

           if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS )
           {
             /* Write to NV */
             tm_pa_nv_write_buffer.cgps_1x_pde_server_addr_ipv4 = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
             (void)sm_nv_write(NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

             tm_pa_nv_write_buffer.cgps_1x_pde_server_port = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id;
             (void)sm_nv_write(NV_CGPS_1X_PDE_SERVER_PORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }
           else if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS )
           {
             /* Write to NV */
             for ( i = 0; (i < 128 && i < set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length); i++ )
             {
               tm_pa_nv_write_buffer.cgps_1x_pde_server_addr_url[i] = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url[i];
             }
             (void)sm_nv_write(NV_CGPS_1X_PDE_SERVER_ADDR_URL_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_1X_MPC_NETWORK_ADRS:

           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_1X_MPC_NETWORK_ADRS;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }

           MSG_MED("Writing 1x MPC Network Address Setting", 0, 0, 0);

           if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS )
           {
             /* Write to NV */
             tm_pa_nv_write_buffer.cgps_1x_mpc_server_addr_ipv4 = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
             (void)sm_nv_write(NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

             tm_pa_nv_write_buffer.cgps_1x_mpc_server_port = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id;
             (void)sm_nv_write(NV_CGPS_1X_MPC_SERVER_PORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }
           else if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS )
           {
             /* Write to NV */
             for ( i = 0; (i < 128 && i < set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length); i++ )
             {
               tm_pa_nv_write_buffer.cgps_1x_mpc_server_addr_url[i] = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url[i];
             }
             (void)sm_nv_write(NV_CGPS_1X_MPC_SERVER_ADDR_URL_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_UMTS_PDE_NETWORK_ADRS:

           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_UMTS_PDE_NETWORK_ADRS;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }

           MSG_MED("Writing UMTS PDE Network Address Setting", 0, 0, 0);

           if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS )
           {
             /* Write to NV */
             tm_pa_nv_write_buffer.cgps_umts_pde_server_addr_ipv4 = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
             (void)sm_nv_write(NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

             tm_pa_nv_write_buffer.cgps_umts_pde_server_port = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_v4.port_id;
             (void)sm_nv_write(NV_CGPS_UMTS_PDE_SERVER_PORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }
           else if ( set_param_ptr->pa_info_type_ptr->server_address.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS )
           {
             /* Write to NV */

             /* clear addr_url first in case the new one is shorter than the
                exiting one, and the new one is not null terminated */
             memset((void *)&tm_pa_nv_write_buffer.cgps_umts_pde_server_addr_url[0], 0, 128);

             for ( i = 0; (i < 128 && i < set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url_length); i++ )
             {
               tm_pa_nv_write_buffer.cgps_umts_pde_server_addr_url[i] = set_param_ptr->pa_info_type_ptr->server_address.server_adrs.server_adrs_url.url[i];
             }
             (void)sm_nv_write(NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           }

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_GPS_LOCK:
         {
           boolean lock_changed = FALSE;

           MSG_MED("Writing GPS Lock Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->gps_lock, 0, 0); /*lint !e571 */

           /* Update RAM copy first */
           if ( TM_CORE_CONFIG_DD(gps_lock) != set_param_ptr->pa_info_type_ptr->gps_lock )
           {
             lock_changed =  TRUE;
           }
           TM_CORE_CONFIG_DD(gps_lock) = set_param_ptr->pa_info_type_ptr->gps_lock;

           /* Write to NV */
           tm_pa_nv_write_buffer.gps1_lock = TM_CORE_CONFIG_DD(gps_lock);
           (void)sm_nv_write(NV_GPS1_LOCK_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_GPS_LOCK;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                    &event_payload);
             }
           }

           if ( lock_changed == TRUE )
           {
             /* if GPS is locked and on-demand positioning/ready mode is enabled, turn it off*/
             if ( TM_CORE_CONFIG_DD(gps_lock) != PDSM_GPS_LOCK_NONE )
             {
               if ( tm_core_get_on_demand_state() == TRUE ||
                    tm_core_get_on_demand_ready_state() == TRUE )
               {
                 tm_core_abort_recover(TRUE, TM_STOP_REASON_ON_DEMAND_GPS_LOCKED);
               }
               /* Clear the cached position if GPS is locked */
               tm_core_clear_cached_pos();

             }

             /* if GPS has been unlocked, reenable on-demand or ready mode if it is set in NV*/
             if ( TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_NONE )
             {
               if ( tm_core_get_on_demand_ready_state() == TRUE )
               {
                 tm_on_demand_ready_enable_handler(TRUE, TM_ON_DEMAND_EVENT_ENABLED);
               }
               /* on-demand feature is enabled */
               else if ( tm_core_get_on_demand_state() == TRUE )
               {
                 /* on-demand feature is enabled in NV */
                 tm_on_demand_charger_poll_init();
               }
             }

             /*Update GPS lock state to Geofencing Core*/
             gm_update_gps_lock_state(TM_CORE_CONFIG_DD(gps_lock));
           }
           retVal = TRUE;
         }
           retVal = TRUE;
           break;

         case PDSM_GNSS_CONSTELL_REPORT_CONFIG:

           MSG_MED("tm_core_handle_set_param: Received GNSS_CONFIG", 0, 0, 0);
           MSG_MED("tm_core_handle_set_param: MeasRep:%d,SVPoly:%d", set_param_ptr->pa_info_type_ptr->z_set_gnss_config.e_meas_report_config,
                   set_param_ptr->pa_info_type_ptr->z_set_gnss_config.e_sv_poly_report_config, 0);
           /* Update RAM copy */
           if ( set_param_ptr->pa_info_type_ptr->z_set_gnss_config.meas_report_config_valid )
           {
             tm_core_info.config_info.gnss_meas_rep_constell_config =
                                                                      set_param_ptr->pa_info_type_ptr->z_set_gnss_config.e_meas_report_config;
           }
           if ( set_param_ptr->pa_info_type_ptr->z_set_gnss_config.sv_poly_report_config_valid )
           {

             tm_core_info.config_info.sv_poly_rep_constell_config =
                                                                    set_param_ptr->pa_info_type_ptr->z_set_gnss_config.e_sv_poly_report_config;
             /*SV Poly has an OEM DR control within Modem. Enable it up so, SM can receive SV poly reports */
             if ( tm_core_info.config_info.sv_poly_rep_constell_config )
             {
               //mgp_OemDreSvPolyDataRequest(TRUE);
               oemdreOn();
             }
             else
             {
               //mgp_OemDreSvPolyDataRequest(FALSE);
               oemdreOff();
             }
           }

           retVal = TRUE;


           pa_info_data.client_id                = set_param_ptr->client_id;
           pa_info_data.pa_event                = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.z_set_gnss_config = set_param_ptr->pa_info_type_ptr->z_set_gnss_config;
           tm_pdapi_pa_event_callback(&pa_info_data);
           break;


         case PDSM_PA_KEEP_WARM:
           MSG_MED("Writing Keep Warm Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->keep_warm_enable, 0, 0);

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_POSITION_MODE:
           MSG_MED("Writing Position Mode Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->position_mode, 0, 0);
           tm_pa_nv_write_buffer.aagps_positioning_modes_supported = set_param_ptr->pa_info_type_ptr->position_mode;
           (void)sm_nv_write(NV_AAGPS_POSITIONING_MODES_SUPPORTED_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* Inform protocol sub-modules for PA changes */
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_UMTS_UP, PDSM_PA_POSITION_MODE, set_param_ptr);
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_UMTS_CP, PDSM_PA_POSITION_MODE, set_param_ptr);
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_LPP_CP, PDSM_PA_POSITION_MODE, set_param_ptr);

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_MT_LR_SUPPORT:
           MSG_MED("Writing MT LR Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->mt_lr_support, 0, 0);
           tm_pa_nv_write_buffer.aagps_mt_lrsupport = set_param_ptr->pa_info_type_ptr->mt_lr_support;
           (void)sm_nv_write(NV_AAGPS_MT_LRSUPPORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_MO_METHOD:
           MSG_MED("Writing MO Method Setting (%d) to NV", set_param_ptr->pa_info_type_ptr->mo_method, 0, 0);
           tm_core_info.config_info.u_mo_method = set_param_ptr->pa_info_type_ptr->mo_method;
           tm_pa_nv_write_buffer.cgps_mo_method = set_param_ptr->pa_info_type_ptr->mo_method;
           (void)sm_nv_write(NV_CGPS_MO_METHOD_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_NMEA_SENTENCE_TYPE:
           MSG_MED("Writing NMEA Format Setting (%x) to NV", set_param_ptr->pa_info_type_ptr->nmea_sentence_type, 0, 0);

           /* Init callback counter */
           tm_core_info.config_info.nmea_cb_count = 2;

           /* Update RAM copy first */
           TM_CORE_CONFIG_DD(nmea_output_format) = set_param_ptr->pa_info_type_ptr->nmea_sentence_type;

           /* Write to NV */
           tm_pa_nv_write_buffer.gnss_nmea_sentence_type = (uint8)(TM_CORE_CONFIG_DD(nmea_output_format) & 0x003F);
           (void)sm_nv_write(NV_GNSS_NMEA_SENTENCE_TYPE_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           tm_pa_nv_write_buffer.gnss_nmea_extended_sentence_type = (uint16)((TM_CORE_CONFIG_DD(nmea_output_format) & 0x3FFC0) >> 6);
           (void)sm_nv_write(NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_APP_INFO:
           MSG_MED("Save App Info", 0, 0, 0);
           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_1X_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_APP_INFO;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }

           MSG_MED("Sending App Info to Sub-modules", 0, 0, 0);

           retVal = TRUE; /*lint !e506 */
           break;

#ifdef FEATURE_TM_SECURITY

         case PDSM_PA_SSD_DATA:
           MSG_MED("Writing SSD to NV", 0, 0, 0);

           tm_pa_nv_write_buffer.gpsone_password.password_len = set_param_ptr->pa_info_type_ptr->ssd_info.ssd_length;

           /* Array length check is done in pa_cmd_set_param_para_check() */
           memscpy((void *)tm_pa_nv_write_buffer.gpsone_password.password,
                   tm_pa_nv_write_buffer.gpsone_password.password_len,
                   (void *)set_param_ptr->pa_info_type_ptr->ssd_info.ssd_data,
                   sizeof(set_param_ptr->pa_info_type_ptr->ssd_info.ssd_data));

           (void)sm_nv_write(NV_GPSONE_PASSWORD_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           sec_enable_data.base_ssd.ssd_length = tm_pa_nv_write_buffer.gpsone_password.password_len;

           /* If length is not valid, tm_sec_enable will create random SSDs */
           if ( (sec_enable_data.base_ssd.ssd_length > 0)
                && (sec_enable_data.base_ssd.ssd_length <= PDSM_MAX_SSD_LEN) )
           {
             memscpy((void *)sec_enable_data.base_ssd.ssd_data,
                     sec_enable_data.base_ssd.ssd_length,
                     (void *)tm_pa_nv_write_buffer.gpsone_password.password,
                     sizeof(tm_pa_nv_write_buffer.gpsone_password.password));
           }

           sec_enable_data.data_validity_mask = TM_SEC_DATA_VALIDITY_SSD;

           tm_sec_enable(&sec_enable_data, GPSONE_SEC_FLAG);

           retVal = TRUE; /*lint !e506 */
           break;


         case PDSM_PA_SEC_UPDATE_RATE:
           MSG_MED("Writing Sec update to NV", 0, 0, 0);

           tm_pa_nv_write_buffer.gps1_sec_update_rate = set_param_ptr->pa_info_type_ptr->sec_rate.rate;

           (void)sm_nv_write(NV_GPS1_SEC_UPDATE_RATE_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           sec_enable_data.rand_update_rate.rate = tm_pa_nv_write_buffer.gps1_sec_update_rate;

           sec_enable_data.data_validity_mask = TM_SEC_DATA_VALIDITY_RAND;

           tm_sec_enable(&sec_enable_data, GPSONE_SEC_FLAG);

           retVal = TRUE; /*lint !e506 */
           break;

#endif /* FEATURE_TM_SECURITY */
         case PDSM_PA_NMEA_CONFIG_INFO:
           MSG_MED("Writing NMEA Port Type Setting (%x) to NV", set_param_ptr->pa_info_type_ptr->nmea_config_info.pd_nmea_port_type, 0, 0);

           /* Update RAM copy first */
           tm_core_info.config_info.nmea_config_type.pd_nmea_port_type = set_param_ptr->pa_info_type_ptr->nmea_config_info.pd_nmea_port_type;
           tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type = set_param_ptr->pa_info_type_ptr->nmea_config_info.pd_nmea_reporting_type;

           /* Write to NV */
           tm_pa_nv_write_buffer.cgps_nmea_config_info.nmea_port_type = tm_core_info.config_info.nmea_config_type.pd_nmea_port_type;
           tm_pa_nv_write_buffer.cgps_nmea_config_info.nmea_reporting_type = tm_core_info.config_info.nmea_config_type.pd_nmea_reporting_type;
           (void)sm_nv_write(NV_CGPS_NMEA_CONFIG_INFO_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           if ( tm_core_info.config_info.nmea_config_type.pd_nmea_port_type != PDSM_PA_NMEA_PORT_TYPE_NONE )
           {
             tm_nmea_diag_open_port((tm_nmea_diag_open_port_id_e_type)tm_core_info.config_info.nmea_config_type.pd_nmea_port_type);
           }
           else
           {
             tm_nmea_diag_close_port(NULL);
           }


           retVal = TRUE; /*lint !e506 */
           break;

         case PDSM_PA_SBAS_USER_PREFERENCE:
           MSG_MED("Writing SBAS_User Preference (%d) to NV",
                   set_param_ptr->pa_info_type_ptr->sbas_user_preference, 0, 0); /*lint !e571 */

           /* Write to NV */
           tm_pa_nv_write_buffer.cgps_sbas_user_preference =
                                                             set_param_ptr->pa_info_type_ptr->sbas_user_preference;

           (void)sm_nv_write(NV_CGPS_SBAS_USER_PREFERENCE_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           // notify MGP of this change
           mgp_InjectSbasUserPreference(set_param_ptr->pa_info_type_ptr->sbas_user_preference);

           retVal = TRUE; /*lint !e506 */
           break;


         case PDSM_PA_EFS_DATA:
         {
#ifdef FEATURE_CGPS_FILE_SYSTEM
           retVal = tm_core_handle_access_efs_data(set_param_ptr, &event_payload);
#endif /* FEATURE_CGPS_FILE_SYSTEM */
           break;
         }

         case PDSM_PA_ON_DEMAND_LOW_POWER_MODE:
           tm_core_info.config_info.on_demand_lpm = set_param_ptr->pa_info_type_ptr->on_demand_lpm;
           //tm_core_on_demand_lpm();
           event_payload.pa_event_payload.pa_event = PDSM_PA_ON_DEMAND_LOW_POWER_MODE;
           tm_pa_nv_write_buffer.cgps_on_demand_enabled = set_param_ptr->pa_info_type_ptr->on_demand_lpm;
           (void)sm_nv_write(NV_CGPS_ON_DEMAND_ENABLED_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);
           event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                                 PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }
           break;

         case PDSM_PA_ON_DEMAND_READY_MODE:
           tm_core_info.config_info.on_demand_ready = set_param_ptr->pa_info_type_ptr->on_demand_ready;
           event_payload.pa_event_payload.pa_event = PDSM_PA_ON_DEMAND_READY_MODE;
           event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_STANDALONE);
           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                                 PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }
           /* send the pdapi event call back since we dont need to write to NV */
           pa_info_data.client_id                  = tm_core_info.pa_client_info.client_id;
           pa_info_data.pa_event                   = PDSM_PA_ON_DEMAND_READY_MODE;
           pa_info_data.pa_info.on_demand_ready = tm_core_info.config_info.on_demand_ready;
           tm_pdapi_pa_event_callback(&pa_info_data);
           break;

         case PDSM_PA_CALL_FLOW_SELECT:

           /* Save the call flow select mask in tm_info */
           tm_core_info.config_info.call_flow_select_mask = set_param_ptr->pa_info_type_ptr->call_flow_select_mask;

           /* Update 2G/3G in ram copy and NV items */
           /* NV Items:  */
           /* NV_AAGPS_3G_MO_LRSUPPORT_V - is for MO-LR call flow */
           /* NV_AAGPS_2G_MO_LRSUPPORT_V - is for AD call flow    */

           /* 2G */
           (set_param_ptr->pa_info_type_ptr->call_flow_select_mask & PDSM_PA_CALL_FLOW_SELECT_2G) ?
              (tm_pa_nv_write_buffer.aagps_2g_mo_lrsupport = NV_AAGPS_3G_MO_LRSUPPORT_V) :
              (tm_pa_nv_write_buffer.aagps_2g_mo_lrsupport = NV_AAGPS_2G_MO_LRSUPPORT_V);
           (void)sm_nv_write(NV_AAGPS_2G_MO_LRSUPPORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* 3G */
           (set_param_ptr->pa_info_type_ptr->call_flow_select_mask & PDSM_PA_CALL_FLOW_SELECT_3G) ?
              (tm_pa_nv_write_buffer.aagps_3g_mo_lrsupport = NV_AAGPS_3G_MO_LRSUPPORT_V) :
              (tm_pa_nv_write_buffer.aagps_3g_mo_lrsupport = NV_AAGPS_2G_MO_LRSUPPORT_V);
           (void)sm_nv_write(NV_AAGPS_3G_MO_LRSUPPORT_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* Update TM UMTS sub protocol module's memory copy of NV items */
           event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
           event_payload.pa_event_payload.pa_event = PDSM_PA_CALL_FLOW_SELECT;

           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_CP);
           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               (void)tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(
                                                                                 PRTL_EVENT_TYPE_PA,
                                                                                 &event_payload);
             }
           }
           break;

         case PDSM_PA_EXTERNAL_SPI:  /* no NV writes are currently required for PDSM_PA_EXTERNAL_SPI */
         {

           pdsm_pa_info_s_type xspi_pa_info_data;

           MSG_MED("Changing XSPI Control Setting %ld", set_param_ptr->pa_info_type_ptr->external_spi.mode, 0, 0);
           if ( PDSM_PA_EXTERNAL_SPI_MODE_DISABLE == set_param_ptr->pa_info_type_ptr->external_spi.mode )
           {
             tm_xspi_disable();
           }
           else
           {
             tm_xspi_enable();
           }

           /* send the pdapi event call back since we dont need to write to NV */
           xspi_pa_info_data.client_id           = set_param_ptr->client_id;
           xspi_pa_info_data.pa_event            = set_param_ptr->e_pa_type;
           xspi_pa_info_data.pa_info.external_spi.mode = set_param_ptr->pa_info_type_ptr->external_spi.mode;
           tm_pdapi_pa_event_callback(&xspi_pa_info_data);
           retVal = TRUE;
           break;

         }

#ifdef FEATURE_CGPS_XTRA_T
         case PDSM_PA_XTRAT_CLIENT_TOKEN:
         {
           boolean             tle_send_success;
           tle_api_info_s_type xtrat_xtmapi_client_info;

           xtrat_xtmapi_client_info.type = TLE_API_E_CLIENT_TOKEN;
           xtrat_xtmapi_client_info.data.client_token = (uint32)set_param_ptr->pa_info_type_ptr->client_cookie;
           xtrat_xtmapi_client_info.client_id = set_param_ptr->client_id;

           tle_send_success = tle_SendXTRATApiInfo(&xtrat_xtmapi_client_info);

           if ( tle_send_success )
           {
             retVal = TRUE;
           }
           else
           {
             retVal = FALSE;
             MSG_HIGH("XTRA-T GNSS_NV_EFS_TLE_XTRAT_CLIENT_TOKEN could not be set", 0, 0, 0);
           }
         }
           break;

         case PDSM_PA_XTRAT_USER_SESSION_CONTROL:
         {
           pdsm_pa_info_s_type pa_info;
           tle_api_info_s_type xtrat_xtmapi_session_info;

           xtra_t_rcvd_keys[xtra_t_key_idx++] = (uint8)set_param_ptr->pa_info_type_ptr->session_control;
           if ( xtra_t_key_idx > XTRA_T_RCVD_KEY_MAX )
           {
             xtra_t_key_idx = 0;

             /* Check for matching XTRA-T Handshack */
             if ( (xtra_t_rcvd_keys[0] == XTRA_T_HANDSHAKE_KEY_0) &&
                  (xtra_t_rcvd_keys[1] == XTRA_T_HANDSHAKE_KEY_1) &&
                  (xtra_t_rcvd_keys[2] == XTRA_T_HANDSHAKE_KEY_2) &&
                  (xtra_t_rcvd_keys[3] == XTRA_T_HANDSHAKE_KEY_3) )
             {
               boolean tle_send_success;

               xtrat_xtmapi_session_info.type = TLE_API_E_USER_SESS_CONTROL;
               xtrat_xtmapi_session_info.data.session_control = xtra_t_rcvd_keys[XTRA_T_RCVD_KEY_MAX];
               xtrat_xtmapi_session_info.client_id = set_param_ptr->client_id;
               tle_send_success = tle_SendXTRATApiInfo(&xtrat_xtmapi_session_info);

               if ( tle_send_success )
               {
                 retVal = TRUE;
               }
               else
               {
                 retVal = FALSE;
                 MSG_HIGH("XTRA-T GNSS_NV_EFS_TLE_XTRAT_USER_SESSION_CONTROL could not be set", 0, 0, 0);
               }
             }
           }

           /* send the pdapi event call back since we dont need to write to NV */
           pa_info.client_id               = set_param_ptr->client_id;
           pa_info.pa_event                = set_param_ptr->e_pa_type;
           pa_info.pa_info.session_control = set_param_ptr->pa_info_type_ptr->session_control;
           tm_pdapi_pa_event_callback(&pa_info);
         }
           break;
#endif /* FEATURE_CGPS_XTRA_T */

         case PDSM_PA_DATA_ENABLE:
         {
#ifndef FEATURE_GNSS_SA
           pd_comms_app_set_data_enable(set_param_ptr->pa_info_type_ptr->data_enable);
#endif /* FEATURE_GNSS_SA */
           retVal = TRUE;
         }
           break;
         case PDSM_PA_LBS_APN_PROFILE:
         case PDSM_PA_XTRA_APN_PROFILE:
         case PDSM_PA_EMERGENCY_LBS_APN_PROFILE:
         {
#ifndef FEATURE_GNSS_SA
           pd_comms_set_apn_profiles(set_param_ptr->client_id, set_param_ptr->e_pa_type, set_param_ptr->pa_info_type_ptr->apn_profiles);
#endif /* FEATURE_GNSS_SA */
           retVal = TRUE;
         }
           break;

         case PDSM_PA_SUPL_VERSION:
         {
           MSG_MED("Writing SUPL Version Setting (%x) to NV", set_param_ptr->pa_info_type_ptr->supl_version, 0, 0); /*lint !e571 */

           /* Write to NV */
           tm_pa_nv_write_buffer.gnss_supl_version = set_param_ptr->pa_info_type_ptr->supl_version;
           (void)sm_nv_write(NV_GNSS_SUPL_VERSION_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* Inform protocol sub-modules for PA changes */
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_UMTS_UP, PDSM_PA_SUPL_VERSION, set_param_ptr);

           retVal = TRUE;
         }
           break;

         case PDSM_PA_SUPL_IS801:
         {
           MSG_MED("Writing SUPL IS801 Setting (%d) to NV", (uint8)set_param_ptr->pa_info_type_ptr->supl_is801, 0, 0);
           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_1X_UP_SUPL_ENABLE, sizeof(uint8),
                                    (void *)&set_param_ptr->pa_info_type_ptr->supl_is801);
           TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable) = (uint8)set_param_ptr->pa_info_type_ptr->supl_is801;

           /* Inform UMTS-UP protocol sub-modules for PA changes */
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_UMTS_UP, PDSM_PA_SUPL_IS801, set_param_ptr);

           /* Inform 1X UP protocol sub-modules for PA changes */
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_1X_UP, PDSM_PA_SUPL_IS801, set_param_ptr);

           /* send the call back to the client */
           pa_info_data.client_id          = set_param_ptr->client_id;
           pa_info_data.pa_event           = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.supl_is801 = set_param_ptr->pa_info_type_ptr->supl_is801;
           tm_pdapi_pa_event_callback(&pa_info_data);

           retVal = TRUE; /*lint !e506 */
         }
           break;

         case PDSM_PA_SUPL_SECURITY:
         case PDSM_PA_SUPL_TLS_VERSION:
         case PDSM_PA_SUPL_HASH_ALGORITHM:
         {
           // read it back, only bit 0 is for SUPL security, bit 1 is for TLS version, bit 2 for hash algorithm
           (void)sm_nv_read_ex(NV_AAGPS_USE_TRANSPORT_SECURITY_I, (void *)&tm_pa_nv_write_buffer);

           if ( set_param_ptr->e_pa_type == PDSM_PA_SUPL_SECURITY )
           {
             MSG_MED("Writing SUPL Security Setting (%x) to NV",
                     (uint8)set_param_ptr->pa_info_type_ptr->supl_security, 0, 0); /*lint !e571 */

             tm_pa_nv_write_buffer.aagps_use_transport_security &= 0xfe;
             tm_pa_nv_write_buffer.aagps_use_transport_security |=
                                                                   ((uint8)set_param_ptr->pa_info_type_ptr->supl_security & 0x01);
           }
           else if ( set_param_ptr->e_pa_type == PDSM_PA_SUPL_TLS_VERSION )
           {
             MSG_MED("Writing SUPL TLS Version Setting (%x) to NV",
                     set_param_ptr->pa_info_type_ptr->supl_tls_version, 0, 0); /*lint !e571 */

             tm_pa_nv_write_buffer.aagps_use_transport_security &= 0xfd;
             // Bit 1 is for SUPL2 TLS version, if bit is not set(default), TLS1.1; otherwise TLS1.0
             if ( set_param_ptr->pa_info_type_ptr->supl_tls_version == PDSM_PA_SUPL_TLS_VERSION_1_0 )
             {
               tm_pa_nv_write_buffer.aagps_use_transport_security |= 0x02;
             }
           }
           else if ( set_param_ptr->e_pa_type == PDSM_PA_SUPL_HASH_ALGORITHM )
           {
             MSG_MED("Writing SUPL Hash Algorithm Setting (%x) to NV",
                     (uint32)set_param_ptr->pa_info_type_ptr->supl_hash_algorithm, 0, 0); /*lint !e571 */

             tm_pa_nv_write_buffer.aagps_use_transport_security &= 0xfb;
             // Bit 2 is for SUPL2 hash algorithm, if bit is not set(default), SHA256; otherwise SHA1
             if ( set_param_ptr->pa_info_type_ptr->supl_hash_algorithm == PDSM_PA_SUPL_HASH_ALGO_SHA1 )
             {
               tm_pa_nv_write_buffer.aagps_use_transport_security |= 0x04;
             }
           }

           /* Write to NV */
           (void)sm_nv_write(NV_AAGPS_USE_TRANSPORT_SECURITY_I, &tm_pa_nv_write_buffer, tm_nv_rdrw_handle);

           /* send the call back to the client */
           pa_info_data.client_id = set_param_ptr->client_id;
           pa_info_data.pa_event  = set_param_ptr->e_pa_type;
           pa_info_data.pa_info   = *(set_param_ptr->pa_info_type_ptr);
           tm_pdapi_pa_event_callback(&pa_info_data);

           /* Inform UMTS-UP protocol sub-modules for PA changes, always use SUPL_SECURITY for SUPL
              SUPL module will copy this uint8 security to its RAM variable (Security/hash/TLS) */
           set_param_ptr->pa_info_type_ptr->supl_security = (boolean)tm_pa_nv_write_buffer.aagps_use_transport_security;
           (void)tm_core_inform_protocol_param_change(TM_PRTL_TYPE_UMTS_UP, PDSM_PA_SUPL_SECURITY, set_param_ptr);

           retVal = TRUE;
         }
           break;

         case PDSM_PA_PE_CONFIG_PARAM:
         {
           boolean bPEConfig;

           MSG_MED("Setting PE config param flag=0x%x", set_param_ptr->pa_info_type_ptr->pe_config_info.peConfigMask, 0, 0);

           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.pe_config_info = set_param_ptr->pa_info_type_ptr->pe_config_info;

           bPEConfig = mgp_SetPositioningMethodConfig(&(set_param_ptr->pa_info_type_ptr->pe_config_info));
           if ( bPEConfig == FALSE )
           {
             MSG_ERROR("Failed to set PE config param", 0, 0, 0);
             pa_info_data.pa_info.pe_config_info.peConfigMask = 0; // mark all as failure
           }
           tm_pdapi_pa_event_callback(&pa_info_data);

           retVal = TRUE;
         }
           break;

         case PDSM_PA_LPP_CONFIG_INFO:
           MSG_MED("Writing LPP Config Info (%x) to NV", set_param_ptr->pa_info_type_ptr->lpp_config_info, 0, 0); /*lint !e571 */
           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_LPP_ENABLE, sizeof(uint8),
                                    &set_param_ptr->pa_info_type_ptr->lpp_config_info);
           TM_CORE_CONFIG_DD(gnss_lte_lpp_enable) = set_param_ptr->pa_info_type_ptr->lpp_config_info;

           /* Inform protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_UMTS_UP);

           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_LPP_CONFIG_INFO;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                    &event_payload);
             }
           }

           /* Inform LPP CP protocol sub-modules for PA changes */
           prtlTableIndex = tm_core_get_prtl_idx_by_type(TM_PRTL_TYPE_LPP_CP);
           if ( (prtlTableIndex > -1) && (prtlTableIndex < TM_PRTL_NUM) )
           {
             if ( tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp != NULL )
             {
               event_payload.pa_event_payload.pa_event = PDSM_PA_LPP_CONFIG_INFO;
               event_payload.pa_event_payload.pa_info_ptr = set_param_ptr->pa_info_type_ptr;
               retVal = tm_core_info.prtl_func_cb_table[prtlTableIndex].event_cb_fp(PRTL_EVENT_TYPE_PA,
                                                                                    &event_payload);
             }
           }
           /* send the call back to the client */
           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.lpp_config_info = set_param_ptr->pa_info_type_ptr->lpp_config_info;
           tm_pdapi_pa_event_callback(&pa_info_data);
           retVal = TRUE;
           break;

         case PDSM_PA_ASST_GLONASS_POS_PROTOCOL:
         {
           MSG_MED("Writing A-Glonass position protocol choices (0x%X) to NV", set_param_ptr->pa_info_type_ptr->asst_glo_protocol, 0, 0); /*lint !e571 */
           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT, sizeof(uint32),
                                    &set_param_ptr->pa_info_type_ptr->asst_glo_protocol);
           TM_CORE_CONFIG_DD(asst_glonass_pos_protocol) = set_param_ptr->pa_info_type_ptr->asst_glo_protocol;

           /* No call to protocol sub-modules since no special handling for this PA event change. */

           /* send the call back to the client */
           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.asst_glo_protocol = set_param_ptr->pa_info_type_ptr->asst_glo_protocol;
           tm_pdapi_pa_event_callback(&pa_info_data);
           retVal = TRUE;
         }
           break;

         case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
         {
           MSG_MED("Setting Emergency protocol selection", 0, 0, 0);

           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_EMERGENCY_PROTOCOL_SELECT, sizeof(uint32),
                                    &set_param_ptr->pa_info_type_ptr->gnss_emergency_protocol_select);
           tm_core_info.config_info.gnss_emergency_protocol_non_lte = set_param_ptr->pa_info_type_ptr->gnss_emergency_protocol_select;
           /* send the call back to the client */
           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.asst_glo_protocol = set_param_ptr->pa_info_type_ptr->asst_glo_protocol;
           tm_pdapi_pa_event_callback(&pa_info_data);
           retVal = TRUE;

         }
           break;

         case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
         {

           /* send the call back to the client */
           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.wifi_wait_timeout_sec = set_param_ptr->pa_info_type_ptr->wifi_wait_timeout_sec;
           tm_pdapi_pa_event_callback(&pa_info_data);

           tm_core_info.config_info.wifi_wait_timeout_sec =
                                                            (set_param_ptr->pa_info_type_ptr->wifi_wait_timeout_sec < TM_WIFI_WAIT_TIMEOUT_SEC_MAX) ?
                                                            set_param_ptr->pa_info_type_ptr->wifi_wait_timeout_sec : TM_WIFI_WAIT_TIMEOUT_SEC_MAX;

           MSG_MED("Setting WIFI scan time out value: %u, %u",
                   set_param_ptr->pa_info_type_ptr->wifi_wait_timeout_sec,
                   tm_core_info.config_info.wifi_wait_timeout_sec, 0);

           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_TM_WIFI_WAIT_TIMEOUT_SEC, sizeof(uint8),
                                    &tm_core_info.config_info.wifi_wait_timeout_sec);

           retVal = TRUE;

         }
           break;

         case PDSM_PA_PREMIUM_SERVICES_CONFIG:
         {
           MSG_MED("Writing Premium Service=%d configuration=%d to NV", set_param_ptr->pa_info_type_ptr->premium_svc_cfg.service,
                   set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config, 0);

           switch ( set_param_ptr->pa_info_type_ptr->premium_svc_cfg.service )
           {
              case PDSM_PA_PREMIUM_SVC_GTP_CELL:
#ifdef FEATURE_CGPS_XTRA_T
              {
                tle_api_gtp_config_s_type z_GtpConfig;

                z_GtpConfig.e_Service = TLE_API_E_SERV_GTP;
                if ( PDSM_PA_PREMIUM_SVC_CFG_DISABLED == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: GTP CELL, configuration disabled", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_DISABLED;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_BASIC == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: GTP CELL, configuration basic", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_BASIC;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_PREMIUM == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: GTP CELL, configuration perimium", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_PREMIUM;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else
                {
                  MSG_MED("Premium Service: GTP CELL, invalid", 0, 0, 0);
                }
              }
#endif /* FEATURE_CGPS_XTRA_T */
                /* Send the pdapi event call back since we dont need to write to NV */
                pa_info_data.client_id               = set_param_ptr->client_id;
                pa_info_data.pa_event                = set_param_ptr->e_pa_type;
                pa_info_data.pa_info.premium_svc_cfg = set_param_ptr->pa_info_type_ptr->premium_svc_cfg;
                tm_pdapi_pa_event_callback(&pa_info_data);
                break;

              case PDSM_PA_PREMIUM_SVC_GTP_ENH_CELL:
#ifdef FEATURE_CGPS_XTRA_T
              {
                tle_api_gtp_config_s_type z_GtpConfig;

                z_GtpConfig.e_Service = TLE_API_E_SERV_TDP;
                if ( PDSM_PA_PREMIUM_SVC_CFG_DISABLED == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: TDP , configuration disabled", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_DISABLED;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_BASIC == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: TDP , configuration basic", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_PREMIUM;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_PREMIUM == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: TDP , configuration perimium", 0, 0, 0);
                  z_GtpConfig.e_Config = TLE_API_E_GTP_PREMIUM;
                  tle_SetGTPConfiguration(&z_GtpConfig);
                }
                else
                {
                  MSG_MED("Premium Service: TDP , invalid", 0, 0, 0);
                }
              }
#endif /* FEATURE_CGPS_XTRA_T */
                /* Send the pdapi event call back since we dont need to write to NV */
                pa_info_data.client_id               = set_param_ptr->client_id;
                pa_info_data.pa_event                = set_param_ptr->e_pa_type;
                pa_info_data.pa_info.premium_svc_cfg = set_param_ptr->pa_info_type_ptr->premium_svc_cfg;
                tm_pdapi_pa_event_callback(&pa_info_data);

                break;

              case PDSM_PA_PREMIUM_SVC_SAP:
              {
                lsmp_premium_services_param_type q_SdpPremiumSvcCfg = LSMP_PREMIUM_SERVICES_TYPE_ENABLE_BASIC;
                uint32                           q_PremiumSvcCfg    = GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_BASIC;

                if ( PDSM_PA_PREMIUM_SVC_CFG_DISABLED == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: SAP, configuration disabled", 0, 0, 0);
                  q_SdpPremiumSvcCfg = LSMP_PREMIUM_SERVICES_TYPE_DISABLE;
                  q_PremiumSvcCfg    = GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_DISABLE;
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_BASIC == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: SAP, configuration enabled basic", 0, 0, 0);
                  q_SdpPremiumSvcCfg = LSMP_PREMIUM_SERVICES_TYPE_ENABLE_BASIC;
                  q_PremiumSvcCfg    = GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_BASIC;
                }
                else if ( PDSM_PA_PREMIUM_SVC_CFG_ENABLED_PREMIUM == set_param_ptr->pa_info_type_ptr->premium_svc_cfg.config )
                {
                  MSG_MED("Premium Service: SAP, configuration enabled premium", 0, 0, 0);
                  q_SdpPremiumSvcCfg = LSMP_PREMIUM_SERVICES_TYPE_ENABLE_PREMIUM;
                  q_PremiumSvcCfg    = GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES_ENABLE_PREMIUM;
                }
                else
                {
                  MSG_ERROR("Premium Service: SAP, configuration invalid", 0, 0, 0);
                }

                /* Write Premium Service Config for SAP to NV */
                sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_CONFIG_PREMIUM_SERVICES,
                                         sizeof(uint32),
                                         (void *)&q_PremiumSvcCfg);

                /* Save the Premium Service configuration for SAP in TM Core */
                tm_core_info.config_info.premium_services_config = q_PremiumSvcCfg;

#ifdef FEATURE_CGPS_XTRA_T
                tle_InjectInsCapability(q_SdpPremiumSvcCfg);
#endif
                /* For SAP, pdapi event call back not sent here, since SDP module will return config status  */
                /* So need to save ongoing Set Premium Services Config state for pdapi event call back later */
                tm_core_info.config_info.prem_svc_cfg_info.prem_svc_cfg_waiting = TRUE;
                tm_core_info.config_info.prem_svc_cfg_info.client_id            = set_param_ptr->client_id;
                tm_core_info.config_info.prem_svc_cfg_info.premium_svc_cfg      = set_param_ptr->pa_info_type_ptr->premium_svc_cfg;

                /* Send the Premium Service configuration for SAP to SDP module */
                (void)sdp_send_premium_svc_cfg_proxy(q_SdpPremiumSvcCfg);
              }
                break;

              default:
                MSG_ERROR("Unknown premium service type", 0, 0, 0);
                break;
           }

           retVal = TRUE;
         }
           break;

         case PDSM_PA_XTRA_VERSION_CHECK:
         {
           MSG_MED("Writing XTRA Version Check configuration=%d to TM", set_param_ptr->pa_info_type_ptr->xtra_version_check, 0, 0);
           tm_xtra_update_version_check(set_param_ptr->pa_info_type_ptr->xtra_version_check);

           /* Send the pdapi event call back since we dont need to write to NV */
           pa_info_data.client_id                  = set_param_ptr->client_id;
           pa_info_data.pa_event                   = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.xtra_version_check = set_param_ptr->pa_info_type_ptr->xtra_version_check;
           tm_pdapi_pa_event_callback(&pa_info_data);

           retVal = TRUE;
         }
           break;

         case PDSM_PA_LPP_GRT_24_OTDOA_AD_SUPPORT:
         {
           MSG_MED("Writing LPP >24 AD/meas choice (0x%X) to NV", set_param_ptr->pa_info_type_ptr->u_gnss_lpp_grt_24_otdoa, 0, 0); /*lint !e571 */
           sm_nv_efs_reg_item_write(GNSS_NV_EFS_SM_LPP_GRT_24_OTDOA_AD_SUPPORT, sizeof(uint8),
                                    &(set_param_ptr->pa_info_type_ptr->u_gnss_lpp_grt_24_otdoa));

           /* Save the configuration for LPP > 24 AD/Meas in TM Core */
           tm_core_info.config_info.u_gnss_lpp_grt_24_otdoa = set_param_ptr->pa_info_type_ptr->u_gnss_lpp_grt_24_otdoa;

           /* Send the call back to the client */
           pa_info_data.client_id              = set_param_ptr->client_id;
           pa_info_data.pa_event               = set_param_ptr->e_pa_type;
           pa_info_data.pa_info.u_gnss_lpp_grt_24_otdoa = set_param_ptr->pa_info_type_ptr->u_gnss_lpp_grt_24_otdoa;
           tm_pdapi_pa_event_callback(&pa_info_data);
           retVal = TRUE;
         }
           break;

         default:
           MSG_ERROR("Unknown set param type", 0, 0, 0);
           retVal = FALSE;
           break;
      }
    }
  }
  return ( retVal ); /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_core_xtra_req_check

DESCRIPTION
   Check the current TM-Core state to determine if XTRA request is allowed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_xtra_cmd_err_e_type tm_core_xtra_req_check
(
  const pdsm_xtra_cmd_s_type *xtra_clnt_req_ptr
)
{
#ifdef FEATURE_CGPS_XTRA

  /* Local Variables */
  pdsm_xtra_cmd_err_e_type       xtra_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;
  tm_cm_phone_state_info_s_type  phone_state_info;
  pdsm_client_id_type            xtra_client_id;

  do{
    /*----------------------------------------------------------------------
       First, check if the TM task is active
    ----------------------------------------------------------------------*/
    if(tm_core_info.tm_task_info.task_active == FALSE)
    {
      MSG_HIGH("Task is not active", 0, 0, 0);
      xtra_cmd_err =  PDSM_XTRA_CMD_ERR_INTERNAL_RESOURCE_ERROR;
      break;
    }

    /*----------------------------------------------------------------------
       Check if the TM task is active
    ----------------------------------------------------------------------*/
    if(xtra_clnt_req_ptr == NULL)
    {
      MSG_HIGH("NULL ptr", 0, 0, 0);
      xtra_cmd_err =  PDSM_XTRA_CMD_ERR_INVALID_PARAM;
      break;
    }

    /*----------------------------------------------------------------------
       Only grant XTRA client ID access
    ----------------------------------------------------------------------*/
    xtra_client_id = pdsmclient_get_client_id_map(PDSM_CLIENT_TYPE_XTRA);

    if ( xtra_client_id != xtra_clnt_req_ptr->client_id)
    {
      MSG_ERROR("TM_PDAPI: PDSM_XTRA_CMD_ERR_CLIENT_ID_P",0,0,0);
      xtra_cmd_err = PDSM_XTRA_CMD_ERR_CLIENT_ID_P;
      break;
    }

    /*----------------------------------------------------------------------
      E911 related checks (E911 call orgination, E911 CBW, ...etc
    ----------------------------------------------------------------------*/
    tm_cm_iface_get_phone_state(&phone_state_info);

    if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
    {
      MSG_ERROR("Phone in E911 state", 0, 0, 0);
      xtra_cmd_err = PDSM_XTRA_CMD_ERR_STATE_S;
      break;
    }

    /*----------------------------------------------------------------------
       GPS lock is enabled
    ----------------------------------------------------------------------*/
    if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_MI)
      ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_ALL))
    {
      MSG_ERROR("MI GPS is locked", 0, 0, 0);
      xtra_cmd_err = PDSM_XTRA_CMD_ERR_GPS_LOCK_S;
      break;
    }

    /*----------------------------------------------------------------------
       Check for command parameter error
    ----------------------------------------------------------------------*/
    switch ( xtra_clnt_req_ptr->cmd )
    {
      case PDSM_XTRA_CMD_SET_DATA:
           xtra_cmd_err = tm_xtra_check_set_data_para(xtra_clnt_req_ptr);
           if (xtra_cmd_err != PDSM_XTRA_CMD_ERR_NOERR)
           {
             // for SET_DATA command failure, need to reset buffer
             MSG_ERROR("XTRA_CMD_SET_DATA failure %d, reset XTRA buffer", xtra_cmd_err,0,0);
             tm_xtra_reset_data_buffer();
           }
           break;

      case PDSM_XTRA_CMD_INITIATE_DOWNLOAD:
           xtra_cmd_err = tm_xtra_check_initiate_download_para(xtra_clnt_req_ptr);
           break;

      case PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS:
           xtra_cmd_err = tm_xtra_check_auto_download_para(xtra_clnt_req_ptr);
           break;

      case PDSM_XTRA_CMD_QUERY_DATA_VALIDITY:
           xtra_cmd_err = tm_xtra_check_query_data_validity_para(xtra_clnt_req_ptr);
           break;

      case PDSM_XTRA_CMD_TIME_INFO_INJ:
           xtra_cmd_err = tm_xtra_check_time_info_inject_para(xtra_clnt_req_ptr);
           break;

      default:
           MSG_ERROR("XTRA: Unrecognized XTRA cmd %d",
                     xtra_clnt_req_ptr->cmd,0,0);
           xtra_cmd_err = PDSM_XTRA_CMD_ERR_UNKNOWN_CMD;
           break;
    } /* end switch */
  } while (0);


  /* XTRA request is granted */
  return xtra_cmd_err;

#else
  return PDSM_XTRA_CMD_ERR_FEATURE_NOT_ENABLED;
#endif /* FEATURE_CGPS_XTRA */
} /* End tm_core_xtra_req_check() */

/*===========================================================================

FUNCTION tm_core_handle_xtra_client_cmd

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_xtra_client_cmd
(
  pdsm_xtra_cmd_s_type *cmd_ptr
)
{
#ifdef FEATURE_CGPS_XTRA

  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check if the TM task is active
  ----------------------------------------------------------------------*/
  if(cmd_ptr == NULL)
  {
    MSG_HIGH("NULL ptr", 0, 0, 0);
    return ret_val;
  }

  /*----------------------------------------------------------------------
     Process the incoming XTRA client request
  ----------------------------------------------------------------------*/
  switch ( cmd_ptr->cmd )
  {
    case PDSM_XTRA_CMD_SET_DATA:
         ret_val = tm_xtra_process_set_data_cmd(cmd_ptr);
         break;
    case PDSM_XTRA_CMD_TIME_INFO_INJ:
        ret_val = tm_xtra_process_inject_time_info(cmd_ptr);
        break;

    case PDSM_XTRA_CMD_INITIATE_DOWNLOAD:
         ret_val = tm_xtra_process_initiate_download_cmd(cmd_ptr);
         break;

    case PDSM_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS:
         ret_val = tm_xtra_process_set_auto_download_cmd(cmd_ptr);
         break;

    case PDSM_XTRA_CMD_QUERY_DATA_VALIDITY:
         ret_val = tm_xtra_process_query_data_validity_cmd(cmd_ptr);
         break;

    default:
         MSG_ERROR("gpOneXTRA: Unrecognized XTRA cmd %d, abort processing",
                   cmd_ptr->cmd,0,0);
         break;
  } /* end switch */

  return ret_val;

#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
}


/*===========================================================================

FUNCTION tm_core_handle_wiper_client_cmd

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_wiper_client_cmd
(
  pdsm_wiper_cmd_s_type *cmd_ptr
)
{
  /* Local Variables */

  /*----------------------------------------------------------------------
     Check if the TM task is active
  ----------------------------------------------------------------------*/
  if(cmd_ptr == NULL)
  {
    MSG_HIGH("NULL ptr", 0, 0, 0);
    return;
  }

  /*----------------------------------------------------------------------
     Process the incoming WIPER client request
  ----------------------------------------------------------------------*/
  switch ( cmd_ptr->cmd )
  {
    case PDSM_WIPER_CMD_LOG_POSITION_REPORT:
         tm_wiper_core_position_report(cmd_ptr->info.position_report_ptr);
         break;

    default:
         MSG_ERROR("gpOneWIPER: Unrecognized WIPER cmd %d, abort processing",
                   cmd_ptr->cmd,0,0);
         break;
  } /* end switch */

  return;
}

/*===========================================================================

FUNCTION tm_core_periodic_ppm_req_check

DESCRIPTION
   Check the current TM-Core state to determine if PPM start request is allowed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_pd_cmd_err_e_type tm_core_periodic_ppm_req_check
(
  const pdsm_periodic_ppm_cmd_s_type *periodic_ppm_clnt_req_ptr
)
{
#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#else
  return PDSM_PD_CMD_ERR_FEATURE_NOT_ENABLED;
#endif /* FEATURE_GNSS_PERIODIC_PPM */
} /* End tm_core_periodic_ppm_req_check() */

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

/*===========================================================================

FUNCTION tm_core_nv_write

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_write
(
  nv_items_enum_type nv_item,
  void* item_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (item_ptr == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return ret_val;
  }


  /*----------------------------------------------------------------------
     Initialize the NV handle
  ----------------------------------------------------------------------*/
  if(tm_nv_initialized == FALSE)
  {
    tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM ,tm_core_nv_write_cb,
                                       tm_core_nv_async_rd_cb);

    if(tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
    {
      tm_nv_initialized = TRUE; /*lint !e506 */
    }
  }

  /*----------------------------------------------------------------------
     Write the NV item
  ----------------------------------------------------------------------*/
  if(tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
  {
     ret_val = sm_nv_write(nv_item, item_ptr, tm_nv_rdrw_handle);
  }

  return ret_val;

} /* End tm_core_nv_write() */

/*===========================================================================

FUNCTION tm_core_nv_async_read

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_nv_async_read
(
  nv_items_enum_type nv_item,
  void* item_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (item_ptr == NULL)
  {
    MSG_ERROR("Null input pointer", 0, 0, 0);
    return ret_val;
  }


  /*----------------------------------------------------------------------
     Initialize the NV handle
  ----------------------------------------------------------------------*/
  if(tm_nv_initialized == FALSE)
  {
    tm_nv_rdrw_handle = sm_nv_rdrw_reg(THREAD_ID_SM_TM ,tm_core_nv_write_cb,
                                       tm_core_nv_async_rd_cb);

    if(tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
    {
      tm_nv_initialized = TRUE; /*lint !e506 */
    }
  }

  /*----------------------------------------------------------------------
     Read the NV item
  ----------------------------------------------------------------------*/
  if(tm_nv_rdrw_handle != SM_NV_CLIENT_HANDLE_ERROR)
  {
     ret_val = sm_nv_async_read(nv_item, item_ptr, tm_nv_rdrw_handle);
  }

  return ret_val;

} /* End tm_core_nv_async_read() */

/*===========================================================================

FUNCTION tm_core_xtra_t_req_check

DESCRIPTION
   Check the current TM-Core state to determine if XTRA-T request is allowed.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
pdsm_xtra_t_cmd_err_e_type tm_core_xtra_t_req_check
(
  const pdsm_xtra_t_cmd_s_type *xtra_t_clnt_req_ptr
)
{
#ifdef FEATURE_CGPS_XTRA_T

  /* Local Variables */
  pdsm_xtra_t_cmd_err_e_type     xtra_t_cmd_err = PDSM_XTRA_T_CMD_ERR_NONE;
  tm_cm_phone_state_info_s_type  phone_state_info;
  pdsm_client_id_type            xtra_t_client_id;

  /*----------------------------------------------------------------------
     First, check if the TM task is active
  ----------------------------------------------------------------------*/
  if(tm_core_info.tm_task_info.task_active == FALSE)
  {
    MSG_HIGH("Task is not active", 0, 0, 0);
    xtra_t_cmd_err =  PDSM_XTRA_T_CMD_ERR_INTERNAL_RESOURCE;
  }

  /*----------------------------------------------------------------------
     Check if the XTRA-T client pointer is valid
  ----------------------------------------------------------------------*/
  if(xtra_t_clnt_req_ptr == NULL)
  {
    MSG_HIGH("NULL ptr", 0, 0, 0);
    xtra_t_cmd_err =  PDSM_XTRA_T_CMD_ERR_INVALID_PARAM;
    return xtra_t_cmd_err;
  }

  /*----------------------------------------------------------------------
     Only grant XTRA-T client ID access
  ----------------------------------------------------------------------*/
  xtra_t_client_id = pdsmclient_get_client_id_map(PDSM_CLIENT_TYPE_XTRA_T);

  if ( xtra_t_client_id != xtra_t_clnt_req_ptr->client_id)
  {
    MSG_ERROR("TM_PDAPI: PDSM_XTRA_T_CMD_ERR_CLIENT_ID_P",0,0,0);
    xtra_t_cmd_err = PDSM_XTRA_T_CMD_ERR_CLIENT_ID_P;
  }

  /*----------------------------------------------------------------------
    E911 related checks (E911 call orgination, E911 CBW, ...etc
  ----------------------------------------------------------------------*/
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
  {
    MSG_ERROR("Phone in E911 state", 0, 0, 0);
    xtra_t_cmd_err = PDSM_XTRA_T_CMD_ERR_STATE_S;
  }

  /*----------------------------------------------------------------------
     GPS lock is enabled
  ----------------------------------------------------------------------*/
  if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_MI)
    ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_ALL))
  {
    MSG_ERROR("MI GPS is locked", 0, 0, 0);
    xtra_t_cmd_err = PDSM_XTRA_T_CMD_ERR_GPS_LOCK_S;
  }

  /*----------------------------------------------------------------------
     Check for command parameter error
  ----------------------------------------------------------------------*/
  switch ( xtra_t_clnt_req_ptr->cmd )
  {
    case PDSM_XTRA_T_CMD_GET_DATA:
    case PDSM_XTRA_T_CMD_SET_DATA:
    case PDSM_XTRA_T_CMD_INITIATE_DOWNLOAD:
    case PDSM_XTRA_T_CMD_INITIATE_UPLOAD:
    default:
         MSG_ERROR("XTRA-T: Unrecognized XTRA-T cmd %d",
                   xtra_t_clnt_req_ptr->cmd,0,0);
         xtra_t_cmd_err = PDSM_XTRA_T_CMD_ERR_UNKNOWN_CMD;
         break;
  } /* end switch */

  /* XTRA request is granted */
  return xtra_t_cmd_err;

#else
  return PDSM_XTRA_T_CMD_ERR_FEATURE_NOT_ENABLED;
#endif /* FEATURE_CGPS_XTRA_T */
} /* End tm_core_xtra_t_req_check() */


/*===========================================================================

FUNCTION tm_core_handle_xtra_t_client_cmd

DESCRIPTION  XTRA-T command handler

DEPENDENCIES

RETURN VALUE  TRUE - success;  FALSE - failure

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_xtra_t_client_cmd
(
  pdsm_xtra_t_cmd_s_type *cmd_ptr
)
{
#ifdef FEATURE_CGPS_XTRA_T

  /* Local Variables */
  boolean ret_val = FALSE;

  /*----------------------------------------------------------------------
     Check if the TM task is active
  ----------------------------------------------------------------------*/
  if(cmd_ptr == NULL)
  {
    MSG_HIGH("NULL ptr", 0, 0, 0);
    return ret_val;
  }

  /*----------------------------------------------------------------------
     Process the incoming XTRA-T client request
  ----------------------------------------------------------------------*/
  switch ( cmd_ptr->cmd )
  {
    case PDSM_XTRA_T_CMD_GET_DATA:
    case PDSM_XTRA_T_CMD_SET_DATA:
    case PDSM_XTRA_T_CMD_INITIATE_DOWNLOAD:
    case PDSM_XTRA_T_CMD_INITIATE_UPLOAD:
    default:
         MSG_ERROR("XTRA-T: Unrecognized XTRA-T cmd %d, abort processing",
                   cmd_ptr->cmd,0,0);
         break;
  } /* end switch */

  return ret_val;

#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA_T */
} /* tm_core_handle_xtra_t_client_cmd */

/*===========================================================================
FUNCTION tm_core_handle_reset_location_service

DESCRIPTION
  This function handles reset locaiton service request from PDAPI.
  This will bring down any ongoing sessions and will also clear XTRA state.
  This function will then pass the request to MGP.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_core_handle_reset_location_service(void)
{
  lm_request_info_s_type lm_info_req_param;

  /*Generate an Event on Reset Location Service Receive*/
  sm_report_event_reset_location_service_received();
  
  /*Record that reset location service is in progress*/ 
  tm_core_set_reset_location_service_in_progress(TRUE);

  /*Abort all sessions including periodic ppm*/
  tm_core_abort_recover(TRUE, TM_STOP_REASON_ON_RESET_LOCATION_SERVICE);

  MSG_HIGH("Sending Reset Location Services Request to MGP",0,0,0);
  /*Send the reset to LM*/  
  lm_info_req_param.e_req_type = LM_REQUEST_RESET_LOCATION_SERVICE;
  if(lm_request(&lm_info_req_param)== FALSE)
  {
    pdsm_pd_ext_reset_location_service_status_e_type status = PDSM_PD_RESET_LOCATION_SERVICE_ERROR_GENERAL;
    
    tm_core_reset_location_service_done_handler(&status);
  }
}

/*===========================================================================
FUNCTION tm_core_handle_prem_svc_status

DESCRIPTION
  This function handles the send premium service indication status ipc msg, 
  and calls tm_pdapi_pa_event_callback_ex to send event to registered clients.

DEPENDENCIES

RETURN VALUE TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
boolean tm_core_handle_prem_svc_status(void *p_Data)
{
  pdsm_pa_info_s_type    pa_info_data;
  pdsm_pa_cmd_err_e_type pa_cmd_err;

  if (NULL == p_Data)
  {
    return FALSE;
  }
  else if (!tm_core_info.config_info.prem_svc_cfg_info.prem_svc_cfg_waiting)
  {
    return FALSE;
  }
  else
  {
    /* Set Premium Service Config ongoing session confirmed */
    pa_cmd_err   = ( *((boolean*) p_Data) ) ? PDSM_PA_CMD_ERR_NOERR : PDSM_PA_CMD_ERR_OTHER;

    /* send the pdapi event call back since we dont need to write to NV */
    pa_info_data.client_id               = tm_core_info.config_info.prem_svc_cfg_info.client_id;
    pa_info_data.pa_event                = PDSM_PA_PREMIUM_SERVICES_CONFIG;
    pa_info_data.pa_info.premium_svc_cfg = tm_core_info.config_info.prem_svc_cfg_info.premium_svc_cfg;
    tm_pdapi_pa_event_callback_ex(&pa_info_data, pa_cmd_err);

    /* Reset flag since ongoing session has been serviced */
    tm_core_info.config_info.prem_svc_cfg_info.prem_svc_cfg_waiting = FALSE;

    return TRUE;
  }
}
