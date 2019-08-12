/*===========================================================================

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R  

DESCRIPTION

  Defines the calibration definitions for qcom accelerometer driver


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_calib.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------
02/22/11       ky      created, calibration specific definitions
==========================================================================*/

#ifndef _SNSDACCELCALIB_H
#define _SNSDACCELCALIB_H


/*============================================================================

INCLUDE FILES

============================================================================*/

#include "sns_dd_accel_priv.h"

/*=======================================================================

Preprocessor Definitions and Constants

========================================================================*/

#define BOSCH_ACCEL_SENSOR_GAIN_X_REG_ADDR        0x16
#define BOSCH_ACCEL_SENSOR_GAIN_Y_REG_ADDR        0x17
#define BOSCH_ACCEL_SENSOR_GAIN_Z_REG_ADDR        0x18
//#define BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR        0x19 (sns_dd_accel_priv.h)
#define BOSCH_ACCEL_SENSOR_OFFSET_X_REG_ADDR      0x1a
#define BOSCH_ACCEL_SENSOR_OFFSET_Y_REG_ADDR      0x1b
#define BOSCH_ACCEL_SENSOR_OFFSET_Z_REG_ADDR      0x1c
//#define BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR      0x1d (sns_dd_accel_priv.h)

#define BOSCH_ACCEL_SENSOR_EEPROM_GAIN_X_REG_ADDR   0x36
#define BOSCH_ACCEL_SENSOR_EEPROM_GAIN_Y_REG_ADDR   0x37
#define BOSCH_ACCEL_SENSOR_EEPROM_GAIN_Z_REG_ADDR   0x38
//#define BOSCH_ACCEL_SENSOR_EEPROM_GAIN_T_REG_ADDR   0x39
#define BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_X_REG_ADDR 0x3a
#define BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_Y_REG_ADDR 0x3b
#define BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_Z_REG_ADDR 0x3c
//#define BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_T_REG_ADDR 0x3d

// Output change per 1 LSB offset register change in 2g range
#define BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G        14
#define BOSCH_ACCEL_SENSOR_OFFSET_TOLERANCE_2G  \
    ( ( BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G ) / 2 )
#define BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G      256       // 512 / G_Range

// Some useful values
#define BOSCH_ACCEL_MAX_UINT_10BIT  0x3ff                             // 1023
#define BOSCH_ACCEL_MAX_INT_10BIT   ( BOSCH_ACCEL_MAX_UINT_10BIT / 2 )  // 511
#define BOSCH_ACCEL_MAX_UINT_3BIT   0x07
#define BOSCH_ACCEL_MAX_UINT_6BIT   0x3f                              // 63

#define BOSCH_ACCEL_SENSOR_CHK_DATA_TIME_LIMIT_MS           15
#define BOSCH_ACCEL_SENSOR_NUMBER_OF_READS_FOR_AVG          10
#define BOSCH_ACCEL_SENSOR_MAX_NUM_OF_AVG_TRIES             10
#define BOSCH_ACCEL_SENSOR_AVG_TOLERANCE                    10
#define BOSCH_ACCEL_SENSOR_WAIT_BETWEEN_EEPROM_WRITES_USEC  20000  // Minimum is 14msec
#define BOSCH_ACCEL_SENSOR_FILTER_CHAIN_FILL_DELAY_USEC     20000

#define BOSCH_ACCEL_ONE_BYTE    1
#define BOSCH_ACCEL_TWO_BYTES   2
#define BOSCH_ACCEL_SIX_BYTES   6



/* orientations */
typedef enum 
{ 
    BOSCH_ACCEL_AXIS_X          = 1,
    BOSCH_ACCEL_AXIS_Y,
    BOSCH_ACCEL_AXIS_Z,
    BOSCH_ACCEL_AXIS_MINUS_X,
    BOSCH_ACCEL_AXIS_MINUS_Y,
    BOSCH_ACCEL_AXIS_MINUS_Z,
    BOSCH_ACCEL_TEMP,
    BOSCH_ACCEL_AXIS_X_EEPROM,
    BOSCH_ACCEL_AXIS_Y_EEPROM,
    BOSCH_ACCEL_AXIS_Z_EEPROM,
    BOSCH_ACCEL_TEMP_EEPROM
} acc_orientation_e;

// Struct for 3-axis acceleration data
typedef struct AccDataStruct
{
    int16_t nsX;
    int16_t nsY;
    int16_t nsZ;
} AccDataType;

// Struct for 3-axis offset data
typedef struct OffsetDataStruct
{
    uint16_t usX;
    uint16_t usY;
    uint16_t usZ;
} OffsetDataType;

// Struct for 3-axis gain data
typedef struct GainDataStruct
{
    uint8_t ucX;
    uint8_t ucY;
    uint8_t ucZ;
} GainDataType;

/* ----------------------------------------------------------------------
**                          STANDARD MACROS
** ---------------------------------------------------------------------- */

#ifndef CONSTRAIN
#define  CONSTRAIN( val, min, max ) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))
#endif

#ifndef MIN_INT16
#define MIN_INT16 -32768
#endif

#ifndef MAX_INT16
#define MAX_INT16 32767
#endif

#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

/*=======================================================================

FUNCTION PROTOTYPES

========================================================================*/

sns_ddf_status_e sns_dd_acc_bma150_calibrate_internal(
    sns_ddf_handle_t,
    sns_ddf_handle_t,
    acc_orientation_e,
    uint32_t,
    boolean,
    OffsetDataType*);

sns_ddf_status_e sns_dd_acc_bma150_calibrate(
    sns_ddf_handle_t,
    acc_orientation_e,
    uint32_t,
    boolean,
    sns_ddf_memhandler_s*,
    sns_ddf_sensor_data_s**);

void bma150_err_msg(char *msg);


#endif /* End include guard  _SNSDACCELCALIB_H */
