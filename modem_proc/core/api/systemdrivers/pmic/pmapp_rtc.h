#ifndef PMAPP_RTC_H
#define PMAPP_RTC_H

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
 * @file pmapp_rtc.h PMIC RTC APPLICATION related declaration.
 *
 * @brief This file contains functions prototypes and variable/type/constant
 *        declarations for the real-time clock application developed for the
 *		  Qualcomm Power Manager Chip Set.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pmapp_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
02/13/09   vk      Removed feature FEATURE_PMIC_RTC from .h file for RPC. 
06/28/07   cng     Added meta comments to RTC APIs
06/18/07   cng     Added meta comments to pm_rtc_rw_cmd
03/07/07   aab     Added pm_rtc_get_power_reset_status() API
05/31/06   Vish    Fixed LINT warnings.
01/19/05   Vish    Updated function header for pm_rtc_rw_cmd().
11/07/03   Vish    Added the task of clearing the alarm trigger condition
                   within pm_reset_rtc_alarm() so that the master
                   RTC alarm interrupt could be cleared afterward. Also
                   resetting all alarms within pm_rtc_init() to avoid
                   any false alarms.
11/03/03   Vish    Changed signature of pm_rtc_init().
                   Renamed pm_s/get_time_adjust() to pm_rtc_s/get_time_adjust()
                   in order to be consistent with earlier PM chip libraries.
                   Added documentation. Wrapped this file under
                   FEATURE_PMIC_RTC.
10/02/03   Vish    Adding alarm support.
09/24/03   Vish    Added pm_rtc_init().
09/19/03   Vish    Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_rtc_cmd_type
 * @brief Real-time clock command type.
 */
typedef enum
{
   PM_RTC_SET_CMD,
   PM_RTC_GET_CMD,
   PM_RTC_INVALID_CMD
}pm_rtc_cmd_type;

/**
 * @enum pm_rtc_display_type
 * @brief Real-time clock display mode type.
 */
typedef enum
{
   PM_RTC_12HR_MODE,
   PM_RTC_24HR_MODE,
   PM_RTC_INVALID_MODE
}pm_rtc_display_type;

/**
 * @struct pm_rtc_julian_type
 * @brief PMIC's version of the Julian time structure.
 */
typedef struct
{
   word year;            /*!< Year [1980..2100] */
   word month;           /*!< Month of year [1..12] */
   word day;             /*!< Day of month [1..31] */
   word hour;            /*!< Hour of day [0..23] */
   word minute;          /*!< Minute of hour [0..59] */
   word second;          /*!< Second of minute [0..59] */
   word day_of_week;     /*!< Day of the week [0..6] Monday..Sunday */
} pm_rtc_julian_type;

/**
 * @enum pm_rtc_alarm_type
 * @brief RTC alarms.
 */
typedef enum
{
   PM_RTC_ALARM_1       = 0x01,
   PM_RTC_ALL_ALARMS    = 0x01,       // This is used for referring collectively to all of the supported alarms
   PM_RTC_ALARM_INVALID
} pm_rtc_alarm_type;


/*===========================================================================

                  REAL TIME CLOCK FUNCTION PROTOTYPES
				  
===========================================================================*/
/**
 * @name pm_rtc_init
 *
 * @brief This function is used for initializing RTC after a "power reset".
 *
 * @param [in] void
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_rtc_init(void);

/**
 * @name pm_get_rtc_display_mode
 *
 * @brief This function returns the current real time clock display mode
 * 		  (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power up and
 *   	  remains so until it is set to 12 hr mode explicitly using
 * 		  pm_set_rtc_display_mode().
 *
 * @param [out] mode_ptr Pointer that is used to return the real time clock display mode.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_get_rtc_display_mode(pm_rtc_display_type  *mode_ptr);

/**
 * @name pm_set_rtc_display_mode
 *
 * @brief This function configures the real time clock display mode
 *		  (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power
 *		  up and remains so until it is set to 12 hr mode explicitly using
 *		  pm_set_rtc_display_mode().
 *
 * @param [in] mode New RTC time display mode to be used.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_set_rtc_display_mode(pm_rtc_display_type mode);

/**
 * @name pm_rtc_rw_cmd
 *
 * @brief This function reads/writes the time and date from/to the PMIC RTC.
 *		  The time/date format must be in 24 or 12 Hr mode depending on
 *		  what mode, RTC has been initialized with; please refer to the
 *		  description of pm_set_rtc_display_mode() for details.
 *
 * @param [in] cmd Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] current_time_ptr Depending on the command, this function will
 *								use the structure pointer to update or return
 *								the current time in the RTC.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type  pm_rtc_rw_cmd(pm_rtc_cmd_type cmd,
										pm_rtc_julian_type *current_time_ptr);

/**
 * @name pm_is_rtc_valid
 *
 * @brief Use this function to check if the RTC time going in or out of the
 *		  PMIC if valid (within range). This function is meant to be used
 *		  in conjunction with "pm_rtc_rw_cmd()".
 *
 * @param [in] rtcMode Display mode (24 or 12 hr)
 * @param [in] time Pointer to the Julian time structure that is to be validated.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern boolean pm_is_rtc_valid(pm_rtc_display_type rtcMode,
								const pm_rtc_julian_type *time);

/**
 * @name pm_rtc_set_time_adjust
 *
 * @brief This function sets the value for the 32768 Hz frequency offset
 *		  adjustment in parts-per-million (PPM). In other words, adjust this
 *		  value to compensate for inaccuracies in the 32768 Hz external crystal,
 *		  which is used by the PMIC to run the RTC.
 *
 * @param [in] time_adjust Adjustment (PPM) = "adjustment value" * (3.05)
 *				- bits 0:5 represent the "adjustment value".
 *				- bit 6 is the sign bit
 *					1: Increase frequency (to compensate for slower than 32768 Hz crystal)
 *					0: Decrease frequency (to compensate for faster than 32768 Hz crystal)
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_rtc_set_time_adjust(uint8 time_adjust);

/**
 * @name pm_rtc_get_time_adjust
 *
 * @brief This function returns the current value used for the 32768 Hz
 *		  frequency offset adjustment in PPM. This adjustment compensates for
 *		  inaccuracies in the 32768 Hz external crystal, which is used by the
 *		  PMIC to run the RTC.
 *
 * @param [out] time_adjust_ptr Pointer to an 'uint8' used for returning the adjustment;
 *				use the following equation to calculate the frequency adjustment
 *				in parts-per-million (PPM):
 *  			adjustment (PPM) = "adjustment value" * (3.05)
 *				- bits 0:5 represent the "adjustment value".
 *				- bit 6 is the sign bit
 *					1: Increase frequency (to compensate for slower than 32768 Hz crystal)
 *					0: Decrease frequency (to compensate for faster than 32768 Hz crystal)
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_rtc_get_time_adjust(uint8* time_adjust_ptr);

/**
 * @name pm_rtc_alarm_rw_cmd
 *
 * @brief This function reads/writes the alarm time and date from/to the PMIC RTC.
 *		  The time/date format must be in 24 or 12 Hr mode depending on what
 *		  mode, RTC has been initialized with.
 *
 * @param [in] cmd Indicates if we want to set/get the current alarm time in RTC.
 * @param [in] what_alarm Indicates the specific alarm to be set/queried in RTC.
 *
 * @param [out] alarm_time_ptr Depending on the command, this function will use
 *								the structure pointer to update or return the
 *								current alarm time in the RTC.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_rtc_alarm_rw_cmd(pm_rtc_cmd_type cmd,
                                            pm_rtc_alarm_type what_alarm,
                                            pm_rtc_julian_type *alarm_time_ptr);
											
/**
 * @name pm_reset_rtc_alarm
 *
 * @brief This function disables the specified alarm so that it doesn't go
 *		  off in the future. If the alarm has already triggered by the time
 *		  this function was called, it will also clear the alarm trigger
 *		  condition so that the master RTC alarm interrupt could be cleared afterwards.
 *
 * @param [in] what_alarm Indicates what alarm to be turned OFF.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_reset_rtc_alarm(pm_rtc_alarm_type what_alarm);

/**
 * @name pm_get_rtc_alarm_status
 *
 * @brief This function returns the status of the various alarms supported in
 *		  RTC. A value of 1 means that the alarm has triggered and 0 means
 *		  otherwise.
 *
 * @param [out] alarm_status Pointer to a valid uint8 used for returning the
 *							 status info for the various alarms (currently
 *							 only one alarm, PM_RTC_ALARM_1).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 * @warning Interrupts are locked for the duration of this function.
 *
 */
extern pm_err_flag_type pm_get_rtc_alarm_status(uint8 *alarm_status);

/**
 * @name pm_rtc_pwr_rst_isr
 *
 * @brief RTC power reset IRQ interrupt service routine; it initializes RTC
 *		  after a power reset.
 *
 * @param [in] void
 *
 * @return  void
 *
 * @remarks pm_init()function(s) must have been called.
 *
 * @warning Clears the RTC power reset IRQ
 *
 */
extern void pm_rtc_pwr_rst_isr(void);

/**
 * @name pm_rtc_get_power_reset_status
 *
 * @brief This function returns the status of rtc power reset latched status
 *		  A value of 1 means that power reset happened and 0 means otherwise.
 *
 * @param [in] void
 *
 * @param [out] OnOff The On or Off command for the timer
 *
 * @return boolean. TRUE, FALSE.
 *
 * @remarks pm_init() function(s) must have been called.
 *
 */
extern boolean pm_rtc_get_power_reset_status(void);

/** @} */ /* end_addtogroup pm_vs */

#endif /* PMAPP_RTC_H  */
