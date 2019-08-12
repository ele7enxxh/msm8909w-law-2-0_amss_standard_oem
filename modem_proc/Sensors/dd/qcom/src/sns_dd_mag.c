/*==============================================================================

    S E N S O R S   M A G N E T O M E T E R    D R I V E R

    DESCRIPTION
    Impelements the driver for the magnetometer driver 

    Copyright (c) 2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag.c#1 $


when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
08/28/11     ag     Added place holder for self test ptr
05/16/2011   sj     Changed Mag wake up timer value to max 15.1 ms (instead of 12.6ms)
04/24/2011   br     Inserted a missed line for error notifying(i.e. sns_dd_mag_notify_smgr_err())
03/24/2011   sj     Changed mag 8973 max frequency to 30Hz
01/24/2011   sj     Cleanup: Fix memory leaks, return values
01/21/2011   sc     Added support for PCSIM playback
01/20/2011   sj     Cleanup: Fix all comments
01/11/2011   yk     Added support for new attrib SNS_DDF_ATTRIB_RESOLUTION_ADC
                    and redefined SNS_DDF_ATTRIB_RESOLUTION.
12/23/2010   sj     Removed mag sensor reset through GSBI3. Now being
                    done by kernel driver on apps processor
11/18/2010   sj     Incorporating code review comments + removed floating point
                    operation
11/12/10     sj     Fixed a few bugs and added support for filtering
11/16/2010   sj     Fixed sizeof bug
11/08/10     RK     Fixed some bugs, cleaned up some code with 
                    appropriate comments added
10/09/10     RK     Fixed a i2c write in get_data, notify data call function updated 
                    and updated header comments for some functions
10/07/10     RK     Update the driver to work with latest DDF & updated the 
                    function headers
08/11/10     RK     created.
==============================================================================*/ 

#include "sns_dd_mag_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_log_types.h"

/* This DAL includes will be deleted once the PIL code brings the
   compass out of reset */
#ifndef SNS_PCSIM
#include <DALDeviceId.h>
#include <DALSys.h>
#include <ddii2c.h>
#endif

#ifdef SNS_PCSIM
#include "sns_playback.h"
#endif


#undef UNIT_TEST_ENABLE 

/*============================================================================
 INTERNAL DEFINITIONS
 ============================================================================*/

#define MAG_PLUS_TEMP_SAMPLE 4
#define MAG_ONLY_SAMPLE 3
#define TEMP_ONLY_SAMPLE 1

// This macro converts the temp from reg to degrees C
#define TEMP_CODE_TO_DEG_IN_CELCIUS(x) 35+(((120-x)*5)>>(3))

//.003*2**24 = Rnd(50331.6) = 50332 & final ans is in 32Q16 format
#define RAW_TO_GAUSS(x) ((50332*x)+((1)<<(7)))>>(8)

//.01*2**16 = Rnd(655.36) = 655 & final ans is in 32Q16 format
#define UT_TO_GAUSS(x) (x*655)

// Worst case delay is 15.1 msec
#define WAKEUP_DELAY_8973 15100

#define NUM_SENSORS_SUPPORTED_BY_AK8973 2

/*============================================================================
  GLOBALS 
 ============================================================================*/

/*TMR related defines*/ 
// Defining a tmr obj to be used by this driver
static sns_ddf_timer_s sns_dd_mag_tmr_obj;
/* Argument to pass to the driver's callback function. The value of the 
arg will indicate the actions to be taken in the CB routine */
static uint8_t sns_dd_mag_tmr_data_req_arg;

uint32_t snsd_mgt_akm_supported_modes  =
               (1 << SNS_DDF_POWERSTATE_ACTIVE | 1 << SNS_DDF_POWERSTATE_LOWPOWER);


/******************************************************************************/
/* indicates the dac offset for AKM; these are based on 7x30 FFA */
uint8_t snsd_mgt_akm_dac_x_setting = AKM_MAGNETOMETER_DAC_POS_OFFSET_0;
uint8_t snsd_mgt_akm_dac_y_setting = AKM_MAGNETOMETER_DAC_POS_OFFSET_0 + 3;
uint8_t snsd_mgt_akm_dac_z_setting = AKM_MAGNETOMETER_DAC_POS_OFFSET_0;

static int_fast8_t snsd_mgt_akm_dac_adj_needed = FALSE;

/*============================================================================
 FUNCTION PROTOTYPES
 ===========================================================================*/

sns_ddf_status_e sns_dd_akm_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);


sns_ddf_status_e sns_dd_akm_get_data(        
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

sns_ddf_status_e sns_dd_akm_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

sns_ddf_status_e sns_dd_akm_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

void sns_dd_akm_sample_sensor_cb(sns_ddf_handle_t dd_handle, void* arg);
      
void sns_dd_akm_hdle_irq(sns_ddf_handle_t dd_handle, sns_ddf_irq_e irq);

sns_ddf_status_e sns_dd_akm_reset(sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_mgt_akm_start_meas_8973(sns_dd_mag_akm_state_s *);

#ifndef SNS_PCSIM
extern sns_ddf_make_dal_i2c_config(
     sns_ddf_i2c_config_s* ,I2cConfigType*);
#endif

/*============================================================================
 STRUCTURE DEFINITIONS
 ============================================================================*/
sns_ddf_driver_if_s sns_mag_driver_fn_list =
{
  &sns_dd_akm_init,
  &sns_dd_akm_get_data,
  &sns_dd_akm_set_attr,
  &sns_dd_akm_get_attr,
  &sns_dd_akm_sample_sensor_cb,
  &sns_dd_akm_hdle_irq,
  &sns_dd_akm_reset,
  NULL /* self test */
};


/*===========================================================================

  FUNCTION:   sns_mgt_akm_config_default_8973

===========================================================================*/
/*!
  @brief Sets the AKM magnetometer to its default operational state
 
  @detail
  - Set x/y/z-axis offset DAC
  - Read out from EEPROM and write to Gain registers
 
  @param[in] state: Ptr to the driver state structure 
 
  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success
 
  $TODO: 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_mgt_akm_config_default_8973(sns_dd_mag_akm_state_s* state)
{
  uint8_t write_buffer[3];
  uint8_t read_buffer[3], out;
  sns_ddf_status_e err_code = SNS_DDF_SUCCESS;

  /* Set x/y/z-axis offset DAC */
  write_buffer[0] = snsd_mgt_akm_dac_x_setting;
  write_buffer[1] = snsd_mgt_akm_dac_y_setting;
  write_buffer[2] = snsd_mgt_akm_dac_z_setting;
  if( (err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_HXDA_8973,
                          write_buffer,
                          3,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Cannot configure X-axis offset DAC AKM magnetometer", 0, 0, 0);
    return err_code;
  }
  
  /* configure gain; read out from EEPROM and write to Gain registers */
  write_buffer[0] = AKM_MAGNETOMETER_EEPROM_MODE_8973;
  if( (err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_MODE_8973,
                          write_buffer,
                          1,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Cannot set EEPROM access mode on AKM magnetometer", 0, 0, 0);
    return err_code;
  }

  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAGNETOMETER_EEPROM_GAIN_8973, 
                              read_buffer,
                              3,
                              &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Cannot read Gain settings from EEPROM of AKM magnetometer", 0, 0, 0);
    return err_code;
  }


  //SNS_MSG_LOW("AKM gain from eeprom %x %x %x", read_buffer[0], read_buffer[1], read_buffer[2]);

  if( (err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_HXGA_8973,
                          read_buffer,
                          3,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Cannot write gain settings to AKM magnetometer", 0, 0, 0);
    return err_code;
  }

  /* read out old data just in case */
  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_H1X_8973, 
                              read_buffer,
                              3,
                              &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Failed to flush out old data. continue regardless", 0, 0, 0);
    return err_code;
  }

  /* go to low power mode from EEPROM mode */
  write_buffer[0] = AKM_MAGNETOMETER_PWR_DOWN_MODE_8973;
  if( (err_code = sns_ddf_write_port(state->port_handle,
                                     AKM_MAG_REG_ADDR_MODE_8973,
                                     write_buffer,
                                     1,
                                     &out)) != SNS_DDF_SUCCESS)
  {
    // SNS_MSG_ERROR("Cannot set EEPROM access mode on AKM magnetometer", 0, 0, 0);
    return err_code;
  }

  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_init

===========================================================================*/
/*!
  @brief Mag driver initialization. 
 
  @detail
  - Allocates memory for driver state structure.
  - Opens the device port by calling sns_ddf_open_port routine
  - Calls sns_mgt_akm_config_default_8973 routine to initialize mag sensor
 
  @param[out] dd_handle_ptr: Pointer that the init function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other  driver functions.
  @param[in]  smgr_handle:   Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params:     NV parameters retrieved for the driver.
  @param[in]  device_info:   Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler:    Memory handler used to dynamically allocate 
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             dd_handle_ptr. The memory allocated using 
                             memhandler will be freed by sensor manager.
  @param[in]  num_devices:   Length of @a device_info.
  @param[out] sensors:       List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors:   Length of @a sensors.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO:
*/
/*=========================================================================*/
                                              
sns_ddf_status_e sns_dd_akm_init(
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
  sns_dd_mag_akm_state_s *state_ptr;
  sns_ddf_status_e stat;
  uint8_t i;

  /* Input checks */
  if (dd_handle_ptr == NULL)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Init dd_handle_ptr so that in case of error the value of pointer 
   * will be consistent */
  *dd_handle_ptr = NULL;

  if(sns_ddf_malloc((void **)&state_ptr, sizeof(sns_dd_mag_akm_state_s)) != SNS_DDF_SUCCESS) 
  { 
    return SNS_DDF_ENOMEM;
  }

  if((stat = sns_ddf_open_port(&(state_ptr->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    // Free the allocated memory for the dd_handle
    sns_ddf_mfree(state_ptr);
    return stat;
  }

  state_ptr->smgr_hndl      = smgr_handle;
  state_ptr->dev_info       = device_info;
  state_ptr->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  state_ptr->curr_state     = SNSD_MGT_ST_IDLE;
  state_ptr->num_data       = 0;

  // TODO what to do with irq_0 and irq_1 and config
  for(i=0; i<num_devices; i++)
  {
      /* Fill out supported sensor info */
      if((*sensors = sns_ddf_memhandler_malloc(memhandler, 2*sizeof(sns_ddf_sensor_e))) == NULL)
      {
        // Free the allocated memory for the dd_handle
        sns_ddf_mfree(state_ptr);
        // Close ddf port which was opened earlier
        sns_ddf_close_port(state_ptr->port_handle);
        return SNS_DDF_ENOMEM;
      } 
      *num_sensors = NUM_SENSORS_SUPPORTED_BY_AK8973;
      (*sensors)[0] = SNS_DDF_SENSOR_MAG;
      (*sensors)[1] = SNS_DDF_SENSOR_TEMP;
      // Sets the default DAC Gain values from EEPROM to D*GA  registers
      if((stat = sns_mgt_akm_config_default_8973(state_ptr))!= SNS_DDF_SUCCESS)
      {
        // Free the allocated memory for the dd_handle
        sns_ddf_mfree(state_ptr);
        // Close ddf port which was opened earlier
        sns_ddf_close_port(state_ptr->port_handle);
        return stat;
      }

  }

  if((stat = sns_ddf_timer_init(&sns_dd_mag_tmr_obj, 
                                (sns_ddf_handle_t)state_ptr, 
                                &sns_mag_driver_fn_list,
                                &sns_dd_mag_tmr_data_req_arg, 0)) != SNS_DDF_SUCCESS)
  {
    // Free the allocated memory for the dd_handle
    sns_ddf_mfree(state_ptr);
    // Close ddf port which was opened earlier
    sns_ddf_close_port(state_ptr->port_handle);
    return stat;
  }

  *dd_handle_ptr = state_ptr;

  //use config info for Calibration?

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were 
         in after init() was called.
 
  @detail
  - resets some of the driver states
  - call the sns_mgt_akm_config_default_8973 in which the DAC offset and
    gain registers set based on the EEPROM values

  @param[in] state: Ptr to the driver structure
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO:
*/ 
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm_reset(sns_ddf_handle_t driver)
{
  sns_ddf_status_e stat;

  ((sns_dd_mag_akm_state_s *)driver)->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  ((sns_dd_mag_akm_state_s *)driver)->curr_state = SNSD_MGT_ST_IDLE;
  ((sns_dd_mag_akm_state_s *)driver)->num_data = 0;
  if((stat = sns_mgt_akm_config_default_8973(driver))!= SNS_DDF_SUCCESS)
  {
    return stat;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_config_hw_pwr_st_8973

===========================================================================*/
/*!
  @brief Routine used to set the power state of AKM 8973 when set as an
  attrib by SMGR
 
  @detail
  - Most of the time mag is in power down mode unless there is a request
    for data. So if mag is in active sampling mode and SMGR requests it
    to go into power down mode this routine would do the PWR DOWN settings.

  @param[in] state: Ptr to the driver structure
  @param[in] *mode: Power state change requested with pwr attrib set
                    routine
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO: Understand what setting in ACTIVE PWR state means in this case
*/ 
/*=========================================================================*/
sns_ddf_status_e sns_dd_config_hw_pwr_st_8973(
  sns_dd_mag_akm_state_s *state, 
  sns_ddf_powerstate_e *mode)
{
  uint8_t akm_write_buffer, out;
  if(*mode == SNS_DDF_POWERSTATE_LOWPOWER)
  {
    if(state->curr_state != SNSD_MGT_ST_IDLE)
    {
      akm_write_buffer = AKM_MAGNETOMETER_PWR_DOWN_MODE_8973; 
      if (sns_ddf_write_port(state->port_handle,
                             AKM_MAG_REG_ADDR_MODE_8973,
                             &akm_write_buffer,
                             1,  
                             &out) != SNS_DDF_SUCCESS)
      {
        // SNS_MSG_ERROR("Failed to start measurement on AKM", 0, 0, 0);
        return (SNS_DDF_EDEVICE);
      }
      state->curr_state = SNSD_MGT_ST_IDLE;
    }
    state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
    return SNS_DDF_SUCCESS;
  }
  else if(*mode == SNS_DDF_POWERSTATE_ACTIVE)
  {
    state->curr_pwr_state = SNS_DDF_POWERSTATE_ACTIVE;
    return SNS_DDF_SUCCESS;
  }
  else
  {
    return SNS_DDF_EINVALID_ATTR;
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_config_pwr_mode

===========================================================================*/
/*!
  @brief Routine used to set the power state when set as an attrib by
  SMGR that calls the appropriate pwr setting routines based on the
  device type requested
 
  @detail
  - Checks if requested power states are valid before calling the
    appropriate routine

  @param[in] state: Ptr to the driver structure
  @param[in] mode:  Power state change requested with pwr attrib set
  routine
 
  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO: 
*/ 
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm_config_pwr_mode
(
  sns_dd_mag_akm_state_s *state,
  sns_ddf_powerstate_e *mode
)
{
  if ((1 << *mode & snsd_mgt_akm_supported_modes) == 0)
  {
     //printf("Unsupported operating mode %d \n", *mode);
     return (SNS_DDF_EFAIL);
  }
  else if(*mode == state->curr_pwr_state)
  {
    return SNS_DDF_SUCCESS;
  }

  sns_dd_config_hw_pwr_st_8973(state, mode);

  state->curr_pwr_state = *mode;

  return(SNS_DDF_SUCCESS);
}
/*===========================================================================

  FUNCTION:   sns_dd_akm_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the AKM8973 mag
 
  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Mag has only pwr attribute that can be changed.
 
  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
    Otherwise SNS_DDF_EINVALID_ATTR
 
  $TODO: 
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm_set_attr
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
  sns_dd_mag_akm_state_s* state = (sns_dd_mag_akm_state_s *)dd_handle;
  switch(attrib)
  {
     case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
        ret_val = sns_dd_akm_config_pwr_mode(state,(sns_ddf_powerstate_e*)value);
        break;

     case SNS_DDF_ATTRIB_RANGE:
        /* AKM support only 1 set of range */
        break;

     case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        /* AKM only supports 1 resolution level for both mag and temp
        */ 
       ret_val = SNS_DDF_EINVALID_ATTR;
       break;

     default:
       return SNS_DDF_EINVALID_ATTR;
  }
  return(ret_val);
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  - All the info of the devices is from their data sheet.
 
  @param[in] dd_handle:  Handle to a driver instance.
  @param[in] sensor:     Sensor whose attribute is to be retrieved.
  @param[in] attrib:     Attribute to be retrieved.
  @param[in] memhandler: Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value:     Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems: Number of elements in @a value.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
    Otherwise SNS_DDF_ENOMEM (memory allocation failure) or 
    SNS_DDF_EINVALID_PARAM (invalid/unknown attribute is requested).
 
  $TODO: 
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm_get_attr
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
    // TODO - fill in pwr cap struct
    case SNS_DDF_ATTRIB_POWER_INFO:   
    {
      sns_ddf_power_info_s* power_attrib;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
           sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib = *value;
      *num_elems = 1;
      power_attrib->active_current = 6800;
      power_attrib->lowpower_current = 0;
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
      device_ranges = *value;
      *num_elems = 1;       
      if(sensor == SNS_DDF_SENSOR_MAG)
      {
        device_ranges->min = -1310720;/* Mag 8973 range min Q16 Gauss units -2mT */
        device_ranges->max = 1310720; /* Mag 8973 range max Q16 Gauss units 2mT*/
      }
      else
      {
        device_ranges->min = -1966080, /*temp  range in degrees celcius Q16 units (-30C) */
        device_ranges->max = 5898240; /*temp  range in degrees celcius Q16 units (90C) */
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
      device_res->bit_len = 8; //8 bit ADC resolution
      device_res->max_freq = 30;
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
      *device_res = UT_TO_GAUSS(1);
    }
    break;
    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay = *value;
      *num_elems = 1;
      //Not clear what this should be!
      device_delay->time_to_active = 12600;
      device_delay->time_to_data = 12600;           }
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
      driver_info->name = "AKM8973";
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
      device_info->model = "AK8973";
      device_info->vendor = "AKM";
      device_info->name = "Magnetometer";
      device_info->version = 1;
    }
    break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data
 
  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Mag is programmed to measurement mode where it would sample
    data and a few msec later be ready with the three axis Mag data.
    Hence a timer is set to be called back with the appropriate delay
    which would be the time to data ready state
 
  @param[in] dd_handle:    Handle to a driver instance.
  @param[in] sensors:      List of sensors for which data isrequested.
  @param[in] num_sensors:  Length of @a sensors.
  @param[in] memhandler:   Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data:        Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO: 
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm_get_data(        
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
  sns_ddf_status_e err_code = SNS_DDF_SUCCESS;
  sns_dd_mag_akm_state_s *state = (sns_dd_mag_akm_state_s *)dd_handle;

  /* state machine */
  switch (state->curr_state)
  {
    case SNSD_MGT_ST_IDLE:
#ifdef UNIT_TEST_ENABLE
      printf("Starting AKM Mgt measurement\n");
#endif      
      if(num_sensors == 2)
      {
        state->num_data = MAG_PLUS_TEMP_SAMPLE;
      }
      else if(num_sensors == 1)
      {
        if(sensors[0] == SNS_DDF_SENSOR_MAG)
        {
          state->num_data = MAG_ONLY_SAMPLE;
        }
        else
        {
          state->num_data = TEMP_ONLY_SAMPLE;
        }
      }
      if((err_code = sns_dd_mgt_akm_start_meas_8973(state)) == SNS_DDF_SUCCESS)
      {
        state->curr_state = SNSD_MGT_ST_STARTED;
        /* Mag returns data asynchronously. Hence set PENDING
         * status to return to Sensor Manager */
        err_code = SNS_DDF_PENDING;

        //set timer here 
        //Done temporarily for testing 
        //sns_ddf_delay(WAKEUP_DELAY_8973);
        //sns_dd_akm_sample_sensor_cb(dd_handle, NULL);
      }
    break;

    case SNSD_MGT_ST_STARTED:
#ifdef UNIT_TEST_ENABLE
      printf("Data collection already in progress \n");
#endif
      /* This is added for completeness and making SMGR aware that it first
      needs to change the HW device state to active and then get data which 
      in case of mag doesn't actually make a difference*/ 
      err_code = SNS_DDF_EFAIL;
    break;

    default:
#ifdef UNIT_TEST_ENABLE
      printf("Unknown MGT state: %d", state->curr_state);
#endif
      err_code = SNS_DDF_EFAIL;
    break;
  }

  return err_code;

}

/*===========================================================================

  FUNCTION:   sns_dd_mgt_akm_start_meas_8973

===========================================================================*/
/*!
  @brief Program the meas mode on AKM 8973
 
  @detail
  - Prog the AKM_MAG_REG_ADDR_MODE_8973 register for meas mode
 
  @param[in] state: Ptr to the driver state

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO: 
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mgt_akm_start_meas_8973(
  sns_dd_mag_akm_state_s *state)
{
  uint8_t akm_write_buffer, out;
  sns_ddf_status_e err_code = SNS_DDF_SUCCESS;
  // setting the cb arg to zero for this tmr request

  /* start measurement mode */
  akm_write_buffer = AKM_MAGNETOMETER_MEAS_MODE_8973; 
  if((err_code=sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_MODE_8973,
                          &akm_write_buffer,
                          1,
                          &out)) == SNS_DDF_SUCCESS)
  {

    err_code = sns_ddf_timer_start(sns_dd_mag_tmr_obj, WAKEUP_DELAY_8973);
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_dd_mgt_akm_adjust_dac

===========================================================================*/
/*!
  @brief Determines the DAC value to adjust to
 
  @detail
  - 
 
  @param[in] data: Data in 8bit signed raw data
  @param[in] dac:  Ptr to current DAC setting
 
  @return
    8bit data giving the DAC adjustment

  $TODO: Contact Victor on the logic of this function
*/
/*=========================================================================*/
static int8_t sns_dd_mgt_akm_adjust_dac(int8_t data, uint8_t *dac)
{
   int8_t step;
   int16_t newdac;

   /* first check to see if the raw_data is near the boundary */
   if (data >= AKM_MAGNETOMETER_DAC_LOW_BOUND && 
       data <= AKM_MAGNETOMETER_DAC_HIGH_BOUND &&
       !snsd_mgt_akm_dac_adj_needed)
   {
      return *dac;
   }

   /* if we are in DAC auto-adjust, want to adjust to +/-32 */
   if (snsd_mgt_akm_dac_adj_needed &&
       data >= AKM_MAGNETOMETER_DAC_DONE_LOW_BOUND &&
       data <= AKM_MAGNETOMETER_DAC_DONE_HIGH_BOUND)
   {
#ifdef UNIT_TEST_ENABLE
      printf("AKM DAC auto-adjust completed. data = %d; dac = 0x%x", data, *dac);
#endif
      return *dac;
   }
#ifdef UNIT_TEST_ENABLE
   printf("AKM DAC auto-adjust: current DAC setting: 0x%x", *dac);
#endif
   // step = -(int8)(data / AKM_MAGNETOMETER_DAC_STEP);
   step = -(int8_t)(data >> AKM_MAGNETOMETER_DAC_STEP_SHIFT);

   /* the DAC settings are not linear, need to check current dac before adjusting */
   if (*dac < AKM_MAGNETOMETER_DAC_POS_OFFSET_0)
   {
      newdac = *dac - step;
      if (newdac > 0x7F)
      {
         newdac = 0x7F;
      }
      else if (newdac < 0x0)
      {
         newdac = AKM_MAGNETOMETER_DAC_POS_OFFSET_0 - newdac;
      }
   }
   else
   {
      newdac = *dac + step;
      if (newdac > 0xFF)
      {
         newdac = 0xFF;
      }
      else if (newdac < AKM_MAGNETOMETER_DAC_POS_OFFSET_0)
      {
         newdac = AKM_MAGNETOMETER_DAC_POS_OFFSET_0 - newdac;
      }
   }
#ifdef UNIT_TEST_ENABLE
   printf("AKM DAC auto-adjus: raw data %d; new DAC 0x%x; step %d", data, newdac, step);
#endif
   if ((uint8_t)newdac != *dac)
   {
      return (uint8_t)newdac;
   }
   
   return *dac;
}
/*===========================================================================

  FUNCTION:   sns_dd_mgt_compute_raw_data_by_dac

===========================================================================*/
/*!
  @brief Takes into account the DAC offset and adjust the raw data
  accordingly
 
  @detail
  - 
 
  @param[in] raw: 8bit signed raw data
  @param[in] dac: DAC offset to be used to scale the raw data

  @return
    Adjusted data
 
  $TODO: 
*/
/*=========================================================================*/
static int32_t sns_dd_mgt_adjust_raw_data_by_dac(int8_t raw, uint8_t dac)
{
   int32_t data;
   if (dac >= AKM_MAGNETOMETER_DAC_POS_OFFSET_0)
   {
      data = raw - (dac - AKM_MAGNETOMETER_DAC_POS_OFFSET_0) * AKM_MAGNETOMETER_DAC_STEP;
   }
   else
   {
      data = raw + (dac * AKM_MAGNETOMETER_DAC_STEP);
   }
   return data;
}

/*===========================================================================

  FUNCTION:   sns_mgt_akm_get_data_axis_8973

===========================================================================*/
/*!
  @brief Gets the magnetic data from the akm magnetometer
 
  @detail
  - Get the raw data from the mag, Check for DAC adjustment
    requirements by calling sns_dd_mgt_akm_adjust_dac routine, And
    adjust the raw data with DAC adjustments.
 
 
  @param[in] state: Ptr to the driver state

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_mgt_akm_get_data_axis_8973(sns_dd_mag_akm_state_s *state)
{
  uint8_t write_data[3], out;
  uint8_t read_data[4], read_temp;
  
  int8_t  mgt_8bit_x, mgt_8bit_y, mgt_8bit_z;
  int32_t adj_mgt_x, adj_mgt_y, adj_mgt_z,tmp;
  uint8_t new_x, new_y, new_z;
  sns_ddf_status_e err_code;

#ifdef SNS_PCSIM
  return SNS_DDF_SUCCESS;
#endif

/* TODO : Below code should not execute for playback ?
 */
  switch(state->num_data)
  {
    case TEMP_ONLY_SAMPLE:
      /* If its to read only temperature, read it and bail out */
      if ( (err_code = sns_ddf_read_port(state->port_handle,
                                         AKM_MAG_REG_ADDR_TMPS_8973,
                                         &read_temp,
                                         1,
                                         &out)) != SNS_DDF_SUCCESS)
      {
         return err_code;
      }
      /* According to AKM 8973 Data sheet the temperature 
       * needs to be computed as follows:
       * Temperature(C) = 35 + ( 120 TMPS code ) / 1.6 (decimal value)
       * where TMPS code is the register value
       * To perform this equation the macro TEMP_CODE_TO_DEG_IN_CELCIUS
       * is used
       */
      state->temp_data_cache = FX_CONV_Q16(TEMP_CODE_TO_DEG_IN_CELCIUS(read_temp), 0);
      return SNS_DDF_SUCCESS;
    
    /* Note we want to have common code between MAG+TEMP and only MAG. So doing the things
     * which are different for these two cases here and then executing the common code
     */
    case MAG_PLUS_TEMP_SAMPLE:
      /* Attempting to read both data and temp in one I2C read to save the
       * I2C over head if that is the request from SMGR 
       * Note that the temperature register is the first register to read
       * followed by the registers containing MAG samples.
       */
      if ( (err_code = sns_ddf_read_port(state->port_handle,
                                         AKM_MAG_REG_ADDR_TMPS_8973, 
                                         read_data,
                                         state->num_data,
                                         &out)) != SNS_DDF_SUCCESS)
      {
        // SNS_MSG_ERROR("Failed to read from %x from AKM", AKM_MAGNETOMETER_H1X, 0, 0);
        return err_code;
      }
      /* read_temp would have a valid value only if state->num_data equals
       * 4. Convert to degC and Q16 and store.
       */
      state->temp_data_cache = FX_CONV_Q16(TEMP_CODE_TO_DEG_IN_CELCIUS(read_data[0]), 0);
      break;

    case MAG_ONLY_SAMPLE:
      /* Note we pass &read_data[1] as the buffer pointer. This is to make sure that
       * in MAg+Temp and Mag only case the Mag sample is read in the same portion 
       * of the buffer
       */
      if ( (err_code = sns_ddf_read_port(state->port_handle,
                                         AKM_MAG_REG_ADDR_H1X_8973, 
                                         &read_data[1],
                                         state->num_data,
                                         &out)) != SNS_DDF_SUCCESS)
    {
      // SNS_MSG_ERROR("Failed to read from %x from AKM", AKM_MAGNETOMETER_H1X, 0, 0);
      return err_code;
    }
    break;
   
    /* Dont know what to do */
    default:
      return SNS_DDF_EINVALID_PARAM;
   }; // end of switch 

   /* Common code for both Mag+Temp and Mag only */

   /* convert based on base range */
   mgt_8bit_x = AKM_MAGNETOMETER_MIN_MAG + read_data[1];
   mgt_8bit_y = AKM_MAGNETOMETER_MIN_MAG + read_data[2];
   mgt_8bit_z = AKM_MAGNETOMETER_MIN_MAG + read_data[3]; 

   /* see if dac needs to be adjusted */
   new_x = sns_dd_mgt_akm_adjust_dac(mgt_8bit_x, &snsd_mgt_akm_dac_x_setting);
   new_y = sns_dd_mgt_akm_adjust_dac(mgt_8bit_y, &snsd_mgt_akm_dac_y_setting);
   new_z = sns_dd_mgt_akm_adjust_dac(mgt_8bit_z, &snsd_mgt_akm_dac_z_setting);
   if (new_x != snsd_mgt_akm_dac_x_setting ||
       new_y != snsd_mgt_akm_dac_y_setting ||
       new_z != snsd_mgt_akm_dac_z_setting )
   {
     /* adjust DAC */
     write_data[0] = new_x;
     write_data[1] = new_y;
     write_data[2] = new_z;

     if ( (err_code = sns_ddf_write_port(state->port_handle,
                                         AKM_MAG_REG_ADDR_HXDA_8973,
                                         write_data,
                                         3,
                                         &out)) != SNS_DDF_SUCCESS)
     {
       // SNS_MSG_ERROR("Failed to write dac values to addr 0x%x", AKM_MAGNETOMETER_HXDA, 0, 0);
       return err_code;
     }
     else
     {
       snsd_mgt_akm_dac_adj_needed = TRUE;
       /* SNS_MSG_HIGH("DAC is changed(0x%x 0x%x 0x%x); need to re-sample", 
                        new_x, new_y, new_z); */
     }       
   }
   else
   {
     /* we are either done with auto-adjust or not needed at all */
     snsd_mgt_akm_dac_adj_needed = FALSE; 
   }

   /* adjust raw data with DAC offset */
   adj_mgt_x = sns_dd_mgt_adjust_raw_data_by_dac(mgt_8bit_x, snsd_mgt_akm_dac_x_setting);
   adj_mgt_y = sns_dd_mgt_adjust_raw_data_by_dac(mgt_8bit_y, snsd_mgt_akm_dac_y_setting);
   adj_mgt_z = sns_dd_mgt_adjust_raw_data_by_dac(mgt_8bit_z, snsd_mgt_akm_dac_z_setting);

    #if 0
    /* Align coordinates to reference frame assuming
       sensor on 7x30 FFA */
    /* if FFA: x=y, y=x, z=-z,
       if SURF: x=-x, y=y, z=-z
       if FLUID: x=x, y=-y, z=-z*/
    switch(hwtype_7x30) 
    {
      case HW_PLATFORM_FFA:
          tmp = adj_mgt_x;
          adj_mgt_x = adj_mgt_y;
          adj_mgt_y = tmp;
          adj_mgt_z = adj_mgt_z * -1;;
          break;
       case HW_PLATFORM_SURF:
          adj_mgt_x = adj_mgt_x * -1;
          adj_mgt_z = adj_mgt_z * -1;   
          break;
       case HW_PLATFORM_FLUID:
          adj_mgt_y = adj_mgt_y * -1;
          adj_mgt_z = adj_mgt_z * -1;
          break;
       case HW_PLATFORM_UNKNOWN:
       default:
          SNS_MSG_HIGH("Unknown 7x30 HW platform: %d", hwtype_7x30, 0, 0);
          break;
    }
    #endif

    /* Align coordinates for FFA for now
     * if FFA: x=y, y=-x, z=z
     * TODO: Based on hardware type read from registry
     * we need to convert accordingly
     */
   tmp = adj_mgt_x;
   adj_mgt_x = adj_mgt_y;
   adj_mgt_y = -1* tmp;
   //adj_mgt_z = adj_mgt_z * -1;
#ifdef UNIT_TEST_ENABLE
  printf("data before converting to fix pt x=%d , y=%d, z=%d\n", adj_mgt_x, adj_mgt_y, adj_mgt_z);
#endif
  /* convert to milli-guass
  1 micro Tesla = 10 milli-guass */
  //Change from original mag
  //state->data_cache[0] = FX_FLTTOFIX(adj_mgt_x * 10, 16);
  //state->data_cache[1] = FX_FLTTOFIX(adj_mgt_y * 10, 16);
  //state->data_cache[2] = FX_FLTTOFIX(adj_mgt_z * 10, 16); 
  state->data_cache[0] = UT_TO_GAUSS(adj_mgt_x);
  state->data_cache[1] = UT_TO_GAUSS(adj_mgt_y);
  state->data_cache[2] = UT_TO_GAUSS(adj_mgt_z); 

  /* update DAC settings */
  snsd_mgt_akm_dac_x_setting = new_x;
  snsd_mgt_akm_dac_y_setting = new_y;
  snsd_mgt_akm_dac_z_setting = new_z;   

  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_dd_mag_log_data
===========================================================================*/
/*!
  @brief log sensor data for magnetometer
 
  @detail
  - Logs latest set of sensor data sampled from the mag sensor. 
 
  @param[in] state:       Ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None
 
*/
/*=========================================================================*/
void sns_dd_mag_log_data(sns_dd_mag_akm_state_s *state,
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
    log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAG;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AKM;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = state->num_data;
    log_struct_ptr->data[0]        = state->data_cache[0];
    log_struct_ptr->data[1]        = state->data_cache[1];
    log_struct_ptr->data[2]        = state->data_cache[2];
    log_struct_ptr->data[3]        = state->temp_data_cache;


    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, 
                      log_struct_ptr);
  } //end of if
} //end of sns_dd_mag_log_data


/*===========================================================================

  FUNCTION:   sns_dd_mag_notify_smgr_err

===========================================================================*/
/*!
  @brief Called SMGR needs to be notified of error 
 
  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when an error is seen in memory
  allocation for sample data or problem in reading the data from the
  Mag. The top level sensor data structure with filled with appropriate
  error type before calling the sns_ddf_smgr_notify_data routine
 
  @param[in] state: Ptr to the driver state structure

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success
 
  $TODO: 
*/
/*=========================================================================*/
static void sns_dd_mag_notify_smgr_err(
  sns_dd_mag_akm_state_s *state,
  sns_ddf_status_e err_code, 
  sns_ddf_sensor_data_s *sensor_data_report,
  uint32_t num_sensors)
{
  uint8_t i;
  for(i=0; i<(uint8_t)num_sensors; i++)
  {
    sensor_data_report[i].sensor = SNS_DDF_SENSOR_MAG;
    sensor_data_report[i].status = err_code;
    sensor_data_report[i].timestamp = sns_ddf_get_timestamp();
  }
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, num_sensors);
}


/*===========================================================================

  FUNCTION:   sns_mgt_akm_sample_sensor_8973

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. 
 
  @note This will routine read the data status & read the data and copy
  it at a malloced location. The address of the data location is passed
  as parameter in the notify_data routine.
 
  @param[in] state: Ptr to the driver state structure

  @return
    None
 
  $TODO: 
*/
/*=========================================================================*/
static void sns_mgt_akm_sample_sensor_8973(sns_dd_mag_akm_state_s *state)
{
  uint8_t akm_read_data, out, size;
  sns_ddf_status_e err_code;
  // Don't need to statically declare the two if temp sensor is not asked for!
  sns_ddf_sensor_data_s *sensor_data_report;
  sns_ddf_sensor_sample_s *data_ptr;
  uint32_t buff_len;

  /* Resetting the state to idle once into this routine implying
  whatever the error case SMGR could request for data again & the driver
  would come back with appropriate error condition again instead of
  nacking based on state being not idle*/
  state->curr_state = SNSD_MGT_ST_IDLE;

  /* Checking for mem alloc size for the base data structure based on
  num sensors requested */
  buff_len = 1; 
  if(state->num_data == MAG_PLUS_TEMP_SAMPLE) 
  {
    buff_len = 2;
  }

  size = buff_len*sizeof(sns_ddf_sensor_data_s);
  if((err_code = sns_ddf_malloc((void **)&sensor_data_report, size)) != SNS_DDF_SUCCESS) 
  {
    // returning the NULL to SMGR indicating no memory even for basic err notify
    sns_ddf_smgr_notify_data(state->smgr_hndl, NULL, 0); 
    return;
  } 

  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_STATUS_8973, 
                              &akm_read_data,
                              1,
                              &out)) != SNS_DDF_SUCCESS)
  {
    //Handling the err cond when status from Mag is not a success
    sns_dd_mag_notify_smgr_err(state, err_code, sensor_data_report, buff_len); 
    sns_ddf_mfree(sensor_data_report);
    return;
  }

#ifdef SNS_PCSIM
  /* force data to be ready to read in simulation */
  akm_read_data |= AKM_MAGNETOMETER_INT_BIT_MASK;
#endif

  if(akm_read_data & AKM_MAGNETOMETER_INT_BIT_MASK)
  {
    if((err_code = sns_mgt_akm_get_data_axis_8973(state)) != SNS_DDF_SUCCESS)
    {
      /* Handling the err cond when status after doing a I2C data read
      there is an error on the bus */ 
      sns_dd_mag_notify_smgr_err(state, err_code, sensor_data_report, buff_len);
      sns_ddf_mfree(sensor_data_report);
      return;
    }
  }
  else
  {
    /* Handling the err cond when status indicates data is still not
    ready after giving it ample time to be ready 13msec */
    sns_dd_mag_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, buff_len);    
    sns_ddf_mfree(sensor_data_report);
    return;
  }

  /* # of samples is based on num_data already precalculated in get_data
  routine and this is used to allocate the correct size for the sample
  structure */
  size = state->num_data*sizeof(sns_ddf_sensor_sample_s);
  if((err_code = sns_ddf_malloc((void **)&data_ptr, size)) != SNS_DDF_SUCCESS) 
  { 
    // Handling the err cond when no mem avail to give data samples to SMGR 
    sns_dd_mag_notify_smgr_err(state, err_code, sensor_data_report, buff_len); 
    sns_ddf_mfree(sensor_data_report);
    return;
  }

#ifdef SNS_PCSIM
  sensor_data_report->sensor = SNS_DDF_SENSOR_MAG;
  sensor_data_report->status = SNS_DDF_SUCCESS;
  sensor_data_report->samples = data_ptr;
  sensor_data_report->timestamp = sns_ddf_get_timestamp();
  if (sns_playback_mag_data(sensor_data_report) != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(data_ptr);
    sns_dd_mag_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, buff_len);    
    sns_ddf_mfree(sensor_data_report);
    return;
  }
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, buff_len);
  /* Log the data */ 
  sns_dd_mag_log_data(state,sensor_data_report->timestamp);    

#else
  // Copying the date before notifying SMGR
  if(state->num_data == MAG_ONLY_SAMPLE)
  {
    data_ptr[0].sample = state->data_cache[0];
    data_ptr[0].status = SNS_DDF_SUCCESS;
    data_ptr[1].sample = state->data_cache[1];
    data_ptr[1].status = SNS_DDF_SUCCESS;
    data_ptr[2].sample= state->data_cache[2];
    data_ptr[2].status = SNS_DDF_SUCCESS;

    /* mark data as ready */ 
    sensor_data_report->sensor = SNS_DDF_SENSOR_MAG;
    sensor_data_report->status = SNS_DDF_SUCCESS;
    sensor_data_report->timestamp = sns_ddf_get_timestamp();
    sensor_data_report->samples = data_ptr;
    sensor_data_report->num_samples = 3;
    sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, buff_len);
    /* Log the data */ 
    sns_dd_mag_log_data(state,sensor_data_report->timestamp);    
  }
  else if(state->num_data == MAG_PLUS_TEMP_SAMPLE)
  {
    data_ptr[0].sample = state->data_cache[0];
    data_ptr[0].status = SNS_DDF_SUCCESS;
    data_ptr[1].sample = state->data_cache[1];
    data_ptr[1].status = SNS_DDF_SUCCESS;
    data_ptr[2].sample= state->data_cache[2];
    data_ptr[2].status = SNS_DDF_SUCCESS;
    data_ptr[3].sample = state->temp_data_cache;
    data_ptr[3].status = SNS_DDF_SUCCESS;
    /* mark data as ready */ 
    sensor_data_report[0].sensor = SNS_DDF_SENSOR_MAG;
    sensor_data_report[0].status = SNS_DDF_SUCCESS;
    sensor_data_report[0].timestamp = sns_ddf_get_timestamp();
    sensor_data_report[0].samples = data_ptr;
	   sensor_data_report[0].num_samples = 3;

    sensor_data_report[1].sensor = SNS_DDF_SENSOR_TEMP;
    sensor_data_report[1].status = SNS_DDF_SUCCESS;
    sensor_data_report[1].timestamp = sns_ddf_get_timestamp();
    sensor_data_report[1].samples = &data_ptr[3];
    sensor_data_report[1].num_samples = 1;

    sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, buff_len);
    /* Log the data */ 
    sns_dd_mag_log_data(state,sensor_data_report[0].timestamp);
  }
  else if(state->num_data == TEMP_ONLY_SAMPLE)
  {
    data_ptr[3].sample = state->temp_data_cache;
    data_ptr[3].status = SNS_DDF_SUCCESS;
    sensor_data_report->sensor = SNS_DDF_SENSOR_TEMP;
    sensor_data_report->status = SNS_DDF_SUCCESS;
    sensor_data_report->timestamp = sns_ddf_get_timestamp();
    sensor_data_report->samples = (data_ptr+3);
    sensor_data_report->num_samples = 1;
    sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, buff_len);
    /* Log the data */ 
    sns_dd_mag_log_data(state,sensor_data_report->timestamp);
  }
#endif
 
  sns_ddf_mfree(data_ptr);
  sns_ddf_mfree(sensor_data_report);
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_sample_sensor_cb

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This must be 
         the callback function submitted when initializing a timer.
 
  @note This will be called within the context of the Sensors Manager task.
     
  @param[in] dd_handle:  Handle to a driver instance.
  @param[in] arg:        The argument submitted when the timer was set.

  @return
    None
 
  $TODO: 
*/
/*=========================================================================*/
void sns_dd_akm_sample_sensor_cb(sns_ddf_handle_t dd_handle, void* arg)
{
  sns_dd_mag_akm_state_s *state = (sns_dd_mag_akm_state_s *)dd_handle;

  sns_ddf_timer_cancel(sns_dd_mag_tmr_obj);

  if(*((uint8_t *)arg) == 0)
  {
    sns_mgt_akm_sample_sensor_8973(state);
  }
  //resetting the global tmr arg value to 0
}

/*===========================================================================

  FUNCTION:   sns_dd_akm_hdle_irq

===========================================================================*/
/*!
  @brief Called to handle interrupt from mag sensor.
 
  @note
     
  @param[in] dd_handle:  Handle to a driver instance.
  @param[in] irq:        Interrupt type received

  @return
    None
 
  $TODO: This will be implemented later
*/
/*=========================================================================*/
void sns_dd_akm_hdle_irq(sns_ddf_handle_t dd_handle, sns_ddf_irq_e irq)
{
  return;
}




