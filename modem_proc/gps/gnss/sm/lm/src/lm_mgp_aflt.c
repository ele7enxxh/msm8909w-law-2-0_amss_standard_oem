 /*======================================================================

                         Location Manager API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which Location Manager(LM) services can 
  be accessed by other tasks (e.g Transport Manager). 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 6/20/06   mr   Initial creation of header file.
======================================================================*/
#include "gps_variation.h"
#include "lm_api.h"
#include "lm_iface.h"
#include "mgp_api.h"
#include "aries_os_api.h"
#include "lm_mgp.h"
#include "lm_mgp_aflt.h"
#include "lm_data.h"
#include "lm_tm.h"
#include "event.h"



/****************** Function Definitions**************************************/

#ifdef FEATURE_CGPS_USES_CDMA
/*
 ******************************************************************************
 * lm_mgp_aflt_get_results 
 *
 * Function description:
 *
 * This function is to request for PPM results
 *
 * Parameters: 
 * u_EndPpm -  stop PPm processing afer PPM results are sent
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/
void lm_mgp_aflt_get_results(boolean u_EndPpm)
{
#ifdef FEATURE_GPSONE_PPM
  mgp_CDMAGetPpmResults(u_EndPpm, PPM_REQUEST_FROM_LM_TM); 
#endif
}   

/*
 ******************************************************************************
 * lm_mgp_aflt_start_ppm_processing 
 *
 * Function description:
 *
 * This function turns AFLT engine to ON for a MS-Assisted/Hybrid session.
 *
 * Parameters: 
 * p_CdmaReq - Pointer to structure containing CDMA(PPM) request from TM
 *
 * Return value: 
 *
 * boolean - AFLT processng successfully started or not
 *
 ******************************************************************************
*/
boolean lm_mgp_aflt_start_ppm_processing
(
  const lm_session_request_cdma_s_type* const p_CdmaReq
)
{

  if(p_CdmaReq == NULL)
  {
    LM_MSG_ERROR("NULL CDMA req received in start PPM message",0,0,0);
    return FALSE;
  }
      
  if(!os_TimerStart(z_lmControl.p_cdmaTimer,p_CdmaReq->q_timeout,0))
  {
    LM_MSG_ERROR("Failed starting CDMA timer",0,0,0);
    return FALSE;
  }
  else
  {
    LM_MSG_MED("Started CDMA timer with value %d",p_CdmaReq->q_timeout,0,0);
  }

  LM_MSG_MED("Starting PPM processing with timeout[%d] QoS[%d]",
              p_CdmaReq->q_timeout,
              p_CdmaReq->q_qos,0);
  
  #ifdef FEATURE_GPSONE_PPM
  {
    mgp_CdmaSearcherOnCmdType z_CdmaSearcherOnCmd;
    z_CdmaSearcherOnCmd.q_Timeout = p_CdmaReq->q_timeout;
    z_CdmaSearcherOnCmd.q_Qos = p_CdmaReq->q_qos;
    z_CdmaSearcherOnCmd.b_FTcalFlag = z_lmControl.u_FTCalEnabled;
    z_CdmaSearcherOnCmd.q_RequestingTask = PPM_REQUEST_FROM_LM_TM;

    mgp_CDMASearcherOn( &z_CdmaSearcherOnCmd );
  }
  #endif
    
  /*Update fix session related state*/
  z_lmControl.e_afltState = LM_MGP_AFLT_STATE_ON;

  return TRUE; /*lint !e506 */
}

/*
 ******************************************************************************
 * lm_mgp_aflt_stop_ppm_processing 
 *
 * Function description:
 *
 * This function turns AFLT to OFF.
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_aflt_stop_ppm_processing()
{
  LM_MSG_MED("Stopping PPM timer",0,0,0);
  (void)os_TimerStop(z_lmControl.p_cdmaTimer);
  
  if(z_lmControl.e_afltState != LM_MGP_AFLT_STATE_OFF)
  {
#ifdef FEATURE_GPSONE_PPM
    event_report(EVENT_GPS_PPM_END);  
    mgp_CDMASearcherOff(PPM_REQUEST_FROM_LM_TM);
    /*Update fix session related state*/
    z_lmControl.e_afltState       = LM_MGP_AFLT_STATE_OFF;
    z_lmControl.u_FTCalEnabled    = FALSE;
#endif
  }
  z_lmControl.u_ppm_request_pending = FALSE;
  z_lmControl.u_ppm_get_results_pending = FALSE;
  z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_CDMA);
}

/*
 ******************************************************************************
 * lm_mgp_aflt_continue_ppm_processing 
 *
 * Function description:
 *
 * This function updates PPM processing params. Currently it only updates the PPM
 * timeout value. The currently running PPM timeout counter is stopped and a new
 * one is started.
 *
 * Parameters: 
 * p_CdmaReq - Request containing params to be modified for PPM 
 *
 * Return value: 
 *
 * boolean - AFLT processng params successfully updated or not
 *
 ******************************************************************************
*/
boolean lm_mgp_aflt_continue_ppm_processing(const lm_session_request_cdma_s_type* const p_CdmaReq)
{

  if(p_CdmaReq == NULL)
  {
    LM_MSG_ERROR("NULL CDMA req received in continue PPM message",0,0,0);
    return FALSE;
  }
  
  (void)os_TimerStop(z_lmControl.p_cdmaTimer);
  
  LM_MSG_MED("Modifying PPM timer to value %d",p_CdmaReq->q_timeout,0,0); 
  /*Start the timer again with new value*/
  if(!os_TimerStart(z_lmControl.p_cdmaTimer,p_CdmaReq->q_timeout,0))
  {
    LM_MSG_ERROR("Unable to modify CDMA/PPM timer",0,0,0);
    return FALSE;
  }
  
  /*Currently only timeout value is looked at and modified when a continue request
    is received. Other values may be looked at and modified in future*/
  
  /*Report event*/
  event_report(EVENT_GPS_LM_PPM_REQUEST_CONTINUE);

  return TRUE; /*lint !e506 */
}

/*
 ******************************************************************************
 * lm_mgp_aflt_handle_ppm_meas_report 
 *
 * Function description:
 *
 * This function handles the PPM report from CDMA searcher. 
 *
 * Parameters: 
 * *p_msg - IPC message containing ppm measurement report
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void lm_mgp_aflt_handle_ppm_meas_report(os_IpcMsgType* p_msg)
{
  srch_ppm_rpt_type *p_ppmReport;
  
  if(p_msg == NULL)
  {
    LM_MSG_ERROR("PPM report from searcher found NULL",0,0,0);
    return;
  }
  
  p_ppmReport = (srch_ppm_rpt_type*) p_msg->p_Data; /*lint !e826 */


  LM_MSG_MED("Received PPM rpt from MGP. sess_state %d rpt_pend %d p.ppm on %d",
              z_lmControl.e_sessionState, 
              z_lmControl.q_reportsPending,
              tm_core_is_periodic_ppm_session_on());

  event_report(EVENT_GPS_PPM_RESULTS);

  if(z_lmControl.e_sessionState == LM_SESSION_STATE_IN_SESSION_REPORT_PENDING)
  {
    if(z_lmControl.q_reportsPending & (uint32)LM_SESSION_REQUEST_CDMA)/*if a PPM report pending*/
    {

      if(!lm_tm_send_ppm_report_to_tm(p_ppmReport))
      {
        LM_MSG_ERROR("Unable to send PPM report to TM",0,0,0);
      }

      /*Set proper AFLT state*/
      /*lm_mgp_update_mgp_state_after_final_fix(z_lmControl.z_gps_request.q_num_fixes,
                                              z_lmControl.z_gps_request.q_tbf);*/
      /*Update CDMA session related state*/
      z_lmControl.q_reportsPending &= ~((uint32)LM_SESSION_REQUEST_CDMA); 

      if(z_lmControl.q_reportsPending == 0)
      {
        lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      }
      
    /*MRAWAL: Need to generate NMEA*/
    }
    else
    {
      /*MRAWAL: Need to generate NMEA*/
    }
  }

#ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif
  /*In any case turn off the CDMA seacher*/
  //mgp_CDMASearcherOff();
  /*Update fix session related state*/
  //z_lmControl.e_afltState       = LM_MGP_AFLT_STATE_OFF;

 
}

/*
 ******************************************************************************
 * lm_mgp_aflt_FTCalEnable 
 *
 * Function description:
 *
 * This function indirectly enables Fast TCal in the PPM module. 
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
void lm_mgp_aflt_FTCalEnable( void )
{
#ifdef FEATURE_GPSONE_PPM
  if( z_lmControl.e_afltState == LM_MGP_AFLT_STATE_OFF )
  {
    z_lmControl.u_FTCalEnabled = TRUE;
    LM_MSG_MED("F-TCal enabled in PPM",0,0,0);
  }
  else
  {
    LM_MSG_ERROR("F-TCal session enabled when PPM already active",0,0,0);

    if( z_lmControl.u_FTCalEnabled == TRUE )
    {
      /* Restart Fast TCal session */
      mgp_CDMAStartFTCal();
    }
  }
#endif
}

#endif /* FEATURE_CGPS_USES_CDMA */