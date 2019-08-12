/*! \file
 *  \n
 *  \brief  pm_wled_driver.c  
 *  \details  
 *  \n &copy; Copyright 2010 - 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/wled/src/pm_wled_driver.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
08/20/14    al      Updating comm lib  
08/29/14    al      KW fixes
05/20/14    al      Architecture update
05/09/14    al      Added AMOLED, PFM and PSM
05/09/14    va      Using common debug and assert Marco
03/26/14    al      8994 WLED support 
09/17/13    al		Adding WLED IRQ APIs and updating get_status 
03/15/13    al		Rewriting according to new architecture 
10/10/12    al	    removed unimplemented
08/12/12    al		added interrupt related declarations
07/24/12    al      New file
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_wled_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the WLED driver data */
static pm_wled_data_type *pm_wled_data_arr[PM_MAX_NUM_PMICS];
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_wled_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_wled_data_type *wled_ptr = NULL;

    wled_ptr = pm_wled_data_arr[pmic_index];

    if (NULL == wled_ptr)
    {
        pm_malloc( sizeof(pm_wled_data_type), (void**)&wled_ptr);
                                                    
        /* Assign Comm ptr */
        wled_ptr->comm_ptr = comm_ptr;

        /* WLED Register Info - Obtaining Data through dal config */
        wled_ptr->wled_reg_table = (pm_wled_register_info_type*)pm_target_information_get_common_info(PM_PROP_WLED_REG);
        CORE_VERIFY_PTR(wled_ptr->wled_reg_table);

        //wled_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_WLED;
		wled_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_WLED_NUM, pmic_index);
        CORE_VERIFY(wled_ptr->num_of_peripherals != 0);

        pm_wled_data_arr[pmic_index] = wled_ptr;
    }
}

pm_wled_data_type* pm_wled_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_wled_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_wled_get_num_peripherals(uint8 pmic_index)
{
  if((pm_wled_data_arr[pmic_index] !=NULL)&&
  	  (pmic_index < PM_MAX_NUM_PMICS))
    {
      return pm_wled_data_arr[pmic_index]->num_of_peripherals;
    }

    return 0;
}
