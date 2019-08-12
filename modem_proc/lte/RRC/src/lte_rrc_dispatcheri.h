/*!
  @file lte_rrc_dispatcheri.h

  @brief
  Internal variables of the RRC dispatcher.
  REVISIT: The design of having a static array of state machines to dispatch the messages

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_dispatcheri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/10   np      W to LTE IRAT Mgr, makes it 18
01/14/10   vatsac  Increased LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID to 17
01/09/10   sk      Increased the max number of STMs per message to include
                   To-G RAT MGR
12/21/09   np      Increased max STMs 15 for To W RAT Mgr
10/06/09   np      Increased max STMs 14 for 1x RAT Mgr
09/11/09   sureshs Increased LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID to 13
09/02/09   sk      Increased the max number of STMs per message to include
                   DO RAT MGR
08/24/09   sureshs Increased LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID to 11
02/16/09   ask     Code review comments resolved
01/21/09   vatsac  Increased LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID to 7
10/31/08   ask     Ported RRC to POSIX APIs
10/15/08   amit    Added lte_cphy_mod_prd_boundary_ind_s
10/09/08   sureshs Added lte_cphy_cell_select_cnf_s
10/08/08   amit    Added lte_cphy_mib_ind_s
10/02/08   sureshs Added structs for Acquisition, Band Scan, System Scan 
                   confirmations in external message union
09/15/08   amit    Added external indications lte_mac_rrc_dl_data_ind_s and
                   lte_pdcpdl_sdu_ind_msg_s
09/02/08   sureshs Added start and stop cnf structs to external msg union
07/23/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_DISPATCHERI_H
#define LTE_RRC_DISPATCHERI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stm2.h>

#include "lte_rrc_dispatcher.h"
#include "lte_rrc_pendq.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*!< Size of the dispatch lookup hash table */
#define LTE_RRC_DISPATCHER_HT_SIZE  25

/*!< Max number of state machines a given msg can be dispatched to */
#define LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID 25


/*! @brief Encapsulates information to dispatch one UMID to one or more STMs
*/
typedef struct _lte_rrc_dispatcher_dispatch_entry_s
{
  /*!< Ptr to the next entry (for hash overflow) */
  struct _lte_rrc_dispatcher_dispatch_entry_s *next;    

  /*!< UMID  */
  uint32 msg_id;                         
  
  /*!< Number of state machine entries 
       If 0, the handler_cb will be invoked instead
   */
  uint8 num_sms;

  /*!< List of state machines msg needs to be dispatched to */
  stm_state_machine_t* sm_list[LTE_RRC_DISPATCHER_MAX_NUM_SM_PER_MSGID];

} lte_rrc_dispatcher_dispatch_entry_s;


typedef union
{
  lte_rrc_msg_u                 regular_msg;
  lte_rrc_pending_msg_indi_s    pending_indi;

} lte_rrc_dispatcher_dispatch_msg_u;

/*! @brief Typedef of internal variables of lte_rrc_dispatcher.c
*/
typedef struct
{
  uint8                 external_mb_umid_cnt;    /*!< Num msgs registered for external mb */
  uint8                 externalota_mb_umid_cnt;    /*!< Num msgs registered for externaldlm mb */
  uint8                 internal_mb_umid_cnt;    /*!< Num msgs registered for internal mb */

  uint32                rcvd_cnt;                /*!< Total number of received msgs */
  stm_state_machine_t   *utf_sm_ptr;             /*!< Ptr to the utf sm */
  lte_rrc_dispatcher_dispatch_msg_u *rcv_msg_pr; /*! Ptr to rcvd msgs*/
  lte_rrc_pending_msg_indi_s *pending_msg_indi ;
  
  /*!< @brief Hash table to speed up lookup of dispatch entries */
  lte_rrc_dispatcher_dispatch_entry_s *dispatch_lut[LTE_RRC_DISPATCHER_HT_SIZE];

  msgr_client_t         msgr_rrc_client;

} lte_rrc_dispatcher_s;


extern msgr_umid_type lte_rrc_dispatcher_external_umid_list[];

extern msgr_umid_type lte_rrc_dispatcher_externalota_umid_list[];

extern msgr_umid_type lte_rrc_dispatcher_internal_umid_list[];


/*! @brief Used by the dispatcher to encapsulate messages sent to state machines
*/
typedef struct
{
  msgr_hdr_s *msg_ptr;  /*!< Ptr to the message buffer */
  uint32     msg_len;   /*!< Length of the message buffer */

} lte_rrc_dispatcher_message_s;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

boolean lte_rrc_dispatcher_ht_lookup
(
  uint32 msg_id,                                /*!< Message id (umid), which is the hash key */
  lte_rrc_dispatcher_dispatch_entry_s **entry   /*!< Return ptr to dispatch entry */
);


#endif /* LTE_RRC_DISPATCHERI_H */
