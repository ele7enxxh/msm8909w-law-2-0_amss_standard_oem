
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_esm.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
==============================================================================*/

#ifndef _MSG_LIB_DECODE_ESM_H_
#define _MSG_LIB_DECODE_ESM_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"

/*===========================================================================
                             DATE DECLARATIONS
=============================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern boolean decode_esm_msg
(
  byte *src,
  word *pos,
  word *buffer_length,
  word esm_len, 
  lte_nas_incoming_msg_type  *nas_incoming_msg_ptr
);

extern boolean decode_deactivate_eps_bearer_req
(
   byte                                     *src,
   word                                     *pos, 
   word                                     *buffer_length,
   lte_nas_esm_deact_eps_bearer_context_req *deactivate_bearer_context_req,
   word                                     *esm_len_in_bits
 );

extern boolean decode_bearer_resource_alloc_rej
(
   byte                                        *src,
   word                                        *pos, 
   word                                        *buffer_length,   
   lte_nas_esm_bearer_resource_allocation_rej  *bearer_resource_alloc_rej,
   word                                        *esm_len_in_bits
 );

extern boolean decode_bearer_modifi_context_rej
(
   byte                                    *src,
   word                                    *pos, 
   word                                    *buffer_length,   
   lte_nas_esm_bearer_resource_modifi_rej  *bearer_resource_modifi_rej,
   word                                    *esm_len_in_bits
 );

extern boolean decode_pdn_disconnect_rej
(
   byte                                    *src,
   word                                    *pos, 
   word                                    *buffer_length,   
   lte_nas_esm_pdn_disconnect_rej          *pdn_disconnect_rej,
   word                                    *esm_len_in_bits
);

#endif

#endif /*FEATURE_LTE*/
