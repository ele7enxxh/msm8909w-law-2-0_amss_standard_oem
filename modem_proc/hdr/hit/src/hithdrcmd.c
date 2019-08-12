/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        HIT TEST APPLICATION UTILITY FUNCTIONS

GENERAL DESCRIPTION
  This file contains the command invokation in the HIT Test
  App for HDR MCAL

EXTERNALIZED FUNCTIONS
  hithdrcmd_handler

REGIONAL FUNCTIONS
    None

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None
    
    Copyright (c) 2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hithdrcmd.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
08/23/2007   vh      Added critical section around command handling
08/16/2007   rmg     Replaced pilot_info with aset_info for RevB
03/28/2007   vh      Created

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_HIT
#ifdef FEATURE_HDR_HIT

#include "comdef.h"
#include "task.h"
#include "hitcmd.h"
#include "hitapp.h"
#include "hdrhitcmd.h"


/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

enum hithdrcmd_enum
{
  HITHDRCMD_ACQUIRE,
  HITHDRCMD_SYNC,
  HITHDRCMD_CC_CONFIG,
  HITHDRCMD_IDLE,
  HITHDRCMD_ASET_INFO,
  HITHDRCMD_DEMOD_FTC,
  HITHDRCMD_EQUALIZER_CONFIG,
  HITHDRCMD_RL_CONFIG,
  HITHDRCMD_RTCMAC_CONFIG,
  HITHDRCMD_MOD_RL,
  HITHDRCMD_FLMAC_CONFIG,
  HITHDRCMD_AMAC_CONFIG,
  HITHDRCMD_AMAC,
  HITHDRCMD_ROTATOR_CONFIG,
  HITHDRCMD_DEACTIVATE_RTCMAC,
};

/*==========================================================================

                     GLOBAL VARIABLES FOR MODULE

==========================================================================*/
rex_crit_sect_type hithdrcmd_crit_sect;
  /* critical section for global command buffer handling */
static hitcmd_buf_type *hithdrcmd_acquire_buf_ptr = NULL;
  /* store the current acquire command buffer */
static hitcmd_buf_type *hithdrcmd_mod_rl_buf_ptr = NULL;
  /* store the current mod_rl command buffer */


/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*===========================================================================

FUNCTION hithdrcmd_acquire_cb

DESCRIPTION
  callback function used with the acquire command

DEPENDENCIES
  None

PARAMETERS
  callback_id
  callback_result

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hithdrcmd_acquire_cb
(
  hdrhitcmd_acquire_status_type callback_result
    /* Callback result indicating acquisition result, system lost reason 
     * or hdr system usability depending on which callback is called
     */
)
{
  boolean final_rsp = (callback_result==HDRHITCMD_ACQUIRE_CB_ACQUIRED) ?
    FALSE:TRUE;

  rex_enter_crit_sect( &hithdrcmd_crit_sect );

  hitcmd_send_status( hithdrcmd_acquire_buf_ptr, 
                      (hitcmd_ret_type)(callback_result), 
                      final_rsp );

  if (final_rsp) 
  {
    hithdrcmd_acquire_buf_ptr = NULL;
  }

  rex_leave_crit_sect( &hithdrcmd_crit_sect );
}

/*===========================================================================

FUNCTION hithdrcmd_mod_rl_cb

DESCRIPTION
  callback function used with the mod_rl command

DEPENDENCIES
  None

PARAMETERS
  void

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void hithdrcmd_mod_rl_cb(void)
{
  rex_enter_crit_sect( &hithdrcmd_crit_sect );

  hitcmd_send_status( hithdrcmd_mod_rl_buf_ptr, HDRHITCMD_COMMAND_SUCCESS, 
                      TRUE );

  hithdrcmd_mod_rl_buf_ptr = NULL;

  rex_leave_crit_sect( &hithdrcmd_crit_sect );
}

/*===========================================================================

FUNCTION hithdrcmd_handler

DESCRIPTION
  Invokes HDR-HIT command corresponding to cmd_id in the cmd_buffer

DEPENDENCIES
  None

PARAMETERS
  cmd_ptr

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_ret_type hithdrcmd_handler
( 
  hitcmd_buf_type *cmd_buf_ptr
    /* pointer cmd packet */
)
{
  hitcmd_ret_type status = 0;
  boolean last_resp = TRUE;
  boolean send_rsp = TRUE;

  void *param_ptr = hitcmd_param_ptr(cmd_buf_ptr);

  if ( param_ptr == NULL ) 
  {
      return HITCMD_ERROR_CMD_ID;
  } /* end if */

  rex_enter_crit_sect( &hithdrcmd_crit_sect );

  switch( cmd_buf_ptr->cmd_id )
  {
  case HITHDRCMD_ACQUIRE:
    if(hithdrcmd_acquire_buf_ptr)
    {
      hitcmd_send_status( hithdrcmd_acquire_buf_ptr, HITCMD_REPLACED, TRUE );
      hithdrcmd_acquire_buf_ptr = NULL;
    }

    status = hdrhitcmd_acquire( (hdrhitcmd_acquire_param_type *)param_ptr, 
                                &hithdrcmd_acquire_cb );

    if (status == HDRHITCMD_COMMAND_ISSUED)
    {
      last_resp = FALSE;
      hithdrcmd_acquire_buf_ptr = cmd_buf_ptr;
    }

    break;

  case HITHDRCMD_SYNC:
    status = hdrhitcmd_sync( (hdrhitcmd_sync_param_type *)param_ptr );
    break;

  case HITHDRCMD_CC_CONFIG:
    status = hdrhitcmd_cc_config( (hdrhitcmd_cc_config_param_type *)
                                  param_ptr );
    break;

  case HITHDRCMD_IDLE:
    status = hdrhitcmd_idle( (hdrhitcmd_idle_param_type *)param_ptr );
    break;

  case HITHDRCMD_ASET_INFO:
    status = hdrhitcmd_aset_info( (hdrhitcmd_aset_info_param_type *)
                                  param_ptr );
    break;

  case HITHDRCMD_DEMOD_FTC:
    status = hdrhitcmd_demod_ftc( (hdrhitcmd_demod_ftc_param_type *)
                                  param_ptr );
    break;

  case HITHDRCMD_EQUALIZER_CONFIG:
    status = hdrhitcmd_equalizer_config( 
             (hdrhitcmd_equalizer_config_param_type *)param_ptr );
    break;

  case HITHDRCMD_RL_CONFIG:
    status = hdrhitcmd_rl_config( (hdrhitcmd_rl_config_param_type *)
                                  param_ptr );
    break;

  case HITHDRCMD_RTCMAC_CONFIG:
    status = hdrhitcmd_rtcmac_config( 
             (hdrhitcmd_rtcmac_config_param_type *)param_ptr );
    break;
  case HITHDRCMD_MOD_RL:
    if(hithdrcmd_mod_rl_buf_ptr)
    {
      hitcmd_send_status( hithdrcmd_mod_rl_buf_ptr, HITCMD_REPLACED, TRUE );
      hithdrcmd_mod_rl_buf_ptr = NULL;
    }

    status = hdrhitcmd_mod_rl( (hdrhitcmd_mod_rl_param_type *)param_ptr, 
                               &hithdrcmd_mod_rl_cb );

    if (status == HDRHITCMD_COMMAND_ISSUED)
    {
      last_resp = FALSE;
      hithdrcmd_mod_rl_buf_ptr = cmd_buf_ptr;
    }

    break;

  case HITHDRCMD_FLMAC_CONFIG:
    status = hdrhitcmd_flmac_config( (hdrhitcmd_flmac_config_param_type *)
                                     param_ptr );
    break;

  case HITHDRCMD_AMAC_CONFIG:
    status = hdrhitcmd_amac_config( (hdrhitcmd_amac_config_param_type *)
                                    param_ptr );
    break;

  case HITHDRCMD_AMAC:
    status = hdrhitcmd_amac( (hdrhitcmd_amac_param_type *)param_ptr );
    break;

  case HITHDRCMD_ROTATOR_CONFIG:
    status = hdrhitcmd_rotator_config( (hdrhitcmd_rotator_config_param_type *)
                                       param_ptr );
    break;

  case HITHDRCMD_DEACTIVATE_RTCMAC:
    status = hdrhitcmd_deactivate_rtcmac( 
             (hdrhitcmd_deactivate_rtcmac_param_type *)param_ptr );
    break;

  default:
    status = HITCMD_ERROR_CMD_ID;
    break;
  }

  rex_leave_crit_sect( &hithdrcmd_crit_sect );

  if (send_rsp)
  {
    hitcmd_send_status( cmd_buf_ptr, status, last_resp );
  }

  return status;
} /* hithdrcmd_handler */

#endif /* FEATURE_HDR_HIT */
#endif /* FEATURE_HIT */
