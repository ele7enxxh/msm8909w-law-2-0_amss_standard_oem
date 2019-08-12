/*! \file  pm_config_modem_npa_node_rsrcs.c
 *  
 *  \brief  This file contains initialization functions for NPA
 *  \details This file contains initialization functions for NPA
 *          and node definitions.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: VU.MSM8962.PMIC.PMA8084_v05312013 - Approved
 *    PMIC code generation NPA Device Setting Value Version: VU.MSM8962.PMIC.PMA8084_v05312013 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8962/pm_config_modem_npa_node_rsrcs.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_npa.h"
#include "pm_modem_proc_npa.h"
#include "pmapp_npa.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_vs.h"
#include "pm_npa_device_smps.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_ANT1   "/node/pmic/client/ant1"
#define PMIC_NPA_CLIENT_NODE_HFPLL1   "/node/pmic/client/hfpll1"
#define PMIC_NPA_CLIENT_NODE_PA1   "/node/pmic/client/pa1"
#define PMIC_NPA_CLIENT_NODE_PLL_HF   "/node/pmic/client/pll_hf"
#define PMIC_NPA_CLIENT_NODE_PLL_SR2   "/node/pmic/client/pll_sr2"
#define PMIC_NPA_CLIENT_NODE_QFPROM   "/node/pmic/client/qfprom"
#define PMIC_NPA_CLIENT_NODE_RAIL_CX   "/node/pmic/client/rail_cx"
#define PMIC_NPA_CLIENT_NODE_RAIL_MSS   "/node/pmic/client/rail_mss"
#define PMIC_NPA_CLIENT_NODE_RAIL_MX   "/node/pmic/client/rail_mx"
#define PMIC_NPA_CLIENT_NODE_RF1   "/node/pmic/client/rf1"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_1X   "/node/pmic/client/rf1_tech_1x"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_DO   "/node/pmic/client/rf1_tech_do"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS   "/node/pmic/client/rf1_tech_gps"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM   "/node/pmic/client/rf1_tech_gsm"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE   "/node/pmic/client/rf1_tech_lte"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_TDSCDMA    "/node/pmic/client/rf1_tech_tdscdma"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_WCDMA   "/node/pmic/client/rf1_tech_wcdma"
#define PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY   "/node/pmic/client/uim1_activity"
#define PMIC_NPA_CLIENT_NODE_UIM1_ENABLE   "/node/pmic/client/uim1_enable"
#define PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY   "/node/pmic/client/uim2_activity"
#define PMIC_NPA_CLIENT_NODE_UIM2_ENABLE   "/node/pmic/client/uim2_enable"
#define PMIC_NPA_CLIENT_NODE_VDD_MEM_UVOL   "/node/pmic/client/vdd_mem_uvol"
#define PMIC_NPA_MODE_ID_CORE_RAIL_MAX_UVOL 1050000
#define PMIC_NPA_CLIENT_NODE_XO_SHUTDOWN   "/node/pmic/client/xo_shutdown"
/*===========================================================================

                OTHER VARIABLES DEFINITIONS

===========================================================================*/

extern pm_npa_pam_client_cfg_type
pm_pam_ant1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_hfpll1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_pa1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_pll_hf_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_pll_sr2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_qfprom_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rail_cx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rail_mss_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rail_mx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_1x_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_do_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gps_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gsm_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_lte_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_tdscdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_wcdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim1_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim1_enable_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim2_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_uim2_enable_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_vdd_mem_uvol_rails_info [];

extern pm_npa_pam_client_cfg_type
pm_pam_xo_shutdown_rails_info [];


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/


/* ANT1 Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_ant1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
};

/* HFPLL1 Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_hfpll1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 12 ),
};

/* PA1 Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_pa1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 13 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
};

/* PLL_HF Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_pll_hf [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 12 ),
};

/* PLL_SR2 Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_pll_sr2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 12 ),
};

/* QFPROM Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_qfprom [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
};

/* RAIL_CX Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rail_cx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, smps, 2 ),
};

/* RAIL_MSS Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rail_mss [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 6 ),
};

/* RAIL_MX Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rail_mx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 1 ),
};

/* RF1 Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_1X Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_1x [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
   PMIC_NPA_NODE_DEP_LIST( A, smps, 3 ),
   PMIC_NPA_NODE_DEP_LIST( A, smps, 5 ),
};

/* RF1_TECH_DO Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_do [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_GPS Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_gps [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_GSM Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_gsm [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_LTE Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_lte [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_TDSCDMA Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_tdscdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* RF1_TECH_WCDMA Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_rf1_tech_wcdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 25 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 26 ),
};

/* UIM1_ACTIVITY Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_uim1_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 9 ),
};

/* UIM1_ENABLE Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_uim1_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 9 ),
};

/* UIM2_ACTIVITY Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_uim2_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 10 ),
};

/* UIM2_ENABLE Client */
npa_node_dependency
pm_modem_proc_client_dev_deps_uim2_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 10 ),
};

/* VDD_MEM_UVOL Client */
npa_node_dependency 
pm_modem_proc_client_dev_deps_vdd_mem_uvol [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 1 ),
};

/*PMIC NPA Node Resource Info */
pm_npa_node_resource_info 
pm_npa_modem_pam_node_rsrcs[] =
{
   {
      PMIC_NPA_GROUP_ID_ANT1,               // Resource Name
      PMIC_NPA_MODE_ID_ANT_MODE_3 - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_ant1_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_ANT1,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_ant1,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_ant1),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_HFPLL1,               // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_hfpll1_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_HFPLL1,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_hfpll1,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_hfpll1),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_PA1,               // Resource Name
      PMIC_NPA_MODE_ID_PA_MODE_3 - 1,        // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attributes
      (void*) pm_pam_pa1_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_PA1,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_pa1,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_pa1),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_PLL_HF,               // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1,        // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attributes
      (void*) pm_pam_pll_hf_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_PLL_HF,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_pll_hf,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_pll_hf),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_PLL_SR2,               // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1,        // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attributes
      (void*) pm_pam_pll_sr2_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_PLL_SR2,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_pll_sr2,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_pll_sr2),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_QFPROM,                        // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1,                // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                      // Resource Attributes
      (void*) pm_pam_qfprom_rails_info,                // Resource User Data
      PMIC_NPA_CLIENT_NODE_QFPROM,                     // Node Name
      NPA_NODE_DEFAULT,                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_qfprom,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_qfprom),      // deps count & Deps Node Dependency count
   },
   {      
      PMIC_NPA_GROUP_ID_RAIL_CX,                         // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1,                 // Maximum value
      NPA_RESOURCE_DEFAULT,                              // Resource Attributes
      (void*) pm_pam_rail_cx_rails_info,                // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_CX,                     // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rail_cx,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rail_cx),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_MSS,               // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1,        // Maximum value
      NPA_RESOURCE_DEFAULT, // Resource Attributes
      (void*) pm_pam_rail_mss_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_MSS,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rail_mss,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rail_mss),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_MX,                         // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1,                // Maximum value
      NPA_RESOURCE_DEFAULT,                              // Resource Attributes
      (void*) pm_pam_rail_mx_rails_info,                // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_MX,                     // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rail_mx,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rail_mx),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1,               // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,        // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attributes
      (void*) pm_pam_rf1_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1,               // Node Name
      NPA_NODE_DEFAULT,                                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_1X,                    // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_rf1_tech_1x_rails_info,            // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_1X,                 // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_1x,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_1x),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_DO,                    // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_rf1_tech_do_rails_info,            // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_DO,                 // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_do,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_do),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GPS,                  // Resource Name
      PMIC_NPA_MODE_ID_GPS_MODE_MAX - 1,        // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                      // Resource Attributes
      (void*) pm_pam_rf1_tech_gps_rails_info,          // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS,               // Node Name
      NPA_NODE_DEFAULT,                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_gps,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_gps),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GSM,                  // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                      // Resource Attributes
      (void*) pm_pam_rf1_tech_gsm_rails_info,          // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM,               // Node Name
      NPA_NODE_DEFAULT,                                // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_gsm,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_gsm),      // deps count & Deps Node Dependency count
   },
  {
      PMIC_NPA_GROUP_ID_RF1_TECH_LTE,               // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_rf1_tech_lte_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE,               // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_lte,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_lte),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA,               // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_rf1_tech_tdscdma_rails_info,       // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_TDSCDMA,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_tdscdma,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_tdscdma),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_WCDMA,                 // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_rf1_tech_wcdma_rails_info,         // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_WCDMA,              // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_rf1_tech_wcdma,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_rf1_tech_wcdma),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ACTIVITY,               // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_uim1_activity_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim1_activity,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim1_activity),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ENABLE,               // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_uim1_enable_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ENABLE,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim1_enable,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim1_enable),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ACTIVITY,               // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_uim2_activity_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim2_activity,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim2_activity),      // deps count & Deps Node Dependency count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ENABLE,               // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1,                 // Maximum value
      NPA_RESOURCE_SINGLE_CLIENT|NPA_RESOURCE_DRIVER_UNCONDITIONAL,                       // Resource Attributes
      (void*) pm_pam_uim2_enable_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ENABLE,            // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_uim2_enable,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_uim2_enable),      // deps count & Deps Node Dependency count
   },
  {
      PMIC_NPA_GROUP_ID_MEM_UVOL,                         // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX_UVOL,        // Maximum value
      NPA_RESOURCE_DEFAULT|NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attributes
      (void*) pm_pam_vdd_mem_uvol_rails_info,      // Resource User Data
      PMIC_NPA_CLIENT_NODE_VDD_MEM_UVOL,                     // Node Name
      NPA_NODE_DEFAULT,                                 // Node Attributes
      NULL,
      pm_modem_proc_client_dev_deps_vdd_mem_uvol,      // Node Dependencies
      NPA_ARRAY_SIZE(pm_modem_proc_client_dev_deps_vdd_mem_uvol),      // deps count & Deps Node Dependency count
   },
};

uint8 num_of_pm_modem_nodes[] ={ 22 };


pm_npa_remote_name_type
pmic_npa_smps_remote_resources [] = 
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 1), "smpa\x01\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 2), "smpa\x02\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 3), "smpa\x03\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 5), "smpa\x05\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 6), "smpa\x06\x00\x00\x00" },
};

pm_npa_remote_resource_type
pmic_npa_remote_smps [1] =
{
   { 
     pmic_npa_smps_remote_resources,
     5
    }
};

pm_npa_remote_name_type
pmic_npa_ldo_remote_resources [] = 
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 4), "ldoa\x04\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 9), "ldoa\x09\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 10), "ldoa\x0A\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 12), "ldoa\x0C\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 14), "ldoa\x0E\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 15), "ldoa\x0F\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 16), "ldoa\x10\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 25), "ldoa\x19\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 26), "ldoa\x1A\x00\x00\x00" },
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
pmic_npa_clk_remote_resources [] = 
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 4), "clka\x04\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 5), "clka\x05\x00\x00\x00" },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 13), "clka\x0D\x00\x00\x00" },
};

pm_npa_remote_resource_type
pmic_npa_remote_clk [1] =
{
   { 
     pmic_npa_clk_remote_resources,
     3
    }
};



