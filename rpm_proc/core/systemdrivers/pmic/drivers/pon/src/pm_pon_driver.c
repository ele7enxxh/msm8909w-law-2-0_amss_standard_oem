/*! \file pm_pon_driver.c
*  \n
*  \brief This file contains PON peripheral driver initialization during which the driver
*         driver data is stored.
*  \n
*  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/pon/src/pm_pon_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/14   kt      Created.
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon_driver.h"
#include "CoreVerify.h"
#include "device_info.h"
#include "pm_utils.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the PON data */
static pm_pon_data_type *pm_pon_data_arr[PM_MAX_NUM_PMICS] ; 

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

void pm_pon_driver_init(pm_comm_info_type *comm_ptr, uint8 pmic_index)
{
    pm_pon_data_type *pon_ptr = NULL;
    
    pon_ptr = pm_pon_data_arr[pmic_index];
    if (pon_ptr == NULL)
    {
       pm_malloc( sizeof(pm_pon_data_type), (void**)&pon_ptr); 

       /* Assign Comm ptr */
       pon_ptr->comm_ptr = comm_ptr;

       pm_pon_data_arr[pmic_index] = pon_ptr;
    }
}

pm_pon_data_type* pm_pon_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_pon_data_arr[pmic_index];
    }

    return NULL;
}
