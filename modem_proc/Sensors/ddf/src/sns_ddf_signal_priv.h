
/**
 * @file sns_ddf_signal_priv.h
 * @brief Common private definitions used across the Sensors
 *  	  Device Driver Framework.
 *  
 * Copyright (c) 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved
 * Qualcomm Technologies Proprietary and Confidential
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

    when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-04-09   pn   Changed all timestamps to sns_ddf_time_t
  2014-11-25   MW   Common definitions to be shared internally in the ddf_signal
==============================================================================*/

#ifndef SNS_DDF_SIGNAL_PRIV_H
#define SNS_DDF_SIGNAL_PRIV_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_debug_str.h"
#include "sns_profiling.h"

#if !defined(SNS_PCSIM)
#include "DALSys.h"
#include "DDITlmm.h"
#include "DALDeviceId.h"
#include "DDIGPIOInt.h"
#endif

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#if !defined(SNS_PCSIM)
// Each driver is allowed up to 2 GPIOs, however, in practice there are no
// devices that use more than one (and some even share this one for multiple
// interrupts). Therefore we save some memory by restricting the number of
// listeners to a more realistic upper limit.
#define SNS_DDF_SIGNAL_MAX_CNT  (SNS_SMGR_NUM_SENSORS_DEFINED+1)

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
// Data structure describing a registered user of the DDF signal service.
typedef struct
{
  bool                  is_registered;  /* TRUE if this signal is registered */
  bool                  int_fired;  /* for debug only */
  uint16_t              gpio_num;
  uint16_t              warning_cnt;
  sns_ddf_handle_t      handle;
  sns_ddf_driver_if_s*  driver;
  uint32_t              gpio_pin_cfg;
  sns_ddf_time_t        timestamp;
  sns_ddf_time_t        prev_timestamp;
  sns_ddf_signal_irq_trigger_e trigger;
  bool                  is_uimg_refac;  
} sns_ddf_sig_info_s;

typedef struct
{
  DalDeviceHandle       *tlmm_handle;
  DalDeviceHandle       *gpio_int_handle;
  DALSYSSyncHandle      sync_handle;
  uint32_t              pending_flags;  /* signal pending flags */
  sns_ddf_sig_info_s    sig_info[SNS_DDF_SIGNAL_MAX_CNT];
} sns_ddf_sig_tb_s;
#endif

extern sns_ddf_driver_if_s sns_dd_qdsp_playback_if;

#endif // SNS_DDF_SIGNAL_PRIV_H
