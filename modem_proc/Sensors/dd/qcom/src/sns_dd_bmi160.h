/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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

#ifndef _SNS_DD_BMI160_H
#define _SNS_DD_BMI160_H


#include "sns_dd_bmi160_common.h"
/*=======================================================================

  compile-time configurations
  all configurations starts with BMI160_CONFIG_

  ========================================================================*/









//<feature_switches> can only be 1 or 0
//define this as 0 if you are not running the driver on Dragonboard (OSSC)
#ifndef BMI160_CONFIG_RUN_ON_OSSC
#define BMI160_CONFIG_RUN_ON_OSSC               0
#endif

//obselete, always 0
#define BMI160_CONFIG_RUN_ON_OSSC_V1            0
//define this as 1 if your Dragonboard is 64bit
#define BMI160_CONFIG_RUN_ON_OSSC_64            0


//to enable debug messages:
//BMI160_CONFIG_ENABLE_DBG_MSG needs to be 1
//also, BMI160_CONFIG_DBG_LEVEL should be set to a proper level

#ifndef BMI160_CONFIG_ENABLE_DBG_MSG
#define BMI160_CONFIG_ENABLE_DBG_MSG            0
#endif

#ifndef BMI160_CONFIG_DBG_LEVEL
//#define BMI160_CONFIG_DBG_LEVEL               0xffffffff              //uncomment this line if all logs need to be supressed
//#define BMI160_CONFIG_DBG_LEVEL                 DBG_HIGH_PRIO         //only messages with DBG_HIGH_PRIO and above will be logged
#define BMI160_CONFIG_DBG_LEVEL                 DBG_MED_PRIO            //only messages with DBG_MED_PRIO and above will be logged
//#define BMI160_CONFIG_DBG_LEVEL                 DBG_ERROR_PRIO            //only messages with DBG_ERROR_PRIO and above will be logged
#endif


//enable this if it is run on 8X26 which needs special handling for log packets
#ifndef BMI160_FOR_8X26_BUILD
#define BMI160_FOR_8X26_BUILD                   0
#endif

//(for Qualcomm internal use only) enable this if there is a PIN conflict with BMI055 on the board
#ifndef FEATURE_BMI160_INT_PIN_CONFLICT_RESOLVE
#define FEATURE_BMI160_INT_PIN_CONFLICT_RESOLVE 0
#endif

//enable this if uimage is supported and required,
//if you need full run-time logs from the driver disable it temporarily for trouble-shooting
//and re-enable it after everything is stable enough for testing
#ifndef BMI160_CONFIG_ENABLE_UIMAGE
#define BMI160_CONFIG_ENABLE_UIMAGE             1
#endif


//enable this if the magnetic sensor is connected to the slave interface of BMI160
#ifndef BMI160_CONFIG_ENABLE_MAG_IF
#define BMI160_CONFIG_ENABLE_MAG_IF             0
#endif

//enalbe this if there is any problem with data streaming
#ifndef BMI160_CONFIG_ENABLE_DUMP_CORE_REG
#define BMI160_CONFIG_ENABLE_DUMP_CORE_REG      0
#endif

//enable this if your platform supports the API sns_ddf_comm_bus_spi_set_clk_freq_hz()
#ifndef BMI160_CONFIG_DDF_BUS_SPI_ENABLE
#define BMI160_CONFIG_DDF_BUS_SPI_ENABLE        1
#endif

//the default clock rate of SPI bus
#define BMI160_CONFIG_SPI_CLOCK_RATE_DFT        (2*1000*1000)
//the min clock rate of SPI required by BMI160
#define BMI160_CONFIG_SPI_CLOCK_MIN_HZ          (1*1000*1000)
//the max clock rate of SPI required by BMI160
#define BMI160_CONFIG_SPI_CLOCK_MAX_HZ          (9500*1000)

//the default clock rate of I2C
#define BMI160_CONFIG_I2C_CLOCK_RATE_DFT        (380000)

//enable this if FIFO support is required
#define BMI160_CONFIG_ENABLE_FIFO               1

/* implement and test the following features in descending priority */

//enable this if motion-detect support is required
#define BMI160_CONFIG_ENABLE_MD                 1
//enable this if low-power motion-detect support is required
#define BMI160_CONFIG_ENABLE_MD_LP              1
//enable this if automatic transition of low-power motion-detect support is required
#define BMI160_CONFIG_ENABLE_MD_LP_AUTO         1

//enable this if low-power motion-detect mode 1 support is required
#define BMI160_CONFIG_ENABLE_MD_LP_MODE1        1
//enable this if low-power motion-detect mode 2 support is required
#define BMI160_CONFIG_ENABLE_MD_LP_MODE2        0


//enable this if double tap sensor support is required
#ifndef BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP
#define BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP     1
#endif

//enable this if adjustment of timestamp by filter delay is required
#ifndef BMI160_CONFIG_ENABLE_TS_ADJUST_FILTER
#define BMI160_CONFIG_ENABLE_TS_ADJUST_FILTER    1
#endif

//enable this if the framework supports this attribute and filter delay adjustment is applied by framework
#ifndef BMI160_CONFIG_ENABLE_ATTRIB_FILTER_DELAY
#define BMI160_CONFIG_ENABLE_ATTRIB_FILTER_DELAY 1
#endif

//enable this if self test support is required
#define BMI160_CONFIG_ENABLE_SELFTEST           1

//enable this if temperature compensation for TCO of accelerometer is required
#ifndef BMI160_CONFIG_ENABLE_ACC_TCOC
#define BMI160_CONFIG_ENABLE_ACC_TCOC           0
#endif

//if enabled, WML request will be adjusted taking (bus) delays into considerations
#define BMI160_CONFIG_ENABLE_FF_WML_ADJUST      1

//if enabled, when more frames are read out than the WML,
//extra frames are buffered and not reported immediately
#define BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES 0

//if enabled, driver will compensate for the situation when frame count get is a little bit less than WML
#ifndef BMI160_CONFIG_ENABLE_WA_FF_FRAMES_UNDERRUN
#define BMI160_CONFIG_ENABLE_WA_FF_FRAMES_UNDERRUN  1
#endif

//this is the number of frames less than WML which is considered as underrun condition
#define BMI160_CONFIG_FF_UNDERRUN_CNT           1


//no need to change those
#define BMI160_CONFIG_FIFO_WML_CAL_ALGO_LDT     1
#define BMI160_CONFIG_FIFO_WML_CAL_ALGO_LTT     0

//if enabled, a gross grained profiling of time spent on DD interfaces will be printed out
#define BMI160_CONFIG_ENABLE_PROFILE_IF_TIME    0


//if enabled, the interrupt from this device is level-triggered
#define BMI160_CONFIG_WMI_IRQ_TRIGGER_TYPE_LVL  0

//if enabled, data log packets (14c1) delivery will be supported
#define BMI160_CONFIG_ENABLE_LOG_DATA_PKT       1

//no need to change
#define BMI160_CONFIG_ENABLE_EARILER_FF_EST_ITVL    1


//no need to change, fine-tuned parameter that just works
#define BMI160_CONFIG_TS_ITVL_EST_COEF          6

//no need to change, fine-tuned parameter that just works
#define BMI160_CONFIG_FIFO_READ_BOOT_WDT        200

//the interrupt latching period (for certain intr such as MDI), 8 means 40ms
#define BMI160_CONFIG_INT_LATCH_MD_REGV         8

//coef out of lab data
#define BMI160_CONFIG_TCOC_ACC_COEFX            -0.17
#define BMI160_CONFIG_TCOC_ACC_COEFY            -0.931
#define BMI160_CONFIG_TCOC_ACC_COEFZ            0.073
//</feature_switches>


#if BMI160_CONFIG_FIFO_WML_CAL_ALGO_LTT && BMI160_CONFIG_FIFO_WML_CAL_ALGO_LDT
#error "one and only one of the two should be enabled"
#endif

#if BMI160_CONFIG_RUN_ON_OSSC

#if !BMI160_CONFIG_RUN_ON_OSSC_64
#if BMI160_CONFIG_ENABLE_UIMAGE
#error "unsupported"
#endif
#endif

#endif

#if BMI160_CONFIG_RUN_UT
#if BMI160_CONFIG_ENABLE_UIMAGE
#error "unsupported"
#endif
#endif


#if BMI160_CONFIG_ENABLE_MAG_IF
#include "sns_dd_bmi160_slave.h"
#endif

#if BMI160_CONFIG_ENABLE_MD_LP_MODE1 && BMI160_CONFIG_ENABLE_MD_LP_MODE2
#error "only one of the two configs can be true"
#endif

#define BMI160_CONFIG_IRQ_TRIGGER_TYPE_DFT      SNS_DDF_SIGNAL_IRQ_RISING
//4: 2.5ms 5: 5ms 6: 10ms
#define BMI160_CONFIG_INT_LATCH_DT_REGV         5

#define BMI160_CONFIG_SSC_TICKS_PER_US          (1e-6 * 32768)

#define BMI160_CONFIG_FIFO_HEADROOM             (1 + 6 + 6 + 8 + 1 + 3) //25

#define BMI160_CONFIG_ACC_LOWEST_ODR            13

#define BMI160_CONFIG_ACC_FASTEST_ODR           200
#define BMI160_CONFIG_ACC_MAX_FREQ              BMI160_CONFIG_ACC_FASTEST_ODR

#define BMI160_CONFIG_GYR_LOWEST_ODR            25
#define BMI160_CONFIG_GYR_FASTEST_ODR           200
#define BMI160_CONFIG_GYR_MAX_FREQ              BMI160_CONFIG_GYR_FASTEST_ODR

#define BMI160_CONFIG_MAG_LOWEST_ODR            1
#define BMI160_CONFIG_MAG_FASTEST_ODR           100
//this works when gyro is in normal mode, refer to datasheet
#define BMI160_CONFIG_TEMP_LOWEST_ODR           1
#define BMI160_CONFIG_TEMP_FASTEST_ODR          1
#define BMI160_CONFIG_TEMP_MAX_FREQ             BMI160_CONFIG_TEMP_FASTEST_ODR

#define BMI160_CONFIG_DT_LOWEST_ODR             200
#define BMI160_CONFIG_DT_FASTEST_ODR            200
#define BMI160_CONFIG_DT_MAX_FREQ               BMI160_CONFIG_DT_FASTEST_ODR

//FIXME: this is observed that when trying to read 1024 bytes,
//the I2C master will read from random address
#define BMI160_SSC_I2C_RD_LEN_MAX               990

#define BMI160_CONFIG_ACC_BWP                   0x02
#define BMI160_CONFIG_GYR_BWP                   0x02

#define BMI160_CONFIG_MD_MODE0_THRESHOLD        0x08
#define BMI160_CONFIG_MD_MODE0_DUR              0x00

#define BMI160_CONFIG_MD_MODE1_THRESHOLD        0x08
#define BMI160_CONFIG_MD_MODE1_DUR              0x00
#define BMI160_CONFIG_MD_MODE1_ODR              50

#define BMI160_CONFIG_MD_MODE1_BWP              0

#define BMI160_CONFIG_ACC_TAP_DUR               4
#define BMI160_CONFIG_ACC_TAP_SHOCK             0
#define BMI160_CONFIG_ACC_TAP_QUIET             0
#define BMI160_CONFIG_ACC_TAP_TH_2G             0x08    //default is 0x0a


//for timestamp related issues (recommended one), the recommended configurations are:
//BMI160_CONFIG_ENABLE_LOG_DATA_L0=1 and BMI160_CONFIG_ENABLE_LOG_DATA_L2=1

//for data integrity issues (also recommend to set the BMI160_CONFIG_DBG_LEVEL to LOW), the recommended configurations are:
//BMI160_CONFIG_ENABLE_LOG_DATA_L1=1 and BMI160_CONFIG_ENABLE_LOG_DATA_L3=1

#define BMI160_CONFIG_ENABLE_LOG_DATA_L0        1
#define BMI160_CONFIG_ENABLE_LOG_DATA_L1        0
#define BMI160_CONFIG_ENABLE_LOG_DATA_L2        1
#define BMI160_CONFIG_ENABLE_LOG_DATA_L3        0

enum DD_DATA_RPT_CONTEXT {
    DD_DATA_RPT_CONTEXT_POLL = 0,
    DD_DATA_RPT_CONTEXT_DRI,
    DD_DATA_RPT_CONTEXT_FIFO_WMI,
    DD_DATA_RPT_CONTEXT_FIFO_FLUSH,
	DD_DATA_RPT_CONTEXT_FIFO_WMI_IMITATED,
    //FIFO frame compensation when FIFO is underrun
    DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION,
    DD_DATA_RPT_CONTEXT_FIFO_FFI,   //FIFO is full
};

typedef struct dd_fifo_read_ctx {
    //enum DD_DATA_RPT_CONTEXT
    uint8_t data_rpt_ctx    :4;
    uint8_t in_irq          :1;
    uint8_t trigger_now     :1;
} dd_fifo_read_ctx_t;


/*=======================================================================

  Preprocessor Definitions and Constants

  ========================================================================*/
#if     BMI160_CONFIG_ENABLE_MAG_IF
#define BMI160_MAX_SUB_DEV_NUM                  4
#define BMI160_MAX_SENSORS_NUM                  5
#else
#define BMI160_MAX_SUB_DEV_NUM                  3
#define BMI160_MAX_SENSORS_NUM                  4
#endif

/* Constants that won't change, go here */
//<portable_part_bst>
#define BOSCH_ACC_RANGE_2G_MAX          FX_FLTTOFIX_Q16(2 * G)
#define BOSCH_ACC_RANGE_2G_MIN          FX_FLTTOFIX_Q16(-2 * G)
#define BOSCH_ACC_RANGE_4G_MAX          FX_FLTTOFIX_Q16(4 * G)
#define BOSCH_ACC_RANGE_4G_MIN          FX_FLTTOFIX_Q16(-4 * G)
#define BOSCH_ACC_RANGE_8G_MAX          FX_FLTTOFIX_Q16(8 * G)
#define BOSCH_ACC_RANGE_8G_MIN          FX_FLTTOFIX_Q16(-8 * G)
#define BOSCH_ACC_RANGE_16G_MAX         FX_FLTTOFIX_Q16(16 * G)
#define BOSCH_ACC_RANGE_16G_MIN         FX_FLTTOFIX_Q16(-16 * G)

#define BOSCH_GYR_RANGE_2000_MAX        FX_FLTTOFIX_Q16(2000 * PI / 180)
#define BOSCH_GYR_RANGE_2000_MIN        FX_FLTTOFIX_Q16(-2000 * PI / 180)
#define BOSCH_GYR_RANGE_1000_MAX        FX_FLTTOFIX_Q16(1000 * PI / 180)
#define BOSCH_GYR_RANGE_1000_MIN        FX_FLTTOFIX_Q16(-1000 * PI  /180)
#define BOSCH_GYR_RANGE_500_MAX         FX_FLTTOFIX_Q16(500 * PI / 180)
#define BOSCH_GYR_RANGE_500_MIN         FX_FLTTOFIX_Q16(-500 * PI  /180)
#define BOSCH_GYR_RANGE_250_MAX         FX_FLTTOFIX_Q16(250 * PI / 180)
#define BOSCH_GYR_RANGE_250_MIN         FX_FLTTOFIX_Q16(-250 * PI / 180)
#define BOSCH_GYR_RANGE_125_MAX         FX_FLTTOFIX_Q16(125 * PI / 180)
#define BOSCH_GYR_RANGE_125_MIN         FX_FLTTOFIX_Q16(-125 * PI / 180)

#define BOSCH_TEMP_MIN                  FX_FLTTOFIX_Q16(-40)
#define BOSCH_TEMP_MAX                  FX_FLTTOFIX_Q16(85)


#ifdef BMI160_CONFIG_RUN_UT
#define BST_FUNC_ATTR_STATIC
#else
#define BST_FUNC_ATTR_STATIC static
#endif

#define BST_ARRAY_SIZE(array)   (sizeof(array)/sizeof(array[0]))
#define BST_MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define BST_MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define BST_MAX3(a, b, c)       (BST_MAX(BST_MAX((a), (b)), (c)))

#define BST_CEIL_P(f) ((f + 0.5))

#ifndef BST_ABS
#define BST_ABS(x) (((int32_t)(x)) > 0 ? (x) : (0 - (x)))
#endif

#define BST_IGNORE_WARNING_UNUSED_VAR(v)        ((void)v)

#define BMS_SCALE_S2US          1000000

static inline uint32_t BST_ABS_DIST_TEST(uint32_t a, uint32_t b)
{
    if (b > a) {
        return b - a;
    } else {
        return a - b;
    }
}

/*@description:
 * this function assumes that between @before and @after, there is at most 1 rewind
 */
static inline
uint32_t BST_TIME_SERIES_DIST(uint32_t before, uint32_t after)
{
    return (int32_t)after - (int32_t)before;
}

#define BMI160_TIMING_ERR_TOLERANCE 0.10f   //+/- 10
static inline bool BST_TIME_ELAPSE_NEAR_ITVL(uint32_t elapse, uint32_t itvl)
{
    int32_t dist;

    if (elapse > itvl) {
        dist = (int32_t)elapse - (int32_t)itvl;
    } else {
        dist = (int32_t)itvl - (int32_t)elapse;
    }


    return ((dist * 10) <= (itvl));
}
//FIXME



#define B0_SET  1
#define B1_SET  (1 << 1)
#define B2_SET  (1 << 2)
#define B3_SET  (1 << 3)
#define B4_SET  (1 << 4)
#define B5_SET  (1 << 5)
#define B6_SET  (1 << 6)
#define B7_SET  (1 << 7)


#define BST_GET_VAL_BIT(val, bit) (((val)>>(bit)) & 0x01)
#define BST_GET_VAL_BITBLOCK(val, start, end) (((val)>>(start)) & ((1<<(end - start + 1))-1))

#define BST_SET_VAL_BIT(val, bit)      (val | (1 << bit))
#define BST_CLR_VAL_BIT(val, bit)      (val & (~(1 << bit)))

#define BST_SET_VAL_BITBLOCK(val, start, end, val_bb) \
    ( (val & \
       (~ (((1<<(end - start + 1))-1) << start) ) \
      )\
      |\
      ((val_bb &\
        ((1<<(end - start + 1))-1)) << start)\
    )


struct bst_odr_ts_map {
    uint8_t odr;
    int32_t interval;
};

struct bst_val_pair {
    uint32_t l;
    uint32_t r;
};

struct bst_cust_param_acc {
    struct {
        uint8_t th;
        uint8_t shock;
        uint8_t quiet;
        uint8_t dur;
    } tap;
};

struct bst_cust_params {
    struct bst_cust_param_acc acc;
};

#define BST_NVM_MAGIC ((uint32_t)'_BST')

#define SNS_DDF_NVM_SIZE            256
#define SNS_DDF_NVM_DEV_START       64
#define SNS_DDF_NVM_DEV_START_BST   128
#define SNS_DDF_NVM_DEV_END         (240 - 1)
struct bst_sensor_nvm_params_entire {
    /*! axis remap: 3 bytes */
    struct {
        uint8_t orient_x;
        uint8_t orient_y;
        uint8_t orient_z;
    } axis_remap;

    /*! reserved aread 1: from offset 3 to 63 */
    uint8_t reserved1[SNS_DDF_NVM_DEV_START - 3];

    union {
        /*! device specific block from offset 64 to offset 640 */
        uint8_t opaque[SNS_DDF_NVM_DEV_END
            - SNS_DDF_NVM_DEV_START + 1];
        struct {
            /*! unknown area to BOSCH */
            uint8_t unknown1[SNS_DDF_NVM_DEV_START_BST
                - SNS_DDF_NVM_DEV_START];

            /*! from offset 128 to offset 240, it is BOSCH area */

            /*-------<BOSCH_AREA>-----*/
            /*! magic written as in the order of:
             * @128: 'T' @129: 'B' @130: 'S' @131: '_'
             */
            uint8_t magic[4];

            /*! param of all bst sensors */
            struct bst_cust_params bst_cust_all;
            /*------</BOSCH_AREA>-----*/
        } dev_param_blk;
    };

    uint8_t reserved2[SNS_DDF_NVM_SIZE - (SNS_DDF_NVM_DEV_END + 1)];
};


struct bst_sbus_spec {
    uint32_t clk_rate;

    uint32_t type       :2;
    uint32_t start_delay_us :30;
};
//</portable_part_bst>


/*=======================================================================

  STRUCTURE DEFINITIONS

  ========================================================================*/

//<portable_part_bmi160>
#define BMI160_REGV_CHIP_ID_MAJOR               (0xd0)
#define BMI160_CHIP_REV_IS_A2(chip_rev)         (0xd0 == ((chip_rev) >> 8))

#define BMI160_SELFTEST_AMP_HIGH                1
#define BMI160_SELFTEST_ACC_SIGN_N              0
#define BMI160_SELFTEST_ACC_SIGN_P              1


#define BMI160_INT_INFO_TAP_FIRST_NONE          0x00
#define BMI160_INT_INFO_TAP_FIRST_X             0x10
#define BMI160_INT_INFO_TAP_FIRST_Y             0x20
#define BMI160_INT_INFO_TAP_FIRST_Z             0x40

#define BMI160_TS_LSB_OVERFLOW_VAL              0x1000000

enum BMI160_INT_MAP_REG_NUM {
    BMI160_INT_MAP_REG_NUM0         = 0,
    BMI160_INT_MAP_REG_NUM1,
    BMI160_INT_MAP_REG_NUM2
};

enum BMI160_ODR_IDX {
    BMI160_ODR_IDX_NORMAL_START,
    BMI160_ODR_IDX_0P78HZ = BMI160_ODR_IDX_NORMAL_START,
    BMI160_ODR_IDX_1P56HZ,
    BMI160_ODR_IDX_3P12HZ,
    BMI160_ODR_IDX_6P25HZ,
    BMI160_ODR_IDX_12P5HZ,
    BMI160_ODR_IDX_25HZ,    //5
    BMI160_ODR_IDX_50HZ,
    BMI160_ODR_IDX_100HZ,
    BMI160_ODR_IDX_200HZ,
    BMI160_ODR_IDX_400HZ,
    BMI160_ODR_IDX_800HZ,
    BMI160_ODR_IDX_1600HZ,  //11
};

#define BMI160_MAX_NUM_AXES     3
#define BMI160_ACC_BIT_LEN      16
#define BMI160_GYR_BIT_LEN      16
#define BMI160_TEMP_BIT_LEN     16

#define BMI160_DATA_LEN_TS      3
#define BMI160_DATA_LEN_ACC     6
#define BMI160_DATA_LEN_GYR     6

//.2 for acc .35 for gyr
#define BMI160_TS_HW_RES_US     39
#define BMI160_TS_HW_RES_US_F   39.275f

#define BMI160_FF_WML_MAX       BMI160_SSC_I2C_RD_LEN_MAX
#define BMI160_FF_DEPTH_BYTES   1024
#define BMI160_FF_MAX_LEVEL_ACC ((BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM - BMI160_FF_FRAME_LEN_TS) / (1 + BMI160_FF_DATA_LEN_ACC))   //142
#define BMI160_FF_MAX_LEVEL_GYR ((BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM - BMI160_FF_FRAME_LEN_TS) / (1 + BMI160_FF_DATA_LEN_GYR))   //142
#define BMI160_FF_MAX_LEVEL_MAG ((BMI160_FF_DEPTH_BYTES - BMI160_CONFIG_FIFO_HEADROOM - BMI160_FF_FRAME_LEN_TS) / (1 + BMI160_FF_DATA_LEN_MAG))   //110

#define BMI160_FF_CACHE_SIZE_PURGE              20
#define BMI160_FF_CACHE_SIZE_ACC                ((1024 / 7) + 10)   //10 is for the sake of overflow handling
//#define BMI160_FF_CACHE_SIZE_GYR                ((1024 / 7) + 10)
//#define BMI160_FF_CACHE_SIZE_MAG                ((1024 / 9) + 10)

#define BMI160_FF_FRAME_LEN_TS  4
#define BMI160_FF_DATA_LEN_ACC  6
#define BMI160_FF_DATA_LEN_GYR  6
#define BMI160_FF_DATA_LEN_MAG  8

#define BMI160_FF_FH_MODE_CTRL                  0x01
#define BMI160_FF_FH_MODE_REGULAR               0x02
#define BMI160_FF_FH_EMPTY                      0x80

#define BMI160_FF_FH_MODE(frame_header_byte)    BST_GET_VAL_BITBLOCK(frame_header_byte, 6, 7)
#define BMI160_FF_FH_PARAM(frame_header_byte)   BST_GET_VAL_BITBLOCK(frame_header_byte, 2, 5)
#define BMI160_FF_FH_EXT(frame_header_byte)     BST_GET_VAL_BITBLOCK(frame_header_byte, 0, 1)

#define BMI160_REGA_USR_CHIP_ID         0x00
#define BMI160_REGA_USR_ERR_REG         0x02
#define BMI160_REGA_USR_PMU_STATUS      0x03
#define BMI160_REGA_USR_DATA_0          0x04
#define BMI160_REGA_USR_DATA_7          0x0b
#define BMI160_REGA_USR_DATA_8          0x0c
#define BMI160_REGA_USR_DATA_13         0x11
#define BMI160_REGA_USR_DATA_14         0x12
#define BMI160_REGA_USR_DATA_19         0x17
#define BMI160_REGA_USR_SENSORTIME_0    0x18
#define BMI160_REGA_USR_SENSORTIME_2    0x1a

#define BMI160_REGA_USR_STATUS          0x1b

#define BMI160_REGA_USR_INT_STATUS_0    0x1c
#define BMI160_REGA_USR_INT_STATUS_1    0x1d
#define BMI160_REGA_USR_INT_STATUS_2    0x1e
#define BMI160_REGA_USR_INT_STATUS_3    0x1f

#define BMI160_REGA_USR_TEMPERATURE_0   0x20
#define BMI160_REGA_USR_TEMPERATURE_1   0x21

#define BMI160_REGA_USR_FIFO_LENGTH_0   0x22

#define BMI160_REGA_USR_FIFO_DATA       0x24

#define BMI160_REGA_USR_ACC_CONF        0x40
#define BMI160_REGA_USR_ACC_RANGE       0x41
#define BMI160_REGA_USR_GYR_CONF        0x42
#define BMI160_REGA_USR_GYR_RANGE       0x43
#define BMI160_REGA_USR_MAG_CONF        0x44

#define BMI160_REGA_USR_FIFO_CONFIG_0   0x46
#define BMI160_REGA_USR_FIFO_CONFIG_1   0x47

#define BMI160_REGA_USR_MAG_IF_0        0x4b
#define BMI160_REGA_USR_MAG_IF_1        0x4c
#define BMI160_REGA_USR_MAG_IF_2        0x4d
#define BMI160_REGA_USR_MAG_IF_3        0x4e
#define BMI160_REGA_USR_MAG_IF_4        0x4f


#define BMI160_REGA_USR_INT_EN_0        0x50
#define BMI160_REGA_USR_INT_EN_1        0x51

#define BMI160_REGA_USR_INT_OUT_CTRL    0x53
#define BMI160_REGA_USR_INT_LATCH       0x54

#define BMI160_REGA_USR_INT_MAP_0       0x55
#define BMI160_REGA_USR_INT_MAP_1       0x56
#define BMI160_REGA_USR_INT_MAP_2       0x57

#define BMI160_REGA_USR_INT_MOTION_0    0x5f
#define BMI160_REGA_USR_INT_MOTION_1    0x60


#define BMI160_REGA_USR_IF_CONF         0x6b
#define BMI160_REGA_USR_PMU_TRIGGER     0x6c

#define BMI160_REGA_USR_SELF_TEST       0x6d

#define BMI160_REGA_USR_INT_TAP_0       0x63
#define BMI160_REGA_USR_INT_TAP_1       0x64
#define BMI160_REGA_CMD_CMD             0x7e
#define BMI160_REGA_CMD_EXT_MODE        0x7f

#define BMI160_INT_STATUS_0_ANYM_INT    (1<<2)
#define BMI160_INT_STATUS_0_D_TAP_INT   (1<<4)

#define BMI160_REGV_CMD_NOOP            0x00
#define BMI160_REGV_CMD_ACC_MODE_SUSP   0x10
#define BMI160_REGV_CMD_ACC_MODE_NORMAL 0x11
#define BMI160_REGV_CMD_ACC_MODE_LP     0x12

#define BMI160_REGV_CMD_GYR_MODE_SUSP   0x14
#define BMI160_REGV_CMD_GYR_MODE_NORMAL 0x15

#define BMI160_REGV_CMD_MAG_MODE_SUSP   0x18
#define BMI160_REGV_CMD_MAG_MODE_NORMAL 0x19

#define BMI160_REGV_CMD_FIFO_FLUSH      0xb0
#define BMI160_REGV_CMD_INT_RESET       0xb1
#define BMI160_REGV_CMD_SOFT_RESET      0xb6

#define BMI160_REGV_CMD_TGT_PAGE_USR   0
#define BMI160_REGV_CMD_TGT_PAGE_COM   1
#define BMI160_REGV_CMD_TGT_PAGE_ACC   2
#define BMI160_REGV_CMD_TGT_PAGE_GYR   3


#define BMI160_REGV_CMD_EXT_MODE_EN_B0  0x37
#define BMI160_REGV_CMD_EXT_MODE_EN_B1  0x9a
#define BMI160_REGV_CMD_EXT_MODE_EN_B2  0xc0



#define BMI160_REGV_ODR_RESERVED       0x00
#define BMI160_REGV_ODR_0_78HZ         0x01
#define BMI160_REGV_ODR_1_56HZ         0x02
#define BMI160_REGV_ODR_3_12HZ         0x03
#define BMI160_REGV_ODR_6_25HZ         0x04
#define BMI160_REGV_ODR_12_5HZ         0x05
#define BMI160_REGV_ODR_25HZ           0x06
#define BMI160_REGV_ODR_50HZ           0x07
#define BMI160_REGV_ODR_100HZ          0x08
#define BMI160_REGV_ODR_200HZ          0x09
#define BMI160_REGV_ODR_400HZ          0x0A
#define BMI160_REGV_ODR_800HZ          0x0B
#define BMI160_REGV_ODR_1600HZ         0x0C
#define BMI160_REGV_ODR_RESERVED0      0x0D
#define BMI160_REGV_ODR_RESERVED1      0x0E
#define BMI160_REGV_ODR_RESERVED2      0x0F

#define BMI160_REGV_ACC_RANGE_PM2G       3
#define BMI160_REGV_ACC_RANGE_PM4G       5
#define BMI160_REGV_ACC_RANGE_PM8G       8
#define BMI160_REGV_ACC_RANGE_PM16G      12


#define BMI160_REGV_GYR_RANGE_PM2000DPS         0
#define BMI160_REGV_GYR_RANGE_PM1000DPS         1
#define BMI160_REGV_GYR_RANGE_PM500DPS          2
#define BMI160_REGV_GYR_RANGE_PM250DPS          3
#define BMI160_REGV_GYR_RANGE_PM125DPS          4

#define BMI160_REGV_PMU_STAT_ACC_SUSPEND        0
#define BMI160_REGV_PMU_STAT_ACC_NORMAL         1
#define BMI160_REGV_PMU_STAT_ACC_LP1            2
#define BMI160_REGV_PMU_STAT_ACC_LP2            3

#define BMI160_REGV_PMU_STAT_GYR_SUSPEND        0
#define BMI160_REGV_PMU_STAT_GYR_NORMAL         1
#define BMI160_REGV_PMU_STAT_GYR_RESERVED       2
#define BMI160_REGV_PMU_STAT_GYR_FAST_STARTUP   3

#define BMI160_REGV_PMU_STAT_MAG_SUSPEND        0
#define BMI160_REGV_PMU_STAT_MAG_NORMAL         1
#define BMI160_REGV_PMU_STAT_MAG_LP1            2
#define BMI160_REGV_PMU_STAT_MAG_LP2            3

#define BMI160_INT_EN_0_BIT_ANYMO_X             0
#define BMI160_INT_EN_0_BIT_ANYMO_Y             1
#define BMI160_INT_EN_0_BIT_ANYMO_Z             2
#define BMI160_INT_EN_0_BIT_D_TAP               4

#define BMI160_INT_EN_1_BIT_DRDY                4
#define BMI160_INT_EN_1_BIT_FF_WML              6

#define BMI160_INT_EN_2_BIT_PEDO                3

#define BMI160_INT_MAP_0_BIT_ANYM               2
#define BMI160_INT_MAP_0_BIT_D_TAP              4

#define BMI160_INT_MAP_1_BIT_DRDY               7
#define BMI160_INT_MAP_1_BIT_WML                6

#define BMI160_SPEC_STARTUP_TIME_SBUS_MS        3


#define BMI160_SPEC_STARTUP_TIME_MS             10
#define BMI160_SPEC_IF_IDLE_TIME_NORMAL_US      2
#define BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US     450


#define BMI160_SPEC_SOFT_RESET_DELAY_TIME_MS    80

#define BMI160_SPEC_ACC_STARTUP_TIME_MS         10  //2.5ms in datasheet
#define BMI160_SPEC_GYR_STARTUP_TIME_MS         80
#define BMI160_SPEC_MAG_STARTUP_TIME_MS         60

#define BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS  50
#define BMI160_SPEC_GYR_SELF_TEST_WAIT_TIME_MS  130


typedef enum {
    SDD_MAG_X = 0,
    SDD_MAG_Y,
    SDD_MAG_Z,
    SDD_MAG_NUM_AXIS,
    SDD_MAG_NUM_DATATYPES
} bmi160_sdd_mag_data_type_e;
#define BMI160_CALL_API(name) bmi160_##name



struct bmi160_val_acc {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct bmi160_val_gyr {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct bmi160_val_mag {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct bmi160_sensor_data {
    struct bmi160_val_acc val_a;
    struct bmi160_val_gyr val_g;
    struct bmi160_val_mag val_m;

    uint32_t              ts_hw;

    uint16_t              val_temp;
    uint16_t              drdy_a    :1;
    uint16_t              drdy_g    :1;
    uint16_t              drdy_m    :1;
};

typedef union bmi160_status_reg {
    struct {
        uint8_t por_detected            :1;
        uint8_t gyr_self_test_ok        :1;
        uint8_t mag_man_op              :1;
        uint8_t foc_rdy                 :1;
        uint8_t nvm_rdy                 :1;
        uint8_t drdy_mag                :1;
        uint8_t drdy_gyr                :1;
        uint8_t drdy_acc                :1;
    } bits;
    uint8_t reg;
} bmi160_status_reg_t;

struct bmi160_sensor_data_raw_buf {
    uint8_t buf_a[6];

    uint8_t buf_g[6];

    uint8_t buf_m[8];

    uint8_t buf_ts[3];

    uint8_t buf_temp[2];

    uint8_t _reserved[2];


    uint8_t avail_ts   :1;
};

typedef union bmi160_sensor_data_req {
    struct {
        uint32_t m:  1;
        uint32_t g:  1;
        uint32_t a:  1;
        uint32_t ts: 1;
        uint32_t t:  1;
        uint32_t p:  1;

    } bits;

    uint32_t req;
} bmi160_sensor_data_req_t;

union bmi160_fifo_config_1 {
    struct {
        uint8_t fifo_stop_on_full:      1;
        uint8_t fifo_time_en:           1;
        uint8_t fifo_tag_int2_en:      1;
        uint8_t fifo_tag_int1_en:      1;
        uint8_t fifo_header_en:         1;
        uint8_t fifo_mag_en:            1;
        uint8_t fifo_acc_en:            1;
        uint8_t fifo_gyr_en:            1;
    } bits;

    uint8_t reg;
};

enum {
    BMI160_FF_FRAME_ERR_NONE = 0,
    BMI160_FF_FRAME_ERR_UNKNOWN,
    BMI160_FF_FRAME_ERR_PARTIAL_READ,
};

typedef union bmi160_fifo_parse_result {
    struct {
        //NU
        uint32_t ff_err         :1;
        //NU
        uint32_t ff_err_code    :3;

        uint32_t ff_overflow    :1;

        uint32_t ff_cache_full_agm      :3;

        uint32_t ff_avail_data_agm      :3;

        uint32_t ff_avail_ts            :1;
        uint32_t ff_avail_ts_header     :1;
        uint32_t ff_avail_end_frame     :1;
    } bits;

    uint32_t rslt;
} bmi160_fifo_parse_result_t;

typedef union bmi160_fifo_status {
    struct {
        //NU
        uint32_t ff_err         :1;
        //NU
        uint32_t ff_err_code    :3;

        uint32_t ff_overflow    :1;

        uint32_t ff_rdy_acc     :1;
        uint32_t ff_rdy_gyr     :1;
        uint32_t ff_rdy_mag     :1;
    } bits;

    uint32_t stat;
} bmi160_fifo_status_t;

typedef union bmi160_hw_err_stat {
    /*! bmi160 sensor error status */
    struct {
        uint8_t fatal_err:      1;
        uint8_t err_code:       4;
        uint8_t i2c_fail_err:   1;
        uint8_t drop_cmd_err:   1;
        uint8_t mag_drdy_err:   1;
    } bits;

    uint8_t         regv;
} bmi160_hw_err_stat_t;

typedef union bmi160_pmu_stat_reg {
    struct {
        uint8_t mag_pmu_status :2;
        uint8_t gyr_pmu_status :2;
        uint8_t acc_pmu_status :2;
    } bits;

    uint8_t reg;
} bmi160_pmu_stat_reg_t;



//</portable_part_bmi160>

//<ssc_specific>

#define DD_ATTR_LOCAL static
//the following files are for log only
#ifndef BMI160_CONFIG_RUN_UT
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"
#endif

#include "sns_dd_bmi160_trace.h"

#if BMI160_CONFIG_RUN_ON_OSSC
#include "qurt_elite_diag.h"    //deprecated by qc internal builds
//#include "lk_debug_ossc.h"
//#include "lk_debug.h"

extern
void memscpy(void *, uint32_t, void *, uint32_t);
#else
#include "sns_memmgr.h"

#if BMI160_FOR_8X26_BUILD
#include "sns_debug_interface_v01.h"
#endif

#endif

#if !BMI160_CONFIG_ENABLE_UIMAGE
#define sns_ddf_malloc_ex(ptr, size, smgr_handle)                               sns_ddf_malloc(ptr, size)
#define sns_ddf_mfree_ex(ptr, smgr_handle)                                      sns_ddf_mfree(ptr)
#define sns_ddf_memhandler_malloc_ex(memhandler, size, smgr_handle)             sns_ddf_memhandler_malloc(memhandler, size)
#endif


#if BMI160_CONFIG_RUN_UT
#include "lk_debug_simu.h"
#else



#if BMI160_CONFIG_ENABLE_UIMAGE
#define DD_MSG0(msk, fmt)                               do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG(MSG_SSID_SNS, msk, fmt "$"); } while (0)
#define DD_MSG1(msk, fmt, arg1)                         do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG_1(MSG_SSID_SNS, msk, fmt "$", arg1); } while (0)
#define DD_MSG2(msk, fmt, arg1, arg2)                   do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG_2(MSG_SSID_SNS, msk, fmt "$", arg1, arg2); } while (0)
#define DD_MSG3(msk, fmt, arg1, arg2, arg3)             do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3); } while (0)
#define DD_MSG4(msk, fmt, arg1, arg2, arg3, arg4)       do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG_4(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3, arg4); } while (0)
#define DD_MSG5(msk, fmt, arg1, arg2, arg3, arg4, arg5) do { if (msk >= BMI160_CONFIG_DBG_LEVEL) UMSG_5(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3, arg4, arg5); } while (0)
#else

#if BMI160_CONFIG_RUN_ON_OSSC
#define DD_MSG0(msk, fmt)                               do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG(MSG_SSID_QDSP6, msk, fmt "$"); } while (0)
#define DD_MSG1(msk, fmt, arg1)                         do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_1(MSG_SSID_QDSP6, msk, fmt "$", arg1); } while (0)
#define DD_MSG2(msk, fmt, arg1, arg2)                   do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_2(MSG_SSID_QDSP6, msk, fmt "$", arg1, arg2); } while (0)
#define DD_MSG3(msk, fmt, arg1, arg2, arg3)             do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6, msk, fmt "$", arg1, arg2, arg3); } while (0)
#define DD_MSG4(msk, fmt, arg1, arg2, arg3, arg4)       do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_4(MSG_SSID_QDSP6, msk, fmt "$", arg1, arg2, arg3, arg4); } while (0)
#define DD_MSG5(msk, fmt, arg1, arg2, arg3, arg4, arg5) do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_5(MSG_SSID_QDSP6, msk, fmt "$", arg1, arg2, arg3, arg4, arg5); } while (0)
#else   //BMI160_CONFIG_RUN_ON_OSSC
#define DD_MSG0(msk, fmt)                               do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG(MSG_SSID_SNS, msk, fmt "$"); } while (0)
#define DD_MSG1(msk, fmt, arg1)                         do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_1(MSG_SSID_SNS, msk, fmt "$", arg1); } while (0)
#define DD_MSG2(msk, fmt, arg1, arg2)                   do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_2(MSG_SSID_SNS, msk, fmt "$", arg1, arg2); } while (0)
#define DD_MSG3(msk, fmt, arg1, arg2, arg3)             do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3); } while (0)
#define DD_MSG4(msk, fmt, arg1, arg2, arg3, arg4)       do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_4(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3, arg4); } while (0)
#define DD_MSG5(msk, fmt, arg1, arg2, arg3, arg4, arg5) do { if (msk >= BMI160_CONFIG_DBG_LEVEL) MSG_5(MSG_SSID_SNS, msk, fmt "$", arg1, arg2, arg3, arg4, arg5); } while (0)
#endif

#endif  //BMI160_CONFIG_ENABLE_UIMAGE

#endif  //BMI160_CONFIG_RUN_UT


#define BMI160_CONFIG_PRIMITIVE_DEBUG_MSG 0
#if BMI160_CONFIG_ENABLE_DBG_MSG

#if BMI160_CONFIG_PRIMITIVE_DEBUG_MSG
#define BMI160_MSG_0(level, msg)
#define BMI160_MSG_1(level, msg, p1)
#define BMI160_MSG_2(level, msg, p1, p2)
#define BMI160_MSG_3(level, msg, p1, p2, p3)
#define BMI160_MSG_3_F(level, msg, p1, p2, p3)          SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, p1, p2, p3)
#define BMI160_MSG_4(level, msg, p1, p2, p3, p4)
#define BMI160_MSG_4_F(level, msg, p1, p2, p3, p4)      SNS_PRINTF_STRING_ID_##level##_4(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING4, p1, p2, p3, p4)
#define BMI160_MSG_5(level, msg, p1, p2, p3, p4, p5)
#define MED MEDIUM

#else

#define DBG_MEDIUM_PRIO DBG_MED_PRIO

#define BMI160_MSG_0(level, msg)                        DD_MSG0(DBG_##level##_PRIO, "BMI160-" msg)
#define BMI160_MSG_1(level, msg, p1)                    DD_MSG1(DBG_##level##_PRIO, "BMI160-" msg, p1)
#define BMI160_MSG_2(level, msg, p1, p2)                DD_MSG2(DBG_##level##_PRIO, "BMI160-" msg, p1, p2)
#define BMI160_MSG_3(level, msg, p1, p2, p3)            DD_MSG3(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3)
#define BMI160_MSG_3_F(level, msg, p1, p2, p3)          DD_MSG3(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3)
#define BMI160_MSG_4(level, msg, p1, p2, p3, p4)        DD_MSG4(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3, p4)
#define BMI160_MSG_4_F(level, msg, p1, p2, p3, p4)      DD_MSG4(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3, p4)
#define BMI160_MSG_5(level, msg, p1, p2, p3, p4, p5)    DD_MSG5(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3, p4, p5)

#endif

#else   //BMI160_CONFIG_ENABLE_DBG_MSG
#define BMI160_MSG_0(level, msg)
#define BMI160_MSG_1(level, msg, p1)
#define BMI160_MSG_2(level, msg, p1, p2)
#define BMI160_MSG_3(level, msg, p1, p2, p3)

#define BMI160_MSG_3_F(level, msg, p1, p2, p3)          //DD_MSG3(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3)
#define BMI160_MSG_4_F(level, msg, p1, p2, p3, p4)      //DD_MSG4(DBG_##level##_PRIO, "BMI160-" msg, p1, p2, p3, p4)

#define BMI160_MSG_4(level, msg, p1, p2, p3, p4)
#define BMI160_MSG_5(level, msg, p1, p2, p3, p4, p5)
#endif  //BMI160_CONFIG_ENABLE_DBG_MSG

extern uint32_t sns_em_convert_usec_to_dspstick(uint32_t microsec_time);
#define BMI160_SSC_US_2_TICKS(us) sns_em_convert_usec_to_dspstick(us)
//#define BMI160_SSC_US_2_TICKS(us) sns_ddf_convert_usec_to_tick(us)
/*=======================================================================

  INTERNAL ENUMS

  ========================================================================*/
/* Supported datatypes */
typedef enum {
    SDD_ACC_X = 0,
    SDD_ACC_Y,
    SDD_ACC_Z,
    SDD_ACC_NUM_AXIS,               /* 3 */
    SDD_TEMP = SDD_ACC_NUM_AXIS,    /* 3 */
    SDD_DOUBLE_TAP,                 /* 4 */
    SDD_ACC_NUM_DATATYPES           /* 5 */
} BMI160_SDD_DATA_TYPE_E;

typedef enum {
    SDD_GYR_X = 0,
    SDD_GYR_Y,
    SDD_GYR_Z,
    SDD_GYR_NUM_AXIS,              /* 3 */
    SDD_GYR_NUM_DATATYPES          /* 4 */
} BMI160_SDD_GYR_DATA_TYPE_E;

enum {
    BMI160_HW_MOD_TYPE_ACC = 0,
    BMI160_HW_MOD_TYPE_GYR,
    BMI160_HW_MOD_TYPE_MAG,
};

/**
 * This struct is used to represent the head of the singly-linked list that
 * contains all device instances related to this device driver.
 */
typedef struct {
    sns_ddf_dev_list_t     next;              // The next device instance
    sns_ddf_driver_info_s  dd_info;           // Device driver info
} sns_dd_bmi160_head_t;


/**
 * This struct represents a single sensor type that is contained within a combo
 * device. (e.g. ACCEL, TEMP, DOUBLETAP, etc.)
 */
typedef struct bmi160_vsensor {
    // Sensor ID based on the sns_ddf_sensor_e enum
    sns_ddf_sensor_e  sensor_type;

    q16_t             biases[SDD_ACC_NUM_AXIS];
} bmi160_vsensor_t;     /* virtual sensor */

/**
 * This struct represents a single sub-device that is contained within a combo
 * device. (e.g. ACCEL/TEMP,  DOUBLETAP, etc.)
 */
/* import the type */
typedef struct bmi160_sub_dev {
    /*! backward reference of parent device instance(i.e.: state) */
    void                            *private_data;
    /*! SMGR handle used to notify async events from this sub-device */
    sns_ddf_handle_t                smgr_handle;
    uint16_t                        num_sensors;

    /* for sensors within the same sub_dev */
    bmi160_vsensor_t                sensors[BMI160_MAX_SENSORS_NUM];
} bmi160_sub_dev_t;

enum {
    BMI160_HW_MOD_IDX_ACC = 0,
    BMI160_HW_MOD_IDX_GYR,
    BMI160_HW_MOD_IDX_MAG,
    BMI160_HW_MOD_MAX_NUM
};

typedef struct bmi160_hw_mod {
    char                    *name;
    int32_t                 hw_mod_type;

    int32_t                 sensor_type_supported[BMI160_MAX_SENSORS_NUM];
    int32_t                 ref_cnt;

    sns_ddf_odr_t           odr_curr;
    uint8_t                 odr_regv;

    int32_t                 odr_map_idx;

    int8_t                  range_idx;

    void                    *dev_inst;

    sns_ddf_status_e        (*set_power_state)(
            struct bmi160_hw_mod    *hw_mod,
            sns_ddf_handle_t        port_handle,
            sns_ddf_powerstate_e    power_state);

    sns_ddf_status_e        (*get_best_match_odr)(
            struct bmi160_hw_mod    *hw_mod,
            sns_ddf_sensor_e        sensor_type,
            uint32_t                odr_req,
            sns_ddf_odr_t           *odr_matched);

    sns_ddf_status_e        (*set_odr)(
            struct bmi160_hw_mod    *hw_mod,
            sns_ddf_handle_t        port_handle,
            uint8_t                 odr_regv);

    /* can be NULL */
    sns_ddf_status_e        (*set_range)(
            struct bmi160_hw_mod    *hw_mod,
            sns_ddf_handle_t        port_handle,
            uint8_t                 range_idx);
} bmi160_hw_mod_t;


struct ts_pair_sys_and_dev {
    sns_ddf_time_t  ts_sys;
    uint32_t        ts_dev;
};

typedef struct __attribute__ ((packed)) {
    sns_ddf_axes_map_s    axes_map;
    uint8_t     Reserved[128 - sizeof(sns_ddf_axes_map_s)];    // Reserved Space
    uint32_t    magic_num;                // Magic Number to verify the start of bmi160 tap settings
    uint8_t th;
    uint8_t shock;
    uint8_t quiet;
    uint8_t dur;
} sns_dd_bmi160_nv_params_s;


typedef struct bmi160_tap_settings {
    uint8_t th;
    uint8_t shock;
    uint8_t quiet;
    uint8_t dur;
} bmi160_tap_settings_s;


struct bmi160_reg_int_context {
    uint32_t        ts_hw;
    //TODO; NU
    uint32_t        ts_sys;

    uint8_t         status;

    uint8_t         int_status_0;
    uint8_t         int_status_1;
    uint8_t         int_status_2;
    uint8_t         int_status_3;

    union {
        struct {
            uint8_t ts              :1;
            uint8_t status_0        :1;
            uint8_t status_1        :1;
            uint8_t status_2        :1;
            uint8_t status_3        :1;
        } bits;

        uint8_t flags;
    } avail;
};

struct sns_dd_bmi160_ff_sample_cache {
    uint32_t                fc;
    sns_ddf_sensor_sample_s samples[BMI160_FF_CACHE_SIZE_ACC * 3];
};

typedef struct bmi160_time_info {
    sns_ddf_time_t  ts_sys_irq;
    sns_ddf_time_t  ts_sys_curr;

    uint32_t        ts_dev_reg;

    uint32_t        ts_dev_ff               :24;
    uint32_t        avail_ts_dev_ff         :1;
} bmi160_time_info_t;

typedef union bmi160_int_en_fifo_drdy_flag {
    struct {
        uint8_t acc :1;
        uint8_t gyr :1;
        uint8_t mag :1;
    } bits;

    uint8_t flag;
} bmi160_int_en_fifo_drdy_flag_t;

typedef union bmi160_int_en_flag {
    struct {
        bmi160_int_en_fifo_drdy_flag_t drdy;
        bmi160_int_en_fifo_drdy_flag_t fifo;

        uint8_t  step           :1;
        uint8_t  sigmot         :1;
        uint8_t  md             :1;
        uint8_t  pmu_trigger    :1;
        uint8_t  dt             :1;
    } bits;

    uint32_t flag;
} bmi160_int_en_flag_t;

typedef union bmi160_int_stat_drdy {
    struct {
        uint8_t acc :1;
        uint8_t gyr :1;
        uint8_t mag :1;
    } bits;

    uint8_t flag;
} bmi160_int_stat_drdy_t;

typedef union bmi160_int_stat_flag {
    struct {
        bmi160_int_stat_drdy_t          drdy_detail;
        uint8_t  drdy                   :1;

        uint8_t  ff_wml                 :1;
        uint8_t  ff_full                :1;
        uint8_t  md                     :1;
        uint8_t  dt                     :1;
        uint8_t  sigmot                 :1;
        uint8_t  pmu_trigger            :1;
    } bits;

    uint32_t flag;
} bmi160_int_stat_flag_t;


enum {
    BMI160_MD_MODE_0 = 0,   /* ANY MOTION in FULL POWER MODE */
    BMI160_MD_MODE_1,       /* ANY MOTION in LP */
    BMI160_MD_MODE_2,       /* ADV MOTION in LP */
};

struct bmi160_motion_detect_mode {
    const uint32_t    mode:    3;

    void        *private_data;

    int32_t     (*enter)(
            struct bmi160_motion_detect_mode *mode);
    int32_t     (*enter_post)(
            struct bmi160_motion_detect_mode *mode);

    int32_t     (*exit)(
            struct bmi160_motion_detect_mode *mode);
};

struct bmi160_fifo_sync_marker {
    int16_t         fc;
    uint32_t        fc_master;
};

typedef struct bmi160_fifo_sample_sync_info {
    //an ever increasing number: frame_counter since streaming
    uint32_t                fc_master;

    struct bmi160_fifo_sync_marker last_sync_marker_a;
    struct bmi160_fifo_sync_marker last_sync_marker_g;
    struct bmi160_fifo_sync_marker last_sync_marker_m;


    sns_ddf_time_t          ts_sys_last_sync_ref;
    uint32_t                ts_dev_last_sync_ref;
    //TODO
    uint32_t                fc_master_last_sync_ref;


    sns_ddf_time_t          ts_itvl_master_sample_exp;
    sns_ddf_time_t          ts_itvl_master_sample_est;
    uint8_t                 ready_for_itvl_est              :1;
    uint8_t                 avail_1st_itvl_est              :1;

    uint8_t                 avail_master_ref                :1;

    //bit 0: acc bit 1: gyr bit2: mag
    uint8_t                 avail_last_ts;
} bmi160_fifo_sample_sync_info_t;

struct bmi160_timer_data {
    int32_t     (*cb)(void *private_data);
};

struct bmi160_timer {
    sns_ddf_timer_s             timer_obj;
    struct bmi160_timer_data    timer_data;
};

#define BMI160_DEV_INSTANCE_MAGIC 0x49313630    //('I' << 24) | ('1' << 16) | ('6' << 8) | '0'
/*!  state struct for BMI160 driver */
typedef struct sns_dd_bmi160_state {
    uint32_t                magic;

    //for new data members, if they are for uimg, place them here
    //<for_uimg>
#if BMI160_CONFIG_ENABLE_MAG_IF
    struct bmi160_slave     *slaves[BMI160_SLAVE_SENSOR_TYPE__NUM];
#endif
    //</for_uimg>

    /*! pointer to next device instance in the device chain */
    sns_ddf_dev_list_t      next;
    sns_ddf_port_id_s       port_id;
    sns_ddf_port_id_s       port_slave;

    /*! handle used to access the (I2C) bus */
    //we added port_handle_real while keeping port_handle to minimize the code changes as a refactoring method
    sns_ddf_handle_t        port_handle_real;
    sns_ddf_handle_t        port_handle;
    sns_ddf_time_t          ts_last_sbus_write;

    /*! GPIO number used for interrupts */
    uint32_t                gpio1;
    uint32_t                gpio2;
    /*! customization and fine-tuning parameters*/

    uint16_t                chip_rev;


    struct bms_platform_data bps_data;

    /*! for the combo device driver */
    uint8_t                 sub_dev_cnt;
    bmi160_sub_dev_t        sub_dev[BMI160_MAX_SUB_DEV_NUM];
    bmi160_sub_dev_t        *sub_dev_cache_acc;
    bmi160_sub_dev_t        *sub_dev_cache_gyr;
    bmi160_sub_dev_t        *sub_dev_cache_mag;

    struct bmi160_hw_mod    hw_mods[BMI160_HW_MOD_MAX_NUM];
    struct bmi160_hw_mod    *hw_mod_cache_acc;
    struct bmi160_hw_mod    *hw_mod_cache_gyr;
    struct bmi160_hw_mod    *hw_mod_cache_mag;
    /*! current range selection idx: starting from 0 */
    uint8_t                 range_idx_acc;

    uint8_t                 range_idx_gyr;


    union bmi160_hw_err_stat     hw_err_st;
    uint8_t                 last_cmd_regv;

    struct bmi160_timer timer_sbus_susp_mode;

    /*! used when notifying the data to SMGR */
    sns_ddf_sensor_data_s   sensor_data_cache;
    sns_ddf_sensor_sample_s sensor_data_cache_samples[3];


    sns_ddf_sensor_data_s   sensor_data_cache_a;
    sns_ddf_sensor_sample_s sensor_data_cache_samples_a[3];

    sns_ddf_sensor_data_s   sensor_data_cache_g;
    sns_ddf_sensor_sample_s sensor_data_cache_samples_g[3];

#if BMI160_CONFIG_ENABLE_MAG_IF
    sns_ddf_sensor_data_s   sensor_data_cache_m;
    sns_ddf_sensor_sample_s sensor_data_cache_samples_m[3];
#endif
    //temperature
    double                  cache_temperature;
    sns_ddf_sensor_data_s   sensor_data_cache_t;
    sns_ddf_sensor_sample_s sensor_data_cache_samples_t[1];

#if BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP
    sns_ddf_sensor_data_s   sensor_data_cache_dt;
    sns_ddf_sensor_sample_s sensor_data_cache_samples_dt[1];
#endif

    q16_t                   data_cache[16];

    struct bmi160_motion_detect_mode *md_mode;

    bmi160_int_en_flag_t    int_en_flags;


    uint32_t                bus_clk_rate;

    uint32_t                bus_type                        :2;

    uint32_t                acc_in_lp                       :1;

    uint32_t                signal_registered               :1;
    uint32_t                test_inertial_in_progress       :1;
    uint32_t                ff_wmi_missing_possible         :1;

    uint32_t                avail_last_ts_dri_acc           :1;
    uint32_t                avail_last_ts_dri_gyr           :1;
    uint32_t                avail_last_ts_dri_mag           :1;

    uint32_t                mag_if_setup_needed             :1;

    //turn off single byte rw monitoring
    uint32_t                sbus_mon_single_byte_rw_off     :1;

    uint32_t                sbus_in_normal_mode             :1;
    uint32_t                timer_pending_sbus_susp_mode    :1;

    uint32_t                ff_frames_underrun_acc          :2;
    uint32_t                ff_frames_underrun_gyr          :2;
    uint32_t                ff_frames_underrun_mag          :2;

    int32_t                 irq_trigger_type;

    q16_t                   bias_acc[SDD_ACC_NUM_AXIS];
    q16_t                   bias_gyr[SDD_ACC_NUM_AXIS];

    sns_ddf_axes_map_s      axes_map_acc;
    sns_ddf_axes_map_s      axes_map_gyr;
    sns_ddf_axes_map_s      axes_map_mag;

    bmi160_tap_settings_s   tap_settings;

    bmi160_hw_mod_t         *hw_mod_odr_max;
    bmi160_hw_mod_t         *hw_mod_ff_master;

    /*! accel sensor sampling frequency. */
    sns_ddf_odr_t odr_acc_req;
    /*! double tap sensor sampling frequency. */
    sns_ddf_odr_t odr_dbt_req;
    /*! gyr sensor sampling frequency. */
    sns_ddf_odr_t odr_gyr_req;
    /*! temperature sensor sampling frequency. */
    sns_ddf_odr_t odr_tem_req_acc;
    sns_ddf_odr_t odr_tem_req_gyr;
    sns_ddf_odr_t odr_tem_req_mag;
    /*! magnetic sensor sampling frequency. */
    sns_ddf_odr_t odr_mag_req;

    //register value caches

    uint8_t                 regvc_int_map_1;

    struct bmi160_reg_int_context reg_int_ctx;

    struct ts_pair_sys_and_dev ts_sync_sys_and_dev;

    sns_ddf_time_t          ts_itvl_odr_master_sample_est;
    uint8_t                 avail_odr_master_itvl_est           :1;

    /*! FIFO */
    /* water mark level in bytes */

    sns_ddf_time_t          ts_ff_irq;

#if BMI160_CONFIG_ENABLE_TS_ADJUST_FILTER
    //use uint16_t to save a little bit of memory
    uint16_t                ts_adjust_acc;
    uint16_t                ts_adjust_gyr;
#endif

    uint16_t                ff_wml;
    uint16_t                ff_suggested_bytes2read;


    uint16_t                ff_wml_req_acc;
    uint16_t                ff_wml_req_gyr;
    uint16_t                ff_wml_req_mag;


    sns_ddf_sensor_data_s   ff_cache_a;
    sns_ddf_sensor_data_s   ff_cache_g;
    sns_ddf_sensor_data_s   ff_cache_m;

    sns_ddf_time_t          ts_last_sample_a;
    sns_ddf_time_t          ts_last_sample_g;
    sns_ddf_time_t          ts_last_sample_m;

    sns_ddf_time_t          ts_last_sample_t;

    sns_ddf_time_t          ts_mag_measurement_started;

    bmi160_fifo_sample_sync_info_t ff_sample_sync_info;

    //estimated ts for 1st frame in the samples cache
    //-1 means that not synced with master yet

    uint8_t                 __barrier0[4];
    uint8_t                 ff_cache_raw[BMI160_FF_DEPTH_BYTES];
    uint8_t                 __barrier1[4];

    uint8_t                 __barrier2[4];
    struct sns_dd_bmi160_ff_sample_cache ff_cache_samples_a;
    uint8_t                 __barrier3[4];

    uint8_t                 __barrier4[4];
    struct sns_dd_bmi160_ff_sample_cache ff_cache_samples_g;
    uint8_t                 __barrier5[4];
#if BMI160_CONFIG_ENABLE_MAG_IF
    uint8_t                 __barrier6[4];
    struct sns_dd_bmi160_ff_sample_cache ff_cache_samples_m;
    uint8_t                 __barrier7[4];
#endif

} sns_dd_bmi160_state_t;
//</ssc_specific>


#endif  //_SNS_DD_BMI160_H

