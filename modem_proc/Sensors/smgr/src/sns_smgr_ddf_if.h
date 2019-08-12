#ifndef SNS_SMGR_DDF_IF_H
#define SNS_SMGR_DDF_IF_H

/*============================================================================

  @file sns_smgr_ddf_if.h

  @brief
  This file contains definition for SMGR-DDF interface

*******************************************************************************
* Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_ddf_if.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-01-07  pn   Changed sns_smgr_set_attr() and sns_smgr_dd_reset() signatures
  2014-12-05  pn   Added sns_smgr_set_power_attr()
  2014-08-23  vy   Added an api to get uimage status of driver using sns handle
  2014-07-09  vy   Added support for non-uimage DD
  2014-04-23  pn   Initial version
============================================================================*/
#include "sns_common.h"
#include "sns_smgr.h"
#include "sns_smgr_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"

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
extern sns_ddf_status_e sns_ddf_smgr_notify_event(
  sns_ddf_handle_t  smgr_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_event_e   event);

extern sns_ddf_status_e sns_smgr_notify_event_in_bigimage(
   sns_ddf_handle_t  smgr_handle,
   sns_ddf_sensor_e  sensor,
   sns_ddf_event_e   event);

extern sns_ddf_status_e sns_ddf_smgr_notify_data (
  sns_ddf_handle_t       smgr_handle,
  sns_ddf_sensor_data_s  data[],
  uint32_t               data_len);

extern sns_ddf_status_e sns_ddf_smgr_notify_test_complete(
  sns_ddf_handle_t  smgr_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_status_e  status,
  uint32_t          err);

extern sns_ddf_status_e sns_smgr_set_power_attr(
  sns_smgr_sensor_s*       sensor_ptr,
  sns_ddf_powerstate_e     powerstate);

extern sns_ddf_status_e sns_smgr_set_attr(
  sns_smgr_sensor_s*       sensor_ptr,
  sns_ddf_sensor_e         sensor,
  sns_ddf_attribute_e      attrib,
  void*                    value);

extern sns_ddf_status_e sns_smgr_get_attr(
  sns_smgr_sensor_s*     sensor_ptr,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems);

extern sns_ddf_status_e sns_smgr_dd_enable_sched_data(
  sns_smgr_sensor_s*  sensor_ptr,
  sns_ddf_sensor_e    sensor,
  bool                enable);

extern sns_ddf_status_e sns_smgr_dd_reset(
  sns_smgr_sensor_s*  sensor_ptr);

extern sns_ddf_status_e sns_smgr_dd_run_test(
  const sns_smgr_sensor_s* sensor_ptr);

extern sns_ddf_status_e sns_smgr_dd_driver_access(
  sns_smgr_sensor_s*                  sensor_ptr,
  sns_smgr_driver_access_req_msg_v01* req_ptr,
  void**                              dd_resp_ptr_ptr,
  uint32_t*                           dd_resp_len_ptr,
  void*                               conn_handle);

extern void sns_smgr_dd_driver_access_cancel(
  sns_smgr_sensor_s*                  sensor_ptr,
  void*                               conn_handle);

extern void sns_smgr_enter_bigimage(void);

extern void sns_smgr_exit_bigimage(void);

extern bool sns_smgr_get_uimg_refac(sns_ddf_handle_t sensor_handle);

#endif /* SNS_SMGR_DDF_IF_H */

