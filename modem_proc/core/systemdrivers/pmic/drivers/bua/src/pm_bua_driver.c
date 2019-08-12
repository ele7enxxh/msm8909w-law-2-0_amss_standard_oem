/*! \file pm_bua_driver.c
*  \n
*  \brief BUA driver APIs implementation.
*  \n
*  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/bua/src/pm_bua_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/13   kt      Initial version
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "device_info.h"
#include "pm_bua_driver.h"


/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

/* BUA register offsets */
#define PMIO_BUA_STATUS1_ADDR                                      0x0008
#define PMIO_BUA_STATUS2_ADDR                                      0x0009
#define PMIO_BUA_INT_RT_STS_ADDR                                   0x0010
#define PMIO_BUA_INT_SET_TYPE_ADDR                                 0x0011
#define PMIO_BUA_INT_POLARITY_HIGH_ADDR                            0x0012
#define PMIO_BUA_INT_POLARITY_LOW_ADDR                             0x0013
#define PMIO_BUA_INT_LATCHED_CLR_ADDR                              0x0014
#define PMIO_BUA_INT_EN_SET_ADDR                                   0x0015
#define PMIO_BUA_INT_EN_CLR_ADDR                                   0x0016
#define PMIO_BUA_INT_LATCHED_STS_ADDR                              0x0018
#define PMIO_BUA_INT_PENDING_STS_ADDR                              0x0019
#define PMIO_BUA_EN_CTL_ADDR                                       0x0046

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the RTC data */
static pm_bua_data_type *pm_bua_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/* Internal BUA driver data init function */
static void pm_bua_driver_data_init(pm_bua_data_type *bua_ptr, peripheral_info_type *peripheral_info);

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_bua_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info,uint8 pmic_index)
{
    pm_bua_data_type *bua_ptr = NULL;

    CORE_VERIFY(pmic_index < PM_MAX_NUM_PMICS);

    if (peripheral_info->peripheral_type != PM_HW_MODULE_BUA ||
        peripheral_info->peripheral_subtype == PM_HW_MODULE_BUA_BATT_GONE)
    {
        return;
    }

    bua_ptr = pm_bua_data_arr[pmic_index];

    if (bua_ptr == NULL)
    {
        pm_malloc( sizeof(pm_bua_data_type), (void**)&bua_ptr);

        /* Assign Comm ptr */
        bua_ptr->comm_ptr = comm_ptr;
        pm_bua_data_arr[pmic_index] =  bua_ptr ;
        pm_bua_driver_data_init(bua_ptr, peripheral_info);
    }
}

void pm_bua_driver_data_init(pm_bua_data_type *bua_ptr, peripheral_info_type *peripheral_info)
{
    pm_bua_common_info_type *bua_common_info = NULL;
    pm_bua_specific_info_type *bua_specific_info = NULL;
    pm_bua_register_info_type *bua_reg_info = NULL;

    pm_malloc(sizeof(pm_bua_common_info_type), (void**)&bua_common_info);
    pm_malloc(sizeof(pm_bua_specific_info_type), (void**)&bua_specific_info);
    pm_malloc(sizeof(pm_bua_register_info_type), (void**)&bua_reg_info);

    /* BUA Common Info */
    bua_common_info->base_address = peripheral_info->base_address;
    bua_common_info->num_peripherals = 1;

    /* BUA Specific Info */
    switch ((pm_hw_module_bua_subtype)peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_BUA_2UICC:
    case PM_HW_MODULE_BUA_2UICC_NO_CHARGER:
        bua_specific_info->num_alarms_supported = 3;
        break;
    case PM_HW_MODULE_BUA_4UICC:
    case PM_HW_MODULE_BUA_4UICC_DUAL_BATT_ALARM:
        bua_specific_info->num_alarms_supported = 5;
        break;
    case PM_HW_MODULE_BUA_3UICC_EXT_CHARGER:
        bua_specific_info->num_alarms_supported = 4;
        break;
    default:
        bua_specific_info->num_alarms_supported = 0;
        break;
    }

    /* BUA Register Info */
    bua_reg_info->status1 = PMIO_BUA_STATUS1_ADDR;
    bua_reg_info->status2 = PMIO_BUA_STATUS2_ADDR;
    bua_reg_info->int_rt_status = PMIO_BUA_INT_RT_STS_ADDR;
    bua_reg_info->int_set_type = PMIO_BUA_INT_SET_TYPE_ADDR;
    bua_reg_info->int_polarity_high = PMIO_BUA_INT_POLARITY_HIGH_ADDR;
    bua_reg_info->int_polarity_low = PMIO_BUA_INT_POLARITY_LOW_ADDR;
    bua_reg_info->int_latched_clr = PMIO_BUA_INT_LATCHED_CLR_ADDR;
    bua_reg_info->int_en_set = PMIO_BUA_INT_EN_SET_ADDR;
    bua_reg_info->int_en_clr = PMIO_BUA_INT_EN_CLR_ADDR;
    bua_reg_info->int_latched_status = PMIO_BUA_INT_LATCHED_STS_ADDR;
    bua_reg_info->int_pending_status = PMIO_BUA_INT_PENDING_STS_ADDR;
    bua_reg_info->en_ctl1 = PMIO_BUA_EN_CTL_ADDR;

    /* Save the Info in resource */
    bua_ptr->bua_common = bua_common_info;
    bua_ptr->bua_specific = bua_specific_info;
    bua_ptr->bua_reg_table = bua_reg_info;
}

pm_bua_data_type* pm_bua_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_bua_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_bua_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) &&
      (pm_bua_data_arr[pmic_index] != NULL))
  {
      return pm_bua_data_arr[pmic_index]->bua_common->num_peripherals;
  }

  return NULL;
}

