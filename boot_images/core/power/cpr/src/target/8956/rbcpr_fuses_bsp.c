/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rbcpr_fuses.h"
#include "rbcpr_qfprom.h"
#include "HALhwio.h"

//TBD to get the fuses mapping 
const rbcpr_bsp_fuse_config_t rbcpr_bsp_fuse_config =
{
    .rail_fuse_config = (rbcpr_bsp_fuse_rail_config_t[])
    {        
        {   //Vdd_Cx
            .rail_name = "vddcx",
            .number_of_fuses = 3,
            .individual_fuse_configs = (rbcpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = RBCPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                        {
                            RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_MSB, CPR0_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = RBCPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                        {
                            RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_MSB, CPR0_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = RBCPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const rbpcr_bsp_part_individual_fuse_config_t[])
                        {
                            RBCPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_TUR)
                        },
                },
            },
        },
    },
    .number_of_fused_rails = 1,
};
