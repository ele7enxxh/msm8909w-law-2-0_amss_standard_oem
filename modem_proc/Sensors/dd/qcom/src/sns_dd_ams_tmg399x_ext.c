/********************************************************************************
* Copyright (c) 2014, "ams AG"
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
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
*******************************************************************************/
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Init and get_attrib routines for ALS/PRX driver. These can be placed outside
   TCM if needed.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/09/15     AH      Fix compiler warnings
10/29/14     PS      Support for virtio bringup
08/26/14     MW      Added supported ODR list, fix chipID check in probe
06/20/14     ps      Prevent reuse of freed pointer by another device instance
02/15/14     fv      Convert tmd37x2 driver to tmg399x
11/14/13     fv      Convert tmd377x driver to tmd37x2
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
11/14/12     fv      Modified refrence driver to use ams/taos sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
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
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"

#include "ams_tmg399x.h"
#include "sns_dd_ams_tmg399x_priv.h"
#include "ams_i2c.h"

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
static sns_dd_head_s dd_head = { NULL, {"dd_name", 1} };
static uint16_t currentState;

extern int32_t subDevId_AlsProx;
extern int32_t subDevId_RgbCt;
extern int32_t subDevId_Gesture;

extern bool firstProx;
extern enum state_machine_sensors_e currentSensor;
extern bool     currentlyActiveSensors[MAX_NUM_OF_SENSORS];  // Als, Prox, Gesture, RGB, CT

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list;

extern sns_ddf_status_e ams_i2c_write_buf_db(sns_dd_state_t* state, uint8_t byteCnt, uint8_t* buf);
static sns_ddf_status_e ams_calibrate_als(sns_dd_state_t* state);
static sns_ddf_status_e sns_dd_ams_tmg399x__alsprx_set_powerstate(sns_dd_state_t* state, uint32_t sub_dev_id, sns_ddf_powerstate_e op_mode,
  sns_ddf_sensor_e sensor, bool init_data);
static sns_ddf_status_e ams_tmg399x_state_machine(sns_dd_state_t* state, sns_ddf_sensor_e sensor, bool sensorEnable);

static sns_ddf_odr_t tmg399x_odr_list[] = {

    SNS_DD_ALSPRX_PRX_FREQ
};

static uint32_t tmg399x_odr_list_size = sizeof(tmg399x_odr_list)/sizeof(tmg399x_odr_list[0]);



  //                      Enable SENSOR
  //==================================================================
  //    ALS                    PRX                    GES                 // STATE
  //--------------------  ---------------------  ---------------------       --------------
static uint8_t enableStates[8][3] =
  {
  {AMS_STATE_ALS,         AMS_STATE_PRX,         AMS_STATE_GES},          // OFF
  {AMS_STATE_ALS,         AMS_STATE_ALS_PRX,     AMS_STATE_GES_ALS},      // ALS
  {AMS_STATE_ALS_PRX,     AMS_STATE_PRX,         AMS_STATE_GES_PRX},      // PRX
  {AMS_STATE_ALS_PRX,     AMS_STATE_ALS_PRX,     AMS_STATE_GES_ALS_PRX},  // ALS + PRX
  {AMS_STATE_GES_ALS,     AMS_STATE_GES_PRX,     AMS_STATE_GES},          // GES
  {AMS_STATE_GES_ALS,     AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS},      // GES + ALS
  {AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_PRX,     AMS_STATE_GES_PRX},      // GES + PRX
  {AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS_PRX},  // GES + ALS + PRX
  };


  //                     Disable SENSOR
  //==================================================================
  //    ALS                    PRX                    GES                 // STATE
  //--------------------  ---------------------  ---------------------       --------------
static uint8_t disableStates[8][3] =
  {
  {AMS_STATE_OFF,         AMS_STATE_OFF,         AMS_STATE_OFF},          // OFF
  {AMS_STATE_OFF,         AMS_STATE_ALS,         AMS_STATE_ALS},          // ALS
  {AMS_STATE_PRX,         AMS_STATE_OFF,         AMS_STATE_PRX},          // PRX
  {AMS_STATE_PRX,         AMS_STATE_ALS,         AMS_STATE_ALS_PRX},      // ALS + PRX
  {AMS_STATE_GES,         AMS_STATE_GES,         AMS_STATE_OFF},          // GES
  {AMS_STATE_GES,         AMS_STATE_GES_ALS,     AMS_STATE_ALS},          // GES + ALS
  {AMS_STATE_GES_PRX,     AMS_STATE_GES,         AMS_STATE_PRX},          // GES + PRX
  {AMS_STATE_GES_PRX,     AMS_STATE_GES_ALS,     AMS_STATE_ALS_PRX},      // GES + ALS + PRX
  };


/*===========================================================================*/
/*
 * @brief sns_dd_fill_sub_dev_slot: fill all info for a sub device.
 *
 * @param[in] sub_dev_slot - sub device slot to fill
 * @param[in] smgr_handle - SMGR handle for this sub device
 * @param[in] gpio - GPIO num for this sub device
 * @param[in] num_sensors - number of sensors supported by this sub device
 * @param[in] sensors - sensor list supported by this sub device
 *
 * @return number of sensors
 */
/*===========================================================================*/
static uint32_t sns_dd_fill_sub_dev_slot
(
 sns_dd_sub_dev_t* sub_dev_slot,
 sns_ddf_handle_t  smgr_handle,
 uint16_t          gpio,
 uint32_t          num_sensors,
 sns_ddf_sensor_e  sensors[]
)
{
    int i;
    uint32_t num_sensors_filled = 0;

    // Check for null-pointers
    if (NULL == sub_dev_slot)
    {
        DD_MSG_1(ERROR, "fill_sub_dev_slot - sub_dev_slot: %d", sub_dev_slot);
        return 0;
    }


    // Check bounds
    if (num_sensors > DD_NUM_SENSORS_PER_SUB_DEV)
    {
        DD_MSG_1(ERROR, "fill_sub_dev_slot - num_sensors: %d", num_sensors);
        return 0;
    }

    for( i = 0 ; i < num_sensors ; i++ )
    {
        sub_dev_slot->sensors[num_sensors_filled++] = sensors[i];
    }

    sub_dev_slot->num_sensors    = num_sensors_filled;
    sub_dev_slot->interrupt_gpio = gpio;
    sub_dev_slot->dri_enable     = false;

    if(sub_dev_slot->interrupt_gpio != 0xFFFF)
    {
        sub_dev_slot->dri_enable     = true;
    }

    sub_dev_slot->smgr_handle    = smgr_handle;
    sub_dev_slot->powerstate     = SNS_DDF_POWERSTATE_LOWPOWER;
    sub_dev_slot->odr            = 0;

    return sub_dev_slot->num_sensors;
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
static sns_dd_state_t *sns_dd_find_same_port_dev(sns_ddf_port_config_s *port_config_ptr)
{
    sns_dd_state_t *cur_dd_dev_ptr = (sns_dd_state_t*) dd_head.next;

    while (cur_dd_dev_ptr)
    {
        if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
        {
            DD_MSG_1(MED, "find_same_port_dev - cur_dev_ptr: %d", cur_dd_dev_ptr);
            return cur_dd_dev_ptr;
        }
        else
        {
            cur_dd_dev_ptr = cur_dd_dev_ptr->next;
        }
    }
    return NULL;
}


/*===========================================================================*/
/*
 * @brief sns_dd_init_timer: initialize timer for the sub device.
 *
 * @param[in] state - DD state handle.
 * @param[in] sub_dev_id
 * @param[in] sensor
 *
 * @return SNS_DDF_SUCCESS if timer init is successful else error code
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_init_timer
(
 sns_dd_state_t*   state,
 uint32_t          sub_dev_id,
 sns_ddf_sensor_e* sensor
 )
{
    sns_ddf_status_e ret = SNS_DDF_SUCCESS;

    DD_I2C_DB_3(0x0001, 0x01, *sensor);
    DD_I2C_DB_3(0x0001, 0x02, sub_dev_id);


    if ( *sensor == SNS_DDF_SENSOR_PROXIMITY )  // example prox sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_PROXIMITY;

        DD_I2C_DB_3(0x0001, 0x03, state->sub_dev[sub_dev_id].timer_arg[0]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);
        // add als
        state->sub_dev[sub_dev_id].timer_arg[1] = SNS_DDF_SENSOR_AMBIENT;

        DD_I2C_DB_3(0x0001, 0x04, state->sub_dev[sub_dev_id].timer_arg[1]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[1],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[1],       // NULL,
                                 FALSE);
        subDevId_AlsProx        = sub_dev_id;
        state->sub_dev_0_enable = false;
    }
    else if ( *sensor == SNS_DDF_SENSOR_RGB )   // example gesture sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_RGB;

        DD_I2C_DB_3(0x0001, 0x05, state->sub_dev[sub_dev_id].timer_arg[0]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);

        state->sub_dev[sub_dev_id].timer_arg[1] = SNS_DDF_SENSOR_CT_C;

        DD_I2C_DB_3(0x0001, 0x06, state->sub_dev[sub_dev_id].timer_arg[1]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[1],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[1],       // NULL,
                                 FALSE);

        subDevId_RgbCt          = sub_dev_id;
        state->sub_dev_1_enable = false;
    }
    else if ( *sensor == SNS_DDF_SENSOR_IR_GESTURE )   // example gesture sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_IR_GESTURE;

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);

        subDevId_Gesture = sub_dev_id;
    }
    else
    {
        ret = SNS_DDF_EINVALID_PARAM;
    }

    DD_I2C_DB_2(0x0001, 0xFF);

    return ret;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to
   associated devices, configures the driver and devices, and places
   the devices in the default power state. Returns the instance handle along
   with a list of supported sensors. This function will be called at init
   time.

   @param[out] dd_handle_ptr  Pointer that this function must malloc and
                              populate. This is a handle to the driver
                              instance that will be passed in to all other
                              functions. NB: Do not use @a memhandler to
                              allocate this memory.
   @param[in]  smgr_handle    Handle used to identify this driver when it
                              calls into Sensors Manager functions.
   @param[in]  nv_params      NV parameters retrieved for the driver.
   @param[in]  device_info    Access info for physical devices controlled by
                              this driver. Used to configure the bus
                              and talk to the devices.
   @param[in]  num_devices    Number of elements in @a device_info.
   @param[in]  memhandler     Memory handler used to dynamically allocate
                              output parameters, if applicable. NB: Do not
                              use memhandler to allocate memory for
                              @a dd_handle_ptr.
   @param[out] sensors        List of supported sensors, allocated,
                              populated, and returned by this function.
   @param[out] num_sensors    Number of elements in @a sensors.

   @return Success if @a dd_handle_ptr was allocated and the driver was
           configured properly. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_init
(
 sns_ddf_handle_t*        dd_handle_ptr,
 sns_ddf_handle_t         smgr_handle,
 sns_ddf_nv_params_s*     nv_params,
 sns_ddf_device_access_s  device_info[],
 uint32_t                 num_devices,
 sns_ddf_memhandler_s*    memhandler,
 sns_ddf_sensor_e*        sensors[],
 uint32_t*                num_sensors
)
{
    sns_dd_state_t*       state;
    sns_ddf_status_e      status = SNS_DDF_SUCCESS;
    uint32_t              dd_num_sensors = 0;
    sns_ddf_handle_t      dd_handle;
    uint16_t              i;
    uint16_t              deviceId;

    DD_MSG_0(HIGH,"sns_dd_init - enter");

    /* Update smgr sensor data for the driver to indicate uImage support  */
    //sns_ddf_smgr_set_uimg_refac(smgr_handle);

    /* basic sanity check */
    if (dd_handle_ptr == NULL || sensors == NULL || num_sensors == NULL)
    {
        DD_MSG_0(ERROR, "init - Pointers are NULL");
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_1(MED, "init - sensors[0] = %d", (*sensors)[0]);

    // If the device instance does not exist, then initialize it
    state = sns_dd_find_same_port_dev(&device_info->port_config);

    if ( NULL == state )
    {
        /* allocate state memory */
        if (sns_ddf_malloc((void **)&state, sizeof(sns_dd_state_t)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            return SNS_DDF_ENOMEM;
        }

        memset(state, 0, sizeof(sns_dd_state_t));

        // Put this device instance at the front of the list (all inserts into
        // the list are placed at the front)
        state->next  = NULL;         // dd_head.next;       //
        dd_head.next = state;
        state->sub_dev_count = 0;


        // skip this function if API not available
        sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

        if (sns_ddf_malloc((void **)&state->s_data, sizeof(state->s_data)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return SNS_DDF_ENOMEM;
        }

        if (sns_ddf_malloc((void **)&state->samples, sizeof(state->samples)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return SNS_DDF_ENOMEM;
        }

        /* open I2C port */
        status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR, "init: I2C open failed");
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return status;
        }

        deviceId = ams_getField_r(state, ID, &status);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR, "init: I2C Read failed");
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return status;
        }

#ifndef VIRTIO_BRINGUP
        // Now check if the device ID is valid
        if(!ams_validDeviceId(deviceId))
        {
            DD_MSG_1(ERROR, "init: device id 0x%02X is invalid", deviceId);

            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return SNS_DDF_EFAIL;
        }
#endif

#ifdef USING_OLD_QDSP6
        // DEBUG ++++
        {
            DD_I2C_DB_3(0x0000, 0x01, *num_sensors);

            DD_I2C_DB_3(0x0000, 0xF1, sizeof(long));
            DD_I2C_DB_3(0x0000, 0xF2, sizeof(int));
            DD_I2C_DB_3(0x0000, 0xF3, sizeof(short));

            for(i = 0; i < *num_sensors; ++i)
            {
                DD_I2C_DB_3(0x0000, 0x11, (*sensors)[i]);
            }

            // TEST
            *num_sensors  = 2;
            *sensors      = sns_ddf_memhandler_malloc( memhandler, sizeof(sns_ddf_sensor_e) * *num_sensors );

#if(AMS_INIT_FIRST == AMS_INIT_PRX_ALS)
            (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
            (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#elif(AMS_INIT_FIRST == AMS_INIT_RGB_CT)
            (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
            (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#elif(AMS_INIT_FIRST == AMS_INIT_GESTURE)
            *num_sensors  = 1;
            (*sensors)[0] = SNS_DDF_SENSOR_IR_GESTURE;
#else
            (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
            (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#endif
            // TEST

            DD_I2C_DB_3(0x0000, 0x02, *num_sensors);

            for(i = 0; i < *num_sensors; ++i)
            {
                DD_I2C_DB_3(0x0000, 0x21, (*sensors)[i]);
            }

            DD_MSG_1(MED, "init - Number of sensors: %d", *num_sensors);
            DD_MSG_1(MED, "init - Number of devices: %d", num_devices);
        }
        // DEBUG ----
#endif

        // Fill out the first sub-device slot
        dd_num_sensors = sns_dd_fill_sub_dev_slot( &state->sub_dev[state->sub_dev_count],
                                                   smgr_handle,
                                                   device_info->first_gpio,
                                                   *num_sensors,
                                                   *sensors);
        if (dd_num_sensors == 0)
        {
            DD_I2C_DB_2(0x0000, 3);

            DD_MSG_0(ERROR, "init - Unable to initialize any sensors.");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return SNS_DDF_EINVALID_DATA;
        }


        // Assign the dd_handle_ptr
        dd_handle = (sns_ddf_handle_t)state;
        *dd_handle_ptr = dd_handle;

        // init timers
        status = sns_dd_init_timer(state, state->sub_dev_count, (sns_ddf_sensor_e *)*sensors);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_I2C_DB_2(0x0000, 4);
            DD_MSG_0(ERROR, "sns_dd_init_timer failed");

            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return status;
        }

        // parse registry if applicable
        status = sns_dd_read_registry(state, state->sub_dev_count, nv_params);
        if(SNS_DDF_SUCCESS != status)
        {
            DD_I2C_DB_2(0x0000, 5);
            DD_MSG_0(ERROR, "INIT - Failed to read registry");

            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return status;
        }

        DD_MSG_1(ERROR, "init - read_registry status: %d", status);


        // reset the device, reset also initializes registers
        status = sns_dd_ams_tmg399x_reset(dd_handle);  // sns_dd_reset(dd_handle);
        if( status != SNS_DDF_SUCCESS )
        {
            DD_I2C_DB_3(0x0000, 6, *num_sensors);

            DD_MSG_0(ERROR, "reset failed in init");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            dd_head.next = NULL;
            return status;
        }

        // Set the output and return
        *num_sensors = dd_num_sensors;
        *sensors = (sns_ddf_sensor_e *)state->sub_dev[state->sub_dev_count].sensors;

        state->sub_dev_count++;

        status = SNS_DDF_SUCCESS;

        DD_I2C_DB_3(0x0000, 0x07, *num_sensors);
        DD_I2C_DB_3(0x0000, 0x08, state->sub_dev_count);
    }
    else
    {
        DD_I2C_DB_2(0x0000, 9);
        DD_I2C_DB_3(0x0000, 0x0A, state->sub_dev_count);

        // If there is an empty slot
        if (state->sub_dev_count < DD_NUM_SUB_DEV)
        {
#ifdef USING_OLD_QDSP6
            // DEBUG ++++
            {
                DD_I2C_DB_3(0x0000, 0x0B, *num_sensors);

                for(i = 0; i < *num_sensors; ++i)
                {
                    DD_I2C_DB_3(0x0000, 0xB1, (*sensors)[i]);
                }

                // TEST
                *num_sensors  = 2;
                *sensors = sns_ddf_memhandler_malloc( memhandler, sizeof(sns_ddf_sensor_e) * *num_sensors );

#if(AMS_INIT_SECOND == AMS_INIT_RGB_CT)
                (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
                (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#elif(AMS_INIT_SECOND == AMS_INIT_PRX_ALS)
                (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
                (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#elif(AMS_INIT_SECOND == AMS_INIT_GESTURE)
                *num_sensors  = 1;
                (*sensors)[0] = SNS_DDF_SENSOR_IR_GESTURE;
#else
                (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
                (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#endif
                // TEST

                DD_I2C_DB_3(0x0000, 0x0C, *num_sensors);

                for(i = 0; i < *num_sensors; ++i)
                {
                    DD_I2C_DB_3(0x0000, 0xC1, (*sensors)[i]);
                }
            }
            // DEBUG ----
#endif

            // Fill out the next sub-device slot
            dd_num_sensors = sns_dd_fill_sub_dev_slot( &state->sub_dev[state->sub_dev_count],
                                                       smgr_handle,
                                                       device_info->first_gpio,
                                                       *num_sensors,
                                                       *sensors );

            DD_I2C_DB_3(0x0000, 0x0B, dd_num_sensors);

            if (dd_num_sensors == 0)
            {
                DD_I2C_DB_2(0x0000, 0x0D);

                DD_MSG_0(ERROR, "init - Unable to initialize any sensors.");
                return SNS_DDF_EFAIL;
            }

            // Assign the dd_handle_ptr
            dd_handle = (sns_ddf_handle_t) ((uint32_t) state | state->sub_dev_count);
            *dd_handle_ptr = dd_handle;

            // init timers
            status = sns_dd_init_timer(state, state->sub_dev_count, (sns_ddf_sensor_e *)*sensors);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x0000, 0x0E);

                DD_MSG_0(ERROR, "init - Failed to init timers");
                return status;
            }

            // parse registry
            status = sns_dd_read_registry(state, state->sub_dev_count, nv_params);

            // Reset the sub-device
            status = sns_dd_ams_tmg399x_reset(dd_handle);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x0000, 0x0F);

                DD_MSG_0(ERROR, "init - Failed to reset tmg399x");
                return status;
            }

            //*num_sensors = dd_num_sensors;
            *sensors = (sns_ddf_sensor_e *)state->sub_dev[state->sub_dev_count].sensors;

            state->sub_dev_count++;

            status = SNS_DDF_SUCCESS;
        }
        else
        {
            DD_I2C_DB_2(0x0000, 0x10);

            DD_MSG_0(ERROR, "Out of sub-device slots.");
            return SNS_DDF_ENOMEM;
        }
    }

    // DEBUG ++++
    {
        DD_I2C_DB_3(0x0000, 0x11, *num_sensors);

        for(i = 0; i < *num_sensors; ++i)
        {
            DD_I2C_DB_3(0x0000, 0x12, (*sensors)[i]);
        }

        DD_MSG_1(HIGH,"sns_dd_i1it - exit: %d", status);
    }
    // DEBUG ----

    return status;
}


/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_prx_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for proximitydistance data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_prx_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t*             thresh_ptr;
  uint16_t*             accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(0);
      device_ranges->max = FX_FLTTOFIX_Q16(SNS_DD_PRX_RES); /* 5 cm = 0.05m */
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      resp->bit_len  = SNS_DD_ALSPRX_PRX_BITS;
      resp->max_freq = SNS_DD_ALSPRX_PRX_FREQ;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = FX_FLTTOFIX_Q16(SNS_DD_PRX_RES);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current       = SNS_DD_PRX_PWR;
      power_ptr->lowpower_current     = SNS_DD_ALSPRX_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = SNS_DD_PRX_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = SNS_DD_PRX_THRESH_NEAR;
      *(uint16_t **)value = thresh_ptr;
      DD_MSG_0(HIGH, "get_attrib threshold");

      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.prox.odr;
      *(uint32_t **)value = odr_ptr;
      DD_MSG_1(HIGH, "TMG399x get_attrib prx odr: %d",  *odr_ptr);
      break;

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_als_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_als_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t*             accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr range");
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(SNS_DD_ALS_RANGES_MIN);
      device_ranges->max = FX_FLTTOFIX_Q16(SNS_DD_ALS_RANGES_MAX);
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr resolution ADC");
      *num_elems = 1;
      resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
      resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr resolution");
      *num_elems = 1;

      *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current   = SNS_DD_ALS_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
     *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
      if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
      {
        dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
      }
      DD_MSG_0(HIGH, "accuracy");
      *accu_ptr = dd_ptr->chip.als_info.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.als.odr;
      *(uint32_t **)value = odr_ptr;
      DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
      break;

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_rgb_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    //
    // handle generic attributes for RGB
    //
    // Same as ALS
    //
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.rgb.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_ct_c_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    // handle generic attributes for CT_C
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.ct_c.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_ir_gesture_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    // handle generic attributes for gesture
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.gesture.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  Returns the requested attribute

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor      Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in]  memhandler  Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    Success if the attribute was retrieved and the buffer was
    populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_get_attrib
(
  sns_ddf_handle_t      handle,
  sns_ddf_sensor_e      sensor,
  sns_ddf_attribute_e   attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)handle & ((uint32_t)DD_HANDLE_ALIGN);

    sns_ddf_status_e           status         = SNS_DDF_SUCCESS;
    bool                       generic_attrib = false;
    sns_ddf_registry_group_s  *reg_group;
    uint8_t                   *reg_group_data;
    sns_ddf_driver_info_s     *driver_info_ptr;
    sns_ddf_driver_info_s      driver_info_l = {
        "TMG399X_ALS_Prox",  /* name */
        1                    /* version */
    };

    sns_ddf_driver_info_s  driver_info_2 = {
        "TMG399X_RGB_CT",    /* name */
        1                    /* version */
    };

    sns_ddf_driver_info_s  driver_info_3 = {
        "TMG399X_Gesture",    /* name */
        1                    /* version */
    };

    sns_ddf_device_info_s *device_info_ptr;

    sns_ddf_device_info_s  device_info_l = {
        "ALS_PRX",     /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    sns_ddf_device_info_s  device_info_2 = {
        "RGB_CT",      /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    sns_ddf_device_info_s  device_info_3 = {
        "Gesture",     /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    DD_I2C_DB_3(0x3000, 0x00, sensor);
    DD_I2C_DB_3(0x3000, 0x01, attrib);
    DD_I2C_DB_3(0x3000, 0x02, *num_elems);
    DD_I2C_DB_3(0x3000, 0x03, sub_dev_id);

    if ( (handle == NULL)      ||
         (memhandler == NULL) )
    {
        DD_I2C_DB_2(0x3000, 0x04);

        return SNS_DDF_EINVALID_PARAM;
    }


    /* check first if the query is for generic attributes */
    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
        DD_I2C_DB_2(0x3000, 0x05);

        if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x06);

            return SNS_DDF_ENOMEM;
        }

        if(sub_dev_id == subDevId_AlsProx)
        {
            DD_I2C_DB_2(0x3000, 0x07);

            *driver_info_ptr = driver_info_l;
        }
        else if(sub_dev_id == subDevId_RgbCt)
        {
            DD_I2C_DB_2(0x3000, 0x08);

            *driver_info_ptr = driver_info_2;
        }
        else if(sub_dev_id == subDevId_Gesture)
        {
            DD_I2C_DB_2(0x3000, 0xF8);

            *driver_info_ptr = driver_info_3;
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0x09);

            return SNS_DDF_EINVALID_PARAM;
        }

        *(sns_ddf_driver_info_s**)value = driver_info_ptr;
        *num_elems = 1;
        generic_attrib = false;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        DD_I2C_DB_2(0x3000, 0x0A);

        if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x0B);

            return SNS_DDF_ENOMEM;
        }

        if(sub_dev_id == subDevId_AlsProx)
        {
            DD_I2C_DB_2(0x3000, 0xA1);

            *device_info_ptr = device_info_l;
        }
        else if(sub_dev_id == subDevId_RgbCt)
        {
            DD_I2C_DB_2(0x3000, 0xA2);

            *device_info_ptr = device_info_2;
        }
        else if(sub_dev_id == subDevId_Gesture)
        {
            DD_I2C_DB_2(0x3000, 0xA3);

            *device_info_ptr = device_info_3;
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0xA4);

            return SNS_DDF_EINVALID_PARAM;
        }

        *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
        *num_elems = 1;
        generic_attrib = false;
        return SNS_DDF_SUCCESS;


    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
        DD_I2C_DB_2(0x3000, 0x0C);

        DD_MSG_0(HIGH, "in attrib reg group");
        if ( (reg_group = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x0D);
            DD_MSG_0(ERROR, "reg_group no mem");

            return SNS_DDF_ENOMEM;
        }

        if (state->nv_db_size == 0)
        {
            DD_I2C_DB_2(0x3000, 0x0E);

            reg_group->group_data = NULL;
            reg_group->size       = 0;

            DD_MSG_0(HIGH, "nv db size 0");
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0x0F);

            if ( (reg_group_data = sns_ddf_memhandler_malloc(memhandler, state->nv_db_size)) == NULL )
            {
                DD_MSG_0(ERROR, "reg_group no mem");

                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "at memcopy");

            SNS_OS_MEMSCPY(reg_group_data, sizeof(sns_dd_nv_db_type), &state->nv_db, sizeof(sns_dd_nv_db_type));
            reg_group->group_data = reg_group_data;
            reg_group->size = state->nv_db_size;
            DD_MSG_0(HIGH, "after memcopy");
        }

        *(sns_ddf_registry_group_s**)value = reg_group;
        *num_elems = 1;
        generic_attrib = true;
        DD_MSG_0(HIGH, "out of attrib group");
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
    {

        *value = &tmg399x_odr_list;
        *num_elems = tmg399x_odr_list_size;
        generic_attrib = true;

        return SNS_DDF_SUCCESS;

    }


    default:
        DD_I2C_DB_2(0x3000, 0x11);

        /* do nothing */
        break;
    }

    /* The query is not for generic attribute but is for specific data type */
    if ( generic_attrib != true )
    {
        DD_I2C_DB_2(0x3000, 0x12);

        switch ( sensor )
        {
        case SNS_DDF_SENSOR_AMBIENT:
            DD_I2C_DB_2(0x3000, 0x13);

            status = sns_dd_ams_tmg399x_als_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_PROXIMITY:
            DD_I2C_DB_2(0x3000, 0x14);

            status = sns_dd_ams_tmg399x_prx_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_RGB:
            DD_I2C_DB_2(0x3000, 0x15);

            status = sns_dd_ams_tmg399x_rgb_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_CT_C:
            DD_I2C_DB_2(0x3000, 0x16);

            status = sns_dd_ams_tmg399x_ct_c_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_IR_GESTURE:
            DD_I2C_DB_2(0x3000, 0x17);

            status = sns_dd_ams_tmg399x_ir_gesture_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        default:
            DD_I2C_DB_2(0x3000, 0x18);

            status = SNS_DDF_EINVALID_ATTR;
        }
    }

    DD_I2C_DB_2(0x3000, 0xFF);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the TMG 399x ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value       Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status     = SNS_DDF_SUCCESS;

    DD_I2C_DB_3(0x2000, 0x00, attrib);
    DD_I2C_DB_3(0x2000, 0x01, sensor);

    DD_MSG_0(MED, "setAttrib: ");

    if ( (dd_handle == NULL) ||
         (value == NULL) ||
         (sub_dev_id ==  0xFF))
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
        DD_I2C_DB_2(0x2000, 0x02);
        DD_MSG_0(MED, "setAttrib: power mode set");
        status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                           *(sns_ddf_powerstate_e *)value,
                                                           sensor,
                                                           TRUE);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(MED, "setAttrib: ");
            return status;
        }

        ams_calc_cpl(&state->chip);

        break;

    case SNS_DDF_ATTRIB_ODR:
        DD_I2C_DB_2(0x2000, 0x03);

        DD_MSG_1(MED, "setAttrib: set odr to: %d", *(uint32_t*)value);

        // State machine
        ams_tmg399x_state_machine(state, sensor, ((*(uint32_t*)value) != 0));
        // This second call to state_machine ensures first data notification
        // for als/rgb/ct in standalone mode is sent out. Without this call
        // the first interrupt does not fire. TODO - find out why this behavior
        ams_tmg399x_state_machine(state, sensor, ((*(uint32_t*)value) != 0));

        switch(sensor)
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            DD_I2C_DB_2(0x2000, 0x04);
            state->chip.setup.prox.odr = *(uint32_t*)value;

            //if (state->chip.setup.prox.odr > 5)
            //{
            //    state->chip.setup.prox.odr = 5;
            //    DD_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.prox.odr);
            //}

            break;

        case SNS_DDF_SENSOR_AMBIENT:
            state->chip.setup.als.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_RGB:
            state->chip.setup.rgb.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_CT_C:
            DD_I2C_DB_2(0x2000, 0x05);
            state->chip.setup.ct_c.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_IR_GESTURE:
            DD_I2C_DB_2(0x2000, 0x06);
            state->chip.setup.gesture.odr = *(uint32_t*)value;
            break;

        default:
            DD_I2C_DB_2(0x2000, 0x07);
            break;
        }

        ams_calc_cpl(&state->chip);

        break;

    default:
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_3(MED, "setAttrib: : %d final wtime: %d final ltime: %d", 1157, state->chip.setup.wait.wtime, state->chip.setup.wait.lTime);

    /* status = ams_write_wtime(state); */

    DD_I2C_DB_2(0x2000, 0xFF);

    return(status);
}

static void ams_set_lux_coefs
(
 sns_dd_state_t* state
)
{
    int i;


    for(i = 0; i < AMS_MAX_NUM_OF_399x_DEVICES; ++i)
    {
        state->chip.lux_coef[i].d_factor  = ams_tmg399x_lux_coef[i].d_factor;
        state->chip.lux_coef[i].r_coef    = ams_tmg399x_lux_coef[i].r_coef;
        state->chip.lux_coef[i].g_coef    = ams_tmg399x_lux_coef[i].g_coef;
        state->chip.lux_coef[i].b_coef    = ams_tmg399x_lux_coef[i].b_coef;
        state->chip.lux_coef[i].ct_coef   = ams_tmg399x_lux_coef[i].ct_coef;
        state->chip.lux_coef[i].ct_offset = ams_tmg399x_lux_coef[i].ct_offset;
    }
}

static sns_ddf_status_e ams_initSensorPor
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;


    state->chip.shadow.val[AMS_REG_ENABLE] = 0x00;

    return status;
}


/*
 * Provides initial operational parameter defaults.
 * These defaults may be changed through the device's get/set
 * attribute files.
 */
static sns_ddf_status_e  ams_set_default_reg_values
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e  status;
    uint8_t           devId;


    DD_MSG_1(HIGH, "in set defualt reg values ID=0x%X", state->chip.shadow.val[AMS_REG_ID]);

    // Set all register to power on reset value as defined in data sheet
    status = ams_i2c_write_buf(state, AMS_REG_ENABLE, MAX_NUM_OF_REGS_TO_RESET, (uint8_t*)ams_power_on_reset_values);
    if ( status != SNS_DDF_SUCCESS )
    {
        return status;
    }

    status = ams_i2c_read_buf(state, AMS_REG_ENABLE, (AMS_REG_CONFIG_AF - AMS_REG_ENABLE + 1), &state->chip.shadow.val[AMS_REG_ENABLE]);

    status = ams_i2c_read_buf(state, AMS_REG_REVID, 2, &state->chip.shadow.val[AMS_REG_REVID]);
    devId  = ams_getField_s(state, ID);
    if ( status != SNS_DDF_SUCCESS ) {
        DD_I2C_DB_2(0x0200, 2);
        return status;
    }

    if(!ams_validDeviceId(devId))
    {
        DD_MSG_0(MED, "defaultRegValues: device id invalid");

        return SNS_DDF_EFAIL;
    }

    switch(devId)
    {
    case 0x27:       // tmg3992
        state->chip.device_index = 0;
        break;

    case 0x2A:       // tmg3993
        state->chip.device_index = 1;
        break;

    default:    // UNKNOWN device ERROR!!!!
        state->chip.device_index = 0;
        return SNS_DDF_EFAIL;
        break;
    }

    // Operational parameters
    ams_clearShadowChangedFlag(state);
    ams_set_lux_coefs(state);

    state->chip.Min_Integ_Time_us = ams_tmg399x_min_integ_time_us[state->chip.device_index];

    ams_initSensorPor(state);

    DD_MSG_1(MED, "defaultRegValues: AMS_REG_ENABLE: 0x%02X", state->chip.shadow.val[AMS_REG_ENABLE]);
    status = ams_i2c_write_byte(state, AMS_REG_ENABLE, state->chip.shadow.val[AMS_REG_ENABLE]);

    state->chip.als_info.atime = ams_getField_s(state, ATIME);
    state->chip.als_info.again = ams_getField_s(state, AGAIN);
    ams_calc_cpl(&state->chip);

    state->chip.als_gain_auto      = true;

    state->chip.setup.prox.detect  = PROX_DETECT_CNT;
    state->chip.setup.prox.release = PROX_RELEASE_CNT;

    ams_setField_s(state, PITHL, 0x00);
    ams_setField_s(state, PITHH, state->chip.setup.prox.detect);

    state->chip.prx_info.event         = SNS_PRX_FAR_AWAY;

    state->chip.setup.als.luxCorrection = 1 << 10;

    state->chip.setup.als.odr                 = 0;
    state->chip.setup.rgb.odr                 = 0;
    state->chip.setup.ct_c.odr                = 0;
    state->chip.setup.prox.odr                = 0;
    state->chip.setup.gesture.odr             = 0;

    DD_I2C_DB_2(0x0200, 0xFF);

    return status;
}


/*===========================================================================

FUNCTION      ams_set_reg_values_from_nv

DESCRIPTION   Reads values from registry NV param store and assigns to sensor

DEPENDENCIES  None

RETURN VALUE  No Return

SIDE EFFECT   None

===========================================================================*/


static void ams_set_reg_values_from_nv(sns_dd_state_t* state)
{
    int di = state->chip.device_index;


    DD_I2C_DB_2(0x0300, 0);

    // Now set NV values

    // device
    if((state->nv_db.device != 0) && (state->nv_db.device <= 9))
    {
        /* state->chip.ams_sensor = (AMS_SENSOR_TYPE)state->nv_db.device; */
    }

    // d_factor
    if(state->nv_db.d_factor != 0)
    {
        state->chip.lux_coef[di].d_factor = state->nv_db.d_factor;
    }

    // r_coef
    if(state->nv_db.r_coef != 0)
    {
        state->chip.lux_coef[di].r_coef = state->nv_db.r_coef;
    }

    // g_coef
    if(state->nv_db.g_coef != 0)
    {
        state->chip.lux_coef[di].g_coef = state->nv_db.g_coef;
    }

    // b_coef
    if(state->nv_db.b_coef != 0)
    {
        state->chip.lux_coef[di].b_coef = state->nv_db.b_coef;
    }

    // ct_coef
    if(state->nv_db.ct_coef != 0)
    {
        state->chip.lux_coef[di].ct_coef = state->nv_db.ct_coef;
    }

    // ct_offset
    if(state->nv_db.ct_offset != 0)
    {
        state->chip.lux_coef[di].ct_offset = state->nv_db.ct_offset;
    }

    // proxDetect
    if(state->nv_db.proxDetect != 0)
    {
        state->chip.setup.prox.detect = state->nv_db.proxDetect;
    }

    // proxRelease
    if(state->nv_db.proxRelease != 0)
    {
        state->chip.setup.prox.release = state->nv_db.proxRelease;
    }

    // luxCorrection
    if(state->nv_db.luxCorrection != 0)
    {
        state->chip.setup.als.luxCorrection = state->nv_db.luxCorrection;
        DD_MSG_1(MED, "lux corr written in default %d", state->chip.setup.als.luxCorrection);
    }
}

static sns_ddf_status_e ams_update_registers
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x1400, 0);

    // Reset the device

    DD_I2C_DB_2(0x1400, 1);

    return status;
}


static sns_ddf_status_e ams_resetRegisters
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x5500, 0);

    // Reset driver state
    ams_set_default_reg_values(state);

    // Now set the usable values
    ams_set_reg_values_from_nv(state);

    // Set register values
    status = ams_update_registers(state);

    return status;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_reset

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
sns_ddf_status_e sns_dd_ams_tmg399x_reset
(
 sns_ddf_handle_t dd_handle
)
{
    sns_dd_state_t*  state           = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id      = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status          = SNS_DDF_SUCCESS;
    bool             reset_registers = true;

    DD_I2C_DB_2(0x6000, 0x00);
    DD_I2C_DB_3(0x6000, 0x01, sub_dev_id);
    DD_I2C_DB_3(0x6000, 0x11, subDevId_AlsProx);
    DD_I2C_DB_3(0x6000, 0x12, subDevId_RgbCt);

    /* basic sanity check */
    if ( dd_handle == NULL || sub_dev_id >= DD_NUM_SUB_DEV )
    {
        DD_I2C_DB_2(0x6000, 0x02);

        return SNS_DDF_EINVALID_PARAM;
    }

    // state variables reset
    state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    state->sub_dev[sub_dev_id].odr = 0;

    DD_MSG_3(HIGH,"sns_dd_reset - enter sub_dev_id=%d subDevId_RgbCt=%d subDevId_AlsProx=%d ", sub_dev_id, subDevId_RgbCt, subDevId_AlsProx);

    if(subDevId_AlsProx != -1)
    {
        DD_I2C_DB_2(0x6000, 0x03);

        if(state->sub_dev[subDevId_AlsProx].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0x04);
            DD_MSG_0(HIGH,"sub device Als/Prox is active");

            reset_registers = false;
        }

        // MW test
        //sns_ddf_smgr_notify_event(state->sub_dev[subDevId_AlsProx].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
    }

    if(subDevId_RgbCt != -1)
    {
        DD_I2C_DB_2(0x6000, 0x05);

        if(state->sub_dev[subDevId_RgbCt].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0x06);
            DD_MSG_0(HIGH,"sub device Rgb/Ct is active");

            reset_registers = false;
        }
    }

    if(subDevId_Gesture != -1)
    {
        DD_I2C_DB_2(0x6000, 0x05);

        if(state->sub_dev[subDevId_Gesture].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0xF6);
            DD_MSG_0(HIGH,"sub device Gesture is active");

            reset_registers = false;
        }
    }

    if( reset_registers )
    {
        DD_I2C_DB_2(0x6000, 0x07);

        // Configure device registers - initiate device reset
        DD_MSG_0(HIGH,"reseting sensor device");

        status = ams_resetRegisters(state);

        currentState  = AMS_STATE_OFF;         // Idle
        currentSensor = AMS_SENSOR_NONE;  // undefined

        // No sensors active:
        memset(&currentlyActiveSensors[0], 0, sizeof(currentlyActiveSensors));
    }

    DD_MSG_0(HIGH,"sns_dd_reset - exit");

    DD_I2C_DB_2(0x6000, 0xFF);

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_run_test

DESCRIPTION   OEM Self Test to calibrate ALS Lux. Takes several lux readings
              and calculates scaling factor for ALS reading

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status     = SNS_DDF_EINVALID_TEST;
    sns_ddf_handle_t smgr_handle;
    uint16_t         deviceId;
    uint16_t         prxLoopCnt = 0;


    /* basic sanity check */
    if ( (state == NULL) || (sub_dev_id >= DD_NUM_SUB_DEV) )
    {
        DD_MSG_0(HIGH, "run_test: Sanity Check FAILED");

        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_0(HIGH, "run_test: ");

    switch(sensor)
    {
    case SNS_DDF_SENSOR_AMBIENT:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: ");

        // Test the ALS
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: SNS_DDF_TEST_SELF");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        case SNS_DDF_TEST_OEM:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: SNS_DDF_TEST_OEM");

            // Perform ams specific test to Calibrate ALS
            if(state->nv_db.calibratePhone)
            {
                // Set Default values
                //fv Read NV
                //fv ams_set_default_reg_values(state);

                status = sns_dd_ams_tmg399x_reset(dd_handle);
                DD_MSG_0(HIGH, "run_test: after reset");
                if( status != SNS_DDF_SUCCESS )
                {
                    DD_MSG_0(HIGH, "run_test: set def reg values failed ");
                    return status;
                }

                /* alsTime_us  = 0; */
                /* proxTime_us = 0; */

                state->chip.setup.als.odr = 10;

                //  enable als and prox. sensor
                status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                                   SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);

                ams_tmg399x_state_machine(state, SNS_DDF_SENSOR_AMBIENT, true);

                //alsTime_us  = ams_als_time_us(state);
                /* proxTime_us = ams_prox_time_us(state); */
                //ams_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us);

                //ams_compute_new_wtime(state, state->chip.setup.prox.odr, proxTime_us);
                //status = ams_write_wtime(state);
                sns_ddf_delay(125000);

                // update als scaling
                DD_MSG_0(HIGH, "run_test: into als scaling");
                status = ams_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    DD_MSG_0(HIGH, "run_test: calib als failed ");
                    return status;
                }

                DD_MSG_0(HIGH, "run_test: out of als scaling");
                smgr_handle = state->sub_dev[sub_dev_id].smgr_handle;
                DD_MSG_0(HIGH, "run_test: reg update notify next");

                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "run_test: Could not notify SMGR");

                    return status;
                }
                DD_MSG_0(HIGH, "run_test: successful reg update notify");
            }
            else  // do not calibratePhone
            {
                DD_MSG_0(HIGH, "run_test: do not calibratePhone");

                return SNS_DDF_EFAIL;
            }
            break;

        default:
            return SNS_DDF_EINVALID_TEST;
        }
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: ");

        /* TODO: Add Prox calibration */
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        case SNS_DDF_TEST_OEM:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: SNS_DDF_TEST_OEM: ");

            prxLoopCnt = 0;

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: init prox");

            if((status = sns_dd_ams_tmg399x_reset(dd_handle)) != SNS_DDF_SUCCESS) return status;

            //  enable als and prox. sensor
            state->chip.setup.als.odr = 10;
            ams_tmg399x_state_machine(state, SNS_DDF_SENSOR_PROXIMITY, true);
            if((status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                                   SNS_DDF_POWERSTATE_ACTIVE,
                                                                   SNS_DDF_SENSOR__ALL, TRUE)) != SNS_DDF_SUCCESS) return status;

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: SNS_PRX_FAR_AWAY");
            while(state->chip.prx_info.event != SNS_PRX_NEAR_BY)
            {
                ams_readDataRegisters(state);
                ams_get_prox_data(state);
                sns_dd_ams_tmg399x__alsprx_prx_binary(state);
                sns_ddf_delay(10000);

                if(++prxLoopCnt > 200)
                {
                    DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: Failed");
                    return SNS_DDF_EFAIL;
                }
                DD_MSG_1(HIGH, "run_test: PRX-TEST-OEM: prxLoopCnt: %d", prxLoopCnt);
            }
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: SNS_PRX_NEAR_BY");

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: Get smgr handle");
            smgr_handle = state->sub_dev[sub_dev_id].smgr_handle;
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: reg update notify next");
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: successful reg update notify");
            break;

        default:
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: NOT SNS_DDF_TEST_OEM");
            return SNS_DDF_EINVALID_TEST;
            break;
        }
        break;

    case SNS_DDF_SENSOR_RGB:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    case SNS_DDF_SENSOR_CT_C:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    case SNS_DDF_SENSOR_IR_GESTURE:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    default:  // UNKNOWN SENSOR
        DD_MSG_0(HIGH, "run_test: RUN_TEST: UNKNOWN SENSOR");
        return SNS_DDF_EINVALID_PARAM;
        break;
    }


    return status;
}


sns_ddf_status_e sns_dd_ams_tmg399x_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
 )
{
    sns_ddf_handle_t    port_handle;
    sns_ddf_status_e    ret = SNS_DDF_SUCCESS;
    uint8_t             chipId;
    uint8_t             read_count;
    uint8_t             revId[2] = {0};

    DD_MSG_0(HIGH, "sns_dd_ams_tmg399x_probe - enter");

    /* Basic sanity check*/
    if (device_info == NULL || sensors == NULL || num_sensors == NULL)
    {
        DD_MSG_0(ERROR, "SNS_DDF_EINVALID_PARAM");
        return SNS_DDF_EINVALID_PARAM;
    }

    *num_sensors = 0;
    *sensors = NULL;

    /* open I2C port */
    ret = sns_ddf_open_port(&port_handle, &device_info->port_config);
    if (ret != SNS_DDF_SUCCESS)
    {
        DD_MSG_1(ERROR, "open_port failed %d", ret);
        return ret;
    }

    /* Read Chip ID register */
    ret = sns_ddf_read_port(port_handle,
                            regAddr[AMS_REG_REVID],
                            &revId[0],
                            2,
                            &read_count);
    if(ret != SNS_DDF_SUCCESS)
    {
        DD_MSG_1(ERROR, "read_port failed %d", ret);
        return ret;
    }

    chipId = revId[1];

    /* Check if data read is what is expected */
    if(!ams_validDeviceId(chipId >> 2))  /* remove VID from ID */
    {
        DD_MSG_1(ERROR, "Bad chip ID 0x%X", chipId);
        return SNS_DDF_EINVALID_DATA;
    }


    // Found device
    *num_sensors = 5;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
        (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
        (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
        (*sensors)[2] = SNS_DDF_SENSOR_RGB;          // RGB
        (*sensors)[3] = SNS_DDF_SENSOR_CT_C;         // CT_C
        (*sensors)[4] = SNS_DDF_SENSOR_IR_GESTURE;   // IR_GESTURE
        ret = SNS_DDF_SUCCESS;
    }
    else
    {
        DD_MSG_0(ERROR, "malloc failed ");
        ret = SNS_DDF_ENOMEM;
    }

    sns_ddf_close_port(port_handle);
    return ret;
}


sns_ddf_status_e sns_dd_read_registry
(
 sns_dd_state_t*      state,
 uint32_t             sub_dev_id,
 sns_ddf_nv_params_s* reg_params
 )
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x0400, 0);

    // handle registry if applicable
    DD_MSG_0(HIGH, "readRegy: set def values");

    state->nv_db_size = 0;      // variable to store size of nv_

    // Load default values in driver - these will be overwritten if
    // registry has good values. if not, these will be written back
    // to nv_db
    ams_set_default_reg_values(state);

    // Recall NV parameters, if any
    if(reg_params)
    {
        bool data_from_registry = false;

        DD_I2C_DB_2(0x0400, 1);

        DD_MSG_0(HIGH, "readRegy: yes reg params");
        if ( (reg_params->status   == SNS_DDF_SUCCESS) &&
             (reg_params->data_len >= sizeof(sns_dd_nv_db_type))
             )
        {
            DD_MSG_0(HIGH, "readRegy: reg params match nv db type");
            sns_dd_nv_db_type *nv_ptr = (sns_dd_nv_db_type *)reg_params->data;
            if ( (nv_ptr->ams_num == ams_magic_num) &&
                 (nv_ptr->drv_ID  == ams_driver_ID))
            {
                DD_I2C_DB_2(0x0400, 2);

                // update value from NV parameters
                DD_MSG_0(HIGH, "readRegy: magic num verified");
                //sns_dd_alsprx_common_db is defined in tmg399x_priv.h file
                data_from_registry = true;
                state->nv_db_size    = reg_params->data_len;

                SNS_OS_MEMSCPY(&state->nv_db, sizeof(sns_dd_nv_db_type), nv_ptr, sizeof(sns_dd_nv_db_type));
                DD_MSG_0(HIGH, "readRegy: in reg params past memcopy");
            }
        }

        if (!data_from_registry)
        {
            int di = state->chip.device_index;

            DD_I2C_DB_2(0x0400, 3);
            DD_I2C_DB_3(0x0400, 0x31, AMS_SENSOR);
            DD_I2C_DB_3(0x0400, 0x32, ams_driver_ID);


            DD_MSG_0(HIGH, "readRegy: in no reg data");

            state->nv_db_size           = REG_FIXED_NV_SIZE_128; //defined as 128 max

            state->nv_db.visible_ratio  = SNS_DD_VISIBLE_TRANS_RATIO;
            state->nv_db.ir_ratio       = SNS_DD_IR_TRANS_RATIO;
            state->nv_db.dc_offset      = SNS_DD_DC_OFFSET;
            state->nv_db.thresh_near    = SNS_DD_PRX_THRESH_NEAR;
            state->nv_db.thresh_far     = SNS_DD_PRX_THRESH_FAR;
            state->nv_db.prx_factor     = SNS_DD_PRX_FACTOR;
            state->nv_db.als_factor     = SNS_DD_ALS_FACTOR;

            state->nv_db.ams_num        = ams_magic_num;
            state->nv_db.drv_ID         = ams_driver_ID;
            state->nv_db.calibratePhone = 1;
            state->nv_db.device         = AMS_SENSOR;
            state->nv_db.calLuxLower    = 250;      //avg set as 300
            state->nv_db.calLuxUpper    = 500;

            state->nv_db.d_factor       = state->chip.lux_coef[di].d_factor;
            state->nv_db.r_coef         = state->chip.lux_coef[di].r_coef;
            state->nv_db.g_coef         = state->chip.lux_coef[di].g_coef;
            state->nv_db.b_coef         = state->chip.lux_coef[di].b_coef;
            state->nv_db.ct_coef        = state->chip.lux_coef[di].ct_coef;   // New
            state->nv_db.ct_offset      = state->chip.lux_coef[di].ct_offset; // New

            state->nv_db.proxDetect     = PROX_DETECT_CNT;
            state->nv_db.proxRelease    = PROX_RELEASE_CNT;
            state->nv_db.luxCorrection  = 1 << 10;
            DD_MSG_0(HIGH, "readRegy: past setting defaults for reg data");
            /*  SNS_OS_MEMZERO(state->nv_db.reserved, sizeof(state->nv_db.reserved)); (no reserved space here)*/
        }

        // Now set the usable values
        ams_set_reg_values_from_nv(state);
        DD_MSG_0(HIGH, "readRegy: setting defaults from nv");

        if (state->nv_db.calibratePhone)
        {
            state->calibration_mode = true;
            DD_MSG_0(HIGH, "readRegy: setting phone calib  true");
        }

        // Notify to write back to registry in case defaults had to be loaded.
        if ( (reg_params) && (!data_from_registry) )
        {
            DD_I2C_DB_2(0x0400, 4);

            DD_MSG_0(HIGH, "readRegy: before notify");
#ifdef USE_DD_I2C_DB
            // Do not notify SMGR of event
#else
            sns_ddf_smgr_notify_event(state->sub_dev[sub_dev_id].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
#endif
            DD_MSG_0(HIGH, "readRegy: after notify");

            DD_I2C_DB_2(0x0400, 5);
        }
    }
    DD_MSG_1(HIGH, "readRegy: NV size = %d", state->nv_db_size);
    return status;
}

#define NUM_OF_LUX_TO_AVE  8


static sns_ddf_status_e ams_compute_lux_ave
(
 sns_dd_state_t* state
)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    luxSum = 0;

    DD_MSG_0(HIGH, "about to get als data and mlux ");

    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        ams_readDataRegisters(state);
        status = ams_get_als_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(HIGH, "failed in get als data ");
            /* return status; */
        }

        status = ams_get_lux(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(HIGH, "failed in app get mlux ");
            /* return status; */
        }

        luxSum += state->chip.als_info.lux;
        DD_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        // Wait some time before another reading
        sns_ddf_delay(15000);
        DD_MSG_0(HIGH, "after 15000 us delay ");
    }

    state->chip.als_info.luxAve = (luxSum / NUM_OF_LUX_TO_AVE);
    DD_MSG_2(MED, "luxsum %d luxsum/8: %d", luxSum, ((luxSum / NUM_OF_LUX_TO_AVE)));

    return SNS_DDF_SUCCESS;
}



/*===========================================================================

FUNCTION      ams_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e ams_calibrate_als
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    uint32_t  tgtLux;
    uint32_t  avgLux;

    avgLux = 1;
    tgtLux = 1;

    // compare the average lux to the passed in calibration lux,
    DD_MSG_0(HIGH, "CAL_ALS: in calibrate als next compute lux ");
    status = ams_compute_lux_ave(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DD_MSG_0(HIGH, "CAL_ALS: computelux ave failed ");
        return status;
    }
    DD_MSG_0(HIGH, "CAL_ALS: out of compute lux ");

    // Find Lux mid way between calLuxLower and calLuxUpper
    tgtLux = (state->nv_db.calLuxLower + state->nv_db.calLuxUpper) / 2;
    DD_MSG_2(MED, "CAL_ALS: tgtLux: %d callxlw: %d", tgtLux, state->nv_db.calLuxLower);
    //tgtLux= tgtLux;    // removed left shift by 1024
    avgLux = state->chip.als_info.luxAve;

    DD_MSG_2(MED, "CAL_ALS: tgtLux<<10: %d callxhi: %d", tgtLux, state->nv_db.calLuxUpper);

    // Mult by 2^10
    state->nv_db.luxCorrection = ((tgtLux << 10) / avgLux);

    state->chip.setup.als.luxCorrection = state->nv_db.luxCorrection;

    DD_MSG_2(MED, "CAL_ALS: avgLux: %d new val of state->nv_db.luxCorrection: %d", avgLux, state->nv_db.luxCorrection);

    // To use:
    // ((lux * state->nv_db.luxCorrection) + 512) / 1024;
    // or
    // ((lux * state->nv_db.luxCorrection) + 512) >> 10;

    return status;
}

/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x__alsprx_set_powerstate

DESCRIPTION   Sets the power state of the AMS ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_ams_tmg399x__alsprx_set_powerstate
(
  sns_dd_state_t*        state,
  uint32_t               sub_dev_id,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  bool                   init_data
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    int i;
    bool activeSensors = false;

    DD_MSG_0(HIGH, "SetPwrState: ENTER");

    if(op_mode == SNS_DDF_POWERSTATE_ACTIVE )
    {
        ams_i2cWriteField_s(state, PON, 1);
        state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
        DD_MSG_0(HIGH, "SetPwrState: Set PON");
    }
    else   /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
    {
        state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

        for(i = 0; i < state->sub_dev_count; ++i)
        {
            if(state->sub_dev[i].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
               activeSensors = true;
        }

        if(!activeSensors)
        {
            status = ams_i2c_write_byte(state, AMS_REG_ENABLE, 0x00);
            DD_MSG_0(HIGH, "SetPwrState: Cleared PON");
        }

    }

    state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
    state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;

    DD_MSG_0(HIGH, "SetPwrState: EXIT");

    return status;
}

static sns_ddf_status_e ams_initSensorProxGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, ATIME,   ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));
    ams_setField_s(state, PPERS,   0x00);
    ams_setField_s(state, APERS,   0x00);
    ams_setField_s(state, LDRIVE,  LDRIVE_100);
    ams_setField_s(state, PGAIN,   PGAIN_4X);
    ams_setField_s(state, AGAIN,   AGAIN_16X);
    ams_setField_s(state, PPLEN,   PPLEN_16uS);
    ams_setField_s(state, PPULSE,  4);

    ams_setField_s(state, GPENTH,  0x00);
    ams_setField_s(state, GEXTH,   0xFF);
    ams_setField_s(state, GPLEN,   GPLEN_32uS);
    ams_setField_s(state, GPULSE,  4);
    ams_setField_s(state, GGAIN,   GGAIN_4X);
    ams_setField_s(state, GLDRIVE, GLDRIVE_100);
    ams_setField_s(state, GWTIME,  GWTIME_2p8mS);
    ams_setField_s(state, GDIMS,   GDIMS_NSWE);
    /* ams_setField_s(state, PPLEN,  2); */
    /* ams_setField_s(state, PPULSE, 2); */
    /* ams_setField_s(state, GPLEN,  2); */
    /* ams_setField_s(state, GPULSE, 9); */
    /* ams_setField_s(state, GWTIME, 0); */
    status = ams_i2cWriteChangedShadowRegisters(state);             if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_GEN | AMS_MSK_AEN | AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_PON);


    return status;
}

static sns_ddf_status_e ams_initSensorGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, PITHL,  0x00);
    ams_setField_s(state, PITHH,  0xFF);

    ams_setField_s(state, ATIME,  ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));
    ams_setField_s(state, PPERS,  0);
    ams_setField_s(state, APERS,  0);
    ams_setField_s(state, LDRIVE, LDRIVE_100);
    ams_setField_s(state, PGAIN,  PGAIN_4X);
    ams_setField_s(state, AGAIN,  AGAIN_16X);
    ams_setField_s(state, PPLEN,  PPLEN_16uS);
    ams_setField_s(state, PPULSE, 4);

    ams_setField_s(state, GPENTH,  0x00);
    ams_setField_s(state, GEXTH,   0xFF);
    ams_setField_s(state, GPLEN,   GPLEN_16uS);
    ams_setField_s(state, GPULSE,  12-1);
    ams_setField_s(state, GFIFOTH, GFIFOTH_1);
    ams_setField_s(state, GEXMSK,  GEXMSK_NONE);
    ams_setField_s(state, GEXPERS, GEXPERS_1);
    ams_setField_s(state, GGAIN,   GGAIN_4X);
    ams_setField_s(state, GLDRIVE, GLDRIVE_100);
    ams_setField_s(state, GWTIME,  GWTIME_2p8mS);
    ams_setField_s(state, GDIMS,   GDIMS_NSWE);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_GEN | AMS_MSK_AEN | AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_PON);


    status = ams_i2cWriteField_r(state, GIEN, 1);

    return status;
}

static sns_ddf_status_e ams_initSensorProx
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, PITHL,  0xff);
    ams_setField_s(state, PITHH,  0x00);
    ams_setField_s(state, PPERS,  0);
    ams_setField_s(state, APERS,  2);
    ams_setField_s(state, ATIME, ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));  // 25
    ams_setField_s(state, WTIME, ams_usecToRegVal(state, MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)));  // 300
    ams_setField_s(state, PPLEN, PPLEN_4uS);
    ams_setField_s(state, PPULSE, 8);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;


    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_AEN | AMS_MSK_PON);

    if((MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)) != 0)
    {
        state->chip.shadow.val[AMS_REG_ENABLE] |= AMS_MSK_WEN;
    }

    firstProx = true;


    return status;
}

sns_ddf_status_e ams_initSensorAls
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, ATIME, ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));       // 200
    ams_setField_s(state, WTIME, ams_usecToRegVal(state, MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)));  // 300

    // DEBUG
    // Set APERS to 0 to generate an interrupt on every cycle
    /* ams_setField_s(state, PPERS, 2); */
    /* ams_setField_s(state, APERS, 2); */
    ams_setField_s(state, PPERS, 2);
    ams_setField_s(state, APERS, 0);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_AIEN | AMS_MSK_AEN | AMS_MSK_PON);

    if((MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)) != 0)
    {
        state->chip.shadow.val[AMS_REG_ENABLE] |= AMS_MSK_WEN;
    }

    return status;
}

static sns_ddf_status_e ams_initSensorAlsProx
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorProx(state);

    ams_setField_s(state, AIEN, 1);


    return status;
}


static sns_ddf_status_e ams_initSensorAlsGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorGesture(state);

    return status;
}

static sns_ddf_status_e ams_initSensorAlsProxGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorProxGesture(state);

    return status;
}

static sns_ddf_status_e ams_tmg399x_state_machine
(
  sns_dd_state_t*        state,
  sns_ddf_sensor_e       sensor,
  bool                   sensorEnable
)
{
    uint16_t         newState        = currentState;
    sns_ddf_status_e status          = SNS_DDF_SUCCESS;


    DD_MSG_0(MED, "StMach: ENTER");

    DD_I2C_DB_3(0x1200, 0x00, sensor);
    DD_I2C_DB_3(0x1200, 0x01, sensorEnable);

    switch(sensor)
    {
    case SNS_DDF_SENSOR_AMBIENT:
        currentSensor = AMS_SENSOR_ALS;
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        currentSensor = AMS_SENSOR_PROX;
        break;

    case SNS_DDF_SENSOR_IR_GESTURE:
        currentSensor = AMS_SENSOR_GESTURE;
        break;

    case SNS_DDF_SENSOR_RGB:
        currentSensor = AMS_SENSOR_RGB;
        break;

    case SNS_DDF_SENSOR_CT_C:
        currentSensor = AMS_SENSOR_CT;
        break;

    default:
        return SNS_DDF_EINVALID_DATA;
        break;
    };

    DD_I2C_DB_3(0x1200, 0x02, currentSensor);
    DD_I2C_DB_3(0x1200, 0x03, currentState);

    DD_MSG_2(MED, "StMach: currentSensor: %d sensorEnable: %d", currentSensor, sensorEnable);

    DD_MSG_2(HIGH, "StMach: ALS: %d prox: %d", currentlyActiveSensors[0], currentlyActiveSensors[1]);
    DD_MSG_2(HIGH, "StMach: RGB: %d CT:   %d", currentlyActiveSensors[3], currentlyActiveSensors[4]);
    DD_MSG_1(HIGH, "StMach: Gest: %d",         currentlyActiveSensors[2]);

    if ( sensorEnable )
    {
        currentlyActiveSensors[currentSensor] = true;

        if(currentSensor == AMS_SENSOR_RGB || currentSensor == AMS_SENSOR_CT)
        {
            currentSensor = AMS_SENSOR_ALS;
        }

        /* Set ALS thresholds to ensure first interrupt */
        if(currentSensor == AMS_SENSOR_ALS)
        {
            DD_MSG_0(MED, "StMach: ams_update_als_thresh" );
            ams_update_als_thresh(state, 0x00, 0x00, 0);
        }

        newState = enableStates[currentState][currentSensor];
    }
    else /* sensorDisable */
    {
        /* If a sensor that is being disabled is already disabled then
        don't process state machine */
        if(!currentlyActiveSensors[currentSensor])
        {
            return status;
        }

        currentlyActiveSensors[currentSensor] = false;

        if(currentSensor == AMS_SENSOR_ALS)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_RGB] && !currentlyActiveSensors[AMS_SENSOR_CT])
            {
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_RGB)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_CT] && !currentlyActiveSensors[AMS_SENSOR_ALS])
            {
                currentSensor = AMS_SENSOR_ALS;
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_CT)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_RGB] && !currentlyActiveSensors[AMS_SENSOR_ALS])
            {
                currentSensor = AMS_SENSOR_ALS;
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_PROX)
        {
            newState = disableStates[currentState][currentSensor];
        }

        if(currentSensor == AMS_SENSOR_GESTURE)
        {
            newState = disableStates[currentState][currentSensor];
        }
    }

    DD_MSG_2(HIGH, "StMach: newState: %d currentState: %d", newState, currentState);

    DD_I2C_DB_3(0x1200, 0x04, newState);

    if( newState != currentState )
    {
        DD_MSG_1(HIGH, "StMach: Change to newState: %d", newState);

        switch(newState)
        {
        case AMS_STATE_OFF:          // Off
            status = ams_initSensorPor(state);
            break;

        case AMS_STATE_ALS:          // Als
            status = ams_initSensorAls(state);
            break;

        case AMS_STATE_PRX:          // Prox
            status = ams_initSensorProx(state);
            break;

        case AMS_STATE_ALS_PRX:      // Als + Prox
            status = ams_initSensorAlsProx(state);
            break;

        case AMS_STATE_GES:          // Gesture
            status = ams_initSensorGesture(state);
            break;

        case AMS_STATE_GES_PRX:      // Gesture + Prox
            status = ams_initSensorProxGesture(state);
            break;

        case AMS_STATE_GES_ALS:      // Gesture + Als
            status = ams_initSensorAlsGesture(state);
            break;

        case AMS_STATE_GES_ALS_PRX:  // Gesture + Als + Prox
            status = ams_initSensorAlsProxGesture(state);
            break;
        }

        currentState = newState;

        status = ams_i2c_write_byte(state, AMS_REG_ENABLE, state->chip.shadow.val[AMS_REG_ENABLE]);
        DD_MSG_2(MED, "StMach: AMS_REG_ENABLE: 0x%X status: %d", state->chip.shadow.val[AMS_REG_ENABLE], status);
   }


    DD_I2C_DB_2(0x1200, 0xFF);

    DD_MSG_0(MED, "StMach: EXIT");

    return status;
}

