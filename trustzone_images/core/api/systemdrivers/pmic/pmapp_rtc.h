
#ifndef PMAPP_RTC_H
#define PMAPP_RTC_H

/*===========================================================================
              P M   R T C   A P P   H E A D E R   F I L E

DESCRIPTION
  This file contains functions prototypes and variable/type/constant
  declarations for the real-time clock application developed for the Qualcomm
  Power Manager Chip Set.

Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pmapp_rtc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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


/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "pm_lib_err.h"

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
// Real-time clock command type
typedef enum
{
   PM_RTC_SET_CMD,
   PM_RTC_GET_CMD,
   PM_RTC_INVALID_CMD
}pm_rtc_cmd_type;

// Real-time clock display mode type
typedef enum
{
   PM_RTC_12HR_MODE,
   PM_RTC_24HR_MODE,
   PM_RTC_INVALID_MODE
}pm_rtc_display_type;

// PMIC's version of the Julian time structure
typedef struct
{
   word year;            // Year [1980..2100]
   word month;           // Month of year [1..12]
   word day;             // Day of month [1..31]
   word hour;            // Hour of day [0..23]
   word minute;          // Minute of hour [0..59]
   word second;          // Second of minute [0..59]
   word day_of_week;     // Day of the week [0..6] Monday..Sunday
} pm_rtc_julian_type;

// RTC alarms
typedef enum
{
   PM_RTC_ALARM_1       = 0x01,
   PM_RTC_ALL_ALARMS    = 0x01,       // This is used for referring collectively to all of the supported alarms
   PM_RTC_ALARM_INVALID
} pm_rtc_alarm_type;


/*===========================================================================
                  REAL TIME CLOCK FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
FUNCTION   pm_rtc_init                                     EXTERNAL FUNCTION

DESCRIPTION
   This function is used for initializing RTC after a "power reset".

PARAMETERS
   None.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked in this function.
===========================================================================*/
extern pm_err_flag_type pm_rtc_init(void);
/*~ FUNCTION pm_rtc_init */


/*===========================================================================
FUNCTION   pm_get_rtc_display_mode                         EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current real time clock display mode
   (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power up and
   remains so until it is set to 12 hr mode explicitly using
   pm_set_rtc_display_mode().

PARAMETERS
   1) Name: mode_ptr
      - Pointer that is used to return the real time clock display mode.
      Type: pm_rtc_display_type*.
      - Valid values pointed to by the pointer:
         PM_RTC_12HR_MODE
         PM_RTC_24HR_MODE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  NULL pointer provided for mode_ptr

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   None.
===========================================================================*/
extern pm_err_flag_type pm_get_rtc_display_mode(
   pm_rtc_display_type  *mode_ptr);
/*~ FUNCTION pm_get_rtc_display_mode */
/*~ PARAM OUT mode_ptr POINTER */


/*===========================================================================
FUNCTION   pm_set_rtc_display_mode                         EXTERNAL FUNCTION

DESCRIPTION
   This function configures the real time clock display mode
   (24 or 12 hour mode). RTC defaults to 24 hr mode on phone power up and
   remains so until it is set to 12 hr mode explicitly using
   pm_set_rtc_display_mode().

PARAMETERS
   1) Name: mode
      - New RTC time display mode to be used.
      Type: pm_rtc_display_type
      - Valid values:
         PM_RTC_12HR_MODE
         PM_RTC_24HR_MODE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  Invalid display mode indicated by 'mode'

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   None.
===========================================================================*/
extern pm_err_flag_type pm_set_rtc_display_mode(
   pm_rtc_display_type mode);
/*~ FUNCTION pm_set_rtc_display_mode */


/*===========================================================================
FUNCTION   pm_rtc_rw_cmd                                   EXTERNAL FUNCTION

DESCRIPTION
   This function reads/writes the time and date from/to the PMIC RTC.
   The time/date format must be in 24 or 12 Hr mode depending on
   what mode, RTC has been initialized with; please refer to the description of
   pm_set_rtc_display_mode() for details.

   24 Hr and 12 Hr mode displays are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31


PARAMETERS
   1) Name: cmd
      - Indicates if we want to set/get the current time in RTC.
      Type: pm_rtc_cmd_type
      - Valid Values:
         PM_RTC_SET_CMD
         PM_RTC_GET_CMD

   2) Name: current_time_ptr
      - Depending on the command, this function will use the structure
        pointer to update or return the current time in the RTC.
      Type: pm_rtc_julian_type*
         word year         = Year [1980..2100]
         word month        = Month of year [1..12]
         word day          = Day of month [1..31]
         word hour         = Depending on the display mode. Refer to the
                             function description.
         word minute       = Minute of hour [0..59]
         word second       = Second of minute [0..59]
         word day_of_week  = Day of the week [0..6] Monday through Sunday

         Note: 'day_of_week' is not required for setting the current time but
               returns the correct information when retrieving time from the RTC.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid command 'cmd'
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE  -->  NULL pointer provided for current_time_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed     
     - PM_ERR_FLAG__RTC_HALTED         -->  RTC is not running
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_set_rtc_display_mode()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type  pm_rtc_rw_cmd(
   pm_rtc_cmd_type       cmd,
   pm_rtc_julian_type   *current_time_ptr);
/*~ FUNCTION pm_rtc_rw_cmd */
/*~ PARAM INOUT current_time_ptr POINTER */


/*===========================================================================
FUNCTION   pm_is_rtc_valid                                 EXTERNAL FUNCTION

DESCRIPTION
   Use this function to check if the RTC time going in or out of the
   PMIC if valid (within range). This function is meant to be used
   in conjunction with "pm_rtc_rw_cmd()".

   Example:
      pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);

      if (pm_is_rtc_valid(rtcMode, &time))
      {
         // RTC time is valid
      }

PARAMETERS
   1) Name: rtcMode
      - Display mode (24 or 12 hr)
      Type: pm_rtc_display_type
      - Valid Values:
            PM_RTC_12HR_MODE
            PM_RTC_24HR_MODE

   2) Name: time
      - Pointer to the Julian time structure that is to be validated.
      Type: pm_rtc_julian_type* Structure pointer.
     - Structure members:

        word year         = Year [1980..2100]
        word month        = Month of year [1..12]
        word day          = Day of month [1..31]
        word hour         = Depending on the display mode
        word minute       = Minute of hour [0..59]
        word second       = Second of minute [0..59]
        word day_of_week  = Day of the week [0..6] Monday through Sunday

RETURN VALUE
   Type: boolean
   - Possible values:
     - TRUE   -->  Time information pointed to by 'time' is valid
     - FALSE  -->  Otherwise

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern boolean pm_is_rtc_valid(
   pm_rtc_display_type          rtcMode,
   const pm_rtc_julian_type    *time);
/*~ FUNCTION pm_is_rtc_valid */
/*~ PARAM IN time POINTER */


/*===========================================================================
FUNCTION   pm_rtc_set_time_adjust                          EXTERNAL FUNCTION

DESCRIPTION
   This function sets the value for the 32768 Hz frequency offset
   adjustment in parts-per-million (PPM). In other words, adjust this value to
   compensate for inaccuracies in the 32768 Hz external crystal, which is used
   by the PMIC to run the RTC.

PARAMETERS
   Name: time_adjust
   - Adjustment (in PPM) = time_adjust * (3.05)
   Type: uint8
   - bits 0:5 represent the adjust value.
   - bit 6 is the sign bit
      1 --> Increase frequency (to compensate for slower than 32768 Hz crystal)
      0 --> Decrease frequency (to compensate for faster than 32768 Hz crystal)

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid value specified for time_adjust
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type pm_rtc_set_time_adjust(uint8 time_adjust);
/*~ FUNCTION pm_rtc_set_time_adjust */


/*===========================================================================
FUNCTION   pm_rtc_get_time_adjust                          EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current value used for the 32768 Hz frequency offset
   adjustment in PPM. This adjustment compensates for inaccuracies in the 32768 Hz
   external crystal, which is used by the PMIC to run the RTC.

PARAMETERS
   Name: time_adjust_ptr
   - Pointer to an 'uint8' used for returning the adjustment; use the following
     equation to calculate the frequency adjustment in parts-per-million (PPM):
                adjustment (PPM) = "adjustment value" * (3.05)
   Type: uint8* and the de-referenced uint8 has to be interpreted as follows
   - bits 0:5 represent the "adjustment value".
   - bit 6 is the sign bit
      1 --> Increase frequency (to compensate for slower than 32768 Hz crystal)
      0 --> Decrease frequency (to compensate for faster than 32768 Hz crystal)

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  NULL provided for time_adjust_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type pm_rtc_get_time_adjust(uint8* time_adjust_ptr);
/*~ FUNCTION pm_rtc_get_time_adjust */
/*~ PARAM OUT time_adjust_ptr POINTER */


/*===========================================================================
FUNCTION   pm_rtc_alarm_rw_cmd                             EXTERNAL FUNCTION

DESCRIPTION
   This function reads/writes the alarm time and date from/to the PMIC RTC.
   The time/date format must be in 24 or 12 Hr mode depending on
   what mode, RTC has been initialized with; please refer to the description of
   pm_set_rtc_display_mode() for details.

   24 Hr and 12 Hr mode displays are related as shown below:

   24 HR-00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23
   12 HR-12 01 02 03 04 05 06 07 08 09 10 11 32 21 22 23 24 25 26 27 28 29 30 31


PARAMETERS
   1) Name: cmd
      - Indicates if we want to set/get the current alarm time in RTC.
      Type: pm_rtc_cmd_type
      - Valid Values:
         PM_RTC_SET_CMD
         PM_RTC_GET_CMD

   2) Name: what_alarm
      - Indicates the specific alarm to be set/queried in RTC.
      Type: pm_rtc_alarm_type
      - Valid Values:
         PM_RTC_ALARM_1

   3) Name: alarm_time_ptr
      - Depending on the command, this function will use the structure
        pointer to update or return the current alarm time in the RTC.
      Type: pm_rtc_julian_type*
         word year         = Year [1980..2100]
         word month        = Month of year [1..12]
         word day          = Day of month [1..31]
         word hour         = Depending on the display mode. Refer to the
                             function description.
         word minute       = Minute of hour [0..59]
         word second       = Second of minute [0..59]
         word day_of_week  = Day of the week [0..6] Monday through Sunday

         Note: 'day_of_week' is not required for setting the current alarm time but
               returns the correct information when retrieving the current
               programmed alarm time from the RTC.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE  -->  Invalid command 'cmd'
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE  -->  Unknown alarm info provided for 'what_alarm'
     - PM_ERR_FLAG__PAR3_OUT_OF_RANGE  -->  NULL pointer provided for alarm_time_ptr
     - PM_ERR_FLAG__SBI_OPT_ERR        -->  Communication with RTC failed
     - PM_ERR_FLAG__SUCCESS            -->  SUCCESS

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_set_rtc_display_mode()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type pm_rtc_alarm_rw_cmd(
   pm_rtc_cmd_type       cmd,
   pm_rtc_alarm_type     what_alarm,
   pm_rtc_julian_type   *alarm_time_ptr);
/*~ FUNCTION pm_rtc_alarm_rw_cmd */
/*~ PARAM INOUT alarm_time_ptr POINTER */


/*===========================================================================
FUNCTION   pm_reset_rtc_alarm                              EXTERNAL FUNCTION

DESCRIPTION
   This function disables the specified alarm so that it doesn't go off in
   the future. If the alarm has already triggered by the time this function
   was called, it will also clear the alarm trigger condition so that the
   master RTC alarm interrupt could be cleared afterward.

PARAMETERS
   1) Name: what_alarm
      - Indicates what alarm to be turned OFF.
      Type: pm_rtc_alarm_type
      - Valid inputs:
           PM_RTC_ALARM_1
           PM_RTC_ALL_ALARMS
      Note: If multiple alarms are available in the future, a subset of that
      can be selectively chosen using the bit-wise OR (|) operator

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  Unknown alarm(s) specified
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type pm_reset_rtc_alarm(pm_rtc_alarm_type what_alarm);
/*~ FUNCTION pm_reset_rtc_alarm */


/*===========================================================================
FUNCTION   pm_get_rtc_alarm_status                         EXTERNAL FUNCTION

DESCRIPTION
   This function returns the status of the various alarms supported in
   RTC. A value of 1 means that the alarm has triggered and 0 means
   otherwise.

PARAMETERS
   1) Name: alarm_status
      -  Pointer to a valid uint8 used for returning the status info
         for the various alarms (currently only one alarm, PM_RTC_ALARM_1).
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
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE -->  NULL pointer provided for 'alarm_status'
     - PM_ERR_FLAG__SBI_OPT_ERR       -->  Communication with RTC failed

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked for the duration of this function.
===========================================================================*/
extern pm_err_flag_type pm_get_rtc_alarm_status(uint8 *alarm_status);
/*~ FUNCTION pm_get_rtc_alarm_status */
/*~ PARAM OUT alarm_status POINTER */


/*===========================================================================
FUNCTION  pm_rtc_pwr_rst_isr                                  LOCAL FUNCTION
DESCRIPTION
  RTC power reset IRQ interrupt service routine; it initializes RTC after
  a power reset.
PARAMETERS
  None.
RETURN VALUE
  None.
DEPENDENCIES
  The following functions need to be called before we call this function:
  1) rflib_init()
  2) pm_init()
  3) pm_init_delayed()
SIDE EFFECTS
  Clears the RTC power reset IRQ
===========================================================================*/
extern void pm_rtc_pwr_rst_isr(void);


/*===========================================================================
FUNCTION   pm_rtc_get_power_reset_status               EXTERNAL FUNCTION

DESCRIPTION
   This function returns the status of rtc power reset latched status
   A value of 1 means that power reset happened and 0 means otherwise.

PARAMETERS
   None

RETURN VALUE
   None

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   None
===========================================================================*/
extern boolean pm_rtc_get_power_reset_status(void);
/*~ FUNCTION pm_rtc_get_power_reset_status */

#endif /* PMAPP_RTC_H  */
