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
#include "SecurityControlV2HWIO.h"
#ifdef WINSIM
#define CPR_FUSE_MAPPING(x,y) (0)
#else
#include "HALhwio.h"
#endif

// -----------------------------------------------------------------
// 9x45 V1 Fuses 
// -----------------------------------------------------------------
const cpr_bsp_fuse_versioned_config_t cpr_bsp_fuse_config_v1 =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0x1, 0xFF),
        },
        .foundry_range_count = 1,
    },
    .rail_fuse_config = (cpr_bsp_fuse_rail_config_t[])
    {
        {   //Vdd_Mss
            .rail_id = CPR_RAIL_MSS,
            .number_of_fuses = 5,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_NOM)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB, CPR1_TARG_VOLT_TUR)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_AGING,
                    .number_of_partial_fuse_configs = 4,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                         {
                             CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR1_TARG_VOLT_OFFSET_SVS2),       /*bit 7*/
                             CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB, CPR1_TARG_VOLT_OFFSET_SVS2_BIT_1), /*bit 6*/
                             CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR1_TARG_VOLT_OFFSET_SVS2),       /*bits 4-5*/
                             CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB, CPR1_TARG_VOLT_OFFSET_SVS),        /*bits 0-3*/
                         },
                },
            },
        },
        {   //Vdd_CX  (only putting in a few corners for the MPSS eLDO voltage calculations
            .rail_id = CPR_RAIL_CX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS2)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_SVS)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB, CPR0_TARG_VOLT_NOM)
                        },
                },
            },
        },
    },
    .number_of_fused_rails = 2,

};


// -----------------------------------------------------------------
// 9x45 V2 Fuses 
// -----------------------------------------------------------------
const cpr_bsp_fuse_versioned_config_t cpr_bsp_fuse_config_v2 =
{
    .hw_versions =
    {
        .foundry_range = (const cpr_config_foundry_range[])
        {
            CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(1, 0xFF), CPR_CHIPINFO_VERSION(0xFF, 0xFF),
        },
        .foundry_range_count = 1,
    },
    .rail_fuse_config = (cpr_bsp_fuse_rail_config_t[])
            {
                {   //Vdd_Mss
                    .rail_id = CPR_RAIL_MSS,
                    .number_of_fuses = 5,
                    .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
                    {
                        {
                            .fuse_type = CPR_FUSE_SVS2,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB_V2, CPR1_TARG_VOLT_SVS2)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_SVS,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB_V2, CPR1_TARG_VOLT_SVS)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_NOMINAL,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB_V2, CPR1_TARG_VOLT_NOM)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_TURBO,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_MSB_V2, CPR1_TARG_VOLT_TUR)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_AGING,
                            .number_of_partial_fuse_configs = 4,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                             {
                                 CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB_V2, CPR1_TARG_VOLT_OFFSET_SVS2),
                                 CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW3_MSB_V2, CPR1_TARG_VOLT_OFFSET_SVS2_BIT_3),
                                 CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB_V2, CPR1_TARG_VOLT_OFFSET_SVS2),
                                 CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_MSB_V2, CPR1_TARG_VOLT_OFFSET_SVS),
                             },
                        },
                    },
                },
                {   //Vdd_CX  (only putting in a few corners for the MPSS eLDO voltage calculations
                    .rail_id = CPR_RAIL_CX,
                    .number_of_fuses = 3,
                    .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
                    {
                        {
                            .fuse_type = CPR_FUSE_SVS2,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB_V2, CPR0_TARG_VOLT_SVS2)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_SVS,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB_V2, CPR0_TARG_VOLT_SVS)
                            },
                        },
                        {
                            .fuse_type = CPR_FUSE_NOMINAL,
                            .number_of_partial_fuse_configs = 1,
                            .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                            {
                                CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW1_LSB_V2, CPR0_TARG_VOLT_NOM)
                            },
                        },
                    },
                },
    },
    .number_of_fused_rails = 2,
};

const cpr_config_bsp_fuse_config_t cpr_bsp_fuse_config =
{
    .versioned_fuses = (const cpr_bsp_fuse_versioned_config_t*[])
    {
        &cpr_bsp_fuse_config_v1,
        &cpr_bsp_fuse_config_v2,
    },
    .versioned_fuses_count = 2,
};

