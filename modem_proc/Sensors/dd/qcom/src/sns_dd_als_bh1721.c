/******************************************************************************
 * MODULE     : sns_dd_als_bh1721.c
 * FUNCTION   : Ambient light Sensor BH1721 Driver source for Qualcomm Sensors
 *            : Device Driver Framework
 * PROGRAMMED : Sensor system Development Group, ROHM CO.,LTD.
 * REMARKS    :
 * AUTHOR     : Masafumi Seike
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
/*******************************************************************************
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  07/09/2015  AH   Fix compiler warnings
=============================================================================*/
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
#include "sns_dd_als_bh1721.h"
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
/* Supported datatypes */
/* for resolution */
typedef enum {
    ALS_AUTO_RESOLUTION = 0,
    ALS_HIGH_RESOLUTION,
    ALS_LOW_RESOLUTION,
    ALS_RESO_NUM_MAX,
    ALS_RESO_NON_USE = 0xFF
} ALS_RESOLUTION_TYPE;

/* for timing range value */
typedef enum {
    ALS_RANGE_TIME_MIN = 0,
    ALS_RANGE_TIME_TYP,
    ALS_RANGE_TIME_MAX,
    ALS_RANGE_NUM_MAX
} ALS_RANGE_TIME_TYPE;

/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/
#if 0 // unused variables
static const unsigned short sns_dd_als_attr_range[ALS_RANGE_NUM_MAX] =
{
    ALS_RANGE_TIME_MIN_VAL,
    ALS_RANGE_TIME_TYP_VAL,
    ALS_RANGE_TIME_MAX_VAL
};

static const uint32_t sns_dd_als_resolution[ALS_RESO_NUM_MAX] =
{
    CMD_AUTO_RESOLUTION,
    CMD_HIGH_RESOLUTION,
    CMD_LOW_RESOLUTION
};
#endif
/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_als_bh1721_reset(sns_ddf_handle_t dd_handle);
		
static sns_ddf_status_e sns_dd_als_bh1721_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_als_bh1721_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_als_bh1721_set_attr( 
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_als_bh1721_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static sns_ddf_status_e sns_dd_als_bh1721_probe(
		sns_ddf_device_access_s  	device_info[],
		sns_ddf_memhandler_s*  		memhandler,
		uint32_t*              		num_sensors,
		sns_ddf_sensor_e*       	sensors[]);		
		
static void sns_dd_als_bh1721_handle_timer(
    sns_ddf_handle_t dd_handle, 
    void* arg);

static sns_ddf_status_e als_driver_init(sns_ddf_handle_t handle, unsigned short data);
static sns_ddf_status_e als_driver_reset(sns_ddf_handle_t handle);
static sns_ddf_status_e als_driver_write_timing(sns_ddf_handle_t handle, unsigned short data);
static sns_ddf_status_e als_driver_power_on_off(sns_ddf_handle_t handle, sns_ddf_powerstate_e data);
static sns_ddf_status_e als_driver_measure_mode(sns_ddf_handle_t handle, unsigned char data);
static sns_ddf_status_e als_driver_rohm_write(sns_ddf_handle_t handle, unsigned char data);
static sns_ddf_status_e als_driver_rohm_read(sns_ddf_handle_t handle, unsigned short *data);
static sns_ddf_status_e als_driver_calc_data(sns_ddf_handle_t handle, unsigned short data, q16_t *out_data);

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_als_bh1721_driver_fn_list =
{
  .init = &sns_dd_als_bh1721_init,
  .get_data = &sns_dd_als_bh1721_get_data,
  .set_attrib = &sns_dd_als_bh1721_set_attr,
  .get_attrib = &sns_dd_als_bh1721_get_attr,
  .handle_timer = &sns_dd_als_bh1721_handle_timer,
  .handle_irq = NULL, // handle irq
  .reset = &sns_dd_als_bh1721_reset,
  .run_test = NULL, // self test
  .enable_sched_data = NULL, // enable_sched_data
  .probe = &sns_dd_als_bh1721_probe,
  NULL // get FIFO data
};

/* data structure for NV items */
typedef struct
{
  uint8_t	reserved_1;  
  uint8_t	reserved_2;  
  uint16_t	reserved_3;  
  uint16_t	reserved_4;      
  uint16_t	reserved_5;      
  uint16_t	reserved_6;      
  uint16_t	als_factor;      /* ALS multiplicative factor in % */
  uint32_t	rohm_num;        /* unique number for Rohm to identify reg items */
  uint32_t	reserved_7[28];	
} sns_dd_nv_db_t;

/* State struct for accel driver */
typedef struct {
    sns_ddf_handle_t		smgr_hndl;    /* SDDI handle used to notify_data */
    sns_ddf_powerstate_e    power_state;
    unsigned char			measure_mode;
    unsigned short			timing;
	uint32_t				measurement_time;
	sns_ddf_timer_s			timer_obj;
	bool					measurement_in_progress;
	sns_dd_nv_db_t			als_nv_db;
	uint16_t				als_nv_db_size;
    sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
} sns_dd_als_state_t;


/*============================================================================

  Global Data Definitions

 ============================================================================*/
static sns_ddf_sensor_e my_sensors[2] = { SNS_DDF_SENSOR__NONE, SNS_DDF_SENSOR_AMBIENT };

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_log_sensor_data
 * FUNCTION   : Logs data packets
 * DETAIL     : 
 * param[in]  : state: Ptr to the driver structure
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise or SNS_DDF_ENOMEM or
 *            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
 * TODO       : Check if open and close port operation is required here?
 * REMARKS    :
 *****************************************************************************/
void sns_dd_als_bh1721_log_sensor_data(
   sns_dd_als_state_t*    dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1_q16,
   uint32_t               raw_data )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_log_sensor_data");

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ROHM;

     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();

     /* Log sensor data */
	 /* Two data items for both humidity and temperature sensor types */
     log_struct_ptr->num_data_types = 2;
     log_struct_ptr->data[0] = raw_data;
     log_struct_ptr->data[1] = data1_q16;
 
     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
	 if(err_code != SNS_SUCCESS)
	 {
		 BH1721_MSG_0(ERROR,"Error in sns_logpkt_commit");
	 }	
   }
   else
   {
  	   BH1721_MSG_1(ERROR,"sns_dd_als_bh1721_log_sensor_data - error=%d",err_code);
   }
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_reset
 * FUNCTION   : Initialize system
 * DETAIL     : reset IC
 * param[in]  : state: Ptr to the driver structure
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise or SNS_DDF_ENOMEM or
 *            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
 * TODO       : Check if open and close port operation is required here?
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e sns_dd_als_bh1721_reset(sns_ddf_handle_t dd_handle)
{
    sns_ddf_status_e stat;

    sns_dd_als_state_t *als_pram;

	BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_reset");
	
    als_pram = (sns_dd_als_state_t *)dd_handle;
    stat = als_driver_init(als_pram, ALS_INIT_SET_TIMING);
    if (stat == SNS_DDF_SUCCESS) {
        als_pram->power_state  = SNS_DDF_POWERSTATE_LOWPOWER;
        als_pram->timing       = ALS_INIT_SET_TIMING;
        als_pram->measure_mode = ALS_RESO_NON_USE;
		als_pram->measurement_time = 24000;	// 24 ms
		als_pram->measurement_in_progress = false;
    }
    return (stat);
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_init
 * FUNCTION   : Initializes the als determines the device to use
 * DETAIL     : Allocates memory for driver state structure.
 *            : Opens the device port by calling sns_ddf_open_port routine
 *            : Calls sns_dd_als_bh1721_reset routine
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
static sns_ddf_status_e sns_dd_als_bh1721_init(
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
    sns_ddf_status_e   stat;
    sns_dd_als_state_t *ptr;
	bool data_from_registry = false;

    /* get als state memory */
    stat = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_als_state_t));
    if ((stat != SNS_DDF_SUCCESS) || (ptr == NULL)) {
        return (SNS_DDF_ENOMEM);
    }

    ptr->smgr_hndl = smgr_handle;

    stat = sns_ddf_open_port(&(ptr->port_handle), &(device_info->port_config));
    if (stat != SNS_DDF_SUCCESS) {
        sns_ddf_mfree((void **)&ptr);
        return (stat);
    }

    BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_init");

	stat = sns_ddf_timer_init( &ptr->timer_obj, 
								ptr, 
								&sns_als_bh1721_driver_fn_list, 
								ptr, //always return dd_handle_ptr
								false); // not periodic
	  if(stat != SNS_DDF_SUCCESS)
	  {
		  BH1721_MSG_0(ERROR,"sns_dd_als_bh1721_init - timer init failed");
		  return stat;
	  }

	/* Fill out supported sensor info */
    *num_sensors   = 2;
    *sensors       = &my_sensors[0];
    *dd_handle_ptr = (sns_ddf_handle_t)ptr;
	
	stat = sns_dd_als_bh1721_reset(ptr);
    if (stat != SNS_DDF_SUCCESS) {
        sns_ddf_close_port(ptr->port_handle);
        sns_ddf_mfree((void **)&ptr);
    }
	
	// Recall NV parameters, if any
    if(nv_params)
    {
        BH1721_MSG_0(MEDIUM, "sns_dd_als_bh1721_init - registry available");
        if ( (nv_params->status   == SNS_DDF_SUCCESS) &&
             (nv_params->data_len >= sizeof(sns_dd_nv_db_t)))
        {
            BH1721_MSG_0(MEDIUM, "Registry params match nv db type");

            sns_dd_nv_db_t *nv_ptr = (sns_dd_nv_db_t *)nv_params->data;

            if (nv_ptr->rohm_num  == SNS_DD_ROHM_NUM)
            {
                // update value from NV parameters
                BH1721_MSG_0(MEDIUM, "Number matches Rohm number");
                
                data_from_registry = true;
                ptr->als_nv_db_size = nv_params->data_len;
                
                SNS_OS_MEMCOPY(&ptr->als_nv_db, nv_ptr, sizeof(sns_dd_nv_db_t));
            }
        }

        if (!data_from_registry) 
        {
            BH1721_MSG_1(MEDIUM, "Loading DD's registry data - Size = %d", sizeof(sns_dd_nv_db_t));
            ptr->als_nv_db_size = 128; //128 bytes
            ptr->als_nv_db.reserved_1 = 0;
            ptr->als_nv_db.reserved_2 = 0;
            ptr->als_nv_db.reserved_3 = 0;
			ptr->als_nv_db.reserved_4 = 0;
			ptr->als_nv_db.reserved_5 = 0;
			ptr->als_nv_db.reserved_6 = 0;
            ptr->als_nv_db.als_factor  = SNS_DD_ALS_FACTOR;
            ptr->als_nv_db.rohm_num = SNS_DD_ROHM_NUM;
			
			SNS_OS_MEMZERO(ptr->als_nv_db.reserved_7, 28*sizeof(uint32_t));       

			BH1721_MSG_0(HIGH, "Set defaults for registry data");
        }

        // Notify to write back to registry in case defaults had to be loaded.
        if ( (nv_params) && (!data_from_registry) ) 
        {
           sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT,SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
           BH1721_MSG_0(HIGH, "Reg Notify done");
        }
    }
		
	/* Sanity Checks for Output Parameters*/
	if (*num_sensors == 0) {
		sns_ddf_close_port(ptr->port_handle);
        sns_ddf_mfree((void **)&ptr);
		stat = SNS_DDF_EINVALID_PARAM;
	}
	if (*sensors == NULL) {
		sns_ddf_close_port(ptr->port_handle);
        sns_ddf_mfree((void **)&ptr);
		stat = SNS_DDF_EINVALID_PARAM;
	}
	if (*dd_handle_ptr == NULL) {
		sns_ddf_close_port(ptr->port_handle);
        sns_ddf_mfree((void **)&ptr);
		stat = SNS_DDF_EINVALID_PARAM;
	}
	
    return (stat);
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_set_attr
 * FUNCTION   : Sets an attribute of ambient light sensor
 * DETAIL     : Called by SMGR to set certain device attributes that are
 *            : programmable. Curently its the power mode and range.
 * param[in]  : dd_handle : Handle to a driver instance.
 * param[in]  : sensor    : Sensor for which this attribute is to be set.
 * param[in]  : attrib    : Attribute to be set.
 * param[in]  : value     : Value to set this attribute.
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EDEVICE or
 *            : SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM
 * TODO       :
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e sns_dd_als_bh1721_set_attr 
( 
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
)
{
    sns_ddf_status_e   result;
    sns_dd_als_state_t *als_pram;

    BH1721_MSG_2(MEDIUM,"sns_dd_als_bh1721_set_attr - attrib=%d, value = %d",attrib,*(uint32_t *)value);
	
	als_pram = (sns_dd_als_state_t *)dd_handle;
    result = SNS_DDF_SUCCESS;

    switch (attrib) {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            /* set power mode */
            result = als_driver_power_on_off(als_pram, *(sns_ddf_powerstate_e *)value);
            if (result == SNS_DDF_SUCCESS) {
                als_pram->power_state  = *(sns_ddf_powerstate_e *)value;
                als_pram->measure_mode = ALS_RESO_NON_USE;
            }
        }
        break;

    case SNS_DDF_ATTRIB_RANGE:
        {
            uint8_t index;
			index = *(uint8_t *)value;
			
			//Set the Resolution mode (H, Auto, L)
			if( index == 0 )
			{
				// H-reso
				BH1721_MSG_0(MEDIUM,"H-Reso");
				result = als_driver_measure_mode(als_pram, CMD_HIGH_RESOLUTION);
                if (result == SNS_DDF_SUCCESS) {
                    als_pram->measure_mode  = CMD_HIGH_RESOLUTION;
					//als_pram->measurement_time = 180000*(als_pram->timing)/300;
					als_pram->measurement_time = (600*(als_pram->timing))+1000;
                }
			}
			else if ( index == 1 )
			{
				// Auto-reso
				BH1721_MSG_0(MEDIUM,"Auto-Reso");
				result = als_driver_measure_mode(als_pram, CMD_AUTO_RESOLUTION);
                if (result == SNS_DDF_SUCCESS) {
                    als_pram->measure_mode  = CMD_AUTO_RESOLUTION;
					//als_pram->measurement_time = 180000*(als_pram->timing)/300;
					als_pram->measurement_time = (600*(als_pram->timing))+1000;
                }
			}
			else
			{
				// L-reso
				BH1721_MSG_0(MEDIUM,"L-Reso");
				result = als_driver_measure_mode(als_pram, CMD_LOW_RESOLUTION);
                if (result == SNS_DDF_SUCCESS) {
                    als_pram->measure_mode  = CMD_LOW_RESOLUTION;
					//als_pram->measurement_time = 24000*(als_pram->timing)/300;
					als_pram->measurement_time = (80*(als_pram->timing))+1000;
                }
			}
        }
        break;
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            /* No operation */
        }
        break;
        
    default:
        result = SNS_DDF_EINVALID_ATTR;
        break;
    }

    BH1721_MSG_1(MEDIUM,"sns_dd_als_bh1721_set_attr - result = %d", result);

	return (result);
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_get_data
 * FUNCTION   : Called by the SMGR to get data
 * DETAIL     : Requests a single sample of sensor data from each of the
 *            : specified sensors. Data is returned immediately after being
 *            : read from the sensor, in which case data[] is populated
 *            : in the same order it was requested,
 * param[in]  : dd_handle   : Handle to a driver instance.
 * param[in]  : sensors     : List of sensors for which data isrequested.
 * param[in]  : num_sensors : Length of @a sensors.
 * param[in]  : memhandler  : Memory handler used to dynamically allocate
 *            :               output parameters, if applicable.
 * param[out] : data        : Sampled sensor data. The number of elements
 *            :               must match @a num_sensors.
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or 
 *            : SNS_DDF_EDEVICE or SNS_DDF_EBUS or SNS_DDF_EINVALID_PARAM 
 * TODO       : Update the each sample status after a check for bound is done
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e sns_dd_als_bh1721_get_data
(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[]
)
{
    sns_ddf_status_e      result;
    sns_dd_als_state_t*   als_pram;
    
    BH1721_MSG_2(MEDIUM,"sns_dd_als_bh1721_get_data - start, num_sen=%d, sensor=%d",num_sensors,sensors[0]);
	
	als_pram = (sns_dd_als_state_t *)dd_handle;

    /* Sanity check*/
    if (sensors[0] != my_sensors[1]) {
        BH1721_MSG_0(ERROR,"sns_dd_als_bh1721_get_data - invalid param");
		return (SNS_DDF_EINVALID_PARAM);
    }
    
	if( als_pram->measurement_in_progress == true )
	{
		BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_get_data - busy!");
		return SNS_DDF_EDEVICE_BUSY;
	}
	
	als_pram->measurement_in_progress = true;

	/* Start the timer */	
	BH1721_MSG_1(MEDIUM,"sns_dd_als_bh1721_get_data - timer %d",als_pram->measurement_time);

	result = sns_ddf_timer_start(als_pram->timer_obj, als_pram->measurement_time);
	if(result != SNS_DDF_SUCCESS)
	{
		BH1721_MSG_0(ERROR,"sns_dd_als_bh1721_get_data - timer start failed");
		return result;
	}
	
	return (SNS_DDF_PENDING);
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_get_attr
 * FUNCTION   : Called by the SMGR to retrieves the value of an attribute of
 *            : the sensor.
 * DETAIL     : range and resolution info is from the device data sheet.
 * param[in]  : dd_handle   : Handle to a driver instance.
 * param[in]  : sensors     : List of sensors for which data isrequested.
 * param[in]  : attrib      : Attribute to be retrieved.
 * param[in]  : memhandler  : Memory handler used to dynamically allocate
 *            :               output parameters, if applicable.
 * param[out] : value       : Pointer that this function will allocate or set
 *            :               to the attribute's value.
 * param[out] : num_elems   : Number of elements in @a value.
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or
 *            : SNS_DDF_EINVALID_ATTR
 * TODO       :
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e sns_dd_als_bh1721_get_attr
(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems
)
{
    sns_dd_als_state_t *als_pram;
    sns_ddf_status_e   result;
    
    BH1721_MSG_1(MEDIUM,"sns_dd_als_bh1721_get_attr - %d", attrib);
	
	als_pram = (sns_dd_als_state_t *)dd_handle;

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
                power_attrib->active_current   = ALS_SENSOR_POWER_VAL;
                power_attrib->lowpower_current = ALS_SENSOR_LOW_POWER_VAL;
                *num_elems                     = 1;
            }
            break;

        case SNS_DDF_ATTRIB_RANGE:
            {
                unsigned short  data;
                sns_ddf_range_s *device_ranges;
                
                *value = sns_ddf_memhandler_malloc(memhandler, 3*sizeof(sns_ddf_range_s));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                device_ranges      = *value;

                device_ranges[0].min = FX_FLTTOFIX_Q16(ALS_SENSOR_RANGE_MIN);
				device_ranges[1].min = FX_FLTTOFIX_Q16(ALS_SENSOR_RANGE_MIN);
				device_ranges[2].min = FX_FLTTOFIX_Q16(ALS_SENSOR_RANGE_MIN);
				
				// For H- reso mode
				data = ALS_SENSOR_RANGE_MAX1;
                result = als_driver_calc_data(als_pram, data, &device_ranges[0].max);
                if (result == SNS_DDF_EDATA_BOUND) {
                    device_ranges[0].max = FX_MAX_Q16;
                }

				// For Auto- reso mode
				data = ALS_SENSOR_RANGE_MAX2;
                result = als_driver_calc_data(als_pram, data, &device_ranges[1].max);
                if (result == SNS_DDF_EDATA_BOUND) {
                    device_ranges[1].max = FX_MAX_Q16;
                }
				
				// For L- reso mode
				data = ALS_SENSOR_RANGE_MAX2;
				result = als_driver_calc_data(als_pram, data, &device_ranges[2].max);
                if (result == SNS_DDF_EDATA_BOUND) {
                    device_ranges[2].max = FX_MAX_Q16;
                }
                *num_elems = 3;
            }
            break;

        case SNS_DDF_ATTRIB_DELAYS:
            {
                sns_ddf_delays_s *device_delay;
                
                *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                device_delay = *value;
                *num_elems   = 1;
                
                device_delay->time_to_active = 3000; // 3ms is the startup time
                //Worst Case Delay means.. MTREG = Max Value = 1020
				//device_delay->time_to_data   = 180000*((als_pram->timing)/300);
				device_delay->time_to_data   = 612000;
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
            {
                sns_ddf_resolution_adc_s *device_res;
                
                *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                device_res          = *value;
                *num_elems          = 1;

                device_res->bit_len = 16;	// ADC resolution is 16-bit
				device_res->max_freq = 134;	// Max Frequency Value = 1 / (16ms * 140 / 300)  at Mtreg =140, L-res mode               
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION:
            {
                unsigned char data;
                q16_t         *update_data;
                
                *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                update_data = *value;
                *num_elems  = 1;
                // Min resolution supported
                data = 1;

                result = als_driver_calc_data(als_pram, (unsigned short)data, update_data);
                if (result == SNS_DDF_EDATA_BOUND) {
                    *update_data = FX_MAX_Q16;
                }
            }
            break;

        case SNS_DDF_ATTRIB_DRIVER_INFO:
            {
                sns_ddf_driver_info_s *driver_info;
                
                *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                driver_info          = *value;
                *num_elems           = 1;
                driver_info->name    = "ROHM BH1721FVC DRIVER";
                driver_info->version = 2;
            }
            break;

        case SNS_DDF_ATTRIB_DEVICE_INFO:
            {
                sns_ddf_device_info_s *device_info;
                
                *value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s));
                if (*value == NULL) {
                    return (SNS_DDF_ENOMEM);
                }
                device_info          = *value;
                *num_elems           = 1;
                device_info->name    = "Ambient Light Sensor";
                device_info->vendor  = "ROHM SEMICONDUCTOR";
                device_info->model   = "BH1721FVC";
                device_info->version = 2;
            }
            break;
                    
        case SNS_DDF_ATTRIB_REGISTRY_GROUP:
			{
				sns_ddf_registry_group_s	*reg_group;
				uint8_t						*reg_group_data;
				
				reg_group = sns_ddf_memhandler_malloc(memhandler,sizeof(*reg_group));
				if (reg_group == NULL)
				{      
					BH1721_MSG_0(HIGH, "no reg group");
					return SNS_DDF_ENOMEM;
				}

				if (als_pram->als_nv_db_size == 0) 
				{
					reg_group->group_data = NULL;
					reg_group->size = 0;
				} 
				else 
				{
					reg_group_data = sns_ddf_memhandler_malloc(memhandler, als_pram->als_nv_db_size);
					if (reg_group_data == NULL)
					{      
						return SNS_DDF_ENOMEM;
					}
     
					SNS_OS_MEMCOPY(reg_group_data, &als_pram->als_nv_db, sizeof(sns_dd_nv_db_t));
					reg_group->group_data = reg_group_data;
					reg_group->size = als_pram->als_nv_db_size;
				}
      
				*value = reg_group;
				*num_elems = 1;
						  
				break;
			}

		default:
            return (SNS_DDF_EINVALID_ATTR);
    }

    BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_get_attr - end");

	return (SNS_DDF_SUCCESS);
}

/******************************************************************************
 * NAME       : sns_dd_als_bh1721_probe
 * FUNCTION   : Called by the SMGR to determin is the part is present using the 
				current configuration.
 * DETAIL     : range and resolution info is from the device data sheet.
 * param[in]  : device_info   : Information describing each of the physical
 *            :                 devices controlled by this driver. This is used 
 *            :                 to configure the bus and talk to the device.
 * param[in]  : memhandler  : Memory handler used to dynamically allocate
 *            :               output parameters, if applicable.
 * param[in]  : sensors     : List of sensors for which data isrequested.
 * param[in]  : num_sensors : Length of @a sensors.
 * RETURN     : The error code definition within the DDF
 *            : SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_ENOMEM or
 *            : SNS_DDF_EINVALID_ATTR
 * TODO       : 
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e sns_dd_als_bh1721_probe
(
    sns_ddf_device_access_s  	device_info[],
	sns_ddf_memhandler_s*		memhandler,
	uint32_t*        			num_sensors,
	sns_ddf_sensor_e*			sensors[]
)
{	
	/* Fill out supported sensor info */
    *num_sensors   = 2;
    *sensors       = &my_sensors[0];
	
    return (SNS_DDF_SUCCESS);
}

static void sns_dd_als_bh1721_handle_timer(
    sns_ddf_handle_t dd_handle, 
    void* arg)
{
	sns_ddf_status_e      result;
    unsigned short        als_data;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_als_state_t    *als_pram;

    BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_handle_timer - start");
	
	als_pram = (sns_dd_als_state_t *)dd_handle;
	als_pram->measurement_in_progress = false;
	
	result = als_driver_rohm_read(als_pram, &als_data);
    if (result != SNS_DDF_SUCCESS) {
        return;
    }
    
	result = sns_ddf_malloc((void **)&data_ptr, sizeof(sns_ddf_sensor_data_s));
	
    if (result != SNS_DDF_SUCCESS) {
        return;
    }
    
    data_ptr->sensor      = my_sensors[1];
    data_ptr->status      = SNS_DDF_SUCCESS;
    data_ptr->timestamp   = sns_ddf_get_timestamp();
    data_ptr->num_samples = 1;
    result = sns_ddf_malloc((void **)&data_ptr->samples, sizeof(sns_ddf_sensor_sample_s));
	    
	//Sanity Check for Data Output
	if (result != SNS_DDF_SUCCESS) {
        return;
    }
    data_ptr->samples->status = als_driver_calc_data(als_pram, als_data, &data_ptr->samples->sample);
	
	result = sns_ddf_smgr_notify_data (als_pram->smgr_hndl, data_ptr, 1);

	/* Log data packet */
	sns_dd_als_bh1721_log_sensor_data(als_pram, SNS_DDF_SENSOR_AMBIENT, 
						(uint32_t)data_ptr->samples->sample,
						(uint32_t)als_data );
	
	sns_ddf_mfree(data_ptr->samples);
    sns_ddf_mfree(data_ptr);

	BH1721_MSG_0(MEDIUM,"sns_dd_als_bh1721_handle_timer - end");
}
/*****************************************************************************
 *                              access function                              *
 ****************************************************************************/
/*****************************************************************************
 * NAME       : als_driver_init
 * FUNCTION   : initialize BH1721
 * REMARKS    :
 ****************************************************************************/
static sns_ddf_status_e als_driver_init(sns_ddf_handle_t handle, unsigned short data)
{
    sns_ddf_status_e result;

    result = als_driver_reset(handle);
    if (result == SNS_DDF_SUCCESS) {
        result = als_driver_write_timing(handle, data);
    }

	BH1721_MSG_1(MEDIUM,"als_driver_init - result = %d",result);

    return (result);
}

/*****************************************************************************
 * NAME       : als_driver_reset
 * FUNCTION   : reset BH1721 register
 * REMARKS    :
 ****************************************************************************/
static sns_ddf_status_e als_driver_reset(sns_ddf_handle_t handle)
{
    sns_ddf_status_e result;

    /* set soft ware reset */
    result = als_driver_rohm_write(handle, CMD_POWER_DOWN);

	BH1721_MSG_1(MEDIUM,"als_driver_reset - result = %d",result);

    return (result);
}

/******************************************************************************
 * NAME       : als_driver_write_timing
 * FUNCTION   : write the timing value ato IC
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_write_timing(sns_ddf_handle_t handle, unsigned short data)
{
    sns_ddf_status_e result;
    unsigned char    low_data;
    unsigned char    high_data;

    if ((data < ALS_RANGE_TIME_MIN_VAL) || (data > ALS_RANGE_TIME_MAX_VAL)) {
        return (SNS_DDF_EINVALID_PARAM);
    }
    high_data = (unsigned char)(((data & 0x3FF) >> 5) | CMD_HIGH_MESURE_TIME);
    low_data  = (unsigned char)((data & 0x1F) | CMD_LOW_MESURE_TIME);
    /* write register to BH1721 via i2c */
    result = als_driver_rohm_write(handle, high_data);
    if (result == SNS_DDF_SUCCESS) {
        result = als_driver_rohm_write(handle, low_data);
    }

	BH1721_MSG_1(MEDIUM,"als_driver_write_timing - result = %d",result);

    return (result);
}

/******************************************************************************
 * NAME       : als_driver_power_on_off
 * FUNCTION   : power on and off BH1721
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_power_on_off(sns_ddf_handle_t handle, sns_ddf_powerstate_e data)
{
    unsigned char    als_ctl;
    sns_ddf_status_e result;

    /* check the parameter of als power */
    if (data > SNS_DDF_POWERSTATE_LOWPOWER) {
        return (SNS_DDF_EINVALID_PARAM);
    }
    /* set ALS control register */
    if (data == SNS_DDF_POWERSTATE_ACTIVE) {
        als_ctl = CMD_POWER_ON;
    } else {
        als_ctl = CMD_POWER_DOWN;
    }
    /* write register to BH1721 via i2c */
    result = als_driver_rohm_write(handle, als_ctl);

	BH1721_MSG_1(MEDIUM,"als_driver_power_on_off - result = %d",result);

    return (result);
}

/******************************************************************************
 * NAME       : als_ioctl_measure_mode
 * FUNCTION   : write the timing value to light integration time control register
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_measure_mode(sns_ddf_handle_t handle, unsigned char data)
{
    sns_ddf_status_e result;

	if (data != CMD_AUTO_RESOLUTION) {
        if (data != CMD_HIGH_RESOLUTION) {
            if (data != CMD_LOW_RESOLUTION) {
                return (SNS_DDF_EINVALID_PARAM);
            }
        }
    }
    /* write register to BH1721 via i2c */
    result = als_driver_rohm_write(handle, data);

    BH1721_MSG_1(MEDIUM,"als_driver_measure_mode - result = %d",result);

	return (result);
}

/******************************************************************************
 * NAME       : als_driver_rohm_write
 * FUNCTION   : read data BH1721
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_rohm_write(sns_ddf_handle_t handle, unsigned char data)
{
    sns_ddf_status_e   result;
    unsigned char      dummy;
    unsigned char      read_count;
    sns_dd_als_state_t *als_pram;

    als_pram = (sns_dd_als_state_t *)handle;
    /* write register to BH1721 via i2c */
    result = sns_ddf_read_port(als_pram->port_handle, data, &dummy, 1, &read_count);
	
	BH1721_MSG_1(MEDIUM,"als_driver_rohm_write - result = %d",result);

	return (result);
}

/******************************************************************************
 * NAME       : als_driver_rohm_read
 * FUNCTION   : read data BH1721
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_rohm_read(sns_ddf_handle_t handle, unsigned short *data)
{
    sns_ddf_status_e   result;
    unsigned short     read_data;
    unsigned char      dummy;
    unsigned char      read_count;
    sns_dd_als_state_t *als_pram;

    als_pram = (sns_dd_als_state_t *)handle;
    dummy    = (unsigned char)((als_pram->timing & 0x1F) | CMD_LOW_MESURE_TIME);
    /* write register to BH1721 via i2c */
    result = sns_ddf_read_port(als_pram->port_handle, dummy, (unsigned char *)&read_data, 2, &read_count);
	
   	if (result == SNS_DDF_SUCCESS) {
        *data = CONVERT_TO_BE(read_data);
    }
	else
	{
		*data = 0;
	}

    BH1721_MSG_3(MEDIUM,"als_driver_rohm_read - data_before_BE=%d, data_after_BE=%d, result = %d",read_data, *data, result);

	return (result);
}

/******************************************************************************
 * NAME       : als_driver_calc_data
 * FUNCTION   : read the value of ALS data and status in BH1721
 * REMARKS    :
 *****************************************************************************/
static sns_ddf_status_e als_driver_calc_data(sns_ddf_handle_t handle, unsigned short data, q16_t *out_data)
{
    unsigned long      dividents;
    unsigned long      overplus;
    unsigned long      change_overplus;
    unsigned long      numerator;
    unsigned long      divisors;
    unsigned long      comp;
    sns_dd_als_state_t *als_pram;
    sns_ddf_status_e   result;

    als_pram = (sns_dd_als_state_t *)handle;
    if (als_pram->timing == 0) {
        result    = SNS_DDF_EDATA_BOUND;
        *out_data = 0;
    } 
	else {
		if(als_pram->als_nv_db.als_factor == 0){
			result    = SNS_DDF_EDATA_BOUND;
            *out_data = 0;
		}
		else
		{
			divisors  = (als_pram->timing) * (als_pram->als_nv_db.als_factor);
			numerator = 25000;	// =(1 / 1.2 * 300) * 100
			dividents = data * numerator;
			comp      = dividents / divisors;
			
			if (comp > 0x7FFF) {
				result    = SNS_DDF_EDATA_BOUND;
				*out_data = FX_MAX_Q16;
			} 
			else {
				result          = SNS_DDF_SUCCESS;
				overplus        = dividents - (divisors * comp);
				change_overplus = overplus << FX_QFACTOR;
				change_overplus = change_overplus / divisors;
				*out_data       = (comp << FX_QFACTOR) | change_overplus;
			}
		}
	}

    BH1721_MSG_2(MEDIUM,"als_driver_calc_data - raw_data=%d comp=%d",data,comp);

	return (result);
}
