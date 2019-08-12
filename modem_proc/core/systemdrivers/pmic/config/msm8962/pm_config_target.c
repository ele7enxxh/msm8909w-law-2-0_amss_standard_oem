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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8962/pm_config_target.c#1 $ 

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



XOCOREDS XOCOREDSConfig_V6_VariantCommon[1] =
{
    {0x7FF, 0x3F}
};
pm_uicc_specific_info_type uicc_specific[1] =
{
   {PM_GPIO_22, PM_PBS_CLIENT_6, PM_LDO_9, PM_LDO_10, PM_LDO_INVALID, PM_LDO_INVALID}
};
mppDS MPPDSConfig_V6_VariantCommon[1] =
{
    {0x9E0, 4}
};

pm_pwr_specific_info smps_specific_a[12] = 
{
   {fts_range, fts_volt},    // FTS 1 VDD_MX
   {fts_range, fts_volt},    // FTS 2  VDD_CX
   {hfs_range, hfs_volt},    // HFS 3
   {hfs_range, hfs_volt},    // HFS 4
   {hfs_range, hfs_volt},    // HFS 5
   {fts_range, fts_volt},    // FTS 6 VDD_MSS
   {fts_range, fts_volt},    // FTS 7
   {fts_range, fts_volt},    // FTS 8
   {fts_range, fts_volt},    // FTS 9
   {fts_range, fts_volt},    // FTS 10
   {fts_range, fts_volt},    // FTS 11
   {fts_range, fts_volt},    // FTS 12
};

pm_pwr_specific_info ldo_specific_a[27] = 
{
    {n1200_range,  n1200_volt},  //L1 NMOS 1200
    {nmos_range,  nmos_volt},  //L2 NMOS 300
    {nmos_range,  nmos_volt},  //L3 NMOS 300
    {nmos_range,  nmos_volt},  //L4 NMOS 600
    {nmos_range,  nmos_volt},  //L5 NMOS 5
    {pmos_range,  pmos_volt},  //L6 PMOS 150
    {pmos_range,  pmos_volt},  //L7 PMOS 5
    {pmos_range,  pmos_volt},  //L8 PMOS 50
    {pmos_range,  pmos_volt},  //L9 PMOS 150
    {pmos_range,  pmos_volt},  //L10 PMOS 150
    {n1200_range,  n1200_volt},  //L11 NMOS 1200
    {pmos_range,  pmos_volt},  //L12 PMOS 300
    {pmos_range,  pmos_volt},  //L13 PMOS 150
    {pmos_range,  pmos_volt},  //L14 PMOS 150
    {pmos_range,  pmos_volt},  //L15 PMOS 300
    {pmos_range,  pmos_volt},  //L16 PMOS 150
    {pmos_range,  pmos_volt},  //L17 PMOS 300
    {pmos_range,  pmos_volt},  //L18 PMOS 300
    {pmos_range,  pmos_volt},  //L19 PMOS 600
    {pmos_range,  pmos_volt},  //L20 PMOS 600
    {pmos_range,  pmos_volt},  //L21 PMOS 600
    {pmos_range,  pmos_volt},  //L22 PMOS 300
    {pmos_range,  pmos_volt},  //L23 PMOS 300
    {pmos_range,  pmos_volt},  //L24 PMOS 150
    {pmos_range,  pmos_volt},  //L25 PMOS 600
    {pmos_range,  pmos_volt},  //L26 PMOS 300
    {nmos_range,  nmos_volt},  //L27 NMOS 600
};

//=======================================================================================

//Mode	            Logic Voltage	Memory Voltage
//Super Turbo	    1.2875V	        1.2875V
//Turbo	            1.2875V	        1.2875V
//Normal	        1.15V	        1.15V
//SVS SOC	        1.05V	        1.05V
//SVS Krait	        1.05V	        1.05V
//Retention	        0.5V	        0.75V

uint32 pm_cx_corners_count = 6;
uint32 pm_cx_corners_lst[6] = 
{500000, 1050000, 1050000, 1150000, 1275000, 1275000};
char PM_MODEM_CX_VREG[50] = "/pmic/device/smps/A/smps2/vec";

pm_pwr_resource_info_type cx_rail[1] = 
{
    {RPM_SMPS_A_REQ, 2, &pm_cx_corners_count, pm_cx_corners_lst, PM_MODEM_CX_VREG }// data 1 is the size of the LUT, data 2 is the CX LUT, data3 is the RPM resource string.
};

uint32 pm_mx_corners_count = 6;
uint32 pm_mx_corners_lst[6] = 
{750000, 1050000, 1050000, 1150000, 1275000, 1275000};
char PM_MODEM_MX_VREG[50] = "/pmic/device/smps/A/smps1/vec";

pm_pwr_resource_info_type mx_rail[1] = 
{
    {RPM_SMPS_A_REQ, 1, &pm_mx_corners_count, pm_mx_corners_lst, PM_MODEM_MX_VREG }// data 1 is the size of the LUT, data 2 is the MX LUT, data3 is the RPM resource string 
};


//SmpsILimDS smps_ilimit_lut[8] = 
//{
// /*Data, 2X,  3x*/
 //  {0x00,2700,3500},
 //  {0x01,2400,3100},
 //  {0x02,2100,2700},
 //  {0x03,1800,2300},
 //  {0x04,1500,1900},
 //  {0x05,1200,1500},
//   {0x06,900,1100},
//   {0x07,600,700}
//};

