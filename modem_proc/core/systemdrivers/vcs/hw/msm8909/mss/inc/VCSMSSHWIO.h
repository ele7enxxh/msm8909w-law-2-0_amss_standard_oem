#ifndef __VCSMSSHWIO_H__
#define __VCSMSSHWIO_H__
/*
===========================================================================
*/
/**
  @file VCSMSSHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MSS_MVC

  'Include' filters applied: MVC_MEM_SVS 
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

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/hw/msm8909/mss/inc/VCSMSSHWIO.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#include <HALhwio.h>

/*
 * HWIO base definitions
 */
extern uint32                      HAL_ldo_nHWIOBaseMSS;
#define MODEM_TOP_BASE             HAL_ldo_nHWIOBaseMSS



/*----------------------------------------------------------------------------
 * MODULE: MSS_MVC
 *--------------------------------------------------------------------------*/

#define MSS_MVC_REG_BASE                                          (MODEM_TOP_BASE      + 0x001b3000)
#define MSS_MVC_REG_BASE_OFFS                                     0x001b3000

#define HWIO_MVC_MEM_SVS_ADDR                                     (MSS_MVC_REG_BASE      + 0x0000001c)
#define HWIO_MVC_MEM_SVS_OFFS                                     (MSS_MVC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_MVC_MEM_SVS_RMSK                                            0x1
#define HWIO_MVC_MEM_SVS_IN          \
        in_dword_masked(HWIO_MVC_MEM_SVS_ADDR, HWIO_MVC_MEM_SVS_RMSK)
#define HWIO_MVC_MEM_SVS_INM(m)      \
        in_dword_masked(HWIO_MVC_MEM_SVS_ADDR, m)
#define HWIO_MVC_MEM_SVS_OUT(v)      \
        out_dword(HWIO_MVC_MEM_SVS_ADDR,v)
#define HWIO_MVC_MEM_SVS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MVC_MEM_SVS_ADDR,m,v,HWIO_MVC_MEM_SVS_IN)
#define HWIO_MVC_MEM_SVS_MEM_SVS_BMSK                                    0x1
#define HWIO_MVC_MEM_SVS_MEM_SVS_SHFT                                    0x0


#endif /* __VCSMSSHWIO_H__ */
