/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
  
#include "tcsr_hwio.h"
#include "acc.h"

extern uint32 railway_target_type;

const acc_target_config acc_config = 
{
    .config = (acc_control_config[])
    {
        {
        .rail_name = "vddcx",
        .acc_register_setting = (const int[])
            {
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_NO_REQUEST
                ACC_REGISTER_SVS_SETTING,       // RAILWAY_RETENTION
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_SVS_KRAIT
                ACC_REGISTER_SVS_SETTING,       // RAILWAY_SVS_SOC
				ACC_REGISTER_SVS_SETTING,       // RAILWAY_SVS_HIGH
                ACC_REGISTER_NOM_SETTING,       // RAILWAY_NOMINAL
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_TURBO
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_TURBO_HIGH
                ACC_REGISTER_NOM_SETTING,       // RAILWAY_SUPER_TURBO
                ACC_MODE_NOT_SUPPORTED,         // RAILWAY_SUPER_TURBO_NO_CPR
            },
        .acc_register_address = (volatile unsigned*) HWIO_TCSR_MEM_ACC_SEL_VDDCX_ADDR,
		.apc_acc_register_address = (volatile unsigned*) HWIO_TCSR_MEM_ACC_SEL_VDDAPC_ADDR,
		.merged_rail_config = 0,
        },
    },
    .number_of_acc_controls = 1,
};

const acc_target_config* ACC_TARGET_CONFIG = &acc_config;

void acc_target_init (int rail_num, acc_control_config* config)
{
    if((railway_target_type == RAILWAY_CX_APC_COMBINED) && (rail_num == 1))
	    config->merged_rail_config = 1;
}		
	    
	
	
