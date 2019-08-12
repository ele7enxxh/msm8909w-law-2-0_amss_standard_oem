/*!
  @file
  esm.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/inc/esm_v.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/03/09   MNK     Added esm_get_pdn_connectivity_req_T message
05/28/09   MNK     Remove unneeded messages
05/08/09   MNK     Added new internal timer expiration message/signal 
04/24/09   MNK     Added emm_eps_bearer_status message
==============================================================================*/

#ifndef ESM_V_H
#define ESM_V_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/


#include "mmcp_variation.h"
#ifdef  FEATURE_LTE

#include "esm.h"
#include "lte_nas.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/



/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern void esm_get_context_status
(
  byte   *ptr2esm_msg /* The data will be in the format of esm_active_eps_ind_T */
);

extern void esm_task
(
 void
);

extern void esm_init
(
  void /*!< void */
);

extern void esm_stop_task
(
  void
);

extern boolean esm_isr_status_get
(
  void
);

void esm_isr_status_set
(
  boolean isr_status
);

extern void esm_reset_isr_bearer_ctxt_act_status
(
  void
);

#ifdef FEATURE_LTE_REL9 
/*===========================================================================

FUNCTION  ESM_GET_EMC_SRV_STATUS

DESCRIPTION
  This function returns emergency bearer service status

DEPENDENCIES
  None

RETURN VALUE
  EMC BS status

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_emc_srv_status_type  esm_get_emc_srv_status(void);

/*===========================================================================

FUNCTION  ESM_SET_EMC_SRV_STATUS

DESCRIPTION
  This function set emergency bearer service status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_set_emc_srv_status
(
  lte_nas_emc_srv_status_type  emc_srv_status
);

#endif
#endif /* ESM_H */

#endif /*FEATURE_LTE*/
