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

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/config/mdm9x07_pmd9607/src/pm_xpu_config_pmd9607.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/15   sv      Provided MPSS access permission for LDO5 & LDO6 rails.(CR934264)
10/01/15   sv      Provided APPS access permission for S1 rail.
08/08/15   ps      Added PMD9607 support for MDM9x07 (CR-892107)
07/25/14   akt     Update for 8909 (CR - 679803)
05/17/14   rk      Modify XPU permission and Int Ownership for PMIC Peripherals (CR - 666063)
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

const pm_spmi_mpu_type pm_spmi_mpu_cfg [] = {

    /* APPS RW, 0 - 14 channels */
    {0, 0x08, 0,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PON            */
    {0, 0x09, 1,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* MISC           */
    {0, 0x0A, 2,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VREFLPDDR      */
    {0, 0x24, 3,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* TEMP_ALARM     */
    {0, 0x31, 4,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VADC1_USR      */
    {0, 0x34, 5,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* VADC2_BTM      */
    {0, 0x61, 6,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* RTC_ALARM      */
    {0, 0x71, 7,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PBS_CLIENT0    */
    {0, 0x72, 8,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PBS_CLIENT1    */
    {0, 0x73, 9,  PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PBS_CLIENT2    */
    {0, 0x79, 10, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* PBS_CLIENT8    */
    {1, 0x14, 11, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* S1_CTRL        */
    {1, 0x15, 12, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* S1_PS          */
    {1, 0x16, 13, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_AP},   /* S1_FREQ        */

    /* RPM RW, 15 - 70 channels */
    {1, 0x2C, 15, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* MBG1           */
    {1, 0x51, 16, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BB_CLK1        */
    {1, 0x52, 17, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BB_CLK2        */
    {1, 0x56, 18, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* RF_CLK3       */
    {1, 0x57, 19, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* DIFF_CLK1       */
    {1, 0x59, 20, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* CLK_DIST       */
    {1, 0x5A, 21, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* SLEEP_CLK1      */
    {1, 0x5B, 22, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* DIV_CLK1      */
    {1, 0x5C, 23, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* DIV_CLK     */
    {1, 0x5D, 24, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* DIV_CLK3     */
    {1, 0x62, 25, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RTC_TIMER      */
    {1, 0x70, 26, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CORE       */
    {1, 0x74, 27, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT3    */
    {1, 0x75, 28, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT4    */
    {1, 0x76, 29, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT5    */
    {1, 0x78, 30, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT7    */
    {1, 0x7A, 31, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT9    */
    {1, 0x7B, 32, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT10    */
    {1, 0x7C, 33, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT11    */
    {1, 0x7D, 34, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT12    */
    {1, 0x7E, 35, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT13    */
    {1, 0x7F, 36, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT14    */
    {1, 0x80, 37, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* PBS_CLIENT15    */
    /* BCLK_CMN, SMPS, LDO are on slave 1 */
    {1, 0x10, 38, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* BCLK_CMN */
    {1, 0x17, 42, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S2_CTRL        */
    {1, 0x18, 43, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S2_PS          */
    {1, 0x19, 44, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S2_FREQ        */
    {1, 0x1A, 45, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S3_CTRL        */
    {1, 0x1B, 46, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S3_PS          */
    {1, 0x1C, 47, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S3_FREQ        */
    {1, 0x1D, 48, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S4_CTRL        */
    {1, 0x1E, 49, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S4_PS          */
    {1, 0x1F, 50, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* S4_FREQ        */
    {1, 0x40, 51, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO1           */
    {1, 0x41, 52, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO2           */
    {1, 0x42, 53, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO3           */
    {1, 0x43, 54, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO4           */
    {1, 0x46, 57, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO7           */
    {1, 0x47, 58, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO8           */
    {1, 0x48, 59, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO9           */
    {1, 0x49, 60, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO10          */
    {1, 0x4A, 61, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO11          */
    {1, 0x4B, 62, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO12          */
    {1, 0x4C, 63, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO13          */
    {1, 0x4D, 64, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO14          */
    {1, 0x4E, 65, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO15          */
    {1, 0x4F, 66, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* LDO16          */
    {1, 0x06, 67, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_RPM},   /* SPMI_OPTIONS   */

    /* MPSS RW, 71 - 80 channels */
    {2, 0x1C, 71, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* BUA_NO_CHARGER */
    {2, 0x32, 72, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* VADC3_MDM      */
    {2, 0x50, 73, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* XO             */
    {2, 0x54, 74, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RF_CLK1        */
    {2, 0x55, 75, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RF_CLK2        */
    {2, 0x60, 76, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* RTC_WR         */
    {2, 0x77, 77, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* PBS_CLIENT6    */
    {2, 0x44, 78, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* LDO5           */
    {2, 0x45, 79, PM_ACCESS_ROGE | PM_ACCESS_NON_SEC | PM_ACCESS_RWE, PM_VMID_MSS},   /* LDO6           */
 

    /* 81 to 94  RW GE */

    {3, 0xA0, 81, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP1     */
    {3, 0xA1, 82, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP2     */
    {3, 0xA2, 83, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP3     */
    {3, 0xA3, 84, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP4     */
    {3, 0xA4, 85, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP5     */
    {3, 0xA5, 86, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* MPP6     */
    {3, 0xC0, 87, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO1    */
    {3, 0xC1, 88, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO2    */
    {3, 0xC2, 89, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO3    */
    {3, 0xC3, 90, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO4    */
    {3, 0xC4, 91, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO5    */
    {3, 0xC5, 92, PM_ACCESS_ROGE | PM_ACCESS_RWGE | PM_ACCESS_NON_SEC, PM_VMID_NOACCESS},   /* GPIO6    */

    /* 95 to 100 */

    {4, 0x00, 95,  PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* GID            */
    {4, 0x01, 96,  PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* REVID          */
    {4, 0x04, 97,  PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* BUS            */
    {4, 0x05, 98,  PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* INT            */
    {4, 0x06, 99,  PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* SPMI           */
    {4, 0xFE, 100, PM_ACCESS_ROGE, PM_VMID_NOACCESS},    /* TRIM           */

};

/* Size of config table. */
const uint32 pm_spmi_mpu_cfg_size = sizeof( pm_spmi_mpu_cfg) / sizeof(pm_spmi_mpu_type);
