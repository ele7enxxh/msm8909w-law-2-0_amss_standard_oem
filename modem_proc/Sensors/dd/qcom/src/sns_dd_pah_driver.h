/********************************************************************************
* Copyright (c) 2016, PixArt Imaging Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the PixArt Imaging Inc. nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
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
* Edit History
* 
* This section contains comments describing changes made to the module. Notice
* that changes are listed in reverse chronological order. Please use ISO format
* for dates.
* 
* when       who       what, where, why
* ---------- ---       -----------------------------------------------------------
* 2016-06-07 bell      - Add functions: pah_set_mode(), pah_run_device().
*                      - Add enum: pah_device.
*                      - Add comments.
* 2016-04-29 bell      - Add PPG 200Hz modes.
*                      - Add helper functions: pah_is_ppg_mode(), pah_is_ppg_20hz_mode(), pah_fifo_data_num_per_ch().
*                      - Add pah_stop_mode
*                      - Remove pah_suspend_mode.
*                      - Fix setting pah_set_report_sample_num_per_ch() after enter_mode() causes bad behavior.
* 2016-04-20 bell      Add pah_stop_mode. pah_none can be regarded as pah_stop_mode.
* 2016-04-12 bell      Add license information and revision information.
* 2016-04-07 bell      Initial revision.
==============================================================================*/


#ifndef __pah_driver_h__
#define __pah_driver_h__
#include <stdio.h>
#include "fixed_point.h"
#include <stdbool.h>

// c
//#include <stdbool.h>
//#include <stdint.h>


typedef enum {

  pah_none,                   // Not used

  pah_stop_mode,              // Stop mode

  pah_touch_mode,             // Touch mode

  pah_ppg_mode,               // PPG mode
  pah_ppg_200hz_mode,         // PPG(200Hz) mode

  pah_ppg_touch_mode,         // PPG + Touch mode
  pah_ppg_200hz_touch_mode,   // PPG(200Hz) + Touch mode

} pah_mode;


typedef enum {

  pah_device_ppg,             // PPG device
  pah_device_touch,           // Touch device

} pah_device;


typedef enum {

  pah_success,                // Success
  pah_no_interrupt,           // The device doesn't raise an interrupt yet

  pah_err_unknown,            // Unknown error
  pah_err_invalid_argument,   // Invalid argument to function
  pah_err_comm_fail,          // Communication(I2C, SPI, ...) failed

  pah_err_invalid_program,    // Generally this indicates a bug in the driver

  pah_err_fifo_checksum_fail, // Failed to compare check sum with fifo data
                // Usually happens to bad data communication

  pah_err_fifo_overflow,      // Fifo in device is overflow
                // Usually happens when pah_task() was too late to be called
  
  pah_err_fifo_underflow,     // Fifo in device is underflow

} pah_ret;


typedef struct {

  uint8_t*    data;
  uint8_t     touch_flag;

} pah_report_fifo;

typedef void(*pah_report_fifo_handle)(void* user_data, pah_report_fifo* fifo);


/**
 * @brief Initialize the driver.
 * 
 * It should been called once before any other function calls.
 *
 * @return True if successful.
 */
bool        pah_init(void);

/**
 * @brief De-initialize the driver.
 *
 * @return None.
 */
void        pah_deinit(void);


/**
 * @brief Set device into a given pah mode and run device.
 * 
 * It change device into a specific mode(refer to enum pah_mode) and then enable device to run if the mode depends on device.
 * Equal to the combination of pah_set_mode() and pah_run(), i.e
 *     pah_set_mode(pah_ppg_mode);
 *     pah_run(pah_device_ppg, true);
 *
 * @param[in]  mode         Refer to enum pah_mode.
 *
 * @return True if setting successfully.
 */
bool        pah_enter_mode(pah_mode mode);


/**
 * @brief Set device into a given pah mode.
 * 
 * It change device into a specific mode(refer to enum pah_mode).
 *
 * @param[in]  mode         Refer to enum pah_mode.
 *
 * @return True if setting successfully.
 */
bool        pah_set_mode(pah_mode mode);

/**
 * @brief Enable/Disable device to run.
 * 
 * It returns false if the current mode isn't activable, such as pah_stop_mode.
 *
 * @param[in]  device       Refer to enum pah_device.
 * @param[in]  enable       Enable/Disable the device.
 *
 * @return True if setting successfully.
 */
bool        pah_run_device(pah_device device, bool enable);


/**
 * @brief Query current mode.
 *
 * @return Refer to enum pah_mode.
 */
pah_mode    pah_query_mode(void);

/**
 * @brief Check if the device is in PPG mode.
 *
 * @return True if the device is in PPG mode.
 */
bool        pah_is_ppg_mode(void);

/**
 * @brief Check if the device is in PPG(20Hz) mode.
 *
 * @return True if the device is in PPG(20Hz) mode.
 */
bool        pah_is_ppg_20hz_mode(void);


/**
 * @brief Process tasks after INT1 interrupt.
 *
 * Whenever the device raises INT1 interrupt, this function must be called once to process tasks.
 * This return pah_ret stands for a erorr code, normally it should been pah_success.
 *
 * If it returns pah_success, you can handle with some tasks according to current mode.
 *
 * Else if it returns pah_no_interrupt, it means that the device didn't raise INT1 interrupt yet.
 * Generally there is an extrinsic factor raising the interrupt which has nothing to do with the device.
 *
 * @return pah_success if the open operation was done successfully.
 *         pah_no_interrupt if the device didn't raise interrupt yet.
 *         pah_err_fifo_overflow if pah_task() was too late to be called.
 *         Otherwise to indicate an error has occurred.
 */
pah_ret     pah_task(void);


/**
 * @brief Access current fifo data.
 * 
 * This information is updated after a successful pah_task().
 *
 * @return Location where fifo data is stored.
 */
uint8_t*    pah_get_fifo_data(void);

/**
 * @brief Read current length of fifo data.
 * 
 * This information is updated after a successful pah_task().
 *
 * @return The length(in bytes) of fifo data.
 */
uint32_t    pah_fifo_data_num_per_ch(void);

/**
 * @brief Check if there are fifo data.
 * 
 * This information is updated after a successful pah_task().
 *
 * @return True if there are fifo data.
 */
bool        pah_has_fifo_data(void);

/**
 * @brief Check if the device is on touch.
 * 
 * This information is updated after a successful pah_task().
 *
 * @return True if the device is on touch.
 */
bool        pah_is_touched(void);


/**
 * @brief Set all values of fifo data to 0.
 *
 * @return None.
 */
void        pah_clear_fifo_data(void);


/**
 * @brief Set report fifo callback.
 *
 * @param[in]  fp_handler   The callback function to be called when pah_task() receives new fifo data.
 * @param[in]  user_data    The user data to be passed as parameter of callback function.
 * 
 * @return None.
 */
void        pah_set_report_fifo_callback(pah_report_fifo_handle fp_handler, void* user_data);


/**
 * @brief Get the specific I2C slave address of the device.
 * 
 * @return I2C slave address of the device.
 */
uint16_t    pah_get_i2c_slave_addr(void);


/**
 * @brief Enable or disable INT2 as touch flag.
 * 
 * Default is disabled.
 *
 * By default, touch/no-touch detection raises a pulse interrupt to INT1 which shared with FIFO interrupt.
 * If INT2 as touch flag is enabled, the device on touch causes INT2 pull-up, otherwise pull-down.
 *
 * @param[in]  enable   True to enable, otherwise to disable.
 * 
 * @return True if setting successfully.
 */
bool        pah_set_int2_as_touch_flag(bool enable);


/**
 * @brief Set report number per channel.
 *
 * The report number determines how many fifo data number the device collects, the device will raise a fifo interrupt.
 * For example, in PPG(20Hz) the device generates one data per 50ms, thus setting report number 20 makes the device raise fifo interrupts each 50ms*20 = 1000ms.
 *
 * @param[in]  report_sample_num_per_ch   The report number per channel.
 * 
 * @return None.
 */
void        pah_set_report_sample_num_per_ch(uint32_t report_sample_num_per_ch);

/**
 * @brief Get report number per channel.
 *
 * @return The report number per channel.
 */
uint32_t    pah_get_report_sample_num_per_ch(void);

/**
 * @brief Get the maximum report number per channel.
 * 
 * Due to the limit of buffer length, the number passing to pah_set_report_sample_num_per_ch() must be less than a maximum value.
 *
 * @return The maximum report number per channel.
 */
uint32_t    pah_get_max_report_sample_num_per_ch(void);

/**
 * @brief Get how many bytes per fifo sample.
 *
 * @return Bytes per fifo sample.
 */
uint32_t    pah_get_bytes_per_sample(void);


/**
 * @brief Check if the driver is valid to the device.
 * 
 * This function reads device registers directly.
 *
 * @return True if the driver is valid to the device.
 */
bool        pah_verify_product_id(void);

/**
 * @brief Check if the device is on touch.
 * 
 * This function reads device registers directly.
 *
 * @param[out]  is_touched   True if the device is on touch.
 *
 * @return pah_success if the open operation was done successfully.
 *         Otherwise to indicate an error has occurred.
 */
pah_ret     pah_read_touch_flag(bool *is_touched);


#endif  // header guard
