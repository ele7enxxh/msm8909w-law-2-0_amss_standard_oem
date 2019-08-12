#ifndef __RPM_IPC_H__
#define __RPM_IPC_H__
/*
===========================================================================
*/
/**
  @file rpm_ipc.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    RPM_DEC

  'Include' filters applied: RPM_IPC[RPM_DEC] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

  ===========================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/dal/framework/sys/src/rpm/rpm_ipc.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#define RPM_CSR_BASE                                                 0x00080000

/*----------------------------------------------------------------------------
 * MODULE: RPM_DEC
 *--------------------------------------------------------------------------*/

#define RPM_DEC_REG_BASE                                                 (RPM_CSR_BASE      + 0x00000000)

#define HWIO_RPM_IPC_ADDR                                                (RPM_DEC_REG_BASE      + 0x0000000c)
#define HWIO_RPM_IPC_RMSK                                                0xffffffff
#define HWIO_RPM_IPC_OUT(v)      \
        out_dword(HWIO_RPM_IPC_ADDR,v)
#define HWIO_RPM_IPC_RSRV_IPC_BMSK                                       0xf0000000
#define HWIO_RPM_IPC_RSRV_IPC_SHFT                                             0x1c
#define HWIO_RPM_IPC_APCS_TZ_IPC_BMSK                                     0xf000000
#define HWIO_RPM_IPC_APCS_TZ_IPC_SHFT                                          0x18
#define HWIO_RPM_IPC_SPARE_IPC_BMSK                                        0xf00000
#define HWIO_RPM_IPC_SPARE_IPC_SHFT                                            0x14
#define HWIO_RPM_IPC_WCN_IPC_BMSK                                           0xf0000
#define HWIO_RPM_IPC_WCN_IPC_SHFT                                              0x10
#define HWIO_RPM_IPC_MPSS_IPC_BMSK                                           0xf000
#define HWIO_RPM_IPC_MPSS_IPC_SHFT                                              0xc
#define HWIO_RPM_IPC_ADSP_IPC_BMSK                                            0xf00
#define HWIO_RPM_IPC_ADSP_IPC_SHFT                                              0x8
#define HWIO_RPM_IPC_APCS_HLOS_IPC_BMSK                                        0xf0
#define HWIO_RPM_IPC_APCS_HLOS_IPC_SHFT                                         0x4
#define HWIO_RPM_IPC_RPM_RSRV_BMSK                                              0xf
#define HWIO_RPM_IPC_RPM_RSRV_SHFT                                              0x0


#endif /* __RPM_IPC_H__ */

