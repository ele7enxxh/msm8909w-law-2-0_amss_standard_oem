/*!
  @file
  lte_rlcdl_cfg.h

  @brief
  definition file for RLC DL configuration related functionality.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_cfg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/31/16   cx      CR923654: Fast nak optimization
04/15/15   ru      CR816655: RLC mitigation to prevent RLF due to FC Shutdown
11/23/14   sb      CR745901: Turn AM to UM mode and do best effort deliver
                   when new Flow contrl level is triggered.
10/02/14   mg      CR733800: Fix compile errors on Jolokia
04/11/14   mg      CR647594: QSH Phase 1
06/27/14   sb      CR629741: Change in LTE to migrate to CFCM
04/03/14   mg      CR643306: DSDS RLC Fast NAK after receiving RF_AVAILABLE_IND 
01/23/14   mg      CR599494: RLC behavior to alleviate the low DSM count problem. 
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
07/18/11   ax      support RLC statistics query 
06/08/10   ax      fixed lint warning
02/17/09   ax      add lte_rlcdl_cfg_deinit()
11/19/08   ax      use lte_rb_cfg_idx_t as a unique identifier across AS
08/27/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_CFG_H
#define LTE_RLCDL_CFG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include "lte_rlcdli.h"
#include "lte_rlc_msg.h"
#include "lte_fc.h"
#include "lte_cphy_msg.h"
#include "qsh.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief internal data structure for lte_rlcdl_cfg module
*/
typedef struct
{
  lte_rlcdli_ctrl_blk_s*  rb_cfg_idx_2_ctrl_blk[LTE_MAX_RB_CFG_IDX];  /*!< radio
                          bearer configuration index to control block mapping */
  uint32                  num_active_rb;          /*!< number of active RB's */
  lte_rlcdli_ctrl_blk_s*  active_cb_ptr[LTE_MAX_ACTIVE_RB];  
  /*!< dense active control block pointer array  */
  uint32                   pkt_log_flush_counter;  /*!< pkt log fluch counter */
  /*!< DSDS Fast NAK timer */
  uint32                  dsds_fast_nak_timer;
  /*!< DSDS Fast NAK timer flag */
  boolean                 dsds_fast_nak_timer_active;
  /*!< tuneaway start timestamp*/
  timetick_type           dsds_fast_nak_ta_start_time;
} lte_rlcdl_cfg_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern void lte_rlcdl_cfg_init( void );

extern void lte_rlcdl_cfg_deinit
( 
  boolean               verify_connect_rel
);

extern lte_rlcdli_ctrl_blk_ptr lte_rlcdl_cfg_get_ctrl_blk
(
  lte_lc_id_t            lc_id
);

extern lte_rlcdli_ctrl_blk_ptr lte_rlcdl_cfg_get_ctrl_blk_by_rb_cfg_idx
(
  lte_rb_cfg_idx_t       rb_cfg_idx
);

extern void lte_rlcdl_cfg_proc
(
  const lte_rlcdl_cfg_req_s*   msg_ptr,
  uint32                       msg_length
);

extern void lte_rlcdl_cfg_timer_expire( void );

extern void lte_rlcdl_cfg_get_stats
(
  lte_rlcdl_stats_req_s*      stats_req_msg
);

EXTERN void lte_rlcdl_cfg_proc_fc_level1_cmd
(
  cfcm_cmd_msg_type_s*   fc_cmd_msg_ptr   /*!< flow control message pointer */
);

extern void lte_rlcdl_cfg_proc_fc_cmd
(
  cfcm_cmd_msg_type_s*   fc_cmd_msg_ptr   
);

extern void lte_rlcdl_cfg_proc_fc_cmd_task_inactive
(
  cfcm_cmd_msg_type_s*   fc_cmd_msg_ptr
);

extern void lte_rlcdl_cfg_qsh_proc(qsh_action_e action, uint32 category_mask);

extern boolean lte_rlcdl_cfg_qsh_analysis
(
  qsh_cb_params_s *params
);

extern void lte_rlcdl_cfg_proc_rf_available( lte_cphy_tuneaway_t ta_type );

extern boolean lte_rlcdl_cfg_get_rf_available_ind( void );

EXTERN void lte_rlcdl_cfg_proc_rf_unavailable( void );

EXTERN lte_rlcdl_cfg_s * lte_rlcdl_cfg_get( void );

extern void lte_rlcdl_cfg_set_rf_available_ind
(
  boolean rf_available_ind 
);

extern void lte_rlcdl_cfg_proc_scell_tuneaway_status
(
  boolean ta_start
);

extern void lte_rlcdl_qos_info_handler
(
  ds_3gpp_bearer_qos_info_msg_s* qos_info
);

extern void lte_rlcdl_cfg_reset_global_fc_mask
( 
  void 
);

#endif /* LTE_RLCDL_CFG_H */
