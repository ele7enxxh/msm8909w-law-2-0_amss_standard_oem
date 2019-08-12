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
        {    //these will cover all chips for now. 
            { CPR_FOUNDRY_SS, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
            { CPR_FOUNDRY_TSMC, CPR_CHIPINFO_VERSION(0, 0), CPR_CHIPINFO_VERSION(0xFF, 0xFF) },
        },
        .foundry_range_count = 2,
    },
    .rail_fuse_config = (cpr_bsp_fuse_rail_config_t[])
    {
        {   //Vdd_Cx  (CX is CPR0 on 8976)
            .rail_id = CPR_RAIL_CX,
            .number_of_fuses = 3,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR0_SVS_TARGET)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR0_NOMINAL_TARGET)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR0_TURBO_TARGET)
                        },
                },
            },
        },
        {   //Vdd_Mss   (MSS is CPR1 on 8976)
            .rail_id = CPR_RAIL_MSS,
            .number_of_fuses = 4,
            .individual_fuse_configs = (cpr_bsp_individual_fuse_config_t[])
            {
                {
                    .fuse_type = CPR_FUSE_SVS2,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB12_ROW0_MSB, CPR1_SVS2_TARGET)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_SVS,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR1_SVS_TARGET)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_NOMINAL,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR1_NOMINAL_TARGET)
                        },
                },
                {
                    .fuse_type = CPR_FUSE_TURBO,
                    .number_of_partial_fuse_configs = 1,
                    .partial_individual_fuse_configs = (const cpr_bsp_part_individual_fuse_config_t[])
                        {
                            CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW7_LSB, CPR1_TURBO_TARGET)
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
        &cpr_bsp_fuse_config_vALL,
    },
    .versioned_fuses_count = 1,
};

