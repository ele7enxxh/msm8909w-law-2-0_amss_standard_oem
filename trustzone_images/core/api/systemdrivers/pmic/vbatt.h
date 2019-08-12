#ifndef VBATT_H
#define VBATT_H
/*===========================================================================

    B A T T E R Y   L E V E L   U T I L I T I E S   H E A D E R   F I L E

DESCRIPTION
  This header file contains all of the definitions necessary for other
  tasks to interface with the battery level utilities.

REFERENCES
  None

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/vbatt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/10   dy      Expanded battery_adc_params_type struct
12/10/09   jtn     Remove featurization for CMI
10/06/09   jtn     Make changes needed for DAL ADC interface.
05/28/09   APU     Introduced an API to read battery temperature.
03/11/09   APU     Added APIs to support the new VBATT info structure and 
                   also support the CHG callback functionality.
12/25/08   APU     Introduced the API: vbatt_read_mv.  
11/14/03   Vish    Added vbatt_get_adc_count_range_cal_info() for retrieving
                   ADC count range calibration info needed by other drivers
                   like ADC. 
08/16/02   rmd     Added the PMIC low battery alarm.
10/06/00   dsb     Removed TG== code.
09/11/98   aks     Added definitions for dual-battery (5GP) gauging support.
12/08/97   aks     Added definitions for internal charger and multiple battery
                   capability.
11/02/97   BK      Added missing T_Q.
09/17/97   aks     Improved overall precision and temperature compensation.  Power
                   drop and non-linear thermistor compensation added.
08/15/97   aks     Made vbatt_read_gauge() defined only for TGP targets.
08/13/97   aks     Added the externalized vbatt_read_gauge() function and
                   associated return type definitions.
03/01/95   tst     Changed to use VBATT_SCALED_MIN instead of 0.
02/14/95   tst     Added VBATT_SCALED_MIN.
02/04/93   jah     Changed vbatt_calibrate() to take byte inputs.
12/17/92   jah     Created

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* The range of Vbatt readings [VBATT_SCALED_MIN,VBATT_SCALE] returned
** by vbatt_read
*/
#define VBATT_SCALED_MIN   0
#define VBATT_SCALE      255

/*
**  The following information from battery_adc_params_type is not provided
**  by the PMIC. It is intended for customers to provide this information
**  if they are using an external battery management IC.
**
**   uint8  ACLineStatus;
**   uint8  BatteryFlag;
**   uint8  BatteryLifePercent;
**   int32  BatteryLifeTime;
**   int32  BatteryFullLifeTime;
**   uint8  BackupBatteryFlag;
**   uint8  BackupBatteryLifePercent;
**   int32  BackupBatteryLifeTime;
**   int32  BackupBatteryFullLifeTime;
**   int32  BatteryCurrent;
**   int32  BatteryAverageCurrent;
**   int32  BatteryAverageInterval;
**   int32  BatterymAHourConsumed;
**   int32  BackupBatteryVoltage;
**   uint8  BatteryChemistry;
*/
typedef struct
{
    uint16 current_batt_adc_voltage ; 
    uint16 previous_batt_adc_voltage ;
    int16  batt_temperature ;
    uint16 batt_id ;
    int32  pmic_temperature;
    int32  ichg_out_mv;
    int32  ichg_mv;
    int32  vchg_mv;
    int32  chg_temperature;
    int32  usb_vbus;
    uint8  current_batt_adc_raw;
    uint8  ACLineStatus;
    uint8  BatteryFlag;
    uint8  BatteryLifePercent;
    int32  BatteryLifeTime;
    int32  BatteryFullLifeTime;
    uint8  BackupBatteryFlag;
    uint8  BackupBatteryLifePercent;
    int32  BackupBatteryLifeTime;
    int32  BackupBatteryFullLifeTime;
    int32  BatteryCurrent;
    int32  BatteryAverageCurrent;
    int32  BatteryAverageInterval;
    int32  BatterymAHourConsumed;
    int32  BackupBatteryVoltage;
    uint8  BatteryChemistry;
} battery_adc_params_type ;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION VBATT_READ

DESCRIPTION
  This function reads the current battery level.

DEPENDENCIES
  None

RETURN VALUE
  A scaled version of the batter level where empty is VBATT_SCALED_MIN,
  and full is VBATT_SCALE.

SIDE EFFECTS
  None

===========================================================================*/
byte 
vbatt_read( void );
/*~ FUNCTION vbatt_read */


/*===========================================================================

FUNCTION VBATT_READ_MV                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function reads the current battery level.

DEPENDENCIES
  None

RETURN VALUE
  unsigned int representing the battery voltage in milli-volts.

EXAMPLE:
  uint16 batt_voltage = vbatt_read_mv() ;
  printf ( "The battery voltage is %d milli volts" , batt_voltage ) ;
  OP ===> 3560 milli volts

SIDE EFFECTS
  None

===========================================================================*/
uint16 
vbatt_read_mv ( void ) ;
/*~ FUNCTION vbatt_read_mv */


/*===========================================================================

FUNCTION VBATT_CALIBRATE

DESCRIPTION
  This function calibrates the battery level reading returned by
  vbatt_read().  This routine need only be called once.  This calibration
  is to correct for the error in the resisters used in the voltage divider
  at the input to the ADC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Values returned by vbatt_read() may shift drastically from values
  returned in previous calls.

===========================================================================*/
void vbatt_calibrate
(
  byte min,
    /* Value returned by the ADC when the battery is at the minimum level
    ** at which the phone can operate.  Any safety margin added to this
    ** measurement should be done by the calibration, to provide the
    ** correction at the lowest level.
    */
  byte max
    /* Value returned by the ADC when the battery is at the fully charged
    ** level.  Any "show full when it's at max charge" margin added to this
    ** measurement should be done by the calibration, to provide the
    ** correction at the lowest level.
    */
);

/*===========================================================================

FUNCTION vbatt_is_low_batt_alarm_triggered            EXTERNALIZED FUNCTION

DESCRIPTION
    Use this function to find out if the low battery alarm has triggered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean vbatt_is_low_batt_alarm_triggered(void);


/*===========================================================================
FUNCTION   vbatt_get_adc_count_range_cal_info              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to get info on the calibrated ADC counts for the min
   and max valid main battery voltage.

DEPENDENCIES
   None

RETURN VALUE
   boolean
      TRUE  ---> If valid calibration data is available
      FALSE ---> Otherwise

SIDE EFFECTS
   None
===========================================================================*/
boolean vbatt_get_adc_count_range_cal_info(
   uint8  *min_count_ptr,
   uint8  *max_count_ptr);



/*===========================================================================
FUNCTION   vbatt_update_adc_battery_params              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to update various battery parameters.

DEPENDENCIES
   Underlying ADC channel should be operative.

RETURN VALUE
   VOID

SIDE EFFECTS
   NONE
===========================================================================*/
void
vbatt_update_adc_battery_params ( void ) ;
/*~ FUNCTION vbatt_update_adc_battery_params */


/*===========================================================================
FUNCTION   vbatt_query_batt_adc_params              EXTERNAL FUNCTION

DESCRIPTION
   Use this function to get info about various battery parameters.

DEPENDENCIES
   Underlying ADC channel should be operative.

RETURN VALUE
   VOID

SIDE EFFECTS
   NONE
===========================================================================*/
void
vbatt_query_batt_adc_params ( battery_adc_params_type *batt_ptr ) ;
/*~ FUNCTION vbatt_query_batt_adc_params */
/*~ PARAM OUT batt_ptr POINTER */

/**
 * Return the battery temperature.
 * 
 * @return int16
 */
int16 
vbatt_read_temperature ( void ) ;

/**
 * Return the PMIC temperature
 * 
 * @return int32
 */
int32
vbatt_read_pmic_temperature(void);

/**
 * Return Handset supply voltage 
 * (Vdd) determining current 
 * leaving the battery
 * 
 * @return int32
 */
int32 
vbatt_read_ichg_out_mv(void);

/**
 * Return the charger temperature
 * 
 * @return int32
 */
int32 
vbatt_read_chg_temperature(void);

/**
 * Return the USB VBUS voltage
 * 
 * @return int32
 */
int32 
vbatt_read_usb_vbus(void);

/**
 * Return the Voltage across the 
 * charger current sense resistor
 * 
 * @return int32
 */
int32 
vbatt_read_ichg_mv(void);

/**
 * Return the Battery ID voltage
 * 
 * @return int32
 */
int32 
vbatt_read_batt_id(void);

int32 
vbatt_read_vchg_mv(void);


#endif /* VBATT_H */


