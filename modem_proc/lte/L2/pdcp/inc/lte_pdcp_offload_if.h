/*!
  @file
  lte_pdcp_offload_if.h

  @brief
  This file contains all External interfaces exported by PDCP OFFLOAD Layer.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/inc/lte_pdcp_offload_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
01/08/15   sb      CR775692: Reduce threshold of OFFLOAD-PDCP Wm
05/21/12   sb      CR363363: Compiler warning fixes
05/16/12   sb      CR361321: Added L2 indication optimization structure to reduce
                         frequency between PDCP to OFFLOAD notification.
===========================================================================*/

#ifndef LTE_PDCP_OFFLOAD_IF_H
#define LTE_PDCP_OFFLOAD_IF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_l2_common.h>
#include <pthread.h>
#include "lte_pdcp_ext_api.h"

/*! @brief OFFLOAD-PDCP UL WM LO: 
*/
#define LTE_OFFLOAD_PDCPUL_WM_LO     20000

/*! @brief OFFLOAD-PDCP UL WM HI: 
*/
#define LTE_OFFLOAD_PDCPUL_WM_HI     30000

/*! @brief OFFLOAD-PDCP UL WM DNE
*/
#define LTE_OFFLOAD_PDCPUL_WM_DNE    0x7FFFFFFF

/*! @brief OFFLOAD-PDCPUL WM DNE CNT
*/
#define LTE_OFFLOAD_PDCPUL_WM_DNE_COUNT  1000


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_task_init

===========================================================================*/
/*!
  @brief
  This function create PDCP OFFLOAD Task with the given priority.

  @detail
  
  @return
  the PDCPDL thread of type pthread_t.
*/
/*=========================================================================*/
extern pthread_t lte_pdcp_offload_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_get_pdcp_data_ind_info

===========================================================================*/
/*!
  @brief
  Get PDCP data ind info pointer
*/
/*=========================================================================*/
lte_l2_ind_info_s *lte_pdcp_offload_get_pdcp_data_ind_info(void);

#endif /* LTE_PDCP_DL_IF_H */
