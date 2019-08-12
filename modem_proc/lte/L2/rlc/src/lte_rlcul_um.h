/*!
  @file
  lte_rlcul_um.h

  @brief
  Header file RLC UL Un-Acknowledged Mode related procedures.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_um.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/20/09   ar      Removed allotment_info_ptr and data_info_ptr arguments
12/05/08   ar      Changed the lte_rlcul_um_build_new_um_pdus() function name to
                   lte_rlcul_um_build_pdus()
11/17/08   ar      Added lte_rlcul_um_build_new_um_pdus() function declaration
10/12/08   ar      Added basic UM support
09/17/08   ar      initial version

==============================================================================*/

#ifndef LTE_RLCUL_UM_H
#define LTE_RLCUL_UM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>

#include "lte_rlcul_tx.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_um_build_pdus

==============================================================================*/
/*!
    @brief
    This function builds the new pdus from fragmented sdu (if any) and watermark
    sdus on a UM radio bearer

    @detail
    Once RLC SDU is dequeued from the WM it is possible that the granted
    allotment might allow only a portion or even nothing to be used from that
    SDU for pdu building. If any such fragmented sdu exists, pdu build starts
    with that fragmented sdu.

    Next this function dequeues an SDU from the  watermark and tries to use it
    for pdu building. In case the grant is  insufficient to build anything or
    sufficient just enough to build a portion  of the sdu, the remaining sdu is
    assigned to the fragmented sdu structure

    @return
    The function has a void return type. But the arguments allotment_info_ptr
    and data_info_ptr act as input/output arguments and are updated with the
    newly built pdu information

*/
/*============================================================================*/
EXTERN void lte_rlcul_um_build_pdus
(
  lte_rlculi_ctrl_blk_s*    const ctrl_blk_ptr,       /*!< Control block ptr */
  lte_rlcul_tx_curr_pdu_info_s*   curr_pdu_ptr        /*!< Ptr to currently
                                                           built pdu info */
);
/* lte_rlcul_um_build_pdus() */


/*==============================================================================

  FUNCTION:  lte_rlcul_um_update_txed_um_pdu_info

==============================================================================*/
/*!
    @brief
    Function for updating the transmitted pdu information structure with newly
    generated UM pdu.

    @detail
    The txed pdus are freed as per the RLCUL periodic timer
    expiry. The UM pdus are chained and an indication is sent to PDCPUL to free
    them.

    @return
    void

    @see related_function()
    lte_rlcul_am_update_txed_ctrl_pdu_info() - For updating the control pdu info
    lte_rlcul_tx_free_txed_pdu_info() - For freeing the UM + Control pdu info

*/
/*============================================================================*/
EXTERN void lte_rlcul_um_update_txed_um_pdu_info
(
  lte_rlcul_tx_pdu_payload_info_s* payload_info_ptr /*!< PDU payload info ptr */
);
 /* lte_rlcul_um_update_txed_um_pdu_info() */


#endif /* LTE_RLCUL_UM_H */
