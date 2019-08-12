/*!
  @file
  lte_pdcp_comp_pdcp.h

  @brief
  This file contains all external interfaces exposed by PDCP COMP for PDCP.

  @author
  sbathwal

*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_comp_pdcp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/29/15   ar      include customer.h for UDC and ELS feature definition
05/15/15   sb      CR 803880: Initial check-in - PDCP Changes for UDC feature
==============================================================================*/

#include <customer.h>

#ifdef FEATURE_LTE_UDC_ENABLED 

#ifndef LTE_PDCPCOMP_PDCP_H
#define LTE_PDCPCOMP_PDCP_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <msgr.h>
#include <dsm_item.h>
#include <lte_dsm_util.h>
#include <lte_pdcp_ul_if.h>
#include "lte_pdcp_comp_protocol.h"
#include <custlte.h>
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES


==============================================================================

                   EXTERNAL FUNCTION PROTOTYPES

==============================================================================

  FUNCTION:  lte_pdcp_comp_get_comp_cb_ptr

===========================================================================*/
/*!
  @brief
  This external function returns the UDC compressor control block for an RB_ID.

  @detail

  @return
*/
/*=========================================================================*/

EXTERN void lte_ps_to_pdcp_comp_wm_non_empty_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);

#endif /* LTE_RLC_PDCP_H */
#endif /* FEATURE_LTE_UDC_ENABLED */


