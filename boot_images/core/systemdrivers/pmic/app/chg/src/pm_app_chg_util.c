/*! \file
*  
*  \brief  pm_app_chg_util.c
*  \details Implementation file for rgb led resourece type.
*    
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/14   pxm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_app_chg.h"
#include "pm_smbb.h"
#include "pm_lbc.h"
#include "pm_lbc_driver.h"
#include "pm_smbb_driver.h"
#include "pm_chg_common.h"
#include "hw_module_type.h"

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_chg_is_charger_present(boolean  *charger_present)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();
    boolean is_charger_present = FALSE;
    pm_chg_attached_chgr_type charger_type;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_charger_present(device_index, PM_LBC_1, &is_charger_present);
        if(err_flag == PM_ERR_FLAG__SUCCESS)
        {
            *charger_present = is_charger_present;
        }
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag |= pm_smbb_chg_get_attached_charger(device_index, PM_SMBB_1, &charger_type);
        if(err_flag == PM_ERR_FLAG__SUCCESS)
        {
            *charger_present = (charger_type == PM_CHG_ATTACHED_CHGR__USB)
                || (charger_type == PM_CHG_ATTACHED_CHGR__BOTH);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_chg_is_battery_present(boolean  *battery_present)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean is_battery_present = FALSE;
    boolean is_battery_therm = FALSE;
    boolean is_battery_id_present = FALSE;
    uint8 device_index = pm_cfg_device_index();
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_battery_present(device_index, PM_LBC_1, &is_battery_present, &is_battery_id_present);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_battery_present(device_index, PM_SMBB_1, &is_battery_present, &is_battery_therm, &is_battery_id_present);
    }

    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
        *battery_present = is_battery_present;  
    }
    return err_flag;
}

pm_err_flag_type  pm_chg_is_debug_board_present(boolean  *debug_board_present)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean is_battery_present = FALSE;
    boolean is_battery_therm = FALSE;
    boolean is_battery_id_present = FALSE;
    uint8 device_index = pm_cfg_device_index();
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_battery_present(device_index, PM_LBC_1, &is_battery_present, &is_battery_id_present);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_battery_present(device_index, PM_SMBB_1, &is_battery_present, &is_battery_therm, &is_battery_id_present);
    }

    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
        if(is_battery_id_present == FALSE)
        {
            *debug_board_present = TRUE; 
        }
        else
        {
            *debug_board_present = FALSE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_chg_get_battery_volt_level(uint32* battery_voltage)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    AdcBootDeviceChannelType channel;
    AdcBootResultType adc_result; 
    uint32 error = 0x0; 
    
    error = AdcBoot_Init();   //Initialize the ADC
    if(0 != error)
    {
        err_flag |= PM_ERR_FLAG__ADC_VBATT_READ_ERR;
    }
    else
    {
        /*Get the channel from where the data is needed*/
        error = AdcBoot_GetChannel(ADC_INPUT_VBATT, &channel);
        if(0 != error)
        {
            err_flag |= PM_ERR_FLAG__ADC_VBATT_READ_ERR;
        }
        else
        {
            /*Read the Voltage of the Battery*/
            error = AdcBoot_Read(&channel, &adc_result);
            if(0 != error)
            {
                err_flag |= PM_ERR_FLAG__ADC_VBATT_READ_ERR;
            }
            
            /*Check for the result*/
            if(0 != adc_result.eStatus)
            {
                *battery_voltage = (uint32)(adc_result.nMicrovolts/1000); 
            }
            else
            {
                err_flag |= PM_ERR_FLAG__ADC_VBATT_READ_ERR;
            }
        }
    }

    return err_flag;
}

pm_err_flag_type pm_chg_set_battery_threshold(
   uint32                 device_index,
   uint32                 dead_battery_threshold,
   uint32                 weak_battery_threshold
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_battery_threshold(device_index, PM_LBC_1, dead_battery_threshold, weak_battery_threshold);;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_battery_threshold(device_index, PM_SMBB_1, dead_battery_threshold, weak_battery_threshold);
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_get_weak_battery_status
(
   uint32                 device_index,
   boolean                *weak
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_weak_battery_status(device_index, PM_SMBB_1, weak);
    }

    return err_flag;
}

pm_err_flag_type pm_chg_set_ibat_max(uint32 device_index, uint32 value_ma)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_ibat_max(device_index, PM_LBC_1, value_ma);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_ibat_max(device_index, PM_SMBB_1, value_ma);
    }

    return err_flag;
}

pm_err_flag_type pm_chg_set_ibat_safe(uint32 device_index, uint32 value_ma)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_ibat_safe(device_index, PM_LBC_1, value_ma);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        // TODO: B family have this feature. But till now the build script is not ready.
        // Leave the implement after the build script is ready.
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type pm_chg_set_vin_min(uint32 device_index, uint32 value_mv)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_vin_min(device_index, PM_LBC_1, value_mv);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        // TODO: B family have this feature. But till now the build script is not ready.
        // Leave the implement after the build script is ready.
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}


pm_err_flag_type 
pm_chg_set_itrkl
(
   uint32                 device_index,
   uint32                 value_ma
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_itrkl(device_index, PM_SMBB_1, value_ma);
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_set_mode
(
   uint32            device_index,
   pm_chg_mode_type  mode
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_mode(device_index, PM_LBC_1, mode);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_mode(device_index, PM_SMBB_1, mode);
    }

    return err_flag;
}

pm_err_flag_type 
pm_disable_chg
(
   uint32               device_index
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_disable_chg(device_index, PM_LBC_1);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_disable_chg(device_index, PM_SMBB_1);
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_get_battery_present
(
   uint32               device_index,
   boolean             *batt_present, 
   boolean             *batt_id
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean is_battery_present = FALSE;
    boolean is_battery_therm = FALSE;
    boolean is_battery_id_present = FALSE;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_battery_present(device_index, PM_LBC_1, &is_battery_present, &is_battery_id_present);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_battery_present(device_index, PM_SMBB_1, &is_battery_present, &is_battery_therm, &is_battery_id_present);
    }

    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
        *batt_present = is_battery_present;
        *batt_id = is_battery_id_present;
    }
    return err_flag;
}

pm_err_flag_type 
pm_chg_set_iusb_max
(
   uint32                device_index,
   uint32                value_ma
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_iusb_max(device_index, PM_SMBB_1, value_ma);
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_get_attached_charger
(
   uint32                     device_index,
   pm_chg_attached_chgr_type *type
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_attached_charger(device_index, PM_SMBB_1, type);
    }

    return err_flag;
}

pm_err_flag_type
pm_chg_get_chg_gone_rt_status
(
   uint32                          device_index,
   boolean                         *chg_gone_rt_status 
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_get_chg_gone_rt_status(device_index, PM_SMBB_1, chg_gone_rt_status);
    }

    return err_flag;
}

pm_err_flag_type
pm_chg_set_comp_override
(
   uint32                          device_index,
   boolean                         comp_override_status 
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_comp_override(device_index, PM_LBC_1, comp_override_status);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = pm_smbb_chg_set_comp_override(device_index, PM_SMBB_1, comp_override_status);
    }

    return err_flag;
}

pm_err_flag_type
pm_chg_get_chg_option
(
	uint32            device_index,
	boolean           *internal_chager
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_chg_option(device_index, PM_LBC_1, internal_chager);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_set_ibat_atc_b
(
   uint32 device_index,
   uint32 value_ma
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_ibat_atc_b(device_index, PM_LBC_1, value_ma);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_wdog_pet
(
   uint32 device_index
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_wdog_pet(device_index, PM_LBC_1);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_wdog_enable
(
   uint32                 device_index,
   boolean                enable
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_wdog_enable(device_index, PM_LBC_1, enable);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type
pm_chg_get_enum_timer_expired
(
    uint32               device_index,
    boolean             *enum_expired
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_enum_timer_expired(device_index, PM_LBC_1, enum_expired);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_get_enum_timer_status
(
   uint32                 device_index,
   boolean                *run
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_get_enum_timer_staus(device_index, PM_LBC_1, run);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_stop_enum_timer
(
   uint32                 device_index
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_stop_enum_timer(device_index, PM_LBC_1);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type 
pm_chg_set_boot_done
(
   uint32                 device_index
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_type chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    if(PM_CHG_LBC == chg_chip_type)
    {
        err_flag = pm_lbc_chg_set_boot_done(device_index, PM_LBC_1);
    }
    else if(PM_CHG_SMBB == chg_chip_type)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

