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

#ifndef _AMS_TMG399X_H
#define _AMS_TMG399X_H

#include <stdint.h>
#include "stdbool.h"

#define ALSPRX_USE_POLLING             /* Polling is used instead of interrupt */
#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef  ALSPRX_DEBUG_TIMER
#undef  ALSPRX_DEBUG_IRQ
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef  ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_ALS_DATA_READY_US       115000  /* 115ms for ALS */
#define SNS_DD_ALSPRX_PRX_DATA_READY_US       540     /* 0.54ms for PRX */
#define SNS_DD_ALSPRX_ALPHA_LO                326
#define SNS_DD_ALSPRX_ALPHA_HI                5220

#define SNS_DD_ALS_ACCURACY_DIV_FACTOR        4       /* 4 means 1/4 or 25% */
#define SNS_DD_ALS_HALF_LUX                   500

#define SNS_DD_ALS_DFLT_MILLI_LUX             100000  /* default value for the last sampled mlux */
#define SNS_DD_ALS_LONG_TIME_AGO_US           5000000 /* time for ignoring the previous data - 5 sec */

/* attributes for light data type */
/* The res and accuracy for ALS are not static values.
They're calculated from the previous data */
#define SNS_DD_ALS_PWR                        330      /* unit of uA */
#define SNS_DD_ALS_RES                        1.0      /* unit of this data type is lux */
#define SNS_DD_ALSPRX_LO_PWR                  1        /* unit of uA */

/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                       3500     /* unit of uA */
#define SNS_DD_PRX_RES                        0.05    /* unit of this data type is meter */
#define SNS_DD_PRX_ACCURACY                   40      /* unit of this data type unit which is mm */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO            25
#define SNS_DD_IR_TRANS_RATIO                 60
#define SNS_DD_DC_OFFSET                      10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                80      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                 50      /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                    200
#define SNS_DD_ALS_FACTOR                    100

#define SNS_DD_PRXDIST_TB_MAX_VALUE           2047    /* 11 bits + 1 sign bit */
#define SNS_DD_ALSPRX_PRX_BITS                8
#define SNS_DD_ALSPRX_PRX_FREQ                20
#define SNS_DD_ALSPRX_ALS_BITS                16
#define SNS_DD_ALSPRX_ALS_FREQ                20

#define SNS_DD_ALS_RANGES_MIN                  1
#define SNS_DD_ALS_RANGES_MAX           (32767)


#define SNS_DD_AMS_NUM_SENSOR_TYPES           4

#define AMS_MAX_NUM_OF_399x_DEVICES          (2)

//
// Register names for the tmg399x
//

typedef enum  {
    AMS_REG_ENABLE,   //  0
    AMS_REG_ATIME,    //  1

    AMS_REG_RESV_2,   //  2

    AMS_REG_WTIME,    //  3
    AMS_REG_AILTHL,   //  4 AMS_REG_AILTL
    AMS_REG_AILTHH,   //  5 AMS_REG_AILTH
    AMS_REG_AIHTHL,   //  6 AMS_REG_AIHTL
    AMS_REG_AIHTHH,   //  7 AMS_REG_AIHTH

    AMS_REG_RES_8,

    AMS_REG_PITHL,    //  9 AMS_REG_PILTH

    AMS_REG_RES_A,

    AMS_REG_PITHH,    //  B AMS_REG_PIHTH
    AMS_REG_PERS,     //  C
    AMS_REG_CONFIG1,  //  D AMS_REG_CONFIG
    AMS_REG_PPULSE,   //  E
    AMS_REG_CONTROL,  //  F

    AMS_REG_CONFIG2,  // 10
    AMS_REG_REVID,    // 11 AMS_REG_REVISION
    AMS_REG_ID,       // 12
    AMS_REG_STATUS,   // 13
    AMS_REG_CDATAL,   // 14 AMS_REG_CDATA
    AMS_REG_CDATAH,   // 15
    AMS_REG_RDATAL,   // 16 AMS_REG_RDATA
    AMS_REG_RDATAH,   // 17
    AMS_REG_GDATAL,   // 18 AMS_REG_GDATA

    AMS_REG_GDATAH,   // 19
    AMS_REG_BDATAL,   // 1A AMS_REG_BDATA
    AMS_REG_BDATAH,   // 1B
    AMS_REG_PDATA,    // 1C

    AMS_REG_POFFSET_NE, // 1D
    AMS_REG_POFFSET_SW, // 1E

    AMS_REG_CONFIG3,    // 1F

    AMS_REG_CONFIG_A0,  // 20
    AMS_REG_CONFIG_A1,  // 21
    AMS_REG_CONFIG_A2,  // 22
    AMS_REG_CONFIG_A3,  // 23
    AMS_REG_CONFIG_A4,  // 24
    AMS_REG_CONFIG_A5,  // 25
    AMS_REG_CONFIG_A6,  // 26
    AMS_REG_CONFIG_A7,  // 27
    AMS_REG_CONFIG_A8,  // 28

    AMS_REG_CONFIG_A9,  // 29
    AMS_REG_CONFIG_AA,  // 2A
    AMS_REG_CONFIG_AB,  // 2B

    AMS_REG_RES_2C,
    AMS_REG_RES_2D,

    AMS_REG_CONFIG_AE,  // 2E  GFLVL
    AMS_REG_CONFIG_AF,  // 2F

    AMS_REG_CONFIG_C7,  // 30
    AMS_REG_PBCLEAR,    // 31
    AMS_REG_IFORCE,     // 32
    AMS_REG_PICLEAR,    // 33
    AMS_REG_CICLEAR,    // 34
    AMS_REG_AICLEAR,    // 35
    AMS_REG_I2C_DB,     // 36
    AMS_REG_GFIFO_N,    // 37
    AMS_REG_GFIFO_S,    // 38

    AMS_REG_GFIFO_W,    // 39
    AMS_REG_GFIFO_E,    // 3A

    AMS_MAX_NUM_OF_REGS // 3B
} REG_NAMES;


typedef enum {
    AMS_REG_GPENTH       = AMS_REG_CONFIG_A0,
    AMS_REG_GEXTH        = AMS_REG_CONFIG_A1,
    AMS_REG_GCONFIG1     = AMS_REG_CONFIG_A2,
    AMS_REG_GCONFIG2     = AMS_REG_CONFIG_A3,

    AMS_REG_GOFFSET_N    = AMS_REG_CONFIG_A4,
    AMS_REG_GOFFSET_S    = AMS_REG_CONFIG_A5,
    AMS_REG_GPULSE       = AMS_REG_CONFIG_A6,
    AMS_REG_GOFFSET_W    = AMS_REG_CONFIG_A7,

    AMS_REG_GOFFSET_E    = AMS_REG_CONFIG_A9,
    AMS_REG_GCONFIG3     = AMS_REG_CONFIG_AA,
    AMS_REG_GCONFIG4     = AMS_REG_CONFIG_AB,

    AMS_REG_GFLVL        = AMS_REG_CONFIG_AE,
    AMS_REG_GSTATUS      = AMS_REG_CONFIG_AF,

    AMS_REG_AILTH        = AMS_REG_AILTHL,
    AMS_REG_AIHTH        = AMS_REG_AIHTHL,
    AMS_REG_CDATA        = AMS_REG_CDATAL,
    AMS_REG_RDATA        = AMS_REG_RDATAL,
    AMS_REG_GDATA        = AMS_REG_GDATAL,
    AMS_REG_BDATA        = AMS_REG_BDATAL,
} PSEUDONYM_REG_NAMES;


typedef enum {
    PBEN,
    GEN,
    PIEN,
    AIEN,
    WEN,
    PEN,
    AEN,
    PON,

    ATIME,

    WTIME,

    AILTHL,
    AIHTHL,
    AILTHH,
    AIHTHH,

    PITHL,
    PITHH,

    PPERS,

    APERS,

    WLONG,

    PPLEN,
    PPULSE,

    LDRIVE,
    RPINS,
    RINTPOL,
    PGAIN,
    AGAIN,

    PSIEN,
    CPSIEN,
    LEDBOOST,

    REVID,

    ID,
    VID,

    CPGSAT,
    PGSAT,
    PINT,
    AINT,
    PBINT,
    GINT,
    PVALID,
    AVALID,

    CDATAL,
    CDATAH,
    RDATAL,
    RDATAH,
    GDATAL,
    GDATAH,
    BDATAL,
    BDATAH,

    PDATA,

    POFFSET_NE,

    POFFSET_SW,

    PCMP,
    SAI,
    PMSK_N,
    PMSK_S,
    PMSK_W,
    PMSK_E,

    // IRBEAM
    PBIEN,
    SLEW,
    ISQZL,
    ISQZT,

    CONFIG_A1,

    ISNL,

    ISOFF,

    IPNL,

    IPOFF,

    IBT,

    ISLEN,

    PBINT2,
    PBUSY,

    // GESTURE MODE
    GPENTH,

    GEXTH,

    GFIFOTH,
    GEXMSK,
    GEXPERS,

    GGAIN,
    GLDRIVE,
    GWTIME,

    GOFFSET_N,

    GOFFSET_S,

    GPLEN,
    GPULSE,

    GOFFSET_W,

    GOFFSET_E,

    GDIMS,

    GIEN,
    GMODE,

    GFLVL,

    GFOV,
    GVALID,

    // 16 bit fields:
    AILTH_16,
    AIHTH_16,

    CDATA_16,
    RDATA_16,
    GDATA_16,
    BDATA_16,

    MAX_NUM_OF_FIELDS
} TMG399X_FIELD_NAMES;


typedef enum {
    AILTH    = AILTH_16,
    AIHTH    = AIHTH_16,

    CDATA    = CDATA_16,
    RDATA    = RDATA_16,
    GDATA    = GDATA_16,
    BDATA    = BDATA_16,
} FIELD_NAMES_16_BITS;


typedef struct {
    uint8_t   bitPos;
    uint8_t   numBits;
    REG_NAMES regName;
} AMS_FIELD_DEFS;

typedef enum {
    BOOST_100,
    BOOST_150,
    BOOST_200,
    BOOST_300,
} AMS_BOOST_DEFS;

typedef enum {
    AGAIN_1X,
    AGAIN_4X,
    AGAIN_16X,
    AGAIN_64X,
} AMS_BOOST_AGAIN;

typedef enum {
    PGAIN_1X,
    PGAIN_2X,
    PGAIN_4X,
    PGAIN_8X,
} AMS_BOOST_PGAIN;

typedef enum {
    GGAIN_1X,
    GGAIN_2X,
    GGAIN_4X,
    GGAIN_8X,
} AMS_BOOST_GGAIN;

typedef enum {
    PPLEN_4uS,
    PPLEN_8uS,
    PPLEN_16uS,
    PPLEN_32uS,
} AMS_BOOST_PPLEN;

typedef enum {
    GPLEN_4uS,
    GPLEN_8uS,
    GPLEN_16uS,
    GPLEN_32uS,
} AMS_BOOST_GPLEN;

typedef enum {
    LDRIVE_100,
    LDRIVE_50,
    LDRIVE_25,
    LDRIVE_12p5,
} AMS_BOOST_LDRIVE;

typedef enum {
    GLDRIVE_100,
    GLDRIVE_50,
    GLDRIVE_25,
    GLDRIVE_12p5,
} AMS_BOOST_GLDRIVE;

typedef enum {
    GWTIME_0mS,
    GWTIME_2p8mS,
    GWTIME_5p6mS,
    GWTIME_8p4mS,
    GWTIME_14mS,
    GWTIME_22p4mS,
    GWTIME_30p8mS,
    GWTIME_39p2mS,
} AMS_BOOST_GWTIME;

typedef enum {
    GDIMS_NSWE,
    GDIMS_NS,
    GDIMS_WE,
//    GDIMS_NSWE,
} AMS_BOOST_GDIMS;

typedef enum {
    GFIFOTH_1,
    GFIFOTH_4,
    GFIFOTH_8,
    GFIFOTH_16,
} AMS_BOOST_GFIFOTH;

typedef enum {
    GEXPERS_1,
    GEXPERS_2,
    GEXPERS_4,
    GEXPERS_7,
} AMS_BOOST_GEXPERS;


typedef struct {
    int16_t d_factor;
    int16_t r_coef;
    int16_t g_coef;
    int16_t b_coef;
    int16_t ct_coef;
    int16_t ct_offset;
} lux_segment;


#define GEXMSK_ALL           0x00
#define GEXMSK_E             0x01
#define GEXMSK_W             0x02
#define GEXMSK_S             0x04
#define GEXMSK_N             0x08
#define GEXMSK_NONE          0x0F

// Enable Register Masks
#define AMS_MSK_PON          0x01
#define AMS_MSK_AEN          0x02
#define AMS_MSK_PEN          0x04
#define AMS_MSK_WEN          0x08
#define AMS_MSK_AIEN         0x10
#define AMS_MSK_PIEN         0x20
#define AMS_MSK_GEN          0x40
#define AMS_MSK_PBEN         0x80

#define AMS_INT_CPGSAT       0x80
#define AMS_INT_PGSAT        0x40
#define AMS_INT_PINT         0x20
#define AMS_INT_AINT         0x10
#define AMS_INT_PBINT        0x08
#define AMS_INT_GINT         0x04
#define AMS_INT_PVALID       0x02
#define AMS_INT_AVALID       0x01


#define PROX_DETECT_CNT       100
#define PROX_RELEASE_CNT       50

#define MS_TO_US(t) (uint32_t)(t * 1000)
#define US_TO_MS(t) (uint32_t)(t / 1000)

#define MAX_NUM_OF_REGS_TO_RESET    (16 * 3)
#define MAX_NUM_OF_BITS              16


extern char           const regAddr[];
extern AMS_FIELD_DEFS const tmg399x[MAX_NUM_OF_FIELDS];
extern uint16_t       const bitFieldMask[];
extern uint8_t        const ams_tmg399x_alsGainVal[];
extern uint16_t       const ams_tmg399x_min_integ_time_us[];
extern lux_segment    const ams_tmg399x_lux_coef[];
extern uint8_t        const ams_power_on_reset_values[];

bool ams_validDeviceId(uint8_t deviceId);



#endif  // _AMS_TMG399X_H
