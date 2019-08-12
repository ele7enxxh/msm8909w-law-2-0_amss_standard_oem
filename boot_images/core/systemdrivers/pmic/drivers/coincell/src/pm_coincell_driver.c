/*! \file pm_coincell_driver.c
*  \n
*  \brief This file contains coincell peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/coincell/src/pm_coincell_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/15   mr      Added support for Coincell Driver (CR-688196)
09/16/14   akm     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_coincell_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the coincell driver data */
static pm_coincell_data_type *pm_coincell_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_coincell_driver_init(pm_comm_info_type *comm_ptr,
                             peripheral_info_type *peripheral_info,
                             uint8 pmic_index)
{
    pm_coincell_data_type *coincell_ptr = NULL;
    uint32 prop_id_arr[] = {PM_PROP_COINCELLA_NUM};

    coincell_ptr = pm_coincell_data_arr[pmic_index];

    if (coincell_ptr == NULL)
    {
        pm_malloc( sizeof(pm_coincell_data_type), (void**)&coincell_ptr);
        /* Assign Comm ptr */
        coincell_ptr->comm_ptr = comm_ptr;

        /* coincell Register Info - Obtaining Data through dal config */
        coincell_ptr->coincell_reg_table = (pm_coincell_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_COINCELL_REG);
        CORE_VERIFY_PTR(coincell_ptr->coincell_reg_table);

        /* coincell Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        coincell_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);
        CORE_VERIFY(coincell_ptr->num_of_peripherals != 0);

        pm_coincell_data_arr[pmic_index] = coincell_ptr;
    }
}

pm_coincell_data_type* pm_coincell_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_coincell_data_arr[pmic_index];
  }

  return NULL;
}


