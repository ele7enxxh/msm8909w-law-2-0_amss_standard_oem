/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

PM RPM PROC

GENERAL DESCRIPTION
This file contains PMIC initialization functions

EXTERNALIZED FUNCTIONS
None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/src/pm_init.c#1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/14   mr      Added PMIC Framework/Driver Test support. (CR-713705)
01/02/14   rk      Remove RPM INIT Client
09/10/13   rk      Fixed the naming convention in \config.
09/10/13   rk      Code refactoring.
02/27/13   hs      Code refactoring.
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/
#include "pm_comm.h"
#include "pm_target_information.h"
#include "pm_rpm_npa.h"
#include "pmapp_npa.h"
#include "pm_resource_manager.h"
#include "pm_rpm_target.h"
#include "pm_mpm_internal.h"
#include "device_info.h"

npa_client_handle handle_rpm_init = NULL;


pm_err_flag_type pm_test_framework (void);
pm_err_flag_type pm_test_npa_framework (void);

#ifdef PM_IMAGE_RPM_PROC
void pm_init( void )
#else
void pm_rpm_init( void )
#endif
{
    pm_comm_channel_init_internal();
    
    pm_version_detect();

    pm_target_information_init();

    pm_comm_info_init();

    pm_resource_manager_init();

    pm_rpm_proc_npa_init ();

    pm_rpm_platform_init();

    //This function may be no-op on targets that do not support
    //sleep yet
    pm_rpm_sleep_init();

    /*Init for SPMI MPM command sequence. */
	pm_mpm_cmd_init();

    handle_rpm_init = npa_create_sync_client (PMIC_NPA_GROUP_ID_RPM_INIT, "rpm_init", NPA_CLIENT_REQUIRED);
    if (handle_rpm_init)
    {
        npa_issue_required_request (handle_rpm_init, PMIC_NPA_MODE_ID_RPM_INIT_MODE_1);
    }

#ifdef PM_TEST_FRAMEWORK_DRIVER
    pm_test_framework ();
#endif
    return ;

}

void pm_post_init( void )
{
#ifdef PM_TEST_FRAMEWORK_DRIVER
    pm_test_npa_framework ();
#endif

    return ;
}
