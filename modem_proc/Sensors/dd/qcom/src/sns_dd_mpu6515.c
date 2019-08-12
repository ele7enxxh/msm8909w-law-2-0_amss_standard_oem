/*
 * @file sns_dd_mpu6515.c
 *
 *
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-07-09  AH  Fix compiler warnings
  2014-07-22  MW  Eliminate compiler warning
  2014-03-12 lka  Modified gyro bias conversion macro.
  2014-03-05 lka  Fixed critical Klocwork errors.
  2014-02-27 lka  Modified ATTRIB_RANGE to use set_config_register().
  2014-02-25 lka  Fixed attributes.
  2014-02-12 lka  Fixed gyro bias conversion.
  2014-02-20  sd  Do not configure MAG when only pressure is requested to save power
  2014-01-28 lka  Fixed Klocwork errors.
  2014-01-09 lka  Added DMP heartbeat as trigger for device recovery.
  2013-12-27 pn   - Updated mpu6515_trigger_fifo_data()
                  - Corrected number of samples in FIFO log packets
                  - Restored changes to MD/DMP concurrency
  2013-12-23 pn   Modified sns_dd_dmp_read_fifo()
  2013-12-19 lka  Fixed gyro bias conversion for DMP.
  2013-12-09  sd  Added general FIFO suport/removed Fix for a MD/DMP concurrency case"
  2013-11-15 lka  Fixed a MD/DMP concurrency case.
  2013-10-22 lka  Fixed DDF handle in recovery mechanism.
  2013-10-16 lka  Added NULL pointer checks.
  2013-10-11 lka  Modified burst-read for register dump to one-byte reads.
                  Added workaround to enforce POWERSTATE_ACTIVE for Accel
                  when streaming.
  2013-10-02 lka  Fixed a race condition between Pressure/Mag enabling and
                  MD interrupt configuration.
  2013-10-02 lka  Fixed MD interrupt configuration to avoid false detections.
  2013-09-29 lka  Added debug info for MPU and DMP registers.
                  Added DD recovery code (device hard-reset).
  2013-09-23 pn   Initializes last_sample_timestamp for all enabled sensors
  2013-09-13 pn   Prevents out-of-bound indexing of parsed_fifo_info[]
  2013-09-11 lka  Uncommented code for temperature compensation on BMP280 pressure sensor 
  2013-09-09 lka  Re-arranged I2C slave configuration setup sequence.
  2013-09-09 ad   Fix compiler warnings
  2013-09-06 pn   Fixed timestamp calculation for FIFO data
  2013-09-05 lka  Fixed gyro power state.
                  Added workaround for MPU register corruption.
  2013-09-04 sd   Fixed FIFO count mask
  2013-09-03 sd   reduce the delay in dmp setup  
  2013-09-03 pn   Restores DMP state when disabling MD interrupt
  2013-08-30 ad   Fix SMD wakeup failure from stationary state
  2013-08-28 pn   Relies on interrupt register value in deciding whether to deregister ISR
  2013-08-28 lka  Disables gyro when device is in MD interrupt mode.
                  Modified AKM/BMP configuration sequence.
  2013-08-26 pn   Fixed issue with interrupt not getting deregistered in some cases.
  2013-08-21 sd   increase FIFO max wm for one client;
  2013-08-19 lka  Re-arranged sequence of enabling pressure/mag.
  2013-08-08 lka  Made reset_sub_dev not to return immediately upon error.
  2013-08-01 lka  Added signal_deregister() code when no entity uses interrupt pin.
                  Fixed return value when returning during bypass error.
  2013-07-26 lka  Added aux device init delay and exit-bypass-mode code.
  2013-07-23 asr  Code Cleanup
  2013-07-14 lka  Added functions to modify MPU registers efficiently.
                  Removed unnecessary comments.
                  Removed limitation of Mag being enabled first before Pressure.
  2013-07-12 lka  Various fixes
  2013-07012 asr  Generalized support for akm8963/bmp280 i2c slave devices in DMP Passthrough configuration and cleaned up code.
  2013-07-12 sd   Using regsitry for axe map
                  Using registry i2c address for slave sensor (mag)
  2013-07-11 pn   Minor changes for HW selftest  
  2013-07-06 sd   Keeps parsing FIFO data if invalid header found
                  keeps parsing FIFO data even when FIFO is full;
  2013-07-01 sd   Fixed wrong range in get attribute 
  2013-06-28 lka  Added support for AKM8963 pass-through.
  2013-06-20 lka  Initial version

==============================================================================*/

#include "sns_dd_mpu6515_priv.h"
#include "sns_dd_mag_akm8963_priv.h"
#include "sns_dd_alt_bmp280.h"

#include <stdbool.h>
#include <stdio.h>
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"

#define AKM8963_SENSITIVITY		(0.15f)   // uT/LSB
#define UTESLA_TO_GAUSS			(0.01f)   // Convert from uT to Gauss


/* Static variables */
/* The FX_FLTTOFIX_Q16 macro provides data in Q16 format as output */
static float mag_akm8963_scale_flt = (AKM8963_SENSITIVITY * UTESLA_TO_GAUSS);
#define AKM8963_RAW_TO_GAUSS_FLT(raw)	((raw) * mag_akm8963_scale_flt)



sns_ddf_status_e mpu6515_init_akm8963_slave( sns_ddf_handle_t hndl,
                                                    sns_ddf_nv_params_s*    nv_params,
                                                    sns_ddf_device_access_s device_info[],
                                                    sns_ddf_memhandler_s*   memhandler);

sns_ddf_status_e mpu6515_init_bmp280_slave(  sns_ddf_handle_t hndl,
                                                    sns_ddf_nv_params_s*    nv_params,
                                                    sns_ddf_device_access_s device_info[],
                                                    sns_ddf_memhandler_s*   memhandler);

/* External References */
extern volatile boolean EnableI2C;
extern q16_t mpu6515_gyro_lpf[7];

// Forward declaration of driver interface functions.
sns_ddf_driver_if_s sns_dd_mpu6515_if;
extern sns_ddf_driver_if_s sns_mag_akm8963_driver_fn_list;
extern sns_ddf_driver_if_s sns_alt_bmp280_driver_fn_list;

// Gyroscope sensitivity values, in LSB/deg/s. In order to maximize resolution
// we shift the value to take advantage of as many fractional bits as possible.
// We then un-shift after calculating the data values to be returned. These must
// correlate directly with the range setting.
const q16_t mpu6515_gyro_sensitivity[] =
{
    MPU6050_DPS_TO_RPSQ16(1.0 / 131.0) << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 65.5)  << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 32.8)  << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 16.4)  << 9
};

// Accelerometer sensitivity values, in G/LSB. These must correlate directly
// with the range setting.
q16_t mpu6515_accel_sensitivity[] =
{
    FX_FLTTOFIX_Q16(G / (8192 * 2)),
    FX_FLTTOFIX_Q16(G / (4096 * 2)),
    FX_FLTTOFIX_Q16(G / (2048 * 2)),
    FX_FLTTOFIX_Q16(G / (1024 * 2))
};

// Register configuration values that correlate to gyro_range[]
uint8_t mpu6515_gyro_range_reg[] =
{
    FS_SEL_250DPS,
    FS_SEL_500DPS,
    FS_SEL_1000DPS,
    FS_SEL_2000DPS
};

// Register configuration values that correlate to accel_range[]
uint8_t mpu6515_accel_range_reg[] =
{
    AFS_SEL_2G,
    AFS_SEL_4G,
    AFS_SEL_8G,
    AFS_SEL_16G
};

// Register configuration values that correlate to accel_lpf[]
static const uint8_t mpu6515_accel_lpf_reg[] =
{
    MPU6500_DLPF_CFG_GYRO_3600HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_5HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_10HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_20HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_41HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_92HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_184HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_460HZ
};

// Register configuration values that correlate to gyro_lpf[]
static const uint8_t mpu6515_gyro_lpf_reg[] =
{
    MPU6500_DLPF_CFG_GYRO_5HZ,
    MPU6500_DLPF_CFG_GYRO_10HZ,
    MPU6500_DLPF_CFG_GYRO_20HZ,
    MPU6500_DLPF_CFG_GYRO_41HZ,
    MPU6500_DLPF_CFG_GYRO_92HZ,
    MPU6500_DLPF_CFG_GYRO_184HZ,
    MPU6500_DLPF_CFG_GYRO_250HZ
};

// Temperature offset, in LSB/degC.
static const int16_t mpu6515_temp_offset_lsb = -521;

// Temperature at which temperature offset value occurs.
static const q16_t mpu6515_temp_offset_deg = FX_FLTTOFIX_Q16(35.0);

// Temperature sensor sensitivity (untrimmed), in LSB/degC.
const q16_t mpu6515_temp_sensitivity = FX_FLTTOFIX_Q16(1.0 / 340.0);


/*
 * DMP-specific definitions for MPU6515
 *
 * Based on app notes for DMP hardware functions (Rev. 0.6)
 */
#define NUM_ELEM 4
#define NUM_AXES 3
#define GYRO_A 0
#define GYRO_B 1
#define ACCL_A 2
#define ACCL_B 3

typedef struct {
  uint8_t val[NUM_ELEM][NUM_AXES];
} mount_matrix_t;

static mount_matrix_t mpu6515_dmp_mount_matrix[] = {
  /* [1 0 0; 0 1 0; 0 0 1] */
  {{{0x4C, 0xCD, 0x6C},
    {0x36, 0x56, 0x76},
    {0x0C, 0xC9, 0x2C},
    {0x26, 0x46, 0x66}} },

  /* 0 1 0; -1 0 0 ; 0 0 1 */
  {{{0xCD, 0x4C, 0x6C},
    {0x36, 0x57, 0x76},
    {0xC9, 0x0C, 0x2C},
    {0x26, 0x47, 0x66}} },

  /* -1 0 0 ; 0 -1 0 ; 0 0 1 */
  {{{0x4C, 0xCD, 0x6C},
    {0x37, 0x57, 0x76},
    {0x0C, 0xC9, 0x2C},
    {0x27, 0x47, 0x66}} },

  /* 0 -1 0 ; 1 0 0 ; 0 0 1 */
  {{{0xCD, 0x4C, 0x6C},
    {0x37, 0x56, 0x76},
    {0xC9, 0x0C, 0x2C},
    {0x27, 0x46, 0x66}} },

  /* -1 0 0 ; 0 1 0 ; 0 0 -1 */
  {{{0x4C, 0xCD, 0x6C},
    {0x37, 0x46, 0x77},
    {0x0C, 0xC9, 0x2C},
    {0x27, 0x46, 0x67}} },

  /* 0 -1 0; 1 0 0 ; 0 0 -1 */
  {{{0xCD, 0x4C, 0x6C},
    {0x37, 0x56, 0x77},
    {0xC9, 0x0C, 0x2C},
    {0x27, 0x46, 0x66}} },

  /* 1 0 0 ; 0 -1 0 ; 0 0 -1 */
  {{{0x4C, 0xCD, 0x6C},
    {0x36, 0x57, 0x77},
    {0x0C, 0xC9, 0x2C},
    {0x26, 0x47, 0x67}} },

  /* 0 1 0 1; 1 0 0 ; 0 0 -1 */
  {{{0xCD, 0x4C, 0x6C},
    {0x36, 0x56, 0x77},
    {0xC9, 0x0C, 0x2C},
    {0x26, 0x46, 0x67}} }
};

typedef struct {
  uint8_t num_pkts;
  uint16_t pkt_start_idx[DMP_MAX_NUM_SAMPLES];
} parsed_fifo_info_s;

typedef struct {
  uint8_t           dim;          // sensor dimension / # of axes
  uint8_t           hdr_size;     // header size [bytes]
  uint8_t           body_size;    // payload size [bytes]
  uint16_t          hdr;          // header identifier
  sns_ddf_sensor_e  ddf_sensor;   // DDF sensor
  uint16_t          odr_addr;     // address of ODR rate divider

  /* Related to enabling/disabling outputs in FIFO */
  uint8_t           enable[2];    // bytes for enabling sensor
  uint8_t           disable[2];   // bytes for disabling sensor
  uint16_t          cfg_addr;     // address of configuring sensor
  uint16_t          odr_cntr;     // odr counter to reset odr while running
} dmp_sensor_t;


static dmp_sensor_t dmp_sensor_map[] = {
/*           Dim HdrSz BodySz Hdr              DDF sensor                 ODR addr      Enable        Disable       CFG addr         ODR_CNTR      */
/*INVALID*/ {0,  0,    0,     DMP_HDR_INVALID, SNS_DDF_SENSOR__NONE,      0,            {0x00,0x00},  {0x00,0x00},  0,               0              },
/*3QUAT*/   {3,  2+2,  12,    0x0800,          SNS_DDF_SENSOR__NONE,      D_ODR_S1,     {0xa3,0xa3},  {0xf4,0x13},  CFG_OUT_3QUAT,   D_ODR_CNTR_S1  },
/*6QUAT*/   {3,  2+2,  12,    0x0400,          SNS_DDF_SENSOR_GAME_RV,    D_ODR_S2,     {0xa3,0xa3},  {0xf4,0x13},  CFG_OUT_6QUAT,   D_ODR_CNTR_S2  },
/*PQUAT*/   {3,  2+2,  12,    0x0200,          SNS_DDF_SENSOR__NONE,      D_ODR_S3,     {0xf3,0xf3},  {0xf4,0x03},  CFG_OUT_PQUAT,   D_ODR_CNTR_S3  },
/*RAW_ACCL*/{3,  2,    6,     0x4000,          SNS_DDF_SENSOR_ACCEL,      D_ODR_S4,     {0xa3,0xa3},  {0xf4,0x12},  CFG_OUT_ACCL,    D_ODR_CNTR_S4  },
/*RAW_GYRO*/{3,  2,    6,     0x2000,          SNS_DDF_SENSOR_GYRO,       D_ODR_S5,     {0xa3,0xa3},  {0xf4,0x11},  CFG_OUT_GYRO,    D_ODR_CNTR_S5  },
/*STEPDET*/ {1,  2+2,  4,     0x0100,          SNS_DDF_SENSOR_STEP_EVENT, 0,            {0xa3,0xa3},  {0xf4,0x0e},  CFG_OUT_STEPDET, 0              },
/*COMPASS*/ {3,  2,    6,     0x1000,          SNS_DDF_SENSOR_MAG,        D_ODR_S6,     {0xa3,0xa3},  {0xf4,0x12},  CFG_OUT_CPASS,   D_ODR_CNTR_S6  },
/*PRESSURE*/{1,  2,    6,     0x8000,          SNS_DDF_SENSOR_PRESSURE,   D_ODR_S7,     {0xa3,0xa3},  {0xf4,0x12},  CFG_OUT_PRESS,   D_ODR_CNTR_S7  },
/*SMD*/     {1,  0,    0,     DMP_HDR_INVALID, SNS_DDF_SENSOR_SMD,        0,            {0x01,0x00},  {0x00,0x00},  D_SMD_ENABLE,    0              },
/*DBLTAP  */{1,  0,    0,     DMP_HDR_INVALID, SNS_DDF_SENSOR__NONE,      0,            {0x00,0x00},  {0x00,0x00},  0,               0              },
/*STEPCNT*/ {1,  0,    0,     DMP_HDR_INVALID, SNS_DDF_SENSOR_STEP_COUNT, 0,            {0x00,0x00},  {0x00,0x00},  0,               0              }
};
uint8_t step_int_en[] = {0xF4,0x44,0xF1};
uint8_t step_int_dis[] = {0xf1,0xf1,0xf1};
static uint8_t dev_fifo[DMP_MAX_PKT_SIZE+DMP_FIFO_SIZE] = { 0xAA };
static uint16_t dev_fifo_len = 0;
static uint16_t dmp_sensor_bitmap = 0x0000;   // supports up to 15 DMP sensors
static const uint16_t accel_only_bitmap = (1<<DMP_SENSOR_RAW_ACCEL);
static bool is_dmp_enabled = false;
static uint16_t prev_dmp_sensor_bitmap = 0x0000;
static parsed_fifo_info_s parsed_fifo_info[ARR_SIZE(dmp_sensor_map)];
static uint8_t bytes_clr_odr_cntr[2] = {0,0};
static uint16_t dmp_bias_addr[MPU6XXX_MAX_NUM_AXES] = {
   D_EXT_GYRO_BIAS_X, D_EXT_GYRO_BIAS_Y, D_EXT_GYRO_BIAS_Z
};

#define DMP_NUM_SENSOR         ( sizeof(dmp_sensor_map) / sizeof(dmp_sensor_map[0]) )
#define DMP_GET_HDR(buf)       ( (((buf)[0] & 0x00FF) << 8 | (buf)[1]) & 0xFFFE )
#define DMP_STEP_DETECTED(buf) ( (((buf)[0] & 0x00FF) << 8 | (buf)[1]) & 0x0001 )
#define DMP_GET_DDF_SENSOR(s)  ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].ddf_sensor : dmp_sensor_map[DMP_SENSOR_INVALID].ddf_sensor )
#define DMP_GET_DIM(s)         ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].dim : dmp_sensor_map[DMP_SENSOR_INVALID].dim )
#define DMP_GET_ENABLE_SEQ(s)  ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].enable : dmp_sensor_map[DMP_SENSOR_INVALID].enable )
#define DMP_GET_DISABLE_SEQ(s) ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].disable : dmp_sensor_map[DMP_SENSOR_INVALID].disable )
#define DMP_GET_CFG_ADDR(s)       ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].cfg_addr : dmp_sensor_map[DMP_SENSOR_INVALID].cfg_addr )
#define DMP_GET_ODR_ADDR(s)       ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].odr_addr : dmp_sensor_map[DMP_SENSOR_INVALID].odr_addr )
#define DMP_GET_ODR_CNTR_ADDR(s)  ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].odr_cntr : dmp_sensor_map[DMP_SENSOR_INVALID].odr_cntr )
#define DMP_GET_HDR_SIZE(s)       ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].hdr_size : dmp_sensor_map[DMP_SENSOR_INVALID].hdr_size )
#define DMP_GET_BODY_SIZE(s)      ( (s)<DMP_NUM_SENSOR ? dmp_sensor_map[(s)].body_size : dmp_sensor_map[DMP_SENSOR_INVALID].body_size )
#define DMP_GET_PKT_SIZE(s)       ( DMP_GET_HDR_SIZE(s) + DMP_GET_BODY_SIZE(s) )

#define DMP_ENABLE_SENSOR(s)      ( dmp_sensor_bitmap |= ( ((s)<DMP_NUM_SENSOR && (s)!=DMP_SENSOR_INVALID) ?  (1<<(s)) : 0x0000 ) )
#define DMP_DISABLE_SENSOR(s)     ( dmp_sensor_bitmap &= ( ((s)<DMP_NUM_SENSOR && (s)!=DMP_SENSOR_INVALID) ? ~(1<<(s)) : 0xFFFF ) )
#define DMP_IS_SENSOR_ENABLED(s)  ( dmp_sensor_bitmap & (1<<(s)) )
#define DMP_ALL_SENSOR_OFF()      ( ((dmp_sensor_bitmap & 0xFFFE)==0 )? true : false )
#define DMP_IS_BITMAP_EQUAL(s)    ( (dmp_sensor_bitmap&0xFFFE) == (s) )
#define DMP_CLEAR_BITMAP()        ( dmp_sensor_bitmap = 0 )
#define DMP_GET_BITMAP()          ( dmp_sensor_bitmap )
#define DMP_SAVE_BITMAP(s)        ( (s) = dmp_sensor_bitmap )
#define DMP_RESTORE_BITMAP(s)     ( dmp_sensor_bitmap = (s) )

/*
 * Gyro bias conversion factor (provided by vendor, DMP version 0.6):
 *    biasRaw_dmp = (biasQ16_in_rad_per_sec) *
                        (180.0/DMP_PI) * (32768.0/2000.0) * 2859.54 / 2^16
 */
#define DMP_CONV_GYRO_BIAS(v)   ( (int32_t) (FX_FIXTOFLT_Q16(v) * 2684347.56975) )   // DPS = 2000

#define DMP_CONV_S16(b)         ( (int16_t) ( ((b)[0]&0x00FF)<<8 | ((b)[1]&0x00FF)) )
#define DMP_CONV_S32(b)         ( (int32_t) ( (((long)(b)[0])<<24) | (((long)(b)[1])<<16) | (((long)(b)[2])<<8) | ((long)(b)[3]) ) )
#define DMP_CONV_U16(b)         ( (((b)[1]&0x00FF)<<8) | ((b)[0]&0x00FF) )
#define DMP_CONV_COMPASS(b)     ( (int16_t) ( ((b)[1]&0x00FF)<<8 | ((b)[0]&0x00FF)) )
// 20bit conversion below (pressure)
#define DMP_CONV_PRESSURE(b)    ( (int32_t) ( (((long)(b)[0])<<12) | (((long)(b)[1])<<4) | (((long)(b)[2])>>4) ) )
#define DMP_MEMMOVE(to,from,size)         \
  do {                                    \
    int it;                               \
    for(it = 0; it < (size); it++) {      \
      (to)[it] = (from)[it];              \
    }                                     \
  } while(0)

#define DMP_IS_DEAD(s)          ( ((s)->mpu_reg[MPU_REG_USER_CTRL] & (DMP_EN_ENABLED|FIFO_EN_ENABLED)) != 0x00  && \
                                  ((s)->mpu_reg[MPU_REG_INT_ENABLE] & DMP_DATA_RDY_EN_ENABLE) != 0x00           && \
                                  !mpu6515_check_dmp_heartbeat_reg(s) )

typedef struct {
  uint32_t line_num;
  uint32_t ts;
  uint16_t recovery_called;
  uint16_t recovery_OK;
  uint8_t whoami;
  uint8_t byte[128];
} log_reg_t;

#define NUM_LOG_MPU  16    // must be power of 2 and < 256
static log_reg_t log_mpu_reg[NUM_LOG_MPU];
static uint8_t log_mpu_idx = 0;
static const uint8_t log_mpu_reg_list[] = {
  MPU_REG_GYRO_CONFIG,  MPU_REG_ACCEL_CONFIG, MPU_REG_ACCEL_CONFIG_2,
  MPU_REG_INT_STATUS,   MPU_REG_INT_ENABLE,   MPU_REG_CONFIG,
  MPU_REG_PWR_MGMT_1,   MPU_REG_PWR_MGMT_2,   MPU_REG_USER_CTRL
};

#define NUM_LOG_DMP  16    // must be power of 2 and < 256
static log_reg_t log_dmp_reg[NUM_LOG_DMP];
static uint8_t log_dmp_idx = 0;

#define RECOVERY_TIMER    32   // must be power of 2
//static uint32_t  recovery_timer = 0;
static bool      recovery_in_progress = false;
static uint16_t  recovery_called = 0;
static uint16_t  recovery_OK = 0;
static mpu6515_s recovery_state;
static sns_ddf_status_e mpu6515_run_device_recovery(mpu6515_s *state);

/*
 * dmp_convert_bias()
 *
 * @breif     Converts bias (Q16, rad/sec, LE) to bias (raw, DMP units, BE)
 * @param[i]  sensor_ptr sensor pointer
 * @param[o]  bias_dmp   converted bias value (DMP units, big endian)
 * @param[i]  bias       original bias value (rad/sec, little endian)
 *
 * @return    None
 */
void dmp_convert_bias(mpu6515_sensor_s* sensor_ptr, int32_t *bias_dmp, int32_t *bias)
{
  int32_t value[MPU6XXX_MAX_NUM_AXES], i, k;
  uint8_t *array;
  sns_ddf_axes_map_s  *axes_map;
  
  if(sensor_ptr == NULL)
  {
    MPU6050_MSG_0(ERROR, "Cannot find axis mapping");
    return;
  }

  axes_map = &(sensor_ptr->axes_map);

  // Converts bias values and reverts axis mapping done by sns_ddf_map_axes()
  // Note: "-1" because DMP subtracts bias.
  value[axes_map->indx_x] = -1 * DMP_CONV_GYRO_BIAS(bias[0]) * axes_map->sign_x;
  value[axes_map->indx_y] = -1 * DMP_CONV_GYRO_BIAS(bias[1]) * axes_map->sign_y;
  value[axes_map->indx_z] = -1 * DMP_CONV_GYRO_BIAS(bias[2]) * axes_map->sign_z;

  // Converts final values to big endian
  for(i = 0; i < MPU6XXX_MAX_NUM_AXES; i++)
  {
    k = (i==0) ? axes_map->indx_x :
        ((i==1) ? axes_map->indx_y : axes_map->indx_z) ;

    array = (uint8_t*) &(bias_dmp[k]);
    array[0] = (uint8_t) ((value[k]>>24) & 0xFF);
    array[1] = (uint8_t) ((value[k]>>16) & 0xFF);
    array[2] = (uint8_t) ((value[k]>>8 ) & 0xFF);
    array[3] = (uint8_t) (value[k] & 0xFF);
    MPU6050_MSG_3(MED, "converted bias i=%d   k=%d val=%d", i, k, value[k]);
  }
}

uint16_t dmp_get_pkt_size(uint8_t dmp_sensor )
{
  return DMP_GET_HDR_SIZE(dmp_sensor) + DMP_GET_BODY_SIZE(dmp_sensor);
}

int mpu6515_config_mpu_reg(mpu6515_s *state, uint16_t addr, uint8_t* buf, uint8_t len, bool set_bits)
{
  int i, num_byte_updated = 0;

  if(state != NULL)
  {
    for(i = 0; i < len && (addr+i) <= MPU_REG_MAP_SIZE; i++)
    {
      if(set_bits)
      {
        if( (state->mpu_reg[(addr+i)%MPU_REG_MAP_SIZE] & buf[i]) != buf[i] ) {
          state->mpu_reg[(addr+i)%MPU_REG_MAP_SIZE] |= buf[i];
          num_byte_updated++;
        }
      }
      else
      {
        if( (state->mpu_reg[(addr+i)%MPU_REG_MAP_SIZE] & buf[i]) != 0 ) {
          state->mpu_reg[(addr+i)%MPU_REG_MAP_SIZE] &= ~(buf[i]);
          num_byte_updated++;
        }
      }
    }
  }
  else
  {
    MPU6050_MSG_1(ERROR, "update_mpu_reg: null state ptr (addr=0x%02x)", addr);
  }

  return num_byte_updated;
}

int mpu6515_config_mpu_reg_byte(mpu6515_s *state, uint16_t addr, uint8_t buf, bool set_bits)
{
  return mpu6515_config_mpu_reg(state,addr, &buf, 1, set_bits);
}

sns_ddf_status_e mpu6515_write_mpu_reg(mpu6515_s *state, uint16_t addr, uint8_t *buf, uint8_t len)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t len_sent = 0;

  if(state != NULL)
  {
    status = sns_ddf_write_port(state->port_handle, addr, buf, len, &len_sent);
  }
  else
  {
    status = SNS_DDF_EFAIL;
  }

  return status;
}

sns_ddf_status_e mpu6515_update_mpu_reg(mpu6515_s *state, uint16_t addr, uint8_t* buf, uint8_t len, bool set_bits)
{
  if(mpu6515_config_mpu_reg(state, addr, buf, len, set_bits) > 0)
  {
    return mpu6515_write_mpu_reg(state, addr, buf, len);
  }

  return SNS_DDF_SUCCESS;
}

sns_ddf_status_e mpu6515_update_mpu_reg_byte(mpu6515_s *state, uint16_t addr, uint8_t buf, bool set_bits)
{
  return mpu6515_update_mpu_reg(state, addr, &buf, 1, set_bits);
}

#define DMP_DESIRED_THRES   64
/**
 * @param[in]   percent : desired threshold (in 'percent'/128)
 *
 * @return      threshold value for DMP
 */
uint16_t sns_dd_dmp_compute_batch_threshold(uint8_t percent, uint8_t pktsize)
{
  int p;
  p = (percent>128) ? 128 : percent;

  pktsize = MAX(1,pktsize);

  return (uint16_t) (DMP_FIFO_SIZE * p / 128 / pktsize);
}

/**
 * @param[in]   hdr : DMP packet header
 *
 * @return      DMP sensor (index)
 */
uint8_t sns_dd_dmp_parse_hdr(uint16_t hdr)
{
  uint8_t i;
  for(i = 0; i < DMP_NUM_SENSOR; i++) {
    if(dmp_sensor_map[i].hdr == hdr) {
      return i;
    }
  }

  return DMP_SENSOR_INVALID;
}

/**
 *
 *
 *
 */
uint8_t sns_dd_dmp_get_sensor(sns_ddf_sensor_e s)
{
  uint8_t i;
  for(i = 0; i < DMP_NUM_SENSOR; i++) {
    if(dmp_sensor_map[i].ddf_sensor == s) {
      return i;
    }
  }

  return DMP_SENSOR_INVALID;
}

/**
 *
 *
 *
 */
sns_ddf_status_e mpu6515_init_akm8963_slave(sns_ddf_handle_t hndl, 
                                            sns_ddf_nv_params_s*    nv_params,
                                            sns_ddf_device_access_s device_info[],
                                            sns_ddf_memhandler_s*   memhandler)
                                            
{
  uint8_t bytes_read, int_reg_conf = 0;
  sns_ddf_status_e        status = SNS_DDF_SUCCESS;
  mpu6515_sensor_s*         sensor = NULL;
  mpu6515_s*              state = mpu6515_get_device(hndl); //TODO: NEED to add check if hndl is not null
  mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(hndl);
  sns_ddf_sensor_e              sensor_type[2];
  sns_ddf_sensor_e * sensor_type_ptr;
  uint32_t num_sensors=1; // usused. TODO: figure out if this is needed.

  MPU6050_MSG_0(HIGH, "mpu6515_init_akm8963_slave: aux_dev Init called");
  if (NULL == sub_dev)
  {
    MPU6050_MSG_0(ERROR, "mpu6515_init_akm8963_slave: Received a NULL sub-device pointer.");
    return SNS_DDF_EINVALID_PARAM;
  }
  if(state == NULL)
    return SNS_DDF_EFAIL;

  sensor = mpu6515_get_sensor(hndl, SNS_DDF_SENSOR_MAG);
  sensor_type_ptr = sensor_type;
  if(sensor == NULL)
  {
    return SNS_DDF_EFAIL;
  }

  status = sns_ddf_read_port(state->port_handle, MPU6050_REG_INT_PIN_CFG, &int_reg_conf, 1, &bytes_read);
  if(status != SNS_DDF_SUCCESS || bytes_read != 1) {
    MPU6050_MSG_2(ERROR, "mpu6515_init_akm8963_slave: err=%d, port_hndl=0x%08x", status, state->port_handle);
    return status;
  }

  MPU6050_MSG_3(HIGH, "add_aux_dev: enters bypass (reg=0x%02x), bit=0x%02x, hndl=0x%08x", int_reg_conf, I2C_BYPASS_EN_BIT, state->port_handle);

  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf | I2C_BYPASS_EN_BIT, &status);
  if(status != SNS_DDF_SUCCESS)
    return status;

  sensor->aux_dev.valid = true;
  sensor->aux_dev.i2c_slave_addr = device_info->port_config.bus_config.i2c->slave_addr;  

  status = sns_mag_akm8963_driver_fn_list.init(&(sensor->aux_dev.st_aux_dev),sub_dev->smgr_handle,nv_params,device_info,1,memhandler,&sensor_type_ptr,&num_sensors);   
  MPU6050_MSG_2(HIGH, "mpu6515_init_akm_slave: aux_dev Init i2c addr: %x , I2c bus instance: %x", sensor->aux_dev.i2c_slave_addr,device_info->port_config.bus_instance);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_status_e init_status = status;
    sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;
    MPU6050_MSG_1(ERROR, "mpu6515_init_akm8963_slave: Cannot Initialize AKM8963 correctly , status=%d ", init_status );
    return init_status;
  }
  else
  {
    MPU6050_MSG_1(HIGH, "mpu6515_init_akm8963_slave: Initialized AKM8963 (aux_handle=0x%x)", sensor->aux_dev.st_aux_dev);
  } 

  /* Resets slave device */
  status = sns_mag_akm8963_driver_fn_list.reset(sensor->aux_dev.st_aux_dev);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_status_e init_status = status;
    sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;
    MPU6050_MSG_1(FATAL, "init_akm8963: akm8963.reset() failed (err=%d)", init_status);
    return init_status;
  }
  else
  {
    MPU6050_MSG_0(HIGH, "init_akm8963: akm8973.reset() OK");
  } 

  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
  if(status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_2(HIGH, "init_akm8963_slave: exits bypass: port_hndl=0x%08x ddf_sensor=%d", state->port_handle, sensor->sensor_type);

  return SNS_DDF_SUCCESS;
}


/**
 * Slave initialization function for BMP280.
 *
 * This function is called by init()
 * 
 */
sns_ddf_status_e mpu6515_init_bmp280_slave( sns_ddf_handle_t hndl,
                                            sns_ddf_nv_params_s*    nv_params,
                                            sns_ddf_device_access_s device_info[],
                                            sns_ddf_memhandler_s*   memhandler)
{
  uint8_t bytes_read, int_reg_conf = 0;
  sns_ddf_status_e        status = SNS_DDF_SUCCESS;
  mpu6515_sensor_s*         sensor = NULL;
  mpu6515_s*              state = mpu6515_get_device(hndl); //TODO: NEED to add check if hndl is not null
  mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(hndl);
  sns_ddf_sensor_e              sensor_type[2];
  sns_ddf_sensor_e * sensor_type_ptr;
  uint32_t num_sensors=1; // usused. TODO: figure out if this is needed.

  MPU6050_MSG_0(HIGH, "mpu6515_init_bmp280_slave: aux_dev Init called");
  if (NULL == sub_dev)
  {
    MPU6050_MSG_0(ERROR, "mpu6515_init_bmp280_slave: Received a NULL sub-device pointer.");
    return SNS_DDF_EINVALID_PARAM;
  }
  if(state == NULL)
    return SNS_DDF_EFAIL;

  sensor = mpu6515_get_sensor(hndl, SNS_DDF_SENSOR_PRESSURE);
  sensor_type_ptr = sensor_type;
  if(sensor == NULL)
  {
    return SNS_DDF_EFAIL;
  }

  status = sns_ddf_read_port(state->port_handle, MPU6050_REG_INT_PIN_CFG, &int_reg_conf, 1, &bytes_read);
  if(status != SNS_DDF_SUCCESS || bytes_read != 1) {
    MPU6050_MSG_2(ERROR, "BMP: err=%d, port_hndl=0x%08x", status, state->port_handle);
    return status;
  }

  MPU6050_MSG_3(HIGH, "add_aux_dev: enters bypass (reg=0x%02x), bit=0x%02x, hndl=0x%08x", int_reg_conf, I2C_BYPASS_EN_BIT, state->port_handle);
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf | I2C_BYPASS_EN_BIT, &status);
  if(status != SNS_DDF_SUCCESS)
    return status;

  sensor->aux_dev.valid = true;
  sensor->aux_dev.i2c_slave_addr = device_info->port_config.bus_config.i2c->slave_addr;
  MPU6050_MSG_2(HIGH, "mpu6515_init_bmp280_slave: aux_dev Init i2c addr: %x , I2c bus instance: %x", sensor->aux_dev.i2c_slave_addr,device_info->port_config.bus_instance);

  status = sns_alt_bmp280_driver_fn_list.init(&(sensor->aux_dev.st_aux_dev),sub_dev->smgr_handle,nv_params,device_info,1,memhandler,&sensor_type_ptr,&num_sensors);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_status_e init_status = status;
    sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;
    MPU6050_MSG_1(ERROR, "mpu6515_init_bmp280_slave: Cannot Initialize BMP280 correctly, status=%d ", init_status );
    return init_status;
  }
  else
  {
    MPU6050_MSG_1(HIGH, "mpu6515_init_bmp280_slave: Initialized BMP280 (aux_handle=0x%x)", sensor->aux_dev.st_aux_dev);
  }

  /* Resets slave device */
  status = sns_alt_bmp280_driver_fn_list.reset(sensor->aux_dev.st_aux_dev);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_status_e init_status = status;
    sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;
    MPU6050_MSG_1(FATAL, "init_bmp280: bmp280.reset() failed (err=%d)", init_status);
    return init_status;
  }
  else
  {
    MPU6050_MSG_0(HIGH, "init_bmp280: bmp280.reset() OK");
  }

  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
  if(status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_2(HIGH, "exits bypass: port_hndl=0x%08x ddf_sensor=%d", state->port_handle, sensor->sensor_type);
  return SNS_DDF_SUCCESS;
}


/**
 * @param[in]
 *
 * @return
 */
sns_ddf_status_e mpu6515_write_dmp_reg(
  mpu6515_s * state, uint16_t address, uint8_t *buf, uint8_t buf_len)
{
  sns_ddf_status_e status;
  uint8_t  dmp_address[2];
  uint8_t  bytes_read;

  dmp_address[0] = MPU6XXX_HIGH_BYTE(address);
  dmp_address[1] = MPU6XXX_LOW_BYTE(address);

  // 1) programs the DMP register address to MPU registers
  status = sns_ddf_write_port( state->port_handle, MPU6500_REG_DMP_CTRL_1,
                               dmp_address, 2, &bytes_read);
  if (status != SNS_DDF_SUCCESS || bytes_read != 2)
    return status;

  // 2) writes DMP register
  status = sns_ddf_write_port( state->port_handle, MPU6500_REG_DMP_CTRL_3,
                               buf, buf_len, &bytes_read);
  if (status != SNS_DDF_SUCCESS || bytes_read != buf_len)
    return status;

  if( address <= 32 ) {
    MPU6050_MSG_1(FATAL, "are you sure??? (addr=%u)", address);
  }

  return SNS_DDF_SUCCESS;
}

sns_ddf_status_e mpu6515_read_dmp_reg(
  mpu6515_s * state, uint16_t address, uint8_t *buf, uint8_t buf_len)
{
  sns_ddf_status_e status;
  uint8_t  dmp_address[2];
  uint8_t  bytes_read;

  dmp_address[0] = MPU6XXX_HIGH_BYTE(address);
  dmp_address[1] = MPU6XXX_LOW_BYTE(address);

  // 1) programs the DMP register address to MPU registers
  status = sns_ddf_write_port( state->port_handle, MPU6500_REG_DMP_CTRL_1,
                               dmp_address, 2, &bytes_read);
  if (status != SNS_DDF_SUCCESS || bytes_read != 2)
    return status;

  // 2) reads DMP register
  status = sns_ddf_read_port( state->port_handle, MPU6500_REG_DMP_CTRL_3,
                              buf, buf_len, &bytes_read);
  if (status != SNS_DDF_SUCCESS || bytes_read != buf_len)
    return status;

  return SNS_DDF_SUCCESS;
}
void mpu6515_print_mpu_reg(mpu6515_s * state)
{
  uint8_t bytes_read, i, mpu_addr;
  sns_ddf_status_e status;

  // Initializes the memory dump
  memset(log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].byte, 0xFF, 128);
  log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].whoami = DEVICE_ID_MPU6515;

  for(i = 0; i < ARR_SIZE(log_mpu_reg_list); i++)
  {
    mpu_addr = log_mpu_reg_list[i];
    status = sns_ddf_read_port(state->port_handle, mpu_addr,
                               &(log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].byte[mpu_addr]),
                               1, &bytes_read);
    if(status != SNS_DDF_SUCCESS)
    {
      // A crude way to identify a failure in memory dump
      log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].whoami = 0xFF;
      return;
    }
  }
}

void mpu6515_print_dmp_reg(mpu6515_s * state)
{
  uint8_t buf[4] = { 0 };
  sns_ddf_status_e status;
  uint8_t log_byte_cnt = 0;
  uint8_t *log_byte = log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].byte;

  status = mpu6515_read_dmp_reg(state, D_SMD_ENABLE, buf, 1);
//  MPU6050_MSG_3(HIGH, "debug: D_SMD_ENABLE: status=%d addr=%d val=%x", status, D_SMD_ENABLE, buf[0]);
  log_byte[log_byte_cnt++] = buf[0];

  status = mpu6515_read_dmp_reg(state, D_BM_ENABLE, buf, 1);
//  MPU6050_MSG_3(HIGH, "debug: D_BM_ENABLE: status=%d addr=%d val=%x", status, D_BM_ENABLE, buf[0]);
  log_byte[log_byte_cnt++] = buf[0];

  status = mpu6515_read_dmp_reg(state, D_BM_BATCH_THLD, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: D_BM_BATCH_THLD: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, CFG_PED_ENABLE, buf, 1);
//  MPU6050_MSG_2(HIGH,          "debug: CFG_PED_ENABLE: status=%d [0]=%x", status, buf[0]);
  log_byte[log_byte_cnt++] = buf[0];

  status = mpu6515_read_dmp_reg(state, CFG_OUT_6QUAT, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_OUT_6QUAT: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, CFG_OUT_GYRO, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_OUT_GYRO: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, CFG_OUT_ACCL, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_OUT_ACCL: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, CFG_OUT_CPASS, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_OUT_CPASS: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, CFG_OUT_PRESS, buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_OUT_PRESS: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];
 
  status = mpu6515_read_dmp_reg(state, D_ODR_S2 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_6QUAT_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_S5 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_GYRO_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_S4 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_ACCL_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_S6 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_CPASS_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];
  
  status = mpu6515_read_dmp_reg(state, D_ODR_S7 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_PRESS_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];


  status = mpu6515_read_dmp_reg(state, D_ODR_CNTR_S2 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_6QUAT_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_CNTR_S5 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_GYRO_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_CNTR_S4 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_ACCL_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];

  status = mpu6515_read_dmp_reg(state, D_ODR_CNTR_S6 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_CPASS_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];
  
  status = mpu6515_read_dmp_reg(state, D_ODR_CNTR_S7 , buf, 2);
//  MPU6050_MSG_3(HIGH,          "debug: CFG_PRESS_ODR: status=%d [0]=%x [1]=%x", status, buf[0], buf[1]);
  log_byte[log_byte_cnt++] = buf[0];
  log_byte[log_byte_cnt++] = buf[1];
//  status = sns_ddf_read_port(state->port_handle, MPU6515_REG_INT_ENABLE, buf, 1, &bytes_read);
//  MPU6050_MSG_3(HIGH,          "debug: INT_ENABLE reg: status=%d bytes_read=%d [0]=%x", status, bytes_read, buf[0]);

//  status = sns_ddf_read_port(state->port_handle, MPU6515_REG_USER_CTRL, buf, 1, &bytes_read);
//  MPU6050_MSG_3(HIGH,          "debug: USER_CTRL reg: status=%d bytes_read=%d [0]=%x", status, bytes_read, buf[0]);

  if(status != SNS_DDF_SUCCESS)
  {
    // no nothing. This is to eliminate compiler warning
  }

}

#define LOG_MPU_REG(state)      \
  do {                          \
    mpu6515_print_mpu_reg(state);           \
    log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].line_num = __LINE__;                            \
    log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].ts       = sns_em_get_timestamp();              \
    log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].recovery_called = recovery_called;              \
    log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].recovery_OK     = recovery_OK;                  \
    MPU6050_MSG_2(HIGH, "LOG_MPU_REG: buf_idx=%u, line_num=%u", log_mpu_idx, log_mpu_reg[log_mpu_idx%NUM_LOG_MPU].line_num);  \
    log_mpu_idx++;              \
  } while(0)                    \

#define LOG_DMP_REG(state)      \
  do {                          \
    mpu6515_print_dmp_reg(state);           \
    log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].line_num = __LINE__;                            \
    log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].ts       = sns_em_get_timestamp();              \
    log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].recovery_called = recovery_called;              \
    log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].recovery_OK = recovery_OK;                      \
    MPU6050_MSG_2(HIGH, "LOG_DMP_REG: buf_idx=%u, line_num=%u", log_dmp_idx, log_dmp_reg[log_dmp_idx%NUM_LOG_DMP].line_num);  \
    log_dmp_idx++;              \
  } while(0)                    \


/**
 * @brief Finds the device instance corresponding to the specified ddf_handle
 */
mpu6515_s* mpu6515_get_device(sns_ddf_handle_t dd_handle)
{
///  return (mpu6515_s*) ((uint32_t) dd_handle & (0xFFFFFFFF - MPU6XXX_STATE_BYTE_ALIGNMENT + 1));  // Zero-out the lower two bits of dd_handle
  return (mpu6515_s *) ( ((uint32_t) dd_handle) & 0xFFFFFFF0 );
}

/**
 * @brief Retrieves the specified sensor type in the sub-device
 */
mpu6515_sensor_s* mpu6515_get_sensor(
    sns_ddf_handle_t dd_handle, sns_ddf_sensor_e sensor)
{
  uint32_t j;

  mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(dd_handle);
  if (NULL == sub_dev)
  {
    MPU6050_MSG_0(ERROR, "get_sensor: Received a NULL sub-device pointer.");
    return NULL;
  }

  // Find the sensor within the sub-device
  for (j = 0; j < sub_dev->num_sensors; j++)
  {
    if (sub_dev->sensors[j].sensor_type == sensor)
      return &(sub_dev->sensors[j]);
  }

  // If the sensor is not found, return NULL
  return NULL;
}

/**
 * @brief Searches for the specified sensor type in the entire device instance
 *
 * It will return the first instance of the specified sensor type that it finds.
 *
 * This is mostly used to check concurrencies.
 *
 * @param[in]   state     Pointer to the device-instance where the search will take place.
 * @param[in]   sensor    The sensor type that is being searched for.
 * @param[out]  sub_dev   (Optional) Pointer to the sub-device that the sensor was found in.
 *
 * @return      A pointer to the desired sensor structure
 */
mpu6515_sensor_s* mpu6515_search_for_sensor(
    mpu6515_s* state, sns_ddf_sensor_e sensor, mpu6515_sub_dev_s** sub_dev_out)
{
  mpu6515_sub_dev_s* sub_dev;
  uint8_t i;
  uint32_t j;

  if (NULL == state)
    return NULL;

  // Find the sensor within the entire device instance
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    sub_dev = &state->sub_devices[i];
    for (j = 0; j < sub_dev->num_sensors; j++)
    {
      if (sub_dev->sensors[j].sensor_type == sensor)
      {
        if (sub_dev_out != NULL)
          *sub_dev_out = sub_dev;
        return &(sub_dev->sensors[j]);
      }
    }
  }

  // If the sensor is not found, return NULL
  if (sub_dev_out != NULL)
    *sub_dev_out = NULL;
  return NULL;
}

/**
 * @brief Retrieves the sub-device associated with the ddf_handle
 */
mpu6515_sub_dev_s* mpu6515_get_sub_dev(sns_ddf_handle_t dd_handle)
{
///  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & (MPU6XXX_STATE_BYTE_ALIGNMENT - 1));
  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & 0x0000000F);
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  /* Return with a null-pointer if the device instance is null or if the sub-
     device index is out-of-bounds */
  if (NULL == state)
    return NULL;

  if (sub_dev_index >= state->sub_device_cnt || sub_dev_index >= MPU6515_MAX_SUB_DEV)
  {
    return NULL;
  }
  return &(state->sub_devices[sub_dev_index]);
}

/**
 * @brief Retrieves the ddf_handle of paticular sensor
 *
 */
sns_ddf_handle_t mpu6515_get_sensor_handle(sns_ddf_handle_t dd_handle, sns_ddf_sensor_e sensor)
{
  uint8_t i=0, j=0;
  mpu6515_s* state = mpu6515_get_device(dd_handle);

  for( i=0; i<state->sub_device_cnt; i++ )
  {
    for(j=0; j<state->sub_devices[i].num_sensors; j++)
    {
      if( state->sub_devices[i].sensors[j].sensor_type == sensor )
      {
        MPU6050_MSG_2(MED, " mpu6515_get_sensor_handle:  Found match sensor=%d, handle=%d ", sensor, ((uint32_t) state | i));
        return (sns_ddf_handle_t) ((uint32_t) state | i);
      }
    }
  }

  return NULL;
}

/**
 * @brief Retrieves the sub-device index associated with the ddf_handle
 *
 * @return The sub-device index if a corresponding sub-device is found.
 *         Otherwise it returns 0xFF to signal that the sub-device was not found.
 */
uint8_t mpu6515_get_sub_dev_idx(sns_ddf_handle_t dd_handle)
{
///  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & (MPU6XXX_STATE_BYTE_ALIGNMENT - 1));
  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & 0x0000000F);
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  /* Return with a null-pointer if the device instance is null or if the sub-
     device index is out-of-bounds */
  if (NULL == state)
    return 0xFF;
  if (sub_dev_index >= state->sub_device_cnt)
  {
    MPU6050_MSG_2(ERROR, "get_sub_dev: sub-dev idx=%u >= sub-dev cnt=%u)", sub_dev_index, state->sub_device_cnt);
    return 0xFF;
  }
  return sub_dev_index;
}

/**
 * @brief       Determines if the DMP is active
 *
 * @param[in]   state     Pointer to the device-instance.
 *
 * @return      TRUE if the DMP is active, FALSE otherwise.
 */
bool mpu6515_is_dmp_active(mpu6515_s* state)
{
  return is_dmp_enabled;
}

/**
 * @brief Searches through all sensors (excluding DMP sensors) within the device
 *        instance to find the highest ODR.
 *
 * @param[in]   state     Pointer to the device-instance.
 *
 * @return      The maximum desired ODR.
 */
uint32_t mpu6515_find_max_desired_odr(mpu6515_s* state)
{
  uint8_t i;
  uint32_t j;
  uint32_t max_odr = 0;

  if (state == NULL)
    return 0;

  for (i = 0; i < state->sub_device_cnt; i++)
  {
    for (j = 0; j < state->sub_devices[i].num_sensors; j++)
    {
      // Do not take DOUBLETAP's desired ODR into account
      if( (max_odr < state->sub_devices[i].sensors[j].desired_odr) &&
          !MPU_IS_DMP_SENSOR(state->sub_devices[i].sensors[j].sensor_type) )
      {
        max_odr = state->sub_devices[i].sensors[j].desired_odr;
      }
    }
  }

  return max_odr;
}


/**
 * @brief Sets the power state of the device
 *
 * @param[in] state  Ptr to the driver structure
 * @param[in] mode   Ptr to the variable of sns_ddf_powerstate_e type
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e mpu6515_set_device_powerstate(
   mpu6515_s* state, sns_ddf_powerstate_e powerstate)
{
  uint8_t buffer = 0;
  uint8_t bytes_read;
  sns_ddf_status_e status;
  bool device_active;

  status = sns_ddf_read_port(state->port_handle,
      MPU6050_REG_PWR_MGMT_1, &buffer, 1, &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;

  MPU6050_MSG_2(HIGH, "set_dev_power: PWR_MGMT_1=0x%02x powerstate=%d",
                buffer, powerstate);

  // No need to perform an expensive I2C operation if we're already at the
  // right power state.
  device_active = ((buffer & (1 << SLEEP)) == SLEEP_DISABLE);
  if((powerstate == SNS_DDF_POWERSTATE_ACTIVE && device_active) ||
     (powerstate == SNS_DDF_POWERSTATE_LOWPOWER && !device_active))
      return SNS_DDF_SUCCESS;

  // Hold all Advanced-?Features under reset
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_USER_CTRL, 0x0C, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  is_dmp_enabled = false;

  // Toggles between ACTIVE and LOW-POWER (MPU's sleep state)
  buffer ^= (1 << SLEEP);
  status = sns_ddf_write_port(
      state->port_handle,
      MPU6050_REG_PWR_MGMT_1,
      &buffer,
      1,
      &bytes_read);

  MPU6050_MSG_2(HIGH, "set_dev_power: PWR_MGMT_1=0x%02x sleep_state=%d",
                buffer, device_active);

  return status;
}

/**
 *
 */
static bool mpu6515_sensors_active(mpu6515_s* state)
{
  // Check for null-pointers
  if (NULL == state)
    return FALSE;

  // Iterate through each sub-device to see if it is active or not
  uint8_t i;
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    if (state->sub_devices[i].powerstate == SNS_DDF_POWERSTATE_ACTIVE)
      return TRUE;
  }
  // No devices found active, return false
  return FALSE;
}

/**
 * Convert the device instance's axes map into an orientation matrix (mounting matrix)
 * that the DMP can understand.
 */
sns_ddf_status_e mpu6515_setup_orientation(mpu6515_s* state)
{
  sns_ddf_status_e   status = SNS_DDF_SUCCESS;
  int m_idx;
  ///m_idx = 4;   // Fluid (with debug board), screen facing up
  m_idx = 0;    // no mapping
  mount_matrix_t *m = &mpu6515_dmp_mount_matrix[m_idx];

  if ( (status = mpu6515_write_dmp_reg(state, FCFG_1, &(m->val[GYRO_A][0]), NUM_AXES)) != SNS_DDF_SUCCESS )
    return status;
  if ( (status = mpu6515_write_dmp_reg(state, FCFG_3, &(m->val[GYRO_B][0]), NUM_AXES)) != SNS_DDF_SUCCESS )
    return status;
  if ( (status = mpu6515_write_dmp_reg(state, FCFG_2, &(m->val[ACCL_A][0]), NUM_AXES)) != SNS_DDF_SUCCESS )
    return status;
  if ( (status = mpu6515_write_dmp_reg(state, FCFG_7, &(m->val[ACCL_B][0]), NUM_AXES)) != SNS_DDF_SUCCESS )
    return status;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs all of the required steps to setup the DMP
 *
 * !!Important!! You can only flash the DMP image ONCE before writing the DMP start address (FW_START)
 *
 * THIS FUNCTION SHOULD ONLY BE CALLED BY mpu6515_reset_device (hardware reset)!!
 */
sns_ddf_status_e mpu6515_setup_dmp(mpu6515_s* state)
{
  sns_ddf_status_e status;

  MPU6050_MSG_1(MEDIUM, "setup_dmp: flash firmware, dmp_bitmap=0x%04x", DMP_GET_BITMAP());
  if ((status = mpu6515_load_dmp_firmware(state, dmp_image.image, SNS_DDF_MAX_ESPIMAGE_SIZE /* dmp_image.size */)) != SNS_DDF_SUCCESS)
    return status;

  DMP_CLEAR_BITMAP();

  // Hold all Advanced-Features under reset
  MPU6050_MSG_0(HIGH, "setup_dmp: enter reset mode");
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_USER_CTRL, 0x0C, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  is_dmp_enabled = false;

  // Delay 10 milliseconds
  sns_ddf_delay(10000);


  /* MPU Setup for Enabling DMP (see Chapter 7 of MPU-6500 Register Map) */
  // Sensor (ADC) sampling rate should already be 200Hz at this point.
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_SMPRT_DIV, 0x04, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure Power Management Registers
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_PWR_MGMT_1, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_PWR_MGMT_2, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure Gyroscope Parameters
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_CONFIG, 0x03, &status);       // digital LPF cutoff = 42Hz
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_GYRO_CONFIG, 0x18, &status);  // FSR = 2000 dps
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure Accelerometer Parameters
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_ACCEL_CONFIG, state->mpu_reg[MPU_REG_ACCEL_CONFIG], &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_ACCEL_CONFIG_2, 0x40, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure FIFO and Interrupts
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_FIFO_EN, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6515_REG_INT_ENABLE, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  state->mpu_reg[MPU_REG_INT_ENABLE] = 0x00;

  // Reset FIFO
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_USER_CTRL, 0x0C, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;

  status = mpu6515_setup_orientation(state);
  if (status != SNS_DDF_SUCCESS)
  {
    MPU6050_MSG_0(ERROR, "Unsuccessful setup of orientation.");
  }

  return SNS_DDF_SUCCESS;
}

/**
 *
 */
sns_ddf_status_e mpu6515_set_powerstate(
   mpu6515_s*           state,
   uint8_t              sub_dev_idx,
   sns_ddf_powerstate_e powerstate)
{
  uint8_t buffer;
  uint8_t bytes_read;
  uint32_t i;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  mpu6515_sensor_s * sensor;

  MPU6050_MSG_2(LOW, "set_powerstate: idx %d  powerstate %d", sub_dev_idx, powerstate);

  // Check for null-pointers or invalid sub-device indices
  if (NULL == state || 0xFF == sub_dev_idx)
    return SNS_DDF_EINVALID_PARAM;

  mpu6515_sub_dev_s * sub_dev = &(state->sub_devices[sub_dev_idx]);
  if (NULL == sub_dev)
    return SNS_DDF_EINVALID_PARAM;

  // Check to see if we need to bring the device out of its slumber.
  if(!mpu6515_sensors_active(state) &&
     powerstate == SNS_DDF_POWERSTATE_ACTIVE)
  {
    status = mpu6515_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);

    if(status != SNS_DDF_SUCCESS)
       return status;
  }

  for (i = 0; i < sub_dev->num_sensors; i++)
  {
    sensor = &sub_dev->sensors[i];

    /* Change the value of the register as needed */
    switch(sensor->sensor_type)
    {
      case SNS_DDF_SENSOR_ACCEL:
      case SNS_DDF_SENSOR_GYRO:
      case SNS_DDF_SENSOR_TEMP:
      case SNS_DDF_SENSOR_MAG:
      case SNS_DDF_SENSOR_PRESSURE:
      case SNS_DDF_SENSOR_STEP_EVENT:
      case SNS_DDF_SENSOR_SMD:
      case SNS_DDF_SENSOR_STEP_COUNT:
      case SNS_DDF_SENSOR_GAME_RV:
      {
        uint8_t dmp_sensor;
        uint8_t *buf_ptr;
        MPU6050_MSG_0(MEDIUM, "set_powerstate: odr setting");
        /* Read the current USER_CTRL */
        status = sns_ddf_read_port(
          state->port_handle,
          MPU6500_REG_USER_CTRL,
          &buffer,
          1,
          &bytes_read);
        if(status != SNS_DDF_SUCCESS)
          return status;

        if(powerstate == SNS_DDF_POWERSTATE_ACTIVE)
        {
          buf_ptr = &(state->mpu_reg[MPU_REG_PWR_MGMT_1]);
          status = sns_ddf_read_port(state->port_handle, MPU_REG_PWR_MGMT_1,
                                     buf_ptr, 2, &bytes_read);
          if(status != SNS_DDF_SUCCESS)
            return status;

          buf_ptr[0] &= ~SLEEP_ENABLE;
          buf_ptr[1] &= ~(STBY_XA_ENABLE | STBY_YA_ENABLE | STBY_ZA_ENABLE);

          // Enables gyro if needed
          if(sensor->sensor_type == SNS_DDF_SENSOR_GYRO ||
             sensor->sensor_type == SNS_DDF_SENSOR_GAME_RV)
          {
            buf_ptr[1] &= ~(STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
          }

          status = mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1, buf_ptr, 2);
          if(status != SNS_DDF_SUCCESS)
            return status;
        }
        else
        {
          sensor->desired_odr = 0;
          sensor->int_enabled = false;
          dmp_sensor = sns_dd_dmp_get_sensor(sensor->sensor_type);
          sensor->num_axes = DMP_GET_DIM(dmp_sensor);

          status = mpu6515_set_odr(state, sensor, sensor->desired_odr);
          if(status != SNS_DDF_SUCCESS)
             return status;

          // Disables gyro if both raw gyro and game rv are not needed
          if(!DMP_IS_SENSOR_ENABLED(DMP_SENSOR_RAW_GYRO) &&
             !DMP_IS_SENSOR_ENABLED(DMP_SENSOR_6QUAT))
          {
            buf_ptr = &(state->mpu_reg[MPU_REG_PWR_MGMT_1]);
            status = sns_ddf_read_port(state->port_handle, MPU_REG_PWR_MGMT_1,
                                       buf_ptr, 2, &bytes_read);
            if(status != SNS_DDF_SUCCESS)
              return status;

            buf_ptr[0] &= ~SLEEP_ENABLE;
            buf_ptr[1] |= (STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
            mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1, buf_ptr, 2);
            if(status != SNS_DDF_SUCCESS)
              return status;
          }

          // Disables accel if no sensor is active
          if(DMP_ALL_SENSOR_OFF())
          {
            buf_ptr = &(state->mpu_reg[MPU_REG_PWR_MGMT_1]);
            status = sns_ddf_read_port(state->port_handle, MPU_REG_PWR_MGMT_1,
                                       buf_ptr, 2, &bytes_read);
            if(status != SNS_DDF_SUCCESS)
              return status;

            buf_ptr[1] |= (STBY_XA_ENABLE | STBY_YA_ENABLE | STBY_ZA_ENABLE);
            mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1, buf_ptr, 2);
            if(status != SNS_DDF_SUCCESS)
              return status;
          }
        }
        break;
      }

      default:
        break;
    }
  }

  /* Set the sub-device's power-state after dealing with all of its internal sensors */
  sub_dev->powerstate = powerstate;

  /* Check if we can bring the device down to save power */
  if(!mpu6515_sensors_active(state))
  {
      status = mpu6515_set_device_powerstate(state, SNS_DDF_POWERSTATE_LOWPOWER);
  }
  return status;
}

sns_ddf_status_e mpu6515_enable_interrupt(
  mpu6515_s*           state,
  mpu6515_sub_dev_s*   sub_dev,
  mpu6515_sensor_s*    sensor,
  mpu6515_interrupt_t  interrupt)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t int_enable_reg_value=0, int_enable_reg_rdbk_val=0;
  uint8_t bytes_read;
  sns_ddf_handle_t            sub_dev_handle;
  uint8_t iter = 0;

  if (NULL == state || NULL == sub_dev || NULL == sensor)
    return SNS_DDF_EINVALID_PARAM;

  // Configure interrupt pin
  sns_ddf_write_port_byte(
      state->port_handle,
      MPU6050_REG_INT_PIN_CFG,
      INT_LEVEL_HIGH | LATCH_INT_EN_PULSE | INT_RD_CLEAR_ON_STATUS_REG,
      &status);

  if(status != SNS_DDF_SUCCESS)
     return status;

  // Read in the current enabled interrupts
  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_INT_ENABLE,
      &int_enable_reg_value, 1,
      &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;

  state->mpu_reg[MPU_REG_INT_ENABLE] = int_enable_reg_value;

  // Configure interrupt specific parameters
  switch(interrupt)
  {
  case MPU6050_INT_MOTION_DETECT:
  {
    // Verify that the sensor is an ACCEL
    if (SNS_DDF_SENSOR_ACCEL != sensor->sensor_type)
    {
      MPU6050_MSG_1(ERROR, "enable_interrupt: Cannot enable motion-detect for sensor type %u (must be ACCEL!)", sensor->sensor_type);
      return SNS_DDF_EINVALID_PARAM;
    }

    // If the device already has the MD interrupt enabled, then no more work needs to be done.
    if ((int_enable_reg_value & MOT_EN_ENABLE) && state->md_enabled)
    {
      MPU6050_MSG_0(LOW, "enable_interrupt: Motion Detect interrupt already enabled");
      return SNS_DDF_SUCCESS;
    }

    // Disables cycle mode
    // Enable the interrupt(s)
    int_enable_reg_value |= MOT_EN_ENABLE;

    break;
  }

  case MPU6050_INT_DATA_READY:
  {
    MPU6050_MSG_0(HIGH, "enable_interrupt ENABLING DRI");
    sensor->int_enabled = true;

    // If the device already has DRI enabled, then no more work needs to be done.
    if (int_enable_reg_value & DATA_RDY_EN_ENABLE)
      return status;

    int_enable_reg_value |= DATA_RDY_EN_ENABLE;

    break;
  }

  case MPU6500_INT_DMP:
  {
    if (state->device_id == DEVICE_ID_MPU6500 || state->device_id == DEVICE_ID_MPU6515)
    {
      // No need to do any setup here, DMP setup is taken care of in setup_dmp.
      int_enable_reg_value |= DMP_DATA_RDY_EN_ENABLE;
    }
    else
    {
      return SNS_DDF_EINVALID_PARAM;
    }
    MPU6050_MSG_0(HIGH, "enable int mpu65xx: dmp");
    break;
  }

  default:
    return SNS_DDF_EINVALID_PARAM;
  }

  if(interrupt == MPU6500_INT_DMP || interrupt == MPU6050_INT_MOTION_DETECT)
  {
    for(iter = 0; iter < state->sub_device_cnt; iter++)
    {
      if (state->sub_devices[iter].sensors[0].sensor_type == sensor->sensor_type)
        break;
    }
  }
  sub_dev_handle = (sns_ddf_handle_t) ( (uint32_t)(state) | (0xF & iter) );

  if(!state->int_registered)
  {
    // Register to receive interrupt notifications
    status = sns_ddf_signal_register(
                sub_dev->int_gpio,
                (sns_ddf_handle_t)state,
                &sns_dd_mpu6515_if,
                SNS_DDF_SIGNAL_IRQ_RISING);
    if(status != SNS_DDF_SUCCESS)
      return status;
    state->int_registered = true;
    MPU6050_MSG_3(HIGH, "gpio enabled: subdev=%u, idx=%d, hndl=%u", sub_dev, iter, sub_dev_handle);
  }

  // Enable interrupt
  sns_ddf_write_port_byte(
      state->port_handle,
      MPU6050_REG_INT_ENABLE,
      int_enable_reg_value,
      &status);
  if(status != SNS_DDF_SUCCESS)
     return status;

  status = sns_ddf_read_port(
     state->port_handle, MPU6050_REG_INT_ENABLE, &int_enable_reg_rdbk_val, 1, &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;

  MPU6050_MSG_3(HIGH, "enable_int: after dmp_bitmap=0x%x reg_val_new=0x%x, reg_val_desired=0x%x",
                dmp_sensor_bitmap, int_enable_reg_rdbk_val, int_enable_reg_value);

  if(int_enable_reg_rdbk_val != int_enable_reg_value)
  {
    MPU6050_MSG_3(FATAL, "enable_int: old=0x%02x new=0x%02x, desired=0x%02x",
                  state->mpu_reg[MPU_REG_INT_ENABLE], int_enable_reg_rdbk_val, int_enable_reg_value);
  }
  else
  {
    // Updates local copy
    state->mpu_reg[MPU_REG_INT_ENABLE] = int_enable_reg_value;
  }


  if(interrupt == MPU6050_INT_MOTION_DETECT)
  {
    const uint8_t threshold_6500 = 8; // 1 lsb = 4 milli-g

    sns_ddf_write_port_byte(
        state->port_handle, MPU6500_REG_MOT_DETECT_CTR,
        WOM_ACCEL_INTEL_ENABLE_MPU6500_SAMPLE_MODE, &status);

    // Set threshold (Forcing internal state of MPU6500 to REST)
    sns_ddf_write_port_byte(
        state->port_handle, MPU6500_REG_MOT_THR, 255, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;

    // Intended delay : 1 msec (1000 usec)
    sns_ddf_delay(1000);

    // Set threshold (desired value)
    sns_ddf_write_port_byte(
        state->port_handle, MPU6050_REG_MOT_THR, threshold_6500, &status);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->md_enabled = TRUE;
  }

  status = sns_ddf_read_port(state->port_handle, MPU_REG_USER_CTRL,
                             &(state->mpu_reg[MPU_REG_USER_CTRL]), 3, &bytes_read);
  if(status == SNS_DDF_SUCCESS)
  {
    MPU6050_MSG_3(HIGH, "*** USER_CTRL=0x%02x  PWR1=0x%02x  PWR2=0x%02x ***",
                  state->mpu_reg[MPU_REG_USER_CTRL], state->mpu_reg[MPU_REG_PWR_MGMT_1], state->mpu_reg[MPU_REG_PWR_MGMT_2]);

    if( ((state->mpu_reg[MPU_REG_PWR_MGMT_1] & 0x07) != 0x00) ||
        DMP_IS_DEAD(state) )
    {
      status = mpu6515_run_device_recovery(state);
    }
  }

  return status;
}

sns_ddf_status_e mpu6515_disable_interrupt(
    mpu6515_s*           state,
    mpu6515_sub_dev_s*          sub_dev,
    mpu6515_sensor_s*           sensor,
    mpu6515_interrupt_t  interrupt)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t int_disable_reg_value=0, int_disable_reg_rdbk_val=0;
  uint8_t bytes_read;

  uint8_t i;
  uint32_t j;
  mpu6515_sub_dev_s* sub_dev_iter;
  mpu6515_sensor_s* sensor_iter;
  //sns_ddf_handle_t sub_dev_handle;

  if (NULL == state || NULL == sub_dev || NULL == sensor)
    return SNS_DDF_EINVALID_PARAM;

  /* read the current MPU6050_REG_INT_ENABLE */
  status = sns_ddf_read_port(
    state->port_handle,
    MPU6050_REG_INT_ENABLE,
    &int_disable_reg_value,
    1,
    &bytes_read);

  if(status != SNS_DDF_SUCCESS)
     return status;

  state->mpu_reg[MPU_REG_INT_ENABLE] = int_disable_reg_value;

  switch(interrupt)
  {
    case MPU6050_INT_DATA_READY:
      // Disable the DRI flag for this particular sensor
      sensor->int_enabled = false;

      /* If other sensors (except DOUBLETAP) are in DRI-mode, then keep DRI
         enabled for the entire device */
      for (i = 0; i < state->sub_device_cnt; i++)
      {
        sub_dev_iter = &state->sub_devices[i];
        for (j = 0; j < sub_dev_iter->num_sensors; j++)
        {
          sensor_iter = &sub_dev_iter->sensors[j];
          /* If the sensor is in DRI-mode and is not a DOUBLETAP sensor, then
           * exit out of the function. */
          if ((sensor_iter->int_enabled) && (!MPU_IS_DMP_SENSOR(sensor_iter->sensor_type)) ) {
            return status;
          }
        }
      }
      int_disable_reg_value &= ~DATA_RDY_EN_ENABLE;
      break;

    case MPU6050_INT_MOTION_DETECT:
      int_disable_reg_value &= ~MOT_EN_ENABLE;
      state->md_enabled = false;
      break;

    case MPU6500_INT_DMP:
      // No need to worry about disabling other portions of the DMP,
      // that is taken care of in mpu6515_set_powerstate
      int_disable_reg_value &= ~DMP_DATA_RDY_EN_ENABLE;
      MPU6050_MSG_0(HIGH, "disable_interrupt: Disabling DMP interrupt mpu65xx");
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  for(i = 0; i < state->sub_device_cnt; i++)
  {
    if (state->sub_devices[i].sensors[0].sensor_type == sensor->sensor_type)
      break;
  }
  //sub_dev_handle = (sns_ddf_handle_t) ( (uint32_t)(state) | (0xF & i) );

  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_ENABLE, int_disable_reg_value, &status);
    if(status != SNS_DDF_SUCCESS)
       return status;

  status = sns_ddf_read_port(
       state->port_handle, MPU6050_REG_INT_ENABLE, &int_disable_reg_rdbk_val, 1, &bytes_read);
    if(status != SNS_DDF_SUCCESS)
       return status;
 
  MPU6050_MSG_3(HIGH, "disable_int: after dmp_bitmap=0x%x reg_val_new=0x%x, reg_val_desired=0x%x", dmp_sensor_bitmap, int_disable_reg_rdbk_val, int_disable_reg_value);

  if(int_disable_reg_rdbk_val != int_disable_reg_value)
  {
    MPU6050_MSG_3(FATAL, "disable_int: old=0x%02x new=0x%02x, desired=0x%02x",
                  state->mpu_reg[MPU_REG_INT_ENABLE], int_disable_reg_rdbk_val, int_disable_reg_value);
  }
  else
  {
    // Updates local copy
    state->mpu_reg[MPU_REG_INT_ENABLE] = int_disable_reg_value;
  }

  // Interrupt pin is not needed. De-register pin.
  if(state->int_registered && !state->md_enabled && int_disable_reg_value==0)
  {
    status = sns_ddf_signal_deregister(sub_dev->int_gpio);
    if(status != SNS_DDF_SUCCESS)
      return status;
    state->int_registered = false;
    MPU6050_MSG_0(HIGH, "gpio disabled");
  }


  status = sns_ddf_read_port(state->port_handle, MPU_REG_USER_CTRL,
                             &(state->mpu_reg[MPU_REG_USER_CTRL]), 3, &bytes_read);
  if(status == SNS_DDF_SUCCESS)
  {
    MPU6050_MSG_3(HIGH, "*** USER_CTRL=0x%02x  PWR1=0x%02x  PWR2=0x%02x ***",
                  state->mpu_reg[MPU_REG_USER_CTRL], state->mpu_reg[MPU_REG_PWR_MGMT_1], state->mpu_reg[MPU_REG_PWR_MGMT_2]);

    if( ((state->mpu_reg[MPU_REG_PWR_MGMT_1] & 0x07) != 0x00) ||
        DMP_IS_DEAD(state)
//      || (++recovery_timer%RECOVERY_TIMER) == (RECOVERY_TIMER-1)
      )
    {
      status = mpu6515_run_device_recovery(state);
    }
  }
  
  return status;
}


// Note: This function is called by the SMGR and helps it route IRQ flags.
static sns_ddf_status_e mpu6515_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor_type,
    bool              enable)
{
  mpu6515_s* state = mpu6515_get_device(handle);
  mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(handle);
  mpu6515_sensor_s* sensor;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i;
  uint8_t dmp_sensor;

  MPU6050_MSG_3(HIGH, "enable_sched_data for sensor %d  dd_handle: 0x%x, true? %d", sensor_type, handle, enable);

  if (NULL == state || NULL == sub_dev)
    return SNS_DDF_EINVALID_PARAM;

  // Do nothing if the sensor type is NONE
  if(SNS_DDF_SENSOR__NONE == sensor_type)
  {
    MPU6050_MSG_0(HIGH, "enable_sched_data nothing to do for sensor type NONE");
    return SNS_DDF_SUCCESS;
  }
  // The function will call itself specifying which sensors to use
  if(SNS_DDF_SENSOR__ALL == sensor_type)
  {
    for (i = 0; i < sub_dev->num_sensors; i++)
    {
      status = mpu6515_enable_sched_data(handle, sub_dev->sensors[i].sensor_type, enable);
      if(status != SNS_DDF_SUCCESS)
         return status;
    }
    return status;
  }

  sensor = mpu6515_get_sensor(handle, sensor_type);
  /* If the sensor type is not found in the sub-device associated with the
     handle, then report an error */
  if (NULL == sensor)
  {
      return SNS_DDF_EINVALID_PARAM;
  }
  dmp_sensor = sns_dd_dmp_get_sensor(sensor->sensor_type);
  if(dmp_sensor == DMP_SENSOR_INVALID)
  {
    // Do nothing. This is to eliminate compiler warning
  }

  if(enable)
  {
    if(state->device_id == DEVICE_ID_MPU6515)
    {
      // Enable the DMP interrupt
      status = mpu6515_enable_interrupt(state, sub_dev, sensor, MPU6500_INT_DMP);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }
    else
    {
      /* Don't enable DRI interrupt if it is in motion detection mode */
      if ( (SNS_DDF_SENSOR_ACCEL != sensor_type) || (FALSE == state->md_enabled) )
      {
        status = mpu6515_enable_interrupt(state, sub_dev, sensor, MPU6050_INT_DATA_READY);
        if(status != SNS_DDF_SUCCESS)
          return status;
      }
      MPU6050_MSG_0(HIGH, "enable_sched_data: MD and DRI are NOT concurrent");
    }
  }
  else
  {
    if(state->device_id == DEVICE_ID_MPU6515)
    {
      MPU6050_MSG_1(HIGH, "enable_sched_data: bitmap=%04x", dmp_sensor_bitmap);
      if(DMP_ALL_SENSOR_OFF())
      {
        status = mpu6515_disable_interrupt(state, sub_dev, sensor, MPU6500_INT_DMP);
        if (status != SNS_DDF_SUCCESS)
          return status;
        MPU6050_MSG_0(HIGH, "enable_sched_data: DMP_DRI disabled");
      }
    }
    else
    {
      status = mpu6515_disable_interrupt(state, sub_dev, sensor, MPU6050_INT_DATA_READY);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }
  }

  return status;
}


/**
 * @brief Configure the low pass filter bandwidth
 *
 * NB: The accel and gyro LPF are configured together and therefore modifying
 * one always modifies the other as well. See MPU-6000/MPU-6050 Register Map and
 * Descriptions, section 4.3 for details.
 *
 * @param[in] state      Ptr to the driver structure
 * @param[in] lpf_index  Index to low pass filter bandwidth value
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e mpu6515_set_lowpass(
   mpu6515_s* state, uint8_t lpf_index)
{
    sns_ddf_status_e status;
    uint8_t bytes_written;
    uint8_t buffer;

    // Since accel and gyro LPFs are linked, it doesn't matter if we use the
    // gyro table or accel table. Suppress compiler warning re: an unused
    // variable. We keep this definition around for reference.
    (void)mpu6515_gyro_lpf_reg;

    // Do not set the lowpass while DOUBLETAP is running
    if (mpu6515_is_dmp_active(state))
    {
      MPU6050_MSG_1(ERROR, "set_lowpass: Cannot set the lowpass for the device instance 0x%X while the DMP is active", state);
      return SNS_DDF_SUCCESS; //return SNS_DDF_EINVALID_PARAM;
    }

    if(lpf_index >= MPU6050_TABLE_SIZE(mpu6515_accel_lpf_reg))
       return SNS_DDF_EINVALID_PARAM;

    buffer = mpu6515_accel_lpf_reg[lpf_index];

    status = sns_ddf_write_port(
        state->port_handle, MPU6050_REG_CONFIG, &buffer, 1, &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;


    state->lpf_index = lpf_index;

          return status;
}

sns_ddf_status_e mpu6515_enable_i2c_slv(mpu6515_s *state)
{
  sns_ddf_status_e    status = SNS_DDF_SUCCESS;
  mpu6515_sub_dev_s*  sub_dev_aux;
  mpu6515_sensor_s*   sensor_aux0 = NULL;
  mpu6515_sensor_s*   sensor_aux1 = NULL;
  uint8_t *i2c_slv_conf_ptr;

  if(DMP_IS_SENSOR_ENABLED(DMP_SENSOR_COMPASS) || DMP_IS_SENSOR_ENABLED(DMP_SENSOR_PRESSURE))
  {
    /* Configures slave devices */
    sensor_aux0 = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_MAG, &sub_dev_aux);
    sensor_aux1 = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_PRESSURE, &sub_dev_aux);
    if(sensor_aux0 == NULL || sensor_aux1 == NULL)
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    i2c_slv_conf_ptr = &(state->mpu_reg[MPU_REG_I2C_MST_CTRL]);
    if( DMP_IS_SENSOR_ENABLED(DMP_SENSOR_COMPASS) )
    {
      i2c_slv_conf_ptr[0] = MPU6515_BIT_WAIT_FOR_ES;   // Delayed sample, 400khz
      i2c_slv_conf_ptr++;

      MPU6050_MSG_3(HIGH, "i2c_slv (mag addr=0x%02x): port_hndl=0x%08x ddf_sensor=%d", sensor_aux0->aux_dev.i2c_slave_addr, state->port_handle, sensor_aux0->sensor_type);
      i2c_slv_conf_ptr[0] = MPU6515_BIT_I2C_SLV_READ | sensor_aux0->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = AKM_MAG_REG_ADDR_INFO_8963;
      i2c_slv_conf_ptr[2] = MPU6515_BIT_I2C_SLV_EN | 0x08;
      i2c_slv_conf_ptr += 3;

      i2c_slv_conf_ptr[0] = sensor_aux0->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = AKM_MAG_REG_ADDR_CNTL1_8963;
      i2c_slv_conf_ptr[2] = MPU6515_BIT_I2C_SLV_EN | 0x01;
      i2c_slv_conf_ptr += 3;

      state->mpu_reg[MPU_REG_I2C_SLV1_DO] = AKM_MAGNETOMETER_SINGLE_MEAS_MODE_8963;
      sns_ddf_write_port_byte(state->port_handle, MPU_REG_I2C_SLV1_DO, state->mpu_reg[MPU_REG_I2C_SLV1_DO], &status);
      if(status != SNS_DDF_SUCCESS)  return status;
    }
    else 
    {
      i2c_slv_conf_ptr[0] = MPU6515_BIT_WAIT_FOR_ES;   // Delayed sample, 400khz
      i2c_slv_conf_ptr++;

      MPU6050_MSG_3(HIGH, "i2c_slv (mag addr=0x%02x): port_hndl=0x%08x ddf_sensor=%d", sensor_aux1->aux_dev.i2c_slave_addr, state->port_handle, sensor_aux1->sensor_type);
      i2c_slv_conf_ptr[0] = MPU6515_BIT_I2C_SLV_READ | sensor_aux1->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = BMP280_DIG_T1_LSB_REG;
      i2c_slv_conf_ptr[2] = MPU6515_BIT_I2C_SLV_EN | 0x08;
      i2c_slv_conf_ptr += 3;

      i2c_slv_conf_ptr[0] = 0;//sensor_aux0->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = 0;// AKM_MAG_REG_ADDR_CNTL1_8963;
      i2c_slv_conf_ptr[2] = 0;//MPU6515_BIT_I2C_SLV_EN | 0x01;
      i2c_slv_conf_ptr += 3;

    }
    if(DMP_IS_SENSOR_ENABLED(DMP_SENSOR_PRESSURE))
    {
      MPU6050_MSG_3(HIGH, "i2c_slv (pressure addr=0x%02x): port_hndl=0x%08x ddf_sensor=%d", sensor_aux1->aux_dev.i2c_slave_addr, state->port_handle, sensor_aux1->sensor_type);
      i2c_slv_conf_ptr[0] = MPU6515_BIT_I2C_SLV_READ | sensor_aux1->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = BMP280_CONFIG_REG;                // BMP280_PRESSURE_MSB_REG - 2;
      i2c_slv_conf_ptr[2] = MPU6515_BIT_I2C_SLV_EN | 0x08;    // bytes
      i2c_slv_conf_ptr += 3;

      i2c_slv_conf_ptr[0] = sensor_aux1->aux_dev.i2c_slave_addr;
      i2c_slv_conf_ptr[1] = BMP280_CTRLMEAS_REG;
      i2c_slv_conf_ptr[2] = MPU6515_BIT_I2C_SLV_EN | 0x01;

      state->mpu_reg[MPU_REG_I2C_SLV3_DO] = BMP280_CTRLMEAS_REG_OSRST__MSK | BMP280_CTRLMEAS_REG_OSRSP__MSK | BMP280_CTRLMEAS_REG_MODE__MSK;
      sns_ddf_write_port_byte(state->port_handle, MPU_REG_I2C_SLV3_DO, state->mpu_reg[MPU_REG_I2C_SLV3_DO], &status);
      if(status != SNS_DDF_SUCCESS)  return status;
    }
    else
    {
      i2c_slv_conf_ptr[0] = 0; 
      i2c_slv_conf_ptr[1] = 0;  
      i2c_slv_conf_ptr[2] = 0; 
      i2c_slv_conf_ptr += 3;

      i2c_slv_conf_ptr[0] = 0;
      i2c_slv_conf_ptr[1] = 0;
      i2c_slv_conf_ptr[2] = 0;

    }
    status = mpu6515_write_mpu_reg(state, MPU_REG_I2C_MST_CTRL, &(state->mpu_reg[MPU_REG_I2C_MST_CTRL]), 13);
    if(status != SNS_DDF_SUCCESS)  return status;

    state->mpu_reg[MPU_REG_I2C_MST_DELAY_CTRL] = MPU6515_BIT_I2C_SLV0_DLY_EN | MPU6515_BIT_I2C_SLV1_DLY_EN |
                                                 MPU6515_BIT_I2C_SLV2_DLY_EN | MPU6515_BIT_I2C_SLV3_DLY_EN;
    sns_ddf_write_port_byte(state->port_handle, MPU_REG_I2C_MST_DELAY_CTRL, state->mpu_reg[MPU_REG_I2C_MST_DELAY_CTRL], &status);
    if(status != SNS_DDF_SUCCESS)  return status;

    /* Configures I2C slave sample delay */
    state->mpu_reg[MPU_REG_I2C_SLV4_CTRL] = 0x05;
    sns_ddf_write_port_byte(state->port_handle, MPU_REG_I2C_SLV4_CTRL, state->mpu_reg[MPU_REG_I2C_SLV4_CTRL], &status);
    if(status != SNS_DDF_SUCCESS)  return status;
  }

  return status;
}

sns_ddf_status_e mpu6515_set_dmp_odr(
    mpu6515_s* state,
    mpu6515_sensor_s* sensor, uint32_t odr)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint16_t n;
  uint8_t n_byte_array[2];
  uint8_t dmp_sensor;
  uint32_t target_odr;

  dmp_sensor = sns_dd_dmp_get_sensor(sensor->sensor_type);
  
  MPU6050_MSG_3(LOW, "set_dmp_odr: ddf_sensor=%u dmp_sensor=%u odr=%u", sensor->sensor_type, dmp_sensor, odr);

  if(odr == 0 && DMP_GET_DDF_SENSOR(dmp_sensor) != SNS_DDF_SENSOR__NONE)
  {
    sensor->desired_odr = odr;
    target_odr = odr;   // == 0
    //if(sensor->odr != target_odr)      // no need to set to zero again
    {
      MPU6050_MSG_2(MED, "set_dmp_odr: odr %u --> %u", sensor->odr, 0);
      n_byte_array[0] = 0;
      n_byte_array[1] = 0;
      sensor->odr = 0;
      switch(dmp_sensor)
      {
        case DMP_SENSOR_SMD:
          status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_SMD), DMP_GET_DISABLE_SEQ(DMP_SENSOR_SMD), 1);
          if(status != SNS_DDF_SUCCESS)  return status;
          break;
        case DMP_SENSOR_STEPDET:
          if( ! DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPCNT) ) {
            n_byte_array[0] = 0xff;
            status = mpu6515_write_dmp_reg(state, CFG_PED_ENABLE, n_byte_array, 1);
            if (status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_STEPDET), DMP_GET_DISABLE_SEQ(DMP_SENSOR_STEPDET), 2);
            if(status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, CFG_PED_INT, step_int_dis, 3);
            if (status != SNS_DDF_SUCCESS)  return status;
          } else {
            MPU6050_MSG_0(ERROR, "stepcnt active. skipping here...");
          }
          break;
        case DMP_SENSOR_STEPCNT:
          if( ! DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPDET) ) {
            n_byte_array[0] = 0xff;   // disable
            status = mpu6515_write_dmp_reg(state, CFG_PED_ENABLE, n_byte_array, 1);
            if(status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_STEPDET), DMP_GET_DISABLE_SEQ(DMP_SENSOR_STEPDET), 2);
            if(status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, CFG_PED_INT, step_int_dis, 3);
            if (status != SNS_DDF_SUCCESS)  return status;
          } else {
            MPU6050_MSG_0(ERROR, "stepdetect active. skipping here...");
          }
          break;
        case DMP_SENSOR_RAW_ACCEL:
        case DMP_SENSOR_RAW_GYRO:
        case DMP_SENSOR_6QUAT:
        case DMP_SENSOR_COMPASS:
        case DMP_SENSOR_PRESSURE:
          status = mpu6515_write_dmp_reg(state, DMP_GET_ODR_CNTR_ADDR(dmp_sensor), bytes_clr_odr_cntr, sizeof(bytes_clr_odr_cntr));
          if(status != SNS_DDF_SUCCESS)  return status;
          status = mpu6515_write_dmp_reg(state, DMP_GET_ODR_ADDR(dmp_sensor), n_byte_array, 2);
          if(status != SNS_DDF_SUCCESS)  return status;
          status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(dmp_sensor), DMP_GET_DISABLE_SEQ(dmp_sensor), 2);
          if(status != SNS_DDF_SUCCESS)  return status;
          break;
        default:
          MPU6050_MSG_0(FATAL, "Should never be here...");
          return SNS_DDF_EINVALID_PARAM;
      }
    }
    DMP_DISABLE_SENSOR(dmp_sensor);
    sensor->last_sample_timestamp = 0;
  }
  else if(odr > 0 && DMP_GET_DDF_SENSOR(dmp_sensor) != SNS_DDF_SENSOR__NONE)
  {
    /*
     * ODR of DMP sensor = 200 / (odr + 1)
     * n = floor(200 / odr) - 1
     *
     * This gives true ODR equivalent or higher to the desired odr.
     */
    sensor->desired_odr = odr;
    n = (MPU6500_DOUBLETAP_SMPLRT / odr) - 1;
    n_byte_array[0] = MPU6XXX_HIGH_BYTE(n);
    n_byte_array[1] = MPU6XXX_LOW_BYTE(n);
    target_odr = MPU6500_DOUBLETAP_SMPLRT / (n + 1);

    //if(sensor->odr != target_odr)
    {
      MPU6050_MSG_2(MED, "set_dmp_odr: odr %u --> %u", sensor->odr, target_odr);
      sensor->odr = target_odr;
      switch(dmp_sensor)
      {
        case DMP_SENSOR_SMD:
          status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_SMD), DMP_GET_ENABLE_SEQ(DMP_SENSOR_SMD), 1);
          if(status != SNS_DDF_SUCCESS)  return status;
          break;
        case DMP_SENSOR_STEPDET:
          if( ! DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPCNT) ) {
            n_byte_array[0] = 0xf1;
            status = mpu6515_write_dmp_reg(state, CFG_PED_ENABLE, n_byte_array, 1);
            if (status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_STEPDET), DMP_GET_ENABLE_SEQ(DMP_SENSOR_STEPDET), 2);
            if(status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, CFG_PED_INT, step_int_en, 3);
            if (status != SNS_DDF_SUCCESS)  return status;
          } else {
            MPU6050_MSG_0(ERROR, "step enabled by stepcnt already...");
          }
          break;
        case DMP_SENSOR_STEPCNT:
          if( ! DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPDET) ) {
            n_byte_array[0] = 0xf1;
            status = mpu6515_write_dmp_reg(state, CFG_PED_ENABLE, n_byte_array, 1);
            if (status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(DMP_SENSOR_STEPDET), DMP_GET_ENABLE_SEQ(DMP_SENSOR_STEPDET), 2);
            if(status != SNS_DDF_SUCCESS)  return status;
            status = mpu6515_write_dmp_reg(state, CFG_PED_INT, step_int_en, 3);
            if (status != SNS_DDF_SUCCESS)  return status;
          } else {
            MPU6050_MSG_0(ERROR, "step enabled by stepdetect already...");
          }
          break;
        case DMP_SENSOR_RAW_ACCEL:
        case DMP_SENSOR_RAW_GYRO:
        case DMP_SENSOR_6QUAT:
        case DMP_SENSOR_COMPASS:
        case DMP_SENSOR_PRESSURE:
          status = mpu6515_write_dmp_reg(state, DMP_GET_ODR_CNTR_ADDR(dmp_sensor), bytes_clr_odr_cntr, sizeof(bytes_clr_odr_cntr));
          if(status != SNS_DDF_SUCCESS)  return status;
          status = mpu6515_write_dmp_reg(state, DMP_GET_ODR_ADDR(dmp_sensor), n_byte_array, 2);
          if(status != SNS_DDF_SUCCESS)  return status;
          status = mpu6515_write_dmp_reg(state, DMP_GET_CFG_ADDR(dmp_sensor), DMP_GET_ENABLE_SEQ(dmp_sensor), 2);
          if(status != SNS_DDF_SUCCESS)  return status;
		  break;
        default:
          MPU6050_MSG_0(FATAL, "Should never be here...");
          return SNS_DDF_EINVALID_PARAM;
      }
    }
    DMP_ENABLE_SENSOR(dmp_sensor);
  }

  return status;
}

sns_ddf_status_e mpu6515_set_odr(
    mpu6515_s* state,
    mpu6515_sensor_s* sensor, uint32_t odr)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  MPU6050_MSG_2(LOW, "set_odr desired_odr=%u sensor=%u", odr, sensor->sensor_type);

  // Max ODR, as physically limited by the device, is 200 Hz
  sensor->desired_odr = MIN(odr,MPU6500_DOUBLETAP_MAX_ODR);

  // Set the ODR of DMP sensor
  status = mpu6515_set_dmp_odr(state, sensor, odr);
  if (status != SNS_DDF_SUCCESS) {
    MPU6050_MSG_1(ERROR, "set_dmp_odr: error=%u", status);
    return status;
  }

  MPU6050_MSG_1(HIGH, "dmp_sensor bitmap=%04x", dmp_sensor_bitmap);

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Configure the given sensor.
 *
 * @param[in] state           Ptr to the device instance
 * @param[in] sensor          Sensor to configure
 * @param[in] range_index     Index in the range table for the sensor
 * @param[in] accel_hpf       Accel high-pass filter; ignored for Gyro
 * @param[in] enable_selftest TRUE to enable HW self-test
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e mpu6515_set_config_register(
   mpu6515_s* state,
   mpu6515_sensor_s* sensor,
   uint8_t           range_index,
   uint8_t           accel_hpf,
   boolean           enable_selftest)
{
  uint8_t  reg_addr = 0;
  uint8_t  reg_data = 0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  // Check for null-pointers
  if (NULL == state || NULL == sensor)
  {
    MPU6050_MSG_2(ERROR, "set_config_register: Null parameter (state = 0x%X) (sensor = 0x%X).", state, sensor);
    return SNS_DDF_EFAIL;
  }

  if (mpu6515_is_dmp_active(state))
  {
    return SNS_DDF_SUCCESS; //SNS_DDF_EINVALID_PARAM;
  }

  switch(sensor->sensor_type)
  {
    case SNS_DDF_SENSOR_ACCEL:
      if(range_index >= MPU6050_TABLE_SIZE(mpu6515_accel_range_reg))
      {
        status = SNS_DDF_EINVALID_PARAM;
      }
      else
      {
        reg_addr  = MPU6050_REG_ACCEL_CONFIG;
        reg_data  = mpu6515_accel_range_reg[range_index] | accel_hpf;
        reg_data |= (enable_selftest == FALSE) ?
          MPU6050_ACCEL_ST_DISABLE : MPU6050_ACCEL_ST_ENABLE;
        sensor->range = range_index;
        state->mpu_reg[MPU_REG_ACCEL_CONFIG] = reg_data;
      }
      break;

    case SNS_DDF_SENSOR_GYRO:
      if(range_index >= MPU6050_TABLE_SIZE(mpu6515_gyro_range_reg))
      {
        status = SNS_DDF_EINVALID_PARAM;
      }
      else
      {
        reg_addr  = MPU6050_REG_GYRO_CONFIG;
        reg_data  = mpu6515_gyro_range_reg[range_index];
        reg_data |= (enable_selftest == FALSE) ?
          MPU6050_GYRO_ST_DISABLE : MPU6050_GYRO_ST_ENABLE;
        sensor->range = range_index;
        state->mpu_reg[MPU_REG_GYRO_CONFIG] = reg_data;
      }
      break;

    default:
      status = SNS_DDF_EINVALID_PARAM;
      break;
    }

  if(enable_selftest && status==SNS_DDF_SUCCESS)
  {
    sns_ddf_write_port_byte(state->port_handle, reg_addr, reg_data, &status);
  }

  MPU6050_MSG_4(HIGH, "range: ddf_sensor=%d range_idx=%d selftest?=%d err=%d",
                sensor->sensor_type, sensor->range, enable_selftest, status);

  return status;
}


// Set @exception to NULL if you do not want the function to use the exception
static bool mpu6515_is_ready_for_reset(
    mpu6515_s* state, sns_ddf_handle_t exception)
{
  uint8_t i;

  // Check for null-pointers
  if (state == NULL)
    return false;

  uint8_t sub_dev_exception_idx = mpu6515_get_sub_dev_idx(exception);
  /* Note: there is no need to check for a valid sub-device exception index.
   * The faulty exception index will essentially exclude the check for an exception.
   */

  // Check each sub-device, unless if it is the exception
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    if ((i != sub_dev_exception_idx) && (state->sub_devices[i].powerstate == SNS_DDF_POWERSTATE_ACTIVE))
    {
      MPU6050_MSG_1(HIGH, "device is NOT ready for reset, sub dev %d is ACTIVE", i);
      return false;
    }
  }

  MPU6050_MSG_0(HIGH, "device is ready for reset");
  return true;
}

/**
 * @brief Resets all sensors that have the same dd_handle (i.e. within the same sub-device)
 */
sns_ddf_status_e mpu6515_reset_sub_dev(
    mpu6515_s* state, uint8_t sub_dev_idx)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_handle_t dd_handle;
  mpu6515_sub_dev_s* sub_dev;
  mpu6515_sensor_s*  sensor_ptr = NULL;
  uint8_t bytes_read, int_reg_conf = 0;

  MPU6050_MSG_1(HIGH, "reset_sub_dev: idx %d", sub_dev_idx);

  // Check for null-pointers, out-of-bound indices, and other invalid parameters
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;
  if (sub_dev_idx >= state->sub_device_cnt)
    return SNS_DDF_EINVALID_PARAM;

  sub_dev = &state->sub_devices[sub_dev_idx];
  dd_handle = (sns_ddf_handle_t) ( (uint32_t) (state) | sub_dev_idx );

  // Checks primary data type
  if(sub_dev->sensors[0].sensor_type == SNS_DDF_SENSOR_MAG)
  {
    sensor_ptr = mpu6515_get_sensor(dd_handle, SNS_DDF_SENSOR_MAG);
    if(sensor_ptr != NULL)
    {
      MPU6050_MSG_1(HIGH, "reset_sub_dev: akm8963.reset() dd_handle=0x%x", sensor_ptr->aux_dev.st_aux_dev);

      status = sns_ddf_read_port(state->port_handle, MPU6050_REG_INT_PIN_CFG, &int_reg_conf, 1, &bytes_read);
      if(status != SNS_DDF_SUCCESS || bytes_read != 1) {
        MPU6050_MSG_2(ERROR, "reset_sub_dev: err=%d, port_hndl=0x%08x", status, state->port_handle);
        return status;
      }

      MPU6050_MSG_3(HIGH, "reset_sub_dev: enters bypass (reg=0x%02x), bit=0x%02x, hndl=0x%08x", int_reg_conf, I2C_BYPASS_EN_BIT, state->port_handle);
      sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf | I2C_BYPASS_EN_BIT, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;

      status = sns_mag_akm8963_driver_fn_list.reset(sensor_ptr->aux_dev.st_aux_dev);
      if(status != SNS_DDF_SUCCESS) {
        sns_ddf_status_e init_status = status;
        sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
        if(status != SNS_DDF_SUCCESS)
          return status;
        MPU6050_MSG_1(FATAL, "reset_sub_dev: akm8963.reset() failed (err=%d)", init_status);
        return init_status;
      }

      sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;
      MPU6050_MSG_2(HIGH, "reset_sub_dev: exits bypass port_hndl=0x%08x ddf_sensor=%d", state->port_handle, sensor_ptr->sensor_type);
    }
    else
    {
      MPU6050_MSG_1(FATAL, "reset_sub_dev: akm8963.reset() failed (NULL ptr, dd_handle=0x%x)", dd_handle);
      return status;
    }
  }
  else if(sub_dev->sensors[0].sensor_type == SNS_DDF_SENSOR_PRESSURE)
  {
    sensor_ptr = mpu6515_get_sensor(dd_handle, SNS_DDF_SENSOR_PRESSURE);
    if(sensor_ptr != NULL)
    {
      MPU6050_MSG_1(HIGH, "reset_sub_dev: bmp280.reset() dd_handle=0x%x", sensor_ptr->aux_dev.st_aux_dev);

      status = sns_ddf_read_port(state->port_handle, MPU6050_REG_INT_PIN_CFG, &int_reg_conf, 1, &bytes_read);
      if(status != SNS_DDF_SUCCESS || bytes_read != 1) {
        MPU6050_MSG_2(ERROR, "reset_sub_dev: err=%d, port_hndl=0x%08x", status, state->port_handle);
        return status;
      }

      MPU6050_MSG_3(HIGH, "reset_sub_dev: enters bypass (reg=0x%02x), bit=0x%02x, hndl=0x%08x", int_reg_conf, I2C_BYPASS_EN_BIT, state->port_handle);
      sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf | I2C_BYPASS_EN_BIT, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;

      status = sns_alt_bmp280_driver_fn_list.reset(sensor_ptr->aux_dev.st_aux_dev);
      if(status != SNS_DDF_SUCCESS) {
        sns_ddf_status_e init_status = status;
        sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
        if(status != SNS_DDF_SUCCESS)
          return status;
        MPU6050_MSG_1(FATAL, "reset_sub_dev: bmp280.reset() failed (err=%d)", init_status);
        return init_status;
      }

      sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_INT_PIN_CFG, int_reg_conf, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;
      MPU6050_MSG_2(HIGH, "reset_sub_dev: exits bypass port_hndl=0x%08x ddf_sensor=%d", state->port_handle, sensor_ptr->sensor_type);
    }
    else
    {
      MPU6050_MSG_1(FATAL, "reset_sub_dev: bmp280.reset() failed (NULL ptr, dd_handle=0x%x)", dd_handle);
      return status;
    }
  }
    
  // Reset the entire sub-device (and all sensors contained within)
  status = mpu6515_set_powerstate(state, sub_dev_idx, SNS_DDF_POWERSTATE_LOWPOWER);
  if (status != SNS_DDF_SUCCESS)
    return status;

  return status;
}


/**
 * @brief Reset the whole device
 *
 * @param[in] state        Ptr to the driver structure
 *
 * @return  status of this operation.
 */
sns_ddf_status_e mpu6515_reset_device(mpu6515_s* state)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i2c_buff, bytes_written;
  mpu6515_sub_dev_s* sub_dev;
  uint8_t i, j;

  // Check for null-pointers
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;

  /* Hardware-reset the device */
  i2c_buff = DEVICE_RESET_ENABLE;
  status = sns_ddf_write_port(
      state->port_handle,
      MPU6050_REG_PWR_MGMT_1,
      &i2c_buff,
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
      return status;
  MPU6050_MSG_1(HIGH, "reset_dev: hard, sub_dev_cnt=%d", state->sub_device_cnt);

  /* Software-reset all of the register fields */
  state->int_registered = false;
  state->cycle_mode = false;
  state->md_enabled = false;
  state->lpf_index = 0;   // TODO: CHECK THAT THIS IS THE PROPER DEFAULT LPF INDEX VALUE
  dev_fifo_len = 0;

  /* Reset all sub-devices within the device */
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    status = mpu6515_reset_sub_dev(state, i);
    if(status != SNS_DDF_SUCCESS)
    {
      MPU6050_MSG_2(ERROR, "reset_dev: failed to reset subdev %d (status=%d)", state->sub_device_cnt, status);
      continue;
    }

    sub_dev = &state->sub_devices[i];

    // TODO: assumes all sub-devices use DMP
    for (j = 0; j < sub_dev->num_sensors; j++)
    {
      sub_dev->sensors[j].desired_odr = 0;
      sub_dev->sensors[j].odr = 0;

      status = mpu6515_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);
      if (status != SNS_DDF_SUCCESS)
        return status;

      if(i == 0)    // load it once for first sub-device
      {
        status = mpu6515_setup_dmp(state);
        if (status != SNS_DDF_SUCCESS)
          return status;

        MPU6050_MSG_0(MED, "reset_dev: DMP loaded");
        break;    // no need to flash again.
      }

      MPU6050_MSG_0(MED, "reset_dev: Avoid loading DMP again");
    }
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Sets the device to its default operational state
 * @see sns_ddf_driver_if_s.reset()
*/
sns_ddf_status_e mpu6515_reset(sns_ddf_handle_t dd_handle)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  sns_ddf_time_t  current_time = sns_ddf_get_timestamp();

  MPU6050_MSG_2(LOW, ".reset(0x%x) state=0x%x", dd_handle, state);

  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;

  state->current_thres=0;

  if( (current_time-state->last_reset_time) > DMP_NO_RESET_WITHIN_HALF_SECOND_IN_TICK)
  {
    state->last_reset_time = current_time;
    
    if( mpu6515_is_ready_for_reset(state, dd_handle) )
    {
      return mpu6515_reset_device(state);
    }
    else
    {
      MPU6050_MSG_0(MEDIUM, "The device is currently active so a software reset of internal structures will be performed.");
      uint8_t sub_dev_idx = mpu6515_get_sub_dev_idx(dd_handle);
      if(sub_dev_idx < MPU6515_MAX_SUB_DEV)
      {
        return mpu6515_reset_sub_dev(state, sub_dev_idx);
      }
      else
      {
        return SNS_DDF_EFAIL;
      }
    }
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Set accel/gyro sensor to standy if possible
 * @see 
*/
sns_ddf_status_e sns_dd_mpu6515_config_standby(mpu6515_s* state)
{
  mpu6515_sensor_s *sensor_accel_ptr, *sensor_gyro_ptr, *sensor_6quat_ptr;
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t bytes_written;
  
  if(!state->md_enabled)
  {
    sensor_accel_ptr  = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, NULL);
    sensor_gyro_ptr   = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_GYRO, NULL);
	sensor_6quat_ptr  = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_GAME_RV, NULL);
    status = sns_ddf_read_port(state->port_handle, MPU_REG_PWR_MGMT_1,
                                   &(state->mpu_reg[MPU_REG_PWR_MGMT_1]), 2, &bytes_written);
    if(status != SNS_DDF_SUCCESS)
       return status;

    if( (sensor_accel_ptr != NULL) && (sensor_accel_ptr->odr > 0) )
    {
        state->mpu_reg[MPU_REG_PWR_MGMT_2] &= ~(STBY_XA_ENABLE | STBY_YA_ENABLE | STBY_ZA_ENABLE);
            
        // assuming [0] is accel... workaround
        state->sub_devices[0].powerstate = SNS_DDF_POWERSTATE_ACTIVE; 
        status = mpu6515_set_odr(state, sensor_accel_ptr, sensor_accel_ptr->desired_odr);
        if(status != SNS_DDF_SUCCESS)
        {
            MPU6050_MSG_1(FATAL, "set_dmp_wm: set_odr failed (%d)", status);
            return status;
        }
    }
    else
    {
        state->mpu_reg[MPU_REG_PWR_MGMT_2] |= (STBY_XA_ENABLE | STBY_YA_ENABLE | STBY_ZA_ENABLE);
    }


    if( (sensor_gyro_ptr != NULL) && (sensor_gyro_ptr->odr > 0) )
    {
        state->mpu_reg[MPU_REG_PWR_MGMT_2] &= ~(STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
    }
    else
    {
        state->mpu_reg[MPU_REG_PWR_MGMT_2] |= (STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
    }

	/*todo ? need to add for 3quat also in case to supportit later*/
	if( (sensor_6quat_ptr != NULL) && (sensor_6quat_ptr->odr > 0) )
    {
        state->mpu_reg[MPU_REG_PWR_MGMT_2] &= ~(STBY_XA_ENABLE | STBY_YA_ENABLE | STBY_ZA_ENABLE);
        state->mpu_reg[MPU_REG_PWR_MGMT_2] &= ~(STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
        // assuming [0] is accel... workaround
        state->sub_devices[0].powerstate = SNS_DDF_POWERSTATE_ACTIVE; 
    }
				  
    status = mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1,
                                    &(state->mpu_reg[MPU_REG_PWR_MGMT_1]), 2);
    if(status != SNS_DDF_SUCCESS)
         return status;
  }

  return status;
}		

/**
 * @brief Sets dmp wm for sensor
 * ASSUMING dmp sensors have same odr
 *
 */
sns_ddf_status_e mpu6515_set_dmp_wm(
  mpu6515_s* state,
  mpu6515_sensor_s * sensor,
  uint16_t watermark)
{
   uint8_t dmp_sensor;
   uint8_t bytes_written;
   uint8_t buffer[2], rb_buffer[2];
   sns_ddf_status_e status;
   uint8_t iter,  num_dmp_fifo_sensors_enabled=0;
   uint16_t current_iter=0,total_watermark=0,max_watermark=0;
   uint32_t true_dmp_pktsize = 0;
   sns_ddf_time_t time_now = sns_ddf_get_timestamp();
   
   MPU6050_MSG_3(MEDIUM, "set_dmp_wm: req_wm=%d ddf_sensor=%d sensor_bitmap=0x%x", 
                 watermark, sensor->sensor_type, dmp_sensor_bitmap);
   dmp_sensor = sns_dd_dmp_get_sensor(sensor->sensor_type);
   mpu6515_sensor_s *sensor_mag_ptr, *sensor_press_ptr;
   
   /* Ensures device is not in SLEEP mode. This may explain why I2C does not respond
      to additional writes (not documented in datasheet). */
   status = mpu6515_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);
   if(status != SNS_DDF_SUCCESS)
     return status;
	 

   for (iter = 1; iter < 16; iter++)
   {  
      current_iter = 0x0001 << iter;
      if(dmp_sensor_bitmap & current_iter)
      {
         mpu6515_sensor_s* sensor_ptr = 
            mpu6515_search_for_sensor(state, DMP_GET_DDF_SENSOR(iter), NULL);
         if (sensor_ptr != NULL)
         {
            if (sensor_ptr->last_sample_timestamp == 0)
            {
               sensor_ptr->last_sample_timestamp = time_now;
               MPU6050_MSG_2(LOW, "set_dmp_wm: ddf_sensor=%d ts=%u", 
                             sensor_ptr->sensor_type, time_now);
            }

            if( sensor_ptr->watermark > max_watermark )
            {
	  		       max_watermark=sensor_ptr->watermark;
            }

            true_dmp_pktsize += DMP_GET_PKT_SIZE(iter)*(sensor_ptr->watermark);
            total_watermark += sensor_ptr->watermark;

            // not every sensor represented in bitmap uses FIFO
            if((iter !=DMP_SENSOR_SMD) && (iter !=DMP_SENSOR_STEPDET) && (iter !=DMP_SENSOR_STEPCNT))
            {
               num_dmp_fifo_sensors_enabled++;
            }
         }
      }
   }

   MPU6050_MSG_3(MEDIUM, "set_dmp_wm: req_wm=%d true_dmp_pktsize=%d total_watermark=%d", 
                 watermark, true_dmp_pktsize, total_watermark);

   MPU6050_MSG_2(MEDIUM, "set_dmp_wm: num_dmp_fifo_sensors_enabled=%d max_watermark=%d", 
                  num_dmp_fifo_sensors_enabled, max_watermark);				 

   if( watermark == 0 )
   {
       DMP_DISABLE_SENSOR(dmp_sensor);
	   is_dmp_enabled = false;
   }	 
     
   if( (total_watermark ==0) && ( dmp_sensor_bitmap ==0 )  )
   {
	   state->current_thres = 0;
	   state->dd_fifo_config.watermark = 0;
        // Resets DMP and fifo
        sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_USER_CTRL, 0x0C, &status);
        
        if (status != SNS_DDF_SUCCESS)
          return status;

		/* Enable/disable Accel and gyro Stand-by Mode */
		sns_dd_mpu6515_config_standby(state);
		
   }
   else
   {
     uint16_t  thres;
     // reconfigure dmp and FIFO
     if(state->device_id == DEVICE_ID_MPU6515)
     {
        
        // Set Batch FIFO packet threshold
        if( true_dmp_pktsize<=MAX_DMP_FIFO_SIZE_2_USE ) {
          thres = max_watermark;
        }
        else
        {
         thres = max_watermark/(true_dmp_pktsize/MAX_DMP_FIFO_SIZE_2_USE+1);  
        }
 
        MPU6050_MSG_3(MEDIUM, "mpu6515_set_dmp_wm: num_dmp_fifo_sensors_enabled=%d, thres=%d, dmp_sensor_bitmap=0x%04x", num_dmp_fifo_sensors_enabled, thres, dmp_sensor_bitmap);
        
		if( state->dd_fifo_config.watermark == thres)
		{
		   MPU6050_MSG_1(MEDIUM, "mpu6515_set_dmp_wm:  threshhold %d is not changed", thres);
		   		 
		}
		else if( (dmp_sensor !=DMP_SENSOR_STEPDET) && (dmp_sensor !=DMP_SENSOR_SMD ) && (dmp_sensor !=DMP_SENSOR_STEPCNT))
        {
		   // if watermark has some positive value and different from current value , then set it and enable fifo.
           // stop/reset DMP and FIFO
            sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_USER_CTRL, 0x0C, &status);
            if (status != SNS_DDF_SUCCESS)
               return status;
            is_dmp_enabled = false;
            buffer[0] = MPU6XXX_HIGH_BYTE(thres);
            buffer[1] = MPU6XXX_LOW_BYTE(thres);
            MPU6050_MSG_1(MEDIUM, "mpu6515_set_dmp_wm: Setting DMP WM threshold (in DMP unit): %u ", thres);
            status = mpu6515_write_dmp_reg(state, D_BM_BATCH_THLD, buffer, 2);
            if (status != SNS_DDF_SUCCESS)
               return status;
            state->current_thres = thres;
			state->dd_fifo_config.watermark = thres; 

            // Enable Batch FIFO
            buffer[0] = 1;
            status = mpu6515_write_dmp_reg(state, D_BM_ENABLE, buffer, 1);
            if (status != SNS_DDF_SUCCESS)
               return status;
        }

        MPU6050_MSG_1(MEDIUM, "mpu6515_set_dmp_wm: current fifo thres=%u pkts", state->current_thres);

        /* Enable/disable Accel and gyro Stand-by Mode */
		status=sns_dd_mpu6515_config_standby(state);
		if (status!=SNS_DDF_SUCCESS)
		  return status;
         
        /* Enable FIFO */
        sensor_mag_ptr   = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_MAG, NULL);
        sensor_press_ptr = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_PRESSURE, NULL);
        buffer[0] = DMP_EN_ENABLED | FIFO_EN_ENABLED;
        if( ((sensor_mag_ptr != NULL) && (sensor_mag_ptr->odr > 0)) || ((sensor_press_ptr != NULL) && (sensor_press_ptr->odr > 0)) )
        {
           buffer[0] |= BIT_I2C_MST_EN;
        }

        status = sns_ddf_write_port(state->port_handle, MPU6500_REG_USER_CTRL, buffer, 1, &bytes_written);
        if(status != SNS_DDF_SUCCESS)  return status;
        is_dmp_enabled = true;
        MPU6050_MSG_2(HIGH, "set_dmp_wm: Enabled dmp and fifo (USER_CTRL=0x%02x  bytes_written=%d)", buffer[0], bytes_written);
       
        /* Enable DMP */ 
        rb_buffer[0] = 0xEE;
        bytes_written = 0;
        status = sns_ddf_read_port(state->port_handle, MPU6515_REG_USER_CTRL, rb_buffer, 1, &bytes_written);
        if(status != SNS_DDF_SUCCESS)  return status;
        MPU6050_MSG_2(HIGH, "set_dmp_wm: read-back USER_CTRL=0x%02x  bytes_read=%d", rb_buffer[0], bytes_written);

        sns_ddf_delay(5000);
        mpu6515_enable_i2c_slv(state);

//        mpu6515_check_dmp_heartbeat_reg(state);
//        mpu6515_read_dmp_firmware(state);
//        mpu6515_print_dmp_reg(state);
//        mpu6515_print_mpu_reg(state);
//        LOG_MPU_REG(state);
//        LOG_DMP_REG(state);
     }
  }
  return SNS_DDF_SUCCESS;
}

/*
 * set_dmp_mode() is ONLY used for switching between MD interrupt and
 *  active ACCEL stream.
 */
sns_ddf_status_e mpu6515_set_dmp_mode(mpu6515_s* state, uint8_t mode)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t buffer = 0, bytes_read;
  uint8_t *buf;

  buf = &(state->mpu_reg[MPU_REG_PWR_MGMT_1]);
  status = sns_ddf_read_port(state->port_handle, MPU_REG_PWR_MGMT_1, buf, 2, &bytes_read);
  if(status != SNS_DDF_SUCCESS)
    return status;

  switch(mode)
  {
    case 0:
      // Disables DMP
      buffer = FIFO_RST_RESET | DMP_RST_RESET;
      status = mpu6515_write_mpu_reg(state, MPU_REG_USER_CTRL, &buffer, 1);
      if(status != SNS_DDF_SUCCESS)
        return status;
      is_dmp_enabled = false;
      if (dev_fifo_len > 0)
      {
        MPU6050_MSG_1(ERROR, "set_dmp_mode: %d bytes from fifo discarded", dev_fifo_len);
      }
      dev_fifo_len = 0;

      // Puts accel and gyro in low-power mode
      //buf[0] |= (CYCLE_ENABLE | TEMP_DIS_ENABLE);
      buf[0] &= ~SLEEP_ENABLE;
      buf[1] |= (STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
      status = mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1, buf, 2);
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Set Accel LPF to 184-Hz bandwidth
      state->mpu_reg[MPU_REG_ACCEL_CONFIG_2] = 0x01;
      status = mpu6515_write_mpu_reg(state, MPU_REG_ACCEL_CONFIG_2, &(state->mpu_reg[MPU_REG_ACCEL_CONFIG_2]), 1);
      if(status != SNS_DDF_SUCCESS)
        return status;

      break;
    case 1:
      // Revert Accel LPF setting (for DMP)
      state->mpu_reg[MPU_REG_ACCEL_CONFIG_2] = 0x40;
      status = mpu6515_write_mpu_reg(state, MPU_REG_ACCEL_CONFIG_2, &(state->mpu_reg[MPU_REG_ACCEL_CONFIG_2]), 1);
      if(status != SNS_DDF_SUCCESS)
        return status;
      
      // Puts accel in active mode (gyro remains low-power)
      //buf[0] &= ~(CYCLE_ENABLE | TEMP_DIS_ENABLE);
      buf[0] &= ~SLEEP_ENABLE;
      buf[1] |= (STBY_XG_ENABLE | STBY_YG_ENABLE | STBY_ZG_ENABLE);
      status = mpu6515_write_mpu_reg(state, MPU_REG_PWR_MGMT_1, buf, 2);
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Enables DMP
      buffer = FIFO_EN_ENABLED | DMP_EN_ENABLED;
      status = mpu6515_write_mpu_reg(state, MPU_REG_USER_CTRL, &buffer, 1);
      if(status != SNS_DDF_SUCCESS)
        return status;
      is_dmp_enabled = true;

      break;
    default:
      MPU6050_MSG_1(ERROR, "set_dmp_mode: invalid mode (%d)", mode);
      break;
  }

  MPU6050_MSG_2(HIGH, "set_dmp_mode: byte=%x, status=%d", buffer, status);

  return status;
}

/**
 * @brief Sets an attribute of the gyroscope
 * @see sns_ddf_driver_if_s.set_attrib()
 */
sns_ddf_status_e mpu6515_set_attr(
   sns_ddf_handle_t    dd_handle,
   sns_ddf_sensor_e    sensor_type,
   sns_ddf_attribute_e attrib,
   void*               value)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  mpu6515_sub_dev_s * sub_device = mpu6515_get_sub_dev(dd_handle);
  uint8_t sub_dev_idx = mpu6515_get_sub_dev_idx(dd_handle);
  mpu6515_sensor_s * sensor;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t n;
  if (dd_handle == NULL || value == NULL || state == NULL || sub_device == NULL)
     return SNS_DDF_EINVALID_PARAM;

  if(sub_dev_idx >= MPU6515_MAX_SUB_DEV)
  {
     return SNS_DDF_EINVALID_PARAM;
  }

  // If the requested sensor type is NONE, then do nothing
  if (sensor_type == SNS_DDF_SENSOR__NONE)
  {
    MPU6050_MSG_0(LOW, "set_attr no actions available for sensor type NONE");
    return SNS_DDF_SUCCESS;
  }
  /* If the requested sensor type is ALL, then iterate through all of the
   * sub-device's sensors and call this function with a specific sensor type */
  if (sensor_type == SNS_DDF_SENSOR__ALL)
  {
    for (n = 0; n < sub_device->num_sensors; n++)
    {
      status = mpu6515_set_attr(dd_handle, sub_device->sensors[n].sensor_type, attrib, value);
      if (status != SNS_DDF_SUCCESS)
        return status;
    }
    return SNS_DDF_SUCCESS;
  }

  sensor = mpu6515_get_sensor(dd_handle, sensor_type);
  /* If the sensor type is not found in the sub-device associated with the
     handle, then report an error */
  if (NULL == sensor)
  {
    MPU6050_MSG_2(ERROR, "set_attr: Cannot find the sensor type %u within the dd_handle 0x%X", sensor_type, dd_handle);
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
    {
      sns_ddf_powerstate_e* powerstate = value;
      return mpu6515_set_powerstate(state, sub_dev_idx, *powerstate);
    }

    case SNS_DDF_ATTRIB_RANGE:
    {
      uint8_t *range_index = value;
      status = SNS_DDF_EINVALID_PARAM;
      
      switch(sensor_type)
      {
        case SNS_DDF_SENSOR_ACCEL:
        case SNS_DDF_SENSOR_GYRO:
          status = mpu6515_set_config_register(state,
                                               sensor,
                                               *range_index,
                                               ACCEL_HPF_None , false);
          break;
        case SNS_DDF_SENSOR_MAG:
          status = sns_mag_akm8963_driver_fn_list.set_attrib(
                      &(sensor->aux_dev.st_aux_dev),
                      sensor_type,
                      SNS_DDF_ATTRIB_RANGE,
                      range_index);
          break;
        case SNS_DDF_SENSOR_PRESSURE:
          status = sns_alt_bmp280_driver_fn_list.set_attrib(
                      &(sensor->aux_dev.st_aux_dev),
                      sensor_type,
                      SNS_DDF_ATTRIB_RANGE,
                      range_index);
          break;
        default:
          break;
      }

      return status;
    }

    case SNS_DDF_ATTRIB_LOWPASS:
    {
      //uint8_t* attr_ptr = value;
      status = SNS_DDF_EINVALID_PARAM;

      switch(sensor_type)
      {
        case SNS_DDF_SENSOR_ACCEL:
        case SNS_DDF_SENSOR_GYRO:
          //status = mpu6515_set_lowpass(state, *attr_ptr);
          break;
        default:
          break;
      }

      return status;
    }

    case SNS_DDF_ATTRIB_BIAS:
    {
      sns_ddf_bias_t* bias = value;
      int32_t dmp_bias[MPU6XXX_MAX_NUM_AXES];
      int i;

      if(sensor_type == SNS_DDF_SENSOR_GYRO)
      {
        if(sensor->biases[0] != bias[0] ||
           sensor->biases[1] != bias[1] ||
           sensor->biases[2] != bias[2])
        {
          dmp_convert_bias( sensor, dmp_bias, bias );

          for(i = 0; i < MPU6XXX_MAX_NUM_AXES; i++)
          {
            sensor->biases[i] = bias[i];

            status = mpu6515_write_dmp_reg(state, dmp_bias_addr[i], (uint8_t*) &(dmp_bias[i]), sizeof(dmp_bias[0]));
            if (status != SNS_DDF_SUCCESS)
              return status;
          }
        }

        status = SNS_DDF_SUCCESS;
      }
      else
      {
        MPU6050_MSG_1(ERROR, "set_attr(bias) invalid ddf_sensor=%d", sensor_type);
        status = SNS_DDF_EINVALID_PARAM;
      }
      return status;
    }
    case SNS_DDF_ATTRIB_ODR:
    {
      sns_ddf_odr_t* odr = value;
      status = SNS_DDF_SUCCESS;
      if(*odr > 0 && state->md_enabled)
      {
        status = mpu6515_disable_interrupt(state, sub_device, sensor, 
                                           MPU6050_INT_MOTION_DETECT);
        if(status == SNS_DDF_SUCCESS)
        {
          sns_ddf_smgr_notify_event(
                    sub_device->smgr_handle,
                    SNS_DDF_SENSOR_ACCEL,
                    SNS_DDF_EVENT_MOTION);
        }
      }
      if (status == SNS_DDF_SUCCESS) 
      {
        status = mpu6515_set_odr(state, sensor, *odr);
      }
      return status;
    }
    case SNS_DDF_ATTRIB_MOTION_DETECT:
    {
      sns_ddf_status_e status = SNS_DDF_EFAIL;
      bool * flag = value;
      sensor = mpu6515_get_sensor(dd_handle, SNS_DDF_SENSOR_ACCEL);

      MPU6050_MSG_3(HIGH, "set_attr %d md_en=%d flag=%d", 9909, state->md_enabled, *flag);
      if (NULL == sensor)
      {
        return SNS_DDF_EINVALID_PARAM;
      }

      if ( *flag == state->md_enabled )     /* is it a redundant request */
      {
        state->cnt.md_dup_warning++;
        MPU6050_MSG_1(HIGH, "set_attr: Redundant request to enable/disable MD. (flag = %u)", *flag);
        return SNS_DDF_SUCCESS;
      }
      else if(*flag)
      {
        if ( ! DMP_IS_BITMAP_EQUAL(accel_only_bitmap) )
        {
          MPU6050_MSG_1(ERROR, "set_attr: md: cannot enable. bitmap=%04x", dmp_sensor_bitmap);
          return SNS_DDF_EINVALID_PARAM;
        }

        if ( sensor->desired_odr )   /* Accel shall be activated for MOTION_DETECT */
        {
          DMP_SAVE_BITMAP(prev_dmp_sensor_bitmap);
          DMP_CLEAR_BITMAP();
          mpu6515_set_dmp_mode(state, 0);
          status = mpu6515_enable_interrupt(state, sub_device, sensor, MPU6050_INT_MOTION_DETECT);
          if( SNS_DDF_SUCCESS == status )
          {
            state->md_accel_odr = sensor->desired_odr;
            state->md_enabled = TRUE;     /* this flag shall be set before calling set_odr() */
            state->cnt.md_state++;
            MPU6050_MSG_0(HIGH, "set_attr: MD enabled");
            //status = mpu6515_set_odr(state, sub_device, sensor, sensor->desired_odr);
          }
          else
          {
            MPU6050_MSG_1(ERROR, "set_attr: cannot enable MD interrupt for instance 0x%X", state);
            state->cnt.md_err++;
          }
        }
        else
        {
          MPU6050_MSG_0(ERROR, "set_attr: cannot enable MD without ACCEL active.");
          state->cnt.md_err++;
          status = SNS_DDF_EFAIL;
        }
        return status;
      }
      else
      {
        MPU6050_MSG_0(MEDIUM, "Disabling Motion-Detect...");
        /* disable motion detection */
        status = mpu6515_disable_interrupt( state, sub_device, sensor, MPU6050_INT_MOTION_DETECT);
        if( SNS_DDF_SUCCESS == status )
        {
          state->md_enabled = FALSE;
          MPU6050_MSG_3(HIGH, "set_attr %d status=%d md_state=%d", 9911, status, state->cnt.md_state);
          /* recover the previous state */
          DMP_RESTORE_BITMAP(prev_dmp_sensor_bitmap);

          sensor->desired_odr = state->md_accel_odr;
          status = mpu6515_set_odr(state, sensor, sensor->desired_odr);
          if(status != SNS_DDF_SUCCESS)
            return status;

          if (!DMP_ALL_SENSOR_OFF())
          {
            status = mpu6515_set_dmp_mode(state, 1);
          }
        }
      }
      return status;
    }

    case SNS_DDF_ATTRIB_IO_REGISTER:
    {
      sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) value;
      uint8_t bytes_written;
      sns_ddf_status_e status;
      uint32_t reg_addr = (uint32_t) io_reg_in->address_ptr;

      // Address sizes are limited to uint16_t for the MPU6XXX
      if (reg_addr > 0xFFFF)
      {
        MPU6050_MSG_1(HIGH, "set_attr: invalid address %a", (io_reg_in->address_ptr));
        return SNS_DDF_EINVALID_PARAM;
      }
      // sns_ddf_write_port is limited to writing 0xFF bytes
      if (io_reg_in->length > 0xFF)
      {
        MPU6050_MSG_1(HIGH, "set_attr: length too long, %d", io_reg_in->length);
        return SNS_DDF_EINVALID_PARAM;
      }

      status = sns_ddf_write_port(
          state->port_handle,
          (uint16_t) reg_addr,  // MPU6515/DMP has address value up to two bytes long
          (uint8_t*) io_reg_in->data,
          (uint8_t) io_reg_in->length,
          &bytes_written);

      MPU6050_MSG_2(MEDIUM, "set_attr: (IO_REGISTER) Wrote to address 0x%X  length %d", io_reg_in->address_ptr, io_reg_in->length);
      return status;
    }
    case SNS_DDF_ATTRIB_FIFO:
    {
      // set fifo attribute.
      sns_ddf_fifo_set_s* fifo = value;
      sensor->watermark = fifo->watermark;
      return mpu6515_set_dmp_wm(state,sensor, fifo->watermark);
    }
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
}

static void mpu6515_log_data(
    mpu6515_s* state,
    sns_ddf_sensor_e  sensor)
{
  sns_err_code_e status;
  sns_log_sensor_data_pkt_s* log;
  uint8_t i;

  status = sns_logpkt_malloc(
      SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s),
      (void**)&log);

  if(status != SNS_SUCCESS || log == NULL)
  {
      // Can't allocate log packet. Ignore silently.
      return;
  }

  log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
  log->sensor_id = sensor;
  log->vendor_id = SNS_DDF_VENDOR_INVENSENSE;
  log->timestamp = state->data_cache_time;
  log->num_data_types = state->sensor_data.num_samples; // dimension

  for(i = 0; i < log->num_data_types; i++)
  {
     log->data[i] = state->sensor_data.samples[i].sample;
  }

  // Commit and free the log packet.
  sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
}

/*===========================================================================
FUNCTION:   mpu6515_log_fifo_data
===========================================================================*/
/*!
  @brief log sensor data from fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] state
  @param[in] dmp_sensor
  */
/*=========================================================================*/
void mpu6515_log_fifo_data(mpu6515_s* state, uint8_t dmp_sensor)
{
  sns_err_code_e status;
  sns_log_sensor_data_pkt_s* logpkt_ptr = NULL;
  uint16 size = sizeof(sns_log_sensor_data_pkt_s) + 
    (state->sensor_data.num_samples-1)*sizeof(int32_t);

  status = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA, size, (void**)&logpkt_ptr);

  if ((status == SNS_SUCCESS) && (logpkt_ptr != NULL))
  {
    uint16_t i;

    logpkt_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    logpkt_ptr->sensor_id = state->sensor_data.sensor;
    logpkt_ptr->vendor_id = SNS_DDF_VENDOR_INVENSENSE;

    logpkt_ptr->timestamp      = state->sensor_data.timestamp;
    logpkt_ptr->end_timestamp  = state->sensor_data.end_timestamp;
    logpkt_ptr->num_data_types = MAX(1,DMP_GET_DIM(dmp_sensor));
    logpkt_ptr->num_samples    = 
      state->sensor_data.num_samples / logpkt_ptr->num_data_types;

    for (i=0; i<ARR_SIZE(logpkt_ptr->data) && i<logpkt_ptr->num_data_types; i++)
    {
      logpkt_ptr->data[i]  = state->sensor_data.samples[i].sample;
    }

    for(i=0; i<state->sensor_data.num_samples; i++)
    {
      logpkt_ptr->samples[i] = state->sensor_data.samples[i].sample;
    }

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, logpkt_ptr);
  }
}

/**
 * @brief Reads bytes from DMP FIFO
 *
 * @param[i] state: Pointer to the device-instance structure
 * @param[i] max_num_byte: maximum number of bytes to be read from FIFO
 * @param[o] buf: pointer to buffer to be written
 *
 * @return  Number of bytes actually read from FIFO (and written to 'buf')
 */
uint16_t sns_dd_dmp_read_fifo(mpu6515_s* state, int max_num_byte, uint8_t* buf)
{
  uint8_t buffer[2] = { 0 };
  uint32_t total_bytes_read = 0, bytes_read = 0;
  uint32 byte_cnt, fifo_cnt = 0;
  sns_ddf_status_e status;

  // Burst-read the FIFO count
  status = sns_ddf_read_port( state->port_handle, MPU6XXX_REG_FIFO_COUNTH, 
                              buffer, 2, (uint8_t*)&bytes_read);
  if (status != SNS_DDF_SUCCESS)
  {
    return 0;
  }

  // Merge the high byte and the low byte of the FIFO_COUNT
  buffer[0] &= 0x1F;
  fifo_cnt = (((uint16_t) buffer[0]) << 8) | (buffer[1]);
  byte_cnt = MIN(fifo_cnt, max_num_byte);
  while (total_bytes_read < byte_cnt)
  {
    status = sns_ddf_read_port_u32_count( 
       state->port_handle,
       MPU6XXX_REG_FIFO_R_W, 
       &buf[total_bytes_read], 
       byte_cnt-total_bytes_read,
       &bytes_read);
    if(status != SNS_DDF_SUCCESS)
    {
      break;
    }
    total_bytes_read += bytes_read;
  }

  if (fifo_cnt != total_bytes_read)
  {
    MPU6050_MSG_3(ERROR, "read_fifo: fifo_cnt=%u byte_cnt=%d read=%d", 
                  fifo_cnt, byte_cnt, total_bytes_read);
  }
  return bytes_read;
}

/**
 * @brief Gets all sensor data from the device and stores it in the local cache.
 *
 * @param[in] state: Pointer to the device-instance structure
 * @param[in] map_axes: Map to the phone's axes if TRUE. Otherwise just cache raw data.
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e mpu6515_get_data_from_device(
   mpu6515_s* state,
   bool map_axes)
{
  uint8_t i;
  sns_ddf_status_e status;
  uint8_t bytes_read;
  int16_t device_data[MPU6050_CACHE__SIZE] = { 0 };
  uint8_t* buffer = (uint8_t*)device_data;
  q16_t sensitivity;
  mpu6515_sensor_s* sensor;
  mpu6515_sub_dev_s* sub_dev;
  uint8_t stepcnt_data[4] = { 0 };
  uint8_t mag_data[6] = { 0 };

  // Read the data
  status = sns_ddf_read_port( state->port_handle,
                              MPU6050_REG_ACCEL_XOUT_H, buffer,
                              MPU6050_CACHE_STEPCNT * 2,  // Read only the accel, temp, and gyro
                              &bytes_read);

  if (status != SNS_DDF_SUCCESS)
     return status;

  state->data_cache_time = sns_ddf_get_timestamp();

  // Combine high and low order bytes
  device_data[MPU6050_CACHE_ACCELX] = buffer[0] << 8  | buffer[1];
  device_data[MPU6050_CACHE_ACCELY] = buffer[2] << 8  | buffer[3];
  device_data[MPU6050_CACHE_ACCELZ] = buffer[4] << 8  | buffer[5];
  device_data[MPU6050_CACHE_TEMP]   = buffer[6] << 8  | buffer[7];
  device_data[MPU6050_CACHE_GYROX]  = buffer[8] << 8  | buffer[9];
  device_data[MPU6050_CACHE_GYROY]  = buffer[10] << 8 | buffer[11];
  device_data[MPU6050_CACHE_GYROZ]  = buffer[12] << 8 | buffer[13];

  // Check for 16 bit signed data out of bounds
  for (i = 0; i < MPU6050_CACHE__SIZE; i++)
  {
      state->data_status[i] =
          (device_data[i] == ((0x8000)) || device_data[i] == ((0x7FFF))) ?
          SNS_DDF_EDATA_BOUND : SNS_DDF_SUCCESS;
  }

  // Report accel data in mG (Q16 format)
  sensor = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &sub_dev);
  if (sensor != NULL)
  {
    sensitivity = mpu6515_accel_sensitivity[sensor->range];
    state->data_cache[MPU6050_CACHE_ACCELX] = device_data[MPU6050_CACHE_ACCELX] * sensitivity;
    state->data_cache[MPU6050_CACHE_ACCELY] = device_data[MPU6050_CACHE_ACCELY] * sensitivity;
    state->data_cache[MPU6050_CACHE_ACCELZ] = device_data[MPU6050_CACHE_ACCELZ] * sensitivity;

    if (map_axes)
      sns_ddf_map_axes(&sensor->axes_map, &state->data_cache[MPU6050_CACHE_ACCELX]);
  }

  // Report gyro data in radians/sec (Q16 format)
  sensor = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_GYRO, &sub_dev);
  if (sensor != NULL)
  {
    sensitivity = mpu6515_gyro_sensitivity[sensor->range];
    state->data_cache[MPU6050_CACHE_GYROX] = (device_data[MPU6050_CACHE_GYROX] * sensitivity) >> 9;
    state->data_cache[MPU6050_CACHE_GYROY] = (device_data[MPU6050_CACHE_GYROY] * sensitivity) >> 9;
    state->data_cache[MPU6050_CACHE_GYROZ] = (device_data[MPU6050_CACHE_GYROZ] * sensitivity) >> 9;

    if (map_axes)
      sns_ddf_map_axes(&sensor->axes_map, &state->data_cache[MPU6050_CACHE_GYROX]);
  }

  // Report temperature data in deg C (Q16 format)
  state->data_cache[MPU6050_CACHE_TEMP] =
      (device_data[MPU6050_CACHE_TEMP] - mpu6515_temp_offset_lsb) *
      mpu6515_temp_sensitivity + mpu6515_temp_offset_deg;

  if (state->hw_st_ptr != NULL)
  {
    return SNS_DDF_SUCCESS;
  }
  
  // Report step counter value, unit-less (raw value)
  sensor = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_STEP_COUNT, &sub_dev);
  if (sensor != NULL)
  {
    status = mpu6515_read_dmp_reg(state, D_PEDSTD_STEPCTR, stepcnt_data, sizeof(stepcnt_data));
    if(status != SNS_DDF_SUCCESS)  return status;

    state->data_cache[MPU6050_CACHE_STEPCNT] = DMP_CONV_S32(stepcnt_data);
    MPU6050_MSG_3(LOW, "stepcnt: val=%08x [0]=%02x [3]=%02x", state->data_cache[MPU6050_CACHE_STEPCNT], stepcnt_data[0], stepcnt_data[3]);
  }

  sensor = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_MAG, &sub_dev);
  if (sensor != NULL)
  {
    status = sns_ddf_read_port(state->port_handle, MPU6515_EXT_SENS_DATA_00, mag_data, sizeof(mag_data), &bytes_read);
    if(status != SNS_DDF_SUCCESS)  return status;

    state->data_cache[MPU6050_CACHE_MAGX] = DMP_CONV_S16(mag_data);
    state->data_cache[MPU6050_CACHE_MAGY] = DMP_CONV_S16(&mag_data[2]);
    state->data_cache[MPU6050_CACHE_MAGZ] = DMP_CONV_S16(&mag_data[4]);
    MPU6050_MSG_3(LOW, "mag x: val=%04x [0]=%02x [1]=%02x", state->data_cache[MPU6050_CACHE_MAGX], mag_data[0], mag_data[1]);
    MPU6050_MSG_3(LOW, "mag y: val=%04x [0]=%02x [1]=%02x", state->data_cache[MPU6050_CACHE_MAGY], mag_data[2], mag_data[3]);
    MPU6050_MSG_3(LOW, "mag z: val=%04x [0]=%02x [1]=%02x", state->data_cache[MPU6050_CACHE_MAGZ], mag_data[4], mag_data[5]);
  }
  return SNS_DDF_SUCCESS;
}


/**
 * @brief Called by the SMGR to get instantaneous data
 * @see sns_ddf_driver_if_s.get_data()
 */
sns_ddf_status_e mpu6515_get_data(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensors[],
   uint32_t               num_sensors,
   sns_ddf_memhandler_s*  memhandler,
   sns_ddf_sensor_data_s* data[])
{
  uint8_t i, j;
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s* data_iter;
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  uint8_t data_cache_idx = 0;

  if(state == NULL || sensors == NULL || memhandler == NULL || data == NULL)
      return SNS_DDF_EINVALID_PARAM;

  for(i = 0; i < num_sensors; i++)
  {
     switch(sensors[i]) {
       case SNS_DDF_SENSOR_ACCEL:
       case SNS_DDF_SENSOR_GYRO:
       case SNS_DDF_SENSOR_TEMP:
       case SNS_DDF_SENSOR_MAG:
       case SNS_DDF_SENSOR_PRESSURE:
       case SNS_DDF_SENSOR_STEP_COUNT:
         break;
       default:
         return SNS_DDF_EINVALID_PARAM;
     }
  }

  status = mpu6515_get_data_from_device(state, TRUE);
  if (status != SNS_DDF_SUCCESS)
     return status;

  *data = sns_ddf_memhandler_malloc(
      memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

  if(*data == NULL)
      return SNS_DDF_ENOMEM;

  for(i = 0; i < num_sensors; i++)
  {
     data_iter = &((*data)[i]);

     data_iter->sensor = sensors[i];
     data_iter->timestamp = state->data_cache_time;
     data_iter->status = SNS_DDF_SUCCESS;

     switch(sensors[i])
     {
       case SNS_DDF_SENSOR_ACCEL:
         data_iter->num_samples = 3;
         data_cache_idx = MPU6050_CACHE_ACCELX;
         break;
       case SNS_DDF_SENSOR_GYRO:
         data_iter->num_samples = 3;
         data_cache_idx = MPU6050_CACHE_GYROX;
            break;
       case SNS_DDF_SENSOR_TEMP:
         data_iter->num_samples = 1;
         data_cache_idx = MPU6050_CACHE_TEMP;
         break;
       case SNS_DDF_SENSOR_STEP_COUNT:    // Step Counter in DMP
         data_iter->num_samples = 1;
         data_cache_idx = MPU6050_CACHE_STEPCNT;
         break;
       default:
         return SNS_DDF_EINVALID_PARAM;
     }

     data_iter->samples = sns_ddf_memhandler_malloc(
         memhandler, sizeof(sns_ddf_sensor_sample_s) * data_iter->num_samples);

     if (data_iter->samples == NULL)
         return SNS_DDF_ENOMEM;

     if (EnableI2C == FALSE)
     {
         static uint32_t dummyData = 1;
         for (j = 0; j < data_iter->num_samples; j++)
         {
             state->data_cache[data_cache_idx + j] = dummyData++;
             state->data_status[data_cache_idx + j] = SNS_DDF_SUCCESS;
         }
     }

     for (j = 0; j < data_iter->num_samples; j++)
     {
         data_iter->samples[j].sample = state->data_cache[data_cache_idx + j];
         data_iter->samples[j].status = state->data_status[data_cache_idx + j];
     }

     mpu6515_log_data(state, sensors[i]);
  }

  return SNS_DDF_SUCCESS;
}

static sns_ddf_handle_t sns_dd_handle_to_smgr_handle(mpu6515_s* state, uint8_t dmp_sensor)
{
  int i;
  for (i = 0; i < state->sub_device_cnt; i++) {
   /// MPU6050_MSG_3(HIGH, "dd-smgr hndl: sub_dev_idx=%d ddf=%u dmp=%u", i, state->sub_devices[i].sensors[0].sensor_type, dmp_sensor);
   /// MPU6050_MSG_1(HIGH, "dd-smgr : smgr_handle=%u", state->sub_devices[i].smgr_handle);

    if (state->sub_devices[i].sensors[0].sensor_type == DMP_GET_DDF_SENSOR(dmp_sensor)) {
      return state->sub_devices[i].smgr_handle;
    }
  }
  return NULL;
}

static void mpu6515_consume_fifo_data(mpu6515_s* state, sns_ddf_time_t timestamp)
{
  uint8_t dmp_sensor;
  sns_ddf_handle_t smgr_handle = NULL;

  /* all samples for each data type are extracted and sent together */
  for (dmp_sensor=0; dmp_sensor<ARR_SIZE(parsed_fifo_info); dmp_sensor++)
  {
    mpu6515_sensor_s* sensor_ptr;
    parsed_fifo_info_s* fifo_info_ptr;
    uint8_t pkt, dim, dimension;
    mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(state);

    if (parsed_fifo_info[dmp_sensor].num_pkts == 0)
    {
      continue;
    }
    sensor_ptr = mpu6515_search_for_sensor(state, DMP_GET_DDF_SENSOR(dmp_sensor), &sub_dev);
    if (sensor_ptr == NULL)
    {
      MPU6050_MSG_2(HIGH, "Huh? dmp_sensor=%d #pkts=%d", 
                    dmp_sensor, parsed_fifo_info[dmp_sensor].num_pkts);
      continue;
    }
    fifo_info_ptr = &parsed_fifo_info[dmp_sensor];
    
    if(sub_dev!= NULL) 
    {
      //MPU6050_MSG_2(HIGH, "SNS_DDF_EVENT_FIFO_WM_INT dmp_sensor=%d sensor_type=%d", 
      //              dmp_sensor, sensor_ptr->sensor_type);
      sns_ddf_smgr_notify_event(sub_dev->smgr_handle, sensor_ptr->sensor_type, SNS_DDF_EVENT_FIFO_WM_INT);
    }

    /* common to all sensors */
    state->sensor_data.status = SNS_DDF_SUCCESS;
    state->sensor_data.sensor = DMP_GET_DDF_SENSOR(dmp_sensor);
    state->sensor_data.num_samples = 
      fifo_info_ptr->num_pkts * dmp_sensor_map[dmp_sensor].dim;
    if (fifo_info_ptr->num_pkts > 1)
    {
      state->sensor_data.timestamp = sensor_ptr->last_sample_timestamp +
        (timestamp - sensor_ptr->last_sample_timestamp)/ fifo_info_ptr->num_pkts;
    }
    else
    {
      state->sensor_data.timestamp = timestamp;
    }
    state->sensor_data.end_timestamp = timestamp;
    
    if (fifo_info_ptr->num_pkts > 1)
    {
      MPU6050_MSG_4(
         MED, "curr_thres=%d sensor=%d #pkts=%d #samples=%d", state->current_thres,
         sensor_ptr->sensor_type, fifo_info_ptr->num_pkts, 
         state->sensor_data.num_samples);
      MPU6050_MSG_3(
         LOW, "last_ts=%u ts=%u end_ts=%u", sensor_ptr->last_sample_timestamp,
         state->sensor_data.timestamp, state->sensor_data.end_timestamp);
    }

    /* sensor specific */
    dimension = dmp_sensor_map[dmp_sensor].dim;
    for (pkt=0; pkt<fifo_info_ptr->num_pkts; pkt++)
    {
      uint8_t* dev_fifo_ptr = &dev_fifo[fifo_info_ptr->pkt_start_idx[pkt]];
      for (dim=0; dim<dimension; dim++)
      {
        switch (dmp_sensor)
        {
        case DMP_SENSOR_6QUAT: // unit-less (Q16)
          {
            state->sensor_data.samples[pkt*dimension+dim].sample = 
              (q16_t) FX_CONV(DMP_CONV_S32(dev_fifo_ptr+(dim<<2)), 
                              DMP_QUAT_SCALE, FX_QFACTOR);
            break;
          }
        case DMP_SENSOR_RAW_ACCEL: // unit = m/s^2 (Q16)
          {
            q16_t sensitivity = mpu6515_accel_sensitivity[sensor_ptr->range];
            state->sensor_data.samples[pkt*dimension+dim].sample = 
              DMP_CONV_S16(dev_fifo_ptr+(dim<<1)) * sensitivity;
            break;
          }
        case DMP_SENSOR_RAW_GYRO: // unit = rad/sec (Q16)
          {
            q16_t sensitivity = mpu6515_gyro_sensitivity[sensor_ptr->range];
            state->sensor_data.samples[pkt*dimension+dim].sample = 
              (DMP_CONV_S16(dev_fifo_ptr+(dim<<1)) * sensitivity) >> 9;
            break;
          }
        case DMP_SENSOR_STEPDET:
          {
            state->data_cache_time = timestamp;
            if(DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPDET))
            {
              state->sensor_data.samples[0].sample = 1;
              mpu6515_log_data(state, DMP_GET_DDF_SENSOR(DMP_SENSOR_STEPDET));

              smgr_handle = sns_dd_handle_to_smgr_handle(state, DMP_SENSOR_STEPDET);
              if(smgr_handle != NULL)
              {
                sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
              }
            }
            if(DMP_IS_SENSOR_ENABLED(DMP_SENSOR_STEPCNT))
            {
              uint8_t stepcnt_data[4] = { 0 };
              if (SNS_DDF_SUCCESS == 
                  mpu6515_read_dmp_reg(state, D_PEDSTD_STEPCTR, 
                                       stepcnt_data, sizeof(stepcnt_data)))
              {
                state->sensor_data.sensor = DMP_GET_DDF_SENSOR(DMP_SENSOR_STEPCNT);
                state->sensor_data.samples[0].sample = DMP_CONV_S32(stepcnt_data);
                state->sensor_data.num_samples = 1;
                mpu6515_log_data(state, DMP_GET_DDF_SENSOR(DMP_SENSOR_STEPCNT));

                smgr_handle = sns_dd_handle_to_smgr_handle(state, DMP_SENSOR_STEPCNT);
                if(smgr_handle != NULL)
                {
                  sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
                }
              }
            }
            break;
          }
        case DMP_SENSOR_COMPASS:
          {
            sns_dd_mag_akm8963_state_s* akm8963_state_ptr =
              (sns_dd_mag_akm8963_state_s*)sensor_ptr->aux_dev.st_aux_dev;
            state->sensor_data.samples[pkt*dimension+dim].sample = 
              FX_FLTTOFIX_Q16(
                 AKM8963_RAW_TO_GAUSS_FLT(DMP_CONV_COMPASS(dev_fifo_ptr+(dim<<1)) * 
                                          akm8963_state_ptr->sens_adj[dim]));
            break;
          }
        case DMP_SENSOR_PRESSURE:
          {
            /* reference: see bmp280_read_up(BMP280_S32_t *upressure)
               in sns_dd_alt_bmp280.c */
            int32_t upressure = DMP_CONV_PRESSURE(dev_fifo_ptr);
            /* reference: see bmp280_read_ut(BMP280_S32_t *utemperature)
               in sns_dd_alt_bmp280.c */
            int32_t utemperature = DMP_CONV_PRESSURE(dev_fifo_ptr+3);
            /* Compensate the temperature before compensating the pressure */
            bmp280_compensate_T_int32(utemperature);
            /* reference: see bmp_get_pressure in sns_dd_alt_bmp280.c */
            uint32_t cpress = bmp280_compensate_P_int64(upressure)/256;
            state->sensor_data.samples[pkt*dimension+dim].sample = 
              cpress*bmp280_pressure_resolution;
            break;
          }
        default:
          MPU6050_MSG_1(FATAL, "skipping dmp_sensor %d", dmp_sensor);
          break;
        } /* switch on dmp_sensor */
      } /* for each dimension */
    } /* for each packet */

    switch (dmp_sensor)
    {
    case DMP_SENSOR_RAW_ACCEL:
    case DMP_SENSOR_RAW_GYRO:
    case DMP_SENSOR_COMPASS:
      sns_ddf_map_axes_multiple(&sensor_ptr->axes_map, &state->sensor_data);
      break;
    }
    if ((dmp_sensor != DMP_SENSOR_STEPDET) && (dmp_sensor != DMP_SENSOR_STEPCNT))
    {
      mpu6515_log_fifo_data(state, dmp_sensor);
      smgr_handle = sns_dd_handle_to_smgr_handle(state, dmp_sensor);
      if(smgr_handle != NULL)
      {
        sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
      }
    }
    sensor_ptr->last_sample_timestamp = timestamp;
  }
}


static void mpu6515_parse_fifo_data(
   mpu6515_s*     state,
   sns_ddf_time_t timestamp
)
{
  uint8_t dmp_sensor, dmp_pkt_size;
  uint16_t fifo_idx = 0;
  if(dev_fifo_len >= DMP_FIFO_SIZE)
  {
    MPU6050_MSG_1(FATAL, "!! DMP fifo is full. %d ",dev_fifo_len);
  }

  memset(parsed_fifo_info, 0, sizeof(parsed_fifo_info));
  while(fifo_idx < dev_fifo_len)
  {
    uint16_t pkt_hdr = DMP_GET_HDR(&dev_fifo[fifo_idx]);
    dmp_sensor = sns_dd_dmp_parse_hdr(pkt_hdr);
    if(dmp_sensor == DMP_SENSOR_INVALID)
    {
      MPU6050_MSG_2(FATAL, "!! unknown hdr=0x%04X fifo_idx=%u",
                    pkt_hdr, fifo_idx);
      fifo_idx++;
      continue;
    }
    dmp_pkt_size = dmp_sensor_map[dmp_sensor].hdr_size +
                   dmp_sensor_map[dmp_sensor].body_size;
    if(dmp_pkt_size > dev_fifo_len-fifo_idx)
    {
      MPU6050_MSG_2(ERROR, "!! incomplete pkt expected=%d avail=%d",
                    dmp_pkt_size, dev_fifo_len-fifo_idx);
      fifo_idx = dev_fifo_len;
      break;
    }
    if (parsed_fifo_info[dmp_sensor].num_pkts+1 >= 
        ARR_SIZE(parsed_fifo_info[dmp_sensor].pkt_start_idx))
    {
      MPU6050_MSG_2(FATAL, "!! %d packets for dmp_sensor %d",
                    parsed_fifo_info[dmp_sensor].num_pkts, dmp_sensor);
      fifo_idx++;
      continue;
    }
    parsed_fifo_info[dmp_sensor].pkt_start_idx[parsed_fifo_info[dmp_sensor].num_pkts++] = 
      fifo_idx + dmp_sensor_map[dmp_sensor].hdr_size;
    fifo_idx += dmp_pkt_size;
  }
  mpu6515_consume_fifo_data(state, timestamp);
  dev_fifo_len -= fifo_idx;
}

/**
 * @brief Called in response to an interrupt for this driver.
 * @see sns_ddf_driver_if_s.handle_irq()
 */
static void mpu6515_handle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  mpu6515_sub_dev_s* sub_dev = mpu6515_get_sub_dev(dd_handle);
  mpu6515_sensor_s* sensor;
  sns_ddf_status_e status;
  uint8_t buffer = 0;
  uint8_t read_count;

  if (NULL == state || NULL == sub_dev || state->device_id != DEVICE_ID_MPU6515)
  {
    return;
  }

  // Check which interrupts occurred.
  status = sns_ddf_read_port(
      state->port_handle, MPU6050_REG_INT_STATUS, &buffer, 1, &read_count);
  if(status != SNS_DDF_SUCCESS)
      return;

  // Motion detection
  if(buffer & MOT_EN_ENABLE)
  {
    sensor = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &sub_dev);
    if (NULL == sensor)
    {
      MPU6050_MSG_1(ERROR, "handle_irq: Cannot find the ACCEL sensor in state 0x%X", state);
      return;
    }

    if ( state->md_enabled )
    {
      status = mpu6515_disable_interrupt(state, sub_dev, sensor, MPU6050_INT_MOTION_DETECT);
      if(status != SNS_DDF_SUCCESS)  return;

      DMP_RESTORE_BITMAP(prev_dmp_sensor_bitmap);

      status = mpu6515_set_dmp_mode(state, 1);
      if(status != SNS_DDF_SUCCESS)  return;

      sns_ddf_smgr_notify_event(
                sub_dev->smgr_handle,
                SNS_DDF_SENSOR_ACCEL,
                SNS_DDF_EVENT_MOTION);
      MPU6050_MSG_0(HIGH, "handle_irq: Notified SMGR of a MOTION DETECT INT");
      /* make accel stream */
      state->md_enabled = FALSE;     /* this flag shall be cleared before calling set_odr() */
    }
  }

  // DMP data ready
  if(buffer & DMP_DATA_RDY_INT_ENABLED)
  {
    uint8_t buffer_dmp = 0, read_cnt_dmp;
    uint16_t bytes_read = 0;
    sns_ddf_handle_t my_smgr_handle;

    status = sns_ddf_read_port(
      state->port_handle, MPU6500_REG_DMP_INT_STATUS, &buffer_dmp, 1, &read_cnt_dmp);
    if(status != SNS_DDF_SUCCESS)
      return;

    if((buffer_dmp&0x08) || (buffer_dmp&0x04))
    {
      MPU6050_MSG_3(FATAL, "handle_irq: step=%u smd=%u fifo=%u", 
                    (buffer_dmp&0x08), (buffer_dmp&0x04), (buffer_dmp&0x01));
      if(buffer_dmp&0x04) {
        my_smgr_handle = sns_dd_handle_to_smgr_handle(state, DMP_SENSOR_SMD);
        state->sensor_data.status = SNS_DDF_SUCCESS;
        state->sensor_data.sensor = DMP_GET_DDF_SENSOR(DMP_SENSOR_SMD);
        state->sensor_data.num_samples = DMP_GET_DIM(DMP_SENSOR_SMD);
        state->sensor_data.timestamp = timestamp;
        state->sensor_data.samples[0].sample = 1;
        state->sensor_data.samples[0].status = SNS_DDF_SUCCESS;
        MPU6050_MSG_3(MED, "parsed: smd_ts=%u sensor=%u smgr_handle=%u",
                      state->sensor_data.samples[0].sample, state->sensor_data.sensor, my_smgr_handle);
        if(my_smgr_handle != NULL && DMP_IS_SENSOR_ENABLED(DMP_SENSOR_SMD)) {
          DMP_DISABLE_SENSOR(DMP_SENSOR_SMD);
          sns_ddf_smgr_notify_data(my_smgr_handle, &state->sensor_data, 1);
        }
      }
    }

    if (dev_fifo_len > 0)
    {
      MPU6050_MSG_1(HIGH, "handle_irq - %d bytes left over", dev_fifo_len);
    }
    bytes_read = sns_dd_dmp_read_fifo(state, DMP_FIFO_SIZE-dev_fifo_len, &dev_fifo[dev_fifo_len]);
    if(bytes_read > 0)
    {
      dev_fifo_len += bytes_read;
      mpu6515_parse_fifo_data(state, timestamp);
    }
  } // if(DMP interrupt)
}

static sns_ddf_status_e mpu6515_run_device_recovery(mpu6515_s *state)
{
  mpu6515_s *state_orig = &recovery_state;
  mpu6515_sub_dev_s *sub_dev;
  mpu6515_sensor_s  *sensor_ptr = NULL, *sensor_orig_ptr;
  sns_ddf_status_e  status = SNS_DDF_EFAIL;
  bool enable_dmp_flag;
  uint8_t i;
  uint32_t j;
  sns_ddf_handle_t sensor_dd_handle = NULL;

  if(recovery_in_progress) {
    return SNS_DDF_SUCCESS;
  }

  if(!recovery_in_progress) {
    recovery_in_progress = true;
  }

  MPU6050_MSG_0(HIGH, "*** Recovery: mechanism triggered ***");
  recovery_called++;

  /* Saves a copy to global */
  SNS_OS_MEMCOPY(state_orig, state, sizeof(mpu6515_s));

  /* Recovers the original state of MPU registers
   *
   * To simplify the recovery mechanism, DD
   * - always streams data according to the sensor bitmap
   *
   * This have some power impact (because it ignores the case
   * when MD interrupt is enabled when recovery takes place).
   * However, if DD handles properly, this should have negligible
   * effect.
   *
   * Sequence:
   *    - Hard-resets device
   *    - Clears internal state info
   *    - Loops through all sub-devices
   *      - Sets powerstate to active for active sensors
   *      - Sets ODR of sensors
   *      - Calls enable_sched_data
   *    - Sets FIFO watermark and enables DMP
   */
  if(mpu6515_reset_device(state) != SNS_DDF_SUCCESS)
  {
    MPU6050_MSG_0(FATAL, "*** Recovery: device reset FAILED ***");
    return SNS_DDF_EFAIL;
  }

  MPU6050_MSG_0(HIGH, "*** Recovery: device reset OK ***");
  if(state_orig->md_enabled)
  {
    sensor_ptr = mpu6515_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &sub_dev);
    if(sub_dev != NULL)
    {
      sns_ddf_smgr_notify_event(sub_dev->smgr_handle, SNS_DDF_SENSOR_ACCEL, SNS_DDF_EVENT_MOTION);
      MPU6050_MSG_0(HIGH, "*** Recovery: disabled MD interrupt ***");
    }
  }

  for(i = 0; i < state->sub_device_cnt; i++)
  {
    sub_dev = &state->sub_devices[i];
    sensor_dd_handle = (sns_ddf_handle_t) ((uint32_t) state | (i & 0xF));
    for (j = 0; j < sub_dev->num_sensors; j++)
    {
      if(sub_dev->sensors[j].sensor_type != SNS_DDF_SENSOR__NONE &&
         sub_dev->sensors[j].sensor_type != SNS_DDF_SENSOR__ALL)
      {
        sensor_ptr      = mpu6515_search_for_sensor(state,      sub_dev->sensors[j].sensor_type, NULL);
        sensor_orig_ptr = mpu6515_search_for_sensor(state_orig, sub_dev->sensors[j].sensor_type, NULL);
        if(sensor_ptr != NULL && sensor_orig_ptr != NULL)
        {
          sub_dev->sensors[j].desired_odr = sensor_orig_ptr->desired_odr;
          if(sub_dev->sensors[j].desired_odr > 0)
          {
            status = mpu6515_set_powerstate(state, i, SNS_DDF_POWERSTATE_ACTIVE);
            if(status != SNS_DDF_SUCCESS)  return status;

            status = mpu6515_set_odr(state, sensor_ptr, sub_dev->sensors[j].desired_odr);
            if(status != SNS_DDF_SUCCESS)  return status;

            enable_dmp_flag = true;
            status = mpu6515_enable_sched_data(sensor_dd_handle, sub_dev->sensors[j].sensor_type, enable_dmp_flag);
            if(status != SNS_DDF_SUCCESS)  return status;

            status = mpu6515_set_dmp_wm(state, sensor_ptr, sub_dev->sensors[j].watermark);
            if(status != SNS_DDF_SUCCESS)
            {
              MPU6050_MSG_1(FATAL, "*** Recovery: cannot enable dmp (err=%d) ***", status);
              return status;
            } 

            MPU6050_MSG_2(HIGH, "*** Recovery: ddf_sensor=%d odr=%d ***",
                          sub_dev->sensors[j].sensor_type, sub_dev->sensors[j].desired_odr);
          }
        }
        else
        {
          MPU6050_MSG_1(ERROR, "*** Recovery: sensor NULL ptr (ddf_sensor=%d) ***", sub_dev->sensors[j].sensor_type);
        }
      }
    }
  }

  if(sensor_ptr != NULL)
  {
    status = mpu6515_set_dmp_wm(state, sensor_ptr, sensor_ptr->watermark);
    if(status != SNS_DDF_SUCCESS)
    {
      MPU6050_MSG_1(FATAL, "*** Recovery: cannot enable dmp (err=%d) ***", status);
      return status;
    }

    MPU6050_MSG_0(HIGH, "*** Recovery: completed ***");
    recovery_OK++;
    recovery_in_progress = false;
    status = SNS_DDF_SUCCESS;
  }
  else
  {
    MPU6050_MSG_0(FATAL, "*** Recovery: failed: sensor_ptr == NULL ***");
    status = SNS_DDF_EFAIL;
  }

  return status;
}

/**
 * @brief Called when the timer set by this driver has expired. This must be
 *        the callback function submitted when initializing a timer.
 *
 * @see sns_ddf_driver_if_s.handle_timer()
 */
static void mpu6515_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  if (NULL == state)
    return;

  if(state->hw_st_ptr != NULL)
  {
    mpu6515_handle_st_timer(state);
  }
}


static sns_ddf_status_e mpu6515_probe(
  sns_ddf_device_access_s*  device_info,
  sns_ddf_memhandler_s*     memhandler,
  uint32_t*                 num_sensors,
  sns_ddf_sensor_e**        sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  mpu6515_device_id_e  dev_id;
  uint8_t i2c_buff = 0;
  uint8_t bytes_read;
  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  MPU6050_MSG_2(LOW, "probe: addr=0x%X bus=0x%X",
                  device_info->port_config.bus_config.i2c->slave_addr, device_info->port_config.bus_instance);

  status = sns_ddf_read_port( port_handle,
                              MPU6050_REG_PWR_MGMT_1,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != SLEEP_ENABLE )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }
  // Validate the WHO_AM_I register to see which device is being probed
  status = sns_ddf_read_port( port_handle,
                              MPU6050_REG_WHO_AM_I,
                              &i2c_buff,
                              1,
                              &bytes_read );
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }

  dev_id = (mpu6515_device_id_e) i2c_buff;
  if ( dev_id == DEVICE_ID_MPU6050 )
  {
    MPU6050_MSG_0(LOW, "MPU6050 found!");
    /* PWR MGMT and WHOAMI registers are correct. This is an MPU 6050 */
    *num_sensors = 3;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
      (*sensors)[2] = SNS_DDF_SENSOR_TEMP;
      status = SNS_DDF_SUCCESS;
    } else {
      status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
  }
  else if ( dev_id == DEVICE_ID_MPU6500 )
  {
    MPU6050_MSG_0(LOW, "MPU6500 found!!");
    /* PWR MGMT and WHOAMI registers are correct. This is an MPU6500 */
    *num_sensors = 5;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
      (*sensors)[2] = SNS_DDF_SENSOR_TEMP;
      (*sensors)[3] = SNS_DDF_SENSOR_DOUBLETAP;
      status = SNS_DDF_SUCCESS;
    } else {
      status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
  }
  else if( dev_id == DEVICE_ID_MPU6515 )
  {
    MPU6050_MSG_0(LOW, "Detected MPU6515");
    *num_sensors = 6;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * (*num_sensors) );
    if( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
      (*sensors)[2] = SNS_DDF_SENSOR_TEMP;
      (*sensors)[3] = SNS_DDF_SENSOR_STEP_EVENT;    // Step Det
      (*sensors)[4] = SNS_DDF_SENSOR_SMD;    //
      (*sensors)[5] = SNS_DDF_SENSOR_STEP_COUNT;
      status = SNS_DDF_SUCCESS;
    }
    else
    {
      status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
  }
  else
  {
    /* Unknown device ID. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }
}

/*
 Trigger_fifo_data function is used to read the specified amount of data from the dmp fifo.
 This function will flush fifo if the the num_packets is 0.
 The function will return immediately without doing anything if trigger_now is set to 0.
*/
static sns_ddf_status_e mpu6515_trigger_fifo_data(
                  sns_ddf_handle_t dd_handle,
                  sns_ddf_sensor_e ddf_sensor,
                  uint16_t num_packets,
                  bool trigger_now )
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  uint16_t bytes_read = 0;
  sns_ddf_time_t timestamp;
  int max_num_bytes = DMP_FIFO_SIZE;

  MPU6050_MSG_3(LOW, "trigger_fifo_data - ddf_sensor=%u #packets=%d now=%d", 
                ddf_sensor, num_packets, trigger_now);

  if (NULL == state)
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  if(!trigger_now)
  {
    return SNS_DDF_EINVALID_PARAM; /* not supported */
  }
#if 1
  if (num_packets != 0)
  {
    return SNS_DDF_EINVALID_PARAM; /* not supported */
  }
#else
  if (num_packets != 0)
  {
    uint8_t num_enabled_sensors = 0;
    uint8_t i;
    for (i=0; i<16; i++) 
    {
      uint16_t bit = 1 << i;
      if (dmp_sensor_bitmap & bit) 
      {
        num_enabled_sensors++;
      }
    }
    if (num_enabled_sensors != 1)
    {
      /* cannot read selectively from shared DMP FIFO */
      return SNS_DDF_EINVALID_PARAM;
    }
    else
    {
      /* should be OK to read only num_packets samples for the enabled sensor */
      uint8_t dmp_sensor = sns_dd_dmp_get_sensor(ddf_sensor);
      uint8_t pkt_size = DMP_GET_PKT_SIZE(dmp_sensor);
      if (pkt_size != 0)
      {
        max_num_bytes = pkt_size * num_packets;
      }
      else
      {
        return SNS_DDF_EINVALID_PARAM;
      }
    }
  }
#endif
  timestamp = sns_ddf_get_timestamp();
  bytes_read = sns_dd_dmp_read_fifo(state, max_num_bytes-dev_fifo_len, 
                                    &dev_fifo[dev_fifo_len]);
  if(bytes_read > 0)
  {
    dev_fifo_len += bytes_read;
    MPU6050_MSG_1(LOW, "trigger_fifo_data - %d bytes read from FIFO", bytes_read);
    mpu6515_parse_fifo_data(state, timestamp);
  }
  return SNS_DDF_SUCCESS;
}


// Function driver API
sns_ddf_driver_if_s sns_dd_mpu6515_if =
{
  .init         = &mpu6515_init,
  .get_data     = &mpu6515_get_data,
  .set_attrib   = &mpu6515_set_attr,
  .get_attrib   = &mpu6515_get_attr,
  .handle_timer = &mpu6515_handle_timer,
  .handle_irq   = &mpu6515_handle_irq,
  .reset        = &mpu6515_reset,
  .run_test     = &mpu6515_run_test,
  .enable_sched_data = &mpu6515_enable_sched_data,
  .probe             = &mpu6515_probe,
  .trigger_fifo_data = &mpu6515_trigger_fifo_data
};
