/*! \file   pm_init.c
*
*  \brief   This file contains PMIC Modem initialization
*  \details PMIC Modem Proc initialization to initialize driver resources
*  
*  &copy; Copyright 2010-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/src/pm_init.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/23/15    ps      Removed RUMI platform check to allow dynamic stubbing for 9x07 (CR-878990)
06/16/14    akt     Add support virtio - return init for RUMI and Virtio (CR-680596)
07/12/12    umr     Add support for BUA.
07/12/12    vk      Satisfy COREBSP Dependency to pm_init() and deploy in RCINIT
                    Framework

============================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "DDIPlatformInfo.h"
#include "npa.h"

#include "pm_qc_pmic.h"
#include "device_info.h"
#include "pm_comm.h"
#include "pm_modem_proc_npa.h"
#include "pm_target_information.h"
#include "pm_resource_manager.h"

#ifdef PM_FEATURE_ON_TARGET_ENGINEERING_BUILD
#include "pm_on_target_test.h"
#endif


#ifdef FEATURE_PMIC_DIAG_SERVICES
#include "pm_diag.h"
#include "pmapp_diag.h"
#endif


/*===========================================================================

                    FUNCTION DEFINITION

===========================================================================*/

/* NOTE : To enable stub code in Pre-Silicon validation, RUMI/VIRTIO
 * .cmm script should set bPMICStub = TRUE .
 */
boolean bPMICStub = FALSE;

/* This macro should be defined on pmic.scons to enable it */
#ifdef PM_TEST_FRAMEWORK_DRIVER
extern void pmic_unit_testcases (void) ;
#endif

#ifdef PM_IMAGE_MODEM_PROC
void pm_init ( void )
{
    /* protects double calls from TMC */
    #if defined(FEATURE_RCINIT)
    {
        static boolean call_once = FALSE;
        if(FALSE != call_once)
        return;
        call_once = TRUE;
    }
    #endif

    pm_comm_channel_init_internal();

    pm_version_detect();

    pm_target_information_init();

    pm_comm_info_init();

    /*
    * Stub out driver initialization in applicable environments.
    */
    if (bPMICStub == FALSE)
    {
        pm_resource_manager_init();

        #ifdef FEATURE_PMIC_DIAG_SERVICES
            pm_app_diag_init();
            pm_diag_init();
        #endif

#if defined(PM_TEST_MODE)
        pm_app_set_mode_init();
#endif
    }

   pm_modem_proc_npa_init ();

   //Disable Battery Alarm for PMIC 2.0
   //(void) pm_uicc_cntrl_enable(PM_BATTERY_ALARM, TRUE);

#ifdef PM_FEATURE_ON_TARGET_ENGINEERING_BUILD
    {
        /* pm_err_flag_type pm_on_target ( void** funcPtrArray , short nosOfFuncPointers ) ; */
        pm_err_flag_type err = PM_ERR_FLAG__SUCCESS ;
        void** table = NULL ;
        int sizeOfTable = 0 ;
        PmicObjects::OnTargetTester::CreateInstance();

        err = PmicObjects::OnTargetTester::GetSingleton()->ExecuteOnTargetTests( table , sizeOfTable ) ;
    }
#endif /* PM_FEATURE_ON_TARGET_ENGINEERING_BUILD */

#ifdef PM_TEST_FRAMEWORK_DRIVER
    pmic_unit_testcases ();
#endif
}
#endif

