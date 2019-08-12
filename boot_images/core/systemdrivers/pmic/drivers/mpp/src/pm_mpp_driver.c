/*! \file pm_mpp_driver.c 
*  \n
*  \brief This file contains MPP peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/mpp/src/pm_mpp_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture  
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_mpp_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the MPP data */
static pm_mpp_data_type *pm_mpp_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_mpp_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
	pm_mpp_data_type *mpp_ptr = NULL;
	uint32 prop_id_arr[] = {PM_PROP_MPPA_NUM, PM_PROP_MPPB_NUM};
	uint8 index;

    mpp_ptr = pm_mpp_data_arr[pmic_index];
    
    if (mpp_ptr == NULL)
    {
        pm_malloc( sizeof(pm_mpp_data_type), (void**)&mpp_ptr);                                    
        /* Assign Comm ptr */
        mpp_ptr->comm_ptr = comm_ptr;
		
		/* MPP Register Info - Obtaining Data through dal config */
        mpp_ptr->mpp_reg_table = (pm_mpp_register_info_type*)pm_target_information_get_common_info(PM_PROP_MPP_REG);
        CORE_VERIFY_PTR(mpp_ptr->mpp_reg_table);
		
        /* MPP Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));
        		
	
        mpp_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);
        CORE_VERIFY(mpp_ptr->num_of_peripherals  != 0);
		
		mpp_ptr->mpp_specific = (pm_mpp_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_MPP_SPECIFIC_DATA);
		CORE_VERIFY_PTR(mpp_ptr->mpp_specific);
		
		pm_malloc(sizeof(uint32)* (mpp_ptr->num_of_peripherals), (void**)&(mpp_ptr->m_mpp_using_abus)); 
		for(index = 0; index < mpp_ptr->num_of_peripherals; index++)
		{
			mpp_ptr->m_mpp_using_abus[index] = 0;
		}

		pm_malloc(sizeof(int)* (mpp_ptr->mpp_specific->num_of_dbus), (void**)&(mpp_ptr->m_dbus_busy)); 
		for(index = 0; index < mpp_ptr->mpp_specific->num_of_dbus; index++)
		{
			mpp_ptr->m_dbus_busy[index] = mpp_ptr->num_of_peripherals;
		}
     pm_mpp_data_arr[pmic_index] = mpp_ptr;
    }
}

pm_mpp_data_type* pm_mpp_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_mpp_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_mpp_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_mpp_data_arr[pmic_index] != NULL))
  {
      return pm_mpp_data_arr[pmic_index]->num_of_peripherals;
  }

  return NULL;
}
