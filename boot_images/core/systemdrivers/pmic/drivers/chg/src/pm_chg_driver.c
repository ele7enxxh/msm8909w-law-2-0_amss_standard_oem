#include "pm_chg_common.h"
#include "hw_module_type.h"
#include "pm_lbc_driver.h"
#include "pm_smbb_driver.h"
#include "pm_chg_driver.h"

/* Static global variable to store the chg driver data */
static pm_chg_data_type *pm_chg_data_arr[PM_MAX_NUM_PMICS];

void
pm_chg_driver_init(
    pm_comm_info_type *comm_ptr,
    peripheral_info_type * peripheral_info,
    uint8 pmic_index
)
{
    pm_chg_data_type *chg_ptr = NULL;

    chg_ptr = pm_chg_data_arr[pmic_index];

    if(NULL == chg_ptr)
    {
        pm_malloc( sizeof(pm_chg_data_type), (void**)&chg_ptr);
        chg_ptr->comm_ptr = comm_ptr;
        if(PM_HW_MODULE_CHARGER_LBC_CHG == peripheral_info->peripheral_subtype)
        {
            chg_ptr->chg_sub_type = PM_CHG_LBC;
            pm_lbc_driver_init(comm_ptr, pmic_index);
        }
        else if(PM_HW_MODULE_CHARGER_SMBC_CHG == peripheral_info->peripheral_subtype)
        {
            chg_ptr->chg_sub_type = PM_CHG_SMBB;
            pm_smbb_driver_init(comm_ptr, pmic_index);
        }

        pm_chg_data_arr[pmic_index] = chg_ptr;
    }

}

pm_chg_type pm_chg_get_peripheral_sub_type(uint32 device_index)
{
    return pm_chg_data_arr[device_index] != NULL
        ? pm_chg_data_arr[device_index]->chg_sub_type
        : PM_CHG_INVAILD;
}


