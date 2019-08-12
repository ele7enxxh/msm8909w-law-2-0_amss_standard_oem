/********************************************************************************
* Copyright (c) 2013, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Init and get_attrib routines for ALS/PRX driver. These can be placed outside
   TCM if needed.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/09/14     vy      Added api to update SMGR about refactoring status
06/06/14     MW      init() function signal register fix
06/27/13     fv      Convert tmd377x driver to tmg399x
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
11/14/12     fv      Modified refrence driver to use ams/taos sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_ams_tmg399x_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
//#include "sns_dd.h"

#ifndef DDK
#include "sns_memmgr.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
//#include "sns_ddf_util.h"
#endif

#define REG_FIXED_NV_SIZE_128      128
uint32 myGPIO;
/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
static const sns_ddf_sensor_e sns_dd_alsprx_sensor_types[SNS_DD_ALSPRX_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT
};

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset
(
  sns_ddf_handle_t dd_handle
);
extern sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list;
extern void sns_dd_ams_tmg399x_alsprx_handle_irq( sns_ddf_handle_t handle, sns_ddf_irq_e irq );
extern sns_ddf_status_e ams_tmg399x_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);
extern void ams_tmg399x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state);
extern sns_ddf_status_e ams_tmg399x_calibrate_als(sns_dd_alsprx_state_t* state);
extern sns_ddf_status_e ams_tmg399x_update_registers(sns_dd_alsprx_state_t* state);


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_init

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
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_init(
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
    sns_ddf_status_e           status = SNS_DDF_SUCCESS;
    sns_dd_alsprx_state_t*     ptr;

    if ( (dd_ptr == NULL)      ||
         (num_sensors == NULL) ||
         (sensors == NULL) )
    {
        SNS_PRINTF_MEDIUM_1(10001);
        DDF_MSG_0(HIGH, "invalid param");
        return SNS_DDF_EINVALID_PARAM;
    }

	/* Update smgr sensor data for the driver to indicate uImage support        */
    sns_ddf_smgr_set_uimg_refac(smgr_handle);

	// Allocate a driver instance
    status = sns_ddf_malloc((void**)&ptr, sizeof(sns_dd_alsprx_state_t));
    if ( status != SNS_DDF_SUCCESS )
    {
        SNS_PRINTF_MEDIUM_1(10002);

        return status;
    }

    ptr->smgr_handle = smgr_handle;
    ptr->int_gpio    = device_info->first_gpio;

    // Open communication port to the device
    status = sns_ddf_open_port(&ptr->port_handle, &device_info->port_config);
    if ( status != SNS_DDF_SUCCESS )
    {
        SNS_PRINTF_MEDIUM_1(10003);

        return status;
    }

    /* // Open additional port for zero byte write (ZBW) */
    /* // for writting special function to control register */
    /* device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT; */
    /* status = sns_ddf_open_port(&ptr->ZBW_port_handle, &device_info->port_config); */
    /* if( status != SNS_DDF_SUCCESS ) */
    /* { */
    /*     SNS_PRINTF_MEDIUM_1(10004); */

    /*     return status; */
    /* } */


    // Fill out output parameters
    *num_sensors = 2;
    *sensors     = (sns_ddf_sensor_e *)sns_dd_alsprx_sensor_types;
    *dd_ptr      = (sns_ddf_handle_t) ptr;
    myGPIO = ptr->int_gpio;
#ifndef DDK
    // Check to see if IRQ's are enabled
    if(sns_ddf_signal_irq_enabled() && (ptr->int_gpio != 0xFFFF))
    {
        // IRQ's are enabled, register the irq
        status = sns_ddf_signal_register(ptr->int_gpio,
                                         *dd_ptr,
                                         &sns_ams_tmg399x_alsprx_driver_fn_list,
                                         SNS_DDF_SIGNAL_IRQ_FALLING);

        if( status != SNS_DDF_SUCCESS )
        {
            SNS_PRINTF_MEDIUM_1(10005);

            return status;
        }
    }
#endif

    // Setup timers
#ifdef DDK
    status = sns_ddf_timer_init(&ptr->sns_dd_alsprx_common_db.ddk_timer.timer_obj,
                                *dd_ptr,
                                &sns_ams_tmg399x_alsprx_driver_fn_list,
                                NULL,
                                TRUE);
    if( status != SNS_DDF_SUCCESS )
    {
        SNS_PRINTF_MEDIUM_1(10007);

        return status;
    }
#endif
    DDF_MSG_0(HIGH, "past irq and timer enable");
    ptr->calibration_mode = false;  //calibration enable flag, currently not used.
    ptr->nv_db_size = 0;            //variable to store size of nv_

    bool data_from_registry = false;
    DDF_MSG_0(HIGH, "set def values");

    // Load default values in driver - these will be overwritten if
    // registry has good values. if not, these will be written back
    // to nv_db
    status = ams_tmg399x_set_default_reg_values(ptr);
    if( status != SNS_DDF_SUCCESS )
    {
        SNS_PRINTF_MEDIUM_1(10007);

        return status;
    }

    // Recall NV parameters, if any
    if(reg_params)
    {
        DDF_MSG_0(HIGH, "yes reg params");
        if ( (reg_params->status   == SNS_DDF_SUCCESS) &&
             (reg_params->data_len >= sizeof(sns_dd_nv_db_type))
             )
        {
            DDF_MSG_0(HIGH, "reg params match nv db type");
            sns_dd_nv_db_type *nv_ptr = (sns_dd_nv_db_type *)reg_params->data;
            if ( (nv_ptr->ams_num == ams_magic_num) &&
                 (nv_ptr->drv_ID  == ams_driver_ID))
            {
                // update value from NV parameters
                DDF_MSG_0(HIGH, "magic num verified");
                //sns_dd_alsprx_common_db is defined in tmg399x_priv.h file
                data_from_registry = true;
                ptr->nv_db_size = reg_params->data_len;

                SNS_OS_MEMCOPY(&ptr->sns_dd_alsprx_common_db.nv_db, nv_ptr, sizeof(sns_dd_nv_db_type));
                DDF_MSG_0(HIGH, "in reg params past memcopy");
            }
        }

        if (!data_from_registry)
        {
            DDF_MSG_0(HIGH, "in no reg data");
            ptr->nv_db_size = REG_FIXED_NV_SIZE_128; //defined as 128 max
            ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO;
            ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = SNS_DD_IR_TRANS_RATIO;
            ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = SNS_DD_DC_OFFSET;
            ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = SNS_DD_PRX_THRESH_NEAR;
            ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = SNS_DD_PRX_THRESH_FAR;
            ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = SNS_DD_PRX_FACTOR;
            ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = SNS_DD_ALS_FACTOR;
            ptr->sns_dd_alsprx_common_db.nv_db.ams_num        = ams_magic_num;
            ptr->sns_dd_alsprx_common_db.nv_db.drv_ID         = ams_driver_ID;
            ptr->sns_dd_alsprx_common_db.nv_db.calibratePhone = 1;
            ptr->sns_dd_alsprx_common_db.nv_db.device         = AMS_SENSOR;  //set to 4 or TMD2771
            ptr->sns_dd_alsprx_common_db.nv_db.calLuxLower    = 250;      //avg set as 300
            ptr->sns_dd_alsprx_common_db.nv_db.calLuxUpper    = 500;

            // DGF, iCoefB, iCoefC, iCoefD are from table in
            // ams_tmg399x_set_default_reg_values() above.
            ptr->sns_dd_alsprx_common_db.nv_db.DGF            = ptr->chip.als_setup.DGF;
            ptr->sns_dd_alsprx_common_db.nv_db.iCoefB         = ptr->chip.als_setup.iCoefB;
            ptr->sns_dd_alsprx_common_db.nv_db.iCoefC         = ptr->chip.als_setup.iCoefC;
            ptr->sns_dd_alsprx_common_db.nv_db.iCoefD         = ptr->chip.als_setup.iCoefD;

            ptr->sns_dd_alsprx_common_db.nv_db.proxDetect     = PROX_DETECT;  //600;
            ptr->sns_dd_alsprx_common_db.nv_db.proxRelease    = PROX_RELEASE;  //400;
            ptr->sns_dd_alsprx_common_db.nv_db.luxCorrection  = 1 << 10;
            DDF_MSG_0(HIGH, "past setting defaults for reg data");
            /*  SNS_OS_MEMZERO(ptr->sns_dd_alsprx_common_db.nv_db.reserved, sizeof(ptr->sns_dd_alsprx_common_db.nv_db.reserved)); (no reserved space here)*/
        }

        // Now set the usable values
        ams_tmg399x_set_reg_values_from_nv(ptr);
        DDF_MSG_0(HIGH, "setting defaults from nv");

        if (ptr->sns_dd_alsprx_common_db.nv_db.calibratePhone)
        {
            SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4414);
            ptr->calibration_mode = true;
            DDF_MSG_0(HIGH, "setting phone calib  true");
        }

        // Notify to write back to registry in case defaults had to be loaded.
        if ( (reg_params) && (!data_from_registry) )
        {
            DDF_MSG_0(HIGH, "before notify");
            sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT,SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
            DDF_MSG_0(HIGH, "after notify");
        }

        // Write updated registers to sensor
        // Should this be done all of time?
        // no "#ifdef DDK"
        //#ifdef DDK
        status = ams_tmg399x_update_registers(ptr);
        if (status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "Failed to reset");
            //       SNS_PRINTF_MEDIUM_1(10011);
        }
        //#endif

    }

    return status;
}


/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_prx_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for proximitydistance data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_prx_query
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
      *odr_ptr   = dd_ptr->chip.setup.odr;
      /* *odr_ptr   = dd_ptr->chip.setup.prox.odr; */
      *(uint32_t **)value = odr_ptr;
      DDF_MSG_1(HIGH, "TMD277 get_attrib prx odr: %d",  *odr_ptr);
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

FUNCTION      sns_dd_ams_tmg399x_alsprx_als_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_als_query
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
      resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
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
      *odr_ptr   = dd_ptr->chip.setup.odr;
      /* *odr_ptr   = dd_ptr->chip.setup.als.odr; */
      *(uint32_t **)value = odr_ptr;
      DDF_MSG_1(HIGH, "TMD277 get_attrib als odr: %d",  *odr_ptr);
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

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_get_attrib

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
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_attrib
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
        "TMG399x",  /* name */
        1           /* version */
    };

    sns_ddf_device_info_s *device_info_ptr;
    sns_ddf_device_info_s  device_info_l = {
        "ALSPRX",   /* name */
        "ams AG",   /* vendor */
        "TMG399x",  /* model */
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
            status = sns_dd_ams_tmg399x_alsprx_als_query(state, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_PROXIMITY:
            status = sns_dd_ams_tmg399x_alsprx_prx_query(state, memhandler, attrib, value, num_elems);
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
