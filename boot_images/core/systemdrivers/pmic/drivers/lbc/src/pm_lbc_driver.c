/*! \file pm_lbc_driver.c
 *  \n
 *  \brief  
 *  \details  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lbc/src/pm_lbc_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   pxm     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lbc_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the MPP data */
static pm_lbc_data_type *pm_lbc_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/


void pm_lbc_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index)
{
    pm_lbc_data_type* lbc_ptr = NULL;
	uint32 prop_id_arr[] = {PM_PROP_LBCA_NUM, PM_PROP_LBCB_NUM};

	lbc_ptr = pm_lbc_data_arr[pmic_index];

	if (NULL == lbc_ptr)
    {
        pm_malloc(sizeof(pm_lbc_data_type), (void**)&lbc_ptr);

        /* Assign Comm ptr */
        lbc_ptr->comm_ptr = comm_ptr;

		/* LBC Register Info - Obtaining Data through dal config */
        lbc_ptr->lbc_reg_table = (pm_lbc_register_info_type*)pm_target_information_get_common_info(PM_PROP_LBC_REG);
        CORE_VERIFY_PTR(lbc_ptr->lbc_reg_table);

        /* LBC Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        lbc_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);
        CORE_VERIFY(lbc_ptr->num_of_peripherals  != 0);

        pm_lbc_data_arr[pmic_index] = lbc_ptr;
    }
}

pm_lbc_data_type* pm_lbc_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_lbc_data_arr[pmic_index];
    }

    return NULL;
}


