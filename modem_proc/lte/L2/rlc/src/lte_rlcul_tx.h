/*!
  @file
  lte_rlcul_tx.h

  @brief
  Header file for RLCUL transmission related functions.

*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_tx.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/11/15   sb      CR761765: Double RLC memory for header building 
                   for each HARQ
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
07/15/14   ru      CR694328: RLC UL Fast Retx Modem Exp Fix
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features 
06/03/13   sb	   CR466136: Build RLC PDU with 4 bytes grant
07/09/12   ar      CR 374751 fix: increased LTE_RLCUL_TX_MAX_PDCP_PDUS_PER_TTI 
                   from 30 to 125 
03/08/12   sb      CR 341977 fix:Changed the macro LTE_RLCUL_TX_MIN_DATA_PDU_SIZE
                   from 6 to 5
06/10/11   ar      CR 290997 fix: use 5bytes as minimum grant check for PDCP
                   control pdus
05/23/11   ar      increased LTE_RLCUL_TX_TXED_PDU_ARRAY_MAX_ROWS to 16 to
                   buffer txed data for 16*5=80ms
04/29/11   ar      added lte_cfg.h for FEATURE_LTE_NIKEL_REDUCED_HARQS
04/19/11   ar      renamed FEATURE_NIKEL_REDUCED_HARQS to
                   FEATURE_LTE_NIKEL_REDUCED_HARQS
04/14/11   ar      added lte_rlcul_tx_max_pdcp_pdus_limit_reached() to check if
                   max pdcp pdus per tti limit has reached
02/02/11   ar      Fixed mac hdr computation logic for 130 byte allot case
07/10/10   ar      CR 245957 fix: use extra_allot for mac_header_size calc
                   only if UL buffer size is zero
07/08/10   ar      added LTE_RLCUL_TX_MIN_DATA_PDU_SIZE and
                   LTE_RLCUL_TX_MIN_CTRL_PDU_SIZE macros
05/27/10   ar      CR 238483 fix: fixed the stall/crash case where last byte of
                   an sdu needs to be transmitted and pdcp hdr len is 2 bytes
05/26/10   ar      CR 238483 fix: include new sdu if allotment is more than
                   pdcp hdr len
02/16/10   ar      removing dsm_lock.h include as its not needed
01/19/10   ar      fixed extra allotment consumption logic
11/06/09   bn      Moved LTE_RLCUL_TX_HDR_BUFF_SIZE to lte_l2_common.h
04/07/09   ar      Added qxdm support for control, signaling and polling logging
03/26/09   ar      added pdu logging support
03/11/09   ar      Separated lte_rlcul_tx_calc_mac_sub_hdr_size() into
                   sufficient and insufficient allotment functions
02/20/09   ar      Moved allotment_info_ptr and data_info_ptr to top level
                   file structure lte_rlcul_tx_s
01/19/09   ax      change RLC hdr buffer from static to dynamic allocated memory
05/12/08   ar      Added function lte_rlcul_tx_get_new_sdu_info() to get the
                   info of the new SDU to be built
11/17/08   ar      1. Added init function lte_rlcul_tx_init() declaration
                   2. Added macros LTE_RLCUL_TX_SDU_SIZE and
                   LTE_RLCUL_TX_UL_WM_SIZE
10/30/08   ar      Addressed code review comments
10/12/08   ar      Added basic UM support
09/17/08   ar      initial version

==============================================================================*/

#ifndef LTE_RLCUL_TX_H
#define LTE_RLCUL_TX_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <a2_ul_phy.h>
#include <lte_mac_rlc.h>
#include <lte_lockless_q.h>
#include <lte_pdcp_ul_if.h>
#include <lte_as.h>
#include <lte_l2_common.h>

#include "lte_rlculi.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Maximum value that the 11 bits length indicator field can take.
 * */
#define LTE_RLCUL_TX_LI_MAX_VALUE                 2047


/*! @brief Maximum number of entries in the txed_ul_pdu_info array. This number
 *  determines how much buffered data can the txed_ul_pdu_info hold
*/
#define LTE_RLCUL_TX_TXED_PDU_ARRAY_MAX_ROWS     16


/*! @brief Maximum size of the PDCP SDU header. This macro is used during packet
 * building for avoiding transmission of PDCP SDU header and SDU payload in two
 * different TTIs
*/
#define LTE_RLCUL_TX_PDCP_HDR_MAX_SIZE            2


/*! @brief Maximum size (in bytes) used for building a single RLC pdu header.
*/
#define LTE_RLCUL_TX_PDU_HDR_MAX_SIZE             1024


/*! @brief Mask for Modulo 2 computation */
#define LTE_RLCUL_TX_MOD_2_MASK                   0x01


/*! @brief Mask for modulo 4 computation */
#define LTE_RLCUL_TX_MOD_4_MASK                   0x03


/*! @brief Mask for modulo 16 computation */
#define LTE_RLCUL_TX_MOD_16_MASK                  0x0F


/*! @brief Half modulus mask for AM
*/
#define LTE_RLCI_AM_HALF_MOD_MASK                 0x1FF


/*! @brief Macro for setting the initial value of the RETX_COUNT
*/
#define LTE_RLCUL_TX_RETX_COUNT_INVALID           0xFF


/*! @brief Macro for setting the initial value for RB_CFG_IDX
*/
#define LTE_RLCUL_TX_RB_CFG_IDX_INVALID           0xFF


/*! @brief minimum rlc control pdu size in bytes.
(1byte of mac sub hdr + 2bytes of RLC control pdu) */
#define LTE_RLCUL_TX_MIN_CTRL_PDU_SIZE            3


/*! @brief maximum number of pdcp pdus that RLC will build per TTI
  RLC header size = 125 x 1.5(for LIs) ~= 190bytes
  For 8 harqs = 190*8 = 1840bytes
  CRM RLC header buffer size = 512x8 = 1520bytes

  A2 UL PHY taskq memory / harq:
  125 * (4 (dma fill for pdcp hdr) + 2(cipher task) + 3 (dma task for sdu)) + 
  20 words (misc tasks) ~= 1250 words
  CRM A2 UL PHY taskq size/harq = A2_UL_PHY_TASK_Q_SIZE = 1750words
*/
#define LTE_RLCUL_TX_MAX_PDCP_PDUS_PER_TTI        125

/*! @brief Maximum number of PDUs contained in a HARQ. This macro gives
 * the maximum number of RLC PDU header offsets that are stored per HARQ. 
*/
#define LTE_RLCUL_TX_MAX_PDUS_PER_HARQ            16
#define LTE_RLCUL_TX_MAX_HDR_OFFSET_BITS          10

/*! @brief Maximum number of Ping pong buffer 
*/
#define LTE_RLCUL_TX_MAX_PING_PONG_BUF_SIZE       2

/*! @brief MACRO to get the meta info pointer of a given dsm item
*/
#define LTE_RLCUL_TX_META_INFO_PTR(sdu_dsm_ptr)\
  (lte_pdcpul_meta_info_s*)((sdu_dsm_ptr)->app_ptr)


/*! @brief MACRO to get the size of a dequeued WM SDU.
 * NOTE: The PDCP sdu size (pdcp header + payload) is stored in the app field
 * of the first item of the  pdcp wm sdu dsm item chain
*/
#define LTE_RLCUL_TX_SDU_SIZE(sdu_dsm_ptr)\
 (dsm_length_packet(sdu_dsm_ptr))


/*! @brief MACRO to get the transmission window index for a given sequence
 * number and modulus. Ex: for SN=600 and mod 1024, the tx window index should
 * be 58 as the tx_win is half the total AM sequence space.
*/
#define LTE_RLCUL_TX_GET_TX_WIN_INDEX(sn)\
  ( (sn) & (LTE_RLCI_AM_HALF_MOD_MASK) )


/*! @brief Macro to get the size(in bytes) of pdus sitting in the retransmission
 * queue
*/
#define LTE_RLCUL_TX_RETX_Q_SIZE(ctrl_blk_ptr)\
  ( ctrl_blk_ptr->state.retx_info.retx_q_size )


/*! @brief Current header buffer size per HARQ 
*/
#define LTE_RLCUL_TX_CURR_MAX_HDR_BUF  \
  (LTE_RLCUL_TX_HDR_BUFF_SIZE / LTE_MAC_UL_DATA_NUM_SW_HARQ)
  
/*! @brief Check whether maximum header offset bits can represent all of 
 * the RLC header memory. This macro is supposed to catch error condition 
 * if the size of header memory cannot be addressed 
 * by LTE_RLCUL_TX_MAX_HDR_OFFSET_BITS
 */
#if (LTE_RLCUL_TX_CURR_MAX_HDR_BUF > LTE_RLCUL_TX_PDU_HDR_MAX_SIZE)
#error "Header buffer per HARQ should be less LTE_RLCUL_TX_MAX_HDR_BUF"
#endif

/*! @brief Structure for holding all the information related to RLC header
 * building
*/
typedef struct
{
  uint8*  buf_ptr;                /*!< dynamically allocated RLC header buffer:
                                       pointer to contiguous memory chunk such
                                       that is accessible by A2*/
  uint16  hdr_buff_offset;        /*!< Offset in the header buffer at which the
                                     current pdu header is stored */
  uint16  hdr_len;                /*!< Length of the RLC header in bytes. The
                                     hdr_len is updated while building a RLC
                                     pdu. On completion, this value is indicated
                                     to A2 to fill the RLC header bytes into the
                                     ERAM. */
  uint16  prev_sdu_li;            /*!< Length indicator for previous sdu */
  uint8   num_li;                 /*!< Number of LIs present in the pdu header*/
  uint8*  a2_res_gather_task_ptr; /*!< Address of the reserved A2 gather task
                                       for writing the RLC header */
  uint16  seq_num;                /*!< Sequence number of the pdu currently
                                       being built */
  uint8   mac_hdr_size;           /*!< Size of the mac header for this pdu */
  uint16  seg_offset;             /*!< Segment Offset if any, in case this pdu
                                       is a retx amd pdu segment */
} lte_rlcul_tx_pdu_hdr_info_s;


/*! @brief This structure maintains payload information of the current pdu being
 * built
*/
typedef struct
{
  dsm_item_type* pdu_chn_h_ptr;      /*!< Pointer to the head of PDU chain */
  dsm_item_type* pdu_chn_t_ptr;      /*!< Pointer to the tail of PDU chain */
  uint16         rlc_payload_size;   /*!< Total rlc payload in this pdu */
  uint16         first_sdu_offset;   /*!< offset of the first sdu in pdu. NOTE:
                                          Offset includes PDCP header size */
  dsm_item_type* last_complete_sdu_ptr; /*!< Ptr to the last complete sdu in
                                             the pdu chain, used during freeing
                                             of txed UM PDUs */
  uint8          num_sdus;           /*!< Num sdus belonging to this pdu */
  boolean        sdu_ends_in_pdu;    /*!< Boolean to check if the current sdu
                                          ends inside the pdu */
} lte_rlcul_tx_pdu_payload_info_s;


/*! @brief Structure for holding the dsm head and tail chain pointer. It is used
 * in the txed ul pdu info structure
 */
typedef struct
{
  dsm_item_type* chain_h_ptr; /*!< Pointer to the head of PDU chain */
  dsm_item_type* chain_t_ptr; /*!< Pointer to the tail of PDU chain */
} lte_rlcul_tx_pdu_chain_s;


/*! @brief Structure to hold the PDU ack related information for a particular
 * Radio Bearer
*/
typedef struct
{
  lte_rb_cfg_idx_t      rb_cfg_idx;      /*!< unique RB configuration index */
  uint32                first_ack_count; /*!< PDCP COUNT of the first acked
                                              sdu */
  uint32                last_ack_count;  /*!< PDCP COUNT of the last acked
                                              sdu */
} lte_rlcul_tx_pdcp_ack_info_s;


/*! @brief Structure for holding the transmitted UM PDU and RLC control PDU
 * information
*/
typedef struct
{
  /*! UM pdus related information */
  lte_rlcul_tx_pdu_chain_s um_pdu[LTE_RLCUL_TX_TXED_PDU_ARRAY_MAX_ROWS];

  /*! RLCDL control PDU related info */
  lte_rlcul_tx_pdu_chain_s ctrl_pdu[LTE_RLCUL_TX_TXED_PDU_ARRAY_MAX_ROWS];

  /*! index associated with current x to (x+5)ms time frame. It is used
   * for inserting txed items into the txed ul pdu info struct and for removing
   * stored pdus from the array */
  uint32                   curr_index;

  /*! Number of bytes held in each row. Set to zero when freed.*/
  uint32                   num_bytes[LTE_RLCUL_TX_TXED_PDU_ARRAY_MAX_ROWS];
} lte_rlcul_tx_txed_pdu_info_s;


/*! @brief Main structure for holding the header and payload information of a
 * pdu currently under pdu build
*/
typedef struct
{
  lte_rlcul_tx_pdu_hdr_info_s     hdr_info;     /*!< Pdu header related info */
  lte_rlcul_tx_pdu_payload_info_s payload_info; /*!< Pdu payload related info */
  lte_sfn_s                       sfn;          /*!< system frame number */
  uint16                          pdu_log_mask; /*!< pdu log mask of txed pdu */
} lte_rlcul_tx_curr_pdu_info_s;

/*! @brief Structure for holding all the information required to locate the
 * RLC header in the header buffer while queueing the PDUs for fast 
 * retransmission.
 */
typedef struct
{
  /*!< RLC PDU Sequence Number */
  uint32 seq_num : 16;
  /*!< Logical channel ID */
  uint32 lc_id          : 8;
  /*!< Unused bit field*/
  uint32 unused  : 8;
} lte_rlcul_tx_harq_pdu_info_s;

/*! @brief Structure for holding header offset information per HARQ for 
 * RLC fast retransmission.
 */
typedef struct 
{
  /*!< Array to hold RLC info */
  lte_rlcul_tx_harq_pdu_info_s pdu_info[LTE_RLCUL_TX_MAX_PDUS_PER_HARQ];
  /*!< Number of RLC PDUs inside the current HARQ */
  uint8                        num_pdus;
} lte_rlcul_tx_harq_info_s;

typedef struct 
{
  /*!< Array to hold RLC info */
  lte_rlcul_tx_harq_info_s harq_info[LTE_RLCUL_TX_MAX_PING_PONG_BUF_SIZE];
  
  /*!< Current index of ping pong buffer */
  uint8                        curr_index;
  /*!< Number of max retx indications sent from MAC/ML1 */
  uint32                       num_max_retx_inds_rcvd;
} lte_rlcul_tx_harq_2_pdu_map_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_init

==============================================================================*/
/*!
    @brief
    This function initializes all the data members belonging to lte_rlcul_tx.c
    file.

    @detail
    Following variables are initialized:
    - Transmitted pdu info array
    - LTE_RLCUL_FREE_PDU_IND message header

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_init
(
  void
);
/* lte_rlcul_tx_init() */

extern void lte_rlcul_tx_deinit( void );

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_assign_allotment

==============================================================================*/
/*!
    @brief
    Function for assigning a given mac allotment to an SDU.

    @detail
    This function considers the RLC header size and the mac sub header size in
    assigned allotment size calculation.

    @return
    Allotment size in bytes that should be used for building this sdu

*/
/*============================================================================*/
EXTERN uint16 lte_rlcul_tx_assign_allotment
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr, /*!< Control block ptr */
  uint16                       sdu_size,     /*!< sdu size */
  uint16                       init_hdr_len, /*!< initial header length */
  uint32                       ul_buff_size  /*!< remaining UL buffer status,
                                                  needed for extra allotment
                                                  consumption logic */
);
/* lte_rlcul_tx_assign_allotment() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_get_new_sdu_info

==============================================================================*/
/*!
    @brief
    This function returns the PDCP SDU information that needs to be considered
    for pdu building

    @detail
    This function looks into the fragmented sdu structure and the RLC-PDCP UL
    watermark and returns the sdu related information. The passed in argument
    "sdu_ptr" is populated with the sdu info.

    @return
    TRUE - If a fragmented SDU or UL WM SDU is available for building
    FALSE - If fragmented sdu structure is NULL and if watermark is empty or
            inactive
            If remaining allotment is less than min pdu size threshold
*/
/*============================================================================*/
EXTERN boolean lte_rlcul_tx_get_new_sdu_info
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr, /*!< Control block ptr */
  lte_rlculi_frag_sdu_s*       sdu_ptr       /*!< Ptr to the empty sdu struct */
);
/* lte_rlcul_tx_get_new_sdu_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_init_curr_pdu_info

==============================================================================*/
/*!
    @brief
    Function to initialize the current pdu header and payload information

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_init_curr_pdu_info
(
  void
);
/* lte_rlcul_tx_init_curr_pdu_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_calc_mac_hdr_size_suff_allot

==============================================================================*/
/*!
    @brief
    Function for calculating the mac sub header size for given sdu size

    @detail
    If the sdu fits in the given allotment: calculate mac sub header size
    as per the sdu size

    @return
    Returns the mac sub header size in bytes

*/
/*============================================================================*/
EXTERN uint8 lte_rlcul_tx_calc_mac_hdr_size_suff_allot
(
  uint16 sdu_size,    /*!< sdu size */
  uint32 ul_buff_size /*!< UL buffer size */
);
/* lte_rlcul_tx_calc_mac_hdr_size_suff_allot() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_calc_mac_hdr_size_insuff_allot

==============================================================================*/
/*!
    @brief
    Function for calculating the mac sub header size for the given allotment
    size

    @detail
    if the sdu needs to be chopped as per the given allotment - calculate mac
    sub header size as per the available allotment

    @return
    Returns the mac sub header size in bytes

*/
/*============================================================================*/
EXTERN uint8 lte_rlcul_tx_calc_mac_hdr_size_insuff_allot
(
  uint16  allot_size,        /*!< allot size for current RLC PDU */
  uint16* used_sdu_allot_ptr /*!< ptr to allot size for current RLC SDU */
);
/* lte_rlcul_tx_calc_mac_hdr_size_insuff_allot() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_calc_bytes_needed_to_add_li

==============================================================================*/
/*!
    @brief
    Function to compute the number of bytes required to add a new LI to the pdu
    header

    @return
    returns the number of bytes required to add a new LI

*/
/*============================================================================*/
EXTERN uint8 lte_rlcul_tx_calc_bytes_needed_to_add_li
(
  uint8 num_lis  /*!< Num of existing LIs in the header */
);
/* lte_rlcul_tx_calc_bytes_needed_to_add_li() */


/*==============================================================================

  FUNCTION:  lte_rlcul_update_pdu_payload_info

==============================================================================*/
/*!
    @brief
    Function updates the payload information of the current pdu

    @detail
    It updates the following fields in payload structure:
    pdu_chn_h_ptr - Pointer to the head of PDU chain
    pdu_chn_t_ptr - Pointer to the tail of PDU chain
    total_pdcp_payload;
    first_sdu_offset;
    num_sdus;
    sdu_ends_in_pdu;
    last_complete_sdu_ptr;

    @return
    void

    @see related_function()
    lte_rlcul_um_build_new_um_pdus()
    lte_rlcul_am_build_new_am_pdus()

*/
/*============================================================================*/
EXTERN void lte_rlcul_update_pdu_payload_info
(
  lte_rlculi_ctrl_blk_s* const     ctrl_blk_ptr,     /*!< Control block ptr */
  dsm_item_type**                  sdu_dsm_ptr,      /*!< ptr to sdu dsm */
  uint16                           total_sdu_size,   /*!< PDCP SDU size */
  uint16                           used_sdu_size     /*!< used sdu size */
);
/* lte_rlcul_update_pdu_payload_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_add_li

==============================================================================*/
/*!
    @brief
    Function to add an LI to the pdu header

    @detail
    Fixed portion of the pdu header is built at the end of building a single
    pdu. Since LIs are added on the fly, some room is left in the hdr buffer for
    writing the fixed header later. In case of UM and new AMD PDUS a 2 byte
    offset is used for writing the LIs. In case of retx pdus (both AMD PDU and
    AMD PDU segments), 4byte offset is used. If there is no Segment Offset to
    fill, the fixed header is written from 2byte offset.

    @return
    void

    @see related_function()
    lte_rlcul_um_build_pdu_fixed_hdr() - Function to build the um pdu fixed
    header portion
    lte_rlcul_am_build_retx_pdu_fixed_hdr() - Function to build the retx pdu
    fixed header portion ( both AMD PDU header or AMD PDU segment header )
    lte_rlcul_am_build_new_pdu_fixed_hdr() - Function to build the new AMD pdu
    fixed header portion
*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_add_li
(
  void
);
/* lte_rlcul_tx_add_li() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_update_tx_win

==============================================================================*/
/*!
    @brief
    Function to update the RLC UL transmission window

    @detail

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_update_tx_win
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr /*!< Control block ptr */
);
/* lte_rlcul_tx_update_tx_win() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_write_a2_tasks

==============================================================================*/
/*!
    @brief
    Function to write the appropriate a2 tasks for pdcp header and payload

    @detail

    @return
    void

    @see related_function()
    lte_rlcul_tx_write_pdu_hdr_a2_task() - A2 driver task for writing the
    reserved A2 driver task for RLC header

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_write_a2_tasks
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr, /*!< Control block ptr */
  dsm_item_type*               sdu_dsm_ptr,  /*!< sdu dsm item ptr */
  uint16                       sdu_offset,   /*!< sdu offset */
  uint16                       used_sdu_size /*!< sdu size used in pdu build */
);
/* lte_rlcul_tx_write_a2_tasks() */

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_update_mac_data_info

==============================================================================*/
/*!
    @brief
    Function for updating the new pdu information to the mac data info structure

    @detail

    @return
    void

    @return value

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_update_mac_data_info
(
  uint16 hdr_size,     /*!< RLC pdu header size */
  uint16 payload_size, /*!< PDCP payload size in case of data pdu else RLC ctrl
                            pdu size*/
  uint8  mac_hdr_size  /*!< Mac sub header size */
);
/* lte_rlcul_tx_update_mac_data_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_get_txed_pdu_info

==============================================================================*/
/*!
    @brief
    Function to get the handle to transmitted UL pdu information array.

    @detail
    The txed_ul_pdu_info array stores the information about transmitted UM PDUs,
    transmitted RLC control pdus and AM ack information. It is looked at after
    every RLC UL periodic timer expiry and all the stale PDUs are purged.

    @return
    Pointer of type lte_rlcul_tx_txed_pdu_info_s*

*/
/*============================================================================*/
EXTERN lte_rlcul_tx_txed_pdu_info_s* lte_rlcul_tx_get_txed_pdu_info
(
  void
);
/* lte_rlcul_tx_get_txed_pdu_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_free_txed_pdu_info

==============================================================================*/
/*!
    @brief
    Function for freeing the transmitted UM and Control pdu information

    @detail
    The txed pdus are freed as per the RLCUL periodic timer expiry. The UM pdus
    are chained and an indication is sent to PDCPUL to free them. RLC control
    pdus are free locally at RLCUL. If any acks have been received for a
    particular RB, then the LTE_RLCUL_ACK_IND is sent to PDCP for that RB.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_free_txed_pdu_info
(
  void
);
/* lte_rlcul_tx_free_txed_pdu_info() */


/*==============================================================================

  FUNCTION:  lte_rlcul_tx_send_free_pdu_ind

==============================================================================*/
/*!
    @brief
    This function sends the free pdu indication to PDCP UL.

    @detail
    RLC UL sends the free pdu indication to PDCP UL with the payload as dsm
    items of transmitted UM pdus. PDCP UL is expected to free the dsm items.

    @return
    Status indicating whether the indication was sent successfully or
    not

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_send_free_pdu_ind
(
  dsm_item_type* free_dsm_chn_ptr  /*!< Chain of WM SDUs that needs to be
                                        freed by PDCP */
);
/* lte_rlcul_tx_send_free_pdu_ind() */

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_max_pdcp_pdus_limit_reached

==============================================================================*/
/*!
    @brief
    Function to check if maximum number of pdcp pdus are built for this TTI, if
    yes return true

    @detail
    RLC will stop building any more pdus once the max limit is reached. It will
    set the mac_hdr_size to 1 byte which will be an indication for MAC to bail
    out from packet build

    @return
    TRUE: if max limit is reached
    FALSE: otherwise

*/
/*============================================================================*/
EXTERN boolean lte_rlcul_tx_max_pdcp_pdus_limit_reached
(
  void
);

EXTERN void lte_rlc_pdcp_unack_sdu
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  dsm_item_type **pkt_ptr
);

/* lte_rlcul_tx_max_pdcp_pdus_limit_reached() */

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_max_harq_retx_ind_handler

==============================================================================*/
/*!
    @brief
    This function queues all the RLC PDU's for fast retransmission. 

    @detail
    This handler is called by RLCUL indicating that MAX HARQ retransmission 
    attempt is reached. Once inside the handler, it queues all the RLC PDUs
    that were present inside the HARQ which had MAX retransmission attempt
    set to true.

    @return
    void

*/
/*============================================================================*/

EXTERN void lte_rlcul_tx_max_harq_retx_ind_handler
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  lte_rlcul_tx_update_harq_2_pdu_fields

==============================================================================*/
/*!
    @brief
    This function updates all the entries of harq_2_pdu_map data structure.

    @detail
    The fields inside harq_2_pdu_map data structure is required during queueing 
    of PDUs in fast retransmission. 

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlcul_tx_update_harq_2_pdu_fields
(
  uint8 harq_id,
  uint16 seq_num,
  uint8 lcid
);

/*==============================================================================

  FUNCTION:  lte_rlc_tx_get_scratch_harq_info

==============================================================================*/
/*!
    @brief
    This function returns the scratch buffer pointer for update

    @detail
    The enqueue operation should always happen on curr_index. For update, this
    function should be called which returns scratch buffer pointer.

    @return
    Pointer to scratch buffer pointer

*/
/*============================================================================*/
EXTERN lte_rlcul_tx_harq_info_s* lte_rlc_tx_get_scratch_harq_info
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  lte_rlc_tx_get_curr_harq_info

==============================================================================*/
/*!
    @brief
    This function returns the current buffer pointer for retx enqueue.

    @detail
    The update operation should always happen on scratch buffer. For enqueue, 
    this function should be called which returns current buffer pointer.

    @return
    Pointer to current buffer pointer

*/
/*============================================================================*/
EXTERN lte_rlcul_tx_harq_info_s* lte_rlc_tx_get_curr_harq_info
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  lte_rlc_init_harq_2_pdu_fields

==============================================================================*/
/*!
    @brief
    This function inits all the entries of harq_2_pdu_map data structure.

    @detail
    Initializes num_pdus of both the ping pong buffer and curr_index of that
    HARQ ID.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlc_init_harq_2_pdu_fields
( 
  void
);

/*==============================================================================

  FUNCTION:  lte_rlc_reset_scratch_harq_info

==============================================================================*/
/*!
    @brief
    This function resets num_pdu entry of scratch harq info.

    @detail
    This function is called before a packet build. This will reset the num_pdus
    field of scratch harq to 0.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlc_reset_scratch_harq_info
(
  uint8 harq_id
);

/*==============================================================================

  FUNCTION:  lte_rlc_switch_curr_index

==============================================================================*/
/*!
    @brief
    This function switches the ping pong buffer index.

    @detail
    After the harq_2_pdu_map data structure for particular harq_id is updated
    for one of the ping pong buffer, this function is called to switch the 
    index and make this buffer available for retx enqueue operation.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlc_switch_curr_index
(
  uint8 harq_id
);


#endif /* LTE_RLCUL_TX_H */
