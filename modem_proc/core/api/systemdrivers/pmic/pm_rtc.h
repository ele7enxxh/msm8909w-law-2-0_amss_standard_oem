#ifndef PM_RTC_H
#define PM_RTC_H

/**
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_rtc.h PMIC RTC related declaration.
 *
 * @brief This file contains enums and API definitions for Real Time Clock Driver.
 */

/*===========================================================================

           R T C   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  This file contains functions prototypes and variable/type/constant
  declarations for the RTC services developed for the Qualcomm Power
  Management IC.

  Copyright (c) 2003-2009, 2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/23/08   jtn     Added API pm_hal_rtc_prescaler_reset()
06/18/07   cng     Added meta comments to miscellaneous RTC APIs
05/31/06   Vish    Fixed LINT warnings.
05/03/05   Vish    Modified pm_hal_rtc_get_time() to read RTC time even in
                   the case when RTC was not running.
01/19/05   Vish    Updated function header for pm_hal_rtc_get_time().
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/07/03   Vish    Added the task of clearing the alarm trigger condition
                   within pm_hal_rtc_disable_alarm() so that the master
                   RTC alarm interrupt could be cleared afterward.
10/02/03   Vish    Added pm_hal_rtc_get_alarm_time() and
                   pm_hal_rtc_get_alarm_status().
09/23/03   Vish    Changed all pm_rtc_xxx variables/fns to pm_hal_rtc_xxx.
09/13/03   Vish    Created.
===========================================================================*/

/*==========================================================================

                        INCLUDE FILES

========================================================================= */
#include "comdef.h"
#include "pm_err_flags.h"

/*==========================================================================

                         TYPE DEFINITIONS

========================================================================= */
/**
* @struct pm_hal_rtc_time_type
* @brief RTC time structure used for setting and retrieving current time and
*        for setting the alarm time.
*/
typedef struct
{
   uint32  sec;  /*!< RTC time in seconds. */
} pm_hal_rtc_time_type;

/**
* @enum pm_hal_rtc_alarm
* @brief Type for selecting the physical alarms available in the RTC.
*/
typedef enum
{
   PM_HAL_RTC_ALARM_1,  /*!< RTC alarm 1 */
   PM_HAL_RTC_ALARM_INVALID  /*!< RTC alarm invalid */
} pm_hal_rtc_alarm;

/**
* @struct pm_hal_rtc_alarm_status_type
* @brief Status of the various physical alarms available in the RTC.
*/
typedef struct
{
   boolean  alarm_1_triggered;  /*!< Alarm 1 is triggered. */
} pm_hal_rtc_alarm_status_type;


/* =========================================================================

                    RTC DRIVER FUNCTION PROTOTYPES

========================================================================= */
/**
 * @name pm_dev_hal_rtc_start
 *
 * @brief Starts the RTC ticking with the indicated time as its current (start-up) time.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] start_time_ptr Pointer to the start-up time for the real-time clock.
 *                            See #pm_hal_rtc_time_type. Valid input is a valid
 *                            non-NULL RTC time structure containing any 32-bit
 *                            number indicating the number of seconds elapsed from
 *                            a known point in time in history.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_start(), which is deprecated. Use pm_dev_hal_rtc_start() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_start(uint8 pmic_chip,
                                             const pm_hal_rtc_time_type *start_time_ptr);
extern pm_err_flag_type pm_hal_rtc_start(const pm_hal_rtc_time_type *start_time_ptr);

/**
 * @name pm_dev_hal_rtc_stop
 *
 * @brief This function stops the RTC from ticking.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_stop(), which is deprecated. Use pm_dev_hal_rtc_stop() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_stop(uint8 pmic_chip);
extern pm_err_flag_type pm_hal_rtc_stop(void);

/**
 * @name pm_dev_hal_rtc_get_time
 *
 * @brief Turn the PMIC abort timer switch on and off
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] time_ptr Pointer to the time stamp structure for the real-time clock.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_get_time(), which is deprecated. Use pm_dev_hal_rtc_get_time() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_get_time(uint8 pmic_chip,
                                                pm_hal_rtc_time_type *time_ptr);
extern pm_err_flag_type pm_hal_rtc_get_time(pm_hal_rtc_time_type *time_ptr);

/**
 * @name pm_dev_hal_rtc_enable_alarm
 *
 * @brief Turn the PMIC abort timer switch on and off
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] what_alarm Indicates which alarm is to be turned on.
 * @param [in] trigger_time_ptr Pointer to the time at which the alarm is to go
 *              off. See #pm_hal_rtc_time_type. Valid input is to a valid non-NULL
 *              RTC time structure containing any 32-bit value indicating the
 *              number of seconds that have elapsed (in relation to a known point
 *              in time in history, as specified in the previous pm_hal_rtc_start()
 *              call) exactly at the moment the alarm is expected to go off.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() and pm_hal_rtc_start() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_enable_alarm(), which is deprecated. Use pm_dev_hal_rtc_enable_alarm() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_enable_alarm(uint8 pmic_chip,
                                                    pm_hal_rtc_alarm what_alarm,
                                                    const pm_hal_rtc_time_type *trigger_time_ptr);
extern pm_err_flag_type pm_hal_rtc_enable_alarm(pm_hal_rtc_alarm what_alarm,
                                                const pm_hal_rtc_time_type *trigger_time_ptr);

/**
 * @name pm_dev_hal_rtc_disable_alarm
 *
 * @brief Disables the specified alarm so that it does not go off in the future.
 *        If the alarm has already been triggered by the time this function
 *        was called, it also clears the alarm trigger condition so that the
 *        master RTC alarm interrupt can be cleared afterwards.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] what_alarm Indicates which alarm is to be turned off.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_disable_alarm(), which is deprecated. Use pm_dev_hal_rtc_disable_alarm() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_disable_alarm(uint8 pmic_chip,
                                                     pm_hal_rtc_alarm  what_alarm);
extern pm_err_flag_type pm_hal_rtc_disable_alarm(pm_hal_rtc_alarm  what_alarm);

/**
 * @name pm_dev_hal_rtc_get_alarm_time
 *
 * @brief Turn the PMIC abort timer switch on and off
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] what_alarm Indicates the alarm to be read.
 *
 * @param [out] alarm_time_ptr Pointer to the time structure that is used by
 *                              this function to return the time at which the
 *                              specified alarm has been programmed to trigger.
 *                              Valid input is any valid non-NULL RTC time structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_get_alarm_time(), which is deprecated. Use pm_dev_hal_rtc_get_alarm_time() instead.
 */
extern pm_err_flag_type
pm_dev_hal_rtc_get_alarm_time(uint8 pmic_chip,
                              pm_hal_rtc_alarm what_alarm,
                              pm_hal_rtc_time_type  *alarm_time_ptr);
extern pm_err_flag_type
pm_hal_rtc_get_alarm_time(pm_hal_rtc_alarm what_alarm,
                          pm_hal_rtc_time_type  *alarm_time_ptr);

/**
 * @name pm_dev_hal_rtc_get_alarm_status
 *
 * @brief Turn the PMIC abort timer switch on and off
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] status_ptr Pointer to a valid uint8 value used for returning
 *                          the status information for the various alarms
 *                          (currently, only one alarm is valid - PM_HAL_RTC_ALARM_1).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_get_alarm_status(), which is deprecated. Use pm_dev_hal_rtc_get_alarm_status() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_get_alarm_status(uint8 pmic_chip,
                                                        uint8 *status_ptr);
extern pm_err_flag_type
pm_hal_rtc_get_alarm_status(uint8 *status_ptr);

/**
 * @name pm_dev_hal_rtc_set_time_adjust
 *
 * @brief Sets the time adjustment correction factor for a RTC crystal
 *        oscillator that is slightly off the 32768 Hz frequency. Every
 *        10th second, the frequency divider is switched from the nominal
 *        32768 to (32768 - 64 + time_adjust).
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] time_adjust Adjustment factor (uint8).
 *             Valid inputs:
 *              - 0 to 63 - Compensates for a slower crystal oscillator.
 *              - 64 - Provides no compensation.
 *              - 65 to 127 - Compensates for a faster crystal oscillator.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_set_time_adjust(), which is deprecated. Use pm_dev_hal_rtc_set_time_adjust() instead.
 */
extern pm_err_flag_type
pm_dev_hal_rtc_set_time_adjust(uint8 pmic_chip,
                               uint8 time_adjust);
extern pm_err_flag_type
pm_hal_rtc_set_time_adjust(uint8 time_adjust);

/**
 * @name pm_dev_hal_rtc_get_time_adjust
 *
 * @brief Returns the current time adjustment correction factor in use, as
 *        set by the previous pm_hal_rtc_set_time_adjust().
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] time_adjust_ptr Pointer to the adjustment factor (uint8).
 *              Valid inputs: Non-NULL pointer to a valid uint8 containing
 *              - 0 to 63 - Compensation for a slower crystal oscillator.
 *              - 64 - No compensation.
 *              - 65 to 127 - Compensation for a faster crystal oscillator.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() and pm_hal_rtc_set_time_adjust() must have been called.
 *
 * @warning Interrupts are disabled during this function.
 *
 * @note Do not use pm_hal_rtc_get_time_adjust(), which is deprecated. Use pm_dev_hal_rtc_get_time_adjust() instead.
 */
extern pm_err_flag_type
pm_dev_hal_rtc_get_time_adjust(uint8 pmic_chip,
                               uint8 *time_adjust_ptr);
extern pm_err_flag_type
pm_hal_rtc_get_time_adjust(uint8 *time_adjust_ptr);

/**
 * @name pm_dev_hal_rtc_prescaler_reset
 *
 * @brief Reset the 1/32768 prescalar in the RTC module.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @note Do not use pm_hal_rtc_prescaler_reset(), which is deprecated. Use pm_dev_hal_rtc_prescaler_reset() instead.
 */
extern pm_err_flag_type pm_dev_hal_rtc_prescaler_reset(uint8 pmic_chip);
extern pm_err_flag_type pm_hal_rtc_prescaler_reset(void);

/**
 * @name pm_dev_hard_reset
 *
 * @brief Initiate hard reset using the RTC module.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 */
extern pm_err_flag_type pm_dev_hard_reset(uint8 pmic_chip);

#endif    /* PM_RTC_H */

