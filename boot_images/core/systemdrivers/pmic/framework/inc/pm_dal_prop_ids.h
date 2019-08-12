#ifndef PM_DAL_PROP_IDS_H
#define PM_DAL_PROP_IDS_H
/*! \file
 *
 *  \brief  pm_dal_prop_id.h ----This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *
 *  &copy; Copyright 2013-2015 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/inc/pm_dal_prop_ids.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/15   mr      Added support for 8956 target (CR-790476)
01/29/15   mr      Added support for Coincell Driver (CR-688196)
09/29/14   mr      Updated/Modified PBS Driver (CR-728234)
07/18/14   akt     Added for 8909 (CR-679803)
03/05/14   aab     Updated to the latest PMIC driver architecture
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

//PMIC HW peripheral specific dal properties -- starting from 1
#define PM_PROP_CLK_REG                        1
#define PM_PROP_SMPS_REG                       2
#define PM_PROP_LDO_REG                        3
#define PM_PROP_VS_REG                         4
#define PM_PROP_BOOST_REG                      5
#define PM_PROP_FTS_VOLT                       6
#define PM_PROP_HFS_VOLT                       7
#define PM_PROP_BOOST_VOLT                     8
#define PM_PROP_NMOS_VOLT                      9
#define PM_PROP_N600_VOLT                     10
#define PM_PROP_N1200_VOLT                    11
#define PM_PROP_PMOS_VOLT                     12
#define PM_PROP_LN_VOLT                       13
#define PM_PROP_FTS_RANGE                     14
#define PM_PROP_HFS_RANGE                     15
#define PM_PROP_BOOST_RANGE                   16
#define PM_PROP_NMOS_RANGE                    17
#define PM_PROP_N600_RANGE                    18
#define PM_PROP_N1200_RANGE                   19
#define PM_PROP_PMOS_RANGE                    20
#define PM_PROP_LN_RANGE                      21
#define PM_PROP_CLK_SLEEP_REG                 22
#define PM_PROP_CLK_XO_REG                    23
#define PM_PROP_CLK_COMMON                    24
#define PM_PROP_GPIO_REG                      25
#define PM_PROP_MPP_REG                       26
#define PM_PROP_RGB_REG                       27
#define PM_PROP_LPG_REG                       28
#define PM_PROP_VIB_REG                       29
#define PM_PROP_WLED_REG                      30
#define PM_PROP_PBS_CLIENT_REG                31
#define PM_PROP_VIB_DATA                      32
#define PM_PROP_WLED_DATA                     33
#define PM_PROP_SMPSA_PHASE_LUT               34
#define PM_PROP_MPP_SPECIFIC_DATA             35
#define PM_PROP_SBL_REG_CONFIG_LUT            36
#define PM_PROP_PBS_RAM_CONFIG_LUT            37
#define PM_PROP_SMPS_ILIMIT_LUT               38
#define PM_PROP_FTS2p5_VOLT                   39
#define PM_PROP_FTS2p5_RANGE                  40
#define PM_PROP_LBC_REG                       41
#define PM_PROP_SMBB_REG                      42
#define PM_PROP_CHG_APP_LUT                   43
#define PM_PROP_IBB_REG                       44
#define PM_PROP_LAB_REG                       45
#define PM_PROP_PWM_REG                       46
#define PM_PROP_ULT_BUCK_RANGE_1              47
#define PM_PROP_ULT_BUCK_VOLT_1               48
#define PM_PROP_ULT_BUCK_RANGE_2              49
#define PM_PROP_ULT_BUCK_VOLT_2               50
#define PM_PROP_ULT_PMOS_RANGE                51
#define PM_PROP_ULT_PMOS_VOLT                 52
#define PM_PROP_ULT_NMOS_RANGE                53
#define PM_PROP_ULT_NMOS_VOLT                 54
#define PM_PROP_LED_LUT                       55
#define PM_PROP_LED_DEFAULT                   56
#define PM_PROP_LED_RGB_CONFIG                57
#define PM_PROP_LED_INFO                      58
#define PM_PROP_SETTINGS_TEST_FLAG_REG        59
#define PM_PROP_SMB_INDEX_ARRAY               60
#define PM_PROP_SMB_HW_INFO                   61
#define PM_PROP_COINCELL_REG                  62

//Target/power grid specific dal properties -- starting from 101
#define PM_PROP_PMIC_NUM                     101
#define PM_PROP_SMPSA_NUM                    102
#define PM_PROP_CLKA_NUM                     103
#define PM_PROP_LDOA_NUM                     104
#define PM_PROP_VSA_NUM                      105
#define PM_PROP_BOOSTA_NUM                   106
#define PM_PROP_SMPSB_NUM                    107
#define PM_PROP_LDOB_NUM                     108
#define PM_PROP_VSB_NUM                      109
#define PM_PROP_GPIOA_NUM                    110
#define PM_PROP_GPIOB_NUM                    111
#define PM_PROP_MPPA_NUM                     112
#define PM_PROP_MPPB_NUM                     113
#define PM_PROP_LPGA_NUM                     114
#define PM_PROP_LPGB_NUM                     115
#define PM_PROP_RGBA_NUM                     116
#define PM_PROP_RGBB_NUM                     117
#define PM_PROP_VIBA_NUM                     118
#define PM_PROP_VIBB_NUM                     119
#define PM_PROP_WLEDA_NUM                    120
#define PM_PROP_WLEDB_NUM                    121
#define PM_PROP_PBS_CLIENTA_NUM              122
#define PM_PROP_PBS_CLIENTB_NUM              123
#define PM_PROP_SPMI_PERIPH_CFG              124
#define PM_PROP_NUMBER_PERIPHERAL            125
#define PM_PROP_LBCA_NUM                     126
#define PM_PROP_SMBBA_NUM                    127
#define PM_PROP_LBCB_NUM                     128
#define PM_PROP_SMBBB_NUM                    129
#define PM_PROP_IBB_NUM                      130
#define PM_PROP_LAB_NUM                      131
#define PM_PROP_PWM_NUM                      132
#define PM_PROP_WLED_NUM                     133
#define PM_PROP_SMBCHGB_NUM                  134
#define PM_PROP_PBS_INFO                     135    /* PBS Information related properties  */
#define PM_PROP_COINCELLA_NUM                136
#define PM_PROP_SMPSC_NUM                    137
#define PM_PROP_LDOC_NUM                     138
#define PM_PROP_MPPC_NUM                     139
#define PM_PROP_PBS_CLIENTC_NUM              140


// Property Device name that we support.
#define PROP_DEVICE_NAME_PDM9645   "/boot/pmd9x45"
#define PROP_DEVICE_NAME_PM8909    "/boot/pm8909"
#define PROP_DEVICE_NAME_PM8952    "/boot/pm8950"
#define PROP_DEVICE_NAME_PM8916    "/boot/pm8916"


#endif // PM_DAL_PROP_IDS_H
