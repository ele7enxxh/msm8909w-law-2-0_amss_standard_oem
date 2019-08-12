
/*!
  @file
  

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/lte_nas_tlb.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/25/09   hnam    Added TLB support
==============================================================================*/

#ifndef LTE_NAS_TLB_H
#define LTE_NAS_TLB_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

#include <msgr.h>
#include <msgr_lte.h>
#include <msgr_umid.h>
#include "lte_tlb.h"
#include "emm_database.h"

typedef union
{
  msgr_hdr_s            msg_hdr; /*!< Message router header */
  lte_tlb_ul_info_req_s ul_req;
}lte_nas_tlb_type;

typedef struct 
{
  q_link_type     link; /*!< Queue link. This must be the first element of the cmd_type. 
                             SGR expects q_link to be the first element and the msgr_hdr_s 
                             as the second element.So, after link_type no element should be 
                             added and the only element is the union below*/
  lte_tlb_ul_info_req_s cmd;

} tlb_cmd_type;
/*! @brief UMIDs of the messages sent by the NAS_TLB to EMM
*/
extern boolean emm_process_outgoing_tlb_message
(
  void                   *cmd_ptr,//Changed the i/p type to void*, to be compatible to function pointer
  emm_ctrl_data_type     *emm_ctrl_data_ptr ,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
);

extern void emm_process_incoming_tlb_msg
(
  byte *in_tlb_msg,
  word  tlb_msg_len
);

#endif /* LTE_NAS_TLB_H */

#endif /*FEATURE_LTE*/

