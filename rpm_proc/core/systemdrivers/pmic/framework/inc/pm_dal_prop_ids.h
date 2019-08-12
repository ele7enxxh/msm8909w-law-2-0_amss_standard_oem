#ifndef PM_DAL_PROP_IDS_H
#define PM_DAL_PROP_IDS_H
/*! \file pm_dal_prop_ids.h
 *  
 *  \brief   This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *  
 *  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_dal_prop_ids.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14   akt     Added PM8909/MSM8909 support.
06/25/13   hs      Created.
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
#define PM_PROP_ULT_LDO_SETTLING_TIME         43
#define PM_PROP_ULT_SMPS_SETTLING_TIME        44

//Target/power grid specific dal properties -- starting from 101
#define PM_PROP_SMPS_NUM                     101
#define PM_PROP_CLK_NUM                      102
#define PM_PROP_LDO_NUM                      103
#define PM_PROP_VS_NUM                       104
#define PM_PROP_BOOST_NUM                    105
#define PM_PROP_MX                           106
#define PM_PROP_CX                           107
#define PM_PROP_GFX                          108
#define PM_PROP_LDO_DEP                      109
#define PM_PROP_SMPS_DEP                     110
#define PM_PROP_BOOST_DEP                    111
#define PM_PROP_VS_DEP                       112
#define PM_PROP_CLK_DEP                      113
#define PM_PROP_PAM_NODE_RSRCS               114
#define PM_PROP_PAM_NODE_NUM                 115
#define PM_PROP_CLK_INFO                     116
#define PM_PROP_SLEEP_ENTER_INFO             117
#define PM_PROP_SLEEP_EXIT_INFO              118
#define PM_PROP_BOOST_BYP_NUM                129
#define PM_PROP_BOOST_BYP_RAIL               130
#define PM_PROP_BOOST_BYP_DEP                131
#define PM_PROP_EBI                          132
#define PM_PROP_BOOST_RAIL                   133
#define PM_PROP_LDO_RAIL                     134
#define PM_PROP_SMPS_RAIL                    135
#define PM_PROP_VS_RAIL                      136



//dummy dal properties to get rid of compiler warnings/errors -- staring from 201
#define PM_PROP_PMIC_DUMMY_1                 201
#define PM_PROP_PMIC_DUMMY_2                 202

//MPM SPMI command dal properties -- starting from 301
#define PM_PROP_MPM_ACTIVE_CMDS               301
#define PM_PROP_MPM_SLEEP_CMDS                302
#define PM_PROP_MPM_CMD_INDICES               303
#define PM_PROP_MPM_RAILS_INFO                304

#endif // PM_DAL_PROP_IDS_H
