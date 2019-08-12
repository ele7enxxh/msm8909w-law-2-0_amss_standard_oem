
/*!
  @file
  lte_nas.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/25/09   hnam    Added TLB support
==============================================================================*/
#ifndef LTE_NAS_H
#define LTE_NAS_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include <customer.h>

#ifdef  FEATURE_LTE

#include "comdef.h"
#include "lte_nas_common_v.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "lte_nas_tlb_message_type.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef union
{
  lte_nas_hdr_type nas_hdr;
  lte_nas_emm_incoming_msg_type emm_incoming_msg;
  lte_nas_esm_incoming_msg_type esm_incoming_msg;
} lte_nas_incoming_msg_type;

typedef union
{
  lte_nas_hdr_type nas_hdr;
  lte_nas_emm_outgoing_msg_type emm_outgoing_msg;
  lte_nas_esm_outgoing_msg_type esm_outgoing_msg;
  lte_nas_tlb_outgoing_msg_type tlb_outgoing_msg;
} lte_nas_outgoing_msg_type;

#ifdef FEATURE_LTE_REL9
typedef enum
{
  LTE_NAS_EMC_SRV_NONE,
  LTE_NAS_EMC_SRV_PENDING,
  LTE_NAS_EMC_SRV_ONLY,
  LTE_NAS_EMC_SRV_EXIST

}lte_nas_emc_srv_status_type;
#endif

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* LTE_NAS_H */

#endif /*FEATURE_LTE*/
