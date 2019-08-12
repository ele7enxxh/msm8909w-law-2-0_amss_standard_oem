/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Module

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager provides Position Determination session control and event notification
  services to its clients.


EXTERNALIZED FUNCTIONS

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/pdapi/src/tm_pdapi_client.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/26/15   muk     Handle TL1.0 messages to get the best available position estimate
06/22/15   ss      Change to allow assistance data deletion if there is no TM session but
                   engine is ON
01/07/15   jv      Added QZSS Support
10/29/14   ssu     GM LOWI Integration
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
05/12/14   ah      Added support for Set XTRA Version Check
03/10/14   ssu     Addressing a few KW errors.
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/30/14   ss      Allowing Set Param when Geofencing Session is running
06/26/13   mj      Wrap z_get_pos_param_options and qos with QWIP featurization
12/04/12   rk      CR 408753 - Initialize 'pd_data' before populating in tm_pdapi_xlate_use_pos_to_pd_ext_pos(). 
10/03/12   rh      Added a few PA commands for SUPL configuration 
09/03/12   rh      Added support for Cell Info and NI message injection 
07/22/12   rk      Added PDSM_PA_ASST_GLONASS_POS_PROTOCOL set/get parameter support.
06/12/12   rk      Update 'tm_pdapi_pa_event_callback' function with strict
                   PA event range check.
06/02/11   rh      Added PDSM_LCS_CMD_SET_VX_CONFIG and Get/Set SUPL security
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
4/02/10    gk      Added an event callback to inject position
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/01/09   gk      Added a check to validate the directory for EFS operation
08/23/09   gk      Added new event for guard timer expiry and MM ind
07/07/09   atien   Send a log of LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type when
                   receiving position from app via pdsm_use_this_position()
07/07/09   atien   Remove QOS Accuracy check for 0 for all cases.  0 is now used
                   for FEATURE_POSITION_SHOPPING
05/21/09   gk      removed checks for position modes
04/01/09   jlp     Added output of a log and f3 indicating the number of
                   successful fixes the device has had since power up.  Also
                   output an event for the first successful fix.
03/30/09   jd      Added a generic abort handling mechanism for SysD
04/02/09   ns      Support for Qwip: externed tm_pd_client_cmd_check
03/12/09   ns      SYSD triggered only for MSB/ Standalone sessions
03/02/09   jd      Added support for Wiper logging over RPC.
02/19/09   ns      Changed QWip Interface header call
02/04/09   atien   MOLR-AD call flow select
02/06/09   atien   Correct command error in tm_xtra_t_client_cmd_proc
01/02/09   atien   Support XTRA-T
01/25/09   ns      Added support for QWiP
08/08/08   cl      Enable non-encrypted lat/long when security is activated
06/09/08   gk      On-Demand Changes PDAPI events not generated correctly
4/28/08    gk      On-Demand changes
3/6/08     gk      if the server type is LOCAL and if MSA, check if the addr provided is NULL
1/16/08    gk      Dont check for qos=0 and operation mode is not msa
11/13/07    gk     Add SUPL Certificate injection through PDAPI
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07   rw      Add support for XTRA feature
06/29/07   gk      set the pd_data pointers to NULL for all the pd_events
06/05/07   gk      support settign of URL
05/27/07   rw      Add check for app info setting URL paramters
05/23/07   gk      moved return the end session command status before sending the command to TM
5/23/07    gk      added DM event for notify verify
5/09/07    gk      KDDI related changes
04/25/07   gk      modified the pa log to match the new strucutre that needed to be changed for WM
04/4/07    gk      PC Time injection handling, modifing PD_EVENT call back handling, missing cmd callbacks
02/14/07   gk      Lint
02/06/07   gk      Renamed UMTS to UMTS_CP
01/31/07   cl      Change to pass LCS response to TM-Core
1/11/07    gk      Error in comparision, changed to logical or
12/06/06   gk      Updated set parms functionality, lcs functionality
12/04/06   gk      Updated LCS response, minor mods for tbf and num_fixes
                   code review changes, add new param to pd_event_callback
11/8/06    gk      Initial checkin for Converged GPS
#########################################################################################
*/
#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */

#include "pdapiclient.h"
#include "tm_pdapi_client.h"
#include "tm_pdapi_iface.h"
#include "pdapidbg.h"
#include "tm_data.h"
#include "pdapibuf.h"
#include "sm_log.h"
#include "sm_dm_event.h"
#include "time_svc.h"
#include "event.h"
#include "event_defs.h"
#include "log.h"
#include "log_codes.h"
#include "tm_common.h"
#include "gm_api.h"
#include "tm_qwip_iface.h"

#include "mgp_api.h"
#include "tm_xspi.h"
#define XTRAT_CLIENT_COOKIE_RANGE 0XFFFFFFFF

static  pdsm_pd_info_s_type z_pd_info;

static pdsm_pd_option_s_type z_get_pos_param_options_type;
static pdsm_pd_qos_type z_get_pos_params_qos_type;

static  tm_pdapi_get_pos_param_s_type   z_get_pos_param_type;

typedef enum
{
  XTRAT_SESSION_STATE_OFF,
  XTRAT_SESSION_STATE_ON
}xtrat_session_state;


static void tm_ext_client_cmd_status(
  pdsm_cmd_s_type  *p_cmd_type,
  pdsm_ext_pos_cmd_err_e_type e_cmd_err
);

extern void tm_core_handle_wiper_client_cmd
(
  pdsm_wiper_cmd_s_type *cmd_ptr
);

extern void gm_inject_wifi_indication
(
   t_wiper_position_report_ex_struct_type *wifi_indication
);

static void tm_wiper_client_cmd_proc(
  pdsm_cmd_s_type *p_cmd_type
);

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

static uint32 cached_client_id;
static int32 cached_sess_type = TM_PDAPI_SESS_TYPE_NONE;

static void tm_pdapi_xlate_use_pos_to_pd_ext_pos
(
  pdsm_pd_ext_pos_s_type            *p_pd_ext_pos,
  pdsm_ext_use_this_pos_data_type   *p_use_pos
);

/*
   log.h says LOG_RECORD_DEFINE has been depricated.  log_hdr_type should be
   placed at the top of the structure and not accessed directly
*/
typedef PACKED struct PACKED_POST
{
   /* log header.  DO NOT ACCESS DIRECTLY! */
   log_hdr_type hdr;

   /* Version of this packet.  Currently only 0 is supported */
   uint8 u_Version;

   /* The number of successful fixes received */
   uint32 q_SuccessfulFixCount;

   /*
      The type of client that has received the successful fix.  This should be
      one of the values from pdsm_client_type_e_type
   */
   uint16 w_ClientType;
} LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type;

#define TM_DELETE_PARAM_THROTTLE_TIMER_VAL 1000 /* for now throttling timer is 1 sec */

/*===========================================================================

FUNCTION  tm_pdapi_cmd_proc

DESCRIPTION
  Handles an incoming PDAPI cmd

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_pdapi_cmd_proc( void* p_data )
{
  pdsm_cmd_s_type   *p_cmd_type;
  pdsm_pd_ext_pos_s_type  z_pd_ext_pos_data;

  pdsm_pd_cmd_err_e_type          e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
  tm_pdapi_get_pos_param_s_type   *p_get_pos_type = &z_get_pos_param_type;
  sysd_cntrl_func_packet_type sysd_cntrl_payload;
  sysd_cntrl_payload.event = SYSD_CNTRL_MAX_EVTS;
  
  if (p_data == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_cmd_type = (pdsm_cmd_s_type*)p_data;


  MSG_LOW( "TM_PDAPI: tm_pdapi_cmd_proc() getting called",0,0,0);

  switch( p_cmd_type->cmd_type )
  {
    case PDSM_CMD_TYPE_PD:
      if(tm_core_is_reset_location_service_in_progress())
      {
        MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
        tm_pd_client_cmd_status( p_cmd_type, PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S );
        return;
      }

      switch (p_cmd_type->cmd.pd.cmd)
      {
        case PDSM_PD_CMD_GET_POS:
          /* handle request for Position Determination.
          */
          /* First find out if the command is O.K, check the parameters
          ** and the session state and other things.
          */
          p_get_pos_type->client_id = p_cmd_type->cmd.pd.client_id;
          p_get_pos_type->client_type =  pdsmclient_get_client_type_map(p_cmd_type->cmd.pd.client_id);
          /*
          p_get_pos_type->option_ptr = &p_cmd_type->cmd.pd.info.option_type;
          p_get_pos_type->qos_ptr = &p_cmd_type->cmd.pd.info.qos;
          */
          p_get_pos_type->option_ptr = &z_get_pos_param_options_type;
          p_get_pos_type->qos_ptr = &z_get_pos_params_qos_type;
          z_get_pos_param_options_type = p_cmd_type->cmd.pd.info.option_type;
          z_get_pos_params_qos_type = p_cmd_type->cmd.pd.info.qos;

          e_cmd_err = tm_core_get_pos_para_check(p_get_pos_type);
          if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
          {
            /* if ok to continue, do validity checks on the parameters */
            e_cmd_err = tm_pd_client_cmd_check( p_cmd_type );
            if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
            {
              if((PDSM_SESSION_OPERATION_STANDALONE_ONLY ==
                  p_cmd_type->cmd.pd.info.option_type.operation_mode) ||
                 (PDSM_SESSION_OPERATION_MSBASED ==
                   p_cmd_type->cmd.pd.info.option_type.operation_mode))
              {
                sysd_cntrl_payload.event = SYSD_CNTRL_EVT_GET_POS;
                tm_core_info.qwip_sysd_triggered = TRUE;
              }
            }
          }
          /* Indicate to client about the status of this command, irrespective of the
          ** fact whether it was success or failure. In case of failures in connecting
          ** to PDE, it will be indicated as an END_EVENT callback.
          */
          tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
          break;

        case PDSM_PD_CMD_END_SESSION:
    case PDSM_PD_CMD_END_SESSION_EX:  
          if(TRUE== tm_core_info.qwip_sysd_triggered )
          {
            sysd_cntrl_payload.event = SYSD_CNTRL_EVT_SESS_END;
          }
          break;

        case PDSM_PD_CMD_INJECT_POS:
          if(TRUE== tm_core_info.qwip_sysd_triggered )
          {
            sysd_cntrl_payload.event = SYSD_CNTRL_EVT_COARSE_POSN_INJ;
          }
          break;

        case PDSM_PD_CMD_RESET_LOCATION_SERVICE:          
          if(TRUE== tm_core_info.qwip_sysd_triggered )
          {
            sysd_cntrl_payload.event = SYSD_CNTRL_MAX_EVTS;
          }
          break;
        default:
           /* Will not be processed by Sysd*/
          sysd_cntrl_payload.event = SYSD_CNTRL_MAX_EVTS;
          MSG_HIGH("PDAPI cmd %d not to be proc by SYSD",
              p_cmd_type->cmd.pd.cmd,0,0);
          break;
      }

      if(sysd_cntrl_payload.event != SYSD_CNTRL_MAX_EVTS)
      {
        MSG_MED("PDAPI cmd (%d) for Sysd (evt %d)",
            p_cmd_type->cmd.pd.cmd,sysd_cntrl_payload.event,0);
        sysd_cntrl_payload.cmd.pdsm_cmd_type = p_cmd_type;
        sysd_cntrl_payload.req_type = SYSD_CNTRL_POS_REQ_TYPE_PDAPI;
        tm_sysd_controller_handler(&sysd_cntrl_payload);
      }
      else if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
      /* Store the client pointer in the command buffer.
      */
        p_cmd_type->cmd.pd.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.pd.client_id);

        /* Now process this command from Client. It can either be a PD session command ot
        ** request to terminate a session.
        */
        tm_pd_client_cmd_proc( p_cmd_type );
      }
      break;

    case PDSM_CMD_TYPE_PARAM:      
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.pa.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.pa.client_id);

      /* Now handle the command for parameter setting.
      */
      tm_pa_cmd_proc( p_cmd_type );
      break;

  #ifdef FEATURE_VECTORONE

    case PDSM_CMD_TYPE_OR:
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.or.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.or.client_id);

      /* Now handle the command for parameter setting.
      */
      or_client_cmd_proc( p_cmd_type );
      break;
  #endif

    case PDSM_CMD_TYPE_LCS:            
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.lcs.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.lcs.client_id);
      tm_lcs_client_cmd_proc( p_cmd_type);
      break;

    case PDSM_CMD_TYPE_EXT_POS:            
      p_cmd_type->cmd.ext_pos.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.lcs.client_id);
      if ( ( pdsmclient_valid_client_id( p_cmd_type->cmd.ext_pos.client_id )) == FALSE )
      {
        tm_ext_client_cmd_status( p_cmd_type, PDSM_EXT_POS_CMD_ERR_CLIENT_ID_P);
        MSG_ERROR("TM_PDAPI: PDSM_EXT_POS_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
      }
      else if(tm_core_is_reset_location_service_in_progress())
      {
        MSG_ERROR("TM_PDAPI: PDSM_EXT_POS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S",0,0,0);      
        tm_ext_client_cmd_status( p_cmd_type, PDSM_EXT_POS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S);
      }
      else
      {
        tm_ext_client_cmd_status( p_cmd_type, PDSM_EXT_POS_CMD_ERR_NOERR );
      }

      tm_pdapi_xlate_use_pos_to_pd_ext_pos(&z_pd_ext_pos_data, &p_cmd_type->cmd.ext_pos.info.pos_to_use);

      sm_log_diag_position((uint32)p_cmd_type->cmd.lcs.client_id,
                           &z_pd_ext_pos_data,
                           POS_FROM_OEM,
                           (boolean)FALSE, (boolean)FALSE);

      #ifdef FEATURE_POSITION_SHOPPING
      #error code not present
#endif
      break;

    case PDSM_CMD_TYPE_XTRA:            /* XTRA related commands */
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.xtra.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.xtra.client_id);

      /* Process the XTRA cmd from client
      */
      tm_xtra_client_cmd_proc( p_cmd_type );
      break;

  case PDSM_CMD_TYPE_XTRA_T:            /* XTRA-T related commands */
    /* Store the client pointer in the command buffer.
    */
    p_cmd_type->cmd.xtra_t.client = pdsmclient_get_client_ptr(p_cmd_type->cmd.xtra_t.client_id);

    /* Process the XTRA-T cmd from client
    */
    tm_xtra_t_client_cmd_proc( p_cmd_type );
    break;

  case PDSM_CMD_TYPE_WIPER:            /* WIPER related commands */
    /* Process the WIPER cmd from client
    */
    tm_wiper_client_cmd_proc( p_cmd_type );
    break;

  case PDSM_CMD_TYPE_XSPI:

    /* NOTE: We do NOT want to check if the client id matches who is
     * requesting fixes. Currently from QMI-LOC, SDP is injecting XSPI
     * while LocMW requests the fixes. The original check no longer
     * matches our new ULP based architecture. */

    /* delegate handling of task to specific function */
    tm_xspi_handle_status_injection(&(p_cmd_type->cmd.xspi));
    break;

#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif

  case PDSM_CMD_TYPE_MOTION:
    /* delegate handling of task to specific function */
    tm_motion_client_cmd_proc( p_cmd_type );
    break;

  case PDSM_CMD_TYPE_DPO_STATUS_RPT:
    {
      boolean b_ret_val;
      /* Enable/Disable the DPO status reporting from MGP */
      if(p_cmd_type->cmd.dpo_status_rpt.enable > 0 ) 
      {
        b_ret_val = tm_core_enable_DpoStatusReport(TRUE);
      }
      else
      {
        b_ret_val = tm_core_enable_DpoStatusReport(FALSE);
      }
     
      MSG_MED("PDSM_CMD_TYPE_DPO_STATUS_RPT: Enable: %d, ret_val: %d",
               p_cmd_type->cmd.dpo_status_rpt.enable,
               b_ret_val,
               0);
    }
    break;

    case PDSM_CMD_TYPE_WIFI_ATTACHMENT_STATUS_IND:
    {
        tm_core_handle_wifi_attachment_status_ind(&p_cmd_type->cmd.wifi_attachment_status_ind_info);
    }
    break;

    case PDSM_CMD_TYPE_EXT_CELL:
      /* delegate handling of task to specific function */
      tm_ext_cell_client_cmd_proc( p_cmd_type );
      break;

    case PDSM_CMD_TYPE_NI_MSG:
      /* delegate handling of task to specific function */
      tm_ni_msg_client_cmd_proc( p_cmd_type );
      break;

    case PDSM_CMD_TYPE_WIFI_ON_DEMAND:
      /* delegate handling of task to specific function */
      tm_wifi_on_demand_client_cmd_proc( p_cmd_type );
      break;

    case PDSM_CMD_TYPE_WIFI_ENABLED_STATUS:
      /* Handle the wifi enabled status */
      tm_core_handle_wifi_enabled_status_ind(p_cmd_type->cmd.wifi_enabled_status);
      break;

  default:
    /* There is nothing we can do with this command
     */
    MSG_ERROR(" TM_PDAPI: Client command not handled ",p_cmd_type->cmd_type,0,0);
    break;
  } /* End Switch statement */

}

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_CHECK

DESCRIPTION
  Checks for Command parameter errors and PDSM state.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

pdsm_pd_cmd_err_e_type tm_pd_client_cmd_check( pdsm_cmd_s_type *p_cmd_type )
{

  pdsm_pd_cmd_s_type       *p_pd_cmd_type;
  pdsm_pd_cmd_err_e_type   e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
  pdsm_pd_cmd_info_s_type  *p_cmd_info_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_PD_CMD_ERR_INVALID_PARAM;
  }

  p_pd_cmd_type = &p_cmd_type->cmd.pd;
  p_cmd_info_type = &p_pd_cmd_type->info;


  /* Check if command is already in error.
  */
  MSG_LOW( "TM_PDAPI: tm_pd_client_cmd_check() getting called",0,0,0);

  if ( p_cmd_info_type->cmd_err != PDSM_PD_CMD_ERR_NOERR )
  {
    return  p_cmd_info_type->cmd_err;
  }

  /* There is no error, proceeed to check for command errors.
  */
  switch( p_pd_cmd_type->cmd )
  {
    case PDSM_PD_CMD_GET_POS:
    {

      /* now assuming session is active check if Transport modes match the new session and old session
      */
      if ((p_pd_cmd_type->info.option_type.lsinfo.server_option < PDSM_SERVER_OPTION_USE_DEFAULT) ||
        (p_pd_cmd_type->info.option_type.lsinfo.server_option > PDSM_SERVER_OPTION_USE_LOCAL))
      {
        e_cmd_err = PDSM_PD_CMD_ERR_LSINFO_P;
      }
      if (p_pd_cmd_type->info.option_type.lsinfo.server_option  == PDSM_SERVER_OPTION_USE_LOCAL)
      {
        if (p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_IPV6_ADDRESS)
        {
          /* only ip v4 and URL currently supported */
          e_cmd_err = PDSM_PD_CMD_ERR_LSINFO_P;
        }

        /* Check if IP Addr or URL are valid */
        if((p_pd_cmd_type->info.option_type.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED) &&
           (((p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_IPV4_ADDRESS) &&
           ((p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs == 0) ||
           (p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id == 0))) ||
           ((p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_addr_type == PDSM_SERVER_TYPE_URL_ADDRESS) &&
           (p_pd_cmd_type->info.option_type.lsinfo.server_adrs.server_adrs.server_adrs_url.url[0] == '\0'))))
        {
          e_cmd_err = PDSM_PD_CMD_ERR_LSINFO_P;
        }
      }
      if ((p_pd_cmd_type->info.option_type.session <= PDSM_PD_SESS_TYPE_MIN) ||
          (p_pd_cmd_type->info.option_type.session >= PDSM_PD_SESS_TYPE_MAX))
      {
        e_cmd_err = PDSM_PD_CMD_ERR_SESS_TYPE_P;
      }
      if ((p_pd_cmd_type->info.option_type.operation_mode <= PDSM_SESSION_OPERATION_MIN) ||
          (p_pd_cmd_type->info.option_type.operation_mode >= PDSM_SESSION_OPERATION_MAX))
      {
        e_cmd_err = PDSM_PD_CMD_ERR_OPERATION_P;
      }
      if (p_pd_cmd_type->info.option_type.fix_rate.num_fixes < 1)
      {
        e_cmd_err = PDSM_PD_CMD_ERR_NUM_FIXES_P;
      }
      if ((p_pd_cmd_type->info.option_type.mode_info.mode <= PDSM_SESS_JGPS_TYPE_NONE) ||
          (p_pd_cmd_type->info.option_type.mode_info.mode >= PDSM_SESS_JGPS_TYPE_MAX))
      {
        e_cmd_err = PDSM_PD_CMD_ERR_INVALID_PARAM;
      }
    } /* End if para_check returned success */

    break;

    case PDSM_PD_CMD_END_SESSION:
  case PDSM_PD_CMD_END_SESSION_EX:  

    break;

  default:
    /* Error in switch statement.
    */
    e_cmd_err = PDSM_PD_CMD_ERR_UNKNOWN_CMD;
    MSG_ERROR("TM_PDAPI: Bad PD command %d",p_pd_cmd_type->cmd,0,0);
    break;

  }/* End of PD switch for different commmands. */

  return e_cmd_err;

}/* End of pd_client_cmd_check( ) */

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_STATUS

DESCRIPTION

  This function is used to notify clients of a specified  call command status.

RETURN VALUE

  None

DEPENDENCIES

===========================================================================*/

void tm_pd_client_cmd_status(
  pdsm_cmd_s_type        *p_cmd_type,
      /* Pointer to PDSM command */
  pdsm_pd_cmd_err_e_type e_cmd_err
     /* PDSM call command error */
)
{
  pdsm_pd_cmd_s_type  *p_pd_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_pd_cmd_type = &p_cmd_type->cmd.pd;

  MSG_LOW( "TM_PDAPI: tm_pd_client_cmd_status() getting called",0,0,0);

  /* Call the user supplied command callback.
  */
  if ( p_pd_cmd_type->cmd_cb_func != NULL )
  {
    p_pd_cmd_type->cmd_cb_func( p_pd_cmd_type->data_block_ptr,
      p_pd_cmd_type->cmd,
      e_cmd_err
      );
  } /* End if no callback is provided. */

  /* In case of errors notify client list.
  */
  if ( e_cmd_err != PDSM_PD_CMD_ERR_NOERR )
  {
    MSG_HIGH( "TM_PDAPI: pd cmd err, cmd = %d, err = %d, client = %d",
      p_pd_cmd_type->cmd, e_cmd_err, p_pd_cmd_type->client_id );

    pdsmclient_list_pd_cmd_err_ntfy( e_cmd_err, p_pd_cmd_type );
    sm_report_event_pd_cmd_err((uint8) PD_GET_POS, e_cmd_err);
    sm_log_pd_cmd_err((uint32)p_pd_cmd_type->client_id, p_pd_cmd_type->cmd, e_cmd_err);
  }
} /* End pd_client_cmd_err() */

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_PROC

DESCRIPTION
  This function is used handle client commands to PDSM.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_pd_client_cmd_proc(
  pdsm_cmd_s_type *p_cmd_type
          /* Pointer to PDSM command */
  )
{
  /* Here commands are handled which are passed from a FIFO queue.
  ** Don't worry about priority and QOS for Alpha release.
  */
  pdsm_pd_cmd_err_e_type          e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
  tm_pdapi_get_pos_param_s_type   *p_get_pos_type = &z_get_pos_param_type;
  pdsm_pd_info_s_type             *p_pd_info_type = &z_pd_info;
  tm_pdapi_end_session_param_s_type z_end_sess_type;
  boolean                         u_ret = TRUE;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_pd_client_cmd_proc() getting called",0,0,0);
  if ( ( pdsmclient_valid_client_id( p_cmd_type->cmd.pd.client_id )) == FALSE )
  {
    e_cmd_err = PDSM_PD_CMD_ERR_CLIENT_ID_P;
    MSG_ERROR("TM_PDAPI: PDSM_PD_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
    tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }


  /* Now proceed with the command as ewerything looks just fine. In case of
  ** connection failure we will indicate an Abort (error) event to clients.
  */
  switch ( p_cmd_type->cmd.pd.cmd )
  {
    case PDSM_PD_CMD_END_SESSION:
  case PDSM_PD_CMD_END_SESSION_EX:  
      /* Handle the session end command first.
      */
      sm_log_end_session(p_cmd_type->cmd.pd.client_id, p_cmd_type->cmd.pd.info.session_end_info.end_session_type);

      z_end_sess_type.client_id = p_cmd_type->cmd.pd.client_id;
      z_end_sess_type.session_type = p_cmd_type->cmd.pd.info.session_end_info.end_session_type;
    z_end_sess_type.receiver_off =  p_cmd_type->cmd.pd.info.session_end_info.receiver_off;
      e_cmd_err = tm_core_end_session_para_check(&z_end_sess_type);
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      cached_client_id = p_cmd_type->cmd.pd.client_id;

      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        if (p_cmd_type->cmd.pd.info.session_end_info.end_session_type == PDSM_PD_END_SESSION_FIX_REQ ||
            p_cmd_type->cmd.pd.info.session_end_info.end_session_type == PDSM_PD_END_SESSION_DLOAD_REQ )
        {
          (void)tm_core_handle_end_session( &z_end_sess_type );  /* Handle the end session command */
        }
      }
      break;

    case PDSM_PD_CMD_GET_POS:

      sm_log_get_position_request(p_cmd_type);
      cached_client_id = p_cmd_type->cmd.pd.client_id;

      p_get_pos_type->client_id = p_cmd_type->cmd.pd.client_id;
      p_get_pos_type->client_type =  pdsmclient_get_client_type_map(p_cmd_type->cmd.pd.client_id);
      p_get_pos_type->option_ptr = &p_cmd_type->cmd.pd.info.option_type;
      p_get_pos_type->qos_ptr = &p_cmd_type->cmd.pd.info.qos;

      /* handle request for Position Determination.
      */
      /* First find out if the command is O.K, check the parameters
      ** and the session state and other things.
      */
      e_cmd_err = tm_core_get_pos_para_check(p_get_pos_type);
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        /* if ok to continue, do validity checks on the parameters */
        e_cmd_err = tm_pd_client_cmd_check( p_cmd_type );
      }
      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure. In case of failures in connecting
      ** to PDE, it will be indicated as an END_EVENT callback.
      */

      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );

      /* In case of error we need to return from here.
      */
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        p_pd_info_type->client_id = p_cmd_type->cmd.pd.client_id;
        p_get_pos_type->internal_session = FALSE;

        u_ret = tm_core_handle_get_pos(p_get_pos_type);
      }

      if (!u_ret )
      {
        MSG_LOW("TM_PDAPI: TM core returned error for handle get pos", 0,0,0);
      }
      break;

    case PDSM_PD_CMD_INJECT_TIME:
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      (void)tm_core_handle_inject_ext_time(&p_cmd_type->cmd.pd.info.ext_time_info);
      break;

    case PDSM_PD_CMD_INJECT_POS:
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      (void)tm_core_handle_inject_ext_pos(p_cmd_type);
      break;

    case PDSM_PD_CMD_GET_GPS_STATE:
      sm_log_get_gps_state_info(p_cmd_type->cmd.pd.client_id);
      e_cmd_err = tm_core_get_gps_state_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        (void)tm_core_handle_get_gps_state(p_cmd_type);
      }
      break;
    case PDSM_PD_CMD_RESET_LOCATION_SERVICE:
      {
      pdsm_client_type_e_type           client_type;      
      tm_cm_phone_state_info_s_type phone_state_info;

      e_cmd_err = PDSM_PD_CMD_ERR_NOERR;
      
      tm_cm_iface_get_phone_state(&phone_state_info);

      client_type = pdsmclient_get_client_type_map(p_cmd_type->cmd.pd.client_id);
      
      if(client_type != PDSM_CLIENT_TYPE_OEM)
      {
        MSG_ERROR("Reset Location Service: Client type not OEM",0,0,0);
        e_cmd_err = PDSM_PD_CMD_ERR_CLIENT_ID_P;
      }
      else if(phone_state_info.e911_state != TM_CM_E911_STATE_NONE)
      {        
        MSG_ERROR("Reset Location Service: Emergency Call is progress",0,0,0);
        e_cmd_err = PDSM_PD_CMD_EMERGENCY_CALL_IN_PROGRESS_S;        
      }

      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {        
        cached_client_id = p_cmd_type->cmd.pd.client_id;
        (void)tm_core_handle_reset_location_service();
      }
      }
      break;

    case PDSM_PD_CMD_GET_BEST_AVAIL_POS:
      /* Request to get the best available position from the engine */
      MSG_HIGH("PDAPI cmd for best avail position", 0, 0, 0);
      sm_log_get_best_avail_pos_info(p_cmd_type->cmd.pd.client_id);
      e_cmd_err = tm_core_get_best_avail_pos_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        tm_core_get_best_avail_pos_param_s_type get_best_avail_pos_param;

        get_best_avail_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI;
        get_best_avail_pos_param.cmd.p_cmd_type = p_cmd_type;
        
        (void)tm_core_handle_best_avail_pos(&get_best_avail_pos_param);
      }
      break;

    case PDSM_PD_CMD_GET_AVAIL_WWAN_POS:
      /* Request to get the available WWAN position from the engine */
      MSG_HIGH("PDAPI cmd for avail WWAN position", 0, 0, 0);
      /* Currently no plans to generate a DM log message for Available WWAN Position */
      e_cmd_err = tm_core_get_best_avail_pos_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_PD_CMD_ERR_NOERR)
      {
        tm_core_get_avail_wwan_pos_param_s_type get_avail_wwan_pos_param;

        get_avail_wwan_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI;
        get_avail_wwan_pos_param.cmd.p_cmd_type = p_cmd_type;
        
        (void)tm_core_handle_avail_wwan_pos(&get_avail_wwan_pos_param);
      }
      break;
  case PDSM_PD_CMD_GET_AVAIL_POS_FOR_SRC:
    {
      /* Request to get the available position from the engine based on Source */
      MSG_HIGH("PDAPI cmd to get avail position estimate", 0, 0, 0);
      /* Currently no plans to generate a DM log message for Available GNSS Position */
      e_cmd_err = tm_core_get_best_avail_pos_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (PDSM_PD_CMD_ERR_NOERR == e_cmd_err)
      {
        tm_core_get_best_avail_pos_param_s_type z_get_available_pos_param = {0};
        uint32 q_get_available_pos_src = 0;
        q_get_available_pos_src = *((uint32 *)p_cmd_type->cmd.pd.data_block_ptr);
        // Check the Source of Position Estimate request and assign the source value accordingly
        if(SM_LOC_EST_SRC_GPS == q_get_available_pos_src)
        {
          z_get_available_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI;
        }
        else
        {
          MSG_HIGH("PDAPIcmd DataSource %d not supported. Defaulting to GNSS", q_get_available_pos_src, 0, 0);
          z_get_available_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI;
        }
        z_get_available_pos_param.cmd.p_cmd_type = p_cmd_type;
        (void)tm_core_handle_best_avail_pos(&z_get_available_pos_param);
      }
      break;
    }
  default:
      MSG_ERROR("TM_PDAPI: command not suported by pd_client_cmd_proc()",p_cmd_type->cmd.pd.cmd,0,0);
      break;

  }

  return;
} /* End pd_client_cmd_proc() */


/* ========================================================================

FUNCTION TM_PA_CMD_GET_PARAM_PARA_CHECK

DESCRIPTION
  Checks for parameter errors.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_pa_cmd_err_e_type tm_pa_cmd_get_param_para_check( pdsm_pa_cmd_s_type   *p_PaCmdType )
{
  pdsm_pa_cmd_err_e_type      e_cmd_err = PDSM_PA_CMD_ERR_NOERR;

  /* check if this is a valid client (ID)handle.
  */

  if (p_PaCmdType == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_PA_CMD_ERR_PARAM_TYPE_P;
  }
  MSG_LOW( "TM_PDAPI: tm_pa_cmd_get_param_para_check() getting called",0,0,0);

  if ( ( pdsmclient_valid_client_id( p_PaCmdType->client_id )) == FALSE )
  {
    e_cmd_err = PDSM_PA_CMD_ERR_CLIENT_ID_P;
    MSG_ERROR("TM_PDAPI: PDSM_PA_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
  }
  if ( (p_PaCmdType->info.set_type <= PDSM_PA_NONE) ||
       (p_PaCmdType->info.set_type >= PDSM_PA_MAX))
  {
    /* App is not currently supported.
    */
    e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
  }
  return e_cmd_err;
} /* End pa_cmd_get_param_para_check() */

/* ========================================================================

FUNCTION TM_PA_CMD_DELETE_PARAM_PARA_CHECK

DESCRIPTION
  Helper function to massage the parameters of pdsm_set_parameters called with
  cmd type set to PDSM_PA_DELETE_PARAMS_EX or deprecated PDSM_PA_DELETE_PARAMS.

RETURN VALUE
  PDSM_PA_CMD_ERR_NOERR:         If no errors
  PDSM_PA_CMD_ERR_DELETE_PARM_P: In case parameter deletion cant continue

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_pa_cmd_err_e_type tm_pa_cmd_delete_param_para_check( pdsm_delete_parms_type   *parms )
{
  pdsm_pa_cmd_err_e_type      e_cmd_err = PDSM_PA_CMD_ERR_NOERR;

  /*Accept the delete param command if MGP is in OFF state or check if on-demand LPM is running */
  /* do not accept delete if the on-demand READY mode is ON */
  boolean delete_parms = FALSE;
  uint32 q_throttle_time_remaining =0;

  if(NULL == parms)
  {
    MSG_ERROR("Null Parms in Delete Param Check",0,0,0);
    return PDSM_PA_CMD_ERR_NOT_ALLOWED;
  }
  
  if (tm_core_info.e_cached_rcvr_state == C_GPS_RCVR_STATE_OFF)
  {
    delete_parms = TRUE;
  }
  else if (tm_core_get_on_demand_ready_state() == TRUE)
  {
    delete_parms = FALSE;
  }
  else if (tm_core_get_on_demand_state() == TRUE)
  {
    delete_parms = TRUE;
  }
  else if (tm_core_is_internal_session_on() == TRUE)
  {
    delete_parms = TRUE;
  }
  else if(FALSE == tm_core_session_active())
  {
    /*Engine is ON but there is no TM session. Example: LM apptrack logic.
      Allow Delete params to go through*/
    delete_parms = TRUE;
  }

  q_throttle_time_remaining = os_TimerCheck(tm_core_info.tm_delete_param_throttle_timer); 
  if (q_throttle_time_remaining > 0)
  {
    delete_parms = FALSE;
    MSG_MED("Delete all throttle timer running. remaining timeout %d", q_throttle_time_remaining,0,0);

  }

  if (delete_parms == TRUE)
  {
    if ((parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_TIME) &&
        ((parms->pdsm_delete_parms_flags & PDSM_PA_TIME_OFFSET) ||
         (parms->pdsm_delete_parms_flags & PDSM_PA_TIME_UNC)))
    {
      /* if both delete time and inject offset or time unc are set, just honor the delete time */
      parms->pdsm_delete_parms_flags &=  ~(PDSM_PA_TIME_OFFSET); 
      parms->pdsm_delete_parms_flags &=  ~(PDSM_PA_TIME_UNC); 
    }

    if ((parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_POS) &&
        ((parms->pdsm_delete_parms_flags & PDSM_PA_POS_OFFSET)||
         (parms->pdsm_delete_parms_flags & PDSM_PA_POS_UNC)))
    {
      /* if both delete time and inject offset are set, just honor the delete time */
      parms->pdsm_delete_parms_flags &=  ~(PDSM_PA_POS_OFFSET); 
      parms->pdsm_delete_parms_flags &=  ~(PDSM_PA_POS_UNC); 
    }

    /* if ephemeris is to be deleted and no SV is marked, mark all the SVs from that
       constellation to be deleted */
    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH )
    {
      parms->eph_sv_mask      = (parms->eph_sv_mask == 0)      ? 0xffffffff : parms->eph_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_GLO )
    {
      parms->glo_eph_sv_mask  = (parms->glo_eph_sv_mask == 0)  ? 0xffffffff : parms->glo_eph_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_SBAS )
    {
      parms->sbas_eph_sv_mask = (parms->sbas_eph_sv_mask == 0) ? 0xffffffff : parms->sbas_eph_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_BDS )
    {
      parms->bds_eph_sv_mask = (parms->bds_eph_sv_mask == 0) ? 0x1fffffffffULL : parms->bds_eph_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_QZSS )
    {
      parms->qzss_eph_sv_mask  = (parms->qzss_eph_sv_mask == 0)  ? 0xffffffff : parms->qzss_eph_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_EPH_GAL )
    {
      parms->gal_eph_sv_mask = (parms->gal_eph_sv_mask == 0) ? 0xfffffffffULL : parms->gal_eph_sv_mask;
    }

    /* if almanac is to be deleted and no SV is marked, mark all the SVs to be deleted */
    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM )
    {
      parms->alm_sv_mask      = (parms->alm_sv_mask == 0)      ? 0xffffffff : parms->alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_GLO )
    {
      parms->glo_alm_sv_mask  = (parms->glo_alm_sv_mask == 0)  ? 0xffffffff : parms->glo_alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_SBAS )
    {
      parms->sbas_alm_sv_mask = (parms->sbas_alm_sv_mask == 0) ? 0xffffffff : parms->sbas_alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_BDS )
    {
      parms->bds_alm_sv_mask = (parms->bds_alm_sv_mask == 0) ? 0x1fffffffffULL : parms->bds_alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_QZSS )
    {
      parms->qzss_alm_sv_mask  = (parms->qzss_alm_sv_mask == 0)  ? 0xffffffff : parms->qzss_alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_ALM_GAL )
    {
      parms->gal_alm_sv_mask = (parms->gal_alm_sv_mask == 0) ? 0xfffffffffULL : parms->gal_alm_sv_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GPS )
    {
      parms->gps_gnss_sv_blacklist_mask  = (parms->gps_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->gps_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GLO )
    {
      parms->glo_gnss_sv_blacklist_mask  = (parms->glo_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->glo_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_BDS )
    {
      parms->bds_gnss_sv_blacklist_mask  = (parms->bds_gnss_sv_blacklist_mask == 0)  ? 0x1fffffffffULL : parms->bds_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_QZSS )
    {
      parms->qzss_gnss_sv_blacklist_mask  = (parms->qzss_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->qzss_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_delete_parms_flags & PDSM_PA_DELETE_GNSS_SV_BLACKLIST_GAL )
    {
      parms->gal_gnss_sv_blacklist_mask  = (parms->gal_gnss_sv_blacklist_mask == 0)  ? 0xfffffffffULL : parms->gal_gnss_sv_blacklist_mask;
    }
    (void)os_TimerStart(tm_core_info.tm_delete_param_throttle_timer,
                        TM_DELETE_PARAM_THROTTLE_TIMER_VAL, 
                        NULL);

  }
  else
  {
    MSG_ERROR("TM_PDAPI: MGP engine state %d,throttle timer %d. Cannot delete aiding data", 
                tm_core_info.e_cached_rcvr_state,  q_throttle_time_remaining,0);
    e_cmd_err = PDSM_PA_CMD_ERR_NOT_ALLOWED;
  }
  return e_cmd_err;
}
/* ========================================================================

FUNCTION TM_PA_CMD_SET_PARAM_PARA_CHECK

DESCRIPTION
  Checks for parameter errors.

RETURN VALUE
  pdsm_pa_cmd_err_e_type

DEPENDENCIES
  Called by registered clients.

===========================================================================*/


static pdsm_pa_cmd_err_e_type tm_pa_cmd_set_param_para_check( pdsm_pa_cmd_s_type   *p_pa_cmd_type )
{
  pdsm_pa_cmd_err_e_type      e_cmd_err = PDSM_PA_CMD_ERR_NOERR;

  /* check if this is a valid client (ID)handle.
  */

  if (p_pa_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_PA_CMD_ERR_PARAM_TYPE_P;
  }

  MSG_LOW( "TM_PDAPI: tm_pa_cmd_set_param_para_check() getting called",0,0,0);

  if ( (p_pa_cmd_type->info.set_type <= PDSM_PA_NONE) || (p_pa_cmd_type->info.set_type >= PDSM_PA_MAX) )
  {
    e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
  }
  switch( p_pa_cmd_type->info.set_type)
  {
    case PDSM_PA_1X_PDE_NETWORK_ADRS:
    case PDSM_PA_1X_MPC_NETWORK_ADRS:
    case PDSM_PA_UMTS_PDE_NETWORK_ADRS:
      /* Adress of the network entity, can be only URL or IPV4 right now */
      if ((p_pa_cmd_type->info.pa_set.server_address.server_addr_type != PDSM_SERVER_TYPE_URL_ADDRESS) &&
          (p_pa_cmd_type->info.pa_set.server_address.server_addr_type != PDSM_SERVER_TYPE_IPV4_ADDRESS))
      {
        /* only URL and IPV4 are supported at this time */
        e_cmd_err = PDSM_PA_CMD_ERR_NETWORK_ADDRS_P;
      }
      break;
    case PDSM_PA_GPS_LOCK:           /* GPS LOCK */
      if ((p_pa_cmd_type->info.pa_set.gps_lock < PDSM_GPS_LOCK_NONE) ||
          (p_pa_cmd_type->info.pa_set.gps_lock > PDSM_GPS_LOCK_ALL))
      {
        e_cmd_err = PDSM_PA_CMD_ERR_GPS_LOCK_P;
      }
      break;
    case PDSM_PA_KEEP_WARM:          /* Enable/Disable Keep WARM */
      break;

    case PDSM_PA_DELETE_PARAMS: /* DEPRECATED. use PDSM_PA_DELETE_PARAMS_EX instead. Intentional cascade */
    case PDSM_PA_DELETE_PARAMS_EX: /* Delete GPS/ GNSS related parameters from the database */
    case PDSM_PA_DELETE_PARAMS_EX1: /* Delete GPS/ GNSS related parameters from the database */
      e_cmd_err = tm_pa_cmd_delete_param_para_check(&p_pa_cmd_type->info.pa_set.delete_params);
      break;

    case PDSM_PA_POSITION_MODE: /* Position modes that are supported on the mobile */
      break;

    case PDSM_PA_MT_LR_SUPPORT: /* Specifies if MT-LR is supported */
      if ((p_pa_cmd_type->info.pa_set.mt_lr_support > PDSM_MT_LR_MAX_VALUE)) 
      {
        e_cmd_err = PDSM_PA_CMD_ERR_MT_LR_P;
      }  
      break;
    case PDSM_PA_MO_METHOD: /* specifies if a CP or UP protocol is used for MO */
      if ((p_pa_cmd_type->info.pa_set.mo_method != MO_CP) &&
          (p_pa_cmd_type->info.pa_set.mo_method != MO_UP))
      {
        e_cmd_err = PDSM_PA_CMD_ERR_MO_METHOD_P;
      }
      break;
    case PDSM_PA_NMEA_SENTENCE_TYPE: /* specifies the NEMA sentence type to be reported to PDSM */
      if (p_pa_cmd_type->info.pa_set.nmea_sentence_type > ((PDAPI_NMEA_MAX_VALUE)))
      {
        e_cmd_err = PDSM_PA_CMD_ERR_NMEA_SENT_P;
      }
      break;

    case PDSM_PA_APP_INFO:
      if (p_pa_cmd_type->info.pa_set.app_info.url == NULL
          || p_pa_cmd_type->info.pa_set.app_info.ver !=  TM_PDAPI_JGPS_PROT_VER1
          || p_pa_cmd_type->info.pa_set.app_info.datum > TM_PDAPI_JGPS_DATUM_TOYKYO
          || p_pa_cmd_type->info.pa_set.app_info.unit >  TM_PDAPI_JGPS_UNIT_DEGREE
          || p_pa_cmd_type->info.pa_set.app_info.acry >  TM_PDAPI_JGPS_ACRY_LOW
          || p_pa_cmd_type->info.pa_set.app_info.num  >  TM_PDAPI_JGPS_NUM_0
          )
      {
        e_cmd_err = PDSM_PA_CMD_ERR_APP_INFO_P;
      }
      break;

    case PDSM_PA_PDAPI_ENABLE:
      break;

    case PDSM_PA_SEC_UPDATE_RATE:
      break;

    case PDSM_PA_SSD_DATA:
      if ( (p_pa_cmd_type->info.pa_set.ssd_info.ssd_length == 0) ||
           (p_pa_cmd_type->info.pa_set.ssd_info.ssd_length > PDSM_MAX_SSD_LEN) )
      {
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
    break;

    case PDSM_PA_NMEA_CONFIG_INFO:
      if (p_pa_cmd_type->info.pa_set.nmea_config_info.pd_nmea_reporting_type  > PDSM_PA_NMEA_REPORT_AT_FINAL_POSITION)
      {
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      if (p_pa_cmd_type->info.pa_set.nmea_config_info.pd_nmea_port_type >= PDSM_PA_NMEA_PORT_TYPE_MAX )
      {
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }

      break;
    case PDSM_PA_EFS_DATA:
      /* Check that the file name is not NULL */
      if (p_pa_cmd_type->info.pa_set.efs_data.efs_data_filename == NULL)
      {
        MSG_HIGH("TM_PDAPI: PDSM_PA_EFS_DATA: Null file name", 0,0,0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      /* Check that data is passed to us, if client is attempting to write */
      else if (((p_pa_cmd_type->info.pa_set.efs_data.efs_data_file_operation == PDSM_FS_CREATE_WRITE_FILE) || 
                (p_pa_cmd_type->info.pa_set.efs_data.efs_data_file_operation == PDSM_FS_APPEND_FILE)) &&
                (p_pa_cmd_type->info.pa_set.efs_data.efs_data_ptr == NULL))
      {
        MSG_HIGH("TM_PDAPI: PDSM_PA_EFS_DATA: NULL data ptr for write cmd", 0,0,0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      /* validate rest of the file attributes including the file name */
      else if (tm_util_efs_dir_and_file_name_is_valid(p_pa_cmd_type->info.pa_set.efs_data.efs_data_filename,
                                                      p_pa_cmd_type->info.pa_set.efs_data.efs_data_file_operation,
                                                      p_pa_cmd_type->info.pa_set.efs_data.efs_data_len) == FALSE)
      {
        MSG_HIGH("TM_PDAPI: PDSM_PA_EFS_DATA: Invalid directory file name,  file_operation %d, file len %d",
          p_pa_cmd_type->info.pa_set.efs_data.efs_data_file_operation,
          p_pa_cmd_type->info.pa_set.efs_data.efs_data_len, 0);
        /* MSG_HIGH needs a constant buffer to be passed in */
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
    break;
    case PDSM_PA_SBAS_USER_PREFERENCE:  /* Enable/disable SBAS user preference */
      break;

    case PDSM_PA_DPO_CONTROL:
      break;

    case PDSM_PA_ON_DEMAND_LOW_POWER_MODE:
    case PDSM_PA_ON_DEMAND_READY_MODE:
      break;

    case PDSM_PA_CALL_FLOW_SELECT:
      break;

    case PDSM_PA_EXTERNAL_SPI:
       switch(p_pa_cmd_type->info.pa_set.external_spi.mode)
       {
       case PDSM_PA_EXTERNAL_SPI_MODE_DISABLE: /* intentional fall through cascade */
       case PDSM_PA_EXTERNAL_SPI_MODE_ENABLE:
          /* add other allowed values here */
          /* do nothing */
          break;

          /* anything else is an error */
       default:
          e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
          break;
       }

       break; /* end case PDSM_PA_EXTERNAL_SPI */

#ifdef FEATURE_CGPS_XTRA_T
     case PDSM_PA_XTRAT_CLIENT_TOKEN:
      // check that the value is within the range 0-0xFFFFFFFF
      if(p_pa_cmd_type->info.pa_set.client_cookie > ((XTRAT_CLIENT_COOKIE_RANGE)))
      {
        MSG_HIGH("TM_PDAPI: PDSM_PA_XTRAT_CLIENT_TOKEN: parameter out of range (0-0xFFFFFFFF)", 0,0,0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_PA_XTRAT_USER_SESSION_CONTROL:
      /* do nothing here. always allow it to pass without errors */
      break;
#endif /* FEATURE_CGPS_XTRA_T */

    case PDSM_PA_DATA_ENABLE:
      /* do nothing here. always allow it to pass without errors */
      break;
    case PDSM_PA_LBS_APN_PROFILE:
    case PDSM_PA_XTRA_APN_PROFILE:
  case PDSM_PA_EMERGENCY_LBS_APN_PROFILE: 
        {
          int profile_index = 0;

          for(;profile_index < PDSM_PA_MAX_APN_PROFILES; profile_index++)
          {
            if(p_pa_cmd_type->info.pa_set.apn_profiles[profile_index].pdp_type > PDSM_PA_APN_PROFILE_PDN_TYPE_MAX)
            {
              e_cmd_err = PDSM_PA_CMD_ERR_PDP_TYPE_P;
              MSG_HIGH("Invalid PDN type",0,0,0);
              break;
            }
            if(p_pa_cmd_type->info.pa_set.apn_profiles[profile_index].srv_system_type > PDSM_PA_APN_PROFILE_SRV_SYS_MAX)
            {
              e_cmd_err = PDSM_PA_CMD_ERR_SRV_SYS_P;
              MSG_HIGH("Invalid SRV SYS type",0,0,0);
              break;
            }
          }
        }

        break;

    case PDSM_PA_PE_CONFIG_PARAM:
      if (p_pa_cmd_type->info.pa_set.pe_config_info.peConfigMask == 0) // config nothing?
      {
        MSG_HIGH("Invalid PE config mask 0x%x",p_pa_cmd_type->info.pa_set.pe_config_info.peConfigMask,0,0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_PA_SUPL_VERSION:
    case PDSM_PA_SUPL_SECURITY:
        /* do nothing here. always allow it to pass without errors */ // FIXME, should we check range here?
        break;
    case PDSM_PA_LPP_CONFIG_INFO:
    case PDSM_PA_ASST_GLONASS_POS_PROTOCOL:
      break;
    case PDSM_PA_SUPL_IS801:
    case PDSM_PA_SUPL_TLS_VERSION:
    case PDSM_PA_SUPL_HASH_ALGORITHM:
    case PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG:
    case PDSM_PA_WIFI_SCAN_TIMEOUT_SEC:
       break;

    case PDSM_PA_PREMIUM_SERVICES_CONFIG:
      if ( (p_pa_cmd_type->info.pa_set.premium_svc_cfg.service >= PDSM_PA_PREMIUM_SVC_MAX) ||
           (p_pa_cmd_type->info.pa_set.premium_svc_cfg.config  >= PDSM_PA_PREMIUM_SVC_CFG_MAX) )
      {
        MSG_ERROR("Invalid Premium Service type=%d", p_pa_cmd_type->info.pa_set.premium_svc_cfg.service, 0, 0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_GNSS_CONSTELL_REPORT_CONFIG:
      /* Verify if NV state allows dispatching of measurement report.
         If both measReport configuration & and SV poly are sent as invalid send error in parameter*/
      if ((1 != tm_core_info.config_info.gnss_meas_rep_qmi_out_control) ||
          ((!p_pa_cmd_type->info.pa_set.z_set_gnss_config.meas_report_config_valid) &&
           (!p_pa_cmd_type->info.pa_set.z_set_gnss_config.sv_poly_report_config_valid)))
      {
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_PA_XTRA_VERSION_CHECK:
      if (p_pa_cmd_type->info.pa_set.xtra_version_check >= PDSM_PA_XTRA_VERSION_CHECK_MAX)
      {
        MSG_ERROR("Invalid XTRA Version Check type=%d", p_pa_cmd_type->info.pa_set.xtra_version_check, 0, 0);
        e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    default:
    e_cmd_err = PDSM_PA_CMD_ERR_PARAM_TYPE_P;
    break;
  }
  return e_cmd_err;
}

/*===========================================================================

FUNCTION TM_PA_CLIENT_CMD_CHECK

DESCRIPTION
  Checks for Position Determination Command parameter errors and whether command
  is allowed in this State or not.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_pa_cmd_err_e_type tm_pa_client_cmd_check( pdsm_cmd_s_type *p_cmd_type )
{
  pdsm_pa_cmd_s_type      *p_pa_cmd_type;
  pdsm_pa_cmd_info_s_type *p_cmd_info_type;
  pdsm_pa_cmd_err_e_type   e_cmd_err = PDSM_PA_CMD_ERR_NOERR;
  /* Call this macro to obtain Sesion tage pointer */

  /* Check if command is already in error.
  */
  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_PA_CMD_ERR_PARAM_TYPE_P;
  }

  p_pa_cmd_type = &p_cmd_type->cmd.pa;
  p_cmd_info_type = &p_pa_cmd_type->info;

  MSG_LOW( "TM_PDAPI: tm_pa_client_cmd_check () getting called",0,0,0);

  if ( p_cmd_info_type->cmd_err != PDSM_PA_CMD_ERR_NOERR )
  {
    return  p_cmd_info_type->cmd_err;
  }

  /* There is no error, proceeed to check for command errors.
  */
  switch( p_pa_cmd_type->cmd )
  {
    case PDSM_PA_CMD_GET_PARAM:
      if ( (e_cmd_err = tm_pa_cmd_get_param_para_check( p_pa_cmd_type )) != PDSM_PA_CMD_ERR_NOERR )
      {
        return e_cmd_err;
      }
      break;

    case PDSM_PA_CMD_SET_PARAM:  /* Same parameter checking for get_param() and set_param() */
      /* Write a function which checks if pa_set ( add field in session_tag )
      ** is already set ( indicating param set in progress).
      */
      if ( (e_cmd_err = tm_pa_cmd_set_param_para_check( p_pa_cmd_type )) != PDSM_PA_CMD_ERR_NOERR)
      {
        return e_cmd_err;
      }
      break;

    default:
    /* Error in switch statement.
      */
      e_cmd_err = PDSM_PA_CMD_ERR_UNKNOWN_CMD;
      MSG_ERROR("TM_PDAPI: Bad PD command %d",p_pa_cmd_type->cmd,0,0);
      return e_cmd_err;

  } /* End of PD switch for different commmands. */



  return e_cmd_err;

}/* End of pa_client_cmd_check( ) */

/*===========================================================================

FUNCTION TM_PA_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_pa_client_cmd_status(
  pdsm_cmd_s_type  *p_cmd_type,
  pdsm_pa_cmd_err_e_type e_cmd_err
)
{
  pdsm_pa_cmd_s_type  *p_pa_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  p_pa_cmd_type = &p_cmd_type->cmd.pa;

  /* Call the user supplied command callback.
  */
  MSG_LOW( "TM_PDAPI: tm_pa_cmd_status () getting called",0,0,0);

  if ( p_pa_cmd_type->cmd_cb_func != NULL )
  {
    p_pa_cmd_type->cmd_cb_func( p_pa_cmd_type->data_block_ptr,
                               p_pa_cmd_type->cmd,
                               e_cmd_err
                             );
  } /* End if no callback is provided. */

  if ( e_cmd_err != PDSM_PA_CMD_ERR_NOERR )
  {
    MSG_ERROR( " pa cmd err, cmd = %d, err = %d, client = %d",
        p_pa_cmd_type->cmd, e_cmd_err, p_pa_cmd_type->client_id );
    pdsmclient_list_pa_cmd_err_ntfy( e_cmd_err, p_pa_cmd_type );
    sm_report_event_pa_cmd_err((uint8)p_pa_cmd_type->cmd, e_cmd_err);
    sm_log_pa_cmd_err((uint32)p_pa_cmd_type->client_id, p_pa_cmd_type->cmd, e_cmd_err);
  }
} /* End pd_client_cmd_err() */

/*===========================================================================

FUNCTION TM_LCS_CLIENT_CMD_STATUS
DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_lcs_client_cmd_status(
  pdsm_cmd_s_type  *p_cmd_type,
  pdsm_lcs_cmd_err_e_type e_cmd_err
)
{
  pdsm_lcs_cmd_s_type  *p_lcs_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  p_lcs_cmd_type = &p_cmd_type->cmd.lcs;

  /* Call the user supplied command callback.
  */
  MSG_LOW( "TM_PDAPI: tm_lcs_cmd_status () getting called",0,0,0);

  if ( p_lcs_cmd_type->cmd_cb_func != NULL )
  {
    p_lcs_cmd_type->cmd_cb_func( p_lcs_cmd_type->data_block_ptr,
                               p_lcs_cmd_type->cmd,
                               e_cmd_err
                             );
  } /* End if no callback is provided. */

  if ( e_cmd_err != PDSM_LCS_CMD_ERR_NOERR )
  {
    MSG_ERROR( " LCS cmd err, cmd = %d, err = %d, client = %d",
        p_lcs_cmd_type->cmd, e_cmd_err, p_lcs_cmd_type->client_id );
    pdsmclient_list_lcs_cmd_err_ntfy( e_cmd_err, p_lcs_cmd_type );
  }
} /* End tm_lcs_client_cmd_status() */

/*===========================================================================

FUNCTION TM_EXT_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_ext_client_cmd_status(
  pdsm_cmd_s_type  *p_cmd_type,
  pdsm_ext_pos_cmd_err_e_type e_cmd_err
)
{
  pdsm_ext_pos_cmd_s_type  *p_ext_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  p_ext_cmd_type = &p_cmd_type->cmd.ext_pos;

  /* Call the user supplied command callback.
  */
  MSG_LOW( "TM_PDAPI: tm_lcs_cmd_status () getting called",0,0,0);

  if ( p_ext_cmd_type->cmd_cb_func != NULL )
  {
    p_ext_cmd_type->cmd_cb_func( p_ext_cmd_type->data_block_ptr,
                               p_ext_cmd_type->cmd,
                               e_cmd_err
                             );
  } /* End if no callback is provided. */

  if ( e_cmd_err != PDSM_EXT_POS_CMD_ERR_NOERR )
  {
    MSG_ERROR( " EXT POS cmd err, cmd = %d, err = %d, client = %d",
        p_ext_cmd_type->cmd, e_cmd_err, p_ext_cmd_type->client_id );
    pdsmclient_list_ext_status_cmd_err_ntfy( e_cmd_err, p_ext_cmd_type );
  }
} /* End tm_lcs_client_cmd_status() */


/*===========================================================================

FUNCTION TM_XTRA_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_xtra_client_cmd_status(
  pdsm_cmd_s_type           *p_cmd_type,
  pdsm_xtra_cmd_err_e_type  e_cmd_err
)
{
  pdsm_xtra_cmd_s_type  *p_xtra_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_xtra_cmd_type = &p_cmd_type->cmd.xtra;

  MSG_LOW( "TM_PDAPI: tm_xtra_cmd_status () getting called",0,0,0);

  /* Only one XTRA client registered at a time */
  if ( e_cmd_err != PDSM_XTRA_CMD_ERR_NOERR )
  {
    MSG_ERROR( " XTRA cmd err, cmd = %d, err = %d, client = %d",
        p_xtra_cmd_type->cmd, e_cmd_err, p_xtra_cmd_type->client_id );
  }

  /* Call the user supplied command callback */
  if ( p_xtra_cmd_type->cmd_cb_func != NULL )
  {
    p_xtra_cmd_type->cmd_cb_func( p_xtra_cmd_type->data_block_ptr,
                                  p_xtra_cmd_type->cmd,
                                  e_cmd_err
                                );
  } /* End if no callback is provided. */


} /* End tm_xtra_client_cmd_status() */


/*===========================================================================

FUNCTION TM_XTRA_T_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_xtra_t_client_cmd_status(
  pdsm_cmd_s_type            *p_cmd_type,
  pdsm_xtra_t_cmd_err_e_type  e_cmd_err
)
{
  pdsm_xtra_t_cmd_s_type  *p_xtra_t_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_xtra_t_cmd_type = &p_cmd_type->cmd.xtra_t;

  MSG_LOW( "TM_PDAPI: tm_xtra_t_client_cmd_status () getting called",0,0,0);

  /* Only one XTRA client registered at a time */
  if ( e_cmd_err != PDSM_XTRA_T_CMD_ERR_NONE )
  {
    MSG_ERROR( " XTRA-T cmd err, cmd = %d, err = %d, client = %d",
        p_xtra_t_cmd_type->cmd, e_cmd_err, p_xtra_t_cmd_type->client_id );
  }

  /* Call the user supplied command callback */
  if ( p_xtra_t_cmd_type->cmd_cb_func != NULL )
  {
    p_xtra_t_cmd_type->cmd_cb_func( p_xtra_t_cmd_type->data_block_ptr,
                                    p_xtra_t_cmd_type->cmd,
                                    e_cmd_err
                                  );
  } /* End if no callback is provided. */


} /* End tm_xtra_t_client_cmd_status() */


/*===========================================================================

FUNCTION TM_PA_CMD_PROC

DESCRIPTION
   This function is used to process PA related commands.

RETURN VALUE
  None

DEPENDENCIES


===========================================================================*/

void tm_pa_cmd_proc(
    pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_pa_cmd_info_s_type *p_cmd_info_type;
  pdsm_pa_cmd_err_e_type  e_cmd_err;
  pdsm_pa_cmd_s_type      *p_pa_cmd_type;
  pdsm_pa_event_type      e_pa_event =0;
  pdsm_pa_info_s_type     z_pa_info_type;
  pdsm_pa_info_s_type     *p_pa_info_type = &z_pa_info_type;
  boolean                 u_ret = TRUE;
  tm_pdapi_param_s_type z_pa_param_type;
  tm_pdapi_param_s_type *p_pa_param_type = &z_pa_param_type;
  pdsm_client_s_type    *client_ptr;

  if (p_cmd_type  == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_cmd_info_type = &p_cmd_type->cmd.pa.info;
  p_pa_cmd_type = &p_cmd_type->cmd.pa;
  MSG_LOW( "TM_PDAPI: tm_pa_cmd_proc () getting called",0,0,0);
  /* Here commands are handled which are passed from a FIFO queue.
  ** Don't worry about priority and QOS for Alpha release.
  */
  e_cmd_err = PDSM_PA_CMD_ERR_NOERR;

  /* Now proceed with the command as ewerything looks just fine. In case of
  ** connection failure we will indicate an Abort (error) event to clients.
  */
  if ( ( pdsmclient_valid_client_id( p_cmd_type->cmd.pa.client_id )) == FALSE )
  {
    e_cmd_err = PDSM_PA_CMD_ERR_CLIENT_ID_P;
    MSG_ERROR("TM_PDAPI: PDSM_PA_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
    tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {
    e_cmd_err = PDSM_PA_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
    MSG_ERROR("TM_PDAPI: PDSM_PA_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S",0,0,0);
    tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }
  
  client_ptr = pdsmclient_get_client_ptr(p_cmd_type->cmd.pa.client_id);
  p_pa_info_type->client_id = p_cmd_type->cmd.pa.client_id;

  switch ( p_pa_cmd_type->cmd )
  {
    case PDSM_PA_CMD_SET_PARAM:
      /* First find out if the command is O.K, check the parameters
      ** and the session state and other things.
      */
      sm_log_set_parameters_request(p_cmd_type);

      p_pa_param_type->client_id = p_cmd_type->cmd.pa.client_id;
      p_pa_param_type->e_pa_type = p_cmd_type->cmd.pa.info.set_type;
      p_pa_param_type->pa_info_type_ptr = &p_cmd_type->cmd.pa.info.pa_set;

      u_ret = tm_core_set_param_check(p_pa_param_type);

      if( u_ret )
      {
        e_cmd_err = tm_pa_client_cmd_check( p_cmd_type );
      }
      else
      {
        e_cmd_err = PDSM_PA_CMD_ERR_NOT_ALLOWED;
        /* Set cmd is not allowed when TM task is not active or there is an active session */
      }

      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure.
      */
      tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );

      if (e_cmd_err == PDSM_PA_CMD_ERR_NOERR)
      {
      /* call the tmcore function to set the parameters */
      u_ret = tm_core_handle_set_param(p_pa_param_type );
      }

      if (p_cmd_type->cmd.pa.info.set_type == PDSM_PA_APP_INFO)
      {
        if (p_cmd_type->cmd.pa.info.pa_set.app_info.url != NULL)
        {
          (void)pdsm_freebuf((char *)p_cmd_type->cmd.pa.info.pa_set.app_info.url);
        }
      }

      if (p_cmd_type->cmd.pa.info.set_type == PDSM_PA_EFS_DATA)
      {
        if (p_cmd_type->cmd.pa.info.pa_set.efs_data.efs_data_filename != NULL)
        {
          (void)pdsm_freebuf((char*)p_cmd_type->cmd.pa.info.pa_set.efs_data.efs_data_filename);
          MSG_LOW("Free EFS data FileName ptr", 0, 0, 0);
        }
        if (p_cmd_type->cmd.pa.info.pa_set.efs_data.efs_data_ptr != NULL)
        {
          (void)pdsm_freebuf((char*)p_cmd_type->cmd.pa.info.pa_set.efs_data.efs_data_ptr);
          MSG_LOW("Free EFS data ptr", 0, 0, 0);
        }
      }
      break ;

    case PDSM_PA_CMD_GET_PARAM:
      p_pa_param_type->client_id = p_cmd_type->cmd.pa.client_id;
      p_pa_param_type->e_pa_type = p_cmd_type->cmd.pa.info.set_type;
      p_pa_param_type->pa_info_type_ptr =&p_cmd_type->cmd.pa.info.pa_set;

      u_ret = tm_core_set_param_check(p_pa_param_type);
      if (u_ret)
      {
        e_cmd_err = tm_pa_client_cmd_check( p_cmd_type );
      }
      else
      {
        e_cmd_err = PDSM_PA_CMD_ERR_NOT_ALLOWED;
        /* Get cmd is not allowed when TM task is not active or there is an active session */
      }


      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure.
      */
      tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );

      if (e_cmd_err != PDSM_PA_CMD_ERR_NOERR)
      {
        return;
      }
      /* call the tmcore function to set the parameters */
      u_ret = tm_core_handle_get_param(p_pa_param_type );

      if ((p_cmd_info_type->set_type == PDSM_PA_DELETE_PARAMS_EX)||
          (p_cmd_info_type->set_type == PDSM_PA_DELETE_PARAMS) ||  /* PDSM_PA_DELETE_PARAMS is deprecated */
         (p_cmd_info_type->set_type == PDSM_PA_DELETE_PARAMS_EX1))  /* PDSM_PA_DELETE_PARAMS is deprecated */
      {
        e_pa_event = PDSM_PA_EVENT_DELETE_PARAMS; /* PDSM_PA_DELETE_PARAMS_EX and PDSM_PA_DELETE_PARAMS
                                                     use the same event since payload data is congruent */
        p_pa_info_type->pa_info.delete_params = p_pa_param_type->pa_info_type_ptr->delete_params;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_GPS_LOCK)
      {
        e_pa_event = PDSM_PA_EVENT_GPS_LOCK;
        p_pa_info_type->pa_info.gps_lock = p_pa_param_type->pa_info_type_ptr->gps_lock;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_KEEP_WARM)
      {
        e_pa_event = PDSM_PA_EVENT_KEEP_WARM;
        p_pa_info_type->pa_info.keep_warm_enable = p_pa_param_type->pa_info_type_ptr->keep_warm_enable;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_MO_METHOD)
      {
        e_pa_event = PDSM_PA_EVENT_MO_METHOD;
        p_pa_info_type->pa_info.mo_method = p_pa_param_type->pa_info_type_ptr->mo_method;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_MT_LR_SUPPORT)
      {
        e_pa_event = PDSM_PA_EVENT_MT_LR_SUPPORT;
        p_pa_info_type->pa_info.mt_lr_support = p_pa_param_type->pa_info_type_ptr->mt_lr_support;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_1X_PDE_NETWORK_ADRS)
      {
        e_pa_event = PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS;
        memscpy(&(p_pa_info_type->pa_info.server_address),
               sizeof(p_pa_info_type->pa_info.server_address),
               &(p_pa_param_type->pa_info_type_ptr->server_address), 
               sizeof(p_pa_param_type->pa_info_type_ptr->server_address));  
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_1X_MPC_NETWORK_ADRS)
      {
        e_pa_event = PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS;
        memscpy(&(p_pa_info_type->pa_info.server_address),
               sizeof(p_pa_info_type->pa_info.server_address),
               &(p_pa_param_type->pa_info_type_ptr->server_address), 
               sizeof(p_pa_param_type->pa_info_type_ptr->server_address));  
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_UMTS_PDE_NETWORK_ADRS)
      {
        e_pa_event = PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS;
        memscpy(&(p_pa_info_type->pa_info.server_address),
               sizeof(p_pa_info_type->pa_info.server_address),
               &(p_pa_param_type->pa_info_type_ptr->server_address),
               sizeof(p_pa_param_type->pa_info_type_ptr->server_address));
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_NMEA_SENTENCE_TYPE)
      {
        e_pa_event = PDSM_PA_EVENT_NMEA_SENTENCE_TYPE;
        p_pa_info_type->pa_info.nmea_sentence_type = p_pa_param_type->pa_info_type_ptr->nmea_sentence_type;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_POSITION_MODE)
      {
        e_pa_event = PDSM_PA_EVENT_POSITION_MODE;
        p_pa_info_type->pa_info.position_mode = p_pa_param_type->pa_info_type_ptr->position_mode;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_NMEA_CONFIG_INFO)
      {
        e_pa_event = PDSM_PA_EVENT_NMEA_CONFIG_INFO;
        p_pa_info_type->pa_info.nmea_config_info.pd_nmea_port_type = p_pa_param_type->pa_info_type_ptr->nmea_config_info.pd_nmea_port_type;
        p_pa_info_type->pa_info.nmea_config_info.pd_nmea_reporting_type = p_pa_param_type->pa_info_type_ptr->nmea_config_info.pd_nmea_reporting_type;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_EFS_DATA)
      {
        e_pa_event = PDSM_PA_EVENT_EFS_DATA;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SBAS_STATUS)
      {
        e_pa_event = PDSM_PA_EVENT_SBAS_STATUS;
        p_pa_info_type->pa_info.sbas_status = p_pa_param_type->pa_info_type_ptr->sbas_status;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_DPO_CONTROL)
      {
        e_pa_event = PDSM_PA_EVENT_DPO_CONTROL;
        p_pa_info_type->pa_info.dpo_control = p_pa_param_type->pa_info_type_ptr->dpo_control;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_ON_DEMAND_LOW_POWER_MODE)
      {
        p_pa_info_type->pa_info.on_demand_lpm = p_pa_param_type->pa_info_type_ptr->on_demand_lpm;
        e_pa_event = PDSM_PA_EVENT_ON_DEMAND_LPM;
        p_pa_info_type->pa_info.on_demand_lpm = p_pa_param_type->pa_info_type_ptr->on_demand_lpm;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_ON_DEMAND_READY_MODE)
      {
        p_pa_info_type->pa_info.on_demand_ready = p_pa_param_type->pa_info_type_ptr->on_demand_ready;
        e_pa_event = PDSM_PA_EVENT_ON_DEMAND_READY_MODE;
        p_pa_info_type->pa_info.on_demand_ready = p_pa_param_type->pa_info_type_ptr->on_demand_ready;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_CALL_FLOW_SELECT)
      {
        e_pa_event = PDSM_PA_EVENT_PA_CALL_FLOW_SELECT;
        p_pa_info_type->pa_info.call_flow_select_mask = p_pa_param_type->pa_info_type_ptr->call_flow_select_mask;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_LBS_APN_PROFILE)
      {
        e_pa_event = PDSM_PA_EVENT_LBS_APN_PROFILE_SELECT;
        memscpy(&p_pa_info_type->pa_info.apn_profiles[0], sizeof(p_pa_info_type->pa_info.apn_profiles),
			    &p_pa_param_type->pa_info_type_ptr->apn_profiles[0], sizeof(p_pa_param_type->pa_info_type_ptr->apn_profiles));
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_XTRA_APN_PROFILE)
      {
        e_pa_event = PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT;
        memscpy(&p_pa_info_type->pa_info.apn_profiles[0], sizeof(p_pa_info_type->pa_info.apn_profiles),
			    &p_pa_param_type->pa_info_type_ptr->apn_profiles[0], sizeof(p_pa_param_type->pa_info_type_ptr->apn_profiles));
      }
      else if(p_cmd_info_type->set_type == PDSM_PA_EMERGENCY_LBS_APN_PROFILE)
      {
        e_pa_event = PDSM_PA_EMERGENCY_LBS_APN_PROFILE;
        memscpy(&p_pa_info_type->pa_info.apn_profiles[0], sizeof(p_pa_info_type->pa_info.apn_profiles),
			    &p_pa_param_type->pa_info_type_ptr->apn_profiles[0], sizeof(p_pa_param_type->pa_info_type_ptr->apn_profiles));
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SUPL_VERSION)
      {
        e_pa_event = PDSM_PA_EVENT_SUPL_VERSION;
        p_pa_info_type->pa_info.supl_version = p_pa_param_type->pa_info_type_ptr->supl_version;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SUPL_SECURITY)
      {
        e_pa_event = PDSM_PA_EVENT_SUPL_SECURITY;
        p_pa_info_type->pa_info.supl_security = p_pa_param_type->pa_info_type_ptr->supl_security;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_PE_CONFIG_PARAM)
      {
        e_pa_event = PDSM_PA_EVENT_PE_CONFIG_PARAM;
        p_pa_info_type->pa_info.pe_config_info = p_pa_param_type->pa_info_type_ptr->pe_config_info;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_LPP_CONFIG_INFO)
      {
        e_pa_event = PDSM_PA_EVENT_LPP_CONFIG_TYPE;
        p_pa_info_type->pa_info.lpp_config_info = p_pa_param_type->pa_info_type_ptr->lpp_config_info;
      }
      /* Below are for the new PA commands with don't have the PA_EVENT bitmask defined */
      else if (p_cmd_info_type->set_type == PDSM_PA_ASST_GLONASS_POS_PROTOCOL)
      {
        e_pa_event = (pdsm_pa_event_type)PDSM_PA_ASST_GLONASS_POS_PROTOCOL;
        p_pa_info_type->pa_info.asst_glo_protocol = p_pa_param_type->pa_info_type_ptr->asst_glo_protocol;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SUPL_IS801)
      {
        e_pa_event = (pdsm_pa_event_type)PDSM_PA_SUPL_IS801;
        p_pa_info_type->pa_info.supl_is801 = p_pa_param_type->pa_info_type_ptr->supl_is801;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SUPL_TLS_VERSION)
      {
        e_pa_event = (pdsm_pa_event_type)PDSM_PA_SUPL_TLS_VERSION;
        p_pa_info_type->pa_info.supl_tls_version = p_pa_param_type->pa_info_type_ptr->supl_tls_version;
      }
      else if (p_cmd_info_type->set_type == PDSM_PA_SUPL_HASH_ALGORITHM)
      {
        e_pa_event = (pdsm_pa_event_type)PDSM_PA_SUPL_HASH_ALGORITHM;
        p_pa_info_type->pa_info.supl_hash_algorithm = p_pa_param_type->pa_info_type_ptr->supl_hash_algorithm;
      }

      else if (p_cmd_info_type->set_type == PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG)
      {
        e_pa_event = PDSM_PA_GNSS_EMERGENCY_PROTOCOL_CONFIG;
        p_pa_info_type->pa_info.gnss_emergency_protocol_select = p_pa_param_type->pa_info_type_ptr->gnss_emergency_protocol_select;
      }

      else if (p_cmd_info_type->set_type == PDSM_PA_WIFI_SCAN_TIMEOUT_SEC)
      {
        e_pa_event = (pdsm_pa_event_type)PDSM_PA_WIFI_SCAN_TIMEOUT_SEC;
        p_pa_info_type->pa_info.wifi_wait_timeout_sec = p_pa_param_type->pa_info_type_ptr->wifi_wait_timeout_sec;
      }

      p_pa_info_type->pa_event =  p_cmd_info_type->set_type;
      sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
      sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

      /* inform the client requesting the get parameters */
      pdsm_client_pa_event_ntfy( client_ptr, e_pa_event, p_pa_info_type);
      break;

    default:
      break;

  }
  return;

} /* End pa_cmd_proc( ) */

/*===========================================================================

FUNCTION TM_LCS_CLIENT_CMD_PROC

DESCRIPTION
  This function is used handle client commands to PDSM.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_lcs_client_cmd_proc(
  pdsm_cmd_s_type *p_cmd_type
          /* Pointer to PDSM command */
  )
{
  boolean                 u_ret = TRUE;
  pdsm_lcs_cmd_err_e_type e_cmd_err = PDSM_LCS_CMD_ERR_NOERR;


  if (p_cmd_type  == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {
    e_cmd_err = PDSM_LCS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
    MSG_ERROR("TM_PDAPI: PDSM_LCS_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S",0,0,0);
    tm_lcs_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }
  

  MSG_LOW( "TM_PDAPI: tm_lcs_cmd_proc () getting called",0,0,0);
  /* Here commands are handled which are passed from a FIFO queue.
  ** Don't worry about priority and QOS for Alpha release.
  */

  switch(p_cmd_type->cmd.lcs.cmd)
  {
    case  PDSM_LCS_CMD_NOTIFY_VERIFY_RESP:
      sm_log_notify_verify_response(&p_cmd_type->cmd.lcs);
      tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
      u_ret = tm_core_handle_lcs_response(&(p_cmd_type->cmd.lcs.info.lcs_cmd_info.resp_info));
      break;
    case PDSM_LCS_CMD_CLIENT_APP_INFO_INIT:
      if (p_cmd_type->cmd.lcs.info.lcs_cmd_info.client_init_type.app_info_ptr)
      {
        u_ret = tm_core_handle_lcs_client_init(&(p_cmd_type->cmd.lcs.info.lcs_cmd_info.client_init_type));

        (void)pdsm_freebuf((char *)p_cmd_type->cmd.lcs.info.lcs_cmd_info.client_init_type.app_info_ptr);
      }
      else
      {
        e_cmd_err = PDSM_LCS_CMD_ERR_APP_INFO_P;
        tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
      }
      break;
    case PDSM_LCS_CMD_CLIENT_RELEASE:
      tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
      u_ret = tm_core_handle_lcs_client_release(&(p_cmd_type->cmd.lcs.info.lcs_cmd_info.client_release_type));
      break;
    case PDSM_LCS_CMD_GET_CLIENT_STATUS:
        tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
      //u_ret = tmCore_HandleLcsClientType();
      break;
    case PDSM_LCS_CMD_QUERY_VX_CONFIG:
        tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
        u_ret = tm_core_handle_lcs_query_vx_config((pdsm_client_type_e_type)p_cmd_type->cmd.lcs.info.lcs_cmd_info.client_type);
      break;
    case PDSM_LCS_CMD_SET_VX_CONFIG:
      tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);
      u_ret = tm_core_handle_lcs_set_vx_config(&(p_cmd_type->cmd.lcs.info.lcs_cmd_info.set_vx_config));
      break;
    default:
      e_cmd_err = PDSM_LCS_CMD_ERR_UNKNOWN_CMD;
      tm_lcs_client_cmd_status(p_cmd_type, e_cmd_err);

      break;
  }

  MSG_HIGH("TM_PDAPI: tm_lcs_cmd_proc () cmd type is %d, return value is %", p_cmd_type->cmd.lcs.cmd, u_ret,0);

  return;
} /* End tm_lcs_client_cmd_proc() */


/*===========================================================================

FUNCTION tm_xtra_client_cmd_proc

DESCRIPTION
  This function is used handle XTRA client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_xtra_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type
     /* Pointer to PDSM command */
)
{
  /* Local Variables */
  boolean u_ret = FALSE;
  pdsm_xtra_cmd_err_e_type e_cmd_err = PDSM_XTRA_CMD_ERR_NOERR;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (p_cmd_type  == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_xtra_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Handle XTRA client request
  ----------------------------------------------------------------------*/
  if(tm_core_is_reset_location_service_in_progress())
  {
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    e_cmd_err = PDSM_XTRA_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else
  {
    /* First check the command and session state.
    */
    e_cmd_err = tm_core_xtra_req_check( &p_cmd_type->cmd.xtra );
  }

  /* Indicate to client about the status of this command, irrespective of the
  ** fact whether it was success or failure.
  */
  tm_xtra_client_cmd_status( p_cmd_type, e_cmd_err );

  /* In case of error we need to return from here.
  */
  if (e_cmd_err == PDSM_XTRA_CMD_ERR_NOERR)
  {
    /* Handle the XTRA client request
    */
    u_ret = tm_core_handle_xtra_client_cmd(&p_cmd_type->cmd.xtra);
  }

  if (!u_ret )
  {
    MSG_LOW("TM_PDAPI: TM core returned error for handling XTRA request", 0,0,0);
  }

  return;
} /* End tm_xtra_client_cmd_proc()*/

/*===========================================================================
FUNCTION tm_pdapi_event_first_successful_fix

DESCRIPTION
  This function generates a DM event to report the first successful fix that
  the diag task has received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_pdapi_event_first_successful_fix(void)
{
  event_report(EVENT_CGPS_DIAG_FIRST_SUCCESSFUL_FIX);
} /* gpsdiag_LogSuccessfulFixCount */

/*===========================================================================
FUNCTION tm_pdapi_log_successful_fix_count

DESCRIPTION
   This function generates a DM log message to report the number of successful
   fixes that the diag task has received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_pdapi_log_successful_fix_count(const uint32 cq_SuccessfulFixCount,
                                   const pdsm_client_type_e_type ce_ClientType)
{
  LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type * pz_LogPkt;

  /* Try to allocate memory for the log packet */
  pz_LogPkt = (LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type *)log_alloc(
   LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C,
   sizeof(LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type));

  if(pz_LogPkt != 0)
  {
    /* log is enabled and space is available */
    pz_LogPkt->u_Version = 0;
    pz_LogPkt->q_SuccessfulFixCount = cq_SuccessfulFixCount;
     pz_LogPkt->w_ClientType = (uint16)ce_ClientType;

    log_commit(pz_LogPkt);
  }
} /* gpsdiag_LogSuccessfulFixCount */


/*===========================================================================

FUNCTION tm_xtra_t_client_cmd_proc

DESCRIPTION
  This function is used handle XTRA-T client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_xtra_t_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  /* Local Variables */
  boolean u_ret = FALSE;
  pdsm_xtra_t_cmd_err_e_type e_cmd_err = PDSM_XTRA_T_CMD_ERR_NONE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (p_cmd_type  == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_xtra_t_client_cmd_proc() getting called",0,0,0);

  /* Send a DM log about XTRA-T session type */
#if (0)
  switch (p_cmd_type->cmd.xtra_t.cmd)
  {
    case PDSM_XTRA_T_CMD_GET_DATA:
      sm_log_xtra_t_session_type((uint8) PDSM_XTRA_T_SESSION_UPLOAD_DTR);
      break;

    default:
      sm_log_xtra_t_session_type((uint8) PDSM_XTRA_T_SESSION_NONE);
      break;
  }
#endif

  /*----------------------------------------------------------------------
     Handle XTRA-T client request
  ----------------------------------------------------------------------*/
  if(tm_core_is_reset_location_service_in_progress())
  {    
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    e_cmd_err = PDSM_XTRA_T_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else
  {
    /* First check the command and session state.
    */
    e_cmd_err = tm_core_xtra_t_req_check( &p_cmd_type->cmd.xtra_t );
  }

  /* Indicate to client about the status of this command, irrespective of the
  ** fact whether it was success or failure.
  */
  tm_xtra_t_client_cmd_status( p_cmd_type, e_cmd_err );

  /* In case of error we need to return from here.
  */
  if (e_cmd_err == PDSM_XTRA_T_CMD_ERR_NONE)
  {
    /* Set the client ID in the XTRA-T request struct
    */
    p_cmd_type->cmd.xtra_t.client_id = p_cmd_type->cmd.xtra_t.client_id;

    /* Handle the XTRA-T client request
    */
    u_ret = tm_core_handle_xtra_t_client_cmd( &p_cmd_type->cmd.xtra_t );
  }

  if (!u_ret )
  {
    MSG_LOW("TM_PDAPI: TM core returned error for handling XTRA-T request", 0,0,0);
  }

  return;
} /* tm_xtra_t_client_cmd_proc */

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif

/*===========================================================================

FUNCTION tm_wiper_client_cmd_proc

DESCRIPTION
  This function is used handle WIPER client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_wiper_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type
     /* Pointer to PDSM command */
)
{
  /* Local Variables */
  pdsm_wiper_cmd_err_e_type e_cmd_err = PDSM_WIPER_CMD_ERR_NOERR;
  uint32 ii = 0;
  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (p_cmd_type  == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    e_cmd_err = PDSM_WIPER_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }

  MSG_LOW( "TM_PDAPI: tm_wiper_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Handle WIPER client request
  ----------------------------------------------------------------------*/
  /* First check the command and session state.
  */

  /* Indicate to client about the status of this command, irrespective of the
  ** fact whether it was success or failure.
  */

  /* In case of error we need to return from here.
  */
  if (e_cmd_err == PDSM_WIPER_CMD_ERR_NOERR)
  {
    /* Handle the WIPER client request
    */
    tm_core_handle_wiper_client_cmd(&p_cmd_type->cmd.wiper);

    if(p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_valid_info_flag & WIPER_LOG_AP_SET_VALID)
    {
      MSG_MED("Injecting wifi scans from HLOS into GM ", 0, 0, 0);
      p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set.e_wifi_scan_src = PDSM_WIFI_SCAN_SRC_HLOS;
      /* Currently HLOS injects the AP frequency information in the AP channel number. So we copy the frequency information into the correct field.  */
      for (ii = 0; ii < p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set.num_of_aps; ii++) 
      {
        p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set.ap_info[ii].freq = 
            p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set.ap_info[ii].channel;
      }
      /* The wifi scans from HLOS are assumed to be FULL scans . So there is no need of filtering based on number of frequencies scanned .
         So the bitmask for identifying the number of frequencies scanned for 2.4Ghz is set to 0xFFFF meaning that enough 2.4Ghz scans
         are present and GM need not filter the 2.4Ghz scans based on frequency.
         However we do not filter any other kind of scan ( 5 Ghz etc.. ) based on number of frequencies scanned. So there is no
         variable or bitmask to identify those bands for now . */
      p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set.z_wlan_channel_info.q_2_4_Ghz_channel_mask = 0xFFFF;
      gm_inject_wifi_scan_indication(&(p_cmd_type->cmd.wiper.info.position_report_ptr->wiper_pos_report.wiper_ap_set));
    }
  }

  return;
} /* End tm_wiper_client_cmd_proc()*/

/*===========================================================================

FUNCTION tm_motion_client_cmd_proc

DESCRIPTION
  This function is used handle MOTION client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_motion_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_pd_cmd_err_e_type cmd_status = PDSM_PD_CMD_ERR_NOERR;

  MSG_LOW( "TM_PDAPI: tm_motion_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if(NULL == p_cmd_type)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  if(tm_core_is_reset_location_service_in_progress())
  {    
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else if( FALSE == mgp_ValidateMotionData(&p_cmd_type->cmd.motion.motion_data) )
  {
    MSG_ERROR("TM_PDAPI: Invalid Motion Data Params", 0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
  }
  else
  {
    (void)mgp_InjectMotionData(&p_cmd_type->cmd.motion.motion_data);
  }

  /* issue callback with appropriate status */
  if(NULL != p_cmd_type)
  {
  if(p_cmd_type->cmd.motion.cmd_cb_func)
  {
    p_cmd_type->cmd.motion.cmd_cb_func(p_cmd_type->cmd.motion.client_data_ptr, PDSM_PD_CMD_INJECT_MOTION_DATA, cmd_status);
    }
  }

  return;
} /* tm_motion_client_cmd_proc */


/*===========================================================================

FUNCTION tm_ext_cell_client_cmd_proc

DESCRIPTION
  This function is used handle External Cell Injection client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_ext_cell_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_pd_cmd_err_e_type cmd_status = PDSM_PD_CMD_ERR_NOERR;

  MSG_LOW( "TM_PDAPI: tm_ext_cell_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if(NULL == p_cmd_type)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {    
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else 
  {
    // Below function needs to do CellInfo validity check and then return cmd_status
    if ( tm_core_external_cell_db_proc(&p_cmd_type->cmd.ext_cell.cell_info) == FALSE)
    {
      // invalid Cell info passed in?
      cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;  
    }
  }

  /* issue callback with appropriate status */
  if(p_cmd_type!= NULL && p_cmd_type->cmd.ext_cell.cmd_cb_func)
  {
    p_cmd_type->cmd.ext_cell.cmd_cb_func(p_cmd_type->cmd.ext_cell.client_data_ptr, PDSM_PD_CMD_INJECT_CELL_INFO, cmd_status);
  }

  return;
} /* tm_ext_cell_client_cmd_proc */


/*===========================================================================

FUNCTION tm_ni_msg_client_cmd_proc

DESCRIPTION
  This function is used handle NI message client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_ni_msg_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_pd_cmd_err_e_type cmd_status = PDSM_PD_CMD_ERR_NOERR;

  MSG_LOW( "TM_PDAPI: tm_ni_msg_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if(NULL == p_cmd_type)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {    
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else if( p_cmd_type->cmd.ni_msg.ni_msg.type != NI_MESSAGE_SUPL ||
           p_cmd_type->cmd.ni_msg.ni_msg.len <= 0 ||
           p_cmd_type->cmd.ni_msg.ni_msg.ni_data == NULL)
  {
    MSG_ERROR("TM_PDAPI: Invalid NI Message Params (type=%d len=%d)", 
              p_cmd_type->cmd.ni_msg.ni_msg.type,
              p_cmd_type->cmd.ni_msg.ni_msg.len,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
  }
  else
  {
    if (tm_core_handle_inject_ni_msg(&p_cmd_type->cmd.ni_msg.ni_msg) == FALSE) // fail to inject
    {
      // either type is not SUPL or length is wrong, otherwise should succeed
      cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;  

      /* don't free buffer for success case, it'll be freed later inside PDCOMM WMS */
      os_MemFree((void **)&(p_cmd_type->cmd.ni_msg.ni_msg.ni_data)); 
    }
  }

  /* issue callback with appropriate status */
  if(p_cmd_type!= NULL && p_cmd_type->cmd.ni_msg.cmd_cb_func)
  {
    p_cmd_type->cmd.ni_msg.cmd_cb_func(p_cmd_type->cmd.ni_msg.client_data_ptr, PDSM_PD_CMD_INJECT_NI_MSG, cmd_status);
  }

  return;
} /* tm_ni_msg_client_cmd_proc */



/*===========================================================================

FUNCTION tm_wifi_on_demand_client_cmd_proc

DESCRIPTION
  This function is used handle WiFi-on-demanc scan result client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_wifi_on_demand_client_cmd_proc
(
  pdsm_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_pd_cmd_err_e_type cmd_status = PDSM_PD_CMD_ERR_NOERR;


  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if(NULL == p_cmd_type)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
    return;
  }
  else if(tm_core_is_reset_location_service_in_progress())
  {    
    MSG_ERROR("TM_PDAPI: Reset Location Service in progress",0,0,0);
    cmd_status = PDSM_PD_CMD_ERR_RESET_LOCATION_SERVICE_IN_PROGRESS_S;
  }
  else if( p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.q_num_wifi_aps == 0 ||
           p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.q_num_wifi_aps > QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V01 ||
           p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr == NULL)
  {
    MSG_ERROR("TM_PDAPI: Invalid WiFi-on-demand-scan-result-injection (numWifiAps=%d first_wifi_ap_ptr=%lx)", 
              p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.q_num_wifi_aps,
              p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr,0);
    cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;
  }
  else
  {
    if (tm_core_handle_inject_wifi_on_demand_msg(&p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list) == FALSE) // fail to inject
    {
      cmd_status = PDSM_PD_CMD_ERR_INVALID_PARAM;  
    }
  }

  if(p_cmd_type != NULL)
  {    
  MSG_MED("Freeing mem. at %lx",p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr, 0, 0);
  os_MemFree((void **)&(p_cmd_type->cmd.wifi_on_demand_info_list_cmd.wifi_on_demand_info_list.first_wifi_ap_ptr)); 
  }

  /* issue callback with appropriate status */
  if(p_cmd_type!= NULL && p_cmd_type->cmd.wifi_on_demand_info_list_cmd.cmd_cb_func)
  {
    p_cmd_type->cmd.wifi_on_demand_info_list_cmd.cmd_cb_func(p_cmd_type->cmd.wifi_on_demand_info_list_cmd.client_data_ptr, 
                                                             PDSM_PD_CMD_INJECT_WIFI_ON_DEMAND, cmd_status);
  }

  return;
} /* tm_wifi_on_demand_client_cmd_proc */

/*===========================================================================
FUNCTION  TM_PDAPI_PD_POS EVENT_CALLBACK

DESCRIPTION
  Sends a PD_EVENT POSITION event to client and logs the event in DM.

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pos_event_callback ( pdsm_pd_event_type  e_pd_event,
                                  pdsm_pd_info_s_type *p_pd_info_type,
                                  tm_pdapi_sess_type  sess_type
                                )
{
  static uint32 q_successful_fix_count = 0;

  if (p_pd_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;

  }
  else if(tm_core_is_internal_session_on() == TRUE)
  {
    MSG_MED("TM_PDAPI: Gps session is internal ", 0,0,0);
    return;
  }

  if (e_pd_event & PDSM_PD_EVENT_POSITION)
  {
    const pdsm_client_type_e_type ce_client_type = pdsmclient_get_client_type_map(p_pd_info_type->client_id);

    q_successful_fix_count++;
    MSG_MED("tm_pdapi_pos_event_callback: PDSM_PD_EVENT_POSITION, Successful Fixes %u",
     q_successful_fix_count, 0, 0);

    if(q_successful_fix_count == 1)
    {
      /* This is the first successful fix */
      tm_pdapi_event_first_successful_fix();
    }

    tm_pdapi_log_successful_fix_count(q_successful_fix_count,
     ce_client_type);
  }

  p_pd_info_type->pd_info.pd_data.meas_data = NULL;
  p_pd_info_type->pd_info.pd_data.pm_data = NULL;
  p_pd_info_type->client_id = p_pd_info_type->client_id;
  (void) time_get(p_pd_info_type->pd_info.time_stamp);

  MSG_LOW( "TM_PDAPI: tm_pdapi_pos_event_callback () getting called",0,0,0);

  if( (tm_core_is_on_demand_session_on() != TRUE ) &&
      (tm_core_is_internal_session_on() != TRUE )
    )
  {
    pdsmclient_list_pd_event_ntfy( e_pd_event, p_pd_info_type);
  }
}

/*===========================================================================
FUNCTION  TM_PDAPI_PD_POS_LOG_GENERATION

DESCRIPTION
  Log PD_EVENT POSITION in DM.

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pos_log_generation( pdsm_pd_event_type   e_pd_event,
                                  pdsm_pd_info_s_type* p_pd_info_type,
                                  boolean              is_sft,
                                  boolean              is_kf,
                                  tm_pdapi_sess_type   sess_type
                                )
{
  if (p_pd_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  else if(tm_core_is_internal_session_on() == TRUE)
  {
    MSG_MED("TM_PDAPI: Gps session is internal ", 0,0,0);
    return;
  }

  p_pd_info_type->pd_info.pd_data.meas_data = NULL;
  p_pd_info_type->pd_info.pd_data.pm_data = NULL;
  p_pd_info_type->client_id = p_pd_info_type->client_id;
  (void) time_get(p_pd_info_type->pd_info.time_stamp);

  MSG_LOW( "TM_PDAPI: tm_pdapi_pos_log_generation () getting called",0,0,0);
  if ((e_pd_event & PDSM_PD_EVENT_POSITION) ||
      (e_pd_event & PDSM_PD_EVENT_VELOCITY) ||
      (e_pd_event & PDSM_PD_EVENT_HEIGHT))
  {
    sm_log_report_position(p_pd_info_type, is_sft, is_kf);
    sm_report_event_gps_pd_position();
  }
}

/*===========================================================================
FUNCTION  TM_PDAPI_PD_EVENT_CALLBACK

DESCRIPTION
  Sends other PD_EVENT  events to client and logs the event in DM. All events
  other than the Position event use this callback

Parameters:

  pd_pd_info_type: the event and the event data to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pd_event_callback ( tm_pdapi_pd_event_s_type *p_pd_info_type ,
                                  tm_pdapi_sess_type  sess_type)
{
  boolean send_event = TRUE;

  if (p_pd_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI pd-event_callback: Null pointer to function ", 0,0,0);
    return;
  }
  else if( (tm_core_is_internal_session_on() == TRUE) &&
           (p_pd_info_type->e_pd_event != PDSM_PD_EVENT_WPS_NEEDED) &&
           (!(p_pd_info_type->e_pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_MASK))
         )
  {
    MSG_MED("TM_PDAPI: Gps session is internal %d ", p_pd_info_type->e_pd_event,0,0);
    return;
  }

  p_pd_info_type->pd_event_data.pd_info_type.pd_info.pd_data.meas_data = NULL;
  p_pd_info_type->pd_event_data.pd_info_type.pd_info.pd_data.pm_data = NULL;
  (void) time_get(p_pd_info_type->pd_event_data.pd_info_type.pd_info.time_stamp);

  if (sess_type != TM_PDAPI_SESS_TYPE_NONE)
  {
    cached_sess_type = sess_type;
  }
  switch (p_pd_info_type->e_pd_event)
  {

     case PDSM_PD_EVENT_DONE:
        sm_report_event_gps_pd_session_done();
        break;

      case PDSM_PD_EVENT_END:
        sm_report_event_gps_pd_event_end((uint8)p_pd_info_type->pd_event_data.pd_info_type.pd_info.end_status);
        break;

      case PDSM_PD_EVENT_BEGIN:
        if(sess_type == TM_PDAPI_SESS_TYPE_MI)
        {
          sm_report_event_gps_pd_session_start(&p_pd_info_type->pd_event_data.pd_get_pos_data,(uint8)MOBILE_INIT_SESSION);
        }
        else
        {
          sm_report_event_gps_pd_session_start(&p_pd_info_type->pd_event_data.pd_get_pos_data,(uint8)NETWORK_INIT_SESSION);
        }
        break;

      case PDSM_PD_EVENT_UPDATE_FAILURE:
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        break;

  /* Communication related events */
      case PDSM_PD_EVENT_COMM_BEGIN:
        sm_report_event_gps_pd_comm_start((uint8) p_pd_info_type->pd_event_data.pd_info_type.pd_info.comm_data.protocol_type);
        break;
      case PDSM_PD_EVENT_COMM_CONNECTED:
        sm_report_event_gps_pd_comm_connected();
        break;
      case PDSM_PD_EVENT_COMM_FAILURE:
        /* this event can be called when the tm_core has cleaned up. use the cahced_client_id */
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        sm_report_event_gps_pd_comm_failure((uint8)p_pd_info_type->pd_event_data.pd_info_type.pd_info.end_status);
        break;
      case PDSM_PD_EVENT_COMM_DONE:
        /* this event can be called when the tm_core has cleaned up. use the cahced_client_id */
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        sm_report_event_gps_pd_comm_done();
        break;
      case PDSM_PD_EVENT_WARM_PROC_BEGIN:
      case PDSM_PD_EVENT_WARM:
      case PDSM_PD_EVENT_NOT_WARM:
      case PDSM_PD_EVENT_WARM_PROC_DONE:
      case PDSM_PD_EVENT_JGPS_PROGRESS:
      case PDSM_PD_EVENT_SECURITY:

      case PDSM_PD_EVENT_GPS_BEGIN:

      case PDSM_PD_EVENT_GPS_DONE:
        /* this event can be called when the tm_core has cleaned up. use the cahced_client_id */
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        break;

      case PDSM_PD_EVENT_DLOAD_BEGIN:
      case PDSM_PD_EVENT_DLOAD:
      case PDSM_PD_EVENT_DLOAD_DONE:
      case PDSM_PD_EVENT_WPS_NEEDED:
      case (uint32)PDSM_PD_DLOAD_EVENT_END: /* keep lint happy */
      break;

      /* add actions particular to xspi injection event here */
      case PDSM_PD_EVENT_REQUEST_EXTERNAL_SPI_INJECTION:
         /* if we don't know which client this event belongs, to use the one
         that started the get position session */
         if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
         {
           p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
         }
         break;

    default:
      break;
  }
  sm_log_pd_event_callback(p_pd_info_type->e_pd_event, &p_pd_info_type->pd_event_data.pd_info_type);

  if( (tm_core_is_internal_session_on() == TRUE) &&
      (p_pd_info_type->e_pd_event != PDSM_PD_EVENT_WPS_NEEDED) &&
      (!(p_pd_info_type->e_pd_event & PDSM_PD_EVENT_COARSE_POS_INJ_MASK))
    )
  {
    MSG_HIGH("Internal Session: PDAPI event %d not sent to PDAPI clients", p_pd_info_type->e_pd_event, 0,0);
    send_event = FALSE;
  }
  /* if there is no on-demand session or if the session type is ON_DEMAND dont send the events
    to pdapi. for gps_done, the session type is not valid. so we need to cache the session id
    and see if the cached session was on demand. then dont send the event. for gps bein the
    tm core session info is not set before sending the event. so check if the passed in sess
    type is on-demand */
  else if (tm_core_is_on_demand_session_on() != TRUE)
  {
    if (p_pd_info_type->e_pd_event == PDSM_PD_EVENT_BEGIN)
    {
      if (sess_type == TM_PDAPI_SESS_TYPE_ON_DEMAND)
      {
        MSG_HIGH("PDAPI event %d not sent to PDAPI clients", p_pd_info_type->e_pd_event, 0,0);
        send_event = FALSE;
      }
    }
    else if (p_pd_info_type->e_pd_event == PDSM_PD_EVENT_DONE ||
             p_pd_info_type->e_pd_event == PDSM_PD_EVENT_GPS_DONE)
    {
      if (cached_sess_type == TM_PDAPI_SESS_TYPE_ON_DEMAND)
      {
        MSG_HIGH("PDAPI event %d not sent to PDAPI clients", p_pd_info_type->e_pd_event, 0,0);
        send_event = FALSE;
      }
    }
  }
  else
  {
    if (p_pd_info_type->e_pd_event == PDSM_PD_EVENT_WPS_NEEDED)
    {
      send_event = TRUE;
    }
    else
    {
      MSG_HIGH("PDAPI event %d not sent to PDAPI clients", p_pd_info_type->e_pd_event, 0,0);
      send_event = FALSE;
    }
  }

  if (send_event == TRUE)
  {
    MSG_MED("PDAPI event 0x %lx sent to PDAPI clients", p_pd_info_type->e_pd_event,0, 0);
    pdsmclient_list_pd_event_ntfy(p_pd_info_type->e_pd_event, &p_pd_info_type->pd_event_data.pd_info_type);
  }

}


/*===========================================================================
FUNCTION  TM_PDAPI_PA_EVENT_CALLBACK

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  p_Client -
  e_pd_event: the event to be sent to PDAPI
===========================================================================*/

void tm_pdapi_pa_event_callback( pdsm_pa_info_s_type *p_pa_info_type)
{
  pdsm_pa_event_type e_pa_event = PDSM_PA_EVENT_NONE;

  if (p_pa_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  // pa event enum range check
  if ( (p_pa_info_type->pa_event < PDSM_PA_NONE) || (p_pa_info_type->pa_event >= PDSM_PA_MAX) )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PA event %d is out of range. tm_pdapi_pa_event_callback() not called.", p_pa_info_type->pa_event);
    return;
  }

  (void) time_get(p_pa_info_type->time_stamp);

  /* No PA event conversion (enum to bitmask style) required after 'PDSM_PA_LPP_CONFIG_INFO' enumerator.
   *  Bitmask style PA event processing is discontinued from 'PDSM_PA_LPP_CONFIG_INFO' onwards.
   */
  switch (p_pa_info_type->pa_event)
  {
    case PDSM_PA_APP_INFO:           /* Set App Info */
      e_pa_event = PDSM_PA_EVENT_APP_INFO;
      break;

    case PDSM_PA_GPS_LOCK:           /* GPS LOCK */
      e_pa_event = PDSM_PA_EVENT_GPS_LOCK;
      break;

    case PDSM_PA_KEEP_WARM:          /* Enable/Disable Keep WARM */
      e_pa_event = PDSM_PA_EVENT_KEEP_WARM;
      break;

    case PDSM_PA_DELETE_PARAMS: /* DEPRECATED. Use PDSM_PA_DELETE_PARAMS_EX instead. Intentional cascade */
    case PDSM_PA_DELETE_PARAMS_EX:/* Delete GPS / GNSS related parameters from the database */
    case PDSM_PA_DELETE_PARAMS_EX1:/* Delete GPS / GNSS related parameters from the database */
      e_pa_event = PDSM_PA_EVENT_DELETE_PARAMS; /* PDSM_PA_DELETE_PARAMS and PDSM_PA_DELETE_PARAMS_EX use
                                                the same event since data payload is congruent */
      break;

    case PDSM_PA_POSITION_MODE: /* Position modes that are supported on the mobile */
      e_pa_event = PDSM_PA_EVENT_POSITION_MODE;
      break;

    case PDSM_PA_MT_LR_SUPPORT: /* Specifies if MT-LR is supported */
      e_pa_event = PDSM_PA_EVENT_MT_LR_SUPPORT;
      break;

    case PDSM_PA_MO_METHOD: /* specifies if a CP or UP protocol is used for MO */
      e_pa_event = PDSM_PA_EVENT_MO_METHOD;
      break;

    case PDSM_PA_NMEA_SENTENCE_TYPE: /* specifies the NEMA sentence type to be reported to PDSM */
      e_pa_event = PDSM_PA_EVENT_NMEA_SENTENCE_TYPE;
      break;

    case PDSM_PA_1X_PDE_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
      e_pa_event = PDSM_PA_EVENT_1X_PDE_NETWORK_ADDRESS;
      break;

    case PDSM_PA_1X_MPC_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
      e_pa_event = PDSM_PA_EVENT_1X_MPC_NETWORK_ADDRESS;
      break;

    case PDSM_PA_UMTS_PDE_NETWORK_ADRS:       /* Adress of the network entity, can be IP or URL */
      e_pa_event = PDSM_PA_EVENT_UMTS_PDE_NETWORK_ADDRESS;
      break;
    case PDSM_PA_PDAPI_ENABLE:
      e_pa_event = PDSM_PA_EVENT_PDAPI_ENABLE;
      break;

    case PDSM_PA_SSD_DATA:       /* Adress of the network entity, can be IP or URL */
      e_pa_event = PDSM_PA_EVENT_SSD_INFO;
      break;

    case PDSM_PA_SEC_UPDATE_RATE:       /* Adress of the network entity, can be IP or URL */
      e_pa_event = PDSM_PA_EVENT_SEC_UPDATE_RATE;
      break;

    case PDSM_PA_NMEA_CONFIG_INFO:
      e_pa_event = PDSM_PA_EVENT_NMEA_CONFIG_INFO;
      break;

    case PDSM_PA_EFS_DATA:
      e_pa_event = PDSM_PA_EVENT_EFS_DATA;
      break;

    case PDSM_PA_DPO_CONTROL:
      e_pa_event = PDSM_PA_EVENT_DPO_CONTROL;
      break;

    case PDSM_PA_ON_DEMAND_LOW_POWER_MODE:
      e_pa_event = PDSM_PA_EVENT_ON_DEMAND_LPM;
      break;

    case PDSM_PA_ON_DEMAND_READY_MODE:
      e_pa_event = PDSM_PA_EVENT_ON_DEMAND_READY_MODE;
      break;

    case PDSM_PA_SBAS_USER_PREFERENCE:
      e_pa_event = PDSM_PA_EVENT_SBAS_USER_PREFERENCE;
      break;

    case PDSM_PA_CALL_FLOW_SELECT:
      e_pa_event = PDSM_PA_EVENT_PA_CALL_FLOW_SELECT;
      break;

    case PDSM_PA_EXTERNAL_SPI:
       e_pa_event = PDSM_PA_EVENT_EXTERNAL_SPI;
       break;

    case PDSM_PA_XTRAT_CLIENT_TOKEN:
      e_pa_event = PDSM_PA_EVENT_XTRAT_CLIENT_TOKEN;
      break;

    case PDSM_PA_XTRAT_USER_SESSION_CONTROL:
      e_pa_event = PDSM_PA_EVENT_XTRAT_SESSION_CONTROL;
      break;


    case PDSM_PA_LBS_APN_PROFILE:
      e_pa_event = PDSM_PA_EVENT_LBS_APN_PROFILE_SELECT;
      break;

    case PDSM_PA_XTRA_APN_PROFILE:
      e_pa_event = PDSM_PA_EVENT_XTRA_APN_PROFILE_SELECT;
      break;

    case PDSM_PA_SUPL_VERSION:
      e_pa_event = PDSM_PA_EVENT_SUPL_VERSION;
      break;

    case PDSM_PA_SUPL_SECURITY:
      e_pa_event = PDSM_PA_EVENT_SUPL_SECURITY;
      break;

    case PDSM_PA_PE_CONFIG_PARAM:
      e_pa_event = PDSM_PA_EVENT_PE_CONFIG_PARAM;
      break;
    case PDSM_PA_LPP_CONFIG_INFO:
      e_pa_event = PDSM_PA_EVENT_LPP_CONFIG_TYPE;
      break;

      // ----- No more new PA event conversion ----- //
    default:
      break;  // This line shouldn't be executed. Only to resolve warning msg. 
  }

  MSG_LOW( "TM_PDAPI: tm_pdapi_pa_event_callback () getting called",0,0,0);
  sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
  sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

  pdsmclient_list_pa_event_ntfy(e_pa_event, p_pa_info_type);
}

/*===========================================================================
FUNCTION  tm_pdapi_pa_event_callback_ex

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  p_Client -
  e_pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pa_event_callback_ex( pdsm_pa_info_s_type *p_pa_info_type, pdsm_pa_cmd_err_e_type   pa_cmd_err)
{
  pdsm_pa_event_type e_pa_event = PDSM_PA_EVENT_NONE;

  if (p_pa_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  // pa event enum range check
  if ( (p_pa_info_type->pa_event < PDSM_PA_NONE) || (p_pa_info_type->pa_event >= PDSM_PA_MAX) )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PA event %d is out of range. tm_pdapi_pa_event_callback() not called.", p_pa_info_type->pa_event);
    return;
  }

  (void) time_get(p_pa_info_type->time_stamp);


  MSG_LOW( "TM_PDAPI: tm_pdapi_pa_event_callback_ex () getting called",0,0,0);
  sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
  sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

  pdsmclient_list_pa_event_ntfy_ex(e_pa_event, p_pa_info_type, pa_cmd_err);
}


/*===========================================================================
FUNCTION  TM_PDAPI_LCS_EVENT_CALLBACK

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  e_pd_event: the event to be sent to PDAPI
===========================================================================*/

void tm_pdapi_lcs_event_callback(pdsm_lcs_event_type e_lcs_event,
                                 pdsm_lcs_info_s_type *p_lcs_info_type)
{
  if (p_lcs_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_pdapi_lcs_event_callback () getting called",0,0,0);

  (void) time_get (p_lcs_info_type->time_stamp);


  if (e_lcs_event == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ  ||
      e_lcs_event == PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ  ||
      e_lcs_event == PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ ||
      e_lcs_event == PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION ||
      e_lcs_event == PDSM_LCS_EVENT_CLIENT_VX_CONFIG)
  {
    tm_pdapi_lcs_send_notification_verifcation((uint32)e_lcs_event, p_lcs_info_type);
  }
  else
  {
    (void)pdsm_lcs_agent_client_event_ntfy(e_lcs_event, p_lcs_info_type);
  }
}

/*
 ******************************************************************************
 * tm_pdapi_lcs_send_notification_verifcation
 *
 * Function description:
 *   LCS Agent's sending of notification verification to PDAPI
 *
 * Parameters:
 *
 *  : pointer to a structure holding decoded PDU
 *
 * Return value:
 *  None
 *
 *
 ******************************************************************************
*/
void tm_pdapi_lcs_send_notification_verifcation( uint32  u_EventType, /* Type of LCS event */
                                                 pdsm_lcs_info_s_type  *p_lcs_info_type) /* data to be sent to UI client */
{

  if (p_lcs_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_lcs_send_notification_verfication () getting called",0,0,0);

  p_lcs_info_type->client_id = pdsm_ni_client_id_get();
  sm_log_notification_verification_request(p_lcs_info_type);
  sm_report_event_ui_req((uint32)p_lcs_info_type->client_id, (byte)p_lcs_info_type->lcs_cb_cmd_type);
  (void)pdsm_lcs_agent_client_event_ntfy((pdsm_lcs_event_type)u_EventType, p_lcs_info_type);

  return;

}


/*===========================================================================
FUNCTION  TM_PDAPI_EXT_STATUS_EVENT_CALLBACK

DESCRIPTION
  Sends a EXT_STATUS_EVENT to client and logs the event in DM.

Parameters:

  ext_status_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_ext_status_event_callback ( pdsm_ext_status_e_type  e_ext_status_event,
                                          pdsm_ext_status_info_s_type *p_ext_status_info_type)
{
  pdsm_ext_status_event_type ext_status_event_type = PDSM_EXT_STATUS_EVENT_POS_REPORT;

  if (p_ext_status_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  (void) time_get(p_ext_status_info_type->time_stamp);
  switch(e_ext_status_event)
  {
    case PDSM_EXT_STATUS_MEASUREMENT:
      sm_log_ext_meas_report((uint32)p_ext_status_info_type->client_id, &p_ext_status_info_type->ext_status_info.ext_meas_report_type);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_MEASUREMENT;
      break;
    case PDSM_EXT_STATUS_NMEA_POS_REPORT:
      sm_log_ext_nmea_position_report((uint32)p_ext_status_info_type->client_id, &p_ext_status_info_type->ext_status_info.ext_nmea_pos_report_type);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_NMEA_POS_REPORT;
      break;
    case PDSM_EXT_STATUS_POS_REPORT:
      sm_log_diag_position((uint32)p_ext_status_info_type->client_id,
                          &p_ext_status_info_type->ext_status_info.ext_pos_report_type, POS_FROM_MGP,
                           (boolean)FALSE, (boolean)TRUE);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_POS_REPORT;
      break;

    case PDSM_EXT_STATUS_GPS_STATE_INFO:
      sm_log_report_ext_status_gps_state_info((uint32)p_ext_status_info_type->client_id, &p_ext_status_info_type->ext_status_info.ext_gps_state_type);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_GPS_STATE_INFO;
      break;

    case PDSM_EXT_STATUS_GENERIC_EVENT:
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_GENERIC_IND;
      break;

    case PDSM_EXT_STATUS_PERIODIC_PPM_REPORT:
      MSG_HIGH("firing PDSM_EXT_STATUS_PERIODIC_PPM_REPORT ", 0,0,0);
      #ifdef FEATURE_GNSS_PERIODIC_PPM
      #error code not present
#endif
      break;

    case PDSM_EXT_STATUS_POS_INJECT_REQUEST:
      MSG_HIGH("TM_PDAPI: request position injection ", 0,0,0);
      sm_log_pos_inj_request((uint32)p_ext_status_info_type->client_id,
                              &p_ext_status_info_type->ext_status_info.ext_pos_inj_req_type);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ;
      break;

    case PDSM_EXT_STATUS_RESET_LOCATION_SERVICE:
      MSG_HIGH("TM_PDAPI: request reset location service ", 0,0,0);    
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_RESET_LOCATION_SERVICE_DONE;    
      p_ext_status_info_type->client_id = cached_client_id;
      break;

    case PDSM_EXT_STATUS_DPO_STATUS:
      MSG_LOW("TM_PDAPI: DPO_STATUS_REPORT ", 0,0,0);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_DPO_STATUS_REPORT;
      break;

    case PDSM_EXT_STATUS_SUPL_SESSION_END:
      MSG_LOW("TM_PDAPI: SUPL SESSION END ", 0,0,0);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_SUPL_SESSION_END;     
      break;     

    case PDSM_EXT_STATUS_EMERG_POS_SESSION_TYPE:
      MSG_LOW("TM_PDAPI: EMERG_POS_SESSION_TYPE ", 0,0,0);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_EMERG_POS_SESSION_TYPE;
      break;

    case PDSM_EXT_STATUS_BEST_AVAIL_POS_REPORT:
      MSG_HIGH("TM_PDAPI: Report best avail position", 0, 0, 0);
      sm_log_report_ext_status_best_avail_pos_info((uint32)p_ext_status_info_type->client_id, &p_ext_status_info_type->ext_status_info.ext_pos_report_qmi_type);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT;
      break;

	case PDSM_EXT_STATUS_GNSS_MEASUREMENT:
      MSG_HIGH("TM_PDAPI: PDSM_EXT_STATUS_GNSS_MEASUREMENT ", 0,0,0);
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_GNSS_MEASUREMENT;
      break;
	case PDSM_EXT_STATUS_SV_POLY_REPORT:
		MSG_HIGH("TM_PDAPI: PDSM_EXT_STATUS_SV_POLY_REPORT ", 0,0,0);
		ext_status_event_type = PDSM_EXT_STATUS_EVENT_SV_POLY;
		break;
	
    case PDSM_EXT_STATUS_AVAIL_WWAN_POS_REPORT:
      MSG_HIGH("TM_PDAPI: Report avail WWAN position", 0, 0, 0);
      /* No support for generating DM log message when available WWAN position info is reported to PDAPI by SM */
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT;
      break;
	case PDSM_EXT_STATUS_AVAIL_POS_REPORT:
      MSG_HIGH("TM_PDAPI: Report avail position", 0, 0, 0);
      /* No support for generating DM log message when available  position info is reported to PDAPI by SM */
      ext_status_event_type = PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT;
      break;

   default:
      break;
  }

  if( ( (tm_core_is_on_demand_session_on() != TRUE ) &&
        (tm_core_is_internal_session_on() != TRUE )
      ) ||
      /*Response to reset location service should always be sent*/
      (ext_status_event_type == PDSM_EXT_STATUS_EVENT_RESET_LOCATION_SERVICE_DONE) ||
      (ext_status_event_type == PDSM_EXT_STATUS_EVENT_BEST_AVAIL_POS_REPORT)       ||
      (ext_status_event_type == PDSM_EXT_STATUS_EVENT_AVAIL_WWAN_POS_REPORT)       ||
      (ext_status_event_type == PDSM_EXT_STATUS_EVENT_AVAIL_POS_REPORT)       ||
      (ext_status_event_type == PDSM_EXT_STATUS_EVENT_POS_INJECT_REQ)
    )
  {
    pdsmclient_list_ext_status_event_ntfy(ext_status_event_type, p_ext_status_info_type);
  }

}


/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA event to client and logs the event in DM.

Parameters:

  xtra_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_xtra_event_callback
(
  pdsm_xtra_event_type   e_xtra_event,
  pdsm_xtra_info_s_type  *p_xtra_info_type
)
{
  /* Check the input parameters */
  if (p_xtra_info_type == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_pdapi_xtra_event_callback () getting called",0,0,0);

  if( e_xtra_event != PDSM_XTRA_EVENT_NONE )
  {
    /* log the event information to diag */
    //sm_log_xtra_event(p_xtra_info_type);

    /* Send the event to gpsOneXTRA client */
    (void)pdsm_xtra_client_request(e_xtra_event, p_xtra_info_type);
  }
} /* End tm_pdapi_xtra_event_callback() */



/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_T_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA-T event to client and logs the event in DM.

Parameters:

  xtra_t_event: the event to be sent to PDAPI
  xtra_t_session_type:  xtra-t session type
===========================================================================*/
void tm_pdapi_xtra_t_event_callback
(
  pdsm_xtra_t_info_s_type     *xtra_t_info_ptr,
  pdsm_xtra_t_session_e_type   xtra_t_session_type
)
{
  boolean  send_event = TRUE;

  /* Check the input parameters */
  if (xtra_t_info_ptr == NULL)
  {
    MSG_ERROR("TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_LOW( "TM_PDAPI: tm_pdapi_xtra_t_event_callback () getting called",0,0,0);

  if( xtra_t_info_ptr->xtra_t_event != PDSM_XTRA_T_EVENT_NONE )
  {
    /* send the DM log information to diag */
#if (0)
    sm_log_xtra_t_event(xtra_t_info_ptr, xtra_t_session_type);
#endif
    /* send the DM event information to diag */
    switch (xtra_t_info_ptr->xtra_t_event)
    {
      case PDSM_XTRA_T_EVENT_SESS_BEGIN:
#if (0)
        sm_report_event_gps_xtra_t_session_begin(xtra_t_session_type);
#endif
        MSG_MED("XTRA-T Event: Session BEGIN: %d",xtra_t_session_type,0,0);
        break;

      case PDSM_XTRA_T_EVENT_DATA:
#if (0)
        sm_report_event_gps_xtra_t_session_data(
          xtra_t_info_ptr->xtra_t_event_payload.data_report.xtra_t_data_len,
          xtra_t_info_ptr->xtra_t_event_payload.data_report.part_number,
          xtra_t_info_ptr->xtra_t_event_payload.data_report.total_parts);
#endif
        MSG_MED("XTRA-T Event: DATA:  len %ld, part %ld, total %ld",
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.xtra_t_data_len,
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.part_number,
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.total_parts);
        break;

      case PDSM_XTRA_T_EVENT_SESS_DONE:
#if (0)
        sm_report_event_gps_xtra_t_session_done(xtra_t_session_type);
#endif
        MSG_MED("XTRA-T Event: Session DONE: %d",xtra_t_session_type,0,0);
        break;

      case PDSM_XTRA_T_EVENT_SESS_END:
#if (0)
        sm_report_event_gps_xtra_t_session_end(xtra_t_info_ptr->xtra_t_event_payload.end_reason);
#endif
        MSG_MED("XTRA-T Event: Session END %d",xtra_t_info_ptr->xtra_t_event_payload.end_reason, 0,0);
        break;

      case PDSM_XTRA_T_EVENT_DOWNLOAD_REQ:    /* Aries 5.0 */
      case PDSM_XTRA_T_EVENT_UPLOAD_REQ:     /* Aries 5.0 */
      case PDSM_XTRA_T_EVENT_NONE:
      default:
        MSG_ERROR("TM_PDAPI_CLIENT: unsupported XTRA-T event",0,0,0);
        send_event = FALSE;
        break;
    }

    /* Send the event to XTRA-T client */
    if (send_event == TRUE)
    {
      (void)pdsm_xtra_t_client_request(xtra_t_info_ptr->xtra_t_event, xtra_t_info_ptr);
    }
  }
} /* End tm_pdapi_xtra_t_event_callback() */

/*===========================================================================
FUNCTION  tm_pdapi_xlate_use_pos_to_pd_ext_pos

DESCRIPTION
  This function translates the received "use this position" from app
  to pdsm_pd_data format.  It is used to format the pdsm_pd_ext_pos_s_type
  for logging by sm_log_diag_position().

Parameters:

===========================================================================*/
static void tm_pdapi_xlate_use_pos_to_pd_ext_pos
(
  pdsm_pd_ext_pos_s_type            *p_pd_ext_pos,
  pdsm_ext_use_this_pos_data_type   *p_use_pos
)
{
  memset( &p_pd_ext_pos->pd_data, 0, sizeof( p_pd_ext_pos->pd_data ) );

  p_pd_ext_pos->pd_data.lat                   = p_use_pos->lat;
  p_pd_ext_pos->pd_data.lon                   = p_use_pos->lon;
  p_pd_ext_pos->pd_data.time_stamp            = p_use_pos->time_stamp;
  p_pd_ext_pos->pd_data.loc_uncertainty_ang   = p_use_pos->loc_uncertainty_ang;
  p_pd_ext_pos->pd_data.loc_uncertainty_a     = p_use_pos->loc_uncertainty_a;
  p_pd_ext_pos->pd_data.loc_uncertainty_p     = p_use_pos->loc_uncertainty_p;
  p_pd_ext_pos->pd_data.opt_field_mask        = p_use_pos->opt_field_mask;
  p_pd_ext_pos->pd_data.altitude              = p_use_pos->altitude;
  p_pd_ext_pos->pd_data.heading               = p_use_pos->heading;
  p_pd_ext_pos->pd_data.velocity_hor          = p_use_pos->velocity_hor;
  p_pd_ext_pos->pd_data.fix_type              = p_use_pos->fix_type;
  p_pd_ext_pos->pd_data.velocity_ver          = p_use_pos->velocity_ver;
  p_pd_ext_pos->pd_data.loc_uncertainty_v     = p_use_pos->loc_uncertainty_v;
  p_pd_ext_pos->pd_data.positioning_source    = p_use_pos->positioning_source;
  p_pd_ext_pos->pd_data.position_type         = p_use_pos->position_type;
  p_pd_ext_pos->pd_data.gpsUtcOffset          = p_use_pos->gpsUtcOffset;
  p_pd_ext_pos->pd_data.loc_uncertainty_conf  = p_use_pos->loc_uncertainty_conf;
  p_pd_ext_pos->pd_data.position_mode         = p_use_pos->position_mode;

  memscpy(&p_pd_ext_pos->pd_data.ext_pos_data, sizeof(p_pd_ext_pos->pd_data.ext_pos_data),
         &p_use_pos->ext_pos_data, sizeof(p_use_pos->ext_pos_data));
} /* tm_pdapi_xlate_use_pos_to_pd_ext_pos */
