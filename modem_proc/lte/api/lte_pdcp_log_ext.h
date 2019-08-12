/*!
  @file
  lte_pdcp_log_ext.c

  @brief
   This header file contains the various defines, enums, and other data
  structure types which are necessary for logging binary packets via DIAG.

  @author
  tahmed

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/15   sb      CR 803880: modified PDCP interface for UDC feature
10/18/13   ta      CR 553489: initial version

===========================================================================*/
#ifndef LTE_PDCP_LOG_EXT_H
#define LTE_PDCP_LOG_EXT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include "lte_as.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief PDCP log security config
*/
typedef PACK(struct)
{
  /* Security configuration */
  uint8                 srb_cipher_algo; /*!< SRB Cipher Algo */
  uint8                 srb_cipher_key_index; /*!< SRB Cipher Key index */
  uint8                 srb_integrity_algo; /*!< SRB Integrity Algo */
  uint8                 srb_integrity_key_index; /*!< SRB Integrity Key index */

  uint8                 drb_cipher_algo; /*!< DRB Cipher Algo */
  uint8                 drb_cipher_key_index; /*!< DRB Cipher Key index */

} lte_pdcp_log_security_cfg_s;

/*! @brief PDCP log add/modified RB cfg action
*/
typedef PACK(struct)
{
  uint8  rb_cfg_idx; /*!< RB cfg index */
  uint8  cfg_act; /*!< cfg action (add/modified) */
} lte_pdcp_log_cfg_act_s;

/*! @brief PDCP UL Log RB info definition
*/
typedef PACK(struct)
{
  uint8       rb_id;      /*!< RB ID */
  uint8       rb_cfg_idx; /*!< RB cfg index */
  uint8       eps_id;     /*!< EPS ID */
  uint8       rb_mode;    /*!< RB mode */
  uint8       rb_type;    /*!< RB type */

  uint8       seq_len;    /*!< PDCP seq len in bits (5, 7, 12 bits) */
  uint16      discard_timer; /*!< PDCP discard timer in msec,
                                     LTE_PDCP_DISCARD_TIMER_NOT_CONFIG is used
                                     when discard timer is not configured or
                                     discard timer is infinity */
  uint8       bearer_type; /*!< Bearer Type: DEFAULT(0), ROHC(1), UDC(2) */
  uint8       rohc_max_cid; /*!< ROHC max context ID */
  uint32      rohc_mask;    /*!< ROHC mask */
  uint16      udc_cfg_act;  /*!< UDC Cfg action RELEASE(1), RESET(2), ADD(4) */
  uint16      udc_ctxt_id;  /*!< UDC Prefill context ID (hex)*/
  uint8       udc_algo_ver; /*!< UDC algo ver */
  uint8       udc_hdr_len;  /*!< UDC min header length,
                            To be printed with scaling factor of 20 
                            i.e '20' if hdr_len =1, '40' if hdr_len=2, etc*/

} lte_pdcp_log_ul_info_s;

/*! @brief PDCP DL Log RB info definition
*/
typedef PACK(struct)
{
  uint8    rb_id;      /*!< RB ID */
  uint8    rb_cfg_idx; /*!< RB cfg index */
  uint8    eps_id;     /*!< EPS ID */
  uint8    rb_mode;    /*!< RB mode */
  uint8    rb_type;    /*!< RB type */

  uint8    seq_len; /*!< PDCP seq len in bits (5, 7, 12 bits) */

  uint8    status_report; /*!< PDCP status report enabled or not */
  uint8    bearer_type; /*!< Bearer Type: DEFAULT(0), ROHC(1), UDC(2) */
  uint8    rohc_max_cid; /*!< ROHC max context ID */
  uint32   rohc_mask;    /*!< ROHC mask */

} lte_pdcp_log_dl_info_s;

/*! @brief Structure for PDCP DL Config Log sub packet
*/
typedef PACK(struct)
{
  uint8                 cfg_reason; /*!< configuration reason mask */

  /* Security configuration */
  lte_pdcp_log_security_cfg_s security_cfg;

  uint8                 array_size;   /*!< this is only useful for parser
                                         should be set to LTE_MAX_ACTIVE_RB */
  uint8                 num_released_rb; /*!< number of released RB */
  uint8                 released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                 num_add_modified_rb; /*!< number of add/modified RB */
  lte_pdcp_log_cfg_act_s    add_modified_rb[LTE_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  uint8                 num_active_rb;
  lte_pdcp_log_dl_info_s    rb_info[LTE_MAX_ACTIVE_RB]; /*!< RB config info */
} lte_pdcp_log_sub_pkt_dl_cfg_s;

/*! @brief Structure for PDCP UL Config Log sub packet
*/
typedef PACK(struct)
{
  uint8                 cfg_reason; /*!< configuration reason mask */

  /* Security configuration */
  lte_pdcp_log_security_cfg_s security_cfg;

  uint8                 array_size;   /*!< this is only useful for parser
                                         should be set to LTE_MAX_ACTIVE_RB */
  uint8                 num_released_rb; /*!< number of released RB */
  uint8                 released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                 num_add_modified_rb; /*!< number of add/modified RB */
  lte_pdcp_log_cfg_act_s    add_modified_rb[LTE_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  uint8                 num_active_rb;
  lte_pdcp_log_ul_info_s    rb_info[LTE_MAX_ACTIVE_RB]; /*!< RB config info */
} lte_pdcp_log_sub_pkt_ul_cfg_s;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#endif
