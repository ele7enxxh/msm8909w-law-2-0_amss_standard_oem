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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/msg_lib_encode_common_emm_ie.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
08/01/11     rajesh    Added extern def for encode_eps_bearer_context_status
03/20/09     hnam      Added extern def for decode_tai_list & encode_hi_lo_priority_classmark
02/03/09     hnam      Added extern def for encode_guti_ie
==============================================================================*/

#ifndef _MSG_LIB_ENCODE_COMMON_EMM_IE_H_
#define _MSG_LIB_ENCODE_COMMON_EMM_IE_H_

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

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  ENCODE_UE_NW_CAP_IE

DESCRIPTION
  This function encodes the UE NW Capability IE, acts as a wrapper function for MM, 
  as MM expects offset increments in bytes

DEPENDENCIES
  None

RETURN VALUE
  UE_NW_CAPABILITY value part

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_ue_nw_cap_ie(lte_nas_emm_ue_capability_type *ue_capability,byte *msg_index,byte *ota_buf);

/*===========================================================================

FUNCTION  DECODE_TAI_LIST_IE

DESCRIPTION
  This function decodes TAI_LIST Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
extern unsigned int decode_tai_list
(
  byte *src,word pos,
  word *buffer_length,
  lte_nas_incoming_msg_type *nas_incoming_msg_ptr
);

/*===========================================================================

FUNCTION  ENCODE_GUTI_IE

DESCRIPTION
  This function encodes the GUTI information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_guti_ie(lte_nas_emm_guti_type *guti,word *pos, byte *ota_buf);

/*===========================================================================

FUNCTION  ENCODE_EMM_SUPPORTED_CODECS_IE

DESCRIPTION
  This function encodes the EMM_SUPPORTED_CODECS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_emm_supported_codecs(cm_supported_codecs_list_T *emm_supported_codecs, byte *ota_buf, word *pos);

/*===========================================================================

FUNCTION  ENCODE_EPS_MOBILE_IDENTITY

DESCRIPTION
  This function encodes the EPS_MOBILE_IDENTITY information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_eps_mobile_identity(lte_nas_emm_eps_mobile_id_type *eps_mobile_id, byte *ota_buf, word *pos);

/*===========================================================================

FUNCTION  ENCODE_IMSI_IE

DESCRIPTION
  This function encodes the IMSI information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_imsi_ie(lte_nas_emm_imsi_type *imsi,word *pos, byte *ota_buf);

/* As per spec 24.301, section 9.9.3.34 minimum length of the
 UE network capability IE is 2 octets (EPS int + EPS cip algos)*/
#define MINIMUM_UE_NW_CAP_IE_LEN 2

/*===========================================================================

FUNCTION  ENCODE_UE_NW_CAPABILITY_IE

DESCRIPTION
  This function encodes the UE NW Capability IE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_ue_nw_capability_ie(lte_nas_emm_ue_capability_type *ue_capability,word *pos,byte *ota_buf);

/*===========================================================================

FUNCTION  ENCODE_EPS_BEARER_CONTEXT_STATUS

DESCRIPTION
  This function encodes the EPS_BEARER_CONTEXT_STATUS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_eps_bearer_context_status
(
  const lte_nas_eps_bearer_context_status_type *eps_bearer_context_status,  
  byte *ota_buf, 
  word *pos
);

/*===========================================================================

FUNCTION  ENCODE_IMEI_IE

DESCRIPTION
  This function encodes the IMEI information element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void encode_imei_ie
(
  lte_nas_emm_imei_type  *imei_ptr,
  word                   *pos,
  byte                   *ota_buf
);

/*===========================================================================
FUNCTION  ENCODE_MS_NW_FEATURE_SUPPORT

DESCRIPTION
  This function encodes the MS_NW_FEATURE_SUPPORT information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_LTE_REL10
extern void encode_ms_nw_feature_support(boolean *ext_periodic_timer_support, byte *ota_buf, word *pos);

/*===========================================================================
FUNCTION  ENCODE_EMM_DEVICE_PROPERTIES_IE

DESCRIPTION
  This function encodes the DEVICE PROPERTIES information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void encode_emm_device_properties_ie(boolean *low_priority_indicator, byte *ota_buf, word *pos);
#endif
#endif

#endif /*FEATURE_LTE*/
