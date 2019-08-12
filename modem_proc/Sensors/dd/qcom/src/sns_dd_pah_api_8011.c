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
* when        version  who       what, where, why
* ----------  ------   ---       -----------------------------------------------------------
* 2016-07-28   1007    bell      - Fix timestamp in reported data was overflow. Change uint32_t to uint64_t.
* 2016-07-12   1006    bell      - Fix wrong return values.
*                                - Fix skip invalid samples initialization. Add the initialization in set_device_mode() function.
* 2016-06-29   1005    bell      - Fix in enable_sched_data(), deregistering interrupt while the device was still running 
*                                  causes the device missed a handle_irq() to handle and then no long raised interrupts.
* 2016-06-07   1004    bell      - Remove directly enter pah mode in enable_sched_data(). Change to set pah modes in set_attrib() and start/stop device in enable_sched_data().
*                                  So that enable_sched_data() no longer resets the condition to discard initial samples.
*                                - Discard initial samples based on duration(in milliseconds) instead of based on number of samples.
*                                - Refactor codes. Add dd_sub_dev_report_samples_realloc() to simplify.
*                                - Remove test code.
* 2016-05-04   1003    bell      - Remove FX_FLTTOFIX_Q16 conversion while report fifo data.
* 2016-04-29   1002    bell      - Replace pah_suspend_mode with pah_stop_mode.
*                                - Add odr==200 availability on PPG subdevice.
*                                - Add a report once Touch subdevice enabled.
* 2016-04-21   1001    bell      Fix timestamp of first data was wrong.
* 2016-04-14   1000    bell      - Add version.
*                                - Reject invalid initial samples.
*                                - Reject invalid watermark at set_attrib().
*                                - Edit log.
* 2016-04-13           bell      - Add log_pkt_pah_data() to log data/timestamp.
*                                - Add 2 subdevices can also share the same interrupt.
* 2016-04-12           bell      Add license information and revision information.
* 2016-04-07           bell      Initial revision.
==============================================================================*/


/*============================================================================
INCLUDE FILES
============================================================================*/

#include "sns_dd_pah_api_8011.h"


// platform
#include "sns_dd_pah_debug_log.h"
#include "sns_dd_pah_i2c.h"
#include "sns_dd_pah_system_clock.h"
#include <sns_log_api_public.h>

// pah driver
#include "sns_dd_pah_driver.h"
#include <stdio.h>
#include "fixed_point.h"

// c
//#include <string.h>

/*============================================================================
SOME DEFINITION
============================================================================*/
#define ADSP_DRIVER_VERSION      1007999

enum
{
  DD_NUM_SUB_DEV = 2,
  DD_NUM_SENSORS_PER_SUB_DEV = 2,
};

#define DD_HANDLE_ALIGN     3


typedef struct int_gpio_state_t int_gpio_state_t;
struct int_gpio_state_t {

  int_gpio_state_t*               next;

  uint16_t                        interrupt_gpio;
  sns_ddf_signal_irq_trigger_e    irq_trigger_mode;

  // interrupt
  bool                            is_interrupt_enabled;

};


typedef struct {

  int_gpio_state_t          *int_gpio_state;
  sns_ddf_handle_t          smgr_handle;

  sns_ddf_sensor_e          sensor_types[DD_NUM_SENSORS_PER_SUB_DEV];
  uint32_t                  sensor_num;

  sns_ddf_powerstate_e      powerstate;
  uint32_t                  odr;
  uint16_t                  water_mark;

  // notify sample
  sns_ddf_time_t            prev_timestamp;
  sns_ddf_sensor_data_s     report_sensor_data;

  // is enable sched data
  bool                      enable_sched;

  //// other state
  // discard initial invalid sample
  bool                      is_sample_ready;
  uint32_t                  discard_sample_duration;

} sub_dev_t;


typedef struct {

  sns_ddf_dev_list_t        next;

  int_gpio_state_t          *int_gpio_list_head;

  sns_ddf_port_id_s         port_id;
  sns_ddf_handle_t          port_handle;  // I2C bus handle

  // sub devices
  sub_dev_t                 sub_dev[DD_NUM_SUB_DEV];
  uint32_t                  sub_dev_count;

  // are sub_dev[0] and sub_dev[1] share the same interrupt pin
  bool                      is_shared_int1;

} state_t;

typedef struct
{
  sns_ddf_dev_list_t        next; /* the next device instance */
  sns_ddf_driver_info_s     dd_info; /* the driver info */
} sns_dd_head_s;


/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/
static sns_dd_head_s g_dd_head = { NULL,{ "dd_name", 1 } };

static const uint32_t g_sub_dev_id_ppg = 0;
static const uint32_t g_sub_dev_id_touch = 1;
//static const uint32_t g_sub_dev_id_touch = 0;
//static const uint32_t g_sub_dev_id_ppg = 1;

static const uint32_t SUB_DEV_ODR_SUPPORT_LIST_PPG[] = { 20, 200 };
static const uint32_t SUB_DEV_ODR_SUPPORT_LIST_TOUCH[] = { 1 };


#define PPG_RESOLUTION 1
#define PPG_MAX_RANGE (300)


#define DISCARD_SAMPLE_DURATION_MS     500  // in milliseconds


/*============================================================================
GLOBAL VARIABLE DEFINITIONS
============================================================================*/
// #define sns_dd_pah8011_if sns_dd_vendor_if_1
sns_ddf_driver_if_s sns_dd_pah8011_if =
{
  &sns_dd_pah_init,
  &sns_dd_pah_get_data,
  &sns_dd_pah_set_attrib,
  &sns_dd_pah_get_attrib,
  &sns_dd_pah_handle_timer,
  &sns_dd_pah_handle_irq,
  &sns_dd_pah_reset,
  &sns_dd_pah_run_test,
  &sns_dd_pah_enable_sched_data,
  &sns_dd_pah_probe,
  &sns_dd_pah_trigger_fifo_data
};


/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/
static sns_ddf_status_e set_device_mode(state_t* state, uint32_t sub_dev_id);
static sns_ddf_status_e enable_interrupt(sub_dev_t *sub_dev, sns_ddf_handle_t dd_handle, bool enable);
static void dd_destroy_state(state_t* state);
static void report_fifo_data(sub_dev_t *sub_dev, sns_ddf_time_t timestamp, uint8_t *fifo_data, uint32_t sample_num_per_ch, bool is_touched);
static void report_touch_detect(sub_dev_t *sub_dev, sns_ddf_time_t timestamp, bool is_touched);
static void log_pkt_pah_data(sns_ddf_sensor_data_s *sensor_data);

static uint32_t convert_tick_count_to_millisec(uint32_t tick_count);


//// create/destroy
static sns_ddf_status_e dd_create_state(
  /* const */ sns_ddf_device_access_s*  device_info,  // in
  state_t**                dd_state    // out
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  state_t* state = NULL;

  // alloc state
  ret = sns_ddf_malloc((void **)&state, sizeof(*state));
  if (ret != SNS_DDF_SUCCESS)
  {
    VENDOR_DD_MSG_0(ERROR, "dd_create_state() fail to alloc state_t");
    goto FAIL;
  }
  memset(state, 0, sizeof(*state));

  // alloc dummy head of gpio list
  ret = sns_ddf_malloc((void **)&state->int_gpio_list_head, sizeof(*state->int_gpio_list_head));
  if (ret != SNS_DDF_SUCCESS)
  {
    VENDOR_DD_MSG_0(ERROR, "dd_create_state() fail to alloc gpio_state_t");
    goto FAIL;
  }

  memset(state->int_gpio_list_head, 0, sizeof(*state->int_gpio_list_head));

  // dummy value
  state->int_gpio_list_head->interrupt_gpio = UINT16_MAX;

  // skip this function if API not available
  sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

  // open I2C port
  ret = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
  if (ret != SNS_DDF_SUCCESS)
  {
    //sns_ddf_mfree(state->report_sensor_data.samples);
    sns_ddf_mfree(state);
    VENDOR_DD_MSG_0(ERROR, "I2C open failed");
    goto FAIL;
  }

  // result
  *dd_state = state;

  return ret;

FAIL:
  dd_destroy_state(state);
  return ret;
}

static void dd_destroy_state(state_t* state)
{
  if (state)
  {
    int_gpio_state_t* int_gpio_state = state->int_gpio_list_head;
    uint32_t index = 0;

    while (int_gpio_state)
    {
      int_gpio_state_t* curr = int_gpio_state;
      int_gpio_state = int_gpio_state->next;
      sns_ddf_mfree(curr);
    }

    for (index = 0; index < state->sub_dev_count; ++index)
    {
      if (state->sub_dev[index].report_sensor_data.samples)
        sns_ddf_mfree(state->sub_dev[index].report_sensor_data.samples);
    }

    if (state->port_handle)
      sns_ddf_close_port(state->port_handle);

    sns_ddf_mfree(state);
  }
}

static sns_ddf_status_e dd_sub_dev_report_samples_realloc(sub_dev_t *sub_dev, uint32_t num_samples)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;

  if (!sub_dev || num_samples == 0)
  {
    VENDOR_DD_MSG_0(ERROR, "dd_sub_dev_report_samples_realloc(). Invalid arguments. \n");
    ret = SNS_DDF_EINVALID_PARAM;
    goto FAIL;
  }

  if (sub_dev->report_sensor_data.num_samples != num_samples)
  {
    sns_ddf_sensor_sample_s* samples = NULL;
    
    ret = sns_ddf_malloc((void **)&samples, sizeof(*samples) * 3 * num_samples);
    if (ret != SNS_DDF_SUCCESS)
    {
      VENDOR_DD_MSG_0(ERROR, "dd_sub_dev_samples_realloc(). Out of memory. \n");
      goto FAIL;
    }
    memset(samples, 0, sizeof(*samples) * 3 * num_samples);

    //// since SNS_DDF_SUCCESS == 0, memset done it
    //for (int i = 0; i < num_samples; ++i)
    //{
    //    samples[i * 3 + 0].status = SNS_DDF_SUCCESS;
    //    samples[i * 3 + 1].status = SNS_DDF_SUCCESS;
    //    samples[i * 3 + 2].status = SNS_DDF_SUCCESS;
    //}

    if (sub_dev->report_sensor_data.samples)
      sns_ddf_mfree(sub_dev->report_sensor_data.samples);

    sub_dev->report_sensor_data.samples = samples;
    sub_dev->report_sensor_data.num_samples = 3 * num_samples;
  }

  return ret;

FAIL:
  VENDOR_DD_MSG_1(FATAL, "dd_sub_dev_report_samples_realloc() failed, ret = %d \n", ret);
  return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_fill_sub_dev_slot: fill all info for a sub device.
*
* @param[in] sub_dev_slot - sub device slot to fill
* @param[in] smgr_handle - SMGR handle for this sub device
* @param[in] gpio - GPIO num for this sub device
* @param[in] num_sensors - number of sensors supported by this sub device
* @param[in] sensors - sensor list supported by this sub device
* @param[out] dd_num_sensors - number of sensors
*
* @return sns_ddf_status_e
*/

/*===========================================================================*/
static uint32_t sns_dd_fill_sub_dev_slot(
  state_t* state,
  sub_dev_t* sub_dev_slot,
  sns_ddf_handle_t smgr_handle,
  uint16_t interrupt_gpio,
  uint32_t num_sensors,
  sns_ddf_sensor_e sensor_types[],
  uint32_t *dd_num_sensors
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  int_gpio_state_t* int_gpio_state = state->int_gpio_list_head;
  uint32_t i = 0;
  uint32_t sensor_num_filled = 0;

  // Check for null-pointers
  if (!state || !sub_dev_slot || !dd_num_sensors)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto FAIL;
  }

  // Check bounds
  if (num_sensors > DD_NUM_SENSORS_PER_SUB_DEV)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto FAIL;
  }

  // alloc report sensor data
  ret = dd_sub_dev_report_samples_realloc(sub_dev_slot, 1);
  if (ret != SNS_DDF_SUCCESS)
    goto FAIL;

  // gpio
  while (int_gpio_state->interrupt_gpio != interrupt_gpio)
  {
    if (int_gpio_state->next)
    {
      int_gpio_state = int_gpio_state->next;
    }
    else
    {
      int_gpio_state_t* new_int_gpio_state = NULL;

      ret = sns_ddf_malloc((void **)&new_int_gpio_state, sizeof(*new_int_gpio_state));
      if (ret != SNS_DDF_SUCCESS)
      {
        VENDOR_DD_MSG_0(ERROR, "sns_dd_fill_sub_dev_slot() fail to alloc gpio_state_t");
        sns_ddf_mfree(sub_dev_slot->report_sensor_data.samples);
        goto FAIL;
      }
      memset(new_int_gpio_state, 0, sizeof(*new_int_gpio_state));

      new_int_gpio_state->interrupt_gpio = interrupt_gpio;
      new_int_gpio_state->irq_trigger_mode = SNS_DDF_SIGNAL_IRQ_RISING;

      // set to list
      int_gpio_state->next = new_int_gpio_state;
      int_gpio_state = new_int_gpio_state;
    }
  }
  sub_dev_slot->int_gpio_state = int_gpio_state;
  VENDOR_DD_MSG_1(ERROR, "sns_dd_fill_sub_dev_slot(), gpio = %d", sub_dev_slot->int_gpio_state->interrupt_gpio);

  sub_dev_slot->smgr_handle = smgr_handle;

  for (i = 0; i < num_sensors; i++)
  {
    sub_dev_slot->sensor_types[sensor_num_filled] = sensor_types[i];
    ++sensor_num_filled;
  }
  sub_dev_slot->sensor_num = sensor_num_filled;

  sub_dev_slot->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  sub_dev_slot->odr = 0;

  *dd_num_sensors = sub_dev_slot->sensor_num;

  return ret;

FAIL:
  VENDOR_DD_MSG_1(FATAL, "sns_dd_fill_sub_dev_slot() failed, ret = %d \n", ret);
  return ret;
}

//// handle
static sns_ddf_handle_t dd_state_to_handle(const state_t* state, uint32_t sub_dev_id)
{
  if (!state)
    return NULL;
  return (sns_ddf_handle_t)((uint32_t)state | state->sub_dev_count);
}

static state_t* dd_state_from_handle(const sns_ddf_handle_t dd_handle)
{
  return (state_t *)((uint32_t)dd_handle & (~DD_HANDLE_ALIGN));
}

static uint32_t dd_sub_dev_id_from_handle(const sns_ddf_handle_t dd_handle)
{
  return (uint32_t)((uint32_t)dd_handle & DD_HANDLE_ALIGN);
}

/*===========================================================================*/
/*
* @brief sns_dd_find_same_port_dev: check if a device with the input
*                         port config has already been initialized.
*
* @param[in] port_config_ptr - I2C port config.
*
* @return NULL if a similar port device was not initialized
*         device state pointer if already initialized
*/
/*===========================================================================*/
static state_t *sns_dd_pah_find_same_port_dev(sns_ddf_port_config_s *port_config_ptr)
{
  state_t *result = NULL;

  state_t *cur_dd_dev_ptr = (state_t*)g_dd_head.next;
  while (cur_dd_dev_ptr)
  {
    // if new port APIs are not available then you can check if I2C bus and slave address
    // is the same. This information is present in port config struct.
    if (sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
    {
      result = cur_dd_dev_ptr;
      break;
    }
    else
    {
      cur_dd_dev_ptr = cur_dd_dev_ptr->next;
    }
  }

  return result;
}



/*===========================================================================

FUNCTION:   sns_dd_pah_init

===========================================================================*/
/**
* @brief Initializes the driver and sets up devices.
*
* @param[out] dd_handle_ptr  Pointer that this function must malloc and
*                            populate. This is a handle to the driver
*                            instance that will be passed in to all other
*                            functions. NB: Do not use @a memhandler to
*                            allocate this memory.
* @param[in]  smgr_handle    Handle used to identify this driver when it
*                            calls into Sensors Manager functions.
* @param[in]  nv_params      NV parameters retrieved for the driver.
* @param[in]  device_info    Access info for physical devices controlled by
*                            this driver. Used to configure the bus
*                            and talk to the devices.
* @param[in]  num_devices    Number of elements in @a device_info.
* @param[in]  memhandler     Memory handler used to dynamically allocate
*                            output parameters, if applicable. NB: Do not
*                            use memhandler to allocate memory for
*                            @a dd_handle_ptr.
* @param[in/out] sensors     List of supported sensors, allocated,
*                            populated, and returned by this function.
* @param[in/out] num_sensors Number of elements in @a sensors.
*
* @return Success if @a dd_handle_ptr was allocated and the driver was
*         configured properly. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensor_types[],
  uint32_t*                num_sensors
  )
{
  state_t* state = NULL;
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  uint32_t dd_num_sensors = 0;

  VENDOR_DD_MSG_1(LOW, ">>> sns_dd_pah_init(), adsp driver[v%d]\n", ADSP_DRIVER_VERSION);

  // basic sanity check
  if (!dd_ptr || !sensor_types || !num_sensors)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  // If the device instance does not exist, then initialize it
  state = sns_dd_pah_find_same_port_dev(&device_info->port_config);

  if (!state)
  {
    // check signal irq support
    if (!sns_ddf_signal_irq_enabled())
    {
		VENDOR_DD_MSG_0(ERROR, "sns_dd_pah_init(), platform didn't enable signal irq \n");
		ret = SNS_DDF_EINVALID_PARAM;
		goto RTN;

    }

    // create state
    ret = dd_create_state(device_info, &state);
    if (ret != SNS_DDF_SUCCESS)
    {
      goto RTN;
    }

    // init platform support
    i2c_init(state->port_handle);
    system_clock_init();

    // Fill out the first sub-device slot
    ret = sns_dd_fill_sub_dev_slot(
      state,
      &state->sub_dev[0],
      smgr_handle,
      device_info->first_gpio,
      *num_sensors,
      *sensor_types,
      &dd_num_sensors);
    if (ret != SNS_DDF_SUCCESS)
      goto RTN;

    if (dd_num_sensors == 0)
    {
      VENDOR_DD_MSG_0(ERROR, "init - Unable to initialize any sensors.\n");
      dd_destroy_state(state);
      ret = SNS_DDF_EINVALID_DATA;
      goto RTN;
    }

    // init pah
    if (!pah_init())
    {
      VENDOR_DD_MSG_0(ERROR, "init - Fail to init pah driver\n");
      dd_destroy_state(state);
      ret = pah_plat_get_last_error();
      if (ret == SNS_DDF_SUCCESS)
        ret = SNS_DDF_EDEVICE;

      goto RTN;
    }

    // set to global
    state->next = g_dd_head.next;
    g_dd_head.next = state;

    // Set the output and return
    *dd_ptr = dd_state_to_handle(state, 0);
    *num_sensors = dd_num_sensors;
    *sensor_types = state->sub_dev[0].sensor_types;

    ++state->sub_dev_count;

    ret = SNS_DDF_SUCCESS;
  }
  else
  {
    // If there is an empty slot
    if (state->sub_dev_count < DD_NUM_SUB_DEV)
    {
      uint32_t sub_dev_id = state->sub_dev_count;

      // Fill out the next sub-device slot
      ret = sns_dd_fill_sub_dev_slot(
        state,
        &state->sub_dev[sub_dev_id],
        smgr_handle,
        device_info->first_gpio,
        *num_sensors,
        *sensor_types,
        &dd_num_sensors);
      if (ret != SNS_DDF_SUCCESS)
        goto RTN;

      if (dd_num_sensors == 0)
      {
        VENDOR_DD_MSG_0(ERROR, "init - Unable to initialize any sensors.\n");
        ret = SNS_DDF_EFAIL;
        goto RTN;
      }

      // Set the output and return
      *dd_ptr = dd_state_to_handle(state, sub_dev_id);
      *num_sensors = dd_num_sensors;
      *sensor_types = state->sub_dev[sub_dev_id].sensor_types;

      ++state->sub_dev_count;

      ret = SNS_DDF_SUCCESS;
    }
    else
    {
      VENDOR_DD_MSG_0(ERROR, "Out of sub-device slots.\n");
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    // determine that the first 2 subdevices share same or different pin
    if (state->sub_dev_count == 2)
    {
      state->is_shared_int1 = (state->sub_dev[g_sub_dev_id_ppg].int_gpio_state->interrupt_gpio == state->sub_dev[g_sub_dev_id_touch].int_gpio_state->interrupt_gpio);
      if (!state->is_shared_int1)
      {
        state->sub_dev[g_sub_dev_id_touch].int_gpio_state->irq_trigger_mode = SNS_DDF_SIGNAL_IRQ_DUAL_EDGE;

        pah_set_int2_as_touch_flag(true);
      }
    }
  }

RTN:
  if (ret == SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_1(LOW, "<<< sns_dd_pah_init(), output *dd_ptr = %d \n", *dd_ptr);
  else
    VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_init failed, ret = %d\n", ret);
  return ret;
}

/*===========================================================================*/
/*
* @brief sns_dd_pah_get_data: Requests a single sample of sensor data.
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
sns_ddf_status_e sns_dd_pah_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[]
  )
{
  //report raw data through irq handler
  return SNS_DDF_PENDING;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_reset

===========================================================================*/
/**
* @brief Resets the driver and device so they return to the state they were
*        in after init() was called.
*
* @param[in] dd_handle  Handle to a driver instance.
*
* @return Success if the driver was able to reset its state and the device.
*         Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_reset(
  sns_ddf_handle_t dd_handle
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);
  uint32_t index = 0;
  bool reset_registers = true;

  VENDOR_DD_MSG_1(LOW, ">>> sns_dd_pah_reset(%d)\n", sub_dev_id);

  // basic sanity check
  if (!dd_handle)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  // check is all sub devices are lowpower
  for (index = 0; index < state->sub_dev_count; ++index)
  {
    if (state->sub_dev[index].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
    {
      reset_registers = false;
      break;
    }
  }

  if (reset_registers)
  {
    VENDOR_DD_MSG_0(LOW, "reset registers...\n");

    if (!pah_enter_mode(pah_stop_mode))
    {
      VENDOR_DD_MSG_0(FATAL, "pah_enter_mode(pah_stop_mode), fail\n");
      ret = pah_plat_get_last_error();
      if (ret == SNS_DDF_SUCCESS)
        ret = SNS_DDF_EDEVICE;

      goto RTN;
    }

    // init skip invalid samples
    if (!pah_is_ppg_mode() && (sub_dev_id < DD_NUM_SUB_DEV))
    {
      state->sub_dev[sub_dev_id].is_sample_ready = false;
      state->sub_dev[sub_dev_id].discard_sample_duration = 0;
    }
  }

RTN:
  if (ret == SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_reset\n");
  else
    VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_reset failed, ret = %d\n", ret);
  return ret;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_set_attrib

===========================================================================*/
/**
* @brief Sets a sensor attribute to a specific value.
*
* @param[in] dd_handle  Handle to a driver instance.
* @param[in] sensor     Sensor for which this attribute is to be set. When
*                       addressing an attribute that refers to the driver
*                       this value is set to SNS_DDF_SENSOR__ALL.
* @param[in] attrib     Attribute to be set.
* @param[in] value      Value to set this attribute.
*
* @return Success if the value of the attribute was set properly. Otherwise
*         a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor_type,
  sns_ddf_attribute_e  attrib,
  void*                value
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);

  VENDOR_DD_MSG_1(LOW, ">>> sns_dd_pah_set_attrib(%d)\n", sub_dev_id);

  // basic sanity check
  if (!dd_handle || !value || (sub_dev_id >= DD_NUM_SUB_DEV))
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  switch (attrib)
  {
  case SNS_DDF_ATTRIB_POWER_STATE:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_POWER_STATE\n");
    const sns_ddf_powerstate_e *mode = (sns_ddf_powerstate_e*)value;
    state->sub_dev[sub_dev_id].powerstate = *mode;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_RANGE:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_RANGE\n");
    // Does not support different ranges
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_ODR:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_ODR\n");

    uint32_t odr = *(uint32_t *)value;

    // check is valid odr value
    if (odr > 0)
    {
      uint32_t odr_support_num = 0;
      const uint32_t *sub_dev_odr_support_list = NULL;
      uint32_t i = 0;
      bool is_odr_in_support_list = false;

      if (sub_dev_id == g_sub_dev_id_ppg)
      {
        odr_support_num = sizeof(SUB_DEV_ODR_SUPPORT_LIST_PPG) / sizeof(SUB_DEV_ODR_SUPPORT_LIST_PPG[0]);
        sub_dev_odr_support_list = SUB_DEV_ODR_SUPPORT_LIST_PPG;
      }
      else if (sub_dev_id == g_sub_dev_id_touch)
      {
        odr_support_num = sizeof(SUB_DEV_ODR_SUPPORT_LIST_TOUCH) / sizeof(SUB_DEV_ODR_SUPPORT_LIST_TOUCH[0]);
        sub_dev_odr_support_list = SUB_DEV_ODR_SUPPORT_LIST_TOUCH;
      }
      else
      {
        ret = SNS_DDF_EINVALID_PARAM;
        goto RTN;
      }

      // check if odr in support list
      for (i = 0; i < odr_support_num; ++i)
      {
        if (odr == sub_dev_odr_support_list[i])
        {
          is_odr_in_support_list = true;
          break;
        }
      }

      if (!is_odr_in_support_list)
      {
        ret = SNS_DDF_EINVALID_PARAM;
        goto RTN;
      }
    }

    state->sub_dev[sub_dev_id].odr = odr;
    VENDOR_DD_MSG_1(MED, "set odr == %d\n", odr);

    // set device mode
    ret = set_device_mode(state, sub_dev_id);
    if (ret != SNS_DDF_SUCCESS)
      goto RTN;


    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_FIFO:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_FIFO\n");

    sns_ddf_fifo_set_s wm = *(sns_ddf_fifo_set_s *)value;

    if (sub_dev_id == g_sub_dev_id_ppg)
    {
      uint32_t max_watermark = pah_get_max_report_sample_num_per_ch();
      if (wm.watermark > max_watermark)
        wm.watermark = (uint16_t)max_watermark;

      if (wm.watermark > 0)
        pah_set_report_sample_num_per_ch(wm.watermark);
    }

    state->sub_dev[sub_dev_id].water_mark = wm.watermark;
    VENDOR_DD_MSG_1(MED, "set watermark == %d\n", wm.watermark);
    ret = SNS_DDF_SUCCESS;
  }
  break;

  default:
  {
    VENDOR_DD_MSG_1(ERROR, "attrib = %d is not implemented.\n", attrib);
    ret = SNS_DDF_EINVALID_PARAM;
    break;
  }
  }

RTN:
  if (ret == SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_set_attrib\n");
  else
    VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_set_attrib failed, ret = %d\n", ret);
  return ret;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_get_attrib

===========================================================================*/
/**
* @brief Retrieves the value of an attribute for a sensor.
*
* @param[in]  dd_handle   Handle to a driver instance.
* @param[in]  sensor      Sensor whose attribute is to be retrieved. When
*                         addressing an attribute that refers to the driver
*                         this value is set to SNS_DDF_SENSOR__ALL.
* @param[in]  attrib      Attribute to be retrieved.
* @param[in]  memhandler  Memory handler used to dynamically allocate
*                         output parameters, if applicable.
* @param[out] value       Pointer that this function will allocate or set
*                         to the attribute's value.
* @param[out] num_elems   Number of elements in @a value.
*
* @return Success if the attribute was retrieved and the buffer was
*         populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor_type,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
  )
{
  sns_ddf_status_e ret = SNS_DDF_EINVALID_ATTR;
  state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);

  VENDOR_DD_MSG_1(LOW, ">>> sns_dd_pah_get_attrib(%d)\n", sub_dev_id);
  //VENDOR_DD_MSG_3(HIGH, "sns_dd_pah_get_attrib - enter sensor=%d, attrib=%d, sub_dev_id=%d", sensor_type, attrib, sub_dev_id);

  // basic sanity check
  if (!dd_handle || !memhandler || !value || !num_elems || (sub_dev_id >= DD_NUM_SUB_DEV))
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  switch (attrib)
  {
  case SNS_DDF_ATTRIB_DRIVER_INFO:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_DRIVER_INFO\n");
    sns_ddf_driver_info_s* driver_info = sns_ddf_memhandler_malloc(memhandler, sizeof(*driver_info));
    if (!driver_info)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

      driver_info->name = "PPG_raw";
    driver_info->version = 1;

    *value = driver_info;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_DEVICE_INFO:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_DEVICE_INFO\n");
    sns_ddf_device_info_s* device_info = sns_ddf_memhandler_malloc(memhandler, sizeof(*device_info));
    if (!device_info)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    if (sub_dev_id == g_sub_dev_id_ppg)
    {
      device_info->name = "PPG";
    }
    else if (sub_dev_id == g_sub_dev_id_touch)
    {
      device_info->name = "Touch";
    }
    else
    {
      ret = SNS_DDF_EINVALID_PARAM;
      goto RTN;
    }

      device_info->vendor = "Pixart";
      device_info->model = "PAH8011";
    device_info->version = 1;

    *value = device_info;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_RANGE:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_RANGE\n");
    sns_ddf_range_s* device_ranges = sns_ddf_memhandler_malloc(memhandler, sizeof(*device_ranges));
    if (!device_ranges)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    device_ranges->min = FX_FLTTOFIX_Q16(0.0);
    device_ranges->max = FX_FLTTOFIX_Q16(PPG_MAX_RANGE);

    *value = device_ranges;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_RESOLUTION:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_RESOLUTION\n");
    sns_ddf_resolution_t* resolution = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution));
    if (!resolution)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    *resolution = FX_FLTTOFIX_Q16(PPG_RESOLUTION);
    *value = resolution;

    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_RESOLUTION_ADC:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_RESOLUTION_ADC\n");
    sns_ddf_resolution_adc_s* resolution_adc = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution_adc));
    if (!resolution_adc)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }
    resolution_adc->bit_len = 32;
    resolution_adc->max_freq = 200;

    *value = resolution_adc;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_DELAYS:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_DELAYS\n");
    sns_ddf_delays_s* device_delay = sns_ddf_memhandler_malloc(memhandler, sizeof(*device_delay));
    if (!device_delay)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }
    device_delay->time_to_active = 2000;    //2ms is the startup time
    device_delay->time_to_data = 50000;     //20Hz, 50000us

    *value = device_delay;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_POWER_INFO:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_POWER_INFO\n");
    sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(memhandler, sizeof(*power));
    if (!power)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }
    power->active_current = 1500;    //1500uA
    power->lowpower_current = 15;    //15uA

    *value = power;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_POWER_STATE:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_POWER_STATE\n");
    sns_ddf_powerstate_e* power_state = sns_ddf_memhandler_malloc(memhandler, sizeof(*power_state));
    if (!power_state)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }
    *power_state = state->sub_dev[sub_dev_id].powerstate;

    *value = power_state;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_ODR:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_ODR\n");
    sns_ddf_odr_t* odr = sns_ddf_memhandler_malloc(memhandler, sizeof(*odr));
    if (!odr)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }
    *odr = state->sub_dev[sub_dev_id].odr;

    VENDOR_DD_MSG_1(MED, "get odr == %d\n", *odr);

    *value = odr;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST\n");
    sns_ddf_odr_t *odr_list = NULL;
    uint32_t odr_support_num = 0;
    const uint32_t *sub_dev_odr_support_list = NULL;
    uint32_t i = 0;

    if (sub_dev_id == g_sub_dev_id_ppg)
    {
      odr_support_num = sizeof(SUB_DEV_ODR_SUPPORT_LIST_PPG) / sizeof(SUB_DEV_ODR_SUPPORT_LIST_PPG[0]);
      sub_dev_odr_support_list = SUB_DEV_ODR_SUPPORT_LIST_PPG;
    }
    else if (sub_dev_id == g_sub_dev_id_touch)
    {
      odr_support_num = sizeof(SUB_DEV_ODR_SUPPORT_LIST_TOUCH) / sizeof(SUB_DEV_ODR_SUPPORT_LIST_TOUCH[0]);
      sub_dev_odr_support_list = SUB_DEV_ODR_SUPPORT_LIST_TOUCH;
    }
    else
    {
      ret = SNS_DDF_EINVALID_PARAM;
      goto RTN;
    }

    odr_list = sns_ddf_memhandler_malloc(memhandler, sizeof(*odr_list) * odr_support_num);
    if (!odr_list)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    for (i = 0; i < odr_support_num; ++i)
      odr_list[i] = sub_dev_odr_support_list[i];

    *value = odr_list;
    *num_elems = odr_support_num;

    ret = SNS_DDF_SUCCESS;
  }
  break;

  case SNS_DDF_ATTRIB_FIFO:
  {
    VENDOR_DD_MSG_0(MED, "SNS_DDF_ATTRIB_FIFO\n");
    sns_ddf_fifo_attrib_get_s* fifo_info = sns_ddf_memhandler_malloc(memhandler, sizeof(*fifo_info));
    if (!fifo_info)
    {
      ret = SNS_DDF_ENOMEM;
      goto RTN;
    }

    fifo_info->is_supported = true;
    fifo_info->is_sw_watermark = false;
    fifo_info->max_watermark = pah_get_max_report_sample_num_per_ch();
    fifo_info->share_sensor_cnt = 0;
    fifo_info->share_sensors[0] = NULL;

    *value = fifo_info;
    *num_elems = 1;
    ret = SNS_DDF_SUCCESS;
  }
  break;

  default:
    VENDOR_DD_MSG_1(ERROR, "attrib = %d is not implemented.\n", attrib);
    ret = SNS_DDF_EINVALID_ATTR;
    break;
  }

RTN:
  if (ret == SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_get_attrib\n");
  else
    VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_get_attrib failed, status = %d\n", ret);
  return ret;
}

void sns_dd_pah_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  //state_t* state = dd_state_from_handle(dd_handle);
  //uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);

  VENDOR_DD_MSG_0(LOW, ">>> sns_dd_pah_handle_timer\n");



  VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_handle_timer\n");
}


/*===========================================================================

FUNCTION:   sns_dd_pah_handle_irq

===========================================================================*/
/*!
@brief dd interrupt handler

@detail
This function will be called within the context of the Interrupt
Service Task (IST), *not* the ISR.

@param[in] dd_handle  Handle to a driver instance.
@param[in] irq        The IRQ representing the interrupt that occured.

@return None
*/
/*=========================================================================*/
void sns_dd_pah_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t gpio_num,
  sns_ddf_time_t timestamp
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);
  int_gpio_state_t* int_gpio_state = NULL;
  pah_ret pah_result = pah_err_unknown;

  VENDOR_DD_MSG_2(LOW, ">>> sns_dd_pah_handle_irq(%d), is_shared_int1 = %d \n", sub_dev_id, state->is_shared_int1);

  // basic sanity check
  if (!dd_handle)
  {
	ret = SNS_DDF_EINVALID_PARAM;
	goto FAIL;
  }

  if (sub_dev_id != g_sub_dev_id_touch && sub_dev_id != g_sub_dev_id_ppg)
  {
	ret = SNS_DDF_EINVALID_PARAM;
	goto FAIL;
  }

  int_gpio_state = state->sub_dev[sub_dev_id].int_gpio_state;
  if (!int_gpio_state)
  {
	ret = SNS_DDF_EINVALID_PARAM;
	goto FAIL;
  }

  if (state->is_shared_int1)
  {
	pah_result = pah_task();
	if (pah_result == pah_success)
	{
	  if (state->sub_dev[g_sub_dev_id_ppg].odr > 0)
	  {
		// In PPG+Touch mode, it's possible the interrupt indicates touch/notouch detection but not fifo.
		if (pah_fifo_data_num_per_ch() > 0)
		  report_fifo_data(&state->sub_dev[g_sub_dev_id_ppg], timestamp, pah_get_fifo_data(), pah_fifo_data_num_per_ch(), pah_is_touched());
	  }

	  if (state->sub_dev[g_sub_dev_id_touch].odr > 0)
	  {
		report_touch_detect(&state->sub_dev[g_sub_dev_id_touch], timestamp, pah_is_touched());
	  }
	}
	else if (pah_result == pah_no_interrupt)
	{
	  // ignore
	}
	else
	{
	  ret = pah_plat_get_last_error();
	  if (ret == SNS_DDF_SUCCESS)
		ret = SNS_DDF_EDEVICE;
	  goto FAIL;
	}
  }
  else // if not state->is_shared_int1
  {
	if (sub_dev_id == g_sub_dev_id_ppg)
	{
	  pah_result = pah_task();
	  if (pah_result == pah_success)
	  {
		report_fifo_data(&state->sub_dev[sub_dev_id], timestamp, pah_get_fifo_data(), pah_fifo_data_num_per_ch(), pah_is_touched());
	  }
	  else if (pah_result == pah_no_interrupt)
	  {
		// ignore
	  }
	  else
	  {
		ret = pah_plat_get_last_error();
		if (ret == SNS_DDF_SUCCESS)
		  ret = SNS_DDF_EDEVICE;
		goto FAIL;
	  }
	}
	else // if (sub_dev_id == g_sub_dev_id_touch)
	{
	  uint32_t gpio_status = 0;

	  ret = sns_ddf_signal_gpio_read(int_gpio_state->interrupt_gpio, &gpio_status);
	  if (ret != SNS_DDF_SUCCESS)
		goto FAIL;

	  report_touch_detect(&state->sub_dev[sub_dev_id], timestamp, (gpio_status > 0));
	}
  }

  VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_handle_irq() \n");
  return;

FAIL:
  VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_handle_irq() Fail. ret = %d, \n", ret);

  // disable schedule
  if (int_gpio_state->is_interrupt_enabled)
  {
	if (sns_ddf_signal_deregister(int_gpio_state->interrupt_gpio) == SNS_DDF_SUCCESS)
	  int_gpio_state->is_interrupt_enabled = false;
  }
}


/*===========================================================================

FUNCTION:   sns_dd_pah_run_test

===========================================================================*/
/**
* @brief Runs a factory test case.
*
* @param[in]  dd_handle  Handle to a driver instance.
* @param[in]  sensor     Sensor on which to run the test.
* @param[in]  test       Test case to run.
* @param[out] err        Optional driver-specific error code.
*
* @return One of the following error codes:
*     SNS_DDF_SUCCESS        - Test passed.
*     SNS_DDF_PENDING        - Test result will be sent as an event.
*     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
*     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
*     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
*     SNS_DDF_EFAIL          - Unknown error occurred.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_run_test
(
  sns_ddf_handle_t    dd_handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
  )
{
  sns_ddf_status_e ret = SNS_DDF_EINVALID_PARAM;
  //state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);

  VENDOR_DD_MSG_1(LOW, ">>> sns_dd_pah_run_test(%d)\n", sub_dev_id);

  if (!pah_verify_product_id())
  {
    ret = pah_plat_get_last_error();
    if (ret == SNS_DDF_SUCCESS)
      ret = SNS_DDF_EDEVICE;

    goto FAIL;
  }

  VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_run_test\n");
  return SNS_DDF_SUCCESS;

FAIL:
  VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_run_test failed, status = %d\n", ret);
  return ret;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_enable_sched_data

===========================================================================*/
/**
* @brief Begins device-scheduled sampling and enables notification via Data
*        Ready Interrupts (DRI).
*
* @param[in] handle  Handle to the driver's instance.
* @param[in] sensor  Sensor to be sampled.
* @param[in] enable  True to enable or false to disable data stream.
*
* @return SNS_DDF_SUCCESS if sensor was successfully configured and
*         internal sampling has commenced or ceased. Otherwise an
*         appropriate error code.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_enable_sched_data
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor_type,
  bool              enable
  )
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  state_t* state = dd_state_from_handle(dd_handle);
  uint32_t sub_dev_id = dd_sub_dev_id_from_handle(dd_handle);
  sub_dev_t *sub_dev = NULL;
  bool enable_int = enable;

  VENDOR_DD_MSG_3(LOW, ">>> sns_dd_pah_enable_sched_data(%d), enable = %d, dd_handle = %d \n", sub_dev_id, enable, dd_handle);

  // basic sanity check
  if (!dd_handle)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }
  if (sub_dev_id != g_sub_dev_id_touch && sub_dev_id != g_sub_dev_id_ppg)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  state->sub_dev[sub_dev_id].enable_sched = enable;

  // register/deregister interrupt
  if (state->is_shared_int1)
  {
    // only first sub_dev
    sub_dev = &state->sub_dev[0];
    enable_int = (state->sub_dev[g_sub_dev_id_ppg].odr > 0) || (state->sub_dev[g_sub_dev_id_touch].odr > 0);
  }
  else
  {
    sub_dev = &state->sub_dev[sub_dev_id];
    enable_int = state->sub_dev[sub_dev_id].odr > 0;
  }
  ret = enable_interrupt(sub_dev, dd_handle, enable_int);
  if (ret != SNS_DDF_SUCCESS)
    goto RTN;

  // run device
  {
    pah_device device = (sub_dev_id == g_sub_dev_id_ppg) ? pah_device_ppg : pah_device_touch;
    if (!pah_run_device(device, enable))
    {
		ret = pah_plat_get_last_error();
		if (ret == SNS_DDF_SUCCESS)
		  ret = SNS_DDF_EDEVICE;

      goto RTN;
    }
  }

  // report once if enable touch
  if (sub_dev_id == g_sub_dev_id_touch && enable)
  {
    sub_dev_t* sub_dev_touch = &state->sub_dev[g_sub_dev_id_touch];
    bool is_touched = false;

    if (pah_success != pah_read_touch_flag(&is_touched))
    {
		ret = pah_plat_get_last_error();
		if (ret == SNS_DDF_SUCCESS)
		  ret = SNS_DDF_EDEVICE;

      goto RTN;
    }

    // init with an indeterminate value
    sub_dev_touch->report_sensor_data.samples[2].sample = -1;

    // report touch status
    report_touch_detect(sub_dev_touch, sns_ddf_get_timestamp(), is_touched);
  }

RTN:
  if (ret == SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_enable_sched_data\n");
  else
    VENDOR_DD_MSG_1(FATAL, "<<< sns_dd_pah_enable_sched_data failed, ret = %d\n", ret);
  return ret;
}

static sns_ddf_status_e set_device_mode(state_t* state,uint32_t sub_dev_id)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  pah_mode mode = pah_none;
  bool is_enable_pah_ppg_mode = false, is_enable_pah_touch_mode = false;

  if (!state)
  {
    ret = SNS_DDF_EINVALID_PARAM;
    goto FAIL;
  }

  is_enable_pah_ppg_mode = state->sub_dev[g_sub_dev_id_ppg].odr > 0;
  is_enable_pah_touch_mode = state->sub_dev[g_sub_dev_id_touch].odr > 0;

  // pah enter mode
  if (is_enable_pah_ppg_mode)
  {
    if (state->sub_dev[g_sub_dev_id_ppg].odr == 200)
    {
      // 200 Hz
      if (is_enable_pah_touch_mode)
        mode = pah_ppg_200hz_touch_mode;
      else
        mode = pah_ppg_200hz_mode;
    }
    else
    {
      // default Hz
      if (is_enable_pah_touch_mode)
        mode = pah_ppg_touch_mode;
      else
        mode = pah_ppg_mode;
    }
  }
  else if (is_enable_pah_touch_mode)
  {
    mode = pah_touch_mode;
  }
  else // disable
  {
    mode = pah_stop_mode;
  }

  if (!pah_set_mode(mode))
  {
    VENDOR_DD_MSG_1(FATAL, "pah_set_mode(%d), fail\n", mode);
    
    ret = pah_plat_get_last_error();
    if (ret == SNS_DDF_SUCCESS)
      ret = SNS_DDF_EDEVICE;
    goto FAIL;
  }

  // init skip invalid samples
  if ((sub_dev_id == g_sub_dev_id_ppg) && !pah_is_ppg_mode())
  {
    state->sub_dev[sub_dev_id].is_sample_ready = false;
    state->sub_dev[sub_dev_id].discard_sample_duration = 0;
  }


  return ret;

FAIL:
  VENDOR_DD_MSG_1(FATAL, "change_device_mode(). Failed, ret = %d, \n", ret);
  return ret;
}

static sns_ddf_status_e enable_interrupt(sub_dev_t *sub_dev, sns_ddf_handle_t dd_handle, bool enable)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  int_gpio_state_t* int_gpio_state = NULL;

  if (!sub_dev)
  {
    VENDOR_DD_MSG_0(FATAL, "enable_interrupt() sub_dev == NULL \n");
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  int_gpio_state = sub_dev->int_gpio_state;
  if (!int_gpio_state)
  {
    VENDOR_DD_MSG_0(FATAL, "enable_interrupt() int_gpio_state == NULL \n");
    ret = SNS_DDF_EINVALID_PARAM;
    goto RTN;
  }

  VENDOR_DD_MSG_3(LOW, "enable_interrupt(), dd_handle = %d, int_gpio_state->is_interrupt_enabled = %d, enable = %d \n", dd_handle, int_gpio_state->is_interrupt_enabled, enable);
  if (enable)
  {
    if (!int_gpio_state->is_interrupt_enabled)
    {
      ret = sns_ddf_signal_register(int_gpio_state->interrupt_gpio, dd_handle, &sns_dd_pah8011_if, int_gpio_state->irq_trigger_mode);
      if (ret != SNS_DDF_SUCCESS)
      {
        VENDOR_DD_MSG_0(FATAL, "enable_interrupt(), sns_ddf_signal_register fail \n");
        goto RTN;
      }
      int_gpio_state->is_interrupt_enabled = true;
    }
  }
  else // disable
  {
    if (int_gpio_state->is_interrupt_enabled)
    {
      ret = sns_ddf_signal_deregister(int_gpio_state->interrupt_gpio);
      if (ret != SNS_DDF_SUCCESS)
      {
        VENDOR_DD_MSG_0(FATAL, "enable_interrupt(), sns_ddf_signal_deregister fail \n");
        goto RTN;
      }
      int_gpio_state->is_interrupt_enabled = false;
    }
  }

RTN:
  if (ret != SNS_DDF_SUCCESS)
    VENDOR_DD_MSG_1(FATAL, "enable_interrupt() failed, ret = %d\n", ret);

  return ret;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_probe

===========================================================================*/
/**
* @brief Probes for the device with a given configuration.

* @param[in]  dev_info    Access info for physical devices controlled by
*                         this driver. Used to determine if the device is
*                         physically present.
* @param[in]  memhandler  Memory handler used to dynamically allocate
*                         output parameters, if applicable.
* @param[out] num_sensors Number of sensors supported. 0 if none.
* @param[out] sensor_type Array of sensor types supported, with num_sensor
*                         elements. Allocated by this function.
*
* @return SNS_DDF_SUCCESS if the part was probed function completed, even
*         if no device was found (in which case num_sensors will be set to
*         0).
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_probe
(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors
  )
{
  VENDOR_DD_MSG_0(LOW, ">>> sns_dd_pah_probe\n");

  sns_ddf_status_e ret = SNS_DDF_SUCCESS;



  //RTN:
  VENDOR_DD_MSG_0(LOW, "<<< sns_dd_pah_probe\n");
  return ret;
}

/*===========================================================================

FUNCTION:   sns_dd_pah_trigger_fifo_data

===========================================================================*/
/**
* @brief Retrieves a set of sensor data. Asynchronous API
*
* @note If a sensor has failed or
*       isn't available, @a sns_ddf_sensor_data_s.status must be used to
*       reflect this status.
*
* @param[in]  dd_handle     Handle to a driver instance.
* @param[in]  sensor        sensor for which data is requested.
*
* @param[in]  num_samples   number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
* @param[in]  trigger now   trigger notify fifo data now or
*       later when trigger_now is set to true.
*
*
* @return SNS_DDF_SUCCESS   if data was populated successfully.
*         via sns_ddf_smgr_data_notify() or if trigger_now is
*         set to false; Otherwise a specific error code is
*         returned.
*
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pah_trigger_fifo_data
(
  sns_ddf_handle_t         dd_handle,
  sns_ddf_sensor_e         sensor,
  uint16_t                 num_samples,
  bool                     trigger_now
  )
{
  return SNS_DDF_SUCCESS;
}


/*===========================================================================

STATIC FUNCTION:

===========================================================================*/
	static void report_fifo_data(sub_dev_t *sub_dev, sns_ddf_time_t timestamp, uint8_t *fifo_data, uint32_t sample_num_per_ch, bool is_touched)
{
  uint32_t* sample_data = (uint32_t*)fifo_data;
  sns_ddf_time_t now_timestamp = 0, prev_timestamp = 0, diff_timestamp = 0;
  uint32_t i = 0;

  VENDOR_DD_MSG_0(LOW, "report_fifo_data().");

  if (pah_get_bytes_per_sample() != 4)
  {
    VENDOR_DD_MSG_0(FATAL, "report_fifo_data(). support exactly 4 bytes_per_sample now \n");
    goto FAIL;
  }

  if (!sub_dev || !sample_data || sample_num_per_ch == 0)
    return;

  now_timestamp = timestamp;
  prev_timestamp = sub_dev->prev_timestamp;
  diff_timestamp = (now_timestamp - prev_timestamp) / sample_num_per_ch;

  // update sub_dev
  sub_dev->prev_timestamp = now_timestamp;

  // skip initial invalid sample
  if (!sub_dev->is_sample_ready)
  {
    if (sub_dev->discard_sample_duration < DISCARD_SAMPLE_DURATION_MS)
    {
      const uint32_t sample_peroid_ms = 1000 / sub_dev->odr;

      sub_dev->discard_sample_duration += (sample_peroid_ms * sample_num_per_ch);
      VENDOR_DD_MSG_1(FATAL, "report_fifo_data(). Discard sample duration = %d (ms) \n", sub_dev->discard_sample_duration);
      return;
    }
    else
    {
      sub_dev->is_sample_ready = true;
      VENDOR_DD_MSG_0(FATAL, "report_fifo_data(). Sample is ready \n");
    }
  }

  if (pah_is_ppg_mode())
  {
    static const uint32_t CH_NUM = 2;
    const q16_t sample_touch = is_touched ? FX_FLTTOFIX_Q16(128) : FX_FLTTOFIX_Q16(0);
    //VENDOR_DD_MSG_2(MED, "sub_dev->water_mark = %d, sample_num_per_ch = %d \n", sub_dev->water_mark, sample_num_per_ch);

    if (sub_dev->water_mark > 0) // FIFO DRI mode
    {
      if (sub_dev->water_mark != sample_num_per_ch)
      {
        VENDOR_DD_MSG_2(FATAL, "report_fifo_data(). watermark = %d doesn't match sample_num_per_ch = %d \n", sub_dev->water_mark, sample_num_per_ch);
        goto FAIL;
      }

      if (dd_sub_dev_report_samples_realloc(sub_dev, sample_num_per_ch) != SNS_DDF_SUCCESS)
        goto FAIL;


      sub_dev->report_sensor_data.status = SNS_DDF_SUCCESS;
      sub_dev->report_sensor_data.sensor = sub_dev->sensor_types[0];
      sub_dev->report_sensor_data.timestamp = prev_timestamp + diff_timestamp;
      sub_dev->report_sensor_data.end_timestamp = now_timestamp;

      for (i = 0; i < sample_num_per_ch; ++i)
      {
        //VENDOR_DD_MSG_4(HIGH, "report_ppg_data_handler(), i = %d, ch1 = %d, ch2 = %d, touch = %d \n", i, sample_data[i * CH_NUM + 0], sample_data[i * CH_NUM + 1], is_touched);
        sub_dev->report_sensor_data.samples[i * 3 + 0].sample = (int32_t)sample_data[i * CH_NUM + 0];
        sub_dev->report_sensor_data.samples[i * 3 + 1].sample = (int32_t)sample_data[i * CH_NUM + 1];
        sub_dev->report_sensor_data.samples[i * 3 + 2].sample = sample_touch;
      }

      if (sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &sub_dev->report_sensor_data, 1) != SNS_DDF_SUCCESS)
      {
        VENDOR_DD_MSG_0(FATAL, "report_fifo_data(), sns_ddf_smgr_notify_data Failed\n");
        goto FAIL;
      }

      log_pkt_pah_data(&sub_dev->report_sensor_data);
    }
    else // DRI mode (water_mark == 0)
    {
      sub_dev->report_sensor_data.status = SNS_DDF_SUCCESS;
      sub_dev->report_sensor_data.sensor = sub_dev->sensor_types[0];

      sub_dev->report_sensor_data.samples[2].sample = sample_touch;
      for (i = 0; i < sample_num_per_ch; ++i)
      {
        sub_dev->report_sensor_data.timestamp = prev_timestamp + diff_timestamp * i;
        sub_dev->report_sensor_data.end_timestamp = prev_timestamp + diff_timestamp * (i + 1);

        sub_dev->report_sensor_data.samples[0].sample = (int32_t)sample_data[i * CH_NUM + 0];
        sub_dev->report_sensor_data.samples[1].sample = (int32_t)sample_data[i * CH_NUM + 1];

        //VENDOR_DD_MSG_4(HIGH, "report_ppg_data_handler(), i = %d, ch1 = %d, ch2 = %d, touch = %d \n", i, sample_data[i * CH_NUM + 0], sample_data[i * CH_NUM + 1], is_touched);
        if (sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &sub_dev->report_sensor_data, 1) != SNS_DDF_SUCCESS)
        {
          VENDOR_DD_MSG_0(FATAL, "report_fifo_data(), sns_ddf_smgr_notify_data Failed\n");
          goto FAIL;
        }

        log_pkt_pah_data(&sub_dev->report_sensor_data);
      }
    }
  }

  return;

FAIL:
  VENDOR_DD_MSG_0(FATAL, "report_fifo_data Failed\n");
  return;
}

static void report_touch_detect(sub_dev_t *sub_dev, sns_ddf_time_t timestamp, bool is_touched)
{
  q16_t sample_touch = FX_FLTTOFIX_Q16(is_touched ? 128 : 0);

  if (sample_touch != sub_dev->report_sensor_data.samples[2].sample)
  {
	sns_ddf_time_t now_timestamp = timestamp;
	sns_ddf_time_t prev_timestamp = sub_dev->prev_timestamp;


    sub_dev->prev_timestamp = now_timestamp;

    sub_dev->report_sensor_data.status = SNS_DDF_SUCCESS;
    sub_dev->report_sensor_data.sensor = sub_dev->sensor_types[0];
    sub_dev->report_sensor_data.timestamp = prev_timestamp;
    sub_dev->report_sensor_data.end_timestamp = now_timestamp;
    sub_dev->report_sensor_data.samples[0].sample = 0;
    sub_dev->report_sensor_data.samples[1].sample = 0;
    sub_dev->report_sensor_data.samples[2].sample = sample_touch;

    VENDOR_DD_MSG_1(LOW, "report_touch_detect(), is_touched = %d \n", is_touched);
    if (sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &sub_dev->report_sensor_data, 1) != SNS_DDF_SUCCESS)
    {
      VENDOR_DD_MSG_0(FATAL, "report_touch_detect(), sns_ddf_smgr_notify_data Failed\n");
      goto FAIL;
    }

    log_pkt_pah_data(&sub_dev->report_sensor_data);
  }

  return;

FAIL:
  VENDOR_DD_MSG_0(FATAL, "report_touch_detect Failed\n");
  return;
}

static void log_pkt_pah_data(sns_ddf_sensor_data_s *sensor_data)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s *log_struct_ptr = NULL;
  log_pkt_t log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA; //SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
  uint32_t i = 0;

  err_code = sns_logpkt_malloc(
    log_pkt_type,
    sizeof(sns_log_sensor_data_pkt_s) + (sensor_data->num_samples - 1)*sizeof(int32_t),
    (void**)&log_struct_ptr);

  if (err_code != SNS_SUCCESS || log_struct_ptr == NULL)
  {
    //VENDOR_DD_MSG_0(FATAL, "log_pkt_data(). sns_logpkt_malloc fail \n");
    goto FAIL;
  }

  log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
  log_struct_ptr->sensor_id = sensor_data->sensor;
  log_struct_ptr->vendor_id = SNS_DDF_VENDOR_PIXART;

  log_struct_ptr->timestamp = sensor_data->timestamp;
  log_struct_ptr->end_timestamp = sensor_data->end_timestamp;

  log_struct_ptr->num_data_types = 3;
  log_struct_ptr->num_samples = sensor_data->num_samples / log_struct_ptr->num_data_types;

  log_struct_ptr->data[0] = sensor_data->samples[0].sample;
  log_struct_ptr->data[1] = sensor_data->samples[1].sample;
  log_struct_ptr->data[2] = sensor_data->samples[2].sample;

  for (i = 0; i < sensor_data->num_samples; ++i)
    log_struct_ptr->samples[i] = sensor_data->samples[i].sample;

  //VENDOR_DD_MSG_0(LOW, "sns_logpkt_commit() \n");
  err_code = sns_logpkt_commit(log_pkt_type, log_struct_ptr);
  if (err_code != SNS_SUCCESS)
  {
    //VENDOR_DD_MSG_0(FATAL, "log_pkt_data(). sns_logpkt_commit fail \n");
    goto FAIL;
  }

  return;
FAIL:
  //VENDOR_DD_MSG_1(FATAL, "log_pkt_data() failed, err_code = %d\n", err_code);
  return;
}

