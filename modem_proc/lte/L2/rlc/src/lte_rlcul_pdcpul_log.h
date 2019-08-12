/*!
  @file
  lte_rlcul_pdcpul_log.h

  @brief
  Header file for PDCP UL related logging procedures

  @author
  araina

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcul_pdcpul_log.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/15/15   sb      CR 803880: RLC Changes for UDC feature
09/26/14   ru      CR729990: Avoid PDCPUL/RLCUL Ping-Pong log buffer overflow
05/19/10   ar      added PDCP ciphering log packet support
04/07/09   ar      changed pdcpul log mask from uint32 to uint16
03/26/09   ar      initial version
==============================================================================*/

#ifndef LTE_RLCUL_PDCPUL_LOG_H
#define LTE_RLCUL_PDCPUL_LOG_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <genlog.h>
#include <dsm_pool.h>
#include <lte_as.h>
#include <lte_pdcp_ul_if.h>
#include <lte_mac_rlc.h>
#include <lte_mac_msg.h>

#include "lte_rlc_rrc.h"
#include "lte_rlc_log.h"
#include "lte_rlcul_log.h"
#include "lte_rlculi.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!< enum to define PDCP PDU type to be logged in PDCP Control PDU log packet */
typedef enum
{
  LTE_PDCP_PDU_TYPE_STATUS,
  LTE_PDCP_PDU_TYPE_ROHC,
  LTE_PDCP_PDU_TYPE_UDC
}lte_pdcp_pdu_type_e;

/*! @brief Structure for PDCPUL Log buffer
*/
typedef struct
{
  lte_pdcp_log_sub_pkt_pdu_s   pdus[2];   /*!< PDU sub pkt double buffer */
  lte_pdcp_log_sub_pkt_pdu_s*  read_ptr;  /*!< Flush happens on this buffer */
  lte_pdcp_log_sub_pkt_pdu_s*  write_ptr; /*!< sub pkts are written to this
                                                buffer */
  boolean                      used;      /*!< being used */
} lte_pdcpul_log_buf_s;


/*! @brief structure for lte_rlcul_pdcpul_log file static variables
*/
typedef struct
{
  lte_pdcpul_log_buf_s        buf[LTE_MAX_ACTIVE_RB];      /*!<buf array*/
  lte_pdcpul_log_buf_s*       mapping[LTE_MAX_RB_CFG_IDX]; /*!< mapping */
  lte_pdcpul_cipher_log_buf_s cipher_buf;                  /*!< cipher pdu buf*/
  uint32                      log_mask;                    /*!< log mask*/
  pthread_mutex_t             ciph_log_lock;               /*!< mutex lock for
                                                                ciph log buff */
  uint16                      num_sdu_bytes_to_log;        /*!< num pdcp sdu
                                                                bytes to log */
  uint8                       last_pdu_ciph_algo;          /*!< ciph alg of last
                                                                logged pdu*/
  uint8                       last_pdu_ciph_key_idx;       /*!< key idx of last
                                                                logged pdu*/
  uint32                      num_log_failures;            /*!< number of 
                                       instances when logging failed */  
  uint32                      num_pdus_not_logged;         /*!< number of 
                                       pdus not logged due to log buffer full */
  /*! LTE_RLCUL_FLUSH_PDCPUL_LOG_BUF_IND msg */
  lte_rlcul_flush_pdcpul_log_buf_ind_s flush_log_ind;
} lte_pdcpul_log_s;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_get_log_mask

==============================================================================*/
/*!
    @brief
    returns the pdcpul log mask

    @return
    uint32
*/
/*============================================================================*/
EXTERN uint32 lte_pdcpul_log_get_log_mask
(
  void
);
/* lte_pdcpul_log_get_log_mask() */



/*==============================================================================

  FUNCTION:  lte_pdcpul_log_get_mapping_ptr

==============================================================================*/
/*!
    @brief
    This function returns the mapping ptr for a given rb_cfg_idx

    @detail
    none

    @return
    mapping ptr of type lte_pdcpul_log_buf_s* for a given rb_cfg_idx

*/
/*============================================================================*/
EXTERN lte_pdcpul_log_buf_s* lte_pdcpul_log_get_mapping_ptr
(
  lte_rb_cfg_idx_t rb_cfg_idx  /*!< RB cfg index */
);
/* lte_pdcpul_log_get_mapping_ptr() */


/*===========================================================================

  FUNCTION:  lte_pdcpul_log_update_status

===========================================================================*/
/*!
  @brief
  This functions updates the local log status based on log code when PDCPUL log
  flush timer is expired

  @details
  The implemenation is subject to change. In the future, there will be a push
  mechanism by diag when new log status is sent to PDCPUL via message. Therefore
  the periodic polling is no longer needed.

  @return
  none
*/
/*=========================================================================*/
EXTERN void lte_pdcpul_log_update_status
(
  void
);
/* lte_pdcpul_log_update_status() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_init

==============================================================================*/
/*!
    @brief
    intialization routine for pdcpul log.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_init
(
  void
);
/* lte_pdcpul_log_init() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_switch_log_buf

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
EXTERN lte_pdcp_log_sub_pkt_pdu_s* lte_pdcpul_log_switch_log_buf
(
  lte_rb_cfg_idx_t       rb_cfg_idx, /*!< RB cfg index */
  pthread_mutex_t* const lock_ptr    /*!< mutex lock ptr */
);
/* lte_pdcpul_log_switch_log_buf() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_switch_cipher_log_buf

==============================================================================*/
/*!
    @brief
    Function switches the cipher log double buffer

    @detail
    The switching of the log buffer is under the cipher logging lock

    @return
    None
*/
/*============================================================================*/
EXTERN lte_pdcp_log_sub_pkt_cipher_pdu_s* lte_pdcpul_log_switch_cipher_log_buf
(
  void
);
/* lte_pdcpul_log_switch_cipher_log_buf() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_get_pdu_ptr

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
EXTERN lte_pdcp_log_pdu_s* lte_pdcpul_log_get_pdu_ptr
(
  lte_rb_cfg_idx_t  rb_cfg_idx,   /*!< rb config index */
  uint16            bytes_to_log  /*!< num bytes to log */
);
/* lte_pdcpul_log_get_pdu_ptr() */


/*===========================================================================

  FUNCTION:  lte_pdcpul_log_pdu

===========================================================================*/
/*!
    @brief
    function for logging the PDCPUL PDU.

    @return
    none

*/
/*=========================================================================*/
EXTERN void lte_pdcpul_log_pdu
(
  lte_rb_cfg_idx_t  rb_cfg_idx,  /*!< rb config index */
  dsm_item_type*    pdu_dsm_ptr, /*!< dsm ptr of pdcp pdu */
  uint16            total_sdu_size, /*!< PDCP SDU size incl hdr*/
  lte_sfn_s         sfn          /*!< sfn when the PDU is txed */
);
/* lte_pdcpul_log_pdu() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_flush_ind_handler

==============================================================================*/
/*!
    @brief
    Handler function for LTE_RLCUL_FLUSH_PDCP_LOG_BUF_IND

    @detail
    This functions submits the pdcp packets to DIAG

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_flush_ind_handler
(
  lte_rb_cfg_idx_t  rb_cfg_idx,  /*!< rb config index */
  uint32            log_code     /*!< log code of the log packet */
);
/* lte_pdcpul_log_flush_ind_handler() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_re_est_rb

==============================================================================*/
/*!
    @brief
    routine needed when a RB is re-established.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_re_est_rb
(
  lte_rb_cfg_idx_t       rb_cfg_idx,        /*!< RB cfg index */
  pthread_mutex_t* const pdu_build_lock_ptr /*!< pdu build lock ptr */
);
/* lte_pdcpul_log_re_est_rb() */


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
EXTERN void lte_pdcpul_log_remove_rb
(
  lte_rb_cfg_idx_t       rb_cfg_idx,        /*!< RB cfg index */
  pthread_mutex_t* const pdu_build_lock_ptr /*!< pdu build lock ptr */
);
/* lte_pdcpul_log_remove_rb() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_add_rb

==============================================================================*/
/*!
    @brief
    routine needed when an RB is added.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_add_rb
(
  lte_rb_cfg_idx_t  rb_cfg_idx  /*!< RB cfg index */
);
/* lte_pdcpul_log_add_rb() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_modify_rb

==============================================================================*/
/*!
    @brief
    routine needed when a RB is modified.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_modify_rb
(
  lte_rb_cfg_idx_t       rb_cfg_idx,        /*!< RB cfg index */
  pthread_mutex_t* const pdu_build_lock_ptr /*!< pdu build lock ptr */
);
/* lte_pdcpul_log_modify_rb() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_flush_timer_expiry_handler

==============================================================================*/
/*!
    @brief
    Handler function for flushing the log buf on log flush timer expiry

    @detail
    This functions submits the pdcp packets to DIAG

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_flush_timer_expiry_handler
(
  lte_rlculi_ctrl_blk_s*  ctrl_blk_ptr  /*!< control block pointer */
);
/* lte_pdcpul_log_flush_timer_expiry_handler() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_deinit

==============================================================================*/
/*!
    @brief
    de-intialization routine for pdcpul log.

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_deinit
(
  void
);
/* lte_pdcpul_log_deinit() */


/*==============================================================================

  FUNCTION:  lte_pdcpul_log_reset_cipher_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Resets the SRB and DRB security configuration for ciphering log packet

    @return
    None
*/
/*============================================================================*/
EXTERN void lte_pdcpul_log_reset_cipher_pdu_security_cfg
(
  void
);
/* lte_pdcpul_log_reset_cipher_pdu_security_cfg() */


#endif /* LTE_RLCUL_PDCPUL_LOG_H */
