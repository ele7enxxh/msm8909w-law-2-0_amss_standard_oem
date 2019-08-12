/*!
  @file
  ds_eps_throughput_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.

*/


#ifndef __DS_EPS_MSG_H__
#define __DS_EPS_MSG_H__

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_throughput_hdlr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"

#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#include "ds_3gpp_msg.h"
#include "ds3gtimer.h"
#include "dsmsgr.h"
#include "lte_cphy_msg.h"



/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DS_LTE_DL_TPUT_T_ACCUMULATE_DEFAULT_VALUE 512 /* in Milliseconds*/
#define DS_EPS_SEC_TO_MS_MULTIPLIER 1000

#define DS_EPS_MAX_DL_TPUT_CONFIDENCE_LEVEL 1000
#define DS_EPS_MIN_DL_TPUT_CONFIDENCE_LEVEL 0
#define DS_EPS_MID_DL_TPUT_CONFIDENCE_LEVEL 500

#define DS_EPS_MAX_DL_TPUT_ALPHA 1000
#define DS_EPS_MIN_DL_TPUT_ALPHA 0

/*Describe*/
typedef enum
{
   DS_EPS_DL_TPUT_ESTM_STATUS_STOP_WAIT_FOR_ML1,
   DS_EPS_DL_TPUT_ESTM_STATUS_STOP,
   DS_EPS_DL_TPUT_ESTM_STATUS_START_WAIT_FOR_ML1,
   DS_EPS_DL_TPUT_ESTM_STATUS_START
}ds_eps_dl_tput_estm_status_type_e;

typedef struct
{
   uint32 report_frequency; /* Description */
   uint16 ds_eps_filtered_confidence_level;
   uint64 ds_eps_filtered_dl_throughput_est;
   uint64 time_since_last_estimatation;
   ds_eps_dl_tput_estm_status_type_e status;
   boolean ds_eps_ignore_historical_data;
   boolean ds_eps_freq_modified;

} ds_eps_downlink_throughput_estimation_s;


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
);

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
);

/*===========================================================================
FUNCTION ds_eps_start_dl_tput_est_timer

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
);


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
);

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
);


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
);

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
void ds_eps_dl_tput_estimation_init
(
   void
);


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
);

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
);


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
);


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
);

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
);

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
);

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
   uint32 *t_accumuate
);

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
);

/*===========================================================================
FUNCTION DS_EPS_LTE_CPHY_DL_THROUGHPUT_REPORT_RSP_HDLR

DESCRIPTION
  This function is used to handle the message from NAS. This message is sent
  whenever T3420 timer expires

  Here we validate the message ID and we update ds_eps_emergency_auth_fail
  
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
);


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
);

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
);

#endif /* FEATURE_DATA_LTE */
#endif
