/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I P  S M S

                      M O D U L E

GENERAL DESCRIPTION

  This module implements the functionality of handling SMS through
  IP. SMS is received and WMS is notified.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmipcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipsms.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/12   gm      Refactor IPAPP callback interface 
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
06/12/08   sv      Resolve Klocwork defects
03/02/07   jqi     Initial release
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_SMS_OVER_IP)


#ifndef FEATURE_IP_CALL
#error "The FEATURE_IP_CALL shall be defined to support FEATURE_SMS_OVER_IP."
#endif

#include "cmipappi.h"   /* Includes definitions for VoIP app */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmdbg.h"     /* CM debug macros */
#include "cmsms.h"    /* Include SMS object */

#ifdef CM_DEBUG
#error code not present
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Functions - internal:
** --------------------------------------------------------------------------
*/


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
/*lint -esym(714, cmipsms_send_mo_sms_request) */
void cmipsms_send_mo_sms_request (

  cmipapp_id_type                 app_id,

  cm_sms_msg_type                 *data,

  sys_sys_mode_e_type             sys_mode,

  sys_modem_as_id_e_type          asubs_id
)
{
  CM_ASSERT ( app_id !=  CMIPAPP_ID_INVALID );

  CM_ASSERT( data != NULL );

  CM_ASSERT_ENUM_IS_INRANGE( sys_mode, SYS_SYS_MODE_MAX ); /*lint !e641 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call the app supplied function for MO SMS
  */
  if ( !cmipapp_send_mo_sms_cmd( app_id, data, sys_mode, asubs_id) )
  {
    CM_ERR_0 ("Informing mo sms failed");

  }

  return;

} /* cmipsms_send_MO_SMS_request () */

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
/*lint -esym(714, cmipsms_rpt_proc) */
void cmipsms_rpt_proc(

    const cm_hdr_type   *rpt_ptr
        /* Pointer to a IP app report */
)
{

  cmipapp_rpt_s_type  *cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) rpt_ptr;
    /* Pointer to a IP app report */

  cmsms_s_type        *sms_ptr     = cmsms_ptr();

      /* Point at sms object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmipapp_rpt_ptr != NULL );

  CM_MSG_LOW_1("START cmipcall_rpt_proc, cmd=%d",
               cmipapp_rpt_ptr->hdr.cmd);

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */
  switch( cmipapp_rpt_ptr->hdr.cmd )
  {

     case CM_IP_SMS_IND_MO_SMS_STATUS:

      /* Send MO SMS status to registered clients.
      */
      sms_ptr->mo_status = cmipapp_rpt_ptr->ip_ind.ind.mo_sms_status.status;

      cmsms_event( sms_ptr, CM_SMS_EVENT_MO_STATUS );

      break;

     case CM_IP_SMS_IND_MT_SMS:

      sms_ptr->mt_sms_ptr = &cmipapp_rpt_ptr->ip_ind.ind.mt_sms;

      /* Send MT SMS to registered clients.
      */
      cmsms_event( sms_ptr, CM_SMS_EVENT_MT_MSG);

      break;

    default:
      CM_ERR_1 ("Unkwn report from IP app %d", cmipapp_rpt_ptr->hdr.cmd);
      break;

  } /* switch */
  

} /* cmipsms_rpt_proc () */

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
/*lint -esym(714, cmipsms_client_cmd_check) */
cm_sms_cmd_err_e_type  cmipsms_client_cmd_check(

  cm_sms_cmd_info_s_type  *cmd_ptr
     /* Pointer to SMS comamnd buffer. */
)
{

  cm_sms_cmd_err_e_type   cmd_err       = CM_SMS_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  */
  if( cmd_ptr->sys_mode != SYS_SYS_MODE_HDR && 
      cmd_ptr->sys_mode != SYS_SYS_MODE_WCDMA )
  {
    return cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IP_CALL
  cmd_ptr->app_id = cmipapp_find_active_app( cmd_ptr->sys_mode, CM_CALL_TYPE_SMS );

  if( cmd_ptr->app_id == CMIPAPP_ID_INVALID )
  {
    cmd_err = CM_SMS_CMD_ERR_SIP;
    CM_ERR_0("CM_SMS_CMD_ERR_SIP");

  }
  #endif

  return cmd_err;

}/* cmipsms_client_cmd_check */



#else /* FEATURE_SMS_OVER_IP */

/*===========================================================================
FUNCTION cmipsms_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmipsms_dummy( void )
/*lint -esym(714,cmipsms_dummy) */
/*lint -esym(765,cmipsms_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_SMS_OVER_IP */

