#ifndef PM_RTC_H
#define PM_RTC_H

/*===========================================================================

           R T C   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  This file contains functions prototypes and variable/type/constant 
  declarations for the RTC services developed for the Qualcomm Power
  Management IC.
  
Copyright (c) 2003, 2004, 2005, 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */

// RTC time struct used for setting and retrieving current time and also for
// setting the alarm time
typedef struct
{
   uint32  sec;
} pm_hal_rtc_time_type;

// Type for selecting different physical alarms available in RTC
typedef enum
{
   PM_HAL_RTC_ALARM_1,
   PM_HAL_RTC_ALARM_INVALID
} pm_hal_rtc_alarm;

// Status of the various physical alarms available in RTC
typedef struct
{
   boolean  alarm_1_triggered;
} pm_hal_rtc_alarm_status_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/*===========================================================================
FUNCTION  pm_hal_rtc_start                                 EXTERNAL FUNCTION

DESCRIPTION
   This function gets the RTC ticking with the indicated time as its current
   (start-up) time.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   
   2) Name: start_time_ptr
      - Pointer to the start-up time for the real-time clock.
      Type: pm_hal_rtc_time_type*
      - Valid inputs:
           Pointer to a valid non-NULL RTC time structure containing any 32
           bit number indicating the number of seconds elapsed from a
           known point in time in history.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> NULL pointer provided for 
                                             start_time_ptr.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_start(
   unsigned pmic_device_index, 
   const pm_hal_rtc_time_type *start_time_ptr
);
//#pragma message("Do not use pm_hal_rtc_start(), which is deprecated. Use pm_dev_hal_rtc_start() instead.")
extern pm_err_flag_type pm_hal_rtc_start(const pm_hal_rtc_time_type *start_time_ptr);



/*===========================================================================
FUNCTION  pm_hal_rtc_stop                                  EXTERNAL FUNCTION

DESCRIPTION
   This function stops the RTC from ticking.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.           

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_stop(unsigned pmic_device_index);
//#pragma message("Do not use pm_hal_rtc_stop(), which is deprecated. Use pm_dev_hal_rtc_stop() instead.")
extern pm_err_flag_type pm_hal_rtc_stop(void);



/*===========================================================================
FUNCTION  pm_hal_rtc_get_time                              EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current time of the RTC. This will be the actual
   present time if the RTC has been ticking or the time at which the RTC
   was last stopped.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: time_ptr
      - RTC structure used for returning the current time of the real-time clock.
      Type: pm_hal_rtc_time_type*
      - Valid inputs:
           Non-NULL valid RTC time structure

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__RTC_HALTED            -> RTC is not running.     
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> NULL pointer provided for 
                                             time_ptr.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_hal_rtc_start()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_get_time(
    unsigned pmic_device_index, 
    pm_hal_rtc_time_type *time_ptr
);
//#pragma message("Do not use pm_hal_rtc_get_time(), which is deprecated. Use pm_dev_hal_rtc_get_time() instead.")
extern pm_err_flag_type pm_hal_rtc_get_time(pm_hal_rtc_time_type *time_ptr);



/*===========================================================================
FUNCTION  pm_hal_rtc_enable_alarm                          EXTERNAL FUNCTION

DESCRIPTION
   This function enables the chosen alarm to go off at the specified time.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: what_alarm
      - Indicates what alarm to be turned ON.
      Type: pm_hal_rtc_alarm
      - Valid inputs:
           PM_HAL_RTC_ALARM_1
   2) Name: trigger_time_ptr
      - Pointer to the time at which the alarm should go off.
      Type: pm_hal_rtc_time_type*
      - Valid inputs:
           Pointer to a valid non-NULL RTC time structure containing any 32 bit
           number indicating the number of seconds that would have 
           elapsed (in relation to a known point in time in history as specified in 
           the previous pm_hal_rtc_start() call) exactly at the moment the alarm is
           expected to go off.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Unknown alarm specified
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     -> NULL pointer provided for 
                                             trigger_time_ptr
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_hal_rtc_start()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_enable_alarm(    
    unsigned pmic_device_index, 
    pm_hal_rtc_alarm             what_alarm,
    const pm_hal_rtc_time_type  *trigger_time_ptr
);
//#pragma message("Do not use pm_hal_rtc_enable_alarm(), which is deprecated. Use pm_dev_hal_rtc_enable_alarm() instead.")
extern pm_err_flag_type pm_hal_rtc_enable_alarm(
    pm_hal_rtc_alarm             what_alarm,
    const pm_hal_rtc_time_type  *trigger_time_ptr
    );



/*===========================================================================
FUNCTION  pm_hal_rtc_disable_alarm                         EXTERNAL FUNCTION

DESCRIPTION
   This function disables the specified alarm so that it doesn't go off in
   the future. If the alarm has already triggered by the time this function
   was called, it will also clear the alarm trigger condition so that the
   master RTC alarm interrupt could be cleared afterward.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: what_alarm
      - Indicates what alarm to be turned OFF.
      Type: pm_hal_rtc_alarm
      - Valid inputs:
           PM_HAL_RTC_ALARM_1

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Unknown alarm specified.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_disable_alarm(
    unsigned pmic_device_index, 
    pm_hal_rtc_alarm  what_alarm
);
//#pragma message("Do not use pm_hal_rtc_disable_alarm(), which is deprecated. Use pm_dev_hal_rtc_disable_alarm() instead.")
extern pm_err_flag_type pm_hal_rtc_disable_alarm(pm_hal_rtc_alarm  what_alarm);



/*===========================================================================
FUNCTION  pm_hal_rtc_get_alarm_time                        EXTERNAL FUNCTION

DESCRIPTION
   This function returns the time at which the specified alarm has been set
   to go off.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: what_alarm
      - Indicates what alarm to be read.
      Type: pm_hal_rtc_alarm
      - Valid inputs:
           PM_HAL_RTC_ALARM_1
   3) Name: alarm_time_ptr
      - Pointer to the time structure that is used by this function to return
        the time at which the specified alarm has been programmed to trigger.
      Type: pm_hal_rtc_time_type*
      - Valid inputs:
           Pointer to a valid non-NULL RTC time structure.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Unknown alarm specified.
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     -> NULL pointer provided for 
                                             alarm_time_ptr
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_get_alarm_time(
   unsigned pmic_device_index, 
   pm_hal_rtc_alarm       what_alarm,
   pm_hal_rtc_time_type  *alarm_time_ptr
);
//#pragma message("Do not use pm_hal_rtc_get_alarm_time(), which is deprecated. Use pm_dev_hal_rtc_get_alarm_time() instead.")
extern pm_err_flag_type pm_hal_rtc_get_alarm_time(
    pm_hal_rtc_alarm       what_alarm,
    pm_hal_rtc_time_type  *alarm_time_ptr
    );



/*===========================================================================
FUNCTION  pm_hal_rtc_get_alarm_status                      EXTERNAL FUNCTION

DESCRIPTION
   This function returns the status of the various alarms supported in
   hardware. A value of 1 means that the alarm has triggered and 0 means
   otherwise.

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: status_ptr
      -  Pointer to a valid uint8 used for returning the status info
         for the various alarms (currently only one alarm, PM_HAL_RTC_ALARM_1).
      Type: uint8*
      - Valid inputs:
         Pointer to a valid uint8 variable.
              Bit7   Bit6   Bit5   Bit4   Bit3   Bit2   Bit1   Bit0
                                                                ^ 
                                                                |
                                                            Alarm1's status

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> NULL pointer provided for 
                                             status_ptr.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_get_alarm_status(unsigned pmic_device_index, uint8 *status_ptr);
//#pragma message("Do not use pm_hal_rtc_get_alarm_status(), which is deprecated. Use pm_dev_hal_rtc_get_alarm_status() instead.")
extern pm_err_flag_type pm_hal_rtc_get_alarm_status(uint8 *status_ptr);



/*===========================================================================
FUNCTION  pm_hal_rtc_set_time_adjust                       EXTERNAL FUNCTION

DESCRIPTION
   This function sets the time adjustment correction factor for the RTC crystal
   oscillator that is slightly off the 32768 Hz frequency. Every 10th second,
   the freq divider is switched from the nominal 32768 to (32768 - 64 + time_adjust).

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: time_adjust
      - Adjustment factor.
      Type: uint8
      - Valid inputs:
           0 to 63    -->  Compensates for a slower crystal oscillator
           64         -->  Provides for no compensation
           65 to 127  -->  Compensates for a faster crystal oscillator

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> Out-of-range value specified for 
                                             time_adjust.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_set_time_adjust(
    unsigned pmic_device_index,
    uint8 time_adjust
);
//#pragma message("Do not use pm_hal_rtc_set_time_adjust(), which is deprecated. Use pm_dev_hal_rtc_set_time_adjust() instead.")
extern pm_err_flag_type pm_hal_rtc_set_time_adjust(uint8 time_adjust);



/*===========================================================================
FUNCTION  pm_hal_rtc_get_time_adjust                       EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current time adjustment correction factor in use as
   set by the previous pm_hal_rtc_set_time_adjust().

PARAMETERS
   1) @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
   2) Name: time_adjust_ptr
      - Pointer to the adjustment factor.
      Type: uint8*
      - Valid inputs:
           Non-NULL pointer to a valid uint8 containing:
           0 to 63    -->  Compensation for a slower crystal oscillator
           64         -->  No compensation
           65 to 127  -->  Compensation for a faster crystal oscillator

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               -> Operation succeeded.
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     -> NULL pointer provided in 
                                             time_adjust_ptr.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -> Feature not available on this 
                                             version of the PMIC.      
     - PM_ERR_FLAG__SBI_OPT_ERR           -> Communication with PM chip failed.

DEPENDENCIES
   The following function must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_hal_rtc_set_time_adjust()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_dev_hal_rtc_get_time_adjust(
    unsigned pmic_device_index, 
    uint8 *time_adjust_ptr
);
//#pragma message("Do not use pm_hal_rtc_get_time_adjust(), which is deprecated. Use pm_dev_hal_rtc_get_time_adjust() instead.")
extern pm_err_flag_type pm_hal_rtc_get_time_adjust(uint8 *time_adjust_ptr);

/**
 * Function pm_hal_rtc_prescaler_reset
 * 
 * Reset the 1/32768 prescaler in the RTC module
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_dev_hal_rtc_prescaler_reset(unsigned pmic_device_index);
//#pragma message("Do not use pm_hal_rtc_prescaler_reset(), which is deprecated. Use pm_dev_hal_rtc_prescaler_reset() instead.")
extern pm_err_flag_type pm_hal_rtc_prescaler_reset(void);

/**
 * 
 * @brief Initiate hard reset using the RTC module
 *
 * @param pmic_device_index Selects the device in which the RTC module exists. 
 *  *                        
 * 
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
 */
extern pm_err_flag_type pm_dev_hard_reset(unsigned pmic_device_index);

#endif // PM_RTC_H


