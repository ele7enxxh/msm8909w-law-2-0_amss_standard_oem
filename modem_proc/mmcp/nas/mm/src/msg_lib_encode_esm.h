
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
  START DATE: 02/05/2009*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_encode_esm.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
==============================================================================*/

#ifndef _MSG_LIB_ESM_ENCODE_H_
#define _MSG_LIB_ESM_ENCODE_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_esm_message_type.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
extern void encode_esm_msg(lte_nas_outgoing_msg_type *out_buf, byte *ota_buf, size_t ota_buf_len, word *pos, boolean encoding_for_uicc);

extern lte_nas_esm_outgoing_msg_type* get_esm_container(lte_nas_outgoing_msg_type *out_buf); 

/* ------- ESM ENCODING ------ */

extern void encode_act_def_bearer_context_accept(lte_nas_esm_act_default_bearer_context_accept *act_def_bearer_context_acc, byte *ota_buf, word *pos, word *esm_container_len);

extern void encode_pdn_conn_request(lte_nas_esm_pdn_connectivity_req *pdn_conn_req, byte *ota_buf, size_t ota_buf_len, word *pos, word *esm_container_len,boolean encoding_for_uicc);

extern void encode_eps_bearer_context_rej(lte_nas_esm_act_default_bearer_context_rej *act_def_bearer_cont_rej, byte *ota_buf, word *pos);

extern void encode_eps_bearer_context_accept(lte_nas_esm_act_dedicated_bearer_context_accept *dedicated_bearer_context_acc, byte *ota_buf, word *pos);

extern void encode_dedicated_eps_bearer_context_rej(lte_nas_esm_act_dedicated_bearer_context_rej *dedicated_bearer_cont_rej, byte *ota_buf, word *pos);

extern void encode_bearer_alloc_req(lte_nas_esm_bearer_resource_allocation_req *bearer_alloc_req, byte *ota_buf, word *pos);

extern void encode_bearer_modifi_req(lte_nas_esm_bearer_resource_modifi_req *bearer_modifi_req, byte *ota_buf, word *pos);

extern void encode_deact_bearer_context_accept(lte_nas_esm_deact_eps_bearer_context_accept *deact_bearer_context_acc, byte *ota_buf, word *pos);

extern void encode_esm_info_resp(lte_nas_esm_info_resp *esm_info_resp, byte *ota_buf, word *pos);

extern void encode_pdn_disconnect_req(lte_nas_esm_pdn_disconnect_req *pdn_disconnect_req, byte *ota_buf, word *pos);

#endif

#endif /*FEATURE_LTE*/
