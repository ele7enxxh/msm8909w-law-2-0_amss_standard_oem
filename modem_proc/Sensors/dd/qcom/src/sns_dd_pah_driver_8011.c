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
  Edit History
  
  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.
  
  when        version  who       what, where, why
  ----------  ------   ---       -----------------------------------------------------------
  2016-07-07   1004	 bell	   - Move reg array settings to new source files.
  2016-07-01   1003    bell      - Fix undefined behaviors when the device wakes up from power down mode. Delay is necessary for the internal controller to reset the whole system and then the device is ready for the upcoming operations.
  2016-06-07   1002    bell      - Add functions: pah_set_mode(), pah_run_device().
                                 - Add enum: pah_device.
  2016-04-29   1001    bell      - Add PPG 200Hz modes.
                                 - Add helper functions: pah_is_ppg_mode(), pah_is_ppg_20hz_mode(), pah_fifo_data_num_per_ch().
                                 - Add pah_stop_mode
                                 - Remove pah_suspend_mode.
                                 - Fix setting pah_set_report_sample_num_per_ch() after enter_mode() causes bad behavior.
  2016-04-14   1000    bell      - Add version
  2016-04-12           bell      - Add license information and revision information
                                 - Modify to reduce the reaction time of touch/no-touch detect.
  2016-04-07           bell      Initial revision.
==============================================================================*/


#include "sns_dd_pah_driver.h"

// pah
#include "sns_dd_pah_driver_8011_reg_array.h"
#include "sns_dd_pah_comm.h"

// platform support
#include "sns_dd_pah_system_clock.h"
#include <stdio.h>
#include "fixed_point.h"

// c
//#include <string.h>
#include "sns_dd_pah_debug_log.h"
#include "qurt_elite_diag.h"  /* for mini-dm debug */

#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"



/*============================================================================
SOME DEFINITION
============================================================================*/

#define PAH_SET_BIT(var,pos)  do { ((var) |= (1 << pos)); }while(0)
#define PAH_CLEAR_BIT(var,pos)  do { ((var) &= ~(1 << pos)); }while(0)
#define PAH_CHECK_BIT(var,pos)  ((var >> pos) & 1)


#define PAH_DRIVER_8011_VERSION    1004

#define DEFAULT_REPORT_SAMPLE_NUM_PER_CH      20 //Max 398

#define MAX_SAMPLES_PER_READ      396
#define MAX_CH_NUM            3
#define BYTES_PER_SAMPLE        4

#define PPG_FRAME_INTERVAL_MS  60    // 50 * 120%
#define TOUCH_FRAME_INTERVAL_MS  1200  // 1000 * 120%

#define I2C_SLAVE_ID  0x15  //I2C 7-bit ID



#define VENDOR_DD_MSG_0(level,msg)              if(wdp_ppg_log) printf("PAH8011 %s\n",msg)

/*
#if ENABLE_DEBUG_TRACE || FREERTOS 
extern bool wdp_ppg_log;
#define debug_trace(msg)        if(wdp_ppg_log) printf("PAH8011_trace %s\n",msg)
#define debug_trace_1(msg, p1)      if(wdp_ppg_log) printf("PAH8011_trace %s %d\n",msg,p1)
#define debug_trace_2(msg, p1, p2)    if(wdp_ppg_log) printf("PAH8011_trace %s %d %d\n",msg,p1,p2)
#define debug_trace_3(msg, p1, p2, p3)  if(wdp_ppg_log) printf("PAH8011_trace %s %d %d %d \n",msg,p1,p2,p3)
#define debug_printf(msg)               if(wdp_ppg_log) printf("PAH8011 %s\n",msg)
#define debug_printf_1(msg, p1)         if(wdp_ppg_log) printf("PAH8011 %s %d\n",msg,p1)
#define debug_printf_2(msg, p1, p2)     if(wdp_ppg_log) printf("PAH8011 %s %d %d\n",msg,p1,p2)
#define debug_printf_3(msg, p1, p2, p3) if(wdp_ppg_log) printf("PAH8011 %s %d %d %d \n",msg,p1,p2,p3)

#else

#define debug_trace(msg)         MSG(MSG_SSID_SNS,DBG_MED_PRIO, " " msg)   
#define debug_trace_1(msg, p1)       MSG_1(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1)
#define debug_trace_2(msg, p1, p2)     MSG_2(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1,p2)
#define debug_trace_3(msg, p1, p2, p3)   MSG_3(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1,p2,p3)
#define debug_printf(msg)                MSG(MSG_SSID_SNS,DBG_MED_PRIO, " " msg)    
#define debug_printf_1(msg, p1)          MSG_1(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1)   
#define debug_printf_2(msg, p1, p2)      MSG_2(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1,p2)    
#define debug_printf_3(msg, p1, p2, p3)  MSG_3(MSG_SSID_SNS,DBG_MED_PRIO, " " msg,p1,p2,p3) 

#endif  // ENABLE_DEBUG_TRACE
*/

/*============================================================================
TYPE DEFINITIONS
============================================================================*/
typedef enum {

  pah_fifo_freq_none,
  pah_fifo_freq_20hz,
  pah_fifo_freq_200hz,

} pah_fifo_freq;


typedef struct {

  uint32_t        report_sample_num_per_ch;
  uint32_t        ch_num;

  uint8_t         is_int2_as_touch_flag;

} pah8011_property_s;


typedef struct {

  // fifo data
  uint8_t                 fifo_data[MAX_SAMPLES_PER_READ * BYTES_PER_SAMPLE];

  // update every task
  uint32_t                fifo_data_num_per_ch;

  // report ppg data
  void*                   user_data;
  pah_report_fifo_handle  fp_report_fifo_handler;

  // private members
  uint8_t                 has_started_tg;
  uint8_t                 has_started_ppg;
  uint8_t                 has_started_touch;
  uint8_t                 fifo_data_contain_touch_bit;

  // read only properties
  pah_mode                mode;
  uint8_t                 touch_flag;

  // properties
  pah8011_property_s      prop_curr;
  pah8011_property_s      prop_next;

} pah8011_state_s;


/*============================================================================
PRIVATE GLOBAL VARIABLES
============================================================================*/
static bool               g_init = false;
static pah8011_state_s    g_state;


/*============================================================================
PRIVATE FUNCTION PROTOTYPES
============================================================================*/
static bool             _pah8011_init(void);
static bool             _pah8011_shutdown(void);
static bool             _pah8011_startup(void);

static bool             _pah8011_start_tg(void);

static bool             _pah8011_start_ppg(void);
static bool             _pah8011_stop_ppg(void);
static bool             _pah8011_start_touch(void);
static bool             _pah8011_stop_touch(void);

static bool             _pah8011_clear_fifo_and_int_req(void);
static bool             _pah8011_update_report_num(void);
static bool             _pah8011_set_report_num(uint32_t samples_per_read);
static bool             _pah8011_cks(uint8_t *fifo, uint32_t samples_read, uint32_t cks_read);
static bool             _pah8011_read_touch_flag(uint8_t *touch_flag);

//// pure functions
static bool             _pah_is_ppg_mode(pah_mode mode);
static pah_fifo_freq    _pah_mode_to_freq(pah_mode mode);
static bool             _pah_is_diff_fifo_freq(pah_mode lhs_mode, pah_mode rhs_mode);


/*============================================================================
PUBLIC FUNCTION DEFINITIONS
============================================================================*/
bool pah_init(void)
{
  pah_deinit();

  debug_trace(">>>> pah_init() \n");

  memset(&g_state, 0, sizeof(g_state));
  g_state.mode = pah_stop_mode;

  // properties
  g_state.prop_curr.report_sample_num_per_ch = 0;
  g_state.prop_curr.ch_num = 2;
  g_state.prop_next.report_sample_num_per_ch = DEFAULT_REPORT_SAMPLE_NUM_PER_CH;
  g_state.prop_next.ch_num = 2;

  if (!pah_verify_product_id())
  {
    if (!_pah8011_startup())
      goto FAIL;
    if (!pah_verify_product_id())
      goto FAIL;
  }
  
  // register arrays
  if (!pah8011_reg_array_init())
    goto FAIL;

  g_init = true;

  debug_trace("<<<< pah_init() \n");
  return true;

FAIL:
  debug_printf("pah_init(). fail \n");
  return false;
}
void pah_deinit(void)
{
  if (!g_init)
    return;

  debug_trace(">>>> pah_deinit() \n");

  _pah8011_shutdown();

  g_init = false;

  debug_trace("<<<< pah_deinit() \n");
}

bool pah_enter_mode(pah_mode mode)
{
  if (!pah_set_mode(mode))
    goto FAIL;

  switch (mode)
  {
  case pah_touch_mode:
    if (!pah_run_device(pah_device_touch, true))
      goto FAIL;
    break;

  case pah_ppg_mode:
  case pah_ppg_200hz_mode:
    if (!pah_run_device(pah_device_ppg, true))
      goto FAIL;
    break;

  case pah_ppg_touch_mode:
  case pah_ppg_200hz_touch_mode:
    if (!pah_run_device(pah_device_ppg, true))
      goto FAIL;
    if (!pah_run_device(pah_device_touch, true))
      goto FAIL;
    break;

  default:
    break;
  }

  return true;

FAIL:
  debug_printf("pah_enter_mode(). fail \n");
  return false;
}

bool pah_set_mode(pah_mode mode)
{
  debug_printf_2(">>>> pah_set_mode(), mode = %d -> %d \n", g_state.mode, mode);

  if (g_state.mode == mode)
  {
    debug_trace("<<<< pah_set_mode() \n");
    return true;
  }

  if (g_state.mode == pah_stop_mode)
  {
    if (!_pah8011_startup())
      goto FAIL;
    if (!_pah8011_init())
      goto FAIL;

    // update properties
    if (!_pah8011_update_report_num())
      goto FAIL;
    g_state.prop_curr.is_int2_as_touch_flag = g_state.prop_next.is_int2_as_touch_flag;
  }

  if (pah_stop_mode == mode)
  {
    if (!_pah8011_shutdown())
      goto FAIL;
  }
  else
  {
    switch (mode)
    {
    case pah_touch_mode:
    {
      _pah8011_stop_ppg();

      if (_pah_is_ppg_mode(g_state.mode) && !_pah8011_clear_fifo_and_int_req())
        goto FAIL;

      if (!pah8011_reg_array_load_mode(pah8011_reg_array_mode_touch))
        goto FAIL;
    }
    break;

    case pah_ppg_mode:
    {
      _pah8011_stop_touch();

      if (_pah_is_diff_fifo_freq(g_state.mode, mode))
      {
        if (!_pah8011_stop_ppg())
          goto FAIL;
        if (!_pah8011_clear_fifo_and_int_req())
          goto FAIL;
      }

      if (!pah8011_reg_array_load_mode(pah8011_reg_array_mode_ppg_20hz))
        goto FAIL;
      if (!_pah8011_update_report_num())
        goto FAIL;
    }
    break;

    case pah_ppg_200hz_mode:
    {
      _pah8011_stop_touch();

      if (_pah_is_diff_fifo_freq(g_state.mode, mode))
      {
        if (!_pah8011_stop_ppg())
          goto FAIL;
        if (!_pah8011_clear_fifo_and_int_req())
          goto FAIL;
      }

      if (!pah8011_reg_array_load_mode(pah8011_reg_array_mode_ppg_200hz))
        goto FAIL;
      if (!_pah8011_update_report_num())
        goto FAIL;
    }
    break;

    case pah_ppg_touch_mode:
    {
      if (_pah_is_diff_fifo_freq(g_state.mode, mode))
      {
        if (!_pah8011_stop_ppg())
          goto FAIL;
        if (!_pah8011_clear_fifo_and_int_req())
          goto FAIL;
      }

      if  (!pah8011_reg_array_load_mode(pah8011_reg_array_mode_ppg_20hz))
        goto FAIL;
      if (!_pah8011_update_report_num())
        goto FAIL;
    }
    break;

    case pah_ppg_200hz_touch_mode:
    {
      if (_pah_is_diff_fifo_freq(g_state.mode, mode))
      {
        if (!_pah8011_stop_ppg())
          goto FAIL;
        if (!_pah8011_clear_fifo_and_int_req())
          goto FAIL;
      }

      if (!pah8011_reg_array_load_mode(pah8011_reg_array_mode_ppg_200hz))
        goto FAIL;
      if (!_pah8011_update_report_num())
        goto FAIL;
    }
    break;

    default:
      goto FAIL;
    }

    // update flag
    if (!pah_comm_write(0x7F, 0x01))    //bank1
      goto FAIL;
    if (!pah_comm_write(0x24, 0x01))
      goto FAIL;
  }

  g_state.mode = mode;

  debug_trace("<<<< pah_set_mode() \n");
  return true;

FAIL:
  debug_printf("pah_set_mode(). fail \n");
  return false;
}

bool pah_run_device(pah_device device, bool enable)
{
  debug_printf_2(">>>> pah_run_device(), device = %d, enable = %d \n", device, enable);

  switch (device)
  {
  case pah_device_ppg:
    if (enable)
    {
      if (!_pah8011_update_report_num())
        goto FAIL;
      if (!_pah8011_start_ppg())
        goto FAIL;
    }
    else
    {
      if (!_pah8011_stop_ppg())
        goto FAIL;
    }
    break;

  case pah_device_touch:
    if (enable)
    {
      if (!_pah8011_start_touch())
        goto FAIL;
    }
    else
    {
      if (!_pah8011_stop_touch())
        goto FAIL;
    }
    break;

  default:
    goto FAIL;
  }

  // update flag
  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;
  if (!pah_comm_write(0x24, 0x01))
    goto FAIL;

  if (enable)
  {
    if (!_pah8011_start_tg())
      goto FAIL;
  }

  debug_trace("<<<< pah_run_device() \n");
  return true;

FAIL:
  debug_printf("pah_run_device(). fail \n");
  return false;
}

pah_mode pah_query_mode(void)
{
  return g_state.mode;
}

bool pah_is_ppg_mode(void)
{
  return _pah_is_ppg_mode(g_state.mode);
}

bool pah_is_ppg_20hz_mode(void)
{
  return g_state.mode == pah_ppg_mode
    || g_state.mode == pah_ppg_touch_mode;
}

pah_ret pah_task(void)
{
  pah_ret result = pah_err_unknown;
  uint8_t int_req = 0;

  debug_trace(">>>> pah_task() \n");

  // reset
  g_state.fifo_data_num_per_ch = 0;

  // read interrupt
  if (!pah_comm_write(0x7F, 0x02))    //Bank 2
  {
    debug_printf("pah_task(). read int_req fail a \n");
    result = pah_err_comm_fail;
    goto FAIL;
  }
  else if (!pah_comm_read(0x1B, &int_req))    //read interrupt status
  {
    debug_printf("pah_task(). read int_req fail b \n");
    result = pah_err_comm_fail;
    goto FAIL;
  }
  else
  {
    debug_printf_1("pah_task(). int_req = %d \n", int_req);

    // no interrupt
    if (int_req == 0)
    {
      // ignore
      debug_trace("<<<< pah_task() \n");
      return pah_no_interrupt;
    }

    // fifo overflow
    if ((int_req & 0x04) != 0)
    {
      // Usually happens when pah_task() was too late to be called.
      // Troubleshooting: Record timestamps before every pah_task() calls, the interval of 
      // different pah_task() calls should be regular.
      
      debug_printf("pah_task(). fifo overflow \n");
      result = pah_err_fifo_overflow;
      goto FAIL;
    }

    // underflow interrupt
    if ((int_req & 0x08) != 0)
    {
      debug_printf("pah_task(). fifo underflow \n");
      result = pah_err_fifo_underflow;
      goto FAIL;
    }

    // fifo interrupt
    if ((int_req & 0x01) != 0)
    {
      uint32_t samples_per_read = 0;
      uint32_t channel_num = 0;
      uint16_t fifo_data_number = 0;

      if (g_state.has_started_ppg)
        channel_num += 2;

      if (channel_num == 0)
      {
        debug_printf("pah_task(). channel_num should never be 0 \n");
        result = pah_err_invalid_program;
        goto FAIL;
      }

      samples_per_read = g_state.prop_curr.report_sample_num_per_ch * channel_num;


      if (!pah_comm_write(0x7F, 0x02))    //Bank 2
      {
        result = pah_err_comm_fail;
        goto FAIL;
      }
      if (!pah_comm_burst_read(0x25, (uint8_t*)&fifo_data_number, 2))
      {
        result = pah_err_comm_fail;
        goto FAIL;
      }
      if (fifo_data_number < samples_per_read)
      {
        if (!pah_comm_write(0x1b, int_req)) //clear interrupt
        {
          debug_printf("pah_task(). clear int_req fail b\n");
          result = pah_err_comm_fail;
          goto FAIL;
        }

        return pah_no_interrupt;
      }

      // read fifo
      if (!pah_comm_write(0x7F, 0x03))    //Bank 3
      {
        debug_printf("pah_task(). pah_comm_write(0x7F, 0x03) fail \n");
        result = pah_err_comm_fail;
        goto FAIL;
      }
      else if (!pah_comm_burst_read(0, g_state.fifo_data, samples_per_read * 4))  //read FIFO
      {
        debug_printf("pah_task(). pah_comm_burst_read(ppg_data) fail \n");
        result = pah_err_comm_fail;
        goto FAIL;
      }
      else
      {
        uint8_t cks[8] = { 0 };
        uint32_t *s = (uint32_t *)g_state.fifo_data;

        // update touch_flag
        if (g_state.fifo_data_contain_touch_bit)
        {
          g_state.touch_flag = s[(samples_per_read - 1)] & 0x01;
        }
        else
        {
          if (!_pah8011_read_touch_flag(&g_state.touch_flag))
          {
            result = pah_err_comm_fail;
            goto FAIL;
          }
        }

        // read checksum
        if (!pah_comm_write(0x7F, 0x02))
        {
          debug_printf("pah_task(). pah_comm_write(0x7F, 0x02) fail \n");
          result = pah_err_comm_fail;
          goto FAIL;
        }
        else if (!pah_comm_burst_read(28, cks, 4))  //checksum
        {
          debug_printf("pah_task(). pah_comm_burst_read(checksum) fail \n");
          result = pah_err_comm_fail;
          goto FAIL;
        }
        else
        {
          if (_pah8011_cks(g_state.fifo_data, samples_per_read, *(uint32_t *)cks))
          {
            // report fifo
            if (g_state.fp_report_fifo_handler)
            {
              pah_report_fifo fifo;
              fifo.data = g_state.fifo_data;
              fifo.touch_flag = g_state.touch_flag;
              g_state.fp_report_fifo_handler(g_state.user_data, &fifo);
            }

            g_state.fifo_data_num_per_ch = g_state.prop_curr.report_sample_num_per_ch;
            result = pah_success;
          }
          else
          {
            debug_printf("pah_task(). _pah8011_cks fail\n");
            result = pah_err_fifo_checksum_fail;
            goto FAIL;
          }
        }
      }
    }

    // touch interrupt
    if ((int_req & 0x02) != 0)
    {
      if (!_pah8011_read_touch_flag(&g_state.touch_flag))
      {
        result = pah_err_comm_fail;
        goto FAIL;
      }

      result = pah_success;
    }

    // clear interrupt
    if (!pah_comm_write(0x7F, 0x02))    //Bank 2
    {
      debug_printf("pah_task(). clear int_req fail a\n");
      result = pah_err_comm_fail;
      goto FAIL;
    }
    if (!pah_comm_write(0x1b, int_req)) //clear interrupt
    {
      debug_printf("pah_task(). clear int_req fail b\n");
      result = pah_err_comm_fail;
      goto FAIL;
    }
  }

  // update properties
  if (!_pah8011_update_report_num())
  {
    result = pah_err_comm_fail;
    goto FAIL;
  }

  debug_trace("<<<< pah_task() \n");
  return result;

FAIL:
  debug_printf_1("pah_task(). fail, result = %d \n", result);
  return result;
}

uint8_t* pah_get_fifo_data(void)
{
  return g_state.fifo_data;
}

uint32_t pah_fifo_data_num_per_ch(void)
{
  return g_state.fifo_data_num_per_ch;
}

bool pah_has_fifo_data(void)
{
  return g_state.fifo_data_num_per_ch > 0;
}

bool pah_is_touched(void)
{
  return g_state.touch_flag > 0;
}

void pah_clear_fifo_data(void)
{
  memset(g_state.fifo_data, 0, sizeof(g_state.fifo_data));
}

void pah_set_report_fifo_callback(pah_report_fifo_handle fp_handler, void* user_data)
{
  debug_trace("==== pah_set_report_fifo_callback() \n");

  g_state.fp_report_fifo_handler = fp_handler;
  g_state.user_data = user_data;
}

uint16_t pah_get_i2c_slave_addr(void)
{
  debug_trace("==== pah_get_i2c_slave_addr() \n");

  return I2C_SLAVE_ID;
}

bool pah_set_int2_as_touch_flag(bool enable)
{
  debug_trace("==== pah_set_int2_as_touch_flag() \n");
  debug_printf_1("pah_set_int2_as_touch_flag(). enable = %d \n", enable);

  g_state.prop_next.is_int2_as_touch_flag = enable;
  return true;
}

void pah_set_report_sample_num_per_ch(uint32_t report_sample_num_per_ch)
{
  static const uint32_t MIN_REPORT_SAMPLE_NUM_PER_CH = 1;
  const uint32_t MAX_REPORT_SAMPLE_NUM_PER_CH = pah_get_max_report_sample_num_per_ch();

  if (report_sample_num_per_ch < MIN_REPORT_SAMPLE_NUM_PER_CH)
    report_sample_num_per_ch = MIN_REPORT_SAMPLE_NUM_PER_CH;
  else if (report_sample_num_per_ch > MAX_REPORT_SAMPLE_NUM_PER_CH)
    report_sample_num_per_ch = MAX_REPORT_SAMPLE_NUM_PER_CH;
  g_state.prop_next.report_sample_num_per_ch = report_sample_num_per_ch;

  debug_printf_1("pah_set_report_sample_num_per_ch(). report_sample_num_per_ch = %d \n", report_sample_num_per_ch);
}
uint32_t pah_get_report_sample_num_per_ch(void)
{
  return g_state.prop_curr.report_sample_num_per_ch;
}
uint32_t pah_get_max_report_sample_num_per_ch(void)
{
  return MAX_SAMPLES_PER_READ / MAX_CH_NUM;
}

uint32_t pah_get_bytes_per_sample(void)
{
  return BYTES_PER_SAMPLE;
}

bool pah_verify_product_id(void)
{
  uint8_t data = 0;

  debug_trace(">>>> pah_verify_product_id() \n");

  if (!pah_comm_write(0x7F, 0x00))    //bank0
    goto FAIL;
  if (!pah_comm_read(0x00, &data))
    goto FAIL;
  if (data != 0x11)
  {
    debug_printf_1("pah_verify_product_id() fail. data = %d \n", data);
    goto FAIL;
  }

  debug_trace("<<<< pah_verify_product_id() \n");
  return true;

FAIL:
  debug_printf("pah_verify_product_id error \n");
  return false;
}

pah_ret pah_read_touch_flag(bool *is_touched)
{
  uint8_t touch_flag = 0;

  debug_trace(">>>> pah_read_touch_flag() \n");

  if (!is_touched)
  {
    debug_printf("pah_read_touch_flag() fail. invalid argument \n");
    return pah_err_invalid_argument;
  }

  if (!_pah8011_read_touch_flag(&touch_flag))
    return pah_err_comm_fail;

  *is_touched = touch_flag > 0;

  debug_trace_1("<<<< pah_read_touch_flag(). is_touched = %d \n", *is_touched);
  return pah_success;
}

/*============================================================================
PRIVATE FUNCTION DEFINITIONS
============================================================================*/
static bool _pah8011_init(void)
{
  uint8_t data = 0;

  debug_printf_1("_pah8011_init(), PAH8011 driver[v%d] \n", PAH_DRIVER_8011_VERSION);

  //// reg init settings
  pah8011_reg_array_load_init();

  //// other init settings
  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  // disable ch a
  if (!pah_comm_read(0x21, &data))
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);
  if (!pah_comm_write(0x21, data))
    goto FAIL;

  // disable ch b
  if (!pah_comm_read(0x23, &data))
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);
  if (!pah_comm_write(0x23, data))
    goto FAIL;

  // disable ch c
  if (!pah_comm_read(0x25, &data))
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);
  if (!pah_comm_write(0x25, data))
    goto FAIL;

  // touch interrupt
  if (!pah_comm_read(0x36, &data))
    goto FAIL;
  PAH_SET_BIT(data, 0);
  PAH_CLEAR_BIT(data, 1);
  if (!pah_comm_write(0x36, data))
    goto FAIL;

  // mask
  if (!pah_comm_read(0x37, &data))
    goto FAIL;
  PAH_SET_BIT(data, 1);
  if (!pah_comm_write(0x37, data))
    goto FAIL;

  // touch bit
  if (!pah_comm_read(0x5A, &data))
    goto FAIL;
  g_state.fifo_data_contain_touch_bit = PAH_CHECK_BIT(data, 0);

  // update flag
  if (!pah_comm_write(0x24, 0x01))
    goto FAIL;

  return true;

FAIL:
  debug_printf("_pah8011_init fail \n");
  return false;
}

static bool _pah8011_shutdown(void)
{
  debug_printf("_pah8011_shutdown() \n");

  if (!pah_comm_write(0x7F, 0x04))    //bank4
    goto FAIL;
  if (!pah_comm_write(0x69, 0x01))
    goto FAIL;

  // reset states
  g_state.has_started_tg = 0;
  g_state.has_started_ppg = 0;
  g_state.has_started_touch = 0;

  // reset properties
  g_state.prop_curr.report_sample_num_per_ch = 0;

  return true;

FAIL:
  debug_printf("_pah8011_shutdown error \n");
  return false;
}

static bool _pah8011_startup(void)
{
  debug_printf("_pah8011_startup() \n");

  if (!pah_comm_write(0x7F, 0x04))    //bank4
    goto FAIL;
  if (!pah_comm_write(0x69, 0x00))
    goto FAIL;

  // When the device wakes up from power down mode, delay is necessary for the internal controller to reset the whole system and then the device is ready for the upcoming operations.
  // Without delay, the driver would be unable to write settings correctly.
  delay_ms(2);

  return true;

FAIL:
  debug_printf("_pah8011_startup error \n");
  return false;
}

static bool _pah8011_start_tg(void)
{
  uint8_t data = 0;

  if (g_state.has_started_tg)
    return true;

  debug_printf("_pah8011_start_tg() \n");

  // must sleep between update flag and TG enable.
  delay_ms(1);

  // mask
  if (!pah_comm_write(0x7f, 0x01))
    goto FAIL;
  if (!pah_comm_read(0x37, &data))    //mask
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);             // IntSramFIFO_MaskInt
  PAH_CLEAR_BIT(data, 2);             // int_sramfifo_overflow_mask
  PAH_CLEAR_BIT(data, 3);             // int_sramfifo_underflow_mask
  if (!pah_comm_write(0x37, data))
    goto FAIL;

  // TG enable
  if (!pah_comm_write(0x30, 0x01))    //TG enable
    goto FAIL;

  // wait for clk
  delay_ms(1);

  // clear int_req
  if (!pah_comm_write(0x7F, 0x02))    //Bank 2
    goto FAIL;
  if (!pah_comm_write(0x1B, 0xFF))    //int_req_array
    goto FAIL;

  // mask
  if (!pah_comm_write(0x7f, 0x01))
    goto FAIL;
  if (!pah_comm_read(0x37, &data))    //mask
    goto FAIL;
  PAH_CLEAR_BIT(data, 4);             // AllInt_MaskInt
  if (!pah_comm_write(0x37, data))
    goto FAIL;

  g_state.has_started_tg = 1;
  return true;

FAIL:
  debug_printf("_pah8011_start_tg error \n");
  return false;
}

static bool _pah8011_start_ppg(void)
{
  uint8_t data = 0;

  if (g_state.has_started_ppg)
    return true;

  debug_printf("_pah8011_start_ppg() \n");

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  //channel b
  if (!pah_comm_read(0x23, &data))
    goto FAIL;
  PAH_SET_BIT(data, 0);
  if (!pah_comm_write(0x23, data))
    goto FAIL;

  //channel c
  if (!pah_comm_read(0x25, &data))
    goto FAIL;
  PAH_SET_BIT(data, 0);
  if (!pah_comm_write(0x25, data))
    goto FAIL;

  //For Auto Clear function
  //data = data >> 1;
  //if(!pah_comm_write(88, data))
  //{
  //    goto FAIL;
  //}

  // update flag
  //if (!pah_comm_write(0x24, 0x01))
  //    goto FAIL;

  g_state.has_started_ppg = 1;
  return true;

FAIL:
  debug_printf("_pah8011_start_ppg error \n");
  return false;
}

static bool _pah8011_stop_ppg(void)
{
  uint8_t data = 0;

  if (!g_state.has_started_ppg)
    return true;

  debug_printf("_pah8011_stop_ppg() \n");

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  //channel b
  if (!pah_comm_read(0x23, &data))
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);
  if (!pah_comm_write(0x23, data))
    goto FAIL;

  //channel c
  if (!pah_comm_read(0x25, &data))
    goto FAIL;
  PAH_CLEAR_BIT(data, 0);
  if (!pah_comm_write(0x25, data))
    goto FAIL;

  //For Auto Clear function
  //data = data >> 1;
  //if(!pah_comm_write(88, data))
  //    goto FAIL;

  // update flag
  //if (!pah_comm_write(0x24, 0x01))
  //    goto FAIL;

  g_state.has_started_ppg = 0;
  return true;
FAIL:
  debug_printf("_pah8011_stop_ppg error \n");
  return false;
}

static bool _pah8011_start_touch(void)
{
  uint8_t data = 0;

  if (g_state.has_started_touch)
    return true;

  debug_printf("_pah8011_start_touch() \n");

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  // 0x36
  if (!pah_comm_read(0x36, &data))
    goto FAIL;
  if (g_state.prop_curr.is_int2_as_touch_flag)
  {
    PAH_CLEAR_BIT(data, 0);
    PAH_SET_BIT(data, 1);
  }
  else
  {
    PAH_SET_BIT(data, 0);
    PAH_CLEAR_BIT(data, 1);
  }
  if (!pah_comm_write(0x36, data))
    goto FAIL;

  // 0x37
  if (!pah_comm_read(0x37, &data))
    goto FAIL;
  if (g_state.prop_curr.is_int2_as_touch_flag)
  {
    PAH_SET_BIT(data, 1);
  }
  else
  {
    PAH_CLEAR_BIT(data, 1);
  }
  if (!pah_comm_write(0x37, data))
    goto FAIL;

  // update flag
  //if (!pah_comm_write(0x24, 0x01))
  //    goto FAIL;

  g_state.has_started_touch = 1;
  return true;

FAIL:
  debug_printf("_pah8011_start_touch error \n");
  return false;
}
static bool _pah8011_stop_touch(void)
{
  uint8_t data = 0;

  if (!g_state.has_started_touch)
    return true;

  debug_printf("_pah8011_stop_touch() \n");

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  // 0x36
  if (!pah_comm_read(0x36, &data))
    goto FAIL;
  PAH_SET_BIT(data, 0);
  PAH_CLEAR_BIT(data, 1);
  if (!pah_comm_write(0x36, data))
    goto FAIL;

  // 0x37
  if (!pah_comm_read(0x37, &data))
    goto FAIL;
  if (g_state.prop_curr.is_int2_as_touch_flag)
  {
    // empty
  }
  else
  {
    PAH_SET_BIT(data, 1);
  }
  if (!pah_comm_write(0x37, data))
    goto FAIL;

  // update flag
  //if (!pah_comm_write(0x24, 0x01))
  //    goto FAIL;

  g_state.has_started_touch = 0;
  return true;

FAIL:
  debug_printf("_pah8011_stop_touch error \n");
  return false;
}

static bool _pah8011_clear_fifo_and_int_req(void)
{
  uint8_t int_req = 0;

  debug_printf("_pah8011_clear_fifo_and_int_req() \n");

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  // clear FIFO
  if (!pah_comm_write(0x70, 0x01))    // clear FIFO
    goto FAIL;
  if (!pah_comm_write(0x24, 0x01))    // update flag
    goto FAIL;
  delay_ms(PPG_FRAME_INTERVAL_MS);
  if (!pah_comm_write(0x70, 0x00))    // clear FIFO
    goto FAIL;
  if (!pah_comm_write(0x24, 0x01))    // update flag
    goto FAIL;

  //clear interrupt
  if (!pah_comm_write(0x7F, 0x02))    //Bank 2
    goto FAIL;
  if (!pah_comm_read(0x1B, &int_req)) //read interrupt status
    goto FAIL;
  if (int_req > 0)
  {
    if (!pah_comm_write(0x1B, int_req)) //clear interrupt
      goto FAIL;
  }

  return true;

FAIL:
  debug_printf("_pah8011_clear_fifo_and_int_req() error \n");
  return false;
}

static bool _pah8011_update_report_num(void)
{
  if (g_state.prop_curr.report_sample_num_per_ch != g_state.prop_next.report_sample_num_per_ch)
  {
    debug_printf_2("_pah8011_update_report_num(). report_sample_num_per_ch %d -> %d \n", g_state.prop_curr.report_sample_num_per_ch, g_state.prop_next.report_sample_num_per_ch);

    if (!_pah8011_set_report_num(g_state.prop_next.report_sample_num_per_ch * g_state.prop_curr.ch_num))
      goto FAIL;
    g_state.prop_curr.report_sample_num_per_ch = g_state.prop_next.report_sample_num_per_ch;
  }

  return true;

FAIL:
  debug_printf("_pah8011_update_report_num() fail \n");
  return false;
}

static bool _pah8011_set_report_num(uint32_t samples_per_read)
{
  uint8_t data = 0;
  uint32_t samples_per_read_plus1 = samples_per_read + 1;

  debug_trace_1("_pah8011_set_report_num(). samples_per_read_plus1 = %d \n", samples_per_read_plus1);

  if (!pah_comm_write(0x7F, 0x01))    //bank1
    goto FAIL;

  //Rpt num. write MSB first.
  data = (((samples_per_read_plus1) >> 8) & 0x01);
  if (!pah_comm_write(87, data))
    goto FAIL;

  //Rpt num. write LSB last.
  data = ((samples_per_read_plus1)& 0xFF);
  if (!pah_comm_write(86, data))
    goto FAIL;

  // update flag
  if (!pah_comm_write(0x24, 0x01))
    goto FAIL;

  return true;

FAIL:
  debug_printf("_pah8011_set_report_num() fail \n");
  return false;
}

static bool _pah8011_cks(uint8_t *fifo, uint32_t samples_read, uint32_t cks_read)
{
  uint32_t *s = (uint32_t *)fifo;
  uint32_t raw;
  uint32_t cks_cal = 0;
  uint32_t i;

  //checksum compare
  for (i = 0; i < samples_read; i++)
  {
    raw = *(s);
    cks_cal = cks_cal ^ raw;
    s++;
  }

  if (cks_cal != cks_read)
  {
    s = (uint32_t *)fifo;

    debug_printf("fifo = { \n");
    for (i = 0; i < samples_read; i++)
    {
      raw = *(s);
      debug_printf_1("%X, ", raw);
      s++;
    }
    debug_printf("} \n");
    debug_printf_2("cks_cal == %X, cks_read == %X \n", cks_cal, cks_read);

    return false;
  }
  return true;
}

static bool _pah8011_read_touch_flag(uint8_t *touch_flag)
{
  uint8_t data = 0;

  if (!touch_flag)
    goto FAIL;

  if (!pah_comm_write(0x7F, 0x02))    //Bank 2
    goto FAIL;
  if (!pah_comm_read(0x00, &data))
    goto FAIL;

  *touch_flag = data & 0x01;

  return true;

FAIL:
  debug_printf("_pah8011_read_touch_flag() fail \n");
  return false;
}


/*============================================================================
Pure functions
============================================================================*/
static bool _pah_is_ppg_mode(pah_mode mode)
{
  return mode == pah_ppg_mode
    || mode == pah_ppg_200hz_mode
    || mode == pah_ppg_touch_mode
    || mode == pah_ppg_200hz_touch_mode;
}

static pah_fifo_freq _pah_mode_to_freq(pah_mode mode)
{
  pah_fifo_freq result = pah_fifo_freq_none;
  switch (mode)
  {
  case pah_ppg_mode:
  case pah_ppg_touch_mode:
    result = pah_fifo_freq_20hz;
    break;

  case pah_ppg_200hz_mode:
  case pah_ppg_200hz_touch_mode:
    result = pah_fifo_freq_200hz;
    break;

  default:
    result = pah_fifo_freq_none;
    break;
  }
  return result;
}

static bool _pah_is_diff_fifo_freq(pah_mode lhs_mode, pah_mode rhs_mode)
{
  pah_fifo_freq lhs_freq = _pah_mode_to_freq(lhs_mode);
  pah_fifo_freq rhs_freq = _pah_mode_to_freq(rhs_mode);

  if (lhs_freq == pah_fifo_freq_none
    || rhs_freq == pah_fifo_freq_none)
    return false;

  return lhs_freq != rhs_freq;
}
