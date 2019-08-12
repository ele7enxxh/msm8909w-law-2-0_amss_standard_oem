/*!
  @file
  lte_rlcul_log.h

  @brief
  Header file for RLC UL related logging procedures

  @author
  araina axiao

*/

/*==============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_log.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/04/16   sb      CR954484: Reduce frequency of F3s without impacting debugging
09/26/14   ru      CR729990: Avoid PDCPUL/RLCUL Ping-Pong log buffer overflow
08/22/12   ax      CR383618: L2 modem heap use reduction phase I  
10/19/09   ax      log packet enhancement
09/17/09   ax      add num_invalid_ctrl_pdus_rcvd as part of the statistics
09/04/09   ax      UL statistics log pkt support
08/06/09   ax      moved LOG_PDU_BUF_SIZE to common header lte_rlc_cfg.h
07/16/09   ar      increased log buffer size from 1024 to 2048 bytes
06/03/09   ax      added log pkt no aggregate support
05/06/09   ax      added array_size to cfg log pkt to sheild log parsers from 
                   potential LTE_MAX_ACTIVE_RB change in value in the future
05/01/09   ar      removed rb cfg log sub pkt support
04/17/09   ar      moved log_mask from cfg sub pkt to pdu log sub pkt
04/07/09   ar      Added qxdm support for control, signaling and polling logging
03/26/09   ar      added pdu logging support
03/04/09   ax      cfg logging
10/28/09   ar      initial version
==============================================================================*/

#ifndef LTE_RLCUL_LOG_H
#define LTE_RLCUL_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <genlog.h>
#include <dsm_pool.h>
#include <lte_as.h>
#include <lte_mac_rlc.h>
#include <lte_mac_msg.h>

#include "lte_rlc_log.h"
#include "lte_rlculi.h"
#include "lte_rlc_cfg.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define LTE_RLCUL_LOG_ERROR(fmt, a, b, c)                                     \
  MSG_3(MSG_SSID_LTE_RLCUL, MSG_LEGACY_FATAL,  fmt, a, b, c );                \
  lte_rlcul_log_error_handler()

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief PACKED UL UM configuration definition
*/
typedef PACK(struct)
{
  uint8  sn_length; /*!< Indicates the UM RLC SN field size in bits. */
} lte_rlcul_log_um_cfg_s;


/*! @brief PACKED UL AM configuration definition
*/
typedef PACK(struct)
{
  uint32 poll_byte;          /*!< Indicates the value of constant Poll_Byte
                                  in bytes. Special value of
                                  LTE_OTA_IE_INFINITY_UINT32(0xffffffff) means
                                  infinite*/
  uint16 poll_pdu;           /*!< Indicates the value of constant Poll_Byte
                                  in pdus. Special value of
                                  LTE_OTA_IE_INFINITY_UINT16(0xffff) means
                                  infinite */
  uint16 t_poll_retransmit;  /*!< Indicates the value of timer T_poll_retransmit
                                  in ms */
  uint8  max_retx_threshold; /*!< Maximum number of re-transmissions */
} lte_rlcul_log_am_cfg_s;


/*! @brief PACKED union of UL configuration
*/
typedef PACK(union)
{
  lte_rlcul_log_um_cfg_s  um_cfg; /*!< configuration for UM */
  lte_rlcul_log_am_cfg_s  am_cfg; /*!< configuration for AM */
} lte_rlcul_log_cfg_u;


/*! @brief RLCUL RB info definition
*/
typedef PACK(struct)
{
  lte_rlc_log_rb_mode_t  rb_mode;              /*!< TM/UM/AM */
  lte_lc_id_t            lc_id;                /*!< logical channel ID */
  lte_rb_id_t            rb_id;                /*!< radio bearer ID */
  lte_rb_cfg_idx_t       rb_cfg_idx;           /*!< RB configuration index */
  lte_rlc_log_rb_type_t  rb_type;              /*!< SRB/DRB */
  lte_rlcul_log_cfg_u    cfg;                  /*!< configuration of the RB */
} lte_rlcul_log_rb_info_s;


/*! @brief RLCUL configuation subpkt
*/
typedef PACK(struct)
{
  lte_rlc_log_cfg_reason_t  cfg_reason;      /*!< configuration reason */

  uint8                     array_size;   /*!< this is only useful for parser
                                         should be set to LTE_MAX_ACTIVE_RB */

  uint8                     num_released_rb; /*!< number of released RBs */
  /*! Released RB cfg index */
  lte_rb_cfg_idx_t          released_rb_cfg_idx[LTE_MAX_ACTIVE_RB];

  uint8                     num_modified_rb; /*!< number of add/modified RBs */
  /*!< add/modified RB configuration index */
  lte_rlc_log_modified_rb_s modified_rb[LTE_MAX_ACTIVE_RB];

  uint8                     num_active_rb;   /*!< number of active RBs*/
  /*!< rb info */
  lte_rlcul_log_rb_info_s   active_rb_info[LTE_MAX_ACTIVE_RB];
} lte_rlcul_log_sub_pkt_cfg_s;


/*! @brief This struct defines the format of PDU subpacket */
typedef PACK(struct)
{
  lte_rb_cfg_idx_t      rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_rb_mode_t rb_mode;    /*!< action on the RB */
  uint8                 sn_length;  /*!< sequence length */
  uint8                 reserved;   /*!< filler */
  uint16                log_mask;   /*!< log mask */
  uint16  vt_a;                       /*!< VT(A) */
  uint16  vt_s;                       /*!< VT(S) */
  uint16  pdu_without_poll;           /*!< PDU_WITHOUT_POLL */
  uint32  byte_without_poll;          /*!< BYTE_WITHOUT_POLL */
  uint16  poll_sn;                    /*!< POLL_SN */
  uint16                num_pdus;   /*!< number of pdu's */
  byte                  log_buf[LTE_RLC_CFG_UL_LOG_PDU_BUF_SIZE]; /*!< PDU log 
                                                                    buffer */
  uint16                size;       /*!< payload size, not submitted to DIAG */
} lte_rlcul_log_sub_pkt_pdu_s;

/*! @brief radio bearer RLC statistics counters
*/
typedef PACK(struct)
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
  uint32 num_invalid_ctrl_pdus_rcvd;  /*!< Total num of invalid control PDUs 
                                           received */
  uint32 num_poll_pdus_txed;          /*!< Total number of RLC PDUs transmitted
                                           with poll bit set */
  uint32 num_t_poll_retx_expiry;      /*!< Total number of times 
                                           t_PollRetransmission expires */
  uint32 reserved;                    /*!< reserved */
} lte_rlcul_log_stat_counter_s;

/*! @brief UL RB statistics definition
*/
typedef PACK(struct)
{
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB configuration index: unique */
  lte_rlc_log_rb_mode_t  rb_mode;    /*!< TM/UM/AM */
  uint32                 num_rst;    /*!< num re-establishment since RB added */
  lte_rlcul_log_stat_counter_s tot; /*!<total stats */
  lte_rlcul_log_stat_counter_s curr;/*!<curr stats since last re-establishment*/
} lte_rlcul_log_rb_stat_s;

/*! @brief This struct defines the format of statistics subpacket */
typedef PACK(struct)
{
  uint8                  num_rb;    /*!< number of radio bearer stats logged */
  uint32                 num_errors;/*!< number of rlcul errors */
  lte_rlcul_log_rb_stat_s rb_stats[LTE_MAX_ACTIVE_RB]; /*!< RB statistics */
} lte_rlcul_log_sub_pkt_stats_s;


#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif


/*! @brief Structure for RLCUL Log buffer
*/
typedef struct
{
  lte_rlcul_log_sub_pkt_pdu_s  pdus[2];   /*!< PDU sub pkt double buffer */
  lte_rlcul_log_sub_pkt_pdu_s* read_ptr;  /*!< Flush happens on this buffer */
  lte_rlcul_log_sub_pkt_pdu_s* write_ptr; /*!< sub pkts are written to this
                                               buffer */
} lte_rlcul_log_buf_s;

/*! @brief structure for lte_rlcul_log file static variables
*/
typedef struct
{
  uint32                             num_errors;        /*!< number of error */
  uint32                             num_forced_pdu_log_flush; /*!< number of 
                                       pdu log flush due to buffer size limit */
  uint32                             num_log_failures;    /*!< number of 
                                       instances when logging failed */  
  uint32                             num_pdus_not_logged; /*!< number of 
                                       pdus not logged due to log buffer full */
  lte_rlcul_log_sub_pkt_cfg_s        ul_cfg;                     /*!< ul cfg */
  lte_rlcul_log_sub_pkt_stats_s      statistics;             /*!< statistics */
  lte_rlcul_log_buf_s*               mapping[LTE_MAX_RB_CFG_IDX];/*!< mapping */
  genlog_s                           genlog;                     /*!< GENLOG */
  uint16                             log_mask;                   /*!< log mask*/
  /*! LTE_RLCUL_FLUSH_RLCUL_LOG_BUF_IND msg */
  lte_rlcul_flush_rlcul_log_buf_ind_s flush_log_ind;
} lte_rlcul_log_s;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_rlcul_log_get_log_mask

==============================================================================*/
/*!
    @brief
    returns the rlcul log mask

    @return
    uint16
*/
/*============================================================================*/
extern uint16 lte_rlcul_log_get_log_mask
(
  void
);
/* lte_rlcul_log_get_log_mask() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_init

==============================================================================*/
/*!
    @brief
    intialization routine for rlcul log.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_init
(
  void
);
/* lte_rlcul_log_init() */

/*==============================================================================

  FUNCTION:  lte_rlcul_log_deinit

==============================================================================*/
/*!
    @brief
    intialization routine for rlcul log.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_deinit
(
  void
);

/*===========================================================================

  FUNCTION:  lte_rlcul_log_update_status

===========================================================================*/
/*!
  @brief
  This functions updates the local log status based on log code when RLCUL log
  flush timer is expired

  @details
  The implemenation is subject to change. In the future, there will be a push
  mechanism by diag when new log status is sent to RLCUL via message. Therefore
  the periodic polling is no longer needed.

  @return
  none
*/
/*=========================================================================*/
extern void lte_rlcul_log_update_status
(
  void
);
/* lte_rlcul_log_update_status() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_re_est_rb

==============================================================================*/
/*!
    @brief
    routine needed when a RB is re-established.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_re_est_rb
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr  /*!< control block ptr */
);
/* lte_rlcul_log_re_est_rb() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_remove_rb

==============================================================================*/
/*!
    @brief
    routine needed when a RB is removed.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_remove_rb
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr,  /*!< control block ptr */
  boolean                log_F3  /*!<flag to indicate if F3 needs to be logged */
);
/* lte_rlcul_log_remove_rb() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_add_rb

==============================================================================*/
/*!
    @brief
    routine needed when an RB is added.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_add_rb
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr  /*!< control block ptr */
);
/* lte_rlcul_log_add_rb() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_modify_rb

==============================================================================*/
/*!
    @brief
    routine needed when a RB is modified.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_modify_rb
(
  lte_rlculi_ctrl_blk_s* const ctrl_blk_ptr  /*!< control block ptr */
);
/* lte_rlcul_log_modify_rb() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_pdu

==============================================================================*/
/*!
    @brief
    This function logs the transmitted pdu

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_pdu
(
  lte_rlculi_ctrl_blk_s*       ctrl_blk_ptr,  /*!< control block ptr */
  const uint8*                 rlc_pdu_ptr,     /*!< pdu ptr */
  uint16                       pkt_len,         /*!< pdu size in bytes */
  uint16                       bytes_to_log,    /*!< bytes to log */
  lte_sfn_s                    sfn,             /*!< sfn the PDU is rxed */
  uint16                       pdu_log_mask     /*!< log mask of txed pdu */
);
/* lte_rlcul_log_pdu() */

/*==============================================================================

  FUNCTION:  lte_rlcul_log_submit_cfg_log_pkt

==============================================================================*/
/*!
    @brief
    commit the log packet.

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_submit_cfg_log_pkt
(
  const lte_rlcul_cfg_req_s*  cfg_msg_ptr  /*!< ptr to cfg msg */
);
/* lte_rlcul_log_submit_cfg_log_pkt() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_switch_log_buf

==============================================================================*/
/*!
    @brief
    Function switches the log buffer for MACUL to log pdus.

    @detail
    The switching of the log buffer is under the pdu build lock

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_switch_log_buf
(
  lte_rb_cfg_idx_t rb_cfg_idx,              /*!< RB cfg index */
  pthread_mutex_t* const pdu_build_lock_ptr /*!< pdu build lock ptr */
);
/* lte_rlcul_log_switch_log_buf() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_flush_ind_handler

==============================================================================*/
/*!
    @brief
    Handler function for LTE_MAC_FLUSH_RLC_LOG_BUF_IND

    @detail
    This functions submits the rlc packets to DIAG

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_flush_ind_handler
(
  lte_rb_cfg_idx_t  rb_cfg_idx  /*!< rb config index */
);
/* lte_rlcul_log_flush_ind_handler() */


/*==============================================================================

  FUNCTION:  lte_rlcul_log_flush_timer_expiry_handler

==============================================================================*/
/*!
    @brief
    Handler function for flushing the log buf on log flush timer expiry

    @detail
    This functions submits the rlc packets to DIAG

    @return
    None
*/
/*============================================================================*/
extern void lte_rlcul_log_flush_timer_expiry_handler
(
  lte_rlculi_ctrl_blk_s*  ctrl_blk_ptr  /*!< control block pointer */
);
/* lte_rlcul_log_flush_timer_expiry_handler() */

extern void lte_rlcul_log_statistics
(
  const lte_rlculi_ctrl_blk_s*     ctrl_blk_ptr
);

extern void lte_rlcul_log_statistics_accumulate
(
  lte_rlculi_ctrl_blk_s*     ctrl_blk_ptr
);

extern void lte_rlcul_log_submit_statistics_log_pkt( void );

extern void lte_rlcul_log_error_handler( void );

extern void lte_rlcul_log_reset_error_counter( void );

#endif /* LTE_RLCUL_LOG_H */
