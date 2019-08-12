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
/******* Murata Manufacturing Co,.Ltd. 2014 ***************************
*
* Filename : sns_dd_press_ZPA2326_core.h
*
* Purpose  : This module has implementation 
*               of Core hedder for PressureSensor device.
*
* $Author  : Noriko Ueno $
* $Date    : 2013/12/20 15:30 $
* $Reviison: 0.3 $
*
***********************************************************************/
#ifndef ZPA2326_CORE_H
#define ZPA2326_CORE_H

#include"sns_dd_press_ZPA2326_ifsys.h"

/**********************************************************************
							VERSION
**********************************************************************/

#define	MAJOR_VER							ZPA2326_RELEASE_VERSION
#define	MINOR_VER							ZPA2326_CORE_VERSION
#define	TEST_VER							ZPA2326_IF_VERSION

#define	ZPA2326_CORE_VERSION				4U


/**********************************************************************
							I2C
**********************************************************************/
#if( ATM_SENSOR_I2C_SA0 == 1 )
#define	ATM_SENSOR_MURATA_DEV_ID			(unsigned char)0xBB
#else
#define	ATM_SENSOR_MURATA_DEV_ID			(unsigned char)0xB9
#endif

#define ATM_SENSOR_I2C_MULTI_MODE			(unsigned char)0x80 /* Use I2C multi mode */


/**********************************************************************
							DEFINE REGISTER 
**********************************************************************/
/**** Init Register ****/
#define ATM_SENSOR_INIT_REGISTER			(unsigned char)0x00

/**** Register *****/
#define ATM_SENSOR_WHO_AM_I_2				(unsigned char)0x05
#define ATM_SENSOR_WHO_AM_I_1				(unsigned char)0x06
#define ATM_SENSOR_WHO_AM_I_0				(unsigned char)0x07

#define ATM_SENSOR_REF_P_XL					(unsigned char)0x08
#define ATM_SENSOR_REF_P_L					(unsigned char)0x09
#define ATM_SENSOR_REF_P_H					(unsigned char)0x0A
#define ATM_SENSOR_KREF_COMP				(unsigned char)0x0B

#define ATM_SENSOR_DEVICE_ID				(unsigned char)0x0F
#define ATM_SENSOR_RES_CONF					(unsigned char)0x10

#define ATM_SENSOR_CTRL_REG0				(unsigned char)0x20
#define ATM_SENSOR_CTRL_REG1				(unsigned char)0x21
#define ATM_SENSOR_CTRL_REG2				(unsigned char)0x22
#define ATM_SENSOR_CTRL_REG3				(unsigned char)0x23
#define ATM_SENSOR_INT_SOURCE_REG			(unsigned char)0x24
#define ATM_SENSOR_THS_P_LOW_REG			(unsigned char)0x25
#define ATM_SENSOR_THS_P_HIGH_REG			(unsigned char)0x26
#define ATM_SENSOR_STATUS_REG				(unsigned char)0x27
#define ATM_SENSOR_PRESS_OUT_XL				(unsigned char)0x28
#define ATM_SENSOR_PRESS_OUT_L				(unsigned char)0x29
#define ATM_SENSOR_PRESS_OUT_H				(unsigned char)0x2A
#define ATM_SENSOR_TEMP_OUT_L				(unsigned char)0x2B
#define ATM_SENSOR_TEMP_OUT_H				(unsigned char)0x2C

#define ATM_SENSOR_REG_DFLT_0				(unsigned char)0x00



/**********************************************************************
							REGISTER BIT 
**********************************************************************/
/****** INIT_VALUE ****/
#define ATM_SENSOR_INIT_VALUE				(unsigned char)0x00

/****** MASK if change all bit *****/
#define ATM_SENSOR_MASK						(unsigned char)0xFF

/****** ATM_SENSOR_REF_P_XL *****/
#define ATM_SENSOR_DFLT_REF_P_XL			(unsigned char)0x00


/****** ATM_SENSOR_REF_P_L *****/
#define ATM_SENSOR_DFLT_REF_P_L				(unsigned char)0x00


/****** ATM_SENSOR_REF_P_H *****/
#define ATM_SENSOR_DFLT_REF_P_H				(unsigned char)0x00


/********* KREF_COMP *********/
#define ATM_SENSOR_MASK_KREF_COMP			(unsigned char)0x1F
#define ATM_SENSOR_DFLT_KREF_COMP			(unsigned char)0x1F


/********** RES_CONF ***********/
#define ATM_SENSOR_INIT_RES_CONF			(unsigned char)0x00
#define ATM_SENSOR_MASK_RES_CONF			(unsigned char)0x7F
#define ATM_SENSOR_DFLT_RES_CONF			(unsigned char)0x00


/******** CTRL_REG0 **********/
#define ATM_SENSOR_ENA_CTRL0				(unsigned char)0x02
#define ATM_SENSOR_DIS_CTRL0				(unsigned char)0x00
#define ATM_SENSOR_MASK_ENA_CTRL0			(unsigned char)0x02
#define ATM_SENSOR_START_ONE_SHOT			(unsigned char)0x01
#define ATM_SENSOR_STOP_ONE_SHOT			(unsigned char)0x00
#define ATM_SENSOR_MASK_ONE_SHOT			(unsigned char)0x01

#define ATM_SENSOR_DFLT_CTRL_REG0			(unsigned char)0x00


/********** CTRL_REG1 *********/
#define ATM_SENSOR_SHRT_CUT_MASK_CTRL1		(unsigned char)0x20
#define ATM_SENSOR_SET_L_EVENT_CTRL1		(unsigned char)0x02

#define ATM_SENSOR_DFLT_CTRL_REG1			(unsigned char)0x3F


/********* CTRL_REG2 ***********/
#define ATM_SENSOR_ENA_AUTOZERO				(unsigned char)0x02
#define ATM_SENSOR_DIS_AUTOZERO				(unsigned char)0x00
#define ATM_SENSOR_MASK_AUTOZERO			(unsigned char)0x02

#define ATM_SENSOR_SW_RESET					(unsigned char)0x04

#define ATM_SENSOR_DFLT_CTRL_REG2			(unsigned char)0x00


/******** CTRL_REG3 ************/
#define ATM_SENSOR_START_ODR				(unsigned char)0x80
#define ATM_SENSOR_STOP_ODR					(unsigned char)0x00
#define ATM_SENSOR_MASK_START_ODR			(unsigned char)0x80
#define ATM_SENSOR_ODR_TIM_CNT				(unsigned char)0x50
#define ATM_SENSOR_MASK_ODR_TIM_CNT			(unsigned char)0x70
#define ATM_SENSOR_DFLT_CTRL_REG3			(unsigned char)0x00


/********* INT_SOURCE **********/
#define ATM_SENSOR_SET_FIFO_E_CTRL1			(unsigned char)0x10
#define ATM_SENSOR_SET_FIFO_F_CTRL1			(unsigned char)0x08
#define ATM_SENSOR_SET_D_RDY_CTRL1			(unsigned char)0x04


/*********** THS_P_L ************/
#define ATM_SENSOR_DFLT_THS_L_HIGH_REG		(unsigned char)0x00


/*********** THS_P_H ************/
#define ATM_SENSOR_DFLT_THS_P_HIGH_REG		(unsigned char)0xFF


/********** STATUS_REG **********/
#define ATM_SENSOR_STATUS_FIFO_E_MASK		(unsigned char)0x04
#define ATM_SENSOR_STATUS_FIFO_F_MASK		(unsigned char)0x08

#define ATM_SENSOR_STATUS_P_DA_MASK			(unsigned char)0x02
#define ATM_SENSOR_STATUS_T_DA_MASK			(unsigned char)0x01


#define MAX_KREF_COMP						(unsigned char)0x1F
#define MIN_ODR_TCNT						(unsigned char)0x10
#define MAX_ODR_TCNT						(unsigned char)0x70
#define MAX_ODR_AVE_CNT						(unsigned char)0x55
#define TWOS_COMPLEMENT						(unsigned char)0x80


/*  Length */
#define INIT_LENGTH							1U
#define MEASURE_LENGTH						1U
#define GET_PRESS_LENGTH					3U
#define GET_TEMP_LENGTH						2U
#define UPDATE_LENGTH						1U
#define AUTOZERO_LENGTH						3U
#define GET_STATUS_LENGTH					1U
#define GET_PARAM_LENGTH					1U
#define PRE_SW_RESET_LENGTH					4U
#define SW_RESET_LENGTH						1U
#define SET_THS_P_LENGTH					2U
#define SET_RES_CONF_LENGTH					1U
#define SET_ODR_TIM_CNT_LENGTH				1U


/* FIFO */
#define FIFO_MAX							16

/* Sensor INFO */
#define PRESS_MIN							-8388608	/* 300hPa */
#define PRESS_MAX							8388607	/* 1100hPa */
#define TEMP_MIN							-273
#define TEMP_MAX							10000


/*******************
        ENUM 
*******************/

/*** sensor status info **/
typedef enum{
	e_Sensor_disable = 0,
	e_Sensor_enable = 1,
	e_Sensor_no_init
}E_SENSOR_STATUS;

/***** measure mode flg *****/
typedef enum{
	e_Sensor_measure_stop = 0,
	e_Sensor_oneshot = 1,
	e_Sensor_odr
}E_SENSOR_MEASURE_MODE;

/***** Autozero flg ******/
typedef enum{
	e_Sensor_dis_autozero = 0,
	e_Sensor_ena_autozero = 1
}E_SENSOR_AUTOZERO;



/**********************************************************************
							RETURN VALUES
**********************************************************************/
/***** Success *******/
#define ATM_SENSOR_OK						0 /* success */
#define ATM_SENSOR_I2C_OK					0 /* success */

/******* Error ******/
#define ATM_SENSOR_DEV_INIT_ERR				-1
#define ATM_SENSOR_NO_MURATA_DEV_ERR		-2
#define ATM_SENSOR_NO_DATA					-3 /* Null */
#define ATM_SENSOR_SET_AUTOZERO_ERR			-4
#define ATM_SENSOR_RESET_WRITE_ERR			-5
#define ATM_SENSOR_I2C_NODATA_ERR			-6
#define ATM_SENSOR_I2C_NODEV_ERR			-7
#define ATM_SENSOR_I2C_READ_TRANSFER_ERR	-8
#define ATM_SENSOR_I2C_WRITE_TRANSFER_ERR	-9
#define ATM_SENSOR_BUF_ERR					-10 /* get buffer error */
#define ATM_SENSOR_INVALID					-11 /* setting values error */
#define ATM_SENSOR_REG_ERR					-12
#define ATM_SENSOR_INIT_FAIL				-13 /* initial fail */
#define ATM_SENSOR_ALREADY_SET_AUTOZERO_ERR	-14 
#define ATM_SENSOR_SET_DEV_ERR				-15
#define ATM_SENSOR_DURING_MEASUREMENT_ERR	-16
/*********************
     sensor driver
***********************/

/***** Sensor Information ******/
typedef struct atm_sensor_data_t{
	E_SENSOR_STATUS eSensor_status;			/* sensor enable/disable/not init */
	E_SENSOR_MEASURE_MODE eMeasureMode;		/* Measure Mode */
	E_SENSOR_AUTOZERO eAutozero;			/* Autozero Mode */
	unsigned char odr_time_count;			/* One-shot:transfer time, ODR: Data rate for input */
	unsigned char odr_ave_count;			/* count of average */

}tagATM_SENSOR_DATA;


/**********************************************************************
							Extern Function
**********************************************************************/

/****** Core ********/
signed int atm_sensor_init( void *client_p );
signed int atm_sensor_exit( void );
signed int atm_sensor_software_reset( void );

signed int atm_sensor_measure( E_SENSOR_MEASURE_MODE  measure_mode );
signed int atm_sensor_autozero( E_SENSOR_AUTOZERO autozero_mode );

signed int atm_sensor_set_parameter( unsigned char reg_addr,
										unsigned char values );
signed int atm_sensor_set_odr_time_count( unsigned char values );
signed int atm_sensor_set_res_conf( unsigned char values );

signed int atm_sensor_get_pressure_temperature( signed int *press_p, 
												signed int *temp_p );
signed int atm_sensor_get_parameter( unsigned char *buf_p,
									unsigned char reg_addr );
unsigned char atm_sensor_get_odr_time_count( void );
unsigned char atm_sensor_get_odr_ave_count( void );
E_SENSOR_STATUS atm_sensor_get_sensor_status( void );
E_SENSOR_MEASURE_MODE atm_sensor_get_measure_mode( void );
E_SENSOR_AUTOZERO atm_sensor_get_autozero_mode( void );

signed int atm_sensor_enable( void );
signed int atm_sensor_disable( void );

#endif
