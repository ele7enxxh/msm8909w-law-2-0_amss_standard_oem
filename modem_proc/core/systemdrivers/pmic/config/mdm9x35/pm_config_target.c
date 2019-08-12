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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/mdm9x35/pm_config_target.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_target_information.h"
#include "pm_npa.h"

#include "pm_mpp_driver.h"
#include "pm_xo_driver.h"
#include "pm_uicc_app.h"
#include "pm_pbs_client.h"

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
    {PM_GPIO_5, PM_PBS_CLIENT_6, PM_LDO_11, PM_LDO_13, PM_LDO_INVALID, PM_LDO_INVALID}
};

//=======================================================================================

//Mode	            Logic Voltage	Memory Voltage
//Super Turbo	    1.1125V	        1.1125V
//Turbo	            1.1125V	        1.1125V
//Normal	    1.0V	        1.1125V
//SVS SOC	    0.9V	        1.0V
//SVS Krait	    0.8V	        1.0V
//Retention	    0.5V	        0.675V


uint8 num_of_cx_corners = 6;
uint32 pm_cx_corners[6] = 
{ 500000, 725000, 725000, 900000, 1000000, 1000000};
char PM_MODEM_CX_VREG[50] = "/pmic/device/ldo/A/ldo3/vec";

uint8 num_of_mx_corners = 6;
uint32 pm_mx_corners[6] = 
{675000, 1000000, 1000000, 1000000, 1000000, 1000000};
char PM_MODEM_MX_VREG[50] = "/pmic/device/smps/A/smps3/vec";

pm_pwr_resource_info_type mx_rail[1] = 
{
    {RPM_SMPS_A_REQ, 3, &num_of_mx_corners, pm_mx_corners, PM_MODEM_MX_VREG }// data 1 is the size of the LUT, data 2 is the MX LUT, data3 is the RPM resource string 
};
pm_pwr_resource_info_type cx_rail[1] = 
{
    {RPM_LDO_A_REQ, 3, &num_of_cx_corners, pm_cx_corners, PM_MODEM_CX_VREG }// data 1 is the size of the LUT, data 2 is the CX LUT, data3 is the RPM resource string.
};

pm_pbs_client_type mss_pbs_client = PM_PBS_CLIENT_10;

pm_pwr_resource_info_type mss_rail[1] = 
{
    {RPM_SMPS_A_REQ, 1, &mss_pbs_client, NULL, NULL } //data 1 is the PBS sequence associated with MSS (if necessary)
};
