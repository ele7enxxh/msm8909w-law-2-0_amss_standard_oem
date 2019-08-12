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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_tlb_message_type.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when           who                          what                              
--------   --------  ------------------------------------------------
09/25/09   hnam      Added TLB support

==============================================================================*/

/*Source:  //source/qcom/qct/modem/lte/NAS/MSG_LIB/main/latest/inc/lte_nas_emm_message_type.h */

/*==============================================================================*/
#ifndef _LTE_NAS_TLB_MESSAGE_TYPE_H_ 
#define _LTE_NAS_TLB_MESSAGE_TYPE_H_ 

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "comdef.h"
#include "lte_nas_common_v.h"
#include "lte_tlb.h"

 /*Define a structure for UL TLB message type*/
typedef struct
{
  lte_nas_hdr_type    lte_nas_hdr;
  byte                msg_payload[LTE_TLB_MAX_NAS_MSG_CONTAINER_SIZE];
  word                tlb_len;
} lte_nas_tlb_ul_msg_type;

typedef union
{
  lte_nas_hdr_type           hdr;
  lte_nas_tlb_ul_msg_type    tlb_msg;
} lte_nas_tlb_outgoing_msg_type;
#endif

 

 

 

