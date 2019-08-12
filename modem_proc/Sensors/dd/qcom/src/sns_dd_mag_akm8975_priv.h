/*==============================================================================

         S E N S O R S    M A G N E T O M E T E R    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the qcom compass driver 


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8975_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
01/18/11     OF     Initial revision (based on sns_dd_mag_priv.h) 
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
03/24/11     OF     Adjust sensitivity values to the measurement data read out 
08/28/11     AG     Added self test functionality 
==============================================================================*/

#ifndef _SNSD_MAG_AKM8975_PRIV_H
#define _SNSD_MAG_AKM8975_PRIV_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_util.h"
#include "fixed_point.h"

// AKM 8975 sensor defines
#define AKM8975_MAG_I2C_ADDR1                 0x0C
#define AKM8975_MAG_I2C_ADDR2                 0x0E
#define AKM8975_MAG_I2C_ADDR3                 0x0D
#define AKM8975_MAG_I2C_ADDR4                 0x0F

/* AKM 8975 Register Addresses */
#define AKM_MAG_REG_ADDR_DEVICEID_8975        0x00
#define AKM_MAG_REG_ADDR_INFO_8975            0x01
#define AKM_MAG_REG_ADDR_STATUS1_8975         0x02
#define AKM_MAG_REG_ADDR_HXL_8975             0x03
#define AKM_MAG_REG_ADDR_HXH_8975             0x04
#define AKM_MAG_REG_ADDR_HYL_8975             0x05
#define AKM_MAG_REG_ADDR_HYH_8975             0x06
#define AKM_MAG_REG_ADDR_HZL_8975             0x07
#define AKM_MAG_REG_ADDR_HZH_8975             0x08
#define AKM_MAG_REG_ADDR_STATUS2_8975         0x09
#define AKM_MAG_REG_ADDR_CNTL_8975            0x0A
#define AKM_MAG_REG_ADDR_SELF_TEST_8975       0x0C
#define AKM_MAG_REG_ADDR_I2C_DISABLE_8975     0x0F
#define AKM_MAG_REG_ADDR_X_SEN_ADJUST_8975    0x10
#define AKM_MAG_REG_ADDR_Y_SEN_ADJUST_8975    0x11
#define AKM_MAG_REG_ADDR_Z_SEN_ADJUST_8975    0x12

#define AKM_MAG_8975_DEVICE_ID				0x48				

/* AKM Measurement Mode */
typedef enum {
	AKM_MAGNETOMETER_PWR_DOWN_MODE_8975   		= 0x00,
	AKM_MAGNETOMETER_SINGLE_MEAS_MODE_8975    	= 0x01,
	AKM_MAGNETOMETER_SELF_TEST_MODE_8975      	= 0x08,
	AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8975	= 0x0F, 
  
} mag8975_operation_mode_e ;

/* AKM INT bit mask */
#define AKM_MAGNETOMETER_DRDY_BIT_MASK     0x1

/* AKM Magnetic sensor overflow bit mask */
#define AKM_MAGNETOMETER_HOFL_BIT_MASK     0x8

/* AKM Self Test Register */
#define AKM_MAGNETOMETER_ASTC_SELF_TEST_ENABLE  0x40
#define AKM_MAGNETOMETER_ASTC_SELF_TEST_DISABLE 0x00

/* AKM8975 Power consumption limits */
#define AKM_MAG8975_LO_PWR			3       /* unit of uA */
#define AKM_MAG8975_HI_PWR       	6000    /* unit of uA, When magnetic sensor is driven */

/* AKM8975 range */
#define AKM_MAG8975_MIN_RANGE	   	-805306 /* Mag 8975 range min Q16 Gauss units -12.29mT */
#define AKM_MAG8975_MAX_RANGE    	805306  /* Mag 8975 range max Q16 Gauss units 12.29mT*/

/* AKM8975 Resulution*/
#define AKM_MAG8975_RESOLUTION_ADC	13       /* 13 bit (0.3 ÏT / LSB)*/

/* AKM8975 number of data types*/
#define AKM_MAG8975_NUM_DATATYPES  	3

typedef enum
{
  SNSD_MAG_ERR_GENERIC,
  SNSD_MAG_ERR_I2C,
  SNSD_MAG_ERR_TIMER,
  SNSD_MAG_ERR_DRDY,
  SNSD_MAG_ERR_OVFL,
  SNSD_MAG_ERR_RESULT_OUT_OF_RANGE,
} sns_akm_err_codes_e;

/* State Enum when Honeywell mag sensor is used */
typedef enum
{
   SNSD_MGT8975_ST_IDLE,          /* 0 */
   SNSD_MGT8975_ST_STARTED,       /* 1; request started */
   SNSD_MGT8975_ST_TEST_STARTED,  /* 2; self test started */
} sns_akm_state_e;

/* State struct for AKM8975 driver */
typedef struct 
{
   sns_ddf_handle_t          smgr_hndl;                        	  /* SDDI handle used to notify_data */               
   sns_ddf_device_access_s*  dev_info;                        	  /* lists of devices managed by this driver */
   sns_ddf_handle_t          port_handle;                      	  /* handle used to access the I2C bus */
   sns_akm_state_e           curr_state;                       	  /* stores the current state of the device */
   sns_ddf_powerstate_e      curr_pwr_state;                         /* power state of the driver to be set by SMGR */
   q16_t                     data_cache[AKM_MAG8975_NUM_DATATYPES];  /* data cache for mag data to be declared Q16 type later*/ 
   q16_t                     sens_cache[AKM_MAG8975_NUM_DATATYPES];  /* sensitivity cache for mag data */ 
   float                     sens_adj[AKM_MAG8975_NUM_DATATYPES]; /* sensitivity adjusment cache for mag data */ 
   uint8_t                   device_id;                           /* Device ID of AKM. should be 0x48 fixed */
   uint8_t                   device_info;                         /* Device information for AKM */
   sns_ddf_timer_s           sns_dd_mag_tmr_obj;                  /* timer obj to be used by this driver*/
   sns_ddf_axes_map_s        axes_map;

} sns_dd_mag_akm8975_state_s;


/* Temporary, until we will have release will all DDF changes available*/
//#define USE_CC41

/*
 * From the table 8.3 of the magnetometer datasheet we see
 * that the sensitivity of the magnetometer is 0.3 microTesla/LSB.
 * Expressed in Gauss units (1 Tesla == 10000 Gauss, 1 microTesla == 0.01Gauss)
 * this means that the sensitivity is 0.003 Gauss/LSB.
 */
#define TESLA_TO_GAUSS 10000 // Gauss

/* The FX_FLTTOFIX_Q16 macro provides data in Q16 format as output */
static q16_t mag_akm8975_scale = FX_FLTTOFIX_Q16(0.3 * TESLA_TO_GAUSS / 1000000);

#define AKM8975_RAW_TO_GAUSS(x) (x * mag_akm8975_scale)

// 9 msec maximum time per section 5.3.3 in AKM8975 data sheet version MS1187-E-02
#define USEC_TIME_FOR_MEASUREMENT_8975 9000

#define NUM_SENSORS_SUPPORTED_BY_AK8975 1

// Should be commented when checked-in
//#define MAG8975_DEBUG

#ifdef MAG8975_DEBUG
#define MAG8975_DBG_MSG_0( msg )          \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg) )
#define MAG8975_DBG_MSG_1( msg, p1 )            \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1 )
#define MAG8975_DBG_MSG_2( msg, p1, p2 )        \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1, p2 )
#define MAG8975_DBG_MSG_3( msg, p1, p2, p3 )          \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1, p2, p3 )
#else
#define MAG8975_DBG_MSG_0( msg )
#define MAG8975_DBG_MSG_1( msg, p1 )
#define MAG8975_DBG_MSG_2( msg, p1, p2 )
#define MAG8975_DBG_MSG_3( msg, p1, p2, p3 )
#endif

#define MAG8975_SELF_TEST_DEBUG

#ifdef MAG8975_SELF_TEST_DEBUG
#define MAG8975_SMGR_DBG_MSG_0( msg )          \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_SMGR, (msg) )
#define MAG8975_SMGR_DBG_MSG_1( msg, p1 )            \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_SMGR, (msg), p1 )
#define MAG8975_SMGR_DBG_MSG_2( msg, p1, p2 )        \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2 )
#define MAG8975_SMGR_DBG_MSG_3( msg, p1, p2, p3 )          \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2, p3 )
#else 
#define MAG8975_SMGR_DBG_MSG_0( msg )
#define MAG8975_SMGR_DBG_MSG_1( msg, p1 )
#define MAG8975_SMGR_DBG_MSG_2( msg, p1, p2 )
#define MAG8975_SMGR_DBG_MSG_3( msg, p1, p2, p3 )
#endif 

// Use define to avoid numbers in code
#define DDF_WRITE_ONE_BYTE    1
#define DDF_READ_ONE_BYTE     1
#define SENSITIVITY_NUM_BYTES 3
#define MAG_DATA_NUM_BYTES    8


#endif /* End include guard  _SNSD_MAG_AKM8975_PRIV_H */
