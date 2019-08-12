/**********************************************************************
* This software is released under the BSD license as follows.
* Copyright (c) 2014, Murata Manufacturing Co.,Ltd.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following 
* conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following
*        disclaimer in the documentation and/or other materials
*        provided with the distribution.
*     3. Neither the name of the Murata Manufacturing Co.,Ltd. nor the
*        names of its contributors may be used to endorse or 
*        promote products derived from this software without specific
*        prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/
/******************* Murata Manufacturing Co.,Ltd. 2014 ***************
 *
 *  Filename:   sns_dd_press_ZPA2326.c
 *
 *  Purpose:    Interface between SMGR and ZPA2326 core driver.
 *
 *  $Author: mplcsds1 $
 *
 *  $Date: 2016/12/13 $
 *
 *  $Revision: #1 $
 *
 * *******************************************************************/

#include "sns_dd_press_ZPA2326.h"
#include "sns_dd_press_ZPA2326_core.h"
#include "sns_dd_press_ZPA2326_ifi2c.h"


/**********************************************************************
STATIC VARIABLE DEFINITIONS
**********************************************************************/

static sns_ddf_sensor_e gATM_ZPA2326_SENSORS[] =
{
	SNS_DDF_SENSOR_PRESSURE ,
	SNS_DDF_SENSOR_TEMP
};

static sns_ddf_power_info_s gATM_ZPA2326_POWER_INFO =
{
	.active_current = ATM_SENSOR_ZPA2326_ACTIVE_CURRENT ,
	.lowpower_current = ATM_SENSOR_ZPA2326_LOWPOWER_CURRENT ,
};

static sns_ddf_range_s gATM_ZPA2326_RANGE_PRESS =
{
	.min = ATM_SENSOR_ZPA2326_PRESS_RANGE_MIN ,
	.max = ATM_SENSOR_ZPA2326_PRESS_RANGE_MAX ,
};

static sns_ddf_range_s gATM_ZPA2326_RANGE_TEMP =
{
	.min = ATM_SENSOR_ZPA2326_TEMP_RANGE_MIN ,
	.max = ATM_SENSOR_ZPA2326_TEMP_RANGE_MAX ,
};

static sns_ddf_resolution_t \
gATM_ZPA2326_RESOLUTION_PRESS = ATM_SENSOR_ZPA2326_PRESS_RESOLUTION ;
static sns_ddf_resolution_t \
gATM_ZPA2326_RESOLUTION_TEMP = ATM_SENSOR_ZPA2326_TEMP_RESOLUTION ;


static sns_ddf_resolution_adc_s gATM_ZPA2326_RESOLUTION_ADC_PRESS =
{
	.bit_len = ATM_SENSOR_ZPA2326_PRESS_RESOLUTION_ADC_LEN ,
	.max_freq = ATM_SENSOR_ZPA2326_PRESS_RESOLUTION_ADC_FREQ ,
};


static sns_ddf_resolution_adc_s gATM_ZPA2326_RESOLUTION_ADC_TEMP =
{
	.bit_len = ATM_SENSOR_ZPA2326_TEMP_RESOLUTION_ADC_LEN ,
	.max_freq = ATM_SENSOR_ZPA2326_TEMP_RESOLUTION_ADC_FREQ ,
};

static sns_ddf_lowpass_freq_t \
gATM_ZPA2326_LOWPASS[ATM_SENSOR_ZPA2326_LOWPASS_ELEMS_NUM] =
{
	ATM_SENSOR_ZPA2326_LOWPASS_ODR1 ,
	ATM_SENSOR_ZPA2326_LOWPASS_ODR2 ,
	ATM_SENSOR_ZPA2326_LOWPASS_ODR3 ,
	ATM_SENSOR_ZPA2326_LOWPASS_ODR4 ,
};


static sns_ddf_driver_info_s gATM_ZPA2326_DRIVER_INFO =
{
	.name = ATM_SENSOR_ZPA2326_DRIVER_INFO_NAME ,
	.version = ATM_SENSOR_ZPA2326_DRIVER_INFO_VER ,
};


static sns_ddf_device_info_s gATM_ZPA2326_DEVICE_INFO =
{
	.name = ATM_SENSOR_ZPA2326_DEVICE_INFO_NAME ,
	.vendor = ATM_SENSOR_ZPA2326_DEVICE_INFO_VENDOR ,
	.model = ATM_SENSOR_ZPA2326_DEVICE_INFO_MODEL ,
	.version = ATM_SENSOR_ZPA2326_DEVICE_INFO_VER ,
};

static unsigned char \
gATM_ZPA2326_ODR[ATM_SENSOR_ZPA2326_LOWPASS_ELEMS_NUM] =
{
	ATM_SENSOR_ZPA2326_REG_ODR1 ,
	ATM_SENSOR_ZPA2326_REG_ODR2 ,
	ATM_SENSOR_ZPA2326_REG_ODR3 ,
	ATM_SENSOR_ZPA2326_REG_ODR4 ,
};


typedef struct {
	sns_ddf_handle_t port_handle;	/* Handle for accessing I2C bus */
	sns_ddf_handle_t smgr_handle;	/* Handle for sns_ddf_notify_data */
	signed int pressure;			/* latest pressure data */
	signed int temperature;			/* latest temperature data */

} sns_dd_atm_zpa2326_state_t;


/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_init
*
*  Description  : Initilize Pressure Sensor.
*  Arguments    : I/O) sns_ddf_handle_t					*dd_handle_ptr
*                 In)  sns_ddf_handle_t					smgr_handle
*                 In)  sns_ddf_nv_params_s				*nv_params
*                 In)  sns_ddf_device_access_s			device_info[]
*                 In)  uint32_t							num_devices
*                 In)  sns_ddf_memhandler_s				*memhandler
*                 I/O) sns_ddf_sensor_e					*sensors[]
*                 I/O) uint32_t							*num_sensors
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
static  sns_ddf_status_e  sns_dd_atm_zpa2326_init (
	sns_ddf_handle_t* dd_handle_ptr,
	sns_ddf_handle_t smgr_handle,
	sns_ddf_nv_params_s* nv_params,
	sns_ddf_device_access_s device_info[],
	uint32_t num_devices,
	sns_ddf_memhandler_s* memhandler,
	sns_ddf_sensor_e* sensors[],
	uint32_t* num_sensors)
{
	signed int ret ;
	sns_dd_atm_zpa2326_state_t *state_handle;

	PRINT_INFO("START","");
	/*** Check if arguments is NULL/ No Data ***/
	if( ( dd_handle_ptr == NULL ) || ( smgr_handle == NULL )
	|| ( nv_params == NULL ) || ( device_info == NULL )
	|| ( num_devices == 0U ) || ( memhandler == NULL ) 
	|| ( sensors == NULL ) || ( num_sensors == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM ;
	}

	/*** Allocate Device Driver Handler ***/
	ret = atm_sensor_ifsys_alloc( \
		(uint16_t)sizeof(sns_dd_atm_zpa2326_state_t) ,\
		(void**)(&state_handle)) ;
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("ALLOCATE FAIL","[ret:%d]",ret);
		return SNS_DDF_ENOMEM ;
	}
	state_handle->smgr_handle = smgr_handle ;

	/*** Initilize I2C Port */
	ret = atm_sensor_ifi2c_init(&(state_handle->port_handle),\
												device_info) ;
	if( ret != ATM_SENSOR_I2C_OK )
	{
		PRINT_ERR_1("I2C INIT FAIL","[ret:%d]",ret);
		return SNS_DDF_EBUS ;
	}

	/*** Initialize Sensor ***/
	ret = atm_sensor_init( &(state_handle->port_handle) );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SENSOR INIT FAIL","[ret:%d]",ret);
		
		/*** Exit I2C Port ***/
		ret = atm_sensor_ifi2c_exit( &(state_handle->port_handle) );
		if( ret != ATM_SENSOR_I2C_OK )
		{
			PRINT_ERR_1("I2C EXIT FAIL","[ret:%d]",ret);
		}
		return SNS_DDF_EDEVICE ;
	}

	/*** Set the Sensor Information ***/
	*num_sensors = ATM_SENSOR_ZPA2326_NUM_SENSORS;
	*sensors = gATM_ZPA2326_SENSORS;
	*dd_handle_ptr = state_handle ;

	PRINT_INFO("END","");
	return SNS_DDF_SUCCESS;
}

/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_get_data
*
*  Description  : Get Sensor Data.
*  Arguments    : In)  sns_ddf_handle_t					dd_handle
*                 In)  sns_ddf_sensor_e					sensors[]
*                 In)  uint32_t							num_sensors
*                 In)  sns_ddf_memhandler_s				*memhandler
*                 I/O) sns_ddf_sensor_data_s			**data
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
static  sns_ddf_status_e  sns_dd_atm_zpa2326_get_data(
	sns_ddf_handle_t dd_handle,
	sns_ddf_sensor_e sensors[],
	uint32_t num_sensors,
	sns_ddf_memhandler_s* memhandler,
	sns_ddf_sensor_data_s** data)
{
	uint8_t i ;
	signed int ret ;
	signed int pressure = 0 ;
	signed int temperature = 0 ;
	sns_ddf_time_t timestamp;
	sns_ddf_sensor_data_s *data_p;
	sns_dd_atm_zpa2326_state_t *state_handle = \
							(sns_dd_atm_zpa2326_state_t *)dd_handle;

	PRINT_INFO_1("START","[num_sensors:%d]",num_sensors) ;
	/*** Check if arguments is NULL ***/
	if( ( dd_handle == NULL ) || ( sensors == NULL )
	|| ( memhandler == NULL ) || ( data == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","") ;
		return SNS_DDF_EINVALID_PARAM ;
	}

	/*** Check if arguments is invalid ***/
	if( num_sensors > ATM_SENSOR_ZPA2326_NUM_SENSORS )
	{
		PRINT_ERR_1("DATA_ERR","[num_sensors:%d]",num_sensors);
		return SNS_DDF_EINVALID_PARAM;
	}
	for( i = 0U ; i < num_sensors ; i++ )
	{
		PRINT_INFO_2("CONFIRM","[sensors[%d]=%d]", i, sensors[i]) ;
		if(( sensors[i] != SNS_DDF_SENSOR_PRESSURE ) && \
			( sensors[i] != SNS_DDF_SENSOR_TEMP ) )
		{
			PRINT_ERR_1("DATA_ERR","[sensor:%d]",sensors[i]);
			return SNS_DDF_EINVALID_PARAM ;
		}
	}

	/*** Check if Sensor is Enabled and Measure mode ***/
	if( ( atm_sensor_get_sensor_status() != e_Sensor_enable ) 
		|| ( atm_sensor_get_measure_mode() != e_Sensor_odr ) )
	{
		PRINT_ERR_2("SENSOR STATUS FAIL","[status:%d][mode:%d]",\
							atm_sensor_get_sensor_status(),\
							atm_sensor_get_measure_mode());
		return SNS_DDF_EFAIL ;
	}

	/*** Get TimeStamp ***/
	ret = atm_sensor_ifsys_get_timestamp(&timestamp);
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("TIMESTAMP_ERR","[ret:%d]",ret) ;
		return SNS_DDF_EFAIL ;
	}

	/*** Get pressure and temperature value from the sensor device ***/
	ret = atm_sensor_get_pressure_temperature( 
										&pressure , &temperature ) ;
	if( ret == ATM_SENSOR_OK )
	{
		state_handle->pressure = pressure ;
		state_handle->temperature = temperature ;
	}
	else if( ret == ATM_SENSOR_NO_DATA )
	{
		pressure = state_handle->pressure ;
		temperature =  state_handle->temperature ;
	}
	else
	{
		PRINT_ERR_1("GET_DATA_FAIL","[ret:%d]",ret) ;
		return SNS_DDF_EDEVICE ;
	}

	/*** Allocate Memory for sns_ddf_sensor_data_s ***/
	ret = atm_sensor_ifsys_memhandler_alloc( memhandler , \
		(uint32_t)(num_sensors * sizeof(sns_ddf_sensor_data_s)),\
		(void **)&data_p );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("MEM_ALLOC_ERR","[ret:%d]",ret) ;
		return SNS_DDF_ENOMEM ;
	}
	
	for( i = 0U ; i < num_sensors ; i++ )
	{
		/*** Allocate Memory for samples ***/
		ret = atm_sensor_ifsys_memhandler_alloc( memhandler ,\
			(uint32_t)(sizeof(sns_ddf_sensor_sample_s)), \
			(void **)(&(data_p[i].samples)) );
		if( ret != ATM_SENSOR_OK )
		{
			PRINT_ERR_1("MEM_ALLOC_ERR","[ret:%d]",ret) ;
			return SNS_DDF_ENOMEM ;
		}	
	
		/*** Calculate Sensor Data ***/
		if( sensors[i] == SNS_DDF_SENSOR_PRESSURE )
		{
			data_p[i].samples->sample = \
				ATM_SENSOR_ZPA2326_CALC_PRESSURE( pressure ) ;
		}
		else if( sensors[i] == SNS_DDF_SENSOR_TEMP )
		{
			data_p[i].samples->sample = \
				ATM_SENSOR_ZPA2326_CALC_TEMPERATURE( temperature ) ;
		}

		/*** Set output parameters ***/
		data_p[i].sensor = sensors[i] ;
		data_p[i].status = SNS_DDF_SUCCESS ;
		data_p[i].timestamp = timestamp ;
		data_p[i].num_samples = ATM_SENSOR_ZPA2326_SENSOR_DATA_NUM ;
	}
	*data = data_p ;

	PRINT_INFO("END","");
	return SNS_DDF_SUCCESS ;
}


/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_get_attr
*
*  Description  : Get Sensor Attribute.
*  Arguments    : In)  sns_ddf_handle_t		dd_handle
*                 In)  sns_ddf_sensor_e		sensor
*                 In)  sns_ddf_attribute_e	attrib
*                 In)  sns_ddf_memhandler_s	*memhandler
*                 I/O)  void			**value
*                 I/O)  uint32_t		*num_elems
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
static  sns_ddf_status_e  sns_dd_atm_zpa2326_get_attr(
	sns_ddf_handle_t dd_handle,
	sns_ddf_sensor_e sensor,
	sns_ddf_attribute_e attrib,
	sns_ddf_memhandler_s* memhandler,
	void** value,
	uint32_t* num_elems)
{
	PRINT_INFO_1("START","[attrib:%d]",attrib) ;
	/*** Check if arguments is NULL ***/
	if( ( dd_handle == NULL ) || ( memhandler == NULL )
	|| ( value == NULL ) || ( num_elems == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM;
	}

	/*** Check type of sensors ***/
	if( ( sensor != SNS_DDF_SENSOR_PRESSURE ) 
		&& ( sensor != SNS_DDF_SENSOR_TEMP )
		&& ( sensor != SNS_DDF_SENSOR__ALL ) )
	{
		PRINT_ERR_1("DIFFERENT THE SENSOR TYPE","[sensor:%d]",sensor) ;
		return SNS_DDF_EINVALID_PARAM ;
	}
	
	switch(attrib)
	{
		case SNS_DDF_ATTRIB_POWER_INFO:
		{
			*value = (void *)(&gATM_ZPA2326_POWER_INFO) ;
			*num_elems = ATM_SENSOR_ZPA2326_POWER_INFO_ELEMS_NUM ;

			PRINT_INFO_2("POWER INFO",\
				"[active_current:%d][lowpower_current:%d]",\
				((sns_ddf_power_info_s *)*value)->active_current,\
				((sns_ddf_power_info_s *)*value)->lowpower_current) ;
			PRINT_INFO_1("POWER INFO","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[POWER INFO]");
			return SNS_DDF_SUCCESS ;
		}

		case SNS_DDF_ATTRIB_RANGE:
		{
			*num_elems = ATM_SENSOR_ZPA2326_RANGE_ELEMS_NUM ;
			if( sensor == SNS_DDF_SENSOR_PRESSURE )
			{
				*value = (void *)(&gATM_ZPA2326_RANGE_PRESS) ;
				PRINT_INFO_2("RANGE",\
					"[PRESS][value_min:0x%2x][value_max:0x%2x]",\
					((sns_ddf_range_s *)*value)->min,\
					((sns_ddf_range_s *)*value)->max) ;
			}
			else
			{
				*value = (void *)(&gATM_ZPA2326_RANGE_TEMP) ;
				PRINT_INFO_2("RANGE",\
					"[TEMP][value_min:0x%2x][value_max:0x%2x]",\
					((sns_ddf_range_s *)*value)->min,\
					((sns_ddf_range_s *)*value)->max) ;
			}

			PRINT_INFO_1("RANGE","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[RANGE]");
			return SNS_DDF_SUCCESS ;
		}

		case SNS_DDF_ATTRIB_RESOLUTION:
		{
			*num_elems = ATM_SENSOR_ZPA2326_RESOLUTION_ELEMS_NUM ;
			if( sensor == SNS_DDF_SENSOR_PRESSURE )
			{
				*value = (void *)(&gATM_ZPA2326_RESOLUTION_PRESS) ;
				PRINT_INFO_1("RESOLUTION",\
					"[PRESS:%d]", *(sns_ddf_resolution_t *)*value);
			}
			else
			{
				*value = (void *)(&gATM_ZPA2326_RESOLUTION_TEMP) ;
				PRINT_INFO_1("RESOLUTION","[TEMP:%d]",\
					(sns_ddf_resolution_t *)*value);
			}
			PRINT_INFO_1("RESOLUTION","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[RESOLUTION]");
			return SNS_DDF_SUCCESS ;
		}

		case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			*num_elems = ATM_SENSOR_ZPA2326_RESOLUTION_ADC_ELEMS_NUM ;
			if( sensor == SNS_DDF_SENSOR_PRESSURE )
			{
				*value = (void *)(&gATM_ZPA2326_RESOLUTION_ADC_PRESS) ;
				PRINT_INFO_2("RESOLUTION_ADC",\
					"[PRESS][bit_len:%d][max_freq:%d]",\
					((sns_ddf_resolution_adc_s *)*value)->bit_len,\
					((sns_ddf_resolution_adc_s *)*value)->max_freq) ;
			}
			else
			{
				*value = (void *)(&gATM_ZPA2326_RESOLUTION_ADC_TEMP) ;
				PRINT_INFO_2("RESOLUTION_ADC",\
					"[TEMP][bit_len:%d][max_freq:%d]",\
					((sns_ddf_resolution_adc_s *)*value)->bit_len,\
					((sns_ddf_resolution_adc_s *)*value)->max_freq) ;
			}
			PRINT_INFO_1("RESOLUTION_ADC","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[RESOLUTION_ADC]");
			return SNS_DDF_SUCCESS ;
		}
		case SNS_DDF_ATTRIB_LOWPASS:
		{
			*value = (void *)gATM_ZPA2326_LOWPASS ;
			*num_elems = ATM_SENSOR_ZPA2326_LOWPASS_ELEMS_NUM ;
			
			PRINT_INFO_4("LOWPASS","[value[0]:%d]\
				[value[1]:%d][value[2]:%d][value[3]:%d]",\
				((sns_ddf_lowpass_freq_t *)*value)[0],\
				((sns_ddf_lowpass_freq_t *)*value)[1],\
				((sns_ddf_lowpass_freq_t *)*value)[2],\
				((sns_ddf_lowpass_freq_t *)*value)[3]) ;
			PRINT_INFO_1("LOWPASS","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[LOWPASS]");
			return SNS_DDF_SUCCESS ;
		}

		case SNS_DDF_ATTRIB_DRIVER_INFO:
		{
			*value = (void *)(&gATM_ZPA2326_DRIVER_INFO) ;
			*num_elems = ATM_SENSOR_ZPA2326_DRIVER_INFO_ELEMS_NUM ;
			
			PRINT_INFO_2("DRIVER_INFO","[name:%d][ver:%d]",\
				((sns_ddf_driver_info_s*)*value)->name,\
				((sns_ddf_driver_info_s*)*value)->version);
			PRINT_INFO_1("DRIVER_INFO","[NUM_ELEMS:%d]",*num_elems);
			PRINT_INFO("END","[DRIVER INFO]");
			return SNS_DDF_SUCCESS ;
		}
		case SNS_DDF_ATTRIB_DEVICE_INFO:
		{
			*value = (void *)(&gATM_ZPA2326_DEVICE_INFO) ;
			*num_elems = ATM_SENSOR_ZPA2326_DEVICE_INFO_ELEMS_NUM ;
			PRINT_INFO("END","[DEVICE INFO]");
			return SNS_DDF_SUCCESS ;
		}
		
		default:
		{
			PRINT_INFO_1("END","[attr:%d]",attrib);
			return SNS_DDF_EINVALID_ATTR ;
		}
	}
}

/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_set_attr
*
*  Description  : Set Sensor Attribute.
*  Arguments    : In)  sns_ddf_handle_t		dd_handle
*                 In)  sns_ddf_sensor_e		sensor
*                 In)  sns_ddf_attribute_e	attrib
*                 In)  void			*value
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
static  sns_ddf_status_e sns_dd_atm_zpa2326_set_attr(
	sns_ddf_handle_t dd_handle,
	sns_ddf_sensor_e sensor,
	sns_ddf_attribute_e attrib,
        void* value)
{
	signed int ret ;
	PRINT_INFO_1("START","[attrib:%d]",attrib) ;

	/*** Check if arguments is NULL ***/
	if( ( dd_handle == NULL ) || ( value == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM ;
	}

	/*** Check type of sensors ***/
	if( ( sensor != SNS_DDF_SENSOR_PRESSURE ) 
		&& ( sensor != SNS_DDF_SENSOR_TEMP )
		&& ( sensor != SNS_DDF_SENSOR__ALL ) )
	{
		PRINT_ERR_1("DIFFERENT THE SENSOR TYPE","[sensor:%d]",sensor) ;
		return SNS_DDF_EINVALID_PARAM ;
	}
	
	switch(attrib)
	{
		case SNS_DDF_ATTRIB_POWER_STATE:
		{
			sns_ddf_powerstate_e *power_state = \
				(sns_ddf_powerstate_e *)value ;

			/*** if Setted State is Active and 
				Measurement Mode is NOT Active, 
				Start Measurement Mode ***/
			if( ( *power_state == SNS_DDF_POWERSTATE_ACTIVE ) 
				&& ( atm_sensor_get_measure_mode() != e_Sensor_odr ) )
			{
				/**-- Start ODR mode --**/
				ret = atm_sensor_measure( e_Sensor_odr ) ;
				if( ret != ATM_SENSOR_OK )
				{
					PRINT_ERR_1("START MEASURE FAIL","[ret:%d]",ret);
					return SNS_DDF_EDEVICE ;
				}
			}
			/*** if Setted State is Lowpower and
				Measurement Mode is Active, Stop Measurement Mode ***/
			else if( ( *power_state == SNS_DDF_POWERSTATE_LOWPOWER ) 
				&& ( atm_sensor_get_measure_mode() == e_Sensor_odr ) )
			{
				/*** Stop ODR mode ***/
				ret = atm_sensor_measure(  e_Sensor_measure_stop ) ;
				if( ret != ATM_SENSOR_OK )
				{
					PRINT_ERR_1("STOP MEASURE FAIL","[ret:%d]",ret);
					return SNS_DDF_EDEVICE ;
				}
			}
			else
			{
				PRINT_INFO("SAME SETTING", "");
			}

			PRINT_INFO("END POWER STATE","[POWER STATE]");
			return SNS_DDF_SUCCESS ;
		}

		case SNS_DDF_ATTRIB_LOWPASS:
		{
			uint8_t *lowpass_index = (uint8_t *)value ;
			unsigned char odr = 0x00U;
			E_SENSOR_MEASURE_MODE mode = e_Sensor_measure_stop;

			/*** Check if arguments is INVALID ***/
			if( *lowpass_index >= ATM_SENSOR_ZPA2326_LOWPASS_ELEMS_NUM)
			{
				PRINT_ERR_1("DATA_ERR","[lowpass_index:%d]",\
												*lowpass_index);
				return SNS_DDF_EINVALID_PARAM ;
			}

			/* Configure ODR */
			odr = gATM_ZPA2326_ODR[*lowpass_index] ;

			/*** Check if ODR setting change ***/
			if( odr != atm_sensor_get_odr_time_count())
			{
				PRINT_INFO_1("CHECK_ODR_TIME_CNT","[odr:0x%2X]",odr);
				/*** If it's under measure, Stop measurement mode  ***/
				mode = atm_sensor_get_measure_mode();
				if( mode == e_Sensor_odr )
				{
					PRINT_INFO_1("STOP_ODR_MODE","[mode:%d]",mode);
					ret = atm_sensor_measure( e_Sensor_measure_stop ) ;
					if( ret != ATM_SENSOR_OK )
					{
						PRINT_ERR_1("STOP MEASURE FAIL","[ret:%d]",ret);
						return SNS_DDF_EDEVICE ;
					}
				}
				
				/*** Set ODR timecount ***/
        		ret = atm_sensor_set_odr_time_count( odr ) ;
				if( ret != ATM_SENSOR_OK )
				{
					PRINT_ERR_1("CONFIG ODR_ERR","[ret:%d]",ret);
					return SNS_DDF_EDEVICE ;
				}
				if( mode == e_Sensor_odr ){
					PRINT_INFO_1("RESTART_ODR_MODE","[mode:%d]",mode);
					ret = atm_sensor_measure(  e_Sensor_odr ) ;
					if( ret != ATM_SENSOR_OK ){
						PRINT_ERR_1("START MEASURE FAIL",\
												"[ret:%d]",ret);
						return SNS_DDF_EDEVICE ;
					}
				}
			}
			else
			{
				PRINT_INFO("SAME SETTING",""); 
			}
			PRINT_INFO("END LOWPASS","[LOWPASS]");
			return SNS_DDF_SUCCESS ;
		}
		default:
		{
			PRINT_INFO_1("END","[attr:%d]",attrib);
			return SNS_DDF_EINVALID_ATTR ;
		}
	}
}


/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_reset
*
*  Description  : Set Sensor Attribute.
*  Arguments    : In)  sns_ddf_handle_t		dd_handle
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
static sns_ddf_status_e sns_dd_atm_zpa2326_reset(
											sns_ddf_handle_t dd_handle)
{
	signed int ret;
	PRINT_INFO("START RESET","") ;
	
	/*** Check if arguments is NULL ***/
	if( dd_handle == NULL )	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM ;
	}
	
	/*** Reset All State ***/
	ret = atm_sensor_software_reset();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("RESET FAIL","[ret:%d]",ret);
		return SNS_DDF_EDEVICE ;
	}
	
	PRINT_INFO("END RESET","");
	return SNS_DDF_SUCCESS ;
}

/************************************************
*
*			SELF TEST
*
*************************************************/
static sns_ddf_status_e sns_dd_atm_zpa2326_self_test(
				sns_ddf_handle_t dd_handle,
				sns_ddf_sensor_e sensor,
				sns_ddf_test_e test,
				uint32_t* err)
{
	signed int ret ;
	signed int pressure = 0 ;
	signed int temperature = 0 ;
	unsigned int i = 0;
	unsigned char buf[INIT_LENGTH];
	sns_dd_atm_zpa2326_state_t *state_handle = \
			(sns_dd_atm_zpa2326_state_t *)dd_handle;
	
	PRINT_INFO_1("START","[test:%d]",test) ;

	/*** Check if arguments is NULL ***/
	if( ( dd_handle == NULL ) || ( err == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM ;
	}

	/*** Check type of sensors ***/
	if( ( sensor != SNS_DDF_SENSOR_PRESSURE ) 
		&& ( sensor != SNS_DDF_SENSOR_TEMP )
		&& ( sensor != SNS_DDF_SENSOR__ALL ) )
	{
		PRINT_ERR("DIFFERENT THE SENSOR TYPE","");
		return SNS_DDF_EINVALID_PARAM ;
	}

	/*** Check test type ***/
	if( test != SNS_DDF_TEST_SELF )
	{
		PRINT_ERR("DIFFERENT TEST MODE","");
		return SNS_DDF_EINVALID_TEST ;
	}
	
	/*** Software reset ***/
	ret = atm_sensor_software_reset();
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("SOFTWARE RESET FAIL","[ret:%d]",ret);
		return SNS_DDF_EDEVICE ;
	}

	/*** Check device if it is Murata's sensor ***/
	/*** Read Device ID ***/
	buf[0] = ATM_SENSOR_DEVICE_ID;
	ret = atm_sensor_ifi2c_read( &(state_handle->port_handle), buf, INIT_LENGTH );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		return SNS_DDF_EBUS;
	}
	PRINT_INFO_1("MURATA ID READ OK","[ret:%d]", ret);
	/*** Judge read ID ***/
	if( buf[0] != ATM_SENSOR_MURATA_DEV_ID )
	{
		PRINT_ERR_2("DIFFERENT MURATA SENSOR","[%d] [MURATA ID:%d]",\
			buf[0],ATM_SENSOR_MURATA_DEV_ID);
		return SNS_DDF_EDEVICE;
	}

	/*** Start measurement mode  ***/
	ret = atm_sensor_measure(  e_Sensor_odr ) ;
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("START MEASURE FAIL","[ret:%d]",ret);
		return SNS_DDF_EDEVICE ;
	}

	/*** Get the press and temp value ***/
	for(i = 0; i < ATM_SENSOR_ZPA2326_TEST_GET_SUM; i++)
	{
		/*** Get the pressure and temperature value from the sensor device ***/
		PRINT_INFO_1("GET DATA","[count:%d]",i) ;
		ret = atm_sensor_get_pressure_temperature( 
											&pressure , &temperature ) ;
		if( ret == ATM_SENSOR_OK )
		{
			PRINT_INFO_1("GET DATA SUCCESS","[count:%d]",i) ;
			break;
		}
		else if( ret == ATM_SENSOR_NO_DATA )
		{
			PRINT_INFO_1("NO DATA","[count:%d]",i) ;
		}
		else
		{
			PRINT_ERR_1("GET_DATA_FAIL","[ret:%d]",ret) ;
			return SNS_DDF_EDEVICE ;
		}
	
		atm_sensor_ifsys_delay( ATM_SENSOR_ZPA2326_TEST_DELAY_TIME );
	}
	
	/*** Stop measurement mode ***/
	ret = atm_sensor_measure(  e_Sensor_measure_stop ) ;
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("STOP MEASURE FAIL","[ret:%d]",ret);
		return SNS_DDF_EDEVICE ;
	}
	
	/*** Confirmation to as whether or 
		not the pressure and temperature value ***/
	if( i == ATM_SENSOR_ZPA2326_TEST_GET_SUM )
	{
		PRINT_ERR("GET DATA FAIL","");
		return SNS_DDF_EDEVICE ;
	}

	PRINT_INFO_2("SENSOR DATA","[press:%x] [temp:%x]",\
											pressure,temperature);
	if( ( pressure > ATM_SENSOR_ZPA2326_PRESSURE_MAX ) || 
		( ATM_SENSOR_ZPA2326_PRESSURE_MIN > pressure ) )
	{
		PRINT_ERR_1("PRESSURE DATA INVALID","[press:%x]",pressure) ;
		return SNS_DDF_EFAIL ;
	}
	else
	{
		PRINT_INFO("PRESSURE DATA CORRECT","") ;
	}
	
	if( ( temperature >  ATM_SENSOR_ZPA2326_TEMPERATURE_MAX ) || 
		( ATM_SENSOR_ZPA2326_TEMPERATURE_MIN > temperature ) )
	{
		PRINT_ERR_1("TEMPERATURE DATA INVALID","[temp:%x]",temperature) ;
		return SNS_DDF_EFAIL ;	
	}
	else
	{
		PRINT_INFO("TEMPERATURE DATA CORRECT","") ;
	}

	return SNS_DDF_SUCCESS;
}

/**********************************************************************
*
*  Name         : sns_dd_atm_zpa2326_probe
*
*  Description  : Set Sensor Attribute.
*  Arguments    : In)  sns_ddf_device_access_s*		device_info
*                 In)  sns_ddf_memhandler_s*		memhandler
*                 I/O)  uint32_t*					num_sensors
*                 I/O)  sns_ddf_sensor_e**			sensors
*  Return Value : Out) ret code defined in "sns_ddf_common.h"
*  
*  Notes        : 
*
**********************************************************************/
sns_ddf_status_e sns_dd_atm_zpa2326_probe(
                        sns_ddf_device_access_s* device_info,
                        sns_ddf_memhandler_s*    memhandler,
                        uint32_t*                num_sensors,
                        sns_ddf_sensor_e**       sensors )
{
	signed int ret ;
	unsigned char buf[INIT_LENGTH];
	sns_ddf_handle_t port_handle;

	PRINT_INFO("START","");
	
	/*** Check if arguments is NULL ***/
	if ( ( device_info == NULL ) || ( memhandler == NULL ) ||
		( num_sensors == NULL ) || ( sensors == NULL ) )
	{
		PRINT_ERR("NODATA_ERR","");
		return SNS_DDF_EINVALID_PARAM ;
	}
	
	*num_sensors = 0U;
	*sensors = NULL;

	/*** init i2c port ***/
	ret = atm_sensor_ifi2c_init(&port_handle, device_info) ;
	if( ret != ATM_SENSOR_I2C_OK )
	{
		PRINT_ERR_1("I2C INIT FAIL","[ret:%d]",ret);
		return SNS_DDF_EBUS ;
	}

	/*** Read Device ID ***/
	buf[0] = ATM_SENSOR_DEVICE_ID;
	ret = atm_sensor_ifi2c_read( &port_handle, buf, INIT_LENGTH );
	if( ret != ATM_SENSOR_OK )
	{
		PRINT_ERR_1("READ FAIL","[ret:%d]",ret);
		ret = atm_sensor_ifi2c_exit( &port_handle);
		if( ret != ATM_SENSOR_I2C_OK )
		{
			PRINT_ERR_1("I2C EXIT FAIL","[ret:%d]",ret);
		}
		return SNS_DDF_EBUS;
	}
	
	PRINT_INFO_1("MURATA ID READ OK","[ret:%d]", ret);
	/*** Judge read ID ***/
	if( buf[0] != ATM_SENSOR_MURATA_DEV_ID )
	{
		PRINT_ERR_2("DIFFERENT MURATA SENSOR","[%d] [MURATA ID:%d]",\
			buf[0],ATM_SENSOR_MURATA_DEV_ID);
		/*** Exit I2C Port ***/
		ret = atm_sensor_ifi2c_exit( &port_handle );
		if( ret != ATM_SENSOR_I2C_OK )
		{
			PRINT_ERR_1("I2C EXIT FAIL","[ret:%d]",ret);
		}
		return SNS_DDF_SUCCESS;
	}
	
	/*** Set the Sensor Information ***/
	*num_sensors = ATM_SENSOR_ZPA2326_NUM_SENSORS;
	*sensors = gATM_ZPA2326_SENSORS;
	
	/*** Exit I2C Port ***/
	ret = atm_sensor_ifi2c_exit( &port_handle );
	if( ret != ATM_SENSOR_I2C_OK )
	{
		PRINT_ERR_1("I2C EXIT FAIL","[ret:%d]",ret);
		return SNS_DDF_EBUS ;
	}
	
	PRINT_INFO_1("END","[ret:%d]",ret);
	return SNS_DDF_SUCCESS;
}

/* 
 * Device Driver Interface.
 */

sns_ddf_driver_if_s sns_dd_press_zpa2326_if =
{
	.init = &sns_dd_atm_zpa2326_init,
	.get_data = &sns_dd_atm_zpa2326_get_data,
	.set_attrib = &sns_dd_atm_zpa2326_set_attr,
	.get_attrib = &sns_dd_atm_zpa2326_get_attr,
	.handle_timer = NULL,
	.handle_irq = NULL,
	.reset = &sns_dd_atm_zpa2326_reset,
	.run_test = &sns_dd_atm_zpa2326_self_test, 
	.enable_sched_data = NULL,
	.probe = &sns_dd_atm_zpa2326_probe,
};
