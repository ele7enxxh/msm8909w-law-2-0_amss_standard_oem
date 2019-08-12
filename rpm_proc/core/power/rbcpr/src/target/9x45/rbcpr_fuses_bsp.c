/*
===========================================================================

FILE:         rbcpr_fuses_bsp.c

DESCRIPTION:
  Per target rbcpr bsp fuse configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rbcpr/src/target/9x45/rbcpr_fuses_bsp.c#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

//#include "comdef.h"
#include "msmhwio.h"
#include "HAL_rbcpr.h"
#include "HAL_rbcpr_hwio.h"
#include "rbcpr_fuses.h"
#include "rbcpr_qfprom.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

const rbcpr_bsp_fuse_config_t rbcpr_bsp_fuse_config =
{
    .rail_fuse_config = (rbcpr_bsp_fuse_rail_config_t[])
    {
        // VDDMX
        {
            .rail_name               = "vddmx",
            .number_of_fuses         = 2,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {
                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type                       = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR2_TARG_VOLT_NOM)
                    },
                },
                // RBCPR_FUSE_TURBO
                {
                    .fuse_type                       = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                       RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR2_TARG_VOLT_TUR)
                    },
                },
            },
        },

        // VDDCX
        {
            .rail_name               = "vddcx",
            .number_of_fuses         = 4,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS2
                {
                    .fuse_type                       = RBCPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS2)
                    },
                },
                // RBCPR_FUSE_SVS
                {
                    .fuse_type                       = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS)
                    },
                },
                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type                       = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_NOM)
                    },
                },
                // RBCPR_FUSE_TURBO
                {
                    .fuse_type                       = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_TUR)
                    },
                },
            },
        },

        // VDDMSS
        {
            .rail_name               = "vddmss",
            .number_of_fuses         = 4,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS2
                {
                    .fuse_type                       = RBCPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS2)
                    },
                },
                // RBCPR_FUSE_SVS
                {
                    .fuse_type                       = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS)
                    },
                },
                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type                       = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_NOM)
                    },
                },
                // RBCPR_FUSE_TURBO
                {
                    .fuse_type                       = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs  = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_TUR)
                    },
                },
            },
        },
    },
    .number_of_fused_rails = 3,
};

