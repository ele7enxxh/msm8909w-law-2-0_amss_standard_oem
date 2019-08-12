/* File: sns_dd_panasonic_gyro.h

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

#ifndef SNS_DD_PANASONIC_GYRO_H
#define SNS_DD_PANASONIC_GYRO_H


/**
 * @File sns_dd_panasonic_gyro.h
 *
 * This file declares the interface functions for the Panasonic Gyro Driver.
 */



//*****************
// VERSION INFO
//*****************

typedef enum version_enum {
  // These need to be of type int32
   
  /* 1.0.3:  - Approved version by Qualcomm for GEN3
   *         - Removed asynchronous initialization code because Qualcomm has no plans to support this
   *	     - Cleaned up whitespace to only use spaces instead of tabs  */
  driver_version_1_0_3 = 2,  
    
  /* 1.0.4:  - Compiler warning fixed on request of Qualcomm, but version ignored by Qualcomm */
  driver_version_1_0_4,
     
  /* 1.0.5:  - First version with factory self test for GEN3 with the following test cases:
   *           SNS_DDF_TEST_SELF_SW
   *           SNS_DDF_TEST_SELF_HW   (SNS_DDF_TEST_SELF also runs this)
   *           SNS_DDF_TEST_CONNECTIVITY 
   *         - I did implement factory calibration, which essentially consists of returning the bias,  
   *           but #defined it out (//#define BIAS) because of three issues explained at the the #define BIAS statement.
   *         - Sent to Qualcomm, but ignored by Qualcomm */
  driver_version_1_0_5,   
  
  /* 1.0.6:  - First version including GEN2.  GEN2 tested and approved by Qualcomm.  
   *         - GEN3 mode works in DDK, but testing at Qualcomm revealed that GEN3 no longer works
   *           on deloyment platform (probably because of added factory self test in 1_0_5).  
   *         - GEN2 factory self test not yet implemented.
   *         - Added ability to use GPIOs and LEDs on DDK platform 
   *         - Improved debug output for plotting with Gnuplot
   *         - Created Gnuplot plotscript
   */
  driver_version_1_0_6,
  
  /* 1.1.0   - GEN2 renamed to GEN2_5
   *         - First version with naming for panasonic_gyro instead of gyro_ewtzna to represent 
   *           both GEN3 and GEN2.        
   *         - Reverted x_sample int16 to int32 and #defined out factory-self-test run_test() 
   *           and brought pre-factory-self-test run_test() back in because
   *           GEN3 was no longer working in 1_0_6 at Qualcomm. (see #define FACTORY_SELF_TEST).
   *         - Works on both GEN2 and GEN3 on DDK
   *         - Sent to Qualcomm for testing on deployment platform as handover candidate with 
   *           working GEN2 and GEN3, but non-working factory self test and deliberately omitted 
   *           factory calibration */
  driver_version_1_1_0,      

  /* 1.1.1   - GEN2_5 renamed back to GEN2
   *         - This version was approved by Qualcomm for GEN3
   */
  driver_version_1_1_1,

  /* 1.1.2   - Added BSD license text
   */
  driver_version_1_1_2,

} version_e;

#define CURRENT_VERSION driver_version_1_1_2;



//*****************
// CONFIGURATION
//*****************

/* Panasonic Gyro Hardware Version 2 EWTSAFMA21 (instead of version 3 EWTZNA) */
// #define GEN2         

/* Choose whether compilation is for DDK or Deployment Platform */
// #define DDK 

/* New Qualcomm DSPS API */
// #define NEW_API                          

/* Returning a BIAS for OEM factory calibration is not recommended due to the following 
 * potential issues regarding measuring the bias at the OEM factory: 
 *
 * - When running the SNS_DDF_TEST_SELF_SW, the hardware self calibration of the 
 *   gyro is off and the bias would be calculated during the 5 seconds of data 
 *   measurement.  After software self test, the gyro would be reset and initialized 
 *   again, which turns the hardware self calibration back on.  
 *   As the self offset calibration of the chip gradually reduces the bias, subtracting 
 *   the previously measured bias would actually cause a larger bias.  
 *
 * - Let's say we'd disable the hardware self offset calibration, then the gyro would 
 *   only get calibrated during those 5 seconds at the OEM factury, which might also 
 *   not be desirable.  
 *
 * - The bias is range dependent. The SNS_DDF_TEST_SELF_SW would return one bias for 
 *   each axis for one range setting, but if a different range setting is chosen, this 
 *   bias wouldn't make sense.  */
// #define BIAS 

/* Code for FACTORY_SELF_TEST made GEN3 to only work on DDK, but no longer on deployment
   platform and was not yet fully implemented for GEN2, which is why it is commented out, 
   but left in the code for future completion.  */
// #define FACTORY_SELF_TEST


#ifdef GEN2

/* GEN2 only:  Code for GEN2 sleep pin functionality on deployment platform.  
                 Implemented by Qualcomm, but not finished, which is why it is commented out.  
                 Luckily testing of GEN2 code was still possible because default state of sleep pin 
                 was "awake".  */
// #define GEN2_SLEEP_PIN_DEPLOYMENT_PLATFORM_FUNCTIONALITY

#endif  // GEN2


#ifdef DDK

/* DDK Platform only:  When enabled, various debugging messages are printed using stdout.  
                       We also get data on an interval defined by TEST_TIMER DURATION and 
                       print it out using stdout.  */
// #define GYRO_DEBUG                       
                              
/* DDK Platform only:  When enabled, we get data on an interval defined by TEST_TIMER DURATION
                       and print out using stdout. Do not use simultaneously with GYRO_DEBUG.  */
// #define GYRO_STREAM_FOR_TESTING_LEGIBLE  

/* DDK Platform only:  call run_test  */
// #define GYRO_FACTORY_TEST_TESTING   

#endif  // DDK





//******************
// GLOBALS
//******************
extern sns_ddf_driver_if_s sns_dd_panasonic_gyro_if;


//******************
// PUBLIC FUNCTIONS
//******************

/**
 * Initialization function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e**       sensors,
    uint32_t*                num_sensors);


/**
 * Get data function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_get_data(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensors[],
    uint32_t                 num_sensors,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_data_s**  data);


/**
 * Set attribute function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_set_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value);


/**
 * Get attribute function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_get_attrib(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems);


/**
 * Handle timer function for Panasonic Gyro driver
 */
void sns_dd_panasonic_gyro_handle_timer(sns_ddf_handle_t dd_handle, void* arg);


/**
 * Handle interrupt function for Panasonic Gyro driver
 */
#ifdef NEW_API
void sns_dd_panasonic_gyro_handle_irq(sns_ddf_handle_t dd_handle, uint32_t gpio_num, sns_ddf_time_t timestamp);
#else
void sns_dd_panasonic_gyro_handle_irq(sns_ddf_handle_t dd_handle, sns_ddf_signal_irq_e irq);
#endif


/**
 * Reset function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_reset(sns_ddf_handle_t dd_handle);


/**
 * Run test function for Panasonic Gyro driver
 */
sns_ddf_status_e sns_dd_panasonic_gyro_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err);


#endif // SNS_DD_panasonic_gyro_H
