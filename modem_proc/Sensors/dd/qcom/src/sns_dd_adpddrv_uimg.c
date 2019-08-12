/********************************************************************************
* Copyright (c) 2014, Analog Devices, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of the Analog Devices nor the
*        names of its contributors may be used to endorse or promote products
*        derived from this software without specific prior written permission.
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

#include <stdio.h>
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_dd_adpddrv.h"

#include "sns_log_api_public.h"
#include "sns_log_types_public.h"

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_log_data: Log the latest sensor data
*			Single sample of sensor data is to be saved for log sensor data.
*       
*
* @param[in]  sensor_type   sensor type.
* @param[in]  hrm_slota   	Sampled slot a sensor data.
* @param[in]  hrm_slotb   	Sampled slot b sensor data.
* @param[in]  hrm_slotb   	sensor skin contact information.
* @param[out] void          void	
*/
/*===========================================================================*/

void sns_dd_hrm_adpddrv_log_data(
   sns_ddf_sensor_e		sensor_type,
   uint32_t				hrm_slota,
   uint32_t				hrm_slotb,
   uint32_t				contact)
{
	sns_err_code_e				err_code;
	sns_log_sensor_data_pkt_s*	log_struct_ptr;
	log_pkt_t					log_pkt_type;   

#if USE_DRAGONBOARD_BUILD
	log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
	log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif
	
	/* Allocate log packet */
	err_code = sns_logpkt_malloc(log_pkt_type,
								 sizeof(sns_log_sensor_data_pkt_s),
								 (void**)&log_struct_ptr);

	if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
	{
		log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
		log_struct_ptr->sensor_id = sensor_type;  // Need to be assigned properly.
		log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ADI;

		/* Time stamp the log with sample time */
		log_struct_ptr->timestamp = sns_ddf_get_timestamp();

		log_struct_ptr->num_samples=1;

		/* Log sensor data */
		log_struct_ptr->num_data_types = 3;
		log_struct_ptr->data[0] = hrm_slota;
		log_struct_ptr->data[1] = hrm_slotb;
		log_struct_ptr->data[2] = contact; // contact information , assume always in contact.
      
		/* Commit log (also frees up the log packet memory) */
		err_code = sns_logpkt_commit(log_pkt_type, log_struct_ptr);
		if(err_code != SNS_SUCCESS)
		{
			ADPDDrv_MSG_0(ERROR,"sns_logpkt_commit - error");
		}
	}
	else
	{
		ADPDDrv_MSG_1(ERROR,"sns_dd_hrm_adux1020_log_data - NO MEMORY with err: %d", err_code);
	}
}
/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_get_data: Requests a single sample of sensor data. 
*       Data is returned immediately after being read from the sensor.
*
* @param[in]  dd_handle    Handle to driver instance.
* @param[in]  sensors      List of sensors for which data is requested.
* @param[in]  num_sensors  Length of @a sensors.
* @param[in]  memhandler   Memory handler used to dynamically allocate 
*                          output parameters, if applicable.
* @param[out] data         Sampled sensor data.

* @return SNS_DDF_SUCCESS if data was populated successfully, error code 
*         otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_hrm_adpddrv_get_data(
	sns_ddf_handle_t        dd_handle,
	sns_ddf_sensor_e        sensors[],
	uint32_t                num_sensors,
	sns_ddf_memhandler_s*   memhandler,
	sns_ddf_sensor_data_s** data)
{
	sns_dd_hrm_state_t		*adpddrv_handler = (sns_dd_hrm_state_t *)dd_handle;
	sns_ddf_status_e		ret = SNS_DDF_SUCCESS;
	sns_ddf_sensor_data_s	*data_ptr;
	sns_ddf_sensor_sample_s *sample_ptr;
	uint16_t				reg_val[2];
	uint8_t					fifo_data[SLOT_AB_DATA_SIZE];
	uint8_t					fifo_sizebytes = 0;
	uint8_t                 nbytes;
	uint32_t				data_out[8] = { 0 };
	uint8_t					idx = 0;
	uint8_t					data_idx = 0;
		
	/* basic sanity check */
	if (dd_handle == NULL || sensors == NULL || num_sensors != 1)
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	if(adpddrv_handler->datasetsize > sizeof(fifo_data))
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	// read REG_INT_STATUS to check fifo count
	reg_val[0] = 0;
	reg_val[1] = 0;
	ret = sns_ddf_read_port(adpddrv_handler->port_handle, REG_INT_STATUS,
							              (uint8_t *)&reg_val[0], 2, &nbytes);
	
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
	{
		return ret;
	}
	fifo_sizebytes = reg_val[0];
	if (fifo_sizebytes < adpddrv_handler->datasetsize)
	{
		ADPDDrv_MSG_1(MED,"Error Incomplete fifo data= %d", reg_val[0]);
		ret = SNS_DDF_EINVALID_DATA;
		return ret;
	}
#ifdef ADPD_UIMAGE_MODE
	*data = sns_ddf_memhandler_malloc_ex(memhandler, 
									   num_sensors * sizeof(sns_ddf_sensor_data_s), dd_handle);
	if(*data == NULL)
	{
		return SNS_DDF_ENOMEM;
	}
	if ((sample_ptr = sns_ddf_memhandler_malloc_ex(memhandler,
											     3 * (sizeof(sns_ddf_sensor_sample_s)), dd_handle)) == NULL)
	{
		return SNS_DDF_ENOMEM;
	}
#else /* uImage_Mode */
	*data = sns_ddf_memhandler_malloc(memhandler, 
									   num_sensors * sizeof(sns_ddf_sensor_data_s));
	if(*data == NULL)
	{
		return SNS_DDF_ENOMEM;
	}
	if ((sample_ptr = sns_ddf_memhandler_malloc(memhandler,
											     3 * (sizeof(sns_ddf_sensor_sample_s)))) == NULL)
	{
		return SNS_DDF_ENOMEM;
	}
#endif /* uImage_Mode */

	data_ptr = *data;
	
	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_get_data");

	if(fifo_sizebytes > 64)
		ADPDDrv_MSG_1(MED,"Fifo size = %d", fifo_sizebytes);
	/* read data output - read all 16 bytes
	 Discard first two sample always - they may be garbage */
	ret = sns_ddf_read_port(adpddrv_handler->port_handle, REG_DATA_BUFFER,
					(uint8_t *)&fifo_data[0], adpddrv_handler->datasetsize, &nbytes);
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != adpddrv_handler->datasetsize)) {
		ADPDDrv_MSG_1(ERROR,"Read port failed, Read count: %d", nbytes);
		return ret;
	}
	for (idx=0; idx<(adpddrv_handler->datasetsize-1); idx+=2)
	{
		data_out[data_idx++] = (((uint16_t)fifo_data[idx] << 8) | ((uint16_t)fifo_data[idx+1] & 0x00FF));
	}
	if(adpddrv_handler->slotmode == ADPDDrv_SLOT_B)
		for (idx = 0; idx < 4; idx++)
		{
			data_out[idx+4] = data_out[idx];
			data_out[idx] = 0;
		}

	//  ESG sensor team puts Slot B (GREEN) into sample_ptr[0].

	sample_ptr[1].sample = (data_out[0] + data_out[1] + data_out[2] + data_out[3]);
	sample_ptr[1].status = SNS_DDF_SUCCESS;
	sample_ptr[0].sample = (data_out[4] + data_out[5] + data_out[6] + data_out[7]);
	sample_ptr[0].status = SNS_DDF_SUCCESS;
    // contact information that the sensor is alway in contact
	sample_ptr[2].sample = ~0;  
	sample_ptr[2].status = SNS_DDF_SUCCESS;
	
	ADPDDrv_MSG_2(MED,"A=%d B= %d", sample_ptr[0].sample, sample_ptr[1].sample);
	data_ptr->status = SNS_DDF_SUCCESS;
	data_ptr->num_samples = 3;
	data_ptr->sensor = sns_dd_hrm_adpddrv_sensor_types[0];
	data_ptr->timestamp = sns_ddf_get_timestamp();
	data_ptr->samples = sample_ptr;
	sns_dd_hrm_adpddrv_log_data(sns_dd_hrm_adpddrv_sensor_types[0],
								(uint32_t)sample_ptr[1].sample,
								(uint32_t)sample_ptr[0].sample,
								(uint32_t)sample_ptr[2].sample);
	return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_hrm_adpddrv_handle_timer: Timer handler function
*
* @param[in] dd_handle  Handle to driver instance.
* @param[in] arg     	arg is a pointer the same memory location with which 
*						the timer object was created and may be set to NULL 
*						if not needed
*
*  @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
void sns_dd_hrm_adpddrv_handle_timer(
	sns_ddf_handle_t dd_handle,
	void* arg)
{
	ADPDDrv_MSG_0(HIGH,"sns_dd_hrm_adpddrv_handle_timer");
}


/*===========================================================================*/
/*!
* @brief sns_dd_hrm_adpddrv_handle_irq ADPDDrv interrupt handler
*
*@detail
*This function will be called within the context of the Interrupt 
*Service Task (IST), *not* the ISR.

*@param[in] handle		Handle to a driver instance. 
*@param[in] gpio_num	The gpio_num representing the interrupt that occurred.
*@param[in] timestamp	

*@return None 
*/
/*===========================================================================*/
void sns_dd_hrm_adpddrv_handle_irq(
	sns_ddf_handle_t handle,
	uint32_t gpio_num,
	sns_ddf_time_t timestamp)
{
	sns_dd_hrm_state_t		*adpddrv_handler = (sns_dd_hrm_state_t *)handle;
	sns_ddf_status_e		ret = SNS_DDF_SUCCESS;
	uint16_t				reg_val[2];
	uint8_t					fifo_data[SLOT_AB_DATA_SIZE];
	uint8_t					fifo_sizebytes = 0;
	uint8_t					nbytes;
	static uint32_t			data_out[8] = { 0 };
	uint8_t					idx = 0;
	uint8_t					data_idx = 0;
	
	if(adpddrv_handler->datasetsize > sizeof(fifo_data))
	{
		return;
	}
	
	// read REG_INT_STATUS
	reg_val[0] = 0;
	reg_val[1] = 0;
	ret = sns_ddf_read_port(adpddrv_handler->port_handle, REG_INT_STATUS,
							              (uint8_t *)&reg_val[0], 2, &nbytes);
	
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
	{
		return;
	}
	
	fifo_sizebytes = reg_val[0];
	if (fifo_sizebytes < adpddrv_handler->datasetsize)
	{
		//ADPDDrv_MSG_1(MED,"Error Incomplete fifo data= %d", reg_val[0]);
		return;
	}
	if(fifo_sizebytes > 64)
		ADPDDrv_MSG_1(MED,"Fifo size = %d", fifo_sizebytes);	
	// read 16 bytes of data
	ret = sns_ddf_read_port(adpddrv_handler->port_handle, REG_DATA_BUFFER,
					(uint8_t *)&fifo_data[0], adpddrv_handler->datasetsize, &nbytes);
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != adpddrv_handler->datasetsize))
	{
		ADPDDrv_MSG_1(ERROR,"Read port failed, Read count: %d", nbytes);
		return;
	}
	memset(data_out, 0, 8 * sizeof(uint32_t));
	for (idx=0; idx<(adpddrv_handler->datasetsize-1); idx+=2)
	{
		data_out[data_idx++] = (((uint16_t)fifo_data[idx] << 8) | ((uint16_t)fifo_data[idx+1] & 0x00FF));
	}
	if(adpddrv_handler->slotmode == ADPDDrv_SLOT_B)
		for (idx = 0; idx < 4; idx++)
		{
			data_out[idx+4] = data_out[idx];
			data_out[idx] = 0;
		}
	

	//  ESG sensor team puts Slot B (GREEN) into sample_ptr[0].
	adpddrv_handler->samples[1].sample = (data_out[0] + data_out[1] + data_out[2] + data_out[3]);
	adpddrv_handler->samples[1].status = SNS_DDF_SUCCESS;
	adpddrv_handler->samples[0].sample = (data_out[4] + data_out[5] + data_out[6] + data_out[7]);			
	adpddrv_handler->samples[0].status = SNS_DDF_SUCCESS;
   // contact information that the sensor is alway in contact
	adpddrv_handler->samples[2].sample = ~0;			
	adpddrv_handler->samples[2].status = SNS_DDF_SUCCESS;

	ADPDDrv_MSG_2(MED,"A = %d B = %d", adpddrv_handler->samples[0].sample,
				  adpddrv_handler->samples[1].sample);
	
	adpddrv_handler->s_data[0].status = SNS_DDF_SUCCESS;
	adpddrv_handler->s_data[0].num_samples = 1;
	adpddrv_handler->s_data[0].sensor = sns_dd_hrm_adpddrv_sensor_types[0];
	adpddrv_handler->s_data[0].timestamp = sns_ddf_get_timestamp();
	adpddrv_handler->s_data[0].samples = adpddrv_handler->samples;
	
	ret = sns_ddf_smgr_notify_data(adpddrv_handler->smgr_handle, adpddrv_handler->s_data, 1);
	if (ret != SNS_DDF_SUCCESS)
	{
		ADPDDrv_MSG_0(ERROR,"Data notification failed");
		return;
	}
	sns_dd_hrm_adpddrv_log_data(sns_dd_hrm_adpddrv_sensor_types[0],
								(uint32_t)adpddrv_handler->samples[1].sample,
								(uint32_t)adpddrv_handler->samples[0].sample,
								(uint32_t)adpddrv_handler->samples[2].sample);
	// clear interrupt by writing 1 to REG_INT_STATUS
	ret = sns_ddf_write_port(adpddrv_handler->port_handle, REG_INT_STATUS, 
			                      (uint8_t*) &reg_val[0], 2, &nbytes);
	
	if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
	{
		ADPDDrv_MSG_0(ERROR,"write port failed");
		return;
	}
}