/*! \file
 *  
 *  \brief  pm_config_npa_dep.c ----This file contains initialization functions for NPA Device layer
 *  \details This file contains initialization functions for NPA Device layer and
 *          the node and resource definition implementations
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8x10_Branch_r1p0_2013_01_03 - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore  this  section will
not contain comments describing changes made to the module.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/config/mdm9x07/pm_config_npa_dep.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/15   ps      Added support for 9x07 target (CR-883336)
05/06/14   rk      Modify the dependency tree for S4 (CR - 657909)
10/09/13   rk      Changing the Dependency tree for CLK LDOs
04/23/13   hs      Fixed the naming convention in \config.
04/12/13   hs      Code refactoring.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_npa.h"

// Component Resource Structure Creation
pm_pwr_resource
smps_a[] =
{
   {(rpm_resource_type)0, 0}, // this is invalid place holder
   {RPM_SMPS_A_REQ, 1},
   {RPM_SMPS_A_REQ, 2},
   {RPM_SMPS_A_REQ, 3},
   {RPM_SMPS_A_REQ, 4},
};
pm_pwr_resource
ldo_a[] =
{
   {(rpm_resource_type)0, 0}, // this is invalid place holder
   {RPM_LDO_A_REQ, 1},
   {RPM_LDO_A_REQ, 2},
   {RPM_LDO_A_REQ, 3},
   {RPM_LDO_A_REQ, 4},
   {RPM_LDO_A_REQ, 5},
   {RPM_LDO_A_REQ, 6},
   {RPM_LDO_A_REQ, 7},
   {RPM_LDO_A_REQ, 8},
   {RPM_LDO_A_REQ, 9},
   {RPM_LDO_A_REQ, 10},
   {RPM_LDO_A_REQ, 11},
   {RPM_LDO_A_REQ, 12},
   {RPM_LDO_A_REQ, 13},
   {RPM_LDO_A_REQ, 14},
   {RPM_LDO_A_REQ, 15},
   {RPM_LDO_A_REQ, 16},
};

// Component Resource Child Depend Structure Creation
static pm_pwr_resource *
smps1_a_child_dep[] =
{
   &ldo_a[10],
};

static pm_pwr_resource *
smps2_a_child_dep[] =
{
   &ldo_a[1],
   &ldo_a[9],
   &ldo_a[12],
};

static pm_pwr_resource *
smps4_a_child_dep[] =
{
   &ldo_a[2],
   &ldo_a[3],
   &ldo_a[7],
   &ldo_a[8],
   &ldo_a[11],
};


// Component Resource Dependency Information
pm_pwr_resource_dependency_info
smps_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // S1
   {
      NULL, // parent resource
      smps1_a_child_dep, // child resource dependencies
      0,  // parent internal client handle
   },
   // S2
   {
      NULL, // parent resource
      smps2_a_child_dep, // child resource dependencies
      0,  // parent internal client handle
   },
   // S3
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // S4
   {
      NULL, // parent resource
      smps4_a_child_dep, // child resource dependencies
      0,  // parent internal client handle
   },
};
pm_pwr_resource_dependency_info
ldo_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // L1
   {
      &smps_a[2],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L2
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L3
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L4
   {
      NULL,
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L5
   {
      NULL,
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L6
   {
      NULL,
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L7
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L8
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L9
   {
      &smps_a[2],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L10
   {
      &smps_a[1],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L11
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L12
   {
      &smps_a[2],
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L13
   {
      NULL,
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L14
   {
      NULL,
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L15
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // L16
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
};
pm_pwr_resource_dependency_info
clk_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // C1
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C2
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C3
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C4
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C5
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C6
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C7
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C8
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C9
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C10
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C11
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C12
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
   // C13
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // parent internal client handle
   },
};

pm_pwr_resource_dependency_info* 
smps_dep[] = 
{
    smps_dep_a
};

pm_pwr_resource_dependency_info* 
ldo_dep[] = 
{
    ldo_dep_a
};

pm_pwr_resource_dependency_info* 
clk_dep[] = 
{
    clk_dep_a 
};

