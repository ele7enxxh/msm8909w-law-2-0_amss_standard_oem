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
//uint32 myGPIO;
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


//
//++ TMG399x ++
//
static uint16_t ams_tmg399x_min_integ_time_us[] = {
    2780,    // TMG3992
    2780,    // TMG39923
};


static struct lux_segment ams_tmg399x_lux_coef[] = {
    {486,  93, 1000, 522, 4490, 1695},    // TMG3992
    {486,  93, 1000, 522, 4490, 1695},    // TMG3993
};


/* static uint8_t const ams_tmg399x_ids[] = { */
/*     0x9C,    // TMG39921/5 */
/*     0x9F     // TMG39923/7 */
/* }; */

//
//-- TMG399x --
//

extern sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list;

extern sns_ddf_status_e ams_tmg399x_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val);

extern sns_ddf_status_e ams_tmg399x_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val);

extern sns_ddf_status_e ams_tmg399x_write_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* buf, uint8_t size);

extern sns_ddf_status_e ams_tmg399x_read_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t byteCnt, uint8_t* buf);

extern sns_ddf_status_e ams_tmg399x_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val);

extern sns_ddf_status_e ams_tmg399x_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val);

extern sns_ddf_status_e ams_tmg399x_write_spec_func(sns_ddf_handle_t port_handle, uint8_t spec_func);

extern uint8_t ams_tmg399x_usecToIcycles(uint32_t time_us, uint32_t minIntegTime_us);

extern sns_ddf_status_e ams_tmg399x_read_all_data(sns_dd_alsprx_state_t* state);

extern sns_ddf_status_e ams_tmg399x_get_prox_data(sns_dd_alsprx_state_t* state);

extern sns_ddf_status_e ams_tmg399x_get_als_data(sns_dd_alsprx_state_t* state);

extern sns_ddf_status_e ams_tmg399x_get_lux(sns_dd_alsprx_state_t* state);



/*============================================================================
                           FUNCTION PROTOTYPES
============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_init
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

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);

sns_ddf_status_e ams_tmg399x_set_default_reg_values(sns_dd_alsprx_state_t* state);

void ams_tmg399x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state);


#define MS_TO_US(t) (t * 1000)
#define US_TO_MS(t) (t / 1000)

//
// ------------------  AMS-TAOS USA Code   ------------------
/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_ams_tmg399x_alsprx_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
/*  sns_dd_als_db_type           *als_db_ptr    = &dd_ptr->sns_dd_als_db; */
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

FUNCTION      sns_dd_ams_tmg399x_alsprx_prx_init

DESCRIPTION   initializes the AMS ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_ams_tmg399x_alsprx_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_PRX_INIT);

  prx_db_ptr->pend_state  = SNS_DD_PEND_STATE_IDLE;
  /* prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN; */
}

/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_common_init

DESCRIPTION   initializes the AMS ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_ams_tmg399x_alsprx_common_init ( sns_dd_alsprx_state_t* dd_ptr )
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
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near   = SNS_DD_PRX_THRESH_NEAR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far    = SNS_DD_PRX_THRESH_FAR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor    = SNS_DD_PRX_FACTOR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor    = SNS_DD_ALS_FACTOR; */

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
#endif
  }

#ifdef ALSPRX_DEBUG_REG
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_source); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor); */
#endif /* ALSPRX_DEBUG_REG */

}

/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_ams_tmg399x_alsprx_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_ams_tmg399x_alsprx_common_init(dd_ptr);

  /* Initialize ALS database */
  sns_dd_ams_tmg399x_alsprx_als_init(dd_ptr);

  /* Initialize PRX database */
  sns_dd_ams_tmg399x_alsprx_prx_init(dd_ptr);
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_set_powerstate

DESCRIPTION   Sets the power state of the AMS ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uint8_t          i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

#ifdef DDK
  uint32_t         odr_uSec;
  static uint16_t  numTimers = 0;
#endif

  DDF_MSG_1(MED, "TMG399 var1: %d", 1126);

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          DDF_MSG_1(MED, "ONLY PROX", 1126);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_PEN;
#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PIEN;
          state->chip.setup.odr    = 20;
          /* state->chip.setup.prox.odr    = 20; */
#endif
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          DDF_MSG_1(MED, "ONLY ALS", 1126);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_AEN;
#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AIEN;
          state->chip.setup.odr     =  7;
          /* state->chip.setup.als.odr     =  7; */
#endif
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)
      {
          DDF_MSG_1(MED, "CHECK IF RE-ENABLE", 1126);
          //SNS_PRINTF_MEDIUM_1(8890);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_PEN;
          //SNS_PRINTF_MEDIUM_1(8891);
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AEN;
          DDF_MSG_1(MED, "TMG399 var1: %d", 5555);

#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PIEN;
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AIEN;
          state->chip.setup.odr     =  7;
          /* state->chip.setup.als.odr     =  20; */
#endif
      }

      state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PON;
      i2c_data = state->chip.setup.reg.enable;
      status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
      if ( status != SNS_DDF_SUCCESS ) {
          DDF_MSG_1(MED, "TMG399 var1: %d", 1127);
          return status;
      }
      DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1128, i2c_data);
#ifdef DDK
      // Set up a timer to read the status register to see if an interrupt has occured.
      /* if(state->chip.setup.als.odr > state->chip.setup.prox.odr) */
      /* { */
      /*     odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.prox.odr - 1]; */
      /* } */
      /* else */
      /* { */
      /*     odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.als.odr - 1]; */
      /* } */

      odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.odr - 1];

      if (numTimers == 0)
      {
          ++numTimers;
          status = sns_ddf_timer_start(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj,
                                       odr_uSec
                                       /* MS_TO_US(50) */
                                       );
          if ( status != SNS_DDF_SUCCESS ) {
              DDF_MSG_1(MED, "TMG399 var1: %d", 1129);
              return status;
          }
      }
#endif
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_ams_tmg399x_alsprx_init_data_struct(state);
    }
#ifdef ALSPRX_DEBUG_BASIC
#error code not present
#endif

    i2c_data = AMS_tmg399x_CNTL_PWROFF;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   AMS_tmg399x_rCONFIG,
                                   status);
      return status;
    }
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1130, i2c_data);

    state->chip.setup.odr = 0;

    /* state->chip.setup.prox.odr = 0; */
    /* state->chip.setup.als.odr  = 0; */

#ifdef DDK
    status = sns_ddf_timer_cancel(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj);
    if ( status != SNS_DDF_SUCCESS ) {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1131);
        return status;
    }
#endif

  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;

  return status;
}


sns_ddf_status_e ams_tmg399x_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;
    /* uword_u wordVal; */
    uint8_t *sh = state->chip.shadow;


#ifdef DDK
    uint8_t byteBuf[16];
#endif

    ams_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    setup  = &state->chip.setup;

#ifdef DDK
    status = ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rID, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
#endif

    // Reset the device

    // Power off
    byteVal = 0x00;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    sh[TMG399X_ATIME]   = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
    sh[TMG399X_WTIME]   = 256 - state->chip.setup.wait.wtime;;

    sh[TMG399X_AILTHL]  = setup->als.thresh_low & 0xFF;
    sh[TMG399X_AILTHH]  = setup->als.thresh_low >> 8;
    sh[TMG399X_AIHTHL]  = setup->als.thresh_high & 0xFF;
    sh[TMG399X_AIHTHH]  = setup->als.thresh_high >> 8;

    sh[TMG399X_PITHL]   = setup->prox.thresh_low;
    sh[TMG399X_PITHH]   = setup->prox.thresh_high;

    sh[TMG399X_PERS]    = setup->persistence;
    sh[TMG399X_CONFIG1] = setup->reg.config;
    sh[TMG399X_PPULSE]  = setup->prox.pulse_count;

    sh[TMG399X_CONTROL] = setup->reg.control;
    sh[TMG399X_CONFIG2] = setup->reg.config2;

    status = ams_tmg399x_write_buf(state->port_handle, TMG399X_ATIME, &sh[TMG399X_ATIME], TMG399X_CONFIG2 - TMG399X_ATIME + 1);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }


    /* sh[TMG399X_ENABLE]  = state->chip.setup.reg.enable; */
    /* status = ams_tmg399x_write_byte(state->port_handle, TMG399X_ENABLE, &sh[TMG399X_ENABLE]); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */


    /* // Power off */
    /* byteVal = 0x00; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set the integration cycle time */
    /* byteVal = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us); */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rATIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set PTIME */
    /* byteVal = ams_tmg399x_usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us); */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPTIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set WTIME */
    /* byteVal = 256 - state->chip.setup.wait.wtime; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rWTIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set ALS Interrupt Threshold low */
    /* wordVal.word = setup->als.thresh_low; */
    /* status = ams_tmg399x_write_word(state->port_handle, AMS_tmg399x_rAILTL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set ALS Interrupt Threshold high */
    /* wordVal.word = setup->als.thresh_high; */
    /* status = ams_tmg399x_write_word(state->port_handle, AMS_tmg399x_rAIHTL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox Interrupt Threshold low */
    /* wordVal.word = setup->prox.thresh_low; */
    /* status = ams_tmg399x_write_word(state->port_handle, TMG399X_PITHL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox Interrupt Threshold high */
    /* wordVal.word = setup->prox.thresh_high; */
    /* status = ams_tmg399x_write_word(state->port_handle, TMG399X_PITHH, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Persistence (Prox and ALS) */
    /* byteVal = setup->persistence; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPERS, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Config Reg */
    /* byteVal = setup->reg.config; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONFIG, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox pulses */
    /* byteVal = setup->prox.pulse_count; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPPULSE, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // LED drive to 100 mA, use IR (CH1) diode for prox, set AGain to 1x */
    /* byteVal = setup->reg.control; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONTROL, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

#ifdef DDK
    ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rCONTROL, &byteVal);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1132, byteVal);
#endif

    // Clear any pending interrupts
    byteVal = 0x07;
    status = ams_tmg399x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Enable Power on (PON).
    byteVal = state->chip.setup.reg.enable;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);

#ifdef DDK
    ams_tmg399x_read_buf(state->port_handle, AMS_tmg399x_rENABLE, 20, byteBuf);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1133, byteBuf[0]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1134, byteBuf[1]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1135, byteBuf[2]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1136, byteBuf[3]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1137, byteBuf[4]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1138, byteBuf[5]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1139, byteBuf[6]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1140, byteBuf[7]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1141, byteBuf[8]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1142, byteBuf[9]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1143, byteBuf[10]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1144, byteBuf[11]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1145, byteBuf[12]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1146, byteBuf[13]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1147, byteBuf[14]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1148, byteBuf[15]);
#endif

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_reset

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
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

    DDF_MSG_0(HIGH, "sns_dd_ams_tmg399x_alsprx_reset");
    DDF_MSG_1(MED, "TMG399 var1: %d", 1149);

    sns_ddf_delay(1000);

    // Set the sensor in use
    DDF_MSG_1(MED, "TMG399 1371: %d", status);

    // Reset driver state
    status = ams_tmg399x_set_default_reg_values(state);
    if(status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Now set the usable values
    ams_tmg399x_set_reg_values_from_nv(state);

    // Set register values
    status = ams_tmg399x_update_registers(state);
    DDF_MSG_1(MED, "TMG399 reset exit: %d", status);
    return status;
}


static uint32_t ams_tmg399x_als_time_us(sns_dd_alsprx_state_t* state)
{
    // ALS enabled
	DDF_MSG_1(MED, "TMG399 als time us: %d", state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us);
    return state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us;
}


static uint32_t ams_tmg399x_prox_time_us(sns_dd_alsprx_state_t* state)
{
    // Prox enabled
	DDF_MSG_1(MED, "TMG399 prx time us: %d", 2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);

    return ( 2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);
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
#endif

sns_ddf_status_e ams_tmg399x_write_wtime(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    // set WEN in enable register
    if(state->chip.setup.wait.wen == 1)
    {
        state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_WEN;
        DDF_MSG_1(MED, "TMG399 WENset %d", 1);
    }
    else
    {
        state->chip.setup.reg.enable &= ~AMS_tmg399x_ENABLE_MSK_WEN;
        DDF_MSG_1(MED, "TMG399 WENset to 0 %d", 0);
    }
    byteVal = state->chip.setup.reg.enable;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);
    DDF_MSG_1(MED, "TMG399 rEnable %d", 1);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1150, byteVal);

    // Set WTIME
    byteVal = 256 - state->chip.setup.wait.wtime;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rWTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    DDF_MSG_2(MED, "TMG399 rWTime %d var2: %d", 1151, byteVal);

    // Set Config Reg
    if(state->chip.setup.wait.lTimeChanged)
    {
        if(state->chip.setup.wait.lTime)
        {
            state->chip.setup.reg.config |= TMG399X_CONFIG_WLONG_MSK;
            DDF_MSG_1(MED, "config retgset %d", 1);
        }
        else
        {
            state->chip.setup.reg.config &= ~TMG399X_CONFIG_WLONG_MSK;
        }

        byteVal = state->chip.setup.reg.config;
        status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONFIG, &byteVal);
        if ( status != SNS_DDF_SUCCESS ) {
            return status;
        }
        DDF_MSG_2(MED, "TMG399 var1: %d config reg i2c %d", 1152, byteVal);
        state->chip.setup.wait.lTimeChanged = false;
    }
    return status;
}

#if 0
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

static void ams_tmg399x_compute_new_wtime(sns_dd_alsprx_state_t* state, uint32_t requestedOdr, uint32_t time_us)
{
    uint32_t requestedOdr_us;
    uint32_t wtime_us;

    uint8_t  wtime;
    uint8_t  wen;
    uint8_t  lTime;

    bool     lTimeChanged;
    DDF_MSG_1(MED, "TMG399 req ODR: %d", requestedOdr);

    if(requestedOdr > MAX_ODR)
    {
        requestedOdr = MAX_ODR;
    }
    requestedOdr_us = ams_tmg399x_odr_to_uSec[requestedOdr - 1];

    // Check to make sure sum of als and prox is less than requestedOdr_us
    // WEN check

    wtime_us = 0;

    if(time_us > requestedOdr_us)
    {
        wtime_us = 0;
        wen      = 0;
    }
    else
    {
        wtime_us = requestedOdr_us - time_us;
        wen      = 1;
    }

    if(wtime_us > 696150)
    {
        wtime_us     = 1000000;
        wtime        = 31;
        lTime        = 1;
        lTimeChanged = true;
    }
    else
    {
        wtime        = wtime_us / state->chip.setup.Min_Integ_Time_us;
        lTime        = 0;
        lTimeChanged = true;
    }

    DDF_MSG_1(MED, "TMG399 wtimeus %d", wtime_us);
    DDF_MSG_1(MED, "TMG399 wtime %d", state->chip.setup.Min_Integ_Time_us);

    state->chip.setup.wait.wtime_us     = wtime_us;
    state->chip.setup.wait.wtime        = wtime;

    DDF_MSG_1(MED, "TMG399 wtime %d", state->chip.setup.wait.wtime);

    state->chip.setup.wait.wen          = wen;
    state->chip.setup.wait.lTime        = lTime;

    DDF_MSG_1(MED, "TMG399 ltime: %d", state->chip.setup.wait.lTime);

    state->chip.setup.wait.lTimeChanged = lTimeChanged;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the TMG 399x ALS/Prx sensor

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
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_attrib
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

    DDF_MSG_1(MED, "TMG399 var1: %d", 1153);

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
      DDF_MSG_1(MED, "power mode set %d", 1154);
      status = sns_dd_ams_tmg399x_alsprx_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
      if(status != SNS_DDF_SUCCESS)
      {
          DDF_MSG_1(MED, "TMG399 var1: %d", 1154);
          return status;
      }

      break;

  case SNS_DDF_ATTRIB_ODR:
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
         state->chip.setup.odr = *(uint32_t*)value;
         /* state->chip.setup.prox.odr = *(uint32_t*)value; */

         DDF_MSG_1(MED, "orig prox requested odr  %d", state->chip.setup.odr);
         /* DDF_MSG_1(MED, "orig prox requested odr  %d", state->chip.setup.prox.odr); */

         /* if (state->chip.setup.prox.odr > 5) */
         if (state->chip.setup.odr > 5)
         {
             state->chip.setup.odr = 5;
             DDF_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.odr);

             /* state->chip.setup.prox.odr = 5; */
             /* DDF_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.prox.odr); */
         }

         DDF_MSG_2(MED, "TMG399 PRX ODR var1: %d var2: %d", 1155, state->chip.setup.odr);
         /* DDF_MSG_2(MED, "TMG399 PRX ODR var1: %d var2: %d", 1155, state->chip.setup.prox.odr); */
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          state->chip.setup.odr = *(uint32_t*)value;
          DDF_MSG_2(MED, "TMG399 ALS ODR var1: %d var2: %d", 1156, state->chip.setup.odr);

          /* state->chip.setup.als.odr = *(uint32_t*)value; */
          /* DDF_MSG_2(MED, "TMG399 ALS ODR var1: %d var2: %d", 1156, state->chip.setup.als.odr); */
      }
      else
      {
          return SNS_DDF_EINVALID_ATTR;
      }


      alsTime_us  = 0;
      proxTime_us = 0;

      if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_AEN) != 0)
      {
          alsTime_us  = ams_tmg399x_als_time_us(state);
      }

      if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_PEN) != 0)
      {
          proxTime_us = ams_tmg399x_prox_time_us(state);
      }

      if((alsTime_us > 0) && (proxTime_us > 0))
      {
          if(state->chip.setup.odr > 5)
          {
              state->chip.setup.odr = 5;
          }
      }
      ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, (alsTime_us + proxTime_us));

      /* if((state->chip.setup.prox.odr > 0) && (state->chip.setup.als.odr > 0)) */
      /* { */
      /*     if(state->chip.setup.prox.odr > 5) */
      /*     { */
      /*         state->chip.setup.prox.odr = 5; */
      /*     } */

      /*     if(state->chip.setup.als.odr > 5) */
      /*     { */
      /*         state->chip.setup.als.odr = 5; */
      /*     } */

      /*     // Both als and prox are enabled at the same time */
      /*     // Now see which is greater */
      /*     if(state->chip.setup.prox.odr > state->chip.setup.als.odr) */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.prox.odr, (alsTime_us + proxTime_us)); */
      /*     } */
      /*     else  // state->chip.setup.als.odr > state->chip.setup.prox.odr */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.als.odr, (alsTime_us + proxTime_us)); */
      /*     } */
      /* } */
      /* else  // only one or the other */
      /* { */
      /*     if(state->chip.setup.odr > 0) */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, alsTime_us); */
      /*     } */
      /*     else  // state->chip.setup.prox.odr > 0 */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, proxTime_us); */
      /*     } */
      /* } */

      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  DDF_MSG_3(MED, "TMG399 : %d final wtime: %d final ltime: %d", 1157, state->chip.setup.wait.wtime, state->chip.setup.wait.lTime);

  status = ams_tmg399x_write_wtime(state);

  return(status);
}

//                                          0  1   2   3
//static uint8_t ams_tmg399x_gain_value[] = { 1, 4, 16, 64 };

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


//uint32_t ams_tmg399x_Get_Max_ALS_Count(sns_dd_alsprx_state_t* state)
//{
//    uint32_t retVal;
//
//    if (state->chip.setup.als.atime_us > 63) retVal = 65535; //65k * 3 / 4
//    retVal = (state->chip.setup.als.atime_us * 1024) - 1;
//
//    return retVal;
//}
//

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


static void ams_tmg399x_set_lux_coefs(sns_dd_alsprx_state_t* state)
{
    int i;

    for(i = 0; (i < ARR_SIZE(state->chip.segment)) &&
               (i < ARR_SIZE(ams_tmg399x_lux_coef)); ++i)
    {
        state->chip.segment[i].d_factor  = ams_tmg399x_lux_coef[i].d_factor;
        state->chip.segment[i].r_coef    = ams_tmg399x_lux_coef[i].r_coef;
        state->chip.segment[i].g_coef    = ams_tmg399x_lux_coef[i].g_coef;
        state->chip.segment[i].b_coef    = ams_tmg399x_lux_coef[i].b_coef;
        state->chip.segment[i].ct_coef   = ams_tmg399x_lux_coef[i].ct_coef;
        state->chip.segment[i].ct_offset = ams_tmg399x_lux_coef[i].ct_offset;
    }
}


/*
 * Provides initial operational parameter defaults.
 * These defaults may be changed through the device's get/set
 * attribute files.
 */
sns_ddf_status_e ams_tmg399x_set_default_reg_values(sns_dd_alsprx_state_t* state)
{
    uint8_t devID;
    sns_ddf_status_e      status;
    uint8_t *sh = state->chip.shadow;


    /* uint16_t tmp; */
    DDF_MSG_0(HIGH, "in set defualt reg values ");

    status = ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rID, &devID);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
        /* return status; */
    }

    switch(devID)
    {
    case 0x9C:
    case 0x9F:
        state->chip.device_index = 0;  // 3992
        break;

    case 0xA8:
    case 0xAB:
        state->chip.device_index = 1;  // 3993
        break;

    default:    // UNKNOWN device ERROR!!!!
        return SNS_DDF_EINVALID_PARAM;
        break;
    }

    sh[TMG399X_ATIME]    = 0x6B; /* 405ms */
    sh[TMG399X_WTIME]    =    0;
    sh[TMG399X_PERS]     = PRX_PERSIST(1) | ALS_PERSIST(2);
    sh[TMG399X_CONFIG1]  = TMG399X_CONFIG_DEFAULT_VALUE;
    sh[TMG399X_PPULSE]   =   10;

    state->chip.params.als_gain = 1;   // ALS Gain = 16
    state->chip.als_gain_auto   = TRUE;   // true;


    /* /\* Operational parameters *\/ */
    ams_tmg399x_set_lux_coefs(state);

    state->chip.setup.Min_Integ_Time_us = ams_tmg399x_min_integ_time_us[state->chip.device_index];

    /* must be a multiple of 50mS */
    state->chip.setup.als.atime_us = MS_TO_US(50);

    /* Min Proximity Integration time us */
    state->chip.setup.prox.ptime_us = MS_TO_US(1);   //state->chip.setup.Min_Integ_Time_us;

    /* this is actually an index into the gain table */
    /* assume clear glass as default */

    DDF_MSG_0(HIGH, "in set defualt reg values -gain ");
    /* default gain trim to account for aperture effects */
    state->chip.setup.als.gain_trim = 1000;

    /* Known external ALS reading used for calibration */
    state->chip.setup.als.cal_target = 130;

    /* CH0 'low' count to trigger interrupt */
    state->chip.setup.als.thresh_low = 0;

    /* CH0 'high' count to trigger interrupt */
    state->chip.setup.als.thresh_high = 0;

    /* Number of 'out of limits' ADC readings */
    /* 3 Consecutive prox values out of range before generating prox interrupt */
    /* Will generate an ALS interrupt after every ALS cycle. */
    state->chip.setup.persistence = PRX_PERSIST(1) | ALS_PERSIST(3);

    /* Default interrupt(s) enabled.
     * 0x00 = none, 0x10 = als, 0x20 = prx 0x30 = bth */
    state->chip.setup.interrupts_en = 0x00;
    state->chip.setup.reg.enable    = AMS_tmg399x_ENABLE_MSK_PON;

    /*default threshold (adjust either manually or with cal routine*/
    state->chip.setup.prox.detect  = PROX_DETECT;
    state->chip.setup.prox.release = PROX_RELEASE;

    /* wait time between prox and als */
    state->chip.setup.wait.wtime = 1;
    state->chip.setup.wait.lTime = 0;

    /* Config register */
    state->chip.setup.reg.config = TMG399X_CONFIG_DEFAULT_VALUE;

    DDF_MSG_0(HIGH, "in set defualt reg values -prox detect ");

    /* Set the prox thresh for detect */
    state->chip.setup.prox.thresh_low  = 0x0000;
    state->chip.setup.prox.thresh_high = state->chip.setup.prox.detect;
    state->chip.data.prox.event        = SNS_PRX_FAR_AWAY;

    state->chip.setup.als.luxCorrection = 1 << 10;

    state->chip.setup.prox.max_samples_cal = 100;
    state->chip.setup.prox.pulse_count     = 8;
    state->chip.setup.odr                  = 0;

    //(0 << 6) | (1 << 5) | (0 << 4) | (0 << 2) | (1);
    state->chip.setup.reg.control = TMG399X_LDRIVE_100pc | TMG399X_PGAIN_4 | TMG399X_AGAIN_4;
    state->chip.setup.reg.config2 = TMG399X_LEDBOOTST_100 | (1);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_tmg399x_set_reg_values_from_nv

DESCRIPTION   Reads values from registry NV param store and assigns to sensor

DEPENDENCIES  None

RETURN VALUE  No Return

SIDE EFFECT   None

===========================================================================*/


void ams_tmg399x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state)
{
    // Now set NV values

    // device
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.device);
    if((state->sns_dd_alsprx_common_db.nv_db.device != 0) && (state->sns_dd_alsprx_common_db.nv_db.device <= 9))
    {
        /* state->chip.ams_sensor = (AMS_TMG399X_SENSOR_TYPE)state->sns_dd_alsprx_common_db.nv_db.device; */
    }

    // DGF
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.DGF);
    if(state->chip.als_setup.DGF != state->sns_dd_alsprx_common_db.nv_db.DGF)
    {
        state->chip.als_setup.DGF = state->sns_dd_alsprx_common_db.nv_db.DGF;
    }

    // CoefB
    SNS_PRINTF_MEDIUM_2(1169, state->sns_dd_alsprx_common_db.nv_db.iCoefB);
    if(state->chip.als_setup.iCoefB != state->sns_dd_alsprx_common_db.nv_db.iCoefB)
    {
        state->chip.als_setup.iCoefB = state->sns_dd_alsprx_common_db.nv_db.iCoefB;
    }

    // CoefC
    SNS_PRINTF_MEDIUM_2(1170, state->sns_dd_alsprx_common_db.nv_db.iCoefC);
    if(state->chip.als_setup.iCoefC != state->sns_dd_alsprx_common_db.nv_db.iCoefC)
    {
        state->chip.als_setup.iCoefC = state->sns_dd_alsprx_common_db.nv_db.iCoefC;
    }

    // CoefD
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.iCoefD);
    if(state->chip.als_setup.iCoefD != state->sns_dd_alsprx_common_db.nv_db.iCoefD)
    {
        state->chip.als_setup.iCoefD = state->sns_dd_alsprx_common_db.nv_db.iCoefD;
    }

    // proxDetect
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxDetect);
    if(state->chip.setup.prox.detect != state->sns_dd_alsprx_common_db.nv_db.proxDetect)
    {
        state->chip.setup.prox.detect = state->sns_dd_alsprx_common_db.nv_db.proxDetect;
    }

    // proxRelease
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxRelease);
    if(state->chip.setup.prox.release != state->sns_dd_alsprx_common_db.nv_db.proxRelease)
    {
        state->chip.setup.prox.release = state->sns_dd_alsprx_common_db.nv_db.proxRelease;
    }

    // luxCorrection
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.luxCorrection);
    if(state->chip.setup.als.luxCorrection != state->sns_dd_alsprx_common_db.nv_db.luxCorrection)
    {
        state->chip.setup.als.luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;
        DDF_MSG_1(MED, "lux corr written in default %d", state->chip.setup.als.luxCorrection);
    }


}


#define NUM_OF_LUX_TO_AVE  8


static sns_ddf_status_e ams_tmg399x_compute_lux_ave(sns_dd_alsprx_state_t* state)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status;

    // Set the device registers - just reset earlier
    /* status = sns_dd_ams_tmg399x_alsprx_reset(state); */
    /* if( status != SNS_DDF_SUCCESS ) */
    /* { */
    /*     SNS_PRINTF_MEDIUM_1(1172); */
    /* } */

    luxSum = 0;

    DDF_MSG_0(HIGH, "about to get als data and mlux ");

    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        status = ams_tmg399x_read_all_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed to read data ");
            /* return status; */
        }

        status = ams_tmg399x_get_als_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed in get als data ");
            /* return status; */
        }

        /* status = ams_tmg399x_API_APP_Get_mLUX(state); */
        status = ams_tmg399x_get_lux(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed in app get mlux ");
            /* return status; */
        }

        luxSum += state->chip.data.als.lux;
        DDF_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        // Wait some time before another reading
        sns_ddf_delay(15000);
        DDF_MSG_0(HIGH, "after 15000 us delay ");
    }

    state->chip.data.als.luxAve = (luxSum / NUM_OF_LUX_TO_AVE) / 1000;
    DDF_MSG_2(MED, "luxsum %d luxsum/8/1000: %d", luxSum, ((luxSum / NUM_OF_LUX_TO_AVE)/1000));

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_tmg399x_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/


sns_ddf_status_e ams_tmg399x_calibrate_als(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    uint32_t  tgtLux;
    uint32_t  avgLux;

    avgLux = 1;
    tgtLux = 1;

    // compare the average lux to the passed in calibration lux,
    DDF_MSG_0(HIGH, "in calibrate als next compute lux ");
    status = ams_tmg399x_compute_lux_ave(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "computelux ave failed ");
        return status;
    }
    DDF_MSG_0(HIGH, "out of compute lux ");

    // Find Lux mid way between calLuxLower and calLuxUpper
    tgtLux = (state->sns_dd_alsprx_common_db.nv_db.calLuxLower + state->sns_dd_alsprx_common_db.nv_db.calLuxUpper) / 2;
    DDF_MSG_2(MED, "tgtLux: %d callxlw: %d", tgtLux, state->sns_dd_alsprx_common_db.nv_db.calLuxLower);
    //tgtLux= tgtLux;    // removed left shift by 1024
    avgLux = state->chip.data.als.luxAve;

    DDF_MSG_2(MED, "tgtLux<<10: %d callxhi: %d", tgtLux, state->sns_dd_alsprx_common_db.nv_db.calLuxUpper);

    // Mult by 2^10
    state->sns_dd_alsprx_common_db.nv_db.luxCorrection = ((tgtLux << 10) / avgLux);

    /* if((state->sns_dd_alsprx_common_db.nv_db.luxCorrection) < 1) */
    /* { */
    /*     state->sns_dd_alsprx_common_db.nv_db.luxCorrection = 1;  //1 is minimum */
    /* } */

    state->chip.setup.als.luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;

    DDF_MSG_2(MED, "avgLux: %d new val of state->sns_dd_alsprx_common_db.nv_db.luxCorrection: %d", avgLux, state->sns_dd_alsprx_common_db.nv_db.luxCorrection);

    // To use:
    // ((lux * state->sns_dd_alsprx_common_db.nv_db.luxCorrection) + 512) / 1024;
    // or
    // ((lux * state->sns_dd_alsprx_common_db.nv_db.luxCorrection) + 512) >> 10;

    return status;
}

/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_run_test

DESCRIPTION   OEM Self Test to calibrate ALS Lux. Takes several lux readings
              and calculates scaling factor for ALS reading

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t       smgr_handle;
    uint32_t alsTime_us;
    //uint32_t proxTime_us;

    if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        DDF_MSG_0(HIGH, "in selftest sensor_ambient ");

        // Test the ALS
        if(test == SNS_DDF_TEST_OEM)
        {
            DDF_MSG_0(HIGH, "in selftest sensor_ambient OEM test ");
            // Perform ams specific test to Calibrate ALS
            if(state->sns_dd_alsprx_common_db.nv_db.calibratePhone)
            {
                // Set Default values
                //fv Read NV
                //fv ams_tmg399x_set_default_reg_values(state);

                status = sns_dd_ams_tmg399x_alsprx_reset(state);
                DDF_MSG_0(HIGH, "after reset");
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "set def reg values failed ");
                    return status;
                }

                alsTime_us  = 0;
                //proxTime_us =0;

                state->chip.setup.odr = 10;
                state->chip.setup.odr = 10;

                /* state->chip.setup.als.odr = 10; */
                /* state->chip.setup.prox.odr = 10; */

                //  enable als and prox. sensor
                status = sns_dd_ams_tmg399x_alsprx_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);


                alsTime_us  = ams_tmg399x_als_time_us(state);
                //proxTime_us = ams_tmg399x_prox_time_us(state);
                ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, alsTime_us);
                /* ams_tmg399x_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us); */

                //ams_tmg399x_compute_new_wtime(state, state->chip.setup.prox.odr, proxTime_us);
                status = ams_tmg399x_write_wtime(state);
                sns_ddf_delay(125000);

                // update als scaling
                DDF_MSG_0(HIGH, "into als scaling");
                status = ams_tmg399x_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "calib als failed ");
                    return status;
                }
                DDF_MSG_0(HIGH, "out of als scaling");
                smgr_handle = state->smgr_handle;
                DDF_MSG_0(HIGH, "reg update notify next");
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    SNS_PRINTF_MEDIUM_1(100010);
                    return status;
                }
                DDF_MSG_0(HIGH, "successful reg update notify");
            }
        }
    }

    return SNS_DDF_SUCCESS;
}
