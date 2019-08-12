/*==================================================================
 *
 * FILE:        deviceprogrammer_initialize.c
 *
 * DESCRIPTION:
 *   
 *
 *        Copyright © 2008-2015 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *               QUALCOMM Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_initialize.c#1 $ 
 *   $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * YYYY-MM-DD   who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 2014-15-12   sb      Merge deviceprogrammer into SBL
 * 2014-10-15   sb      Updated for 8909
 * 2013-06-03   ah      Added legal header
 * 2013-05-31   ab      Initial checkin
 */

#include <locale.h>
#include "deviceprogrammer_utils.h"
#include "deviceprogrammer_entry_initialize.h"
#include "msm.h"
#include "ClockBoot.h"
#include "pm.h"
#include "SpmiBus.h"
#include "SpmiCfg.h"
#include "ddr_drivers.h"
#include "deviceprogrammer_firehose.h"
#include "deviceprogrammer_security.h"

#include "pm_rgb.h"

extern void main_firehose(void);
extern boolean Clock_PreDDRInitEx(uint32 ddr_type);

boot_pbl_shared_data_type *pbl_shared_global;


void deviceprogrammer_init_hw() {
    char err_log[256] = {0};

    initFirehoseProtocol(); // initializes default values for fh structure

#ifndef FEATURE_FIREHOSE_SKIP_CLOCK
#ifndef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
    // This needs to be called before calling Clock_Init()
    if (TRUE != Clock_PreDDRInitEx(DDR_TYPE_LPDDR2))                     { strlcat(err_log, "clock pre ddr:fail ", sizeof(err_log)); }
#endif

    // Lowering clock frequencies because otherwise eMMC card initialization sometimes fails
    // if the clock '' is set too high
    if (FALSE == Clock_Init(CLOCK_BOOT_PERF_MIN, CLOCK_BOOT_PERF_MIN)) { strlcat(err_log, "init clock:fail ", sizeof(err_log)); }
#endif

    // Call setlocale to set the locale to the default values
    // for string functions such as strncasecmp to work properly
    setlocale(LC_ALL, "C");

    if(PM_ERR_FLAG__SUCCESS != pm_pon_channel_config())            { strlcat(err_log, "init pmic:fail ", sizeof(err_log)); }

    // Initialize eMMC storage device, open up handles
    if (FALSE == initFirehoseStorage())                                { strlcat(err_log, "init storage:fail ", sizeof(err_log)); }

#ifndef FEATURE_FIREHOSE_SKIP_CLOCK
    // This calls the Clock API to bump up clocks.
    // Before doing this, we need support for it in the Clock SCons
    // env.AddLibsToImage(['CLOCK_BOOT_DRIVER', 'EMMCBLD_IMAGE'],
    // ClockChipsetSBL_lib)
    // in file core\systemdrivers\clock\hw\msm8974\build\clock_sbl.scons
    if (FALSE == Clock_Init(CLOCK_BOOT_PERF_MAX, CLOCK_BOOT_PERF_MAX)) { strlcat(err_log, "init clock:fail ", sizeof(err_log)); }
#endif

#ifndef SKIP_SECBOOT_CHECK_NOT_RECOMMENDED_BY_QUALCOMM 
    // This check below is to ensure that only VIP programmer is run on secure boot devices
    // In otherwords, signing the non VIP programmer is highly not recommended
    if (FALSE == isValidationMode() && TRUE == isAuthenticationEnabled()) { strlcat(err_log, "Secure boot detected. VIP not enabled:fail ", sizeof(err_log)); }
#endif

    // These PMIC calls were added to have long key power off to be
    // working from flash programmer so that device can be powered off in
    // factory for devices that do not have removable battery
    pm_pon_reset_source_cfg(0,PM_PON_RESET_SOURCE_KPDPWR,904,500,
    PM_PON_RESET_CFG_NORMAL_SHUTDOWN);
    pm_pon_reset_source_ctl(0, PM_PON_RESET_SOURCE_KPDPWR,PM_ON);

    initFirehoseTransport();    // Initialize USB transport
    if (strlen(err_log) > 0) {
        while (1) {
            logMessage(err_log);
            delaySeconds(2);
        }
    }
}

// The entry point from SBL.
void deviceprogrammer_entry(boot_pbl_shared_data_type *pbl_shared)
{
    pbl_shared_global = pbl_shared;

    deviceprogrammer_zi_buffers ();
    deviceprogrammer_init_hw();
    main_firehose();
}

