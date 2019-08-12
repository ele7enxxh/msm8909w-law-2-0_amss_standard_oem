/*!
  @file
  sm_cm_msg_if.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_cm_msg_if.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
==============================================================================*/

#ifndef SM_CM_MESSAGES_H
#define SM_CM_MESSAGES_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include<customer.h>

#include "comdef.h"
#include <msgr.h>
#include "cm_gw.h"
#include "sys_gw_v.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

typedef enum
{
  SM_PDP_UE_INITIATED = 0,
  SM_PDP_NETWORK_INITIATED
}sm_pdp_initiator_T;

/*! @brief REQUIRED This struct represents the PDP Context Request Log Info
*/
typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  context_id;
  dword                  assoc_context_id;
  boolean                is_primary_pdp;
  sm_pdp_initiator_T     initiator;
  uint8                  nsapi;
  uint8                  sapi;
  char                   apn[MAX_APN_ADR_LEN];
} sm_pdp_context_request_log_info_ind_T; 



/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#endif /* SM_CM_MESSAGES_H */


