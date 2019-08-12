/*==============================================================================
 Copyright (c) 2014, AKM
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. Neither the name of the AKM nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==============================================================================*/

/*==============================================================================

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_fifo_akm099xx_priv.h#1 $

when         who     what, where, why
--------     ---     -----------------------------------------------------------
12/03/15     AVD     AKM_MAG099XX moving average filter disabled
11/19/15     JTL     Add info for better timestamping during continuous FIFO flush
11/14/15     MW      Polling jitter failure fix
10/09/15     PN      use avg sample period for timestamping
10/01/15     AKM     Timer update
08/25/15     AKM     Add AK09913/AK09916 support.
05/15/18     AH      Fix timestamp issue
04/01/15     PS      Enable uImage support for 8996v2
03/31/15     AKM     Updated max. electric current values, AKM_MAG099??_HI_PWR
03/31/15     AKM     Updated copyright message
01/28/15     PS      Disable uImage support
11/12/14     MW      Add UIMG_FUNCTIONS_AVAILABLE, COMPILE_FOR_HD22 flags
09/10/14     AKM     Add AK09914 and AK09915
01/31/13     AKM     AK09912
10/05/11     RY      Updated AKM_MAG8963_LO_PWR from 0 to 1 (uA).
09/15/11     RY      Modified for AK8963.
01/18/11     OF      Initial revision (based on sns_dd_mag_priv.h)
02/27/11     OF      Add Device ID and Device Info, read data in one 8 byte read
03/24/11     OF      Adjust sensitivity values to the measurement data read out
==============================================================================*/

#ifndef _SNSD_MAG_AKM099XX_PRIV_H
#define _SNSD_MAG_AKM099XX_PRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"

/* compile & debug definition */
#include "sns_dd_mag_fifo_akm099xx_debug.h"

/*=======================================================================

INTERNAL DEFINITIONS

========================================================================*/

#ifndef AKM_MAG_ENABLE_UIMG
/* Define extended DDF uImage functions if missing in HD22 package */
#define sns_ddf_malloc_ex(ptr, size, smgr_handle)                               sns_ddf_malloc(ptr, size)
#define sns_ddf_mfree_ex(ptr, smgr_handle)                                      sns_ddf_mfree(ptr)
#define sns_ddf_memhandler_malloc_ex(memhandler, size, smgr_handle)             sns_ddf_memhandler_malloc(memhandler, size)
#define sns_ddf_smgr_is_in_uimage()     false
#endif


#ifdef AKM_COMPILE_FOR_HD22
#define AKM099xx_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC
sns_ddf_driver_if_s sns_dd_vendor_if_1;
#define AKM099xx_FIFO_DD_IF sns_dd_vendor_if_1
#else
#define AKM099xx_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA
sns_ddf_driver_if_s sns_dd_mag_akm09914_akm09915_fn_list;
#define AKM099xx_FIFO_DD_IF sns_dd_mag_akm09914_akm09915_fn_list
#endif

/* First of all, define this device driver's name and version.
 * Make a version number. Each range must be within the following:
 * major: 7bit (0-127)
 * minor: 8bit (0-255)
 * revision: 16bit (0-32767)
 */
#define AKM_MAKE_DRV_VERSION(major, minor, revision) \
  (int32_t)(((0x7F&(major))<<24) | ((0xFF&(minor))<<16) | ((0xFFFF)&(revision)))

#define AKM_MAG_DRIVER_NAME    "AK099xx_fifo"
#define AKM_MAG_DRIVER_VERSION AKM_MAKE_DRV_VERSION(2,0,0)

#define AKM_MAG_DEVICE_VENDOR  "AKM"
#define AKM_MAG_DEVICE_NAME    "Magnetometer"
#define AKM_MAG_DEVICE_VERSION 1


/*
 * Referenced data sheet version
 * AK09916 data sheet version 015007392-E-00
 * AK09915 data sheet version 015006484-E-00
 * AK09914 data sheet version preliminary_E-01
 * AK09913 data sheet version 015007259-E-00
 * AK09912 data sheet version MS1547_E-02
 * AK09911 data sheet version MS1526_E-01
 */
/* 9350 usec maximum time is enough for every device.
 * section 8.3 in AK09916 data sheet
 *   TYP 7.2 ms MAX 8.2 ms
 * section 8.3 in AK09915 data sheet
 *   TYP 4.5 ms MAX 4.95 ms for low-power mode
 *   TYP 8.5 ms MAX 9.35 ms for low-noise mode (This driver doesn't implement this mode.)
 * section 8.3 in AK09914 data sheet
 *   TYP 7.2 ms MAX 8.5 ms
 * section 8.3 in AK09913 data sheet
 *   TYP 7.2 ms MAX 8.2 ms
 * section 5.3.3 in AK09912 data sheet
 *   TYP 7.2 ms MAX 8.5 ms
 * section 5.3.3 in AK09911 data sheet
 *   TYP 7.2 ms MAX 8.5 ms
 */
#define USEC_TIME_FOR_MEASUREMENT            8500 /* the longest delay value*/
#define USEC_TIME_FOR_MEASUREMENT_EXT        4950
#define USEC_TIME_FOR_DEVICE_DELAY           3600
#define USEC_TIME_FOR_DEVICE_DELAY_EXT       2250
#define USEC_TIME_FOR_TRIGGER_TIMER_HANDLER  100
#define USEC_TIME_FOR_RETRY                  100
#define NUM_MEASUREMENT_RETRY                25

/* This is the time that the device be ready for accepting a I2C/SPI command.
 * PORT value is referred.
 * section 8.2 in AK09916 data sheet
 * section 8.2 in AK09915 data sheet
 * section 8.2 in AK09914 data sheet
 * section 8.2 in AK09913 data sheet
 * section 5.3.2 in AK09912 data sheet
 * section 5.3.2 in AK09911 data sheet
 */
#define USEC_TIME_TO_ACTIVE                  100


/* Use define to avoid numbers in code */
#define DDF_WRITE_ONE_BYTE                   1
#define DDF_READ_ONE_BYTE                    1
#define DDF_WRITE_TWO_BYTES                  2
#define DDF_READ_TWO_BYTES                   2

// AKM sensor defines
#define AKM099XX_MAG_I2C_ADDR1               0x0C
#define AKM099XX_MAG_I2C_ADDR2               0x0E
#define AKM099XX_MAG_I2C_ADDR3               0x0D
#define AKM099XX_MAG_I2C_ADDR4               0x0F

/* AKM Register Addresses */
#define AKM_MAG_REG_ADDR_WIA1_099XX          0x00
#define AKM_MAG_REG_ADDR_WIA2_099XX          0x01
#define AKM_MAG_REG_ADDR_INFO1_099XX         0x02
#define AKM_MAG_REG_ADDR_INFO2_099XX         0x03

#define AKM_MAG_REG_ADDR_STATUS1_099XX       0x10
#define AKM_MAG_REG_ADDR_HXL_099XX           0x11
#define AKM_MAG_REG_ADDR_HXH_099XX           0x12
#define AKM_MAG_REG_ADDR_HYL_099XX           0x13
#define AKM_MAG_REG_ADDR_HYH_099XX           0x14
#define AKM_MAG_REG_ADDR_HZL_099XX           0x15
#define AKM_MAG_REG_ADDR_HZH_099XX           0x16
#define AKM_MAG_REG_ADDR_TEMPERATURE_099XX   0x17
#define AKM_MAG_REG_ADDR_STATUS2_099XX       0x18

#define AKM_MAG_REG_ADDR_CNTL1_099XX         0x30
#define AKM_MAG_REG_ADDR_CNTL2_099XX         0x31
#define AKM_MAG_REG_ADDR_CNTL3_099XX         0x32
#define AKM_MAG_REG_ADDR_I2C_DISABLE_099XX   0x36

#define AKM_MAG_REG_ADDR_X_SEN_ADJUST_099XX  0x60
#define AKM_MAG_REG_ADDR_Y_SEN_ADJUST_099XX  0x61
#define AKM_MAG_REG_ADDR_Z_SEN_ADJUST_099XX  0x62

#define AKM_MAG_COMPANY_ID                   0x48

/* AKM Measurement Mode */
typedef enum {
  AKM_MAG_PWR_DOWN_MODE_099XX        = 0x00,
  AKM_MAG_SINGLE_MEAS_MODE_099XX     = 0x01,
  AKM_MAG_CONT_MEAS_MODE_1_099XX     = 0x02, /*  10Hz */
  AKM_MAG_CONT_MEAS_MODE_2_099XX     = 0x04, /*  20Hz */
  AKM_MAG_CONT_MEAS_MODE_3_099XX     = 0x06, /*  50Hz */
  AKM_MAG_CONT_MEAS_MODE_4_099XX     = 0x08, /* 100Hz */
  AKM_MAG_CONT_MEAS_MODE_5_099XX     = 0x0A, /* 200Hz only AK09915 */
  AKM_MAG_CONT_MEAS_MODE_6_099XX     = 0x0C, /*   1Hz only AK09915 */
  AKM_MAG_SELF_TEST_MODE_099XX       = 0x10,
  AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX = 0x1F,
} mag099xx_operation_mode_e;

/* AKM INT bit mask */
#define AKM_MAG_DRDY_BIT_MASK               0x01

/* AKM DOR bit mask */
#define AKM_MAG_DOR_BIT_MASK                0x02

/* AKM Magnetic sensor overflow bit mask */
#define AKM_MAG_HOFL_BIT_MASK               0x08

/* No valid data in FIFO bit mask */
#define AKM_MAG_INV_BIT_MASK                0x04

/* CTRL1 settings */
#define AKM_MAG099XX_TEMPERATURE_MASK       0x80 /* valid for AK09912, AK09914 */
#define AKM_MAG099XX_NSF_BIT_MASK           0x00 /* valid for AK09912, AK09914, AK09915 */

/* CTRL2 settings */
#define AKM_MAG099XX_FIFO_BIT_MASK          0x80 /* valid for AK09914, AK09915 */
#define AKM_MAG099XX_SDR_BIT_MASK           0x40 /* valid for AK09915 */

/* Soft reset */
#define AKM_MAG_SOFT_RESET                  0x1

/* AKM099XX number of data types*/
#define AKM_MAG099XX_NUM_SUPPORTED_SENSORS  1
#define AKM_MAG099XX_NUM_DATATYPES          3
#define AKM_MAG099XX_NUM_SENSITIVITY        3
#define AKM_MAG099XX_NUM_DATA_BYTES         9
#define AKM_MAG099XX_NUM_SUPPORTED_ODR      4
#define AKM_MAG099XX_NUM_SUPPORTED_ODR_EXT  6

/*
 *  Supported ODRs.
 *  AK09915 supports 1Hz and 200Hz sampling mode.
 *  This driver doesn't implement these mode because there's no use case yet.
 */
#define AKM_MAG099XX_SUPPORTED_ODR_POWER_DOWN 0
#define AKM_MAG099XX_SUPPORTED_ODR_1HZ        1
#define AKM_MAG099XX_SUPPORTED_ODR_10HZ       10
#define AKM_MAG099XX_SUPPORTED_ODR_20HZ       20
#define AKM_MAG099XX_SUPPORTED_ODR_26HZ       26
#define AKM_MAG099XX_SUPPORTED_ODR_50HZ       50
#define AKM_MAG099XX_SUPPORTED_ODR_100HZ      100
#define AKM_MAG099XX_SUPPORTED_ODR_200HZ      200

/* AK099XX ODR tolerance, i.e. clock error */
#define AKM_MAG099XX_ODR_TOLERANCE   (10.0f)

/* AK099XX filter delay in usec. */
#define AKM_MAG099XX_FILTER_DELAY    0
#define AKM_MAG099XX_AVERAGING       1 /* MUST BE 1 or larger */

/* The following parameters are different from each devices. */

/* DEVICE_ID */
#define AKM_MAG09916_DEVICE_ID       0x09
#define AKM_MAG09915_DEVICE_ID       0x10
#define AKM_MAG09914_DEVICE_ID       0x07
#define AKM_MAG09913_DEVICE_ID       0x08
#define AKM_MAG09912_DEVICE_ID       0x04
#define AKM_MAG09911_DEVICE_ID       0x05

#define FLUSH_FIFO                   0

/* FIFO SIZE */
#define AKM_MAG09916_FIFO_SIZE       0
#define AKM_MAG09915_FIFO_SIZE       32
#define AKM_MAG09914_FIFO_SIZE       16
#define AKM_MAG09913_FIFO_SIZE       0
#define AKM_MAG09912_FIFO_SIZE       0
#define AKM_MAG09911_FIFO_SIZE       0

/* Power consumption limits */
#define AKM_MAG09916_LO_PWR          1    /* unit of uA */
#define AKM_MAG09916_HI_PWR          1100 /* unit of uA @ 100Hz */
#define AKM_MAG09915_LO_PWR          3    /* unit of uA */
#define AKM_MAG09915_HI_PWR          900  /* unit of uA @ 100Hz low power */
#define AKM_MAG09914_LO_PWR          3    /* unit of uA */
#define AKM_MAG09914_HI_PWR          1500 /* unit of uA @ 100Hz */
#define AKM_MAG09913_LO_PWR          3    /* unit of uA */
#define AKM_MAG09913_HI_PWR          1500 /* unit of uA @ 100Hz */
#define AKM_MAG09912_LO_PWR          3    /* unit of uA */
#define AKM_MAG09912_HI_PWR          1000 /* unit of uA @ 100Hz */
#define AKM_MAG09911_LO_PWR          3    /* unit of uA */
#define AKM_MAG09911_HI_PWR          2400 /* unit of uA @ 100Hz */

/* Maximum range in Q16 Gauss units */
#define AKM_MAG09916_MIN_RANGE       -3219128 /* Minimum -49.12G */
#define AKM_MAG09916_MAX_RANGE       3219128  /* Maximum  49.12G */
#define AKM_MAG09915_MIN_RANGE       -3219128 /* Minimum -49.12G */
#define AKM_MAG09915_MAX_RANGE       3219128  /* Maximum  49.12G */
#define AKM_MAG09914_MIN_RANGE       -1609864 /* Minimum -24.56G */
#define AKM_MAG09914_MAX_RANGE       1609864  /* Maximum  24.56G */
#define AKM_MAG09913_MIN_RANGE       -3219128 /* Minimum -49.12G */
#define AKM_MAG09913_MAX_RANGE       3219128  /* Maximum  49.12G */
#define AKM_MAG09912_MIN_RANGE       -3219128 /* Minimum -49.12G */
#define AKM_MAG09912_MAX_RANGE       3219128  /* Maximum  49.12G */
#define AKM_MAG09911_MIN_RANGE       -3219128 /* Minimum -49.12G */
#define AKM_MAG09911_MAX_RANGE       3219128  /* Maximum  49.12G */

/* Resulution */
#define AKM_MAG09916_RESOLUTION_ADC  16 /* 16 bit */
#define AKM_MAG09915_RESOLUTION_ADC  16 /* 16 bit */
#define AKM_MAG09914_RESOLUTION_ADC  16 /* 16 bit */
#define AKM_MAG09913_RESOLUTION_ADC  16 /* 16 bit */
#define AKM_MAG09912_RESOLUTION_ADC  16 /* 16 bit */
#define AKM_MAG09911_RESOLUTION_ADC  14 /* 14 bit */

/* Sampling frequency */
#define AKM_MAG09916_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_100HZ
#define AKM_MAG09915_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_200HZ
#define AKM_MAG09914_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_100HZ
#define AKM_MAG09913_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_100HZ
#define AKM_MAG09912_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_100HZ
#define AKM_MAG09911_SUPPORTED_ODR   AKM_MAG099XX_SUPPORTED_ODR_100HZ

/* Sensitivity of the magnetometer is:
 * Expressed in Gauss units (1 Tesla == 10000 Gauss, 1 microTesla == 0.01Gauss) */
#define AKM_MAG09916_SENSITIVITY     (0.0015f)  /* Gauss/LSB */
#define AKM_MAG09915_SENSITIVITY     (0.0015f)  /* Gauss/LSB */
#define AKM_MAG09914_SENSITIVITY     (0.00075f) /* Gauss/LSB */
#define AKM_MAG09913_SENSITIVITY     (0.0015f)  /* Gauss/LSB */
#define AKM_MAG09912_SENSITIVITY     (0.0015f)  /* Gauss/LSB */
#define AKM_MAG09911_SENSITIVITY     (0.006f)   /* Gauss/LSB */

/* FUSE ROM type */
typedef enum {
  SNSD_AKM_FUSE_NONE,  /* Device has no FUSE ROM */
  SNSD_AKM_FUSE_TYPEA, /* coef. = (ASA / 256.0f) + 0.5f */
  SNSD_AKM_FUSE_TYPEB, /* coef. = (ASA / 256.0f) + 1.0f */
  SNSD_AKM_FUSE_TYPEC, /* coef. = (ASA / 128.0f) + 0.5f */
} sns_akm_fuse_type_e;

#define AKM099XX_SOFTIRON_ENABLE  0x64 /* Other values make soft-iron correction disable. */

/*==============================================================================
  State struct for driver
==============================================================================*/
typedef struct
{
  sns_ddf_handle_t          smgr_handle;     /* SMGR handle */
  sns_ddf_handle_t          port_handle;     /* Port handle for bus access */
  sns_ddf_timer_s           sns_dd_tmr_obj;  /* timer obj */
  uint32_t                  gpio1;           /* GPIO for interrupt */

  /* parameters to show the current status of the device/driver. */
  sns_ddf_powerstate_e      curr_powerstate;    /* power state of the driver to be set by SMGR.
                                                   In case of this driver, while some operation is
                                                   on going, this state is set to ACTIVE even if
                                                   the device state is power-down mode. */
  mag099xx_operation_mode_e curr_mode;  /* stores the current operation mode of the device*/
  sns_ddf_odr_t             curr_odr;   /* current ODR setting in continuous measurement mode
                                           If this value is larger than 0, it represents the
                                           device is in continuous measurement mode. */


  /* platform information */
  sns_ddf_axes_map_s axes_map;


  /* parameters which are determined when the connected device is specified. */
  uint8_t             device_select; /* store the current connected device.
                                        This value holds the same number as WIA2. */
  sns_akm_fuse_type_e fuse_type;     /* the type of FUSE ROM */
  float               sens_adj_flt[AKM_MAG099XX_NUM_SENSITIVITY]; /* sensitivity adjusment cache for mag data */
  float               mag_scale_flt; /* scale factor which converts from sensor unit to SI */
  bool                has_drdy;      /* true if the device has DRDY pin */
  bool                has_nsf;       /* true if the device has nsf */
  bool                extended_odr;  /* if the device support extended odr, then true. */


  /* parameters for measurement */
  q16_t          data_cache[AKM_MAG099XX_NUM_DATATYPES * AKM_MAG099XX_AVERAGING];
                                 /* data cache for mag data */
  uint8_t        num_cached;     /* the number of data in data_cache */
  uint8_t        retry;          /* measurement retried number. this value will increase
                                    if measurement is not done (i.e. DRDY is low) */
  float          avg_sampling_intvl;  /* Running average of sampling interval */
  float          sampling_intvl_factor; /* Saved value for 1000000/curr_odr/avg_sampling_intvl */
  sns_ddf_time_t time_prev_int;  /* timestamp of the last interrupt. NOT INCLUDE SAMPLE DELAY. */
  uint32_t       num_samples_since_prev_int; /* Number of samples received since the "time_prev_int"
                                                was set */
  double         time_prev_smgr_ev; /* Timestamp of the most recent event sent to SMGR. NOT INCLUDE SAMPLE DELAY. */
  sns_ddf_time_t sample_delay;   /* delay value of hardware and averaging.
                                    The unit is same as timestamp (tick) */


  /* parameters for fifo */
  uint8_t device_fifo_size;    /* the size of fifo.
                                  0: device does not support fifo.
                                  +: device supports fifo. */
  uint16_t water_mark;         /* current water mark (WM) setting for fifo operation.
                                  0: fifo is disabled.
                                  +: fifo is activated */
  uint16_t previous_water_mark;
} sns_dd_mag_akm099xx_state_s;


/*==============================================================================
  Utility function both bigImage and uImage (implement in uimg.c file).
==============================================================================*/
sns_ddf_status_e sns_mgt_akm_set_operation_mode_099xx(
    sns_dd_mag_akm099xx_state_s *state,
    mag099xx_operation_mode_e   mode
);

sns_ddf_status_e sns_mgt_akm_sample_fifo_099xx(
    sns_dd_mag_akm099xx_state_s *state,
    uint16_t                    num_samples,
    sns_ddf_time_t              ts_now,
    bool                        in_interrupt_handler
);

/*==============================================================================
  DDF API for bigImage
==============================================================================*/
sns_ddf_status_e sns_dd_akm099xx_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors
);

sns_ddf_status_e sns_dd_akm099xx_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

sns_ddf_status_e sns_dd_akm099xx_set_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
);

sns_ddf_status_e sns_dd_akm099xx_get_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    sns_ddf_memhandler_s* memhandler,
    void**               value,
    uint32_t*            num_elems
);

void sns_dd_akm099xx_handle_timer(
    sns_ddf_handle_t dd_handle,
    void* arg
);

void sns_dd_akm099xx_handle_irq(
    sns_ddf_handle_t dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_akm099xx_reset(
    sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_akm099xx_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    bool              enable
);

sns_ddf_status_e sns_dd_akm099xx_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err
);

sns_ddf_status_e sns_dd_akm099xx_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
);

sns_ddf_status_e sns_dd_akm099xx_trigger_fifo_data(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    uint16_t                 num_samples,
    bool                     trigger_now
);

#endif /* End include guard  _SNSD_MAG_AKM099XX_PRIV_H */
