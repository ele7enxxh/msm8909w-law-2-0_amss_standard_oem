/*===========================================================================

                             D S M S G R S E N D . C
 
GENERAL DESCRIPTION
  Implementation of DS MSGR SENDer-side helper apis.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/msgr/src/dsmsgrsend.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   ua      Fixed compilation erros.
11/18/10   sa      Added function dsmsgrsnd_msg_send_ext().
08/09/09   vk      Moving DS to its own MSGR technology space
05/01/09   vk      Added support for test synchronization with DS task
12/22/08   vk      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#ifdef TEST_FRAMEWORK
#error code not present
#endif
#include "amssassert.h"
#include "dsmsgr.h"
#include "msgr.h"
#include "msgr_ds.h"
#include "ds_3gppi_utils.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

boolean dsmsgrsnd_msg_send
(
  msgr_umid_type         msgtype,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
)
{
  return dsmsgrsnd_per_subs_msg_send(msgtype, msg, msglen, 0);
} /* dsmsgrsnd_msg_send */

boolean dsmsgrsnd_per_subs_msg_send
(
  msgr_umid_type         msgtype,  
  msgr_hdr_struct_type   *msg,
  uint32                 msglen,
  uint8                  inst_id
)
{
  
  return dsmsgrsnd_per_subs_msg_send_ext
           (msgtype, MSGR_DS_MSGRRECV, msg, msglen, inst_id);
} /* dsmsgrsnd_per_subs_msg_send */

boolean dsmsgrsnd_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
)
{
  return dsmsgrsnd_per_subs_msg_send_ext
           (msgtype, tech_mod_type, msg, msglen, 0);
}/* dsmsgrsnd_msg_send_ext */

boolean dsmsgrsnd_per_subs_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen,
  uint8                  inst_id
)
{
  errno_enum_type  ret_val = E_FAILURE;

  ASSERT(msg != NULL);

  /* Initialize the header info */
  msgr_init_hdr_inst(msg, tech_mod_type, msgtype, inst_id);
  ret_val = msgr_send(msg, msglen);
  if ( ret_val != E_SUCCESS)
  {
    DS_3GPP_MSG2_ERROR("Unable to send msg to tech %d err:%d",
                        tech_mod_type,ret_val);
    return FALSE;
  }
  return TRUE;
} /* dsmsgrsnd_per_subs_msg_send_ext */


