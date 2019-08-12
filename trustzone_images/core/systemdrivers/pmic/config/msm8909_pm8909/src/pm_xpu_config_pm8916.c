/*! \file
*  \n
*  \brief  pm_xpu_config.c
*  \n
*  \n This file contains configuration data specific for PMIC SPMI XPU
      Peripheral protection for MSM8994 device.
*  \n
*  \n &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/config/msm8909_pm8909/src/pm_xpu_config_pm8916.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/14   rk        Modify XPU permission and Int Ownership for PMIC Peripherals (CR - 666063)

08/28/13   vtw     Created.
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "pm_access_cfg.h"

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/* PMIC SPMI RPU configuration table.
 * Config data are grouped per RPU group. See pm_config.h.
 */

const pm_spmi_mpu_type pm8916_pm_spmi_mpu_cfg [] = {
    {0, 0x09, 0, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},    /* MISC           */
    {0, 0x08, 1, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},    /* PON            */
    {0, 0x0A, 2, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},    /* VREFLPDDR      */
    {0, 0x10, 3, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP}, /* LBC_CHGR    */
    {0, 0x12, 4, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP}, /* LBC_BAT_IF    */
    {0, 0x13, 5, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP}, /* LBC_USB    */
    {0, 0x16, 6, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP}, /* LBC_MISC */
    {0, 0x24, 7, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},    /* TEMP_ALARM     */
    {0, 0x28, 8, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},    /* COIN           */
    {0, 0x31, 9, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VADC1_USR        */
    {0, 0x33, 10, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},      /* VADC3_LC_VBMS      */
    {0, 0x34, 11, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VADC2_BTM       */
    {0, 0x35, 12, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VADC4_LC_VBAT       */
    {0, 0x40, 13, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* BMS_VM      */
    {0, 0x61, 14, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* RTC_ALARM       */
    {0, 0x62, 15, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* RTC_TIMER       */
    {0, 0x17, 16, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},      /* S2_CTRL        */
    {0, 0x18, 17, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},      /* S2_PS          */
    {0, 0x19, 18, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},      /* S2_FREQ        */
    {0, 0xBC, 19, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PWM_3D      */
    {0, 0xC0, 20, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VIB1      */
    {0, 0xF0, 21, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* CDC_D     */
    {0, 0xF1, 22, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* CDC_A     */
    {0, 0xF2, 23, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* CDC_BOOST_FREQ    */
    {0, 0xF3, 24, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* CDC_NCP_FREQ     */
    /* 0 to 30 APPS */
    {1, 0x2C, 31, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* MBG1           */
    {1, 0x51, 32, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BB_CLK1        */
    {1, 0x52, 33, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BB_CLK2        */
    
    {1, 0x59, 34, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* CLK_DIST       */
    {1, 0x5A, 35, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* SLEEP_CLK1     */
    {1, 0x70, 36, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* PBS_CORE       */
    {1, 0x71, 37, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT0    */
    {1, 0x72, 38, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT1    */
    {1, 0x73, 39, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT2    */
    {1, 0x10, 40, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BCLK_GEN_MAIN  */
    {1, 0x40, 41, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO1           */
    {1, 0x41, 42, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO2           */
    {1, 0x42, 43, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO3           */
    {1, 0x43, 44, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO4           */
    {1, 0x44, 45, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO5           */
    {1, 0x46, 46, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO7           */
    {1, 0x47, 47, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO8           */
    {1, 0x48, 48, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO9           */
    {1, 0x49, 49, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO10          */
    {1, 0x4A, 50, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO11          */
    {1, 0x4B, 51, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO12          */
    {1, 0x51, 52, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO18          */
    {1, 0x52, 53, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO19          */
    {1, 0x53, 54, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO20          */
    {1, 0x06, 55, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* SPMI_OPTIONS   */
    {0, 0x45, 56, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO6           */
    {0, 0x4C, 57, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO13          */
    {0, 0x50, 58, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* LDO17          */
    {1, 0x1A, 59, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S3_CTRL        */
    {1, 0x1B, 60, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S3_PS          */
    {1, 0x1C, 61, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S3_FREQ        */
    {1, 0x1D, 62, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S4_CTRL        */
    {1, 0x1E, 63, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S4_PS          */
    {1, 0x1F, 64, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S4_FREQ        */
    {1, 0x15, 65, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S1_PS          */
    {1, 0x16, 66, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S1_FREQ        */
    {1, 0x14, 67, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},         /* S1_CTRL        */
    /* 31 to 73 RPM */

    {1, 0x54, 74, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RF_CLK1        */
    {1, 0x55, 75, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RF_CLK2        */

	
    {2, 0x1C, 76, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},        /* BUA_EXT_CHARGER*/
    {1, 0x32, 77, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},         /* VADC3_MDM      */
    {2, 0x50, 78, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* XO             */
    {2, 0x60, 79, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},         /* RTC_WR         */
    {2, 0x74, 80, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS },         /* PBS_CLIENT3    */
    {2, 0x75, 81, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* PBS_CLIENT4    */
    {2, 0x76, 82, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* PBS_CLIENT5    */
    {2, 0x77, 83, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* PBS_CLIENT6    */
    {2, 0x4D, 84, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},         /* LDO14          */
    {2, 0x4E, 85, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},         /* LDO15          */
    {2, 0x4F, 86, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},         /* LDO16          */
    /* 76 to 90 MPSS */
    {3, 0xA0, 91, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP1         */
    {3, 0xA1, 92, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP2         */
    {3, 0xA2, 93, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP3         */
    {3, 0xA3, 94, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP4         */
    {3, 0xC0, 95, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},       /* GPIO1          */
    {3, 0xC1, 96, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},    /* GPIO2           */
    {3, 0xC2, 97, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},    /* GPIO3           */
    {3, 0xC3, 98, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},    /* GPIO4           */
    /* 91 to 110  RW GE */
    {4, 0x01, 111, PM_ACCESS_ROGE, PM_VMID_NOACCESS},          /* REVID          */
    {4, 0x04, 112, PM_ACCESS_ROGE, PM_VMID_NOACCESS},          /* BUS            */
    {4, 0x05, 113, PM_ACCESS_ROGE, PM_VMID_NOACCESS},          /* INT            */
    {4, 0x06, 114, PM_ACCESS_ROGE, PM_VMID_NOACCESS},          /* SPMI           */
    {4, 0xFE, 115, PM_ACCESS_ROGE, PM_VMID_NOACCESS},       /* TRIM           */
    {4, 0x00, 116, PM_ACCESS_ROGE,PM_VMID_NOACCESS},     /* GID            */
    /* 111 to 116 */
};

/* Size of config table. */
const uint32 pm8916_pm_spmi_mpu_cfg_size = sizeof( pm8916_pm_spmi_mpu_cfg) / sizeof(pm8916_pm_spmi_mpu_cfg);
