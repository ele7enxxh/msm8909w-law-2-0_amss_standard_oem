/*==============================================================================

    S E N S O R S   L I G H T   A N D   P R O X I M I T Y   D R I V E R

DESCRIPTION

   Impelements the driver for the CM36283 driver.

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_cm36283.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
12/09/13     yh      Initial version of alsprx driver for cm36283
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_cm36283_priv.h"
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
extern sns_ddf_status_e sns_dd_alsprx_cm36283_init
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

static sns_ddf_status_e sns_dd_alsprx_cm36283_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_cm36283_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_cm36283_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

void sns_dd_alsprx_cm36283_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_alsprx_cm36283_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_alsprx_cm36283_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);
sns_ddf_status_e sns_dd_alsprx_cm36283_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );
 
 static sns_ddf_status_e sns_dd_alsprx_cm36283_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors );
  
sns_ddf_driver_if_s sns_alsprx_cm36283_driver_fn_list =
{
  .init = &sns_dd_alsprx_cm36283_init,
  .get_data = &sns_dd_alsprx_cm36283_get_data,
  .set_attrib = &sns_dd_alsprx_cm36283_set_attrib,
  .get_attrib = &sns_dd_alsprx_cm36283_get_attrib,
  .handle_timer = NULL,
  .handle_irq = &sns_dd_alsprx_cm36283_handle_irq,
  .reset = &sns_dd_alsprx_cm36283_reset,
  .run_test = &sns_dd_alsprx_cm36283_run_test,  
  .enable_sched_data = &sns_dd_alsprx_cm36283_enable_sched_data,
  .probe = &sns_dd_alsprx_cm36283_probe
};

sns_ddf_status_e sns_dd_alsprx_cm36283_als_samples(sns_dd_alsprx_state_t   *state,
                                            sns_ddf_sensor_data_s   *data_ptr,
                                            sns_ddf_memhandler_s    *memhandler,
                                            sns_ddf_sensor_sample_s *sample_data);

sns_ddf_status_e sns_dd_alsprx_cm36283_prox_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data);

void sns_dd_alsprx_cm36283_set_default_reg(sns_dd_alsprx_state_t* dd_ptr);

sns_ddf_status_e sns_dd_alsprx_cm36283_get_mlux(sns_dd_alsprx_state_t* state);

sns_ddf_status_e sns_dd_alsprx_cm36283_get_als_data(sns_dd_alsprx_state_t* state);
sns_ddf_status_e sns_dd_alsprx_cm36283_get_prox_data(sns_dd_alsprx_state_t* state);


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_write_word

DESCRIPTION   write a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_write_word(sns_ddf_handle_t port_handle, uint16_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t write_count;

    status = sns_ddf_write_port(port_handle,
                                reg,
                                &word_val->byte[0],
                                2,
                                &write_count
                                );
    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_read_word

DESCRIPTION   read a word from device
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_read_word(sns_ddf_handle_t port_handle, uint16_t reg, uword_u* word_val)
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
FUNCTION      sns_dd_alsprx_cm36283_log

DESCRIPTION   Log the latest sensor data
===========================================================================*/
void sns_dd_alsprx_cm36283_log(
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

   DDF_MSG_0(HIGH, "CM36283 Log Data");

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
       DDF_MSG_1(HIGH, "CM36283 Log Data - logpkt_malloc failed with err: %d", err_code);
       dd_ptr->dropped_logs++;
   }
#endif
}


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_enable_irq

DESCRIPTION  Enable interrupt mode for CM36283
============================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_enable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_alsprx_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uword_u readALS;
    uword_u readPS;	
    uword_u writeALS;
    uword_u writePS;

    if ((sensor != SNS_DDF_SENSOR_AMBIENT) && (sensor != SNS_DDF_SENSOR_PROXIMITY))
    {
        return SNS_DDF_EINVALID_PARAM;
    }
    else
    {
            if (sensor == SNS_DDF_SENSOR_AMBIENT)
            {
                if (state->chip.setup.als.int_enable == TRUE)
                {
                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    state->chip.setup.als.int_enable = TRUE;
						
                    status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_CONF,&readALS);
	              if ( status != SNS_DDF_SUCCESS )
                        return status;

                    writeALS.word= readALS.word| ALS_INT_ENABLE;
                    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF,&writeALS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;			
                }
            	}

            if (sensor == SNS_DDF_SENSOR_PROXIMITY)
            {
                if (state->chip.setup.prox.int_enable == TRUE)
                {
                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    state->chip.setup.prox.int_enable = TRUE;
						
                    status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CONF1_CONF2,&readPS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;

                    writePS.word= readPS.word| PS_INT_CLOSING_AWAY;
                    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2,&writePS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;
                }
            }
     }
    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_disable_irq

DESCRIPTION  Disable interrupt mode for CM36283
============================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_disable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_alsprx_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uword_u readALS;
    uword_u readPS;	
    uword_u writeALS;
    uword_u writePS;

    if ((sensor != SNS_DDF_SENSOR_AMBIENT) && (sensor != SNS_DDF_SENSOR_PROXIMITY))
    {
        return SNS_DDF_EINVALID_PARAM;
    }
    else
    {
            if (sensor == SNS_DDF_SENSOR_AMBIENT)
            {
                if (state->chip.setup.als.int_enable == FALSE)
                {
                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    state->chip.setup.als.int_enable = FALSE;
					
                    status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_CONF,&readALS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;

                    writeALS.word = (readALS.word & (~ALS_INT_ENABLE));
                    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF,&writeALS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;	
                }		
	     }		

            if (sensor == SNS_DDF_SENSOR_PROXIMITY)
            {
                if (state->chip.setup.prox.int_enable == FALSE)
                {
                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    state->chip.setup.prox.int_enable = FALSE;

                    status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CONF1_CONF2,&readPS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;

                    writePS.word= (readPS.word & (~PS_INT_CLOSING_AWAY));
                    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2,&writePS);
                    if ( status != SNS_DDF_SUCCESS )
                        return status;
                }
            }
          
            if ((state->chip.setup.als.int_enable==FALSE) & (state->chip.setup.prox.int_enable==FALSE))
            {
                status = sns_ddf_signal_deregister(state->int_gpio);
		   if(status != SNS_DDF_SUCCESS)
		   	return status;
            	}
		
    }
    return SNS_DDF_SUCCESS;
}



/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_enable_sched_data

DESCRIPTION   enable schedule data
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_enable_sched_data(sns_ddf_handle_t dd_handle,
                                                         sns_ddf_sensor_e sensor,
                                                         bool             enable
                                                         )
{
    sns_dd_alsprx_state_t* state = dd_handle;

    if(enable)
        return sns_dd_alsprx_cm36283_enable_irq(state, sensor);
    else
        return sns_dd_alsprx_cm36283_disable_irq(state, sensor);

}


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_prx_binary

DESCRIPTION   ensure far or near now
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_prx_binary
(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t pdata;
    uint16_t detect;
    uint16_t release;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    pdata   = state->chip.data.prox.data;
    DDF_MSG_1(MED, "pdata %d",  pdata );
    detect  = state->chip.setup.prox.detect;
    DDF_MSG_1(MED, "detect %d",  detect );
    release = state->chip.setup.prox.release;
    DDF_MSG_1(MED, "release %d",  release );

      if(pdata > detect)
      {
        state->chip.data.prox.event = SNS_PRX_NEAR_BY;
      }
      else if(pdata < release)
      {
        state->chip.data.prox.event = SNS_PRX_FAR_AWAY;
      }

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_als_thresh

DESCRIPTION   set als threshold
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_set_als_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t alsdata, t_low, t_high;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    DDF_MSG_1(MED, "ALS High Threshold %d",  state->chip.setup.als.thresh_high );
    DDF_MSG_1(MED, "ALS Low Threshold %d",  state->chip.setup.als.thresh_low);

    alsdata = state->chip.data.als.data;
    DDF_MSG_1(MED, "alsdata %d",  alsdata );

    t_low = (alsdata * (100 - ALS_THRESH_SELECT))/100;   //set to 20 as default
    DDF_MSG_1(MED, "als t low %d",  t_low );
    
    t_high = (alsdata * (100 + ALS_THRESH_SELECT))/100;   //set to 20 as default 
    DDF_MSG_1(MED, "als t high %d",  t_high);	

    wordVal.word = t_low; 
    DDF_MSG_1(MED, "als t low wordval.word %d",  wordVal.word);
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_THDL, &wordVal);
        if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
              return status;
           }

    wordVal.word = t_high; 
    DDF_MSG_1(MED, "als t high wordval.word %d",  wordVal.word);
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_THDH, &wordVal);
       if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
             return status;
          }

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_prx_thresh

DESCRIPTION   set prx threshold
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_set_prx_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint8_t psTHDL=0;
    uint8_t psTHDH=0;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    DDF_MSG_1(MED, "PS High Threshold %d",  state->chip.setup.prox.thresh_high );
    DDF_MSG_1(MED, "PS Low Threshold %d",  state->chip.setup.prox.thresh_low);

    psTHDL = state->chip.setup.prox.thresh_low;
    psTHDH = state->chip.setup.prox.thresh_high;
    wordVal.word = (psTHDH << 8) | psTHDL;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_THD, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    return status;
}


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_read_status_reg

DESCRIPTION   get data and report
===========================================================================*/
void sns_dd_alsprx_cm36283_read_status_reg(sns_dd_alsprx_state_t* state, sns_ddf_time_t timestamp, CM36283_SIG_TYPE sig_type)
{
    uword_u statusReg;
    uint8_t  numOfSensors;
    uint8_t  i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    sns_ddf_sensor_e sensors[2];
    numOfSensors  = 0;

    status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_INT_FLAG,&statusReg);
    if ( status != SNS_DDF_SUCCESS )
        return;

    if(statusReg.word & PS_INT_FLG_CLOSE)
    {
        /* PROX interrupt */
        state->chip.data.prox.event = SNS_PRX_NEAR_BY;

        state->chip.setup.prox.thresh_low = SNS_DD_PRX_THRESH_FAR;	
        state->chip.setup.prox.thresh_high= SNS_DD_PRX_THRESH_MAX;
        status = sns_dd_alsprx_cm36283_set_prx_thresh(state);
	  if (status != SNS_DDF_SUCCESS )
            return ;

        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DDF_MSG_1(MED, "CM36283 out of prox data and prx_binary set state: %d", 1115);
    }
    else if(statusReg.word & PS_INT_FLG_AWAY)
    {
         /* PROX interrupt */
         state->chip.data.prox.event = SNS_PRX_FAR_AWAY;

        state->chip.setup.prox.thresh_low = SNS_DD_PRX_THRESH_MIN;	
        state->chip.setup.prox.thresh_high= SNS_DD_PRX_THRESH_NEAR;
        status = sns_dd_alsprx_cm36283_set_prx_thresh(state);
	  if (status != SNS_DDF_SUCCESS )
            return ;		 

        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DDF_MSG_1(MED, "CM36283 out of prox data and prx_binary set state: %d", 1115);
    }

    if(statusReg.word & ALS_INT_FLG)
    {
        /* ALS interrupt */
        DDF_MSG_1(MED, "CM36283 ALS Interrupt  (int reg): %d", ALS_INT_FLG);
	
        /*Read ALS data*/
        sns_dd_alsprx_cm36283_get_als_data(state);
        sns_dd_alsprx_cm36283_set_als_thresh(state); /* set als threshold to be +/- 20% of current value for next interrupt.*/
		
        /* compute LUX*/
        sns_dd_alsprx_cm36283_get_mlux(state);
        sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
        numOfSensors++;
        DDF_MSG_1(MED, "CM36283 ALS Interrupt, data read: %d", ALS_INT_FLG);

        DDF_MSG_2(MED, "CM36283 ALS int numOfSensors: %d lux: %d", numOfSensors, state->chip.data.als.lux);
    }

    if(numOfSensors > 0)
    {
        /* Had at least one interrupt*/
	  status = sns_ddf_malloc((void **)&sensor_data, numOfSensors*sizeof(sns_ddf_sensor_data_s));	
        if (status != SNS_DDF_SUCCESS )
        {
            DDF_MSG_1(MED, "CM36283 var1: %d", 1116);
            /* Failed miserably. Can't even notify SMGR */
            return ;
        }

        for(i = 0; i < numOfSensors; i++)
        {
            sensor_data[i].sensor    = sensors[i];
            sensor_data[i].status    = SNS_DDF_SUCCESS;
            sensor_data[i].timestamp = timestamp;

            switch(sensors[i])
            {
            case SNS_DDF_SENSOR_PROXIMITY:
                DDF_MSG_1(MED, "CM36283 var1: %d", 1117);

		   status = sns_ddf_malloc((void **)&buf_ptr, 2 * sizeof(sns_ddf_sensor_sample_s));
                if(status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "CM36283 cannot notify smgr prx: %d", 1118);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                sns_dd_alsprx_cm36283_prox_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            case SNS_DDF_SENSOR_AMBIENT:
                DDF_MSG_1(MED, "CM36283 var1: %d", 1119);
		   status = sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s));		
                if(status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "CM36283 cannot notify smgr als: %d", 1120);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                sns_dd_alsprx_cm36283_als_samples(state, &sensor_data[i], NULL, buf_ptr);
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
    DDF_MSG_1(MED, "CM36283 out of read status reg: %d", 1123);
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_handle_irq

DESCRIPTION   irq handle
===========================================================================*/

void sns_dd_alsprx_cm36283_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;
    DDF_MSG_1(MED, "CM36283 in handle irq: %d", 1124);	
    sns_dd_alsprx_cm36283_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_powerstate

DESCRIPTION   set power state
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uword_u writeEN, readPS, readALS;

  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_CONF,&readALS);
  if ( status != SNS_DDF_SUCCESS )
      return status;

  status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CONF1_CONF2,&readPS);
  if ( status != SNS_DDF_SUCCESS )
      return status;

  state->chip.setup.prox.enable = readPS.word;
  state->chip.setup.als.enable = readALS.word;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          DDF_MSG_0(MED, "ONLY PROX");
          state->chip.setup.prox.enable &= ~CM36283_PRX_PWR_DOWN;
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          DDF_MSG_0(MED, "ONLY ALS");
          state->chip.setup.als.enable &= ~CM36283_ALS_PWR_DOWN;
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)   
      {
          state->chip.setup.prox.enable &= ~CM36283_PRX_PWR_DOWN;
          state->chip.setup.als.enable &= ~CM36283_ALS_PWR_DOWN;
          DDF_MSG_1(MED, "CM36283 var1: %d", 5555);
      }

      writeEN.word = state->chip.setup.als.enable;
      status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF, &writeEN);
      if ( status != SNS_DDF_SUCCESS ) {
          return status;
      }
	writeEN.word = state->chip.setup.prox.enable;
      status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2, &writeEN);
      if ( status != SNS_DDF_SUCCESS ) {
          return status;
      }  
	  
      DDF_MSG_2(MED, "CM36283 var1: %d var2: %d", 1128, writeEN.word);
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    if (init_data == TRUE)
    {
      /* cleanup */
      state->dropped_logs = 0;
      state->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;

      if (state->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
      {
         SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
       }
    }

      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          DDF_MSG_0(MED, "ONLY PROX");
          state->chip.setup.prox.enable |= CM36283_PRX_PWR_DOWN;
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          DDF_MSG_0(MED, "ONLY ALS");
          state->chip.setup.als.enable |= CM36283_ALS_PWR_DOWN;
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)   
      {
          state->chip.setup.prox.enable |= CM36283_PRX_PWR_DOWN;
          state->chip.setup.als.enable |= CM36283_ALS_PWR_DOWN;
          DDF_MSG_1(MED, "CM36283 var1: %d", 5555);
      }
	  
    writeEN.word = state->chip.setup.als.enable;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF, &writeEN);
    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }
    writeEN.word = state->chip.setup.prox.enable;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2, &writeEN);
    if ( status != SNS_DDF_SUCCESS )
    {
      return status;
    }	
    DDF_MSG_2(MED, "CM36283 var1: %d var2: %d", 1130, writeEN.word);

    state->chip.setup.prox.odr = 0;
    state->chip.setup.als.odr  = 0;
  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;
  return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_update_registers

DESCRIPTION   update registers
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t psTHDL=0;
    uint8_t psTHDH=0;
    uint16_t als_range=0;	
    uword_u wordVal;

    lsc_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    setup  = &state->chip.setup;

    /* Reset the device*/
    /* Power off*/
    wordVal.word = ALS_DISABLE;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Set Prox Interrupt Threshold low*/
    wordVal.word = setup->als.thresh_low;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_THDL, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    /* Set Prox Interrupt Threshold high*/
    wordVal.word = setup->als.thresh_high;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_THDH, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    wordVal.word = PS_DISABLE;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }


    wordVal.word = state->chip.setup.prox.steps;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF3_MS, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    } 	

    psTHDL = state->chip.setup.prox.thresh_low;
    psTHDH = state->chip.setup.prox.thresh_high;
    wordVal.word = (psTHDH << 8) | psTHDL;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_THD, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
	
    /* Enable Power on */
    als_range = state->chip.setup.als.range;	
    wordVal.word = ALS_CONF_VAL | als_range;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_ALS_CONF, &wordVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    wordVal.word = PS_CONF12_VAL;
    status = sns_dd_alsprx_cm36283_write_word(state->port_handle, CM36283_PS_CONF1_CONF2, &wordVal);

    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_reset

DESCRIPTION   reset als and ps sensor
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status;

    /* Reset driver state*/
    sns_dd_alsprx_cm36283_set_default_reg(state);

    status = sns_dd_alsprx_cm36283_update_registers(state);
    DDF_MSG_1(MED, "CM36283 reset exit: %d", status);
    return status;
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_attrib

DESCRIPTION   set attrib
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_set_attrib
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
      status = sns_dd_alsprx_cm36283_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
      if(status != SNS_DDF_SUCCESS)
      {
          return status;
      }
      sns_ddf_delay(30000);
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

  return(status);
}
/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_get_mlux

DESCRIPTION   calc the Lux value
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_get_mlux(sns_dd_alsprx_state_t* state)
{
    uint16_t als_data;
    uint16_t als_range;	
    uint32_t als_lux;
    uint32_t als_mlux;
    DDF_MSG_0(HIGH, "api app get mlux");

    als_data = state->chip.data.als.data;
    als_range = state->chip.setup.als.range;
	
    switch(als_range)
    {
        case CM36283_RANGE_A:
            als_data *= CM36283_RANGE_A_RATION;
            break; 
        case CM36283_RANGE_B:
            als_data *= CM36283_RANGE_B_RATION;
            break; 
        case CM36283_RANGE_C:
            als_data *= CM36283_RANGE_C_RATION;
            break;
        case CM36283_RANGE_D:
            als_data *= CM36283_RANGE_D_RATION;
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
FUNCTION      sns_dd_alsprx_cm36283_get_prox_data

DESCRIPTION   get prox data from device
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_get_prox_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;
    uword_u pdata;

    status = sns_dd_alsprx_cm36283_read_word(state->port_handle,
                               CM36283_PS_DATA,
                               &pdata);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX prx data err");
        return status;
    }
    state->chip.data.prox.data = pdata.word;
    DDF_MSG_2(MED, "CM36283 out of get prox data: %d var2: %d", 1158, pdata.word);

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_get_als_data

DESCRIPTION   get als data from device
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_get_als_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;
    uword_u alsdata;

    status = sns_dd_alsprx_cm36283_read_word(state->port_handle,
                               CM36283_ALS_DATA,
                               &alsdata);

    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX als data err1");
        return status;
    }
    state->chip.data.als.data = alsdata.word;
    DDF_MSG_2(MED, "CM36283 var1: %d var2: %d", 1159, alsdata.word);

    return status;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_prox_samples

DESCRIPTION   prox report handle
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_prox_samples(sns_dd_alsprx_state_t   *state,
                                             sns_ddf_sensor_data_s   *data_ptr,
                                             sns_ddf_memhandler_s    *memhandler,
                                             sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = 2;

    DDF_MSG_1(MED, "CM36283 var1: %d", 1161);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "CM36283 var1: %d", 1162);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "CM36283 var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }
    DDF_MSG_1(MED, "CM36283 Faraway event num: %d", SNS_PRX_FAR_AWAY);
    DDF_MSG_1(MED, "CM36283 nearby event num: %d", SNS_PRX_NEAR_BY);
    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.prox.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.prox.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "PRX Data...");
    DDF_MSG_3(FATAL, "TMD PRX Data: Event:%d  Reported Data: %d  Raw Data: %d", state->chip.data.prox.event, data_ptr->samples[0].sample, state->chip.data.prox.data);
    sns_dd_alsprx_cm36283_log(state, SNS_DDF_SENSOR_PROXIMITY, 
                        state->chip.data.prox.event, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.prox.data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_als_samples

DESCRIPTION   als report handle
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_als_samples(sns_dd_alsprx_state_t    *state,
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
            DDF_MSG_1(MED, "CM36283 var1: %d", 1165);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "CM36283 var1: %d", 1166);
        return SNS_DDF_EINVALID_PARAM;
    }

    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.als.lux, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.data.als.data;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "ALS Data...");

    sns_dd_alsprx_cm36283_log(state, SNS_DDF_SENSOR_AMBIENT, 
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_get_data

DESCRIPTION   get als and prox data
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_get_data
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

    if((status = sns_dd_alsprx_cm36283_get_prox_data(state)) != SNS_DDF_SUCCESS) 
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err1: %d", status);
        return status;
    }

    if((status = sns_dd_alsprx_cm36283_prx_binary(state)) != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err2: %d", status);
        return status;
    }

    DDF_MSG_0(HIGH, "ALSPRX Get Data als");
    if((status = sns_dd_alsprx_cm36283_get_als_data(state))  != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err1: %d", status);
        return status;
    }

    if((status = sns_dd_alsprx_cm36283_get_mlux(state))      != SNS_DDF_SUCCESS) 
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
            sns_dd_alsprx_cm36283_prox_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning als data to smgr");
            sns_dd_alsprx_cm36283_als_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        default:
            DDF_MSG_0(MED, "Neither ALS or Prox called");
         break;
        }
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_default_reg

DESCRIPTION   set default parameter
===========================================================================*/
void sns_dd_alsprx_cm36283_set_default_reg(sns_dd_alsprx_state_t* state)
{
    /* uint16_t tmp; */
    DDF_MSG_0(HIGH, "in set defualt reg values ");
    /* Operational parameters */

    DDF_MSG_0(HIGH, "in set defualt reg values -gain ");

    /* Known external ALS reading used for calibration */
    state->chip.setup.als.range = CM36283_RANGE_A;
    state->chip.setup.als.int_enable = FALSE;
    state->chip.setup.prox.int_enable = FALSE;	
    state->chip.setup.als.enable = SNS_DD_ALSPRX_PWR_DOWN;
    state->chip.setup.prox.enable = SNS_DD_ALSPRX_PWR_DOWN;
	
    /*default threshold (adjust either manually or with cal routine*/
    state->chip.setup.prox.detect = SNS_DD_PRX_THRESH_NEAR;
    state->chip.setup.prox.release= SNS_DD_PRX_THRESH_FAR;
 
    DDF_MSG_0(HIGH, "in set defualt reg values -prox detect ");
    /* Set the prox thresh for detect */
    state->chip.setup.prox.thresh_low  =  SNS_DD_PRX_THRESH_FAR;
    state->chip.setup.prox.thresh_high =  SNS_DD_PRX_THRESH_NEAR;
    state->chip.data.prox.event        = SNS_PRX_FAR_AWAY;
    state->chip.setup.prox.steps =  PS_STEPS_255;	

    state->chip.setup.prox.odr             = SNS_ALSPRO_F_ODR;
    state->chip.setup.als.odr              = SNS_ALSPRO_F_ODR;

    state->chip.setup.als.thresh_low = SNS_ALS_THR_LOW_F;
    state->chip.setup.als.thresh_high = SNS_ALS_THR_HIG_F;

}

/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_set_reg_from_nv

DESCRIPTION   set default parameter from nv
===========================================================================*/
void sns_dd_alsprx_cm36283_set_reg_from_nv(sns_dd_alsprx_state_t* state)
{
    if(state->sns_dd_alsprx_common_db.nv_db.thresh_near != 0)
    {
        state->chip.setup.prox.detect = state->sns_dd_alsprx_common_db.nv_db.thresh_near;
    }

    if(state->sns_dd_alsprx_common_db.nv_db.thresh_far != 0)
    {
        state->chip.setup.prox.release = state->sns_dd_alsprx_common_db.nv_db.thresh_far;
    }

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

FUNCTION      sns_dd_alsprx_cm36283_compute_lux_ave

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_compute_lux_ave(sns_dd_alsprx_state_t* state)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status;

    luxSum = 0;
    
    DDF_MSG_0(HIGH, "about to get als data and mlux ");
    
    
    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        status = sns_dd_alsprx_cm36283_get_als_data(state); 
        if( status != SNS_DDF_SUCCESS )
        {
        DDF_MSG_0(HIGH, "failed in get als data ");
        }
        status = sns_dd_alsprx_cm36283_get_mlux(state);    
        if( status != SNS_DDF_SUCCESS )
        {
        DDF_MSG_0(HIGH, "failed in app get mlux ");
        }
        luxSum += state->chip.data.als.lux;
        DDF_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);
       
        // Wait some time before another reading
        sns_ddf_delay(25000);
        DDF_MSG_0(HIGH, "after 25000 us delay ");
    }
    
    state->chip.data.als.luxAve = (luxSum / NUM_OF_LUX_TO_AVE);   
	    DDF_MSG_2(MED, "luxsum %d luxsum/8/1000: %d", luxSum, state->chip.data.als.luxAve);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

FUNCTION      sns_dd_alsprx_cm36283_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_calibrate_als(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    uint32_t  tgtLux,avgLux,tgtLuxLowRange, tgtLuxHiRange;
    avgLux=1;
    tgtLux=1;
	
    // compare the average lux to the passed in calibration lux,
    DDF_MSG_0(HIGH, "in calibrate als next compute lux ");
    if ((status = sns_dd_alsprx_cm36283_compute_lux_ave(state)) != SNS_DDF_SUCCESS) 
    {
     DDF_MSG_0(HIGH, "computelux ave failed ");
     return status;
    }
 
     DDF_MSG_0(HIGH, "out of compute lux ");
     // Find Lux mid way between calLuxLower and calLuxUpper
     tgtLux = (state->sns_dd_alsprx_common_db.nv_db.calLuxLower + state->sns_dd_alsprx_common_db.nv_db.calLuxUpper) / 2;
     
     tgtLuxLowRange = 0.1*tgtLux;
     tgtLuxHiRange = 10*tgtLux;
     DDF_MSG_2(MED, "tgtLux low %d tgt lux hi %d", tgtLuxLowRange, tgtLuxHiRange);
	 avgLux = state->chip.data.als.luxAve;
     DDF_MSG_1(MED, "Avg Lux: %d",  avgLux);
	
	//check if avg lux is outside expected range of target lux. If so, return efail
    if ((avgLux <= tgtLuxLowRange) || (avgLux >= tgtLuxHiRange))
	 {
      DDF_MSG_3(HIGH, "Avg Lux in Test %d lux out of Expected range. Below %d lux or above %d lux. Setting status to efail.",avgLux,tgtLuxLowRange,tgtLuxHiRange);
      return SNS_DDF_EFAIL;
	 }
	 
     tgtLux = FX_FLTTOFIX_Q16(tgtLux); 
     avgLux = FX_FLTTOFIX_Q16(avgLux);
     
     DDF_MSG_2(MED, "tgtLux q16 0x:%x avglux q16: 0x%x", tgtLux, avgLux);  
     state->sns_dd_alsprx_common_db.nv_db.luxCorrection = FX_DIV_Q16(tgtLux,avgLux); 
     DDF_MSG_1(MED, "new val of state->sns_dd_alsprx_common_db.nv_db.luxCorrection q16: 0x%x",  state->sns_dd_alsprx_common_db.nv_db.luxCorrection);

    return status;
}


/*===========================================================================
FUNCTION      sns_dd_alsprx_cm36283_run_test

DESCRIPTION   sensor self test
===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_cm36283_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t       smgr_handle;	
    uword_u readByte;
	
    if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        DDF_MSG_0(HIGH, "in selftest sensor_ambient ");
        if(test == SNS_DDF_TEST_SELF)
        {
            /* Read & Verify Device ID */
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_DEV_ID, &readByte);
            if ( status != SNS_DDF_SUCCESS )
            {
                DDF_MSG_1(HIGH, "chip id readback error %d", readByte.word);
                return status;
            }
	      if(readByte.word != CM36283_CHIP_ID)
            {
                DDF_MSG_1(HIGH, "chip id readback error %d", readByte.word);
                return SNS_DDF_EDEVICE;
            }			
		
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_CONF, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_THDL, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_THDH, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_ALS_DATA, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
		
            smgr_handle = state->smgr_handle;
            /* Perform ams specific test to Calibrate ALS */
	     /*  controlled by registry paramter calibratePhone. */
            if(state->sns_dd_alsprx_common_db.nv_db.calibratePhone)
            {
                /* Set Default values already est in reset */
                status = sns_dd_alsprx_cm36283_reset(state);
                               
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in reset ");
					sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_CM36283_ALS_CAL_FAILED_RST);
                    return status;
                }
                DDF_MSG_0(HIGH, "after reset"); 

                state->chip.setup.als.odr = 10;
                state->chip.setup.prox.odr = 10;
                  
                /*  enable sensor  */
                status = sns_dd_alsprx_cm36283_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in set power state -sensor enable ");
					sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_CM36283_ALS_CAL_FAILED_ENB);
                    return status;
                }
                 sns_ddf_delay(125000); // Delay for write time to filter to the part                             
                /* update als scaling  */
                DDF_MSG_0(HIGH, "into als scaling");
                status = sns_dd_alsprx_cm36283_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                { 
                    DDF_MSG_0(HIGH, "calib als failed ");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_CM36283_ALS_CAL_FAILED_LUX);
                    return status;
                }
                DDF_MSG_0(HIGH, "out of als scaling");

                state->sns_dd_alsprx_common_db.nv_db.visible_ratio = 1;
			
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "failed in selftest -no registry notify");
                    sns_ddf_smgr_notify_test_complete(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EFAIL, SNS_DD_CM36283_ALS_CAL_FAILED_REG);
                    return status;
                }
                DDF_MSG_0(HIGH, "successful registry update notify complete");
            }
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
            /* Read & Verify Device ID */
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_DEV_ID, &readByte);
            if ( status != SNS_DDF_SUCCESS )
            {
                DDF_MSG_1(HIGH, "chip id readback error %d", readByte.word);
                return status;
            }
	      if(readByte.word != CM36283_CHIP_ID)
            {
                DDF_MSG_1(HIGH, "chip id readback error %d", readByte.word);
                return SNS_DDF_EDEVICE;
            }		  	
		  		
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CONF1_CONF2, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CONF3_MS, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_CANC, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_THD, &readByte);
            if( status != SNS_DDF_SUCCESS ) return status;
            status = sns_dd_alsprx_cm36283_read_word(state->port_handle, CM36283_PS_DATA, &readByte);
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
FUNCTION      sns_dd_alsprx_cm36283_probe

DESCRIPTION   sensor probe
===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_cm36283_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
)
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uword_u readID;

  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read & Verify Device ID */
  status = sns_dd_alsprx_cm36283_read_word(port_handle, CM36283_DEV_ID, &readID);
  DDF_MSG_1(MED, "chip id wordval.word readback %d",  readID.word); 
  if ( status != SNS_DDF_SUCCESS )
  {
      DDF_MSG_1(HIGH, "chip id readback error %d", readID.word);
      return status;
  }
                             
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
