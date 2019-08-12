 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM TASK

GENERAL DESCRIPTION
  This module contains TM task setup related functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_task.c#1 $  
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/15   ss       Supporting Geofence Client Config indication
07/08/15   rh      Changed NV refresh to be done on one specifc sub 
04/28/14   ah      Added handling of Set Premium Services indication status
04/17/14   skm     NV Async Read changes
12/23/13   mj	   Remove FEATURE_DUAL_SIM
12/06/13   ah      Support for non-deferrable timers
10/29/13   mj      Initialize WCDMA and TDSCDMA interfaces for segment loading
03/01/13   kj      Move to the DOG Heartbeat mechanism on DIME and beyond
02/28/13   mj      Update gm_register_task_init_notifier for new param and added TM_CORE_MSG_ID_API_REGISTRATION
02/21/13   mj      Adding missed code for handling lsmp param read IPC msgs
07/31/12   mjohn   Support to handle PARAM read/write IPC msgs
03/03/10   ns      DSDS Support. Use MMGSDI for DSDS
07/30/09   gk      ODP 2.0 support
01/12/09   atien   XTRA-T support
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08   gk      On-demand changes
07/20/07   gk      include pdapi_client.h for pdapi_init
07/17/07   lt      L1 interface.
06/14/07   gk      fixed a compile error
06/12/07   cl      Only allow PD API access when TM task is active
12/12/06   ank     Added support for TM_CORE_CM_MSG_ID_CMD_NTFY.
12/07/06   cl      Added NV support
06/13/06   cl      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_LBS_TASK
#include <stdio.h>
#include "mobile.h"
#include "qw.h"
#include "tm_common.h"
#include "gps_common.h"
#endif
#include "msg.h"
#include "aries_os_api.h"
#include "sm_nv.h"
#include "pdapibuf.h"
#include "pdapiclient.h"
#include "tm_data.h"    /* Definition of TM task data structure */
#include "tm_diag.h"    
#include "tm_xtra_t.h"
#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
#include "tm_is801.h"
#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */
#include "tm_pdapi_client.h"
#ifdef FEATURE_CGPS_PDCOMM
#include "pd_comms_api.h"
#endif
#include "tm_cm_iface.h"
#include "tm_rr_iface.h"
#include "tm_nmea.h"
#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif /* FEATURE_RUNTIME_DEVMAP */

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif

#ifdef FEATURE_CGPS_UMTS_CP
  #include "tm_mm_iface.h"
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    #include "tm_rrc_iface.h"
  #endif
#endif

#ifdef FEATURE_CGPS_UMTS_UP
  #include "l1lsmif.h"
#endif
#ifdef FEATURE_USER_PLANE_MRL
  #include "tm_l1_iface.h"
#endif

#ifdef FEATURE_TM_SECURITY
#include "tm_security.h"
#endif /* FEATURE_TM_SECURITY */

#include "tm_ruim.h"

#include "tm_xspi.h"

#ifdef FEATURE_GNSS_PERIODIC_PPM
#error code not present
#endif
#include "gm_api.h"

#include  "tm_lte_nas_msgr_iface.h"
#include "gnss_wwan_iface.h"

#if defined(FEATURE_NO_MODEM)
/* Initialize the GPS Diag Module */
extern void gpsdiag_init (void);  
#endif

/* Array to hold the callbacks for notification */
sm_ApiInitNotifierCb   pz_SmNotifierCb[TM_MAX_NOTIFIERS];

#ifdef __GRIFFON__
void tm_griffon_app_track_start(void);
static pdsm_client_id_type griffon_pdsm_client_id;
static uint32 num_sessions = 0;
#endif /* __GRIFFON__ */

static volatile enum { TM_TASK_STATE_RUN, TM_TASK_STATE_STOP } e_tm_task_state;
/*Task wait mask*/
uint8 tm_task_wait_mask = 0;

#define TM_WAIT_GM_TASK_MASK 0x1


/*===========================================================================
FUNCTION tm_start_up

DESCRIPTION
  This function sets up IPC and creates timers.
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_start_up(void)
{
  (void)os_IpcStart((uint32)THREAD_ID_SM_TM);

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();

#if defined FEATURE_RUNTIME_DEVMAP && !defined FEATURE_CGPS_LBS_TASK_NMEA_PORT 
  rdm_register_open_func( RDM_NMEA_SRVC, tm_nmea_open_cb );
  rdm_register_close_func( RDM_NMEA_SRVC, tm_nmea_close_cb );
#endif

#ifndef __GRIFFON__

  tm_nmea_sio_init();

#endif  // __GRIFFON__

  /* First create Dynamic PDSM buffer pools.
  */
  pdsm_pool_init( );

} /* End tm_StartUp() */

/*===========================================================================
FUNCTION tm_module_init

DESCRIPTION
  This function initializes all the data structure/interfaces in TM
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_module_init(void)
{
#ifdef FEATURE_SEGMENT_LOADING
  /* Initialize WCDMA and TDSCDMA interfaces for segment loading */
  if (!gnss_wwan_init_wcdma_handle())
  {
	MSG_HIGH("Failed to retrieve WCDMA handle for segment loading",0,0,0);
  }
	  
  if (!gnss_wwan_init_tdscdma_handle())
  {
	MSG_HIGH("Failed to retrieve TDSCDMA handle for segment loading",0,0,0);
  } 
#endif

  #if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
  /* Initialize IS801 module, is801 module has to be initialized before tmCore_Init */
  is801_init();
  #endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */

  /* Initialize with CM */
  (void)tm_cm_client_init();

#ifdef FEATURE_MMGSDI_SESSION_LIB
  /* Initialize as RUIM (GSDI) client */
  tm_ruim_gsdi_init();
#endif

#if defined (FEATURE_CGPS_UMTS_CP_GSM) && !defined (FEATURE_GNSS_LOW_MEMORY)
  /* Initialize the TM-RR Interface module */
  tm_rr_iface_init();
#endif /* FEATURE_CGPS_UMTS_CP_GSM && !defined (FEATURE_GNSS_LOW_MEMORY) */
  /* Initialize TM-Core data structures */
  tm_core_init();

  /* Initialize PD API module */
  pdapi_init();

  /* Register CGPS Diag with AMSS Diag module */
  tm_diag_init();

  #ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
 
  tm_msgr_init();
}

/*
 ******************************************************************************
 * tm_offline_handler
 *
 * Description:
 *
 *  This function is used to handle TASK_OFFLINE IPC
 *
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void tm_offline_handler( void )
{

  MSG_HIGH("TASK OFFLINE received",0,0,0);

  tm_core_abort_recover(TRUE, TM_STOP_REASON_TASK_OFFLINE);

  tm_core_info.tm_task_info.task_active = FALSE;  
}

/*
 ******************************************************************************
 * tm_task_stop_handler
 *
 * Description:
 *
 * This function is used to handle Stop IPC
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void tm_task_stop_handler( void )
{
  MSG_HIGH("TASK STOP received",0,0,0);

  tm_core_abort_recover(TRUE, TM_STOP_REASON_TASK_STOP);

  tm_core_info.tm_task_info.task_active = FALSE;

  os_TimerStop(tm_core_info.tm_delete_param_throttle_timer); 

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
}

/*=============================================================================

  FUNCTION      tm_task_stop()

  DESCRIPTION 	Exit TM task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void tm_task_stop( void )
{
  e_tm_task_state = TM_TASK_STATE_STOP;
}

/*===========================================================================
FUNCTION tm_proc

DESCRIPTION
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_proc(void)
{
  os_IpcMsgType         *ipc_msg_ptr;
  
  e_tm_task_state = TM_TASK_STATE_RUN;
      
  while ( e_tm_task_state == TM_TASK_STATE_RUN )
  {
    ipc_msg_ptr = os_IpcReceive();
    if( NULL == ipc_msg_ptr )
    {
      MSG_ERROR("NULL pointer rcvd from os_IpcReceive()", 0, 0, 0);
      continue;
    }
  
    switch (ipc_msg_ptr->q_MsgId)
    {
      /*Handle task maintenance messages*/
      case C_OS_MSG_ID_OFFLINE: 
#ifdef FEATURE_GNSS_SA
 	 MSG_ERROR("TM told to go OFFLINE", 0, 0, 0);
#else  	  	  
        tm_offline_handler();
#endif
      break;
    
      case C_OS_MSG_ID_STOP: 
        tm_task_stop_handler();
      break;

      /* Timer Expiry */
      case C_OS_MSG_ID_TIMER_EXPIRY:
        tm_core_timer_dispatcher((void *)ipc_msg_ptr->p_Data);
      break;

      // Both Read/Write completion handled here.
      case C_OS_MSG_ID_NV_RDRW:  
        (void)sm_nv_rdrw_done();
      break;
      
      case  C_OS_MSG_ID_MSGR_MSG:
        (void)tm_handle_msgr_msg();
      break;

      case TM_CORE_MSG_ID_DIAG:
        tm_diag_msg_handler((void *)ipc_msg_ptr->p_Data);
      break;

      case TM_CORE_MSG_ID_SESS_REQ_API:
        tm_core_lm_sess_req_handler((void *)ipc_msg_ptr->p_Data);
      break;

      case TM_CORE_MSG_ID_SESS_UPDATE_API:
        tm_core_lm_sess_update_handler((void *)ipc_msg_ptr->p_Data);
      break;

      case TM_CORE_MSG_ID_SESS_INFO_API:
        tm_core_lm_sess_info_handler((void *)ipc_msg_ptr->p_Data);
      break;
      
      case TM_CORE_MSG_ID_PDAPI:
        tm_pdapi_cmd_proc ((void*)ipc_msg_ptr->p_Data);
      break;
      
      case  TM_CORE_PDCOMM_MSG_ID_WRITE_ACK:
      case  TM_CORE_PDCOMM_MSG_ID_READ_DATA:
      case  TM_CORE_PDCOMM_MSG_ID_NOTIFY:
      case  TM_CORE_PDCOMM_MSG_ID_IOCTL_ACK:
      case  TM_CORE_PDCOMM_MSG_ID_HASH_RESP:
      case  TM_CORE_PDCOMM_MSG_ID_HANDLE:
      case  TM_CORE_PDCOMM_MSG_ID_SMS_ACK_REPORT:
        tm_core_pdcomm_evt_handler(ipc_msg_ptr->q_MsgId, (void *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_CM_MSG_ID_EVENT_NTFY:
        tm_cm_iface_event_proc ((void*)ipc_msg_ptr->p_Data);
      break;

      case TM_CORE_CM_MSG_ID_CMD_NTFY:
        tm_cm_iface_cmd_proc ((void*)ipc_msg_ptr->p_Data);
      break;

#ifdef FEATURE_RUNTIME_DEVMAP

        /* RDM SIO Close */
      case TM_CORE_NMEA_MSG_ID_SIO_OPEN_CB:
      tm_nmea_open_handler((void*)ipc_msg_ptr->p_Data);      
      break;


      /* SIO Close */
      case TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB_2:
        tm_nmea_open_handler_part2();
        break;

#endif


      /* Diag SIO Close */
      case TM_CORE_NMEA_MSG_ID_DIAG_SIO_CLOSE_CB_2:
        tm_nmea_diag_open_port_part2();
        break;

#ifdef FEATURE_RUNTIME_DEVMAP

      case TM_CORE_NMEA_MSG_ID_SIO_CLOSE_CB:
      tm_nmea_close_handler();      
      break;
      
#endif
#ifndef FEATURE_GNSS_LOW_MEMORY
      #ifdef FEATURE_CGPS_UMTS_CP_GSM
      case TM_CORE_RR_MSG_ID_EVENT_NTFY:
        tm_rr_iface_proc_event ((void *)ipc_msg_ptr->p_Data);
        break;
      #endif

      #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
      case TM_CORE_MSG_ID_RRC:
        tm_rrc_iface_proc_event((void *)ipc_msg_ptr->p_Data);
        break; 
      #endif

      #ifdef FEATURE_CGPS_UMTS_CP
      case TM_CORE_MSG_ID_MM:
        tm_mm_iface_proc_event((void *)ipc_msg_ptr->p_Data);
        break;
      #endif
#endif /* FEATURE_GNSS_LOW_MEMORY */
      #ifdef FEATURE_TM_SECURITY
      case TM_CORE_SEC_MSG_ID_CLNT_ACTIVATED:
      tm_sec_handle_clnt_activated();
      break;
      #endif 

      #ifdef FEATURE_CGPS_UMTS_CELLDB
      case TM_CORE_MSG_ID_MC_CELLDB_UPDATE:
        tm_core_cell_db_proc( (void *)ipc_msg_ptr->p_Data);
      break;
      #endif

      
      #ifdef FEATURE_USER_PLANE_MRL 
      
      case TM_CORE_MSG_ID_L1:
        tm_l1_iface_proc_event((void *)ipc_msg_ptr->p_Data);
        break;
        
      case TM_CORE_MSG_ID_LTE_L1:
      #ifdef FEATURE_CGPS_LTE_CELLDB
        tm_lte_l1_iface_proc_event((void *)ipc_msg_ptr->p_Data);
      #endif 
        break;
        
      #endif

      case TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT:
        //tm_core_on_demand_pos_punc_tunc_proc( (void*)ipc_msg_ptr->p_Data);
        tm_core_report_gps_state_info((void*)ipc_msg_ptr->p_Data);
      break;

      case TM_CORE_ON_DEMAND_MSG_ID_CHARGER_CONNECTED: 
        tm_core_on_demand_charger_connected();
      break;

      case TM_CORE_ON_DEMAND_MSG_ID_CHARGER_NOT_CONNECTED: 
        tm_core_on_demand_charger_disconnected();
        break;

      case TM_CORE_ON_DEMAND_MSG_ID_BATTERY_FULLY_CHARGED:
        tm_core_on_demand_charger_charged();
        break;

#ifdef FEATURE_GPSONE
#ifdef FEATURE_MMGSDI_SESSION_LIB
      case TM_CORE_MSG_ID_SIM_CARD_INSERTED:
        tm_gsdi_handle_sim_card_inserted_evt();
        break;

      case TM_CORE_MSG_ID_SIM_SUBSCRIPTION_READY:
        tm_gsdi_sim_handle_subscription_ready_evt((void*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_SIM_SESSION_CHANGED:
        tm_gsdi_handle_session_changed_evt();
        break;

      case TM_CORE_MSG_ID_SIM_SESSION_CLOSED:
        tm_gsdi_handle_session_closed_evt((void*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_MMGSDI_CLIENT_REG:
        tm_gsdi_handle_client_reg_cb();
        break;

      case TM_CORE_MSG_ID_MMGSDI_LBS_SRV_AVAILABLE:
        tm_gsdi_handle_lbs_svc_available();
        break;
#else
#ifndef FEATURE_GNSS_SA

      case TM_CORE_MSG_ID_SIM_INIT_COMPLETED:
        tm_gsdi_sim_init_handler();
        break;

      case TM_CORE_MSG_ID_MMGSDI_CDMA_SVC_INITIALIZED:
        tm_ruim_cdma_svc_init_report((void*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_MMGSDI_CMD_READ_CDMA_SVC_TBL:
        tm_ruim_mmgsdi_read_cmda_svc_table();
        break;
#endif /* #ifndef FEATURE_GNSS_SA */		
#endif /* FEATURE_MMGSDI_SESSION_LIB */

      case TM_CORE_MSG_ID_REFRESH_CARRIER_SPECIFIC_NV:
        tm_core_refresh_carrier_specific_nv((void*)ipc_msg_ptr->p_Data);
        break;
#endif /* FEATURE_GPSONE */

      case TM_CORE_MSG_ID_ODP_SESS_START:
        tm_core_handle_on_demand_sess_start((void*)ipc_msg_ptr->p_Data);
        break;
          
      case TM_CORE_MSG_ID_XSPI_INJECTION_REQUEST:
        tm_xspi_handle_injection_request((tm_xspi_request_type*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_XTM_CLIENT_TOKEN:
        tm_xtm_client_token_nv_write_status((void*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_XTM_SESSION_CONTROL:
        tm_xtm_session_control_nv_write_status((void*)ipc_msg_ptr->p_Data);
        break;
      case TM_CORE_GM_MSG_ID_ADD_RESPONSE:
      case TM_CORE_GM_MSG_ID_DELETE_RESPONSE:
      case TM_CORE_GM_MSG_ID_PURGE_RESPONSE:
      case TM_CORE_GM_MSG_ID_BREACH_NOTIFY:
      case TM_CORE_GM_MSG_ID_POS_REQ:
      case TM_CORE_GM_MSG_ID_CLIENT_BLOB_NOTIFY:
      case TM_CORE_GM_MSG_ID_CLIENT_GEOFENCE_HANDELS_NOTIFY: 
      case TM_CORE_GM_MSG_ID_GEOFENCE_HANDELS_NOTIFY:
      case TM_CORE_GM_MSG_ID_EDIT_NOTIFY:
      case TM_CORE_GM_MSG_ID_QUERY_NOTIFY:
      case TM_CORE_GM_MSG_ID_UNSOL_EVENT_NOTIFY:
      case TM_CORE_GM_MSG_ID_PRIO_POS_REQ_INDICATION_NOTIFY:
      case TM_CORE_GM_MSG_ID_SET_ENGINE_CONFIG_NOTIFY:
      case TM_CORE_GM_MSG_ID_ABORT_POS_REQ:
      case TM_CORE_GM_MSG_ID_BATCH_BREACH_NOTIFY:
      case TM_CORE_GM_MSG_ID_PROXIMITY_NOTIFY:
      case TM_CORE_GM_MSG_ID_TZ_REQ_NOTIFY:
  	  case TM_CORE_GM_MSG_ID_BATCH_DWELL_NOTIFY:
      case TM_CORE_GM_MSG_ID_SET_CLIENT_CONFIG_NOTIFY:
        tm_core_gm_evt_handler(ipc_msg_ptr->q_MsgId, (void *)ipc_msg_ptr->p_Data);
        break;
      case TM_CORE_MGP_WBIQ_INFO:
        tm_diag_wbiq_info_handler((sm_ReportWBIQInfoStructType *)ipc_msg_ptr->p_Data);
        break;
      case TM_CORE_DUMMY_GRIFFON_MSG:
        #ifdef __GRIFFON__
        tm_griffon_app_track_start();
        #endif /* __GRIFFON__ */
	      break;  

      case TM_CORE_SM_LOC_LTE_OTDOA_MEASUREMENTS:
        tm_core_otdoa_evt_handler((void*)ipc_msg_ptr->p_Data);
        break;

      #ifdef FEATURE_WCDMA_GPS_CELL_ID_READING
      case TM_CORE_MSG_ID_RRC_CELL_INFO_DCH_REPORT:
        tm_core_rrc_cell_info_dch_report_handler((tm_rrc_cell_info_dch_report_type*)ipc_msg_ptr->p_Data);
        break;
      #endif /*FEATURE_WCDMA_GPS_CELL_ID_READING*/        

      #if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
      case TM_CORE_MSG_ID_FTCAL_ENABLE:
        tm_is801_ftcal_enabled(TRUE);
        break;

      case TM_CORE_MSG_ID_FTCAL_DISABLE:
        tm_is801_ftcal_enabled(FALSE);
        break;
 
      case TM_CORE_MSG_ID_FTCAL_TT_DATA:
        tm_is801_ftcal_tt_data((cgps_FtcalTTStruct*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_FTCAL_PPM_DATA:
        tm_is801_ftcal_ppm_data((cgps_FtcalPpmStruct*)ipc_msg_ptr->p_Data);
        break;
#endif
      case TM_CORE_MSG_ID_RESET_LOCATION_SERVICE_DONE:
        tm_core_reset_location_service_done_handler((pdsm_pd_ext_reset_location_service_status_e_type*)ipc_msg_ptr->p_Data);
        break;
      case TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT:
        (void)tm_core_handle_engine_error_recovery_report((gnss_engine_err_recovery_rpt*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT:
        (void)tm_core_handle_engine_dpo_status_report((sm_DpoStatusReportStructType*)ipc_msg_ptr->p_Data);
        break;

	  case TM_CORE_MSG_ID_LSMP_GET_PARAM:
	  	(void)tm_core_get_param_msg_handler((void*)ipc_msg_ptr->p_Data);
        break;	

	  case TM_CORE_MSG_ID_LSMP_SEND_PREM_SVC_IND_STATUS:
	  	(void)tm_core_handle_prem_svc_status((void*)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_NAS_DL_DATA_EVENT:
      case TM_CORE_MSG_ID_NAS_FAILURE_EVENT:
        tm_core_lpp_cp_evt_handler(ipc_msg_ptr->q_MsgId, (void*)ipc_msg_ptr->p_Data);
        break;

	  case TM_CORE_MSG_ID_API_REGISTRATION:
	  	(void)tm_core_api_callback_handler((tm_api_notifier_type*)ipc_msg_ptr->p_Data);
		break;	

     case TM_CORE_GNSS_LPP_ECID_MEASUREMENTS:
       tm_core_ecid_evt_handler((void*)ipc_msg_ptr->p_Data);
       break;

      case TM_CORE_MSG_ID_RETRIEVE_IMSI:
        tm_core_retrieve_imsi((tm_RetrieveIMSIType *)ipc_msg_ptr->p_Data);
        break;

      case TM_CORE_MSG_ID_WIFI_FIX_RESP:
        tm_core_handle_wifi_fix_response((tm_wifi_fix_response_type*)ipc_msg_ptr->p_Data);
        break;

      case C_OS_MSG_ID_DOG_HB:
      {
        // Respond to the dog heart beat. Pat the dog. 
        os_DogHeartbeatReport();
        break;
      }

      default:
        MSG_ERROR("Unknown MSG ID",0,0,0);
        break;
    } /* End switch MsgId */

    (void)os_IpcDelete(ipc_msg_ptr);
  } /* End receive */
  
} /* End tm_Proc() */

#ifdef __GRIFFON__
void tm_griffon_app_track_start()
{
   pdsm_pd_option_s_type    option_data; 
   pdsm_pd_qos_type         qos_data;
     
   memset(&option_data, 0, sizeof(option_data));  
   memset(&qos_data, 0, sizeof(qos_data));  

  option_data.session = PDSM_PD_SESS_TYPE_NEW;       /* PD option type */
  option_data.operation_mode = PDSM_SESSION_OPERATION_STANDALONE_ONLY; /* operation mode */
  option_data.fix_rate.num_fixes = 100;      /* Fix rate for tracking sessions. */
  option_data.fix_rate.time_between_fixes = 1;      /* Fix rate for tracking sessions. */

  qos_data.accuracy_threshold = 50;
  qos_data.gps_session_timeout = 45;
  num_sessions++;
  //printf("Starting session %d  \n", num_sessions);
  (void)pdsm_get_position(NULL,
                          NULL, /* user data pointer */
                          &option_data,
                          &qos_data,
                          griffon_pdsm_client_id);
}


void griffonSendMsg (void)
{
  os_IpcMsgType* p_Msg;


  p_Msg = os_IpcCreate(0, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if (p_Msg)
  {
    p_Msg->q_MsgId = TM_CORE_DUMMY_GRIFFON_MSG;
    p_Msg->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    p_Msg->q_Size           = 0;

    if (os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_TM))
    {
      MSG_MED("GPSDIAG IpcMsg %d sent.", p_Msg->q_MsgId, 0, 0);
    }
    else
    {
      MSG_ERROR("GPSDIAG IpcMsg %d NOT sent!!", p_Msg->q_MsgId, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("GPSDIAG IpcMsg is NULL!", 0, 0, 0);
  }

}

void griffon_DoneEventCb (const void *p_DataBlock, pdsm_pd_event_type u_PdEvent,
                          const pdsm_pd_info_s_type* p_PdInfo)
{
  
  //printf("Done event received %d \n", (uint32)u_PdEvent);	  

  if (u_PdEvent == PDSM_PD_EVENT_DONE)
  {
    griffonSendMsg();
  }

}

/*lint -e818 Pointer parameter could be declared as pointing to const */
void griffon_gpsdiag_PdEventsDispatchCb (void* p_DataBlock, pdsm_pd_event_type u_PdEvent,
                                 const pdsm_pd_info_s_type* p_PdInfo)
{
   //pdsm_client_type_e_type e_ClientType;

   if(p_PdInfo == NULL)
   {
      //printf("gpsdiag_PdEventsDispatchCb: No Valid Information");
      return;
   }

   //printf("PD event received %d \n", u_PdEvent);	  

   if(u_PdEvent & (PDSM_PD_EVENT_POSITION | PDSM_PD_EVENT_VELOCITY | PDSM_PD_EVENT_HEIGHT))

   {
      //printf("Position event received \n");	  
   }
   else if(u_PdEvent & (PDSM_PD_EVENT_DONE | PDSM_PD_EVENT_DLOAD_DONE |
                        PDSM_PD_EVENT_COMM_DONE | PDSM_PD_EVENT_GPS_DONE))
   {
     griffon_DoneEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
   }
//   else if(u_PdEvent & (pdsm_pd_event_type)(PDSM_PD_EVENT_END | PDSM_PD_DLOAD_EVENT_END))
//   {
//      gpsdiag_EndEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
//   }
//   else if(u_PdEvent & (PDSM_PD_EVENT_BEGIN | PDSM_PD_EVENT_DLOAD_BEGIN |
//                        PDSM_PD_EVENT_COMM_BEGIN | PDSM_PD_EVENT_GPS_BEGIN))
//   {
//      gpsdiag_StartEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
//   }
//   else if(u_PdEvent & (PDSM_PD_EVENT_UPDATE_FAILURE | PDSM_PD_EVENT_COMM_FAILURE))
//   {
//      gpsdiag_FailureEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
//   }
//   else if(u_PdEvent & (PDSM_PD_EVENT_COMM_CONNECTED))
//   {
//      gpsdiag_ConnectedEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
//   }
//   else if(u_PdEvent & (PDSM_PD_EVENT_DLOAD))
//   {
//      gpsdiag_DloadOccurredEventCb(p_DataBlock, u_PdEvent, p_PdInfo);
//   }
} /* griffon_gpsdiag_PdEventsDispatchCb */
//
//
//#define GPSDIAG_ERROR_VALUE     (-1)
//#define XTRA_BUF_SIZE (2048)
//
//
//#include "Orion.h"
//
//
///*===========================================================================*/
void tm_griffon_test_client_init(void)
{
   // 'Get position'-client  

       
  griffon_pdsm_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_TEST2);

  if ( griffon_pdsm_client_id < 0 ) {
    printf("The dummy client cannot be created");
  }
  else if ( PDSM_CLIENT_OK != pdsm_client_act(griffon_pdsm_client_id) ){
    printf("The dummy client cannot be activated");
  }

  (void)pdsm_client_pd_reg(griffon_pdsm_client_id,
                           NULL,
                           griffon_gpsdiag_PdEventsDispatchCb,
                           PDSM_CLIENT_EVENT_REG,
                           (pdsm_pd_event_type)PDSM_PD_EVENT_MASK,
                           NULL);
  griffonSendMsg();

} /* end tm_griffon_test_client_init */
#endif /* __GRIFFON__ */

/*===========================================================================
FUNCTION tm_task_init_notifier_cb

DESCRIPTION
  Callback function registered for task init notification
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void tm_task_init_notifier_cb(uint32 thread_id)
{
  if(thread_id == THREAD_ID_SM_GM)
  {
    tm_task_wait_mask &= ~(TM_WAIT_GM_TASK_MASK);  
  }

  if(tm_task_wait_mask == 0)
  {     
    pdsm_set_ready_flag();
  }
}

/*===========================================================================
FUNCTION SM_TM_TASK

DESCRIPTION
  Use pdsmsclient_task for now, change to tm_Task later
  
RETURN VALUE

DEPENDENCIES
=============================================================================*/
void sm_tm_task(
  dword dummy                     /* Rex requirement, not used */
)
{
  (void)dummy;
    
  MSG_HIGH("Starting TM task init...",0,0,0);

  /* Set up IPC and timers */
  tm_start_up();

  os_TaskReadyAck();
  os_TaskStopRegister();

  /* TM module initialization */
  tm_module_init();
  

#if defined(FEATURE_NO_MODEM)
  /* in no modem builds, there is no pdcomms, hence we need to init gpsdiag here */
  gpsdiag_init();
#endif

#ifdef __GRIFFON__
//  tm_griffon_test_client_init();
#endif /* __GRIFFON__ */
  

  /* set global flag indicating PDAPI is initialized */
  tm_task_wait_mask = 0;

  if(gm_register_task_init_notifier(tm_task_init_notifier_cb, THREAD_ID_SM_GM))
  {
    tm_task_wait_mask |= TM_WAIT_GM_TASK_MASK;
  }

  /* set global flag for NV OEM features:
     indicating if PDSM should unconditionally accept notification-verification */
  pdsm_set_gnss_oem_feature_mask(tm_core_info.config_info.q_gnss_oem_feature_mask);


  tm_proc();
 
} /* End tm_Task() */

