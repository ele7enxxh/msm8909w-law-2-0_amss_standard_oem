#ifndef PM_RPM_SMPS_TRANS_APPLY__H
#define PM_RPM_SMPS_TRANS_APPLY__H

/*! \file pm_rpm_smps_trans_apply.h
 *  \n
 *  \brief This file contains prototype definitions for PMIC SMPS
 *         register resources and register resource dependencies functions.
 *  \n  
 *  \n &copy; Copyright 2011-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_rpm_smps_trans_apply.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/13   hs      Adding settling time for regulators.
09/10/13   hs      Fixed the naming convention in \config.
09/10/13   hs      Code refactoring.
1/29/13    aks     Adding support for 5v boost as a separate driver
08/07/12   hs      Added support for 5V boost.
07/20/12   hs      Added settling time.
04/12/12   hs      Added fields in pm_npa_smps_data for properly utilizing the 
                   TransactionManager.
01/26/12   wra     Created.

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_rpm_utilities.h"
#include "rpmserver.h"
#include "pm_smps_driver.h"

#define GET_PMIC_INDEX_SMPS(resourceType) ((resourceType & 0xFF000000 )>> 24) - 0x61;


typedef struct 
{
    unsigned        LpHpCurrentThreshold:15; // in mA
    unsigned        IsSawControlled:1;       //0 - not SAW controlled 
    unsigned        AccessAllowed:1;         // 0 – NO NPA Access; 1 – NPA Access Granted
    unsigned        AlwaysOn:1;              // 0 - allow to be turned off; 1 - always on
    unsigned        MinPwrMode:5;            // 0 - AUTO, 1- IPEAK, 2 - NPM
    unsigned        MinFreq:4;               // 1 - 9.6M, 2 - 6.4M, 3 - 4.8M, 5 - 3.2M, etc
    unsigned        MaxFreq:4;               // 1 - 9.6M, 2 - 6.4M, 3 - 4.8M, 5 - 3.2M, etc
    unsigned        DroopDetection:1;        // 0 - OFF, 1 - ON 
    //32 bit boundary
    unsigned        MinVoltage:16;           // in mV
    unsigned        MaxVoltage:16;           // in mV
    //32 bit boundary
    unsigned        MinSettlingTime:16;      // in micro secs
    unsigned        SettlingErrorEnabled:1;  // ignore settling failure  
    unsigned        SettlingEnabled:1;       // ignore settling  
    unsigned        Reserved:14;
    //32 bit boundary
}pm_rpm_smps_rail_info_type;

// SMPS Callback data used during translate and apply
typedef struct 
{
    rpm_resource_type                         resourceType;
    pm_smps_data_type                        *smpsDriverData;
    pm_rpm_smps_rail_info_type               **railInfo;
    pm_pwr_resource_dependency_info          **depInfo;
    pm_pwr_settling_time_info_type           *settlingTimeInfo;
    pm_pwr_settling_time_info_type           *ultSettlingTimeInfo;
    uint64                                   *settlingStartTime;
    uint64                                   *settlingEndTime;
}pm_npa_smps_data_type;

void pm_rpm_smps_register_resources(rpm_resource_type resource, uint32 num_npa_resources,uint8 pmic_index);
void pm_rpm_smps_register_resource_dependencies(rpm_resource_type resource, uint32 num_npa_resources, uint8 pmic_index);
pm_npa_smps_data_type* pm_rpm_smps_get_resource_data(uint8 pmic_chip);
void pm_rpm_smps_dependency_execute(rpm_application_info *info, pm_npa_smps_int_rep* shadow_data);


#endif // PM_RPM_SMPS_TRANS_APPLY__H
