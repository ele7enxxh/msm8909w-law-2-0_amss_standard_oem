/*!
  @file
  lte_rrc_mhi.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_mhi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/10   amit    Added dlm_processing_active to private data
03/17/10   amit    Added next_mu_id to private data and mu_id to 
                   lte_rrc_mh_ul_msg_book_keeping_s
02/22/10   amit    Moved LTE_RRC_DSM_POOL to lte_rrc_int_msg.h
09/16/09   amit    Removed lint warnings
08/10/09   amit    Added HO substate to differentiate between HO & RLF
11/23/08   amit    Removed the resend field from ul_msg_req
09/11/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_MHI_H
#define LTE_RRC_MHI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <dsm_item.h>
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_mh.h"

/*! @brief Size of UL history buffer
*/
#define LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED 5

/*! @brief Size of DL history buffer
*/
#define LTE_RRC_MH_MAX_NUM_DL_MSG_SAVED 5

/*! @brief Max length of ASN1 encoded UL message
*/
#define LTE_RRC_MH_MAX_UL_ASN1_ENCODED_LENGTH 200

/*! @brief Max length of ASN1 encoded DL message
*/
#define LTE_RRC_MH_MAX_DL_ASN1_ENCODED_LENGTH 200

/*! @brief Last index used in dl_hist
*/
#define LTE_RRC_MH_LAST_DL_HIST_INDEX ((lte_rrc_mh.dd_ptr->next_dl_hist_index + \
LTE_RRC_MH_MAX_NUM_DL_MSG_SAVED - 1) % LTE_RRC_MH_MAX_NUM_DL_MSG_SAVED)

/*! Size of MU_ID in use array.  Size is determined by dividing 
    MU_ID range (256) by the number of MU IDs that can be represented in a
    uint32 bitmap (32) 
*/
#define LTE_RRC_MH_MU_ID_IN_USE_ARRAY_SIZE 8

#define LTE_RRC_MH_NUM_BITS_IN_UNIT32 32

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Enumeration for cause of saving (having book keeping of) a sent 
uplink message
*/
typedef enum
{
  LTE_RRC_MH_SAVE_CAUSE_WT_CNF,  /*!< Waiting for confirmation */
  LTE_RRC_MH_SAVE_CAUSE_HIST,    /*!< Saved in history buffer */
  LTE_RRC_MH_SAVE_CAUSE_BOTH,    /*!< Both of the above */
  LTE_RRC_MH_SAVE_CAUSE_MAX      /*!< Invalid */
} lte_rrc_mh_save_cause_e;

/*! @brief Enumeration for MH sub-state when MH is in HANDOVER (can be HO or RLF)
*/
typedef enum
{
  LTE_RRC_MH_HO_SUBSTATE_HO,  /*!< Handover */
  LTE_RRC_MH_HO_SUBSTATE_RLF, /*!< RLF */
  LTE_RRC_MH_HO_SUBSTATE_MAX  /*!< Invalid */
} lte_rrc_mh_ho_substate_e;

/*! @brief Structure for book keeping of messages sent on uplink. The reason why 
book keeping info of the message is maintained is indicated by the save_cause, which 
can either be waiting for ack (cnf for this message has not been received from 
pdcp) or history (this is one of the last few messages sent on uplink) or both.
*/
typedef struct
{
  boolean valid;                       /*!< Valid or not */
  uint8 mu_id;                         /*!< mu_id used for this message */
  lte_rrc_pdu_buf_s pdu_buf;           /*!< Encoded message */
  lte_rb_id_t rb_id;                   /*!< Radio bearer ID */
  lte_rrc_mh_save_cause_e save_cause;  /*!< Save cause */
  boolean ack_needed;                  /*!< Ack needed or not */
  msgr_umid_type cnf_umid;             /*!< UMID of the confirmation message 
                                            that will be sent (if requested) 
                                            for this message */
} lte_rrc_mh_ul_msg_book_keeping_s;

/*! @brief Structure to maintain a history of sent uplink messages
*/
typedef struct
{
  unsigned char encoded_msg[LTE_RRC_MH_MAX_UL_ASN1_ENCODED_LENGTH];
                  /*!< Encoded message */
  uint32 length;  /*!< Length of the encoded message */
  uint8 bk_index; /*!< Index in the book_keeping array of this message */
} lte_rrc_mh_ul_msg_hist_s;

/*! @brief Structure to maintain a history of received downlink messages
*/
typedef struct
{
  unsigned char encoded_msg[LTE_RRC_MH_MAX_DL_ASN1_ENCODED_LENGTH];
                             /*!< Encoded message */
  uint32 length;             /*!< Length of the encoded message */
  int pdu_num;               /*!< pdu_num of the message */
  lte_rrc_cell_id_s cell_id; /*!< Cell identity, valid (received) only for CCCH 
                                  & PCCH messages */
  lte_sfn_s sfn;             /*!< SFN on which the msg was received, valid 
                                  (received) only for CCCH & PCCH messages */
  lte_rb_id_t rb_id;         /*!< Radio bearer id, valid (received) only for 
                                  DCCH messages */
} lte_rrc_mh_dl_msg_hist_s;

/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  uint8 placeholder;
} lte_rrc_mh_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  /*!< Book keeping of uplink messages. If the array is already full when sending 
       a message on the UL, get rid of one of the old messages in the array which 
       is not in the history buffer and for which ack is not needed. */
  lte_rrc_mh_ul_msg_book_keeping_s ul_msgs_bk[LTE_RRC_MH_MAX_BOOK_KEEPING_MSG];

  /*!< Number of messages cnf is yet to be received for. Basically the number 
       of mu_ids used */
  uint32 num_cnf_due;

  /*!< Next mu_id to be used */
  uint8 next_mu_id;
  
  /*!< Array of bitmap to keep track of MU_ID in use */
  uint32 mu_id_in_use[LTE_RRC_MH_MU_ID_IN_USE_ARRAY_SIZE];

  /*!< Last LTE_RRC_MH_NUM_UL_MSG_SAVED UL messages sent on uplink in ASN1 
       encoded format (with the length and index in ul_msgs_bk[]). This is used 
       as a circular queue. */
  lte_rrc_mh_ul_msg_hist_s ul_hist[LTE_RRC_MH_MAX_NUM_UL_MSG_SAVED];

  /*!< Next index to be used in ul_hist[] */
  uint8 next_ul_hist_index;

  /*!< Last LTE_RRC_MH_NUM_DL_MSG_SAVED DL messages received on downlink. This 
       contains DCCH messages from PDCP, as well as CCCH and PCCH messages from 
       MAC. Maintains other information received with the message like 
       cell_identity, sfn, rb_id, etc. */
  lte_rrc_mh_dl_msg_hist_s dl_hist[LTE_RRC_MH_MAX_NUM_DL_MSG_SAVED];

  /*!< Next index to be used in dl_hist[] */
  uint8 next_dl_hist_index;

  /*!< Pointer to encoded Last DL message received. If the length of the encoded 
       msg is <= LTE_RRC_MH_MAX_DL_ASN1_ENCODED_LENGTH, this points to static
       memory for encoded message in dl_hist[]. Else it is dynamic memory which
       needs to be freed.
       If dynamic memory is used, it is freed when dlm_processing_active becomes FALSE.
       The purpose of storing it is to have the last message received available
       for modules during dlm processing */
  unsigned char *last_dl_msg_ptr;

  /*!< Substate if state is HANDOVER (HO/RLF) */
  lte_rrc_mh_ho_substate_e ho_substate;

  /*!< Flag to indicate if last DL message received is being processed - if yes, 
       DLMs received will be added to pending queue until processing is
       complete */
  boolean dlm_processing_active;
  
  void *failed_ul_unencoded_msg_ptr;

} lte_rrc_mh_dynamic_data_s;

/*! @brief Structure for state-machine per-instance local variables
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_mh_static_data_s *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_mh_dynamic_data_s *dd_ptr;
} lte_rrc_mh_s;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* LTE_RRC_MHI_H */
