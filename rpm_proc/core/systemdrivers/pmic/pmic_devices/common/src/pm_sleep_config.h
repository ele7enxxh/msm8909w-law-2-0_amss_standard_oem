/*! \file
 *
 *  \brief  pm_sleep_config.h
 *  \details PMIC sleep API include file
 *
 *  &copy; Copyright 2014 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/pmic_devices/common/src/pm_sleep_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/14  vtw     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "bare_metal_info.h"

/*===========================================================================

                      TYPES AND STRUCTURES

===========================================================================*/

/**
 * Register fields to configure for sleep.
 */
typedef struct sleep_register
{
  pm_register_data_type       slaveID ;
  pm_register_address_type    regAddr ;
  pm_register_data_type       data;
} pm_sleep_reg_type;

/**
 *  Structure for sleep configuration.
 */
typedef struct
{
  const pm_sleep_reg_type* p_cfg; /* Can be NULL. */
} pm_sleep_cfg_type;


