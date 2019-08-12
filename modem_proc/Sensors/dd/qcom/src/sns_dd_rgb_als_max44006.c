/********************************************************************************
* Copyright (c) 2013, Maxim Integrated Products, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Maxim Integrated nor the
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
* Copyright (c) 2010-2014 by Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Proprietary and Confidential
******************************************************************************/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
08-08-2014   MW      Eliminate KW error
07-22-2014   MW      Eliminate compiler warning
06-05-2014   MW      Fixed debug log in set_attrib(): CR 675464
==============================================================================*/

/*
TODO:
1. Add registry support - done
2. Add DRI for RGB - done
3. Change RGB return data to uW/cm2 - done
4. Add self-test - done
5. probe function
6. Remove fake pri sensor type

*/

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "stdbool.h"
#include <string.h>
#include "sns_dd_rgb_als_max44006.h"

#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_memmgr.h"

/* Basic definitions */
sns_ddf_driver_if_s sns_als_rgb_max44006_driver_if;

#define QUALCOMM_MAX_LUX				32767

#define DD_HANDLE_ALIGN_BITS			0xFFFFFFFC

#define MICROSECS_IN_MILLISEC 			1000

static int32_t als_id = -1;
static int32_t rgb_id = -1;

static sns_ddf_sensor_e sns_dd_als_sensor_types[] =
{
    SNS_DDF_SENSOR_PROXIMITY,//this is a hack
    SNS_DDF_SENSOR_AMBIENT
};

static sns_ddf_sensor_e sns_dd_rgb_sensor_types[] =
{
    SNS_DDF_SENSOR_RGB,
    SNS_DDF_SENSOR_CT_C
};

static my_dd_head_s max44006_dd_head =
{
    NULL,{"als_rgb", 1}
};


/*===========================================================================
===========================================================================*/
/* function prototypes */
/*===========================================================================
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_set_power_mode
    (
    sns_dd_als_rgb_state_t* dd_handle,
    sns_ddf_powerstate_e	op_mode,
    uint32_t 				idx
    );

static sns_ddf_status_e sns_dd_max44006_reset_chip
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    );

static void max44006_dd_fill_sub_dev_slot
    (
    max44006_sub_dev_s*           sub_dev,
    uint32_t*                     num_sensors,
    sns_ddf_sensor_e*             sensors[],
    int                           dev_index,
    sns_ddf_handle_t              smgr_handle,
    uint16_t                      gpio
    );

static sns_ddf_status_e sns_dd_als_max44006_read_clr
    (
    sns_dd_als_rgb_state_t* state,
    int* als_data
    );

static sns_ddf_status_e sns_dd_als_max44006_read_rgb
    (
    sns_dd_als_rgb_state_t* state,
    int* r_data, int* g_data, int* b_data
    );

static sns_ddf_status_e sns_dd_als_max44006_read_ir
    (
    sns_dd_als_rgb_state_t* state,
    int* ir_data
    );

static sns_ddf_status_e sns_dd_als_max44006_read_temp
    (
    sns_dd_als_rgb_state_t* state,
    int* temp_data
    );

static sns_ddf_status_e sns_dd_als_max44006_read_ir_comp
    (
    sns_dd_als_rgb_state_t* state,
    int* ir_comp_data
    );

static sns_ddf_status_e sns_als_max44006_driver_update_int_thresh
    (
    sns_dd_als_rgb_state_t* state,
    uint16_t hi_tresh_val,
    uint16_t lo_tresh_val,
    uint8_t amb_pst_tmr
    );

static sns_ddf_status_e sns_dd_max44006_disable_interrupt
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    );
static sns_ddf_status_e sns_dd_max44006_enable_interrupt
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    );


static sns_ddf_status_e sns_dd_als_max44006_get_lux
    (
    sns_dd_als_rgb_state_t* state,
    int *lux_val,
    int *clr_data
    );

static sns_ddf_status_e sns_dd_als_max44006_init_lowpower
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    );

static sns_ddf_status_e sns_dd_als_max44006_init_normal
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    );

static sns_ddf_status_e sns_dd_als_max44006_connectivity_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  );

static sns_ddf_status_e sns_dd_max44006_oem_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  );


static sns_ddf_status_e sns_dd_als_max44006_run_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  );

static void sns_dd_max44006_handle_timer 
(
  sns_ddf_handle_t handle,
  void* arg
);

static sns_ddf_status_e sns_dd_max44006_oem_test_none(
  sns_dd_als_rgb_state_t* state,
  void* arg
);

static sns_ddf_status_e sns_dd_max44006_oem_test_low_gain_check(
  sns_dd_als_rgb_state_t* state,
  void* arg
);

static sns_ddf_status_e sns_dd_max44006_oem_test_high_gain_check(
  sns_dd_als_rgb_state_t* state,
  void* arg
);

static sns_ddf_status_e sns_dd_max44006_oem_test_gain_trim(
  sns_dd_als_rgb_state_t* state, uint16_t amb_reg_addr, 
  uint32_t low_thresh, uint32_t hi_thresh,
  void* arg, bool * done
);


static sns_ddf_status_e sns_dd_max44006_check_threshold(
    sns_dd_als_rgb_state_t* state,
    uint16_t amb_reg_addr,uint32_t low_thresh, uint32_t hi_thresh,
    uint32_t * amb_data,
    max44006_dd_amb_data_check_e * result
);

static sns_ddf_status_e sns_dd_max44006_save_data_from_part_to_state(
    sns_dd_als_rgb_state_t* state,
    uint16_t amb_reg_addr, uint32_t amb_data,
    uint8_t trim_value

);

static sns_ddf_status_e sns_dd_max44006_write_all_gain_regs(
  sns_dd_als_rgb_state_t* state,
  uint8_t gain
);
    
static bool	sns_max_44006_check_reg02_val
(
    uint32_t reg_02
)
{
    uint32_t amb_tim;
    amb_tim = (reg_02 >> MAX44006_AMB_TIM_BITS_SHIFT) & MAX44006_AMB_TIM_BITS_MASK;
    if (amb_tim > 0x04)
    {
        MAX44006_MSG_1(HIGH, "sns_max_44006_check_reg02_val - amb_tim is invalid %u",amb_tim);
        return false;
    }
    return true;
}

void sns_dd_max44006_log_data
    (
    sns_dd_als_rgb_state_t* state,
    sns_ddf_sensor_e       sensor,
    uint32_t               data1, 
    uint32_t               data2,
    uint32_t               data3,
    uint32_t               data4,
    uint32_t               data5 
    )
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
        log_struct_ptr->sensor_id = sensor;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_MAXIM;

        /* Timestamp the log with sample time */
        log_struct_ptr->timestamp = sns_ddf_get_timestamp();

        /* Log the sensor data */
        if (sensor == SNS_DDF_SENSOR_AMBIENT)
        {
            log_struct_ptr->num_data_types = 3;
            log_struct_ptr->data[0] = data1;
            log_struct_ptr->data[1] = data2;
            log_struct_ptr->data[2] = data3;
        }
        else if(sensor == SNS_DDF_SENSOR_RGB)
        {
            log_struct_ptr->num_data_types = 3;
            log_struct_ptr->data[0] = data1;
            log_struct_ptr->data[1] = data2;
            log_struct_ptr->data[2] = data3;
        }
        else if(sensor == SNS_DDF_SENSOR_CT_C)
        {
            log_struct_ptr->num_data_types = 2;
            log_struct_ptr->data[0] = data1;
            log_struct_ptr->data[1] = data2;
        }

        /* Commit log (also frees up the log packet memory) */
        err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
            log_struct_ptr);
    }
    else
    {
        /* Do nothing */
    }

}

void calcxy(float red, float green, float blue, float *x, float *y)
{
    float X,Y,Z;
    float Matrix[3][3] = {{67.17, 24.75, -0.0864},
    {11.06, 71.32, -6.37}, 
    {7.22, -17.65, 79.24}};

    // Cells[0][0]=67.17;
    // Cells[0][1]=24.75;
    // Cells[0][2]=-0.0864;
    // Cells[1][0]=11.06;
    // Cells[1][1]=71.32;
    // Cells[1][2]=-6.37;
    // Cells[2][0]=7.22;
    // Cells[2][1]=-17.65;
    // Cells[2][2]=79.24;

    X=Matrix[0][0]*red+Matrix[1][0]*green+Matrix[2][0]*blue;
    Y=Matrix[0][1]*red+Matrix[1][1]*green+Matrix[2][1]*blue;
    Z=Matrix[0][2]*red+Matrix[1][2]*green+Matrix[2][2]*blue;

    if((X+Y+Z)==0)
    {
        *x=0;
        *y=0;
    }
    else
    {
        *x=X/(X+Y+Z);
        *y=Y/(X+Y+Z);
    }
}

float get_cct_value(int r, int g, int b)
{
    float x,y;
    float n;
    float cct;

    calcxy(r, g, b, &x, &y);

    //Placing ShapeCircle on the Chromaticity diagram
    if(x<0 || y<0 || x>.8  || y>.8)
    {
        x=0;
        y=0;
    }

    n=(x-0.3320)/(0.1858-y);
    cct = 449*(n*n*n)+3525*(n*n)+6823.3*n+5520.33;

    return cct;
}

/*===========================================================================*/
/* function definitions */
/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_set_power_mode: sets the chip to normal and
*        low power states.
*
* @param[in] state    Handle to driver instance.
* @param[in] op_mode  Power mode desired
*
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_set_power_mode
    (
    sns_dd_als_rgb_state_t*	state,
    sns_ddf_powerstate_e	op_mode,
    uint32_t				idx
    )
{
    sns_ddf_status_e ret = SNS_DDF_SUCCESS;

    MAX44006_MSG_0(HIGH, "sns_dd_als_max44006_set_power_mode - enter");

    if(op_mode == SNS_DDF_POWERSTATE_LOWPOWER)
    {		
        if (idx == als_id)
        {
            if (rgb_id != -1)
            {
                if(state->sub_dev[rgb_id].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
                {
                    // dont put chip in low power
                    MAX44006_MSG_0(HIGH, "RGB is active.");
                    state->sub_dev[als_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
                    return ret;
                }
                else
                {
                    // OK to put in low power
                    ret = sns_dd_als_max44006_init_lowpower(state, idx);
                }
            }
            else
            {
                // OK to put in low power
                ret = sns_dd_als_max44006_init_lowpower(state, idx);
            }
        }
        else if (idx == rgb_id)
        {
            if (als_id != -1)
            {
                if(state->sub_dev[als_id].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
                {
                    // dont put chip in low power
                    MAX44006_MSG_0(HIGH, "ALS is active.");
                    state->sub_dev[rgb_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
                    return ret;
                }
                else
                {
                    // OK to put in low power
                    ret = sns_dd_als_max44006_init_lowpower(state, idx);
                }
            }
            else
            {
                // OK to put in low power
                ret = sns_dd_als_max44006_init_lowpower(state, idx);
            }
        }
    }
    else
    {
        // good to put in active power.
        ret = sns_dd_als_max44006_init_normal(state, idx);
    }

    return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_reset: sets the chip to low power state.
*        Please note that MAX44006 power consumption even in normal state
*        is comparable to shutdown power of most parts.
*
* @param[in] dd_handle Handle to driver instance.
*
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_reset
    (
    sns_ddf_handle_t dd_handle
    )
{
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);
    uint32_t idx = (uint32_t)dd_handle & 3;

    MAX44006_MSG_1(HIGH, "sns_dd_als_max44006_reset - idx=%d", idx);

    /* basic sanity check */
    if (dd_handle == NULL)
        return SNS_DDF_EINVALID_PARAM;

    if(idx >= MAX44006_DD_MAX_SENSOR_TYPES)
        return SNS_DDF_EINVALID_PARAM;

    // Notify SMGR to update registry in case there was failure the first time
    if((state->reg_ret != SNS_DDF_SUCCESS) && (als_id != -1))
    {
        state->reg_ret = sns_ddf_smgr_notify_event(state->sub_dev[als_id].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
        MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_event = %d", state->reg_ret);
    }

    // Notify SMGR to update registry in case there was failure the first time
    if((state->reg_rgb_ret != SNS_DDF_SUCCESS) && (rgb_id != -1))
    {
        state->reg_rgb_ret = sns_ddf_smgr_notify_event(state->sub_dev[rgb_id].smgr_handle, SNS_DDF_SENSOR_RGB, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
        MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_event = %d", state->reg_rgb_ret);
    }

    state->sub_dev[idx].enable = false;

    /* Dont reset all chip for particular sensor*/

    if((idx == als_id) && state->sub_dev[als_id].powerstate == SNS_DDF_POWERSTATE_LOWPOWER)
    {
        if(rgb_id != -1)
        {
            if (state->sub_dev[rgb_id].powerstate == SNS_DDF_POWERSTATE_LOWPOWER)
            {
                // good to reset
                return sns_dd_max44006_reset_chip(state, idx);
            }
            else{
                // cannot reset because rgb is running
                MAX44006_MSG_0(HIGH, "RGB is active");
                return SNS_DDF_SUCCESS;
            }
        }
        else{
            // good to reset
            return sns_dd_max44006_reset_chip(state, idx);
        }
    }

    if((idx == rgb_id) && state->sub_dev[rgb_id].powerstate == SNS_DDF_POWERSTATE_LOWPOWER)
    {
        if(als_id != -1)
        {
            if (state->sub_dev[als_id].powerstate == SNS_DDF_POWERSTATE_LOWPOWER)
            {
                // good to reset
                return sns_dd_max44006_reset_chip(state, idx);
            }
            else{
                // cannot reset because als is running
                MAX44006_MSG_0(HIGH, "ALS is active");
                return SNS_DDF_SUCCESS;
            }
        }
        else{
            // good to reset
            return sns_dd_max44006_reset_chip(state, idx);
        }
    }

    MAX44006_MSG_0(HIGH, "sns_dd_als_max44006_reset - exit");
    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e max44006_dd_read_als_registry
    (
    sns_dd_als_rgb_state_t* state, 
    uint32_t                idx,
    sns_ddf_nv_params_s*    reg_params
    )
{
    bool data_from_registry = false;
    max44006_dd_nv_param_s*  nv_ptr;

    MAX44006_MSG_0(HIGH, "max44006_dd_read_als_registry - enter");

    /* Recall NV parameters if any */
    if (reg_params)
    {
        if ( (reg_params->status == SNS_DDF_SUCCESS) && (reg_params->data_len >= sizeof(max44006_dd_nv_param_s)) )
        {
            nv_ptr = (max44006_dd_nv_param_s *)reg_params->data;

            if ( (nv_ptr->max44006_magic_num == MAX44006_MAGIC_NUM) && (reg_params->data_len != 0) )
            {
                // update value from NV parameters
                data_from_registry = true;

                state->nv_size = reg_params->data_len;

                state->nv_param.max44006_magic_num = nv_ptr->max44006_magic_num;
                state->nv_param.visible_ratio = nv_ptr->visible_ratio;
                state->nv_param.ir_ratio = nv_ptr->ir_ratio;
                state->nv_param.dc_offset = nv_ptr->dc_offset;
                state->nv_param.als_thresh_hi = nv_ptr->als_thresh_hi;
                state->nv_param.als_thresh_lo = nv_ptr->als_thresh_lo;
                state->nv_param.reserved_1 = nv_ptr->reserved_1;
                state->nv_param.als_factor = nv_ptr->als_factor;
                state->nv_param.reserved_2 = nv_ptr->reserved_2;
                state->nv_param.als_gain = nv_ptr->als_gain;
                state->nv_param.als_time = nv_ptr->als_time;
            }
        }
        if (!data_from_registry)
        {
            MAX44006_MSG_0(HIGH, "Using defaults for NV params for ALS");
            // default value of NV parameters
            state->nv_size = 128;

            state->nv_param.max44006_magic_num = MAX44006_MAGIC_NUM;
            state->nv_param.visible_ratio = 100;
            state->nv_param.ir_ratio = 100;
            state->nv_param.dc_offset = 0;
            state->nv_param.als_thresh_hi = MAX44006_AMB_TRESH_HI;
            state->nv_param.als_thresh_lo = MAX44006_AMB_TRESH_LO;
            state->nv_param.reserved_1 = 0;
            state->nv_param.als_factor = 100;
            state->nv_param.reserved_2 = 0;
            state->nv_param.als_gain = MAX44006_AMB_PGA_FACTOR_BITS;
            state->nv_param.als_time = MAX44006_AMB_TIM_BITS;

            // Notify SMGR to update registry
            state->reg_ret = sns_ddf_smgr_notify_event(state->sub_dev[idx].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
            MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_event = %d", state->reg_ret);
        }

    }

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e max44006_dd_read_rgb_registry
    (
    sns_dd_als_rgb_state_t* state, 
    uint32_t                idx,
    sns_ddf_nv_params_s*    reg_params
    )
{
    bool data_from_registry = false;
    max44006_dd_rgb_nv_param_s*  nv_ptr;
    sns_ddf_status_e ret;
    uint8_t reg_val;
    uint8_t nbytes;

    MAX44006_MSG_0(HIGH, "max44006_dd_read_rgb_registry - enter");

    /* Recall NV parameters if any */
    if (reg_params)
    {	
        if ( (reg_params->status == SNS_DDF_SUCCESS) && (reg_params->data_len >= sizeof(max44006_dd_rgb_nv_param_s)) )
        {
            nv_ptr = (max44006_dd_rgb_nv_param_s *)reg_params->data;

            if ( (nv_ptr->magic_num == MAX44006_MAGIC_NUM) && (reg_params->data_len != 0) ) 
            {
                state->nv_rgb_param.magic_num = nv_ptr->magic_num;
                state->nv_rgb_param.r_low_thresh = nv_ptr->r_low_thresh;
                state->nv_rgb_param.r_high_thresh = nv_ptr->r_high_thresh;
                state->nv_rgb_param.g_low_thresh = nv_ptr->g_low_thresh;
                state->nv_rgb_param.g_high_thresh = nv_ptr->g_high_thresh;
                state->nv_rgb_param.b_low_thresh = nv_ptr->b_low_thresh;
                state->nv_rgb_param.b_high_thresh = nv_ptr->b_high_thresh;
                state->nv_rgb_param.c_low_thresh = nv_ptr->c_low_thresh;
                state->nv_rgb_param.c_high_thresh = nv_ptr->c_high_thresh;
                state->nv_rgb_param.ir_low_thresh = nv_ptr->ir_low_thresh;
                state->nv_rgb_param.ir_high_thresh = nv_ptr->ir_high_thresh;
                state->nv_rgb_param.register_02 =  nv_ptr->register_02;
                state->nv_rgb_size = reg_params->data_len;

                data_from_registry = true;
            }
        }
        if (!data_from_registry)
        {
            MAX44006_MSG_0(HIGH, "Using defaults for NV params for RGB");
            // default value of NV parameters. Note, register_02 is not set. use default values inited in driver
            state->nv_rgb_size = 128;

            ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_CLR_REG,
                &reg_val, 1, &nbytes);
            
            if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
            {
                MAX44006_MSG_2(ERROR, "max44006_dd_read_rgb_registry -"
                    "read SNS_DD_MAX44006_TRIM_GAIN_CLR_REG error=%d, nbtres %d", ret, nbytes);
                return ret;
            }
            state->nv_rgb_param.c_gain_trim = reg_val & MAX44006_GAIN_TRIM_MASK;

            ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_RED_REG,
                &reg_val, 1, &nbytes);
            
            if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
            {
                MAX44006_MSG_2(ERROR, "max44006_dd_read_rgb_registry -"
                    "read SNS_DD_MAX44006_TRIM_GAIN_RED_REG error=%d, nbytes %d", ret, nbytes);
                return ret;
            }
            state->nv_rgb_param.r_gain_trim = reg_val & MAX44006_GAIN_TRIM_MASK;

            ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG,
                &reg_val, 1, &nbytes);
            
            if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
            {
                MAX44006_MSG_2(ERROR, "max44006_dd_read_rgb_registry -"
                    "read SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG error=%d, nbytes %d", ret, nbytes);
                return ret;
            }
            state->nv_rgb_param.b_gain_trim = reg_val & MAX44006_GAIN_TRIM_MASK;

            ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG,
                &reg_val, 1, &nbytes);
            
            if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
            {
                MAX44006_MSG_2(ERROR, "max44006_dd_read_rgb_registry -"
                    "read SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG error=%d, nbytes %d", ret, nbytes);
                return ret;
            }
            state->nv_rgb_param.g_gain_trim = reg_val & MAX44006_GAIN_TRIM_MASK;

            ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_IR_REG,
                &reg_val, 1, &nbytes);
            
            if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
            {
                MAX44006_MSG_2(ERROR, "max44006_dd_read_rgb_registry -"
                    "read SNS_DD_MAX44006_TRIM_GAIN_IR_REG error=%d, nbytes %d, nbytes %d", ret, nbytes);
                return ret;
            }
            state->nv_rgb_param.ir_gain_trim = reg_val & MAX44006_GAIN_TRIM_MASK;

            MAX44006_MSG_0(HIGH, "max44006_dd_read_rgb_registry - "
                                "populated trim nv params successfully");

            
            // tested with 6.25 ms integ time, ampga = 2
            state->nv_rgb_param.c_low_thresh = 0x3000;
            state->nv_rgb_param.c_high_thresh = 0x30A8;
            state->nv_rgb_param.r_low_thresh = 0x2000;
            state->nv_rgb_param.r_high_thresh = 0x20A8;
            state->nv_rgb_param.g_low_thresh = 0x2000;
            state->nv_rgb_param.g_high_thresh = 0x20A8;
            state->nv_rgb_param.b_low_thresh = 0x2000;
            state->nv_rgb_param.b_high_thresh = 0x20A8;
            state->nv_rgb_param.ir_low_thresh = 0x1600;
            state->nv_rgb_param.ir_high_thresh = 0x16A8;
            /*NOTE: only OEM Test uses this register_02 value below*/
            state->nv_rgb_param.register_02  = ((MAX44006_AMB_PGA_FACTOR_BITS_OEM_TEST & 3) << 0) |
                                                ((MAX44006_AMB_TIM_BITS_OEM_TEST & 7) << 2)| 
                                                ((MAX44006_TEMPEN_BIT & 1) << 5) |
                                                ((MAX44006_COMPEN_BIT & 1) << 6) |
                                                ((MAX44006_TRIM_BIT & 1) << 7);

            state->nv_rgb_param.magic_num = MAX44006_MAGIC_NUM;
    
            // Notify SMGR to update registry
            state->reg_rgb_ret = sns_ddf_smgr_notify_event(state->sub_dev[idx].smgr_handle, SNS_DDF_SENSOR_RGB, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
            MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_event = %d", state->reg_rgb_ret);
        }
        state->nv_rgb_param.data_from_registry = (uint32_t)data_from_registry;

    }

    return SNS_DDF_SUCCESS;
}

static void max44006_dd_fill_sub_dev_slot
    (
    max44006_sub_dev_s*           sub_dev,
    uint32_t*                     num_sensors,
    sns_ddf_sensor_e*             sensors[],
    int                           dev_index,
    sns_ddf_handle_t              smgr_handle,
    uint16_t                      gpio
    )
{
    if (dev_index == als_id)
    {
        *sensors = (sns_ddf_sensor_e *)sns_dd_als_sensor_types;
        *num_sensors = 2;
        sub_dev->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
        sub_dev->smgr_handle = smgr_handle;
        sub_dev->irq = gpio;
        if(gpio == 0xFFFF)
        {
            sub_dev->dri_enable = false;
        }
        else
        {
            sub_dev->dri_enable = true;
        }
    }
    else if (dev_index == rgb_id)
    {
        *sensors = (sns_ddf_sensor_e *)sns_dd_rgb_sensor_types;
        *num_sensors = 2;
        sub_dev->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
        sub_dev->smgr_handle = smgr_handle;
        sub_dev->irq = gpio;
        if(gpio == 0xFFFF)
        {
            sub_dev->dri_enable = false;
        }
        else
        {
            sub_dev->dri_enable = true;
        }
    }
    else
        return;
}

sns_ddf_status_e sns_dd_als_max44006_init
    (
    sns_ddf_handle_t*        dd_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     reg_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors
    )
{
    sns_dd_als_rgb_state_t*		state;
    sns_ddf_status_e        ret;
    int                     dev_index;

    /* basic sanity check */
    if (dd_ptr == NULL || sensors == NULL || num_sensors == NULL)
    {
        MAX44006_MSG_0(ERROR,"Invalid input parameters");
        return SNS_DDF_EINVALID_PARAM;
    }

    MAX44006_MSG_0(HIGH,"sns_dd_als_max44006_init - enter ");

    if (max44006_dd_head.next == NULL)
    {
        MAX44006_MSG_0(MED,"First init");
        /* Allocate a device instance and initialize it */

        if (sns_ddf_malloc((void **)&state, sizeof(*state)) != SNS_DDF_SUCCESS)
        {
            MAX44006_MSG_0(ERROR,"sns_ddf_malloc failed");
            return SNS_DDF_ENOMEM;
        }

        memset(state, 0, sizeof(*state));

        MAX44006_MSG_1(MED,"state = %p", state);

        /* link this dev instance to the front of the list */
        state->next = NULL;
        max44006_dd_head.next = state;

        /* open I2C port */
        ret = sns_ddf_open_port(&state->port_handle, &device_info->port_config);

        if (ret != SNS_DDF_SUCCESS)
        {
            sns_ddf_mfree(state);
            max44006_dd_head.next = NULL;
            MAX44006_MSG_1(ERROR,"sns_ddf_open_port error=%d", ret);

            return ret;
        }

        // TBD
        if ((*sensors)[0] == SNS_DDF_SENSOR_PROXIMITY) // hack uses SNS_DDF_SENSOR_PROXIMITY
        {
            MAX44006_MSG_0(MED,"ALS is being initialized");
            als_id = 0;
            dev_index = als_id;
            state->enabled_als = false;
            state->common.max44006_als_odr = 0;
            state->common.max44006_dummy_odr = 0;

            ret = max44006_dd_read_als_registry(state, dev_index, reg_params);
            if(ret != SNS_DDF_SUCCESS)
            {
                MAX44006_MSG_0(ERROR,"Registry read failed");
                return ret;
            }
        }
        else if ((*sensors)[0] == SNS_DDF_SENSOR_RGB)
        {
            MAX44006_MSG_0(MED,"RGB is being initialized");
            rgb_id = 0;
            dev_index = rgb_id;
            state->enabled_rgb = false;
            state->enabled_ct_c = false;
            state->common.max44006_rgb_odr = 0;
            state->common.max44006_ct_c_odr = 0;
            ret = max44006_dd_read_rgb_registry(state, dev_index, reg_params);
            if(ret != SNS_DDF_SUCCESS)
            {
                MAX44006_MSG_0(ERROR,"Registry read failed");
                return ret;
            }
        }
        else
        {
            MAX44006_MSG_2(ERROR,"Invalid sensor %d, num_sensors=%d", (*sensors)[0], *num_sensors);
            return SNS_DDF_EINVALID_PARAM;
        }

        max44006_dd_fill_sub_dev_slot(&state->sub_dev[0],
            num_sensors,
            sensors,
            dev_index,
            smgr_handle,
            device_info->first_gpio);

        ret = sns_dd_max44006_reset_chip(state, dev_index);

        if (ret != SNS_DDF_SUCCESS)
        {
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state);
            max44006_dd_head.next = NULL;
            MAX44006_MSG_1(ERROR,"sns_dd_max44006_reset_chip error=%d", ret);

            return ret;
        }

        *dd_ptr = (sns_ddf_handle_t)state;

    }
    else
    {
        state = max44006_dd_head.next;

        if ((*sensors)[0] == SNS_DDF_SENSOR_PROXIMITY) //  hack uses SNS_DDF_SENSOR_PROXIMITY
        {
            MAX44006_MSG_0(MED,"ALS is being initialized");
            als_id = 1;
            dev_index = als_id;
            state->enabled_als = false;
            state->common.max44006_als_odr = 0;
            state->common.max44006_dummy_odr = 0;

            ret = max44006_dd_read_als_registry(state, dev_index, reg_params);
            if(ret != SNS_DDF_SUCCESS)
            {
                MAX44006_MSG_0(ERROR,"Registry read failed");
                return ret;
            }
        }
        else if ((*sensors)[0] == SNS_DDF_SENSOR_RGB)
        {
            MAX44006_MSG_0(MED,"RGB is being initialized");
            rgb_id = 1;
            dev_index = rgb_id;
            state->enabled_rgb = false;
            state->enabled_ct_c = false;
            state->common.max44006_rgb_odr = 0;
            state->common.max44006_ct_c_odr = 0;
            ret = max44006_dd_read_rgb_registry(state, dev_index, reg_params);
            if(ret != SNS_DDF_SUCCESS)
            {
                MAX44006_MSG_0(ERROR,"Registry read failed");
                return ret;
            }
        }
        else
        {
            MAX44006_MSG_1(ERROR,"sensor[0]=%d", (*sensors)[0]);
            return SNS_DDF_EINVALID_PARAM;
        }

        max44006_dd_fill_sub_dev_slot(&state->sub_dev[1],
            num_sensors,
            sensors,
            dev_index,
            smgr_handle,
            device_info->first_gpio);
        ret = sns_dd_max44006_reset_chip(state, dev_index);

        if (ret != SNS_DDF_SUCCESS)
        {
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state);
            max44006_dd_head.next = NULL;

            return ret;
        }

        *dd_ptr = (sns_ddf_handle_t)((uint32_t)state | dev_index);

    }

    MAX44006_MSG_0(HIGH,"Init success");
    return ret;
}


/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_get_data: Requests a single sample of sensor data.
*       Data is returned immediately after being read from the sensor.
*
* @param[in]  dd_handle    Handle to driver instance.
* @param[in]  sensors      List of sensors for which data is requested.
* @param[in]  num_sensors  Length of @a sensors.
* @param[in]  memhandler   Memory handler used to dynamically allocate
*                          output parameters, if applicable.
* @param[out] data         Sampled sensor data.

* @return SNS_DDF_SUCCESS if data was populated successfully, error code
*         otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_get_data
    (
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s** data
    )
{
    sns_dd_als_rgb_state_t    *state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    uint32_t                  dev_index = (uint32_t)dd_handle & 3;
    sns_ddf_status_e          ret = SNS_DDF_SUCCESS;
    sns_ddf_sensor_data_s*    s_data;
    sns_ddf_sensor_sample_s*  samples;

    /* basic sanity check */
    if (dd_handle == NULL || sensors == NULL || memhandler == NULL || data == NULL) 
        return SNS_DDF_EINVALID_PARAM;

    if(sensors[0] == SNS_DDF_SENSOR_PROXIMITY) // hack uses SNS_DDF_SENSOR_PROXIMITY
        return SNS_DDF_EINVALID_PARAM;

    MAX44006_MSG_3(HIGH,"sns_dd_als_max44006_get_data dev_index=%d, sensor=%d, num_sensors=%d", dev_index, sensors[0], num_sensors);

    if (dev_index == als_id)
    {
        int lux_data = 0;
        int clr_data = 0;

        /* allocate sensor data structure */
        s_data = sns_ddf_memhandler_malloc(memhandler, sizeof(*s_data));

        if (s_data == NULL)
            return SNS_DDF_ENOMEM;

        *data = s_data;

        ret = sns_dd_als_max44006_get_lux(state,
            &lux_data,
            &clr_data);

        lux_data = (lux_data * state->nv_param.als_factor) / 100;

        MAX44006_MSG_2(MED,"lux_data=%d clr_data=%d", lux_data, clr_data);

        /* allocate samples buffer */
        samples = sns_ddf_memhandler_malloc(memhandler, 1 * sizeof(*samples));

        if (samples == NULL)
            return SNS_DDF_ENOMEM;

        if ( ret == SNS_DDF_SUCCESS)
        {
            samples[0].status = SNS_DDF_SUCCESS;
            samples[0].sample = lux_data << 16;
            s_data[0].num_samples = 1;
            s_data[0].sensor = SNS_DDF_SENSOR_AMBIENT;
            s_data[0].status = SNS_DDF_SUCCESS;
            s_data[0].timestamp = sns_ddf_get_timestamp();
            s_data[0].samples = samples;

            sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_AMBIENT, (lux_data*1000), samples[0].sample, clr_data, 0, 0);
        }

        return ret;
    }
    else if (dev_index == rgb_id)
    {
        int r_data;
        int g_data;
        int b_data;
        int c_data;
        int temp_data;
        int i;
        float cct_data;

        /* temp_data*/
        ret = sns_dd_als_max44006_read_temp(state, &temp_data);
        if ( ret != SNS_DDF_SUCCESS)
        {
            return ret = SNS_DDF_EFAIL;
        }	

        /* Clear data */
        ret =  sns_dd_als_max44006_read_clr(state, &c_data);
        if ( ret != SNS_DDF_SUCCESS)
        {
            return ret = SNS_DDF_EFAIL;
        }

        /* RGB data */
        ret = sns_dd_als_max44006_read_rgb(state, &r_data, &g_data, &b_data);
        if ( ret != SNS_DDF_SUCCESS)
        {
            return ret = SNS_DDF_EFAIL;
        }

        cct_data = get_cct_value(r_data, g_data, b_data);

        /* allocate sensor data structure */
        *data = sns_ddf_memhandler_malloc(
            memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

        if (*data == NULL)
            return SNS_DDF_ENOMEM;

        for (i = 0; i < num_sensors; i++)
        {
            s_data = &((*data)[i]);

            if( (samples = sns_ddf_memhandler_malloc(memhandler,
                (3*sizeof(sns_ddf_sensor_sample_s)))) == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            if (sensors[i] == SNS_DDF_SENSOR_RGB)
            {
                // s_data->sensor = SNS_DDF_SENSOR_RGB;
                // s_data->num_samples = 3;
                // samples[0].sample = (uint32_t)r_data;
                // samples[0].status = SNS_DDF_SUCCESS;
                // samples[1].sample = (uint32_t)g_data;
                // samples[1].status = SNS_DDF_SUCCESS;
                // samples[2].sample = (uint32_t)b_data;
                // samples[2].status = SNS_DDF_SUCCESS;

                s_data->sensor = SNS_DDF_SENSOR_RGB;
                s_data->num_samples = 3;
                samples[0].sample = FX_FLTTOFIX_Q16((float)r_data * RED_U_WATT_PER_LSB);
                samples[0].status = SNS_DDF_SUCCESS;
                samples[1].sample = FX_FLTTOFIX_Q16((float)g_data * GREEN_U_WATT_PER_LSB);
                samples[1].status = SNS_DDF_SUCCESS;
                samples[2].sample = FX_FLTTOFIX_Q16((float)b_data * BLUE_U_WATT_PER_LSB);
                samples[2].status = SNS_DDF_SUCCESS;

                sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_RGB, (uint32_t)r_data, (uint32_t)g_data, (uint32_t)b_data, 0, 0);
            }
            else
            {
                s_data->sensor = SNS_DDF_SENSOR_CT_C;
                s_data->num_samples = 2;
                samples[0].sample = FX_FLTTOFIX_Q16(cct_data);
                samples[0].status = SNS_DDF_SUCCESS;
                samples[1].sample = FX_FLTTOFIX_Q16((float)c_data * CLR_U_WATT_PER_LSB);
                samples[1].status = SNS_DDF_SUCCESS;

                sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_CT_C, samples[0].sample, (uint32_t)c_data, 0, 0, 0);
            }

            s_data->status = SNS_DDF_SUCCESS;
            s_data->timestamp = sns_ddf_get_timestamp();
            s_data->samples = samples;
        }

        return ret;
    }

    return SNS_DDF_EINVALID_PARAM;
}

/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_set_attrib: Sets certain device attributes that
are programmable. Currently only power mode is a settable attribute.
*
* @param[in] dd_handle  Handle to driver instance.
* @param[in] sensor     Sensor for which this attribute is to be set.
* @param[in] attrib     Attribute to be set.
* @param[in] value      Value to set this attribute.
*
*  @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_set_attrib
    (
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
    )
{
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    uint32_t idx = (uint32_t)dd_handle & 3;

    /* basic sanity check */
    if (dd_handle == NULL || value == NULL) 
        return SNS_DDF_EINVALID_PARAM;

    MAX44006_MSG_2(HIGH, "sns_dd_als_max44006_set_attrib - attrib=%d, idx=%d", attrib, idx);

    /* set attribute */
    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_STATE:

        return sns_dd_als_max44006_set_power_mode(state,
            *(sns_ddf_powerstate_e *)value, idx);

    case SNS_DDF_ATTRIB_RANGE:
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_ODR:
        //return SNS_DDF_EINVALID_PARAM;
        if(sensor == SNS_DDF_SENSOR_AMBIENT)
        {
            state->common.max44006_als_odr = *(sns_ddf_odr_t *)value;
        }
        else if(sensor == SNS_DDF_SENSOR_PROXIMITY) // hack uses SNS_DDF_SENSOR_PROXIMITY
        {
            state->common.max44006_dummy_odr = *(sns_ddf_odr_t *)value;
        }
        else if (sensor == SNS_DDF_SENSOR_RGB)
        {
            state->common.max44006_rgb_odr = *(sns_ddf_odr_t *)value;
        }
        else if (sensor == SNS_DDF_SENSOR_CT_C)
        {
            state->common.max44006_ct_c_odr = *(sns_ddf_odr_t *)value;
        }
        else{
            return SNS_DDF_EINVALID_ATTR;}

        return SNS_DDF_SUCCESS;
    default:
        return SNS_DDF_EINVALID_ATTR;
    }
}

/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_get_attrib: Retrieves the value of an attribute.
*
* @param[in] dd_handle   Handle to driver instance.
* @param[in] sensor      Sensor for which this attribute is to be retrieved.
* @param[in] attrib      Attribute to be retrieved.
* @param[in] memhandler  Memory handler used to dynamically allocate
output parameters, if applicable.
* @param[out] value      Pointer that this function will allocate or set
*                        to the attribute's value.
* @param[out] num_elems   Number of elements in @a value.
*
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44006_get_attrib
    (
    sns_ddf_handle_t      handle,
    sns_ddf_sensor_e      sensor,
    sns_ddf_attribute_e   attrib,
    sns_ddf_memhandler_s* memhandler,
    void**                value,
    uint32_t*             num_elems
    )
{
    sns_dd_als_rgb_state_t    *state = (sns_dd_als_rgb_state_t *)((uint32_t)handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    uint32_t                  dev_index = (uint32_t)handle & 3;
    sns_ddf_driver_info_s     *driver_info;
    sns_ddf_device_info_s     *device_info;
    sns_ddf_range_s           *device_ranges;
    sns_ddf_resolution_t      *resolution;
    sns_ddf_resolution_adc_s  *resolution_adc;
    sns_ddf_power_info_s      *power;
    sns_ddf_powerstate_e      *power_state;

    /* basic sanity check */
    if (handle == NULL || memhandler == NULL)
        return SNS_DDF_EINVALID_PARAM;

    /* basic sanity check */
    if (dev_index >= MAX44006_DD_MAX_SENSOR_TYPES)
        return SNS_DDF_EINVALID_PARAM;

    MAX44006_MSG_3(HIGH, "sns_dd_als_max44006_get_attrib - sensor=%d, attrib=%d, dev_index=%d", sensor, attrib, dev_index);
    MAX44006_MSG_2(HIGH, "rgb_id=%d als_id=%d", rgb_id, als_id);

    /* return attribute */
    switch (attrib)
    {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
        driver_info = sns_ddf_memhandler_malloc(memhandler,
            sizeof(*driver_info));
        if (driver_info == NULL)
            return SNS_DDF_ENOMEM;
        driver_info->version = 1;
        *value = driver_info;
        *num_elems = 1;

        if (dev_index == rgb_id)
        {
            driver_info->name = "MAX44006 RGB/CT_C";
        }
        else if (dev_index == als_id)
        {
            driver_info->name = "MAX44006 ALS";
        }
        else
            return SNS_DDF_EINVALID_ATTR;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        device_info = sns_ddf_memhandler_malloc(memhandler,
            sizeof(*device_info));
        if (device_info == NULL)
            return SNS_DDF_ENOMEM;

        device_info->vendor = "Maxim Integrated";
        device_info->model = "MAX44006";

        if (dev_index == rgb_id)
        {
            device_info->name = "MAX44006 RGB/CT_C";
        }
        else if (dev_index == als_id)
        {
            device_info->name = "MAX44006 ALS";
        }
        else
            return SNS_DDF_EINVALID_PARAM;

        device_info->version = 1;
        *value = device_info;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RANGE:
        device_ranges = sns_ddf_memhandler_malloc(memhandler,
            sizeof(*device_ranges));
        if (device_ranges == NULL)
            return SNS_DDF_ENOMEM;
        device_ranges->min = FX_FLTTOFIX_Q16(0);
        device_ranges->max = FX_FLTTOFIX_Q16(QUALCOMM_MAX_LUX);
        *value = device_ranges;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RESOLUTION:
        resolution = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution));
        if (resolution == NULL)
            return SNS_DDF_ENOMEM;
        *resolution = FX_FLTTOFIX_Q16(1);
        *value = resolution;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        resolution_adc = sns_ddf_memhandler_malloc(memhandler,
            sizeof(*resolution_adc));
        if (resolution_adc == NULL)
            return SNS_DDF_ENOMEM;
        resolution_adc->bit_len = 14;
        resolution_adc->max_freq = 10;  // 10Hz from 100 ms integ time
        *value = resolution_adc;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_POWER_INFO:
        power = sns_ddf_memhandler_malloc(memhandler, sizeof(*power));
        if (power == NULL)
            return SNS_DDF_ENOMEM;

        /*see the power consumption page 3/26 in Max44006 datasheet */
        power->active_current = 30;
        power->lowpower_current = 1;
        *value = power;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_POWER_STATE:
        power_state = sns_ddf_memhandler_malloc(memhandler,
            sizeof(*power_state));
        if (power_state == NULL)
            return SNS_DDF_ENOMEM;
        *power_state = state->sub_dev[dev_index].powerstate;
        *value = power_state;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_ODR:
        //return SNS_DDF_EINVALID_PARAM;
        {
            uint32_t* odr_ptr;
            if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }

            if (sensor == SNS_DDF_SENSOR_AMBIENT)
                *odr_ptr = state->common.max44006_als_odr;
            else if (sensor == SNS_DDF_SENSOR_PROXIMITY) // hack uses SNS_DDF_SENSOR_PROXIMITY
                *odr_ptr = state->common.max44006_dummy_odr;
            else if (sensor == SNS_DDF_SENSOR_RGB)
                *odr_ptr = state->common.max44006_rgb_odr;
            else if (sensor == SNS_DDF_SENSOR_CT_C)
                *odr_ptr = state->common.max44006_ct_c_odr;
            else
                return SNS_DDF_EINVALID_ATTR;

            *value = odr_ptr;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
        {
            MAX44006_MSG_0(HIGH, "SNS_DDF_ATTRIB_REGISTRY_GROUP");
            sns_ddf_registry_group_s *reg_group_ptr;
            uint8_t *reg_group_data_ptr;
            uint32_t nv_size;

            if(dev_index == als_id)
            {
              nv_size = state->nv_size;
            }
            else
            {
              nv_size = state->nv_rgb_size;
            }

            if ( (reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }

            if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, nv_size)) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }

            if(dev_index == als_id)
            {
              SNS_OS_MEMSCPY(reg_group_data_ptr, sizeof(max44006_dd_nv_param_s), &state->nv_param, sizeof(max44006_dd_nv_param_s));
            }
            else
            {
              SNS_OS_MEMSCPY(reg_group_data_ptr, sizeof(max44006_dd_rgb_nv_param_s), &state->nv_rgb_param, sizeof(max44006_dd_rgb_nv_param_s));
            }            

            reg_group_ptr->group_data = reg_group_data_ptr;
            reg_group_ptr->size = nv_size;    
            *(sns_ddf_registry_group_s**)value = reg_group_ptr;

            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

    default:
        return SNS_DDF_EINVALID_ATTR;
    }
}

/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_handle_irq: interrupt handler
*
* @detail This function will be called within the context of the Interrupt
*         Service Task (IST), *not* the ISR.
*
* @param[in] dd_handle  Handle to driver instance.
* @param[in] irq        The IRQ representing the interrupt that occured.
*
* @return None
*/
/*===========================================================================*/
static void sns_dd_als_max44006_handle_irq
    (
    sns_ddf_handle_t dd_handle,
    uint32_t irq,
    sns_ddf_time_t timestamp
    )
{

    uint8_t 				regval;
    uint8_t 				nbytes;
    sns_ddf_status_e 		ret;
    int clr_data = 0;

    sns_dd_als_rgb_state_t	*state = (sns_dd_als_rgb_state_t *)((int)dd_handle & DD_HANDLE_ALIGN_BITS);
#ifdef MAX44006_DEBUG
    uint32_t				dev_index = (int)dd_handle & 3;
#endif

    MAX44006_MSG_3(HIGH, "sns_dd_als_max44006_handle_irq idx = %d als_id=%d rgb_id=%d", dev_index, als_id, rgb_id);

    /* basic sanity check */
    if (dd_handle == NULL)
    {
        ret = SNS_DDF_EFAIL;
        return;
    }

    /* read the interrupt status register to clear the interrupt */
    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_STATUS_REG,
        &regval, 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return;
    }

    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_STATUS_REG = %d", regval);

    if (!(regval & 1)) /* Check if AMBINTS interrupt flag is not asserted*/
    {
        MAX44006_MSG_0(MED, "No irq. Exit");
        return; /* Wrong interrupt signal received, */
    }

    if ((als_id != -1) && state->sub_dev[als_id].enable)
    {
        sns_ddf_sensor_sample_s	samples;
        sns_ddf_sensor_data_s	s_data;
        int lux_data = 0;

        ret = sns_dd_als_max44006_get_lux(state,
            &lux_data,
            &clr_data);

        lux_data = (lux_data * state->nv_param.als_factor) / 100;

        MAX44006_MSG_2(MED, "lux_data=%d, clear_data=%d", lux_data, clr_data);

        if (ret == SNS_DDF_SUCCESS)
        {
            samples.status = SNS_DDF_SUCCESS;
            samples.sample = lux_data << 16;
            s_data.num_samples = 1;
            s_data.sensor = SNS_DDF_SENSOR_AMBIENT;
            s_data.status = SNS_DDF_SUCCESS;
            s_data.timestamp = sns_ddf_get_timestamp();
            s_data.samples = &samples;

            ret = sns_ddf_smgr_notify_data(state->sub_dev[als_id].smgr_handle, &s_data, 1);
            MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_data error=%d", ret);

            sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_AMBIENT, (lux_data*1000), samples.sample, clr_data, 0, 0);
        }
    }

    if((rgb_id != -1) && state->sub_dev[rgb_id].enable)
    {
        sns_ddf_sensor_sample_s	samples_rgb[3];
        sns_ddf_sensor_sample_s	samples_ct[2];
        sns_ddf_sensor_data_s	s_data[2];
        int r_data;
        int g_data;
        int b_data;
        float cct_data;

        /* Clear data */
        ret =  sns_dd_als_max44006_read_clr(state, &clr_data);
        if ( ret != SNS_DDF_SUCCESS)
        {
            return;
        }

        /* RGB data */
        ret = sns_dd_als_max44006_read_rgb(state, &r_data, &g_data, &b_data);
        if ( ret != SNS_DDF_SUCCESS)
        {
            return;
        }

        cct_data = get_cct_value(r_data, g_data, b_data);

        MAX44006_MSG_3(MED, "r=%d g=%d b=%d", r_data, g_data, b_data);

        s_data[0].sensor = SNS_DDF_SENSOR_RGB;
        s_data[0].num_samples = 3;
        // samples_rgb[0].sample = (uint32_t)r_data;
        // samples_rgb[0].status = SNS_DDF_SUCCESS;
        // samples_rgb[1].sample = (uint32_t)g_data;
        // samples_rgb[1].status = SNS_DDF_SUCCESS;
        // samples_rgb[2].sample = (uint32_t)b_data;
        // samples_rgb[2].status = SNS_DDF_SUCCESS; 

        samples_rgb[0].sample = FX_FLTTOFIX_Q16((float)r_data * RED_U_WATT_PER_LSB);
        samples_rgb[0].status = SNS_DDF_SUCCESS;
        samples_rgb[1].sample = FX_FLTTOFIX_Q16((float)g_data * GREEN_U_WATT_PER_LSB);
        samples_rgb[1].status = SNS_DDF_SUCCESS;
        samples_rgb[2].sample = FX_FLTTOFIX_Q16((float)b_data * BLUE_U_WATT_PER_LSB);
        samples_rgb[2].status = SNS_DDF_SUCCESS; 

        s_data[0].status = SNS_DDF_SUCCESS;
        s_data[0].timestamp = sns_ddf_get_timestamp();
        s_data[0].samples = samples_rgb;

        s_data[1].sensor = SNS_DDF_SENSOR_CT_C;
        s_data[1].num_samples = 2;
        samples_ct[0].sample = FX_FLTTOFIX_Q16(cct_data);
        samples_ct[0].status = SNS_DDF_SUCCESS;
        samples_ct[1].sample = FX_FLTTOFIX_Q16((float)clr_data * CLR_U_WATT_PER_LSB);
        samples_ct[1].status = SNS_DDF_SUCCESS;

        s_data[1].status = SNS_DDF_SUCCESS;
        s_data[1].timestamp = sns_ddf_get_timestamp();
        s_data[1].samples = samples_ct;

        ret = sns_ddf_smgr_notify_data(state->sub_dev[rgb_id].smgr_handle, s_data, 2);
        MAX44006_MSG_1(MED, "sns_ddf_smgr_notify_data error=%d", ret);

        sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_RGB, (uint32_t)r_data, (uint32_t)g_data, (uint32_t)b_data, 0, 0);

        sns_dd_max44006_log_data(state, SNS_DDF_SENSOR_CT_C, samples_ct[0].sample, (uint32_t)clr_data, 0, 0, 0); 

    }

    state->hi_thresh = (uint16_t)(clr_data + 0.1*clr_data);
    state->lo_thresh = (uint16_t)(clr_data - 0.1*clr_data);

    ret = sns_als_max44006_driver_update_int_thresh(state,
        state->hi_thresh,
        state->lo_thresh,
        MAX44006_AMB_THR_PST_TMR);

    MAX44006_MSG_0(MED, "handle_irq - Exit");
    return;
}


/*==========================================================================*/
/*
* @brief sns_dd_max44006_reset_chip: Resets the chip.
*
* @param[in] state    Handle to driver instance.
*
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_max44006_reset_chip
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    )
{
    uint8_t reg_val;
    uint8_t nbytes;
    sns_ddf_status_e ret;

    MAX44006_MSG_0(HIGH, "sns_dd_max44006_reset_chip - enter");

    /* basic sanity check */
    if (state == NULL)
        return SNS_DDF_EINVALID_PARAM;

    reg_val = (1 << 4);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_2(ERROR, "sns_ddf_write_port error=%d, nbytes=%d", ret, nbytes);
        return ret;
    }

    MAX44006_MSG_0(HIGH, "sns_dd_max44006_reset_chip - exit");
    return SNS_DDF_SUCCESS;
}

/*==========================================================================*/
/*
* @brief sns_dd_als_max44006_init_normal: sets the chip to normal state or defaults configuration.
*
* @param[in] state    Handle to driver instance.
*
* @return SNS_DDF_SUCCESS if successful, error code otherwise.
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_als_max44006_init_normal
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    )
{
    uint8_t reg_val;
    uint8_t nbytes;
    sns_ddf_status_e ret;

    MAX44006_MSG_1(HIGH, "sns_dd_als_max44006_init_normal - idx=%d", idx);

    /* basic sanity check */
    if (state == NULL)
        return SNS_DDF_EINVALID_PARAM;


    /* Initialize registers here for RGB, Clear, ALS, IR*/
    reg_val = 0x00; /* Status register's value */
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }

    reg_val = (MAX44006_MAIN_CFG_CL_RGB_IR | MAX44006_MAIN_CFG_AMB_INT_ENABLE_BIT); /* Status register's value */
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }		

    /* Ambient configuration */	
#if 0
    reg_val = state->nv_rgb_param.register_02;
#else
    reg_val = ((MAX44006_AMB_PGA_FACTOR_BITS & 3) << 0) |
        ((MAX44006_AMB_TIM_BITS & 7) << 2)| 
        ((MAX44006_TEMPEN_BIT & 1) << 5) |
        ((MAX44006_COMPEN_BIT & 1) << 6) |
        ((MAX44006_TRIM_BIT & 1) << 7);
#endif


    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_CFG_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }	


    /* Set ADC Gains for Clear, RGB, IR Channels */
    reg_val = state->nv_rgb_param.c_gain_trim; 
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_CLR_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }	


    reg_val = state->nv_rgb_param.r_gain_trim;
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_RED_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }	

    reg_val = state->nv_rgb_param.g_gain_trim;
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }	


    reg_val = state->nv_rgb_param.b_gain_trim;
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }		

    reg_val = state->nv_rgb_param.ir_gain_trim;
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_IR_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        return ret;
    }		



    state->sub_dev[idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;

    MAX44006_MSG_0(HIGH, "sns_dd_als_max44006_init_normal - exit");
    return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
* @brief sns_dd_als_max44006_init_lowpower: sets the chip to low power
*        state. Please note that MAX44006 power consumption even in normal state
*        is comparable to shutdown power of most parts.
*
* @param[in] state    Handle to driver instance.
*
* @return SNS_DDF_SUCCESS
*/
/*===========================================================================*/
sns_ddf_status_e sns_dd_als_max44006_init_lowpower
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    )
{
    uint8_t reg_val;
    uint8_t nbytes;
    sns_ddf_status_e ret;

    MAX44006_MSG_1(HIGH, "sns_dd_als_max44006_init_lowpower - idx=%d", idx);

    /* basic sanity check */
    if (state == NULL)
        return SNS_DDF_EINVALID_PARAM;

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port error=%d", ret);
        return ret;
    }

    reg_val |= (1 << 3);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port error=%d", ret);
        return ret;
    }
    state->sub_dev[idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    MAX44006_MSG_0(HIGH, "sns_dd_als_max44006_init_lowpower - exit");
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_read_clr
    (
    sns_dd_als_rgb_state_t* state,
    int* als_data
    )
{
    sns_ddf_status_e ret;
    uint8_t  nbytes;
    uint8_t  data_bytes[2];

    /* Read Clear data*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CLR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        return SNS_DDF_EFAIL;
    }

    *als_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *als_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "CLEAR=0x%04X", *als_data);

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_read_rgb
    (
    sns_dd_als_rgb_state_t* state,
    int* r_data, int* g_data, int* b_data
    )
{
    sns_ddf_status_e ret;
    uint8_t  nbytes;
    uint8_t  data_bytes[2];

    /* Read Red data*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_RED_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        return SNS_DDF_EFAIL;
    }

    *r_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *r_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "RED=0x%04X", *r_data);

    /* Read Green data*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        return SNS_DDF_EFAIL;
    }

    *g_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *g_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "GREEN=0x%04X", *g_data);

    /* Read Blue data*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_BLUE_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        return SNS_DDF_EFAIL;
    }

    *b_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *b_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "BLUE=0x%04X", *b_data);

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_read_ir
    (
    sns_dd_als_rgb_state_t* state,
    int* ir_data
    )
{
    sns_ddf_status_e ret;
    uint8_t  nbytes;
    uint8_t  data_bytes[2];

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_IR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {

        return SNS_DDF_EFAIL;
    }

    *ir_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    *ir_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "IR=0x%04X", *ir_data);

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_read_temp
    (
    sns_dd_als_rgb_state_t* state,
    int* temp_data
    )
{
    sns_ddf_status_e ret;
    uint8_t  nbytes;
    uint8_t  data_bytes[2];

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_TEMP_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {

        return SNS_DDF_EFAIL;
    }

    *temp_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *temp_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "TEMP=0x%04X", *temp_data);

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_read_ir_comp
    (
    sns_dd_als_rgb_state_t* state,
    int* ir_comp_data
    )
{
    sns_ddf_status_e ret;
    uint8_t  nbytes;
    uint8_t  data_bytes[2];

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_COMP_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {

        return SNS_DDF_EFAIL;
    }

    *ir_comp_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    *ir_comp_data &= 0x3FFF;

    MAX44006_MSG_1(HIGH, "IR_COMP=0x%04X", *ir_comp_data);

    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_dd_als_max44006_get_lux
    (
    sns_dd_als_rgb_state_t* state,
    int *lux_val,
    int *clr_data
    )
{

    sns_ddf_status_e ret;
    uint8_t  nbytes;
    int ir_val;
    int ir_comp_val;
    int clr_val;
    int normalized_clr_cts;
    int normalized_comp_cts;
    //const uint32_t ampga_factor[] = {1, 4, 16, 256};
    uint32_t ampga_factor_const = MAX44006_AMB_PGA_FACTOR_BITS & 3;
    uint32_t green_data;
    float ir_green_ratio;
    float light_source_factor = 0.0;
    uint8_t  data_bytes[2];
    //uint16_t arr_index = 0;

    MAX44006_MSG_0(HIGH, "sns_dd_als_max44006_get_lux - enter");

    ret = sns_dd_als_max44006_read_ir(state, &ir_val);
    if (ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }

    ret = sns_dd_als_max44006_read_ir_comp(state, &ir_comp_val);
    if (ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }

    ret = sns_dd_als_max44006_read_clr(state, &clr_val);
    if (ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }

#ifdef MAX44006_DEBUG

    // read AMBGA
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CFG_REG,
        (uint8_t *)&data_bytes[0], 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        return SNS_DDF_EFAIL;
    }

    // read MAIN CFG
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        (uint8_t *)&data_bytes[0], 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        return SNS_DDF_EFAIL;
    }

    MAX44006_MSG_1(HIGH, "SNS_DD_MAX44006_MAIN_CFG=0x%02X", data_bytes[0]);

    // read AMB CFG
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CFG_REG,
        (uint8_t *)&data_bytes[0], 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        return SNS_DDF_EFAIL;
    }

    MAX44006_MSG_1(HIGH, "SNS_DD_MAX44006_AMB_CFG_REG=0x%02X", data_bytes[0]);

#endif

    /* **** Note that this algorithm assumes AMBGA = 0x02 *****/

    *clr_data = clr_val;
    //arr_index = MAX44006_AMB_PGA_FACTOR_BITS & 3;
    // normalized_clr_cts = clr_val * ampga_factor[arr_index];
    normalized_clr_cts = clr_val; /* if AMBPGA is 2 */
    //arr_index = MAX44006_AMB_PGA_FACTOR_BITS & 3;
    // normalized_comp_cts = ir_comp_val * ampga_factor[arr_index];
    normalized_comp_cts = ir_comp_val; /* if AMBPGA is 2 */

    // ampga_factor_const = (ampga_factor_const == 3) ? 1 : 2;
    ampga_factor_const = 2;

    /* Read Green data*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        return SNS_DDF_EFAIL;
    }

    green_data = (uint32_t)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);

    green_data &= 0x3FFF;	

    MAX44006_MSG_1(HIGH, "GREEN=%04X", green_data);
    MAX44006_MSG_2(HIGH, "normalized_clr_cts=%d    %04X", normalized_clr_cts, normalized_clr_cts);
    MAX44006_MSG_2(HIGH, "normalized_comp_cts=%d   %04X", normalized_comp_cts, normalized_comp_cts);	

    ir_green_ratio = (float)ir_val/(float)green_data;

    /*************************************************************************************************************
    Under Incandescent lamp, the IR will pick a lots of  IR photons and the counts will be very high 
    or saturated (around 16K). Under Fluorescent lamp ,
    the IR ch will be very less(since Fluorescent lamp will not have much of IR). Let’s say the visible lux 
    for both case is 2K counts .

    So under Incandescent lamp ratio of IR to clear could be 16K/5K>>1
    Under Fluorescent lamp ratio of IR to clear could be say 1K/5K <<1
    **************************************************************************************************************/

    if (ir_green_ratio > 1.0)
    {
        MAX44006_MSG_0(MED, "ir_green_ratio > 1.0 case");
        light_source_factor = 0.0584; // under Incandescent lamp ratio 
    }
    else if (ir_green_ratio <= 1.0)
    {
        MAX44006_MSG_0(MED, "ir_green_ratio <= 1.0");
        light_source_factor = 0.166; // under Fluorescent lamp ratio 
    }

    /* Returns calibrated value */
    *lux_val = (int32_t)((float)(normalized_clr_cts - ampga_factor_const*normalized_comp_cts)*(float)light_source_factor);

    if(*lux_val < 0)
    {
        *lux_val = 1000;
    }

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_als_max44006_driver_update_int_thresh
    (
    sns_dd_als_rgb_state_t* state,
    uint16_t hi_tresh_val,
    uint16_t lo_tresh_val,
    uint8_t amb_pst_tmr
    )
{
    sns_ddf_status_e ret;
    uint8_t reg_val;
    uint8_t nbytes;

    MAX44006_MSG_3(HIGH, "sns_als_max44006_driver_update_int_thresh hi=%d, lo=%d, pst_tmr=%d", hi_tresh_val, lo_tresh_val, amb_pst_tmr);

    reg_val = (uint8_t)(hi_tresh_val >> 8);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_UPTHR_HI_REG,
        &reg_val, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    reg_val = (uint8_t)(hi_tresh_val & 0xFF);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_UPTHR_LO_REG,
        &reg_val, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    reg_val =  (uint8_t)(lo_tresh_val >> 8);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_LOTHR_HI_REG,
        &reg_val, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    reg_val = (uint8_t)(lo_tresh_val & 0xFF);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_LOTHR_LO_REG,
        &reg_val, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    reg_val = amb_pst_tmr;
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_THR_PST_TMR_REG,
        &reg_val, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    MAX44006_MSG_0(HIGH, "sns_als_max44006_driver_update_int_thresh - exit");

    return SNS_DDF_SUCCESS;

}


sns_ddf_status_e sns_dd_max44006_enable_interrupt
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    )
{
    sns_ddf_status_e ret;
    uint8_t regval;
    uint8_t nbytes;
    uint8_t data_byte[2];

    MAX44006_MSG_2(HIGH, "sns_dd_max44006_enable_interrupt - idx=%d, gpio=%d", idx, state->sub_dev[idx].irq);

    // return error if DRI is not enabled for this sub device
    if(!state->sub_dev[idx].dri_enable)
    {
        MAX44006_MSG_0(ERROR, "DRI is not enabled for this subdevice");
        return SNS_DDF_EINVALID_PARAM;
    }

    // Update threshold registers to guarantee an interrupt
    state->hi_thresh = MAX44006_AMB_TRESH_HI;
    state->lo_thresh = MAX44006_AMB_TRESH_LO;	

    ret = sns_als_max44006_driver_update_int_thresh(state,
        state->hi_thresh,
        state->lo_thresh,
        MAX44006_AMB_THR_PST_TMR);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    MAX44006_MSG_2(HIGH, "enable=%d, dri_enable=%d", idx, state->sub_dev[idx].irq);
    // check if interrupt is already enabled
    if(((als_id != -1) && state->sub_dev[als_id].enable && state->sub_dev[als_id].dri_enable)
        ||
        ((rgb_id != -1) && state->sub_dev[rgb_id].enable && state->sub_dev[rgb_id].dri_enable))
    {
        MAX44006_MSG_0(HIGH, "Interrupt already enabled");
        state->sub_dev[idx].enable = true;
        return SNS_DDF_SUCCESS;
    }	

    /* read the interrupt status register to clear the interrupt */
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &regval, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_1(HIGH, "before SNS_DD_MAX44006_MAIN_CFG = 0x%02X", regval);

    regval |= MAX44006_MAIN_CFG_AMB_INT_ENABLE_BIT; /* Set AMBINTE bit*/

    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &regval, 1, &nbytes);

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &regval, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_1(HIGH, "after SNS_DD_MAX44006_MAIN_CFG = 0x%02X", regval);

    /* read the interrupt status register to clear the interrupt */
    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_STATUS_REG,
        &regval, 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_1(HIGH, "after SNS_DD_MAX44006_STATUS_REG = 0x%02X", regval);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_UPTHR_HI_REG,
        (uint8_t *)&data_byte[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (2 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_2(HIGH, "after SNS_DD_MAX44006_AMB_UPTHR_HI_REG = 0x%02X%02X", data_byte[0], data_byte[1]);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_LOTHR_HI_REG,
        (uint8_t *)&data_byte[0], 2, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (2 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_2(HIGH, "after SNS_DD_MAX44006_AMB_LOTHR_HI_REG = 0x%02X%02X", data_byte[0], data_byte[1]);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_THR_PST_TMR_REG,
        (uint8_t *)&data_byte[0], 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_1(HIGH, "after SNS_DD_MAX44006_AMB_THR_PST_TMR_REG = 0x%02X", data_byte[0]);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_CFG_REG,
        (uint8_t *)&data_byte[0], 1, &nbytes);

    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }

    MAX44006_MSG_1(HIGH, "after SNS_DD_MAX44006_AMB_CFG_REG = 0x%02X", data_byte[0]);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    ret = sns_ddf_signal_register(
        state->sub_dev[idx].irq,
        (sns_ddf_handle_t)((uint32_t)state|idx),
        &sns_als_rgb_max44006_driver_if,
        SNS_DDF_SIGNAL_IRQ_FALLING);
    if (ret != SNS_DDF_SUCCESS)
        return ret;

    state->sub_dev[idx].enable = true;

    MAX44006_MSG_0(HIGH, "sns_dd_max44006_enable_interrupt - exit");
    return ret;
}
/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_disable_interrupt
    (
    sns_dd_als_rgb_state_t* state,
    uint32_t idx
    )
{
    sns_ddf_status_e ret;
    uint8_t regval;
    uint8_t nbytes;

    state->sub_dev[idx].enable = false;

    // make sure we don't disable the interrupt if another sub device is enabled
    if(idx == als_id)
    {
        if((rgb_id != -1) && state->sub_dev[rgb_id].enable)
        {
            // RGB is active
            MAX44006_MSG_0(HIGH, "RGB is active");
            return SNS_DDF_SUCCESS;
        }
    }
    else if(idx == rgb_id)
    {
        if((als_id != -1) && state->sub_dev[als_id].enable)
        {
            // ALS is active
            MAX44006_MSG_0(HIGH, "ALS is active");
            return SNS_DDF_SUCCESS;
        }
    }

    /* read the interrupt status register to clear the interrupt */
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &regval, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        return ret;
    }
    regval &= ~(1 << 0); /* Clear AMBINTE bit*/

    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &regval, 1, &nbytes);

    if (ret != SNS_DDF_SUCCESS)
        return ret;

    ret = sns_ddf_signal_deregister(state->sub_dev[idx].irq);
    if (ret != SNS_DDF_SUCCESS)
        return ret;

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_als_max44006_enable_sched_data
    (
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
    sns_ddf_status_e      	ret;
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    uint32_t                dev_index = (uint32_t)dd_handle & 3;

    MAX44006_MSG_2(HIGH, "sns_dd_als_max44006_enable_sched_data idx=%d, sensor=%d", dev_index, sensor);

    if (enable)
    {
        ret = sns_dd_max44006_enable_interrupt(state, dev_index);
    }
    else
    {
        ret = sns_dd_max44006_disable_interrupt(state, dev_index);
    }	
    return ret;

    return SNS_DDF_EINVALID_PARAM;
}

sns_ddf_status_e sns_dd_als_max44006_connectivity_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  )
{
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    uint32_t                dev_index = (uint32_t)dd_handle & 3;
    sns_ddf_status_e  ret = SNS_DDF_SUCCESS;
    uint8_t reg_val = 0;
    uint8_t nbytes = 0;

    MAX44006_MSG_2(HIGH, "sns_dd_als_max44006_connectivity_test test=%d, sensor=%d", test, sensor);

    if(state == NULL)
    {
       return SNS_DDF_EINVALID_PARAM;
    }

    if(test != SNS_DDF_TEST_CONNECTIVITY)
    {
       return SNS_DDF_EINVALID_TEST;
    }

    ret = sns_dd_max44006_reset_chip(state, dev_index);
    if (ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }
    /*IMPORTANT delay, from part reset to normal operation. Do not remove this delay*/
    sns_ddf_delay(10000);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_CFG_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port - error=%d", ret);
        return ret;
    }
    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_AMB_CFG_REG read = 0x%X",reg_val);

    /*POR value of amb cfg register must be 0x20*/
    if(reg_val != 0x20)
    {
        MAX44006_MSG_0(ERROR, "Unexpected POR value of amb cfg register");
        return SNS_DDF_EFAIL;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_oem_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  )
{
    sns_ddf_status_e status;
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    //uint32_t                dev_index = (uint32_t)dd_handle & 3;
    MAX44006_MSG_2(HIGH, "sns_dd_max44006_oem_test - test=%d, sensor=%d",test, sensor);

    /* basic sanity check */
    if (state == NULL)
    {
    	MAX44006_MSG_0(ERROR, "sns_dd_max44006_oem_test, state is NULL");
        return SNS_DDF_EINVALID_PARAM;
    }

    if (sensor != SNS_DDF_SENSOR_RGB)
    {
    	MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test is for SNS_DDF_SENSOR_RGB, not sensor type %d",sensor);
        return SNS_DDF_EINVALID_PARAM;
    }

	if (rgb_id == -1)
    {
    	MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test invalid rgb_id, %d",rgb_id);
        return SNS_DDF_EINVALID_PARAM;
    }
        
    if (state->oem_test.running)
    {
        return SNS_DDF_EDEVICE_BUSY;
    }

    if (!sns_max_44006_check_reg02_val(state->nv_rgb_param.register_02))
    {
        MAX44006_MSG_0(ERROR, "sns_dd_max44006_oem_test - need rgb registry values to be correct");
        return SNS_DDF_EINVALID_PARAM;
    }

    if (!state->nv_rgb_param.data_from_registry)
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test - need rgb registry to be populated");
	
    /* first oem test, indicate appropriately so timer can act accordingly*/
    state->oem_test.running = true;
    state->oem_test.curtest = MAX44006_OEM_TEST_NONE;

    /* init and fire timer immediately and return pending */
    if (!state->oem_test.test_timer_inited)
    {
        status = sns_ddf_timer_init(&state->oem_test.test_timer,dd_handle,&sns_als_rgb_max44006_driver_if,dd_handle,false);
        if (status != SNS_DDF_SUCCESS)
        {
            MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test - timer init error %d",status);
            goto fail;
        }
        state->oem_test.test_timer_inited = true;
    }
    status = sns_ddf_timer_start(state->oem_test.test_timer,1000); // 1ms start up time 
    if (status != SNS_DDF_SUCCESS) 
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test - timer start error %d",status);
        goto fail;
    }
    
    return SNS_DDF_PENDING;
    
    fail:
        state->oem_test.running = false;
        return SNS_DDF_EFAIL;

}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_als_max44006_run_test(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
  )
{
    sns_ddf_status_e status;
    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    //uint32_t                dev_index = (uint32_t)dd_handle & 3;

    MAX44006_MSG_2(HIGH, "sns_dd_als_max44006_run_test test=%d, sensor=%d", test, sensor);

    if(state == NULL)
    {
       MAX44006_MSG_0(ERROR, "sns_dd_als_max44006_run_test SNS_DDF_EINVALID_PARAM");
       return SNS_DDF_EINVALID_PARAM;
    }

    switch (test)
    {
    case SNS_DDF_TEST_CONNECTIVITY:
         status = sns_dd_als_max44006_connectivity_test(dd_handle, sensor, test, err);
         break;

    case SNS_DDF_TEST_OEM:
        status = sns_dd_max44006_oem_test(dd_handle, sensor, test, err);
        break;

    default:
        status = SNS_DDF_EINVALID_TEST;
    }

    return status;
}


/*===========================================================================
===========================================================================*/
void sns_dd_max44006_handle_timer(
  sns_ddf_handle_t dd_handle, 
  void* arg
)
{
    sns_ddf_status_e ret;
    bool done;

    sns_dd_als_rgb_state_t* state = (sns_dd_als_rgb_state_t *)((uint32_t)dd_handle & (uint32_t)DD_HANDLE_ALIGN_BITS);
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_handle_timer test %d", state->oem_test.curtest);

    done = false;
    
    /* execute the test */
    /* NOTE: 	Tests execute in a certain order */
    /* 			Order is - none, low gain check, high gain check, clr, r, g, b, ir*/
    switch(state->oem_test.curtest)
    {
    case MAX44006_OEM_TEST_NONE:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_none(state,arg))
            goto fail;
        break;

    case MAX44006_OEM_TEST_TRIM_GAIN_LOW_TEST:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_low_gain_check(state,arg))
            goto fail;
        break;

    case MAX44006_OEM_TEST_TRIM_GAIN_HIGH_TEST:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_high_gain_check(state,arg))
            goto fail;	
        break;

    case MAX44006_OEM_TEST_TRIM_GAIN_CLEAR:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_gain_trim(state,
                                SNS_DD_MAX44006_AMB_CLR_HI_REG,
                                state->nv_rgb_param.c_low_thresh,
                                state->nv_rgb_param.c_high_thresh,arg,&done))
            goto fail;
        break;

    case MAX44006_OEM_TEST_TRIM_GAIN_RED:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_gain_trim(state,
                            SNS_DD_MAX44006_AMB_RED_HI_REG,
                            state->nv_rgb_param.r_low_thresh,
                            state->nv_rgb_param.r_high_thresh,arg,&done))
        goto fail;
    break;
    
    case MAX44006_OEM_TEST_TRIM_GAIN_GREEN:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_gain_trim(state,
                                SNS_DD_MAX44006_AMB_GREEN_HI_REG,
                                state->nv_rgb_param.g_low_thresh,
                                state->nv_rgb_param.g_high_thresh,arg,&done))
            goto fail;
        break;
        
    case MAX44006_OEM_TEST_TRIM_GAIN_BLUE:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_gain_trim(state,
                                SNS_DD_MAX44006_AMB_BLUE_HI_REG,
                                state->nv_rgb_param.b_low_thresh,
                                state->nv_rgb_param.b_high_thresh,arg,&done))
            goto fail;
        break;

    case MAX44006_OEM_TEST_TRIM_GAIN_IR:
        if (SNS_DDF_SUCCESS != sns_dd_max44006_oem_test_gain_trim(state,
                                SNS_DD_MAX44006_AMB_IR_HI_REG,
                                state->nv_rgb_param.ir_low_thresh,
                                state->nv_rgb_param.ir_high_thresh,arg,&done))
            goto fail;
        break;
 default:
    MAX44006_MSG_1(ERROR, "sns_dd_max44006_handle_timer, invalid test %d", state->oem_test.curtest);
             sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    MAX44006_OEM_TEST_FAIL);
    goto fail;
    break;

    }
    
    /* start timer for next test */
    if (done)
    {
  MAX44006_MSG_0(ERROR, "sns_dd_max44006_handle_timer done flag is set");
        state->oem_test.running = false;
        state->oem_test.curtest = MAX44006_OEM_TEST_NONE;
        /*TODO: write operational values (values for normal operation prior to oem test,  back to all the registers*/
        return;
    }
    /*not done yet, start timer again*/
    MAX44006_MSG_0(ERROR, "sns_dd_max44006_handle_timer done flag is not set");
    ret = sns_ddf_timer_start(state->oem_test.test_timer,state->oem_test.timer_val_microsec);
    if (ret != SNS_DDF_SUCCESS) 
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_handle_timer - timer start error %d",ret);
        goto fail;
    }
    MAX44006_MSG_0(ERROR, "sns_dd_max44006_handle_timer done flag is not set, started timer again");
    
    return;

fail:
    MAX44006_MSG_0(ERROR, "sns_dd_max44006_handle_timer, fail");
    state->oem_test.running = false;
    state->oem_test.curtest = MAX44006_OEM_TEST_NONE;
    /*TODO: write operational values (values for normal operation prior to oem test,  back to all the registers*/	
    return;
    
    
}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_oem_test_none(
  sns_dd_als_rgb_state_t* state, 
  void* arg
)
{
    uint32 amb_tim = 0;
    float amb_tim_millisec = 100;
    uint8_t reg_val;
    uint8_t nbytes;
    sns_ddf_status_e ret;
    max44006_dd_oem_test_ret_e oem_test_ret = MAX44006_OEM_TEST_FAIL;

    MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none");

    //sanity check of low and high gain thresholds
    if ((state->nv_rgb_param.c_low_thresh > state->nv_rgb_param.c_high_thresh) ||
            (state->nv_rgb_param.r_low_thresh > state->nv_rgb_param.r_high_thresh) ||
            (state->nv_rgb_param.g_low_thresh > state->nv_rgb_param.g_high_thresh) ||
            (state->nv_rgb_param.b_low_thresh > state->nv_rgb_param.b_high_thresh) ||
            (state->nv_rgb_param.ir_low_thresh > state->nv_rgb_param.ir_high_thresh))
    {
        MAX44006_MSG_0(ERROR, "sns_dd_max44006_oem_test_none - incorrect low/high thresholds nv params"
            "low threshold should not be higher than high threshold");
        goto fail;
    }

    amb_tim = (state->nv_rgb_param.register_02 >> MAX44006_AMB_TIM_BITS_SHIFT)
              & MAX44006_AMB_TIM_BITS_MASK;

    switch (amb_tim)
    {
    case 4:
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none, amb_tim 400ms");
        amb_tim_millisec = 400;
        break;
    case 3:
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none, amb_tim 1.5625ms");
        amb_tim_millisec = 1.6; /*1.5625*/
        break;
    case 2:
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none, amb_tim 6.25ms");
        amb_tim_millisec = 6.3; /*6.25*/
        break;
    case 1:
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none, amb_tim 25ms");
        amb_tim_millisec = 25;
        break;
    case 0:
    default:
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_none, amb_tim 100ms");
        amb_tim_millisec = 100;
        break;
    }

    ret = sns_dd_max44006_reset_chip(state, rgb_id);
    if (ret != SNS_DDF_SUCCESS)
    {
        goto fail;
    }
    /*IMPORTANT delay, from part reset to normal operation. Do not remove this delay*/
    sns_ddf_delay(10000);
    ret = sns_dd_als_max44006_init_normal(state, rgb_id);
    if (ret != SNS_DDF_SUCCESS)
    {
        goto fail;
    }

   /* read the interrupt status register to clear the interrupt */
    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port - error=%d", ret);
        oem_test_ret = MAX44006_OEM_TEST_I2C_FAIL;
        goto fail;
    }
    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_STATUS_REG = 0x%X",reg_val);

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_STATUS_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port - error=%d", ret);
        oem_test_ret = MAX44006_OEM_TEST_I2C_FAIL;
        goto fail;
    }
    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_STATUS_REG second read = 0x%X",reg_val);

    reg_val = MAX44006_MAIN_CFG_CL_RGB_IR; /* Status register's value */
    MAX44006_MSG_1(HIGH, "SNS_DD_MAX44006_MAIN_CFG = 0x%X",reg_val);
    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_MAIN_CFG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_write_port - error=%d", ret);
        oem_test_ret = MAX44006_OEM_TEST_I2C_FAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_MAIN_CFG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port - error=%d", ret);
        oem_test_ret = MAX44006_OEM_TEST_I2C_FAIL;
        goto fail;
    }
    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_MAIN_CFG read = 0x%X",reg_val);

    state->oem_test.timer_val_microsec = (amb_tim_millisec * 2 + 1) * MICROSECS_IN_MILLISEC;
    MAX44006_MSG_1(HIGH, "timer = %d",state->oem_test.timer_val_microsec);

    /* set register 02 - Ambient configuration */ 
    reg_val = (state->nv_rgb_param.register_02 | MAX44006_AMB_TRIM_BIT_ON);
    MAX44006_MSG_1(HIGH, "Setting SNS_DD_MAX44006_AMB_CFG_REG = 0x%X",reg_val);


    ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_AMB_CFG_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_handle_timer - i2c write error=%d",ret);
        goto fail;
    }
    ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44006_AMB_CFG_REG,
        &reg_val, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    {
        MAX44006_MSG_1(ERROR, "sns_ddf_read_port - error=%d", ret);
        oem_test_ret = MAX44006_OEM_TEST_I2C_FAIL;
        goto fail;
    }
    MAX44006_MSG_1(MED, "SNS_DD_MAX44006_AMB_CFG_REG read = 0x%X",reg_val);


    /*gain trim low threshold check- write zeros to gain registers*/
    ret = sns_dd_max44006_write_all_gain_regs(state,MAX44006_GAIN_TRIM_MIN);
    if (ret != SNS_DDF_SUCCESS) 
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs - error=%d", ret);
        goto fail;
    }
    state->oem_test.curtest = MAX44006_OEM_TEST_TRIM_GAIN_LOW_TEST;
    /*IMPORTANT delay, for new settings to affect part output. Do not remove this delay*/
    sns_ddf_delay(210000);
    return SNS_DDF_SUCCESS;
fail:
    sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    oem_test_ret);	
    return SNS_DDF_EFAIL;

}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_oem_test_low_gain_check(
    sns_dd_als_rgb_state_t* state, 
    void* arg
)
{
    uint8_t nbytes;
    uint32_t data;
    sns_ddf_status_e ret;
    uint8_t  data_bytes[2];
#ifdef MAX44006_LOG_ALL_REG_IN_OEM_TEST	
    uint32_t cdata, rdata, gdata, bdata, irdata; //for debug purposes
    /*read all  channels - for debug only*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CLR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
        "- reading clr data err %d bytes %d",ret, nbytes);
    }
    else
    {
        cdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        cdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
            "CLEAR=0x%04X", cdata);
    }
    
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_RED_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
        "- reading red data err %d bytes %d",ret, nbytes);
    }
    else
    {
        rdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        rdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
            "RED=0x%04X", rdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
        "- reading green data err %d bytes %d",ret, nbytes);
    }
    else
    {
        gdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        gdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
            "GREEN=0x%04X", gdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_BLUE_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
        "- reading blue data err %d bytes %d",ret, nbytes);
    }
    else
    {
        bdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        bdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
            "BLUE=0x%04X", bdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_IR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
        "- reading ir data err %d bytes %d",ret, nbytes);
    }
    else
    {
        irdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        irdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
            "IR=0x%04X", irdata);
    }	
#endif


    MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_low_gain_check");
    /*see if adc values are greater  than  the lower threshold value, even with zero gain trim*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CLR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- reading clr data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
        "CLEAR=0x%04X", data);
    if (data > state->nv_rgb_param.c_low_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- zero gain clr data read %u low threshold %u",data, 
            state->nv_rgb_param.c_low_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_RED_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- reading red data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check RED=0x%04X", data);
    if (data > state->nv_rgb_param.r_low_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- zero gain red data read %u low threshold %u",data, 
            state->nv_rgb_param.r_low_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- reading green data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
        "GREEN=0x%04X", data);
    if (data > state->nv_rgb_param.g_low_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- zero gain green data read %u low threshold %u",data, 
            state->nv_rgb_param.g_low_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_BLUE_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- reading blue data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check BLUE=0x%04X", data);
    if (data > state->nv_rgb_param.b_low_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- zero gain blue data read %u low threshold %u",data, 
            state->nv_rgb_param.b_low_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }	

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_IR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- reading ir data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_low_gain_check"
        "IR=0x%04X", data);
    if (data > state->nv_rgb_param.ir_low_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- zero gain ir data read %u low threshold %u",data, 
            state->nv_rgb_param.ir_low_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    /*start gain trim high threshold check- write 0x3F to gain registers*/
    ret = sns_dd_max44006_write_all_gain_regs(state,MAX44006_GAIN_TRIM_MAX);
    if (ret != SNS_DDF_SUCCESS) 
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_low_gain_check"
            "- could not write max gain, error =%d",ret);
        goto fail;
    }
    state->oem_test.curtest = MAX44006_OEM_TEST_TRIM_GAIN_HIGH_TEST;
    return SNS_DDF_SUCCESS;

fail:
 sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    MAX44006_OEM_TEST_FAIL);
    return ret;

}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_oem_test_high_gain_check(
  sns_dd_als_rgb_state_t* state, 
  void* arg
)
{	
    uint8_t nbytes;
    uint32_t data;
    sns_ddf_status_e ret;
    uint8_t  data_bytes[2];

    MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_high_gain_check");
#ifdef MAX44006_LOG_ALL_REG_IN_OEM_TEST	
    uint32_t cdata, rdata, gdata, bdata, irdata; //for debug purposes
    /*read all  channels - for debug only*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CLR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
        "- reading clr data err %d bytes %d",ret, nbytes);
    }
    else
    {
        cdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        cdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
            "CLEAR=0x%04X", cdata);
    }
    
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_RED_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
        "- reading red data err %d bytes %d",ret, nbytes);
    }
    else
    {
        rdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        rdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
            "RED=0x%04X", rdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
        "- reading green data err %d bytes %d",ret, nbytes);
    }
    else
    {
        gdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        gdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
            "GREEN=0x%04X", gdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_BLUE_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
        "- reading blue data err %d bytes %d",ret, nbytes);
    }
    else
    {
        bdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        bdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
            "BLUE=0x%04X", bdata);
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_IR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
        "- reading ir data err %d bytes %d",ret, nbytes);
    }
    else
    {
        irdata = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
        irdata &= 0x3FFF;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
            "IR=0x%04X", irdata);
    }	
#endif	
    /*see if adc values are lesser  than the higher threshold value, even with max gain trim*/
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_CLR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- reading clr data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
        "CLEAR=0x%04X", data);
    if (data < state->nv_rgb_param.c_high_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- zero gain clr data read %u hi threshold %u",data, 
            state->nv_rgb_param.c_high_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_RED_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- reading red data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
        "RED=0x%04X", data);
    if (data < state->nv_rgb_param.r_high_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- zero gain red data read %u hi threshold %u",data, 
            state->nv_rgb_param.r_high_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_GREEN_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- reading green data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
        "GREEN=0x%04X", data);
    if (data < state->nv_rgb_param.g_high_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- zero gain green data read %u hi threshold %u",data, 
            state->nv_rgb_param.g_high_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_BLUE_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- reading blue data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check BLUE=0x%04X", data);
    if (data < state->nv_rgb_param.b_high_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- zero gain blue data read %u hi threshold %u",data, 
            state->nv_rgb_param.b_high_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }	

    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44006_AMB_IR_HI_REG,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- reading ir data err %d bytes %d",ret, nbytes);
        goto fail;
    }
    data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    data &= 0x3FFF;
    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_high_gain_check"
        "IR=0x%04X", data);
    if (data < state->nv_rgb_param.ir_high_thresh)
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- zero gain ir data read %u hi threshold %u",data, 
            state->nv_rgb_param.ir_high_thresh);
        ret = SNS_DDF_EFAIL;
        goto fail;
    }

    /*start gain trim tests, write mid gain to all regs */
    ret = sns_dd_max44006_write_all_gain_regs(state,MAX44006_GAIN_TRIM_BIT6);
    if (ret != SNS_DDF_SUCCESS) 
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_high_gain_check"
            "- could not write mid gain, error =%d",ret);
        goto fail;
    }
    state->oem_test.curtest = MAX44006_OEM_TEST_TRIM_GAIN_CLEAR;
    return SNS_DDF_SUCCESS;

fail:
 sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    MAX44006_OEM_TEST_FAIL);
    return ret;


}
/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_check_threshold(
    sns_dd_als_rgb_state_t* state,
    uint16_t amb_reg_addr,uint32_t low_thresh, uint32_t hi_thresh,
    uint32_t * amb_data,
    max44006_dd_amb_data_check_e * result
)
{
    uint8_t nbytes;
    sns_ddf_status_e ret;
    uint8_t  data_bytes[2];

    MAX44006_MSG_1(HIGH, "sns_dd_max44006_check_threshold"
        ", reg addr 0x%X", amb_reg_addr);
    /* read adc values */
    ret = sns_ddf_read_port(state->port_handle, amb_reg_addr,
        (uint8_t *)&data_bytes[0], 2, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2))
    {
        MAX44006_MSG_2(ERROR, "sns_dd_max44006_check_threshold"
            "- reading data err %d addr %d",ret, amb_reg_addr);
        goto fail;
    }
    *amb_data = (int)((uint16_t)data_bytes[0] << 8 |  (uint16_t)data_bytes[1]);
    (*amb_data) &= 0x3FFF;
    MAX44006_MSG_2(HIGH, "sns_dd_max44006_check_threshold"
        "addr0x%x = 0x%04X", amb_reg_addr, *amb_data);
    MAX44006_MSG_3(HIGH, "max44006 data %u, low %u, high %u", *amb_data, low_thresh, hi_thresh);

    if ( (*amb_data <= hi_thresh) && (*amb_data >= low_thresh ) )
        *result =  MAX44006_AMB_DATA_WITHIN_THRESHOLDS;
    else if (*amb_data > hi_thresh)
        *result = MAX44006_AMB_DATA_ABOVE_UPPER_THRESHOLD;
    else
        *result = MAX44006_AMB_DATA_BELOW_LOWER_THRESHOLD;

    return SNS_DDF_SUCCESS;

fail:
    return ret;

}
    

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_save_data_from_part_to_state(sns_dd_als_rgb_state_t* state,
                                    uint16_t amb_reg_addr, uint32_t amb_data, uint8_t trim_value)
{
    
    MAX44006_MSG_0(HIGH, "sns_dd_max44006_save_data_from_part_to_state");
    switch (amb_reg_addr)
    {
    case SNS_DD_MAX44006_AMB_CLR_HI_REG:
        state->nv_rgb_param.c_gain_trim = trim_value;
        state->nv_rgb_param.c_trim_amb_value = amb_data; 
        break;
    case SNS_DD_MAX44006_AMB_RED_HI_REG:
        state->nv_rgb_param.r_gain_trim = trim_value;
        state->nv_rgb_param.r_trim_amb_value = amb_data;
        break;
    case SNS_DD_MAX44006_AMB_GREEN_HI_REG:
        state->nv_rgb_param.g_gain_trim = trim_value;
        state->nv_rgb_param.g_trim_amb_value = amb_data;
        break;
    case SNS_DD_MAX44006_AMB_BLUE_HI_REG:
        state->nv_rgb_param.b_gain_trim = trim_value;
        state->nv_rgb_param.b_trim_amb_value = amb_data;
        break;
    case SNS_DD_MAX44006_AMB_IR_HI_REG:
        state->nv_rgb_param.ir_gain_trim = trim_value;
        state->nv_rgb_param.ir_trim_amb_value = amb_data;
        break;		
    default:
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_save_data_from_part_to_state"
            ", invalid reg addr 0x%x",amb_reg_addr);
        goto fail;
        break;
    }

    return SNS_DDF_SUCCESS;
    
fail:
    return SNS_DDF_EFAIL;
}


/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_oem_test_gain_trim(
  sns_dd_als_rgb_state_t* state, uint16_t amb_reg_addr, 
  uint32_t low_thresh, uint32_t hi_thresh,
  void* arg, bool * done
)
{
    uint8_t nbytes;
    sns_ddf_status_e ret;
    max44006_dd_amb_data_check_e result = MAX44006_AMB_DATA_NOT_CHECKED;
    uint32_t amb_data = 0;
    uint16_t trim_reg_addr;
    uint8_t trim_value;
    uint8_t only_least_sig_bit_set;
    uint8_t least_sig_bit_clear_mask;
    

    MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim, reg addr %d",amb_reg_addr);
    *done = false;

    //read current trim value
    switch (amb_reg_addr)
    {
    case SNS_DD_MAX44006_AMB_CLR_HI_REG:
        trim_reg_addr = SNS_DD_MAX44006_TRIM_GAIN_CLR_REG;
     low_thresh = state->nv_rgb_param.c_low_thresh;
     hi_thresh = state->nv_rgb_param.c_high_thresh;
        break;
    case SNS_DD_MAX44006_AMB_RED_HI_REG:
        trim_reg_addr = SNS_DD_MAX44006_TRIM_GAIN_RED_REG;
     low_thresh = state->nv_rgb_param.r_low_thresh;
     hi_thresh = state->nv_rgb_param.r_high_thresh;
        break;
    case SNS_DD_MAX44006_AMB_GREEN_HI_REG:
        trim_reg_addr = SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG;
     low_thresh = state->nv_rgb_param.g_low_thresh;
     hi_thresh = state->nv_rgb_param.g_high_thresh;
        break;
    case SNS_DD_MAX44006_AMB_BLUE_HI_REG:
        trim_reg_addr = SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG;
     low_thresh = state->nv_rgb_param.b_low_thresh;
     hi_thresh = state->nv_rgb_param.b_high_thresh;
        break;
    case SNS_DD_MAX44006_AMB_IR_HI_REG:
        trim_reg_addr = SNS_DD_MAX44006_TRIM_GAIN_IR_REG;
     low_thresh = state->nv_rgb_param.ir_low_thresh;
     hi_thresh = state->nv_rgb_param.ir_high_thresh;
        break;		
    default:
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_gain_trim"
            ", invalid reg addr 0x%x",amb_reg_addr);
        goto fail;
        break;		
    }
    ret = sns_ddf_read_port(state->port_handle, trim_reg_addr,
        &trim_value, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    {
        
        MAX44006_MSG_3(ERROR, "sns_dd_max44006_oem_test_gain_trim, "
                              "could not read trim reg addr 0x%x, error %d, nbytes %d",
                                    trim_reg_addr, ret, nbytes);
        goto fail;
    }
    trim_value &= MAX44006_GAIN_TRIM_MASK;
    
    //check thresholds
    ret = sns_dd_max44006_check_threshold(state, amb_reg_addr,
                                            low_thresh,
                                            hi_thresh,
                                            &amb_data,
                                            &result);
    if (ret != SNS_DDF_SUCCESS)
    {
        MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_gain_trim"
            "- sns_dd_max44006_check_threshold returned %d", ret);
        goto fail;
    }
    if (result == MAX44006_AMB_DATA_WITHIN_THRESHOLDS)
    {
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim - "
                            "MAX44006_AMB_DATA_WITHIN_THRESHOLDS trim_value=0x%X", trim_value);
        //save trim values and amb_data to register structure
        ret = sns_dd_max44006_save_data_from_part_to_state(state, amb_reg_addr, amb_data, trim_value);
        if (ret != SNS_DDF_SUCCESS)
        {
            MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_gain_trim"
                "- sns_dd_max44006_save_data_from_part_to_state returned %d", ret);
            goto fail;
        }

        if (amb_reg_addr == SNS_DD_MAX44006_AMB_IR_HI_REG)
        {
            /*turn TRIM bit on in registry, for use in normal mode*/
            state->nv_rgb_param.register_02 |= MAX44006_AMB_TRIM_BIT_ON;
            ret = sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_SUCCESS,
                                                    0);
            /* if the completion notification itself failed, give a message*/
            if ( ret != SNS_DDF_SUCCESS)
            {
                MAX44006_MSG_1(ERROR, "sns_dd_max44006_oem_test_gain_trim"
                    "- test complete notification error=%d",ret);
            }

            sns_ddf_delay(100000);

            //done, post message to save to registry, post test complete
            ret = sns_ddf_smgr_notify_event(state->sub_dev[rgb_id].smgr_handle, 
                                                        SNS_DDF_SENSOR_RGB,
                                                        SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
            MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim - notify to update registry"
                "returned %d, ", ret);
            
            MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "test completed successfully");
            *done = true;
   return SNS_DDF_SUCCESS;
        }
        else
        {
            //move to next test
            MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "sub-test %d completed successfully, next test starting", state->oem_test.curtest);
            state->oem_test.curtest = (max44006_dd_oem_test_e)((int)(state->oem_test.curtest + 1));
   MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim - test %d", state->oem_test.curtest);
            return SNS_DDF_SUCCESS;
        }
    }
    else if (result == MAX44006_AMB_DATA_ABOVE_UPPER_THRESHOLD)
    {
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_gain_trim - "
                            "MAX44006_AMB_DATA_ABOVE_UPPER_THRESHOLD");
        /*if bit 0 is set, and we got a value above upper thresold, then test has failed.
        this is because, the only way to get here with last bit set is : we got a value less than lower threshold, 
        with bit1 set. Then we set bit0, to try and get a value within the threshold, but we got a value above the threshold.
        */
        
        if (trim_value & MAX44006_GAIN_TRIM_BIT0)
        {
            MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "test complete with error");
            ret = sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    MAX44006_OEM_TEST_THRESHOLD_MATCH_FAIL);
            goto fail;
        }

        //get bit mask with only the least signicant bit that is 1, set
        //NOTE, we can only get here with at most MAX44006_GAIN_TRIM_BIT1 set,
        //due to (trim_value & MAX44006_GAIN_TRIM_BIT0) check above 
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "trim value %d",trim_value );
        only_least_sig_bit_set = (trim_value) & ~(trim_value - 1);

        //turn off the least significat bit set, and turn on the next least significant bit
        least_sig_bit_clear_mask = ~only_least_sig_bit_set;
        trim_value &= least_sig_bit_clear_mask;
        only_least_sig_bit_set >>= 1;
        trim_value |= only_least_sig_bit_set;
        trim_value &= MAX44006_GAIN_TRIM_MASK;
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "new trim value %d",trim_value );
        
        //save new trim value
        ret = sns_ddf_write_port(state->port_handle, trim_reg_addr,
            &trim_value, 1, &nbytes);
        if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
        {
            MAX44006_MSG_3(ERROR,
                            "sns_dd_max44006_oem_test_gain_trim"
                            "- new trim value, reg addr 0x%x, sns_ddf_write_port error=%d, nbytes %d",
                            trim_reg_addr, ret, nbytes);
            goto fail;
        }

        //stay with current test, repeat test with new trim value
        return SNS_DDF_SUCCESS;
    
    }
    else /*result == MAX44006_AMB_DATA_BELOW_LOWER_THRESHOLD*/
    {
        MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_gain_trim - "
                            "MAX44006_AMB_DATA_BELOW_LOWER_THRESHOLD");

        /*if bit 0 is set, and we got a value above upper thresold, then test has failed.
        this is because, the only way to get here with last bit set is : we got a value less than lower threshold, 
        with bit1 set. Then we set bit0, to try and get a value within the threshold, but we got a value below the threshold again.
        */
        
        if (trim_value & MAX44006_GAIN_TRIM_BIT0)
        {
            MAX44006_MSG_0(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "test complete with error");
            ret = sns_ddf_smgr_notify_test_complete(state->sub_dev[rgb_id].smgr_handle,
                                                    SNS_DDF_SENSOR_RGB,
                                                    SNS_DDF_EFAIL,
                                                    MAX44006_OEM_TEST_THRESHOLD_MATCH_FAIL);
            goto fail;
        }
        //get bit mask with only the least signicant bit that is 1, set
        //NOTE, we can only get here with at most MAX44006_GAIN_TRIM_BIT1 set,
        //due to (trim_value & MAX44006_GAIN_TRIM_BIT0) check above 
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "trim value %d",trim_value );
        only_least_sig_bit_set = (trim_value) & ~(trim_value -1);

        //turn on next bit as well
        only_least_sig_bit_set >>= 1;
        trim_value |= only_least_sig_bit_set;
        trim_value &= MAX44006_GAIN_TRIM_MASK;		
        MAX44006_MSG_1(HIGH, "sns_dd_max44006_oem_test_gain_trim -"
                                "new trim value %d",trim_value );
        ret = sns_ddf_write_port(state->port_handle, trim_reg_addr,
            &trim_value, 1, &nbytes);
        if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
        {
            MAX44006_MSG_3(ERROR,
                            "sns_dd_max44006_oem_test_gain_trim"
                            "- new trim value, reg addr 0x%x, sns_ddf_write_port error=%d, nbytes %d",
                            trim_reg_addr, ret, nbytes);
            goto fail;
        }

        //stay with current test, repeat test with new trim value
        return SNS_DDF_SUCCESS;
    }
    
fail:
    return SNS_DDF_EFAIL;

}

/*===========================================================================
===========================================================================*/
sns_ddf_status_e sns_dd_max44006_write_all_gain_regs(
  sns_dd_als_rgb_state_t* state,
  uint8_t gain
)

{

    uint8_t reg_val;
    uint8_t nbytes;
    sns_ddf_status_e ret;


    MAX44006_MSG_1(HIGH, "sns_dd_max44006_write_all_gain_regs gain=%d",gain);

    /* basic sanity check */
    if (state == NULL)
        return SNS_DDF_EINVALID_PARAM;


    /* Initialize registers here for RGB, Clear, IR*/
       reg_val = gain; 
       ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_CLR_REG,
          &reg_val, 1, &nbytes);
       if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
       {
          MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs"
             "- sns_ddf_write_port error=%d", ret);
          goto done;
       }

       ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_RED_REG,
          &reg_val, 1, &nbytes);
       if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
       {
          MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs - sns_ddf_write_port error=%d", ret);
          goto done;
       }

       ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG,
          &reg_val, 1, &nbytes);
       if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
       {
          MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs - sns_ddf_write_port error=%d", ret);
          goto done;
       }


       ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG,
          &reg_val, 1, &nbytes);
       if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
       {
          MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs - sns_ddf_write_port error=%d", ret);
          goto done;
       }

       ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44006_TRIM_GAIN_IR_REG,
          &reg_val, 1, &nbytes);
       if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
       {
          MAX44006_MSG_1(ERROR, "sns_dd_max44006_write_all_gain_regs - sns_ddf_write_port error=%d", ret);
          goto done;
       }

done:
       return ret;

}


/*===========================================================================
===========================================================================*/

sns_ddf_driver_if_s sns_als_rgb_max44006_driver_if =
{
    .init = &sns_dd_als_max44006_init,
    .get_data = &sns_dd_als_max44006_get_data,
    .set_attrib = &sns_dd_als_max44006_set_attrib,
    .get_attrib = &sns_dd_als_max44006_get_attrib,
    .handle_timer = &sns_dd_max44006_handle_timer,
    .handle_irq = &sns_dd_als_max44006_handle_irq,
    .reset = &sns_dd_als_max44006_reset,
    .run_test = &sns_dd_als_max44006_run_test,
    .enable_sched_data = &sns_dd_als_max44006_enable_sched_data,
    // &sns_ddf_max44006_probe,
    .probe = NULL
};

/*===========================================================================
===========================================================================*/
