#ifndef HDRPAC_H
#define HDRPAC_H

/*=========================================================================

      H D R  P A C K E T  A P P L I C A T I O N  C O N T R O L L E R

DESCRIPTION
  This file contains all functions and utilities exported by the Packet
  Application Controller.

Copyright (c) 2001-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpac.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/12   ash     Use new HDR API to inform LTE indication.
11/18/11   vpk     HDR SU API cleanup
09/16/11   dvk     Fixed Compiler warnings
09/08/11   ash     Added support for eHRPD to HRPD fallback.
06/20/11   ps      Moved hdrpac_get_sn_ppp_status_from_nv to hdrpac_api.h
06/03/11   mg      Added hdrpac_set_ehrpd_apns_are_valid().
05/26/10   gc      Added code changes to include support for RMAC4(RevB)
                   subtype for QOS call.
04/09/09   ls      Moved hdrpac_get_rtcmac_flow() to hdrpac_mdm( modem side )
08/21/08   yz      Added hdrpac_current_sub3_rmac_type()
1/12/07    sju     Added hdrpac_get_current_max_configured_sci()
07/24/06   mpa     Moved hdrpac_get_rtcmac_flow() from hdrpac_mdm.h
09/09/05   etv     Added support for AT-init AUP of rate params.
05/17/05   vas     Changes to support PCP Rev interaces
03/07/05   pba     Moved all the PACC related processing to hdrpacc.h
03/07/05   pba     Separated handling of In-Use and In-Config messages for PAC.
01/18/05   ifk     Added function prototype for hdrpac_set_an_auth_status()
09/20/04   mpa     Added hdrpac_close_connection().
09/09/04   mpa     Added QOS support
03/10/04    gr     Added hdrpac_get_sn_ppp_status_from_nv
03/09/04    gr     Added HDRPAC_CLEAR_SN_PPP_STATUS_CMD
11/14/03    vr     Added HDRPAC_SESSION_CLOSED_CMD
10/21/03   vas     Added HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD
10/13/03   vas     Added new commands for hdrpacdsif
06/18/03   vas     Added command PAC_INIT
02/03/03   ht      Based on asw/COMMON/vcs/hdrpac.h_v   1.10
                   Add changes to support HDR 3GData call.
03/17/02   vas     Removed function hdrpac_set_an_auth_status()
03/01/02   hcg     Added hdrpac_get_an_auth_info_from_nv() to read 
                   authentication variables from NV.
02/26/02   vas     Added hdrpac_app_flow_is_enabled
02/05/02   vas     Changed AN authentication status from boolean to enum. 
12/01/01   vas     Added support to specify the app subtype for the stream to
                   be flow controlled. Added function to set the HDR AN
                   Authentication status.
10/31/01   vas     Added hdrpac_update_rlp_reg_srvc() & structs	for commands
09/14/01   vas     Added hdrpac_flow_is_enabled().
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/28/01   vas     Removed hdrpac_get_id_app_bnd_to_service_nw()
08/17/01   vas     Added hdrpac_get_id_app_bnd_to_service_nw()
07/30/01   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "dsrlp_v.h"
#include "ps_iface_defs.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS

This section contains definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

typedef enum
{
  HDRPAC_INIT_CMD                    = 0,
  HDRPAC_SN_IFACE_UP_CMD             = 1,
  HDRPAC_SN_IFACE_ROUTEABLE_CMD      = 2,
  HDRPAC_SN_IFACE_DOWN_CMD           = 3,
  HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD = 4,
  HDRPAC_SESSION_CLOSED_CMD          = 5,
  HDRPAC_CLEAR_SN_PPP_STATUS_CMD     = 6,
  HDRPAC_SN_IFACE_COMING_UP_CMD      = 7
} hdrpac_cmd_name_enum_type;

typedef struct
{
   hdrpac_cmd_name_enum_type name;        /* Name of the command           */
   hdrcom_hai_protocol_name_enum_type sender; /* protocol giving cmd, for debug */
} hdrpac_cmd_type;
 
/* EVDO Page Monitor Period structure */
typedef struct
{
  uint8         slot_cycle_index;
  boolean       force_long_sleep;
} hdrpac_page_monitor_period_info_type;
 
 /*---------------------------------------------------------------------------
  CB function pointer used to inform the PS iface events.
---------------------------------------------------------------------------*/
typedef void (hdrpac_iface_events_cb) (ps_iface_event_enum_type evt, void *event_data_ptr); 

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION HDRPAC_CURRENT_RMAC_SUB_TYPE

DESCRIPTION
  This is utility function to return the current RMAC subtype
DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  uint16 - Current RMAC subtype

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hdrpac_current_rmac_sub_type(void);

/*===========================================================================
FUNCTION  HDRPAC_POWERUP_INIT

DESCRIPTION
  This function does the initializations for the PAC on power up.

DEPENDENCIES
  Needs to be called after hdrstream initialization.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpac_powerup_init( void );

/*===========================================================================
FUNCTION  HDRPAC_INIT

DESCRIPTION
  This function does the initializations for the PAC when entering HDR mode.

DEPENDENCIES
  Needs to be called after hdrstream_init().

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrpac_init( void );

/*===========================================================================
FUNCTION HDRPAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to the PAC.

DEPENDENCIES
  Indications that are processed in the DS task context

PARAMETERS
  ind_name     - Name of the indication for OVHD to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_process_ind
(
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type *ind_data_ptr
);

/*===========================================================================

FUNCTION HDRPAC_PROCESS_CMD

DESCRIPTION
  This function processes commands for the PAC

DEPENDENCIES
  None
  
PARAMETERS
  pac_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  DS task context
===========================================================================*/
extern void hdrpac_process_cmd
(
  hdrpac_cmd_type *pac_cmd_ptr 
);

/*===========================================================================
FUNCTION HDRPAC_CLOSE_CONNECTION

DESCRIPTION
  This function closes a connection through the hdr protocol (ALMP). 

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_close_connection( void );

/*===========================================================================
FUNCTION HDRPAC_SET_AN_AUTH_STATUS

DESCRIPTION
  This function informs the PAC whether CHAP on the AN stream succeded or
  failed. This function is registered with PPP to be called on the CHAP
  success/failure.

DEPENDENCIES
  None

PARAMETERS
  auth_succeeded - True if CHAP authentication succeeded, false if it failed

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_set_an_auth_status
(
  boolean auth_succeeded
);


/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_START_HIGH_PRIORITY_TRAFFIC_MODE

DESCRIPTION
  This function is called from the IOCTL handler when the HDR_TRAFFIC_MODE
  IOCTL is called to indicate high priority HDR traffic handling is needed.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_start_high_priority_traffic_mode( void );


/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_STOP_HIGH_PRIORITY_TRAFFIC_MODE

DESCRIPTION
  This function is called from the IOCTL handler when the HDR_TRAFFIC_MODE
  IOCTL is called to indicate high priority HDR traffic handling is no longer
  needed.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_stop_high_priority_traffic_mode( void );

/*===========================================================================
FUNCTION HDRPAC_ENABLE_1X_HANDDOWN_MODE

DESCRIPTION
  This function is called from the IOCTL handler when the handdown to 1x enable
  IOCTL is called.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_enable_1x_handdown_mode( void );

/*===========================================================================
FUNCTION HDRPAC_DISABLE_1X_HANDDOWN_MODE

DESCRIPTION
  This function is called from the IOCTL handler when the handdown to 1x disable
  IOCTL is called.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_disable_1x_handdown_mode( void );

/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_ENABLE_VT_RATE_INERTIA

DESCRIPTION
  This interface is used to enable Video Telephony Rate Inertia. A GAUP update
  for VT Rate parameters is sent only on certain scenarios explained below.

  It doesn't make sense to send an AttributeUpdateRequest for VT RateParams 
  again when we have one already in the pipeline. So we send an update only 
  when the InConfig Mode is Normal.

  ------------------------------------
  | Current | In Config | Send GAUP? |
  ------------------------------------
  | VT      | Normal    | Yes        |
  ------------------------------------
  | VT      | VT        | No         |
  ------------------------------------
  | Normal  | Normal    | Yes        |
  ------------------------------------
  | Normal  | VT        | No         |
  ------------------------------------

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  E_SUCCESS       - If the GAUP Update for VT RateParams was sent out 
                    successfully or if the InConfig RP Mode was already VT.
  E_NOT_SUPPORTED - If the current subtype of RMAC was not ST-1.
  E_NOT_AVAILABLE - If we don't have a free Rsp timer to send a GAUP update.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern hdrcom_errno_enum_type hdrpac_enable_vt_rate_inertia( void );


/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_DISABLE_VT_RATE_INERTIA

DESCRIPTION
  This interface is used to disable Video Telephony Rate Inertia. A GAUP update
  for Normal Rate parameters is sent only on certain scenarios explained below.

  It doesn't make sense to send an AttributeUpdateRequest for Normal RateParams
  again when we have one already in the pipeline. So we send an update only 
  when the InConfig Mode is VT.

  ------------------------------------
  | Current | In Config | Send GAUP? |
  ------------------------------------
  | VT      | Normal    | No         |
  ------------------------------------
  | VT      | VT        | Yes        |
  ------------------------------------
  | Normal  | Normal    | No         |
  ------------------------------------
  | Normal  | VT        | Yes        |
  ------------------------------------

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  E_SUCCESS       - If the GAUP Update for VT RateParams was sent out 
                    successfully or if the InConfig RP Mode was already VT.
  E_NOT_SUPPORTED - If the current subtype of RMAC was not ST-1.
  E_NOT_AVAILABLE - If we don't have a free Rsp timer to send a GAUP update.
   
SIDE EFFECTS
  None.

===========================================================================*/
extern hdrcom_errno_enum_type hdrpac_disable_vt_rate_inertia( void );


/*===========================================================================
FUNCTION  HDRPAC_GET_APP_SUBTYPE_FOR_STREAM

DESCRIPTION 
   This function returns the appsubtype for a given stream id

DEPENDENCIES
  Needs to be called after hdrstream initialization.
  Init for DS.

PARAMETERS
  Stream Identifier

RETURN VALUE
  App subtype from the hdrpac structure

SIDE EFFECTS
  None

===========================================================================*/

extern hdrcom_hai_app_subtype_enum_type hdrpac_get_app_subtype_for_stream
(
    hdrcom_hai_stream_enum_type stream_id 
);

/*===========================================================================
FUNCTION HDRPAC_GET_STREAM_FOR_APP_SUBTYPE

DESCRIPTION
  This function gets stream that the particular application subtype has been
  negotiated on. If the app subtype is not found, it returns HDRHAI_STREAM0
  This should be considered as meaning the app is not bound to a stream.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrcom_hai_stream_enum_type hdrpac_get_stream_for_app_subtype
(
  hdrcom_hai_app_subtype_enum_type app_subtype
);

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION HDRPAC_GET_SUBNETID

DESCRIPTION
  This function gets the current subnet id by querying HDR CP. This is needed 
  only in s101 tunnel mode since CM is not aware of the subnet id. DS can 
  act as a pass through between HDR CP and CM to propagate the subnet id. 

DEPENDENCIES
  None

PARAMETERS
  In: hdraddr_type: This will return the subnet id.

RETURN VALUE
  E_SUCCESS - if operation is successful
  E_NO_DATA - If the color code has changed and we have not been received yet
              the new subnet from the SP message yet.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrpac_get_subnetid
(
  hdraddr_type subnet
);
#endif /*FEATURE_DATA_OPTHO*/
/*===========================================================================
  FUNCTION HDRPAC_CHANGE_EIDLE_SLOTTED_MODE
 
  DESCRIPTION
  This interface is used to set the Enhanced Idle Mode slot cycle index. A GAUP 
  update is sent with the suggested slot cycle value.
 
  DEPENDENCIES
  None.
 
  PARAMETERS
  slotted_mode_option - the desired option (e.g., NORMAL, SHORT, or SHORTER)
   
  RETURN VALUE
  E_SUCCESS       - If the GAUP Update for slot cycle update was sent out 
  successfully.
  E_NOT_SUPPORTED - If EIDLE slot cycle update is not supported.
  E_NOT_AVAILABLE - If EIDLE slot cycle update is not available.
    
  SIDE EFFECTS
  None.
 
===========================================================================*/
hdrcom_errno_enum_type hdrpac_change_eidle_slotted_mode
( 
 uint32 slotted_mode_option 
);

/*===========================================================================
  FUNCTION HDRPAC_GET_CURRENT_MAX_CONFIGURED_SCI
  
  DESCRIPTION
  This function retrieves the currently configured max value for slotted 
  sleep from Idle State protocol and converts it into an 
  hdrcp_scmidle_sm_override_enum_type.

  Note that this routine makes an assumption that the AT/AN would never
  (on their own) configure slot_cycle3 to be 213 or 426 ms.  This assumption
  avoids the need to separately store whether the socket API is overridding
  the normal AT/AN configuration.
  
  DEPENDENCIES
  None.
  
  PARAMETERS
  None.
    
  RETURN VALUE
  HDRSCMIDLE_SM_OVERRIDE_INVALID   - Couldn't get SlottedMode attribute value
  HDRSCMIDLE_SM_OVERRIDE_213_MSECS - Current configuration is for 213ms sleep
  HDRSCMIDLE_SM_OVERRIDE_426_MSECS - Current configuration is for 426ms sleep
  HDRSCMIDLE_SM_OVERRIDE_NONE    - Current configuration is whatever AT/AN 
                                configured (not being overridden)
    
  SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrcp_scmidle_sm_override_enum_type hdrpac_get_current_max_configured_sci (void);

#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION HDRPAC_LTE_ATTACH_SUCCESS_IND()

DESCRIPTION
  This function informs HDR about LTE service.

DEPENDENCIES
  None

PARAMETERS
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_lte_attach_success_ind(void);
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */

/*===========================================================================

FUNCTION HDRPAC_SET_DATA_EHRPD_CAPABILITY

DESCRIPTION
  This function sets the flag which tells eHRPD capability (e.g. APN,
  authentication) are valid or not from upper layer (data service)'s point of
  view.
  It is a wrapper function of hdrscp_set_data_ehrpd_capability().

DEPENDENCIES
  None

PARAMETERS
  TRUE - upper layer's eHRPD capability is valid. 
  FALSE - upper layer's eHRPD capability is invalid.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_set_data_ehrpd_capability 
( 
  boolean data_ehrpd_capability
);

/*===========================================================================

FUNCTION HDRPAC_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This function sets force_long_sleep flag.
  It is a wrapper function of hdridle_set_force_long_sleep().
 
  When force_long_sleep flag is set, AT uses 40 seconds as sleep period when
  it is ready to go to sleep state.
 
DEPENDENCIES
  None

PARAMETERS
  force_long_sleep - turn on/off force_long_sleep flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_set_force_long_sleep
(
  boolean force_long_sleep
);

/*===========================================================================

FUNCTION HDRPAC_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This function gets the current value of force_long_sleep flag.
  It is a wrapper function of hdridle_get_force_long_sleep().
 
  When force_long_sleep flag is set, AT uses 40 seconds as sleep period when
  it is ready to go to sleep state. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - force_long_sleep flag is set. 
  FALSE - force_long_sleep flag is not set.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpac_get_force_long_sleep( void );

/*===========================================================================

FUNCTION HDRPAC_GET_EVDO_PAGE_MONITOR_PERIOD

DESCRIPTION
  This function gets the evdo page monitor period, including force_long_sleep
  flag value and current maximum slot cycle index.

DEPENDENCIES
  None

PARAMETERS
  page_monitor_period - pointer that page monitor period info will be copied
                        to.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_get_evdo_page_monitor_period
( 
  hdrpac_page_monitor_period_info_type *page_monitor_period 
);

/*===========================================================================

FUNCTION HDRPAC_REGISTER_FOR_IFACE_EVENTS

DESCRIPTION
  This function registers a callback which calls when an iface event is 
  generated.

DEPENDENCIES
  None

PARAMETERS
  fn_ptr - pointer that holds the callback for iface events

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_register_for_iface_events
( 
  hdrpac_iface_events_cb *fn_ptr 
);

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION HDRPAC_GET_CURRENT_TUNNEL_MODE

DESCRIPTION
  Query API to get the current tunnel mode value from HDR

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  0 – Tunnel mode disabled
  1 – Tunnel mode enabled
  2 – Tunnel-ActiveHO mode
   
SIDE EFFECTS
  None

===========================================================================*/
uint8 hdrpac_get_current_tunnel_mode
( 
  void 
);

/*===========================================================================
FUNCTION HDRPAC_IS_PREREG_ALLOWED

DESCRIPTION
  Query API to get the current prereg state from HDR

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  – Prereg Allowed
  FALSE – Otherwise
     
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpac_is_prereg_allowed
( 
  void 
);


/*===========================================================================
FUNCTION HDRPAC_SEND_IRMI

DESCRIPTION
  This function notifies HDR to send Inter-RAT Mobility Indication

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - If IRMI if IRMI is allowed
  FALSE - Otherwise
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpac_send_irmi
( 
  void
);

/*===========================================================================
FUNCTION HDRPAC_OPEN_TUNNEL_CONNECTION

DESCRIPTION
  This function calls the hdralmp_open_connection to initiate tunnel call
  bringup. This function is synonymous to the ds3g_msh_initiate_call()
  that 3GPP2 calls to bringup data call via CM

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpac_open_tunnel_connection
(
  void
);

/*===========================================================================
FUNCTION HDRPAC_CLOSE_TUNNEL_CONNECTION

DESCRIPTION
  This function calls hdralmp_close_connection() to tear down a tunnel call
  via HDR. This function is synonymous to the ds3g_msh_hangup_call() that
  3GPP2 calls to bring down data call via CM

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpac_close_tunnel_connection
(
  void
);

/*===========================================================================
FUNCTION HDRPAC_SET_PREREG_PPP_STATUS

DESCRIPTION
  This function calls hdrsap_set_prereg_status() to notify HDR about any
  PPP events that could change the Prereg status

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpac_set_prereg_ppp_status
(
  boolean ppp_up
);

/*===========================================================================
FUNCTION HDRPAC_SET_PREREG_VSNCP_STATUS

DESCRIPTION
  This function calls hdrsap_set_prereg_status() to notify HDR about any
  VSNCP events that could change the Prereg status

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpac_set_prereg_vsncp_status
(
  boolean vsncp_up
);

#endif /* FEATURE_DATA_OPTHO */

#endif /* HDRPAC_H */
