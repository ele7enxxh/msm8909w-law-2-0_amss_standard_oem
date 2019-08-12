/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the ap3216c ALS(Light) and Proximity Sensor

Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_ap3216c_ap3220_ext.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
06/06/14     MW      init() function signal register fix
08/09/13     yh      Initial version of driver for AP3216C
==============================================================================*/
/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_ap3216c_ap3220_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_memmgr.h"

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif

#define REG_FIXED_NV_SIZE_128      128

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
static const sns_ddf_sensor_e sns_dd_alsprx_sensor_types[SNS_DD_ALSPRX_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT
};

/*=============================================================================
                           STATIC FUNCTION PROTOTYPES
==============================================================================*/
extern sns_ddf_status_e sns_dd_alsprx_ap3216c_reset
(
  sns_ddf_handle_t dd_handle
);
extern sns_ddf_driver_if_s sns_alsprx_ap3216c_driver_fn_list;
extern void sns_dd_alsprx_ap3216c_handle_irq( sns_ddf_handle_t handle, sns_ddf_irq_e irq );
extern void sns_dd_ap3216c_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);
extern void sns_dd_ap3216c_set_reg_values_from_nv(sns_dd_alsprx_state_t* state);

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_init

DESCRIPTION   init
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_init(
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
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_dd_alsprx_state_t* ptr;

    if ( (dd_ptr == NULL)      ||
         (num_sensors == NULL) ||
         (sensors == NULL) )
    {
        DDF_MSG_0(HIGH, "invalid param");
        return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_ddf_malloc((void**)&ptr, sizeof(sns_dd_alsprx_state_t));
    if ( status != SNS_DDF_SUCCESS )
    {
        return status;
    }

    ptr->smgr_handle = smgr_handle;
    ptr->int_gpio    = device_info->first_gpio;

    status = sns_ddf_open_port(&ptr->port_handle, &device_info->port_config);
    if ( status != SNS_DDF_SUCCESS )
    {
        return status;
    }

    device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
    status = sns_ddf_open_port(&ptr->ZBW_port_handle, &device_info->port_config);
    if( status != SNS_DDF_SUCCESS )
    {
        return status;
    }


    /* Fill out output parameters*/
    *num_sensors = 2;
    *sensors     = (sns_ddf_sensor_e *)sns_dd_alsprx_sensor_types;
    *dd_ptr      = (sns_ddf_handle_t) ptr;

    /* Check to see if IRQ's are enabled*/
    if(sns_ddf_signal_irq_enabled() && (ptr->int_gpio != 0xFFFF))
    {
        /* IRQ's are enabled, register the irq*/
        status = sns_ddf_signal_register(ptr->int_gpio,
                                         *dd_ptr,
                                         &sns_alsprx_ap3216c_driver_fn_list,
                                         SNS_DDF_SIGNAL_IRQ_FALLING);

        if( status != SNS_DDF_SUCCESS )
        {
            return status;
        }
    }

     DDF_MSG_0(HIGH, "past irq and timer enable");
     ptr->nv_db_size =0;
     bool data_from_registry = false;
     DDF_MSG_0(HIGH, "set def values");
     sns_dd_ap3216c_set_default_reg_values(ptr);

    /* Recall NV parameters, if any*/
    if(reg_params)
    {
        DDF_MSG_0(HIGH, "yes registry params available");
        if ( (reg_params->status   == SNS_DDF_SUCCESS) &&
             (reg_params->data_len >= sizeof(sns_dd_nv_db_type)))
        {
            DDF_MSG_0(HIGH, "registry params match nv db type");
            sns_dd_nv_db_type *nv_ptr = (sns_dd_nv_db_type *)reg_params->data;
            if ( (nv_ptr->lsc_num  == LSC_MAGIC_NUM) &&
                 (nv_ptr->drv_ID          == LSC_DRIVER_ID))
            {
                /* update value from NV parameters*/
                DDF_MSG_0(HIGH, "magic number for AP3216C verified");
                data_from_registry = true;
                ptr->nv_db_size = reg_params->data_len;
                
                SNS_OS_MEMCOPY(&ptr->sns_dd_alsprx_common_db.nv_db, nv_ptr, sizeof(sns_dd_nv_db_type));
                DDF_MSG_0(HIGH, "in registry params past memcopy");
                             
                sns_dd_ap3216c_set_reg_values_from_nv(ptr);
                DDF_MSG_0(HIGH, "setting defaults from nv");
            }
        }

        if (!data_from_registry) 
        {
            DDF_MSG_0(HIGH, "No registry data");
            ptr->nv_db_size = REG_FIXED_NV_SIZE_128;
            ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = 5;
            ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = 80;
            ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = 10;
            ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = SNS_DD_PRX_THRESH_NEAR;
            ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = SNS_DD_PRX_THRESH_FAR;
            ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = 1;
            ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = 1;
            ptr->sns_dd_alsprx_common_db.nv_db.lsc_num = (uint32_t)LSC_MAGIC_NUM;
            ptr->sns_dd_alsprx_common_db.nv_db.drv_ID  = (uint32_t)LSC_DRIVER_ID;
            ptr->sns_dd_alsprx_common_db.nv_db.device = 4;
            ptr->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high = 20;
            ptr->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low = 20;

            DDF_MSG_0(HIGH, "Set defaults for registry data");
            /* Writing the same values above to the part*/
            sns_dd_ap3216c_set_reg_values_from_nv(ptr);
            DDF_MSG_0(HIGH, "Registry Updated, sending the same values sent to registry to part");

           /*  SNS_OS_MEMZERO(ptr->sns_dd_alsprx_common_db.nv_db.reserved, sizeof(ptr->sns_dd_alsprx_common_db.nv_db.reserved)); (no reserved space here)*/
        }

        if (ptr->sns_dd_alsprx_common_db.nv_db.calibratePhone) 
        {
          
           ptr->calibration_mode = true;
           DDF_MSG_0(HIGH, "setting phone calib  true");
        }
        if ( (reg_params) && (!data_from_registry) ) 
        {
           DDF_MSG_0(HIGH, "before notify");
           sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT,SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
           DDF_MSG_0(HIGH, "after notify");
        }
    }

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_prx_query

DESCRIPTION   prox query
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_prx_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t*             thresh_ptr;
  uint16_t*             accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

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
      device_ranges->max = SNS_DD_PRX_RES; /* 5 cm = 0.05m */
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_RES_ADC,
                                    SNS_DD_ALSPRX_PRX_BITS);
      resp->bit_len  = SNS_DD_ALSPRX_PRX_BITS;
      resp->max_freq = SNS_DD_ALSPRX_PRX_FREQ;
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

      *resp = SNS_DD_PRX_RES;
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
      *thresh_ptr = SNS_DD_PRX_THRESH_NEAR; 
      *(uint16_t **)value = thresh_ptr; 
      DDF_MSG_0(HIGH, "get_attrib threshold");
      
      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.prox.odr;
      *(uint32_t **)value = odr_ptr;
      DDF_MSG_1(HIGH, "AP3216C get_attrib prx odr: %d",  *odr_ptr);
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
FUNCTION      sns_dd_alsprx_ap3216c_als_query

DESCRIPTION   als query
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_als_query
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t*             accu_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

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
      DDF_MSG_0(HIGH, "in get_attr range");
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(0.01);
      device_ranges->max = FX_FLTTOFIX_Q16(10000);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      DDF_MSG_0(HIGH, "in get_attr resolution ADC");
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
      DDF_MSG_0(HIGH, "in get_attr resolution");
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
      power_ptr->active_current   = SNS_DD_ALS_PWR;
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
      DDF_MSG_0(HIGH, "accuracy");
      SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                    DBG_DD_ALSPRX_GET_ATTRIB_ACC,
                                    dd_ptr->sns_dd_als_db.accuracy);
      *accu_ptr = dd_ptr->sns_dd_als_db.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.als.odr;
      *(uint32_t **)value = odr_ptr;
      DDF_MSG_1(HIGH, "AP3216C get_attrib als odr: %d",  *odr_ptr);
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
FUNCTION      sns_dd_alsprx_ap3216c_get_attrib

DESCRIPTION   sensor get attrib
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_get_attrib
(
  sns_ddf_handle_t      handle,
  sns_ddf_sensor_e      sensor,
  sns_ddf_attribute_e   attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *)handle;
  sns_ddf_status_e       status = SNS_DDF_SUCCESS;
  BOOLEAN                generic_attrib = FALSE;
  sns_ddf_registry_group_s  *reg_group;
  uint8_t                   *reg_group_data;
  sns_ddf_driver_info_s *driver_info_ptr;
  sns_ddf_driver_info_s  driver_info_l = {
    "AP3216C",  /* name */
    1           /* version */
  };

  sns_ddf_device_info_s *device_info_ptr;
  sns_ddf_device_info_s  device_info_l = {
    "ALSPRX",   /* name */
    "LSC",   /* vendor */
    "AP3216C",  /* model */
    1           /* version */
  };

  if ( (handle == NULL)      ||
       (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_DD_ALSPRX, DBG_DD_ALSPRX_GET_ATTRIB_REQ, sensor,attrib );

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

          
    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
      DDF_MSG_0(HIGH, "in attrib reg group");
      reg_group = sns_ddf_memhandler_malloc(memhandler,sizeof(*reg_group));
      if (reg_group == NULL)
      {      
        DDF_MSG_0(HIGH, "no reg group");
        return SNS_DDF_ENOMEM;
      }

      if (state->nv_db_size == 0) 
      {
        reg_group->group_data = NULL;
        reg_group->size = 0;
        DDF_MSG_0(HIGH, "nv db size 0");
      } 
      else 
      {
        reg_group_data = sns_ddf_memhandler_malloc(memhandler, state->nv_db_size);
        if (reg_group_data == NULL)
        {      
          return SNS_DDF_ENOMEM;
        }
        DDF_MSG_0(HIGH, "at memcopy");
     
        SNS_OS_MEMCOPY(reg_group_data, &state->sns_dd_alsprx_common_db.nv_db, sizeof(sns_dd_nv_db_type));
        reg_group->group_data = reg_group_data;
        reg_group->size = state->nv_db_size;
        DDF_MSG_0(HIGH, "after memcopy");
      }
      
      *value = reg_group;
      *num_elems = 1;
      generic_attrib = true;
      DDF_MSG_0(HIGH, "out of attrib group");
      return SNS_DDF_SUCCESS;
      
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
        status = sns_dd_alsprx_ap3216c_als_query(state, memhandler, attrib, value, num_elems);
        break;

      case SNS_DDF_SENSOR_PROXIMITY:
        status = sns_dd_alsprx_ap3216c_prx_query(state, memhandler, attrib, value, num_elems);
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
