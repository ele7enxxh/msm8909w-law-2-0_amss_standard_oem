#ifndef DS707_ASYNC_MGR_H
#define DS707_ASYNC_MGR_H
/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ M G R 
DESCRIPTION
  This file contains the main functionality for IS707 async/fax mode-specific
  handlers. It mainly deals with call control.

EXTERNALIZED FUNCTIONS
  ds707_async_mgr_powerup_init()
    Called at powerup to initialize data structures, register with 3gdsmgr.

  ds707_async_get_active_iface_ptr()
    Function that returns pointer to 707 async iface if it's active.
    
  ds707_async_ps_tcp_enqueue()
    Function to queue data to be sent over async/fax air interface.

  ds707_async_process_cmd()
    Function that processes commands to Async/Fax controller.

  ds707_async_process_signals()   
    Function that processes signals to Async/Fax controller.
  
   

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_async_mgr_powerup_init() must be called at startup.

Copyright (c) 2002-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_mgr.h_v   1.9   23 Jan 2003 16:37:18   ajithp  $
    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_async_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/30/14    niv    Added wrapper functions for NV_REFRESH 
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
11/22/10    sn     Replaced REX premption macros with critical section for 
                   mutual exclusion.
03/04/09    sa     AU level CMI modifications.
04/27/08    pp     Added meta_info_ptr as an arg to ds707_async_tx_um_SN_data.
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function  
07/06/05    ar     Add cause parameter to ds707_async_user_answer_hdlr().
02/04/04    ak     Updated to use ps_phys_link for phys_link ops.
04/18/03    ak     Updated switch_call_handler to take extra cmd_ptr param.
01/23/03    atp    Added handlers for returning RLP stats, connection info.
01/20/03    ar     Added dsat_dial_modifier_info_type to 
                   ds707_async_dial_str_cb interface.
12/02/02    atp    Added header, comments, etc.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/10/02    atp    Sync up to changes in signature of dial_str_cb(), 
                   call_answer_cb() from atcop.
08/22/02    atp    Cleanup for shipping.
08/20/02    atp    Additional handlers (force_down, etc) to sync with ds3gmgr.
08/14/02    atp    Added ATZ handler.
07/28/02    atp    Updated comments.
07/24/02    atp    Added ds707_async_phys_link_tear_down().
07/21/02    atp    Added support for incoming calls.
07/16/02    ak     Updated for incoming_call_hdlr taking cmd_ptr.
07/16/02    atp    Added ds707_async_get_active_iface_ptr().
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "cm.h"
#include "ds3gmgr.h"
#include "dstask.h"
#include "ps_iface.h"
#include "rex.h"

/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*-----------------------------------------------------------------------------
  Global critical section for ASYNC call externed through this header.
-----------------------------------------------------------------------------*/
extern rex_crit_sect_type ds707_async_global_crit_sect;


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
                        DS707 ASYNC MGR INITIALIZATION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_MGR_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate 
              functionality with 3G DSMGR.
 
DEPENDENCIES  Assumes common watermarks, RLP, etc are initialized elsewhere.
              The things initialized here are specific to 707 async.

RETURN VALUE  Signals that DS should look for.

SIDE EFFECTS  None.
===========================================================================*/
rex_sigs_type ds707_async_mgr_powerup_init
(
  void
);


/*===========================================================================
                           3G DSMGR HANDLERS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_ASYNC_ORIG_HDLR

DESCRIPTION   Called from 3G DSMGR when originating an async data call.
 
DEPENDENCIES  Watermarks to RLP already set up correctly.  Passed in ptrs are
              not NULL.

RETURN VALUE  DS3G_SUCCESS - call can be made.  
              DS3G_FAILURE - call can't be made.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_orig_hdlr
(
  const byte                     *dial_string,                    
  uint8                          dial_string_len,
  cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
  boolean                        *cdma_params_changed,
  cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
  boolean                        *gw_cs_params_changed,
  cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
  boolean                        *gw_ps_params_changed,
  cm_srv_type_e_type             *cm_srv_type,
  void                           *mode_spec_info_ptr,
  sys_modem_as_id_e_type          subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ID_HDLR

DESCRIPTION   Called after call originates.  Gives the CM call id to this
              mode specific handler.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_id_hdlr
(
  cm_call_id_type        cm_call_id,
  void                    *mode_spec_info_ptr,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_CONN_HDLR

DESCRIPTION   Called by 3G DSMGR when traffic channel connects.
 
DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS - call connected ok.
              DS3G_FAILURE - call failed.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_call_conn_hdlr
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_INCOMING_CALL_HDLR

DESCRIPTION   Called when CM receives a page from BS.  
 
DEPENDENCIES  None.

RETURN VALUE  Tells ds3gmgr alerting user or reject call if error.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_incom_e_type ds707_async_incoming_call_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params, /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params,  /* end params if call rejected      */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_USER_ANSWER_HDLR

DESCRIPTION   Called by CM after it gets the go-ahead to answer call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_user_answer_hdlr
(
  cm_call_id_type        cm_call_id,
  cm_ans_params_s_type  *ans_parameters, /* Answer params sent to Call Mgr  */
  ie_cm_cc_cause_s_type *cause_ptr       /* Call reject cause information   */
);

/*===========================================================================
FUNCTION      DS707_ASYNC_USER_END_HDLR

DESCRIPTION   Called by 3G DSMGR before telling CM to end a call.  

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_user_end_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params,
  boolean               *delay_disconnect,
  void                  *mode_spec_info_ptr
);

/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ENDED_HDLR

DESCRIPTION   Called by 3G DSMGR when a call release has been received from
              CM.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr,
  boolean                 call_was_connected,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_SWITCH_CALLS_HDLR

DESCRIPTION   Called when CM receives an incoming page for the same type of
              call as what we are originating.
              
              In this case, we want to end the origination, but accept the
              incoming page.  We must then switch our call_id to the 
              incoming call id.
 
DEPENDENCIES  None.

RETURN VALUE  Tells ds3gmgr alerting user or reject call if error, for 
              incoming call. 

SIDE EFFECTS  None.
===========================================================================*/
ds3g_incom_e_type ds707_async_switch_calls_hdlr
(
  ds_cmd_type           *cmd_ptr,        /* Info from CM                   */  
  cm_call_id_type        inc_call_id,    /* incoming call id               */
  cm_call_id_type        exist_call_id,  /* outgoing call id               */
  cm_ans_params_s_type  *ans_params,     /* parms for call to be accepted  */
  cm_end_params_s_type  *end_params_orig,/* parms for call to be ended     */
  cm_end_params_s_type   *end_params_inc, /* if incoming call to be ended  */
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_FORCE_CALL_DOWN_HDLR

DESCRIPTION   Called by 3G DSMGR when there's a simultaneous incoming and
              outgoing call. The outgoing call has to be torn down and needs
              end params which are supplied in this function. The call ended
              handler won't be called later in this case.

DEPENDENCIES  None.

RETURN VALUE  TRUE - Indicates there was a call to be torn down.
              FALSE- Indicates there were no active/originating calls.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_force_call_down_hdlr
(
  cm_call_id_type      exist_call_id,/*Call id of outgoing call to be ended*/
  cm_end_params_s_type    *end_params,    /*End params to be filled in function */
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_GET_CALL_CONN_INFO_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes ds_get_call_information()
              to get call info Eg: "CDMA" as a NULL terminated string.

DEPENDENCIES  None.

RETURN VALUE  Indicates if call connection info is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_get_call_conn_info_hdlr
(
  cm_call_id_type call_id,
  byte            *system_information,
  byte            max_system_information_length
);

/*===========================================================================
FUNCTION      DS707_ASYNC_GET_DATA_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_current_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean  ds707_async_get_data_count_hdlr
(
  cm_call_id_type    call_id,
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
);

/*===========================================================================
FUNCTION      DS707_ASYNC_GET_LAST_SESS_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_last_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the last call which
              is being released or current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean  ds707_async_get_last_sess_count_hdlr
(
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
);


/*===========================================================================
                         IFACE HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_BRING_UP

DESCRIPTION   Called via the async IFACE.  Brings up the async iface.

DEPENDENCIES  None.

RETURN VALUE  0 for success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_iface_bring_up
(
  ps_iface_type    *iface_ptr,                   /* Pointer to async iface */
  void             *unused
);

/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_BRING_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface up command
              on the async iface.
                  
DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_iface_bring_up_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_TEAR_DOWN

DESCRIPTION   Called via the async IFACE.  Tears down the async iface.

DEPENDENCIES  None.

RETURN VALUE  0 for success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_iface_tear_down
(
  ps_iface_type    *iface_ptr,
  void             *not_used
);

/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_TEAR_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface down command
              on the async iface.
                  
DEPENDENCIES  Called in DS TASK context.  Called only when PS_IFACE is up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_iface_tear_down_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_GET_ACTIVE_IFACE_PTR

DESCRIPTION   Function that returns pointer to 707 async iface if it's
              active. 

DEPENDENCIES  None.

RETURN VALUE  Ptr to 707 async ps_iface if active, else NULL.

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_async_get_active_iface_ptr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PHYS_LINK_TEAR_DOWN

DESCRIPTION   Called via the async IFACE.  Tears down the traffic channel.
              Sends a message to the DS task which will cause the
              channel to be torn down.

DEPENDENCIES  None.

RETURN VALUE  0 for success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_phys_link_tear_down
(
  ps_phys_link_type    *phys_link_ptr,
  void              *not_used
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to a phys link tear down
              command on the async iface.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_phys_link_tear_down_cmd_hdlr
(
  void
);


/*===========================================================================
                     DIAL STRING CALLBACKS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_DIAL_STR_CB()

DESCRIPTION   This function processes the dial string it has registered for
              and initiates the appropriate data call over IS707_ASYNC_IFACE.

RETURN VALUE  Ptr to call_abort_cb().

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
dsat_call_abort_cb_type ds707_async_dial_str_cb
(
  const byte *dial_string,                  /* Dial string to be processed */ 
  boolean    digit_mode, /* Flag tells if dial string contains digits only */
  dsat_dial_modifier_info_type  * modifiers,       /* Dial sting modifiers  */
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ANSWER_CB()

DESCRIPTION   This function is registered with atcop for handling ATA during
              an incoming call.

RETURN VALUE  Ptr to call_abort_cb().

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
dsat_call_abort_cb_type ds707_async_call_answer_cb
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ABORT_CB()

DESCRIPTION   This function is registered as abort handler with ATCOP. If
              ATCOP receives an AT cmd while processing an abortable cmd,
              this function gets called.  It is registered along with the
              dial string handler. It posts appropriate event to async_mgr
              which initiates the call termination.

RETURN VALUE  None.

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_abort_cb
(
  void
);


/*===========================================================================
                           3G SIOLIB HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_ESC_SEQ_HDLR

DESCRIPTION   Handler registered with SIO to be called when "+++" is entered
              on laptop.  Sends EIA 617 "escape to online command" message
              to IWF.

DEPENDENCIES  None.

RETURN VALUE  ONLINE_DATA mode, as mobile doesn't go to ONLINE_CMD mode in
              case of 707 async.

SIDE EFFECTS  None.
===========================================================================*/
dsat_mode_enum_type ds707_async_esc_seq_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_DTR_CHANGED_HDLR

DESCRIPTION   Handler registered with SIO to be called when there is a
              change in dtr state.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_dtr_changed_hdlr
(
  boolean dtr_status
);

/*===========================================================================
FUNCTION      DS707_ASYNC_AT_RSP_FMTING_FUNC_HDLR

DESCRIPTION   Handler registered with atcop to be called when an AT response
              needs to be formatted in online mode. This function does the
              insertion of 617 codes.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_at_rsp_fmting_func_hdlr
(
  dsm_item_type **in_ptr_ptr,         /* Raw response to format in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in
                                         in_ptr_ptr                        */
  boolean is_result_code,             /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr,        /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
  dsat_result_enum_type result        /* Result code                       */
);


/*===========================================================================
FUNCTION      DS707_ASYNC_AT_RSP_ROUTING_FUNC_HDLR

DESCRIPTION   Handler registered with atcop to be called when an AT response
              needs to be sent in online data mode. This function does the
              enqueuing to Um.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_at_rsp_routing_func_hdlr
(
  dsm_item_type *rsp_ptr,              /* Final response to be sent        */
  const dsat_mode_enum_type at_state,  /* Mode (cmd/online_data/online_cmd */
  const boolean rearm_autodetect       /* flag to rearm autodetect         */
);

/*===========================================================================
FUNCTION      DS707_ASYNC_ATZ_HDLR

DESCRIPTION   Handler registered with atcop to be called when an ATZ command
              comes in.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  Hangs up call.
===========================================================================*/
void ds707_async_atz_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_ATZ_CMD_HDLR

DESCRIPTION   Called in DS task context in response to an ATZ cmd called
              from atcop.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Hangs up call.
===========================================================================*/
void ds707_async_atz_cmd_hdlr
(
  void
);

/*===========================================================================
                          ENQUEUE DATA 
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_PS_TCP_ENQUEUE

DESCRIPTION   This function adds the passed dsm item to the ps707_tcp_send_q.
              It can be called at ISR level by the serial driver to send
              serial data or at the DS task level by dsatcop to send 617
              reflected commands and results.

DEPENDENCIES  The value of the passed item ptr becomes NULL after this.

RETURN VALUE  None.

SIDE EFFECTS  Changes ps707_tcp_send_q and ps707_tcp_send_q_cnt.
===========================================================================*/
void ds707_async_ps_tcp_enqueue
(
  dsm_item_type **item_ptr  /* Item containing data to be sent over Um TCP */
);


/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_ASYNC_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this 
              mode handler.
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_async_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PROCESS_SIGNALS

DESCRIPTION   Processes sigals set by other parts of the system.  Currently,
              no signals should be set for this mode handler.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_process_signals
(
  rex_sigs_type   sigs
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD_HDLR

DESCRIPTION   Called when timer expires for protocols to open (PPP + TCP
              conn establishment) after physical channel is established.
              Tear down call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_opening_timer_expired_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_OPENED_CMD_HDLR

DESCRIPTION   Called when DS gets cmd from PS saying protocol stack opened,
              ie TCP connection with IWF successfully established.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_opened_cmd_hdlr
(
  void
);

/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_CLOSED_CMD_HDLR

DESCRIPTION   Called when DS gets cmd from PS saying protocol stack closed,
              ie TCP connection with IWF disconnected.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_closed_cmd_hdlr
(
  void
);


/*===========================================================================
FUNCTION      DS707_ASYNC_SETUP_RLP

DESCRIPTION   Sets up RLP with a TX watermark and RLP RX func and RLP RX
              queue.  If the inputs are NULL, then use defaults as 
              defined by this module.  Else use the ones passed into this
              function.
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_setup_rlp
(
  dsm_watermark_type  *tx_watermark_ptr,         /* Tx wm ptr for txmit frs*/
  void                (*post_rx_func_ptr)(uint32),/* rx cb    for rx frames */
  dsm_watermark_type  *post_rx_wm_ptr            /* RLP Rx watermark  */
);


/*===========================================================================
FUNCTION      DS707_ASYNC_POST_RLP_RX_FUNC()

DESCRIPTION   This function performs the RLP post-RX processing function
              for the IS99 service. The PPP RX signal is set which indicates
              to the PS task there are RLP data frame(s) ready for PPP
              processing.

              This function runs in the context of the RX task.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_post_rlp_rx_func
( 
  uint32 unused
);

/*===========================================================================
FUNCTION      DS707_ASYNC_RX_UM_SN_DATA

DESCRIPTION   Called by PPP to get forward-link data from RLP.  

DEPENDENCIES  None.

RETURN VALUE  A pointer to a DSM item.  DSM item may be a packet chain.
              If there is no data to return, then returns NULL.

SIDE EFFECTS  None.
===========================================================================*/
dsm_item_type* ds707_async_rx_um_SN_data
(
  void *user_data
);

/*===========================================================================
FUNCTION      DS707_ASYNC_TX_UM_SN_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.  

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_tx_um_SN_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void             *meta_info_ptr,
  void *user_data
);

/*===========================================================================
FUNCTION      DS707_ASYNC_SET_CS_DATA_SERVICE

DESCRIPTION   Set the value for cs data service

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_async_set_cs_data_service 
( 
  boolean   val
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DS707_ASYNC_MGR_H    */

