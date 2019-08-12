/*======================================================================

               LM task main file

 GENERAL DESCRIPTION
  This file contains the implementation of Location Manager(LM) task.
  This task handles requests from various tasks like TM, MGP, GPS DIAG. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/lm/src/lm_task.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/11/14   jv      Added Constellation configuration support
10/28/13   hm      Added Rf dev test memory release handler call at the end of RF dev test event report
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos) 
02/08/13   ssu     Move to the DOG Heartbeat mechanism on DIME and beyond. 
02/10/10   vp      OEM DR Data Enablement
06/24/08   ank     Reinstate dynamic hepe logic for tunnel-exit scenario.
06/18/08   jlp     Added messaging support so tm_diag can set the Sv No Exist
                    list in cd.
08/01/07   rw      Add GPS time info support
07/31/07   ank     Added Dynamic QoS and Override logic.
07/23/07   jw      By default should not disable Jammer Detect in MGP.
12/19/06   br      Clean-up.
10/11/06   br/mr   Removed include pdsmi.h.
04/04/06   mr      Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "msg.h"
#include "task.h"
#define LM_DATA_DECLARE 
#include "lm_data.h"  /*Don't move this declaration down*/
#undef LM_DATA_DECLARE
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "lm_iface.h"
#include "lm_tm.h"
#include "lm_mgp.h"
#include "lm_mgp_aflt.h"
#include "lm_diag.h"
#include "sm_api.h"
#include "queue.h"
#include "event.h"
#include "sm_nv.h"
#include "sm_oemdrsync.h"
#include "sm_nv_efs.h"
#include "mgp_api.h"
#include "sm_util.h"

#ifdef FEATURE_GPSONE_PPM
/* Typedef union of all messages from PDSM to Searcher.
*/
typedef union srch_pdsm_msg_u {
    srch_pdsm_cmd_hdr_type          hdr;
    srch_pdsm_start_ppm_cmd_type    start_ppm;
    srch_pdsm_get_ppm_rslt_cmd_type get_ppm_results;
    srch_pdsm_end_ppm_cmd_type      end_ppm;
} srch_pdsm_msg_u_type;

typedef union pdsm_srch_all_msg_u{
    q_link_type              link;
    pdsm_srch_msg_u_type     srch_to_pdsm;
    srch_pdsm_msg_u_type     pdsm_to_srch;
} pdsm_srch_all_msg_u_type;
#endif /* FEATURE_GPSONE_PPM */

/* Bacward compatibility with AFLT
*/
q_type          pdsm_srch_msg_free_q;  

/****************** Variables ************************************************/
#ifdef FEATURE_GPSONE_PPM
pdsm_srch_all_msg_u_type z_afltSrchMsgBuffers[5];
#endif

static volatile enum { LM_TASK_STATE_RUN, LM_TASK_STATE_STOP } e_lm_task_state;

/****************** Function Definitions**************************************/

/*
 ******************************************************************************
 * lm_startup
 *
 * Function description:
 *
 * Contains task initialization code.
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
static void lm_startup( void )
{
/*Call all the initialization code here*/
 boolean ret = os_IpcStart((uint32)THREAD_ID_SM_LM);

 if(ret == FALSE)
 {
   MSG_ERROR("PDCOMM APP: IPC Start Failed",0,0,0);
 }

 os_TaskReadyAck();
 os_TaskStopRegister();

  /* Register for DOG heartbeat monitoring */
  os_DogHeartbeatRegister();
} /*lm_startup() ends*/

/*===========================================================================

FUNCTION lm_read_nv

DESCRIPTION
  This function reads the following LM NV items:
  1. CGPS Application-Based Tracking GPS-On Threshold 
  2. CGPS Application-Based Tracking GPS-Idle Threshold
  3. CGPS Application-Based Tracking Periodic Request Delay Margin
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lm_read_nv(void)
{
  /* NV buffer, used to read NV items */
  nv_item_type      z_nv_read_buffer;
  uint32			hepe_threshold = 0;  
  
  /* CGPS Application-Based Tracking GPS-On Threshold  */
  
  if(sm_nv_read(NV_AAGPS_APP_TRACKING_GPSON_THSLD_I, (void *)&z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRACKING_GPSON_THSLD_I(sec) = %d",
               (uint32)z_nv_read_buffer.aagps_app_tracking_gpson_thsld,0,0);  
    lm_set_rc_on_threshold_period((uint32)z_nv_read_buffer.aagps_app_tracking_gpson_thsld*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRACKING_GPSON_THSLD_I setting default(sec) = %d",
                 LM_MAX_RC_ON_THRESHOLD/1000,0,0);
    lm_set_rc_on_threshold_period(LM_MAX_RC_ON_THRESHOLD);
  }

  /* CGPS Application-Based Tracking GPS-Idle Threshold*/
  if(sm_nv_read(NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I, (void *)&z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I(sec) = %d",
               (uint32)z_nv_read_buffer.aagps_app_tracking_gpsidle_thsld,0,0);    
    lm_set_rc_idle_threshold((uint32)z_nv_read_buffer.aagps_app_tracking_gpsidle_thsld*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I setting default(sec) = %d",
                 LM_MAX_RC_IDLE_THRESHOLD/1000,0,0);
    lm_set_rc_idle_threshold(LM_MAX_RC_IDLE_THRESHOLD);
  }

  /* CGPS Application-Based Tracking Periodic Request Delay Margin*/
  if(sm_nv_read(NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I, (void *)&z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I(sec) = %d",
               (uint32)z_nv_read_buffer.aagps_app_trkg_periodic_req_dly_margin,0,0);      
    lm_set_rc_shut_off_delay_margin((uint32)z_nv_read_buffer.aagps_app_trkg_periodic_req_dly_margin*1000);
  }
  else
  {
    LM_MSG_ERROR("Failed reading NV NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I setting default(sec) = %d",
                 LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT/1000,0,0);
    lm_set_rc_shut_off_delay_margin(LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT);
  }
  /* CGPS QoS_Override_Threshold */
  if(sm_nv_read(NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I, (void *)&z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I(m) = %d",
               (uint32)z_nv_read_buffer.cgps_qos_override_acc_threshold,0,0);      
    z_lmControl.z_early_exit_decision.w_QoSThresholdOverride = z_nv_read_buffer.cgps_qos_override_acc_threshold;
  }

  /* CGPS QoS_Override_Timeout */
  if(sm_nv_read(NV_CGPS_QOS_OVERRIDE_TIME_I, (void *)&z_nv_read_buffer))
  {
    LM_MSG_MED("Read NV NV_CGPS_QOS_OVERRIDE_TIME_I(s) = %d",
               (uint32)z_nv_read_buffer.cgps_qos_override_time,0,0);      
    z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride = z_nv_read_buffer.cgps_qos_override_time * 1000;
  }

  /* OEM DRE Data Enablement */
  if(sm_nv_read(NV_GNSS_OEM_FEATURE_MASK_I, (void *)&z_nv_read_buffer))
  {
	  LM_MSG_MED("Read NV NV_GNSS_OEM_FEATURE_MASK = %d",
			  (uint32)z_nv_read_buffer.gnss_oem_feature_mask,0,0);
	  z_lmControl.q_gnss_oem_feature = z_nv_read_buffer.gnss_oem_feature_mask;
  }
  
  /* Read HEPE Max threshold*/
  sm_nv_init();
  sm_nv_efs_init();
  (void)sm_nv_efs_reg_item_read(GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD, 
								sizeof(uint32),
								&hepe_threshold);
  /* Validate for boundary limits*/
  if( hepe_threshold <= GNSS_NV_EFS_SM_LM_MAX_HEPE_THRESHOLD && hepe_threshold >= GNSS_NV_EFS_SM_LM_MIN_HEPE_THRESHOLD)
  {
      z_lmControl.q_HepeThreshold = hepe_threshold;
  }
  else
  {	
	  /* if out of boundary limits, force it to default value */
      LM_MSG_ERROR("NV item has out of limits value = %d, hence setting default NV NV_GNSS_LM_HEPE_THRESHOLD = %d ",(int32) hepe_threshold, (int32)GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT,0);
	  z_lmControl.q_HepeThreshold = GNSS_NV_EFS_SM_LM_HEPE_THRESHOLD_DEFAULT;
  }

  sm_init_constellation_config();
  
}

/*
 ******************************************************************************
 * lm_task_state_init
 *
 * Function description:
 *
 * Initializes the variables in z_lmControl structure
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_task_state_init( void )
{
  lm_tm_clear_session();
  z_lmControl.e_afltState = LM_MGP_AFLT_STATE_OFF;
  z_lmControl.e_currentMgpState =  LM_MGP_STATE_OFF;
  z_lmControl.e_desiredMgpState =  LM_MGP_STATE_OFF;
  memset(&z_lmControl.z_cdma_request,0,sizeof(z_lmControl.z_cdma_request));
  memset(&z_lmControl.z_fix_request,0,sizeof(z_lmControl.z_fix_request));
  memset(&z_lmControl.z_gps_request,0,sizeof(z_lmControl.z_gps_request));
  /*Set MGP maintenance related thresholds and timeouts to default values*/
  lm_read_nv();
  z_lmControl.u_searchMode = 0x10; /*Set default GPS SRCH mode: High */ 
  z_lmControl.u_DisableJammerDetect = FALSE; /* Default is to not disabe Jammer Detection*/    
  z_lmControl.z_utcData.u_utcModelValid = FALSE;
  z_lmControl.u_ppm_request_pending = FALSE;
  z_lmControl.u_ppm_get_results_pending = FALSE;
  z_lmControl.u_oemdre_onoff = FALSE; /*OEM DRE Enablement flag*/

  z_lm_sarf_state_db.e_client_type = LM_SARF_CLIENT_DIAG;
  z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_SLEEP;
  /*MRAWAL: read UTC data from EFS*/

  if((z_lmControl.z_early_exit_decision.w_QoSThresholdOverride) &&
     (z_lmControl.z_early_exit_decision.q_QoSTimeoutOverride))
  {
    z_lmControl.z_early_exit_decision.u_UseQoSOverride = TRUE;
  }
  else
  {
  z_lmControl.z_early_exit_decision.u_UseQoSOverride = FALSE;
  }
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterEngineOn = FALSE;
  z_lmControl.z_early_exit_decision.u_FirstValidFixAfterSessStart = FALSE;
  qw_set(z_lmControl.z_early_exit_decision.d_TimeEngOn,0,0);
  qw_set(z_lmControl.z_early_exit_decision.d_TimeSessionStart,0,0);
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterEngineOn = 0;
  z_lmControl.z_early_exit_decision.q_TimeOfFirstValidFixAfterSessStart = 0;
  z_lmControl.z_early_exit_decision.f_dynamic_hepe_limit = 0;
  z_lmControl.z_early_exit_decision.u_FirstReportedFixAfterEngineOn = FALSE;
  z_lmControl.z_early_exit_decision.f_LastValidFixHepe = LM_LAST_VALID_FIX_HEPE_MAX;
  z_lmControl.z_early_exit_decision.q_NullFixReportCount = 0;
  z_lmControl.z_early_exit_decision.u_ReinstateDynamicHepe = FALSE;

  z_lmControl.u_FTCalEnabled = FALSE;
}

/*
 ******************************************************************************
 * lm_create_timers
 *
 * Function description:
 *
 * Creates the CDMA, FIX and GPS session timers.
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 ******************************************************************************
*/
void lm_create_timers(void)
{
  /*Create session timers*/
  z_lmControl.p_fixTimer  = os_TimerCreateInCgpsTimerGroup((uint32)LM_FIX_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_cdmaTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_CDMA_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_gpsTimer  = os_TimerCreateInCgpsTimerGroup((uint32)LM_GPS_TIMER_ID,(uint32)THREAD_ID_SM_LM);

  /*Create MGP state timers*/
  z_lmControl.p_goingONTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_ON_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_goingOFFTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_OFF_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_goingIDLETimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_GOING_IDLE_TIMER_ID,(uint32)THREAD_ID_SM_LM);

  /*Create App based tracking related MGP state timers*/
  z_lmControl.p_rcOnTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_RC_ON_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_mgpShutOffTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_MGP_SHUT_OFF_TIMER_ID,(uint32)THREAD_ID_SM_LM);
  z_lmControl.p_IqTestingTimer = os_TimerCreateInCgpsTimerGroup((uint32)LM_IQ_TESTING_TIMER_ID,(uint32)THREAD_ID_SM_LM);
}

#ifdef FEATURE_GPSONE_PPM
/*===========================================================================
FUNCTION pdsm_srch_msg_free_q_init

DESCRIPTION
  This function is init the message free q for AFLT engine interactions.

RETURN VALUE
  None

DEPENDENCIES
 Works with old PDSM-SRCH interface
=============================================================================*/
void  pdsm_srch_msg_free_q_init( void )
{
  int i;
  pdsm_srch_all_msg_u_type *free_buffer = NULL;

  // Sanjeev using same free q as defined by pdsmtask.c
  // This is done so that AFLT code is not touched...
  
  (void)q_init( &pdsm_srch_msg_free_q );

  for(i=0; i < 5; i++ )
  {
    free_buffer = &z_afltSrchMsgBuffers[i];
    
    (void)q_link( &free_buffer, &free_buffer->link );
    q_put( &pdsm_srch_msg_free_q, &free_buffer->link );
    
  } /* End for FREE Q size */
} /* End pdsm_srch_msg_free_q_init() */
#endif

/*
 ******************************************************************************
 * lm_handle_stop
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
void lm_handle_stop( void )
{
  LM_MSG_HIGH("TASK STOP received",0,0,0);

  /* Deregister with the Dog Heartbeat before stopping task */
  os_DogHeartbeatDeregister();

  os_TaskStopAck();
}

/*
 ******************************************************************************
 * lm_handle_offline
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
void lm_handle_offline( void )
{

  LM_MSG_HIGH("TASK OFFLINE received",0,0,0);

}


/*
 ******************************************************************************
 * lm_proc
 *
 * Function description:
 *
 * This function handles the all the incoming IPC messages. It will call the
 * appropriate function based on the type of IPC message received.
 *
 * Parameters: 
 *
 * *msg - Pointer to received IPC message 
 *
 * Return value: 
 *
 ******************************************************************************
*/
static void lm_proc( os_IpcMsgType* p_msg )
{
  /*lm_task calls this function whenever it recieves a message*/

  if( p_msg == NULL )
  {
      LM_MSG_MED(" Received NULL IPC Message ",0,0,0);
      return;
  }
   
  switch(p_msg->q_MsgId)
  {
    case C_SM_LM_MSG_SESSION_REQUEST:
    {
      lm_tm_handle_session_request(p_msg);
      break;
    }
    
    case C_SM_LM_MSG_REQUEST:
    {
      lm_tm_handle_request(p_msg);
      break;
    }
    
    case C_SM_LM_MSG_INFO:
    {
      lm_tm_handle_info(p_msg);
      break;
    }

    /* Handle timer expiry messages*/
    case C_OS_MSG_ID_TIMER_EXPIRY:
    { 
       /*lint -e826 */              
      switch((lm_timer_ids_e_type)((os_TimerExpiryType*)&p_msg->p_Data[0])->q_TimerId)
      {
        case LM_FIX_TIMER_ID:
        {
          lm_tm_handle_fix_timer_timeout();
          break;
        }
        case LM_GPS_TIMER_ID:
        {
          lm_tm_handle_gps_timer_timeout();
          break;
        }
	#ifdef FEATURE_CGPS_USES_CDMA
        case LM_CDMA_TIMER_ID:
        {
          lm_tm_handle_cdma_timer_timeout();
          break;
        }
	#endif /* FEATURE_CGPS_USES_CDMA */
        case LM_GOING_ON_TIMER_ID:
        {
          lm_tm_handle_goingOn_timer_timeout();
          break;
        }
        case LM_GOING_OFF_TIMER_ID:
        {
          lm_tm_handle_goingOff_timer_timeout();
          break;
        }
        case LM_GOING_IDLE_TIMER_ID:
        {
          lm_tm_handle_goingIdle_timer_timeout();
          break;
        }
        case LM_RC_ON_TIMER_ID:
        {
          lm_mgp_handle_rcOn_timer_timeout();
          break;
        }
        case LM_MGP_SHUT_OFF_TIMER_ID:
        {
          lm_mgp_handle_shutOff_timer_timeout();
          break;
        }
        case LM_IQ_TESTING_TIMER_ID:
        {
          LM_MSG_HIGH("Iqtest testing timer expired", 0, 0, 0);
          break;
        }
        default:
        {
          LM_MSG_ERROR("Invalid timer id %d in timeout message",
                       ((os_TimerExpiryType*)&p_msg->p_Data[0])->q_TimerId,0,0); /*lint -e826 */ 
        }

      }/*switch(*((lm_timer_ids_e_type*)p_msg->p_Data) ends*/
      break;
    }

    /*Handle MGP messages*/

      /*****************************************************
      ** MSG ID's: MGP Info reports MGP => LM
      ******************************************************/
   case ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS:
   {
      switch(((sm_GpsMeasRptStructType*) p_msg->p_Data)->e_MeasRptReason)
      {
      case GPS_MEAS_RPT_MEAS_DONE:
         {
            if( z_lmControl.z_gps_request.multi_report_msa )
            {
               LM_MSG_HIGH("done-meas, don't stop MC: periodic", 0, 0, 0 );

               /* if multiple MSA rept, don't stop; TM to stop this when done */
               lm_mgp_handle_gps_meas(p_msg);
            }
            else
            {
               LM_MSG_HIGH("done-meas, stop MC: non-periodic", 0, 0, 0 );

               /* regular case, stop MGP now */  
               lm_mgp_handle_gps_meas_done(p_msg);
            }
         } /* end case GPS_MEAS_RPT_MEAS_DONE */
         break;
      case GPS_MEAS_RPT_PERIODIC:
         {
            lm_mgp_handle_gps_meas(p_msg);  
         } /* end case GPS_MEAS_RPT_PERIODIC*/
         break;

      default:
         LM_MSG_ERROR("unhandled meas report type [%d] \n", 
                      ((sm_GpsMeasRptStructType*) p_msg->p_Data)->e_MeasRptReason, 0, 0);
         break;
      }   /* end switch((sm_GpsMeasRptStructType*) p_msg->p_Data)->e_MeasRptReason) */
   } /* end case ARIES_MSG_ID_MGP_LM_REPORT_GPS_MEAS */
   break;
    
    case ARIES_MSG_ID_MGP_LM_REPORT_FIX:
    {
      lm_mgp_handle_fix_report(p_msg);
      break;
    }  
    
    case ARIES_MSG_ID_MGP_LM_REPORT_POS_ESTIMATE:
    {
      lm_mgp_handle_pos_estimate_report(p_msg);
      break;
    }            
    
    case ARIES_MSG_ID_MGP_LM_REPORT_UTC_INFO:
    {
        
      lm_mgp_handle_utc_info((sm_UtcInfoRptStructType *)(p_msg->p_Data));
      break;
    }

    case MSG_ID_MGP_LM_REPORT_SV_POLY:
    {
      lm_mgp_handle_sv_poly_report(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: MGP status reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_ASSISTANCE_STATUS:
    { 
      lm_mgp_handle_assist_status_report(p_msg);  
      break;
    }
    
    case ARIES_MSG_ID_MGP_LM_REPORT_GENERAL_STATUS:
    {
      lm_mgp_handle_status_update(p_msg);
      break;
    }
    
    case ARIES_MSG_ID_MGP_LM_REPORT_EVENT:
    {
      /*MRAWAL: need to handle event reports from MGP*/
      switch( ((sm_ReportEventStructType*)p_msg->p_Data)->e_EventType )
      {
        case SM_EVENT_ME_IQ_DONE:
         event_report( EVENT_GPS_LM_IQ_TEST_COMPLETE );  
#ifdef FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION
         
         /* Reported the end of  IQ test, this is a good time to call IQ test memory release handler before another
                   RF dev test or mission mode command is received,as this is converging point of  several event reports from ME*/
          mgp_IqTestCompleteMemRelHandler(); 
#endif
        break;
      
        case SM_EVENT_PE_NEW_NAVDATA:
        case SM_EVENT_PE_NEW_ALMANAC:
        case SM_EVENT_PE_DEL_NAVDATA:
      case SM_EVENT_PE_DEL_ALMANAC:
      (void)lm_tm_send_mgp_event_to_tm((sm_ReportEventStructType*)(p_msg->p_Data)); 
      break;
      
      case SM_EVENT_PE_NEW_IONO_UTC:
        lm_tm_send_mgp_event_to_tm((sm_ReportEventStructType*)(p_msg->p_Data)); 
        break;

        default:
          MSG_MED("Unknown MGP Event %d\n",  ((sm_ReportEventStructType*)p_msg->p_Data)->e_EventType , 0, 0);
          break;
      }
      break;
    }

    /***************************************************
    ** MSG ID's: PGI RF Info reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_RF_INFO:
    {
      lm_mgp_handle_rf_info(p_msg);  
      break;  
    }

    /***************************************************
    ** MSG ID's: MC WBIQ  Info reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_WB_IQ_PARAMS:
    {
      lm_mgp_handle_wbiq_info(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: MC NBIQ info reports MGP => LM
    ****************************************************/
    case SM_MSG_ID_MGP_LM_REPORT_NBIQ_PARAMS:
    {
      lm_mgp_handle_nbiq_info(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: MC GNSS RF status info reports MGP => LM
    ****************************************************/
    case SM_MSG_ID_MGP_LM_REPORT_GNSS_RF_STATUS_INFO:
    {
      lm_mgp_handle_gnss_rf_status_info(p_msg);
      break;
    }

    /***************************************************
    ** MSG ID's: GPS time reports MGP => LM
    ****************************************************/
    case ARIES_MSG_ID_MGP_LM_REPORT_GPS_TIME:
    {
      lm_mgp_handle_gps_time(p_msg);  
      break;  
    }

    /***************************************************
    ** MSG ID's: MGP data request MGP => LM
    ****************************************************/    
     case ARIES_MSG_ID_MGP_LM_REQ_SA_DATA:
     {
       lm_mgp_handle_request_sa_data();  
       break;
     }
    #ifdef FEATURE_CGPS_USES_CDMA
    /***************************************************
    ** MSG ID's: CDMA reports AFLT => LM
    ****************************************************/
    case ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS:
    {
      break;
    }
    
    case ARIES_MSG_ID_AFLT_LM_REPORT_STATUS:
    {
      break;
    }
    
    case ARIES_MSG_ID_AFLT_LM_REPORT_PPM_MEAS_IS801:
    {
      lm_mgp_aflt_handle_ppm_meas_report(p_msg);
      break;
    }
    #endif /*FEATURE_CGPS_USES_CDMA*/
    /*Handle DIAG messages*/
    case C_SM_LM_DIAG_GSC_MSG:
    {
      lm_diag_handle_diag_gsc_msg(p_msg);
      break;
    }
    
    case C_SM_LM_SV_NO_EXIST:
    {
      const lm_sv_no_exist_cmd_type * cpz_SvNoExistCmd
       = (const lm_sv_no_exist_cmd_type *)p_msg->p_Data;
      (void) cpz_SvNoExistCmd;
      LM_MSG_MED("Sv No Exist cmd rcvd from TM diag %08X, %u\n",
       cpz_SvNoExistCmd->q_SvNoExistMask, cpz_SvNoExistCmd->e_Action, 0);
      lm_SvNoExistCmd_handler( p_msg ); /* Sv No Exist command handler */
      break;
    }

    case C_SM_LM_FORCE_DPO_FOR_POWER_MEAS:
    {
      const lm_force_dpo_for_power_meas_cmd_type * cpz_ForceDpoForPowerMeasCmd
       = (const lm_force_dpo_for_power_meas_cmd_type *)p_msg->p_Data;
      (void) cpz_ForceDpoForPowerMeasCmd;
      LM_MSG_MED("Force DPO for Power Meas cmd rcvd from TM diag %u\n",
       cpz_ForceDpoForPowerMeasCmd->u_ForceDpoForPowerMeas, 0, 0);
      lm_ForceDpoForPowerMeasCmd_handler( p_msg ); /* Force DPO for Power Meas command handler */
      break;
    }

    /*Handle task maintenance messages*/
    case C_OS_MSG_ID_OFFLINE: 
    {
      lm_handle_offline();
      break;
    }
    
    case C_OS_MSG_ID_STOP: 
    {
      lm_handle_stop();
      break;
    }

    case ARIES_MSG_ID_TM_LM_PRESC_DWELL:
    {
      LM_MSG_MED("Presc dwell cmd rcvd from TM diag\n",0,0,0);
      lm_presc_dwell_handler( p_msg ); /* Prescribed dwell command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_SARF_MODE_SWITCH:
    {
      LM_MSG_MED("SARF mode-switch IPC rcvd from TM diag\n",0,0,0);
      lm_Rf_VerifModeSwitch_handler( p_msg ); /* Prescribed dwell command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_IQ_TEST:
    {
      LM_MSG_MED("IqTest cmd rcvd from TM diag\n",0,0,0);
      lm_IqTestCmd( p_msg ); /* IQ Test command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_1SV_STANDALONE:
    {
      LM_MSG_MED("Standalone-1SV cmd rcvd from TM diag\n",0,0,0);
      lm_StandaloneTestCmd_handler( p_msg ); /* Standalone Test command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RFDEV_TEST:
    {
      LM_MSG_MED("RF Dev Test cmd rcved from TM diag\n",0,0,0);
      lm_RfDevTestCmd_handler(p_msg);
      break;
    }

    case ARIES_MSG_ID_TM_LM_MEM_BW_TEST:
    {
      LM_MSG_MED("Mem BW Test cmd rcvd from TM diag\n",0,0,0);
      lm_MemBwTestCmd_handler( p_msg ); /* Memory bandwidth Test command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_TSG_CONFIG:
    {
      LM_MSG_MED("TSG Config cmd rcvd from TM diag\n",0,0,0);
      lm_DiagTsgCmd( p_msg ); /* TSG config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RF_CONFIG:
    {
      LM_MSG_MED("GNSS RF Config cmd rcvd from TM diag\n",0,0,0);
      lm_DiagGnssRfCmd( p_msg ); /* RF config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_PRX_RF_CONFIG:
    {
      LM_MSG_MED("GNSS Prx RF Config cmd rcvd from TM diag\n",0,0,0);
      lm_DiagGnssPrxRfCmd( p_msg ); /* RF config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_RF_LINEARITY_CONFIG:
    {
      LM_MSG_MED("GNSS RF Linearity Config cmd rcvd from TM diag\n",0,0,0);
      lm_DiagGnssRfLinearityCmd( p_msg ); /* RF Linearity config command handler */
      break;
    }

    case ARIES_MSG_ID_TM_LM_SPECTRUM_ANALYZER_CMD:
    {
      LM_MSG_MED("Spectrum Analyzer cmd rcvd from TM diag\n",0,0,0);
      lm_spectrum_analyzer_handler( p_msg ); /* Spectrum Analyzer command handler */
      break;
    }

    case C_SM_LM_SBAS_START:
    {
      LM_MSG_MED("TM diag - starting SBAS test",0,0,0);
      lm_DiagSbasStart( (mgp_ConfigStructType *)(p_msg->p_Data) );
      break;
    }

    case C_SM_LM_SBAS_STOP:
    {
      LM_MSG_MED("TM diag - stopping SBAS test",0,0,0);
      lm_DiagSbasStop();
      break;
    }

    case SM_MSG_ID_MGP_LM_DRSYNC_CONFIG:
      /* TBD: send to diag task also */
      oemDrSyncConfig((gnss_DRSyncConfigType*)(p_msg->p_Data));
      break;

    case SM_MSG_ID_MGP_LM_DRSYNC_PULSE_DESC:
      /* TBD: send to diag task also */
      oemDrSyncPulseDescriptor((gnss_DRSyncPulseDescType*)(p_msg->p_Data));
      break;

    case SM_MSG_ID_MGP_LM_DEBUG_NMEA:
    {
      lm_debugNMEA(p_msg);
      break;
    }

    case SM_MSG_ID_MGP_LM_REPORT_XTRA_INJ_DONE:
    { 
      lm_xtraInjDone();
      break;
    }

    case SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE:
    {
      mgp_XtraInjInd();
      break;
    }

    case SM_MSG_ID_MGP_RESET_LOCATION_SERVICE_DONE:
    { 
      lm_ReportResetLocationServiceDone((pdsm_pd_ext_reset_location_service_status_e_type*)(p_msg->p_Data));
      break;
    }

    case C_OS_MSG_ID_DOG_HB:
    {
      // Respond to the dog heart beat. Pat the dog. 
      os_DogHeartbeatReport();
      break;
    }

    default:
    {
      LM_MSG_MED("Illegal IPC message type: %d received",
                  p_msg->q_MsgId,0,0);
      break;
    }
                      
  }/* switch(p_msg->q_MsgId) ends */

}/*lm_proc() ends*/


/*=============================================================================

  FUNCTION      lm_task_stop()

  DESCRIPTION 	Exit LM task loop

  DEPENDENCIES  None

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void lm_task_stop( void )
{
    e_lm_task_state = LM_TASK_STATE_STOP;
}

/*
 ******************************************************************************
 * lm_task
 *
 * Function description:
 *
 * This is the main entry function of the LM task. It will initalize IPC
 * and internal data structures and will also undergo the startup procedure with
 * TMC. It then waits infintely for incoming IPC messages.
 *
 * Parameters: 
 *
 * dummy - task input parameters(currently not used)
 *
 * Return value: 
 *
 ******************************************************************************
*/
/*lint -e715 */ 
void lm_task( dword q_dummy )
{
/*The entry point for the task and the main task loop
  are present in this function */

  os_IpcMsgType* p_msg;

  lm_startup();
  lm_task_state_init();

  lm_create_timers();

#ifdef FEATURE_GPSONE_PPM
  pdsm_srch_msg_free_q_init();
#endif

  e_lm_task_state = LM_TASK_STATE_RUN;
      
  while ( e_lm_task_state == LM_TASK_STATE_RUN )
  {
    
    p_msg = os_IpcReceive();

    if(p_msg == NULL)
    {
      LM_MSG_ERROR("NULL message received",0,0,0);
    }
    else
    {
      lm_proc(p_msg);
      if(!os_IpcDelete(p_msg))
      {
        LM_MSG_ERROR("Unable to delete received IPC message",0,0,0);
      }
    }

  }/*while(1) ends*/

} /*lm_task() ends*/ 
