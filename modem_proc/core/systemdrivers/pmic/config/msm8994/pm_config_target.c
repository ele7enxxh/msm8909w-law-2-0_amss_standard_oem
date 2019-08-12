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
 *  &copy; Copyright 2010 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8994/pm_config_target.c#1 $ 

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
    {PM_GPIO_21, PM_PBS_CLIENT_6, PM_LDO_9, PM_LDO_10, PM_LDO_INVALID, PM_LDO_INVALID}
};

//=======================================================================================

//Mode	            Logic Voltage	Memory Voltage
//Super Turbo	    1.05V	        1.05V
//Turbo	            0.9875V	        1.05V
//Normal	        0.9V	        0.95V
//SVS SOC	        0.8125V	        0.95V
//SVS Krait	        0.725V	        0.95V
//Retention	        0.5V	        0.675V


uint8 num_of_cx_corners = 6;
uint32 pm_cx_corners[6] = 
{ 500000, 725000, 812500, 900000, 987500, 1050000};
char PM_MODEM_CX_VREG[50] = "/pmic/device/smps/A/smps1/vec";

uint8 num_of_mx_corners = 6;
uint32 pm_mx_corners[6] = 
{675000, 950000, 950000, 950000, 1050000, 1050000};
char PM_MODEM_MX_VREG[50] = "/pmic/device/smps/A/smps12/vec";

pm_pwr_resource_info_type mx_rail[1] = 
{
    {RPM_SMPS_A_REQ, 12, &num_of_mx_corners, pm_mx_corners, PM_MODEM_MX_VREG }// data 1 is the size of the LUT, data 2 is the MX LUT, data3 is the RPM resource string 
};
pm_pwr_resource_info_type cx_rail[1] = 
{
    {RPM_SMPS_A_REQ, 1, &num_of_cx_corners, pm_cx_corners, PM_MODEM_CX_VREG }// data 1 is the size of the LUT, data 2 is the CX LUT, data3 is the RPM resource string.
};

pm_pwr_resource_info_type mss_rail[1] = 
{
    {RPM_SMPS_A_REQ, 7, NULL, NULL, NULL } //data 1 is the PBS sequence associated with MSS (if necessary)
};