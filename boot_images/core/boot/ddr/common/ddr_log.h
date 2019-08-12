#ifndef __DDR_LOG_H__
#define __DDR_LOG_H__

/**
 * @file ddr_log.h
 * @brief
 * Header file for DDR logging.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_log.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
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
  DDR_SITE_UPDATE_CALIBRATION,
#endif
};

enum ddr_log_error
{
  DDR_UNSUPPORTED_DEVICE,
  DUAL_RANK_NOT_SELECTED,
  SITE_CALIBRATION_NOT_FOUND,
};


/*==============================================================================
                                  DATA
==============================================================================*/
extern enum ddr_log_level ddr_log_level;


#endif /* __DDR_LOG_H__ */
