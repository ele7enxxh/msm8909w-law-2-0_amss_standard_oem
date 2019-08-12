/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

P M    NPA    D E V I C E    S E R V I C E S

GENERAL DESCRIPTION
This file contains initialization functions for NPA Device layer

EXTERNALIZED FUNCTIONS
None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2010           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/src/pm_rpm_npa_device.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/15   mr      Removed Boost, Boost Bypass and VS related code (CR-814301)
04/23/13   hs      Fixed the naming convention in \config.
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
01/29/13   aks     Adding support for Boost as separate peripheral
08/07/12   hs      Added support for 5V boost.
02/28/12   wra     Redone for Badger
===========================================================================*/

/*===========================================================================

                INCLUDE FILES

===========================================================================*/
#include <string.h>
#include "assert.h"
#include "err.h"
#include "DALDeviceId.h"
#include "npa.h"
#include "rpmserver.h"
#include "npa_resource.h"

#include "pm_qc_pmic.h"
#include "pmapp_npa.h"

#include "pm_target_information.h"
#include "device_info.h"
#include "pm_npa.h"
#include "pm_rpm_npa_device.h"
#include "pm_npa_device.h"

#include "pm_rpm_clk_buffer_trans_apply.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_rpm_smps_trans_apply.h"


/*===========================================================================

                LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
void
pm_rpm_register_resources (void)
{
    uint32 resource  = 0;
    uint8  pmic_count = 0;
    uint32 num_of_peripherals = 0;
    uint8  num_of_pmics = PM_MAX_NUM_PMICS;

    for(pmic_count = 0; pmic_count < num_of_pmics; pmic_count++)
    {
        num_of_peripherals = pm_smps_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         resource = (uint32) RPM_SMPS_A_REQ + (pmic_count << 24);
         pm_rpm_smps_register_resources((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }

        num_of_peripherals = pm_ldo_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         resource = (uint32) RPM_LDO_A_REQ + (pmic_count << 24);
         pm_rpm_ldo_register_resources((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }

        num_of_peripherals = pm_clk_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         num_of_peripherals = num_of_peripherals -1; //**at NPA layer XO is not regisreted as one of the resources
         resource = (uint32) RPM_CLK_BUFFER_A_REQ + (pmic_count << 24);
         pm_rpm_clk_buffer_register_resources((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }
    }

    for(pmic_count = 0; pmic_count < num_of_pmics; pmic_count++)
    {
        num_of_peripherals = pm_smps_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         resource = (uint32) RPM_SMPS_A_REQ + (pmic_count << 24);
         pm_rpm_smps_register_resource_dependencies((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }

        num_of_peripherals = pm_ldo_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         resource = (uint32) RPM_LDO_A_REQ + (pmic_count << 24);
         pm_rpm_ldo_register_resource_dependencies((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }

        num_of_peripherals = pm_clk_get_num_peripherals(pmic_count);
        if(num_of_peripherals > 0)
        {
         num_of_peripherals = num_of_peripherals -1; //**at NPA layer XO is not regisreted as one of the resources
         resource = (uint32) RPM_CLK_BUFFER_A_REQ + (pmic_count << 24);
         pm_rpm_clk_buffer_register_resource_dependencies((rpm_resource_type)resource, num_of_peripherals, pmic_count);
        }
    }
}

/*===========================================================================

FUNCTION pm_rpm_proc_npa_register_resources

DESCRIPTION
This function initializes the NPA RPM Device layer for PMIC.

Does the following:
* Initializes the PMIC NPA Device Node and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
NPA Framework should be available in the build being compiled for

SIDE EFFECTS
NONE.

===========================================================================*/
void
pm_rpm_proc_npa_register_resources (void)
{
    /* Initialize Local RPM Clk regime client node and Resource */
    pm_rpm_register_resources ();
}
