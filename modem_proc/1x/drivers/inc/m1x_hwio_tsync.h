#ifndef M1X_HWIO_TSYNC_H
#define M1X_HWIO_TSYNC_H
/*===========================================================================

         T S Y N C    H A R D W A R E    I N P U T  /  O U T P U T

DESCRIPTION
  This file contains macros to support interaction with the QUALCOMM
  MSM and QSC Register Sets related to TSYNC HW block.

  Copyright (c) 2013 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/m1x_hwio_tsync.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/15   srk     Remove modem names.
08/14/13   cjb     Updated TSYNC registers for MDM
04/22/13   cjb     Same as MDM

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* CORE */
#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MPM2_TSYNC
 *--------------------------------------------------------------------------*/

#define MPM2_TSYNC_REG_BASE                                  (MPM2_MPM_BASE      + 0x00006000)

#define HWIO_MPM2_MPM_TSYNC_CTL_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_TSYNC_CTL_RMSK                              0x7ff
#define HWIO_MPM2_MPM_TSYNC_CTL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, HWIO_MPM2_MPM_TSYNC_CTL_RMSK)
#define HWIO_MPM2_MPM_TSYNC_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_CTL_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_CTL_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_CTL_IN)
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_BMSK                        0x7fc
#define HWIO_MPM2_MPM_TSYNC_CTL_WIDTH_SHFT                          0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_BMSK                            0x2
#define HWIO_MPM2_MPM_TSYNC_CTL_IRQ_SHFT                            0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_BMSK                         0x1
#define HWIO_MPM2_MPM_TSYNC_CTL_ENABLE_SHFT                         0x0

#define HWIO_MPM2_MPM_TSYNC_CMD_ADDR                         (MPM2_TSYNC_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_TSYNC_CMD_RMSK                                0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_CMD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_CMD_START_BMSK                          0x1
#define HWIO_MPM2_MPM_TSYNC_CMD_START_SHFT                          0x0

#define HWIO_MPM2_MPM_TSYNC_START_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_TSYNC_START_RMSK                        0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, HWIO_MPM2_MPM_TSYNC_START_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_IN)
#define HWIO_MPM2_MPM_TSYNC_START_TIME_BMSK                   0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_START_TIME_SHFT                         0x0

#define HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK                       0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_START_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_START_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_START_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_BMSK                 0xff
#define HWIO_MPM2_MPM_TSYNC_START_PHASE_PHASE_SHFT                  0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR                      (MPM2_TSYNC_REG_BASE      + 0x00000010)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK                       0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_BMSK                0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PERIOD_SHFT                      0x0

#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x00000014)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,v)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_ADDR,m,v,HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_IN)
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_PERIOD_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR                 (MPM2_TSYNC_REG_BASE      + 0x00000018)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK                  0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_TIME_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_TIME_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_BMSK             0x7ffffff
#define HWIO_MPM2_MPM_TSYNC_STATUS_TIME_TIME_SHFT                   0x0

#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR                (MPM2_TSYNC_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK                      0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_IN          \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_RMSK)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_ADDR, m)
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_BMSK                0xff
#define HWIO_MPM2_MPM_TSYNC_STATUS_PHASE_PHASE_SHFT                 0x0

#define HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR                       (MPM2_TSYNC_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK                              0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_IN          \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, HWIO_MPM2_MPM_GRFC_CXO_EN_RMSK)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR, m)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,v)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_GRFC_CXO_EN_ADDR,m,v,HWIO_MPM2_MPM_GRFC_CXO_EN_IN)
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_BMSK                      0x1
#define HWIO_MPM2_MPM_GRFC_CXO_EN_MUX_SEL_SHFT                      0x0

#endif /* M1X_HWIO_TSYNC_H */
