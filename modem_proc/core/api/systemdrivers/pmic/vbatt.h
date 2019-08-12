#ifndef VBATT_H
#define VBATT_H

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
 * @file vbatt.h BATTERY LEVEL Utilities related declaration.
 *
 * @brief This header file contains all of the definitions necessary for
 *		  other tasks to interface with the battery level utilities.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/vbatt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
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

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/**
 * @brief Min Value of VBAT
 */	
#define VBATT_SCALED_MIN   0   
/**
 * @brief Max Value of VBAT
 */	
#define VBATT_SCALE      255

/**
 * @struct battery_adc_params_type
 * @brief The following information from battery_adc_params_type is not provided
 *		  by the PMIC. It is intended for customers to provide this information
 *		  if they are using an external battery management IC.
 */
typedef struct
{
    uint16 current_batt_adc_voltage ;    /*!< Current ADC read of Battery voltage */	
    uint16 previous_batt_adc_voltage ;    /*!< Previous ADC read of Battery voltage */
    int16  batt_temperature ;    /*!< Battery temperature */
    uint16 batt_id ;    /*!< Battery ID */
    int32  pmic_temperature;    /*!< PMIC temperature */
    int32  ichg_out_mv;    /*!< Battery current charge out */
    int32  ichg_mv;    /*!< Battery current charge */
    int32  vchg_mv;    /*!< Battery charge in millivolts */
    int32  chg_temperature;    /*!< Battery charging temperature */
    int32  usb_vbus;    /*!< Status of USB Vbus */
    uint8  current_batt_adc_raw;    /*!< Raw ADC reading of Battery current */
    uint8  ACLineStatus;    /*!< AC line status	*/
    uint8  BatteryFlag;    /*!< Battery flag */
    uint8  BatteryLifePercent;    /*!< Life percent of Battery */
    int32  BatteryLifeTime;    /*!< Lifetime of Battery */
    int32  BatteryFullLifeTime;    /*!< Full lifetime of Battery */
    uint8  BackupBatteryFlag;    /*!< Battery backup flag */
    uint8  BackupBatteryLifePercent;    /*!< Life percent of Battery backup  */
    int32  BackupBatteryLifeTime;    /*!< Lifetime of Battery backup  */
    int32  BackupBatteryFullLifeTime;    /*!< Full lifetime of Battery backup */
    int32  BatteryCurrent;    /*!< Current drawn frm Battery */
    int32  BatteryAverageCurrent;    /*!< Battery's average current */
    int32  BatteryAverageInterval;    /*!< Average of Battery's Internal Voltage */
    int32  BatterymAHourConsumed;    /*!< mAH consumed from Battery */
    int32  BackupBatteryVoltage;    /*!< Battery's backup voltage */
    uint8  BatteryChemistry;    /*!< Battery Chemistry */
} battery_adc_params_type ;


/*===========================================================================

                    VBAT DRIVER FUNCTION DECLARATIONS

===========================================================================*/
/**
 * @name vbatt_read
 *
 * @brief This function reads the current battery level.
 *
 * @param void
 *
 * @return byte A scaled version of the batter level where empty is
 *				VBATT_SCALED_MIN, and full is VBATT_SCALE.
 */
byte vbatt_read( void );

/**
 * @name vbatt_read_mv
 *
 * @brief This function reads the current battery level.
 *
 * @param void
 *
 * @return uint16 Representing the battery voltage in milli-volts.
 */
uint16 vbatt_read_mv ( void ) ;

/**
 * @name vbatt_calibrate
 *
 * @brief This function calibrates the battery level reading returned by
 *		  vbatt_read().  This routine need only be called once.  This
 *		  calibration is to correct for the error in the resisters used in
 *		  the voltage divider at the input to the ADC.
 *
 * @param [in] min Value returned by the ADC when the battery is at the minimum
 *					level at which the phone can operate.  Any safety margin
 *					added to this measurement should be done by the calibration,
					to provide the correction at the lowest level.
 * @param [in] max Value returned by the ADC when the battery is at the fully
 *					charged level.  Any "show full when it's at max charge"
 *					margin added to this measurement should be done by the
 *					calibration, to provide the correction at the lowest level.
 *
 * @return  void
 */
void vbatt_calibrate(byte min, byte max);

/**
 * @name vbatt_is_low_batt_alarm_triggered
 *
 * @brief Use this function to find out if the low battery alarm has triggered.
 *
 * @param [in] void
 *
 * @return  boolean TRUE: If triggered
 *					FALSE: Otherwise
 */
boolean vbatt_is_low_batt_alarm_triggered(void);

/**
 * @name vbatt_get_adc_count_range_cal_info
 *
 * @brief Use this function to get info on the calibrated ADC counts for
 *		  the min and max valid main battery voltage.
 *
 * @param [out] min_count_ptr Min ADC count valid main battery voltage.
 * @param [out] max_count_ptr Max ADC count valid main battery voltage.
 *
 * @return  boolean TRUE: If valid calibration data is available
 *					FALSE: Otherwise
 */
boolean vbatt_get_adc_count_range_cal_info(uint8  *min_count_ptr,
											uint8  *max_count_ptr);
   
/**
 * @name vbatt_update_adc_battery_params
 *
 * @brief Use this function to update various battery parameters.
 *
 * @param [in] void
 *
 * @return  void
 *
 * @remarks Underlying ADC channel should be operative.
 */
void vbatt_update_adc_battery_params ( void ) ;

/**
 * @name vbatt_query_batt_adc_params
 *
 * @brief Use this function to get info about various battery parameters.
 *
 * @param [out] batt_ptr Get the ADC param of Battery.
 *
 * @return  void
 *
 * @remarks Underlying ADC channel should be operative.
 */
void vbatt_query_batt_adc_params ( battery_adc_params_type *batt_ptr ) ;

/**
 * @name vbatt_read_temperature
 *
 * @brief Return the battery temperature.
 *
 * @param void
 *
 * @return  int32
 */
int16 vbatt_read_temperature ( void ) ;

/**
 * @name vbatt_read_pmic_temperature
 *
 * @brief Return the PMIC temperature.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_pmic_temperature(void);

/**
 * @name vbatt_read_ichg_out_mv
 *
 * @brief Return Handset supply voltage (Vdd) determining current leaving the battery.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_ichg_out_mv(void);

/**
 * @name vbatt_read_chg_temperature
 *
 * @brief Return the charger temperature.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_chg_temperature(void);

/**
 * @name vbatt_read_usb_vbus
 *
 * @brief Return the USB VBUS voltage.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_usb_vbus(void);

/**
 * @name vbatt_read_ichg_mv
 *
 * @brief Return the Voltage across the charger current sense resistor.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_ichg_mv(void);

/**
 * @name vbatt_read_batt_id
 *
 * @brief Return the Battery ID voltage.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_batt_id(void);

/**
 * @name vbatt_read_vchg_mv
 *
 * @brief Read Battery voltage in milli volts.
 *
 * @param void
 *
 * @return  int32
 */
int32 vbatt_read_vchg_mv(void);


#endif    /* VBATT_H */


