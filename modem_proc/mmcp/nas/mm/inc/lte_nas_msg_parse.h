/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_msg_parse.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/16/10   hnam    Added a parameter to get_nas_sec_info()
06/10/09   hnam    Changed buffer_length from byte to word
09/28/09   hnam    Moved nas_msg_integrity_check() extern defs here, to avoid compiler warning
04/10/09   hnam    Reverted back buffer length from word to byte
04/10/09   hnam    Removed all the function definitions here, have placed these definitions in other files.
03/18/09   MNK     Added missing parameter in the signature for encode_esm_msg
02/05/09   MNK     Changed the buffer_length in decode_nas_msg to word
==============================================================================*/
#ifndef _lte_nas_msg_parse_H_
#define _lte_nas_msg_parse_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "mm_v.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas_security_info.h"
#include "esm_cm_msg_if.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  ENCODE_NAS_MSG

DESCRIPTION
  This function encodes all the outgoing NAS message and puts it into the OTA buffer

DEPENDENCIES
  None

RETURN VALUE
  word - number of octets encoded

SIDE EFFECTS
  None

===========================================================================*/
extern word encode_nas_msg
(
  lte_nas_outgoing_msg_type *out_buf,
  size_t out_buf_len, 
  byte *ota_buf,
  size_t ota_buf_len,
  lte_nas_ul_security_info_type *ul_nas_security_info
); 

/*===========================================================================

FUNCTION  DECODE_NAS_MSG

DESCRIPTION
  This function decodes all the incoming NAS message and puts it into the 'C' structure

DEPENDENCIES
  None

RETURN VALUE
  byte - number of messages decoded

SIDE EFFECTS
  None

===========================================================================*/
extern byte decode_nas_msg
(
  byte                           *src, 
  word                            buf_len,
  lte_nas_dl_security_info_type  *dl_security_info,
  emm_ctrl_data_type             *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr 
);

extern lte_nas_protocol_discriminator nas_retrieve_msg(byte index,lte_nas_incoming_msg_type *incoming_buffer);

extern lte_nas_sec_info_params_type *get_nas_sec_info(byte *sec_hdr);

extern boolean nas_msg_integrity_check
( 
   byte                          *src, 
   word                          *pos, 
   word                          *buffer_length,
   lte_nas_dl_security_info_type *nas_dl_sec_info,
   boolean                        nas_msg_lib,
   boolean                        smc
);

/*===========================================================================

FUNCTION  GET_DL_NAS_SQN

DESCRIPTION
  This function saves SQN from the received DL message to NAS DL CNT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void get_dl_nas_sqn
(
  byte  *buffer,
  byte  *nas_dl_cnt
);

extern void decode_pdn_conn_req
(
  byte                           *src, 
  word                           buffer_length, 
  esm_pdn_connectivity_req_T     *pdn_conn_req
);

#endif

#endif /*FEATURE_LTE*/
