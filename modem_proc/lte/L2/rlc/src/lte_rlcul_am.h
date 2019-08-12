/*!
  @file
  lte_rlcul_am.h

  @brief
  Header file RLC UL Acknowledged Mode (AM) related procedures.

  @author
  araina

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_am.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/12/15   sb      CR776561: FR 25276: Fast LTE connection release
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
07/15/14   ru      CR694328: RLC UL Fast Retx Modem Exp Fix
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
06/07/11   ar      added lte_rlcul_am_reset_max_retx_ind_sent_flag() to send
                   just one IND per RRC config
08/06/09   ax      move configurable resource size variable to lte_rlc_cfg.h
03/26/09   ar      added pdu logging support
03/11/09   ar      Added lte_rlcul_am_process_buff_pdcp_ack_info() to cleanup
                   buffered PDCP acks during rb release/re-est
03/04/09   ar      CR 173722 fix - corrected lte_rlcul_am_get_retx_sdu_info()
                   for retx case for pdu with FI=1X
02/20/09   ar      Removed allotment_info_ptr and data_info_ptr arguments
02/12/09   ar      1. Changed macro LTE_RLCUL_AM_E2_BIT_SET to an inline func
                   2. Removed Klockwork errors
09/17/08   ar      initial version

==============================================================================*/

#ifndef LTE_RLCUL_AM_H
#define LTE_RLCUL_AM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <queue.h>
#include <lte_rlc_mac.h>

#include "lte_rlculi.h"
#include "lte_rlcdl.h"
#include "lte_rlcul_tx.h"
#include "lte_rlc_cfg.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Bitmask for Poll bit in the AM pdu header
*/
#define LTE_RLCUL_AM_POLL_BIT_MASK                0x20


/*! @brief Macro for minimum status pdu size.
 *  15 bits = D/C (1) + CPT (3) + ACK_SN(10) + E1 (1)
*/
#define LTE_RLCUL_AM_STATUS_PDU_MIN_SIZE          15


/*! @brief Macro for a NACK_SN, E1 and E2 set size (in bits).
 *  12 bits = NACK_SN (10) + E1 (1) + E2 (1) each NACK
*/
#define LTE_RLCUL_AM_STATUS_PDU_NACK_SIZE         12


/*! @brief Macro for a SO_start and SO_end set size(in bits)
 *  30 bits = SOstart (15) + SOend (15) each segmented NACK
*/
#define LTE_RLCUL_AM_STATUS_PDU_NACK_SEG_SIZE     30


/*! @brief Bitmask for Acknowldeged Sequence Number (ACK_SN)
*/
#define LTE_RLCUL_AM_ACK_SN_MASK                  0x0FFC


/*! @brief Bitmask for status pdu Extension 1 (E1) bit. The E1 field indicates
 *  whether or not a set of NACK_SN, E1 and E2 follows.
*/
#define LTE_RLCUL_AM_E1_BIT_MASK                  0x0002


/*! @brief Bitmask for status pdu Extension 2 (E2) bit. The E2 field indicates
 *  whether or not a set of SOstart and SOend follows.
*/
#define LTE_RLCUL_AM_E2_BIT_MASK                  0x0001


/*! @brief Bitmask for Segment Offset start (SO_start) field
*/
#define LTE_RLCUL_AM_SO_START_MASK                0x7FFF


/*! @brief Macro for Segment Offset start field size (in bits)
*/
#define LTE_RLCUL_AM_SO_END_SIZE                  15


/*! @brief Macro for maximum num of NACKS that can be stored in an AM
 *  retransmission queue. Currently, budgeting is done as 256 nacks per RB
 *  TODO: Revisit the retx q size budgeting in future
*/
#define LTE_RLCUL_AM_MAX_RETX_Q_CNT  LTE_MAX_ACTIVE_RB*256


/*! @brief Macro to get the ACK_SN from first 2bytes of RLC PDU header buffer
*/
#define LTE_RLCUL_AM_GET_ACK_SN(buf) \
  ( ((buf) & (LTE_RLCUL_AM_ACK_SN_MASK)) >> 2 )


/*! @brief Macro to get the NACK_SN from RLC PDU header buffer
*/
#define LTE_RLCUL_AM_GET_NACK_SN(buf) \
  ( ((buf) >> 2) )


/*! @brief Macro to get the Extension 1 (E1) bit value from first 2bytes of
 *  RLC PDU header buffer
*/
#define LTE_RLCUL_AM_GET_E1_BIT(buf) \
  ( ( (buf) & (LTE_RLCUL_AM_E1_BIT_MASK) ) >> 1 )


/*! @brief Macro to get the Segment Offset start (SO_start) field from the RLC
 * PDU header buffer
*/
#define LTE_RLCUL_AM_GET_SO_START(buf) \
  ( (buf) >> (LTE_RLCUL_AM_SO_END_SIZE) )


/*! @brief Macro to get the Segment Offset end (SO_end) field from the RLC
 * PDU header buffer
*/
#define LTE_RLCUL_AM_GET_SO_END(buf) \
  ( (buf) & (LTE_RLCUL_AM_SO_START_MASK) )


/*! @brief convert network byte order byte array into host byte order uint16
*/
#define LTE_RLCUL_AM_HDR_NTOHS(byte0, byte1) \
  ( ((byte0) << 8) | (byte1) )


/*! @brief Retransmission queue (RE_EX_Q) element structure.The RE_TX_Q is a
 * shared resource of MACUL and RLCUL task. MACUL needs to work on the RE_TX_Q
 * during the PDU building operation. RLCUL works on the RE_TX_Q on reception of
 * a status pdu. Due to possible contention, RE_TX_Q uses a mutex lock for any
 * kind of operation.
*/
typedef struct
{
  q_link_type link;       /*!< Link to the next item in the queue. NOTE: Current
                               UMTS targets have the size of this link field of
                               4bytes. In case the size increases in future, the
                               q implementation will have to be re-considered
                               for memory efficiency */
  uint16      seq_num;    /*!< Sequence num of the PDU that needs to be retx*/
  uint16      seg_start;  /*!< Starting offset of the segmented RLC PDU. In case
                               of complete RLC PDU retx, seg_start=0 */
  uint16      seg_length; /*!< Length of the segmented RLC PDU considered for
                               retx. In case of complete RLC PDU retx,
                               seg_length = total_pdcp_payload */
} lte_rlcul_am_retx_q_elem_s;


/*! @brief Structure to hold info about a NACK received in the status pdu
*/
typedef struct
{
  uint16 nack_sn;  /*!< NACK_SN field indicates the SN of the AMD PDU (or
                        portions of it) that has been detected as lost at the
                        receiving side of the AM RLC entity */
  uint16 so_start; /*!< so_start (together with the so_end field) indicates
                        the portion of the AMD PDU with SN = NACK_SN that has
                        been detected as lost at the receiving side */
  uint16 so_end;   /*!< So_end (together with the so_start field) indicates
                        the portion of the AMD PDU with SN = NACK_SN that has
                        been detected as lost at the receiving side */
} lte_rlcul_am_nack_info_s;


/*! @brief Structure to hold the info about the sdu belonging to an AMD PDU that
 * needs to be retransmitted
*/
typedef struct
{
  dsm_item_type* sdu_dsm_ptr;   /*!< sdu dsm item ptr */
  uint16         sdu_offset;    /*!< starting offset */
  uint16         sdu_size;      /*!< sdu size in bytes */
  uint16         retx_seg_size; /*!< sdu segment size that needs to be retx-ed*/
} lte_rlcul_am_retx_sdu_s;


/*! @brief Structure to hold the info that is cracked out from the received
 * status pdu
*/
typedef struct
{
  uint16 ack_sn;    /*!< ACK_SN field indicates the first SN of AMD PDU not
                         completely received and not reported in STATUS PDU */
  uint16 num_nacks; /*!< Num of nacked pdus/pdu segments */
  /*! Nacked pdu/pdu segment info. +1 is required to satisfy Klockwork */
  lte_rlcul_am_nack_info_s nack_info[LTE_RLC_CFG_UL_MAX_NACKS_PER_STATUS_PDU+1];
} lte_rlcul_am_status_pdu_info_s;


/*! @brief Internal variables of lte_rlcul_am.c
*/
typedef struct
{
  lte_rlcul_am_status_pdu_info_s status;       /*!< Status pdu contents info */
  lte_rlcul_max_retx_ind_s       max_retx_ind; /*!< LTE_RLCUL_MAX_RETX_IND msg*/
  q_type                         retx_elem_q;  /*!< Q for holding the retx
                                                    q elements TODO: rename it
                                                    to free retx resource q */

  /*! Retx q element array. Each element contains info about pdu that needs to
   * be re_txed - needed for the AM retx q */
  lte_rlcul_am_retx_q_elem_s  retx_q_elem_array[LTE_RLCUL_AM_MAX_RETX_Q_CNT];

  /*! Memory chunk reserved for RLCDL to write the control pdu */
  uint8                  status_pdu_buff[LTE_RLC_CFG_UL_AM_STATUS_PDU_MAX_SIZE];

  lte_rlcul_am_retx_q_elem_s  retx_elem; /*!< Retx_q elem that is to be retx */
  /*! boolean to indicate whether to send MAX_RETX_IND to RRC or not. This is 
    needed to avoid cases where a lot of rlc pdus hit maxRetxThreshold together
    and end up sending multiple indications to RRC */
  boolean                     max_retx_ind_sent;

  /*! Boolean to indicate if HARQ ACK_REQ need to be sent to ML1 */
  boolean                     cphy_harq_ack_req;
  /*! HARQ ID on which last SRB1 ACK was built */
  uint8                       harq_id;
} lte_rlcul_am_s;

/*
* @brief Parameter struct for  LTE_RLCUL_STOP_STAT_PROHIBIT_IND. If there was
* status PDU in last HARQ during RLC fast retransmission, then it will have 
* to be rebuilt. This indication will ensure DL to rebuilt the control
* PDU.
*/
typedef struct
{
  msgr_hdr_struct_type           hdr;   /*!< Message header */
  uint8                          lc_id; /*!< Logical channel ID */
}lte_rlcul_am_stop_stat_prohibit_ind_s;

/*==============================================================================

  FUNCTION:  LTE_RLCUL_AM_E2_BIT_SET

==============================================================================*/
/*!
  @brief
  Checks if Extension 2 (E2) bit in RLC PDU header buffer is set

  @return
  TRUE if E2 bit is set
*/
/*============================================================================*/
static inline boolean LTE_RLCUL_AM_E2_BIT_SET
(
  uint32 buf  /*!< buf containing unpacked RLC header */
)
{

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

return ((buf & LTE_RLCUL_AM_E2_BIT_MASK) == 1);

} /* LTE_RLCUL_AM_E2_BIT_SET() */


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_am_reset_max_retx_ind_sent_flag

==============================================================================*/
/*!
    @brief
    This function sets the max_retx_ind_sent flag to FALSE

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_reset_max_retx_ind_sent_flag
(
  void
);
/* lte_rlcul_am_reset_max_retx_ind_sent_flag() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_init

==============================================================================*/
/*!
    @brief
    This function initializes all the data members belonging to lte_rlcul_am.c
    file.

    @detail
    Following variables are initialized:
    - Free RETX_Q element q
    - LTE_RLCUL_ACK_IND message header
    - LTE_RLCUL_MAX_RETX_IND message header

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_init
(
  void
);
/* lte_rlcul_am_init() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_get_retx_elem_q_ptr

==============================================================================*/
/*!
    @brief
    Function returns a handle to the retransmission element q

    @detail
    Retransmission element q holds the available NACK elements that can be used
    for enqueueing a NACK into the retx queue. This function returns the pointer
    to that free retx element q

    @return
    Retx element queue pointer of type lte_circ_q_s*

*/
/*============================================================================*/
EXTERN q_type* lte_rlcul_am_get_retx_elem_q_ptr
(
  void
);
/* lte_rlcul_am_get_retx_elem_q_ptr() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_build_ctrl_pdu

==============================================================================*/
/*!
    @brief
    This function handles the processing related to building AM RLC DL control
    pdu.

    @detail
    This function calls the RLCDL functional API lte_rlcdl_get_ctrl_pdu() with
    all the allotment (minus 1byte for mac sub header). If RLCDL builds a
    control pdu, it checks if the size is greater than the fill task size, then
    it pushes down the control pdu to a dsm item and write the dsm gather size.
    On the contrary, if the status pdu size is less than fill task size, it
    writes the fill task for the same.
    NOTE: DSM_DS_SMALL_ITEM_POOL is used for allocating a dsm item, if required

    @return
    void

    @see related_function()
    lte_rlcdl_get_ctrl_pdu()

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_build_ctrl_pdu
(
  lte_rlculi_ctrl_blk_s*    const ctrl_blk_ptr,       /*!< control block ptr */
  lte_mac_allotment_info_s* const allotment_info_ptr, /*!< Ptr to mac allot
                                                           info struct */
  lte_sfn_s                       sfn                 /*!< system frame number*/
);
/* lte_rlcul_am_build_ctrl_pdu() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_build_retx_pdus

==============================================================================*/
/*!
    @brief
    Function for building the retransmission AMD PDUS/PDU segments

    @detail
    This function uses a lock (retx_q_lock) over the AM RB retransmission queue

    @return
    void

    @see related_function()
    lte_rlcul_um_build_pdus()
    lte_rlcul_am_build_new_pdus()

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_build_retx_pdus
(
  lte_rlculi_ctrl_blk_s*    const ctrl_blk_ptr,       /*!< Control block ptr */
  lte_mac_allotment_info_s* const allotment_info_ptr, /*!< Ptr to mac allotment
                                                           info struct */
  lte_mac_ul_data_info_s*   const data_info_ptr,      /*!< Ptr to mac ul data
                                                           info struct */
  lte_rlcul_tx_curr_pdu_info_s*   curr_pdu_ptr        /*!< Ptr to curr retx pdu
                                                           info */
); /* lte_rlcul_am_build_retx_pdus() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_build_new_pdus

==============================================================================*/
/*!
    @brief
    This function builds the new pdus from fragmented sdu (if any) and watermark
    sdus on an AM radio bearer

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
EXTERN void lte_rlcul_am_build_new_pdus
(
  lte_rlculi_ctrl_blk_s*    const ctrl_blk_ptr,       /*!< Control block ptr */
  lte_mac_allotment_info_s* const allotment_info_ptr, /*!< Ptr to mac allotment
                                                           info struct */
  lte_rlcul_tx_curr_pdu_info_s*   curr_pdu_ptr        /*!< Ptr to currently
                                                           built pdu info */
);
/* lte_rlcul_am_build_new_pdus() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_update_txed_ctrl_pdu_info

==============================================================================*/
/*!
    @brief
    Function for updating the transmitted pdu information structure with newly
    generated RLC DL control pdu.

    @detail
    The txed pdus are freed as per the RLCUL periodic timer expiry. RLC control
    pdus are freed locally at RLC UL.

    @return
    void

    @see related_function()
    lte_rlcul_um_update_txed_um_pdu_info() - For updating the UM pdu info
    lte_rlcul_tx_free_txed_pdu_info() - For freeing the UM + Control pdu info

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_update_txed_ctrl_pdu_info
(
  dsm_item_type* ctrl_pdu_ptr, /*!< Control pdu dsm pointer */
  uint16 ctrl_pdu_size         /*!< Control pdu size*/
);
/* lte_rlcul_am_update_txed_ctrl_pdu_info */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_process_status_pdu

==============================================================================*/
/*!
    @brief
    This function processes the ctrl pdu received from RLC DL

    @detail
    Currently the only valid form of control pdu is the status pdu carrying the
    transmission status of an AM RB.

    @return
    void

    @note
    The dsm item of the status pdu is freed in this function

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_process_ctrl_pdu
(
  lte_rb_id_t    rb_id,       /*!< Radio bearer type */
  dsm_item_type* ctrl_pdu_ptr /*!< Status pdu dsm item ptr */
);
/* lte_rlcul_am_process_status_pdu() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_update_t_poll_retx_counter

==============================================================================*/
/*!
    @brief
    This function is called upon RLCUL periodic timer expiry and it updates
    the poll retransmit counter and handles its expiry

    @detail
    Following actions are carried out in this function:
    - Start/restart the t_poll_retx timer if start_t_poll_retx flag is set.
    - If t_poll_retx is already running, decrement the t_poll_retx_counter by
    LTE_RLCULI_TIMER_PERIODICITY. If timer expires, check if UL WM is empty or
    retx queue is empty or transmission window has stalled. If yes, move the
    pdu associated with VT(s)-1 to retx queue and set the poll trigger to TRUE

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_update_t_poll_retx_counter
(
  lte_rlculi_ctrl_blk_s* ctrl_blk_ptr /*!< Control block pointer */
);
/* lte_rlcul_am_update_t_poll_retx_counter() */


/*==============================================================================

  FUNCTION:  lte_rlcul_am_process_buff_pdcp_ack_info

==============================================================================*/
/*!
    @brief
    This function processes the buffered ACK info for a re-established /
    released RB

    @detail
    If the buffered ack info for released/re-established RB is present,
    in the (curr_count) or (curr_count-1) row, relevant ack indication is sent
    to PDCPUL.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_process_buff_pdcp_ack_info
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr /*!< Control block ptr */
);
/* lte_rlcul_am_process_buff_pdcp_ack_info() */

/*==============================================================================

  FUNCTION:  lte_rlcul_am_free_pdus_rb_release

==============================================================================*/
/*!
    @brief
    This function frees all unacked PDUs on RB release

    @return
    void

*/
/*============================================================================*/
extern void lte_rlcul_am_free_pdus_rb_release
(
  lte_rlculi_ctrl_blk_s*        ctrl_blk_ptr     /*!< Control block pointer */
);

/*==============================================================================

  FUNCTION:  lte_rlcul_am_get_unacked_sdu

==============================================================================*/
/*!
    @brief
    This function gets the unacked RLC PDU from RLC Tx buffer based on VT(A) and VT(S)

    @detail

    @return
    void

*/
/*============================================================================*/

EXTERN void lte_rlcul_am_get_unacked_sdu
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  dsm_item_type **pkt_ptr
);

/*==============================================================================

  FUNCTION:  lte_rlcul_am_fast_enqueue_retx_pdus

==============================================================================*/
/*!
    @brief
    This function is called for queueing one RLC PDUs per LC_ID that was present
    inside the HARQ which did MAX retransmission attempt.

    @detail
    The function queues the RLC PDU inside the ret_x queue. For queueing, it 
    needs to parse SN, SEG_OFFSET and LSF flag from the respective RLC header
    buffer memory offset. A safe function to queue the PDU is
    lte_rlcul_am_process_nack() which also eliminates duplicates, if any. 
    Care should be taken while initializing so_start and so_end for the nack
    info. There are 3 scenarios: full PDU, first half - second half PDU
    and first half - part - second half PDU.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_fast_enqueue_retx_pdus
(
  uint8 harq_id
);

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_am_reset_cphy_harq_ack_flag

==============================================================================*/
/*!
    @brief
    This function sets the cphy_harq_ack_req flag to FALSE

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_am_reset_cphy_harq_ack_req_flag
(
  void
);

#endif /* LTE_RLCUL_AM_H */
