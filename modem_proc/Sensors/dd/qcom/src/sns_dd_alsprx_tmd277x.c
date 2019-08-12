/********************************************************************************
* Copyright (c) 2013, "ams AG"
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
/*------------------------------------------------------------------------
   Copyright (c) 2013-2016 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
  ------------------------------------------------------------------------*/
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the ams AG ALS(Light) and Proximity Sensor
   This driver has 3 sub-modules:
   1. The common handler that provides the driver interface
   2. The light(ALS) driver that handles light data type
   3. The proximity driver that handles proximity data type

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     -----------------------------------------------------------
03/01/16     sc      Add self test type SNS_DDF_TEST_SELF
02/23/16     sc      Reset prox/als thresholds when interrupt is disabled
02/10/16     sd      set sleep state in reset
01/20/16     sc      Fix continuous interrupt firing between thresholds
01/19/16     sc      Fix an issue with resetting prox detect/release
10/08/15     dq      Fix prox can not work on 8996 qrd
07/22/14     MW      Eliminated compiler warning
03/19/14     MW      Fix max ODR for prox and als
03/12/14     MW      Fix KW issue
12/17/13     MW      Fix prox in polling mode
12/09/13     MW      Add delay timer for first integ cycle before data is returned in get_data
09/08/13     ag      Fixed issue with zero lux reporting after every reset
07/30/13     pn      Fixed issue of subsequent interrupts not triggering
07/24/13     vh      Eliminated compiler warnings
06/21/13     rs      updated ALS get_data procedure to fix CR. Reduced Prox LED current to 50%.
06/13/13     rs      Updated ALS calibration correction factor to be in Q16 to support float calibration factors
06/10/13     an      Probe function Update.
06/03/13     ps      Update Vendor name
05/21/13     rs      Added NV param R/W to registry, fixed Prox DRI startup condition and Prox+ALS DRI stream + updated attributes
04/25/13     ag      Use only 16 bits for raw data in log packets
04/03/13     ag      Add log packet, use new debug macro
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
12/11/09     fv      Convert Intersil driver to drive AMS TSL/TMD 277x ALS/PROX sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_tmd277x_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_memmgr.h"
//#include "sns_dd.h"

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
//#include "sns_ddf_util.h"
#endif

#ifdef ALSPRX_SDD_TEST
#error code not present
#endif


/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
static ams_als_lux_coef ams_init_lux_coefs[] = {
    {53, 2000, 600, 1000},   /* TSL2571 */
    {53, 2000, 600, 1000},   /* TSL2671 */
    {53, 2000, 600, 1000},   /* TSL2771 */

    {24, 2000, 600, 1000},   /* TMD2671 */
    {24, 2000, 600, 1000},   /* TMD2771 */

    {60, 1870, 630, 1000},   /* TSL2572 */
    {60, 1870, 630, 1000},   /* TSL2672 */
    {60, 1870, 630, 1000},   /* TSL2772 */

    {20, 1750, 630, 1000},   /* TMD2672 */
    {20, 1750, 630, 1000}    /* TMD2772 */
};


//
// CHECK THIS TABLE
//
static uint16_t ams_min_integ_time_us[] = {
    2720,    // TSL2571
    2720,    // TSL2671
    2720,    // TSL2771

    2720,    // TMD2671
    2720,    // TMD2771

    2730,    // TSL2572
    2730,    // TSL2672
    2730,    // TSL2772

    2730,    // TMD2672
    2730     // TMD2772
};


//
// Convert ODR in Hz to uSec
//
#define MAX_ODR  40

static uint32_t odr_to_uSec[] = {
    // uSec        odr
    1000000,    //  1
     500000,    //  2
     333333,    //  3
     250000,    //  4
     200000,    //  5
     166667,    //  6
     142857,    //  7
     125000,    //  8
     111111,    //  9
     100000,    // 10
      90909,    // 11
      83333,    // 12
      76923,    // 13
      71429,    // 14
      66667,    // 15
      62500,    // 16
      58824,    // 17
      55556,    // 18
      52632,    // 19
      50000,    // 20
      47619,    // 21
      45455,    // 22
      43478,    // 23
      41667,    // 24
      40000,    // 25
      38462,    // 26
      37037,    // 27
      35714,    // 28
      34483,    // 29
      33333,    // 30
      32258,    // 31
      31250,    // 32
      30303,    // 33
      29412,    // 34
      28571,    // 35
      27778,    // 36
      27027,    // 37
      26316,    // 38
      25641,    // 39
      25000     // 40
};

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_alsprx_tmd277x_init
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

static sns_ddf_status_e sns_dd_alsprx_tmd277x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_tmd277x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_tmd277x_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_alsprx_tmd277x_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

void sns_dd_alsprx_tmd277x_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_alsprx_tmd277x_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_ams_tmd277x_alsprx_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);
sns_ddf_status_e sns_dd_alsprx_tmd277x_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );

 static sns_ddf_status_e sns_dd_alsprx_tmd277x_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );

sns_ddf_driver_if_s sns_alsprx_tmd277x_driver_fn_list =
{
  .init = &sns_dd_alsprx_tmd277x_init,
  .get_data = &sns_dd_alsprx_tmd277x_get_data,
  .set_attrib = &sns_dd_alsprx_tmd277x_set_attrib,
  .get_attrib = &sns_dd_alsprx_tmd277x_get_attrib,
  .handle_timer = &sns_dd_alsprx_tmd277x_handle_timer,
  .handle_irq = &sns_dd_alsprx_tmd277x_handle_irq,
  .reset = &sns_dd_alsprx_tmd277x_reset,
  .run_test = &sns_dd_ams_tmd277x_alsprx_run_test,
  .enable_sched_data = &sns_dd_alsprx_tmd277x_enable_sched_data,
  .probe = &sns_dd_alsprx_tmd277x_probe

};

sns_ddf_status_e tmd277x_als_sensor_samples(sns_dd_alsprx_state_t   *state,
                                            sns_ddf_sensor_data_s   *data_ptr,
                                            sns_ddf_memhandler_s    *memhandler,
                                            sns_ddf_sensor_sample_s *sample_data);

sns_ddf_status_e tmd277x_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data);

void ams_tmd277x_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);

sns_ddf_status_e API_APP_Get_mLUX(sns_dd_alsprx_state_t* state);

sns_ddf_status_e tmd277x_get_als_data(sns_dd_alsprx_state_t* state);
sns_ddf_status_e tmd277x_get_prox_data(sns_dd_alsprx_state_t* state);


sns_ddf_status_e ams_tmd277x_calibrate_als(sns_dd_alsprx_state_t* state);

sns_ddf_status_e tmd277x_write_wtime(sns_dd_alsprx_state_t* state);

//
// +++++++++++++++++++  AMS-TAOS USA Code   +++++++++++++++++++
//

/*!
 ***************************************
 * @brief Function to write a single byte
 * to the TMD277x at register /b reg
 *
 * @param reg : Register address to write byte
 * @param byteVal : byte value to write at register
 *
 * @return status of write
 ***************************************
 */
sns_ddf_status_e tmd277x_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | reg,
                                byte_val,
                                1,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e tmd277x_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | reg,
                                &word_val->byte[0],
                                2,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e tmd277x_read_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t byteCnt, uint8_t* buf)
{
    sns_ddf_status_e status;
    uint8_t          read_count;

    status = sns_ddf_read_port(port_handle,
                               TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | reg,
                               buf,
                               byteCnt,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e tmd277x_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;

    status = sns_ddf_read_port(port_handle,
                               TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | reg,
                               byte_val,
                               1,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e tmd277x_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;
    uword_u          word_tmp;

    status = sns_ddf_read_port(port_handle,
                               TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | reg,
                               &word_tmp.byte[0],
                               2,
                               &read_count
                               );
    if(status == SNS_DDF_SUCCESS)
    {
        word_val->word = word_tmp.word;
    }
    return status;
}


/*!
 ***************************************
 * @brief Write special function to the TMD277x command register
 *
 * Will write special function byte to slave device, and dummy byte will be ignored.
 *
 * @param port_handle : Handle to port data
 * @param spec_func   : Special function to write to command register
 * - 0x00  - Normal operation
 * - 0x05  - Proximity interrupt clear
 * - 0x06  - ALS interrupt clear
 * - 0x07  - Proximity and ALS interrupt clear
 * - other - Reserved, do NOT write
 *
 * @return status
 * - SNS_DDF_SUCCESS    - No error
 * - SNS_DDF_EDEVICE    - Comm error
 * - SNS_DDF_EBUS       - Comm error
 * - SNS_EINVALID_PARAM - Incorrect Config parameters
 ***************************************
 */
sns_ddf_status_e tmd277x_write_spec_func(sns_ddf_handle_t port_handle, uint8_t spec_func)
{
    sns_ddf_status_e status;
    uint8_t          rw_count;
    uint8_t          i2c_data;

    i2c_data   = (TMD277x_CMD_REG | TMD277x_CMD_SPL_FN | spec_func);

#ifdef DDK
{
    uint8_t          dummy_byte;
    dummy_byte = i2c_data;

    status = sns_ddf_read_port(port_handle,
                               i2c_data,
                               (uint8_t*)&dummy_byte,
                               1,
                               &rw_count
                               );
}
#else
    status = sns_ddf_write_port(port_handle,
                                i2c_data,
                                NULL,
                                0,
                                &rw_count
                                );
#endif
    return status;
}


uint8_t usecToIcycles(uint32_t time_us, uint32_t minIntegTime_us)
{
    uint8_t integrationCycles;

    if(time_us < minIntegTime_us)
    {
        time_us = minIntegTime_us;
    }

    integrationCycles = time_us / minIntegTime_us;

    return (256 - integrationCycles);
}

#define MS_TO_US(t) (t * 1000)
#define US_TO_MS(t) (t / 1000)

//
// ------------------  AMS-TAOS USA Code   ------------------
//

/*===========================================================================
FUNCTION      sns_dd_alsprx_tmd277x_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_alsprx_tmd277x_log(
   sns_dd_alsprx_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1,
   uint32_t               data1_q16,
   uint32_t               data2,
   uint32_t               data2_q16,
   uint32_t               raw_data )
{
#ifndef DDK
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   TMD2771_MSG_0(HIGH, "TMD277x Log Data");

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AMS;

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
       TMD2771_MSG_1(HIGH, "TMD277x Log Data - logpkt_malloc failed with err: %d", err_code);
       dd_ptr->dropped_logs++;
   }
#endif
}



sns_ddf_status_e sns_dd_alsprx_tmd277x_enable_sched_data(sns_ddf_handle_t dd_handle,
                                                         sns_ddf_sensor_e sensor,
                                                         bool             enable
                                                         )
{
    sns_dd_alsprx_state_t* state = dd_handle;

    uint8_t i2c_data;
    uint8_t byteVal;
    uint8_t readByte;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    ams_setup*       setup;

    // Don't need these flags in DRI mode
    state->first_als_indication = false;
    state->first_prox_indication = false;

    setup = &state->chip.setup;

    //
    // Clear interrupts
    // Clear any pending interrupts
    byteVal = 0x07;
    status = tmd277x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    TMD2771_MSG_3(MED, "enable_sched: enable=%d sensor=%d", enable, sensor, 1100);

    // set up for run.
    //
    if(enable)
    {
        // IRQ's are enabled, register the irq
        status = sns_ddf_signal_register(state->int_gpio,
                                         state,
                                         &sns_alsprx_tmd277x_driver_fn_list,
                                         SNS_DDF_SIGNAL_IRQ_FALLING);
        if (status != SNS_DDF_SUCCESS)
        {
            return status;
        }
    }
    else
    {
        // de-register IRQ
        sns_ddf_signal_deregister(state->int_gpio);
    }

    if(sensor == SNS_DDF_SENSOR_PROXIMITY)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & TMD277x_ENABLE_MSK_PEN) == 0x00)
            {
                // if prox was not enabled previously, write new ptime
                byteVal = usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us);
                status = tmd277x_write_byte(state->port_handle, TMD277x_rPTIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                TMD2771_MSG_2(MED, "enable_sched: 0x%x ==> rPTIME", byteVal, 1102);

                // DEBUG READ
                tmd277x_read_byte(state->port_handle, TMD277x_rPTIME, &readByte);
                TMD2771_MSG_2(MED, "enable_sched: rPTIME ==> 0x%x", readByte, 1103);
            }
            state->chip.setup.prox.enabled = true;
            state->chip.setup.reg.enable  |= (TMD277x_ENABLE_MSK_PIEN | TMD277x_ENABLE_MSK_PEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(TMD277x_ENABLE_MSK_PIEN | TMD277x_ENABLE_MSK_PEN);

            // Reset Prox Interrupt Thresholds
            wordVal.word = 0xFFFF;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPILTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                return status;
            }
            wordVal.word = 0x0000;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPIHTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                return status;
            }
        }
    }
    else if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & TMD277x_ENABLE_MSK_AEN) == 0x00)
            {
                // if ALS not enabled, write new atime
                byteVal = usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
                status = tmd277x_write_byte(state->port_handle, TMD277x_rATIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                TMD2771_MSG_2(MED, "enable_sched: atime_us=%d", setup->als.atime_us, 1104);
                TMD2771_MSG_2(MED, "enable_sched: 0x%x ==> rATIME", byteVal, 1105);
            }
            state->chip.setup.als.enabled = true;
            state->chip.setup.reg.enable |= (TMD277x_ENABLE_MSK_AIEN | TMD277x_ENABLE_MSK_AEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(TMD277x_ENABLE_MSK_AIEN | TMD277x_ENABLE_MSK_AEN);

            // Reset ALS Interrupt Thresholds
            wordVal.word = state->chip.setup.als.thresh_low;
            status = tmd277x_write_word(state->port_handle, TMD277x_rAILTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                return status;
            }
            wordVal.word = state->chip.setup.als.thresh_high;
            status = tmd277x_write_word(state->port_handle, TMD277x_rAIHTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                return status;
            }
        }
    }

    state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_PON;
    i2c_data = state->chip.setup.reg.enable;
    status   = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &i2c_data);
    TMD2771_MSG_3(MED, "enable_sched: 0x%x ==> rENABLE status=%d", i2c_data, status, 1106);

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_alsprx_tmd277x_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_tmd277x_prx_binary
(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t pdata;
    uint16_t detect;
    uint16_t release;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;


    pdata   = state->chip.data.prox.data;
    detect  = state->chip.setup.prox.detect;
    release = state->chip.setup.prox.release;
    TMD2771_MSG_3(MED, "prx_binary: data=%d detect=%d release=%d", pdata, detect, release);

    if(state->chip.data.prox.event == SNS_PRX_FAR_AWAY)
    {
        if(pdata > detect)
        {
            state->chip.data.prox.event = SNS_PRX_NEAR_BY;

            state->chip.setup.prox.thresh_low  = state->chip.setup.prox.release;
            state->chip.setup.prox.thresh_high = 0xFFFF;

            // Set Prox interrupt low
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPILTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                TMD2771_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            TMD2771_MSG_2(MED, "prx_binary: 0x%x ==> rPILTL", wordVal.word, 1107);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPIHTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                TMD2771_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            TMD2771_MSG_2(MED, "prx_binary: 0x%x ==> rPIHTL", wordVal.word, 1108);
        }
        else
        {
            state->chip.setup.prox.thresh_low  = 0x0000;
            state->chip.setup.prox.thresh_high = state->chip.setup.prox.detect;

            // Set Prox Interrupt Threshold low back to original
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPILTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS )
            {
               TMD2771_MSG_1(HIGH, "ALSPRX prx error in set back to original %d", wordVal.word);
               return status;
            }

            // Set Prox Interrupt Threshold high back to original
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPIHTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS )
            {
               TMD2771_MSG_1(HIGH, "ALSPRX prx error in set back to original %d", wordVal.word);
               return status;
            }
       }
    }
    else  // SNS_PRX_NEAR_BY
    {
        if(pdata < release)
        {
            state->chip.data.prox.event = SNS_PRX_FAR_AWAY;

            state->chip.setup.prox.thresh_low  = 0x0000;
            state->chip.setup.prox.thresh_high = state->chip.setup.prox.detect;

            // Set Prox interrupt low
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPILTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                TMD2771_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            TMD2771_MSG_2(MED, "prx_binary: 0x%x ==> rPILTL", wordVal.word, 1109);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = tmd277x_write_word(state->port_handle, TMD277x_rPIHTL, &wordVal);
            if ( status != SNS_DDF_SUCCESS ) {
                TMD2771_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            TMD2771_MSG_2(MED, "prx_binary: 0x%x ==> rPIHTL", wordVal.word, 1110);
        }
    }
    return status;
}





/*===========================================================================

FUNCTION      sns_dd_alsprx_tmd277x_set_als_thresh

DESCRIPTION   This function is called in DRI mode- sets the ALS thresholds to
              make ALS sensor function like an event sensor.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_tmd277x_set_als_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint32_t alsdata, t_low, t_high;
    uword_u wordVal,readALSTh;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    TMD2771_MSG_2(MED, "als_thresh: hi=%d lo=%d",
              state->chip.setup.als.thresh_high, 
              state->chip.setup.als.thresh_low);

    alsdata   = state->chip.data.als.ch0;
    TMD2771_MSG_1(MED, "als_thresh: alsdata=%d",  alsdata );
    
    t_low  = (alsdata * (100-state->chip.setup.als.thresh_low))/100;   //set to 20 as default
    t_high = (alsdata * (100+state->chip.setup.als.thresh_high))/100;   //set to 20 as default 

    //set als threshold low at -x% of data and set als threshold high at x% of the pdata, x defined in registry

    wordVal.word = t_low; 
    status = tmd277x_write_word(state->port_handle, TMD277x_rAILTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS )
    {
       TMD2771_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
       return status;
    }
    TMD2771_MSG_2(MED, "als_thresh: 0x%x ==> rAILTL", wordVal.word, 1111);

    status = tmd277x_read_word(state->port_handle, TMD277x_rAILTL, &readALSTh);
    if ( status != SNS_DDF_SUCCESS )
    {
       TMD2771_MSG_1(HIGH, "ALS thresh low readback error %d", readALSTh.word);
       return status;
    }
    TMD2771_MSG_2(MED, "als_thresh: rAILTL ==> 0x%x", readALSTh.word, 1112);

    wordVal.word = t_high; 
    status = tmd277x_write_word(state->port_handle, TMD277x_rAIHTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS )
    {
       TMD2771_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
       return status;
    }
    TMD2771_MSG_2(MED, "als_thresh: 0x%x ==> rAIHTL", wordVal.word, 1113);

    status = tmd277x_read_word(state->port_handle, TMD277x_rAIHTL, &readALSTh);
    if ( status != SNS_DDF_SUCCESS )
    {
       TMD2771_MSG_1(HIGH, "ALS thresh high readback error %d", readALSTh.word);
       return status;
    }
    TMD2771_MSG_2(MED, "als_thresh: rAIHTL ==> 0x%x", readALSTh.word, 1114);

    return status;
}



void tmd277x_read_status_reg(sns_dd_alsprx_state_t* state, sns_ddf_time_t timestamp, TMD277x_SIG_TYPE sig_type)
{
    uint8_t  statusReg;
    uint8_t  numOfSensors;
    uint8_t  i;
    uint8_t  clrInterrupts;
    bool     hasInterrupt;

    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    sns_ddf_sensor_e sensors[2];


    numOfSensors  = 0;
    clrInterrupts = 0;
    hasInterrupt  = false;

    tmd277x_read_byte(state->port_handle,
                      TMD277x_rSTATUS,
                      &statusReg);

    TMD2771_MSG_1(MED, "read_status_reg: rSTATUS ==> 0x%x" , statusReg);

    TMD2771_MSG_3(MED, "als_ind=%d  prox_ind=%d first_timer=%d", state->first_als_indication, state->first_prox_indication, state->first_intg_cycle_complete);


    if(statusReg & TMD277x_STA_PRX_INTR ||
      (state->first_prox_indication && state->first_intg_cycle_complete))
    {
        /* PROX interrupt */
        TMD2771_MSG_0(MED, "read_status_reg: prox int");

        tmd277x_get_prox_data(state);
        sns_dd_alsprx_tmd277x_prx_binary(state); /* set near or far */

        clrInterrupts |= TMD277x_CMD_PROX_INTCLR;
        hasInterrupt = true;
        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        TMD2771_MSG_1(MED, "TMD277 out of prox data and prx_binary set state: %d", 1115);
    }

    if(statusReg & TMD277x_STA_ALS_INTR ||
      (state->first_als_indication && state->first_intg_cycle_complete))
    {
        /* ALS interrupt */
        TMD2771_MSG_0(MED, "read_status_reg: als int");

        clrInterrupts |= TMD277x_CMD_ALS_INTCLR;
        hasInterrupt = true;

        //
        // On an ALS interrupt, will read the ALS data and store it.
        // Will NOT return data (lux).
        // Lux will be returned in the timer.
        //
#ifdef DDK
        if(sig_type == TIMER_TYPE)
#endif
        {
            // Read ALS data
            tmd277x_get_als_data(state);
            sns_dd_alsprx_tmd277x_set_als_thresh(state); /* set als threshold to be +/- 20% of current value for next interrupt.*/

            // compute LUX
            API_APP_Get_mLUX(state);
            sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
            numOfSensors++;
            TMD2771_MSG_1(MED, "TMD277 ALS Interrupt, data read: %d", TMD277x_STA_ALS_INTR);
        }

        TMD2771_MSG_2(MED, "TMD277 ALS int numOfSensors: %d lux: %d", numOfSensors, state->chip.data.als.lux);
    }

    if(numOfSensors > 0)
    {
        // Had at least one interrupt
        if ((sns_ddf_malloc((void **)&sensor_data, numOfSensors*sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        {
            TMD2771_MSG_1(MED, "TMD277 var1: %d", 1116);
            /* Failed miserably. Can't even notify SMGR */
            return;
        }

        for(i = 0; i < numOfSensors; i++)
        {
            sensor_data[i].sensor    = sensors[i];
            sensor_data[i].status    = SNS_DDF_SUCCESS;
            sensor_data[i].timestamp = timestamp;

            switch(sensors[i])
            {
            case SNS_DDF_SENSOR_PROXIMITY:
                if((sns_ddf_malloc((void **)&buf_ptr, 2 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_1(MED, "TMD277 cannot notify smgr prx: %d", 1118);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }
                TMD2771_MSG_2(MED, "read_status_reg: buf_ptr=0x%x", buf_ptr, 1117);

                tmd277x_prox_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            case SNS_DDF_SENSOR_AMBIENT:
                if((sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_1(MED, "TMD277 cannot notify smgr als: %d", 1120);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }
                TMD2771_MSG_2(MED, "read_status_reg: buf_ptr=0x%x", buf_ptr, 1119);

                tmd277x_als_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                if (!state->chip.data.als.Lux_OK)
                {
                    TMD2771_MSG_1(ERROR, "ALS Invalid Data", 1119);
                    sensor_data[i].status    = SNS_DDF_EINVALID_DATA;
                }
                break;

            default:
                TMD2771_MSG_1(MED, "No data available: %d", 1119);
                return;
            }
        }

        sns_ddf_smgr_notify_data(state->smgr_handle, sensor_data, numOfSensors);
        for(i = 0; i < numOfSensors; i++)
        {
            TMD2771_MSG_1(MED, "read_status_reg: freeing 0x%x", sensor_data[i].samples);
            sns_ddf_mfree(sensor_data[i].samples);
        }
        sns_ddf_mfree(sensor_data);
    }

    if (hasInterrupt)
    {
        TMD2771_MSG_1(MED, "read_status_reg: clrInterrupts=0x%x", clrInterrupts);
        tmd277x_write_spec_func(state->port_handle, clrInterrupts);

        /* - 0x05  - Proximity interrupt clear * - 0x06  - ALS interrupt clear * - 0x07  - Proximity and ALS interrupt clear*/


#ifdef DDK
        tmd277x_read_byte(state->port_handle,
                          TMD277x_rSTATUS,
                          &statusReg);
        TMD2771_MSG_2(MED, "TMD277 status reg var1: %d var2: %d", 1122, statusReg);
#endif
    }

    TMD2771_MSG_1(MED, "TMD277 out of read status reg: %d", 1123);
}


/*===========================================================================

  FUNCTION:   sns_dd_alsprx_TMD277x_handle_irq

===========================================================================*/
/*!
  @brief AMS TMD277x interrupt handler

  @detail
  This function will be called within the context of the Interrupt
  Service Task (IST), *not* the ISR.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
void sns_dd_alsprx_tmd277x_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    TMD2771_MSG_1(MED, "handle_irq()", 1124);

    tmd277x_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_tmd277x_handle_timer

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
static void sns_dd_alsprx_tmd277x_handle_timer
(
  sns_ddf_handle_t handle,
  void*            arg
)
{
    sns_ddf_status_e  status;
    sns_ddf_time_t    timestamp;
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    /* sanity check */
    if(state == NULL)
    {
       TMD2771_MSG_0(ERROR, "handle_timer() - NULL driver handle");
       return;
    }

    TMD2771_MSG_1(MED, "handle_timer()", 5);

    // For the first cycle, we use a timer that goes off when data should be ready,
    // and notifies SMGR, to make this first sample return as fast as possible.
    // Afterwards, normal operation will continue.
    if ( (NULL != arg)
      && (NULL != state)
      && (*(uint32_t *)arg == state->first_integ_cycle_timer_arg)
      && !state->first_intg_cycle_complete )
    {
        state->first_intg_cycle_complete = true;

        // If there is wait time, enable it now
        if (state->chip.setup.wait.wtime_us > 0)
        {
            state->chip.setup.wait.wen = 1;
            status = tmd277x_write_wtime(state);
            if (status != SNS_DDF_SUCCESS)
            {
                TMD2771_MSG_1(HIGH, "wtime error, status: %d", status);
            }
        }
    }

    timestamp = sns_ddf_get_timestamp();

    tmd277x_read_status_reg(state, timestamp, TIMER_TYPE);
}


/*==============================================================================

FUNCTION      sns_dd_alsprx_tmd277x_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_tmd277x_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
/*  sns_dd_als_db_type *als_db_ptr = &dd_ptr->sns_dd_als_db; */
/*  sns_dd_alsprx_common_db_type *common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db; */

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_INIT);

/*   als_db_ptr->pend_state        = SNS_DD_PEND_STATE_IDLE; */
/*   als_db_ptr->next_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG; */
/*   als_db_ptr->req_rng_sel       = SNS_DD_ISL_ALS_DEF_RNG; */
/* #ifndef DDK */
/*   als_db_ptr->last_get_time     = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp()); */
/* #endif */
/*   als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX; */

/*   als_db_ptr->als_conversion_lo = (SNS_DD_ALSPRX_ALPHA_LO * (uint32_t) common_db_ptr->nv_db.als_factor) / \ */
/*                                   (common_db_ptr->nv_db.visible_ratio * 10); */

/*   als_db_ptr->als_conversion_hi = (SNS_DD_ALSPRX_ALPHA_HI * (uint32_t) common_db_ptr->nv_db.als_factor) / \ */
/*                                   (common_db_ptr->nv_db.visible_ratio * 10); */

}

/*==============================================================================

FUNCTION      sns_dd_alsprx_tmd277x_prx_init

DESCRIPTION   initializes the ams AG ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_tmd277x_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  /*  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db; */

  /* SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
  /*                            DBG_DD_ALSPRX_PRX_INIT); */

  /* prx_db_ptr->pend_state = SNS_DD_PEND_STATE_IDLE; */
  /* prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN; */
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_tmd277x_common_init

DESCRIPTION   initializes the ams AG ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_tmd277x_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;
#ifdef ALSPRX_USE_POLLING
  /* dd_ptr->sns_dd_alsprx_common_db.als_timer.active = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.als_timer.defined = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.prx_timer.active = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.prx_timer.defined = FALSE; */
#endif /* ALSPRX_USE_POLLING */
  /* initialize NV item db to default values if needed */
  if (dd_ptr->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
  {
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio      = SNS_DD_IR_TRANS_RATIO; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset     = SNS_DD_DC_OFFSET; */

    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor    = SNS_DD_PRX_FACTOR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor    = SNS_DD_ALS_FACTOR; */

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
#endif
  }

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_tmd277x_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_tmd277x_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_alsprx_tmd277x_common_init(dd_ptr);

  /* Initialize ALS database */
  sns_dd_alsprx_tmd277x_als_init(dd_ptr);

  /* Initialize PRX database */
  sns_dd_alsprx_tmd277x_prx_init(dd_ptr);
}


/*===========================================================================

FUNCTION      sns_dd_alsprx_tmd277x_set_powerstate

DESCRIPTION   Sets the power state of the ISL ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_tmd277x_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uint8_t          i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  TMD2771_MSG_3(MED, "set_powerstate: mode=%d sensor=%d", op_mode, sensor, 1126);

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          TMD2771_MSG_1(MED, "set_powerstate: ONLY PROX", 1126);
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_PEN;
#ifdef DDK
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_PIEN;
          state->chip.setup.prox.odr    = 20;
#endif
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          TMD2771_MSG_1(MED, "set_powerstate: ONLY ALS", 1126);
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_AEN;
#ifdef DDK
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_AIEN;
          state->chip.setup.als.odr     =  7;
#endif
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)
      {
          TMD2771_MSG_1(MED, "set_powerstate: ALL", 1126);
          //SNS_PRINTF_MEDIUM_1(8890);
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_PEN;
          //SNS_PRINTF_MEDIUM_1(8891);
          state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_AEN;
       }

      state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_PON;
      i2c_data = state->chip.setup.reg.enable;
      status   = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &i2c_data);
      if ( status != SNS_DDF_SUCCESS ) {
          TMD2771_MSG_1(MED, "TMD277 var1: %d", 1127);
          return status;
      }
      TMD2771_MSG_2(MED, "set_powerstate: 0x%x ==> rENABLE", i2c_data, 1128);

 #ifdef DDK
      // Set up a timer to read the status register to see if an interrupt has occured.
      status = sns_ddf_timer_start(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj,
                                   MS_TO_US(25));
      if ( status != SNS_DDF_SUCCESS ) {
          TMD2771_MSG_1(MED, "TMD277 var1: %d", 1129);
          return status;
      }

 #endif
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_alsprx_tmd277x_init_data_struct(state);
      sns_dd_alsprx_tmd277x_reset((sns_ddf_handle_t)state);
    }
 #ifdef ALSPRX_DEBUG_BASIC
 #error code not present
 #endif

    i2c_data = SNS_DD_ISL_ALSPRX_PWR_DOWN;
    status   = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &i2c_data);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                   status);
      return status;
    }
    TMD2771_MSG_2(MED, "set_powerstate: 0x%x ==> rENABLE", i2c_data, 1130);

    state->chip.setup.prox.odr = 0;
    state->chip.setup.als.odr  = 0;

#ifdef DDK
    status = sns_ddf_timer_cancel(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj);
    if ( status != SNS_DDF_SUCCESS ) {
        TMD2771_MSG_1(MED, "TMD277 var1: %d", 1131);
        return status;
    }
#endif

  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;

  return status;
}


sns_ddf_status_e tmd277x_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;
    uword_u wordVal;

 #ifdef DDK
    uint8_t byteBuf[16];
 #endif

    ams_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    setup  = &state->chip.setup;


 #ifdef DDK
    status = tmd277x_read_byte(state->port_handle, TMD277x_rID, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
 #endif

    // Reset the device

    // Power off
    byteVal = 0x00;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set the integration cycle time
    byteVal = usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
    status = tmd277x_write_byte(state->port_handle, TMD277x_rATIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set PTIME
    byteVal = usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us);
    status = tmd277x_write_byte(state->port_handle, TMD277x_rPTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set WTIME
    byteVal = 256 - state->chip.setup.wait.wtime;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rWTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set ALS Interrupt Threshold low
    wordVal.word = state->chip.setup.als.thresh_low;
    status = tmd277x_write_word(state->port_handle, TMD277x_rAILTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set ALS Interrupt Threshold high
    wordVal.word = state->chip.setup.als.thresh_high;
    status = tmd277x_write_word(state->port_handle, TMD277x_rAIHTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    
    // Set Prox Interrupt Threshold low
    wordVal.word = 0xFFFF;
    status = tmd277x_write_word(state->port_handle, TMD277x_rPILTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set Prox Interrupt Threshold high
    wordVal.word = 0x0000;
    status = tmd277x_write_word(state->port_handle, TMD277x_rPIHTL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }


    // Set Persistence (Prox and ALS)
    byteVal = setup->persistence;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rPERS, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set Config Reg
    byteVal = setup->reg.config;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rCONFIG, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Set Prox pulses
    byteVal = setup->prox.pulse_count;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rPPULSE, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // LED drive to 100 mA, use IR (CH1) diode for prox, set AGain to 1x
    byteVal = setup->reg.control;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rCONTROL, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

#ifdef DDK
    tmd277x_read_byte(state->port_handle, TMD277x_rCONTROL, &byteVal);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1132, byteVal);
#endif

    // Clear any pending interrupts
    byteVal = 0x07;
    status = tmd277x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Enable Power on (PON).
    byteVal = state->chip.setup.reg.enable;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &byteVal);

#if defined(DD_DEBUG)
    {
        sns_ddf_status_e stat;
        uint8_t i, reg_vals[24];
        stat = tmd277x_read_buf(state->port_handle, TMD277x_rENABLE, 
                                sizeof(reg_vals), reg_vals);
        TMD2771_MSG_1(LOW, "update_registers: registers read status=%d", stat);
        if (stat == SNS_DDF_SUCCESS)
        {
            for (i=0; i<=sizeof(reg_vals)-3; i+=3)
            {
                TMD2771_MSG_3(LOW, "  0x%02x 0x%02x 0x%02x", 
                          reg_vals[i], reg_vals[i+1], reg_vals[i+2]);
            }
        }
    }
#endif
#ifdef DDK
    tmd277x_read_buf(state->port_handle, TMD277x_rENABLE, 20, byteBuf);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1133, byteBuf[0]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1134, byteBuf[1]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1135, byteBuf[2]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1136, byteBuf[3]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1137, byteBuf[4]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1138, byteBuf[5]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1139, byteBuf[6]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1140, byteBuf[7]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1141, byteBuf[8]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1142, byteBuf[9]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1143, byteBuf[10]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1144, byteBuf[11]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1145, byteBuf[12]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1146, byteBuf[13]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1147, byteBuf[14]);
    TMD2771_MSG_2(MED, "TMD277 var1: %d var2: %d", 1148, byteBuf[15]);
#endif

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_tmd277x_reset

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
sns_ddf_status_e sns_dd_alsprx_tmd277x_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status;

    TMD2771_MSG_1(MED, "reset()", 1149);

    sns_ddf_delay(1000);

    // Set the sensor in use
 #if(AMS_SENSOR == TMD2771_SENSOR)
    state->chip.ams_sensor = TMD2771;
 #elif(AMS_SENSOR == TMD2772_SENSOR)
    state->chip.ams_sensor = TMD2772;
 #endif
  //  TMD2771_MSG_1(MED, "TMD277 1371: %d", status);
    // Reset driver state
    ams_tmd277x_set_default_reg_values(state);

    status = tmd277x_update_registers(state);
    TMD2771_MSG_1(MED, "reset: status=%d", status);

    state->first_als_indication = false;
    state->first_prox_indication = false;

    return status;
}


uint32_t tmd277x_als_time_us(sns_dd_alsprx_state_t* state)
{
    // ALS enabled
    TMD2771_MSG_1(MED, "als_time_us: %d", 
              state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us);
    return state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us;
}


uint32_t tmd277x_prox_time_us(sns_dd_alsprx_state_t* state)
{
    // Prox enabled
    TMD2771_MSG_1(MED, "prox_time_us: %d", 
              2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);

    return ( 2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);
}


uint32_t tmd277x_wait_time(sns_dd_alsprx_state_t* state)
{
    uint32_t waitTime;

    waitTime   = state->chip.setup.Min_Integ_Time_us * state->chip.setup.wait.wtime;

    return (waitTime + ((state->chip.setup.wait.lTime * 11) * waitTime));
}


uint32_t tmd277x_compute_total_time_us(sns_dd_alsprx_state_t* state)
{
    return (tmd277x_als_time_us(state) + tmd277x_prox_time_us(state) + tmd277x_wait_time(state));
}


sns_ddf_status_e tmd277x_write_wtime(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    // set WEN in enable register
    if(state->chip.setup.wait.wen == 1)
    {
        state->chip.setup.reg.enable |= TMD277x_ENABLE_MSK_WEN;
        TMD2771_MSG_0(MED, "write_wtime: WENset to 1");
    }
    else
    {
        state->chip.setup.reg.enable &= ~TMD277x_ENABLE_MSK_WEN;
        TMD2771_MSG_0(MED, "write_wtime: WENset to 0");
    }
    byteVal = state->chip.setup.reg.enable;
    status   = tmd277x_write_byte(state->port_handle, TMD277x_rENABLE, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    TMD2771_MSG_2(MED, "write_wtime: 0x%x ==> rENABLE", byteVal, 1150);

    // Set WTIME
    byteVal = 256 - state->chip.setup.wait.wtime;
    status = tmd277x_write_byte(state->port_handle, TMD277x_rWTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    TMD2771_MSG_2(MED, "write_wtime: 0x%x ==> rWTIME", byteVal, 1151);

    // Set Config Reg
    if(state->chip.setup.wait.lTimeChanged)
    {
        if(state->chip.setup.wait.lTime)
        {
            state->chip.setup.reg.config |= TMD277X_CONFIG_WLONG_MSK;
            TMD2771_MSG_1(MED, "config retgset %d", 1);
        }
        else
        {
            state->chip.setup.reg.config &= ~TMD277X_CONFIG_WLONG_MSK;
        }

        byteVal = state->chip.setup.reg.config;
        status = tmd277x_write_byte(state->port_handle, TMD277x_rCONFIG, &byteVal);
        if ( status != SNS_DDF_SUCCESS ) {
            return status;
        }
        TMD2771_MSG_2(MED, "write_wtime: 0x%x ==> rCONFIG", byteVal, 1152);
        state->chip.setup.wait.lTimeChanged = false;
    }
    return status;
}


uint32_t tmd277x_odr_from_uSec(uint32_t requestedTime_us)
{
    uint16_t i;
    uint16_t foundOdr;
    uint32_t midDn;

    foundOdr = MAX_ODR - 1;

    if(requestedTime_us > 750000)
    {
        return 1;
    }

    if(requestedTime_us < 46536)
    {
        return 21;
    }

    for(i = 1; i < MAX_ODR; i++)
    {
        midDn = ((odr_to_uSec[i] - odr_to_uSec[i-1]) / 2) - 1;

        if(requestedTime_us > (odr_to_uSec[i] - midDn))
        {
            foundOdr = i + 1;
            break;
        }
    }
    TMD2771_MSG_1(MED, "odr_from_uSec: %d", foundOdr);
    return foundOdr;
}

void tmd277x_compute_new_wtime(sns_dd_alsprx_state_t* state, uint32_t requestedOdr, uint32_t time_us)
{
    uint32_t requestedOdr_us = 0;
    uint32_t wtime_us;

    uint8_t  wtime;
    uint8_t  wen;
    uint8_t  lTime;

    bool     lTimeChanged;
    if (requestedOdr > 0)
    {
        requestedOdr_us = odr_to_uSec[requestedOdr - 1];
    }

    TMD2771_MSG_1(MED, "new_wtime: req ODR: %d", requestedOdr);
    TMD2771_MSG_2(MED, "new_wtime: time_us(%u) odr_us(%u)", time_us, requestedOdr_us);
    
    // Check to make sure sum of als and prox is less than requestedOdr_us
    // WEN check

    wtime_us = 0;

    if(time_us >= requestedOdr_us)
    {
        wtime_us = 0;
        wen      = 0;
    }
    else
    {
        wtime_us = requestedOdr_us - time_us;
        wen      = 1;
    }

    if (wtime_us > 256 * state->chip.setup.Min_Integ_Time_us)
    {
        wtime        = wtime_us / (12 * state->chip.setup.Min_Integ_Time_us);
        lTime        = 1;
        lTimeChanged = true;
    }
    else
    {
        wtime        = wtime_us / state->chip.setup.Min_Integ_Time_us;
        lTime        = 0;
        lTimeChanged = true;
    }
    TMD2771_MSG_1(MED, "new_wtime: Min_Integ_Time_us=%d", state->chip.setup.Min_Integ_Time_us);
    TMD2771_MSG_3(MED, "new_wtime: wtime_us=%d wtime=%d wen=%d", wtime_us, wtime, wen);
    TMD2771_MSG_2(MED, "new_wtime: ltime=%d changed=%d", lTime, lTimeChanged);
    state->chip.setup.wait.wtime_us     = wtime_us;
    state->chip.setup.wait.wtime        = wtime;
    state->chip.setup.wait.wen          = wen;
    state->chip.setup.wait.lTime        = lTime;
    state->chip.setup.wait.lTimeChanged = lTimeChanged;
}


/*===========================================================================

  FUNCTION:   sns_dd_alsprx_tmd277x_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the TMD 277x ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.

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
static sns_ddf_status_e sns_dd_alsprx_tmd277x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

    uint32_t alsTime_us;
    uint32_t proxTime_us;

    TMD2771_MSG_3(MED, "set_attrib: sensor=%d attrib=%d", sensor, attrib, 1153);

#ifdef ALSPRX_DEBUG_BASIC
#error code not present
#endif /* ALSPRX_DEBUG_BASIC */

    alsTime_us  = 0;
    proxTime_us = 0;

  if ( (dd_handle == NULL) ||
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
  case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      TMD2771_MSG_1(MED, "power mode set %d", 1154);
      status = sns_dd_alsprx_tmd277x_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
      if(status != SNS_DDF_SUCCESS)
      {
          TMD2771_MSG_1(MED, "TMD277 var1: %d", 1154);
          return status;
      }

      break;

  case SNS_DDF_ATTRIB_ODR:
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          state->chip.setup.prox.odr = *(uint32_t*)value;
          TMD2771_MSG_1(MED, "orig prox requested odr  %d", state->chip.setup.prox.odr);
          if (state->chip.setup.prox.odr > SNS_DD_ALSPRX_PRX_FREQ)
          {
            state->chip.setup.prox.odr = SNS_DD_ALSPRX_PRX_FREQ;
            TMD2771_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.prox.odr);
          }
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          state->chip.setup.als.odr = *(uint32_t*)value;
      }
      else
      {
          return SNS_DDF_EINVALID_ATTR;
      }

      TMD2771_MSG_3(MED, "set_attrib: prox.odr=%d als.odr=%d", 
                state->chip.setup.prox.odr, state->chip.setup.als.odr, 1156);
      
      alsTime_us  = tmd277x_als_time_us(state);
      proxTime_us = tmd277x_prox_time_us(state);

      if((state->chip.setup.prox.odr > 0) && (state->chip.setup.als.odr > 0))
      {
          if(state->chip.setup.prox.odr > SNS_DD_ALSPRX_PRX_FREQ)
         {
          state->chip.setup.prox.odr = SNS_DD_ALSPRX_PRX_FREQ;

         }

          if(state->chip.setup.als.odr > SNS_DD_ALSPRX_ALS_FREQ)
         {
          state->chip.setup.als.odr = SNS_DD_ALSPRX_ALS_FREQ;

         }
          // Both als and prox are enabled at the same time
          // Now see which is greater
          if(state->chip.setup.prox.odr > state->chip.setup.als.odr)
          {
              tmd277x_compute_new_wtime(state, state->chip.setup.prox.odr, (alsTime_us + proxTime_us));
          }
          else  // state->chip.setup.als.odr > state->chip.setup.prox.odr
          {
              tmd277x_compute_new_wtime(state, state->chip.setup.als.odr, (alsTime_us + proxTime_us));
          }
      }
      else if((state->chip.setup.prox.odr > 0) || (state->chip.setup.als.odr > 0))
      {
          if(state->chip.setup.als.odr > 0)
          {
              tmd277x_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us);
          }
          else  // state->chip.setup.prox.odr > 0
          {
              tmd277x_compute_new_wtime(state, state->chip.setup.prox.odr, proxTime_us);
          }
      }
      else // disabling both als and prox
      {
          tmd277x_compute_new_wtime(state, 0, 0);
      }
      
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  // If there was change in ODR (non zero) then start a timer to be
  // notified when 1st sample will be ready.
  if((attrib == SNS_DDF_ATTRIB_ODR) && (*(uint32_t*)value != 0))
  {
     // start timer with delay equal to als + prox sample time, plus an
     // extra Min time for a buffer
     //TODO - ideally should include als time only if ALS enabled, and prox
     // only if Prox enabled, and start the time when PON is enabled, but both
     // are always enabled initially.
     uint32_t initial_delay_us = alsTime_us + proxTime_us + state->chip.setup.Min_Integ_Time_us;

     TMD2771_MSG_1(HIGH, "delay = %d (1st cycle only)", initial_delay_us);

     state->first_intg_cycle_complete = false;
     state->first_als_indication = true;
     state->first_prox_indication = true;

     // Disable wait time for the first sample
     state->chip.setup.wait.wen = 0;
     status = tmd277x_write_wtime(state);

     status = sns_ddf_timer_start(state->first_integ_cycle_timer, initial_delay_us);
     if ( status != SNS_DDF_SUCCESS ) 
     {
         TMD2771_MSG_1(MED, "TMD277 status: %d", status);
         return status; 
     }
  }
  else
  {
     status = tmd277x_write_wtime(state);
  }

  return(status);
}

//                              0  1   2    3
static uint8_t gain_value[] = { 1, 8, 16, 120 };

uint8_t getAgain(sns_dd_alsprx_state_t* state)
{
    uint8_t gain;
    uint8_t retVal;

    gain = state->chip.setup.reg.control & 0x03;

    retVal = gain_value[gain];

    return retVal;
}


uint32_t APP_DEV_Get_Max_ALS_Count(sns_dd_alsprx_state_t* state)
{
    uint32_t retVal;
    uint8_t integrationCycles;

    integrationCycles = 256 - usecToIcycles(state->chip.setup.als.atime_us, state->chip.setup.Min_Integ_Time_us);

    if (integrationCycles > 63)
    {
        retVal = 65535;
    }
    else
    {
        retVal = integrationCycles * 1024;
    }

    return retVal;
}


bool APP_DEV_Saturated(sns_dd_alsprx_state_t* state, uint16_t data)
{
    uint32_t max_count;

    max_count = APP_DEV_Get_Max_ALS_Count(state);
    if (data >= max_count)
    {
        TMD2771_MSG_0(HIGH, "ALSPRX als saturation error");
        return true;
    }
    return false;
}


sns_ddf_status_e API_APP_Get_mLUX(sns_dd_alsprx_state_t* state)
{
    int32_t  CPkL;
    int32_t  Lux1;
    int32_t  Lux2;
    int32_t  DGF;
    int32_t  CoefB;
    int32_t  CoefC;
    int32_t  CoefD;
    uint16_t C0Data;
    uint16_t C1Data;
    uint8_t  ALS_xGain;
    uint32_t ALS_Time_us;
    sns_ddf_status_e  status;

    TMD2771_MSG_0(HIGH, "api app get mlux");
    DGF   = state->chip.als_setup.DGF;
    CoefB = state->chip.als_setup.iCoefB;
    CoefC = state->chip.als_setup.iCoefC;
    CoefD = state->chip.als_setup.iCoefD;

    C0Data = state->chip.data.als.ch0;
    C1Data = state->chip.data.als.ch1;
    TMD2771_MSG_0(HIGH, "c0data");
    ALS_xGain   = getAgain(state);
    // Use actual integration time
    ALS_Time_us = (256 - usecToIcycles(state->chip.setup.als.atime_us, state->chip.setup.Min_Integ_Time_us)) * state->chip.setup.Min_Integ_Time_us;
    TMD2771_MSG_0(HIGH, "als gain");

   if (C0Data == 0)   // If C0Data= 0, no need to compute lux. Change 6/21 as per AMS from bad read/invalid data error
    {
     state->chip.data.als.lux    = 0;
     status = SNS_DDF_SUCCESS;
     if (state->chip.data.als.reset)
     {
         TMD2771_MSG_0(HIGH, "C0Data is 0, Lux is 0, Reset is 1, marking data as INVALID");
         state->chip.data.als.Lux_OK = false;
         state->chip.data.als.reset = false;
         status = SNS_DDF_EINVALID_DATA;
     }
     else
     {
         TMD2771_MSG_0(HIGH, "C0Data is 0, Lux is 0, Reset is 0");
         state->chip.data.als.Lux_OK = true;
     }

     TMD2771_MSG_0(HIGH, "C0Data is 0, Lux is 0");
     return status;
    }

   state->chip.data.als.reset = false;
    TMD2771_MSG_0(HIGH, "good read");
    if (APP_DEV_Saturated(state, C1Data)) // Check for saturated data
    {
        TMD2771_MSG_0(HIGH, "C1Data saturated, returning max lux");
        state->chip.data.als.lux = SNS_DD_ALS_MAX_MILLI_LUX;
        return SNS_DDF_SUCCESS;//SNS_DDF_EDATA_BOUND;
    }
    TMD2771_MSG_0(HIGH, "data not saturated");
    CPkL = (ALS_xGain * (int)ALS_Time_us / DGF);
    //max CPkL = 120 * 700,000 / 24 = 3,500,000 / DGF
    //max CPkL = 120 * 200,000 / 24 = 1,000,000 / DGF
    if (CPkL == 0) CPkL = 1;                        // avoid divide by zero
    Lux1 = 0;
    Lux2 = 0;

    Lux1 = 20 * (1000  * C0Data - CoefB * C1Data);
    Lux2 = 20 * (CoefC * C0Data - CoefD * C1Data);
    state->chip.data.als.Lux_Extended = false;
    TMD2771_MSG_0(HIGH, "setting lux1 and lux 2");

    if (APP_DEV_Saturated(state, C0Data)) // Check for saturated data
    {
        TMD2771_MSG_0(HIGH, "ALSPRX als saturation error, returning max lux");
        state->chip.data.als.lux = SNS_DD_ALS_MAX_MILLI_LUX;
        return SNS_DDF_SUCCESS;
    }

    if (Lux2 > Lux1) Lux1 = Lux2;
    if (Lux1 < 0)
    {
        TMD2771_MSG_0(HIGH, "lux1 below 0");
        state->chip.data.als.lux = 0;
        return SNS_DDF_SUCCESS;
    }
    if (Lux1 > 10000000) //10,000,000 Check to keep from overflowing 2,147,483,648 maximum 32 bit signed integer
    {
        Lux1 = 50 * (Lux1 / CPkL);
        TMD2771_MSG_0(HIGH, "setting lux1 a");
    }
    else
    {
        Lux1 = 50 * Lux1 / CPkL;
        TMD2771_MSG_0(HIGH, "setting lux1 b");
    }

    state->chip.data.als.lux = Lux1;

    state->chip.data.als.Lux_OK = true;
    TMD2771_MSG_3(MED, "als.lux: %d lux1: %d lux2: %d", state->chip.data.als.lux , Lux1, Lux2);
    return SNS_DDF_SUCCESS;
}



sns_ddf_status_e tmd277x_get_prox_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e      status = SNS_DDF_SUCCESS;
    uword_u pdata;

    //
    // Get Prox data
    //
    status = tmd277x_read_word(state->port_handle,
                               TMD277x_rPDATA,
                               &pdata);
    if(status != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_0(HIGH, "ALSPRX prx data err");
        return status;
    }

    state->chip.data.prox.data = pdata.word;
    TMD2771_MSG_2(MED, "get_prox_data: rPDATA ==> 0x%x", pdata.word, 1158);

    return status;
}


sns_ddf_status_e tmd277x_get_als_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e      status = SNS_DDF_SUCCESS;
    uword_u c0data;
    uword_u c1data;

    //
    // Get Chan 0 data
    //
    status = tmd277x_read_word(state->port_handle,
                               TMD277x_rC0DATA,
                               &c0data);
    if(status != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_0(HIGH, "ALSPRX als data err1");
        return status;
    }

    state->chip.data.als.ch0 = c0data.word;

    TMD2771_MSG_2(MED, "get_als_data: rC0DATA ==> 0x%x", c0data.word, 1159);

    //
    // Get Chan 1 data
    //
    status = tmd277x_read_word(state->port_handle,
                               TMD277x_rC1DATA,
                               &c1data);
    if(status != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_0(HIGH, "ALSPRX als data err2");
        return status;
    }

    state->chip.data.als.ch1 = c1data.word;
    TMD2771_MSG_2(MED, "get_als_data: rC1DATA ==> 0x%x", c1data.word, 1160);

    return status;
}


sns_ddf_status_e tmd277x_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = 2;

    TMD2771_MSG_1(MED, "prox_samples", 1161);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            TMD2771_MSG_1(MED, "TMD277 var1: %d", 1162);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        TMD2771_MSG_1(MED, "TMD277 var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }
    TMD2771_MSG_1(MED, "prox_samples: Faraway event num: %d", SNS_PRX_FAR_AWAY);
    TMD2771_MSG_1(MED, "prox_samples: nearby event num: %d", SNS_PRX_NEAR_BY);
    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.prox.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.prox.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    TMD2771_MSG_0(HIGH, "PRX Data...");
    TMD2771_MSG_3(FATAL, "prox_samples: Event=%d  Data=%d  Raw Data=%d", 
              state->chip.data.prox.event, data_ptr->samples[0].sample, state->chip.data.prox.data);
    sns_dd_alsprx_tmd277x_log(state, SNS_DDF_SENSOR_PROXIMITY,
                        state->chip.data.prox.event, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.prox.data);

    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e tmd277x_als_sensor_samples(sns_dd_alsprx_state_t    *state,
                                            sns_ddf_sensor_data_s    *data_ptr,
                                            sns_ddf_memhandler_s     *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = 2;

    TMD2771_MSG_1(MED, "als_samples", 1164);

    uint32_t alsluxtoq16 = FX_FLTTOFIX_Q16(state->chip.data.als.lux/1000.0f); // Move rounding to after correction is applied
    TMD2771_MSG_1(MED, "Q16(als lux/1000) 0x%x", alsluxtoq16);


    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(
                                                      memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            TMD2771_MSG_1(MED, "TMD277 var1: %d", 1165);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        TMD2771_MSG_1(MED, "TMD277 var1: %d", 1166);
        return SNS_DDF_EINVALID_PARAM;
    }


    TMD2771_MSG_2(FATAL, "als_samples: luxCorrection=%d nv_db.luxCorrection=%d", 
              state->chip.setup.als.luxCorrection, 
              state->sns_dd_alsprx_common_db.nv_db.luxCorrection);
    data_ptr->samples[0].sample = FX_MUL_Q16(alsluxtoq16,state->chip.setup.als.luxCorrection);
    if ((int32_t)data_ptr->samples[0].sample < 0) // Change to max if overflow
    {
        data_ptr->samples[0].sample = FX_MAX_Q16;
    }
    data_ptr->samples[0].sample &= 0xFFFF0000; // Round to whole Lux
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.als.ch0;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = state->chip.data.als.ch1;
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    TMD2771_MSG_0(HIGH, "ALS Data...");
    TMD2771_MSG_2(FATAL, "TMD LIGHT Data: MLux:%d  Reported Data: %d", state->chip.data.als.lux, data_ptr->samples[0].sample);
    TMD2771_MSG_2(FATAL, "TMD LIGHT Data: Raw Data Ch0: %d Raw Data Ch1: %d", state->chip.data.als.ch0, state->chip.data.als.ch1);

    sns_dd_alsprx_tmd277x_log(state, SNS_DDF_SENSOR_AMBIENT,
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.ch0);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_alsprx_tmd277x_get_data

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
static sns_ddf_status_e sns_dd_alsprx_tmd277x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
    uint8_t i;
    sns_ddf_time_t        timestamp;
    sns_ddf_status_e      status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_alsprx_state_t *state = dd_handle;

    TMD2771_MSG_0(HIGH, "ALSPRX Get Data ");

    //SLH - this should really be replaced with an rSTATUS read to check if data is ready or not
    if(!state->first_intg_cycle_complete)
    {
        TMD2771_MSG_0(HIGH, "first integ cycle not complete yet. return pending.");
        return SNS_DDF_PENDING;
    }

    TMD2771_MSG_0(HIGH, "ALSPRX Get Data prx");
    if((status = tmd277x_get_prox_data(state)) != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_1(HIGH, "ALSPRX Get Data prx err1: %d", status);
        return status;
    }
    if((status = sns_dd_alsprx_tmd277x_prx_binary(state)) != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_1(HIGH, "ALSPRX Get Data prx err2: %d", status);
        return status;
    }

    TMD2771_MSG_0(HIGH, "ALSPRX Get Data als");
    if((status = tmd277x_get_als_data(state))  != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_1(HIGH, "ALSPRX Get Data als err1: %d", status);
        return status;
    }
    if((status = API_APP_Get_mLUX(state))      != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_1(HIGH, "ALSPRX Get Data als err2: %d", status);
        return status;
    }

    timestamp = sns_ddf_get_timestamp();
    data_ptr  = sns_ddf_memhandler_malloc(
                                         memhandler,
                                         num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(data_ptr == NULL)
    {
        TMD2771_MSG_1(HIGH, "ALSPRX Get Data malloc error, Size: %d", (num_sensors * sizeof(sns_ddf_sensor_data_s)));
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    TMD2771_MSG_0(HIGH, "ALSPRX Get Data, processing data for smgr");

    for(i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor    = sensors[i];
        data_ptr[i].status    = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            TMD2771_MSG_0(HIGH, "ALSPRX Get Data, returning prx data to smgr");
            tmd277x_prox_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            TMD2771_MSG_0(HIGH, "ALSPRX Get Data, returning als data to smgr");
            tmd277x_als_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        default:
            TMD2771_MSG_0(MED, "Neither ALS or Prox called");
            break;


        }
    }

    return SNS_DDF_SUCCESS;
}


static void ams_set_lux_coefs(sns_dd_alsprx_state_t* state)
{
    state->chip.als_setup.DGF    = ams_init_lux_coefs[state->chip.ams_sensor].DGF;
    state->chip.als_setup.iCoefB = ams_init_lux_coefs[state->chip.ams_sensor].iCoefB;
    state->chip.als_setup.iCoefC = ams_init_lux_coefs[state->chip.ams_sensor].iCoefC;
    state->chip.als_setup.iCoefD = ams_init_lux_coefs[state->chip.ams_sensor].iCoefD;
}


/*
 * Provides initial operational parameter defaults.
 * These defaults may be changed through the device's get/set
 * attribute files.
 */
void ams_tmd277x_set_default_reg_values(sns_dd_alsprx_state_t* state)
{
    /* uint16_t tmp; */
    TMD2771_MSG_0(HIGH, "in set default reg values ");
    /* Operational parameters */
    ams_set_lux_coefs(state);

    state->chip.setup.Min_Integ_Time_us = ams_min_integ_time_us[state->chip.ams_sensor];

    /* must be a multiple of 50mS */
    state->chip.setup.als.atime_us = MS_TO_US(50);

    /* Min Proximity Integration time us */
    state->chip.setup.prox.ptime_us = state->chip.setup.Min_Integ_Time_us;

    /* this is actually an index into the gain table */
    /* assume clear glass as default */
 #if(AMS_SENSOR == TMD2771_SENSOR)
    state->chip.setup.als.again  = TMD2771_AGAIN_1x;
    state->chip.setup.prox.pgain = 0;      // 2771 has NO prox gain, set as zero
 #elif(AMS_SENSOR == TMD2772_SENSOR)
    state->chip.setup.als.again  = TMD2772_AGAIN_1x;
    state->chip.setup.prox.pgain = TMD2772_PGAIN_8x;
 #elif(AMS_SENSOR == CT406_SENSOR)
    state->chip.setup.als.again  = TMD2772_AGAIN_1x;
    state->chip.setup.prox.pgain = TMD2772_PGAIN_8x;
 #endif
    TMD2771_MSG_0(HIGH, "in set default reg values -gain ");
    /* default gain trim to account for aperture effects */
    state->chip.setup.als.gain_trim = 1000;

    /* Known external ALS reading used for calibration */
    state->chip.setup.als.cal_target = 130;
    /* Number of 'out of limits' ADC readings */
    /* 1 Consecutive prox values out of range before generating prox interrupt */
    /* Will generate an ALS interrupt after every ALS value out of range. */
    state->chip.setup.persistence = 0x11;

    /* Default interrupt(s) enabled.
     * 0x00 = none, 0x10 = als, 0x20 = prx 0x30 = bth */
    state->chip.setup.interrupts_en = 0x00;
    state->chip.setup.reg.enable    = SNS_DD_ISL_ALSPRX_PWR_DOWN;

    /*default threshold (adjust either manually or with cal routine*/
    /* initialize detect and release only if not already obtained from nv */
    if (state->nv_db_size == 0)
    {
        state->chip.setup.prox.detect  = 600;
        state->chip.setup.prox.release = 400;
    }
    /* state->chip.setup.prox.thres = 500; */
    /* state->chip.setup.prox.mul   = 20; */

    /* wait time between prox and als */
    state->chip.setup.wait.wtime = 1;
    state->chip.setup.wait.lTime = 0;
    state->chip.setup.wait.wen   = 0;

    /* Config register */
    state->chip.setup.reg.config = 0;

    /* tmp = state->chip.setup.prox.thres * state->chip.setup.prox.mul/100; */

    /* state->chip.setup.prox.detect  = state->chip.setup.prox.thres + tmp; */
    /* state->chip.setup.prox.release = state->chip.setup.prox.thres - tmp; */
    TMD2771_MSG_0(HIGH, "in set default reg values -prox detect ");
    /* Set the prox thresh for detect */
    state->chip.setup.prox.thresh_low  =  0x0000;
    state->chip.setup.prox.thresh_high =  state->chip.setup.prox.detect;

    state->chip.data.prox.event        = SNS_PRX_FAR_AWAY;

    state->chip.setup.prox.max_samples_cal = 100;
    state->chip.setup.prox.pulse_count     = 8;
    state->chip.setup.prox.odr             = 0;
    state->chip.setup.als.odr              = 0;
    state->chip.setup.reg.control          =
        TMD2771_PDRIVE_50pc          |  // PDRIVE  limit max drive to TMD2771_PDRIVE_50pc
        TMD2772_PDIODE_CH1           |  // PDIODE
        state->chip.setup.prox.pgain |  // PGAIN
        state->chip.setup.als.again;    // AGAIN
    state->chip.data.als.reset            = true;
}

/*===========================================================================

FUNCTION      ams_tmd277x_set_reg_values_from_nv

DESCRIPTION   Reads values from registry NV param store and assigns to sensor

DEPENDENCIES  None

RETURN VALUE  No Return

SIDE EFFECT   None

===========================================================================*/


void ams_tmd277x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state)
{
    // Now set NV values

    // device
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.device);
    if((state->sns_dd_alsprx_common_db.nv_db.device != 0) && (state->sns_dd_alsprx_common_db.nv_db.device <= 9))
    {
        state->chip.ams_sensor = (TMD277x_NAME_DEF)state->sns_dd_alsprx_common_db.nv_db.device;
    }

    // DGF
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.DGF);
    if(state->sns_dd_alsprx_common_db.nv_db.DGF != 0)
    {
        state->chip.als_setup.DGF = state->sns_dd_alsprx_common_db.nv_db.DGF;
    }

    // CoefB
    SNS_PRINTF_MEDIUM_2(1169, state->sns_dd_alsprx_common_db.nv_db.iCoefB);
    if(state->sns_dd_alsprx_common_db.nv_db.iCoefB != 0)
    {
        state->chip.als_setup.iCoefB = state->sns_dd_alsprx_common_db.nv_db.iCoefB;
    }

    // CoefC
    SNS_PRINTF_MEDIUM_2(1170, state->sns_dd_alsprx_common_db.nv_db.iCoefC);
    if(state->sns_dd_alsprx_common_db.nv_db.iCoefC != 0)
    {
        state->chip.als_setup.iCoefC = state->sns_dd_alsprx_common_db.nv_db.iCoefC;
    }

    // CoefD
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.iCoefD);
    if(state->sns_dd_alsprx_common_db.nv_db.iCoefD != 0)
    {
        state->chip.als_setup.iCoefD = state->sns_dd_alsprx_common_db.nv_db.iCoefD;
    }

    // proxDetect
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxDetect);
    if(state->sns_dd_alsprx_common_db.nv_db.proxDetect != 0)
    {
        state->chip.setup.prox.detect = state->sns_dd_alsprx_common_db.nv_db.proxDetect;
    }

    // proxRelease
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxRelease);
    if(state->sns_dd_alsprx_common_db.nv_db.proxRelease != 0)
    {
        state->chip.setup.prox.release = state->sns_dd_alsprx_common_db.nv_db.proxRelease;
    }

    // luxCorrection
    // SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.luxCorrection);
    if(state->sns_dd_alsprx_common_db.nv_db.luxCorrection != 0)
    {

        state->chip.setup.als.luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;
        TMD2771_MSG_1(MED, "lux corr written in default %d", state->chip.setup.als.luxCorrection);
        if(state->chip.setup.als.luxCorrection <= 0)
        {
          state->chip.setup.als.luxCorrection = 1<<16;
          TMD2771_MSG_1(MED, "lux corr set to 1<<16 %d", state->chip.setup.als.luxCorrection);
        }
    }

    // ALS Thresh High
    //SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high);
    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high != 0)
    {

        state->chip.setup.als.thresh_high = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high;
        TMD2771_MSG_1(MED, "ALS High written %d", state->chip.setup.als.thresh_high);
    }

    // ALS Thresh Low
  //  SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low);
    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low != 0)
    {

        state->chip.setup.als.thresh_low = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low;
        TMD2771_MSG_1(MED, "ALS Low Written %d", state->chip.setup.als.thresh_low);
    }



}
#define NUM_OF_LUX_TO_AVE  8
#define LUX_AVG_DIVIDER (NUM_OF_LUX_TO_AVE * 1000)



static sns_ddf_status_e compute_lux_ave(sns_dd_alsprx_state_t* state)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status;

    luxSum = 0;

    TMD2771_MSG_0(HIGH, "about to get als data and mlux ");


    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        status = tmd277x_get_als_data(state);
        if( status != SNS_DDF_SUCCESS )
        {
            TMD2771_MSG_0(HIGH, "failed in get als data ");
        }
        status = API_APP_Get_mLUX(state);
        if( status != SNS_DDF_SUCCESS )
        {
            TMD2771_MSG_0(HIGH, "failed in app get mlux ");
        }
        luxSum += state->chip.data.als.lux;
        TMD2771_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        // Wait some time before another reading
        sns_ddf_delay(25000);
        TMD2771_MSG_0(HIGH, "after 25000 us delay ");
    }
    TMD2771_MSG_2(MED, "luxsum %d luxsum/8/1000: %d", luxSum, state->chip.data.als.luxAve);

    state->chip.data.als.luxAve = (luxSum / LUX_AVG_DIVIDER);
        TMD2771_MSG_2(MED, "luxsum %d luxsum/8/1000: %d", luxSum, state->chip.data.als.luxAve);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_tmd277x_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/


sns_ddf_status_e ams_tmd277x_calibrate_als(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    uint32_t  tgtLux,avgLux,tgtLuxLowRange, tgtLuxHiRange;
    avgLux=1;
    tgtLux=1;

    // compare the average lux to the passed in calibration lux,
    TMD2771_MSG_0(HIGH, "in calibrate als next compute lux ");
    if ((status = compute_lux_ave(state)) != SNS_DDF_SUCCESS)
    {
        TMD2771_MSG_0(HIGH, "computelux ave failed ");
        return status;
    }

     TMD2771_MSG_0(HIGH, "out of compute lux ");
     // Find Lux mid way between calLuxLower and calLuxUpper
     tgtLux = (state->sns_dd_alsprx_common_db.nv_db.calLuxLower + state->sns_dd_alsprx_common_db.nv_db.calLuxUpper) / 2;

     tgtLuxLowRange = 0.5*tgtLux;
     tgtLuxHiRange = 1.5*tgtLux;
     TMD2771_MSG_2(MED, "tgtLux low %d tgt lux hi %d", tgtLuxLowRange, tgtLuxHiRange);
     avgLux = state->chip.data.als.luxAve;
     TMD2771_MSG_1(MED, "Avg Lux: %d",  avgLux);

    //check if avg lux is outside expected range of target lux. If so, return efail
    if ((avgLux <= tgtLuxLowRange) || (avgLux >= tgtLuxHiRange))
    {
       TMD2771_MSG_3(HIGH, "Avg Lux in Test %d lux out of Expected range. Below %d lux or above %d lux. Setting status to efail.",avgLux,tgtLuxLowRange,tgtLuxHiRange);
       return SNS_DDF_EFAIL;
    }

     tgtLux = FX_FLTTOFIX_Q16(tgtLux); 
     avgLux = FX_FLTTOFIX_Q16(avgLux);

     TMD2771_MSG_2(MED, "tgtLux q16 0x:%x avglux q16: 0x%x", tgtLux, avgLux);
     state->sns_dd_alsprx_common_db.nv_db.luxCorrection = FX_DIV_Q16(tgtLux,avgLux);
     TMD2771_MSG_1(MED, "new val of state->sns_dd_alsprx_common_db.nv_db.luxCorrection q16: 0x%x",  state->sns_dd_alsprx_common_db.nv_db.luxCorrection);

    return status;
}

/*===========================================================================

FUNCTION      sns_dd_ams_tmd277x_alsprx_run_test

DESCRIPTION   OEM Self Test to calibrate ALS Lux. Takes several lux readings
              and calculates scaling factor for ALS reading

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/

sns_ddf_status_e sns_dd_ams_tmd277x_alsprx_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t       smgr_handle, port_handle;
    uint32_t alsTime_us;
    /*uint32_t proxTime_us;*/

    if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        TMD2771_MSG_0(HIGH, "in selftest sensor_ambient ");

        // Test the ALS
        if(test == SNS_DDF_TEST_OEM)
        {
            smgr_handle = state->smgr_handle;
            TMD2771_MSG_0(HIGH, "in selftest sensor_ambient OEM test ");
            // Perform ams specific test to Calibrate ALS
            // controlled by registry paramter calibratePhone.
            if(state->sns_dd_alsprx_common_db.nv_db.calibratePhone)
            {
                // Set Default values already est in reset
                //ams_tmd277x_set_default_reg_values(state);

                status = sns_dd_alsprx_tmd277x_reset(state);

                if( status != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_0(HIGH, "failed in reset ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_TMD277X_ALS_CAL_FAILED_RST);
                    return status;
                }
                TMD2771_MSG_0(HIGH, "after reset");
                alsTime_us=0;
                /*proxTime_us=0;*/
                state->chip.setup.als.odr = 10;
                state->chip.setup.prox.odr = 10;

                //  enable sensor
                status = sns_dd_alsprx_tmd277x_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);
                if( status != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_0(HIGH, "failed in set power state -sensor enable ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_TMD277X_ALS_CAL_FAILED_ENB);
                    return status;
                }

                alsTime_us  = tmd277x_als_time_us(state);
                /*proxTime_us = tmd277x_prox_time_us(state);*/
                tmd277x_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us);

                status = tmd277x_write_wtime(state);
                sns_ddf_delay(125000); // Delay for write time to filter to the part
                // update als scaling
                TMD2771_MSG_0(HIGH, "into als scaling");
                status = ams_tmd277x_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_0(HIGH, "calib als failed ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_TMD277X_ALS_CAL_FAILED_LUX);
                    return status;
                }
                TMD2771_MSG_0(HIGH, "out of als scaling");
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    TMD2771_MSG_0(HIGH, "failed in selftest -no registry notify");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_TMD277X_ALS_CAL_FAILED_REG);
                    return status;
                }
                TMD2771_MSG_0(HIGH, "successful registry update notify complete");
            }
        }
        else if (test == SNS_DDF_TEST_SELF)
        {
            uint8_t i2c_buff;
            uint8_t bytes_read;

            port_handle = state->port_handle;

            status = sns_ddf_read_port(port_handle,
                           TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | TMD277x_CHIPID,
                           &i2c_buff,
                           1,
                           &bytes_read
                           );

            if(status != SNS_DDF_SUCCESS)
            {
               return status;
            }
          
            if(( i2c_buff != DEVICEID_TMD27711 ) && ( i2c_buff != DEVICEID_TMD27713 )
              && ( i2c_buff != DEVICEID_TMD27721 ) && ( i2c_buff != DEVICEID_TMD27723 ))
            {
               TMD2771_MSG_1(HIGH, "TMD277x Chip ID = %d", i2c_buff);
               return SNS_DDF_EDEVICE;
            }
        }
        else
        {
           TMD2771_MSG_0(HIGH, "Invalid test");
           return SNS_DDF_EINVALID_TEST;
        }
    }
    else if(sensor == SNS_DDF_SENSOR_PROXIMITY)
    {
       if (test == SNS_DDF_TEST_SELF)
       {
           uint8_t i2c_buff;
           uint8_t bytes_read;

           port_handle = state->port_handle;

           status = sns_ddf_read_port(port_handle,
                           TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | TMD277x_CHIPID,
                           &i2c_buff,
                           1,
                           &bytes_read
                           );

           if(status != SNS_DDF_SUCCESS)
           {
              return status;
           }
          
           if(( i2c_buff != DEVICEID_TMD27711 ) && ( i2c_buff != DEVICEID_TMD27713 )
              && ( i2c_buff != DEVICEID_TMD27721 ) && ( i2c_buff != DEVICEID_TMD27723 ))
           {
              TMD2771_MSG_1(HIGH, "TMD277x Chip ID = %d", i2c_buff);
              return SNS_DDF_EDEVICE;
           }
       }
       else
       {
           TMD2771_MSG_0(HIGH, "Invalid test");
           return SNS_DDF_EINVALID_TEST;
       }
    }
    else
    {
        TMD2771_MSG_0(HIGH, "Invalid Sensor type");
        return SNS_DDF_EINVALID_TEST;
    }

    return SNS_DDF_SUCCESS;
}

/*=============================================================================
 * probe function for TMD2771 DIGITAL ALS and PROXIMITY MODULE.
 ============================================================================*/

static sns_ddf_status_e sns_dd_alsprx_tmd277x_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  *num_sensors = 0;
  *sensors = NULL;
  // Open communication port to the device
  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  status = sns_ddf_read_port(port_handle,
                             TMD277x_CMD_REG | TMD277x_CMD_AUTO_INC | TMD277x_CHIPID,
                             &i2c_buff,
                             1,
                             &bytes_read
                             );

  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  if(( i2c_buff != DEVICEID_TMD27711 )
    && ( i2c_buff != DEVICEID_TMD27713 )
    && ( i2c_buff != DEVICEID_TMD27721 )
    && ( i2c_buff != DEVICEID_TMD27723 ))
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Register contents match up with the expected chip ids. This is probably a tmd277x */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  }
  else
  {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;

} /* End sns_dd_alsprx_tmd277x_probe */

