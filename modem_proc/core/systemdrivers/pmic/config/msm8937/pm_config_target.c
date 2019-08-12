/*! \file
 *
 *  \brief  rpm_settings.c ----This file contains customizable target specific driver settings & PMIC registers.
 *  \details This file contains customizable target specific
 * driver settings & PMIC registers. This file is generated from database functional
 * configuration information that is maintained for each of the targets.
 *
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *
 *  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8937/pm_config_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/14   mr      Resolved issue of .scons file for MSM8909. (CR-769500)
12/05/14   mr      Added Dual PMIC support for MSM8909. (CR-764010)
10/28/14   akt     PBS client3 is used for BUA in 8909 (CR-736180).
10/22/13   rk      GPIO1 used for BUA in 8916.
10/03/13   rk      Changes required for new ULT BUCKs and LDOs
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_target_information.h"
#include "pm_npa.h"

#include "pm_mpp_driver.h"
#include "pm_xo_driver.h"
#include "pm_pbs_client.h"
#include "pm_uicc_app.h"
#include "pm_pwr_alg.h"


/*===========================================================================

                    TARGET SPECIFIC DATA DEFINITIONS

===========================================================================*/

/* PM8937 Info for VIRTIO/RUMI only */
uint32  pm_model_info[][3] =
{
    /* PMIC_Model, AllLayer_Rev, MetalLayer_Rev */
    {          25,            1,              0 },    /* PMIC_IS_PM8937 */
    {          17,            1,              0 },    /* PMIC_IS_PMI8937 */
    {           0,            0,              0 }     /* Array terminator */
};

/* PM8950, PMi8950 */
uint32  num_of_smps[]   = {  6,  0 };
uint32  num_of_ldo[]    = { 23,  1 };
uint32  num_of_clkbuf[] = { 14, 10 };
uint32  num_of_gpio[]   = {  6,  2 };
uint32  num_of_mpp[]    = {  4,  4 };
uint32  num_of_rtc[]    = {  1, 0 };
uint32  num_of_talm[]   = {  1, 0 };
uint32  num_of_megaxo[] = {  1, 0 };
uint32  num_of_boost[]  = {  1,  0 };
uint32  num_of_vs[]     = {  0, 0 };

pm_xo_core_specific_info_type xocore_specific[1] =
{
    {0x7FF, 0x3F}
};

pm_mpp_specific_info_type mpp_specific[2] =
{
    {0x9E0, 4},    /* PM8950 */
    {0x9E0, 4}     /* PMi8950 */
};

pm_uicc_specific_info_type uicc_specific[1] =
{
    {PM_GPIO_3, PM_PBS_CLIENT_3, PM_LDO_14, PM_LDO_15, PM_LDO_INVALID, PM_LDO_INVALID}
};

/*===========================================================================
 * MODE              LOGIC VOLTAGE   MEMORY VOLTAGE
 * Super Turbo       1.2875V         1.2875V
 * Turbo             1.2875V         1.2875V
 * Normal            1.15V           1.15V
 * SVS SOC           1.05V           1.05V
 * SVS Krait         1.05V           1.05V
 * Retention         0.5V            0.75V
===========================================================================*/
uint32 pm_cx_corners_count = 4;
uint32 pm_cx_corners_lst[4] = {500000, 1050000, 1150000, 1287500};
char PM_MODEM_CX_VREG[50] = "/pmic/device/smps/A/smps2/vec";

pm_pwr_resource_info_type cx_rail[1] =
{
    {RPM_SMPS_A_REQ, 2, &pm_cx_corners_count, pm_cx_corners_lst, PM_MODEM_CX_VREG }
};

uint32 pm_mx_corners_count = 4;
uint32 pm_mx_corners_lst[4] = {750000, 1050000, 1150000, 1287500};
char PM_MODEM_MX_VREG[50] = "/pmic/device/ldo/A/ldo3/vec";

pm_pwr_resource_info_type mx_rail[1] =
{
    {RPM_LDO_A_REQ, 3, &pm_mx_corners_count, pm_mx_corners_lst, PM_MODEM_MX_VREG }
};

uint32  mss_stepper_rate = 1500;

pm_pwr_resource_info_type mss_rail[1] =
{
    {RPM_SMPS_A_REQ, 1, NULL, NULL, &mss_stepper_rate }
    // data1 : PBS sequence associated with MSS (if necessary)
    // data2 : mss ocp workaround : disable ocp, save mode, mode=pwm, Enable mss, enable ocp, restore mode
    // data3 : mss stepper rate
};
