#ifndef ESTK_SMS_H
#define ESTK_SMS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SHORT  MESSAGE


GENERAL DESCRIPTION

  This header file contains ESTK functions that handle SMS.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2008 QUALCOMM Technologies, Inc (QTI) and its licensors.  
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/estk/src/estk_sms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/14   vr      Check WMS ready before sending SMS to WMS
08/07/14   dy      Process WMS events in GSTK task context
05/06/14   gs      Removed the ESTK featurization
12/17/09   sg      Fix compilation errors
11/03/08   sk      Featurized ESTK based on external interface support
08/21/08   xz      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_ESTK
#include "estk_priv.h"

/*===========================================================================
FUNCTION: estk_process_wms_msg_submit_rpt_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process WMS Message Submit
  Report event

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of WMS event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wms_msg_submit_rpt_event_cmd(
  const estk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION: estk_process_wms_cfg_event_cmd

DESCRIPTION :
  This function will be called by ESTK to process WMS Configuration event

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of WMS event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_wms_cfg_event_cmd(
  const estk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION: estk_sms_reg

DESCRIPTION:
  ESTK registration of WMS client

PARAMETERS:
  None
 
DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_sms_reg( void );

/*===========================================================================
FUNCTION: estk_process_send_sms_req

DESCRIPTION:
  GSTK calls this ESTK function to process the SEND SMS request.

PARAMETERS:
  gstk_req_ptr : [Input] gstk SEND SMS cmd pointer
 
DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When SMS is successfully sent
    ESTK_ERROR:   - When there is failure in sending SMS to WMS module

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_sms_req(
  gstk_cmd_from_card_type *gstk_req_ptr);

#endif /* FEATURE_ESTK */
#endif /* ESTK_SMS_H */

