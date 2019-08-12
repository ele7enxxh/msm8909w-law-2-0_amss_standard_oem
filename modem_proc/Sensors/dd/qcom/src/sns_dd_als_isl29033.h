/*==========================================================================
 *
 *                      SENSORS  AMBIENT LIGHT SENSOR  DRIVER
 *
 * DESCRIPTION : Implements the driver for the Intersil ALS(Ambient Light Sensor) 
 *      This driver has 2 sub-modules:
 *      1. The common handler that provides the driver interface
 *      2. The light(ALS) driver that handles light data type
 *  
 * AUTHOR : Louis Park  @  <lpark@intersil.com> 
 *                            
 * Copyright (c) 2013 by Intersil Corporation All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the Intersil Corporation nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
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
 *
 ==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
06-24-2014   MW      Fixed polling mode, added DRI mode, added log packets

==============================================================================*/

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*==============================================================================
                    INTERNAL DEFINITIONS
==============================================================================*/
#define SNS_DD_ALS_PWR                 75
#define MODE_ARRAY_SIZE		              25
#define SNS_DD_ALS_LOW_PWR             1
#define SNS_DD_ALS_NUM_SENSOR_TYPES    2
#define SNS_DD_ALS_DATA_READY_uS       100000  /* 100ms for ALS : Rext=499KOhm and 16-bit ADC */
#define MODULE_NAME                    "isl29033"
#define ALS_DD_INTR_CLR_BIT            0x04
#define ALS_DD_RES                     FX_FLTTOFIX_Q16(0.122)   /* For 16-bit ADC and 8000 range. Unit of this data type is lux */

/* I2C slave address of this sensor is 0x44 */
/******************************************************************************
                        Intersil Sensor(ISL29033) defines
******************************************************************************/
/* Command Register 1 Addresses Defines */
#define ALS_DD_ISL_COMMAND_REG_1                0x00
#define INTR_FLG_CLR				                        0xE3

/******************************************************************************
                         Operating modes of ISL29033 defines
******************************************************************************/
# define ALS_DD_ISL_MODE_POS                    5                               /* Operating mode select D7:5 */
# define ALS_DD_ISL_MODE_SET_PWDN               (0x00 << ALS_DD_ISL_MODE_POS)   /* 000 : Power Down the Device */
# define ALS_DD_ISL_MODE_SET_RESERVED           (0x01 << ALS_DD_ISL_MODE_POS)   /* 001 : Set Device in Reserved Mode */
# define ALS_DD_ISL_MODE_SET_ALS_CONT           (0x05 << ALS_DD_ISL_MODE_POS)   /* 101 : Set Device in ALS Continuous Mode */
# define ALS_DD_ISL_MODE_SET_IR_CONT            (0x06 << ALS_DD_ISL_MODE_POS)   /* 110 : Set Device in IR Continuous Mode */
# define ALS_DD_ISL_MODE_MASK                   0xE4
# define ALS_DD_ISL_WRITE_MASK                  0x03

/******************************************************************************
                         Interrupt Persistency  for ISL29033 defines
******************************************************************************/
                                                                                /* Persistency Select D1:0 */
# define ALS_DD_ISL_INTR_PERSIST_CLEAR          0xFC                            /* MASK for Clear */
# define ALS_DD_ISL_INTR_PERSIST_SET_1          0x00                            /* 0x00 : SET INTERRUPT Persistency 1 */
# define ALS_DD_ISL_INTR_PERSIST_SET_4          0x01                            /* 0x01 : SET INTERRUPT Persistency 4 */
# define ALS_DD_ISL_INTR_PERSIST_SET_8          0x02                            /* 0x02 : SET INTERRUPT Persistency 8 */
# define ALS_DD_ISL_INTR_PERSIST_SET_16         0x03                            /* 0x03 : SET INTERRUPT Persistency 16 */
# define ALS_DD_ISL_INTR_PERSIST_MASK           0x03
# define ALS_DD_ISL_INTR_WRITE_MASK             0xE0

/******************************************************************************
                         Command Register 2 Addresses Defines
******************************************************************************/ 
# define ALS_DD_ISL_COMMAND_REG_2               0x01

/******************************************************************************
                         ADC Resolution of ISL29033 defines
******************************************************************************/
# define ALS_DD_ISL_ADC_RES_BIT_POS             2                                       /* ADC Resolution Select D3:2 */
# define ALS_DD_ISL_ADC_RES_SET_4              (0x03 << ALS_DD_ISL_ADC_RES_BIT_POS)     /* 0x03 : Set ADC Resolution 4 Bit */
# define ALS_DD_ISL_ADC_RES_SET_8              (0x02 << ALS_DD_ISL_ADC_RES_BIT_POS)     /* 0x02 : Set ADC Resolution 8 Bit */
# define ALS_DD_ISL_ADC_RES_SET_12             (0x01 << ALS_DD_ISL_ADC_RES_BIT_POS)     /* 0x01 : Set ADC Resolution 12 Bit */
# define ALS_DD_ISL_ADC_RES_SET_16             (0x00 << ALS_DD_ISL_ADC_RES_BIT_POS)     /* 0x00 : Set ADC Resolution 16 Bit */
# define ALS_DD_ISL_ADC_RES_MASK                0x0C
# define ALS_DD_ISL_ADC_RES_WRITE_MASK          0x03
#define SNS_DD_ISL_ALS_FREQ                     9

/******************************************************************************
                         Range selection for ISL29033 defines
******************************************************************************/
# define ALS_DD_ISL_RANGE_BIT_POS               0                                       /* ALS Range Select D1:0 */
# define ALS_DD_ISL_RANGE_SET_125               (0x00 << ALS_DD_ISL_RANGE_BIT_POS)      /* 0x00 : Set ALS Range 125Lux */
# define ALS_DD_ISL_RANGE_SET_500               (0x01 << ALS_DD_ISL_RANGE_BIT_POS)      /* 0x01 : Set ALS Range 500Lux */
# define ALS_DD_ISL_RANGE_SET_2000              (0x02 << ALS_DD_ISL_RANGE_BIT_POS)      /* 0x02 : Set ALS Range 2000Lux */
# define ALS_DD_ISL_RANGE_SET_8000              (0x03 << ALS_DD_ISL_RANGE_BIT_POS)      /* 0x03 : Set ALS Range 8000Lux */
# define ALS_DD_ISL_RANGE_MASK                  0x03
# define ALS_DD_ISL_RANGE_WRITE_MASK            0x0C

/******************************************************************************
                        Data Registers of ISL29033 defines
******************************************************************************/
# define ALS_DD_ISL_DATA_REG_LBYTE              0x02                            /* Data Register LSB Byte */
# define ALS_DD_ISL_DATA_REG_HBYTE              0x03                            /* Data Register MSB Byte */

/******************************************************************************
                        LOW and HIGH Threshold Register defines
******************************************************************************/
# define ALS_DD_ISL_LOW_THRES_LBYTE             0x04                            /* Low Threshold LSB Byte */
# define ALS_DD_ISL_LOW_THRES_HBYTE             0x05                            /* Low Threshold MSB Byte */
# define ALS_DD_ISL_HIGH_THRES_LBYTE            0x06                            /* High Threshold LSB Byte */
# define ALS_DD_ISL_HIGH_THRES_HBYTE            0x07                            /* High Threshold MSB Byte */
# define ALS_DD_ISL_LT_VALUE			                 0xccc				 /* Low threshold to 5% of maximum value*/
# define ALS_DD_ISL_HT_VALUE 			                0xcccc				/* high threshold to 20% of high value */

/*=============================================================================
                  TYPE DEFINITIONS
=============================================================================*/

typedef enum
{
  SNS_DD_PEND_STATE_IDLE,                                                       /* Idle State */
  SNS_DD_PEND_STATE_PEND,                                                       /* Waiting A Response */
  SNS_DD_PEND_STATE_RSVD                                                        /* Reserved State */
} sns_dd_pend_state_type;

/* timer object type that is a parameter of sns_dd_start_general_timer/sns_dd_stop_general_timer  */

typedef struct
{
  bool                  active;
  sns_ddf_timer_s       timer_obj;
} sns_dd_timer_type;

/* The State of device */
typedef enum
{
  SNS_DD_DEV_STOPPED,                                                           /*0*/
  SNS_DD_DEV_CONFIGURED,                                                        /*1*/
  SNS_DD_DEV_GETDATA,                                                           /*2*/
  SNS_DD_DEV_MAX_STATES                                                         /*3*/
}sns_dd_device_state_type;

/* The Mode of device */
typedef enum
{
  SNS_DD_DEV_UNDEF,                                                             /*0*/
  SNS_DD_DEV_POWER_DOWN,                                                        /*1*/
  SNS_DD_DEV_ALS_CONT,                                                          /*2*/
  SNS_DD_DEV_IR_CONT
}sns_dd_device_mode_type;

/* Interrupt persist value in number of integration cycles.
 * The interrupt pin and the interrupt flag are triggered 
 * when the sensor data reading is out of the interrupt 
 * threshold window after SNS_DD_DEV_PERSIST_x consecutive 
 * number of integration cycles.   
*/
typedef enum
{
  SNS_DD_DEV_PERSIST_1   = 1,
  SNS_DD_DEV_PERSIST_4   = 4,
  SNS_DD_DEV_PERSIST_8   = 8,
  SNS_DD_DEV_PERSIST_16  = 16
}sns_dd_device_persist_cycles;

/* ADC reolution in bits */
typedef enum
{
  SNS_DD_DEV_ADC_4      = 4,
  SNS_DD_DEV_ADC_8      = 8,
  SNS_DD_DEV_ADC_12     = 12,
  SNS_DD_DEV_ADC_16     = 16
}sns_dd_device_adc_resolution_bits;

/* Device Ranges - Unit lux */
typedef enum
{
  SNS_DD_DEV_RANGE_125  = 125,
  SNS_DD_DEV_RANGE_500  = 500,
  SNS_DD_DEV_RANGE_2000 = 2000,
  SNS_DD_DEV_RANGE_8000 = 8000
}sns_dd_device_range;

/* Data Structure for Light (ALS) Driver */
typedef struct
{
  uint32_t                      als_odr;
  uint32_t                      als_data;
  uint16_t                      range;                                          /* Range Value */
  uint16_t                      als_low_thres;                                  /* ALS Low Threshold */
  uint16_t                      als_high_thres;                                 /* ALS High Threshold */
  uint8_t                       adc_resolution;                                 /* ADC Resolution */
  uint8_t                       persist;                                        /* Persistency */
  sns_dd_device_mode_type       mode;                                           /* Mode of Operation */
  sns_dd_pend_state_type        pend_state;                                     /* Idle or Wait State for The Response */
} sns_dd_als_db_type;

/* common data structure belong to all sub modules */
typedef struct
{
  sns_dd_timer_type             als_timer;                                      /* Timer To wait for Data to be ready */
  sns_ddf_powerstate_e          pwr_mode;
  sns_dd_device_state_type      state;
} sns_dd_als_common_db_type;

/* State struct for ALS ISL29033 */
typedef struct
{
  sns_ddf_handle_t              smgr_handle;                                    /* SDDI Handle used to Notify Data */
  sns_ddf_handle_t              port_handle;                                    /* Handle used to access the I2C Bus */
  sns_dd_als_db_type            sns_dd_als_db;
  sns_dd_als_common_db_type     sns_dd_als_common_db;
  uint16_t                      gpio1;                                          /* GPIO number used in DRI mode */
  bool                          dri_enable;
  bool                          polling_enable;
  sns_ddf_sensor_sample_s       samples[2];
  sns_ddf_sensor_data_s         s_data;
#ifdef ISL29033_DEBUG
  uint8_t                       last_mode;
#endif /* ISL29033_DEBUG */
} sns_dd_als_state_t;

typedef enum
{
  SNS_DDF_ISL_RANGE_125,                                                        /* 0x00 = range select 125 */
  SNS_DDF_ISL_RANGE_500,                                                        /* 0x01 = range select 500 */
  SNS_DDF_ISL_RANGE_2000,                                                       /* 0x02 = range select 2000 */
  SNS_DDF_ISL_RANGE_8000                                                        /* 0x03 = range select 8000 */
 }sns_ddf_range_e;

typedef enum
{
  SNS_DDF_ISL_ADC_RES_16,                                                       /* 0x00 = ADC Resolution bit 16 */
  SNS_DDF_ISL_ADC_RES_12,                                                       /* 0x01 = ADC Resolution bit 12 */
  SNS_DDF_ISL_ADC_RES_8,                                                        /* 0x02 = ADC Resolution bit 8 */
  SNS_DDF_ISL_ADC_RES_4                                                         /* 0x03 = ADC Resolution bit 4 */
}sns_ddf_adc_res_e;

/* Enable this macro to see debug messages */
//#define ISL29033_DEBUG

#ifdef ISL29033_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ISL29033_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, msg)
#define ISL29033_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1)
#define ISL29033_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2)
#define ISL29033_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define ISL29033_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define ISL29033_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define ISL29033_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define ISL29033_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define ISL29033_MSG_0(level,msg)
#define ISL29033_MSG_1(level,msg,p1)
#define ISL29033_MSG_2(level,msg,p1,p2)
#define ISL29033_MSG_3(level,msg,p1,p2,p3)
#endif







