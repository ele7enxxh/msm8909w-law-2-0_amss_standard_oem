/*! \file  pm_config_modem_npa_node_rsrcs.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: Wearables_PM8916_NPA_v3p01_2015_08_21 - UNLOCKED

=============================================================================

                      THIS NPA SW LABEL IS UNLOCKED
    DO NOT SUBMIT THIS GENERATED FILE TO P4 BY REMOVING THIS SECTION!!!

=============================================================================


 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2015 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8909/pm8916/pm_config_modem_npa_node_rsrcs.c#1 $ 
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
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_DO "/node/pmic/client/rf1_tech_do"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS "/node/pmic/client/rf1_tech_gps"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM "/node/pmic/client/rf1_tech_gsm"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM2 "/node/pmic/client/rf1_tech_gsm2"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM3 "/node/pmic/client/rf1_tech_gsm3"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE "/node/pmic/client/rf1_tech_lte"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE2 "/node/pmic/client/rf1_tech_lte2"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_TDSCDMA  "/node/pmic/client/rf1_tech_tdscdma"
#define PMIC_NPA_CLIENT_NODE_RF1_TECH_WCDMA "/node/pmic/client/rf1_tech_wcdma"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_1X "/node/pmic/client/rf2_tech_1x"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_DO "/node/pmic/client/rf2_tech_do"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_GPS "/node/pmic/client/rf2_tech_gps"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM "/node/pmic/client/rf2_tech_gsm"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM2 "/node/pmic/client/rf2_tech_gsm2"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM3 "/node/pmic/client/rf2_tech_gsm3"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_LTE "/node/pmic/client/rf2_tech_lte"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_LTE2 "/node/pmic/client/rf2_tech_lte2"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_TDSCDMA  "/node/pmic/client/rf2_tech_tdscdma"
#define PMIC_NPA_CLIENT_NODE_RF2_TECH_WCDMA "/node/pmic/client/rf2_tech_wcdma"
#define PMIC_NPA_CLIENT_NODE_RF3_GPS1 "/node/pmic/client/rf3_gps1"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDD "/node/pmic/client/sensor_vdd"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDD_2 "/node/pmic/client/sensor_vdd_2"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO "/node/pmic/client/sensor_vddio"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO_2 "/node/pmic/client/sensor_vddio_2"
#define PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY "/node/pmic/client/uim1_activity"
#define PMIC_NPA_CLIENT_NODE_UIM1_ENABLE "/node/pmic/client/uim1_enable"
#define PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY "/node/pmic/client/uim2_activity"
#define PMIC_NPA_CLIENT_NODE_UIM2_ENABLE "/node/pmic/client/uim2_enable"
#define PMIC_NPA_CLIENT_NODE_UIM3_ACTIVITY "/node/pmic/client/uim3_activity"
#define PMIC_NPA_CLIENT_NODE_UIM3_ENABLE "/node/pmic/client/uim3_enable"
/*===========================================================================

                        OTHER VARIABLES DEFINITION

===========================================================================*/

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_ant1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_pll_sr2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_qfprom_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rail_cx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rail_mx_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_1x_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_do_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_gps_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_gsm_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_gsm2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_gsm3_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_lte_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_lte2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_tdscdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf1_tech_wcdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_1x_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_do_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_gps_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_gsm_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_gsm2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_gsm3_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_lte_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_lte2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_tdscdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf2_tech_wcdma_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_rf3_gps1_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_sensor_vdd_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_sensor_vdd_2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_sensor_vddio_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_sensor_vddio_2_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim1_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim1_enable_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim2_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim2_enable_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim3_activity_rails_info [];

extern pm_npa_pam_client_cfg_type
pm8916_pm_pam_uim3_enable_rails_info [];

/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/


/* NPA NODE DEPENDENCY */

/* ANT1 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_ant1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* PLL_SR2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_pll_sr2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 7 ),
};

/* QFPROM Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_qfprom [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 6 ),
};

/* RAIL_CX Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rail_cx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, smps, 1 ),
};

/* RAIL_MX Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rail_mx [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 3 ),
};

/* RF1_TECH_1X Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_1x [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_DO Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_do [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_GPS Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gps [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
};

/* RF1_TECH_GSM Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_GSM2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_GSM3 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm3 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_LTE Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_LTE2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_TDSCDMA Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_tdscdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF1_TECH_WCDMA Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf1_tech_wcdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_1X Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_1x [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_DO Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_do [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_GPS Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gps [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_GSM Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_GSM2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_GSM3 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm3 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_LTE Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_LTE2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_TDSCDMA Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_tdscdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF2_TECH_WCDMA Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf2_tech_wcdma [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 18 ),
};

/* RF3_GPS1 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_rf3_gps1 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, clk, 2 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 4 ),
   PMIC_NPA_NODE_DEP_LIST( A, clk, 5 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 1 ),
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 4 ),
};

/* SENSOR_VDD Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_sensor_vdd [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 17 ),
};

/* SENSOR_VDD_2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_sensor_vdd_2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 17 ),
};

/* SENSOR_VDDIO Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_sensor_vddio [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 6 ),
};

/* SENSOR_VDDIO_2 Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_sensor_vddio_2 [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 6 ),
};

/* UIM1_ACTIVITY Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim1_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
};

/* UIM1_ENABLE Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim1_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 14 ),
};

/* UIM2_ACTIVITY Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim2_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
};

/* UIM2_ENABLE Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim2_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 15 ),
};

/* UIM3_ACTIVITY Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim3_activity [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
};

/* UIM3_ENABLE Client */
npa_node_dependency
pm8916_pm_modem_proc_client_dev_deps_uim3_enable [] =
{
   PMIC_NPA_NODE_DEP_LIST( A, ldo, 16 ),
};

/* NPA NODE RESOURCE */
pm_npa_node_resource_info
pm8916_pm_npa_modem_pam_node_rsrcs [] =
{
   {
      PMIC_NPA_GROUP_ID_ANT1, // Resource Name
      PMIC_NPA_MODE_ID_ANT_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_ant1_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_ANT1, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_ant1, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_ant1), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_PLL_SR2, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_pll_sr2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_PLL_SR2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_pll_sr2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_pll_sr2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_QFPROM, // Resource Name
      PMIC_NPA_MODE_ID_GENERIC_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_qfprom_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_QFPROM, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_qfprom, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_qfprom), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_CX, // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_ENABLE_PREPOST_CHANGE_EVENTS, // Resource Attribute
      (void*) pm8916_pm_pam_rail_cx_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_CX, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rail_cx, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rail_cx), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RAIL_MX, // Resource Name
      PMIC_NPA_MODE_ID_CORE_RAIL_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rail_mx_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RAIL_MX, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rail_mx, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rail_mx), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_1X, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_1x_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_1X, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_1x, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_1x), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_DO, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_do_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_DO, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_do, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_do), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GPS, // Resource Name
      PMIC_NPA_MODE_ID_GPS_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_gps_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GPS, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gps, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gps), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GSM, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_gsm_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GSM2, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_gsm2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_GSM3, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_gsm3_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_GSM3, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm3, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_gsm3), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_LTE, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_lte_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_LTE2, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_lte2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_LTE2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_lte2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_TDSCDMA, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_tdscdma_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_TDSCDMA , // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_tdscdma, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_tdscdma), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF1_TECH_WCDMA, // Resource Name
       - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf1_tech_wcdma_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF1_TECH_WCDMA, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf1_tech_wcdma, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf1_tech_wcdma), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_1X, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DRIVER_UNCONDITIONAL_FIRST, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_1x_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_1X, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_1x, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_1x), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_DO, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_do_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_DO, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_do, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_do), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_GPS, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_gps_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_GPS, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gps, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gps), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_GSM, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_gsm_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_GSM2, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_gsm2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_GSM3, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_gsm3_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_GSM3, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm3, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_gsm3), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_LTE, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_lte_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_LTE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_LTE2, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_lte2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_LTE2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_lte2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_TDSCDMA, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_tdscdma_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_TDSCDMA , // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_tdscdma, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_tdscdma), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF2_TECH_WCDMA, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf2_tech_wcdma_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF2_TECH_WCDMA, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf2_tech_wcdma, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf2_tech_wcdma), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_RF3_GPS1, // Resource Name
      PMIC_NPA_MODE_ID_RF_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_DEFAULT, // Resource Attribute
      (void*) pm8916_pm_pam_rf3_gps1_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_RF3_GPS1, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_rf3_gps1, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_rf3_gps1), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_SENSOR_VDD, // Resource Name
      PMIC_NPA_MODE_ID_SENSOR_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm8916_pm_pam_sensor_vdd_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_SENSOR_VDD, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_sensor_vdd, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_sensor_vdd), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_SENSOR_VDD_2, // Resource Name
      PMIC_NPA_MODE_ID_SENSOR_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm8916_pm_pam_sensor_vdd_2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_SENSOR_VDD_2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_sensor_vdd_2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_sensor_vdd_2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_SENSOR_VDDIO, // Resource Name
      PMIC_NPA_MODE_ID_SENSOR_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm8916_pm_pam_sensor_vddio_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_sensor_vddio, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_sensor_vddio), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2, // Resource Name
      PMIC_NPA_MODE_ID_SENSOR_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT, // Resource Attribute
      (void*) pm8916_pm_pam_sensor_vddio_2_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO_2, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_sensor_vddio_2, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_sensor_vddio_2), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ACTIVITY, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim1_activity_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ACTIVITY, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim1_activity, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim1_activity), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM1_ENABLE, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim1_enable_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM1_ENABLE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim1_enable, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim1_enable), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ACTIVITY, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim2_activity_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ACTIVITY, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim2_activity, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim2_activity), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM2_ENABLE, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim2_enable_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM2_ENABLE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim2_enable, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim2_enable), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM3_ACTIVITY, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim3_activity_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM3_ACTIVITY, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim3_activity, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim3_activity), // Deps Count & Deps Node Dependency Count
   },
   {
      PMIC_NPA_GROUP_ID_UIM3_ENABLE, // Resource Name
      PMIC_NPA_MODE_ID_UIM_MODE_MAX - 1, // Maximum Value
      NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
      (void*) pm8916_pm_pam_uim3_enable_rails_info, // Resource User Data
      PMIC_NPA_CLIENT_NODE_UIM3_ENABLE, // Node Name
      NPA_NODE_DEFAULT, // Node Attributes
      NULL,
      pm8916_pm_modem_proc_client_dev_deps_uim3_enable, // Node Dependency
      NPA_ARRAY_SIZE(pm8916_pm_modem_proc_client_dev_deps_uim3_enable), // Deps Count & Deps Node Dependency Count
   },
};

uint32 pm8916_num_of_pm_modem_nodes [] = { 36 };


pm_npa_remote_name_type
pm8916_pmic_npa_clk_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 2), "clka\x02\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 4), "clka\x04\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, clk, 5), "clka\x05\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pm8916_pmic_npa_remote_clk [1] = 
{
   {
      pm8916_pmic_npa_clk_remote_resources,
      3
   }
};

pm_npa_remote_name_type
pm8916_pmic_npa_ldo_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 1), "ldoa\x01\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 3), "ldoa\x03\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 4), "ldoa\x04\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 6), "ldoa\x06\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 7), "ldoa\x07\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 14), "ldoa\x0E\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 15), "ldoa\x0F\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 16), "ldoa\x10\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 17), "ldoa\x11\x00\x00\x00"    },
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, ldo, 18), "ldoa\x12\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pm8916_pmic_npa_remote_ldo [1] = 
{
   {
      pm8916_pmic_npa_ldo_remote_resources,
      10
   }
};

pm_npa_remote_name_type
pm8916_pmic_npa_smps_remote_resources [] =
{
   { PMIC_DEV_RSRC_NAME_VEC_IN( A, smps, 1), "smpa\x01\x00\x00\x00"    },
};

pm_npa_remote_resource_type
pm8916_pmic_npa_remote_smps [1] = 
{
   {
      pm8916_pmic_npa_smps_remote_resources,
      1
   }
};
