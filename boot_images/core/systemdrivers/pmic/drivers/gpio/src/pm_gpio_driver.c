/*! \file pm_gpio_driver.c
 *  \n
 *  \brief  
 *  \details  
 *  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/gpio/src/pm_gpio_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
03/25/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_gpio_driver.h"
#include "CoreVerify.h"
#include "device_info.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the GPIO data */
static pm_gpio_data_type *pm_gpio_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_gpio_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
	pm_gpio_data_type *gpio_ptr = NULL;
	uint32 prop_id_arr[] = {PM_PROP_GPIOA_NUM, PM_PROP_GPIOB_NUM};
		
	gpio_ptr = pm_gpio_data_arr[pmic_index];

	if (gpio_ptr == NULL)
	{

	    pm_malloc( sizeof(pm_gpio_data_type), (void**)&gpio_ptr); 
												
        /* Assign Comm ptr */
        gpio_ptr->comm_ptr = comm_ptr;

        /* GPIO Register Info - Obtaining Data through dal config */
        gpio_ptr->gpio_reg_table = (pm_gpio_register_info_type*)pm_target_information_get_common_info(PM_PROP_GPIO_REG);
        CORE_VERIFY_PTR(gpio_ptr->gpio_reg_table);

        /* GPIO Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));
	
        gpio_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index],pmic_index);
        CORE_VERIFY(gpio_ptr->num_of_peripherals  != 0);

        pm_gpio_data_arr[pmic_index] = gpio_ptr;
    }
}

pm_gpio_data_type* pm_gpio_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_gpio_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_gpio_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_gpio_data_arr[pmic_index] != NULL))
  {
      return pm_gpio_data_arr[pmic_index]->num_of_peripherals;
  }

  return NULL;
}
