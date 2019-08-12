/*! \file pm_version.c 
*  \n
*  \brief This file contains PMIC version auto-detection implementation
*  \n
*  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/framework/src/pm_version.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/13   kt      New PMIC Version driver format.
04/16/13   kt      Added pm_get_pbs_info API.
01/26/13   kt      Added pm_get_pmic_info API.
01/24/13   kt      Adding device index param to pmic model and revision APIs.
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_version.h"
#include "device_info.h"
#include "spmi_1_0_lite.h"
#include "DALSys.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/* Maximum number of PMIC SLAVE IDs */
#define PM_MAX_NUM_SLAVE_IDS     10

/* QC PMIC Peripheral type value */
#define PM_TYPE_QC_DEVICE        0x51

/* Static global variable to store the pmic device info */
static pm_device_info_type pm_device_info_arr[PM_MAX_NUM_DEVICES];

/* Static global variable to store the pmic device primary slave id value */
static uint32 pm_primary_slave_id[PM_MAX_NUM_DEVICES];

/* Flag to check if PMIC Version driver is initialized */
static boolean pm_version_initialized = FALSE;

/* Number of QC Pmics on the target */
static uint8 pm_num_of_devices = 0;

/*=========================================================================
      Function Definitions
==========================================================================*/
/** 
 * @name pm_version_detect 
 *  
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_version_detect(void) 
{
  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
  uint32            slave_id = 0;
  uint8             pmic_index = 0;
  uint8             data = 0;
  uint16            base_addr = 0x0100;
  uint16            metal_rev_addr = 0x02 + base_addr;
//uint16            all_layer_rev_addr = 0x03 + base_addr;
  uint16            periph_type_addr = 0x04 + base_addr;
//uint16            periph_subtype_addr = 0x05 + base_addr;
  const             uint8 num_of_bytes = 4;
  uint8             pmic_data[num_of_bytes] = {0,0,0,0};

  /* This function needs to be called only once to read the PMIC
     version info and store it */
  if (pm_version_initialized)
  {
    return err_flag;
  }

  DALSYS_memset(pm_device_info_arr, 0, sizeof(pm_device_info_arr));

  for(slave_id = 0; slave_id < PM_MAX_NUM_SLAVE_IDS; slave_id = slave_id + 2)
  {
    if(pmic_index >= PM_MAX_NUM_DEVICES)
    {
      break;
    }

    data = 0;

    /* Auto-detection code to check if QC PMIC device exists or not for
       specific slave id */

    /* Read PMIC Device Peripheral Type value and check if PMIC device is
       valid */
    err_flag = pm_spmi_lite_read_byte(slave_id, periph_type_addr, &data, 0);

    /* Make sure if the QC PMIC device exists or not based on if the SPMI
       read operation for PMIC peripheral type value for the specific slave
       id is successful or not and if the data read is equal to QC PMIC
       peripheral type value or not */

    if ((err_flag != PM_ERR_FLAG__SUCCESS) || (data != PM_TYPE_QC_DEVICE))
    {
      /* Once it's confirmed that the QC PMIC device corresponding to the
         slave id doesn't exist, then disregard the error flag returned
         from SPMI read operation and continue to check for other slave ids */
      err_flag = PM_ERR_FLAG__SUCCESS;
      continue;
    }

    /* Read PMIC Device revision information */
    err_flag = pm_spmi_lite_read_byte_array(slave_id, metal_rev_addr, num_of_bytes, pmic_data, 0);

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
      return err_flag;
    }

    /* Store PMIC Device revison information in static global array */
    pm_device_info_arr[pmic_index].ePmicModel = (pm_model_type)pmic_data[3];
    pm_device_info_arr[pmic_index].nPmicAllLayerRevision = (uint32)pmic_data[1];
    pm_device_info_arr[pmic_index].nPmicMetalRevision = (uint32)pmic_data[0];

    /* Store PMIC Device Primary slave id value */
    pm_primary_slave_id[pmic_index] = slave_id;

    /* SW Work around for the wrong all-layer revisions used in earlier PMICs */
    if (pm_device_info_arr[pmic_index].ePmicModel != PMIC_IS_PM8941)
    {
      if (pm_device_info_arr[pmic_index].nPmicAllLayerRevision < 1)
      {
        pm_device_info_arr[pmic_index].nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision + 1;
      }
    }else
    {
      if (pm_device_info_arr[pmic_index].nPmicAllLayerRevision < 2)
      {
        pm_device_info_arr[pmic_index].nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision + 1;
      }
    }

    pmic_index++;
    pm_num_of_devices++;
  }

  pm_version_initialized = TRUE;
  return err_flag;
}

/** 
 * @name pm_get_pmic_model 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
pm_model_type pm_get_pmic_model(uint8 pmic_index)
{
  pm_model_type model = PMIC_IS_INVALID;

  if((!pm_version_initialized) || (pmic_index >= pm_num_of_devices)) 
  {
    return model;
  }

  model = pm_device_info_arr[pmic_index].ePmicModel;

  return model;
}

/** 
 * @name pm_get_pmic_revision 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
uint8 pm_get_pmic_revision(uint8 pmic_index)
{
  uint8 all_layer_rev = 0;

  if((!pm_version_initialized) || (pmic_index >= pm_num_of_devices)) 
  {
    return all_layer_rev;
  }

  all_layer_rev = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;

  return all_layer_rev;
}

/** 
 * @name pm_get_pmic_info 
 *  
 * @brief This is a public API. Please refer to pm_version.h 
 *        file for further info on this function.
 */
pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr)
{

  if((pmic_device_info_ptr == NULL) || (!pm_version_initialized) || (pmic_index >= pm_num_of_devices)) 
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  pmic_device_info_ptr->ePmicModel = pm_device_info_arr[pmic_index].ePmicModel;
  pmic_device_info_ptr->nPmicAllLayerRevision = pm_device_info_arr[pmic_index].nPmicAllLayerRevision;
  pmic_device_info_ptr->nPmicMetalRevision = pm_device_info_arr[pmic_index].nPmicMetalRevision;

  return PM_ERR_FLAG__SUCCESS;
}

/** 
 * @name pm_get_slave_id
 *  
 * @brief Please refer to device_info.h file for further info on
 *        this function.
 */
pm_err_flag_type pm_get_slave_id(uint8 pmic_index, uint8 slave_id_index, uint32* slave_id_ptr)
{

  if((slave_id_ptr == NULL) || (!pm_version_initialized) || (pmic_index >= pm_num_of_devices) || (slave_id_index > 1)) 
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (slave_id_index == 1)
  {
    /* Need to add 1 to the primary slave id to get secondary slave id */
    *slave_id_ptr = pm_primary_slave_id[pmic_index] + 1;
  }else
  {
    *slave_id_ptr = pm_primary_slave_id[pmic_index];
  }

  return PM_ERR_FLAG__SUCCESS;
}
