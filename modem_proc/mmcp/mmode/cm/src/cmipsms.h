#ifndef CMIPSMS_H
#define CMIPSMS_H

#include "mmcp_variation.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   I P   S MS 

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface an application (VoIP) to
   Call Manager. Call Manager can interact with the application to place
   SMS on whatever technology application may support. This way current
   Call Manager interfaces to its clients need not change and at the same
   time clients get to make use of new technology.

  Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipsms.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/07   jqi     Initial release.
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_SMS_OVER_IP)

#include "cm.h"
#include "cm_v.h"


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - Called by CM
** ------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION cmipsms_send_MO_SMS_request

DESCRIPTION
  Apps that have a registered function to send MO SMS message request.
  
DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipsms_send_mo_sms_request (

  cmipapp_id_type                 app_id,

  cm_sms_msg_type                 *data,

  sys_sys_mode_e_type             sys_mode,

  sys_modem_as_id_e_type          asubs_id
);

#ifdef FEATURE_SMS_OVER_IP
/*===========================================================================

FUNCTION cmipsms_rpt_proc

DESCRIPTION
  Process app reports (i.e. notifications of SMS activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmipsms_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
);
#endif

/*===========================================================================

FUNCTION  cmipsms_client_cmd_check

DESCRIPTION
  Check for sms command parameter errors and whether a specified SMS command
  is allowed in the current state of the sms/call/phone.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  CM_SMS_CMD_ERR_NONE if command is allowed in the current state
  of the sms/call/phone, specific cm_sms_cmd_err_e_type otherwise.

SIDE EFFECTS
  none
===========================================================================*/
extern cm_sms_cmd_err_e_type  cmipsms_client_cmd_check(

    cm_sms_cmd_info_s_type  *cmd_ptr
        /* Pointer to SMS comamnd buffer. */
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined(FEATURE_SMS_OVER_IP) */

#endif /* CMIPSMS_H */
