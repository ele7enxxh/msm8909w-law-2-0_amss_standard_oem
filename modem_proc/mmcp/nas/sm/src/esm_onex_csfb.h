


/*!
  @file
  esm_onex_csfb.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_onex_csfb.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/08/10   MNK     1XCSFB introduction
                  
==============================================================================*/

#ifndef ESM_ONEX_CSFB_H
#define ESM_ONEX_CSFB_H
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include "esm_v.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
struct esm_1xcsfbm;

typedef boolean handle_1xcsfbm_cm_message_fp ( struct esm_cmd* cmd, struct esm_1xcsfbm* mgr);
typedef boolean handle_1xcsfbm_emm_message_fp ( struct esm_cmd* cmd, struct esm_1xcsfbm* mgr);

typedef struct esm_1xcsfbm
{
  handle_1xcsfbm_cm_message_fp* check_and_handle_cm_message_ptr ;
  handle_1xcsfbm_emm_message_fp* check_and_handle_emm_message_ptr ;
  uint8                          csfb_call_id;
  dword                          trans_id;
}esm_1xcsfbm_T;


void init_esm_1xcsfbm(esm_1xcsfbm_T* mgr_ptr);

#endif 

#endif /*FEATURE_LTE*/
