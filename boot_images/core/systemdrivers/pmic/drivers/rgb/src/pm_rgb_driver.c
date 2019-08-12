/*! \file pm_rgb_driver.c 
*  \n
*  \brief This file contains RGB peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/rgb/src/pm_rgb_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rgb_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the RGB driver data */
static pm_rgb_data_type *pm_rgb_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_rgb_driver_init(pm_comm_info_type *comm_ptr , uint8 pmic_index)
{
    pm_rgb_data_type *rgb_ptr = NULL;
    uint32 prop_id_arr[] = {PM_PROP_RGBA_NUM, PM_PROP_RGBB_NUM}; 

    rgb_ptr = pm_rgb_data_arr[pmic_index];

    if (rgb_ptr == NULL)
    {
        pm_malloc( sizeof(pm_rgb_data_type), (void**)&rgb_ptr);                                            
        /* Assign Comm ptr */
        rgb_ptr->comm_ptr = comm_ptr;

        /* RGB Register Info - Obtaining Data through dal config */
        rgb_ptr->rgb_reg_table = (pm_rgb_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_RGB_REG);
        CORE_VERIFY_PTR(rgb_ptr->rgb_reg_table);

        /* RGB Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        rgb_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);
        CORE_VERIFY(rgb_ptr->num_of_peripherals != 0);

        pm_rgb_data_arr[pmic_index] = rgb_ptr;
    }
}

pm_rgb_data_type* pm_rgb_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_rgb_data_arr[pmic_index];
  }

  return NULL;
}


