/*!
  @file
  lte_pdcpi.h

  @brief
  Top level PDCP file including all data structures, enumerations etc. internal
  to PDCP module.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/pdcp/src/lte_pdcpi.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/12/15   sb      CR918318: Drop packet instead of asserting when legacy SRB PDU 
                   is decoded as ELS PDU
10/13/14   mg      CR738157: QSH - Memory held in RLC UL buffers
08/19/14   mg      CR711800: Async QSH analysis call in FTM mode
07/14/14   mg      CR665352: PDCP UL statistics API
06/30/14   mg      CR 640102: Log packet of Number of packets with different 
                   delays in PDCP WMks for all bearers
11/19/13   sb      CR535830: PDCP/RLC Re-arch and support of REL-11 features
05/24/13   sb      CR491559: Free metainfo for packets sent for Re-RoHC and reallocate
                   them on recompression of packets 
10/23/12   sb      Code changes for case when RRC explicitly signals that RoHC is enabled
06/15/11   ax      RoHC support
01/26/2009 bq      initial version
==============================================================================*/

#ifndef LTE_PDCPI_H
#define LTE_PDCPI_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <lte_assert.h>
#include <lte_as.h>
#include <msgr.h>
#include <dsm_item.h>
#include <pthread.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*! @brief PDCP status report control PDU type
*/
#define LTE_PDCP_CONTROL_PDU_STATUS_REPORT       0

/*! @brief PDCP ROHC Feedback control PDU type
*/
#define LTE_PDCP_CONTROL_PDU_ROHC_FEEDBACK       1

/*===========================================================================
**                  MACROS
**=========================================================================*/
/*! @brief This macro finds the difference between two sequence numbers.
*/
#define LTE_PDCP_SEQ_DIFF(seq1, seq2, seq_modulus) \
(((seq1) >= (seq2)) ?\
 ((seq1) - (seq2)) :\
 (((seq_modulus) - (seq2)) + (seq1)))

#define LTE_PDCP_SEQ_GE( seq1, seq2, seq_modulus) \
  ((((seq1) - (seq2)) & ((seq_modulus) >> 1)) == 0)

#define LTE_PDCP_COUNT_GE( seq1, seq2) \
  ((((seq1) - (seq2)) & 0x80000000) == 0)

/*! @brief increases the passed in sequence number by 1 bound by the mask
*/
#define LTE_PDCP_SEQ_INC(seq,mask) (((seq)+1) & (mask))

/*! @brief decrement the passed in sequence number by 1 bound by the mask
*/
#define LTE_PDCP_SEQ_DEC(seq,mask) (((seq)+(mask)) & (mask))

/*! @brief PDCP DL statistics, part of the control block
*/
typedef struct
{
  uint32  num_flow_ctrl_trig;   /*!< number of timer flow control trigger */
  uint32  num_data_pdu;         /*!< number of PDCP data pdu received */
  uint32  data_pdu_bytes;       /*!< PDCP data PDU in bytes received */
  uint32  num_control_pdu;      /*!< number of PDCP control pdu received */
  uint32  control_pdu_bytes;    /*!< PDCP control PDU in bytes received */
  uint32  num_control_pdu_gen;   /*!< number of PDCP control pdu generated */
  uint32  control_pdu_gen_bytes; /*!< PDCP control PDU in bytes generated */
  uint32  num_status_report;  /*!< number of status report received */
  uint32  num_rohc_feedback;  /*!< number of ROHC feedback received */
  uint32  num_rohc_fail;      /*!< nmuber of ROHC decompression failed */
  uint32  num_integrity_fail;  /*!< number of integrity check failed */
  uint32  num_miss_pdu_to_upper_layer; /*!< gap to upper layer */
  uint32  num_miss_pdu_from_low_layer; /*!< gap from low layer */
  uint32  num_dup_pdu;       /*!< number of dup PDUs */
  uint32  dup_pdu_bytes;     /*!< dup PDU in bytes */
  uint32  num_out_of_win_pdu;   /*!< number of discard PDUs */
  uint32  out_of_win_pdu_bytes; /*!< discard PDUs bytes */
  uint32  num_invalid_pdu;   /*!< number of invalid PDUs */
  uint32  invalid_pdu_bytes; /*!< invalid PDU in bytes */
  uint32  num_invalid_els_pdus; /*!< number of invalid ELS PDUs */ 
  uint32  invalid_els_pdu_bytes; /*!< invalid ELS PDU in bytes */ 
} lte_pdcpdl_stats_set_s;


/*! @brief Enumeration for PDCP state */
typedef enum
{
  LTE_PDCPI_MODE_NON_FTM = 0, /*!< Non-FTM PDCP mode */
  LTE_PDCPI_MODE_FTM,         /*!< FTM PDCP mode */
  LTE_PDCPI_STATE_MAX         /*!< Max Value for this enum */
} lte_pdcpi_mode_e;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
EXTERN lte_pdcpi_mode_e lte_pdcpdl_get_ftm_mode(void);


#endif /* LTE_PDCPI_H */
