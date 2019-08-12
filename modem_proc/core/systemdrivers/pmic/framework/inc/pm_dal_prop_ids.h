#ifndef PM_DAL_PROP_IDS_H
#define PM_DAL_PROP_IDS_H
/*! \file
 *
 *  \brief  pm_dal_prop_id.h ----This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *
 *  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/framework/inc/pm_dal_prop_ids.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/13   hs      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

/* PMIC HW Peripheral specific DAL Properties */
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
#define PM_PROP_LDO_SETTLING_TIME             22
#define PM_PROP_SMPS_SETTLING_TIME            23
#define PM_PROP_VS_SETTLING_TIME              24
#define PM_PROP_BOOST_SETTLING_TIME           25
#define PM_PROP_CLK_SLEEP_REG                 26
#define PM_PROP_CLK_XO_REG                    27
#define PM_PROP_CLK_COMMON                    28
#define PM_PROP_BOOST_BYP_SETTLING_TIME       29
#define PM_PROP_FTS2p5_VOLT                   30
#define PM_PROP_FTS2p5_RANGE                  31
#define PM_PROP_ULT_BUCK_RANGE_1              32
#define PM_PROP_ULT_BUCK_VOLT_1               33
#define PM_PROP_ULT_BUCK_RANGE_2              34
#define PM_PROP_ULT_BUCK_VOLT_2               35
#define PM_PROP_ULT_PMOS_RANGE                36
#define PM_PROP_ULT_PMOS_VOLT                 37
#define PM_PROP_ULT_NMOS_RANGE                38
#define PM_PROP_ULT_NMOS_VOLT                 39
#define PM_PROP_BOOST_BYP_RANGE               40
#define PM_PROP_BOOST_BYP_REG                 41
#define PM_PROP_BOOST_BYP_VOLT                42
#define PM_PROP_CLK_LDO_RANGE                 43
#define PM_PROP_CLK_LDO_VOLT                  44
#define PM_PROP_PWRON_REG                     45
#define PM_PROP_GPIO_REG                      46
#define PM_PROP_MPP_REG                       47
#define PM_PROP_RTC_REG                       48
#define PM_PROP_TALM_REG                      49
#define PM_PROP_SMPS_ILIMIT_LUT               50


/* Target/Power Grid specific DAL Properties */
#define PM_PROP_SMPS_NUM                     101
#define PM_PROP_CLK_NUM                      102
#define PM_PROP_LDO_NUM                      103
#define PM_PROP_VS_NUM                       104
#define PM_PROP_BOOST_NUM                    105
#define PM_PROP_GPIO_NUM                     106
#define PM_PROP_MPP_NUM                      107
#define PM_PROP_PWRON_NUM                    108
#define PM_PROP_RTC_NUM                      109
#define PM_PROP_TALM_NUM                     110
#define PM_PROP_MEGAXO_NUM                   111

#define PM_PROP_MX                           112
#define PM_PROP_CX                           113
#define PM_PROP_GFX                          114
#define PM_PROP_MSS                          115

#define PM_PROP_PAM_NODE_RSRCS               116
#define PM_PROP_PAM_NODE_NUM                 117

#define PM_PROP_XO_SPECIFIC                  118
#define PM_PROP_UICC_SPECIFIC                119
#define PM_PROP_MPP_SPECIFIC                 120

#define PM_PROP_REMOTE_LDO                   121
#define PM_PROP_REMOTE_CLK                   122
#define PM_PROP_REMOTE_SMPS                  123
#define PM_PROP_REMOTE_VS                    124

/* Dummy DAL Properties to get rid of compiler warnings/errors */
#define PM_PROP_PMIC_DUMMY_1                 201
#define PM_PROP_PMIC_DUMMY_2                 202

/* MPM SPMI command DAL Properties */
#define PM_PROP_MPM_ACTIVE_CMDS              301
#define PM_PROP_MPM_SLEEP_CMDS               302
#define PM_PROP_MPM_CMD_INDICES              303
#define PM_PROP_MPM_RAILS_INFO               304

#endif  /* PM_DAL_PROP_IDS_H */
