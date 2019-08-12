
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

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 2/5/2009*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_emm.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/21/10   zren    Added EMM/ESM STATUS feature
06/16/09   hnam    Added "lte_nas_dl_security_info_type" in decipher_nas_msg
05/17/09   hnam    Added the code to support security
04/10/09   hnam    Deleted authentication reject function extern definition.
04/10/09   hnam    Added function definitions for existing emm messages
==============================================================================*/

#ifndef _MSG_LIB_DECODE_EMM_H_
#define _MSG_LIB_DECODE_EMM_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_security_info.h"
#include "mm_v.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* Below mentioned function declarations are required for decode_nas_msg*/

extern boolean decode_detach_request(byte *src,word *pos, word *buffer_length, lte_nas_emm_mt_detach_req_type *mt_detach);

extern boolean decode_attach_reject
(
  byte *src,
  word *pos,
  word *buffer_length,
  lte_nas_emm_attach_rej_type *attach_reject,
  lte_nas_incoming_msg_type  *nas_incoming_msg_ptr
);

/*===========================================================================

FUNCTION  DECODE_ATTACH_ACCEPT

DESCRIPTION
  This function decodes ATTACH_ACCEPT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean decode_attach_accept
(
  byte  *src,
  word  *pos, 
  word  *buffer_length, 
  lte_nas_emm_attach_accept_type  *attach_accept,
  struct emm_ctrl_data_tag        *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type       *nas_incoming_msg_ptr
);

extern boolean decode_security_mode_cmd(byte *src,word *pos, word *buffer_length, lte_nas_emm_security_mode_command *security_mode_cmd);

extern boolean decode_service_rej(byte *src,word *pos, word *buffer_length, lte_nas_emm_service_rej_type *serv_rej);

extern boolean decode_authentication_req
(
  byte                 *src,
  word                 *pos, 
  word                 *buffer_length, 
  lte_nas_emm_auth_req *auth_req
);

extern boolean decode_tau_accept
(
  byte                        *src,
  word                        *pos, 
  word                        *buffer_length, 
  lte_nas_emm_tau_accept_type *tau_accept
);

extern boolean decode_tau_reject
(
  byte                *src,
  word                *pos, 
  word                *buffer_length, 
  lte_nas_emm_tau_rej *tau_rej
);

extern boolean decipher_nas_msg
(
   byte                          *src,  
   word                          *buffer_length,
   lte_nas_dl_security_info_type *nas_dl_sec_info
);

extern boolean decode_identity_req
(
  byte                      *src,
  word                      *pos, 
  word                      *buffer_length, 
  lte_nas_emm_identity_req  *identity_req
);

extern boolean decode_guti_realloc_cmd
(
  byte                          *src,
  word                          *pos, 
  word                          *buffer_length, 
  lte_nas_emm_guti_realloc_cmd  *guti_realloc_cmd
);

extern boolean decode_emm_info
(
  byte                          *src,
  word                          *pos, 
  word                          *buffer_length, 
  lte_nas_emm_info              *emm_info
);

extern lte_nas_sec_info_params_type  nas_sec_params;

/*===========================================================================

FUNCTION  DECODE_EMM_STATUS

DESCRIPTION
  This function decodes EMM_STATUS message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_emm_status
(
  byte                  *src,
  word                  *pos, 
  word                  *buffer_length,
  lte_nas_emm_status    *emm_status
);

/*===========================================================================

FUNCTION  DECODE_DOWNLINK_NAS_TRANSPORT

DESCRIPTION
  This function decodes DOWNLINK_NAS_TRANSPORT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/

boolean decode_dl_nas_transport
(
  byte                          *src_ptr,
  word                          *pos_ptr, 
  word                          *buffer_length_ptr, 
  lte_nas_emm_dl_nas_transport  *dl_nas_transport_ptr
);

/*===========================================================================

FUNCTION  DECODE_CS_SERV_NOTIFICATION

DESCRIPTION
  This function decodes CS_SERV_NOTIFICATION message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/

extern boolean decode_cs_serv_notification
(
  byte                              *src,
  word                              *pos, 
  word                              *buffer_length, 
  lte_nas_emm_cs_serv_notification  *cs_serv_notification
);

#if defined (FEATURE_LTE_REL9) 
/*===========================================================================

FUNCTION  DECODE_DL_GENERIC_NAS_TRANSPORT

DESCRIPTION
  This function decodes DL_GENERIC_NAS_TRANSPORT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean decode_dl_generic_nas_transport
(
  byte                                  *src,
  word                                  *pos, 
  word                                  *buffer_length, 
  lte_nas_emm_dl_generic_nas_transport  *dl_generic_nas_transport
);
#endif

#endif

#endif /*FEATURE_LTE*/
