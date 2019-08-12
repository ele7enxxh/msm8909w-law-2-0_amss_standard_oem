/*! \file
 *  \n
 *  \brief  pm_xo_driver.c
 *  \details
 *  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/xo/src/pm_xo_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   rh      File created
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_xo_driver.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the xo data */
static pm_xo_core_data_type *pm_xo_core_data_arr[PM_MAX_NUM_PMICS];;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_xo_core_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_xo_core_data_type *xo_ptr = NULL;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    xo_ptr = pm_xo_core_data_arr[pmic_index];

    if (xo_ptr == NULL)
    {
        pm_malloc( sizeof(pm_xo_core_data_type), (void**)&xo_ptr);

        /* Assign Comm ptr */
        xo_ptr->comm_ptr = comm_ptr;

        /* XO Register Info - Obtaining Data through dal config */
        xo_ptr->xo_core_register = (pm_xo_core_register_info_type*)pm_target_information_get_common_info(PM_PROP_CLK_XO_REG);

        CORE_VERIFY_PTR(xo_ptr->xo_core_register);

        xo_ptr->xo_core_common = (pm_xo_core_specific_info_type*)pm_target_information_get_specific_info(PM_PROP_XO_SPECIFIC);

        CORE_VERIFY_PTR(xo_ptr->xo_core_common);

        xo_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_MEGAXO_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(xo_ptr->num_of_peripherals != 0);

        pm_xo_core_data_arr[pmic_index] = xo_ptr;
    }
}

pm_xo_core_data_type* pm_xo_core_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_xo_core_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_xo_core_get_num_peripherals(uint8 pmic_index)
{
  if( (pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_xo_core_data_arr[pmic_index] != NULL))
  {
      return pm_xo_core_data_arr[pmic_index]->num_of_peripherals;
  }

  return NULL;
}

