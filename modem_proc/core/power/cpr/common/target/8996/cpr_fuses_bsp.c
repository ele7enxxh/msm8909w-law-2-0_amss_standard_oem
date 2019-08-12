/*===========================================================================

  Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.


  Fuses spread across multiple locations are always defined from MSB to LSBs. As the fuse reader
  code reads partial values, it shifts previous results left to make room for the new bits
  Example config showing the required order from MSBs to LSBs:
             CPR_FUSE_MAPPING(QFPROM__ROW1_LSB, CPRn_TARG_VOLT_OFFSET_SVS2),       //bit 7
             CPR_FUSE_MAPPING(QFPROM__ROW3_MSB, CPRn_TARG_VOLT_OFFSET_SVS2_BIT_1), //bit 6
             CPR_FUSE_MAPPING(QFPROM__ROW2_MSB, CPRn_TARG_VOLT_OFFSET_SVS2),       //bits 4-5
             CPR_FUSE_MAPPING(QFPROM__ROW2_MSB, CPRn_TARG_VOLT_OFFSET_SVS),        //bits 0-3

  ===========================================================================*/

#include "cpr_fuses.h"
#include "cpr_device_hw_version.h"
#include "cpr_qfprom.h"
#include "HALhwio.h"
#ifdef WINSIM
#define SECURITY_CONTROL_BASE 0
#endif

const cpr_bsp_fuse_versioned_config_t cpr_bsp_fuse_config_vALL =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            { CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
        },
        .foundry_range_count = 2,
    },
    .rail_fuse_config = (cpr_bsp_fuse_rail_config_t[])
    {
        {   //Vdd_Mx
            .rail_id = CPR_RAIL_MX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR4_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR4_TARG_VOLT_TUR)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SUTUR,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR4_TARG_VOLT_SUTUR)
                        },
                },
            },
        },
        {   //Vdd_Cx
            .rail_id = CPR_RAIL_CX,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_TUR)
                        },
                },
            },
        },
        {   //Vdd_Gfx
            .rail_id = CPR_RAIL_GFX,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR2_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR2_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR2_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR2_TARG_VOLT_TUR)
                        },
                },
            },
        },
        {   //Vdda_Ebi
            .rail_id = CPR_RAIL_VDDA_EBI,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR3_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR3_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR3_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR3_TARG_VOLT_TUR_4),
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR3_TARG_VOLT_TUR_3_0)
                        },
                },
            },
        },
        {   //Vdd_Mss
            .rail_id = CPR_RAIL_MSS,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR1_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR1_TARG_VOLT_SVS_4_2),
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR1_TARG_VOLT_SVS_1_0)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR1_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR1_TARG_VOLT_TUR)
                        },
                },
            },
        },
        {   //Vdd_APC0
            .rail_id = CPR_RAIL_APC0,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_LSB, CPR6_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_LSB, CPR6_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_LSB, CPR6_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW4_LSB, CPR6_TARG_VOLT_TUR_5_2),
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR6_TARG_VOLT_TUR_1_0)
                        },
                },
            },
        },
        {   //Vdd_APC1
            .rail_id = CPR_RAIL_APC1,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW6_LSB, CPR7_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW5_MSB, CPR7_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW5_MSB, CPR7_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW5_MSB, CPR7_TARG_VOLT_TUR)
                        },
                },
            },
        },
        {   //Vdd_SSC_Mx
            .rail_id = CPR_RAIL_SSC_MX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_NOM_4_1),
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR5_TARG_VOLT_NOM_0)
                        },
                },
            },
        },
        {   //Vdd_SSC_Cx
            .rail_id = CPR_RAIL_SSC_CX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 2,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR5_TARG_VOLT_NOM_4_1),
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_LSB, CPR5_TARG_VOLT_NOM_0)
                        },
                },
            },
        },
    },
    .number_of_fused_rails = 9,
};
 
const cpr_config_bsp_fuse_config_t cpr_bsp_fuse_config =
{
    .versioned_fuses = (const cpr_bsp_fuse_versioned_config_t*[])
    {
        &cpr_bsp_fuse_config_vALL,
    },
    .versioned_fuses_count = 1,
};

