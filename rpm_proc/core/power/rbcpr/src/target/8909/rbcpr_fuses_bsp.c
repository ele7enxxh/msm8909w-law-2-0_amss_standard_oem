/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rbcpr_fuses.h"
#include "rbcpr_qfprom.h"
#include "HALhwio.h"


const rbcpr_bsp_fuse_config_t rbcpr_bsp_fuse_config =
{
    .rail_fuse_config = (rbcpr_bsp_fuse_rail_config_t[])
    {
        // VDDMX
        {   
            .rail_name = "vddmx",
            .number_of_fuses = 3,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR2_SVS_TARGET)
                    },
                },

                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR2_NOMINAL_TARGET)
                    },
                },

                // RBCPR_FUSE_TURBO
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR2_TURBO_TARGET)
                    },
                },
            },
        },

        // VDDCX
        {   
            .rail_name = "vddcx",
            .number_of_fuses = 3,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_SVS_TARGET)
                    },
                },

                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_NOMINAL_TARGET)
                    },
                },

                // RBCPR_FUSE_TURBO
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TURBO_TARGET)
                    },
                },
            },
        },

        // VDDAPC0
        {   
            .rail_name = "vddapc0",
            .number_of_fuses = 3,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_MSB, CPR1_SVS_TARGET)
                    },
                },

                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_LSB, CPR1_NOMINAL_TARGET)
                    },
                },

                // RBCPR_FUSE_TURBO
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW0_LSB, CPR1_TURBO_TARGET)
                    },
                },
            },
        },
    },
    .number_of_fused_rails = 3,
};
