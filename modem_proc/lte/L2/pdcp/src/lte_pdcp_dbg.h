/*!
  @file
  lte_pdcp_dbg.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  gxiao

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_dbg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/16/15   ru      CR896720: PDCP Discard Timer Overwrite
06/29/15   ar      include customer.h for UDC and ELS feature definition
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
01/08/15   sb      CR775692: Reduce threshold of OFFLOAD-PDCP Wm
12/23/14   sb      CR772190: Extend A2 diag framework for L2
04/11/14   mg      CR647594: QSH Phase 1
07/24/14   mg      CR699600: Revert CR669056
07/03/14   mg      CR669056: PDCP discard timer algorithm
09/16/13   mg      CR501406: Packet based flow control required for DSM items to 
                   efficiently support Small IP packet tests at high data rates 
08/31/11   ax      make watermark size runtime configurable
06/15/11   ax      RoHC support
01/24/11   ax      fix compiler warning
01/17/11   ax      remove invalid assert on flow state
01/07/11   ax      add pdcp assert mask
03/05/10   ax      initial version
==============================================================================*/

#ifndef LTE_PDCP_DBG_H
#define LTE_PDCP_DBG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <customer.h>
#include "comdef.h"
#include <custlte.h>
#include "ps_rohc.h"
#include "qsh.h"
#include <lte_mac_msg.h>
#include <lte_mac_ind.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define LTE_PDCP_DBG_UDC_RESET_ASSERT_MASK                                   0x1
#define LTE_PDCP_DBG_OUTOF_META_INFO_ASSERT_MASK                             0x2
#define LTE_PDCP_DBG_PENDING_ANORMALY_ASSERT_MASK                            0x4
#define LTE_PDCP_DBG_WINDOW_STALL_ASSERT_MASK                                0x8

#define LTE_PDCP_DBG_DISCARD_TIMER_OVERWRITE_DISABLE                           0
#define LTE_PDCP_DBG_DISCARD_TIMER_DISABLE                                0xFFFF

typedef struct
{
  uint32   wm_low;       /*!< pdcp to RLC watermark low */
  uint32   wm_hi;        /*!< pdcp to RLC watermark high */
  uint32   wm_dne;       /*!< pdcp to RLC watermark DNE */
  uint32   wm_dne_count;   /*!< pdcp to RLC watermark DNE COUNT*/
} lte_pdcp_dbg_wm_cfg_s;

/*! @brief pdcp related debug variables
*/
typedef struct
{
  lte_pdcp_dbg_wm_cfg_s  pdcp_a2_wm;  /*!< pdcp to A2 watermark cfg */
  lte_pdcp_dbg_wm_cfg_s  pdcp_rlc_wm; /*!< pdcp to rlc watermark cfg */
  lte_pdcp_dbg_wm_cfg_s  offload_pdcp_wm; /*!< offload to pdcp watermark cfg */
  #ifdef FEATURE_LTE_UDC_ENABLED 
  lte_pdcp_dbg_wm_cfg_s  pdcpcomp_pdcp_wm; /*!< pdcpcomp to pdcp watermark cfg */
  #endif /*FEATURE_LTE_UDC_ENABLED */
  uint32   max_num_enqueue_wo_notify;/*!< max number of enqueue without notify*/
  uint32   option_mask;           /*!< optional feature mask */
  uint32   assert_mask;           /*!< assert mask variable */
  rohc_params_comp_channel_s_type comp_channel; /*!< compressor ch parameters*/
  rohc_params_comp_flow_s_type    comp_flow; /*!< compressor flow parameters*/
  rohc_params_decomp_channel_s_type decomp_channel; /*!< decompressor ch parameters*/
  rohc_params_decomp_flow_s_type  decomp_flow; /*!< decompressor flow parameters*/
  uint16   discard_timer_value;     /*!< discard timer value need to be overwritten */ 
} lte_pdcp_dbg_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

extern void lte_pdcp_dbg_init( void );

extern uint32 lte_pdcp_dbg_get_pdcp_a2_wm_lo( void );

extern uint32 lte_pdcp_dbg_get_pdcp_a2_wm_hi( void );

extern uint32 lte_pdcp_dbg_get_pdcp_a2_wm_dne( void );

extern uint32 lte_pdcp_dbg_get_pdcp_a2_wm_dne_q_cnt( void );


extern uint32 lte_pdcp_dbg_get_max_num_enq_wo_notify( void );

extern boolean lte_pdcp_dbg_f3_ul_ip_info( void );

extern boolean lte_pdcp_dbg_f3_dl_ip_info( void );

extern boolean lte_pdcp_dbg_assert
(
  uint32       trigger_mask
);

extern lte_pdcp_dbg_s* lte_pdcp_dbg_get( void );

/*==============================================================================
 
   FUNCTION:  lte_pdcp_dbg_analysis
 
 ==============================================================================*/
 /*!
     @brief
     PDCP callback API registered with QSH.
 
     @detail
.....This API should be invoked by QSH during error fatal case or when a
     request comes from DIAG. PDCP is expected to execute required analysis 
     routine either in A2 task context or in L2 task context
 
     @return
     TRUE is analysis is performed else FALSE
 */
 /*============================================================================*/
extern boolean lte_pdcp_dbg_analysis
(
  qsh_cb_params_s *params
);

/*===========================================================================

  FUNCTION:  lte_pdcp_dbg_diag_cb_handle

===========================================================================*/
/*!
    @brief

    @detail

    @return
    None

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcp_dbg_diag_cb_handle
(
  a2_diag_req_reqitem_s* reqitem
);

#endif /* LTE_PDCP_DBG_H */
