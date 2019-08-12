#ifndef _EMM_GPS_HANDLER_H_
#define _EMM_GPS_HANDLER_H_
/*===========================================================================
  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_gps_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "mm_v.h"
#include "emm_gps_lib.h"

/*===========================================================================
FUNCTION    EMM_PROCESS_DL_GENERIC_NAS_TRANSPORT_MESSAGE

DESCRIPTION
  This function puts the incoming RAW buffer from RRC module, into the correct 
  'C' structure that MSG_LIB identifies and then sends it to the GPS module

DEPENDENCIES
  None

RETURN VALUE
  NONE 

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_dl_generic_nas_transport_msg
(
  lte_nas_emm_dl_generic_nas_transport *dl_gen_nas_msg_ptr,
  emm_ctrl_data_type                   *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_PROCESS_UL_GENERIC_NAS_TRANSPORT_MESSAGE

DESCRIPTION
  This function puts the incoming RAW buffer from GPS module, into the correct 
  'C' structure that MSG_LIB identifies and then calls emm_Send_message, 
  to take care of encoding and hence routing this message to RRC

DEPENDENCIES
  None

RETURN VALUE
  NONE 

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_process_ul_generic_nas_transport_msg
(
  void                   *cmd_ptr, 
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
);

#endif /*FEATURE_LTE*/

#endif /* _EMM_GPS_HANDLER_H_ */
