
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     H D R   P A C K E T   A P P L I C A T I O N   C O N T R O L L E R

GENERAL DESCRIPTION
  The PAC manages multiple data applications and their associated data
  streams. It's functions include opening and closing of RLP instances,
  mapping the RLP instances to streams and routing received messages to
  the appropriate protocol instance.

EXTERNALIZED FUNCTIONS

  hdrpac_powerup_init()
    Initializes the PAC module at powerup.

  hdrpac_ind_cb()
    Queues an indication to be processed by PAC

  hdrpac_flow_enable()
    Enables flow on the current stream.

  hdrpac_flow_disable()
    Disables flow on the current stream.

  hdrpac_process_ind()
    Processes indications given to the PAC

  hdrpac_get_an_auth_info_from_nv()
    Read authentication variables from NV

  hdrpac_get_sn_ppp_status_from_nv()
    Read the SN PPP status from the NV


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrpac.c#1 $: //source/qcom/qct/modem/data/1x/hdrdata/main/latest/src/hdrpac.c#26 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
02/14/13   hr      Removed session changed ind when SCI override is rejected   
                   because small SCI is negotiated by AN. 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA
04/16/12   ash     Use new HDR API to inform LTE indication.
02/22/12   jz      Disable active HDR->1X handdown for JCDMA
11/18/11   vpk     HDR SU API cleanup
09/26/11   msh     Fixed compiler warnings   
09/08/11   ash     Added support for eHRPD to HRPD fallback.
08/10/11   cnx     Removed session changed ind when SCI override is rejected   
                   because small SCI is negotiated by AN.
08/03/11   vpk     Use hdrtrace
07/26/11   vpk     Remove unused HDR headers
06/03/11   mg      Added hdrpac_set_ehrpd_apns_are_valid().
05/12/11   op      Migrated to MSG 2.0 macros
05/10/11   cnx     Always notify DS when session changed and SCI NONE AUR is accepted by AN.
02/08/11   ls      Global variable cleanup
09/24/10   ms      Read the updated value for RMAC sub type since HDR gets
                   initialized after DS.
07/29/10   rp      Reset sci_gaup_to_default_in_progress flag when session is
                   closed.   
06/11/10   pr      Incorporated fixes for the issue with AT getting stuck   
                   trying to update SCI. 
05/26/10   gc      Added code changes to include support for RMAC4(RevB)
                   subtype for QOS call.
07/27/09   spn     Added call to Notify DS SYS CHG HDLR of change in HDR 
                   session.
06/24/09   ms      If AT fails to send QoS Request due to no signal 
                   or no stream assigned, it should be notified to 
                   HDR_QoS Negotiation manager.
04/09/09   ls      Moved hdrpac_get_rtcmac_flow() to hdrpac_mdm( modem side )
03/27/2009 ls      In hdrpac_process_ind(), if session_change_reason is 
                   HDRHAI_SCHG_OPENED_NORMAL, we call dshdr_an_auth_status_init()
                   to initialize AN authentication status. In hdrpac_process_init_cmd(),
                   it also initializes AN authentication status.
08/21/08   yz      Added hdrpac_current_sub3_rmac_type() and called to update
                   cache at powerup and HDRIND_SCM_SESSION_CHANGED event.
04/24/08   pba     AT initiated GAUP support for SCI7.
04/23/08   hs      Fixed error: DS does not handle session change correctly
04/22/08   spn     Fixed case of handoff from MMPA to other packet apps. 
09/19/07   cc      Replaced HDR_RMAC3_... by HDR_RMAC_... interfaces.
08/29/07   cws     Added support for MCTAP. Removed ref to FEATURE_HDR_IS890A as
                   is890a and is890b are both supersets of is890.
08/01/07   sk      Fixed featurization error
07/22/07   sk      If NV item NV_HDR_EMPA_SUPPORTED_I is set, the HDR session
                   is not tied to PPP.
07/06/07   cc      Added HDRSCP_MC_RMAC support for MSM7500 Rev-B Integration.
06/13/07   etv     Added support for OFS Outage notification.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
04/17/07   ara     Fixed Lint errors
04/11/07   ara     Added fix for CR 107287. This fix will prevent the PPP tear down of the 
                   IPV6 iface if there is a PPP tear down of the IPV4 iface and the IPV6
                   iface is still up.
03/28/07   sk      Added support to handle RF condition
03/26/07   sk      Added EMPA MPA handoff 
03/06/07   sju     Added support to allow overriding SCI multiple times.
01/15/07   sju     Fixed typo.
01/12/07   sju     Moved hdrscmidle.h to hdrpac.h
01/10/07   sju     Fixed setting vt rate and slottedmode attribute during 
                   personality switch.
01/08/07   dna/sju Changed the logic used in HDRPAC_SET_EIDLE_SLOTTED_MODE()..
01/04/07   dna/sju Fixed GAUPing SlottedMode attribute for QChat App.
11/17/06   spn     Merged EIDLE IOCTL Changes
10/16/06   kvd     Moved hdrpac_get_sn_ppp_status_from_nv to hdrpac_mdm.c
10/02/06   sk      Code Featurized
08/24/06   vas     Added handling of session changes dure to prior session
09/19/06   sa      Cleaned up the HDR set EIDLE function.
08/09/06   spn     Corrected wrong comments
07/25/06   ksu     EMPA: 1x RLP code review comments
07/24/06   mpa     Moved hdrpac_get_rtcmac_flow() from hdrpac_mdm
06/20/06   mpa     Moved hdrpacc_update_fcp_state_in_scmdb() to hdrfcp.c
06/20/06   mpa     See 05/10/06 entry.  hdrpac_init() was moved to 
                   hdrpac_mdm_powerup_init() instead.
06/05/06   pba     Added support to store session data in EFS.
05/30/06   ksu     EMPA: RLP init code review comments
05/10/06   vas     Moved hdrpac_init() to hdrpacc_powerup_init() from 
                   hdrpac_powerup_init(). This will ensure a command is queued 
                   for DS task to do powerup initialization only after the 
                   hdrstream_powerup_init() is done. This is because 
                   hdrpacc_powerup_init() is called from the hdrmc task after 
                   hdrstream_powerup_init. Hdrpac_powerup_init() is called 
                   during dstask powerup initialization & not guaranteed to 
                   execute after stream init
05/04/06   ksu     EMPA: RSP part 2
03/18/06   spn     Set the Pri-TC to RLP bindings when Default Packet App 
                   is negotiated.
03/02/06   spn     Re-wrote the code when HDRIND_RMAC3_FLOW_MAPPING_CHANGED
                   indication is given.
02/28/06   spn     QNM was not being notified of RLP bound events, when MRLP
                   was not initialised.
02/21/06   spn     Removed a Debug message as it was misleading.
02/13/06   spn     dsrlp_cleanup() and dsrlp_init() calls when RMAC3_MAPPING_
                   CHANGED indication is given, should be called only if 
                   MRLP has been initialised. Added a check to see if 
                   MRLP has been initialised.
11/04/05   etv     Updated hdrrmac3_get_flow_mapping to be called once for 
                   each MAC flow.
10/28/05   as/vrk  Fixed compiler warnings in function hdrpac_process_ind
10/27/05   ksu     EMPA: octet stream, octet sequencing
10/26/05   dna     Integration updates to GAUP support for RMAC1 rate params
10/19/05   sy      Support for version2.0 reservation signalling messages.
10/05/09   vas/sy  hdrpac_get_rtcmac_flow() returns default RMAC flow incase no match found for 
                   RLP flow.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/14/05   vas     Compilation fixes to compile when FEATURE_HDR_RL_RATE_INTERIA
                   is turned off.
09/13/05   vas     Added code to enable/disable or disable the rate params 
                   based on if GAUP was requested for normal or VT params
09/13/05   vas     Notification as to outcome of Rate parameters GAUP
09/08/05   dna/etv Added support for AT-init GAUP of Rate Parameters.
08/08/05   sk/sy   Merged semi-dynamic QOS changes from Sandbox.
07/11/05   vas     Ignored substream in hdrpac_get_rtcmac_flow() 
                   if using default packet apps
07/08/05   jyw     Removed the Xon and Xoff message for IS890 and IS890A.
07/08/05   jyw     Added the support to the IS890A.
05/23/05   sy      Removed dsiface.h inclusion as it has been deleted.
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
03/24/05   pba     Fixed compiler warning, included hdrmrlpc.h
03/07/05   pba     Moved all the PACC related processing to hdrpacc.c 
03/07/05   pba     Separated handling of In-Use and In-Config messages for PAC.
03/07/05   pba     Process all indications to PAC in DS task context. Session 
                   related indications need to be processed both by PAC in DS 
                   task context and PACC in MC task context.
03/07/05   pba     Added code to process HDRIND_RUP_CONNECTION_INITIATED.
01/18/05   ifk     Took out setting of ppp_set_an_auth_status_function.
11/22/04   vas     Added GAUP Support
10/30/04   mpa     Added GAUP support.
10/20/04   vas     Added fix to close the stream state if HDR session is closed
09/23/04   vas     Changed a ERR message to MSG_MED as it is not an error
09/20/04   mpa     Added hdrpac_close_connection().
09/09/04   mpa     Added support for packet app specific negotiation
03/20/04    ak     Updates for new pkt mgr interfaces.
03/10/04    gr     Added function to read the SN PPP status from NV
03/09/04    gr     Added code to process HDRPAC_CLEAR_SN_PPP_STATUS_CMD
02/13/04   vas     Removed processing of connection opened/closed ind
11/14/03    vr     Process indications in HDRMC task context. Only hdrpacdsif
                   part of Session Closed IND will be processed in DS task
                   context.
10/22/03   mpa     ALN trigger featurized under FEATURE_HDR_ALN
10/14/03   vas     Added code to ensure that xon is sent if DataReady received
                   on AN stream or on active/dormant SN stream.
                   Added processing for HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD
10/14/03   vr      Changed call to ds_initiate_aln to ds707_pkt_post_aln_cmd
10/13/03   vas     Restructured code. Added code for routing commands to
                   hdrpacdsif
10/03/03   vas     Ensure that the AN stream is "open" after powerup
                   (or entering HDR online)
09/18/03   vas     Removed registering and de-registering of SN RLP callbacks
                   at session open.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSID
07/23/03   mpa     Merged back in MSM6500 baseline
07/10/03   vas     Added code to send Xon when HDR session opened and data
                   session is dormant. Mapping func. changed to accomodate
                   Fixed sr id values for AN & SN RLPs.
06/18/03   vas     Queue the pac_init command
05/15/03   mpa     Receiving HDRIND_LUP_ALN_TRIGGER to initiate ppp resync
04/03/03   mpa     Added HDRTRACE_IND event.
03/05/03   vas     Changed hdralmp_connection_is_closing to
                   hdrcon_connection_is_closing
02/27/03    vr     Map app to stream in HDRMC task context if a new session
                   is opened.
02/10/03   kvd     Send XON on SN stream when HDR session is reconfigured or
                   a previous session reopened.
01/30/03   vas     Fix to previous check in.
01/30/03   vas     End Data Session if HDR session is closed (under feature).
01/14/03   vas     Deactivate TestApp if the SessionReconfiguration causes
                   TestApp negotiation change.
10/03/02   vas     Process ConnectionClosed ind to cleanup RLP for SN stream
08/20/02   vas     Added check to ensure NAI/password lengths read from NV
                   are valid values.
08/13/02   vas     Initialize SN RLP when Conn Opened ind recvd only if the
                   Ind indicates that Connection Open was triggered by network
                   or user. This will prevent RLP from incorrectly being init
                   for HDR signaling related connections (Reconfig,etc).
07/19/02   vas     Check if connection is closing before initializing RLP
06/12/02   vas     Route InConfig messages based on the negotiated App
                   Subtype for which they are received.
03/18/02   vas     Added code to register a callback function with PPP which
                   sets HMP auth status. Gets called when CHAP succeeds/fails
03/16/02   vas     Added code to send XonReq when DataReady rcvd on SN stream
                   if the stream is already in Open state, or if the call is
                   dormant/able to accept data on the SN stream.
03/01/02   hcg     Updated to support multiple simultaneous RLP instances by
                   modifying stream support and indication processing.  Also,
                   incorporated authentication NV support by adding
                   hdrpac_get_an_auth_info_from_nv().
02/26/02   vas     Added hdrpac_app_flow_is_enabled
02/05/02   vas     Changed AN authentication status from boolean to enum.
01/14/02   vas     When PAC received Connection Opened Indication, it will
                   check to make sure DSMGR is originating a call or a call is
                   active before registering watermarks & initializing RLP.
12/03/01   vas     hdrscp_set_an_auth_status prototype changed
12/01/01   vas     Added support to specify the app subtype for the stream to
                   be flow controlled. Added function to set the HDR AN
                   Authentication status.
10/29/01   vas     Added hdrpac_update_rlp_reg_srvc() & associated processing
                   for edge case handling of registering of RLP service after
                   HDR connection has been opened.
10/08/01   vas     Handle Default Signalling App & stream not being used in
                   hdrpac_map_app_to_stream()
09/25/01   vas     Added protection against NULL messages being passed to
                   hdrpac_msg_cb(). In msg_cb() free dsm item if error in
                   processing msg.
09/14/01   vas     Added hdrpac_flow_is_enabled(). Check whether HDRRLP
                   registered successfully before initializing RLP instance.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/28/01   vas     Removed hdrpac_get_id_app_bnd_to_service_nw(). Added call
                   to hdrrlp_reg_srvc() to register service for desired RLP.
08/17/01   vas     Added hdrpac_get_id_app_bnd_to_service_nw()
07/30/01   vas     Created module.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "err.h"
#include "data_msg.h"

#include "hdrds.h"
#include "hdrpacdsif.h"

#include "hdrcom_api.h"
#include "hdrcp_api.h"
#include "dsm.h"
#include "dsrlp_v.h"
#include "hdrrlp.h"
#include "ps.h"

#ifdef FEATURE_HDR_QOS
  #include "ds707_sec_pkt_mgr_handoff.h"
#endif /* FEATURE_HDR_QOS */

#include "dstaski.h"
#include "dsatctab.h"

#include "dshdr_an_mgr.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgr_hdr.h"
#include "ds707_extif.h"

#include "ps_iface_defs.h"
#include "ps_phys_link.h"

#ifdef FEATURE_HDR_IS890
  #include "hdrcom_api.h"
#endif /* FEATURE_HDR_IS890 */

#include "hdrpac_mdm.h"
#include "hdrmrlpctl.h"
#include "hdrpacqnm.h"

#include "data_err_suppr.h"
#include "ds707_sys_chg_hdlr.h"
#include "ds707_pdn_context.h"
#include "dscam.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
  #include "ds707_pkt_mgr_hdlr.h"
#endif /* FEATURE_DATA_OPTHO */

#include "ds_dsd_ext_i.h"
#include "ds707_roaming.h"
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* PAC runs in the DS task */
#define HDRPAC_TASK_PTR &ds_tcb

/*--------------------
  Mapping Table:
  RLP ID = APP Subtype for AN and SN streams. Thus,
    For AN, RLP ID = 1,
    For SN, RLP ID = 2.
  The stream that these are on depends on what has been negotiated on during
  the Session Configuration.
---------------------*/

typedef struct
{
  hdrcom_hai_app_subtype_enum_type app_subtype; /* Application Subtypes */
  hdrcom_hai_stream_enum_type stream_id;        /* Stream associated with App */
  boolean flow_enabled;  /* RLP flow enabled or not (Stream open/closed)  */
} hdrpac_map_table_type;

#define HDRPAC_MAX_STREAMS 4  /* Maximum number of Streams possible */

typedef struct
{
  /* Rate Parameters Mode successfully negotiated with the AN */
  hdrscmrmac0_rate_params_mode_enum_type val_an_has;

  /* Rate Parameters Mode the application thinks is the current setting */
  hdrscmrmac0_rate_params_mode_enum_type val_app_thinks;

  /* Critical Section to protect updates to current and Inconfig Rate 
     Parameters Mode variables */
  rex_crit_sect_type mode_crit_sec;

  /* Transaction ID of the last RateParam AttribUpdate transaction */
  uint8 latest_trans_id;

  /* Transaction ID of the last RateParam AttribUpdate transaction
   * initiated through the API (not from HDRIND_IDLE_CONNECTION_OPENED) */
  uint32 latest_api_initiated_trans_id;

} hdrpac_vt_rate_params_status_type;

typedef struct
{
  /* Slotted Mode value successfully negotiated with the AN */
  hdrcp_scmidle_sm_override_enum_type val_an_has;

  /* Slotted Mode value the application thinks is the current setting */
  hdrcp_scmidle_sm_override_enum_type val_app_thinks;

  /* Critical Section to protect updates to current and Inconfig Rate 
     Parameters Mode variables */
  rex_crit_sect_type mode_crit_sec;

  /* Transaction ID of the last SlottedMode AttribUpdate transaction */
  uint8 latest_trans_id;
} hdrpac_slotted_mode_params_status_type;

/* The following structure contains all the global variables used by PAC */
static struct
{
  /* Array containing the mapping of Application Subtypes, Application
     Instance (Service ID of RLP) and streams. Array element 0 contains
     entries for Stream 0.
  */
  hdrpac_map_table_type map_table[HDRPAC_MAX_STREAMS];

  hdrpac_vt_rate_params_status_type rp;
  hdrpac_slotted_mode_params_status_type sm;
  boolean sm_init;
  boolean sci_gaup_to_default_in_progress;
} hdrpac;

#define HDRPAC_RP_NO_OUTSTANDING_CONFIG 0xFFFFFFFF
#define HDRPAC_SM_NO_OUTSTANDING_CONFIG 0xFFFFFFFF

static uint16 hdrpac_current_rmac = HDRCP_SCP_DEFAULT_RMAC;

#ifdef FEATURE_EIDLE_SCI
static hdrpac_iface_events_cb *iface_events_cb_ptr = NULL;
#endif /* FEATURE_EIDLE_SCI */
/*===========================================================================

                     LOCAL FUNCTION DECLARATIONS

===========================================================================*/

static void hdrpac_process_session_closed( void );

static void hdrpac_map_app_to_stream (
                      hdrcom_hai_session_change_reason_enum_type session_change_reason
                                );



#if defined (FEATURE_HDR_QOS) && defined (FEATURE_HDR_REVA_L1)
static void hdrpac_process_rmac_flow_mapping_changed_ev(void);
#endif /* defined (FEATURE_HDR_QOS) && defined (FEATURE_HDR_REVA_L1) */

/*===========================================================================

                        FUNCTION DEFINITIONS

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
  uint16 : Current RMAC subtype

SIDE EFFECTS
  None
===========================================================================*/
uint16 hdrpac_current_rmac_sub_type(void)
{
 /*-----------------------------------------------------------------------
  Read the RMAC type from protocol. If the HDR protocol didnt get initialized
  before the Data services.
  -----------------------------------------------------------------------*/
 
  if( hdrpac_current_rmac == HDRCP_SCP_DEFAULT_RMAC )
  {
    hdrpac_current_rmac = 
           hdrcp_scp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL );
  }
  return hdrpac_current_rmac;
}

/*===========================================================================


FUNCTION HDRPAC_PS_IFACE_GENERIC_IND

DESCRIPTION
  This function allows for a generic call to 
  ps_iface_generic_ind for both IPV4 and IPV6.

DEPENDENCIES
  None

PARAMETERS
  event - event that has occurred.
  *event_data_ptr - data associated with the event.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrpac_ps_iface_generic_ind
(
ps_iface_event_enum_type             event,
void                                *event_data_ptr
)
{
  ps_iface_type             *iface_ptr;
  uint8                     curr_iface_index = 0;

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {      

    iface_ptr = 
      ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

    ps_iface_generic_ind(iface_ptr,
                         event,
                         event_data_ptr); 
  }
#ifdef FEATURE_EIDLE_SCI                         
  if (iface_events_cb_ptr) {
      DATA_HDR_MSG0( MSG_LEGACY_LOW, "Passing event to Modehandler" );
     iface_events_cb_ptr(event, event_data_ptr);
  }
#endif /* FEATURE_EIDLE_SCI */ 
} /* hdrpac_ps_iface_generic_ind() */


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
  HDRSCMIDLE_SM_OVERRIDE_INVALID    - Couldn't get SlottedMode attribute value
  HDRSCMIDLE_SM_OVERRIDE_213_MSECS  - Current configuration is for 213ms sleep
  HDRSCMIDLE_SM_OVERRIDE_426_MSECS  - Current configuration is for 426ms sleep
  HDRSCMIDLE_SM_OVERRIDE_1280_MSECS - Current configuration is for 1280ms sleep
  HDRSCMIDLE_SM_OVERRIDE_NONE       - Current configuration is whatever AT/AN 
                                      configured (not being overridden)
    
  SIDE EFFECTS
  None.
  
===========================================================================*/

hdrcp_scmidle_sm_override_enum_type hdrpac_get_current_max_configured_sci (void)
{
  hdrcp_scmidle_sm_override_enum_type ret_val = HDRSCMIDLE_SM_OVERRIDE_INVALID;
#ifdef FEATURE_EIDLE_SCI
  hdrcp_idlec_slotted_mode_type slotted_mode_attrib;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrcp_idlec_get_slotted_mode_attrib(&slotted_mode_attrib))
  {
    if (slotted_mode_attrib.slot_cycle3 == 0x05)
    {
      ret_val = HDRSCMIDLE_SM_OVERRIDE_213_MSECS;
    }
    else if (slotted_mode_attrib.slot_cycle3 == 0x06)
    {
      ret_val = HDRSCMIDLE_SM_OVERRIDE_426_MSECS;
    }
    else if (slotted_mode_attrib.slot_cycle3 == 0x07) 
    {
      ret_val = HDRSCMIDLE_SM_OVERRIDE_1280_MSECS;
    }
    else
    {
      ret_val = HDRSCMIDLE_SM_OVERRIDE_NONE;
    }
  }
#endif /* FEATURE_EIDLE_SCI */
  return ret_val;
}

/*===========================================================================
FUNCTION  HDRPAC_GET_APP_SUBTYPE_FOR_STREAM

DESCRIPTION 
   This function returns the appsubtype for a given stream id from the hdrpac
   map table. This is an accessor function called from the Modem to access the 
   hdrpac structure residing in the Apps side

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
hdrcom_hai_app_subtype_enum_type hdrpac_get_app_subtype_for_stream(hdrcom_hai_stream_enum_type stream_id) 
{
  return(hdrpac.map_table[(stream_id)].app_subtype);
} /*hdrpac_get_app_subtype_for_stream*/

/*===========================================================================
FUNCTION  HDRPAC_POWERUP_INIT

DESCRIPTION
  This function does the initializations for the PAC on power up.

DEPENDENCIES
  Needs to be called after hdrstream initialization.
  Init for DS.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_powerup_init( void )
{
  int i; /* index counter */

  /* Initialize array elements to known values */
  for ( i=0; i<HDRPAC_MAX_STREAMS; i++ )
  {
    hdrpac.map_table[i].app_subtype = HDRHAI_APP_STREAM_NOT_USED;
  }

  /* Initialize variables to keep track of Rate Parameters setting */
  hdrpac.rp.val_app_thinks = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  hdrpac.rp.val_an_has = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  hdrpac.rp.latest_api_initiated_trans_id = HDRPAC_RP_NO_OUTSTANDING_CONFIG;

  /* Initialize variables to keep track of Slotted Mode setting */
  hdrpac.sm.val_app_thinks = HDRSCMIDLE_SM_OVERRIDE_NONE;
  hdrpac.sm.val_an_has = hdrpac_get_current_max_configured_sci();
  hdrpac.sci_gaup_to_default_in_progress = FALSE;

  hdrpac.sm_init = FALSE;

  /* Initialize the Critical section variable */
  rex_init_crit_sect(&hdrpac.rp.mode_crit_sec);
  rex_init_crit_sect(&hdrpac.sm.mode_crit_sec);

  /*---------------------------------------------------------------------------
  Powerup initialization of hdrpacdsif module.
  ---------------------------------------------------------------------------*/
  hdrpacdsif_powerup_init();

  /*---------------------------------------------------------------------------
  Powerup initialization of hdrpacdsif module.
  ---------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
  Read the RMAC type from protocol. The HDR protocol should initialized
  before the Data service.
  -----------------------------------------------------------------------*/
  hdrpac_current_rmac =
           hdrcp_scp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL );
} /* hdrpac_powerup_init() */

/* <EJECT> */

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
void hdrpac_init( void )
{

  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to queue up a command to PAC that will run in the DS task.
     This function needs to run after the hdrpac_powerup_init function as
     well as hdrstream_powerup_init function is called. It is called from
     hdrstream_powerup_init() which runs in the MC task and is higher
     priority than DS task. By queueing up a command in the DS task, we
     ensure that the pac_powerup_init function has been called when we
     process the queued command
  */

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_INIT_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
} /* hdrpac_init() */

/* <EJECT> */

/*===========================================================================

FUNCTION HDRPAC_PROCESS_INIT_CMD

DESCRIPTION
 This function processes the init command to PAC.

DEPENDENCIES

PARAMETERS

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void hdrpac_process_init_cmd
(
void
)
{
  hdrcom_hai_stream_enum_type stream_id; /* Stream corresponding the RLP       */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Stream Configuration has been negotiated, then map the applications
  to the associated streams. A valid stream configuration will exist if the
  HDR session is open */
  hdrpac_map_app_to_stream(HDRHAI_SCHG_OPENED_NORMAL); 

  /* initialize AN authentication status */
  dshdr_an_auth_status_init();

  /* If the AN stream is negotiated, then we need to make sure that the state
  of the stream is Open. Since the state is not stored in NV, It may be
  Closed when the AT just powers up. Note that we do not send an Xon message,
  just update the state so that we don't discard data received on the stream
  */
  if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_AN) )
  {
    /*-----------------------------------------------------------------------
      Determine which RLP (stream) the Xon message is to be sent
    -----------------------------------------------------------------------*/
    stream_id = hdrpac_get_stream_for_app_subtype( HDRHAI_DEF_PACKET_APP_AN );

    /*-----------------------------------------------------------------------
      If the stream is a valid one, enable flow for it
    -----------------------------------------------------------------------*/
    if (stream_id != HDRHAI_STREAM0)
    {
      hdrpac.map_table[stream_id].flow_enabled = TRUE;
    }
    else
    {
      DATA_HDR_MSG1(MSG_LEGACY_ERROR,
                    "Cant enable flow for (Stream:%d)", stream_id);
    }

  }

  /* Set the RLP of the Primary TC when DPA */
  if ( hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
  {
    ds707_pkt_set_pri_tc_rlp( DSRLP_HDR_SN_RLP_SR_ID_VALUE, DSRLP_VER_HDR );
  }

  /* Initialize sys change handler */
  ds707_sys_chg_hdlr_init();

} /* hdrpac_process_init_cmd() */

/*===========================================================================
FUNCTION HDRPAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to the PAC. Indications are 
  processed in the DS task context

DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication for PAC to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrpac_process_ind
(
hdrcom_ind_name_enum_type ind_name,
hdrcom_ind_data_union_type *ind_data_ptr
)
{
  ps_phys_link_type         *phys_link_ptr;
#ifdef FEATURE_HDR_RL_RATE_INERTIA
  ps_hdr_rev0_rate_inertia_failure_code_enum_type fail_code;
#endif /* FEATURE_HDR_RL_RATE_INERTIA */
 

#ifdef FEATURE_EIDLE_SCI
  hdrerrno_enum_type err_no = E_SUCCESS; /* Error code */
  ps_hdr_slotted_mode_failure_code_enum_type sm_fail_code;
  hdrcp_scmidle_sm_override_enum_type sm_current_sci;
  hdrpac_page_monitor_period_info_type page_monitor_period;
  boolean        report_outcome = TRUE; /* report session_changed(current_sci) to App*/
#endif /* FEATURE_EIDLE_SCI */

  boolean process_session_changed_is_needed = TRUE;

  hdrcom_hai_session_change_reason_enum_type session_change_reason; 
  ps_iface_rf_conditions_enum_type ps_rf_condition;
  handoff_client_data client_data;
  ds_dsd_event_info_type  event_info={0};

  DATA_MDM_ARG_NOT_CONST(ind_data_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDRCOM_TRACE_IND(HDRHAI_PACKET_APPLICATION_CONTROLLER, ind_name, 0);

  DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: Processing HDR Indication: %d", 
                    ind_name);
  /*-------------------------------------------------------------------------
  Determine which indication has been received.
  -------------------------------------------------------------------------*/
  switch ( ind_name )
  {
    case HDRIND_STREAM_CONFIG_COMPLETE:
      /* If we need to do further negotiation */
      break;

    case HDRIND_SCP_NO_PREV_SESSION:
      /*-----------------------------------------------------------------------
      The prior session could not be opened. Thus, the session is closed.
      -----------------------------------------------------------------------*/
      hdrpac_process_session_closed();
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /*-----------------------------------------------------------------------
      Read the RMAC type which might be updated with SCM_CHANGED event
      -----------------------------------------------------------------------*/
      hdrpac_current_rmac =
               hdrcp_scp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL );

      session_change_reason = 
      (hdrcom_hai_session_change_reason_enum_type) ind_data_ptr->change_reason;

       if ( (session_change_reason == HDRHAI_SCHG_CLOSED_NORMAL) || 
           (session_change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR) )
       {
         hdrpac.sci_gaup_to_default_in_progress = FALSE;
       }

      if ( session_change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
      {
        /*-----------------------------------------------------------------------
        This is not really a session close, but an attempt to restore the prior
        session. So ignore it.
        -----------------------------------------------------------------------*/
        DATA_HDR_MSG0(MSG_LEGACY_MED, "Attempting to restore prior session.."
                      "ignoring Session changed ind"); 
        hdrmrlpctl_restore_prior_session_attempted();

#ifdef FEATURE_DATA_OPTHO
        /* Notify S101 so it will let EPC know to not attempt prereg
        while HDR is attempting to restore prior session */
        DATA_HDR_MSG0(MSG_LEGACY_HIGH, "HDR attempting Prior session restore."
                                        "Notify S101 about session chg");
        ds707_s101_hdr_session_chg_ev(FALSE);
#endif /* FEATURE_DATA_OPTHO */
        return;
      }
      else if( session_change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR )
      {
        /*-----------------------------------------------------------------------
        The prior session has been restored - so nothing has to be be done at
        the Packet app layer. Simply ignore this indication
        -----------------------------------------------------------------------*/
        DATA_HDR_MSG0(MSG_LEGACY_MED, "Prior session restored..."
                      "ignoring Session changed ind"); 

        process_session_changed_is_needed = FALSE;
        hdrmrlpctl_prior_session_restored();
        /*------------------------------------------------------------------------
         Give a bearer tech changed notification to indicate the opening of a new
         session
        --------------------------------------------------------------------------*/
        ds707_pkt_bearer_tech_changed_notification();

#ifdef FEATURE_DATA_OPTHO
        /* Notify S101 that prior session has been restored*/
        DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Prior session restored."
                                        "Notify S101 about session chg");
        ds707_s101_hdr_session_chg_ev(TRUE);
#endif /* FEATURE_DATA_OPTHO */

      }      
      else if( session_change_reason == HDRHAI_SCHG_OPENED_NORMAL )
      {
        /*-----------------------------------------------------------------------
        We need to initialize AN authentication status when session is opening
        -----------------------------------------------------------------------*/
        DATA_HDR_MSG0(MSG_LEGACY_MED, "Session is opening, we need to "
                      "initialize AN authentication status"); 
        /* initialize AN authentication status */
        dshdr_an_auth_status_init();

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
        /*-----------------------------------------------------------------------
          New session is opened, reset PPP failure flag
        -----------------------------------------------------------------------*/
        ds707_set_ppp_failure_flag(FALSE);
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
      }

      if (process_session_changed_is_needed)
      {
        if ( hdrcp_smp_session_is_open(HDRHAI_PACKET_APPLICATION_CONTROLLER) &&
             hdrcp_stream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY)
           )
        {
          /*---------------------------------------------------------------------
          Map the Application instances to the appropriate stream as negotiated
          for the new session.
          ---------------------------------------------------------------------*/
          hdrpac_map_app_to_stream( session_change_reason );

          /*---------------------------------------------------------------------
          Upon session configuration complete, enable flow on the AN stream,
          if it has been negotiated.  Flow is always enabled for AN stream to
          support AN-initiated authentication procedures.
          ---------------------------------------------------------------------*/
          if (hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_AN) )
          {
            /*--------------------------------------------------------------------- 
              In Tunnel Mode, AN does not need to wait for AT to transmit
              reverse link (which is the case with direct mode) and it can
              send RLP data immediately after sending LinkOpenConf.
              So don't enable the AN flow here and enable it only after AN
              RLP is inited in hdrpac_mdm_an_process_cm_call_connected_cmd().
              Even if optho is not enabled, S101 will transition to tunnel
              disallowed when UE gets LTE service. So if session nego happens
              when DSD pref sys is LTE OOS, we'll end up not enabling the AN
              flow. Hence check for Tunnel Value and enable flow if tunnel
              value is not 1/2
            ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_OPTHO
            if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state() ||
                FALSE == ds707_s101_is_tunnel_mode() ||
                (hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_AN)==TRUE )
               )
#endif /* FEATURE_DATA_OPTHO */
            {
              hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_AN );
            }
          }

          /*---------------------------------------------------------------------
          Upon session configuration complete, enable flow on the SN stream,
          if it has been negotiated and there is a dormant data session.
          ---------------------------------------------------------------------*/
          phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

          if ( hdrcp_stream_stream_is_assigned(HDRHAI_DEF_PACKET_APP_SN))
          {
            ds707_pkt_set_pri_tc_rlp( DSRLP_HDR_SN_RLP_SR_ID_VALUE, DSRLP_VER_HDR );

            if ( ( hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_SN)==TRUE ) ||
              ds707_extif_is_pkt_data_sess_on_hdr() ||
              ds707_extif_is_pkt_data_sess_on_cdma() )
            {
              hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_SN );
            }
            /*------------------------------------------------------------------------
             Give a bearer tech changed notification to indicate the opening of a new
             session
            --------------------------------------------------------------------------*/
            ds707_pkt_bearer_tech_changed_notification();
          }

  #ifdef FEATURE_HDR_QOS
          if ( hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) &&
              (( hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_SN)==TRUE ) ||
              ds707_extif_is_pkt_data_sess_on_hdr() ||
              ds707_extif_is_pkt_data_sess_on_cdma() ))
          {
            hdrpac_flow_enable( HDRHAI_MULTIFLOW_PACKET_APP_SN );

          }
          else if ( hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) &&
                    ( ( hdrpac_app_flow_is_enabled(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)==TRUE ) ||
                      ds707_pkt_is_dormant( phys_link_ptr ) ) )
          {
            hdrpac_flow_enable( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN );
          }
  #ifdef FEATURE_HDR_REVB
          else if ( hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) &&
                    ( ( hdrpac_app_flow_is_enabled(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)==TRUE ) ||
                      ds707_pkt_is_dormant( phys_link_ptr ) ) )
          {
            hdrpac_flow_enable( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
          }
  #endif /* FEATURE_HDR_REVB */
  #endif /* FEATURE_HDR_QOS */
        }
        else
        {
          hdrpac_process_session_closed();
        }
      }

      /* Notify that hdr session has changed */
      ds707_sys_chg_hdlr_hdr_session_changed();

#ifdef FEATURE_EIDLE_SCI
      rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

      sm_current_sci = hdrpac_get_current_max_configured_sci();

      if (session_change_reason == HDRHAI_SCHG_OPENED_NORMAL)
      {
        /* new sessoin opened, invalidate scmidle_info */
        hdrcp_scmidle_set_scmidle_info_valid(FALSE);
      }
      else if(session_change_reason == HDRHAI_SCHG_PERSONALITY_SWITCH)
      {
        /* always update val_an_has to sm_current_sci in PS case*/
        hdrpac.sm.val_an_has = sm_current_sci;
      }
  
      if (sm_current_sci == HDRSCMIDLE_SM_OVERRIDE_INVALID)
      {
        /* Just report session changed indication to appication.
         * use cases:
         * 1. PS revA-> rev0
         * 2. session change reason == HDRHAI_SCHG_CLOSED_NORMAL 
         */
        DATA_HDR_MSG0( MSG_LEGACY_HIGH, 
                       "SMGaup SesChg: Couldn't retrieve SlottedMode attrib");

      }
      else if( !hdrcp_scmidle_saved_scmidle_info_is_valid())
      {
        /* saved_scmidle_info is not valid means sci was not overridden after 
         * session changed. Update app_thinks and an_has to  current_sci
         */
        DATA_HDR_MSG4 (MSG_LEGACY_HIGH, 
                       "SMGaup SesChg: (%d->%d) Negotiation Completed [app %d->%d]",
                       hdrpac.sm.val_an_has, sm_current_sci, 
                       hdrpac.sm.val_app_thinks, sm_current_sci);
        hdrpac.sm.val_an_has = sm_current_sci;
        hdrpac.sm.val_app_thinks = sm_current_sci;
      }
      else 
      {
        /* sci was overridden while receiving session changed ind  */

        if (sm_current_sci == hdrpac.sm.val_app_thinks)
        {  
          /* After session changed, current sci (as well as an_has) is same as
           * app_thinks. Nothing need to be done here,
           * use cases:
           * 1. restore prior session successfully
           * 2. Personality switch from revA->rev0->revA when nothing happened in 
           *    rev0 system (e.g. app didn't close in rev0)
           */
          DATA_HDR_MSG4 (MSG_LEGACY_HIGH, 
                         "SMGaup SesChg: (%d->%d) No change on App SlottedMode[app %d->%d]",
                         hdrpac.sm.val_an_has, sm_current_sci, 
                         hdrpac.sm.val_app_thinks, sm_current_sci);
        }
        else
        {
          /* Current sci is different than app_thinks */

          if (hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_NONE)
          {
            /* SCI was overridden but it is no longer needed by the App.   
             * Use case:
             * 1. power off while SCI was overridden, then successfully restore prior session.
             * 2. PS revA->rev0, app closed, PS rev0->revA
             * 3. PS revA1(override)->revA2(no override and report to app)->revA1(override)
             */

            /* Send the AttributeUpdateRequest Message with new SlottedMode parameter. */
            err_no = hdrcp_scmidle_gaup_slotted_mode_attrib( hdrpac.sm.val_app_thinks, 
                                                      &hdrpac.sm.latest_trans_id );

            /* hdrpac.sm.latest_trans_id is not modified unless E_SUCCESS is returned. */
       
            if (err_no == E_SUCCESS)
            {
              DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                             "SMGaup SesChg: (%d->%d) In Progress [TransID %d, AN actual %d]", 
                             hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                             hdrpac.sm.latest_trans_id, 
                             hdrpac_get_current_max_configured_sci());

              hdrpac.sci_gaup_to_default_in_progress = TRUE;
            }
            else
            {
              DATA_HDR_MSG3( MSG_LEGACY_FATAL, 
                             "SMGaup SesChg: (%d->%d) Failed! TX couldn't start [AN actual %d]", 
                             hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                             hdrpac_get_current_max_configured_sci());
            }

          }
          else
          {
            /* sci was overridden but now is changing to a new override value 
             * use case:
             * 1. revA#1 was overridden to 5, revA#2 was overridden to 6, PS from revA#1 to revA#2
             */
            DATA_HDR_MSG4 (MSG_LEGACY_HIGH, 
                           "SMGaup SesChg: (%d->%d) Session change Completed [app %d->%d]",
                           hdrpac.sm.val_an_has, sm_current_sci, 
                           hdrpac.sm.val_app_thinks, sm_current_sci);
            hdrpac.sm.val_an_has = sm_current_sci;
            hdrpac.sm.val_app_thinks = sm_current_sci;
          }
        }
      }

      /* Return session_changed indication with current sci back to Application*/
      if (report_outcome)
      {

        DATA_HDR_MSG0( MSG_LEGACY_LOW, "SMGaup Debug: calling ps_iface_generic_ind" );
        hdrpac_get_evdo_page_monitor_period(&page_monitor_period);
        hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV,
                                     &page_monitor_period );
        
      }

      rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);
#endif /* FEATURE_EIDLE_SCI */

      break;

    case HDRIND_FCP_DATA_READY_RCVD:
      /*---------------------------------------------------------------------
      This indication is given by FCP when it receives a Data Ready message
      on a closed stream.  If this DataReady message is for the Access
      Network, it indicates a desire to communicate on the Access Network
      stream.  Always enable flow for the AN stream. Send an XonReq on the
      SN stream only if the stream is already in open state.
      ---------------------------------------------------------------------*/
      if ( hdrpac.map_table[ind_data_ptr->stream_id].app_subtype ==
           HDRHAI_DEF_PACKET_APP_AN)
      {
        /*-------------------------------------------------------------------
          For some reason the AN stream is flow controlled.  Just enable
          flow, since the AT can always handle AN traffic.
        -------------------------------------------------------------------*/
#ifdef FEATURE_DATA_OPTHO
        /*--------------------------------------------------------------------- 
          In Tunnel Mode, AN does not need to wait for AT to transmit
          reverse link (which is the case with direct mode) and it can
          send RLP data immediately after sending LinkOpenConf.
          So don't enable the AN flow here and enable it only after AN
          RLP is inited in hdrpac_mdm_an_process_cm_call_connected_cmd()
          Even if optho is not enabled, S101 will transition to tunnel
          disallowed when UE gets LTE service. So if data_ready is recvd
          when DSD pref sys is LTE OOS, we'll end up not enabling the AN
          flow. Hence check for Tunnel Value and enable flow if tunnel value
          is not 1/2
        ---------------------------------------------------------------------*/
        if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state() ||
            FALSE == ds707_s101_is_tunnel_mode() ||
            (hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_AN)==TRUE )
           )
#endif /* FEATURE_DATA_OPTHO */
           {
              DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "DRdy rcvd. Send Xon AN strm:%d",
                       ind_data_ptr->stream_id);
              hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_AN );
           }
      }
      else if ( hdrpac.map_table[ind_data_ptr->stream_id].app_subtype ==
                HDRHAI_DEF_PACKET_APP_SN )
      {
        if ( ( hdrpac_app_flow_is_enabled(HDRHAI_DEF_PACKET_APP_SN)==TRUE ) ||
             ds707_extif_is_pkt_data_sess_on_hdr() ||
             ds707_extif_is_pkt_data_sess_on_cdma() )
        {
          /*-----------------------------------------------------------------
            Send an Xon on the Service Network stream if the flow is already
            enabled, or if the call is in dormant or active state.
          -----------------------------------------------------------------*/
          DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "DRdy rcvd. Send Xon SN strm:%d",
                         ind_data_ptr->stream_id);
          hdrpac_flow_enable( HDRHAI_DEF_PACKET_APP_SN );
        }
      }
#ifdef FEATURE_HDR_QOS
      else if ( hdrpac.map_table[ind_data_ptr->stream_id].app_subtype ==
                HDRHAI_MULTIFLOW_PACKET_APP_SN )
      {
        if ( ( hdrpac_app_flow_is_enabled(HDRHAI_MULTIFLOW_PACKET_APP_SN)==TRUE ) ||
             ds707_extif_is_pkt_data_sess_on_hdr() ||
             ds707_extif_is_pkt_data_sess_on_cdma() )
        {
          /*-----------------------------------------------------------------
            Send an Xon on the Service Network stream if the flow is already
            enabled, or if the call is in dormant or active state.
          -----------------------------------------------------------------*/
          DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "DRdy rcvd. Send Xon SN strm:%d",
                         ind_data_ptr->stream_id);
          hdrpac_flow_enable( HDRHAI_MULTIFLOW_PACKET_APP_SN );
        }
      }
      else if ( hdrpac.map_table[ind_data_ptr->stream_id].app_subtype ==
                HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN )
      {
        if ( ( hdrpac_app_flow_is_enabled(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)==TRUE ) ||
             ds707_extif_is_pkt_data_sess_on_hdr() ||
             ds707_extif_is_pkt_data_sess_on_cdma() )
        {
          /*-----------------------------------------------------------------
            Send an Xon on the Service Network stream if the flow is already
            enabled, or if the call is in dormant or active state.
            -----------------------------------------------------------------*/
          DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "DRdy rcvd. Send Xon SN strm:%d",
                         ind_data_ptr->stream_id);
          hdrpac_flow_enable( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN );
        }
      }
//VIP HACK
#ifdef FEATURE_HDR_REVB
      else if ( hdrpac.map_table[ind_data_ptr->stream_id].app_subtype ==
                HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN )
      {
        if ( ( hdrpac_app_flow_is_enabled(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN)==TRUE ) ||
             ds707_extif_is_pkt_data_sess_on_hdr() ||
             ds707_extif_is_pkt_data_sess_on_cdma() )
        {
          /*-----------------------------------------------------------------
            Send an Xon on the Service Network stream if the flow is already
            enabled, or if the call is in dormant or active state.
            -----------------------------------------------------------------*/
          DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "DRdy rcvd. Send Xon SN strm:%d",
                         ind_data_ptr->stream_id);
          hdrpac_flow_enable( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
        }
      }
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
      break;

    case HDRIND_LUP_ALN_TRIGGER:
#ifdef FEATURE_HDR_ALN
      /* Initiate a PPP resync if needed */
      ds707_pkt_post_aln_cmd();
#endif /* FEATURE_HDR_ALN */

      break;

#ifdef FEATURE_HDR_QOS
    case HDRIND_SCM_MRLP_GAUP:
      if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        /*---------------------------------------------------------------------
          Process the GAUP notification
        ---------------------------------------------------------------------*/
        hdrmrlpctl_process_rlp_gaup_notification();
      }
      break;

    case HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED:
      /*---------------------------------------------------------------------
        GAUP Request Accepted
      ---------------------------------------------------------------------*/
      if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        DATA_HDR_MSG1 (MSG_LEGACY_MED, "GAUP Accepted for Trans:%d", 
                       ind_data_ptr->gaup_trans_id);

        hdrpacqnm_qos_request_status_notification_cb(
                                                    ind_data_ptr->gaup_trans_id,
                                                    DS707_QOS_GAUP_ACCEPTED );
      }
      break;

    case HDRIND_SCM_MRLP_AT_GAUP_REJECTED:
      /*---------------------------------------------------------------------
        GAUP Request Rejected
      ---------------------------------------------------------------------*/
      if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
          hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
      {
        DATA_HDR_MSG1 (MSG_LEGACY_MED, "GAUP Rejected for Trans:%d", 
                       ind_data_ptr->gaup_trans_id);

        hdrpacqnm_qos_request_status_notification_cb(
                                                    ind_data_ptr->gaup_trans_id,
                                                    DS707_QOS_GAUP_REJECTED );
      }
      break;

    case HDRIND_SCM_MRLP_AT_GAUP_FAILED:
      /*---------------------------------------------------------------------
        GAUP Request Failed transmission
      ---------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        Send the Failure status notification to QoS Negotiation manager 
        This will cleanup the resources if any allocated. 
      ---------------------------------------------------------------------*/
        DATA_HDR_MSG1 (MSG_LEGACY_MED, "GAUP Timed Out for Trans:%d", 
                       ind_data_ptr->gaup_trans_id);

        hdrpacqnm_qos_request_status_notification_cb(
                                                    ind_data_ptr->gaup_trans_id,
                                                    DS707_QOS_GAUP_FAILED );
      break;
#endif /* FEATURE_HDR_QOS */

    case HDRIND_RUP_CONNECTION_INITIATED:
      /*-----------------------------------------------------------------------
      Initialize RLP. This is done early when the TCAM is received rather than 
      when the connection is opened, because data may already be received and 
      discarded by the time the connection opened indication is processed.
      ------------------------------------------------------------------------*/
      hdrpac_mdm_sn_init(); 

      //Add Code to cause all the reservations that have reservation KK Idle state = 2 to go "On"
      hdrcp_rup_continue_connection_setup(HDRHAI_PACKET_APPLICATION_CONTROLLER);
      break;

#ifdef FEATURE_HDR_RL_RATE_INERTIA

    case HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED:
      /*---------------------------------------------------------------------
        RMAC GAUP Request Accepted
      ---------------------------------------------------------------------*/
      DATA_HDR_MSG1 (MSG_LEGACY_MED, "RMAC GAUP Accepted for Trans:%d", 
                     ind_data_ptr->gaup_trans_id);

      /* Enter Critical section... */
      rex_enter_crit_sect(&hdrpac.rp.mode_crit_sec);

      /* Remember when the most recent GAUP transaction initiated through the 
       * API (not from HDRIND_IDLE_CONNECTION_OPENED) is completed.
       */
      if (ind_data_ptr->gaup_trans_id == 
          hdrpac.rp.latest_api_initiated_trans_id)
      {
        hdrpac.rp.latest_api_initiated_trans_id = 
        HDRPAC_RP_NO_OUTSTANDING_CONFIG;
      }

      /* If this indication is not in response to our most recent request 
       * we do not need to do any further processing.  Due to race conditions
       * there may be a small period where things are out of sync, but they
       * will sync back up when the outcome of the "latest" transaction is 
       * finally known.
       */
      if (ind_data_ptr->gaup_trans_id == hdrpac.rp.latest_trans_id)
      {
        /* At this point we know that the AN has the same setting for the 
         * rate parameters that the appliction thinks it has.
         */
        hdrpac.rp.val_an_has = hdrpac.rp.val_app_thinks;

        /* Inform Apps about the GAUP accept here */
        hdrpac_ps_iface_generic_ind(IFACE_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV,
                                    NULL);

        if ( hdrpac.rp.val_an_has == HDRSCMRMAC0_RATE_PARAMS_VT_MODE )
        {
          hdrrmac0_enable_rate_inertia();
        }
      }

      /* Leave Critical section... */
      rex_leave_crit_sect(&hdrpac.rp.mode_crit_sec);

      break;

    case HDRIND_SCM_RMAC_AT_GAUP_REJECTED:
    case HDRIND_SCM_RMAC_AT_GAUP_FAILED:
      /*---------------------------------------------------------------------
        RMAC GAUP Request Rejected/Failed
      ---------------------------------------------------------------------*/
      if (ind_name == HDRIND_SCM_RMAC_AT_GAUP_REJECTED)
      {
        fail_code = PS_HDR_REV0_RATE_INERTIA_REQUEST_REJECTED;
        DATA_HDR_MSG1 (MSG_LEGACY_MED, "GAUP Rejected for Trans:%d", 
                       ind_data_ptr->gaup_trans_id);
      }
      else
      {
        fail_code = PS_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX;
        DATA_HDR_MSG1 (MSG_LEGACY_MED, "GAUP Failed TX for Trans:%d", 
                       ind_data_ptr->gaup_trans_id);
      }

      /* Enter Critical section... */
      rex_enter_crit_sect(&hdrpac.rp.mode_crit_sec);

      /* Remember when the most recent GAUP transaction initiated through the 
       * API (not from HDRIND_IDLE_CONNECTION_OPENED) is completed.
       */
      if (ind_data_ptr->gaup_trans_id == 
          hdrpac.rp.latest_api_initiated_trans_id)
      {
        hdrpac.rp.latest_api_initiated_trans_id = 
        HDRPAC_RP_NO_OUTSTANDING_CONFIG;
      }

      /* If this indication is not in response to our most recent request 
       * we do not need to do any further processing.  Due to race conditions
       * there may be a small period where things are out of sync, but they
       * will sync back up when the outcome of the "latest" transaction is 
       * finally known.
       */
      if (ind_data_ptr->gaup_trans_id == hdrpac.rp.latest_trans_id)
      {
        if (hdrpac.rp.val_app_thinks == HDRSCMRMAC0_RATE_PARAMS_VT_MODE)
        {
          /* When the app gets this notification that the VT rate parameters 
           * request has failed, it will again think the normal params are
           * active.
           */
          hdrpac.rp.val_app_thinks = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;

          hdrpac_ps_iface_generic_ind(IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV, 
                                      &fail_code);
        }
      }

      /* Leave Critical section... */
      rex_leave_crit_sect(&hdrpac.rp.mode_crit_sec);

      break;

    case HDRIND_IDLE_CONNECTION_OPENED:
      /* Enter Critical section... */
      rex_enter_crit_sect(&hdrpac.rp.mode_crit_sec);

      if ((hdrpac.rp.val_app_thinks == HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE) &&
          (hdrpac.rp.val_an_has == HDRSCMRMAC0_RATE_PARAMS_VT_MODE) && 
          (hdrpac.rp.latest_api_initiated_trans_id == 
           HDRPAC_RP_NO_OUTSTANDING_CONFIG))
      {
        (void) hdrscmrmac_prop_gaup_rate_params(
                                               HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE, &hdrpac.rp.latest_trans_id );
      }

      /* The race condition between HDRIND_IDLE_CONNECTION_OPENED and
          hdrpac_enable_vt_rate_inertia is as follows:
  
         1. HDRIND_IDLE_CONNECTION_OPENED when app thinks we are "normal"
            but AN think we are "VT" (no GAUP has been initiated via the API
            at this point)
         2. hdrscmrmac_prop_gaup_rate_params( normal, latest_trans_id = 1 )
         3. hdrpac_enable_vt_rate_inertia() sets 
            latest_api_initiated_trans_id = 2
         4. hdrscmrmac_prop_gaup_rate_params( VT, latest_trans_id = 2 )
         5. HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED ( trans_id = 1 ) ignored
         6. HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED ( trans_id = 2 ) report success
      */

      /* Leave Critical section... */
      rex_leave_crit_sect(&hdrpac.rp.mode_crit_sec);
      break;

#endif /* FEATURE_HDR_RL_RATE_INERTIA */

#if defined (FEATURE_HDR_QOS) && defined (FEATURE_HDR_REVA_L1)
    case HDRIND_RMAC_FLOW_MAPPING_CHANGED:
      hdrpac_process_rmac_flow_mapping_changed_ev();
      break;

    case HDRIND_RMAC_FLOW_DEACTIVATED:
      hdrpacqnm_rtcmac_deactivated_notification (
                                                *(uint8*)(ind_data_ptr) );  
      break;

    case HDRIND_RMAC_FLOW_ACTIVATED:
      hdrpacqnm_rtcmac_activated_notification (
                                              *(uint8*)(ind_data_ptr) );  //TODO: Need to add this parameter to the Union
      break;
#endif /* FEATURE_HDR_QOS && FEATURE_HDR_REVA_L1 */

#ifdef FEATURE_EIDLE_SCI
    case HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED:
      /*---------------------------------------------------------------------
        IDLE GAUP Request Accepted
        ---------------------------------------------------------------------*/
      rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

      /* If this indication is not in response to our most recent request 
       * we do not need to do any further processing.  Due to race conditions
       * there may be a small period where things are out of sync, but they
       * will sync back up when the outcome of the "latest" transaction is 
       * finally known.
       */
      if (ind_data_ptr->gaup_trans_id == hdrpac.sm.latest_trans_id)
      {
        hdrpac.sci_gaup_to_default_in_progress = FALSE;

        /* At this point the AN should have the same slotted mode settings as
         * what the app thinks.
         */
        DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                       "SMGaup Accept: (%d->%d) Completed [TransID %d] [AN actual %d]", 
                       hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                       hdrpac.sm.latest_trans_id, 
                       hdrpac_get_current_max_configured_sci());

        hdrpac.sm.val_an_has = hdrpac.sm.val_app_thinks;

        hdrpac_get_evdo_page_monitor_period(&page_monitor_period);

        /* Inform Apps about the GAUP accept here */
        if ((hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_1280_MSECS) ||
            (hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_426_MSECS) ||
            (hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_213_MSECS))
        {
          hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV,
                                       NULL);
          hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV,
                                       &page_monitor_period );
        }
        /* cancel override successfully*/
        else if (hdrpac.sm.val_an_has == HDRSCMIDLE_SM_OVERRIDE_NONE)
        {
          /* Invalidate the saved sm info becaues override has been canceled */
          hdrcp_scmidle_set_scmidle_info_valid(FALSE);
          /* Notify App */
          hdrpac_ps_iface_generic_ind ( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV,
                                        &page_monitor_period /* SCI NONE */ );
        }
      }
      else
      {
        DATA_HDR_MSG5( MSG_LEGACY_HIGH, 
          "SMGaup Accept: Ignored TransID %d (expecting %d) [an_has %d, app_thinks %d, AN actual %d]",
           ind_data_ptr->gaup_trans_id, hdrpac.sm.latest_trans_id,
           hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
           hdrpac_get_current_max_configured_sci());
      }

      rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);

      break;

    case HDRIND_SCM_IDLE_AT_GAUP_REJECTED:
    case HDRIND_SCM_IDLE_AT_GAUP_FAILED:
      /*---------------------------------------------------------------------
        IDLE GAUP Request Rejected/Failed
        ---------------------------------------------------------------------*/
      rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

      sm_current_sci = hdrpac_get_current_max_configured_sci();

      /* If this indication is not in response to our most recent request 
       * we do not need to do any further processing.  Due to race conditions
       * there may be a small period where things are out of sync, but they
       * will sync back up when the outcome of the "latest" transaction is 
       * finally known.
       */
      if (ind_data_ptr->gaup_trans_id == hdrpac.sm.latest_trans_id)
      {
         hdrpac.sci_gaup_to_default_in_progress = FALSE; 

        if ((hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_1280_MSECS) ||
            (hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_426_MSECS) ||
            (hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_213_MSECS))
        {
          /* When the app gets this notification that the SCI override failed
           * it will again think it has the normal (negotiated) value.
           */
          if (ind_name == HDRIND_SCM_IDLE_AT_GAUP_REJECTED)
          {
            DATA_HDR_MSG6( MSG_LEGACY_HIGH, 
                           "SMGaup Reject: (%d->%d) failed [TransID %d, AN actual %d, app %d->%d]",
                           hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                           hdrpac.sm.latest_trans_id, 
                           sm_current_sci,
                           hdrpac.sm.val_app_thinks, sm_current_sci);
            sm_fail_code = PS_HDR_SLOTTED_MODE_REQUEST_REJECTED;
          }
          else
          {
            DATA_HDR_MSG6( MSG_LEGACY_HIGH, 
                           "SMGaup TX Failed: (%d->%d) failed [TransID %d, AN actual %d, app %d->%d]", 
                           hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                           hdrpac.sm.latest_trans_id, 
                           sm_current_sci,
                           hdrpac.sm.val_app_thinks, sm_current_sci);
            sm_fail_code = PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX;
          }

          /* Invalidate the saved sm info if override failed  and current sm value 
           * is negotiated value
           */
          if (hdrpac.sm.val_an_has != HDRSCMIDLE_SM_OVERRIDE_NONE)
          {
            /* App tries to modify "override" value, e.g from 426ms to 213ms
             * even it failed, saved_scmidle_info is still valid*/
          }
          else
          {
            hdrcp_scmidle_set_scmidle_info_valid(FALSE);
          }

          hdrpac.sm.val_app_thinks = sm_current_sci;

          hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV,
                                &sm_fail_code);
        }
        else
        {
          if (ind_name == HDRIND_SCM_IDLE_AT_GAUP_REJECTED)
          {
            DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                           "SMGaup Reject: (%d->%d) failed [TransID %d, AN actual %d]", 
                           hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                           hdrpac.sm.latest_trans_id, 
                           sm_current_sci);
          }
          else
          {
            DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                           "SMGaup TX Failed: (%d->%d) failed [TransID %d, AN actual %d]", 
                           hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                           hdrpac.sm.latest_trans_id, 
                           sm_current_sci);
          }
        }
      }
      else
      {
        DATA_HDR_MSG5( MSG_LEGACY_HIGH, 
                       "SMGaup Reject/TX Failed: Ignored TransID %d (expecting %d) [an_has %d, app_thinks %d, AN actual %d]",
                       ind_data_ptr->gaup_trans_id, hdrpac.sm.latest_trans_id,
                       hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                       sm_current_sci);
      }
      rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);

      break;
#endif /* FEATURE_EIDLE_SCI */   

    case HDRIND_INIT_NETWORK_ACQUIRED:
#ifdef FEATURE_EIDLE_SCI  
      rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

      /* Because following initialization issue on power up, a flag is needed to initiate
       * hdrpac.sm.val_an_has correctly during first time power up acquisition. 
       * ( hdrpac_power_init() is called before hdrsmp_powerup_init() is completed due to 
       * hdrmc is blocked while calculating sha1 for scmdb. )
       */
      if (!hdrpac.sm_init)
      {
        hdrpac.sm.val_an_has = hdrpac_get_current_max_configured_sci();
        hdrpac.sm_init = TRUE;

        /* for the case of AN negotiated sci <=6, after power cycle, we don't want to
         * change it to OVERRIDE_NONE
         */
        if (!hdrcp_scmidle_saved_scmidle_info_is_valid())
        {
          hdrpac.sm.val_app_thinks = hdrpac_get_current_max_configured_sci();
        }
      }
  
      /* The race condition for VT rate parameters GAUP that required adding 
       * hdrpac.rp.latest_api_initiated_trans_id does not exist for 
       * slotted mode GAUP because the API will immediately fail if called
       * when the AT isn't acqauired, so this event can't occur while a GAUP 
       * is already in progress.
       */
      if ((hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_NONE) &&
          (hdrcp_scmidle_saved_scmidle_info_is_valid()))
      {
        /* Send the AttributeUpdateRequest Message with new SlottedMode parameter. */
        err_no = hdrcp_scmidle_gaup_slotted_mode_attrib( hdrpac.sm.val_app_thinks, 
                                                      &hdrpac.sm.latest_trans_id );

        /* hdrpac.sm.latest_trans_id is not modified unless E_SUCCESS is returned. */

        if (err_no == E_SUCCESS)
        {
          DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                         "SMGaup ACQ: (%d->%d) In Progress [TransID %d, AN actual %d]", 
                         hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                         hdrpac.sm.latest_trans_id, 
                         hdrpac_get_current_max_configured_sci());
          
          hdrpac.sci_gaup_to_default_in_progress = TRUE;
        }
        else
        {
          DATA_HDR_MSG3( MSG_LEGACY_FATAL, 
                         "SMGaup ACQ: (%d->%d) Failed! TX couldn't start [AN actual %d]", 
                         hdrpac.sm.val_an_has, hdrpac.sm.val_app_thinks,
                         hdrpac_get_current_max_configured_sci());
        }
      }

      rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);
#endif /* FEATURE_EIDLE_SCI */
#ifdef FEATURE_HDR_QOS
      /*---------------------------------------------------------------------
      The network acquire indication is given by protocol. In case of RASM
      QUEUE FULL, the protocol forces the system loss and reacquires again.
      The DS is making use of this indication to send QoS release message
      over the air again. This time, all the messages are bundled. Hence, the 
      likelyhood of RASM QUEUE becoming FULL is minimal.
      ---------------------------------------------------------------------*/
      hdrmrlpctl_network_acquired();
#endif /* FEATURE_HDR_QOS */
      break;

#ifdef FEATURE_HDR_QOS
  case HDRIND_LMAC_RF_CONDITION_CHANGED:
    /*---------------------------------------------------------------------
    Convert the HDR RF condition to PS RF condition. So that there is no
    dependency.
    ---------------------------------------------------------------------*/
    ps_rf_condition =PS_IFACE_RF_CONDITIONS_INVALID;
    switch((hdrcom_ind_lmac_rf_change_enum_type) ind_data_ptr->hdr_rf_condition)
    {
      case HDRIND_LMAC_RF_CONDITION_GOOD:
        ps_rf_condition = PS_IFACE_RF_CONDITIONS_GOOD;
        break;
        
      case HDRIND_LMAC_RF_CONDITION_BAD:
       ps_rf_condition = PS_IFACE_RF_CONDITIONS_BAD;
       break;
       
      case HDRIND_LMAC_RF_CONDITION_DONOT_CARE:
        ps_rf_condition = PS_IFACE_RF_CONDITIONS_DONT_CARE;
        break;
        
      default:
        DATA_HDR_MSG1(MSG_LEGACY_HIGH, "Invalid indication %d, Ignore",
             (hdrcom_ind_lmac_rf_change_enum_type) ind_data_ptr->hdr_rf_condition);
        break;
    }
    if(ps_rf_condition != PS_IFACE_RF_CONDITIONS_INVALID)
    {
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, "RF condition changed to %d",
           (hdrcom_ind_lmac_rf_change_enum_type) ind_data_ptr->hdr_rf_condition); 
      client_data.rf_type =ps_rf_condition; 
      ds707_sec_pkt_mgr_handoff_rf_change(HANDOFF_CLIENT_HDR_PROTOCOL, &client_data);
    }
    break;
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_EIDLE_SCI
  case HDRIND_IDLE_AN_GAUP_SLOTTEDMODE:
    rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

    sm_current_sci = hdrpac_get_current_max_configured_sci();

    /* SCI has been changed by AN using GAUP, invalidate scmidle_info*/
    hdrcp_scmidle_set_scmidle_info_valid(FALSE);

    DATA_HDR_MSG4 (MSG_LEGACY_HIGH, 
                   "SMGaup SesChg: (%d->%d) Negotiation Completed [app %d->%d]",
                   hdrpac.sm.val_an_has, sm_current_sci, 
                   hdrpac.sm.val_app_thinks, sm_current_sci);
    hdrpac.sm.val_an_has = sm_current_sci;
    hdrpac.sm.val_app_thinks = sm_current_sci;

    /* Inform Application*/

    DATA_HDR_MSG0( MSG_LEGACY_LOW, "SMGaup Debug: calling ps_iface_generic_ind" );
    hdrpac_get_evdo_page_monitor_period(&page_monitor_period);
    hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SESSION_CHANGED_EV,
                                 &page_monitor_period );
    

    rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);
    break;
#endif /* FEATURE_EIDLE_SCI */

    case HDRIND_OUTAGE_IMMINENT:
      DATA_HDR_MSG2( MSG_LEGACY_HIGH,
                     "Outage imminent in %d ms for %d ms",
                     ind_data_ptr->outage_info.time_to_outage_ms,
                     ind_data_ptr->outage_info.outage_duration_ms);

      ds707_pkt_mgr_process_outage_notification(
        ind_data_ptr->outage_info.time_to_outage_ms,
        ind_data_ptr->outage_info.outage_duration_ms);

     break;

#ifdef FEATURE_DATA_OPTHO
    case HDRIND_IDLE_CONNECTION_OPENED:
    case HDRIND_IDLE_CONNECTION_ALREADY_OPENED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Connection Opened/Already Open: %d",
                    ind_name);
      if (DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state())
      {
        ds707_pkti_tunnel_call_conn_hdlr();
      }
      else
      {
        DATA_HDR_MSG0(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: S101 SM in None State."
                    "Ignoring Connection Opened/Already Open Ind");
      }
      break;

   case HDRIND_IDLE_CONNECTION_FAILED:
   case HDRIND_IDLE_CONNECTION_NOT_ALLOWED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Connection Failed/Not Allowed: %d",
                    ind_name);
      if (DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state())
      {

        ds707_pkti_tunnel_call_failed_hdlr(
          ind_data_ptr->conn_fail_info.failure_reason,
          ind_data_ptr->conn_fail_info.backoff_time);
      }
      else
      {
        DATA_HDR_MSG0(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: S101 SM in None State."
                    "Ignoring Connection Failed/Not Allowed Ind");
      }
      break;

    case HDRIND_CONNECTED_CONNECTION_CLOSED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Connection Closed: %d",
                    ind_name);
      if (DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state())
      {
        /* OPTHO_ToDo: HDR doesn't send the end_reason. So set it
        to be 0 by default so DS reports end reason unspecified to
        upper layers */
        ds707_pkti_tunnel_call_ended_hdlr( DS707_S101_DOWN_REASON_UNSPECIFIED );
      }
      else
      {
        DATA_HDR_MSG0(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: S101 SM in None State."
                    "Ignoring Connection Closed Ind");
      }
      break;

    case HDRIND_OVHD_PREREG_INFO_UPDATED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Prereg info Updated: %d",
                    ind_name);

      ds707_s101_prereg_chg_info(ind_data_ptr->prereg_is_allowed);
      break;

    case HDRIND_SAP_TUNNELMODE_STATUS_CHANGED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Tunnel Mode status changed: %d", 
                    ind_name);

      ds707_s101_tunnel_mode_chg_info(ind_data_ptr->tunnel_mode_value);
      break;

    case HDRIND_AMP_IRMI_ACCEPTED:
      ds707_s101_irmi_response(TRUE);
      break;

    case HDRIND_AMP_IRMI_FAILED:
      ds707_s101_irmi_response(FALSE);
      break;

    case HDRIND_OVHD_COLOR_CODE_CHANGED:
      DATA_HDR_MSG2(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: Color code changed: %d, "
                    "new color code value: %d", 
                    ind_name, ind_data_ptr->color_code);
      ds707_s101_process_color_code_chg(ind_data_ptr->color_code);
      break;

     case HDRIND_RUP_ACT_HO_COMPLETED:
       /* Though the indication name says act_ho_completed, DS treats it as
          actv_ho still in progress since we haven't got HDR full service from
          CM */
       ds707_s101_set_hdr_actv_ho_in_prog();
       break;

     case HDRIND_ALMP_EXIT_HDR_MODE:
       /* If we get exit HDR while in active ho in progress, it means active ho
          failed and HDR got deactivated */
       if (ds707_s101_is_hdr_actv_ho_in_prog())
       {
         ds707_s101_set_hdr_actv_ho_failed();
       }
       break;

#endif /* FEATURE_DATA_OPTHO */


    case HDRIND_ALMP_RESELECT_EXIT_HDR:
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, 
                  "PAC: HDRIND: ALMP reselect exit HDR");
      event_info.event = DS_DSD_CLIENT_RADIO_HANDDOWN_EV;
      event_info.data.hand_down.source_radio = DS_DSD_RADIO_HDR;
      event_info.data.hand_down.target_radio = DS_DSD_RADIO_1X;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, 
                                &event_info);
      break;
    case HDRIND_LMAC_ORAT_MEAS_FAILED:
      DATA_HDR_MSG1(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: LMAC IRAT measurements failed"
                    " Fail Reason: %d", ind_data_ptr->irat_meas_fail_reason); 
      dscam_stop_flow_timer((dscam_cam_d2l_redir_enum_type)ind_data_ptr->irat_meas_fail_reason);    
      break;
    case HDRIND_LMAC_ORAT_REDIR_SUCCESS:
      DATA_HDR_MSG0(MSG_LEGACY_HIGH, 
                    "PAC: HDRIND: LMAC IRAT measurements succeeded"); 
      dscam_stop_flow_timer(DS_CAM_D2L_REDIR_SUCCESS);    
      break;
    default:
      /*---------------------------------------------------------------------
      Default: Indication not recognized.
      ---------------------------------------------------------------------*/
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "PAC: Unrecognized Indication: %d",
                    ind_name);
      break;
  }
} /* hdrpac_process_ind() */

/*===========================================================================
FUNCTION HDRPAC_APP_FLOW_IS_ENABLED

DESCRIPTION
  This function returns whether the stream for an RLP instance is in
  Open or Closed state.

  The difference between this and hdrpac_flow_is_enabled is in the parameter
  it takes as input. hdrpac_flow_is_enabled takes RLP ID, whereas this
  takes the app subtype. If the app_subtype is specified as PacketAppAny,
  then the function returns true if any of the streams have flow enabled.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If the stream corresponding to the RLP instance is Open
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrpac_app_flow_is_enabled
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  uint8 i;
  hdrcom_hai_stream_enum_type stream_id; /* Stream corresponding the RLP       */
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( app_subtype == HDRHAI_APP_PACKET_APP_ANY )
  {
    /*-----------------------------------------------------------------------
      If the App Subtype is specified as any, check flow
      on every stream that has been negotiated during configuration
    -----------------------------------------------------------------------*/
    for(i=0; i<HDRPAC_MAX_STREAMS;i++)
    {
      if(hdrpac.map_table[i].flow_enabled == TRUE)
      {
        ret_val = TRUE;
      }
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Determine which stream flow is being queried
    -----------------------------------------------------------------------*/
    stream_id = hdrpac_get_stream_for_app_subtype( app_subtype );

    /*-----------------------------------------------------------------------
      If the RLP is a valid one, query flow for it
    -----------------------------------------------------------------------*/
    if (stream_id != HDRHAI_STREAM0)
    {
      ret_val = hdrpac.map_table[stream_id].flow_enabled;
    }
    else
    {
      DATA_HDR_MSG1 (MSG_LEGACY_ERROR, "Inval query - strm:%d", stream_id);
    }
  }

  DATA_HDR_MSG2 (MSG_LEGACY_MED, 
                 "hdrpac_app_flow_is_enabled: App_subtype: %d, Enabled: %d",
                 app_subtype, ret_val);

  return(ret_val);
} /* hdrpac_app_flow_is_enabled() */

/*===========================================================================
FUNCTION HDRPAC_FLOW_ENABLE

DESCRIPTION
  This function enables flow on the current stream by commanding the Flow
  Control Protocol to send an Xon message for that stream.

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be enabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_flow_enable
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  uint8 i;
  hdrcom_hai_stream_enum_type stream_id; /* Stream corresponding the RLP       */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( app_subtype == HDRHAI_APP_PACKET_APP_ANY )
  {
    /*-----------------------------------------------------------------------
      If the App Subtype is specified as any, then the Xon message is sent
      on every stream that has been negotiated during configuration
    -----------------------------------------------------------------------*/
    for(i=0; i<HDRPAC_MAX_STREAMS;i++)
    {
      switch (hdrpac.map_table[i].app_subtype) 
      {
        case HDRHAI_DEF_PACKET_APP_AN:
        case HDRHAI_DEF_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS
        case HDRHAI_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_MULTIFLOW_PACKET_APP_SN:
        case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
#ifdef FEATURE_HDR_REVB
        case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
        DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "Sending XON Req Message. Stream:%d",
                       i);
        hdrcp_fcp_send_xon_req( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                             (hdrcom_hai_stream_enum_type)i ); /*lint !e64*/

        hdrpac.map_table[i].flow_enabled = TRUE;
          break;

        default:
          /* nothing to do */
          break;
      }
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Determine which stream the Xon message is to be sent
    -----------------------------------------------------------------------*/
    stream_id = hdrpac_get_stream_for_app_subtype( app_subtype );

    /*-----------------------------------------------------------------------
      If the RLP is a valid one, enable flow for it
    -----------------------------------------------------------------------*/
    if (stream_id != HDRHAI_STREAM0)
    {
      DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "Sending XON Req Message. Stream:%d",
                     stream_id);
      hdrcp_fcp_send_xon_req( HDRHAI_PACKET_APPLICATION_CONTROLLER, stream_id );

      hdrpac.map_table[stream_id].flow_enabled = TRUE;
    }
    else
    {
      DATA_HDR_MSG1 (MSG_LEGACY_ERROR, "Cant Xon Stream:%d",stream_id);
    }
  }
} /* hdrpac_flow_enable() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRPAC_FLOW_DISABLE

DESCRIPTION
  This function disables flow on the current stream by commanding the Flow
  Control Protocol to send a Xoff message for that stream.

DEPENDENCIES
  None

PARAMETERS
   app_subtype - Application Subtype whose flow is to be disabled

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_flow_disable
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  uint8 i;
  hdrcom_hai_stream_enum_type stream_id; /* Stream corresponding the RLP       */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( app_subtype == HDRHAI_APP_PACKET_APP_ANY )
  {
    /*-----------------------------------------------------------------------
      If the App Subtype is specified as any, then the Xoff message is sent
      on every stream that has been negotiated during configuration
    -----------------------------------------------------------------------*/
    for(i=0; i<HDRPAC_MAX_STREAMS;i++)
    {
      switch (hdrpac.map_table[i].app_subtype) 
      {
        case HDRHAI_DEF_PACKET_APP_AN:
        case HDRHAI_DEF_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS
        case HDRHAI_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_MULTIFLOW_PACKET_APP_SN:
        case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
#ifdef FEATURE_HDR_REVB
        case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
        case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
        DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "Sending XOFF Req Message. Stream:%d",
                       i);
        hdrcp_fcp_send_xoff_req( HDRHAI_PACKET_APPLICATION_CONTROLLER,
                              (hdrcom_hai_stream_enum_type)i );  /*lint !e64*/

        hdrpac.map_table[i].flow_enabled = FALSE;
          break;

        default:
          /* nothing to do */
          break;
      }
    }
  }
  else
  {
    stream_id = hdrpac_get_stream_for_app_subtype( app_subtype );

    /*-----------------------------------------------------------------------
     If the stream is a valid one, disable flow for it
    -----------------------------------------------------------------------*/
    if (stream_id != HDRHAI_STREAM0)
    {
      DATA_HDR_MSG1 (MSG_LEGACY_HIGH, "Sending XOFF Req Message. Stream:%d",
                     stream_id);
      hdrcp_fcp_send_xoff_req( HDRHAI_PACKET_APPLICATION_CONTROLLER,stream_id );

      hdrpac.map_table[stream_id].flow_enabled = FALSE;
    }
    else
    {
      DATA_HDR_MSG1 (MSG_LEGACY_ERROR, "Cant Xoff Stream:%d",stream_id);
    }

  }
} /* hdrpac_flow_disable() */

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
hdrcom_hai_stream_enum_type hdrpac_get_stream_for_app_subtype
(
hdrcom_hai_app_subtype_enum_type app_subtype
)
{
  int i = 1;
  hdrcom_hai_stream_enum_type stream_id = HDRHAI_STREAM0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (i=1;i<HDRPAC_MAX_STREAMS;i++)
  {
    if (hdrpac.map_table[i].app_subtype == app_subtype)
    {
      stream_id = hdrpac.map_table[i].stream_id;
      break;
    }
  }
  return stream_id;
} /* hdrpac_get_stream_for_app_aubtype() */
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
)

{
  hdrerrno_enum_type error_no;
  uint8              i;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( subnet == NULL )
  {
    DATA_HDR_MSG0( MSG_LEGACY_ERROR, 
                   "HDRPAC: get_subnetid invalid input parameter" );
    return E_INVALID_ARG;
  }

  DATA_HDR_MSG0( MSG_LEGACY_HIGH, 
                 "HDRPAC: get_subnetid" );

  error_no = hdrcp_ovhd_get_subnet(subnet,
                                HDRHAI_PACKET_APPLICATION_CONTROLLER);

  if ( error_no == E_SUCCESS )
  {
    for ( i = 0; i < HDRADDR_LENGTH_IN_BYTES; i++ )
    {
      DATA_HDR_MSG1( MSG_LEGACY_HIGH, "subnet: 0x%x", subnet[i++] );
    }
  }
  else 
  {
    DATA_HDR_MSG1(MSG_LEGACY_ERROR,
                  "HDR PAC: failed to get subnetid from HDRCP, "
                  "error_no: %d", error_no);
  }
  return error_no;
} /* hdrpac_get_stream_for_app_aubtype() */
#endif /*FEATURE_DATA_OPTHO*/
/*===========================================================================
FUNCTION HDRPAC_PROCESS_SESSION_CLOSED

DESCRIPTION
  This function processes a session closed notification
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_process_session_closed( void )
{
  /* Initialize variables to keep track of Rate Parameters setting */
  hdrpac.rp.val_app_thinks = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  hdrpac.rp.val_an_has = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  hdrpac.rp.latest_api_initiated_trans_id = 
  HDRPAC_RP_NO_OUTSTANDING_CONFIG;

  /*---------------------------------------------------------------------
  Notify the PAC DS If that the session has closed.
  ---------------------------------------------------------------------*/
#ifndef FEATURE_DEDICATED_C2K_MODE_DATA
  hdrpacdsif_post_smp_session_closed_cmd_to_ds();
#endif

  ds707_pkt_clear_pri_tc_rlp( );

  /*---------------------------------------------------------------------
  Need to update the stream mapping. This is dependant on the Stream
  protocol processing the indication before PAC.
  ---------------------------------------------------------------------*/
  hdrpac_map_app_to_stream(HDRHAI_SCHG_CLOSED_NORMAL); 

  /*------------------------------------------------------------------------------------------------
   Make the last bearer tech to invalid when session is closed. This 
   will be used when the bearer tech changed notification is given 
   the next time.
  ------------------------------------------------------------------------------------------------*/
  ds707_pkt_bearer_tech_reset();
} /* hdrpac_process_session_closed() */

/*===========================================================================
FUNCTION HDRPAC_MAP_APP_TO_STREAM

DESCRIPTION
  This function gets the negotiated Stream Configuration from the Stream
  Protocol. It then maps the RLP instance associated with an application
  subtype to the appropriate stream. It calls RLP functions to register
  the appropriate callback function with PCP and and also activates/
  deactivates the Test App functions.

  The function is called when a new session is opened, a session is closed,
  session is renegotiated or if a personality switch occurs. This function
  determines if the SN multiflow App subtype has changed from from DPA to 
  MFPA or vice-versa, in case of a personality switch.

  The function also gives appropriate notifications to various entities
  (AN manager, MFPA control module, etc) regarding the session status.

DEPENDENCIES
  None

PARAMETERS
  session_change reason - Informs the function of the session change reason.
                   This allows the function to give appropriate notifications
                   to entities regarding this.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_map_app_to_stream
(
hdrcom_hai_session_change_reason_enum_type session_change_reason
)
{
  uint8 i; /* index counter */
  hdrcom_hai_app_subtype_enum_type negotiated_app_subtype[HDRPAC_MAX_STREAMS];
  /* Contains the negotiated Application Subtype for each stream. The
  index of the array is the Stream for which the Application subtype has
  been negotiated. */
  boolean prev_session_had_dpa_sn = FALSE;
#ifdef FEATURE_HDR_QOS
  boolean prev_session_had_mpa_sn = FALSE;
  /* These flags keep track of whether the previous session contained the
     Default Packet App or the Multiflow Packet App on the SN stream. This 
     is used in order to determine if a personality switch has occured and 
     to give appropriate indication to the Packet App */
  boolean prev_session_had_empa_sn = FALSE;

#ifdef FEATURE_HDR_REVB
  boolean prev_session_had_mmpa_sn = FALSE;
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_QOS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Get the configuration information mapping Application Subtypes to
  Streams from the stream protocol */
  if ( !hdrcp_stream_get_stream_config( &negotiated_app_subtype[0] ) )
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR,
                  "Stream Configuration for App Subtypes not negotiated");
    return;
  }

  /*-----------------------------------------------------------------------
   Note that this function has 3 loops for going through each stream.
   Though the loops could be consolidated, they have been kept separate
   to avoid possible inconstencies. The first for loops goes through all
   the streams and deregisters callbacks/deactivates test app if the
   configuration for this has changed.
   The second loop assigns the stream mapping.
   The 3rd loop registers callbacks/activates the test app based on the
   new stream configuration.
   The separate loops allow that there is no inconsistency in the mapping
   table when any of the Macros/function are used to determine the
   stream/application instance/application subtype from the mapping table.
  -----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
   Go through the current stream mapping. If the mapping has changed then
   do the necessary cleanup for that stream.
  -----------------------------------------------------------------------*/
  for (i=0; i<HDRPAC_MAX_STREAMS;i++)
  {
    /*-----------------------------------------------------------------------
    The index of the array is the Stream for which the Application subtype
    has been negotiated. For example, the array element 0 of
    negotiated_app_subtype contains the the App subtype for Stream 0, array
    element 1 contains the App subtype for Stream 1,etc. Same goes for the
    mapping table. The Stream enumeration matches the index.
    eg. HDRHAI_STREAM0 has value 0
    -----------------------------------------------------------------------*/
    hdrpac.map_table[i].stream_id = (hdrcom_hai_stream_enum_type) i;

    /*-----------------------------------------------------------------------
    This function gets called when the Session gets Reconfigured. Check the
    previous stream configuration for the Test App and deactivate the Test
    App on that stream.
    -----------------------------------------------------------------------*/
    if (( hdrpac.map_table[i].app_subtype == HDRHAI_DEF_PACKET_APP_IS890 ) ||
        ( hdrpac.map_table[i].app_subtype == HDRHAI_ENH_TEST_APP_IS890A  ) || 
        ( hdrpac.map_table[i].app_subtype == HDRHAI_MC_TEST_APP_IS890B   ) )
    {
#if defined( FEATURE_HDR_IS890 )
      hdrcp_is890_deactivate( i );
#else
      DATA_HDR_MSG0(MSG_LEGACY_ERROR, "IS-890 not supported in this build");
#endif /* FEATURE_HDR_IS890 */
    }
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_DEF_PACKET_APP_AN )
    {
      dshdr_an_mgr_session_close( hdrpac.map_table[i].stream_id );
      hdrpac.map_table[i].flow_enabled = FALSE;
    }
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_DEF_PACKET_APP_SN )
    {
      prev_session_had_dpa_sn = TRUE;
      hdrpac.map_table[i].flow_enabled = FALSE;
    }
#ifdef FEATURE_HDR_QOS
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_SN)
    {
      prev_session_had_mpa_sn = TRUE;
      hdrpac.map_table[i].flow_enabled = FALSE;      
      if ( session_change_reason == HDRHAI_SCHG_CLOSED_NORMAL )
      {
        hdrmrlpctl_process_session_change( DS707_QOS_SESSION_CLOSED,
                                           HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                           HDRHAI_MULTIFLOW_PACKET_APP_SN);
        ds707_pkt_bearer_tech_changed_notification();

      }
    }
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN )
    {
      prev_session_had_empa_sn = TRUE;
      hdrpac.map_table[i].flow_enabled = FALSE;      
      if ( session_change_reason == HDRHAI_SCHG_CLOSED_NORMAL )
      {
        hdrmrlpctl_process_session_change( DS707_QOS_SESSION_CLOSED,
                                           HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                           HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
        ds707_pkt_bearer_tech_changed_notification();

      }
    }
#ifdef FEATURE_HDR_REVB
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN )
    {
      prev_session_had_mmpa_sn = TRUE;
      hdrpac.map_table[i].flow_enabled = FALSE;      
      if ( session_change_reason == HDRHAI_SCHG_CLOSED_NORMAL )
      {
        hdrmrlpctl_process_session_change( DS707_QOS_SESSION_CLOSED,
                                           HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                           HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
        ds707_pkt_bearer_tech_changed_notification();

      }
    }
#endif /* FEATURE_HDR_REVB */
    else if ( hdrpac.map_table[i].app_subtype == HDRHAI_MULTIFLOW_PACKET_APP_AN ||
              hdrpac.map_table[i].app_subtype == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN ||
              hdrpac.map_table[i].app_subtype == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN )
    {
      hdrpac.map_table[i].flow_enabled = FALSE;
    }
#endif /* FEATURE_HDR_QOS */
  }

  /*-----------------------------------------------------------------------
    Assign the new stream mapping and perform the necessary initializations
  -----------------------------------------------------------------------*/
  for (i=0; i<HDRPAC_MAX_STREAMS;i++)
  {
    /* Assign the Application Subtype for the stream */
    hdrpac.map_table[i].app_subtype = negotiated_app_subtype[i];
  }

  for (i=0; i<HDRPAC_MAX_STREAMS;i++)
  {
    DATA_HDR_MSG2 (MSG_LEGACY_MED, "Str:%d app_subtype:%d",
                   i,hdrpac.map_table[i].app_subtype);

    /* The Stream bound to the Service Network is marked as the current
    Application on which data transfer can take place */
    switch ( hdrpac.map_table[i].app_subtype )
    {
      case HDRHAI_DEF_PACKET_APP_AN:
        dshdr_an_mgr_session_init( hdrpac.map_table[i].stream_id );
        break;

      case HDRHAI_MULTIFLOW_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS
        /* If the change reason indicates that this is  personality switch and the
           previous session contained Default Packet App, it means that a
           switch has occured from a personality containing DPA to a personality
           containing MFPA. Appropriate action, such as notifying the App of a 
           handoff needs to occur.
           If the switch occured from a personality containing MPA to another
           containing MPA, then we assume that these MPAs are hardlinked, and this
           session change is ignored. Note that the RMAC may have changed, so
           ignoring this indication may not be the right thing to do. Need to
           revisit when designing for switch from MPA to MPA
            */

        switch (session_change_reason)
        {
          case HDRHAI_SCHG_OPENED_NORMAL:
            /* Whenever there is a new session or a session reconfiguration, we get a session open indication from the protocol layer.
            In order to process this indication correctly, we see if there was an existing session. If it is a session open
            due to reconfig, we handle it similar to a personlaity switch. Otherwise we handle it like a session open */
            if(TRUE == prev_session_had_dpa_sn)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->MPA");
              hdrmrlpctl_process_session_change(DS707_QOS_SESSION_PERSONALITY_SWITCHED,
                                                HDRHAI_DEF_PACKET_APP_SN,
                                                HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->MPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->MPA");  
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mmpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->MPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_OPENED, 
                                               HDRHAI_APP_STREAM_NOT_USED,
                                               HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            break;
          case HDRHAI_SCHG_CLOSED_NORMAL:
            /*---------------------------------------------------------------
            Already processed this reason. No need to print any message.
            ---------------------------------------------------------------*/
            break;  
          case HDRHAI_SCHG_CLOSED_RESTORE_PRIOR:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR:
            DATA_HDR_MSG1(MSG_LEGACY_HIGH, "Ignoring session_change_reason :%d", session_change_reason);
            break;
          
          case HDRHAI_SCHG_PERSONALITY_SWITCH:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS:
            if (prev_session_had_dpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->MPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_OPENED, 
                                                 HDRHAI_DEF_PACKET_APP_SN,
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->MPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->MPA");  
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if ( prev_session_had_mmpa_sn == TRUE )
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->MPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid switching");
            }
            break;
          default:
            DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid reason :%d", session_change_reason);
            break;
        }

#endif /* FEATURE_HDR_QOS */
        break;
      case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS        
        switch (session_change_reason)
        {
          case HDRHAI_SCHG_OPENED_NORMAL:
            /* Whenever there is a new session or a session reconfiguration, we get a session open indication from the protocol layer.
            In order to process this indication correctly, we see if there was an existing session. If it is a session open
            due to reconfig, we handle it similar to a personlaity switch. Otherwise we handle it like a session open */
            if(TRUE == prev_session_had_dpa_sn)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->EMPA");
              hdrmrlpctl_process_session_change(DS707_QOS_SESSION_PERSONALITY_SWITCHED,
                                                HDRHAI_DEF_PACKET_APP_SN,
                                                HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                   HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->EMPA");  
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mmpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_OPENED, 
                                               HDRHAI_APP_STREAM_NOT_USED,
                                               HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            break;
          case HDRHAI_SCHG_CLOSED_NORMAL:
            /*---------------------------------------------------------------
            Already processed this reason. No need to print any message.
            ---------------------------------------------------------------*/
            break;
          case HDRHAI_SCHG_CLOSED_RESTORE_PRIOR:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR:
            DATA_HDR_MSG1(MSG_LEGACY_HIGH, "Ignoring session_change_reason :%d", session_change_reason);
            break;

          
          case HDRHAI_SCHG_PERSONALITY_SWITCH:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS:
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch");
            if (prev_session_had_dpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_DEF_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mmpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->EMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid switching");
            }
            break;
          default:
            DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid reason :%d", session_change_reason);
            break;
        }
#endif /* FEATURE_HDR_QOS */
        break;

      case HDRHAI_DEF_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS
        /* If the previous session contained Default Packet App, it means that a
           switch has occured from a personality containing MFPA to a personality
           containing DPA. Appropriate action, such as notifying the App of a 
           handoff needs to occur */

        if ((session_change_reason == HDRHAI_SCHG_PERSONALITY_SWITCH) ||
            (session_change_reason == HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS)||
            (session_change_reason == HDRHAI_SCHG_OPENED_NORMAL))
            /* HDRHAI_SCHG_OPENED_NORMAL is handled just like a personlity switch if there was a previous session.
            This session open is given to us only when there is a session reconfiguration */
        {
          if (prev_session_had_dpa_sn == TRUE)
          {
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->DPA");
            hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                               HDRHAI_DEF_PACKET_APP_SN,
                                               HDRHAI_DEF_PACKET_APP_SN);
            ds707_pkt_bearer_tech_changed_notification();

          }
          if (prev_session_had_mpa_sn == TRUE)
          {
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->DPA");
            hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                               HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                               HDRHAI_DEF_PACKET_APP_SN);
            ds707_pkt_bearer_tech_changed_notification();

          }
          else if (prev_session_had_empa_sn == TRUE)
          {
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->DPA");
            hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                               HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                               HDRHAI_DEF_PACKET_APP_SN);
            ds707_pkt_bearer_tech_changed_notification();

          }
          else if (prev_session_had_mmpa_sn == TRUE)
          {
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->DPA");
            hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                               HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                               HDRHAI_DEF_PACKET_APP_SN);
            ds707_pkt_bearer_tech_changed_notification();

          }
          else
          {
            DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid session change");
          }
        }
#endif /* FEATURE_HDR_QOS */
        break;

      case HDRHAI_DEF_SIG_APP:
        /* Nothing else to do, but it's a valid case */
        break;

      case HDRHAI_MULTIFLOW_PACKET_APP_AN:
      case HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_AN:
      case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_AN:
        DATA_HDR_MSG0(MSG_LEGACY_ERROR, "AN MRLP not supported in this build");
        break;

      case HDRHAI_DEF_PACKET_APP_IS890:
      case HDRHAI_ENH_TEST_APP_IS890A:
      case HDRHAI_MC_TEST_APP_IS890B:
#if defined( FEATURE_HDR_IS890 )
        hdrcp_is890_activate( i );
#else
        DATA_HDR_MSG0(MSG_LEGACY_ERROR, "IS-890 not supported in this build");
#endif /* FEATURE_HDR_IS890 */
        break;

      case HDRHAI_APP_STREAM_NOT_USED:
        break;

#ifdef FEATURE_HDR_REVB
      case HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN:
#ifdef FEATURE_HDR_QOS        
        switch (session_change_reason)
        {
          case HDRHAI_SCHG_OPENED_NORMAL:
            /* Whenever there is a new session or a session reconfiguration, we get a session open indication from the protocol layer.
            In order to process this indication correctly, we see if there was an existing session. If it is a session open
            due to reconfig, we handle it similar to a personlaity switch. Otherwise we handle it like a session open */
            if(TRUE == prev_session_had_dpa_sn)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->MMPA");
              hdrmrlpctl_process_session_change(DS707_QOS_SESSION_PERSONALITY_SWITCHED,
                                                HDRHAI_DEF_PACKET_APP_SN,
                                                HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                   HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                   HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->MMPA");  
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mmpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_OPENED, 
                                               HDRHAI_APP_STREAM_NOT_USED,
                                               HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            break;
          case HDRHAI_SCHG_CLOSED_NORMAL:
            /*---------------------------------------------------------------
            Already processed this reason. No need to print any message.
            ---------------------------------------------------------------*/
            break;
          case HDRHAI_SCHG_CLOSED_RESTORE_PRIOR:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR:
            DATA_HDR_MSG1(MSG_LEGACY_HIGH, "Ignoring session_change_reason :%d", session_change_reason);
            break;
          
          case HDRHAI_SCHG_PERSONALITY_SWITCH:
          case HDRHAI_SCHG_OPENED_RESTORE_PRIOR_AND_PS:
            DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch");
            if (prev_session_had_dpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from DPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_DEF_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_empa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from EMPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else if (prev_session_had_mmpa_sn == TRUE)
            {
              DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Personality switch from MMPA->MMPA");
              hdrmrlpctl_process_session_change( DS707_QOS_SESSION_PERSONALITY_SWITCHED, 
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN,
                                                 HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN);
              ds707_pkt_bearer_tech_changed_notification();

            }
            else
            {
              DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid switching");
            }
            break;

          default:
            DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid reason :%d", session_change_reason);
            break;
        }
#endif /* FEATURE_HDR_QOS */
        break;
#endif /* FEATURE_HDR_REVB */
//VIP HAck End

      default:
        DATA_HDR_MSG1(MSG_LEGACY_ERROR, "Invalid app subtype %d found",
                      hdrpac.map_table[i].app_subtype);
        break;
    }
  }
  return;
} /* hdrpac_map_app_to_stream() */

/* <EJECT> */

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
void hdrpac_set_an_auth_status
(
boolean auth_succeeded
)
{
  hdrcp_hmp_set_an_auth_status( auth_succeeded,
                             HDRHAI_PACKET_APPLICATION_CONTROLLER );
} /* hdrpac_set_an_auth_status() */

/* <EJECT> */

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
  None
===========================================================================*/
void hdrpac_process_cmd
(
hdrpac_cmd_type *pac_cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -save -e641 supress enum to int warning 641*/
  HDRCOM_TRACE_CMD( HDRHAI_PACKET_APPLICATION_CONTROLLER, pac_cmd_ptr->name,
                pac_cmd_ptr->sender, 0 );
  /*lint -restore Restore lint error 641*/

  /*-------------------------------------------------------------------------
  Determine which command has been received.
  -------------------------------------------------------------------------*/
  
  switch ( pac_cmd_ptr->name )
  {
	

    case HDRPAC_INIT_CMD:
      /*---------------------------------------------------------------------
      Command to initialize module
      ---------------------------------------------------------------------*/
      hdrpac_process_init_cmd();
      break;

    case HDRPAC_SN_IFACE_UP_CMD:
    case HDRPAC_SN_IFACE_COMING_UP_CMD:
    case HDRPAC_SN_IFACE_ROUTEABLE_CMD:
    case HDRPAC_SN_IFACE_DOWN_CMD:
    case HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD:
    case HDRPAC_SESSION_CLOSED_CMD:
    case HDRPAC_CLEAR_SN_PPP_STATUS_CMD:
      hdrpacdsif_process_cmd( pac_cmd_ptr );
      break;

    default:
      /*---------------------------------------------------------------------
      Unrecognized command.
      ---------------------------------------------------------------------*/
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "PAC: Received Unrecognized command:%d",
                    pac_cmd_ptr->name);
      break;
  
  }
} /* hdrpac_process_cmd() */

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
void hdrpac_start_high_priority_traffic_mode( void )
{

  /* We need to tell HDRMC to enter high priority traffic mode */
  hdrcp_rup_set_traffic_mode( HDRSRCH_S1XHDR_HP_TC_MODE,
                           HDRHAI_PACKET_APPLICATION_CONTROLLER );

} /* hdrpac_start_high_priority_traffic_mode() */


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
void hdrpac_stop_high_priority_traffic_mode( void )
{

  /* We need to tell HDRMC to leave high priority traffic mode */
  hdrcp_rup_set_traffic_mode( HDRSRCH_DEFAULT_TC_MODE,
                           HDRHAI_PACKET_APPLICATION_CONTROLLER );

} /* hdrpac_stop_high_priority_traffic_mode() */

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
void hdrpac_enable_1x_handdown_mode( void )
{ 
  if(ds707_roaming_is_curr_mode_jcdma()||
     ds707_roaming_is_simplified_jcdma_mode())
  {
    DATA_HDR_MSG0 (MSG_LEGACY_MED,"1x handdown is not allowed in JCDMA");
    return;
  }
  
  DATA_HDR_MSG0 (MSG_LEGACY_MED,"hdrpac_enable_1x_handdown_mode");
  hdrcp_rup_set_tc_handdown_mode(HDRSRCH_1X_HANDDOWN_ENABLED_MODE,
                              HDRHAI_PACKET_APPLICATION_CONTROLLER);
} /* hdrpac_enable_1x_handdown_mode() */

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
void hdrpac_disable_1x_handdown_mode( void )
{
  DATA_HDR_MSG0 (MSG_LEGACY_MED,"hdrpac_disable_1x_handdown_mode");
  hdrcp_rup_set_tc_handdown_mode(HDRSRCH_1X_HANDDOWN_DISABLED_MODE,
                              HDRHAI_PACKET_APPLICATION_CONTROLLER);
} /* hdrpac_disable_1x_handdown_mode() */

/* <EJECT> */
/*===========================================================================
FUNCTION HDRPAC_SET_VT_RATE_INERTIA

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  E_SUCCESS       - If the GAUP Update for VT RateParams was sent out 
                    successfully or if the InConfig RP Mode was already VT.
  E_NOT_SUPPORTED - If the current subtype of RMAC was not ST-1.
  E_NOT_AVAILABLE - If we don't have a free Rsp timer to send a GAUP update.
  E_NO_NET        - The HDR protocol was not idle or connected
  
SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrpac_set_vt_rate_inertia
(
boolean enable_vt_rate_inertia
)
{
  
#ifdef FEATURE_HDR_RL_RATE_INERTIA

  hdrerrno_enum_type err_no = E_SUCCESS; /* Error code */
  ps_hdr_rev0_rate_inertia_failure_code_enum_type fail_code;
  ps_hdr_rev0_rate_inertia_failure_code_enum_type *fail_code_ptr = NULL;

  hdrscmrmac0_rate_params_mode_enum_type rp_mode_to_propose = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
  boolean                   propose_gaup   = FALSE;/* Assume no GAUP */
  boolean                   report_outcome = TRUE; /* Assume we will report */
  boolean                   hdr_sys_is_available;  /* If HDR is acquired */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter the critical section... */
  rex_enter_crit_sect(&hdrpac.rp.mode_crit_sec);

  hdr_sys_is_available = 
  hdrcp_almp_system_is_acquired( HDRHAI_PACKET_APPLICATION_CONTROLLER );

  if (hdrcp_scp_get_current_subtype( HDRHAI_RTC_MAC_PROTOCOL ) != 
      HDRSCP_SUB1_RMAC)
  {
    fail_code = PS_HDR_REV0_RATE_INERTIA_NOT_SUPPORTED;
    fail_code_ptr = &fail_code;
    err_no = E_NOT_SUPPORTED;
  }
  else if (enable_vt_rate_inertia && !hdr_sys_is_available)
  {
    /* Note that we allow disabling rate intertia without an HDR system */

    fail_code = PS_HDR_REV0_RATE_INERTIA_NO_NET;
    fail_code_ptr = &fail_code;
    err_no = E_NO_NET;
  }

  /* We keep track of what the app thinks the current rate parameters setting
   * is.  If the app is requesting a setting it already thinks it has, then
   * all we need to do is return "success".
   */
  if (err_no == E_SUCCESS)
  {
    if (enable_vt_rate_inertia && 
        (hdrpac.rp.val_app_thinks == HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE ))
    {
      propose_gaup = TRUE;
      rp_mode_to_propose = HDRSCMRMAC0_RATE_PARAMS_VT_MODE;
    }
    else if (!enable_vt_rate_inertia)
    {
      if (!hdr_sys_is_available)
      {
        /* Just assume success and note that we are out of sync with AN */
        hdrpac.rp.val_app_thinks = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
      }
      else if (hdrpac.rp.val_app_thinks == HDRSCMRMAC0_RATE_PARAMS_VT_MODE )
      {
        propose_gaup = TRUE;
        rp_mode_to_propose = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
      }
    }
  }
  else if ((err_no == E_NOT_SUPPORTED) && 
           (!enable_vt_rate_inertia) && 
           (hdrpac.rp.val_app_thinks != HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE))
  {
    /* The AT enabled the vt rate inertia then switched personalities so RMAC1 is 
    * no longer configured.  Remember the app’s intentions and return E_SUCCESS.  We will
    * fulfill the request if/when we switch back to RMAC1.
    */
    err_no = E_SUCCESS;
    fail_code_ptr = NULL;
    hdrpac.rp.val_app_thinks = HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE;
    DATA_HDR_MSG0( MSG_LEGACY_HIGH, "SMGaup Set: Allow canceling override while RMAC1 is not configured");
  }

  if (propose_gaup)
  {
    /* Send the AttributeUpdateRequest Message with new Rate parameters.
     * hdrpac.rp.latest_trans_id is not modified unless E_SUCCESS is returned.
     */
    err_no = hdrscmrmac_prop_gaup_rate_params(
                                             rp_mode_to_propose, &hdrpac.rp.latest_trans_id );  /*lint !e644*/

    if (err_no == E_SUCCESS)
    {
      /* We were able to send the GAUP message.  In the case where we are
       * proposing VT, we still update what the "app thinks" here, in case
       * the app proposes normal again before we report the status of the
       * VT request.  If we report to the app that the VT request failed,
       * then we update what the "app thinks" again at that point.
       */

      hdrpac.rp.val_app_thinks = rp_mode_to_propose;


      /* The race condition between hdrpac_enable_vt_rate_inertia and
         HDRIND_IDLE_CONNECTION_OPENED is as follows:

         1. hdrpac_enable_vt_rate_inertia()
         2. hdrscmrmac_prop_gaup_rate_params()
         3. latest_api_initiated_trans_id = latest_trans_id
         4. HDRIND_IDLE_CONNECTION_OPENED - latest_api_initiated_trans_id
            is not "HDRPAC_RP_NO_OUTSTANDING_CONFIG", which indicates that
            we are in the process of chaning the VT parameters to VT, so
            we avoid incorrectly trying to GAUP the rate params back to 
            "normal" if HDRIND_IDLE_CONNECTION_OPENED occurs before the 
            outcome of the VT proposal is known.
         5. HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED - latest_api_initiated_trans_id
            set back to "HDRPAC_RP_NO_OUTSTANDING_CONFIG"
      */   

      hdrpac.rp.latest_api_initiated_trans_id = hdrpac.rp.latest_trans_id;

      if (enable_vt_rate_inertia)
      {
        /* If we are trying to enable VT, we wait for the outcome of the 
         * GAUP message before reporting the out come to the app.
         */
        report_outcome = FALSE;
      }
      else
      {
        /* If we are disabling VT, we assume success and then if it fails
         * we keep track if we are out of sync.  This is necessary because
         * the app will close and forget about the VT parameters state no
         * matter what the outcome.
         */
        hdrrmac0_disable_rate_inertia();
      }
    }
    else
    {
      /* We failed to send the GAUP message */
      fail_code = PS_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX;
      fail_code_ptr = &fail_code;
    }
  }

  if (report_outcome)
  {
    /* Inform the application of the failure */   

    if (fail_code_ptr == NULL)
    {
      hdrpac_ps_iface_generic_ind(IFACE_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV, 
                                  fail_code_ptr); 
    }
    else
    {
      hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV, 
                                   fail_code_ptr); /* NULL ptr means "success" */
    }
  }

  /* Leave the Critical section... */
  rex_leave_crit_sect(&hdrpac.rp.mode_crit_sec);

  return err_no;
#else
  DATA_MDM_ARG_NOT_USED(enable_vt_rate_inertia);
  return E_NOT_SUPPORTED;
#endif /* FEATURE_HDR_RL_RATE_INERTIA */


} /* hdrpac_set_vt_rate_inertia() */

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
  E_NO_NET        - The HDR protocol was not idle or connected
  
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrpac_enable_vt_rate_inertia( void )
{
  return(hdrpac_set_vt_rate_inertia( TRUE ));
} /* hdrpac_enable_vt_rate_inertia */

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
  E_NO_NET        - The HDR protocol was not idle or connected
   
SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrpac_disable_vt_rate_inertia( void )
{
  return(hdrpac_set_vt_rate_inertia( FALSE ));
} /* hdrpac_disable_vt_rate_inertia */


#if defined (FEATURE_HDR_QOS) && defined (FEATURE_HDR_REVA_L1)
/*===========================================================================
FUNCTION HDRPAC_PROCESS_RMAC_FLOW_MAPPING_CHANGED_EV

DESCRIPTION 
    This function is called whenever HDRPAC gets an indication 
    conveying that the RLP <-> RMAC Mapping has changed. 

    When this function is called, we get the unbound and bound list from 
    RMAC and if it has only been unbound then we cleanup. If it has been 
    then we cleanup and then initialize. 
    If Default RLP has been changed, then irrespective of what happens
    we cleanup and initialise the rlp.

DEPENDENCIES
    None.
    
PARAMETERS
    None.
  
RETURN VALUE
    None. 
    
SIDE EFFECTS
    If an RLP <-> RMAC  is changed then cleanup/initialization procedures are
    performed on that particular RLP.

===========================================================================*/
static void hdrpac_process_rmac_flow_mapping_changed_ev(void)
{

  hdrl1_rmac_flow_mapping_struct_type unbound_list[HDRL1_RMAC_MAX_NUM_MAC_FLOWS];
  hdrl1_rmac_flow_mapping_struct_type bound_list[HDRL1_RMAC_MAX_NUM_MAC_FLOWS];
  hdrcom_hai_stream_enum_type qos_stream_id;
  dsrlp_enhanced_ver_enum_type enhanced;
  uint8 curr_mac_flow;
  uint8 curr_rlp_flow;
  uint8 rscb_index;

  /* Array to remember what operations have been done on the RLP */  
  uint8 rlp_status[DSRLP_MAX_HDR_MRLP_FLOW_NUMBER];

  /* Masks to convey what has been done to the RLP */  
  uint8 rlp_init_state = 0x00;
  uint8 rlp_unbound = 0x01;
  uint8 rlp_bound   = 0x02;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the rlp status */  
  for ( curr_rlp_flow = 0; 
      curr_rlp_flow < DSRLP_MAX_HDR_MRLP_FLOW_NUMBER; 
      curr_rlp_flow++ )
  {
    rlp_status[curr_rlp_flow] = rlp_init_state;
  }


  hdrl1_rmac_get_changed_flow_mapping(unbound_list, bound_list);
  if (hdrcp_stream_stream_is_assigned(HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    qos_stream_id = hdrpac_get_stream_for_app_subtype( HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN );
    enhanced = DSRLP_ENHANCED_VER_MMPA;
  }
  else if (hdrcp_stream_stream_is_assigned(HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN))
  {
    qos_stream_id = hdrpac_get_stream_for_app_subtype( HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN );
    enhanced = DSRLP_ENHANCED_VER_EMPA;
  }
  else
  {
    qos_stream_id = hdrpac_get_stream_for_app_subtype( HDRHAI_MULTIFLOW_PACKET_APP_SN );
    enhanced = DSRLP_VER_MFPA;
  }

  for (curr_mac_flow=0; curr_mac_flow<HDRL1_RMAC_MAX_NUM_MAC_FLOWS; curr_mac_flow++)
  {
    for (curr_rlp_flow=0; curr_rlp_flow<unbound_list[curr_mac_flow].flow_count; curr_rlp_flow++)
    {
      if ( unbound_list[curr_mac_flow].app_flows[curr_rlp_flow].stream_id == (uint16)qos_stream_id )
      {
        hdrpacqnm_rlp_unbound_notification (
                                           unbound_list[curr_mac_flow].app_flows[curr_rlp_flow].sub_stream_id,
                                           DSRLP_VER_HDR_MULTIFLOW_REVERSE,
                                           unbound_list[curr_mac_flow].mac_flow_id );

        /* Update the status to show that the RLP was unbound */  
        rlp_status[unbound_list[curr_mac_flow].app_flows[curr_rlp_flow].sub_stream_id] |= rlp_unbound;     
      }
    }
  }  


  for (curr_mac_flow=0; curr_mac_flow<HDRL1_RMAC_MAX_NUM_MAC_FLOWS; curr_mac_flow++)
  {
    for (curr_rlp_flow=0; curr_rlp_flow<bound_list[curr_mac_flow].flow_count; curr_rlp_flow++)
    {
      if ( bound_list[curr_mac_flow].app_flows[curr_rlp_flow].stream_id == (uint16)qos_stream_id )
      {
        hdrpacqnm_rlp_bound_notification (
                                         bound_list[curr_mac_flow].app_flows[curr_rlp_flow].sub_stream_id,
                                         DSRLP_VER_HDR_MULTIFLOW_REVERSE,
                                         bound_list[curr_mac_flow].mac_flow_id );

        /* Update the status to show that the RLP was bound */  
        rlp_status[bound_list[curr_mac_flow].app_flows[curr_rlp_flow].sub_stream_id] |= rlp_bound;     
      }
    }
  }  



  /* We need to either cleanup/initialize the RLP only if MRLP is initialised */  
  if ( hdrmrlpctl_is_mrlp_initialised() == TRUE )
  {
    for ( curr_rlp_flow = 0; 
        curr_rlp_flow < DSRLP_MAX_HDR_MRLP_FLOW_NUMBER; 
        curr_rlp_flow++ )
    {
      rscb_index = dsrlp_get_rlp_rscb_index(
                                           curr_rlp_flow,
                                           DSRLP_VER_HDR_MULTIFLOW_REVERSE ); 

      if ( rscb_index == DSRLPI_INVALID_RSCB_INDEX )
      {
        continue;
      }

      /* Special Handling for DEFAULT RLP */
      if ( rscb_index == DSRLP_HDR_DEFAULT_MRLP_REV_INDEX )
      {
        if ( rlp_status[ curr_rlp_flow ] != rlp_init_state )
        {
          dsrlp_cleanup (
                        curr_rlp_flow, 
                        DSRLP_VER_HDR_MULTIFLOW_REVERSE );                 

          dsrlp_init( rscb_index,
                      curr_rlp_flow, 
                      DSRLP_VER_HDR_MULTIFLOW_REVERSE,
                      FALSE,  
                      enhanced );
        }
      }
      else
      {
        /* Act only if the RLP Status has been changed */
        if ( rlp_status[ curr_rlp_flow ] != rlp_init_state )
        {
          /* cleanup if it has only been unbound */
          if ( rlp_status[ curr_rlp_flow ]  == rlp_unbound )
          {
            DATA_HDR_MSG2 (MSG_LEGACY_MED,"RLP:%d ver:%d was unbound. ",
                           curr_rlp_flow,
                           DSRLP_VER_HDR_MULTIFLOW_REVERSE );  

            dsrlp_cleanup (
                          curr_rlp_flow, 
                          DSRLP_VER_HDR_MULTIFLOW_REVERSE );                 
          }
          /* cleanup and initialize if it has been bound or rebound */
          else
          {
            DATA_HDR_MSG2 (MSG_LEGACY_MED,"RLP:%d ver:%d was bounded/rebinded. ",
                           curr_rlp_flow,
                           DSRLP_VER_HDR_MULTIFLOW_REVERSE );  

            dsrlp_cleanup (
                          curr_rlp_flow, 
                          DSRLP_VER_HDR_MULTIFLOW_REVERSE );                 
            dsrlp_init( rscb_index,
                        curr_rlp_flow, 
                        DSRLP_VER_HDR_MULTIFLOW_REVERSE,  
                        FALSE,
                        enhanced );
          }
        } /* if rlp state was changed */
      }
    } /* for each rlp */
  } /* if mrlp is initialized */

} /* hdrpac_process_rmac_flow_mapping_changed_ev() */
#endif /* FEATURE_HDR_QOS && FEATURE_HDR_REVA_L1 */

/*===========================================================================
  FUNCTION HDRPAC_SET_EIDLE_SLOTTED_MODE
 
  DESCRIPTION
    This is wrapped by hdrpac_change_eidle_slotted_mode.  See that 
    function header for details.
 
  DEPENDENCIES
 
  PARAMETERS
   
  RETURN VALUE
   
  SIDE EFFECTS
 
  ===========================================================================*/
hdrerrno_enum_type hdrpac_set_eidle_slotted_mode
(
hdrcp_scmidle_sm_override_enum_type requested_sm_override
)
{
#ifdef FEATURE_EIDLE_SCI

  hdrerrno_enum_type err_no = E_SUCCESS; /* Error code */
  ps_hdr_slotted_mode_failure_code_enum_type sm_fail_code = PS_HDR_SLOTTED_MODE_NOT_SUPPORTED;
  ps_hdr_slotted_mode_failure_code_enum_type *sm_fail_code_ptr = NULL;
  boolean                   propose_gaup   = FALSE;/* Assume no GAUP */
  boolean                   report_outcome = TRUE; /* Assume we will report */
  boolean                   hdr_sys_is_available;  /* If HDR is acquired */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect(&hdrpac.sm.mode_crit_sec);

  /********************/
  /* Check for errors */
  /********************/

  hdr_sys_is_available = 
  hdrcp_almp_system_is_acquired( HDRHAI_PACKET_APPLICATION_CONTROLLER );


  if (hdrcp_scp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL ) != HDRSCP_ENH_IDLE)
  {
    DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                   "SMGaup Set: (%d->%d) attempt failed, EIDLE not negotiated [app %d->%d]",
                   hdrpac.sm.val_an_has, requested_sm_override,
                   hdrpac.sm.val_app_thinks, requested_sm_override );

    sm_fail_code = PS_HDR_SLOTTED_MODE_NOT_SUPPORTED;
    sm_fail_code_ptr = &sm_fail_code;
    err_no = E_NOT_SUPPORTED;
  }
  else if (requested_sm_override >= HDRSCMIDLE_SM_MAX_NUM_SCIS)
  {
    DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                   "SMGaup Set: (%d->%d) attempt failed, Unsupported SCI requested [app %d->%d]",
                   hdrpac.sm.val_an_has, requested_sm_override,
                   hdrpac.sm.val_app_thinks, requested_sm_override );

    sm_fail_code = PS_HDR_SLOTTED_MODE_NOT_SUPPORTED;
    sm_fail_code_ptr = &sm_fail_code;
    err_no = E_OUT_OF_RANGE;
  }
  else if ((requested_sm_override != HDRSCMIDLE_SM_OVERRIDE_NONE) &&
           !hdr_sys_is_available)
  {
    /* Note that we allow canceling the override without an HDR system */

    DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                   "SMGaup Set: (%d->%d) attempt failed, Not acquired [app %d->%d]",
                   hdrpac.sm.val_an_has, requested_sm_override,
                   hdrpac.sm.val_app_thinks, requested_sm_override );

    sm_fail_code = PS_HDR_SLOTTED_MODE_NO_NET;
    sm_fail_code_ptr = &sm_fail_code;
    err_no = E_NO_NET;
  }
  else if( !hdrcp_scmidle_saved_scmidle_info_is_valid() && 
           hdrpac_get_current_max_configured_sci() != HDRSCMIDLE_SM_OVERRIDE_NONE )
  {
    /* If negotiated sm value is <= 6, override is not supported*/

    DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                   "SMGaup Set: (%d->%d) attempt failed, Small SCI negotiated [app %d->%d]",
                   hdrpac.sm.val_an_has, requested_sm_override,
                   hdrpac.sm.val_app_thinks, requested_sm_override );

    sm_fail_code = PS_HDR_SLOTTED_MODE_NOT_SUPPORTED;
    sm_fail_code_ptr = &sm_fail_code;
    err_no = E_NOT_SUPPORTED;
  }
  else if (hdrpac.sci_gaup_to_default_in_progress &&
           (requested_sm_override != HDRSCMIDLE_SM_OVERRIDE_NONE))
  {
    /* If GAUP of SCI 9 is in progress we immediately return TX failure 
       for SCI overrides. This is because AT does not allow more than
       1 outstanding GAUP. However as a special case AT does allow 
       GAUP of SCI 9 (reset to default) when there is an outstanding 
       GAUP */
     DATA_HDR_MSG2( MSG_LEGACY_HIGH, 
                    "SMGaup Set: (%d->%d) attempt failed, SCI 9 GAUP in progress",
                    hdrpac.sm.val_app_thinks, requested_sm_override );
 
     sm_fail_code = PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX;
     sm_fail_code_ptr = &sm_fail_code;
     err_no = E_NOT_SUPPORTED;
  }

  /********************/
  /* Determine if we  */
  /* should send GAUP */
  /********************/

  /* We keep track of what the app thinks the current override setting
   * is.  If the app is requesting a setting it already thinks it has, then
   * all we need to do is return "success".
   */
  if (err_no == E_SUCCESS)
  {
    if ((requested_sm_override != HDRSCMIDLE_SM_OVERRIDE_NONE) &&
        (requested_sm_override != hdrpac.sm.val_app_thinks) &&
        (requested_sm_override != hdrpac.sm.val_an_has))
    {
      /* App is trying it initiate an override or change the override value
       * e.g. 213ms->426ms
       */
      propose_gaup = TRUE;
    }
    else if (requested_sm_override == HDRSCMIDLE_SM_OVERRIDE_NONE)
    {
      /* App is trying to cancel the override */
      if (!hdr_sys_is_available)
      {
        /* Just assume success for now, and sync back up with the AN when we 
         * re-acquire 
         */
        DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                       "SMGaup Set: (%d->%d) pending, [app %d->%d]",
                       hdrpac.sm.val_an_has, requested_sm_override,
                       hdrpac.sm.val_app_thinks, requested_sm_override );
        hdrpac.sm.val_app_thinks = HDRSCMIDLE_SM_OVERRIDE_NONE;
      }
      else if ((hdrpac.sm.val_app_thinks != HDRSCMIDLE_SM_OVERRIDE_NONE) ||
               ((hdrpac.sm.val_app_thinks == HDRSCMIDLE_SM_OVERRIDE_NONE) &&
                (hdrpac.sm.val_an_has != HDRSCMIDLE_SM_OVERRIDE_NONE)))
      {
        /* Condition1: Try to cancel the override right now */
        /* Condition2: (Unlikely to happen)
         *   An_has 426/213 but app_thinks == NONE, and app tries to cancel  
         *   For example: AT tried to go back to OVERRIDE_NONe but failed (returned 
         *   E_SUCCESS ), and App tries to calcel override again. 
         *   For example: 
         *      - cancel override(426) when not acquire,
         *      - after acquisition, an_has == smaller sci(426), but app_thinks == OVERRIDE_NONE
         *      - AT sent GAUP(OVERRIDE_NONe) but tx failed 
         *      - if App cancels override(426) again now, send GAUP. 
         */
        propose_gaup = TRUE;
      }
      else
      {
        /* The app is trying to cancel the override when there is no override */
        DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                       "SMGaup Set: (%d->%d) attempt is not needed, no change requested [app %d->%d]",
                       hdrpac.sm.val_an_has, requested_sm_override,
                       hdrpac.sm.val_app_thinks, requested_sm_override );
      }
    }
    else
    {
      /* Case1: The app is trying to initiate, change the override and has requested the same 
       *   override value it already thinks is in use.  Just report success but don't send 
       *   another GAUP.
       * Case2: An_has 426/213 but app_thinks == OVERRIDE_NONE, if app tries to propose smaller sci again, 
       *        don't need to send GAUP.
       *   For example: 
       *      - cancel override(426) when not acquire,
       *      - after acquisition, an_has == smaller sci(426), but app_thinks == OVERRIDE_NONE
       *      - AT sent GAUP(OVERRIDE_NONe) but tx failed 
       *      - if App starts override(426) again now, Don't send GAUP. 
       */
      DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                     "SMGaup Set: (%d->%d) attempt is not needed, no change requested [app %d->%d]",
                     hdrpac.sm.val_an_has, requested_sm_override,
                     hdrpac.sm.val_app_thinks, requested_sm_override );

      /* following is only required for case2 but it won't change val_app_thinks for case1 */
      hdrpac.sm.val_app_thinks = requested_sm_override;
    }
  }
  else if ((err_no == E_NOT_SUPPORTED) && 
           (requested_sm_override == HDRSCMIDLE_SM_OVERRIDE_NONE) && 
           (hdrcp_scmidle_saved_scmidle_info_is_valid()))
  {
    /* The AT enabled the override then switched personalities so Enhanced Idle State is 
    * no longer configured.  Remember the app’s intentions and return E_SUCCESS.  We will
    * fulfill the request if/when we switch back to Enhanced Idle State.
    */
    err_no = E_SUCCESS;
    sm_fail_code_ptr = NULL;
    hdrpac.sm.val_app_thinks = HDRSCMIDLE_SM_OVERRIDE_NONE;
    DATA_HDR_MSG0( MSG_LEGACY_HIGH, "SMGaup Set: Allow canceling override while Default Idle is configured");
  }

  if (propose_gaup)
  {
    /*************/
    /* Send GAUP */
    /*************/

    /* Send the AttributeUpdateRequest Message with new SlottedMode parameter. */
    err_no = hdrcp_scmidle_gaup_slotted_mode_attrib( requested_sm_override, 
                                                  &hdrpac.sm.latest_trans_id );

    /* hdrpac.sm.latest_trans_id is not modified unless E_SUCCESS is returned. */

    if (err_no == E_SUCCESS)
    {
      /* We were able to send the GAUP message.  In the case where we are
       * proposing OVERRIDE, we still update what the "app thinks" here, in case
       * the app proposes to cancel the override before we report the status of 
       * this request.  If we report to the app that the override request failed,
       * then we update what the "app thinks" again at that point.
       */

      DATA_HDR_MSG6( MSG_LEGACY_HIGH, 
                     "SMGaup Set: (%d->%d) In Progress [TransID %d, AN actual %d, app %d->%d]",
                     hdrpac.sm.val_an_has, requested_sm_override,
                     hdrpac.sm.latest_trans_id, 
                     hdrpac_get_current_max_configured_sci(),
                     hdrpac.sm.val_app_thinks, requested_sm_override);

      hdrpac.sm.val_app_thinks = requested_sm_override;

      if (requested_sm_override != HDRSCMIDLE_SM_OVERRIDE_NONE)
      {
        /* If we are trying to initiate an override, we wait for the outcome of 
         * the GAUP message before reporting the outcome to the app.
         */
        report_outcome = FALSE;
      }
      else
      {
        hdrpac.sci_gaup_to_default_in_progress = TRUE;
      }
    }
    else
    {
      DATA_HDR_MSG4( MSG_LEGACY_HIGH, 
                     "SMGaup Set: (%d->%d) attempt failed, TX couldn't start [app %d->%d]",
                     hdrpac.sm.val_an_has, requested_sm_override,
                     hdrpac.sm.val_app_thinks, requested_sm_override );
      sm_fail_code = PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX;
      sm_fail_code_ptr = &sm_fail_code;
    }
  }

  if (report_outcome)
  {


    if (sm_fail_code_ptr == NULL)
    {
      hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV,
                            NULL); /* NULL ptr means "success" */
    }
    else
    {
      hdrpac_ps_iface_generic_ind( IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV,
                            sm_fail_code_ptr);
    }
  }

  rex_leave_crit_sect(&hdrpac.sm.mode_crit_sec);
  return err_no;
#else
  DATA_MDM_ARG_NOT_USED(requested_sm_override);
  return E_NOT_SUPPORTED;
#endif /* FEATURE_EIDLE_SCI */

} /* hdrpac_set_eidle_slotted_mode() */  


/*===========================================================================
  FUNCTION HDRPAC_CHANGE_EIDLE_SLOTTED_MODE
  
  DESCRIPTION
  This interface is used to force the Enhanced Idle State protocol to send 
  a AttributeUpdateRequest message using the Generic Attribute Update 
  Protocol (GAUP) to change the SlottedMode configuration attribute.  This 
  attribute is used by the AN to determine when it can send a page to the AT.
  This is analogous to the Slot Cycle Index (SCI) used in IS-2000.

  This interface should be called in the context of the PS task as a result
  of a socket IOCTL.

  This interface provides 3 options:
  1 - Override the current SlottedMode configuration to force the AT to wake
      up every 426ms.
  2 - Override the current SlottedMode configuration to force the AT to wake
      up every 213ms.
  0 - Cancel the override and restore the SlottedMode configuration to what 
      it was before.
  
  If the caller is attempting to start an override or change the override 
  value (i.e. requested_sm_override > 0) and this function returns a value 
  other than E_SUCCESS, the caller should assume that no chnage was made to 
  the SlottedMode configuration.

  If the caller is attempting to cancel the override (i.e. 
  requested_sm_override == 0) this function will always return E_SUCCESS.
  The caller should assume the SlottedMode configuration is changed back to 
  what it was before the override.
  
  DEPENDENCIES
  none.
  
  PARAMETERS
  requested_sm_override:  0 = cancel override (restore original SCI)
                          1 = override SCI to 426ms
                          2 = override SCI to 213ms
    
  RETURN VALUE
  E_SUCCESS       - The requested_sm_override is accepted and the 
                    SlottedMode attribute has a) been GAUPed as requested,
                    b) was already set as requested or c) will be GAUPed ASAP.
  E_FAILURE       - The GAUP message could not be transmitted.
  E_NOT_SUPPORTED - requested_sm_override is not supported becuase 
                    a) Idle State protocol subtype not EIDLE or
                    b) the requested value is out of range.
  E_NO_NET        - The HDR protocol was not idle or connected at the time
                    the attempt was made to initiate an override.
    
  SIDE EFFECTS

  The IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV event is reported
  to PDN_INSTANCE with the following event_data depending on the 
  outcome, as follows:

  If GAUP was sent, the event is given later, when the GAUP outcome is known:
  GAUP_ACCEPT     - Report IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV
  GAUP_REJECT     - PS_HDR_SLOTTED_MODE_REQUEST_REJECTED
  GAUP_FAILED     - PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX

  If GAUP was not sent, the event is given immediately and reflects the errno
  value returned by this function:
  E_SUCCESS       - Report IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV
  E_FAILURE       - PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX
  E_NOT_SUPPORTED - PS_HDR_SLOTTED_MODE_NOT_SUPPORTED
  E_NO_NET        - PS_HDR_SLOTTED_MODE_NO_NET
  
===========================================================================*/
hdrerrno_enum_type hdrpac_change_eidle_slotted_mode
(
uint32 requested_sm_override 
)
{
#ifdef FEATURE_EIDLE_SCI
  return(hdrpac_set_eidle_slotted_mode(
                                      (hdrcp_scmidle_sm_override_enum_type) requested_sm_override ));
#else
  DATA_MDM_ARG_NOT_USED(requested_sm_override);
  return E_NOT_SUPPORTED;
#endif /* FEATURE_EIDLE_SCI */
} /* hdrpac_change_eidle_slotted_mode */

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
void hdrpac_lte_attach_success_ind(void)
{
  hdrcp_scp_lte_is_attached();
} /* hdrpac_lte_attach_success_ind() */
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
)
{
#ifdef FEATURE_EHRPD
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  hdrcp_scp_set_data_ehrpd_capability(data_ehrpd_capability);
#else
  hdrcp_scp_set_ehrpd_apns_are_valid(data_ehrpd_capability);
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */
} /* hdrpac_set_data_ehrpd_capability() */

/*===========================================================================

FUNCTION HDRPAC_SET_FORCE_LONG_SLEEP

DESCRIPTION
  This function sets force_long_sleep flag.
  It is a wrapper function of hdrcp_idle_set_force_long_sleep().
 
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
)
{
  hdrcp_idle_set_force_long_sleep(force_long_sleep);
} /* hdrpac_set_force_long_sleep */

/*===========================================================================

FUNCTION HDRPAC_GET_FORCE_LONG_SLEEP

DESCRIPTION
  This function gets the current value of force_long_sleep flag.
  It is a wrapper function of hdrcp_idle_get_force_long_sleep().
 
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
boolean hdrpac_get_force_long_sleep( void )
{
  return hdrcp_idle_get_force_long_sleep();
} /* hdrpac_get_force_long_sleep */

/*===========================================================================

FUNCTION HDRPAC_GET_EVDO_PAGE_MONITOR_PERIOD

DESCRIPTION
  This function gets the evdo page monitor period, including force_long_sleep
  flag value and current maximum slot cycle index.

DEPENDENCIES
  None

PARAMETERS
  page_monitor_period_ptr - pointer that page monitor period info will be copied
                        to.

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_get_evdo_page_monitor_period
( 
  hdrpac_page_monitor_period_info_type *page_monitor_period_ptr 
)
{
  hdrcp_idlec_slotted_mode_type slotted_mode_attrib;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == page_monitor_period_ptr)
  {
    DATA_HDR_MSG0( MSG_LEGACY_ERROR, "page_monitor_period_ptr is NULL");
    return;
  }

  page_monitor_period_ptr->force_long_sleep = hdrpac_get_force_long_sleep();

  if ( hdrcp_idlec_get_slotted_mode_attrib(&slotted_mode_attrib) )
  {
    page_monitor_period_ptr->slot_cycle_index = slotted_mode_attrib.slot_cycle3;
  }
  else 
  {
    /* With default IDLE state, sleep cycle index is invalid */
    page_monitor_period_ptr->slot_cycle_index = 0xfe;
  }
} /* hdrpac_get_page_monitor_period() */

/*===========================================================================

FUNCTION HDRPAC_REGISTER_FOR_IFACE_EVENTS

DESCRIPTION
  This function registers a callback which calls when an iface event is 
  generated.

DEPENDENCIES
  None

PARAMETERS
  fn_ptr - pointer that holds the callback for iface events generated

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrpac_register_for_iface_events
( 
  hdrpac_iface_events_cb *fn_ptr 
)
{
#ifdef FEATURE_EIDLE_SCI
  iface_events_cb_ptr = fn_ptr;
#endif /* FEATURE_EIDLE_SCI */
}

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
)
{
 return hdrcp_sap_get_current_tunnel_mode_value
                          (HDRHAI_PACKET_APPLICATION_CONTROLLER);
} /* hdrpac_get_current_tunnel_mode */

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
)
{
  return hdrcp_ovhd_prereg_is_allowed
                          (HDRHAI_PACKET_APPLICATION_CONTROLLER);
}

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
)
{
  DATA_HDR_MSG0( MSG_LEGACY_HIGH, 
                 "Notifying HDR to send IRMI");
  return hdrcp_amp_send_irmi_msg(HDRHAI_PACKET_APPLICATION_CONTROLLER);
} /* hdrpac_send_irmi */

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
)
{
  DATA_HDR_MSG0 (MSG_LEGACY_HIGH, "Initiating Tunnel Call via HDR");

  hdrcp_almp_open_connection(HDRHAI_OPEN_USER_REQ, 
                          HDRHAI_PACKET_APPLICATION_CONTROLLER);
  return;

} /* hdrpac_open_tunnel_connection */

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
)
{
  DATA_HDR_MSG0 (MSG_LEGACY_HIGH, "Hanging up Tunnel Call via HDR");

  hdrcp_almp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE, 
                           HDRHAI_PACKET_APPLICATION_CONTROLLER);
  return;

} /* hdrpac_close_tunnel_connection */

/*===========================================================================
FUNCTION HDRPAC_SET_PREREG_PPP_STATUS

DESCRIPTION
  This function calls hdrcp_sap_set_prereg_status() to notify HDR about any
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
)
{
  if (ppp_up)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "HDRPAC: Sending "
                    "HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_OPENED to HDR");
    hdrcp_sap_set_prereg_status(HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_OPENED);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "HDRPAC: Sending "
                    "HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_CLOSED to HDR");
    hdrcp_sap_set_prereg_status(HDRHAI_PREREG_STATUS_EVENT_PPP_SESSION_CLOSED);
  }

  return;
} /* hdrpac_set_prereg_ppp_status */

/*===========================================================================
FUNCTION HDRPAC_SET_PREREG_VSNCP_STATUS

DESCRIPTION
  This function calls hdrcp_sap_set_prereg_status() to notify HDR about any
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
)
{
  if (vsncp_up)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "HDRPAC: Sending "
                    "HDRHAI_PREREG_STATUS_EVENT_VSNCP_ESTABILISHED to HDR");
    hdrcp_sap_set_prereg_status(HDRHAI_PREREG_STATUS_EVENT_VSNCP_ESTABILISHED);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "HDRPAC: Sending "
                    "HDRHAI_PREREG_STATUS_EVENT_VSNCP_RELEASED to HDR");
    hdrcp_sap_set_prereg_status(HDRHAI_PREREG_STATUS_EVENT_VSNCP_RELEASED);
  }

} /* hdrpac_set_prereg_vsncp_status  */


#endif /* FEATURE_DATA_OPTHO */

