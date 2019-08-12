/*===========================================================================

 A L T I M E T E R    &    T E M P E R A T U R E    S E N S O R    D R I V E R

DESCRIPTION

  BMP085 Altimeter and Temperature Sensor Registers address and their format.


Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alt_bmp085_priv.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------ 
05/09/13       pn      Updated message macros
04/18/13       pn      Added message macros
06/26/12       sd      Added BMP085_NUM_DATA_TYPES 
02/22/11       AS      Draft
==========================================================================*/

#ifndef _BMP085_H_
#define _BMP085_H_

/*
 * BMP085 Sensor Registers address.
 * Registers are 8 bit each (per I2C read/write address)
 */

/* The PROM holds the calibration parameters */
#define BMP085_REG_PROM_AC1_MSB		0xAA
#define BMP085_REG_PROM_AC1_LSB		0xAB
#define BMP085_REG_PROM_AC2_MSB		0xAC
#define BMP085_REG_PROM_AC2_LSB		0xAD
#define BMP085_REG_PROM_AC3_MSB		0xAE
#define BMP085_REG_PROM_AC3_LSB		0xAF
#define BMP085_REG_PROM_AC4_MSB		0xB0
#define BMP085_REG_PROM_AC4_LSB		0xB1
#define BMP085_REG_PROM_AC5_MSB		0xB2
#define BMP085_REG_PROM_AC5_LSB		0xB3
#define BMP085_REG_PROM_AC6_MSB		0xB4
#define BMP085_REG_PROM_AC6_LSB		0xB5
#define BMP085_REG_PROM_B1_MSB		0xB6
#define BMP085_REG_PROM_B1_LSB		0xB7
#define BMP085_REG_PROM_B2_MSB		0xB8
#define BMP085_REG_PROM_B2_LSB		0xB9
#define BMP085_REG_PROM_MB_MSB		0xBA
#define BMP085_REG_PROM_MB_LSB		0xBB
#define BMP085_REG_PROM_MC_MSB		0xBC
#define BMP085_REG_PROM_MC_LSB		0xBD
#define BMP085_REG_PROM_MD_MSB		0xBE
#define BMP085_REG_PROM_MD_LSB		0xBF

#define BMP085_CHIP_ID_REG		0xD0
#define BMP085_VERSION_REG		0xD1

#define BMP085_SOFT_RESET_REG		0xE0

#define BMP085_CONTROL_REG		0xF4
#define BMP085_ADC_OUT_MSB_REG		0xF6
#define BMP085_ADC_OUT_LSB_REG		0xF7
#define BMP085_ADC_OUT_XLSB_REG		0xF8

/*
 * BMP085 Sensor Registers format
 */

/* BMP085_CHIP_ID_REG */
#define BMP085_CHIP_ID			0x55

/* BMP085_CONTROL_REG */
#define BMP085_TEMP_MEASURE        	0x2E
#define BMP085_PRESS_MEASURE        	0x34

/* calibration parameters */
#define BMP085_PARAM_MG      3038
#define BMP085_PARAM_MH     -7357
#define BMP085_PARAM_MI      3791
#define BMP085_PARAM_MJ     64385

/*
 *  Misc
 */

#define BMP085_I2C_ADDR		(0xEE>>1)
#define BMP085_NUM_DATA_TYPES  2

//#define BMP085_DEBUG
#ifdef BMP085_DEBUG
#if defined QDSP6
#define BMP085_ERR_MSG(msg)           \
  MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "BMP085 - " msg)
#define BMP085_ERR_MSG3(msg,p1,p2,p3) \
  MSG_3(MSG_SSID_SNS,DBG_ERROR_PRIO, "BMP085 - " msg,p1,p2,p3)
#else
#include "sns_debug_str.h"
#define BMP085_ERR_MSG(msg)           \
  SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define BMP085_ERR_MSG3(msg,p1,p2,p3) \
  SNS_PRINTF_STRING_ID_ERROR_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING_3,p1,p2,p3)
#endif
#else
#define BMP085_ERR_MSG(msg)
#define BMP085_ERR_MSG3(msg,p1,p2,p3)
#endif

#endif /* _BMP085_H_ */



