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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_calib.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
02/22/11     ky      created, ported from InGeo BREWAPP
==============================================================================*/   

/*============================================================================

                                INCLUDE FILES

============================================================================*/

#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_dd_accel_calib.h"

/*=======================================================================

GLOBAL DATA DEFINITIONS

========================================================================*/

extern sns_ddf_driver_if_s sns_accel_driver_fn_list;

//=============================================================================
/**
@brief enable or disable EEPROM write

@note In order to read offset and gain registers the EEPROM_write bit must
be set.

@param[in]  port_handle     handle used to access the I2C bus
@param[in]  enable          if TRUE enable EEPROM write

@return
SNS_DDF_SUCCESS             success
any other code              failed (see sns_ddf_write_port)

*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_set_eeprom_enable
(
 sns_ddf_handle_t   port_handle,
 boolean            enable
)
{
    uint8_t bosch_write_buffer, out;
    sns_ddf_status_e stat;

    if (enable == TRUE)
    {
        // enable EEPROM write
        bosch_write_buffer = BOSCH_ACCEL_SENSOR_NORMAL_MODE | BOSCH_ACCEL_SENSOR_EE_W;
    }
    else
    {
        // disable EEPROM write
        bosch_write_buffer = BOSCH_ACCEL_SENSOR_NORMAL_MODE;
    }

    stat = sns_ddf_write_port(port_handle,
        BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
        &bosch_write_buffer,
        BOSCH_ACCEL_ONE_BYTE,
        &out);

    return stat;
}

//=============================================================================
/**
@brief Get accelerometer offset and gain values of a specific entity
(either an axis or temperature). Used for calibration.

@note In order to read offset and gain registers the EEPROM_write bit must
be set.

@note There is no option to read registers from EEPROM

@param[in]  port_handle     handle used to access the I2C bus
@param[in]  accel_axis      Axis from which to read data
@param[out] usOffset_ptr    Offset read
@param[out] ucGain_ptr      Gain read

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_get_offset_gain
(
 sns_ddf_handle_t port_handle,
 acc_orientation_e  accel_axis, 
 uint16_t *usOffset_ptr,
 uint8_t  *ucGain_ptr
 )
{
    uint8_t ucOffsetRegAddr;
    uint8_t ucGainRegAddr;
    uint8_t bosch_read_data = 0, out = 0;
    sns_ddf_status_e stat;

    switch ( accel_axis )
    {
    case BOSCH_ACCEL_AXIS_X :
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_X_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_X_REG_ADDR;
        break;

    case BOSCH_ACCEL_AXIS_Y :
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_Y_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_Y_REG_ADDR;
        break;

    case BOSCH_ACCEL_AXIS_Z :
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_Z_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_Z_REG_ADDR;
        break;

    //case BOSCH_ACCEL_TEMP :
    //    ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR;
    //    ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR;
    //    break;

    default:
        return SNS_DDF_EFAIL;
    }

    // Read MSB
    stat = sns_ddf_read_port(port_handle,
        ucOffsetRegAddr, 
        &bosch_read_data,
        BOSCH_ACCEL_ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    *usOffset_ptr = bosch_read_data << BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT;

    // Read LSB/gain
    stat = sns_ddf_read_port(port_handle,
        ucGainRegAddr, 
        &bosch_read_data,
        BOSCH_ACCEL_ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    // B7 and B6 are offset LSB, the rest is gain
    *usOffset_ptr |= ( bosch_read_data & BOSCH_ACCEL_SENSOR_LSB_MASK ) >> 
        BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT;
    *ucGain_ptr = bosch_read_data & BOSCH_ACCEL_SENSOR_GAIN_T_MASK;

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Get accelerometer 3-axis offset and gain values. Used for calibration.

@note In order to read offset and gain registers the EEPROM_write bit must
be set.

@note There is no option to read registers from EEPROM

@param[in]  port_handle     handle used to access the I2C bus
@param[out] rOffset         Offset read
@param[out] rGain           Gain read

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_get_offset_gain_axes
(
 sns_ddf_handle_t port_handle,
 OffsetDataType *pOffset, 
 GainDataType   *pGain
 )
{
    sns_ddf_status_e stat;

    stat = sns_dd_acc_bma150_cal_get_offset_gain( port_handle, BOSCH_ACCEL_AXIS_X, &pOffset->usX, &pGain->ucX );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_bma150_cal_get_offset_gain( port_handle, BOSCH_ACCEL_AXIS_Y, &pOffset->usY, &pGain->ucY );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_bma150_cal_get_offset_gain( port_handle, BOSCH_ACCEL_AXIS_Z, &pOffset->usZ, &pGain->ucZ );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Read 3-axis acceleration data.

@note This function reads both MSB and LSB of acceleration data. In order for
the function to return correct values shadow_dis bit must be set to 0.
[Dev Tip] To convert raw reading to Milli-g units, do the following:
nsAccMilliG = ( nsAccRaw * 1000 * BMA150_RANGE_G ) /
BOSCH_ACCEL_MAX_INT_10BIT;

@attention Verify that accelerometer is in normal mode before calling this
function. If needed, use ConfigMode().

@param[in]  port_handle     handle used to access the I2C bus
@param[out] accData         Measured acceleration.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           fail
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_get_data( sns_ddf_handle_t port_handle, AccDataType *accData )
{
    int16_t bosch_raw_data[3] = {0};
    uint8_t read_bytes = 0 ;
    sns_ddf_status_e stat;

    stat = sns_ddf_read_port(port_handle, 
        BOSCH_ACCEL_SENSOR_X_LSB_REG_ADDR, // 0x02, 
        (void*)bosch_raw_data,
        BOSCH_ACCEL_SIX_BYTES,
        &read_bytes);
    if (SNS_DDF_SUCCESS != stat)
    {
        accData->nsX = 0;
        accData->nsY = 0;
        accData->nsZ = 0;

        return stat;
    }

    accData->nsX = bosch_raw_data[0] >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT;
    accData->nsY = bosch_raw_data[1] >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT;
    accData->nsZ = bosch_raw_data[2] >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT;

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Checks if new acceleration data is available.

@attention Verify that accelerometer is in normal mode before calling this
function. If needed, use ConfigMode().

@param[in]  port_handle     handle used to access the I2C bus
@param[in] usAxis           Axis to query about

@return TRUE  New data is available
@return FALSE No new data available or error occurred
*/
//=============================================================================
static boolean sns_dd_acc_bma150_cal_check_data_availability( sns_ddf_handle_t port_handle, uint16_t usAxis )
{
    uint8_t ucRegAddr;
    uint8_t bosch_read_data[2], out;
    sns_ddf_status_e stat;

    switch ( usAxis )
    {
    case ( BOSCH_ACCEL_AXIS_X ):
        ucRegAddr = BOSCH_ACCEL_SENSOR_X_LSB_REG_ADDR;
        break;

    case ( BOSCH_ACCEL_AXIS_Y ):
        ucRegAddr = BOSCH_ACCEL_SENSOR_Y_LSB_REG_ADDR;
        break;

    case ( BOSCH_ACCEL_AXIS_Z ):
        ucRegAddr = BOSCH_ACCEL_SENSOR_Z_LSB_REG_ADDR;
        break;

    default:
        return FALSE;   // Error
    }

    // Need to read the "old" values to kick off a new update and get fresh data.
    stat = sns_ddf_read_port(port_handle,
        ucRegAddr, 
        bosch_read_data,
        BOSCH_ACCEL_TWO_BYTES,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return FALSE;
    }

    // Check for new data (available from same register as the LSB value)
    stat = sns_ddf_read_port(port_handle,
        ucRegAddr, 
        bosch_read_data,
        BOSCH_ACCEL_ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return FALSE;
    }

    if ( 0 == ( bosch_read_data[ 0 ] & BOSCH_ACCEL_SENSOR_NEW_DATA_MASK ) )
    {
        return FALSE;   // No data available
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
static sns_ddf_status_e sns_dd_acc_bma150_cal_get_avg_data
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
        * Limit wait to BOSCH_ACCEL_SENSOR_CHK_DATA_TIME_LIMIT_MS msec, so we don't 
        * get stuck in an infinite loop if the accelerometer HW is faulty.
        */
        boolean             bNewDataIsAvailable;
        sns_ddf_time_t      uTimeStart = sns_ddf_get_timestamp();
        sns_ddf_time_t      uTimeElapsedMs;

        do
        {
            // If time limit exceeded exit with error.
            uTimeElapsedMs  = (sns_em_convert_dspstick_to_usec((sns_ddf_get_timestamp()-uTimeStart))) / 1000 ;
            if ( BOSCH_ACCEL_SENSOR_CHK_DATA_TIME_LIMIT_MS < uTimeElapsedMs )
            {
                return SNS_DDF_EFAIL;
            }

            bNewDataIsAvailable = sns_dd_acc_bma150_cal_check_data_availability( port_handle, BOSCH_ACCEL_AXIS_X ) &&
                sns_dd_acc_bma150_cal_check_data_availability( port_handle, BOSCH_ACCEL_AXIS_Y ) &&
                sns_dd_acc_bma150_cal_check_data_availability( port_handle, BOSCH_ACCEL_AXIS_Z );
        } while ( FALSE == bNewDataIsAvailable );

        // Get new data
        if ( SNS_DDF_SUCCESS != sns_dd_acc_bma150_cal_get_data( port_handle, &accCurrent ) )
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
        ( ( accMaximum.nsX - accMinimum.nsX ) <= BOSCH_ACCEL_SENSOR_AVG_TOLERANCE ) &&
        ( ( accMaximum.nsY - accMinimum.nsY ) <= BOSCH_ACCEL_SENSOR_AVG_TOLERANCE ) &&
        ( ( accMaximum.nsZ - accMinimum.nsZ ) <= BOSCH_ACCEL_SENSOR_AVG_TOLERANCE );

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
static inline int16_t sns_dd_acc_bma150_cal_divide_and_round
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
@brief Set accelerometer offset and gain values for a specific entity
(either an axis or temperature). Used for calibration.

@note In order to write offset and gain registers (even those that are part
of the image, not only the EEPROM registers) the EEPROM_write bit must
be set.

@param[in] port_handle      handle used to access the I2C bus
@param[in] accel_axis       Axis for which to set data
@param[in] usOffset         Offset to set
@param[in] ucGain           Gain to set

SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_set_offset_gain
(
 sns_ddf_handle_t port_handle,
 acc_orientation_e accel_axis, 
 uint16_t usOffset, 
 uint8_t  ucGain
 )
{
    uint8_t ucOffsetRegAddr;
    uint8_t ucGainRegAddr;
    uint8_t bosch_write_buffer, out;
    boolean  write_to_eeprom_b = FALSE; // Default: write to image, not EEPROM.
    sns_ddf_status_e stat;

    // Check for valid values
    if ( BOSCH_ACCEL_MAX_UINT_10BIT < usOffset ||
        BOSCH_ACCEL_MAX_UINT_6BIT  < ucGain    )
    {
        return SNS_DDF_EFAIL;
    }

    // Set addresses according to entity handled.
    switch ( accel_axis )
    {
    case BOSCH_ACCEL_AXIS_X:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_X_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_X_REG_ADDR;
        break;

    case BOSCH_ACCEL_AXIS_Y:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_Y_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_Y_REG_ADDR;
        break;

    case BOSCH_ACCEL_AXIS_Z:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_Z_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_Z_REG_ADDR;
        break;

    //case BOSCH_ACCEL_TEMP:
    //    ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR;
    //    ucGainRegAddr   = BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR;
    //    break;

    case BOSCH_ACCEL_AXIS_X_EEPROM:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_X_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_EEPROM_GAIN_X_REG_ADDR;
        write_to_eeprom_b  = TRUE;
        break;

    case BOSCH_ACCEL_AXIS_Y_EEPROM:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_Y_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_EEPROM_GAIN_Y_REG_ADDR;
        write_to_eeprom_b  = TRUE;
        break;

    case BOSCH_ACCEL_AXIS_Z_EEPROM:
        ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_Z_REG_ADDR;
        ucGainRegAddr   = BOSCH_ACCEL_SENSOR_EEPROM_GAIN_Z_REG_ADDR;
        write_to_eeprom_b  = TRUE;
        break;

    //case BOSCH_ACCEL_TEMP_EEPROM:
    //    ucOffsetRegAddr = BOSCH_ACCEL_SENSOR_EEPROM_OFFSET_T_REG_ADDR;
    //    ucGainRegAddr   = BOSCH_ACCEL_SENSOR_EEPROM_GAIN_T_REG_ADDR;
    //    write_to_eeprom_b  = TRUE;
    //    break;

    default:
        return SNS_DDF_EFAIL;
    }

    // Write MSB
    bosch_write_buffer = ( uint8_t )( usOffset >> BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT );
    stat = sns_ddf_write_port(port_handle,
        ucOffsetRegAddr,
        &bosch_write_buffer,
        BOSCH_ACCEL_ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    // When writing to EEPROM we need to wait between writes
    if (TRUE == write_to_eeprom_b)
    {
        sns_ddf_delay( BOSCH_ACCEL_SENSOR_WAIT_BETWEEN_EEPROM_WRITES_USEC );
    }

    /**
    * Write LSB/gain
    * B7 and B6 are offset LSB, the rest is gain
    */
    bosch_write_buffer = ( uint8_t )( ( usOffset << BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT ) &
        BOSCH_ACCEL_SENSOR_LSB_MASK );
    bosch_write_buffer |= ucGain & BOSCH_ACCEL_SENSOR_GAIN_T_MASK;
    stat = sns_ddf_write_port(port_handle,
        ucGainRegAddr,
        &bosch_write_buffer,
        BOSCH_ACCEL_ONE_BYTE,
        &out);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    // When writing to EEPROM we need to wait between writes
    if ( TRUE == write_to_eeprom_b )
    {
        sns_ddf_delay( BOSCH_ACCEL_SENSOR_WAIT_BETWEEN_EEPROM_WRITES_USEC );
    }

    return SNS_DDF_SUCCESS;
}

//=============================================================================
/**
@brief Set accelerometer 3-axis offset and gain values. Used for calibration.

@note In order to write offset and gain registers (even those that are part
of the image, not only the EEPROM registers) the EEPROM_write bit must
be set.

@param[in] port_handle      handle used to access the I2C bus
@param[in] pOffset          Offset to set
@param[in] pGain            Gain to set

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
*/
//=============================================================================
static sns_ddf_status_e sns_dd_acc_bma150_cal_set_offset_gain_axes
(
 sns_ddf_handle_t port_handle,
 OffsetDataType *pOffset, 
 GainDataType   *pGain
 )
{
    sns_ddf_status_e stat;

    stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle, BOSCH_ACCEL_AXIS_X, pOffset->usX, pGain->ucX );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle, BOSCH_ACCEL_AXIS_Y, pOffset->usY, pGain->ucY );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }
    stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle, BOSCH_ACCEL_AXIS_Z, pOffset->usZ, pGain->ucZ );
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_acc_bma150_cal_apply_calibrate

===========================================================================*/
/*!
@brief apply calibration data for the BOSCH BMA-150 accelerometer

@detail
    this function gets an offset deviation for all axes, and set it to
    the BMA-150 image memory, and optionally, to the EEPROM memory.

@param[in] dd_handle            Handle to a driver instance.
@param[in]  port_handle         handle used to access the I2C bus
@param[in]  stOffset            Maximal number of iterations of the calibration loop.
@param[in]  write_to_eeprom_b   Indicate whether to write the final offset values 
                                to the EEPROM or not.
@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed

$TODO: 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_bma150_cal_apply_calibrate
(
 sns_ddf_handle_t   dd_handle,
 sns_ddf_handle_t   port_handle,
 OffsetDataType     stOffset,
 boolean            write_to_eeprom_b
)
{
    OffsetDataType stOriginalOffset;
    GainDataType stGain;
    sns_ddf_status_e stat;
    sns_ddf_status_e rc_calib;

    /**
    * setting accelerometer to default operational state, 25hz, 2g range
    * reset calls to sns_dd_acc_bma150_config_default
    */
    if ( (stat = sns_accel_driver_fn_list.reset(dd_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    // In order to read offset values from the image we need to enable EEPROM write.
    stat = sns_dd_acc_bma150_cal_set_eeprom_enable(port_handle, TRUE);
    if ( stat != SNS_DDF_SUCCESS )
    {
        return stat;
    }

    // Get original offset values
    stat = sns_dd_acc_bma150_cal_get_offset_gain_axes( port_handle, &stOriginalOffset, &stGain );
    if ( SNS_DDF_SUCCESS != stat )
    {
        goto apply_calib_bail;
    }

    // Write new offsets to image
    stat = sns_dd_acc_bma150_cal_set_offset_gain_axes( port_handle, &stOffset, &stGain );
    if ( SNS_DDF_SUCCESS != stat )
    {
        goto apply_calib_bail;
    }

    // Wait until filter chain is filled by values with new offset settings
    sns_ddf_delay( BOSCH_ACCEL_SENSOR_FILTER_CHAIN_FILL_DELAY_USEC );


    // If we don't want to actually write to the EEPROM just return.
    if ( FALSE == write_to_eeprom_b )
    {
        stat = SNS_DDF_SUCCESS;
        goto apply_calib_bail;
    }

    // Write new X offset value to EEPROM (only if it changed).
    if ( stOffset.usX != stOriginalOffset.usX )
    {
        stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
            BOSCH_ACCEL_AXIS_X_EEPROM, 
            stOffset.usX, 
            stGain.ucX );
        if (SNS_DDF_SUCCESS != stat)
        {
            goto apply_calib_bail;
        }
    }

    // Write new Y offset value to EEPROM (only if it changed).
    if ( stOffset.usY != stOriginalOffset.usY )
    {
        stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
            BOSCH_ACCEL_AXIS_Y_EEPROM, 
            stOffset.usY, 
            stGain.ucY );
        if (SNS_DDF_SUCCESS != stat)
        {
            goto apply_calib_bail;
        }
    }

    // Write new Z offset value to EEPROM (only if it changed).
    if ( stOffset.usZ != stOriginalOffset.usZ )
    {
        stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
            BOSCH_ACCEL_AXIS_Z_EEPROM, 
            stOffset.usZ, 
            stGain.ucZ );
        if (SNS_DDF_SUCCESS != stat)
        {
            goto apply_calib_bail;
        }
    }

    stat =  SNS_DDF_SUCCESS; // Device is now calibrated

apply_calib_bail:

    rc_calib = stat;

    // Disable EEPROM write.
    stat = sns_dd_acc_bma150_cal_set_eeprom_enable(port_handle, FALSE);
    if ( stat != SNS_DDF_SUCCESS )
    {
        return stat;
    }

    return rc_calib;
}

/*===========================================================================

FUNCTION:   sns_dd_acc_bma150_cal_do_calibrate

===========================================================================*/
/*!
@brief do the actual calibration of the BOSCH BMA-150 accelerometer

@detail
    For further details see 'sns_dd_acc_bma150_calibrate' and Calibration application-note

@param[in]  port_handle         handle used to access the I2C bus
@param[in]  uMaximumTries       Maximal number of iterations of the calibration loop.
@param[in]  accReference_ptr    Ptr for accel data
@param[in]  write_to_eeprom_b   Indicate whether to write the final offset values 
                                to the EEPROM or not.
@param[out] rc_offset_ptr       Sampled sensor data

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed

$TODO: 
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_bma150_cal_do_calibrate
(
 sns_ddf_handle_t   port_handle,
 uint32_t           uMaximumTries,
 const AccDataType* accReference_ptr,
 boolean            write_to_eeprom_b,
 OffsetDataType*    rc_offset_ptr
 )
{
    OffsetDataType stOriginalOffset;
    GainDataType stGain;

    OffsetDataType stOffset;
    uint32_t uTryCount;

    int16_t nsDeltaX;
    int16_t nsDeltaY;
    int16_t nsDeltaZ;

    sns_ddf_status_e stat;

    // Get original offset values
    stat = sns_dd_acc_bma150_cal_get_offset_gain_axes( port_handle, &stOriginalOffset, &stGain );
    if ( SNS_DDF_SUCCESS != stat )
    {
        goto do_calib_bail;
    }

    // Start offset values from original values
    stOffset.usX = stOriginalOffset.usX;
    stOffset.usY = stOriginalOffset.usY;
    stOffset.usZ = stOriginalOffset.usZ;

    /**
    * The following loop does the actual calibration. The device is calibrated
    * if the difference between all axis acceleration data and the reference
    * data is within tolerance.
    * The general outline is:
    * 1. Get average data reading. If reading is not stable try again (for a 
    *    maximum of BOSCH_ACCEL_SENSOR_MAX_NUM_OF_AVG_TRIES)
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
            stat = sns_dd_acc_bma150_cal_get_avg_data( port_handle,
                &bDataIsStable,
                &accAverage,
                BOSCH_ACCEL_SENSOR_NUMBER_OF_READS_FOR_AVG );
            if ( SNS_DDF_SUCCESS != stat )
            {
                goto do_calib_bail;
            }
            ++nGetAvgCounter;
        } while ( FALSE == bDataIsStable && 
            nGetAvgCounter < BOSCH_ACCEL_SENSOR_MAX_NUM_OF_AVG_TRIES );

        // Check if we got stable data
        if ( FALSE == bDataIsStable )
        {
            stat = SNS_DDF_EFAIL;
            goto do_calib_bail;
        }

        // Calculate delta between average value and reference
        nsDeltaX = accAverage.nsX - accReference_ptr->nsX;
        nsDeltaY = accAverage.nsY - accReference_ptr->nsY;
        nsDeltaZ = accAverage.nsZ - accReference_ptr->nsZ;

        // If offset error is outside tolerance recalculate the offset
        if ( ABS( nsDeltaX ) > BOSCH_ACCEL_SENSOR_OFFSET_TOLERANCE_2G || 
            ABS( nsDeltaY ) > BOSCH_ACCEL_SENSOR_OFFSET_TOLERANCE_2G || 
            ABS( nsDeltaZ ) > BOSCH_ACCEL_SENSOR_OFFSET_TOLERANCE_2G )
        {
            /**
            * Calculate new offsets.
            * For 2G range, each LSB in offset changes output value by 
            * BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G so divide by this value.
            * Also make sure values are between 0 and BOSCH_ACCEL_MAX_UINT_10BIT
            */
            int32_t nTemp = stOffset.usX - 
                sns_dd_acc_bma150_cal_divide_and_round( nsDeltaX, BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G );
            stOffset.usX = ( uint16_t )CONSTRAIN( nTemp, 0, BOSCH_ACCEL_MAX_UINT_10BIT );
            nTemp = stOffset.usY - 
                sns_dd_acc_bma150_cal_divide_and_round( nsDeltaY, BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G );
            stOffset.usY = ( uint16_t )CONSTRAIN( nTemp, 0, BOSCH_ACCEL_MAX_UINT_10BIT );
            nTemp = stOffset.usZ -
                sns_dd_acc_bma150_cal_divide_and_round( nsDeltaZ, BOSCH_ACCEL_SENSOR_OFFSET_PER_LSB_2G );
            stOffset.usZ = ( uint16_t )CONSTRAIN( nTemp, 0, BOSCH_ACCEL_MAX_UINT_10BIT );

            // Write new offsets to image
            stat = sns_dd_acc_bma150_cal_set_offset_gain_axes( port_handle, &stOffset, &stGain );
            if ( SNS_DDF_SUCCESS != stat )
            {
                goto do_calib_bail;
            }

            // Wait until filter chain is filled by values with new offset settings
            sns_ddf_delay( BOSCH_ACCEL_SENSOR_FILTER_CHAIN_FILL_DELAY_USEC );
        }
        else
        {
            // Device is calibrated (values are currently only in image).

            // raise sampled sensor data
            rc_offset_ptr->usX = stOffset.usX;
            rc_offset_ptr->usY = stOffset.usY;
            rc_offset_ptr->usZ = stOffset.usZ;

            // If we don't want to actually write to the EEPROM just return.
            if ( FALSE == write_to_eeprom_b )
            {
                stat = SNS_DDF_SUCCESS;
                goto do_calib_bail;
            }
            // Write new X offset value to EEPROM (only if it changed).
            if ( stOffset.usX != stOriginalOffset.usX )
            {
                stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
                    BOSCH_ACCEL_AXIS_X_EEPROM, 
                    stOffset.usX, 
                    stGain.ucX );
                if (SNS_DDF_SUCCESS != stat)
                {
                    goto do_calib_bail;
                }
            }

            // Write new Y offset value to EEPROM (only if it changed).
            if ( stOffset.usY != stOriginalOffset.usY )
            {
                stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
                    BOSCH_ACCEL_AXIS_Y_EEPROM, 
                    stOffset.usY, 
                    stGain.ucY );
                if (SNS_DDF_SUCCESS != stat)
                {
                    goto do_calib_bail;
                }
            }

            // Write new Z offset value to EEPROM (only if it changed).
            if ( stOffset.usZ != stOriginalOffset.usZ )
            {
                stat = sns_dd_acc_bma150_cal_set_offset_gain( port_handle,
                    BOSCH_ACCEL_AXIS_Z_EEPROM, 
                    stOffset.usZ, 
                    stGain.ucZ );
                if (SNS_DDF_SUCCESS != stat)
                {
                    goto do_calib_bail;
                }
            }

            stat =  SNS_DDF_SUCCESS; // Device is now calibrated
            goto do_calib_bail;
        }
    } // !for

do_calib_bail:

    return stat;
}

/*===========================================================================

FUNCTION:   sns_dd_acc_bma150_calibrate_internal

===========================================================================*/
/*!
@brief  This function sets the environment before/after calibration

@detail 
        1.  Set accelerometer to default operational state, 25hz, 2g range
            (Calls to the 'sns_dd_acc_bma150_reset' routine)
        2.  calls to 'sns_dd_acc_bma150_cal_do_calibrate' (actual calibrate function)
        3.  unset accelerometer state
        4.  @see 'sns_dd_acc_bma150_calibrate'

@param[in] dd_handle        Handle to a driver instance.
@param[in] port_handle      handle used to access the I2C bus
@param[in] orientation      The orientation of the device.
                            indicates the axis on which the 1-g is acting
                            see also Table 1 in the Calibration application-note
                            (reference values for all orientations)
                            see acc_orientation_e
@param[in] max_tries        Maximal number of iterations of the calibration loop.
@param[in] write_to_eeprom_b    Indicate whether to write the final offset values 
                                to the EEPROM or not.
@param[out] rc_offset_ptr   Sampled sensor data

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
SNS_DDF_EBUS            from sns_dd_acc_bma150_config_default

$TODO: 
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_acc_bma150_calibrate_internal
(
 sns_ddf_handle_t           dd_handle,
 sns_ddf_handle_t           port_handle,
 acc_orientation_e          orientation,
 uint32_t                   max_tries,
 boolean                    write_to_eeprom_b,
 OffsetDataType*            rc_offset_ptr
 )
{
    sns_ddf_status_e stat;
    sns_ddf_status_e rc_calib;

    AccDataType accReference = { 0 };

    switch( orientation )
    {
    case BOSCH_ACCEL_AXIS_X:
        accReference.nsX = BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    case BOSCH_ACCEL_AXIS_Y:
        accReference.nsY = BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    case BOSCH_ACCEL_AXIS_Z:
        accReference.nsZ = BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    case BOSCH_ACCEL_AXIS_MINUS_X:
        accReference.nsX = -BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    case BOSCH_ACCEL_AXIS_MINUS_Y:
        accReference.nsY = -BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    case BOSCH_ACCEL_AXIS_MINUS_Z:
        accReference.nsZ = -BOSCH_ACCEL_SENSOR_REFERENCE_OFFSET_2G;
        break;

    default:
        return SNS_DDF_EFAIL;
    }

    /**
    * setting accelerometer to default operational state, 25hz, 2g range
    * reset calls to sns_dd_acc_bma150_config_default
    */
    if ( (stat = sns_accel_driver_fn_list.reset(dd_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    // In order to read offset values from the image we need to enable EEPROM write.
    stat = sns_dd_acc_bma150_cal_set_eeprom_enable(port_handle, TRUE);
    if ( stat != SNS_DDF_SUCCESS )
    {
        return stat;
    }

    // Do actual calibration
    rc_calib = sns_dd_acc_bma150_cal_do_calibrate( port_handle,
        max_tries, 
        &accReference, 
        write_to_eeprom_b,
        rc_offset_ptr
    );

    // Disable EEPROM write.
    stat = sns_dd_acc_bma150_cal_set_eeprom_enable(port_handle, FALSE);
    if ( stat != SNS_DDF_SUCCESS )
    {
        return stat;
    }

    return rc_calib;
}
