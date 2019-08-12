/*! \file pm_lpg_driver.c 
*  \n
*  \brief This file contains LPG peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lpg/src/pm_lpg_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lpg_driver.h"
#include "CoreVerify.h"
#include "pm_comm.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the LPG driver data */
static pm_lpg_data_type *pm_lpg_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_lpg_driver_init(pm_comm_info_type *comm_ptr , uint8 pmic_index)
{
    pm_lpg_data_type *lpg_ptr = NULL;
    uint32 prop_id_arr[] = {PM_PROP_LPGA_NUM, PM_PROP_LPGB_NUM}; 

    lpg_ptr = pm_lpg_data_arr[pmic_index];
    
    if (lpg_ptr == NULL)
    {
        pm_malloc( sizeof(pm_lpg_data_type), (void**)&lpg_ptr);
                                           
        /* Assign Comm ptr */
        lpg_ptr->comm_ptr = comm_ptr;

        /* LPG Register Info - Obtaining Data through dal config */
        lpg_ptr->lpg_reg_table = (pm_lpg_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_LPG_REG);
        CORE_VERIFY_PTR(lpg_ptr->lpg_reg_table);

        /* LPG Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

		lpg_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index],pmic_index);
        CORE_VERIFY(lpg_ptr->num_of_peripherals  != 0);

		pm_lpg_data_arr[pmic_index] = lpg_ptr;

    }
}

pm_lpg_data_type* pm_lpg_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_lpg_data_arr[pmic_index];
  }

  return NULL;
}
