/******************************************************************************
* MODULE     : sns_dd_hall_bu52061.c
* FUNCTION   : Hall Effect Sensor BH52061 Driver source for Qualcomm System
*            : Device Driver Framework
* PROGRAMMED : US Design Center, Applications Engineering Team
* REMARKS    :
* AUTHOR     : Kristopher Bahar
* COPYRIGHT  : Copyright (C) 2013 - ROHM CO.,LTD. All rights reserved.
*            :
*            : Redistribution and use in source and binary forms, with or
*            : without modification, are permitted provided that the
*            : following conditions are met:
*            : 1. Redistributions of source code must retain the above
*            : copyright notice, this list of conditions and the following
*            : disclaimer.
*            : 2. Redistributions in binary form must reproduce the above
*            : copyright notice, this list of conditions and the following
*            : disclaimer in the documentation and/or other materials
*            : provided with the distribution.
*            : 3. Neither the name of ROHM CO.,LTD. nor the names of its
*            : contributors may be used to endorse or promote products
*            : derived from this software without specific prior written
*            : permission.
*            : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
*            : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*            : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*            : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*            : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*            : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*            : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*            : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*            : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*            : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*            : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*            : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*            : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*            : SUCH DAMAGE.
*****************************************************************************/

/****************************************************************************
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ****************************************************************************/
/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  05-11-2015  AH   Convert data in Q16 format
============================================================================*/
/*============================================================================

INCLUDE FILES

============================================================================*/
#define NULL (0)

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_hall_bu52061nvx.h"
#include "stdbool.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h" 
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_memmgr.h"

/*=======================================================================

INTERNAL ENUMS

========================================================================*/

/*=======================================================================

Preprocessor Definitions and Constants

========================================================================*/

/*=======================================================================

INTERNAL FUNCTION PROTOTYPES

========================================================================*/

#define BU52061_MAX_FREQ 5  // Hz
/* Delay used to send initial indication/ the current state of mag field */
#define HALL_SENSOR_INIT_IND_DELAY    (200000)  // 200 microseconds
extern sns_ddf_driver_if_s sns_hall_bu52061_driver_fn_list;

static sns_ddf_odr_t hall_odr_list[] = {

    BU52061_MAX_FREQ
};

static uint32_t num_hall_odrs = sizeof(hall_odr_list)/sizeof(hall_odr_list[0]);

/*=======================================================================

STRUCTURE DEFINITIONS

========================================================================*/
typedef struct {
    sns_ddf_handle_t		smgr_hndl;    	/* SDDI handle used to notify_data */
    uint16_t				gpio;
    uint8_t					device_selection;
    uint32_t    odr;
    sns_ddf_timer_s	  first_indication_timer;
} sns_dd_hall_state_t;

static void sns_dd_hall_bu52061_log_data(
    sns_dd_hall_state_t*		dd_ptr,
    uint32_t						sensor_status)
{
    sns_err_code_e				err_code;
    sns_log_sensor_data_pkt_s*	log_struct_ptr;

    // Allocate log packet
    err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_HALL_EFFECT; 
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ROHM;

        // Timestamp the log with sample time 
        log_struct_ptr->timestamp = sns_ddf_get_timestamp();

        // Log sensor data
        log_struct_ptr->num_data_types = 1;
        log_struct_ptr->data[0] = sensor_status;

        // Commit log (also frees up the log packet memory) 
        err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                      log_struct_ptr);
        if(err_code != SNS_SUCCESS)
        {
            // Do nothing
        }
    }
}

static sns_ddf_status_e sns_dd_hall_bu52061_handle_indication(sns_ddf_handle_t 	dd_handle)
{
    sns_ddf_status_e    	result;
    sns_dd_hall_state_t		*hall_pram;
    sns_ddf_sensor_data_s 	*data_ptr;
    uint32_t gpio_val;
    uint32_t sensor_status;

    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_handle_indication - start");

    hall_pram = (sns_dd_hall_state_t *)dd_handle;

	if (!hall_pram)
	{
		BU52061_MSG_0(ERROR,"Invalid arguments");
		return SNS_DDF_EINVALID_PARAM;
	}

    result = sns_ddf_signal_gpio_read(hall_pram->gpio, &gpio_val);

    // GPIO value = 1 means mag field is not present
    // GPIO value = 0 means mag field is present
    sensor_status = gpio_val ? MAG_FIELD_NO_DETECT : MAG_FIELD_DETECT;

    BU52061_MSG_2(MEDIUM,"gpio_val=%d sensor_status=%d", gpio_val, sensor_status);

    result = sns_ddf_malloc((void **)&data_ptr, sizeof(sns_ddf_sensor_data_s));
    if (result != SNS_DDF_SUCCESS) {
        BU52061_MSG_0(ERROR,"data_ptr mem error");
        return (SNS_DDF_ENOMEM);
    }

    data_ptr->sensor      = SNS_DDF_SENSOR_HALL_EFFECT;
    data_ptr->status      = SNS_DDF_SUCCESS;
    data_ptr->num_samples = 1;
    data_ptr->timestamp = sns_ddf_get_timestamp();

    result = sns_ddf_malloc((void **)&data_ptr->samples, sizeof(sns_ddf_sensor_sample_s));
    if (result != SNS_DDF_SUCCESS) {
        BU52061_MSG_0(ERROR,"data_ptr->samples mem error");
        return (SNS_DDF_ENOMEM);
    }

    data_ptr->samples->sample = FX_FLTTOFIX_Q16(sensor_status);
    data_ptr->samples->status = SNS_DDF_SUCCESS;

    result = sns_ddf_smgr_notify_data(hall_pram->smgr_hndl, data_ptr, 1);
    if (result != SNS_DDF_SUCCESS) {
        BU52061_MSG_1(ERROR,"sns_ddf_smgr_notify_data error=%d", result);
        return result;
    }

    sns_dd_hall_bu52061_log_data(hall_pram, sensor_status);

    BU52061_MSG_0(MEDIUM,"sns_dd_hall_bu52061_handle_indication - end");

    sns_ddf_mfree(data_ptr->samples);
    sns_ddf_mfree(data_ptr);

    return(SNS_DDF_SUCCESS); 
}

/*============================================================================

Global Data Definitions

============================================================================*/

static sns_ddf_sensor_e my_sensors[2] = { SNS_DDF_SENSOR_HALL_EFFECT, SNS_DDF_SENSOR__NONE };

/******************************************************************************
* NAME       : sns_dd_hall_bu52061_init
* FUNCTION   : Initializes the HALL determines the device to use
* DETAIL     : Allocates memory for driver state structure.
*            : Copy GPIO reference for registering magnetic field detection interrupt
*            : Calls sns_dd_hall_bu52061_reset routine
* param[out] : dd_handle_ptr : Pointer that this function must malloc and
*            :                 populate. This is a handle to the driver
*            :                 instance that will be passed in to all other 
*            :                 functions.
* param[in]  : smgr_handle   : Handle used to identify this driver when it
*            :                 calls into Sensors Manager functions.
* param[in]  : nv_params     : NV parameters retrieved for the driver.
* param[in]  : device_info   : Information describing each of the physical
*            :                 devices controlled by this driver. This is used 
*            :                 to configure the bus and talk to the device.
* param[in]  : memhandler    : Memory handler used to dynamically allocate
*            :                 output parameters, if applicable. NB: Do not use
*            :                 memhandler to allocate memory for @a 
*            :                 dd_handle_ptr.
* param[in]  : num_devices   : Length of @a device_info.
* param[out] : sensors       : List of supported sensors, populated and
*            :                 returned by this function.
* param[out] : num_sensors   : Length of @a sensors.
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or 
*            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
* TODO       :
* REMARKS    :
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_init(
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
    sns_ddf_status_e   stat = SNS_DDF_SUCCESS;
    sns_dd_hall_state_t *ptr;

    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_init");

    /* Sanity Checks for Output Parameters*/
    if (dd_handle_ptr == NULL || sensors == NULL || num_sensors == NULL) {
        return(SNS_DDF_EINVALID_PARAM);
    }

    /* get mag state memory */
    stat = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_hall_state_t));
    if (stat != SNS_DDF_SUCCESS) {
        return (SNS_DDF_ENOMEM);
    }

    ptr->smgr_hndl = smgr_handle;

    /* Fill out supported sensor info */
    *num_sensors   = 2;
    *sensors       = (sns_ddf_sensor_e *)my_sensors;
    *dd_handle_ptr = (sns_ddf_handle_t)ptr;

    BU52061_MSG_0(MEDIUM,"sns_dd_hall_bu52061_init");

    //Copy the GPIO value from device info and store this in state variable gpio
    //if "first_gpio" == to 0xFFFF then there is no GPIO pin that can be assigned to this part and return error
    if(device_info->first_gpio == 0xFFFF){
        BU52061_MSG_1(ERROR,"first_gpio=%d", device_info->first_gpio);
        return(SNS_DDF_EINVALID_PARAM);
    }
    else{
        ptr->gpio = device_info->first_gpio;
    }

    //Save the Device_Select Info for use in Set/Get Attrib for Driver Name, Etc...
    ptr->device_selection = device_info->device_select;

    //init timer
    stat = sns_ddf_timer_init( 
        &ptr->first_indication_timer,
        ( sns_ddf_handle_t )ptr,
        &sns_hall_bu52061_driver_fn_list, 
        ptr, //always return dd_handle_ptr
        false);
    if(stat != SNS_DDF_SUCCESS)
    {
        BU52061_MSG_1(ERROR,"sns_ddf_timer_init error=%d", stat);
        return stat;
    }
    return (stat);
}

static sns_ddf_status_e sns_dd_hall_bu52061_get_data
    (
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s** data
    )
{
    return SNS_DDF_EBUS;
}

/******************************************************************************
* NAME       : sns_dd_hall_bh1721_set_attr
* FUNCTION   : Sets an attribute of hall effect sensor
* DETAIL     : Called by SMGR to set certain device attributes that are
*            : programmable. Since this has no settings, this is only there to
*			  : interface with QC DDF
* param[in]  : dd_handle : Handle to a driver instance.
* param[in]  : sensor    : Sensor for which this attribute is to be set.
* param[in]  : attrib    : Attribute to be set.
* param[in]  : value     : Value to set this attribute.
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EDEVICE or
*            : SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
* TODO       :
* REMARKS    : Most cases will only return SUCCESS as this part is only connected via 
interrupt/GPIO
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_set_attr 
    ( 
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
    )
{
    sns_dd_hall_state_t *hall_pram;

    hall_pram = (sns_dd_hall_state_t *)dd_handle;
    /* basic sanity check */
    if (dd_handle == NULL || value == NULL) return SNS_DDF_EINVALID_PARAM;

    BU52061_MSG_0(MEDIUM,"sns_dd_hall_bu52061_set_attr");
    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RANGE:
        /* Does not support different ranges */
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_ODR:
        hall_pram->odr = *(uint32_t *)value;
        return SNS_DDF_SUCCESS;

    default:
        return SNS_DDF_EINVALID_PARAM;
    }

    return SNS_DDF_SUCCESS;

}

/******************************************************************************
* NAME       : sns_dd_hall_bh1721_get_attr
* FUNCTION   : Called by the SMGR to retrieves the value of an attribute of
*            : the sensor.
* param[in]  : dd_handle   : Handle to a driver instance.
* param[in]  : sensors     : List of sensors for which data is requested.
* param[in]  : attrib      : Attribute to be retrieved.
* param[in]  : memhandler  : Memory handler used to dynamically allocate
*            :               output parameters, if applicable.
* param[out] : value       : Pointer that this function will allocate or set
*            :               to the attribute's value.
* param[out] : num_elems   : Number of elements in @a value.
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or
*            : SNS_DDF_EINVALID_ATTR
* TODO       : SNS_DDF_ATTRIB_ODR case is empty.  DDF is unclear about what variable to set here.
* REMARKS    :
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_get_attr
    (
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems
    )
{
    sns_dd_hall_state_t *hall_pram;
    sns_ddf_status_e   result = SNS_DDF_SUCCESS;

    BU52061_MSG_0(MEDIUM,"sns_dd_hall_bu52061_get_attr");

    /* basic sanity check */
    if (dd_handle == NULL || memhandler == NULL || value == NULL ||
        num_elems == NULL) return SNS_DDF_EINVALID_PARAM;

    hall_pram = (sns_dd_hall_state_t *)dd_handle;

    //if(hall_pram->device_selection == NULL){
    //	return(SNS_DDF_EINVALID_PARAM);
    //}

    switch(attrib)
    {
    case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power_attrib;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            power_attrib                   = *value;
            *num_elems                     = 1;

            if(hall_pram->device_selection == BU52053NVX)
            {
               power_attrib->active_current   = HALL_SENSOR_BU52053NVX_POWER_VAL;
               power_attrib->lowpower_current = HALL_SENSOR_BU52053NVX_LOW_POWER_VAL;
            }
            else
            {
               power_attrib->active_current   = HALL_SENSOR_POWER_VAL;
               power_attrib->lowpower_current = HALL_SENSOR_LOW_POWER_VAL;
            }
            break;
        }

    case SNS_DDF_ATTRIB_RANGE:
        {
            //unsigned short  data;
            sns_ddf_range_s *device_ranges;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            device_ranges      = *value;
            *num_elems = 1;

            //For Device Ranges, I set Min = 0 and Max = 1
            device_ranges->min = FX_FLTTOFIX_Q16(HALL_SENSOR_RANGE_MIN);
            device_ranges->max = FX_FLTTOFIX_Q16(HALL_SENSOR_RANGE_MAX);
            break;
        }

    case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *device_delay;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            device_delay = *value;
            *num_elems   = 1;

            device_delay->time_to_active = 100;
            device_delay->time_to_data   = 0;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s *device_res;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            device_res          = *value;
            *num_elems          = 1;

            device_res->bit_len = 1;	// No ADC, GPIO pin only, 0 or 1, thus bit length of "1"
            device_res->max_freq = BU52061_MAX_FREQ;
            break;
        }


    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t      *resolution;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            resolution = *value;
            *num_elems  = 1;

            //"Smallest unit the sensor device can measure.  THis refers to the absolute maximum resolution the device can measure"
            *resolution = FX_FLTTOFIX_Q16(1);
            break;				
        }

    case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            /*
            Driver INFO
            Name: This name can be more Generic (ROHM Omnipolar Hall ICs)
            */
            sns_ddf_driver_info_s *driver_info;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            driver_info          = *value;
            *num_elems           = 1;

            driver_info->name    = "ROHM_Hall_Effect";
            driver_info->version = 1;
            break;
        }

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            /*
            Device_info
            Name: Hall Effect Sensor
            Vendor: ROHM
            Model: PN
            -For this we need to look at the “device_select” number and have that correspond to the different PNs
            -Devices will be mapped in the header
            --This active driver should be  indexed as “0”
            */
            sns_ddf_device_info_s *device_info;

            *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s));
            if (*value == NULL) {
                return (SNS_DDF_ENOMEM);
            }
            device_info          = *value;
            *num_elems           = 1;

            switch(hall_pram->device_selection)
            {
            case BU52061NVX:
                device_info->model   = "BU52061NVX";
                break;
            case BU52054GWZ:
                device_info->model   = "BU52054GWZ";
                break;						
            case BU52055GWZ:
                device_info->model   = "BU52055GWZ";
                break;
            case BU52015GUL:
                device_info->model   = "BU52015GUL";
                break;
            case BU52001GUL:
                device_info->model   = "BU52001GUL";
                break;
            case BU52053NVX:
                device_info->model   = "BU52053NVX";
                break;
            case BU52056NVX:
                device_info->model   = "BU52056NVX";
                break;
            case BU52011HFV:
                device_info->model   = "BU52011HFV";
                break;
            case BU52021HFV:
                device_info->model   = "BU52021HFV";
                break;
            case BU52025G:
                device_info->model   = "BU52025G";
                break;
            case BD7411G:
                device_info->model   = "BD7411G";
                break;
            default:
                device_info->model   = "BU52053NVX";
                break;
            }
            device_info->name    = "ROHM_Hall_Effect";
            device_info->vendor  = "ROHM_SEMICONDUCTOR";
            device_info->version = 1;
            break;
        }
    case SNS_DDF_ATTRIB_ODR:
        {
            uint32_t *odr;
            odr = sns_ddf_memhandler_malloc(memhandler,	sizeof(*odr));
            if (odr == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            *odr = hall_pram->odr;
            *value = odr;
            *num_elems = 1;
            break;           
        }
    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
        {

           *value = &hall_odr_list;
           *num_elems = num_hall_odrs;
        }
		break;

    default:
        return (SNS_DDF_EINVALID_ATTR);
    }
    BU52061_MSG_0(MEDIUM,"sns_dd_hall_bu52061_get_attr - end");
    return result;
}

/******************************************************************************
* NAME       : sns_dd_hall_bu52061_handle_irq
* FUNCTION   : Called when IRQ is triggered
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise or SNS_DDF_ENOMEM or
*            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
* DETAIL     : This driver is currently set up so that this handle triggers when 
*				a magnetic field is detected (falling edge).
* TODO       : Qualcomm has not fully defined the DDF for Hall Sensors... After talking with QC, we should just 
*				return a data sample of "5" when this interrupt triggers.  When DDF is updated, we can change this
*				back to whatever they want returned...
* REMARKS    : 
*****************************************************************************/
static void sns_dd_hall_bu52061_handle_irq(
    sns_ddf_handle_t 	dd_handle, 
    uint32_t 			gpio_num,
    sns_ddf_time_t timestamp)
{
    sns_ddf_status_e    	result;
    sns_dd_hall_state_t *hall_pram;

    if(dd_handle == NULL)
        return;

    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_handle_irq ");
    
    hall_pram = (sns_dd_hall_state_t *)dd_handle;
    
    result = sns_dd_hall_bu52061_handle_indication(dd_handle);

    if(result != SNS_DDF_SUCCESS)
    {
        BU52061_MSG_1(ERROR,"sns_dd_hall_bu52061_handle_indication ERROR=%d", result);
    }
}

/******************************************************************************
* NAME       : sns_dd_hall_bu52061_reset
* FUNCTION   : Initialize system
* DETAIL     : reset IC
* param[in]  : state: Ptr to the driver structure
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise or SNS_DDF_ENOMEM or
*            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
* TODO       : 
* REMARKS    : Reset will not do anything to the IC since it is only connected via
GPIO pin... but remains to keep structure.  Will always return SNS_DDF_SUCCESS
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_reset(sns_ddf_handle_t dd_handle)
{
    //sns_ddf_status_e stat;
    sns_dd_hall_state_t *hall_pram;

    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_reset");

    hall_pram = (sns_dd_hall_state_t *)dd_handle;

	if (!hall_pram)
	{
		BU52061_MSG_0(ERROR,"Invalid arguments");
		return SNS_DDF_EINVALID_PARAM;
	}

    hall_pram->odr = 0;
    return (SNS_DDF_SUCCESS);
}

/******************************************************************************
* NAME       : sns_dd_hall_bu52061_enable_sched_data
* FUNCTION   : Called by the SMGR to enable/disable scheduled data
* DETAIL     : This function will register/de-register the GPIO IRQ
* param[in]  : dd_handle   : Handle to a driver instance.
* param[in]  : sensors     : List of sensors for which data is requested.
* param[in]  : enable		: Bool value denoting enable or disable or IRQ
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or
*            : SNS_DDF_EINVALID_ATTR
* TODO       : 
* REMARKS    :
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_enable_sched_data
    (
    sns_ddf_handle_t    dd_handle,
    sns_ddf_sensor_e	   sensor,
    bool				enable
    )
{	
    sns_dd_hall_state_t *hall_pram;
    sns_ddf_status_e   result = SNS_DDF_SUCCESS;
    //uint32_t gpio_val;

    hall_pram = (sns_dd_hall_state_t *)dd_handle;

    BU52061_MSG_2(HIGH,"sns_dd_hall_bu52061_enable_sched_data enable=%d sensor=%d", enable, sensor);

    if(!hall_pram || hall_pram->gpio == 0xFFFF){
        return(SNS_DDF_EINVALID_PARAM);
    }

    //result = sns_ddf_signal_gpio_read(hall_pram->gpio, &gpio_val);
    //BU52061_MSG_1(HIGH,"gpio_val=%d", enable, sensor);

    if(enable == 1){
        result = sns_ddf_signal_register( 
            hall_pram->gpio,
            hall_pram,
            &sns_hall_bu52061_driver_fn_list,
            SNS_DDF_SIGNAL_IRQ_DUAL_EDGE);

        if(result != SNS_DDF_SUCCESS){
            BU52061_MSG_0(ERROR,"sns_dd_hall_bu52061_enable_sched_data - IRQ Register Failed");
            return result;
        }

        /* send initial indication/the present state of mag field */
        sns_ddf_timer_start(hall_pram->first_indication_timer, HALL_SENSOR_INIT_IND_DELAY);
    }
    else{
        result = sns_ddf_signal_deregister(hall_pram->gpio);
        if(result != SNS_DDF_SUCCESS){
            BU52061_MSG_0(ERROR,"sns_dd_hall_bu52061_enable_sched_data - IRQ De-Register Failed");
            return result;
        }
    }

    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_enable_sched_data end");

    return result;
}

/******************************************************************************
* NAME       : sns_dd_hall_bu52061_probe
* FUNCTION   : Called by the SMGR to determine is the part is present using the 
current configuration.
* DETAIL     : used to check if the proper device has been selected
* param[in]  : device_info   : Information describing each of the physical
*            :                 devices controlled by this driver. This is used 
*            :                 to configure the bus and talk to the device.
* param[in]  : memhandler  : Memory handler used to dynamically allocate
*            :               output parameters, if applicable.
* param[in]  : sensors     : List of sensors for which data is requested.
* param[in]  : num_sensors : Length of @a sensors.
* RETURN     : The error code definition within the DDF
*            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or
*            : SNS_DDF_EINVALID_ATTR
* TODO       : 
* REMARKS    : Probe will not do anything to the IC since it is only connected via
GPIO pin... but remains to keep structure.  Will always return SNS_DDF_SUCCESS
*****************************************************************************/
static sns_ddf_status_e sns_dd_hall_bu52061_probe
    (
    sns_ddf_device_access_s  	device_info[],
    sns_ddf_memhandler_s*		memhandler,
    uint32_t*        			num_sensors,
    sns_ddf_sensor_e*			sensors[]
)
{

	if (!sensors)
	{
		BU52061_MSG_0(ERROR,"Invalid arguments");
		return SNS_DDF_EINVALID_PARAM;
	}

    /* Fill out supported sensor info */
    *num_sensors   = 2;
    *sensors       = (sns_ddf_sensor_e *)my_sensors;

    return (SNS_DDF_SUCCESS);
}

void sns_dd_hall_bu52061_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
    sns_ddf_status_e    	result;
    sns_dd_hall_state_t *hall_pram;

    if(dd_handle == NULL)
        return;

    hall_pram = (sns_dd_hall_state_t *)dd_handle;
    
    BU52061_MSG_0(HIGH,"sns_dd_hall_bu52061_handle_timer ");

    result = sns_dd_hall_bu52061_handle_indication(dd_handle);

    if(result != SNS_DDF_SUCCESS)
    {
        BU52061_MSG_1(ERROR,"sns_dd_hall_bu52061_handle_indication ERROR=%d", result);  
    }
}

sns_ddf_driver_if_s sns_hall_bu52061_driver_fn_list =
{
    .init = &sns_dd_hall_bu52061_init,
    .get_data = &sns_dd_hall_bu52061_get_data,
    .set_attrib = &sns_dd_hall_bu52061_set_attr,
    .get_attrib = &sns_dd_hall_bu52061_get_attr,
    .handle_timer = &sns_dd_hall_bu52061_handle_timer,
    .handle_irq = &sns_dd_hall_bu52061_handle_irq,
    .reset = &sns_dd_hall_bu52061_reset,
    .run_test = NULL,
    .enable_sched_data = &sns_dd_hall_bu52061_enable_sched_data, 
    .probe = &sns_dd_hall_bu52061_probe,
    NULL // get FIFO data
};

