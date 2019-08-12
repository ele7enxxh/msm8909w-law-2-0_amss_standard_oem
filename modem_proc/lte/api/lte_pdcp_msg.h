/*!
  @file
  lte_pdcp_msg.h

  @brief
  This file contains all External message data structure and IDs interfaces
  exported by PDCP layer.


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_pdcp_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/16   sb      CR800409: HFN resync enhancement - FR24395
09/08/15   ar      CR894006: UDC Reset Error Handling
07/23/15   sb      CR867973: Enhance logging and QSH for UDC
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
06/10/15   sb      CR850899: PDCP should send Ctrl PDU to disable UDC when UDC 
                   is released by enB while bearer is still active
05/13/15   sb      CR 803880: modified PDCP interface for UDC feature
09/19/14   sb      CR706535: Defense mechanism in UE to counter HFN mismatch
09/04/14   sb      CR694693: Fix to ensure UL PDUs are not built after RAB
                   is released
08/05/14   sb      CR696130: Reset Ciphering parameter in OFFLOAD when security 
                   parameter changes
04/11/14   mg      CR647594: QSH Phase 1
07/14/14   mg      CR665352: PDCP UL statistics API
07/03/14   mg      CR669056: PDCP discard timer algorithm
04/02/14   sb      CR642230: REL11 RoHC context continuity during HO 
                   within same eNB
01/22/14   md      CR564602: Support for continuing RoHC context 
                   during handover within the same eNB.
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
11/18/13   sb      CR576794: New UL and DL RAB DEREG messages defined 
10/13/13   mg      CR 561786: Downlink PDCP changes for BOLT.
06/30/13   sb      CR495372: Remove LTE_PDCPDL_PEER_PIGGYBACK_ROHC_FB_IND
                   message and handle piggyback RoHC FB synchronously
05/13/13   sb      CR484633: Avoid crash because of PDCPDL message Q full with 
                   CRYPTO_DONE_IND message
06/16/12   sb      CR365196 and CR365004: Fix IOT to handle 
                   scenarios where RB switches back and forth with RoHC enabled and 
                   disabled. Also to reduce the delay in UL after HO
04/20/12   ap      added support for providing the sfn and subfn for DL DCCH
09/06/11   ax      add support for piggyback RoHC feedback
06/15/11   ax      RoHC support
05/16/11   ae      Inclue lte_security.h
03/21/11   mm      Defined ROHC profile mask type
02/23/11   ax      add support for SMC with invalid MAC-I followed by valid SMC
01/31/11   ax      add support for pdcp dl flow control
09/17/10   wshaw   add watchdog support for LTE tasks
04/28/10   ar      added message structures and UMID defs for PDCPUL and PDCPDL
                   START and STOP request and confirm messages
03/03/10   ar      added FTM related msg router messages and msg structures
10/13/09   dd      Changing the Counter RSPs to CNFs
07/20/09   dd      Added pdcp_hdr to the lte_pdcpdl_sdu_ind_msg_s
06/17/09   bq      Add Crypto Done Ind for UL and DL.
05/27/09   bq      Move Security Config from RB level to top level.
05/11/09   bq      Added Security related interface.
04/20/09   dd      Added lte_pdcdl_counter_req_msg_s,
                   lte_pdcdl_counter_rsp_msg_s,
                   lte_pdcul_counter_req_msg_s, lte_pdcul_counter_rsp_msg_s
04/15/09   mm      Removed hand_over field in lte_pdcpul_recfg_prep_req_msg_s
                   and added cfg_reason field in its place. This is for RRC
                   to indicate RLF to PDCP.
04/10/09   bq      add MACRO for LTE_PDCP_DISCARD_TIMER_NOT_CONFIG
07/07/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_MSG_H
#define LTE_PDCP_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <dsm_queue.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include "lte_l2_comdef.h"
#include <lte_security.h>
#include <udcif.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*-----------------------------------------------------------------------*/
/*     LTE PDCP/RRC External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief PDCP seq len in bits
*/
#define LTE_PDCP_SRB_SEQ_LEN 5
#define LTE_PDCP_DRB_SEQ_LEN_7 7
#define LTE_PDCP_DRB_SEQ_LEN_12 12
#define LTE_PDCP_DRB_SEQ_LEN_15 15
#define LTE_PDCP_DRB_AM_SEQ_LEN 12
#define LTE_PDCP_CHK_SUM_BYTES_SIZE 5

/*! @brief PDCP RB Add/Modifed/Remove/Re-establish cfg action
*/
typedef enum
{
  LTE_PDCP_CFG_ACT_INVALID,
  LTE_PDCP_CFG_ACT_ADD,
  LTE_PDCP_CFG_ACT_MODIFIED,
  LTE_PDCP_CFG_ACT_MODIFIED_SUSPEND,    /*!< Action used for modifying then suspending RB */
  LTE_PDCP_CFG_ACT_MODIFIED_RESUME,     /*!< Action used for modifying then resuming RB */
  LTE_PDCP_CFG_ACT_REMOVE,
  LTE_PDCP_CFG_ACT_FULL_CONFIG          /*!< Action used for full configuration - retain 
                                            the EPS id and its DS watermark, delete all
                                            other RB config  */
} lte_pdcp_cfg_act_e;

/*! @brief PDCP DL integrity verification result
*/
typedef enum
{
  LTE_PDCPDL_INTEGRITY_VERIFY_NA,
  LTE_PDCPDL_INTEGRITY_VERIFY_SUCCESS,
  LTE_PDCPDL_INTEGRITY_VERIFY_FAIL
} lte_pdcpdl_integrity_verify_status_e;

/*! @brief PDCP discard timer not configured
*/
#define LTE_PDCP_DISCARD_TIMER_NOT_CONFIG LTE_OTA_IE_INFINITY_UINT16


/*! @brief DRB count info struct
*/

typedef struct
{
  lte_rb_cfg_idx_t    rb_cfg_idx;      /*!< RB Cfg ID */
  uint32 count; /*!< Count*/
} lte_pdcp_count_info_s;

/*!
  @brief
  An enum describing the bit positions in the
  lte_pdcp_rohc_profile_mask_t
  If support of two ROHC profile identifiers with the same 8 LSB's is signalled, 
  only the profile corresponding to the highest value will be applied by RRC.
  */
enum
{
  LTE_PDCP_ROHC_PROFILE_ID_0X0000 = 0x0000,
  LTE_PDCP_ROHC_PROFILE_ID_0X0001 = 0x0001,
  LTE_PDCP_ROHC_PROFILE_ID_0X0002 = 0x0002,
  LTE_PDCP_ROHC_PROFILE_ID_0X0003 = 0x0004,
  LTE_PDCP_ROHC_PROFILE_ID_0X0004 = 0x0008,
  LTE_PDCP_ROHC_PROFILE_ID_0X0006 = 0x0010,
  LTE_PDCP_ROHC_PROFILE_ID_0X0101 = 0x0021,
  LTE_PDCP_ROHC_PROFILE_ID_0X0102 = 0x0042,
  LTE_PDCP_ROHC_PROFILE_ID_0X0103 = 0x0084,
  LTE_PDCP_ROHC_PROFILE_ID_0X0104 = 0x0108,
  /*! UPDATE the value below whenever any of the above bitmasks change */
  LTE_PDCP_ROHC_PROFILE_ID_CURRENT_VALID_MASK_BITS = 0x1FF
} ;

/*! @brief
  ROHC profile mask that indicates what ROHC profiles are supported
*/
typedef uint32 lte_pdcp_rohc_profile_mask_t;

/*!
  @brief An enum describing the bit positions in the
  lte_pdcp_udc_cfg_mask_t */
enum
{
  /* RELEASE will be set in the following conditions
           i) If disableUDC (for all DRBs) is set by the eNB
           ii) If UDC Cfg is being released for individual DRBs*/
  LTE_PDCP_UDC_CFG_MASK_RELEASE = 0x01,
  /* RESET_BUFFER will be set in the following conditions
           i) If resetUDC (for all DRBs) is set by the eNB or 
           ii) If UDC Cfg is being reset for individual DRBs*/
  LTE_PDCP_UDC_CFG_MASK_RESET_BUFFER = 0x02,
  LTE_PDCP_UDC_CFG_MASK_ADD_MOD = 0x04,
  LTE_PDCP_UDC_CFG_MASK_MAX
};

/*! @brief PDCP bearer type */
typedef enum
{
  LTE_PDCP_BEARER_TYPE_DEFAULT = 0, 
  LTE_PDCP_BEARER_TYPE_ROHC = 1,
  LTE_PDCP_BEARER_TYPE_UDC = 2,
  LTE_PDCP_BEARER_TYPE_NONE
} lte_pdcp_bearer_type_e;

/*! @brief PDCP UDC Cfg Mask */
typedef uint16 lte_pdcp_udc_cfg_mask_t;

/*! @brief PDCP UDC release reason type */
typedef enum
{
  LTE_PDCP_UDC_REL_REASON_NONE = 0, /*!< UDC release reason not applicable */
  LTE_PDCP_UDC_REL_REASON_ELS_RECFG = 1,  /*!< UDC cfg released explicitly through ELS Reconfig */
  LTE_PDCP_UDC_REL_REASON_LEGACY = 2, /*!< UDC cfg released implicitly due to legacy messages */
  LTE_PDCP_UDC_REL_REASON_MAX
} lte_pdcp_udc_rel_reason_type_e;


/*! @brief UDC addmod config structure
*/
typedef struct
{ 
  /*! Algo version to be used
  Should be <= algoVersionSupported
  Default value is 1*/
  udc_lib_version_e_type          algo_version;

  /*! UDCMemorySize in KBytes
  Should be <= udcContextMemorySizeMax
  Default value is 4K*/
  udc_context_buf_size_e_type     cntx_buff_size;

  /*! UDCPrefillContextId
  Default value is "Use all zero buffer"*/
  uint16                          prefill_context_id;

  /*! UDCHeader Length In terms of 20 bytes Default value is 4*/
  uint8                           hdr_length;

} lte_pdcp_udc_addmod_cfg_s;

/*! @brief UDC reset config structure
*/
typedef struct
{ 
  /*! If PDCP reset seq number is included in the UDC reset config */
  boolean                         reset_seq_num_present;

  /*! PDCP seq number that failed checksum and caused reset, range (0..4095) 
    Though ASN1 allows PDCP seq num upto 32767, ELS spec states the max allowed
    value is 4095 */
  uint16                          reset_seq_num;

  /*! If PDCP checksum src bytes is included in the UDC reset config */
  boolean                         checksum_src_bytes_present;

  /*! If computed checksum fails, the source bytes from UL Compression memory 
    that were used to compute the checksum; Size 5 bytes */
  uint8                          checksum_src_bytes[LTE_PDCP_CHK_SUM_BYTES_SIZE];

} lte_pdcp_udc_reset_cfg_s;

/*! @brief UDC DRB details
*/
typedef struct
{ 
  /*!< UDC Cfg Mask*/
  lte_pdcp_udc_cfg_mask_t         cfg_mask;

  /*!< Addmod config is applicable only if 
    UDC cfg mask = LTE_PDCP_UDC_CFG_MASK_ADD_MOD */
  lte_pdcp_udc_addmod_cfg_s       udc_addmod_cfg;

  /*!< Reset config is applicable only if 
    UDC cfg mask = LTE_PDCP_UDC_CFG_MASK_RESET_BUFFER */
  lte_pdcp_udc_reset_cfg_s        udc_reset_cfg;

  /*!< UDC release reason, when release reason is not applicable 
    this field will be set to LTE_PDCP_UDC_REL_REASON_NONE */
  lte_pdcp_udc_rel_reason_type_e  udc_rel_reason;
} lte_pdcpul_udc_cfg_s;


/*! @brief PDCP UL RB info definition
*/
typedef struct
{
  lte_rb_id_t    rb_id;      /*!< RB ID */
  lte_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t   eps_id;     /*!< EPS ID */
  lte_rb_rlc_mode_e  rb_mode;    /*!< RB mode */
  lte_rb_type_e  rb_type;    /*!< RB type */

  uint8          seq_len;    /*!< PDCP seq len in bits (5, 7, 12 bits) */
  uint16         discard_timer; /*!< PDCP discard timer in msec,
                                     LTE_PDCP_DISCARD_TIMER_NOT_CONFIG is used
                                     when discard timer is not configured or
                                     discard timer is infinity */

  lte_pdcp_bearer_type_e bearer_type; /*!< Bearer type*/

  uint8          rohc_max_cid; /*!< ROHC max context ID */
  lte_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

  lte_pdcpul_udc_cfg_s  udc_cfg; /*!< ELS UDC Parameters*/  
} lte_pdcpul_info_s;

/*! @brief PDCP DL RB info definition
*/
typedef struct
{
  lte_rb_id_t    rb_id;      /*!< RB ID */
  lte_rb_cfg_idx_t rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t   eps_id;     /*!< EPS ID */
  lte_rb_rlc_mode_e rb_mode;    /*!< RB mode */
  lte_rb_type_e  rb_type;    /*!< RB type */

  uint8          seq_len; /*!< PDCP seq len in bits (5, 7, 12 bits) */

  boolean        status_report; /*!< PDCP status report enabled or not */

  lte_pdcp_bearer_type_e bearer_type; /*!< Bearer type*/ 

  uint16         rohc_max_cid; /*!< ROHC max context ID */

  lte_pdcp_rohc_profile_mask_t  rohc_mask;    /*!< ROHC mask */

} lte_pdcpdl_info_s;

/*! @brief PDCP add/modified RB cfg action
*/
typedef struct
{
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  lte_pdcp_cfg_act_e                  cfg_act; /*!< cfg action (add/modified) */
} lte_pdcp_cfg_act_s;

/* @brief security configuration
*/
typedef struct
{
  /*!< SRB Cipher Algo (NONE/SNOW3G/AES) */
  lte_cipher_algo_e srb_cipher_algo;
  uint8        srb_cipher_key_index; /*!< SRB Cipher Key index */
  uint8        *srb_cipher_key_ptr; /*!< SRB Cipher Key Pointer */

  /*!< SRB Integrity Algo (NONE/SNOW3G/AES)*/
  lte_integrity_algo_e srb_integrity_algo;
  uint8        srb_integrity_key_index; /*!< SRB Integrity Key index */
  uint8        *srb_integrity_key_ptr; /*!< SRB Integrity Key Pointer */

  /*!< DRB Cipher Algo (NONE/SNOW3G/AES) */
  lte_cipher_algo_e drb_cipher_algo;
  uint8        drb_cipher_key_index; /*!< DRB Cipher Key index */
  uint8        *drb_cipher_key_ptr; /*!< DRB Cipher Key Pointer */
} lte_pdcp_security_cfg_s;

/*! @brief configuration request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  lte_rb_cfg_idx_t                    released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                               num_add_modified_rb; /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s                  add_modified_rb[LTE_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  lte_pdcpul_info_s*                  rb_cfg_base_ptr;  /*!< UL RB config data base ptr */

  /*!< Whether security cfg is valid, PDCP will apply the security
       config from RRC only it is set to TRUE */
  boolean                             security_cfg_valid;
  lte_pdcp_security_cfg_s             security_cfg;

  /*!<  Boolean to indciate RoHC-Continue-R11 field */
  boolean                             rohc_reset;

} lte_pdcpul_cfg_req_msg_s;

/*! @brief configuration request confirmation from PDCP UL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_errno_e                         status; /*!< status */
} lte_pdcpul_cfg_cnf_msg_s;

/*! @brief configuration request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  lte_rb_cfg_idx_t                    released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< released RB cfg index */
  uint8                               num_add_modified_rb; /*!< number of add/modified RB */
  lte_pdcp_cfg_act_s                  add_modified_rb[LTE_MAX_ACTIVE_RB]; /*!< cfg action for add/modified RB ID */
  lte_pdcpdl_info_s*                  rb_cfg_base_ptr;  /*!< DL RB config data base ptr */

  /*!< Whether security cfg is valid, PDCP will apply the security
       config from RRC only it is set to TRUE */
  boolean                             security_cfg_valid;
  lte_pdcp_security_cfg_s             security_cfg;

  /*!<  Boolean to indciate RoHC-Continue-R11 field */
  boolean                             rohc_reset;

} lte_pdcpdl_cfg_req_msg_s;

/*! @brief configuration request confirmation from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_errno_e                         status; /*!< status */
} lte_pdcpdl_cfg_cnf_msg_s;

/*! @brief Counter Request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpul_counter_req_msg_s;

/*! @brief Counter Response from PDCPUL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  uint8                               num_active_drb;
                                      /*!< number of active DRB */
  lte_pdcp_count_info_s               drb_count_info[LTE_MAX_ACTIVE_RB];
                                      /*!< DRB info*/
} lte_pdcpul_counter_cnf_msg_s;

/*! @brief Counter Request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_counter_req_msg_s;

/*! @brief Counter Response from PDCPDL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  lte_pdcp_count_info_s               drb_count_info[LTE_MAX_ACTIVE_RB];
                                      /*!< DRB info*/
  uint8                               num_active_drb;
                                      /*!< number of active DRB */
} lte_pdcpdl_counter_cnf_msg_s;

/*! @brief Enter Factory Test Mode(FTM) Request from FTM module to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpul_enter_ftm_req_msg_s;

/*! @brief Enter Factory Test Mode(FTM) confirm from PDCPUL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpul_enter_ftm_cnf_msg_s;

/*! @brief Enter Factory Test Mode(FTM) Request from FTM module to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_enter_ftm_req_msg_s;

/*! @brief Enter Factory Test Mode(FTM) confirm from PDCPDL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_enter_ftm_cnf_msg_s;

/*! @brief Exit Factory Test Mode(FTM) Request from FTM module to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpul_exit_ftm_req_msg_s;

/*! @brief Exit Factory Test Mode(FTM) confirm from PDCPUL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpul_exit_ftm_cnf_msg_s;

/*! @brief Exit Factory Test Mode(FTM) Request from FTM module to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_exit_ftm_req_msg_s;

/*! @brief Exit Factory Test Mode(FTM) confirm from PDCPDL to FTM module
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_exit_ftm_cnf_msg_s;

/*! @brief PDCPDL SDU Type
*/
typedef enum
{
  LTE_PDCP_SDU_SPEC,
  LTE_PDCP_SDU_ELS,
  LTE_PDCP_SDU_MAX
} lte_pdcp_sdu_type_e;

/*! @brief signal data (SDU) request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach;
                                      /*!< dsm attachment
                                      contained signal message */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  uint8                               mu_id; /*!< message unit identifier */

  /* Security Info. */
  boolean                             integrity_protect;  /*! need integrity protection. */
  boolean                             cipher; /*! need cipher. */
  
  /*!< Enum to indicate PDCP UL SDU type*/
  lte_pdcp_sdu_type_e                 sdu_type;
  
} lte_pdcpul_sdu_req_msg_s;

/*! @brief request from RRC to reset security state
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
} lte_pdcpdl_sec_reset_req_msg_s;

/*! @brief Number of different ranges of PDCP UL delays
*/
#define LTE_PDCP_DELAY_STATS_SIZE               21

/*! @brief PDCP UL statistics for UDC
  */
typedef struct
{
  uint32  num_udc_comp_pkts; /*!<*total num of UDC compressed pkts*/
  uint32  udc_comp_bytes; /*!<*total num of UDC compressed  bytes*/  
  uint32  num_udc_fc_uncomp_pkts; /*!< total num of UDC uncompressed pkts*/
  uint32  udc_fc_uncomp_bytes; /*!<*total num of UDC non-compressed  bytes*/
  uint32  num_udc_fail;  /*!< Number of UDC compression failed*/
  uint16  num_udc_reset; /*!< Num of time UDC RESET is called */
  uint32  num_ue_udc_ctrl_pdus;      /*!< num of UE triggered UDC Ctrl PDUs */
  uint32  num_enb_udc_ctrl_pdus;      /*!< num of eNB triggered UDC Ctrl PDUs */
  uint32  num_udc_dsm_fc_down;         /*!< num of  CMD_DOWN for DSM monitor*/
  uint32  num_udc_dsm_fc_shutdown;        /*!< num of  CMD_SHUT_DOWN for DSM monitor*/
  uint32  num_udc_cpu_fc_down;         /*!< num of  CMD_DOWN for CPU monitor*/
  uint32  num_udc_cpu_fc_freeze;           /*!< num of  CMD_UP for CPU monitor*/
  uint32  num_enb_trigg_udc_reset;  /*!<num of UDC reset triggered by enB during non-HO/non-RLF*/
}lte_pdcpul_udc_stats_s;

/*! @brief PDCP UL statistics, part of the control block
  */
typedef struct
{
  uint32  num_flow_ctrl_trig;  /*!< number of timer flow disable */
  uint32  num_data_pdu;        /*!< number of PDCP pdu transmitted */
  uint32  data_pdu_bytes;      /*!< PDCP PDU in bytes transmitted */
  uint32  data_bytes_from_ps;  /*!< Total bytes dequeued from PS Wm*/
  uint32  num_control_pdu;     /*!< number of PDCP pdu transmitted */
  uint32  control_pdu_bytes;   /*!< PDCP PDU in bytes transmitted */
  uint32  num_status_report;  /*!< number of status report transmitted */
  uint32  num_rohc_feedback;  /*!< number of ROHC feedback transmitted */
  uint32  num_rohc_fail;      /*!< nmuber of ROHC compression failed */
  uint32  num_discard_sdu;   /*!< number of time based discard SDUs */
  uint32  discard_sdu_bytes; /*!< time based discard SDUs bytes */
  uint32  num_retransmit_sdu;  /*!< number of retransmit SDUs */
  uint32  retransmit_sdu_bytes; /*!< retransmit SDU in bytes */
  uint32  num_piggyback_rohc_feedback_rxed;  /*!< number of piggyback ROHC 
                                                  feedback rxed */
  uint32  num_rerohc_pdu_drop;  /*!< number of RoHC PDUs drop during HO*/
  uint32  rerohc_pdu_drop_bytes; /*!< number of RoHC PDU bytes drop during HO*/
  uint32  pdcp_delay_pkt_cnt[LTE_PDCP_DELAY_STATS_SIZE]; /*number of packets
  for each delay range bin. Last bin conatins packets having delay greater
  than a range*/
  lte_pdcpul_udc_stats_s udc_stats;  /*!< Stats related to UDC*/
} lte_pdcpul_stats_set_s;

/*! @brief UL RB statistics
*/
typedef struct
{
  lte_eps_id_t eps_id;
  lte_pdcpul_stats_set_s rb_stats;
}lte_pdcpul_rb_stats_s;

typedef struct
{
  lte_pdcpul_rb_stats_s rb_metric_stats[LTE_MAX_ACTIVE_DRB];
}lte_pdcpul_qual_metrics_s;

/*! @brief UL data metric statistics
*/
typedef struct
{
  msgr_hdr_struct_type   hdr;          /*!< common message router header */
  lte_eps_id_t eps_id;                     /*!< eps bearer for which info is requested*/
  lte_pdcpul_qual_metrics_s* rb_stats_ptr; /*!< Pointer to the allocated memory where L2 will return the requested DRB stats*/
}lte_pdcpul_rb_stats_req_msg_s;

#define LTE_PDCP_MAX_MUID_SIZE_IN_MSG 16

/*! @brief SDU request confirmation from PDCP UL to RRC for AM SRB
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  uint8                               num_ack_muid; /*!< number of ack MU ID */
  uint8                               ack_muid[LTE_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< ack MU ID array */
  uint8                               num_nack_muid; /*!< number of nack MU ID */
  uint8                               nack_muid[LTE_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< nack MU ID array */
  uint8                               num_maybe_muid; /*!< number of maybe transmitted MU ID */
  uint8                               maybe_muid[LTE_PDCP_MAX_MUID_SIZE_IN_MSG]; /*!< maybe transmitted MU ID array */
} lte_pdcpul_sdu_cnf_msg_s;

/*! @brief reconfiguration prep request from RRC to PDCPUL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  /*!< configuration reason mask - The applicable values are LTE_L2_CFG_HO,
    LTE_L2_CFG_RELEASE and LTE_L2_CFG_RADIO_FAILURE. The other config reasons
    are not applicable for this request */
  lte_l2_cfg_reason_e                 cfg_reason; /*!< configuration reason mask */
  uint8                               num_released_rb; /*!< number of released RB */
  lte_rb_cfg_idx_t                    released_rb_cfg_idx[LTE_MAX_ACTIVE_RB]; /*!< released RB cfg index */
} lte_pdcpul_recfg_prep_req_msg_s;

/*! @brief reconfiguration prep request confirmation from PDCP UL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_errno_e                         status; /*!< status */
} lte_pdcpul_recfg_prep_cnf_msg_s;

/*! @brief reconfiguration prep request from RRC to PDCPDL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  boolean                             hand_over; /*!< handover or not: TRUE for handover */
} lte_pdcpdl_recfg_prep_req_msg_s;

/*! @brief reconfiguration prep request confirmation from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  /* Message payload */
  lte_errno_e                         status; /*!< status */
} lte_pdcpdl_recfg_prep_cnf_msg_s;

/*! @brief signal message (SDU) indication from PDCP DL to RRC
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment contained signal message */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */

  /* Security related info. */
  uint8                               pdcp_hdr;    /*!<PDCP PDU header*/
  /*The above hdr is being sent separately to RRC because it is part of the
  input to the integrity check function*/
  uint32                              count;      /*!< PDCP count */
  uint8                               mac_i[4];   /*!< MAC_I */

  /*!< Integrity Verfication Result */
  lte_pdcpdl_integrity_verify_status_e     integrity_status;
  lte_sfn_s                           sfn;        /*!< The system and subframe number */

  /*!< Enum to indicate PDCP DL SDU type*/
  lte_pdcp_sdu_type_e                 sdu_type;
  
} lte_pdcpdl_sdu_ind_msg_s;

/*-----------------------------------------------------------------------*/
/*     LTE PDCP/PS External message definitions                          */
/*-----------------------------------------------------------------------*/
/*! @brief RAB register request from PS to PDCP UL
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_eps_id_t                        eps_id;      /*!< EPS ID */
  void *                              ps_pdcp_wm_ptr; /*!< PS to PDCP WM ptr */
} lte_pdcpul_rab_register_req_msg_s;

/*! @brief RAB register request confirmation from PDCP UL to PS
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_eps_id_t                        eps_id;      /*!< EPS ID */
  lte_errno_e                         status;      /*!< status */
} lte_pdcpul_rab_register_cnf_msg_s;

/*! @brief RAB register request from PS to PDCP DL to let PDCP DL register PDCP DL
     to A2 WM with A2
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_eps_id_t                        eps_id;      /*!< EPS ID */
  dsm_watermark_type*                 pdcp_wm_ptr; /*!< PS to PDCP WM ptr */
} lte_pdcpdl_rab_register_req_msg_s;

/*! @brief RAB register request confirmation from PDCP DL to PS
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_eps_id_t                        eps_id;      /*!< EPS ID */
  lte_errno_e                         status;      /*!< status */
} lte_pdcpdl_rab_register_cnf_msg_s;

/*! @brief RAB deregister request from PS to PDCP  to let PDCP DL/UL deregister
     DL/UL WM
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
 lte_eps_id_t                        eps_id;      /*!< EPS ID */
} lte_pdcp_rab_deregister_req_msg_s;

/*! @brief RAB deregister request confirmation from PDCP DL/UL to PS
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  lte_eps_id_t                        eps_id;      /*!< EPS ID */
  lte_errno_e                         status;      /*!< status */
} lte_pdcp_rab_deregister_cnf_msg_s;

/*! @brief LTE_PDCPUL_HFN_UPDATE_REQ request message definition
*/

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB Cfg index */
} lte_pdcpul_conn_reestab_req_msg_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  lte_eps_id_t eps_id; /*!< eps id  */
  boolean prev_resync_passed; /*!< boolean to indicate if UE recovered from
                             previous HFN mismatch*/
} lte_pdcpdl_hfn_update_req_msg_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  lte_rb_cfg_idx_t       rb_cfg_idx; /*!< RB Cfg index */
} lte_pdcpdl_hfn_state_reset_req_msg_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;    /*!< common message router header */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
  lte_errno_e                         status;      /*!< status */
} lte_pdcpdl_hfn_update_cnf_msg_s;


/*-----------------------------------------------------------------------*/
/*     LTE PDCP DL/PDCP UL message definitions                          */
/*----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type                hdr;  /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
  /* dsm attachment contained PDCP peer control message from EnodeB */
} lte_pdcpdl_peer_ctrl_ind_msg_s;

/*! @brief message indication of piggyback RoHC feedback received from eNB
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;        /*!< message header */
  msgr_attach_s                       dsm_attach; /*!< dsm attachment */
  uint16                              fb_cid;     /*!< feedback context ID */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB Cfg index */
} lte_pdcpdl_peer_pb_rohc_fb_ind_msg_s;

/*! @brief indication that is triggered when PDCP to A2 low watermark crosssed
*/
typedef struct
{
  msgr_hdr_struct_type                msg_hdr;    /*!< message header */
  lte_eps_id_t                        eps_id; /*!< EPS ID */
} lte_pdcpdl_wm_low_ind_msg_s;

/*! @brief LTE_PDCPXX_START_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  boolean tune_away_enabled;        /*!< TRUE indicates TA can happen */
} lte_pdcp_start_req_msg_s;

/*! @brief LTE_PDCPXX_START_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  lte_errno_e            cnf_status; /*!< confimation status message */
} lte_pdcp_start_cnf_msg_s;

/*! @brief LTE_PDCPXX_STOP_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;  /*!< common message router header */
} lte_pdcp_stop_req_msg_s;

/*! @brief LTE_PDCPXX_STOP_CNF request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
  lte_errno_e            cnf_status; /*!< confimation status message */
} lte_pdcp_stop_cnf_msg_s;

/*! @brief LTE_PDCPXX_QSH_REQ request message definition
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;    /*!< common message router header */
    /*! whether to run analysis, reset stats ... */
  uint32 action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} lte_pdcp_qsh_analysis_req_msg_s;

/*! @brief UL data Stats CNF message
*/
typedef struct
{
  msgr_hdr_struct_type   msg_hdr;     /*!< common message router header */
  lte_errno_e            cnf_status;  /*!< confirmation status message  - SUCCESS/FAILURE*/
  uint8                  num_report;  /*!<number of reported structure(s) (one report per DRB).*/
}lte_pdcpul_rb_stats_cnf_msg_s;

/*! @brief indication that is triggered by PDCP to request RRC to trigger RLF
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
} lte_pdcpul_rlf_ind_msg_s;

/*! @brief indication that is triggered by PDCP to request RRC to trigger Conn rel
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
} lte_pdcpul_conn_rel_ind_msg_s;

/*! @brief RAB register IND from COMP to PDCP  for UL Data
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;         /*!< message header */
  /* Message payload */
  lte_rb_cfg_idx_t                    rb_cfg_idx; /*!< RB cfg index */
  lte_eps_id_t                        eps_id;     /*!< EPS ID */
  boolean                             send_ctrl_pdu; /*!< Send UDC ctrl PDU*/
} lte_pdcpul_rab_reg_ind_msg_s;

/*-----------------------------------------------------------------------*/
/*     LTE PDCPUL External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the LTE PDCPUL module receives.  */
enum
{
  LTE_PDCPUL_FIRST_REQ = MSGR_UMID_BASE(MSGR_LTE_PDCPUL, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,CFG,0x00,
                   lte_pdcpul_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,SDU,0x01,
                   lte_pdcpul_sdu_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,RECFG_PREP,0x02,
                   lte_pdcpul_recfg_prep_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,RAB_REGISTER,0x03,
                   lte_pdcpul_rab_register_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,COUNTER,0x04,
                   lte_pdcpul_counter_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,ENTER_FTM,0x05,
                   lte_pdcpul_enter_ftm_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,EXIT_FTM,0x06,
                   lte_pdcpul_exit_ftm_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,START,0x07,
                   lte_pdcp_start_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,STOP,0x08,
                   lte_pdcp_stop_req_msg_s),  
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,COMP_ALLOC,0x09,
                   lte_pdcpul_comp_cfg_req_msg_s), 
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,COMP_FREE,0x0a,
                   lte_pdcpul_comp_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,COMP_PKT,0x0b,
                   lte_pdcpul_comp_pkt_req_msg_s),  
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,ROHC_RESET,0x0c,
                   lte_pdcpul_comp_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,RAB_DEREGISTER,0x0d,
                   lte_pdcp_rab_deregister_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,RB_STATS,0x0e,
                   lte_pdcpul_rb_stats_req_msg_s),                   
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,OFFLOAD_RAB_DEREGISTER,0x0f,
                   lte_pdcp_offload_rab_dereg_ind_msg_s),    
  MSGR_DEFINE_UMID(LTE,PDCPUL,REQ,CONN_REESTAB,0x10,
                   lte_pdcpul_conn_reestab_req_msg_s),                    
  LTE_PDCPUL_MAX_REQ,
  LTE_PDCPUL_LAST_REQ = LTE_PDCPUL_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPDL External Request messages                              */
/*-----------------------------------------------------------------------*/
/*! @brief external requests that the LTE PDCPDL module sends/receives.  */
enum
{
  LTE_PDCPDL_FIRST_REQ = MSGR_UMID_BASE(MSGR_LTE_PDCPDL, MSGR_TYPE_REQ),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,CFG,0x00,
                   lte_pdcpdl_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,RECFG_PREP,0x01,
                   lte_pdcpdl_recfg_prep_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,RAB_REGISTER,0x02,
                   lte_pdcpdl_rab_register_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,COUNTER,0x03,
                   lte_pdcpdl_counter_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,ENTER_FTM,0x04,
                   lte_pdcpdl_enter_ftm_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,EXIT_FTM,0x05,
                   lte_pdcpdl_exit_ftm_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,START,0x06,
                   lte_pdcp_start_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,STOP,0x07,
                   lte_pdcp_stop_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,FC,0x08,
                   cfm_fc_cmd_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,SEC_RESET,0x09,
                   lte_pdcpdl_sec_reset_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,DECOMP_ALLOC,0x0a,
                   lte_pdcpdl_decomp_cfg_req_msg_s), 
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,DECOMP_FREE,0x0b,
                   lte_pdcpdl_decomp_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,DECOMP_PKT,0x0c,
                   lte_pdcpdl_decomp_pkt_req_msg_s),  
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,A2_RAB_DEREGISTER,0x0d,
                   lte_pdcpdl_a2_rab_deregister_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,RAB_DEREGISTER,0x0e,
                   lte_pdcp_rab_deregister_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,ROHC_RESET,0x0f,
                   lte_pdcpdl_decomp_cfg_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,QSH_ANALYSIS,0x10,
                   lte_pdcp_qsh_analysis_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,CIPH_RESET,0x11,
                   lte_pdcpdl_ciph_reset_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,HFN_UPDATE,0x12,
                   lte_pdcpdl_hfn_update_req_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,REQ,HFN_STATE_RESET,0x13,
                   lte_pdcpdl_hfn_state_reset_req_msg_s),
  LTE_PDCPDL_MAX_REQ,
  LTE_PDCPDL_LAST_REQ = LTE_PDCPDL_MAX_REQ - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPUL External Confirmation messages                         */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the LTE PDCPUL module sends  */
enum
{
  LTE_PDCPUL_FIRST_CNF = MSGR_UMID_BASE(MSGR_LTE_PDCPUL, MSGR_TYPE_CNF),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,CFG,0x00,
                   lte_pdcpul_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,RECFG_PREP,0x01,
                   lte_pdcpul_recfg_prep_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,SDU,0x02,
                   lte_pdcpul_sdu_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,RAB_REGISTER,0x03,
                   lte_pdcpul_rab_register_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,COUNTER,0x04,
                   lte_pdcpul_counter_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,ENTER_FTM,0x05,
                   lte_pdcpul_enter_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,EXIT_FTM,0x06,
                   lte_pdcpul_exit_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,START,0x07,
                   lte_pdcp_start_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,STOP,0x08,
                   lte_pdcp_stop_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,RAB_DEREGISTER,0x09,
                   lte_pdcp_rab_deregister_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,CNF,RB_STATS,0x0a,
                   lte_pdcpul_rb_stats_cnf_msg_s),
  LTE_PDCPUL_MAX_CNF,
  LTE_PDCPUL_LAST_CNF = LTE_PDCPUL_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPDL External Confirmation messages                         */
/*-----------------------------------------------------------------------*/
/*! @brief external Confirmations that the LTE PDCPDL module sends  */
enum
{
  LTE_PDCPDL_FIRST_CNF = MSGR_UMID_BASE(MSGR_LTE_PDCPDL, MSGR_TYPE_CNF),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,CFG,0x00,
                   lte_pdcpdl_cfg_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,RECFG_PREP,0x01,
                   lte_pdcpdl_recfg_prep_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,RAB_REGISTER,0x02,
                   lte_pdcpdl_rab_register_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,COUNTER,0x03,
                   lte_pdcpdl_counter_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,ENTER_FTM,0x04,
                   lte_pdcpdl_enter_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,EXIT_FTM,0x05,
                   lte_pdcpdl_exit_ftm_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,START,0x06,
                   lte_pdcp_start_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,STOP,0x07,
                   lte_pdcp_stop_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,RAB_DEREGISTER,0x08,
                   lte_pdcp_rab_deregister_cnf_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,CNF,HFN_UPDATE,0x09,
                   lte_pdcpdl_hfn_update_cnf_msg_s),
  LTE_PDCPDL_MAX_CNF,
  LTE_PDCPDL_LAST_CNF = LTE_PDCPDL_MAX_CNF - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPDL External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE PDCPDL module sends  */
enum
{
  LTE_PDCPDL_FIRST_IND = MSGR_UMID_BASE(MSGR_LTE_PDCPDL, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,SDU,0x00,
                   lte_pdcpdl_sdu_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,PEER_CTRL,0x01,
                   lte_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,TX_STATUS,0x02,
                   lte_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,TX_ROHC_FB,0x03,
                   lte_pdcpdl_peer_ctrl_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,WM_LOW,0x04,
                   lte_pdcpdl_wm_low_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPDL,IND,PEER_ROHC_FB,0x05,
                   lte_pdcpdl_peer_ctrl_ind_msg_s),
  LTE_PDCPDL_MAX_IND,
  LTE_PDCPDL_LAST_IND = LTE_PDCPDL_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPUL External Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief external Indications that the LTE PDCPUL module sends         */
enum
{
  LTE_PDCPUL_FIRST_IND = MSGR_UMID_BASE(MSGR_LTE_PDCPUL, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(LTE,PDCPUL,IND,RL_FAIL, 0x00, 
                   lte_pdcpul_rlf_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,IND,CONN_REL, 0x01, 
                   lte_pdcpul_conn_rel_ind_msg_s),
  MSGR_DEFINE_UMID(LTE,PDCPUL,IND,RAB_REG, 0x02, 
                   lte_pdcpul_rab_reg_ind_msg_s),
/* Temporary. Remove after Posix support for signals*/
  LTE_PDCPUL_MAX_IND,
  LTE_PDCPUL_LAST_IND = LTE_PDCPUL_MAX_IND - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPUL Internal timer Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief Internal timer Indications that the LTE PDCPUL module receives  */
enum
{
  LTE_PDCPUL_FIRST_TMRI = MSGR_UMID_BASE(MSGR_LTE_PDCPUL, MSGR_TYPE_TMRI),
  MSGR_DEFINE_UMID(LTE,PDCPUL,TMRI,DISCARD_TIMER_TICK_EXPIRED,0x00,(void*)),
  MSGR_DEFINE_UMID(LTE,PDCPUL,TMRI,LOG_STATS_FLUSH_TIMER_EXPIRED,0x01,(void*)),
  MSGR_DEFINE_UMID(LTE,PDCPUL,TMRI,WDOG_TIMER_EXPIRED,0x02,(void*)),
  #ifdef FEATURE_LTE_UDC_ENABLED
  MSGR_DEFINE_UMID(LTE,PDCPUL,TMRI,UDC_RESET_TIMER_EXPIRED,0x3,(void*)),
  #endif /*FEATURE_LTE_UDC_ENABLED*/
  LTE_PDCPUL_MAX_TMRI,
  LTE_PDCPUL_LAST_TMRI = LTE_PDCPUL_MAX_TMRI - 1
};

/*-----------------------------------------------------------------------*/
/*     LTE PDCPDL Internal timer Indication messages                           */
/*-----------------------------------------------------------------------*/
/*! @brief Internal timer Indications that the LTE PDCPDL module receives  */
enum
{
  LTE_PDCPDL_FIRST_TMRI = MSGR_UMID_BASE(MSGR_LTE_PDCPDL, MSGR_TYPE_TMRI),
  MSGR_DEFINE_UMID(LTE,PDCPDL,TMRI,LOG_FLUSH_TIMER_EXPIRED,0x00,(void*)),
  MSGR_DEFINE_UMID(LTE,PDCPDL,TMRI,WDOG_TIMER_EXPIRED,0x01,(void*)),
  LTE_PDCPDL_MAX_TMRI,
  LTE_PDCPDL_LAST_TMRI = LTE_PDCPDL_MAX_TMRI - 1
};

/*! @brief Supervisory messages that PDCP DL receives or throws
*/

enum
{
  MSGR_DEFINE_UMID(LTE, PDCPDL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, PDCPDL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, PDCPDL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(LTE, PDCPDL, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL,
                   none)
};

/*! @brief Supervisory messages that PDCP UL receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(LTE, PDCPUL, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, PDCPUL, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, PDCPUL, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY,
                   none)
};

#endif /* LTE_PDCP_MSG_H */
