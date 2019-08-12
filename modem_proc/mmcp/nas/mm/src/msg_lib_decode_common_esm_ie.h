
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_common_esm_ie.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Rel-10, Added declaration for decode_t3396_value
==============================================================================*/

#ifndef _MSG_LIB_DECODE_COMMON_ESM_IE_H_
#define _MSG_LIB_DECODE_COMMON_ESM_IE_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern boolean decode_eps_qos(byte *src,word *pos, word *buffer_length, sdf_qos_T *eps_qos,word *esm_msg_len);

extern boolean decode_apn_addr(byte *src,word *pos, word *buffer_length, apn_T *apn_addr,word *esm_len_in_bits);

extern boolean decode_transaction_id(byte *src,word *pos, word *buffer_length, gsm_umts_transaction_id_T *ti,word *esm_len_in_bits);

extern boolean decode_prot_conf_opt(byte *src,word *pos, word *buffer_length, protocol_cfg_options_T *prot_conf_opt,word *esm_msg_len);

extern boolean decode_apn_ambr(byte *src,word *pos, word *buffer_length, apn_ambr_T *apn_ambr,word *esm_msg_len);

extern boolean decode_qos(byte *src,word *pos, word *buffer_length, qos_T *neg_qos,word *esm_msg_len);

extern boolean decode_pdn_address(byte *src, word *pos, word *buffer_length,pdn_address_T *pdn_address, word *esm_msg_len);

extern boolean decode_tft(byte *src,word *pos, word *buffer_length, tft_type_T *tft,word *esm_msg_len);

#ifdef FEATURE_LTE_REL10
extern boolean decode_t3396_value(byte *src, word *pos, word *buffer_length, timer_3gpp_type3 *backoff_timer, word *esm_len_in_bits);
#endif

#endif

#endif /*FEATURE_LTE*/
