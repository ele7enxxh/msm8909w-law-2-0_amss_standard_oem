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
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the AMS ALS(Light) and Proximity Sensor
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
07/20/2014   VY      Disabled logging in uImage
06/11/2014   vy      Refactored for uImage
08/15/13     vh      Eliminated Klocwork warnings
06/27/13     fv      Convert tmd377x driver to tmg399x
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
12/11/09     fv      Convert Intersil driver to drive AMS TSL/TMD 377x ALS/PROX sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_ams_tmg399x_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
//#include "sns_dd.h"
#include <string.h>
//#include <stdlib.h>

#ifndef DDK
#include "sns_memmgr.h"
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
/* static ams_als_lux_coef ams_tmg399x_init_lux_coefs[] = { */
/*     {53, 2000, 600, 1000},   /\* TSL2571 *\/ */
/*     {53, 2000, 600, 1000},   /\* TSL2671 *\/ */
/*     {53, 2000, 600, 1000},   /\* TSL2771 *\/ */

/*     {24, 2000, 600, 1000},   /\* TMD2671 *\/ */
/*     {24, 2000, 600, 1000},   /\* TMD2771 *\/ */

/*     {60, 1870, 630, 1000},   /\* TSL2572 *\/ */
/*     {60, 1870, 630, 1000},   /\* TSL2672 *\/ */
/*     {60, 1870, 630, 1000},   /\* TSL2772 *\/ */

/*     {20, 1750, 630, 1000},   /\* TMD2672 *\/ */
/*     {20, 1750, 630, 1000}    /\* TMD2772 *\/ */
/* }; */


//
// CHECK THIS TABLE
//
//
// Convert ODR in Hz to uSec
//
#define MAX_ODR  40

#define NUM_ALS_SAMPLES_RET 7
#define NUM_PROX_SAMPLES_RET 2

#if 0
static uint32_t ams_tmg399x_odr_to_uSec[] = {
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

#define ODR_USEC_MAX  ams_tmg399x_odr_to_uSec[0]
#define ODR_USEC_MIN  ams_tmg399x_odr_to_uSec[MAX_ODR-1]
#endif


//
//++ TMG399x ++
//
static uint8_t const ams_tmg399x_als_gains[] = {1, 4, 16, 64};


//
//-- TMG399x --
//

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_init
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

static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_ams_tmg399x_alsprx_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

static void sns_dd_ams_tmg399x_alsprx_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset
(
  sns_ddf_handle_t dd_handle
);


extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);


static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );

sns_ddf_status_e ams_tmg399x_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val);

sns_ddf_status_e ams_tmg399x_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val);

sns_ddf_status_e ams_tmg399x_write_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* buf, uint8_t size);

sns_ddf_status_e ams_tmg399x_read_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t byteCnt, uint8_t* buf);

sns_ddf_status_e ams_tmg399x_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val);

sns_ddf_status_e ams_tmg399x_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val);

sns_ddf_status_e ams_tmg399x_write_spec_func(sns_ddf_handle_t port_handle, uint8_t spec_func);

uint8_t ams_tmg399x_usecToIcycles(uint32_t time_us, uint32_t minIntegTime_us);

sns_ddf_status_e ams_tmg399x_read_all_data(sns_dd_alsprx_state_t* state);

sns_ddf_status_e ams_tmg399x_get_prox_data(sns_dd_alsprx_state_t* state);

sns_ddf_status_e ams_tmg399x_get_als_data(sns_dd_alsprx_state_t* state);

sns_ddf_status_e ams_tmg399x_get_lux(sns_dd_alsprx_state_t* state);

static sns_ddf_status_e ams_tmg399x_prox_sensor_samples(
  sns_dd_alsprx_state_t   *state,
  sns_ddf_sensor_data_s   *data_ptr,
  sns_ddf_memhandler_s    *memhandler,
  sns_ddf_sensor_sample_s *sample_data
);

static sns_ddf_status_e ams_tmg399x_als_sensor_samples(
  sns_dd_alsprx_state_t    *state,
  sns_ddf_sensor_data_s    *data_ptr,
  sns_ddf_memhandler_s     *memhandler,
  sns_ddf_sensor_sample_s  *sample_data
);

static void sns_dd_ams_tmg399x_alsprx_log(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data1,
  uint32_t               data1_q16,
  uint32_t               data2,
  uint32_t               data2_q16,
  uint32_t               raw_data 
);


static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_prx_binary(
  sns_dd_alsprx_state_t* state
);

static sns_ddf_status_e sns_dd_alsprx_tmg399x_set_als_thresh(
  sns_dd_alsprx_state_t* state
);

static void ams_tmg399x_read_status_reg(
  sns_dd_alsprx_state_t* state, 
  sns_ddf_time_t timestamp, 
  AMS_tmg399x_SIG_TYPE sig_type
);

static void ams_tmg399x_calc_cpl(ams_tmg399x_chip *chip);

static sns_ddf_status_e ams_tmg399x_set_als_gain(sns_dd_alsprx_state_t* state, int gain);


/*============================================================================
                           STRUCTURES
============================================================================*/
sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list =
{
  &sns_dd_ams_tmg399x_alsprx_init,
  &sns_dd_ams_tmg399x_alsprx_get_data,
  &sns_dd_ams_tmg399x_alsprx_set_attrib,
  &sns_dd_ams_tmg399x_alsprx_get_attrib,
  &sns_dd_ams_tmg399x_alsprx_handle_timer,
  &sns_dd_ams_tmg399x_alsprx_handle_irq,
  &sns_dd_ams_tmg399x_alsprx_reset,
  &sns_dd_ams_tmg399x_alsprx_run_test,
  &sns_dd_ams_tmg399x_alsprx_enable_sched_data
};


/*============================================================================
                           FUNCTION DEFINITIONS
============================================================================*/
//
// +++++++++++++++++++  AMS-TAOS USA Code   +++++++++++++++++++
//

/*!
 ***************************************
 * @brief Function to write a single byte
 * to the TMG399x at register /b reg
 *
 * @param reg : Register address to write byte
 * @param byteVal : byte value to write at register
 *
 * @return status of write
 ***************************************
 */
sns_ddf_status_e ams_tmg399x_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    DDF_MSG_0(HIGH, "ams_tmg399x_write_byte");
    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                byte_val,
                                1,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                &word_val->byte[0],
                                2,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_write_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* buf, uint8_t size)
{
    sns_ddf_status_e status;
    uint8_t          write_count;


    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                buf,
                                size,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t byteCnt, uint8_t* buf)
{
    sns_ddf_status_e status;
    uint8_t          read_count;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
                               buf,
                               byteCnt,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
                               byte_val,
                               1,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;
    uword_u          word_tmp;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
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
 * @brief Write special function to the TMG399x command register
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
sns_ddf_status_e ams_tmg399x_write_spec_func(sns_ddf_handle_t port_handle, uint8_t spec_func)
{
    sns_ddf_status_e status;
    uint8_t          rw_count;
    uint8_t          i2c_data;

    i2c_data   = (AMS_tmg399x_CMD_REG | AMS_tmg399x_CMD_SPL_FN | spec_func);

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


uint8_t ams_tmg399x_usecToIcycles(uint32_t time_us, uint32_t minIntegTime_us)
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
FUNCTION      sns_dd_ams_tmg399x_alsprx_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
static void sns_dd_ams_tmg399x_alsprx_log(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data1,
  uint32_t               data1_q16,
  uint32_t               data2,
  uint32_t               data2_q16,
  uint32_t               raw_data 
)
{
#ifndef DDK
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   DDF_MSG_0(HIGH, "TMG399x Log Data");

   if( sns_ddf_smgr_is_in_uimage() )
     return;
     
   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     DDF_MSG_0(HIGH, "in sns_dd_ams_tmg399x_alsprx_log");
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
       DDF_MSG_1(HIGH, "TMG399x Log Data - logpkt_malloc failed with err: %d", err_code);
       dd_ptr->dropped_logs++;
   }
#endif
}



static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_enable_sched_data(
  sns_ddf_handle_t dd_handle,
  sns_ddf_sensor_e sensor,
  bool             enable
)
{
    sns_dd_alsprx_state_t* state = dd_handle;

    uint8_t i2c_data;
    uint8_t byteVal;
    uint8_t readByte;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    ams_setup*       setup;

    setup = &state->chip.setup;

    //
    // Clear interrupts
    // Clear any pending interrupts
    byteVal = 0x07;
    DDF_MSG_0(HIGH, "sns_dd_ams_tmg399x_alsprx_enable_sched_data");
    status = ams_tmg399x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    DDF_MSG_2(MED, "TMG399 enable: %d var2: %d", 1100, enable);

    // set up for run.
    //

    if(sensor == SNS_DDF_SENSOR_PROXIMITY)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_PEN) == 0x00)
            {
                // if prox was not enabled previously, write new ptime
                byteVal = ams_tmg399x_usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us);
                status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPTIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                DDF_MSG_2(MED, "TMG399 enb sch data prx var1: %d var2: %d", 1102, byteVal);

                // DEBUG READ
                ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rPTIME, &readByte);
                DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1103, readByte);
            }
            state->chip.setup.prox.enabled = true;
            state->chip.setup.reg.enable  |= (AMS_tmg399x_ENABLE_MSK_PIEN | AMS_tmg399x_ENABLE_MSK_PEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(AMS_tmg399x_ENABLE_MSK_PIEN | AMS_tmg399x_ENABLE_MSK_PEN);
        }
    }
    else if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_AEN) == 0x00)
            {
                // if ALS not enabled, write new atime
                byteVal = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
                status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rATIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                DDF_MSG_2(MED, "TMG399 TMD277 enb sch data als: %d var2: %d", 1104, setup->als.atime_us);
                DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1105, byteVal);
            }
            state->chip.setup.als.enabled = true;
            state->chip.setup.reg.enable |= (AMS_tmg399x_ENABLE_MSK_AIEN | AMS_tmg399x_ENABLE_MSK_AEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(AMS_tmg399x_ENABLE_MSK_AIEN | AMS_tmg399x_ENABLE_MSK_AEN);
        }
    }

    state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PON;
    i2c_data = state->chip.setup.reg.enable;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1106, i2c_data);

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_prx_binary(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t pdata;
    uint16_t detect;
    uint16_t release;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;


    pdata   = state->chip.data.prox.data;
    DDF_MSG_1(MED, "pdata %d",  pdata );
    detect  = state->chip.setup.prox.detect;
    DDF_MSG_1(MED, "detect %d",  detect );
    release = state->chip.setup.prox.release;
    DDF_MSG_1(MED, "release %d",  release );

    if(state->chip.data.prox.event == SNS_PRX_FAR_AWAY)
    {
        if(pdata > detect)
        {
            state->chip.data.prox.event = SNS_PRX_NEAR_BY;

            state->chip.setup.prox.thresh_high = 0xFFFF;
            state->chip.setup.prox.thresh_low  = state->chip.setup.prox.release;

            // Set Prox interrupt low
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHL, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 pdata > detect.. release: %d var2: %d", state->chip.setup.prox.release, wordVal.byte[0]);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHH, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 prox high var1: %d var2: %d", 1108, wordVal.byte[0]);
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
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHL, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 pdata < release: %d var2: %d", 1109, wordVal.byte[0]);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHH, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1110, wordVal.byte[0]);
        }
    }
    return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_tmg399x_set_als_thresh

DESCRIPTION   This function is called in DRI mode- sets the ALS thresholds to
              make ALS sensor function like an event sensor.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_tmg399x_set_als_thresh(
  sns_dd_alsprx_state_t* state
)
{
    uint32_t clearData;
    uint32_t t_low;
    uint32_t t_high;
    uword_u  wordVal;
    uword_u  readALSTh;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DDF_MSG_0(HIGH, "sns_dd_alsprx_tmg399x_set_als_thresh:");
    DDF_MSG_1(MED, "ALS High Threshold %d",  state->chip.setup.als.thresh_high );
	DDF_MSG_1(MED, "ALS Low Threshold %d",  state->chip.setup.als.thresh_low);

    clearData   = state->chip.als_inf.clear_raw;
    DDF_MSG_1(MED, "clearData %d",  clearData );

    t_low = (clearData * (100 - state->chip.setup.als.thresh_low)) / 100;   //set to 20 as default
    DDF_MSG_1(MED, "als t low %d",  t_low );

    t_high = (clearData * (100 + state->chip.setup.als.thresh_high)) / 100; //set to 20 as default
    DDF_MSG_1(MED, "als t high %d",  t_high);

	//set als threshold low at -x% of data and set als threshold high at x% of the pdata, x defined in registry

    wordVal.word = t_low;
    DDF_MSG_1(MED, "als t low wordval.word %d",  wordVal.word);
    status = ams_tmg399x_write_word(state->port_handle, TMG399X_AILTHL, &wordVal);
        if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
              return status;
           }
    status = ams_tmg399x_read_word(state->port_handle, TMG399X_AILTHL, &readALSTh);
    DDF_MSG_1(MED, "als t low wordval.word readback %d",  readALSTh.word);
	    if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low readback error %d", readALSTh.word);
              return status;
           }

    wordVal.word = t_high;
    DDF_MSG_1(MED, "als t high wordval.word %d",  wordVal.word);
    status = ams_tmg399x_write_word(state->port_handle, TMG399X_AIHTHL, &wordVal);

       if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
             return status;
          }
    status = ams_tmg399x_read_word(state->port_handle, TMG399X_AIHTHL, &readALSTh);
   DDF_MSG_1(MED, "als t high wordval.word readback %d",  readALSTh.word);
	   if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS thresh high readback error %d", readALSTh.word);
             return status;
          }

    return status;
}


static void ams_tmg399x_read_status_reg(
  sns_dd_alsprx_state_t* state, 
  sns_ddf_time_t timestamp, 
  AMS_tmg399x_SIG_TYPE sig_type
)
{
    uint8_t  statusReg;
    //uint8_t  enableReg;
    uint8_t  numOfSensors;
    uint8_t  i;
    uint8_t  clrInterrupts;
    bool     hasInterrupt;
    sns_ddf_status_e eresult;
    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    sns_ddf_sensor_e sensors[2];


    numOfSensors  = 0;
    clrInterrupts = 0;
    hasInterrupt  = false;

/* #ifdef DDK */
/*     uint8_t byteBuf[20]; */
/*     ams_tmg399x_read_buf(state->port_handle, */
/*                          AMS_tmg399x_rENABLE, */
/*                          16, */
/*                          byteBuf); */
/*     ams_tmg399x_read_buf(state->port_handle, */
/*                          AMS_tmg399x_rPDATA, */
/*                          2, */
/*                          byteBuf); */
/* #endif */

    /* ams_tmg399x_read_byte(state->port_handle, */
    /*                       AMS_tmg399x_rSTATUS, */
    /*                       &statusReg); */

    // Read all of the data
    ams_tmg399x_read_all_data(state);

    statusReg = state->chip.shadow[TMG399X_STATUS];
    //enableReg = state->chip.shadow[TMG399X_ENABLE];
    DDF_MSG_1(MED, "TMG399 in read_status_reg  status_reg is: %d" , statusReg);
    //DDF_MSG_1(MED, "TMG399 in read_status_reg  enable_reg is: %d" , enableReg);

    if(statusReg & AMS_tmg399x_STA_ALS_INTR)
    {
        //
        // ALS interrupt
        //

        DDF_MSG_1(MED, "TMG399 ALS Interrupt  (int reg): %d", AMS_tmg399x_STA_ALS_INTR);

        clrInterrupts |= AMS_tmg399x_CMD_ALS_INTCLR;
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
           DDF_MSG_0(HIGH, "TIMER_TYPE:");
            // Read ALS data
            ams_tmg399x_get_als_data(state);

            /* set als threshold to be +/- 20% of current value for next interrupt.*/
            sns_dd_alsprx_tmg399x_set_als_thresh(state);

            // compute LUX
            ams_tmg399x_get_lux(state);

            sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
            numOfSensors++;
            DDF_MSG_1(MED, "TMG399 ALS Interrupt, data read: %d", AMS_tmg399x_STA_ALS_INTR);
        }

        DDF_MSG_2(MED, "TMG399 ALS int numOfSensors: %d lux: %d", numOfSensors, state->chip.data.als.lux);
    }


    if(statusReg & AMS_tmg399x_STA_PRX_INTR)
    {
        //
        // PROX interrupt
        //

        DDF_MSG_1(MED, "TMG399 in prox interrupt (prox int reg): %d", AMS_tmg399x_STA_PRX_INTR);

        ams_tmg399x_get_prox_data(state);
        sns_dd_ams_tmg399x_alsprx_prx_binary(state); /* set near or far */

        clrInterrupts |= AMS_tmg399x_CMD_PROX_INTCLR;
        hasInterrupt = true;
        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DDF_MSG_1(MED, "TMG399 out of prox data and prx_binary set state: %d", 1115);

    }

    if(numOfSensors > 0)
    {
        // Had at least one interrupt
        if ((sns_ddf_malloc((void **)&sensor_data, numOfSensors*sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1116);
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
                DDF_MSG_1(MED, "TMG399 var1: %d", 1117);
                if((sns_ddf_malloc((void **)&buf_ptr, NUM_PROX_SAMPLES_RET * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "TMG399 cannot notify smgr prx: %d", 1118);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                ams_tmg399x_prox_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            case SNS_DDF_SENSOR_AMBIENT:
                DDF_MSG_1(MED, "TMG399 var1: %d", 1119);
                if((sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(HIGH, "TMG399 cannot notify smgr als: %d", 1120);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                ams_tmg399x_als_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            default:
                DDF_MSG_1(HIGH, "Unexpected sensor type: %d", sensors[i]);
                break;
            }
        }

        eresult = sns_ddf_smgr_notify_data(state->smgr_handle, sensor_data, numOfSensors);
        DDF_MSG_2(HIGH, "result sns_ddf_smgr_notify_data %d,numOfSensors %d", eresult,numOfSensors);
        if(eresult != SNS_DDF_SUCCESS)
        {
           //Do nothing is is to eliminate compiler warning
        }
        sns_ddf_mfree(buf_ptr);
        sns_ddf_mfree(sensor_data);
    }

    if(hasInterrupt)
    {
        DDF_MSG_1(MED, "TMG399 clearing interrrupt write (05-prox 06 als 07 both)   written value: %d", clrInterrupts);
        ams_tmg399x_write_spec_func(state->port_handle, clrInterrupts);

		/* - 0x05  - Proximity interrupt clear * - 0x06  - ALS interrupt clear * - 0x07  - Proximity and ALS interrupt clear*/


#ifdef DDK
        ams_tmg399x_read_byte(state->port_handle,
                              AMS_tmg399x_rSTATUS,
                              &statusReg);
        DDF_MSG_2(MED, "TMG399 status reg var1: %d var2: %d", 1122, statusReg);
#endif
    }

    DDF_MSG_1(MED, "TMG399 out of read status reg: %d", 1123);
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_handle_irq

===========================================================================*/
/*!
  @brief AMS TMG399x interrupt handler

  @detail
  This function will be called within the context of the Interrupt
  Service Task (IST), *not* the ISR.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
static void sns_dd_ams_tmg399x_alsprx_handle_irq(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    DDF_MSG_1(HIGH, "TMG399 in handle irq: %d", 1124);
    DDF_MSG_1(HIGH, "Time stamp : %d", timestamp);
    ams_tmg399x_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_handle_timer

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
static void sns_dd_ams_tmg399x_alsprx_handle_timer(
  sns_ddf_handle_t handle,
  void*            arg
)
{
    sns_ddf_time_t    timestamp;
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    DDF_MSG_1(LOW, "TMG399 var1: %d", 5);

    timestamp = sns_ddf_get_timestamp();

    ams_tmg399x_read_status_reg(state, timestamp, TIMER_TYPE);
}


#if 0
uint32_t ams_tmg399x_wait_time(sns_dd_alsprx_state_t* state)
{
    uint32_t waitTime;

    waitTime   = state->chip.setup.Min_Integ_Time_us * state->chip.setup.wait.wtime;

    return (waitTime + ((state->chip.setup.wait.lTime * 11) * waitTime));
}


uint32_t ams_tmg399x_compute_total_time_us(sns_dd_alsprx_state_t* state)
{
    return (ams_tmg399x_als_time_us(state) + ams_tmg399x_prox_time_us(state) + ams_tmg399x_wait_time(state));
}


uint32_t ams_tmg399x_odr_from_uSec(uint32_t requestedTime_us)
{
    uint16_t i;
    uint16_t foundOdr;
    uint32_t midDn;

    foundOdr = MAX_ODR - 1;

    /* if(requestedTime_us > 750000) */
    if(requestedTime_us > ODR_USEC_MAX)
    {
        return 1;
    }

    /* if(requestedTime_us < 46536) */
    if(requestedTime_us < ODR_USEC_MIN)
    {
        return MAX_ODR;
    }

    for(i = 1; i < MAX_ODR-1; i++)
    {
        /* midDn = ((ams_tmg399x_odr_to_uSec[i] - ams_tmg399x_odr_to_uSec[i-1]) / 2) - 1; */
        midDn = ((ams_tmg399x_odr_to_uSec[i-1] - ams_tmg399x_odr_to_uSec[i]) / 2) - 1;

        if(requestedTime_us > (ams_tmg399x_odr_to_uSec[i] - midDn))
        {
            foundOdr = i + 1;
            break;
        }
    }
    DDF_MSG_1(MED, "TMG399 Calculated ODR: %d", foundOdr);
    return foundOdr;
}
#endif

//static uint8_t ams_tmg399x_gain_value[] = { 1, 4, 16, 64 };
//
//uint8_t ams_tmg399x_getAgain(sns_dd_alsprx_state_t* state)
//{
//    uint8_t gain;
//    uint8_t retVal;
//
//    gain = state->chip.setup.reg.control & TMG399X_AGAIN_MASK;
//
//    retVal = ams_tmg399x_gain_value[gain];
//
//    return retVal;
//}


//
//uint32_t ams_tmg399x_Get_Max_ALS_Count(sns_dd_alsprx_state_t* state)
//{
//    uint32_t retVal;
//
//    if (state->chip.setup.als.atime_us > 63) retVal = 65535; //65k * 3 / 4
//    retVal = (state->chip.setup.als.atime_us * 1024) - 1;
//
//    return retVal;
//}


/* bool ams_tmg399x_Saturated(sns_dd_alsprx_state_t* state, uint16_t data) */
/* { */
/*     uint32_t max_count; */

/*     max_count = ams_tmg399x_Get_Max_ALS_Count(state); */
/*     if (data >= max_count) */
/*     { */
/*         DDF_MSG_0(HIGH, "ALSPRX als saturation error"); */
/*         return true; */
/*     } */
/*     return false; */
/* } */


/* sns_ddf_status_e ams_tmg399x_API_APP_Get_mLUX(sns_dd_alsprx_state_t* state) */
/* { */
/*     int32_t  CPkL; */
/*     int32_t  Lux1; */
/*     int32_t  Lux2; */
/*     int32_t  DGF; */
/*     int32_t  CoefB; */
/*     int32_t  CoefC; */
/*     int32_t  CoefD; */
/*     uint16_t C0Data; */
/*     uint16_t C1Data; */
/*     uint8_t  ALS_xGain; */
/*     uint32_t ALS_Time_us; */
/*     DDF_MSG_0(HIGH, "api app get mlux"); */
/*     DGF   = state->chip.als_setup.DGF; */
/*     CoefB = state->chip.als_setup.iCoefB; */
/*     CoefC = state->chip.als_setup.iCoefC; */
/*     CoefD = state->chip.als_setup.iCoefD; */

/*     C0Data = state->chip.data.als.ch0; */
/*     C1Data = state->chip.data.als.ch1; */
/*     DDF_MSG_0(HIGH, "c0data"); */
/*     ALS_xGain   = ams_tmg399x_getAgain(state); */
/*     ALS_Time_us = state->chip.setup.als.atime_us; */
/*     DDF_MSG_0(HIGH, "als gain"); */
/*     if (C0Data == 0) return SNS_DDF_EINVALID_DATA;                    // Check bad read */
/*     DDF_MSG_0(HIGH, "good read"); */
/*     if (ams_tmg399x_Saturated(state, C1Data)) return SNS_DDF_EDATA_BOUND; // Check for saturated data */
/*     DDF_MSG_0(HIGH, "data not saturated"); */
/*     CPkL = (ALS_xGain * (int)ALS_Time_us / DGF); */
/*     //max CPkL = 120 * 700,000 / 24 = 3,500,000 / DGF */
/*     //max CPkL = 120 * 200,000 / 24 = 1,000,000 / DGF */
/*     if (CPkL == 0) CPkL = 1;                        // avoid divide by zero */
/*     Lux1 = 0; */
/*     Lux2 = 0; */

/*     Lux1 = 20 * (1000  * C0Data - CoefB * C1Data); */
/*     Lux2 = 20 * (CoefC * C0Data - CoefD * C1Data); */
/*     state->chip.data.als.Lux_Extended = false; */
/*     DDF_MSG_0(HIGH, "setting lux1 and lux 2"); */

/*     /\* if (Chip.mAPP_ALS_Setup.Lux_Extend_Enable) *\/ */
/*     /\* { *\/ */
/*     /\*     if (ams_tmg399x_CloseSaturation75(state, C0Data)) //If saturated and gain = 1x *\/ */
/*     /\*     { *\/ */
/*     /\*         Lux1 = 20 * (state->chip.data.als.Last_C1C0_Ratio1000 - CoefB) * C1Data; *\/ */
/*     /\*         Lux2 = 0; *\/ */
/*     /\*         state->chip.data.als.Lux_Extended = true; *\/ */
/*     /\*     } *\/ */
/*     /\*     else *\/ */
/*     /\*     { *\/ */
/*     /\*         if (C1Data > 0) *\/ */
/*     /\*         { *\/ */
/*     /\*             state->chip.data.als.Last_C1C0_Ratio1000 = C0Data * 1000 / C1Data; *\/ */
/*     /\*         } *\/ */
/*     /\*     } *\/ */
/*     /\* } *\/ */
/*     /\* else *\/ */
/*     { */
/*         if (ams_tmg399x_Saturated(state, C0Data)) //If saturated and gain = 1x */
/*         { */
/*             DDF_MSG_0(HIGH, "ALSPRX als saturation error"); */
/*             return SNS_DDF_EDATA_BOUND; */
/*         } */
/*     } */

/*     if (Lux2 > Lux1) Lux1 = Lux2; */
/*     if (Lux1 < 0) */
/*     { */
/*         DDF_MSG_0(HIGH, "lux1 below 0"); */
/*         return SNS_DDF_EDATA_BOUND; */
/*     } */
/*     if (Lux1 > 10000000) //10,000,000 Check to keep from overflowing 2,147,483,648 maximum 32 bit signed integer */
/*     { */
/*         Lux1 = 50 * (Lux1 / CPkL); */
/*         DDF_MSG_0(HIGH, "setting lux1 a"); */
/*     } */
/*     else */
/*     { */
/*         Lux1 = 50 * Lux1 / CPkL; */
/*         DDF_MSG_0(HIGH, "setting lux1 b"); */
/*     } */

/*     state->chip.data.als.lux = Lux1; */

/*     state->chip.data.als.Lux_OK = true; */
/*     DDF_MSG_3(MED, "als.lux: %d lux1: %d lux2: %d", state->chip.data.als.lux , Lux1, Lux2); */
/*     return SNS_DDF_SUCCESS; */
/* } */


sns_ddf_status_e ams_tmg399x_read_all_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    status = ams_tmg399x_read_buf(state->port_handle,
                                  TMG399X_STATUS,
                                  TMG399X_PDATA - TMG399X_STATUS + 1,
                                  &state->chip.shadow[TMG399X_STATUS]);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX als data err1");
    }

    return status;
}


sns_ddf_status_e ams_tmg399x_get_prox_data(sns_dd_alsprx_state_t* state)
{
    /* uint8_t* tBuf; */

    /* tBuf = &state->chip.shadow[TMG399X_PDATA]; */
    /* state->chip.data.prox.data = getWord(tBuf, 0); */

    //
    // Get Prox data
    //
    state->chip.data.prox.data = state->chip.shadow[TMG399X_PDATA];

    DDF_MSG_2(MED, "TMG399 out of get prox data: %d var2: %d", 1158, state->chip.data.prox.data);
    DDF_MSG_1(HIGH, "GPIO Number: %d", myGPIO);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e ams_tmg399x_get_als_data(sns_dd_alsprx_state_t* state)
{
    uint8_t* tBuf;

    tBuf = &state->chip.shadow[TMG399X_CDATAL];
    DDF_MSG_0(HIGH, "ams_tmg399x_get_als_data:");
    //
    // Get Color data
    //
    state->chip.als_inf.clear_raw = getWord(tBuf, 0);
    state->chip.als_inf.red_raw   = getWord(tBuf, 2);
    state->chip.als_inf.green_raw = getWord(tBuf, 4);
    state->chip.als_inf.blue_raw  = getWord(tBuf, 6);

    // ir = (R + G + B - C + 1) / 2
    state->chip.als_inf.ir =
        (state->chip.als_inf.red_raw  + state->chip.als_inf.green_raw +
         state->chip.als_inf.blue_raw - state->chip.als_inf.clear_raw + 1) >> 1;

    // ir can not be negative
    if (state->chip.als_inf.ir < 0)
    {
        state->chip.als_inf.ir = 0;
    }
    DDF_MSG_1(HIGH, "state->chip.als_inf.ir %d clr: %d", state->chip.als_inf.ir);
    DDF_MSG_2(MED, "TMG399x var1: %d clr: %d", 1159, state->chip.als_inf.clear_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d red: %d", 1159, state->chip.als_inf.red_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d grn: %d", 1159, state->chip.als_inf.green_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d blu: %d", 1159, state->chip.als_inf.blue_raw);

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_tmg399x_prox_sensor_samples(
  sns_dd_alsprx_state_t   *state,
  sns_ddf_sensor_data_s   *data_ptr,
  sns_ddf_memhandler_s    *memhandler,
  sns_ddf_sensor_sample_s *sample_data
)
{
    uint32_t num_samples = NUM_PROX_SAMPLES_RET;

    DDF_MSG_1(MED, "TMG399 var1: %d", 1161);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1162);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }
    DDF_MSG_1(MED, "TMG399 Faraway event num: %d", SNS_PRX_FAR_AWAY);
    DDF_MSG_1(MED, "TMG399 nearby event num: %d", SNS_PRX_NEAR_BY);
    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.prox.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = FX_FLTTOFIX_Q16(state->chip.data.prox.data);
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "PRX Data...");
    DDF_MSG_3(FATAL, "TMD PRX Data: Event:%d  Reported Data: %d  Raw Data: %d", state->chip.data.prox.event, data_ptr->samples[0].sample, state->chip.data.prox.data);
    sns_dd_ams_tmg399x_alsprx_log(state, SNS_DDF_SENSOR_PROXIMITY,
                        state->chip.data.prox.event, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.prox.data);

    DDF_MSG_1(HIGH, "GPIO Number: %d", myGPIO);
    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_tmg399x_als_sensor_samples(
  sns_dd_alsprx_state_t    *state,
  sns_ddf_sensor_data_s    *data_ptr,
  sns_ddf_memhandler_s     *memhandler,
  sns_ddf_sensor_sample_s  *sample_data
)
{
    // Return Lux, Red, Green, Blue and Clear data
    //uint32_t num_samples = NUM_ALS_SAMPLES_RET;
    uint32_t num_samples = 3;
    uint32_t luxCorrection;
    uint32_t lux;

    DDF_MSG_1(MED, "TMG399 var1: %d", 1164);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(
                                                      memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1165);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1166);
        return SNS_DDF_EINVALID_PARAM;
    }

    lux           = state->chip.data.als.lux;
    luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;
// temporarily commenting out some code
   // DDF_MSG_2(FATAL, "Lux Correction: %d  alslux now not div by 1000: %d",state->chip.setup.als.luxCorrection, alsluxby1000);
  //  DDF_MSG_2(FATAL, "1st term before +512: %d  all before div by 1024: %d", (alsluxby1000 * (state->chip.setup.als.luxCorrection)),((alsluxby1000 * (state->chip.setup.als.luxCorrection)) + 512.0));

    /* data_ptr->samples[0].sample = FX_FLTTOFIX_Q16((((lux / 1000.0) * luxCorrection) + 512.0) / 1024.0); */

    //data_ptr->samples[0].sample = FX_FLTTOFIX_Q16(((lux * luxCorrection) >> 16) / 1000.0);
    DDF_MSG_1(HIGH, "luxCorrection: %d", luxCorrection);
    data_ptr->samples[0].sample = FX_FLTTOFIX_Q16((lux * luxCorrection) / 1024.0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;

    data_ptr->samples[1].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.red_raw);
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;

    data_ptr->samples[2].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.green_raw);
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[3].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.blue_raw);
    //data_ptr->samples[3].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[4].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.clear_raw);
    //data_ptr->samples[4].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[5].sample = FX_FLTTOFIX_Q16(state->chip.shadow[TMG399X_ATIME]);
    //data_ptr->samples[5].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[6].sample = FX_FLTTOFIX_Q16(state->chip.params.als_gain);
    //data_ptr->samples[6].status = SNS_DDF_SUCCESS;

    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "ALS Data...");
    DDF_MSG_2(FATAL, "TMD LIGHT Data: MLux:%d  Reported Data: %d", state->chip.data.als.lux, data_ptr->samples[0].sample);


    sns_dd_ams_tmg399x_alsprx_log(state, SNS_DDF_SENSOR_AMBIENT,
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.ch0);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_get_data

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
static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_data(
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

    DDF_MSG_0(HIGH, "ALSPRX Get Data ");
    DDF_MSG_0(HIGH, "ALSPRX Get Data prx");

    ams_tmg399x_read_all_data(state);
    status = ams_tmg399x_get_prox_data(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err1: %d", status);
        return status;
    }

    status = sns_dd_ams_tmg399x_alsprx_prx_binary(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err2: %d", status);
        return status;
    }

    status = ams_tmg399x_get_als_data(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err1: %d", status);
        return status;
    }

    /* status = ams_tmg399x_API_APP_Get_mLUX(state); */
    status = ams_tmg399x_get_lux(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err2: %d", status);
        return status;
    }

    timestamp = sns_ddf_get_timestamp();
    data_ptr  = sns_ddf_memhandler_malloc(
                                         memhandler,
                                         num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(data_ptr == NULL)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data malloc error, Size: %d", (num_sensors * sizeof(sns_ddf_sensor_data_s)));
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    DDF_MSG_0(HIGH, "ALSPRX Get Data, processing data for smgr");

    for(i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor    = sensors[i];
        data_ptr[i].status    = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning prx data to smgr");
            ams_tmg399x_prox_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning als data to smgr");
            ams_tmg399x_als_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        default:
            DDF_MSG_1(HIGH, "Unexpected sensor type: %d", sensors[i]);
            break;
        }
    }

    return SNS_DDF_SUCCESS;
}

 
static void ams_tmg399x_calc_cpl(ams_tmg399x_chip *chip)
{
    uint32_t cpl;
    uint32_t sat;
    uint8_t  atime = chip->shadow[TMG399X_ATIME];

    cpl  = 256 - chip->shadow[TMG399X_ATIME];
    /* cpl *= INTEGRATION_CYCLE; */
    cpl *= chip->setup.Min_Integ_Time_us;
    cpl /= 1000;  // 100;
    cpl *= ams_tmg399x_als_gains[chip->params.als_gain];

    /* sat = min_t(uint32_t, MAX_ALS_VALUE, (uint32_t)(256 - atime) << 10); */
    sat = min(MAX_ALS_VALUE, (uint32_t)(256 - atime) << 10);
    sat = sat * 8 / 10;
    /* dev_info(&chip->client->dev, */
    /* 		"%s: cpl = %u [gain %u, sat. %u]", */
    /* 		__func__, cpl, */
    /* 		ams_tmg399x_als_gains[chip->params.als_gain], sat); */
    chip->als_inf.cpl = cpl;
    chip->als_inf.saturation = sat;
}


/* static int ams_tmg399x_set_als_gain(ams_tmg399x_chip *chip, int gain) */
static sns_ddf_status_e ams_tmg399x_set_als_gain(sns_dd_alsprx_state_t* state, int gain)
{
    /* int rc; */
    ams_tmg399x_chip *chip = &state->chip;
    uint8_t ctrl_reg   = chip->shadow[TMG399X_CONTROL] & ~TMG399X_AGAIN_MASK;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    DDF_MSG_0(HIGH, "ams_tmg399x_set_als_gain");
    switch (gain) {
    case 1:
        ctrl_reg |= TMG399X_AGAIN_1;
        break;
    case 4:
        ctrl_reg |= TMG399X_AGAIN_4;
        break;
    case 16:
        ctrl_reg |= TMG399X_AGAIN_16;
        break;
    case 60:
    case 64:
        ctrl_reg |= TMG399X_AGAIN_64;
        break;
    default:
        /* dev_err(&chip->client->dev, "%s: wrong als gain %d\n", */
        /* 		__func__, gain); */
        return SNS_DDF_EINVALID_PARAM;
    }

    status = ams_tmg399x_write_byte(state->port_handle, TMG399X_CONTROL, &ctrl_reg);
    if ( status == SNS_DDF_SUCCESS )
    {
        chip->shadow[TMG399X_CONTROL] = ctrl_reg;
        chip->params.als_gain = (ctrl_reg & TMG399X_AGAIN_MASK);
    }
    return status;

    /* rc = ams_tmg399x_i2c_write(chip, TMG399X_CONTROL, ctrl_reg); */
    /* if (!rc) { */
    /* 	chip->shadow[TMG399X_CONTROL] = ctrl_reg; */
    /* 	chip->params.als_gain = ctrl_reg; */
    /* 	/\* dev_info(&chip->client->dev, "%s: new gain %d\n", *\/ */
    /* 	/\* 		__func__, gain); *\/ */
    /* } */

    /* return rc; */
}


sns_ddf_status_e ams_tmg399x_get_lux(sns_dd_alsprx_state_t* state)
{
    uint32_t rp1, gp1, bp1, cp1;
    uint32_t lux = 0;
    uint32_t cct;
    /* int ret; */

    ams_tmg399x_chip *chip;
    chip = &state->chip;

    uint32_t sat = chip->als_inf.saturation;
    uint32_t sf;
    DDF_MSG_0(HIGH, "ams_tmg399x_get_lux ");
    /* use time in ms get scaling factor */
    ams_tmg399x_calc_cpl(chip);
    DDF_MSG_1(HIGH, "chip->als_inf.saturation=%d",chip->als_inf.saturation);
    if (!chip->als_gain_auto)
    {
        DDF_MSG_0(HIGH, "!chip->als_gain_auto");
        if (chip->als_inf.clear_raw <= MIN_ALS_VALUE)
        {   DDF_MSG_2(HIGH, "chip->als_inf.clear_raw=%d MIN_ALS_VALUE=%d",chip->als_inf.clear_raw,MIN_ALS_VALUE);
            /* dev_info(&chip->client->dev, */
            /*          "%s: darkness\n", __func__); */
            lux = 0;
            goto exit;
        }
        else if (chip->als_inf.clear_raw >= sat)
        {
          DDF_MSG_2(HIGH, "chip->als_inf.clear_raw=%d sat=%d",chip->als_inf.clear_raw,sat);
          DDF_MSG_1(HIGH, "chip->als_inf.lux=%d",chip->als_inf.lux);
          /* dev_info(&chip->client->dev, */
          /*          "%s: saturation, keep lux & cct\n", __func__); */
          lux = chip->als_inf.lux;
          goto exit;
        }
    }
    else
    {
        DDF_MSG_0(HIGH, "chip->als_gain_auto");
        uint8_t gain = ams_tmg399x_als_gains[chip->params.als_gain];
        /* int rc = -EIO; */
        sns_ddf_status_e rc = SNS_DDF_SUCCESS;

        if (gain == 16 && chip->als_inf.clear_raw >= sat)
        {
            DDF_MSG_0(HIGH, "gain == 16 && chip->als_inf.clear_raw >= sat");
            rc = ams_tmg399x_set_als_gain(state, 1);
            /* rc = ams_tmg399x_set_als_gain(chip, 1); */
        }
        else if (gain == 16 &&
                 chip->als_inf.clear_raw < GAIN_SWITCH_LEVEL)
        {
            rc = ams_tmg399x_set_als_gain(state, 60);
            /* rc = ams_tmg399x_set_als_gain(chip, 60); */
        }
        else if ((gain == 60 &&
                  chip->als_inf.clear_raw >= (sat - GAIN_SWITCH_LEVEL)) ||
                 (gain == 1 &&
                  chip->als_inf.clear_raw > GAIN_SWITCH_LEVEL))
        {
            rc = ams_tmg399x_set_als_gain(state, 16);
            /* rc = ams_tmg399x_set_als_gain(chip, 16); */
        }

        if (rc != SNS_DDF_SUCCESS)
        {
            /* dev_info(&chip->client->dev, "%s: gain adjusted, skip\n", */
            /*          __func__); */
            ams_tmg399x_calc_cpl(chip);
            /* ret = -EAGAIN; */
            lux = chip->als_inf.lux;
            goto exit;
        }

        if (chip->als_inf.clear_raw <= MIN_ALS_VALUE)
        {
            /* dev_info(&chip->client->dev, */
            /*          "%s: darkness\n", __func__); */
            lux = 0;
            goto exit;
        }
        else if (chip->als_inf.clear_raw >= sat)
        {
            /* dev_info(&chip->client->dev, "%s: saturation, keep lux\n", */
            /*          __func__); */
            lux = chip->als_inf.lux;
            goto exit;
        }
    }

    /* remove ir from counts*/
    rp1 = chip->als_inf.red_raw   - chip->als_inf.ir;
    gp1 = chip->als_inf.green_raw - chip->als_inf.ir;
    bp1 = chip->als_inf.blue_raw  - chip->als_inf.ir;
    cp1 = chip->als_inf.clear_raw - chip->als_inf.ir;
    DDF_MSG_1(HIGH, "rp1=%d",rp1);
    DDF_MSG_1(HIGH, "gp1=%d",gp1);
    DDF_MSG_1(HIGH, "bp1=%d",bp1);
    DDF_MSG_1(HIGH, "cp1=%d",cp1);
    DDF_MSG_1(HIGH, "chip->als_inf.ir=%d",chip->als_inf.ir);
    if(cp1 == 0)
    {
      //Do nothing. This is to eliminate compiler warning
    }

    if (!chip->als_inf.cpl)
    {
        /* dev_info(&chip->client->dev, "%s: zero cpl. Setting to 1\n", */
        /*          __func__); */
        chip->als_inf.cpl = 1;
    }

    if (chip->als_inf.red_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.red_raw > chip->als_inf.ir");
        lux += chip->segment[chip->device_index].r_coef * rp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux rp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].r_coef * rp1)); */
    }

    if (chip->als_inf.green_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.green_raw > chip->als_inf.ir");
        lux += chip->segment[chip->device_index].g_coef * gp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux gp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].g_coef * rp1)); */
    }

    if (chip->als_inf.blue_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.blue_raw > chip->als_inf.ir");
        lux -= chip->segment[chip->device_index].b_coef * bp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux bp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].b_coef * rp1)); */
    }

    sf = chip->als_inf.cpl;

    if (sf > 131072)  // 0x2_0000
    {
        goto error;
    }

    lux /= sf;
    lux *= chip->segment[chip->device_index].d_factor;
    lux += 512;
    lux >>= 10;

    // Save as a 32 bit int
    //chip->als_inf.lux = (uint16_t) lux;
    cct = ((chip->segment[chip->device_index].ct_coef * bp1) / rp1) +
        chip->segment[chip->device_index].ct_offset;

    chip->als_inf.cct = (uint16_t) cct;

 exit:

    chip->als_inf.lux  = lux;
    chip->data.als.lux = lux;

    return SNS_DDF_SUCCESS;
    /* return 0; */

 error:
    /* dev_err(&chip->client->dev, "ERROR Scale factor = %d", sf); */

    return SNS_DDF_EINVALID_DATA;
    /* return 1; */
}
