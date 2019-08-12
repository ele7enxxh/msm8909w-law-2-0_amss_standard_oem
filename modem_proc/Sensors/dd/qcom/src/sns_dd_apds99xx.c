/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for Avago ALS (Light) and Proximity Sensor APDS-99xx/QPDS-T9x0
   This driver has 3 sub-modules:
   1. The common handler that provides the driver interface
   2. The light(ALS) driver that handles light data type
   3. The proximity driver that handles proximity data type

********************************************************************************
* Copyright (c) 2012, Avago Technologies.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Avago nor the
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
*******************************************************************************

==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_apds99xx.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
22-Jul-2014   MW      Eliminated compiler warning
05-Jun-2014   sc      Fix expected device ID for APDS9930 in probe()
11-Nov-2013   sc      Fixed a bug that could cause DRI mode not working when disabling interrupt
29-Oct-2013   sc      Fixed a bug in copying into string literal when getting sensor info
18-Oct-2013   sc      Set dri_enabled in enable_sched_data(), instead of relying on first_gpio value
26-Aug-2013   pn      Fixed DEVICE_INFO attribute response
12-Jul-2013  lka      Added compile-time configurations
29-Apr-2013   vh      Free driver structure if init fails
18-Apr-2013   ag      Featurize debug code
06-Apr-2013   ag      Remove DRI workaround
19-Feb-2013  jtl      Added probe function
18-Jan-2013   ag      DRI workaround
10-Jan-2013   br      Support up to 20Hz for ALS & PROX in polling mode
09-Jan-2013   pn      Refactored zero-byte write function.
26-Dec-2012   pn      Updated message macros for QDSP6
05-Dec-2012   vh      Fixed sampling time issue for ALS & PROX in polling mode
03-Dec-2012   pn      Returns actual ODR only if requested ODR is greater than zero
28-Sep-2012   vh      Added memory for number of sensors for I2C stubbed out code
27-Oct-2011   KK      Avago Technologies, at Singapore, first draft
04-Feb-2012   KK      Avago Technologies, at Taipei, final (integration with Ricky Yuen)
13-Feb-2012   KK      Do nothing when polling mode in sns_dd_apds91xx_set_powerstate(SNS_DDF_POWERSTATE_ACTIVE)
17-Feb-2012   KK      Include support of Q-part number for Nokia (QPDS-T900/T930)
23-Feb-2012   KK      Tidy up code according to Bruce Oh's comments
29-Feb-2012   KK      Merge code of 17-Feb and 23-Feb
10-Mar-2012   AG      Enabled DRI
12-Mar-2012   AG      Enabled log packets
15-Mar-2012   AG      Fixed simultaneous PRX and ALS in DRI mode
21-Mar-2012   KK      Implement xtalk calibration for LGE sns_dd_apds99xx_prox_calibration,
                      add SNS_DDF_ATTRIB_PROX_CAL_FACTOR to set prox_cal_factor,
                      amend sns_dd_apds99xx_als_convert_to_mlux to avoid negative lux
21-Mar-2012   KK      Amend bright sunlight checking condition;
                      Added 10ms delay right after PEN and PON are set in prx_calibration
21-Mar-2012   KK      Revert sns_dd_apds99xx_als_convert_to_mlux which causing 0 lux at bright condition
                      Add PRX and ALS calibration in self-test routine following 80-N7859-1C document
02-Apr-2012   KK      Merged calibration with DRI chnages
08-May-2012   AG      Fix INT issue on QC hardware
11-May-2012   AG      Incorporate variable als change percentage
16-May-2012   AG      Fix polling mode
17-May-2012   PN      Renamed device driver interface to sns_dd_apds99xx_driver_if
06-08-2012    AG      Ensure registers that are not changed are programmed by default
                      for both streaming and test/calibration
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_dd_apds99xx_priv.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_memmgr.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#define CHECK_STATUS(status)        \
  if ( status != SNS_DDF_SUCCESS )  \
  {                                 \
    return status;                  \
  }

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
extern volatile boolean EnableI2C;

static uint8_t sns_dd_als_atime_tb[] = { 0xF6, 0xED, 0xDB }; /* 27.2ms,  51.68ms , 100.64ms */
static uint32_t sns_dd_als_atime_us_tb[] = { 27200, 51680, 100640 }; /* 27.2ms,  51.68ms , 100.64ms */
static uint16_t sns_dd_als_res_tb[] = { 0x2800, 0x4c00, 0x9400 };
static uint8_t sns_dd_als_adc_bit_tb[] = { 14, 15, 16 };
#define   ALSPRX_DDF_TIMER_MARGIN_US   (15000)
#ifdef ADSP_HWCONFIG_L
static uint32_t sns_dd_als_integration_time_tb[] = { 27200*2,
                                                     51680*2,
                                                     100640*2 };
#else
static uint32_t sns_dd_als_integration_time_tb[] = { 27200+ALSPRX_DDF_TIMER_MARGIN_US,
                                                     51680+ALSPRX_DDF_TIMER_MARGIN_US,
                                                     100640+ALSPRX_DDF_TIMER_MARGIN_US };
#endif

static uint8_t sns_dd_als_again_tb[] = { 1, 8, 16, 120 };
static uint8_t sns_dd_als_again_bit_tb[] = {0x00, 0x01, 0x02, 0x03 };

static const sns_ddf_sensor_e sns_dd_apds99xx_sensor_types[SNS_DD_APDS99XX_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT
};

static sns_ddf_sensor_e prx_timer = SNS_DDF_SENSOR_PROXIMITY;
static sns_ddf_sensor_e als_timer = SNS_DDF_SENSOR_AMBIENT;

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/

static sns_ddf_status_e sns_dd_apds99xx_init
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

static sns_ddf_status_e sns_dd_apds99xx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_apds99xx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

static sns_ddf_status_e sns_dd_apds99xx_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_apds99xx_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

static void sns_dd_apds99xx_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

static sns_ddf_status_e sns_dd_apds99xx_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_apds99xx_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
);

static sns_ddf_status_e sns_dd_apds99xx_calibration
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
);

static sns_ddf_status_e sns_dd_apds99xx_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
);

sns_ddf_driver_if_s sns_dd_apds99xx_driver_if =
{
  &sns_dd_apds99xx_init,
  &sns_dd_apds99xx_get_data,
  &sns_dd_apds99xx_set_attrib,
  &sns_dd_apds99xx_get_attrib,
  &sns_dd_apds99xx_handle_timer,
  &sns_dd_apds99xx_handle_irq,
  &sns_dd_apds99xx_reset,
  &sns_dd_apds99xx_calibration, /* calibration test - ALS and PRX*/
  &sns_dd_apds99xx_enable_sched_data,
  &sns_dd_apds99xx_probe
};

/*===========================================================================
FUNCTION      sns_dd_apds99xx_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_apds99xx_log(
   sns_dd_apds99xx_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1,
   uint32_t               data1_q16,
   uint32_t               data2,
   uint32_t               data2_q16,
   uint32_t               raw_data )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AVAGO;

     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();

     /* Log the sensor data */
     if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = raw_data;
      }
     else
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = data2;
       log_struct_ptr->data[3] = data2_q16;
       log_struct_ptr->data[4] = raw_data;
     }

     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
     dd_ptr->dropped_logs++;
     ALSPRX_MSG_3(ERROR,"log - %d dropped=%d err=%d", 
                  11, dd_ptr->dropped_logs, err_code);
   }
}

/*===========================================================================

  FUNCTION:   sns_dd_isl29028_int_disable

===========================================================================*/
/*!
  @brief Disable ISL29028 INT since it shares a pin with APDS99xx.

  @detail
  Resets  the driver state structure

  @param[in] handle  Handle to a driver instance.

  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned.
*/
/*=========================================================================*/
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG

#define ISL29028_I2C_ADDR 0x45
#define SNS_DD_ISL_ALSPRX_CONFIG_REG         0x01
#define SNS_DD_ISL_ALSPRX_INTERRUPT_REG      0x02
#define SNS_DD_ISL_ALSPRX_PROX_LT_REG        0x03
#define SNS_DD_ISL_ALSPRX_PROX_HT_REG        0x04
#define SNS_DD_ISL_ALSPRX_ALSIR_TH1_REG      0x05
#define SNS_DD_ISL_ALSPRX_ALSIR_TH2_REG      0x06
#define SNS_DD_ISL_ALSPRX_ALSIR_TH3_REG      0x07
#define SNS_DD_ISL_ALSPRX_ALSIR_TEST1_REG    0x0E
#define SNS_DD_ISL_ALSPRX_ALSIR_TEST2_REG    0x0F

typedef struct {
   sns_ddf_handle_t         smgr_handle;    /* SDDI handle used to notify_data */
   uint32_t                 dropped_logs;
   sns_ddf_handle_t         port_handle;    /* handle used to access the I2C bus */
   sns_dd_als_db_type       sns_dd_als_db;
   sns_dd_prx_db_type       sns_dd_prx_db;
   sns_dd_apds99xx_common_db_type sns_dd_alsprx_common_db;
} sns_dd_alsprx_state_t;

sns_ddf_status_e sns_dd_isl29028_int_disable()
{
  sns_ddf_status_e status;
  uint8_t data, out;
  sns_dd_alsprx_state_t* ptr;
  sns_ddf_i2c_config_s    i2c_config;
  sns_ddf_device_access_s device;

  device.device_select = 0;
  device.port_config.bus = SNS_DDF_BUS_I2C;
  device.port_config.bus_config.i2c  = &i2c_config;
  i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
  i2c_config.bus_acq_timeout = -1;
  i2c_config.bus_freq = 400;
  i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_config.slave_addr = ISL29028_I2C_ADDR;
  i2c_config.xfer_timeout = -1;

  status = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_alsprx_state_t));
  if (status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  status = sns_ddf_open_port(&(ptr->port_handle),
                              &(device.port_config));

  if (status != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(ptr);
    return status;
  }

  /* ISL29028a reset sequence */
  data =  0x29;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TEST2_REG,
                     &data, 1, &out);

  data =  0x0;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TEST1_REG,
                     &data, 1, &out);

  data =  0x0;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TEST2_REG,
                     &data, 1, &out);

  data =  0x0;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_CONFIG_REG,
                     &data, 1, &out);

  /* write valid information to INTERRUPT register */
  data =  0x7; //16 conversions for ALS; AND
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_INTERRUPT_REG,
                     &data, 1, &out);

  /* update thresholds so interrupts are not generated */
  data =  0xFF;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_PROX_LT_REG,
                     &data, 1, &out);

  data =  0xFF;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_PROX_HT_REG,
                     &data, 1, &out);

  data =  0xFF;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TH1_REG,
                     &data, 1, &out);

  data =  0xFF;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TH2_REG,
                     &data, 1, &out);

  data =  0xFF;
  sns_ddf_write_port(ptr->port_handle,
                     SNS_DD_ISL_ALSPRX_ALSIR_TH3_REG,
                     &data, 1, &out);

  sns_ddf_close_port(ptr->port_handle);
  sns_ddf_mfree(ptr);
  if (status != SNS_DDF_SUCCESS)
  {
     return status;
  }

  return SNS_DDF_SUCCESS;
}
#endif

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_prx_thresh

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_prx_thresh
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t                 pilt,
  uint16_t                 piht
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_w;
  uint8_t          thresh[4];

  thresh[0] = (pilt & 0xFF); /* PILTL */
  thresh[1] = (pilt >> 8);   /* PILTH */
  thresh[2] = (piht & 0xFF); /* PIHTL */
  thresh[3] = (piht >> 8);   /* PIHTH */

  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_APDS99XX_CMD_WORD|SNS_DD_APDS99XX_PILTL_ADDR,
                              (uint8_t*)&thresh[0], 4, &bytes_w);
  CHECK_STATUS(status);

#ifdef ALSPRX_DEBUG
  {
    uint8_t   i2c_read_data[4];

    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);

    ALSPRX_MSG_2(HIGH, "set_prx_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_prx_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);
  }
#endif

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_als_thresh

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_als_thresh
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t                 ailt,
  uint16_t                 aiht
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_w;
  uint8_t          thresh[4];

  thresh[0] = (ailt & 0xFF); /* AILTL */
  thresh[1] = (ailt >> 8);   /* AILTH */
  thresh[2] = (aiht & 0xFF); /* AIHTL */
  thresh[3] = (aiht >> 8);   /* AIHTH */


  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_APDS99XX_CMD_WORD|SNS_DD_APDS99XX_AILTL_ADDR,
                              (uint8_t*)&thresh[0], 4, &bytes_w);
  CHECK_STATUS(status);

#ifdef ALSPRX_DEBUG
  {
    uint8_t   i2c_read_data[4];

    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_AILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_AILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_AIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_AIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);

    ALSPRX_MSG_2(HIGH, "set_als_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_als_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);

    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);

    ALSPRX_MSG_2(HIGH, "set_als_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_als_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);
  }
#endif

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_cycle_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_set_cycle_time
(
  sns_dd_apds99xx_state_t* dd_ptr
)
{
  uint32_t max_odr;
  uint32_t odr;

  max_odr = MAX(dd_ptr->sns_dd_apds99xx_common_db.als_req_odr,
                dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr);

  if (max_odr >= 17)
  {
    /* resulting freq = 20Hz */
    odr = 20; /* Hz */
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 40000; /* us */
  }
  else if (max_odr >= 13)
  {
    /* resulting freq = 16.8Hz */
    odr = 16;
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 50000;
  }
  else if (max_odr >= 9)
  {
    /* resulting freq = 12.8Hz */
    odr = 12;
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 66667;
  }
  else if (max_odr >= 5)
  {
    /* resulting freq = 8.9Hz */
    odr = 8;
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 100000;
  }
  else if (max_odr > 0)
  {
    /* resulting freq = 4.7Hz */
    /* (max_odr > 0) && (max_odr <=5) */
    odr = 4;
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 200000;
  }
  else
  {
    odr = 0;
    dd_ptr->sns_dd_apds99xx_common_db.cycle_time = 0;
  }

  dd_ptr->sns_dd_apds99xx_common_db.als_odr = odr; /* Hz */
  dd_ptr->sns_dd_apds99xx_common_db.prx_odr = odr; /* Hz */
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_update_intg_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_update_intg_time
(
  sns_dd_apds99xx_state_t* dd_ptr,
  bool             prx_enabled,
  bool             als_enabled,
  bool             static_setting
)
{
  uint8_t          atime=0;
  uint8_t          ptime=0;
  uint8_t          wtime=0;
  uint32_t         cycle_time_us;
  uint32_t         atime_us=0;
  uint32_t         ptime_us=0;
  int32_t          wtime_us=0;
  int32_t          wtime_32=0;
  uint8_t          bytes_w;
  int8_t           res_idx;
  sns_ddf_status_e status;

  if (static_setting)
  {
    /* ATIME */
    atime  = sns_dd_als_atime_tb[dd_ptr->sns_dd_als_db.als_res_index];
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ATIME_ADDR,
                                (uint8_t*)&atime, 1, &bytes_w);
    CHECK_STATUS(status);

    /* PTIME */
    ptime  = SNS_DD_APDS99XX_2_72MS_ADC_TIME;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PTIME_ADDR,
                                (uint8_t*)&ptime, 1, &bytes_w);
    CHECK_STATUS(status);
  }
  else
  {
    /* Set ATIME, PTIME, WTIME */
    cycle_time_us = dd_ptr->sns_dd_apds99xx_common_db.cycle_time;

    /* PTIME */
    if (prx_enabled)
    {
      ptime  = SNS_DD_APDS99XX_2_72MS_ADC_TIME;
      ptime_us = 2720;
    }

    /* ATIME */
    if (als_enabled)
    {
      for (res_idx=dd_ptr->sns_dd_als_db.als_res_index; res_idx >=0; res_idx--)
      {
        atime_us = sns_dd_als_atime_us_tb[res_idx];
        if (cycle_time_us > atime_us)
        {
          ALSPRX_MSG_3(HIGH, "update_intg_time - idx=%d cycle_time=%d atime=%d", 
                       res_idx, cycle_time_us, atime_us);
          break; /* current atime works */
        }
      } /* for */
      ALSPRX_MSG_2(HIGH, "update_intg_time - %d idx=%d", 600, res_idx);
      if (res_idx >=0)
      {
        if (res_idx != dd_ptr->sns_dd_als_db.als_res_index)
        {
          /* reducing resolution and atime to get the right ODR */
          dd_ptr->sns_dd_als_db.als_res_index = res_idx;
        }
      }
      else
      {
        dd_ptr->sns_dd_als_db.als_res_index = 0; /* pick smallest atime */
        ALSPRX_MSG_1(ERROR, "update_intg_time - %d", 666);
      }
      atime = sns_dd_als_atime_tb[dd_ptr->sns_dd_als_db.als_res_index];
      atime_us = sns_dd_als_atime_us_tb[dd_ptr->sns_dd_als_db.als_res_index];
    }
    else
    {
      /* ALS will be ON for PRX support */
      atime = 0xFC; /* try reducing atime for prx only to 10.88ms */
      atime_us = 10880;
    }

    /* WTIME */
    wtime_us = (cycle_time_us - (atime_us + ptime_us));
    if (wtime_us < 0)
    {
      /* This should not happen. Make sure cycle time is computed correctly
         and atime is picked correctly. */
      wtime = 0xFF; /* minumum one cycle */
      ALSPRX_MSG_1(ERROR, "update_intg_time - wtime=%d", wtime);
    }
    else
    {
      wtime_32 = (int32_t)(256 - (wtime_us / 2720)); /* 32 bit math */
      wtime = (uint8_t)wtime_32; /* convert to 8 bits */
    }

    ALSPRX_MSG_3(ERROR, "update_intg_time - ptime=%d atime=%d wtime=%d", 
                 ptime, atime, wtime);
    ALSPRX_MSG_3(ERROR, "update_intg_time - ptime_us=%d atime_us=%d wtime_us=%d", 
                 ptime_us, atime_us, wtime_us);

    /* Program */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ATIME_ADDR,
                                &atime, 1, &bytes_w);
    CHECK_STATUS(status);

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PTIME_ADDR,
                                  &ptime, 1, &bytes_w);
    CHECK_STATUS(status);

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_WTIME_ADDR,
                                  &wtime, 1, &bytes_w);
    CHECK_STATUS(status);
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_clear_int_reg

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  sns_ddf_status_e

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_clear_int_reg
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint8_t reg_addr
)
{
  uint8_t written = 0;
  sns_ddf_status_e status;
#ifndef QDSP6
  status = sns_ddf_write_port(dd_ptr->ZBW_port_handle, 0, &reg_addr, 1, &written);
#else
  status = sns_ddf_write_port(dd_ptr->port_handle, reg_addr, NULL, 0, &written);
#endif
  ALSPRX_MSG_1(LOW, "clear_int_reg - reg_addr=0x%x", reg_addr);
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_enable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_enable_interrupt
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint16_t         i2c_data = 0x0;
  uint8_t          bytes_w;
  uint16_t         i2c_pon_data = SNS_DD_APDS99XX_PWR_ON;

  ALSPRX_MSG_3(LOW, "ENABLE INTERRUPT START    enable_interrupt - sensor=%d prx=%d als=%d",
               sensor, dd_ptr->sns_dd_prx_db.enable, dd_ptr->sns_dd_als_db.enable);


  if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef ALSPRX_DEBUG
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_STATUS_ADDR,
                               (uint8_t*)&i2c_data, 1, &bytes_w);
#endif

  if (SNS_DDF_SENSOR_PROXIMITY == sensor)
  {
    if (dd_ptr->sns_dd_prx_db.enable == TRUE)
    {
      ALSPRX_MSG_0(LOW, "ENABLE INTERRUPT END    proximity already enabled");
      return SNS_DDF_SUCCESS;
    }

#if 1
    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_pon_data, 1, &bytes_w);
    CHECK_STATUS(status);


    /* Clear interrupts */
    status = sns_dd_apds99xx_clear_int_reg(dd_ptr, SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);
#endif

    status = sns_dd_apds99xx_update_intg_time(dd_ptr, TRUE, dd_ptr->sns_dd_als_db.enable, FALSE);
    CHECK_STATUS(status);

    status = sns_dd_apds99xx_set_prx_thresh(dd_ptr, SNS_DD_APDS99XX_PILT_INIT, SNS_DD_APDS99XX_PIHT_INIT);
    CHECK_STATUS(status);

    if (dd_ptr->sns_dd_als_db.enable == FALSE)
    {
      status = sns_dd_apds99xx_set_als_thresh(dd_ptr, 0, 0xFFFF); /* don't want AINT set */
      CHECK_STATUS(status);
    }

    /* ENABLE */
    i2c_data = (SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_PRX_ENABLE|
                SNS_DD_APDS99XX_PRX_INT_ENABLE | SNS_DD_APDS99XX_WAIT_ENABLE);
    if (dd_ptr->sns_dd_als_db.enable)
    {
      i2c_data |= SNS_DD_APDS99XX_ALS_INT_ENABLE;
    }
    dd_ptr->sns_dd_prx_db.enable = TRUE;
  }
  else if (SNS_DDF_SENSOR_AMBIENT == sensor)
  {
    if (dd_ptr->sns_dd_als_db.enable == TRUE)
    {
      ALSPRX_MSG_0(LOW, "ENABLE INTERRUPT END    light already enabled");
      return SNS_DDF_SUCCESS;
    }

#if 1
    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_pon_data, 1, &bytes_w);
    CHECK_STATUS(status);


    /* Clear interrupts */
    status = sns_dd_apds99xx_clear_int_reg(dd_ptr, SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);
#endif

    status = sns_dd_apds99xx_update_intg_time(dd_ptr, dd_ptr->sns_dd_prx_db.enable, TRUE, FALSE);
    CHECK_STATUS(status);

    status = sns_dd_apds99xx_set_als_thresh(dd_ptr, 0xFFFF, 0);
    CHECK_STATUS(status);

    if (!dd_ptr->sns_dd_prx_db.enable)
    {
       status = sns_dd_apds99xx_set_prx_thresh(dd_ptr, 0, 0xFFFF); /* PINT should not be set */
        CHECK_STATUS(status);
    }

    /* ENABLE */
    i2c_data = (SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_ALS_INT_ENABLE|
                SNS_DD_APDS99XX_WAIT_ENABLE);
    if (dd_ptr->sns_dd_prx_db.enable == TRUE)
    {
      i2c_data |= (SNS_DD_APDS99XX_PRX_ENABLE|SNS_DD_APDS99XX_PRX_INT_ENABLE);
    }
    dd_ptr->sns_dd_als_db.enable = TRUE;
  }

  /* Update ENABLE bits*/
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                              (uint8_t*)&i2c_data, 1, &bytes_w);
  CHECK_STATUS(status);
  dd_ptr->enable_reg_data = i2c_data;

  status = sns_ddf_signal_register(dd_ptr->interrupt_gpio, dd_ptr,
                                   &sns_dd_apds99xx_driver_if, SNS_DDF_SIGNAL_IRQ_FALLING);

  ALSPRX_MSG_3(LOW, "ENABLE INTERRUPT END    enable_int - %d data=%d status=%d", 1100, i2c_data, status);

#if 0
  {
    uint8_t i;

    for (i=0; i <26; i++)
    {
          status = sns_ddf_read_port(dd_ptr->port_handle,
                                     SNS_DD_APDS99XX_CMD_BYTE|i,
                                     (uint8_t*)&i2c_data, 1, &bytes_w);
          ALSPRX_MSG_3(LOW, "", status, i, i2c_data);
    }
  }
#endif
  return status;

}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_disable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_disable_interrupt
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint16_t         i2c_data = SNS_DD_APDS99XX_PWR_DOWN;
  uint16_t         i2c_pon_data = SNS_DD_APDS99XX_PWR_ON;
  uint8_t          bytes_w;

  ALSPRX_MSG_2(LOW, "DISABLE INTERRUPT START    disable_int - %d sensor=%d", 1200, sensor);

  if (SNS_DDF_SENSOR_PROXIMITY == sensor)
  {
    if (dd_ptr->sns_dd_prx_db.enable == FALSE)
    {
      return SNS_DDF_SUCCESS;
    }

    if (dd_ptr->sns_dd_als_db.enable == TRUE)
    {
      i2c_data = (SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_ALS_INT_ENABLE|
                  SNS_DD_APDS99XX_WAIT_ENABLE);
    }

    dd_ptr->sns_dd_prx_db.enable = FALSE;
    dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr = 0;
    dd_ptr->sns_dd_apds99xx_common_db.prx_odr = 0;
  }
  else if (SNS_DDF_SENSOR_AMBIENT == sensor)
  {
    if (dd_ptr->sns_dd_als_db.enable == FALSE)
    {
      return SNS_DDF_SUCCESS;
    }
    if (dd_ptr->sns_dd_prx_db.enable == TRUE)
    {
      i2c_data = (SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_PRX_ENABLE|SNS_DD_APDS99XX_ALS_ENABLE|
                  SNS_DD_APDS99XX_PRX_INT_ENABLE|SNS_DD_APDS99XX_WAIT_ENABLE);
    }

    dd_ptr->sns_dd_als_db.enable = FALSE;
    dd_ptr->sns_dd_apds99xx_common_db.als_req_odr = 0;
    dd_ptr->sns_dd_apds99xx_common_db.als_odr = 0;
  }
  else
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Update Integration times and thresholds */
  if (dd_ptr->sns_dd_als_db.enable || dd_ptr->sns_dd_prx_db.enable)
  {
#if 1
    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_pon_data, 1, &bytes_w);
    CHECK_STATUS(status);

    /* Clear interrupts */
    status = sns_dd_apds99xx_clear_int_reg(dd_ptr, SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);
#endif

    status = sns_dd_apds99xx_update_intg_time(dd_ptr, dd_ptr->sns_dd_prx_db.enable,
                                              dd_ptr->sns_dd_als_db.enable, FALSE);
    CHECK_STATUS(status);

    if (dd_ptr->sns_dd_als_db.enable)
    {
        status = sns_dd_apds99xx_set_als_thresh(dd_ptr, 0xFFFF, 0);
        CHECK_STATUS(status);
    }

    if (dd_ptr->sns_dd_prx_db.enable)
    {
      status = sns_dd_apds99xx_set_prx_thresh(dd_ptr, SNS_DD_APDS99XX_PILT_INIT, SNS_DD_APDS99XX_PIHT_INIT);
      CHECK_STATUS(status);
    }

    /* Update ENABLE bits*/
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);
    dd_ptr->enable_reg_data = i2c_data;

  }
  else
  {
    /* Clear ENABLE bits*/
    i2c_data = SNS_DD_APDS99XX_PWR_ON;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);
    dd_ptr->enable_reg_data = i2c_data;

    /* Clear interrupts */
    status = sns_dd_apds99xx_clear_int_reg(dd_ptr, SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);

    /* De-register INT notification */
    status = sns_ddf_signal_deregister(dd_ptr->interrupt_gpio);
    CHECK_STATUS(status);
  }

#if 0
  {
    uint8_t i;

    for (i=0; i <26; i++)
    {
          status = sns_ddf_read_port(dd_ptr->port_handle,
                                     SNS_DD_APDS99XX_CMD_BYTE|i,
                                     (uint8_t*)&i2c_data, 1, &bytes_w);
          //CHECK_STATUS(i);
          ALSPRX_MSG_3(LOW, "disable_int - status=%d i=%d data=0x%x", status, i, i2c_data);
    }
  }
#endif

  ALSPRX_MSG_1(LOW, "DISABLE INTERRUPT END    status=%d", status);
  return status;

}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_enable_sched_data

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t*)handle;

  ALSPRX_MSG_3(LOW, "sched - %d sensor=%d enable=%d", 1000, sensor, enable);

  if(enable)
  {
      dd_ptr->dri_enabled = true;
      return sns_dd_apds99xx_enable_interrupt(dd_ptr, sensor);
  }
  else
  {
      return sns_dd_apds99xx_disable_interrupt(dd_ptr, sensor);
  }
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_als_change_pcnt

DESCRIPTION   Sets the ALS change [ercentage

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_set_als_change_pcnt
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint32_t                 als_change_pcnt
)
{
  if ((als_change_pcnt <= SNS_DD_ALSPRX_ALS_CHANGE_MIN_PCNT) ||
      (als_change_pcnt >= SNS_DD_ALSPRX_ALS_CHANGE_MAX_PCNT))
  {
    dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_change_pcnt = SNS_DD_APDS99XX_DEFAULT_ALS_CHANGE_PCNT;
  }
  else
  {
    dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_change_pcnt = als_change_pcnt;
  }
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_prx_factor

DESCRIPTION   Sets the Proximity calibration factor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_prx_factor
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e        sensor_type,
  uint16_t                prx_factor
)
{
  uint16_t   i2c_data = 0x0;
  uint8_t    bytes_w;
  sns_ddf_status_e status=SNS_DDF_SUCCESS;

  if (sensor_type == SNS_DDF_SENSOR_PROXIMITY) {
    ALSPRX_MSG_2(LOW, "set_prx_factor - sensor=%d factor=%d", sensor_type, prx_factor);

#ifdef ADSP_HWCONFIG_L
    if(prx_factor != 0)
    {
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor = prx_factor;
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near = dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor + SNS_DD_APDS99XX_PRX_THESHOLD;
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_far = dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near - 100;
    }
    else
    {
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor = prx_factor;
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near = SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD;
      dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_far = SNS_DD_APDS99XX_PRX_FAR_THRESHOLD;
    }
#else
    dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor = prx_factor;
    dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near = dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor + SNS_DD_APDS99XX_PRX_THESHOLD;
    dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_far = dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near - 100;
#endif
    dd_ptr->sns_dd_prx_db.thresh_near = dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near;
    dd_ptr->sns_dd_prx_db.thresh_far = dd_ptr->sns_dd_apds99xx_common_db.nv_db.thresh_far;

    /* PILT */
    i2c_data  = 0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  SNS_DD_APDS99XX_CMD_WORD|SNS_DD_APDS99XX_PILTL_ADDR,
                                  (uint8_t*)&i2c_data,
                                  2,
                                  &bytes_w);

    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }

    /* PIHT */
    i2c_data  = dd_ptr->sns_dd_prx_db.thresh_near;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  SNS_DD_APDS99XX_CMD_WORD|SNS_DD_APDS99XX_PIHTL_ADDR,
                                  (uint8_t*)&i2c_data,
                                  2,
                                  &bytes_w);

    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }
  }

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_start_general_timer

DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_start_general_timer
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint32_t               sensor_type,
  uint32_t duration
)
{
  sns_ddf_status_e status;

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT) {
    if ( dd_ptr->sns_dd_als_db.sns_dd_timer.active )
    {
      return;
    }

    status = sns_ddf_timer_start(dd_ptr->sns_dd_als_db.timer, duration);
    if ( status != SNS_DDF_SUCCESS )
    {
      return;
    }

    dd_ptr->sns_dd_als_db.sns_dd_timer.active = true;
  }
  else { // SNS_DDF_SENSOR_PROXIMITY
    if ( dd_ptr->sns_dd_prx_db.sns_dd_timer.active )
    {
      return;
    }

    status = sns_ddf_timer_start(dd_ptr->sns_dd_prx_db.timer, duration);
    if ( status != SNS_DDF_SUCCESS )
    {
      return;
    }

    dd_ptr->sns_dd_prx_db.sns_dd_timer.active = true;
  }
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_stop_general_timer

DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_stop_general_timer( sns_dd_apds99xx_state_t* dd_ptr, uint32_t sensor_type )
{
  if (sensor_type == SNS_DDF_SENSOR_AMBIENT || sensor_type == SNS_DDF_SENSOR__ALL ) {
    if ( true == dd_ptr->sns_dd_als_db.sns_dd_timer.active ) {
      sns_ddf_timer_cancel(dd_ptr->sns_dd_als_db.timer);
      dd_ptr->sns_dd_als_db.sns_dd_timer.active = false;
    }
  }
  if (sensor_type == SNS_DDF_SENSOR_PROXIMITY || sensor_type == SNS_DDF_SENSOR__ALL ) {
    if ( true == dd_ptr->sns_dd_prx_db.sns_dd_timer.active ) {
      sns_ddf_timer_cancel(dd_ptr->sns_dd_prx_db.timer);
      dd_ptr->sns_dd_prx_db.sns_dd_timer.active = false;
    }
  }
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_req_data

DESCRIPTION   A function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_req_data
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint32_t               sensor_type
)
{
  sns_ddf_status_e status=SNS_DDF_SUCCESS;

  if ( (sensor_type == SNS_DDF_SENSOR_AMBIENT) || (sensor_type == SNS_DDF_SENSOR_PROXIMITY) )
  {
    sns_dd_apds99xx_start_general_timer (dd_ptr, sensor_type,
                                sns_dd_als_integration_time_tb[dd_ptr->sns_dd_als_db.als_res_index]);
  }
  else
  {
    status = SNS_DDF_EFAIL;
  }

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_als_convert_to_mlux

DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None

===========================================================================*/
uint32_t sns_dd_apds99xx_als_convert_to_mlux
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t cdata,
  uint16_t irdata
)
{
  uint32_t luxValue32;
  int32_t IAC1, IAC2, IAC;

  ALSPRX_MSG_2(HIGH, "cdata = %d, irdata = %d", cdata, irdata);

  if (cdata >= sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index]) {
    luxValue32 = 30000*1000; // make the maximum return regardless 9900 or 9930
    return (luxValue32);
  }
  else if (irdata >= sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index]) {
    luxValue32 = 30000*1000; // make the maximum return regardless 9900 or 9930
    return (luxValue32);
  }

  IAC1 = (cdata - (SNS_DD_APDS99XX_ALS_COE_B*irdata)/100);
  IAC2 = ((SNS_DD_APDS99XX_ALS_COE_C*cdata)/100 - (SNS_DD_APDS99XX_ALS_COE_D*irdata)/100);

  ALSPRX_MSG_2(HIGH, "IAC1 = %d, IAC2 = %d", IAC1, IAC2);

  if (IAC1 > IAC2)
    IAC = IAC1;
  else if (IAC1 <= IAC2)
    IAC = IAC2;
  else
    IAC = 0;

  if (IAC1<0 && IAC2<0) {
    if (cdata < (sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index]/2))
      IAC = 0;
    else {
      luxValue32 = 30000*1000; // make the maximum return regardless 9900 or 9930
      return (luxValue32);
      //IAC = 0x7fffffff;	// cdata and irdata saturated
    }
  }

  luxValue32 = ((10*IAC*SNS_DD_APDS99XX_ALS_GA*SNS_DD_APDS99XX_ALS_DF)); /* multiply by 1000 and divide by 100 */
  luxValue32 /= (((272*(256-sns_dd_als_atime_tb[dd_ptr->sns_dd_als_db.als_res_index]))/100)*sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);

  if (dd_ptr->device_select == 1) { // APDS-9930/QPDS-T930
    if (dd_ptr->sns_dd_als_db.als_reduce) {
      luxValue32 = (luxValue32*60)/10;
    }
  }

  // for ALS calibration
  luxValue32 = (luxValue32 * dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_factor)/100;

  ALSPRX_MSG_2(HIGH, "GA = %d, COEB = %d", SNS_DD_APDS99XX_ALS_GA, SNS_DD_APDS99XX_ALS_COE_B);
  ALSPRX_MSG_2(HIGH, "COEC = %d, COED = %d", SNS_DD_APDS99XX_ALS_COE_C, SNS_DD_APDS99XX_ALS_COE_D);
  ALSPRX_MSG_2(HIGH, "DF = %d, luxValue32 = %d", SNS_DD_APDS99XX_ALS_DF, luxValue32);
  ALSPRX_MSG_2(HIGH, "als_res_index = %d, als_gain_index = %d", dd_ptr->sns_dd_als_db.als_res_index, dd_ptr->sns_dd_als_db.als_gain_index);

  return (luxValue32);
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_async_notify

DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_async_notify
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data_val,
  uint32_t               prx_bin ,
  uint32_t               raw_data
)
{
  uint8_t num_sensors = 1, num_samples=2;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint32_t data_lux;

  ALSPRX_MSG_2(HIGH, "async_notify - %d sensor=%d", 700, sensor_type);

  if ( sensor_type == SNS_DDF_SENSOR_AMBIENT )
  {
    /* convert to lux */
    data_lux = (uint32_t) (data_val / 1000);

    /* saturate */
    if (dd_ptr->device_select == 0) { /* APDS-9900/QPDS-T900 */
      data_lux = (data_lux > 10000) ? 10000 : data_lux;
    }
    else { /* APDS-9930/QPDS-T930 */
      data_lux = (data_lux > 30000) ? 30000 : data_lux;
    }

    /* related to floating point library removal; change to FX_CONV_Q16(val,0) */
    dd_ptr->sensor_sample[0].sample = FX_CONV_Q16(data_lux, 0);
    dd_ptr->sensor_sample[0].status = SNS_DDF_SUCCESS;

    dd_ptr->sensor_sample[1].sample = raw_data;  /* ADC output - cdata */
    dd_ptr->sensor_sample[1].status = SNS_DDF_SUCCESS;

    sns_dd_apds99xx_log(dd_ptr, sensor_type, data_val, dd_ptr->sensor_sample[0].sample, 0, 0, raw_data);

    ALSPRX_MSG_2(LOW, "async_notify - als data Lux=%d cdata=%d\r\n", data_lux, raw_data);
  }
  else
  {
    dd_ptr->sensor_sample[0].sample = FX_CONV_Q16(prx_bin, 0);  /* proximity binary */
    dd_ptr->sensor_sample[0].status = SNS_DDF_SUCCESS;

    dd_ptr->sensor_sample[1].sample = raw_data;  /* ADC output - pdata */
    dd_ptr->sensor_sample[1].status = SNS_DDF_SUCCESS;

    sns_dd_apds99xx_log(dd_ptr, sensor_type, prx_bin, dd_ptr->sensor_sample[0].sample, 0, 0, raw_data);

    ALSPRX_MSG_2(LOW, "async_notify - prx data %d %d\r\n", prx_bin, raw_data);
  }

  dd_ptr->sensor_data.status = status;
  dd_ptr->sensor_data.sensor = sensor_type;
  dd_ptr->sensor_data.num_samples = num_samples;
  if (dd_ptr->dri_enabled)
  {
    dd_ptr->sensor_data.timestamp = dd_ptr->int_timestamp;
  }
  else
  {
  dd_ptr->sensor_data.timestamp = sns_ddf_get_timestamp();
  }
  dd_ptr->sensor_data.samples = dd_ptr->sensor_sample;

  sns_ddf_smgr_notify_data(dd_ptr->smgr_handle, &dd_ptr->sensor_data, num_sensors);
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_als_rsp

DESCRIPTION   This function is called by the arbitration manager when ALS(light) data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_als_rsp
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t               raw_data,
  uint32_t               data_mlux
)
{

  uint16_t ailt;
  uint16_t aiht;
  uint16_t diff;

  if (dd_ptr->dri_enabled)
  {
    diff = PCNT_OF_RAW(dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_change_pcnt,
                       raw_data);
    ailt = (raw_data - diff);
    aiht = (raw_data + diff);
    sns_dd_apds99xx_set_als_thresh(dd_ptr, ailt, aiht);
    ALSPRX_MSG_3(HIGH, "als_rsp - raw_data=%d ailt=%d aiht=%d", 
                 raw_data, ailt, aiht);
  }

  sns_dd_apds99xx_async_notify(dd_ptr, SNS_DDF_SENSOR_AMBIENT,
                             data_mlux, 0,
                             (uint32_t) raw_data);

  dd_ptr->sns_dd_als_db.last_mlux  = data_mlux;

}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_als_raw_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  int32_t	0 success, 1 pending, -1 error

SIDE EFFECT   None

===========================================================================*/
bool sns_dd_apds99xx_als_raw_data_ready
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t cdata,
  uint16_t irdata
)
{
  uint32_t data_mlux;
  uint32_t adc_saturation_data;
  uint32_t adc_low_data;
  bool change_gain=false;
  uint8_t   i2c_data, i2c_read_data;
  uint8_t          bytes_w, bytes_rw;

  /* convert reading to mlux */
  data_mlux = sns_dd_apds99xx_als_convert_to_mlux(dd_ptr,
                                                cdata,
                                                irdata);

  /********************************************************
   * check lux value to see if a different range is needed
   *********************************************************/

  adc_saturation_data = sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index] * 90;
  adc_saturation_data = adc_saturation_data /100;

  if (!dd_ptr->sns_dd_als_db.als_reduce) {
    adc_low_data = sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index] * 10;
    adc_low_data = adc_low_data /100;
  }
  else {
    adc_low_data = sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index] * 9;
    adc_low_data = adc_low_data /100;
  }

  if ( (cdata >= adc_saturation_data) || (cdata < adc_saturation_data && ((data_mlux/1000) >= dd_ptr->sns_dd_als_db.saturated_lux)) )
  {
    /* lower gain if possible */
    if (dd_ptr->sns_dd_als_db.als_gain_index != SNS_DD_ALS_GAIN_1X) {
      dd_ptr->sns_dd_als_db.als_gain_index--;
      change_gain = true;

      ALSPRX_MSG_1(LOW, "raw_data_ready - gain decrease to %d\r\n",
                   sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);

    }
    else if (dd_ptr->device_select == 1) {  // APDS-9930/QPDS-T930
      if (!dd_ptr->sns_dd_als_db.als_reduce) {
        i2c_data  = SNS_DD_APDS99XX_ALS_REDUCE;
        sns_ddf_write_port(dd_ptr->port_handle,
			            SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONFIG_ADDR,
				    (uint8_t*)&i2c_data,
			            1,
			            &bytes_w);

	dd_ptr->sns_dd_als_db.als_reduce = true;

        ALSPRX_MSG_1(LOW, "raw_data_ready - gain decrease 6x from %d\r\n",
                     sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);

        return false; // changing GAIN, don't inform SMGR
      }
    }
  }
  else if (cdata < adc_low_data)
  {
    /* increase gain if possible */
    if (dd_ptr->device_select == 0) { // APDS-9900/QPDS-T900
      if (dd_ptr->sns_dd_als_db.als_gain_index != SNS_DD_ALS_GAIN_120X) {
        dd_ptr->sns_dd_als_db.als_gain_index++;
        change_gain = true; // changing GAIN, don't inform SMGR

        ALSPRX_MSG_1(LOW, "raw_data_ready - gain increase to %d\r\n",
                     sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);
      }
    }
    else {  // APDS-9930/QPDS-T930
      if ( dd_ptr->sns_dd_als_db.als_reduce )
      {
        i2c_data  = 0;
        sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONFIG_ADDR,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);

        dd_ptr->sns_dd_als_db.als_reduce = false;

        ALSPRX_MSG_1(LOW, "raw_data_ready - gain increase to %d\r\n",
                     sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);

        return false; // changing GAIN, don't inform SMGR, kk 03-Sep-2012
      }
      else if (dd_ptr->sns_dd_als_db.als_gain_index != SNS_DD_ALS_GAIN_120X) {
        dd_ptr->sns_dd_als_db.als_gain_index++;
        change_gain = true; // changing GAIN, don't inform SMGR

        ALSPRX_MSG_1(LOW, "raw_data_ready - gain increase to %d\r\n",
                     sns_dd_als_again_tb[dd_ptr->sns_dd_als_db.als_gain_index]);
      }
    }
  }

  if (change_gain)
  {
     sns_ddf_read_port(dd_ptr->port_handle,
                             SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONTROL_ADDR,
                             (uint8_t*)&i2c_read_data, 1, &bytes_rw);

    i2c_data  = (i2c_read_data & 0xFC);
    i2c_data  = (i2c_data | sns_dd_als_again_bit_tb[dd_ptr->sns_dd_als_db.als_gain_index]);

    sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONTROL_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);

    ALSPRX_MSG_2(ERROR, "raw_data_ready - %d %d", 850, 600);

    return false;	// changing GAIN, don't update DSPS first, wait for next cycle
  }
  else {
    // report to DSPS as the lux range is ok
    sns_dd_apds99xx_als_rsp(dd_ptr, cdata, data_mlux);
  }

  return true;	// ok to update DSPS
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_prx_rsp

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_prx_rsp
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t               pdata
)
{
  if ( pdata >= dd_ptr->sns_dd_prx_db.thresh_near )
  {
    ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 1);
    if ( dd_ptr->sns_dd_als_db.irdata < (sns_dd_als_res_tb[dd_ptr->sns_dd_als_db.als_res_index]) ) /* just in case bright sunlight will false trigger it */
    {
      if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_NEAR_BY )
      {
        dd_ptr->sns_dd_prx_db.prx_detection_state = SNS_PRX_NEAR_BY;
      }
      dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_NEAR_BY;

      if (dd_ptr->dri_enabled)
      {
        ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 3);
        sns_dd_apds99xx_set_prx_thresh(dd_ptr,
                                       SNS_DD_APDS99XX_PILT_NEAR,
                                       SNS_DD_APDS99XX_PIHT_NEAR);
      }
    }
    else
    {
      if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_FAR_AWAY )
      {
        dd_ptr->sns_dd_prx_db.prx_detection_state = SNS_PRX_FAR_AWAY;
        dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_FAR_AWAY;
        sns_dd_apds99xx_set_prx_thresh(dd_ptr,
                                       SNS_DD_APDS99XX_PILT_FAR,
                                       SNS_DD_APDS99XX_PIHT_FAR);

      }
    }
  }
  else if ( pdata < dd_ptr->sns_dd_prx_db.thresh_far )
  {
    ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 2);
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_FAR_AWAY )
    {
      dd_ptr->sns_dd_prx_db.prx_detection_state = SNS_PRX_FAR_AWAY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_FAR_AWAY;

    if (dd_ptr->dri_enabled)
    {
       ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 4);
       sns_dd_apds99xx_set_prx_thresh(dd_ptr,
                                      SNS_DD_APDS99XX_PILT_FAR,
                                      SNS_DD_APDS99XX_PIHT_FAR);
    }
  }
}
/*===========================================================================

FUNCTION      sns_dd_apds99xx_prxcomm_rsp

DESCRIPTION   This function is called by the arbitration manager when proximity
              data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_prxcomm_rsp
(
  sns_dd_apds99xx_state_t* dd_ptr,
  uint16_t               pdata
)
{
  *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache = 0;

  sns_dd_apds99xx_prx_rsp(dd_ptr, pdata); /* binary decision */
  sns_dd_apds99xx_async_notify(dd_ptr,
                                 SNS_DDF_SENSOR_PROXIMITY,
                                 *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache,
                                 dd_ptr->sns_dd_prx_db.prx_detection_state,
                                 (uint32_t) pdata);
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_handle_device_indication

DESCRIPTION   This function is called by the arbitration manager when proximity
              data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_apds99xx_handle_device_indication
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e         sensor_type,
  bool                     second_type
)
{

  uint8_t   i2c_read_data[6];
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t   bytes_rw;
  uint16_t  cdata, irdata, pdata;
  int32_t   als_update_flag=0;
  uint8_t   i2c_data;
  uint32_t  data_mlux;

  ALSPRX_MSG_2(LOW, "handle_dev_ind - %d %d", 800, 1);

  if (second_type == FALSE)
  {
    /* read cdata, irdata and pdata */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_WORD|SNS_DD_APDS99XX_CDATAL_ADDR,
                               (uint8_t*)&i2c_read_data, 6, &bytes_rw);

    if ( status != SNS_DDF_SUCCESS )
    {
      ALSPRX_MSG_2(LOW, "handle_dev_ind - %d %d", 800, 2);
      return;
    }

  cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);
  irdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);
  pdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);

    ALSPRX_MSG_3(LOW, "handle_dev_ind - cdata=%x, irdata=%x, pdata=%x\r\n", cdata, irdata, pdata);


    /* keep a copy */
    dd_ptr->sns_dd_als_db.cdata = cdata;
    dd_ptr->sns_dd_als_db.irdata = irdata;
    dd_ptr->sns_dd_prx_db.pdata = pdata;
  }

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)  {
    if (dd_ptr->sns_dd_apds99xx_common_db.als_calibration) {

      dd_ptr->sns_dd_apds99xx_common_db.als_calibration = false;
      // complete
      data_mlux = sns_dd_apds99xx_als_convert_to_mlux(dd_ptr, dd_ptr->sns_dd_als_db.cdata, dd_ptr->sns_dd_als_db.irdata);
      data_mlux = data_mlux/1000; // convert mlux to lux

      ALSPRX_MSG_1(LOW, "handle_dev_ind - lux =%d\r\n", data_mlux);

      if ( (data_mlux <= (SNS_DD_ALS_CALIBRATED_LUX*110)/100) &&
          (data_mlux >= (SNS_DD_ALS_CALIBRATED_LUX*90)/100)) {

        dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_factor = 100;
        sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_SUCCESS, 0);
        sns_ddf_smgr_notify_event(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
      }
      else if ( (data_mlux >= (SNS_DD_ALS_CALIBRATED_LUX*60)/100) &&
                 (data_mlux <= (SNS_DD_ALS_CALIBRATED_LUX*140)/100) ) {

        dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_factor = (SNS_DD_ALS_CALIBRATED_LUX * 100)/data_mlux;
        sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_SUCCESS, 0);
        sns_ddf_smgr_notify_event(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
      }
      else {
        sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EFAIL, SNS_DD_APDS99XX_ALS_CAL_FAILED);
      }
    }
    else if ((dd_ptr->dri_enabled) ||
             ((!(dd_ptr->dri_enabled)) && (dd_ptr->sns_dd_apds99xx_common_db.als_get_data))) {
        /* indicate the data to the light(ALS) sub-driver */
        als_update_flag = sns_dd_apds99xx_als_raw_data_ready(dd_ptr,
                                                             dd_ptr->sns_dd_als_db.cdata,
                                                             dd_ptr->sns_dd_als_db.irdata);
        if (als_update_flag) {
            dd_ptr->sns_dd_apds99xx_common_db.als_get_data = false;
        }
    }

  }
  else {  // SNS_DDF_SENSOR_PROXIMITY
    if (dd_ptr->sns_dd_apds99xx_common_db.prx_calibration) {
      dd_ptr->sns_dd_apds99xx_common_db.prx_calibration = false;

      if (++dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_loop < 10) {
        dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata += dd_ptr->sns_dd_prx_db.pdata;
        dd_ptr->sns_dd_apds99xx_common_db.prx_calibration = true;
        sns_dd_apds99xx_req_data (dd_ptr, sensor_type);
        return; // continue getting data
      }
      else {
        // complete
        dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata /= dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_loop;
        if (dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata > SNS_DD_APDS99XX_PRX_CAL_THRESHOLD) {

#ifdef ADSP_HWCONFIG_L
          sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EFAIL, dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata);
#else
          sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EFAIL, SNS_DD_APDS99XX_PRX_CAL_FAILED);
#endif
        }
        else {
          sns_dd_apds99xx_set_prx_factor(dd_ptr, sensor_type, dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata);
#ifdef ADSP_HWCONFIG_L
          sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_SUCCESS, dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata);
#else
          sns_ddf_smgr_notify_test_complete(dd_ptr->smgr_handle, sensor_type, SNS_DDF_SUCCESS, 0);
#endif
          sns_ddf_smgr_notify_event(dd_ptr->smgr_handle, sensor_type, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
        }
      }
    }
    else if ((dd_ptr->dri_enabled) ||
             ((!dd_ptr->dri_enabled) && (dd_ptr->sns_dd_apds99xx_common_db.prx_get_data))) {
        dd_ptr->sns_dd_apds99xx_common_db.prx_get_data = false;
        /* indicate the data to the proximity common handler */
        sns_dd_apds99xx_prxcomm_rsp (dd_ptr, dd_ptr->sns_dd_prx_db.pdata);
      }
  }

  ALSPRX_MSG_2(LOW, "handle_dev_ind - %d %d", 800, 3);

  if (!dd_ptr->dri_enabled) {
    if (sensor_type ==  SNS_DDF_SENSOR_AMBIENT) {
      if ((!als_update_flag)) {
        sns_dd_apds99xx_start_general_timer (dd_ptr, SNS_DDF_SENSOR_AMBIENT,
                                             sns_dd_als_integration_time_tb[dd_ptr->sns_dd_als_db.als_res_index]);
        return;
      }
      dd_ptr->sns_dd_als_db.enable = false;
    }
    else {
      dd_ptr->sns_dd_prx_db.enable = false;
    }
    i2c_data = dd_ptr->enable_reg_data;
    sns_ddf_write_port(dd_ptr->port_handle,
                       SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                       (uint8_t*)&i2c_data,
                       1,
                       &bytes_rw);
  }

  ALSPRX_MSG_2(LOW, "handle_dev_ind - %d %d", 800, 10);
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_handle_irq

===========================================================================*/
/*!
  @brief Interrupt handler

  @detail
  Called in response to an interrupt for this driver.

  @see sns_ddf_driver_if_s.handle_irq()

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
static void sns_dd_apds99xx_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t*)handle;
  sns_ddf_status_e         status = SNS_DDF_SUCCESS;
  uint8_t                  bytes_rw;
  uint8_t                  i2c_data = 0x0;
  uint8_t                  disable_data=SNS_DD_APDS99XX_PWR_ON;
  uint8_t                  reg_addr=0;

  ALSPRX_MSG_2(LOW, "HANDLE IRQ START     irq - %d gpio_num=%d", 600, gpio_num);

  /* STATUS */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_STATUS_ADDR,
                             (uint8_t*)&i2c_data, 1, &bytes_rw);
  if(status != SNS_DDF_SUCCESS)
  {
    ALSPRX_MSG_3(LOW, "irq - %d %d port read result=%d", 600, 5, status);
    return;
  }

  ALSPRX_MSG_3(LOW, "irq - %d %d status=%d", 600, 1, i2c_data);

  dd_ptr->int_timestamp = timestamp;

  /* is this really needed? */
  /* Turn OFF all ENABLE bits first */
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                              (uint8_t*)&disable_data,
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    return;
  }

  /* Check which interrupts occured */
  if ((i2c_data & SNS_DD_APDS99XX_PINT_STATUS ) && dd_ptr->sns_dd_prx_db.enable)
  {
    /* Proximity Interrupt */
    ALSPRX_MSG_2(LOW, "irq - prox %d %d", 600, 3);
    sns_dd_apds99xx_handle_device_indication(dd_ptr,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             FALSE);
    reg_addr = SNS_DD_APDS99XX_CMD_CLR_PS_INT;

  }

  if ((i2c_data & SNS_DD_APDS99XX_AINT_STATUS) && dd_ptr->sns_dd_als_db.enable)
  {
    /* ALS Interrupt */
    ALSPRX_MSG_2(LOW, "irq - als %d %d", 600, 2);
    sns_dd_apds99xx_handle_device_indication(dd_ptr,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             reg_addr);
    if (reg_addr != 0)
    {
      reg_addr = SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT;
    }
    else
    {
      reg_addr = SNS_DD_APDS99XX_CMD_CLR_ALS_INT;
    }
  }

#ifdef ALSPRX_DEBUG
    if ((i2c_data & SNS_DD_APDS99XX_PINT_STATUS ) && !(dd_ptr->sns_dd_prx_db.enable))
      MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - PINT set, but PRX not enabled");

    if ((i2c_data & SNS_DD_APDS99XX_AINT_STATUS) && !(dd_ptr->sns_dd_als_db.enable))
      MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - AINT set, but ALS not enabled");
#endif

  /* Clear interrupts */
  if (reg_addr == 0)
  {
    reg_addr = SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT;
  }
  status = sns_dd_apds99xx_clear_int_reg(dd_ptr, reg_addr);

  ALSPRX_MSG_3(LOW, "irq - clear int - %d addr=%d result=%d", 
               600, reg_addr, status);

  /* Enable Sensor */
  i2c_data = dd_ptr->enable_reg_data;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                              (uint8_t*)&i2c_data,
                              1,
                              &bytes_rw);
  ALSPRX_MSG_3(LOW, "irq - enable - %d data=%d result=%d",
               600, i2c_data, status);
  if ( status != SNS_DDF_SUCCESS )
  {
    return;
  }

  ALSPRX_MSG_2(LOW, "HANDLE IRQ END     irq - done %d %d", 600, 20);
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is
         the callback function submitted when initializing a timer.

  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] arg        The argument submitted when the timer was set.

  @return None
*/
/*=========================================================================*/
static void sns_dd_apds99xx_handle_timer
(
  sns_ddf_handle_t handle,
  void* arg
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t*)handle;
  sns_ddf_sensor_e       sensor_type=*(sns_ddf_sensor_e*)arg;

  ALSPRX_MSG_2(LOW, "handle_timer - %d in sensor=%d", 500, sensor_type);

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT) {
    if ( false == dd_ptr->sns_dd_als_db.sns_dd_timer.active ) return; // when sensor_type is als
  }
  else {  // SNS_DDF_SENSOR_PROXIMITY
    if ( false == dd_ptr->sns_dd_prx_db.sns_dd_timer.active ) return; // when sensor_type is prx
  }

  ALSPRX_MSG_2(LOW, "handle_timer - %d sensor=%d", 500, sensor_type);

  sns_dd_apds99xx_stop_general_timer(dd_ptr, sensor_type);

  if (((sensor_type == SNS_DDF_SENSOR_AMBIENT) &&
       ((dd_ptr->sns_dd_apds99xx_common_db.als_get_data) || (dd_ptr->sns_dd_apds99xx_common_db.als_calibration))) ||
      ((sensor_type == SNS_DDF_SENSOR_PROXIMITY) &&
       ((dd_ptr->sns_dd_apds99xx_common_db.prx_get_data) || (dd_ptr->sns_dd_apds99xx_common_db.prx_calibration)))) {
    sns_dd_apds99xx_handle_device_indication(dd_ptr, sensor_type, FALSE);
    }

}

/*==============================================================================

FUNCTION      sns_dd_apds99xx_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_apds99xx_als_init (sns_dd_apds99xx_state_t* dd_ptr)
{
  sns_dd_als_db_type *als_db_ptr = &dd_ptr->sns_dd_als_db;

  als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX;
  als_db_ptr->als_res_index     = SNS_DD_ALSPRX_RES_14BIT;
  als_db_ptr->als_gain_index    = SNS_DD_ALS_GAIN_8X;

  als_db_ptr->enable            = false;
  als_db_ptr->sns_dd_timer.active = false;

  if (dd_ptr->device_select == 0)
    dd_ptr->sns_dd_als_db.saturated_lux = SNS_DD_APDS99XX_ALS_9900_MAX_LUX;
  else {
    als_db_ptr->als_reduce = false;
    als_db_ptr->saturated_lux = SNS_DD_APDS99XX_ALS_9930_MAX_LUX;
  }

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_apds99xx_prx_init

DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_apds99xx_prx_init ( sns_dd_apds99xx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN;
  prx_db_ptr->thresh_near = dd_ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor + SNS_DD_APDS99XX_PRX_THESHOLD;  // SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD;
  prx_db_ptr->thresh_far = prx_db_ptr->thresh_near - 100; // SNS_DD_APDS99XX_PRX_FAR_THRESHOLD;

  prx_db_ptr->enable     = false;
  prx_db_ptr->sns_dd_timer.active = false;

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_apds99xx_common_init

DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/

sns_ddf_status_e sns_dd_apds99xx_common_init ( sns_dd_apds99xx_state_t* dd_ptr )
{
  dd_ptr->sns_dd_apds99xx_common_db.prx_odr = 0;
  dd_ptr->sns_dd_apds99xx_common_db.als_odr = 0;
  dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr = 0;
  dd_ptr->sns_dd_apds99xx_common_db.als_req_odr = 0;

  dd_ptr->sns_dd_apds99xx_common_db.state = SNS_DD_DEV_STOPPED;

  dd_ptr->sns_dd_apds99xx_common_db.als_get_data = false;
  dd_ptr->sns_dd_apds99xx_common_db.prx_get_data = false;
  dd_ptr->sns_dd_apds99xx_common_db.prx_calibration = false;
  dd_ptr->sns_dd_apds99xx_common_db.als_calibration = false;

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_apds99xx_init_data_struct
(
  sns_dd_apds99xx_state_t* dd_ptr
)
{
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

  /* Initialize the common database */
  if ( (init_status = sns_dd_apds99xx_common_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  if ( (init_status = sns_dd_apds99xx_als_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  if ( (init_status = sns_dd_apds99xx_prx_init(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  return init_status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_powerstate

DESCRIPTION   Sets the power state of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_powerstate
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e     sensor_type,
  sns_ddf_powerstate_e   op_mode,
  bool                init_data
)
{
  uint8_t   i2c_data = 0x0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_w;

  if (EnableI2C != TRUE)
    return SNS_DDF_SUCCESS;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
    /* ENABLE=PON */
    i2c_data = SNS_DD_APDS99XX_PWR_ON;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    CHECK_STATUS(status);

    /* Clear interrupts */
    status = sns_dd_apds99xx_clear_int_reg(dd_ptr, SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);

    /* PERS */
    i2c_data  = SNS_DD_APDS99XX_PPERS_2|SNS_DD_APDS99XX_APERS_2;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PERS_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);

    /* PPCOUNT */
    i2c_data  = SNS_DD_APDS99XX_PRX_PULSE_NUMBER;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PPCOUNT_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);

    // kk 03-Sep-2012 /* to reset ALSREDUCE */
    /* CONFIG */
    i2c_data  = 0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONFIG_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);

    /* CONTROL */
    if (dd_ptr->device_select == 0) {
      i2c_data  = (sns_dd_als_again_bit_tb[dd_ptr->sns_dd_als_db.als_gain_index]|SNS_DD_APDS99XX_PRX_IR_DIOD|SNS_DD_APDS99XX_PDRVIE_12_5MA);
      }
      else {
      i2c_data  = (sns_dd_als_again_bit_tb[dd_ptr->sns_dd_als_db.als_gain_index]|SNS_DD_APDS99XX_PRX_IR_DIOD|SNS_DD_APDS99XX_PDRVIE_100MA|SNS_DD_APDS99XX_PGAIN_4X);
      }

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_CONTROL_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    CHECK_STATUS(status);

    status = sns_dd_apds99xx_update_intg_time(dd_ptr, FALSE, FALSE, TRUE);
    CHECK_STATUS(status);
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    i2c_data  = SNS_DD_APDS99XX_PWR_DOWN;

    if (sensor_type == SNS_DDF_SENSOR__ALL) {
        if (init_data == true)
        {
            /* cleanup */
            status = sns_dd_apds99xx_init_data_struct(dd_ptr);
          CHECK_STATUS(status);
        }
    }
    else if (sensor_type == SNS_DDF_SENSOR_AMBIENT) {
        if (dd_ptr->sns_dd_prx_db.enable) {
            i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_PRX_ENABLE;
            if (dd_ptr->dri_enabled)
            {
              i2c_data |= (SNS_DD_APDS99XX_PRX_INT_ENABLE|SNS_DD_APDS99XX_WAIT_ENABLE);
        }
        }

        dd_ptr->sns_dd_als_db.enable = false;
    }
    else { /* SNS_DDF_SENSOR_PROXIMITY */
        if (dd_ptr->sns_dd_als_db.enable) {
            i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE;
            if (dd_ptr->dri_enabled)
            {
              i2c_data |= (SNS_DD_APDS99XX_ALS_INT_ENABLE|SNS_DD_APDS99XX_WAIT_ENABLE);
        }
        }

        dd_ptr->sns_dd_prx_db.enable = false;
    }

    if  ((!dd_ptr->dri_enabled) && (i2c_data == SNS_DD_APDS99XX_PWR_DOWN))
    {
      sns_dd_apds99xx_stop_general_timer( dd_ptr, sensor_type ); /* stop any pending timers */
    }

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    CHECK_STATUS(status);
    dd_ptr->enable_reg_data = i2c_data;
  }

  dd_ptr->sns_dd_apds99xx_common_db.pwr_mode = op_mode;
  dd_ptr->sns_dd_apds99xx_common_db.state = SNS_DD_DEV_CONFIGURED;

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_odr

DESCRIPTION   Sets ODR of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_odr
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e         sensor_type,
  uint32_t                 odr
)
{
  sns_ddf_status_e status=SNS_DDF_SUCCESS;
  bool             change=TRUE;

  switch ( sensor_type )
  {
    case SNS_DDF_SENSOR_AMBIENT:
      if (dd_ptr->sns_dd_apds99xx_common_db.als_req_odr == odr)
      {
        change = FALSE;
      }
      dd_ptr->sns_dd_apds99xx_common_db.als_req_odr = odr;
      break;

    case SNS_DDF_SENSOR_PROXIMITY:
      if (dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr == odr)
    {
        change = FALSE;
    }
      dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr = odr;
      break;

  default:
      status = SNS_DDF_EFAIL;
  }

  if (change)
  {
    sns_dd_apds99xx_set_cycle_time(dd_ptr);
  }

  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_set_resolution

DESCRIPTION   Sets the resolution of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds99xx_set_resolution
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  sns_dd_alsprx_res_e    resolution
)
{
  uint16_t   i2c_data = 0x0;
  uint8_t    bytes_w;
  sns_ddf_status_e status=SNS_DDF_EFAIL;

  switch ( sensor_type )
  {
    case SNS_DDF_SENSOR_AMBIENT:
      if (resolution == sns_dd_als_adc_bit_tb[SNS_DD_ALSPRX_RES_14BIT])  {
        i2c_data  = sns_dd_als_atime_tb[SNS_DD_ALSPRX_RES_14BIT];
        dd_ptr->sns_dd_als_db.als_res_index = SNS_DD_ALSPRX_RES_14BIT;
      }
      else if (resolution == sns_dd_als_adc_bit_tb[SNS_DD_ALSPRX_RES_15BIT]) {
        i2c_data  = sns_dd_als_atime_tb[SNS_DD_ALSPRX_RES_15BIT];
        dd_ptr->sns_dd_als_db.als_res_index = SNS_DD_ALSPRX_RES_15BIT;
      }
      else {  // SNS_DD_ALSPRX_RES_16BIT
        i2c_data  = sns_dd_als_atime_tb[SNS_DD_ALSPRX_RES_16BIT];
        dd_ptr->sns_dd_als_db.als_res_index = SNS_DD_ALSPRX_RES_16BIT;
      }

      status = sns_ddf_write_port(dd_ptr->port_handle,
                               SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ATIME_ADDR,
                               (uint8_t*)&i2c_data,
                               1,
                               &bytes_w);

      break;

  default:
      status = SNS_DDF_EFAIL;
  }


  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  Resets  the driver state structure

  @param[in] handle  Handle to a driver instance.

  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_apds99xx_reset
(
  sns_ddf_handle_t dd_handle
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t *)dd_handle;
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

  ALSPRX_MSG_1(LOW, "reset - %d", 400);
  dd_ptr->driver_state=6;

  if ( dd_handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( (init_status = sns_dd_apds99xx_init_data_struct(dd_ptr)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
  init_status = sns_dd_isl29028_int_disable();
  ALSPRX_MSG_2(LOW, "reset - %d int disable result=%d", 0, init_status);
#endif

  if ( (init_status = sns_dd_apds99xx_set_powerstate(dd_ptr, SNS_DDF_SENSOR__ALL,
                          SNS_DDF_POWERSTATE_LOWPOWER, false)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  dd_ptr->driver_state=10;

  return init_status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to
   associated devices, configures the driver and devices, and places
   the devices in the default power state. Returns the instance handle along
   with a list of supported sensors. This function will be called at init
   time.

   @param[out] dd_handle_ptr  Pointer that this function must malloc and
                              populate. This is a handle to the driver
                              instance that will be passed in to all other
                              functions. NB: Do not use @a memhandler to
                              allocate this memory.
   @param[in]  smgr_handle    Handle used to identify this driver when it
                              calls into Sensors Manager functions.
   @param[in]  nv_params      NV parameters retrieved for the driver.
   @param[in]  device_info    Access info for physical devices controlled by
                              this driver. Used to configure the bus
                              and talk to the devices.
   @param[in]  num_devices    Number of elements in @a device_info.
   @param[in]  memhandler     Memory handler used to dynamically allocate
                              output parameters, if applicable. NB: Do not
                              use memhandler to allocate memory for
                              @a dd_handle_ptr.
   @param[out] sensors        List of supported sensors, allocated,
                              populated, and returned by this function.
   @param[out] num_sensors    Number of elements in @a sensors.

   @return Success if @a dd_handle_ptr was allocated and the driver was
           configured properly. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_apds99xx_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{

  sns_ddf_status_e       stat = SNS_DDF_SUCCESS;
  sns_dd_apds99xx_state_t* ptr;
  uint8_t     id_value;
  uint8_t     bytes_rw;
  sns_dd_nv_db_type* nv_ptr;

  ALSPRX_MSG_1(LOW, "==> sns_dd_apds99xx_init %x\r\n", device_info->device_select);

  if ( (dd_ptr == NULL)      ||
       (num_sensors == NULL) ||
       (sensors == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_apds99xx_state_t)) != SNS_DDF_SUCCESS )
  {
    return SNS_DDF_ENOMEM;
  }
  ptr->smgr_handle = smgr_handle;

  /* open normal port */
  if ( (stat = sns_ddf_open_port(&ptr->port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS )
  {
    sns_ddf_mfree(ptr);
    return stat;
  }

#ifndef QDSP6
  /* open additional port for zero byte write(ZBW) */
  device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
  if ( (stat = sns_ddf_open_port(&ptr->ZBW_port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS )
  {
    return stat;
  }
#endif

  *dd_ptr = (sns_ddf_handle_t)ptr;

  ptr->interrupt_gpio = device_info->first_gpio;
  ptr->dri_enabled= FALSE;
  ptr->driver_state = 1;

  /* timer for ALS */
  stat = sns_ddf_timer_init(&ptr->sns_dd_als_db.timer,
                            *dd_ptr,
                            &sns_dd_apds99xx_driver_if,
                            (void *)&als_timer,
                            false);
  if ( stat != SNS_DDF_SUCCESS )
  {
    sns_ddf_mfree(ptr);
    return stat;
  }

  /* timer for PROX */
  stat = sns_ddf_timer_init(&ptr->sns_dd_prx_db.timer,
                            *dd_ptr,
                            &sns_dd_apds99xx_driver_if,
                            (void *)&prx_timer,
                            false);

  if ( stat != SNS_DDF_SUCCESS )
  {
    sns_ddf_mfree(ptr);
    return stat;
  }

  /* determine if APDS-990x family or APDS-993x family */
#ifndef ADSP_HWCONFIG_L
  ptr->device_select = device_info[0].device_select;
#endif
  /* 0 = APDS-9900/QPDS-T900 */
  /* 1 = APDS-9930/QPDS-T930 */

  /* temporary used for testing - REMOVE after used */
//  ptr->device_select = 1; /* testing on APDS-9930/QPDS-T930 */

  /* Fill out supported sensor info */
    *num_sensors = SNS_DD_APDS99XX_NUM_SENSOR_TYPES;
    *sensors = (sns_ddf_sensor_e *)sns_dd_apds99xx_sensor_types;

#if 1
if (EnableI2C == TRUE)
{
  /* Check ID Register if it is APDS-9900/QPDS-T900 */
  stat = sns_ddf_read_port(ptr->port_handle,
                             SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ID_ADDR,
                             (uint8_t*)&id_value,
                             1,
                             &bytes_rw);
#ifdef ADSP_HWCONFIG_L
  if (id_value == 0x29){/*9900*/
    ptr->device_select = 0;
  }
  else{/*9930*/
    ptr->device_select = 1;
  }
#endif

  if ( stat != SNS_DDF_SUCCESS )
  {
    ALSPRX_MSG_1(LOW, "init - stat=%d", stat);
    sns_ddf_timer_release(ptr->sns_dd_prx_db.timer);
    sns_ddf_timer_release(ptr->sns_dd_als_db.timer);
    sns_ddf_mfree(ptr);
    return stat;
  }

  ptr->driver_state=2;


  if (ptr->device_select == 0) {
    if (id_value != 0x29)
    {
      ptr->driver_state= ((id_value<<8)| 3);
      ALSPRX_MSG_2(LOW, "init - %d id=%d", 20, id_value);
      // the underlying sensor is not APDS-9900/QPDS-T900
      sns_ddf_close_port(ptr->port_handle);
      sns_ddf_timer_release(ptr->sns_dd_prx_db.timer);
      sns_ddf_timer_release(ptr->sns_dd_als_db.timer);
      sns_ddf_mfree(ptr);
      return SNS_DDF_EFAIL;
    }
  }
  else {
    if (id_value != 0x39)
    {
      ptr->driver_state= ((id_value<<8)| 4);
      ALSPRX_MSG_2(LOW, "init - %d id=%d", 30, id_value);
      // the underlying sensor is not APDS-9930/QPDS-T930
      sns_ddf_close_port(ptr->port_handle);
      sns_ddf_timer_release(ptr->sns_dd_prx_db.timer);
      sns_ddf_timer_release(ptr->sns_dd_als_db.timer);
      sns_ddf_mfree(ptr);
      return SNS_DDF_EFAIL;
    }
  }
}
#endif

  ptr->driver_state=5;

  /* Resets the driver */
  stat = sns_dd_apds99xx_reset((sns_ddf_handle_t) ptr);

  /* Recall NV parameters if any */
  if (nv_params) {
    bool data_from_registry = FALSE;

    if ( (nv_params->status == SNS_DDF_SUCCESS) && (nv_params->data_len >= sizeof(sns_dd_nv_db_type)) ) {
      nv_ptr = (sns_dd_nv_db_type *)nv_params->data;
      if ( (nv_ptr->version_num != 0) &&
           (nv_ptr->id != 0) &&
           (nv_params->data_len != 0) ) {

        // update value from NV parameters
        data_from_registry = TRUE;
        ptr->sns_dd_apds99xx_common_db.nv_size = nv_params->data_len;
        ptr->sns_dd_apds99xx_common_db.nv_db.visible_ratio = nv_ptr->visible_ratio;
        ptr->sns_dd_apds99xx_common_db.nv_db.ir_ratio = nv_ptr->ir_ratio;
        ptr->sns_dd_apds99xx_common_db.nv_db.als_factor = nv_ptr->als_factor;
        ptr->sns_dd_apds99xx_common_db.nv_db.version_num = nv_ptr->version_num;
        ptr->sns_dd_apds99xx_common_db.nv_db.id = nv_ptr->id;
        // update prx factor and thresh through this
        sns_dd_apds99xx_set_prx_factor(ptr, SNS_DDF_SENSOR_PROXIMITY, nv_ptr->prx_factor);
        ALSPRX_MSG_1(LOW, "prx_factor=%d", nv_ptr->prx_factor );
        // update als change pcnt
        sns_dd_apds99xx_set_als_change_pcnt(ptr, nv_ptr->als_change_pcnt);
        ALSPRX_MSG_1(LOW, "als_change_pcnt=%d", nv_ptr->als_change_pcnt );
        ALSPRX_MSG_1(HIGH, "als ga = %d", SNS_DD_APDS99XX_ALS_GA);
        ALSPRX_MSG_1(HIGH, "als coe b = %d", SNS_DD_APDS99XX_ALS_COE_B);
        ALSPRX_MSG_1(HIGH, "als coe c = %d", SNS_DD_APDS99XX_ALS_COE_C);
        ALSPRX_MSG_1(HIGH, "als coe d = %d", SNS_DD_APDS99XX_ALS_COE_D);
      }
    }
    if (!data_from_registry)
    {
      // default value of NV parameters
      ptr->sns_dd_apds99xx_common_db.nv_size = 128;
      ptr->sns_dd_apds99xx_common_db.nv_db.visible_ratio = 20; // 20% visible transmittance
      ptr->sns_dd_apds99xx_common_db.nv_db.ir_ratio = 80; // 80% ir transmittance
      ptr->sns_dd_apds99xx_common_db.nv_db.dc_offset = 0; // DC offset
      ptr->sns_dd_apds99xx_common_db.nv_db.thresh_near = SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD;
      ptr->sns_dd_apds99xx_common_db.nv_db.thresh_far = SNS_DD_APDS99XX_PRX_FAR_THRESHOLD;
#ifdef ADSP_HWCONFIG_L
      ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor = 400; //default prx_factor
#else
      ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor = 0;
#endif
      ptr->sns_dd_apds99xx_common_db.nv_db.als_factor = 100; // factor of 1
      ptr->sns_dd_apds99xx_common_db.nv_db.version_num = 1;
      ptr->sns_dd_apds99xx_common_db.nv_db.id = 0x10|SNS_DDF_VENDOR_AVAGO;  /* 0001xxxx = APDS-9900, 0010xxxx = APDS-9930, 0011xxx = APDS-9190, 0100xxxx = APDS-9130 */
      ptr->sns_dd_apds99xx_common_db.nv_db.als_change_pcnt = SNS_DD_APDS99XX_DEFAULT_ALS_CHANGE_PCNT;
      sns_ddf_smgr_notify_event(ptr->smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
      ALSPRX_MSG_1(LOW, "prx_factor=%d", ptr->sns_dd_apds99xx_common_db.nv_db.prx_factor);
    }

  }
  return stat;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode, resolution and ODR.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_apds99xx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  ALSPRX_MSG_3(LOW, "set_attr - %d sensor=%d attr=%d", 300, sensor, attrib);
//  ALSPRX_MSG_2(LOW, "", 300, *(uint64_t *)value);

  if ( (dd_handle == NULL) ||
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      /* don't check if als or prx, either one will do */
      status = sns_dd_apds99xx_set_powerstate((sns_dd_apds99xx_state_t *)dd_handle,
                                              sensor,
                                            *(sns_ddf_powerstate_e *)value,
                                            true);
      break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      /* set resolution */
      status = sns_dd_apds99xx_set_resolution((sns_dd_apds99xx_state_t *)dd_handle,
                                            sensor,
                                            *(sns_dd_alsprx_res_e *)value);
      break;

    case SNS_DDF_ATTRIB_ODR:
      /* output data rate */
      status = sns_dd_apds99xx_set_odr((sns_dd_apds99xx_state_t *)dd_handle,
                                       sensor,
                                       *(uint32_t *)value);
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  return(status);
}

/*===========================================================================

FUNCTION      sns_dd_apds99xx_common_request_data

DESCRIPTION   As a async driver, this driver needs preparing a sensor data.
              This function calls an appropriate preparing function according to
              the data type.

DEPENDENCIES  None

RETURN VALUE  FALSE if successful, TRUE otherwise

SIDE EFFECT   None
===========================================================================*/
bool sns_dd_apds99xx_common_request_data
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type
)
{
  uint8_t   i2c_data = 0x0;
  uint8_t          bytes_w;

  /* make sure device is in the right state */
  if ( dd_ptr->sns_dd_apds99xx_common_db.state != SNS_DD_DEV_CONFIGURED )
  {
    /* device state is stopped OR invalid */
    return true;
  }

  /* Request data from device */
  if ( sensor_type == SNS_DDF_SENSOR_AMBIENT )
  {
    dd_ptr->sns_dd_apds99xx_common_db.als_get_data = true;

    if (dd_ptr->sns_dd_als_db.enable == false) {
      if (dd_ptr->sns_dd_prx_db.enable) { /* if prox already enabled */
        i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_PRX_ENABLE;
      }
      else {
        i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE;
      }

      dd_ptr->sns_dd_als_db.enable = true;

      sns_ddf_write_port(dd_ptr->port_handle,
                       SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                       (uint8_t*)&i2c_data,
                       1,
                       &bytes_w);
      dd_ptr->enable_reg_data = i2c_data;
    }

    sns_dd_apds99xx_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT);
  }
  else /* SNS_DDF_SENSOR_PROXIMITY */
  {
    dd_ptr->sns_dd_apds99xx_common_db.prx_get_data = true;

    if (dd_ptr->sns_dd_prx_db.enable == false) {
      /* need to make use of ALS function as supplementary support for Prox */
      i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE|SNS_DD_APDS99XX_PRX_ENABLE;
      dd_ptr->sns_dd_prx_db.enable = true;

      sns_ddf_write_port(dd_ptr->port_handle,
                       SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                       (uint8_t*)&i2c_data,
                       1,
                       &bytes_w);
      dd_ptr->enable_reg_data = i2c_data;
    }

    sns_dd_apds99xx_req_data (dd_ptr, SNS_DDF_SENSOR_PROXIMITY);
  }

  return false;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested

  This driver is a pure asynchronous one, so no data will be written to buffer.
  As a result, the return value will be always PENDING if the process does
  not fail.  This driver will notify the Sensors Manager via asynchronous
  notification when data is available.

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in]  sensors      List of sensors for which data is requested.
  @param[in]  num_sensors  Length of @a sensors.
  @param[in]  memhandler   Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements must
                           match @a num_sensors.

  @return SNS_DDF_SUCCESS if data was populated successfully. If any of the
          sensors queried are to be read asynchronously SNS_DDF_PENDING is
          returned and data is via @a sns_ddf_smgr_data_notify() when
          available. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_apds99xx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
  sns_ddf_status_e status = SNS_DDF_PENDING;
  uint8_t          i;

  ALSPRX_MSG_3(LOW, "get_data - %d sensor=%d num=%d",
               200, sensors[0], num_sensors);

  if ( (dd_handle == NULL) || (sensors == NULL) ||
       (num_sensors < 1) || (num_sensors > 2) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (EnableI2C == FALSE)
  {
     static uint8_t toggle[2] = {0};
     sns_ddf_sensor_data_s *data_ptr;
     sns_ddf_sensor_sample_s *sample_ptr;
     int8 i;

     *data = sns_ddf_memhandler_malloc(
         memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

     if(*data == NULL)
         return SNS_DDF_ENOMEM;

     for (i = 0; i < num_sensors; i++)
     {
       data_ptr = &((*data)[i]);

       if( (sample_ptr = sns_ddf_memhandler_malloc(memhandler,
                           (2*sizeof(sns_ddf_sensor_sample_s)))) == NULL)
       {
         return SNS_DDF_ENOMEM;
       }

       sns_ddf_delay(50);

       if (sensors[i] == SNS_DDF_SENSOR_PROXIMITY)
       {
         data_ptr->sensor = SNS_DDF_SENSOR_PROXIMITY;
         if (toggle[i])
         {
           sample_ptr[0].sample = FX_CONV_Q16(0, 0); //far = 0; near = 1
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 100; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
         else
         {
           sample_ptr[0].sample = FX_CONV_Q16(1, 0); //far = 0; near = 1
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 5; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
       }
       else
       {
         data_ptr->sensor = SNS_DDF_SENSOR_AMBIENT;
         if (toggle[i])
         {
           sample_ptr[0].sample = (int32_t)(20<<16); //20 lux
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 60; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
         else
         {
           sample_ptr[0].sample = (int32_t)(50<<16); //50 lux
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 150; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
       }
       data_ptr->status = SNS_DDF_SUCCESS;
       data_ptr->num_samples = 2;
       data_ptr->timestamp = sns_ddf_get_timestamp();
       data_ptr->samples = sample_ptr;
       toggle[i] = ~toggle[i];
     }

     return SNS_DDF_SUCCESS;
  }

  /* Look at all the requests */
  for ( i = 0; i < num_sensors; i++ )
  {
    if ( sensors[i] != SNS_DDF_SENSOR_AMBIENT &&
         sensors[i] != SNS_DDF_SENSOR_PROXIMITY )
    {
      status = SNS_DDF_EINVALID_PARAM;
      break;
    }

    if ( sns_dd_apds99xx_common_request_data((sns_dd_apds99xx_state_t *)dd_handle,
                       sensors[i]) == true )
    {
      status = SNS_DDF_EFAIL;
      break;
    }
  }

  return status;
}

/*==============================================================================

FUNCTION      sns_dd_apds99xx_prx_query

DESCRIPTION   Called by sns_dd_apds99xx_get_attrib to get an attribute value for proximitydistance data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_apds99xx_prx_query
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t* thresh_ptr;
  uint16_t* accu_ptr;
  sns_ddf_power_info_s* power_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(0.00); /* From 0 mm*/
      device_ranges->max = FX_FLTTOFIX_Q16(0.05);  /* to 50 mm*/
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      resp->bit_len = 10;
      resp->max_freq = 20;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      q16_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(q16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *resp =  SNS_DD_PRX_RES;
       *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_PRX_PWR;
      power_ptr->lowpower_current = SNS_DD_ALS_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = SNS_DD_PRX_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = dd_ptr->sns_dd_prx_db.thresh_near;
      *(uint16_t **)value = thresh_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      if ( dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr > 0 )
      {
        *odr_ptr = dd_ptr->sns_dd_apds99xx_common_db.prx_odr;
      }
      else
      {
        *odr_ptr = dd_ptr->sns_dd_apds99xx_common_db.prx_req_odr;
      }
      *value = odr_ptr;
      ALSPRX_MSG_2(LOW, "prx_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_apds99xx_als_query

DESCRIPTION   Called by sns_dd_apds99xx_get_attrib to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_apds99xx_als_query
(
  sns_dd_apds99xx_state_t* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(0.01); // open air is 0.01 Lux but with cover glass+black ink it will be 1 Lux
      device_ranges->max = FX_FLTTOFIX_Q16(10000.0);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      *num_elems = 3;

      resp[0].bit_len = sns_dd_als_adc_bit_tb[SNS_DD_ALSPRX_RES_14BIT];
      resp[0].max_freq = 20;
      resp[1].bit_len = sns_dd_als_adc_bit_tb[SNS_DD_ALSPRX_RES_15BIT];
      resp[1].max_freq = 10;
      resp[2].bit_len = sns_dd_als_adc_bit_tb[SNS_DD_ALSPRX_RES_16BIT];
      resp[2].max_freq = 5;

      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = SNS_DD_ALS_RES;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = SNS_DD_ALS_PWR;
      power_ptr->lowpower_current = SNS_DD_ALS_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
     *accu_ptr = SNS_DD_ALS_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      if ( dd_ptr->sns_dd_apds99xx_common_db.als_req_odr > 0 )
      {
        *odr_ptr = dd_ptr->sns_dd_apds99xx_common_db.als_odr;
      }
      else
      {
        *odr_ptr = dd_ptr->sns_dd_apds99xx_common_db.als_req_odr;
      }
      *value = odr_ptr;
      ALSPRX_MSG_2(LOW, "als_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  Returns the requested attribute

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor      Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in]  memhandler  Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    Success if the attribute was retrieved and the buffer was
    populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_apds99xx_get_attrib
(
  sns_ddf_handle_t     handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t *)handle;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  bool generic_attrib = false;
  sns_ddf_driver_info_s *driver_info_ptr;
  sns_ddf_device_info_s *device_info_ptr;
  sns_ddf_registry_group_s *reg_group_ptr;
  uint8_t *reg_group_data_ptr;

  ALSPRX_MSG_2(LOW, "get_attr - driver_state=%d gpio=%x", 
               dd_ptr->driver_state, dd_ptr->interrupt_gpio);

  if ( (handle == NULL)      ||
       (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  ALSPRX_MSG_3(LOW, "get_attr - %d sensor=%d attr=%d", 
               100, sensor, attrib);

  /* check first if the query is for generic attributes */
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      driver_info_ptr->name = (dd_ptr->device_select == 0) ?
          "APDS-9900/QPDS-T900 Proximity & Light" : "APDS-9930/QPDS-T930 Proximity & Light";
      driver_info_ptr->version = 2;
      *(sns_ddf_driver_info_s**)value = driver_info_ptr;
      *num_elems = 1;
      generic_attrib = true;
      break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      device_info_ptr->name = "Proximity & Light";
      device_info_ptr->vendor = "Avago";
      device_info_ptr->model = (dd_ptr->device_select == 0) ? "APDS-9900/QPDS-T900" : "APDS-9930/QPDS-T930";
      device_info_ptr->version = 2;
      *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
      *num_elems = 1;
      generic_attrib = true;
      break;

    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
      if ( (reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, dd_ptr->sns_dd_apds99xx_common_db.nv_size)) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      SNS_OS_MEMCOPY(reg_group_data_ptr, &dd_ptr->sns_dd_apds99xx_common_db.nv_db, sizeof(sns_dd_nv_db_type));

      reg_group_ptr->group_data = reg_group_data_ptr;
      reg_group_ptr->size = dd_ptr->sns_dd_apds99xx_common_db.nv_size;
      *(sns_ddf_registry_group_s**)value = reg_group_ptr;
      ALSPRX_MSG_2(LOW, "get_attrib - size=%d data=%d", 
                   reg_group_ptr->size, (uint8_t) *reg_group_ptr->group_data);

      ALSPRX_MSG_3(LOW, "get_attrib - ratio=%d factor=%d id=%d",
                   dd_ptr->sns_dd_apds99xx_common_db.nv_db.visible_ratio,
                   dd_ptr->sns_dd_apds99xx_common_db.nv_db.als_factor,
                   dd_ptr->sns_dd_apds99xx_common_db.nv_db.id );

      *num_elems = 1;
      generic_attrib = true;
      break;

    default:
      /* do nothing */
      break;
  }

  /* The query is not for generic attribute but is for specific data type */
  if ( generic_attrib != true )
  {
    switch ( sensor )
    {
      case SNS_DDF_SENSOR_AMBIENT:
        status = sns_dd_apds99xx_als_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      case SNS_DDF_SENSOR_PROXIMITY:
        status = sns_dd_apds99xx_prx_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      default:
        status = SNS_DDF_EINVALID_ATTR;
    }
  }

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_prox_calibration

===========================================================================*/
/*!
  @ Called by the SMGR to run calibration test for ALS or PRX.

  @detail
  Returns the cal_factor in error, to be stored in NV memory

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor_type Sensor whose attribute is to be retrieved.
  @param[in] test         Test ID used to decide what to test.
  @param[out] err         prx_factor or als_factor.

  @return
    Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_apds99xx_calibration
(
  sns_ddf_handle_t    handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
)
{
  sns_dd_apds99xx_state_t* dd_ptr = (sns_dd_apds99xx_state_t *)handle;
  sns_ddf_status_e status = SNS_DDF_PENDING;
  sns_ddf_status_e init_status = SNS_DDF_EFAIL;
  uint8_t   i2c_data = 0x0;
  uint8_t   bytes_w;

  if (handle == NULL)
  {
    return SNS_DDF_ENOMEM;
  }

  ALSPRX_MSG_2(LOW, "calib - prx data %d %d\r\n", sensor_type, test);
  /* check what is the test type */
  switch ( test )
  {
    case SNS_DDF_TEST_SELF:
    case SNS_DDF_TEST_OEM:

      if (sensor_type == SNS_DDF_SENSOR_PROXIMITY) {
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
          init_status = sns_dd_isl29028_int_disable();
          ALSPRX_MSG_2(LOW, "calib - %d prox dis stat=%d", 0, init_status);
#endif
          init_status = sns_dd_apds99xx_set_powerstate(dd_ptr, SNS_DDF_SENSOR__ALL,
                                                      SNS_DDF_POWERSTATE_ACTIVE, false);

          if(init_status != SNS_DDF_SUCCESS)
          {
              ALSPRX_MSG_1(ERROR, "sns_dd_apds99xx_set_powerstate error", init_status);
          }

          if (dd_ptr->sns_dd_prx_db.enable == false) {

            dd_ptr->sns_dd_prx_db.enable = true;
            i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_PRX_ENABLE|SNS_DD_APDS99XX_WAIT_ENABLE;

            sns_ddf_write_port(dd_ptr->port_handle,
                             SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                             (uint8_t*)&i2c_data,
                             1,
                             &bytes_w);

          }
          else if (dd_ptr->sns_dd_apds99xx_common_db.prx_get_data == true) {
            return SNS_DDF_EDEVICE_BUSY;
          }

          dd_ptr->sns_dd_apds99xx_common_db.prx_calibration = true;
          dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_loop = 0;
          dd_ptr->sns_dd_apds99xx_common_db.prx_calibration_pdata = 0;

          ALSPRX_MSG_1(LOW, "prx data %d %d\r\n", 100);

          sns_dd_apds99xx_req_data (dd_ptr, sensor_type);
          //sns_dd_apds99xx_common_request_data(dd_ptr, sensor_type);
      }
      else if (sensor_type == SNS_DDF_SENSOR_AMBIENT) {
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
        init_status = sns_dd_isl29028_int_disable();
        ALSPRX_MSG_2(LOW, "calib - %d als dis stat=%d", 0, init_status);
#endif

        init_status = sns_dd_apds99xx_set_powerstate(dd_ptr, SNS_DDF_SENSOR__ALL,
                                                    SNS_DDF_POWERSTATE_ACTIVE, false);

        if (dd_ptr->sns_dd_als_db.enable == false) {

          dd_ptr->sns_dd_als_db.enable = true;
          i2c_data = SNS_DD_APDS99XX_PWR_ON|SNS_DD_APDS99XX_ALS_ENABLE;

          sns_ddf_write_port(dd_ptr->port_handle,
                           SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ENABLE_ADDR,
                           (uint8_t*)&i2c_data,
                           1,
                           &bytes_w);
        }
        else if (dd_ptr->sns_dd_apds99xx_common_db.als_get_data == true) {
          return SNS_DDF_EDEVICE_BUSY;
        }

        dd_ptr->sns_dd_apds99xx_common_db.als_calibration = true;
        sns_dd_apds99xx_req_data (dd_ptr, sensor_type);
        //sns_dd_apds99xx_common_request_data(dd_ptr, sensor_type);
      }

      break;

    default:
      /* do nothing */
      return SNS_DDF_EINVALID_TEST;
  }

  return status;
}
/*===========================================================================

  FUNCTION:   sns_dd_apds99xx_probe

===========================================================================*/
/**
 * @brief Probes for the device with a given configuration.
 *
 * This commands the driver to look for the device with the specified
 * configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
 * struct.
 *
 * @param[in]  dev_info    Access info for physicol devices controlled by
 *                         this driver. Used to determine if the device is
 *                         physically present.
 * @param[in]  memhandler  Memory handler used to dynamically allocate
 *                         output parameters, if applicable.
 * @param[out] num_sensors Number of sensors supported. 0 if none.
 * @param[out] sensor_type Array of sensor types supported, with num_sensor
 *                         elements. Allocated by this function.
 *
 * @return SNS_DDF_SUCCESS if the part was probed function completed, even
 *         if no device was found (in which case num_sensors will be set to
 *         0).
 */
static sns_ddf_status_e sns_dd_apds99xx_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
)
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_ID_ADDR,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != 0x20 /* APDS-9900 */ &&
      i2c_buff != 0x29 /* APDS-9901 */ &&
      i2c_buff != 0x39 /* APDS-9930 */ )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

#if 0
  /* Read & Verify Proximity Time Control Reg */
  status = sns_ddf_read_port( port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_PTIME_ADDR,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != 0xFF )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Read & Verify Wait Time Reg */
  status = sns_ddf_read_port( port_handle,
                              SNS_DD_APDS99XX_CMD_BYTE|SNS_DD_APDS99XX_WTIME_ADDR,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != 0xFF )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }
#endif /* 0 */

  /* Registers registers are correct. This is probably an APDS-9900 or APDS-9901 */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
    sns_ddf_close_port( port_handle );
  return status;
}
