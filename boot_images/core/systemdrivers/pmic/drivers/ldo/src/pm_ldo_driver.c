/*! \file pm_ldo_driver.c
*  \n
*  \brief This file contains LDO peripheral driver initialization during which the driver
*         driver data is stored.
*  \n   
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/pkg/bootloaders/dev/akmani.BOOT.BF.2.3.Build03_8994_Code_optimization/boot_images/core/systemdrivers/pmic/drivers/ldo/src/pm_ldo_driver.c#1

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ldo_driver.h"
#include "CoreVerify.h"
#include "pm_target_information.h"
#include "pm_comm.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the LDO driver data */
static pm_ldo_data_type *pm_ldo_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

static pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info);
static pm_pwr_range_info_type* pm_ldo_get_range_setting_info(peripheral_info_type *peripheral_info);

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/

void pm_ldo_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_ldo_data_type *ldo_ptr = NULL;
    uint32 ldo_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;
    uint32 prop_id_arr[] = {PM_PROP_LDOA_NUM, PM_PROP_LDOB_NUM};

    ldo_ptr = pm_ldo_data_arr[pmic_index];

    if (ldo_ptr == NULL)
    {
        pm_malloc( sizeof(pm_ldo_data_type), (void**)&ldo_ptr); 

                                                    
        /* Assign Comm ptr */
        ldo_ptr->comm_ptr = comm_ptr;

        /* LDO Register Info - Obtaining Data through dal config */
        ldo_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_LDO_REG);

        CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_reg_table);

        /* LDO Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        ldo_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index],pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(ldo_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* LDO pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(ldo_ptr->pm_pwr_data.num_of_peripherals), (void**)&(ldo_ptr->pm_pwr_data.pwr_specific_info));

        /* Save first LDO peripheral's base address */
        ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

        /* LDO Subtype pointer malloc */
        pm_malloc(sizeof(uint8)*(ldo_ptr->pm_pwr_data.num_of_peripherals), (void**)&(ldo_ptr->periph_subtype));
        
        pm_ldo_data_arr[pmic_index] = ldo_ptr;
        
        
    }
    
    if (ldo_ptr != NULL)
    {
        base_address = ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        periph_offset = ldo_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Peripheral Baseaddress should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        /* Calculate LDO peripheral index */
        ldo_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Peripheral Index should be less than max number of peripherals */
        CORE_VERIFY(ldo_index < ldo_ptr->pm_pwr_data.num_of_peripherals);

        /* Save LDO's Peripheral Subtype value */
        ldo_ptr->periph_subtype[ldo_index] = peripheral_info->peripheral_subtype;

        /* Peripheral Index should be less than number of peripherals */
        CORE_VERIFY(ldo_index < (ldo_ptr->pm_pwr_data.num_of_peripherals));

        /* Save LDO's Peripheral Type value */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type = peripheral_info->peripheral_type;

        /* Save each LDO peripheral's base address */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_base_address = peripheral_info->base_address;

        /* LDO Vset Info - Obtaining Data through dal config */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset = pm_ldo_get_volt_setting_info(peripheral_info);

        CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset);

        /* LDO Range Info - Obtaining Data through dal config */
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range = pm_ldo_get_range_setting_info(peripheral_info);

        CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_range);
    }
}

pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_volt_info_type *volt_info = NULL;

  switch ((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_LDO_N50:
    case PM_HW_MODULE_LDO_N150:
    case PM_HW_MODULE_LDO_N300:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_NMOS_VOLT);
      break;
    case PM_HW_MODULE_LDO_N600:
    case PM_HW_MODULE_LDO_N600_STEPPER:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N600_VOLT);
      break;
    case PM_HW_MODULE_LDO_N1200:
    case PM_HW_MODULE_LDO_N1200_STEPPER:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_N1200_VOLT);
      break;
    case PM_HW_MODULE_LDO_P50:
    case PM_HW_MODULE_LDO_P150:
    case PM_HW_MODULE_LDO_P300:
    case PM_HW_MODULE_LDO_P600:
    case PM_HW_MODULE_LDO_P1200:
    case PM_HW_MODULE_LDO_LV_P50:
    case PM_HW_MODULE_LDO_LV_P150:
    case PM_HW_MODULE_LDO_LV_P300:
    case PM_HW_MODULE_LDO_LV_P600:
    case PM_HW_MODULE_LDO_LV_P1200:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_PMOS_VOLT);
      break;
    case PM_HW_MODULE_LDO_USB_LDO:
    case PM_HW_MODULE_LDO_LN_LDO:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_LN_VOLT);
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_ULT_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_ULT_LDO_N50:
    case PM_HW_MODULE_ULT_LDO_N150:
    case PM_HW_MODULE_ULT_LDO_N300:
    case PM_HW_MODULE_ULT_LDO_N300_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N600:
    case PM_HW_MODULE_ULT_LDO_N600_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N900:
    case PM_HW_MODULE_ULT_LDO_N900_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N1200:
    case PM_HW_MODULE_ULT_LDO_N1200_STEPPER:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_NMOS_VOLT);
      break;
    case PM_HW_MODULE_ULT_LDO_P50:
    case PM_HW_MODULE_ULT_LDO_P150:
    case PM_HW_MODULE_ULT_LDO_P300:
    case PM_HW_MODULE_ULT_LDO_P450:
    case PM_HW_MODULE_ULT_LDO_P600:
    case PM_HW_MODULE_ULT_LDO_P1200:
    case PM_HW_MODULE_ULT_LDO_LV_P50:
    case PM_HW_MODULE_ULT_LDO_LV_P150:
    case PM_HW_MODULE_ULT_LDO_LV_P300:
    case PM_HW_MODULE_ULT_LDO_LV_P600:
    case PM_HW_MODULE_ULT_LDO_LV_P1200:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_PMOS_VOLT);
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  default:
    volt_info = NULL;
    break;
  }

  return volt_info;
}

pm_pwr_range_info_type* pm_ldo_get_range_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_range_info_type *range_info = NULL;

  switch ((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_LDO_N50:
    case PM_HW_MODULE_LDO_N150:
    case PM_HW_MODULE_LDO_N300:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_NMOS_RANGE);
      break;
    case PM_HW_MODULE_LDO_N600:
    case PM_HW_MODULE_LDO_N600_STEPPER:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_N600_RANGE);
      break;
    case PM_HW_MODULE_LDO_N1200:
    case PM_HW_MODULE_LDO_N1200_STEPPER:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_N1200_RANGE);
      break;
    case PM_HW_MODULE_LDO_P50:
    case PM_HW_MODULE_LDO_P150:
    case PM_HW_MODULE_LDO_P300:
    case PM_HW_MODULE_LDO_P600:
    case PM_HW_MODULE_LDO_P1200:
    case PM_HW_MODULE_LDO_LV_P50:
    case PM_HW_MODULE_LDO_LV_P150:
    case PM_HW_MODULE_LDO_LV_P300:
    case PM_HW_MODULE_LDO_LV_P600:
    case PM_HW_MODULE_LDO_LV_P1200:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_PMOS_RANGE);
      break;
    case PM_HW_MODULE_LDO_USB_LDO:
    case PM_HW_MODULE_LDO_LN_LDO:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_LN_RANGE);
      break;
    default:
      range_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_ULT_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_ULT_LDO_N50:
    case PM_HW_MODULE_ULT_LDO_N150:
    case PM_HW_MODULE_ULT_LDO_N300:
    case PM_HW_MODULE_ULT_LDO_N300_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N600:
    case PM_HW_MODULE_ULT_LDO_N600_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N900:
    case PM_HW_MODULE_ULT_LDO_N900_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N1200:
    case PM_HW_MODULE_ULT_LDO_N1200_STEPPER:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_NMOS_RANGE);
      break;
    case PM_HW_MODULE_ULT_LDO_P50:
    case PM_HW_MODULE_ULT_LDO_P150:
    case PM_HW_MODULE_ULT_LDO_P300:
    case PM_HW_MODULE_ULT_LDO_P450:
    case PM_HW_MODULE_ULT_LDO_P600:
    case PM_HW_MODULE_ULT_LDO_P1200:
    case PM_HW_MODULE_ULT_LDO_LV_P50:
    case PM_HW_MODULE_ULT_LDO_LV_P150:
    case PM_HW_MODULE_ULT_LDO_LV_P300:
    case PM_HW_MODULE_ULT_LDO_LV_P600:
    case PM_HW_MODULE_ULT_LDO_LV_P1200:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_PMOS_RANGE);
      break;
    default:
      range_info = NULL;
      break;
    }
    break;
  default:
    range_info = NULL;
    break;
  }

  return range_info;
}

pm_ldo_data_type* pm_ldo_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_ldo_data_arr[pmic_index];
  }

  return NULL;
}


uint8 pm_ldo_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_ldo_data_arr[pmic_index] !=NULL))
  {
      return pm_ldo_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return NULL;
}



