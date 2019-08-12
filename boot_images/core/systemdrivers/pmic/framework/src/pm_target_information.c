/*! \file pm_target_information.c
*
*  \brief This file contains target specific PMIC settings common across processors.
*  \n
*  &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_target_information.c#1 $

when        who         what, where, why
--------    ---         ----------------------------------------------------------
09/29/14    mr          Updated/Modified PBS Driver (CR-728234)
07/16/14    akm         Comm change Updates
03/31/14    akm         Updated to the latest PMIC driver architecture
09/19/13    aab         Added support for SBL_REG_CONFIG and PBS_RAM_CONFIG
12/06/12    hw          Architecture change from Module to Peripheral based driver
08/07/12    hs          Added support for 5V boost.
04/16/12    hs          Removed irq files.
04/03/10    wra         Creating file for MSM8960
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"
#include "DALSys.h"
#include "pm_version.h"
#include "pm_comm.h"
#include "device_info.h"
#include "SpmiCfg.h"
#include "CoreVerify.h"
/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_target);
static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_common);
static DALSYSPropertyVar prop_target;
static DALSYSPropertyVar prop_common;

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/
/*
    Description: The function retrieves PMIC property handle.
                 On success it queries the SPMI configurations.
                 It configures SPMI channel and interrupt ownership for peripherals.
    @param None
    @return
    PM_ERR_FLAG__SUCCESS on success otherwise PM_ERR_FLAG__SPMI_OPT_ERR.
    @dependencies
    SPMI APIs.
    @sa None
*/

pm_err_flag_type
pm_target_information_spmi_chnl_cfg(char* pmic_props)
{
  DALResult result = DAL_SUCCESS;
  DALSYSPropertyVar prop_var;
  uint32 num_periph = 0;
  SpmiCfg_ChannelCfg* periph_cfg_ptr = (SpmiCfg_ChannelCfg*) NULL;
  pm_err_flag_type pm_error = PM_ERR_FLAG__SUCCESS;

  /* PMIC target config data handle property */
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);

  /* Get handle. */
  result = DALSYS_GetDALPropertyHandleStr((const char*)pmic_props, hProp);
  CORE_VERIFY(result == DAL_SUCCESS);

  /* Get number of peripherals. */
  result = DALSYS_GetPropertyValue(hProp,NULL, PM_PROP_NUMBER_PERIPHERAL, &prop_var);
  CORE_VERIFY(result == DAL_SUCCESS);
  num_periph = *(uint32*)prop_var.Val.pStruct;

  /* Get spmi config data pointer. */
  result = DALSYS_GetPropertyValue(hProp,NULL, PM_PROP_SPMI_PERIPH_CFG, &prop_var);
  CORE_VERIFY(result == DAL_SUCCESS);
  periph_cfg_ptr = (SpmiCfg_ChannelCfg*)prop_var.Val.pStruct;

  /* Initialize SPMI and enable SPMI global interrupt. */
  if (SpmiCfg_Init(TRUE) == SPMI_SUCCESS)
  {
    /* Configures SPMI channel and peripheral interrupt ownership. */
    if (SpmiCfg_ConfigureChannels(periph_cfg_ptr, num_periph) != SPMI_SUCCESS)
    {
      pm_error = PM_ERR_FLAG__SPMI_OPT_ERR;
    }
  }

  return pm_error;
}

void pm_target_information_init(void)
{
  /*
   * Runtime detection to support multiple PMIC configuration on any target.
   * We're only concerned with the primary PMIC here so we use index 0.
   */
  char* pmic_props = NULL;
  uint8 pm_model = pm_get_pmic_model((uint8)0);

  switch(pm_model)
  {
     case PMIC_IS_PM8994:
       pmic_props = "/boot/pm8994";
       break;

     case PMIC_IS_PMD9635:
       pmic_props = "/boot/pmd9x45";
       break;

	 case PMIC_IS_PM8916:
       pmic_props = "/boot/pm8916";
       break;

     case PMIC_IS_PM8909:
       pmic_props = "/boot/pm8909";
       break;

     case PMIC_IS_PM8950:
       pmic_props = "/boot/pm8950";
       break;

     default:
       break;
  }
  if(NULL == pmic_props)
  {
    /*
     * ERR Fatal her as we were unable to discover the PMIC.  No device ID for PMIC -
     * just use 0.
     */
    DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
      "PMIC:  Initialization failed to identify PMIC device!  PMIC Model listed as: %d.", pm_model);
    return;
  }

  if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((const char*)pmic_props, hProp_target))
  {
    DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
      "PMIC:  Initialization failed hProp_target handle retrieval!");
  }

  if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr("/boot/pmic/common", hProp_common))
  {
    DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
      "PMIC:  Initialization failed hProp_common handle retrieval!");
  }
}

pm_err_flag_type pm_target_information_read_peripheral_rev(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type base_address = (pm_register_address_type)peripheral_info->base_address;
    uint32 digital_minor_index = 0;
    uint32 digital_major_index = 1;
    uint32 analog_minor_index = 2;
    uint32 analog_major_index = 3;
    uint32 peripheral_type_index = 4;
    uint32 peripheral_subtype_index = 5;
    uint32 peripheral_dummy_index = 6;
    const uint32 num_of_bytes = 7;
    pm_register_data_type temp_peripheral_info[7] = {0};

    if(!comm_ptr)
    {
        return PM_ERR_FLAG__COMM_TYPE_NOT_RECOGNIZED;
    }

    // Reset the peripheral info
    peripheral_info->peripheral_type = 0;
    peripheral_info->peripheral_subtype = 0;
    peripheral_info->analog_major_version = 0;
    peripheral_info->analog_minor_version = 0;
    peripheral_info->digital_major_version = 0;
    peripheral_info->digital_minor_version = 0;

    /* Burst read the peripheral info */
    pm_comm_read_byte_array(comm_ptr->slave_id, base_address, num_of_bytes, temp_peripheral_info, 0);

    /* When we burst read multiple bytes from a non-existent peripheral, the data returned
       should be same on all the bytes so we need to do the below check and return error */
    if(temp_peripheral_info[peripheral_subtype_index] == temp_peripheral_info[peripheral_dummy_index])
    {
        return PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }

    /* Valid peripheral type can never be 0 */
    if(temp_peripheral_info[peripheral_type_index] == 0)
    {
        return PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }

    peripheral_info->peripheral_type = temp_peripheral_info[peripheral_type_index];
    peripheral_info->peripheral_subtype = temp_peripheral_info[peripheral_subtype_index];
    peripheral_info->digital_major_version = temp_peripheral_info[digital_major_index];
    peripheral_info->digital_minor_version = temp_peripheral_info[digital_minor_index];
    peripheral_info->analog_major_version = temp_peripheral_info[analog_major_index];
    peripheral_info->analog_minor_version = temp_peripheral_info[analog_minor_index];

    return err_flag;
}

void* pm_target_information_get_common_info(uint32 prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_common, NULL, prop_id, &prop_common))
  {
      info = (void*)prop_common.Val.pStruct;
  }

  return info;
}

void* pm_target_information_get_specific_info(uint32 prop_id)
{
  void *info = NULL;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, NULL, prop_id, &prop_target))
  {
      info = (void*)prop_target.Val.pStruct;
  }

  return info;
}

uint32 pm_target_information_get_periph_count_info(uint32 prop_id , uint8 pmic_index)
{
  uint32 count = 0;

  if(DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, NULL, prop_id, &prop_target))
  {
      count = (uint8)prop_target.Val.dwVal;
  }

  return count;
}
