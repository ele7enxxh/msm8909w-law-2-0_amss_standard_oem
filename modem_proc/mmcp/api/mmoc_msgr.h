#ifndef MMOC_MSGR_H
#define MMOC_MSGR_H

/**===========================================================================

      M U L T I M O D E   C O N T R O L L E R   M E S S A G E   R O U T E R

                             H E A D E R   F I L E

DESCRIPTION
  This header contains definitions for MMOC to notify offline mode change
  to other modem modules. This also has information for other modem modules
  to register with message router for the message sent out from MMOC.
  
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/api/main/latest/mmoc_msgr.h

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/12   gm      MMOC message router header added.

===========================================================================*/

#include <msgr.h>
#include <msgr_umid.h>

/*==============================================================================
                           Module definitions
==============================================================================*/
/* NOTE: MMOC modules should take 0x51 to 0x99 */

#define MSGR_MODULE_MMOC        0x51
#define MSGR_MM_MMOC            MSGR_TECH_MODULE( MSGR_TECH_MM, MSGR_MODULE_MMOC )


/* This is the module for indicating to modem tasks if they are interested in
** knowing OFFLINE state from MMOC.

   In order to use this, modem tasks should use message router interface.

   MMOC posts an indication with MSGR ID: MM_MMOC_OFFLINE_IND.

   Any interested tasks should do the following to receive these indications:

   Initialize: Using API msgr_client_set_rex_q

   Subscribe:
   
   msgr_register(<module_id>, <task_msgr_client>,
                 MSGR_ID_REX, MM_MMOC_OFFLINE_IND);
     where:
       module_id = MSGR_TECH_MODULE( <tech_name>, <module_name>)
       task_msgr_client = static data of type msgr_client_t.

   How to recv:
     1. Wait for signal provided in initialize step.
     2. Retrieve payload from queue provided to msgr_client_set_rex_q.
     3. Process OFFLINE mode change.
     
*/

/* Payload for all types of messages sent from MMOC. Currently this is a
** dummy payload, OFFLINE indication doesn't need any payload.
*/
typedef struct mmoc_offline_ind_s {

  msgr_hdr_struct_type         hdr;
    /* Customary for all payloads to MSGR */

  /* There is no payload needed to be sent for offline indication. */

} mmoc_offline_ind_s_type;

/* In order to define MSGR_ID such as MM_MMOC_OFFLINE_IND in this case
*/
enum
{
  MM_MMOC_IND_FIRST = MSGR_UMID_BASE(MSGR_MM_MMOC, MSGR_TYPE_IND),
    
  MSGR_DEFINE_UMID(MM, MMOC, IND, OFFLINE, 0x01, mmoc_offline_ind_s_type),
    /* MM_MMOC_OFFLINE_IND */

  MM_MMOC_IND_MAX
};

#endif /* MMOC_MSGR_H */

