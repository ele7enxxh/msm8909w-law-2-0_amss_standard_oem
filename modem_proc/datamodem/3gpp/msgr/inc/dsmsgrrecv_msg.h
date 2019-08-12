#ifndef DSMSGRRECV_MSG_H
#define DSMSGRRECV_MSG_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

               M S G R   I N T E R F A C E   F U N C T I O N S 

                            H E A D E R    F I L E

DESCRIPTION
  This file defines definitions for use by clients of dsmsgr_send and receive
  modules. This header file is intended to be used internal to the 3G Dsmgr 
  module only.


  Copyright (c) 2008-2014 by Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/msgr/inc/dsmsgrrecv_msg.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/10   ss      DYNAMIC WDOG support for DATA tasks.
08/09/09   vk      Moving DS to its own MSGR technology space
12/22/08   vk      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "msgr.h"
#include <msgr_ds.h>
#include "appmgr_msg.h"
#include "wwan_coex_mgr.h"

/* Message IDs
*/
#define DS_DESENSE_IND_ID     0x01

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

enum
{
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, 
                   none),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, 
                   none),
  MSGR_DEFINE_UMID(DS, MSGRRECV, TMRI, WDOG_RPT_TMR_EXPIRED, 0x00, 
                   none),
  MSGR_DEFINE_UMID(DS, MSGRRECV, IND, DESENSE, DS_DESENSE_IND_ID,
                   cxm_coex_desense_ind_s)
};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================*/

#endif /* DSMSGRRECV_MSG_H */
