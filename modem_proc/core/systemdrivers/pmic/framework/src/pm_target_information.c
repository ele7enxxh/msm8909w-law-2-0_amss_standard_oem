/*! \file
*
*  \brief  pm_target_information.c ----This file contains PMIC settings.
*  \details This file contains target specific PMIC settings common across processors.
*
*    PMIC code generation Version: 1.0.0.0
*    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
*    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
*    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
*    This file contains code for Target specific settings and modes.
*
*  &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/src/pm_target_information.c#1 $


when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/23/15    ps       Added PMD9607 model (CR-878990)
11/11/13    rk       Vunicore support - Include target related handle initialization for 8926
10/03/13    rk       Change srequired for new ULT BUCKs and LDOs
08/26/13    rk       Remove BOOST and VS functionalities.

===========================================================================*/

/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "DALSys.h"
#include "pm_target_information.h"
#include "pm_version.h"
#include "pm_utils.h"
#include "pm_comm.h"
#include "device_info.h"


/*===========================================================================

                    STATIC VARIABLES

===========================================================================*/

static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_common);
static DALSYS_PROPERTY_HANDLE_DECLARE(hProp_target);
static DALSYSPropertyVar prop_common;                /* Union to retreive property values */
static DALSYSPropertyVar prop_target;                /* Union to retreive property values */

void pm_target_information_init(void)
{
    /*
    * Runtime detection to support multiple PMIC configuration on any target.
    * We're only concerned with the primary PMIC here so we use index 0.
    */
    char* pmic_props = NULL;
    const char* pmic_props_common = "/modem/pmic/common";
    pm_model_type pm_model = pm_get_pmic_model((uint8) 0);

    switch(pm_model)
    {
        case PMIC_IS_PMD9635:
            pmic_props = "/modem/pmd9635";
            break;
        case PMIC_IS_PMD9607:
            pmic_props = "/modem/pmd9607";
            break;
        case PMIC_IS_PMI8994:
            pmic_props = "/modem/pm8994";
            break;
        case PMIC_IS_PM8916:
            pmic_props = "/modem/pm8916";
            break;
        case PMIC_IS_PM8909:
            pmic_props = "/modem/pm8909";
            break;
        case PMIC_IS_PM8950:
            pmic_props = "/modem/pm8950";
            break;
        case PMIC_IS_PM8937:
            pmic_props = "/modem/pm8937";
            break;
        default:
            break;
    }

    if(NULL == pmic_props)
    {
        /*
         * ERR Fatal here as we were unable to discover the PMIC.
         * No device ID for PMIC - just use 0.
         */
        DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
                         "PMIC:  Initialization failed to identify PMIC device!  "
                         "PMIC Model listed as: %d.", pm_model);
        return;
    }

    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr((const char*)pmic_props, hProp_target))
    {
        DALSYS_LogEvent (0, DALSYS_LOGEVENT_FATAL_ERROR,
                         "PMIC:  Initialization failed hProp_target handle retrieval!");
    }
    if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(pmic_props_common, hProp_common))
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

uint32 pm_target_information_get_periph_count_info(uint32 prop_id, uint8 pmic_index)
{
  uint32 *peripheral_count = NULL;

  if ((pmic_index < PM_MAX_NUM_PMICS) && (PMIC_IS_UNKNOWN != pm_get_pmic_model(pmic_index)) &&
      (DAL_SUCCESS == DALSYS_GetPropertyValue(hProp_target, NULL, prop_id, &prop_target)))
  {
      peripheral_count = (uint32*)prop_target.Val.pStruct;
      return peripheral_count[pmic_index];
  }

  return 0;
}

