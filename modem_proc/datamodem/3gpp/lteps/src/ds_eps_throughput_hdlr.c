/*!
  @file
  ds_eps_throughput_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.

*/



/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_throughput_hdlr.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

==============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_LTE

#include "ds_eps_pdn_context.h"
#include "ds_eps_throughput_hdlr.h"
#include "ds_3gpp_nv_manager.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

ds_eps_downlink_throughput_estimation_s ds_eps_dl_tput_est_tbl[DS_SUBSCRIPTION_MAX] = {{NULL}};

/*===========================================================================
FUNCTION  DS_EPS_CHECK_AND_UPDATE_ML1_STATUS

DESCRIPTION
  This function checks the current status of ML1 reporting status and
  sends (start/stop) ML1 request if required

  If report frequency is 0, this function and reporting is going on
  this function will send to stop report request and return FALSE.
  If reporting frequcny is positive and reporting is stop, this function
  will send start request and return TRUE

PARAMETERS
  subs_id - sys_modem_as_id_e_type (Subscription ID)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is reporting is expected after this, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_check_and_update_ml1_status
(
   sys_modem_as_id_e_type subs_id
)
{
  lte_cphy_dl_tput_estm_report_req_s ds_lte_dl_tput_req;
  uint32 report_frequency = 0 ;
  uint8 instance_id = 0;
  boolean any_lte_bearer_up = FALSE;

  /*------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  instance_id = SYS_AS_ID_TO_INST_ID(subs_id);
  DS_3GPP_MSG1_HIGH("ds_eps_check_and_update_lte_ml1_status for %d",subs_id);
  any_lte_bearer_up = ds_eps_bearer_cntxt_check_if_any_lte_is_bear_up(subs_id);
  

  report_frequency = 
     ds_eps_dl_tput_est_tbl[subs_id].report_frequency;
  ds_lte_dl_tput_req.t_accumulate = (uint16)ds_3gpp_nv_manager_get_dl_tput_t_accumulate(subs_id);

  DS_3GPP_MSG2_LOW("report_frequency: %d for %d", report_frequency, subs_id);

  DS_3GPP_MSG2_LOW("status: %d for %d", ds_eps_dl_tput_est_tbl[subs_id].status, subs_id);

  if ((report_frequency == 0 ||
      (any_lte_bearer_up == FALSE)) &&
      ((ds_eps_dl_tput_est_tbl[subs_id].status == 
       DS_EPS_DL_TPUT_ESTM_STATUS_START) ||
      (ds_eps_dl_tput_est_tbl[subs_id].status == 
       DS_EPS_DL_TPUT_ESTM_STATUS_START_WAIT_FOR_ML1)))
  {
    ds_lte_dl_tput_req.subs_id = subs_id;
    ds_lte_dl_tput_req.req_type = LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_STOP;
    DS_3GPP_MSG2_LOW("Sending LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ with req type %d"
                      "and t_accumulate %d",ds_lte_dl_tput_req.req_type,
                      ds_lte_dl_tput_req.t_accumulate);
    (void) dsmsgrsnd_per_subs_msg_send_ext( 
       LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ,
       MSGR_DS_3GPP, 
       (msgr_hdr_struct_type*)(&ds_lte_dl_tput_req),
       sizeof(lte_cphy_dl_tput_estm_report_req_s),
       instance_id
    );
    ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_STOP_WAIT_FOR_ML1;
    ds_eps_send_last_report_and_reset_info(subs_id);
    return FALSE;
  }
  else if (report_frequency > 0 &&
      (any_lte_bearer_up == TRUE) &&
       ((ds_eps_dl_tput_est_tbl[subs_id].status == 
       DS_EPS_DL_TPUT_ESTM_STATUS_STOP) ||
      (ds_eps_dl_tput_est_tbl[subs_id].status == 
       DS_EPS_DL_TPUT_ESTM_STATUS_STOP_WAIT_FOR_ML1))) 
  {
    ds_lte_dl_tput_req.subs_id = subs_id;
    ds_lte_dl_tput_req.req_type = LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_START;
 
    ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_START_WAIT_FOR_ML1;

    DS_3GPP_MSG2_LOW("Sending LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ with req type %d "
                       "and t_accumulate %d",ds_lte_dl_tput_req.req_type,
                       ds_lte_dl_tput_req.t_accumulate);
    
    (void) dsmsgrsnd_per_subs_msg_send_ext( 
       LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ,
       MSGR_DS_3GPP, 
       (msgr_hdr_struct_type*)(&ds_lte_dl_tput_req),
       sizeof(lte_cphy_dl_tput_estm_report_req_s),
       instance_id
    );
    ds_eps_bearer_cntxt_reset_to_current_dl_byte_count(subs_id);
    }
  else if (report_frequency > 0 &&
      (any_lte_bearer_up == TRUE) &&
       (ds_eps_dl_tput_est_tbl[subs_id].status == 
       DS_EPS_DL_TPUT_ESTM_STATUS_START) &&
           ds_eps_dl_tput_est_tbl[subs_id].ds_eps_freq_modified == TRUE) 
    {
    ds_lte_dl_tput_req.subs_id = subs_id;
      ds_lte_dl_tput_req.req_type = LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_UPDATE;
    
    ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_START_WAIT_FOR_ML1;

    DS_3GPP_MSG2_LOW("Sending LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ with req type %d "
                       "and t_accumulate %d",ds_lte_dl_tput_req.req_type,
                       ds_lte_dl_tput_req.t_accumulate);
    
    (void) dsmsgrsnd_per_subs_msg_send_ext( 
       LTE_CPHY_DL_TPUT_ESTM_REPORT_REQ,
       MSGR_DS_3GPP, 
       (msgr_hdr_struct_type*)(&ds_lte_dl_tput_req),
       sizeof(lte_cphy_dl_tput_estm_report_req_s),
       instance_id
    );
    ds_eps_bearer_cntxt_reset_to_current_dl_byte_count(subs_id);
    ds_eps_dl_tput_est_tbl[subs_id].ds_eps_freq_modified = FALSE;
  }
  return TRUE;
}/*ds_eps_check_and_update_ml1_status*/

/*===========================================================================
FUNCTION  DS_EPS_DL_TPUT_ESTIMATION_INIT

DESCRIPTION
  This function initializes default values of downlink throughput internal
  structures.


PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_dl_tput_estimation_init(void)
{
   uint8 index = 0;
   /*------------------------------------------------------------------------*/
   for( index = 0 ; index < DS_SUBSCRIPTION_MAX; index++)
   {
      ds_eps_dl_tput_est_tbl[index].ds_eps_filtered_confidence_level = 0;
      ds_eps_dl_tput_est_tbl[index].ds_eps_filtered_dl_throughput_est = 0;
      ds_eps_dl_tput_est_tbl[index].time_since_last_estimatation = 0;
      ds_eps_dl_tput_est_tbl[index].status = DS_EPS_DL_TPUT_ESTM_STATUS_STOP;
      ds_eps_dl_tput_est_tbl[index].ds_eps_ignore_historical_data = TRUE;
      ds_eps_dl_tput_est_tbl[index].ds_eps_freq_modified = FALSE;
   }
}/*ds_eps_dl_tput_estimation_init*/


/*===========================================================================
FUNCTION  DS_EPS_DL_TPUT_ESTIMATION_RESET

DESCRIPTION
  This function resets default values of downlink throughput internal
  structures.


PARAMETERS
  subs_id - Subscription ID to be reset

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_dl_tput_estimation_reset
(
   sys_modem_as_id_e_type subs_id
)
{
  time_type ds_eps_time_now;
  uint64 current_time = 0;
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  time_get_ms( ds_eps_time_now );
  current_time = QW_CVT_Q2N(ds_eps_time_now); 
  ds_eps_dl_tput_est_tbl[subs_id].time_since_last_estimatation = current_time;

  DS_3GPP_MSG1_HIGH("ds_eps_dl_tput_estimation_reset for ID %d",subs_id);
  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_confidence_level = 0;
  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_dl_throughput_est = 0;
  ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_STOP;
  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_ignore_historical_data = TRUE;
  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_freq_modified = FALSE;
}/*ds_eps_dl_tput_estimation_reset*/


/*===========================================================================
FUNCTION  DS_EPS_DL_TPUT_PROCESS_ESTIMATION_INDICATION

DESCRIPTION
  This function processes the message that is received from LTE ML1 through
  message router


PARAMETERS
  lte_cphy_dl_tput_estm_report_ind_s - Estimation report message from ML1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_dl_tput_process_estimation_indication
(
   lte_cphy_dl_tput_estm_report_ind_s *est_report
)
{
   uint32 alpha = 0;
   uint32 confidence_level = DS_EPS_MIN_DL_TPUT_CONFIDENCE_LEVEL;

   uint64 dl_est_total_byte_count = 0;
   uint64 pdcp_total_bytes_count = 0;
   uint64 time_diff = 0;
   uint64 current_time = 0;
   time_type dl_time_type_now;
   /*------------------------------------------------------------------------*/
   
   if (est_report == NULL)
   {
     DS_3GPP_MSG0_LOW("est_report passed is NULL");
     return;
   }

   if (!ds3gsubsmgr_is_subs_id_valid(est_report->subs_id))
   {
     return;
   }

   DS_3GPP_MSG1_HIGH("ds_eps_dl_tput_estimation_update for  ID %d",est_report->subs_id);

   time_get_ms( dl_time_type_now );

   current_time = QW_CVT_Q2N(dl_time_type_now);

   if (est_report->data_freeze == FALSE) 
   {
      DS_3GPP_MSG0_LOW("est_report->data_freeze is not set");

      alpha = DS_EPS_MAX_DL_TPUT_ALPHA / 
              ds_eps_dl_tput_est_tbl[est_report->subs_id].report_frequency;
      
      DS_3GPP_MSG1_HIGH("est_report->confidence_level %d", est_report->confidence_level);
      if (est_report->confidence_level == LTE_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_HIGH) 
      {
         confidence_level = DS_EPS_MAX_DL_TPUT_CONFIDENCE_LEVEL;
      }
      else if (est_report->confidence_level == LTE_CPHY_DL_TPUT_ESTM_CONFIDENCE_LEVEL_MEDIUM) 
      {
         confidence_level = DS_EPS_MID_DL_TPUT_CONFIDENCE_LEVEL;
      }
      
      pdcp_total_bytes_count = 
                     ds_eps_bearer_cntxt_get_total_new_dl_byte_count(est_report->subs_id);

      DS_3GPP_MSG3_HIGH("est_alpha %d, "
                        "ML1 bytes %lu, "
                        "PDCP bytes %lu",
                         alpha,
                         est_report->additional_avail_bytes,
                         pdcp_total_bytes_count);

      dl_est_total_byte_count = est_report->additional_avail_bytes + pdcp_total_bytes_count;

      time_diff = (current_time - ds_eps_dl_tput_est_tbl[est_report->subs_id].
                           time_since_last_estimatation);

      ds_eps_calculate_update_dl_tput_estimation(est_report->subs_id, dl_est_total_byte_count, 
                                              confidence_level, time_diff, alpha);
   }
   else
   {
      DS_3GPP_MSG0_LOW("est_report->data_freeze is set");
   }

   ds_eps_dl_tput_est_tbl[est_report->subs_id].time_since_last_estimatation =  current_time;

   if (est_report->moved_to_idle) 
   {
      DS_3GPP_MSG0_HIGH("ML1 moved out of LTE received");
   }
}

/*===========================================================================
FUNCTION  DS_EPS_CALCULATE_UPDATE_DL_TPUT_ESTIMATION

DESCRIPTION
  This function calculates and updates the filtered values of estimation


PARAMETERS
  subs_id - Subscription ID
  new_byte_count - new byte count ML1 and PDCP combined
  confidence_level - Confidence level from ML1 (0/500/1000)
  time_difference - bwteen new and old report
  alpha - 0-1000 (for preseving decimal)
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_calculate_update_dl_tput_estimation
(
  sys_modem_as_id_e_type subs_id,
  uint64 new_byte_count,
  uint32 confidence_level,
  uint64 time_difference,
  uint32 alpha
)
{

  uint64 new_tput = 0;
  uint64 old_tput = 0;
  uint32 old_confidence = 0;
  uint32 new_confidence = 0;
  uint32 one_minus_alpha = 0;
  /*---------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if(ds_eps_dl_tput_est_tbl[subs_id].ds_eps_ignore_historical_data == TRUE)
  {
    DS_3GPP_MSG1_ERROR("First indication for subs_id %d. ingnore historyical data"
                       ,subs_id);
    alpha = DS_EPS_MAX_DL_TPUT_ALPHA;
    ds_eps_dl_tput_est_tbl[subs_id].ds_eps_ignore_historical_data = FALSE;
  }
  
  one_minus_alpha =  DS_EPS_MAX_DL_TPUT_ALPHA - alpha;
  old_tput = ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_dl_throughput_est;
  old_confidence = ds_eps_dl_tput_est_tbl[subs_id].
             ds_eps_filtered_confidence_level;

  DS_3GPP_MSG3_HIGH("old_confidence %lu, "
                    "confidence_level %lu"
                    "old_tput bps %lu", 
                     old_confidence,
                     confidence_level,
                     old_tput);

  old_confidence = one_minus_alpha * old_confidence;
  confidence_level = alpha *  confidence_level;

  new_confidence = (confidence_level + old_confidence)/DS_EPS_MAX_DL_TPUT_ALPHA;

  ds_eps_dl_tput_est_tbl[subs_id].
             ds_eps_filtered_confidence_level = new_confidence;

  DS_3GPP_MSG3_HIGH("old_confidence  weighted %lu, "
                    "confidence_level  weighted %lu, " 
                    "filtered_confidence %lu",
                    old_confidence,
                    confidence_level,
                    new_confidence);

  old_tput = one_minus_alpha * old_tput;

  new_tput = (new_byte_count * DS_EPS_SEC_TO_MS_MULTIPLIER)/time_difference;
  DS_3GPP_MSG3_HIGH("Total bytes %lu, "
                    "time_difference %lu, "
                    "new_tput bps %lu", 
                    new_byte_count,
                    time_difference,
                    new_tput); 

  new_tput = alpha *  new_tput;
  DS_3GPP_MSG2_HIGH("old_tput bps weighted %lu, "
                    "new_tput bps weighted %lu, ",
                     old_tput,
                     new_tput);

  new_tput = (new_tput + old_tput)/DS_EPS_MAX_DL_TPUT_ALPHA;

  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_dl_throughput_est = new_tput;
  DS_3GPP_MSG1_HIGH("filtered_estimation %lu, ", 
          ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_dl_throughput_est);


}/*ds_eps_calculate_update_dl_tput_estimation*/


/*===========================================================================
FUNCTION  DS_EPS_GET_CURRENT_TPUT_VALUES

DESCRIPTION
  This function prepares the current estimation report in PS format


PARAMETERS
  subs_id - Subscription ID
  ps_sys_dl_throughput_info_type - the PS structure to send the indication
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_get_current_tput_values
(
   sys_modem_as_id_e_type subs_id,
   ps_sys_dl_throughput_info_type *dl_tput_info_type
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if (dl_tput_info_type == NULL) 
  {
    DS_3GPP_MSG0_ERROR("dl_tput_info_type passed is NULL");
    return;
  }

  dl_tput_info_type->confidence_level = 
     (uint8)((ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_confidence_level*7)/1000);


  dl_tput_info_type->downlink_allowed_rate = 
     (uint32)(ds_eps_dl_tput_est_tbl[subs_id].ds_eps_filtered_dl_throughput_est*8/(1000));

  DS_3GPP_MSG2_HIGH("Current confidence_level %d, "
                    "Current downlink_allowed_rate %d",
                    dl_tput_info_type->confidence_level, 
                    dl_tput_info_type->downlink_allowed_rate); 
}/*ds_eps_get_current_tput_values*/


/*===========================================================================
FUNCTION  DS_EPS_UPDATE_DOWNLINK_THROUGHPUT_STATUS_AND_TIMERS

DESCRIPTION
  This function starts/stops reporting timer and sends request ML1 based on
  if there is client registered or not, if LTE is available, if any bearer is
  up


PARAMETERS
  subs_id - Subscription ID
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_eps_update_downlink_throughput_status_and_timers
(
   sys_modem_as_id_e_type subs_id
)
{
   uint32 report_frequency = 0;
 /*------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  
  report_frequency = ds_eps_dl_tput_est_tbl[subs_id].report_frequency;
  DS_3GPP_MSG2_HIGH("report_frequency %d for %d",
                     report_frequency, subs_id);

  ds_eps_check_and_update_dl_tput_reporting_timer(subs_id, report_frequency);

  (void)ds_eps_check_and_update_ml1_status(subs_id);

} /*ds_eps_update_downlink_throughput_status_and_timers*/

/*===========================================================================
FUNCTION  DS_EPS_SET_DL_TPUT_REPORT_FREQUENCY

DESCRIPTION
  This function saves reporting frequency for processing ML1 report, if there
  are multiple clients QMI will only send the lowest value


PARAMETERS
  subs_id - Subscription ID
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_set_dl_tput_report_frequency
(
   sys_modem_as_id_e_type subs_id,
   uint32 t_report
)
{
  uint32 t_accumuate = 0;
  /*------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  t_accumuate =  ds_3gpp_nv_manager_get_dl_tput_t_accumulate(subs_id);
  if (t_accumuate == 0) 
  {
     DS_3GPP_MSG0_ERROR("t_accumulate is zero");
    return;
  }

  DS_3GPP_MSG1_HIGH("ds_eps_update_dl_tput_info subs_id %d",subs_id);

  ds_eps_dl_tput_est_tbl[subs_id].report_frequency = 
     t_report/t_accumuate;

  ds_eps_dl_tput_est_tbl[subs_id].ds_eps_freq_modified = TRUE;
  /*Print the set freq*/

}

/*===========================================================================
FUNCTION  DS_EPS_SET_DL_TPUT_REPORT_FREQUENCY

DESCRIPTION
  This function gets t_accumulate nv value from 3gpp nv manager and provides
  callers


PARAMETERS
  subs_id - Subscription ID
  *t_accumulate - inputer value to be stored pointer
 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_dl_estimated_throughput_t_accumuate
(
   sys_modem_as_id_e_type subs_id,
   uint32* t_accumulate
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  if (t_accumulate == NULL) 
  {
    DS_3GPP_MSG0_ERROR("t_accumulate passed is NULL");
    return;
  }
  *t_accumulate = ds_3gpp_nv_manager_get_dl_tput_t_accumulate(subs_id);
}


/*===========================================================================
FUNCTION DS_EPS_LTE_CPHY_DL_THROUGHPUT_ESTM_IND_HDLR

DESCRIPTION
  This function is used to handle the message from lTE ML1. This message is
  sent every t_accumulate time from ML1 when requested

  
PARAMETERS
  msgr_type - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE is message is received correctly

SIDE EFFECTS
  None.
===========================================================================*/ 
boolean ds_eps_lte_cphy_dl_throughput_estm_ind_hdlr
(
  msgr_umid_type              msgr_type,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  lte_cphy_dl_tput_estm_report_ind_s  *ds_lte_dl_tput_estm_report_ptr = NULL;
  /* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */


  if((msgr_type != LTE_CPHY_DL_TPUT_ESTM_REPORT_IND) ||
     (dsmsg_ptr == NULL))
  {
    DS_LTE_MSG2_ERROR("ds_eps_lte_cphy_dl_throughput_ind_hdlr: Invalid "
                      "message %d or dsmsg_ptr %d",
                      msgr_type,
                      dsmsg_ptr);
    return FALSE;
  }


  ds_lte_dl_tput_estm_report_ptr = (lte_cphy_dl_tput_estm_report_ind_s *) dsmsg_ptr;
  DS_LTE_MSG1_HIGH("ds_eps_lte_cphy_dl_throughput_estm_ind_hdlr: received for subs_id "
                      " %d ",ds_lte_dl_tput_estm_report_ptr->subs_id);
  
  ds_eps_dl_tput_process_estimation_indication(ds_lte_dl_tput_estm_report_ptr);

  return TRUE;
}/*ds_eps_lte_cphy_dl_throughput_estm_ind_hdlr*/


/*===========================================================================
FUNCTION DS_EPS_LTE_CPHY_DL_THROUGHPUT_ESTM_IND_HDLR

DESCRIPTION
  This function is used to handle the message from lTE ML1. This message is
  received in reply to requsts every t_accumulate time from ML1 when requested

  
PARAMETERS
  msgr_type - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE is message is received correctly

SIDE EFFECTS
  None.
===========================================================================*/ 
boolean ds_eps_lte_cphy_dl_throughput_report_rsp_hdlr
(
  msgr_umid_type              msgr_type,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  lte_cphy_dl_tput_estm_report_rsp_s *ds_lte_tput_estm_report_rsp_ptr = NULL;
  lte_cphy_dl_tput_estm_report_type_e response_type = LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_START;
  /* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  if((msgr_type != LTE_CPHY_DL_TPUT_ESTM_REPORT_RSP) ||
     (dsmsg_ptr == NULL))
  {
    DS_LTE_MSG2_ERROR("ds_eps_lte_cphy_dl_throughput_report_rsp_hdlr: Invalid "
                      "message %d or dsmsg_ptr:0x%x",
                      msgr_type,
                      dsmsg_ptr);
    return FALSE;
  }

  ds_lte_tput_estm_report_rsp_ptr = (lte_cphy_dl_tput_estm_report_rsp_s *) dsmsg_ptr;

  subs_id =  ds_lte_tput_estm_report_rsp_ptr->subs_id;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  response_type = ds_lte_tput_estm_report_rsp_ptr->rsp_type;
  
  DS_LTE_MSG3_HIGH("Received reponse type %d received while status is %d "
                   "for subs_id(CM) %d",response_type, 
                    ds_eps_dl_tput_est_tbl[subs_id].status, subs_id);

  if (response_type == LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_START ||
      response_type == LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_UPDATE) 
  {
     if(ds_eps_dl_tput_est_tbl[subs_id].status == 
        DS_EPS_DL_TPUT_ESTM_STATUS_START_WAIT_FOR_ML1)
     {
       ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_START;
     }
  }
  else if (response_type == LTE_CPHY_DL_TPUT_ESTM_REPORT_TYPE_STOP) 
  {
     if(ds_eps_dl_tput_est_tbl[subs_id].status == 
        DS_EPS_DL_TPUT_ESTM_STATUS_STOP_WAIT_FOR_ML1)
     {
       ds_eps_dl_tput_est_tbl[subs_id].status = DS_EPS_DL_TPUT_ESTM_STATUS_STOP;
     }
  }
  
  return TRUE;
}/*ds_eps_lte_cphy_dl_throughput_report_rsp_hdlr*/


/*===========================================================================
FUNCTION DS_EPS_SEND_LAST_REPORT_AND_RESET_INFO

DESCRIPTION
  This function sends the report and clears the info. No further information
  will not be stored/reported until LTE is back

  
PARAMETERS
  subs_id - Subscription

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE is message is received correctly

SIDE EFFECTS
  None.
===========================================================================*/ 

void ds_eps_send_last_report_and_reset_info
(
  sys_modem_as_id_e_type subs_id
)
{
  ps_sys_dl_throughput_info_type dl_tput_info;
  int16                             dss_errno = 0;
  /*------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DS_3GPP_MSG1_HIGH("ds_eps_s_send_last_report_and_stop_reporting_timer  "
                     "last report tput for subs: %d",subs_id);

  ds_eps_get_current_tput_values (subs_id, &dl_tput_info);
  ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                      PS_SYS_CONF_DL_THROUGHPUT_INFO,
                      (ps_sys_subscription_enum_type)
                      ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                      &dl_tput_info, &dss_errno);
  ds_eps_dl_tput_estimation_reset(subs_id);
}/*DS_EPS_SEND_LAST_REPORT_AND_RESET_INFO*/

/*===========================================================================
FUNCTION DS_EPS_CHECK_IF_DL_TPUT_EST_TIMER_RUNNING

DESCRIPTION
  This function checks if reporting timer is running for that subscriptions

  
PARAMETERS
  subs_id - Subscription

DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE is timer is running, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/ 
boolean ds_eps_check_if_dl_tput_est_timer_running
(
   sys_modem_as_id_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds3g_timer_enum_type dl_put_est_report_timer = 
     DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN;
  /*--------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  dl_put_est_report_timer = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN + subs_id;

  ret_val = ds3gi_is_timer_running(dl_put_est_report_timer);

  DS_3GPP_MSG2_HIGH("DL estimated tput timer for CM sub id %d is running:  %d"
                    ,subs_id, ret_val);

  return ret_val;

}

/*===========================================================================
FUNCTION DS_EPS_START_DL_TPUT_EST_TIMER

DESCRIPTION
  This function starts reporting timer for that subscriptions

  
PARAMETERS
  subs_id - Subscription

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/ 

void ds_eps_start_dl_tput_est_timer
(
   sys_modem_as_id_e_type subs_id
)
{
  uint32 timer_frequency = 0;
  uint32 t_accumulate = 0;
  uint32 timer_interval = 0;
  ds3g_timer_enum_type dl_put_est_report_timer_val = 
     DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN;
  /*--------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  timer_frequency = ds_eps_dl_tput_est_tbl[subs_id].report_frequency;

  t_accumulate = ds_3gpp_nv_manager_get_dl_tput_t_accumulate(subs_id);
  timer_interval = timer_frequency*t_accumulate;
  dl_put_est_report_timer_val = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN + subs_id;

  ds3gi_start_timer(dl_put_est_report_timer_val, timer_interval);

  DS_3GPP_MSG2_HIGH("DL estimated tput timer started for  CM sub id %d is with:  %d"
                    ,subs_id, timer_interval);

}

/*===========================================================================
FUNCTION DS_EPS_STOP_DL_TPUT_EST_TIMER

DESCRIPTION
  This function stops reporting timer for that subscriptions

  
PARAMETERS
  subs_id - Subscription

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/ 
void ds_eps_stop_dl_tput_est_timer
(
   sys_modem_as_id_e_type subs_id
)
{
  ds3g_timer_enum_type dl_put_est_report_timer_val = 
     DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN;
  /*--------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid CM sub ID %d",subs_id);
    return;
  }

  dl_put_est_report_timer_val = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN + subs_id;

  ds3gi_stop_timer(dl_put_est_report_timer_val);

  DS_3GPP_MSG1_HIGH("DL estimated tput timer stopped for  CM sub id %d"
                    ,subs_id);

}

/*===========================================================================
FUNCTION DS_EPS_CHECK_AND_UPDATE_DL_TPUT_REPORTING_TIMER

DESCRIPTION
  This function checks if reporting timer is needed to started or not based
  on technology and user preference

  
PARAMETERS
  subs_id - Subscription
  reporting_frequency - client reported reporting frequency

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/ 
void ds_eps_check_and_update_dl_tput_reporting_timer
(
   sys_modem_as_id_e_type subs_id,
   uint32 reporting_frequency
)
{
   boolean lte_bearer_up = FALSE;
   uint64 current_time = 0;
   time_type ds_eps_time_now;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid CM sub ID %d",subs_id);
    return;
  }

  lte_bearer_up = ds_eps_bearer_cntxt_check_if_any_lte_is_bear_up(subs_id);
  if (lte_bearer_up == TRUE) 
  {
     if (ds_eps_check_if_dl_tput_est_timer_running(subs_id) && (reporting_frequency > 0)) 
     {
        DS_3GPP_MSG1_HIGH("Report time already running for CM sub ID %d",subs_id);
     }
     else if (reporting_frequency > 0)
     {
        time_get_ms( ds_eps_time_now );
        current_time = QW_CVT_Q2N(ds_eps_time_now); 
        ds_eps_dl_tput_est_tbl[subs_id].time_since_last_estimatation = current_time;
        ds_eps_bearer_cntxt_reset_to_current_dl_byte_count(subs_id);
       ds_eps_start_dl_tput_est_timer(subs_id);
     }
  }
  else if (ds_eps_check_if_dl_tput_est_timer_running(subs_id))
  {
     ds_eps_stop_dl_tput_est_timer(subs_id);
  }


}/*ds_3gpp_pdn_cntx_check_and_update_dl_tput_report_timer*/


/*===========================================================================
FUNCTION DS_EPS_DL_TPUT_TIMER_EXPIRE_HDLR

DESCRIPTION
  This function is called when the reporting timer expires and 3GPP MH will
  send filtered report to client through PS

PARAMETERS
  subs_id - Subscription for which report is being generated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_dl_tput_timer_expire_hdlr
(
   sys_modem_as_id_e_type subs_id
)
{
  ps_sys_dl_throughput_info_type dl_tput_info;
  int16                             dss_errno = 0;
  boolean continue_reporting = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  continue_reporting = ds_eps_check_and_update_ml1_status(subs_id);
  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntx_dl_tput_timer_expired_cmd_hdlr  "
                     "continue_reporting: %d",continue_reporting);
  if (continue_reporting == TRUE) 
  {
     ds_eps_get_current_tput_values (subs_id, &dl_tput_info);

     DS_3GPP_MSG1_LOW("ds_3gpp_pdn_cntx_dl_tput_timer_expired_cmd_hdlr  "
                     "reporting regular tput for subs: %d",subs_id);

     ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                         PS_SYS_CONF_DL_THROUGHPUT_INFO,
                         (ps_sys_subscription_enum_type)
                           ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                         &dl_tput_info, &dss_errno);

  }

} /* ds_3gpp_pdn_cntx_dl_tput_timer_expired_cmd_hdlr */



/*===========================================================================
FUNCTION DS_EPS_DL_TPUT_REPORTING_FREQUENCY_MODIFY_HDLR

DESCRIPTION
  This function is called clients modify the reporting frequency

PARAMETERS
  subs_id - Subscription for which report is being generated
  report_frequency - that is minimul of all clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_dl_tput_reporting_frequency_modify_hdlr
(
   sys_modem_as_id_e_type subs_id,
   uint32 t_report
)
{
  time_type time_type_now;
  uint64 current_time = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntx_dl_tput_timer_modified_cmd_hdlr subs_id %d",subs_id);

  time_get_ms( time_type_now );
  current_time = QW_CVT_Q2N(time_type_now); 
  ds_eps_dl_tput_est_tbl[subs_id].time_since_last_estimatation = current_time;

  ds_eps_bearer_cntxt_reset_to_current_dl_byte_count(subs_id);
  ds_eps_set_dl_tput_report_frequency(subs_id,t_report);
  ds_eps_update_downlink_throughput_status_and_timers(subs_id);

} /* ds_3gpp_pdn_cntx_dl_tput_timer_modified_cmd_hdlr */

#endif /* FEATURE_DATA_LTE */


