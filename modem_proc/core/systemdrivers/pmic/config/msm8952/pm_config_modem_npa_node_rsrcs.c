/*! \file  pm_config_modem_npa_node_rsrcs.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: MSM8952_PM8950_NPA_v1p01_2014_12_29 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8952/pm_config_modem_npa_node_rsrcs.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa.h"
#include "pm_modem_proc_npa.h"
#include "pmapp_npa.h"
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_ANT1 "/node/pmic/client/ant1"
#define PMIC_NPA_CLIENT_NODE_PLL_SR2 "/node/pmic/client/pll_sr2"
#define PMIC_NPA_CLIENT_NODE_QFPROM "/node/pmic/client/qfprom"
#define PMIC_NPA_CLIENT_NODE_RAIL_CX "/node/pmic/client/rail_cx"
#define PMIC_NPA_CLIENT_NODE_RAIL_MX "/node/pmic/client/rail_mx"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_1X "/node/pmic/client/rf1_tech_1x"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS "/node/pmic/client/rf1_tech_gps"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS_ELNA "/node/pmic/client/rf1_tech_gps_elna"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE "/node/pmic/client/rf1_tech_lte"
#define PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY "/node/pmic/client/uim1_activity"
#define PMIC_NPA_CLIENT_NODE_UIM1_ENABLE "/node/pmic/client/uim1_enable"
#define PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY "/node/pmic/client/uim2_activity"
#define PMIC_NPA_CLIENT_NODE_UIM2_ENABLE "/node/pmic/client/uim2_enable"
/*===========================================================================

                        OTHER VARIABLES DEFINITION

===========================================================================*/

extern pm_npa_pam_client_cfg_type
pm_pam_ant1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_pll_sr2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_qfprom_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rail_cx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rail_mx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_1x_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gps_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gps_elna_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_lte_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim1_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim1_enable_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim2_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim2_enable_rails_info [];

/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/


/* NPA NODE DEPENDENCY */

/* ANT1 Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_ant1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* PLL_SR2 Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_pll_sr2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 7 ),
};

/* QFPROM Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_qfprom [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 6 ),
};

/* RAIL_CX Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rail_cx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 2 ),
};

/* RAIL_MX Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rail_mx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 3 ),
};

/* RF1_TECH_1X Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rf1_tech_1x [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 3 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 19 ),
};

/* RF1_TECH_GPS Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rf1_tech_gps [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 3 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 19 ),
};

/* RF1_TECH_GPS_ELNA Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rf1_tech_gps_elna [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 3 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 19 ),
};

/* RF1_TECH_LTE Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_rf1_tech_lte [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 3 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 19 ),
};

/* UIM1_ACTIVITY Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_uim1_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
};

/* UIM1_ENABLE Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_uim1_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
};

/* UIM2_ACTIVITY Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_uim2_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
};

/* UIM2_ENABLE Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_uim2_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
};

/* NPA NODE RESOURCE */
pm_npa_node_resource_info
pm_npa_modem_pam_node_rsrcs [] =
{
   {
      PMIC_NPA_GROUP_ID_ANT1, // Resource Name
      PMIC_NPA_MODE_ID_ANT_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_ant1_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_ANT1, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_ant1, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_ant1), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_PLL_SR2, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_pll_sr2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_PLL_SR2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_pll_sr2, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_pll_sr2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_QFPROM, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_qfprom_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_QFPROM, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_qfprom, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_qfprom), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_CX, // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm_pam_rail_cx_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_CX, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rail_cx, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rail_cx), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_MX, // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm_pam_rail_mx_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_MX, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rail_mx, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rail_mx), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_1X, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm_pam_rf1_tech_1x_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_1X, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_1x, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_1x), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GPS, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm_pam_rf1_tech_gps_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_gps, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_gps), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GPS_ELNA, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm_pam_rf1_tech_gps_elna_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS_ELNA, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_gps_elna, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_gps_elna), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_LTE, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm_pam_rf1_tech_lte_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_lte, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_lte), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ACTIVITY, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_uim1_activity_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim1_activity, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim1_activity), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ENABLE, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_uim1_enable_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ENABLE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim1_enable, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim1_enable), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ACTIVITY, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_uim2_activity_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim2_activity, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim2_activity), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ENABLE, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm_pam_uim2_enable_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ENABLE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim2_enable, // Node Dependency
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim2_enable), // Deps Count & Deps Node Dependency Count
   },
};

uint32 num_of_pm_modem_nodes [] = { 13 };


pm_npa_remote_name_type
pmic_npa_clk_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 2), "clka\x02\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 3), "clka\x03\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 4), "clka\x04\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 5), "clka\x05\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pmic_npa_remote_clk [1] = 
{
   {
      pmic_npa_clk_remote_resources,
      4
   }
};

pm_npa_remote_name_type
pmic_npa_ldo_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 1), "ldoa\x01\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 3), "ldoa\x03\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 4), "ldoa\x04\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 6), "ldoa\x06\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 7), "ldoa\x07\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 14), "ldoa\x0E\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 15), "ldoa\x0F\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 18), "ldoa\x12\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 19), "ldoa\x13\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pmic_npa_remote_ldo [1] = 
{
   {
      pmic_npa_ldo_remote_resources,
      9
   }
};

pm_npa_remote_name_type
pmic_npa_smps_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 2), "smpa\x02\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pmic_npa_remote_smps [1] = 
{
   {
      pmic_npa_smps_remote_resources,
      1
   }
};
