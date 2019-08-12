/* File: sns_dd_panasonic_gyro.c

Driver Version 1.1.2

Copyright (c) 2012, Panasonic Corporation All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of Panasonic nor the names of its contributors may
be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
  

//******************
// INCLUDES
//******************

#include "sns_ddf_attrib.h"    
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"

#include "sns_dd_panasonic_gyro.h"
#include "stdlib.h"                         // for abs()

#ifdef DDK
  #include <stdio.h>                        // Required in DDK for NULL
  #include <pio/pio.h>
  #include <board.h>
  #include <led.h>
  #include "AT91SAM7S256.h"
#else
  #ifdef GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY
    #include "DDITlmm.h"
    #include "DALDeviceId.h"
  #endif
#endif



//******************
// DEFINITIONS
//******************

#ifdef GEN2
  #define GYRO_MODEL              "EWTSAFMA"
#else
  #define GYRO_MODEL              "EWTZNA"
#endif


#define NOISE_THRESHOLD 50                  // Factory Self Test Threshold (part is ok below value)
#define OFFSET_THRESHOLD 1875               // Factory Self Test Threshold (part is ok below value)

#define TEST_TIMER_DURATION         (1000*100)  // In microseconds.  Only used by GYRO_DEBUG and GYRO_STREAM_FOR_TESTING_LEGIBLE and GYRO_FACTORY_TEST_TESTING

#define NUM_READ_BYTES              (6)
#define NUM_WRITE_BYTES             (1)
#define GYRO_NUM_AXES               (3)
#ifdef GEN2
  #define GYRO_NUM_RANGE              (4)
#else
  #define GYRO_NUM_RANGE              (3)
#endif
#define GYRO_NUM_RESOLUTION_ADC     (1)
#define GYRO_NUM_LPF                (7)

#define GYRO_ADC_RESOLUTION_MAX     (0.000174533) // (250 degrees/s) x (1/25000) x (pi/180) = LSB rad/s
#define GYRO_BIAS_XAXIS             (1.047197551) // (60 deg/sec) x (pi/180)
#define GYRO_BIAS_YAXIS             (1.047197551) // (60 deg/sec) x (pi/180)
#define GYRO_BIAS_ZAXIS             (1.047197551) // (60 deg/sec) x (pi/180)

#define GYRO_ACTIVE_CURRENT         (6010)  // 6mA AVDD + 10uA DVDD
#define GYRO_LOW_POWER_CURRENT      (12)    // 10uA AVDD + 2uA DVDD

#define GYRO_TIME_TO_ACTIVE         (300000)    // with Self Offset Correction ON
#define GYRO_TIME_TO_DATA           (500)       // Interval between data retrieval instruction received and time data is ready for reading.
                                                // We read 7 registers, which takes about 200usec.  Round up a bit.

#define GYRO_ADC_BIT_LENGTH         (16)
#define GYRO_ADC_FREQUENCY          (100)   // DLPF setting changes data rate from 8kHz (max) to 1kHz (min)

#define GYRO_LPF_0                  (7*FX_ONE_Q16)
#define GYRO_LPF_1                  (10*FX_ONE_Q16)
#define GYRO_LPF_2                  (25*FX_ONE_Q16)
#define GYRO_LPF_3                  (50*FX_ONE_Q16)
#define GYRO_LPF_4                  (100*FX_ONE_Q16)
#define GYRO_LPF_5                  (200*FX_ONE_Q16)
#define GYRO_LPF_6                  (256*FX_ONE_Q16)

#define TIMER_ID_POLLING            0
#define TIMER_ID_INIT_STEP2         1
#define TIMER_ID_INIT_STEP3         2

#define GYRO_RANGE_0_MIN            (-8.726646)     // -500 deg/sec = -8.726646 rad/sec
#define GYRO_RANGE_0_MAX            (8.726646)      // +500 deg/sec = 8.726646 rad/sec
#define GYRO_RANGE_1_MIN            (-17.453292)    // -1000 deg/sec = -17.453292 rad/sec
#define GYRO_RANGE_1_MAX            (17.453292)     // +1000 deg/sec = 17.453292 rad/sec
#define GYRO_RANGE_2_MIN            (-34.9065850)   // -2000 deg/sec = -34.9065850 rad/sec
#define GYRO_RANGE_2_MAX            (34.9065850)    // +2000 deg/sec = 34.9065850 rad/sec
#ifdef GEN2
#define GYRO_RANGE_3_MIN            (-4.363323)     // -250 deg/sec = -4.363323 rad/sec
#define GYRO_RANGE_3_MAX            (4.363323)      // +250 deg/sec = 4.363323 rad/sec
#endif

#define GYRO_INIT_DELAY_STEP2       (150 * 1000)    // 150ms per datasheet
#define GYRO_INIT_DELAY_STEP8       (100 * 1000)    // 100ms per datasheet

#ifdef GEN2
#define GYRO_GPIO_SLEEP             68
#endif

sns_ddf_driver_if_s sns_dd_panasonic_gyro_if =
{
    &sns_dd_panasonic_gyro_init,
    &sns_dd_panasonic_gyro_get_data,
    &sns_dd_panasonic_gyro_set_attrib,
    &sns_dd_panasonic_gyro_get_attrib,
    &sns_dd_panasonic_gyro_handle_timer,
    &sns_dd_panasonic_gyro_handle_irq,
    &sns_dd_panasonic_gyro_reset,
    &sns_dd_panasonic_gyro_run_test
};

#if defined GYRO_DEBUG || defined GYRO_STREAM_FOR_TESTING_LEGIBLE || defined GYRO_FACTORY_TEST_TESTING
static uint32_t m_timer_id_polling = TIMER_ID_POLLING;
#endif
//static uint32_t m_timer_id_init_step2 = TIMER_ID_INIT_STEP2;
static uint32_t m_timer_id_init_step3 = TIMER_ID_INIT_STEP3;

typedef struct {
    sns_ddf_handle_t    smgr_handle;
    sns_ddf_handle_t    port_handle;
    sns_ddf_timer_s     init_timer;
#ifdef GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY
    DalDeviceHandle     *gpio_handle;
    uint32_t            gpio_sleep_pin_cfg;
#endif
#if defined GYRO_DEBUG || defined GYRO_STREAM_FOR_TESTING_LEGIBLE || defined GYRO_FACTORY_TEST_TESTING
    sns_ddf_timer_s     polling_timer;
#endif
} sns_dd_panasonic_gyro_state_s;

sns_ddf_status_e sns_dd_panasonic_gyro_init_step2(sns_ddf_handle_t dd_handle);

sns_ddf_status_e sns_dd_panasonic_gyro_init_step3(sns_ddf_handle_t dd_handle);

sns_ddf_status_e sns_dd_panasonic_gyro_set_powerstate(sns_ddf_handle_t dd_handle, sns_ddf_powerstate_e powerstate);

sns_ddf_status_e sns_dd_panasonic_gyro_set_range(sns_ddf_handle_t dd_handle, uint8_t range_index);

sns_ddf_status_e sns_dd_panasonic_gyro_set_resolution_adc(sns_ddf_handle_t dd_handle, uint8_t resolution_adc_index);

sns_ddf_status_e sns_dd_panasonic_gyro_set_lowpass(sns_ddf_handle_t dd_handle, uint8_t lowpass_index);

sns_ddf_status_e sns_dd_panasonic_gyro_set_motion_detect(sns_ddf_handle_t dd_handle, bool motion_detect);

const char* getStatusString( sns_ddf_status_e status );

#ifdef NEW_API
sns_ddf_status_e sns_dd_panasonic_gyro_set_register_for_test();
#endif

#ifdef BIAS
static q16_t gyro_bias_xaxis_var = 0;
static q16_t gyro_bias_yaxis_var = 0;
static q16_t gyro_bias_zaxis_var = 0;
#endif



//******************
// PUBLIC FUNCTIONS
//******************

/*****************************************
    sns_dd_panasonic_gyro_init
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e**       sensors,
    uint32_t*                num_sensors)
{
    sns_ddf_status_e status;
    sns_dd_panasonic_gyro_state_s* ptr;
    uint8_t write_buffer[1] = {0};
    uint8_t write_count;
    static sns_ddf_sensor_e panasonic_gyro_sensors[] = {SNS_DDF_SENSOR_GYRO};

    
#ifdef DDK    
// Configure GPIOs for LEDs
#define PIN_LED_DS1   {1 << 0,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}   // LED #0 pin definition (PA0)
#define PIN_LED_DS2   {1 << 1,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}   // LED #1 pin definition (PA1)
#define PIN_LED_DS3   {1 << 2,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}   // LED #2 pin definition (PA2)
#define PIN_LED_DS4   {1 << 3,  AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}   // LED #3 pin definition (PA3)
    LED_Configure(1);
    //LED_Toggle(0);
    LED_Set(1);        // PIO_Clear(&pinLED1); 
    //LED_Clear(0);    // PIO_Set(&pinLED1); 
#endif // DDK    
    
    
#ifdef GYRO_DEBUG
    printf("\n\r");
    printf("\n\r sns_dd_panasonic_gyro_init - Enter init");
#endif

    if(num_devices != 1)
    {
        // only one device is supported per instance of driver
        return SNS_DDF_EINVALID_PARAM;
    }
    
    // Allocate driver instance
    status = sns_ddf_malloc((void**)&ptr, sizeof(sns_dd_panasonic_gyro_state_s));
    if(status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_init - Allocated memory");
    printf("\n\r sns_dd_panasonic_gyro_init - My handle = %u", ptr);
#endif

    ptr->smgr_handle = smgr_handle;

    // Open communication to devices
    status = sns_ddf_open_port(
        &ptr->port_handle,
        &device_info->port_config);
    if(status != SNS_DDF_SUCCESS)
    {
        // Cleanup memory and exit
        sns_ddf_mfree((void*)ptr);
        ptr = 0;
        return status;
    }

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_init - sns_ddf_open_port returned '%s'", getStatusString(status));
    printf("\n\r sns_dd_panasonic_gyro_init - Port handle = %u", ptr->port_handle);
#endif

    *num_sensors = 1;
    *sensors = panasonic_gyro_sensors;
    *dd_handle_ptr = ptr;
    
    // Start gyro initialization
    // The following register configurations are based on the recommendations in Application Note (datasheet section 12):
        
    // Turn off sleep mode and set RSET bit to reset all registers to initial configuration (set register 0x3E to 0xC4)
#ifdef GEN2   
    write_buffer[0] = 0xC0;   // GEN2 doesn't have RSTPD bit
#else
    write_buffer[0] = 0xC4;
#endif
    status = sns_ddf_write_port(
        ptr->port_handle,
        0x3E,           // Register address to write to
        write_buffer,
        1,              // Number of bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    sns_ddf_delay(1000);

    // Clear RSET bit and leave awake (set register 0x3E to 0x44)
#ifdef GEN2   
    write_buffer[0] = 0x40;   // GEN2 doesn't have RSTPD bit
#else
    write_buffer[0] = 0x44;
#endif
    status = sns_ddf_write_port(
        ptr->port_handle,
        0x3E,           // Register address to write to
        write_buffer,
        1,              // Number of bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }    
    
    // Do blocking initialization
    sns_ddf_delay(GYRO_INIT_DELAY_STEP2);
    status = sns_dd_panasonic_gyro_init_step2(ptr);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }
    sns_ddf_delay(GYRO_INIT_DELAY_STEP8);
    status = sns_dd_panasonic_gyro_init_step3(ptr);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_init - Returning '%s'", getStatusString(status));
#endif
    
#if defined GYRO_DEBUG || defined GYRO_STREAM_FOR_TESTING_LEGIBLE || defined GYRO_FACTORY_TEST_TESTING
    {
        // Set periodic timer for testing
        sns_ddf_status_e timer_status;
        timer_status = sns_ddf_timer_init(&(ptr->polling_timer), ptr, &sns_dd_panasonic_gyro_if, &m_timer_id_polling, true);
        if(timer_status == SNS_DDF_SUCCESS)
        {
            timer_status = sns_ddf_timer_start(ptr->polling_timer, TEST_TIMER_DURATION);
        }
        else
        {
            timer_status = sns_ddf_timer_release(ptr->polling_timer);
        }
    }
#endif

#ifdef GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY
    // TLMM
    /* attach to GPIO to get gpio_handle */
    if( DAL_DeviceAttach(DALDEVICEID_TLMM, &ptr->gpio_handle )== DAL_ERROR)
    {
      //Log error;
    }
    /* configruing the pin as output, set to 1 */
    ptr->gpio_sleep_pin_cfg =
      DAL_GPIO_CFG_OUT( GYRO_GPIO_SLEEP, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, DAL_GPIO_LOW_VALUE);
    /* enable it */
    DalTlmm_ConfigGpio(ptr->gpio_handle, ptr->gpio_sleep_pin_cfg, DAL_TLMM_GPIO_ENABLE );
#endif

    return SNS_DDF_SUCCESS;
}


/*****************************************
    sns_dd_panasonic_gyro_get_data
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_get_data(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensors[],
    uint32_t                 num_sensors,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_data_s**  data)
{
    uint8_t i;
    uint8_t read_buffer[NUM_READ_BYTES] = {0};
    uint8_t read_count;
    uint8_t read_buffer_range[NUM_READ_BYTES] = {0};
    uint8_t read_count_range;
    int32_t x_sample,y_sample,z_sample = 0;

    q16_t x_sample_q16,y_sample_q16,z_sample_q16 = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s* data_ptr;
    sns_dd_panasonic_gyro_state_s* state = dd_handle;

#ifdef GYRO_DEBUG
    printf("\n\r");
    printf("\n\r sns_dd_panasonic_gyro_get_data - Enter get_data");
    printf("\n\r sns_dd_panasonic_gyro_get_data - My handle = %u", state);
    printf("\n\r sns_dd_panasonic_gyro_get_data - Port handle = %u", state->port_handle);
#endif

#ifdef GYRO_DEBUG
    printf("\r\nJust before\n\r");
#endif
    
    // Synchronous driver, read data now
    status = sns_ddf_read_port(
        state->port_handle,
        0xFB,           // register address. 0xFB for Burst Read (without temperature), 0xFF with temp
        read_buffer,
        NUM_READ_BYTES,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
    {
#ifdef GYRO_DEBUG
    printf("FB failed\n\r");
#endif
        return status;
    }
    
#ifdef GYRO_DEBUG
    printf("In between\n\r");
#endif
    
    // Read which range we're using so we can convert results accordingly
    status = sns_ddf_read_port(
        state->port_handle,
        0x16,           // register address 0x16 (FS_DLPF_CTRL)
        read_buffer_range,
        1,              // num bytes
        &read_count_range);
    if(status != SNS_DDF_SUCCESS)
    {
#ifdef GYRO_DEBUG
    printf("0x16 failed\n\r");
#endif
        return status;
    }
    
#ifdef GYRO_DEBUG
    printf("In end\n\r");
#endif

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_get_data - sns_ddf_read_port returned '%s'", getStatusString(status));
    printf("\n\r sns_dd_panasonic_gyro_get_data - Read port");
    printf("\n\r sns_dd_panasonic_gyro_get_data - Num Bytes Read = %u", read_count);
    printf("\n\r sns_dd_panasonic_gyro_get_data -");
    for(i = 0; i < read_count; i++)
    {
        printf(" Data[%u] = %u", i, read_buffer[i]);
    }
#endif

    timestamp = sns_ddf_get_timestamp();

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_get_data - Got timestamp");
#endif

    data_ptr = sns_ddf_memhandler_malloc(
        memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(data_ptr == 0)
    {
        return SNS_DDF_ENOMEM;
    }

    *data = data_ptr;

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_get_data - Allocated memory for sensors");
#endif
    
    for(i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor = sensors[i];
        data_ptr[i].timestamp = timestamp;

#ifdef GYRO_DEBUG
        printf("\n\r sns_dd_panasonic_gyro_get_data - Setup sensor %u", i);
#endif
        
        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_GYRO:
            data_ptr[i].status = SNS_DDF_SUCCESS;
            data_ptr[i].samples = sns_ddf_memhandler_malloc(
                memhandler, GYRO_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
            if(data_ptr[i].samples == 0)
            {
                data_ptr[i].status = SNS_DDF_ENOMEM;
                data_ptr[i].num_samples = 0;
                return SNS_DDF_ENOMEM;
            }

            x_sample = (((int32_t)(read_buffer[0])) << 8) + read_buffer[1];
            y_sample = (((int32_t)(read_buffer[2])) << 8) + read_buffer[3];
            z_sample = (((int32_t)(read_buffer[4])) << 8) + read_buffer[5];
            
#ifdef GYRO_STREAM_FOR_TESTING_LEGIBLE
            {
                printf("\n %d", (int16_t)x_sample );
                printf("  %d", (int16_t)y_sample);
                printf("  %d", (int16_t)z_sample);
                switch((read_buffer_range[0] & 0x18))
                {
                    // Range is +/-2000 dps, sensitivity is 12.5 LSB per dps
                    case 0x18:
                        printf("  12.5"); 
                        break;
                    // range is +/-1000 dps, sensitivity is 25 LSB per dps
                    case 0x10:
                        printf("  25"); 
                        break;
                    // range is +/-500 dps, sensitivity is 50 LSB per dps
                    case 0x08:
                        printf("  50"); 
                        break;
#ifdef GEN2
                    // range is +/-250 dps, sensitivity is 100 LSB per dps
                    case 0x00:
                        printf(" 100"); 
                        break;
#endif
                    default:
                        break;
                }
                static unsigned int count = 0;
                unsigned long hs = TEST_TIMER_DURATION/10000 * count++;
                printf(" %u",hs);             
            }
#endif
                
            x_sample_q16 = FX_FLTTOFIX_Q16(x_sample); 
            y_sample_q16 = FX_FLTTOFIX_Q16(y_sample); 
            z_sample_q16 = FX_FLTTOFIX_Q16(z_sample); 
                        
#ifndef GEN2 /* GEN3 */
            y_sample_q16 = -y_sample_q16;
            z_sample_q16 = -z_sample_q16;
#else /* GEN2 */
            x_sample_q16 = -x_sample_q16;
#endif
 
            // switch on the current range setting and convert reading accordingly, with axes to match rotation convention
            switch((read_buffer_range[0] & 0x18))
            {
                // Range is +/-2000 dps, sensitivity is 12.5 LSB per dps
                case 0x18:
                    data_ptr[i].samples[0].sample = (q16_t) (y_sample_q16/12.5*PI/180) ;   // X-axis data
                    data_ptr[i].samples[1].sample = (q16_t) (x_sample_q16/12.5*PI/180) ;    // Y-axis datas
                    data_ptr[i].samples[2].sample = (q16_t) (z_sample_q16/12.5*PI/180) ;   // Z-axis data
                    break;
                // range is +/-1000 dps, sensitivity is 25 LSB per dps
                case 0x10:
                    data_ptr[i].samples[0].sample = (q16_t) (y_sample_q16/25*PI/180) ;     // X-axis data
                    data_ptr[i].samples[1].sample = (q16_t) (x_sample_q16/25*PI/180) ;      // Y-axis datas
                    data_ptr[i].samples[2].sample = (q16_t) (z_sample_q16/25*PI/180) ;     // Z-axis data
                    break;
                // range is +/-500 dps, sensitivity is 50 LSB per dps
                case 0x08:
                    data_ptr[i].samples[0].sample = (q16_t) (y_sample_q16/50*PI/180) ;     // X-axis data
                    data_ptr[i].samples[1].sample = (q16_t) (x_sample_q16/50*PI/180) ;      // Y-axis datas
                    data_ptr[i].samples[2].sample = (q16_t) (z_sample_q16/50*PI/180) ;     // Z-axis data
                    break;
#ifdef GEN2     // range is +/-250 dps, sensitivity is 100 LSB per dps
                case 0x00:
                    data_ptr[i].samples[0].sample = (q16_t) (y_sample_q16/100*PI/180) ;     // X-axis data
                    data_ptr[i].samples[1].sample = (q16_t) (x_sample_q16/100*PI/180) ;      // Y-axis datas
                    data_ptr[i].samples[2].sample = (q16_t) (z_sample_q16/100*PI/180) ;     // Z-axis data
                    break;
#endif
                default:
                    break;
            }   
            

            
            data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
            data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
            data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;
            data_ptr[i].num_samples = GYRO_NUM_AXES;
            break;

        default:
            data_ptr[i].samples = 0;
            data_ptr[i].status = SNS_DDF_EINVALID_PARAM;
            data_ptr[i].num_samples = 0;
            break;
        }
    }

    return SNS_DDF_SUCCESS;
}


/*****************************************
    sns_dd_panasonic_gyro_set_attrib
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value)
{
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* powerstate = value;
            return sns_dd_panasonic_gyro_set_powerstate(dd_handle, *powerstate);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint8_t* range_index = value;
            if(*range_index >= GYRO_NUM_RANGE)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            return sns_dd_panasonic_gyro_set_range(dd_handle, *range_index);
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            uint8_t* resolution_adc_index = value;
            if(*resolution_adc_index >= GYRO_NUM_RESOLUTION_ADC)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            return sns_dd_panasonic_gyro_set_resolution_adc(dd_handle, *resolution_adc_index);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint8_t* lowpass_index = value;
            if(*lowpass_index >= GYRO_NUM_LPF)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            return sns_dd_panasonic_gyro_set_lowpass(dd_handle, *lowpass_index);
        }

        case SNS_DDF_ATTRIB_MOTION_DETECT:
        {
            bool* motion_detect = value;
            return sns_dd_panasonic_gyro_set_motion_detect(dd_handle, *motion_detect);
        }

        case SNS_DDF_ATTRIB_THRESHOLD:
        {
            return SNS_DDF_EINVALID_ATTR;
        }

        case SNS_DDF_ATTRIB_ACCURACY:
        {
            return SNS_DDF_EINVALID_ATTR;
        }

        default:
        {
            return SNS_DDF_EINVALID_ATTR;
        }
    }
}


/*****************************************
    sns_dd_panasonic_gyro_get_attrib
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_get_attrib(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems)
{
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_power_info_s));
            if(power == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            power->active_current = GYRO_ACTIVE_CURRENT;  
            power->lowpower_current = GYRO_LOW_POWER_CURRENT;  

            *value = power;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s* delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if(delays == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            delays->time_to_active = GYRO_TIME_TO_ACTIVE;
            delays->time_to_data = GYRO_TIME_TO_DATA;
            
            *value = delays;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
                memhandler, GYRO_NUM_RANGE * sizeof(sns_ddf_range_s));
            if(ranges == NULL)
            {
            return SNS_DDF_ENOMEM;
            }

            // Ranges in radians per second, in ascending order
            ranges[0].min = FX_FLTTOFIX_Q16(GYRO_RANGE_0_MIN); 
            ranges[0].max = FX_FLTTOFIX_Q16(GYRO_RANGE_0_MAX);
            ranges[1].min = FX_FLTTOFIX_Q16(GYRO_RANGE_1_MIN);
            ranges[1].max = FX_FLTTOFIX_Q16(GYRO_RANGE_1_MAX);
            ranges[2].min = FX_FLTTOFIX_Q16(GYRO_RANGE_2_MIN);
            ranges[2].max = FX_FLTTOFIX_Q16(GYRO_RANGE_2_MAX);
#ifdef GEN2
            ranges[3].min = FX_FLTTOFIX_Q16(GYRO_RANGE_3_MIN);
            ranges[3].max = FX_FLTTOFIX_Q16(GYRO_RANGE_3_MAX);
#endif

            *value = ranges;
            *num_elems = GYRO_NUM_RANGE;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s* resolution_adcs = sns_ddf_memhandler_malloc(
                memhandler, GYRO_NUM_RESOLUTION_ADC * sizeof(sns_ddf_resolution_adc_s));
            if(resolution_adcs == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            resolution_adcs[0].bit_len = GYRO_ADC_BIT_LENGTH;
            resolution_adcs[0].max_freq = GYRO_ADC_FREQUENCY;

            *value = resolution_adcs;
            *num_elems = GYRO_NUM_RESOLUTION_ADC;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resolution = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_resolution_t));
            if(resolution == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            *resolution = FX_FLTTOFIX_Q16(GYRO_ADC_RESOLUTION_MAX);

            *value = resolution;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            sns_ddf_lowpass_freq_t* lpfs = sns_ddf_memhandler_malloc(
                memhandler, GYRO_NUM_LPF * sizeof(sns_ddf_lowpass_freq_t));
            if(lpfs == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            lpfs[0] = GYRO_LPF_0;  
            lpfs[1] = GYRO_LPF_1;
            lpfs[2] = GYRO_LPF_2;
            lpfs[3] = GYRO_LPF_3;
            lpfs[4] = GYRO_LPF_4;
            lpfs[5] = GYRO_LPF_5;
            lpfs[6] = GYRO_LPF_6;

            *value = lpfs;
            *num_elems = GYRO_NUM_LPF;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s* info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if(info == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            info->name = "Panasonic GYRO Driver";
            info->version = CURRENT_VERSION;

            *value = info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s* info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_device_info_s));
            if(info == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            info->name = "Gyro";
            info->vendor = "Panasonic";
            info->model = GYRO_MODEL;
            info->version = 1;

            *value = info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_THRESHOLD:
        {
            return SNS_DDF_EINVALID_ATTR;
        }

        case SNS_DDF_ATTRIB_ACCURACY:
        {
            return SNS_DDF_EINVALID_ATTR;
        }

        case SNS_DDF_ATTRIB_BIAS:
        {
            sns_ddf_bias_t* bias = sns_ddf_memhandler_malloc(
                memhandler, GYRO_NUM_AXES*sizeof(sns_ddf_bias_t));
            if(bias == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            bias[0] = FX_FLTTOFIX_Q16(GYRO_BIAS_XAXIS);
            bias[1] = FX_FLTTOFIX_Q16(GYRO_BIAS_YAXIS);
            bias[2] = FX_FLTTOFIX_Q16(GYRO_BIAS_ZAXIS);
            
#ifdef BIAS
            bias[0] = gyro_bias_xaxis_var;
            bias[1] = gyro_bias_yaxis_var;
            bias[2] = gyro_bias_zaxis_var;
#endif
            

            *value = bias;
            *num_elems = GYRO_NUM_AXES;
            return SNS_DDF_SUCCESS;
        }

        default:
        {
            return SNS_DDF_EINVALID_ATTR;
        }
    }
}


/*****************************************
    sns_dd_panasonic_gyro_handle_timer
*****************************************/
void sns_dd_panasonic_gyro_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    uint32_t timer_id = *((uint32_t*)arg);
    sns_ddf_status_e status;

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_handle_timer - Timer ID = %u", timer_id );
#endif

    switch(timer_id)
    {
        // Timer for streaming data out, just for testing purposes
        case TIMER_ID_POLLING:
        {
            sns_ddf_memhandler_s memhandler;
            sns_ddf_memhandler_init(&memhandler);     
            
            
#ifdef GYRO_FACTORY_TEST_TESTING 
            uint32_t          error;
            uint32_t*         err = &error;
            sns_ddf_status_e result;
            
            // only run once
            static uint8_t doneYet = 0;
            if (!doneYet) {  
                doneYet = 1;
                
                // run tests and print results
                
                result = sns_dd_panasonic_gyro_run_test(
                    dd_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_TEST_CONNECTIVITY, err);
                if (result == SNS_DDF_SUCCESS) printf("\r\nConnectivity test successful with debug code %i", *err);
                else printf("\r\nConnectivity test failed with code: %i and debug code: %i", result, *err);
                
                result = sns_dd_panasonic_gyro_run_test( 
                    dd_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_TEST_SELF_HW, err);
                if (result == SNS_DDF_SUCCESS) printf("\r\nHW self test successful with debug code %i", *err);
                else printf("\r\nHW self test failed with code: %i and debug code: %i", result, *err);
                
                result = sns_dd_panasonic_gyro_run_test(
                    dd_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_TEST_SELF_SW, err);
                if (result == SNS_DDF_SUCCESS) printf("\r\nSW self test successful with debug code %i", *err);
                else printf("\r\nSW self test failed with code: %i and debug code: %i", result, *err);   
                
            }
#endif

            
#ifdef GYRO_STREAM_FOR_TESTING_LEGIBLE
            sns_ddf_sensor_e sensors[1] = {SNS_DDF_SENSOR_GYRO};
            sns_ddf_sensor_data_s* sensor_data;
            sns_dd_panasonic_gyro_get_data(dd_handle, sensors, 1, &memhandler, &sensor_data);
            
           // printf(" samples=%u, x=%i, y=%i, z=%i",sensor_data->num_samples, sensor_data->samples[0].sample, sensor_data->samples[1].sample, sensor_data->samples[2].sample);
#endif

#ifdef GYRO_DEBUG
            void* data;
            uint8_t i;
            uint32_t num;
            sns_ddf_power_info_s* data_as_power;
            sns_ddf_delays_s* data_as_delays;
            sns_ddf_range_s* data_as_range;
            sns_ddf_resolution_adc_s* data_as_resolution_adc;
            sns_ddf_resolution_t* data_as_resolution;
            sns_ddf_lowpass_freq_t* data_as_lowpass_freq;
            sns_ddf_driver_info_s* data_as_driver_info;
            sns_ddf_device_info_s* data_as_device_info;
            sns_ddf_bias_t* data_as_bias;

            sns_ddf_sensor_e sensors[1] = {SNS_DDF_SENSOR_GYRO};
            sns_ddf_sensor_data_s* sensor_data;

            sns_dd_panasonic_gyro_get_data(dd_handle, sensors, 1, &memhandler, &sensor_data);
            printf("\n\r sns_dd_panasonic_gyro_handle_timer - Getting data - %s", getStatusString(status));
            printf("\n\r sns_dd_panasonic_gyro_handle_timer - Sensor = %u :: Status = %s :: Timestamp = %u", sensor_data->sensor, getStatusString(sensor_data->status), sensor_data->timestamp);
            for(i = 0; i < sensor_data->num_samples; i++)
            {
                printf("\n\r sns_dd_panasonic_gyro_handle_timer - Data Sample %u :: Value = %u :: Status = %s", i, sensor_data->samples[i].sample, getStatusString(sensor_data->samples[i].status));
            }

            // TEST GETTING ATTRIBUTES HERE
            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_POWER_INFO,
                &memhandler,
                &data,
                &num);
            data_as_power = data;
            printf("\n\r Power Info: active = %u uA :: low power = %u uA",data_as_power->active_current,data_as_power->lowpower_current);

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_DELAYS,
                &memhandler,
                &data,
                &num);
            data_as_delays = data;
            printf("\n\r Delays: time to active = %u us :: time to data = %u us",data_as_delays->time_to_active,data_as_delays->time_to_data);

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_RANGE,
                &memhandler,
                &data,
                &num);
            data_as_range = data;
            for(i = 0; i < num; i++)
            {
                printf("\n\r Range: %u) %f to %f rad/s",i,FX_FIXTOFLT_Q16(data_as_range[i].min),FX_FIXTOFLT_Q16(data_as_range[i].max));
            }

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_RESOLUTION_ADC,
                &memhandler,
                &data,
                &num);
            data_as_resolution_adc = data;
            for(i = 0; i < num; i++)
            {
                printf("\n\r Resolution ADC: %u) bit length = %u :: max freq = %u Hz",i,data_as_resolution_adc[i].bit_len,data_as_resolution_adc[i].max_freq);
            }

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_RESOLUTION,
                &memhandler,
                &data,
                &num);
            data_as_resolution = data;
            printf("\n\r Resolution: %f rad/s",FX_FIXTOFLT_Q16(*data_as_resolution));

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_LOWPASS,
                &memhandler,
                &data,
                &num);
            data_as_lowpass_freq = data;
            for(i = 0; i < num; i++)
            {
                printf("\n\r Low Pass Freq: %u) %f Hz",i,FX_FIXTOFLT_Q16(data_as_lowpass_freq[i]));
            }

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_DRIVER_INFO,
                &memhandler,
                &data,
                &num);
            data_as_driver_info = data;
            printf("\n\r Driver Info: name = %s :: version = %i",data_as_driver_info->name,data_as_driver_info->version);

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_DEVICE_INFO,
                &memhandler,
                &data,
                &num);
            data_as_device_info = data;
            printf("\n\r Device Info: name = %s :: vendor = %s :: model = %s :: version = %i",data_as_device_info->name
                                                                                             ,data_as_device_info->vendor
                                                                                             ,data_as_device_info->model
                                                                                             ,data_as_device_info->version);

            sns_dd_panasonic_gyro_get_attrib(
                dd_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_ATTRIB_BIAS,
                &memhandler,
                &data,
                &num);
            data_as_bias = data;
            printf("\n\r Bias: x = %f rad/s :: y = %f rad/s :: z = %f rad/s",FX_FIXTOFLT_Q16(data_as_bias[0]),FX_FIXTOFLT_Q16(data_as_bias[1]),FX_FIXTOFLT_Q16(data_as_bias[2]));

#endif

            // Free memory amd exit from debug polling
            sns_ddf_memhandler_free(&memhandler);

            return;
        }
        
        // Timer to do step 2 of the initialization after a delay
        case TIMER_ID_INIT_STEP2:
        {
#ifdef GYRO_DEBUG
            printf("\n\r sns_dd_panasonic_gyro_handle_timer - Step 2 of initialization");
#endif
            // Release timer
            status = sns_ddf_timer_release(state->init_timer);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EDEVICE);
                return;
            }

            sns_dd_panasonic_gyro_init_step2(dd_handle);

            // Create new timer for next step
            status = sns_ddf_timer_init(&(state->init_timer), dd_handle, &sns_dd_panasonic_gyro_if, &m_timer_id_init_step3, false);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EDEVICE);
                return;
            }
            // Start timer for next step
            status = sns_ddf_timer_start(state->init_timer, (GYRO_INIT_DELAY_STEP8));
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_timer_release(state->init_timer);
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EDEVICE);
                return;
            }
            
            return;
        }
        
        // Initialization timer to do final step 3 of the initialization process after a delay
        case TIMER_ID_INIT_STEP3:
        {
#ifdef GYRO_DEBUG            
            printf("\n\r sns_dd_panasonic_gyro_handle_timer - Step 3 of initialization/");       
#endif            

            // Release timer
            status = sns_ddf_timer_release(state->init_timer);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EDEVICE);
                return;
            }

            sns_dd_panasonic_gyro_init_step3(dd_handle);

            // notify SMGR that initialization process is complete.
            sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_INIT);

            return;
        }
        default:
        {
            sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EDEVICE);
            return;
        }
    }
}


/*****************************************
    sns_dd_panasonic_gyro_handle_irq
*****************************************/
#ifdef NEW_API
void sns_dd_panasonic_gyro_handle_irq(sns_ddf_handle_t dd_handle, uint32_t gpio_num, sns_ddf_time_t timestamp)
{
}
#else
void sns_dd_panasonic_gyro_handle_irq(sns_ddf_handle_t dd_handle, sns_ddf_signal_irq_e irq)
{
}
#endif


/*****************************************
    sns_dd_panasonic_gyro_reset
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_reset(sns_ddf_handle_t dd_handle)
{
    sns_ddf_status_e status;
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    uint8_t write_buffer[1] = {0};
    uint8_t write_count;

#ifdef GYRO_DEBUG
    printf("\n\r sns_dd_panasonic_gyro_reset - Entering (step 1 of reset");
#endif
    
    // The following register configurations are based on the recommendations in Application Note (datasheet section 12):
        
    // Turn off sleep mode and set RSET bit to reset all registers to initial configuration (set register 0x3E to 0xC4)
#ifdef GEN2   
    write_buffer[0] = 0xC0;   // GEN2 doesn't have RSTPD bit
#else
    write_buffer[0] = 0xC4;
#endif
    status = sns_ddf_write_port(
        state->port_handle,
        0x3E,           // Register address to write to
        write_buffer,
        1,              // Number of bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    sns_ddf_delay(1000);

    // Clear RSET bit and leave awake (set register 0x3E to 0x44)
#ifdef GEN2   
    write_buffer[0] = 0x40;   // GEN2 doesn't have RSTPD bit
#else
    write_buffer[0] = 0x44;
#endif
    status = sns_ddf_write_port(
        state->port_handle,
        0x3E,           // Register address to write to
        write_buffer,
        1,              // Number of bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    // Do blocking reset
    sns_ddf_delay(GYRO_INIT_DELAY_STEP2);
    status = sns_dd_panasonic_gyro_init_step2(dd_handle);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }
    sns_ddf_delay(GYRO_INIT_DELAY_STEP8);
    status = sns_dd_panasonic_gyro_init_step3(dd_handle);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }
    return SNS_DDF_SUCCESS; 
}


/*****************************************
    sns_dd_panasonic_gyro_run_test
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err)
{
   
#ifndef FACTORY_SELF_TEST
 
    switch(test)
    {
    case SNS_DDF_TEST_SELF:
    case SNS_DDF_TEST_IRQ:
    case SNS_DDF_TEST_CONNECTIVITY:
    default:
        return SNS_DDF_SUCCESS;
    }
  
#else
  
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    uint8_t read_buffer[NUM_READ_BYTES] = {0};
    uint8_t read_count;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = {0};
    uint8_t write_count;
  
    if (0) return SNS_DDF_EDEVICE_BUSY;
  
    switch(test)
    {
    case SNS_DDF_TEST_IRQ: 
        return SNS_DDF_EINVALID_TEST;
        
    case SNS_DDF_TEST_SELF:
    case SNS_DDF_TEST_SELF_HW:
        *err = 0;
      
        // cancel sleep mode
#ifdef GEN2   
        write_buffer[0] = 0x40;   // GEN2 doesn't have RSTPD bit
#else
        write_buffer[0] = 0x44;
#endif
        status = sns_ddf_write_port(state->port_handle, 0x3E, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
        
        sns_ddf_delay(150 * 1000);  // wait 150 msec for stabilization of sensing element
        
        write_buffer[0] = 0x09;  // 100Hz data rate
        status = sns_ddf_write_port(state->port_handle, 0x15, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
        
        write_buffer[0] = 0x9A;  // Dynamic range : +/-2000dps, Cutoff frequency : 100Hz
        status = sns_ddf_write_port(state->port_handle, 0x16, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;        
  
        write_buffer[0] = 0x01;  // SOC disable setting
        status = sns_ddf_write_port(state->port_handle, 0x29, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;          
      
        write_buffer[0] = 0x00;  // start updating DIAG_MON register
        status = sns_ddf_write_port(state->port_handle, 0x2B, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
        
        
        // Detect interrupt of DIAG (Software)
        status = sns_ddf_read_port(state->port_handle, 0x1A, read_buffer, 1, &read_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;    
        //  if (read_buffer[0] != 0x00) return SNS_DDF_EDEVICE;  // only read this register, but ignore value
        
        sns_ddf_delay(2 * 1000);  // wait 2 msec
        
        // Check IC state by reading diagnostic register DIAG_MONI  (it is updated 100ms after power on)
#ifdef GEN2
        status = sns_ddf_read_port(state->port_handle, 0x2F, read_buffer, 1, &read_count);
#else
        status = sns_ddf_read_port(state->port_handle, 0x39, read_buffer, 1, &read_count);
#endif
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;    
        if (read_buffer[0] != 0x00) {
            if (read_buffer[0] & 32) {
                // checksum verification failed
                *err = 1;
                return SNS_DDF_EDEVICE;
            }
            if (read_buffer[0] & 2) {
                // drive frequency check failed
                *err = 2;
                return SNS_DDF_EDEVICE;
            }
            if (read_buffer[0] & 1) {
                // drive amplitude check failed
                *err = 3;
                return SNS_DDF_EDEVICE;
            } 
            return SNS_DDF_EDEVICE; 
        }
        
        sns_ddf_delay(2 * 1000);  // wait 2 msec
        
        // Check 'already read'
#ifdef GEN2
        status = sns_ddf_read_port(state->port_handle, 0x2F, read_buffer, 1, &read_count);
#else
        status = sns_ddf_read_port(state->port_handle, 0x39, read_buffer, 1, &read_count);
#endif
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;    
        if (read_buffer[0] != 0x40) {
            *err = 4;
            return SNS_DDF_EDEVICE;    
        }
        
        /*
        write_buffer[0] = 0x1;  // stop updating DIAG_MON register
        status = sns_ddf_write_port(state->port_handle, 0x2B, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
        */
    
        return SNS_DDF_SUCCESS;
        
      
    case SNS_DDF_TEST_CONNECTIVITY: 
        {
            uint8_t original;
            *err=0;
            
            // read original value.  0x16 (FS_DLPF_CTRL)
            status = sns_ddf_read_port(state->port_handle, 0x16, read_buffer, 1, &read_count);
            if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
            original = read_buffer[0];
          
            // write a changed value
            status = sns_dd_panasonic_gyro_set_lowpass(dd_handle, 0);
            if(status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;
            
            // read changed value 0x16 (FS_DLPF_CTRL)
            status = sns_ddf_read_port(state->port_handle, 0x16, read_buffer, 1, &read_count);
            if(status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
            
            // verify that value was set correctly
            if ( (read_buffer[0] & 4) && (read_buffer[0] & 2) && (~read_buffer[0] & 1) )     //  (bits 2 and 1 set, bit 0 cleared)
            {
                // value was set correctly
            } else { *err=1; return SNS_DDF_EBUS; }
           
            // write back original value 
            write_buffer[0] = original;
            status = sns_ddf_write_port(state->port_handle, 0x16, write_buffer, 1, &write_count);
            if(status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;
   
            return SNS_DDF_SUCCESS; 
        }
      
    case SNS_DDF_TEST_SELF_SW:
        {
            int16_t x_sample = 0, y_sample = 0, z_sample = 0;
            int16_t x_sample_last = 0, y_sample_last = 0, z_sample_last = 0;
            int i;

// #define INCREMENTAL_AVG          
#ifdef INCREMENTAL_AVG             
            double x_avg = 0.0, y_avg = 0.0, z_avg = 0.0;
            int t=1;
#else
            int x_avg = 0, y_avg = 0, z_avg = 0;
#endif
      
            // cancel sleep mode
#ifdef GEN2   
        write_buffer[0] = 0x40;   // GEN2 doesn't have RSTPD bit
#else
        write_buffer[0] = 0x44;
#endif
            status = sns_ddf_write_port(state->port_handle, 0x3E, write_buffer, 1, &write_count);
            if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
            
            sns_ddf_delay(150 * 1000);  // wait 150 msec for stabilization of sensing element
            
            write_buffer[0] = 0x09;  // 100Hz data rate
            status = sns_ddf_write_port(state->port_handle, 0x15, write_buffer, 1, &write_count);
            if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
            
            write_buffer[0] = 0x9A;  // Dynamic range : +/-2000dps, Cutoff frequency : 100Hz
            status = sns_ddf_write_port(state->port_handle, 0x16, write_buffer, 1, &write_count);
            if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;        
      
            write_buffer[0] = 0x01;  // SOC disable setting
            status = sns_ddf_write_port(state->port_handle, 0x29, write_buffer, 1, &write_count);
            if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;          

            for (i=0; i<5*50; i++) {
                *err = 0;
                sns_ddf_delay(20 * 1000);  // wait a 1/50s because of 50Hz sample rate

                // register address. 0xFB for Burst Read (without temperature), 0xFF with temp
                status = sns_ddf_read_port(state->port_handle, 0xFB, read_buffer, NUM_READ_BYTES, &read_count);
                if (status != SNS_DDF_SUCCESS) return status;
                
                x_sample_last = x_sample;
                y_sample_last = y_sample;
                z_sample_last = z_sample;
                
                x_sample = (((int16_t)(read_buffer[0])) << 8) + read_buffer[1];
                y_sample = (((int16_t)(read_buffer[2])) << 8) + read_buffer[3];
                z_sample = (((int16_t)(read_buffer[4])) << 8) + read_buffer[5];
                
                
                // test for offset error
                // printf("\r\n x_sample=%i y_sample=%i z_sample=%i", x_sample, y_sample, z_sample);
                if (x_sample > OFFSET_THRESHOLD) { *err=1; return SNS_DDF_EFAIL; }
                if (y_sample > OFFSET_THRESHOLD) { *err=2; return SNS_DDF_EFAIL; }
                if (y_sample > OFFSET_THRESHOLD) { *err=3; return SNS_DDF_EFAIL; }
         
#ifdef INCREMENTAL_AVG 
                x_avg += (x_sample - x_avg) / t;
                y_avg += (y_sample - y_avg) / t;
                z_avg += (z_sample - z_avg) / t;
                ++t;
#else           
                x_avg += x_sample;
                y_avg += y_sample;
                z_avg += z_sample;
#endif
                
                // test for noise error
                if (i > 0) {
                    // printf("\r\n x_sample=%i y_sample=%i z_sample=%i", abs(x_sample - x_sample_last), abs(y_sample - y_sample_last), abs(z_sample - z_sample_last));
                    if (abs(x_sample - x_sample_last) > NOISE_THRESHOLD) { *err=4; return SNS_DDF_EFAIL; }
                    if (abs(y_sample - y_sample_last) > NOISE_THRESHOLD) { *err=5; return SNS_DDF_EFAIL; }
                    if (abs(z_sample - z_sample_last) > NOISE_THRESHOLD) { *err=6; return SNS_DDF_EFAIL; }
                }
               
            } // for
          
            
#ifndef INCREMENTAL_AVG        
            x_avg/=250;  // 5*50 samples  // to get accuracy after the comma, we could convert to Qualcomm type before dividing
            y_avg/=250;  // 5*50 samples
            z_avg/=250;  // 5*50 samples
#endif
            
#ifdef GYRO_FACTORY_TEST_TESTING
            printf("\r\n x_avg=%d y_avg=%d z_avg=%d", (int)x_avg, (int)y_avg, (int)z_avg);
#endif         
       
            
#ifdef BIAS
            // calculate bias in degrees and Qualcomm coordinate system
            
            // convert to Qualcomm type
            q16_t x_avg_q16 = FX_FLTTOFIX_Q16(x_avg);     
            q16_t y_avg_q16 = FX_FLTTOFIX_Q16(y_avg);     
            q16_t z_avg_q16 = FX_FLTTOFIX_Q16(z_avg);
            // printf("\r\n x_avg=%d y_avg=%d z_avg=%d", x_avg_q16, y_avg_q16, z_avg_q16);
            // printf("\r\n x_avg=%d y_avg=%d z_avg=%d", (int)FX_FIXTOFLT_Q16(x_avg_q16), (int)FX_FIXTOFLT_Q16(y_avg_q16), (int)FX_FIXTOFLT_Q16(z_avg_q16));
                          
            // Read which range we're using so we can convert results accordingly
            status = sns_ddf_read_port(state->port_handle, 0x16, read_buffer, 1, &read_count);
            if(status != SNS_DDF_SUCCESS) return status;
            
            // switch on the current range setting and convert reading accordingly, with axes to match rotation convention
            switch((read_buffer[0] & 0x18))
            {
                // Range is +/-2000 dps, sensitivity is 12.5 LSB per dps
                case 0x18:
                    gyro_bias_xaxis_var = (q16_t) (-y_avg_q16/12.5*PI/180) ;   // X-axis data
                    gyro_bias_yaxis_var = (q16_t) (x_avg_q16/12.5*PI/180) ;    // Y-axis datas
                    gyro_bias_zaxis_var = (q16_t) (-z_avg_q16/12.5*PI/180) ;   // Z-axis data
                    break;
                // range is +/-1000 dps, sensitivity is 25 LSB per dps
                case 0x10:
                    gyro_bias_xaxis_var = (q16_t) (-y_avg_q16/25*PI/180) ;     // X-axis data
                    gyro_bias_yaxis_var = (q16_t) (x_avg_q16/25*PI/180) ;      // Y-axis datas
                    gyro_bias_zaxis_var = (q16_t) (-z_avg_q16/25*PI/180) ;     // Z-axis data
                    break;
                // range is +/-500 dps, sensitivity is 50 LSB per dps
                case 0x08:
                    gyro_bias_xaxis_var = (q16_t) (-y_avg_q16/50*PI/180) ;     // X-axis data
                    gyro_bias_yaxis_var = (q16_t) (x_avg_q16/50*PI/180) ;      // Y-axis datas
                    gyro_bias_zaxis_var = (q16_t) (-z_avg_q16/50*PI/180) ;     // Z-axis data
                    break;
                default:
                    break;       
            }               
#endif // BIAS
          
            return SNS_DDF_SUCCESS;
        }
        
    case SNS_DDF_TEST_OEM:
        /* OEM can put code for testing gyro here */
        return SNS_DDF_SUCCESS;
      
    default:
        return SNS_DDF_EINVALID_TEST;
    }
#endif  // FACTORY_SELF_TEST
}


//******************
// PRIVATE FUNCTIONS
//******************


/*****************************************
    sns_dd_panasonic_gyro_init_step2
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_init_step2(sns_ddf_handle_t dd_handle)
{
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = {0};
    uint8_t write_count;
    
    // Reg 0x29, set to 0x00 (SOC Enable)
    write_buffer[0] = 0x00;
    status = sns_ddf_write_port(
        state->port_handle,
        0x29,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }
 
#ifndef GEN2
    // Reg 0x4D, set to 0x4E (buffer update on; start update without time lag; window1 threshold+/-10dps)
    write_buffer[0] = 0x4E;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4D,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }
        
    // Reg 0x4F, set to 0x80 (FHP=1, fc:0.02Hz)
    write_buffer[0] = 0x80;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4F,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }

    // Reg 0x18, set to 0x20 (SOC mode select = Normal mode)
    write_buffer[0] = 0x20;
    status = sns_ddf_write_port(
        state->port_handle,
        0x18,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }

    
    // Reg 0x4C, set to 0x7D (Window1DataMask = 1(Disable), WIN2 = 1(Disable), WIN1 = 1(Disable), TBST=11 (SOC boost time 100ms), BST=1(ON)
    write_buffer[0] = 0x7D;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4C,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }
#endif
    
    return SNS_DDF_SUCCESS;
}



/*****************************************
    sns_dd_panasonic_gyro_init_step3
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_init_step3(sns_ddf_handle_t dd_handle)
{
    sns_ddf_status_e status;
  
#ifndef GEN2
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    uint8_t write_buffer[1] = {0};
    uint8_t write_count;
  
   // Reg 0x4C, set to 0x2C (Window1DataMask = 0(Enable), WIN2 = 1(Disable), WIN1 = 0(Enable), TBST=11 (SOC boost time 100ms), BST=0(OFF)
    write_buffer[0] = 0x2C;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4C,           // Register address
        write_buffer,
        1,              // Num bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
        return status;
    }
#endif
    
    // Leave gyro in low power mode
    status = sns_dd_panasonic_gyro_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_LOWPOWER); 
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    return SNS_DDF_SUCCESS;
}


/*****************************************
    sns_dd_panasonic_gyro_set_powerstate
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_powerstate(sns_ddf_handle_t dd_handle, sns_ddf_powerstate_e powerstate)
{
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1];
    uint8_t write_count;

    
#ifdef GEN2    
#ifdef DDK    
#define PIN_SLEEP     {1 << 28, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}    // SNS_TST1 (PA28)
    const Pin pinSLEEP = PIN_SLEEP;
    PIO_Configure(&pinSLEEP, 1);
    //PIO_Set(&pinSLEEP);    // puts gyro into sleep mode
    //PIO_Clear(&pinSLEEP);    // takes gyro out of sleep mode
#endif // DDK
#endif // GEN2
    
    switch(powerstate)
    {
        case SNS_DDF_POWERSTATE_ACTIVE:
        {
            // Set sensor into active power mode.  Per recommendation from Panasonic, use steps 1, 2, 7, 8, 9 from
            // SOC configuration Application Note.

#ifdef GEN2
#ifndef DDK
#ifdef GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY
            DalTlmm_GpioOut( state->gpio_handle, state->gpio_sleep_pin_cfg, DAL_GPIO_LOW_VALUE );
            sns_ddf_delay(1000);     // only 10uS according to datasheet, this is 1ms
#endif
#else
            PIO_Clear(&pinSLEEP);    // takes gyro out of hardware sleep mode
            sns_ddf_delay(1000);     // only 10uS according to datasheet, this is 1ms
#endif /* DDK */
#endif
          
            // Clear RSET bit and leave awake (set register 0x3E to 0x44)
#ifdef GEN2   
        write_buffer[0] = 0x40;   // GEN2 doesn't have RSTPD bit
#else
        write_buffer[0] = 0x44;
#endif
            status = sns_ddf_write_port(
                state->port_handle,
                0x3E,           // Register address to write to
                write_buffer,
                1,              // Number of bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
                return status;
            }

            sns_ddf_delay(GYRO_INIT_DELAY_STEP2);

#ifndef GEN2            
            // Reg 0x4C, set to 0x7D (Window1DataMask = 1(Disable), WIN2 = 1(Disable), WIN1 = 1(Disable), TBST=11 (SOC boost time 100ms), BST=1(ON)
            write_buffer[0] = 0x7D;
            status = sns_ddf_write_port(
                state->port_handle,
                0x4C,           // Register address
                write_buffer,
                1,
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
                return status;
            }
#endif
            
            sns_ddf_delay(GYRO_INIT_DELAY_STEP8);

#ifndef GEN2            
            // Reg 0x4C, set to 0x2C (Window1DataMask = 0(Enable), WIN2 = 1(Disable), WIN1 = 0(Enable), TBST=11 (SOC boost time 100ms), BST=0(OFF)
            write_buffer[0] = 0x2C;
            status = sns_ddf_write_port(
                state->port_handle,
                0x4C,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
                return status;
            }
#endif
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_POWERSTATE_LOWPOWER:
        {
            // Set sensor into "low power"
#ifdef GEN2   
        write_buffer[0] = 0x00;   // GEN2 doesn't have RSTPD bit
#else
        write_buffer[0] = 0x04;
#endif
            status = sns_ddf_write_port(
                state->port_handle,
                0x3E,           // Register address to write to
                write_buffer,
                1,              // Number of bytes
                &write_count);
            
#ifdef GEN2
#ifndef DDK
#ifdef GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY
            DalTlmm_GpioOut( state->gpio_handle, state->gpio_sleep_pin_cfg, DAL_GPIO_HIGH_VALUE );
#endif
#else
            PIO_Set(&pinSLEEP);    // puts gyro into hardware sleep mode
#endif /* DDK */
#endif       
            if(status != SNS_DDF_SUCCESS)
            {
                sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_GYRO, SNS_DDF_EVENT_EBUS);
                return status;
            }
            return SNS_DDF_SUCCESS;
        }

        default:
        {
            return SNS_DDF_EINVALID_PARAM;
        }
    }
}


/*****************************************
    sns_dd_panasonic_gyro_set_range
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_range(sns_ddf_handle_t dd_handle, uint8_t range_index)
{
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t read_buffer[1];
    uint8_t read_count;
    uint8_t write_buffer[1];
    uint8_t write_count;

    // Set range via index into list of ranges supported by device.    
    // 500 dps  50.0 LSB/dps
    // 1000 dps 25.0 LSB/dps
    // 2000 dps 12.5 LSB/dps

    // Read the register so we can write back without changing other bits
    status = sns_ddf_read_port(
        state->port_handle,
        0x16,           // register address 0x16 (FS_DLPF_CTRL)
        read_buffer,
        1,              // num bytes
        &read_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    switch(range_index)
    {
        case 0: // 500 dps  50.0 LSB/dps. Set bit 3, clear bit 4.
        {
            write_buffer[0] = ((read_buffer[0] | 0x08) & 0xEF);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            break;
        }

        case 1: // 1000 dps 25.0 LSB/dps. Set bit 4, clear bit 3.
        {
            write_buffer[0] = ((read_buffer[0] | 0x10) & 0xF7);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            break;
        }

        case 2: // 2000 dps 12.5 LSB/dps.  Set bits 3 and 4.
        {
            write_buffer[0] = (read_buffer[0] | 0x18);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            break;
        }
        
#ifdef GEN2       
        case 3: // 250 dps 100 LSB/dps.  Clear bits 3 and 4.
        {
            write_buffer[0] = (read_buffer[0] & ~0x18);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            break;
        }
#endif
        
        default:
        {
            return SNS_DDF_EINVALID_PARAM;
        }

    }
    
    // Now reset the SOC
#ifdef GEN2
    
        write_buffer[0] = 0x01;  // SOC disable setting
        status = sns_ddf_write_port(state->port_handle, 0x29, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS;   
    
        sns_ddf_delay(1000); 
        
        write_buffer[0] = 0x00;  // SOC enable setting
        status = sns_ddf_write_port(state->port_handle, 0x29, write_buffer, 1, &write_count);
        if (status != SNS_DDF_SUCCESS) return SNS_DDF_EBUS; 
        
        sns_ddf_delay(1000); 

    // put into sleep
    write_buffer[0] = 0x00;   
    status = sns_ddf_write_port(state->port_handle, 0x3E, write_buffer, 1, &write_count);
    if(status != SNS_DDF_SUCCESS) return status;
    sns_ddf_delay(1000);
    
    // take out of sleep
    write_buffer[0] = 0x40;   
    status = sns_ddf_write_port(state->port_handle, 0x3E, write_buffer, 1, &write_count);
    if(status != SNS_DDF_SUCCESS) return status;

    sns_ddf_delay(1000);
       
#endif
    
#ifndef GEN2    
    // Reg 0x4C, set to 0x7D (Window1DataMask = 1(Disable), WIN2 = 1(Disable), WIN1 = 1(Disable), TBST=11 (SOC boost time 100ms), BST=1(ON)
    write_buffer[0] = 0x7D;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4C,           // Register address
        write_buffer,
        1,
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    sns_ddf_delay(GYRO_INIT_DELAY_STEP8);
    
    // Reg 0x4C, set to 0x2C (Window1DataMask = 0(Enable), WIN2 = 1(Disable), WIN1 = 0(Enable), TBST=11 (SOC boost time 100ms), BST=0(OFF)
    write_buffer[0] = 0x2C;
    status = sns_ddf_write_port(
        state->port_handle,
        0x4C,           // Register address
        write_buffer,
        1,              // Num bytes
        &write_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }
#endif
    
    return SNS_DDF_SUCCESS;
}


/*****************************************
    sns_dd_panasonic_gyro_set_resolution_adc
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_resolution_adc(sns_ddf_handle_t dd_handle, uint8_t resolution_adc_index)
{
    // Set ADC resolution via an index into list of supported ADC resolutions.
    // There is currently only one available resolution, so just return success.
    return SNS_DDF_SUCCESS;
}


/*****************************************
    sns_dd_panasonic_gyro_set_lowpass
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_lowpass(sns_ddf_handle_t dd_handle, uint8_t lowpass_index)
{
    // Sets the LPF frequency via an index into the list of supported frequencies.
    sns_dd_panasonic_gyro_state_s* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t read_buffer[1];
    uint8_t read_count;
    uint8_t write_buffer[1];
    uint8_t write_count;

    status = sns_ddf_read_port(
        state->port_handle,
        0x16,           // register address 0x16 (FS_DLPF_CTRL)
        read_buffer,
        1,              // num bytes
        &read_count);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    switch(lowpass_index)
    {
        case 0: // GYRO_LPF_0, set DLPF = 6 (set bits 2 and 1, clear bit 0)
        {
            write_buffer[0] = ((read_buffer[0] | 0x06) & (0xFE));
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }

        case 1: // GYRO_LPF_1, set DLPF = 5 (set bits 2 and 0, clear bit 1)
        {
            write_buffer[0] = ((read_buffer[0] | 0x05) & 0xFD);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }

        case 2: // GYRO_LPF_2, set DLPF = 4 (set bit 2, clear bits 0 and 1)
        {
            write_buffer[0] = ((read_buffer[0] | 0x04) & 0xFC);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }

        case 3: // GYRO_LPF_3, set DLPF = 3 (set bit 0 and 1, clear bit 2)
        {
            write_buffer[0] = ((read_buffer[0] | 0x03) & 0xFB);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }
        case 4: // GYRO_LPF_4, set DLPF = 2 (set bit 1, clear bit 0 and 2)
        {
            write_buffer[0] = ((read_buffer[0] | 0x02) & 0xFA);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }
        case 5: // GYRO_LPF_5, set DLPF = 1 (set bit 0, clear bit 1 and 2)
        {
            write_buffer[0] = ((read_buffer[0] | 0x01) & 0xF9);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }
        case 6: // GYRO_LPF_6, set DLPF = 0 (clear bits 0, 1 and 2)
        {
            write_buffer[0] = (read_buffer[0] & 0xF8);
            status = sns_ddf_write_port(
                state->port_handle,
                0x16,           // Register address
                write_buffer,
                1,              // Num bytes
                &write_count);
            if(status != SNS_DDF_SUCCESS)
            {
                return status;
            }
            return SNS_DDF_SUCCESS;
        }
        
        default:
        {
            return SNS_DDF_EINVALID_PARAM;
        }    
    }
}


/*****************************************
    sns_dd_panasonic_gyro_set_motion_detect
*****************************************/
sns_ddf_status_e sns_dd_panasonic_gyro_set_motion_detect(sns_ddf_handle_t dd_handle, bool motion_detect)
{
    return SNS_DDF_EINVALID_PARAM;
}


const char* getStatusString( sns_ddf_status_e status )
{
    static const char str_sns_ddf_success[]        = "SNS_DDF_SUCCESS";
    static const char str_sns_ddf_pending[]        = "SNS_DDF_PENDING";
    static const char str_sns_ddf_efail[]          = "SNS_DDF_EFAIL";
    static const char str_sns_ddf_enomem[]         = "SNS_DDF_ENOMEM";
    static const char str_sns_ddf_einvalid_data[]  = "SNS_DDF_EINVALID_DATA";
    static const char str_sns_ddf_einvalid_param[] = "SNS_DDF_EINVALID_PARAM";
    static const char str_sns_ddf_einvalid_attr[]  = "SNS_DDF_EINVALID_ATTR";
    static const char str_sns_ddf_ebus[]           = "SNS_DDF_EBUS";
    static const char str_sns_ddf_edevice[]        = "SNS_DDF_EDEVICE";
    static const char str_sns_ddf_edata_bound[]    = "SNS_DDF_EDATA_BOUND";
    static const char str_sns_ddf_edevice_busy[]   = "SNS_DDF_EDEVICE_BUSY";
    static const char str_sns_ddf_einvalid_test[]  = "SNS_DDF_EINVALID_TEST";
    static const char str_sns_ddf_default[]        = "SNS_DDF_DEFAULT";

    const char* string;

    switch( status )
    {
    case SNS_DDF_SUCCESS:        string = str_sns_ddf_success;        break;
    case SNS_DDF_PENDING:        string = str_sns_ddf_pending;        break;
    case SNS_DDF_EFAIL:          string = str_sns_ddf_efail;          break;
    case SNS_DDF_ENOMEM:         string = str_sns_ddf_enomem;         break;
    case SNS_DDF_EINVALID_DATA:  string = str_sns_ddf_einvalid_data;  break;
    case SNS_DDF_EINVALID_PARAM: string = str_sns_ddf_einvalid_param; break;
    case SNS_DDF_EINVALID_ATTR:  string = str_sns_ddf_einvalid_attr;  break;
    case SNS_DDF_EBUS:           string = str_sns_ddf_ebus;           break;
    case SNS_DDF_EDEVICE:        string = str_sns_ddf_edevice;        break;
    case SNS_DDF_EDATA_BOUND:    string = str_sns_ddf_edata_bound;    break;
    case SNS_DDF_EDEVICE_BUSY:   string = str_sns_ddf_edevice_busy;   break;
    case SNS_DDF_EINVALID_TEST:  string = str_sns_ddf_einvalid_test;  break;
    default:                     string = str_sns_ddf_default;        break;
    }

    return string;
}
