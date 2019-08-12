/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    C A L I B R A T I O N

DESCRIPTION

   Implements the calibration for the accelerometer driver 

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl_calib.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
02/22/11     ky      Calibration procedure for ADXL, ported from BMA-150
==============================================================================*/   

/*============================================================================

                                INCLUDE FILES

============================================================================*/

#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_dd_accel_adxl_calib.h"

/*=======================================================================

Preprocessor Definitions and Constants

========================================================================*/

#define ONE_BYTE    1

/*=======================================================================

GLOBAL DATA DEFINITIONS

========================================================================*/

extern sns_ddf_driver_if_s sns_accel_adxl350_driver_fn_list;


//=============================================================================
/**
@brief Set calibration environment

@param[in]  dd_handle       Handle to a driver instance.
@param[in]  port_handle     handle used to access the I2C bus

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           error
*/
//=============================================================================
static sns_ddf_status_e sns_dd_adxl_set_calib_env
(
 sns_ddf_handle_t           dd_handle,
 sns_ddf_handle_t           port_handle
)
{
    uint8_t buf = 0;
    uint8_t byte_count = 0;
    uint8_t mask = ADXL350_RATE_MASK;
    uint8_t adxl_rate_100_hz = 0x0A; /* ADXL350_DATA_RATE_100_HZ  */
    sns_ddf_status_e stat;

    /**
    * setting accelerometer to default operational state: 16g range, full res, 3200hz
    */
    if ( (stat = sns_accel_adxl350_driver_fn_list.reset(dd_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    /**
    * we perform calibration in data rate of 100hz
    */
    stat = sns_ddf_read_port(port_handle,
        ADXL350_REG_RATE_AND_POWER_MODE,
        &buf,
        ONE_BYTE,
        &byte_count);
    if (stat != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    buf &= ~mask;
    buf |= (adxl_rate_100_hz & mask);

    stat = sns_ddf_write_port(port_handle,
        ADXL350_REG_RATE_AND_POWER_MODE,
        &buf,
        ONE_BYTE,
        &byte_count);
    if (stat != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    // ODR settling at 100hz
    sns_ddf_delay(11100); // 11.1 ms

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Get accelerometer offset value of a specific axis

@param[in]  port_handle     handle used to access the I2C bus
@param[in]  accel_axis      Axis from which to read data
@param[out] usOffset_ptr    Offset read

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_adxl_get_offset
(
 sns_ddf_handle_t port_handle,
 acc_orientation_e  accel_axis, 
 int8_t *usOffset_ptr
 )
{
    uint8_t ucOffsetRegAddr;
    uint8_t read_count = 0, out = 0;
    sns_ddf_status_e stat;

    switch ( accel_axis )
    {
    case ACCEL_AXIS_X :
        ucOffsetRegAddr = ADXL350_REG_OFFSET_X;
        break;

    case ACCEL_AXIS_Y :
        ucOffsetRegAddr = ADXL350_REG_OFFSET_Y;
        break;

    case ACCEL_AXIS_Z :
        ucOffsetRegAddr = ADXL350_REG_OFFSET_Z;
        break;

    default:
        return SNS_DDF_EFAIL;
    }

    stat = sns_ddf_read_port(port_handle,
        ucOffsetRegAddr, 
        &read_count,
        ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    *usOffset_ptr = (int8_t)(read_count);

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Get accelerometer 3-axis offset values.

@param[in]  port_handle     handle used to access the I2C bus
@param[out] pOffset         Offset read

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_adxl_get_offset_axes
(
 sns_ddf_handle_t port_handle,
 OffsetDataType *pOffset
 )
{
    sns_ddf_status_e stat;

    stat = sns_dd_adxl_get_offset( port_handle, ACCEL_AXIS_X, &pOffset->i8X );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_adxl_get_offset( port_handle, ACCEL_AXIS_Y, &pOffset->i8Y );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_adxl_get_offset( port_handle, ACCEL_AXIS_Z, &pOffset->i8Z );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Read 3-axis acceleration data.
@detail see also 'sns_dd_adxl350_get_data'

@attention Verify that accelerometer is in normal mode before calling this
function. If needed, use ConfigMode().

@param[in]  port_handle     handle used to access the I2C bus
@param[out] accData         Measured acceleration.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           fail
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_adxl_cal_get_data( sns_ddf_handle_t port_handle, AccDataType *accData )
{
    int16_t raw_data[3] = {0};
    uint8_t read_count = 0 ;
    sns_ddf_status_e stat;

    stat = sns_ddf_read_port(port_handle, 
        ADXL350_REG_DATA_X0, // 0x32 
        (uint8_t*)raw_data,
        sizeof(raw_data),
        &read_count);
    if (SNS_DDF_SUCCESS != stat)
    {
        accData->nsX = 0;
        accData->nsY = 0;
        accData->nsZ = 0;

        return stat;
    }

    accData->nsX = raw_data[0];
    accData->nsY = raw_data[1];
    accData->nsZ = raw_data[2];

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief  Checks if new acceleration data is available.
@detail see also 'sns_dd_adxl350_get_data'

@attention Verify that accelerometer is in normal mode before calling this
function.

@param[in]  port_handle     handle used to access the I2C bus

@return TRUE  New data is available
@return FALSE No new data available or error occurred
*/
//=============================================================================
static boolean sns_dd_acc_adxl_cal_check_data_availability( sns_ddf_handle_t port_handle )
{
    uint8_t intr_status = 0; /* updated regardless if interrupts are enabled */
    uint8_t read_count = 0;
    sns_ddf_status_e stat;

    /* Check if data ready */
    stat = sns_ddf_read_port(port_handle,
        ADXL350_REG_INTERRUPT_SOURCE,
        &intr_status,
        ONE_BYTE,
        &read_count);

    if (SNS_DDF_SUCCESS != stat)
    {
        return FALSE;
    }

    if (!(intr_status & ADXL350_INT_DATA_READY))
    {
        return FALSE;
    }

    return TRUE;
}

//=============================================================================
/**
@brief Reads acceleration data several times calculates averages and checks
that the difference between minimum and maximum readings is within
allowed tolerance (so it is "stable"). Used for calibration.

@param[in]  port_handle     handle used to access the I2C bus
@param[out] bDataIsStable   Indicates whether the data is stable or not.
@param[out] accAverage      Average acceleration data.
@param[in]  usReadCount     Number of times to read data for averaging.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           fail
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_adxl_cal_get_avg_data
(
 sns_ddf_handle_t port_handle,
 boolean        *bDataIsStable,
 AccDataType    *accAverage, 
 uint16_t       usReadCount
 )
{
    // Prepare min/max variables.
    AccDataType accMinimum = { MAX_INT16, MAX_INT16, MAX_INT16 };
    AccDataType accMaximum = { MIN_INT16, MIN_INT16, MIN_INT16 };  

    // Prepare sum variables (acceleration data is 10 bits, so int32_t can be used)
    int32_t nSumX = 0, nSumY = 0, nSumZ = 0;

    AccDataType accCurrent;
    uint32_t uCnt;

    for ( uCnt = 0; uCnt < usReadCount; ++uCnt )
    {
        /**
        * Wait for new data to be available
        * Limit wait to ADXL350_CAL_CHK_DATA_TIME_LIMIT_MS msec, so we don't 
        * get stuck in an infinite loop if the accelerometer HW is faulty.
        */
        boolean             bNewDataIsAvailable;
        sns_ddf_time_t      uTimeStart = sns_ddf_get_timestamp();
        sns_ddf_time_t      uTimeElapsedMs;

        do
        {
            // If time limit exceeded exit with error.
            uTimeElapsedMs  = (sns_em_convert_dspstick_to_usec((sns_ddf_get_timestamp()-uTimeStart))) / 1000 ;
            if ( ADXL350_CAL_CHK_DATA_TIME_LIMIT_MS < uTimeElapsedMs )
            {
                return SNS_DDF_EFAIL;
            }

            bNewDataIsAvailable = sns_dd_acc_adxl_cal_check_data_availability( port_handle );
        } while ( FALSE == bNewDataIsAvailable );

        // Get new data
        if ( SNS_DDF_SUCCESS != sns_dd_acc_adxl_cal_get_data( port_handle, &accCurrent ) )
        {
            return SNS_DDF_EFAIL;
        }

        // Find Min/Max values and add to sum
        accMinimum.nsX = MIN( accCurrent.nsX, accMinimum.nsX );
        accMinimum.nsY = MIN( accCurrent.nsY, accMinimum.nsY );
        accMinimum.nsZ = MIN( accCurrent.nsZ, accMinimum.nsZ );
        accMaximum.nsX = MAX( accCurrent.nsX, accMaximum.nsX );
        accMaximum.nsY = MAX( accCurrent.nsY, accMaximum.nsY );
        accMaximum.nsZ = MAX( accCurrent.nsZ, accMaximum.nsZ );
        nSumX += accCurrent.nsX;
        nSumY += accCurrent.nsY;
        nSumZ += accCurrent.nsZ;
    }

    // Calculate averages
    accAverage->nsX = ( int16_t )( nSumX / ( int32_t )usReadCount );
    accAverage->nsY = ( int16_t )( nSumY / ( int32_t )usReadCount );
    accAverage->nsZ = ( int16_t )( nSumZ / ( int32_t )usReadCount );

    // Check for stable data: data variance should be within specified limits
    *bDataIsStable = 
        ( ( accMaximum.nsX - accMinimum.nsX ) <= ADXL350_CAL_AVG_TOLERANCE ) &&
        ( ( accMaximum.nsY - accMinimum.nsY ) <= ADXL350_CAL_AVG_TOLERANCE ) &&
        ( ( accMaximum.nsZ - accMinimum.nsZ ) <= ADXL350_CAL_AVG_TOLERANCE );

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Return round(nsDividend/nsDivisor) using integer arithmetic

@param[in] nsDividend Dividend
@param[in] nsDivisor  Divisor

@return Rounded result of dividend divided by divisor.
*/
//=============================================================================
static inline int16_t sns_dd_acc_adxl_cal_divide_and_round
( 
 int16_t nsDividend, 
 int16_t nsDivisor
 )
{
    if ( nsDivisor == 0 )
    {
        return 0;
    }

    /* nsDivisor is constant */
    //if ( nsDivisor < 0 )
    //{
    //    nsDividend = -nsDividend; 
    //    nsDivisor = -nsDivisor;
    //}

    if ( nsDividend > 0 )
    {
        return ( nsDividend + ( nsDivisor / 2 ) ) / nsDivisor;
    }

    return ( nsDividend - ( nsDivisor / 2 ) ) / nsDivisor;
}

//=============================================================================
/**
@brief Set accelerometer offset value for a specific axis

@param[in] port_handle      handle used to access the I2C bus
@param[in] accel_axis       Axis for which to set data
@param[in] usOffset         Offset to set

SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_adxl_cal_set_offset
(
 sns_ddf_handle_t port_handle,
 acc_orientation_e accel_axis, 
 int8_t usOffset
 )
{
    uint8_t ucOffsetRegAddr;
    uint8_t write_count, out;
    sns_ddf_status_e stat;

    // Check for valid values
    if ( ADXL350_CAL_MAX_UINT_8BIT < ((uint8_t)usOffset) )
    {
        return SNS_DDF_EFAIL;
    }

    // Set addresses according to entity handled.
    switch ( accel_axis )
    {
    case ACCEL_AXIS_X:
        ucOffsetRegAddr = ADXL350_REG_OFFSET_X;
        break;

    case ACCEL_AXIS_Y:
        ucOffsetRegAddr = ADXL350_REG_OFFSET_Y;
        break;

    case ACCEL_AXIS_Z:
        ucOffsetRegAddr = ADXL350_REG_OFFSET_Z;
        break;

    default:
        return SNS_DDF_EFAIL;
    }

    write_count = ( uint8_t )( usOffset );
    stat = sns_ddf_write_port(port_handle,
        ucOffsetRegAddr,
        &write_count,
        ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Set accelerometer 3-axis offset values. Used for calibration.

@param[in] port_handle      handle used to access the I2C bus
@param[in] pOffset          Offset to set

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_adxl_cal_set_offset_axes
(
 sns_ddf_handle_t port_handle,
 OffsetDataType *pOffset
 )
{
    sns_ddf_status_e stat;

    stat = sns_dd_acc_adxl_cal_set_offset( port_handle, ACCEL_AXIS_X, pOffset->i8X );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_adxl_cal_set_offset( port_handle, ACCEL_AXIS_Y, pOffset->i8Y );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_adxl_cal_set_offset( port_handle, ACCEL_AXIS_Z, pOffset->i8Z );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_acc_adxl_cal_apply_calibrate

===========================================================================*/
/*!
@brief apply calibration data for the AD ADXL accelerometer

@detail
    this function gets an offset deviation for all axes, and set it to
    the ADXL image memory

@param[in] dd_handle            Handle to a driver instance.
@param[in]  port_handle         handle used to access the I2C bus
@param[in]  stOffset            Maximal number of iterations of the calibration loop.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed

$TODO: 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_adxl_cal_apply_calibrate
(
 sns_ddf_handle_t   dd_handle,
 sns_ddf_handle_t   port_handle,
 OffsetDataType     stOffset
)
{
    OffsetDataType stOriginalOffset;
    sns_ddf_status_e stat;

    /**
    * setting accelerometer to default operational state: 16g range, full res, 3200hz
    */
    if ( (stat = sns_accel_adxl350_driver_fn_list.reset(dd_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    // Get original offset values
    stat = sns_dd_adxl_get_offset_axes( port_handle, &stOriginalOffset );
    if ( SNS_DDF_SUCCESS != stat )
    {
        return stat;
    }

    // Write new offsets to image
    stat = sns_dd_acc_adxl_cal_set_offset_axes( port_handle, &stOffset );
    if ( SNS_DDF_SUCCESS != stat )
    {
        return stat;
    }

    // Wait until filter chain is filled by values with new offset settings
    sns_ddf_delay( ADXL350_CAL_FILTER_CHAIN_FILL_DELAY_USEC );

    return SNS_DDF_SUCCESS; // Device is now calibrated
}

/*===========================================================================

FUNCTION:   sns_dd_acc_adxl_cal_do_calibrate

===========================================================================*/
/*!
@brief do the actual calibration of the AD ADXL accelerometer

@detail
    For further details see 'sns_dd_adxl350_calibrate' and Calibration application-note

@param[in]  port_handle         handle used to access the I2C bus
@param[in]  uMaximumTries       Maximal number of iterations of the calibration loop.
@param[in]  accReference_ptr    Ptr for accel data
@param[out] rc_offset_ptr       Sampled sensor data

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed

$TODO: 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_adxl_cal_do_calibrate
(
 sns_ddf_handle_t   port_handle,
 uint32_t           uMaximumTries,
 const AccDataType* accReference_ptr,
 OffsetDataType*    rc_offset_ptr
 )
{
    OffsetDataType stOriginalOffset;
    OffsetDataType stOffset;
    uint32_t uTryCount;

    int16_t nsDeltaX;
    int16_t nsDeltaY;
    int16_t nsDeltaZ;

    sns_ddf_status_e stat;

    // Get original offset values
    stat = sns_dd_adxl_get_offset_axes( port_handle, &stOriginalOffset );
    if ( SNS_DDF_SUCCESS != stat )
    {
        return stat;
    }

    // Start offset values from original values
    stOffset.i8X = stOriginalOffset.i8X;
    stOffset.i8Y = stOriginalOffset.i8Y;
    stOffset.i8Z = stOriginalOffset.i8Z;

    /**
    * The following loop does the actual calibration. The device is calibrated
    * if the difference between all axis acceleration data and the reference
    * data is within tolerance.
    * The general outline is:
    * 1. Get average data reading. If reading is not stable try again (for a 
    *    maximum of ADXL350_CAL_MAX_NUM_OF_AVG_TRIES)
    * 2. Check if the device is calibrated, if so end.
    * 3. Calculate new offset values, write them and start over.
    *
    * This is done for a maximum of uMaximumTries times.
    */
    for ( uTryCount = 0 ; uTryCount < uMaximumTries ; ++uTryCount )
    {
        int32_t nGetAvgCounter = 0;
        boolean bDataIsStable;
        AccDataType accAverage;
        // Try to get stable average data reading
        do
        {
            stat = sns_dd_acc_adxl_cal_get_avg_data( port_handle,
                &bDataIsStable,
                &accAverage,
                ADXL350_CAL_NUMBER_OF_READS_FOR_AVG );
            if ( SNS_DDF_SUCCESS != stat )
            {
                return stat;
            }
            ++nGetAvgCounter;
        } while ( FALSE == bDataIsStable && 
            nGetAvgCounter < ADXL350_CAL_MAX_NUM_OF_AVG_TRIES );

        // Check if we got stable data
        if ( FALSE == bDataIsStable )
        {
            return SNS_DDF_EFAIL;
        }

        // Calculate delta between average value and reference
        nsDeltaX = accAverage.nsX - accReference_ptr->nsX;
        nsDeltaY = accAverage.nsY - accReference_ptr->nsY;
        nsDeltaZ = accAverage.nsZ - accReference_ptr->nsZ;

        // If offset error is outside tolerance recalculate the offset
        if ( ABS( nsDeltaX ) > ADXL350_CAL_OFFSET_TOLERANCE_2G || 
            ABS( nsDeltaY ) > ADXL350_CAL_OFFSET_TOLERANCE_2G || 
            ABS( nsDeltaZ ) > ADXL350_CAL_OFFSET_TOLERANCE_2G )
        {
            /**
            * Calculate new offsets.
            * For 2G range, each LSB in offset changes output value by 
            * ADXL350_CAL_OFFSET_PER_LSB_2G so divide by this value.
            * Also make sure values are between 0 and ADXL350_CAL_MAX_UINT_8BIT
            */
            int32_t nTemp = stOffset.i8X - 
                sns_dd_acc_adxl_cal_divide_and_round( nsDeltaX, ADXL350_CAL_OFFSET_PER_LSB_2G );
            stOffset.i8X = ( int8_t )nTemp;
            nTemp = stOffset.i8Y - 
                sns_dd_acc_adxl_cal_divide_and_round( nsDeltaY, ADXL350_CAL_OFFSET_PER_LSB_2G );
            stOffset.i8Y = ( int8_t )nTemp;
            nTemp = stOffset.i8Z -
                sns_dd_acc_adxl_cal_divide_and_round( nsDeltaZ, ADXL350_CAL_OFFSET_PER_LSB_2G );
            stOffset.i8Z = ( int8_t )nTemp;

            // Write new offsets to image
            stat = sns_dd_acc_adxl_cal_set_offset_axes( port_handle, &stOffset );
            if ( SNS_DDF_SUCCESS != stat )
            {
                return stat;
            }

            // Wait until filter chain is filled by values with new offset settings
            sns_ddf_delay( ADXL350_CAL_FILTER_CHAIN_FILL_DELAY_USEC );
        }
        else
        {
            // Device is calibrated (values are currently only in image).

            // raise sampled sensor data
            rc_offset_ptr->i8X = stOffset.i8X;
            rc_offset_ptr->i8Y = stOffset.i8Y;
            rc_offset_ptr->i8Z = stOffset.i8Z;

            return SNS_DDF_SUCCESS; // Device is now calibrated
        }
    } // !for

    return SNS_DDF_EFAIL;
}

/*===========================================================================

FUNCTION:   sns_dd_adxl350_calibrate_internal

===========================================================================*/
/*!
@brief  This function sets the environment before/after calibration

@detail 
        1.  Set accelerometer to default operational state, 25hz, 2g range
            (Calls to the 'sns_dd_adxl350_reset' routine)
        2.  calls to 'sns_dd_acc_adxl_cal_do_calibrate' (actual calibrate function)
        3.  unset accelerometer state
        4.  @see 'sns_dd_adxl350_calibrate'

@param[in] dd_handle        Handle to a driver instance.
@param[in] port_handle      handle used to access the I2C bus
@param[in] orientation      The orientation of the device.
                            indicates the axis on which the 1-g is acting
                            see also Table 1 in the Calibration application-note
                            (reference values for all orientations)
                            see acc_orientation_e
@param[in] max_tries        Maximal number of iterations of the calibration loop.
@param[out] rc_offset_ptr   Sampled sensor data

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed

$TODO: 
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_adxl350_calibrate_internal
(
 sns_ddf_handle_t           dd_handle,
 sns_ddf_handle_t           port_handle,
 acc_orientation_e          orientation,
 uint32_t                   max_tries,
 OffsetDataType*            rc_offset_ptr
 )
{
    sns_ddf_status_e stat;
    sns_ddf_status_e rc_calib;

    AccDataType accReference = { 0 };

    switch( orientation )
    {
    case ACCEL_AXIS_X:
        accReference.nsX = ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    case ACCEL_AXIS_Y:
        accReference.nsY = ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    case ACCEL_AXIS_Z:
        accReference.nsZ = ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    case ACCEL_AXIS_MINUS_X:
        accReference.nsX = -ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    case ACCEL_AXIS_MINUS_Y:
        accReference.nsY = -ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    case ACCEL_AXIS_MINUS_Z:
        accReference.nsZ = -ADXL350_CAL_REFERENCE_OFFSET_2G;
        break;

    default:
        return SNS_DDF_EFAIL;
    }

    /**
    * setting accelerometer to default operational state
    */
    if ( (stat = sns_dd_adxl_set_calib_env(dd_handle, port_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    // Do actual calibration
    rc_calib = sns_dd_acc_adxl_cal_do_calibrate( port_handle,
        max_tries, 
        &accReference, 
        rc_offset_ptr
    );

    return rc_calib;
}
