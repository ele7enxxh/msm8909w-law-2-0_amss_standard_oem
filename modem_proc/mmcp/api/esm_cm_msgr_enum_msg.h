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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/esm_cm_msgr_enum_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/25/09   MNK     Added BEARER_RESOURCE_MODIFICATION
05/08/09   MNK     Added new internal timer expiration message/signal 
==============================================================================*/

#ifndef ESM_CM_MSGR_MSG_ENUM_H
#define ESM_CM_MSGR_MSG_ENUM_H

#include<customer.h>

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

#include <msgr.h>
#include <msgr_lte.h>
#include <msgr_umid.h>
#include <msgr.h>
#include <msgr_nas.h>

/*enum
  {
    #MSGR_DEFINE _UMID( MYTECH, MYMODULE, CMD, FOO, MYMODULE_ID_FOO,  mymodule_cmd_foo_s )
  };
enum
  {
    MYTECH_MYMODULE_CMD_FOO = MSGR_UMID( MSGR_TECH_MYTECH,
                                         MSGR_MODULE_MYMODULE,
                                         MSGR_TYPE_CMD,
                                         MYMODULE_ID_FOO )
  };
  -# The the UMID will get added the MSGR internal registration tables

  After using #MSGR_DEFINE_UMID in a header file, software can use the symbol
  MYTECH_MYMODULE_CMD_FOO as a UMID in any of the MSGR API's.

*/



/*! @brief UMIDs of the messages sent by the CM to ESM
*/
enum
{
  NAS_ESM_REQ_FIRST = MSGR_UMID_BASE(MSGR_NAS_ESM, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(NAS, ESM, REQ, BEARER_RESOURCE_ALLOC,0x01, esm_bearer_resource_alloc_req_T ),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, BEARER_RESOURCE_REL,0x02, esm_bearer_resource_rel_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, PDN_CONNECTIVTY,0x03, esm_pdn_connectivity_req_T ),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, PDN_DISCONNECT,0x04, esm_pdn_disconnect_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, BEARER_RESOURCE_ALLOC_ABORT,0x05, esm_bearer_resource_alloc_abort_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, PDN_CONNECTIVITY_ABORT,0x06, esm_pdn_connectivity_abort_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, DRB_REESTABLISH,0x07, esm_drb_reestablish_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, CNF, ACT_DEFAULT_BEARER_CONTEXT_ACCEPT,0x08, esm_act_default_bearer_context_accept_T),
  MSGR_DEFINE_UMID(NAS, ESM, IND, ACT_DEFAULT_BEARER_CONTEXT_REJ,0x09, esm_act_default_bearer_context_rej_T),
  MSGR_DEFINE_UMID(NAS, ESM, CNF, ACT_DEDICATED_BEARER_CONTEXT_ACCEPT, 0x0a,esm_act_dedicated_bearer_context_accept_T),
  MSGR_DEFINE_UMID(NAS, ESM, IND, ACT_DEDICATED_BEARER_CONTEXT_REJ, 0x0b,esm_act_dedicated_bearer_context_rej_T),
  MSGR_DEFINE_UMID(NAS, ESM, IND, MODIFY_BEARER_CONTEXT_REJ,0x0c, esm_modify_bearer_context_rej_T),
  MSGR_DEFINE_UMID(NAS, ESM, CNF, MODIFY_BEARER_CONTEXT_ACCEPT, 0x0d, esm_modify_bearer_context_accept_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, BEARER_RESOURCE_MODIFICATION, 0x0e, esm_bearer_resource_modification_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, CNF, PDN_CONNECT_PACKED, 0x0f, esm_pdn_connectivity_packed_cnf_T),

  MSGR_DEFINE_UMID(NAS, ESM, REQ, 1XCSFB_CALL, 0x10, esm_1xCSFB_call_req_T),
  MSGR_DEFINE_UMID(NAS, ESM, REQ, 1XCSFB_ABORT, 0x11, esm_1xCSFB_abort_req_T),

  NAS_ESM_REQ_MAX,
  NAS_ESM_REQ_LAST = NAS_ESM_REQ_MAX - 1
} ;               

/*! @brief UMIDs of the messages sent by ESM to itself
*/
enum
{
  NAS_ESM_TMRI_FIRST = MSGR_UMID_BASE(MSGR_NAS_ESM, MSGR_TYPE_TMRI),

  MSGR_DEFINE_UMID(NAS, ESM, TMRI, TIMER_EXPIRED, 0x01,esm_self_signal_timer_expired_T),
  NAS_ESM_TMRI_MAX,
  NAS_ESM_TMRI_LAST = NAS_ESM_TMRI_MAX - 1
} ;

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* ESM_CM_MSGR_MSG_ENUM_H */


