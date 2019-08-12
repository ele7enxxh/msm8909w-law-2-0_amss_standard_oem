/*==============================================================================

 A L T I M E T E R    &    T E M P E R A T U R E    S E N S O R    D R I V E R

DESCRIPTION

   Impelements the driver for the BMP085 driver
   Code is based on BST-BMP085-DS000-05.pdf.

Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alt_bmp085.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
05/28/14     MW      Fixed KW issue
05/21/13     pn      Added data logging
05/09/13     pn      Properly handles one data type request while measuring 
                     the other data type
04/18/13     pn      Fixed problem handling multiple sensor types in get_data()
03/12/13     jtl     add specific initializers to driver fn ptr table
02/19/13     jtl     Added probe function
01/15/13     sd      updated driver name and model name to BMP085/BMP180
11/27/12     vh      Reset the data requested attribute state for SNS_DDF_ATTRIB_POWER_STATE
09/13/12     vh      Eliminated compiler warnings
07/07/12     sd      Fixed get temperature attribute doesn't work.
07/07/12     sd      support concurrent requests of pressure and temperature, but only 
                     one request each at a time. 
07/02/12     sd      fixed clocwork error/warnings 
05/22/12     pn      sensor type in error report must be the requested sensor type
01/16/12     br      deleted ASSERT in sns_dd_alt_bmp085_handle_timer() 
11/14/11     br      change the max freq from 10 to 30
26/09/11	 albert  added selftest
08/04/11     sd      added support all sensor when get/set attribute
                     read both temperature and pressure when pressure is requested
02/22/11     AS      Draft

==============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd.h"

#include "sns_dd_alt_bmp085_priv.h"

#include "fixed_point.h"    /* FX_FLTTOFIX_Q16() */

/* Include files required to support alt log packet */
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/

/*=======================================================================

                   INTERNAL DEFINES & ENUMS

========================================================================*/

#define ONE_BYTE 1
#define ONE_WORD 2

/**
 * Swap the MS byte and the LS byte in a 16 bit word.
 */
#define SWAP16(val) ((val << 8) | (val >> 8))

/**
 * Oversampling and Resolution Mode
 */
typedef enum
{
    BMP085_MODE_ULTRA_LOW_POWER = 0,
    BMP085_MODE_STANDARD = 1,
    BMP085_MODE_HIGH_RESOLUTION = 2,
    BMP085_MODE_ULTRA_HIGH_RESOLUTION = 3,
    BMP085_NUM_MODES
} bmp085_mode_e ;

/**
 * State
 * Note: The sensor automatically moves to standby after measurement.
 */
typedef enum
{
    BMP085_STATE_IDLE = 0,
    BMP085_STATE_START_MEASURE_PRESSURE = 1,
    BMP085_STATE_START_MEASURE_TEMPERATURE = 2,
    BMP085_STATE_START_MEASURE_TEMPERATURE_AND_PRESSURE = 3,
} bmp085_state_e ;

/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_alt_bmp085_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_alt_bmp085_set_attr(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value);

static sns_ddf_status_e sns_dd_alt_bmp085_get_attr(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems);

static void sns_dd_alt_bmp085_handle_timer(
    sns_ddf_handle_t dd_handle,
    void* arg);

static void sns_dd_alt_bmp085_interrupt_handler(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp);

static sns_ddf_status_e sns_dd_alt_bmp085_reset(sns_ddf_handle_t dd_handle);
static sns_ddf_status_e sns_dd_alt_bmp085_self_test( 
				sns_ddf_handle_t dd_handle, 
				sns_ddf_sensor_e sensor, 
				sns_ddf_test_e test,
				uint32_t* err);

static sns_ddf_status_e sns_dd_alt_bmp085_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors );


/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_alt_bmp085_driver_fn_list =
{
    .init = &sns_dd_alt_bmp085_init,
    .get_data = &sns_dd_alt_bmp085_get_data,
    .set_attrib = &sns_dd_alt_bmp085_set_attr,
    .get_attrib = &sns_dd_alt_bmp085_get_attr,
    .handle_timer = &sns_dd_alt_bmp085_handle_timer,
    .handle_irq = &sns_dd_alt_bmp085_interrupt_handler,
    .reset = &sns_dd_alt_bmp085_reset,
    .run_test = &sns_dd_alt_bmp085_self_test,
    .enable_sched_data = NULL,
    .probe = &sns_dd_alt_bmp085_probe
};

/*
 * Device specific calibration parameters stored in PROM.
 * Calibration is done in the factory and requires complicated equipment.
 */
typedef struct {
   int16_t ac1;
   int16_t ac2;
   int16_t ac3;
   uint16_t ac4;
   uint16_t ac5;
   uint16_t ac6;
   int16_t b1;
   int16_t b2;
   int16_t mb;
   int16_t mc;
   int16_t md;
} bmp085_calibration_param_t;


/**
 *  Driver context.
 *
 *  @i2c_handle - i2c handle used to access the I2C bus.
 *  @smgr_handle - sensor manager handle used to notify_data.
 *  @timer - timer object used to delay until conversion completed.
 *  @mode - power and oversampling mode.
 *  @state - idle or measurement in progress.
 *  @chip_id - reported from the device.
 *  @version - reported from the device.
 *  @param_b5 - calculated parameter depends on temperature.
 *  @ut - uncompansated temperature.
 *  @up - uncompansated pressure.
 *  @temperature - in 0.1°C.
 *  @pressure - in Pascal.
 *  @calibration - calibration parameters.
 */
typedef struct  {
    sns_ddf_handle_t    i2c_handle;
    sns_ddf_handle_t    smgr_handle;
    sns_ddf_timer_s     timer;

    bmp085_mode_e mode;
    bmp085_state_e state;

    uint8_t chip_id;
    uint8_t version;

    int32_t param_b5;

    uint16_t ut;
    uint32_t up;

    int32_t temperature;
    int32_t pressure;
    boolean  temp_requested;
    boolean  pressure_requested;

    sns_ddf_sensor_data_s    data_report[BMP085_NUM_DATA_TYPES];
    sns_ddf_sensor_sample_s  sample[BMP085_NUM_DATA_TYPES];

    bmp085_calibration_param_t calibration;

#ifdef CONFIG_BMP085_UNIT_TEST
    sns_ddf_sensor_data_s*    report;
#endif
} bmp085_drv_t;

/**
 *  Mode Prarameters.
 *
 *  @conversion_time_usec - Conversion time in micro-seconds.
 *  @avg_current_uA - avareage current in micro-ampers.
 *  @bits - number of bits.
 *  @num_of_samples - internal_number_of_samples
 *  @rms_nois_pa - RMS noise in Pascals.
 */
typedef struct
{
    uint32_t conversion_time_usec;
    uint32_t avg_current_uA;
    uint32_t bits;
    uint32_t num_of_samples;
    uint32_t rms_nois_pa;
} bmp085_mode_param_t;
/*============================================================================

  Local Data Definitions

 ============================================================================*/

/**
 * Driver state.
 *
 * This variable is mainly for debug,
 * since the device registers are access via I2C.
 */
static bmp085_drv_t *bmp085_drv_dbg = NULL;

static sns_ddf_sensor_e bmp085_sensors[] =
{
    SNS_DDF_SENSOR_PRESSURE,
    SNS_DDF_SENSOR_TEMP
};


/**
 * Mode Prarameters.
 */
static const bmp085_mode_param_t bmp085_mode_param[BMP085_NUM_MODES] =
{
//   usec , uA , bits , #   noise
    {4500 ,  3 , 16 , 1 ,  6}, // ULTRA_LOW_POWER
    {7500 ,  5 , 17 , 2 ,  5}, // STANDARD
    {13500,  7 , 18 , 4 ,  4}, // HIGH_RESOLUTION
    {25500, 12 , 19 , 8 ,  3}, // ULTRA_HIGH_RESOLUTION
};

/**
 * Reported Temprature resolution is 0.1°C (Celcius).
 */
static const q16_t bmp085_temperature_resolution = FX_FLTTOFIX_Q16(0.1);

/**
 * Reported Pressure resolution is 1.0 Pa (Pascal) = 0.01 hPa.
 * Expected range 80K to 120K Pascals.
 */
static const q16_t bmp085_pressure_resolution = FX_FLTTOFIX_Q16(0.01);

/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_check_chip_id

===========================================================================*/
/*!
  @brief Verify the chip id

  @detail Read the chip id and compare against expected value.

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_check_chip_id(bmp085_drv_t *drv)
{
    sns_ddf_status_e status;
    uint8_t chip_id = 0;
    uint8_t read_count = 0;

    status = sns_ddf_read_port(drv->i2c_handle,
                               BMP085_CHIP_ID_REG,
                               &chip_id, ONE_BYTE, &read_count);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    if (chip_id != BMP085_CHIP_ID)
    {
        return SNS_DDF_EDEVICE;
    }

    drv->chip_id = chip_id;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_read_calibration_params

===========================================================================*/
/*!
  @brief Read calibration parameters.

  @detail Read factory calibration parameters stored in PROM.

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_read_calibration_params(bmp085_drv_t *drv)
{
    sns_ddf_status_e status;
    uint16_t *ptr = (void*) &drv->calibration;
    uint8_t read_count = 0;
    int i;

    status = sns_ddf_read_port(drv->i2c_handle,
                               BMP085_REG_PROM_AC1_MSB,
                               (void*) &drv->calibration,
                               sizeof(drv->calibration),
                               &read_count);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Swap the MS byte with the LS byte */
    for (i = 0; i < sizeof(drv->calibration)/2; i++)
    {
        ptr[i] = SWAP16(ptr[i]);
    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_start_measure_temperature

===========================================================================*/
/*!
  @brief Start the HW to measure temperature.

  @detail

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_start_measure_temperature(bmp085_drv_t *drv)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t write_count = 0;
    uint8_t write_buffer = 0;

   write_buffer = BMP085_TEMP_MEASURE;
   status = sns_ddf_write_port(drv->i2c_handle,
                               BMP085_CONTROL_REG,
                               &write_buffer,
                               ONE_BYTE,
                               &write_count);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }
   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_get_ut

===========================================================================*/
/*!
  @brief Read uncompensated temperature.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[out] ut: Pointer to the uncompensated temperature.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_get_ut(bmp085_drv_t *drv, uint16_t *ut)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint16 uncompensated_temp;
    uint8_t read_count = 0;

   status = sns_ddf_read_port(drv->i2c_handle,
                              BMP085_ADC_OUT_MSB_REG,
                              (void*) &uncompensated_temp,
                              sizeof(uncompensated_temp),
                              &read_count);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   uncompensated_temp = SWAP16(uncompensated_temp);

   *ut = uncompensated_temp;

   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_start_measure_pressure

===========================================================================*/
/*!
  @brief Start the HW to measure pressure.

  @detail

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_start_measure_pressure(bmp085_drv_t *drv)
{
   sns_ddf_status_e status = SNS_DDF_SUCCESS;
   uint8_t write_count = 0;
   uint8_t write_buffer = 0;

   /*
    * Using Ultra-Low-Power mode (osrs = 0)
    * for the fastest conversion time and lowest power consumtion
    */
   write_buffer = BMP085_PRESS_MEASURE + (drv->mode << 6);

   status = sns_ddf_write_port(drv->i2c_handle,
                               BMP085_CONTROL_REG,
                               &write_buffer,
                               ONE_BYTE,
                               &write_count);


   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_get_up

===========================================================================*/
/*!
  @brief Read uncompensated pressure.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[out] up: Pointer to the uncompensated pressure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_get_up(bmp085_drv_t *drv, uint32_t *up)
{
   sns_ddf_status_e status = SNS_DDF_SUCCESS;
   uint32 uncompensated_press;
   uint8_t read_count = 0;
   uint8_t data[3] = {0}; /* MSB , LSB , XLSB */

   status = sns_ddf_read_port(drv->i2c_handle,
                              BMP085_ADC_OUT_MSB_REG,
                              data,
                              sizeof(data),
                              &read_count);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   uncompensated_press = (((uint32_t) data[0] << 16) |
                          ((uint32_t) data[1] << 8) |
                           (uint32_t) data[2]) >> (8 - drv->mode);

   *up = uncompensated_press;

   return SNS_DDF_SUCCESS;

}

/*===========================================================================

  FUNCTION:   bmp085_calculate_temperature

===========================================================================*/
/*!
  @brief Calculate temperature.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] ut: uncompensated temperature.

  @return
    temperature in 0.1°C

  $TODO:
*/
/*=========================================================================*/
int32_t bmp085_calculate_temperature(bmp085_drv_t *drv, uint16_t ut)
{
    bmp085_calibration_param_t *cal = &drv->calibration;
    int32_t temperature = 0;
    int32_t x1,x2;

   x1 = (((int32_t) ut - (int32_t) cal->ac6) * (int32_t) cal->ac5) >> 15;
   x2 = ((int32_t) cal->mc << 11) / (x1 + cal->md);
   /* Note: Save b5 param - to be use for pressure calculation. */
   drv->param_b5 = x1 + x2;

   temperature = ((drv->param_b5 + 8) >> 4);  /* temperature in 0.1°C */

   return temperature;
}

/*===========================================================================

  FUNCTION:   bmp085_calculate_pressure

===========================================================================*/
/*!
  @brief Calculate pressure

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] up: uncompensated pressure.

  @return
    pressure in Pascal. Expected range 80K to 120K Pascals.

  $TODO:
*/
/*=========================================================================*/
int32_t bmp085_calculate_pressure(bmp085_drv_t *drv, uint32_t up)
{
    bmp085_calibration_param_t *cal = &drv->calibration;
    int32_t pressure,x1,x2,x3,b3,b6;
    uint32_t b4, b7;
    int32_t osrs = drv->mode;

   b6 = drv->param_b5 - 4000;

   /*****calculate B3************/
   x1 = (b6*b6) >> 12;
   x1 *= cal->b2;
   x1 >>=11;

   x2 = (cal->ac2*b6);
   x2 >>=11;

   x3 = x1 +x2;

	b3 = (((((int32_t)cal->ac1 )*4 + x3) << osrs) + 2) >> 2;

   /*****calculate B4************/
   x1 = (cal->ac3* b6) >> 13;
   x2 = (cal->b1 * ((b6*b6) >> 12) ) >> 16;
   x3 = ((x1 + x2) + 2) >> 2;
   b4 = (cal->ac4 * (uint32_t) (x3 + 32768)) >> 15;

   b7 = ((uint32_t)(up - b3) * (50000 >> osrs));

   if (b7 < 0x80000000)
   {
     pressure = (b7 << 1) / b4;
   }
   else
   {
     pressure = (b7 / b4) << 1;
   }

   x1 = pressure >> 8;
   x1 *= x1;
   x1 = (x1 * BMP085_PARAM_MG) >> 16;
   x2 = (pressure * BMP085_PARAM_MH) >> 16;
   pressure += (x1 + x2 + BMP085_PARAM_MI) >> 4; /* pressure in Pa */

   return (pressure);
}

/*===========================================================================

  FUNCTION:   bmp085_log_data

===========================================================================*/
/*!
  @brief log data

  @detail

  @param[in] data_ptr: pointer to the data structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

*/
/*=========================================================================*/
static void bmp085_log_data(sns_ddf_sensor_data_s *data_ptr)
{
    sns_err_code_e status;
    sns_log_sensor_data_pkt_s* log;

    if(data_ptr == NULL)
      return;

    status = sns_logpkt_malloc(
        SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log);
    if ( (status == SNS_SUCCESS) && (log != NULL) )
    {
        log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log->sensor_id = data_ptr->sensor;
        log->vendor_id = SNS_DDF_VENDOR_BOSCH;
        log->timestamp = data_ptr->timestamp;
        log->num_data_types = 1;
        log->data[0] = data_ptr->samples[0].sample;
        sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
    }
}

/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail

  @param[in] drv: pointer to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: Soft-Reset
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_reset(sns_ddf_handle_t dd_handle)
{
    bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;
    /* Sensor reset is done only by the GPIO - controlled by the daemon. */
    drv->pressure_requested = false;
    drv->temp_requested = false;
    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_init

===========================================================================*/
/*!
  @brief Initializes the 3 axis Bosch alterometer BMP085
              determines the device to use

  @detail
  - Allocates memory for driver drv structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_alt_bmp085_reset routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors)
{
    sns_ddf_status_e status;
    bmp085_drv_t *drv;
    uint8_t read_count = 0;
    uint8_t version = 0;

    status = sns_ddf_malloc((void **)&drv, sizeof(bmp085_drv_t));

    if (status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }

    memset(drv, 0, sizeof(bmp085_drv_t));

    drv->smgr_handle = smgr_handle;

    status = sns_ddf_open_port(&(drv->i2c_handle),
                               &(device_info->port_config));

    if (status != SNS_DDF_SUCCESS)
    {
        goto port_err;
    }

    status = sns_ddf_timer_init(&drv->timer,
                                (sns_ddf_handle_t) drv,
                                &sns_alt_bmp085_driver_fn_list,
                                drv, false);

    if (status != SNS_DDF_SUCCESS)
    {
        goto timer_err;
    }

    /* Fill out supported sensor info */
    *num_sensors = 2;
    *sensors = bmp085_sensors;

    *dd_handle_ptr = (sns_ddf_handle_t) drv;
    bmp085_drv_dbg = drv; /* debug */

    /* Verify access to the device */
    status = sns_dd_alt_bmp085_check_chip_id(drv);

    if (status != SNS_DDF_SUCCESS)
    {
        goto init_err;
    }

    status = sns_ddf_read_port(drv->i2c_handle,
                               BMP085_VERSION_REG,
                               &version,
                               ONE_BYTE, &read_count);

    drv->version = version;

    if (status != SNS_DDF_SUCCESS)
    {
        goto init_err;
    }

    status = sns_dd_alt_bmp085_read_calibration_params(drv);

    if (status != SNS_DDF_SUCCESS)
    {
        goto init_err;
    }

    /* Set the sensor to default setup */
    status = sns_dd_alt_bmp085_reset(drv);

    drv->mode = BMP085_MODE_ULTRA_LOW_POWER; /* oversampling = 0 */

    if (status != SNS_DDF_SUCCESS)
    {
        goto init_err;
    }

    /*
     * Read Temperature for calculation of param_b5 ,
     * which is required for pressure calculation.
     */
    status = sns_dd_alt_bmp085_start_measure_temperature(drv);
    if (status)
    {
        goto init_err;
    }
    sns_ddf_delay(bmp085_mode_param[drv->mode].conversion_time_usec);
    status = sns_dd_alt_bmp085_get_ut(drv, &drv->ut);
    if (status)
    {
        goto init_err;
    }
    drv->temperature = bmp085_calculate_temperature(drv, drv->ut);

    return SNS_DDF_SUCCESS;

init_err:
    sns_ddf_timer_release(drv->timer);
timer_err:
    sns_ddf_close_port(drv->i2c_handle);
port_err:
    sns_ddf_mfree(drv);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the bmp085 alterometer

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: DDF should add more attributes to support new features of the device.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_set_attr(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value_ptr)
{
    bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;

    if ((sensor != SNS_DDF_SENSOR_PRESSURE) && (sensor != SNS_DDF_SENSOR__ALL))
    {
        BMP085_ERR_MSG("invalid sensor");
        return SNS_DDF_EINVALID_PARAM;
    }

    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e power_state = *((sns_ddf_powerstate_e*) value_ptr);

            /*
             * Note: The sensor automatically moves to standby
             * after sampling temperature or pressure.
             * If current state is not IDLE, it means that we already started
             * measurement mode and started the timer.
             */
            if ((power_state == SNS_DDF_POWERSTATE_LOWPOWER) &&
                (drv->state != BMP085_STATE_IDLE))
            {
                sns_ddf_timer_cancel(drv->timer);
                drv->state = BMP085_STATE_IDLE;

                /* reset the data requested state */
                drv->pressure_requested = false;
                drv->temp_requested = false;
            }
        }
        break;
    case SNS_DDF_ATTRIB_RANGE:
        /* Not Applicable */
        break;
    case SNS_DDF_ATTRIB_LOWPASS:
        /* Not Applicable */
        break;
    case SNS_DDF_ATTRIB_RESOLUTION:
        /*
        * Not Applicable
        * Using Ultra-Low-Power mode (osrs = 0)
        * for the fastest conversion time and lowest power consumtion.
        */
        break;

    default:
        BMP085_ERR_MSG("invalid attrib");
        return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - range and resolution info is from the device data sheet.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in] attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_alt_bmp085_get_attr(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems)
{
    bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;

    if ((sensor != SNS_DDF_SENSOR_PRESSURE) && (sensor != SNS_DDF_SENSOR_TEMP)
         && (sensor != SNS_DDF_SENSOR__ALL))
    {
        BMP085_ERR_MSG("invalid sensor");
        return SNS_DDF_EINVALID_PARAM;
    } 

    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power_attrib;
            power_attrib = sns_ddf_memhandler_malloc(memhandler,
                                                     sizeof(sns_ddf_power_info_s));
            if (power_attrib == NULL)
            {
                BMP085_ERR_MSG("malloc failure power_attrib");
                return SNS_DDF_ENOMEM;
            }
            /*
             * 3 uA in Ultra-Low-Power mode (osrs = 0),
             * Lowest power consumtion and fastest conversion time.
             */
            power_attrib->active_current = bmp085_mode_param[drv->mode].avg_current_uA;

            /* 0.1 uA in Standby mode (automatically after measurement). */
            power_attrib->lowpower_current = 0;
            *num_elems = 1;
            *value = power_attrib;
        }
        break;

    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;

            device_ranges = sns_ddf_memhandler_malloc(memhandler,
                                                      sizeof(sns_ddf_range_s));
            if (device_ranges == NULL)
            {
                BMP085_ERR_MSG("malloc failure power_attrib");
                return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;

            if (sensor == SNS_DDF_SENSOR_PRESSURE)
            {
                /*
                 * Pressure Range is 300 to 1100 hPa = 30,000 to 110,000 Pascals.
                 * Which is +9000 meters to -500 meters at sea level.
                 * Pressure at sea level is 1013.25 hPa.
                 * A pressure change of 1 hPa (100 Pascals) corresponds to 8.43 meters
                 * at sea level.
                 */
                device_ranges[0].min = FX_FLTTOFIX_Q16(300.0);
                device_ranges[0].max = FX_FLTTOFIX_Q16(1100.0);
            }

            if (sensor == SNS_DDF_SENSOR_TEMP)
            {
                /*
                 *  Temperature Operational range is -40°C to +85°C.
                 *  Full Accuracy range is 0°C to +65°C.
                 */
                device_ranges[0].min = FX_FLTTOFIX_Q16(-40.0 );
                device_ranges[0].max = FX_FLTTOFIX_Q16( 85.0 );
            }

            *value = device_ranges;
        }
        break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                 return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            /* 16 to 19 bits - according to mode. */
            resp->bit_len = bmp085_mode_param[drv->mode].bits;
            /*
             *  4.5 msec is the minimal conversion time (osrs=0),
             *  adding software handling will take about 10 msec per sample,
             *  therefore about 100 samples per second.
             */
            resp->max_freq = 30;
            *value = resp;
        }
        break;

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *device_res;
            int res_num = 1; /* Only one resolution supported */

            device_res = sns_ddf_memhandler_malloc(memhandler,
                                                   res_num*sizeof(sns_ddf_resolution_t));

            if (device_res == NULL)
            {
                BMP085_ERR_MSG("malloc failure device_res");
                return SNS_DDF_ENOMEM;
            }

            *num_elems = res_num;

            if (sensor == SNS_DDF_SENSOR_PRESSURE)
            {
                device_res[0] = bmp085_pressure_resolution; /* 0.01 hPa */
            }

            if (sensor == SNS_DDF_SENSOR_TEMP)
            {
                device_res[0] = bmp085_temperature_resolution; /* 0.1°C */
            }

            *value = device_res;
        }
        break;

    case SNS_DDF_ATTRIB_LOWPASS:
        /* Not supported */
        return SNS_DDF_EINVALID_PARAM;

    case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *device_delay;
            device_delay = sns_ddf_memhandler_malloc(memhandler,
                                                     sizeof(sns_ddf_delays_s));
            if (device_delay == NULL)
            {
                BMP085_ERR_MSG("malloc failure device_delay");
                return SNS_DDF_ENOMEM;
            }
            /*
             * Start-Up time after power up.
             * Before first communication is 10.0 milisec
             */
            device_delay->time_to_active = 10000;
            /*
             * Conversion time at Ultra-Low-Power mode is 4.5 milisec.
             */
            device_delay->time_to_data = bmp085_mode_param[drv->mode].conversion_time_usec;

            *value = device_delay;
            *num_elems = 1;
        }
        break;

    case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *driver_info;
            driver_info = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_driver_info_s));
            if (driver_info == NULL)
            {
                BMP085_ERR_MSG("malloc failure driver_info");
                return SNS_DDF_ENOMEM;
            }

            driver_info->name = "BMP085/BMP180";
            driver_info->version = 1;

            *num_elems = 1;
            *value = driver_info;
        }
        break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *device_info;
            device_info = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_device_info_s));
            if (device_info == NULL)
            {
                BMP085_ERR_MSG("malloc failure device_info");
                return SNS_DDF_ENOMEM;
            }

            device_info->vendor = "BOSCH";
            device_info->model = "BMP085/BMP180";
            device_info->name = "Pressure";
            device_info->version = 1; 
            *value = device_info;
            *num_elems = 1;
        }
        break;

    default:
        BMP085_ERR_MSG("invalid attrib");
        return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors.

  @param[in] dd_handle     Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: use FIFO for multiple samples per axis.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alt_bmp085_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s**  data)
{
    sns_ddf_status_e status;
    uint8_t          i;
    bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;
    /* Conversion time for temperature is always equals to the pressure conversion time at ULTRA_LOW_POWER mode which is 4.5ms */
    int conversion_time_usec = bmp085_mode_param[BMP085_MODE_ULTRA_LOW_POWER].conversion_time_usec;

    /* Check parameters */
    if (dd_handle == NULL || sensors == NULL || memhandler == NULL  )
    {
      BMP085_ERR_MSG("sensor not initialized");
      return SNS_DDF_EINVALID_PARAM;
    }

    for (i = 0; i < num_sensors; i++)
    {
      if ( sensors[i] == SNS_DDF_SENSOR_PRESSURE  )
      {
          if (drv->pressure_requested )
          {
             BMP085_ERR_MSG3("pressure requested; state=%d req=0x%x", drv->state, 
                             drv->pressure_requested | (drv->temp_requested<<4), 0);
          }
          else
          {
             drv->pressure_requested = true; 
          }
      }
      else if ( sensors[i] == SNS_DDF_SENSOR_TEMP  )
      {
          if (drv->temp_requested )
          {
              BMP085_ERR_MSG3("temp requested; state=%d req=0x%x", drv->state, 
                              drv->pressure_requested | (drv->temp_requested<<4), 0);
          }
          else
          {
             drv->temp_requested = true; 
          }
      }
      else
      {
         BMP085_ERR_MSG("invalid sensor type");
         return SNS_DDF_EINVALID_PARAM;
      }
    }

    if ( drv->state == BMP085_STATE_IDLE )
    {
        status = sns_dd_alt_bmp085_start_measure_temperature(drv);
        if (status)
        {
            return status;
        }
        if (drv->pressure_requested == false)
        {
            drv->state = BMP085_STATE_START_MEASURE_TEMPERATURE;
        }
        else
        {
            drv->state = BMP085_STATE_START_MEASURE_TEMPERATURE_AND_PRESSURE;
        }
    }
    else
    {
        if ( (drv->state == BMP085_STATE_START_MEASURE_TEMPERATURE) &&
             drv->pressure_requested )
        {
            drv->state = BMP085_STATE_START_MEASURE_TEMPERATURE_AND_PRESSURE;
        }
        return SNS_DDF_PENDING;
    }

#ifdef CONFIG_BMP085_UNIT_TEST
    sns_ddf_delay(conversion_time_usec);
    sns_dd_alt_bmp085_handle_timer(dd_handle, NULL);
    *data = drv->report;
    return drv->report->status;
#else
    status = sns_ddf_timer_start(drv->timer, conversion_time_usec);

    if (status)
    {
        return status;
    }

    return SNS_DDF_PENDING;
#endif
}


/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_interrupt_handler

===========================================================================*/
/*!
  @brief BMP085 interrupt handler

  @detail

  @param[in] drv: pointer to the driver structure
  @param[in] irq:

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: Sensor interrupt line (MSM GPIO) is not routed to the SIC
         (the ARM7 Interrupt controller).
*/
/*=========================================================================*/
static void sns_dd_alt_bmp085_interrupt_handler(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
    SNS_ASSERT(0); /* Shouldn't happen */
}

/*===========================================================================

  FUNCTION:   sns_dd_alt_bmp085_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is
         the callback function submitted when initializing a timer.

  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] arg        The argument submitted when the timer was set.

  @return None
*/
/*=========================================================================*/
static void sns_dd_alt_bmp085_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
    sns_ddf_status_e status = SNS_DDF_ENOMEM; /* Default for malloc error */
    
    bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;

    if (drv->state == BMP085_STATE_IDLE)
    {
        drv->pressure_requested = false;
        drv->temp_requested = false;
        return;    /* return silently. A timer can be queued when DD is being changed to IDLE STATE */
    }

    if (drv->state == BMP085_STATE_START_MEASURE_TEMPERATURE_AND_PRESSURE)
    {
        /* save temperature data in report[1], and reserve report[0] for pressure data*/
        drv->data_report[1].timestamp   = sns_ddf_get_timestamp();
        drv->data_report[1].samples     = &(drv->sample[1]);
        drv->data_report[1].num_samples = 1;
        drv->data_report[1].sensor = SNS_DDF_SENSOR_TEMP;

        status = sns_dd_alt_bmp085_get_ut(drv, &drv->ut);
        drv->data_report[1].status = status;
        if (status)
        {
            goto report_err;
        }

        drv->temperature = bmp085_calculate_temperature(drv, drv->ut);
        drv->sample[1].sample = drv->temperature * bmp085_temperature_resolution;
        drv->sample[1].status = SNS_DDF_SUCCESS;

        if (drv->temp_requested)
        {
            bmp085_log_data(&drv->data_report[1]);
        }

        status = sns_dd_alt_bmp085_start_measure_pressure(drv);
     
        if (status)
        {
            goto report_err;
        }

        drv->state = BMP085_STATE_START_MEASURE_PRESSURE;
        status = sns_ddf_timer_start(drv->timer, bmp085_mode_param[drv->mode].conversion_time_usec);
        if (status)
        {
            goto report_err;
        }
        return;
     }

    if (drv->state == BMP085_STATE_START_MEASURE_TEMPERATURE)
    {

        drv->data_report[0].timestamp   = sns_ddf_get_timestamp();
        drv->data_report[0].samples     = drv->sample;
        drv->data_report[0].num_samples = 1;
        drv->data_report[0].sensor = SNS_DDF_SENSOR_TEMP;

        status = sns_dd_alt_bmp085_get_ut(drv, &drv->ut);

        if (status)
        {
            goto report_err;
        }

        drv->temperature = bmp085_calculate_temperature(drv, drv->ut);
        drv->sample[0].sample = drv->temperature * bmp085_temperature_resolution;
        drv->sample[0].status = SNS_DDF_SUCCESS;

        bmp085_log_data(&drv->data_report[0]);
    }

    if (drv->state == BMP085_STATE_START_MEASURE_PRESSURE)
    {
        drv->data_report[0].timestamp   = sns_ddf_get_timestamp();
        drv->data_report[0].samples     = drv->sample;
        drv->data_report[0].num_samples = 1;
        drv->data_report[0].sensor = SNS_DDF_SENSOR_PRESSURE;

        status = sns_dd_alt_bmp085_get_up(drv, &drv->up);

        if (status)
        {
            goto report_err;
        }

        drv->pressure = bmp085_calculate_pressure(drv, drv->up);
        drv->sample[0].sample = drv->pressure * bmp085_pressure_resolution;
        drv->sample[0].status = SNS_DDF_SUCCESS;

        bmp085_log_data(&drv->data_report[0]);
    }

report_err:
  
    drv->data_report->status = status;

#ifdef CONFIG_BMP085_UNIT_TEST
    drv->report = drv->data_report;
#else
    if (drv->smgr_handle != NULL)
    {
        if( drv->pressure_requested && drv->temp_requested )
        {
            sns_ddf_smgr_notify_data(drv->smgr_handle, drv->data_report, 2);
        }
        else if( drv->pressure_requested || drv->temp_requested )
        {
            sns_ddf_smgr_notify_data(drv->smgr_handle, drv->data_report, 1);
        }
        else
        {
            BMP085_ERR_MSG3("bad state=%d", drv->state, 0, 0);
        }
        drv->pressure_requested = drv->temp_requested = false;
    }
 
#endif
    /* Measurement completed, set back to idle state regardless of any error. */
	drv->state = BMP085_STATE_IDLE;

}

/* Avoid warnings from compiler/Linker */
void bmp085_dummy(void)
{
    bmp085_drv_dbg->chip_id = 0;
    bmp085_log_data(NULL);

}


/**
	\brief Macro holding the lower bound value for the pressure in hPa
*/
#define PLAUSIBLE_PRESS_MIN	900		// in hPa
/**
	\brief Macro holding the upper bound value for the pressure in hPa
*/
#define PLAUSIBLE_PRESS_MAX	1100	// in hPa
/**
	\brief Macro holding the lower bound value for the temperature in deg celsius 
*/
#define PLAUSIBLE_TEMP_MIN	0		// in deg celsius
/**
	\brief Macro holding the upper bound value for the temperature in deg celsius 
*/
#define PLAUSIBLE_TEMP_MAX	40		// in deg celsius

/**
	\brief This function checks the uncompensated pressure and temperature value for valid ranges
	\param tUT uncompensated temperature value\n
	\param tUP uncompensated pressure value
	\return  0 if the action is successful \n
			32	tUT = 0 \n
			33	tUT < lower bound (16000) @ RT \n
			36	tUT = all bits 1 \n
			48	tUP = 0 \n
			49	tUP < lower bound @ RT, room pressure (min. 900 hPa) \n
			51	tUP = all bits 1
*/
unsigned char bmp085_st_check_UTUP(unsigned int tUT,unsigned int tUP)
{
// check UT and UP for valid range
	if(tUT==0)					
		return 32;
	else if(tUT < 16000)		
		return 33;
	else if(tUT == 65535)		
		return 36;
	else if(tUP==0)				
		return 48;
	else if(tUP < 15000)		
		return 49;
	else if(tUP == 65535)		
		return 51;
	else return 0;	
}


/**
	\brief This function checks if the calibration parameters are in 4 sigma range
	\param bmp085_drv_t *drv
	\return  0 if the action is successful \n
			64	calibration parameter AC6 below lower bound \n
			65	calibration parameter AC6 above upper bound \n
			66	calibration parameter AC5 below lower bound \n
			67	calibration parameter AC5 above upper bound \n
			68	calibration parameter AC1 below lower bound \n
			69	calibration parameter AC1 above upper bound \n
			70	calibration parameter AC2 below lower bound \n
			71	calibration parameter AC2 above upper bound \n
			72	calibration parameter AC4 below lower bound \n
			73	calibration parameter AC4 above upper bound \n
			74	calibration parameter AC3 below lower bound \n
			75	calibration parameter AC3 above upper bound \n
			123	calibration parameter B1, B2, MC and MD out of range
*/

unsigned char bmp085_st_check_cal_param (bmp085_drv_t *drv)
{
	// check if calibration parameters are in 4 sigma range
	if (drv->calibration.ac1 <= -29250)		
		return 68;
	else if(drv->calibration.ac1 >= 32750)
		return 69;
	else if(drv->calibration.ac2 <= -4820) 
		return 70;
	else if(drv->calibration.ac2 >= 3626) 	
		return 71;
	else if(drv->calibration.ac3 <= -16660)
		return 74;	
	else if(drv->calibration.ac3 >= -13151) 
		return 75;
	else if(drv->calibration.ac4 <= 20780)
		return 72;
	else if(drv->calibration.ac4 >= 44820) 
		return 73;
	else if(drv->calibration.ac5 <= 21730)
		return 66;
	else if(drv->calibration.ac5 >= 34475) 
		return 67;
	else if(drv->calibration.ac6 <= 500)
		return 64;
	else if(drv->calibration.ac6 >= 56200) 
		return 65;
	else if(drv->calibration.b1 >= 8637)
		return 123;
	else if(drv->calibration.b1 <= 4935) 
		return 123;
	else if(drv->calibration.b2 >= 297)
		return 123;
	else if(drv->calibration.b2 <= -182) 
		return 123;
	else return 0;
}


/**
 \brief This function is a subfunction of the crc check
 \param seed actual seed value
 \param data actual data for calculation
 \return  new seed value
*/

unsigned char bmp085_st_calc_crc(unsigned char seed, unsigned char data)
{ 
	unsigned char poly = 0x1D;
	unsigned char bit, din;
	
	for (bit = 0; bit < 8; bit++)
	{ 
		if (((seed & 0x80) > 0) ^ ((data & 0x80) > 0))
			din = 1;
		else
			din = 0;
		seed = (seed & 0x7F) << 1;
		data = (data & 0x7F) << 1;
		seed = seed ^ (poly * din);
	}
	
	return seed;
}


/**
	\brief This function checks if the crc register content is valid
	\param bmp085_drv_t *drv
	\return  0 if the action is successful \n
			100	if the crc register content is not equal to the calculated CRC value
*/
int bmp085_st_check_CRC (bmp085_drv_t *drv)
{
	unsigned char i;	
	unsigned char  current_register;
	unsigned char crc_val;
	unsigned char registers[32]; 
	sns_ddf_status_e status;

	uint8_t read_count = 0;



	// read all relevant registers for CRC calculation


	status = sns_ddf_read_port(drv->i2c_handle,
								0x80,
								registers, 32, &read_count);
	
	 if (status != SNS_DDF_SUCCESS)
	 {
		 return 2;
	 }



	crc_val = 0xFF;

	for (i=0; i<32; i++)
	{ 
		current_register = registers[i];
		if (i != 4) /* that is ee_crc register */
			crc_val = bmp085_st_calc_crc(crc_val, current_register);
	}
	
	crc_val = (crc_val ^ 0xFF);
	if (crc_val == registers[4]) // calculated crc is correct
	{
		return 0;
	}
	else // data or crc is wrong
	{
		return 100;
	}
}


/**
	\brief This function checks for any error that could have occurred
	\param	bmp085_drv_t *drv
	\return 0	sensor ok \n
			2	no I2C communication or no BMP085 type found \n
			32	tUT = 0 \n
			33	tUT < lower bound (16000) @ RT \n
			36	tUT = all bits 1 \n
			48	tUP = 0 \n
			49	tUP < lower bound @ RT, room pressure (min. 900 hPa) \n
			51	tUP = all bits 1 \n
			64	calibration parameter AC6 below lower bound \n
			65	calibration parameter AC6 above upper bound \n
			66	calibration parameter AC5 below lower bound \n
			67	calibration parameter AC5 above upper bound \n
			68	calibration parameter AC1 below lower bound \n
			69	calibration parameter AC1 above upper bound \n
			70	calibration parameter AC2 below lower bound \n
			71	calibration parameter AC2 above upper bound \n
			72	calibration parameter AC4 below lower bound \n
			73	calibration parameter AC4 above upper bound \n
			74	calibration parameter AC3 below lower bound \n
			75	calibration parameter AC3 above upper bound \n
			100	CRC register content unequal calculated CRC \n
			123	calibration parameter B1, B2, MC and MD out of range \n
			124	sensor not calibrated, all calibration parameters = 0 \n
			222	pressure and temperature values not plausible (default: not from 900 hPa to 1100 hPa and 0 deg celsius to 40 deg celsius) \n
*/

unsigned char bmp085_st_check_for_errors(bmp085_drv_t *drv)
{

	unsigned char error_code_st;


	// check AC1-6 for 0
	if ((drv->calibration.ac1 == 0) && (drv->calibration.ac2 == 0) && (drv->calibration.ac3 == 0) && (drv->calibration.ac4 == 0) && (drv->calibration.ac5 == 0) && (drv->calibration.ac6 == 0)) // sensor not calibrated
	{
		return 124; // sensor not calibrated
	}
	//**********************************************************
	//************test range UT and UP test device**************
	//**********************************************************
	error_code_st = bmp085_st_check_UTUP(drv->ut,drv->up); // check UT and UP range
	if (error_code_st)
	{
		return error_code_st; // error 32 ... 51
	}
	//****************************************************
	//***calculate pressure and temperature test device***
	//****************************************************
    drv->temperature = bmp085_calculate_temperature(drv, drv->ut);
	drv->pressure = bmp085_calculate_pressure(drv, drv->up);



	//****************************************************
	//******check bmp085_st sigma ranges of cal. param.******
	//****************************************************
	error_code_st = bmp085_st_check_cal_param (drv);
	if(error_code_st)
	{
		return error_code_st; // error 64-75, 123
	}
	//**********************************************************
	//*************check PROM test device sensor****************
	//**********************************************************		
	error_code_st = bmp085_st_check_CRC (drv);
	if(error_code_st)
	{		
		return error_code_st; // error = 100
	}
	//**********************************************************
	//***************test range of dut sensor*******************
	//**********************************************************
	if((drv->temperature < PLAUSIBLE_TEMP_MIN*10) || (drv->temperature > PLAUSIBLE_TEMP_MAX*10) || (drv->pressure < PLAUSIBLE_PRESS_MIN*100) || (drv->pressure > PLAUSIBLE_PRESS_MAX*100)) // dut sensor range check
	{
		return 222;
	}
	return 0;
}


static sns_ddf_status_e sns_dd_alt_bmp085_self_test( 
				sns_ddf_handle_t dd_handle, 
				sns_ddf_sensor_e sensor, 
				sns_ddf_test_e test,
				uint32_t* err)
{
	bmp085_drv_t *drv = (bmp085_drv_t *)dd_handle;
	sns_ddf_status_e status;
	
	unsigned char selftest_error;


	/* Verify access to the device */
    status = sns_dd_alt_bmp085_check_chip_id(drv);

    if (status != SNS_DDF_SUCCESS)
    {
         return status;
    }

	status = sns_dd_alt_bmp085_read_calibration_params(drv);

    if (status != SNS_DDF_SUCCESS)
    {
         return status;
    }

	
	/*
	* Read Temperature for calculation of param_b5 ,
	* which is required for pressure calculation.
	*/
    status = sns_dd_alt_bmp085_start_measure_temperature(drv);
    if (status)
    {
 	    return status;
    }
    sns_ddf_delay(bmp085_mode_param[drv->mode].conversion_time_usec);
	
    status = sns_dd_alt_bmp085_get_ut(drv, &drv->ut);
    if (status)
    {
 	    return status;
    }

    status = sns_dd_alt_bmp085_start_measure_pressure(drv);
    if (status)
    {
        return status;
    }
	sns_ddf_delay(bmp085_mode_param[drv->mode].conversion_time_usec);
	
	
	status = sns_dd_alt_bmp085_get_up(drv, &drv->up);
	
	if (status)
	{
		return status;
	}

	selftest_error = bmp085_st_check_for_errors(drv);


	if(selftest_error == 0) status = SNS_DDF_SUCCESS;
	else status = SNS_DDF_EFAIL;

	sns_ddf_smgr_notify_test_complete(((bmp085_drv_t *)dd_handle)->smgr_handle,
	sensor, status, *err);

	return status;

	
}

static sns_ddf_status_e sns_dd_alt_bmp085_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              BMP085_CHIP_ID_REG,
                              &i2c_buff,
                              1,
                              &bytes_read );
                             
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != BMP085_CHIP_ID )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Registers registers are correct. This is probably a BMP085 */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PRESSURE;
    (*sensors)[1] = SNS_DDF_SENSOR_TEMP;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;
}
