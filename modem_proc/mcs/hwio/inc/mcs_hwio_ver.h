#ifndef MCS_HWIO_VER_H
#define MCS_HWIO_VER_H
/*===========================================================================

         M O D E M   H W I O   V E R S I O N   H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM Modem hardware for previous version of the associated
  chipset.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
===========================================================================*/


/*----------------------------------------------------------------------------------------------------------------------*/
/* msm8x26v1 defs */

#define HWIO_SYS_ONLINE_CTRLn_V1_ADDR(n)                               (UNIV_STMR_REG_BASE      + 0x00000018 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CTRLn_V1_PHYS(n)                               (UNIV_STMR_REG_BASE_PHYS + 0x00000018 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CTRLn_V1_RMSK                                        0xff
#define HWIO_SYS_ONLINE_CTRLn_V1_MAXn                                           1
#define HWIO_SYS_ONLINE_CTRLn_V1_INI(n)        \
        in_dword_masked(HWIO_SYS_ONLINE_CTRLn_V1_ADDR(n), HWIO_SYS_ONLINE_CTRLn_V1_RMSK)
#define HWIO_SYS_ONLINE_CTRLn_V1_INMI(n,mask)    \
        in_dword_masked(HWIO_SYS_ONLINE_CTRLn_V1_ADDR(n), mask)
#define HWIO_SYS_ONLINE_CTRLn_V1_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_CTRLn_V1_ADDR(n),val)
#define HWIO_SYS_ONLINE_CTRLn_V1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SYS_ONLINE_CTRLn_V1_ADDR(n),mask,val,HWIO_SYS_ONLINE_CTRLn_V1_INI(n))
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_G1_V1_BMSK                              0x80
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_G1_V1_SHFT                               0x7
#define HWIO_SYS_ONLINE_CTRLn_IRQ_EN_V1_BMSK                                 0x40
#define HWIO_SYS_ONLINE_CTRLn_IRQ_EN_V1_SHFT                                  0x6
#define HWIO_SYS_ONLINE_CTRLn_TDS_EN_V1_BMSK                                 0x20
#define HWIO_SYS_ONLINE_CTRLn_TDS_EN_V1_SHFT                                  0x5
#define HWIO_SYS_ONLINE_CTRLn_LTE_EN_V1_BMSK                                 0x10
#define HWIO_SYS_ONLINE_CTRLn_LTE_EN_V1_SHFT                                  0x4
#define HWIO_SYS_ONLINE_CTRLn_UMTS_EN_V1_BMSK                                 0x8
#define HWIO_SYS_ONLINE_CTRLn_UMTS_EN_V1_SHFT                                 0x3
#define HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_V1_BMSK                               0x4
#define HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_V1_SHFT                               0x2
#define HWIO_SYS_ONLINE_CTRLn_DO_EN_V1_BMSK                                   0x2
#define HWIO_SYS_ONLINE_CTRLn_DO_EN_V1_SHFT                                   0x1
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_V1_BMSK                                  0x1
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_V1_SHFT                                  0x0

#define HWIO_SYS_ONLINE_CMDn_V1_ADDR(n)                                (UNIV_STMR_REG_BASE      + 0x00000020 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CMDn_V1_PHYS(n)                                (UNIV_STMR_REG_BASE_PHYS + 0x00000020 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CMDn_V1_RMSK                                          0x1
#define HWIO_SYS_ONLINE_CMDn_V1_MAXn                                            1
#define HWIO_SYS_ONLINE_CMDn_V1_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_CMDn_V1_ADDR(n),val)
#define HWIO_SYS_ONLINE_CMDn_TRIG_V1_BMSK                                     0x1
#define HWIO_SYS_ONLINE_CMDn_TRIG_V1_SHFT                                     0x0

#define HWIO_SYS_ONLINE_TIMEn_V1_ADDR(n)                               (UNIV_STMR_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_SYS_ONLINE_TIMEn_V1_PHYS(n)                               (UNIV_STMR_REG_BASE_PHYS + 0x00000028 + 0x4 * (n))
#define HWIO_SYS_ONLINE_TIMEn_V1_RMSK                                   0x7ffffff
#define HWIO_SYS_ONLINE_TIMEn_V1_MAXn                                           1
#define HWIO_SYS_ONLINE_TIMEn_V1_INI(n)        \
        in_dword_masked(HWIO_SYS_ONLINE_TIMEn_V1_ADDR(n), HWIO_SYS_ONLINE_TIMEn_V1_RMSK)
#define HWIO_SYS_ONLINE_TIMEn_V1_INMI(n,mask)    \
        in_dword_masked(HWIO_SYS_ONLINE_TIMEn_V1_ADDR(n), mask)
#define HWIO_SYS_ONLINE_TIMEn_V1_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_TIMEn_V1_ADDR(n),val)
#define HWIO_SYS_ONLINE_TIMEn_V1_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SYS_ONLINE_TIMEn_V1_ADDR(n),mask,val,HWIO_SYS_ONLINE_TIMEn_V1_INI(n))
#define HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_V1_BMSK                       0x7ffffff
#define HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_V1_SHFT                             0x0

#define HWIO_SYS_ONLINE_STATUS_V1_ADDR                                 (UNIV_STMR_REG_BASE      + 0x00000030)
#define HWIO_SYS_ONLINE_STATUS_V1_PHYS                                 (UNIV_STMR_REG_BASE_PHYS + 0x00000030)
#define HWIO_SYS_ONLINE_STATUS_V1_RMSK                                        0x3
#define HWIO_SYS_ONLINE_STATUS_V1_IN          \
        in_dword_masked(HWIO_SYS_ONLINE_STATUS_V1_ADDR, HWIO_SYS_ONLINE_STATUS_V1_RMSK)
#define HWIO_SYS_ONLINE_STATUS_V1_INM(m)      \
        in_dword_masked(HWIO_SYS_ONLINE_STATUS_V1_ADDR, m)
#define HWIO_SYS_ONLINE_STATUS_ONLINE_DONE_V1_BMSK                            0x3
#define HWIO_SYS_ONLINE_STATUS_ONLINE_DONE_V1_SHFT                            0x0

/*----------------------------------------------------------------------------------------------------------------------*/
/* msm8974v2 defs */

#define HWIO_SYS_ONLINE_CTRLn_8974V2_ADDR(n)                               (UNIV_STMR_REG_BASE      + 0x00000018 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CTRLn_8974V2_PHYS(n)                               (UNIV_STMR_REG_BASE_PHYS + 0x00000018 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CTRLn_8974V2_RMSK                                        0x7f
#define HWIO_SYS_ONLINE_CTRLn_8974V2_MAXn                                           1
#define HWIO_SYS_ONLINE_CTRLn_8974V2_INI(n)        \
        in_dword_masked(HWIO_SYS_ONLINE_CTRLn_8974V2_ADDR(n), HWIO_SYS_ONLINE_CTRLn_8974V2_RMSK)
#define HWIO_SYS_ONLINE_CTRLn_8974V2_INMI(n,mask)    \
        in_dword_masked(HWIO_SYS_ONLINE_CTRLn_8974V2_ADDR(n), mask)
#define HWIO_SYS_ONLINE_CTRLn_8974V2_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_CTRLn_8974V2_ADDR(n),val)
#define HWIO_SYS_ONLINE_CTRLn_8974V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SYS_ONLINE_CTRLn_8974V2_ADDR(n),mask,val,HWIO_SYS_ONLINE_CTRLn_8974V2_INI(n))
#define HWIO_SYS_ONLINE_CTRLn_IRQ_EN_8974V2_BMSK                                 0x40
#define HWIO_SYS_ONLINE_CTRLn_IRQ_EN_8974V2_SHFT                                  0x6
#define HWIO_SYS_ONLINE_CTRLn_TDS_EN_8974V2_BMSK                                 0x20
#define HWIO_SYS_ONLINE_CTRLn_TDS_EN_8974V2_SHFT                                  0x5
#define HWIO_SYS_ONLINE_CTRLn_LTE_EN_8974V2_BMSK                                 0x10
#define HWIO_SYS_ONLINE_CTRLn_LTE_EN_8974V2_SHFT                                  0x4
#define HWIO_SYS_ONLINE_CTRLn_UMTS_EN_8974V2_BMSK                                 0x8
#define HWIO_SYS_ONLINE_CTRLn_UMTS_EN_8974V2_SHFT                                 0x3
#define HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_8974V2_BMSK                               0x4
#define HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_8974V2_SHFT                               0x2
#define HWIO_SYS_ONLINE_CTRLn_DO_EN_8974V2_BMSK                                   0x2
#define HWIO_SYS_ONLINE_CTRLn_DO_EN_8974V2_SHFT                                   0x1
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_8974V2_BMSK                                  0x1
#define HWIO_SYS_ONLINE_CTRLn_GSM_EN_8974V2_SHFT                                  0x0

#define HWIO_SYS_ONLINE_CMDn_8974V2_ADDR(n)                                (UNIV_STMR_REG_BASE      + 0x00000020 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CMDn_8974V2_PHYS(n)                                (UNIV_STMR_REG_BASE_PHYS + 0x00000020 + 0x4 * (n))
#define HWIO_SYS_ONLINE_CMDn_8974V2_RMSK                                          0x1
#define HWIO_SYS_ONLINE_CMDn_8974V2_MAXn                                            1
#define HWIO_SYS_ONLINE_CMDn_8974V2_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_CMDn_8974V2_ADDR(n),val)
#define HWIO_SYS_ONLINE_CMDn_TRIG_8974V2_BMSK                                     0x1
#define HWIO_SYS_ONLINE_CMDn_TRIG_8974V2_SHFT                                     0x0

#define HWIO_SYS_ONLINE_TIMEn_8974V2_ADDR(n)                               (UNIV_STMR_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_SYS_ONLINE_TIMEn_8974V2_PHYS(n)                               (UNIV_STMR_REG_BASE_PHYS + 0x00000028 + 0x4 * (n))
#define HWIO_SYS_ONLINE_TIMEn_8974V2_RMSK                                   0x7ffffff
#define HWIO_SYS_ONLINE_TIMEn_8974V2_MAXn                                           1
#define HWIO_SYS_ONLINE_TIMEn_8974V2_INI(n)        \
        in_dword_masked(HWIO_SYS_ONLINE_TIMEn_8974V2_ADDR(n), HWIO_SYS_ONLINE_TIMEn_8974V2_RMSK)
#define HWIO_SYS_ONLINE_TIMEn_8974V2_INMI(n,mask)    \
        in_dword_masked(HWIO_SYS_ONLINE_TIMEn_8974V2_ADDR(n), mask)
#define HWIO_SYS_ONLINE_TIMEn_8974V2_OUTI(n,val)    \
        out_dword(HWIO_SYS_ONLINE_TIMEn_8974V2_ADDR(n),val)
#define HWIO_SYS_ONLINE_TIMEn_8974V2_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SYS_ONLINE_TIMEn_8974V2_ADDR(n),mask,val,HWIO_SYS_ONLINE_TIMEn_8974V2_INI(n))
#define HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_8974V2_BMSK                       0x7ffffff
#define HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_8974V2_SHFT                             0x0

#define HWIO_SYS_ONLINE_STATUS_8974V2_ADDR                                 (UNIV_STMR_REG_BASE      + 0x00000030)
#define HWIO_SYS_ONLINE_STATUS_8974V2_PHYS                                 (UNIV_STMR_REG_BASE_PHYS + 0x00000030)
#define HWIO_SYS_ONLINE_STATUS_8974V2_RMSK                                        0x3
#define HWIO_SYS_ONLINE_STATUS_8974V2_IN          \
        in_dword_masked(HWIO_SYS_ONLINE_STATUS_8974V2_ADDR, HWIO_SYS_ONLINE_STATUS_8974V2_RMSK)
#define HWIO_SYS_ONLINE_STATUS_8974V2_INM(m)      \
        in_dword_masked(HWIO_SYS_ONLINE_STATUS_8974V2_ADDR, m)
#define HWIO_SYS_ONLINE_STATUS_ONLINE_DONE_8974V2_BMSK                            0x3
#define HWIO_SYS_ONLINE_STATUS_ONLINE_DONE_8974V2_SHFT                            0x0

#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR                              (MSS_PERPH_REG_BASE      + 0x00000080)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_PHYS                              (MSS_PERPH_REG_BASE_PHYS + 0x00000080)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_RMSK                                     0x1
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_IN          \
        in_dword_masked(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR, HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_RMSK)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_INM(m)      \
        in_dword_masked(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR, m)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_OUT(v)      \
        out_dword(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR,v)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ADDR,m,v,HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_IN)
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ENABLE_BMSK                              0x1
#define HWIO_MSS_DSM_PHASE_HIGHZ_ENABLE_ENABLE_SHFT                              0x0

#endif  /* MCS_HWIO_VER_H */
