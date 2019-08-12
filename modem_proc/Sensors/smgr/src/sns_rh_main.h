#ifndef SNS_RH_MAIN_H
#define SNS_RH_MAIN_H

/*============================================================================

  @file sns_rh_main.h

  @brief
  This file contains definition for Request Handler

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_rh_main.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $*/
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-09  pn   Updated timestamps from uint32_t to sns_ddf_time_t
  2015-01-16  vy   Moved uImage logging to debug module  
  2014-07-21  pn   Updated sensor_event implementation to support updated API
  2014-06-05  vy   Further refactored to support uImage  
  2014-04-29  pn   Refactored to support microImage
  2014-04-23  pn   Initial version
============================================================================*/
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_pm.h"
#include "sns_queue.h"
#include "sns_rh.h"
#include "sns_smgr.h"
#include "sns_smgr_api_v01.h"
#include "sns_smgr_internal_api_v02.h"
#include "sns_smgr_main.h"
#include "sns_smgr_sensor_config.h"

/*===========================================================================

                   DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                   STRUCTURE DEFINITIONS

===========================================================================*/
typedef union
{
  sns_smgr_sensor_event_ind_msg_v02           sensor_event;
  sns_smgr_sensor_status_monitor_ind_msg_v02  sensor_status_monitor;
  sns_smgr_single_sensor_test_ind_msg_v01     self_test;
} sns_rh_infrequent_indication_u;

struct sns_rh_sensor_event_s
{
  bool                               enabled;
  sns_q_s                            request_queue;
  sns_smgr_ddf_sensor_s*             ddf_sensor_ptr; /* associated DDF sensor */
  sns_smgr_sensor_event_status_e_v02 status;    /* most recent status */
  sns_ddf_time_t                     last_event_ts;  /* ts of the most recent event */
  uint64_t                           sensor_event;

  /* when sensor event transitions to one of its states, the reports
     for that state get transfered to the main report queue */
  sns_q_s                            enabled_state_rpt_q;
  sns_q_s                            disabled_state_rpt_q;
  sns_q_s                            occurred_state_rpt_q;
};

struct sns_rh_self_test_s
{
  sns_smgr_ddf_sensor_s*        ddf_sensor_ptr;
  sns_rh_mr_req_q_item_s*       req_ptr;
};

typedef struct
{
  sns_rh_sensor_event_s           sensor_events[SNS_SMGR_NUM_SENSORS_DEFINED];
  sns_rh_self_test_s              self_test;

  /* queue for reports built from report requests messages */
  sns_q_s                         status_request_queue;

  /* queue for response messages from SMGR task destined to clients */
  sns_smgr_protected_q_s          resp_queue;

  /* queue for response messages from SMGR task destined to clients */
  sns_smgr_protected_q_s          ind_queue;

  sns_rh_infrequent_indication_u  infrequent_ind;

} sns_rh_s;

extern sns_rh_s sns_rh;


/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern void sns_rh_cancel_service(void* connection_handle);

extern void sns_rh_process_big_image_signals(OS_FLAGS sig_mask);

extern void sns_rh_task(void* p_arg);

#endif /* SNS_RH_MAIN_H */

