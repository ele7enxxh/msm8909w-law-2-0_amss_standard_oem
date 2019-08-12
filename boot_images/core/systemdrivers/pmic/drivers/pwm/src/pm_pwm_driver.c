/*! \file
 *  \n
 *  \brief  pwm_V6_Config.h 
 *  \details  
 *  \n &copy; Copyright 2012, 2014 QUALCOMM Technologies, Inc. All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pwm/src/pm_pwm_driver.c#1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------- 
08/20/14    al      Updating comm lib
08/29/14    al      KW fixes
05/20/14    al      Architecture update
05/09/14    va      Using common debug and assert Marco
10/16/12    al	    removed unimplemented irq
07/24/12    al      New file
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pwm_driver.h"
#include "CoreVerify.h"
#include "pm_utils.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the PWM driver data */
static pm_pwm_data_type *pm_pwm_data_arr[PM_MAX_NUM_PMICS];
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_pwm_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_pwm_data_type *pwm_ptr = NULL;

    pwm_ptr = pm_pwm_data_arr[pmic_index];

    if (NULL == pwm_ptr)
    {
        pm_malloc( sizeof(pm_pwm_data_type), (void**)&pwm_ptr);
                                                    
        /* Assign Comm ptr */
        pwm_ptr->comm_ptr = comm_ptr;

        /* PWM Register Info - Obtaining Data through dal config */
        pwm_ptr->pwm_reg_table = (pm_pwm_register_info_type *)pm_target_information_get_common_info(PM_PROP_PWM_REG);
        CORE_VERIFY_PTR(pwm_ptr->pwm_reg_table);

		pwm_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_PWM_NUM, pmic_index);
        CORE_VERIFY(pwm_ptr->num_of_peripherals != 0);

        pm_pwm_data_arr[pmic_index] = pwm_ptr;
    }
}

pm_pwm_data_type* pm_pwm_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_pwm_data_arr[pmic_index];
    }
    return NULL;
}

uint8 pm_pwm_get_num_peripherals(uint8 pmic_index)
{
  if((pm_pwm_data_arr[pmic_index] !=NULL)&& 
  	  (pmic_index < PM_MAX_NUM_PMICS))
  {
      return pm_pwm_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}

