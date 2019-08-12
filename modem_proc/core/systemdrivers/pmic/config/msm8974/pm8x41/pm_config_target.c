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
 *  &copy; Copyright 2010--2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8974/pm8x41/pm_config_target.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_ipwr.h"
#include "pm_target_information.h"

#include "mpp_v6.h"
#include "xo_core_v6.h"
#include "pm_uicc_app.h"

extern pm_pwr_range_info_type fts_range[];
extern pm_pwr_volt_info_type fts_volt[];

extern pm_pwr_range_info_type hfs_range[];
extern pm_pwr_volt_info_type hfs_volt[];

extern pm_pwr_range_info_type n1200_range[];
extern pm_pwr_volt_info_type n1200_volt[];

extern pm_pwr_range_info_type nmos_range[];
extern pm_pwr_volt_info_type nmos_volt[];

extern pm_pwr_range_info_type pmos_range[];
extern pm_pwr_volt_info_type pmos_volt[];

extern pm_pwr_range_info_type ln_range[];
extern pm_pwr_volt_info_type ln_volt[];

extern pm_pwr_range_info_type boost_range[];
extern pm_pwr_volt_info_type boost_volt[];

XOCOREDS XOCOREDSConfig_V6_VariantCommon[1] =
{
    {0x7FF, 0x3F}
};
pm_uicc_specific_info_type uicc_specific[1] =
{
    {PM_GPIO_31, PM_PBS_CLIENT_6, PM_LDO_9, PM_LDO_10, PM_LDO_INVALID, PM_LDO_INVALID}
};
mppDS MPPDSConfig_V6_VariantCommon[1] =
{
    {0x9E0, 4}
};

pm_pwr_specific_info smps_specific_a[3] = 
{
    {hfs_range, hfs_volt}, //HFS1
    {hfs_range, hfs_volt}, //HFS2
    {hfs_range, hfs_volt}, //HFS3 
};

pm_pwr_specific_info boost_specific_a[1] = 
{
    {boost_range, boost_volt}// 5V BOOST
};

pm_pwr_specific_info ldo_specific_a[24] = 
{
    {nmos_range, nmos_volt}, // LDO1 NMOS
    {nmos_range, nmos_volt}, // LDO2 NMOS
    {nmos_range, nmos_volt}, // LDO3 NMOS
    {nmos_range, nmos_volt}, // LDO4 NMOS
    {ln_range, ln_volt}, // LDO5 LN
    {pmos_range, pmos_volt}, // LDO6 PMOS
    {ln_range, ln_volt}, // LDO7 LN
    {pmos_range, pmos_volt}, // LDO8 PMOS
    {pmos_range, pmos_volt}, // LDO9 PMOS
    {pmos_range, pmos_volt}, // LDO10 PMOS
    {nmos_range, nmos_volt}, // LDO11 NMOS
    {pmos_range, pmos_volt}, // LDO12 PMOS
    {pmos_range, pmos_volt}, // LDO13 PMOS
    {pmos_range, pmos_volt}, // LDO14 PMOS
    {pmos_range, pmos_volt}, // LDO15 PMOS
    {pmos_range, pmos_volt}, // LDO16 PMOS
    {pmos_range, pmos_volt}, // LDO17 PMOS
    {pmos_range, pmos_volt}, // LDO18 PMOS
    {pmos_range, pmos_volt}, // LDO19 PMOS
    {pmos_range, pmos_volt}, // LDO20 PMOS
    {pmos_range, pmos_volt}, // LDO21 PMOS
    {pmos_range, pmos_volt}, // LDO22 PMOS
    {pmos_range, pmos_volt}, // LDO23 PMOS
    {pmos_range, pmos_volt} // LDO24 PMOS
};

// Beginning Data structures for Device Index2
pm_pwr_specific_info smps_specific_b[8] = 
{
   {hfs_range, hfs_volt},    // HFS 1
   {fts_range, fts_volt},    // FTS 2
   {hfs_range, hfs_volt},    // HFS 3
   {fts_range, fts_volt},    // FTS 4
   {fts_range, fts_volt},    // FTS 5
   {fts_range, fts_volt},    // FTS 6
   {fts_range, fts_volt},    // FTS 7
   {fts_range, fts_volt}     // FTS 8
};

//=======================================================================================

//Mode	            Logic Voltage	Memory Voltage
//Super Turbo	    1.05V	        1.05V
//Turbo	            0.9875V	        1.05V
//Normal	    0.9V	        0.95V
//SVS SOC	    0.8125V	        0.95V
//SVS Krait	    0.725V	        0.95V
//Retention	    0.5V	        0.675V


uint32 pm_cx_corners_count = 6;
uint32 pm_cx_corners_lst[6] = 
{ 500000, 725000, 812500, 900000, 987500, 1050000};
char PM_MODEM_CX_VREG[50] = "/pmic/device/smps/B/smps2/vec";

pm_pwr_resource_info_type cx_rail[1] = 
{
    {RPM_SMPS_B_REQ, 2, &pm_cx_corners_count, pm_cx_corners_lst, PM_MODEM_CX_VREG }// data 1 is the size of the LUT, data 2 is the CX LUT, data3 is the RPM resource string.
};

uint32 pm_mx_corners_count = 6;
uint32 pm_mx_corners_lst[6] = 
{675000, 950000, 950000, 950000, 1050000, 1050000 };
char PM_MODEM_MX_VREG[50] = "/pmic/device/smps/B/smps1/vec";

pm_pwr_resource_info_type mx_rail[1] = 
{
    {RPM_SMPS_B_REQ, 1, &pm_mx_corners_count, pm_mx_corners_lst, PM_MODEM_MX_VREG }// data 1 is the size of the LUT, data 2 is the MX LUT, data3 is the RPM resource string 
};


//SmpsILimDS smps_ilimit_lut[8] = 
//{
 /*Data, 2X,  3x*/
 //  {0x00,2700,3500},
 //  {0x01,2400,3100},
 //  {0x02,2100,2700},
 //  {0x03,1800,2300},
 //  {0x04,1500,1900},
 //  {0x05,1200,1500},
//   {0x06,900,1100},
//   {0x07,600,700}
//};


