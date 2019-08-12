/*! \file
*  \n
*  \brief  pm_tz.c ---- RPM TZ RELATED DEFINITION
*  \n
*  \n This header file contains class implementation of the IComm
*  \n messaging interface
*  \n
*  \n &copy; Copyright 2012--2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/framework/src/pm_tz.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/15   ps      Added PMD9607 support for MDM9x07 (CR-892107)
12/05.14   akt     Added PM8916 support for MSM8909 (CR-764010)
09/02/14   mr      Added PMIC Framework/Driver Test support. (CR-713705)
11/12/13   vtw     Updated for multi-family SPMI driver support.
09/17/13   rk      Removed VS related code.
08/28/13   vtw     Added XPU protection support.
6/28/13    vtw     Updated for SPMI Peripheral CFG ownership.
06/06/13   vtw     Fixed VS and SMPS rail types.
03/26/13   vtw     Updated to use DAL properties.
12/03/13   umr     RPM request for resource
10/04/12   umr     LDO control for 9x25
08/08/12   umr     Integrate to 8974
10/03/11   hs      Added code check to ensure the power rail does get turned
                   on/off before returning.
9/16/2011  hs      File created for 8960

========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "DALSys.h" /* For DALSYS_BusyWait */
#include "pm.h"
#include "pm_npa_vreg.h"
#include "pm_npa_device.h"
#include "rpm.h"
#include "SpmiCfg.h"
#include "pm_access_cfg.h"
#include "Chipinfo.h"
#include "pm_version.h"

/* ============================================================================
**    Definitions
** ==========================================================================*/
#define PMIC_NPA_MODE_ID_QFPROM_DISABLE     0
#define PMIC_NPA_MODE_ID_QFPROM_ENABLE       1

/*===========================================================================
Global Variables
===========================================================================*/

/**
 * Qfprom rail id and type.
 */
static uint32 qfprom_rail_id;
static uint32 qfprom_type;

/**
 * RPM message package size.
 */
static uint32 rail_pkt_size = 0;

/**
 * Pointer to rail config data.
 */

static uint8* rail_enable_ptr = NULL;
static uint8* rail_disable_ptr = NULL;

/**
 * XPU configuration table.
 */
static pm_spmi_rpu_type* pm_spmi_rpu_cfg_ptr = NULL;
static uint32 pm_spmi_rpu_cfg_size = 0;

/*=======================================================================*/

#ifdef PM_TEST_FRAMEWORK_DRIVER
extern pm_err_flag_type pm_test_framework (void);
#endif

extern pm_err_flag_type pm_version_detect(void)  ;

/*=========================================================================
      Function Definitions
=========================================================================*/
/**  Function : pm_spmi_cfg_init
** ======================================================================*/
/*
    Description: The function retrieves PMIC property handle.
                 On success it queries the SPMI configurations.
                 It also configures SPMI Peripheral configurations
                 and PVC ports.

    @param None
    @return
    DAL_SUCCESS -- Initialization was successful.
    DAL_ERROR -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
static DALResult
pm_spmi_cfg_init(void)
{
  uint32 i;
  DALResult result = DAL_SUCCESS;
  DALSYSPropertyVar prop_var;
  uint32 num_periph = 0;
  uint32 num_pvc = 0;
  uint32 num_prio = 0;
  uint8* priorities_ptr = (uint8*) NULL;
  SpmiCfg_ChannelCfg* periph_cfg_ptr = (SpmiCfg_ChannelCfg*) NULL;
  SpmiCfg_PvcPortCfg* pvc_cfg_ptr = (SpmiCfg_PvcPortCfg*) NULL;
  ChipInfoFamilyType ChipFamily = CHIPINFO_FAMILY_UNKNOWN;
  pm_model_type pm_model;
  char* pmic_props = NULL;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  /* TZ PMIC handle property */
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);



  /* Configures spmi config data. */
  if (SpmiCfg_Init(FALSE) != SPMI_SUCCESS)
  {
    return DAL_ERROR;
  }

  err_flag = pm_version_detect();
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
	  return DAL_ERROR;
  }

  /// TODO - update this logic by segregating the target config and common code
  DALSYS_InitMod(NULL);
  ChipFamily = Chipinfo_GetFamily();
  pm_model = pm_get_pmic_model((uint8)0);
  if (PMIC_IS_PM8916 == pm_model && CHIPINFO_FAMILY_MSM8909 == ChipFamily)
  {
	  pmic_props = "/pmic/periph/pm8916"; //Only for MSM8909 + PM8909
  }
  else
  {
	pmic_props = "/pmic/periph"; // For all other targets and MSM8900 default PMIC
  }


     /* Get handle. */

  result = DALSYS_GetDALPropertyHandleStr(pmic_props, hProp);
  if (result != DAL_SUCCESS)
  {
    return result;
  }

  /* For Bear Family Chipsets the SPMI Channel config has moved to SBL so that it is
   * configured in the beginning only to allow any PMIC peripheral access through SPMI.
   * So no need of this SPMI Config again in TZ.
   */
  
  if ((CHIPINFO_FAMILY_MSM8916 != ChipFamily) && (CHIPINFO_FAMILY_MSM8936 != ChipFamily) &&
      (CHIPINFO_FAMILY_MSM8909 != ChipFamily) && (CHIPINFO_FAMILY_MDM9x07 != ChipFamily))
  {
    /* Get number of peripherals. */
    result = DALSYS_GetPropertyValue(hProp,"number_peripheral",0, &prop_var);
    if (result == DAL_SUCCESS)
    {
      num_periph = *(uint32*)prop_var.Val.pStruct;
    }
    else
    {
      return result;
    }

    /* Get spmi config data pointer. */
    result = DALSYS_GetPropertyValue(hProp,"spmi_periph_cfg",0, &prop_var);
    if (result == DAL_SUCCESS)
    {
      periph_cfg_ptr = (SpmiCfg_ChannelCfg*)prop_var.Val.pStruct;
    }
    else
    {
      return result;
    }

    if (SpmiCfg_ConfigureChannels(periph_cfg_ptr, num_periph) != SPMI_SUCCESS)
    {
      return DAL_ERROR;
    }
  }
  /* Get number of pvc ports. */
  result = DALSYS_GetPropertyValue(hProp,"number_pvc_port",0, &prop_var);
  if (result == DAL_SUCCESS)
  {
    num_pvc = *(uint32*)(prop_var.Val.dwVal);
  }
  else
  {
    return result;
  }

  /* Get pvc config data pointer. */
  result = DALSYS_GetPropertyValue(hProp,"pm_arb_pvc_cfg",0, &prop_var);
  if (result == DAL_SUCCESS)
  {
    pvc_cfg_ptr = (SpmiCfg_PvcPortCfg*)prop_var.Val.pStruct;
  }
  else
  {
    return result;
  }
  /* Get number of pvc prios. */
  if (DALSYS_GetPropertyValue(hProp,"number_priorities",0, &prop_var) == DAL_SUCCESS)
  {
    num_prio = *(uint32*)(prop_var.Val.dwVal);
  }
  else
  {
    return result;
  }

  /* Get pvc prio data pointer. */
  if (DALSYS_GetPropertyValue(hProp,"pm_arb_prio_cfg",0, &prop_var) == DAL_SUCCESS)
  {
    priorities_ptr = (uint8*)prop_var.Val.pStruct;
  }
  else
  {
    return result;
  }
  /* Configures pvc config data. */
  for(i = 0; i < num_pvc; i++)
  {
    if (SpmiCfg_ConfigurePvcPort(&pvc_cfg_ptr[i]) != SPMI_SUCCESS)
    {
      return DAL_ERROR;
    }
  }
  /* Configures pvc pvc port priority. */
  if (SpmiCfg_SetPortPriorities(priorities_ptr, num_prio) != SPMI_SUCCESS)
  {
    return DAL_ERROR;
  }
  /* Enable PVC Port */
  if (SpmiCfg_SetPVCPortsEnabled(TRUE) != SPMI_SUCCESS)
  {
    return DAL_ERROR;
  }
  /* Get XPU config table. */
  result = DALSYS_GetPropertyValue(hProp,"xpu_cfg",0, &prop_var);
  if (result == DAL_SUCCESS)
  {
    pm_spmi_rpu_cfg_ptr = (pm_spmi_rpu_type*)prop_var.Val.pStruct;

    /* Get size of config table. */
    result = DALSYS_GetPropertyValue(hProp,"xpu_cfg_size",0, &prop_var);
    pm_spmi_rpu_cfg_size = *(uint32*)prop_var.Val.pStruct;
  }

  return result;

}/* pm_spmi_cfg_init */

/* ========================================================================
**  Function : pm_tz_device_init
** ======================================================================*/
/*
    Description: The function retrieves PMIC property handle.
               On success it queries the qfprom rail ID, type and rail config data.

    @param None
    @return
    PM_ERR_FLAG__SUCCESS -- Initialization was successful.
    PM_ERR_FLAG__DAL_SERVICE_FAILED -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
pm_err_flag_type
pm_tz_device_init(void)
{
  DALResult result = DAL_SUCCESS;
  DALSYSPropertyVar prop_var;
  pm_npa_smps_kvps* smps_ptr;
  pm_npa_ldo_kvps* ldo_ptr;
  pm_npa_vs_kvps* vs_ptr;

  /* TZ PMIC handle property */
  DALSYS_PROPERTY_HANDLE_DECLARE(hPropPMIC);

  /* Get handle. */
  result = DALSYS_GetDALPropertyHandleStr("/tz/pmic", hPropPMIC);
  if (result == DAL_SUCCESS)
  {
  /* Get rail ID. */
    result = DALSYS_GetPropertyValue(hPropPMIC,
                                   "QFPROM_rail_id",
                                   0, &prop_var);
    if (result == DAL_SUCCESS)
    {
      qfprom_rail_id = prop_var.Val.dwVal;

      /* Get rail type. */
      result = DALSYS_GetPropertyValue(hPropPMIC,
                                   "QFPROM_rail_type",
                                   0, &prop_var);
      if (result == DAL_SUCCESS)
      {
        qfprom_type = prop_var.Val.dwVal;

        /* Get qfprom power rail config data. */
        result = DALSYS_GetPropertyValue(hPropPMIC,
                                         "pam_qfprom_rail",
                                         0, &prop_var);

      switch (qfprom_type)
        {
        case RPM_SMPS_A_REQ:
        case RPM_SMPS_B_REQ:
          rail_pkt_size = sizeof(pm_npa_smps_kvps);
          smps_ptr =  (pm_npa_smps_kvps*)prop_var.Val.pStruct;
          rail_enable_ptr = (uint8*)&smps_ptr [PMIC_NPA_MODE_ID_QFPROM_ENABLE];
          rail_disable_ptr = (uint8*)&smps_ptr [PMIC_NPA_MODE_ID_QFPROM_DISABLE];
          break;
        case RPM_LDO_A_REQ:
        case RPM_LDO_B_REQ:
          rail_pkt_size = sizeof(pm_npa_ldo_kvps);
          ldo_ptr = (pm_npa_ldo_kvps*)prop_var.Val.pStruct;
          rail_enable_ptr = (uint8*)&ldo_ptr [PMIC_NPA_MODE_ID_QFPROM_ENABLE];
          rail_disable_ptr = (uint8*)&ldo_ptr [PMIC_NPA_MODE_ID_QFPROM_DISABLE];
          break;
        case RPM_VS_A_REQ:
        case RPM_VS_B_REQ:
          rail_pkt_size = sizeof(pm_npa_vs_kvps);
          vs_ptr = (pm_npa_vs_kvps*)prop_var.Val.pStruct;
          rail_enable_ptr = (uint8*)&vs_ptr [PMIC_NPA_MODE_ID_QFPROM_ENABLE];
          rail_disable_ptr = (uint8*)&vs_ptr [PMIC_NPA_MODE_ID_QFPROM_DISABLE];
          break;
       default:
        break;
        }

         /* Init SPMI Peripheral configurations. */
        result = pm_spmi_cfg_init();
      }

    }

    #ifdef PM_TEST_FRAMEWORK_DRIVER
      pm_test_framework ();
    #endif
  }

  /* Return PMIC error flag. */
  return (result == DAL_SUCCESS) ? PM_ERR_FLAG__SUCCESS : PM_ERR_FLAG__DAL_SERVICE_FAILED;

} /* pm_tz_device_init */

/* ========================================================================
**  Function : pm_enable_qfuse_power_rail
** ======================================================================*/
/*
    Description: Requests rpm to enable qfuse power rail.

    @param None
    @return
    PM_ERR_FLAG__SUCCESS -- On success.
    PM_ERR_FLAG__INVALID_POINTER -- Failed.

    @dependencies
    None.

    @sa None
*/
pm_err_flag_type
pm_enable_qfuse_power_rail(void)
{
  /* Check for config data avaialable. */
  if (rail_enable_ptr == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  ubsp_rpm_send(qfprom_type, qfprom_rail_id, RPM_ACTIVE_SET, rail_enable_ptr , rail_pkt_size);

  /* Return PMIC success. */
  return PM_ERR_FLAG__SUCCESS;

}/* pm_enable_qfuse_power_rail */

/* ========================================================================
**  Function : pm_disable_qfuse_power_rail
** ======================================================================*/
/*
    Description: Requests rpm to disable qfuse power rail.

    @param None
    @return
    PM_ERR_FLAG__SUCCESS -- On success.
    PM_ERR_FLAG__INVALID_POINTER -- Failed.

    @dependencies
    None.

    @sa None
*/
pm_err_flag_type
pm_disable_qfuse_power_rail(void)
{
  /* Check for config data avaialable. */
  if (rail_disable_ptr == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  ubsp_rpm_send(qfprom_type, qfprom_rail_id, RPM_ACTIVE_SET, rail_disable_ptr, rail_pkt_size );

  /* Return PMIC success. */
  return PM_ERR_FLAG__SUCCESS;

}/* pm_disable_qfuse_power_rail */
/**
 * @brief This function returns the pointer to the PMIC SPMI
 *        XPU configuration table.
 *
 * @details
 *        The table contains PMIC SPMI access control Read/
 *        Write information for single VMID. Data is read-only.
 *
 * @param
 *        [in] rpu_cfg_ptr: pointer to the PMIC SPMI config
 *        table.
 *        [in] cfg_sz_ptr: size of the config table.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS - On success. PMIC error code
 *         otherwise.
 *
 */
pm_err_flag_type pm_spmi_xpu_config
(
  const pm_spmi_rpu_type ** rpu_cfg_ptr,
  uint32* cfg_sz_ptr
)
{
  /* Device init failed or devcfg data not found,
   * return error.
   */
  if (rpu_cfg_ptr == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  *rpu_cfg_ptr = pm_spmi_rpu_cfg_ptr;
  *cfg_sz_ptr = pm_spmi_rpu_cfg_size;

  /* Return PMIC success. */
  return PM_ERR_FLAG__SUCCESS;

}/* pm_spmi_xpu_config */

