

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/msg_lib_decode_common_emm_ie.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
05/14/10   RI      Mainlined spec changes upto Dec 09.
02/03/09     hnam      Added extern def for decode_guti_ie
==============================================================================*/
#ifndef _MSG_LIB_DECODE_COMMON_EMM_IE_H_
#define _MSG_LIB_DECODE_COMMON_EMM_IE_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "comdef.h"
#include "lte_nas_emm_message_type.h"

/*===========================================================================

                      DATA DECLARATIONS

===========================================================================*/

#define EMM_GUTI_LEN 11
#define MAX_MS_IDENTITY_LEN 8
#define MIN_MS_IDENTITY_LEN 5
#define MAX_BCD_VAL 9
#define MIN_EMERGENCY_NUM_LEN 3
#define MAX_EMERGENCY_NUM_LEN 48
#define MIN_EMERGENCY_NUM_INFO_LEN 2
#define MAX_TAI_LIST_LEN 96
#define MIN_TAI_LIST_LEN 6
#define MAX_TAC_VAL 15
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
extern boolean decode_nw_name(byte *src,word *pos, word *buffer_length, lte_nas_nw_name_type *nw_name);

extern boolean decode_guti_ie(byte *src,word *pos, word *buffer_length, lte_nas_emm_guti_type *guti);

extern boolean decode_ms_identity_ie(byte *src,word *pos, word *buffer_length, gmm_ie_LV_mobile_id_T *ms_id);

extern boolean decode_tai_list_ie(byte *src,word *pos, word *buffer_length, lte_nas_emm_tai_lst_info_type *msg_type);

extern boolean decode_eps_bearer_context_status(byte *src,word *pos, word *buffer_length, lte_nas_eps_bearer_context_status_type *bearer_context_status);

extern void decode_emergency_num_list
(
  byte                           *src,
  word                           *pos, 
  word                           *buffer_length,
  byte                            ie_length, 
  mmr_emergency_num_list_s_type  *emergency_num_list
);

#endif


#endif /*FEATURE_LTE*/
