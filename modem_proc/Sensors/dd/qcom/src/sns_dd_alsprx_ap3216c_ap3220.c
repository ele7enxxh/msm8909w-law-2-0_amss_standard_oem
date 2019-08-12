/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the ap3216c ALS(Light) and Proximity Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_ap3216c_ap3220.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/18/13     MW      Fix KW issues
09/09/13     ag      Fix compile error due to debug messages 
08/09/13     yh      Initial version of driver for AP3216C 
==============================================================================*/
/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_ap3216c_ap3220_priv.h"
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
extern sns_ddf_status_e sns_dd_alsprx_ap3216c_init
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

static sns_ddf_status_e sns_dd_alsprx_ap3216c_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_ap3216c_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_ap3216c_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

void sns_dd_alsprx_ap3216c_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_alsprx_ap3216c_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_alsprx_ap3216c_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);
sns_ddf_status_e sns_dd_alsprx_ap3216c_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );
 
 static sns_ddf_status_e sns_dd_alsprx_ap3216c_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );
  
sns_ddf_driver_if_s sns_alsprx_ap3216c_driver_fn_list =
{
  .init = &sns_dd_alsprx_ap3216c_init,
  .get_data = &sns_dd_alsprx_ap3216c_get_data,
  .set_attrib = &sns_dd_alsprx_ap3216c_set_attrib,
  .get_attrib = &sns_dd_alsprx_ap3216c_get_attrib,
  .handle_timer = NULL,
  .handle_irq = &sns_dd_alsprx_ap3216c_handle_irq,
  .reset = &sns_dd_alsprx_ap3216c_reset,
  .run_test = &sns_dd_alsprx_ap3216c_run_test,  
  .enable_sched_data = &sns_dd_alsprx_ap3216c_enable_sched_data,
  .probe = &sns_dd_alsprx_ap3216c_probe
};

sns_ddf_status_e sns_dd_ap3216c_als_sensor_samples(sns_dd_alsprx_state_t   *state,
                                            sns_ddf_sensor_data_s   *data_ptr,
                                            sns_ddf_memhandler_s    *memhandler,
                                            sns_ddf_sensor_sample_s *sample_data);

sns_ddf_status_e sns_dd_ap3216c_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data);

void sns_dd_ap3216c_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);

sns_ddf_status_e sns_dd_ap3216c_get_mlux(sns_dd_alsprx_state_t* state);

sns_ddf_status_e sns_dd_ap3216c_get_als_data(sns_dd_alsprx_state_t* state);
sns_ddf_status_e sns_dd_ap3216c_get_prox_data(sns_dd_alsprx_state_t* state);

/*===========================================================================
FUNCTION      sns_dd_ap3216c_write_byte

DESCRIPTION   write a byte from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
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
FUNCTION      sns_dd_ap3216c_write_word

DESCRIPTION   write a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
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
FUNCTION      sns_dd_ap3216c_read_byte

DESCRIPTION   read a byte from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
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
FUNCTION      sns_dd_ap3216c_read_word

DESCRIPTION   read a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
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
FUNCTION      sns_dd_alsprx_ap3216c_log

DESCRIPTION   Log the latest sensor data
===========================================================================*/
void sns_dd_alsprx_ap3216c_log(
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

   DDF_MSG_0(HIGH, "AP3216C Log Data");

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
       DDF_MSG_1(HIGH, "AP3216C Log Data - logpkt_malloc failed with err: %d", err_code);
       dd_ptr->dropped_logs++;
   }
#endif
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_enable_sched_data

DESCRIPTION   enable schedule data
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_enable_sched_data(sns_ddf_handle_t dd_handle,
                                                         sns_ddf_sensor_e sensor,
                                                         bool             enable
                                                         )
{
    sns_dd_alsprx_state_t* state = dd_handle;
    uint8_t byteVal;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    byteVal = SNS_CLR_ALL_INT;
    status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_INT_STATUS, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
          DDF_MSG_0(HIGH, "ALSPRX clear interrupt err");
          return status;
        }

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_prx_binary

DESCRIPTION   ensure far or near now
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_prx_binary
(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t pdata;
    uint16_t near_thresh;
    uint16_t far_thresh;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    pdata = state->chip.data.prox.data;
    DDF_MSG_1(MED, "pdata %d",  pdata );
    near_thresh = state->chip.setup.prox.thresh_high;
    DDF_MSG_1(MED, "near_thresh %d",  near_thresh );
    far_thresh = state->chip.setup.prox.thresh_low;
    DDF_MSG_1(MED, "far_thresh %d",  far_thresh );

      if(pdata > near_thresh)
      {
        state->chip.data.prox.event = SNS_PRX_NEAR_BY;
      }
      else if(pdata < far_thresh)
      {
        state->chip.data.prox.event = SNS_PRX_FAR_AWAY;
      }
      
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_set_als_thresh

DESCRIPTION   set als threshold
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_set_als_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint32_t alsdata, t_low, t_high;
    uword_u wordVal,readALSTh;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    DDF_MSG_1(MED, "ALS High Threshold %d",  state->chip.setup.als.thresh_high );
    DDF_MSG_1(MED, "ALS Low Threshold %d",  state->chip.setup.als.thresh_low);

    alsdata   = state->chip.data.als.data;
    DDF_MSG_1(MED, "alsdata %d",  alsdata );

    if(alsdata < SNS_ALS_INT_CNT_L) alsdata = SNS_ALS_INT_CNT_L;
    t_low = alsdata - SNS_ALS_INT_CNT_L;   /*set to 10 as default*/

    DDF_MSG_1(MED, "als t low %d",  t_low );

    if(alsdata > SNS_ALS_INT_CNT_H ) alsdata = SNS_ALS_INT_CNT_H;
    t_high = alsdata + SNS_ALS_INT_CNT_L;   /*set to 10 as default*/

    DDF_MSG_1(MED, "als t high %d",  t_high);

    wordVal.word = t_low; 
    DDF_MSG_1(MED, "als t low wordval.word %d",  wordVal.word);
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_ALS_LTHL, &wordVal);
        if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
              return status;
           }
    status = sns_dd_ap3216c_read_word(state->port_handle, AP3216C_ALS_LTHL, &readALSTh);
    DDF_MSG_1(MED, "als t low wordval.word readback %d",  readALSTh.word); 
        if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low readback error %d", readALSTh.word);
              return status;
           }

    wordVal.word = t_high; 
    DDF_MSG_1(MED, "als t high wordval.word %d",  wordVal.word);
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_ALS_HTHL, &wordVal);
       if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
             return status;
          }
    status = sns_dd_ap3216c_read_word(state->port_handle, AP3216C_ALS_HTHL, &readALSTh);
    DDF_MSG_1(MED, "als t high wordval.word readback %d",  readALSTh.word);
       if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS thresh high readback error %d", readALSTh.word);
             return status;
          }

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_ap3216c_read_status_reg

DESCRIPTION   get data and report
===========================================================================*/
void sns_dd_ap3216c_read_status_reg(sns_dd_alsprx_state_t* state, sns_ddf_time_t timestamp, AP3216C_SIG_TYPE sig_type)
{
    uint8_t  statusReg;
    uint8_t  numOfSensors;
    uint8_t  i;

    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    sns_ddf_sensor_e sensors[2];
    numOfSensors  = 0;

    sns_dd_ap3216c_read_byte(state->port_handle,
                      AP3216C_INT_STATUS,
                      &statusReg);
    DDF_MSG_1(MED, "AP3216C in read_status_reg  status_reg is: %d" , statusReg);

   if(statusReg & AP3216C_STA_PRX_INTR)
    {
        /* PROX interrupt */
        DDF_MSG_1(MED, "AP3216C in prox interrupt (prox int reg): %d", AP3216C_STA_PRX_INTR);
        sns_dd_ap3216c_get_prox_data(state);
        sns_dd_alsprx_ap3216c_prx_binary(state); /* set near or far */

        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DDF_MSG_1(MED, "AP3216C out of prox data and prx_binary set state: %d", 1115);
    }

    if(statusReg & AP3216C_STA_ALS_INTR)
    {
        /* ALS interrupt */
        DDF_MSG_1(MED, "AP3216C ALS Interrupt  (int reg): %d", AP3216C_STA_ALS_INTR);

         /*Read ALS data*/
            sns_dd_ap3216c_get_als_data(state);
            sns_dd_alsprx_ap3216c_set_als_thresh(state); /* set als threshold to be +/- 20% of current value for next interrupt.*/
            /* compute LUX*/
            sns_dd_ap3216c_get_mlux(state);
            sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
            numOfSensors++;
            DDF_MSG_1(MED, "AP3216C ALS Interrupt, data read: %d", AP3216C_STA_ALS_INTR);

        DDF_MSG_2(MED, "AP3216C ALS int numOfSensors: %d lux: %d", numOfSensors, state->chip.data.als.lux);
    }

    if(numOfSensors > 0)
    {
        /* Had at least one interrupt*/
        if ((sns_ddf_malloc((void **)&sensor_data, numOfSensors*sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        {
            DDF_MSG_1(MED, "AP3216C var1: %d", 1116);
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
                DDF_MSG_1(MED, "AP3216C var1: %d", 1117);
                if((sns_ddf_malloc((void **)&buf_ptr, 2 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "AP3216C cannot notify smgr prx: %d", 1118);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                sns_dd_ap3216c_prox_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            case SNS_DDF_SENSOR_AMBIENT:
                DDF_MSG_1(MED, "AP3216C var1: %d", 1119);
                if((sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "AP3216C cannot notify smgr als: %d", 1120);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                sns_dd_ap3216c_als_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;
            default:
                 DDF_MSG_1(MED, "No data available: %d", 1119);
            return;
            }
        }

        sns_ddf_smgr_notify_data(state->smgr_handle, sensor_data, numOfSensors);

        sns_ddf_mfree(buf_ptr);
        sns_ddf_mfree(sensor_data);
    }
    DDF_MSG_1(MED, "AP3216C out of read status reg: %d", 1123);
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_handle_irq

DESCRIPTION   irq handle
===========================================================================*/
void sns_dd_alsprx_ap3216c_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;
    DDF_MSG_1(MED, "AP3216C in handle irq: %d", 1124);
    sns_dd_ap3216c_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_common_init

DESCRIPTION   initializes als and ps
===========================================================================*/
void sns_dd_alsprx_ap3216c_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;

  if (dd_ptr->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
  }
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_init_data_struct

DESCRIPTION   initialize data structures for ALS and PRX
===========================================================================*/
void sns_dd_alsprx_ap3216c_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_alsprx_ap3216c_common_init(dd_ptr);
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_set_powerstate

DESCRIPTION   set power state
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uint8_t          i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

        DDF_MSG_1(MED, "AP3216C var1: %d", 1126);
  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          DDF_MSG_1(MED, "ONLY PROX", 1126);
          state->chip.setup.reg.enable |= AP3216Cx_ENABLE_MSK_PEN;
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          DDF_MSG_1(MED, "ONLY ALS", 1126);
          state->chip.setup.reg.enable |= AP3216Cx_ENABLE_MSK_AEN;
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)
      //DDF_MSG_1(MED, "CHECK IF RE-ENABLE", 1126);
      {
          state->chip.setup.reg.enable |= AP3216Cx_ENABLE_MSK_PEN;
          state->chip.setup.reg.enable |= AP3216Cx_ENABLE_MSK_AEN;
          DDF_MSG_1(MED, "AP3216C var1: %d", 5555);
      }

      i2c_data = state->chip.setup.reg.enable;
      status   = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_MODE_COMMAND, &i2c_data);
      if ( status != SNS_DDF_SUCCESS ) {
          DDF_MSG_1(MED, "AP3216C var1: %d", 1127);
          return status;
      }
      DDF_MSG_2(MED, "AP3216C var1: %d var2: %d", 1128, i2c_data);
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_alsprx_ap3216c_init_data_struct(state);
    }

    i2c_data = SNS_DD_ALSPRX_PWR_DOWN;
    status   = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_MODE_COMMAND, &i2c_data);
    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }
    DDF_MSG_2(MED, "AP3216C var1: %d var2: %d", 1130, i2c_data);

    state->chip.setup.prox.odr = 0;
    state->chip.setup.als.odr  = 0;
  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;
  return status;
}
/*===========================================================================
FUNCTION      sns_dd_ap3216c_update_registers

DESCRIPTION   update registers
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;
    uword_u wordVal;

    lsc_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    setup  = &state->chip.setup;

    /* Reset the device*/
    /* Power off*/
    byteVal = 0x00;
    status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_MODE_COMMAND, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Set Prox Interrupt Threshold low*/
    wordVal.word = setup->prox.thresh_low;
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_PX_LTHL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Set Prox Interrupt Threshold high*/
    wordVal.word = setup->prox.thresh_high;
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_PX_HTHL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    wordVal.word = state->chip.setup.als.thresh_low;
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_ALS_LTHL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    wordVal.word = state->chip.setup.als.thresh_high;
    status = sns_dd_ap3216c_write_word(state->port_handle, AP3216C_ALS_HTHL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    /* Set Prox pulses and current*/
    byteVal = (setup->prox.led_pulse << 4)| setup->prox.led_ratio;
    status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_PX_LED_DRI, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Clear any pending interrupts*/
    byteVal = SNS_CLR_ALL_INT;
    status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_INT_STATUS, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    byteVal = AP3216C_RANGE_C;
    if(state->chip.setup.als.again == AP3216C_AGAIN_16x)
    {
       status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_ALS_CON, &byteVal);
    }
       if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    /* Enable Power on */
    byteVal = state->chip.setup.reg.enable;
    status = sns_dd_ap3216c_write_byte(state->port_handle, AP3216C_MODE_COMMAND, &byteVal);

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_reset

DESCRIPTION   reset als and ps sensor
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status;

    DDF_MSG_1(MED, "AP3216C var1: %d", 1149);

    /* Reset driver state*/
    sns_dd_ap3216c_set_default_reg_values(state);

    status = sns_dd_ap3216c_update_registers(state);
    DDF_MSG_1(MED, "AP3216C reset exit: %d", status);
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_set_attrib

DESCRIPTION   set attrib
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_ap3216c_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

    DDF_MSG_1(MED, "AP3216C var1: %d", 1153);

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
      status = sns_dd_alsprx_ap3216c_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
      if(status != SNS_DDF_SUCCESS)
      {
          DDF_MSG_1(MED, "AP3216C var1: %d", 1154);
          return status;
      }

      break;

  case SNS_DDF_ATTRIB_ODR:
          if(sensor == SNS_DDF_SENSOR_PROXIMITY)
          {
            state->chip.setup.prox.odr = *(uint32_t*)value;
          }
          else if(sensor == SNS_DDF_SENSOR_AMBIENT)
          {
            state->chip.setup.als.odr = *(uint32_t*)value;
          }
          else
            {
               return SNS_DDF_EINVALID_ATTR;
            }
      break;

  default:
      return SNS_DDF_EINVALID_PARAM;
  }
        if(state->chip.setup.prox.odr > SNS_ALSPRO_INT_ODR && state->chip.setup.als.odr > SNS_ALSPRO_F_ODR)
          state->chip.setup.prox.odr = SNS_ALSPRO_INT_ODR;
        if(state->chip.setup.als.odr > SNS_ALSPRO_INT_ODR)
          state->chip.setup.als.odr = SNS_ALSPRO_INT_ODR;

  return(status);
}
/*===========================================================================
FUNCTION      sns_dd_ap3216c_get_mlux

DESCRIPTION   set attrib
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_get_mlux(sns_dd_alsprx_state_t* state)
{
    uint8_t readByte;
    uint16_t als_data;
    uint32_t als_lux;
    uint32_t als_mlux;
    DDF_MSG_0(HIGH, "api app get mlux");

    als_data = state->chip.data.als.data;
    sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_CON, &readByte);
    readByte &= SNS_RANGE_MASK;
    switch(readByte)
    {
        case AP3216C_RANGE_A:
            als_data *= AP3216C_RANGE_A_RATION;
            break; 
        case AP3216C_RANGE_B:
            als_data *= AP3216C_RANGE_B_RATION;
            break; 
        case AP3216C_RANGE_C:
            als_data *= AP3216C_RANGE_C_RATION;
            break;
        case AP3216C_RANGE_D:
            als_data *= AP3216C_RANGE_D_RATION;
            break;
        default:
        DDF_MSG_1(MED, "not correct range: %d", 1119);
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
FUNCTION      sns_dd_ap3216c_get_prox_data

DESCRIPTION   get prox data from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_get_prox_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e      status;
    uword_u pdata;

    status = sns_dd_ap3216c_read_word(state->port_handle,
                               AP3216C_PX_ADC_LSB,
                               &pdata);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX prx data err");
        return status;
    }
    state->chip.data.prox.data = pdata.word;
    DDF_MSG_2(MED, "AP3216C out of get prox data: %d var2: %d", 1158, pdata.word);

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_ap3216c_get_als_data

DESCRIPTION   get als data from device
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_get_als_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e      status;
    uword_u alsdata;

    status = sns_dd_ap3216c_read_word(state->port_handle,
                               AP3216C_ALS_ADC_LSB,
                               &alsdata);

    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX als data err1");
        return status;
    }
    state->chip.data.als.data = alsdata.word;
    DDF_MSG_2(MED, "AP3216C debugCode: %d alsdata: %d", 1159, alsdata.word);

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_ap3216c_prox_sensor_samples

DESCRIPTION   prox report handle
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = 2;

    DDF_MSG_1(MED, "AP3216C var1: %d", 1161);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "AP3216C var1: %d", 1162);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "AP3216C var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }
    DDF_MSG_1(MED, "AP3216C Faraway event num: %d", SNS_PRX_FAR_AWAY);
    DDF_MSG_1(MED, "AP3216C nearby event num: %d", SNS_PRX_NEAR_BY);
    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.prox.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.prox.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "PRX Data...");
    DDF_MSG_3(FATAL, "TMD PRX Data: Event:%d  Reported Data: %d  Raw Data: %d", state->chip.data.prox.event, data_ptr->samples[0].sample, state->chip.data.prox.data);
    sns_dd_alsprx_ap3216c_log(state, SNS_DDF_SENSOR_PROXIMITY, 
                        state->chip.data.prox.event, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.prox.data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_ap3216c_als_sensor_samples

DESCRIPTION   als report handle
===========================================================================*/
sns_ddf_status_e sns_dd_ap3216c_als_sensor_samples(sns_dd_alsprx_state_t    *state,
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
            DDF_MSG_1(MED, "AP3216C var1: %d", 1165);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "AP3216C var1: %d", 1166);
        return SNS_DDF_EINVALID_PARAM;
    }

    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.als.lux, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.als.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "ALS Data...");

    sns_dd_alsprx_ap3216c_log(state, SNS_DDF_SENSOR_AMBIENT, 
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_get_data

DESCRIPTION   get als and prox data
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_ap3216c_get_data
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

    DDF_MSG_0(HIGH, "ALSPRX Get Data ");
    DDF_MSG_0(HIGH, "ALSPRX Get Data prx");
    if((status = sns_dd_ap3216c_get_prox_data(state)) != SNS_DDF_SUCCESS) 
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err1: %d", status);
        return status;
    }

    if((status = sns_dd_alsprx_ap3216c_prx_binary(state)) != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err2: %d", status);
        return status;
    }

    DDF_MSG_0(HIGH, "ALSPRX Get Data als");
    if((status = sns_dd_ap3216c_get_als_data(state))  != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err1: %d", status);
        return status;
    }

    if((status = sns_dd_ap3216c_get_mlux(state))      != SNS_DDF_SUCCESS) 
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
            sns_dd_ap3216c_prox_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning als data to smgr");
            sns_dd_ap3216c_als_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        default:
            DDF_MSG_0(MED, "Neither ALS or Prox called");
         break;
        }
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_ap3216c_set_default_reg_values

DESCRIPTION   set default parameter
===========================================================================*/
void sns_dd_ap3216c_set_default_reg_values(sns_dd_alsprx_state_t* state)
{
    /* uint16_t tmp; */
    DDF_MSG_0(HIGH, "in set defualt reg values ");
    /* Operational parameters */

    state->chip.setup.als.again  = AP3216C_AGAIN_16x;
    state->chip.setup.prox.pgain = AP3216C_PGAIN_2x;

    DDF_MSG_0(HIGH, "in set defualt reg values -gain ");

    /* Known external ALS reading used for calibration */
    state->chip.setup.als.cal_target = SNS_ALS_CAL_TARGET;

    state->chip.setup.reg.enable = SNS_DD_ALSPRX_PWR_DOWN;
 
    DDF_MSG_0(HIGH, "in set defualt reg values -prox detect ");
    /* Set the prox thresh for detect */
    state->chip.setup.prox.thresh_low  =  SNS_DD_PRX_THRESH_FAR;
    state->chip.setup.prox.thresh_high =  SNS_DD_PRX_THRESH_NEAR;
    state->chip.data.prox.event        = SNS_PRX_FAR_AWAY;

    state->chip.setup.prox.led_pulse     = SNS_PRO_LED_PULSE;
    state->chip.setup.prox.led_ratio     = SNS_PRO_LED_RATIO;
    state->chip.setup.prox.odr             = SNS_ALSPRO_F_ODR;
    state->chip.setup.als.odr              = SNS_ALSPRO_F_ODR;

    state->chip.setup.als.thresh_low = SNS_ALS_THR_LOW_F;
    state->chip.setup.als.thresh_high = SNS_ALS_THR_HIG_F;
}

/*===========================================================================
FUNCTION      sns_dd_ap3216c_set_reg_values_from_nv

DESCRIPTION   set default parameter from nv
===========================================================================*/
void sns_dd_ap3216c_set_reg_values_from_nv(sns_dd_alsprx_state_t* state)
{
    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high != 0)
    {
    
        state->chip.setup.als.thresh_high = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_high;
        DDF_MSG_1(MED, "ALS High written %d", state->chip.setup.als.thresh_high);
    }

    if(state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low != 0)
    {
    
        state->chip.setup.als.thresh_low = state->sns_dd_alsprx_common_db.nv_db.ALS_thresh_low;
        DDF_MSG_1(MED, "ALS Low Written %d", state->chip.setup.als.thresh_low);
    }
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_run_test

DESCRIPTION   sensor self test
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ap3216c_run_test
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
    if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        DDF_MSG_0(HIGH, "in selftest sensor_ambient ");
        if(test == SNS_DDF_TEST_SELF)
        {
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_CON, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_CAL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_LTHL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_LTHH, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_HTHL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_ALS_HTHH, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
        }

        else if(test == SNS_DDF_TEST_OEM)
        {
            smgr_handle = state->smgr_handle;
            DDF_MSG_0(HIGH, "in selftest sensor_ambient OEM test ");

                status = sns_dd_alsprx_ap3216c_reset(state);
                               
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in reset ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AP3216C_ALS_CAL_FAILED_RST);
                    return status;
                }
                DDF_MSG_0(HIGH, "after reset"); 
                  
                /*  enable sensor */
                status = sns_dd_alsprx_ap3216c_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in set power state -sensor enable ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AP3216C_ALS_CAL_FAILED_ENB);
                    return status;
                }
                                                                 
                DDF_MSG_0(HIGH, "out of als scaling");
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in selftest -no registry notify");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_AP3216C_ALS_CAL_FAILED_REG);
                    return status;
                }
                DDF_MSG_0(HIGH, "successful registry update notify complete");
        }
        else
        {
           DDF_MSG_0(HIGH, "NOT OEM test ");
           return SNS_DDF_EINVALID_TEST;
        }
    }
    else if(sensor == SNS_DDF_SENSOR_PROXIMITY)
        {
            if(test == SNS_DDF_TEST_SELF)
            {
                status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_CON, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_LED_DRI, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_INT_FORM, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_MEAN_TIME, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_LED_WTIME, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_CAL_L, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_CAL_H, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_LTHL, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_LTHH, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_HTHL, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
                    status = sns_dd_ap3216c_read_byte(state->port_handle, AP3216C_PX_HTHH, &readByte);
                if( status != SNS_DDF_SUCCESS ) return status;
            }
            else
                {
                    DDF_MSG_0(HIGH, "NOT correct test ");
                    return SNS_DDF_EINVALID_TEST;
                }
        }
    else 
        {
            DDF_MSG_0(HIGH, "NOT CORRECT Sensor");
            return SNS_DDF_EINVALID_TEST;
        }
    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_ap3216c_probe

DESCRIPTION   sensor probe
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_ap3216c_probe
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
                              AP3216C_MODE_COMMAND,
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
    (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
    sns_ddf_close_port( port_handle );
  return status;
}
