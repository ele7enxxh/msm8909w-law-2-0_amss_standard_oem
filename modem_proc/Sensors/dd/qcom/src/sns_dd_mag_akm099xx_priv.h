/*==============================================================================

         S E N S O R S    M A G N E T O M E T E R    D R I V E R

DESCRIPTION

  Defines the interface for the magnetometer driver


-- Copyright Notice --

  Copyright (c) 2004 Asahi Kasei Microdevices Corporation, Japan
  All Rights Reserved.

-- End Asahi Kasei Microdevices Copyright Notice --
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm099xx_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
01/31/13     AKM    AK09912
10/05/11     RY     Updated AKM_MAG8963_LO_PWR from 0 to 1 (uA).
09/15/11     RY     Modified for AK8963.
01/18/11     OF     Initial revision (based on sns_dd_mag_priv.h)
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
03/24/11     OF     Adjust sensitivity values to the measurement data read out
==============================================================================*/

#ifndef _SNSD_MAG_AKM099XX_PRIV_H
#define _SNSD_MAG_AKM099XX_PRIV_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_util.h"
#include "fixed_point.h"

// AKM sensor defines
#define AKM099XX_MAG_I2C_ADDR1                 0x0C
#define AKM099XX_MAG_I2C_ADDR2                 0x0E
#define AKM099XX_MAG_I2C_ADDR3                 0x0D
#define AKM099XX_MAG_I2C_ADDR4                 0x0F

/* AKM Register Addresses */
#define AKM_MAG_REG_ADDR_WIA1_099XX            0x00
#define AKM_MAG_REG_ADDR_WIA2_099XX            0x01
#define AKM_MAG_REG_ADDR_INFO1_099XX           0x02
#define AKM_MAG_REG_ADDR_INFO2_099XX           0x03

#define AKM_MAG_REG_ADDR_STATUS1_099XX         0x10
#define AKM_MAG_REG_ADDR_HXL_099XX             0x11
#define AKM_MAG_REG_ADDR_HXH_099XX             0x12
#define AKM_MAG_REG_ADDR_HYL_099XX             0x13
#define AKM_MAG_REG_ADDR_HYH_099XX             0x14
#define AKM_MAG_REG_ADDR_HZL_099XX             0x15
#define AKM_MAG_REG_ADDR_HZH_099XX             0x16
#define AKM_MAG_REG_ADDR_TEMPERATURE_099XX     0x17
#define AKM_MAG_REG_ADDR_STATUS2_099XX         0x18

#define AKM_MAG_REG_ADDR_CNTL1_099XX           0x30
#define AKM_MAG_REG_ADDR_CNTL2_099XX           0x31
#define AKM_MAG_REG_ADDR_CNTL3_099XX           0x32
#define AKM_MAG_REG_ADDR_I2C_DISABLE_099XX     0x36

#define AKM_MAG_REG_ADDR_X_SEN_ADJUST_099XX    0x60
#define AKM_MAG_REG_ADDR_Y_SEN_ADJUST_099XX    0x61
#define AKM_MAG_REG_ADDR_Z_SEN_ADJUST_099XX    0x62

#define AKM_MAG_COMPANY_ID                     0x48

/* AKM Measurement Mode */
typedef enum {
  AKM_MAG_PWR_DOWN_MODE_099XX                = 0x00,
  AKM_MAG_SINGLE_MEAS_MODE_099XX             = 0x01,
  AKM_MAG_SELF_TEST_MODE_099XX               = 0x10,
  AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX         = 0x1F,
} mag099xx_operation_mode_e ;

/* AKM INT bit mask */
#define AKM_MAG_DRDY_BIT_MASK                  0x1

/* AKM Magnetic sensor overflow bit mask */
#define AKM_MAG_HOFL_BIT_MASK                  0x8

/* Soft reset */
#define AKM_MAG_SOFT_RESET                     0x1

/* AKM099XX number of data types*/
#define AKM_MAG099XX_NUM_SUPPORTED_SENSORS     1
#define AKM_MAG099XX_NUM_DATATYPES             3
#define AKM_MAG099XX_NUM_SENSITIVITY           3
#define AKM_MAG099XX_NUM_DATA_BYTES            9

/* The following parameters are different from each devices. */

/* DEVICE_ID */
#define AKM_MAG09912_DEVICE_ID                 0x04
#define AKM_MAG09911_DEVICE_ID                 0x05

/* AKM09912 option settings */
#define AKM_MAG09912_TEMPERATURE_MASK          0x80
#define AKM_MAG09912_NSF_BIT_MASK              0x40

/* AKM09912 power consumption limits */
#define AKM_MAG09912_LO_PWR     3       /* unit of uA */
#define AKM_MAG09912_HI_PWR     100     /* unit of uA @ 10Hz */
/* AKM09911 power consumption limits */
#define AKM_MAG09911_LO_PWR     3       /* unit of uA */
#define AKM_MAG09911_HI_PWR     240     /* unit of uA @ 10Hz */

/* AKM09912 range */
#define AKM_MAG09912_MIN_RANGE -3219128 /* Minimum range in Q16 Gauss units -49.12G */
#define AKM_MAG09912_MAX_RANGE  3219128 /* Maximum range in Q16 Gauss units  49.12G */
/* AKM09911 range */
#define AKM_MAG09911_MIN_RANGE -3219128 /* Minimum range in Q16 Gauss units -49.12G */
#define AKM_MAG09911_MAX_RANGE  3219128 /* Maximum range in Q16 Gauss units  49.12G */

/* AKM09912 resulution */
#define AKM_MAG09912_RESOLUTION_ADC 16       /* 16 bit (0.15 uT / LSB) */
/* AKM09911 resulution */
#define AKM_MAG09911_RESOLUTION_ADC 14       /* 14 bit (0.6  uT / LSB) */

/*
 * From the table 5.3.3 of the magnetometer datasheet we see
 * that the sensitivity of the magnetometer is:
 * Expressed in Gauss units (1 Tesla == 10000 Gauss, 1 microTesla == 0.01Gauss) */
/*  AKM09912 sensitivity: 0.15 microTesla/LSB */
#define AKM_MAG09912_SENSITIVITY    (0.15f)   /* uT/LSB */
/*  AKM09911 sensitivity: 0.6  microTesla/LSB */
#define AKM_MAG09911_SENSITIVITY    (0.6f)    /* uT/LSB */


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
  SNSD_MAG_ST_IDLE,          /* 0 */
  SNSD_MAG_ST_STARTED,       /* 1; request started */
  SNSD_MAG_ST_TEST_SEQ1,
  SNSD_MAG_ST_TEST_SEQ2
} sns_akm_state_e;

/* State struct for AKM099XX driver */
typedef struct
{
  sns_ddf_handle_t         smgr_hndl;                              /* SDDI handle used to notify_data */               
  sns_ddf_device_access_s* dev_info;                               /* lists of devices managed by this driver */
  uint8_t                  device_select;
  sns_ddf_handle_t         port_handle;                            /* handle used to access the I2C bus */
  sns_akm_state_e          curr_state;                             /* stores the current state of the device */
  sns_ddf_powerstate_e     curr_pwr_state;                         /* power state of the driver to be set by SMGR */
  sns_ddf_axes_map_s       axes_map;
  sns_ddf_timer_s          sns_dd_mag_tmr_obj;                     /* timer obj to be used by this driver  */
  q16_t                    data_cache[AKM_MAG099XX_NUM_DATATYPES]; /* data cache for mag data to be declared Q16 type later*/ 
  float                    sens_adj[AKM_MAG099XX_NUM_DATATYPES];   /* sensitivity adjusment cache for mag data */ 
  float                    mag_scale_flt;                          /* scale factor which converts from sensor unit to SI */
  uint8_t                  retry;                                  /* measurement retried number. this value will increase 
                                                                                                         if measurement is not done (i.e. DRDY is low) */
} sns_dd_mag_akm099xx_state_s;

#endif /* End include guard  _SNSD_MAG_AKM099XX_PRIV_H */

