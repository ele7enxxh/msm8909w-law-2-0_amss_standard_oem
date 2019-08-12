/*===========================================================================

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R  

DESCRIPTION

  Defines the calibration definitions for qcom accelerometer driver (ADXL)


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl_calib.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------
02/22/11       ky      created, ADXL calibration specific definitions
==========================================================================*/

#ifndef _SNSDACCELADXLCALIB_H
#define _SNSDACCELADXLCALIB_H


/*============================================================================

INCLUDE FILES

============================================================================*/

#include "sns_dd_accel_adxl350_priv.h"

/*=======================================================================

Preprocessor Definitions and Constants

========================================================================*/

// Output change per 1 LSB offset register change in 2g range
#define ADXL350_CAL_OFFSET_PER_LSB_2G        4
#define ADXL350_CAL_OFFSET_TOLERANCE_2G  \
    ( ( ADXL350_CAL_OFFSET_PER_LSB_2G ) / 2 )
#define ADXL350_CAL_REFERENCE_OFFSET_2G      256       // 512 / G_Range

// Some useful values
#define ADXL350_CAL_MAX_UINT_8BIT  0xff // 255 (+-150 mg)

#define ADXL350_CAL_CHK_DATA_TIME_LIMIT_MS           15
#define ADXL350_CAL_NUMBER_OF_READS_FOR_AVG          10  // 0.1 sec * hz, min. 10
#define ADXL350_CAL_MAX_NUM_OF_AVG_TRIES             10
#define ADXL350_CAL_AVG_TOLERANCE                    10
#define ADXL350_CAL_FILTER_CHAIN_FILL_DELAY_USEC     20000


/* orientations */
typedef enum 
{ 
    ACCEL_AXIS_X          = 1,
    ACCEL_AXIS_Y,
    ACCEL_AXIS_Z,
    ACCEL_AXIS_MINUS_X,
    ACCEL_AXIS_MINUS_Y,
    ACCEL_AXIS_MINUS_Z,
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
    int8_t i8X;
    int8_t i8Y;
    int8_t i8Z;
} OffsetDataType;

/* ----------------------------------------------------------------------
**                          STANDARD MACROS
** ---------------------------------------------------------------------- */

//#ifndef CONSTRAIN
//#define  CONSTRAIN( val, min, max ) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))
//#endif

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

sns_ddf_status_e sns_dd_adxl350_calibrate_internal(
    sns_ddf_handle_t,
    sns_ddf_handle_t,
    acc_orientation_e,
    uint32_t,
    OffsetDataType*);

sns_ddf_status_e sns_dd_adxl350_calibrate(
    sns_ddf_handle_t,
    acc_orientation_e,
    uint32_t,
    boolean,
    sns_ddf_memhandler_s*,
    sns_ddf_sensor_data_s**);


#endif /* End include guard  _SNSDACCELADXLCALIB_H */
