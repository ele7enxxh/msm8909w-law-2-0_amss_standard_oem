/*==============================================================================

         S E N S O R S    M A G N E T O M E T E R    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the qcom compass driver 


Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/07/10     RK     Update the driver to work with latest DDF
10/09/10     RK     Added the step size as shifts constant 
11/18/2010   sj     Incorporating code review comments
01/28/2011   br     deleted SNS_ASSERT related definitions
02/08/2011   ag     Add AKM8975 Magnetometer support (original checkin by osnat)
==============================================================================*/

#ifndef _SNSD_MAG_PRIV_H
#define _SNSD_MAG_PRIV_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"

/* AKM 8973 Register Addresses */
#define AKM_MAG_REG_ADDR_STATUS_8973     0xC0
#define AKM_MAG_REG_ADDR_TMPS_8973       0xC1
#define AKM_MAG_REG_ADDR_H1X_8973        0xC2
#define AKM_MAG_REG_ADDR_H1Y_8973        0xC3
#define AKM_MAG_REG_ADDR_H1Z_8973        0xC4
#define AKM_MAG_REG_ADDR_MODE_8973       0xE0
#define AKM_MAG_REG_ADDR_HXDA_8973       0xE1
#define AKM_MAG_REG_ADDR_HYDA_8973       0xE2
#define AKM_MAG_REG_ADDR_HZDA_8973       0xE3
#define AKM_MAG_REG_ADDR_HXGA_8973       0xE4
#define AKM_MAG_REG_ADDR_HYGA_8973       0xE5
#define AKM_MAG_REG_ADDR_HZGA_8973       0xE6


/* AKM EEPROM Gain address */
#define AKM_MAGNETOMETER_EEPROM_GAIN_8973 0x66

/* AKM x-axis offset DAC */
#define AKM_MAGNETOMETER_X_OFFSET   0x80
#define AKM_MAGNETOMETER_Y_OFFSET   0x80
#define AKM_MAGNETOMETER_Z_OFFSET   0x80

/* AKM Measurement Mode */
#define AKM_MAGNETOMETER_MEAS_MODE_8973     0x0
#define AKM_MAGNETOMETER_EEPROM_MODE_8973   0x2
#define AKM_MAGNETOMETER_PWR_DOWN_MODE_8973 0x3

/* AKM INT bit mask */
#define AKM_MAGNETOMETER_INT_BIT_MASK     0x1

/* AKM Gain setting */
#define AKM_MAGNETOMETER_GAIN_268   0x0
#define AKM_MAGNETOMETER_GAIN_272   0x1
#define AKM_MAGNETOMETER_GAIN_276   0x2
#define AKM_MAGNETOMETER_GAIN_280   0x3
#define AKM_MAGNETOMETER_GAIN_284   0x4
#define AKM_MAGNETOMETER_GAIN_288   0x5
#define AKM_MAGNETOMETER_GAIN_292   0x6
#define AKM_MAGNETOMETER_GAIN_296   0x7
#define AKM_MAGNETOMETER_GAIN_300   0x8
#define AKM_MAGNETOMETER_GAIN_304   0x9
#define AKM_MAGNETOMETER_GAIN_308   0xA
#define AKM_MAGNETOMETER_GAIN_312   0xB
#define AKM_MAGNETOMETER_GAIN_316   0xC
#define AKM_MAGNETOMETER_GAIN_320   0xD
#define AKM_MAGNETOMETER_GAIN_324   0xE
#define AKM_MAGNETOMETER_GAIN_328   0xF

/* AKM magnetic field reading, range is -128uT to +128uT, 1uT/code */
#define AKM_MAGNETOMETER_MIN_MAG    -128

/* AKM Micro-Tesla per DAC Step */
#define AKM_MAGNETOMETER_DAC_STEP   16
#define AKM_MAGNETOMETER_DAC_STEP_SHIFT 4

/* AKM DAC offsets */
#define AKM_MAGNETOMETER_DAC_POS_OFFSET_0  0x80

/* AKM value for kicking off DAC auto-adjust */
#define AKM_MAGNETOMETER_DAC_LOW_BOUND    -112
#define AKM_MAGNETOMETER_DAC_HIGH_BOUND   112

/* AKM value for stopping DAC auto-adjust */
#define AKM_MAGNETOMETER_DAC_DONE_LOW_BOUND    -32
#define AKM_MAGNETOMETER_DAC_DONE_HIGH_BOUND   32

/* Supported datatypes */
typedef enum
{
   SNS_MGT_X = 0,
   SNS_MGT_Y = 1,
   SNS_MGT_Z = 2,
   SNS_MGT_NUM_DATATYPES = 3
} sns_dd_mgt_data_e;

/* State Enum when Honeywell mag sensor is used */
typedef enum
{
   SNSD_MGT_ST_IDLE,          /* 0 = initial state of driver */
   SNSD_MGT_ST_STARTED,       /* 1 = request for data started */
} sns_akm_state_e;

/* State struct for AKM driver */
typedef struct 
{
   sns_ddf_handle_t        smgr_hndl;                        /* SDDI handle used to notify_data */               
   sns_ddf_device_access_s *dev_info;                        /* lists of devices managed by this driver */
   sns_ddf_handle_t        port_handle;                      /* handle used to access the I2C bus */
   sns_akm_state_e         curr_state;                       /* stores the current state of the device */
   sns_ddf_powerstate_e    curr_pwr_state;                   /* power state of the driver to be set by SMGR */
   q16_t                   data_cache[SNS_MGT_NUM_DATATYPES];/* data cache for mag data  to be declared Q16 type later*/ 
   q16_t                   temp_data_cache;                  /* data cache for temp sensor data to be declared Q16 type later*/ 
   uint8_t                 num_data;                         /* Indicates number of data types requested. = 4 when mag+temp
                                                                is requested */
} sns_dd_mag_akm_state_s;

#endif /* End include guard  SNSD_MAG_PRIV_H */
