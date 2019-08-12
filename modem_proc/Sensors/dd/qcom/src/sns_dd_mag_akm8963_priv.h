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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8963_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/05/11     RY     Updated AKM_MAG8963_LO_PWR from 0 to 1 (uA).
09/15/11     RY     Modified for AK8963.
01/18/11     OF     Initial revision (based on sns_dd_mag_priv.h) 
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
03/24/11     OF     Adjust sensitivity values to the measurement data read out
==============================================================================*/

#ifndef _SNSD_MAG_AKM8963_PRIV_H
#define _SNSD_MAG_AKM8963_PRIV_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_util.h"
#include "fixed_point.h"

// AKM 8963 sensor defines
#define AKM8963_MAG_I2C_ADDR1                 0x0C
#define AKM8963_MAG_I2C_ADDR2                 0x0E
#define AKM8963_MAG_I2C_ADDR3                 0x0D
#define AKM8963_MAG_I2C_ADDR4                 0x0F

/* AKM 8963 Register Addresses */
#define AKM_MAG_REG_ADDR_DEVICEID_8963        0x00
#define AKM_MAG_REG_ADDR_INFO_8963            0x01
#define AKM_MAG_REG_ADDR_STATUS1_8963         0x02
#define AKM_MAG_REG_ADDR_HXL_8963             0x03
#define AKM_MAG_REG_ADDR_HXH_8963             0x04
#define AKM_MAG_REG_ADDR_HYL_8963             0x05
#define AKM_MAG_REG_ADDR_HYH_8963             0x06
#define AKM_MAG_REG_ADDR_HZL_8963             0x07
#define AKM_MAG_REG_ADDR_HZH_8963             0x08
#define AKM_MAG_REG_ADDR_STATUS2_8963         0x09
#define AKM_MAG_REG_ADDR_CNTL1_8963           0x0A
#define AKM_MAG_REG_ADDR_CNTL2_8963           0x0B
#define AKM_MAG_REG_ADDR_SELF_TEST_8963       0x0C
#define AKM_MAG_REG_ADDR_I2C_DISABLE_8963     0x0F
#define AKM_MAG_REG_ADDR_X_SEN_ADJUST_8963    0x10
#define AKM_MAG_REG_ADDR_Y_SEN_ADJUST_8963    0x11
#define AKM_MAG_REG_ADDR_Z_SEN_ADJUST_8963    0x12

#define AKM_MAG_8963_DEVICE_ID                0x48				

/* AKM Measurement Mode */
/* Output resolution is fixed to 16bit mode */
typedef enum {
	AKM_MAGNETOMETER_PWR_DOWN_MODE_8963   		= 0x00,
	AKM_MAGNETOMETER_SINGLE_MEAS_MODE_8963    	= 0x11,
	AKM_MAGNETOMETER_SELF_TEST_MODE_8963      	= 0x18,
	AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8963	= 0x0F, 
  
} mag8963_operation_mode_e ;

/* AKM INT bit mask */
#define AKM_MAGNETOMETER_DRDY_BIT_MASK     0x1

/* AKM Magnetic sensor overflow bit mask */
#define AKM_MAGNETOMETER_HOFL_BIT_MASK     0x8

/* Soft reset */
#define AKM_MAGNETOMETER_SOFT_RESET        0x1

/* AKM added */
#define AKM_MAGNETOMETER_ASTC_SELF_TEST_DISABLE	0x00
#define AKM_MAGNETOMETER_ASTC_SELF_TEST_ENABLE	0x40

/* AKM8963 Power consumption limits */
#define AKM_MAG8963_LO_PWR			1       /* unit of uA, actually 0.1 uA for AK8963 */
#define AKM_MAG8963_HI_PWR       	5000    /* unit of uA, When magnetic sensor is driven */

/* AKM8963 range */
#define AKM_MAG8963_MIN_RANGE	   	-3219128	/* Mag 8963 range min Q16 Gauss units -49.12G */
#define AKM_MAG8963_MAX_RANGE    	3219128		/* Mag 8963 range max Q16 Gauss units 49.12G*/

/* AKM8963 Resulution*/
#define AKM_MAG8963_RESOLUTION_ADC	16       /* 16 bit (0.15 uT / LSB)*/

/* AKM8963 number of data types*/
#define AKM_MAG8963_NUM_DATATYPES  	3

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
   SNSD_MGT8963_ST_IDLE,          /* 0 */
   SNSD_MGT8963_ST_STARTED,       /* 1; request started */
   SNSD_MGT8963_ST_TEST_SEQ1,
   SNSD_MGT8963_ST_TEST_SEQ2,
   SNSD_MGT8963_ST_TEST_SEQ3
} sns_akm_state_e;

/* State struct for AKM8963 driver */
typedef struct 
{
   sns_ddf_handle_t         smgr_hndl;                        	/* SDDI handle used to notify_data */               
   sns_ddf_device_access_s* dev_info;                        	/* lists of devices managed by this driver */
   sns_ddf_handle_t         port_handle;                      	/* handle used to access the I2C bus */
   sns_akm_state_e          curr_state;                       	/* stores the current state of the device */
   sns_ddf_powerstate_e     curr_pwr_state;                   	/* power state of the driver to be set by SMGR */
   q16_t                    data_cache[AKM_MAG8963_NUM_DATATYPES];	/* data cache for mag data to be declared Q16 type later*/ 
   q16_t                    sens_cache[AKM_MAG8963_NUM_DATATYPES];	/* sensitivity cache for mag data */ 
   float                 	 sens_adj[AKM_MAG8963_NUM_DATATYPES];	/* sensitivity adjusment cache for mag data */ 
   uint8_t			 device_id;				/* Device ID of AKM. should be 0x48 fixed */
   uint8_t			 device_info;				/* Device information for AKM */
   sns_ddf_timer_s 	 sns_dd_mag_tmr_obj;			/* timer obj to be used by this driver	*/
   sns_ddf_axes_map_s       axes_map;
} sns_dd_mag_akm8963_state_s;

#endif /* End include guard  _SNSD_MAG_AKM8963_PRIV_H */

