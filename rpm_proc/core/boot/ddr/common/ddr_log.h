#ifndef __DDR_LOG_H__
#define __DDR_LOG_H__

/**
 * @file ddr_log.h
 * @brief
 * Header file for DDR logging.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/common/ddr_log.h#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/26/13   sl      Added DDR_ERR_FATAL().
04/02/13   tw      Added error code for when training parameter goes out 
                   of bounds
10/09/12   tw      Added error codes for when training parameter is missing
09/12/12   sl      Use macro-style DDR logging.
07/19/12   tw      Implemented qdss swevent wrapper around DDR logging.
03/02/12   tw      Initial version.
================================================================================
                      Copyright 2012 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  MACROS
==============================================================================*/
#ifdef BUILD_BOOT_CHAIN
  #define DDR_LOG(...)
#else
  #include "swevent.h"

  #define DDR_LOG(level, event, ...)                                 \
            do                                                       \
            {                                                        \
              if ((level) >= ddr_log_level)                          \
              {                                                      \
                SWEVENT((tracer_event_id_t)(event), ##__VA_ARGS__);  \
              }                                                      \
            } while (0)
#endif

#define DDR_ERR_FATAL()  do{} while(1)


/*==============================================================================
                                  TYPES
==============================================================================*/
enum ddr_log_level
{
  DDR_STATUS  = 0,
  DDR_WARNING = 1,
  DDR_ERROR   = 2,
};

enum ddr_log_event
{
#ifdef BUILD_BOOT_CHAIN
  DDR_INIT,
  DDR_SELF_REFRESH_ENTRY_IDLE,
  DDR_SELF_REFRESH_ENTRY_ASAP,
  DDR_SELF_REFRESH_EXIT,
  DDR_DPD_ENTRY,
  DDR_DPD_EXIT,
  DDR_DMM,
  DDR_BIMC_POWER_COLLAPSE_ENTRY,
  DDR_BIMC_POWER_COLLAPSE_EXIT,
  DDR_PRE_CLK_SWITCH,
  DDR_POST_CLK_SWITCH,
  DDR_FATAL_ERROR,
  DDR_SITE_CALIBRATION,
  DDR_SITE_UPDATE_CALIBRATION,
#else
  DDR_INIT,
  DDR_SELF_REFRESH_ENTRY_IDLE = DDR_EVENT_BIMC_SR_ENTRY_IDLE,
  DDR_SELF_REFRESH_ENTRY_ASAP = DDR_EVENT_BIMC_SR_ENTRY_ASAP,
  DDR_SELF_REFRESH_EXIT = DDR_EVENT_BIMC_SR_EXIT,
  DDR_DPD_ENTRY = DDR_EVENT_BIMC_DPD_ENTRY,
  DDR_DPD_EXIT = DDR_EVENT_BIMC_DPD_EXIT,
  DDR_DMM = DDR_EVENT_BIMC_PASR_UPDATE,
  DDR_BIMC_POWER_COLLAPSE_ENTRY = DDR_EVENT_BIMC_PWR_COLLAPSE_ENTRY,
  DDR_BIMC_POWER_COLLAPSE_EXIT = DDR_EVENT_BIMC_PWR_COLLAPSE_EXIT,
  DDR_PRE_CLK_SWITCH = DDR_EVENT_BIMC_PRE_CLK_SWITCH,
  DDR_POST_CLK_SWITCH = DDR_EVENT_BIMC_POST_CLK_SWITCH,
  DDR_FATAL_ERROR = DDR_EVENT_DDR_DRIVER_ERROR_FATAL,
  DDR_SITE_CALIBRATION,
  DDR_SITE_UPDATE_CALIBRATION,
  DDR_GET_CX_INDEX,
  DDR_GET_MX_INDEX,
#endif
};

enum ddr_log_error
{
  DDR_UNSUPPORTED_DEVICE,
  DUAL_RANK_NOT_SELECTED,
  SITE_CALIBRATION_NOT_FOUND,
  TRAINING_PARAMETER_NOT_FOUND,
  TRAINING_PARAMETER_INDEX_OUT_OF_BOUND,
};


/*==============================================================================
                                  DATA
==============================================================================*/
extern enum ddr_log_level ddr_log_level;


#endif /* __DDR_LOG_H__ */
