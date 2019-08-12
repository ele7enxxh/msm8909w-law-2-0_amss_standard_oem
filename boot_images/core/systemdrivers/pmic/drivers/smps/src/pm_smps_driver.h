#ifndef PM_SMPS_DRIVER__H
#define PM_SMPS_DRIVER__H

/*! \file pm_smps_driver.h
 *  \n
 *  \brief This file contains SMPS peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smps/src/pm_smps_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
05/15/14   akm     Updated SMPS ilim and freq API
03/31/14   akm     Updated to the latest PMIC driver architecture   
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "pm_pwr_alg.h"

/*===========================================================================

                     SMPS TYPES AND STRUCTURES 

===========================================================================*/

typedef struct
{
    pm_comm_info_type  *comm_ptr;
    pm_pwr_data_type   pm_pwr_data;
} pm_smps_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_smps_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_smps_data_type* pm_smps_get_data(uint8 pmic_index);

#endif // PM_SMPS_DRIVER__H

