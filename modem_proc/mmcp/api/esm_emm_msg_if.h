/*!
  @file
  esm_emm_msg_if.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/esm_emm_msg_if.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Rel-10, Added struct esm_unblock_all_apns_ind_T
08/25/09   MNK     RRC Interface changes caused ESM-EMM interface chnages for active_eps_ind
08/03/09   MNK     Added emm_get_pdn_connectivity_req_T message
04/24/09   MNK     Added emm_eps_bearer_status message
03/06/09   MNK     Added additional cause field in the esm_failure_ind
==============================================================================*/

#ifndef ESM_EMM_MESSAGES_H
#define ESM_EMM_MESSAGES_H

#include<customer.h>

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include <msgr_lte.h>
#include "lte_nas_common.h"
#include "lte_nas_local_cause.h"
#include "lte_nas_esm_message_type.h"
#include "lte.h"
#include "emm_irat_if_msg.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

typedef struct
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T esm_hdr;

  word            length; /*!< Brief description of this member */
  lte_nas_esm_incoming_msg_type esm_incoming_msg;
} esm_data_ind_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
//typedef struct
//{
//  dword            trans_id; /*!< Brief description of this member */
//  emm_cause_T       emm_cause; /*!< Brief description of this member */
//} esm_reject_ind_T;

typedef struct
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T      esm_hdr;

  dword                  trans_id;          /*!< Brief description of this member */
  lte_nas_local_cause_T  emm_cause;         /*!< Brief description of this member */
  lte_nas_local_cause_T  emm_ota_cause;     /*!< EMM over tha air cause values */

  emm_failure_type       emm_failure_cause;      /*!< EMM failure type and cause */

} esm_failure_ind_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T esm_hdr;

  lte_nas_local_cause_T       emm_cause; /*!< Brief description of this member */
} esm_sig_connection_released_ind_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
#define EPS_MAX_NUM_ACTIVE_EPS_BEARERS 11

typedef struct
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T esm_hdr;

  byte num_active_bearers; /*!< Number of active EPS bearers */
  /*!< All EPS bearers that are currently active, this includes bearers that 
  have been newly added as well as those that are already existing */
  lte_rb_eps_bearer_map_info_s active_eps_bearers[EPS_MAX_NUM_ACTIVE_EPS_BEARERS]; /*!< EPS bearers */

  byte num_added_bearers; /*!< Number of added EPS bearers */
  /*!< EPS bearers that have been newly added */
  lte_rb_eps_bearer_map_info_s added_eps_bearers[EPS_MAX_NUM_ACTIVE_EPS_BEARERS]; 

  byte num_removed_bearers; /*!< Number of removed EPS bearers */
  lte_rb_eps_bearer_map_info_s removed_eps_bearers[EPS_MAX_NUM_ACTIVE_EPS_BEARERS]; /*!< Removed EPS bearers */
} esm_active_eps_ind_T;

typedef struct 
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T esm_hdr;

  lte_nas_local_cause_T cause;
}esm_detach_ind_T;

/* This data type is used by the EMM to inform ESM about the context status received from i
the network MME in the tracking area update accept
*/
typedef struct
{
  msgr_hdr_s                               msg_hdr;
  lte_nas_eps_bearer_context_status_type   context_status;
} esm_eps_bearer_status_ind_T ;

/* Used when EMM requests for PDN connectivity request to be sent at the time of attach*/

typedef struct
{
  msgr_hdr_s  msg_hdr;
  uint8       seq_num;     
  boolean     responding_to_page;
}esm_get_pdn_connectivity_req_T;


/* Used by EMM to inform ESM of either RAT change or ISR status */

typedef struct
{
  msgr_hdr_s                               msg_hdr;
} esm_get_isr_status_ind_T;

typedef struct
{
  msgr_hdr_s                               msg_hdr;
  boolean                                  isr_status;
} esm_isr_status_change_ind_T;



typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
  uint8                  csfb_call_id;
  ie_cm_cc_reject_T      reject;        
}emm_1xCSFB_esr_call_rsp_T; 

typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
  uint8                  csfb_call_id;
}emm_1xCSFB_esr_call_abort_rsp_T; 

/* This data type is used by EMM to instruct ESM to unblock all APNs */
typedef struct
{
  msgr_hdr_s             msg_hdr; /*!< Message router header */
}esm_unblock_all_apns_ind_T;
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#endif /* ESM_EMM_MESSAGES_H */


