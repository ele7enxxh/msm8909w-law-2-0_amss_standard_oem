/*!
  @file
  lte_pdcp_offlaod_protocol.h

  @brief
  This file contains defines and functions interfaces for PODCP OFFLOAD protocol stack.

  @author
  sbathwal

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

                        EDIT HISTORY FOR MODDLE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_offload_protocol.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/16   sb      CR996691: Buffer DL packets while IPA Wm is not registered with PDCP
07/23/15   sb      CR867973: Enhance logging and QSH for UDC
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
10/23/14   sb      CR744770: For JO use A2 api for SW decipher
09/25/14   sb      CR716955: Code feauterization change to care of Jolokia/Tabasco
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
09/04/14   sb      CR694693: Fix to ensure UL PDUs are not built after RAB
                   is released
08/05/14   sb      CR696130: Reset Ciphering parameter in OFFLOAD when security 
                   parameter changes
08/01/14   sb      CR701729: Remove CFM header file
03/22/14   sb      CR636070: New API to reset RoHC stats
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
10/13/13   mg      CR 561786: Downlink PDCP changes for BOLT.
02/21/13   sb      CR454011: RoHC Feedback comes as PDCP Data PDU instead of 
                   normal Control PDU
12/19/12   sb      CR431445: Have common A2 watermark for both PDCP and OFFLOAD tasks. Remove 
                   OFFLOAD-A2 warermark and use only PDCP-A2 watermark
06/16/12   sb      CR365196 and CR365004: Fix to handle scenarios 
                   where RB switches back and forth with RoHC enabled and 
                   disabled. Also to reduce the delay in UL after HO
05/21/12   sb      CR363363: Compiler warning fixes
04/22/12   sb      Initial checkin
===========================================================================*/

#ifndef LTE_PDCP_OFFLOAD_PROTOCOL_H
#define LTE_PDCP_OFFLOAD_PROTOCOL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include "ps_rohc.h"
#include "lte_l2_comdef.h"
#include "mutils_circ_q.h"
#include "lte_pdcp_offload_msg.h"
#include "lte_pdcp_dl_if.h"
#include "lte_pdcp_ul_if.h"
#include "lte_pdcpi.h"
#include "lte_fc.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief max pkt length supported by SW decipher for PDCP OFFLOAD primarily
*/
#define LTE_PDCPOFFLOAD_SW_CIPHER_MAX_MSG_LEN                   2048
#define RECOMPRESS_FAILED    0
#define RECOMPRESS_PASSED    1

/*! @brief ROHC RB state for Compressor
*/
typedef enum
{
  LTE_PDCP_OFFLOAD_COMP_IDLE = 0,
  LTE_PDCP_OFFLOAD_COMP_ACTIVE = 2,  
} lte_pdcp_offlaod_comp_state_e;

/*! @brief ROHC RB state for DeCompressor
*/
typedef enum
{
  LTE_PDCP_OFFLOAD_DECOMP_IDLE = 1,
  LTE_PDCP_OFFLOAD_DECOMP_ACTIVE = 4,
} lte_pdcp_offlaod_decomp_state_e;

/*! @brief PDCP OFFLOAD FLOW state
*/
typedef enum
{
  LTE_PDCPOFFLOAD_FLOW_ENABLE = 0,  /* enabled */
  LTE_PDCPOFFLOAD_FLOW_DISABLE= 1, /* disable */
} lte_pdcp_offload_comp_flow_e;

/*! @brief pdcp offload to A2 flow related data
*/
typedef struct
{
  dsm_watermark_type*  pdcp_a2_wm;      /*!< pointer to PDCP A2 watermark */
} lte_pdcp_offload_flow_s;

/*! @brief PDCP OFFLOAD parameter control block for compressor
*/
typedef lte_pdcp_offload_comp_info_s lte_pdcp_offload_comp_cfg_s;

/*! @brief PDCP OFFLOAD parameter control block for Decompressor
*/
typedef lte_pdcp_offload_decomp_info_s lte_pdcp_offlaod_decomp_cfg_s;

typedef enum
{
  LTE_PDCP_OFFLOAD_HFN_MISMATCH_STATE_DEFAULT,
  LTE_PDCP_OFFLOAD_HFN_MISMATCH_STATE_INC,
  LTE_PDCP_OFFLOAD_HFN_MISMATCH_STATE_DEC
}lte_pdcp_offload_hfn_state_e;


/*! @brief PDCP OFFLOAD PDCP WM
*/
typedef struct
{
  dsm_watermark_type  wm_item; /*!< PDCP RLC tx watermark */
  q_type              wm_q;       /*!< PDCP RLC tx queue */
} lte_pdcp_offload_pdcp_wm_s;

/*! @brief PDCP OFFLOAD control block for UL Data
*/
typedef struct
{
  lte_pdcp_offlaod_comp_state_e state;   /*!< PDCP OFFLOAD decomp state */
  volatile lte_pdcp_offload_comp_flow_e flow;

  dsm_watermark_type  *ps_to_pdcp_offlaod_wm_ptr;
  lte_pdcp_offload_pdcp_wm_s   pdcp_offlaod_to_pdcp_wm;

  /*!< RoHC handle if configured  */
  rohc_handle_type   rohc_handle;

} lte_pdcp_offload_comp_state_s;

/*! @brief PDCP OFFLOAD control block for DL Data
*/
typedef struct
{
  lte_pdcp_offlaod_decomp_state_e state;   /*!< PDCP OFFLOAD decomp state */

  /*!< RoHC handle if configured  */
  rohc_handle_type   rohc_handle;

  lte_pdcp_offload_hfn_state_e hfn_state;

  boolean hfn_update_pend;

} lte_pdcp_offload_decomp_state_s;

/*! @brief PDCP OFFLOAD statistics of the RB for compressor
*/
typedef struct
{
  uint32  num_rohc_fail;  /*!< Number of ROHC compression failed*/
  uint32  num_piggyback_fb; /*!< Number of piggyback ROHC feedback rxed*/
  uint32  num_recompression; /*!< Num of times RoHC bearer was recompressed*/
  uint32  num_cb_trigger;/*!< Num of times HFN desync callback was triggered*/
  uint32  data_bytes_from_ps; /*!< total bytes dequeued from PS Wm for RoHC compression*/
} lte_pdcp_offload_comp_stats_s;

/*! @brief PDCP OFFLOAD statistics of the RB for Decompressor
*/
typedef struct
{
  uint32  num_rohc_fail;  /*!< Number of ROHC compression failed*/
  uint32  num_fail_after_hfn_update; /*!< Number of ROHC decompression failed
                                                                 after last HFN update*/
  uint16  num_cb_trigger; /*Absolute number of trigger from RoHC*/
  uint16  num_probable_cb_trigger; /**num of trigger that resulted in OFFLOAD sending PDCP an indication*/
} lte_pdcp_offload_decomp_stats_s;



/*! @brief PDCP OFFLOAD RB control block for UL Data
*/
typedef struct
{
  lte_pdcp_offload_comp_cfg_s                  cfg;      /*!< config */
  lte_pdcp_offload_comp_state_s                state;    /*!< state  */
  lte_pdcp_offload_comp_stats_s                stats;    /*!< stats  */
} lte_pdcp_offload_comp_cb_s;


/*! @brief buffers needed for sec API to perform SW decipher
*/
typedef struct
{
  uint8* plain_buf_ptr;     /*!< plain text buffer */
  uint8* ciphered_buf_ptr;  /*!< ciphered buffer */
  uint16 buf_size;          /*!< buffer size */
  uint16 last_ciph_value;
  lte_cipher_algo_e ciph_algo;
  uint8 ciph_keys[LTE_CIPHER_INTEGRITY_KEY_LEN];
} lte_pdcp_offload_decomp_cipher_buf_s;


/*! @brief PDCP OFFLOAD RB control block for DL Data
*/
typedef struct
{
  lte_pdcp_offlaod_decomp_cfg_s                  cfg;      /*!< config */
  lte_pdcp_offload_decomp_state_s                state;    /*!< state  */
  lte_pdcp_offload_decomp_stats_s                stats;    /*!< stats  */
} lte_pdcp_offload_decomp_cb_s;

typedef struct
{
  dsm_watermark_type  *ps_to_pdcp_offlaod_wm_ptr;
}lte_pdcp_offload_comp_eps_info_s;

typedef struct
{
  lte_pdcp_offload_flow_s   pdcp_offload_flow;
}lte_pdcp_offload_decomp_eps_info_s;

/*! @brief PDCP OFFLOAD protocol stack control block for UL
*/
typedef struct
{
  lte_pdcp_offload_comp_cb_s     rb_cb[LTE_MAX_RB_CFG_IDX];    /*!< RB control block array */
  
  lte_pdcp_offload_comp_eps_info_s   eps_info[LTE_MAX_EPS]; /*!< EPS related information */
  
  uint32                  num_error;   /*!< number of ROHC errors */

} lte_pdcp_offload_comp_protocol_cb_s;

typedef struct
{
  lte_pdcp_offload_decomp_cb_s     rb_cb[LTE_MAX_RB_CFG_IDX];    /*!< RB control block array */
  lte_pdcp_offload_decomp_eps_info_s   eps_info[LTE_MAX_EPS]; /*!< EPS related information */
  mutils_circ_q_s*     pdcp_offload_cir_q_ptr; /*PDCPDL and PDCP OFFLOAD circular queue*/
  uint32                  num_error;   /*!< number of ROHC errors */
  uint32                  num_ciph_param_change;  /*number of times Ciph parameter changed by OFFLOAD*/
  lte_pdcp_offload_decomp_cipher_buf_s      ciph_buf;  /*ciphering buffer */

} lte_pdcp_offload_decomp_protocol_cb_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  lte_pdcp_offlaod_get_ul_cb_size

===========================================================================*/
/*!
    @brief
    This function is called to return the total control block size for PDCP OFFLOAD for UL Data.

    @detail

    @return
    ROHC protocol stack UL control block size

    @note

    @see

*/
/*=========================================================================*/
extern uint32 lte_pdcp_offlaod_get_ul_cb_size
(
  void
);

extern void lte_pdcp_offload_process_peer_rohc_fb_ind
(
  lte_rb_cfg_idx_t  rb_cfg_idx,
  dsm_item_type*    pkt_ptr
);

extern void lte_pdcp_offload_wm_enqueue_handler
(
  lte_rb_cfg_idx_t	rb_cfg_idx
);

extern void lte_pdcp_offload_protocol_static_init(void);

extern void lte_pdcp_offload_comp_cfg_req_handler
(
  lte_pdcpul_comp_cfg_req_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_decomp_cfg_req_handler
(
  lte_pdcpdl_decomp_cfg_req_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_a2_rab_deregister_req_handler
(
  lte_pdcpdl_a2_rab_deregister_req_msg_s *msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_ul_rab_dereg_ind_handler
(
  lte_pdcp_offload_rab_dereg_ind_msg_s *msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_ul_rab_reg_ind_handler
(
  lte_pdcp_offload_rab_reg_ind_msg_s *msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_decomp_pkt_req_handler
(
  lte_pdcpdl_decomp_pkt_req_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_offload_recomp_pkt_req_handler
(
  lte_pdcpul_comp_pkt_req_msg_s* msg_ptr /*!< input message pointer */
);

extern lte_pdcp_offload_decomp_protocol_cb_s* lte_pdcp_offload_get_decomp_cb
(
  void
);

extern lte_pdcp_offload_comp_protocol_cb_s* lte_pdcp_offload_get_comp_cb
(
  void
);

extern lte_pdcp_offload_comp_cb_s* lte_pdcp_offload_get_comp_rb_cb
(
  lte_rb_cfg_idx_t    rb_cfg_idx
);

extern dsm_watermark_type* lte_pdcp_offload_get_pdcp_a2_wm
(
  lte_eps_id_t eps_id
);

extern void lte_pdcp_offload_ciph_reset_req_handler
(
  void
);

extern void lte_pdcp_offload_hfn_state_reset_req_handler
(
  lte_pdcp_offload_reset_hfn_state_req_msg_s* msg_ptr
);

/*==============================================================================

  FUNCTION:  lte_pdcp_offload_hfn_cnf_handler

==============================================================================*/
/*!
    @brief
    This handles HFN cnf sent by PDCP after HFN is updated.

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcp_offload_hfn_cnf_handler
(
  lte_pdcpdl_hfn_update_cnf_msg_s *hfn_cnf
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_process_peer_piggyback_rohc_fb_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the PDCP RoHC feedback received as part
    of the regular RoHC packets.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcp_offload_process_peer_piggyback_rohc_fb_ind
(
  lte_pdcpdl_peer_pb_rohc_fb_ind_msg_s* pb_rohc_fb_ind
);

/*==============================================================================

  FUNCTION:  lte_pdcp_offload_get_pdcp_offload_pdcp_wm

==============================================================================*/
/*!
    @brief
    This function is called to get the PDCP OFFLOAD PDCP WM maintained by PDCP OFFLOAD.

    @detail

    @return
    PDCP OFFLOAD PDCP WM ptr

    @note

    @see

*/
/*============================================================================*/
extern dsm_watermark_type* lte_pdcp_offload_get_pdcp_offload_pdcp_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

extern dsm_watermark_type* lte_pdcp_offload_get_ps_pdcp_offload_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

EXTERN uint32 lte_pdcp_offload_num_comp_fail
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN uint32 lte_pdcp_offload_num_bytes_from_ps
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN uint32 lte_pdcp_offload_num_decomp_fail
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN void lte_pdcp_offload_reset_comp_stats
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN void lte_pdcp_offload_reset_decomp_stats
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_send_wm_low_ind

===========================================================================*/
/*!
  @brief
  notify PDCP OFFLOAD that low watermark has been crossed on disabled RB

  @return
  None
*/
/*=========================================================================*/
extern void lte_pdcp_offload_send_wm_low_ind
(
  uint32  rb_cfg_idx    /*!< rb_cfg_idx for the RB */
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_ciph_buf_init

===========================================================================*/
/*!
    @brief
    This function initializes dynamic cipher and plain buffer.

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcp_offload_ciph_buf_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_ciph_buf_deinit

===========================================================================*/
/*!
    @brief
    This function de-initializes dynamic cipher and plain buffer.

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcp_offload_ciph_buf_deinit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcp_offload_get_ciph_buf_size

===========================================================================*/
/*!
    @brief
    This function returns dynamic cipher and plain buffer size.

    @return
    None
*/
/*============================================================================*/
extern uint16 lte_pdcp_offload_get_ciph_buf_size
(
  void
);

extern void lte_pdcp_offload_to_pdcp_hi_water_cb
(
  dsm_watermark_type *wm_ptr,
  void*              callback_data
);
#endif /* LTE_PDCP_OFFLOAD_PROTOCOL_H */

