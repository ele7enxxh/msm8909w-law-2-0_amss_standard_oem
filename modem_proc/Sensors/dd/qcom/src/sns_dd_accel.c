/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the accelerometer driver

Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/29/12     ag      Updated dummy data
06/08/12     vh      Added run time switch for I2C code
05/17/12     vh      Enabled I2C functionality
08/28/11     ag      Added place holder for self test ptr
08/22/11     sc      Removed a line of un-used code that has reference to adxl350.
08/11/11     br      Changed the unit of LOWPASS to q16_t
04/08/10     RY      Added axis conversion for FLUID and FFA/SURF.
06/25/10     jww     Ported from Sensors Framework for AMSS
08/24/10     RK      Ported to latest DDF. Added open port as part of init.
                     Updated all the function comments to doxygen style
08/30/10     RK      Added support for the lowpass filter freq get and set attribute
                     options and made the changes to accept index as an input
                     to the set attribute routine rather than the actual attribute
                     value
10/06/10     RK      Updated the accel driver to work with the new DDF interface
                     (time delay is still with busywait in this version)
10/06/10     RK      Added the SNS_DDF_ATTRIB_DELAYS attrib and corrected the
                     device info attrib
10/07/10     RK      Fixed the range config bug(BW bitmap instead of BW value)
10/09/10     RK      Currently updated each sample status SUCCESS to be changed later
10/12/10     RK      Changed the temperature calculation to return in Q16 degrees
                     units to SMGR
10/19/10     sj      Support for accel log packets
10/21/10     RK      Removed dropped_logs variable, flipped X & Y axis & updated
                     support for log packet
10/25/10     RK      Added the out of bound check and also flipped the
                     sign of the Y-axis
10/26/10     RK      Fixed compiler warnings
01/11/11     yk      Added support for new attrib SNS_DDF_ATTRIB_RESOLUTION_ADC
                     and redefined SNS_DDF_ATTRIB_RESOLUTION.
01/20/11     br      Updated lines for the resolution attributes
01/21/11     sc      Added support for PCSIM playback
02/03/11     br      Modified the line which calculate resolution
02/22/11     KY      Added calibration support
==============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_dd_accel_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd.h"

//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#if defined SNS_PCSIM
#include "sns_playback.h"
#endif

#ifdef CONFIG_USE_BMA150_CALIB
#include "sns_dd_accel_calib.h"
#endif

/*=======================================================================

                  Extern Variables

========================================================================*/
extern volatile boolean EnableI2C;

/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/

static const uint32_t  sns_dd_accel_range[3] =
{
  BOSCH_ACCEL_SENSOR_RANGE_2G,
  BOSCH_ACCEL_SENSOR_RANGE_4G,
  BOSCH_ACCEL_SENSOR_RANGE_8G
};

static const uint32_t  sns_dd_accel_bw[7] =
{
  BOSCH_ACCEL_SENSOR_BW_BITMAP_25Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_50Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_100Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_190Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_375Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_750Hz,
  BOSCH_ACCEL_SENSOR_BW_BITMAP_1500Hz
};

static const q16_t sns_dd_accel_freq[7] =
{
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_25Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_50Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_100Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_190Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_375Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_750Hz),
  FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_BW_VALUE_1500Hz),
};

/*=======================================================================

                   INTERNAL ENUMS

========================================================================*/
/* Supported datatypes */
typedef enum
{
   SDD_ACC_X = 0,
   SDD_ACC_Y,
   SDD_ACC_Z,
   SDD_ACC_NUM_AXES,
   SDD_TEMP = SDD_ACC_NUM_AXES,
   SDD_ACC_NUM_DATATYPES
} sdd_acc_data_type_e;


/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_acc_bma150_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_acc_bma150_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_acc_bma150_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static void sns_dd_acc_bma150_interrupt_handler
(sns_ddf_handle_t dd_handle, sns_ddf_irq_e irq);
static sns_ddf_status_e sns_dd_acc_bma150_reset(sns_ddf_handle_t dd_handle);

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_accel_driver_fn_list =
{
  &sns_dd_acc_bma150_init,
  &sns_dd_acc_bma150_get_data,
  &sns_dd_acc_bma150_set_attr,
  &sns_dd_acc_bma150_get_attr,
  NULL,                                 //R.K. Probably replace this with dummy function
  &sns_dd_acc_bma150_interrupt_handler,
  &sns_dd_acc_bma150_reset,
  NULL /* self test */
};

/* State struct for BMA150 driver */
typedef struct {
   sns_ddf_handle_t smgr_hndl;    /* SDDI handle used to notify_data */
   q16_t            bias[SDD_ACC_NUM_AXES];    /* Accelerometer bias */
   q16_t            data_cache[SDD_ACC_NUM_DATATYPES]; /* data cache */
   uint8_t          out_of_bound_flag;
   sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
   uint8_t          range;       /* current range selection */
   uint16_t         lowpass_bw;   /* current lowpass filter BW */
   bool             motion_detect_enabled;
} sns_dd_acc_bma150_state_t;


static sns_ddf_status_e sns_dd_acc_bma150_config_motion_int(
   sns_dd_acc_bma150_state_t  *state,
   bool                       enable);

static sns_ddf_status_e sns_dd_acc_bma150_config_motion_detect_thresh(
    sns_dd_acc_bma150_state_t  *state);


/*============================================================================

  Global Data Definitions

 ============================================================================*/

static uint8_t sns_dd_bosch_accel_ctrl_reg_val;
static sns_ddf_sensor_e my_sensors[] =
  {
    SNS_DDF_SENSOR_ACCEL,
    SNS_DDF_SENSOR_TEMP
  };


/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_config_range

===========================================================================*/
/*!
  @brief Sets the BOSCH acclerometer to the passed in range

  @detail
  - The range information is taken from the driver structure whcih is
    set to BOSCH_ACCEL_SENSOR_RANGE_2G as part of the reset function
    before calling this one.
    This routine also sets the BW to 25hz

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_config_range
(
  sns_dd_acc_bma150_state_t *state
)
{
  uint8_t bosch_write_buffer, out;
  sns_ddf_status_e stat;

  //Can't do the Read modify write as the upper bits 5,6&7 (reserved bits)
  //contain some calibration data and that needs copied untouched for
  //every write into this register

  if ( (stat = sns_ddf_read_port(state->port_handle,
                              BOSCH_ACCEL_SENSOR_CTRL2_REG_ADDR,
                              &bosch_write_buffer,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }

  bosch_write_buffer &= BOSCH_ACCEL_CTRL2_RESERVED_BITS_MASK;

  /* set range and bandwidth */
  bosch_write_buffer = bosch_write_buffer | state->range | state->lowpass_bw;
  if ( (stat = sns_ddf_write_port(state->port_handle,
                         BOSCH_ACCEL_SENSOR_CTRL2_REG_ADDR,
                         &bosch_write_buffer,
                         1,
                         &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }


  if(state->motion_detect_enabled)
  {
      // Update the motion detect threshold to reflect the new range.
      return sns_dd_acc_bma150_config_motion_detect_thresh(state);
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_config_default

===========================================================================*/
/*!
  @brief Sets the BOSCH acclerometer to its default operational state

  @detail
  - Programs the register BOSCH_ACCEL_SENSOR_CTRL5_REG_ADDR for the MD
  threshold

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_bma150_config_default
(
  sns_dd_acc_bma150_state_t *state
)
{
  if (EnableI2C == TRUE)
  {
    uint8_t  bosch_write_buffer, out;
    uint8_t  rec_gain;
    uint16_t temp_smb380_cal;
    sns_ddf_status_e stat;

    /* Code for SMB380/BMA150 Tri-Axial Accelerometer and temp sensor */

    /* Contrary to spec, the Bosch SMB380 seems to be asleep after reset */
    /* take out of sleep */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_NORMAL_MODE;
    if( (stat = sns_ddf_write_port(state->port_handle,
                                BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                                &bosch_write_buffer,
                                1,
                                &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    // storing the last write done to CTRL3 REG in sns_dd_bosch_accel_ctrl_reg_val
    sns_dd_bosch_accel_ctrl_reg_val = bosch_write_buffer;

    /* soft reset */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_NORMAL_MODE | BOSCH_ACCEL_SENSOR_SOFTRESET;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    // storing the last write done to CTRL3 REG in sns_dd_bosch_accel_ctrl_reg_val
    sns_dd_bosch_accel_ctrl_reg_val = bosch_write_buffer;

    /* Changed the original delay from 10ms to 1msec as per the spec this delay
    is req only if there is an EEPROM write */
    sns_ddf_delay(1000);

    /* remove soft reset */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_NORMAL_MODE;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    // storing the last write done to CTRL3 REG in sns_dd_bosch_accel_ctrl_reg_val
    sns_dd_bosch_accel_ctrl_reg_val = bosch_write_buffer;

    /* set the range */
    if ( (stat = sns_dd_acc_bma150_config_range(state)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    /***************************************************************************/
    /* set any motion threshold to be set only when motion detect is to be used*/
    //R.K in future need to set sleep wake delays along with this
    /*
    if ( (stat = sns_dd_acc_bma150_config_motion_detect_thresh(state))
              != SNS_DDF_SUCCESS)
    {
       return stat;
    }
    */

    /* set any motion duration, clear HG and LG hysterisis */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_ANY_MOTION_DUR_3_SEQ;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL6_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
       return stat;
    }


    /* enable any motion, disable HG, LG and alert modes */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_ANY_MOTION;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL4_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
       return stat;
    }

    /* configure advanced interrupt as latched, disable advanced interrupt, disable wakeup */
    bosch_write_buffer = BOSCH_ACCEL_SENSOR_LATCH_INT;

    /* set wakeup pause period */
    // R.K. probably dictated by SMGR input in future
    bosch_write_buffer |= BOSCH_ACCEL_SENSOR_WAKEUP_PAUSE_80mS;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL1_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    /* use prefixed cal values for now */

    // R.K. Once confirmed thse can be defined as constants
    /* stick some "reasonable" calibration constants into the SMB380/BMA150 */
    /* so we can get a beginning temperature value out of it. */
    temp_smb380_cal = 0xcc;    /* recommended "starter" cal */
    rec_gain        = 0x1b;    /* recommended gain */

    /* write the temp calibration data                                     */
    /* MSB */
    bosch_write_buffer = (temp_smb380_cal & BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_MASK) >>
                         BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_R_SHIFT;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    /* LSB with new calibration data, maintaining the recommended gain value */
    bosch_write_buffer = ((temp_smb380_cal & BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_MASK) <<
                          BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_L_SHIFT) | rec_gain;
    if ( (stat = sns_ddf_write_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
    {
      return stat;
    }

    /* let calibration values settle in by waiting 1 ms*/
    sns_ddf_delay(1000);
  }

  return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  - resets the bias to zero & range to BOSCH_ACCEL_SENSOR_RANGE_2G which
    is stored in the driver state structure
    Calls the sns_dd_acc_bma150_config_default routine

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Check if open and close port operation is required here?
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_reset(sns_ddf_handle_t dd_handle)
{
  uint8_t i;
  sns_dd_acc_bma150_state_t *state = (sns_dd_acc_bma150_state_t *)dd_handle;

  for (i = 0; i < SDD_ACC_NUM_AXES; i++)
  {
    state->bias[i] = 0;
  }
  state->range = BOSCH_ACCEL_SENSOR_RANGE_2G;
  state->lowpass_bw = BOSCH_ACCEL_SENSOR_BW_BITMAP_25Hz;
  state->out_of_bound_flag = 0;
  state->motion_detect_enabled = false;

  /* R.K  Do we need to close and open the port here? If so this can't be
  called from init function (not adding this for CC2) */

  // Configure BMA150
#if defined (SNS_PCSIM)
  return SNS_DDF_SUCCESS;
#else
  if (EnableI2C == FALSE)
    return SNS_DDF_SUCCESS;
  else
    return sns_dd_acc_bma150_config_default(state);
#endif
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_init

===========================================================================*/
/*!
  @brief Initializes the 3 axis Bosch accelerometer BMA150
              determines the device to use

  @detail
  - Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_acc_bma150_reset routine

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
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_init(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{
  sns_ddf_status_e stat;
  sns_dd_acc_bma150_state_t *ptr;

  if(sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_acc_bma150_state_t)) != SNS_DDF_SUCCESS)
  {
    return SNS_DDF_ENOMEM;
  }
  ptr->smgr_hndl = smgr_handle;
  ptr->motion_detect_enabled = false;

  if((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    return stat;
  }

  /* Fill out supported sensor info */
  *num_sensors = 2;
  *sensors = my_sensors;
  *dd_handle_ptr = (sns_ddf_handle_t)ptr;

  // Resets the bma150
  stat = sns_dd_acc_bma150_reset(ptr);

  return stat;
}
/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_config_mode

===========================================================================*/
/*!
  @brief Sets the BOSCH acclerometer to the passed in operating mode

  @detail
  - Set the operational mode (sleep or NORMAL_MODE) by programming
  BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR register

  @param[in] state: Ptr to the driver structure
  @param[in] mode: Ptr to the variable of sns_ddf_powerstate_e type

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Get away with read modify write
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_config_mode
(
   sns_dd_acc_bma150_state_t *state,
   sns_ddf_powerstate_e      *mode
)
{
   sns_ddf_status_e status;
   uint8_t bosch_write_buffer, out;

   /* Perform read, modify & write to update mode */
   //R.k. store in local memory what was commanded last or last contents of
   // this register rather than doing an I2c read again
  /* if ( (status = sns_ddf_read_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) != SNS_DDF_SUCCESS)
   {
     return status;
   } */

   /* clear opmode bit B0 */
   bosch_write_buffer = sns_dd_bosch_accel_ctrl_reg_val & 0xE;

   switch (*mode)
   {
   case SNS_DDF_POWERSTATE_ACTIVE:
      /* Reset mode bit */
      bosch_write_buffer &= ~(BOSCH_ACCEL_SENSOR_SLEEP_MODE);
      break;
   case SNS_DDF_POWERSTATE_LOWPOWER:
      /* Set mode bit */
      bosch_write_buffer |= BOSCH_ACCEL_SENSOR_SLEEP_MODE;
      break;
   default:
      return SNS_DDF_EINVALID_PARAM;

   }

   /* Set opmode */
   if ( (status = sns_ddf_write_port(state->port_handle,
                          BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                          &bosch_write_buffer,
                          1,
                          &out)) != SNS_DDF_SUCCESS)
   {
     return status;
   }
   // storing the last write done to CTRL3 REG in sns_dd_bosch_accel_ctrl_reg_val
   sns_dd_bosch_accel_ctrl_reg_val = bosch_write_buffer;

   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the bosch accelerometer

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

  $TODO: Do we call the motion detect hreshold change routine with the
  range attribute change?
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_set_attr
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SMGR dd init : switch(%d)",attrib);
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
       /* set power mode */
       ret_val = sns_dd_acc_bma150_config_mode((sns_dd_acc_bma150_state_t*)dd_handle,
                                           (sns_ddf_powerstate_e *)value);
    break;

    case SNS_DDF_ATTRIB_RANGE:
      /* change the range for accel */
      if (sensor == SNS_DDF_SENSOR_ACCEL)
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SMGR dd init : SNS_DDF_SENSOR_ACCEL");
        if(*((uint32_t *)value) > SNS_DD_ACCEL_MAX_RANGE)
        {
          return SNS_DDF_EINVALID_ATTR;
        }
        else
        {
          ((sns_dd_acc_bma150_state_t*)dd_handle)->range = sns_dd_accel_range[*((uint32_t *)value)];
        }
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SMGR dd init : ret_val");
        ret_val = sns_dd_acc_bma150_config_range((sns_dd_acc_bma150_state_t*)dd_handle);
      }
    break;

    case SNS_DDF_ATTRIB_LOWPASS:
      /* change the range for accel */
      if (sensor == SNS_DDF_SENSOR_ACCEL)
      {
        if(*((uint32_t *)value) > BOSCH_ACCEL_SENSOR_BW_BITMAP_1500Hz)
        {
          return SNS_DDF_EINVALID_ATTR;
        }
        else
        {
          ((sns_dd_acc_bma150_state_t*)dd_handle)->lowpass_bw = sns_dd_accel_bw[*((uint32_t *)value)];
        }

        ret_val = sns_dd_acc_bma150_config_range((sns_dd_acc_bma150_state_t*)dd_handle);
      }

    break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      /* BMA150 only supports 1 resolution level for both accel and temp */
    break;

    case SNS_DDF_ATTRIB_MOTION_DETECT:
    {
        bool* enable = value;

        if(!sns_ddf_signal_irq_enabled())
        {
            // We're running on a platform that doesn't support interrupts, so
            // this attribute is invalid.
            return SNS_DDF_EINVALID_PARAM;
        }

        if(*enable)
        {
            sns_dd_acc_bma150_config_motion_detect_thresh(dd_handle);
            sns_dd_acc_bma150_config_motion_int(dd_handle, true);

            // Register to receive notification when the interrupt fires.
            return sns_ddf_signal_register(
                SNS_DDF_SIGNAL_IRQ_MOTION_DETECT,
                SNS_DDF_SIGNAL_IRQ_HIGH,
                sns_dd_acc_bma150_interrupt_handler,
                dd_handle);
        }
        else
        {
            sns_dd_acc_bma150_config_motion_int(dd_handle, false);
            return sns_ddf_signal_deregister(SNS_DDF_SIGNAL_IRQ_MOTION_DETECT);
        }
    }

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  return(ret_val);
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bosch_get_data_all

===========================================================================*/
/*!
  @brief Gets the acceleration data from the bosch accelerometer as well
  as the temperature

  @detail
  - read the accel data and temp data registers and scales the values
    and stores them in the driver state structure in Q16 format

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_get_data_all
(
  sns_dd_acc_bma150_state_t *state
)
{
#if defined (SNS_PCSIM)
    return SNS_DDF_SUCCESS;
#endif

  if (EnableI2C == TRUE)
  {
    uint8_t bosch_read_data[7], out;
    int32_t accel_32bit;
    sns_ddf_status_e stat;

    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sns_dd_acc_bma150_get_data_all");

    /* Reseting the out of bound flag which would be set in this routine if
       if the raw data is out of bound of bound.
    */
    state->out_of_bound_flag = 0;

    /* Read the data */
    if ( (stat = sns_ddf_read_port(state->port_handle,
                                BOSCH_ACCEL_SENSOR_X_LSB_REG_ADDR,
                                bosch_read_data,
                                7,
                                &out)) != SNS_DDF_SUCCESS)
    {
       return stat;
    }

    /* X-axis */
    /* lsb=bosch_read_data[0], msb=bosch_read_data[1] */
    /* retrieve the data as a 10 bit value and perform sign extension */
    accel_32bit = (((int32_t)bosch_read_data[1]) << 24) >> (24 - BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT) |
                  (((int32_t)bosch_read_data[0]) >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT);

    if((accel_32bit == BOSCH_ACCEL_MAX_RAW_DATA_VALUE)||
       (accel_32bit == BOSCH_ACCEL_MIN_RAW_DATA_VALUE))
    {
      state->out_of_bound_flag |= (0x1<<SDD_ACC_X);
    }
    /* Convert to m/s^2 */
    /* The conversion formula is:
         (accel_32bit*BOSCH_ACCELEROMETER_FULL_SCALE_G*G)/MAX_COUNT_10BIT_SIGNED
       for 2G range:
         accel_32bit*2*9.8/511
       for 4G range:
         accel_32bit*4*9.8/511
       for 8G range:
         accel_32bit*8*9.8/511

       Since (2*9.8/511) will be precomputed by compiler, it will be a constant
     */

    switch (state->range)
    {
       case BOSCH_ACCEL_SENSOR_RANGE_2G:
          state->data_cache[SDD_ACC_X] = accel_32bit * FX_FLTTOFIX((2*G/511), 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_4G:
          state->data_cache[SDD_ACC_X] = accel_32bit * FX_FLTTOFIX(4*G/511, 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_8G:
          state->data_cache[SDD_ACC_X] = accel_32bit * FX_FLTTOFIX(8*G/511, 16);
          break;
       default:
          state->data_cache[SDD_ACC_X] = accel_32bit * FX_FLTTOFIX(2*G/511, 16);
          break;
    }


    /* Y-axis */
    /* lsb=bosch_read_data[2], msb=bosch_read_data[3] */
    /* retrieve the data as a 10 bit value and perform sign extension */
    accel_32bit = (((int32_t)bosch_read_data[3]) << 24) >> (24 - BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT) |
                  (((int32_t)bosch_read_data[2]) >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT);
    if((accel_32bit == BOSCH_ACCEL_MAX_RAW_DATA_VALUE)||
       (accel_32bit == BOSCH_ACCEL_MIN_RAW_DATA_VALUE))
    {
      state->out_of_bound_flag |= (0x1<<SDD_ACC_Y);
    }

    /* Convert to m/s^2 */
    /* Magic numbers explained above */

    switch (state->range)
    {
       case BOSCH_ACCEL_SENSOR_RANGE_2G:
          state->data_cache[SDD_ACC_Y] = accel_32bit * FX_FLTTOFIX(2*G/511, 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_4G:
          state->data_cache[SDD_ACC_Y] = accel_32bit * FX_FLTTOFIX(4*G/511, 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_8G:
          state->data_cache[SDD_ACC_Y] = accel_32bit * FX_FLTTOFIX(8*G/511, 16);
          break;
       default:
          state->data_cache[SDD_ACC_Y] = accel_32bit * FX_FLTTOFIX(2*G/511, 16);
          break;
    }

    /* Z-axis */
    /* lsb=bosch_read_data[4], msb=bosch_read_data[5] */
    /* retrieve the data as a 10 bit value and perform sign extension */
    accel_32bit = (((int32_t)bosch_read_data[5]) << 24) >> (24 - BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT) |
                  (((int32_t)bosch_read_data[4]) >> BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT);

    if((accel_32bit == BOSCH_ACCEL_MAX_RAW_DATA_VALUE)||
       (accel_32bit == BOSCH_ACCEL_MIN_RAW_DATA_VALUE))
    {
      state->out_of_bound_flag |= (0x1<<SDD_ACC_Z);
    }

    /* Convert to m/s^2 */
    /* Magic numbers explained above */

    switch (state->range)
    {
       case BOSCH_ACCEL_SENSOR_RANGE_2G:
          state->data_cache[SDD_ACC_Z] = accel_32bit * FX_FLTTOFIX(2*G/511, 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_4G:
          state->data_cache[SDD_ACC_Z] = accel_32bit * FX_FLTTOFIX(4*G/511, 16);
          break;
       case BOSCH_ACCEL_SENSOR_RANGE_8G:
          state->data_cache[SDD_ACC_Z] = accel_32bit * FX_FLTTOFIX(8*G/511, 16);
          break;
       default:
          state->data_cache[SDD_ACC_Z] = accel_32bit * FX_FLTTOFIX(2*G/511, 16);
          break;
    }


    /* convert read data to 1/10 degree */
    /* factored like this to avoid floating point operations */
    state->data_cache[SDD_TEMP] = (int32_t) ((bosch_read_data[6]<<15) -
                                             (BOSCH_ACCEL_SENSOR_TEMP_OFFSET_DEG<<16));
    if((accel_32bit == BOSCH_TEMP_MAX_RAW_DATA_VALUE)||
       (accel_32bit == BOSCH_TEMP_MIN_RAW_DATA_VALUE))
    {
      state->out_of_bound_flag |= (0x1<<SDD_TEMP);
    }
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_get_data_from_device

===========================================================================*/
/*!
  @brief Apply bias to the accel data scaled by the range from the
  device.

  @detail
  - Calls the sns_dd_acc_bma150_get_data_all to fetch the scaled 3 axis
    and temp data from the device and apply bias to it.

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Probably change the name of this function to reflect what it is
  or even get away with this function and do the bias in the
  sns_dd_acc_bma150_get_data_all routine
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_get_data_from_device
(
  sns_dd_acc_bma150_state_t *state
)
{
  uint8_t i;
  sns_ddf_status_e stat;

  if ( (stat = sns_dd_acc_bma150_get_data_all(state)) == SNS_DDF_SUCCESS)
  {
     /* Bias correction */
     for (i=0; i<SDD_ACC_NUM_AXES; i++)
     {
        state->data_cache[i] -= state->bias[i];
     }
  }

  return stat;
}


/*===========================================================================
  FUNCTION:   sns_dd_accel_log_data
===========================================================================*/
/*!
  @brief log sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None

  $TODO:
*/
/*=========================================================================*/
void sns_dd_accel_log_data(
   sns_dd_acc_bma150_state_t *state,
   sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = SDD_ACC_NUM_DATATYPES;
    log_struct_ptr->data[0]  = state->data_cache[SDD_ACC_Y];
    log_struct_ptr->data[1] = state->data_cache[SDD_ACC_X];
    log_struct_ptr->data[2]  = state->data_cache[SDD_ACC_Z];
    log_struct_ptr->data[3] = state->data_cache[SDD_TEMP];


    //Commit log (also frees up the log packet memory)
    err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                   log_struct_ptr);
  }

  if (err_code != SNS_SUCCESS)
  {
    //printf("Failed to log sensor data packet with error code %d", err_code);
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_get_data

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
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Update the each sample status after a check for bound is done
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
)
{
  uint8_t i;
  sns_ddf_status_e stat;
  sns_ddf_sensor_data_s *data_ptr;
  sns_dd_acc_bma150_state_t *state = (sns_dd_acc_bma150_state_t *)dd_handle;

  /* Sanity check*/
  for (i = 0; i < num_sensors; i++)
  {
    if (sensors[i] != SNS_DDF_SENSOR_ACCEL &&
        sensors[i] != SNS_DDF_SENSOR_TEMP)
    {
      return SNS_DDF_EINVALID_PARAM;
    }
  }

  /* this is a sync driver, so try to read data now */
  if ( (stat = sns_dd_acc_bma150_get_data_from_device(
          (sns_dd_acc_bma150_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
  {
     return stat;
  }

  if( (data_ptr = sns_ddf_memhandler_malloc(memhandler,
        (num_sensors)*(sizeof(sns_ddf_sensor_data_s)))) == NULL)
  {
    return SNS_DDF_ENOMEM;
  }


  *data = data_ptr;
  /* The filling of the data doen in reverse order to avoid extra checks
  to fill up the logging structure
  */
  for (i=0; i <num_sensors; i++)
  {
    data_ptr[i].sensor = sensors[i];
    data_ptr[i].status = SNS_DDF_SUCCESS;
    data_ptr[i].timestamp = sns_ddf_get_timestamp();

    if (sensors[i] == SNS_DDF_SENSOR_ACCEL)
    {
      if( (data_ptr[i].samples = sns_ddf_memhandler_malloc(memhandler,
            SDD_ACC_NUM_AXES*sizeof(sns_ddf_sensor_sample_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      /* On 8660 the Y is the horizontal axis and X is the vertical (UP) axis
         hence the reverse data report & the flipping the sign of the Y axis
         is also required to comply with the SAE coordinate system on 8660 FFA
      */

     if (EnableI2C == FALSE)
     {
        static uint32 dummyData = 1<<16;
        static uint8_t count=0;

        data_ptr[i].samples[0].sample = dummyData;
        data_ptr[i].samples[1].sample = dummyData;
        data_ptr[i].samples[2].sample = dummyData;
        if (count++ < 4)
        {
          dummyData = (dummyData<<2);
        }
        else if (count < 9)
        {
          dummyData = (dummyData>>2);
        }
        else
        {
          count = 0;
        }
        data_ptr->num_samples = 3;
     }
     else
     {
#if defined (SNS_PCSIM)
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DD : sns_dd_acc_bma150_get_data");
        data_ptr[i].samples[0].sample  = 10;
        data_ptr[i].samples[1].sample = 20;
        data_ptr[i].samples[2].sample  = 30;
        data_ptr->num_samples = 3;
        /*
                  if (sns_playback_accel_data(&data_ptr[i]) != SNS_DDF_SUCCESS)
                  {
                     return SNS_DDF_EBUS;
                  }
             */
#else
        /* Handle axis conversion between FLUID and FFA/SURF for now */
        /* FLUID:    X=X, Y=-Y, Z=-Z */
        /* FFA/SURF: X=Y, Y=-X, Z=Z */
        /* This needs to be handled by the registry in the future */
#ifdef FLUID_BUILD
        data_ptr[i].samples[0].sample  = state->data_cache[SDD_ACC_X];
        data_ptr[i].samples[1].sample = -(state->data_cache[SDD_ACC_Y]);
        data_ptr[i].samples[2].sample  = -state->data_cache[SDD_ACC_Z];
#else
        data_ptr[i].samples[0].sample  = state->data_cache[SDD_ACC_Y];
        data_ptr[i].samples[1].sample = -(state->data_cache[SDD_ACC_X]);
        data_ptr[i].samples[2].sample  = state->data_cache[SDD_ACC_Z];
#endif /* FLUID_BUILD */

        data_ptr[i].samples[0].status =
          (sns_ddf_status_e)(((state->out_of_bound_flag >> SDD_ACC_Y)& 0x1)*SNS_DDF_EDATA_BOUND);
        data_ptr[i].samples[1].status =
          (sns_ddf_status_e)(((state->out_of_bound_flag >> SDD_ACC_X) & 0x1)*SNS_DDF_EDATA_BOUND);
        data_ptr[i].samples[2].status =
          (sns_ddf_status_e)(((state->out_of_bound_flag >> SDD_ACC_Z) & 0x1)*SNS_DDF_EDATA_BOUND);
        data_ptr[i].num_samples = SDD_ACC_NUM_AXES;
#endif /* SNS_PCSIM */
      }
    }
    else // TEMPER sensor
    {
      if( (data_ptr[i].samples = sns_ddf_memhandler_malloc(memhandler,
                                   sizeof(sns_ddf_sensor_sample_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      data_ptr[i].samples[0].sample = state->data_cache[SDD_TEMP];
      data_ptr[i].samples[0].status =
        (sns_ddf_status_e)(((state->out_of_bound_flag >> SDD_TEMP) & 0x1) * SNS_DDF_EDATA_BOUND);
      data_ptr[i].num_samples = 1;
     }
  }

  sns_dd_accel_log_data(state, data_ptr[0].timestamp);

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - range and resolution info is from the device data sheet.

  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_acc_bma150_get_attr
(
  sns_ddf_handle_t       dd_handle,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  sns_ddf_memhandler_s*  memhandler,
  void**                 value,
  uint32_t*              num_elems
)
{
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power_attrib;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                    sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib = *value;
      power_attrib->active_current = 200;
      power_attrib->lowpower_current = 1;
      *num_elems = 1;
    }
    break;

    // repeated twice I pic the one below
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;

      if (sensor == SNS_DDF_SENSOR_ACCEL)
      {
        if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      SNS_DD_ACCEL_MAX_RANGE*sizeof(sns_ddf_range_s)))
            == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_ranges = *value;
        *num_elems = 3;
        device_ranges[0].min = BOSCH_ACCEL_SENSOR_RANGE_2G_MIN;
        device_ranges[0].max = BOSCH_ACCEL_SENSOR_RANGE_2G_MAX;
        device_ranges[1].min = BOSCH_ACCEL_SENSOR_RANGE_4G_MIN;
        device_ranges[1].max = BOSCH_ACCEL_SENSOR_RANGE_4G_MAX;
        device_ranges[2].min = BOSCH_ACCEL_SENSOR_RANGE_8G_MIN;
        device_ranges[2].max = BOSCH_ACCEL_SENSOR_RANGE_8G_MAX;
      }
      else
      { // TEMPER sensor
        if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s))) == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_ranges = *value;
        *num_elems = 1;
        device_ranges->min = BOSCH_ACCEL_SENSOR_TEMPER_MIN;
        device_ranges->max = BOSCH_ACCEL_SENSOR_TEMPER_MAX;
      }
    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      device_res = *value;

      *num_elems = 1;
      if (sensor == SNS_DDF_SENSOR_ACCEL)
      {
        device_res->bit_len = BOSCH_ACCEL_SENSOR_ACC_BIT_LEN;
        device_res->max_freq = 200;
      }
      else
      { // TEMPER Sensor
        device_res->bit_len = BOSCH_ACCEL_SENSOR_TEMPER_BIT_LEN;
        device_res->max_freq = 200;
      }
    }
    break;

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      device_res = *value;

      *num_elems = 1;
      if (sensor == SNS_DDF_SENSOR_ACCEL)
      {
        *device_res = FX_FLTTOFIX_Q16(4*G/512);   /* For the range +-4G */
      }
      else
      { // TEMPER Sensor
        *device_res = FX_FLTTOFIX_Q16(0.5); // Degrees C
      }
    }
    break;

    case SNS_DDF_ATTRIB_LOWPASS:
    {
      uint8_t i;
      sns_ddf_lowpass_freq_t *freq_set;
      *num_elems = BOSCH_ACCEL_SENSOR_BW_BITMAP_1500Hz+1;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,(*num_elems)*sizeof(sns_ddf_lowpass_freq_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      freq_set = *value;

      //Since frequencies are indexed frm 0 to 6 adding a 1 to get the total
      *num_elems = BOSCH_ACCEL_SENSOR_BW_BITMAP_1500Hz+1;
      for(i=0; i<*num_elems; i++)
      {
       *(freq_set+i) = sns_dd_accel_freq[i];
      }
    }
    break;

    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                    sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay = *value;
      *num_elems = 1;
      //Not clear what this should be!
      device_delay->time_to_active = 3000; // 3ms is the startup time
      device_delay->time_to_data = 330;
    }
    break;

    case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      sns_ddf_driver_info_s *driver_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      driver_info = *value;
      *num_elems = 1;
      driver_info->name = "Bosch";
      driver_info->version = 1;
    }
    break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *device_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_info = *value;
      *num_elems = 1;
      device_info->model = "BMA150";
      device_info->vendor = "BOSCH";
      device_info->name = "Accelerometer";
      device_info->version = 1;
    }
    break;

    default:
     return SNS_DDF_EINVALID_PARAM;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_config_motion_int

===========================================================================*/
/*!
  @brief Configure (enable/disable) the motion interrupt

  @detail
  -
  @param[in] state: Ptr to the driver structure
  @param[in] enable: If true enable the interrupt if false disable the
  interrupt

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Get away with read modify write.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_config_motion_int
(
   sns_dd_acc_bma150_state_t  *state,
   bool                        enable
)
{
   sns_ddf_status_e status = SNS_DDF_SUCCESS;
   uint8_t bosch_write_buffer, out;

   /* Perform read, modify & write to set motion interrupt bit */
   if ( (status = sns_ddf_read_port(state->port_handle,
                                 BOSCH_ACCEL_SENSOR_CTRL1_REG_ADDR,
                                 &bosch_write_buffer,
                                 1,
                                 &out)) == SNS_DDF_SUCCESS)
   {
      if (enable)
      {
         /* enable advanced interrupt */
         bosch_write_buffer |= BOSCH_ACCEL_SENSOR_ENABLE_ADV_INT;
         /* enable wakeup mode */
         bosch_write_buffer |= BOSCH_ACCEL_SENSOR_WAKEUP;
      }
      else
      {
         /* disable advanced interrupt */
         bosch_write_buffer &= ~(BOSCH_ACCEL_SENSOR_ENABLE_ADV_INT);
         /* disable wakeup mode */
         bosch_write_buffer &= ~(BOSCH_ACCEL_SENSOR_WAKEUP);
      }

      status = sns_ddf_write_port(state->port_handle,
                               BOSCH_ACCEL_SENSOR_CTRL1_REG_ADDR,
                               &bosch_write_buffer,
                               1,
                               &out);
   }

   if(status == SNS_DDF_SUCCESS)
   {
       state->motion_detect_enabled = enable;
   }

   return status;
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_reset_bosch_motion_int

===========================================================================*/
/*!
  @brief Resets Bosch interrupt latched state

  @detail
  -  programs the BOSCH_ACCEL_SENSOR_RESET_INT in
  BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR reg @param[in] state: Ptr to the
  driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Not sure where this is going to be used.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_reset_bosch_motion_int
(
   sns_dd_acc_bma150_state_t  *state
)
{
   uint8_t bosch_write_buffer, out;
   sns_ddf_status_e stat;

  bosch_write_buffer = sns_dd_bosch_accel_ctrl_reg_val| BOSCH_ACCEL_SENSOR_RESET_INT;

  if((stat = sns_ddf_write_port(state->port_handle,
                             BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR,
                             &bosch_write_buffer,
                             1,
                             &out)) != SNS_DDF_SUCCESS)
  {
    return stat;
  }

  sns_dd_bosch_accel_ctrl_reg_val = bosch_write_buffer;

   // TODO - what to do on core?
   /* Switch GPIO to LOW to indicate motion interrupt reset */
   //sns_gpio_out(SNS_GPIO_COND_SIG, SNS_GPIO_LOW_VAL);

   return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_interrupt_handler

===========================================================================*/
/*!
  @brief BMA150 interrupt handler

  @detail
  -  programs the BOSCH_ACCEL_SENSOR_RESET_INT in
  BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR reg @param[in] state: Ptr to the
  driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  This function is taken out today because it needs sns_ddf_notify_event to be
  implemented by SMGR
  Not sure if this function is expected to call just the reset
  function or all the sns_dd_acc_bma150_config_motion_int &
  sns_dd_acc_bma150_reset_bosch_motion_int routines.
*/
/*=========================================================================*/
static void sns_dd_acc_bma150_interrupt_handler
(
  sns_ddf_handle_t dd_handle,
  sns_ddf_irq_e irq
)
{
   // Disable interrupt
   // sns_dd_acc_bma150_config_motion_int(dd_handle, false);

   // Reset the interrupt flag
   sns_dd_acc_bma150_reset_bosch_motion_int(dd_handle);

   // Notify manager of MOTION DETECT event
   sns_ddf_smgr_notify_event(((sns_dd_acc_bma150_state_t*)dd_handle)->smgr_hndl,
                             SNS_DDF_SENSOR_ACCEL,
                             SNS_DDF_EVENT_MOTION);
}


/*===========================================================================

  FUNCTION:   sns_dd_acc_bma150_config_motion_detect_thresh

===========================================================================*/
/*!
  @brief Configure the motion interrupt threshold

  @detail
  - Programs the register BOSCH_ACCEL_SENSOR_CTRL5_REG_ADDR for the MD
  threshold

  @param[in] state: Ptr to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Make sure it is called every time there is a change in the
  range.(not required for FC release but needed to be checked later)
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_acc_bma150_config_motion_detect_thresh
(
   sns_dd_acc_bma150_state_t *state
)
{
   uint8_t bosch_write_buffer, out;

   /***********************************************************
    The magic number used below is based on a fixed detect threshold of 47 and adjusted by
    the current range as follows:
       detect_thresh / scale_factor_for_current_range
    For 2G, the scale factor is 15.6
    For 4G, the scale factor is 31.2
    For 8G, the scale factor is 62.4

    So, the thresholds are:
      2G: 47/15.6 = 3
      4G: 47/31.2 = 2
      8G: 47/62.4 = 1
    **********************************************************/
  switch(state->range)
  {
    case BOSCH_ACCEL_SENSOR_RANGE_2G:
      bosch_write_buffer = 3;
      break;
    case BOSCH_ACCEL_SENSOR_RANGE_4G:
      bosch_write_buffer = 2;
      break;
    case BOSCH_ACCEL_SENSOR_RANGE_8G:
      bosch_write_buffer = 1;
      break;
    default:
      bosch_write_buffer = 3;
      break;
   }

   return sns_ddf_write_port(state->port_handle,
                          BOSCH_ACCEL_SENSOR_CTRL5_REG_ADDR,
                          &bosch_write_buffer,
                          1,
                          &out);
}


/*===========================================================================

FUNCTION:   sns_dd_acc_bma150_calibrate

===========================================================================*/
/*!
@brief  Calibrate the BOSCH BMA-150 accelerometer

@detail
    1.  This function is the BMA-150 calibrate entry point.
    2.  It converts the internal representation of the offset bias
        to a more general, sensor-related data (sns_ddf_sensor_data_s)
    3.  For further details see Calibration application-note

@attention Calibration should be performed while the module is positioned
    in a known orientation that is provided to this function, in order
    to compensate for the natural gravity acceleration.
@attention The module must remain stationary during calibration, or else
    calibration might fail.
@attention Calibration values are written to the accelerometer EEPROM. Since
    this memory supports a limited number of write cycles, number of
    writes should be limited. If this function is called only for
    testing, use the bWriteToEEPROM parameter to disable EEPROM writes.

@param[in] dd_handle          Handle to a driver instance.
@param[in] orientation        The orientation of the device.
                              indicates the axis on which the 1-g is acting
                              see also Table 1 in the Calibration application-note
                              (reference values for all orientations)
@param[in] max_tries          Maximal number of iterations of the calibration loop.
@param[in] write_to_eeprom_b  Indicate whether to write the final offset values
                              to the EEPROM or not.
@param[in] memhandler         Memory handler used to dynamically allocate
                              output parameters, if applicable.
@param[out] sensor_data       Sampled sensor data. The number of elements
                              must match @a num_sensors.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
SNS_DDF_EBUS            from sns_dd_acc_bma150_config_default
SNS_DDF_ENOMEM          no memory

$TODO:
*/
/*=========================================================================*/

#ifdef CONFIG_USE_BMA150_CALIB
static sns_ddf_status_e sns_dd_acc_bma150_calibrate
(
 sns_ddf_handle_t           dd_handle,
 acc_orientation_e          orientation,
 uint32_t                   max_tries,
 boolean                    write_to_eeprom_b,
 sns_ddf_memhandler_s*      memhandler,
 sns_ddf_sensor_data_s**    sensor_data
 )
{
    sns_dd_acc_bma150_state_t*  state = (sns_dd_acc_bma150_state_t *)dd_handle;
    sns_ddf_status_e            stat;
    sns_ddf_sensor_data_s*      data_ptr;
    OffsetDataType              stOffset;


    if (sensor_data)
    {
        *sensor_data    = NULL;
    }


    stat = sns_dd_acc_bma150_calibrate_internal(dd_handle,
        state->port_handle,
        orientation,
        max_tries,
        write_to_eeprom_b,
        &stOffset);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }


    /* raise bias offset data */
    if (sensor_data && memhandler)
    {
        if( (data_ptr = sns_ddf_memhandler_malloc(memhandler,
            sizeof(sns_ddf_sensor_data_s))) == NULL)
        {
            return SNS_DDF_ENOMEM;
        }

        *sensor_data = data_ptr;

        data_ptr->sensor = SNS_DDF_SENSOR_ACCEL;
        data_ptr->status = SNS_DDF_SUCCESS;
        data_ptr->timestamp = sns_ddf_get_timestamp();

        if( (data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
            SDD_ACC_NUM_AXES*sizeof(sns_ddf_sensor_sample_s))) == NULL)
        {
            return SNS_DDF_ENOMEM;
        }

        data_ptr->samples[0].sample = stOffset.usX;
        data_ptr->samples[0].status = SNS_DDF_SUCCESS;

        data_ptr->samples[1].sample  = stOffset.usY;
        data_ptr->samples[1].status = SNS_DDF_SUCCESS;

        data_ptr->samples[2].sample  = stOffset.usZ;
        data_ptr->samples[2].status = SNS_DDF_SUCCESS;

        data_ptr->num_samples = SDD_ACC_NUM_AXES;

    }


    return SNS_DDF_SUCCESS;
}
#endif // CONFIG_USE_BMA150_CALIB
