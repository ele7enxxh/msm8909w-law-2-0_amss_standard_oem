/*******************************************************************************
* Copyright (c) 2013-2015, ALPS ELECTRIC CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of ALPS ELECTRIC CO., LTD. nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
* IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/******************************************************************************
 *
 * REVISON HISTORY
 *
 * VERSION  DATE        WHO         DESCRIPTION
 * -------  ----------  --------    ------------------------------------------
 * 1.1      Feb/24/2015 ALPS        Change sensor type.
 *                                  Primarily is uv, secondarily is ambient.
 *                                  Delete "DDK" option.
 *                                  Add Dragonboard8074 debug option.
 *                                  Add calculation equation under solar light.
 * 1.0      10/30/2014  ALPS        Initial version.
 * -------  ----------  --------    ------------------------------------------
 *
 *****************************************************************************/

#ifndef ALPS_DRIVER_HSVDDD002A_H
#define ALPS_DRIVER_HSVDDD002A_H

#include "sns_ddf_driver_if.h"

/* Debug using Dragonboard8074 */
//#define _USE_DRAGONBOARD8074

/* Enable this to support secondary sensor type */
//#define HSVDDD002A_SUPPORT_SECONDARY

/* Enable this macro to view driver debug logs */
//#define HSVDDD002A_DEBUG
#define LOG_TAG "[HSVDDD], "

/*-----------------------------------------------------------------------------
 *    Register address (HSVDDD002A)
 *---------------------------------------------------------------------------*/
#define REG_STA        0x0B     /* R   : Self-Test response */
#define REG_STB        0x0C     /* R   : Self-Test response */
#define REG_MIF        0x0D     /* R   : "More Info" Register */
#define REG_WHO        0x0F     /* R   : "Who I Am" Register */
#define REG_UVA_AMB    0x10     /* R   : Output Value UV-A/Ambient Register */
#define REG_AMB        0x10     /* R   : Output Value Ambient Register */
#define REG_UVA        0x14     /* R   : Output Value UV-A Register */
#define REG_SR1        0x18     /* R   : Status Register1 */
#define REG_CS1        0x1B     /* R/W : Control setting Register1 */
#define REG_CS2        0x1C     /* R/W : Control setting Register2 */
#define REG_CS3        0x1D     /* R/W : Control setting Register3 */
#define REG_CS4        0x1E     /* R/W : Control setting Register4 */

/*-----------------------------------------------------------------------------
 *    Register bit define (HSVDDD002A)
 *---------------------------------------------------------------------------*/
#define REG_INF_TE          0x5445  /* More Info Register */

#define REG_WHO_RES         0x49

#define REG_SR1_DRDY        0x40    /* Data ready (0:Not ready, 1:Ready) */
#define REG_SR1_INT         0x10    /* Interrupt information (0:No int., 1:Int.) */

#define REG_CS1_PC1         0x80    /* Power Mode (0:Stand-by, 1:Active) */
#define REG_CS1_RS2         0x60    /* Dynamic range of output data. */
#define REG_CS1_RS1         0x60    /* Sensor input method */
#define REG_CS1_ODR         0x1C    /* Output Data Rate */
#define REG_CS1_ODR_10Hz    0x00    /* Output Data Rate (000:10Hz) */
#define REG_CS1_ODR_20Hz    0x04    /* Output Data Rate (001:20Hz) */
#define REG_CS1_ODR_50Hz    0x08    /* Output Data Rate (010:50Hz) */
#define REG_CS1_ODR_100HZ   0x0C    /* Output Data Rate (011:100Hz) */
#define REG_CS1_ODR_200HZ   0x10    /* Output Data Rate (1001:200Hz) */
#define REG_CS1_FS1         0x02    /* Functional state (0:Normal, 1:Force) */

#define REG_CS2_AMBDEN      0x40    /* Ambient Measurement Enable (0:Disable, 1:En) */
#define REG_CS2_UVADEN      0x10    /* UV-A Measurement Enable (0:Disable, 1:En) */

#define REG_CS3_SRST        0x80    /* Soft reset */
#define REG_CS3_FORCE       0x40    /* Start forced measurement */
#define REG_CS3_STCA        0x20    /* Self-test-A function */
#define REG_CS3_STCB        0x10    /* Self-test-B function */

#define REG_CS4_RNG         0xC0    /* Set amplifier gain (ratio). */

/*-----------------------------------------------------------------------------
 *    SelfTest bit define (HSVDDD002A)
 *---------------------------------------------------------------------------*/
/* Self-test resiter value */
#define STR_REG_DEF         0x55    /* Default value  */
#define STR_REG_PASS        0xAA    /* Pass value  */
#define STR_REG_X           0x01    /* X sensor offset error  */
#define STR_REG_Y           0x02    /* Y sensor offset error  */
#define STR_REG_Z           0x04    /* Z sensor offset error  */

/* Self-test error number */
#define STR_PASS            0x00    /* SelfTest is Pass  */
#define STR_ERR_I2C         0x01    /* Error : I2C communication  */
#define STR_ERR_INIT        0x02    /* Error : default register value  */
#define STR_ERR_1ST         0x03    /* Error : 1st register value  */
#define STR_ERR_2ND         0x04    /* Error : 2nd register value  */
#define STR_ERR_STA         0x10    /* SelfTest-A is error  */
#define STR_ERR_STB         0x20    /* SelfTest-B is error  */
#define STR_ERR_STBEX       0x40    /* SelfTest-BEX is error  */
#define STR_ERR_STC         0x80    /* SelfTest-C is error  */


/* Sensor type */
enum {
    HSVDDD_UVA,
    HSVDDD_AMB,

    HSVDDD_SNSNUM
};

/* Data type */
enum {
    HSVDDD_UVA_Q16,
    HSVDDD_UVB_Q16,
    HSVDDD_UVI_Q16,

    HSVDDD_UV_DATANUM
};

enum {
    HSVDDD_AMB_LX_Q16,
    HSVDDD_AMB_RAW,
    HSVDDD_AMB_LX_UI32,

    HSVDDD_AMB_DATANUM
};


/* Sensor power (uA)  */
#define HSVDDD_SENSOR_POWER_ACTIVE (2000)
#define HSVDDD_SENSOR_LOWPOWER     (5)

/* Sensor range */
/* uv-A (unit : mW/cm2)  */
#define HSVDDD_UVA_SENSOR_RANGE_MIN   (FX_FLTTOFIX_Q16(0.0f))
#define HSVDDD_UVA_SENSOR_RANGE_MAX   (FX_FLTTOFIX_Q16(4.9f))
/* Ambient (unit : lx)  */
#define HSVDDD_AMB_SENSOR_RANGE_MIN   (FX_FLTTOFIX_Q16(0.0f))
#define HSVDDD_AMB_SENSOR_RANGE_MAX   (FX_FLTTOFIX_Q16(266000.0f))

/* Sensor resolution */
/* uv-A (unit : mW/cm2)  */
#define HSVDDD_UVA_SENSOR_RESOLUTION  (0.0012f)
/* Ambient (unit : lx)  */
#define HSVDDD_AMB_SENSOR_RESOLUTION  (65.137f)

/* Sensor bit length */
#define HSVDDD_SENSOR_BIT_LEN          (13)

/* Sensor frequency */
#define HSVDDD_SENSOR_MAX_FREQ         (50)

/* Sensor processing time  */
#define HSVDDD_SENSOR_DELAY_ACTIVE     (3030)
#define HSVDDD_SENSOR_DELAY_DATA       (10000)

/* DDF infomation  */
#define HSVDDD_SENSOR_DRIVER_NAME      "ALPS UV/Ambient device driver"
#define HSVDDD_SENSOR_DRIVER_VER       (1)
#define HSVDDD_SENSOR_DEVICE_UVA_NAME  "UV-A"
#define HSVDDD_SENSOR_DEVICE_AMB_NAME  "Ambient"
#define HSVDDD_SENSOR_DEVICE_VENDOR    "ALPS ELECTRIC CO., LTD."
#define HSVDDD_SENSOR_DEVICE_MODEL     "HSVDDD002A"
#define HSVDDD_SENSOR_DEVICE_VER       1

#define HSVDDD_SENSOR_READ_DATA_NUM    (6)

#define HSVDDD_MEASURE_TIME_US         (5000)
#define HSVDDD_SOFTRESET_TIME_US       (3600)

/* Debug macro  */
#ifdef HSVDDD002A_DEBUG
    #if defined QDSP6
        //#include "qurt_elite_diag.h"
        #define DBG_MEDIUM_PRIO DBG_MED_PRIO
        #define HSVDDD002A_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "DD - " msg)
        #define HSVDDD002A_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "DD - " msg,p1)
        #define HSVDDD002A_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "DD - " msg,p1,p2)
        #define HSVDDD002A_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "DD - " msg,p1,p2,p3)
    #else /* QDSP6 */
        #define MED MEDIUM
        #include "sns_debug_str.h"
        #define HSVDDD002A_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
        #define HSVDDD002A_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
        #define HSVDDD002A_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
        #define HSVDDD002A_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
    #endif /* QDSP6 */
#else /* HSVDDD002A_DEBUG */
    #define HSVDDD002A_MSG_0(level,msg)
    #define HSVDDD002A_MSG_1(level,msg,p1)
    #define HSVDDD002A_MSG_2(level,msg,p1,p2)
    #define HSVDDD002A_MSG_3(level,msg,p1,p2,p3)
#endif /* HSVDDD002A_DEBUG */

#endif

