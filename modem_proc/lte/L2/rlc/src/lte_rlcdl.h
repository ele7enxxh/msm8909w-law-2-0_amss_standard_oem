/*!
  @file
  lte_rlcdl.h

  @brief
  this header file defines external function APIs provided by RLCDL.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/09/2008 ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_H
#define LTE_RLCDL_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <pthread.h>
#include <msgr.h>
#include <lte_as.h>
#include <dsm_item.h>


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief LTE_RLCDL_CTRL_PDU_IND payload definition
*/
typedef struct
{
  msgr_hdr_struct_type  msg_hdr;      /*!< message header */
  lte_rb_cfg_idx_t      rb_cfg_idx;   /*!< RB configuration index */
  dsm_item_type*        ctrl_pdu_ptr; /*!< dsm item of the RLC UL status pdu
                                             for this rb_id */
} lte_rlcdl_ctrl_pdu_ind_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlcdl_init( void );

extern uint32 lte_rlcdl_get_ctrl_pdu_size
(
  lte_lc_id_t lc_id
);

extern uint16 lte_rlcdl_get_ctrl_pdu
(
  lte_lc_id_t          lc_id,
  uint16               allotment,
  uint8*               ctrl_pdu_mem_ptr
);

#endif /* LTE_RLCDL_H */
