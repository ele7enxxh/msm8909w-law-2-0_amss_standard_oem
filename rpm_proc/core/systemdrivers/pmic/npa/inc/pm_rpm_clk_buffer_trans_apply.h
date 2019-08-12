#ifndef PM_RPM_CLK_BUFFER_TRANS_APPLY__H
#define PM_RPM_CLK_BUFFER_TRANS_APPLY__H
/*===========================================================================


PMIC Clock Buffer Translation and Apply (Aggregate)Functionality


DESCRIPTION
This file contains prototype definitions for PMIC Clock Buffer Translation and 
Apply (Aggregate)Functionality


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_rpm_clk_buffer_trans_apply.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/13   rk     Clk driver input check (do not disable critical clocks) 
09/10/13   rk      Fixed the naming convention in \config.
09/10/13   rk      Code refactoring.
04/12/12   hs      Added fields in pm_npa_clk_buffer_data for properly utilizing the 
                   TransactionManager.
01/26/12   wra     Created.

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_rpm_utilities.h"
#include "rpmserver.h"
#include "pm_clk_driver.h"

#define GET_PMIC_INDEX_CLK(resourceType) ((resourceType & 0xFF000000 )>> 24) - 0x61;


typedef struct 
{
    unsigned        AccessAllowed:1;         // 0 – NO NPA Access; 1 – NPA Access Granted
    unsigned        AlwaysOn:1;              // 0 - allow to be turned off; 1 - always on
    unsigned        reserved:30;           // padding
    //32 bit boundary
}pm_rpm_clk_info_type;


typedef struct 
{
    rpm_resource_type                         resourceType;
    pm_clk_data_type                         *clkBuffDriverData;
	   pm_rpm_clk_info_type                     **clkInfo;	 
    pm_pwr_resource_dependency_info          **depInfo;
}pm_npa_clk_buffer_data_type;

void pm_rpm_clk_buffer_register_resources(rpm_resource_type resource, uint32 num_npa_resources, uint8 pmic_index);

void pm_rpm_clk_buffer_register_resource_dependencies(rpm_resource_type resource, uint32 num_npa_resources, uint8 pmic_index);


#endif // PM_RPM_CLK_BUFFER_TRANS_APPLY__H
