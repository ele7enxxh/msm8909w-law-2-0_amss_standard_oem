/*!
  @file
  lte_rlcdli.h

  @brief
  RLC DL internal definitions file.

  @author
  axiao

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdli.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/04/16   cx      CR938962: Reduce dup retx caused by fast nak
04/15/15   ru      CR816655: RLC mitigation to prevent RLF due to FC Shutdown
11/23/14   sb      CR745901: Turn AM to UM mode and do best effort deliver
                   when new Flow contrl level is triggered.
01/03/15   sb      CR776981: Calculate num bytes in RLCDL and other QSH improvements
09/19/14   ru      CR726194: Avoid UE crash when RLCDL header is greater than 512 bytes
01/31/14   mg      CR606601: Extend '0xB097 LTE RLC UL Statistics' log packet to
                   include number of UM RLC PDUs missed
01/23/14   mg      CR599494: RLC behavior to alleviate the low DSM count problem.
05/21/13   ax      CR330379: add SFN as part of the PDCP SDUs delivery 
10/25/11   ax      eMBMS support
07/21/11   ax      fixed CR#297113: detect and release huge SDU 
06/28/10   ax      added support for memory based flow control
11/13/09   ax      support for SDU flush within in-sequence segments of VR(R)
10/19/09   ax      log packet enhancement
10/02/09   ax      fix CR212348: make status_prohibit_counter and mask volatile
08/10/09   ax      added support for RLC statistics log pkt
08/06/09   ax      added lte_rlcdli_get_task_scratch_pad() API
04/17/09   ax      update RLCDL logging according to the ICD
04/13/09   ax      add overlapping segment support
01/06/09   ax      support delivery reason
01/06/09   ax      incorporate code review comments
08/19/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDLI_H
#define LTE_RLCDLI_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include "lte_rlc_rrc.h"
#include "lte_rlci.h"
#include "lte_rlc_pdcp.h"
#include "ds3gpp_ext_msg.h"
/*lint -e613*/
/*lint -e528*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief mask when status prohibit is running
*/
#define LTE_RLCDLI_STATUS_PROHIBIT_MASK                                 0

/*! @brief mask when status prohibit is not running
*/
#define LTE_RLCDLI_STATUS_ALLOWED_MASK                          0xFFFFFFFF

/*! @brief mask when status is not triggered
*/
#define LTE_RLCDLI_STATUS_NOT_TRIGGERED_MASK                             0

/*! @brief mask when status is triggered
*/
#define LTE_RLCDLI_STATUS_TRIGGERED_MASK                        0xFFFFFFFF

/*! @brief size of the reception buffer for received PDUs
*/
#define LTE_RLCDLI_RECEPTION_BUF_SIZE                                 1024

/*! @brief prepare for future optimization
*/
#define LTE_RLCDLI_GET_RECEPTION_BUF_INDEX(sn) ( (sn) & 0x3ff )

/*! @brief whether or not the sn falls within the UM reordering window
*/
#define LTE_RLCDLI_WITHIN_UM_REORDERING_WIN(sn, vr_h, seq_modulus, mask) \
(                                                                        \
  LTE_RLCI_SEQ_GE(LTE_RLCI_SEQ_DEC(vr_h,mask), sn, seq_modulus)          \
)   

/*! @brief whether or not the sn falls within the AM receiving window
*/
#define LTE_RLCDLI_WITHIN_AM_RECEIVING_WIN(sn, vr_r)               \
(                                                                  \
  LTE_RLCI_SEQ_GE(sn, vr_r, LTE_RLCI_SN10_MODULUS)                 \
)

/*! @brief trailing edge of a window for a given leading edge
*/
#define LTE_RLCDLI_WIN_TRAILING_EDGE(vr_h, seq_modulus, mask)      \
(                                                                  \
  ((vr_h) + ((seq_modulus) >> 1)) & (mask)                         \
)

/*! @brief get the pdu ptr in reception buffer for a given sn
*/
#define LTE_RLCDLI_GET_PDU_PTR(ctrl_blk_ptr, sn)                   \
(                                                                  \
  (ctrl_blk_ptr)->reseq_buf_ptr->                                  \
    buf[LTE_RLCDLI_GET_RECEPTION_BUF_INDEX((sn))].head_pdu_ptr     \
)                                                                  \

/*! @brief REQUIRED one-sentence brief description of this macro
*/
#define LTE_RLCDLI_RND_UP_BIT_2_BYTE(x)             (((x) + 7) >> 3)

/*! @brief RLC DL RB state enum
*/
typedef enum
{
  LTE_RLCDLI_STATE_INACTIVE = 0,     /*!< Not active */
  LTE_RLCDLI_STATE_ACTIVE,           /*!< active */
  LTE_RLCDLI_STATE_REESTABLISHMENT,  /*!< Reestablishment in progress */
  LTE_RLCDLI_STATE_MAX               /*!< Max */
} lte_rlcdli_state_e;

/*! @brief defintion of RECEPTION_BUF element
*/
typedef struct
{
  dsm_item_type*  head_pdu_ptr;  /*!< pointer to the pdu/head pdu segment*/
} lte_rlcdli_reception_buf_element_s;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  lte_rlcdli_reception_buf_element_s  buf[LTE_RLCDLI_RECEPTION_BUF_SIZE];  
                                         /*!< reception buffer for rxed PDUs */
  dsm_item_type*     sdu_fragment_ptr;   /*!< residual sdu fragment */
  uint32             sdu_fragment_sz;    /*!< size of the sdu fragment */
  uint32             flush_offset;       /*!< segment offset up to which SDU 
                               has already been flushed and delivered to PDCP */
} lte_rlcdli_reception_buf_s;

/*! @brief RLC DL state varaibles
*/
typedef struct
{
  uint16             vr_r;              /*!< VR(R) or VR(UR) */
  uint16             vr_x;              /*!< VR(X) or VR(UX) */
  uint16             vr_ms;             /*!< VR(MS) */
  uint16             vr_h;              /*!< VR(H) */
} lte_rlcdli_state_var_s;

/*! @brief structure related to the NAK queue 
*/
typedef struct
{
  q_type  nak_q;                /*!< nak queue, lockless, use extern lock */
  uint32  nak_q_size_in_bits;   /*!< nak queue size in bits  */
  volatile uint32  status_prohibit_mask; /*!< 0: status prohibit timer is 
                                        running; 0xffffffff:otherwise */
  uint32  status_trigger_mask;  /*!< 0:no status reporting has been triggered
                                     0xffffffff otherwise */
  uint16  prev_nak_q_size; /*!< the old nak q size */
  uint16  prev_vr_r; /*!< the old nak vr_r*/
  uint16  num_new_holes; /*!< the number of detected new holes */
} lte_rlcdli_rx_status_s;

/*! @brief structure related to timers
*/
typedef struct
{
  volatile int32             status_prohibit_counter; /*!< remaining number of 
  ms before T_status_prohibit expires. 0 means the timer is not running.  */
  int32             reordering_counter; /*!< remaining number of ms 
  before T_reordering expires. 0 means the timer is not running.  */
  int32*            reordering_counter_ptr; /*!< NULL if timer not running, 
  otherwise it should point to reordering_counter */
} lte_rlcdli_timer_s;

/*! @brief RLC DL statistics, part of the control block
*/
typedef struct
{
  uint32  num_data_pdu;       /*!< number of RLC data pdu received */
  uint64  data_pdu_bytes;     /*!< RLC data PDU in bytes received */
  uint32  num_status_rxed;    /*!< number of status rxed and forwarded to UL */
  uint64  status_rxed_bytes;  /*!< number of bytes in status rxed for UL */
  uint32  num_invalid_pdu;    /*!< number of invalid PDUs */
  uint64  invalid_pdu_bytes;  /*!< invalid PDU in bytes */
  uint32  num_retx_pdu;       /*!< number of retx PDUs from receiver perspective
                                   only the transmitter knows the real number */
  uint64  retx_pdu_bytes;     /*!< retx PDU in bytes */
  uint32  num_dup_pdu;        /*!< number of duplicate PDUs received */
  uint64  dup_pdu_bytes;      /*!< bytes that are received as duplicates */
  uint32  num_dropped_pdu;    /*!< number of dropped PDUs, excluding dups */
  uint64  dropped_pdu_bytes;  /*!< dropped PDU in bytes, excluding dups */
  uint32  num_dropped_pdu_fc; /*!< number of dropped PDUs due to flow control */
  uint64  dropped_pdu_bytes_fc;/*!< dropped PDU in bytes due to flow control */
  uint32  num_sdu;            /*!< number of RLC sdu reassembled */
  uint64  sdu_bytes;          /*!< RLC SDUs in bytes reassembled */
  uint32  num_nonseq_sdu;     /*!< number of out of order SDU reassembled in HO */
  uint32  num_status_sent;    /*!< number of status PDU sent */
  uint32  num_comp_nack;      /*!< number of complete PDU NACKs sent in status */
  uint32  num_segm_nack;      /*!< number of segmented PDU NACKs sent */
  uint32  num_reord_exp;      /*!< number of times t_reordering expired */
  uint32  num_reord_start;    /*!< number of times t_reordering started */
  uint32  num_missed_um_pdu;  /*!< number of UM PDUs missing on DL*/
  uint32  num_sdu_bytes_flushed_fc; /*!< num of SDU bytes flushed by LEVEL1_FC */
  uint32  num_fi_mismatch;    /*!< Num of FI mismatch in RLC */
} lte_rlcdli_stats_set_s;

/*! @brief RLC statistics of the RB
*/
typedef struct
{
  uint16                  num_rst; /*!< number of reestablishments since activated */
  lte_rlcdli_stats_set_s  curr_stats;  /*!< statistics of the RB since last reestablishment/activation*/
  lte_rlcdli_stats_set_s  stats_upto_rst; /*!< accumulation upto the last est*/
} lte_rlcdli_stats_s;

/*! @brief configuration for the RB including derived configuration for 
     better performance
*/
typedef struct
{
  lte_rlcdl_rb_info_s  rb_cfg;          /*!< configuration passed down from RRC */
  lte_rlcdl_cfg_u      cfg_used;        /*!< configuration used locally in RLC*/
  uint16               seq_modulus;     /*!<modulo value for seq num */
  uint16               seq_mask;        /*!<sequence mask */
  uint16               rb_type_log_mask;/*!< pkt logging mask for SRB/DRB */
  uint8                fixed_hdr_size;  /*!<size in byte for the fixed hdr portion */
  ds_3gpp_lte_qci_enum_type qci_value;  /*!< QCI value */
} lte_rlcdli_rb_info_s;

/*! @brief RLC DL control block definition
*/
typedef struct
{
  lte_rlcdli_state_e          rb_state; /*!< state of the RB */
  lte_rlcdli_rb_info_s        cfg;      /*!< configuration for the RB */
  lte_rlcdli_state_var_s      state_var;/*!< state variables of the RB */
  lte_rlcdli_reception_buf_s* reseq_buf_ptr; /*!< ptr to resequencing buffer */
  lte_rlcdli_timer_s          timers;   /*!< timers related info */
  lte_rlcdli_rx_status_s      status;   /*!< AM status PDU related info */
  pthread_mutex_t             lock;     /*!< Mutex needed for reentrance */
  lte_rlcdli_stats_s          stats;    /*!< statistics of the RB */
  uint32                      num_bytes_rlcdl; /*!< Num of bytes in RLCDL buff */
  uint32                      fc_mask;  /*!< Flow control mask */
} lte_rlcdli_ctrl_blk_s;

typedef lte_rlcdli_ctrl_blk_s* lte_rlcdli_ctrl_blk_ptr;


/*==============================================================================

  FUNCTION:  lte_rlcdli_clear_status_trigger

==============================================================================*/
/*!
    @brief
    clear the status reporting trigger.

    @return
    None
*/
/*============================================================================*/
static inline void lte_rlcdli_clear_status_trigger
(
  lte_rlcdli_ctrl_blk_ptr  ctrl_blk_ptr  /*!< ptr to the control block */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  ctrl_blk_ptr->status.status_trigger_mask = 
    LTE_RLCDLI_STATUS_NOT_TRIGGERED_MASK;
} /* lte_rlcdli_clear_status_trigger() */

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlcdli_init( void );

extern void lte_rlcdli_signal_pdcp( void );

extern void lte_rlcdli_flush_um
(
  lte_rlcdli_ctrl_blk_ptr    ctrl_blk_ptr
);

extern void lte_rlcdli_flush_am
(
  lte_rlcdli_ctrl_blk_ptr    ctrl_blk_ptr
);

extern void lte_rlcdli_forced_flush_um
(
  lte_rlcdli_ctrl_blk_ptr    ctrl_blk_ptr,
  uint16                     sn
);

extern void lte_rlcdli_forced_flush_am
(
  lte_rlcdli_ctrl_blk_ptr    ctrl_blk_ptr,
  uint16                     sn
);

extern void lte_rlcdli_set_sdu_del_reason
(
  lte_rlcdl_sdu_del_reason_e del_reason
);

extern void lte_rlcdli_extract_lis
(
  lte_rb_rlc_mode_e             rb_mode,        
  uint8                         fixed_hdr_size,
  dsm_item_type*                dsm_ptr       
);

extern boolean lte_rlcdli_get_li
(
  uint16* li  
);

extern uint32 lte_rlcdli_get_q_full_cnt( void );

extern void lte_rlcdli_cfg_rxed( void );

extern void lte_rlcdli_detect_huge_sdu
(
  dsm_item_type**  dsm_ptr, 
  uint32*          size_ptr,    
  lte_rb_cfg_idx_t rb_cfg_idx   
);

extern void lte_rlcdli_set_sfn
(
  lte_sfn_s        sfn
);

extern void lte_rlcdli_start
(
  void
);

extern void lte_rlcdli_stop
(
  void
);

extern void lte_rlcdli_set_status_trigger
(
  lte_rlcdli_ctrl_blk_ptr  ctrl_blk_ptr  /*!< ptr to the control block */
);

#endif /* LTE_RLCDLI_H */
