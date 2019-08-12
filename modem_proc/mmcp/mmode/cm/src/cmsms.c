/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R  S M S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager SMS Object.

EXTERNALIZED FUNCTIONS

  cmsms_ptr - Return a pointer to the one and only SMS object.

  cmsms_init - Initializing the SMS object.

  cmsms_client_cmd_proc -Process clients' SMS commands.

  cmsms_get_sms_data_buffer - Return the pointer to MO SMS buffer

  cmxsms_sms_data_buffer_fill - Fill the MO SMS buffer with specified data.

  cmsms_event - Notified registered clients the SMS event.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1999 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/12   vm      Added pref channel for MO SMS
02/25/12   jqi     Free MO SMS data buffer correctly.
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/16/11   xs      Mainline KDDI high psist requirement
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/23/10   xs      Superlint critical/high error fixed
01/19/10   aj      Added VOIP support for emergency call
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
12/17/08   jd      Fixed Lint errors
12/14/07   sk/ak   Added DCM
08/07/07   ic      Lint clean up
06/22/07   sk      DDTM rewrite.
03/07/07   jqi     Initial release.



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmtaski.h"   /* For alloc/free */
#include "cmsds.h"
#include "cmregprxi.h"
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
#include "cmxsms.h"    /* Interface to cmsms.c*/
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */
#include "cmsms.h"    /* Interface to cmsms.c*/
#include "cmclnup.h"
#ifdef FEATURE_HDR_HYBRID
#include "cmxcall.h"
#endif

#ifdef FEATURE_SMS_OVER_IP
#include "cmipsms.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmefs.h"

/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmsms_ptr

DESCRIPTION
  Return a pointer to the one and only SMS object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmsms_s_type  *cmsms_ptr( void )
{

  static cmsms_s_type cmsms;
    /* The one and only SMS object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return &cmsms;
}


/*===========================================================================

FUNCTION cmsms_init

DESCRIPTION
  Initializing the SMS object.

  This function must be called before the SMS object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_init( void )
{

  cmsms_s_type    *sms_ptr  =  cmsms_ptr();
      /* Point at sms object to be initialized */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );

  /* Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( ! CM_INIT_CHECK(sms_ptr->init_mask) );
  CM_INIT_MARK( sms_ptr->init_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CDSMS
  sms_ptr->sms_state                  = CM_SMS_STATE_IDLE;
  sms_ptr->sms_client_ptr             = CM_CLIENT_PTR_NONE;
  sms_ptr->l2_ack_requested           = FALSE;
  sms_ptr->awi_ack.seq_no             = 0;
  sms_ptr->awi_ack.bf                 = 0;
  sms_ptr->awi_ack.error_class        = 0;
  sms_ptr->awi_ack.status             = 0;
  #endif
  sms_ptr->data                       = NULL;
  sms_ptr->mo_status                  = CM_SMS_MO_NONE;
  sms_ptr->mt_sms_ptr                 = NULL;
}


/*===========================================================================

FUNCTION cmsms_sms_data_buffer_allocate()

DESCRIPTION
 Returns a pointer to the dynamic allocated buffer hold MO SMS msgs.

DEPENDENCIES 
 None 

RETURN VALUE
  Pointer to cm_sms_msg_type.

SIDE EFFECTS
  Nothing.

===========================================================================*/
cm_sms_msg_type *cmsms_sms_data_buffer_allocate()
{

  /* Allocate memory for user data for SMS
  */
  cm_sms_msg_type* wms_data = cm_mem_malloc(sizeof(cm_sms_msg_type));

  return wms_data;
}  /* cmsms_sms_data_buffer_allocate */


/*===========================================================================

FUNCTION cmsms_sms_data_buffer_free

DESCRIPTION
 Free the dynamic allocated buffer hold MO SMS msgs.
 1) when command is rejected at the command check routine. 
 2) After Sending the SMS over IP
 3) After sending MO SMS status message for SMS sending over CDMA.

DEPENDENCIES 
 NONE 

RETURN VALUE
  Pointer to cm_sms_msg_type.

SIDE EFFECTS
  Nothing.

===========================================================================*/
static void cmsms_sms_data_buffer_free(cm_sms_msg_type *sms_ptr)
{

  if( sms_ptr !=  NULL )
  {
    cm_mem_free( sms_ptr );
  }

}  /* cmsms_sms_data_buffer_free */


/*===========================================================================

FUNCTION cmsms_sms_data_buffer_fill

DESCRIPTION
 Fill in the static global location for outgoing SMS msgs.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmsms_sms_data_buffer_fill
(
  cm_sms_msg_type* dest_sms_msg,

  const cm_sms_msg_type*    sms_msg
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sms_msg           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dest_sms_msg->address.digit_mode = sms_msg->address.digit_mode;
  dest_sms_msg->address.number_of_digits = memscpy( dest_sms_msg->address.digits,
                                                    sizeof(dest_sms_msg->address.digits),
                                                    sms_msg->address.digits,
                                                    sms_msg->address.number_of_digits );

  dest_sms_msg->address.number_mode = sms_msg->address.number_mode;
  dest_sms_msg->address.number_plan = sms_msg->address.number_plan;
  dest_sms_msg->address.number_type = sms_msg->address.number_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dest_sms_msg->ota.data_len = memscpy( dest_sms_msg->ota.data,
                                        sizeof(dest_sms_msg->ota.data),
                                        sms_msg->ota.data,
                                        sms_msg->ota.data_len );

  dest_sms_msg->ota.format = sms_msg->ota.format;
  dest_sms_msg->chn_pref = sms_msg->chn_pref;
  CM_MSG_HIGH_1("MO SMS preferred channel = %d",dest_sms_msg->chn_pref);
  
}  /* cmsms_sms_data_buffer_fill */


/*===========================================================================

FUNCTION  cmsms_client_cmd_check

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
static cm_sms_cmd_err_e_type  cmsms_client_cmd_check(

    const cmsms_s_type     *sms_ptr,
        /* Pointer to a sms object */

    cm_sms_cmd_s_type          *sms_cmd_ptr
        /* Pointer to a CM command */
)
/*lint -esym(715,sms_ptr) */
/*lint -esym(818,cmd_ptr) */
{
  cm_sms_cmd_info_s_type  *cmd_info_ptr = NULL;
      /* Point at command information component */

  cmph_s_type               *ph_ptr     = cmph_ptr();
      /* Pointer for phone object */

  cmss_s_type               *ss_ptr     = cmss_ptr();
      /* Pointer for ss object */

  cm_sms_cmd_err_e_type   cmd_err       = CM_SMS_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  #if defined FEATURE_DOMAIN_SELECTION
  cmsds_s_type              *sds_ptr        = cmsds_ptr();
      /*  Domain selection object*/
  #endif
  
  sys_modem_as_id_e_type asubs_id = SYS_MODEM_AS_ID_1;

  cmph_sub_config_s_type *ph_sub_config_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr->cmd_type == CM_CMD_TYPE_SMS );
  CM_ASSERT( ph_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (cmph_is_msim())
  {
    asubs_id = sms_cmd_ptr->cmd_subs;
  }

  cmd_info_ptr = CMD_INFO_PTR( sms_cmd_ptr );
  CM_MSG_HIGH_2( "smscmd: cmsms_cmd_check, cmd=%d, cmd_subs=%d", 
                  sms_cmd_ptr->cmd, asubs_id);

  /* check cmd is already in error or not */

  if (cmd_info_ptr->cmd_err != CM_SMS_CMD_ERR_NOERR)
  {
    return cmd_info_ptr->cmd_err;
  }

  ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return CM_SMS_CMD_ERR_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */
  switch( sms_cmd_ptr->cmd )
  {
                         /*-------------------------*/
                         /* Mobile Originated SMS   */
                         /*-------------------------*/

    case CM_SMS_CMD_MO_MSG:

      /* The mode pref over which the SMS is sent.
      */
      switch( cmd_info_ptr->mode_pref )
      {
        case CM_MODE_PREF_CDMA_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_CDMA;

        break;

        case CM_MODE_PREF_HDR_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_HDR;

        break;


        case CM_MODE_PREF_WCDMA_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_WCDMA;

          break;

        case CM_MODE_PREF_LTE_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_LTE;

        break;

        default:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_NONE;

          CM_MSG_HIGH_1(" MO SMS mode pref 0x%x ", cmd_info_ptr->mode_pref);

          cmd_err = CM_SMS_CMD_ERR_MODE_PREF;

          return cmd_err;
      }
      CM_MSG_MED_2("Allow SMS in ECBM %d for sub %d",
                   ph_sub_config_ptr->is_allow_sms_in_ecbm_enabled, asubs_id);
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* this check is added to avoid forwarding an SMS to 1xcp when there are in inactive state due to an 
      ** acquisition which is ongoing on SD side as a part of BSR */
     #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && defined(FEATURE_CM_LTE)
      if((cmd_info_ptr->sys_mode == SYS_SYS_MODE_CDMA ) && 
         (cmregprx_is_gwl_protocol_active(SD_SS_MAIN)== TRUE))
      {
        cmd_err = CM_SMS_CMD_ERR_MODE_PREF;
        CM_MSG_HIGH_1(" MO SMS sys mode 0x%x and GWL is active on MAIN", cmd_info_ptr->sys_mode);
        return cmd_err;
      }
      #endif

      if ((cmd_info_ptr->sys_mode != SYS_SYS_MODE_LTE) &&
          (ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE))
      {
        cmd_err = CM_SMS_CMD_ERR_MODE_PREF;
        CM_MSG_HIGH_1(" Conflicting mode_pref received while in LTE %d ", cmd_info_ptr->mode_pref);
        return cmd_err;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_SMS_OVER_IP

      cmd_err =  cmipsms_client_cmd_check( cmd_info_ptr );

      if( cmd_err != CM_SMS_CMD_ERR_NOERR )
      {
        return cmd_err;
      }

      #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE ||
               cmclnup_is_ongoing_subsc(BM(sms_cmd_ptr->cmd_subs)))
      {
        cmd_err = CM_SMS_CMD_ERR_OFFLINE_S;
        CM_ERR_0( "CM_SMS_CMD_ERR_OFFLINE_S");
      }
      /* If NV allow_sms_in_ecbm is enabled, SMS is allowed
          ** during emergency callback mode else follow legacy behaviour
          ** Currently 1x is supported only on MAIN stack on sub 1. 
          ** Dont have support for 1x on other subs when its enabled, 
          ** NV should be read from corresponding stacks 
          */
      else if( (ph_sub_config_ptr->is_allow_sms_in_ecbm_enabled == FALSE) &&
                cmph_is_in_emergency_cb() &&
               !cmcall_emergency_call_active() )
      {
        /* No SMS is allowed when phone is in emergency callback mode.
        ** If phone is in emergency state and emergency call is still active
        ** allow SMS to go thorough.
        */
        cmd_err = CM_SMS_CMD_ERR_EMERGENCY_MODE_S;
      }
      else if( ph_ptr->cdma_lock_mode == CM_CDMA_LOCK_MODE_ON )
      {
        /* CDMA lock is On */
        cmd_err = CM_SMS_CMD_ERR_CDMA_LOCK_S;
        CM_ERR_0( "CM_SMS_CMD_ERR_CDMA_LOCK_S");
      }
      #ifdef FEATURE_CDSMS
      else if (sms_ptr->sms_state != CM_SMS_STATE_IDLE)
      {
        /* SMS state is not idle */
        cmd_err = CM_SMS_CMD_ERR_NOT_IDLE_S;
        CM_ERR_0( "CM_SMS_CMD_ERR_NOT_IDLE_S");
      }
      #endif
      /* Check if a DO call is in origination or connected, if it is then
      ** if the ddtm mask is set to block MO DBM return error
      */
      /*lint -e655 */
      /* 655: bit-wise operation uses (compatible) enum's */
      else if( cm_is_jcdma_enable() && ( cmcall_count_on_ss(CM_SS_HDR) != 0 ) && 
                ( ( ph_ptr->cur_ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM ) ||
                  ( ( ph_ptr->ddtm_pref == CM_DDTM_PREF_ON )&&
                    ( ph_ptr->ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM )))
             )
      {
        cmd_err = CM_SMS_CMD_ERR_DDTM;
      }
      /*lint +e655 */
      else
      {
        if( ( !ss_ptr->info.hdr_hybrid ||
              ss_ptr->info.hdr_srv_status != SYS_SRV_STATUS_SRV ) &&
            (ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV))
        {
          cmd_err = CM_SMS_CMD_ERR_NO_SRV_S;
        }
        else if (ss_ptr->info.sys_mode == SYS_SYS_MODE_AMPS)
        {
          cmd_err = CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S;
          CM_ERR_0( "CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S");
        }
        #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_DOMAIN_SELECTION)
        else if ((ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE) &&
                 ((cmsds_get_csfb_type_allowed(asubs_id) != CM_CALL_CSFB_TYPE_1XCSFB) ||
                 (cmsds_get_config_ptr(asubs_id)->is_sms_over_s102_supported == FALSE))
                )
        {
          cmd_err = CM_SMS_CMD_ERR_NO_SRV_S;
          CM_ERR_0( "CM_SMS_CMD_ERR_NO_SRV_S: NO S102 tunnel");
        }
        #endif
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SMS_CMD_AWI_ACK:

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_SMS_CMD_ERR_OFFLINE_S;
        CM_ERR_0( "CM_SMS_CMD_ERR_OFFLINE_S");
      }
      else
      {
        if( ss_ptr->info.sys_mode != SYS_SYS_MODE_AMPS )
        {
          cmd_err = CM_SMS_CMD_ERR_NOT_ANALOG_S;
          CM_ERR_0( "CM_SMS_CMD_ERR_NOT_ANALOG_S");
        }
      }
      break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /*-------------------------*/
                      /* Enable broadcast SMS    */
                      /*-------------------------*/

      case CM_SMS_CMD_BC_ENABLE:

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_ERR_0( "CM_SMS_CMD_ERR_CLIENT_ID_P");
      }
      break;

    default:
      break;
  }

  CM_MSG_HIGH_1("cmsms cmd_err: %d", cmd_err);

  return cmd_err;
}


/*===========================================================================

FUNCTION cmsms_client_cmd_err

DESCRIPTION
  Notify clients of a specified sms command error.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_client_cmd_err(

    const cm_sms_cmd_s_type       *sms_cmd_ptr,
        /* Pointer to a CM command */

    cm_sms_cmd_err_e_type      cmd_err
        /* Indicate the call command error */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr->cmd_type == CM_CMD_TYPE_SMS );
  CM_ASSERT( BETWEEN( cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_3("START cmsms_client_cmd_err() sms_cmd=%d, cmd_err=%d, client=%p",
                 sms_cmd_ptr->cmd, cmd_err, sms_cmd_ptr->client_ptr );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( sms_cmd_ptr->cmd_cb_func != NULL )
  {
    sms_cmd_ptr->cmd_cb_func( sms_cmd_ptr->data_block_ptr,
                              sms_cmd_ptr->cmd,
                              cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_SMS_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "sms cmd err, cmd=%d, err=%d, client=%ld",
            sms_cmd_ptr->cmd, cmd_err, sms_cmd_ptr->client_id );

    cmclient_list_sms_cmd_err_ntfy( cmd_err, sms_cmd_ptr );
  }

}



/*===========================================================================

FUNCTION cmsms_client_cmd_forward_ll

DESCRIPTION
  Forward a client SMS commands to the protocal stack.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from the protocal statck before
  continuing processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsms_client_cmd_forward_ll(

    const cmsms_s_type  *sms_ptr,
        /* Pointer to a sms object */

    const cm_sms_cmd_s_type  *sms_cmd_ptr
        /* Pointer to a CM command */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sms_ptr == NULL || sms_cmd_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

  return (cmxsms_client_cmd_forward_mc( sms_ptr, sms_cmd_ptr ));

  #else

  return FALSE;

  #endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

}

/*===========================================================================

FUNCTION cmsms_info_alloc

DESCRIPTION
  Allocate a sms info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sms info struct.

SIDE EFFECTS
  none

===========================================================================*/
static cm_sms_info_s_type  *cmsms_info_alloc( void )
{
  static cm_sms_info_s_type  sms_info;
      /* Currently there is only a need for one sms info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simple return a pointer to the statically allocated
  ** sms info buffer.
  */
  return &sms_info;
}


/*===========================================================================

FUNCTION cmsms_info_get

DESCRIPTION
  Copy the current sms state information into a specified buffer.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if sms information is valid, FALSE otherwise.

  Note that sms information only becomes valid after cmsms_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsms_info_get(

    cm_sms_info_s_type    *sms_info_ptr
        /* Copy sms state info into this buffer */
)
{

  cmsms_s_type          *sms_ptr  =  cmsms_ptr();
      /* Point at sms object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_info_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(sms_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy sms parameters into sms_info struct.
  */
  #ifdef FEATURE_CDSMS
  sms_info_ptr->sms_state             = sms_ptr->sms_state;
  sms_info_ptr->sms_client_id         = cmclient_get_client_id (sms_ptr->sms_client_ptr );
  sms_info_ptr->awi_ack.bf            = sms_ptr->awi_ack.bf;
  sms_info_ptr->awi_ack.error_class   = sms_ptr->awi_ack.error_class;
  sms_info_ptr->awi_ack.seq_no        = sms_ptr->awi_ack.seq_no;
  sms_info_ptr->awi_ack.status        = sms_ptr->awi_ack.status;
  sms_info_ptr->l2_ack_requested      = sms_ptr->l2_ack_requested;
  #endif
  sms_info_ptr->mo_status             = sms_ptr->mo_status;

  if( sms_ptr->mt_sms_ptr != NULL )
  {
    sms_info_ptr->mt_sms_format     = (uint8)sms_ptr->mt_sms_ptr->sms.format;
    sms_info_ptr->length            = sms_ptr->mt_sms_ptr->sms.data_len;
    sms_info_ptr->data              = sms_ptr->mt_sms_ptr->sms.data;
    sms_info_ptr->sys_mode          = sms_ptr->mt_sms_ptr->sys_mode;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return boolean indicating whether sms information is valid.
  */
  return CM_BOOLEAN( CM_INIT_CHECK(sms_ptr->init_mask) );
}

/*===========================================================================

FUNCTION cmsms_event

DESCRIPTION
  Notify the client list of a specified sms event.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_event(

    const cmsms_s_type     *sms_ptr,
        /* pointer to a sms object.  */

    cm_sms_event_e_type    sms_event
        /* notify client list of this sms event */
)
/*lint -esym(715,sms_ptr)*/
{
  cm_sms_info_s_type *sms_info_ptr = cmsms_info_alloc();
      /* Allocate a sms info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Free Memory for MO SMS */

  if( sms_event ==  CM_SMS_EVENT_MO_STATUS )
  {
    cmsms_sms_data_buffer_free(sms_ptr->data);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy sms information into
  ** allocated sms info buffer.
  */
  (void) cmsms_info_get( sms_info_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify clients list of sms event.
  */
  cmclient_list_sms_event_ntfy( sms_event, sms_info_ptr );

}
/*lint +esym(715,sms_ptr)*/


/*===========================================================================

FUNCTION cmsms_client_cmd_ll_reply

DESCRIPTION
  Process MC reply to clients sms commands.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_client_cmd_ll_reply(

    cmsms_s_type              *sms_ptr,
        /* Pointer to a sms object */

    cm_sms_cmd_s_type             *sms_cmd_ptr,
        /* Pointer to a CM command */

    cm_sms_cmd_err_e_type     cmd_err
        /* Indicate sms command error */
)
/*lint -esym(715,sms_ptr) */
/*lint -esym(818,sms_ptr) */
/*lint -esym(715,cmd_err) */
/*lint -esym(818,cmd_err) */
{
  cm_sms_cmd_info_s_type     *cmd_info_ptr = NULL;
    /* sms command information pointer */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX));
  CM_ASSERT( sms_cmd_ptr->cmd_type == CM_CMD_TYPE_SMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CMD_INFO_PTR(sms_cmd_ptr);
  CM_MSG_LOW_2("START cmsms_client_cmd_ll_reply, cmd=%d, err=%d",
                 sms_cmd_ptr->cmd, cmd_err);

  /*
  ** 1. Update sms fields
  ** 2. Notify clients of sms event
  */

    switch( sms_cmd_ptr->cmd )
    {
                       /*-------------------------*/
                       /*           MO SMS        */
                       /*-------------------------*/

      case CM_SMS_CMD_MO_MSG:

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        switch( cmd_info_ptr->sys_mode)
        {
          case SYS_SYS_MODE_LTE:
          case SYS_SYS_MODE_CDMA:

            #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

            /* Send MO SMS over CDMA protocol
            */

            /* update SMS state and copy relevant info to SMS object
            */

            sms_ptr->sms_client_ptr     = sms_cmd_ptr->client_ptr;
            sms_ptr->sms_state          = CM_SMS_STATE_HOLD_ORIG_RETRY;
            sms_ptr->data               = cmd_info_ptr->data;
            sms_ptr->l2_ack_requested   = cmd_info_ptr->l2_ack_requested;
            sms_ptr->is_ok_to_send      = TRUE;


            cmxsms_mo_msg_start( sms_ptr );

            #else

            CM_ERR_1( "MO SMS invalid sys_mode=%d",
                                            cmd_info_ptr->sys_mode);
            #endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SYS_SYS_MODE_HDR:

            #ifdef FEATURE_SMS_OVER_IP
            /* Send MO SMS over IP APP
            */

            cmipsms_send_mo_sms_request( cmd_info_ptr->app_id,
                                         cmd_info_ptr->data,
                                         cmd_info_ptr->sys_mode,
                                         SYS_MODEM_AS_ID_1);

            /* Free the memory for MO SMS
            */
            cmsms_sms_data_buffer_free(cmd_info_ptr->data);

            /* Notify client list of MO SMS event.
            */
            cmsms_event( sms_ptr, CM_SMS_EVENT_MO_MSG );

            break;

            #endif
 
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SYS_SYS_MODE_NONE:
          case SYS_SYS_MODE_NO_SRV:
          case SYS_SYS_MODE_AMPS:
          case SYS_SYS_MODE_GSM:
          case SYS_SYS_MODE_WCDMA:
          case SYS_SYS_MODE_GPS:
          case SYS_SYS_MODE_GW:
          case SYS_SYS_MODE_MAX:
          default:
            break;
        }

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                       /* Answer an incoming sms */
                       /*-------------------------*/
       #ifdef FEATURE_CDSMS

       case CM_SMS_CMD_AWI_ACK:

        /* update SMS state and copy relevant info to SMS object */
        sms_ptr->sms_client_ptr         = sms_cmd_ptr->client_ptr;
        sms_ptr->awi_ack.bf             = cmd_info_ptr->awi_ack.bf;
        sms_ptr->awi_ack.error_class    = cmd_info_ptr->awi_ack.error_class;
        sms_ptr->awi_ack.status         = cmd_info_ptr->awi_ack.status;
        sms_ptr->awi_ack.seq_no         = cmd_info_ptr->awi_ack.seq_no;

        cmsms_event( sms_ptr, CM_SMS_EVENT_AWI_ACK );

        break;


       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                       /* Enable broadcast SMS    */
                       /*-------------------------*/

       case CM_SMS_CMD_BC_ENABLE:

        /* update SMS state and copy relevant info to SMS object */
        sms_ptr->sms_client_ptr         = sms_cmd_ptr->client_ptr;
        sms_ptr->bc_enable              = cmd_info_ptr->bc_enable;

        cmsms_event( sms_ptr, CM_SMS_EVENT_BC_ENABLED );

        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #endif /* FEATURE_CDSMS */

      default:

        CM_ERR_1( "Bad sms command %d", (uint8) sms_cmd_ptr->cmd);


    } /* switch */

}
/*lint +esym(715,cmd_err) */
/*lint +esym(715,sms_ptr) */
/*lint +esym(818,cmd_err) */
/*lint +esym(818,sms_ptr) */



/*===========================================================================

FUNCTION cmsms_client_cmd_proc

DESCRIPTION
  Process clients' SMS commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_client_cmd_proc(

    cm_sms_cmd_s_type  *sms_cmd_ptr
        /* pointer to a CM command */
)
{

  cmsms_s_type             *sms_ptr       = cmsms_ptr();
      /* Point at phone object */

  cm_sms_cmd_err_e_type     sms_cmd_err;

  boolean wait_for_reply;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_ASSERT( sms_cmd_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( sms_cmd_ptr->cmd_type == CM_CMD_TYPE_SMS );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the phone.
  */
  sms_cmd_err = cmsms_client_cmd_check( sms_ptr, sms_cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmsms_client_cmd_err( sms_cmd_ptr, sms_cmd_err );

  if( sms_cmd_err != CM_SMS_CMD_ERR_NOERR )
  {

   /* Free the memory for MO SMS
   */
   {

      cm_sms_cmd_info_s_type  *cmd_info_ptr = NULL;
         /* Point at command information component */

      cmd_info_ptr = CMD_INFO_PTR( sms_cmd_ptr );

      if( sms_cmd_ptr->cmd == CM_SMS_CMD_MO_MSG )
      {
        cmsms_sms_data_buffer_free(cmd_info_ptr->data);
      }
   }

   return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to protocal stacks.
  */
  wait_for_reply = cmsms_client_cmd_forward_ll( sms_ptr, sms_cmd_ptr );

  /* If NO need to wait for replay from MC,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmsms_client_cmd_ll_reply( sms_ptr,
                               sms_cmd_ptr,
                               CM_SMS_CMD_ERR_NOERR );
  }

}


/*===========================================================================
FUNCTION cmsms_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsms_dem_is_apps_wakeup_from_suspend_event(

    cm_sms_event_e_type   sms_event
        /* Call event */
)
/*lint -esym(715, sms_event)*/
{
  #if !(defined (FEATURE_APPS_POWER_COLLAPSE) && \
       (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)))
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else
  switch ( sms_event )
  {
    case CM_SMS_EVENT_MT_MSG:
    case CM_SMS_EVENT_MSG_WAITING:

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
} /* cmsms_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, sms_event)*/

