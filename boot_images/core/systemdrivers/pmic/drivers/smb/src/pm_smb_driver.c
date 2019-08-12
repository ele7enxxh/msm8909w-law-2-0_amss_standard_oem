
#include "pm_smb.h"
#include "pm_smb_driver.h"
#include "pm_utils.h"
#include "pm_target_information.h"
#include "CoreVerify.h"

static pm_smb_info_type* smb_info = NULL; 

int smb1360_icl_list[] =
{
    300, 400, 450, 500, 600, 700, 800, 850, 900, 950, 
         1000, 1100, 1200, 1300, 1400, 1500
};

int smb1360_ibat_list[] =
{
    450, 600, 750, 900, 1050, 1200, 1350, 1500
};
pm_smb_feature_list_type smb1360 =
{
    // nv access
    {0x40, BIT(6)},

    // aicl
    {0x06, BIT(0)},

    // charger enable
    {0x07, BIT(7), // enable by pin or register.
                   // If by default enabled by pin and want to change to register
                   // Make sure configure register to enable at first,
                   // And then transfer to "controlled by register"
                   // Otherwise we'll suffer momentarily charging disable
     0x07, BIT(6), // 1: active high, 0: active low
     0x07, BIT(0),
     0x42, BIT(1)}, // charge enable

    // mode_control
    {0x05, BIT(7), // usb500/usb100/ac mode determined by pin or register
     0x05, BIT(5), // reset  state 00b is USB500 mode
     0x41, BIT(1), // usb ac mode
     0x41, BIT(0)}, // 41H[0] == 05H[5]-->USB100, otherwise USB500

    // icl
    {0x05, 3, 0}, // ICL setting

    // ibat
    {0x13, 4, 2}, // IBAT setting

    //
    smb1360_icl_list, sizeof(smb1360_icl_list) / sizeof(int)/*16*/,

    smb1360_ibat_list, sizeof(smb1360_ibat_list) / sizeof(int)/*8*/
};
/*
pm_smb_feature_list_type smb358 =
{
    // Currently copied from smb1360 only. Will check later
    // nv access
    {0x40, BIT(6)},

    // aicl
    {0x06, BIT(0)},

    // charger enable
    {0x07, BIT(7), // enable by pin or register.
                   // If by default enabled by pin and want to change to register
                   // Make sure configure register to enable at first,
                   // And then transfer to "controlled by register"
                   // Otherwise we'll suffer momentarily charging disable
     0x07, BIT(6), // 1: active high, 0: active low
     0x07, BIT(0),
     0x42, BIT(1)}, // charge enable

    // mode_control
    {0x05, BIT(7), // usb500/usb100/ac mode determined by pin or register
     0x05, BIT(5), // reset  state 00b is USB500 mode
     0x41, BIT(1), // usb ac mode
     0x41, BIT(0)}, // 41H[0] == 05H[5]-->USB100, otherwise USB500

    // icl
    {0x05, 3, 0}, // ICL setting

    // ibat
    {0x13, 4, 2}, // IBAT setting

    smb1360_icl_list, sizeof(smb1360_icl_list) / sizeof(int),

    smb1360_ibat_list, sizeof(smb1360_ibat_list) / sizeof(int)
};
*/
pm_smb_feature_list_type* pm_smb_chip_array[] = 
{
        &smb1360,
//        &smb358,
};

pm_smb_feature_list_type* pm_smb_get_feature_list(pm_smb_index_type index)
{
    if(index >= SMB_INDEX_INVALID || NULL == pm_smb_chip_array)
    {
        return NULL;
    }
    else
    {
        return pm_smb_chip_array[index];
    }
}

void pm_smb_driver_init()
{
    pm_smb_hw_info_type* info = NULL;

    if(NULL == smb_info)
    {
        info = (pm_smb_hw_info_type*)pm_target_information_get_specific_info((uint32)PM_PROP_SMB_HW_INFO);
        if(NULL != info)
        {
            pm_malloc( sizeof(pm_smb_info_type), (void*)&smb_info);
            CORE_VERIFY(smb_info);
        }
        else
        {
            return;
        }

        smb_info->hw_info = info;
        smb_info->feature_list = pm_smb_get_feature_list(info->index);
    }
}

pm_smb_info_type* pm_smb_get_info()
{
    return smb_info;
}


