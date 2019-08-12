/*!
  @file
  lte_pdcp_ul_if.h

  @brief
  This file contains all External interfaces exported by PDCP UL layer.

  @author
  bqiu


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/inc/lte_pdcp_ul_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/15   sb      CR891958: Add 'Pdu Type' field in 0xB0B3 log packet
06/26/15   sb      CR862079: Change to upgrade PDCP support to
                   ELS spec ver D and UDC ver F
05/29/15   sb      CR844071: Logging change for PDCP Ctrl and UL Cipher PDU
05/15/15   sb      CR 803880: PDCP Changes for UDC feature
06/30/14   mg      CR 640102: Log packet of Number of packets with different 
                   delays in PDCP WMks for all bearers
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
06/06/13   sb      CR495372: Remove LTE_PDCPUL_CRYPTO_DONE_IND
                   message and handle integrity checked SRB SDU synchronously
08/31/11   ax      reduce pdcp-rlc wm low and hi size for bidirectional tput 
01/04/10   ax      move what is needed by FTM under API dir
06/30/10   ar      added PDCP integrity log packet support
05/19/10   ar      added PDCP ciphering log packet support
03/05/10   ax      added option to f3 cipher related info
08/25/09   ax      updated watermark HI, LO and DNE per flow control design
08/21/09   bq      Increase DNE to MAX (need to fix by flow control feature)
08/03/09   bq      RLF implementation
07/16/09   ar      increased log buffer size from 1024 to 2048 bytes
06/15/09   bq      change some WM parameters
04/17/09   ar      added log code to lte_pdcpul_log_submit_pdu_log_pkt api
03/26/09   ar      1 exported UL pdu log macros
                   2 fixed lint warning
03/04/09   bq      add PDCP UL logging APIs for RLCUL
02/23/09   bq      expose PDCP UL logging APIs
02/12/09   bq      expose lte_pdcpul_task_init
07/07/08   bq      Initial version
===========================================================================*/

#ifndef LTE_PDCP_UL_IF_H
#define LTE_PDCP_UL_IF_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <dsm_queue.h>
#include <lte_as.h>
#include <genlog.h>
#include <pthread.h>
#include <a2_common.h>
#include "lte_pdcp_ext_api.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief PDCP UL RLC WM Low: highest BSR(150k) + 10ms*50Mbps
*/
#define LTE_PDCPUL_RLC_WM_LO     30000

/*! @brief PDCP UL RLC WM High: highest BSR(150k) + 20ms*50Mbps
*/
#define LTE_PDCPUL_RLC_WM_HI     45000

/*! @brief PDCP UL RLC WM DNE
    TODO, need to tune later so that DNE will never be crossed over during
   Normal or HO case. It is currently set to a very high level to avoid complex
   handling during HO when there is large amount of data to be retxed
*/
#define LTE_PDCPUL_RLC_WM_DNE    0x7FFFFFFF

/*! @brief
    Maximum number bytes in PDU log packet
*/
#define LTE_PDCP_LOG_PDU_MAX_BUF_SIZE            2048

/*! @brief
    Ciphering and integrity key size in bytes
*/
#define LTE_PDCP_LOG_KEY_SIZE_IN_BYTES           16

/*! @brief
    Number of ciphering pdu payload bytes to log
*/
#define LTE_PDCP_LOG_NUM_CIPH_PAYLOAD_BYTES      2

/*! @brief
    Ciphering pdu log buffer size in bytes
    max pdcp sdus for UE category 3 * num pdcp sdu bytes * log timer duration
*/
#define LTE_PDCP_LOG_CIPHER_PDU_BUF_SIZE \
( 25 * (2+LTE_PDCP_LOG_NUM_CIPH_PAYLOAD_BYTES) * 50 )

/*! @brief Log Mask for PDCP */
#define LTE_PDCP_LOG_DL_CONFIG_MASK              0x00000001

#define LTE_PDCP_LOG_DL_DATA_PDU_MASK            0x00000002

#define LTE_PDCP_LOG_DL_CONTROL_PDU_MASK         0x00000004

#define LTE_PDCP_LOG_DL_CIPHER_DATA_PDU_MASK     0x00000008

#define LTE_PDCP_LOG_DL_STATISTICS_MASK          0x00000010

#define LTE_PDCP_LOG_DL_INTEG_DATA_PDU_MASK      0x00000020

#define LTE_PDCP_LOG_UL_CONFIG_MASK              0x00000040

#define LTE_PDCP_LOG_UL_DATA_PDU_MASK            0x00000080

#define LTE_PDCP_LOG_UL_CONTROL_PDU_MASK         0x00000100

#define LTE_PDCP_LOG_UL_CIPHER_DATA_PDU_MASK     0x00000200

#define LTE_PDCP_LOG_UL_STATISTICS_MASK          0x00000400

#define LTE_PDCP_LOG_UL_INTEG_DATA_PDU_MASK      0x00000800

#define LTE_PDCP_LOG_UL_DELAY_STATISTICS_MASK    0x00001000

#define LTE_PDCP_LOG_INIT_MASK                   0xFFFFFFFF

#ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief This struct defines the format of each PDU logged */
typedef PACK(struct)
{
  uint16 pdu_size;            /*! Size of PDU pkt */
  uint16 logged_bytes;       /*! logged bytes */
  lte_sfn_s sfn;            /*! sfn */
  uint8  type:      2;     /*! PDU type: STATUS(0), ROHC(1), UDC(2) */  
  uint8  reserved1: 6;    /*!< reserved for future use */
} lte_pdcp_log_pdu_s;

/*! @brief This struct defines the format of PDU subpacket */
typedef PACK(struct)
{
  uint8    rb_cfg_idx; /*!< RB cfg index */
  uint8    rb_mode;    /*!< RB mode */
  uint8    seq_len; /*!< PDCP seq len in bits (5, 7, 12 bits) */

  /*! Total Num of PDU logged */
  uint16 num_pdus;
  /*!  Log buffer */
  byte  log_buf[LTE_PDCP_LOG_PDU_MAX_BUF_SIZE];
  /*! Size of the payload, this is not sent cross the wire */
  uint16 size;
} lte_pdcp_log_sub_pkt_pdu_s;

/*! @brief Structure for PDCP cipher/integrity log pdu RB info */
typedef PACK(struct)
{
  uint16    rb_cfg_idx: 6; /*!< rb config index */
  uint16    mode:       1; /*!< rb mode UM(0) or AM(1) */
  uint16    seq_len:    2; /*!< PDCP SN length 5bit(0), 7bit(1), 12bit(2), 15bit(3) */
  uint16    bearer_id:  5; /*!< radio bearer identifier (rb_id-1) */
  uint16    valid_pdu:  1; /*!< 0=invalid pdu, 1=valid pdu) */
  uint16    reserved:   1; /*!< reserved for future use */
}lte_pdcp_log_pdu_rb_info_s;

/*! @brief Structure for PDCP cipher log pdu */
typedef PACK(struct)
{
  lte_pdcp_log_pdu_rb_info_s    rb_info;       /*!< radio bearer cfg info */
  uint16                        pdu_size;      /*!< total size of the pdu */
  uint16                        logged_bytes;  /*!< num pdu bytes logged */
  lte_sfn_s                     sfn;           /*!< system/sub frame info */
  uint32                        count;         /*!< PDCP count */
  uint8                         comp_pdu: 1;   /*!<PDCP compressed pdu FALSE(0), TRUE(1)
                                               if comp_pdu = TRUE, then parse the 
                                               nth byte of log_buf where,
                                               n = 2nd byte if seq_len = 7bits 
                                               n = 3rd byte if seq_len = 12bits or 15bits*/
  uint8                         pdu_type: 2;   /*!< PDU Type DEFAULT(0), ROHC(1), UDC(2) */
  uint8                         reserved: 5;   /*!< reserved for future use */
}lte_pdcp_log_cipher_pdu_s;

/*! @brief Structure for PDCP ciphering Log sub packet */
typedef PACK(struct)
{
  uint8  srb_cipher_key[LTE_PDCP_LOG_KEY_SIZE_IN_BYTES]; /*!< srb cipher key */
  uint8  drb_cipher_key[LTE_PDCP_LOG_KEY_SIZE_IN_BYTES]; /*!< drb cipher key */
  uint8  srb_cipher_algo;                                /*!< srb cipher algo */
  uint8  drb_cipher_algo;                                /*!< drb cipher algo */
  uint16 num_pdus;                                       /*!< num pdus logged */
  uint8  log_buf[LTE_PDCP_LOG_CIPHER_PDU_BUF_SIZE];      /*!< pdu log buffer */
  uint32 size;                                           /*!< payload size */
} lte_pdcp_log_sub_pkt_cipher_pdu_s;

#ifdef _WIN32
   #pragma pack(pop) // Revert alignment to what it was previously
#endif

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_pdcpul_get_ps_pdcp_wm

===========================================================================*/
/*!
    @brief
    This function is called to get the PS-PDCP WM maintained by DS.

    @detail

    @return
    PS-PDCP WM ptr

    @note

    @see

*/
/*=========================================================================*/
extern dsm_watermark_type* lte_pdcpul_get_ps_pdcp_wm
(
  lte_rb_cfg_idx_t    rb_cfg_idx
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_get_pdcp_rlc_wm

==============================================================================*/
/*!
  @brief
    This function is called to get the PDCP-RLC WM maintained by PDCP
    for SRB.

  @detail

  @return
    PDCP-RLC WM ptr

    @note

    @see

*/
/*============================================================================*/
extern dsm_watermark_type* lte_pdcpul_get_pdcp_rlc_wm
(
  lte_rb_cfg_idx_t  rb_cfg_idx
);

/*===========================================================================

  FUNCTION:  lte_pdcpdl_log_submit_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function is called to submit PDU in RLCUL.

    @detail

    @return

    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_log_submit_pdu_log_pkt
(
  lte_rb_cfg_idx_t            rb_cfg_idx,  /*!< rb config index */
  lte_pdcp_log_sub_pkt_pdu_s* log_pdu_ptr, /*!< log pdu buffer ptr */
  log_code_type               log_code     /*!< pdu log code */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_submit_cipher_pdu_log_pkt

===========================================================================*/
/*!
    @brief
    This function submits the CIPHER PDU log pkt.
    The log buffer is then reset to start as an empty buffer for the same stream

    @return
    None

*/
/*=========================================================================*/
extern void lte_pdcpul_log_submit_cipher_pdu_log_pkt
(
  lte_pdcp_log_sub_pkt_cipher_pdu_s *log_pdu_ptr /*!< log pdu buffer ptr */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_log_get_pdu_rb_info

===========================================================================*/
/*!
    @brief
    Function returns the PDCPUL rb info needed by RLC for logging the ciphering
    and integrity pdus

    @return
    lte_pdcp_log_pdu_rb_info_s - rb info bitfield

*/
/*=========================================================================*/
extern lte_pdcp_log_pdu_rb_info_s lte_pdcpul_log_get_pdu_rb_info
(
  lte_rb_cfg_idx_t  rb_cfg_idx  /*!< rb config index */
);

/*==============================================================================

  FUNCTION:  lte_pdcpul_log_set_cipher_pdu_security_cfg

==============================================================================*/
/*!
    @brief
    Sets the SRB and DRB security configuration for ciphering log
    packet

    @return
    None
*/
/*============================================================================*/
extern void lte_pdcpul_log_set_cipher_pdu_security_cfg
(
  void* ciph_buf_ptr /*!< pdcpul cipher buf ptr */
);
/* lte_pdcpul_log_set_cipher_pdu_security_cfg() */

/*===========================================================================

  FUNCTION:  lte_pdcpul_get_ps_pdcp_buffer_size

===========================================================================*/
/*!
  @brief
  This fucntion returns the size of PS-PDCP watermark.

  @detail

  @return
  size of PS-PDCP watermark. 
  0 if PS-PDCP watermark is not registered or PDCP is deactivated. 
*/
/*=========================================================================*/
extern uint32 lte_pdcpul_get_ps_pdcp_buffer_size
(
  lte_rb_cfg_idx_t  rb_cfg_idx  /*!< rb config index */
);

/*===========================================================================

  FUNCTION:  lte_pdcpul_update_delay_stats

===========================================================================*/
/*!
    @brief
    This function is called to update the delay statics for the current DSM item
    and the rb_cfg_idx

    @detail

    @return
    
    @note

    @see

*/
/*=========================================================================*/
extern void lte_pdcpul_update_delay_stats
(
  lte_rb_cfg_idx_t    rb_cfg_idx,
  uint16 discard_tick
);

/*===========================================================================
FUNCTION dsm_is_wm_empty()

DESCRIPTION
 This routine determines whether the input watermark has data queued in
 it or not.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark

RETURN VALUE
 TRUE if watermark has no data queued in it, FALSE if it does

SIDE EFFECTS
 None
===========================================================================*/
extern boolean lte_dsm_is_wm_empty
(
 dsm_watermark_type *wm_ptr
);


#endif /* LTE_PDCP_UL_IF_H */

