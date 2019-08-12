/*!
  @file
  lte_LL1_smem.h

  @brief
  Shared memory interface for LTE LL1.

  @detail
  Constants and structures for LTE LL1 FW-SW shared memory.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_smem.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_LL1_SMEM_H
#define LTE_LL1_SMEM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "modem_fw_memmap.h"
#include "intf_prs_sm.h"
#include "intf_prs_fd_logging_types.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

// XXX TODO: These should be removed, or defined in terms of FW SMEM header.

/* uart utility driver on ARM9 */
//#define Q6_SMEM_UART_PHYS_START_ADDR 0x03a30000

/* PLT server buffer address */
#define Q6_SMEM_PLT_PHYS_START_ADDR  FW_SMEM_LTE_PLT_ADDR
#define Q6_SMEM_LTE_START_ADDR  FW_SMEM_LTE_ADDR

/* Enum for VPE clock setting for NLIC modes */
typedef enum
{
  LTE_LL1_SMEM_NLIC_CLK_SETTING_SVS2 = 0,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_SVS,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_NOMINAL,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_TURBO,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_MAX,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_INVALID,
  LTE_LL1_SMEM_NLIC_CLK_SETTING_DEFAULT,
  _LTE_LL1_SMEM_NLIC_CLK_SETTING_FORCE_32_BIT = 0x7FFFFFFF
}lte_LL1_smem_nlic_clk_setting_e;

typedef struct
{
  /* PRS smem region */
  lte_LL1_prs_occasion_meas_s prs_smem;

  /* Other LTE smem region can be defined here */
  lte_LL1_smem_nlic_clk_setting_e nlic_clock_setting;
  
  /* PRS FD Logging */
  lte_LL1_prs_fd_logging_db_s prs_fd_log_db;
  
}lte_LL1_smem_t;

#endif /* LTE_LL1_SMEM_H */
