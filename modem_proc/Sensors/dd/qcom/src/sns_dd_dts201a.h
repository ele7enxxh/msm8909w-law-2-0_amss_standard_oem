/*===========================================================================

* Copyright (c) 2014, Partron Co., Ltd. - Sensor Lab.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of the Partron Co., Ltd. nor the
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
 ******************************************************************************/

/*******************************************************************************
  Copyright (c) 2015 Qualcomm Technologies, Inc.   All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
*******************************************************************************/

/*==============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  when         who     what, where, why
--------     ---     ------------------------------------------------------ 
03-Apr-2015 AH    Qualcomm specific changes
28-Nov-2014 msahn Initial release

==========================================================================*/

#ifndef _DD_DTS201A_H
#define _DD_DTS201A_H

#include "fixed_point.h"
#include "sns_ddf_util.h"

/*=======================================================================
INTERNAL DEFINITIONS
========================================================================*/

/* When functions related to uimg are available, define UMIG_FUNCTIONS_AVAILABLE.
* This flag can also control whether or not the driver supports uImage operation.
*/
//#define UIMG_FUNCTIONS_AVAILABLE

#ifndef UIMG_FUNCTIONS_AVAILABLE
#define sns_ddf_malloc_ex(ptr, size, smgr_handle)							sns_ddf_malloc(ptr, size)
#define sns_ddf_mfree_ex(ptr, smgr_handle)								sns_ddf_mfree(ptr)
#define sns_ddf_memhandler_malloc_ex(memhandler, size, smgr_handle)	sns_ddf_memhandler_malloc(memhandler, size)
#define sns_ddf_smgr_is_in_uimage()     false
#endif

/* Enable this flag when compiling on OpenSSC HD22 builds.
*/
//#define COMPILE_FOR_HD22

#ifdef COMPILE_FOR_HD22
#define DTS201A_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC
sns_ddf_driver_if_s sns_dd_vendor_if_1;
#define DTS201A_DD_IF sns_dd_vendor_if_1
#else
#define DTS201A_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA
extern sns_ddf_driver_if_s sns_dd_dts201a_fn_list;
#define DTS201A_DD_IF sns_dd_dts201a_fn_list
#endif

/*=======================================================================
INTERNAL DEFINITIONS
========================================================================*/
/* To enable debug logs
*/
//#define	ENABLE_DTS2001A_DEBUG_LOG

/* Enable this flag when ONE_MODE builds.
*/
#define ONE_MODE

#ifdef ONE_MODE
#define	STX	0x1A
#define	ETX	0x2E
#else	/* TWO MODE */
#define	STX_N1 0x1A
#define	ETX_N1 0x27
#define	STX_M1 0x13
#define	ETX_M1 0x15
#define	STX_H2 0x28
#define	ETX_H2 0x35
#define	STX_M2 0x36
#define	ETX_M2 0x38
#endif

#define	DTS201A_ID0 0x0000
#define	DTS201A_ID1 0x0000
//#define	DTS201A_ID0 0x3026
//#define	DTS201A_ID1 0x0010
//#define	DTS201A_ID0 0x3027
//#define	DTS201A_ID1 0x0110
//#define	DTS201A_ID0 0x3218
//#define	DTS201A_ID1 0x1110

#define	DTS_CONVERSION_TIME	10000	/* 10 msec */
#define	DTS_MOVING_AVG		8
#define	DTS_N_EMISSIVITY		1		/* Depend on Object */
#define	DTS_H_EMISSIVITY		0.98		/* Fixed */

/* CONTROL REGISTERS */
#define	DTS_CUST_ID0		0x00		/* customer ID Low byte */
#define	DTS_CUST_ID1		0x01		/* customer ID High byte */
#define	DTS_SM				0xA0		/* Sensor Measurement */
#define	DTS_SM_USER		0xA1		/* Sensor Measurement User Config */
#define	DTS_SM_AZSM		0xA2		/* Auto-Zero corrected Sensor Measurement */
#define	DTS_SM_AZSM_USER	0xA3		/* Auto-Zero corrected Sensor Measurement User Config */
#define	DTS_TM				0xA4		/* Temperature Measurement */
#define	DTS_TM_USER		0xA5		/* Temperature Measurement User Config */
#define	DTS_TM_AZTM		0xA6		/* Auto-Zero corrected Temperature Measurement */
#define	DTS_TM_AZTM_USER	0xA7		/* Auto-Zero corrected Temperature Measurement User Config */
#define	DTS_START_NOM		0xA8		/* transition to normal mode */
#define	DTS_START_CM		0xA9		/* transition to command mode */
#define	DTS_MEASURE		0xAA		/* measurement cycle and calculation and storage 1*/
#define	DTS_MEASURE1		0xAB		/* measurement cycle and calculation and storage 1*/
#define	DTS_MEASURE2		0xAC		/* measurement cycle and calculation and storage 2*/
#define	DTS_MEASURE4		0xAD		/* measurement cycle and calculation and storage 4*/
#define	DTS_MEASURE8		0xAE		/* measurement cycle and calculation and storage 8*/
#define	DTS_MEASURE16		0xAF		/* measurement cycle and calculation and storage 16*/

#ifndef NULL
#define NULL 0
#endif

/* Range */
#define SNS_DD_DTS_MAX_RANGE				1	/* 2 */
#define DTS_OBJECT_RANGE_NORMAL			0
#define DTS_OBJECT_RANGE_HUMAN			1

#define DTS_OBJECT_TEMP_RANGE_N_MAX		FX_FLTTOFIX_Q16(150)
#define DTS_OBJECT_TEMP_RANGE_N_MIN		FX_FLTTOFIX_Q16(-10)
#define DTS_OBJECT_TEMP_RANGE_H_MAX		FX_FLTTOFIX_Q16(45)
#define DTS_OBJECT_TEMP_RANGE_H_MIN		FX_FLTTOFIX_Q16(30)

#define DTS_SENSOR_TEMP_RANGE_MIN		FX_FLTTOFIX_Q16(-40)
#define DTS_SENSOR_TEMP_RANGE_MAX		FX_FLTTOFIX_Q16(85)

/* Resolution */
#define DTS_OBJECT_ADC_BIT_LEN			24
#define DTS_SENSOR_ADC_BIT_LEN			24

/* ODR */
#define DTS_ODR_1HZ_1000MS				1
#define DTS_ODR_5HZ_200MS					5
#define DTS_ODR_10HZ_100MS				10
#define DTS_ODR_20HZ_50MS					20

/* Share */
struct dts201a_data {
	uint8_t sstat;
	uint32_t object;
	uint32_t ambient;
	uint32_t object_ary[DTS_MOVING_AVG];
};

typedef enum
{
	SDD_OBJECT_TEMP_N = 0,
	SDD_OBJECT_TEMP_H,
	SDD_OBJECT_TEMP_A,
	SDD_OBJECT_NUM,
	SDD_SENSOR_TEMP_N = SDD_OBJECT_NUM,
	SDD_SENSOR_TEMP_A,
	SDD_SENSOR_NUM = 2,
	SDD_NUM_DATATYPES = 6
} sdd_dts_data_type_e;

typedef struct {
	uint32_t  object_temp1_bias;
	uint32_t  object_temp1_scale;
	uint32_t  object_temp2_bias;
	uint32_t  object_temp2_scale;
	uint32_t  object_temp3_bias;
	uint32_t  object_temp3_scale;
}sns_dd_dts201a_nv_t;

typedef struct {
	sns_ddf_handle_t smgr_hndl;	// Handle used with sns_ddf_notify_data.
	sns_ddf_handle_t port_handle;	// Handle used to access the I2C bus.
	q16_t data_cache[SDD_NUM_DATATYPES];
	uint32_t object_odr;
	uint32_t ambient_odr;
	uint8_t range;				// Current range selection.
	uint8_t out_of_bound_flag;
	bool object_temp_enabled;
	bool ambient_temp_enabled;
	struct dts201a_data out;
	sns_ddf_timer_s timer_obj;
	sns_dd_dts201a_nv_t nv_source;
	
} sns_dd_dts201a_state_t;

/* debug */
#ifdef ENABLE_DTS2001A_DEBUG_LOG
#define DD_DTS201A_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "DTS201a - " msg)
#define DD_DTS201A_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "DTS201a - " msg,p1)
#define DD_DTS201A_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "DTS201a - " msg,p1,p2)
#define DD_DTS201A_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "DTS201a - " msg,p1,p2,p3)
#else
#define DD_DTS201A_MSG_0(level,msg)
#define DD_DTS201A_MSG_1(level,msg,p1)
#define DD_DTS201A_MSG_2(level,msg,p1,p2)
#define DD_DTS201A_MSG_3(level,msg,p1,p2,p3)
#endif


/*=======================================================================
INTERNAL FUNCTIONS
========================================================================*/

sns_ddf_status_e dts201a_write_byte_block
    (
    sns_ddf_handle_t port_handle,
    uint8_t reg_addr, 
    uint8_t *wbuf,
    uint8_t len    
    );

sns_ddf_status_e dts201a_read_byte_block
    (
    sns_ddf_handle_t port_handle,
    uint8_t *rbuf, 
    uint8_t len
    );

#endif /* End include guard  _DD_DTS201A_H */
