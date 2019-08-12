#ifndef SNS_SMGR_UTIL_H
#define SNS_SMGR_UTIL_H

/*============================================================================

  @file sns_smgr_util.h

  @brief
  This file contains definition for Sensor Manager utility functions

******************************************************************************
* Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_util.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who   what, where, why
  ----------  ---   ----------------------------------------------------------
  2015-03-08  pn    Added sns_smgr_set_md_state()
  2016-02-22  tc    Added is_daf_active function
  2015-10-13  jtl   Added heartbeat check function
  2015-09-24  pn    Code cleanup
  2015-09-22  pn    Self-scheduling streaming sensors are monitored for activities
  2015-08-24  bd    Code clean up
  2015-04-09  pn    Replaced sns_smgr_get_tick64() with sns_smgr_update_last_tick()
  2014-09-25  pn    Added support for bigImage depots
  2014-05-12  tc    Converted frequencies to fixed point, Q16. This is to
                    accommodate frequencies less than 1. Q16 fixed point is
                    used, instead of floating point, because the Q6 does not
                    have hardware-support for floating point division.
  2014-04-23  pn    Initial version
 
============================================================================*/

#include "sns_common.h"
#include "sns_smgr.h"
#include "sns_smgr_priv.h"

/*===========================================================================

                   SMGR MACRO

===========================================================================*/


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                          INLINE FUNCTIONS

===========================================================================*/

/*===========================================================================

                          FUNCTIONS

===========================================================================*/
extern void sns_smgr_update_last_tick(void);

extern void sns_smgr_load_default_cal(
  sns_smgr_cal_s*               cal_ptr);

extern uint32_t sns_smgr_get_sched_intval(
  q16_t                         req_freq);

extern q16_t sns_smgr_rate_hz_to_q16(uint16_t rate);

extern sns_smgr_sensor_s* sns_smgr_find_sensor_by_driver_uuid(
  uint8_t                       uuid[16]);

extern sns_smgr_sensor_s* sns_smgr_find_sensor(
  smgr_sensor_id_e              sensor_id);

extern sns_smgr_ddf_sensor_s* sns_smgr_find_ddf_sensor(
  smgr_sensor_id_e              sensor_id,
  uint8_t                       data_type);

extern sns_smgr_ddf_sensor_s* sns_smgr_match_ddf_sensor(
  const sns_smgr_sensor_s*      sensor_ptr,
  sns_ddf_sensor_e              ddf_sensor_type);

extern void sns_smgr_set_sensor_state(
  sns_smgr_sensor_s*            sensor_ptr, 
  sns_smgr_sensor_state_e       state);

extern bool sns_smgr_is_on_change_sensor(
  sns_ddf_sensor_e              ddf_sensor_type);

extern bool sns_smgr_is_event_sensor(
  const sns_smgr_sensor_s*      sensor_ptr, 
  uint8_t                       data_type);

extern bool sns_smgr_ddf_sensor_is_event_sensor(
  const sns_smgr_ddf_sensor_s*  ddf_sensor_ptr);

extern uint16_t sns_smgr_get_max_sample_count(
  const sns_smgr_ddf_sensor_s*  ddf_sensor_ptr);

extern q16_t smgr_get_max_requested_freq(
  const sns_smgr_sensor_s*      sensor_ptr);

extern uint32_t smgr_sensor_type_max_odr(
  const sns_smgr_sensor_s*      sensor_ptr);

extern q16_t sns_smgr_get_max_req_freq(void);

extern void sns_smgr_get_adjacent_odrs(
  const sns_smgr_sensor_s*      sensor_ptr,
  q16_t                         frequency,
  uint32_t                      adjacent_odrs[2]);

extern uint32_t sns_smgr_choose_odr(
  const sns_smgr_sensor_s*      sensor_ptr);

extern q16_t sns_smgr_compute_depot_data_rate(
  const sns_smgr_ddf_sensor_s*  ddf_sensor_ptr);

extern void sns_smgr_put_next_request(
  sns_smgr_sensor_s*            sensor_ptr,
  sns_smgr_request_response_s*  req_resp_ptr);

extern sns_smgr_request_response_s* sns_smgr_get_next_request(
  sns_smgr_sensor_s*            sensor_ptr);

extern void sns_smgr_get_self_test_result(
  sns_smgr_self_test_result_s*  result_ptr);

extern void sns_smgr_heartbeat_monitor_queue_put(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr);

extern void sns_smgr_heartbeat_monitor_queue_delete(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr);

extern bool sns_smgr_sensor_is_ok(
  const sns_smgr_ddf_sensor_s *ddf_sensor_ptr);

extern void sns_smgr_set_md_state(bool enable);

#endif /* SNS_SMGR_UTIL_H */

