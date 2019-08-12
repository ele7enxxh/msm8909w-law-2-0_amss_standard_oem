/*! \file pm_pbs_client_driver.c
*  \n
*  \brief PBS Client driver initialization.
*  \n  
*  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
06/13/13   kt      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "hw_module_type.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/
/* Static global variable to store the RGB driver data */
static pm_pbs_client_data_type *pm_pbs_client_data_arr[PM_MAX_NUM_PMICS]; 

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_pbs_client_data_type *pbs_client_ptr = NULL;
    uint32 prop_id_arr[] = {PM_PROP_PBS_CLIENTA_NUM, PM_PROP_PBS_CLIENTB_NUM}; 

    if (peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT)
    {
        return;
    }

    pbs_client_ptr = pm_pbs_client_data_arr[pmic_index];
    
    if (pbs_client_ptr == NULL)
    {
        pm_malloc( sizeof(pm_pbs_client_data_type), (void**)&pbs_client_ptr);

        /* Assign Comm ptr */
        pbs_client_ptr->comm_ptr = comm_ptr;

        /* PBS Client Register Info - Obtaining Data through dal config */
        pbs_client_ptr->pbs_client_reg_table = (pm_pbs_client_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_PBS_CLIENT_REG);
        CORE_VERIFY_PTR(pbs_client_ptr->pbs_client_reg_table);

        /* PBS Client Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));
                                                              
		pbs_client_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index],pmic_index);
        CORE_VERIFY(pbs_client_ptr->num_of_peripherals != 0);
        pm_pbs_client_data_arr[pmic_index] = pbs_client_ptr;
    }
}

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_pbs_client_data_arr[pmic_index];
  }

  return NULL;
}

