
/*!
  @file
  emm_esm_if_msg.h

  @brief
  EMM internal data structure definitions.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/emm_esm_if_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/10   MNK     1XCSFB introduction 
10/14/09   MNK     Changed the detach cause type in detach cmd
09/28/09   vdr     Removed extern definition of emm_get_nas_transaction_id()
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/03/09   MNK     Added emm_get_pdn_connectivity_req_T message
04/24/09   MNK     Added emm_eps_bearer_status message
04/23/09   zren    Added detach_cmd message
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
===========================================================================*/

#ifndef _EMM_ESM_IF_MSG_H_
#define _EMM_ESM_IF_MSG_H_

#include "mmcp_variation.h"
#include <customer.h>

#ifdef  FEATURE_LTE

#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "queue.h"
#include "lte_nas_esm_message_type.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include "lte_nas.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

#define LTE_ESM_MAX_MESSAGE_LENGTH 1024

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

enum
{
  MSGR_DEFINE_UMID(NAS, EMM, REQ, SERVICE, 0x01, emm_service_req_T ),
  MSGR_DEFINE_UMID(NAS, EMM, REQ, DATA, 0x02, emm_data_req_T ),
  MSGR_DEFINE_UMID(NAS, EMM, CMD, DETACH, 0x03, emm_detach_cmd_T ),
  MSGR_DEFINE_UMID(NAS, EMM, CMD, EPS_BEARER_STATUS, 0x04, emm_eps_bearer_status_cmd_T ),
  MSGR_DEFINE_UMID(NAS, EMM, REQ, 1XCSFB_ESR_CALL, 0x05, emm_1xCSFB_esr_call_req_T ),
  MSGR_DEFINE_UMID(NAS, EMM, REQ, 1XCSFB_ESR_CALL_ABORT , 0x06, emm_1xCSFB_esr_call_abort_req_T ),
#ifdef FEATURE_LTE_REL9
  MSGR_DEFINE_UMID(NAS, EMM, CMD, EMC_SRV_STATUS, 0x07, emm_emc_srv_status_cmd_T)
#endif
} ;

/* 
    This data type is used by the ESM in order to trigger transmition 
    of the Service Request message
*/
typedef struct
{
  msgr_hdr_s  msg_hdr;
  dword       trans_id;
#ifdef FEATURE_LTE_REL9
  boolean     emc_bc; 
#endif
  sys_lte_reestab_calltype_e call_type;
} emm_service_req_T ;

/* 
    This data type is used by the ESM in order to deliver ESM Message to the NW
*/
typedef struct
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  dword emm_transaction_id ;
  lte_nas_esm_outgoing_msg_type esm_out_msg;
} emm_data_req_T ;

/* This data type is used by the ESM in order to trigger MO detach procedure */
typedef struct
{
  msgr_hdr_s              msg_hdr;
  lte_nas_local_cause_enum_T  detach_cause;
} emm_detach_cmd_T ;

/* This data type is used by the ESM to update EMM about the context status whenever there is a change in the staus of any contex
   from active to inactive and vice versa 
*/
typedef struct
{
  msgr_hdr_s                               msg_hdr;
  lte_nas_eps_bearer_context_status_type   context_status;
  lte_nas_isr_status_type                  isr_status;
} emm_eps_bearer_status_cmd_T ;

typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
  lte_csfb_service_type  csfb_service_type;
  boolean                partial_esr;
}emm_1xCSFB_esr_call_req_T; 

typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
}emm_1xCSFB_esr_call_abort_req_T; 

#ifdef FEATURE_LTE_REL9
typedef struct
{
  msgr_hdr_s                   msg_hdr;
  lte_nas_emc_srv_status_type  emc_srv_status;

}emm_emc_srv_status_cmd_T; 
#endif

typedef union
{  
  msgr_hdr_s        msg_hdr; /*!< Message router header */

  /*! @brief Messages sent by the ESM to EMM. These will be part of the ESM cmd type structure in the union part */                                       
  /*! @brief Messages sent by the EMM to REX*/ 
  emm_service_req_T             emm_service_req ;
  emm_data_req_T                emm_data_req ;
  emm_detach_cmd_T              emm_detach_cmd;
  emm_eps_bearer_status_cmd_T   emm_eps_bearer_status;
  emm_1xCSFB_esr_call_req_T     emm_1xCSFB_esr_call_req;
  emm_1xCSFB_esr_call_abort_req_T  emm_1xCSFB_esr_call_abort_req;
#ifdef FEATURE_LTE_REL9
  emm_emc_srv_status_cmd_T      emm_emc_srv_status_cmd;
#endif
} emm_msgr_cmd_type ;


typedef struct

{

  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type. 
                            MSGR expects q_link to be the first element and the msgr_hdr_s 
                            as the second element.So, after link_type no element should be 
                            added and the only element is the union below*/

 
  /*! @brief List all the incoming messages/commands*/ 
  emm_msgr_cmd_type emm_msgr_cmd ;

} emm_cmd_type ;


#endif /* _EMM_ESM_IF_MSG_H_ */
#endif /*FEATURE_LTE*/
