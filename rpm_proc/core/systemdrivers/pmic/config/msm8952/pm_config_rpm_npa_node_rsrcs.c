/*! \file  pm_config_rpm_npa_node_rsrcs.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: MSM8952_PM8950_NPA_v1p00_2014_12_15 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/msm8952/pm_config_rpm_npa_node_rsrcs.c#1 $ 

===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pm_npa.h"
#include "pm_rpm_npa.h"
#include "pmapp_npa.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_LPDDR      "/node/pmic/client/lpddr"
/*===========================================================================

                        OTHER VARIABLES DEFINITION

===========================================================================*/

extern pm_npa_pam_client_cfg_type
pm_rpm_pam_lpddr_rails_info [];

/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* NPA NODE DEPENDENCY */
/* LPDDR Client */
static pm_rpm_local_resource_client_dependency 
pm_rpm_proc_client_dev_deps_lpddr [] =
{
   {RPM_LDO_A_REQ, 2, PM_RPM_NPA_CLIENT_LDO_REQ, 0 },
   {RPM_LDO_A_REQ, 3, PM_RPM_NPA_CLIENT_LDO_REQ, 0 },
};

static pm_rpm_resource_client_dependency_info
pm_rpm_proc_client_dev_deps_lpddr_info =
{
   NPA_ARRAY_SIZE(pm_rpm_proc_client_dev_deps_lpddr),
   pm_rpm_proc_client_dev_deps_lpddr
};

/* NPA NODE RESOURCE */
pm_npa_node_resource_info
pm_npa_rpm_pam_node_rsrcs[] =
{
   {
      PMIC_NPA_GROUP_ID_LPDDR,               // Resource Name
      PMIC_NPA_MODE_ID_DDR_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*)pm_rpm_pam_lpddr_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_LPDDR,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      (npa_user_data*)&pm_rpm_proc_client_dev_deps_lpddr_info, // Deps Count & Deps
      NULL,
   },
};

uint32 num_of_pm_rpm_nodes[] ={ 1 };

