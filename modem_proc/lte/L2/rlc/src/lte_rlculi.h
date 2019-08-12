/*!
  @file
  lte_rlculi.h

  @brief
  This file contains all the internally defined structures/function prototypes
  for RLC uplink module.

  @author
  araina

*/

/*==============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlculi.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/25/16   sb      CR987470:Static mutex for PDU Build and Retx Q locks
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
05/15/15   sb      CR 803880: RLC Changes for UDC feature
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
07/15/14   ru      CR694328: RLC UL Fast Retx Modem Exp Fix
06/12/14   ru      CR665154: SRLTE: Fast retransmission of UL RLC packets lost 
                   due to HARQ failures in Tuneaway
06/10/14   sb      CR674016: Fix DSM leak when RLF recovery fails
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features 
12/10/12   sb      CR399603: Avoid flow control for high priority bearers to maintain 
                         voice quality
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
07/18/11   ax      support RLC statistics query 
07/13/10   ar      added num_max_pdus_per_lc_exceeded stat
07/06/10   ar      added num_partial_pdcp_hdr_nacks_rcvd stat
10/19/09   ax      log packet enhancement
10/08/09   ax      add num_t_poll_retx_expiry to statistics collected
09/17/09   ax      add num_invalid_ctrl_pdus_rcvd as part of the statistics
09/04/09   ax      UL statistics log pkt support
04/10/09   ar      changed byte_without_poll datatype from uint16 to uint32
02/19/09   ar      1 changed the t_poll_retx timer from uint16 to int16
                   2 added pdu build mutex lock
                   3 added function lte_rlculi_dsm_chain_enqueue()
02/18/09   ar      Added lte_rlculi_deinit() for freeing rlculi.c resources
11/20/08   ar      lte_rb_cfg_idx_t is used as a unique identifier, as opposed
                   to lte_rb_id_t, across AS. Updated book keeping structures
                   and handling accordingly.
11/17/08   ar      1. Added lte_rlculi_init() function declaration
                   2. Removed lte_rlculi_init_active_rb_info() and
                   lte_rlculi_get_ctrl_blk_by_index() function declarations
10/30/08   ar      Addressed code review comments
10/12/08   ar      Added basic UM support
09/16/08   ar      Added basic RLCUL configuration support
09/10/08   ar      intial version

==============================================================================*/

#ifndef LTE_RLCULI_H
#define LTE_RLCULI_H

/*==============================================================================

						   INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <mutils_circ_q.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include <lte_rlc_rrc.h>

#include "lte_rlci.h"
#include "lte_rlc_log.h"
#include "lte_rlc_msg.h"
#include "ds3gpp_ext_msg.h"
#include "lte_mac_msg.h"
#include "lte_mac_rlc.h"


/*==============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

==============================================================================*/

/*! @brief Maximum size of the Transmission Window (TX_WIN) size */
#define LTE_RLCULI_MAX_TX_WINDOW_SIZE LTE_RLCI_AM_HALF_MOD


/*! @brief Periodicity in milliseconds of the RLCUL periodic timer
*/
#define LTE_RLCULI_TIMER_PERIODICITY 5


/*! @brief This macro is used to lock a given mutex
*/
#define RLCUL_LOCK_MUTEX()


/*! @brief This macro is used to unlock a given mutex
*/
#define RLCUL_UNLOCK_MUTEX()


/*==============================================================================

				   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Enumeration for various RLCUL RB states */
typedef enum
{
  LTE_RLCULI_RB_STATE_INACTIVE = 0,  /*!< RB has not been configured yet */
  LTE_RLCULI_RB_STATE_RE_EST,        /*!< RB is under RLC Re-establishment */
  LTE_RLCULI_RB_STATE_ACTIVE,        /*!< RB is configured and active */
  LTE_RLCULI_RB_STATE_VALUE_MAX      /*!< Max Value for this enum */
} lte_rlci_rb_state_e;


/*! @brief Transmission window (TX_WIN) element structure for AM entities.
 *  TX_WIN which holds the information of all the transmitted pdus that have not
 *  been ACKed by eNB yet.
*/
typedef struct
{
  uint16         seq_num;            /*!< RLC SN associated with the RLC pdu */
  uint16         rlc_payload_size;   /*!< Size(in bytes) of the rlc pdu payload.
                                          This size excludes RLC header size */
  dsm_item_type* pdu_chain_ptr;      /*!< DSM chain head pointer of the RLC SDUs
                                          belonging to this RLC PDU */
  uint16         first_sdu_offset;   /*!< Offse(in bytes) of the 1st SDU. Offset
                                          is zero if it's the start of SDU and
                                          non-zero if it’s a sdu segment */
  uint8          num_complete_sdus;  /*!< For storing the num of SDUs ending in
                                          this PDU (used during the dsm item
                                          freeing procedure) */
  uint8          ref_count;          /*!< Reference counter for keeping track of
                                          num of segments of a given RLC PDU in
                                          the RE_TX_Q. NOTE: num of segments is
                                          not expected to exceed 255 */
  uint8          retx_count;         /*!< Count to maintain the num of times a
                                          seq number has been retx-ed. If
                                          retx_count = Max_Retx_Threshold, an
                                          indication is sent to RRC about the
                                          same. RRC should re-est all the RBs.
                                          Meanwhile, if RLCUL receives any pdu
                                          build call from MACUL, it will still
                                          build pdus */
} lte_rlculi_tx_win_elem_s;


/*! @brief This structure maintains the dsm item information about an SDU that
 * is partially transmitted out of the UE.
*/
typedef struct
{
  dsm_item_type*  sdu_dsm_ptr;    /*!< Pointer to the RLC SDU dsm item */
  uint16          sdu_size;       /*!< PDCP header + PDCP payload size */
  uint16          sdu_offset;     /*!< SDU offset from where data needs to be
                                       picked for next transmission. NOTE: This
                                       offset includes PDCP header */
} lte_rlculi_frag_sdu_s;


/*! @brief This structure holds the re-transmission related information of a
 * particular AM RB
*/
typedef struct
{
  q_type          retx_q;      /*!< RE_TX_Q is applicable only for AM mode. The
                                    RE_TX_Q is a shared resource of MACUL and
                                    RLCUL task. MACUL needs to work on the
                                    RE_TX_Q during the pdu building operation.
                                    RLCUL works on the RE_TX_Q on reception of a
                                    status pdu. Due to possible contention,
                                    RE_TX_Q uses a mutex lock for any kind of
                                    operation. */
  uint32          retx_q_size; /*!< Size in bytes of the data in the retx q */
} lte_rlculi_retx_info_s;


/*! @brief This structure maintains the polling related information for a
 * particular AM RB
*/
typedef struct
{
  uint32  byte_without_poll;   /*!< Total number of bytes (excluding RLC header)
                                    txed since the last poll bit is set */
  uint16  pdu_without_poll;    /*!< Counter of number of PDUs transmitted since
                                    the last poll bit is set */
  int16   t_poll_retx_counter; /*!< Remaining number of ms before
                                    t_poll_retransmit expires. timer value <=0
                                    means the timer is not running. It is
                                    initially set to t_poll_retransmit */
  uint16  poll_sn;             /*!< Poll send state variable. This state
                                    variable holds the value of VT(S)-1 upon the
                                    most recent transmission of a RLC data PDU
                                    with the poll bit set to “1”. It is
                                    initially set to 0. */
  boolean t_poll_retx_running; /*!< Flag to indicate if the t_poll_retransmit
                                    timer is running or not */
  boolean poll_trigger;        /*!< TRUE if poll is triggered, else FALSE. This
                                    flag will be set in RLCUL context if
                                    t_poll_retx expires and UL wm and retx q is
                                    empty and pdu with sn vt_s-1 is moved to the
                                    retx q. When MACUL builds the pdu, it sets
                                    the poll bit and resets the poll_trigger */
  boolean start_t_poll_retx;   /*!< This boolean is set in the MAC UL context
                                    and reset in RLC UL context. If set, RLCUL
                                    starts the t_poll_retx timer */
} lte_rlculi_poll_info_s;


/*! @brief Structure for maintaining different RLC UL state variables */
typedef struct
{
  lte_rlculi_frag_sdu_s frag_sdu; /*!< Once RLC SDU is dequeued from the WM
                                       it is possible that the granted
                                       allotment might allow only a portion
                                       or even nothing to be used from that
                                       SDU for pdu building. In this case,
                                       the remaining portion needs to be
                                       available for tx at the next transmit
                                       opportunity */

  uint16 vt_s;  /*!< Send state variable: VT(S) or VT(US). This state variable
                     holds the value of the SN to be assigned for the next newly
                     generated AMD PDU/UMD PDU, and it serves as the higher edge
                     of the STATUS receiving window for AM. It is initially set
                     to 0, and is updated whenever the RLC entity delivers an
                     AMD PDU with SN = VT(S) or UMD PDU with SN=VT(US) */

  uint16 vt_a;  /*!< Acknowledgment state variable: It holds the SN of the next
                     AMD PDU for which a positive ACK is to be received in
                     sequence. It also serves as the lower edge of the TX window
                     and the STATUS receiving window. It is initially set to 0,
                     and is updated whenever the AM RLC entity receives a
                     positive ACK for an AMD PDU with SN = VT(A).*/

  uint16 vt_ms; /*!< Maximum send state variable:This state variable equals
                     VT(A) + AM_Window_Size, and it serves as the higher edge of
                     the transmitting window (TX_WIN) */
  
  lte_rlculi_tx_win_elem_s tx_win[LTE_RLCULI_MAX_TX_WINDOW_SIZE]; /*!< TX window
                                           for storing txed pdu info */

  lte_rlculi_retx_info_s   retx_info; /*!< Re-transmission related info*/

  lte_rlculi_poll_info_s   poll_info; /*!< Polling related info */

} lte_rlculi_state_s;


/*! @brief Structure containing the necessary RLC UL statistic information */
typedef struct
{
  uint32 num_new_data_pdus_txed;      /*!< Total number of new RLC PDUs txed */
  uint32 new_data_pdu_bytes_txed;     /*!< Total new data pdu bytes transmitted
                                           including header */
  uint32 num_sdus_txed;               /*!< Total number of new RLC SDUs txed */
  uint32 sdu_bytes_txed;              /*!< Total number of new RLC SDUs txed in
                                           bytes */
  uint32 num_ctrl_pdus_txed;           /*!< number of ctrl pdu transmitted */
  uint32 ctrl_pdu_bytes_txed;          /*!< Total ctrl pdu bytes transmitted */
  uint32 num_pdus_re_txed;            /*!< Total number of re-txed RLC PDUs */
  uint32 pdu_bytes_re_txed;           /*!< Total pdu bytes re-transmitted */
  uint32 num_ctrl_pdus_rcvd;          /*!< Total num of control PDUs received */
  uint32 num_complete_nacks_rcvd;     /*!< Total number of NACKs received for
                                           complete RLC PDUs */
  uint32 num_segment_nacks_rcvd;      /*!< Total number of NACKs received for
                                           RLC PDU segments */
  uint32 num_partial_pdcp_hdr_nacks_rcvd; /*!< Total number of NACKs received
                                           for partial pdcp hdr retx */
  uint32 num_invalid_ctrl_pdus_rcvd;  /*!< Total num of invalid control PDUs
                                           received */
  uint32 num_poll_pdus_txed;          /*!< Total number of RLC PDUs transmitted
                                           with poll bit set */
  uint32 num_t_poll_retx_expiry;      /*!< Total number of times
                                           t_PollRetransmission expires */
  uint32 num_max_pdus_per_lc_exceeded; /*!< num of times rlc built max pdus per
                                            logical channel */
  uint32 num_fretx_pdus;               /*!< num of fast retx RLC PDUs */
} lte_rlculi_stat_info_s;


/*! @brief Structure for holding the RLCUL RB statistics */
typedef struct
{
  uint16                    num_rst;     /*!< number of reestablishments since
                                          activated */
  lte_rlculi_stat_info_s    curr_stats;  /*!< statistics of the RB since last
                                              re_est or initial activation*/
  lte_rlculi_stat_info_s    total_stats; /*!< cumulative stats of the RB */

  uint32                    num_tx_win_bytes; /*!< Number of bytes held in UL 
                                           for AM mode. For UM mode it is 0.*/
  uint32 num_compwm_empty_pswm_nonempty; /*!< num of times COMP Wm is empty
                                          where PS Wm is non-empty*/
} lte_rlculi_stats_s;


/*! @brief Radio bearer specific configuration related information structure */
typedef struct
{
  lte_rlcul_rb_info_s rb_info;       /*!< RLC UL configuration info */
  lte_rlci_rb_state_e rb_state;      /*!< Radio Bearer state:Possible states are
                                        ACTIVE, RE_EST_IN_PROGRESS, INACTIVE */
  uint16              seq_modulus;   /*!< modulo value for seq num */
  uint16              seq_mask;      /*!< sequence mask */
  uint8               fixed_hdr_len; /*!< Length of the fixed portion of the RLC
                                          header */
  lte_rlcul_cfg_u    cfg_bak;         /*!< configuration back up for NW config */
} lte_rlculi_rb_info_s;


/*! @brief Uplink control block data structure */
typedef struct
{
  lte_rlculi_rb_info_s  cfg;   /*!< RLC UL configuration info */
  lte_rlculi_state_s    state; /*!< RLC UL state info */
  lte_rlculi_stats_s    stats; /*!< RLC UL statistics info */
} lte_rlculi_ctrl_blk_s;


/*! @brief Structure containing the information about all active(configured) RBs
    */
typedef struct
{
  lte_rlculi_ctrl_blk_s*  active_ctrl_blk_ptrs[LTE_MAX_ACTIVE_RB];
                                     /*!< Array of active control block ptrs */
  uint8                   num_active_rbs; /*!< Number of active RBs */
} lte_rlculi_active_rb_info_s;

typedef struct
{
  ds_3gpp_lte_qci_enum_type qci_value [LTE_MAX_EPS];
}lte_rlculi_qos_info_s;

typedef struct
{
  pthread_mutex_t       pdu_build_lock; /*!< Lock to maintain consistency during
                                                            pdu building (MACUL) and config
                                                            updates (RLCUL) */
  pthread_mutex_t       retx_q_lock; /*!< Mutex lock over the retx q to guarantee
                                                          re-entrance between MACUL and RLCUL */
  boolean               in_use; /*!< Boolean to indicate if mutex is in use*/
}lte_rlculi_rb_mutex_s;

typedef struct
{
  lte_rlculi_rb_mutex_s  rlculi_mutex [LTE_MAX_ACTIVE_RB]; 
  lte_rlculi_rb_mutex_s* rlculi_mutex_ptr[LTE_MAX_RB_CFG_IDX];
}lte_rlculi_rb_mutex_info_s;

typedef struct
{
  boolean      lcid_prioritized [LTE_MAX_LC]; 
  /*EPSID may have QCI=1 but current LCID mapped it to may not required to prioritized*/
}lte_rlculi_lcid_priority_s;



/*==============================================================================

					EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlculi_init

==============================================================================*/
/*!
    @brief
    This function initializes all the data members belonging to lte_rlculi.c
    file.

    @detail
    Following variables are initialized:
    - UL Control block
    - Active RB info array
    - RB_CFG_IDX to control block index array
    - LC_ID to control block index array
    - Free control block pointer q
    - AM retx queue
    - retx_q_lock

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_init
(
  void
);
/* lte_rlculi_init */


/*==============================================================================

  FUNCTION:  lte_rlculi_deinit

==============================================================================*/
/*!
    @brief
    Free resources belonging to rlculi file

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_rlculi_deinit
(
  boolean verify_connect_rel /* whether to verify connection released */
);
/* lte_rlculi_deinit() */


/*==============================================================================

  FUNCTION:  lte_rlculi_get_ctrl_blk_by_rb_cfg_idx

==============================================================================*/
/*!
    @brief
    This function returns the control block pointer of the requested RB config
    index.

    @return
    The returned pointer is of type lte_rlculi_ctrl_blk_s

    @see related_function()
    Alternatively, for getting the control block using lc_id instead of
    rb_cfg_idx use lte_rlculi_get_ctrl_blk_by_lc_id().
*/
/*============================================================================*/
EXTERN lte_rlculi_ctrl_blk_s* lte_rlculi_get_ctrl_blk_by_rb_cfg_idx
(
  lte_rb_cfg_idx_t rb_cfg_idx /* Radio Bearer configuration index */
);
/* lte_rlculi_get_ctrl_blk_by_rb_cfg_idx() */


/*==============================================================================

  FUNCTION:  lte_rlculi_get_ctrl_blk_by_lc_id

==============================================================================*/
/*!
    @brief
    This function returns the control block pointer of the requested LC ID.

    @return
    The returned pointer is of type lte_rlculi_ctrl_blk_s

    @see related_function()
    Alternatively, for getting the control block using rb_cfg_idx instead of
    lc_id use lte_rlculi_get_ctrl_blk_by_rb_cfg_idx().
*/
/*============================================================================*/
EXTERN lte_rlculi_ctrl_blk_s* lte_rlculi_get_ctrl_blk_by_lc_id
(
  lte_lc_id_t lc_id /* Logical Channel ID */
);
/* lte_rlculi_get_ctrl_blk_by_lc_id() */


/*==============================================================================

  FUNCTION:  lte_rlculi_get_active_rb_info

==============================================================================*/
/*!
    @brief
    This function can be used to get the active radio bearer information.

    @return
    Pointer to the variable of type lte_rlculi_active_rb_info_s, holding the
    information of active radio bearers.

    @retval value
    The pointer points to the structure containing:
    num_active_rbs - Number of active RBs
    mapping_tbl[LTE_MAX_ACTIVE_RB] - Mapping table contains tuples of
    <ctrl block index, lc_id, rb_cfg_idx> for all active radio bearers.
*/
/*============================================================================*/
EXTERN lte_rlculi_active_rb_info_s* lte_rlculi_get_active_rb_info
(
  void
);
/* lte_rlculi_get_active_rb_info() */


/*==============================================================================

  FUNCTION:  lte_rlculi_add_active_rb_info

==============================================================================*/
/*!
    @brief
    This function adds a newly added RB info to the RLC book keeping arrays
    which hold information about active Radio Bearers.

    @detail
    Following arrays are updated:
    rb_cfg_idx_2_ctrl_blk[]
    lc_id_2_ctrl_blk[]

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_add_active_rb_info
(
  lte_rlculi_ctrl_blk_s* ctrl_blk_ptr  /*!< Control block ptr of the added RB */
);
/* lte_rlculi_add_active_rb_info */


/*==============================================================================

  FUNCTION:  lte_rlculi_modify_active_rb_info

==============================================================================*/
/*!
    @brief
    This function updates the the RLC book keeping arrays which hold information
    about active Radio Bearers. RB info of modified radio bearers is updated.

    @detail
    Following arrays are updated:
    rb_cfg_idx_2_ctrl_blk[]
    lc_id_2_ctrl_blk[]

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_modify_active_rb_info
(
  lte_rlculi_ctrl_blk_s* ctrl_blk_ptr  /*!< Control block ptr of modified RB */
);
/* lte_rlculi_modify_active_rb_info */


/*==============================================================================

  FUNCTION:  lte_rlculi_delete_active_rb_info

==============================================================================*/
/*!
    @brief
    This function deletes the RB info from the RLC book keeping arrays
    which hold information about active Radio Bearers.

    @detail
    Following arrays are updated:
    rb_cfg_idx_2_ctrl_blk[]
    lc_id_2_ctrl_blk[]

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_delete_active_rb_info
(
  lte_rlculi_ctrl_blk_s* ctrl_blk_ptr  /*!< Control block ptr of deleted RB */
);
/* lte_rlculi_delete_active_rb_info */


/*==============================================================================

  FUNCTION:  lte_rlculi_periodic_timer_exp_handler

==============================================================================*/
/*!
    @brief
    This function handles the RLCUL periodic timer expiry and calls the relevant
    RLCUL functions.

    @detail
    Following RLCUL functionality is related to timer expiry:
    - Freeing up of RLCUL txed UM pdus.

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_periodic_timer_exp_handler
(
  void
);
/* lte_rlculi_periodic_timer_exp_handler */


/*==============================================================================

  FUNCTION:  lte_rlculi_dsm_chain_enqueue

==============================================================================*/
/*!
    @brief
    This function enqueues a source dsm item/chain to a target dsm chain.

    @detail
    none

    @return
    void

*/
/*============================================================================*/
EXTERN void lte_rlculi_dsm_chain_enqueue
(
  dsm_item_type** src_chain_h_ptr,    /*!< source dsm chain head pointer */
  dsm_item_type** src_chain_t_ptr,    /*!< source dsm chain tail pointer */
  dsm_item_type** target_chain_h_ptr, /*!< target dsm chain head pointer */
  dsm_item_type** target_chain_t_ptr  /*!< target dsm chain tail pointer */
);
/* lte_rlculi_dsm_chain_enqueue */


/*==============================================================================

  FUNCTION:  lte_rlculi_get_stats

==============================================================================*/
/*!
    @brief
    fill in RLCUL statistics.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_rlculi_get_stats
(
  lte_rlcul_stats_req_s*      stats_req_msg /*!< pointer to request msg */
);
/* lte_rlculi_get_stats */

#endif /* LTE_RLCULI_H */
