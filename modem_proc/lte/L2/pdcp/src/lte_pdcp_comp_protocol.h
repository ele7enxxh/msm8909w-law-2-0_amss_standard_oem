/*!
  @file
  lte_pdcp_comp_protocol.h

  @brief
  This file contains defines and functions interfaces for PDCP COMP protocol stack.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcp_comp_protocol.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/16   sb      CR979285: UDC HO fixes and enhancements
07/23/15   sb      CR867973: Enhance logging and QSH for UDC
06/29/15   ar      include customer.h for UDC and ELS feature definition
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/15/15   sb      CR 803880: Initial check-in - PDCP Changes for UDC feature
===========================================================================*/

#ifndef LTE_PDCP_COMP_PROTOCOL_H
#define LTE_PDCP_COMP_PROTOCOL_H

#include <customer.h>

#ifdef FEATURE_LTE_UDC_ENABLED 

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include "lte_l2_comdef.h"
#include "mutils_circ_q.h"
#include "timetick.h"
#include "lte_pdcp_comp_msg.h"
#include "lte_pdcp_dl_if.h"
#include "lte_pdcp_ul_if.h"
#include "lte_pdcpi.h"
#include "lte_fc.h"
#include "lte_pdcp_ul_protocol.h"
#include "udcif.h"

/*! @brief PDCP Control PDU header with UDC status looks as under
  | D/C(1bit) | PDU_TYPE(3bits) | CS(3bits) | U/N(1bit) |
  UDC status PDU type value is 001 */
#define LTE_PDCP_COMP_UDC_STATUS_PDU_TYPE_MASK    0x10

/*! @brief PDCP Control PDU header with UDC status looks as under
  | D/C(1bit) | PDU_TYPE(3bits) | CS(3bits) | U/N(1bit) |
  UDC status Compression Status (CS) disable value is 000 */
#define LTE_PDCP_COMP_UDC_STATUS_CS_DIS_MASK   0x00

/*! @brief PDCP Control PDU header with UDC status looks as under
  | D/C(1bit) | PDU_TYPE(3bits) | CS(3bits) | U/N(1bit) |
  UDC status Compression Status (CS) enable value is 001 */
#define LTE_PDCP_COMP_UDC_STATUS_CS_EN_MASK   0x02

/*! @brief PDCP Control PDU header with UDC status looks as under
  | D/C(1bit) | PDU_TYPE(3bits) | CS(3bits) | U/N(1bit) |
  UE initiated UDC status PDU has U/N bit as 0 */
#define LTE_PDCP_COMP_UDC_STATUS_UE_INITATED   0x00

/*! @brief PDCP Control PDU header with UDC status looks as under
  | D/C(1bit) | PDU_TYPE(3bits) | CS(3bits) | U/N(1bit) |
  Network initiated UDC status PDU has U/N bit as 1 */
#define LTE_PDCP_COMP_UDC_STATUS_ENB_INITATED   0x01

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*!@brief To check if CPU minitor is active in current
CFCM command'
*/
#define LTE_PDCP_IS_CPU_MONITOR_ACTIVE(x)     ((x) & CFCM_MONITOR_MASK_CPU)

/*!@brief To check if DSM Large monitor is active in current
CFCM command'
*/
#define LTE_PDCP_IS_DSM_LARGE_MONITOR_ACTIVE(x)     ((x) & CFCM_MONITOR_MASK_DSM_LARGE_POOL_COMP)

/*!@brief To check if DSM DUP monitor is active in current
CFCM command'
*/
#define LTE_PDCP_IS_DSM_DUP_MONITOR_ACTIVE(x)     ((x) & CFCM_MONITOR_MASK_DSM_DUP_POOL_COMP)

/*! @brief COMP RB state for UDC
*/
typedef enum
{
  LTE_PDCP_COMP_IDLE,
  LTE_PDCP_COMP_ACTIVE
} lte_pdcp_comp_state_e;

/*! @brief PDCP RB COMP flow state
*/
typedef enum
{
  LTE_PDCPCOMP_FLOW_ENABLE = LTE_PDCPUL_FLOW_ENABLE,  /* enabled */
  LTE_PDCPCOMP_FLOW_DISABLE = LTE_PDCPUL_FLOW_DISABLE, /* cross high wm */
  LTE_PDCPCOMP_FLOW_SUSPEND = LTE_PDCPUL_FLOW_SUSPEND, /* during HO or
                                                   RB release or RLF suspend */
  LTE_PDCPCOMP_FLOW_DISABLE_SUSPEND = LTE_PDCPUL_FLOW_DISABLE_SUSPEND/* during HO
                                    or RB release or RLF suspend */
} lte_pdcp_comp_flow_e;

/*! @brief Different interpretations of the DSM app field for UDC cases
*/
typedef enum
{
  LTE_PDCP_COMP_APP_FIELD_RESERVED = 0,   /* reserved */
  LTE_PDCP_COMP_APP_FIELD_UDC_STATUS, /* means dsm item a UDC status pdu */
  LTE_PDCP_COMP_APP_FIELD_MAX
} lte_pdcp_comp_app_field_e;

typedef enum
{
  LTE_PDCP_COMP_FULL_COMP_STATE = 1,
  LTE_PDCP_COMP_HDR_ONLY_COMP_STATE,
  LTE_PDCP_COMP_NO_COMP_STATE,
  LTE_PDCP_COMP_MAX_STATE
}lte_pdcp_comp_fc_state;

/*! @brief PDCP COMP parameter control block for compressor
*/
typedef lte_pdcp_comp_info_s lte_pdcp_comp_cfg_s;

/*! @brief PDCPCOMP-PDCP WM
*/
typedef struct
{
  dsm_watermark_type  wm_item; /*!< COMP PDCP tx watermark */
  q_type              wm_q;       /*!< COMP PDCP tx queue */
} lte_pdcpcomp_pdcp_wm_s;

/*! @brief PDCP COMP control block for UL Data
*/
typedef struct
{
  lte_pdcp_comp_state_e state;   /*!< PDCP OFFLOAD decomp state */
  volatile lte_pdcp_comp_flow_e flow;

  dsm_watermark_type  *ps_to_pdcpcomp_wm_ptr;
  lte_pdcpcomp_pdcp_wm_s   pdcpcomp_to_pdcp_wm;

  /*!< UDC handle if configured  */
  udc_comp_inst_handle_type   udc_handle;

  /*!<1st packet after RESET is compressed*/
  boolean            buff_rst_req;

  boolean rab_reg_udc_ctrl;     /*!<Boolean to indicate if UDC ctrl PDU needs to be sent
                                     during RAB_REG*/

} lte_pdcp_comp_state_s;

/*! @brief PDCP OFFLOAD statistics of the RB for compressor
*/
typedef struct
{
  uint32  num_udc_comp_pkts; /*!<*total num of UDC compressed pkts*/
  uint32  udc_comp_bytes; /*!<*total num of UDC compressed  bytes*/  
  uint32  num_udc_fc_uncomp_pkts; /*!< total num of UDC uncompressed pkts*/
  uint32  udc_fc_uncomp_bytes; /*!<*total num of UDC non-compressed  bytes*/
  uint32  data_bytes_from_ps;  /*!< total bytes dequeued from PS Wm*/
  uint32  num_udc_fail;  /*!< Number of UDC compression failed*/
  uint16  num_udc_reset; /*!< Num of time UDC RESET is called */
  uint16  num_udc_reenq_req; /*!< Num of req to reenque pkts to PS watermark*/
  uint32  num_ue_udc_ctrl_pdus;      /*!< num of UE triggered UDC Ctrl PDUs */
  uint32  num_enb_udc_ctrl_pdus;      /*!< num of eNB triggered UDC Ctrl PDUs */
  uint32  num_udc_reenq_pkts;   /*!< Num of pkts reenqued to PS watermark*/
  uint32  udc_reenq_bytes;  /*!< Num of bytes reenqued to PS watermark*/
  uint32  num_udc_dsm_fc_down;         /*!< num of  CMD_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_down;         /*!< num of  CMD_DOWN for CPU monitor*/
  uint32  num_udc_dsm_fc_shutdown;        /*!< num of  CMD_SHUT_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_freeze;           /*!< num of  CMD_UP for CPU monitor*/
  uint32  num_uncomp_retx_pdus;  /*!< num of RETX PDUs enqueued
                                             to COMP Wm */
} lte_pdcp_comp_stats_s;


/*! @brief PDCP COMP RB control block for UL Data
*/
typedef struct
{
  lte_pdcp_comp_cfg_s                  cfg;      /*!< config */
  lte_pdcp_comp_state_s                state;    /*!< state  */
  lte_pdcp_comp_stats_s                stats;    /*!< statistics of the RB since last 
                                                                                      reestablishment/activation*/
  lte_pdcp_comp_stats_s                stats_upto_rst;    /*!< accumulation upto the last est*/
} lte_pdcp_comp_cb_s;

typedef struct
{
  dsm_watermark_type  *ps_to_pdcpcomp_wm_ptr;
}lte_pdcp_comp_eps_info_s;

/*! @brief PDCP COMP protocol stack control block for UL
*/
typedef struct
{
  uint8                      num_rb_supported;       /*!<Num of RBs supporting UDC*/
  uint8                      num_active_rb;          /*!< Number of active UDC RB */
  lte_pdcp_comp_fc_state     fc_state; /*!< UDC FC state */
  boolean                    buff_transfer_supported;       /*!<Buffer forward supported*/
  lte_pdcp_comp_cb_s         rb_cb[LTE_MAX_RB_CFG_IDX];    /*!< RB control block array */
  
  lte_pdcp_comp_eps_info_s   eps_info[LTE_MAX_EPS]; /*!< EPS related information */
  
} lte_pdcp_comp_protocol_cb_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

  FUNCTION:  lte_pdcp_comp_get_ul_cb_size

===========================================================================*/
/*!
    @brief
    This function is called to return the total control block size for PDCP COMP for UL Data.

    @detail

    @return
    COMP protocol stack UL control block size

    @note

    @see

*/
/*=========================================================================*/
extern uint32 lte_pdcp_comp_get_ul_cb_size
(
  void
);

extern void lte_pdcp_comp_wm_enqueue_handler
(
  lte_rb_cfg_idx_t	rb_cfg_idx
);

extern void lte_pdcp_comp_protocol_static_init(void);

extern lte_pdcp_comp_protocol_cb_s* lte_pdcp_comp_get_comp_cb
(
  void
);

extern void lte_pdcp_comp_to_pdcp_lo_water_cb
(
  dsm_watermark_type *wm_ptr,
  void*              callback_data
);

extern dsm_item_type* lte_pdcp_comp_generate_udc_status_pdu
(
  lte_rb_cfg_idx_t  rb_cfg_idx,  /*!< Rb Cfg Idx */
  boolean           comp_enable, /*!< generate status pdu with comp enable or disable */
  boolean           ue_initiated /*!< UE initiated  status pdu with comp enable or disable */
);
/*==============================================================================

  FUNCTION:  lte_pdcp_comp_get_pdcp_comp_pdcp_wm

==============================================================================*/
/*!
    @brief
    This function is called to get the PDCP OFFLOAD PDCP WM maintained by PDCP COMP.

    @detail

    @return
    PDCP COMP-PDCP WM ptr

    @note

    @see

*/
/*============================================================================*/
extern dsm_watermark_type* lte_pdcp_comp_get_pdcpcomp_pdcp_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

extern dsm_watermark_type* lte_pdcp_comp_get_ps_pdcpcomp_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

EXTERN uint32 lte_pdcp_comp_num_comp_fail
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN void lte_pdcp_comp_reset_comp_stats
(
 lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN void lte_pdcp_comp_reset_fc_state
(
  void
);

EXTERN lte_pdcp_comp_cb_s* lte_pdcp_comp_get_comp_cb_ptr
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcp_comp_send_wm_low_ind

===========================================================================*/
/*!
  @brief
  notify PDCP COMP that low watermark has been crossed on disabled RB

  @return
  None
*/
/*=========================================================================*/
extern void lte_pdcp_comp_send_wm_low_ind
(
  uint32  rb_cfg_idx    /*!< rb_cfg_idx for the RB */
);

extern void lte_pdcp_comp_send_wm_low_ind_force_trigger
(
  uint32  rb_cfg_idx    /*!< rb_cfg_idx for the RB */
);

extern void lte_pdcp_comp_fc_ind_handler
(
  cfcm_cmd_msg_type_s*        fc_msg_ptr /*!< flow control message ptr */
);

extern void lte_pdcp_comp_rb_add_mod_ind_handler
(
  lte_pdcp_comp_rb_add_mod_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_rb_rel_ind_handler
(
  lte_pdcp_comp_rb_rel_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_rb_reset_ind_handler
(
  lte_pdcp_comp_rb_reset_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_rab_reg_ind_handler
(
  lte_pdcp_comp_rab_reg_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_rab_dereg_ind_handler
(
  lte_pdcp_comp_rab_dereg_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_reestab_ind_handler
(
  lte_pdcp_comp_reestab_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_mod_resume_ind_handler
(
  lte_pdcp_comp_mod_resume_ind_msg_s* msg_ptr /*!< input message pointer */
);

extern void lte_pdcp_comp_to_pdcp_hi_water_cb
(
  dsm_watermark_type *wm_ptr,
  void*              callback_data
);

EXTERN void lte_pdcp_comp_qsh_analysis_routine
(
  qsh_action_e action,
  uint32 category_mask
);

EXTERN void lte_pdcp_comp_send_wm_enqueue_ind_msg
(
  lte_rb_cfg_idx_t rb_cfg_idx
);

EXTERN errno_enum_type lte_pdcp_comp_is_msg_pending
(
  void
);

EXTERN void lte_pdcp_comp_statistics_accumulate
(
  lte_rb_cfg_idx_t rb_cfg_idx /*!< rb_cfg_idx */
);

EXTERN lte_pdcp_comp_stats_s* lte_pdcp_comp_get_udc_statistics
(
  lte_rb_cfg_idx_t rb_cfg_idx /*!< rb_cfg_idx */
);

#endif /*FEATURE_LTE_UDC_ENABLED */

#endif /* LTE_PDCP_COMP_PROTOCOL_H */



