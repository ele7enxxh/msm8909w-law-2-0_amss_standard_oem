/*!
  @file
  lte_pdcp_comp_if.h

  @brief
  This file contains all External interfaces exported by PDCP COMP Layer.

  @author
  sbathwal
*/

/*===========================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regdlated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODDLE

This section contains comments describing changes made to the moddle.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/inc/lte_pdcp_comp_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/15   sb      CR 803880: Initial checkin - PDCP Changes for UDC feature
===========================================================================*/
#ifdef FEATURE_LTE_UDC_ENABLED 

#ifndef LTE_PDCP_COMP_IF_H
#define LTE_PDCP_COMP_IF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <pthread.h>
#include "lte_pdcp_ext_api.h"

/*! @brief COMP-PDCP UL WM LO: 
*/
#define LTE_PDCPCOMP_PDCP_WM_LO     10000

/*! @brief COMP-PDCP UL WM HI: 
*/
#define LTE_PDCPCOMP_PDCP_WM_HI     15000

/*! @brief COMP-PDCP UL WM DNE
*/
#define LTE_PDCPCOMP_PDCP_WM_DNE    0xFFFFFFFF

/*! @brief COMP-PDCPUL WM DNE CNT
*/
#define LTE_PDCPCOMP_PDCP_WM_DNE_COUNT  0xFFFFFFFF

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcp_comp_task_init

===========================================================================*/
/*!
  @brief
  This function create PDCP COMP Task with the given priority.

  @detail
  
  @return
  the PDCPDL thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t lte_pdcp_comp_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_pdcp_comp_get_ps_pdcp_buffer_size

===========================================================================*/
/*!
  @brief
  This fucntion returns the size of PS-COMP watermark.

  @detail

  @return
  size of PS-COMP watermark. 
  0 if PS-COMP watermark is not registered or PDCP is deactivated. 
*/
/*=========================================================================*/
extern uint32 lte_pdcp_comp_get_ps_pdcpcomp_buffer_size
(
  lte_rb_cfg_idx_t  rb_cfg_idx  /*!< rb config index */
);

#endif /* LTE_PDCP_DL_IF_H */
#endif /*FEATURE_LTE_UDC_ENABLED */

