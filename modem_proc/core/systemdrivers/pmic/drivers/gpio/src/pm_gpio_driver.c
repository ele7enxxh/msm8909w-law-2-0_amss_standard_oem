/*! \file
 *  \n
 *  \brief  pm_gpio_driver.c
 *  \details
 *  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/gpio/src/pm_gpio_driver.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
03/01/12    hs        Updated the driver.
01/12/11    wra     Added target specific configuration hookups
10/29/10    wra     Initial Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_gpio_driver.h"


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

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    gpio_ptr = pm_gpio_data_arr[pmic_index];

    if (gpio_ptr == NULL)
    {
        pm_malloc( sizeof(pm_gpio_data_type), (void**)&gpio_ptr);

        /* Assign Comm ptr */
        gpio_ptr->comm_ptr = comm_ptr;

        /* LDO Register Info - Obtaining Data through dal config */
        gpio_ptr->gpio_register = (pm_gpio_register_info_type*)pm_target_information_get_common_info(PM_PROP_GPIO_REG);

        CORE_VERIFY_PTR(gpio_ptr->gpio_register);

        gpio_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_GPIO_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(gpio_ptr->num_of_peripherals != 0);

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
    if( (pmic_index < PM_MAX_NUM_PMICS) &&
        (NULL != pm_gpio_data_arr[pmic_index]) )
    {
        return pm_gpio_data_arr[pmic_index]->num_of_peripherals;
    }

    return NULL;
}