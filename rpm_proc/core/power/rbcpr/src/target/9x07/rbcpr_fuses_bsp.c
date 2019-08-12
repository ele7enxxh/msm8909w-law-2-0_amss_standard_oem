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
            .number_of_fuses = 5,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS)
                    },
                },
				
				// RBCPR_FUSE_SVS_PLUS
                {
                    .fuse_type = RBCPR_FUSE_SVS_PLUS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING( QFPROM_CORR_CALIB_ROW5_MSB, CPR1_TARG_VOLT_SVSPLUS)
                    },
                },

                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_NOM)
                    },
                },

				// RBCPR_FUSE_NOMINAL_PLUS
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_NOMPLUS)
                    },
                },
				
                // RBCPR_FUSE_TURBO
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_TUR)
                    },
                },
            },
        },

        // VDDCX
        {   
            .rail_name = "vddcx",
            .number_of_fuses = 5,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {

                // RBCPR_FUSE_SVS
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS)
                    },
                },
				
				// RBCPR_FUSE_SVS_PLUS
                {
                    .fuse_type = RBCPR_FUSE_SVS_PLUS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS)
                    },
                },
				
                // RBCPR_FUSE_NOMINAL
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_NOM)
                    },
                },
				
				// RBCPR_FUSE_NOMINAL_PLUS
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_NOM)
                    },
                },

                // RBCPR_FUSE_TURBO
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                    {
                        RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_TUR)
                    },
                },
            },
        },
       
    },
    .number_of_fused_rails = 2,
};
