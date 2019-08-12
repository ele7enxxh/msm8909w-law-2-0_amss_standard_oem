/*!
  @file
  lte_rlcdl_nak.h

  @brief
  Definition file for RLC DL NAK related procedures and functions.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_nak.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/19/2008 ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_NAK_H
#define LTE_RLCDL_NAK_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <queue.h>
#include "lte_rlci.h"
#include "lte_rlcdl_hdr.h"
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*-------------------------------------------------------------------------*/
/*! @brief nak hole element definition
*/
typedef struct
{
  q_link_type          link;         /*!<  q link needed for queue operation */
  uint16               sn;           /*!<  sequence number */
  uint16               so_start;     /*!<  start offset of the hole */
  uint16               so_end;       /*!<  end offset of the hole   */
} lte_rlcdl_nak_element_s;

/*! @brief update the previous status of fast nak with current status 
*/
#define LTE_RLCDL_NAK_UPDATE_PREV_STATUS(ctrl_blk_ptr)                  \
  (ctrl_blk_ptr)->status.prev_nak_q_size =                              \
     lte_lockless_q_cnt(&(ctrl_blk_ptr->status.nak_q));                 \
  (ctrl_blk_ptr)->status.prev_vr_r = ctrl_blk_ptr->state_var.vr_r;      \
  (ctrl_blk_ptr)->status.num_new_holes = 0;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlcdl_nak_init( void );

extern void lte_rlcdl_nak_deinit(void);

extern boolean lte_rlcdl_nak_resource_freed( void );

extern lte_rlcdl_nak_element_s* lte_rlcdl_nak_get_elem( void );

extern void lte_rlcdl_nak_free_elem
( 
  lte_rlcdl_nak_element_s*     nak_elem_ptr
);

extern void lte_rlcdl_nak_append_pdu_to_chain
(
  uint16                       sn
);

extern void lte_rlcdl_nak_append_pdu_seg_to_chain
(
  uint16                       sn,
  uint16                       so_start,
  uint16                       so_end
);

extern void lte_rlcdl_nak_append_chain_to_nak_q
(
  lte_rlcdli_ctrl_blk_ptr      ctrl_blk_ptr,
  uint16                       new_vr_ms
);

extern void lte_rlcdl_nak_trim
(
  lte_rlcdli_ctrl_blk_ptr      ctrl_blk_ptr,
  const dsm_item_type*         dsm_ptr,
  uint16                       sn
);

extern uint32 lte_rlcdl_nak_build_status
(
  const lte_rlcdli_ctrl_blk_ptr      ctrl_blk_ptr,
  uint8*                             status_pdu_ptr,
  uint32                             allotment
);

extern void lte_rlcdl_nak_q_flush
(
  lte_rlcdli_ctrl_blk_ptr      ctrl_blk_ptr
);

extern uint16 lte_rlcdl_nak_get_temp_q_size
(
  void
);

extern  boolean lte_rlcdl_nak_suppress_status_report
(
  lte_rlcdli_ctrl_blk_ptr   ctrl_blk_ptr
);

static inline void lte_rlcdl_nak_proc_missing_segment
(
  const dsm_item_type*   dsm_ptr,  /*!< head dsm item ptr for the sequence */
  uint16           sn        /*!< sequence number */
)
{
  dsm_item_type*           next_dsm_ptr;
  uint16                   so_start, so_end;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/

  /* is first segments missing */
  so_start = LTE_RLCDL_HDR_SEGMENT_SO(dsm_ptr);
  if(so_start != LTE_RLCI_SO_MIN)
  {
    /* first segment missing */
    lte_rlcdl_nak_append_pdu_seg_to_chain(sn,
                                          LTE_RLCI_SO_MIN,
                                          (so_start -1));
  }
  for(next_dsm_ptr = (dsm_item_type*) dsm_ptr->link.next_ptr;
      dsm_ptr != NULL;
      dsm_ptr = next_dsm_ptr,
      next_dsm_ptr = (dsm_item_type*) dsm_ptr->link.next_ptr)
  {
    so_start = LTE_RLCDL_HDR_SEGMENT_SO(dsm_ptr) + 
      LTE_RLCDL_HDR_GET_DATA_LEN(dsm_ptr);

    if(next_dsm_ptr != NULL)
    {
      so_end = LTE_RLCDL_HDR_SEGMENT_SO(next_dsm_ptr);
      if(so_start == so_end)
      {
        /* no segment missing from dsm_ptr to next_dsm_ptr */
        continue;
      }
      else
      {
        lte_rlcdl_nak_append_pdu_seg_to_chain(sn,
                                              so_start,
                                              (so_end -1));
      }
    }
    else
    {
      /* this is the last item */
      if(!LTE_RLCDL_HDR_LSF(dsm_ptr))
      {
        lte_rlcdl_nak_append_pdu_seg_to_chain(sn,
                                              so_start,
                                              LTE_RLCI_SO_END_LAST_BYTE);
      }
      break;
    }
  }

} /* lte_rlcdl_nak_proc_missing_segment() */

static inline uint16 lte_rlcdl_nak_cache_missing_pdus
(
  const lte_rlcdli_ctrl_blk_s* ctrl_blk_ptr, /*!< ptr to control block */
  uint16                       start_sn,     /*!< start sequence number*/
  uint16                       end_sn        /*!< end sequence number*/
)
{
  uint32          sn;
  dsm_item_type * dsm_ptr;
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  for(sn = start_sn;
      LTE_RLCI_AM_SEQ_GT(end_sn, 
                         sn,
                         ctrl_blk_ptr->state_var.vr_r);
      sn = LTE_RLCI_AM_SEQ_INC(sn))
  {
    dsm_ptr = LTE_RLCDLI_GET_PDU_PTR(ctrl_blk_ptr,sn);
    if(dsm_ptr == NULL)
    {
      /* entire PDU is missing */
      lte_rlcdl_nak_append_pdu_to_chain(sn);
      continue;
    }
    else if(!LTE_RLCDL_HDR_FULLY_RXED(dsm_ptr))
    {
      lte_rlcdl_nak_proc_missing_segment(dsm_ptr,sn);
    }
  }
  return lte_rlcdl_nak_get_temp_q_size();
} /* lte_rlcdl_nak_cache_missing_pdus() */

#endif /* LTE_RLCDL_NAK_H */
