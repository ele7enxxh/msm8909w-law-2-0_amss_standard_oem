/********************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
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
*******************************************************************************/
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
*******************************************************************************/
/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R

DESCRIPTION

  Private header file for TMG399x driver

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
--------     ---     ------------------------------------------------------
07/09/15     AH      Fix compiler warnings
09/17/14     ps      Remove AVS component dependency
02/15/14     fv      Convert tmd37x2 driver to tmg399x
11/14/13     fv      Convert tmd377x driver to tmd37x2
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
11/14/12     fv      Modified refrence driver to use ams/taos sensor
==========================================================================*/

#ifndef _SNSDALSPRXPRIV_H
#define _SNSDALSPRXPRIV_H

#include "fixed_point.h"

#include "sns_ddf_util.h"
#include "sns_debug_str.h"

//
// AMS-TAOS USA ++++++++++++++++ defs ++++++++++++++++
//


// Define if using AMS GESTURE
//#define USE_AMS_GESTURE

// Enable this macro for HD22 build
//#define USING_OLD_QDSP6

#ifdef USING_OLD_QDSP6

  // Set AMS_INIT_FIRST to the set of sensors to init first
  // Then set AMS_INIT_SECOND to the second set etc.
  #define AMS_INIT_PRX_ALS  1
  #define AMS_INIT_RGB_CT   2
  #define AMS_INIT_GESTURE  3

  #define AMS_INIT_FIRST    AMS_INIT_RGB_CT
  #define AMS_INIT_SECOND   AMS_INIT_PRX_ALS
  #define AMS_INIT_THIRD

  #if(AMS_INIT_FIRST == AMS_INIT_GESTURE)
    #define SNS_DDF_SENSOR_IR_GESTURE SNS_DDF_SENSOR_MAG
    #define SNS_DDF_SENSOR_RGB        SNS_DDF_SENSOR_ACCEL
  #elif(AMS_INIT_FIRST == AMS_INIT_RGB_CT)
    #define SNS_DDF_SENSOR_RGB        SNS_DDF_SENSOR_MAG
    #define SNS_DDF_SENSOR_IR_GESTURE SNS_DDF_SENSOR_ACCEL
  #endif

  #define SNS_DDF_SENSOR_CT_C       SNS_DDF_SENSOR_TEMP

#endif

#define SENSOR_AMS_TMG3992  0
#define SENSOR_AMS_TMG3993  1

#define ams_magic_num 50
#define ams_driver_ID 1

#define WHICH_AMS_SENSOR    SENSOR_AMS_TMG3993

#define AMS_SENSOR          SENSOR_AMS_TMG3993

#define REG_FIXED_NV_SIZE_128      128

#define MAX_NUM_OF_SENSORS    5

#define MAX_CYCLE_TIME_ms    (45)  // 50.0
#define ALS_INTEG_TIME_ms    (25)
#if(MAX_CYCLE_TIME_ms < ALS_INTEG_TIME_ms)
  #error MAX CYCLE TIME _must_ be greater than ALS INTEG TIME
#endif
/*
 *  +-------------+      +
 *  | integration | wait |  x N
 *  | time        | time | o o o
 *--+             +------+
 *  ^                    ^
 *  |<- Max Cycle time ->|
 *  |<- ODR (MAX_CYCLE_TIME_ms * N) ->|
 *
 * integration time: 1 .. 256 integration cycles of 2.78 ms.
 * wait time: 0 .. 256, used to reduse current consumption, when WEN
 *            is enabled.
 *
 */

//#define TMG399x_DD_DEBUG
//#define USE_DD_I2C_DB


#include <stdbool.h>

/*
 * AMS-TAOS definitions and structures
 */

#define getWord(buf, addr) ((buf[addr+1] << 8) + buf[addr])

#define ROUND_SHFT_VAL   4
#define ROUND_ADD_VAL    (1 << (ROUND_SHFT_VAL - 1))

typedef enum {
    IRQ_TYPE,
    TIMER_TYPE
} AMS_SIG_TYPE;


typedef union {
    uint16_t   word;
    uint8_t    byte[2];
} uword_u;

typedef union {
    uint16_t   val;
    uint8_t    byte[2];
} uint16_u;

typedef union {
    uint32_t   val;
    uint16_t   word[2];
    uint8_t    byte[4];
} uint32_u;

/* proximity data */
typedef struct  {
    uint16_t pdata;
    int16_t  event;
    /* sns_ddf_timer_s timer_obj; */
} ams_prox_info;

typedef struct {
    uint32_t atime_us;               // in uSec
    uint8_t  again;
    uint32_t luxCorrection;
    uint32_t odr;
    bool     enabled;
} ams_als_setup;


typedef struct {
    uint8_t  pgain;
    uint32_t ptime_us;               // in uSec
    uint16_t pulse_count;
    uint16_t max_samples_cal;   /* for calibration mode*/
    uint16_t detect;
    uint16_t release;
    uint32_t odr;
    bool     enabled;
} ams_prox_setup;


typedef struct {
    uint32_t odr;
    bool     enabled;
} ams_gesture_setup;


typedef struct {
    uint32_t wtime_us;      // in uSec
    uint8_t  wtime;
    uint8_t  lTime;
    uint8_t  wen;
    bool     lTimeChanged;
} ams_wait;


typedef struct {
    uint8_t  enable;
    uint8_t  config;
    uint8_t  control;
} ams_reg;

typedef struct  {
    ams_als_setup     als;
    ams_als_setup     rgb;
    ams_als_setup     ct_c;
    ams_prox_setup    prox;
    ams_gesture_setup gesture;

    uint8_t           persistence;
    uint32_t          wait_time_us;

    ams_reg           reg;
    ams_wait          wait;
} ams_setup;


typedef struct {
    uint32_t cpl;
    uint32_t saturation;
    uint16_t clear_raw;
    uint16_t red_raw;
    uint16_t green_raw;
    uint16_t blue_raw;
    q16_t    clear_irradiance;
    q16_t    red_irradiance;
    q16_t    green_irradiance;
    q16_t    blue_irradiance;
    uint32_t lux;
    uint32_t cct;
    int16_t  ir;
    uint8_t  atime;
    uint8_t  again;
    uint32_t luxAve;
    uint16_t accuracy;
} ams_als_info;



#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//
// AMS-TAOS USA ---------------- defs ----------------
//

/*=======================================================================

                  Macros

========================================================================*/
/* Negative ADC counts will be treated as zero */
#define ALSPRX_CONV_TO_UNSIGNED(var, bits) ((var & (1<<(bits-1))) ? (0) : (var))

/*=======================================================================

                  TYPE DEFINITIONS

========================================================================*/
typedef enum
{
  SNS_DD_PEND_STATE_IDLE,                     /* idle state */
  SNS_DD_PEND_STATE_PEND,                     /* waiting a response */
  SNS_DD_PEND_STATE_RSVD                      /* reserved */
} sns_dd_pend_state_type;

typedef enum
{
  SNS_PRX_FAR_AWAY,
  SNS_PRX_NEAR_BY,
  SNS_PRX_NEAR_BY_UNKNOWN
} sns_dd_prx_nearby_type;

/* The state of a device */
typedef  enum
{
  SNS_DD_DEV_STOPPED,                         /* 0 */
  SNS_DD_DEV_CONFIGURED,                      /* 1 */
  SNS_DD_DEV_GET_DATA,                        /* 2 */
  SNS_DD_DEV_MAX_STATES                       /* 3 */
}sns_dd_device_state_type;

typedef enum
{
  SNS_DD_ALSPRX_NV_SOURCE_DEFAULT,
  SNS_DD_ALSPRX_NV_SOURCE_REG
}sns_dd_nv_source;

/* data structure for light(ALS) driver */
typedef struct
{
  int32_t                   als_data_cache;
  sns_dd_pend_state_type    pend_state;           /* idle or wait state for the response */
  uint8_t                   next_rng_sel;         /* range value that should be used for next sampling */
  uint8_t                   req_rng_sel;          /* Configuratin during the adaptive range selection */
  uint32_t                  last_get_time;
  uint32_t                  last_mlux;
  uint16_t                  accuracy;
  uint32_t                  als_conversion_lo;
  uint32_t                  als_conversion_hi;
  uint32_t                  als_req_cnt;      /* for history */
} sns_dd_als_db_type;

/* data structure for proximitydistance driver */
typedef struct
{
  int32_t                   prxdist_data_cache;
  sns_dd_pend_state_type    pend_state;                /* idle of wait the response */
  int32_t                   prx_data_cache;
  sns_dd_prx_nearby_type    last_nearby;
  uint16_t                  accuracy;
  uint32_t                  prx_req_cnt;      /* for history */
} sns_dd_prx_db_type;

/* data structure for NV items */
typedef struct
{
    uint8_t  visible_ratio;   // visible light transmission ratio of the glass/pipe in %
    uint8_t  ir_ratio;        // IR transmission ratio of light glass/pipe in %
    uint16_t dc_offset;       // DC offset in ADC count
    uint16_t thresh_near;     // near detection threshold in ADC count adjusted by DC offset
    uint16_t thresh_far;      // far detection threshold in ADC count adjusted by DC offset
    uint16_t prx_factor;      // PRX multiplicative factor in %
    uint16_t als_factor;      // ALS multiplicative factor in %

    // All vars below this, MUST be 32 bits
    uint32_t  ams_num;        // unique number for AMS
    uint32_t  drv_ID;         // identify specific driver
    uint32_t  calibratePhone; // if true, calibrate phone (should only have to done during manufacture)
    uint32_t device;          // current sensor
    uint32_t calLuxLower;     // ALS value of calibration light lower
    uint32_t calLuxUpper;     // ALS value of calibration light upper
    uint32_t d_factor;        // ALS coeficients to use for current sensor  // Changed name
    uint32_t r_coef;          // ALS coeficients to use for current sensor  // Changed name
    uint32_t g_coef;          // ALS coeficients to use for current sensor  // Changed name
    uint32_t b_coef;          // ALS coeficients to use for current sensor  // Changed name
    uint32_t ct_coef;         // ALS coeficients to use for current sensor  // New value
    uint32_t ct_offset;       // ALS coeficients to use for current sensor  // New value
    uint32_t proxDetect;      // Proximity Detect count
    uint32_t proxRelease;     // Proximity Release count
    uint32_t luxCorrection;   // Lux correction factor
} sns_dd_nv_db_type;  // sizeof(sns_dd_nv_db_type) = 60

typedef struct
{
  sns_ddf_powerstate_e      pwr_mode;
  sns_dd_device_state_type  state;
} sns_dd_alsprx_common_db_type;



//
//++ TMG399x ++
//
#define min(a, b)  (((a) < (b)) ? (a) : (b))

#define REG_TO_TIME_uS(rVal, minIntegTimeuS) ((256 - rVal) * minIntegTimeuS)



/*
enum ERROR_CODES {
    EINVAL,
    EIO,
    EAGAIN,
};*/


enum state_machine_sensors_e {
    AMS_SENSOR_ALS,         // 0
    AMS_SENSOR_PROX,        // 1
    AMS_SENSOR_GESTURE,     // 2
    AMS_SENSOR_RGB,         // 3
    AMS_SENSOR_CT,          // 4
    AMS_SENSOR_NONE,        // 5
};


enum state_machine_states_e {
    AMS_STATE_OFF,          // 0
    AMS_STATE_ALS,          // 1
    AMS_STATE_PRX,          // 2
    AMS_STATE_ALS_PRX,      // 3
    AMS_STATE_GES,          // 4
    AMS_STATE_GES_ALS,      // 5
    AMS_STATE_GES_PRX,      // 6
    AMS_STATE_GES_ALS_PRX,  // 7
};


enum tmg399x_ctrl_reg {
    AMS_Tmg399x_CTRL_AGAIN_1        = (0 << 0),
    AMS_Tmg399x_CTRL_AGAIN_4        = (1 << 0),
    AMS_Tmg399x_CTRL_AGAIN_16       = (2 << 0),
    AMS_Tmg399x_CTRL_AGAIN_60       = (3 << 0),
    AMS_Tmg399x_CTRL_AGAIN_MASK     = (3 << 0),

    AMS_Tmg399x_CTRL_PDRIVE_100MA   = (0 << 6),
    AMS_Tmg399x_CTRL_PDRIVE_50MA    = (1 << 6),
    AMS_Tmg399x_CTRL_PDRIVE_25MA    = (2 << 6),
    AMS_Tmg399x_CTRL_PDRIVE_12MA    = (3 << 6),
    AMS_Tmg399x_CTRL_PDRIVE_MASK    = (3 << 6),
};


enum tmg399x_cmd_reg {
    AMS_Tmg399x_CMD_REG           = (1 << 7),
    AMS_Tmg399x_CMD_INCR          = (0x1 << 5),
    AMS_Tmg399x_CMD_SPL_FN        = (0x3 << 5),
    AMS_Tmg399x_CMD_PROX_INT_CLR  = (0x5 << 0),
    AMS_Tmg399x_CMD_ALS_INT_CLR   = (0x6 << 0),
};


enum tmg399x_en_reg {
    AMS_Tmg399x_EN_PWR_ON   = (1 << 0),
    AMS_Tmg399x_EN_ALS      = (1 << 1),
    AMS_Tmg399x_EN_PRX      = (1 << 2),
    AMS_Tmg399x_EN_WAIT     = (1 << 3),
    AMS_Tmg399x_EN_ALS_IRQ  = (1 << 4),
    AMS_Tmg399x_EN_PRX_IRQ  = (1 << 5),
    AMS_Tmg399x_EN_SAI      = (1 << 6),
};


enum tmg399x_status {
    AMS_Tmg399x_ST_ALS_VALID  = (1 << 0),
    AMS_Tmg399x_ST_PRX_VALID  = (1 << 1),
    AMS_Tmg399x_ST_ALS_IRQ    = (1 << 4),
    AMS_Tmg399x_ST_PRX_IRQ    = (1 << 5),
    AMS_Tmg399x_ST_PRX_SAT    = (1 << 6),
};


enum {
    AMS_Tmg399x_ALS_GAIN_MASK    = (3 << 0),
    AMS_Tmg399x_ALS_AGL_MASK     = (1 << 2),
    AMS_Tmg399x_ALS_AGL_SHIFT    =        2,
    AMS_Tmg399x_ATIME_PER_100    =      273,
    AMS_Tmg399x_ATIME_DEFAULT_MS =       50,
    SCALE_SHIFT                  =       11,
    RATIO_SHIFT                  =       10,
    MAX_ALS_VALUE                =   0xffff,
    MIN_ALS_VALUE                =       10,
    GAIN_SWITCH_LEVEL            =      100,
    GAIN_AUTO_INIT_VALUE         = AMS_Tmg399x_CTRL_AGAIN_16,
};


typedef struct {
    uint16_t prox_th_min;
    uint16_t prox_th_max;
    uint16_t als_deltaP;
    uint16_t als_gain;
} tmg399x_parameters;


typedef struct {
    uint8_t   val[AMS_MAX_NUM_OF_REGS];
    uint8_t   changed[AMS_MAX_NUM_OF_REGS];
} shadow_type;


//struct tmg399x_chip {
typedef struct  {
    tmg399x_parameters params;

    bool               als_gain_auto;
    uint8_t            device_index;

    // LEGACY
    ams_setup          setup;

    uint32_t           Min_Integ_Time_us;

    //FV
    uint8_t            numOfSensors;
    sns_ddf_sensor_e   sensors[SNS_DD_AMS_NUM_SENSOR_TYPES];

    uint8_t            clrInterrupts;

    ams_prox_info      prx_info;
    ams_als_info       als_info;

    shadow_type        shadow;
    lux_segment        lux_coef[AMS_MAX_NUM_OF_399x_DEVICES];
    //FV
} tmg399x_chip;
//};
//
//-- TMG399x --
//


// New for combo driver


#define DD_HANDLE_ALIGN     3

/* Constants */
enum
    {
        DD_NUM_SUB_DEV             = 3,
        DD_NUM_SENSORS_PER_SUB_DEV = 3,
    };

/* Register definitions */

/* Basic definitions */

typedef struct {
    bool                           dri_enable;
    uint16_t                       interrupt_gpio;
    sns_ddf_handle_t               smgr_handle;  /* notify_data handle */
    uint32_t                       odr;
    sns_ddf_powerstate_e           powerstate;
    sns_ddf_timer_s                timer_obj[2];
    uint32_t                       num_sensors;
    sns_ddf_sensor_e               sensors[DD_NUM_SENSORS_PER_SUB_DEV];
    sns_ddf_sensor_e               timer_arg[2];
} sns_dd_sub_dev_t;


typedef struct
{
    sns_ddf_dev_list_t             next;     /* the next device instance */
    sns_ddf_driver_info_s          dd_info;  /* the driver info */
} sns_dd_head_s;


typedef struct {
    sns_ddf_port_id_s              port_id;
    sns_ddf_handle_t               port_handle;  /* I2C bus handle */
    sns_ddf_dev_list_t             next;
    sns_dd_sub_dev_t               sub_dev[DD_NUM_SUB_DEV];
    uint32_t                       sub_dev_count;
    sns_ddf_sensor_data_s*         s_data;
    sns_ddf_sensor_sample_s*       samples;
    bool                           sub_dev_0_enable;
    bool                           sub_dev_1_enable;

    // Legacy:
    uint32_t                     dropped_logs;

    sns_dd_als_db_type           sns_dd_als_db;
    sns_dd_prx_db_type           sns_dd_prx_db;
    sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
    sns_ddf_timer_s              timer;
    uint8_t                      num_requests;
    bool                         two_requests;
    bool                         calibration_mode;    /* calibration mode enable flag */

    sns_dd_nv_db_type            nv_db;                  /* NV item DB type */
    sns_dd_nv_source             nv_source;
    uint16_t                     nv_db_size;   /* size of NV item */

    tmg399x_chip                 chip;
    /* sns_ddf_handle_t             ZBW_port_handle; */
    uint32_t                     int_gpio;
} sns_dd_state_t;


#ifdef TMG399x_DD_DEBUG
  #define DBG_MEDIUM_PRIO DBG_MED_PRIO
  #define DD_MSG_0(level,msg)          MSG(MSG_SSID_SNS,   DBG_##level##_PRIO, "DD - " msg)
  #define DD_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS, DBG_##level##_PRIO, "DD - " msg,p1)
  #define DD_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS, DBG_##level##_PRIO, "DD - " msg,p1,p2)
  #define DD_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS, DBG_##level##_PRIO, "DD - " msg,p1,p2,p3)
#else /* TMG399x_DD_DEBUG */
  #define DD_MSG_0(level,msg)
  #define DD_MSG_1(level,msg,p1)
  #define DD_MSG_2(level,msg,p1,p2)
  #define DD_MSG_3(level,msg,p1,p2,p3)
#endif /* TMG399x_DD_DEBUG */

sns_ddf_status_e sns_dd_ams_tmg399x_init
(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
);

sns_ddf_status_e sns_dd_ams_tmg399x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
);

sns_ddf_status_e sns_dd_ams_tmg399x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

sns_ddf_status_e sns_dd_ams_tmg399x_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

void sns_dd_ams_tmg399x_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

void sns_dd_ams_tmg399x_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_ams_tmg399x_reset
(
  sns_ddf_handle_t dd_handle
);


sns_ddf_status_e sns_dd_ams_tmg399x_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);


sns_ddf_status_e sns_dd_ams_tmg399x_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );

sns_ddf_status_e sns_dd_read_registry
(
 sns_dd_state_t*      state,
 uint32_t             sub_dev_id,
 sns_ddf_nv_params_s* reg_params
 );

sns_ddf_status_e sns_dd_ams_tmg399x_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
 );

sns_ddf_status_e ams_get_lux
(
 sns_dd_state_t* state
);

void ams_calc_cpl
(
 tmg399x_chip *chip
);

sns_ddf_status_e ams_get_als_data
(
 sns_dd_state_t* state
);

sns_ddf_status_e ams_get_prox_data
(
 sns_dd_state_t* state
);

sns_ddf_status_e ams_readDataRegisters
(
 sns_dd_state_t* state
);

sns_ddf_status_e sns_dd_ams_tmg399x__alsprx_prx_binary
(
  sns_dd_state_t* state
);

void ams_update_als_thresh
(
 sns_dd_state_t* state,
 uint16_t        als_low,
 uint16_t        als_high,
 uint8_t         apers
);

#ifdef USE_DD_I2C_DB
  #define DD_I2C_DB_2(p1, p2)  {                        \
    uint8_t  p[4];                                      \
    uint16_u p3;                                        \
    p3.val = p1;                                        \
    p[0] = p3.byte[1];                                  \
    p[1] = p3.byte[0];                                  \
    p[2] = p2;                                          \
    ams_i2c_write_buf_db(state, 3, &p[0]);}

  #define DD_I2C_DB_3(p1, p3, p2)  {            \
    uint8_t  p[9];                              \
    uint16_u pa;                                \
    uint32_u pb;                                \
    pa.val = p1;                                \
    pb.val = p2;                                \
    p[0] = pa.byte[1];                          \
    p[1] = pa.byte[0];                          \
    p[2] = p3;                                  \
    p[3] = pb.byte[3];                          \
    p[4] = pb.byte[2];                          \
    p[5] = pb.byte[1];                          \
    p[6] = pb.byte[0];                          \
    ams_i2c_write_buf_db(state, 7, &p[0]);}
#else
  #define DD_I2C_DB_2(p1, p2)
  #define DD_I2C_DB_3(p1, p2, p3)

  #define DD_I2C_DB_2_sp(p1, p2)  {                        \
    uint8_t  p[4];                                      \
    uint16_u p3;                                        \
    p3.val = p1;                                        \
    p[0] = p3.byte[1];                                  \
    p[1] = p3.byte[0];                                  \
    p[2] = p2;                                          \
    ams_i2c_write_buf_db(state, 3, &p[0]);}

  #define DD_I2C_DB_3_sp(p1, p3, p2)  {            \
    uint8_t  p[9];                              \
    uint16_u pa;                                \
    uint32_u pb;                                \
    pa.val = p1;                                \
    pb.val = p2;                                \
    p[0] = pa.byte[1];                          \
    p[1] = pa.byte[0];                          \
    p[2] = p3;                                  \
    p[3] = pb.byte[3];                          \
    p[4] = pb.byte[2];                          \
    p[5] = pb.byte[1];                          \
    p[6] = pb.byte[0];                          \
    ams_i2c_write_buf_db(state, 7, &p[0]);}
#endif



#endif /* End include guard  SNSDALSPRXPRIV_H */
