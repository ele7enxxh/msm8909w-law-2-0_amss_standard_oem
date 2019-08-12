/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "railway.h"
#include "acc.h"
#include "CoreVerify.h"
#include <string.h>
#include "DALSys.h"

extern const unsigned acc_register_settings[];

static void acc_voltage_change_pre_callback(const railway_settings* settings, void* cookie)
{
    CORE_VERIFY_PTR(cookie);

    acc_setting_control* setting_control = (acc_setting_control*)cookie;
    const acc_control_config* setting_config = setting_control->config;

    const int target_acc_setting = setting_config->acc_register_setting[settings->mode];
    
    CORE_VERIFY(target_acc_setting!=ACC_MODE_NOT_SUPPORTED);

    if(target_acc_setting<setting_control->current_acc_setting)
    {
        //If setting decreases then need to walk through settings from current setting to target
        for(int i = setting_control->current_acc_setting-1; i>=target_acc_setting; i--)
        {
            *(setting_config->acc_register_address) = acc_register_settings[i];
        }
        setting_control->current_acc_setting = (acc_register_state)target_acc_setting;
    }
}

static void acc_voltage_change_post_callback(const railway_settings* settings, void* cookie)
{
    CORE_VERIFY_PTR(cookie);

    acc_setting_control* setting_control = (acc_setting_control*)cookie;
    const acc_control_config* setting_config = setting_control->config;

    const int target_acc_setting = setting_config->acc_register_setting[settings->mode];
    
    CORE_VERIFY(target_acc_setting!=ACC_MODE_NOT_SUPPORTED);

    if(target_acc_setting>setting_control->current_acc_setting)
    {
        //If setting decreases then need to walk through settings from current setting to target
        for(int i = setting_control->current_acc_setting+1; i<=target_acc_setting; i++)
        {
            *(setting_config->acc_register_address) = acc_register_settings[i]; //Actually do the HWIO write
        }
        setting_control->current_acc_setting = (acc_register_state)target_acc_setting;
    }
}

void acc_init(void)
{
    for(int i=0; i<ACC_TARGET_CONFIG->number_of_acc_controls; i++)
    {
        const int rail_num = rail_id(ACC_TARGET_CONFIG->config[i].rail_name);
        CORE_VERIFY(rail_num!=RAIL_NOT_SUPPORTED_BY_RAILWAY);
  
        acc_setting_control* setting_control;

        DALResult dalResult = DALSYS_Malloc(sizeof(acc_setting_control), (void**)&setting_control);
        CORE_VERIFY_PTR(setting_control);
        CORE_VERIFY(dalResult==DAL_SUCCESS);

        //HW defaults to the NOM setting.
        setting_control->current_acc_setting = ACC_REGISTER_NOM_SETTING;
        setting_control->config = &ACC_TARGET_CONFIG->config[i];
        
        railway_settings rail_settings;
        railway_get_current_settings(rail_num, &rail_settings);
     
        railway_set_callback(rail_num, RAILWAY_LAST_PRECHANGE_CB, acc_voltage_change_pre_callback, setting_control);
        railway_set_callback(rail_num, RAILWAY_FIRST_POSTCHANGE_CB, acc_voltage_change_post_callback, setting_control);
    }
}
