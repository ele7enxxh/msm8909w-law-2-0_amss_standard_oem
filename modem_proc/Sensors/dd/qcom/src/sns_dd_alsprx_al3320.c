/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the AL3320 ALS(Light) and Proximity Sensor

Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_al3320.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/22/14     MW      Eliminate compiler warning
09/23/13     yh      Initial version of driver for AL3320
==============================================================================*/
/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_al3320_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_memmgr.h"

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_alsprx_al3320b_init
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

static sns_ddf_status_e sns_dd_alsprx_al3320b_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_al3320b_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_al3320b_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

void sns_dd_alsprx_al3320b_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_alsprx_al3320b_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_alsprx_al3320b_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);

sns_ddf_status_e sns_dd_alsprx_al3320b_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );

static sns_ddf_status_e sns_dd_alsprx_al3320b_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );

sns_ddf_driver_if_s sns_alsprx_al3320b_driver_fn_list =
{
  .init = &sns_dd_alsprx_al3320b_init,
  .get_data = &sns_dd_alsprx_al3320b_get_data,
  .set_attrib = &sns_dd_alsprx_al3320b_set_attrib,
  .get_attrib = &sns_dd_alsprx_al3320b_get_attrib,
  .handle_timer = NULL,
  .handle_irq = &sns_dd_alsprx_al3320b_handle_irq,
  .reset = &sns_dd_alsprx_al3320b_reset,
  .run_test = &sns_dd_alsprx_al3320b_run_test,
  .enable_sched_data = &sns_dd_alsprx_al3320b_enable_sched_data,
  .probe = sns_dd_alsprx_al3320b_probe
};

sns_ddf_status_e sns_dd_al3320b_als_sensor_samples(sns_dd_alsprx_state_t   *state,
                                            sns_ddf_sensor_data_s   *data_ptr,
                                            sns_ddf_memhandler_s    *memhandler,
                                            sns_ddf_sensor_sample_s *sample_data);

void sns_dd_al3320b_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);

sns_ddf_status_e sns_dd_al3320b_get_mlux(sns_dd_alsprx_state_t* state);

sns_ddf_status_e sns_dd_al3320b_get_als_data(sns_dd_alsprx_state_t* state);

/*===========================================================================
FUNCTION      sns_dd_al3320b_write_byte

DESCRIPTION   write a byte from device
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                reg,
                                byte_val,
                                1,
                                &write_count
                                );
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_write_word

DESCRIPTION   write a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                reg,
                                &word_val->byte[0],
                                2,
                                &write_count
                                );
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_read_byte

DESCRIPTION   read a byte from device
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;

    status = sns_ddf_read_port(port_handle,
                               reg,
                               byte_val,
                               1,
                               &read_count
                               );
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_read_word

DESCRIPTION   read a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;
    uword_u          word_tmp;

    status = sns_ddf_read_port(port_handle,
                               reg,
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

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_log

DESCRIPTION   Log the latest sensor data
===========================================================================*/
void sns_dd_alsprx_al3320b_log(
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

     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
       dd_ptr->dropped_logs++;
   }
#endif
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_enable_sched_data

DESCRIPTION   enable schedule data
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_al3320b_enable_sched_data(sns_ddf_handle_t dd_handle,
                                                         sns_ddf_sensor_e sensor,
                                                         bool             enable
                                                         )
{
    sns_dd_alsprx_state_t* state = dd_handle;
    uint8_t byteVal;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    byteVal = AL3320B_INT_CLR_MSK;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_FLAG_STAT, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    if(enable){
        byteVal = AL3320B_INT_ENA_MSK;
            status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_INT_CON, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
    }
    else {
        byteVal = AL3320B_INT_DIS_MSK;
            status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_INT_CON, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
    }

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_set_als_thresh

DESCRIPTION   set als threshold
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_al3320b_set_als_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint32_t alsdata, t_low, t_high;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    alsdata   = state->chip.data.als.data;

    t_low = alsdata * AL3320B_ALS_INT_L;   /*set to 20% as default*/
    t_high = alsdata * AL3320B_ALS_INT_H;   /*set to 20% as default*/
    if (t_high > AL3320B_ALS_INT_FULL)
        t_high = AL3320B_ALS_INT_FULL;

    wordVal.word = t_low;
    status = sns_dd_al3320b_write_word(state->port_handle, AL3320B_ALS_LTHREL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ){
        DDF_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
        return status;
    }

    wordVal.word = t_high;
    status = sns_dd_al3320b_write_word(state->port_handle, AL3320B_ALS_HTHREL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ){
        DDF_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
        return status;
    }

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_read_status_reg

DESCRIPTION   get data and report
===========================================================================*/
void sns_dd_al3320b_read_status_reg(sns_dd_alsprx_state_t* state, sns_ddf_time_t timestamp, AL3320B_SIG_TYPE sig_type)
{
    uint8_t i=0;
    uint8_t byteVal;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    /*Read ALS data*/
    sns_dd_al3320b_get_als_data(state);
    sns_dd_alsprx_al3320b_set_als_thresh(state); /* set als threshold to be +/- 20% of current value for next interrupt.*/
    /* compute LUX*/
    sns_dd_al3320b_get_mlux(state);

    /* Had at least one interrupt*/
    if ((sns_ddf_malloc((void **)&sensor_data, sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
    {
        /* Failed miserably. Can't even notify SMGR */
        return;
    }

    sensor_data[i].sensor    = SNS_DDF_SENSOR_AMBIENT;
    sensor_data[i].status    = SNS_DDF_SUCCESS;
    sensor_data[i].timestamp = timestamp;

     if((sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
        {
            /* Failed miserably. Can't even notify SMGR */
            return;
        }

    sns_dd_al3320b_als_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);

    sns_ddf_smgr_notify_data(state->smgr_handle, sensor_data, 1);
    sns_ddf_mfree(buf_ptr);
    sns_ddf_mfree(sensor_data);

    byteVal = AL3320B_INT_CLR_MSK;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_FLAG_STAT, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return;
    }
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_handle_irq

DESCRIPTION   irq handle
===========================================================================*/
void sns_dd_alsprx_al3320b_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;
    sns_dd_al3320b_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_set_powerstate

DESCRIPTION   set power state
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_al3320b_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uint8_t          i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE ){
    state->chip.setup.reg.enable |= AL3320B_ENABLE_MSK_AEN;
    i2c_data = state->chip.setup.reg.enable;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_SYSTEM_CON, &i2c_data);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    i2c_data = AL3320B_PWD_MSK_AEN;
    status   = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_SYSTEM_CON, &i2c_data);
    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }
    state->chip.setup.als.odr  = 0;
  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;
  return status;
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_update_registers

DESCRIPTION   update registers
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;
    uword_u wordVal;

    //lsc_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    //setup  = &state->chip.setup;

    /* Reset the device*/
    /* Power off*/
    byteVal = 0x00;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_SYSTEM_CON, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    wordVal.word = state->chip.setup.als.thresh_low;
    status = sns_dd_al3320b_write_word(state->port_handle, AL3320B_ALS_LTHREL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    wordVal.word = state->chip.setup.als.thresh_high;
    status = sns_dd_al3320b_write_word(state->port_handle, AL3320B_ALS_HTHREL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    byteVal = AL3320B_MEAN_TIME;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_ALS_MTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Clear any pending interrupts*/
    byteVal = AL3320B_INT_CLR_MSK;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_FLAG_STAT, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    byteVal = AL3320B_DEFAULT_RANGE;
       status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_ALS_CON, &byteVal);
       if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    byteVal = AL3320B_INT_DIS_MSK;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_INT_CON, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Enable Power on */
    byteVal = state->chip.setup.reg.enable;
    status = sns_dd_al3320b_write_byte(state->port_handle, AL3320B_SYSTEM_CON, &byteVal);

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_reset

DESCRIPTION   reset als sensor
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_al3320b_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status;

    /* Reset driver state*/
    sns_dd_al3320b_set_default_reg_values(state);
    status = sns_dd_al3320b_update_registers(state);
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_set_attrib

DESCRIPTION   set attrib
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_al3320b_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

  if ( (dd_handle == NULL) ||
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
        status = sns_dd_alsprx_al3320b_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
        if(status != SNS_DDF_SUCCESS){
             return status;
        }
        break;

    case SNS_DDF_ATTRIB_ODR:
        state->chip.setup.als.odr = *(uint32_t*)value;
        break;

    case SNS_DDF_ATTRIB_RANGE:
        /*Noop, just return success*/
        return SNS_DDF_SUCCESS;

    default:
        return SNS_DDF_EINVALID_PARAM;
  }

  return(status);
}
/*===========================================================================
FUNCTION      sns_dd_al3320b_get_mlux

DESCRIPTION   get mlux
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_get_mlux(sns_dd_alsprx_state_t* state)
{
    uint8_t readByte;
    uint16_t als_data;
    uint32_t als_lux;
    uint32_t als_mlux;

    als_data = state->chip.data.als.data;
    sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_CON, &readByte);

    switch(readByte)
    {
        case AL3320B_RANGE_1X1:
            als_data *= AL3320B_RANGE_1X1_RES;
            break;

        case AL3320B_RANGE_1X2:
            als_data *= AL3320B_RANGE_1X2_RES;
            break;

        case AL3320B_RANGE_1X3:
            als_data *= AL3320B_RANGE_1X3_RES;
            break;

        case AL3320B_RANGE_1X4:
            als_data *= AL3320B_RANGE_1X4_RES;
            break;

        case AL3320B_RANGE_3X1:
            als_data *= AL3320B_RANGE_3X1_RES;
            break;

        case AL3320B_RANGE_3X2:
            als_data *= AL3320B_RANGE_3X2_RES;
            break;

        case AL3320B_RANGE_3X3:
            als_data *= AL3320B_RANGE_3X3_RES;
            break;

        case AL3320B_RANGE_3X4:
            als_data *= AL3320B_RANGE_3X4_RES;
            break;

        default:
            return SNS_DDF_EINVALID_DATA;
    }

    als_lux = als_data;
    als_mlux = als_lux * 1000;

    state->chip.data.als.lux = als_lux;
    state->chip.data.als.mlux = als_mlux;
    state->chip.data.als.lux_ok = true;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_get_als_data

DESCRIPTION   get als data from device
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_get_als_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e      status;
    uword_u alsdata;

    status = sns_dd_al3320b_read_word(state->port_handle,
                               AL3320B_ALS_DATAL,
                               &alsdata);

    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }
    state->chip.data.als.data = alsdata.word;

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_als_sensor_samples

DESCRIPTION   als report handle
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_als_sensor_samples(sns_dd_alsprx_state_t    *state,
                                            sns_ddf_sensor_data_s    *data_ptr,
                                            sns_ddf_memhandler_s     *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = 2;

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(
                                                      memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.als.lux, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.als.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    sns_dd_alsprx_al3320b_log(state, SNS_DDF_SENSOR_AMBIENT,
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_get_data

DESCRIPTION   get als data
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_al3320b_get_data
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

    if((status = sns_dd_al3320b_get_als_data(state))!= SNS_DDF_SUCCESS)
    {
        return status;
    }

    if((status = sns_dd_al3320b_get_mlux(state))!= SNS_DDF_SUCCESS)
    {
        return status;
    }

    timestamp = sns_ddf_get_timestamp();
    data_ptr  = sns_ddf_memhandler_malloc(
                                         memhandler,
                                         num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(data_ptr == NULL)
    {
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    for(i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor    = sensors[i];
        data_ptr[i].status    = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_AMBIENT:
            sns_dd_al3320b_als_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;
        default:
            break;
        }
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_set_default_reg_values

DESCRIPTION   set default parameter
===========================================================================*/
void sns_dd_al3320b_set_default_reg_values(sns_dd_alsprx_state_t* state)
{
    state->chip.setup.reg.enable      = AL3320B_PWD_MSK_AEN;

    state->chip.setup.als.odr         = AL3320B_ALS_F_ODR;

    state->chip.setup.als.thresh_low  = AL3320B_ALS_THR_LOW_F;
    state->chip.setup.als.thresh_high = AL3320B_ALS_THR_HIG_F;
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_set_reg_values_from_nv

DESCRIPTION   set default parameter from nv
===========================================================================*/
void sns_dd_al3320b_set_reg_values_from_nv(sns_dd_alsprx_state_t* state)
{
    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high != 0)
    {

      state->chip.setup.als.thresh_high = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high;
    }

    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low != 0)
    {

      state->chip.setup.als.thresh_low = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low;
    }
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_compute_lux_ave

DESCRIPTION   compute lux average
===========================================================================*/
static sns_ddf_status_e sns_dd_al3320b_compute_lux_ave(sns_dd_alsprx_state_t* state)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status;

    luxSum = 0;

    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        status = sns_dd_al3320b_get_als_data(state);
        if( status != SNS_DDF_SUCCESS )
        {
        DDF_MSG_0(HIGH, "failed in get als data ");
        }
        status = sns_dd_al3320b_get_mlux(state);
        if( status != SNS_DDF_SUCCESS )
        {
        DDF_MSG_0(HIGH, "failed in app get mlux ");
        }
        luxSum += state->chip.data.als.lux;
        DDF_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        sns_ddf_delay(25000);
    }

    state->chip.data.als.luxAve = (luxSum / NUM_OF_LUX_TO_AVE);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_al3320b_calibrate_als

DESCRIPTION   set calibrate als
===========================================================================*/
sns_ddf_status_e sns_dd_al3320b_calibrate_als(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    uint32_t  tgtLux,avgLux,tgtLuxLowRange, tgtLuxHiRange;
    avgLux=1;
    tgtLux=1;

    if ((status = sns_dd_al3320b_compute_lux_ave(state)) != SNS_DDF_SUCCESS)
    {
     return status;
    }

     tgtLux = (state->sns_dd_alsprx_common_db.nv_db.calLuxLower + state->sns_dd_alsprx_common_db.nv_db.calLuxUpper) / 2;
     tgtLuxLowRange = 0.1*tgtLux;
     tgtLuxHiRange = 10*tgtLux;
     avgLux = state->chip.data.als.luxAve;

    if ((avgLux <= tgtLuxLowRange) || (avgLux >= tgtLuxHiRange))
     {
      return SNS_DDF_EFAIL;
     }

     tgtLux = FX_FLTTOFIX_Q16(tgtLux);
     avgLux = FX_FLTTOFIX_Q16(avgLux);

     state->sns_dd_alsprx_common_db.nv_db.luxCorrection = FX_DIV_Q16(tgtLux,avgLux);

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_run_test

DESCRIPTION   sensor self test
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_al3320b_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t       smgr_handle;
    uint8_t readByte;

        if(test == SNS_DDF_TEST_SELF)
        {
                status = sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_CAL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_LTHREL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_LTHREH, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_HTHREL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_al3320b_read_byte(state->port_handle, AL3320B_ALS_HTHREH, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;

            smgr_handle = state->smgr_handle;
            if(state->sns_dd_alsprx_common_db.nv_db.calibratePhone)
            {
                status = sns_dd_alsprx_al3320b_reset(state);

                if( status != SNS_DDF_SUCCESS )
                {
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AL3320B_ALS_CAL_FAILED_RST);
                    return status;
                }

                state->chip.setup.als.odr = 5;

                /*  enable sensor */
                status = sns_dd_alsprx_al3320b_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);
                if( status != SNS_DDF_SUCCESS )
                {
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AL3320B_ALS_CAL_FAILED_ENB);
                    return status;
                }
                sns_ddf_delay(125000);
                status = sns_dd_al3320b_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AL3320B_ALS_CAL_FAILED_LUX);
                    return status;
                }
                state->sns_dd_alsprx_common_db.nv_db.visible_ratio = 1;
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AL3320B_ALS_CAL_FAILED_REG);
                    return status;
                }
            }
        }
        else
        {
           return SNS_DDF_EINVALID_TEST;
        }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_al3320b_probe

DESCRIPTION   sensor probe
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_al3320b_probe
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
                              AL3320B_ALS_CAL,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }

  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR__NONE;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
    sns_ddf_close_port( port_handle );
  return status;
}
