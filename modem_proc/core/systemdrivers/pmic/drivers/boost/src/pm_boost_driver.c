/*! \file
*  \n
*  \brief
*  pm_boost_driver.c
*  \details
*  \n &copy; Copyright 2012 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/29/13    aks      Adding support for 5v boost as a separate driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_boost_driver.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
/* Static global variable to store the BOOST driver data */
static pm_boost_data_type *pm_boost_data_arr[PM_MAX_NUM_PMICS];


/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS

===========================================================================*/

void pm_boost_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_boost_data_type *boost_ptr = NULL;
    uint16 boost_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    boost_ptr = pm_boost_data_arr[pmic_index];

    if (boost_ptr == NULL)
    {
        pm_malloc( sizeof(pm_boost_data_type), (void**)&boost_ptr);

        /* Assign Comm ptr */
        boost_ptr->comm_ptr = comm_ptr;

        /* BOOST Register Info - Obtaining Data through dal config */
        boost_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_BOOST_REG);

        CORE_VERIFY_PTR(boost_ptr->pm_pwr_data.pwr_reg_table);

        boost_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_BOOST_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(boost_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* BOOST pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(boost_ptr->pm_pwr_data.num_of_peripherals), (void**)&(boost_ptr->pm_pwr_data.pwr_specific_info));

        CORE_VERIFY_PTR(boost_ptr->pm_pwr_data.pwr_specific_info);

        /* Save first BOOST peripheral's base address */
        boost_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;
        pm_boost_data_arr[pmic_index] = boost_ptr;
    }

    if (boost_ptr != NULL)
    {
        base_address = boost_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        periph_offset = boost_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Peripheral base_address should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        /* Calculate BOOST peripheral index */
        boost_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Peripheral Index should be less than number of peripherals */
        CORE_VERIFY(boost_index < (boost_ptr->pm_pwr_data.num_of_peripherals));

        /* Save BOOST's Peripheral Type value */
        boost_ptr->pm_pwr_data.pwr_specific_info[boost_index].periph_type = peripheral_info->peripheral_type;

        /* Save each BOOST peripheral's base address */
        boost_ptr->pm_pwr_data.pwr_specific_info[boost_index].periph_base_address = peripheral_info->base_address;

         /* Boost bypass has only range 0 */
        boost_ptr->pm_pwr_data.pwr_specific_info[boost_index].pwr_range = 0;

        /* Boost bypass Vset Info - Obtaining Data through dal config */
        boost_ptr->pm_pwr_data.pwr_specific_info[boost_index].pwr_vset = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_BOOST_VOLT);
    }
}

pm_boost_data_type* pm_boost_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_boost_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_boost_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_boost_data_arr[pmic_index] != NULL))
  {
      return pm_boost_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return NULL;
}

