#ifndef __NOC_ERROR_HWIO_H__
#define __NOC_ERROR_HWIO_H__
/*
===========================================================================
*/
/**
  @file NOC_error_HWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    PC_NOC
    SYSTEM_NOC

  'Include' filters applied: REG_BASE[SYSTEM_NOC] REG_BASE[PC_NOC] 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
*/
/*
  ===========================================================================

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

  ===========================================================================

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/8909/NOC_error_HWIO.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_REG_BASE                                                                      (PC_NOC_BASE      + 0x00000000)

/*----------------------------------------------------------------------------
 * MODULE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_REG_BASE                                                                 (SYSTEM_NOC_BASE      + 0x00000000)

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DEFAULT_SWAY
 *--------------------------------------------------------------------------*/

#define BIMC_S_DEFAULT_SWAY_REG_BASE                                                   (BIMC_BASE      + 0x00058000)

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_STATUS_ADDR                                 (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000100)

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_CLEAR_ADDR                                  (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000108)

#define HWIO_BIMC_S_DEFAULT_SWAY_INTERRUPT_ENABLE_ADDR                                 (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x0000010c)

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_0_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000400)

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_1_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000410)

#define HWIO_BIMC_S_DEFAULT_SWAY_DEFAULT_SLAVE_STATUS_2_ADDR                           (BIMC_S_DEFAULT_SWAY_REG_BASE      + 0x00000420)

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL1
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL1_REG_BASE                                                   (BIMC_BASE      + 0x00001000)

#define HWIO_BIMC_BRIC_INTERRUPT_STATUS_0_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x00000100)

#define HWIO_BIMC_BRIC_INTERRUPT_CLEAR_0_ADDR                                   (BIMC_GLOBAL1_REG_BASE      + 0x00000108)

#define HWIO_BIMC_BRIC_INTERRUPT_ENABLE_0_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x0000010c)

#define HWIO_BIMC_BRIC_INTERRUPT_STATUS_1_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x00000110)

#define HWIO_BIMC_BRIC_INTERRUPT_CLEAR_1_ADDR                                   (BIMC_GLOBAL1_REG_BASE      + 0x00000118)

#define HWIO_BIMC_BRIC_INTERRUPT_ENABLE_1_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x0000011c)

#define HWIO_BIMC_BRIC_INTERRUPT_STATUS_2_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x00000120)

#define HWIO_BIMC_BRIC_INTERRUPT_CLEAR_2_ADDR                                   (BIMC_GLOBAL1_REG_BASE      + 0x00000128)

#define HWIO_BIMC_BRIC_INTERRUPT_ENABLE_2_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x0000012c)

#define HWIO_BIMC_BRIC_INTERRUPT_STATUS_3_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x00000130)

#define HWIO_BIMC_BRIC_INTERRUPT_CLEAR_3_ADDR                                   (BIMC_GLOBAL1_REG_BASE      + 0x00000138)

#define HWIO_BIMC_BRIC_INTERRUPT_ENABLE_3_ADDR                                  (BIMC_GLOBAL1_REG_BASE      + 0x0000013c)

/*----------------------------------------------------------------------------
 * MODULE: BIMC_S_DDR0_SCMO
 *--------------------------------------------------------------------------*/

#define BIMC_S_DDR0_SCMO_REG_BASE                                                    (BIMC_BASE      + 0x00048000)

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_STATUS_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000100)

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_CLEAR_ADDR                                   (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000108)

#define HWIO_BIMC_S_DDR0_SCMO_INTERRUPT_ENABLE_ADDR                                  (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000010c)

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_ADDR_ADDR                                         (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000120)

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_0_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000128)

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_1_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x0000012c)

#define HWIO_BIMC_S_DDR0_SCMO_ESYN_APACKET_2_ADDR                                    (BIMC_S_DDR0_SCMO_REG_BASE      + 0x00000130)

#endif /* __NOC_ERROR_HWIO_H__ */

