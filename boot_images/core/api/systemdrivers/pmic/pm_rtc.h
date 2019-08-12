#ifndef __PM_RTC_H__
#define __PM_RTC_H__

/*! \file pm_rtc.h
 *  \n
 *  \brief This header file contains API and type definitions for PMIC RTC driver.
 *  \n
 *  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
/**
 * @name pm_rtc_get_time
 * 
 * @brief This function returns the current time of the RTC (in secs).
 *        This will be the actual present time if the RTC has been
 *        ticking or the time at which the RTC was last stopped.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[out] time_ptr: Current RTC tick count (in secs)
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__RTC_HALTED = RTC is not running.
 *          PM_ERR_FLAG__INVALID_POINTER = NULL time pointer passed in.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SPMI_OPT_ERR = SPMI error.
 */
pm_err_flag_type pm_rtc_get_time(uint8 pmic_device_index, uint32 *time_ptr);

#endif /* __PM_RTC_H__ */
