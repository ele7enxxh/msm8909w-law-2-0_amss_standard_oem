/*!
  @file
  lte_pdcp_dl_protocol.h

  @brief
  This file contains defines and functions interfaces for PDCP DL protocol stack.

  @author
  bqiu, gxiao

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_dl_protocol.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/16   sb      CR996691: Buffer DL packets while IPA Wm is not registered with PDCP
12/24/15   sb      CR938646: Reduce frequency of F3s without impacting debugging
08/04/15   sb      CR883240: Remove unused feature and code related to FC
01/14/15   ru      CR775616: Fix for PDCPDL Ciph Buf Corruption
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
08/05/14   sb      CR696130: Reset Ciphering parameter in OFFLOAD when security 
                   parameter changes
04/11/14   mg      CR647594: QSH Phase 1
06/27/14   sb      CR629741: Change in LTE to migrate to CFCM
06/12/14   sb      CR656696: KW fixes
05/08/14   mg      CR 500110: Klockwork Fixes
04/03/14   sb      CR626708: PDCP DL reordering buffer optimization
04/02/14   sb      CR642230: REL11 RoHC context continuity during HO 
                   within same eNB
09/04/13   sb      Added initialization for SFN and SubFN reported to RRC 
                   in LTE_PDCPDL_SDU_IND. Also log SFN info for any DL PDCP
                   PDU received
10/13/13   mg      CR 561786: Downlink PDCP changes for BOLT.
07/11/13   ax      CR509957: fix crash in lte_pdcpdl_log_statistics
06/30/13   sb      CR505750: Logging issue for DL SRB packets
05/13/13   sb      CR484633: Avoid crash because of PDCPDL message Q full with 
                   CRYPTO_DONE_IND message
05/08/13   sb      CR466844: Remove assert and free the SDU if LTE_PDCPDL_SDU_IND
                   message sending fails to RRC
04/05/13   sb      CR457099: PDCP DL logging timer optimization
11/20/12   ax      CR383618: L2 modem heap use reduction phase I  
10/21/12   sb      CR408815: Handle case when there are frequent large SRB packets
05/21/12   sb      CR363363: Compiler warning fixes
01/10/12   ax      CR328903: set feedback_for for decompressor 
09/06/11   ax      add support for piggyback RoHC feedback
06/15/11   ax      RoHC support
03/06/11   ax      add support for release 9 PDCP full configuration
02/23/11   ax      add support for SMC with invalid MAC-I followed by valid SMC
01/31/11   ax      add support for pdcp dl flow control
09/22/10   ax      fixed CR#256772: pdcp configuration with security timeout
08/25/10   ar      save cfg for RECFG and HO cases and clean it after
                   LTE_RRC_CONFIG_COMPLETED_IND
05/19/10   ar      added PDCP ciphering log packet support
04/26/10   ax      Added support for API to query the DL DRB activities
01/30/10   ax      notify A2 for X number of pkt enqueued to the watermark
11/09/09   ax      relocated num_rst in statistics
09/10/09   bq      RLF reestablish fix
08/27/09   bq      Security Corner case fix
08/24/09   bq      Move DL DNE
06/17/09   bq      security implementation
02/17/09   bq      fix cfg validation
07/18/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_DL_PROTOCOL_H
#define LTE_PDCP_DL_PROTOCOL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include "ps_rohc.h"
#include "lte_l2_comdef.h"
#include "mutils_circ_q.h"
#include "lte_pdcp_msg.h"
#include "lte_pdcp_offload_msg.h"
#include "lte_pdcp_offload_protocol.h"
#include "lte_pdcp_dl_if.h"
#include "lte_pdcp_ul_if.h"
#include "lte_pdcpi.h"
#include "lte_fc.h"
#include "qsh.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* MAX PDCP status report buffer */
#define LTE_PDCPDL_STATUS_REPORT_BYTE_SIZE 64

/*! @brief max pkt length supported by SW decipher for RoHC primarily
*/
#define LTE_PDCP_SW_CIPHER_MAX_MSG_LEN                               2048

/*! @brief PDCP RB DL state
*/
typedef enum
{
  LTE_PDCPDL_IDLE = 0,
  LTE_PDCPDL_ACTIVE = 1,
/* DL SRB waiting for Security Setup Done from RRC. */
  LTE_PDCPDL_SECURITY_PENDING = 2
} lte_pdcpdl_state_e;

/*! @brief PDCP RB DL flow state
*/
typedef enum
{
  LTE_PDCPDL_FLOW_ENABLE = 0,
  LTE_PDCPDL_FLOW_DISABLE = 1
} lte_pdcpdl_flow_e;


/*enum to specify the PDCPDL Protocol state in which accordingly which
callback function registered with IPA will be handled */
typedef enum
{
  LTE_PDCPDL_DEFAULT_STATE_HFN,
  LTE_PDCPDL_STATE_INC_HFN,
  LTE_PDCPDL_STATE_DEC_HFN,
}lte_pdcpdl_protocol_hfn_state_e;


/*! @brief PDCP RB DL cfg parameter control block
*/
typedef lte_pdcpdl_info_s lte_pdcpdl_cfg_s;

/*! @brief PDCP RB DL state control block
*/
typedef struct
{
  lte_pdcpdl_state_e state;   /*!< PDCP state */
  lte_pdcpdl_flow_e  flow;    /*!< PDCP flow mask */

  uint32             rx_hfn;  /*!< PDCP RX HFN */
  uint32             rx_seq;  /*!< PDCP RX seq */
  uint32             last_submitted_rx_seq; /*!< PDCP Last Submitted RX seq */

  uint32             seq_modulus; /*!< PDCP seq modulus */
  uint32             seq_mask; /*!< PDCP seq mask */
  uint32             reordering_window; /*!< PDCP reordering window */

  q_type             reorder_q;   /*!< reordering queue*/

  /*!< SRB queue header wait for security config done from RRC. */
  dsm_item_type      *pending_security_srb_head_ptr;

  /*!< SRB queue tail wait for security config done from RRC.  */
  dsm_item_type      *pending_security_srb_tail_ptr;

  lte_pdcpdl_protocol_hfn_state_e hfn_state;

} lte_pdcpdl_state_s;

/*! @brief PDCP statistics of the RB
*/
typedef struct
{
  uint32  num_rst;     /*!< number of reestablishments since activated */
  lte_pdcpdl_stats_set_s  curr_stats;  /*!< statistics of the RB since last reestablishment/activation*/
  lte_pdcpdl_stats_set_s  stats_upto_rst; /*!< accumulation upto the last est*/
  uint16 num_dl_pdus_after_hfn_update;
  uint16 num_pdus_ipa_wm;
  uint16 num_cb_trigger; /*Absolute number of callback trigger from IPA*/
  uint16 num_probable_cb_trigger; /*Probable number of callback trigger from IPA that resulted in action by PDCP*/
} lte_pdcpdl_stats_s;

/*! @brief PDCP RB DL control block
*/
typedef struct
{
  lte_pdcpdl_cfg_s                  cfg;      /*!< config */
  lte_pdcpdl_state_s                state;    /*!< state  */
  lte_pdcpdl_stats_s                stats;    /*!< stats  */
} lte_pdcpdl_cb_s;

/*! @brief PDCP AM DRB reordering queue
*/
typedef struct
{
  /* PDCP DL DRB reordering queue resources (each element contain dsm ptr) */
  dsm_item_type*      reordering_queue[1<<LTE_PDCP_DRB_AM_SEQ_LEN];
} lte_pdcpdl_drb_reordering_queue_s;

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
  lte_pdcpdl_info_s                   *rb_cfg_base_ptr;

} lte_pdcpdl_saved_cfg_req_s;

/*! @brief PDCP DL protocol stack control block
*/
typedef struct
{
  lte_pdcpdl_cb_s     *rb_cb[LTE_MAX_RB_CFG_IDX];    /*!< RB control block array */
  lte_pdcpdl_cb_s*    eps_id_2_rb_cb_ptr[LTE_MAX_EPS]; /*!< eps ID to control
                                                       block mapping */
  uint32              num_enqueue_wo_notify;        /*!< num pkt without notify*/
  mutils_circ_q_s*    rlc_pdcp_cir_q_ptr;           /*!< RLC PDCP cirular queue */

  /*!< security config */
  lte_pdcp_security_cfg_s security_cfg;

   /* Saved config request during HO. */
  lte_pdcpdl_saved_cfg_req_s save_cfg_req;

  boolean                 security_valid;        /*!< security configured */

  uint32                  num_error;   /*!< number of PDCP DL errors */

  uint8                   num_active_rb; /*!<Num of Active RBs*/

  uint32                  num_rrc_sdu_fail; /*!< num of times LTE_PDCPDL_SDU_IND msg sending fails*/
} lte_pdcpdl_protocol_cb_s;

/*! @brief state of the flow in terms of pkt drop rate
*/
typedef enum
{
  LTE_PDCPDL_FLOW_NO_DROP = 0,   /*!< top state: no packet drop */
  LTE_PDCPDL_FLOW_UP,            /*!< stepping up via timer */
  LTE_PDCPDL_FLOW_DOWN,          /*!< stepping down via timer */
  LTE_PDCPDL_FLOW_MIN,           /*!< hit bottom: worse drop rate */
  LTE_PDCPDL_FLOW_MAX            /*!< Max */
} lte_pdcpdl_flow_state_e;

typedef struct
{
  dsm_watermark_type  *pdcp_wm_ptr;    /*!<PDCP-IPA WM ptr */
  dsm_watermark_type  pdcp_wm;         /*!< Temp PDCP Static WM */
  q_type              pdcp_wm_q;
  uint32              target_rate_idx; /*!< index to the drop rate */
  uint32              pkt_counter;     /*!< number of pkt since last drop */
  boolean             wm_hi;           /*!< wm hits persistent high  */
  boolean             a2_registered;   /*!< wm is registered with A2 dl per?  */
  lte_pdcpdl_flow_state_e state;       /*!< state of the flow */
  uint32              num_drop_sdu;    /*!< total number of dropped PDCP SDU */
  uint32              num_drop_bytes;  /*!< total dropped PDCP SDU in bytes */
  uint32              num_wm_hi;       /*!< number of times wm high hits */
  uint32              num_empty_wm_sdu;/*!< number of sdus dropped because of empty 
                                                                            watermark pointer*/ 
} lte_pdcpdl_flow_s;

/*! @brief watermarks have issue with deallocation, so let us not do that
*/
typedef struct
{
  lte_pdcpdl_flow_s pdcp_flow[LTE_MAX_EPS];/*!< PDCP to A2 flow data */
  boolean           dsm_low;               /*!< dsm pool low */
  lte_fc_cfg_s      target_drop_rate;      /*!< info on target drop rate */
  uint32            min_rate;              /*!< lowest rate */
  uint32            drop_pkt_sz_threshold; /*!< pkt sz bigger than this might 
                                                be considered for dropping */
  uint32            num_dsm_low;           /*!< number of times dsm low hits */
                                                
 /*when RB is not activated/added, this dummy Ctrl Blk to be pointed to for
  better performance: no check of NULL pointer is needed*/
  lte_pdcpdl_cb_s           lte_pdcpdl_dummy_rb_cb;

  boolean                 enable_hfn_resync_feature; /*boolean to indicate if HFNresync feature is enabled*/
} lte_pdcpdl_protocol_static_data_s;

typedef union
{
  lte_sfn_s sfn;
  uint16  value;
}lte_pdcp_sfn_info_u;
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_data_ind

===========================================================================*/
/*!
    @brief
    This function is called to process the data indication from RLCDL

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpdl_process_data_ind
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_reestablish_req

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
extern void lte_pdcpdl_process_reestablish_req
(
  lte_l2_cfg_reason_e cfg_reason,
  boolean rohc_reset
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_security_cfg

===========================================================================*/
/*!
    @brief
    This function is called to process security cfg from RRC.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpdl_process_security_cfg
(
  lte_pdcp_security_cfg_s *security_cfg_ptr
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_recfg_req

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
extern boolean lte_pdcpdl_process_recfg_req
(
  lte_l2_cfg_reason_e cfg_reason,
  lte_pdcp_cfg_act_e  cfg_act,
  lte_rb_cfg_idx_t    rb_cfg_idx,
  lte_pdcpdl_info_s* cfg_info_ptr,
  boolean            log_F3
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_connection_release

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
extern void lte_pdcpdl_process_connection_release
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_cfg_completed_ind

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
extern void lte_pdcpdl_process_cfg_completed_ind
(
  void
);


/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_recfg_prep_req

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
extern void lte_pdcpdl_process_recfg_prep_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_valid_recfg_req

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
extern boolean lte_pdcpdl_valid_recfg_req
(
  lte_l2_cfg_reason_e cfg_reason,
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t released_rb_cfg_idx[], /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s add_modified_rb[] /*!< cfg action for add/modified RB ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_ho_save_recfg_req

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
extern void lte_pdcpdl_save_recfg_req
(
  uint8   num_released_rb, /*!< number of released RB */
  lte_rb_cfg_idx_t released_rb_cfg_idx[], /*!< released RB cfg index */
  uint8   num_add_modified_rb, /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s add_modified_rb[], /*!< cfg action for add/modified RB ID */
  lte_pdcpdl_info_s* rb_cfg_base_ptr /*!< RB config base ptr */
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_reset_saved_recfg_req

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
extern void lte_pdcpdl_reset_saved_recfg_req
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_rab_register_req

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

extern boolean lte_pdcpdl_process_rab_register_req
(
  lte_eps_id_t  eps_id,      /*!< EPS ID */
  dsm_watermark_type  *pdcp_wm_ptr /*!< PS to PDCP WM ptr */
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_rab_deregister_req

===========================================================================*/
/*!
    @brief
    This function is called to process RAB deregister request from PS.

    @detail

    @return
    TRUE for success, FALSE for fail

    @note

    @see

*/
/*=========================================================================*/
extern boolean lte_pdcpdl_process_rab_deregister_req
(
  lte_eps_id_t  eps_id      /*!< EPS ID */
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_protocol_static_init

===========================================================================*/
/*!
  @brief
  static init PDCP DL protocol stack that is supposed to be called only
  once during task bringup

  @return
  None

*/
/*=========================================================================*/
extern void lte_pdcpdl_protocol_static_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_protocol_dynamic_init

===========================================================================*/
/*!
  @brief
  Dynamic init PDCP DL protocol stack that is called everytime start REQ is
  received

  @return
  None

*/
/*=========================================================================*/
extern void lte_pdcpdl_protocol_dynamic_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_protocol_deinit

===========================================================================*/
/*!
  @brief
  Performs the resource release everytime stop REQ is received

  @return
  None

*/
/*=========================================================================*/
extern void lte_pdcpdl_protocol_deinit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_pdcp_a2_wm

===========================================================================*/
/*!
    @brief
    This function is called to get the PDCP A2 WM maintained by PDCP.

    @detail

    @return
    PDCP A2 WM ptr

    @note

    @see

*/
/*=========================================================================*/
dsm_watermark_type* lte_pdcpdl_get_pdcp_a2_wm
(
  lte_eps_id_t eps_id
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_flush_log

===========================================================================*/
/*!
    @brief
    This function is called to flush the log for PDCP DL

    @detail

    @return

    @note

    @see
*/
/*=========================================================================*/
extern void lte_pdcpdl_flush_log
(
  boolean flush_all
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_cb_size

===========================================================================*/
/*!
    @brief
    This function is called to return the total control block size for PDCP DL.

    @detail

    @return
    PDCP DL protocol stack control block size

    @note

    @see

*/
/*=========================================================================*/
extern uint32 lte_pdcpdl_get_cb_size
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_cb

===========================================================================*/
/*!
    @brief
    This function is called to return the control block ptr for PDCP DL.

    @detail

    @return
    PDCP DL protocol stack control block ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcpdl_protocol_cb_s* lte_pdcpdl_get_cb
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_rb_cb

===========================================================================*/
/*!
    @brief
    This function is called to return the RB control block ptr for PDCP DL.

    @detail

    @return
    PDCP DL RB block ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcpdl_cb_s* lte_pdcpdl_get_rb_cb
(
  lte_rb_cfg_idx_t    rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_get_security_cfg

===========================================================================*/
/*!
    @brief
    This function is called to return the security cfg for PDCP DL.

    @detail

    @return
    Security Cfg Ptr

    @note

    @see

*/
/*=========================================================================*/
extern lte_pdcp_security_cfg_s* lte_pdcpdl_get_security_cfg
(
  void
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_process_counter_check_req

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
extern uint8 lte_pdcpdl_process_counter_check_req
(
  lte_pdcp_count_info_s *drb_count_info_ptr
);

extern lte_pdcpdl_cb_s* lte_pdcpdl_get_rb_cb_by_eps_id
(
  lte_eps_id_t                  eps_id /*!< eps ID */
);

extern void lte_pdcpdl_wm_low_handler
(
  lte_pdcpdl_wm_low_ind_msg_s*    msg_ptr  /*!< watermark low message ptr */
);

extern void lte_pdcpdl_fc_req_handler
(
  cfcm_cmd_msg_type_s*        fc_msg_ptr /*!< flow control message ptr */
);

extern uint32 lte_pdcpdl_get_num_dropped_pkt
(
  lte_eps_id_t                  eps_id /*!< eps ID */
);

extern uint32 lte_pdcpdl_get_num_dropped_pkt_bytes
(
  lte_eps_id_t                  eps_id /*!< eps ID */
);

extern void lte_pdcpdl_sec_reset_req_handler( void );

extern void lte_pdcpdl_update_eps_2_cb_mapping( void );

extern lte_pdcpdl_protocol_static_data_s* lte_pdcpdl_get_static_data( void );

extern void lte_pdcpdl_rohc_fb_handler
(
  dsm_item_type **fb_pkt,      /*!< pointer to the fb dsm pointer */
  void           *fb_cookie,   /*!< fb cookie */
  uint16          fb_cid       /*!< feedback context ID */
);

extern void lte_pdcpdl_hfn_update_req_handler( lte_pdcpdl_hfn_update_req_msg_s* );

extern void lte_pdcpdl_hfn_state_reset_req_handler( lte_pdcpdl_hfn_state_reset_req_msg_s* );


/*==============================================================================

  FUNCTION:  lte_pdcpdl_init_circ_q_handle

==============================================================================*/
/*!
    @brief
    initialize the PDCP PDU circular q.

    @detail
    This API is exposed for testing purpose only where PDCP OFFLOAD might want to 
    execute PDCP OFFLOAD related test without bringing up PDCPDL module.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpdl_init_circ_q_handle( void );
EXTERN uint32 lte_pdcpdl_get_q_full_cnt( void );
EXTERN uint32 lte_pdcpdl_get_pkt_drop_q_full_cnt( void );
EXTERN void lte_pdcpdl_data_srb_ind_send_msg(void);


/*===========================================================================

  FUNCTION:  lte_pdcpdl_start_log_timer

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
EXTERN void lte_pdcpdl_start_log_timer (void);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_stop_log_timer

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
EXTERN void lte_pdcpdl_stop_log_timer (void);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_num_pdus_rx_reordering_queue

===========================================================================*/
/*!
    @brief
    This function is called to check number of PDUs in reordering queue

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/

EXTERN uint32 lte_pdcpdl_num_pdus_rx_reordering_queue
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN void lte_pdcpdl_qsh_analysis_routine 
(  
  qsh_action_e action,
  uint32 category_mask
);

#endif /* LTE_PDCP_DL_PROTOCOL_H */

