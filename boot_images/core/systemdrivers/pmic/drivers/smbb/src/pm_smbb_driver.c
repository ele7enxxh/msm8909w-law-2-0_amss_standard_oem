/*! \file pm_smbb_driver.c
 *  \n
 *  \brief  
 *  \details  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smbb/src/pm_smbb_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/13   aab     Added pm_smbb_chg_get_chg_gone_rt_status() 
06/17/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smbb_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the MPP data */
static pm_smbb_data_type *pm_smbb_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/


void pm_smbb_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index)
{
    pm_smbb_data_type* smbb_ptr = NULL;
	uint32 prop_id_arr[] = {PM_PROP_SMBBA_NUM, PM_PROP_SMBBB_NUM};

 	smbb_ptr = pm_smbb_data_arr[pmic_index];

	if (NULL == smbb_ptr)
    {
        pm_malloc(sizeof(pm_smbb_data_type), (void**)&smbb_ptr);
                                                    
        /* Assign Comm ptr */
        smbb_ptr->comm_ptr = comm_ptr;
		
		/* SMBB Register Info - Obtaining Data through dal config */
        smbb_ptr->smbb_reg_table = (pm_smbb_register_info_type*)pm_target_information_get_common_info(PM_PROP_SMBB_REG);
        CORE_VERIFY_PTR(smbb_ptr->smbb_reg_table);
		
        /* SMBB Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));
		
        smbb_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);
        CORE_VERIFY(smbb_ptr->num_of_peripherals  != 0);

        pm_smbb_data_arr[pmic_index] = smbb_ptr;
    }
}


pm_smbb_data_type* pm_smbb_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_smbb_data_arr[pmic_index];
    }

    return NULL;
}


