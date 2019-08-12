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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_declarations.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
05/17/09   hnam    Added the code to support security
==============================================================================*/

#ifndef _MSG_LIB_DECLARATIONS_H_
#define _MSG_LIB_DECLARATIONS_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================
                         HEADER 
==============================================================================*/
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas.h"
#include "lte_nas_security_info.h"
#include "mm_v.h"

/*==============================================================================
                      MACRO DECLARATIONS
==============================================================================*/
/* EPS QoS IE */
#define MIN_EPS_QOS_LENGTH 1

/* QoS IE */
#define MIN_QOS_LENGTH 12

/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
/*===========================================================================

FUNCTION  MSG_LIB_VALIDATE_UPDATE_INCOMING_NAS_MSG_LENGTH

DESCRIPTION
  This function checks the incoming NAS message length, if the length 
  is greater then it is truncated to the MAX length for that OTA message

DEPENDENCIES
  None

RETURN VALUE
  BYTE: Updated length of the incoming NAS message

SIDE EFFECTS
  None

===========================================================================*/
extern word msg_lib_validate_update_incoming_nas_msg_length
( 
  word                            buffer_length,
  lte_nas_protocol_discriminator  pd,
  lte_nas_message_id              message_id
);

#endif

#define MIN_EMM_LEN 2*WORD_SIZE /*pd + security header + message type*/

#define MIN_ESM_LEN 3*WORD_SIZE /*pd + security header + message type  + Bearer Id*/

#define SKIP_DECODING_ESM(offset_ptr,buffer_len_ptr,esm_len)\
           {*offset_ptr     = *offset_ptr     + esm_len;\
        *buffer_len_ptr = *buffer_len_ptr - esm_len;\
            esm_len         = 0;}

#define ADJUST_OFFSET_AND_BUF_LEN(num_bits_read,offset_ptr,buffer_len_ptr,esm_len_ptr)\
            {*offset_ptr     = *offset_ptr     + num_bits_read;\
         *buffer_len_ptr = *buffer_len_ptr - num_bits_read;\
             *esm_len_ptr    = *esm_len_ptr    - num_bits_read;}

extern byte total_num_messages;

extern byte nas_emm_index;

extern byte nas_esm_index;

#define WORD_SIZE 8

extern void report_emm_success
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);

extern boolean msg_lib_encode_error;

extern void report_emm_failure
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);

extern void unknown_emm_msg_received
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);

extern void report_esm_failure
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);

extern void report_esm_success
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
);

//extern lte_nas_dl_security_info_type *nas_dl_security_info;

extern dword calc_gprs_timer
(
  byte  gprs_timer_octet
);

extern boolean is_bcd_valid
(
  byte bcd_value
);

extern boolean esm_length_check_failed;

extern boolean emm_len_check_failed;

extern boolean detach_reason;

extern boolean nas_msg_integrity_check
( 
   byte                          *src, 
     word                          *pos, 
     word                          *buffer_length,
   lte_nas_dl_security_info_type *nas_dl_sec_info, 
   boolean                       nas_msg_lib,
   boolean                       smc
);

/*===========================================================================

FUNCTION    : REPORT_IF_SECURITY_INVALID

DESCRIPTION:  In order for INTER-RAT to happen, NAS_SECURITY & GUTI must be valid. 
This function reports if NAS_SECURITY is invalid.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern
boolean report_invalid_guti_or_nas_security
(
  boolean                     guti_present,
  lte_nas_incoming_msg_type  *nas_incoming_msg_ptr,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
);

#endif

#endif /*FEATURE_LTE*/

