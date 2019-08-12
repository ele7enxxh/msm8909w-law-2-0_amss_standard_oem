/*==============================================================================

    S E N S O R S   G Y R O S C O P E   D R I V E R

DESCRIPTION

   Impelements the Invensense 3050 gyroscope driver

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_gyro.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
0709/15      AH      Fix compiler warnings
09/19/13     vh      Fixed resolution change issue
07/24/13     vh      Eliminated compiler warnings
06/10/13     an      Probe function Update.
04/03/13     ag      Added error message in log_data function
01/09/13     sd      Removed sns_playback.h for QDSP6
----------   ---     -----------------------------------------------------------
06/19/12     vh      Added run time switch for I2C code
----------   ---     -----------------------------------------------------------
05/24/12     ag      - Check for bus error in inertial sensor test
                     - Remove feature FEATURE_MPU3050_SELF_TEST_IMPL
03/28/12     ag      Update reset sequence, self test delay, self test threshold
02/28/12     br      Inserted delay loop when resetting the device
02/01/12     ag      Return correct error code during self test for unsupported sensors
12/05/11     yk      Fixed bad merge that caused driver to adjust orientation statically.
11/30/11     add     added wait time after reset during selftest
11/08/11     yk      Added support for axis mapping.
10/20/11     sd      Use Consistent Pointer Type Between SMGR/DD When Setting Attributes
10/10/11     ag      Change self test implementation to invoke generic inertial test
09/06/11     ag      Self test related cleanup and changes from code review
09/06/11     ag      Updated self test startup sequence, cleanup
08/31/11     ag      Self test ported from MPL_4_0_1 for internal use
08/28/11     ag      Added place holder for self test ptr
08/13/11     br      change the order of sns_dd_gyro_lpfbw by ascending order
08/10/11     ks      moved gyro_init, get_attrib to sns_dd_gyro_ext.c for memory optimization
06/28/11     br      changed getting SNS_DDF_ATTRIB_RESOLUTION as range was changed
06/17/10     ad      Updated temperature max sampling frequency to 10Hz
06/16/11     ad      align gyro axes to SAE coordinate frame for 8960
06/08/11     ad      changed the default gyro range to +/-1000dps
04/08/10     RY      Fixed gyro x-axis for FLUID.
04/08/10     RY      Added axis conversion for FLUID and FFA/SURF.
11/03/11     ad      implement invensense app note for device bug workaround
                     AN-MPU3000A-18 & AN-MPU-3000A-02-01
02/03/11     br      Chnaged the range value for resolution calculation
01/21/11     sc      Added support for PCSIM playback
01/20/11     ad      Report measurement range limits in radians per second
01/11/11     yk      Added support for new attrib SNS_DDF_ATTRIB_RESOLUTION_ADC
                     and redefined SNS_DDF_ATTRIB_RESOLUTION.
11/10/10     ad      add support for resolution attribute
10/21/10     ad      align axes to agreed upon SAE coordinate frame
10/20/10     ad      added support for logging sensor data
10/18/10     ad      updated device clock source to PLL with gyro X reference
10/12/10     ad      added raw sample data bounds check
10/11/10     ad      added support for power states
10/08/10     ad      added gyroscope and temperature data computation
09/28/10     ad      initial version

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

#include "sns_dd_gyro_priv.h"

#include "sns_log_types.h"
#include "sns_log_api.h"

#include "sns_dd.h"
#include "stdbool.h"

#include "sns_debug_str.h"
#include "sns_debug_api.h"

#if defined (SNS_PCSIM)
#include "sns_playback.h"
#endif

#include "sns_dd_inertial_test.h"

/*=======================================================================
 EXTERN VARIABLES
========================================================================*/
extern volatile boolean EnableI2C;

/*=======================================================================
 INTERNAL DEFINITIONS AND TYPES
========================================================================*/
/* 10 ms */
#define GYRO_3050_IDLE_TO_READY_TIME 100000

//Effective gyroscope data scaled values
int32_t sns_dd_gyro_range_scale[GYRO_3050_NUM_FS_RANGES];

//Supported gyroscope range settings
static const uint32_t sns_dd_gyro_range[GYRO_3050_NUM_FS_RANGES] =
{
   GYRO_3050_BITS_FS_250DPS,
   GYRO_3050_BITS_FS_500DPS,
   GYRO_3050_BITS_FS_1000DPS,
   GYRO_3050_BITS_FS_2000DPS
};

enum
{
   GYRO_3050_SF_NORMAL,
   GYRO_3050_SF_CORRECTED,
   GYRO_3050_NUM_SF
};

//Supported gyroscope range scaled values
static const int32_t sns_dd_gyro_range_sf[GYRO_3050_NUM_SF][GYRO_3050_NUM_FS_RANGES] =
{
  {
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF,1),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF,2),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF,4),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF,8)
  },
  {
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF_COR,1),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF_COR,2),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF_COR,4),
   GYRO_3050_DATA_SCALE(GYRO_3050_LSB_PER_DPS_SF_COR,8)
  }
};

static bool sns_dd_gyro_z_polarity_rev;

//Supported gyroscope low pass filter bandwidth settings
static const uint32_t sns_dd_gyro_lpfbw[GYRO_3050_NUM_DLPF_BW] =
{
   GYRO_3050_BITS_DLPF_CFG_5HZ,
   GYRO_3050_BITS_DLPF_CFG_10HZ,
   GYRO_3050_BITS_DLPF_CFG_20HZ,
   GYRO_3050_BITS_DLPF_CFG_42HZ,
   GYRO_3050_BITS_DLPF_CFG_98HZ,
   GYRO_3050_BITS_DLPF_CFG_188HZ,
   GYRO_3050_BITS_DLPF_CFG_256HZ_NOLPF2,
};

#define GYRO3050_SELF_TEST_DEBUG
#ifdef GYRO3050_SELF_TEST_DEBUG
#define GYRO3050_SMGR_DBG_MSG_0( )            \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING0 )
#define GYRO3050_SMGR_DBG_MSG_1( p1 )         \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING1, p1 )
#define GYRO3050_SMGR_DBG_MSG_2( p1, p2 )     \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING2, p1, p2 )
#define GYRO3050_SMGR_DBG_MSG_3( p1, p2, p3 ) \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, p1, p2, p3 )
#else
#define GYRO3050_SMGR_DBG_MSG_0( )
#define GYRO3050_SMGR_DBG_MSG_1( p1 )
#define GYRO3050_SMGR_DBG_MSG_2( p1, p2 )
#define GYRO3050_SMGR_DBG_MSG_3( p1, p2, p3 )
#endif

#define DD_MPU3050_RESET_MAX_DELAY_US  5000
#define DD_MPU3050_RESET_WAIT_ONCE_US  200
#define DD_MPU3050_RESET_WAIT_LOOP_MAX (DD_MPU3050_RESET_MAX_DELAY_US/DD_MPU3050_RESET_WAIT_ONCE_US)
/*=======================================================================
 INTERNAL FUNCTION PROTOTYPES
========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_get_data(
   sns_ddf_handle_t,
   sns_ddf_sensor_e*,
   uint32_t,
   sns_ddf_memhandler_s*,
   sns_ddf_sensor_data_s**);

static sns_ddf_status_e sns_dd_gyro3050_set_attr(
   sns_ddf_handle_t,
   sns_ddf_sensor_e,
   sns_ddf_attribute_e,
   void*);

static void sns_dd_gyro3050_timer_handler(
   sns_ddf_handle_t dd_handle,
   void* arg);

static sns_ddf_status_e sns_dd_gyro3050_self_test(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_test_e       test,
  uint32_t             *err);

static sns_ddf_status_e sns_dd_gyro3050_config_mode(
   sns_dd_gyro3050_state_s* state,
   sns_ddf_powerstate_e mode);

static sns_ddf_status_e sns_dd_gyro3050_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );

/*=======================================================================
                   STRUCTURE DEFINITIONS
========================================================================*/
/* Gyroscope driver API */
sns_ddf_driver_if_s sns_gyro_driver_fn_list =
{
  .init = &sns_dd_gyro3050_init,
  .get_data = &sns_dd_gyro3050_get_data,
  .set_attrib = &sns_dd_gyro3050_set_attr,
  .get_attrib = &sns_dd_gyro3050_get_attr,
  .handle_timer = sns_dd_gyro3050_timer_handler,
  .handle_irq = NULL,
  .reset = &sns_dd_gyro3050_reset,
  .run_test = &sns_dd_gyro3050_self_test,
  .enable_sched_data = NULL,
  .probe = &sns_dd_gyro3050_probe

};


/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_config_mode
===========================================================================*/
/*!
  @brief Sets the operating mode of the gyroscope

  @detail

  @param[in] state: Ptr to the driver structure
  @param[in] mode: Ptr to the variable of sns_ddf_powerstate_e type

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Get away with read modify write
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_config_mode(
   sns_dd_gyro3050_state_s* state,
   sns_ddf_powerstate_e mode)
{
   uint8_t data, out;
   sns_ddf_status_e status;

   status = sns_ddf_read_port(state->port_handle,
                              GYRO_3050_REG_PWR_MGM,
                              &data,
                              1,
                              &out);
   if (status != SNS_DDF_SUCCESS)
      return status;

   switch (mode)
   {
      case SNS_DDF_POWERSTATE_ACTIVE:
         data &= ~(GYRO_3050_BIT_SLEEP);
         break;
      case SNS_DDF_POWERSTATE_LOWPOWER:
         data |= GYRO_3050_BIT_SLEEP;
         break;
      default:
         return SNS_DDF_EINVALID_PARAM;
   }

   status = sns_ddf_write_port(state->port_handle,
                               GYRO_3050_REG_PWR_MGM,
                               &data,
                               1,
                               &out);

   return status;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_config_range_lpfbw
===========================================================================*/
/*!
  @brief Configure the range and low pass filter bandwidth

  @detail

  @param[in] state: Ptr to the driver structure
  @param[in] range: index to range value
  @param[in] lpfbw: index to low pass filter bandwidth value

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_config_range_lpfbw(
   sns_dd_gyro3050_state_s* state,
   uint8_t range,
   uint8_t lpfbw)
{
   uint8_t data, out;
   sns_ddf_status_e status;

   if (range >= GYRO_3050_NUM_FS_RANGES)
      return SNS_DDF_EINVALID_PARAM;

   if (lpfbw >= GYRO_3050_NUM_DLPF_BW)
      return SNS_DDF_EINVALID_PARAM;

   data = sns_dd_gyro_range[range] | sns_dd_gyro_lpfbw[lpfbw];
   status = sns_ddf_write_port(state->port_handle,
                               GYRO_3050_REG_DLPF_FS_SYNC,
                               &data,
                               1,
                               &out);

   if (status == SNS_DDF_SUCCESS)
   {
      state->range = range;
      state->lpf_bw = lpfbw;
   }

   return status;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_det_sf_zpol_corr
===========================================================================*/
/*!
  @brief Determine the scaling factor and z axis polarity
  based on silicon revision and product revision

  @detail
  1) Invensense App Note AN-MPU3000A-18:
     MPU-3000 / MPU-3050 Rev J and K Device Errata
     If the silicon_id is 19, 20 or 22, the corresponding gyro sensitivity
     trim is 115 LSB/dps. Otherwise  gyro sensitivity trim is 131 LSB/dps
  2) Invensense App Note AN-MPU-3000A-02-01
     MPU-3000 Rev J Device Errata
     If revision ID of MPU-3050 is 12 at address Reg 01 [3:0] and product
     revision is 22 at register address 06 [7:2], then append a negative sign
     for all Z-axis raw gyro data received from MPU-3050

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise error code
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_det_sf_zpol_corr(
   sns_ddf_handle_t portHandle)
{
   if (EnableI2C == TRUE)
   {
     uint8_t data, out, siliconRev, scaleFactor, productRev;
     sns_ddf_status_e status;
     int8_t i;

     //program appropriate mask
     data = 0x34;
     status = sns_ddf_write_port(portHandle,
                                 GYRO_3050_REG_BANK_SEL,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //set the read offset to 6
     data = 0x6;
     status = sns_ddf_write_port(portHandle,
                                 GYRO_3050_REG_MEM_START_ADDR,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //read silicon revision
     status = sns_ddf_read_port(portHandle,
                                GYRO_3050_REG_MEM_R_W,
                                &data,
                                1,
                                &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     siliconRev = data >> 2;

     //read product revision
     status = sns_ddf_read_port(portHandle,
                                GYRO_3050_REG_PRODUCT_ID,
                                &data,
                                1,
                                &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     productRev = data & 0x0F;

     //reset the mask
     data = 0;
     status = sns_ddf_write_port(portHandle,
                                 GYRO_3050_REG_BANK_SEL,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //determine scale factor
     if (siliconRev == 19 || siliconRev == 20 || siliconRev == 22)
     {
        scaleFactor = GYRO_3050_SF_CORRECTED;
     }
     else
     {
        scaleFactor = GYRO_3050_SF_NORMAL;
     }

     for (i = GYRO_3050_NUM_FS_RANGES-1; i >= 0; i--)
     {
        sns_dd_gyro_range_scale[i] = sns_dd_gyro_range_sf[scaleFactor][i];
     }

     //determine Z-axis polarity inversion
     if (siliconRev == 22 && productRev == 12)
     {
        sns_dd_gyro_z_polarity_rev = true;
     }
     else
     {
        sns_dd_gyro_z_polarity_rev = false;
     }

     SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_SMGR,
                                 DBG_SMGR_GENERIC_STRING2,
                                 siliconRev,
                                 productRev);
     SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_SMGR,
                                 DBG_SMGR_GENERIC_STRING2,
                                 scaleFactor,
                                 sns_dd_gyro_z_polarity_rev);
   }

   return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_config
===========================================================================*/
/*!
  @brief Sets the device to its default operational state

  @detail

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Check if it the delays are correct and try not use read modify
  write
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_config(
   sns_dd_gyro3050_state_s *state)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if (EnableI2C == TRUE)
  {
     uint8_t data, out;
     uint32_t  reset_wait_loop;

     GYRO3050_SMGR_DBG_MSG_1(500);

     //reset device
     data = GYRO_3050_BIT_H_RESET;
     status = sns_ddf_write_port(state->port_handle,
                                 GYRO_3050_REG_PWR_MGM,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     // Wait for the device to come back up
     for (reset_wait_loop = 0; reset_wait_loop < DD_MPU3050_RESET_WAIT_LOOP_MAX; reset_wait_loop++)
     {
        /* read MPU3050_REG_PWR_MGMT_1 to check if reset have been done */
        status = sns_ddf_read_port(state->port_handle,
                                GYRO_3050_REG_PWR_MGM,
                                &data,
                                1,
                                &out);
        if ((SNS_DDF_SUCCESS == status) && (data & GYRO_3050_BIT_SLEEP))
        {
           break;
        }
        sns_ddf_delay(DD_MPU3050_RESET_WAIT_ONCE_US);
     }

     SNS_PRINTF_STRING_ID_FATAL_3(SNS_DBG_MOD_DSPS_SMGR,
                    DBG_SMGR_GENERIC_STRING3,
                    1090, reset_wait_loop, status);

     //configure device clock source to be PLL with gyro X reference
     data = SDD_GYRO_CLK_SEL_PLLGYROX;
     status = sns_ddf_write_port(state->port_handle,
                                 GYRO_3050_REG_PWR_MGM,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //configure interrupt
     data = (GYRO_3050_BIT_ACTL_LOW |
             GYRO_3050_BIT_OPEN_DRAIN |
             GYRO_3050_BIT_INT_PULSE_WIDTH_50US |
             GYRO_3050_BIT_INT_ANYRD_2CLEAR |
             GYRO_3050_BIT_RAW_RDY_EN);

     status = sns_ddf_write_port(state->port_handle,
                                 GYRO_3050_REG_INT_CFG,
                                 &data,
                                 1,
                                 &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //determine scale factor based on silicon id
     status = sns_dd_gyro3050_det_sf_zpol_corr(state->port_handle);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //configure range and filter bandwidth
     status = sns_dd_gyro3050_config_range_lpfbw(state,
                                                 state->range,
                                                 GYRO_3050_DEF_DLPF_BW_INDEX);
     if (status != SNS_DDF_SUCCESS)
        return status;

     status = sns_dd_gyro3050_config_mode(state, SNS_DDF_POWERSTATE_LOWPOWER);

     GYRO3050_SMGR_DBG_MSG_2(505, status);
   }

   return status;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_reset
===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
    Calls the sns_dd_gyro3050_config_default routine

  @param[in] dd_handle: driver handle

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Check if open and close port operation is required here?
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_gyro3050_reset(
   sns_ddf_handle_t dd_handle)
{
  sns_dd_gyro3050_state_s    *state = (sns_dd_gyro3050_state_s *)dd_handle;

   if (dd_handle == NULL)
      return SNS_DDF_EINVALID_PARAM;

   // Configure device
#ifdef SNS_PCSIM
   return SNS_DDF_SUCCESS;
#else
   return sns_dd_gyro3050_config(state);
#endif
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_set_attr
===========================================================================*/
/*!
  @brief Sets an attribute of the gyroscope

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Curently its the power mode and range.

  @param[in] dd_handle Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib Attribute to be set.
  @param[in] value Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

  $TODO: Do we call the motion detect hreshold change routine with the
  range attribute change?
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_set_attr(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensor,
   sns_ddf_attribute_e attrib,
   void* value)
{
#if defined (SNS_PCSIM)
     return SNS_DDF_SUCCESS;
#endif

   sns_ddf_status_e status = SNS_DDF_SUCCESS;

   if (EnableI2C == TRUE)
   {
     if (dd_handle == NULL || value == NULL)
        return SNS_DDF_EINVALID_PARAM;

#ifdef SNS_PCSIM
   return SNS_DDF_SUCCESS;
#endif

     switch (attrib)
     {
        case SNS_DDF_ATTRIB_POWER_STATE:
           /* set power mode */
           status =
              sns_dd_gyro3050_config_mode((sns_dd_gyro3050_state_s*)dd_handle,
                                          *((sns_ddf_powerstate_e *)value));
           break;


        case SNS_DDF_ATTRIB_RANGE:
           /* set the range */
           if (sensor == SNS_DDF_SENSOR_GYRO)
           {
              status = sns_dd_gyro3050_config_range_lpfbw(
                 (sns_dd_gyro3050_state_s*)dd_handle,
                 *((uint32_t *)value),
                 ((sns_dd_gyro3050_state_s*)dd_handle)->lpf_bw);
           }
           break;

        case SNS_DDF_ATTRIB_LOWPASS:
           /* set the low pass filter bandwidth */
           if (sensor == SNS_DDF_SENSOR_GYRO)
           {
              status = sns_dd_gyro3050_config_range_lpfbw(
                 (sns_dd_gyro3050_state_s*)dd_handle,
                 ((sns_dd_gyro3050_state_s*)dd_handle)->range,
                 *((uint32_t *)value));
           }
           break;

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
           /* MPU3050 gyro supports single resolution, hence not settable
              Return error */
        default:
           return SNS_DDF_EINVALID_PARAM;
     }
   }

   return status;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_get_data_from_device
===========================================================================*/
/*!
  @brief Gets the gyroscope and temperature data from device

  @detail
  - read the gyroscope and temperature data registers and scales the values
    and stores them in the driver state structure in Q16 format

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_get_data_from_device(
   sns_dd_gyro3050_state_s *state)
{
#ifdef SNS_PCSIM
   return SNS_DDF_SUCCESS;
#endif

   if (EnableI2C == TRUE)
   {
     uint8_t i;
     sns_ddf_status_e status;
     uint8_t data[8], out;
     int16_t devData[SDD_GYRO_NUM_DATATYPES];
     /* Read the data */
     status = sns_ddf_read_port(state->port_handle,
                                GYRO_3050_REG_TEMP_OUT_H,
                                data,
                                8,
                                &out);
     if (status != SNS_DDF_SUCCESS)
        return status;

     //compute device data
     devData[SDD_GYRO_X] = (int16_t)(((int16_t)data[2] << 8) | data[3]);
     devData[SDD_GYRO_Y] = (int16_t)(((int16_t)data[4] << 8) | data[5]);
     devData[SDD_GYRO_Z] = (int16_t)(((int16_t)data[6] << 8) | data[7]);

     if (sns_dd_gyro_z_polarity_rev)
     {
        devData[SDD_GYRO_Z] = -devData[SDD_GYRO_Z];
     }

     devData[SDD_TEMP] = (int16_t)(((int16_t)data[0] << 8) | data[1]);

     //check for 16 bit signed data out of bounds
     for (i=0; i < SDD_GYRO_NUM_DATATYPES; i++)
     {
        if (devData[i] == ((GYRO_3050_MIN_RAW_DATA_VALUE)) ||
            devData[i] == GYRO_3050_MAX_RAW_DATA_VALUE)
        {
           state->data_status[i] = SNS_DDF_EDATA_BOUND;
        }
        else
        {
           state->data_status[i] = SNS_DDF_SUCCESS;
        }
     }

     //report gyro data in radians/sec (Q16 format)
     state->data_cache[SDD_GYRO_X] =
        ((devData[SDD_GYRO_X] * sns_dd_gyro_range_scale[state->range]) >> 9);
     state->data_cache[SDD_GYRO_Y] =
        ((devData[SDD_GYRO_Y] * sns_dd_gyro_range_scale[state->range]) >> 9);
     state->data_cache[SDD_GYRO_Z] =
        ((devData[SDD_GYRO_Z] * sns_dd_gyro_range_scale[state->range]) >> 9);

     // Map the data to the phone's coordinates system.
     sns_ddf_map_axes(&state->axes_map, &state->data_cache[SDD_GYRO_X]);

     //report temperature data in degC (Q16 format)
     state->data_cache[SDD_TEMP] =
        ((int32_t)(devData[SDD_TEMP]) - GYRO_3050_TEMP_OFFSET_LSB) *
        GYRO_3050_TEMP_SCALE + GYRO_3050_TEMP_OFFSET_CELSIUS;

     for (i=0; i < SDD_GYRO_NUM_DATATYPES; i++)
     {
        SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_SMGR,
                                    DBG_SMGR_GENERIC_STRING2,
                                    state->data_cache[i],
                                    state->data_status[i]);
     }
   }

   return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_log_data
===========================================================================*/
/*!
  @brief log sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] dd_handle    Handle to the driver instance
  @param[in] sample_time  Time that the sensor was sampled

  @return
  None

  $TODO:
*/
/*=========================================================================*/
void sns_dd_gyro3050_log_data(
   sns_ddf_handle_t dd_handle,
   sns_ddf_time_t sample_time)
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   DDF_MSG_0(HIGH, "MPU3050 Log Data");

   //Allocate log packet
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
      log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
      log_struct_ptr->sensor_id = SNS_DDF_SENSOR_GYRO;
      log_struct_ptr->vendor_id = SNS_DDF_VENDOR_INVENSENSE;

      //Timestamp the log with sample time
      log_struct_ptr->timestamp = sample_time;

      //Log the sensor data
      log_struct_ptr->data[SDD_GYRO_X] =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_X];
      log_struct_ptr->data[SDD_GYRO_Y] =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_Y];
      log_struct_ptr->data[SDD_GYRO_Z] =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_Z];
      log_struct_ptr->data[SDD_TEMP] =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_TEMP];

      log_struct_ptr->num_data_types = SDD_GYRO_NUM_DATATYPES;

      //Commit log (also frees up the log packet memory)
      err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                   log_struct_ptr);
   }
   else
   {
      DDF_MSG_1(HIGH, "MPU3050 Log Data - logpkt_malloc failed with err: %d", err_code);
   }
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_get_data
===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Data is returned immediately after being read from the
    sensor, in which case data[] is populated in the same order it was
    requested,

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[out] buffer       Destination buffer where data will be
                           deposited.
  @param[out] buffer_len   Length of @a buffer.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_get_data(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensors[],
   uint32_t num_sensors,
   sns_ddf_memhandler_s* memhandler,
   sns_ddf_sensor_data_s* data[])
{
   uint8_t i;
   sns_ddf_status_e status;
   sns_ddf_sensor_data_s* data_ptr;
   sns_ddf_time_t sample_time;

   if (dd_handle == NULL || sensors == NULL ||
       memhandler == NULL || data == NULL)
      return SNS_DDF_EINVALID_PARAM;

   /* Sanity check*/
   for (i = 0; i < num_sensors; i++)
   {
      if (sensors[i] != SNS_DDF_SENSOR_GYRO &&
          sensors[i] != SNS_DDF_SENSOR_TEMP)
         return SNS_DDF_EINVALID_PARAM;
   }

   /* read data from device */
   status = sns_dd_gyro3050_get_data_from_device(
      (sns_dd_gyro3050_state_s*)dd_handle);
   if (status != SNS_DDF_SUCCESS)
      return status;

   sample_time = sns_ddf_get_timestamp();

   data_ptr =
      sns_ddf_memhandler_malloc(memhandler,
                                num_sensors * sizeof(sns_ddf_sensor_data_s));
   if (data_ptr == NULL)
      return SNS_DDF_ENOMEM;

   *data = data_ptr;

   for (i = 0; i < num_sensors; i++)
   {
      data_ptr[i].sensor = sensors[i];
      data_ptr[i].timestamp = sample_time;

      if (sensors[i] == SNS_DDF_SENSOR_GYRO)
      {
         data_ptr[i].samples = sns_ddf_memhandler_malloc(
            memhandler,
            SDD_GYRO_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
         if (data_ptr[i].samples == NULL)
            return SNS_DDF_ENOMEM;

#ifdef SNS_PCSIM
         if (sns_playback_gyro_data(&data_ptr[i]) != SNS_DDF_SUCCESS)
         {
            return SNS_DDF_EBUS;
         }
#else
         if (EnableI2C != TRUE)
         {
           static uint32 gyroDummyData = 1001;
           data_ptr[i].samples[0].sample = gyroDummyData++;
           data_ptr[i].samples[1].sample = gyroDummyData++;
           data_ptr[i].samples[2].sample = gyroDummyData++;
           data_ptr->num_samples = 3;
         }
         else
         {
           data_ptr[i].samples[SDD_GYRO_X].sample =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_X];
           data_ptr[i].samples[SDD_GYRO_X].status =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_status[SDD_GYRO_X];

           data_ptr[i].samples[SDD_GYRO_Y].sample =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_Y];
           data_ptr[i].samples[SDD_GYRO_Y].status =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_status[SDD_GYRO_Y];

           data_ptr[i].samples[SDD_GYRO_Z].sample =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_GYRO_Z];
           data_ptr[i].samples[SDD_GYRO_Z].status =
           ((sns_dd_gyro3050_state_s*)dd_handle)->data_status[SDD_GYRO_Z];

           data_ptr[i].num_samples = SDD_GYRO_NUM_AXES;
         }
#endif
      }
      else //temperature sensor
      {
         data_ptr[i].samples = sns_ddf_memhandler_malloc(
            memhandler,
            sizeof(sns_ddf_sensor_sample_s));
         if (data_ptr[i].samples == NULL)
            return SNS_DDF_ENOMEM;

         data_ptr[i].samples[0].sample =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_cache[SDD_TEMP];
         data_ptr[i].samples[0].status =
         ((sns_dd_gyro3050_state_s*)dd_handle)->data_status[SDD_TEMP];
         data_ptr[i].num_samples = 1;
      }
      data_ptr[i].status = SNS_DDF_SUCCESS;
   }

   sns_dd_gyro3050_log_data(dd_handle, sample_time);

   return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_timer_handler
===========================================================================*/
/*!
  @brief timer handler

  @detail

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static void sns_dd_gyro3050_timer_handler(
   sns_ddf_handle_t dd_handle,
   void* arg)
{
   if (dd_handle == NULL)
      return;

   //sns_ddf_smgr_notify_timer(arg);

}

static void sns_dd_gyro3050_test_notify(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_status_e  status,
    uint32_t          err,
    q16_t*            biases)
{
    if (EnableI2C == TRUE)
    {
      static const uint8_t num_axes = SDD_GYRO_NUM_AXES;
      sns_dd_gyro3050_state_s    *state = (sns_dd_gyro3050_state_s *)handle;
      q16_t* biases_ptr;
      uint8_t i;

      sns_ddf_smgr_notify_test_complete(state->smgr_hndl, sensor, status, err);

      if ( status == SNS_DDF_SUCCESS )
      {
          biases_ptr = state->biases;

          for(i = 0; i < num_axes; i++)
              biases_ptr[i] = biases[i];

          sns_ddf_smgr_notify_event(state->smgr_hndl, sensor, SNS_DDF_EVENT_BIAS_READY);
      }
    }
}

static sns_ddf_status_e sns_dd_gyro3050_self_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err)
{
    sns_ddf_status_e status;
    sns_dd_inertial_test_config_s test_config;
    sns_dd_gyro3050_state_s    *state = (sns_dd_gyro3050_state_s *)dd_handle;

    if (test != SNS_DDF_TEST_SELF)
    {
       return SNS_DDF_EINVALID_TEST;
    }

    if (sensor != SNS_DDF_SENSOR_GYRO)
    {
       return SNS_DDF_EINVALID_PARAM;
    }

    test_config.sample_rate = 20;
    test_config.num_samples = 64;

    test_config.max_variance = (int64_t)(FX_CONV(FX_FLTTOFIX_Q16(0.00015),16,32));
    test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.8);  // x-axis bias
    test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.8);  // y-axis bias
    test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.8);  // z-axis bias

    GYRO3050_SMGR_DBG_MSG_2(state->range, state->lpf_bw);

    status = sns_dd_gyro3050_reset(dd_handle);
    GYRO3050_SMGR_DBG_MSG_1(status);
    if(status == SNS_DDF_EBUS)
    {
       *err = SNS_DD_ITEST_I2C_ERR;
       return SNS_DDF_EFAIL;
    }


    sns_dd_gyro3050_config_mode(state, SNS_DDF_POWERSTATE_ACTIVE);
    sns_dd_gyro3050_config_range_lpfbw(state, 2, 4);

    /* need to wait idle to ready time */
    sns_ddf_delay(GYRO_3050_IDLE_TO_READY_TIME);

    status = sns_dd_inertial_test_run(
        sensor,
        dd_handle,
        &sns_gyro_driver_fn_list,
        &test_config,
        &sns_dd_gyro3050_test_notify);

    if(status != SNS_DDF_SUCCESS)
        return status;

    return SNS_DDF_PENDING;
}

/*=============================================================================
 * Probe function for GYRO 3050 sensor.
 =============================================================================*/
static sns_ddf_status_e sns_dd_gyro3050_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors )
{
  /*
   * Initialize data before we begin probing for device existance.
   */
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  status = SNS_DDF_SUCCESS;
  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              GYRO_3050_REG_I2C_ADDR,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if((status != SNS_DDF_SUCCESS) || (bytes_read != 1))
  {
    sns_ddf_close_port( port_handle );
    return status;
  }

  /* this device is not the MPU 3000 or the MPU 3050*/
  if((i2c_buff != 0x68) && (i2c_buff != 0x69))
  {
    sns_ddf_close_port( port_handle );
    return status;
  }

  /* This probe is successful */

  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] =  SNS_DDF_SENSOR_GYRO;
    (*sensors)[1] = SNS_DDF_SENSOR_TEMP;

    status = SNS_DDF_SUCCESS;
  }
  else
  {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;

} /* End sns_dd_gyro3050_probe */

