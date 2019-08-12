/*===========================================================================

C A L L   M A N A G E R   I P   A P P L I C A T I O N

M O D U L E

DESCRIPTION

Contains implementation of functions that interface an application (VoIP)
to Call Manager. Call Manager can interact with the application to place
calls on whatever technology application may support. This way current
Call Manager interfaces to its clients (UI) need not change and at the
same time clients (UI) get to make use of new technology.

Copyright (c) 2005 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmipapp.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/10/14   xs      New req: checking voice capabilityfor USSD
05/10/14   sm      FR3707:IMS deregister request for subs unavailable cmd 
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
04/23/13   xs      USSD detect support
01/15/13   xs      Added video share support
11/22/12   xs      Added PS supplemetary service support
04/25/12   gm      Refactor IPAPP callback interface 
03/27/12   xs      VoLTE Remove of user from Conf-call
03/23/12   gm      DIME integration fix: TMC deprecation and others.
01/06/12   gm      Data synchronization changes for freefloating platform
12/21/11   xs      L->W SRVCC support
09/19/11   xs      Fix invalid data block sent in call orig.
08/25/11   xs      Add user data block support for some APIs
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
08/16/10   aj      support VOIP deregistration on ps detach
08/05/10   fj      Add OFFLINE check in cmipapp_is_wait_on_oprt_mode_cnf() 
06/17/10   sv      Corrected featurization of cmipapp_reg_abort_func
05/26/10   jqi     EHRPD VOIP silent redial support.
03/02/10   fj      Updated lte_srv_status in the HDR HYBRID condition.
01/26/10   np      Added support LTE system status info
11/05/09   mp      Added support to remove the last party added for VOIP calls
03/24/09   sv      updated to cmtaski.h
11/28/08   rm      CMI defeaturisation
11/11/08   cl      Remove check for call waiting callback func as it's not 
mandatory for IP Call Apps to be activate.
10/30/08   jd      Adding support for IMS registration
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
03/29/08   rm      Adding UMTS interface support for cmipapp
06/22/07   sk      Added support for ATOM.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
VoIP.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
02/17/06   ka      Adding support for videoshare
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
-fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
01/05/06   ka      Adding debug messages
12/11/05   ic      Lint cleanup
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
rex_leave_crit_sect()  
05/01/05   ka      Correcting logic to compute hdr srv status before
reporting it to clients.
04/21/05   ka      Changed function name to cmipapp_cap_reg
04/20/05   ka      Informing apps of service status change in CM
04/06/05   ka      Header Description change
03/14/05   ka      Cleaning find active app, app id allocate and removing
feature flags
02/09/05   ic      Lint cleanup
02/02/05   dk/ka   Initial release
02/04/05   ka      Lint fixes
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */


#include "cmipappi.h"   /* Interface header file for applications */
#include "comdef.h"    /* For ARR_SIZE() */
#include "cmdbg.h"     /* Interface to CM debug services */

#include "cmtaski.h"    /* Interface to command queue processing */
#include "cmss.h"      /* Serving system info structure */
#include "cmph.h"      /* Phone object interface */

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmsds.h"
#include "cm_msgr_msg.h"
#include "cmefs.h"

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
** Datatypes
** --------------------------------------------------------------------------
*/


/* Stores details of all apps
*/
/*lint -esym(765, cmipapp_arr) */  
/* in_CS for All tasks: Array and its objects */
static cmipapp_s_type cmipapp_arr[CMIPAPP_POOL_SIZE];
/**--------------------------------------------------------------------------
** Functions - Internal
** --------------------------------------------------------------------------
*/

/*=========================================================================

FUNCTION
cmipapp_crit_sect_ptr

DESCRIPTION

Critical section to protect shared global data related to CM IP APP.

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
static rex_crit_sect_type* cmipapp_crit_sect_ptr(void)
{
  /* For now we use the same lock, once have lock sequencing framework
  ** we can have separate lock for CMIP APP */
  return cmtask_crit_sect_ptr();
}

#if defined(FEATURE_IP_CALL)

/*===========================================================================

FUNCTION cmipapp_send_reg_status

DESCRIPTION
Sends CM ip app registration status

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void cmipapp_send_reg_status (

                                     cmipapp_id_type                app_id,
                                     /* App ID
                                     */

                                     cm_ipapp_reg_status_e_type     ipapp_reg_status
                                     /* ipapp's registration status
                                     */

                                     )
{

  cm_sd_rpt_u_type      *rpt_cmd_ptr = NULL;
  uint8  cap_index = 0;

  /*----------------------------------------------------------------------*/

  /* Using sd report queue to pass on ipapp registration message. Since ipapp
  ** reports are common to technologies, need to use a way that is common to 
  ** all.
  */
  rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal();
  /* Fill in command type */
  rpt_cmd_ptr->hdr.cmd                            = CM_IP_APP_REG_STATUS;

  /* App registration status */
  rpt_cmd_ptr->ip_app_reg_status.ipapp_reg_status = ipapp_reg_status;

  /* Call type's that app can service */

  /* Copying bit mask call type's app can support
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
  {
    rpt_cmd_ptr->ip_app_reg_status.call_type_mask[cap_index]   = 
      cmipapp_arr[app_id].app_capability.call_type[cap_index];

  /* System modes app can support for calls */
    rpt_cmd_ptr->ip_app_reg_status.sys_mode[cap_index]         =
      (sys_sys_mode_mask_e_type)(cmipapp_arr[app_id].app_capability.sys_mode[cap_index]);
  }

  /* cause for registration, de-registration */
  rpt_cmd_ptr->ip_app_reg_status.ipapp_cause      = 
    cmipapp_arr[app_id].reason.ipapp_cause;

  /* Warning info tied to registration */
  rpt_cmd_ptr->ip_app_reg_status.warn_info        = 
    cmipapp_arr[app_id].reason.warn_info;

  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_cmd_ptr);

} /* cmipapp_send_reg_status () */
#endif /* #if defined(FEATURE_IP_CALL)*/
/*===========================================================================

FUNCTION cmipapp_next_trans_id

DESCRIPTION
Gives the next transaction identifier.

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static uint32 cmipapp_next_trans_id ( void )
{
  static uint32 trans_id = CMIPAPP_TRANS_ID_NONE;


  return ++trans_id;

}


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/*===========================================================================
FUNCTION cmcall_end_cause_to_cmipcall_end_cause

DESCRIPTION
  Map CM End Cause to CMIPAPP End Cause.

DEPENDENCIES
  None

RETURN VALUE
  CMIPAPP End CAuse

SIDE EFFECTS
  none
===========================================================================*/
cmipapp_end_cause_e_type cmcall_end_cause_to_cmipcall_end_cause
(
  cm_call_end_e_type  cmcall_end_cause
)
{
  cmipapp_end_cause_e_type cmipcall_end_cause;

  switch (cmcall_end_cause)
  {
    case CM_CALL_END_IP_LOW_BATTERY:             
      cmipcall_end_cause = CMIPAPP_END_CAUSE_LOW_BATTERY;
      break;

    case CM_CALL_END_IP_DEAD_BATTERY:             
      cmipcall_end_cause = CMIPAPP_END_CAUSE_DEAD_BATTERY;
      break;

    default:
      cmipcall_end_cause = CMIPAPP_END_CAUSE_NORMAL;
      break;
  }

  return cmipcall_end_cause;
}


/*===========================================================================

FUNCTION cmipapp_is_ussd_supported_call_type

DESCRIPTION
This checks whether given call_type mask if supported with USSD
USSD should use same RAT as voice does

DEPENDENCIES
NA

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
None
===========================================================================*/
static boolean cmipapp_is_ussd_supported_call_type
(
  cmipapp_call_type_e_type call_type_mask
)
{
  if(call_type_mask & SYS_BM_64BIT(CM_CALL_TYPE_VOICE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmipapp_reset

DESCRIPTION
Initializes the app entry pointed by app_id
CAUTION:- Function gets called within a lock since it accesses app_id that
can be used by multiple apps. Function should not wait on any
event.

DEPENDENCIES
None

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
static void cmipapp_reset(

                          cmipapp_id_type                         app_id
                          /* App id assigned by CM
                          */
                          )
{
  uint8    cap_index =0;

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());

  /* Serialize data access cmipapp_arr[] */

  /* Reset all details of the particular app
  */
  cmipapp_arr[app_id].app_id         = CMIPAPP_ID_INVALID;

  for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
  {
  /* Call types app can service */
    cmipapp_arr[app_id].app_capability.call_type[cap_index]      = CMIPAPP_CALL_TYPE_NONE;
  /* Technologies app can work on */
    cmipapp_arr[app_id].app_capability.sys_mode[cap_index]      = CMIPAPP_SYS_MODE_NONE;
  }

  /* APP status active/inactive */
  cmipapp_arr[app_id].app_run_status = CMIPAPP_RUN_STATUS_NONE;

  /* ip app registration/dereg/failure cause */
  cmipapp_arr[app_id].reason.ipapp_cause         = CM_IPAPP_CAUSE_NONE;

  /* ip app registration/dereg/failure warning */
  cmipapp_arr[app_id].reason.warn_info.present = FALSE;


  /* Initializes transaction id to NONE */
  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  /* Initializes oprt mode sent to NONE */
  cmipapp_arr[app_id].oprt_mode_sent  = SYS_OPRT_MODE_NONE;

  /* Initializes oprt mode sent to NONE */
  cmipapp_arr[app_id].srv_domain_sent = CM_SRV_DOMAIN_PREF_NONE;
   
  /* Initializes subs cmd  sent to NONE */
  cmipapp_arr[app_id].subs_cmd  = CM_PH_CMD_NONE;

  memset ((byte *)&(cmipapp_arr[app_id].app_cb), 0,
    sizeof(cmipapp_arr[app_id].app_cb));

  /* Initializes user_data_block to default */
  cmipapp_arr[app_id].user_data_block= CMIPAPP_VAL_DEFAULT_DATA_BLOCK;

  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

} /* cmipapp_reset() */

/*===========================================================================

FUNCTION cmipapp_reset_trans_id

DESCRIPTION
Reset trans id being used for tracking app's oprt mode cnf

DEPENDENCIES
None

RETURN VALUE
none

SIDE EFFECTS
Clears trans_id in cmipapp_arr[]

===========================================================================*/
void cmipapp_reset_trans_id (void)
/*lint -esym(714,cmipapp_reset_trans_id) */
/*lint -esym(765,cmipapp_reset_trans_id) */
/* Can't be static as it would result in compiler warnings
*/
{

  /*----------------------------------------------------------------------*/

  unsigned int id_count = 0;
  /* Index
  */

  /*----------------------------------------------------------------------*/

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {  
    cmipapp_arr[id_count].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  /*----------------------------------------------------------------------*/

} /* cmipapp_reset_trans_id () */


/*===========================================================================

FUNCTION cmipapp_init

DESCRIPTION
Initializes table to maintain app details. Reads phone identity

DEPENDENCIES
None

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
/*lint -esym(714, cmipapp_init) */  
/*lint -esym(765, cmipapp_init) */  
void cmipapp_init(void)
{
  cmipapp_id_type app_id;

  /*----------------------------------------------------------------------*/

  /* Initialize cmipapp_arr[] with invalid app ids and the state of app
  ** to inactive
  */
  memset (cmipapp_arr, 0, sizeof(cmipapp_arr));

  for (app_id = 0; app_id < ARR_SIZE(cmipapp_arr); app_id++)
  {
    cmipapp_reset (app_id);
  }

  /* Read phone identity and cache it for read by cmipapp_get_ph_identity()
  */

} /* cmipapp_init() */




/*===========================================================================

FUNCTION cmipapp_cb_init

DESCRIPTION
Initializes function pointers to NULL

DEPENDENCIES
None

RETURN VALUE
none

SIDE EFFECTS
Modifies argument with pointers to call related functions

===========================================================================*/
void cmipapp_cb_init(

                     cmipapp_cb_s_type                *app_cb_ptr
                     /* Structure to be initialized with callback functions
                     */
                     )
                     /*lint -esym(715,app_cb_ptr) */
                     /*lint -esym(818,app_cb_ptr) */
{
#if defined(FEATURE_IP_CALL)
  if (app_cb_ptr == NULL)
  {
    return;
  }

  /* Initialize structure fields with default call back values */
  /* Points to call origination function */
  app_cb_ptr->cmipapp_cmd_orig_func_ptr                   =   NULL;

  /* Points to call answering function */
  app_cb_ptr->cmipapp_cmd_answer_func_ptr                 =   NULL;

  /* Points to incoming call response function */
  app_cb_ptr->cmipapp_cmd_invite_res_func_ptr             =   NULL;

  /* Points to call end function */
  app_cb_ptr->cmipapp_cmd_end_func_ptr                    =   NULL;

  /* Points to start continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_start_cont_dtmf_func_ptr =   NULL;

  /* Points to stop continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_stop_cont_dtmf_func_ptr  =   NULL;

  /* To put call on hold */
  app_cb_ptr->cmipapp_cmd_hold_func_ptr                   =   NULL;

  /* To resume a call on hold */
  app_cb_ptr->cmipapp_cmd_active_func_ptr                 =   NULL;

  /* To put call on local hold */
  app_cb_ptr->cmipapp_cmd_local_hold_sub_func_ptr             =   NULL;

  /* To retrieve a call on local hold */
  app_cb_ptr->cmipapp_cmd_local_retrieve_sub_func_ptr         =   NULL;

  /* To modify call type */
  app_cb_ptr->cmipapp_cmd_modify_func_ptr                 =   NULL;

  /* To inform change in serving system parameters */
  app_cb_ptr->cmipapp_cmd_serv_sys_info_sub_ptr               =   NULL;

  app_cb_ptr->cmipapp_cmd_act_answer_hold_sub_func_ptr        =   NULL;


  app_cb_ptr->cmipapp_cmd_deact_answer_hold_sub_func_ptr      =   NULL;

  app_cb_ptr->cmipapp_cmd_answer_call_waiting_sub_func_ptr    =   NULL;

  app_cb_ptr->cmipapp_cmd_simple_flash_sub_func_ptr           =   NULL;

  app_cb_ptr->cmipapp_cmd_ect_sub_func_ptr                    =   NULL;

  app_cb_ptr->cmipapp_cmd_mpty_sub_func_ptr                   =   NULL;

  app_cb_ptr->cmipapp_inform_oprt_mode                    =   NULL;  

  app_cb_ptr->cmipapp_cmd_notify_ho_sub_func_ptr              =   NULL;

  /* To send MO SMS over IP app */
  app_cb_ptr->cmipapp_sms_cmd_mo_sms_sub_func_ptr             =   NULL;

  app_cb_ptr->cmipapp_cmd_abort_sub_func_ptr                  =   NULL;

  app_cb_ptr->cmipapp_inform_srv_domain_sub                   =   NULL;

  app_cb_ptr->cmipapp_srvcc_status_sub_func_ptr               =   NULL;

  app_cb_ptr->cmipcall_rm_last_party_from_conf_sub            =   NULL;

  app_cb_ptr->cmipapp_sups_cmd_sub                            =   NULL;

  app_cb_ptr->cmipapp_ussd_cmd_sub                            =   NULL;
  
  app_cb_ptr->cmipapp_subsc_chgnd               	     =   NULL;

  app_cb_ptr->cmipcall_add_party_to_conf_sub				  =   NULL;

  app_cb_ptr->cmipcall_call_deflection_cmd_sub_func_ptr       =    NULL;

  app_cb_ptr->cmipapp_cmd_end_ext_sub_func_ptr                =   NULL;

  /* Points to call origination function */
  app_cb_ptr->cmipapp_cmd_orig_sub_func_ptr                   =   NULL;

  /* Points to call answering function */
  app_cb_ptr->cmipapp_cmd_answer_sub_func_ptr                 =   NULL;

  /* Points to incoming call response function */
  app_cb_ptr->cmipapp_cmd_invite_res_sub_func_ptr             =   NULL;

  /* Points to call end function */
  app_cb_ptr->cmipapp_cmd_end_sub_func_ptr                    =   NULL;

  /* Points to start continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_start_cont_dtmf_sub_func_ptr =   NULL;

  /* Points to stop continuous dtmf function */
  app_cb_ptr->cmipapp_inband_cmd_stop_cont_dtmf_sub_func_ptr  =   NULL;

  /* To put call on hold */
  app_cb_ptr->cmipapp_cmd_hold_sub_func_ptr                   =   NULL;

  /* To resume a call on hold */
  app_cb_ptr->cmipapp_cmd_active_sub_func_ptr                 =   NULL;

  /* To modify call type */
  app_cb_ptr->cmipapp_cmd_modify_sub_func_ptr                 =   NULL;

#endif

} /* cmipapp_cb_init() */
/*lint +esym(715,app_cb_ptr) */
/*lint +esym(818,app_cb_ptr) */



/*===========================================================================

FUNCTION cmipapp_reg

DESCRIPTION
Used by application to register callback functions with CM

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK         - Registration Successful

CMIPAPP_STATUS_CB_REG_FAILED - Registration Failed

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg (

                                   const cmipapp_cb_s_type          *app_cb_ptr,
                                   /* Structure pointing to app call back functions
                                   */

                                   cmipapp_id_type                   app_id,
                                   /* App id that is registering call backs
                                   */

                                   uint32                            data_block
                                   /* User token passed to CM at registration. CM will
                                   ** return it back in callback and other notification 
                                   ** function
                                   */

                                   )
                                   /*lint -esym(715,app_cb_ptr) */
                                   /*lint -esym(818,app_cb_ptr) */
                                   /*lint -esym(715,app_id) */
                                   /*lint -esym(715,data_block) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ((app_cb_ptr == NULL) ||
    (!(app_id < ARR_SIZE(cmipapp_arr)))
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Copy the call backs that app wants to register in cmipapp_arr
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */
  *(&(cmipapp_arr[app_id].app_cb)) = *app_cb_ptr;
  cmipapp_arr[app_id].user_data_block = data_block;
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  return CMIPAPP_STATUS_OK;
#else
  return CMIPAPP_STATUS_NONE ;
#endif

} /* cmipapp_reg () */
/*lint +esym(715,app_cb_ptr) */
/*lint +esym(818,app_cb_ptr) */
/*lint +esym(715,app_id) */
/*lint +esym(715,data_block) */


/*===========================================================================

FUNCTION cmipapp_id_deallocate

DESCRIPTION
Application calls to deallocate app id

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK                - App id deallocation Successful
CMIPAPP_STATUS_ID_DEALLOC_FAILED - App id deallocation Failed

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_id_deallocate (

  cmipapp_id_type                         app_id
  /* App id assigned by CM
  */
  )
  /*lint -esym(715,app_id) */
{
#if defined(FEATURE_IP_CALL)

  cm_sd_rpt_u_type                 *rpt_cmd_ptr = NULL;
  /* Report being sent to CM
  */

  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if (!(app_id < ARR_SIZE(cmipapp_arr)))
  {
    return CMIPAPP_STATUS_ID_DEALLOC_FAILED;
  }

  /* Obtain a lock
  */

  /* Reset all details of the particular app
  */
  cmipapp_reset (app_id);

  /* Free the lock
  */

  /* Update the DDTM status.
  */
  rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal();
  /* Fill in command type */
  rpt_cmd_ptr->hdr.cmd = CM_IP_APP_UPDATE_DDTM;

  CM_MSG_HIGH_1 ("Sending CM_IP_APP_UPDATE_DDTM. app_id %d deallocated", app_id);

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_cmd_ptr);

 

  return CMIPAPP_STATUS_OK;
#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_id_deallocate () */
/*lint +esym(715,app_id) */


/*===========================================================================

FUNCTION cmipapp_map_sysmode_to_cm_sysmode

DESCRIPTION
Process app reports (i.e. notifications of call activity or
replies to clients call commands).

DEPENDENCIES
None.

RETURN VALUE
CM system mode that corresponds to App system mode

SIDE EFFECTS
none

===========================================================================*/
sys_sys_mode_e_type cmipapp_map_sysmode_to_cm_sysmode(

  cmipapp_sys_mode_e_type      sys_mode
  /* sys mode */
  )
{
  switch (sys_mode)
  {
  case CMIPAPP_SYS_MODE_NONE:   return SYS_SYS_MODE_NONE;

  case CMIPAPP_SYS_MODE_CDMA:   return SYS_SYS_MODE_CDMA;

  case CMIPAPP_SYS_MODE_HDR:    return SYS_SYS_MODE_HDR;

  case CMIPAPP_SYS_MODE_GSM:    return SYS_SYS_MODE_GSM;

  case CMIPAPP_SYS_MODE_WCDMA:  return SYS_SYS_MODE_WCDMA;

    case CMIPAPP_SYS_MODE_LTE:    return SYS_SYS_MODE_LTE;
    
    case CMIPAPP_SYS_MODE_WLAN:   return SYS_SYS_MODE_WLAN;

  default:
    CM_ERR_1( "CMIPAPP sys mode %d not mapped to CM sys mode!!!",sys_mode);
    CM_ASSERT(FALSE);
    return SYS_SYS_MODE_MAX;

  }

} /* cmipapp_map_sysmode_to_cm_sysmode () */

/*===========================================================================

FUNCTION cmipapp_find_active_app

DESCRIPTION
Finds an active app with a particular sys_mode and call type

DEPENDENCIES
None

RETURN VALUE

App id

SIDE EFFECTS
None

===========================================================================*/
/*lint -esym(714, cmipapp_find_active_app) */
/*lint -esym(765, cmipapp_find_active_app) */
cmipapp_id_type cmipapp_find_active_app (

  sys_sys_mode_e_type     sys_mode,
  /* The system mode
  */

  cm_call_type_e_type     call_type
  /* call type
  */

  )
{

  cmipapp_id_type   app_id    = CMIPAPP_ID_INVALID;
  /* app id
  */

  unsigned int id_count = 0;
  /* Index
  */
  uint8    cap_index =0; /*capablities array index*/

  boolean is_match_found=0;

  /*----------------------------------------------------------------------*/

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT_ENUM_IS_INRANGE (call_type, CM_CALL_TYPE_MAX);   /*lint !e641 */
  CM_ASSERT_ENUM_IS_INRANGE (sys_mode,  SYS_SYS_MODE_MAX);   /*lint !e641 */

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /* Check is made if there is an app registered
  ** with required call type and system mode
  */
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    /* CM only routes the service when the IP app is in the active state.
    */
    for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
    {
    if((cmipapp_arr[id_count].app_id != CMIPAPP_ID_INVALID) 
      &&
        (cmipapp_arr[id_count].app_capability.call_type[cap_index] & SYS_BM_64BIT(call_type)) /*lint !e641 */           
      &&   
        (cmipapp_arr[id_count].app_capability.sys_mode[cap_index] & SYS_BM_64BIT(sys_mode)) /*lint !e641 */       
      &&
      (cmipapp_arr[id_count].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE ) 
      )
    {
      app_id = cmipapp_arr[id_count].app_id;
        is_match_found=TRUE;
      break;
    }
    }
    if(is_match_found==TRUE)
    {break;}
  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if(app_id != CMIPAPP_ID_INVALID)
  {
    CM_MSG_HIGH_3 (" Found active app %d for call type %d, sys mode %d", 
      app_id, call_type, sys_mode);
  }    
  return app_id;

} /* cmipapp_find_active_app () */



/*===========================================================================

FUNCTION cmipapp_find_active_app_sys_mode

DESCRIPTION
Finds an active app with a particular call type

DEPENDENCIES
None

RETURN VALUE

App id

SIDE EFFECTS
None

===========================================================================*/
sys_sys_mode_e_type  cmipapp_find_active_app_sys_mode (

  cm_call_type_e_type     call_type
  /* call type
  */

  )
{

  unsigned int id_count = 0;
  /* Index
  */
  sys_sys_mode_e_type  sys_mode = SYS_SYS_MODE_NONE;

  uint8    cap_index =0; /*capablities array index*/

  boolean is_match_found=0;
  /*----------------------------------------------------------------------*/

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT_ENUM_IS_INRANGE (call_type, CM_CALL_TYPE_MAX);   /*lint !e641 */

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /* Check is made if there is an app registered
  ** with required call type and system mode
  */
  is_match_found=FALSE;
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    /* CM only routes the service when the IP app is in the active state.
    */
    for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
    {
    if((cmipapp_arr[id_count].app_id != CMIPAPP_ID_INVALID) 
      &&
        (cmipapp_arr[id_count].app_capability.call_type[cap_index] & SYS_BM_64BIT(call_type)) /*lint !e641 */           
      &&
      (cmipapp_arr[id_count].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE ) 
      )
    {
        sys_mode = cmipapp_map_sysmode_to_cm_sysmode(cmipapp_arr[id_count].app_capability.sys_mode[cap_index]);
        is_match_found=TRUE;
      break;
    }
    }
    if(is_match_found==TRUE)
    { break; }
  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  CM_MSG_HIGH_2 (" Found active app sys  mode %d for call type %d", 
    sys_mode, call_type );
  return sys_mode;

} /* cmipapp_find_active_app () */

/*===========================================================================

FUNCTION cmipapp_set_user_cb_data

DESCRIPTION
Sets user data block to IP App data.

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void cmipapp_set_user_cb_data
(
 cmipapp_id_type   app_id,
 uint32            user_data_block
 )
{
  if (!(app_id < ARR_SIZE(cmipapp_arr)))
  {
    CM_MSG_MED_1("app_id %d out of range", app_id);
    return;
  }

  cmipapp_arr[app_id].user_data_block = user_data_block;
  CM_MSG_MED_2("Store data_block %d for app_id %d", user_data_block, app_id);
}

/*===========================================================================

FUNCTION cmipapp_call_type_reg

DESCRIPTION
Allows apps to register call handling capabilities

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK                   - Successfully registered
CMIPAPP_STATUS_CALL_TYPE_REG_FAILED - Error in registering app capabilities

SIDE EFFECTS
None
===========================================================================*/

cmipapp_status_e_type cmipapp_cap_reg (

                                       cmipapp_id_type                 app_id,
                                       /* App id
                                       */

                                       cmipapp_call_type_e_type        call_type,
                                       /* Bit Masks of call type it supports
                                       */

                                       cmipapp_sys_mode_e_type         sys_mode
                                       /* System modes app can support for calls
                                       */
                                       )
                                       /*lint -esym(715,app_id) */
                                       /*lint -esym(715,call_type) */
                                       /*lint -esym(715,sys_mode) */
{
#if defined(FEATURE_IP_CALL)

  uint8    cap_index =0;

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_CALL_TYPE_REG_FAILED;
  }

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE,
    CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, 
    CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */

  /* Access the table to set call_type and system mode
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  CM_MSG_HIGH_0 ("Resetting capabilties before taking new ones");

  for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
  {  
	/* Call types app can service */
    cmipapp_arr[app_id].app_capability.call_type[cap_index]      = CMIPAPP_CALL_TYPE_NONE;
  
	/* Technologies app can work on */
    cmipapp_arr[app_id].app_capability.sys_mode[cap_index]      = CMIPAPP_SYS_MODE_NONE;
  }

  if(cmipapp_cap_update(call_type,sys_mode,app_id) == CMIPAPP_STATUS_OK)
  {
    CM_MSG_HIGH_3 ("cmipapp_cap_reg: Call Type(%d) and Sys Mode(%d) of APP(%d)updated sucessfully",
                   call_type,sys_mode,app_id);
  }

  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  return CMIPAPP_STATUS_OK;
#else
  return CMIPAPP_STATUS_NONE ;
#endif

} /* cmipapp_cap_reg () */
/*lint +esym(715,app_id) */
/*lint +esym(715,call_type) */
/*lint +esym(715,sys_mode) */

/*===========================================================================

FUNCTION cmipapp_ecmp_reg

DESCRIPTION
Allows apps to inform the ECMP (emergency call mode preference) value to CM.
IMS is expected to set the ECMP value that been recieved in 200-OK. 
If not set by IMS, the default value at CM is 3GPP

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK                   - Successfully registered
CMIPAPP_STATUS_CALL_TYPE_REG_FAILED - Error in registering ecmp value

SIDE EFFECTS
None
===========================================================================*/

cmipapp_status_e_type cmipapp_ecmp_reg (

                                       cmipapp_id_type                 app_id,
                                       /* App id
                                       */

                                       cmipapp_ecmp_e_type             ecmp
                                       /* ECMP Value (from 200 ok response)
                                       */
                                       )
                                       /*lint -esym(715,app_id) */
                                       /*lint -esym(715,ecmp) */
{
#if defined(FEATURE_IP_CALL)

  CM_MSG_LOW_0("ECMP phased out API kept for backward compatibility");

  return CMIPAPP_STATUS_OK;
#else
  return CMIPAPP_STATUS_NONE ;
#endif

} /* cmipapp_ecmp_reg () */
/*lint +esym(715,app_id) */
/*lint +esym(715,call_type) */
/*lint +esym(715,sys_mode) */

/*===========================================================================

FUNCTION cmipapp_cap_reg_with_reason

DESCRIPTION
Allows clients to register call handling capabilities

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
TRUE - Successfully registered, FALSE - Error in registering

SIDE EFFECTS
None
===========================================================================*/

cmipapp_status_e_type cmipapp_cap_reg_with_reason (

  cmipapp_id_type                 app_id,
  /* App ID
  */

  cmipapp_call_type_e_type        call_type,
  /* Bit Masks of call type it supports
  */

  cmipapp_sys_mode_e_type         sys_mode,
  /* System modes app can support for calls
  */

  cmipapp_cap_reg_reason_s_type   reason
  /* reason carries success or details of failure  
  ** with respect to app registeration with network.
  */  
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,call_type) */
  /*lint -esym(715,sys_mode) */
  /*lint -esym(715,reason) */
{
#if defined(FEATURE_IP_CALL)
  cmipapp_status_e_type  app_reg_status = CMIPAPP_STATUS_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_CALL_TYPE_REG_FAILED;
  }

  /* Sanity check on call_type and system mode
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE, CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */

  /* Access the table to set call_type and system mode  
  */  
  app_reg_status = cmipapp_cap_reg (app_id, call_type, sys_mode);

  if (app_reg_status == CMIPAPP_STATUS_OK)
  { 
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());
    cmipapp_arr[app_id].reason         = reason; 
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());
  }

  return app_reg_status;
#else
  return CMIPAPP_STATUS_NONE ;
#endif
}
/*lint +esym(715,app_id) */
/*lint +esym(715,call_type) */
/*lint +esym(715,sys_mode) */
/*lint +esym(715,reason) */

/*===========================================================================

FUNCTION cmipapp_inform_status

DESCRIPTION
App informs CM if it is active or inactive

DEPENDENCIES
cmipapp_init

RETURN VALUE
CMIPAPP_STATUS_OK             - Successfully
CMIPAPP_STATUS_INFORM_FAILED  - Error in processing app inform cmd

SIDE EFFECTS
None
===========================================================================*/
cmipapp_status_e_type cmipapp_inform_status (

  cmipapp_id_type                app_id,
  /* App ID
  */

  cmipapp_run_status_e_type      app_run_status
  /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
  ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,app_run_status) */
{ 
#if defined(FEATURE_IP_CALL)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  


  CM_MSG_HIGH_2 (" App %d informs status %d",
    app_id, app_run_status);

  /* Check for invalid app id
  */
  if (app_id == CMIPAPP_ID_INVALID)
  {
    return CMIPAPP_STATUS_INFORM_FAILED;
  }

  /* Sanity check on app run status
  */
  CM_ASSERT_ENUM_IS_INRANGE (app_run_status, CMIPAPP_RUN_STATUS_MAX); /*lint !e641 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  CM_MSG_HIGH_1 ("App run status Failure: %d", cmipapp_arr[app_id].app_run_status);

  /* Status transition from Inactive to Inactive
  */
  if ( cmipapp_arr[app_id].app_run_status == CMIPAPP_RUN_STATUS_INACTIVE 
    &&
    app_run_status ==  CMIPAPP_RUN_STATUS_INACTIVE
    )
  {
    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_FAILURE);

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  /* Status transition from Active to Inactive
  */
  if ( cmipapp_arr[app_id].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE 
    &&
    app_run_status ==  CMIPAPP_RUN_STATUS_INACTIVE
    )
  {
    /* Access the table to set the app status
    */
    cmipapp_arr[app_id].app_run_status = app_run_status;

    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_DEREGISTERED);

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status transition to Active.
  ** does not depend on current state.
  */
  if (app_run_status ==  CMIPAPP_RUN_STATUS_ACTIVE)
  {
    /* for ipapp to be made Active, all callbacks should be available
    */
    if ( ((cmipapp_arr[app_id].app_cb.cmipapp_cmd_orig_func_ptr == NULL) &&
        (cmipapp_arr[app_id].app_cb.cmipapp_cmd_orig_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.
      cmipapp_cmd_invite_res_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.
      cmipapp_cmd_invite_res_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_sub_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_ext_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.
      cmipapp_inband_cmd_start_cont_dtmf_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.
      cmipapp_inband_cmd_start_cont_dtmf_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.
      cmipapp_inband_cmd_stop_cont_dtmf_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.
      cmipapp_inband_cmd_stop_cont_dtmf_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.cmipapp_cmd_hold_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_hold_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.cmipapp_cmd_active_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.cmipapp_cmd_active_sub_func_ptr == NULL))
      ||
      ((cmipapp_arr[app_id].app_cb.
      cmipapp_cmd_modify_func_ptr == NULL) &&
       (cmipapp_arr[app_id].app_cb.
      cmipapp_cmd_modify_sub_func_ptr == NULL))
      )
    {
      return CMIPAPP_STATUS_INFORM_FAILED;
    }

    /* Access the table to set the app status
    */
    cmipapp_arr[app_id].app_run_status = app_run_status;

    cmipapp_send_reg_status (app_id, CM_IPAPP_REG_STATUS_REGISTERED); 

    CM_MSG_HIGH_1 (" App %d is ACTIVE", app_id);

    return CMIPAPP_STATUS_OK;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   

  CM_ERR_0 (" run_status is invalid"); 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CMIPAPP_STATUS_INFORM_FAILED;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_inform_status () */
/*lint +esym(715,app_id) */
/*lint +esym(715,app_run_status) */

/*===========================================================================

FUNCTION cmipapp_inform_status_with_callid

DESCRIPTION
App informs CM if videoshare call for a voice call is available

DEPENDENCIES
cmipapp_init
General status of the app should be active

RETURN VALUE
CMIPAPP_STATUS_OK             - Successfully
CMIPAPP_STATUS_INFORM_FAILED  - Error in processing app inform cmd

SIDE EFFECTS
None
===========================================================================*/
cmipapp_status_e_type cmipapp_inform_status_with_callid (

  cmipapp_id_type                app_id,
  /* App ID
  */

  cmipapp_run_status_e_type      app_run_status,
  /* CMIPAPP_RUN_STATUS_ACTIVE  - App is active
  ** CMIPAPP_RUN_STATUS_INACTIVE  - App is inactive
  */

  cm_call_id_type                voice_call_id
  /* voice_call_id for which videoshare is available
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,app_run_status) */
  /*lint -esym(715,voice_call_id) */
{ 
#if defined(FEATURE_IP_CALL) 
  cmipapp_ind_s_type      ip_ind;
  /* cm ipapp report */

  memset(&ip_ind, 0, sizeof(cmipapp_ind_s_type));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for invalid app id
  */
  if ((app_id == CMIPAPP_ID_INVALID) ||
    /*lint -e641*/
    (cmipapp_arr[app_id].app_run_status != CMIPAPP_RUN_STATUS_INACTIVE) 
    /*lint +e641*/
    )
  {
    return CMIPAPP_STATUS_INFORM_FAILED;
  }

  /* Sanity check on app run status
  */
  CM_ASSERT_ENUM_IS_INRANGE (app_run_status, CMIPAPP_RUN_STATUS_MAX); /*lint !e641 */


  /*  
  ** Report videoshare availability to CM
  */

  /* Queue a report informing CM that videoshare is available for 
  ** voice_call_id
  */
  ip_ind.ind.vs_status.id         = voice_call_id;

  /* Active app maps to videoshare being available.
  ** Inactive app maps to videoshare not being available
  */
  if (app_run_status == CMIPAPP_RUN_STATUS_INACTIVE)
  {
    ip_ind.ind.vs_status.is_vs_poss = FALSE;  
  }
  else if (app_run_status == CMIPAPP_RUN_STATUS_ACTIVE)
  {
    ip_ind.ind.vs_status.is_vs_poss = TRUE;
  }

  CM_MSG_HIGH_2("CM->CM: CM_IP_CALL_IND_VS_STATUS, voice_call_id %d,app_run_status %d",
                 voice_call_id, 
                 app_run_status);

  /* data_block is useful during the course of a call.
  ** Since this api will be getting called to inform 
  ** a general status, data_block support to app is not
  ** needed.
  */
  cmipapp_rpt_ind (CM_IP_CALL_IND_VS_STATUS, ip_ind, 0);

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_inform_status_with_callid () */
/*lint +esym(715,app_id) */
/*lint +esym(715,app_run_status) */
/*lint +esym(715,voice_call_id) */

/*===========================================================================

FUNCTION cmipapp_id_allocate

DESCRIPTION
Returns a free index from cmipapp_arr[]

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_APPID_PTR_ERR - Supplied app id ptr is NULL

CMIPAPP_STATUS_APPID_EXHAUST - No free app id available

CMIPAPP_STATUS_OK            - App id successfully allocated

SIDE EFFECTS

Modifies global cmipapp_arr[].
===========================================================================*/
cmipapp_status_e_type cmipapp_id_allocate (

  cmipapp_call_type_e_type     call_type,
  /* App type
  */

  cmipapp_sys_mode_e_type      sys_mode,
  /* System modes app can support for calls
  */

  cmipapp_id_type             *app_id_ptr
  /* App ID pointer
  */
  )
  /*lint -esym(715,call_type) */
  /*lint -esym(715,sys_mode) */
  /*lint -esym(715,app_id_ptr) */
  /*lint -esym(818,app_id_ptr) */
{
#if defined(FEATURE_IP_CALL)
  cmipapp_id_type             app_id = CMIPAPP_ID_INVALID;
  /* Stores the app id returned by cmipapp_id_allocate ()
  */

  boolean  is_duplicate_app_present  = FALSE;
  /* Checks for the presence of a duplicate
  ** app with same call_type and system mode
  */

  cmipapp_id_type         id_count   = 0;
  /* App id loop
  */
  uint8                  cap_index   = 0;  

  /*----------------------------------------------------------------------*/


  /* Verify if the call type and system mode values are valid
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE, 
    CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, 
    CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */


  if (app_id_ptr == NULL)
  {
    return CMIPAPP_STATUS_APPID_PTR_ERR;
  }

  /* Only one app can exist for a combination of
  ** call type and system mode. Travel through
  ** the list of apps to find a duplicate, if
  ** none get a app_id returned.
  */

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /* Check is made if there is an alternate app registered
  ** with same call type and system mode
  */
  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
    {

    if ((cmipapp_arr[id_count].app_id    != CMIPAPP_ID_INVALID)
      &&
        (cmipapp_arr[id_count].app_capability.call_type[cap_index] == call_type)
      &&
        (cmipapp_arr[id_count].app_capability.sys_mode[cap_index] == sys_mode)
      )
    {
      is_duplicate_app_present = TRUE;
      break;
    }
    }
    if(is_duplicate_app_present==TRUE)
        {break;}
  }

  /* If there is no duplicate app registered
  ** allocate a app id
  */
  if (!is_duplicate_app_present)
  {
    for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
    {
      if (cmipapp_arr[id_count].app_id == CMIPAPP_ID_INVALID)
      {
        app_id                      = id_count;

        cmipapp_reset (app_id);
        cmipapp_arr[app_id].app_id  = app_id;

        /* Access the table to set sys_mode and call type */
        if(cmipapp_cap_update(call_type,sys_mode,app_id) == CMIPAPP_STATUS_OK)
        {
          CM_MSG_HIGH_3 ("Call Type(%d) and Sys Mode(%d) of APP(%d)updated sucessfully",
                          call_type,sys_mode,app_id);
        }
        break;
      }
    }
  }
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if (app_id == CMIPAPP_ID_INVALID)
  {
    if (is_duplicate_app_present)
    {
      return CMIPAPP_STATUS_DUP_APP_EXISTS;
    }
    else
    {
      return CMIPAPP_STATUS_APPID_EXHAUST;
    }
  }

  CM_MSG_HIGH_1( "App id being allocated = %d", app_id);


  /* Set app id ptr to the assigned app id
  */
  *app_id_ptr = app_id;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_id_allocate () */
/*lint +esym(715,call_type) */
/*lint +esym(715,sys_mode) */
/*lint +esym(715,app_id_ptr) */
/*lint +esym(818,app_id_ptr) */


/*===========================================================================

FUNCTION cmipapp_cap_update

DESCRIPTION
Updates the capabilties array.
If there is a duplicate of the same call type already in the 
capabilities array, the capability in the array is replaced 
with this new one

DEPENDENCIES
cmipapp_init

RETURN VALUE

CMIPAPP_STATUS_OK            - Capabilities successfully updated

SIDE EFFECTS

Modifies global cmipapp_arr[].
===========================================================================*/
cmipapp_status_e_type cmipapp_cap_update (

  cmipapp_call_type_e_type     call_type,
  /* App type
  */

  cmipapp_sys_mode_e_type      sys_mode,
  /* System modes app can support for calls
  */

  cmipapp_id_type             app_id
  /* App ID pointer
  */
  )
  /*lint -esym(715,call_type) */
  /*lint -esym(715,sys_mode) */
  /*lint -esym(715,app_id_ptr) */
  /*lint -esym(818,app_id_ptr) */
{
#if defined(FEATURE_IP_CALL)  
  uint8    cap_index =0;

  /*----------------------------------------------------------------------*/


  /* Verify if the call type and system mode values are valid
  */
  CM_ASSERT (BETWEEN (call_type, CMIPAPP_CALL_TYPE_NONE, 
    CMIPAPP_CALL_TYPE_MAX)); /*lint !e641 */
  /*Commenting this since sys mode may be NONE to remove some capability*/
  //CM_ASSERT (BETWEEN (sys_mode, CMIPAPP_SYS_MODE_NONE, 
    //CMIPAPP_SYS_MODE_MAX));   /*lint !e641 */

  /* Only one app can exist for a combination of
  ** call type and system mode. Travel through
  ** the list of apps to find a duplicate, if
  ** none add the value, if value already present, replace
  */

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /* Check is made if there is an alternate app registered
        with same call type and system mode

    currently the call types supported over IP are 
    CM_CALL_TYPE_VOICE
    CM_CALL_TYPE_VS
    CM_CALL_TYPE_EMERGENCY
    CM_CALL_TYPE_VT
    CM_CALL_TYPE_SMS
    Adding support for only the supported call types.
    Need to add support here when new call types are supported*/

  CM_MSG_HIGH_3( "cmipapp_cap_update (App %d): CallTypeMask:%d and SysmodeMask%d",
                 app_id, call_type,sys_mode );

  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_VOICE))
  {
      
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_VOICE] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_VOICE] = SYS_BM_64BIT(CM_CALL_TYPE_VOICE);
    
  }
  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_EMERGENCY))
  {
    
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_EMERGENCY] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_EMERGENCY] = SYS_BM_64BIT(CM_CALL_TYPE_EMERGENCY);    
  }
  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_VT))
  {
      
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_VT] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_VT] = SYS_BM_64BIT(CM_CALL_TYPE_VT);        
  }
  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_VS))
  {
    
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_VS] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_VS] = SYS_BM_64BIT(CM_CALL_TYPE_VS);            
  }
  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_SMS))
  {
    
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_SMS] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_SMS] = SYS_BM_64BIT(CM_CALL_TYPE_SMS);            
  }
  if(call_type & SYS_BM_64BIT(CM_CALL_TYPE_SUPS))
  {
    
    cmipapp_arr[app_id].app_capability.sys_mode[CM_CALL_TYPE_SUPS] = sys_mode;
    cmipapp_arr[app_id].app_capability.call_type[CM_CALL_TYPE_SUPS] = SYS_BM_64BIT(CM_CALL_TYPE_SUPS);            
  }
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());
  
  CM_MSG_HIGH_1 ("Current capabilties for App ID %d are:",app_id);
  for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
  {
    if(cmipapp_arr[app_id].app_capability.sys_mode[cap_index] || cmipapp_arr[app_id].app_capability.call_type[cap_index])
      CM_MSG_HIGH_2( "CallTypeMask:%d , SysmodeMask%d",
                     cmipapp_arr[app_id].app_capability.call_type[cap_index],
                     cmipapp_arr[app_id].app_capability.sys_mode[cap_index]);
  }

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_cap_update () */
/*lint +esym(715,call_type) */
/*lint +esym(715,sys_mode) */
/*lint +esym(715,app_id_ptr) */
/*lint +esym(818,app_id_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_orig_func

DESCRIPTION
Registers function for the app to originate a call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_orig_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_orig_f_type                 *cmipapp_cmd_orig_func_ptr
  /* Ptr to call orig function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_orig_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_orig_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Origination function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_orig_func_ptr = 
    cmipapp_cmd_orig_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_orig_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_orig_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_answer_func

DESCRIPTION
Registers function for the app to answer a MT call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_answer_f_type              *cmipapp_cmd_answer_func_ptr
  /* Ptr to MT call answer function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_answer_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_answer_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Answer function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_func_ptr = 
    cmipapp_cmd_answer_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_answer_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_answer_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_answer_call_waiting_func

DESCRIPTION
Registers function for the app to answer an call waiting MT call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_call_waiting_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_answer_call_waiting_sub_f_type *answer_call_waiting_func_ptr
  /* Ptr to MT call answer function
  */
  )
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!answer_call_waiting_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Answer function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_answer_call_waiting_sub_func_ptr = 
    answer_call_waiting_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_answer_call_waiting_func () */

/*===========================================================================

FUNCTION cmipapp_reg_simple_flash_func

DESCRIPTION
Registers function for the app to answer an call waiting MT call or
to remove the last party added to the conference call

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Registration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_simple_flash_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_simple_flash_sub_f_type *simple_flash_func_ptr
  /* Ptr to simple flash request function
  */
  )
{

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!simple_flash_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Flash function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_simple_flash_sub_func_ptr = 
    simple_flash_func_ptr;

  return CMIPAPP_STATUS_OK;

} /* cmipapp_reg_simple_flash_func () */

/*===========================================================================

FUNCTION cmipapp_reg_invite_res_func

DESCRIPTION
Registers function for the app to respond to a invite message.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_invite_res_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_invite_res_f_type          *cmipapp_cmd_invite_res_func_ptr
  /* Ptr to invite response function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_invite_res_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_invite_res_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* Invite response function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_invite_res_func_ptr = 
    cmipapp_cmd_invite_res_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_invite_res_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_invite_res_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_end_func

DESCRIPTION
Registers function for the app to end a call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_end_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_end_f_type                 *cmipapp_cmd_end_func_ptr
  /* Ptr to call end function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_end_func_ptr) */

{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_end_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_func_ptr = 
    cmipapp_cmd_end_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_end_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_end_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_end_ext_func

DESCRIPTION
Registers function for the app to end a call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_end_ext_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_end_ext_sub_f_type                 *cmipapp_cmd_end_ext_sub_func_ptr
  /* Ptr to call end function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_reg_end_ext_func) */

{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_end_ext_sub_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_end_ext_sub_func_ptr = 
    cmipapp_cmd_end_ext_sub_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_end_ext_func () */


/*===========================================================================

FUNCTION cmipapp_reg_inband_start_cont_dtmf_func

DESCRIPTION
Registers function for the app to start continuous inband dtmf.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inband_start_cont_dtmf_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_inband_cmd_start_cont_dtmf_f_type

  *cmipapp_inband_cmd_start_cont_dtmf_func_ptr
  /* Ptr to start inband dtmf function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_inband_cmd_start_cont_dtmf_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_inband_cmd_start_cont_dtmf_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to start cont dtmf */
  cmipapp_arr[app_id].app_cb.cmipapp_inband_cmd_start_cont_dtmf_func_ptr = 
    cmipapp_inband_cmd_start_cont_dtmf_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_inband_start_cont_dtmf_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_inband_cmd_start_cont_dtmf_func_ptr) */



/*===========================================================================

FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func

DESCRIPTION
Registers function for the app to stop inband continuous dtmf.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inband_stop_cont_dtmf_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_inband_cmd_stop_cont_dtmf_f_type

  *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr
  /* Ptr to stop inband dtmf function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_inband_cmd_stop_cont_dtmf_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_inband_cmd_stop_cont_dtmf_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to stop cont dtmf */
  cmipapp_arr[app_id].app_cb.cmipapp_inband_cmd_stop_cont_dtmf_func_ptr = 
    cmipapp_inband_cmd_stop_cont_dtmf_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_inband_stop_cont_dtmf_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_inband_cmd_stop_cont_dtmf_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_hold_func

DESCRIPTION
Registers function for the app to hold a call

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_hold_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_hold_f_type                *cmipapp_cmd_hold_func_ptr
  /* Ptr to call hold function 
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_hold_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_hold_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to Hold call */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_hold_func_ptr = 
    cmipapp_cmd_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_hold_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_hold_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_active_func

DESCRIPTION
Registers function for the app to make the call active.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_active_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_active_f_type              *cmipapp_cmd_active_func_ptr
  /* Ptr to call active function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_active_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_active_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to put call as active */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_active_func_ptr = 
    cmipapp_cmd_active_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_active_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_active_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_modify_func

DESCRIPTION
Registers function for the app to modify the call type (ex:- voice to VT).

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_modify_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_modify_f_type              *cmipapp_cmd_modify_func_ptr
  /* Ptr to call modify function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_modify_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_modify_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to modify call */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_modify_func_ptr = 
    cmipapp_cmd_modify_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_modify_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_modify_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_serv_sys_info_func

DESCRIPTION
Registers function for the app to inform app about current serving system.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_serv_sys_info_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_srv_sys_info_sub_f_type        *cmipapp_cmd_srv_sys_info_func_ptr
  /* Ptr to serving system info
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_srv_sys_info_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_srv_sys_info_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to inform about serving system change */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_serv_sys_info_sub_ptr = 
    cmipapp_cmd_srv_sys_info_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

}
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_srv_sys_info_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_act_answer_hold_func

DESCRIPTION
Registers function for the app to activate answer hold.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_act_answer_hold_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_act_answer_hold_sub_f_type 
  *cmipapp_cmd_act_answer_hold_func_ptr
  /* Ptr to activate answer hold function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_act_answer_hold_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_act_answer_hold_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to activate answer hold */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_act_answer_hold_sub_func_ptr = 
    cmipapp_cmd_act_answer_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_act_answer_hold_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_act_answer_hold_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_deact_answer_hold_func

DESCRIPTION
Registers function for the app to de-activate answer hold.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_deact_answer_hold_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_deact_answer_hold_sub_f_type 
  *cmipapp_cmd_deact_answer_hold_func_ptr
  /* Ptr to de-activate answer hold function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_deact_answer_hold_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_deact_answer_hold_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment to deactivate answer hold */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_deact_answer_hold_sub_func_ptr = 
    cmipapp_cmd_deact_answer_hold_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_deact_answer_hold_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_deact_answer_hold_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_ect_func

DESCRIPTION
Registers function for the app to provide Call transfer funcitonality.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_ect_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_ect_sub_f_type 
  *cmipapp_cmd_ect_func_ptr
  /* Ptr to call transfer function.
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_ect_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_ect_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for ect */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_ect_sub_func_ptr = 
    cmipapp_cmd_ect_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_ect_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_ect_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_ip_explicit_call_transfer_func

DESCRIPTION
Registers function for the app to provide IP Explicit Call transfer funcitonality.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_ip_explicit_call_transfer_func (

  cmipapp_id_type                                  app_id,
  /* App id assigned by CM */

  cmipapp_cmd_ip_explicit_call_transfer_f_type    *cmipapp_cmd_ip_ect_sub_func_ptr
  /* Ptr to call ip explicit call transfer function   */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_ect_func_ptr) */
{
#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_ip_ect_sub_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for ect */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_ip_ect_sub_func_ptr = 
    cmipapp_cmd_ip_ect_sub_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  CM_MSG_HIGH_0 ("FEATURE_IP_CALL is not defined");
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_ect_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_ect_func_ptr) */

/*===========================================================================

FUNCTION cmipapp_reg_mpty_func

DESCRIPTION
Registers function for the app to provide Multiparty functionality.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_mpty_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_mpty_sub_f_type 
  *cmipapp_cmd_mpty_func_ptr
  /* Ptr to de-activate answer hold function
  */

  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_cmd_mpty_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_mpty_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for mpty */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_mpty_sub_func_ptr = 
    cmipapp_cmd_mpty_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_mpty_func () */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_cmd_mpty_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_srv_domain_func

DESCRIPTION
Registers function for the app to be informed of Phone's srv_domain.  

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
CMIPAPP_STATUS_OK             - Registration successfull.
CMIPAPP_STATUS_CB_REG_FAILED  - Registration failed.

SIDE EFFECTS
None
===========================================================================*/
cmipapp_status_e_type cmipapp_reg_srv_domain_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_inform_srv_domain_sub_f_type 
  *cmipapp_inform_srv_domain_func_ptr
  /* Ptr to record srv_domain function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_inform_srv_domain_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_inform_srv_domain_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  CM_MSG_HIGH_0 ("Registering srv_domain_func");

  /* Registering cb for informing app phone's srv_domain in case of DETACH */
  cmipapp_arr[app_id].app_cb.cmipapp_inform_srv_domain_sub = 
    cmipapp_inform_srv_domain_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_srv_domain_func */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_inform_srv_domain_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_oprt_mode_func

DESCRIPTION
Registers function for the app to be informed of Phone's operating mode.  

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
CMIPAPP_STATUS_OK             - Registration successfull.
CMIPAPP_STATUS_CB_REG_FAILED  - Registration failed.

SIDE EFFECTS
None
===========================================================================*/
cmipapp_status_e_type cmipapp_reg_oprt_mode_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_inform_oprt_mode_f_type 
  *cmipapp_inform_oprt_mode_func_ptr
  /* Ptr to record oprt mode function
  */
  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_inform_oprt_mode_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_inform_oprt_mode_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  CM_MSG_HIGH_0 ("Registering oprt_mode_func");

  /* Registering cb for informing app phone's operating mode */
  cmipapp_arr[app_id].app_cb.cmipapp_inform_oprt_mode = 
    cmipapp_inform_oprt_mode_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

} /* cmipapp_reg_oprt_mode_func */
/*lint +esym(715,app_id) */
/*lint +esym(715,cmipapp_inform_oprt_mode_func_ptr) */


/*===========================================================================

FUNCTION cmipapp_reg_subs_cmd_func

DESCRIPTION
  Registers function for the app to be informed of Phone's subscription change .
  Not currently used by IMS.   

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK             - Registration successfull.
  CMIPAPP_STATUS_CB_REG_FAILED  - Registration failed.

SIDE EFFECTS
  None
===========================================================================*/
cmipapp_status_e_type cmipapp_reg_subs_cmd_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_send_subsc_chngd_f_type
                                      *cmipapp_sub_cmd_func_ptr
    /* Ptr to record subs cmd function
    */
)

{
  #if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_sub_cmd_func_ptr)
     )
  {
	 return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  CM_MSG_HIGH_0 ("Registering subs_cmd_cb");

  /* Registering cb for informing app phone's subscription */
  cmipapp_arr[app_id].app_cb.cmipapp_subsc_chgnd = 
                                     cmipapp_sub_cmd_func_ptr;

  return CMIPAPP_STATUS_OK;
  
  #else
  return CMIPAPP_STATUS_NONE;
  #endif

} /* cmipapp_reg_subs_cmd_func */


/*===========================================================================

FUNCTION cmipapp_reg_mo_sms_func

DESCRIPTION
Registers function for the app to be informed of MO SMS message.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_mo_sms_func (

  cmipapp_id_type                      app_id,
  /* App id assigned by CM
  */

  cmipapp_sms_cmd_mo_sms_sub_f_type 
  *cmipapp_sms_cmd_mo_sms_func_ptr
  /* Ptr to send MO SMS message function
  */

  )
  /*lint -esym(715,app_id) */
  /*lint -esym(715,cmipapp_sms_cmd_mo_sms_func_ptr) */
{
#if defined(FEATURE_IP_CALL)
  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( app_id >= ARR_SIZE(cmipapp_arr) ||     
    ( ! cmipapp_sms_cmd_mo_sms_func_ptr )
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* function assigment for mpty */
  cmipapp_arr[app_id].app_cb.cmipapp_sms_cmd_mo_sms_sub_func_ptr= 
    cmipapp_sms_cmd_mo_sms_func_ptr;

  return CMIPAPP_STATUS_OK;

#else
  return CMIPAPP_STATUS_NONE;
#endif

}/* cmipapp_reg_mo_sms_func */
/*lint -esym(715,app_id) */
/*lint -esym(715,cmipapp_sms_cmd_mo_sms_func_ptr) */

/*===========================================================================

FUNCTION cmipapp_reg_abort_func

DESCRIPTION
Registers function for the app to abort a call.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_abort_func (

  cmipapp_id_type                         app_id,
  /* App id assigned by CM
  */

  cmipapp_cmd_abort_sub_f_type                *cmipapp_cmd_abort_func_ptr
  /* Ptr to call end function
  */
  )
{

#if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
    (!cmipapp_cmd_abort_func_ptr)
    )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_abort_sub_func_ptr = 
    cmipapp_cmd_abort_func_ptr;

  return CMIPAPP_STATUS_OK;

#else

  SYS_ARG_NOT_USED(app_id);
  SYS_ARG_NOT_USED(cmipapp_cmd_abort_func_ptr);

  CM_ERR_0("cmipapp_reg_abort_func is not supported");

  return CMIPAPP_STATUS_CB_REG_FAILED;

#endif /* FEATURE_IP_CALL */

} /* cmipapp_reg_abort_func () */


/*===========================================================================

FUNCTION cmipapp_reg_call_deflection_func

DESCRIPTION
  Registers function for the app to handle communication deflection.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_call_deflection_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_call_deflection_sub_f_type      *cmipcall_call_deflection_cmd_func_ptr
    /* Ptr to call end function
    */
)
{

  #if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipcall_call_deflection_cmd_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipcall_call_deflection_cmd_sub_func_ptr = 
                                  cmipcall_call_deflection_cmd_func_ptr;

  return CMIPAPP_STATUS_OK;

  #else

  SYS_ARG_NOT_USED(app_id);
  SYS_ARG_NOT_USED(cmipcall_call_deflection_cmd_func_ptr);

  CM_ERR_0("cmipcall_call_deflection_cmd_func_ptr is not supported");

  return CMIPAPP_STATUS_CB_REG_FAILED;

  #endif /* FEATURE_IP_CALL */

} /* cmipapp_reg_abort_func () */



/*===========================================================================

FUNCTION cmipapp_reg_secure_call_capability_func

DESCRIPTION
  Registers function for the app to handle secure call capability

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inform_secure_call_capability_func (

  cmipapp_id_type                                    app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_inform_secure_call_capability_f_type  *cmipapp_cmd_inform_secure_call_capability_func_ptr
    /* Ptr to Secure call capability function
    */
)
{

  #if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_inform_secure_call_capability_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  cmipapp_arr[app_id].app_cb.cmipapp_cmd_inform_secure_call_capability_func_ptr = 
    cmipapp_cmd_inform_secure_call_capability_func_ptr;

  return CMIPAPP_STATUS_OK;

  #else

  SYS_ARG_NOT_USED(app_id);
  SYS_ARG_NOT_USED(cmipapp_cmd_inform_secure_call_capability_func_ptr);

  CM_ERR_0("cmipapp_reg_inform_secure_call_capability_func is not supported");

  return CMIPAPP_STATUS_CB_REG_FAILED;

  #endif /* FEATURE_IP_CALL */

} /* cmipapp_reg_abort_func () */



/*===========================================================================

FUNCTION cmipapp_reg_inform_secure_context_func

DESCRIPTION
  Registers function for the app to send secure context info

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_inform_secure_context_func (

  cmipapp_id_type                             app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_inform_secure_context_f_type    *cmipapp_cmd_inform_secure_context_func_ptr
    /* Ptr to secure_context function
    */
)
{

  #if defined(FEATURE_IP_CALL)

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)) ||     
        (!cmipapp_cmd_inform_secure_context_func_ptr)
     )
  {
    return CMIPAPP_STATUS_CB_REG_FAILED;
  }

  /* End function assigment */
  cmipapp_arr[app_id].app_cb.cmipapp_cmd_inform_secure_context_func_ptr = 
                                  cmipapp_cmd_inform_secure_context_func_ptr;

  return CMIPAPP_STATUS_OK;

  #else

  SYS_ARG_NOT_USED(app_id);
  SYS_ARG_NOT_USED(cmipapp_cmd_inform_secure_context_func_ptr);

  CM_ERR_0("cmipapp_reg_inform_secure_context_func is not supported");

  return CMIPAPP_STATUS_CB_REG_FAILED;

  #endif /* FEATURE_IP_CALL */

} /* cmipapp_reg_abort_func () */


#if defined(FEATURE_IP_CALL)
/*===========================================================================

FUNCTION cmipapp_rpt_get_buf_else_err_fatal

DESCRIPTION
Allocate a IP app rpt buffer. Error fatal if allocation is NOT
successful.

DEPENDENCIES
cmipapp_init

RETURN VALUE
Pointer to allocated IP app rpt buffer.

SIDE EFFECTS
none

===========================================================================*/
static cmipapp_rpt_s_type *cmipapp_rpt_get_buf_else_err_fatal (

  void
  )
{

  cmipapp_rpt_s_type         *cmipapp_rpt_ptr = NULL;
  /* CM report command buffer */

  /*----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) cm_mem_malloc(sizeof(cmipapp_rpt_s_type));

  return cmipapp_rpt_ptr;

}  /* cmipapp_rpt_get_buf_else_err_fatal */

/*===========================================================================

FUNCTION cmipapp_add_reference

DESCRIPTION
Add reference to the reference counted object

DEPENDENCIES

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipapp_add_reference
(

 const cmipapp_rpt_s_type  *cmipapp_rpt_ptr
 /* Pointer to a IP app report */
 )
{
  switch( cmipapp_rpt_ptr->hdr.cmd )
  {
  case CM_IP_CALL_IND_SUPS_CONF:
    if(cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.sups_conf.participants);
    }
    break;
  case CM_IP_CALL_JOIN_CONF_CALL:
    if(cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.join_conf_call.participant_uri);
    }
    break;
  case CM_IP_CALL_CONF_INFO:
    if(cmipapp_rpt_ptr->ip_ind.ind.conf_info.conf != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.conf_info.conf);
    }
    break;

  case CM_IP_CALL_IND_MT_INVITE:
    if(cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info.ip_gen_param != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info.ip_gen_param);
    }

	if (cmipapp_rpt_ptr->ip_ind.ind.mt_invite.caller_info != NULL)
	{
	    CM_MSG_HIGH_0("cmippap_add_ref :Added ref for caller_info, info_text");
	    ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.mt_invite.caller_info);
	}

    break;

  case CM_IP_CALL_IND_CALL_UPDATE_INFO:
    if(cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name);
    }
    break;

  case CM_IP_CALL_IND_FORWARD_INFO:
    if(cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info);
    }
    break;

  case CM_IP_SUPS_ERASE_SS_CONF:
	if(cmipapp_rpt_ptr->ip_ind.ind.erase_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.erase_ss_conf.sups_ss_error_text);
    }
    break;

  case  CM_IP_SUPS_ACTIVATE_SS_CONF:  
    if(cmipapp_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_error_text);
    }
    break;

  case CM_IP_SUPS_DEACTIVATE_SS_CONF: 
    if(cmipapp_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_error_text);
    }
    break;

  case CM_IP_SUPS_REGISTER_SS_CONF: 
	if(cmipapp_rpt_ptr->ip_ind.ind.register_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.register_ss_conf.sups_ss_error_text);
    }
    break;	

   case CM_IP_SUPS_INTERROGATE_SS_CONF:
	if(cmipapp_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.call_barring_num_info_list != NULL)
	{
	   ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.call_barring_num_info_list);
           CM_MSG_HIGH_0 ("Added ref for call_barring_num_info_list");
	}
	break;
	
   case CM_IP_CALL_IND_CALL_END:
	if(cmipapp_rpt_ptr->ip_ind.ind.call_end.end_reason != NULL)
    {
	  ref_cnt_obj_add_ref(cmipapp_rpt_ptr->ip_ind.ind.call_end.end_reason);
      CM_MSG_MED_0("cmippap_add_ref :Added ref for end reason");
		
	}
	break;

  default:
    break;
  }

}

/*===========================================================================

FUNCTION cmipapp_release_reference_end_reason

DESCRIPTION
Release reference to the reference counted object during call end 

DEPENDENCIES

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipapp_release_reference_end_reason
(  
  cmcall_s_type           *call_ptr 
)
{

 if(call_ptr!= NULL)
 {
  if( call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason != NULL)
  {
	CM_MSG_HIGH_0("Released ref for end reason");
    REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason);
  }
 }
}

/*===========================================================================

FUNCTION cmipapp_release_reference

DESCRIPTION
Release reference to the reference counted object

DEPENDENCIES

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void cmipapp_release_reference
(

 const cmipapp_rpt_s_type  *cmipapp_rpt_ptr
 /* Pointer to a IP app report */
 )
{
  switch( cmipapp_rpt_ptr->hdr.cmd )
  {

  case CM_IP_SUPS_ERASE_SS_CONF:
	if(cmipapp_rpt_ptr->ip_ind.ind.erase_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.erase_ss_conf.sups_ss_error_text);
    }
    break;

  case  CM_IP_SUPS_ACTIVATE_SS_CONF:  
    if(cmipapp_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.activate_ss_conf.sups_ss_error_text);
    }
    break;

  case CM_IP_SUPS_DEACTIVATE_SS_CONF: 
    if(cmipapp_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.deactivate_ss_conf.sups_ss_error_text);
    }
    break;

  case CM_IP_SUPS_REGISTER_SS_CONF: 
	if(cmipapp_rpt_ptr->ip_ind.ind.register_ss_conf.sups_ss_error_text != NULL)
    {
      ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.register_ss_conf.sups_ss_error_text);
    }
    break;	

  case CM_IP_SUPS_INTERROGATE_SS_CONF:
    if(cmipapp_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.call_barring_num_info_list != NULL)
    {
      ref_cnt_obj_release(cmipapp_rpt_ptr->ip_ind.ind.interrogate_ss_conf.interrogate_ss_res.call_barring_num_info_list);
      CM_MSG_HIGH_0( "Released ref for call_barring_num_info_list");
    }
    break;	

  default:
    break;
  }

}


#endif /*   #if defined(FEATURE_IP_CALL) */

/*===========================================================================

FUNCTION cmipapp_rpt_ind

DESCRIPTION
Allows Voip Apps to queue up messages to the CM. Grab a buffer from the
cmipapp_rpt_free_q, fill it in, and call cmipapp_rpt_ind with it.
All this will do is queue it and set a signal for the CM task.  When the
command is done, the buffer will be queued as directed in the command
header, and a task you specify may be signaled.

DEPENDENCIES
cm_ip_rpt_free_q already initialized, cmipapp_init

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
void cmipapp_rpt_ind (

                      cm_name_type        ip_rpt_name,
                      /* Name of report being queued to CM
                      */

                      cmipapp_ind_s_type ip_ind,
                      /* Union of all rpt types getting queued to CM
                      */

                      uint32           data_block
                      /* Token created and used by CM.
                      ** data_block is passed by CM, when calling app registered
                      ** callbacks. App needs to pass the most recent value
                      ** given by CM, all the time when cmipapp_rpt_ind () is called.
                      */

                      )
                      /*lint -esym(715,ip_rpt_name) */
                      /*lint -esym(715,ip_ind) */
                      /*lint -esym(715,data_block) */
{
#if defined(FEATURE_IP_CALL)
  cmipapp_rpt_s_type         *cmipapp_rpt_ptr = NULL;
  /* CM report command buffer */

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH_1("RPT name %d ", ip_rpt_name);

  /* Allocate rpt buffer
  */
  cmipapp_rpt_ptr          = cmipapp_rpt_get_buf_else_err_fatal ();

  /* Name of report being queued
  */
  cmipapp_rpt_ptr->hdr.cmd = ip_rpt_name;

  /* Report structure pertaining to ip_rpt_name
  */
  cmipapp_rpt_ptr->ip_ind  = ip_ind;

  /* Copy the data_block value from app
  */
  cmipapp_rpt_ptr->data_block = data_block;

  (void) q_link( cmipapp_rpt_ptr, &cmipapp_rpt_ptr->hdr.link ); /* init link */

  /* before queuing the IPAPP report, if there is reference counted object 
  ** add reference on it. After deliver the buf to upper layers delete the reference.
  */
  cmipapp_add_reference(cmipapp_rpt_ptr);

  q_put( cmtask_get_cm_rpt_q(), &cmipapp_rpt_ptr->hdr.link );        /* and queue it */
  (void) rex_set_sigs( get_cm_tcb(), CM_RPT_Q_SIG );      /* signal the CM task */
#endif

} /* cmipapp_rpt_ind() */
/*lint +esym(715,ip_rpt_name) */
/*lint +esym(715,ip_ind) */
/*lint +esym(715,data_block) */


/*===========================================================================

FUNCTION cmipapp_call_id_allocate

DESCRIPTION
Returns a new CM call id

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
CM_CALL_ID_INVALID - invalid call id
valid call id

SIDE EFFECTS
None
===========================================================================*/
cm_call_id_type cmipapp_call_id_allocate (

  void
  )
{
#if defined(FEATURE_IP_CALL)
  return cm_call_id_allocate ();
#else
  return CM_CALL_ID_INVALID;
#endif
}


/*===========================================================================

FUNCTION cmipapp_call_id_deallocate

DESCRIPTION
Deallocates a CM call id

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void cmipapp_call_id_deallocate (

                                 cm_call_id_type call_id
                                 /* Call id */
                                 )
                                 /*lint -esym(715,call_id) */
{
#if defined(FEATURE_IP_CALL)
  cm_call_id_deallocate (call_id);
#endif

  return;
}
/*lint +esym(715,call_id) */

/*===========================================================================

FUNCTION cmipapp_inform_srv_system_change

DESCRIPTION
Apps that have a registered function for being informed of service
change get notified.

DEPENDENCIES
CM must have already been initialized with
cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
/*lint -esym(714, cmipapp_inform_srv_system_change) */  
/*lint -esym(765, cmipapp_inform_srv_system_change) */  
void cmipapp_inform_srv_system_change (

                                       const cm_mm_msim_ss_info_s_type *ss_info_ptr
                                       /* Serving system information */
                                       )
{
  cmipapp_cmd_srv_sys_info_sub_f_type  *sys_info_funcptr = NULL;
  /* Stores app provided call back for reporting service change
  */
  cmipapp_cmd_common_s_type         cmd_common;

  int                               list_loop        = 0;
  /* Loops through client list
  */

  cmipapp_srv_status_s_type         srv_status;
  /* Service status that gets reported to apps
  */

  cmipapp_id_type                   app_id;

  CM_ASSERT (ss_info_ptr != NULL);
  /*----------------------------------------------------------------------*/

  if(!cmph_is_lte_capable_on_sub(ss_info_ptr->asubs_id))
  {
    return;
  }

  /* Initialize service status
  */
  srv_status.evdo_srv_status  = SYS_SRV_STATUS_NO_SRV;
  srv_status.umts_srv_status  = SYS_SRV_STATUS_NO_SRV;
  srv_status.lte_srv_status   = SYS_SRV_STATUS_NO_SRV;


  /* Copy the current HDR service information
  */
  if (ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_1 && cmss_is_hybr_status_on())
  {
    /* Phone is in hybrid mode, 1x + HDR
    */
    if(ss_info_ptr->stack_info[1].sys_mode == SYS_SYS_MODE_HDR)
    {
      srv_status.evdo_srv_status = ss_info_ptr->stack_info[1].srv_status;
      srv_status.lte_srv_status  = ss_info_ptr->stack_info[0].srv_status;
    }
    else if(ss_info_ptr->stack_info[1].sys_mode == SYS_SYS_MODE_LTE)
    {
      srv_status.lte_srv_status = ss_info_ptr->stack_info[1].srv_status;
    }
    else if(ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_LTE)
    {
      srv_status.evdo_srv_status = ss_info_ptr->stack_info[1].srv_status;
      srv_status.lte_srv_status = ss_info_ptr->stack_info[0].srv_status;
    }
  }
  else
  {
    /* Non hybrid mode
    */
    if (ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_HDR)
    {

      srv_status.evdo_srv_status = ss_info_ptr->stack_info[0].srv_status;

    }
    else if (ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_WCDMA)
    {
      srv_status.umts_srv_status = ss_info_ptr->stack_info[0].srv_status;
    }
    else if (ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_LTE)
    {
      srv_status.lte_srv_status = ss_info_ptr->stack_info[0].srv_status;
    }
    else
    { 
      CM_MSG_MED_2("System mode  = %d Srv status = %d",
        ss_info_ptr->stack_info[0].sys_mode,ss_info_ptr->stack_info[0].srv_status);
    } 

  }

  cmd_common.asubs_id = ss_info_ptr->asubs_id;

  /* Loop through app store to call registered serving system info
  ** call backs.
  */

  /* A Lock is required to avoid a situation where an app is trying to
  ** write to cmipapp_arr[index0] and CM is trying to read the value.
  ** A Possible garbage here can lead us to get an invalid function ptr
  ** resulting in a crash.
  ** Lock does not solve sequencing issues where App has deregistered
  ** and CM calls a callback.
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    /* Serialize data access cmipapp_arr[] */
    app_id = cmipapp_arr[list_loop].app_id;
    sys_info_funcptr = 
      cmipapp_arr[list_loop].app_cb.cmipapp_cmd_serv_sys_info_sub_ptr;
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if ( app_id != CMIPAPP_ID_INVALID && sys_info_funcptr != NULL)
    {
      /* Call the app supplied function for service indication
      */
      if (!sys_info_funcptr(&srv_status, cmd_common))
      {
        CM_ERR_1 ("CM->IMS: Informing app id %d for srv chg failed",
          cmipapp_arr[list_loop].app_id);

      }
      else
      {
        CM_MSG_HIGH_3("CM->IMS: sys_info,lte srv_status %d,app_id %d,as_id %d",
                      srv_status.lte_srv_status,app_id,cmd_common.asubs_id);
      }
    }
  }
} /* cmipapp_inform_srv_system_change () */

/*===========================================================================

FUNCTION cmipapp_send_oprt_mode

DESCRIPTION
Send Phone's operating mode to IP app

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
boolean cmipapp_send_oprt_mode
(
 sys_oprt_mode_e_type  oprt_mode,
 /* Operating mode
 */
 boolean wait_for_cleanup
 )
 /*lint -esym(714,cmipapp_send_oprt_mode) */
 /*lint -esym(765,cmipapp_send_oprt_mode) */
 /* Can't be static as it would result in compiler warnings
 */
{

  int list_loop = 0;
  boolean is_cb_success = FALSE;
  /* Loop through list of apps 
  */

  cmipapp_inform_oprt_mode_f_type *oprt_mode_funcptr = NULL;
  /* oprt mode cb func ptr
  */

  cmipapp_id_type        app_id;

  sys_oprt_mode_e_type   oprt_mode_sent;

  cmipapp_trans_id_type  trans_id;

  uint32                 user_data_block;

  boolean                do_callback = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  trans_id = 0;

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    do_callback = FALSE;
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    /* Serialize data access cmipapp_arr[] */
    oprt_mode_funcptr = cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode;
    app_id = cmipapp_arr[list_loop].app_id;
    oprt_mode_sent = cmipapp_arr[list_loop].oprt_mode_sent;
    user_data_block = cmipapp_arr[list_loop].user_data_block;

    if (  app_id != CMIPAPP_ID_INVALID
          && oprt_mode_funcptr != NULL
          && (oprt_mode_sent != oprt_mode ||
              oprt_mode == SYS_OPRT_MODE_LPM) )
    {      
      /* Trans id for sending oprt mode
      */
      trans_id = cmipapp_next_trans_id();
      cmipapp_arr[list_loop].active_trans_id = trans_id;

      /* Oprt mode value being sent
      */
      cmipapp_arr[list_loop].oprt_mode_sent  = oprt_mode;

      do_callback = TRUE;
    }
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if(do_callback)
    {
      /* Call the app supplied function for service indication
      */
      is_cb_success = oprt_mode_funcptr (oprt_mode, 
                                         trans_id,
                                         wait_for_cleanup,
                                         user_data_block);
      
      CM_MSG_HIGH_3("CLNUP: send oprt_mode is_cb_success %d, app_id %d wait_for_cleanup %d", \
                  is_cb_success, app_id, wait_for_cleanup );
    }
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("CLNUP: Oprt_mode do_callback %d, is_cb_success %d, app_id %d", \
                 do_callback, is_cb_success, app_id );

  return is_cb_success;
  
} /* cmipapp_send_oprt_mode () */
/*===========================================================================

FUNCTION cmipapp_send_srv_domain

DESCRIPTION
Send Phone's srv_domain to IP app

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
void cmipapp_send_srv_domain
(
 cm_srv_domain_pref_e_type  srv_domain,
 /* srv domain
 */
 sys_modem_as_id_e_type asubs_id
 )
 /*lint -esym(714,cmipapp_send_srv_domain) */
 /*lint -esym(765,cmipapp_send_srv_domain) */
 /* Can't be static as it would result in compiler warnings
 */
{

  int list_loop = 0;
  /* Loop through list of apps 
  */

  cmipapp_inform_srv_domain_sub_f_type *srv_domain_funcptr = NULL;
  /* oprt mode cb func ptr
  */
  cmipapp_cmd_common_s_type cmd_common;

  uint32                      user_data_block;

  cmipapp_id_type             app_id;

  uint32                      trans_id;

  cm_srv_domain_pref_e_type   current_srv_domain;

  boolean                     do_callback = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_common.asubs_id = asubs_id;

  trans_id = 0;
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    do_callback = FALSE;
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    /* Serialize data access cmipapp_arr[] */
    srv_domain_funcptr =
      cmipapp_arr[list_loop].app_cb.cmipapp_inform_srv_domain_sub;
    user_data_block = cmipapp_arr[list_loop].user_data_block;
    app_id = cmipapp_arr[list_loop].app_id;
    current_srv_domain = cmipapp_arr[list_loop].srv_domain_sent;

    if ( app_id != CMIPAPP_ID_INVALID && srv_domain_funcptr &&
      current_srv_domain != srv_domain )
    {
      /* Trans id for sending oprt mode
      */
      trans_id = cmipapp_next_trans_id();
      cmipapp_arr[list_loop].active_trans_id = trans_id;

      /* Oprt mode value being sent
      */
      cmipapp_arr[list_loop].srv_domain_sent  = srv_domain;

      do_callback = TRUE;
    }
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if(do_callback)
    {
      /* Call the app supplied function for service indication
      */
      if (!srv_domain_funcptr (srv_domain, 
        trans_id,
        user_data_block,
        cmd_common))
      {
        CM_ERR_1 ("Informing app id %d for srv_domain changed", app_id);

      }      
      else
      {
        CM_MSG_HIGH_2 ("callback successfull for app %d srv dom %d", 
          app_id,srv_domain );

      }

    }
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmipapp_send_srv_domain () */




/*===========================================================================

FUNCTION cmipapp_is_wait_on_srv_domain_cnf

DESCRIPTION
Checks if ipapp would need to deregister on this request and
also if inform_srv_domain callback is registered.

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
boolean cmipapp_is_wait_on_srv_domain_cnf (

  cm_srv_domain_pref_e_type   srv_domain
  /* srv domain preference 
  */
  )
  /*lint -esym(714,cmipapp_is_wait_on_srv_domain_cnf ) */
  /*lint -esym(765,cmipapp_is_wait_on_srv_domain_cnf ) */
  /* Can't be static as it would result in compiler warnings
  */
{

  int                         list_loop   = 0;
  /* Loop through list of apps 
  */

  boolean is_wait = FALSE;

  cmipapp_id_type app_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* IP app perform deregistration only for PS_DETACH srv domain pref
  */
  if ( srv_domain != CM_SRV_DOMAIN_PREF_PS_DETACH && srv_domain != CM_PS_DETACH_BY_MMODE_TRIGGER)
  {  
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if any app is registered to be informed of PS DETACH,
  ** wait for confirmation
  */

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    app_id = cmipapp_arr[list_loop].app_id;

    if (  ( app_id != CMIPAPP_ID_INVALID)
      &&
      (cmipapp_arr[list_loop].app_cb.cmipapp_inform_srv_domain_sub)
      &&
      (cmipapp_arr[list_loop].srv_domain_sent == srv_domain)
      &&
      (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
      )
    {
      is_wait = TRUE;
      break;
    }
  }     
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if(is_wait)
  {
    CM_MSG_HIGH_1 ("App %d has cb registered for srv_domain_pref",
      app_id);
  }

  return is_wait;
}


/*===========================================================================

FUNCTION cmipapp_is_srv_dom_ps_detach

DESCRIPTION
Checks if CM sent PS-Detach to ipapp 

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
boolean cmipapp_is_srv_dom_ps_detach()
{

  int                         list_loop   = 0;
  /* Loop through list of apps 
  */

  boolean ret = FALSE;

  cmipapp_id_type app_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if any app is registered to which we informed PS DETACH,
  */

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    app_id = cmipapp_arr[list_loop].app_id;

    if (   app_id != CMIPAPP_ID_INVALID
      &&
      cmipapp_arr[list_loop].app_cb.cmipapp_inform_srv_domain_sub
      &&
      (cmipapp_arr[list_loop].srv_domain_sent == CM_SRV_DOMAIN_PREF_PS_DETACH ||
       cmipapp_arr[list_loop].srv_domain_sent == CM_PS_DETACH_BY_MMODE_TRIGGER)
      )
    {
      ret = TRUE;
      CM_MSG_HIGH_2 ("App %d srv_domain_pref sent %d",app_id,cmipapp_arr[list_loop].srv_domain_sent);
      break;
    }
  }     
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  return ret;
}


/*===========================================================================

FUNCTION cmipapp_is_wait_on_oprt_mode_cnf

DESCRIPTION
Checks if ipapp would need to deregister on this request and
also if inform_oprt_mode callback is registered.

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
boolean cmipapp_is_wait_on_oprt_mode_cnf (

  sys_oprt_mode_e_type  oprt_mode
  /* Operating mode
  */
  )
  /*lint -esym(714,cmipapp_is_wait_on_oprt_mode_cnf) */
  /*lint -esym(765,cmipapp_is_wait_on_oprt_mode_cnf) */
  /* Can't be static as it would result in compiler warnings
  */
{

  int list_loop = 0;
  /* Loop through list of apps 
  */  

  boolean is_wait = FALSE;

  cmipapp_id_type app_id = CMIPAPP_ID_INVALID;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* IP app perform deregistration only 
  ** on LPM and PWR_OFF
  */
  if ((oprt_mode != SYS_OPRT_MODE_PWROFF)  &&
    (oprt_mode != SYS_OPRT_MODE_LPM)
#ifdef FEATURE_CM_LTE
    &&
    (oprt_mode != SYS_OPRT_MODE_OFFLINE)
    &&    
    (oprt_mode != SYS_OPRT_MODE_OFFLINE_CDMA)
#endif
    )
  {  
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For PWR_OFF and LPM wait till apps send 
  ** CM back a confirmation for the oprt_mode 
  ** request sent
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    app_id = cmipapp_arr[list_loop].app_id;

    if (  (app_id != CMIPAPP_ID_INVALID)
      &&
      (cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode)
      &&
      (cmipapp_arr[list_loop].oprt_mode_sent == oprt_mode)
      &&
      (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
      )
    {
      is_wait = TRUE;
      break;
    }
  }  
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  CM_MSG_HIGH_3("Is Wait %d for App Id %d, in oprt_mode %d", is_wait, app_id, oprt_mode);

  return is_wait;

} /* cmipapp_is_wait_on_oprt_mode_cnf () */


/*===========================================================================

FUNCTION cmipapp_oprt_mode_cnf

DESCRIPTION
Api called by app to confirm processing
of operating mode request.

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

void cmipapp_oprt_mode_cnf (

                            cmipapp_id_type                      app_id,
                            /* App id assigned by CM
                            */

                            cmipapp_trans_id_type                trans_id
                            /* Transaction id
                            */   
                            )
                            /*lint -esym(715,app_id) */
                            /*lint -esym(715,trans_id) */
{   
#if defined(FEATURE_IP_CALL)
  int                               list_loop        = 0;
  /* Loops through client list
  */

  cm_sd_rpt_u_type                 *rpt_cmd_ptr = NULL;
  /* Report being sent to CM
  */

  sys_oprt_mode_e_type          oprt_mode   = SYS_OPRT_MODE_NONE;
  /* oprt_mode that was last sent to app
  */

  cmipapp_id_type slow_app_id = CMIPAPP_ID_INVALID;
  /* App id for which CM has to wait */

  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)))     
  {
    CM_ERR_1 ("CLNUP: Oprt Incorrect app id %d", app_id);
    return;
  }

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH_1 ("IMS->CM: App %d confirming oprt mode", app_id);

  if (cmipapp_arr[app_id].active_trans_id != trans_id) 
  {    
    CM_ERR_3 ("CLNUP: Incorrect trans_id %d from app %d, expected %d",
                       trans_id, app_id, cmipapp_arr[app_id].active_trans_id);
    return;
  }

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /*----------------------------------------------------------------------*/

  /* Operating mode being confirmed by the apps
  */
  oprt_mode   = cmipapp_arr[app_id].oprt_mode_sent;

  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  /*----------------------------------------------------------------------*/

  /* 
  ** If there is an app registered with oprt_mode func cb, 
  ** return if it has not been informed of the 
  ** operating mode or if the transaction id is not NONE.
  **
  ** oprt_mode_sent is also checked to avoid ALLAPS_OPRT_CNF
  ** from being sent when just one app has been informed of
  ** oprt_mode and it immed returns back with conf (Depends
  ** on App's priority).
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
      &&
      (cmipapp_arr[list_loop].app_cb.cmipapp_inform_oprt_mode)        
      &&
      (
      (cmipapp_arr[list_loop].oprt_mode_sent != oprt_mode)
      ||
      (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
      )

      )
    {
      /* This app yet to confirm operating mode change, CM needs to wait */
      slow_app_id = cmipapp_arr[list_loop].app_id;

    }
  }  
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if(slow_app_id != CMIPAPP_ID_INVALID)
  {
    CM_MSG_MED_1("Wait for oprt_mode_cnf from IP APP ID %d", slow_app_id);
    return;
  }

  /*----------------------------------------------------------------------*/

  /* Send report when all ipapps
  ** have confirmed operating mode
  ** If all the apps have confirmed a particular operating
  ** mode then it is time to send CM_IP_APP_ALLAPPS_OPRT_CNF
  ** to CM.
  */ 
  rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal();

  /* Fill in command type */
  rpt_cmd_ptr->hdr.cmd                       = CM_IP_APP_ALLAPPS_OPRT_CNF;


  CM_MSG_HIGH_0 ("Sending CM_IP_APP_ALLAPPS_OPRT_CNF");

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_cmd_ptr);
#endif

} /* cmipapp_oprt_mode_cnf () */
/*lint +esym(715,app_id) */
/*lint +esym(715,trans_id) */


/*===========================================================================

FUNCTION cmipapp_srv_domain_cnf

DESCRIPTION
Api called by app to confirm processing
of srv domain request.

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

void cmipapp_srv_domain_cnf (

                             cmipapp_id_type                      app_id,
                             /* App id assigned by CM
                             */

                             cmipapp_trans_id_type                trans_id,
                             /* Transaction id
                             */

                             cmipapp_ind_common_s_type   ind_common
                               /**< Common indication data */
                             )
                             /*lint -esym(715,app_id) */
                             /*lint -esym(715,trans_id) */
{   
#if defined(FEATURE_IP_CALL)
  int                               list_loop        = 0;
  /* Loops through client list
  */

  cm_sd_rpt_u_type                 *rpt_cmd_ptr = NULL;
  /* Report being sent to CM
  */

  cm_srv_domain_pref_e_type        srv_domain   = CM_SRV_DOMAIN_PREF_NONE;


  cmipapp_id_type slow_app_id = CMIPAPP_ID_INVALID;
  /* waiting for srv_domain_cnf for this app_id */

  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)))     
  {
    CM_ERR_1 ("Incorrect app id %d", app_id);
    return;
  }

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH_1 ("IMS->CM: App %d confirming srv domain", app_id);

  if (cmipapp_arr[app_id].active_trans_id != trans_id) 
  {    
    CM_MSG_MED_1 ("Not expecting srv_domain_cnf from app %d", app_id);
    return;
  }

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /*----------------------------------------------------------------------*/
  /* Service domain being confirmed by the apps
  */
  srv_domain   = cmipapp_arr[app_id].srv_domain_sent;

  /* reset  trans id fields
  */
  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;
 

   
  /*----------------------------------------------------------------------*/

  /* 
  ** If there is an app registered with srv_domain func cb, 
  ** return if it has not been informed of the 
  ** srv domain change or if the transaction id is not NONE.
  **
  ** srv_domain_sent is also checked to avoid ALLAPS_OPRT_CNF
  ** from being sent when just one app has been informed of
  ** srv_domain and it immed returns back with conf (Depends
  ** on App's priority).
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
      &&
      (cmipapp_arr[list_loop].app_cb.cmipapp_inform_srv_domain_sub) 
      &&
      (
      (cmipapp_arr[list_loop].srv_domain_sent != srv_domain)
      ||
      (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
      )
      )
    {
      slow_app_id = cmipapp_arr[list_loop].app_id;
    }
  }  
  /* when cnf from all the apps have been received,
  loop to reset srv_domain_sent for all the apps
  so that later when PS_DETACH happens again, it should be sent to IMS APPs*/
  if(slow_app_id == CMIPAPP_ID_INVALID) 
  {
    for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
    {
      if(cmipapp_arr[list_loop].srv_domain_sent == CM_SRV_DOMAIN_PREF_PS_DETACH)
      {
        cmipapp_arr[list_loop].srv_domain_sent = CM_SRV_DOMAIN_PREF_NONE;
      }
    }
  }
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());


  if(slow_app_id != CMIPAPP_ID_INVALID)
  {
    CM_MSG_MED_1("Wait for srv_domain_cnf from IP APP ID %d", slow_app_id);
    return;
  }

  /*----------------------------------------------------------------------*/

  /* Send report when all ipapps
  ** have confirmed operating mode
  ** If all the apps have confirmed a particular operating
  ** mode then it is time to send CM_IP_APP_ALLAPPS_OPRT_CNF
  ** to CM.
  */ 
  rpt_cmd_ptr = cm_sd_rpt_get_buf_else_err_fatal();
  /* Fill in command type */
  
  rpt_cmd_ptr->hdr.cmd                       = CM_IP_APP_ALLAPPS_SRV_DOM_CNF;


  CM_MSG_HIGH_0 ("Sending CM_IP_APP_ALLAPPS_SRV_DOM_CNF");

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_cmd_ptr);
#endif

} /* cmipapp_srv_domain_cnf () */
/*lint +esym(715,app_id) */
/*lint +esym(715,trans_id) */

/*===========================================================================

FUNCTION cmipapp_send_srvcc_notification

DESCRIPTION
Send SRVCC notification to IP app

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
void cmipapp_send_srvcc_notification
(
   cmipapp_srvcc_notification_e_type  srvcc_status,
   /* srvcc status  */

   sys_modem_as_id_e_type             as_id  
)
 /*lint -esym(714,cmipapp_send_srvcc_notification) */
 /*lint -esym(765,cmipapp_send_srvcc_notification) */
 /* Can't be static as it would result in compiler warnings
 */
{

  int list_loop = 0;
  /* Loop through list of apps 
  */

  cmipapp_send_srvcc_sub_f_type *srvcc_funcptr = NULL;
  /* oprt mode cb func ptr
  */
  cmipapp_cmd_common_s_type cmd_common;

  uint32                     user_data_block;

  cmipapp_id_type            app_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_common.asubs_id = as_id;

  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    app_id = cmipapp_arr[list_loop].app_id;
    srvcc_funcptr = cmipapp_arr[list_loop].app_cb.cmipapp_srvcc_status_sub_func_ptr;
    user_data_block = cmipapp_arr[list_loop].user_data_block;
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if ( app_id != CMIPAPP_ID_INVALID && srvcc_funcptr != NULL)
    {
      /* Call the app supplied function for service indication
      */
      if (!srvcc_funcptr (srvcc_status, user_data_block, cmd_common))
      {
        CM_ERR_3 ("CM->IMS: Informing app id %d for srvcc status,srvcc_status %d,as_id %d", 
                  app_id, srvcc_status, cmd_common.asubs_id);
      }      
      else
      {
        CM_MSG_HIGH_3 ("CM->IMS: srvcc callback successfull for app %d,srvcc_status %d,as_id %d", 
                       app_id, srvcc_status, cmd_common.asubs_id);
      }
    }
  }  

} /* cmipapp_send_srv_domain () */


/*===========================================================================

FUNCTION cmipapp_send_drvcc_notification

DESCRIPTION
Send DRVCC notification to IP app

DEPENDENCIES
Call object must have been initialized with cmcall_init.

RETURN VALUE
None.

SIDE EFFECTS
Nothing.

===========================================================================*/
void cmipapp_send_drvcc_notification
(
 cmipapp_drvcc_notification_e_type  drvcc_status,
 /* drvcc status
 */
 boolean orm_send_status,
 sys_modem_as_id_e_type             as_id
 )
{

  int list_loop = 0;
  /* Loop through list of apps 
  */

  cmipapp_send_drvcc_sub_f_type *drvcc_funcptr = NULL;
  /* drvcc status notification cb func ptr
  */
  cmipapp_drvcc_status_params_u_type drvcc_status_params = {0};
  uint32                     user_data_block;
  cmipapp_id_type            app_id;
  cmipapp_cmd_common_s_type cmd_common;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_common.asubs_id = as_id;
  
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    app_id = cmipapp_arr[list_loop].app_id;
    drvcc_funcptr = cmipapp_arr[list_loop].app_cb.cmipapp_drvcc_status_sub_func_ptr;
    user_data_block = cmipapp_arr[list_loop].user_data_block;
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if(drvcc_status == CMIPAPP_DRVCC_HO_FAIL_NOTIFY)
    {
      drvcc_status_params.drvcc_ho_fail.onex_orm_status = orm_send_status;
      CM_MSG_MED_1("1xorm send status sent to IMS in DRVCC_HO_FAIL %d", orm_send_status);
    }
    if ( app_id != CMIPAPP_ID_INVALID && drvcc_funcptr != NULL)
    {
      /* Call the app supplied function for service indication
      */
      if (!drvcc_funcptr (drvcc_status, drvcc_status_params, user_data_block, cmd_common))
      {
        CM_ERR_2("Informing app id %d for drvcc status %d failed", app_id, drvcc_status);
      }      
      else
      {
        CM_MSG_HIGH_2("drvcc callback successfull for app %d, notification %d", app_id, drvcc_status);
      }
    }
  }  
} /* cmipapp_send_drvcc_notification () */


/*===========================================================================
FUNCTION cmipapp_get_app_ptr

DESCRIPTION
Return cmipapp pointers per app_id. If app_id is out of range return NULL

RETURN VALUE
TRUE - cmipapp_s_type *
FALSE - NULL
===========================================================================*/
static cmipapp_s_type *cmipapp_get_app_ptr(cmipapp_id_type     app_id)
{
  if ( !(app_id < ARR_SIZE(cmipapp_arr)))     
  {
    CM_MSG_MED_1 ("Incorrect app id %d", app_id);
    return NULL;
  }

  return &cmipapp_arr[app_id];
}


/*===========================================================================
FUNCTION cmipapp_send_orig_cmd

DESCRIPTION
Send call orig cmd to IP App.

RETURN VALUE
TRUE - IP App recvd call origination.
FALSE - call orig is not sent to IP App
===========================================================================*/
boolean cmipapp_send_orig_cmd(

                              cmipapp_id_type          app_id,

                              const cm_num_s_type      called_num,
                              /* Number dialed - phone that is being called */

                              const cm_call_id_type    call_id,
                              /* call ID of the call */

                              cmipapp_call_type_e_type call_type,
                              /* call type for the call */

                              cmipapp_sys_mode_e_type  sys_mode,
                              /* Sys mode call needs to be originated on */

                              cm_ip_call_attrib_info_s_type  *call_attrib_info,
                              /* Indicate audio/video attributes */

                              const cm_npi_type              pi,
                              /* Presentation indicator */

                              sys_modem_as_id_e_type   asubs_id
                                /**< Indicates the asubs_id */
                              )
{
  cmipapp_cmd_orig_f_type *fn_ptr = NULL;
  cmipapp_cmd_orig_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_orig_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_orig_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid send orig CB for app %d", app_id);
    return FALSE;
  }

  CM_MSG_HIGH_6 ("CM->IMS: CMIPAPP: Sending IP orig, sys_mode %d, app_id %d pi %d orig call_id %d, call_type %d, as_id %d",\
    sys_mode, app_id, pi, call_id, call_type, cmd_common.asubs_id);

  CM_MSG_HIGH_1 ( "CM->IMS: CMIPAPP: Secure_call %d", call_attrib_info->is_secure_call );
  
  /* Call the callback and return the result of the call */
  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(called_num, call_id, call_type, sys_mode,
      cmipapp_arr[app_id].user_data_block, call_attrib_info, pi, cmd_common);
  }
  else
  {
    return fn_ptr(called_num, call_id, call_type, sys_mode,
      cmipapp_arr[app_id].user_data_block, call_attrib_info, pi);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_answer_cmd

DESCRIPTION
Send answer response for incoming call.

RETURN VALUE
TRUE - IP App sent call response.
FALSE - Answer response was not sent.
===========================================================================*/
boolean cmipapp_send_answer_cmd(

                                cmipapp_id_type                  app_id,
                                /* App handling this call */

                                const cm_call_id_type            call_id,
                                /* call id of incoming call */

                                boolean                          accept,
                                /* Flag indicating whether the call is accepted or not */

                                cmipapp_incom_rej_cause_e_type   answer_cause,
                                /* Cause for rejecting a call */

                                cmipapp_call_type_e_type         call_type,
                                /* Indicate the call type when call is accepted by client */

                                cm_ip_call_attrib_info_s_type    call_attrib,
                                /* Current call attributes. */

                                const cm_npi_type                pi,
                                /* Presentation indicator */

                                cm_ip_call_sip_reject_code_s_type      sip_reject_code,
                                /* SIP code sent for rejecting incoming call */

                                sys_modem_as_id_e_type   asubs_id
                                  /**< Indicates the asubs_id */
                                )
{
  /* No need to validate app_id. Because app_id was validated before assigning
  ** to call_ptr */

  cmipapp_cmd_answer_f_type *fn_ptr = NULL;
  cmipapp_cmd_answer_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_answer_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_answer_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Answer callback is NULL for app %d", app_id);
    return FALSE;
  }

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Sending ANSWER, call_id %d, cause %d, is_accept %d",
    call_id, answer_cause, accept);

  CM_MSG_HIGH_4 ("CM->IMS: CMIPAPP: Sending ANSWER, pi %d, reject_code: present %d, code %d as_id %d", 
    pi, sip_reject_code.is_reject_code_valid,sip_reject_code.reject_code, cmd_common.asubs_id);



  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, accept, answer_cause,
      cmipapp_arr[app_id].user_data_block, call_type, call_attrib, pi, sip_reject_code, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, accept, answer_cause,
      cmipapp_arr[app_id].user_data_block, call_type, call_attrib, pi, sip_reject_code);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_invite_response

DESCRIPTION
Send response to invite received by IP App.

RETURN VALUE
TRUE - Was able to send response to IP App.
FALSE - Failed to send response to IP App.
===========================================================================*/
boolean cmipapp_send_invite_response(

                                     cmipapp_id_type                  app_id,
                                     /* App handling this call */

                                     cm_call_id_type                  call_id,
                                     /* call id of the call */

                                     boolean                          accept,
                                     /* Flag indicating whether the call is accepted or not */

                                     cmipapp_incom_rej_cause_e_type   answer_cause,
                                     /* If Invite is declined, specify the reason */

                                     sys_modem_as_id_e_type   asubs_id
                                       /**< Indicates the asubs_id */
                                     )
{ 
  cmipapp_cmd_invite_res_f_type *fn_ptr = NULL;
  cmipapp_cmd_invite_res_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_invite_res_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_invite_res_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invite response CB is null for app %d",app_id);
    return FALSE;
  }

  CM_MSG_MED_3("CM->IMS: CMIPAPP: Sending Invite response to app %d for call_id %d, as_id %d",
    app_id, call_id, cmd_common.asubs_id);
  
  /* Callback IP App provided func */
  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr( call_id, accept, answer_cause,
      cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    return fn_ptr( call_id, accept, answer_cause,
      cmipapp_arr[app_id].user_data_block);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_call_end_cmd

DESCRIPTION
Send cmd to end the call with given call id parameter.

RETURN VALUE
TRUE - Call end was sent.
FALSE - Call end was not sent to IP App.
===========================================================================*/
boolean cmipapp_send_call_end_cmd(

                                  cmipapp_id_type       app_id,

                                  const cm_call_id_type call_id,

                                  cm_call_end_e_type    end_cause,

                                  sys_modem_as_id_e_type   asubs_id
                                    /**< Indicates the asubs_id */
                                  )
{
  cmipapp_cmd_end_f_type *fn_ptr = NULL;
  cmipapp_cmd_end_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_end_ext_sub_f_type *sub_fn_ext_ptr = NULL;

  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_end_params_s_type end_params;

  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  /* Convert CM End Cause to CMIPAPP End Cause */
  end_params.end_cause = cmcall_end_cause_to_cmipcall_end_cause(end_cause);

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_end_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_end_sub_func_ptr;
  sub_fn_ext_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_end_ext_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL && sub_fn_ext_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP:Invalid callend CB for app %d", app_id);
    return FALSE;
  }

  if(sub_fn_ext_ptr)
  {
    CM_MSG_HIGH_4("CM->IMS: Sending IP END EXT, call_id  %d, app_id %d, as_id %d end_cause %d", 
      call_id, app_id,cmd_common.asubs_id, end_params.end_cause);
    
    return sub_fn_ext_ptr(call_id, cmipapp_arr[app_id].user_data_block, end_params, cmd_common);
  }
  else if(sub_fn_ptr)
  {
    CM_MSG_HIGH_3("CM->IMS: Sending IP END, call_id  %d, app_id %d, as_id %d", 
      call_id, app_id,cmd_common.asubs_id);
    return sub_fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    CM_MSG_HIGH_2("CM->IMS: Sending IP END, call_id  %d, app_id %d", 
      call_id, app_id);

    return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_start_inband_dtmf_cmd

DESCRIPTION
Send cmd to start DTMF.

RETURN VALUE
TRUE - If start DTMF was sent to App.
FALSE - Failed to send cmd to App.
===========================================================================*/
boolean cmipapp_send_start_inband_dtmf_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  uint8                 digit,
  /* DTMF digit */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
  )
{
  cmipapp_inband_cmd_start_cont_dtmf_f_type *fn_ptr = NULL;
  cmipapp_inband_cmd_start_cont_dtmf_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_inband_cmd_start_cont_dtmf_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_inband_cmd_start_cont_dtmf_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Inband DTMF CB is invalid for app %d", app_id);
    return FALSE;
  }

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send start inband DTMF for call %d,as_id %d", 
    call_id, cmd_common.asubs_id);

  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, digit, cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, digit, cmipapp_arr[app_id].user_data_block);
  }

}

/*===========================================================================
FUNCTION cmipapp_send_stop_inband_dtmf_cmd

DESCRIPTION
Send cmd to stop DTMF.

RETURN VALUE
TRUE - If stop DTMF was sent to App.
FALSE - Failed to send cmd to App.
===========================================================================*/
boolean cmipapp_send_stop_inband_dtmf_cmd(

  cmipapp_id_type       app_id,

  const cm_call_id_type call_id,

  uint16                duration,
  /* DTMF duration */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
  )
{ 
  cmipapp_inband_cmd_stop_cont_dtmf_f_type *fn_ptr = NULL;
  cmipapp_inband_cmd_stop_cont_dtmf_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_inband_cmd_stop_cont_dtmf_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_inband_cmd_stop_cont_dtmf_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Stop inband DTMF CB is invalid for app %d",
      app_id);
    return FALSE;
  }

  CM_MSG_MED_1 ("CMIPAPP: Send stop inband DTMF for call %d", call_id);

  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, duration, cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, duration, cmipapp_arr[app_id].user_data_block);
  }

}

/*===========================================================================
FUNCTION cmipapp_send_hold_cmd

DESCRIPTION
Sends hold cmd for a call to IP App.

RETURN VALUE
TRUE - Sent hold cmd to IP App.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_hold_cmd(

                              cmipapp_id_type       app_id,

                              const cm_call_id_type call_id,

                              sys_modem_as_id_e_type   asubs_id
                                /**< Indicates the asubs_id */

                              )
{
  cmipapp_cmd_hold_f_type *fn_ptr = NULL;
  cmipapp_cmd_hold_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_hold_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_hold_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Hold CB is invalid for app %d" ,app_id);
    return FALSE;
  }

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send Hold for call %d,as_id %d", 
    call_id, cmd_common.asubs_id);

  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_local_hold_cmd

DESCRIPTION
Sends local hold cmd for a call to IP App.

RETURN VALUE
TRUE - Sent local hold cmd to IP App.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_local_hold_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  const sys_modem_as_id_e_type  asubs_id

)
{
  cmipapp_cmd_local_hold_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_local_hold_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_3("CMIPAPP: Local Hold CB is invalid for app %d, call_id %d, asubs_id %d",
                     app_id, call_id, asubs_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_2 ("CM->IMS: CMIPAPP: Send Local Hold for call %d with asubs_id %d", 
                    call_id, asubs_id);

  return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_local_retrieve_cmd

DESCRIPTION
  Sends local retrieve cmd to previously local held call.

RETURN VALUE
  TRUE - Sent local retrieve cmd to IP App.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_local_retrieve_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  const sys_modem_as_id_e_type  asubs_id

)
{
  cmipapp_cmd_local_retrieve_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_local_retrieve_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_3("CMIPAPP: Local Retrieve CB ptr is invalid for app %d, call_id %d, asubs_id %d",
                     app_id, call_id, asubs_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_2("CM->IMS: CMIPAPP: Send Local Retrieve for call %d with asubs_id %d", 
                   call_id, asubs_id);

  return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_resume_cmd

DESCRIPTION
Sends resume cmd to previously held call.

RETURN VALUE
TRUE - Sent resume cmd to IP App.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_resume_cmd(

                                cmipapp_id_type       app_id,

                                const cm_call_id_type call_id,

                                sys_modem_as_id_e_type   asubs_id
                                  /**< Indicates the asubs_id */

                                )
{
  cmipapp_cmd_active_f_type *fn_ptr = NULL;
  cmipapp_cmd_active_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_active_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_active_sub_func_ptr;

  if(fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Resume CB ptr is invalid for app %d", app_id);
    return FALSE;
  }

  CM_MSG_HIGH_2 ("CM->IMS: CMIPAPP: Send resume for call_id %d, as_id %d", 
                    call_id, asubs_id);

  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block);
  }
}

/*===========================================================================
FUNCTION cmipapp_send_modify_call_cmd

DESCRIPTION
Sends cmd to modify call type.

RETURN VALUE
TRUE - Sent change call type cmd to IP App.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_modify_call_cmd(

                                     cmipapp_id_type                app_id,

                                     const cm_call_id_type          call_id,

                                     cmipapp_call_type_e_type       cap,
                                     /* Requested call type */

                                     cm_ip_call_attrib_info_s_type  call_attrib,
                                     /* Audio/Video call attribute */

                                     sys_modem_as_id_e_type   asubs_id
                                       /**< Indicates the asubs_id */

                                     )
{
  cmipapp_cmd_modify_f_type *fn_ptr = NULL;
  cmipapp_cmd_modify_sub_f_type *sub_fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_modify_func_ptr;
  sub_fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_modify_sub_func_ptr;

  if( fn_ptr == NULL && sub_fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Modify call CB is invalid for app %d", app_id);
    return FALSE;
  }

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Send cmd to modify call type call %d, em_type %d, as_id %d", 
    call_id, call_attrib.em_type, asubs_id);

  if(sub_fn_ptr != NULL)
  {
    return sub_fn_ptr(call_id, cap,
      cmipapp_arr[app_id].user_data_block, call_attrib, cmd_common);
  }
  else
  {
    return fn_ptr(call_id, cap,
      cmipapp_arr[app_id].user_data_block, call_attrib);
  }
}


/*===========================================================================
FUNCTION cmipapp_send_modify_response

DESCRIPTION
Send response to modify call type response.

RETURN VALUE
TRUE - Modify response is sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_modify_response(

                                     cmipapp_id_type               app_id,

                                     const cm_call_id_type         call_id,

                                     boolean                       call_modif_rsp,
                                     /* Call type to modify the call into */

                                     cmipapp_call_type_e_type      call_type_rsp,
                                     /* responded call type */

                                     cm_ip_call_attrib_info_s_type call_attrib,
                                     /* Audio/Video call attribute */

                                     sys_modem_as_id_e_type   asubs_id
                                       /**< Indicates the asubs_id */

                                     )
{ 
  cmipapp_cmd_modify_rsp_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_modify_rsp_sub_func_ptr;


  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid modify call response CB for app %d",
      app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_4 ("CM->IMS: CMIPAPP: Send modify resp call %d, as_id %d rsp %d, em_type %d", 
                 call_id, asubs_id, call_modif_rsp, call_attrib.em_type);
  


  return fn_ptr(call_id, call_modif_rsp, cmipapp_arr[app_id].user_data_block,
    call_type_rsp, call_attrib, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_activate_answer_hold_cmd

DESCRIPTION
Sends cmd to activate answer hold.

RETURN VALUE
TRUE - Answer hold was activate sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_activate_answer_hold_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

  )
{
  cmipapp_cmd_act_answer_hold_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_act_answer_hold_sub_func_ptr;


  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid activate answer hold CB for app %d",
      app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send activate answer hold for call %d, as_id %d", 
    call_id, asubs_id);

  return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_deactivate_answer_hold_cmd

DESCRIPTION
Sends cmd to deactivate answer hold.

RETURN VALUE
TRUE - Answer hold was deactivate sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_deactivate_answer_hold_cmd(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

  )
{
  cmipapp_cmd_deact_answer_hold_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_deact_answer_hold_sub_func_ptr;

  if( fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid answer hold CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send deactivate answer hold for call %d, as_id %d", 
    call_id, asubs_id);

  return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_answer_call_waiting_response

DESCRIPTION
Answer call which is in waiting.

RETURN VALUE
TRUE - Answer cmd is sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_answer_call_waiting_response(

  cmipapp_id_type               app_id,

  const cm_call_id_type         call_id,

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */

  )
{ 
  cmipapp_cmd_answer_call_waiting_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_answer_call_waiting_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid call waiting CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send answer waiting for call %d, as_id %d", 
    call_id, asubs_id);

  return fn_ptr(call_id, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_simple_flash_cmd

DESCRIPTION
Send flash event either to accept a pending call or remove last participant
from a conference.

RETURN VALUE
TRUE - flash request sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_simple_flash_cmd(

                                      cmipapp_id_type              app_id,

                                      const cm_call_id_type        call_id,

                                      const cm_mm_call_info_s_type *call_info_ptr,
                                      /* Call info on flash cmd is applied */

                                      sys_modem_as_id_e_type   asubs_id
                                        /**< Indicates the asubs_id */
                                      )
{
  cmipapp_cmd_simple_flash_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_simple_flash_sub_func_ptr;


  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid flash CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_2 ("CM->IMS: CMIPAPP: Send flash cmd for call %d, as_id %d",
    call_info_ptr->call_id, asubs_id);

  return fn_ptr(call_id, call_info_ptr, cmipapp_arr[app_id].user_data_block, cmd_common);
}


/*===========================================================================
FUNCTION cmipapp_send_ect_on_calls_cmd

DESCRIPTION
Applies ECT supplementary service on existing calls for the APP.
ECT - A & B have a call
A & C have a call
Now A wants connect C and B by using ECT service.

RETURN VALUE
TRUE - ECT request sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_ect_on_calls_cmd(

                                      cmipapp_id_type               app_id,

                                      sys_modem_as_id_e_type   asubs_id
                                        /**< Indicates the asubs_id */
                                      )
{
  cmipapp_cmd_ect_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_ect_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid ECT on Calls CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_2 ("CM->IMS: CMIPAPP: Send cmd ECT to app %d, as_id %d",
    app_id, asubs_id);

  return fn_ptr(cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_mpty_cmd

DESCRIPTION
When a phone has 2 calls, one active, othe hold. This API merges both into a
conference - MPTY ( MultiParTY).

RETURN VALUE
TRUE - MPTY cmd sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_mpty_cmd(

                              cmipapp_id_type               app_id,

                              sys_modem_as_id_e_type   asubs_id
                                /**< Indicates the asubs_id */
                              )
{
  cmipapp_cmd_mpty_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_mpty_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid MPTY CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_2 ("CM->IMS: CMIPAPP: Send cmd MPTY to app %d, as_id %d",
    app_id, asubs_id);

  return fn_ptr(cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_mo_sms_cmd

DESCRIPTION
Sends MO SMS to IP APP.

RETURN VALUE
TRUE - SMS request sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_mo_sms_cmd(

                                cmipapp_id_type     app_id,

                                cm_sms_msg_type     *data,
                                /* The pointer to MO SMS data */

                                sys_sys_mode_e_type sys_mode,
                                /* System mode over which SMS is sent */

                                sys_modem_as_id_e_type   asubs_id
                                  /**< Indicates the asubs_id */
                                )
{
  cmipapp_sms_cmd_mo_sms_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_sms_cmd_mo_sms_sub_func_ptr;

  if( fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid MO SMS CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Send MO SMS request at sys %d,app_id %d,as_id %d", 
                 sys_mode, app_id, asubs_id);

  return fn_ptr(data, sys_mode, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_abort_call_cmd

DESCRIPTION
Sends abort request to IP App for existing call.

RETURN VALUE
TRUE - Abort request sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_abort_call_cmd(

                                    cmipapp_id_type     app_id,

                                    const cm_call_id_type call_id,

                                    const cmipapp_abort_cause_e_type abort_cause,
                                    /* Reason for aborting a call */

                                    sys_modem_as_id_e_type   asubs_id
                                      /**< Indicates the asubs_id */
                                    )
{ 
  cmipapp_cmd_abort_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_abort_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid abort call CB for app %d", app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Send abort request for call %d abort_cause %d,as_id %d",
    call_id, abort_cause,asubs_id);

  return fn_ptr(call_id, abort_cause, cmipapp_arr[app_id].user_data_block,cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_remove_last_party_cmd

DESCRIPTION
Sends request to remove last party from conference.

RETURN VALUE
TRUE - Request was sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_remove_last_party_cmd(

  cmipapp_id_type     app_id,

  cm_num_s_type       call_num,
  /* Call number need to be removed from conference */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
  )
{ 
  cmipapp_send_rm_last_party_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipcall_rm_last_party_from_conf_sub;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid remove last part CB ptr for app %d",
      app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send remove last party from conf,app_id %d,as_id %d",
    app_id, asubs_id);

  return fn_ptr(call_num, cmipapp_arr[app_id].user_data_block, cmd_common);
}


/*===========================================================================
FUNCTION cmipapp_send_add_new_party_cmd

DESCRIPTION
  Sends request to add new party 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_add_new_party_cmd(

  cmipapp_id_type     app_id,

  cm_call_id_type      call_id,
    /* call that needs to add this new party */

  cm_num_s_type       party_num,
    /* party need to be added to  from normal/conference */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
)
{ 
  cmipapp_send_add_new_party_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipcall_add_party_to_conf_sub;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid add party CB ptr for app %d",
                                                              app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_2 ("CM->IMS: CMIPAPP: Send add party to normal/conf Sucess,app_id %d,as_id %d",
    app_id, asubs_id);

  return fn_ptr(call_id ,party_num, cmipapp_arr[app_id].user_data_block, cmd_common);
}


/*===========================================================================
FUNCTION cmipapp_send_call_deflection_cmd

DESCRIPTION
  Sends request to deflect incoming call to deflect_num 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_call_deflection_cmd(

  cmipapp_id_type     app_id,

  cm_call_id_type      call_id,
    /* call that needs to be deflected to deflect_num */

  cm_num_s_type       deflect_num,
    /* destination number on which call is to be deflected */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
)
{ 
  cmipapp_call_deflection_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipcall_call_deflection_cmd_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid call deflection CB ptr for app %d",
                                                              app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Send call deflection for call id %d,app_id %d,as_id %d", 
    call_id, app_id, asubs_id);

  return fn_ptr(call_id ,deflect_num, cmipapp_arr[app_id].user_data_block, cmd_common);
}



/*===========================================================================
FUNCTION cmipapp_send_sups_cmd

DESCRIPTION
Sends request of sups command .

RETURN VALUE
TRUE - Request was sent.
FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_sups_cmd(

                              cmipapp_id_type     app_id,

                              cmipsups_cmd_s_type    *sups_cmd,

                              sys_modem_as_id_e_type   asubs_id
                                /**< Indicates the asubs_id */
                              )
{ 
  cmipapp_send_sups_cmd_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }
  if(sups_cmd->cmd_type == CM_SUPS_CMD_PROCESS_USS ||
    sups_cmd->cmd_type == CM_SUPS_CMD_USS_NOTIFY_RES ||
    sups_cmd->cmd_type == CM_SUPS_CMD_USS_RES ||
    sups_cmd->cmd_type == CM_SUPS_CMD_RELEASE)
  {
    fn_ptr = cmipapp_ptr->app_cb.cmipapp_ussd_cmd_sub;
  }
  else
  {
    fn_ptr = cmipapp_ptr->app_cb.cmipapp_sups_cmd_sub;
  }

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid sups cmd ptr for app %d",app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_MED_3 ("CM->IMS: CMIPAPP: Send PS sups cmd, type=%d,app_id %d,as_id %d", 
    sups_cmd->cmd_type, app_id, asubs_id);

  return fn_ptr(sups_cmd, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_find_ics_support_app

DESCRIPTION
Check if ICS is supported and return the corresponding app_id. 

RETURN VALUE
cmipapp_id_type
===========================================================================*/

cmipapp_id_type cmipapp_find_ics_support_app( void )
{

  cmipapp_id_type   app_id = CMIPAPP_ID_INVALID;
  unsigned int      id_count = 0;

  /* Obtain a lock
  */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    /* CM only routes the service when the IP app is in the active state.
    */
    if((cmipapp_arr[id_count].app_id != CMIPAPP_ID_INVALID) 
      &&
      (cmipapp_arr[id_count].app_cb.cmipapp_sups_cmd_sub != NULL) 
      &&
      (cmipapp_arr[id_count].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE ) 
      )
    {
      app_id = cmipapp_arr[id_count].app_id;
      break;
    }

  }

  /* Free the lock
  */
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if(app_id != CMIPAPP_ID_INVALID)
  {
    CM_MSG_HIGH_1 (" Found ICS app %d", app_id);
  }    
  return app_id;

}

/*===========================================================================
FUNCTION cmipapp_find_ussd_support_app

DESCRIPTION
Check if USSD is supported over IP and return the corresponding app_id. 
As of now, CM doesn't prioritize one sys-mode over other while finding the USSD app

RETURN VALUE
cmipapp_id_type
===========================================================================*/

cmipapp_id_type cmipapp_find_ussd_support_app( void )
{

  cmipapp_id_type   app_id = CMIPAPP_ID_INVALID;
  unsigned int      id_count = 0;
  unsigned int      cap_index = 0;  
  boolean           is_app_found=FALSE;

  /* Obtain a lock */
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  for (id_count = 0; id_count < ARR_SIZE(cmipapp_arr); id_count++)
  {
    /* Need to proceed further only if app is valid and active and ussd supported */
    if((cmipapp_arr[id_count].app_id != CMIPAPP_ID_INVALID)
      && (cmipapp_arr[id_count].app_run_status == CMIPAPP_RUN_STATUS_ACTIVE )
      && (cmipapp_arr[id_count].app_cb.cmipapp_ussd_cmd_sub != NULL))
    {
      /* Now check for VOLTE / VOWLAN support on this app */
      for(cap_index=0;cap_index<CMIPAPP_CAP_ARR_MAX;cap_index++)
      {
        if(((cmipapp_arr[id_count].app_capability.sys_mode[cap_index] & SYS_BM_64BIT(SYS_SYS_MODE_LTE)) ||
           (cmipapp_arr[id_count].app_capability.sys_mode[cap_index] & SYS_BM_64BIT(SYS_SYS_MODE_WLAN)))
          && 
           (cmipapp_is_ussd_supported_call_type(cmipapp_arr[id_count].app_capability.call_type[cap_index])))
        {
          app_id = cmipapp_arr[id_count].app_id;

          is_app_found=TRUE;
          
          CM_MSG_HIGH_2 ("Found USSD app %d over sys_mode %d",app_id,cmipapp_arr[id_count].app_capability.sys_mode[cap_index]);
          break;
        }
      }
    }
    if(is_app_found==TRUE)
    {
      break;
    }
  }

  /* Free the lock */
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());
 
  return app_id;

}

/*===========================================================================

FUNCTION cmipapp_is_wait_on_subsc_cmd_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipapp_is_wait_on_subsc_cmd_cnf (

  cm_ph_cmd_e_type  subs_cmd
    /* subs cmd to wait 
    */
)

{

  int list_loop = 0;
    /* Loop through list of apps 
    */  
    
  boolean is_wait = FALSE;
  
  cmipapp_id_type app_id;

  sys_modem_as_id_e_type lte_sub_id = cmph_get_sub_with_lte_cap();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* IP app perform deregistration only on subs not available
  */
  if ( subs_cmd != CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE &&
       subs_cmd != CM_PH_CMD_DEACTIVATE_SUBS &&
	   subs_cmd != CM_PH_CMD_DUAL_STANDBY_PREF)
  {  
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmph_get_sub_config_ptr(lte_sub_id)->active_ims_stack == SYS_IMS_STACK_STATUS_ENABLED_THPY)
  {    
    CM_MSG_HIGH_0 ("EXT IMS enabled");
    is_wait = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  else
  {
  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */
  
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    app_id = cmipapp_arr[list_loop].app_id;

    if (  (app_id != CMIPAPP_ID_INVALID)
          &&
		  (cmipapp_arr[list_loop].app_cb.cmipapp_subsc_chgnd)
          &&
          (cmipapp_arr[list_loop].subs_cmd == subs_cmd)
          &&
          (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
       )
    {
        CM_MSG_HIGH_1 ("App %d has cb registered for subs_cmd", app_id);
      is_wait = TRUE;
      break;
    }
  }  
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return is_wait;
} /* cmipapp_is_wait_on_subsc_cmd_cnf () */


/*===========================================================================

FUNCTION cmipapp_subs_cmd_cnf

DESCRIPTION
  Api called by app to confirm processing
  of  subs unavailable cmd.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmipapp_subs_cmd_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id,
    /* Transaction id
    */   
	
  uint8                              subs_id_bitmask
    /* Bitmask of the subs_id impacted. Based on SYS_MODEM_AS_ID_MASK_*
    */
)
{   
  #if defined(FEATURE_IP_CALL)
  int                               list_loop        = 0;
    /* Loops through client list
    */

  cm_ph_cmd_e_type  subs_cmd   = CM_PH_CMD_NONE;
    /* subs cmd last sent to app
    */

  cmipapp_id_type slow_app_id = CMIPAPP_ID_INVALID;
    /* App id for which CM has to wait */
    
  cmipapp_ind_s_type          ipapp_ind;
    /* Report being sent to CM
    */
    
  /*----------------------------------------------------------------------*/

  CM_ASSERT (app_id < ARR_SIZE(cmipapp_arr));

  if ( !(app_id < ARR_SIZE(cmipapp_arr)))     
  {
    CM_ERR_1 ("CLNUP: Subs Incorrect app id %d", app_id);
    return;
  }

  /*----------------------------------------------------------------------*/

  CM_MSG_HIGH_3 ("IMS->CM: App %d confirming subs cmd,trans_id %d,subs_id_bitmask %d", 
    app_id,trans_id,subs_id_bitmask);

  if (cmipapp_arr[app_id].active_trans_id != trans_id) 
  {    
    CM_ERR_3 ("CLNUP: Incorrect trans_id %d from app %d, expected %d",
                       trans_id, app_id, cmipapp_arr[app_id].active_trans_id);
    return;
  }

  rex_enter_crit_sect(cmipapp_crit_sect_ptr());
  /* Serialize data access cmipapp_arr[] */

  /*----------------------------------------------------------------------*/

  /* Subs cmd  being confirmed by the apps
  */
  subs_cmd   = cmipapp_arr[app_id].subs_cmd;

  cmipapp_arr[app_id].active_trans_id = CMIPAPP_TRANS_ID_NONE;

  /*----------------------------------------------------------------------*/

  /* 
  ** Send  ALL APPS CNF only when all apps registered for subs  
  ** unavailable cmd has replied.
  */
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {

    if (  (cmipapp_arr[list_loop].app_id != CMIPAPP_ID_INVALID)
          &&
		  (cmipapp_arr[list_loop].app_cb.cmipapp_subsc_chgnd)        
          &&
         ((cmipapp_arr[list_loop].subs_cmd != subs_cmd)
            ||
            (cmipapp_arr[list_loop].active_trans_id != CMIPAPP_TRANS_ID_NONE)
          )
       )
    {
      /* This app yet to confirm , CM needs to wait */
      slow_app_id = cmipapp_arr[list_loop].app_id;
    }
  }  
  rex_leave_crit_sect(cmipapp_crit_sect_ptr());

  if(slow_app_id != CMIPAPP_ID_INVALID)
  {
    CM_MSG_MED_1("Wait for subs_cmd_cnf from IP APP ID %d", slow_app_id);
    return;
  }
  
  /*----------------------------------------------------------------------*/

  /* Send report when all ipapps
  ** have confirmed subs change 
  */ 
  
  memset(&ipapp_ind, 0, sizeof(cmipapp_ind_s_type));

  ipapp_ind.ind.subs_chng_cnf.subs_id_bitmask =subs_id_bitmask;

  cmipapp_rpt_ind (CM_IP_APP_ALLAPSS_SUBSC_CNF, ipapp_ind, NULL);

  CM_MSG_HIGH_0 ("CLNUP: Sending CM_IP_APP_ALLAPSS_SUBSC_CNF");
  
  #else
  #endif
  
} /* cmipapp_subs_cmd_cnf () */


/*===========================================================================

FUNCTION cmipapp_send_subsc_cmd

DESCRIPTION
  Send Phone's subs unavailable cmd  to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cmipapp_send_subsc_cmd
(
  cm_ph_cmd_e_type  subs_cmd,
    /* subs cmd  
    */
    uint8                   subs_id_bitmask,
      /* Bitmask of the subs_id impacted. Based on SYS_MODEM_AS_ID_MASK_*
      */
    uint8                     reason_mask[]

)
{
  
  int list_loop = 0;
  boolean is_cb_success = FALSE;
    /* Loop through list of apps 
    */

  cmipapp_send_subsc_chngd_f_type *subs_cmd_funcptr = NULL;
    /* subs cmd cb func ptr
    */

  cmipapp_id_type        app_id = CMIPAPP_ID_INVALID;

  cmipapp_trans_id_type  trans_id =0 ;

  uint32                 user_data_block;

  boolean                do_callback = FALSE;

  sys_modem_as_id_e_type lte_sub_id = cmph_get_sub_with_lte_cap();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmph_get_sub_config_ptr(lte_sub_id)->active_ims_stack == SYS_IMS_STACK_STATUS_ENABLED_THPY &&
     (subs_id_bitmask & BM(lte_sub_id)) )
  {    
    cm_subsc_chgnd_ind_msg_type subsc_chgnd_ind;

    subsc_chgnd_ind.subs_cmd = subs_cmd;    
    subsc_chgnd_ind.as_id = lte_sub_id;
    subsc_chgnd_ind.msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(lte_sub_id);

    if(E_SUCCESS == cm_msgr_send( MM_CM_SUBSC_CHGND_IND,
                         MSGR_MM_CM,
                         (msgr_hdr_s*)&subsc_chgnd_ind,
                                  sizeof(cm_subsc_chgnd_ind_msg_type) ))
    {
      is_cb_success = TRUE;
    }

    CM_MSG_HIGH_3("CM->MSGR: MM_CM_SUBSC_CHGND_IND subs_cmd=%d, asubs_id=%d, subs_id_bitmask %d",
                     subs_cmd, lte_sub_id, subs_id_bitmask);    
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  else
  {
  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {
    do_callback = FALSE;

    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    /* Serialize data access cmipapp_arr[] */

      subs_cmd_funcptr  = cmipapp_arr[list_loop].app_cb.cmipapp_subsc_chgnd;
    app_id = cmipapp_arr[list_loop].app_id;
    user_data_block = cmipapp_arr[list_loop].user_data_block;
	
      if (  app_id != CMIPAPP_ID_INVALID
          && subs_cmd_funcptr != NULL )
    {      
      /* Trans id for sending subs cmd 
      */
      trans_id = cmipapp_next_trans_id();
      cmipapp_arr[list_loop].active_trans_id = trans_id;
	  cmipapp_arr[list_loop].subs_cmd  = subs_cmd;

      do_callback = TRUE;
    }

    rex_leave_crit_sect(cmipapp_crit_sect_ptr());

    if(do_callback)
    {
      /* Call the app supplied function for service indication
      */
      is_cb_success = subs_cmd_funcptr (subs_cmd,
							  trans_id,
                               user_data_block,
                                        subs_id_bitmask,reason_mask);
      } /* do_callback */
    }   /* for (...)   */
  }     /* ! THPY_IMS  */

  CM_MSG_HIGH_3 ("CLNUP: subs_cmd do_callback %d, is_cb_success %d, app_id %d",\
                  do_callback, is_cb_success, app_id);

  return is_cb_success;
  
} /* cmipapp_send_subsc_cmd () */

/*===========================================================================
FUNCTION cmipapp_send_switch_speech_codec_cmd

DESCRIPTION
  Sends request to switch speech codec for call 

RETURN VALUE
  TRUE - Request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_switch_speech_codec_cmd(

  cmipapp_id_type     app_id,

  cm_call_id_type      call_id,
    /* call that needs to be deflected to deflect_num */

  sys_speech_codec_e_type       switch_to_codec,
    /* destination number on which call is to be deflected */

  sys_modem_as_id_e_type   asubs_id
    /**< Indicates the asubs_id */
)
{ 
	cmipapp_call_switch_codec_s_type switch_codec_params = {0};
  cmipapp_call_switch_codec_sub_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmipapp_ptr does not exist for app_id %d",app_id);
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipcall_call_switch_codec_cmd_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid switch speech codec CB ptr for app %d",
                                                              app_id);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_3("CM->IMS: CMIPAPP: Send switch speech codec for call id %d, switch_to_codec %d,as_id %d", 
    call_id, switch_to_codec,asubs_id);

  switch_codec_params.call_id = call_id;
  switch_codec_params.switch_to_codec = switch_to_codec;

  return fn_ptr(switch_codec_params, cmipapp_arr[app_id].user_data_block, cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_send_ip_explict_call_transfer_on_calls_cmd

DESCRIPTION
  IMS supports IP Explicit Call transfer of type - Blind, Assured and Consultative
  This function sends the ECT request to IMS

RETURN VALUE
  TRUE - IP ECT request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_ip_explict_call_transfer_on_calls_cmd
(
  cmipapp_id_type                         app_id,

  sys_modem_as_id_e_type                  asubs_id,
  /**< Indicates the asubs_id */

  cm_call_id_type                         transferee_call_id,
  /**< call id of call between Transferor (UE B) and Transferee (UE A) */

  cmipapp_ip_expicit_call_transfer_e_type ect_type,
  /**< ECT Type */

  cm_num_s_type                           transfer_target_call_num,
  /**< Calling Number of Transfer target (UE C) */

  cm_call_id_type                         transfer_target_call_id
  /**< Call ID of Transfer Target (UE C) */

)
{
  cmipapp_cmd_ip_explicit_call_transfer_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_ip_ect_sub_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid IP ECT on Calls CB for app %d", app_id);
    return FALSE;
  }

  if (ect_type <= CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_NONE || 
      ect_type >= CMIPAPP_EXPLICIT_CALL_TRANSFER_TYPE_MAX)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid ECT type %d", ect_type);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_3 ("CM->IMS: CMIPAPP: Send IP ECT to app %d, as_id %d, ect_type %d",
    app_id, asubs_id, ect_type);
  CM_MSG_HIGH_1("CM->IMS: CMIPAPP: target_call_id %d", transfer_target_call_id);

  return fn_ptr(
    transferee_call_id,
    ect_type,
    transfer_target_call_num,
    transfer_target_call_id,
    cmipapp_arr[app_id].user_data_block,
    cmd_common);
}

/*===========================================================================
FUNCTION cmipapp_is_active_vops

DESCRIPTION
  Finds if there are active app registered for WLAN mode.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  retun TRUE if IMS registered for Voice/VT or SMS over WiFi

SIDE EFFECTS
  None

===========================================================================*/
boolean cmipapp_is_active_wlan_apps()
{
  return ((cmipapp_find_active_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VOICE) != CMIPAPP_ID_INVALID) ||
          (cmipapp_find_active_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VT) != CMIPAPP_ID_INVALID) ||
          (cmipapp_find_active_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_SMS) != CMIPAPP_ID_INVALID));
}


/*===========================================================================
FUNCTION cmipapp_send_secure_call_capability

DESCRIPTION
  This Function sends Secure call capability/ TF card status to all 
  registered IPAPP

RETURN VALUE
  TRUE - Secure call capability request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_secure_call_capability
(

 sys_modem_as_id_e_type                  asubs_id,
 /**< Indicates the asubs_id */

 cmipapp_secure_call_capability_e_type  secure_call_capability
 /**<  Secure Call capability enum  */
 )
{
  cmipapp_cmd_inform_secure_call_capability_f_type  *func_ptr = NULL;
  /* Secure call capability func ptr */

  cmipapp_id_type           app_id;
  uint32                    user_data_block;
  cmipapp_cmd_common_s_type cmd_common;
  int                       list_loop = 0;
  boolean                   is_cb_success = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (secure_call_capability <= CMIPAPP_SECURE_CALL_CAPABILITY_NONE || 
      secure_call_capability >= CMIPAPP_SECURE_CALL_CAPABILITY_MAX)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid secure call capability %d", secure_call_capability);
    return FALSE;
  }



  for (list_loop = 0; list_loop < CMIPAPP_POOL_SIZE; ++list_loop)
  {  
    rex_enter_crit_sect(cmipapp_crit_sect_ptr());
    /* Serialize data access cmipapp_arr[] */
    func_ptr = cmipapp_arr[list_loop].app_cb.cmipapp_cmd_inform_secure_call_capability_func_ptr;
    app_id = cmipapp_arr[list_loop].app_id;
    user_data_block = cmipapp_arr[list_loop].user_data_block;
    cmd_common.asubs_id = asubs_id;

    if (app_id   != CMIPAPP_ID_INVALID && 
        func_ptr != NULL)
    {
      is_cb_success = func_ptr (secure_call_capability,
                                user_data_block,
                                cmd_common);

      CM_MSG_HIGH_3 ("CM->IMS: CMIPAPP: Send Secure Call capability to app %d, as_id %d, capability %d",
                     app_id, asubs_id, secure_call_capability);
    }
    rex_leave_crit_sect(cmipapp_crit_sect_ptr());
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("Secure Call capability, is_cb_success %d, app_id %d",
                is_cb_success, app_id );

  return is_cb_success;
}

/*===========================================================================
FUNCTION cmipapp_send_secure_context_info

DESCRIPTION
  This Function sends Secure Context Info for ongoing VoIP secure call 
  registered IPAPP

RETURN VALUE
  TRUE - Secure call capability request was sent.
  FALSE - Failed to send to IP App.
===========================================================================*/
boolean cmipapp_send_secure_context_info
(
  cmipapp_id_type                         app_id,

  sys_modem_as_id_e_type                  asubs_id,
  /**< Indicates the asubs_id */

  cm_call_id_type                         call_id,
  /**<  Secure Context info enum  */

  cmipapp_secure_context_e_type           secure_context_info
  /**<  Secure Context info enum  */
)
{
  cmipapp_cmd_inform_secure_context_f_type *fn_ptr = NULL;
  cmipapp_cmd_common_s_type cmd_common;
  cmipapp_s_type *cmipapp_ptr = cmipapp_get_app_ptr(app_id);

  if(cmipapp_ptr == NULL)
  {
    return FALSE;
  }

  fn_ptr = cmipapp_ptr->app_cb.cmipapp_cmd_inform_secure_context_func_ptr;

  if(fn_ptr == NULL)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid Secure Context Info CB for app %d", app_id);
    return FALSE;
  }

  if (secure_context_info <= CMIPAPP_SECURE_CONTEXT_NONE || 
      secure_context_info >= CMIPAPP_SECURE_CONTEXT_MAX)
  {
    CM_MSG_HIGH_1("CMIPAPP: Invalid secure call capability %d", secure_context_info);
    return FALSE;
  }

  cmd_common.asubs_id = asubs_id;

  CM_MSG_HIGH_3 ("CM->IMS: CMIPAPP: Send Secure Call capability, as_id %d, call_id %d, context_info %d",
                  asubs_id, call_id, secure_context_info);

  return fn_ptr(
                call_id,
                secure_context_info,
                cmipapp_arr[app_id].user_data_block,
                cmd_common);
}

/*===========================================================================

FUNCTION cmipapp_notify_ue_based_cw

DESCRIPTION
Api called by app to notify if ue based call waiting is enabled

DEPENDENCIES
cmipapp_init

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/

void cmipapp_notify_ue_based_cw (

                            cmipapp_id_type                      app_id,
                            sys_modem_as_id_e_type               asubs_id,
                            boolean                              use_ue_cw
                            )

{   

  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(asubs_id);

  if(use_ue_cw != volte_sub_info_ptr->ue_based_cw.use_ue_based_cw)
  {
  
    volte_sub_info_ptr->ue_based_cw.use_ue_based_cw = use_ue_cw;
    cmefs_fs_write(CMEFS_UE_BASED_CW,
                     (byte*)&(volte_sub_info_ptr->ue_based_cw),
                     sizeof(cm_ue_based_cw_s_type),
                     asubs_id);
  }

  return;


}
