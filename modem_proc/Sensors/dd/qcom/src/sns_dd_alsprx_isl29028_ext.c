/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Init and get_attrib routines for ALS/PRX driver. These can be placed outside
   TCM if needed.

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_isl29028_ext.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/29/12     ag      Enabled dummy data 
07/14/11     dc      support Range attribute for proximity sensor
07/05/11     ag      init and get_attrib separation
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_dd.h"
#include "sns_dd_alsprx_isl29028_priv.h"

extern volatile boolean EnableI2C;

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
#ifdef ALSPRX_USE_POLLING
static sns_ddf_sensor_e prx_timer = SNS_DDF_SENSOR_PROXIMITY;
static sns_ddf_sensor_e als_timer = SNS_DDF_SENSOR_AMBIENT;
#endif /* ALSPRX_USE_POLLING */

static const sns_ddf_sensor_e sns_dd_alsprx_sensor_types[SNS_DD_ALSPRX_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT
};

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_alsprx_isl29028_reset
(
  sns_ddf_handle_t dd_handle
);

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29028_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to 
   associated devices, configures the driver and devices, and places 
   the devices in the default power state. Returns the instance handle along 
   with a list of supported sensors. This function will be called at init 
   time.
    
   @param[out] dd_handle_ptr  Pointer that this function must malloc and 
                              populate. This is a handle to the driver
                              instance that will be passed in to all other
                              functions. NB: Do not use @a memhandler to
                              allocate this memory.
   @param[in]  smgr_handle    Handle used to identify this driver when it 
                              calls into Sensors Manager functions.
   @param[in]  nv_params      NV parameters retrieved for the driver.
   @param[in]  device_info    Access info for physical devices controlled by 
                              this driver. Used to configure the bus
                              and talk to the devices.
   @param[in]  num_devices    Number of elements in @a device_info. 
   @param[in]  memhandler     Memory handler used to dynamically allocate 
                              output parameters, if applicable. NB: Do not
                              use memhandler to allocate memory for
                              @a dd_handle_ptr.
   @param[out] sensors        List of supported sensors, allocated, 
                              populated, and returned by this function.
   @param[out] num_sensors    Number of elements in @a sensors.
  
   @return Success if @a dd_handle_ptr was allocated and the driver was 
           configured properly. Otherwise a specific error code is returned.
 
*/
/*=========================================================================*/                                             
sns_ddf_status_e sns_dd_alsprx_isl29028_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     reg_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{

  sns_ddf_status_e       stat = SNS_DDF_SUCCESS;
  sns_dd_alsprx_state_t* ptr;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_SMGR, //SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_SMGR_GENERIC_STRING1, //DBG_DD_ALSPRX_NV_DATA_FROM_SMGR,
                             100);
#endif /* ALSPRX_DEBUG_BASIC */

  if ( (dd_ptr == NULL)      || 
       (num_sensors == NULL) || 
       (sensors == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_alsprx_state_t)) != SNS_DDF_SUCCESS )
  {
    return SNS_DDF_ENOMEM;
  }
  ptr->smgr_handle = smgr_handle;

  if ( (stat = sns_ddf_open_port(&ptr->port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS )
  {
    return stat;
  }

  *dd_ptr = (sns_ddf_handle_t)ptr;

if (EnableI2C == TRUE)
{
#ifdef ALSPRX_USE_POLLING
  stat = sns_ddf_timer_init(&ptr->sns_dd_alsprx_common_db.als_timer.timer_obj, 
                            *dd_ptr, 
                            &sns_alsprx_isl29028_driver_fn_list, 
                            &als_timer, 
                            FALSE);
  if ( stat != SNS_DDF_SUCCESS )
  {
    return stat;
  }

  stat = sns_ddf_timer_init(&ptr->sns_dd_alsprx_common_db.prx_timer.timer_obj, 
                            *dd_ptr, 
                            &sns_alsprx_isl29028_driver_fn_list, 
                            &prx_timer, 
                            FALSE);
  if ( stat != SNS_DDF_SUCCESS )
  {
    return stat;
  }
#endif /* ALSPRX_USE_POLLING */
}
  /* Consume registry data */
  ptr->sns_dd_alsprx_common_db.nv_source = SNS_DD_ALSPRX_NV_SOURCE_DEFAULT;
  if ((reg_params->status == SNS_DDF_SUCCESS) &&
      (reg_params->data_len == sizeof(sns_dd_nv_db_type)))
  {
    ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = reg_params->data[0];
    ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = reg_params->data[1];
    ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = ((uint16_t)reg_params->data[3]<<8) | 
                                                     reg_params->data[2];
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = ((uint16_t)reg_params->data[5]<<8) | 
                                                       reg_params->data[4];
    ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = ((uint16_t)reg_params->data[7]<<8) | 
                                                       reg_params->data[6];
    ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = ((uint16_t)reg_params->data[9]<<8) |
                                                       reg_params->data[8];
    ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = ((uint16_t)reg_params->data[11]<<8) |
                                                       reg_params->data[10];
    ptr->sns_dd_alsprx_common_db.nv_source = SNS_DD_ALSPRX_NV_SOURCE_REG;
  }

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_SMGR, //SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_SMGR_GENERIC_STRING1, //DBG_DD_ALSPRX_NV_DATA_FROM_SMGR,
                               reg_params->status);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_SMGR, //SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_SMGR_GENERIC_STRING1, //DBG_DD_ALSPRX_NV_DATA_FROM_SMGR,
                               reg_params->data_len);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_SMGR, //SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_SMGR_GENERIC_STRING1, //DBG_DD_ALSPRX_NV_DATA_FROM_SMGR,
                               reg_params->nvitem_grp);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_SMGR, //SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_SMGR_GENERIC_STRING1, //DBG_DD_ALSPRX_NV_DATA_FROM_SMGR,
                               reg_params->nvitem_id);
#endif /* ALSPRX_DEBUG_REG */

  /* Fill out supported sensor info */
  *num_sensors = SNS_DD_ALSPRX_NUM_SENSOR_TYPES;
#if ALSPRX_DEBUG_ONE_SENSOR
  *num_sensors = 1;
#endif
  *sensors = (sns_ddf_sensor_e *)sns_dd_alsprx_sensor_types; 

  /* Resets the driver */
  stat = sns_dd_alsprx_isl29028_reset((sns_ddf_handle_t) ptr);

  return stat;
}


/*==============================================================================

FUNCTION      sns_dd_alsprx_prx_query
  
DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for proximitydistance data type 

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_prx_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t* thresh_ptr;
  uint16_t* accu_ptr;
  sns_ddf_power_info_s* power_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0);
      device_ranges->max = FX_FLTTOFIX_Q16(50.0);
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES_ADC,
                                    SNS_DD_ALSPRX_PRX_BITS);
      resp->bit_len = SNS_DD_ALSPRX_PRX_BITS;
      resp->max_freq = SNS_DD_ALSPRX_PRX_FREQ;
      *value = resp;
      break;
    }
    
    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_PRX_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_PWR,
                                    power_ptr->active_current); // add power_ptr->lowpower_current to F3
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_ACC,
                                    SNS_DD_PRX_ACCURACY);
      *accu_ptr = SNS_DD_PRX_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near;
      *(uint16_t **)value = thresh_ptr;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_THRESH,
                                    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near);
      break;

    default:
      SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_ATTRIB_ERR,
                                  attrib);
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_als_query
  
DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e sns_dd_alsprx_als_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0);
      device_ranges->max = FX_FLTTOFIX_Q16(2000.0);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES_ADC,
                                    SNS_DD_ALSPRX_ALS_BITS);
      resp->bit_len = SNS_DD_ALSPRX_ALS_BITS;
      resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = SNS_DD_ALS_RES;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES,
                                    *resp);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_ALS_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_PWR,
                                    power_ptr->active_current); // add power_ptr->lowpower_current to F3
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      dd_ptr->sns_dd_als_db.accuracy = dd_ptr->sns_dd_als_db.last_mlux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
      if ( dd_ptr->sns_dd_als_db.accuracy < SNS_DD_ALS_HALF_LUX )
      {
        dd_ptr->sns_dd_als_db.accuracy = SNS_DD_ALS_HALF_LUX;
      }
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_ACC,
                                    dd_ptr->sns_dd_als_db.accuracy);
      *accu_ptr = dd_ptr->sns_dd_als_db.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    default:
      SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_ATTRIB_ERR,
                                  attrib);
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29028_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  Returns the requested attribute
 
  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor      Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved. 
  @param[in]  memhandler  Memory handler used to dynamically allocate 
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set 
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    Success if the attribute was retrieved and the buffer was 
    populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_alsprx_isl29028_get_attrib
(
  sns_ddf_handle_t     handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)handle;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  BOOLEAN generic_attrib = FALSE;
  sns_ddf_driver_info_s *driver_info_ptr;
  sns_ddf_driver_info_s driver_info_l = {
    "ISL29028", /* name */
    1           /* version */
  };
  sns_ddf_device_info_s *device_info_ptr;
  sns_ddf_device_info_s device_info_l = {
    "ALSPRX",   /* name */
    "Intersil", /* vendor */
    "ISL29028", /* model */
    1           /* version */
  };

  if ( (handle == NULL)      || 
       (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_ATTRIB_REQ,
                                sensor,
                                attrib );
  
  /* check first if the query is for generic attributes */
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *driver_info_ptr = driver_info_l; /* Is memcopy needed because of strings */
      *(sns_ddf_driver_info_s**)value = driver_info_ptr;
      *num_elems = 1;
      generic_attrib = TRUE;
      break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *device_info_ptr = device_info_l;
      *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
      *num_elems = 1;
      generic_attrib = TRUE;
      break;

    default:
      /* do nothing */
      break;
  }

  /* The query is not for generic attribute but is for specific data type */
  if ( generic_attrib != TRUE )
  {
    switch ( sensor )
    {
      case SNS_DDF_SENSOR_AMBIENT:
        status = sns_dd_alsprx_als_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      case SNS_DDF_SENSOR_PROXIMITY:
        status = sns_dd_alsprx_prx_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      default:
        SNS_PRINTF_STRING_ID_HIGH_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_SENSOR_TYPE_ERR,
                                    sensor);
        status = SNS_DDF_EINVALID_ATTR;
    }
  }

  return status;
}
