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
/******* Murata Manufacturing Co,.Ltd. 2014 *****************
*
* Filename : sns_dd_press_ZPA2326.h
*
* Purpose  : Interface between SMGR and ZPA2326 core driver
*
* $Author  : Yusuke Yoshida $
* $Date    : 2014/06/10 20:04 $
* $Revision: #1 $
*
**************************************************************/

#include "fixed_point.h"
#include "sns_ddf_util.h"

/**********************************************************************
						INTERNAL DEFINITIONS
**********************************************************************/

#define ATM_SENSOR_ZPA2326_NUM_SENSORS					2U
#define ATM_SENSOR_ZPA2326_ACTIVE_CURRENT				45U
#define ATM_SENSOR_ZPA2326_LOWPOWER_CURRENT				1U
// define FX_QFACTOR		   (16)
// define FX_CONV(a,q1,q2)	   (((q2)>(q1))?(a)<<((q2)-(q1)):(a)>>((q1)-(q2)))
// define FX_CONV_Q16(a,q1)    (FX_CONV(a,q1,FX_QFACTOR))
// Avoiding avoid shift count is negative warning by not using FX_CONV macro
//#define ATM_SENSOR_ZPA2326_PRESS_RANGE_MIN				(int32_t)(FX_CONV_Q16((int32_t)300,0))
#define ATM_SENSOR_ZPA2326_PRESS_RANGE_MIN				(int32_t)(300 << 16)
//#define ATM_SENSOR_ZPA2326_PRESS_RANGE_MAX				(int32_t)(FX_CONV_Q16((int32_t)1100,0))
#define ATM_SENSOR_ZPA2326_PRESS_RANGE_MAX				(int32_t)(1100 << 16)
//#define ATM_SENSOR_ZPA2326_TEMP_RANGE_MIN				(int32_t)(FX_CONV_Q16((int32_t)-10,0))
#define ATM_SENSOR_ZPA2326_TEMP_RANGE_MIN				(int32_t)(-10 << 16)
//#define ATM_SENSOR_ZPA2326_TEMP_RANGE_MAX				(int32_t)(FX_CONV_Q16((int32_t)65,0))
#define ATM_SENSOR_ZPA2326_TEMP_RANGE_MAX				(int32_t)(65 << 16)
#define ATM_SENSOR_ZPA2326_PRESS_RESOLUTION				(int32_t)(FX_FLTTOFIX_Q16(0.015625))
#define ATM_SENSOR_ZPA2326_TEMP_RESOLUTION				(int32_t)(FX_FLTTOFIX_Q16(0.00625))
#define ATM_SENSOR_ZPA2326_PRESS_RESOLUTION_ADC_LEN		24U
#define ATM_SENSOR_ZPA2326_PRESS_RESOLUTION_ADC_FREQ	23U
#define ATM_SENSOR_ZPA2326_TEMP_RESOLUTION_ADC_LEN		16U
#define ATM_SENSOR_ZPA2326_TEMP_RESOLUTION_ADC_FREQ		23U
//#define ATM_SENSOR_ZPA2326_LOWPASS_ODR1					(int32_t)(FX_CONV_Q16((int32_t)1,0))
#define ATM_SENSOR_ZPA2326_LOWPASS_ODR1					(int32_t)(1 << 16)
//#define ATM_SENSOR_ZPA2326_LOWPASS_ODR2					(int32_t)(FX_CONV_Q16((int32_t)5,0))
#define ATM_SENSOR_ZPA2326_LOWPASS_ODR2					(int32_t)(5 << 16)
//#define ATM_SENSOR_ZPA2326_LOWPASS_ODR3					(int32_t)(FX_CONV_Q16((int32_t)11,0))
#define ATM_SENSOR_ZPA2326_LOWPASS_ODR3					(int32_t)(11 << 16)
//#define ATM_SENSOR_ZPA2326_LOWPASS_ODR4					(int32_t)(FX_CONV_Q16((int32_t)23,0))
#define ATM_SENSOR_ZPA2326_LOWPASS_ODR4					(int32_t)(23 << 16)

#define ATM_SENSOR_ZPA2326_DRIVER_INFO_NAME				"Murata ZPA2326-0311A Pressure Sensor Driver"
#define ATM_SENSOR_ZPA2326_DRIVER_INFO_VER				1

#define ATM_SENSOR_ZPA2326_DEVICE_INFO_NAME				"Pressure Sensor"
#define ATM_SENSOR_ZPA2326_DEVICE_INFO_VENDOR			"Murata"
#define ATM_SENSOR_ZPA2326_DEVICE_INFO_MODEL			"ZPA2326-0311A"
#define ATM_SENSOR_ZPA2326_DEVICE_INFO_VER				1



#define ATM_SENSOR_ZPA2326_POWER_INFO_ELEMS_NUM			1U
#define ATM_SENSOR_ZPA2326_RANGE_ELEMS_NUM				1U
#define ATM_SENSOR_ZPA2326_RESOLUTION_ELEMS_NUM			1U
#define ATM_SENSOR_ZPA2326_RESOLUTION_ADC_ELEMS_NUM		1U
#define ATM_SENSOR_ZPA2326_LOWPASS_ELEMS_NUM			4U
#define ATM_SENSOR_ZPA2326_DRIVER_INFO_ELEMS_NUM		1U
#define ATM_SENSOR_ZPA2326_DEVICE_INFO_ELEMS_NUM		1U
#define ATM_SENSOR_ZPA2326_SENSOR_DATA_NUM				1U


#define ATM_SENSOR_ZPA2326_REG_ODR1						0x10U
#define ATM_SENSOR_ZPA2326_REG_ODR2						0x50U
#define ATM_SENSOR_ZPA2326_REG_ODR3						0x60U
#define ATM_SENSOR_ZPA2326_REG_ODR4						0x70U

#define ATM_SENSOR_ZPA2326_CALC_PRESSURE(x)				((x << 8 ) / 25)
#define ATM_SENSOR_ZPA2326_CALC_TEMPERATURE(x)			(int32_t)((( x - 27456 ) << 12 ) /10 )


#define ATM_SENSOR_ZPA2326_TEST_DELAY_TIME				200000U
#define ATM_SENSOR_ZPA2326_TEST_GET_SUM					10U

#define ATM_SENSOR_ZPA2326_PRESSURE_MAX					0x006B6C00
#define ATM_SENSOR_ZPA2326_PRESSURE_MIN					0x001D4C00
#define ATM_SENSOR_ZPA2326_TEMPERATURE_MAX				0x000093E0
#define ATM_SENSOR_ZPA2326_TEMPERATURE_MIN				0x00006500

