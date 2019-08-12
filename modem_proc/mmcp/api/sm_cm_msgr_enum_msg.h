/*!
  @file
  

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_cm_msgr_enum_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/08/09   MRK     Added new message to send PDP Activation prameters 
==============================================================================*/

#ifndef SM_CM_MSGR_MSG_ENUM_H
#define SM_CM_MSGR_MSG_ENUM_H

#include<customer.h>

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"

#include <msgr.h>
#include <msgr_umid.h>
#include <msgr_nas.h>
#include "sm_cm_msg_if.h"

/*enum
  {
    #MSGR_DEFINE _UMID( MYTECH, MYMODULE, CMD, FOO, MYMODULE_ID_FOO,  mymodule_cmd_foo_s )
  };
enum
  {
    MYTECH_MYMODULE_CMD_FOO = MSGR_UMID( MSGR_TECH_MYTECH,
                                         MSGR_MODULE_MYMODULE,
                                         MSGR_TYPE_CMD,
                                         MYMODULE_ID_FOO )
  };
  -# The the UMID will get added the MSGR internal registration tables

  After using #MSGR_DEFINE_UMID in a header file, software can use the symbol
  MYTECH_MYMODULE_CMD_FOO as a UMID in any of the MSGR API's.

*/



/*! @brief UMIDs of the messages sent by the CM to ESM
*/
enum
{
  NAS_SM_REQ_FIRST = MSGR_UMID_BASE(MSGR_NAS_SM, MSGR_TYPE_REQ),

  MSGR_DEFINE_UMID(NAS, SM, IND, PDP_CONTEXT_REQUEST_LOG_INFO, 0x01, sm_pdp_context_request_log_info_ind_T ),

  NAS_SM_REQ_MAX,
  NAS_SM_REQ_LAST = NAS_SM_REQ_MAX - 1
} ;               


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


#endif /* SM_CM_MSGR_MSG_ENUM_H */


