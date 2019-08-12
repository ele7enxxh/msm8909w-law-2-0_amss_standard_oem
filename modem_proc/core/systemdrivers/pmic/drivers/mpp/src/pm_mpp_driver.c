/*! \file
*  \n
*  \brief  pm_mpp_driver.c
*  \details
*  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/mpp/src/pm_mpp_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   hs      Updated for Badger.
04/05/11   hs      Refactored the code.
12/20/10   wra     Initial Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_mpp_driver.h"


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
    uint8 index;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    mpp_ptr = pm_mpp_data_arr[pmic_index];

    if (mpp_ptr == NULL)
    {
        pm_malloc( sizeof(pm_mpp_data_type), (void**)&mpp_ptr);

        /* Assign Comm ptr */
        mpp_ptr->comm_ptr = comm_ptr;

        /* LDO Register Info - Obtaining Data through dal config */
        mpp_ptr->mpp_register = (pm_mpp_register_info_type*)pm_target_information_get_common_info(PM_PROP_MPP_REG);

        CORE_VERIFY_PTR(mpp_ptr->mpp_register);

        mpp_ptr->mpp_common = (pm_mpp_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_MPP_SPECIFIC);

        CORE_VERIFY_PTR(mpp_ptr->mpp_common);

        mpp_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_MPP_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(mpp_ptr->num_of_peripherals != 0);

        pm_malloc(sizeof(uint32)* (mpp_ptr->num_of_peripherals), (void**)&(mpp_ptr->m_mpp_using_abus));
        for(index = 0; index < mpp_ptr->num_of_peripherals; index++)
        {
            mpp_ptr->m_mpp_using_abus[index] = 0;
        }

        pm_malloc(sizeof(int)* (mpp_ptr->mpp_common->num_of_dbus), (void**)&(mpp_ptr->m_dbus_busy));
        for(index = 0; index < mpp_ptr->mpp_common->num_of_dbus; index++)
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
