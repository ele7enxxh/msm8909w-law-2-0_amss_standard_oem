#ifndef DS3GMGR_H
#define DS3GMGR_H
/*===========================================================================

                3 G   D A T A   S E R V I C E S   M A N A G E R

                           H E A D E R   F I L E

DESCRIPTION
  This file contains functions and definitions exported by 3G Dsmgr to other
  Data Services software units, namely: the top-level task dispatcher, ATCoP
  and Mode-Specific Handlers.

  Copyright (c) 2001-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgr.h_v   1.5   28 Feb 2003 18:54:50   rchar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gmgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/24/13   sd      TSTS changes for DS3G.
05/02/13   vs      NV item to check call origination is allowed before PS attach
11/21/12   sd      Feature Dual SIM related changes.
03/09/12   msh     Removed FEATURE_DATA_NIKEL 
02/01/12   ss      RPM: Fixed of sending ATTACH request again due to PS session
                   initiation after PS ATTACH is ignored by network.
01/25/12   nd      Deprecating API cm_get_pref_srv_type_for_packet_orig usage.
01/02/12   msh     Coan: Feature cleanup
01/13/12   sa      Added support for CSVT for CSFB targets.
09/05/11   ss      Changes to support RPM.
08/09/11   sha     Global variable clean up.
03/23/11   sn      Globals cleanup for free floating of tasks.
11/22/10   ttv     Added support for DSDS.
06/17/10   as      Added 2nd version (ext) to get current n/w
08/22/08   ua      Added support for N/w Initiated Qos
09/22/09   vrk     Merged LTE changes.
09/04/09   ss      CMI SU level modifications.
03/04/09   sa      AU level CMI modifications.
01/09/08   yz      Added support for ISIM refresh voting feature.
02/16/06   ar      Merge in support for multimedia call fallback.
09/16/05   as/gr   Added function ds3g_is_dorm_allowed_in_pkt_mode to allow
                   dormancy in WLAN mode.
09/02/05   rc      Added support for Mobile Initiated QOS Modification.
08/05/05   ar      Added call_modify_handler for call modify events and
                   call_originated_handler for external call orig events.
06/30/05   ar      Added supprot for asynchronous call setup confirmation.
05/20/05   ar      Added supprot for call_progress_handler.
09/17/04   rc      Added ps_data_fail_ind_handler() to support power save
                   mode operation.
09/10/04   sb      S-PDP changes
10/07/03   vsk     Added ds3g_call_limit_e_type definition
09/16/03   ar      Added ds3g_get_call_was_connected() prototype.
07/25/03   ak      Added sys_chng_hdlr function to handler table.
07/21/03   usb     Added ds3g_is_pref_pkt_mode to validate currently
                   preferred  mode for packet data calls.
05/21/03   vas     Added DS3G_IGNORE enum to ds3g_incom_e_type.
04/18/03   ak      Added parameter to switch_calls interface, to support
                   bypass alert in 1x.
02/26/03   rc      Added data_suspend_ind_handler() and 
                   data_resume_ind_handler()
11/04/02   tmr     Added call_was_connected paramter to call_ended_handler
10/14/02   vsk     Added support to get statistics for the call which has ended
08/19/02   ak      Updated for multiple calls.
08/13/02   dgy     Added return value to get_call_connection_info_handler() 
                   and get_data_counters_handler().
08/02/02   dgy     Added support for UI data call screen.
07/24/02   atp     Added DS3G_ALERT_USER to ds3g_incom_e_type enum.
07/16/02   ak      Incoming call handler also takes cmd_ptr.
07/01/02   ak      add cm_srv_type to orig_hdlr prototype.
06/18/02   tmr     Added the delay_disconnect paramter to user_end_handler() and
                   added the complete_ll_disconnect_handler()
06/05/02   ar      Removed online-command support wrapper functions. 
                   Added at_return_to_online_data_complete_handler().
05/21/02   smp     Removed ds3g_mode_e_type definition. Use sys_sys_mode_e_type
                   instead.
05/16/02   smp     Updates for Multimode CM.
05/12/02   ak      Updated parameters for mode-specific handlers.
04/30/02   ak      Changed call_id_hdlr signature to take mode_spec info.
04/30/02   ak      Changed orig_hdlr signature.  Changed ds3g_initiate_call()
                   signature.
04/19/02   vsk     Added PDP context preservation support
04/12/02   rc      Removed enum DS3G_CALL_TYPE_E_TYPE. Added parameter
                   stack_type to originate_call_handler,
                   incoming_call_handler, and ds3g_initiate_call().                   
02/25/02   tmr     Added complete_ll_connect_handler() to mode-specific
                   handler function table
02/19/02   rlp     Code review changes.
02/13/02   vsk     removed call_id in call orig handler function and also
                   changed prototypes to use cm_call_id_type
02/07/02   vsk     Added ds3gi_get_current_networ_mode() function prototype
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added end_params to call
                   confirm, call connected and user end handlers, since CM
                   now needs this.
12/14/01   smp     Added function prototype for ds3g_stop_alerting() and
                   renamed ds3g_ring_serial_port() to ds3g_alert_user().
08/31/01   smp     Updates due to CM name changes.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/05/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "rex.h"
#include "cm.h"
#include "cm_dualsim.h"
#include "sys.h"
#include "dsm.h"
#include "dstask_v.h"
#include "dsat_v.h"
#include "dsmgr_api.h"
#include "ps_ip6_sm.h"
#include "ds_sys.h"
#include "ds3g_ext.h"
#include "ds3gsubsmgr.h"
#include "ds3geventmgr.h"
#include "sys_v.h"
#include "reg_sim.h"
#include "ps_sys_event.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  3G APN name for a PDN.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                     apn_string_len;
  char                       apn_string[DS_SYS_MAX_APN_LEN];
}ds3g_apn_name_type;

/*-------------------------------------------------------------------------- 
  Local typedef to pass LTE attach SM parameters
  It currently contains
  1. Seq_num - Attach req or not
  2. responding to page - Indicates if the attach request is sent out
                          because of a page from the network
  --------------------------------------------------------------------------*/

typedef struct
{
  boolean responding_to_page;
  uint8   seq_num;
}ds3g_eps_attach_sm_info_type;

/*---------------------------------------------------------------------------
  APN Packet System Mode used internally by DS modules for packet system mode
  for an APN. The enum value are the same as for sys_sys_mode_e_type if
  covered by sys_sys_mode_e_type.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PKT_SYS_MODE_NONE   = SYS_SYS_MODE_NONE,  /* -1: internal use */

  DS_PKT_SYS_MODE_NO_SRV =  SYS_SYS_MODE_NO_SRV,
    /**< No service */

  DS_PKT_SYS_MODE_AMPS   =  SYS_SYS_MODE_AMPS,
    /**< Analog Mobile Phone System (AMPS) mode. */

  DS_PKT_SYS_MODE_CDMA   =  SYS_SYS_MODE_CDMA,
    /**< CDMA mode. */

  DS_PKT_SYS_MODE_GSM    =  SYS_SYS_MODE_GSM,
    /**< GSM mode. */

  DS_PKT_SYS_MODE_HDR    =  SYS_SYS_MODE_HDR,
    /**< HDR mode. */

  DS_PKT_SYS_MODE_WCDMA  =  SYS_SYS_MODE_WCDMA,
    /**< WCDMA mode. */

  DS_PKT_SYS_MODE_GPS    =  SYS_SYS_MODE_GPS,
    /**< GPS mode. */

  DS_PKT_SYS_MODE_GW     =  SYS_SYS_MODE_GW,
    /**< GSM and WCDMA mode. */

  DS_PKT_SYS_MODE_WLAN   =  SYS_SYS_MODE_WLAN,
    /**< WLAN mode. */

  DS_PKT_SYS_MODE_LTE    =  SYS_SYS_MODE_LTE,
    /**< LTE mode. */

  DS_PKT_SYS_MODE_GWL    =  SYS_SYS_MODE_GWL,
    /**< GSM, WCDMA, and LTE mode. */

  DS_PKT_SYS_MODE_TDS    =  SYS_SYS_MODE_TDS,
    /**< TDS mode. */

  DS_PKT_SYS_MODE_IWLAN,

  /** @cond
  */
  DS3G_PKT_SYS_MODES_NUM   /* FOR INTERNAL USE ONLY! */
  /** @endcond
  */

} ds_pkt_sys_mode_e_type;

/*---------------------------------------------------------------------------
  Action to be taken when an incoming call is received. This is determined by
  the mode-specific handler.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_ANSWER_CALL,                /* Answer the incoming call immediately */
  DS3G_REJECT_CALL,                /* Reject the incoming call             */
  DS3G_ALERT_USER,                 /* Ring laptop                          */
  DS3G_DO_NOTHING,                 /* do nothing (may be ringing TE...?)   */
  DS3G_IGNORE                      /* Ignore the incoming call             */
} ds3g_incom_e_type;


#define DS_3GPP_IS_3GPP_CURRENT_MODE(call_mode)  \
  (                                              \
      (call_mode == SYS_SYS_MODE_WCDMA) ||       \
      (call_mode == SYS_SYS_MODE_GSM)   ||       \
      (call_mode == SYS_SYS_MODE_LTE)   ||       \
      (call_mode == SYS_SYS_MODE_TDS)            \
  )

/*---------------------------------------------------------------------------
  Result returned by mode-specific handler functions.
---------------------------------------------------------------------------*/
#define DS3G_SUCCESS        1
#define DS3G_FAILURE        0

/* Maximum number of valid subscriptions */
#define DS3G_MAX_SUBS DS3GSUBSMGR_SUBS_ID_COUNT

/*---------------------------------------------------------------------------
  Max CALL ID
---------------------------------------------------------------------------*/
#define DS3G_CALL_ID_MAX    255

#define DS3G_IS_SUBS_ID_VALID(subs_id) \
  ( ( (subs_id > SYS_MODEM_AS_ID_NONE) && \
      (subs_id < SYS_MODEM_AS_ID_MAX - 1) ) ? TRUE : FALSE )

/*---------------------------------------------------------------------------
  Table listing the number of calls allowed in DS per system mode.
  And if CS+PS calls are allowed if multiple calls are allowed
---------------------------------------------------------------------------*/
typedef struct
{
  byte  num_cs_calls;               /* number of cs calls allowed cs + fax */
  byte  num_ps_calls;               /* number of PS calls allowed          */
  boolean cs_and_ps_allowed_flag;   /* Is concurrent CS + PS calls allowed */
}ds3g_call_limit_e_type;


typedef union
{
  cm_gw_ps_call_info_s_type     gw_ps_call;
      /* PS GSM or WCDMA specific info */
#ifdef FEATURE_DATA_LTE
  cm_lte_call_info_s_type       lte_call;
   /* LTE specific call info*/
#endif /* FEATURE_DATA_LTE */
} dsgwl_ps_call_info_u_type;


/*---------------------------------------------------------------------------
  This is a union of all the orig parameters. This will be used in the 
  orig_call_hdlr_ext function to get orig params. This new orig_call_hdlr_ext
  will be applicable to all the modes. initate_call in dsmgr should also
  switch to this whenever possible.
---------------------------------------------------------------------------*/
typedef struct
{
  cm_cdma_orig_params_s_type   *cdma_orig_params_p;
  boolean                      *cdma_orig_params_changed;
  cm_gw_cs_orig_params_s_type  *gw_cs_orig_params_p;
  boolean                      *gw_cs_orig_params_changed;
  cm_gw_ps_orig_params_s_type  *gw_ps_orig_params_p;
  boolean                      *gw_ps_orig_params_changed;
#ifdef FEATURE_DATA_LTE
  cm_pdn_connectivity_s_type   *lte_orig_params_p;
  boolean                      *lte_orig_params_changed;
#endif /* FEATURE_DATA_LTE */
}ds3g_orig_params_s;

/*---------------------------------------------------------------------------
  This is a enum of the cliend ids that will subscribe to DDS/Device mode
  change events in ds3gsubsmgr: ds3gsubsmgr_event_reg()
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_CLIENT_ID_INVALID = -1,
  DS3G_CLIENT_ID_MIN     = 0,
  DS3G_CLIENT_ID_MAX
}ds3g_client_id;

/*---------------------------------------------------------------------------
  Structure for CM PH event info
---------------------------------------------------------------------------*/
typedef struct
{
  cm_ph_event_e_type                  ph_event;
  sys_modem_as_id_e_type              as_id;
  cm_srv_domain_pref_e_type           srv_domain_pref;
  sys_oprt_mode_e_type                oprt_mode;
  cm_mode_pref_e_type                 mode_pref;
  cm_ph_state_e_type                  ph_state;
  sys_modem_device_mode_e_type        device_mode;
  sys_modem_dual_standby_pref_e_type  standby_pref;
  sys_modem_as_id_e_type              active_subs;
  sys_modem_as_id_e_type              default_voice_subs;
  sys_modem_as_id_e_type              default_data_subs;
  cm_hybr_pref_e_type                 hybr_pref;
  boolean                             emerg_to_normal_mode;
  sys_data_priority_e_type            priority_type;
} ds3g_cm_ph_ev_info_type;

/*---------------------------------------------------------------------------
  Structure for Subscription info
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type            subscription_id;
    /**< Active Subscription ID assigned to this subscription */

  boolean                           is_subphone_active;
  /**< Flag indicating if this subphone is active. */

  mmgsdi_static_data_type           app_id_1x;
    /**< 1x Application ID associated with this subscription. */

  mmgsdi_session_type_enum_type     session_type_1x;
    /**< 1x UIM session type on which this subscription is currently provisioned. */
		
  mmgsdi_static_data_type           app_id_gw;
    /**< GW Application ID associated with this subscription. */

  mmgsdi_session_type_enum_type     session_type_gw;
    /**< GW UIM session type on which this subscription is currently provisioned. */	

  boolean                           is_operational;
    /**< Flag indicating if this subscription is in operation right now */

  boolean                           is_priority_subs;
    /**< Flag indicating if this subscription is the priority subscription */

  boolean                           is_default_voice;
    /**< Flag indicating if this subscription is the default voice subs */

  boolean                           is_default_data;
    /**< Flag indicating if this subscription is the default data subs */

  cm_pref_s_type                    pref_info;
    /**< Acquisition preferences for this subscription. */

  uint8                             uicc_id[CM_MMGSDI_MAX_UICC_LEN];
    /**< UICC ID of the card to which this subscription belongs. */

  sys_subs_feature_t                sub_feature_mode;
    /**< Enum indicating if this subscription is SGLTE */

  boolean                           is_available_gw;
  /**< Flag indicating if GW subscription is available */

  sys_sys_mode_mask_e_type          subs_capability;
  /**< Indicating maximum subscription capability, mainly gives info on multimode subs*/  
} ds3g_cm_sub_info_type;


/*---------------------------------------------------------------------------
  Struct for DS_CMD_CM_AC_STATE
---------------------------------------------------------------------------*/
typedef struct
{
  cm_ac_state_e_type       state;       /* Access Control state */
  cm_ac_end_reason_e_type  end_reason;  /* Access Control end reason */
} ds3g_cm_ac_state_info_type;

/*---------------------------------------------------------------------------
  Struct for DS_CMD_CM_AC_EVENT
---------------------------------------------------------------------------*/
typedef struct
{
  cm_ac_event_e_type        event;       /* Access Control  event */
} ds3g_cm_ac_event_info_type;

/*---------------------------------------------------------------------------
 Type to distinguish if CALL END Command is from CM or Internal
---------------------------------------------------------------------------*/
typedef enum
{
  DS_CM_CALL_END_NONE = -1,
  DS_CM_CALL_END_EXTERNAL,
  DS_CM_CALL_END_INTERNAL,
  DS_CM_CALL_END_MAX
} ds3g_cm_call_end_e_type;

/*---------------------------------------------------------------------------
  Structure for CM call info
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type          subs_id;
  cm_call_id_type                 call_id;
  cm_client_id_type               client_id;
  cm_call_event_e_type            call_event;
  cm_call_type_e_type             call_type;
  cm_call_type_e_type             cm_call_type;
  sys_sys_mode_e_type             call_mode;
  cm_call_mode_info_s_type        mode_info;
  cm_call_end_e_type              end_status;
  cm_call_cmd_err_e_type          call_cmd_err;
  uint8                           seq_num;
  boolean                         responding_to_page;/* If Yes, no Low priority signaling */
  cm_call_event_user_data_s_type  uus_info;
  ds3g_cm_call_end_e_type         end_type;
  uint32                          call_inst;
  boolean                         failed_orig;
  uint32                          sdf_id;
} ds3g_call_info_cmd_type;

/*---------------------------------------------------------------------------
  Structure passed with DS_CMD_3GPP_SYS_CHG_SS_INFO command.
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  sys_sys_mode_e_type       sys_mode;
  cm_cell_srv_ind_s_type    cell_srv_ind;
#ifdef FEATURE_DATA_LTE
  sys_div_duplex_e_type     div_duplex;
#endif /* FEATURE_DATA_LTE */
}ds3g_sys_chg_ss_info_type;

/*---------------------------------------------------------------------------
  Structure passed with DS_CMD_3GPP_ROAMING_STATUS_CHG command.
---------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type  subscription_id;
  sys_roam_status_e_type  gw_roam_status;
}ds3g_roaming_ind_type;

/*---------------------------------------------------------------------------
  Table of handler functions for circuit-switched data calls. Each circuit-
  switched mode-specific handler maintains such a table, and registers this
  table with 3G Dsmgr.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    This handler is invoked when the user initiates a data call. The mode-
    specific handler should specify the orig_params for the technology
    it will use.  The *_params_changed for the technology should also be set
    to TRUE.  The mode_spec_orig_info_ptr contains data that is passed to
    CM by the entity that wants to originate the call.  Current call models
    have the mode-specific handler (or some mode-specific entity) making 
    that call.  The information within the struct is defined by the mode-spec
    modules.
    
    User must also fill out the SRV type of his call.
  -------------------------------------------------------------------------*/
  boolean (*originate_call_handler)
                  (
                    const byte                     *dial_string,
                    uint8                           dial_string_len,
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

  /*-------------------------------------------------------------------------
    This handler is invoked after the origination, in order to pass the call
    id allocated by Call Mgr to the mode-specific handler.  The mode_spec
    info is passed into ds3g_initiate_call, and passed back to the mode_hdlr
    in this call.
  -------------------------------------------------------------------------*/
  void (*call_id_handler)
                  (
                    cm_call_id_type          call_id,
                    void                    *mode_spec_info_ptr,
                    sys_modem_as_id_e_type   subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_CALL_CONF event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should validate any
    parameters that may have been proposed by the network. A return value of
    SUCCESS indicates that the parameters are acceptable, FAILURE indicates
    that the parameters are not acceptable i.e. the call should be released.
    The clear flag indicates the call has no data mode and should not be
    managed by Data Services.
  -------------------------------------------------------------------------*/
  boolean (*call_conf_handler)
                  (
                    cm_call_id_type             call_id,
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    cm_end_params_s_type       *end_params,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
                    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_CALL_PROGRESS_IND
    event is rx'ed from Call Mgr. The mode_info_ptr points to
    mode-specific information provided by Call Mgr. A return value of
    SUCCESS indicates that the message was processed successfully,
    FAILURE indicates some problem occurred.  The clear flag indicates
    the call nolonger has data mode and should not be managed by Data
    Services.
  -------------------------------------------------------------------------*/
  boolean (*call_progress_handler)
                  (
                    cm_call_id_type             call_id,
                    cm_call_mode_info_s_type   *mode_info_ptr,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
                    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_CONNECT event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should set up the lower
    layers for data transfer, and optionally, set change_serial_mode to TRUE
    (and specify the additional serial parameters) if the serial mode needs
    to be changed. A return value of SUCCESS indicates that the data path is
    established, FAILURE indicates that the lower layers could not be set up
    i.e. the call should be released.  The delay_connect parameter should be
    set to FALSE if the call should be connected immediately, and TRUE if
    the connection should be delayed.  If connection is delayed, the 
    call_connected_complete_handler() will be called when connection is 
    completed. 
  -------------------------------------------------------------------------*/
  boolean (*call_connected_handler)
                  (
                    cm_call_id_type            call_id,
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    boolean                   *delay_connect,
                    cm_end_params_s_type      *end_params,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REL_IND event is 
    rx'ed from Call Mgr. The mode-specific handler should make a note that
    the radio link  interface has been released and no data transfer 
    can take place on the radio link interface. 
    A return value of SUCCESS indicates that mode specific handler
    has de-coupled the protocol layers from the radio link interface .
    FAILURE indicates that the lower layers could not be decoupled.
    i.e. the call should be released.
  -------------------------------------------------------------------------*/
  boolean (*rab_rel_handler)
                  (
                    cm_call_id_type           call_id,
                    cm_end_params_s_type     *end_params,
                    sys_modem_as_id_e_type    subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_IND event is 
    rx'ed from Call Mgr. The mode-specific handler should make a note that
    the radio link interface has been re-establised and ready
    for data transfer. The mode specific handler should bind the appropriate
    protocol layers to the radio link interface 
    A return value of SUCCESS indicates that mode specific handler
    has coupled the protocol layers to  the radio link interface.
    FAILURE indicates that the lower layers could not be coupled.
    i.e. the call should be released.
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_ind_handler)
                  (
                    cm_call_id_type            call_id,
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    cm_end_params_s_type      *end_params,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_REJ event is 
    rx'ed from Call Mgr. 
    A return value of SUCCESS indicates that mode specific handler
    has either started a call release procedure or ignored the event
    FAILURE indicates that the call cannot proceed further and the call should
    be released.      
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_rej_handler)
                  (
                    cm_call_id_type             call_id,
                    dsgwl_ps_call_info_u_type  *call_info
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_RAB_REESTAB_FAIL event is 
    rx'ed from Call Mgr. 
    A return value of SUCCESS indicates that mode specific handler
    has either started a call release procedure or ignored the event
    FAILURE indicates that the call cannot proceed further and the call should
    be released.      
  -------------------------------------------------------------------------*/
  boolean (*rab_re_estab_fail_handler)
                  (
                    cm_call_id_type          call_id,
                    cm_end_params_s_type    *end_params,
                    sys_modem_as_id_e_type   subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_SETUP_IND event is rx'ed
    from Call Mgr. The mode_info_ptr points to mode-specific information
    provided by Call Mgr. The mode-specific handler should validate the setup
    parameters proposed by the network, and specify the setup_res_params. A 
    return value of SUCCESS indicates that the setup was accepted, FAILURE
    indicates that the setup was rejected. The clear flag indicates the call
    nolonger has data mode and should not be managed by Data Services.
  -------------------------------------------------------------------------*/
  boolean (*setup_handler)
                  (
                    cm_call_id_type                call_id,
                    cm_call_mode_info_s_type      *mode_info_ptr,
                    cm_setup_res_params_s_type    *setup_res_params,
                    sys_sys_mode_e_type            call_mode,
                    boolean                       *delay_rsp,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
                    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                    sys_modem_as_id_e_type         subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr. The mode-specific handler should specify the ans_params and
    ring type, and optionally, set change_serial_mode to TRUE (and specify
    the additional serial parameters) if the serial mode needs to be changed.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.
  -------------------------------------------------------------------------*/
  ds3g_incom_e_type (*incoming_call_handler)
                  (
                    ds_cmd_type            *cmd_ptr,
                    cm_call_id_type         call_id,
                    cm_ans_params_s_type   *ans_params,
                    cm_end_params_s_type   *end_params,
                    sys_modem_as_id_e_type  subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the user or TE answers the call. The mode-
    specific handler should specify the ans_params and set the cause_params.
  -------------------------------------------------------------------------*/
  void (*user_answer_handler)
                  (
                    cm_call_id_type        call_id,
                    cm_ans_params_s_type  *ans_params,
                    ie_cm_cc_cause_s_type *cause_params,
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the user or TE ends the call. The mode-
    specific handler may attempt to shut down lower layer protocols.  If
    the handler would like to delay the disconnect, the delay_disconnect
    paramter should be set to TRUE.  In this case the end_params data will
    be ignored.  If delaying the disconnect is not desired, delay_disconnect
    must be set to FALSE, and the end_params data must be filled in.
  -------------------------------------------------------------------------*/
  void (*user_end_handler)
                  (
                    cm_call_id_type        call_id,
                    cm_end_params_s_type  *end_params,
                    boolean               *delay_disconnect,
                    void                  *user_info_ptr,
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_COMPLETE_LL_CONNECT_CMD event is 
    rx'ed from mode-specific handlers if delay_connect was set to TRUE when
    the call_connected_handler() was called. The mode-specific handler should 
    set up the lower layers for data transfer, and optionally, set 
    change_serial_mode to TRUE
    (and specify the additional serial parameters) if the serial mode needs
    to be changed. A return value of SUCCESS indicates that the data path is
    established, FAILURE indicates that the lower layers could not be set up
    i.e. the call should be released.  
  -------------------------------------------------------------------------*/
  boolean (*complete_ll_connect_handler)
                  (
                    cm_call_id_type         call_id,
                    cm_end_params_s_type   *end_params,
                    sys_modem_as_id_e_type  subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_COMPLETE_LL_DISCONNECT_CMD event is 
    rx'ed from lower layers if delay_disconnect was set to TRUE when
    the user_end_handler() was called.  The handler must fill in the 
    end_params data prior to returning.
  -------------------------------------------------------------------------*/
  void (*complete_ll_disconnect_handler)
                                   (
                                     cm_call_id_type           call_id,
                                     cm_end_params_s_type     *end_params,
                                     sys_modem_as_id_e_type    subs_id
                                   );
  
  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_END event is rx'ed from
    Call Mgr. The mode-specific handler should clean up all call related
    state and shut down protocol layers.  
    
    The cmd_ptr points to the command received by DS from CM.  Contains
    details about why the call ended.  The call_was_connected paramter
    indicates to the mode-specific handler whether or not the call was
    in the connected state when ended.
  -------------------------------------------------------------------------*/
  void (*call_ended_handler)
                  (
                    ds_cmd_type             *cmd_ptr,
                    boolean                  call_was_connected,
                    sys_modem_as_id_e_type   subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used when a mode-specific handler has an origination 
    in progress when an incoming page of the same type appears.  Mode-spec
    handler must end originating call.  It can accept or reject or do 
    nothing with incoming call (normal incoming call processing).
    
    There are two end_params.  The first is for the originating call, which
    is always ended.  The second is for the incoming call.  That call may
    also be ended (but is not required to be ended).
  -------------------------------------------------------------------------*/
  ds3g_incom_e_type (*switch_call_handler)
                        (
                          ds_cmd_type            *cmd_ptr,
                          cm_call_id_type         inc_call_id,
                          cm_call_id_type         exist_call_id,
                          cm_ans_params_s_type   *ans_params,
                          cm_end_params_s_type   *end_params_orig,
                          cm_end_params_s_type   *end_params_inc,
                          sys_modem_as_id_e_type  subs_id
                        );

  /*-------------------------------------------------------------------------
    This handler is used when there is an incoming call which will override
    an originating call.  This tells the mode-speci handler that their
    call should be ended immediately.
    
    NOTE:  the mode-specific handler will not get a call_ended() after this.
    This is the only indication the mode-specific handler will get that
    their call has been torn down.
    
    A return of TRUE indicates that there was a call to be torn down.
    FALSE indicates that the mode-spec handler had no active/originating
    calls.
  -------------------------------------------------------------------------*/
  boolean (*force_call_down_handler)
                  (
                    cm_call_id_type         exist_call_id,
                    cm_end_params_s_type   *end_params,
                    sys_modem_as_id_e_type  subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked from the function 
    ds_get_current_session_data_counters(), which is periodically invoked by
    UI during a data call. The mode-specific handler should return by 
    reference the total count of bytes transmitted and received at RLC for 
    the current call. The total byte count is accumulative for the call. If 
    RAB is re-established after PDP context is preserved during the call, the
    total byte count includes bytes transmitted and received after RAB is 
    re-established. The return value indicates if the byte count is valid.
  -------------------------------------------------------------------------*/
  boolean (*get_data_counters_handler)
                  (
                    cm_call_id_type    call_id,
                    uint32            *rx_from_network_byte_cnt,
                    uint32            *tx_to_network_byte_cnt
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a service change event with
    suspend = TRUE. The mode-specific handler may attempt to shut down  
    connection to the old RAT. 
    The call id is not passed into this function.
    The mode-specific handler should do the required processing for all
    active calls.
  -------------------------------------------------------------------------*/
  void (*data_suspend_ind_handler)
                  (
                    sys_modem_as_id_e_type  subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a service change event with
    suspend = FALSE. The mode-specific handler may attempt to resume
    connection to the new RAT.  
    The call id is not passed into this function.
    The mode-specific handler should do the required processing for all
    active calls.
  -------------------------------------------------------------------------*/
  void (*data_resume_ind_handler)
                  (
                    sys_modem_as_id_e_type  subs_id
                  );

  /*-------------------------------------------------------------------------
   Called whenever the system mode changes due to mode preference
    to one of the modes that DS tracks (1x, GSM, WCDMA).
  -------------------------------------------------------------------------*/
  void (*mode_pref_change_handler)
                  (
                    sys_modem_as_id_e_type     subs_id,
                    sys_sys_mode_e_type        new_mode,
                    sys_sys_mode_e_type        old_mode
                  );

  /*-------------------------------------------------------------------------
     Called whenever the system mode changes to one of the modes that DS
     tracks (1x, GSM, WCDMA).
  -------------------------------------------------------------------------*/
  void (*sys_change_handler)
                  (
                    sys_modem_as_id_e_type     subs_id,
                    sys_sys_mode_e_type        new_mode,
                    sys_sys_mode_e_type        old_mode
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a context modification event.
    Happens for network initiated context modification.
    For an embedded call, notify app about the modified context parameters
  -------------------------------------------------------------------------*/
  void (*context_mod_ind_handler)
                  (
                    cm_call_id_type             call_id,
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a NW modification REQ event.
   -------------------------------------------------------------------------*/
  void (*nw_modify_req_handler )
                  (
                    cm_call_id_type                call_id,
                    ds_cmd_type                   *cmd_ptr,
                    cm_pdp_act_res_params_s_type  *act_res_params_ptr,
                    sys_modem_as_id_e_type         subs_id
                  );

 /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a context modification CONF event.
    For an embedded call, notify app about the modified context parameters
  -------------------------------------------------------------------------*/
  void (*context_modify_cnf_handler)
                  (
                    cm_call_id_type             call_id,
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a context modification reject event.
    For an embedded call, notify app about the modified context parameters
  -------------------------------------------------------------------------*/
  void (*context_modify_rej_handler)
                  (
                    cm_call_id_type             call_id,
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a context promottion event.
    Happens for R99 to R97 handover and networkl initiated primary
    context release
  -------------------------------------------------------------------------*/
  void (*context_prom_ind_handler)
                  (
                    cm_call_id_type            call_id,
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked when CM sends a ss event indicating that it 
    failed to acquire service and hence could not get PS data out of dormancy.
  -------------------------------------------------------------------------*/
  void (*ps_data_fail_ind_handler)
                  (
                    sys_modem_as_id_e_type    subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is invoked for any generic indication that needs to be 
    passed to upper layers. Currently used for dormancy status for
    applications to determine if they can attempt fast dormancy successfully
  -------------------------------------------------------------------------*/
  void (*ps_data_generic_ind_handler)
                  (
                    ds3g_call_info_cmd_type  *call_info_ptr,
                    sys_modem_as_id_e_type    subs_id
                  );

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

  /*-------------------------------------------------------------------------
    This handler is invoked when DS MGR receive ISIM refresh voting request
    from MMGSDI. It is used to check to see how DS should vote to allow ISIM
    refresh to proceed or not.
  -------------------------------------------------------------------------*/
  boolean (*isim_refresh_voting_handler)
                  (
                    sys_modem_as_id_e_type      subs_id
                  );

#ifdef FEATURE_DATA_LTE
  /*-------------------------------------------------------------------------
    This handler is invoked when DS MGR receives CM_CALL_EVENT_ACT_BEARER_IND 
    from CM. Mode handler processes the indication by allocating the iface
    and registering with pdcp. MH sends cm_mm_call_cmd_act_bearer_rsp back
    to CM
  -------------------------------------------------------------------------*/
  boolean (*bearer_activate_handler)
                  (
                    cm_call_id_type            call_id,
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    cm_act_bearer_rsp_s_type  *cm_act_res_p,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the incoming attach request from CM.
  -------------------------------------------------------------------------*/
  void (*get_pdn_conn_ind_hdlr)
                  (
                    ds3g_eps_attach_sm_info_type  *attach_info_p,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the PDN Connectivity Reject Indication
  -------------------------------------------------------------------------*/
  void (*pdn_conn_rej_ind_hdlr)
                  (
                    ds3g_call_info_cmd_type    *call_info_p,
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the PDN Connectivity Fail Indication
  -------------------------------------------------------------------------*/
  void (*pdn_conn_fail_ind_hdlr)
                  (
                    ds3g_call_info_cmd_type    *call_info_p,
                    sys_modem_as_id_e_type      subs_id
                  );

  void (*pdn_conn_req_err_hdlr)
                  (
                    ds_cmd_type                *cmd_ptr,
                    sys_modem_as_id_e_type      subs_id
                  );
  /*-------------------------------------------------------------------------
    This handler is used to handle the Bearer allocation rejected Indication
  -------------------------------------------------------------------------*/
  void (*bearer_alloc_rej_ind_hdlr)
                  (
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the Bearer allocation failed Indication
  -------------------------------------------------------------------------*/
  void (*bearer_alloc_fail_ind_hdlr)
                  (
                    cm_call_mode_info_s_type  *mode_info_ptr,
                    sys_modem_as_id_e_type     subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the Bearer modification reject Indication
  -------------------------------------------------------------------------*/
  void (*bearer_mod_rej_ind_hdlr)
                  (
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This handler is used to handle the Bearer modification Indication
  -------------------------------------------------------------------------*/
  boolean (*bearer_mod_ind_hdlr)
                  (
                    cm_call_mode_info_s_type   *mode_info_ptr,
                    cm_call_id_type             call_id,
                    sys_modem_as_id_e_type      subs_id
                  );

  /*-------------------------------------------------------------------------
    This is the extended orig call handler which is very similar to the 
    former. It has provision to specify LTE params and can be extended to
    any new mode handler as well.
    
    User must also fill out the SRV type of his call.
  -------------------------------------------------------------------------*/
  boolean (*originate_call_handler_ext)
                  (
                    const byte                     *dial_string,
                    uint8                           dial_string_len,
                    ds3g_orig_params_s              orig_params,
                    cm_srv_type_e_type             *cm_srv_type,
                    void                           *mode_spec_info_ptr,
                    sys_modem_as_id_e_type          subs_id
                  );
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_LTE

  /*-------------------------------------------------------------------------
    This handler is used to handle the context transfer Indication during
    LTE-GW interRAT transition.
  -------------------------------------------------------------------------*/
  void (*cntxt_transfer_ind_hdlr)
                  (
                    sys_sys_mode_e_type         target_rat,
                    sys_modem_as_id_e_type      subs_id
                  );

#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT  
  /*-------------------------------------------------------------------------
    This handler is invoked when the CM_CALL_EVENT_INCOMING_PAGE event is
    rx'ed from Call Mgr. The mode specific handler should decide whether
    to accept this call or reject it, and DS3G will notify CM accordingly.
  -------------------------------------------------------------------------*/
  ds3g_incom_e_type (*incoming_page_handler)
                  (
                    ds_cmd_type                *cmd_ptr,
                    sys_modem_as_id_e_type      subs_id
                  );
#endif /* FEATURE_DATA_S033_INCOMING_PAGE_HANDLER_SUPPORT */

  /*-------------------------------------------------------------------------
    This handler is used to handle the BSR flag reset from CMIF
  -------------------------------------------------------------------------*/
  void (*bsr_reset_hdlr)
                  (
                    sys_modem_as_id_e_type  subs_id
                  );

  void (*plmn_change_hdlr)
                  (
                    sys_plmn_id_s_type      old_plmn_id,
                    sys_modem_as_id_e_type  subs_id
                  );

  /*------------------------------------------------------------------------- 
    Handler to refresh the actual throughput for all active calls
  -------------------------------------------------------------------------*/
  void(*refresh_actual_throughput_hdlr)
                  (
                    ds3gsubsmgr_subs_id_e_type ds3g_subs_id,
                    uint16 timer_expiry_count
                  );

  /*------------------------------------------------------------------------- 
    Handler to refresh the actual throughput for all active calls
  -------------------------------------------------------------------------*/
  void(*send_downlink_throughput_estimation_indicaiton)
                  (
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    Handler to update modem power statistics configuration
    -------------------------------------------------------------------------*/
  void(*config_modem_power_stats)(sys_modem_as_id_e_type subs_id,
                                        boolean enable_modem_power_stats
                                       );

  /*------------------------------------------------------------------------- 
    Handler query for modem power statistics
    -------------------------------------------------------------------------*/
  void(*get_modem_power_stats)(sys_modem_as_id_e_type subs_id,
                                uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX]        
                                       );

  /*------------------------------------------------------------------------- 
    Handler to request reporting modem power stats to LTE (asynchronously)
    -------------------------------------------------------------------------*/
  void(*request_modem_power_stats)(sys_modem_as_id_e_type subs_id);

  /*------------------------------------------------------------------------- 
    Handler to inform lower layer throughput setup for all active calls
  -------------------------------------------------------------------------*/
  void(*prepare_downlink_estimated_throughput_report)
                  (
                    sys_modem_as_id_e_type subs_id,
                    uint32 t_report
                  );


  /*------------------------------------------------------------------------- 
    Handler to get t_accumuate from the respective rat
  -------------------------------------------------------------------------*/
  void(*get_downlink_throughput_t_accumulate)
                  (
                    sys_modem_as_id_e_type subs_id,
                    uint32 *t_accumulate
                  );



  /*------------------------------------------------------------------------- 
    Handler to report Throughput Indication based on the current sys mode
  -------------------------------------------------------------------------*/
  void(*report_throughput_info_hdlr)
                  (
                    ds3gsubsmgr_subs_id_e_type ds3g_subs_id
                  );

  /*-------------------------------------------------------------------------
    Handler to get t_accumuate from the respective rat
  -------------------------------------------------------------------------*/
  void(*set_downlink_throughput_running_t_accumulate)
                  (
                    sys_modem_as_id_e_type subs_id,
                    uint32 running_t_accumulate
                  );

  /*-------------------------------------------------------------------------
    Handler to report coex data flow enable
  -------------------------------------------------------------------------*/
  void(*coex_flow_enable_hdlr)
                  (
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    Handler to report coex data flow disable
  -------------------------------------------------------------------------*/
  void(*coex_flow_disable_hdlr)
                  (
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    Handler to get total UL & DL WM count for all active bearers
  -------------------------------------------------------------------------*/
  uint64(*get_total_uldl_wm_cnt_hdlr)
                  (
                    sys_modem_as_id_e_type subs_id
                  );

  /*-------------------------------------------------------------------------
    Handler to bring down all phys links
  -------------------------------------------------------------------------*/
  void(*bringdown_phys_link_hdlr)
                  (
                    sys_modem_as_id_e_type subs_id
                  );

}ds3g_hdlr_func_tbl_type;

/*---------------------------------------------------------------------------
  Structure is used when calling function in 3G DSMGR.  These structures
  are then passed right back to this interface.  Contains info that is
  important in configuring the response to orig or end a call.  Specific
  to IS-707 data
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                drs_bit;          /* contains DRS bit value       */
  boolean                use_hdown_timer;  /* use the holddown timer?      */
  uint32                 pkt_instance;     /* pkt instance to work on      */
  sys_sys_mode_e_type    new_network;      /* new system (1X or HDR) the AT
                                              is on - needed for data
                                              session handoffs             */
  ps_iface_type         *iface_ptr;        /* iface this is                */
  boolean                force_cdma_only;  /* flag to force the mode to be
                                              CDMA specific                */
  boolean                inc_call;         /* is this an incoming call     */
  ps_phys_link_type     *phys_link_ptr;    /* Helps pass current physlink  */
  boolean                epzid;            /* is EPZID changed             */
} ds3g_pkt_call_info_type;

/*---------------------------------------------------------------------------
  Structure to store various PLMN ID Info
---------------------------------------------------------------------------*/
typedef struct
{
  sys_plmn_list_s_type      eplmn_list;
  reg_sim_plmn_list_s_type  ehplmn_list;
  sys_plmn_id_s_type        rplmn;
  sys_plmn_id_s_type        hplmn;
}ds3g_plmn_id_info_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_INIT

DESCRIPTION   This function performs power-up initialization of 3G Dsmgr.
              This includes initialization of some state variables and
              registration with Call Manager for handling of incoming/outgoing
              data calls.
              This function must be called once at data services task startup.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_init( void );

/*===========================================================================
FUNCTION      DS3G_PROCESS_INIT_COMPLETE_CMD

DESCRIPTION   This function is called to process the init complete command.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_init_complete_cmd(void);

/*===========================================================================
FUNCTION      DS3G_PROCESS_LOW_LATENCY_COMPLETE_CMD

DESCRIPTION   This function is called to process the low latency command.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds3g_process_low_latency_cmd(const ds_cmd_type  *cmd_ptr);
/*===========================================================================
FUNCTION      DS3G_INIT_CRIT_SECT

DESCRIPTION   Called once at mobile power-up.  Initializes all DS3G 
              critical sections 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_init_crit_sect(void);

/*===========================================================================
FUNCTION      DS3G_CLIENT_REG

DESCRIPTION   This function registers ds3gmgr as client with external modules 
              like CM during task init

               This function must be called once at data services task startup.
               Since this function registers with external modules that might
               start interacting with DS immediately this function should be
               called at the very end of the task init

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_client_reg(void);

/*===========================================================================
FUNCTION      DS3G_PROCESS_CMDS

DESCRIPTION   This function processes 3G Dsmgr commands. A command processing
              function is called based on the type of command received.

              This function should be called by the top-level task dispatcher
              whenever any 3G Dsmgr command is received.

DEPENDENCIES  3G Dsmgr should have been initialized prior to calling this 
              function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_cmds
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3G_REGISTER_MODE_SP_HANDLER

DESCRIPTION   This function registers a mode-specific handler function table,
              for the specified mode and call type.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  If a mode-specific handler function table was previously
              registered for the specified mode and call type, it will be
              over-written.
===========================================================================*/
void ds3g_register_mode_sp_handler
(
  sys_sys_mode_e_type             mode,        /* Network mode             */
  ds_call_type_e_type             call_type,   /* Call type-Circuit or pkt */
  ds3g_hdlr_func_tbl_type        *hdlr_tbl_ptr /* Handler function table   */
);

/*===========================================================================
FUNCTION      DS3G_SEND_DOWNLINK_THROUGHPUT_ESTIMATION_IND

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_send_downlink_throughput_estimation_ind
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3G_REFRESH_ACTUAL_THROUGHPUT

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput for all active calls for the current
              sample interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_refresh_actual_throughput
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id,
  uint16                     timer_expiry_count
);

/*===========================================================================
FUNCTION      DS3G_PREPARE_DOWNLINK_THROUGHPUT_ESTIMATION

DESCRIPTION   This function invokes the mode handlers to starting lower layer 
              for estimated downlink throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_prepare_downlink_throughput_estimation
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id
);
/*===========================================================================
FUNCTION      DS3G_SET_DOWNLINK_THROUGHPUT_RUNNING_T_ACCUMULATE

DESCRIPTION   This function invokes the mode handlers to starting lower layer 
              for estimated downlink throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_downlink_throughput_running_t_accumulate
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint32                      running_t_accumulate
);

/*===========================================================================
FUNCTION      DS3G_GET_DL_TPUT_T_ACCUMUATE_VALUE

DESCRIPTION   This function invokes the mode handlers to refresh/update 
              the actual throughput rate for all active calls for the
              current sampling interval

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_dl_tput_t_accumuate_value
(
  ds3gsubsmgr_subs_id_e_type  ds3g_subs_id,
  uint32*                      t_accumuate
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_NETWORK_MODE

DESCRIPTION   This function returns the current system that we are camped on.
              The current system is the one available on the preferred radio.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_network_mode(void);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_NETWORK_MODE_EX

DESCRIPTION   This function returns the current system that we are camped on
              for a SUBS ID.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type for the subs_id mentioned.
              If invalid subs_id, return SYS_SYS_MODE_NO_SRV.

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_network_mode_ex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PREFERRED_MODE

DESCRIPTION   This function returns the current preferred system.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_preferred_mode(void);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_PREFERRED_MODE_EX

DESCRIPTION   This function returns the current preferred system for a SUBS ID.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type for the subs_id mentioned.
              If invalid subs_id, return SYS_SYS_MODE_NO_SRV.

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_preferred_mode_ex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_CS_DOMAIN_MODE

DESCRIPTION   This utility function return radio which is CS domain registered.
  
DEPENDENCIES  None

RETURN VALUE  Returns mode which is CS domain registered

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_cs_domain_mode(void);

/*===========================================================================
FUNCTION      DS3G_GET_TARGET_RAT_EX

DESCRIPTION   This function returns the target rat on passed subs 
              (used during eHRPD - LTE reselection).

DEPENDENCIES  None

RETURN VALUE  NONE if in steady state 
              LTE if in transient state during reselection

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_target_rat_ex
(
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_TARGET_RAT

DESCRIPTION   This function returns the target rat 
              (used during eHRPD - LTE reselection)

DEPENDENCIES  None

RETURN VALUE  NONE if in steady state
              LTE if in transient state during reselection

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_target_rat(void);

/*===========================================================================
FUNCTION      DS3G_SET_TARGET_RAT_EX

DESCRIPTION   This function sets the target rat on passed sub 
              (used during eHRPD - LTE reselection)

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_target_rat_ex
(
  sys_sys_mode_e_type     trat,
  ds3geventmgr_attach_prof_info_type   *attach_prof_info,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SRV_STATUS_EX

DESCRIPTION   This function returns the current service status of passed sub 
              of the system that is indicated in the DS_CM_SS_SRV_CHG_CMD in
              the srv_status field.

DEPENDENCIES  None

RETURN VALUE  Returns uint32 which has current_srv_status maintained in ds3g

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_current_srv_status_ex
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SRV_STATUS

DESCRIPTION   This function returns the current service status of the system
              that is indicated in the DS_CM_SS_SRV_CHG_CMD in the srv_status
              field.

DEPENDENCIES  None

RETURN VALUE  Returns uint32 which has current_srv_status maintained in ds3g

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_current_srv_status(void);

/*===========================================================================
FUNCTION      DS3G_IS_SYS_AVAILABLE_EX

DESCRIPTION   This function returns boolean depending on whether the system is
              available or not on the passed subs.

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_sys_available_ex
(
  sys_sys_mode_e_type     sys,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_IS_SYS_AVAILABLE

DESCRIPTION   This function returns boolean depending on whether the system is
              available or not

DEPENDENCIES  None

RETURN VALUE  boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_sys_available
(
  sys_sys_mode_e_type sys
);

/*===========================================================================
FUNCTION      DS3G_GET_CALL_WAS_CONNECTED

DESCRIPTION   This function returns the was connected flag for the given call
              identifier.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr call_was_connected flag

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_call_was_connected
(
  cm_call_id_type    cm_call_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_SESSION_DATA_COUNTERS

DESCRIPTION   This function should be invoked by UI for displaying data call
              statistics. It returns by reference the total count of bytes
              transmitted and received at RLC for the current call:

              *rx_from_network_byte_cnt - Total bytes rxed for the call
              *tx_to_network_byte_cnt   - Total bytes txed for the call

  The total byte count is accumulative for the call. If RAB is
              re-established after PDP context is preserved during the call,
              the total byte count includes bytes transmitted and received
              after RAB is re-established.

DEPENDENCIES  None

RETURN VALUE  This function returns the data statistics type, indicating if 
              the information passed by reference, e.g., *rx_from_network_byte_cnt,
              is valid.

SIDE EFFECTS  None
===========================================================================*/
ds_stats_e_type ds3g_get_current_session_data_counters
(
  cm_call_id_type  call_id,                   /* Call ID */
  uint32          *rx_from_network_byte_cnt,  /* RX byte count for the call */
  uint32          *tx_to_network_byte_cnt     /* TX byte count for the call */
);

/*===========================================================================
FUNCTION      DS3G_SET_CURRENT_OPRT_MODE_SRV_DOMAIN_EX

DESCRIPTION   This function sets the new oprt_mode_srv_domain on passed sub
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_current_oprt_mode_srv_domain_ex
(
  cm_srv_domain_pref_e_type  new_srv_domain,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_OPRT_MODE_SRV_DOMAIN_EX

DESCRIPTION   This function returns the current oprt_mode_srv_domain on 
              passed sub.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
cm_srv_domain_pref_e_type ds3g_get_current_oprt_mode_srv_domain_ex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_REGD_SRV_DOMAIN_EX

DESCRIPTION   This function returns the current regd_srv_domain on pass sub
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
sys_srv_domain_e_type ds3g_get_current_regd_srv_domain_ex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_REGD_SRV_DOMAIN

DESCRIPTION   This function returns the current regd_srv_domain
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
sys_srv_domain_e_type ds3g_get_current_regd_srv_domain(void);

/*===========================================================================
FUNCTION      DS3G_GET_CURRENT_CS_DOMAIN_MODE_FOR_SUBS_ID

DESCRIPTION   This function returns the current CS domain mode this
              subscription has camped on.
  
DEPENDENCIES  None

RETURN VALUE  Returns the CS Domain mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_current_cs_domain_mode_ex
(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3G_SET_CS_SUBS_ID

DESCRIPTION   This function sets the subs id in DS3GMGR for call origination.
              (Currently used only for CSD call origination)

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_cs_subs_id
(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3G_RESET_CS_SUBS_ID

DESCRIPTION   This function clears the subs id in DS3GMGR for call origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  This clears the cs subs id set by ds3g_set_cs_subs_id()
===========================================================================*/
void ds3g_reset_cs_subs_id(void);

/*===========================================================================
FUNCTION      DS3G_GET_CS_SUBS_ID

DESCRIPTION   This function get the subs id in DS3GMGR for call origination.
              (Currently used only for CSD call origination)

DEPENDENCIES  None

RETURN VALUE  Returns the current SUBS_ID set by CSD module

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_cs_subs_id(void);

/*===========================================================================
FUNCTION      DS3G_GET_PS_SUBS_ID

DESCRIPTION   This function gets the subs id for PS data call.

DEPENDENCIES  None

RETURN VALUE  Returns the subs_id on which PS data call can be made.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_ps_subs_id(void);

/*===========================================================================
FUNCTION      DS3G_SET_VOICE_SUBS_ID

DESCRIPTION   This function set the subs id in DS3GMGR for Voice subscription.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_voice_subs_id
(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3G_GET_VOICE_SUBS_ID

DESCRIPTION   This function gets the default voice subs id for VT data call.

DEPENDENCIES  None

RETURN VALUE  Returns the subs_id which is selected as default voice subs.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_as_id_e_type ds3g_get_voice_subs_id(void);

/*===========================================================================
FUNCTION      DS3G_GET_STANDBY_PREF

DESCRIPTION   Returns dual standby preference value stored in DS3G.

DEPENDENCIES  None

RETURN VALUE  Dual standby preference.

SIDE EFFECTS  None
===========================================================================*/
sys_modem_dual_standby_pref_e_type ds3g_get_standby_pref(void);

/*===========================================================================
FUNCTION       DS3G_GET_CM_CLIENT_ID

DESCRIPTION    Returns the value of client ID assigned by CM.

DEPENDENCIES   None

RETURN VALUE
  cm_client_id_type - Client ID assigned by CM

SIDE EFFECTS   None

===========================================================================*/
cm_client_id_type ds3g_get_cm_client_id(void);

/*===========================================================================
FUNCTION      DS3G_SET_CM_CLIENT_ID

DESCRIPTION   Sets client ID assigned by CM in DS3G maintained cache.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_cm_client_id
(
  cm_client_id_type  cm_client_id
);

/*===========================================================================
FUNCTION      DS3G_SET_LAST_CALL_TYPE

DESCRIPTION   Sets call type for the previous call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_last_call_type
(
  ds_call_type_e_type  last_call_type
);

/*===========================================================================
FUNCTION      DS3G_SET_LAST_CALL_MODE

DESCRIPTION   Sets operating mode for the previous call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_set_last_call_mode
(
  sys_sys_mode_e_type last_call_mode
);

/*===========================================================================
FUNCTION      DS3G_SV_MODE_OF_OPERATION

DESCRIPTION   Returns boolean - TRUE if SV mode of operation else FALSE

DEPENDENCIES  None

RETURN VALUE  TRUE: SV mode of operation
              FALSE: otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_sv_mode_of_operation(void);

/*===========================================================================
FUNCTION      DS3G_DSAT_INIT_SERVICE_MODE

DESCRIPTION   This function calls into dsat_init_service_mode depending on
              SV or NON-SV mode of operation.

DEPENDENCIES  None

RETURN VALUE  Returns the 3G Dsmgr network mode type

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dsat_init_service_mode
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GI_DSAT_UPDATE_SERVICE_MODE

DESCRIPTION   This function updates the dsat srv mode.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_dsat_update_service_mode
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      ds3g_bsr_is_in_progress

DESCRIPTION   Returns if a Better System Reselection is in progress.

DEPENDENCIES  None

RETURN VALUE  TRUE - BSR in in progress.
              FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_bsr_is_in_progress
(
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
FUNCTION      DS3G_CACHE_RADIO_INFO

DESCRIPTION   Updates the global radio info cache with the info provided by 
              CM in the ss event. This cache is updated everytime CM generates
              SS event for CM_SS_EVENT_SRV_CHANGED

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_cache_radio_info
(
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr
);

/*===========================================================================
FUNCTION      DS3G_IS_HDR_IN_SRVC

DESCRIPTION   Checks the global radio info cache to see if HDR is in service

DEPENDENCIES  None

RETURN VALUE  TRUE - If HDR is in service
              FALSE - Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_hdr_in_srvc(void);

/*===========================================================================
FUNCTION      DS3G_GET_LTE_SRV_STATUS

DESCRIPTION   Checks the global radio info cache and returns the srv status 
              if LTE is present in any of the stack. Else returns no serv by
              default.
  
DEPENDENCIES  None

RETURN VALUE  Limited/Full Serice
              No Service (If LTE not available on any stack)

SIDE EFFECTS  None
===========================================================================*/
sys_srv_status_e_type ds3g_get_lte_srv_status(void);

/*===========================================================================
FUNCTION      DS3G_GET_CM_CLIENT_INIT_STATUS

DESCRIPTION   This fucntion checks if CM client registration has finished.

DEPENDENCIES  None

RETURN VALUE  TRUE: CM client registration done
              FALSE: otherwise

SIDE EFFECTS  None 
===========================================================================*/
boolean ds3g_get_cm_client_init_status(void);
/*===========================================================================
FUNCTION      DS3G_REPORT_THROUGHPUT_INFO

DESCRIPTION   This function invokes the mode handler to send the throughput 
              information event to PS 
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_report_throughput_info
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
);

/*===========================================================================
FUNCTION      DS3G_RESOLVE_SUBS_ID

DESCRIPTION   This is an external API function.
              The clients call this to resolve a DS internal subs id to CM
              subs id.

DEPENDENCIES  None

RETURN VALUE  Status of the resolve and the resolved subs id

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_resolve_subs_id
(
  ds_sys_subscription_enum_type  ds_subs_id,
  sys_modem_as_id_e_type        *ret_subs_id_ptr
);

/*===========================================================================
FUNCTION      DS3G_IS_HDR_HYBRID_STACK_ACTIVE

DESCRIPTION   This function returns the status of HDR hybrid stack.

DEPENDENCIES  None

RETURN VALUE  TRUE:  HDR Hybrid stack active
              FALSE: HDR Hybrid stack inactive

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_hdr_hybrid_stack_active
(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3G_IS_LTE_HYBRID_STACK_ACTIVE

DESCRIPTION   This function returns the status of GW hybrid stack.

DEPENDENCIES  None

RETURN VALUE  TRUE:  GW Hybrid stack active
              FALSE: GW Hybrid stack inactive

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_lte_hybrid_stack_active
(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================
FUNCTION      DS3G_GET_PLMN_ID_INFO

DESCRIPTION   This function returns the various PLMN IDs.
              Note: memory has to be allocated by user.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_plmn_id_info
(
  ds_sys_subscription_enum_type   ds_subs_id,
  ds3g_plmn_id_info_type         *plmn_id_info_p
);

/*===========================================================================
FUNCTION      DS3G_CHECK_IF_RPLMN_IS_VPLMN

DESCRIPTION   Utility function to check if passed RPLMN is VPLMN or not.

DEPENDENCIES  None

RETURN VALUE  TRUE:  Passed RPLMN is VPLMN.
              FALSE: Passed RPLMN is not VPLMN.
                i.e. Passed RPLM is neither in EHPLMN list nor the HPLMN

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_check_if_rplmn_is_vplmn
(
  ds3g_plmn_id_info_type  *plmn_id_info_p
);

/*===========================================================================
FUNCTION DS3GMGR_SEND_TRAT_IND

DESCRIPTION
  This function will extract the Profile Context for a given attach profile
  and sent it along with TRAT indication to DS3G

PARAMETERS
  profile_id: Attach Profile ID
  subs_id: Current active subscription

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If indication was sent successfully
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gmgr_send_trat_ind
(
  sys_sys_mode_e_type       trat,
  uint16                    profile_id,
  ds_umts_pdp_profile_type *prof_context,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_ROAMING_STATUS

DESCRIPTION   This function is a wrapper to call the 3GPP get function. 
              This tells whether the UE is Roaming or not.
  
PARAMETERS    Subscription Id 
    
DEPENDENCIES  None.
  
RETURN VALUE  ds3geventmgr_roaming_status_info: is_roaming - in roam or not 
                                                sys_mode - current sys mode 
                                                              
   
SIDE EFFECTS  None.
  
===========================================================================*/
ds3geventmgr_roaming_status_info_type ds3g_get_roaming_status
(
  sys_modem_as_id_e_type subs_id
);

#endif /* DS3GMGR_H */
