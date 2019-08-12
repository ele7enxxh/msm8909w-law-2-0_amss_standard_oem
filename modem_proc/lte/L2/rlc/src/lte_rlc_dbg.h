/*!
  @file
  lte_rlc_dbg.h

  @brief
  RLC debug related utilities api.

  @author
  axiao

*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlc_dbg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/04/16   cx      CR938964: Reduce dup retx caused by fast nak
01/31/16   cx      CR923654: Fast nak optimization
04/15/15   ru      CR816655: RLC mitigation to prevent RLF due to FC Shutdown
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
04/30/15   mg      CR832561: Adaptive b factor scaling
04/23/15   mg      CR824175:  Video threshold size determination as function of 
                   FEC and segment duration for DPD calculations
02/05/15   mg      CR788317: Invalid MSI and 0xB08C logging changes
01/27/15   mg      CR786757: EMBMS DPD not kicking in on service startup
01/23/15   ru      CR785611: Dont force flush SRBs during AM2UM delivery
12/23/14   sb      CR772190: Extend A2 diag framework for L2 
11/23/14   sb      CR745901: Turn AM to UM mode and do best effort deliver
                   when new Flow contrl level is triggered.
12/18/14   mg      CR 768623: Embms DSDS DPD feature - Phase 1
01/23/14   mg      CR599494: RLC behavior to alleviate the low DSM count problem.
01/09/11   ax      CR326734: remove efs_get() from lte_pdcpul_log_init(), which 
                   is part of start procedure due to unpredictalbe delays
10/17/11   ax      make RLC-A2 eMBMS watermark level runtime configurable
06/26/10   ax      make AM receive window size configurable in flow control
03/26/10   ax      added lte_rlc_dbg_get_max_pending_rlcdl_sdu() api
03/18/10   ax      make profile log pkt frequency configurable
07/08/09   ax      make starting segment offset configurable for DEC08 spec
06/08/09   ax      added PDCPUL log pkt no aggregate support
06/02/09   ax      RLC debugging related utilities
==============================================================================*/

#ifndef LTE_RLC_DBG_H
#define LTE_RLC_DBG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "ds3gpp_ext_msg.h"
#include "a2_diag.h"
#include "lte_rlcdli.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define LTE_RLC_DBG_FC_SHUTDOWN_ASSERT_MASK                               0x1
#define LTE_RLC_DBG_DEFAULT_LTA_LENGTH                                    200
#define LTE_RLC_DBG_DEFAULT_PATH_LOSS_THRESHOLD                            90
#define LTE_RLC_DBG_PATH_LOSS_HIGH_REST_RETX_CNT                            6
#define LTE_RLC_DBG_PATH_LOSS_LOW_REST_RETX_CNT                             3

typedef struct
{
  uint32   wm_low;       /*!< watermark low */
  uint32   wm_hi;        /*!< watermark high */
  uint32   wm_dne;       /*!< watermark DNE */
} lte_rlc_dbg_wm_cfg_s;

typedef struct
{
  uint8            num_bearer_bypass_fc;  /*number of bearers to bypass a2*/
  ds_3gpp_lte_qci_enum_type   qci_value[DS_LTE_QCI_9+1];
}lte_rlc_dbg_bypass_fc_bearers;

typedef struct
{
  uint16 ta_gap_threshold;
  uint16 shrink_threshold;
}lte_rlc_dbg_fast_nak_config;

/*! @brief structure that contains all debug related variables
*/
typedef struct
{
  boolean  rlcdl_log_pkt_no_aggregate; /*!< instant commit for every DL PDU*/
  boolean  rlcul_log_pkt_no_aggregate; /*!< instant commit for every UL PDU*/
  boolean  pdcpul_log_pkt_no_aggregate;/*!<instant commit for PDCPUL PDU*/
  uint8    rlc_min_so;                 /*!<RLC minimum segment offset   */
  uint32   profile_log_freq_mask;      /*!<profile logging freq mask   */
  uint32   max_pending_rlcdl_sdu;      /*!<maximum pending rlcdl sdus allowed*/
  uint16   seq_modulus_at_fc;          /*!< sequence modulus during flow control
                                            1024 for standard window size 512 */
  uint32   assert_mask;         /*!<control what condition assert is needed   */
  lte_rlc_dbg_wm_cfg_s  embms_wm_cfg;       /*!<RLC-A2 watermark configuration*/
  lte_rlc_dbg_bypass_fc_bearers bypass_fc_bearers; /*!< Bearers bypassed for forced flush */
  uint16   pdcpul_sdu_bytes_to_log;    /*!< num pdcp sdu to be logged */
  /*AM reordering timer value under low DSM count behavior*/
  uint16   fc_am_t_reordering;
  /*AM status timer value under low DSM count behavior*/
  uint16   fc_am_t_status_prohibit;
  /*UM reordering timer value under low DSM count behavior*/
  uint16   fc_um_t_reordering;  
  /*Level to start aggresive configuration */
  uint16   fc_rx_win_high_threshold;  
  /*Level to stop aggresive configuration*/
  uint16   fc_rx_win_low_threshold;
  /*Level to start Fast ACK in RLCDL UM*/
  uint8    embms_dyn_pri_mask;/*!< variable to indicate whether the dynamic 
             priority logic is active or not, 0-inactive*/
  uint8    embms_disable_send_burst_pri;  /*!<Disable L2 sending the priority
              change indication to ML1, 0-send, 1-Don't send*/
  uint8    embms_content_control; /*!<variable to request control ind from the
             service layer, 0-None, 1-All Objects, 2-Video Objects only*/           
  uint8    embms_status_control; /*!<variable to request decode status from the
             service layer, 0-Disable, 1-All Objects, 2-Video Objects only*/ 
  uint8    embms_serv_pri; /*Priority of the monitored service to be overwritten.
             Valid values are 0 to 5 - Initialized to 0xFF*/
  uint16   embms_hi_pri_timer; /*< Duration of high priority after a new service started in
             ms-default=10s (10000ms)*/
  uint32   embms_sca_fact_a; /*Threshold scaling factor A. 
             NV/EFS item, default = 103*/
  uint32   embms_sca_fact_b; /*Threshold scaling factor B, used for not raising 
             priority when loss > Th_pri + B*1/N,where N = FLOOR{SegDuration/MSP} */
  boolean  embms_first_burst_hi_pri; /*Whether first burst of every segment 
             should be high priority*/
  uint16   embms_min_msp_bcast; /*Minimum MSP value to use LOW priority instead of 
             background priority*/
  uint8    embms_av_trans_pat; /*Audio video transmit pattern*/
  uint32   embms_vid_seg_thres_size_per_ms; /*Minumum size of the video segment (Th_a)*/
  boolean  embms_use_background_low_pri; /*Use the background priority for
             EMBMS low priority*/
  uint32   embms_sca_fact_delta; /*Delta for scaling factor*/

  boolean  embms_use_adaptive_b_fact; /*Use the background priority for
             EMBMS low priority*/
  uint32   embms_sca_fact_b_min; /*Minimum value of the b factor*/
  uint32  lte_rlc_lta_detection_threshould; /* rlc detection tuneaway length threshold */
  uint32  lte_rlc_detection_pathloss_threshould; /* rlc dtection passloss threshold */

  uint16   rlc_fast_nak_ta_gap_threshold; /* The threshold for RLC modular to determin it
             is a long TA or short TA*/
  uint16   rlc_fast_nak_shrink_threshold; /* The threshold for RLC modular to 
             to determin whether nak q size shrink appropriately*/
} lte_rlc_dbg_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_rlc_dbg_init( void );

extern boolean lte_rlc_dbg_dl_log_pkt_no_aggregate( void );

extern boolean lte_rlc_dbg_ul_log_pkt_no_aggregate( void );

extern boolean lte_rlc_dbg_pdcpul_log_pkt_no_aggregate( void );

extern uint8 lte_rlc_dbg_get_min_so( void );

extern uint32 lte_rlc_dbg_get_profile_log_freq( void );

extern uint32 lte_rlc_dbg_get_max_pending_rlcdl_sdu( void );

extern uint16 lte_rlc_dbg_get_seq_mod_at_fc( void );

extern boolean lte_rlc_dbg_assert
(
  uint32       trigger_mask
);

extern lte_rlc_dbg_s* lte_rlc_dbg_get( void );

extern uint16 lte_rlc_dbg_get_pdcpul_log_sdu_bytes(void);

extern void lte_rlc_dbg_diag_cb_handle
(
  a2_diag_req_reqitem_s* reqitem
);

extern boolean lte_rlc_dbg_bearer_bypass_fc
(
  lte_rlcdli_ctrl_blk_ptr  ctrl_blk_ptr
);

#endif /* LTE_RLC_DBG_H */
