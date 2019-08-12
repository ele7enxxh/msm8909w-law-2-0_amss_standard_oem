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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8916/pm_config_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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

pm_xo_core_specific_info_type xocore_specific[1] =
{
    {0x7FF, 0x3F}
};

pm_mpp_specific_info_type mpp_specific[1] = 
{
    {0x9E0, 4}
};

pm_uicc_specific_info_type uicc_specific[1] =
{
    {PM_GPIO_1, PM_PBS_CLIENT_6, PM_LDO_14, PM_LDO_15, PM_LDO_16, PM_LDO_INVALID}
};

//==============================================================================
//Mode              Logic Voltage   Memory Voltage
//Super Turbo       1.2875V         1.2875V
//Turbo             1.2875V         1.2875V
//Normal            1.15V           1.15V
//SVS SOC           1.05V           1.05V
//SVS Krait         1.05V           1.05V
//Retention         0.5V            0.75V
//==============================================================================

uint32 pm_cx_corners_count = 4;
uint32 pm_cx_corners_lst[4] = {500000, 1050000, 1150000, 1287500};
char PM_MODEM_CX_VREG[50] = "/pmic/device/smps/A/smps1/vec";

pm_pwr_resource_info_type cx_rail[1] =
{
    {RPM_SMPS_A_REQ, 1, &pm_cx_corners_count, pm_cx_corners_lst, PM_MODEM_CX_VREG }
};

uint32 pm_mx_corners_count = 4;
uint32 pm_mx_corners_lst[4] = {750000, 1050000, 1150000, 1287500};
char PM_MODEM_MX_VREG[50] = "/pmic/device/ldo/A/ldo3/vec";

pm_pwr_resource_info_type mx_rail[1] =
{
    {RPM_LDO_A_REQ, 3, &pm_mx_corners_count, pm_mx_corners_lst, PM_MODEM_MX_VREG }
};

/* CX and MSS same rail */
pm_pwr_resource_info_type mss_rail[1] =
{
    {RPM_SMPS_A_REQ, 1, &pm_cx_corners_count, pm_cx_corners_lst, PM_MODEM_CX_VREG }
};
