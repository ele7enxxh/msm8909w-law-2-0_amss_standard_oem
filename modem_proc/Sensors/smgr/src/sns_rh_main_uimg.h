#ifndef SNS_RH_MAIN_UIMG_H
#define SNS_RH_MAIN_UIMG_H
/*============================================================================

  @file sns_rh_main_uimg.h

  @brief
  This file contains definitions for the uImage portion of Request Handler

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_main_uimg.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-02  pn   Processes WAKE_SIG in bigImage
  2016-01-26  pn   Changed sns_rh_init_uimg() signature
  2015-10-20  bd   Added reserved fifo support
  2015-09-24  pn   Code cleanup
  2015-09-18  pn   Report scheduling changes for Batching 
  2015-09-11  pn   Refactored sns_rh_send_report_indications()
  2015-08-24  pn   Added support for concurrent selftest and streaming
  2015-08-21  bd   Code cleanup
  2015-05-27  bd   Flow control feature
  2015-02-03  pn   Made sns_rh_get_latest_sample_timestamp() global
  2015-01-13  hw   Make sns_rh_prepare_report_for_flushing as an API
  2014-09-08  pn   Maintains external report rate 
  2014-07-21  pn   Added unsignal_me()
  2014-06-16  pn   Unshared sns_rh_uimg
  2014-06-05  vy   Further refactored to support uImage  
  2014-04-29  pn   Initial version
============================================================================*/
#include "sns_osa.h"
#include "sns_queue.h"
#include "sns_rh.h"
#include "sns_smgr_api_v01.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
  
                     STRUCTURE DEFINITIONS
  
  ===========================================================================*/
typedef union
{
  sns_smgr_periodic_report_ind_msg_v01  periodic;
  sns_smgr_buffering_ind_msg_v01        buffering;
  sns_smgr_buffering_query_ind_msg_v01  query;
} sns_rh_report_indication_u;

/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern void sns_rh_init_uimg(void);

extern void sns_rh_task_loop(void);

extern void sns_rh_leave_island(void);

extern void sns_rh_enter_island(void);

extern void sns_rh_handle_app_wake_sig(void);

extern void sns_rh_handle_app_sleep_sig(void);

extern sns_q_s* sns_rh_get_report_queue(void);

extern sns_q_s* sns_rh_get_suspended_report_queue(void);

extern sns_smgr_periodic_report_ind_msg_v01* sns_rh_get_periodic_ind_buf(void);

extern sns_smgr_buffering_ind_msg_v01* sns_rh_get_buffering_ind_buf(void);

extern sns_ddf_time_t *sns_rh_link_lastest_sample_ts(
  const sns_smgr_ddf_sensor_s*  ddf_sensor_ptr);

extern sns_smgr_buffering_query_ind_msg_v01* sns_rh_get_query_ind_buf(void);

extern sns_ddf_time_t sns_rh_get_latest_sample_timestamp(
  const sns_rh_rpt_item_s* item_ptr);

extern void sns_rh_update_report_timer(void);

extern bool sns_rh_add_to_ready_list(
  sns_rh_rpt_spec_s*  report_ptr);

extern void sns_rh_rpt_gen_remove_from_ready_list(
  sns_rh_rpt_spec_s* report_ptr);

extern bool sns_rh_ok_to_send_report(
  const sns_rh_rpt_spec_s *rpt_ptr);

extern bool sns_rh_ok_to_trim_nonwakeup_buffer(
  const sns_rh_rpt_spec_s* rpt_ptr);

extern void sns_rh_unsignal_me(uint32_t sig);

extern void sns_rh_prepare_report_for_flushing(sns_rh_rpt_spec_s* rpt_ptr);

extern bool sns_rh_send_report_indication(sns_rh_rpt_spec_s* rpt_ptr);

extern bool sns_rh_send_ready_reports(void);

extern void sns_rh_process_ready_sample_sig(const sns_rh_rpt_spec_s* rpt_ptr);

extern bool sns_rh_new_samples_available(void);

#endif /* SNS_RH_MAIN_UIMG_H */

