/*! \file pm_vib_driver.c 
*  \n
*  \brief This file contains VIB peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/vib/src/pm_vib_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vib_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the VIB driver data */
static pm_vib_data_type *pm_vib_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_vib_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index)
{
    pm_vib_data_type *vib_ptr = NULL;
    uint32 prop_id_arr[] = {PM_PROP_VIBA_NUM, PM_PROP_VIBB_NUM}; 

    vib_ptr = pm_vib_data_arr[pmic_index];
    
    if (vib_ptr == NULL)
    {
        pm_malloc( sizeof(pm_vib_data_type), (void**)&vib_ptr);
                                                    
        /* Assign Comm ptr */
        vib_ptr->comm_ptr = comm_ptr;

        /* VIB Register Info - Obtaining Data through dal config */
        vib_ptr->vib_reg_table = (pm_vib_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_VIB_REG);
        CORE_VERIFY_PTR(vib_ptr->vib_reg_table);

        vib_ptr->vib_data = (pm_vib_specific_data_type*)pm_target_information_get_specific_info((uint32)PM_PROP_VIB_DATA);
        CORE_VERIFY_PTR(vib_ptr->vib_data);

        /* VIB Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        //vib_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_VIB;
        vib_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index],pmic_index);
        CORE_VERIFY(vib_ptr->num_of_peripherals  != 0);
        pm_vib_data_arr[pmic_index] = vib_ptr;
    }
}

pm_vib_data_type* pm_vib_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_vib_data_arr[pmic_index];
  }

  return NULL;
}
