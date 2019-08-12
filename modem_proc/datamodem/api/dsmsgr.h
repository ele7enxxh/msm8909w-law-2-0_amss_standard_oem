#ifndef DSMSGR_H
#define DSMSGR_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

               M S G R   I N T E R F A C E   F U N C T I O N S 

                            H E A D E R    F I L E

DESCRIPTION
  This file defines definitions for use by clients of dsmsgr_send and receive
  modules. This header file is intended to be used internal to the 3G Dsmgr 
  module only.


  Copyright (c) 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsmsgr.h#1 $ $DateTime$ $Author$

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/11   ars     Removed featurization for SU API compliance
11/18/10   sa      Added function dsmsgrsnd_msg_send_ext().
08/09/09   vk      Moving DS to its own MSGR technology space
05/01/09   vk      Added support for test synchronization with DS task
12/22/08   vk      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "msgr.h"

#include "pthread.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*
** Receiver-side defs
*/

typedef boolean (* dsmsgrcv_msg_hdlr_f) 
(
  msgr_umid_type,
  const msgr_hdr_struct_type *
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


boolean dsmsgrsnd_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
);

boolean dsmsgrsnd_per_subs_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen,
  uint8                  inst_id
);

boolean dsmsgrsnd_msg_send
(
  msgr_umid_type         msgtype,  
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
);

boolean dsmsgrsnd_per_subs_msg_send
(
  msgr_umid_type         msgtype,  
  msgr_hdr_struct_type   *msg,
  uint32                 msglen,
  uint8                  inst_id
);

/*
** Receiver-side APIs
*/

pthread_t dsmsgrrecv_task (int priority);
void dsmsgrrcv_reg_hdlr (msgr_umid_type msgrtype, dsmsgrcv_msg_hdlr_f dshdlr);
void dsmsgrrcv_init (void);

void dsmsgr_post_ds_cmd (void);
void dsmsgr_process_ds_cmd (void * ds_cmd_p);

/*=========================================================================*/

#endif /* DSMSGR_H */
