/*
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/hal/common/inc/rfd_legacy_inc.h#1 $
$Date: 2016/12/13 $

*/

#ifndef RFD_LEGACY_INC_H
#define RFD_LEGACY_INC_H

// hacky way to determine if we should use this file or the hwio files
#ifdef MTC_REG_BASE
#include "msmhwio.h"
#else

// jcen temp file with old RF HWIO defs

#define MTC_REG_BASE 0
#define MTC_REG_BASE_PHYS 0

#define NAV_REG_BASE 0 
#define NAV_REG_BASE_PHYS 0

#define EDGE_REG_BASE 0
#define EDGE_REG_BASE_PHYS 0

#define HWIO_GRFC_PART_RT_CONFIG_c_ADDR(c)                                    (MTC_REG_BASE      + 0x0000111c + 0x4 * (c))
#define HWIO_GRFC_PART_RT_CONFIG_c_PHYS(c)                                    (MTC_REG_BASE_PHYS + 0x0000111c + 0x4 * (c))
#define HWIO_GRFC_PART_RT_CONFIG_c_RMSK                                       0x1f1f1f1f
#define HWIO_GRFC_PART_RT_CONFIG_c_MAXc                                              0x5
#define HWIO_GRFC_PART_RT_CONFIG_c_INI(c) \
        in_dword(HWIO_GRFC_PART_RT_CONFIG_c_ADDR(c))
#define HWIO_GRFC_PART_RT_CONFIG_c_INMI(c,mask) \
        in_dword_masked(HWIO_GRFC_PART_RT_CONFIG_c_ADDR(c), mask)
#define HWIO_GRFC_PART_RT_CONFIG_c_OUTI(c,val) \
        out_dword(HWIO_GRFC_PART_RT_CONFIG_c_ADDR(c),val)
#define HWIO_GRFC_PART_RT_CONFIG_c_OUTMI(c,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_PART_RT_CONFIG_c_ADDR(c),mask,val,HWIO_GRFC_PART_RT_CONFIG_c_INI(c));\
        HWIO_INTFREE()
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT3_BMSK                       0x10000000
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT3_SHFT                             0x1c
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT3_BMSK                          0xf000000
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT3_SHFT                               0x18
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT2_BMSK                         0x100000
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT2_SHFT                             0x14
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT2_BMSK                            0xf0000
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT2_SHFT                               0x10
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT1_BMSK                           0x1000
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT1_SHFT                              0xc
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT1_BMSK                              0xf00
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT1_SHFT                                0x8
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT0_BMSK                             0x10
#define HWIO_GRFC_PART_RT_CONFIG_c_EN_RT_GRFC_OUT0_SHFT                              0x4
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT0_BMSK                                0xf
#define HWIO_GRFC_PART_RT_CONFIG_c_SRC_GRFC_OUT0_SHFT                                  0

#define HWIO_GRFC_FULL_RT_CONFIG_d_ADDR(d)                                    (MTC_REG_BASE      + 0x00001134 + 0x4 * (d))
#define HWIO_GRFC_FULL_RT_CONFIG_d_PHYS(d)                                    (MTC_REG_BASE_PHYS + 0x00001134 + 0x4 * (d))
#define HWIO_GRFC_FULL_RT_CONFIG_d_RMSK                                       0x7f7f7f7f
#define HWIO_GRFC_FULL_RT_CONFIG_d_MAXd                                              0x3
#define HWIO_GRFC_FULL_RT_CONFIG_d_INI(d) \
        in_dword(HWIO_GRFC_FULL_RT_CONFIG_d_ADDR(d))
#define HWIO_GRFC_FULL_RT_CONFIG_d_INMI(d,mask) \
        in_dword_masked(HWIO_GRFC_FULL_RT_CONFIG_d_ADDR(d), mask)
#define HWIO_GRFC_FULL_RT_CONFIG_d_OUTI(d,val) \
        out_dword(HWIO_GRFC_FULL_RT_CONFIG_d_ADDR(d),val)
#define HWIO_GRFC_FULL_RT_CONFIG_d_OUTMI(d,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_FULL_RT_CONFIG_d_ADDR(d),mask,val,HWIO_GRFC_FULL_RT_CONFIG_d_INI(d));\
        HWIO_INTFREE()
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT3_BMSK                       0x40000000
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT3_SHFT                             0x1e
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT3_BMSK                         0x3f000000
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT3_SHFT                               0x18
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT2_BMSK                         0x400000
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT2_SHFT                             0x16
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT2_BMSK                           0x3f0000
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT2_SHFT                               0x10
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT1_BMSK                           0x4000
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT1_SHFT                              0xe
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT1_BMSK                             0x3f00
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT1_SHFT                                0x8
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT0_BMSK                             0x40
#define HWIO_GRFC_FULL_RT_CONFIG_d_EN_RT_GRFC_OUT0_SHFT                              0x6
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT0_BMSK                               0x3f
#define HWIO_GRFC_FULL_RT_CONFIG_d_SRC_GRFC_OUT0_SHFT                                  0


#define HWIO_GRFC_POL_CONFIG_ADDR                                             (MTC_REG_BASE      + 0x00001200)
#define HWIO_GRFC_POL_CONFIG_PHYS                                             (MTC_REG_BASE_PHYS + 0x00001200)
#define HWIO_GRFC_POL_CONFIG_RMSK                                               0xffffff
#define HWIO_GRFC_POL_CONFIG_IN                                               \
        in_dword_masked(HWIO_GRFC_POL_CONFIG_ADDR, HWIO_GRFC_POL_CONFIG_RMSK)
#define HWIO_GRFC_POL_CONFIG_INM(m)                                           \
        in_dword_masked(HWIO_GRFC_POL_CONFIG_ADDR, m)
#define HWIO_GRFC_POL_CONFIG_OUT(v)                                           \
        out_dword(HWIO_GRFC_POL_CONFIG_ADDR,v)
#define HWIO_GRFC_POL_CONFIG_OUTM(m,v)                                        \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_POL_CONFIG_ADDR,m,v,HWIO_GRFC_POL_CONFIG_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_POL_CONFIG_GRFC_POL_BMSK                                      0xffffff
#define HWIO_GRFC_POL_CONFIG_GRFC_POL_SHFT                                             0

#define HWIO_GRFC_STALL_BYPASS_ADDR                                           (MTC_REG_BASE      + 0x00001204)
#define HWIO_GRFC_STALL_BYPASS_PHYS                                           (MTC_REG_BASE_PHYS + 0x00001204)
#define HWIO_GRFC_STALL_BYPASS_RMSK                                             0xffffff
#define HWIO_GRFC_STALL_BYPASS_IN                                             \
        in_dword_masked(HWIO_GRFC_STALL_BYPASS_ADDR, HWIO_GRFC_STALL_BYPASS_RMSK)
#define HWIO_GRFC_STALL_BYPASS_INM(m)                                         \
        in_dword_masked(HWIO_GRFC_STALL_BYPASS_ADDR, m)
#define HWIO_GRFC_STALL_BYPASS_OUT(v)                                         \
        out_dword(HWIO_GRFC_STALL_BYPASS_ADDR,v)
#define HWIO_GRFC_STALL_BYPASS_OUTM(m,v)                                      \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_STALL_BYPASS_ADDR,m,v,HWIO_GRFC_STALL_BYPASS_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_STALL_BYPASS_STALL_BYPASS_BMSK                                0xffffff
#define HWIO_GRFC_STALL_BYPASS_STALL_BYPASS_SHFT                                       0

#define HWIO_GRFC_RT_INPUT_ENGINE_ADDR                                        (MTC_REG_BASE      + 0x00001208)
#define HWIO_GRFC_RT_INPUT_ENGINE_PHYS                                        (MTC_REG_BASE_PHYS + 0x00001208)
#define HWIO_GRFC_RT_INPUT_ENGINE_RMSK                                          0xffffff
#define HWIO_GRFC_RT_INPUT_ENGINE_IN                                          \
        in_dword_masked(HWIO_GRFC_RT_INPUT_ENGINE_ADDR, HWIO_GRFC_RT_INPUT_ENGINE_RMSK)
#define HWIO_GRFC_RT_INPUT_ENGINE_INM(m)                                      \
        in_dword_masked(HWIO_GRFC_RT_INPUT_ENGINE_ADDR, m)
#define HWIO_GRFC_RT_INPUT_ENGINE_GRFC_ENGINE_BMSK                              0xffffff
#define HWIO_GRFC_RT_INPUT_ENGINE_GRFC_ENGINE_SHFT                                     0

#define HWIO_GRFC_RT_INPUT_HW_ADDR                                            (MTC_REG_BASE      + 0x0000120c)
#define HWIO_GRFC_RT_INPUT_HW_PHYS                                            (MTC_REG_BASE_PHYS + 0x0000120c)
#define HWIO_GRFC_RT_INPUT_HW_RMSK                                                0x3fff
#define HWIO_GRFC_RT_INPUT_HW_IN                                              \
        in_dword_masked(HWIO_GRFC_RT_INPUT_HW_ADDR, HWIO_GRFC_RT_INPUT_HW_RMSK)
#define HWIO_GRFC_RT_INPUT_HW_INM(m)                                          \
        in_dword_masked(HWIO_GRFC_RT_INPUT_HW_ADDR, m)
#define HWIO_GRFC_RT_INPUT_HW_VDD_BMSK                                            0x2000
#define HWIO_GRFC_RT_INPUT_HW_VDD_SHFT                                               0xd
#define HWIO_GRFC_RT_INPUT_HW_GND_BMSK                                            0x1000
#define HWIO_GRFC_RT_INPUT_HW_GND_SHFT                                               0xc
#define HWIO_GRFC_RT_INPUT_HW_TX_AGC_ADJ_OE_BMSK                                   0x800
#define HWIO_GRFC_RT_INPUT_HW_TX_AGC_ADJ_OE_SHFT                                     0xb
#define HWIO_GRFC_RT_INPUT_HW_TX_AGC_ADJ_BMSK                                      0x400
#define HWIO_GRFC_RT_INPUT_HW_TX_AGC_ADJ_SHFT                                        0xa
#define HWIO_GRFC_RT_INPUT_HW_TRK_LO_ADJ_OE_BMSK                                   0x200
#define HWIO_GRFC_RT_INPUT_HW_TRK_LO_ADJ_OE_SHFT                                     0x9
#define HWIO_GRFC_RT_INPUT_HW_TRK_LO_ADJ_BMSK                                      0x100
#define HWIO_GRFC_RT_INPUT_HW_TRK_LO_ADJ_SHFT                                        0x8
#define HWIO_GRFC_RT_INPUT_HW_PA_ON2_SRC_BMSK                                       0x80
#define HWIO_GRFC_RT_INPUT_HW_PA_ON2_SRC_SHFT                                        0x7
#define HWIO_GRFC_RT_INPUT_HW_PA_ON_SRC_BMSK                                        0x40
#define HWIO_GRFC_RT_INPUT_HW_PA_ON_SRC_SHFT                                         0x6
#define HWIO_GRFC_RT_INPUT_HW_TX_ON2_BMSK                                           0x20
#define HWIO_GRFC_RT_INPUT_HW_TX_ON2_SHFT                                            0x5
#define HWIO_GRFC_RT_INPUT_HW_TX_ON_BMSK                                            0x10
#define HWIO_GRFC_RT_INPUT_HW_TX_ON_SHFT                                             0x4
#define HWIO_GRFC_RT_INPUT_HW_PA_RANGE2_BMSK                                         0xc
#define HWIO_GRFC_RT_INPUT_HW_PA_RANGE2_SHFT                                         0x2
#define HWIO_GRFC_RT_INPUT_HW_PA_RANGE_BMSK                                          0x3
#define HWIO_GRFC_RT_INPUT_HW_PA_RANGE_SHFT                                            0

#define HWIO_GRFC_FSM_RESET_ADDR                                              (MTC_REG_BASE      + 0x00001210)
#define HWIO_GRFC_FSM_RESET_PHYS                                              (MTC_REG_BASE_PHYS + 0x00001210)
#define HWIO_GRFC_FSM_RESET_RMSK                                                0xffffff
#define HWIO_GRFC_FSM_RESET_OUT(v)                                            \
        out_dword(HWIO_GRFC_FSM_RESET_ADDR,v)
#define HWIO_GRFC_FSM_RESET_FSM_RST_BMSK                                        0xffffff
#define HWIO_GRFC_FSM_RESET_FSM_RST_SHFT                                               0

#define HWIO_GRFC_DEACTIVATE_IMM_ADDR                                         (MTC_REG_BASE      + 0x00001214)
#define HWIO_GRFC_DEACTIVATE_IMM_PHYS                                         (MTC_REG_BASE_PHYS + 0x00001214)
#define HWIO_GRFC_DEACTIVATE_IMM_RMSK                                           0xffffff
#define HWIO_GRFC_DEACTIVATE_IMM_IN                                           \
        in_dword_masked(HWIO_GRFC_DEACTIVATE_IMM_ADDR, HWIO_GRFC_DEACTIVATE_IMM_RMSK)
#define HWIO_GRFC_DEACTIVATE_IMM_INM(m)                                       \
        in_dword_masked(HWIO_GRFC_DEACTIVATE_IMM_ADDR, m)
#define HWIO_GRFC_DEACTIVATE_IMM_OUT(v)                                       \
        out_dword(HWIO_GRFC_DEACTIVATE_IMM_ADDR,v)
#define HWIO_GRFC_DEACTIVATE_IMM_OUTM(m,v)                                    \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_DEACTIVATE_IMM_ADDR,m,v,HWIO_GRFC_DEACTIVATE_IMM_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_DEACTIVATE_IMM_DEACTIVATE_BMSK                                0xffffff
#define HWIO_GRFC_DEACTIVATE_IMM_DEACTIVATE_SHFT                                       0

#define HWIO_GRFC_ACTIVATE_IMM_ADDR                                           (MTC_REG_BASE      + 0x00001218)
#define HWIO_GRFC_ACTIVATE_IMM_PHYS                                           (MTC_REG_BASE_PHYS + 0x00001218)
#define HWIO_GRFC_ACTIVATE_IMM_RMSK                                             0xffffff
#define HWIO_GRFC_ACTIVATE_IMM_IN                                             \
        in_dword_masked(HWIO_GRFC_ACTIVATE_IMM_ADDR, HWIO_GRFC_ACTIVATE_IMM_RMSK)
#define HWIO_GRFC_ACTIVATE_IMM_INM(m)                                         \
        in_dword_masked(HWIO_GRFC_ACTIVATE_IMM_ADDR, m)
#define HWIO_GRFC_ACTIVATE_IMM_OUT(v)                                         \
        out_dword(HWIO_GRFC_ACTIVATE_IMM_ADDR,v)
#define HWIO_GRFC_ACTIVATE_IMM_OUTM(m,v)                                      \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_ACTIVATE_IMM_ADDR,m,v,HWIO_GRFC_ACTIVATE_IMM_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_ACTIVATE_IMM_ACTIVATE_BMSK                                    0xffffff
#define HWIO_GRFC_ACTIVATE_IMM_ACTIVATE_SHFT                                           0

#define HWIO_GRFC_POL_CONFIG_RF_ADDR                                          (MTC_REG_BASE      + 0x0000121c)
#define HWIO_GRFC_POL_CONFIG_RF_PHYS                                          (MTC_REG_BASE_PHYS + 0x0000121c)
#define HWIO_GRFC_POL_CONFIG_RF_RMSK                                            0x3fffff
#define HWIO_GRFC_POL_CONFIG_RF_IN                                            \
        in_dword_masked(HWIO_GRFC_POL_CONFIG_RF_ADDR, HWIO_GRFC_POL_CONFIG_RF_RMSK)
#define HWIO_GRFC_POL_CONFIG_RF_INM(m)                                        \
        in_dword_masked(HWIO_GRFC_POL_CONFIG_RF_ADDR, m)
#define HWIO_GRFC_POL_CONFIG_RF_OUT(v)                                        \
        out_dword(HWIO_GRFC_POL_CONFIG_RF_ADDR,v)
#define HWIO_GRFC_POL_CONFIG_RF_OUTM(m,v)                                     \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_POL_CONFIG_RF_ADDR,m,v,HWIO_GRFC_POL_CONFIG_RF_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_POL_CONFIG_RF_GRFC_POL_BMSK                                   0x3fffff
#define HWIO_GRFC_POL_CONFIG_RF_GRFC_POL_SHFT                                          0

#define HWIO_GRFC_STALL_BYPASS_RF_ADDR                                        (MTC_REG_BASE      + 0x00001220)
#define HWIO_GRFC_STALL_BYPASS_RF_PHYS                                        (MTC_REG_BASE_PHYS + 0x00001220)
#define HWIO_GRFC_STALL_BYPASS_RF_RMSK                                          0x3fffff
#define HWIO_GRFC_STALL_BYPASS_RF_IN                                          \
        in_dword_masked(HWIO_GRFC_STALL_BYPASS_RF_ADDR, HWIO_GRFC_STALL_BYPASS_RF_RMSK)
#define HWIO_GRFC_STALL_BYPASS_RF_INM(m)                                      \
        in_dword_masked(HWIO_GRFC_STALL_BYPASS_RF_ADDR, m)
#define HWIO_GRFC_STALL_BYPASS_RF_OUT(v)                                      \
        out_dword(HWIO_GRFC_STALL_BYPASS_RF_ADDR,v)
#define HWIO_GRFC_STALL_BYPASS_RF_OUTM(m,v)                                   \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_STALL_BYPASS_RF_ADDR,m,v,HWIO_GRFC_STALL_BYPASS_RF_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_STALL_BYPASS_RF_STALL_BYPASS_BMSK                             0x3fffff
#define HWIO_GRFC_STALL_BYPASS_RF_STALL_BYPASS_SHFT                                    0

#define HWIO_GRFC_FSM_RESET_RF_ADDR                                           (MTC_REG_BASE      + 0x00001224)
#define HWIO_GRFC_FSM_RESET_RF_PHYS                                           (MTC_REG_BASE_PHYS + 0x00001224)
#define HWIO_GRFC_FSM_RESET_RF_RMSK                                             0x3fffff
#define HWIO_GRFC_FSM_RESET_RF_OUT(v)                                         \
        out_dword(HWIO_GRFC_FSM_RESET_RF_ADDR,v)
#define HWIO_GRFC_FSM_RESET_RF_FSM_RST_BMSK                                     0x3fffff
#define HWIO_GRFC_FSM_RESET_RF_FSM_RST_SHFT                                            0

#define HWIO_GRFC_DEACTIVATE_IMM_RF_ADDR                                      (MTC_REG_BASE      + 0x00001228)
#define HWIO_GRFC_DEACTIVATE_IMM_RF_PHYS                                      (MTC_REG_BASE_PHYS + 0x00001228)
#define HWIO_GRFC_DEACTIVATE_IMM_RF_RMSK                                        0x3fffff
#define HWIO_GRFC_DEACTIVATE_IMM_RF_IN                                        \
        in_dword_masked(HWIO_GRFC_DEACTIVATE_IMM_RF_ADDR, HWIO_GRFC_DEACTIVATE_IMM_RF_RMSK)
#define HWIO_GRFC_DEACTIVATE_IMM_RF_INM(m)                                    \
        in_dword_masked(HWIO_GRFC_DEACTIVATE_IMM_RF_ADDR, m)
#define HWIO_GRFC_DEACTIVATE_IMM_RF_OUT(v)                                    \
        out_dword(HWIO_GRFC_DEACTIVATE_IMM_RF_ADDR,v)
#define HWIO_GRFC_DEACTIVATE_IMM_RF_OUTM(m,v)                                 \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_DEACTIVATE_IMM_RF_ADDR,m,v,HWIO_GRFC_DEACTIVATE_IMM_RF_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_DEACTIVATE_IMM_RF_DEACTIVATE_BMSK                             0x3fffff
#define HWIO_GRFC_DEACTIVATE_IMM_RF_DEACTIVATE_SHFT                                    0

#define HWIO_GRFC_ENGINE_RF_ADDR                                              (MTC_REG_BASE      + 0x0000122c)
#define HWIO_GRFC_ENGINE_RF_PHYS                                              (MTC_REG_BASE_PHYS + 0x0000122c)
#define HWIO_GRFC_ENGINE_RF_RMSK                                                0x3fffff
#define HWIO_GRFC_ENGINE_RF_IN                                                \
        in_dword_masked(HWIO_GRFC_ENGINE_RF_ADDR, HWIO_GRFC_ENGINE_RF_RMSK)
#define HWIO_GRFC_ENGINE_RF_INM(m)                                            \
        in_dword_masked(HWIO_GRFC_ENGINE_RF_ADDR, m)
#define HWIO_GRFC_ENGINE_RF_GRFC_ENGINE_RF_BMSK                                 0x3fffff
#define HWIO_GRFC_ENGINE_RF_GRFC_ENGINE_RF_SHFT                                        0

#define HWIO_GRFC_ACTIVATE_IMM_RF_ADDR                                        (MTC_REG_BASE      + 0x00001230)
#define HWIO_GRFC_ACTIVATE_IMM_RF_PHYS                                        (MTC_REG_BASE_PHYS + 0x00001230)
#define HWIO_GRFC_ACTIVATE_IMM_RF_RMSK                                          0x3fffff
#define HWIO_GRFC_ACTIVATE_IMM_RF_IN                                          \
        in_dword_masked(HWIO_GRFC_ACTIVATE_IMM_RF_ADDR, HWIO_GRFC_ACTIVATE_IMM_RF_RMSK)
#define HWIO_GRFC_ACTIVATE_IMM_RF_INM(m)                                      \
        in_dword_masked(HWIO_GRFC_ACTIVATE_IMM_RF_ADDR, m)
#define HWIO_GRFC_ACTIVATE_IMM_RF_OUT(v)                                      \
        out_dword(HWIO_GRFC_ACTIVATE_IMM_RF_ADDR,v)
#define HWIO_GRFC_ACTIVATE_IMM_RF_OUTM(m,v)                                   \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GRFC_ACTIVATE_IMM_RF_ADDR,m,v,HWIO_GRFC_ACTIVATE_IMM_RF_IN); \
        HWIO_INTFREE()
#define HWIO_GRFC_ACTIVATE_IMM_RF_ACTIVATE_BMSK                                 0x3fffff
#define HWIO_GRFC_ACTIVATE_IMM_RF_ACTIVATE_SHFT                                        0


#define HWIO_MODEM_SSBIc_ENABLE_ADDR(c)                                       (MTC_REG_BASE      + 0x00001000 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_ENABLE_PHYS(c)                                       (MTC_REG_BASE_PHYS + 0x00001000 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_ENABLE_RMSK                                                 0x1
#define HWIO_MODEM_SSBIc_ENABLE_MAXc                                                 0x7
#define HWIO_MODEM_SSBIc_ENABLE_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_ENABLE_ADDR(c),val)
#define HWIO_MODEM_SSBIc_ENABLE_OUTMI(c,mask,val) \
        out_dword_masked(HWIO_MODEM_SSBIc_ENABLE_ADDR(c),mask,val,HWIO_MODEM_SSBIc_ENABLE_shadow[c])
#define HWIO_MODEM_SSBIc_ENABLE_CLK_EN_BMSK                                          0x1
#define HWIO_MODEM_SSBIc_ENABLE_CLK_EN_SHFT                                            0

#define HWIO_MODEM_SSBIc_CTL_ADDR(c)                                          (MTC_REG_BASE      + 0x00001004 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_CTL_PHYS(c)                                          (MTC_REG_BASE_PHYS + 0x00001004 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_CTL_RMSK                                               0x1fffff
#define HWIO_MODEM_SSBIc_CTL_MAXc                                                    0x7
#define HWIO_MODEM_SSBIc_CTL_INI(c) \
        in_dword(HWIO_MODEM_SSBIc_CTL_ADDR(c))
#define HWIO_MODEM_SSBIc_CTL_INMI(c,mask) \
        in_dword_masked(HWIO_MODEM_SSBIc_CTL_ADDR(c), mask)
#define HWIO_MODEM_SSBIc_CTL_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_CTL_ADDR(c),val)
#define HWIO_MODEM_SSBIc_CTL_OUTMI(c,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_MODEM_SSBIc_CTL_ADDR(c),mask,val,HWIO_MODEM_SSBIc_CTL_INI(c));\
        HWIO_INTFREE()
#define HWIO_MODEM_SSBIc_CTL_WAKEUP_FINAL_CNT_BMSK                              0x1e0000
#define HWIO_MODEM_SSBIc_CTL_WAKEUP_FINAL_CNT_SHFT                                  0x11
#define HWIO_MODEM_SSBIc_CTL_SSBI_DATA_PDEN_BMSK                                 0x10000
#define HWIO_MODEM_SSBIc_CTL_SSBI_DATA_PDEN_SHFT                                    0x10
#define HWIO_MODEM_SSBIc_CTL_ACTIVATE_RESERVE_BMSK                                0x8000
#define HWIO_MODEM_SSBIc_CTL_ACTIVATE_RESERVE_SHFT                                   0xf
#define HWIO_MODEM_SSBIc_CTL_DISABLE_TERM_SYM_BMSK                                0x4000
#define HWIO_MODEM_SSBIc_CTL_DISABLE_TERM_SYM_SHFT                                   0xe
#define HWIO_MODEM_SSBIc_CTL_SLAVE_ID_BMSK                                        0x3f00
#define HWIO_MODEM_SSBIc_CTL_SLAVE_ID_SHFT                                           0x8
#define HWIO_MODEM_SSBIc_CTL_FTM_MODE_BMSK                                          0x80
#define HWIO_MODEM_SSBIc_CTL_FTM_MODE_SHFT                                           0x7
#define HWIO_MODEM_SSBIc_CTL_SEL_RD_DATA_BMSK                                       0x60
#define HWIO_MODEM_SSBIc_CTL_SEL_RD_DATA_SHFT                                        0x5
#define HWIO_MODEM_SSBIc_CTL_ENABLE_SSBI_INT_BMSK                                   0x10
#define HWIO_MODEM_SSBIc_CTL_ENABLE_SSBI_INT_SHFT                                    0x4
#define HWIO_MODEM_SSBIc_CTL_SSBI_DATA_DEL_BMSK                                      0xc
#define HWIO_MODEM_SSBIc_CTL_SSBI_DATA_DEL_SHFT                                      0x2
#define HWIO_MODEM_SSBIc_CTL_IDLE_SYMS_BMSK                                          0x3
#define HWIO_MODEM_SSBIc_CTL_IDLE_SYMS_SHFT                                            0

#define HWIO_MODEM_SSBIc_RESET_ADDR(c)                                        (MTC_REG_BASE      + 0x00001008 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_RESET_PHYS(c)                                        (MTC_REG_BASE_PHYS + 0x00001008 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_RESET_RMSK                                                  0x1
#define HWIO_MODEM_SSBIc_RESET_MAXc                                                  0x7
#define HWIO_MODEM_SSBIc_RESET_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_RESET_ADDR(c),val)
#define HWIO_MODEM_SSBIc_RESET_MICRO_RESET_BMSK                                      0x1
#define HWIO_MODEM_SSBIc_RESET_MICRO_RESET_SHFT                                        0

#define HWIO_MODEM_SSBIc_CMD_ADDR(c)                                          (MTC_REG_BASE      + 0x0000100c + 0x20 * (c))
#define HWIO_MODEM_SSBIc_CMD_PHYS(c)                                          (MTC_REG_BASE_PHYS + 0x0000100c + 0x20 * (c))
#define HWIO_MODEM_SSBIc_CMD_RMSK                                              0xfff00ff
#define HWIO_MODEM_SSBIc_CMD_MAXc                                                    0x7
#define HWIO_MODEM_SSBIc_CMD_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_CMD_ADDR(c),val)
#define HWIO_MODEM_SSBIc_CMD_SEND_TERM_SYM_BMSK                                0x8000000
#define HWIO_MODEM_SSBIc_CMD_SEND_TERM_SYM_SHFT                                     0x1b
#define HWIO_MODEM_SSBIc_CMD_WAKEUP_SLAVE_BMSK                                 0x4000000
#define HWIO_MODEM_SSBIc_CMD_WAKEUP_SLAVE_SHFT                                      0x1a
#define HWIO_MODEM_SSBIc_CMD_USE_ENABLE_BMSK                                   0x2000000
#define HWIO_MODEM_SSBIc_CMD_USE_ENABLE_SHFT                                        0x19
#define HWIO_MODEM_SSBIc_CMD_RDWRN_BMSK                                        0x1000000
#define HWIO_MODEM_SSBIc_CMD_RDWRN_SHFT                                             0x18
#define HWIO_MODEM_SSBIc_CMD_REG_ADDR_BMSK                                      0xff0000
#define HWIO_MODEM_SSBIc_CMD_REG_ADDR_SHFT                                          0x10
#define HWIO_MODEM_SSBIc_CMD_REG_DATA_BMSK                                          0xff
#define HWIO_MODEM_SSBIc_CMD_REG_DATA_SHFT                                             0

#define HWIO_MODEM_SSBIc_BYPASS_ADDR(c)                                       (MTC_REG_BASE      + 0x00001010 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_BYPASS_PHYS(c)                                       (MTC_REG_BASE_PHYS + 0x00001010 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_BYPASS_RMSK                                                 0x3
#define HWIO_MODEM_SSBIc_BYPASS_MAXc                                                 0x7
#define HWIO_MODEM_SSBIc_BYPASS_INI(c) \
        in_dword(HWIO_MODEM_SSBIc_BYPASS_ADDR(c))
#define HWIO_MODEM_SSBIc_BYPASS_INMI(c,mask) \
        in_dword_masked(HWIO_MODEM_SSBIc_BYPASS_ADDR(c), mask)
#define HWIO_MODEM_SSBIc_BYPASS_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_BYPASS_ADDR(c),val)
#define HWIO_MODEM_SSBIc_BYPASS_OUTMI(c,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_MODEM_SSBIc_BYPASS_ADDR(c),mask,val,HWIO_MODEM_SSBIc_BYPASS_INI(c));\
        HWIO_INTFREE()
#define HWIO_MODEM_SSBIc_BYPASS_OVR_VALUE_BMSK                                       0x2
#define HWIO_MODEM_SSBIc_BYPASS_OVR_VALUE_SHFT                                       0x1
#define HWIO_MODEM_SSBIc_BYPASS_OVR_MODE_BMSK                                        0x1
#define HWIO_MODEM_SSBIc_BYPASS_OVR_MODE_SHFT                                          0

#define HWIO_MODEM_SSBIc_PRIORITIES_ADDR(c)                                   (MTC_REG_BASE      + 0x00001014 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_PRIORITIES_PHYS(c)                                   (MTC_REG_BASE_PHYS + 0x00001014 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_PRIORITIES_RMSK                                            0x3f
#define HWIO_MODEM_SSBIc_PRIORITIES_MAXc                                             0x7
#define HWIO_MODEM_SSBIc_PRIORITIES_INI(c) \
        in_dword(HWIO_MODEM_SSBIc_PRIORITIES_ADDR(c))
#define HWIO_MODEM_SSBIc_PRIORITIES_INMI(c,mask) \
        in_dword_masked(HWIO_MODEM_SSBIc_PRIORITIES_ADDR(c), mask)
#define HWIO_MODEM_SSBIc_PRIORITIES_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_PRIORITIES_ADDR(c),val)
#define HWIO_MODEM_SSBIc_PRIORITIES_OUTMI(c,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_MODEM_SSBIc_PRIORITIES_ADDR(c),mask,val,HWIO_MODEM_SSBIc_PRIORITIES_INI(c));\
        HWIO_INTFREE()
#define HWIO_MODEM_SSBIc_PRIORITIES_PRIORITY1_BMSK                                  0x38
#define HWIO_MODEM_SSBIc_PRIORITIES_PRIORITY1_SHFT                                   0x3
#define HWIO_MODEM_SSBIc_PRIORITIES_PRIORITY0_BMSK                                   0x7
#define HWIO_MODEM_SSBIc_PRIORITIES_PRIORITY0_SHFT                                     0

#define HWIO_MODEM_SSBIc_RD_ADDR(c)                                           (MTC_REG_BASE      + 0x00001018 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_RD_PHYS(c)                                           (MTC_REG_BASE_PHYS + 0x00001018 + 0x20 * (c))
#define HWIO_MODEM_SSBIc_RD_RMSK                                               0x3ff00ff
#define HWIO_MODEM_SSBIc_RD_MAXc                                                     0x7
#define HWIO_MODEM_SSBIc_RD_INI(c) \
        in_dword(HWIO_MODEM_SSBIc_RD_ADDR(c))
#define HWIO_MODEM_SSBIc_RD_INMI(c,mask) \
        in_dword_masked(HWIO_MODEM_SSBIc_RD_ADDR(c), mask)
#define HWIO_MODEM_SSBIc_RD_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_RD_ADDR(c),val)
#define HWIO_MODEM_SSBIc_RD_USE_ENABLE_BMSK                                    0x2000000
#define HWIO_MODEM_SSBIc_RD_USE_ENABLE_SHFT                                         0x19
#define HWIO_MODEM_SSBIc_RD_RDWRN_BMSK                                         0x1000000
#define HWIO_MODEM_SSBIc_RD_RDWRN_SHFT                                              0x18
#define HWIO_MODEM_SSBIc_RD_REG_ADD_BMSK                                        0xff0000
#define HWIO_MODEM_SSBIc_RD_REG_ADD_SHFT                                            0x10
#define HWIO_MODEM_SSBIc_RD_REG_DATA_BMSK                                           0xff
#define HWIO_MODEM_SSBIc_RD_REG_DATA_SHFT                                              0

#define HWIO_MODEM_SSBIc_STATUS_ADDR(c)                                       (MTC_REG_BASE      + 0x0000101c + 0x20 * (c))
#define HWIO_MODEM_SSBIc_STATUS_PHYS(c)                                       (MTC_REG_BASE_PHYS + 0x0000101c + 0x20 * (c))
#define HWIO_MODEM_SSBIc_STATUS_RMSK                                                0x1f
#define HWIO_MODEM_SSBIc_STATUS_MAXc                                                 0x7
#define HWIO_MODEM_SSBIc_STATUS_INI(c) \
        in_dword(HWIO_MODEM_SSBIc_STATUS_ADDR(c))
#define HWIO_MODEM_SSBIc_STATUS_INMI(c,mask) \
        in_dword_masked(HWIO_MODEM_SSBIc_STATUS_ADDR(c), mask)
#define HWIO_MODEM_SSBIc_STATUS_OUTI(c,val) \
        out_dword(HWIO_MODEM_SSBIc_STATUS_ADDR(c),val)
#define HWIO_MODEM_SSBIc_STATUS_SSBI_DATA_IN_BMSK                                   0x10
#define HWIO_MODEM_SSBIc_STATUS_SSBI_DATA_IN_SHFT                                    0x4
#define HWIO_MODEM_SSBIc_STATUS_RD_CLOBBERED_BMSK                                    0x8
#define HWIO_MODEM_SSBIc_STATUS_RD_CLOBBERED_SHFT                                    0x3
#define HWIO_MODEM_SSBIc_STATUS_RD_READY_BMSK                                        0x4
#define HWIO_MODEM_SSBIc_STATUS_RD_READY_SHFT                                        0x2
#define HWIO_MODEM_SSBIc_STATUS_READY_BMSK                                           0x2
#define HWIO_MODEM_SSBIc_STATUS_READY_SHFT                                           0x1
#define HWIO_MODEM_SSBIc_STATUS_MCHN_BUSY_BMSK                                       0x1
#define HWIO_MODEM_SSBIc_STATUS_MCHN_BUSY_SHFT                                         0

#define HWIO_MODEM_SSBI_INTR_MASK_ADDR                                        (MTC_REG_BASE      + 0x00001100)
#define HWIO_MODEM_SSBI_INTR_MASK_PHYS                                        (MTC_REG_BASE_PHYS + 0x00001100)
#define HWIO_MODEM_SSBI_INTR_MASK_RMSK                                              0xff
#define HWIO_MODEM_SSBI_INTR_MASK_IN                                          \
        in_dword_masked(HWIO_MODEM_SSBI_INTR_MASK_ADDR, HWIO_MODEM_SSBI_INTR_MASK_RMSK)
#define HWIO_MODEM_SSBI_INTR_MASK_INM(m)                                      \
        in_dword_masked(HWIO_MODEM_SSBI_INTR_MASK_ADDR, m)
#define HWIO_MODEM_SSBI_INTR_MASK_OUT(v)                                      \
        out_dword(HWIO_MODEM_SSBI_INTR_MASK_ADDR,v)
#define HWIO_MODEM_SSBI_INTR_MASK_OUTM(m,v)                                   \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_MODEM_SSBI_INTR_MASK_ADDR,m,v,HWIO_MODEM_SSBI_INTR_MASK_IN); \
        HWIO_INTFREE()
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI7_INTR_MASK_BMSK                              0x80
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI7_INTR_MASK_SHFT                               0x7
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI6_INTR_MASK_BMSK                              0x40
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI6_INTR_MASK_SHFT                               0x6
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI5_INTR_MASK_BMSK                              0x20
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI5_INTR_MASK_SHFT                               0x5
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI4_INTR_MASK_BMSK                              0x10
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI4_INTR_MASK_SHFT                               0x4
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI3_INTR_MASK_BMSK                               0x8
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI3_INTR_MASK_SHFT                               0x3
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI2_INTR_MASK_BMSK                               0x4
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI2_INTR_MASK_SHFT                               0x2
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI1_INTR_MASK_BMSK                               0x2
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI1_INTR_MASK_SHFT                               0x1
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI0_INTR_MASK_BMSK                               0x1
#define HWIO_MODEM_SSBI_INTR_MASK_SSBI0_INTR_MASK_SHFT                                 0

#define HWIO_MODEM_SSBI_INTR_CLEAR_ADDR                                       (MTC_REG_BASE      + 0x00001104)
#define HWIO_MODEM_SSBI_INTR_CLEAR_PHYS                                       (MTC_REG_BASE_PHYS + 0x00001104)
#define HWIO_MODEM_SSBI_INTR_CLEAR_RMSK                                             0xff
#define HWIO_MODEM_SSBI_INTR_CLEAR_OUT(v)                                     \
        out_dword(HWIO_MODEM_SSBI_INTR_CLEAR_ADDR,v)
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI7_INTR_CLEAR_BMSK                            0x80
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI7_INTR_CLEAR_SHFT                             0x7
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI6_INTR_CLEAR_BMSK                            0x40
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI6_INTR_CLEAR_SHFT                             0x6
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI5_INTR_CLEAR_BMSK                            0x20
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI5_INTR_CLEAR_SHFT                             0x5
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI4_INTR_CLEAR_BMSK                            0x10
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI4_INTR_CLEAR_SHFT                             0x4
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI3_INTR_CLEAR_BMSK                             0x8
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI3_INTR_CLEAR_SHFT                             0x3
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI2_INTR_CLEAR_BMSK                             0x4
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI2_INTR_CLEAR_SHFT                             0x2
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI1_INTR_CLEAR_BMSK                             0x2
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI1_INTR_CLEAR_SHFT                             0x1
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI0_INTR_CLEAR_BMSK                             0x1
#define HWIO_MODEM_SSBI_INTR_CLEAR_SSBI0_INTR_CLEAR_SHFT                               0

#define HWIO_MODEM_SSBI_INTR_STATUS_ADDR                                      (MTC_REG_BASE      + 0x00001108)
#define HWIO_MODEM_SSBI_INTR_STATUS_PHYS                                      (MTC_REG_BASE_PHYS + 0x00001108)
#define HWIO_MODEM_SSBI_INTR_STATUS_RMSK                                            0xff
#define HWIO_MODEM_SSBI_INTR_STATUS_IN                                        \
        in_dword_masked(HWIO_MODEM_SSBI_INTR_STATUS_ADDR, HWIO_MODEM_SSBI_INTR_STATUS_RMSK)
#define HWIO_MODEM_SSBI_INTR_STATUS_INM(m)                                    \
        in_dword_masked(HWIO_MODEM_SSBI_INTR_STATUS_ADDR, m)
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI7_INTR_STATUS_BMSK                          0x80
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI7_INTR_STATUS_SHFT                           0x7
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI6_INTR_STATUS_BMSK                          0x40
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI6_INTR_STATUS_SHFT                           0x6
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI5_INTR_STATUS_BMSK                          0x20
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI5_INTR_STATUS_SHFT                           0x5
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI4_INTR_STATUS_BMSK                          0x10
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI4_INTR_STATUS_SHFT                           0x4
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI3_INTR_STATUS_BMSK                           0x8
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI3_INTR_STATUS_SHFT                           0x3
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI2_INTR_STATUS_BMSK                           0x4
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI2_INTR_STATUS_SHFT                           0x2
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI1_INTR_STATUS_BMSK                           0x2
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI1_INTR_STATUS_SHFT                           0x1
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI0_INTR_STATUS_BMSK                           0x1
#define HWIO_MODEM_SSBI_INTR_STATUS_SSBI0_INTR_STATUS_SHFT                             0


#define HWIO_BBRX_CTL_ADDR                                                    (MTC_REG_BASE      + 0x0000125c)
#define HWIO_BBRX_CTL_PHYS                                                    (MTC_REG_BASE_PHYS + 0x0000125c)
#define HWIO_BBRX_CTL_RMSK                                                           0x3
#define HWIO_BBRX_CTL_IN                                                      \
        in_dword_masked(HWIO_BBRX_CTL_ADDR, HWIO_BBRX_CTL_RMSK)
#define HWIO_BBRX_CTL_INM(m)                                                  \
        in_dword_masked(HWIO_BBRX_CTL_ADDR, m)
#define HWIO_BBRX_CTL_OUT(v)                                                  \
        out_dword(HWIO_BBRX_CTL_ADDR,v)
#define HWIO_BBRX_CTL_OUTM(m,v)                                               \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_BBRX_CTL_ADDR,m,v,HWIO_BBRX_CTL_IN); \
        HWIO_INTFREE()
#define HWIO_BBRX_CTL_BBRX_RST_GRFC_EN_BMSK                                          0x2
#define HWIO_BBRX_CTL_BBRX_RST_GRFC_EN_SHFT                                          0x1
#define HWIO_BBRX_CTL_BBRX_RST_N_BMSK                                                0x1
#define HWIO_BBRX_CTL_BBRX_RST_N_SHFT                                                  0


#define HWIO_BP1_SCH2_AMPL_MON_ADDR                                           (NAV_REG_BASE      + 0x00000248)
#define HWIO_BP1_SCH2_AMPL_MON_PHYS                                           (NAV_REG_BASE_PHYS + 0x00000248)
#define HWIO_BP1_SCH2_AMPL_MON_RMSK                                           0xffffffff
#define HWIO_BP1_SCH2_AMPL_MON_IN                                             \
        in_dword_masked(HWIO_BP1_SCH2_AMPL_MON_ADDR, HWIO_BP1_SCH2_AMPL_MON_RMSK)
#define HWIO_BP1_SCH2_AMPL_MON_INM(m)                                         \
        in_dword_masked(HWIO_BP1_SCH2_AMPL_MON_ADDR, m)
#define HWIO_BP1_SCH2_AMPL_MON_BP1_SCH2_AMPLMONQ_BMSK                           0x3ff800
#define HWIO_BP1_SCH2_AMPL_MON_BP1_SCH2_AMPLMONQ_SHFT                                0xb
#define HWIO_BP1_SCH2_AMPL_MON_BP1_SCH2_AMPLMONI_BMSK                              0x7ff
#define HWIO_BP1_SCH2_AMPL_MON_BP1_SCH2_AMPLMONI_SHFT                                  0

#define HWIO_BP1_SCH3_AMPL_MON_ADDR                                           (NAV_REG_BASE      + 0x0000024c)
#define HWIO_BP1_SCH3_AMPL_MON_PHYS                                           (NAV_REG_BASE_PHYS + 0x0000024c)
#define HWIO_BP1_SCH3_AMPL_MON_RMSK                                           0xffffffff
#define HWIO_BP1_SCH3_AMPL_MON_IN                                             \
        in_dword_masked(HWIO_BP1_SCH3_AMPL_MON_ADDR, HWIO_BP1_SCH3_AMPL_MON_RMSK)
#define HWIO_BP1_SCH3_AMPL_MON_INM(m)                                         \
        in_dword_masked(HWIO_BP1_SCH3_AMPL_MON_ADDR, m)
#define HWIO_BP1_SCH3_AMPL_MON_BP1_SCH3_AMPLMONQ_BMSK                           0x3ff800
#define HWIO_BP1_SCH3_AMPL_MON_BP1_SCH3_AMPLMONQ_SHFT                                0xb
#define HWIO_BP1_SCH3_AMPL_MON_BP1_SCH3_AMPLMONI_BMSK                              0x7ff
#define HWIO_BP1_SCH3_AMPL_MON_BP1_SCH3_AMPLMONI_SHFT                                  0


#define HWIO_GSM_RXF_STAGE5_IIR_COEF_ADDR                                     (EDGE_REG_BASE      + 0x00000340)
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_PHYS                                     (EDGE_REG_BASE_PHYS + 0x00000340)
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_RMSK                                     0x7fffffff
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_IN                                       \
        in_dword_masked(HWIO_GSM_RXF_STAGE5_IIR_COEF_ADDR, HWIO_GSM_RXF_STAGE5_IIR_COEF_RMSK)
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_INM(m)                                   \
        in_dword_masked(HWIO_GSM_RXF_STAGE5_IIR_COEF_ADDR, m)
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_OUT(v)                                   \
        out_dword(HWIO_GSM_RXF_STAGE5_IIR_COEF_ADDR,v)
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_OUTM(m,v)                                \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GSM_RXF_STAGE5_IIR_COEF_ADDR,m,v,HWIO_GSM_RXF_STAGE5_IIR_COEF_IN); \
        HWIO_INTFREE()
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR2_COEF2_BMSK                     0x7f000000
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR2_COEF2_SHFT                           0x18
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR2_COEF1_BMSK                       0x7f0000
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR2_COEF1_SHFT                           0x10
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR1_COEF2_BMSK                         0x7f00
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR1_COEF2_SHFT                            0x8
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR1_COEF1_BMSK                           0x7f
#define HWIO_GSM_RXF_STAGE5_IIR_COEF_STG5_IIR1_COEF1_SHFT                              0


#define HWIO_GSM_RXF_STAGE5_FIR_COEF_ADDR                                     (EDGE_REG_BASE      + 0x00000344)
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_PHYS                                     (EDGE_REG_BASE_PHYS + 0x00000344)
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_RMSK                                         0xffff
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_IN                                       \
        in_dword_masked(HWIO_GSM_RXF_STAGE5_FIR_COEF_ADDR, HWIO_GSM_RXF_STAGE5_FIR_COEF_RMSK)
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_INM(m)                                   \
        in_dword_masked(HWIO_GSM_RXF_STAGE5_FIR_COEF_ADDR, m)
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_OUT(v)                                   \
        out_dword(HWIO_GSM_RXF_STAGE5_FIR_COEF_ADDR,v)
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_OUTM(m,v)                                \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GSM_RXF_STAGE5_FIR_COEF_ADDR,m,v,HWIO_GSM_RXF_STAGE5_FIR_COEF_IN); \
        HWIO_INTFREE()
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF2_BMSK                         0xf000
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF2_SHFT                            0xc
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF1_BMSK                          0xf00
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF1_SHFT                            0x8
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF0_BMSK                           0xf0
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR2_COEF0_SHFT                            0x4
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR1_COEF1_BMSK                            0xf
#define HWIO_GSM_RXF_STAGE5_FIR_COEF_STG5_FIR1_COEF1_SHFT                              0


#define HWIO_RXFE_WBw_TOP_CFG_ADDR(w)                                         (RXFE_REG_BASE      + 0x00004000 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CFG_PHYS(w)                                         (RXFE_REG_BASE_PHYS + 0x00004000 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CFG_RMSK                                                   0x3
#define HWIO_RXFE_WBw_TOP_CFG_MAXw                                                   0x2
#define HWIO_RXFE_WBw_TOP_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_TOP_CFG_ADDR(w))
#define HWIO_RXFE_WBw_TOP_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_TOP_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_TOP_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_TOP_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_TOP_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_TOP_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_TOP_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_TOP_CFG_TEMP_BMSK                                              0x2
#define HWIO_RXFE_WBw_TOP_CFG_TEMP_SHFT                                              0x1
#define HWIO_RXFE_WBw_TOP_CFG_USE_REMAPTABLE_BMSK                                    0x1
#define HWIO_RXFE_WBw_TOP_CFG_USE_REMAPTABLE_SHFT                                      0

#define HWIO_RXFE_WBw_TOP_CTL_ADDR(w)                                         (RXFE_REG_BASE      + 0x00004004 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CTL_PHYS(w)                                         (RXFE_REG_BASE_PHYS + 0x00004004 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CTL_RMSK                                               0xfffff
#define HWIO_RXFE_WBw_TOP_CTL_MAXw                                                   0x2
#define HWIO_RXFE_WBw_TOP_CTL_INI(w) \
        in_dword(HWIO_RXFE_WBw_TOP_CTL_ADDR(w))
#define HWIO_RXFE_WBw_TOP_CTL_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_TOP_CTL_ADDR(w), mask)
#define HWIO_RXFE_WBw_TOP_CTL_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_TOP_CTL_ADDR(w),val)
#define HWIO_RXFE_WBw_TOP_CTL_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_TOP_CTL_ADDR(w),mask,val,HWIO_RXFE_WBw_TOP_CTL_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_TOP_CTL_LNA_STATE_BMSK                                     0xe0000
#define HWIO_RXFE_WBw_TOP_CTL_LNA_STATE_SHFT                                        0x11
#define HWIO_RXFE_WBw_TOP_CTL_CAPT_ON_TRIG_BMSK                                  0x10000
#define HWIO_RXFE_WBw_TOP_CTL_CAPT_ON_TRIG_SHFT                                     0x10
#define HWIO_RXFE_WBw_TOP_CTL_LOAD_ON_TRIG_BMSK                                   0x8000
#define HWIO_RXFE_WBw_TOP_CTL_LOAD_ON_TRIG_SHFT                                      0xf
#define HWIO_RXFE_WBw_TOP_CTL_STSP_ON_TRIG_BMSK                                   0x6000
#define HWIO_RXFE_WBw_TOP_CTL_STSP_ON_TRIG_SHFT                                      0xd
#define HWIO_RXFE_WBw_TOP_CTL_FRAC_CNTR_VAL_BMSK                                  0x1ff8
#define HWIO_RXFE_WBw_TOP_CTL_FRAC_CNTR_VAL_SHFT                                     0x3
#define HWIO_RXFE_WBw_TOP_CTL_USE_FRAC_CNTR_BMSK                                     0x4
#define HWIO_RXFE_WBw_TOP_CTL_USE_FRAC_CNTR_SHFT                                     0x2
#define HWIO_RXFE_WBw_TOP_CTL_TRIG_SRC_SEL_BMSK                                      0x2
#define HWIO_RXFE_WBw_TOP_CTL_TRIG_SRC_SEL_SHFT                                      0x1
#define HWIO_RXFE_WBw_TOP_CTL_CLEAR_BMSK                                             0x1
#define HWIO_RXFE_WBw_TOP_CTL_CLEAR_SHFT                                               0

#define HWIO_RXFE_WBw_TOP_CMD_ADDR(w)                                         (RXFE_REG_BASE      + 0x00004008 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CMD_PHYS(w)                                         (RXFE_REG_BASE_PHYS + 0x00004008 + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_CMD_RMSK                                                   0x7
#define HWIO_RXFE_WBw_TOP_CMD_MAXw                                                   0x2
#define HWIO_RXFE_WBw_TOP_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_TOP_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_TOP_CMD_WB_GATE_IMM_TRIG_BMSK                                  0x4
#define HWIO_RXFE_WBw_TOP_CMD_WB_GATE_IMM_TRIG_SHFT                                  0x2
#define HWIO_RXFE_WBw_TOP_CMD_WB_GATE_ARM_BMSK                                       0x2
#define HWIO_RXFE_WBw_TOP_CMD_WB_GATE_ARM_SHFT                                       0x1
#define HWIO_RXFE_WBw_TOP_CMD_SWI_STMR_TRIG_BMSK                                     0x1
#define HWIO_RXFE_WBw_TOP_CMD_SWI_STMR_TRIG_SHFT                                       0

#define HWIO_RXFE_WBw_TOP_STATUS_ADDR(w)                                      (RXFE_REG_BASE      + 0x0000400c + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_STATUS_PHYS(w)                                      (RXFE_REG_BASE_PHYS + 0x0000400c + 0x1000 * (w))
#define HWIO_RXFE_WBw_TOP_STATUS_RMSK                                                0x7
#define HWIO_RXFE_WBw_TOP_STATUS_MAXw                                                0x2
#define HWIO_RXFE_WBw_TOP_STATUS_INI(w) \
        in_dword(HWIO_RXFE_WBw_TOP_STATUS_ADDR(w))
#define HWIO_RXFE_WBw_TOP_STATUS_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_TOP_STATUS_ADDR(w), mask)
#define HWIO_RXFE_WBw_TOP_STATUS_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_TOP_STATUS_ADDR(w),val)
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATE_OPEN_BMSK                                   0x4
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATE_OPEN_SHFT                                   0x2
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATED_TRIGGERED_BMSK                             0x2
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATED_TRIGGERED_SHFT                             0x1
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATE_ARMED_BMSK                                  0x1
#define HWIO_RXFE_WBw_TOP_STATUS_WB_GATE_ARMED_SHFT                                    0

#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ADDR(w)                           (RXFE_REG_BASE      + 0x00004010 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_PHYS(w)                           (RXFE_REG_BASE_PHYS + 0x00004010 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_RMSK                              0xffffffff
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_MAXw                                     0x2
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ACTION_SAMPLE_BMSK                0xffffffff
#define HWIO_RXFE_WBw_WB_GATE_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                         0

#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_ADDR(w)                             (RXFE_REG_BASE      + 0x00004014 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004014 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_RMSK                                 0x7ffffff
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_MAXw                                       0x2
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_INI(w) \
        in_dword(HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_ADDR(w))
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_ADDR(w), mask)
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_ADDR(w),val)
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDT_ARMED_BMSK                 0x4000000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDT_ARMED_SHFT                      0x1a
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDT_TRIGGERED_BMSK             0x2000000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDT_TRIGGERED_SHFT                  0x19
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDATED_BMSK                    0x1000000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_IQMC_UPDATED_SHFT                         0x18
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDT_ARMED_BMSK               0x800000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDT_ARMED_SHFT                   0x17
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDT_TRIGGERED_BMSK           0x400000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDT_TRIGGERED_SHFT               0x16
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDATED_BMSK                  0x200000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_UPDATED_SHFT                      0x15
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_START_ARMED_BMSK              0x100000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_START_ARMED_SHFT                  0x14
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_START_TRIGGERED_BMSK           0x80000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_START_TRIGGERED_SHFT              0x13
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STARTED_BMSK                   0x40000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STARTED_SHFT                      0x12
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOP_ARMED_BMSK                0x20000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOP_ARMED_SHFT                   0x11
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOP_TRIGGERED_BMSK            0x10000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOP_TRIGGERED_SHFT               0x10
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOPPED_BMSK                    0x8000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_CG_STOPPED_SHFT                       0xf
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDT_ARMED_BMSK                 0x4000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDT_ARMED_SHFT                    0xe
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDT_TRIGGERED_BMSK             0x2000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDT_TRIGGERED_SHFT                0xd
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDATED_BMSK                    0x1000
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_UPDATED_SHFT                       0xc
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_START_ARMED_BMSK                 0x800
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_START_ARMED_SHFT                   0xb
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_START_TRIGGERED_BMSK             0x400
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_START_TRIGGERED_SHFT               0xa
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STARTED_BMSK                     0x200
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STARTED_SHFT                       0x9
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOP_ARMED_BMSK                  0x100
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOP_ARMED_SHFT                    0x8
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOP_TRIGGERED_BMSK               0x80
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOP_TRIGGERED_SHFT                0x7
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOPPED_BMSK                      0x40
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBDC_FG_STOPPED_SHFT                       0x6
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_START_ARMED_BMSK                    0x20
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_START_ARMED_SHFT                     0x5
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_START_TRIGGERED_BMSK                0x10
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_START_TRIGGERED_SHFT                 0x4
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STARTED_BMSK                         0x8
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STARTED_SHFT                         0x3
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOP_ARMED_BMSK                      0x4
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOP_ARMED_SHFT                      0x2
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOP_TRIGGERED_BMSK                  0x2
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOP_TRIGGERED_SHFT                  0x1
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOPPED_BMSK                         0x1
#define HWIO_RXFE_WBw_WB_TIMINGCTL_STATUS_WBPWR_STOPPED_SHFT                           0

#define HWIO_RXFE_WBw_EC_CFG_ADDR(w)                                          (RXFE_REG_BASE      + 0x00004018 + 0x1000 * (w))
#define HWIO_RXFE_WBw_EC_CFG_PHYS(w)                                          (RXFE_REG_BASE_PHYS + 0x00004018 + 0x1000 * (w))
#define HWIO_RXFE_WBw_EC_CFG_RMSK                                                    0x1
#define HWIO_RXFE_WBw_EC_CFG_MAXw                                                    0x2
#define HWIO_RXFE_WBw_EC_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_EC_CFG_ADDR(w))
#define HWIO_RXFE_WBw_EC_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_EC_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_EC_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_EC_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_EC_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_EC_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_EC_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_EC_CFG_Y2_BYPASS_BMSK                                          0x1
#define HWIO_RXFE_WBw_EC_CFG_Y2_BYPASS_SHFT                                            0

#define HWIO_RXFE_WBw_VSHIFTER_CFG_ADDR(w)                                    (RXFE_REG_BASE      + 0x0000401c + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSHIFTER_CFG_PHYS(w)                                    (RXFE_REG_BASE_PHYS + 0x0000401c + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSHIFTER_CFG_RMSK                                              0x3
#define HWIO_RXFE_WBw_VSHIFTER_CFG_MAXw                                              0x2
#define HWIO_RXFE_WBw_VSHIFTER_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSHIFTER_CFG_ADDR(w))
#define HWIO_RXFE_WBw_VSHIFTER_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSHIFTER_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSHIFTER_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSHIFTER_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_VSHIFTER_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSHIFTER_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_VSHIFTER_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSHIFTER_CFG_SHIFT_BMSK                                        0x3
#define HWIO_RXFE_WBw_VSHIFTER_CFG_SHIFT_SHFT                                          0

#define HWIO_RXFE_WBw_WBPWR_CFG_ADDR(w)                                       (RXFE_REG_BASE      + 0x00004020 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CFG_PHYS(w)                                       (RXFE_REG_BASE_PHYS + 0x00004020 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CFG_RMSK                                                0x7f
#define HWIO_RXFE_WBw_WBPWR_CFG_MAXw                                                 0x2
#define HWIO_RXFE_WBw_WBPWR_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBPWR_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBPWR_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBPWR_CFG_DEC_FACTOR_BMSK                                     0x70
#define HWIO_RXFE_WBw_WBPWR_CFG_DEC_FACTOR_SHFT                                      0x4
#define HWIO_RXFE_WBw_WBPWR_CFG_REPEAT_MODE_ENABLE_BMSK                              0x8
#define HWIO_RXFE_WBw_WBPWR_CFG_REPEAT_MODE_ENABLE_SHFT                              0x3
#define HWIO_RXFE_WBw_WBPWR_CFG_START_ALWAYS_ARMED_BMSK                              0x4
#define HWIO_RXFE_WBw_WBPWR_CFG_START_ALWAYS_ARMED_SHFT                              0x2
#define HWIO_RXFE_WBw_WBPWR_CFG_ENABLE_PREFILTER_BMSK                                0x2
#define HWIO_RXFE_WBw_WBPWR_CFG_ENABLE_PREFILTER_SHFT                                0x1
#define HWIO_RXFE_WBw_WBPWR_CFG_ENABLE_BMSK                                          0x1
#define HWIO_RXFE_WBw_WBPWR_CFG_ENABLE_SHFT                                            0

#define HWIO_RXFE_WBw_WBPWR_CMD_ADDR(w)                                       (RXFE_REG_BASE      + 0x00004024 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CMD_PHYS(w)                                       (RXFE_REG_BASE_PHYS + 0x00004024 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CMD_RMSK                                                0x3f
#define HWIO_RXFE_WBw_WBPWR_CMD_MAXw                                                 0x2
#define HWIO_RXFE_WBw_WBPWR_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_CMD_ACC_CLR_BMSK                                        0x20
#define HWIO_RXFE_WBw_WBPWR_CMD_ACC_CLR_SHFT                                         0x5
#define HWIO_RXFE_WBw_WBPWR_CMD_MAX_CLR_BMSK                                        0x10
#define HWIO_RXFE_WBw_WBPWR_CMD_MAX_CLR_SHFT                                         0x4
#define HWIO_RXFE_WBw_WBPWR_CMD_START_IMM_TRIG_BMSK                                  0x8
#define HWIO_RXFE_WBw_WBPWR_CMD_START_IMM_TRIG_SHFT                                  0x3
#define HWIO_RXFE_WBw_WBPWR_CMD_START_ARM_BMSK                                       0x4
#define HWIO_RXFE_WBw_WBPWR_CMD_START_ARM_SHFT                                       0x2
#define HWIO_RXFE_WBw_WBPWR_CMD_STOP_IMM_TRIG_BMSK                                   0x2
#define HWIO_RXFE_WBw_WBPWR_CMD_STOP_IMM_TRIG_SHFT                                   0x1
#define HWIO_RXFE_WBw_WBPWR_CMD_STOP_ARM_BMSK                                        0x1
#define HWIO_RXFE_WBw_WBPWR_CMD_STOP_ARM_SHFT                                          0

#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ADDR(w)                       (RXFE_REG_BASE      + 0x00004028 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_PHYS(w)                       (RXFE_REG_BASE_PHYS + 0x00004028 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_RMSK                          0xffffffff
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_MAXw                                 0x2
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ACTION_SAMPLE_BMSK            0xffffffff
#define HWIO_RXFE_WBw_WBPWR_START_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                     0

#define HWIO_RXFE_WBw_WBPWR_START_MASK_ADDR(w)                                (RXFE_REG_BASE      + 0x0000402c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_START_MASK_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x0000402c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_START_MASK_RMSK                                   0xffffffff
#define HWIO_RXFE_WBw_WBPWR_START_MASK_MAXw                                          0x2
#define HWIO_RXFE_WBw_WBPWR_START_MASK_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_START_MASK_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_START_MASK_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_START_MASK_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_START_MASK_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_START_MASK_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_START_MASK_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBPWR_START_MASK_ADDR(w),mask,val,HWIO_RXFE_WBw_WBPWR_START_MASK_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBPWR_START_MASK_MASK_BMSK                              0xffffffff
#define HWIO_RXFE_WBw_WBPWR_START_MASK_MASK_SHFT                                       0

#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ADDR(w)                        (RXFE_REG_BASE      + 0x00004030 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_PHYS(w)                        (RXFE_REG_BASE_PHYS + 0x00004030 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_RMSK                           0xffffffff
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_MAXw                                  0x2
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ACTION_SAMPLE_BMSK             0xffffffff
#define HWIO_RXFE_WBw_WBPWR_STOP_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                      0

#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_ADDR(w)                                 (RXFE_REG_BASE      + 0x00004034 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_PHYS(w)                                 (RXFE_REG_BASE_PHYS + 0x00004034 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_RMSK                                    0xffffffff
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_MAXw                                           0x2
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_FINAL_ACC_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_FINAL_ACC_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_FINAL_ACC_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_FINAL_ACC_BMSK                          0xffffffff
#define HWIO_RXFE_WBw_WBPWR_FINAL_ACC_FINAL_ACC_SHFT                                   0

#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_ADDR(w)                                  (RXFE_REG_BASE      + 0x00004038 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_PHYS(w)                                  (RXFE_REG_BASE_PHYS + 0x00004038 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_RMSK                                     0xffffffff
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_MAXw                                            0x2
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_CURR_ACC_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_CURR_ACC_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_CURR_ACC_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_CURR_ACC_BMSK                            0xffffffff
#define HWIO_RXFE_WBw_WBPWR_CURR_ACC_CURR_ACC_SHFT                                     0

#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_ADDR(w)                                   (RXFE_REG_BASE      + 0x0000403c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_PHYS(w)                                   (RXFE_REG_BASE_PHYS + 0x0000403c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_RMSK                                      0xffffffff
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_MAXw                                             0x2
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBPWR_MAX_ACC_ADDR(w))
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBPWR_MAX_ACC_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBPWR_MAX_ACC_ADDR(w),val)
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_MAX_ACC_BMSK                              0xffffffff
#define HWIO_RXFE_WBw_WBPWR_MAX_ACC_MAX_ACC_SHFT                                       0

#define HWIO_RXFE_WBw_WBDC_FG_CFG_ADDR(w)                                     (RXFE_REG_BASE      + 0x00004040 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_CFG_PHYS(w)                                     (RXFE_REG_BASE_PHYS + 0x00004040 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_CFG_RMSK                                              0x1f
#define HWIO_RXFE_WBw_WBDC_FG_CFG_MAXw                                               0x2
#define HWIO_RXFE_WBw_WBDC_FG_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_CFG_OVRD_ENABLE_BMSK                                  0x10
#define HWIO_RXFE_WBw_WBDC_FG_CFG_OVRD_ENABLE_SHFT                                   0x4
#define HWIO_RXFE_WBw_WBDC_FG_CFG_ACC_UPDATE_MODE_BMSK                               0x8
#define HWIO_RXFE_WBw_WBDC_FG_CFG_ACC_UPDATE_MODE_SHFT                               0x3
#define HWIO_RXFE_WBw_WBDC_FG_CFG_SBI_SRC_SEL_BMSK                                   0x4
#define HWIO_RXFE_WBw_WBDC_FG_CFG_SBI_SRC_SEL_SHFT                                   0x2
#define HWIO_RXFE_WBw_WBDC_FG_CFG_DC_CAN_BYPASS_BMSK                                 0x2
#define HWIO_RXFE_WBw_WBDC_FG_CFG_DC_CAN_BYPASS_SHFT                                 0x1
#define HWIO_RXFE_WBw_WBDC_FG_CFG_ENABLE_BMSK                                        0x1
#define HWIO_RXFE_WBw_WBDC_FG_CFG_ENABLE_SHFT                                          0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_ADDR(w)                                (RXFE_REG_BASE      + 0x00004044 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x00004044 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_RMSK                                         0x1f
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_MAXw                                          0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_BYPASS_BMSK                                  0x10
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_BYPASS_SHFT                                   0x4
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_LEFT_SHIFT_BMSK                               0xf
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CFG_LEFT_SHIFT_SHFT                                 0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_ADDR(w)                                (RXFE_REG_BASE      + 0x00004048 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x00004048 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_RMSK                                          0xf
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_MAXw                                          0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_LEFT_SHIFT_BMSK                               0xf
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CFG_LEFT_SHIFT_SHFT                                 0

#define HWIO_RXFE_WBw_WBDC_FG_CMD_ADDR(w)                                     (RXFE_REG_BASE      + 0x0000404c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_CMD_PHYS(w)                                     (RXFE_REG_BASE_PHYS + 0x0000404c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_CMD_RMSK                                              0xff
#define HWIO_RXFE_WBw_WBDC_FG_CMD_MAXw                                               0x2
#define HWIO_RXFE_WBw_WBDC_FG_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_CMD_ACC_CLR_BMSK                                      0x80
#define HWIO_RXFE_WBw_WBDC_FG_CMD_ACC_CLR_SHFT                                       0x7
#define HWIO_RXFE_WBw_WBDC_FG_CMD_ACC_LOAD_BMSK                                     0x40
#define HWIO_RXFE_WBw_WBDC_FG_CMD_ACC_LOAD_SHFT                                      0x6
#define HWIO_RXFE_WBw_WBDC_FG_CMD_UPDATE_IMM_TRIG_BMSK                              0x20
#define HWIO_RXFE_WBw_WBDC_FG_CMD_UPDATE_IMM_TRIG_SHFT                               0x5
#define HWIO_RXFE_WBw_WBDC_FG_CMD_UPDATE_ARM_BMSK                                   0x10
#define HWIO_RXFE_WBw_WBDC_FG_CMD_UPDATE_ARM_SHFT                                    0x4
#define HWIO_RXFE_WBw_WBDC_FG_CMD_START_IMM_TRIG_BMSK                                0x8
#define HWIO_RXFE_WBw_WBDC_FG_CMD_START_IMM_TRIG_SHFT                                0x3
#define HWIO_RXFE_WBw_WBDC_FG_CMD_START_ARM_BMSK                                     0x4
#define HWIO_RXFE_WBw_WBDC_FG_CMD_START_ARM_SHFT                                     0x2
#define HWIO_RXFE_WBw_WBDC_FG_CMD_STOP_IMM_TRIG_BMSK                                 0x2
#define HWIO_RXFE_WBw_WBDC_FG_CMD_STOP_IMM_TRIG_SHFT                                 0x1
#define HWIO_RXFE_WBw_WBDC_FG_CMD_STOP_ARM_BMSK                                      0x1
#define HWIO_RXFE_WBw_WBDC_FG_CMD_STOP_ARM_SHFT                                        0

#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ADDR(w)                     (RXFE_REG_BASE      + 0x00004050 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_PHYS(w)                     (RXFE_REG_BASE_PHYS + 0x00004050 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_RMSK                        0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_MAXw                               0x2
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ACTION_SAMPLE_BMSK          0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_START_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                   0

#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ADDR(w)                      (RXFE_REG_BASE      + 0x00004054 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_PHYS(w)                      (RXFE_REG_BASE_PHYS + 0x00004054 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_RMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_MAXw                                0x2
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ACTION_SAMPLE_BMSK           0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_STOP_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                    0

#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ADDR(w)                    (RXFE_REG_BASE      + 0x00004058 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_PHYS(w)                    (RXFE_REG_BASE_PHYS + 0x00004058 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_RMSK                       0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_MAXw                              0x2
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ACTION_SAMPLE_BMSK         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                  0

#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_ADDR(w)                            (RXFE_REG_BASE      + 0x0000405c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_PHYS(w)                            (RXFE_REG_BASE_PHYS + 0x0000405c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_RMSK                                    0x3ff
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_MAXw                                      0x2
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_DELAY_VALUE_BMSK                        0x3ff
#define HWIO_RXFE_WBw_WBDC_FG_UPDATE_DELAY_DELAY_VALUE_SHFT                            0

#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_ADDR(w)                                  (RXFE_REG_BASE      + 0x00004060 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_PHYS(w)                                  (RXFE_REG_BASE_PHYS + 0x00004060 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_RMSK                                        0x3ffff
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_MAXw                                            0x2
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_OVRD_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_OVRD_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_OVRD_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_OVRD_I_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_OVRD_I_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_OVRD_I_BMSK                                 0x3ffff
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_I_OVRD_I_SHFT                                       0

#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_ADDR(w)                                  (RXFE_REG_BASE      + 0x00004064 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_PHYS(w)                                  (RXFE_REG_BASE_PHYS + 0x00004064 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_RMSK                                        0x3ffff
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_MAXw                                            0x2
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_OVRD_Q_BMSK                                 0x3ffff
#define HWIO_RXFE_WBw_WBDC_FG_OVRD_Q_OVRD_Q_SHFT                                       0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004068 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004068 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_CURR_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_I_CURR_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x0000406c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x0000406c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_CURR_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_CURR_Q_CURR_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004070 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004070 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_PREV_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_I_PREV_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x00004074 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004074 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_PREV_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_PREV_Q_PREV_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004078 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004078 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_LOAD_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_I_LOAD_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x0000407c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x0000407c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_LOAD_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST1_LOAD_Q_LOAD_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004080 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004080 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_CURR_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_I_CURR_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x00004084 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004084 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_CURR_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_CURR_Q_CURR_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004088 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004088 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_PREV_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_I_PREV_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x0000408c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x0000408c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_PREV_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_PREV_Q_PREV_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_ADDR(w)                             (RXFE_REG_BASE      + 0x00004090 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004090 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_LOAD_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_I_LOAD_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x00004094 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x00004094 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_LOAD_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_FG_EST2_LOAD_Q_LOAD_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_CFG_ADDR(w)                                     (RXFE_REG_BASE      + 0x00004098 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_CFG_PHYS(w)                                     (RXFE_REG_BASE_PHYS + 0x00004098 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_CFG_RMSK                                               0x7
#define HWIO_RXFE_WBw_WBDC_CG_CFG_MAXw                                               0x2
#define HWIO_RXFE_WBw_WBDC_CG_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_CFG_SBI_SRC_SEL_BMSK                                   0x4
#define HWIO_RXFE_WBw_WBDC_CG_CFG_SBI_SRC_SEL_SHFT                                   0x2
#define HWIO_RXFE_WBw_WBDC_CG_CFG_ACC_UPDATE_MODE_BMSK                               0x2
#define HWIO_RXFE_WBw_WBDC_CG_CFG_ACC_UPDATE_MODE_SHFT                               0x1
#define HWIO_RXFE_WBw_WBDC_CG_CFG_ENABLE_BMSK                                        0x1
#define HWIO_RXFE_WBw_WBDC_CG_CFG_ENABLE_SHFT                                          0

#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_ADDR(w)                                (RXFE_REG_BASE      + 0x0000409c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x0000409c + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_RMSK                                         0x1f
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_MAXw                                          0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_BYPASS_BMSK                                  0x10
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_BYPASS_SHFT                                   0x4
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_LEFT_SHIFT_BMSK                               0xf
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CFG_LEFT_SHIFT_SHFT                                 0

#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_ADDR(w)                                (RXFE_REG_BASE      + 0x000040a0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x000040a0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_RMSK                                          0xf
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_MAXw                                          0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_LEFT_SHIFT_BMSK                               0xf
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CFG_LEFT_SHIFT_SHFT                                 0

#define HWIO_RXFE_WBw_WBDC_CG_CMD_ADDR(w)                                     (RXFE_REG_BASE      + 0x000040a4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_CMD_PHYS(w)                                     (RXFE_REG_BASE_PHYS + 0x000040a4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_CMD_RMSK                                              0xff
#define HWIO_RXFE_WBw_WBDC_CG_CMD_MAXw                                               0x2
#define HWIO_RXFE_WBw_WBDC_CG_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_CMD_ACC_CLR_BMSK                                      0x80
#define HWIO_RXFE_WBw_WBDC_CG_CMD_ACC_CLR_SHFT                                       0x7
#define HWIO_RXFE_WBw_WBDC_CG_CMD_ACC_LOAD_BMSK                                     0x40
#define HWIO_RXFE_WBw_WBDC_CG_CMD_ACC_LOAD_SHFT                                      0x6
#define HWIO_RXFE_WBw_WBDC_CG_CMD_UPDATE_IMM_TRIG_BMSK                              0x20
#define HWIO_RXFE_WBw_WBDC_CG_CMD_UPDATE_IMM_TRIG_SHFT                               0x5
#define HWIO_RXFE_WBw_WBDC_CG_CMD_UPDATE_ARM_BMSK                                   0x10
#define HWIO_RXFE_WBw_WBDC_CG_CMD_UPDATE_ARM_SHFT                                    0x4
#define HWIO_RXFE_WBw_WBDC_CG_CMD_START_IMM_TRIG_BMSK                                0x8
#define HWIO_RXFE_WBw_WBDC_CG_CMD_START_IMM_TRIG_SHFT                                0x3
#define HWIO_RXFE_WBw_WBDC_CG_CMD_START_ARM_BMSK                                     0x4
#define HWIO_RXFE_WBw_WBDC_CG_CMD_START_ARM_SHFT                                     0x2
#define HWIO_RXFE_WBw_WBDC_CG_CMD_STOP_IMM_TRIG_BMSK                                 0x2
#define HWIO_RXFE_WBw_WBDC_CG_CMD_STOP_IMM_TRIG_SHFT                                 0x1
#define HWIO_RXFE_WBw_WBDC_CG_CMD_STOP_ARM_BMSK                                      0x1
#define HWIO_RXFE_WBw_WBDC_CG_CMD_STOP_ARM_SHFT                                        0

#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ADDR(w)                     (RXFE_REG_BASE      + 0x000040a8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_PHYS(w)                     (RXFE_REG_BASE_PHYS + 0x000040a8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_RMSK                        0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_MAXw                               0x2
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ACTION_SAMPLE_BMSK          0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_START_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                   0

#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ADDR(w)                      (RXFE_REG_BASE      + 0x000040ac + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_PHYS(w)                      (RXFE_REG_BASE_PHYS + 0x000040ac + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_RMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_MAXw                                0x2
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ACTION_SAMPLE_BMSK           0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_STOP_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                    0

#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ADDR(w)                    (RXFE_REG_BASE      + 0x000040b0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_PHYS(w)                    (RXFE_REG_BASE_PHYS + 0x000040b0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_RMSK                       0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_MAXw                              0x2
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ACTION_SAMPLE_BMSK         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                  0

#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_ADDR(w)                            (RXFE_REG_BASE      + 0x000040b4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_PHYS(w)                            (RXFE_REG_BASE_PHYS + 0x000040b4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_RMSK                                    0x3ff
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_MAXw                                      0x2
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_DELAY_VALUE_BMSK                        0x3ff
#define HWIO_RXFE_WBw_WBDC_CG_UPDATE_DELAY_DELAY_VALUE_SHFT                            0

#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_ADDR(w)                             (RXFE_REG_BASE      + 0x000040b8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040b8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_CURR_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_I_CURR_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x000040bc + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040bc + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_CURR_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_CURR_Q_CURR_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_ADDR(w)                             (RXFE_REG_BASE      + 0x000040c0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040c0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_LOAD_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_I_LOAD_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x000040c4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040c4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_LOAD_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST1_LOAD_Q_LOAD_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_ADDR(w)                             (RXFE_REG_BASE      + 0x000040c8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040c8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_CURR_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_I_CURR_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x000040cc + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040cc + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_CURR_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_CURR_Q_CURR_Q_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_ADDR(w)                             (RXFE_REG_BASE      + 0x000040d0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040d0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_LOAD_I_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_I_LOAD_I_SHFT                                  0

#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_ADDR(w)                             (RXFE_REG_BASE      + 0x000040d4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_PHYS(w)                             (RXFE_REG_BASE_PHYS + 0x000040d4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_RMSK                                0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_MAXw                                       0x2
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_LOAD_Q_BMSK                         0xffffffff
#define HWIO_RXFE_WBw_WBDC_CG_EST2_LOAD_Q_LOAD_Q_SHFT                                  0

#define HWIO_RXFE_WBw_VSRC_CFG_ADDR(w)                                        (RXFE_REG_BASE      + 0x000040d8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CFG_PHYS(w)                                        (RXFE_REG_BASE_PHYS + 0x000040d8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CFG_RMSK                                             0x3fffff
#define HWIO_RXFE_WBw_VSRC_CFG_MAXw                                                  0x2
#define HWIO_RXFE_WBw_VSRC_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_CFG_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSRC_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_VSRC_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSRC_CFG_BYPASS_BMSK                                      0x200000
#define HWIO_RXFE_WBw_VSRC_CFG_BYPASS_SHFT                                          0x15
#define HWIO_RXFE_WBw_VSRC_CFG_T1BYT2_BMSK                                      0x1fc000
#define HWIO_RXFE_WBw_VSRC_CFG_T1BYT2_SHFT                                           0xe
#define HWIO_RXFE_WBw_VSRC_CFG_TIMING_OFFSET_BMSK                                 0x3fff
#define HWIO_RXFE_WBw_VSRC_CFG_TIMING_OFFSET_SHFT                                      0

#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_ADDR(w)                                   (RXFE_REG_BASE      + 0x000040dc + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_PHYS(w)                                   (RXFE_REG_BASE_PHYS + 0x000040dc + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_RMSK                                      0xffffffff
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_MAXw                                             0x2
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_T2BYT1M1_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_T2BYT1M1_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_T2BYT1M1_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSRC_T2BYT1M1_ADDR(w),mask,val,HWIO_RXFE_WBw_VSRC_T2BYT1M1_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_T2BYT1M1_BMSK                             0xffffffff
#define HWIO_RXFE_WBw_VSRC_T2BYT1M1_T2BYT1M1_SHFT                                      0

#define HWIO_RXFE_WBw_VSRC_CMD_ADDR(w)                                        (RXFE_REG_BASE      + 0x000040e0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CMD_PHYS(w)                                        (RXFE_REG_BASE_PHYS + 0x000040e0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CMD_RMSK                                                  0xf
#define HWIO_RXFE_WBw_VSRC_CMD_MAXw                                                  0x2
#define HWIO_RXFE_WBw_VSRC_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_CMD_SCTR_LOAD_BMSK                                        0x8
#define HWIO_RXFE_WBw_VSRC_CMD_SCTR_LOAD_SHFT                                        0x3
#define HWIO_RXFE_WBw_VSRC_CMD_APPLY_TIMING_OFFSET_BMSK                              0x4
#define HWIO_RXFE_WBw_VSRC_CMD_APPLY_TIMING_OFFSET_SHFT                              0x2
#define HWIO_RXFE_WBw_VSRC_CMD_VSRC_UPDT_IMM_TRIG_BMSK                               0x2
#define HWIO_RXFE_WBw_VSRC_CMD_VSRC_UPDT_IMM_TRIG_SHFT                               0x1
#define HWIO_RXFE_WBw_VSRC_CMD_VSRC_UPDT_ARM_BMSK                                    0x1
#define HWIO_RXFE_WBw_VSRC_CMD_VSRC_UPDT_ARM_SHFT                                      0

#define HWIO_RXFE_WBw_VSRC_STATUS_ADDR(w)                                     (RXFE_REG_BASE      + 0x000040e4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_STATUS_PHYS(w)                                     (RXFE_REG_BASE_PHYS + 0x000040e4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_STATUS_RMSK                                               0x7
#define HWIO_RXFE_WBw_VSRC_STATUS_MAXw                                               0x2
#define HWIO_RXFE_WBw_VSRC_STATUS_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_STATUS_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_STATUS_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_STATUS_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_STATUS_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_STATUS_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDATED_BMSK                                  0x4
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDATED_SHFT                                  0x2
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDT_TRIGGERED_BMSK                           0x2
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDT_TRIGGERED_SHFT                           0x1
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDT_ARMED_BMSK                               0x1
#define HWIO_RXFE_WBw_VSRC_STATUS_VSRC_UPDT_ARMED_SHFT                                 0

#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ADDR(w)                         (RXFE_REG_BASE      + 0x000040e8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_PHYS(w)                         (RXFE_REG_BASE_PHYS + 0x000040e8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_RMSK                            0xffffffff
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_MAXw                                   0x2
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ACTION_SAMPLE_BMSK              0xffffffff
#define HWIO_RXFE_WBw_VSRC_UPDT_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                       0

#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_ADDR(w)                                (RXFE_REG_BASE      + 0x000040ec + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x000040ec + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_RMSK                                         0x7f
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_MAXw                                          0x2
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_T1BYT2_BMSK                                  0x7f
#define HWIO_RXFE_WBw_VSRC_CURR_T1BYT2_T1BYT2_SHFT                                     0

#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_ADDR(w)                              (RXFE_REG_BASE      + 0x000040f0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_PHYS(w)                              (RXFE_REG_BASE_PHYS + 0x000040f0 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_RMSK                                 0xffffffff
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_MAXw                                        0x2
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_T2BYT1M1_BMSK                        0xffffffff
#define HWIO_RXFE_WBw_VSRC_CURR_T2BYT1M1_T2BYT1M1_SHFT                                 0

#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_ADDR(w)                            (RXFE_REG_BASE      + 0x000040f4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_PHYS(w)                            (RXFE_REG_BASE_PHYS + 0x000040f4 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_RMSK                               0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_MAXw                                      0x2
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_ADDR(w),mask,val,HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_SCTR_BMSK                          0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_LOAD_VALUE_SCTR_SHFT                                   0

#define HWIO_RXFE_WBw_VSRC_SCTR_WA_ADDR(w)                                    (RXFE_REG_BASE      + 0x000040f8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_PHYS(w)                                    (RXFE_REG_BASE_PHYS + 0x000040f8 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_RMSK                                       0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_MAXw                                              0x2
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_SCTR_WA_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_SCTR_WA_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_SCTR_WA_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_VSRC_SCTR_WA_ADDR(w),mask,val,HWIO_RXFE_WBw_VSRC_SCTR_WA_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_WA_COUNT_BMSK                              0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_WA_WA_COUNT_SHFT                                       0

#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_ADDR(w)                              (RXFE_REG_BASE      + 0x000040fc + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_PHYS(w)                              (RXFE_REG_BASE_PHYS + 0x000040fc + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_RMSK                                 0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_MAXw                                        0x2
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_SCTR_SNAPSHOT_BMSK                   0xffffffff
#define HWIO_RXFE_WBw_VSRC_SCTR_SNAPSHOT_SCTR_SNAPSHOT_SHFT                            0

#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ADDR(w)                               (RXFE_REG_BASE      + 0x00004100 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_PHYS(w)                               (RXFE_REG_BASE_PHYS + 0x00004100 + 0x1000 * (w))
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_RMSK                                  0xffffffff
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_MAXw                                         0x2
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_INI(w) \
        in_dword(HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ADDR(w))
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ADDR(w), mask)
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ADDR(w),val)
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ACC_SNAPSHOT_BMSK                     0xffffffff
#define HWIO_RXFE_WBw_VSRC_ACC_SNAPSHOT_ACC_SNAPSHOT_SHFT                              0

#define HWIO_RXFE_WBw_WB_GDA_ADDR(w)                                          (RXFE_REG_BASE      + 0x00004104 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_GDA_PHYS(w)                                          (RXFE_REG_BASE_PHYS + 0x00004104 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WB_GDA_RMSK                                                    0xf
#define HWIO_RXFE_WBw_WB_GDA_MAXw                                                    0x2
#define HWIO_RXFE_WBw_WB_GDA_INI(w) \
        in_dword(HWIO_RXFE_WBw_WB_GDA_ADDR(w))
#define HWIO_RXFE_WBw_WB_GDA_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WB_GDA_ADDR(w), mask)
#define HWIO_RXFE_WBw_WB_GDA_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WB_GDA_ADDR(w),val)
#define HWIO_RXFE_WBw_WB_GDA_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WB_GDA_ADDR(w),mask,val,HWIO_RXFE_WBw_WB_GDA_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WB_GDA_EN4_BMSK                                                0x8
#define HWIO_RXFE_WBw_WB_GDA_EN4_SHFT                                                0x3
#define HWIO_RXFE_WBw_WB_GDA_EN3_BMSK                                                0x4
#define HWIO_RXFE_WBw_WB_GDA_EN3_SHFT                                                0x2
#define HWIO_RXFE_WBw_WB_GDA_EN2_BMSK                                                0x2
#define HWIO_RXFE_WBw_WB_GDA_EN2_SHFT                                                0x1
#define HWIO_RXFE_WBw_WB_GDA_EN1_BMSK                                                0x1
#define HWIO_RXFE_WBw_WB_GDA_EN1_SHFT                                                  0

#define HWIO_RXFE_WBw_DEC_FILT_CFG_ADDR(w)                                    (RXFE_REG_BASE      + 0x00004108 + 0x1000 * (w))
#define HWIO_RXFE_WBw_DEC_FILT_CFG_PHYS(w)                                    (RXFE_REG_BASE_PHYS + 0x00004108 + 0x1000 * (w))
#define HWIO_RXFE_WBw_DEC_FILT_CFG_RMSK                                             0x3f
#define HWIO_RXFE_WBw_DEC_FILT_CFG_MAXw                                              0x2
#define HWIO_RXFE_WBw_DEC_FILT_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_DEC_FILT_CFG_ADDR(w))
#define HWIO_RXFE_WBw_DEC_FILT_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_DEC_FILT_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_DEC_FILT_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_DEC_FILT_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_DEC_FILT_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_DEC_FILT_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_DEC_FILT_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC3_MODE_BMSK                                   0x30
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC3_MODE_SHFT                                    0x4
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC2_MODE_BMSK                                    0xc
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC2_MODE_SHFT                                    0x2
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC1_MODE_BMSK                                    0x3
#define HWIO_RXFE_WBw_DEC_FILT_CFG_DEC1_MODE_SHFT                                      0

#define HWIO_RXFE_WBw_IQMC_CFG0_ADDR(w)                                       (RXFE_REG_BASE      + 0x0000410c + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CFG0_PHYS(w)                                       (RXFE_REG_BASE_PHYS + 0x0000410c + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CFG0_RMSK                                               0xfff
#define HWIO_RXFE_WBw_IQMC_CFG0_MAXw                                                 0x2
#define HWIO_RXFE_WBw_IQMC_CFG0_INI(w) \
        in_dword(HWIO_RXFE_WBw_IQMC_CFG0_ADDR(w))
#define HWIO_RXFE_WBw_IQMC_CFG0_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_IQMC_CFG0_ADDR(w), mask)
#define HWIO_RXFE_WBw_IQMC_CFG0_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IQMC_CFG0_ADDR(w),val)
#define HWIO_RXFE_WBw_IQMC_CFG0_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_IQMC_CFG0_ADDR(w),mask,val,HWIO_RXFE_WBw_IQMC_CFG0_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_IQMC_CFG0_BYPASS_BMSK                                        0x800
#define HWIO_RXFE_WBw_IQMC_CFG0_BYPASS_SHFT                                          0xb
#define HWIO_RXFE_WBw_IQMC_CFG0_UPDATE_MODE_BMSK                                   0x400
#define HWIO_RXFE_WBw_IQMC_CFG0_UPDATE_MODE_SHFT                                     0xa
#define HWIO_RXFE_WBw_IQMC_CFG0_DELAY_VALUE_BMSK                                   0x3ff
#define HWIO_RXFE_WBw_IQMC_CFG0_DELAY_VALUE_SHFT                                       0

#define HWIO_RXFE_WBw_IQMC_CFG1_ADDR(w)                                       (RXFE_REG_BASE      + 0x00004110 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CFG1_PHYS(w)                                       (RXFE_REG_BASE_PHYS + 0x00004110 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CFG1_RMSK                                           0x3ffffff
#define HWIO_RXFE_WBw_IQMC_CFG1_MAXw                                                 0x2
#define HWIO_RXFE_WBw_IQMC_CFG1_INI(w) \
        in_dword(HWIO_RXFE_WBw_IQMC_CFG1_ADDR(w))
#define HWIO_RXFE_WBw_IQMC_CFG1_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_IQMC_CFG1_ADDR(w), mask)
#define HWIO_RXFE_WBw_IQMC_CFG1_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IQMC_CFG1_ADDR(w),val)
#define HWIO_RXFE_WBw_IQMC_CFG1_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_IQMC_CFG1_ADDR(w),mask,val,HWIO_RXFE_WBw_IQMC_CFG1_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_IQMC_CFG1_B_BMSK                                         0x3ffe000
#define HWIO_RXFE_WBw_IQMC_CFG1_B_SHFT                                               0xd
#define HWIO_RXFE_WBw_IQMC_CFG1_A_BMSK                                            0x1fff
#define HWIO_RXFE_WBw_IQMC_CFG1_A_SHFT                                                 0

#define HWIO_RXFE_WBw_IQMC_CURR_VALS_ADDR(w)                                  (RXFE_REG_BASE      + 0x00004114 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_PHYS(w)                                  (RXFE_REG_BASE_PHYS + 0x00004114 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_RMSK                                      0x3ffffff
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_MAXw                                            0x2
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_INI(w) \
        in_dword(HWIO_RXFE_WBw_IQMC_CURR_VALS_ADDR(w))
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_IQMC_CURR_VALS_ADDR(w), mask)
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IQMC_CURR_VALS_ADDR(w),val)
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_B_BMSK                                    0x3ffe000
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_B_SHFT                                          0xd
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_A_BMSK                                       0x1fff
#define HWIO_RXFE_WBw_IQMC_CURR_VALS_A_SHFT                                            0

#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_ADDR(w)                                   (RXFE_REG_BASE      + 0x00004118 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_PHYS(w)                                   (RXFE_REG_BASE_PHYS + 0x00004118 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_RMSK                                             0x3
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_MAXw                                             0x2
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IQMC_UPDT_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_UPDATE_IMM_TRIG_BMSK                             0x2
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_UPDATE_IMM_TRIG_SHFT                             0x1
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_UPDATE_ARM_BMSK                                  0x1
#define HWIO_RXFE_WBw_IQMC_UPDT_CMD_UPDATE_ARM_SHFT                                    0

#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ADDR(w)                         (RXFE_REG_BASE      + 0x0000411c + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_PHYS(w)                         (RXFE_REG_BASE_PHYS + 0x0000411c + 0x1000 * (w))
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_RMSK                            0xffffffff
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_MAXw                                   0x2
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_INI(w) \
        in_dword(HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ADDR(w))
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ADDR(w), mask)
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ADDR(w),val)
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ADDR(w),mask,val,HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ACTION_SAMPLE_BMSK              0xffffffff
#define HWIO_RXFE_WBw_IQMC_UPDT_ACTION_SAMPLE_ACTION_SAMPLE_SHFT                       0

#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_ADDR(w)                                (RXFE_REG_BASE      + 0x00004124 + 0x1000 * (w))
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x00004124 + 0x1000 * (w))
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_RMSK                                        0x3ff
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_MAXw                                          0x2
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_NOTCH_COMMON_CFG_ADDR(w))
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_NOTCH_COMMON_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_NOTCH_COMMON_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_NOTCH_COMMON_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_NOTCH_COMMON_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_AGC_VALUE_BMSK                              0x3ff
#define HWIO_RXFE_WBw_NOTCH_COMMON_CFG_AGC_VALUE_SHFT                                  0

#define HWIO_RXFE_WBw_NOTCHn_CFG0_ADDR(w,n)                                   (RXFE_REG_BASE      + 0x00004128 + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PHYS(w,n)                                   (RXFE_REG_BASE_PHYS + 0x00004128 + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG0_RMSK                                          0x7fffff
#define HWIO_RXFE_WBw_NOTCHn_CFG0_MAXw                                               0x2
#define HWIO_RXFE_WBw_NOTCHn_CFG0_MAXn                                               0x5
#define HWIO_RXFE_WBw_NOTCHn_CFG0_INI2(w,n) \
        in_dword(HWIO_RXFE_WBw_NOTCHn_CFG0_ADDR(w,n))
#define HWIO_RXFE_WBw_NOTCHn_CFG0_INMI2(w,n,mask) \
        in_dword_masked(HWIO_RXFE_WBw_NOTCHn_CFG0_ADDR(w,n), mask)
#define HWIO_RXFE_WBw_NOTCHn_CFG0_OUTI2(w,n,val) \
        out_dword(HWIO_RXFE_WBw_NOTCHn_CFG0_ADDR(w,n),val)
#define HWIO_RXFE_WBw_NOTCHn_CFG0_OUTMI2(w,n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_NOTCHn_CFG0_ADDR(w,n),mask,val,HWIO_RXFE_WBw_NOTCHn_CFG0_INI2(w,n));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_HYST_BMSK                                 0x700000
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_HYST_SHFT                                     0x14
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_REF_2_BMSK                                 0xffc00
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_REF_2_SHFT                                     0xa
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_REF_1_BMSK                                   0x3ff
#define HWIO_RXFE_WBw_NOTCHn_CFG0_PWR_REF_1_SHFT                                       0

#define HWIO_RXFE_WBw_NOTCHn_CFG1_ADDR(w,n)                                   (RXFE_REG_BASE      + 0x0000412c + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG1_PHYS(w,n)                                   (RXFE_REG_BASE_PHYS + 0x0000412c + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG1_RMSK                                        0x7fffffff
#define HWIO_RXFE_WBw_NOTCHn_CFG1_MAXw                                               0x2
#define HWIO_RXFE_WBw_NOTCHn_CFG1_MAXn                                               0x5
#define HWIO_RXFE_WBw_NOTCHn_CFG1_INI2(w,n) \
        in_dword(HWIO_RXFE_WBw_NOTCHn_CFG1_ADDR(w,n))
#define HWIO_RXFE_WBw_NOTCHn_CFG1_INMI2(w,n,mask) \
        in_dword_masked(HWIO_RXFE_WBw_NOTCHn_CFG1_ADDR(w,n), mask)
#define HWIO_RXFE_WBw_NOTCHn_CFG1_OUTI2(w,n,val) \
        out_dword(HWIO_RXFE_WBw_NOTCHn_CFG1_ADDR(w,n),val)
#define HWIO_RXFE_WBw_NOTCHn_CFG1_OUTMI2(w,n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_NOTCHn_CFG1_ADDR(w,n),mask,val,HWIO_RXFE_WBw_NOTCHn_CFG1_INI2(w,n));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_NOTCHn_CFG1_ENABLE_BMSK                                 0x40000000
#define HWIO_RXFE_WBw_NOTCHn_CFG1_ENABLE_SHFT                                       0x1e
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_2_BMSK                      0x3c000000
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_2_SHFT                            0x1a
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_1_BMSK                       0x3c00000
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_1_SHFT                            0x16
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_0_BMSK                        0x3c0000
#define HWIO_RXFE_WBw_NOTCHn_CFG1_LEFT_SHIFT_BITS_0_SHFT                            0x12
#define HWIO_RXFE_WBw_NOTCHn_CFG1_PHASE_INCR_REAL_BMSK                           0x3ffff
#define HWIO_RXFE_WBw_NOTCHn_CFG1_PHASE_INCR_REAL_SHFT                                 0

#define HWIO_RXFE_WBw_NOTCHn_CFG2_ADDR(w,n)                                   (RXFE_REG_BASE      + 0x00004130 + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG2_PHYS(w,n)                                   (RXFE_REG_BASE_PHYS + 0x00004130 + 0x1000 * (w)+0xC * (n))
#define HWIO_RXFE_WBw_NOTCHn_CFG2_RMSK                                           0x3ffff
#define HWIO_RXFE_WBw_NOTCHn_CFG2_MAXw                                               0x2
#define HWIO_RXFE_WBw_NOTCHn_CFG2_MAXn                                               0x5
#define HWIO_RXFE_WBw_NOTCHn_CFG2_INI2(w,n) \
        in_dword(HWIO_RXFE_WBw_NOTCHn_CFG2_ADDR(w,n))
#define HWIO_RXFE_WBw_NOTCHn_CFG2_INMI2(w,n,mask) \
        in_dword_masked(HWIO_RXFE_WBw_NOTCHn_CFG2_ADDR(w,n), mask)
#define HWIO_RXFE_WBw_NOTCHn_CFG2_OUTI2(w,n,val) \
        out_dword(HWIO_RXFE_WBw_NOTCHn_CFG2_ADDR(w,n),val)
#define HWIO_RXFE_WBw_NOTCHn_CFG2_OUTMI2(w,n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_NOTCHn_CFG2_ADDR(w,n),mask,val,HWIO_RXFE_WBw_NOTCHn_CFG2_INI2(w,n));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_NOTCHn_CFG2_PHASE_INCR_IMAG_BMSK                           0x3ffff
#define HWIO_RXFE_WBw_NOTCHn_CFG2_PHASE_INCR_IMAG_SHFT                                 0

#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_ADDR(w)                         (RXFE_REG_BASE      + 0x00004170 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_PHYS(w)                         (RXFE_REG_BASE_PHYS + 0x00004170 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_RMSK                                 0xfff
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_MAXw                                   0x2
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_INI(w) \
        in_dword(HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_ADDR(w))
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_ADDR(w), mask)
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_ADDR(w),mask,val,HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_UPDATE_DELAY_BMSK                    0xffc
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_UPDATE_DELAY_SHFT                      0x2
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_SBI_MODE_EN_BMSK                       0x2
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_SBI_MODE_EN_SHFT                       0x1
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_FEATURE_EN_BMSK                        0x1
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CFG_FEATURE_EN_SHFT                          0

#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_ADDR(w)                         (RXFE_REG_BASE      + 0x00004174 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_PHYS(w)                         (RXFE_REG_BASE_PHYS + 0x00004174 + 0x1000 * (w))
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_RMSK                                   0x1
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_MAXw                                   0x2
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_IMM_UPDATE_BMSK                        0x1
#define HWIO_RXFE_WBw_WBDC_FG_LSHIFT_UPDT_CMD_IMM_UPDATE_SHFT                          0

#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_ADDR(w)                                (RXFE_REG_BASE      + 0x00004178 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_PHYS(w)                                (RXFE_REG_BASE_PHYS + 0x00004178 + 0x1000 * (w))
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_RMSK                                          0x1
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_MAXw                                          0x2
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_ADDR(w),val)
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_IMM_SNAPSHOT_CMD_BMSK                         0x1
#define HWIO_RXFE_WBw_IMM_SNAPSHOT_CMD_IMM_SNAPSHOT_CMD_SHFT                           0

#define HWIO_RXFE_WBw_SPARE_REG_ADDR(w)                                       (RXFE_REG_BASE      + 0x0000417c + 0x1000 * (w))
#define HWIO_RXFE_WBw_SPARE_REG_PHYS(w)                                       (RXFE_REG_BASE_PHYS + 0x0000417c + 0x1000 * (w))
#define HWIO_RXFE_WBw_SPARE_REG_RMSK                                          0xffffffff
#define HWIO_RXFE_WBw_SPARE_REG_MAXw                                                 0x2
#define HWIO_RXFE_WBw_SPARE_REG_INI(w) \
        in_dword(HWIO_RXFE_WBw_SPARE_REG_ADDR(w))
#define HWIO_RXFE_WBw_SPARE_REG_INMI(w,mask) \
        in_dword_masked(HWIO_RXFE_WBw_SPARE_REG_ADDR(w), mask)
#define HWIO_RXFE_WBw_SPARE_REG_OUTI(w,val) \
        out_dword(HWIO_RXFE_WBw_SPARE_REG_ADDR(w),val)
#define HWIO_RXFE_WBw_SPARE_REG_OUTMI(w,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_RXFE_WBw_SPARE_REG_ADDR(w),mask,val,HWIO_RXFE_WBw_SPARE_REG_INI(w));\
        HWIO_INTFREE()
#define HWIO_RXFE_WBw_SPARE_REG_SPARE_BITS_31_0_BMSK                          0xffffffff
#define HWIO_RXFE_WBw_SPARE_REG_SPARE_BITS_31_0_SHFT                                   0
#endif

#endif