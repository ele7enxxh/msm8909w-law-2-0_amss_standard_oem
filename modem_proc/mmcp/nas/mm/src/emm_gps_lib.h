

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef _EMM_GPS_LIB_H_
#define _EMM_GPS_LIB_H_

#include "mmcp_variation.h"
#include <customer.h>

#ifdef  FEATURE_LTE
#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "emm_gps_if_msg.h"
#include "mm_v.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

/*==========================================================================
 FUNCTION  EMM_ALLOCATE_UL_GENERIC_IEI_MEMORY

DESCRIPTION
  Allocates nas out msg memory required to pull up the dsm items

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_allocate_ul_generic_iei_memory
(
   uint8                                num_attach,
   dsm_item_type                        **dsm_arr_ptr, 
   lte_nas_emm_ul_generic_nas_transport *ul_generic_transport,
   emm_ctrl_data_type                   *emm_ctrl_data_ptr
);

/*===========================================================================
 
FUNCTION  EMM_GENERIC_NAS_DSM_FREE

DESCRIPTION
  This function would free the dsm items of the UL generic nas MSG Req

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_generic_nas_dsm_free
(
  uint8              num_dsm_ptr,
  dsm_item_type      **dsm_arr_ptr  
);

/*===========================================================================
FUNCTION    EMM_SEND_GENERIC_NAS_TRANSPORT_FAILURE_IND

DESCRIPTION
  This function sends FAILURE_IND with error cause to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_generic_nas_failure_ind
(
  emm_irat_failure_cause_type  cause_type,
  byte                         cause,
  dword                        trans_id,
  uint64                       access_barring_rem_time

);

/*===========================================================================
FUNCTION    EMM_UL_GENERIC_NAS_TRANSPORT_FAILED

DESCRIPTION
  This function sends UL_GENERIC_TRANSPORT_TRANSMISSION_FAILURE to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ul_generic_nas_transport_failed
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tac,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_UL_GENERIC_NAS_TRANSPORT_SUCESS

DESCRIPTION
  This function sends UL_GENERIC_TRANSPORT_TRANSMISSION_SUCESS to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_ul_generic_nas_transport_sucess
(
  dword                          trans_id
);


#endif /* _EMM_GPS_LIB_H_ */
#endif /*FEATURE_LTE*/
