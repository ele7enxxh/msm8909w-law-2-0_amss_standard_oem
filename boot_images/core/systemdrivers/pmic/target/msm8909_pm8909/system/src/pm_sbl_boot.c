/*! \file pm_sbl_boot.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/target/msm8909_pm8909/system/src/pm_sbl_boot.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/16   kc      Update for the new CDT entries for PM8916 (CR-1069962)
09/20/16   kc      Update for the new CDT entries for PM8916 (CR-1068895)
06/30/16   kc      Update for the new CDT entries for PM8916 (CR-1033194)
12/14/15   mr      Update for the new CDT entries for PM8916 (CR-943917)
08/20/15   akt     Update for the new CDT entreis for PM8916 (CR-885232)
04/21/15   yps     Added support PM8916 for DDI (CR-821344)
03/23/15   mr      Added support for PMIC Test Framework (CR-801845)
12/05/14   akt     Update CDT check for pm8916 with SMB (CR-775895)
12/05/14   akt     Added support for PM8916 and PM8909 co-existence (CR-764010)
09/29/14   mr      Updated/Modified PBS Driver and Write PBS ROM Version in
                   REV_ID.PBS_OTP_ID Register (CR-728234)
08/22/14   akt     Added for virtio testing (CR-713705)
01/24/14   rk      remove pm_validate_pon as it is used for SMBC
12/19/13   rk      Added spmi channel config setings
11/22/13   rk      Added optimization of SBL and PBS generated code
07/15/13   aab     Enabled SBL charging
02/26/13   kt      Added PMIC SMEM Init function
02/14/13   kt      Added PBS Info reads and stored the info in Glb Ctxt
01/15/13   aab     Fixed KW error/Clean
12/06/12   hw      replace comdef.h with com_dtypes.h
07/26/12   umr     Add Chip Info Chip ID and Rev.
04/09/12   umr     Add PON reason API for SBL to return pon data.
10/03/11   hs      Added code check to ensure the power rail does get turned
                   on/off before returning.
09/28/11   dy      Replace delay loop with DALSYS_BusyWait
08/14/11   dy      Check for more PMIC revisions
11/09/10   wra     File created for 8960

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_sbl_boot.h"
#include "pm_config_sbl.h"
#include "pm_pbs_info.h"
#include "device_info.h"
#include "pm_target_information.h"
#include "DALSys.h" /* For DALSYS_BusyWait */
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
#include "pm_comm.h"
#include "SpmiCfg.h"
#include "pm_app_chg.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "boot_logger.h"
#include "CoreVerify.h"
#include "SpmiBus.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
#ifdef PM_TEST_FRAMEWORK_DRIVER
pm_err_flag_type pm_test_framework (void);
#endif

pm_err_flag_type
pm_driver_post_init (void)
{
  pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;

  /* Dead battery charging is NOT enabled in PRE-SIL platforms like RUMI/VIRTIO */
  if ( (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI) ||
       (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_VIRTIO) )
  {
        return err;
  }

  err = pm_chg_sbl_charging_state_entry();  //Enable SBL Dead/Weak battery Charging

#ifdef PM_TEST_FRAMEWORK_DRIVER
  err = pm_test_framework ();
#endif

  return err;
}

static pm_err_flag_type
pm_device_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_pbs_info_type pbs_info;
  pm_model_type pm_model;

  /* PON KPDPWR configuration:  s1_timer=10256ms, S2_timer=100ms,  Warm Reset */
  err_flag |= pm_pon_reset_source_cfg(0, PM_PON_RESET_SOURCE_KPDPWR, 10256, 2000, PM_PON_RESET_CFG_WARM_RESET);

  /* PON RESIN_AND_KPDPWR configuration:  s1_timer=6720ms, S2_timer=100ms,  Hard Reset */
  err_flag |= pm_pon_reset_source_cfg(0, PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, 10256, 2000, PM_PON_RESET_CFG_HARD_RESET);

  pm_model = pm_get_pmic_model(0);
  if (pm_model == PMIC_IS_PM8909)
  {
    /* Write PBS ROM ID in REV_ID.PBS_OTP_ID (0x0141) */
    if (PM_ERR_FLAG__SUCCESS == pm_get_pbs_info(0, &pbs_info))
    {
       uint8 rom_id = (uint8)((pbs_info.rom_version & 0x0000FF00) >> 8);
       err_flag |= pm_comm_write_byte(0, 0x0141, rom_id, 1);    /* Update REV_ID.PBS_OTP_ID */
    }
  }

  return err_flag;
}

pm_err_flag_type pm_device_init ( void )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    DalDeviceHandle *phPlatformInfoDevice;
    DalChipInfoFamilyType eChipInfoFamilyType;
    DalPlatformInfoPlatformInfoType platformInfo = {DALPLATFORMINFO_TYPE_UNKNOWN,};
    DALResult result;

    Spmi_Result spmi_err_flag = SpmiCfg_Init(TRUE);

    if ( spmi_err_flag != SPMI_SUCCESS )
    {
        err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
    }

    DALSYS_InitMod(NULL);

    /* Get the chip family - this is primarily needed to confirm that CDT types are */
    eChipInfoFamilyType = DalChipInfo_ChipFamily();
    if (eChipInfoFamilyType == DALCHIPINFO_FAMILY_MSM8909)
    {
        boot_log_message("pm_device_init, TEST - MSM8909 detected");
        /* Get the platform info */
        result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
        if (result != DAL_SUCCESS)
        {
            boot_log_message("pm_device_init, ERROR DALAttach Failed");
            err_flag |= (pm_err_flag_type)result;
        }
        else
        {
            result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
            if (result != DAL_SUCCESS)
            {
                boot_log_message("pm_device_init, ERROR DALPlatform Failed");
                err_flag |= (pm_err_flag_type)result;
            }
        }
    }
    else
    {
        boot_log_message("pm_device_init, WARNING - CHIPINFO is NOT MSM8909");
    }

    /* PMIC peripheral to SPMI Channel mapping.
      This function must be called before calling any SPMI R/W. */

    /// If MSM8909 + PM8916 use the 8916 config table
#ifdef FEATURE_BOOT_DDR_DEBUG_WITH_PM8916
    err_flag |= pm_target_information_spmi_chnl_cfg(PROP_DEVICE_NAME_PM8916);
    boot_log_message("pm_device_init, INFO - PM8916 is selected");
#else
    if((DALPLATFORMINFO_TYPE_CDP == platformInfo.platform && 0x02 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_CDP == platformInfo.platform && 0x03 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_CDP == platformInfo.platform && 0x05 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x02 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x04 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x05 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x07 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x08 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x09 == platformInfo.subtype) ||
	   (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x0A == platformInfo.subtype) ||
	   (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x0B == platformInfo.subtype) ||
	   (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x0C == platformInfo.subtype) ||
	   (DALPLATFORMINFO_TYPE_MTP == platformInfo.platform && 0x0D == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_RCM == platformInfo.platform && 0x01 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_RCM == platformInfo.platform && 0x02 == platformInfo.subtype) ||
       (DALPLATFORMINFO_TYPE_QRD == platformInfo.platform && 0x04 == (0x04 & platformInfo.version)))
    {
        err_flag |= pm_target_information_spmi_chnl_cfg(PROP_DEVICE_NAME_PM8916);
        boot_log_message("pm_device_init, INFO - PM8916 is selected");
    }
    else
    {
        err_flag |= pm_target_information_spmi_chnl_cfg(PROP_DEVICE_NAME_PM8909);
        boot_log_message("pm_device_init, INFO - PM8909 is selected");
    }
#endif
    CORE_VERIFY(SPMI_BUS_SUCCESS == SpmiBus_Init());
    err_flag |= pm_version_detect();

    pm_comm_info_init();

    pm_target_information_init();

    err_flag |= pm_pon_init();

    err_flag |= pm_sbl_config(); /* SBL Configuration */
    pm_sbl_config_test(); /* SBL Configuration validation, only executes complete code if spare reg 0x88F bit 0 is set*/

    err_flag |= pm_pbs_info_init();  /* Read PBS INFO for the pmic devices */
    pm_pbs_ram_version_validation_test(); /* PBS RAM Version validation, only executes complete code if spare reg 0x88F bit 0 is set*/

    err_flag |= pm_device_post_init(); /* Initialize PMIC with the ones PDM can not perform */

    return err_flag; /* NON ZERO return means an ERROR */
}

