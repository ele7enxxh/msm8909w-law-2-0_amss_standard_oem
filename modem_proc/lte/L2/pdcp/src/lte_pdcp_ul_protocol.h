/*!
  @file
  lte_pdcp_ul_protocol.h

  @brief
  This file contains defines and functions interfaces for PDCP UL protocol stack.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_ul_protocol.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/24/15   sb      CR938646: Reduce frequency of F3s without impacting debugging
03/01/16   sb      CR979285: UDC HO fixes and enhancements
09/16/15   ru      CR896720: PDCP Discard Timer Overwrite
09/08/15   ar      CR894006: UDC Reset Error Handling
07/23/15   sb      CR867973: Enhance logging and QSH for UDC
07/20/15   sb      CR872867: For Full config, enable flow only if it was not 
                   suspended due to COMP or OFFLOAD tasks
06/11/15   sb      CR813882: Postpone re-registration of callback with MACUL after 
                   recompression until flow is resumed
06/29/15   ar      include customer.h for UDC and ELS feature definition
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
11/11/14   sb      CR748751: For RLF do not enable UL flow after recompression
10/15/14   sb      CR733993: Do not re-enqueue packet after RAB is dereg
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
04/11/14   mg      CR647594: QSH Phase 1
07/24/14   mg      CR699600: Revert CR669056
07/14/14   mg      CR665352: PDCP UL statistics API
07/03/14   mg      CR669056: PDCP discard timer algorithm
06/10/14   sb      CR672057: Revert timings of PDCP re-establishment to original and 
                   increase Num of IRs to huge number
06/12/14  sb       CR656696: KW fixes
05/08/14  sb       CR660943: Fix bug introduced because of CR642185
04/25/14  sb       CR642185 :Delay PDCP re-establishment until RRC notifies for 
                   re-establishment complete
05/08/14   mg      CR 500110: Klockwork Fixes
04/02/14   sb      CR642230: REL11 RoHC context continuity during HO 
                   within same eNB
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
10/02/13   mg      CR489345: Get num of PDCP PDU bytes transmitted for an EPSID
06/06/13   sb      CR495372: Remove LTE_PDCPUL_CRYPTO_DONE_IND
                   message and handle integrity checked SRB SDU synchronously
06/19/13   sb      CR458155: Set the reason for RoHC Reset (HO/Discard) so that
                   OFFLOAD calls the correct RoHC IOCTL
05/24/13   sb      CR491559: Free metainfo for packets sent for Re-RoHC and reallocate
                   them on recompression of packets
04/05/13   sb      CR457099: PDCP UL logging timer optimization
03/20/13   sb      CR464450: When EPSID and RBID mapping changes during 
                   RoHC ON/OFF testing, make sure to have PS watermark pointed 
                   correctly by OFFLOAD or PDCUL.
11/20/12   ax      CR383618: L2 modem heap use reduction phase I  
06/16/12   sb      CR365196 and CR365004: Fix to handle scenarios 
                   where RB switches back and forth with RoHC enabled and 
                   disabled. Also to reduce the delay in UL after HO
10/06/11   ax      make use of non_empty callback for PS-PDCP watermark
09/06/11   ax      add support for piggyback RoHC feedback
06/15/11   ax      RoHC support
03/06/11   ax      add support for release 9 PDCP full configuration
12/02/10   ax      fixed CR266146 cipher with old key during RLF or HO.
08/25/10   ar      save cfg for RECFG and HO cases and clean it after
                   LTE_RRC_CONFIG_COMPLETED_IND
07/08/10   ax      fixed flow state issue
05/19/10   ar      added PDCP ciphering log packet support
02/10/09   ar      added PS->PDCPUL wm enqueue ind optimization
11/09/09   ax      relocated num_rst in statistics
09/10/09   bq      RLF reestablish fix
08/27/09   bq      Security Corner case fix
08/03/09   bq      RLF implementation
02/22/09   bq      add flow suspend state during HO/RB release
02/17/09   bq      fix cfg validation
02/02/09   bq      eNodeB generate status report handling
01/28/09   bq      add peer ctrl ind handler
07/18/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_UL_PROTOCOL_H
#define LTE_PDCP_UL_PROTOCOL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <customer.h>
#include <comdef.h>
#include <custlte.h>
#include "ps_rohc.h"
#include "lte_l2_comdef.h"
#include "mutils_circ_q.h"
#include "lte_pdcp_msg.h"
#include "lte_pdcp_offload_msg.h"
#include "lte_pdcp_offload_protocol.h"
#include "lte_pdcp_ul_if.h"
#include "lte_pdcpi.h"
#include "lte_l2_common.h"
#include "qsh.h"
#include "lte_pdcp_msg.h"
#include "ds3gpp_ext_msg.h"

#ifdef FEATURE_LTE_UDC_ENABLED
#include "lte_pdcp_comp_msg.h"
#endif /*FEATURE_LTE_UDC_ENABLED*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief PDCP RB UL state
*/
typedef enum
{
  LTE_PDCPUL_IDLE = 0,
  LTE_PDCPUL_ACTIVE = 1
} lte_pdcpul_state_e;

/*! @brief bit mask (first LSB) that marks the watermark state:
     0 for enable; 1 for disable
*/
#define LTE_PDCPUL_FLOW_MASK_DISABLE                            1

/*! @brief bit mask (second LSB) that marks the flow state:
     1 for suspend; 0 for resume
*/
#define LTE_PDCPUL_FLOW_MASK_SUSPEND                            2

/*! @brief bit mask (third LSB) that marks the flow state:
     1 for suspend_COMP; 0 for resume_COMP
*/
#define LTE_PDCPUL_FLOW_MASK_SUSPEND_COMP                       4

/*!@brief Get Discard Tick Mask
16 MSB bits of app_field carry discard_tick
*/
#define LTE_PDCP_GET_DISC_TICK(x)         ((x >> 16) & 0x0000FFFF)

/*!@brief Set Discard Tick Mask
16 MSB bits of app_field carry discard_tick
*/
#define LTE_PDCP_SET_DISC_TICK(x)         (x <<= 16)

/*!@brief  3 MSB bits of PDCP header will have '111' if it is an ELS SDU
*/
#define LTE_PDCP_ELS_BIT_MASK                   0xE000


/*!@brief PDCP header 1st byte masking
*/
#define LTE_PDCP_HDR_FIRST_BYTE(x)        ((x >> 8) & 0x000000FF)

/*!@brief PDCP header 2nd bytemasking
*/
#define LTE_PDCP_HDR_SECOND_BYTE(x)        (x & 0x000000FF)


/*!@brief Set PDCP SRB cipher algo
Bits 20th, 21st and 22nd carry SRB cipher algo
*/
#define LTE_PDCP_SET_SRB_CIPH_ALGO(x)        (x << 20) 

/*!@brief Set PDCP SRB cipher index
Bits 16th to 19th carry SRB cipher index
*/
#define LTE_PDCP_SET_SRB_CIPH_IDX(x)        (x << 16)


/*!@brief Get PDCP SRB cipher algo
Bits 20th, 21st and 22nd carry SRB cipher algo
*/
#define LTE_PDCP_GET_SRB_CIPH_ALGO(x)        ((x >> 20) & 0x00000007)

/*!@brief Get PDCP SRB cipher index
Bits 16th to 19th carry SRB cipher index
*/
#define LTE_PDCP_GET_SRB_CIPH_IDX(x)        ((x >> 16) & 0x0000000F)


/*!@brief ELS mini-signature for an ELS SDU
complement of PDCP SN + '010'
*/
#define LTE_PDCP_ELS_MINI_SIGN(x)         ((((~x)<<3) | (0x2)) & 0x000000FF)

/*!@brief To check if packet is compressed
or uncompressed'
*/
#define LTE_PDCP_IS_COMP_PKT(x)         ((x) & LTE_PDCPUL_PKT_TYPE_COMP)

/*!@brief To check if packet is PDCP Control PDU
*/
#define LTE_PDCP_IS_CTRL_PDU(x)         ((x) == LTE_PDCPUL_PKT_TYPE_CONTROL)

/*!@brief To check if packet is new uncompressed'
*/
#define LTE_PDCP_IS_NEW_PKT(x)         ((x) == LTE_PDCPUL_PKT_TYPE_NEW)

/*!@brief To check if packet is retx compressed
or retx uncompressed'
*/
#define LTE_PDCP_IS_RETX_PKT(x)         ((x) & LTE_PDCPUL_PKT_TYPE_RETX)

/*!@brief Set packet type as 'NEW' to kind field of the packet
*/
#define LTE_PDCP_SET_PKT_TYPE_NEW(dsm_ptr)       \
(                                                \
  (dsm_ptr)->kind = LTE_PDCPUL_PKT_TYPE_NEW      \
)                                                

/*!@brief Set packet type as 'RETX' to kind field of the packet
*/
#define LTE_PDCP_SET_PKT_TYPE_RETX(dsm_ptr)       \
(                                                 \
  (dsm_ptr)->kind = LTE_PDCPUL_PKT_TYPE_RETX      \
)                                                 

/*!@brief Set packet type as 'COMP' to kind field of the packet
*/
#define LTE_PDCP_SET_PKT_TYPE_COMP(dsm_ptr)         \
(                                                   \
  (dsm_ptr)->kind = LTE_PDCPUL_PKT_TYPE_COMP     \
)                                                   
 
 /*!@brief Set packet type as 'CONTROL' to kind field of the packet
 */
#define LTE_PDCP_SET_PKT_TYPE_CTRL(dsm_ptr)         \
(                                                   \
  (dsm_ptr)->kind = LTE_PDCPUL_PKT_TYPE_CONTROL     \
)                                                   

/*! @brief copy basic PDCP hdr and discard info from source to target
*/
#define LTE_PDCP_HDR_DISC_COPY_INFO(src_dsm_ptr, target_dsm_ptr)     \
(                                                               \
  (target_dsm_ptr)->app_field = (src_dsm_ptr)->app_field        \
)

/*! @brief copy basic PDCP pkt type info from source to target
*/
#define LTE_PDCP_PKT_TYPE_COPY_INFO(src_dsm_ptr, target_dsm_ptr)     \
(                                                               \
  (target_dsm_ptr)->kind = (src_dsm_ptr)->kind        \
)

/*! @brief PDCP RB UL flow state
*/
typedef enum
{
  LTE_PDCPUL_FLOW_ENABLE = 0,  /* enabled */
  LTE_PDCPUL_FLOW_DISABLE = LTE_PDCPUL_FLOW_MASK_DISABLE, /* cross high wm */
  LTE_PDCPUL_FLOW_SUSPEND = LTE_PDCPUL_FLOW_MASK_SUSPEND, /* during HO or
                                                   RB release or RLF suspend */
  LTE_PDCPUL_FLOW_DISABLE_SUSPEND = LTE_PDCPUL_FLOW_MASK_SUSPEND |
                                    LTE_PDCPUL_FLOW_MASK_DISABLE, /* during HO
                                    or RB release or RLF suspend */
  LTE_PDCPUL_FLOW_SUSPEND_COMP = LTE_PDCPUL_FLOW_MASK_SUSPEND_COMP, /* during UDC
                                    compression */
  LTE_PDCPUL_FLOW_DISABLE_SUSPEND_COMP = LTE_PDCPUL_FLOW_MASK_SUSPEND |
                                    LTE_PDCPUL_FLOW_MASK_DISABLE |
                                    LTE_PDCPUL_FLOW_MASK_SUSPEND_COMP /* during HO
                                    or RB release or RLF suspend */
} lte_pdcpul_flow_e;

typedef enum
{
  LTE_PDCPUL_PKT_TYPE_NEW = 1,
  LTE_PDCPUL_PKT_TYPE_COMP=2, 
  LTE_PDCPUL_PKT_TYPE_RETX=4,  
  LTE_PDCPUL_PKT_TYPE_RETX_COMP=(LTE_PDCPUL_PKT_TYPE_RETX | LTE_PDCPUL_PKT_TYPE_COMP),
  LTE_PDCPUL_PKT_TYPE_CONTROL=8
}lte_pdcpul_pkt_type_e;

/*! @brief PDCP RB UL cfg parameter control block
*/
typedef lte_pdcpul_info_s lte_pdcpul_cfg_s;

/*! @brief invalid TX queue index
*/
#define LTE_PDCPUL_INVALID_TX_QUEUE_INDEX 0xFFFF

/* MAX PDCP status report buffer */
#define LTE_PDCPUL_STATUS_REPORT_BYTE_SIZE 256

/* Discard Timer Tick in msec */
#define LTE_PDCPUL_DISCARD_TIMER_TICK_IN_MSEC  10

#ifdef FEATURE_LTE_UDC_ENABLED
/* UDC reset Timer in msec */
#define LTE_PDCPUL_UDC_RESET_TIMER_IN_MSEC     60000

/* UDC reset counter threshold */
#define LTE_PDCPUL_UDC_RESET_CNT_THRESHOLD     5
#endif /*FEATURE_LTE_UDC_ENABLED*/

/* Discard Threshold when ROHC is config */
#define LTE_PDCPUL_DISCARD_THRESHOLD_WITH_ROHC  12

/*! @brief saved configuration info from RRC to PDCP for add/delete RB during HO
 *  this structure is used to reapply the add/delete config later during HO
 *  when HO is completed or RLF is happening
*/
typedef struct
{
  /*!< number of released RB */
  uint8                               num_released_rb;
  /*!< released RB cfg index */
  lte_rb_cfg_idx_t                    released_rb_cfg_idx[LTE_MAX_ACTIVE_RB];
  /*!< number of add RB */
  uint8                               num_add_rb;
  /*!< add RB Cfg index */
  lte_rb_cfg_idx_t                    add_rb_cfg_idx[LTE_MAX_ACTIVE_RB];
  /*!< UL RB config data base ptr */
  lte_pdcpul_info_s                   *rb_cfg_base_ptr;

} lte_pdcpul_saved_cfg_req_s;

/*! @brief PDCP RB UL state control block
*/
typedef struct
{
  lte_pdcpul_state_e state;   /*!< PDCP state */
  lte_pdcpul_flow_e  flow;    /*!< PDCP flow mask */

  uint32             tx_hfn;  /*!< PDCP TX HFN */
  uint32             tx_seq;  /*!< PDCP TX seq */

  uint32             seq_modulus; /*!< PDCP seq modulus */
  uint32             seq_mask; /*!< PDCP seq mask */

  dsm_watermark_type  *ps_to_pdcp_wm_ptr;   /*!< PS to PDCP watermark */

  dsm_item_type      *dl_tx_status_report_ptr; /*!< PDCP DL generate status report */

  /*For Debug info*/

  dsm_item_type      *dequeued_high_dsm_ptr; /*Head of high prio DSM chains to discarded
                                                                                  on calling DSM API*/
                                                                                     	
  dsm_item_type      *dequeued_normal_dsm_ptr; /*Head of normal prio DSM chains to discarded
                                                                                  on calling DSM API*/

  const dsm_item_type      *last_proc_item_disc; /*Last DSM item processed by DSM callback  
                                                                           lte_pdcpul_traverse_wm_to_discard_pkts
                                                                           during discard timer expiry*/
                                                                           
  dsm_wm_iterator_req_enum_type last_iter_req; /*Last iter req processed by DSM callback  
                                                                           lte_pdcpul_traverse_wm_to_discard_pkts
                                                                           during discard timer expiry*/

  /* Discard related state */
  /*!< Number of consecutive discard */
  uint16             num_consecutive_discard;
  /*!< Number of consecutive discard threshold*/
  uint16             num_consecutive_discard_threshold;
  /*!< PDCP discard timer unit based on
       LTE_PDCPUL_DISCARD_TIMER_TICK_IN_MSEC */
  uint16             discard_timer_tick;

  /*Prev bearer state of this rb_id. to be used during reconfig */
  lte_pdcp_bearer_type_e  prev_state;

 /*Curr bearer state of this rb_id. to be used during reconfig */
  lte_pdcp_bearer_type_e  curr_state;

 /*Boolean to indicate recompression for RoHC was requested durng RLF*/
  boolean rohc_recomp_req_rlf;

} lte_pdcpul_state_s;

/*! @brief PDCP statistics of the RB
*/
typedef struct
{
  uint32  num_rst;     /*!< number of reestablishments since activated */
  uint32  udc_comp_state;        /*!<*UDC compression state*/
  lte_pdcpul_stats_set_s  curr_stats;  /*!< statistics of the RB since last reestablishment/activation*/
  lte_pdcpul_stats_set_s  stats_upto_rst; /*!< accumulation upto the last est*/
  uint32 num_suspend_flow;  /*!<number of times UL flow is suspended*/
  uint32 num_resume_flow;   /*!<number of times UL flow is resumed after suspend*/
  uint32 num_suspend_flow_comp; /*!<number of times UL flow is suspended because of request
                                sent to OFFLOAD/COMP*/
  uint32 num_resume_flow_comp;  /*!<number of times UL flow is resumed after suspend because of request
                                sent to OFFLOAD/COMP*/
} lte_pdcpul_stats_s;

typedef struct
{
  /*Pointing to chain of unACKed SDUs during re-establishment*/
  dsm_item_type*  unacked_sdu_head_ptr;
 /*Total bytes of chain of unACKed SDUs during re-establishment*/
  uint32  unacked_sdu_bytes;
}lte_pdcpul_reestab_info_s;

/*! @brief PDCP RB UL control block
*/
typedef struct
{
  lte_pdcpul_cfg_s                  cfg;      /*!< config */
  lte_pdcpul_state_s                state;    /*!< state  */
  lte_pdcpul_stats_s                stats;    /*!< stats  */
  lte_pdcpul_reestab_info_s         reestab_info; /*!< reestab info */
} lte_pdcpul_cb_s;

/*! @brief PS to PDCP watermark related information
*/
typedef struct
{
  dsm_watermark_type*  ps_pdcp_wm_ptr;  /*!< PS to PDCP watermark pointer */
  lte_rb_cfg_idx_t rb_cfg_idx;  /*Corresponding RB_CF_IDX for EPS_ID*/
  lte_pdcp_bearer_type_e rb_map_type;
} lte_pdcpul_eps_info_s;

/*! @brief PDCP UL protocol stack control block
*/
typedef struct
{
  lte_pdcpul_cb_s*    rb_cb[LTE_MAX_RB_CFG_IDX];    /*!< RB control block array */

  lte_pdcpul_cb_s     *discard_cfg_rb_cb[LTE_MAX_ACTIVE_RB];
  uint8               num_discard_cfg_rb;

  /*!< security config */
  lte_pdcp_security_cfg_s security_cfg;

  /* Saved config request during HO. */
  lte_pdcpul_saved_cfg_req_s save_cfg_req;

  uint8                   num_active_rb; /*!<Num of Active RBs*/

  boolean       no_srb_cipher; /*if RRC asks an SDU not to be ciphered*/

} lte_pdcpul_protocol_cb_s;

/*! @brief Callback user data for DSM iterator function
*/
typedef struct
{
  uint32 seq_index;
  lte_rb_cfg_idx_t rb_cfg_idx;
}lte_pdcpul_protocol_dsm_iterator_cb_s;


/*! @brief PDCP UL protocol static data structure
*/
typedef struct
{
  lte_pdcpul_eps_info_s   eps_info[LTE_MAX_EPS]; /*!< EPS related information */
  dsm_watermark_type        lte_pdcpul_dummy_wm;
  q_type                    lte_pdcpul_dummy_q;

  /*when RB is not activated/added, this dummy Ctrl Blk to be pointed to for
  better performance: no check of NULL pointer is needed*/
  lte_pdcpul_cb_s           lte_pdcp_dummy_rb_cb;
  lte_pdcpul_rlc_wm_s  pdcp_rlc_wm[LTE_MAX_ACTIVE_SRB]; /*!< PDCP RLC WM */
  uint16 sys_discard_tick;
  int lte_pdcpul_thread_id;
  uint32  ho_udc_retx_pdus[LTE_MAX_EPS]; /*!< HO UDC Retx PDUs per EPS bearer..
                                          This stats is maintained in between LTE calls */
  uint32  ho_udc_retx_bytes[LTE_MAX_EPS]; /*!< HO UDC Retx bytes per EPS bearer..
                                          This stats is maintained in between LTE calls */
  uint32 num_udc_reset_rlfs;              /*!< num RLFs triggered to RRC due to 
                                               consecutive UDC resets */
  uint32 num_udc_reset_cnt_timer_running; /*!< Num of times UDC reset is received 
                                               while UDC reset timer is running */
} lte_pdcpul_static_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  lte_pdcpul_process_re_est_status_ind_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the reest status ind from RLC UL for
    AM SRB during HO

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpul_process_re_est_status_ind
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type *ack_pdu_ptr,
  dsm_item_type *nack_pdu_ptr,
  dsm_item_type *maybe_pdu_ptr
);

EXTERN void lte_pdcpul_sdu_ind_handler
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type *ack_pdu_ptr,
  dsm_item_type *nack_pdu_ptr,
  dsm_item_type *maybe_pdu_ptr
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig request for add/modified/delete.

    @detail

    @return
    TRUE for success, FALSE for fail

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_process_recfg_req
(
  lte_l2_cfg_reason_e cfg_reason,
  lte_pdcp_cfg_act_e  cfg_act,
  lte_rb_cfg_idx_t  rb_cfg_idx,
  boolean          *no_ctrl_pdu,
  lte_pdcpul_info_s* cfg_info_ptr,
  boolean            log_F3
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_rab_register_req

===========================================================================*/
/*!
    @brief
    This function is called to process RAB register request from PS.

    @detail

    @return
    TRUE for success, FALSE for fail

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_process_rab_register_req
(
  lte_eps_id_t  eps_id,      /*!< EPS ID */
  dsm_watermark_type* ps_to_pdcp_wm_ptr /*!< PS to PDCP WM ptr */
);

/*==============================================================================

FUNCTION:  lte_pdcpul_process_ps_rab_reg_ind_handler

==============================================================================*/
/*!
    @brief
    This function is called to process RAB rab reg ind from OFFLOAD/COMP
    when bearer is modified from ROHC/UDC0->DEFAULT.

    @detail

    @return
    None

    @note

    @see

*/
/*============================================================================*/
extern void lte_pdcpul_process_ps_rab_reg_ind_handler
(
  lte_pdcpul_rab_reg_ind_msg_s* msg_ptr      /*!< EPS ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_rab_deregister_req

===========================================================================*/
/*!
    @brief
    This function is called to process RAB Deregister request from PS.

    @detail

    @return
    TRUE for success, FALSE for fail

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_process_rab_deregister_req
(
  lte_eps_id_t  eps_id      /*!< EPS ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_discard_timer_tick_ind

===========================================================================*/
/*!
    @brief
    This function is called to process discard timer tick ind.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_discard_timer_tick_ind
(
  void
);

#ifdef FEATURE_LTE_UDC_ENABLED
/*==============================================================================

  FUNCTION:  lte_pdcpul_process_reset_udc_reset_timer_cnt

==============================================================================*/
/*!
    @brief
    This function is called to reset UDC reset timer counter.

    @detail

    @return

    @note

    @see

*/
/*============================================================================*/

extern void lte_pdcpul_process_reset_udc_reset_timer_cnt
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_udc_reset_timer_expiry_ind

===========================================================================*/
/*!
    @brief
    This function is called to process UDC reset timer expiry ind.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_udc_reset_timer_expiry_ind
(
  void
);
#endif /*FEATURE_LTE_UDC_ENABLED*/

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_connection_release

===========================================================================*/
/*!
    @brief
    This function is called to process connection release.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_connection_release
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_sdu_req

===========================================================================*/
/*!
    @brief
    This function is called to process SDU request from RRC.

    @detail

    @return
    TRUE for success

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_process_sdu_req
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  uint8             mu_id,
  dsm_item_type*    pkt_ptr,
  boolean           integrity_protect,  /*! need integrity protection. */
  boolean           cipher, /*! need cipher. */
  lte_pdcp_sdu_type_e sdu_type /*! ELS or not */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_recfg_prep_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig prep request.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_recfg_prep_req
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  boolean           log_F3
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_ho_recfg_prep_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig prep request for HO.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_ho_recfg_prep_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_rlf_recfg_prep_req

===========================================================================*/
/*!
    @brief
    This function is called to process RB reconfig prep request for RLF.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_rlf_recfg_prep_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_send_control_pdu

===========================================================================*/
/*!
    @brief
    This function is called to send PDCP control msg generated by PDCP DL.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_send_control_pdu
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type*    pkt_ptr,
  boolean           status_report
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_peer_status_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the PDCP status report via PDCPDL from
    EnodeB.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_peer_status_ind
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type*    pkt_ptr
);


extern uint16 lte_pdcpul_encap_pdcp_hdr
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  dsm_item_type* pkt_ptr,
  uint16         *pkt_len,
  lte_pdcp_sdu_type_e sdu_type
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_update_data_stats

==============================================================================*/
/*!
    @brief
    This function update PDCP UL stats based on PDCP SDU sent OTA
    
    @detail
    This API will be called by RLC when PDCP SDU is dequeued from WM to 
    build RLC PDU
    
    @return
    none
    
    @note

    @see

*/
/*============================================================================*/
EXTERN void lte_pdcpul_update_data_stats
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  uint32 size
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_get_pdcp_hdr_cipher_info

==============================================================================*/
/*!
    @brief
 This API returns PDCP header info and ciphering params per
 packet

    @detail
This will be called by RLC while writing RLC PDU to A2 

    @return

*/
/*============================================================================*/
EXTERN uint8 lte_pdcpul_get_pdcp_hdr_cipher_info
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  dsm_item_type* sdu_dsm_ptr,
  uint8* pdcp_hdr,
  uint8* cipher_algorithm,
  uint8* cipher_key_index,
  uint32* count
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_calc_pdcp_hdr_size

==============================================================================*/
/*!
    @brief
  Calculate PDCP header size based on RB and PDU type

  @return

*/
/*============================================================================*/
EXTERN uint16 lte_pdcpul_calc_pdcp_hdr_size
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type* pkt_ptr
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_reg_non_empty_cb_func

==============================================================================*/
/*!
    @brief
    This API will register Non-empty callback function to PS-PDCP WM for DRB OR
    PDCP-RLC WM for SRB

    @return
    None
*/
/*============================================================================*/

EXTERN void lte_pdcpul_reg_non_empty_cb_func
(
  dsm_watermark_type* wm_ptr,
  lte_rb_cfg_idx_t rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_protocol_static_init

===========================================================================*/
/*!
  @brief
  one time Init PDCP UL protocol stack

  @return

*/
/*=========================================================================*/
extern void lte_pdcpul_protocol_static_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_protocol_dynamic_init

===========================================================================*/
/*!
  @brief
  dynamic Init PDCP UL protocol stack at start REQ to allocate memory resrouces

  @return

*/
/*=========================================================================*/
extern void lte_pdcpul_protocol_dynamic_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_protocol_dynamic_init

===========================================================================*/
/*!
  @brief
  PDCP UL protocol stack deinit at stop REQ to deallocate memory resrouces

  @return

*/
/*=========================================================================*/
extern void lte_pdcpul_protocol_deinit
(
  void
);
/*===========================================================================

  FUNCTION:  lte_pdcpul_process_reestablish_req

===========================================================================*/
/*!
    @brief
    This function is called to perform reestablish for active RBs
    the cfg reason can be RLF or HO

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_process_reestablish_req
(
  lte_l2_cfg_reason_e cfg_reason,
  boolean             no_ctrl_pdu,
  boolean rohc_reset
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_crypto_done_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the crypto done ind

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpul_process_crypto_done_ind
(
  lte_rb_cfg_idx_t    rb_cfg_idx,
  dsm_item_type*      dsm_ptr,
  uint8               mac_i[4]
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_cfg_completed_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the config completed ind from RRC

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpul_process_cfg_completed_ind
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_counter_check_req

===========================================================================*/
/*!
    @brief
    This function is called to process the counter check request from RRC

    @detail

    @return
    Number of active drbs

    @note

    @see
*/
/*=========================================================================*/
extern uint8 lte_pdcpul_process_counter_check_req
(
  lte_pdcp_count_info_s *drb_count_info_ptr
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_process_security_cfg

===========================================================================*/
/*!
    @brief
    This function is called to process security cfg from RRC.

    @detail

    @return
    TRUE if any parameter has changed.

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_process_security_cfg
(
  lte_pdcp_security_cfg_s *security_cfg_ptr
);


/*===========================================================================

  FUNCTION:  lte_pdcpul_get_cb_size

===========================================================================*/
/*!
    @brief
    This function is called to return the total control block size for PDCP UL.

    @detail

    @return
    PDCP UL protocol stack control block size

    @note

    @see

*/
/*=========================================================================*/
extern uint32 lte_pdcpul_get_cb_size
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_get_cb

===========================================================================*/
/*!
    @brief
    This function is called to return the control block ptr for PDCP UL.

    @detail

    @return
    PDCP UL protocol stack control block ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcpul_protocol_cb_s* lte_pdcpul_get_cb
(
  void
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_static_data_init

==============================================================================*/
/*!
    @brief
    This function is called to initialize the static structure lte_pdcpul_static_s.

    @detail
    This function sets the eps_info.rb_cfg_idx to LTE_INVALID_RB_CFG_IDX

    @return
    void

    @note

    @see

*/
/*============================================================================*/
extern void lte_pdcpul_static_data_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_get_rb_cb

===========================================================================*/
/*!
    @brief
    This function is called to return the RB control block ptr for PDCP UL.

    @detail

    @return
    PDCP UL RB block ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcpul_cb_s* lte_pdcpul_get_rb_cb
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_valid_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to valid RB reconfig request for add/modified/delete.

    @detail

    @return
    TRUE for success, FALSE for fail

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpul_valid_recfg_req
(
  lte_l2_cfg_reason_e cfg_reason,
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t released_rb_cfg_idx[], /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s add_modified_rb[] /*!< cfg action for add/modified RB ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_save_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to save RB reconfig request for add/delete

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_save_recfg_req
(
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t released_rb_cfg_idx[], /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s add_modified_rb[], /*!< cfg action for add/modified RB ID */
  lte_pdcpul_info_s* rb_cfg_base_ptr /*!< RB config base ptr */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_reset_saved_recfg_req

===========================================================================*/
/*!
    @brief
    This function is called to reset saved recfg req

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_reset_saved_recfg_req
(
  void
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_get_cb

==============================================================================*/
/*!
  @brief
    This function is called to return the Stattic data pointer PDCP UL.

    @detail

  @return
    PDCP UL protocol static data structure

    @note

    @see

*/
/*============================================================================*/
extern lte_pdcpul_static_s* lte_pdcpul_get_static_data_ptr
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_get_security_cfg

===========================================================================*/
/*!
    @brief
    This function is called to return the security cfg for PDCP UL.

    @detail

    @return
    Security Cfg Ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcp_security_cfg_s* lte_pdcpul_get_security_cfg
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_flush_log_stats

===========================================================================*/
/*!
    @brief
    This function is called to flush the statistics log for PDCP UL

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpul_flush_log_stats
(
  void
);

extern boolean lte_pdcpul_flow_enabled
(
  lte_pdcpul_flow_e      flow
);

extern boolean lte_pdcpul_flow_suspended
(
  lte_pdcpul_flow_e      flow
);

extern void lte_pdcpul_suspend_flow
(
  lte_pdcpul_cb_s*      cb_ptr
);

extern void lte_pdcpul_resume_flow
(
  lte_rb_cfg_idx_t      rb_cfg_idx /*!< rb_cfg_idx of control block */
);

extern void lte_pdcpul_suspend_flow_comp
(
  lte_pdcpul_cb_s*      cb_ptr
);

extern void lte_pdcpul_resume_flow_comp
(
  lte_rb_cfg_idx_t      rb_cfg_idx /*!< rb_cfg_idx of control block */
);

extern void lte_pdcpul_update_eps_2_cb_mapping
(
  lte_l2_cfg_reason_e   cfg_reason
);

extern dsm_watermark_type* lte_pdcp_offload_get_ps_pdcp_offload_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

extern void lte_pdcpul_conn_reestab_req_handler
(
  lte_pdcpul_conn_reestab_req_msg_s     *conn_reestab_req   /*!<Conn Reestab req msg */
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_process_rb_stats_req

==============================================================================*/
/*!
    @brief
    Returns the number of active DRB statistics copied into the statistics
    pointer passed by the calling function.

    @detail

    @return

*/
/*============================================================================*/
extern uint8 lte_pdcpul_process_rb_stats_req
(
  lte_pdcpul_rb_stats_req_msg_s* msg_ptr
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_start_log_timer

===========================================================================*/
/*!
    @brief
    This function will start the Log Flush timer.

    @detail

    @return
    None

    @note

    @see

*/
/*=========================================================================*/
EXTERN void lte_pdcpul_start_log_timer (void);

/*===========================================================================

  FUNCTION:  lte_pdcpul_stop_log_timer

===========================================================================*/
/*!
    @brief
    This function will Stop the Log Flush timer.

    @detail

    @return
    None

    @note

    @see

*/
/*=========================================================================*/
EXTERN void lte_pdcpul_stop_log_timer (void);

EXTERN void lte_pdcpul_process_free_pdu_ind
(
  dsm_item_type *pdu_ptr
);

EXTERN void lte_pdcpul_process_free_pkt_ind
(
  dsm_item_type *pdu_ptr
);

EXTERN void lte_pdcpul_qsh_analysis_routine
(
  qsh_action_e action,
  uint32 category_mask
);
EXTERN void lte_pdcpul_protocol_rohc_comp_done_ind
(
  lte_rb_cfg_idx_t rb_cfg_idx,
  lte_l2_cfg_reason_e cfg_reason
);

EXTERN void lte_pdcpul_send_offload_rab_reg_ind
(
  lte_rb_cfg_idx_t rb_cfg_idx,  /*!< RB config index */
  dsm_watermark_type* ps_to_pdcp_wm_ptr /*!< PS to PDCP WM ptr */
);
#ifdef FEATURE_LTE_UDC_ENABLED 
EXTERN void lte_pdcpul_send_comp_rab_reg_ind
(
  lte_rb_cfg_idx_t rb_cfg_idx,  /*!< RB config index */
  lte_pdcp_cfg_act_e cfg_act, /*!< cfg_act */
  dsm_watermark_type* ps_to_pdcp_wm_ptr /*!< PS to PDCP WM ptr */
);
#endif /*FEATURE_LTE_UDC_ENABLED */
EXTERN inline uint32 lte_pdcpul_get_count
(
  const dsm_item_type* pkt_ptr /*!< packet for  which count is required*/
);


/*===========================================================================
FUNCTION lte_pdcpul_check_overwrite_discard_timer()

DESCRIPTION
 This routine is used to check whether we need to overwrite the discard timer 
 config for a RB

DEPENDENCIES
 None

PARAMETERS

RETURN VALUE
 boolean

SIDE EFFECTS
 None
===========================================================================*/
EXTERN boolean lte_pdcpul_check_overwrite_discard_timer
(
  lte_pdcpul_cb_s* rb_cb_ptr
);

/*===========================================================================
FUNCTION lte_pdcpul_overwrite_discard_timer_cfg()

DESCRIPTION
 This routine is used to overwrite the discard timer config

DEPENDENCIES
 None

PARAMETERS

RETURN VALUE
 None

SIDE EFFECTS
 None
===========================================================================*/
EXTERN void lte_pdcpul_overwrite_discard_timer_cfg
(
  ds_3gpp_bearer_qos_info_msg_s* qos_info
);
#endif /* LTE_PDCP_UL_PROTOCOL_H */

