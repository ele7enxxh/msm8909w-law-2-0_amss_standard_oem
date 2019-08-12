#ifndef GMM_EMM_COMMON_H
#define GMM_EMM_COMMON_H
/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmm_emm_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/10   zren    Created to hold difinitions moved from gmm_msg.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


typedef struct
{
  byte length;
  byte value[8];
} gmm_ie_LV_ms_network_caps_T;


typedef struct
{
  byte length;
  byte id_type;     /* type of identity, digit 1, odd/even indication */
  byte value[8];
} gmm_ie_LV_mobile_id_T;

typedef struct
{
  byte IEI;
  byte value[2];
} gmm_ie_TV_drx_parameter_T;

gmm_ie_TV_drx_parameter_T get_drx_parameter( void );

gmm_ie_LV_ms_network_caps_T get_ms_network_capability( void );

cm_supported_codecs_list_T gmm_get_supported_codec_list( void );

#if defined(FEATURE_LTE)
extern void gmm_send_sm_isr_status_ind(boolean isr_status);
#endif
#endif /* GMM_EMM_COMMON_H */
