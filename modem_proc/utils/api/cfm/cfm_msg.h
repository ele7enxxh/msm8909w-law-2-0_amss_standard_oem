/*!
  @file
  cfm_msg.h

  @brief
  CFM related UMIDs.

  @author
  axiao

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/cfm/cfm_msg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/20/12   ax      CR316940 CPU based flow control  
04/30/10   ax      initial version
==============================================================================*/

#ifndef CFM_MSG_H
#define CFM_MSG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr_umid.h>
#include <appmgr.h>
#include "msgr_utils.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief Supervisory messages that CFM receives or throws
*/

enum
{
  MSGR_DEFINE_UMID(UTILS, CFM, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(UTILS, CFM, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(UTILS, CFM, SPR, THREAD_READY, APPMGR_ID_THREAD_READY,
                   none),
  MSGR_DEFINE_UMID(UTILS, CFM, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL,
                   none)
};

/*! @brief UMID for all commands sent by CFM
*/
enum
{
  MSGR_DEFINE_UMID(UTILS, CFM, CMD, FC, 0x0, cfm_fc_cmd_msg_s),
  UTILS_CFM_MAX_CMD,
  UTILS_CFM_LAST_CMD = UTILS_CFM_MAX_CMD - 1
};

/*! @brief UMID for all indciation recieved CFM
*/
enum
{
  MSGR_DEFINE_UMID(UTILS, CFM, IND, REG,   0x0,   cfm_reg_ind_msg_s),
  MSGR_DEFINE_UMID(UTILS, CFM, IND, DEREG, 0x1,   cfm_dereg_ind_msg_s),
  MSGR_DEFINE_UMID(UTILS, CFM, IND, MONITOR, 0x2, cfm_monitor_ind_msg_s),
  UTILS_CFM_MAX_IND,
  UTILS_CFM_LAST_IND = UTILS_CFM_MAX_IND - 1
};

/*! @brief UMID for RLCDL timer
*/
enum
{
  MSGR_DEFINE_UMID(UTILS, CFM, TMRI, EXPIRE, 0x0, msgr_hdr_s),
  UTILS_CFM_MAX_TMRI,
  UTILS_CFM_LAST_TMRI = UTILS_CFM_MAX_TMRI - 1
} ;
/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/



#endif /* CFM_MSG_H */
