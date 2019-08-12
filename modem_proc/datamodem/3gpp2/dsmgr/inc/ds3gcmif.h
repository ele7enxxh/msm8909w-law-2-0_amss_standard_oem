#ifndef DS3GCMIF_H
#define DS3GCMIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

           C A L L   M G R   I N T E R F A C E   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains functions for interfacing to Call Manager, including
  Data Services client initialization and processing of call events. This
  header file is intended to be used internal to the 3G Dsmgr module only.


  Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gcmif.h_v   1.1   28 Feb 2003 18:53:34   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gcmif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/23/15   sd      Added call_id as a parameter in the new call end command 
                   callback from CM.
05/24/13   sd      TSTS changes for DS3G.
12/21/12   ssb     Supporting Centralized call control Feature
03/09/12   msh     Removed FEATURE_DATA_NIKEL 
01/02/12   msh     Coan: Feature cleanup
01/13/12   sa      Added support for CSVT for CSFB targets.
06/13/11   dvk     Merged accessor function to get CM client ID.
11/22/10   ttv     Added support for DSDS.
08/22/08   ua      Added support for N/w initated Qos.
09/22/09   vrk     Merged LTE changes.
03/09/09   sa      AU level CMI modifications.
02/15/09   ls      Removed DS3G_GET_CM_CALL_TYPE()
12/12/08   sn      Fixed off-target lint errors.
10/28/05   as/vrk  Added prototype for ds3gi_process_cm_context_modify_*
                   commands
08/26/05   ar      Added support for CS Data incall modification & fallback.
06/30/05   ar      Added supprot for asynchronous call setup confirmation.
05/20/05   ar      Added support for DS_CM_CALL_PROGRESS_CMD event.
09/17/04   rc      Added prototype for ds3gi_process_cm_ss_ps_data_fail_cmd()
                   to support power save mode operation.
09/10/04   sb      S-PDP changes
10/07/03   vsk     Removed prototype for ds3gi_process_cm_ret_val()
04/09/03   rsl     Added ds3gi_process_cm_ret_val().
02/26/03   rc      Changed ds3gi_process_cm_ss_sys_mode_chg_cmd() to
                   ds3gi_process_cm_ss_srv_chg_cmd().
05/21/02   smp     Added prototype for ds3gi_process_cm_ss_sys_mode_chg_cmd().
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Changed DS3G_CALL_TYPE_CKT and DS3G_CALL_TYPE PKT to 
                   DS_CALL_TYPE_CKT and DS_CALL_TYPE_PKT.
01/11/02   smp     Merged from packet data branch. Added macros to convert
                   from CM call type to DS call type and vice versa.
07/03/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "cm.h"
#include "dstask_v.h"
#include "ds3gmgrint.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define DS3G_GET_DS_CALL_TYPE( cm_call_type ) \
( (cm_call_type == CM_CALL_TYPE_PS_DATA) ? DS_CALL_TYPE_PKT : DS_CALL_TYPE_CKT )

#ifdef FEATURE_3GPP_CSFB
#define DS3G_CALL_IS_ETSI_MODE( call_mode ) \
( (SYS_SYS_MODE_GSM == call_mode) || (SYS_SYS_MODE_WCDMA == call_mode)\
  || (SYS_SYS_MODE_LTE == call_mode) || (SYS_SYS_MODE_TDS == call_mode)  )
#else 
#define DS3G_CALL_IS_ETSI_MODE( call_mode ) \
( (SYS_SYS_MODE_GSM == call_mode) || \
  (SYS_SYS_MODE_WCDMA == call_mode) || \
  (SYS_SYS_MODE_TDS == call_mode) )
#endif /* FEATURE_3GPP_CSFB */


/*-------------------------------------------------------------------------
  Struct for SS IRAT Context transfer command
-------------------------------------------------------------------------*/
typedef struct
{
  sys_sys_mode_e_type     irat_ctxt_target_rat;
  sys_modem_as_id_e_type  irat_asubs_id;
} ds3g_ss_irat_cntxt_tx_type;

/*---------------------------------------------------------------------------
  Variable to hold the subs id mask from CM in case of SUB_NOT_AVAIL.
  This vaiable will always keep track of the latest value from CM. As per CM,
  they will pass only incrememted bit masks (i.e. 01 followed by 11) and
  expect only the most recent one to be returned. Overwritting the previous
  value is ok.
---------------------------------------------------------------------------*/
extern uint8 ds3gi_sub_not_avail_subs_id_mask;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_CM_INIT

DESCRIPTION   This function initializes the Data Services Call Manager
              client, and registers for call events of interest, namely:
                CM_CALL_EVENT_END
                CM_CALL_EVENT_INCOM
                CM_CALL_EVENT_CONNECT

              The following are only applicable to GSM/WCDMA:
                CM_CALL_EVENT_ORIG
                CM_CALL EVENT_SETUP_IND
                CM_CALL_EVENT_PROGRESS_INFO_IND
                CM_CALL_EVENT_CALL_CONF
                CM_CALL_EVENT_RAB_REL_IND
                CM_CALL_EVENT_RAB_ESTAB_IND
                CM_CALL_EVENT_RAB_REESTAB_REJ
                CM_CALL_EVENT_PDP_MODIFY_IND,
                CM_CALL_EVENT_PDP_MODIFY_CONF,
                CM_CALL_EVENT_PDP_MODIFY_REJ,
                CM_CALL_EVENT_MODIFY_IND
                CM_CALL_EVENT_MODIFY_RES
                CM_CALL_EVENT_MODIFY_COMPLETE_CONF

              It then calls the individual mode handlers to allow them to
              register for mode-specific events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_init(void);

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              of the completion status of the call command previously issued
              by 3G Dsmgr.

              This callback function may be invoked in relation to the
              following call commands: cm_call_cmd_answer(),
              cm_call_cmd_orig(), cm_call_cmd_end(), cm_call_cmd_setup_res().

              In each case, if the command was successful, no action is
              taken. If the cm_call_cmd_orig() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task. Errors in all
              other call commands do not require any action.

DEPENDENCIES  Assumes a one-call system.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_cb
(
  void                   *data_block_ptr, /* Data blk provided in call cmd */
  cm_call_cmd_e_type      call_cmd,       /* Call command id               */
  cm_call_cmd_err_e_type  call_cmd_err    /* Call command error code       */
);

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_EXT_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of 
              the completion status of the call command previously issued by
              3G Dsmgr.
  
              This callback function may be invoked in relation to
              cm_mm_call_cmd_orig_exten3()

              In each case, if the command was successful, no action is taken.
              If the cm_mm_call_cmd_orig_exten3() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task. Errors in all other
              call commands do not require any action.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_ext_cb
(
  void                          *data_block_ptr,     /* Data block */
  cm_call_cmd_e_type             call_cmd,           /* Call command id */
  cm_call_cmd_err_e_type         call_cmd_err,       /* Call command error code */
  cm_call_cmd_err_cause_e_type   call_cmd_err_cause, /* Cause of the error */
  cm_alpha_s_type                alpha               /* Alpha value */
);

/*===========================================================================
FUNCTION      DS3GI_CM_CALL_CMD_END_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of 
              the completion status of the call command previously issued by
              3G Dsmgr.
  
              This callback function may be invoked in relation to
              cm_mm_call_cmd_exten_end(). Added call_id as one of the 
              parameters.

              In case, if the command was successful, no action is taken.
              If the cm_mm_call_cmd_exten_end() was unsuccessful, a
              DS_CM_CALL_ENDED_CMD is sent to the DS task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_call_cmd_end_cb
(
  void                          *data_block_ptr,     /* Data block */
  cm_call_cmd_e_type             call_cmd,           /* Call command id */
  cm_call_cmd_err_e_type         call_cmd_err,       /* Call command error code */
  cm_call_id_type                call_id             /* Call ID passed with the call command */  
);

/*===========================================================================
FUNCTION      DS3GI_CM_SS_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr
              of the completion status of the ss command previously issued
              by 3G Dsmgr.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ss_cmd_cb
(
  void                 *data_block_ptr,
  cm_ss_cmd_e_type      ss_cmd,
  cm_ss_cmd_err_e_type  ss_cmd_err
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_END_CMD

DESCRIPTION   This function processes the DS_CM_CALL_END_CMD from Call
              Manager. This command is either a confirmation of a release
              initiated by 3G Dsmgr, or a notification of call release by the
              network. Note that if a data call is released from the UI, this
              will appear as a network call release.

DEPENDENCIES  The DS_CM_CALL_END_CMD command is valid in any call state,
              except IDLE. It is ignored if the call state is IDLE, or if
              the specified call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_end_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_INCOM_CMD

DESCRIPTION   This function processes the DS_CM_CALL_INCOM_CMD from Call
              Manager. It calls the mode-specific incoming_call_handler, and
              then either answers the incoming call, rings the serial port or
              rejects the incoming call (i.e. does nothing), depending on the
              return value from the handler function.

DEPENDENCIES  Assumes that simultaneous orig + inc call are always same
              technology.


RETURN VALUE  None

SIDE EFFECTS  Updates the call_state, serial_state, at_state
              and may change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_incom_cmd
(
  ds_cmd_type  *cmd_ptr
);

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_INCOMING_PAGE_CMD

DESCRIPTION   This function processes the DS_CM_CALL_INCOMING_PAGE_CMD from
              Call Manager.
              It checks whether the incoming page can be answered or should
              be rejected.
              And returns the same to CM.
              This is a new event from CM, for SVDO/SVLTE, where if DS is going
              to accept the incoming page, the DO/LTE stack will be shut down.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_incoming_page_cmd
(
  ds_cmd_type  *cmd_ptr
);
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_CONNECTED_CMD

DESCRIPTION   This function processes the DS_CM_CALL_CONNECTED_CMD from Call
              Manager.  This command signifies that either an outgoing or an
              incoming call is connected, and end-to-end data transfer may
              proceed.

              This function invokes the mode-specific call_connected_handler,
              and changes the serial mode to Rawdata or Packet, if directed to
              do so by the mode-specific handler. Note that the serial mode
              may have been changed earlier, either before the origination or
              when the call was answered.

DEPENDENCIES  The DS_CM_CALL_CONNECTED_CMD command is valid only in the ORIG
              and WAIT_FOR_CONNECT call states, and is ignored in all other
              call states. This command is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  Updates the call_state. May also update the serial_state, and
              change the serial mode to Rawdata or Packet.
===========================================================================*/
void ds3gi_process_cm_call_connected_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_SETUP_CMD

DESCRIPTION   This function processes the DS_CM_CALL_SETUP_CMD from Call 
              Manager. It simply calls the mode-specific setup_handler, which
              validates the setup parameters, and either accepts or rejects the
              call.

              If the call is accepted, the mode-specific handler may specify
              new setup parameters which are sent to Call Manager in the setup
              response.
              The setup response may be sent immediately or be delayed at the
              direction of the mode-specific handler.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_SETUP_CMD is valid only in the IDLE or ORIG call 
              states, and is ignored in all other call states.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_setup_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_CONF_CMD

DESCRIPTION   This function processes the DS_CM_CALL_CONF_CMD received from
              Call Manager. It simply calls the mode-specific call_conf_handler
 
              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_CONF_CMD is valid only in the ORIG call state, 
              and is ignored in all other call states. Its is also ignored if
              the call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_conf_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_NW_MODIFY_REQ_CMD

DESCRIPTION   This function processes the DS_CM_CALL_NW_MODIFY_REQ received 
              from Call Manager. It simply calls the mode-specific
              call_nw_modify_handler.
 
              No state change is involved for 3G Dsmgr.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_nw_modify_req_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_PROGRESS_CMD

DESCRIPTION   This function processes the DS_CM_CALL_PROGRESS_CMD from Call
              Manager. It simply calls the mode-specific setup_handler which
              may pass notification to higher-layer clients.
 
              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_PROGRESS_CMD is ignored if the call id is not
              recognized and call is not dual-mode.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_progress_cmd
(
  ds_cmd_type  *cmd_ptr
);

#ifdef FEATURE_MULTIMEDIA_FALLBACK
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_MODIFY_CMD

DESCRIPTION   This function processes the DS_CM_CALL_MODIFY_CMD from Call
              Manager. It simply calls the mode-specific modify_handler which
              may pass notification to higher-layer clients.
 
              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_MODIFY_CMD is valid only in the ACTIVE call state, 
              and is ignored in all other call states. Its is also ignored if
              the call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_modify_cmd
(
  ds_cmd_type  *cmd_ptr
);
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REL_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RAB_REL_CMD received from
              Call Manager.It simply calls the mode-specific rab_rel_handler.

              RAB release indicates that the Radio link has been released, but
              the call stays up. This is done when there is no activity on the
              RAB to conserve radio resources.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_RAB_REL_CMD is valid only in the 
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states.
              It is also ignored if the call id is not recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_call_rab_rel_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_IND_CMD

DESCRIPTION   This function processes the DS_CM_CALL_RAB_REESTAB_IND_CMD
              received from Call Manager.It simply calls the mode-specific
              rab_re_estab_ind_handler.

              RAB Restablishment indication means that the RAB has been
              re-established successfully for the call. The protocol layers
              should now bind to the new RAB that has been established. This
              could be in response to a RAB re-establishment request sent by
              the mode specific handler ( Mobile originated) or just the case
              where the network initiates the RAB re-establishment procedure.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_RAB_REESTAB_IND_CMD is valid only in the 
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. It is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_REJ_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_RAB_REESTAB_REJ_CMD
              received from Call Manager.It simply calls the mode-specific
              call_conf_handler.

              RAB re-establishment reject is sent by the SM layers when the SM
              signalling layer procedures to re-establish the RAB have failed.
              This could be due to network rejecting the request due to loading
              conditions or other factors.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_EVENT_RAB_REESTAB_REJ_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. Its is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_rej_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_RAB_REESTAB_FAIL_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD 
              received from Call Manager.It simply calls the mode-specific
              call_conf_handler.

              RAB re-establihment fail indication is sent when SM signalling
              layer proceeds successfully but the RRC layer could not complete
              the RAB setup.
              SM timesout on the RLC setup procedures and sends this event.

              No state change is involved for 3G Dsmgr.

DEPENDENCIES  The DS_CM_CALL_EVENT_RAB_REESTAB_FAIL_CMD is valid only in the
              DS3G_CALL_ACTIVE_CALL_STATE call state, and is ignored in all
              other call states. Its is also ignored if the call id is not
              recognized.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_rab_reestab_fail_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION   This function processes the DS_CM_SS_SRV_CHG_CMD from 
              Call Manager.
 
              This function checks the suspend state (ps_data_suspend)from CM
              and does the following:
               TRUE  : Calls the mode specific data suspend handler.
               FALSE : Calls the mode specific data suspend handler.
 
               This function also calls a function to set the new mode.

               Pseudo code for HDR mode
               [ HDR service availability]
               1. if ( (sys_mode == HDR) &&
                      (srv_status == service) )
                  HDR is available on main stack
                  In this case, we do not need to change anything, since
                  sys_mode is already set to HDR
 
              2. if ( ( (sys_mode == CDMA) || (sys_mode == No Service) ) &&
                      (hdr_hybrid == TRUE) &&
                      (hdr_srv_status == service) )
                 HDR is available on hybrid stack
                 In this case, we do need to set sys_mode to HDR mode

              Special case for LTE to HDR system change indication from CM

              When system changes from LTE to HDR, CM keeps sys_mode as LTE
              i.e. the main stack still says LTE but the hybrid stack indicates
              HDR. So when DS gets LTE on main stack, it should check if hybrid
              stack is active and if it is then DS should assume that the new
              system is HDR. It is the responsibility of CM to make sure that
              when system moves to LTE, hybrid stack is cleaned up.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_ss_srv_chg_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_PH_EVENT_CMD

DESCRIPTION   This function processes the DS_CMD_CM_PH_EV_INFO for 
              Call Manager PH events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_ph_event_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CONTEXT_MODIFY_CMD

DESCRIPTION
  This function processes the DS_CM_PDP_MODIFY_IND_CMD from Call Manager.

  Used only in UMTS PS call handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void  ds3gi_process_cm_context_modify_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_PROMOTE_CMD

DESCRIPTION   This function processes the DS_CM_PDP_PROMOTE_IND_CMD from 
              Call Manager.
              Used only in UMTS PS call handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_promote_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_PS_DATA_FAIL_CMD

DESCRIPTION   This function processes the DS_CM_SS_PS_DATA_FAIL_CMD from 
              Call Manager.
              This event is posted only when PS is in dormant mode & also in
              power save mode and CM could not acquire service to restore
              ps service.
              This function loops through all the registered handler function
              tables and calls the ps_data_fail_ind_handler() if it is
              registered.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_cm_ss_ps_data_fail_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_IND_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_IND_CMD from Call 
              Manager.
              Used only in UMTS PS call handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_CNF_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_CONF_CMD from Call 
              Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_cnf_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CONTEXT_MODIFY_REJ_CMD

DESCRIPTION   This function processes the DS_CM_PDP_MODIFY_REJ_CMD from 
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_context_modify_rej_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_PS_SIG_REL_CNF_CMD

DESCRIPTION   This function handles the command posted as a result of the
              dormancy indication CM_CALL_EVENT_PS_SIG_REL_CNF from CM.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_ps_sig_rel_cnf_cmd
(
  ds_cmd_type  *cmd_ptr
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_ACT_BEARER_IND_CMD

DESCRIPTION   This function processes the DS_CM_CALL_EVENT_ACT_BEARER_IND 
              from Call Manager. This command is notification of bearer setup
              by network.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_act_bearer_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_PDN_CONN_REJ_IND from CM. This function
              invokes the pdn conn rej handler in the LTE mode handler.
              Note that this indication and function have no meaning in a
              mode other than LTE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_pdn_conn_rej_ind_cmd
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_FAIL_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_PDN_CONN_FAIL_IND from CM. This
              function invokes the pdn conn FAIL handler in the LTE mode
              handler. Note that this indication and function have no meaning
              in a mode other than LTE.

              This indication is given whenever a PDN connection request was
              aborted. This is the way we clean up the resources that have
              beeen allocated for that call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_pdn_conn_fail_ind_cmd
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_GET_PDN_CONN_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the
              indication CM_CALL_EVENT_GET_PDN_CONN_IND from CM. This function
              invokes the attach handler in the LTE mode handler. Note that
              this indication and function have no meaning in a mode other
              than LTE

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_get_pdn_conn_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_ALLOC_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_RES_ALLOC_REJ from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_alloc_rej_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_ALLOC_FAIL_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_RES_ALLOC_FAIL from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_alloc_fail_ind_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_MOD_REJ_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_BEARER_MOD_REJ from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_mod_rej_ind_cmd
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_CALL_BEARER_MOD_IND_CMD

DESCRIPTION   This function handles the command posted as a result of the 
              indication CM_CALL_EVENT_BEARER_MOD_IND from CM. This indication
              and function are applicable only in the LTE mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_call_bearer_mod_ind_cmd
(
  ds_cmd_type *cmd_ptr
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_STANDBY_INFO_CMD

DESCRIPTION   This function processes the DS_CM_STANDBY_INFO_CMD from 
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_standby_info_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_STANDBY_SUB_INFO_CMD

DESCRIPTION   This function processes the DS_CM_STANDBY_SUB_INFO_CMD from 
              Call Manager.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_standby_sub_info_cmd
(
  ds_cmd_type  *cmd_ptr
);

#if defined(FEATURE_DATA_LTE)
/*===========================================================================
FUNCTION      DS3GI_PROCESS_CM_SS_3GPP_CNTXT_TRANSFER_CMD

DESCRIPTION   This function processes the DS_CM_SS_3GPP_CNTXT_TRANSFER_CMD
              from Call Manager.  This command signifies that L<->GW inter
              RAT transition is in progress.

              This function invokes mode-specific cntxt_transfer_ind_handler.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_process_cm_ss_3gpp_cntxt_transfer_cmd
(
  ds_cmd_type  *cmd_ptr
);
#endif /* defined(FEATURE_DATA_LTE) && defined(FEATURE_DS_UMTS_REARCH) */

/*===========================================================================
FUNCTION      DS3G_DISPATCH_SYS_CHANGE_HANDLER_EX

DESCRIPTION   This function only dispatches the mode handlers sys change
              handlers. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_sys_change_handler_ex
(
  sys_sys_mode_e_type     old_mode,
  sys_sys_mode_e_type     new_mode,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_DISPATCH_SYS_CHANGE_HANDLER

DESCRIPTION   This function only dispatches the mode handlers sys change
              handlers. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_sys_change_handler
(
  sys_sys_mode_e_type old_mode,
  sys_sys_mode_e_type new_mode
);

/*===========================================================================
FUNCTION      DS3G_DISPATCH_BSR_RESET_HANDLER

DESCRIPTION   This function only dispatches the mode handlers BSR reset 
              handler. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_bsr_reset_handler
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_PLMN_CHANGE_HDLR 

DESCRIPTION   This function  is used to notify PLMN change so that network 
              override table can be reset for cause code 50 and 51 throttling

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_plmn_change_hdlr
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3G_SET_CURRENT_PLMN_ID

DESCRIPTION   This function sets the new_plmn_id.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_current_plmn_id
(
  sys_plmn_id_s_type   *new_plmn_id,
  sys_sys_mode_e_type   call_mode,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PLMN_ID

DESCRIPTION   This function gets the current_plmn_id.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_current_plmn_id
(
  sys_plmn_id_s_type     *current_plmn_id,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_SUSPEND_EX

DESCRIPTION   If the suspend flag is set to TRUE, the mode specific Suspend Ind 
              handler is called.This function does not check if the current
              suspend state is different than the previous suspend state.
              It is the responsiblility of the mode specific handler to check
              the suspend state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_suspend_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_SUSPEND

DESCRIPTION   This is a wrapper function for the ds3gi_process_data_suspend. 
              This function will be called from DSD IF depending on the
              suspend flag. This is called only for FEATURE DUAL SIM not
              defined. IF the feature is defined, ds3gi_process_data_suspend is
              called from SS event processing. If the suspend flag is set to
              TRUE, the mode specific Suspend Ind handler is called.
 
              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_suspend
(
  sys_sys_mode_e_type  mode
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_RESUME_EX

DESCRIPTION   If the suspend flag is set to FALSE, the mode specific Resume 
              handler is called.
              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_resume_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_DATA_RESUME

DESCRIPTION   This is a wrapper function for the ds3gi_process_data_resume. 
              This function will be called from DSD IF depending on the
              suspend flag. This is called only for FEATURE DUAL SIM not
              defined. IF the feature is defined, ds3gi_process_data_resume is
              called from SS event processing.
 
              If the suspend flag is set to FALSE, the mode specific
              Resume handler is called.
              This function does not check if the current suspend state is
              different than the previous suspend state. It is the
              responsiblility of the mode specific handler to check the suspend
              state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_data_resume
(
  sys_sys_mode_e_type  mode
);

/*===========================================================================
FUNCTION      DS3GI_CM_AC_CMD_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of 
              the completion status of the Acess Control command previously
              issued by 3G Dsmgr.
              This callback function may be invoked in relation to the
              following call commands: cm_ac_cmd_1xPPP_nw_cleanup().
              In each case, if the command was successful, no action is taken.
              If the cm_ac_cmd_1xPPP_nw_cleanup() was unsuccessful we will
              ASSERT.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ac_cmd_cb
(
  void                   *data_block_ptr,        /* Data blk provided in call cmd */
  cm_ac_cmd_e_type        ac_cmd,                /* Access Control command id */
  cm_ac_cmd_err_e_type    ac_cmd_err             /* Access Control command error code */
);

/*===========================================================================
FUNCTION      DS3GI_CM_AC_STATE_CB

DESCRIPTION   Callback function invoked by Call Manager to notify 3G Dsmgr of 
              the current status of the Acess Control command previously issued
              by 3G Dsmgr.
              This callback function may be invoked in relation to the
              following call commands: cm_ac_cmd_1xPPP_nw_cleanup()

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_cm_ac_state_cb
(
  void                    *data_block_ptr,/* Data blk provided in call cmd */
  cm_ac_cmd_e_type         ac_cmd,        /* Access Control command id */
  cm_ac_state_e_type       state,         /* Access Control command state*/
  cm_ac_end_reason_e_type  end_reason     /* Access Control state end reason */
);

/*===========================================================================
FUNCTION      DS3GI_GET_HDLR_TBL_PTR_FOR_CALL_INST

DESCRIPTION   Function that returns a pointer to the appropriate handler 
              function table based on the current mode and call type.

DEPENDENCIES  None

RETURN VALUE  Handler function table pointer

SIDE EFFECTS  None
===========================================================================*/
ds3g_hdlr_func_tbl_type* ds3gi_get_hdlr_tbl_ptr_for_call_inst
(
  ds3gi_call_info_type  *call_inst_ptr
);

#ifdef FEATURE_DATA_CM_CENTRIC_EMERG_MODE
/*===========================================================================
FUNCTION      DS3GI_SET_CM_EMERGENCY_ONLY_MODE_EX

DESCRIPTION   This function calls cm_ac_cmd_control_nw_access to issue 
              whether enter or exit emergency only mode.
  
DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_set_cm_emergency_only_mode_ex
(
  boolean                 is_emergency_only,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GI_SET_CM_EMERGENCY_ONLY_MODE

DESCRIPTION   This function calls cm_ac_cmd_control_nw_access to issue whether 
              enter or exit emergency only mode.
  
DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_set_cm_emergency_only_mode
(
  boolean  is_emergency_only
);

/*===========================================================================
FUNCTION      DS3GI_REG_AC_EVENT

DESCRIPTION   This function register the callback functions for CM access
              control events.
  
DEPENDENCIES  None

RETURN VALUE  TRUE: SUCC, FALSE: FAIL

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_reg_ac_event(void);
#endif /*FEATURE_DATA_CM_CENTRIC_EMERG_MODE*/

/*===========================================================================
FUNCTION      ds3g_is_ph_state_emergency_cb_mode

DESCRIPTION   Function returns if UE is in emergency callback mode or not.

DEPENDENCIES  None

RETURN VALUE  TRUE:  UE is in emergency callback mode 
              FALSE: UE is not in emergency callback mode

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_ph_state_emergency_cb_mode(void);

/*===========================================================================
FUNCTION      DS3G_DISPATCH_MODE_PREF_CHANGE_HANDLER

DESCRIPTION   This function only dispatches the mode handlers sys change
              due to mode pref chnage handlers. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dispatch_mode_pref_change_handler
(
  sys_sys_mode_e_type    old_mode,
  sys_sys_mode_e_type    new_mode,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      ds3g_get_sys_sel_cb_fn_ptr

DESCRIPTION   Returns the sys_sel_cb_fn_ptr

DEPENDENCIES  None

RETURN VALUE  void *

SIDE EFFECTS  None
===========================================================================*/
void *ds3g_get_sys_sel_cb_fn_ptr
(
  void 
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION      DS3G_PRINT_PLMN_ID

DESCRIPTION   This function prints the plmn_id in MCC-MNC format.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_print_plmn_id
(
  void  *current_plmn
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_FLOW_DISABLE_EX

DESCRIPTION   Mode specific flow disable handler is called. This function 
              does not check if the current flow state is different than the
              previous flow state.
              It is the responsiblility of the mode specific handler to check
              the flow state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_flow_disable_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_FLOW_ENABLE_EX

DESCRIPTION   Mode specific flow disable handler is called. This function 
              does not check if the current flow state is different than the
              previous flow state.
              It is the responsiblility of the mode specific handler to check
              the flow state and act accordingly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_flow_enable_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_TOTAL_ULDL_WM_CNT_EX

DESCRIPTION   Mode specific get total UL & DL wm cnt handler is called.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint64 ds3g_get_total_uldl_wm_cnt_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      DS3G_PROCESS_BRINGDOWN_PHYS_LINK_EX

DESCRIPTION   Mode specific get total UL & DL wm cnt handler is called.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_bringdown_phys_link_ex
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION      ds3g_process_pdn_conn_req_err_cmd

DESCRIPTION   This functions processes PDN Connection request error
               command from CM and does mode specific handling

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_pdn_conn_req_err_cmd
(
  ds_cmd_type  *cmd_ptr
);

#endif /* DS3GCMIF_H */
