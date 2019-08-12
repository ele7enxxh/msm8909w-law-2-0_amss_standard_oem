/*******************************************************************************
* Copyright (c) 2013, Yamaha Corporation
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Yamaha Corporation nor the names of its
*       contributors may be used to endorse or promote products derived from
*       this software without specific prior written permission.
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
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"

/*
 * define
 */
#define YAS_ADDR_SLAVE					( 0x2e )	/* slave address */
#define YAS_ADDR_ID						( 0x80 )	/* device id reg address */
#define YAS_ADDR_COIL					( 0x81 )	/* coil reg address */
#define YAS_ADDR_MEASURE_COMMAND		( 0x82 )	/* measure command reg address */
#define YAS_ADDR_CONFIG					( 0x83 )	/* config reg address */
#define YAS_ADDR_MEASURE_INTERVAL		( 0x84 )	/* measure interval reg address */
#define YAS_ADDR_OFFSET					( 0x85 )	/* hard offset x reg address */
#define YAS_ADDR_TEST1					( 0x88 )	/* test1 reg address */
#define YAS_ADDR_TEST2					( 0x89 )	/* test2 reg address */
#define YAS_ADDR_CAL					( 0x90 )	/* cal reg address */
#define YAS_ADDR_MEASURE_DATA			( 0xb0 )	/* measue data reg address */

#define YAS530_DEVICE_ID				( 0x01 )	/* device id for MS-3E */
#define YAS532_DEVICE_ID				( 0x02 )	/* device id for MS-3R */

#define YAS_INIT_TEST1					( 0x00 )	/* Test1 reg default value */
#define YAS_INIT_TEST2					( 0x00 )	/* Test2 reg default value */
#define YAS_INIT_COIL					( 0x00 )	/* Coil reg default value */
#define YAS_INIT_MEASURE_INTERVAL		( 0x00 )	/* Measure Interval reg default value */
#define YAS_INIT_CONFIG					( 0x00 )	/* Config reg default value */

#define YAS_MEASURE_COMMAND_START		( 0x01 )	/* Measure command value */
#define YAS_MEASURE_COMMAND_LDTC		( 0x02 )
#define YAS_MEASURE_COMMAND_FORS		( 0x04 )

#define YAS_MEASURE_COMMAND_NORMAL		( 0x00 )

#define YAS_RAWDATA_UNDER				(	 0 )

#define YAS530_RAWDATA_CENTER			( 2048 )
#define YAS530_RAWDATA_OVERFLOW			( 4095 )
#define YAS532_RAWDATA_CENTER			( 4096 )
#define YAS532_RAWDATA_OVERFLOW			( 8190 )

#define YAS_FLOW_X						( 0x01 )
#define YAS_FLOW_Y1						( 0x02 )
#define YAS_FLOW_Y2						( 0x04 )
#define YAS_FLOW_MASK					( 0x01 )

#define YAS_VER_A						( 0x00 )
#define YAS_VER_B						( 0x01 )
#define YAS_VER_AB						( 0x00 )
#define YAS_VER_AC						( 0x01 )
#define YAS530_VER_A_COEF				(  380 )
#define YAS530_VER_B_COEF				(  550 )
#define YAS532_VER_AB_COEF				( 1800 )
#define YAS532_VER_AC_COEF				(  900 )

#define YAS530_CAL_DATA_SIZE			(	16 )	/* Calib data reg size for MS-3E */
#define YAS532_CAL_DATA_SIZE			(	14 )	/* Calib data reg size for MS-3R */
#define YAS_MEASURE_DATA_SIZE			(	 8 )	/* Measure data reg size */
#define YAS_OFFSET_SIZE					(	 3 )	/* offset reg size */
#define YAS_MATRIX_SIZE					(	 9 )	/* 3 * 3 matrix */

#define YAS_HARD_OFFSET_MASK			( 0x3f )
#define YAS_HARD_OFFSET_INIT			( -128 )
#define YAS_HARD_OFFSET_MIN				(  -31 )
#define YAS_HARD_OFFSET_MAX				(	31 )

#define YAS_MEASURE_NORMAL_COUNT		(	 1 )
#define YAS_MEASURE_OFFSET_COUNT		(	 6 )

#define YAS_DATA_NUM					(	 3 )

#define YAS_WAIT_GIVEUP			   ( 180000000 )	/* 3min */
#define YAS_WAIT_COILINIT			 ( 3000000 )	/* 3s */
#define YAS_WAIT_MEASURE				( 2000 )	/* 2ms */

#define YAS_SENSOR_TYPE					SNS_DDF_SENSOR_MAG

/* self test */
#define YAS_MEASURE_TEST_COUNT			(	 2 )
#define YAS530_XMIN_SENSITIVITY			(  133 )
#define YAS530_YMIN_SENSITIVITY			(  160 )
#define YAS532_XMIN_SENSITIVITY			(	29 )	/* @ VDDIO=1.7V */
#define YAS532_YMIN_SENSITIVITY			(	39 )	/* @ VDDIO=1.7V */

/* config */
#define YAS_POSITION					(	 4 )	/* 0..7 */

/* attribute */
#define YAS_ACTIVE_CURRENT				(  400 )	/* uA @ 50Hz */
#define YAS_LOWPOWER_CURRENT			(	 1 )	/* uA @ TOP=25degre,SCL=SDA= IOVDD=VDD=3.6V, RSTN=VDD */

#define YAS_DELAY_ACTIVE				(	 0 )	/* us */
#define YAS_DELAY_DATA					( 2000 )	/* us */

#define YAS_RESOLUTION				   ( 0.003 )	/* G */

#define YAS_MAX_FREQ					(  500 )
#define YAS530_BIT_LENGTH				(	12 )
#define YAS532_BIT_LENGTH				(	13 )

#define YAS530_DEVICE_MAJOR_VER				(  530 << 16 )
#define YAS532_DEVICE_MAJOR_VER				(  532 << 16 )
#define YAS_DRIVER_VER					(	 6 )
#define YAS530_DRIVER_NAME				"Yamaha MS-3E Driver"
#define YAS532_DRIVER_NAME				"Yamaha MS-3R Driver"

#define YAS_DEVICE_NAME					"Magnetometer"
#define YAS_DEVICE_VENDOR_NAME			"Yamaha"
#define YAS530_DEVICE_MODEL_NAME		"MS-3E"
#define YAS532_DEVICE_MODEL_NAME		"MS-3R"

#define YAS_Q16_ERROR					((q16_t)0x7fffffff)

#define TICK_TO_USEC(t)					((uint32_t)((t)*(30.517578125))) /* 32kHz */

/*
 * typedef
 */
typedef enum
{
	YAS_STATE_NORMAL = 0,
	YAS_STATE_WAIT_INITCOIL,
	YAS_STATE_INITCOIL,
	YAS_STATE_GIVEUP,
} yas_measure_state_e;

typedef enum
{
	YAS_TEST_NOERROR = 0,
	YAS_TEST_CONNECTIVITY,
	YAS_TEST_SENSITIVITY,
	YAS_TEST_OVERFLOW,
} yas_test_err_e;

typedef struct
{
	int32_t cx, cy1, cy2;
	int32_t a2, a3, a4, a5, a6, a7, a8, a9, k;
	uint8_t ver;
} yas_coef_s;

typedef struct
{
	int32_t v[ 3 ];
} yas_vector_s;

typedef struct
{
	sns_ddf_handle_t		smgr_handle;
	sns_ddf_handle_t		port_handle;
	sns_ddf_timer_s			timer_handle;
	sns_ddf_memhandler_s*	memhandler;
	bool					enable;						/* false:disable true:enable */
	/* timer for initialize coil */
	sns_ddf_time_t			time_giveup;
	sns_ddf_time_t			time_coilinit;
	/* measure */
	yas_measure_state_e		state;
	yas_vector_s			raw_data;
	int8_t					measure_count;
	int32_t					measure_temp;
	sns_ddf_time_t			measure_time;
	/* calibration coef */
	yas_coef_s				coef;
	/* hw offset */
	int8_t					offset[ YAS_OFFSET_SIZE ];
	/* device information */
	uint8_t					dev_id;
	int32_t					offset_coef;
	int32_t					center;
	int32_t					overflow;
	/* for retrieval data */
	sns_ddf_sensor_data_s	data;
	sns_ddf_sensor_sample_s samples[ YAS_DATA_NUM ];
	/* for self test */
	int8_t					test_measure_count;
	yas_vector_s			pre_raw_data;
} yas_state_s;

/*
 * values
 */
static const sns_ddf_sensor_e yas_sensors[] =
{
	YAS_SENSOR_TYPE,
};

static const int32_t yas_matrix[ YAS_MATRIX_SIZE ] =
{
#if   ( YAS_POSITION == 0 )
	-1,  0,  0,  0,  1,  0,  0,  0, -1
#elif ( YAS_POSITION == 1 )
	 0, -1,  0, -1,  0,  0,  0,  0, -1
#elif ( YAS_POSITION == 2 )
	 1,  0,  0,  0, -1,  0,  0,  0, -1
#elif ( YAS_POSITION == 3 )
	 0,  1,  0,  1,  0,  0,  0,  0, -1
#elif ( YAS_POSITION == 4 )
	-1,  0,  0,  0, -1,  0,  0,  0,  1
#elif ( YAS_POSITION == 5 )
	 0, -1,  0,  1,  0,  0,  0,  0,  1
#elif ( YAS_POSITION == 6 )
	 1,  0,  0,  0,  1,  0,  0,  0,  1
#elif ( YAS_POSITION == 7 )
	 0,  1,  0, -1,  0,  0,  0,  0,  1
#endif
};

/*
 * prototype
 */
static sns_ddf_status_e sns_dd_mag_yas_init(
	sns_ddf_handle_t*		dd_handle_ptr,
	sns_ddf_handle_t		smgr_handle,
	sns_ddf_nv_params_s*	nv_params,
	sns_ddf_device_access_s device_info[],
	uint32_t				num_devices,
	sns_ddf_memhandler_s*	memhandler,
	sns_ddf_sensor_e*		sensors[],
	uint32_t*				num_sensors );

static sns_ddf_status_e sns_dd_mag_yas_get_data(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors[],
	uint32_t				num_sensors,
	sns_ddf_memhandler_s*	memhandler,
	sns_ddf_sensor_data_s** data );

static sns_ddf_status_e sns_dd_mag_yas_set_attrib(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_attribute_e		attrib,
	void*					value );

static sns_ddf_status_e sns_dd_mag_yas_get_attrib(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_attribute_e		attrib,
	sns_ddf_memhandler_s*	memhandler,
	void**					value,
	uint32_t*				num_elems );

static sns_ddf_status_e sns_dd_mag_yas_run_test(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_test_e			test,
	uint32_t*				err );

static void sns_dd_mag_yas_handle_timer( sns_ddf_handle_t dd_handle, void* arg );
static void sns_dd_mag_yas_handle_irq( sns_ddf_handle_t dd_handle, uint32_t gpio_num, sns_ddf_time_t timestamp);
static sns_ddf_status_e sns_dd_mag_yas_reset( sns_ddf_handle_t	dd_handle );

/*
 * driver entry
 */
sns_ddf_driver_if_s sns_mag_yas_driver_fn_list =
{
	&sns_dd_mag_yas_init,
	&sns_dd_mag_yas_get_data,
	&sns_dd_mag_yas_set_attrib,
	&sns_dd_mag_yas_get_attrib,
	&sns_dd_mag_yas_handle_timer,
	&sns_dd_mag_yas_handle_irq,
	&sns_dd_mag_yas_reset,
	&sns_dd_mag_yas_run_test
};

/*
 * i2c write.
 */
static sns_ddf_status_e yas_write(
	sns_ddf_handle_t port_handle,
	uint8_t reg_addr,
	uint8_t* buffer,
	uint8_t bytes )
{
	uint8_t write_count = 0;

	return sns_ddf_write_port( port_handle, reg_addr, buffer, bytes, &write_count );
}

/*
 * i2c read.
 */
static sns_ddf_status_e yas_read(
	sns_ddf_handle_t port_handle,
	uint8_t reg_addr,
	uint8_t* buffer,
	uint8_t bytes )
{
	uint8_t read_count = 0;

	return sns_ddf_read_port( port_handle, reg_addr, buffer, bytes, &read_count );
}

/*
 * set enable/disable.
 */
static void yas_set_enable( yas_state_s* ptr, bool enable )
{
	if ( enable == true )
	{
		sns_ddf_time_t current_time = sns_ddf_get_timestamp();

		switch ( ptr->state )
		{
		case YAS_STATE_INITCOIL :
		case YAS_STATE_WAIT_INITCOIL :
			ptr->time_giveup = current_time;
			break;

		default :
			ptr->state = YAS_STATE_NORMAL;
			break;
		}
	}

	ptr->enable = enable;
}

/*
 * set measure command.
 */
static sns_ddf_status_e yas_set_measure( yas_state_s* ptr, uint8_t option )
{
	uint8_t data = YAS_MEASURE_COMMAND_START;
	uint8_t bytes = sizeof( data );

	data |= option;

	return yas_write( ptr->port_handle, YAS_ADDR_MEASURE_COMMAND, &data, bytes );
}

/*
 * set initcoil.
 */
static sns_ddf_status_e yas_set_initcoil( yas_state_s* ptr )
{
	uint8_t data = YAS_INIT_COIL;
	uint8_t bytes = sizeof( data );

	return yas_write( ptr->port_handle, YAS_ADDR_COIL, &data, bytes );
}

/*
 * set hw offset.
 */
static sns_ddf_status_e yas_set_offset( yas_state_s* ptr )
{
	uint32_t i;
	uint8_t reg_addr = YAS_ADDR_OFFSET;
	uint8_t data;
	uint8_t bytes = sizeof( data );
	sns_ddf_status_e status;

	for ( i = 0; i < YAS_OFFSET_SIZE; i++ )
	{
		data = ( uint8_t )( ptr->offset[ i ] & YAS_HARD_OFFSET_MASK );
		status = yas_write( ptr->port_handle, reg_addr, &data, bytes );
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}
		reg_addr++;
	}

	return SNS_DDF_SUCCESS;
}

/*
 * get calibration for YAS530.
 */
static sns_ddf_status_e yas_cal_coef_530( yas_state_s* ptr, uint8_t* dck )
{
	uint8_t cal[ YAS530_CAL_DATA_SIZE ];
	uint8_t bytes = sizeof( cal );
	sns_ddf_status_e status;
	yas_coef_s *coef = &( ptr->coef );
	uint8_t dx;
	uint8_t dy1;
	uint8_t dy2;
	uint8_t d2;
	uint8_t d3;
	uint8_t d4;
	uint8_t d5;
	uint8_t d6;
	uint8_t d7;
	uint8_t d8;
	uint8_t d9;
	uint8_t d0;

	/* dummy read. */
	status = yas_read( ptr->port_handle, YAS_ADDR_CAL, cal, bytes );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	status = yas_read( ptr->port_handle, YAS_ADDR_CAL, cal, bytes );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	dx	= cal[ 0 ];
	dy1 = cal[ 1 ];
	dy2 = cal[ 2 ];
	d2	= ( uint8_t )( ( cal[ 3 ] >> 2 ) & 0x3f );
	d3	= ( uint8_t )( ( ( cal[ 3 ] << 2 ) & 0x0c ) | ( ( cal[ 4 ] >> 6 ) & 0x03 ) );
	d4	= ( uint8_t )( cal[ 4 ] & 0x3f );
	d5	= ( uint8_t )( ( cal[ 5 ] >> 2 ) & 0x3f );
	d6	= ( uint8_t )( ( ( cal[ 5 ] << 4 ) & 0x30 ) | ( ( cal[ 6 ] >> 4 ) & 0x0f ) );
	d7	= ( uint8_t )( ( ( cal[ 6 ] << 3 ) & 0x78 ) | ( ( cal[ 7 ] >> 5 ) & 0x07 ) );
	d8	= ( uint8_t )( ( ( cal[ 7 ] << 1 ) & 0x3e ) | ( ( cal[ 8 ] >> 7 ) & 0x01 ) );
	d9	= ( uint8_t )( ( ( cal[ 8 ] << 1 ) & 0xfe ) | ( ( cal[ 9 ] >> 7 ) & 0x01 ) );
	d0	= ( uint8_t )( ( cal[ 9 ] >> 2 ) & 0x1f );

	coef->cx  = ( int32_t )( ( dx * 6 ) - 768 );
	coef->cy1 = ( int32_t )( ( dy1 * 6 ) - 768 );
	coef->cy2 = ( int32_t )( ( dy2 * 6 ) - 768 );
	coef->a2  = ( int32_t )( d2 - 32 );
	coef->a3  = ( int32_t )( d3 - 8 );
	coef->a4  = ( int32_t )( d4 - 32 );
	coef->a5  = ( int32_t )( d5 + 38 );
	coef->a6  = ( int32_t )( d6 - 32 );
	coef->a7  = ( int32_t )( d7 - 64 );
	coef->a8  = ( int32_t )( d8 - 32 );
	coef->a9  = ( int32_t )d9;
	coef->k   = ( int32_t )( d0 + 10 );
	coef->ver = ( uint8_t )( cal[ 15 ] & 0x07 );

	*dck = ( uint8_t )( ( cal[ 9 ] << 1 ) & 0x06 ) | ( ( cal[ 10 ] >> 7 ) & 0x01 );

	return SNS_DDF_SUCCESS;
}

/*
 * get calibration for YAS532.
 */
static sns_ddf_status_e yas_cal_coef_532( yas_state_s* ptr, uint8_t* dck )
{
	uint8_t cal[ YAS532_CAL_DATA_SIZE ];
	uint8_t bytes = sizeof( cal );
	yas_coef_s *coef = &( ptr->coef );
	sns_ddf_status_e status;
	uint8_t dx;
	uint8_t dy1;
	uint8_t dy2;
	uint8_t d2;
	uint8_t d3;
	uint8_t d4;
	uint8_t d5;
	uint8_t d6;
	uint8_t d7;
	uint8_t d8;
	uint8_t d9;
	uint8_t d0;

	/* dummy read. */
	status = yas_read( ptr->port_handle, YAS_ADDR_CAL, cal, bytes );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	status = yas_read( ptr->port_handle, YAS_ADDR_CAL, cal, bytes );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	dx	= cal[ 0 ];
	dy1 = cal[ 1 ];
	dy2 = cal[ 2 ];
	d2	= ( uint8_t )( ( cal[ 3 ] >> 2 ) & 0x3f );
	d3	= ( uint8_t )( ( ( cal[ 3 ] << 2 ) & 0x0c ) | ( ( cal[ 4 ] >> 6 ) & 0x03 ) );
	d4	= ( uint8_t )( cal[ 4 ] & 0x3f );
	d5	= ( uint8_t )( ( cal[ 5 ] >> 2 ) & 0x3f );
	d6	= ( uint8_t )( ( ( cal[ 5 ] << 4 ) & 0x30 ) | ( ( cal[ 6 ] >> 4 ) & 0x0f ) );
	d7	= ( uint8_t )( ( ( cal[ 6 ] << 3 ) & 0x78 ) | ( ( cal[ 7 ] >> 5 ) & 0x07 ) );
	d8	= ( uint8_t )( ( ( cal[ 7 ] << 1 ) & 0x3e ) | ( ( cal[ 8 ] >> 7 ) & 0x01 ) );
	d9	= ( uint8_t )( ( ( cal[ 8 ] << 1 ) & 0xfe ) | ( ( cal[ 9 ] >> 7 ) & 0x01 ) );
	d0	= ( uint8_t )( ( cal[ 9 ] >> 2 ) & 0x1f );

	coef->cx  = ( int32_t )( ( dx * 10 ) - 1280 );
	coef->cy1 = ( int32_t )( ( dy1 * 10 ) - 1280 );
	coef->cy2 = ( int32_t )( ( dy2 * 10 ) - 1280 );
	coef->a2  = ( int32_t )( d2 - 32 );
	coef->a3  = ( int32_t )( d3 - 8 );
	coef->a4  = ( int32_t )( d4 - 32 );
	coef->a5  = ( int32_t )( d5 + 38 );
	coef->a6  = ( int32_t )( d6 - 32 );
	coef->a7  = ( int32_t )( d7 - 64 );
	coef->a8  = ( int32_t )( d8 - 32 );
	coef->a9  = ( int32_t )d9;
	coef->k   = ( int32_t )d0;
	coef->ver = ( uint8_t )( cal[ 13 ] & 0x01 );

	*dck = ( uint8_t )( ( cal[ 9 ] << 1 ) & 0x06 ) | ( ( cal[ 10 ] >> 7 ) & 0x01 );

	return SNS_DDF_SUCCESS;
}

/*
 * get calibration.
 */
static sns_ddf_status_e yas_get_cal( yas_state_s* ptr, uint8_t* dck )
{
	sns_ddf_status_e status;

	if ( ptr->dev_id == YAS532_DEVICE_ID )
	{
		status = yas_cal_coef_532( ptr, dck );
	}
	else
	{
		status = yas_cal_coef_530( ptr, dck );
	}

	return status;
}

/*
 * get raw data for YAS530.
 */
static sns_ddf_status_e yas_get_rawdata_530( yas_state_s* ptr )
{
	sns_ddf_status_e state = SNS_DDF_EFAIL;
	uint8_t data[ YAS_MEASURE_DATA_SIZE ];
	uint8_t bytes = sizeof( data );
	uint8_t busy;
	int32_t raw;
	int32_t temp;
	yas_vector_s* p = &( ptr->raw_data );

	/* time stamp. */
	ptr->measure_time = sns_ddf_get_timestamp();

	/* get measure result. */
	state = yas_read( ptr->port_handle, YAS_ADDR_MEASURE_DATA, data, bytes );
	if ( state != SNS_DDF_SUCCESS )
	{
		return state;
	}

	/* busy. */
	busy = ( int8_t )( ( data[ 0 ] >> 7 ) & 0x01 );
	if ( busy != 0 )
	{
		return SNS_DDF_EDEVICE;
	}

	/* temp. */
	temp =	( int32_t )( data[ 0 ] & 0x7f ) << 2;
	temp |= ( int32_t )( data[ 1 ] >> 6 ) & 0x03;
	ptr->measure_temp = temp;

	/* raw x. */
	raw =  ( int32_t )( data[ 2 ] & 0x7f ) << 5;
	raw |= ( int32_t )( ( data[ 3 ] >> 3 ) & 0x1f );
	p->v[ 0 ] = raw;

	/* raw y1. */
	raw =  ( int32_t )( data[ 4 ] & 0x7f ) << 5;
	raw |= ( int32_t )( ( data[ 5] >> 3 ) & 0x1f );
	p->v[ 1 ] = raw;

	/* raw y2. */
	raw =  ( int32_t )( data[ 6 ] & 0x7f ) << 5;
	raw |= ( int32_t )( ( data[ 7] >> 3 ) & 0x1f );
	p->v[ 2 ] = raw;

	return state;
}

/*
 * get raw data for YAS532.
 */
static sns_ddf_status_e yas_get_rawdata_532( yas_state_s* ptr )
{
	sns_ddf_status_e state = SNS_DDF_EFAIL;
	uint8_t data[ YAS_MEASURE_DATA_SIZE ];
	uint8_t bytes = sizeof( data );
	uint8_t busy;
	int32_t raw;
	int32_t temp;
	yas_vector_s* p = &( ptr->raw_data );

	/* time stamp. */
	ptr->measure_time = sns_ddf_get_timestamp();

	/* get measure result. */
	state = yas_read( ptr->port_handle, YAS_ADDR_MEASURE_DATA, data, bytes );
	if ( state != SNS_DDF_SUCCESS )
	{
		return state;
	}

	/* busy. */
	busy = ( int8_t )( ( data[ 0 ] >> 7 ) & 0x01 );
	if ( busy != 0 )
	{
		return SNS_DDF_EDEVICE;
	}

	/* temp. */
	temp =	( int32_t )( data[ 0 ] & 0x7f ) << 3;
	temp |= ( int32_t )( data[ 1 ] >> 5 ) & 0x07;
	ptr->measure_temp = temp;

	/* raw x. */
	raw =  ( int32_t )( data[ 2 ] & 0x7f ) << 6;
	raw |= ( int32_t )( ( data[ 3 ] >> 2 ) & 0x3f );
	p->v[ 0 ] = raw;

	/* raw y1. */
	raw =  ( int32_t )( data[ 4 ] & 0x7f ) << 6;
	raw |= ( int32_t )( ( data[ 5] >> 2 ) & 0x3f );
	p->v[ 1 ] = raw;

	/* raw y2. */
	raw =  ( int32_t )( data[ 6 ] & 0x7f ) << 6;
	raw |= ( int32_t )( ( data[ 7] >> 2 ) & 0x3f );
	p->v[ 2 ] = raw;

	return state;
}

static sns_ddf_status_e yas_get_rawdata( yas_state_s* ptr, uint8_t* flow )
{
	sns_ddf_status_e status;
	yas_vector_s* p = &( ptr->raw_data );

	if ( ptr->dev_id == YAS532_DEVICE_ID )
	{
		status = yas_get_rawdata_532( ptr );
	}
	else
	{
		status = yas_get_rawdata_530( ptr );
	}

	/* check flow. */
	*flow = 0;
	if ( ( p->v[ 0 ] <= YAS_RAWDATA_UNDER )
	  || ( ptr->overflow <= p->v[ 0 ] ) )
	{
		*flow |= YAS_FLOW_X;
	}
	if ( ( p->v[ 1 ] <= YAS_RAWDATA_UNDER )
	  || ( ptr->overflow <= p->v[ 1 ] ) )
	{
		*flow |= YAS_FLOW_Y1;
	}
	if ( ( p->v[ 2 ] <= YAS_RAWDATA_UNDER )
	  || ( ptr->overflow <= p->v[ 2 ] ) )
	{
		*flow |= YAS_FLOW_Y2;
	}

	return status;
}

/*
 * start measurement.
 */
static sns_ddf_status_e yas_start_measure_normal( yas_state_s* ptr, uint8_t option )
{
	sns_ddf_status_e status;

	ptr->measure_count = YAS_MEASURE_NORMAL_COUNT;

	/* start measure. */
	status = yas_set_measure( ptr, option );
	if ( status == SNS_DDF_SUCCESS )
	{
		/* wait 2ms. */
		status = sns_ddf_timer_start( ptr->timer_handle, YAS_WAIT_MEASURE );
	}

	return status;
}

/*
 * start calculation hardware offset.
 */
static sns_ddf_status_e yas_start_measure_offset( yas_state_s* ptr )
{
	sns_ddf_status_e status;

	ptr->measure_count = YAS_MEASURE_OFFSET_COUNT;

	/* reset hw offset. */
	ptr->offset[ 0 ] = 0;
	ptr->offset[ 1 ] = 0;
	ptr->offset[ 2 ] = 0;

	status = yas_set_initcoil( ptr );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	status = yas_set_offset( ptr );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	/* start measure. */
	status = yas_set_measure( ptr, YAS_MEASURE_COMMAND_NORMAL );
	if ( status == SNS_DDF_SUCCESS )
	{
		/* wait 2ms. */
		status = sns_ddf_timer_start( ptr->timer_handle, YAS_WAIT_MEASURE );
	}

	return status;
}

/*
 * complete measure.
 */
static sns_ddf_status_e yas_measure_complete( yas_state_s* ptr, uint8_t *flow )
{
	uint8_t correct;
	uint8_t option = YAS_MEASURE_COMMAND_NORMAL;
	uint32_t i;
	sns_ddf_status_e status;
	static const int8_t correct_tbl[ YAS_MEASURE_OFFSET_COUNT ] = { 0, 1, 2, 4, 8, 16 };

	/* get measure result. */
	status = yas_get_rawdata( ptr, flow );
	if ( status != SNS_DDF_SUCCESS )
	{
		/* error */
		return status;
	}

	/* restart measure. */
	ptr->measure_count--;
	if ( 0 < ptr->measure_count )
	{
		/* calc hw offset. */
		correct = correct_tbl[ ptr->measure_count ];
		for ( i = 0; i < YAS_DATA_NUM; i++ )
		{
			if ( ptr->center < ptr->raw_data.v[ i ] )
			{
				ptr->offset[ i ] = ( int8_t )( ptr->offset[ i ] + correct );
			}
			else if ( ptr->raw_data.v[ i ] < ptr->center )
			{
				ptr->offset[ i ] = ( int8_t )( ptr->offset[ i ] - correct );
			}
			else
			{
				/* Do Nothing. */
			}
		}

		/* set hw offset. */
		status = yas_set_offset( ptr );
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}

		/* start measure. */
		if ( ( ptr->test_measure_count != 0 )
		  && ( ptr->measure_count == 1 ) )
		{
			option = YAS_MEASURE_COMMAND_LDTC;
		}
		status = yas_set_measure( ptr, option );
		if ( status == SNS_DDF_SUCCESS )
		{
			/* wait 2ms. */
			status = sns_ddf_timer_start( ptr->timer_handle, YAS_WAIT_MEASURE );
		}
	}

	return status;
}

/*
 * retrieval data
 */
static sns_ddf_status_e yas_retrieval_data( yas_state_s* ptr, uint8_t flow )
{
	int32_t i;
	int32_t k;
	int32_t sx;
	int32_t sy;
	int32_t sz;
	int32_t sy1;
	int32_t sy2;
	int32_t temp = ptr->measure_temp;
	uint8_t cnv_flow;
	yas_coef_s* p = &( ptr->coef );
	yas_vector_s tmp;
	yas_vector_s val;
	yas_vector_s* raw = &( ptr->raw_data );
	sns_ddf_sensor_data_s* data = &( ptr->data );

	raw->v[ 0 ] += ptr->offset[ 0 ] * ptr->offset_coef;
	raw->v[ 1 ] += ptr->offset[ 1 ] * ptr->offset_coef;
	raw->v[ 2 ] += ptr->offset[ 2 ] * ptr->offset_coef;

	sx	= raw->v[ 0 ] - ( ( p->cx  * temp ) / 100 );
	sy1 = raw->v[ 1 ] - ( ( p->cy1 * temp ) / 100 );
	sy2 = raw->v[ 2 ] - ( ( p->cy2 * temp ) / 100 );
	sy	= sy1 - sy2;
	sz	= -sy1 - sy2;

	tmp.v[ 0 ] = ( p->k * (   100 * sx + p->a2 * sy + p->a3 * sz ) ) / 10;
	tmp.v[ 1 ] = ( p->k * ( p->a4 * sx + p->a5 * sy + p->a6 * sz ) ) / 10;
	tmp.v[ 2 ] = ( p->k * ( p->a7 * sx + p->a8 * sy + p->a9 * sz ) ) / 10;

	cnv_flow = 0;
	for ( i = 0; i < YAS_DATA_NUM; i++ )
	{
		val.v[ i ]	= yas_matrix[ i * 3 + 0 ] * tmp.v[ 0 ];
		val.v[ i ] += yas_matrix[ i * 3 + 1 ] * tmp.v[ 1 ];
		val.v[ i ] += yas_matrix[ i * 3 + 2 ] * tmp.v[ 2 ];

		for ( k = 0; k < 3; k++ )
		{
			if ( yas_matrix[ i * 3 + k ] != 0 )
			{
				cnv_flow |=  ( ( flow >> k ) & YAS_FLOW_MASK ) << i;
			}
		}
	}

	data->samples = ptr->samples;
	for ( i = 0; i < YAS_DATA_NUM; i++ )
	{
		/* nT ¨ G */
		if ( cnv_flow & YAS_FLOW_MASK )
		{
			data->samples[ i ].status = SNS_DDF_EDATA_BOUND;
			data->samples[ i ].sample = YAS_Q16_ERROR;
		}
		else
		{
			data->samples[ i ].status = SNS_DDF_SUCCESS;
			data->samples[ i ].sample = FX_FLTTOFIX_Q16( val.v[ i ] / 100000.0 );
		}
		cnv_flow >>= 1;
	}

	data->sensor = YAS_SENSOR_TYPE;
	data->status = SNS_DDF_SUCCESS;
	data->timestamp = ptr->measure_time;
	data->num_samples = YAS_DATA_NUM;

	return sns_ddf_smgr_notify_data( ptr->smgr_handle, data, 1 );
}

/*
 * start self test
 */
static sns_ddf_status_e yas_start_self_test( yas_state_s* ptr, uint32_t* err )
{
	sns_ddf_status_e status;

	ptr->test_measure_count = YAS_MEASURE_TEST_COUNT;
	*err = YAS_TEST_NOERROR;

	status = yas_start_measure_offset( ptr );
	if ( status != SNS_DDF_SUCCESS )
	{
		*err = YAS_TEST_CONNECTIVITY;
	}
	else
	{
		status = SNS_DDF_PENDING;
	}

	return status;
}

/*
 * self test
 */
static sns_ddf_status_e yas_self_test( yas_state_s* ptr, uint8_t flow )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	int32_t sx;
	int32_t sy;
	int32_t xmin;
	int32_t ymin;
	yas_vector_s* plus;
	yas_vector_s* minus;
	yas_coef_s* p = &( ptr->coef );

	if ( flow != 0 )
	{
		/* error */
		ptr->test_measure_count = 0;
		sns_ddf_smgr_notify_test_complete( ptr->smgr_handle,
						YAS_SENSOR_TYPE, SNS_DDF_EFAIL, YAS_TEST_OVERFLOW );
	}
	else
	{
		ptr->test_measure_count--;
		if ( ptr->test_measure_count == 0 )
		{
			if ( ptr->dev_id == YAS532_DEVICE_ID )
			{
				xmin = YAS532_XMIN_SENSITIVITY;
				ymin = YAS532_YMIN_SENSITIVITY;
				plus = &( ptr->pre_raw_data );
				minus = &( ptr->raw_data );

				sx = p->k * 100 * ( plus->v[ 0 ] - minus->v[ 0 ] );
				sx = sx / 10000;
				sy = p->k * p->a5 *
					( ( plus->v[ 1 ] - minus->v[ 1 ] ) - ( plus->v[ 2 ] - minus->v[ 2 ] ) );
				sy = sy / 10000;
			}
			else
			{
				xmin = YAS530_XMIN_SENSITIVITY;
				ymin = YAS530_YMIN_SENSITIVITY;
				plus = &( ptr->pre_raw_data );
				minus = &( ptr->raw_data );

				sx = minus->v[ 0 ] - plus->v[ 0 ];
				sy = ( minus->v[ 1 ] - plus->v[ 1 ] ) - ( minus->v[ 2 ] - plus->v[ 2 ] );
			}

			if ( ( sx < xmin ) || ( sy < ymin ) )
			{
				/* error */
				sns_ddf_smgr_notify_test_complete( ptr->smgr_handle,
								YAS_SENSOR_TYPE, SNS_DDF_EFAIL, YAS_TEST_SENSITIVITY );
			}
			else
			{
				/* success */
				sns_ddf_smgr_notify_test_complete( ptr->smgr_handle,
								YAS_SENSOR_TYPE, SNS_DDF_SUCCESS, YAS_TEST_NOERROR );
			}
		}
		else
		{
			/* keep LDTC=1/FORS=0 data */
			ptr->pre_raw_data = ptr->raw_data;
			/* measure LDTC=1/FORS=1 */
			status = yas_start_measure_normal( ptr,
								YAS_MEASURE_COMMAND_FORS | YAS_MEASURE_COMMAND_LDTC );
			if ( status != SNS_DDF_SUCCESS )
			{
				/* error */
				ptr->test_measure_count = 0;
				sns_ddf_smgr_notify_test_complete( ptr->smgr_handle,
								YAS_SENSOR_TYPE, SNS_DDF_EFAIL, YAS_TEST_CONNECTIVITY );
			}
		}
	}

	return status;
}

/*
 * init
 */
static sns_ddf_status_e sns_dd_mag_yas_init(
	sns_ddf_handle_t*		dd_handle_ptr,
	sns_ddf_handle_t		smgr_handle,
	sns_ddf_nv_params_s*	nv_params,
	sns_ddf_device_access_s device_info[],
	uint32_t				num_devices,
	sns_ddf_memhandler_s*	memhandler,
	sns_ddf_sensor_e*		sensors[],
	uint32_t*				num_sensors )
{
	yas_state_s* ptr;
	sns_ddf_status_e status;

	/* allocate a driver instance. */
	status = sns_ddf_malloc( ( void** )&ptr, sizeof( yas_state_s ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return SNS_DDF_ENOMEM;
	}

	/* open communication port to the device. */
	ptr->smgr_handle = smgr_handle;
	ptr->memhandler = memhandler;
	status = sns_ddf_open_port( &( ptr->port_handle ),
						&( device_info->port_config ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		sns_ddf_mfree( ptr );
		return status;
	}

	/* initialize timer. */
	status = sns_ddf_timer_init( &( ptr->timer_handle ), ptr,
						&sns_mag_yas_driver_fn_list, NULL, false );
	if ( status != SNS_DDF_SUCCESS )
	{
		sns_ddf_close_port( ptr->port_handle );
		sns_ddf_mfree( ptr );
		return status;
	}

	/* reset */
	status = sns_dd_mag_yas_reset( ( sns_ddf_handle_t )ptr );
	if ( status != SNS_DDF_SUCCESS )
	{
		sns_ddf_close_port( ptr->port_handle );
		sns_ddf_timer_release( ptr->timer_handle );
		sns_ddf_mfree( ptr );
	}
	else
	{
		/* fill out output parameters. */
		*num_sensors = ( sizeof( yas_sensors ) / sizeof( sns_ddf_sensor_e ) );
		*sensors = ( sns_ddf_sensor_e* )&yas_sensors;
		*dd_handle_ptr = ( sns_ddf_handle_t )ptr;
	}

	return status;
}

/*
 * get_data
 */
static sns_ddf_status_e sns_dd_mag_yas_get_data(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors[],
	uint32_t				num_sensors,
	sns_ddf_memhandler_s*	memhandler,
	sns_ddf_sensor_data_s** data )
{
	yas_state_s* ptr = ( yas_state_s* )dd_handle;
	sns_ddf_status_e status = SNS_DDF_EFAIL;
	sns_ddf_time_t current_time = sns_ddf_get_timestamp();

	if ( ( ptr->enable != true )			/* low power */
	  || ( ptr->measure_count != 0 )		/* in measurement. */
	  || ( ptr->test_measure_count != 0 ) ) /* in test. */
	{
		return status;
	}

	/* initialize memhandler. */
	ptr->memhandler = memhandler;

	/* initialize hw offset. */
	if ( ( ptr->offset [ 0 ] < YAS_HARD_OFFSET_MIN )
	  || ( YAS_HARD_OFFSET_MAX < ptr->offset [ 0 ] ) )
	{
		/* start measure offset. */
		ptr->time_giveup = current_time;
		ptr->time_coilinit = current_time;
		ptr->state = YAS_STATE_INITCOIL;
		status = yas_start_measure_offset( ptr );
	}
	else
	{
		/* check state */
		switch( ptr->state )
		{
		case YAS_STATE_NORMAL :
		case YAS_STATE_GIVEUP :
			/* start measurement. */
			status = yas_start_measure_normal( ptr, YAS_MEASURE_COMMAND_NORMAL );
			break;

		case YAS_STATE_WAIT_INITCOIL :
			if ( TICK_TO_USEC( current_time - ptr->time_giveup ) >= YAS_WAIT_GIVEUP )
			{
				ptr->state = YAS_STATE_GIVEUP;
				status = yas_start_measure_normal( ptr, YAS_MEASURE_COMMAND_NORMAL );
			}
			else if ( TICK_TO_USEC( current_time - ptr->time_coilinit ) >= YAS_WAIT_COILINIT )
			{
				/* start measure offset. */
				ptr->state = YAS_STATE_INITCOIL;
				status = yas_start_measure_offset( ptr );
			}
			else
			{
				/* start measure normal. */
				status = yas_start_measure_normal( ptr, YAS_MEASURE_COMMAND_NORMAL );
			}
			break;

		case YAS_STATE_INITCOIL :
		default :
			break;
		}
	}

	/* check result of measure */
	if ( status == SNS_DDF_SUCCESS )
	{
		*data = ( sns_ddf_sensor_data_s* )NULL;
		status = SNS_DDF_PENDING;
	}

	return status;
}

/*
 * set_attrib
 */
static sns_ddf_status_e sns_dd_mag_yas_set_attrib(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_attribute_e		attrib,
	void*					value )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;

	switch( attrib )
	{
	case SNS_DDF_ATTRIB_POWER_STATE :
		{
			sns_ddf_powerstate_e* power_state = ( sns_ddf_powerstate_e* )value;
			bool enable = false;

			if ( *power_state == SNS_DDF_POWERSTATE_ACTIVE )
			{
				enable = true;
			}
			yas_set_enable( ( yas_state_s* )dd_handle, enable );
		}
		break;

	default :
		status = SNS_DDF_EINVALID_ATTR;
		break;
	}

	return status;
}

/*
 * get_attrib
 */
static sns_ddf_status_e sns_dd_mag_yas_get_attrib(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_attribute_e		attrib,
	sns_ddf_memhandler_s*	memhandler,
	void**					value,
	uint32_t*				num_elems )
{
	yas_state_s* ptr = ( yas_state_s* )dd_handle;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;

	switch( attrib )
	{
	case SNS_DDF_ATTRIB_POWER_INFO :
		{
			sns_ddf_power_info_s* info = ( sns_ddf_power_info_s* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_power_info_s ) );
			if ( info != NULL )
			{
				info->active_current = YAS_ACTIVE_CURRENT;
				info->lowpower_current = YAS_LOWPOWER_CURRENT;
				*value = info;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	case SNS_DDF_ATTRIB_DELAYS :
		{
			sns_ddf_delays_s* delays = ( sns_ddf_delays_s* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_delays_s ) );
			if ( delays != NULL )
			{
				delays->time_to_active = YAS_DELAY_ACTIVE;
				delays->time_to_data   = YAS_DELAY_DATA;
				*value = delays;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	case SNS_DDF_ATTRIB_RESOLUTION :
		{
			sns_ddf_resolution_t* res = ( sns_ddf_resolution_t* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_resolution_t ) );
			if ( res != NULL )
			{
				*res = FX_FLTTOFIX_Q16( YAS_RESOLUTION );
				*value = res;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	case SNS_DDF_ATTRIB_RESOLUTION_ADC :
		{
			sns_ddf_resolution_adc_s* res = ( sns_ddf_resolution_adc_s* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_resolution_adc_s ) );
			if ( res != NULL )
			{
				if ( ptr->dev_id == YAS532_DEVICE_ID )
				{
					res->bit_len = YAS532_BIT_LENGTH;
				}
				else
				{
					res->bit_len = YAS530_BIT_LENGTH;
				}
				res->max_freq = YAS_MAX_FREQ;
				*value = res;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	case SNS_DDF_ATTRIB_DRIVER_INFO :
		{
			sns_ddf_driver_info_s* info = ( sns_ddf_driver_info_s* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_driver_info_s ) );
			if ( info != NULL )
			{
				if ( ptr->dev_id == YAS532_DEVICE_ID )
				{
					info->name = YAS532_DRIVER_NAME;
				}
				else
				{
					info->name = YAS530_DRIVER_NAME;
				}
				info->version = YAS_DRIVER_VER;
				*value = info;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	case SNS_DDF_ATTRIB_DEVICE_INFO :
		{
			sns_ddf_device_info_s* info = ( sns_ddf_device_info_s* )sns_ddf_memhandler_malloc(
								memhandler, sizeof( sns_ddf_device_info_s ) );
			if ( info != NULL )
			{
				if ( ptr->dev_id == YAS532_DEVICE_ID )
				{
					info->model = YAS532_DEVICE_MODEL_NAME;
					info->version = YAS532_DEVICE_MAJOR_VER;
				}
				else
				{
					info->model = YAS530_DEVICE_MODEL_NAME;
					info->version = YAS530_DEVICE_MAJOR_VER;
				}
				info->name = YAS_DEVICE_NAME;
				info->vendor = YAS_DEVICE_VENDOR_NAME;
				info->version |= ( int32_t )ptr->coef.ver << 8;
				info->version |= ( int32_t )YAS_DRIVER_VER;
				*value = info;
				*num_elems = 1;
			}
			else
			{
				status = SNS_DDF_ENOMEM;
			}
		}
		break;

	default :
		status = SNS_DDF_EINVALID_ATTR;
		break;
	}

	return status;
}

/*
 * handle_timer
 */
static void sns_dd_mag_yas_handle_timer( sns_ddf_handle_t dd_handle, void* arg )
{
	uint8_t flow = 0;
	sns_ddf_status_e status;
	yas_state_s* ptr = ( yas_state_s* )dd_handle;

	if ( ptr->measure_count <= 0 )
	{
		return;
	}

	/* get measure result. */
	status = yas_measure_complete( ptr, &flow );

	if( 0 < ptr->test_measure_count )
	{
		if ( status == SNS_DDF_SUCCESS )
		{
			if ( ptr->measure_count == 0 )
			{
				yas_self_test( ptr, flow );
			}
		}
		else
		{
			/* error */
			sns_ddf_smgr_notify_test_complete( ptr->smgr_handle,
								YAS_SENSOR_TYPE, SNS_DDF_EFAIL, YAS_TEST_CONNECTIVITY );
		}
	}
	else
	{
		if ( status == SNS_DDF_SUCCESS )
		{
			/* check state. */
			switch( ptr->state )
			{
			case YAS_STATE_INITCOIL :
				if ( ptr->measure_count == 0 )
				{
					status = yas_retrieval_data( ptr, flow );
					if ( status == SNS_DDF_SUCCESS )
					{
						if ( flow != 0 )
						{
							/* start soft timer. */
							ptr->time_coilinit = ptr->measure_time;
							ptr->state = YAS_STATE_WAIT_INITCOIL;
						}
						else
						{
							ptr->state = YAS_STATE_NORMAL;
						}
					}
				}
				break;

			case YAS_STATE_NORMAL :
				status = yas_retrieval_data( ptr, flow );
				if( status == SNS_DDF_SUCCESS )
				{
					if ( flow != 0 )
					{
						/* start soft timer. */
						ptr->time_giveup = ptr->measure_time;
						ptr->time_coilinit = ptr->measure_time;
						ptr->state = YAS_STATE_WAIT_INITCOIL;
					}
				}
				break;

			case YAS_STATE_GIVEUP :
			case YAS_STATE_WAIT_INITCOIL :
				status = yas_retrieval_data( ptr, flow );
				break;

			default :
				break;
			}
		}

		if( status != SNS_DDF_SUCCESS )
		{
			/* error */
			sns_ddf_smgr_notify_event( ptr->smgr_handle,
								YAS_SENSOR_TYPE, SNS_DDF_EVENT_EDEVICE );
		}
	}
}

/*
 * handle_irq
 */
static void sns_dd_mag_yas_handle_irq( sns_ddf_handle_t dd_handle, 
									uint32_t gpio_num, 
									sns_ddf_time_t timestamp)
{
	/* Do Nothing. */
}

/*
 * reset
 */
static sns_ddf_status_e sns_dd_mag_yas_reset( sns_ddf_handle_t	dd_handle )
{
	uint8_t id;
	uint8_t ver;
	uint8_t buffer;
	uint8_t dck = 0;
	yas_state_s* ptr = ( yas_state_s* )dd_handle;
	sns_ddf_status_e status;

	/* initialize state. */
	ptr->state = YAS_STATE_NORMAL;
	ptr->measure_count = 0;
	ptr->test_measure_count = 0;
	ptr->offset[ 0 ] = YAS_HARD_OFFSET_INIT;
	ptr->offset[ 1 ] = YAS_HARD_OFFSET_INIT;
	ptr->offset[ 2 ] = YAS_HARD_OFFSET_INIT;
	ptr->dev_id = 0;
	yas_set_enable( ptr, false );

	/* get device id. */
	status = yas_read( ptr->port_handle, YAS_ADDR_ID, &id, sizeof( id ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	/* check device id. */
	if ( ( id != YAS530_DEVICE_ID )
	  && ( id != YAS532_DEVICE_ID ) )
	{
		return SNS_DDF_EDEVICE;
	}
	ptr->dev_id = id;

	/* initialize test1 register. */
	buffer = YAS_INIT_TEST1;
	status = yas_write( ptr->port_handle, YAS_ADDR_TEST1,
						&buffer, sizeof( buffer ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	/* initialize test2 register. */
	buffer = YAS_INIT_TEST2;
	status = yas_write( ptr->port_handle, YAS_ADDR_TEST2,
						&buffer, sizeof( buffer ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	/* read calibration register. */
	status = yas_get_cal( ptr, &dck );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	ver = ptr->coef.ver;
	switch( id )
	{
	case YAS532_DEVICE_ID :
		ptr->center = YAS532_RAWDATA_CENTER;
		ptr->overflow = YAS532_RAWDATA_OVERFLOW;
		switch ( ver )
		{
		case YAS_VER_AB :
			ptr->offset_coef = YAS532_VER_AB_COEF;
			break;
		case YAS_VER_AC :
		default :
			ptr->offset_coef = YAS532_VER_AC_COEF;
			break;
		}
		break;
	case YAS530_DEVICE_ID :
	default :
		ptr->center = YAS530_RAWDATA_CENTER;
		ptr->overflow = YAS530_RAWDATA_OVERFLOW;
		switch ( ver )
		{
		case YAS_VER_B :
			ptr->offset_coef = YAS530_VER_B_COEF;
			break;
		case YAS_VER_A :
		default :
			ptr->offset_coef = YAS530_VER_A_COEF;
			break;
		}
		break;
	}

	/* initialize config register. */
	buffer = ( uint8_t )( YAS_INIT_CONFIG | ( ( dck & 0x07 ) << 2 ) );
	status = yas_write( ptr->port_handle, YAS_ADDR_CONFIG,
						&buffer, sizeof( buffer ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	/* initialize measure interval register. */
	buffer = YAS_INIT_MEASURE_INTERVAL;
	status = yas_write( ptr->port_handle, YAS_ADDR_MEASURE_INTERVAL,
						&buffer, sizeof( buffer ) );
	if ( status != SNS_DDF_SUCCESS )
	{
		return status;
	}

	return SNS_DDF_SUCCESS;
}

/*
 * test
 */
static sns_ddf_status_e sns_dd_mag_yas_run_test(
	sns_ddf_handle_t		dd_handle,
	sns_ddf_sensor_e		sensors,
	sns_ddf_test_e			test,
	uint32_t*				err )
{
	sns_ddf_status_e status = SNS_DDF_EINVALID_TEST;
	yas_state_s* ptr = ( yas_state_s* )dd_handle;

	if ( ( ptr->measure_count != 0 )		/* in measurement. */
	  || ( ptr->test_measure_count != 0 ) ) /* in test. */
	{
		return SNS_DDF_EDEVICE_BUSY;
	}

	switch( test )
	{
	case SNS_DDF_TEST_SELF :
	case SNS_DDF_TEST_CONNECTIVITY :
		status = yas_start_self_test( ptr, err );
		break;

	case SNS_DDF_TEST_IRQ :
	default :
		break;
	}

	return status;
}
