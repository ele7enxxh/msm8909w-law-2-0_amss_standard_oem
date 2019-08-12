/*============================================================================
  @file sns_sam_algo_gpio.c

  GPIO related functions for SAM algorithms

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include "sns_debug_api.h"
#include "sns_sam_algo_gpio.h"
#include "sns_sam_pm.h"
#include "sns_debug_str.h"

#if defined(FEATURE_TEST_ALGO_GPIO_AMD) || defined(FEATURE_TEST_ALGO_GPIO_RMD)

#include "DDITlmm.h"
#include "DALDeviceId.h"

#define SNS_SAM_TEST_GPIO_MAX_NUM 1

/* Array of GPIO pins available for sensor test */
static uint16_t sns_sam_test_gpio_num_a[SNS_SAM_TEST_GPIO_MAX_NUM] = {118};

/* GPIO handle used to handle GPIOs */
static DalDeviceHandle *gpio_handle;

/**
 * @brief Configure and enable GPIOs
 *
 * @return None
 */
static void sns_sam_gpio_enable(sns_sam_algo_callback const *cbFunc, uint16_t gpio_num)
{
  DALResult result = DAL_ERROR;
  DALGpioSignalType gpio_cfg;

  gpio_cfg = DAL_GPIO_CFG_OUT(gpio_num,
                              0,
                              DAL_GPIO_OUTPUT,
                              DAL_GPIO_NO_PULL,
                              DAL_GPIO_2MA,
                              DAL_GPIO_LOW_VALUE);


  result = DalTlmm_ConfigGpio(gpio_handle,
                              gpio_cfg,
                              DAL_TLMM_GPIO_ENABLE);
  if( result != DAL_SUCCESS )
  {
    cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "Error enabling gpio: %d, result: %d",
        &(sns_sam_algo_debug_params) { .filename = "sns_sam_algo_gpio.c", .line_num = __LINE__, .num_params_valid = 2, .param1 = gpio_num, .param2 = result });
  }
}

/**
 * Initialize GPIO.
 *
 * @return none
 */
void sns_sam_init_gpio( sns_sam_algo_callback const *cbFunc )
{
  DALResult result = DAL_ERROR;
  uint32_t i;
  result = DAL_DeviceAttach(DALDEVICEID_TLMM, &gpio_handle);
  if( result != DAL_SUCCESS )
  {
    gpio_handle = NULL;
    cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "Error initializing gpio handle, result: %d",
        &(sns_sam_algo_debug_params) { .filename = "sns_sam_algo_gpio.c", .line_num = __LINE__, .num_params_valid = 1, .param1 = result });
    return;
  }

  for( i=0; i<SNS_SAM_TEST_GPIO_MAX_NUM; i++ )
  {
    sns_sam_gpio_enable(cbFunc, sns_sam_test_gpio_num_a[i]);
  }
}


/**
 * Set GPIO state on a particular GPIO pin. This function can only be used by
 * one algorithm each time, since all sensors algos are sharing the same GPIO
 * pins for this test.
 *
 * @param[i] gpioIndex: index of GPIO array to indicate which GPIO is to be set
 * @param[i] setToHigh: whether set this GPIO to high
 *
 * @return None
*/
SNS_SAM_UIMAGE_CODE void sns_sam_algo_set_gpio(sns_sam_algo_callback const *cbFunc, uint32_t gpioIndex, bool setToHigh)
{
  DALResult result = DAL_ERROR;
  int16_t gpio_num;
  DALGpioSignalType gpio_cfg;

  if(gpioIndex < SNS_SAM_TEST_GPIO_MAX_NUM && NULL != gpio_handle)
  {
    gpio_num = sns_sam_test_gpio_num_a[gpioIndex];
    gpio_cfg = DAL_GPIO_CFG_OUT(gpio_num,
                                0,
                                DAL_GPIO_OUTPUT,
                                DAL_GPIO_NO_PULL,
                                DAL_GPIO_2MA,
                                DAL_GPIO_LOW_VALUE);

    result = DalTlmm_GpioOut(gpio_handle,
                             gpio_cfg,
                             setToHigh ? DAL_GPIO_HIGH_VALUE : DAL_GPIO_LOW_VALUE);

    if( DAL_SUCCESS != result )
    {
      cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "Error setting QMD GPIO: %d, err: %d",
          &(sns_sam_algo_debug_params) { .filename = "sns_sam_algo_gpio.c", .line_num = __LINE__, .num_params_valid = 2, .param1 = gpio_num, .param2 = result });
    }
  }
  else
  {
    cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "Error gpio index out of bound %d or gpio is not initialized correctly",
        &(sns_sam_algo_debug_params) { .filename = "sns_sam_algo_gpio.c", .line_num = __LINE__, .num_params_valid = 1, .param1 = gpioIndex });
  }
}
#else /* FEATURE_TEST_ALGO_GPIO */
/**
 * Initialize GPIO.
 *
 * @return none
 */
void sns_sam_init_gpio( sns_sam_algo_callback const *cbFunc )
{
  //do nothing
}

/**
 * @brief Pulls the GPIO pin up/down depending on input.
 *
 */
SNS_SAM_UIMAGE_CODE void sns_sam_algo_set_gpio(sns_sam_algo_callback const *cbFunc, uint32_t gpioIndex, bool setToHigh)
{
  //do nothing
}
#endif /* FEATURE_TEST_ALGO_GPIO */
