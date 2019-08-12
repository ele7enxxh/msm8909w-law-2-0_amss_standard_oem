#ifndef __PM_RTC_H__
#define __PM_RTC_H__

/** @file pm_rtc.h
 *
 *  This header file contains API and type definitions for PMIC RTC driver.
 */
/*
 *  Copyright 2013-2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/14   leo     (TechComm) Updated/added Doxygen comments and markup.
11/08/13   kt      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** @addtogroup pm_rtc
@{ */

/**
 * Returns the current time of the RTC (in seconds).
 * This is the actual current time if the RTC has been
 * ticking, or the time at which the RTC was last stopped.
 * 
 * @param[in] pmic_device_index Primary PMIC: 0, Secondary PMIC: 1.
 * @param[out] time_ptr Current RTC tick count (in seconds).
 *
 * @return
 * PM_ERR_FLAG__SUCCESS = Success. \n
 * PM_ERR_FLAG__RTC_HALTED = RTC is not running. \n
 * PM_ERR_FLAG__INVALID_POINTER = NULL time pointer was passed in. \n
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature is not available. \n
 * PM_ERR_FLAG__SPMI_OPT_ERR = SPMI error. \n
 * See #pm_err_flag_type.
 */
pm_err_flag_type pm_rtc_get_time(uint8 pmic_device_index, uint32 *time_ptr);

/** @} */ /* end_addtogroup pm_rtc */

#endif /* __PM_RTC_H__ */
