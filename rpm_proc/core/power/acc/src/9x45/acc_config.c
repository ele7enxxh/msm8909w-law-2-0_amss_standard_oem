/*
===========================================================================

FILE:         acc_config.c

DESCRIPTION:
  Per target ACC configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/acc/src/9x45/acc_config.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "tcsr_hwio.h"
#include "acc.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

const acc_target_config acc_config =
{
    .config = (const acc_control_config[])
    {
        {
            .rail_name = "vddcx",
            .acc_register_setting = (const int[])
            {
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_NO_REQUEST
                ACC_REGISTER_SVS_SETTING,       // RAILWAY_RETENTION
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_SVS_KRAIT
                ACC_REGISTER_SVS_SETTING,       // RAILWAY_SVS_SOC
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_SVS_HIGH
                ACC_REGISTER_NOM_SETTING,       // RAILWAY_NOMINAL
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_TURBO
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_TURBO_HIGH
                ACC_REGISTER_NOM_SETTING,       // RAILWAY_SUPER_TURBO
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_SUPER_TURBO_NO_CPR
            },
            .acc_register_address = (volatile unsigned *)HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR,
        },
    },
    .number_of_acc_controls = 1,
};

const acc_target_config *ACC_TARGET_CONFIG = &acc_config;

