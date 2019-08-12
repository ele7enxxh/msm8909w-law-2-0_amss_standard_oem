/*!
  @file
  

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/inc/esm_emm_msgr_enum_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Added support for Rel-10 ESM backoff timer (T3396)
10/07/10   MNK     1XCSFB introduction 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/03/09   MNK     Added emm_get_pdn_connectivity_req_T message
04/24/09   MNK     Added eps_bearer_status message
==============================================================================*/

#ifndef ESM_EMM_MSGR_MSG_ENUM_H
#define ESM_EMM_MSGR_MSG_ENUM_H

#include "mmcp_variation.h"
#include<customer.h>
#ifdef  FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

#include <msgr.h>
#include <msgr_lte.h>
#include <msgr_umid.h>
#include <msgr_nas.h>

/*enum
  {
    #MSGR_DEFINE _UMID( MYTECH, MYMODULE, CMD, FOO, MYMODULE_ID_FOO,  mymodule_cmd_foo_s )
  };
enum
  {
    MYTECH_MYMODULE_CMD_FOO = MSGR_UMID( MSGR_TECH_MYTECH,
                                         MSGR_MODULE_MYMODULE,
                                         :87MSGR_TYPE_CMD,
                                         MYMODULE_ID_FOO )
  };
  -# The the UMID will get added the MSGR internal registration tables

  After using #MSGR_DEFINE_UMID in a header file, software can use the symbol
  MYTECH_MYMODULE_CMD_FOO as a UMID in any of the MSGR API's.

*/


/*! @brief UMIDs of the messages sent by EMM to ESM
*/
enum
{
  NAS_ESM_IND_FIRST = MSGR_UMID_BASE(MSGR_NAS_ESM, MSGR_TYPE_IND),

  MSGR_DEFINE_UMID(NAS, ESM, IND, DATA, 0x01, esm_data_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, FAILURE, 0x02,esm_failure_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, SIG_CONNECTION_RELEASED, 0x03, esm_sig_connection_released_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, ACTIVE_EPS, 0x04, esm_active_eps_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, DETACH, 0x05, esm_detach_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, EPS_BEARER_STATUS, 0x06, esm_eps_bearer_status_ind_T ),
  MSGR_DEFINE_UMID(NAS, ESM, IND, GET_PDN_CONNECTIVITY_REQ, 0x07, esm_get_pdn_connectivity_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, IND, GET_ISR_STATUS, 0x08, esm_get_isr_status_ind_T),
  MSGR_DEFINE_UMID(NAS, ESM, IND, ISR_STATUS_CHANGE, 0x0a, esm_isr_status_change_ind_T),
  MSGR_DEFINE_UMID(NAS, ESM, RSP,1XCSFB_ESR_CALL, 0x0b, emm_1xCSFB_esr_call_rsp_T),
  MSGR_DEFINE_UMID(NAS, ESM, RSP,1XCSFB_ESR_CALL_ABORT, 0x0c, emm_1xCSFB_esr_call_abort_rsp_T),
#ifdef FEATURE_LTE_REL10  
  MSGR_DEFINE_UMID(NAS, ESM, IND, UNBLOCK_ALL_APNS, 0x0d, esm_unblock_all_apns_ind_T),
#endif
  NAS_ESM_IND_MAX,
  NAS_ESM_IND_LAST = NAS_ESM_IND_MAX - 1
} ;
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* ESM_EMM_MSGR_MSG_ENUM_H */

#endif /*FEATURE_LTE*/
