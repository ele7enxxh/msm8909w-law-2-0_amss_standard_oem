/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps25h.c
* Authors           : Jianjian Huo
*                   : Wei Wang
* Version           : V 1.0.7
* Date              : 07/30/2014
* Description       : LPS25H pressure sensor driver source file
*
********************************************************************************
* Copyright (c) 2014, STMicroelectronics.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of the STMicroelectronics nor the
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
* REVISON HISTORY
*
* Version      when         who      what
* -------- ----------       ---     -----------------------------------------------------------
*  1.0.7        09/30/14     WW      Improve accuracy (Wei Wang)
*               08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
*               07/27/2014   VY      Created for uImage
*
*******************************************************************************/

/*==============================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "sns_dd_press_lps25h.h"

/**
 * LPS25H pressure sensor device driver interface.
 */
sns_ddf_driver_if_s sns_dd_press_lps25h_if =
{
    .init       = &sns_dd_press_lps25h_init,
    .get_data   = &sns_dd_press_lps25h_get_data,
    .set_attrib = &sns_dd_press_lps25h_set_attr,
    .get_attrib = &sns_dd_press_lps25h_get_attr,
    .handle_timer = &sns_dd_press_lps25h_handle_timer,
    .handle_irq = NULL,
    .reset      = &sns_dd_press_lps25h_reset,
    .run_test   = &sns_dd_press_lps25h_run_test,
    .enable_sched_data = NULL,
    .probe      = &sns_dd_press_lps25h_probe
};


/**
 * @brief Enable FIFO in LPS25H
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_press_enable_FIFO(
  sns_ddf_handle_t dd_handle)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = { 0 };
    uint8_t bytes_written = 0;

    // enable FIFO
    write_buffer[0] = 0x0
                | (0<<7)    //BOOT:
                | (1<<6)    //FIFO_EN:
                | (1<<5)    //WTM_EN:
                | (0<<4)    //FIFO_MEAN_DEC:
                | (0<<2)    //SWRESET:
                | (0<<1)    //AUTO_ZERO:
                | 0;        //ONE_SHOT:    
                          
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS25H_CTRL_REG2,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    return SNS_DDF_SUCCESS;
}


/**
 * @brief Log data packets
 *
 * @param[in]  sensor      Sensor ID
 * @param[in]  data        Data to log
 * @param[in]  timestamp   Timestamp of data
 */
static void sns_dd_log_data_lps25h(sns_ddf_sensor_e sensor, uint32_t data,
                                   sns_ddf_time_t timestamp)
{
    sns_err_code_e status;
    sns_log_sensor_data_pkt_s* log;

    status = sns_logpkt_malloc(
        SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log);
    if ( (status == SNS_SUCCESS) && (log != NULL) )
    {
        log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log->sensor_id = sensor;
        log->vendor_id = SNS_DDF_VENDOR_STMICRO;
        log->timestamp = timestamp;
        log->num_data_types = 1;
        log->data[0] = data;
        sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
    }
}


/** 
 * @brief Retrieves a single set of pressure and/or temperature data from lps25h
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t i;
    uint8_t read_buffer[5];
    uint8_t read_count = 0;
    q16_t sample;
    uint32_t pressure;
    int16_t temperature;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_press_lps25h_state_t *state = dd_handle;

    //if current power mode is LOWPOWER , return error.
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors > 2))
        return SNS_DDF_EDEVICE;
    
    //allocate memory for sns_ddf_sensor_data_s data structure
    data_ptr = sns_ddf_memhandler_malloc_ex(memhandler, 
                 num_sensors * sizeof(sns_ddf_sensor_data_s), state->smgr_hndl);
    if(NULL == data_ptr)
        return SNS_DDF_ENOMEM;

    *data = data_ptr;

    for(i = 0; i < num_sensors; i++)
    {
        // get current time stamp
        timestamp = sns_ddf_get_timestamp();
    
        // This is a synchronous driver, so try to read data now.        
        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PRESSURE:
            status = sns_ddf_read_port(
                state->port_handle,
                AUTO_INCREMENT | STM_LPS25H_PRESS_OUT_XL,
                read_buffer,
                STM_LPS25H_NUM_BYTES_PRESS,
                &read_count);
            // check if the read was a success
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(read_count != STM_LPS25H_NUM_BYTES_PRESS)
                return SNS_DDF_EBUS;

            // enable FIFO after first data available.
            /*if(0 == state->FIFO_enable)
            {
                status = sns_dd_press_enable_FIFO(dd_handle);
                if(status != SNS_DDF_SUCCESS)
                    return status;
                state->FIFO_enable = 1;
            }*/
            
            break;
        
        case SNS_DDF_SENSOR_TEMP:
            status = sns_ddf_read_port(
                state->port_handle,
                AUTO_INCREMENT | STM_LPS25H_TEMP_OUT_L,
                read_buffer,
                STM_LPS25H_NUM_BYTES_TEMP,
                &read_count);
            // check if the read was a success
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(read_count != STM_LPS25H_NUM_BYTES_TEMP)
                return SNS_DDF_EBUS;
            break;
        default:
        	return SNS_DDF_EINVALID_PARAM;
        	break;
        }

        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;
    
        //allocate memory for data samples.
        data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler, 
                                sizeof(sns_ddf_sensor_sample_s), state->smgr_hndl);
        if(NULL == data_ptr[i].samples)
            return SNS_DDF_ENOMEM;

        if(sensors[i] == SNS_DDF_SENSOR_PRESSURE) 
        {
            pressure = (uint32_t)((uint32_t)read_buffer[0] 
                                   | (uint32_t)((read_buffer[1])<<8) 
                                   | (uint32_t)((read_buffer[2])<<16));
            
            //convert raw pressure LSB data to Q16 and milli-bar 
            sample = pressure <<4;
        }else{
            temperature = (int32_t) ((read_buffer[1] << 8) | (read_buffer[0]));
            sample = FX_FLTTOFIX_Q16(42.5) + FX_DIV_Q16(FX_CONV_Q16(temperature,0), FX_CONV_Q16((int32_t)480,0));
        }

        data_ptr[i].samples->sample = sample;
        data_ptr[i].samples->status = SNS_DDF_SUCCESS;
        data_ptr[i].num_samples = 1;

        // Log sensor data
        sns_dd_log_data_lps25h(data_ptr[i].sensor, data_ptr[i].samples->sample,
                                     data_ptr[i].timestamp);
    }
    
    return SNS_DDF_SUCCESS;
}


void sns_dd_press_lps25h_handle_timer(
    sns_ddf_handle_t dd_handle, 
    void* arg)
{
    sns_dd_press_lps25h_state_t* state = (sns_dd_press_lps25h_state_t *)dd_handle;
    sns_ddf_status_e status;
    uint8_t i2c_buffer[STM_LPS25H_NUM_BYTES_PRESS];
    uint8_t ctrl_reg1;
    uint8_t rw_bytes;
    uint32_t P_uBar[10];
    int32_t T_mC[10];
    int i, P_DupCount, T_DupCount;
    uint32_t err;

    if(state->self_test_enable == 1)
    {
        err=STMERR_TEST_UNKNOWN;
        state->self_test_enable = 0;

	       // 0. Backup CTRL_REG1
        status = sns_ddf_read_port(state->port_handle,  STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if((status != SNS_DDF_SUCCESS) || (rw_bytes!=1))
        {
            err = STMERR_TEST_I2C_FAIL;
            sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				           SNS_DDF_SENSOR_PRESSURE,
				          status, err);
            return;
        }

	       // 1. Set LPS25H to active mode, 12.5Hz ODR
	       LPS25H_MSG_0(MED, "1. Set sensor active.");
	       i2c_buffer[0] = 0x0
			       | (1<<7)                        // PD (1 for active)
			       | (STM_LPS25H_ODR_12p5_12p5<<4) // ODR2:0 (Bits [6:4]) - data rate
			       | (0<<3)                        // DIFF_EN (interrupt enable)
			       | (1<<2)                        // BDU (block data update)
			       | (0<<1)                        // RESET_AZ
			       | (0);                          // SIM (SPI Serial Interface Mode selection)
	       status = sns_ddf_write_port(state->port_handle, STM_LPS25H_CTRL_REG1, i2c_buffer, 1, &rw_bytes);
        if((status != SNS_DDF_SUCCESS) || (rw_bytes!=1))
        {
            err = STMERR_TEST_I2C_FAIL;
            sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				           SNS_DDF_SENSOR_PRESSURE,
				          status, err);
            return;
        }

        // 2. Collect 10 samples for both pressure and temperature
	       LPS25H_MSG_0(MED, "2. Collect 10 samples.");
        for (i=0; i<10; i++)
        {
    	        sns_ddf_delay(100000);	// Wait 100ms for one sample ready.
             status = sns_ddf_read_port(state->port_handle,  AUTO_INCREMENT | STM_LPS25H_PRESS_OUT_XL, i2c_buffer,
        		                              STM_LPS25H_NUM_BYTES_PRESS, &rw_bytes);
            if((status != SNS_DDF_SUCCESS) || (rw_bytes!=STM_LPS25H_NUM_BYTES_PRESS))
            {
                err = STMERR_TEST_I2C_FAIL;
                sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				               SNS_DDF_SENSOR_PRESSURE,
				               status, err);
                 return;
            }
            P_uBar[i]=(uint64_t)((i2c_buffer[2]<<16)|(i2c_buffer[1]<<8)|i2c_buffer[0])*1000/4096;
            status = sns_ddf_read_port(state->port_handle,  AUTO_INCREMENT | STM_LPS25H_TEMP_OUT_L, i2c_buffer,
        		                               STM_LPS25H_NUM_BYTES_TEMP, &rw_bytes);
            if((status != SNS_DDF_SUCCESS) || (rw_bytes!=STM_LPS25H_NUM_BYTES_TEMP))
            {
                err = STMERR_TEST_I2C_FAIL;
                sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				               SNS_DDF_SENSOR_PRESSURE,
				               status, err);
                 return;
            }
            T_mC[i]=(int32_t)((int16_t)((i2c_buffer[1]<<8)|i2c_buffer[0]))*1000/480+42500;
            LPS25H_MSG_2(MED, "P=%duBar, T=%dmC",P_uBar[i],T_mC[i]);
        }

         // 3. Analyze data.
         LPS25H_MSG_0(MED, "3. Analyze data.");
         P_DupCount=T_DupCount=0;
	        err=STMERR_TEST_OK;
	        for (i=0;i<10;i++)
	        {
		            if ((P_uBar[i]<900000)||(P_uBar[i]>1100000)) err|=STMERR_TEST_1ST_SENSOR_FAIL;
		            if ( (i>0) && (P_uBar[i]==P_uBar[i-1]) ) P_DupCount++;
		            if ((T_mC[i]<0)||(T_mC[i]>50000)) err|=STMERR_TEST_2ND_SENSOR_FAIL;
		            if ( (i>0) && (T_mC[i]==T_mC[i-1]) ) T_DupCount++;
	        }
	        if (P_DupCount>5) err|=STMERR_TEST_1ST_SENSOR_FAIL;
	        if (T_DupCount>5) err|=STMERR_TEST_2ND_SENSOR_FAIL;

        // 4. restore CTRL_REG1 and put sensor power down.
	       LPS25H_MSG_0(MED, "4. Set sensor power down.");
	       status = sns_ddf_write_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if((status != SNS_DDF_SUCCESS) || (rw_bytes!=1))
        {
            err = STMERR_TEST_I2C_FAIL;
            sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				           SNS_DDF_SENSOR_PRESSURE,
				           status, err);
            return;
        }
        
        if(err != STMERR_TEST_OK) status = SNS_DDF_EFAIL;

	       sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
				           SNS_DDF_SENSOR_PRESSURE,
				           status, err);
    }
}


