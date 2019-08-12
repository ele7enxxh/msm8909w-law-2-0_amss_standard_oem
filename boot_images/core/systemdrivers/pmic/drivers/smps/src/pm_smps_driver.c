/*! \file pm_smps_driver.c
*  \n
*  \brief This file contains SMPS peripheral driver initialization during which the driver
*         driver data is stored. 
*  \n   
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smps/src/pm_smps_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smps_driver.h"
#include "CoreVerify.h"
#include "pm_version.h"
#include "pm_comm.h"


/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the SMPS driver data */
static pm_smps_data_type *pm_smps_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

static pm_pwr_volt_info_type* pm_smps_get_volt_setting_info(peripheral_info_type *peripheral_info);
static pm_pwr_range_info_type* pm_smps_get_range_setting_info(peripheral_info_type *peripheral_info);

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/


void pm_smps_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_smps_data_type *smps_ptr = NULL;
    uint16 smps_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_device_info_type pmic_info;

    uint32 prop_id_arr[] = {PM_PROP_SMPSA_NUM, PM_PROP_SMPSB_NUM};

    /* Initializing PMIC info */
    pmic_info.ePmicModel = PMIC_IS_UNKNOWN;
    pmic_info.nPmicAllLayerRevision = 0;
    pmic_info.nPmicMetalRevision = 0;

    // Initialize the driver for CTL peripheral subtypes only and return in other cases
    switch(peripheral_info->peripheral_type)
    {
    case PM_HW_MODULE_FTS:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_FTS__FTS_CTL:
        case PM_HW_MODULE_FTS__FTS2p5_CTL:
           break;
        default:
           return;
        }
        break;
    case PM_HW_MODULE_HF_BUCK:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_HF_BUCK_GP_CTL:
        case PM_HW_MODULE_HF_BUCK_RF_CTL:
           break;
        default:
           return;
        }
        break;
    case PM_HW_MODULE_ULT_BUCK:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_ULT_BUCK_CTL_LV:
        case PM_HW_MODULE_ULT_BUCK_CTL2:
        case PM_HW_MODULE_ULT_BUCK_CTL3:
        case PM_HW_MODULE_ULT_BUCK_CTL_MV:
           break;
        default:
           return;
        }
        break;
    default:
        return;
    }

    smps_ptr = pm_smps_data_arr[pmic_index];

    if (smps_ptr == NULL)
    {
        pm_malloc( sizeof(pm_smps_data_type), (void**)&smps_ptr);
                                                    
        /* Assign Comm ptr */
        smps_ptr->comm_ptr = comm_ptr;

        /* SMPS Register Info - Obtaining Data through dal config */
        smps_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_SMPS_REG);

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_reg_table);

        /* SMPS Num of peripherals - Obtaining Data through dal config */
        CORE_VERIFY(pmic_index < (sizeof(prop_id_arr)/sizeof(prop_id_arr[0])));

        smps_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(prop_id_arr[pmic_index], pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(smps_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* SMPS pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(smps_ptr->pm_pwr_data.num_of_peripherals), (void**)&(smps_ptr->pm_pwr_data.pwr_specific_info));

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info);

        /* Save first SMPS peripheral's base address */
        smps_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

        pm_smps_data_arr[pmic_index] = smps_ptr;
    }
    
    if (smps_ptr != NULL)
    {
        base_address = smps_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        periph_offset = smps_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Peripheral Baseaddress should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        /* Calculate SMPS peripheral index */
        smps_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Get PMIC rev info */
        err_flag = pm_get_pmic_info(pmic_index, &pmic_info);

        CORE_VERIFY(err_flag == PM_ERR_FLAG__SUCCESS);

        /* Temporary work-around for the wrong SMPS base address issue on PMD9635 v1.x PMICs */
        if ((pmic_info.ePmicModel == PMIC_IS_PMD9635) && (pmic_info.nPmicAllLayerRevision == 1) && (smps_index > 3))
        {
            smps_index = smps_index - 1;
        }

        /* Peripheral Index should be less than number of peripherals */
        CORE_VERIFY(smps_index < (smps_ptr->pm_pwr_data.num_of_peripherals));


        /* Save SMPS's Peripheral Type value */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type = peripheral_info->peripheral_type;

        /* Save each SMPS peripheral's base address */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_base_address = peripheral_info->base_address;

        /* SMPS Vset Info - Obtaining Data through dal config */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset = pm_smps_get_volt_setting_info(peripheral_info);

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset);

        /* SMPS Range Info - Obtaining Data through dal config */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range = pm_smps_get_range_setting_info(peripheral_info);

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range);
    }
}

pm_pwr_volt_info_type* pm_smps_get_volt_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_volt_info_type *volt_info = NULL;

  switch ((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_FTS:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_FTS__FTS_CTL:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_FTS_VOLT);
      break;
    case PM_HW_MODULE_FTS__FTS2p5_CTL:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_FTS2p5_VOLT);
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_HF_BUCK:
    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_HFS_VOLT);
    break;
  case PM_HW_MODULE_ULT_BUCK:
    switch(peripheral_info->peripheral_subtype)
    {
      case PM_HW_MODULE_ULT_BUCK_CTL_LV:
      case PM_HW_MODULE_ULT_BUCK_CTL2:
      case PM_HW_MODULE_ULT_BUCK_CTL3:
      {
         volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_VOLT_1);
         break;
      }
      case PM_HW_MODULE_ULT_BUCK_CTL_MV:
      {
         volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_VOLT_2);
         break;
      }
      default:
      {
         // we should not reach this point
         break;
      }
    }
    break;
  default:
    volt_info = NULL;
    break;
  }

  return volt_info;
}

pm_pwr_range_info_type* pm_smps_get_range_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_range_info_type *range_info = NULL;

  switch ((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_FTS:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_FTS__FTS_CTL:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_FTS_RANGE);
      break;
    case PM_HW_MODULE_FTS__FTS2p5_CTL:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_FTS2p5_RANGE);
      break;
    default:
      range_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_HF_BUCK:
    range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_HFS_RANGE);
    break;
  case PM_HW_MODULE_ULT_BUCK:
    switch(peripheral_info->peripheral_subtype)
    {
      case PM_HW_MODULE_ULT_BUCK_CTL_LV:
      case PM_HW_MODULE_ULT_BUCK_CTL2:
      case PM_HW_MODULE_ULT_BUCK_CTL3:
      {
         range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_RANGE_1);
         break;
      }
      case PM_HW_MODULE_ULT_BUCK_CTL_MV:
      {
         range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_RANGE_2);
         break;
      }
      default:
      {
         // we should not reach this point
         break;
      }
    }
    break;
  default:
    range_info = NULL;
    break;
  }

  return range_info;
}

pm_smps_data_type* pm_smps_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_smps_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_smps_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_smps_data_arr[pmic_index] != NULL))
  {
      return pm_smps_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return NULL;
}

