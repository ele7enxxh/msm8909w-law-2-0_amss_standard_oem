/*!
  @file
  lte_pdcpoffload_pdcp.h

  @brief
  This file contains all external interfaces exposed by PDCP OFFLOAD for PDCP.

*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcpoffload_pdcp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/21/12   sb      CR363363: Compiler warning fixes
==============================================================================*/

#ifndef LTE_PDCPOFFLOAD_PDCP_H
#define LTE_PDCPOFFLOAD_PDCP_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <msgr.h>
#include <dsm_item.h>
#include <lte_dsm_util.h>
#include <lte_pdcp_ul_if.h>
#include "lte_pdcp_offload_protocol.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES


==============================================================================

                   EXTERNAL FUNCTION PROTOTYPES

==============================================================================

  FUNCTION:  lte_pdcp_offload_get_comp_cb_ptr

===========================================================================*/
/*!
  @brief
  This external function returns the compressor control block for an RB_ID.

  @detail

  @return
*/
/*=========================================================================*/

EXTERN lte_pdcp_offload_comp_cb_s* lte_pdcp_offload_get_comp_cb_ptr
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

/*==============================================================================

  FUNCTION:  lte_pdcp_offload_get_decomp_cb_ptr

===========================================================================*/
/*!
  @brief
  This external function returns the Decompressor control block for an RB_ID.

  @detail

  @return
*/
/*=========================================================================*/

EXTERN lte_pdcp_offload_decomp_cb_s* lte_pdcp_offload_get_decomp_cb_ptr
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN lte_pdcp_offload_decomp_eps_info_s* lte_pdcp_offload_get_decomp_eps_info_ptr
(
  lte_eps_id_t eps_id
);

EXTERN void lte_ps_to_pdcp_offload_wm_non_empty_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);

#endif /* LTE_RLC_PDCP_H */

