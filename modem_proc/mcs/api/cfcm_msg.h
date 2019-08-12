/*!
  @file
  cfcm_msg.h

  @brief
  CFCM related UMIDs.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cfcm_msg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/09/14   rj      CFCM changes to Process monitor's input in its task context
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCM_MSG_H
#define CFCM_MSG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr_umid.h>
#include <appmgr.h>
#include "cfcm.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief Supervisory messages that CFCM receives or throws
*/

/*! @brief CFCM Msgr IDs for All Messages */
enum
{
  /* CFCM common IDs */
  CFCM_REGISTER_ID         = 0x00,
  CFCM_DEREGISTER_ID       = 0x01,
  CFCM_MONITOR_ID          = 0x02
};



/*! @brief These Requests are used for CFCM Flow Control 
 */
enum 
{
  /*! MCS_CFCM_CLIENT_REGISTER_REQ msg which will 
           be used by clients to register with CFCM */
  MSGR_DEFINE_UMID( MCS, CFCM, REQ, CLIENT_REGISTER,
                    CFCM_REGISTER_ID, cfcm_reg_req_msg_type_s ),  

  /*! MCS_CFCM_CLIENT_DEREGISTER_REQ msg which will 
           be used by clients to de-register with CFCM */
  MSGR_DEFINE_UMID( MCS, CFCM, REQ, CLIENT_DEREGISTER,
                    CFCM_DEREGISTER_ID, cfcm_dereg_req_msg_type_s)

};

enum
{
  /*! MCS_CFCM_LOOPBACK_SPR msg which will 
           be used for LoopBack purpose */
  MSGR_DEFINE_UMID(MCS, CFCM, SPR, LOOPBACK,
             MSGR_ID_LOOPBACK, msgr_spr_loopback_struct_type)
};

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/



#endif /* CFCM_MSG_H */
