/*==============================================================================

    S E N S O R S   AKM 8975 M A G N E T O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the magnetometer driver

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8975.c#1 $


when         who    what, where, why
----------   ---    ----------------------------------------------------------- 
09/13/12     vh     Eliminated compiler warnings
06/20/12     ag     Removing feature FEATURE_AKM8975_SELF_TEST_IMPL. Assume 
                    it is always defined.
06/19/12     vh     Added run time switch for I2C code
02/02/12     ad     Send error report to sensor manager with valid sensor id 
01/06/12     SD     Changed max frequence from 60hz to 50hz 
11/09/11     YK     Added support for axis mapping. 
10/12/11     AG     Don't release timer when init fails
10/11/11     br     deleted line which releases resources when init fail(we need the resource for get_attr()..)
09/27/11     AG     Moved definition of FEATURE_AKM8975_SELF_TEST_IMPL to arm7.scons
09/21/11     AG     Featurize self test; implement AKM review feedback
09/01/11     AG     Implement changes from code review
08/28/11     AG     Added self test functionality 
06/23/11     dc     removed debug flag for external release
06/04/11     br     Changed to honor SENSOR__ALL too in addition to SENSOR_MAG. Changed log msg level
03/24/11     OF     Adjust sensitivity values to the measurement data read out
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
01/18/11     OF     Split mag driver to AK8975 & AK8973 separte files, splited from sns_dd_mag.c version#8
04/08/10     RY     Added axis conversion for FLUID and FFA/SURF.
==============================================================================*/

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_dd_mag_akm8975_priv.h"


/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/


/*=======================================================================

                  Extern Variables

========================================================================*/
extern volatile boolean EnableI2C;
extern sns_ddf_driver_if_s sns_mag_akm8975_driver_fn_list;

/*=======================================================================

  Global Data Definitions

 ========================================================================*/

static sns_dd_mag_akm8975_state_s *drv_akm8975_dbg = NULL;

/*=======================================================================

                   FUNCTION PROTOTYPES

========================================================================*/
sns_ddf_status_e sns_dd_akm8975_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

sns_ddf_status_e sns_dd_akm8975_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

sns_ddf_status_e sns_dd_akm8975_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

sns_ddf_status_e sns_dd_akm8975_reset(
        sns_ddf_handle_t dd_handle);

sns_ddf_status_e sns_dd_akm8975_self_test(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_test_e       test,
        uint32_t             *err);

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/


/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_config_pwr_mode

===========================================================================*/
/*!
  @brief Routine used to set the power state when set as an attrib by
  SMGR that calls the appropriate pwr setting routines based on the
  device type requested

  @detail
  When power is turned ON, AK8975 is in power-down mode. When any other mode
  is selected the AK8975 transits to the specified mode and starts operation.
  When operation finishes the AK8975 turns back to power-down mode.
  Therefore there isn't much difference between SNS_DDF_POWERSTATE_LOWPOWER and
  SNS_DDF_POWERSTATE_ACTIVE modes. When the former is set, we need to cancel
  any measurement in progress.

  @param[in/out] state: Ptr to the driver structure
  @param[in] *mode: Power state change requested with an pwr attrib set
  routine

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success;

  $TODO:
*/

/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_config_pwr_mode
(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_powerstate_e        mode
)
{
  uint8_t akm_write_buffer, out;

  if(mode == state->curr_pwr_state)
  {
    return SNS_DDF_SUCCESS;
  }

  if(mode == SNS_DDF_POWERSTATE_LOWPOWER)
  {
    if(state->curr_state != SNSD_MGT8975_ST_IDLE)
    {
      MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8888);

      akm_write_buffer = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975; 
      if (sns_ddf_write_port(state->port_handle,
                             AKM_MAG_REG_ADDR_CNTL_8975,
                             &akm_write_buffer,
                             DDF_WRITE_ONE_BYTE,
                             &out) != SNS_DDF_SUCCESS)
      {
        SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_FAILED_TO_PWR_DOWN);
        return (SNS_DDF_EDEVICE);
      }
      // If current state is not IDLE, it means that we start measurement mode and also started the timer
      sns_ddf_timer_cancel(state->sns_dd_mag_tmr_obj);
      state->curr_state = SNSD_MGT8975_ST_IDLE;
    }
    state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  }
  else if(mode == SNS_DDF_POWERSTATE_ACTIVE)
  {
    state->curr_pwr_state = SNS_DDF_POWERSTATE_ACTIVE;
  }
  else
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_PWR_MODE_NOT_SUPPORTED, mode);
    return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_mgt_akm_get_sensitivity_data_8975

===========================================================================*/
/*!
  @brief Gets the sensitivity adjusment values of the AKM8975 magnetometer

  @detail
  - Get the axis sensitivity adjusment values from the fuse ROM, store it in the driver state
    structure in Q16 gauss units.

  @param[in] *state - ptr to the driver state

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success;

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_mgt_akm_get_sensitivity_data_8975(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t           out, akm_write_buffer;
  uint8_t           read_data[SENSITIVITY_NUM_BYTES];
  sns_ddf_status_e  err_code;
  int               i;

  /* Set Fuse ROM access mode */
  akm_write_buffer = AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // TODO: ADD Write Error string
    //SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_WRITE_FAILURE, AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8975);
    return err_code;
  }

  /* Read the data */
  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_X_SEN_ADJUST_8975,
                              read_data,
                              SENSITIVITY_NUM_BYTES,
                              &out)) != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_X_SEN_ADJUST_8975);
    return err_code;
  }

  for(i=0 ; i<3 ; ++i)
  {
    state->sens_cache[i] = read_data[i];
    // Sensitivity adjusment is calculated acording to AKM8975 datasheet version MS1187-E-02 section 8.3.11
    state->sens_adj[i] = (((state->sens_cache[i] - 128 ) * 0.5)/128) + 1;
  }

  MAG8975_DBG_MSG_3(DBG_DD_MAG8975_SENSITIVITY_DATA, state->sens_cache[0], state->sens_cache[1], state->sens_cache[2]);

  /* Set Power-down mode */
  akm_write_buffer = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // TODO: ADD Write Error string
    //SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_WRITE_FAILURE, AKM_MAGNETOMETER_PWR_DOWN_MODE_8975);
    return err_code;
  }

  return SNS_DDF_SUCCESS;
}


/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_init

===========================================================================*/
/*!
  @brief Mag driver initialization.

  @detail
  Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine

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
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_akm8975_init(
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
  sns_dd_mag_akm8975_state_s* state;
  sns_ddf_status_e            status;
  uint8_t                     out;

  *dd_handle_ptr = NULL;
  *sensors      = NULL;

  MAG8975_DBG_MSG_0(DBG_DD_MAG8975_INITIALIZING);

  if(sns_ddf_malloc((void **)&state, sizeof(sns_dd_mag_akm8975_state_s)) != SNS_DDF_SUCCESS)
  {
    return SNS_DDF_ENOMEM;
  }

  memset(state, 0, sizeof(sns_dd_mag_akm8975_state_s));
  drv_akm8975_dbg = state; /* Debug Only */
  *dd_handle_ptr  = state;

  if((status = sns_ddf_open_port(&(state->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  state->smgr_hndl      = smgr_handle;
  state->dev_info       = device_info;
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  state->curr_state     = SNSD_MGT8975_ST_IDLE;

  sns_ddf_axes_map_init(
      &state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

  if((*sensors = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e))) == NULL)
  {
    status = SNS_DDF_ENOMEM;
    goto errorCleanUp;
  }

  /* Fill out supported sensor info */
  (*sensors)[0]   = SNS_DDF_SENSOR_MAG;
  *num_sensors    = NUM_SENSORS_SUPPORTED_BY_AK8975;

  /* Init akm8975 timer */
  if((status = sns_ddf_timer_init(&state->sns_dd_mag_tmr_obj,
                                (sns_ddf_handle_t)state,
                                &sns_mag_akm8975_driver_fn_list,
                                state,
                                0)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  // Read Device ID, to make sure device is working properly
  status = sns_ddf_read_port((sns_ddf_handle_t)state->port_handle,
                              AKM_MAG_REG_ADDR_DEVICEID_8975,
                              &state->device_id,
                              DDF_READ_ONE_BYTE,
                              &out);

  if(status != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_DEVICEID_8975);
    goto errorCleanUp;
  }

  if (EnableI2C == TRUE)
  {
    if(state->device_id != AKM_MAG_8975_DEVICE_ID)
    {
      status = SNS_DDF_EDEVICE;
      goto errorCleanUp;
    }
  }

  // Read Device INFO
  status = sns_ddf_read_port((sns_ddf_handle_t)state->port_handle,
                              AKM_MAG_REG_ADDR_INFO_8975,
                              &state->device_info,
                              DDF_READ_ONE_BYTE,
                              &out);

  if(status != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_INFO_8975);
    goto errorCleanUp;
  }

  /* Read sensitivity data */
  if ((status = sns_mgt_akm_get_sensitivity_data_8975(state)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  return SNS_DDF_SUCCESS;

  errorCleanUp:
    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  - resets some of the driver states
  call the sns_mgt_akm_config_default_8975 in which the DAC offset and
  gain registers set based on the EEPROM values

  @param[in] dd_handle   Handle to a driver instance.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm8975_reset(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e            status;
  sns_dd_mag_akm8975_state_s* state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  status = sns_dd_akm8975_config_pwr_mode(state, SNS_DDF_POWERSTATE_LOWPOWER);

  state->curr_state      = SNSD_MGT8975_ST_IDLE;

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the either the AKM8973 or AKM8975 mag

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Mag has only pwr attribute that can be changed.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] sensor     Sensor for which this attribute is to be set.
  @param[in] attrib     Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm8975_set_attr
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e            ret_val = SNS_DDF_SUCCESS;
  sns_dd_mag_akm8975_state_s* state = (sns_dd_mag_akm8975_state_s *)dd_handle;
  sns_ddf_powerstate_e        power_state;

  MAG8975_DBG_MSG_2(DBG_DD_MAG8975_SET_ATTRIB_REQ,
                                sensor,
                                attrib );

  if ((sensor != SNS_DDF_SENSOR_MAG) && (sensor != SNS_DDF_SENSOR__ALL))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
     case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
       power_state = *(sns_ddf_powerstate_e *)value;
       ret_val = sns_dd_akm8975_config_pwr_mode(state, power_state);
       MAG8975_DBG_MSG_1(DBG_DD_MAG8975_SET_POWER_STATE_REQ, state);
        break;

     case SNS_DDF_ATTRIB_RANGE:
        /* AKM8975 support only 1 set of range */
       MAG8975_DBG_MSG_0(DBG_DD_MAG8975_SET_RANGE_REQ);
        break;

     case SNS_DDF_ATTRIB_RESOLUTION:
        /* AKM8975 only supports 1 resolution level */
       MAG8975_DBG_MSG_0(DBG_DD_MAG8975_SET_RESOLUTION_REQ);
       break;

     default:
       return SNS_DDF_EINVALID_ATTR;
  }
  return(ret_val);
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - All the info of the devices is from their data sheet.

  @param[in] dd_handle    Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in] attrib       Attribute to be retrieved.
  @param[in] memhandler   Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm8975_get_attr
(
  sns_ddf_handle_t       dd_handle,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  sns_ddf_memhandler_s*  memhandler,
  void**                 value,
  uint32_t*              num_elems
)
{
  MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_ATTRIB_REQ, sensor, attrib );

  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

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
      power_attrib                    = *value;
      *num_elems                      = 1;
      power_attrib->active_current    = AKM_MAG8975_HI_PWR; /* unit of uA */
      power_attrib->lowpower_current  = AKM_MAG8975_LO_PWR; /* unit of uA */
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_POWER_INFO_REQ,
                                   power_attrib->active_current,
                                   power_attrib->lowpower_current);
    }
    break;
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))
            == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
      device_ranges       = *value;
      *num_elems          = 1;
      device_ranges->min  = AKM_MAG8975_MIN_RANGE;
      device_ranges->max  = AKM_MAG8975_MAX_RANGE;
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_RANGE_REQ,
                                   device_ranges->min,
                                   device_ranges->max);
    }
    break;
	#ifndef USE_CC41
	case SNS_DDF_ATTRIB_RESOLUTION_ADC:
	{
		sns_ddf_resolution_adc_s *device_res_adc;
		if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
		{
		  return SNS_DDF_ENOMEM;
		}
		device_res_adc 				= *value;
		*num_elems 					= 1;
		device_res_adc->bit_len 	= AKM_MAG8975_RESOLUTION_ADC; //13 bit ADC resolution
		device_res_adc->max_freq 	= 50;
	}
	break;
	#endif
    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_res  = *value;
      *num_elems  = 1;
#ifdef USE_CC41
     *device_res = AKM_MAG8975_RESOLUTION_ADC;
#else
     *device_res = mag_akm8975_scale;
#endif
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_RESOLUTION_REQ, *device_res);
    }
    break;
    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay  = *value;
      *num_elems    = 1;
      device_delay->time_to_active  = USEC_TIME_FOR_MEASUREMENT_8975;
      device_delay->time_to_data    = USEC_TIME_FOR_MEASUREMENT_8975;
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_DELAYS_REQ,
                                   device_delay->time_to_active,
                                   device_delay->time_to_data);
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
      driver_info->name = "AK8975";
#ifdef USE_CC41
      driver_info->version = "1.0";
#else
      driver_info->version = 1;
#endif
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
      device_info->model = "AK8975";
      device_info->vendor = "AKM";
      device_info->name = "Magnetometer";
#ifdef USE_CC41
      device_info->version = "1.0";
#else
      device_info->version = 1;
#endif
    }
    break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_akm8975_self_test

===========================================================================*/
/**
 * @brief Runs a factory self test case. 
 *  
 * Self tests cover an embedded hardware test in cases where the sensor 
 * supports it, as well as driver based sensor tests. This is generally run 
 * in a factory setting, and must not be called while a device is streaming 
 * data. 
 *  
 * @param[in] dd_handle  Handle to a driver instance. 
 * @param[in] sensor     Sensor on which to run the test. 
 * @param[in] test       Test case to run. 
 *  
 * @return One of the following error codes:
 *     SNS_DDF_SUCCESS        - Test passed.
 *     SNS_DDF_PENDING        - Test result will be sent as an event.
 *     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
 *     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
 *     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
 *     SNS_DDF_EFAIL          - Unknown error occured.
 */
sns_ddf_status_e sns_dd_akm8975_self_test(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_test_e       test,
  uint32_t             *err)
{
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;
  uint8_t                    akm_write_buffer_astc, akm_write_buffer_cntl;
  uint8_t                    out, akm_write_buffer_lpm;
  sns_ddf_status_e           status = SNS_DDF_SUCCESS;
  sns_akm_err_codes_e        err_code = SNSD_MAG_ERR_GENERIC;

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8100);

  *err = SNSD_MAG_ERR_GENERIC;

  /* check parameter - sensor */
  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* check parameter - test */
  if (test != SNS_DDF_TEST_SELF)
  {
    return SNS_DDF_EINVALID_TEST;
  }

  /* check state */
  if (state->curr_state != SNSD_MGT8975_ST_IDLE)
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, state->curr_state);
    return SNS_DDF_EDEVICE_BUSY;
  }

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8200);

  akm_write_buffer_astc = AKM_MAGNETOMETER_ASTC_SELF_TEST_ENABLE; 
  akm_write_buffer_cntl = AKM_MAGNETOMETER_SELF_TEST_MODE_8975;
  akm_write_buffer_lpm = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975; 

  status = sns_ddf_write_port(state->port_handle,
                            AKM_MAG_REG_ADDR_CNTL_8975,
                            &akm_write_buffer_lpm,
                            DDF_WRITE_ONE_BYTE,  
                            &out);

  MAG8975_SMGR_DBG_MSG_2(DBG_SMGR_GENERIC_STRING2, 8210, status);

  if (status == SNS_DDF_SUCCESS)
  {
    sns_ddf_delay(100); /* wait for mode change to take effect */
  
    if(((status = sns_ddf_write_port(state->port_handle,
                                     AKM_MAG_REG_ADDR_SELF_TEST_8975,
                                     &akm_write_buffer_astc,
                                     DDF_WRITE_ONE_BYTE,
                                     &out)) == SNS_DDF_SUCCESS) &&
        ((status = sns_ddf_write_port(state->port_handle,
                                      AKM_MAG_REG_ADDR_CNTL_8975,
                                      &akm_write_buffer_cntl,
                                      DDF_WRITE_ONE_BYTE,
                                      &out)) == SNS_DDF_SUCCESS))
    {
      state->curr_state = SNSD_MGT8975_ST_TEST_STARTED;
      if ((status = sns_ddf_timer_start(state->sns_dd_mag_tmr_obj, 
                       USEC_TIME_FOR_MEASUREMENT_8975)) != SNS_DDF_SUCCESS)
      {
        err_code = SNSD_MAG_ERR_TIMER;
        state->curr_state = SNSD_MGT8975_ST_IDLE;
      }
    }
    else
    {
      err_code = SNSD_MAG_ERR_I2C;
    }
    MAG8975_SMGR_DBG_MSG_2(DBG_SMGR_GENERIC_STRING2, 8300, err_code);
  }
  else
  {
    err_code = SNSD_MAG_ERR_I2C;
  }

  if (status == SNS_DDF_SUCCESS)
  {
    return SNS_DDF_PENDING;
  }
  else
  {
    *err = err_code;
    return SNS_DDF_EFAIL;
  }
}


