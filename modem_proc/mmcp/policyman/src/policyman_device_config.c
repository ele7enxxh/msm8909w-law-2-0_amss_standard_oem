/**
  @file policy_device_config.c

  @brief  Policy which reads the device configuration and makes it available
          to the rest of modem.
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/src/policyman_device_config.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "mre_regex.h"
#include "mre_efs.h"

#include "policyman_cfgitem.h"
#include "policyman_device_config.h"
#include "policyman_dbg.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_policies.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_refresh.h"
#include "policyman_rf.h"
#include "policyman_rules.h"
#include "policyman_subs.h"
#include "policyman_set.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "log.h"
#include "trm.h"
#include <stringl/stringl.h>



STATIC size_t   g_num_Sim             = 0;
STATIC size_t   g_maxActiveVoiceSubs  = 0;



#define V1_SIZE ( (sizeof(size_t) * 2)             + \
                   sizeof(sys_specialization_t) )


#define V2_SIZE (  sizeof(pdc->num_sims)           + \
                   sizeof(pdc->max_active_voice)   + \
                   sizeof(pdc->specialization)     + \
                   sizeof(pdc->max_active_data) )

#define PARAMETER_MASK_NONE           0x00000000  /* Only for initialization, not testing */
#define PARAMETER_MASK_NMAXVOICE      0x00000001  /* max_active_voice is present */
#define PARAMETER_MASK_NMAXDATA       0x00000002  /* max_active_data is present */
#define PARAMETER_MASK_SPECIALIZATION 0x00000004  /* specialization is present */
#define PARAMETER_MASK_CONFIG         0x00000008  /* configs are present */

typedef struct
{
  POLICYMAN_CMD_HDR;

  int  explicit_index;
} policyman_explicit_index_cmd_t;


#if (defined(FEATURE_CDMA) || defined(CDMA_FEATURE_DEFINED) || defined(FEATURE_CDMA1X) || defined(FEATURE_CDMA_1900) || defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_RX_DIVERSITY))
  #define CDMA_FEATURE_DEFINED
#endif

/*-------- policyman_device_config_init --------*/
void policyman_device_config_init(
 void
)
{
  g_num_Sim            = 0;
  g_maxActiveVoiceSubs = 0;
}

/*=============================================================================
  Device configuration item and APIs to access it.
=============================================================================*/

/*-------- policyman_device_config_dtor --------*/
STATIC void policyman_device_config_dtor(
  void  *pObj
)
{
  device_configuration_t *pMe = (device_configuration_t *)pObj;

  policyman_mem_free((void *)pMe->pECallRatCfg);
  policyman_mem_free((void *)pMe->pConfigs);
  mre_simple_dtor(pObj);
}

 /*-------- policyman_device_config_item_new --------*/
device_configuration_t * policyman_device_config_item_new(
  void
)
 {
  return (device_configuration_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                                sizeof(device_configuration_t),
                                                                policyman_device_config_dtor );
}

/*-------- policyman_efs_put_file_device_config --------*/
STATIC mre_efs_status_t policyman_efs_put_file_device_config(
  policyman_item_t const    *pItem
)
{
  mre_efs_status_t        efs_status  = MRE_EFS_STATUS_SUCCESS;
  device_configuration_t *pdc         = (device_configuration_t *)pItem;
  device_config_efs_v3_t  dev_cfg_efs;

  dev_cfg_efs.version          = POLICYMAN_DEVICE_CONFIG_VERSION_TAG + POLICYMAN_DEVICE_CONFIG_VERSION_NUM;
  dev_cfg_efs.num_sims         = pdc->num_sims;
  dev_cfg_efs.max_active_voice = pdc->max_active_voice;
  dev_cfg_efs.specialization   = pdc->specialization;
  dev_cfg_efs.max_active_data  = pdc->max_active_data;
  dev_cfg_efs.explicit_index   = pdc->explicit_index;

  efs_status = mre_efs_put_file( POLICYMAN_DEVICE_CONFIG_NV_PATH,
                                 (void *)&dev_cfg_efs,
                                 sizeof(dev_cfg_efs),
                                 FALSE,
                                 SYS_MODEM_AS_ID_NONE );
  return efs_status;
}


/*-------- policyman_device_config_update_to_efs --------*/
void policyman_device_config_update_to_efs(
  policyman_item_t const *pItem
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;

  g_num_Sim = pdc->num_sims;
  policyman_efs_put_file_device_config(pItem);

  POLICYMAN_MSG_HIGH_5( "Wrote device config to efs: numSims %d, maxActiveVoice %d, maxActiveData %d, specialization %d, explicit index %d",
                        pdc->num_sims,
                        pdc->max_active_voice,
                        pdc->max_active_data,
                        pdc->specialization,
                        pdc->explicit_index );
}

#define EFS_VALIDATE(test)                            \
  test_is_valid     = (test);                         \
  if (test_is_valid) { validities |= BM(num_tests); } \
  ++num_tests;                                        \
  config_is_valid  &= test_is_valid

/*-------- policyman_device_config_validate_efs_config --------*/
STATIC boolean policyman_device_config_validate_efs_config(
  device_config_efs_v3_t  *dev_cfg_efs,
  device_context_t        *pEfsDevCfg
)
{
  boolean  config_is_valid = TRUE;  // Overall result
  boolean  test_is_valid   = TRUE;  //
  size_t   validities      = 0;     // Bitmask of test results
  size_t   num_tests       = 0;     // Number of tests performed

  EFS_VALIDATE(   (dev_cfg_efs->version > 0)
               && (dev_cfg_efs->version <= POLICYMAN_DEVICE_CONFIG_VERSION_NUM) );

  EFS_VALIDATE(   (dev_cfg_efs->num_sims > 0)
               && (dev_cfg_efs->num_sims <= POLICYMAN_NUM_SUBS) );

  pEfsDevCfg->num_sims = dev_cfg_efs->num_sims;

  EFS_VALIDATE(   (dev_cfg_efs->max_active_voice > 0)
               && (dev_cfg_efs->max_active_voice <= POLICYMAN_NUM_SUBS) );

  pEfsDevCfg->max_active_voice = dev_cfg_efs->max_active_voice;

  EFS_VALIDATE(   (dev_cfg_efs->specialization >= SYS_SPECIAL_NONE)
               && (dev_cfg_efs->specialization <= SYS_SPECIAL_VOLTE_E911_CALLS) );

  pEfsDevCfg->specialization = dev_cfg_efs->specialization;

  // V1 and V2 have config (feature and ratMask)
  // Map to V2 structure (common with V1) and verify
  {
    if (dev_cfg_efs->version < 3)
    {
      device_config_efs_v2_t  *dev_cfg_efs_v2 = (device_config_efs_v2_t *)dev_cfg_efs;

      EFS_VALIDATE(   (dev_cfg_efs_v2->config.feature >= SYS_SUBS_FEATURE_MODE_NORMAL)
                   && (dev_cfg_efs_v2->config.feature <= SYS_SUBS_FEATURE_MODE_SRLTE) );
    }
  }

  // V1 does not have max_active_data.
  // V2 has max active data in different location from V3
  {
    if (2 == dev_cfg_efs->version)
    {
      device_config_efs_v2_t  *dev_cfg_efs_v2 = (device_config_efs_v2_t *)dev_cfg_efs;

      EFS_VALIDATE(   (dev_cfg_efs_v2->max_active_data >= 0)
                   && (dev_cfg_efs_v2->max_active_data <= POLICYMAN_NUM_SUBS) );

      pEfsDevCfg->max_active_data = dev_cfg_efs_v2->max_active_data;
    }

    if (3 == dev_cfg_efs->version)
    {
      EFS_VALIDATE(   (dev_cfg_efs->max_active_data >= 0)
                   && (dev_cfg_efs->max_active_data <= POLICYMAN_NUM_SUBS) );

      pEfsDevCfg->max_active_data = dev_cfg_efs->max_active_data;
      pEfsDevCfg->explicit_index  = dev_cfg_efs->explicit_index;
    }
  }

  POLICYMAN_MSG_HIGH_3( "device_config_validate_efs: config_is_valid %d, num_tests %d, validities 0x%04x",
                         config_is_valid,
                         num_tests,
                         validities );

  return config_is_valid;
}

/*-------- policyman_retrieve_device_config --------*/
STATIC policyman_status_t policyman_retrieve_device_config(
  device_context_t   *pEfsDevCfg
)
{
  mre_efs_status_t        efs_status      = MRE_EFS_STATUS_ERROR;
  device_config_efs_v3_t *dev_cfg_efs_ptr = NULL;
  size_t                  dev_cfg_size    = 0;
  policyman_status_t      status          = POLICYMAN_STATUS_SUCCESS;

  efs_status = mre_efs_get_file( POLICYMAN_DEVICE_CONFIG_NV_PATH,
                                 (void **)&dev_cfg_efs_ptr,
                                 &dev_cfg_size,
                                 SYS_MODEM_AS_ID_NONE );

  if (MRE_EFS_STATUS_SUCCESS != efs_status)
  {
    POLICYMAN_MSG_HIGH_2( "No persisted device config from EFS; efs_status %d, buffer 0x%08x",
                           efs_status,
                           dev_cfg_efs_ptr );

    status = POLICYMAN_STATUS_ERR;
    goto Done;
  }

  if ((dev_cfg_efs_ptr->version & POLICYMAN_DEVICE_CONFIG_VERSION_TAG) != POLICYMAN_DEVICE_CONFIG_VERSION_TAG)
  {
    // Version marker not present
    POLICYMAN_MSG_ERROR_2( "Invalid device config from EFS; size %d, vers 0x%08x",
                           dev_cfg_size,
                           dev_cfg_efs_ptr->version );

    efs_status = MRE_EFS_STATUS_ERROR;
    status     = POLICYMAN_STATUS_ERR_INVALID_VERSION;
    goto Done;
  }

  dev_cfg_efs_ptr->version &= POLICYMAN_DEVICE_CONFIG_VERSION_MASK; // Remove tag and leave version

  if (!policyman_device_config_validate_efs_config(dev_cfg_efs_ptr, pEfsDevCfg))
  {
    POLICYMAN_MSG_ERROR_1( "Invalid device version from EFS; vers 0x%08x", dev_cfg_efs_ptr->version);

    efs_status = MRE_EFS_STATUS_ERROR;
    status = POLICYMAN_STATUS_ERR_INVALID_VERSION;
    goto Done;
  }

  POLICYMAN_MSG_HIGH_6( "---------- Device Config from EFS (v%d): nSim %d, maxActiveVoice %d, maxActiveData %d, specialization %d, explicit index %d",
                        dev_cfg_efs_ptr->version,
                        pEfsDevCfg->num_sims,
                        pEfsDevCfg->max_active_voice,
                        pEfsDevCfg->max_active_data,
                        pEfsDevCfg->specialization,
                        pEfsDevCfg->explicit_index );

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);

  efs_status = MRE_EFS_STATUS_SUCCESS;

Done:
  POLICYMAN_MEM_FREE_IF(dev_cfg_efs_ptr);
  return status;
}

/*-------- policyman_device_config_get_default_from_efs --------*/
policyman_status_t policyman_device_config_get_default_from_efs(
  cfgitem_execute_ctx_t *pCfgCtx
)
{
  device_configuration_t  *pdc       = NULL;
  device_context_t         efsDevCfg;
  policyman_status_t       status;


  efsDevCfg.num_sims         = 0;
  efsDevCfg.max_active_voice = 0;
  efsDevCfg.max_active_data  = 0;
  efsDevCfg.specialization   = SYS_SPECIAL_NONE;
  efsDevCfg.explicit_index   = -1;

  status = policyman_retrieve_device_config(&efsDevCfg);
  if (POLICYMAN_SUCCEEDED(status))
  {
    pdc = policyman_device_config_item_new();

    pdc->num_sims         = efsDevCfg.num_sims;
    pdc->max_active_voice = efsDevCfg.max_active_voice;
    pdc->max_active_data  = efsDevCfg.max_active_data;
    pdc->specialization   = efsDevCfg.specialization;
    pdc->modified         = FALSE;
    pdc->asubs_id         = pCfgCtx->asubs_id;
    pdc->explicit_index   = efsDevCfg.explicit_index;

    // Since we have got a valid config, update the static value

    g_num_Sim = efsDevCfg.num_sims;

    /*  Don't update any config here. They will be created later from XML or default function */
    pdc->hardware_mask = policyman_rf_get_overall_rat_mask();
    pdc->num_cfgs      = 0;
    pdc->e911_num_cfgs = 0;
    pdc->pECallRatCfg  = NULL;
    pdc->pECallRatCfg  = NULL;

    pCfgCtx->pItem = (policyman_item_t *)pdc;
  }

  return status;
}


/*-------- policyman_device_config_compare --------*/
boolean policyman_device_config_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
  )
{
  device_configuration_t  *pdcNew = (device_configuration_t *) pData1;
  device_configuration_t  *pdcOld = (device_configuration_t *) pData2;
  size_t                  nCfgBytes;

  if (
        pdcNew == NULL
     || pdcOld == NULL
     || pdcNew->num_sims != pdcOld->num_sims
     || pdcNew->num_cfgs != pdcOld->num_cfgs
     || pdcNew->max_active_voice != pdcOld->max_active_voice
     || pdcNew->max_active_data != pdcOld->max_active_data
     || pdcNew->current_cfg_index != pdcOld->current_cfg_index
     || pdcNew->explicit_index != pdcOld->explicit_index
     || pdcNew->forceUpdate
     )
  {
    return FALSE;
  }

  nCfgBytes = pdcNew->num_sims * pdcNew->num_cfgs * sizeof(uint32);

  return memcmp(pdcNew->pConfigs, pdcOld->pConfigs, nCfgBytes) == 0;
}

/*-------- policyman_device_config_display --------*/
void policyman_device_config_display(
  policyman_item_t  *pItem
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  sys_overall_feature_t    feature;
  sys_subs_feature_t       subsFeature = SYS_SUBS_FEATURE_MODE_NORMAL;
  size_t                   nSim;
  uint32                   ratCap;

  policyman_device_config_overall_feature(pItem, &feature);

  POLICYMAN_MSG_HIGH_0( "Device configuration:" );
  POLICYMAN_MSG_HIGH_6( " nSim %d, maxActiveVoice %d, maxActiveData %d, nCfgs %d, nECallCfgs %d, hardware_mask 0x%04x", 
                        pdc->num_sims,
                        pdc->max_active_voice,
                        pdc->max_active_data,
                        pdc->num_cfgs,
                        pdc->e911_num_cfgs,
                        pdc->hardware_mask );

  POLICYMAN_MSG_HIGH_4( " specialization %d, overall feature %d, CfgIndex %d, index modem controlled %d",
                       pdc->specialization,
                       feature,
                       pdc->current_cfg_index,
                       pdc->explicit_index );

  for ( nSim = 0;
       (pdc->num_cfgs > 0) && nSim < pdc->num_sims;
        nSim++
      )
  {
    (void)policyman_device_config_get_current_subphone_capability(pItem, nSim, &ratCap);
    subsFeature = device_config_get_subs_feature(pdc, 0, nSim);

    POLICYMAN_MSG_HIGH_3( "  subphone[%d] feature %d, ratMask 0x%04x",
                          nSim,
                          subsFeature,
                          ratCap );
  }
}

/*-------- policyman_device_config_get_default_sims --------*/
void policyman_device_config_get_default_sims(
  size_t  *pnSims,
  size_t  *pnActiveVoice,
  size_t  *pnActiveData
)
{
  mre_efs_status_t               status;
  void                          *pData       = NULL;
  size_t                         size;
  sys_modem_device_mode_e_type   device_mode;
  policyman_rf_device_info_t    *pInfo       = policyman_rf_get_overall_info();

  if (pInfo == NULL)
  {
    POLICYMAN_ERR_FATAL("No RF info when getting default device configuration", 0, 0, 0);
  }

  status = mre_efs_get_file( DEVICE_MODE_NV_FILE,
                             &pData,
                             &size,
                             SYS_MODEM_AS_ID_NONE );

  if (  (status == MRE_EFS_STATUS_SUCCESS)
     && (size == sizeof(sys_modem_device_mode_e_type)) )
  {
    device_mode = *((sys_modem_device_mode_e_type *) pData);
    POLICYMAN_MSG_HIGH_1("Successful EFS read of device_mode NV; device_mode %d", device_mode);
  }
  else
  {
    POLICYMAN_MSG_HIGH_0(DEVICE_MODE_NV_FILE " not present or invalid");
    device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
  }

  *pnActiveData = 1 ;

  switch (device_mode)
  {
    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
      *pnSims = 1;
      *pnActiveVoice = 1;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
      *pnSims = 2;
      *pnActiveVoice = 1;
      break;

    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      *pnSims = 2;
      *pnActiveVoice = 2;
      break;

    case SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY:
      *pnSims = 3;
      *pnActiveVoice = 1;
      break;

    default:
      POLICYMAN_MSG_ERROR_1("Unknown device mode from NV: %d, setting default device mode SINGLE_SIM", device_mode);
      *pnSims = 1;
      *pnActiveVoice = 1;
      break;
  }

  POLICYMAN_MEM_FREE_IF(pData);
}


/*-------- policyman_device_config_get_default --------*/
policyman_item_t *policyman_device_config_get_default(
  policy_execute_ctx_t *pCtx
)
{
  device_configuration_t      *pdc            = NULL;
  size_t                       nSims          = 1;
  size_t                       maxActiveVoice = 1;
  size_t                       maxActiveData  = 1;
  sys_specialization_t         specialization = SYS_SPECIAL_NONE;
  size_t                       idx;

  //  In test mode and limited mode we only work as single-SIM
  if (  !policyman_is_test_mode()
     && !policyman_cfgitem_limited_capabilities_is_enabled()
     )
    {
      policyman_device_config_get_default_sims(&nSims, &maxActiveVoice, &maxActiveData);
    }

  pdc           = policyman_device_config_item_new();
  pdc->pConfigs = policyman_mem_alloc(sizeof(subs_info_t) * nSims);

  pdc->asubs_id         = pCtx->asubs_id;
  pdc->num_sims         = nSims;
  pdc->max_active_voice = maxActiveVoice;
  pdc->max_active_data  = maxActiveData;
  pdc->specialization   = specialization;
  pdc->hardware_mask    = policyman_rf_get_overall_rat_mask();

  /* Determine the maximum active voice subs ever possible */
  if (maxActiveVoice > g_maxActiveVoiceSubs)
  {
    g_maxActiveVoiceSubs = maxActiveVoice;
  }

  pdc->num_cfgs          = 1;
  pdc->current_cfg_index = 0;
  pdc->e911_num_cfgs = 1;
  pdc->pECallRatCfg  = (uint32 *)policyman_mem_alloc((sizeof(uint32) * nSims));

  /* Since we have got a valid config, update the static value
   */
  g_num_Sim = nSims;

  /*  The first SIM gets hardware RAT capabilities.
        For overall feature mode MultiSIM, use subs feature mode as set by EFS read/deafult operation
        to have legacy W/G+G DSDS/DSDA working
   */
  pdc->pConfigs[0].feature = SYS_SUBS_FEATURE_MODE_NORMAL;
  pdc->pConfigs[0].ratInfo = pdc->hardware_mask;
  pdc->pECallRatCfg[0]     = pdc->hardware_mask;

  /*  All other SIMs get GSM.
   */
  for (idx = 1 ; idx < nSims ; ++idx)
  {
    pdc->pConfigs[idx].feature = SYS_SUBS_FEATURE_MODE_NORMAL;
    pdc->pConfigs[idx].ratInfo = SYS_SYS_MODE_MASK_GSM;
    pdc->pECallRatCfg[idx]     = SYS_SYS_MODE_MASK_GSM;
  }

  POLICYMAN_MSG_HIGH_1(SUBS_PREFIX "Default device configuration created", pdc->asubs_id );

  return (policyman_item_t *) pdc;
}

/*-------- device_config_get_subphone_index --------*/
STATIC size_t device_config_get_subphone_index(
 size_t  num_sims,
 size_t  nCfg,
 size_t  nSim
)
{
  return  num_sims * nCfg + nSim;
}

/*-------- device_config_get_base_config --------*/
uint32 device_config_get_base_config(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
)
{
  size_t  index;

  index  = device_config_get_subphone_index(pdc->num_sims, nCfg, nSim);

  return (nCfg >= pdc->num_cfgs || nSim > pdc->num_sims) ? 0
         : pdc->pConfigs[index].ratInfo;
}

/*-------- device_config_get_config --------*/
uint32 device_config_get_config(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
  )
{
  size_t  index = device_config_get_subphone_index(pdc->num_sims, nCfg, nSim);

  return (nCfg >= pdc->num_cfgs || nSim > pdc->num_sims) ? 0
         : pdc->pConfigs[index].ratInfo & pdc->hardware_mask;
}

/*-------- device_config_get_current_config --------*/
uint32 device_config_get_current_config(
  device_configuration_t const  *pdc,
  size_t                         nSim
)
{
  return device_config_get_config(pdc, pdc->current_cfg_index, nSim);
}

/*-------- device_config_get_subs_feature --------*/
sys_subs_feature_t device_config_get_subs_feature(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
)
{
  size_t             index;
  sys_subs_feature_t subsFeature;

  index       = device_config_get_subphone_index(pdc->num_sims, nCfg, nSim);
  subsFeature = pdc->pConfigs[index].feature;

  // sanitize feature for other SIMS. They get only NORMAL or Dual Multimode
  if (  nSim > 0
     && subsFeature != SYS_SUBS_FEATURE_MODE_NORMAL
     && subsFeature != SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE
     )
  {
    subsFeature = SYS_SUBS_FEATURE_MODE_NORMAL;
  }

  return subsFeature;
}

/*-------- policyman_device_config_check_parameters --------*/
STATIC policyman_status_t policyman_device_config_check_parameters(
  device_configuration_t  *pItem,
  void                    *pParam
)
{
  policyman_status_t status = POLICYMAN_STATUS_SUCCESS;

  if (pItem == NULL || pParam == NULL)
  {
    POLICYMAN_MSG_ERROR_0("Invalid parameter passed");
    status = POLICYMAN_STATUS_ERR_INVALID_ARGS;
    goto Done;
  }

  if (policyman_cfgitem_id((policyman_item_t const *)pItem) != POLICYMAN_ITEM_DEVICE_CONFIGURATION)
  {
    POLICYMAN_MSG_ERROR_0("Wrong policy item passed");
    status = POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

Done:
  return status;
}

/*-------- policyman_device_config_num_sims --------*/
policyman_status_t policyman_device_config_num_sims(
  policyman_item_t const  *pItem,
  size_t                  *pnSims
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pnSims);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pnSims = pdc->num_sims;
  }

  return status;
}


/*-------- policyman_device_config_max_active_voice --------*/
policyman_status_t policyman_device_config_max_active_voice(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pnMaxActive);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pnMaxActive = pdc->max_active_voice ;
  }

  return status;
}

/*-------- policyman_device_config_max_active_data --------*/
policyman_status_t policyman_device_config_max_active_data(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActiveData
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pnMaxActiveData);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pnMaxActiveData = pdc->max_active_data;
  }

  return status;
}


/*-------- policyman_device_overall_config_feature --------*/
policyman_status_t policyman_device_config_overall_feature(
  policyman_item_t const  *pItem,
  sys_overall_feature_t   *pFeature
)
{
  device_configuration_t  *pdc         = (device_configuration_t *) pItem;
  policyman_status_t       status;
  sys_subs_feature_t       subsFeature = SYS_SUBS_FEATURE_MODE_NORMAL;

  status = policyman_device_config_check_parameters(pdc, pFeature);
  if (POLICYMAN_FAILED(status))
  {
    return status;
  }

  // get subs feature only when we have a valid config
  if (pdc->num_cfgs > 0)
  {
    subsFeature = device_config_get_subs_feature(pdc, 0, 0);
  }

  if (pdc->num_sims > 1)
  {
    *pFeature = SYS_OVERALL_FEATURE_MODE_MULTISIM;
  }
  else if (subsFeature == SYS_SUBS_FEATURE_MODE_SRLTE)
  {
    *pFeature = SYS_OVERALL_FEATURE_MODE_SRLTE;
  }
  else
  {
    *pFeature = SYS_OVERALL_FEATURE_MODE_NORMAL;
  }

  return (pdc->modified & DEV_CONFIG_FEATURE_MODIFIED)?
          POLICYMAN_STATUS_WRN_MODIFIED : POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_device_config_num_cfgs --------*/
policyman_status_t policyman_device_config_num_cfgs(
  policyman_item_t const  *pItem,
  size_t                  *pnCfgs
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pnCfgs);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pnCfgs = pdc->num_cfgs;
  }

  return status;
}

/*-------- policyman_device_config_get_config --------*/
policyman_status_t policyman_device_config_get_config(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  uint32                  *pRatCfg
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pRatCfg);
  if (  POLICYMAN_SUCCEEDED(status)
     && nCfg < pdc->num_cfgs
     && nSim < pdc->num_sims
     )
  {
    *pRatCfg = device_config_get_config(pdc, nCfg, nSim);
  }

  return status;
}


/*-------- policyman_device_config_get_subs_feature --------*/
policyman_status_t policyman_device_config_get_subs_feature(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  sys_subs_feature_t      *pFeature
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pFeature);
  if (  POLICYMAN_SUCCEEDED(status)
     && nCfg < pdc->num_cfgs
     && nSim < pdc->num_sims
     )
  {
    *pFeature = device_config_get_subs_feature(pdc, nCfg, nSim);
  }

  return status;
}


/*-------- policyman_device_config_get_specialization --------*/
policyman_status_t policyman_device_config_get_specialization(
  policyman_item_t const  *pItem,
  sys_specialization_t    *pSpecial
)
{
  device_configuration_t  *pdc = (device_configuration_t *) pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pSpecial);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pSpecial = pdc->specialization;
  }

  return status;
}

/*-------- policyman_get_current_device_config --------*/
policyman_status_t policyman_get_current_device_config(
  policyman_item_t ** pdc
)
{
  policyman_item_id_t  id     = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  policyman_status_t   status;

  status = policyman_get_items( (policyman_item_id_t const *)&id,
                                1,
                                (policyman_item_t const **)pdc );

  return status;
}

/*-------- policyman_get_current_num_sim --------*/
policyman_status_t policyman_get_current_num_sim(
  size_t *pnSim
)
{
  device_configuration_t   *pdc = NULL;
  policyman_status_t   status = POLICYMAN_STATUS_SUCCESS;

  /* Return nSIM if it has been read once
       The value doesn't change in run time
   */
  if (g_num_Sim > 0)
  {
    *pnSim = g_num_Sim;
    return status;
  }

  status = policyman_get_current_device_config((policyman_item_t **)&pdc);

  if (status == POLICYMAN_STATUS_SUCCESS)
  {
    *pnSim = pdc->num_sims;

    /* Update nSim as well if we have got info correctly
      */
      g_num_Sim = *pnSim;
    }

  REF_CNT_OBJ_RELEASE_IF(pdc);
  return status;
}

/*-------- policyman_get_max_capability_for_all_configs --------*/
STATIC void policyman_get_max_capability_for_all_configs(
  device_configuration_t  *pdc,
  size_t                   nSim,
  uint32                  *pRatCfg
)
{
  size_t nCfg;
  boolean  include1x;

  *pRatCfg = 0;
  for (nCfg = 0 ; nCfg < pdc->num_cfgs ; ++nCfg)
  {
    *pRatCfg |= device_config_get_config(pdc, nCfg, nSim);
  }

  // for primary card add CH in max cap if allowed
  include1x = policyman_get_boolean_with_default("pm:include_1x_for_hlos", FALSE);

  if (nSim == 0 && include1x)
  {
    *pRatCfg |= (SYS_SYS_MODE_MASK_CDMA | SYS_SYS_MODE_MASK_HDR);
  }
}

/*-------- policyman_device_config_get_max_capability --------*/
policyman_status_t policyman_device_config_get_max_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
)
{
  device_configuration_t  *pdc = (device_configuration_t *)pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pRatCfg);
  if (POLICYMAN_FAILED(status))
  {
    return status;
  }

  if (nSim >= pdc->num_sims)
  {
    POLICYMAN_MSG_ERROR_2( "requested config for nSim = %d while device supports %d cards",
                           nSim,
                           pdc->num_sims );

    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  policyman_get_max_capability_for_all_configs(pdc, nSim, pRatCfg);
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_get_current_subphone_capability --------*/
policyman_status_t policyman_device_config_get_current_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
)
{
  device_configuration_t  *pdc  = (device_configuration_t *)pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pRatCfg);
  if (POLICYMAN_FAILED(status))
  {
    return status;
  }

  if (nSim >= pdc->num_sims)
  {
    POLICYMAN_MSG_ERROR_2( "requested config for nSim = %d while device supports %d cards",
                           nSim,
                           pdc->num_sims );

    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  *pRatCfg = device_config_get_current_config(pdc, nSim);
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_e911_subphone_capability --------*/
policyman_status_t policyman_get_e911_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   numItems,
  uint32                  *pRatArray
)
{
  size_t                   nSim;
  device_configuration_t  *pdc                 = (device_configuration_t *)pItem;
  boolean                  primarySubsActive   = FALSE;
  boolean                  secondarySubsActive = FALSE;
  policyman_status_t       status;
  policyman_state_t       *pState              = policyman_state_get_state();

  status = policyman_device_config_check_parameters(pdc, pRatArray);
  if (POLICYMAN_FAILED(status))
  {
    goto Done;
  }

  // client shouldn't request for more items that we configured for
  if (numItems > pdc->num_sims)
  {
    status = POLICYMAN_STATUS_ERR_INVALID_ARGS;
    goto Done;
  }

  /* For legacy configs we only have one entry in pECallRatCfg[]
       which is same as the current config
  */
  if (pdc->e911_num_cfgs < 2)
  {
    for (nSim = 0; nSim < numItems; nSim++)
    {
      pRatArray[nSim] = device_config_get_current_config(pdc, nSim);
    }

    goto Done;
  }

  /* Single SIM configs get current subphone capability.
       MSIM configs get e911 mask based on active SUBS
   */
  if (pdc->num_sims > 1 && pdc->pECallRatCfg != NULL)
  {
    primarySubsActive   = policyman_is_cm_subs_active(pState, SYS_MODEM_AS_ID_1);
    secondarySubsActive = policyman_is_cm_subs_active(pState, SYS_MODEM_AS_ID_2);

    POLICYMAN_MSG_HIGH_2( "e911_subphone_capability active subs: primary %d, secondary %d",
                          primarySubsActive,
                          secondarySubsActive );

    // return error if both subs active. current subphone cap is the correct one.
    if (primarySubsActive && secondarySubsActive)
    {
      status = POLICYMAN_STATUS_ERR;
      goto Done;
    }

    // copy the relevant e911 config based on which subs is active
    for (nSim = 0; nSim < numItems; nSim++)
    {
      size_t nSubs = nSim;

      if (secondarySubsActive)
    {
        nSubs += numItems;
      }

      pRatArray[nSim] = pdc->pECallRatCfg[nSubs] & pdc->hardware_mask;
    }
  }

Done:

  for (nSim = 0; nSim < numItems; nSim++)
  {
    POLICYMAN_MSG_HIGH_3( "e911_subphone_capability pRatArray[idx %d of %d] mask = 0x%04x",
                          nSim,
                          numItems,
                          pRatArray[nSim] );
  }

  return status;
}

/*-------- policyman_device_config_get_current_config_index --------*/
policyman_status_t policyman_device_config_get_current_config_index(
  policyman_item_t const  *pItem,
  size_t                  *pIndex
)
{
  device_configuration_t  *pdc    = (device_configuration_t *)pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pIndex);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pIndex = pdc->current_cfg_index;
  }

  return status;
}

/*-------- policyman_device_config_max_active_subs --------*/
policyman_status_t policyman_device_config_max_active_subs(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
)
{
  device_configuration_t  *pdc    = (device_configuration_t *)pItem;
  policyman_status_t       status;

  status = policyman_device_config_check_parameters(pdc, pnMaxActive);
  if (POLICYMAN_SUCCEEDED(status))
  {
    *pnMaxActive = g_maxActiveVoiceSubs;
  }

  return status;
}

/*-------- policyman_check_and_update_configurations --------*/
STATIC void policyman_check_and_update_configurations(
  device_configuration_t *pdc
)
{
  size_t   nSim;

  pdc->hardware_mask = policyman_rf_get_overall_rat_mask();

  /* Ensure single configuration is CHGWLT + G */
  if (pdc->num_cfgs == 0)
  {
    POLICYMAN_MSG_HIGH_0("no configuration specified, adding <CHGWLT+G>");

    /* single config is supported when not defined by XML */
    pdc->num_cfgs          = 1;
    pdc->current_cfg_index = 0;
    pdc->explicit_index    = -1;
    pdc->pConfigs = (subs_info_t *)policyman_mem_alloc((sizeof(subs_info_t) * pdc->num_sims));

    pdc->pConfigs[0].ratInfo = pdc->hardware_mask;

    for (nSim = 1; nSim < pdc->num_sims; nSim++)
    {
      pdc->pConfigs[nSim].ratInfo = SYS_SYS_MODE_MASK_GSM;
    }
  }

  /*Create and Fill in the e911 config as well if not created */
  if (pdc->e911_num_cfgs == 0)
  {
    POLICYMAN_MSG_HIGH_0("no e911 configuration specified, adding <CHGWLT+G>");

  /* single e911 config is supported when not defined by XML */
    pdc->e911_num_cfgs = 1;
    pdc->pECallRatCfg  = (uint32 *)policyman_mem_alloc((sizeof(uint32) * pdc->num_sims));

    for (nSim = 0; nSim < pdc->num_sims; nSim++)
    {
      pdc->pECallRatCfg[nSim] = device_config_get_current_config(pdc, nSim);
    }
  }
}

/*-------- policyman_ensure_configurations --------*/
void policyman_ensure_configurations(
  policyman_set_t       *pItemSet
)
{
  device_configuration_t  *pdcItem   = NULL;
  boolean                  item_is_present;

  item_is_present = policyman_cfgitem_is_present_in_itemset( pItemSet,
                                                             POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                             SYS_MODEM_AS_ID_1,
                                                             (policyman_item_t  **)&pdcItem);

  if (item_is_present)
  {
    policyman_check_and_update_configurations(pdcItem);
  }
  else
  {
    POLICYMAN_MSG_ERROR_0("failed to get device config item");
  }
}

/*-------- device_config_copy_configs --------*/
STATIC void device_config_copy_configs(
  device_configuration_t const *pCurrentDc,
  device_configuration_t       *pdc
)
{
  /* Allocate memory for subphone configs if that is NULL */
  if (pdc->num_cfgs == 0 && pCurrentDc->num_cfgs > 0)
  {
    size_t sizeConfigs;

    pdc->num_cfgs = pCurrentDc->num_cfgs;
    sizeConfigs   = pCurrentDc->num_sims * pCurrentDc->num_cfgs * sizeof(subs_info_t);

    pdc->pConfigs = (subs_info_t *)policyman_mem_alloc(sizeConfigs);
    memscpy(pdc->pConfigs, sizeConfigs, pCurrentDc->pConfigs, sizeConfigs);
  }

  /* Allocate memory for e911 configs if that is NULL */
  if (pdc->e911_num_cfgs == 0 && pCurrentDc->e911_num_cfgs > 0)
  {
    size_t sizeE911Configs;

    pdc->e911_num_cfgs = pCurrentDc->e911_num_cfgs;
    sizeE911Configs    = pCurrentDc->num_sims * pCurrentDc->e911_num_cfgs * sizeof(uint32);

    pdc->pECallRatCfg  = (uint32 *)policyman_mem_alloc(sizeE911Configs);
    memscpy(pdc->pECallRatCfg, sizeE911Configs, pCurrentDc->pECallRatCfg, sizeE911Configs);
  }
}

/*-------- device_config_copy_parameters --------*/
STATIC void device_config_copy_parameters(
  device_configuration_t const *pCurrentDc,
  device_configuration_t       *pdc
)
{
  pdc->num_sims          = pCurrentDc->num_sims;
  pdc->max_active_voice  = pCurrentDc->max_active_voice;
  pdc->max_active_data   = pCurrentDc->max_active_data;
  pdc->current_cfg_index = pCurrentDc->current_cfg_index;
  pdc->specialization    = pCurrentDc->specialization;
  pdc->explicit_index    = pCurrentDc->explicit_index;
  pdc->hardware_mask     = pCurrentDc->hardware_mask;

  device_config_copy_configs(pCurrentDc, pdc);
}

/*-------- device_config_get_num_configs --------*/
STATIC size_t device_config_get_num_configs(
  void
)
{
  device_configuration_t  *pdc     = NULL;
  size_t                   numCfgs = 0;
  policyman_status_t       status;

  status = policyman_get_current_device_config((policyman_item_t **)&pdc);
  if (POLICYMAN_SUCCEEDED(status))
  {
    numCfgs = pdc->num_cfgs;
  }
  REF_CNT_OBJ_RELEASE_IF(pdc);

  return numCfgs;
}

/*-------- policyman_device_config_item_clone --------*/
device_configuration_t *policyman_device_config_item_clone(
  device_configuration_t  *pdc
)
{
  device_configuration_t  *pNew;

  pNew = policyman_device_config_item_new();
  device_config_copy_parameters(pdc, pNew);
  return pNew;
}

/*-------- policyman_find_lte_cfg_for_subs --------*/
STATIC size_t policyman_find_lte_cfg_for_subs(
  device_configuration_t *pdc,
  sys_modem_as_id_e_type  ddsSubs
)
{
  size_t  nCfg;
  uint32  ratMask;

  /* Loop through all configs and see which one has LTE for SIM which has DDS */
  for (nCfg = 0; nCfg < pdc->num_cfgs; nCfg++)
  {
    ratMask = device_config_get_base_config(pdc, nCfg, SUBS_ID_TO_NSIM(ddsSubs));

    if ((ratMask & SYS_SYS_MODE_MASK_LTE) != 0)
    {
      return nCfg;
    }
  }

  return 0;
}


/*-------- policyman_update_current_cfg_index --------*/
STATIC boolean policyman_update_current_cfg_index(
  device_configuration_t *pdc,
  sys_modem_as_id_e_type  ddsSubs
)
{
  size_t  index;
  boolean index_updated = FALSE;

  /*  If index is explicitly set, no work to do - just return.
   */
  if (-1 != pdc->explicit_index)
  {
    index = (size_t) pdc->explicit_index;
  }
  else
  {
    index = policyman_find_lte_cfg_for_subs(pdc, ddsSubs);
  }

  if (index != pdc->current_cfg_index)
  {
    POLICYMAN_MSG_HIGH_3( "configuration index: old %d, new %d, explicit %d",
                          pdc->current_cfg_index,
                          index,
                          pdc->explicit_index );

    pdc->current_cfg_index = index;
    index_updated = TRUE;
  }

  return index_updated;
}

/*-------- set_explicit_index --------*/
STATIC void set_explicit_index(
  policy_execute_ctx_t  *pCtx,
  int                    explicitIndex
)
{
  device_configuration_t  *pCurrentDc = NULL;

  pCurrentDc = (device_configuration_t *)policyman_cfgitem_find_item_in_context( pCtx,
                                                                                 POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                                                 SYS_MODEM_AS_ID_1 );

  if (NULL != pCurrentDc)
  {
    device_configuration_t *pdc;

    pdc = policyman_device_config_item_clone(pCurrentDc);

    pdc->explicit_index = explicitIndex;
    if (policyman_update_current_cfg_index(pdc, pCtx->asubs_id))
    {
      policyman_set_replace(pCtx->pItemSet, &pdc);
    }
    ref_cnt_obj_release(pdc);
  }

  REF_CNT_OBJ_RELEASE_IF(pCurrentDc);
}

/*-------- policyman_device_config_update_subphone_config --------*/
void policyman_device_config_update_subphone_config(
  sys_modem_as_id_e_type  ddsSubs,
  policyman_set_t        *pItemSet
)
{
  int                   explicitIndex;
  policy_execute_ctx_t  ctx;

  ctx.pItemSet = pItemSet;
  ctx.asubs_id = ddsSubs;

  // we don't overwrite explcit index here so get whatever we currently have
  (void)policyman_device_config_get_explicit_index(&explicitIndex);
  set_explicit_index(&ctx, explicitIndex);
}

/*-------- policyman_execute_process_explicit_index --------*/
STATIC void policyman_execute_process_explicit_index(
  policyman_cmd_t *pCmd
)
{
  policyman_explicit_index_cmd_t *pDevCfgCmd = (policyman_explicit_index_cmd_t *) pCmd;
  policy_execute_ctx_t            ctx;

  ctx.pItemSet = policyman_itemset_new();

  policyman_state_enter_crit_section();
  ctx.pState   = pDevCfgCmd->pState;
  ctx.asubs_id = policyman_subs_get_dds_subs(pDevCfgCmd->pState);

  // update the index and execute the policies to update other items
  set_explicit_index(&ctx, pDevCfgCmd->explicit_index);
  policyman_state_leave_crit_section();

  policyman_policies_run_policy_check(ctx.pState, ctx.pItemSet);
  policyman_cfgitem_update_items(ctx.pItemSet, pCmd);
  REF_CNT_OBJ_RELEASE_IF(ctx.pItemSet);
}

/*-------- policyman_device_config_set_explicit_index --------*/
policyman_status_t policyman_device_config_set_explicit_index(
  int  explicitIndex
)
{
  policyman_explicit_index_cmd_t *pCmd;
  policyman_status_t              status = POLICYMAN_STATUS_SUCCESS;
  size_t                          numCfgs;

  POLICYMAN_MSG_HIGH_1("received explicit config index %d from client", explicitIndex);

  numCfgs = device_config_get_num_configs();

  if (explicitIndex >= (int)numCfgs)
  {
    POLICYMAN_MSG_ERROR_2( "explicit index %d exceeds number of configs (%d)",
                           explicitIndex,
                           numCfgs );

    status = POLICYMAN_STATUS_ERR_INVALID_ARGS;
    goto Done;
  }
  
  pCmd = POLICYMAN_CMD_NEW( policyman_explicit_index_cmd_t,
                            policyman_execute_process_explicit_index,
                            NULL,
                            NULL );

  pCmd->explicit_index = explicitIndex;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);

Done:
  return status;
}

/*-------- policyman_device_config_get_explicit_index --------*/
policyman_status_t policyman_device_config_get_explicit_index(
  int *pExplicitIndex
)
{
  policyman_status_t       status;
  device_configuration_t  *pdc     = NULL;

  if (NULL == pExplicitIndex)
  {
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  status = policyman_get_current_device_config((policyman_item_t **)&pdc);
  if (POLICYMAN_FAILED(status))
  {
    POLICYMAN_MSG_ERROR_0("policyman_device_config_get_explicit_index called before device config available");
    return POLICYMAN_STATUS_ERR_NOT_READY;
  }

  *pExplicitIndex = pdc->explicit_index;
  REF_CNT_OBJ_RELEASE_IF(pdc);

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_reset_subs_feature --------*/
void policyman_device_config_reset_subs_feature(
  sys_modem_as_id_e_type subsId
)
{
  device_configuration_t  *pdc;
  size_t                   cfgIndex;

  (void)policyman_get_current_device_config((policyman_item_t **)&pdc);

  for (cfgIndex = 0; cfgIndex < pdc->num_cfgs; cfgIndex++)
  {
    size_t        index;
    subs_info_t  *pSubsInfo;

    index     = device_config_get_subphone_index(pdc->num_sims, cfgIndex, SUBS_ID_TO_NSIM(subsId));
    pSubsInfo = &pdc->pConfigs[index];

    if (pSubsInfo->feature != SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE)
    {
      POLICYMAN_MSG_HIGH_3( SUBS_PREFIX "cfg %d -- resetting feature from %d to NORMAL",
                            subsId,
                            cfgIndex,
                            pSubsInfo->feature );

      pSubsInfo->feature = SYS_SUBS_FEATURE_MODE_NORMAL;
    }
  }
  REF_CNT_OBJ_RELEASE_IF(pdc);
}

/*-------- policyman_get_device_config_from_itemset --------*/
device_configuration_t *policyman_get_device_config_from_itemset(
  policyman_set_t *pItemSet
)
{
  device_configuration_t  **ppDc;
  device_configuration_t   *pDc  = NULL;

  /* check if itemSet already has device config item */
  ppDc = (device_configuration_t **)policyman_cfgitem_find_item_in_itemset( pItemSet,
                                                                            POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                                            SYS_MODEM_AS_ID_1 );
  if (NULL != ppDc && NULL != *ppDc)
  {
    pDc = *ppDc;
    ref_cnt_obj_add_ref(pDc);
  }

  return pDc;
}

/*=============================================================================
  Device configuration action parsing and APIs
=============================================================================*/


typedef struct
{
  sys_subs_feature_t    feature;
  rat_capability_info_t ratInfo;
} subs_info_action_t;


typedef struct
{
  POLICYMAN_ACTION_BASE;

  size_t                 num_sims;
  size_t                 max_active_voice;
  size_t                 max_active_data;
  size_t                 num_cfgs;
  uint32                 modified;
  uint32                 parameterMask;
  sys_specialization_t  specialization;
  const char            *pConfigName;
  size_t                 e911_num_cfg;
  size_t                 current_cfg_index;
  rat_capability_info_t *pECallRatCfg;
  subs_info_action_t     subs_info[];
} device_config_action_t;


/*-------- policyman_check_feature_is_enabled  --------*/
boolean policyman_check_feature_is_enabled(
  sys_subs_feature_t  feature
)
{
  boolean ret = FALSE;

  switch (feature)
  {
    case SYS_SUBS_FEATURE_MODE_SVLTE:
#if (defined(CDMA_FEATURE_DEFINED) && defined(FEATURE_LTE))
      ret = TRUE;
#endif
      break;

    case SYS_SUBS_FEATURE_MODE_SRLTE:
#if defined(FEATURE_1XSRLTE)
      ret = TRUE;
#endif
      break;

    case SYS_SUBS_FEATURE_MODE_SGLTE:
      ret = TRUE;
      break;

    default:
      ret = TRUE;
      break;
  }

  POLICYMAN_MSG_HIGH_2( "check_feature_is_enabled: feature %d, returns %d",
                        feature,
                        ret );

  return ret;
}



/*-------- policyman_feature_is_supported_by_hardware --------*/
boolean policyman_feature_is_supported_by_hardware(
  policyman_set_t   *pItemSet
)
{
  boolean                       is_supported = TRUE;
  device_configuration_t      **ppItem       = NULL;

  ppItem = policyman_cfgitem_find_item_in_itemset( pItemSet,
                                                   POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                   SYS_MODEM_AS_ID_1 );

  if (NULL != ppItem)
  {
    sys_subs_feature_t  feature;
    feature = device_config_get_subs_feature(*ppItem, 0, 0);

    switch (feature)
    {
      case SYS_SUBS_FEATURE_MODE_NORMAL:
        break;

      case SYS_SUBS_FEATURE_MODE_SVLTE:
          POLICYMAN_MSG_ERROR_0("SVLTE specified in configuration, but not supported");
          is_supported = FALSE;
        break;

      case SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE:
      case SYS_SUBS_FEATURE_MODE_SRLTE:
        if (!policyman_check_feature_is_enabled(feature))
        {
          POLICYMAN_MSG_ERROR_0("SRLTE specified in configuration, but not supported");
          is_supported = FALSE;
        }

        break;

      case SYS_SUBS_FEATURE_MODE_SGLTE:
          POLICYMAN_MSG_ERROR_0("SGLTE specified in configuration, but not supported");
          is_supported = FALSE;
        break;

      default:
        POLICYMAN_MSG_ERROR_1("Unimplemented feature type: %d", feature);
        break;
    }
  }

  return is_supported;
}


/*-------- policyman_device_config_get_feature --------*/
STATIC boolean policyman_device_config_get_feature(
  char const          *pStr,
  sys_subs_feature_t  *pFeature
)
{
  boolean     succeeded = TRUE;
  char        token[32];
  size_t      i;


  /*  Element that maps a feature name to a feature.
   */
  typedef struct
  {
    char const          *pName;
    sys_subs_feature_t  feature;
  } feature_elem;

  /*  Array of feature elements.
   */
  static feature_elem features[] =
  {
    {"normal",      SYS_SUBS_FEATURE_MODE_NORMAL},
    {"svlte",       SYS_SUBS_FEATURE_MODE_SVLTE},
    {"sglte",       SYS_SUBS_FEATURE_MODE_SGLTE},
    {"srlte",       SYS_SUBS_FEATURE_MODE_SRLTE},
    {"dualmmode",   SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE},
  };

  /*  If nothing is specified, use NORMAL.
   */
  if (!policyman_get_token(&pStr, token, sizeof(token)))
  {
    *pFeature = SYS_SUBS_FEATURE_MODE_NORMAL;
    goto Done;
  }

  /*  If the attribute is present, map the name to the feature.
   */
  for (i = 0 ; i < ARR_SIZE(features) ; ++i)
  {
    feature_elem  *pfe = &features[i];
    if (strcasecmp(token, pfe->pName) == 0)
    {
      *pFeature = pfe->feature;
      goto Done;
    }
  }

  succeeded = FALSE;
  POLICYMAN_MSG_ERROR_0("Invalid feature value in <device_configuration>");
  POLICYMAN_UT_MSG_S1("feature = \"%s\" specified in <device_configuration>", pStr);

Done:
  return succeeded;
}

/*-------- device_config_action_get_subphone --------*/
STATIC subs_info_action_t *device_config_action_get_subphone(
  device_config_action_t  *pAction,
  size_t                   nCfg,
  size_t                   nSim
)
{
  size_t cfgIndex = device_config_get_subphone_index(pAction->num_sims, nCfg, nSim);
  return &pAction->subs_info[cfgIndex];
}

/*-------- device_config_read_mask --------*/
STATIC boolean device_config_read_mask(
  rat_capability_info_t   *pRatInfo,
  const char              *pRatStr
)
{
  boolean  succeeded   = FALSE;

  /* Read Rat mask provided in this subphone string */
  pRatInfo->base        = POLICYMAN_BASE_NONE;
  pRatInfo->excludeMask = 0;

  succeeded = policyman_rat_config_parse_rats( pRatStr,
                                              &pRatInfo->includeMask );
  if (succeeded)
  {
    POLICYMAN_MSG_HIGH_1("Include Mask = 0x%04x", pRatInfo->includeMask);
  }

  return succeeded;
}

/*-------- device_config_get_e911_info --------*/
STATIC rat_capability_info_t *device_config_get_e911_info(
  device_config_action_t  *pAction,
  size_t                   nCfg,
  size_t                   nSim
)
{
  size_t cfgIndex = device_config_get_subphone_index(pAction->num_sims, nCfg, nSim);
  return &pAction->pECallRatCfg[cfgIndex];
}

/*-------- device_config_get_attribute_for_subphone --------*/
STATIC mre_status_t device_config_get_attribute_for_subphone(
  const policyman_xml_element_t  *pChild,
  size_t                          nSim,
  const char                    **ppStr
)
{
  mre_status_t   status = MRE_STATUS_SUCCESS;

  if (nSim < 1)
  {
    *ppStr = policyman_xml_get_attribute(pChild, "primary");
  }
  else
  {
    *ppStr = policyman_xml_get_attribute(pChild, "secondary");
  }

  if (*ppStr == NULL)
  {
    POLICYMAN_MSG_ERROR_0("no primary or secondary attribute present in XML");
    status = MRE_STATUS_ERR_INVALID_ACTION;
  }

  return status;
}

/*-------- device_config_read_e911_ratMask --------*/
STATIC mre_status_t device_config_read_e911_ratMask(
  device_config_action_t        *pAction,
  const policyman_xml_element_t *pChild,
  size_t                         nCfg,
  size_t                         nSim
)
{
  rat_capability_info_t       *pSubPhoneInfo;
  mre_status_t                 status;
  boolean                      haveMask      = FALSE;
  const char                  *pStr          = NULL;

  status = device_config_get_attribute_for_subphone(pChild, nSim, &pStr);
  if (MRE_SUCCEEDED(status))
  {
    pSubPhoneInfo = device_config_get_e911_info(pAction, nCfg, nSim);
    haveMask = device_config_read_mask(pSubPhoneInfo, pStr);

    if (haveMask)
    {
      POLICYMAN_MSG_HIGH_2("read subphone[%d] e911 mask = 0x%04x", nSim, pSubPhoneInfo->includeMask);
      status = MRE_STATUS_SUCCESS;
    }
  }

  return status;
}

/*-------- device_config_read_subphone_ratMask --------*/
STATIC mre_status_t device_config_read_subphone_ratMask(
  subs_info_action_t            *pSubPhoneInfo,
  const policyman_xml_element_t *pChild,
  size_t                         nSim
)
{
  mre_status_t   status;
  boolean        haveMask   = FALSE;
  const char    *pStr       = NULL;

  status = device_config_get_attribute_for_subphone(pChild, nSim, &pStr);
  if (MRE_SUCCEEDED(status))
  {
    haveMask = device_config_read_mask(&pSubPhoneInfo->ratInfo, pStr);

    if (haveMask)
    {
      POLICYMAN_MSG_HIGH_2("read subphone[%d] capability 0x%04x", nSim, pSubPhoneInfo->ratInfo.includeMask);
      status = MRE_STATUS_SUCCESS;
    }
  }

  return status;
}


/*-------- policyman_device_config_get_subs_info --------*/
STATIC mre_status_t policyman_device_config_get_subs_info(
  device_config_action_t        *pAction,
  policyman_xml_element_t const *pElem,
  size_t                         nCfg
)
{
  mre_status_t         status = MRE_STATUS_ERR_INVALID_ACTION;
  size_t               subsId;
  subs_info_action_t  *pSubsInfo;
  const char          *pStr;

  pStr = policyman_xml_get_attribute(pElem, "id");
  if (pStr == NULL)
  {
    goto Done;
  }

  subsId = atoi(pStr);
  if (subsId > pAction->num_sims)
  {
    POLICYMAN_MSG_ERROR_2( "device_configuration: config has a subs id of %d, but there are only %d SIMs",
                           subsId,
                          pAction->num_sims );
    goto Done;
  }

  /* get subphone info */
  pSubsInfo = device_config_action_get_subphone(pAction, nCfg, (subsId - 1));

  /*  Get any feature mode associated with this subs - default is "normal".
   */
  pSubsInfo->feature = SYS_SUBS_FEATURE_MODE_NORMAL;
  pStr = policyman_xml_get_attribute(pElem, "feature");
  if (pStr != NULL)
  {
    POLICYMAN_MSG_ERROR_0("Setting features in <device_configuration> is deprecated");

    if (!policyman_device_config_get_feature(pStr, &pSubsInfo->feature))
    {
      goto Done;
    }

    pAction->modified |= 0;
  }

  pElem = policyman_xml_get_child(pElem, 0);
  if (pElem == NULL)
  {
    goto Done;
  }

  status = policyman_rat_capability_read_info( pElem,
                                               &pSubsInfo->ratInfo );

Done:
  return status;
}

/*-------- read_device_config_from_inline_configs --------*/
STATIC mre_status_t read_device_config_from_inline_configs(
  device_config_action_t        *pAction,
  const policyman_xml_element_t *pChild,
  size_t                         nCfg
)
{
  mre_status_t  status   = MRE_STATUS_SUCCESS;
  size_t        nSim;

  for (nSim = 0; nSim < pAction->num_sims; nSim++)
  {
    subs_info_action_t *pSubPhoneInfo;

    pSubPhoneInfo = device_config_action_get_subphone(pAction, nCfg, nSim);

    pSubPhoneInfo->feature = SYS_SUBS_FEATURE_MODE_NORMAL;
    status = device_config_read_subphone_ratMask(pSubPhoneInfo, pChild, nSim);
  }

  return status;
}

/*-------- read_device_config_from_expanded_configs --------*/
STATIC mre_status_t read_device_config_from_expanded_configs(
  device_config_action_t        *pAction,
  const policyman_xml_element_t *pChild,
  size_t                         nCfg
)
{
  mre_status_t  status = MRE_STATUS_FAILED;
  size_t        i;
  size_t        nSIMs;

  /* Get number of children to detect if this is old style config */
  nSIMs = policyman_xml_num_children(pChild);
  if (nSIMs != pAction->num_sims)
  {
    POLICYMAN_UT_MSG_3( "device_configuration: number of SIMs is %d but configuration %d has %d elements",
                        pAction->num_sims,
                        nCfg,
                        nSIMs );
    goto Done;
  }

  status = MRE_STATUS_SUCCESS;
  for (i = 0 ; !MRE_FAILED(status) && i < nSIMs ; ++i)
  {
    const policyman_xml_element_t *pSubs = policyman_xml_get_child(pChild, i);

    if (pSubs == NULL)
    {
      status = MRE_STATUS_ERR_NOT_PRESENT;
      break;
    }

    if (!policyman_xml_tag_is(pSubs, "subs"))
    {
      POLICYMAN_UT_MSG_0("device_configuration: top level elements of 'config' must be 'subs'");
      goto Done;
    }

    status = policyman_device_config_get_subs_info(pAction, pSubs, nCfg);
  }

Done:
   return status;
}

/*-------- policyman_device_config_read_cfg --------*/
STATIC mre_status_t policyman_device_config_read_cfg(
  device_config_action_t        *pAction,
  const policyman_xml_element_t *pChild,
  size_t                         nCfg
)
{
  mre_status_t  status;

  /* Read the subphone info from nested subs_id tags first. If not available
       then look for feature and Rat mask attributes on primary and secondary subphones
  */
  status = read_device_config_from_expanded_configs(pAction, pChild, nCfg);
  if (MRE_FAILED(status))
  {
    status = read_device_config_from_inline_configs(pAction, pChild, nCfg);
  }

  return status;
}

/*-------- policyman_device_config_read_e911_config --------*/
STATIC mre_status_t policyman_device_config_read_e911_config(
  device_config_action_t        *pAction,
  const policyman_xml_element_t *pChild,
  size_t                         nCfg
)
  {
  mre_status_t  status   = MRE_STATUS_SUCCESS;
  size_t        nSim;

  for (nSim = 0; nSim < pAction->num_sims; nSim++)
  {
    status = device_config_read_e911_ratMask(pAction, pChild, nCfg, nSim);
  }

  return status;
}

/*-------- policyman_device_config_get_num_active_voice --------*/
STATIC boolean policyman_device_config_get_num_active_voice(
  size_t                        *pnActive,
  policyman_xml_element_t const *pCfg
)
{
  const char  *pStr;
  boolean     succeeded = FALSE;

  /* max_active is not a mandatory parameter */
  pStr = policyman_xml_get_attribute(pCfg, "max_active");

  if (pStr == NULL)
  {
    pStr = policyman_xml_get_attribute(pCfg, "max_active_voice");
  }

  succeeded = (pStr != NULL);

  if (succeeded)
  {
    *pnActive= atoi(pStr);
  }
  else
  {
    POLICYMAN_UT_MSG_0("<device_configuration> max_active/max_active_voice attribute is not present");
  }

  return succeeded;
}

/*-------- policyman_device_config_get_num_active_data --------*/
STATIC boolean policyman_device_config_get_num_active_data(
  size_t                        *pnActiveData,
  policyman_xml_element_t const *pCfg
)
{
  const char  *pStr;
  boolean     succeeded = FALSE;

  pStr = policyman_xml_get_attribute(pCfg, "max_active_data");
  succeeded = (pStr != NULL);

  if (succeeded)
  {
    *pnActiveData= atoi(pStr);
  }
  else
  {
    POLICYMAN_MSG_HIGH_0("<device_configuration> max_active_data attribute is not present");
  }

  return succeeded;
}


/*-------- policyman_device_config_get_num_sims --------*/
STATIC boolean policyman_device_config_get_num_sims(
  size_t                        *pnSims,
  const policyman_xml_element_t *pElem
)
{
  const char  *pStr;
  boolean     succeeded = FALSE;

  /* num_sims is a mandatory parameter */
  pStr = policyman_xml_get_attribute(pElem, "num_sims");
  succeeded = (pStr != NULL);

  if (succeeded)
  {
    *pnSims = atoi(pStr);
  }
  else
  {
    POLICYMAN_MSG_ERROR_0("<device_configuration> requires a num_sims attribute");
  }

  return succeeded;
}


/*-------- policyman_device_config_get_special --------*/
STATIC boolean policyman_device_config_get_special(
  const char            *pStr,
  sys_specialization_t  *pSpecialization
)
{
  boolean     succeeded = TRUE;
  size_t      i;

  typedef struct
  {
    char const            *pName;
    sys_specialization_t  specialization;
  } special_map_t;

  special_map_t special_map[] =
  {
    {"none",                  SYS_SPECIAL_NONE},
    {"sbm_rat_search_order",  SYS_SPECIAL_SBM_RAT_SEARCH_ORDER},
    {"slte",                  SYS_SPECIAL_SLTE},
    {"dsda_roam",             SYS_SPECIAL_DSDA_ROAM},
    {"dsds_roam",             SYS_SPECIAL_DSDS_ROAM},
    {"sbm_periodic_search",   SYS_SPECIAL_SBM_PERIODIC_SEARCH},
    {"volte_e911_calls",      SYS_SPECIAL_VOLTE_E911_CALLS},
  };

  if (pStr == NULL)
  {
    *pSpecialization = SYS_SPECIAL_NONE;
    goto Done;
  }

  for (i = 0 ; i < ARR_SIZE(special_map) ; ++i)
  {
    if (strcasecmp(pStr, special_map[i].pName) == 0)
    {
      *pSpecialization = special_map[i].specialization;
      goto Done;
    }
  }

  POLICYMAN_UT_MSG_S1("specialization attribute of <device_configuration> invalid: %s", pStr);
  succeeded = FALSE;

Done:
  return succeeded;
}

/*-------- policyman_update_configs_for_action_execute --------*/
STATIC void policyman_update_configs_for_action_execute(
  device_configuration_t  *pdc,
  device_config_action_t  *pdcAction,
  policyman_state_t       *pState,
  policyman_set_t         *pItemSet
)
{
  size_t                 index;
  policy_execute_ctx_t   rcCtx;
  sys_modem_as_id_e_type ddsSubs;

  /* add entries to the list */
  pdc->num_cfgs           = pdcAction->num_cfgs;
  rcCtx.pState            = pState;
  rcCtx.pItemSet          = pItemSet;
  rcCtx.asubs_id          = SYS_MODEM_AS_ID_1;

  /* delete the previous configs[] array and create a new one */
  policyman_mem_free(pdc->pConfigs);
  pdc->pConfigs = (subs_info_t *)policyman_mem_alloc(sizeof(subs_info_t)* pdcAction->num_cfgs * pdcAction->num_sims);

  for (index = 0; index < pdcAction->num_sims * pdcAction->num_cfgs; index++)
  {
    pdc->pConfigs[index].feature = pdcAction->subs_info[index].feature;
    pdc->pConfigs[index].ratInfo = policyman_rat_capability_evaluate( &pdcAction->subs_info[index].ratInfo,
                                                                     &rcCtx );
  }

  /* update current index as per the DDS subs */
  ddsSubs = policyman_subs_get_dds_subs(pState);
  (void)policyman_update_current_cfg_index( pdc, ddsSubs);

  POLICYMAN_MSG_HIGH_2( "<device_configuration>: updated %d config entries with current index %d",
                        pdcAction->num_cfgs,
                        pdc->current_cfg_index );
}

/*-------- policyman_get_num_config_from_all_configs --------*/
STATIC void policyman_get_num_config_from_all_configs(
  mre_xml_element_t const *pElem,
  size_t                   totalCfgs,
  size_t                  *pNcfgs
)
{
  size_t                         index;
  const policyman_xml_element_t *pChild;

  /* loop through all configs to figure out which are subphone capabilities
       and which ones are e911 capabilities
   */
  for (index=0; index < totalCfgs; index++)
  {
    pChild = policyman_xml_get_child(pElem, index);
    if (pChild != NULL && policyman_xml_tag_is(pChild, "config"))
    {
      (*pNcfgs)++;
    }
  }
}

/*-------- device_config_read_values_from_mask --------*/
STATIC void device_config_read_values_from_mask(
  device_config_action_t *pdcAction,
  policyman_state_t      *pState,
  policyman_set_t        *pItemSet,
  device_configuration_t *pdc
)
{
  if (pdcAction->parameterMask & PARAMETER_MASK_NMAXVOICE)
  {
    pdc->max_active_voice = pdcAction->max_active_voice;
  }

  if (pdcAction->parameterMask & PARAMETER_MASK_NMAXDATA)
  {
    pdc->max_active_data = pdcAction->max_active_data;
  }

  if (pdcAction->parameterMask & PARAMETER_MASK_SPECIALIZATION)
  {
    pdc->specialization = pdcAction->specialization;
  }

  /* Update config always from XML */
  if (pdcAction->parameterMask & PARAMETER_MASK_CONFIG)
  {
    /* See if config already exists in list of supported configs, update the index accordingly
      */
    if (pdc->num_cfgs > 0)
    {
      POLICYMAN_MSG_ERROR_1("device config update is not allowed, already having %d configs", pdc->num_cfgs);
    }
    else
    {
      policyman_update_configs_for_action_execute(pdc, pdcAction, pState, pItemSet);
    }
  }
}

/*-------- read_e911_config_from_action_includeMask --------*/
STATIC void read_e911_config_from_action_includeMask(
  device_config_action_t *pdcAction,
  device_configuration_t *pdc
)
{
  size_t index;

  for (index=0; index < pdc->num_sims * pdc->e911_num_cfgs; index++)
  {
    pdc->pECallRatCfg[index] = pdcAction->pECallRatCfg[index].includeMask;
  }
}

/*-------- device_config_read_e911_config_from_action --------*/
STATIC void device_config_read_e911_config_from_action(
  device_config_action_t *pdcAction,
  device_configuration_t *pdc
)
{
  size_t               e911CfgSize  = 0;

  /* Don't allow update if we already have more entries in e911 configs[]. */
  if (pdc->e911_num_cfgs > 0)
  {
    POLICYMAN_MSG_HIGH_0("current e911 config already has entries, not updating from Action");
    return;
  }

  /* Free the already allocated array first. */
  pdc->e911_num_cfgs = pdcAction->e911_num_cfg;
  policyman_mem_free(pdc->pECallRatCfg);

  if (pdc->e911_num_cfgs > 0)
  {
    e911CfgSize        = sizeof(uint32)* pdc->num_sims * pdc->e911_num_cfgs;
    pdc->pECallRatCfg  = (uint32 *)policyman_mem_alloc(e911CfgSize);

    read_e911_config_from_action_includeMask(pdcAction, pdc);
  }
}

/*-------- policyman_device_config_create --------*/
STATIC device_configuration_t *policyman_device_config_create(
  device_config_action_t  *pdcAction,
  policyman_state_t       *pState,
  policyman_set_t         *pItemSet
)
{
  device_configuration_t   *pdc;
  device_configuration_t   *pCurrentDc   = NULL;
  policyman_status_t        status;

  POLICYMAN_MSG_HIGH_1(SRC_LINE "action <device_configuration> executing", LINE_NUM(pdcAction));

  /* get device config from pItemSet*/
  {
   (void)policyman_cfgitem_is_present_in_itemset( pItemSet,
                                                  POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                  SYS_MODEM_AS_ID_1,
                                                  (policyman_item_t **)&pCurrentDc);

     if(NULL != pCurrentDc) {ref_cnt_obj_add_ref((void *) pCurrentDc);}
  }   
  /* if not present in pItemSet, get from database*/ 
  if(NULL == pCurrentDc)
  {
    (void)policyman_get_current_device_config((policyman_item_t **)&pCurrentDc);
  }

  /* determine the size of new device config item and create it */
  pdc = (device_configuration_t *)policyman_device_config_item_new();

  /* max_active_data defaults to 1 */
  pdc->max_active_data = 1;
  pdc->explicit_index = -1;
  pdc->hardware_mask   = policyman_rf_get_overall_rat_mask();

  /* restore all parameters from existing device config */
  if (NULL != pCurrentDc)
  {
    device_config_copy_parameters((device_configuration_t const *)pCurrentDc, pdc);
  }

  /* Read all mandatory and optional parameters one by one */
  pdc->num_sims = pdcAction->num_sims;
  g_num_Sim     = pdc->num_sims;

  /* Determine the maximum active voice subs ever possible */
  if (pdc->max_active_voice > g_maxActiveVoiceSubs)
  {
    g_maxActiveVoiceSubs = pdc->max_active_voice;
  }

  device_config_read_values_from_mask(pdcAction, pState, pItemSet, pdc);

  /* Read e911 config next */
  device_config_read_e911_config_from_action(pdcAction, pdc);

  REF_CNT_OBJ_RELEASE_IF(pCurrentDc);
  POLICYMAN_MSG_HIGH_1(SRC_LINE "<device_configuration> item created", LINE_NUM(pdcAction));

  return pdc;
}

/*-------- policyman_device_config_execute --------*/
STATIC boolean policyman_device_config_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  device_config_action_t  *pdcAction = (device_config_action_t *) pAction;
  policy_execute_ctx_t    *pdcCtx    = (policy_execute_ctx_t *) pCtx;
  device_configuration_t  *pdc       = NULL;


  pdc = policyman_device_config_create(pdcAction, pdcCtx->pState, pdcCtx->pItemSet);
  policyman_set_replace(pdcCtx->pItemSet, &pdc);
  ref_cnt_obj_release(pdc);

  return TRUE;
}


/*-------- policyman_device_config_present --------*/
STATIC boolean policyman_device_config_present(
  void
)
{
  boolean                 isPresent;
  device_configuration_t  *pdc;

  pdc = (device_configuration_t *) policyman_cfgitem_get_item( POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                               SYS_MODEM_AS_ID_1 );

  isPresent = (pdc != NULL && (pdc->modified & DEV_CONFIG_NUM_SIMS_MODIFIED) == 0);

  REF_CNT_OBJ_RELEASE_IF(pdc);

  return isPresent;
}


/*-------- policyman_device_config_if_execute --------*/
STATIC boolean policyman_device_config_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean                  succeeded = FALSE;
  policyman_status_t       status;
  policy_execute_ctx_t    *pGetCtx   = (policy_execute_ctx_t *) pCtx;
  device_configuration_t  *pdcItem   = NULL;
  boolean                  item_is_present;

  item_is_present = policyman_cfgitem_is_present_in_itemset( pGetCtx->pItemSet,
                                                             POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                             SYS_MODEM_AS_ID_1,
                                                             (policyman_item_t **)&pdcItem);

  if (item_is_present)
  {
    if( (pdcItem)->num_cfgs > 0)
    {
      succeeded = TRUE;
      POLICYMAN_UT_MSG_1(SRC_LINE "<device_config_if> skipping update - item present", LINE_NUM(pAction));
    }
  }
  else
  {
    succeeded = policyman_device_config_execute(pAction, pCtx);
  }

  return succeeded;
}


/*-------- policyman_device_config_action_dtor --------*/
STATIC void policyman_device_config_action_dtor(
  void  *pObj
)
{
  device_config_action_t  *pMe = (device_config_action_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pMe->pConfigName);
  POLICYMAN_MEM_FREE_IF(pMe->pECallRatCfg);
  policyman_action_dtor(pObj);
}

/*-------- device_config_create_e911_config --------*/
STATIC void device_config_create_e911_config_array(
 policyman_xml_element_t const  *pElem,
 policyman_policy_t             *pPolicy,
 device_config_action_t        **ppAction,
 size_t                          nSims,
 size_t                         *pnCfgs,
 size_t                         *ptotalCfgs
)
{
  size_t                  nCfgs;
  size_t                  dcSize;
  size_t                  e911cfgSize;
  device_config_action_t *pAction;

  *ptotalCfgs = policyman_xml_num_children(pElem);
  policyman_get_num_config_from_all_configs(pElem, *ptotalCfgs, pnCfgs);
  POLICYMAN_UT_MSG_2("--------------- read totalCfgs = %d, ncfgs = %d", *ptotalCfgs, *pnCfgs);

  nCfgs = *pnCfgs;

  /*  Allocate the action.
        Get device configuration size from nSims, nCfgs and e911 configs
   */
  dcSize =  sizeof(device_config_action_t)
          + sizeof(subs_info_action_t)* nSims * nCfgs;

  pAction = (device_config_action_t *)policyman_action_new( dcSize,
                                                            (mre_policy_t *)pPolicy,
                                                            NULL,  // No execute
                                                            policyman_device_config_action_dtor,
                                                            SUBS_THIS,
                                                            pElem );

  pAction->pECallRatCfg = NULL;
  pAction->e911_num_cfg = *ptotalCfgs - nCfgs;
  e911cfgSize           = sizeof(rat_capability_info_t) * nSims * pAction->e911_num_cfg;

  if (pAction->e911_num_cfg > 0)
  {
    pAction->pECallRatCfg = (rat_capability_info_t *)policyman_mem_alloc(e911cfgSize);
  }

  *ppAction = pAction;
}

/*-------- device_config_read_e911_and_subphone_configs --------*/
STATIC mre_status_t device_config_read_e911_and_subphone_configs(
 policyman_xml_element_t const  *pElem,
 device_config_action_t         *pAction,
 size_t                          totalCfgs
)
{
  size_t        i;
  size_t        nCfgs;
  mre_status_t  status  = MRE_STATUS_SUCCESS;

  /* Loop through all configs and read subphone configs and e911 configs */
  for (i = 0, nCfgs = 0; MRE_SUCCEEDED(status) && i < totalCfgs ; ++i)
  {
    const policyman_xml_element_t *pChild = policyman_xml_get_child(pElem, i);

    if (NULL == pChild)
    {
      continue;
    }

    if (policyman_xml_tag_is(pChild, "config"))
    {
     /* read subphone config */
      pAction->parameterMask |= PARAMETER_MASK_CONFIG;
      status = policyman_device_config_read_cfg(pAction, pChild, nCfgs);
      ++nCfgs;
    }
    else if (pChild != NULL && policyman_xml_tag_is(pChild, "e911_config"))
    {
      /* read e911 config : Maximum 2 configs are allowed */
      status = policyman_device_config_read_e911_config(pAction, pChild, (i - nCfgs));
    }
  }

  return status;
}


/*-------- policyman_device_config_create_new_action --------*/
mre_status_t policyman_device_config_create_new_action(
  policyman_xml_element_t const *pElem,
  policyman_policy_t            *pPolicy,
  mre_action_t            **ppAction,
  pfn_execute_t                   executefn
)
{
  mre_status_t             status         = MRE_STATUS_ERR_INVALID_ACTION;
  device_config_action_t  *pAction        = NULL;
  size_t                   nSims          = 1;
  size_t                   nMaxVoice      = 1;
  size_t                   nMaxData       = 1;
  size_t                   nCfgs          = 0;
  size_t                   totalCfgs      = 0;
  sys_specialization_t     specialization = SYS_SPECIAL_NONE;
  uint32                   parameterMask  = 0;
  const char              *pStr;

  POLICYMAN_UT_MSG_0("--------------- <device_configuration> Parse the action");

  /*  Get the number of SIM slots, simultaneously active subscriptions, and specialization.
   */
  if (!policyman_device_config_get_num_sims(&nSims, pElem))
  {
    goto Done;
  }

  if (policyman_device_config_get_num_active_voice(&nMaxVoice, pElem))
  {
    parameterMask |= PARAMETER_MASK_NMAXVOICE;

    /* Determine the maximum active voice subs ever possible */
    if (nMaxVoice > g_maxActiveVoiceSubs)
    {
      g_maxActiveVoiceSubs = nMaxVoice;
    }
  }

  if (policyman_device_config_get_num_active_data(&nMaxData, pElem))
  {
    parameterMask |= PARAMETER_MASK_NMAXDATA;
  }

  /*  Deal with the specialization, which should no longer be set here */
  pStr = policyman_xml_get_attribute(pElem, "specialization");
  if (pStr != NULL)
  {
    POLICYMAN_MSG_ERROR_0("'specialization' attribute of <device_configuration> is deprecated");
    if (policyman_device_config_get_special(pStr, &specialization))
    {
      parameterMask |= PARAMETER_MASK_SPECIALIZATION;
    }
  }

  device_config_create_e911_config_array(pElem, pPolicy, &pAction, nSims, &nCfgs, &totalCfgs);

  /*  Initialize the action. */
  POLICYMAN_MSG_HIGH_0("<device_configuration> Initialize the action");

  pAction->execute          = executefn;
  pAction->num_sims         = nSims;
  pAction->max_active_voice = nMaxVoice;
  pAction->max_active_data  = nMaxData;
  pAction->num_cfgs         = nCfgs;
  pAction->parameterMask    = parameterMask;
  pAction->specialization   = specialization;

  status = device_config_read_e911_and_subphone_configs(pElem, pAction, totalCfgs);

Done:
  if (MRE_FAILED(status))
  {
    POLICYMAN_MSG_ERROR_0("<device_configuration> read config failed");
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }

  *ppAction = (mre_action_t *) pAction;

  return status;
}


/*-------- policyman_device_config_action_new --------*/
mre_status_t policyman_device_config_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_device_config_create_new_action( pElem,
                                                    (policyman_policy_t *) pPolicy,
                                                    ppAction,
                                                    policyman_device_config_execute);
}


/*-------- policyman_device_config_if_action_new --------*/
mre_status_t policyman_device_config_if_action_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  return policyman_device_config_create_new_action( pElem,
                                                    (policyman_policy_t *) pPolicy,
                                                    ppAction,
                                                    policyman_device_config_if_execute);
}


/*=============================================================================
  <define_config>
=============================================================================*/


/*-------- named_device_configuration_t --------*/
typedef struct
{
  POLICYMAN_NAMED_OBJECT_BASE;

  device_configuration_t  *pdc;
} named_device_config_t;


/*-------- policyman_named_config_dtor --------*/
STATIC void policyman_named_config_dtor(
  void  *pObj
)
{
  named_device_config_t *pMe = (named_device_config_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pMe->pdc);
  policyman_named_object_dtor(pObj);
}


/*-------- policyman_device_config_define_execute --------*/
STATIC boolean policyman_device_config_define_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean                  succeeded;
  device_config_action_t  *pdcAction    = (device_config_action_t *) pAction;
  policy_execute_ctx_t    *pdcCtx       = (policy_execute_ctx_t *) pCtx;
  device_configuration_t  *pdc          = policyman_device_config_create(pdcAction, pdcCtx->pState, pdcCtx->pItemSet);
  named_device_config_t   *pNamedConfig;

  pNamedConfig = (named_device_config_t *) policyman_mem_alloc(sizeof(named_device_config_t));
  ref_cnt_obj_init(pNamedConfig, policyman_named_config_dtor);

  policyman_named_object_init( (policyman_named_object_t *)pNamedConfig,
                               pdcAction->pConfigName,
                               POLICYMAN_NAMED_CONFIG );
  pNamedConfig->pdc = pdc;

  succeeded = mre_named_object_add( (policyman_named_object_t *)pNamedConfig,
                                     MRE_NAMESPACE_SUBS,
                                    (mre_policy_t *) pdcAction->pPolicy );

  ref_cnt_obj_release(pNamedConfig);

  if (succeeded)
  {
    POLICYMAN_MSG_HIGH_1(SRC_LINE "<define_config>: named configuration successfully created", LINE_NUM(pdcAction));
    POLICYMAN_UT_MSG_S1("   name = %s", pNamedConfig->pName);
  }

  return succeeded;
}


/*-------- policyman_device_config_define_new --------*/
mre_status_t policyman_device_config_define_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t            status;
  device_config_action_t  *pAction;
  char const              *pStr;

  status = policyman_device_config_create_new_action( pElem,
                                                      (policyman_policy_t *) pPolicy,
                                                      ppAction,
                                                      policyman_device_config_define_execute);

  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pStr = policyman_xml_get_attribute(pElem, "name");
  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_0("<define_config> requires a \"name\" attribute");
    status = MRE_STATUS_ERR_INVALID_ACTION;
    goto Done;
  }

  pAction = (device_config_action_t *) *ppAction;
  pAction->pConfigName = policyman_str_dup(pStr);

Done:
  if (MRE_FAILED(status))
  {
    REF_CNT_OBJ_RELEASE_IF(*ppAction);
  }

  return status;
}


/*=============================================================================
  <use_config>
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  char const              *pConfigName;
  device_configuration_t  *pConfig;
} use_config_action_t;

/*-------- policyman_use_config_action_dtor --------*/
STATIC void policyman_use_config_action_dtor(
  void  *pObj
)
{
  use_config_action_t *pMe = (use_config_action_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pMe->pConfigName);
  REF_CNT_OBJ_RELEASE_IF(pMe->pConfig);
  policyman_action_dtor(pObj);
}

/*-------- policyman_use_config_find_config --------*/
STATIC device_configuration_t *policyman_use_config_find_config(
  mre_action_t const  *pAction
)
{
  use_config_action_t     *pCfgAct   = (use_config_action_t *)pAction;
  device_configuration_t  *pConfig   = NULL;
  named_device_config_t   *pNamedCfg;

  /*  If there is a name in the configuration item, look up the object
   */
  if (pCfgAct->pConfigName != NULL)
  {
    pNamedCfg = (named_device_config_t *) mre_named_object_find( pCfgAct->pConfigName,
                                                                 POLICYMAN_NAMED_CONFIG,
                                                                 pAction->pPolicy );

    if (pNamedCfg == NULL)
    {
      POLICYMAN_MSG_ERROR_0("<use_config>: unable to find named configuration");
      goto Done;
    }

    if (pNamedCfg->pdc == NULL)
    {
      POLICYMAN_MSG_ERROR_0("<use_config>: named configuration has no config item");
      goto Done;
    }

    pCfgAct->pConfig = pNamedCfg->pdc;
    ref_cnt_obj_add_ref(pCfgAct->pConfig);

    policyman_mem_free((void *) pCfgAct->pConfigName);
    pCfgAct->pConfigName = NULL;
  }

  pConfig = pCfgAct->pConfig;

Done:
  return pConfig;
}

/*-------- policyman_use_config_copy_configs --------*/
STATIC void policyman_use_config_copy_configs(
  mre_action_t const     *pAction,
  device_configuration_t *pdc
)
{
  device_configuration_t  *pConfig;

  pConfig = policyman_use_config_find_config(pAction);
  if (pConfig == NULL || pdc == NULL)
  {
    POLICYMAN_MSG_ERROR_0("<use_config>: no configuration to copy configs");
    return;
  }

  device_config_copy_configs((device_configuration_t const *)pConfig, pdc);
}

/*-------- policyman_device_config_use_execute --------*/
STATIC boolean policyman_device_config_use_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean                  succeeded = FALSE;
  device_configuration_t  *pConfig;
  device_configuration_t  *pdc;
  policy_execute_ctx_t    *pDcCtx = (policy_execute_ctx_t *)pCtx;

  pConfig = policyman_use_config_find_config(pAction);

  if (pConfig == NULL)
  {
    POLICYMAN_MSG_ERROR_1(SRC_LINE "<use_config>: no configuration", LINE_NUM(pAction));
    goto Done;
  }

  /* ensure subphone and e911 configs are filled properly in named config */
  policyman_check_and_update_configurations(pConfig);

  /* create a new device config with copying all configs and e911 configs  */
  pdc = policyman_device_config_item_new();
  device_config_copy_parameters(pConfig, pdc);

  policyman_set_replace(pDcCtx->pItemSet, &pdc);
  POLICYMAN_MSG_HIGH_4( SRC_LINE "<use_config>: configuration with num_sims %d, max_active_voice %d, max_active_data %d",
                        LINE_NUM(pAction),
                        pdc->num_sims,
                        pdc->max_active_voice,
                        pdc->max_active_data );

  ref_cnt_obj_release(pdc);
  succeeded = TRUE;

Done:
  return succeeded;
}

/*-------- policyman_device_config_use_if_execute --------*/
STATIC boolean policyman_device_config_use_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean                  succeeded  = TRUE;
  device_configuration_t  *pDevCfg;
  policy_execute_ctx_t    *pGetCtx    = (policy_execute_ctx_t *) pCtx;
  boolean                  item_is_present;

  item_is_present = policyman_cfgitem_is_present_in_itemset( (policyman_set_t *)pGetCtx->pItemSet,
                                                              POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                              SYS_MODEM_AS_ID_1,
                                                              (policyman_item_t **)&pDevCfg);


  if (item_is_present)
  {
    POLICYMAN_UT_MSG_1(SRC_LINE "<use_config_if> item already present, copy configs", LINE_NUM(pAction));
    policyman_use_config_copy_configs(pAction, pDevCfg);
  }
  else
  {
    succeeded = policyman_device_config_use_execute(pAction, pCtx);
  }

  return succeeded;
}


/*-------- policyman_device_config_use_create --------*/
mre_status_t policyman_device_config_use_create(
  policyman_xml_element_t const  *pElem,
  policyman_policy_t             *pPolicy,
  mre_action_t                  **ppAction,
  pfn_execute_t                   executefn
)
{
  mre_status_t         status = MRE_STATUS_ERR_INVALID_ACTION;
  char const          *pStr;
  use_config_action_t *pAction;

  pStr = policyman_xml_get_attribute(pElem, "name");
  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_0("<use_config> requires a \"name\" attribute");
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( use_config_action_t,
                                  (mre_policy_t *)pPolicy,
                                  executefn,
                                  policyman_use_config_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->pConfigName = policyman_str_dup(pStr);

  *ppAction = (mre_action_t *) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- policyman_device_config_use_new --------*/
mre_status_t policyman_device_config_use_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
)
{
  return policyman_device_config_use_create( pElem,
                                             (policyman_policy_t *) pPolicy,
                                             ppAction,
                                             policyman_device_config_use_execute );
}


/*-------- policyman_device_config_use_if_new --------*/
mre_status_t policyman_device_config_use_if_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
)
{
  return policyman_device_config_use_create( pElem,
                                             (policyman_policy_t *) pPolicy,
                                             ppAction,
                                             policyman_device_config_use_if_execute );
}


/*=============================================================================
  <feature>

  NOTE:
  The <feature> action is only supported as part of the <initial> block, and
  sets the global feature and specialization states.  This doesn't result
  in a device_configuration item change, because that is not expected to change.
=============================================================================*/


typedef struct
{
  POLICYMAN_ACTION_BASE;

  boolean               ssFeatureIsPresent;
  sys_subs_feature_t    feature;
  sys_subs_feature_t    singleSimFeature;
} feature_action_t;


/*-------- execute_feature_mode --------*/
STATIC boolean execute_feature_mode(
  policyman_action_t const     *pAction,
  void                         *pCtx,
  sys_modem_as_id_e_type        subsId
)
{
  device_configuration_t  *pdc      = NULL;
  boolean                        item_is_present;

  /* We are reading feature here means it is not set in <device_config> item definition
        Update the SIM1 config with the feature if we have the subphone configs array.
  */
  item_is_present = policyman_cfgitem_is_present_in_itemset( ((policy_execute_ctx_t *)pCtx)->pItemSet,
                                                                               POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                             SYS_MODEM_AS_ID_1,
                                                             (policyman_item_t **)&pdc);

  if (pdc == NULL)
  {
    pdc =  (device_configuration_t *)policyman_cfgitem_get_device_config();
  }

  if (pdc != NULL)
  {
    size_t              cfgIndex;
    feature_action_t   *pfAction = (feature_action_t *) pAction;

    POLICYMAN_MSG_HIGH_4( SUBS_PREFIX SRC_LINE "updating feature to %d (ss feature %d)",
                          subsId,
                          LINE_NUM(pfAction),
                          pfAction->feature,
                           (pfAction->ssFeatureIsPresent ? pfAction->singleSimFeature : -1) );

    for (cfgIndex = 0; cfgIndex < pdc->num_cfgs; cfgIndex++)
    {
      size_t index;

      index = device_config_get_subphone_index(pdc->num_sims, cfgIndex, subsId);

      pdc->pConfigs[index].feature = pfAction->feature;

      // overwrite feature mode with single SIM feature if present
      if (pdc->num_sims == 1 && pfAction->ssFeatureIsPresent)
      {
        pdc->pConfigs[index].feature = pfAction->singleSimFeature;
      }
    }
  }

  return TRUE;
}

/*-------- policyman_device_config_feature_execute --------*/
STATIC boolean policyman_device_config_feature_execute(
  mre_action_t const        *pAction,
  void                      *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_feature_mode);
}


/*-------- policyman_device_config_feature_new --------*/
mre_status_t policyman_device_config_feature_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t          status = MRE_STATUS_ERR_MALFORMED_XML;
  char const            *pStr;
  sys_subs_feature_t    feature;
  feature_action_t      *pAction = NULL;
  policyman_subs_t       pm_subs;


  pStr = policyman_xml_get_text(pElem);
  if (!policyman_device_config_get_feature(pStr, &feature))
  {
    goto Done;
  }

  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = POLICYMAN_ACTION_NEW( feature_action_t,
                                  pPolicy,
                                  policyman_device_config_feature_execute,
                                  policyman_action_dtor,
                                  pm_subs,
                                  pElem );

  pAction->feature        = feature;
  pAction->singleSimFeature = SYS_SUBS_FEATURE_MODE_NORMAL;

  // read single sim feature
  pStr = policyman_xml_get_attribute(pElem, "single_sim");
  if (pStr != NULL)
  {
    pAction->ssFeatureIsPresent = policyman_device_config_get_feature(pStr, &pAction->singleSimFeature);
  }

  status = MRE_STATUS_SUCCESS;

Done:
  *ppAction = (mre_action_t *) pAction;
  return status;
}


/*=============================================================================
  <specialization>
=============================================================================*/


typedef struct
{
  POLICYMAN_ACTION_BASE;

  sys_specialization_t  specialization;
} specialization_action_t;


/*-------- policyman_device_config_specialization_execute --------*/
STATIC boolean policyman_device_config_specialization_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  specialization_action_t *pSAction = (specialization_action_t *)pAction;
  device_configuration_t  *pdc      = NULL;
  device_configuration_t **ppItem;

  // We are reading specialization here means it is not set in <device_config> item definition
  ppItem = (device_configuration_t **)policyman_cfgitem_find_item_in_itemset( ((policy_execute_ctx_t *)pCtx)->pItemSet,
                                                                               POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                                               SYS_MODEM_AS_ID_1 );

  // retrieve from database if not present in itemset
  pdc = (ppItem != NULL) ? *ppItem
                         : (device_configuration_t *)policyman_cfgitem_get_device_config();

  if (pdc != NULL)
  {   
    pdc->specialization = pSAction->specialization;
    POLICYMAN_MSG_HIGH_1("updating device specialization to %d", pSAction->specialization);
  }

  return TRUE;
}

/*-------- policyman_device_config_specialization_new --------*/
mre_status_t policyman_device_config_specialization_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t             status          = MRE_STATUS_ERR_MALFORMED_XML;
  char const              *pStr;
  sys_specialization_t     specialization  = SYS_SPECIAL_NONE;
  specialization_action_t *pAction         = NULL;

  pStr = policyman_xml_get_text(pElem);
  if (!policyman_device_config_get_special(pStr, &specialization))
  {
    goto Done;
  }

  pAction = (specialization_action_t *)mre_action_new( sizeof(*pAction),
                                                       pPolicy,
                                                       policyman_device_config_specialization_execute,
                                                       policyman_action_dtor,
                                                       pElem);

  pAction->specialization = specialization;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppAction = (mre_action_t *) pAction;
  return status;
}


/*=============================================================================
  <select_config>
=============================================================================*/


typedef struct
{
  POLICYMAN_ACTION_BASE;

  int  cfgIndex;
} select_config_action_t;

/*-------- policyman_device_config_select_config_execute --------*/
STATIC boolean policyman_device_config_select_config_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  select_config_action_t  *pScAction  = (select_config_action_t *)pAction;
  size_t                   numCfgs;

  numCfgs = device_config_get_num_configs();

  if (pScAction->cfgIndex < (int)numCfgs)
  {
    POLICYMAN_MSG_HIGH_2( SRC_LINE "setting explicit device config index to %d",
                          LINE_NUM(pScAction),
                          pScAction->cfgIndex );
    set_explicit_index(pCtx, pScAction->cfgIndex);
  }
  else
  {
    POLICYMAN_MSG_ERROR_3( SRC_LINE "explicit index %d exceeds number of configs (%d)",
                           LINE_NUM(pScAction),
                           pScAction->cfgIndex,
                           numCfgs );
  }

  return TRUE;
}

/*-------- policyman_device_config_select_config_new --------*/
mre_status_t policyman_device_config_select_config_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_action_t                  **ppAction
)
{
  mre_status_t             status      = MRE_STATUS_ERR_MALFORMED_XML;
  char const              *pStr;
  int                      cfgIndex;
  select_config_action_t  *pAction     = NULL;

  pStr = policyman_xml_get_attribute(pElem, "index");
  if (pStr == NULL)
  {
    POLICYMAN_MSG_ERROR_0("<select_config> requires a \"index\" attribute");
    goto Done;
  }

  cfgIndex = atoi(pStr);

  pAction = POLICYMAN_ACTION_NEW( select_config_action_t,
                                  pPolicy,
                                  policyman_device_config_select_config_execute,
                                  policyman_action_dtor,
                                  SUBS_THIS,
                                  pElem );

  pAction->cfgIndex = cfgIndex;

  status = MRE_STATUS_SUCCESS;

 Done:
   *ppAction = (mre_action_t *) pAction;
   return status;
}

/*-----------------------------------------------------------------------------
  <imei> condition
-----------------------------------------------------------------------------*/


/*-------- policyman_imei_condition_t --------*/
typedef struct
{
  POLICYMAN_CONDITION_BASE;

  mre_regex_t *pRegEx;
} policyman_imei_condition_t;


/*-------- policyman_condition_imei_dtor --------*/
STATIC void policyman_condition_imei_dtor(
  void  *pObj
)
{
  policyman_imei_condition_t *pCond = (policyman_imei_condition_t *) pObj;

  if (pCond->pRegEx != NULL)
  {
    ref_cnt_obj_release(pCond->pRegEx);
  }

  policyman_condition_dtor(pObj);
}


/*-------- policyman_device_imei_print --------*/
void policyman_device_imei_print(
  byte  *pImei
)
{
  POLICYMAN_MSG_HIGH_9( "Device IMEI:  %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                        pImei[0], pImei[1], pImei[2],
                        pImei[3], pImei[4], pImei[5],
                        pImei[6], pImei[7], pImei[8] );
}


/*-------- policyman_condition_imei_evaluate --------*/
STATIC boolean policyman_condition_imei_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_imei_condition_t  *pCond        = (policyman_imei_condition_t*)pCondition;
  byte                        *rawIMEI      = policyman_state_get_raw_imei();
  char const                  *pIMEI        = policyman_state_get_imei();
  boolean                     result        = FALSE;

  policyman_device_imei_print(rawIMEI);
  POLICYMAN_UT_MSG_S1("IMEI: %s", pIMEI);

  result =      pIMEI != NULL
            &&  mre_regex_match(pCond->pRegEx, pIMEI);

  POLICYMAN_MSG_HIGH_3( SUBS_PREFIX SRC_LINE "condition <imei> with above device IMEI returns %d",
                        CONDITION_SUBS(pCond),
                        LINE_NUM(pCond),
                        result );

  return result;
}


/*-------- policyman_condition_imei_new --------*/
mre_status_t policyman_condition_imei_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  policyman_status_t          status = POLICYMAN_STATUS_ERR_INVALID_CONDITION;
  policyman_imei_condition_t  *pCondition = NULL;
  char const                  *pStr;
  mre_regex_t                 *pRegEx;

  pStr = policyman_xml_get_attribute(pElem, "regex");
  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<imei>: must specify regex attribute");
    goto Done;
  }

  pRegEx = mre_regex_create(pStr);
  if (pRegEx == NULL)
  {
    POLICYMAN_UT_MSG_S1("<imei>: invalid regex: %s", pStr);
    goto Done;
  }

  /*  Allocate the condition
   */
  pCondition = POLICYMAN_CONDITION_NEW( policyman_imei_condition_t,
                                        pPolicy,
                                        policyman_condition_imei_evaluate,
                                        policyman_condition_imei_dtor,
                                        SUBS_THIS,
                                        pElem );

  pCondition->pRegEx = pRegEx;
  status = POLICYMAN_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;

  return status;
}

/*-------- policyman_device_config_allocate_and_init_diag_cfg_log --------*/
STATIC LOG_PM_CONFIG_INFO_type * policyman_device_config_allocate_and_init_diag_cfg_log(
  size_t    num_cfgs
)
{
  LOG_PM_CONFIG_INFO_type   *log_ptr;
  size_t                     log_size;

  log_size =  sizeof(LOG_PM_CONFIG_INFO_type);

  log_ptr  = (LOG_PM_CONFIG_INFO_type *)log_alloc_ex( LOG_PM_CONFIG_INFO_C,
                                                      log_size );

  if (NULL != log_ptr)
  {
    log_ptr->version      = LOG_PM_CONFIG_INFO_LOG_PACKET_VERSION;

    memset(&log_ptr->subs_cfg_info, 0, sizeof(log_ptr->subs_cfg_info) );
  }

  return log_ptr;
}

/*-------- policyman_device_config_fill_overall_info --------*/
STATIC void policyman_device_config_fill_overall_info(
  LOG_PM_CONFIG_INFO_type   *log_ptr
)
{
  policyman_rf_device_info_t    *pOverall;

  pOverall = policyman_rf_get_overall_info();

  if (NULL != pOverall)
  {
    log_ptr->hw_rat_mask     = pOverall->ratMask;
    log_ptr->hw_gw_bands     = pOverall->gwBands;
    log_ptr->lte_band_count  = sizeof(sys_lte_band_mask_e_type) / sizeof(uint64);
#if defined(LTE_BAND_NUM) && (LTE_BAND_NUM == 256)
    log_ptr->hw_lte_bands_0 = pOverall->lteBands.bits_1_64;
    log_ptr->hw_lte_bands_1 = pOverall->lteBands.bits_65_128;
    log_ptr->hw_lte_bands_2 = pOverall->lteBands.bits_129_192;
    log_ptr->hw_lte_bands_3 = pOverall->lteBands.bits_193_256;
#else
    log_ptr->hw_lte_bands_0  = pOverall->lteBands;
    log_ptr->hw_lte_bands_1 = 0;
    log_ptr->hw_lte_bands_2 = 0;
    log_ptr->hw_lte_bands_3 = 0;
#endif
    log_ptr->hw_tds_bands    = pOverall->tdsBands;
  }
  else
  {
    log_ptr->hw_rat_mask     = 0;
    log_ptr->hw_gw_bands     = 0;
    log_ptr->hw_lte_bands_0  = 0;
    log_ptr->hw_lte_bands_1  = 0;
    log_ptr->hw_lte_bands_2  = 0;
    log_ptr->hw_lte_bands_3  = 0;
    log_ptr->hw_tds_bands    = 0;
  }
}

/*-------- policyman_device_config_fill_config_info --------*/
STATIC void policyman_device_config_fill_config_info(
  device_configuration_t    *pdc,
  LOG_PM_CONFIG_INFO_type   *log_ptr
)
{
  size_t   cfgIdx;
  size_t   maxIdx;

  maxIdx = (pdc->num_cfgs > ARR_SIZE(log_ptr->subs_cfg_info)) ? ARR_SIZE(log_ptr->subs_cfg_info)
                                                              : pdc->num_cfgs;

  for (cfgIdx = 0; cfgIdx < maxIdx; ++cfgIdx)
  {
    log_ptr->subs_cfg_info[cfgIdx].subs_feature = (uint8)pdc->pConfigs[cfgIdx].feature;
    log_ptr->subs_cfg_info[cfgIdx].reserved_1   = 0;
    log_ptr->subs_cfg_info[cfgIdx].reserved_2   = 0;
    log_ptr->subs_cfg_info[cfgIdx].reserved_3   = 0;
    log_ptr->subs_cfg_info[cfgIdx].rat_info     = pdc->pConfigs[cfgIdx].ratInfo;
  }
}

/*-------- policyman_device_config_fill_diag_cfg_log --------*/
void policyman_device_config_fill_diag_cfg_log(
  device_configuration_t    *pdc,
  LOG_PM_CONFIG_INFO_type   *log_ptr
)
{
  log_ptr->version         = LOG_PM_CONFIG_INFO_LOG_PACKET_VERSION;
  log_ptr->changed_mask    = 0xFFFFFFFF;
  log_ptr->num_sim_cfg     = (pdc->num_cfgs & 0x0f) << 4;
  log_ptr->num_sim_cfg    |= (pdc->num_sims & 0x0f);
  log_ptr->max_active      = (pdc->max_active_data  & 0x0f) << 4;
  log_ptr->max_active     |= (pdc->max_active_voice & 0x0f);
  log_ptr->modified        = (uint8)pdc->modified;
  log_ptr->specialization  = (uint8)pdc->specialization;
}

/*-------- policyman_device_config_create_and_send_diag_cfg_log --------*/
void policyman_device_config_create_and_send_diag_cfg_log(
  policyman_item_t          *pCfgItem,
  int                        notifyHistoryIndex
)
{
  device_configuration_t    *pdc;
  LOG_PM_CONFIG_INFO_type   *log_ptr;

  pdc     = (device_configuration_t *)pCfgItem;

  log_ptr = policyman_device_config_allocate_and_init_diag_cfg_log(pdc->num_cfgs);

  if (NULL != log_ptr)
  {
    policyman_device_config_fill_diag_cfg_log(pdc, log_ptr);

    policyman_device_config_fill_overall_info(log_ptr);

    policyman_device_config_fill_config_info(pdc, log_ptr);

    policyman_cfgitem_update_notify_history_cfg(log_ptr, notifyHistoryIndex);

    // Submit the log to DIAG
    log_commit( (void *)log_ptr );
  }
}

/*-------- policyman_device_configuration_get_limited_item --------*/
void policyman_device_configuration_get_limited_item(
  mre_set_t             *pItemSet,
  sys_modem_as_id_e_type subsId
)
{
  policyman_item_t     *pDc;
  policy_execute_ctx_t  ctx;

  ctx.pItemSet = pItemSet;
  ctx.asubs_id = subsId;

  pDc = policyman_device_config_get_default(&ctx);

  mre_set_add(pItemSet, &pDc);
  ref_cnt_obj_release(pDc);
}

/*-------- policyman_device_config_reset_max_active_data --------*/
void policyman_device_config_reset_max_active_data(
  mre_set_t  *pItemSet
)
{
  device_configuration_t   *pdc = NULL;
  policyman_status_t   status = POLICYMAN_STATUS_SUCCESS;

  status = policyman_get_current_device_config((policyman_item_t **)&pdc);

  if (POLICYMAN_SUCCEEDED(status))
  {
    pdc->max_active_data = 1;
    mre_set_replace(pItemSet, &pdc);
  }

  REF_CNT_OBJ_RELEASE_IF(pdc);
}