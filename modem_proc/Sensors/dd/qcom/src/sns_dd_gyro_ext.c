/*==============================================================================

    S E N S O R S   G Y R O S C O P E   D R I V E R  E X T E R N A L

DESCRIPTION

   Implements part of the Invensense 3050 gyroscope driver that will reside outside tightly coupled memory.

Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_gyro_ext.c#1 $


when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
06/20/13     pn      Increased max_freq to 250Hz
05/24/12     ag      Remove feature FEATURE_MPU3050_SELF_TEST_IMPL
01/20/12     ag      enable reading bias attrib for all implementations of self test 
11/21/11     yk      Added support for axis mapping.
11/14/11     br      Changed max freq of temp from 10 to 200 
09/06/11     ag      Self test related cleanup and changes from code review 
08/31/11     ag      Self test ported from MPL_4_0_1 for internal use
08/13/11     br      make it ascending order of sns_dd_gyro_lpfbw_val,
                     changed LOWPASS unit to q16_t
08/11/11     br      make it return the ODR value for get_attr() forLOWPASS
08/10/11     ks      refactor gyro driver for memory optimization
 
==============================================================================*/

/*============================================================================
 INCLUDE FILES
============================================================================*/
#include "sns_ddf_attrib.h"  
#include "sns_ddf_common.h"  
#include "sns_ddf_comm.h" 
#include "sns_ddf_driver_if.h" 
#include "sns_ddf_memhandler.h" 
#include "sns_ddf_util.h" 

#include "sns_dd_gyro_priv.h" 

extern int32_t sns_dd_gyro_range_scale[GYRO_3050_NUM_FS_RANGES];

/*=======================================================================
 INTERNAL DEFINITIONS AND TYPES
========================================================================*/

//Gyroscope minimum measurement value in radians per second
static const int32_t sns_dd_gyro_range_val_min[GYRO_3050_NUM_FS_RANGES] =
{
   GYRO_3050_CONV_DPS_TO_RPSQ16(-250),
   GYRO_3050_CONV_DPS_TO_RPSQ16(-500),
   GYRO_3050_CONV_DPS_TO_RPSQ16(-1000),
   GYRO_3050_CONV_DPS_TO_RPSQ16(-2000)
};

//Gyroscope maximum measurement value in radians per second
static const int32_t sns_dd_gyro_range_val_max[GYRO_3050_NUM_FS_RANGES] =
{
   GYRO_3050_CONV_DPS_TO_RPSQ16(250),
   GYRO_3050_CONV_DPS_TO_RPSQ16(500),
   GYRO_3050_CONV_DPS_TO_RPSQ16(1000),
   GYRO_3050_CONV_DPS_TO_RPSQ16(2000)
};

//Supported gyroscope low pass filter bandwidth values
static const uint32_t sns_dd_gyro_lpfbw_val[GYRO_3050_NUM_DLPF_BW] =
{
   FX_FLTTOFIX_Q16(5),      /*GYRO_3050_BITS_DLPF_CFG_5HZ*/
   FX_FLTTOFIX_Q16(10),     /*GYRO_3050_BITS_DLPF_CFG_10HZ*/
   FX_FLTTOFIX_Q16(20),     /*GYRO_3050_BITS_DLPF_CFG_20HZ*/
   FX_FLTTOFIX_Q16(42),     /*GYRO_3050_BITS_DLPF_CFG_42HZ*/
   FX_FLTTOFIX_Q16(98),     /*GYRO_3050_BITS_DLPF_CFG_98HZ*/
   FX_FLTTOFIX_Q16(188),    /*GYRO_3050_BITS_DLPF_CFG_188HZ*/
   FX_FLTTOFIX_Q16(256),    /*GYRO_3050_BITS_DLPF_CFG_256HZ_NOLPF2*/
};

//Supported sensor types
static sns_ddf_sensor_e sns_dd_gyro_sensor_types[] =
{
   SNS_DDF_SENSOR_GYRO,
   SNS_DDF_SENSOR_TEMP
};

/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_init
===========================================================================*/
/*!
  @brief Initializes the gyroscope
 
  @detail
  - Allocates memory for driver state structure.
  - Opens the device port
  - Initializes the driver and configures the device

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
sns_ddf_status_e sns_dd_gyro3050_init(
   sns_ddf_handle_t* dd_handle_ptr,
   sns_ddf_handle_t smgr_handle,
   sns_ddf_nv_params_s* nv_params,
   sns_ddf_device_access_s device_info[],
   uint32_t num_devices,
   sns_ddf_memhandler_s* memhandler,
   sns_ddf_sensor_e* sensors[],
   uint32_t* num_sensors)                                  
{
   sns_ddf_status_e status;
   sns_dd_gyro3050_state_s* ptr;

   if (dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL)
      return SNS_DDF_EINVALID_PARAM;

   status = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_gyro3050_state_s));
   if (status != SNS_DDF_SUCCESS)
      return status;

   ptr->smgr_hndl = smgr_handle;

   status = sns_ddf_open_port(&(ptr->port_handle), 
                              &(device_info->port_config));
   if (status != SNS_DDF_SUCCESS)
   {
      sns_ddf_mfree(ptr);   
      return status;
   }

   sns_ddf_axes_map_init(
       &ptr->axes_map, (nv_params != NULL) ? &nv_params->data[0] : NULL);

   //supported sensor info
   *num_sensors = sizeof(sns_dd_gyro_sensor_types)/sizeof(sns_ddf_sensor_e);
   *sensors = sns_dd_gyro_sensor_types;

   *dd_handle_ptr = (sns_ddf_handle_t)ptr;

   //reset the device
   status = sns_dd_gyro3050_reset(ptr);

   return status;
}


/*===========================================================================
  FUNCTION:   sns_dd_gyro3050_get_attr
===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  - range and resolution info is from the device data sheet.
 
  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[out] buffer      Destination buffer for the attribute
                          value.
  @param[out] buffer_len  Length @a buffer.

  @return
    The error code definition within the DDF 
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 
  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_gyro3050_get_attr(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensor,
   sns_ddf_attribute_e attrib,
   sns_ddf_memhandler_s* memhandler,
   void** value,
   uint32_t* num_elems)
{
   if (dd_handle == NULL || value == NULL)
      return SNS_DDF_EINVALID_PARAM;

   switch (attrib)
   {
      case SNS_DDF_ATTRIB_POWER_INFO:
      {
         sns_ddf_power_info_s* power_attrib = 
         sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s));
         if (power_attrib == NULL)
            return SNS_DDF_ENOMEM;

         power_attrib->active_current = 5900;
         power_attrib->lowpower_current = 5;

         *value = power_attrib;
         *num_elems = 1;

         break;
      }

      case SNS_DDF_ATTRIB_RANGE:
      {
         sns_ddf_range_s* device_ranges; 
         uint8_t i;

         if (sensor == SNS_DDF_SENSOR_GYRO)
         {
            device_ranges = sns_ddf_memhandler_malloc(
               memhandler, 
               GYRO_3050_NUM_FS_RANGES * sizeof(sns_ddf_range_s));
            if (device_ranges == NULL)
               return SNS_DDF_ENOMEM;

            for (i = 0; i < GYRO_3050_NUM_FS_RANGES; i++)
            {
               device_ranges[i].min = sns_dd_gyro_range_val_min[i];
               device_ranges[i].max = sns_dd_gyro_range_val_max[i];
            }

            *num_elems = GYRO_3050_NUM_FS_RANGES;
         }
         else //temperature sensor
         {
            device_ranges = 
            sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s));
            if (device_ranges == NULL)
               return SNS_DDF_ENOMEM;

            device_ranges[0].min = GYRO_3050_FS_TEMP_MIN;
            device_ranges[0].max = GYRO_3050_FS_TEMP_MAX;

            *num_elems = 1;
         }
         *value = device_ranges;

         break;
      }

      case SNS_DDF_ATTRIB_LOWPASS:
      {
         uint8_t i;
         sns_ddf_lowpass_freq_t *freq_set = 
         sns_ddf_memhandler_malloc(memhandler, 
                                   GYRO_3050_NUM_DLPF_BW * 
                                   sizeof(sns_ddf_lowpass_freq_t));
         if (freq_set == NULL)
            return SNS_DDF_ENOMEM;

         for (i = 0; i < GYRO_3050_NUM_DLPF_BW; i++)
         {
            freq_set[i] = sns_dd_gyro_lpfbw_val[i];
         }

         *value = freq_set;
         *num_elems = GYRO_3050_NUM_DLPF_BW;

         break;
      }

      case SNS_DDF_ATTRIB_DRIVER_INFO:
      {
         sns_ddf_driver_info_s *driver_info = 
         sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s));
         if (driver_info == NULL)
            return SNS_DDF_ENOMEM;

         driver_info->name = "InvensenseGyro";
         driver_info->version = 1;

         *value = driver_info;
         *num_elems = 1;

         break;
      }

      case SNS_DDF_ATTRIB_DEVICE_INFO:
      {
         sns_ddf_device_info_s *device_info =
         sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s));
         if (device_info == NULL)
            return SNS_DDF_ENOMEM;

         device_info->name = "GYROSCOPE";
         device_info->vendor = "INVENSENSE";
         device_info->model = "MPU3050";
         device_info->version = 1;

         *value = device_info;
         *num_elems = 1;

         break;
      }

      case SNS_DDF_ATTRIB_DELAYS:
      {
         sns_ddf_delays_s *device_delay = 
         sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
         if (device_delay == NULL)
            return SNS_DDF_ENOMEM;

         device_delay->time_to_active = GYRO_3050_STARTUP_DELAY_MICROSEC;
         device_delay->time_to_data = GYRO_3050_DATA_DELAY_MICROSEC;   

         *value = device_delay;
         *num_elems = 1;

         break;
      }

      case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      {
         sns_ddf_resolution_adc_s *device_res = 
            sns_ddf_memhandler_malloc(memhandler, 
                                      sizeof(sns_ddf_resolution_adc_s));
         if(device_res == NULL)
            return SNS_DDF_ENOMEM;
         
         device_res->bit_len  = GYRO_3050_RAW_DATA_BIT_LEN;
         device_res->max_freq = 250;
         *value = device_res;
         *num_elems = 1;
         
         break;
      }

      case SNS_DDF_ATTRIB_RESOLUTION:
      {
         sns_ddf_resolution_t *device_res = 
            sns_ddf_memhandler_malloc(memhandler, 
                                      sizeof(sns_ddf_resolution_t));
         if(device_res == NULL)
            return SNS_DDF_ENOMEM;
    
         if (sensor == SNS_DDF_SENSOR_GYRO)
         {
            //report the resolution of range 1(+-1000deg/s) as range is fixed by SMGR
            *device_res = sns_dd_gyro_range_scale[((sns_dd_gyro3050_state_s*)dd_handle)->range] >> 9;
         }
         else //temperature sensor
         {
            *device_res = GYRO_3050_TEMP_SCALE;
         }
    
         *value = device_res;
         *num_elems = 1;
    
         break;
      }

      case SNS_DDF_ATTRIB_BIAS:
         {
            sns_ddf_bias_t *device_bias;
            uint8_t        i; 

            if ( sensor == SNS_DDF_SENSOR_GYRO )
            {
               device_bias =  sns_ddf_memhandler_malloc(memhandler, 
                                                        SDD_GYRO_NUM_AXES * sizeof(sns_ddf_bias_t));
               if ( device_bias == NULL )
                  return SNS_DDF_ENOMEM;

               for ( i=0; i < SDD_GYRO_NUM_AXES; i++ )
               {
                  device_bias[i] = ((sns_dd_gyro3050_state_s *)dd_handle)->biases[i] ;
               }

               *num_elems = SDD_GYRO_NUM_AXES;
            }
            else //temperature sensor
            {
               return SNS_DDF_EINVALID_PARAM;
            }

            *value = device_bias;

            break;
         }

      default:
         return SNS_DDF_EINVALID_PARAM;
   }

   return SNS_DDF_SUCCESS;
}
