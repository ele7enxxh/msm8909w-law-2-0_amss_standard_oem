/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         S E S S I O N   C O N F I G U R A T I O N   P R O T O C O L

GENERAL DESCRIPTION
  The Session Configuration Protocol (SCP) is the main state machine
  controlling the session layer of the HDR protocol stack.

EXTERNALIZED FUNCTIONS (Global)

  hdrscp_msg_cb()      - Callback registered with HMP to deliver
                              messages to SCP
  hdrscp_ind_cb()   - Callback registered for delivery of indications
  hdrscp_activate()        - Command function to activate SCP
  hdrscp_deactivate()      - Command function to deactivate SCP
  hdrscp_powerup_init() - Powerup Initiaization command

  hdrscp_get_session_config_token - Accessor function to return the
                                    Session Config Token
  hdrscp_get_current_subtype - Accessor function to get the current (InUse)
                               subtype of a requested protocol
  hdrscp_get_config_subtype - Accessor function to get the negotiated
                              (InConfig) subtype of a requested protocol

EXTERNALIZED FUNCTIONS (Regional)
  hdrscp_process_cmd()     - Function called by hdrmc task to process
                              a command for SCP
  hdrscp_process_ind()     - Function called by hdrmc task to process
                              an indication for SCP
  hdrscp_process_msg()     - Function called by hdrmc task to process
                              a message for SCP
  hdrscp_process_sig()     - Function called by hdrmc task to process
                              a signal for SCP
  hdrscp_open_to_at_init_state - Function to perform processing for
                                 transition from Open to AT Init State
  hdrscp_an_init_to_open_state - Function to perform processing for
                                 transition from AN Init to Open State
  hdrscp_enter_open_state - Function to perform processing for transition
                            into Open State

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Session Management Protocol must be initialized before any other
  1xEV-DO Protocols which have session-related data.

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrscp.c_v   1.62   24 Apr 2003 11:42:18   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/16/16   vko     Reduce excessive F3
11/28/15   vko     Do not reset ehrpd credential during mcfg refresh
09/21/15   vko     Delay session token change during connection setup 
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
02/17/15   ljl     Disabled Rev B mode.
11/18/14   vko     Do not close ehrpd session immediately during hotswap
09/05/14   cnx     Always register USIM subscription ready event.
07/09/14   cnx     Support NV refresh without reset.
05/09/14   vko     Support to handle bad NV read gracefully
01/15/14   vko     Fixed compiler warnings
01/21/14   vko     Moved calling hdrsap_set_prereg_status under FEATURE_LTE_TO_HDR_OH
02/05/14   sju     Added check that AN should send SCC only for restore 
                   prior session with more than 1 personality.
01/18/14   vko     Fixed not sending pre-reg status when NV67252 is 1
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/01/13   smd     Used new secapi to get random number.
06/27/13   cnx     Set awaiting_slp_callback to FALSE after AT enters AN_INIT 
                   in hdrscp_complete_tx_outcome_cb().
03/21/13   cnx     Write NV when device is powerdown/reset.
03/19/13   cnx     Stop processing OVHD if NAI changes.
03/19/13   cnx     Send SessionClose when both NAI and eHRPD capability change
03/08/13   cnx     Do not write NV_HDRSCP_SESSION_STATUS_I if NV value not changed.
03/07/13   cnx     Set eHRPD credentials to false when USIM card is removed.
03/01/13   cnx     Do not write NV_HDRSCP_SESSION_STATUS_I if NV value not changed.
11/24/12   ukl     Fix Klocwork issues
09/20/12   cnx     Save session to EFS when powerdown in tunnel.
08/23/12   cnx     Do not send HDRIND_SCP_NO_PREV_SESSION if negotitate PCCC only.
08/11/12   wsh     Defined QPCH feature, but disable by NV
08/07/12   cnx     Handle hardlinked subtypes when checking IRAT personality.
06/28/12   cnx     Reset default subtypes if AT exits tunnel without session
06/26/12   kss     Added ftm check to hdrscp_get_config_subtype.
06/15/12   smd     Removed inclusion of hdrpac.h.
06/01/12   cnx     Fixed additional buffer overflow when using protocol name enum.  
04/06/12   cnx     Force to close HPRD session after LTE attach.
03/20/12   cnx     Fixed compiler errors for Dime.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
02/27/12   smd     Added updates of FTM msgr interface changes.
02/24/12   cnx     Fixed buffer overflow when config request is cancelled.
02/23/12   cnx     Use hdrscp_get_config_subtype to get correct HDR revision.
01/17/12   cnx     Fixed buffer overflow when using protocol name enum. 
12/05/11   cnx     Support hdrcp_config_info.conf.
11/08/11   cnx     Support Optimized Handoff phase 3.
11/04/11   cnx     Propose all IRAT subtypes in the session negotiation.
10/03/11   cnx     Override local copy after query Data eHRPD capability value. 
09/29/11   cnx     Check irat subtypes before store the personality; 
                   Fix subtype error in AT-INIT phase.
09/24/11   cnx     Fixed bugs for Optimized Handoff.
09/01/11   cnx     Refeaturized 'query data's eHRPD capability'.
08/26/11   cnx     Renamed ehrpd_apns to data_ehrpd_capability.
08/09/11   cnx     Fixed compiler error.
08/01/11   cnx     Merged Optimized Handoff changes.
07/28/11   cnx     Disabled Enh_AMAC and Enh_CMAC in RevA for KDDI.
07/18/11   cnx     Merged from 11.01.00.
06/03/11   cnx     Supported eHRPD network setting and eHRPD APN.
04/27/11   sju     Added API to provide current HDR revision.
04/27/11   cnx     Changed the return value of hdrscp_get_current_subtype to 
                   0xFFFF when requested protocol is invalid.
12/15/10   pba     Added ehrpd over Rev B support.
10/18/10   pba     Used default subtype when SCMDB has already been reset.
10/15/10   pxu     Replaced JCDMA #ifdef macros with dynamic function checks.
10/07/10   cnx     Fixed a buffer overflow issue when processing the DeletePersonality msg.
07/12/10   pba     Disable eHRPD when not supported by target.
06/21/10   cnx     Fixed compiler warnings.
06/04/10   wsh     Restructured hdrscp data type.
04/09/10   pba     Do not enable eHRPD when RevB with MMPA is chosen 
                   (and no eHRPD)
03/22/10   pba     Added support for eHRPD session negotiation based on 
                   confirming availability of USIM App
03/20/09   wsh     Enabled BCMCS negotiation
02/22/10   pba     Reset the connection retry counter once connection is opened
                   succesfully.
01/15/09   wsh     KA message not send after KA timer expires
12/02/09   pba     IMSI is valid when atleast MCC or MNC or MSIN is non zero.
11/02/09   wsh     Fixed LINT warnings
10/30/09   pba     Added ehrpd support.
10/28/09   wsh     Fixed compiler warning for ARM compiler
10/27/09   pba     Klockwork fixes.
09/18/09   wsh     CMI: support changes from modem DATA
07/27/09   pba     Fixed condition check that validates whether AT's session 
                   is configured correctly.
06/22/09   pba     Process SCP AUR message only in SCPS_OPEN or PS_GAUP state.
04/01/09   grl     Added temporary JCDMA featurization.
03/16/09   etv     Added hdrscp_at_is_dorb_capable.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/27/09   wsh     Added run-time check for KDDI feature flags
02/18/09   wsh     Updated file to include private header hdrmc_v.h
01/30/09   etv     Merged the following changes to main branch
                   Added a new NV item to select BCMCS feature supported
01/23/09   wsh     Fixed crash in logging
12/23/08   pba     Support for Quick Idle Protocol
12/17/08   pba     Updated F3 messages.
12/10/08   smd     Replaced assert.h with amssassert.h
12/04/08   pba     Lint and Compiler warning fixes
11/21/08   pba     Reset SCP ConfigRequest info after ConnectionDeny.
10/20/08   pba     Fixed crash due to wrong personality index use in Rev0 mode.
09/05/08   wsh     Removed dynamic checking if session is tied to PPP
09/08/08   etv     Process HDRPHY_CONFIG_COMPLETE indication.
08/07/08   wsh     Fixed OTA msg parsed w/ wrong subtype during negotiation
08/06/08   pba     Look at main personality protocol subtype when inuse 
                   personality protocol subtype is hardlink.
05/20/08   pba     Set inconfig instance of AMP to inuse values at the start of 
                   session negotiation.
05/09/08   pba     Fixed an issue where SCP was stuck in AT_INIT phase after 
                   connection setup failure due to acquisition loss.
04/09/08   sju     Added hdrscp_get_logging_subtype()
04/02/08   wsh     DCM changes
02/05/08   pba     Removed FEATURE_HDR_CUSTOM_CONFIG instead use only 
                   FEATURE_HDR_REVB_CUSTOM_CONFIG.
01/28/08   pba     Fixed featurization.
11/20/07   wsh     KDDI passport mode phase 1
11/07/07   pba     Allow MC-TAP or MMPA when RMAC4 is negotiated and allow MC-TAP
                   or MMPA when RUP1 is negotiated.
10/17/07   pba     Updates based on code review comments.
10/17/07   pba     Send session close if RMAC4, RUP1 and MMPA are not 
                   negotiated together.
10/15/07   pba     Allow hardlink for RUP.
09/18/07   pba     Propose Enhanced and MC TAP in ATSupportedAppSubtypes only 
                   if Rev A and Rev B protocols are enabled respectively.
09/11/07   wsh     Removed lint high warnings
09/04/07   wsh     AT sends multiple UATI requests when openning a session
08/30/07   wsh     HDRTRACE_HDRSCP_PREVIOUS_SESSION_NEEDED is added back in
08/15/07   etv     Added support for AT-initiated PHY config.
08/22/07   pba     Propose MC test app in ATSupportedApplicationSubtypes 
                   attribute.
08/21/07   wsh     Added dynamic checking if HDR session is tied to PPP
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
08/15/07   pba     Close the session if RUP1 is negotiated without RMAC4.
08/07/07   wsh     Fixed error where ConfigLock was sent reliably.
07/30/07   pba     Disable BCMCS for now.
07/30/07   pba     Updated comments.
07/19/07   sju     Added support to check if rup has valid config. 
07/18/07   pba     Propose MMPA in ATSupportedApplicationSubtypes.
07/02/07   cc      Added support for AT-initiated FTC MAC config.
06/14/07   kss     Added support for AT-initiated AC MAC config.
06/12/07   pba     Removed ASSERT in hdrscp_process_ovhd_info_updated().
05/29/07   wsh     Disallow flow00 to be deactivated for MPA
05/22/07   pba     Obsoleted substates in SCP
05/22/07   wsh     Added featurization flags for 6075/6085 target
05/22/07   pba     Close the session if the AT goes back to IDLE in 
                   HDRSCPS_AT_COMPLETE_SUBSTATE.
05/01/07   pba     Updated F3 message.
04/11/07   pba     Fixed compiler warning.
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/07   wsh     Added support for new prior session handling
03/21/07   pba     When SCC with continue field = 0 is received, AT will check
                   if all personalities are negotiated as per allowed 
03/21/07   pba     While checking for whether MPA/EMPA hardlink is allowed, 
                   also look in the current configreq message for RMAC subtype.
03/06/07   wsh     Removal of RVCT compiler warnings
03/07/07   yll     Removed SCP custom config.
02/23/07   wsh     Disallow flow01 to be activated and flow00 to be deactivated
02/21/07   wsh     Fixed lint warnings.
02/12/07   pba     Do not reject non-default security layer protocol if 
                   fallback value is same.
01/12/06   pba     Reject Rev-A default subtype attributes when Phy is not ST2
01/24/07   pba     Updated Restrict Stream Configuration functionality to allow
                   hardlinking of MPA/EMPA.
01/17/07   yll     CSNA changes for HOMER interface.
01/08/07   yll     Added support for CSNA
12/20/06   pba     Accept both ConfigComplete and SoftConfigComplete during
                   prior session restore.
12/19/06   pba     Allow  only one personality at max to have non-default 
                   security layer protocol subtypes.
12/11/06   hal     Featurized the SupportedCDMAChannels attribute for JCDMA targets
11/29/06   wsh     Mobile should reject SCT GAUP if not configured for RevA;
                   Mobile should list EMPA/MPA in ATSupportedAppSubtype 
                   according to NV items.
11/21/06   wsh     Fixed potential problem that if a AN proposed value is 
                   rejected by AT during AN_INIT state, it might be rolled
                   by to the AT_INIT stage value, instead of the fallback.
10/30/06   pba     Propose default and all non-default subtypes for all 
                   protocols whose inconfig subtype is hardlink.
10/10/06   mpa     Added support for MSM6575.
10/10/06   pba     Send session close if none of the pktapp was negotiated
                   for a personality.
10/10/06   pba     Print error and return default subtype if protocol type is
                   greater than HDRHAI_PROTOCOL_TYPE_ENUM_MAX.
10/04/06   pba     Renamed some function names. Need not write to EFS on 
                   session reconfigured, data will be written on graceful
                   powerdown.
10/02/06   pba     Command all protocols to configure in both initial 
                   configuration and ConfigStart case and let individual 
                   protocols to decide if it needs to configure.
10/02/06   pba     Close the session if connection is lost during AT_INIT phase. 
09/22/06   pba     Reverted HDRHAI_ABORT_NORMAL_CLOSE_SESS_CFG.
09/11/06   pba     Ignore ConfigStart when ConfigLock is set.
09/05/06   mpa     Added support for FEATURE_HDR_QSM.
09/05/06   mpa     Propose E-ACMAC and E-CCMAC by default.
08/17/06   pba     While proposing PriorSession, propose PriorSession attribute 
                   and the rest of the SCP's configuration atrributes as two 
                   separate ConfigRequest messages.
06/20/06   pba     Removed redundant AN auth status maintained by SCP.
                   Addedndum fix: Set ConfigLock to unlocked when connection
                   closes.
                   Config Lock is hardlinked attribute of SCP instead of per
                   personality.
06/05/06   pba     Restore prior session only after making sure that current 
                   and previous session HWID are same.
06/05/06   pba     Reorganized processing of Config Complete message routine.
05/25/06   pba     Removed #define FEATURE_HDR_CUSTOM_CONFIG that was temporarily
                   added for testing purposes.
05/24/06   pba     Added hdrscp_config_subtype_is_hardlink() routine.
05/15/06   pba     Addendum fix: Include default subtype also when AT includes 
                   a non-default or hardlink subtype in ConfigRequest message.
05/10/06   yll/ksu Added support for EMPA
05/09/06   etv     Fixed bug (NULL indication data dereference).
05/08/06   mpa     Only propose BCMCS for non JCDMA builds.
04/27/06   pba     Corrected the value of max number of AT proposed non-default
                   protocol subtypes.
04/21/06   hal     Added HDRIND_SCP_NO_PREV_SESSION indication
                   Added SESSION_CHANGED reasons to the HDRIND_SCM_SESSION_CHANGED 
                   indication
                   Clear ConfigResponse and GAUPResponse timers on SessionClose
04/19/06   pba     Do not accept hardlink subtype for protocol's in main 
                   personality.
04/17/06   pba     Added support to accept/propose hardlink for Stream protocol
04/17/06   mpa     Added force rel0 config, featurized custom config.
04/13/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/13/06   dna     Account for "system lost during AT_INIT while changing PCCC"
04/02/06   dna     Fix so Multimode Capability Discovery subtype is negotiated
03/30/06   pba     Do not propose SCP attributes if re-negotiation is initiated
                   by AT to propose PCCC.
03/27/06   yll     When session is successfully opened, notify SD to reset
                   its session negotiation telescopic avoidance timeline.
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
03/22/06   pba     Added support for DeletePersonality and ConfigurationLock.
03/20/06   yll     Changed the HDRSCP_CON_FAIL_RETRY_COUNT from 5 to 4, to
                   account for the 5 tries (i.e., 4 retries).
03/17/06   pba     Switch personality only if 4MSB of current and GAUP'ed SCT
                   value do not match.
03/17/06   pba     While negotiating prior session look at personality count
                   from SCP's prior session configuration to process SCC/CC.
02/26/06   pba     Do not wait for SLPAck to switch personality on GAUP of SCT.
02/13/06   pba     Fixed handling of protocol subtype change in AN_INIT phase.
02/10/06   pba     Ensure session data is written to EFS when commit=0 in SCC.
02/07/06   dna     1) Ensure prior session seed is used for idle slot
                   2) Allow ConfigReq in AT_INIT state if ConfigComp was sent
02/02/06   pba     Propose ATSupportedAppSubtypes only during main personality
                   negotiation.
                   Ignore SoftConfigComplete message if personality_index_store
                   is set to a value greater than max allowed personalities.
                   Do not accept hardlink subtype for STREAM protocol.
                   In Rev0 mode accept only PersonalityCount value equal 1.
02/01/06   pba     Fixes for HardLink subtype negotiation.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
01/17/06   hal     Fixed packing macro to pack correctly when multiple subtypes
                   are selected in the SCP custom config
01/14/06   dna     Don't use new session parameters until ALMP completely 
                   transitions out of connected state.
                   Allow hardlink subtype for all protocols
12/15/05   etv     Configure RUP only for Phy ST-2.
12/13/05   pba     Added support for multiple personality negotiation
12/12/05   pba     Include PersonalityCount and ATSupportedAppSubtypes only if
                   PhyLayer Subtype 2 is enabled
12/06/05   etv     Added support for Generic MMCD prot's AT-init session neg.
11/29/05   dna     Resurrect NV_HDRSCP_BCMCS_ENABLE_I
11/14/05   dna     Phase 0 support for multiple personalities
11/07/05   hal     Fixed parsing of CReq messages
11/07/05   pba     Modified SCMDB calls to use updated API
11/04/05   etv     Instruct SCM to update defaults for protocol whose subtype
                   negotiated is different from current
10/26/05   hal     Completed support for AT proposed attribs, currently disabled
                   until base station support is ready
10/26/05   dna     Integration updates to GAUP support for RMAC1 rate params
10/22/05   dna     Allow RMAC1 to be negotiated even if physical subtype is 0
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/19/05   mpa     Added hdrscp_is_configuring().
09/12/05   hal     Added support for AT-init RUP Config
                   Added support for AN-init GAUP of SessionConfigToken
                   Added configurable attributes SessionConfigToken, 
                   SupportGAUPSessionConfigToken, PersonalityCount and
                   SupportConfigLock
                   Added AT-init attribs ATSupportedAppSubtypes and
                   PersonalityCount
08/22/05   pba     Added SCM support.
08/09/05   kss     Fixed typo in hdrscp_attrib_was_sent().
07/14/05   dna     Featurize call to hdrmrlpc_config_is_needed under QOS
07/07/05   dna     Added support for Enhanced Idle State protocol
07/07/05   mpa     Added support to propose all non-default subtypes during 
                   AT initiated config and an NV item to selectively enable/
                   disable support for non-default protocol subtypes.
06/07/05   mpa     Temporarily featurized support for RMAC sub 1 under
                   FEATURE_HDR_QOS
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup (More)
03/09/05   pba     Lint cleanup
03/07/05   pba     Changed hdrpacmc_configure() to hdrpacc_configure()
02/04/05   mpa     Reject negotiation of BCMCS subtype 1 if disabled in NV.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
11/04/04   mpa     Added support for RMAC subtype 1
10/29/04   mpa     Removed starting with closed session if bcmcs is supported
10/19/04   mpa     Moved hdrscp_session_is_open() check to hdrsmp.
09/09/04   mpa     Added support for packet app AT initiated negotiation
07/02/04   mpa     Added support for new broadcast protocol.
03/08/04   mpa     Always open a new session on powerup if the sn ppp was up 
                   at the time of powerdown
10/03/03   mpa     Accept default physical layer protocol explicitly
09/16/03   mpa     Converted F3 messages to use new MSG2.0 SSID
09/16/03   mpa     Don't print SCP_SESSION_OPEN event for RLP.
06/26/03   mpa     Give new HDRHAI_CLOSE_CONN_OPEN_DENY reason for SCP failure
                   if one of the connection failures in the last round of setups
                   was caused by a ConnDeny from the AN. The resulting session
                   close will be counted towards avoiding the system.
06/11/03   mpa     Set restore prior session flag to FALSE by default
03/12/03   mpa     Updated all scp fail reasons
01/28/03   mpa     Don't restore prior session when deactivated internally
09/12/02   mpa     hdrscp_failed_data should be allocated out of the heap
08/23/02   dna     Only allow prior session to be restored if it was active.
08/23/02   dna     When the PriorSession info is negotiated, still use the
                   SessionConfigurationToken of latest ConfigComplete message.
06/26/02   mpa     Don't write session status to NV twice in
                   process_con_connection_closed.
06/06/02   dna     Restore ability to restore previous session.
05/13/02   nmw     Added IDLE.Connection Failed ind support.
                   Added FEATURE_HDR_SESSION_QUICK_FAILURE
                   Fixed indication usage
                   Limited configurtaion retry attempts before failing session
03/20/02   vas     Commented out msgs in hdrscp_an_is_authenticated as this
                   function is obselete and needs to be removed.
03/05/02   dna     Store configured app. subtypes in NV!
02/05/02   vas     Store the AN authentication status using it's own separate
                   NV item instead of piggy backing on physical layer subtype.
                   This includes functionality to disable authentication
01/17/02   dna     Remove hdrscp_init()
01/15/02   dna     Fixed case where SLP callback for config req. comes after
                   ConfigResponse.  We were setting the timer incorrectly.
12/03/01   vas     Added command for setting the AN authentication status and
                   writing it to NV.
12/01/01   vas     Added new functions & support for maintaining HDR AN
                   authenentication status.
11/30/01   om      Added security layer support
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
09/01/01   dna     Make config request/response handling identical to LUPC,
                   STREAM, SMP and eventually all others.
                   Modified handling of "waiting_for_ack" flags in tx_outcome
                   callbacks for c_req and c_comp msgs based on code review
08/02/01   nmn     Updated config req msg processing
07/16/01   dna     Removed LUP as a AT initiated config candidate
                   Changed hdrslp_abort_msg() to hdrhmp_abort_msg()
07/10/01   nmn     fixed config outcome_cb for FAILURE
06/06/01   dna     new param for hmp_send_msg()
05/08/01   nmn     beautification
05/02/01   nmn     modified NV interface for subtypes and session status
05/01/01   dna     NV_HDRSCP_SUBTYPE_I renamed to NV_HDRSCP_PROTOCOL_SUBTYPE_I
                   Hack to stub security
03/30/01   nmn     added LUP config support
03/21/01   dna     Removed an event message
01/02/01   nmn     Initial Revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "stdarg.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "nv.h"
#include "ran.h"
#include "memory.h"
#include "sys.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdralmp.h"
#include "hdrutil.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrhai.h"
#include "hdrmci.h"
#include "hdrpacc.h"
#include "hdramp.h"
#include "hdrscp.h"
#include "hdrscpi.h"
#include "hdrscps.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrind.h"
#include "hdrsec.h"
#include "hdrauth.h"
#include "hdrtrace.h"
#include "qw.h"
#include "hdridle.h"
#include "hdrlup.h"
#include "hdrstream.h"
#include "hdrsmp.h"
#include "hdrcon.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif /* FEATURE_HDRMC_DEBUG */
#include "hdrpac_api.h"

#ifdef FEATURE_HDR_QOS 
#include "hdrmrlpc.h"
#endif /* FEATURE_HDR_QOS */
#include "hdridlec.h"
#include "hdrscmdb.h"
#include "hdrscmscp.h"

/* Included for AT-init config */
#include "hdrscmgmmcdp.h"
#include "hdrrmac.h"
#include "hdrrupc.h"  
#include "hdramac.h"

#include "hdralmps.h"
#include "hdrcsna.h"
#include "hdrfmac.h"
#include "hdrphy.h"

#include "sd_v.h"

#ifdef FEATURE_EHRPD
#include "hdrpac_api.h"
#include "fs_public.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"
#include "hdrhitmsg.h"
#endif /* FEATURE_FACTORY_TESTMODE */
#include "hdrrups.h"

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

//#define FEATURE_HDR_PHY3_NEGOTIATION

/* The following are used in conjunction with the "neg_flags" variable to
   determine whether any protocols are in the process of negotiation.  They
   must be included in the code as part of including the AT-initiated
   negotiation for the protocol. */
#define HDRSCP_IDLE_NEG           0x00000001U
#define HDRSCP_STREAM_NEG         0x00000002U
#define HDRSCP_PAC_NEG            0x00000004U
#define HDRSCP_RUP_NEG            0x00000008U
#define HDRSCP_GMMCDP_NEG         0x00000010U
#define HDRSCP_RMAC_NEG           0x00000020U
#ifdef FEATURE_HDR_CSNA
#define HDRSCP_CSNA_NEG           0x00000040U
#else
#define HDRSCP_CSNA_NEG           0x00000000U
#endif
#define HDRSCP_AMAC_NEG           0x00000080U
#define HDRSCP_FMAC_NEG           0x00000100U

#ifdef FEATURE_HDR_PHY3_NEGOTIATION
#define HDRSCP_PHY_NEG            0x00000200U
#else
#define HDRSCP_PHY_NEG            0x00000000U
#endif /* FEATURE_HDR_PHY3_NEGOTIATION */

#define HDRSCP_ALL_NEG  (HDRSCP_IDLE_NEG |    \
                         HDRSCP_STREAM_NEG |  \
                         HDRSCP_PAC_NEG |     \
                         HDRSCP_RUP_NEG |     \
                         HDRSCP_GMMCDP_NEG |  \
                         HDRSCP_RMAC_NEG |    \
                         HDRSCP_CSNA_NEG |    \
                         HDRSCP_AMAC_NEG |    \
                         HDRSCP_FMAC_NEG |    \
                         HDRSCP_PHY_NEG)

#define HDRSCP_CONFIG_COMPLETE_MSG        0x00 /* ConfigurationComplete     */
#define HDRSCP_CONFIG_START_MSG           0x01 /* ConfigurationStart        */
#define HDRSCP_SOFT_CONFIG_COMPLETE_MSG   0x02 /* SoftConfigurationComplete */
#define HDRSCP_LOCK_CONFIG_MSG            0x03 /* LockConfiguration         */
#define HDRSCP_LOCK_CONFIG_ACK_MSG        0x04 /* LockConfigurationAck      */
#define HDRSCP_UNLOCK_CONFIG_MSG          0x05 /* UnlockConfiguration       */
#define HDRSCP_UNLOCK_CONFIG_ACK_MSG      0x06 /* UnlockConfigurationAck    */
#define HDRSCP_DELETE_PERSONALITY_MSG     0x07 /* DeletePersonality         */
#define HDRSCP_DELETE_PERSONALITY_ACK_MSG 0x08 /* DeletePersonalityAck      */

#define HDRSCP_CONFIG_REQ_MSG             0x50 /* ConfigurationRequest      */
#define HDRSCP_CONFIG_RSP_MSG             0x51 /* ConfigurationResponse     */
#define HDRSCP_ATTRIB_UPDATE_REQ_MSG      0x52 /* AttributeUpdateRequest    */
#define HDRSCP_ATTRIB_UPDATE_ACCEPT_MSG   0x53 /* AttributeUpdateAccept     */
#define HDRSCP_ATTRIB_UPDATE_REJECT_MSG   0x54 /* AttributeUpdateReject     */

#define HDRSCP_MSG_PRIORITY    40     /* Priority for all messages */

#define HDRSCP_CON_FAIL_RETRY_COUNT 4
  /* 4 connection retries, i.e., 5 total tries including the first */
  /* connection try, before declaring connection failure */

/* The following are bit masks that are used in conjunction with flags
   that keep track whether AT has received SLP and L3 ACK for individual
   SCP configuration requests that were sent */
#define HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB 0x01
#define HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB 0x02

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
             AT Initiated Configuration Attribute Definitions
-------------------------------------------------------------------------*/
#define HDRSCP_PERSONALITY_COUNT_ATTRIB_ID 0x0110
#define HDRSCP_PERSONALITY_COUNT_ATTRIB_DEFAULT 0x0001
#define HDRSCP_PERSONALITY_COUNT_ATTRIB_MIN_VAL 0x0001
#define HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL 0x0004

/* The max. number of ATSupportedAppSubtype possible */

#ifdef FEATURE_HDR_CSNA
  #define HDRSCP_NUM_APP_SUBTYPES 11
#else
  #define HDRSCP_NUM_APP_SUBTYPES 10
#endif /*FEATURE_HDR_CSNA*/

/* The length in bytes of the ATSuppAppSubtypes attrib EXCLUDING the
   AppSubtype field (which is variable depending on AT model) is: 
   Attrib ID:         16 bits
   Value ID:           8 bits
   NumAppSubtypes (6): 8 bits
   Total:             4 bytes = 32 bits / 8
*/
#define HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_BASE_LEN 4

#define HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_ID 0x1001
#define HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_VALUE_ID 0

/* Size in bits of the NumAppSubtypes field */
#define HDRSCP_NUM_APP_SUBTYPES_SIZE 8

/* The size in bits of each supported app subtype */
#define HDRSCP_AT_SUPP_APP_SUBTYPE_SIZE 16
/*-------------------------------------------------------------------------
          End AT Initiated Configuration Attribute Definitions
-------------------------------------------------------------------------*/

/* Number of octets in Nonce parameter given for prior session validation */
#define HDRSCP_NONCE_LEN 4

/* Max overall length of SecurityPacket field of PriorSession Attribute */
#define HDRSCP_SECURITY_PKT_MAX_LEN  ( HDRSEC_CRYPTOSYNC_SHORT_LEN + \
                                       HDRAUTH_ACPAC_LEN + \
                                       HDRSCP_NONCE_LEN )

#define HDRSCP_BITS_PER_BYTE 8
#define HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID 0x1000 /* attribute ID of the prior session */

#define HDRSCP_MSG_ID_SIZE              8
#define HDRSCP_TRANSACTION_ID_SIZE      8
#define HDRSCP_ATTRIB_LEN_SIZE          8
#define HDRSCP_ATTRIB_ID_SIZE           16
#define HDRSCP_VALUE_ID_SIZE            8
#define HDRSCP_SIMPLE_ATTRIB_VAL_SIZE   16

/* For a complex attribute, ATTRIB_VAL_SIZE is the size of the
 * attribute value structure, including the ValueID
 */

#define HDRSCP_PRIOR_SESSION_RESTORE_VAL_SIZE 152


/* The attribute has Length, attribute ID and one value */
#define HDRSCP_PRIOR_SESSION_ATTRIB_LEN \
        ( HDRSCP_ATTRIB_LEN_SIZE + \
          HDRSCP_ATTRIB_ID_SIZE + \
          HDRSCP_PRIOR_SESSION_RESTORE_VAL_SIZE )

#define HDRSCP_CREQ_PRIOR_SESSION_ATTRIB_LEN_BYTES \
        ((HDRSCP_ATTRIB_ID_SIZE + HDRSCP_PRIOR_SESSION_RESTORE_VAL_SIZE) \
         / HDRSCP_BITS_PER_BYTE)

/* The smallest attribute is a length with an AttributeID */
#define HDRSCP_MIN_ATTRIB_LEN \
        ( HDRSCP_ATTRIB_LEN_SIZE + \
          HDRSCP_ATTRIB_ID_SIZE )

/*-----------------------------------------------------------------------------
                      Simple attribute definitions 
 ----------------------------------------------------------------------------*/
#define HDRSCP_SESS_CFG_TOKEN_ATTRIB_ID                0x0100
#define HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_ID      0x0101
#define HDRSCP_SUPP_CFG_LOCK_ATTRIB_ID                 0x0102
#define HDRSCP_PERSONALITY_CNT_ATTRIB_ID               0x0110

#define HDRSCP_SESS_CFG_TOKEN_ATTRIB_DEFAULT           0x0000
#define HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_DEFAULT 0x0000
#define HDRSCP_SUPP_CFG_LOCK_ATTRIB_DEFAULT            0x0000
#define HDRSCP_PERSONALITY_CNT_ATTRIB_DEFAULT          0x0001

#define HDRSCP_SIMPLE_ATTRIB_LEN \
        ( HDRSCP_ATTRIB_LEN_SIZE + \
          HDRSCP_ATTRIB_ID_SIZE + \
          HDRSCP_SIMPLE_ATTRIB_VAL_SIZE )
        
/* The field in the config response message is length in bytes,
 * excluding the length of length field.
 */
#define HDRSCP_SIMPLE_ATTRIB_LEN_BYTES \
        ( (HDRSCP_ATTRIB_ID_SIZE + \
          HDRSCP_SIMPLE_ATTRIB_VAL_SIZE) / \
          HDRSCP_BITS_PER_BYTE )
          
/* Field length of personality index */          
#define HDRSCP_PERSONALITY_INDEX_SIZE 4

typedef enum
{
  HDRSCP_CONFIG_REQ,  /* Standard ConfigReq message */
  HDRSCP_GAUP_REQ     /* AttributeUpdateRequest msg */

} hdrscp_creq_msg_enum_type;

typedef enum
{
  HDRSCP_CR_AN_INITIATED,           /* begin config, AN initiated */
  HDRSCP_CR_AT_INITIATED,           /* begin config, AT initiated */
  HDRSCP_CR_SUBSEQUENT_PERSONALITY, /* config another personality */
  HDRSCP_CR_RETRY,                  /* continue after fade, etc. */
  HDRSCP_CR_RETRY_BACKOFF           /* continue after backoff time */

} hdrscpi_begin_config_enum_type;

/* This struct is used to hold protocol subtypes received in a
   config request or config response */
typedef struct
{
  boolean is_accepted;
  uint16 value;
} hdrscp_protocol_subtype_attrib_type;

hdrscp_type hdrscp;

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/**********************
** All Pack routines take a pointer to a dsm_item and
** a variable of the type belonging to the appropriate message
**********************/

/*---------------------
* Packing Configuration Complete Message
---------------------*/
#define HDRSCP_PACK_CC_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSCP_PACK_CC_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))
/*---------------------
* Packing Configuration Request Message
---------------------*/
#define HDRSCP_PACK_CON_REQ_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSCP_PACK_CON_REQ_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))

#define HDRSCP_PACK_CON_REQ_LEN( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + (voffset)), 8))

#define HDRSCP_PACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset)), 16))

/* For packing protocol subtype attribute values */
#define HDRSCP_PACK_CON_REQ_ATTRIBUTE_VALUE( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + (voffset)), 16))

/* For packing previous session attribute */
#define HDRSCP_PACK_CON_REQ_VALUE_ID( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + (voffset) + 0), 8))
#define HDRSCP_PACK_CON_REQ_RESTORE( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + (voffset) + 8), 1))
#define HDRSCP_PACK_CON_REQ_RESERVED( msg_ptr, aoffset, voffset)\
          (HDRBIT_PACK8((msg_ptr), 0x00, ((aoffset) + (voffset) + 9), 7))
#define HDRSCP_PACK_CON_REQ_UATI( msg_ptr, val, aoffset, voffset)\
          (hdraddr_pack_addr( (msg_ptr), (val), (uint16) ((aoffset) + (voffset) + 16)))
#define HDRSCP_PACK_CON_REQ_SEC_LEN( msg_ptr, val, aoffset, voffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + (voffset) + 144), 8))
#define HDRSCP_PACK_CON_REQ_SEC_OCTET( msg_ptr, val, aoffset, voffset, num) \
          (HDRBIT_PACK8((msg_ptr), val, ((aoffset) + (voffset) + 152 + (8 * num)), 8))

/*---------------------
* Packing Configuration Response Message
---------------------*/
#define HDRSCP_PACK_CON_RSP_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSCP_PACK_CON_RSP_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))
#define HDRSCP_PACK_CON_RSP_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), ((aoffset) + 0), 8))
#define HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + 8), 16))

/* For packing protocol subtype attribute values */
#define HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE( msg_ptr, val, aoffset) \
          (HDRBIT_PACK16((msg_ptr), (val), ((aoffset) + 24), 16))

/*---------------------
* Packing LockConfigurationAck/UnLockConfigurationAck Message
---------------------*/
#define HDRSCP_PACK_LOCK_CFG_ACK_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSCP_PACK_LOCK_CFG_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))
#define HDRSCP_PACK_UNLOCK_CFG_ACK_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))

/**********************
** Unpack routines.  All take a pointer to a DSM item and a pointer to
** a variable of the appropriate type
**********************/
#define HDRSCP_UNPACK_MSG_ID( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), 0, 8))
#define HDRSCP_UNPACK_TRANS_ID( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), 8, 8))
/*---------------------
* Unpacking Configuration Complete Message
---------------------*/
#define HDRSCP_UNPACK_CC_SES_TOKEN( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), 16, 16))

/*---------------------
* Unpacking SoftConfigurationComplete Message
---------------------*/
#define HDRSCP_UNPACK_SCC_PERSONALITY_INDEX_STORE( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), 16, 4))
#define HDRSCP_UNPACK_SCC_CONTINUE( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), 20, 1))
#define HDRSCP_UNPACK_SCC_COMMIT( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), 21, 1))
#define HDRSCP_UNPACK_SCC_SES_TOKEN( msg_ptr, val_ptr) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), 22, 16))

/*---------------------
* Unpacking Configuration Request Message
* Note: This request can only be received with the Protocol Subtype attribute.
---------------------*/
#define HDRSCP_UNPACK_CON_REQ_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))
#define HDRSCP_UNPACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + 8), 16))

/* For unpacking protocol subtype values */
#define HDRSCP_UNPACK_CON_REQ_ATTRIBUTE_VALUE( msg_ptr, val_ptr, aoffset, voffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + (voffset)), 16))

/*---------------------
* Unpacking Configuration Response Message
---------------------*/
#define HDRSCP_UNPACK_CON_RSP_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 0), 8))
#define HDRSCP_UNPACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + 8), 16))

/* For unpacking protocol subtype attribute values */
#define HDRSCP_UNPACK_CON_RSP_ATTRIBUTE_VALUE( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), ((aoffset) + 24), 16))

/* For unpacking previous session attribute value IDs */
#define HDRSCP_UNPACK_CON_RSP_VALUE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), ((aoffset) + 24), 8))

/*---------------------
* Unpacking DeletePersonality Message
---------------------*/
#define HDRSCP_UNPACK_DELETE_PERSONALITY_COUNT( msg_ptr, val) \
          (HDRBIT_UNPACK8((msg_ptr), (val), 16, 4))
#define HDRSCP_UNPACK_DELETE_PERSONALITY_INDEX( msg_ptr, val, offset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), (20 + (offset)), 4))

/*---------------------
* Packing DeletePersonalityAck Message
---------------------*/
#define HDRSCP_PACK_DELETE_PERSONALITY_ACK_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRSCP_PACK_DELETE_PERSONALITY_ACK_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))

/*--------------------------------------------------------------------------
                              SCP TIMERS

        AN Initiated State Timer and Configuration Response Timer
--------------------------------------------------------------------------*/

rex_timer_type hdrscp_an_init_state_timer;
rex_timer_type hdrscp_config_timer;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

LOCAL void hdrscp_config_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
);

LOCAL void hdrscp_complete_tx_outcome_cb
(
  void * data_ptr,
  hdrerrno_enum_type err_val
);

LOCAL uint32 hdrscp_map_pid_to_neg_flag
(
  hdrhai_protocol_name_enum_type prot_name
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_RESET_CON_FAIL_RETRY_COUNT

DESCRIPTION
  This helper function resets the Connection Failure Retry Counter

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  hdrscp.con_fail_retry_count value is changed to 0.

===========================================================================*/
void hdrscp_reset_con_fail_retry_count (void)
{
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "SCP:Con Fail Retry Counter reset");
  hdrscp.config_conn_setup.con_fail_retry_count = 0;
  hdrscp.config_conn_setup.rxed_con_deny = FALSE;

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Reset Connection Failure Retry Counter means that we either exit 
   * AT INIT state or Connection is opened. We are no longer waiting for 
   * PreRegAllowed == 1 to re-open connection */
  hdrscp.awaiting_prereg_allowed = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH*/

} /* hdrscp_reset_con_fail_retry_count */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_RESET_CONFIG_REQ_INFO

DESCRIPTION
  This helper function resets all appropriate config request info data

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrscp_reset_config_req_info (void)
{
  hdrscp.config_req_info.sent_previous_session_attrib = FALSE;
  hdrscp.config_req_info.sent_personality_count_attrib = FALSE;
  hdrscp.config_req_info.awaiting_slp_ack = 0;
  hdrscp.config_req_info.awaiting_l3_ack = 0;
  hdrscp.config_req_info.num_cfg_req_sent = 0;
  memset( hdrscp.config_req_info.mapping_req_resp, 0, 
          sizeof(hdrscp.config_req_info.mapping_req_resp) );
  hdrscp.config_req_info.prior_session_is_accepted = FALSE;

} /* hdrscp_reset_config_req_info */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_OPEN_TO_AT_INIT_STATE

DESCRIPTION
  This helper function clears the config_requested flag upon transition from
    Open State to AT Init State

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_open_to_at_init_state (void)
{
  hdrscp.config_requested = FALSE;
} /* hdrscp_an_init_to_open_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_AN_INIT_TO_OPEN_STATE

DESCRIPTION
  This helper function resets the previous and negotiated data during a
    transition from AN Init State to Open State

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_an_init_to_open_state (void)
{
  memset(hdrscp.session_params.negotiated.subtype, 0,
         sizeof(hdrscp.session_params.negotiated.subtype));
  hdrscp.session_params.negotiated.config_token = 0;
} /* hdrscp_an_init_to_open_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_ENTER_OPEN_STATE

DESCRIPTION
  This helper function resets the negotiation status, sets the session
    status, and saves the new session status into NV, upon entering
    Open state

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_enter_open_state (void)
{
  /* variable to transport session status info to NV */
  nv_hdrscp_session_status_enum_type nv_session_status = NV_SCP_SESSION_MAX;
  nv_hdrscp_session_status_enum_type current_nv_status = NV_SCP_SESSION_INACTIVE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdrscp.negotiation_state = HDRSCP_NEG_NONE;
  if ((hdrscp.session_status == HDRSCP_SESSION_INACTIVE) ||
      ((hdrscps_get_state() == HDRSCPS_PS_GAUP) && hdrscp.ps_gaup_failed))
  {
    /* when coming from INACTIVE or PS_GAUP state with a GAUP failure, 
       the AT has a default session, will transition to AT_INIT or
       PS_GAUP state shortly after */
    hdrscp.session_status = HDRSCP_SESSION_DEFAULT;
    nv_session_status = NV_SCP_SESSION_DEFAULT;
  }
  else
  {
    /* session negotiated or restored */
    hdrscp.session_status = HDRSCP_SESSION_ACTIVE;
    nv_session_status = NV_SCP_SESSION_ACTIVE;
    hdrscmdb_set_current_session_validity(TRUE);
  }

  if ( ( hdrutil_read_nv_item( NV_HDRSCP_SESSION_STATUS_I, 
                               (nv_item_type*)&current_nv_status ) == NV_DONE_S ) &&
       (current_nv_status == (nv_hdrscp_session_status_enum_type)hdrscp.session_status) )
  {
    /* Do nothing if NV's value is the same as current session status */
    HDR_MSG_PROT( MSG_LEGACY_MED, "session does not change, do not write NV" );
  }
  else
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                    "write NV_HDRSCP_SESSION_STATUS_I, current %d, new value %d",
                    current_nv_status, nv_session_status );

    hdrutil_write_nv_item( NV_HDRSCP_SESSION_STATUS_I,
                           (nv_item_type*)&nv_session_status );
  }

} /* hdrscp_enter_open_state */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_SESSION_CONFIG_TOKEN_IS_CHANGED

DESCRIPTION
  This function finds if the session configuration token changed while AT was
  in connected state

DEPENDENCIES
  None

PARAMETERS
  None 
  
RETURN VALUE
  TRUE if session configuration token changed during connected state, FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_session_config_token_is_changed( void )
{

  return hdrscp.session_cfg_token_changed;

} /* hdrscp_session_config_token_is_changed() */

/*===========================================================================

FUNCTION HDRSCP_GENERATE_PRIOR_SESSION_ATTRIBUTE

DESCRIPTION
  Local helper function to configure the previous session

DEPENDENCIES
  Message ID and Transaction ID have already been packed into send_msg_ptr

PARAMETERS
  send_msg_ptr - pointer to the dsm item where the previous session info
                 is to be packed
  aoffset - offset of this attribute in the dsm item
  send_on_tc   - prior session attribute to be send on traffic channel

RETURN VALUE
  Number of bits in the previous session attribute added to the send_msg_ptr

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdrscp_generate_prior_session_attribute
(
  dsm_item_type * send_msg_ptr,
  uint16 aoffset,
  boolean send_on_tc
)
{
  uint32 security_nonce; /* 32-bit random number for security packet */
  uint8  count; /* counter for FOR loop */
  uint16 voffset; /* value offset */
  uint8  sec_packet_length; /* Length of security packet */
  uint8  sec_packet[HDRSCP_SECURITY_PKT_MAX_LEN]; /* signed security packet */
  hdraddr_type uati; /* our UATI */
  hdrscp_config_params_type previous_config;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Get SCP's previous configuration parameters from SCMDB */
  hdrscmdb_get_scp_prev_session_info(&previous_config);

  /*-- generate security data --*/
  security_nonce = hdrutil_get_random();
  hdrauth_get_previous_session_data( security_nonce,
    &sec_packet_length, sec_packet,
    previous_config.subtype[HDRHAI_SECURITY_PROTOCOL],
    previous_config.subtype[HDRHAI_AUTHENTICATION_PROTOCOL],
    send_on_tc );

  /*-- assign values to Configuration Request fields --*/
  if (hdramp_get_previous_uati( uati ) != E_SUCCESS)
  {
    ERR("Failure to get previous session's UATI",0,0,0);
    return 0;
  }

  /* Store relevant data about this config request for future reference */
  hdrscp.config_req_info.value_id++;
  hdrscp.config_req_info.sent_previous_session_attrib = TRUE;

  voffset = 0;

  /* Pack previous session attribute */
  HDRSCP_PACK_CON_REQ_LEN( send_msg_ptr,
    (uint8) (HDRSCP_CREQ_PRIOR_SESSION_ATTRIB_LEN_BYTES + sec_packet_length),
    aoffset, voffset);
  voffset += HDRSCP_ATTRIB_LEN_SIZE;

  HDRSCP_PACK_CON_REQ_ATTRIBUTE_ID(send_msg_ptr,
    HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID, aoffset, voffset);
  voffset += HDRSCP_ATTRIB_ID_SIZE;

  HDRSCP_PACK_CON_REQ_VALUE_ID(send_msg_ptr, hdrscp.config_req_info.value_id,
     aoffset, voffset);

  /* Restore is TRUE */
  HDRSCP_PACK_CON_REQ_RESTORE(send_msg_ptr, 1, aoffset, voffset);
  HDRSCP_PACK_CON_REQ_RESERVED(send_msg_ptr, aoffset, voffset);
  HDRSCP_PACK_CON_REQ_UATI(send_msg_ptr, uati, aoffset, voffset);
  HDRSCP_PACK_CON_REQ_SEC_LEN(send_msg_ptr, sec_packet_length, aoffset, voffset);

  for (count = 0; count < sec_packet_length; count++)
  {
    HDRSCP_PACK_CON_REQ_SEC_OCTET(send_msg_ptr, sec_packet[count],
      aoffset, voffset, count);
  }

  return (HDRSCP_PRIOR_SESSION_ATTRIB_LEN +
          (sec_packet_length * HDRSCP_BITS_PER_BYTE));

} /* hdrscp_generate_prior_session_attribute */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_MUST_PROPOSE_SUBTYPES

DESCRIPTION
  If any "new" subtypes are supported, then SCP should propose all its
  supported subtypes.  This is a Release A requirement and generally a
  good idea, so that the AN can accept the ones it supports and the AT
  can get a chance to do AT initiated configuration of the given protocol
  subtype.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - at least one subtype needs to be configured
  FALSE - no subtypes need to be configured

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrscp_must_propose_subtypes (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((hdrscp.permitted_subtypes.prot_subtype_mask.enh_amac) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.enh_cmac) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.enh_fmac) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.enh_idle) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.gmmcdp) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.mc_fmac) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rup) ||
      (hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer) ||  
      /* The phone will not propose BCMCS subtype1 in rev0 mode if 
         FEATURE_JCDM?A is defined */
      (!hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA) &&
       hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled)
     )
  {
    return TRUE;
  }
  return FALSE;
} /* hdrscp_must_propose_subtypes() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_ATTRIB_WAS_SENT

DESCRIPTION
  Determine if a given protocol subtype attribute was sent in the most
  recent ConfigurationRequest message.

DEPENDENCIES
  None.

PARAMETERS
  protocol_type - The protocol_type (attribute ID) of interest
  protocol_subtype - The protocol_subtype (attribute value) of interest

RETURN VALUE
  TRUE if the given attribute/value was proposed in the most
  recent ConfigurationRequest message 

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdrscp_attrib_was_sent
(
  uint16 protocol_type,
  uint16 protocol_subtype
)
{
  int j; /* loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If the protocol type is invalid or if we did not include the protocol
     type in config request message then return false. "num_subtypes" will
     be zero if protocol type was not included */
  if ((protocol_type >= HDRHAI_PROTOCOL_TYPE_ENUM_MAX) || 
      (hdrscp.config_req_info.ptype_attrib[protocol_type].num_subtypes == 0))
  {
    return FALSE;
  }

  for (j = 0; j < hdrscp.config_req_info.ptype_attrib[protocol_type].num_subtypes; j++)
  {
    if (hdrscp.config_req_info.ptype_attrib[protocol_type].subtype[j] == protocol_subtype)
    {
      return TRUE;
    }
  } /* loop for protocol's each subtype */

  return FALSE;
} /* hdrscp_attrib_was_sent() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PACK_PROTOCOL_TYPE_ATTRIB

DESCRIPTION
  Helper function to pack the protocol subtypes attributes.

DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  num_subtypes  - Number of subtypes.                  
  subtypes      - list of subtypes to pack for the given protocol type.
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_protocol_type_attrib
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint8  num_subtypes,
  uint16 subtypes[]
)
{
  uint16 voffset = 0; 
    /* Offset within the attribute */
  uint8 index = 0;
    /* Loop index */
  uint16 attrib_len = 0;
    /* Length of the attribute fields in the ConfigRequest message, excluding
      the length of length field itself */
  boolean pack_default_subtype = FALSE;
     /* Flag indicating if default subtype needs to be packed */
  boolean pack_hardlink_subtype = FALSE;
     /* Flag indicating if hardlink subtype needs to be packed */
  uint8 total_num_subtypes = 0;
     /* Total number of subtypes in addition to the subtypes passed in by the calling 
        function, that are packed by this routine */
  hdrscp_subtype_entry_type *attrib_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT (protocol_name < HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX);

  attrib_ptr = &hdrscp.config_req_info.ptype_attrib[protocol_name];

  /*-------------------------------------------------------------------------
  Pack the given protocol type attribute. Packs the length, attribute ID i.e.
  protocol name and attibute values i.e. protocol subtypes.
  -------------------------------------------------------------------------*/

  /* Initialize total_num_subtypes to value passed in */
  total_num_subtypes = num_subtypes;

  /* If the current protocol subtype is non-default and if AT proposes only 
     non-default subtypes AN would not be able to choose the default subtype
     for the protocol hence we include the default subtype also when current
     is non-default. Here we look at the config subtype directly instead of 
     using one of the routines hdrscp_get_current_subtype() or 
     hdrscp_get_config_subtype() because these routines are meant to return
     the actual protocol subtype in the case when the subtype is hardlinked. 
     However here we want to know the actual inconfig subtype. So this way 
     if the inconfig subtype is non-default (incl hardlink) AT will propose
     the default and all non-default subtypes in AT_INIT phase. */
  if ( hdrscp.session_params.negotiated.subtype[protocol_name] !=
         HDRSCP_DEFAULT_PROTOCOL )
  {
    pack_default_subtype = TRUE;
    total_num_subtypes++;
  }

  /* For RTCMAC we propose hardlink for personality negotiation subsequent
     to main personality, so that AN could accept hardlink for RMAC along 
     with hardlink for stream protocol whenever main personality's SN packet
     app is MPA/EMPA */
  if ((protocol_name == HDRHAI_RTC_MAC_PROTOCOL) && 
      (hdrscmdb_personality_is_valid(TRUE, 0)))
  {
    pack_hardlink_subtype = TRUE;
    total_num_subtypes++;
  }

  /* Calculate the attrib_len based on total number of subtypes packed */
  attrib_len = (HDRSCP_ATTRIB_ID_SIZE + (total_num_subtypes *
                     HDRSCP_SIMPLE_ATTRIB_VAL_SIZE)) / HDRSCP_BITS_PER_BYTE;

  /* Pack length field */
  HDRSCP_PACK_CON_REQ_LEN(send_msg_ptr, attrib_len, aoffset, voffset);
  voffset += HDRSCP_ATTRIB_LEN_SIZE;

  /* Pack attribute id - protocol name */
  HDRSCP_PACK_CON_REQ_ATTRIBUTE_ID(send_msg_ptr, protocol_name, aoffset,
                                   voffset);
  voffset += HDRSCP_ATTRIB_ID_SIZE;

  index = 0;

  while (index < num_subtypes)
  {
    /* Remember the protocol subtypes that are sent in configuration request
       message so that we can check AN is accepting only what AT sent */
    attrib_ptr->subtype[index] = subtypes[index];

    /* Pack each attribute value - protocol subtype */
    HDRSCP_PACK_CON_REQ_ATTRIBUTE_VALUE(send_msg_ptr, subtypes[index],
                                        aoffset, voffset);
    voffset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;

    index++;
  }

  if (pack_default_subtype)
  {
    attrib_ptr->subtype[index] = HDRSCP_DEFAULT_PROTOCOL;

    /* Pack default subtype */
    HDRSCP_PACK_CON_REQ_ATTRIBUTE_VALUE(send_msg_ptr, 
                                        HDRSCP_DEFAULT_PROTOCOL,
                                        aoffset, voffset);
    voffset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;

    index++;
  }

  if (pack_hardlink_subtype)
  {
    attrib_ptr->subtype[index] = HDRSCP_HARDLINK;

    /* Pack default subtype */
    HDRSCP_PACK_CON_REQ_ATTRIBUTE_VALUE(send_msg_ptr, 
                                        HDRSCP_HARDLINK,
                                        aoffset, voffset);
    voffset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;

    index++;
  }

  /* Remember the protocol name and number of subtypes */
  attrib_ptr->type = protocol_name;
  attrib_ptr->num_subtypes = index;

  /* Check to ensure subtype array did not buffer overflow */
  ASSERT (index <= HDRSCP_MAX_SUBTYPES);

  return voffset;

} /* hdrscp_pack_protocol_type_attrib() */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRSCP_PACK_PROTOCOL_TYPE_ATTRIB1

DESCRIPTION
  This is a wrapper function to hdrscp_pack_protocol_type_attrib.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  num_subtypes  - Number of subtypes.
  subtype1      - Subtype of the protocol type to pack in CReq array.
  
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_protocol_type_attrib1
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint16 subtype1
)
{
  return  hdrscp_pack_protocol_type_attrib(send_msg_ptr, 
                                           protocol_name, 
                                           aoffset, 
                                           1, 
                                           &subtype1);
} /* hdrscp_pack_protocol_type_attrib1() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PACK_PROTOCOL_TYPE_ATTRIB2

DESCRIPTION
  This is a wrapper function to hdrscp_pack_protocol_type_attrib.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  num_subtypes  - Number of subtypes.
  subtype1      - Subtype of the protocol type to pack in CReq array.
  subtype2      - Subtype of the protocol type to pack in CReq array.
  
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_protocol_type_attrib2
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint16 subtype1,
  uint16 subtype2
)
{
  uint16 subtype[2];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  subtype[0] = subtype1;
  subtype[1] = subtype2;

  return  hdrscp_pack_protocol_type_attrib(send_msg_ptr, 
                                           protocol_name,
                                           aoffset, 
                                           2, 
                                           subtype);
} /* hdrscp_pack_protocol_type_attrib2() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PACK_PROTOCOL_TYPE_ATTRIB3

DESCRIPTION
  This is a wrapper function to hdrscp_pack_protocol_type_attrib.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  num_subtypes  - Number of subtypes.
  subtype1      - Subtype of the protocol type to pack in CReq array.
  subtype2      - Subtype of the protocol type to pack in CReq array.
  subtype3      - Subtype of the protocol type to pack in CReq array.  
  
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_protocol_type_attrib3
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint16 subtype1,
  uint16 subtype2,
  uint16 subtype3
)
{
  uint16 subtype[3];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  subtype[0] = subtype1;
  subtype[1] = subtype2;
  subtype[2] = subtype3;

  return  hdrscp_pack_protocol_type_attrib(send_msg_ptr, 
                                           protocol_name,
                                           aoffset, 
                                           3, 
                                           subtype);
} /* hdrscp_pack_protocol_type_attrib3() */

#ifdef FEATURE_LTE_TO_HDR_OH
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PACK_PROTOCOL_TYPE_ATTRIB4

DESCRIPTION
  This is a wrapper function to hdrscp_pack_protocol_type_attrib.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  num_subtypes  - Number of subtypes.
  subtype1      - Subtype of the protocol type to pack in CReq array.
  subtype2      - Subtype of the protocol type to pack in CReq array.
  subtype3      - Subtype of the protocol type to pack in CReq array.  
  subtype4      - Subtype of the protocol type to pack in CReq array.  
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_protocol_type_attrib4
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint16 subtype1,
  uint16 subtype2,
  uint16 subtype3,
  uint16 subtype4
)
{
  uint16 subtype[4];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  subtype[0] = subtype1;
  subtype[1] = subtype2;
  subtype[2] = subtype3;
  subtype[3] = subtype4;

  return  hdrscp_pack_protocol_type_attrib(send_msg_ptr, 
                                           protocol_name,
                                           aoffset, 
                                           4, 
                                           subtype);
} /* hdrscp_pack_protocol_type_attrib4() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRSCP_PACK_DEFAULT_PROTOCOL_TYPE_ONLY

DESCRIPTION
  This is a wrapper function to hdrscp_pack_protocol_type_attrib.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_default_protocol_type_only
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset
)
{
  return  hdrscp_pack_protocol_type_attrib(send_msg_ptr, 
                                           protocol_name,
                                           aoffset, 
                                           0, 
                                           NULL);
} /* hdrscp_pack_default_protocol_type_only() */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRSCP_PACK_SECURITY_LAYER_PROTOCOL_TYPE

DESCRIPTION
  This functions packs the protocol type for the given security layer protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr  - The DSM item that the ConfigurationRequest message is being 
                  packed into.
  protocol_name - Name of the protocol, whose subtypes is to be packed.
  aoffset       - offset into the dsm item where to add the attribute
                  have been added to this message so far.
  non_default_subtype - Non-default protocol subtype for the given security layer
                        protocol.
  
RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdrscp_pack_security_layer_protocol_type
(
  dsm_item_type *send_msg_ptr,
  hdrhai_protocol_name_enum_type protocol_name,
  uint16 aoffset,
  uint16 non_default_subtype
)
{
  uint16 offset = 0;
    /* Offset where the next attribute packed into the ConfigurationRequest
       message should start. */
  uint16 mp_protocol_subtypes[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];
    /* Protocol subtypes in main personality */
  boolean mp_is_negotiated = FALSE;
    /* Whether main personality is negotiated or not */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the protocol subtypes that were negotiated for the main personality */
  if (hdrscmdb_get_main_personality_ps(mp_protocol_subtypes) == E_SUCCESS)
  {
    mp_is_negotiated = TRUE;
  }

  if (hdrscmdb_nondefault_security_ps_is_allowed(protocol_name))
  {
    /* If non-default security protocol has not been negotiated in any
       other personality we propose it*/
    offset = hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                                               protocol_name, 
                                               aoffset, 
                                               non_default_subtype);
  }
  else if (mp_protocol_subtypes[protocol_name] == non_default_subtype)
  {
     ASSERT (mp_is_negotiated == TRUE);
    /* If main personality's protocol subtype is non-default we explicitly 
       propose hardlink in order to different devices that have limitation
       of only one instance of non-default protocol, from later ones which
       may not */
    offset = hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                                               protocol_name, 
                                               aoffset, 
                                               HDRSCP_HARDLINK);
  }
  else if (hdrscp.session_params.negotiated.subtype[protocol_name] != 
           HDRSCP_DEFAULT_PROTOCOL)
  {
    ASSERT (mp_is_negotiated == TRUE);
    /* If non-default security protocol subtype is already negotiated in 
       some personality other than main personality, the above two checks
       would fail. So exclusively check if the current subtype (fall-back 
       value) is default or not, if not then we have propose the default 
       subtype only. */
    offset = hdrscp_pack_default_protocol_type_only(send_msg_ptr,
                                                    protocol_name, 
                                                    aoffset);
  }

  return offset;
} /* hdrscp_pack_security_layer_protocol_type() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_CONFIG_SUBTYPES

DESCRIPTION
  Local helper function to add the subtype attributes to the 
  ConfigurationRequest message.

DEPENDENCIES
  None.

PARAMETERS
  send_msg_ptr - pointer to the dsm item where the previous session info
                 is to be packed
  aoffset - offset into the DSM item where to add config attributes

RETURN VALUE
  Offset where the next attribute packed into the ConfigurationRequest
  message should start.  In other words, offset of the last bit packed 
  by this routine plus one.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint16 hdrscp_config_subtypes
(
  dsm_item_type * send_msg_ptr,
  uint16 aoffset
)
{
  uint16 initial_aoffset = aoffset;
    /* offset into the DSM item where to add config attributes */
  uint8 protocol_name;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(hdrscp.config_req_info.ptype_attrib, 0, 
         sizeof(hdrscp_subtype_entry_type) * HDRHAI_PROTOCOL_TYPE_ENUM_MAX);

  /* Broadcast Protocol */
  if (hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_BROADCAST_PROTOCOL,
                   aoffset, HDRSCP_GENERIC_BCMCS);
  }
 
#ifdef FEATURE_LTE_TO_HDR_OH
  /* Initialization state protocol */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_INITIALIZATION_STATE_PROTOCOL,
                   aoffset, HDRSCP_IRAT_INIT);
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
     
  /* IDLE state protocol */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.enh_idle)
  { 
    if (hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle)
    {
#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
      {
        aoffset += hdrscp_pack_protocol_type_attrib4(send_msg_ptr, 
                       HDRHAI_IDLE_STATE_PROTOCOL,
                       aoffset, HDRSCP_IRAT_QUICK_IDLE, HDRSCP_IRAT_IDLE, 
                       HDRSCP_QUICK_IDLE, HDRSCP_ENH_IDLE);
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr, 
                     HDRHAI_IDLE_STATE_PROTOCOL,
                     aoffset, HDRSCP_QUICK_IDLE, HDRSCP_ENH_IDLE);
    }
    else 
    {
#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
      {
        aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr, 
                       HDRHAI_IDLE_STATE_PROTOCOL,
                       aoffset, HDRSCP_IRAT_IDLE, HDRSCP_ENH_IDLE);
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                     HDRHAI_IDLE_STATE_PROTOCOL,
                     aoffset, HDRSCP_ENH_IDLE);
    }
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle)
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
    {
      aoffset += hdrscp_pack_protocol_type_attrib3(send_msg_ptr, 
                     HDRHAI_IDLE_STATE_PROTOCOL,
                     aoffset, HDRSCP_IRAT_QUICK_IDLE, HDRSCP_IRAT_IDLE, HDRSCP_QUICK_IDLE);
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_IDLE_STATE_PROTOCOL,
                   aoffset, HDRSCP_QUICK_IDLE);
  }
  else
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
    {
      aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                     HDRHAI_IDLE_STATE_PROTOCOL,
                     aoffset, HDRSCP_IRAT_IDLE);
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

  /* Route Update protocol */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rup)
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
    {
      aoffset += hdrscp_pack_protocol_type_attrib3(send_msg_ptr, 
                   HDRHAI_ROUTE_UPDATE_PROTOCOL,
                   aoffset, HDRSCP_IRAT_MC_RUP, HDRSCP_IRAT_RUP, HDRSCP_MC_RUP);
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                 HDRHAI_ROUTE_UPDATE_PROTOCOL,
                 aoffset, HDRSCP_MC_RUP);
  }
  else
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
    {
      aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                     HDRHAI_ROUTE_UPDATE_PROTOCOL,
                     aoffset, HDRSCP_IRAT_RUP);
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Overhead Messages protocol */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,
                   aoffset, HDRSCP_IRAT_OVHD);
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Physical Layer Subtype 3 and 2 */
  if ((hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer) &&
      (hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer))
  {
    aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr, 
                   HDRHAI_PHYSICAL_LAYER_PROTOCOL, 
                   aoffset, HDRSCP_SUB3_PHYS_LAYER, HDRSCP_SUB2_PHYS_LAYER);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_PHYSICAL_LAYER_PROTOCOL, 
                   aoffset, HDRSCP_SUB3_PHYS_LAYER);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_PHYSICAL_LAYER_PROTOCOL, 
                   aoffset, HDRSCP_SUB2_PHYS_LAYER);
  }

  /* Enhanced ACMAC */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.enh_amac)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_AC_MAC_PROTOCOL,
                   aoffset, HDRSCP_ENH_ACMAC);
  }

  /* Enhanced CCMAC */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.enh_cmac)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_CC_MAC_PROTOCOL,
                   aoffset, HDRSCP_ENH_CCMAC);
  }

 /* MC RMAC, Enhanced RTCMAC, Subtype 1 RMAC */
  if ((hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac) &&
      (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3) &&
      (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1))
  {
    aoffset += hdrscp_pack_protocol_type_attrib3(send_msg_ptr, 
                   HDRHAI_RTC_MAC_PROTOCOL, 
                   aoffset, HDRSCP_MC_RMAC, HDRSCP_SUB3_RMAC, HDRSCP_SUB1_RMAC);
  }
  else if ((hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac) &&
           (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3))
  {
    aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr,
                   HDRHAI_RTC_MAC_PROTOCOL,
                   aoffset, HDRSCP_MC_RMAC, HDRSCP_SUB3_RMAC);
  }
  else if ((hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3) &&
           (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1))
  {
    aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr, 
                   HDRHAI_RTC_MAC_PROTOCOL, 
                   aoffset, HDRSCP_SUB3_RMAC, HDRSCP_SUB1_RMAC);
  }
  else if ((hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac) &&
           (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1))
  {
    aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr,
                   HDRHAI_RTC_MAC_PROTOCOL,
                   aoffset, HDRSCP_MC_RMAC, HDRSCP_SUB1_RMAC);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_RTC_MAC_PROTOCOL,
                   aoffset, HDRSCP_MC_RMAC);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_RTC_MAC_PROTOCOL,
                   aoffset, HDRSCP_SUB3_RMAC);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_RTC_MAC_PROTOCOL,
                   aoffset, HDRSCP_SUB1_RMAC);
  }

  /* MultiCarrier FTCMAC and Enhanced FTCMAC */
  if ((hdrscp.permitted_subtypes.prot_subtype_mask.mc_fmac) &&
      (hdrscp.permitted_subtypes.prot_subtype_mask.enh_fmac))
  {
    aoffset += hdrscp_pack_protocol_type_attrib2(send_msg_ptr, 
                   HDRHAI_FTC_MAC_PROTOCOL, 
                   aoffset, HDRSCP_MC_FMAC, HDRSCP_ENH_FMAC);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.mc_fmac)
  {
      aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_FTC_MAC_PROTOCOL, 
                   aoffset, HDRSCP_MC_FMAC);
  }
  else if (hdrscp.permitted_subtypes.prot_subtype_mask.enh_fmac)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr, 
                   HDRHAI_FTC_MAC_PROTOCOL, 
                   aoffset, HDRSCP_ENH_FMAC);
  }

  /* Generic MultiMode Capability Protocol */
  if (hdrscp.permitted_subtypes.prot_subtype_mask.gmmcdp)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                   aoffset, HDRSCP_GENERIC_MMCDP);
  }
 
#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_SIGNALING_ADAPTATION_PROTOCOL,
                   aoffset, HDRSCP_IRAT_SAP);
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
   
  /* If we are proposing at least one other protocol subtype that is 
   * non-default, we also need to include the security protocol subtypes.
   */
  if (initial_aoffset != aoffset)
  {
    aoffset += hdrscp_pack_security_layer_protocol_type(send_msg_ptr,
                   HDRHAI_KEY_EXCHANGE_PROTOCOL,
                   aoffset, HDRSCP_DH_KEY_EXCHANGE);

    aoffset += hdrscp_pack_security_layer_protocol_type(send_msg_ptr,
                   HDRHAI_AUTHENTICATION_PROTOCOL,
                   aoffset, HDRSCP_SHA1_AUTH);

    aoffset += hdrscp_pack_security_layer_protocol_type(send_msg_ptr, 
                   HDRHAI_SECURITY_PROTOCOL,
                   aoffset, HDRSCP_GENERIC_SECURITY);
  }

  /* Since now AT supports hardlink of all SN packet apps, we propose
     hardlink for stream protocol for all personalities subsequent to 
     main personality. */
  if (hdrscmdb_personality_is_valid(TRUE, 0))
  {
    aoffset += hdrscp_pack_protocol_type_attrib1(send_msg_ptr,
                   HDRHAI_STREAM_PROTOCOL,
                   aoffset, HDRSCP_HARDLINK);
  }

  /* Some protocols do not have non-default subtypes, but they could be
     hardlinked. If it is hardlinked in the current instance then we 
     have to include default subtype for the protocol in ConfigRequest 
     message, so that AN could choose that in the new personality */
  for (protocol_name = 0; protocol_name < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; 
       protocol_name++)
  {
    if ((hdrscp.session_params.negotiated.subtype[protocol_name] == 
         HDRSCP_HARDLINK) &&
        (hdrscp.config_req_info.ptype_attrib[protocol_name].num_subtypes == 0))
    {
      aoffset += hdrscp_pack_default_protocol_type_only(send_msg_ptr,
                     (hdrhai_protocol_name_enum_type) protocol_name, aoffset);
    }
  }

  return (aoffset);
 
} /* hdrscp_config_subtypes */ 

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_COMMAND_PROTOCOLS_TO_CONFIGURE

DESCRIPTION
  Local helper function to command all protocols which are allowed to
  send configuration request messages to send them, if necessary

  Currently, there is a clean, two-step separation between SCP configuring
  subtypes and allowing the protocols to configure for themselves.  If there
  is ever a sequence where the result of a Protocol's configuration may
  result in a different desired subtype for any type, the interface will
  need to be expanded.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  The neg_flags variable will be set to indicate any protocols which have
    accepted the configure command and are sending configuration request
    messages

===========================================================================*/
LOCAL void hdrscp_command_protocols_to_configure 
(
  boolean only_configure_pccc
)
{

  /* set all flags to FALSE, start from scratch calling all configure
     commands */
  hdrscp.neg_flags = 0x00000000;

  if (only_configure_pccc)
  {
    /* If the AT is trying to update the AN's value for the PreferredControl
       ChannelCycle, the only protocol that needs to configure is idle state.
     */
    if (hdridlec_configure_pccc_only(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_IDLE_NEG;
    }
  }
  else
  {
    /* For both cases, initial configuration initiated by AT or ConfigStart
       initiated by AN SCP commands all protocols that have AT configurable 
       attributes  to configure. Each protocol should check if wants to 
       configure based it own requirements. If the protocol returns that it
       needs to configure then SCP will set the corresponding bit in the flag
       for that protocol and would wait for the protocol to indicate that it
       has completed the configuration before sending the ConfigComplete 
       message */
  
    /************************************************************************
     * Not all protocol propose during AT_INIT stage, if it does, SCP will  *
     * call hdrXXX_configure() to command XXX to send ConfigReq. Typically, *
     * every protocol keeps 2 variables, hdrXXXc.negotiated and             *
     * hdrXXXc.current, for in_config and in_use stances. During AT_INIT    *
     * stage, negotiated value is updated to reflect the mutally agreed     *
     * upon value of AN and AT. During AN_INIT stage, however, if this      *
     * attribute is rejected, its value needs to go back to the fallback    *
     * value, which is the current in_use stance value. This is only        *
     * necessary if 1) the protocol propose during AT_INIT, 2) it does not  *
     * use the newer scmdb functions to manage negotiation. If so, the      *
     * rollback need to be added in hdrXXX_progress_config_req_msg()        *
     ************************************************************************/
    if (hdridle_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_IDLE_NEG;
    }

    /* If the stream protocol needs to configure, the AT must wait until
     * the AN responds to the ConfigurationRequest message sent by the 
     * stream protocol before it knows what applications HDRPAC should
     * configure (e.g. default vs. multi-flow packet app).  If the AT 
     * already has an acceptable steram configuration, then HDRPAC can 
     * ask the application(s) to configure now, without waiting.
     */
    if (hdrstream_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "Configure the HDR stream");
      hdrscp.neg_flags |= HDRSCP_STREAM_NEG;
    }
    else 
    {                              
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "HDR stream configured");

      if (hdrpacc_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
      {
        hdrscp.neg_flags |= HDRSCP_PAC_NEG;
      }

      #ifdef FEATURE_HDR_CSNA
      if (hdrcsna_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) )
      {
        hdrscp.neg_flags |= HDRSCP_CSNA_NEG;      
      }
      #endif /* FEATURE_HDR_CSNA */
    }
  
    if ((hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA) || 
        !hdrscp_phone_is_set_to_rev0_mode()) &&
        hdrrup_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
        hdrscp.neg_flags |= HDRSCP_RUP_NEG;
    }

    if (hdrscmgmmcdp_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_GMMCDP_NEG;
    }

    if (hdrrmac_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_RMAC_NEG;
    }

    if (hdramac_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_AMAC_NEG;
    }

    if (hdrfmac_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_FMAC_NEG;
    }

#ifdef FEATURE_HDR_PHY3_NEGOTIATION
    if (hdrphy_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
    {
      hdrscp.neg_flags |= HDRSCP_PHY_NEG;
    }
#endif /* FEATURE_HDR_PHY3_NEGOTIATION */

  }
} /* hdrscp_command_protocols_to_configure */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_WAITING_FOR_PROTOCOLS

DESCRIPTION
  Local helper function to determine whether any protocols are still
    waiting for their configuration response messages.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - at least one protocol is still in the process of configuration
  FALSE - no more protocols are in the process of configuration

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrscp_waiting_for_protocols (void)
{
  return (hdrscp.neg_flags != 0x00000000);
} /* hdrscp_waiting_for_protocols */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_MAX_CON_RETRY

DESCRIPTION
  Local helper function to determine whether we should try to open the connection
  for the purpose of session configuration.  Declare SCP failure if we reach
  a max number of allowed retries.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE  - We have reached the max number of connection setup retries.
          Declare SCP failure.
  FALSE - Keep trying to open a connection.  Max number of retries not reached.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdrscp_max_con_retry ( void )
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscp.config_conn_setup.con_fail_retry_count >= HDRSCP_CON_FAIL_RETRY_COUNT)
  {
    /* we've received at least one connection deny from the network in
       the last round of attempts */
    if (hdrscp.config_conn_setup.rxed_con_deny)
    {
      hdrscp_fail_data.reason = HDRHAI_CLOSE_CONN_OPEN_DENY;
    }
    /* The connection setup always failed for other reasons than receiving a
       connection deny message from the network */
    else
    {
      hdrscp_fail_data.reason = HDRHAI_CLOSE_CONN_OPEN_FAIL;
    }

    hdrscp_reset_con_fail_retry_count();

    /* Stop trying to setup a connection. Declare session config failure. */
    ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
    hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);

    return TRUE;
  }

  /* Haven't reached max allowed connection setup retries.  Keep trying. */
  return FALSE;

} /* hdrscp_max_con_retry() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_SEND_CONFIG_COMPLETE_MSG

DESCRIPTION
  Local helper function to send a configuration complete message

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrscp_send_config_complete_msg (void)
{
  dsm_item_type * send_msg_ptr; /* DSM item pointer to store packed message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hdrscp_max_con_retry() )
  {
    /* We were unable to open a connection after max retries.  *
     * Fail the configuration.                                 */
    return;
  }

  hdrscp.config_conn_setup.con_fail_retry_count++;
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "SCP:CON FAIL RETRY COUNT = %d",
                  hdrscp.config_conn_setup.con_fail_retry_count);

  /* get new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* we wait until we get the ACK for sending the Config Complete
     in case the channel is lost, in which case we will have to resend
     the config complete message. */
  hdrscp.config_complete.awaiting_slp_callback = TRUE;
  hdrscp.config_complete.trans_id++;

  /* pack Configuration Complete Message */
  HDRSCP_PACK_CC_MSG_ID( send_msg_ptr, HDRSCP_CONFIG_COMPLETE_MSG );
  HDRSCP_PACK_CC_TRANS_ID( send_msg_ptr, hdrscp.config_complete.trans_id );

  hdrhmp_send_msg( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, /* protocol */
                   HDRHAI_RTC,
                   TRUE, /* reliable = TRUE, best effort = FALSE */
                   HDRSCP_MSG_PRIORITY, /* priority */
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr, /* pointer to the message to be delivered */
                   hdrscp_complete_tx_outcome_cb, /* function to call for outcome */
                   NULL); /* data to deliver to outcome callback */

} /* hdrscp_send_config_complete_msg */

/*===========================================================================

FUNCTION HDRSCP_PACK_AT_SUPP_APP_SUBTYPES_ATTRIB

DESCRPTION
  This function packs the complex attribute ATSupportedApplicationSubtypes.

DEPENDENCIES
  None.

PARAMETERS
  config_req_msg_ptr - The DSM item containing the ConfigReq message.
  attrib_offset_ptr - A pointer to the offset at which the function should
    begin packing the attribute.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_pack_at_supp_app_subtypes_attrib
( 
  dsm_item_type *config_req_msg_ptr,
  uint16 *attrib_offset_ptr
)
{
  uint16 app_subtype[HDRSCP_NUM_APP_SUBTYPES];
  uint16 attrib_val_offset = *attrib_offset_ptr;

  int num_app_subtypes = 0; /* # of app subtypes supported */
  int i;  /* Loop counter */

#ifdef FEATURE_EHRPD
  boolean is_ehprd_allowed = FALSE;
#endif /* FEATURE_EHRPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  app_subtype[num_app_subtypes++] = HDRHAI_DEF_PACKET_APP_AN;
  app_subtype[num_app_subtypes++] = HDRHAI_DEF_PACKET_APP_SN;
  app_subtype[num_app_subtypes++] = HDRHAI_DEF_PACKET_APP_IS890;

  if (hdrscp.permitted_subtypes.app_subtype_mask.enable_mfpa)
  {
    app_subtype[num_app_subtypes++] = HDRHAI_MULTIFLOW_PACKET_APP_SN;
  }

  if (hdrscp.permitted_subtypes.app_subtype_mask.enable_empa)
  {
    app_subtype[num_app_subtypes++] = HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN;
  }

#ifdef FEATURE_HDR_CSNA
  if (hdrscp.permitted_subtypes.app_subtype_mask.enable_csna)
  {
    app_subtype[num_app_subtypes++] = HDRHAI_3G1X_CSN_APP;
  }
#endif /*FEATURE_HDR_CSNA*/

  if (hdrscp.permitted_subtypes.app_subtype_mask.enable_mmpa)
  {
    app_subtype[num_app_subtypes++] = HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN;
  }

  if (hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer)
  {
  app_subtype[num_app_subtypes++] = HDRHAI_ENH_TEST_APP_IS890A;
  }

  if (hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer)
  {
    app_subtype[num_app_subtypes++] = HDRHAI_MC_TEST_APP_IS890B;
  }

#ifdef FEATURE_EHRPD

  /* HDR does not want to initiate Session Close msg if AT roams to another 
   * network which does not support eHRPD. If DS will not set Data eHPRD
   * capability flag after processing Roaming Indication. HDR has to query
   * DS to get current Data eHRPD capability value.
   */
  hdrscp_set_data_ehrpd_capability_internal(
    hdrpacdsif_get_data_ehrpd_capability() );

  if ( hdrscp_ehrpd_is_allowed() )
  {
    is_ehprd_allowed = TRUE; 
  }

  if ( is_ehprd_allowed )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "eHRPD capability will be advertised" );
    hdrscp.session_params.negotiated.ehrpd_was_advertised = TRUE;
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, "eHRPD capability won't be advertised" );
    hdrscp.session_params.negotiated.ehrpd_was_advertised = FALSE;
  }

  if ( hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa &&
       is_ehprd_allowed )
  {
    app_subtype[num_app_subtypes++] = 
        HDRHAI_ALT_ENHANCED_MULTIFLOW_PACKET_APP_SN;
  }

  if ( hdrscp.permitted_subtypes.app_subtype_mask.enable_ammpa &&
       is_ehprd_allowed )
  {
    app_subtype[num_app_subtypes++] = 
        HDRHAI_ALT_MULTILINK_MULTIFLOW_PACKET_APP_SN;
  }
#endif /* FEATURE_EHRPD */

  ASSERT(num_app_subtypes <= HDRSCP_NUM_APP_SUBTYPES);

  HDRBIT_PACK8( config_req_msg_ptr, (HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_BASE_LEN + 
    HDRSCP_AT_SUPP_APP_SUBTYPE_SIZE / 8 * num_app_subtypes),
    attrib_val_offset, HDRSCP_ATTRIB_LEN_SIZE );
  attrib_val_offset += HDRSCP_ATTRIB_LEN_SIZE;

  HDRBIT_PACK16( config_req_msg_ptr, HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_ID,
    attrib_val_offset, HDRSCP_ATTRIB_ID_SIZE );
  attrib_val_offset += HDRSCP_ATTRIB_ID_SIZE;

  HDRBIT_PACK8( config_req_msg_ptr, HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_VALUE_ID,
    attrib_val_offset, HDRSCP_VALUE_ID_SIZE );
  attrib_val_offset += HDRSCP_VALUE_ID_SIZE;

  HDRBIT_PACK8( config_req_msg_ptr, num_app_subtypes,
    attrib_val_offset, HDRSCP_NUM_APP_SUBTYPES_SIZE );
  attrib_val_offset += HDRSCP_NUM_APP_SUBTYPES_SIZE;

  for( i = 0; i < num_app_subtypes; i++ )
  {
    HDRBIT_PACK16( config_req_msg_ptr, app_subtype[i],
      attrib_val_offset, HDRSCP_AT_SUPP_APP_SUBTYPE_SIZE );
    attrib_val_offset += HDRSCP_AT_SUPP_APP_SUBTYPE_SIZE;
  }

  *attrib_offset_ptr = attrib_val_offset;

} /* hdrscp_pack_at_supp_app_subtypes_attrib() */

/*===========================================================================

FUNCTION HDRSCP_PACK_SIMPLE_ATTRIB

DESCRPTION
  This function packs the simple attribute PersonalityCount.

DEPENDENCIES
  None.

PARAMETERS
  config_req_msg_ptr - The DSM item containing the ConfigReq message.
  attrib_offset_ptr - A pointer to the offset at which the function should
    begin packing the attribute.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_pack_simple_attrib
( 
  dsm_item_type *config_req_msg_ptr,
  uint16 *attrib_offset_ptr,
  uint16 attrib_id,
  uint16 attrib_val
)
{
  uint16 attrib_val_offset = *attrib_offset_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRBIT_PACK8( config_req_msg_ptr, HDRSCP_SIMPLE_ATTRIB_LEN_BYTES, 
    attrib_val_offset, HDRSCP_ATTRIB_LEN_SIZE );
  attrib_val_offset += HDRSCP_ATTRIB_LEN_SIZE;

  HDRBIT_PACK16( config_req_msg_ptr, attrib_id, 
    attrib_val_offset, HDRSCP_ATTRIB_ID_SIZE );
  attrib_val_offset += HDRSCP_ATTRIB_ID_SIZE;

  HDRBIT_PACK16( config_req_msg_ptr, attrib_val,
    attrib_val_offset, HDRSCP_SIMPLE_ATTRIB_VAL_SIZE );
  attrib_val_offset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;

  *attrib_offset_ptr = attrib_val_offset;

} /* hdrscp_pack_simple_attrib() */


/*===========================================================================

FUNCTION HDRSCP_PS_GAUP_IS_ALLOWED

DESCRIPTION
  This function checks if AT is allowed to send prior session GAUP.
  Specifically, it checks if AN has enabled PS_GAUP in SectorParam
  message and if AT is configured to RevA
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  True if AT can use prior GAUP

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean hdrscp_ps_gaup_is_allowed( void )
{
#ifdef FEATURE_HDR_PS_GAUP
  #error code not present
#else
  return FALSE;
#endif /* FEATURE_HDR_PS_GAUP */
} /* hdrscp_ps_gaup_is_allowed() */


/*===========================================================================

FUNCTION HDRSCP_CONFIG_PRIOR_SESSION

DESCRIPTION
  Local helper function to config the previous session

DEPENDENCIES
  Message ID and Transaction ID have already been packed into send_msg_ptr

PARAMETERS
  send_msg_ptr - pointer to the dsm item where the previous session info
                 is to be packed
  aoffset - offset of this attribute in the dsm item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrscp_config_prior_session( void )
{
  dsm_item_type* send_msg_ptr;
  uint16 aoffset;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Store relevant data about this config request for future reference */
  hdrscp.config_req_info.transaction_id++;

  HDRSCP_PACK_CON_REQ_MSG_ID(send_msg_ptr, HDRSCP_CONFIG_REQ_MSG);
  HDRSCP_PACK_CON_REQ_TRANS_ID(send_msg_ptr,
                               hdrscp.config_req_info.transaction_id);

  /* Set aoffset to the offset where the first attribute is to 
     be packed in */
  aoffset = HDRSCP_MSG_ID_SIZE + HDRSCP_TRANSACTION_ID_SIZE;

  aoffset += hdrscp_generate_prior_session_attribute(send_msg_ptr, aoffset, TRUE);
     
  hdrhmp_send_msg(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCP_MSG_PRIORITY,
                  HDRHAI_IN_USE, /* Instance of protocol sending message */
                  send_msg_ptr,
                  hdrscp_config_tx_outcome_cb,
                  (void*)((dword)HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID));

  hdrscp.config_req_info.mapping_req_resp
    [hdrscp.config_req_info.num_cfg_req_sent].transaction_id = 
    hdrscp.config_req_info.transaction_id;
  hdrscp.config_req_info.mapping_req_resp
    [hdrscp.config_req_info.num_cfg_req_sent].attrib_id = 
    HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID;
  hdrscp.config_req_info.num_cfg_req_sent++;
  hdrscp.config_req_info.awaiting_slp_ack |= 
                         HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB;
} /* hdrscp_config_prior_session */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_CONFIG

DESCRIPTION
  Local helper function to manage the configuration process.

  Called with ConfigReason == HDRSCP_CR_AT_INITIATED when:
  * SCP protocol is activated
  * Idle state requests to change PCCC
    a) At time of request, if idle
    b) When ALMP returns to idle, if already configuring at time of request
    c) When AT acquires and gets overhead info

  Called with ConfigReason == HDRSCP_CR_AN_INITIATED when:
  * A ConfigStart message is received from the AN
    
  Called with ConfigReason == HDRSCP_CR_SUBSEQUENT_PERSONALITY when:
  * A SoftConfigComplete is received with continue = 1
    
  Called with ConfigReason == HDRSCP_CR_RETRY when:
  * AT initiated negotiation was interrupted due to poor RF conditions
    a) When ALMP returns to idle
    b) When AT acquires and gets overhead info
  * Connection setup fails during AT initiated state prior to AT sending
    ConfigComplete

DEPENDENCIES
  None

PARAMETERS
  config_reason - indicates whether the config was requested by an entity
                  in the AT, or by a Config Start message of the AN
                  (only examined if the state is OPEN state)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrscp_config
(
  hdrscpi_begin_config_enum_type config_reason
)
{
  dsm_item_type * send_msg_ptr; /* DSM item pointer to store packed message */
  uint16 aoffset; /* Bit offset from start of message of current attrib */
  hdrscps_state_enum_type scp_state = hdrscps_get_state(); /* SCP state */

  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;
  boolean only_configure_pccc = FALSE;
  int num_cfg_req = 0;
  boolean prior_session_valid = FALSE;
  boolean ps_gaup_allowed = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hdrscp_max_con_retry() )
  {
    /* We were unable to open a connection after max retries.  *
     * Fail the configuration.                                 */
    return;
  }

  /* Currently only Idle state can initiate AT-initiated re-negotiation.
   * Everyone else should always be happy with what we agree upon the first 
   * time, or use GAUP to update the configuration.  Even Idle will use GAUP
   * for Release A.
   */
  if (hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
      (config_reason == HDRSCP_CR_AT_INITIATED ||
       ((config_reason == HDRSCP_CR_RETRY || 
         config_reason == HDRSCP_CR_RETRY_BACKOFF) &&
        (hdrscp.negotiation_state == HDRSCP_NEG_AT_MAIN_PERSONALITY ||
         hdrscp.negotiation_state == HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY)
       )
      )
     )
  {
    /* We are re-negotiating a session, and the negotiation is AT initiated 
     * (either just starting or re-trying a negotiation that is AT initiated)
     */
    if (hdridlec_pccc_attrib_config_is_needed())
    {
      only_configure_pccc = TRUE;
    }
    else
    {
      /* Idle wanted to change the PCCC but has now changed its mind. */
      HDR_MSG_PROT (MSG_LEGACY_ERROR, "Idle PCCC re-negotiation canceled");
      if( scp_state == HDRSCPS_AT_INIT )
      {
        /* We fail the negotiation because idle changed its mind about PCCC
         * re-negotiation in the middle of the negotiation.  This would leave
         * the AT stuck in AT initiated state if the negotiation proceeds
         */
        hdrscp_fail_data.reason = HDRHAI_CLOSE_CFG_RSP_EXP;
        hdrscp_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
        hdrscp_fail_data.subtype = HDRSCP_DEFAULT_SCP;
        hdrscp_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;
        hdrscp_fail_data.attribute_id = 0;
        ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
        hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
      }
      return;
    }
  }

  /* Check consistency of scp_state, negotiation_state and config_reason */

  ASSERT ((scp_state == HDRSCPS_AT_INIT) || (scp_state == HDRSCPS_OPEN));

  if (scp_state == HDRSCPS_OPEN)
  {
    ASSERT(( hdrscp.negotiation_state == HDRSCP_NEG_NONE ) &&
           ( config_reason != HDRSCP_CR_SUBSEQUENT_PERSONALITY ));
  }
  else
  {
    ASSERT( hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION ||
            hdrscp.negotiation_state == HDRSCP_NEG_AN_MAIN_PERSONALITY ||
            hdrscp.negotiation_state == HDRSCP_NEG_AT_MAIN_PERSONALITY ||
            hdrscp.negotiation_state == HDRSCP_NEG_AN_SUBSEQUENT_PERSONALITY ||
            hdrscp.negotiation_state == HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY );
  }


  HDRTRACE_HDRSCP_BEGIN_CONFIGURATION();

  if (scp_state == HDRSCPS_OPEN)
  {
    prior_session_valid = hdrscmdb_prior_session_is_valid();
    ps_gaup_allowed = hdrscp_ps_gaup_is_allowed();
    HDRTRACE_HDRSCP_PREVIOUS_SESSION_NEEDED(prior_session_valid);

    /* copy current values into negotiated as "fall-back" values.
     * For subsequent personalities, negotiated is re-set by SCM.
     * only per personality attributes are reset to "fall-back" by
     * SCM */
#ifdef FEATURE_MEMCPY_REMOVAL
    hdrscp.session_params.negotiated = hdrscp.session_params.current;
#else
    memcpy(&hdrscp.session_params.negotiated,
           &hdrscp.session_params.current,
           sizeof(hdrscp_config_params_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    hdrscmdb_set_amp_inconfig_to_inuse();

    /* We would have already returned if IDLE changed its mind about PCCC
       negotiation. So at this point AT could be renegotiating a new sesison
       or restoring a prior session. If we are restoring a prior session, 
       then we check if we are allowed to do PS GAUP, if yes we would 
       switch to PS_GAUP state and GAUP the prior session attribute else 
       would go to AT_INIT and send Configuration request with prior 
       session attribute.
     */

    if (prior_session_valid && ps_gaup_allowed &&
        (!hdrscp.ps_gaup_failed))
    {      
      hdrscps_change_state( HDRSCPS_PS_GAUP );
    }
    else
    {
      hdrscps_change_state( HDRSCPS_AT_INIT );
    }

    if (prior_session_valid)
    {
      hdrscp.negotiation_state = HDRSCP_NEG_PRIOR_SESSION;
    }
    else 
    {
      if (config_reason == HDRSCP_CR_AT_INITIATED)
      {
        hdrscp.negotiation_state = HDRSCP_NEG_AT_MAIN_PERSONALITY;
        if ( !only_configure_pccc )
        {
          hdrind_give_ind( HDRIND_SCP_NO_PREV_SESSION, NULL );
        }
      }
      else
      {
        hdrscp.negotiation_state = HDRSCP_NEG_AN_MAIN_PERSONALITY;
      }
    }
  }
  else if (scp_state == HDRSCPS_AT_INIT) /* called during AT_INIT state */
  {
    if (config_reason == HDRSCP_CR_RETRY)
    {
      hdrscp.config_conn_setup.con_fail_retry_count++;
      HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "SCP:CON FAIL RETRY COUNT = %d",
                      hdrscp.config_conn_setup.con_fail_retry_count);
    }

    /* Update the negotiation state if necessary */
    if (config_reason == HDRSCP_CR_SUBSEQUENT_PERSONALITY)
    {
      if (hdrscp.negotiation_state == HDRSCP_NEG_AT_MAIN_PERSONALITY)
      {
        hdrscp.negotiation_state = HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY;
      }
      else if (hdrscp.negotiation_state == HDRSCP_NEG_AN_MAIN_PERSONALITY)
      {
        hdrscp.negotiation_state = HDRSCP_NEG_AN_SUBSEQUENT_PERSONALITY;
      }
      else
      {
        hdrscp.negotiation_state = HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY;
      }
    }
  }

  /*--------------------------------------------------------------------------- 
             Build ConfigurationRequest message to be sent by SCP
  ---------------------------------------------------------------------------*/

  HDR_MSG_PROT_3 (MSG_LEGACY_MED, 
                  "hdrscp_config() reason:%d scp_state:%d neg_state:%d",
                   config_reason, scp_state, hdrscp.negotiation_state );

  if (only_configure_pccc)
  {
    /* When re-negotiation is initated by Idle state protocol to change PCCC
     * there is no need to propose SCP attributes */
  }
  else
  {
    /* If we are negotiating Prior Sesssion then send an individual 
       Configuration Request message containing the prior session 
       attribute alone. This is done to maintain compatibility with
       all ANs. Some ANs are not be able to handle Prior Session
       attribute and the rest of the SCP attributes included in 
       single ConfigRequest message */
    if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION)
    {
      /* Once AT sends out GAUP or ConReq to restore prior session, it could 
         possible lose network and miss the Accept from AN, in which case
         the AT can no longer silently restore prior session. AT can still
         try to restore prior session thru GAUP/ConfigReq. */
      hdrscmdb_disable_silent_restore();
      if (ps_gaup_allowed && (!hdrscp.ps_gaup_failed))
      {
        /* should never get here if FEATURE_HDR_PS_GAUP is not defined */
#ifdef FEATURE_HDR_PS_GAUP
        #error code not present
#endif
        return;
      }
      else
      {
        hdrscp_config_prior_session();
      }
    }

    num_cfg_req = hdrscp.config_req_info.num_cfg_req_sent;
    /* 7.4.6.1.5.1 "The access terminal shall include in the ConfigurationRequest 
     * messages all supported non-default subtypes of all protocols Types. 
     * If the access terminal supports a value of PersonalityCount greater than 
     * 0x0001, then the access terminal shall send a ConfigurationRequest message
     * containing the ATSupportedApplicationSubtypes attribute."
     */

    /* Check the NV item to see if we are configured to do Release A */
    if (hdrscp_must_propose_subtypes())
    {
      send_msg_ptr = hdrutil_new_msg_buffer();

      /* Store relevant data about this config request for future reference */
      hdrscp.config_req_info.transaction_id++;

      HDRSCP_PACK_CON_REQ_MSG_ID(send_msg_ptr, HDRSCP_CONFIG_REQ_MSG);
      HDRSCP_PACK_CON_REQ_TRANS_ID(send_msg_ptr,
                                   hdrscp.config_req_info.transaction_id);

      /* Set aoffset to the offset where the first attribute is to be packed in */
      aoffset = HDRSCP_MSG_ID_SIZE + HDRSCP_TRANSACTION_ID_SIZE;

      aoffset = hdrscp_config_subtypes(send_msg_ptr, aoffset);

      /* Include PersonalityCount and ATSupportedAppSubtypes only if
         we choose to do Release A or Release B*/
      if ( !hdrscp_phone_is_set_to_rev0_mode() )
      {
        /* As ATSupportedAppSubtypes and PersonalityCount attributes are
           hardlinked, it is enough if we just propose only for the Main
           personality */
        if ((hdrscp.negotiation_state != HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY) &&
            (hdrscp.negotiation_state != HDRSCP_NEG_AN_SUBSEQUENT_PERSONALITY))
        {
          /* Pack the ATSupportedApplicationSubtypes attribute */
          hdrscp_pack_at_supp_app_subtypes_attrib( send_msg_ptr, &aoffset );

          /* Pack the PersonalityCount attribute */
          hdrscp_pack_simple_attrib( send_msg_ptr, &aoffset,
                                     HDRSCP_PERSONALITY_COUNT_ATTRIB_ID, 
                                     HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL );
          hdrscp.config_req_info.sent_personality_count_attrib = TRUE;
        }
      }

      HDRTRACE_HDRSCP_ATTEMPT_CONFIG_SUBTYPE( TRUE );

      hdrhmp_send_msg(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                      HDRHAI_RTC,
                      TRUE,
                      HDRSCP_MSG_PRIORITY,
                      HDRHAI_IN_USE, /* Instance of protocol sending message */
                      send_msg_ptr,
                      hdrscp_config_tx_outcome_cb,
                      NULL);

      hdrscp.config_req_info.mapping_req_resp[num_cfg_req].transaction_id =
                              hdrscp.config_req_info.transaction_id;
      /* It doesn't matter what we fill in here because currently we just want
         to know if the ConfigResponse is for PriorSession or not. But if we
         send more than two SCP ConfigRequests and want differentiate each of 
         the responses then we will need to fill this accordingly. So for now 
         just set it to zero */
      hdrscp.config_req_info.mapping_req_resp[num_cfg_req].attrib_id = 0;
      num_cfg_req++;

      hdrscp.config_req_info.awaiting_slp_ack |= 
                              HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB;
    }
    else
    {
      HDRTRACE_HDRSCP_ATTEMPT_CONFIG_SUBTYPE( FALSE );
    }
  } /* end if-else for AT reconfiguring PCCC */

  /* Remember the number of SCP ConfigRequests that we are sending */
  hdrscp.config_req_info.num_cfg_req_sent = num_cfg_req;

  /* If SCP has nothing to propose (only legacy PCCC re-negotiation case
   * or Rev0 mode), then proceed right away to start other protocols 
   * configuring.
   */
  if (hdrscp.config_req_info.num_cfg_req_sent == 0)
  {
    hdrscp_command_protocols_to_configure( only_configure_pccc );

    if (!hdrscp_waiting_for_protocols())
    {
      /* This happens for AN initiated negotiation, and possibly if the AT 
       * was interrupted during AT initiated state of an AT initiated
       * negotiation, after all protocols had signaled they were done but
       * SCP had not yet entered HDRSCPS_AT_COMPLETE_SUBSTATE.
       */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                    "No AT initiated attributes at this time");
      hdrscp_send_config_complete_msg();
    }
  }

} /* hdrscp_config */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_CURRENT_SUBTYPE

DESCRIPTION
  This accessor function returns the current subtype of the requested protocol

  NOTE:- 
  In SCMDB when a particular personality is to be committed before pushing
  the negotiated data to SCP (and other protocols) SCMDB would loop through 
  hdrscmdb.inuse.scp.subtype and set all the protocols whose subtype is
  hardlink to that of the main personality's protocol subtype.

  So if in the current personality the protocol subtype is configured as 
  hardlink this function would return the subtype of the protocol in main 
  personality.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol, 0xFFFF will be returned if invalid 
  protocol is requested. 

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrscp_get_current_subtype
(
  hdrhai_protocol_name_enum_type protocol
)
{
  uint16 protocol_subtype[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];
    /* Protocol subtype's in main personality */
  hdrerrno_enum_type err_no;
    /* Status of get_main_personality_protocol_subtype call */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
#ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_get_mode() == FTM_MODE)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "FTM mode is enabled, link type %d",
                    hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type );

    if ( hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 0)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
        return HDRSCP_DEFAULT_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_DEFAULT_FMAC;
      }
    }         
    else if ( hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 1)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
         return HDRSCP_SUB2_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_SUB3_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_ENH_FMAC;
      }
    }
    else if (hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 2)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
        return HDRSCP_SUB3_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_MC_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_MC_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_MC_FMAC;
      }
    }
  }
#endif 
  /* The array of protocol subtypes uses hdrhai_protocol_type_enum_type, which 
   * is only 8 bits. As the index of the array, variable 'protocol' has to be 
   * in boundary, which means it has to be less than 
   * HDRHAI_PROTOCOL_TYPE_ENUM_MAX. However, the type of variable 'protocol', is 
   * hdrhai_protocol_name_enum_type, which is 32 bits. To eliminate Klocwork
   * warning, we use HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX, which is the same
   * value of HDRHAI_PROTOCOL_TYPE_ENUM_MAX, to do the sanity check.
   */
  if ( (protocol >= HDRHAI_PHYSICAL_LAYER_PROTOCOL) &&
       (protocol < HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX) )
  {
    if (hdrscp.session_params.current.subtype[protocol] == HDRSCP_HARDLINK)
    {
      err_no = hdrscmdb_get_main_personality_ps(protocol_subtype);

      if (err_no != E_SUCCESS)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        "SCMDB has been reset. So by default the subtype is 0");
        memset(protocol_subtype, 0, 
               sizeof(hdrscp.session_params.current.subtype) );
      }

      return protocol_subtype[protocol];
    }
    else
    {
      return hdrscp.session_params.current.subtype[protocol];
    }
  }
  else
  {
    ERR("Invalid Protocol ID 0x%x",protocol,0,0);
    return 0xFFFF;
  }

} /* hdrscp_get_current_subtype */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_CONFIG_SUBTYPE

DESCRIPTION
  This accessor function returns the configured subtype of the requested
  protocol

  NOTE:- If in the current round of negotiating a personality, the protocol 
  subtype is configured as hardlink this function would return the subtype
  of the protocol in main personality.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned

RETURN VALUE
  Subtype value of the requested protocol

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrscp_get_config_subtype
(
  hdrhai_protocol_name_enum_type protocol
)
{
  hdrscps_state_enum_type cur_state = hdrscps_get_state();
    /* SCP state */
  uint16 protocol_subtype[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];
    /* Protocol subtype's in main personality */
  hdrerrno_enum_type err_no;
    /* Status of get_main_personality_protocol_subtype call */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* The array of protocol subtypes uses hdrhai_protocol_type_enum_type, which 
   * is only 8 bits. As the index of the array, variable 'protocol' has to be 
   * in boundary, which means it has to be less than 
   * HDRHAI_PROTOCOL_TYPE_ENUM_MAX. However, the type of variable 'protocol', is 
   * hdrhai_protocol_name_enum_type, which is 32 bits. To eliminate Klocwork
   * warning, we use HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX, which is the same
   * value of HDRHAI_PROTOCOL_TYPE_ENUM_MAX, to do the sanity check.
   */
  if ( (protocol < HDRHAI_PHYSICAL_LAYER_PROTOCOL) ||
       (protocol >= HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX) )
  {
    ERR("Invalid Protocol ID 0x%x",(uint32)protocol,0,0);
    return 0;
  }

#ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_get_mode() == FTM_MODE)
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "FTM mode is enabled, link type %d",
                    hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type );

    if ( hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 0)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
        return HDRSCP_DEFAULT_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_DEFAULT_FMAC;
      }
    }         
    else if ( hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 1)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
         return HDRSCP_SUB2_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_DEFAULT_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_SUB3_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_ENH_FMAC;
      }
    }
    else if (hdrhitmsg_get_rmac_params_ptr()->modem_fwd_rev_link_type == 2)
    {
      if (protocol == HDRHAI_PHYSICAL_LAYER_PROTOCOL)
      {
        return HDRSCP_SUB3_PHYS_LAYER;
      }
      else if (protocol == HDRHAI_ROUTE_UPDATE_PROTOCOL)
      {
        return HDRSCP_MC_RUP;
      }
      else if (protocol == HDRHAI_RTC_MAC_PROTOCOL)
      {
        return HDRSCP_MC_RMAC;
      }
      else if (protocol == HDRHAI_FTC_MAC_PROTOCOL)
      {
        return HDRSCP_MC_FMAC;
      }
    }
  }
#endif 

  if ((cur_state == HDRSCPS_AT_INIT) ||
      (cur_state == HDRSCPS_AN_INIT))
  {
    /* Since this function is called by individual protocols, that wants to know
     * their configured subtype, returning hardlink subtype would not make sense
     * to the protocol so instead we look at the main personality and return the
     * actual subtype. If the protocol wants to know if subtype is hardlink then
     * it has to call *hdrscmdb_protocol_subtype_is_hardlink*
     */
    if (hdrscp.session_params.negotiated.subtype[protocol] == HDRSCP_HARDLINK)
    {
      err_no = hdrscmdb_get_main_personality_ps(protocol_subtype);

      if (err_no != E_SUCCESS)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        "SCMDB has been reset. So by default the subtype is 0");
        memset(protocol_subtype, 0, 
               sizeof(hdrscp.session_params.current.subtype) );
      }

      return protocol_subtype[protocol];
    }
    else
    {
      return hdrscp.session_params.negotiated.subtype[protocol];
    }
  }
  else
  {
    if (hdrscp.session_params.current.subtype[protocol] == HDRSCP_HARDLINK)
    {
      err_no = hdrscmdb_get_main_personality_ps(protocol_subtype);

      if (err_no != E_SUCCESS)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        "SCMDB has been reset. So by default the subtype is 0");
        memset(protocol_subtype, 0, 
               sizeof(hdrscp.session_params.current.subtype) );
      }

      return protocol_subtype[protocol];
    }
    else
    {
      return hdrscp.session_params.current.subtype[protocol];
    }
  }
} /* hdrscp_get_config_subtype */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSCP_PERSONALITY_SWITCH_IS_PENDING

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
   TRUE - personality switch is pending
   FALSE - personality switch is not pending 

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrscp_personality_switch_is_pending (void)
{
  boolean ps_is_pending = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscp.session_cfg_token_changed && 
      hdrcon_cc_received(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
  {
    ps_is_pending = TRUE;
  }

  return ps_is_pending;

} /* hdrscp_personality_switch_is_pending */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_LOGGING_SUBTYPE

DESCRIPTION
  This accessor function is used by SLP to get the configured subtype of the 
  requested protocol. If active personality switch is in progress, this function
  returns the configured subtype that the AT is going to commit after connection 
  is closed, otherwise, it returns the configured subtype. 

  NOTE:- If in the current round of negotiating a personality, the protocol 
  subtype is configured as hardlink this function would return the subtype
  of the protocol in main personality.

DEPENDENCIES
  None.

PARAMETERS
  protocol - Protocol type for which the subtype is returned
  prot_inst - Instance (InUse/InConfig) of the protocol to log

RETURN VALUE
  Subtype value of the requested protocol

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrscp_get_logging_subtype
(
  hdrhai_protocol_name_enum_type protocol,
  hdrhai_protocol_instance_enum_type prot_inst,
  hdrhai_channel_enum_type channel
)
{
  uint16 protocol_subtype[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];
    /* Protocol subtype's in main personality */
  hdrerrno_enum_type err_no;
    /* Status of get_main_personality_protocol_subtype call */
  hdrscp_per_personality_config_params_type const *scp_c_ptr;
    /* Pointer of SCP configuration* parameters*/
  uint8 pindex;
    /* Personality index which the AT is going to commit*/
  uint16 logging_subtype;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* The array of protocol subtypes uses hdrhai_protocol_type_enum_type, which 
   * is only 8 bits. As the index of the array, variable 'protocol' has to be 
   * in boundary, which means it has to be less than 
   * HDRHAI_PROTOCOL_TYPE_ENUM_MAX. However, the type of variable 'protocol', is 
   * hdrhai_protocol_name_enum_type, which is 32 bits. To eliminate Klocwork
   * warning, we use HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX, which is the same
   * value of HDRHAI_PROTOCOL_TYPE_ENUM_MAX, to do the sanity check.
   */
  if ( (protocol < HDRHAI_PHYSICAL_LAYER_PROTOCOL) ||
       (protocol >= HDRHAI_EXTERNAL_PROTOCOL_NAME_ENUM_MAX) )
  {
    ERR("Invalid Protocol ID 0x%x",(uint32)protocol,0,0);
    return 0;
  }

  /* AN might have swiched personality before AT has.
     Need to parse AN message with the pending personality */
  if ((channel == HDRHAI_FTC) &&
      hdrscp_personality_switch_is_pending())
  {
    /* Based on the new personality, find the subtype of requested protocol*/
    if (hdrscp.session_params.negotiated.personality_count > 1)
    {
      pindex = (hdrscp.session_params.negotiated.config_token >> 12); 
    }
    else 
    {
      /* In Rev 0 mode there is only one personality which is the 
         main personality */
      pindex = 0; 
    }

    HDR_MSG_PROT_1 (MSG_LEGACY_MED, "DEBUG: new pindex = %d ", pindex);

    scp_c_ptr  = hdrscmdb_get_negotiated_config(HDRHAI_SESSION_CONFIGURATION_PROTOCOL, pindex);

    if (scp_c_ptr == NULL)
    {
      /* current fix takes care of 1) PriorSession/PS and AT sends CC first, 
         2) PS, AN sends CC+TCA. There are other cases where logging may be
         wrong. Since those cases are fairly rare, will re-evaluate those
         cases in the future */
      HDR_MSG_PROT(MSG_LEGACY_FATAL, "hdrscp_get_logging_subtype:Negotiated PS is NULL");
      return 0;
    }

    if (scp_c_ptr->subtype[protocol] == HDRSCP_HARDLINK)
    {
      err_no = hdrscmdb_get_main_personality_ps(protocol_subtype);

      if (err_no != E_SUCCESS)
      {
        HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        "SCMDB has been reset. So by default the subtype is 0");
        memset(protocol_subtype, 0, 
               sizeof(hdrscp.session_params.current.subtype) );
      }

      logging_subtype =  protocol_subtype[protocol];
    }
    else
    {
      logging_subtype =  scp_c_ptr->subtype[protocol];
    }
  }
  else if (prot_inst == HDRHAI_IN_CONFIG)
  {
    logging_subtype = hdrscp_get_config_subtype(protocol);
  }
  else
  {
    logging_subtype = hdrscp_get_current_subtype(protocol);
  }

  HDR_MSG_PROT_3 (MSG_LEGACY_LOW, "logging protocol:instance:subtype = %d:%d:%d",
                  (uint32)protocol, (uint32)prot_inst,
                  (uint32)logging_subtype);

  return logging_subtype;
} /* hdrscp_get_logging_subtype */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRSCP_CONFIG_SUBTYPE_IS_HARDLINK

DESCRIPTION
  This function finds if a given protocol's negotiated subtype is hardlink.
  This function is used when there is a ongoing negotiation i.e. when SCP's
  configuration is not yet committed to the database.
  
DEPENDENCIES
  None.

PARAMETERS
  Protocol - Protocol Type for which subtype need to be checked.

RETURN VALUE
  TRUE if protocol subtype is hardlink, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_config_subtype_is_hardlink
(
  hdrhai_protocol_name_enum_type protocol
)
{
  if (hdrscp.session_params.negotiated.subtype[protocol] == HDRSCP_HARDLINK)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* hdrscp_config_subtype_is_hardlink() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_RESTORE_FROM_EFS

DESCRIPTION
  This boolean accessor function tells the caller whether they should
  restore data from EFS or use default values.  It is intended to be used
  only during initialization.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - caller should restore the previous session's data from EFS
  FALSE - caller should use default values for session-related data

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_restore_from_efs
(
  hdrhai_protocol_name_enum_type sender
)
{
  boolean ret_val; /* return value */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Functionally, hdrscp.session_status is no longer needed to determine if
     session data is valid on EFS, instead, hdrscmdb.session_db_status should be
     used. However, many test scripts sets hdrscp.session_status to INACTIVE
     to force session negotiation, so make a special case and check if it is
     INACTIVE. */
  ret_val = (hdrscp.session_status != HDRSCP_SESSION_INACTIVE);

  return (ret_val);
} /* hdrscp_restore_from_efs */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_SESSION_CONFIG_TOKEN

DESCRIPTION
  This accessor function returns the session configuration token.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  value of the session configuration token (specified in the forward
  Configuration Complete message)

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrscp_get_session_config_token
(
  hdrhai_protocol_name_enum_type sender
)
{
  HDRTRACE_HDRSCP_GET_TOKEN(sender, hdrscps_get_state(),
                             hdrscp.session_params.current.config_token);
  return hdrscp.session_params.current.config_token;
} /* hdrscp_get_session_config_token */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_GET_APP_SUBTYPE_CONFIG

DESCRIPTION
  This accessor function returns the configuration for app subtypes.

DEPENDENCIES
  Must be called after hdrscp powerup initialization.

PARAMETERS
  app_subtype_mask - App subtype config mask

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_get_app_subtype_config 
(
  hdrscp_app_subtype_enable_mask_type *app_subtype_mask
)
{

    *app_subtype_mask = hdrscp.permitted_subtypes.app_subtype_mask;

} /* hdrscp_get_app_subtype_config() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSCP_PROCESS_SESSION_CHANGED

DESCRIPTION
  This function is called when the session is changed from Active to Inactive 
  or vice versa.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscp_process_session_changed
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  /* If the session is being closed, clear any existing 
     ConfigResponse timers */
  if( ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_NORMAL ||
      ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
  {
    (void) rex_clr_timer( &hdrscp_config_timer );
  }

  /* Update the local copy of current configuration from SCMDB */
  hdrscmdb_get_scp_session_info(&hdrscp.session_params.current);

} /* hdrscp_process_session_changed() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSCP_COMMIT_SESSION_TO_DB

DESCRIPTION
  This is a wrapper function that invokes the SCMDB routine to write session
  data to EFS.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
static void hdrscp_commit_session_to_db( void )
{
  hdrscmdb_set_current_session_validity(FALSE);
  hdrscmdb_set_scp_session_info(&hdrscp.session_params.negotiated);
} /* hdrscp_commit_session_to_db() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_CHECK_OK_TO_CONFIG

DESCRIPTION
  This function checks if AT is allowed to start a configuration to change
  the idle page slot. 

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_check_ok_to_config( void )
{
  if (hdrscp.config_requested && 
      hdralmp_system_is_acquired(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) &&
      hdrovhd_info_is_updated(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) &&
      !hdrscp_config_is_locked())
  {
    return TRUE;
  }

  return FALSE;
} /* hdrscp_check_ok_to_config() */


/*===========================================================================

FUNCTION HDRSCP_RESTORE_PRIOR_SESSION

DESCRIPTION
  This function informs AMP and SCMDB to restore prior session

DEPENDENCIES
  None.

PARAMETERS
  reason - Reason we are restoring prior session

RETURN VALUE
  TRUE - if prior session can be restore
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrscp_restore_prior_session
(
  hdrscp_ps_restore_process_enum_type  reason
)
{
  boolean retval = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  HDR_MSG_PROT_1(MSG_LEGACY_MED, "hdrscp_restore_prior_session() reason %d",
      (uint32) reason);

  hdrscmdb_set_current_session_validity(FALSE);
  
  /* AMP checks if HWID's of previous and current session are same,
     otherwise fails the session  */
  if (hdramp_previous_session_opened(reason == HDRSCP_PS_RESTORE_SILENT))
  {
    /* Copy all prior session state information as current session state
       information */ 
    hdrscmdb_process_previous_session_opened();

    /* if config_req was used to restore prior session, the
       session token might have been updated, otherwise,
       the config_token would've been already copied from
       previous session. */
    if (reason == HDRSCP_PS_RESTORE_CONFIGREQ)
    {
      /* Set SessionConfigToken to value that was given in most recent Config
         Complete message {Reference IS-856-2 page 7-48 lines 31-35
         Section 7.4.6.1.6.1} */
      hdrscmdb_set_session_config_token(
          hdrscp.session_params.negotiated.config_token);
    }

    /* When we restore prior session, we always have to tell the protocols to 
       use the new configuration attributes, so we call session config token 
       changed */
    hdrscmdb_session_config_token_changed( HDRHAI_SCHG_OPENED_RESTORE_PRIOR );
    /* Reset the session "session_cfg_token_changed" flag */

    (void) sd_ss_ind_hdr_opr_ses_opened(NULL);
    retval = TRUE;
  }
  hdrscp.session_cfg_token_changed = FALSE;
  return retval;
} /* hdrscp_restore_prior_session */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_ALMP_BACK_TO_IDLE

DESCRIPTION
  This function processes the ALMP.BackToIdle Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_almp_back_to_idle(void)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (hdrscps_get_state())
  {
    case HDRSCPS_AN_INIT:
      (void) rex_clr_timer( &hdrscp_an_init_state_timer );
      (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_AN_INIT_STATE_TIMER_SIG);

      hdrscp_fail_data.reason = HDRHAI_CLOSE_QUICK_FAILURE;
      ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
      hdrind_give_ind(HDRIND_SCP_FAILED, (void *)&ind_data);
      break;

    case HDRSCPS_INACTIVE:
      HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                          HDRIND_ALMP_RETURNING_TO_IDLE, hdrscps_get_state());
    break;

    case HDRSCPS_AT_INIT:
        /* stop the config request timer */
        (void) rex_clr_timer(&hdrscp_config_timer);
        (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);

        hdrscp_fail_data.reason = HDRHAI_CLOSE_QUICK_FAILURE;
        ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
        hdrind_give_ind(HDRIND_SCP_FAILED, (void *)&ind_data);
    break;

    case HDRSCPS_OPEN:
      if (hdrscp_check_ok_to_config())
      {
        hdrscp_config(HDRSCP_CR_AT_INITIATED);
      }
    break;

    default:
      ERR("unknown state in ALMP.BackToIdle processing",0,0,0);
    break;
  }

} /* hdrscp_process_almp_back_to_idle */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_ALMP_CONNECTION_CLOSED

DESCRIPTION
  This function processes the ALMP.Connection_Closed Indication

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_almp_connection_closed(void)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;
  hdrscps_state_enum_type curr_state = hdrscps_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((curr_state == HDRSCPS_OPEN) || (curr_state == HDRSCPS_AN_INIT))
  {
    if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION_COMPLETE)
    {
      if (hdrscp_restore_prior_session(HDRSCP_PS_RESTORE_CONFIGREQ))
      {
#ifdef FEATURE_EHRPD
        if ( hdrscp_check_ok_to_open_state() )
#endif /* FEATURE_EHRPD */
        {
          /* Writing the session status to NV will be done in change_state */
          hdrscps_change_state(HDRSCPS_OPEN);
#ifdef FEATURE_LTE_TO_HDR_OH
          hdrsap_set_prereg_status( HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_OPENED );
#endif /* FEATURE_LTE_TO_HDR_OH */          
        }
      }
    }
    else if (hdrscp.negotiation_state == HDRSCP_NEG_COMPLETE)
    {
      /* if it is not a restore prior session, then session data must've
         changed and needed to be saved during power-down */
      hdrscp_commit_session_to_db();  

      /* For normal case, also notify SD that the session 
       * is successfully opened */
      (void) sd_ss_ind_hdr_opr_ses_opened(NULL);

      /* Check if AT's SessionConfigToken was changed during negotiation */
      if (hdrscp_session_config_token_is_changed())
      {
        /* Update SCMDB's inuse instance and return SCM_SESSION_CHANGED indication
           for protocols to get their current configuration parameters */
        hdrscmdb_session_config_token_changed( HDRHAI_SCHG_OPENED_NORMAL );

        hdrscp.session_cfg_token_changed = FALSE;
      }

#ifdef FEATURE_EHRPD
      if ( hdrscp_check_ok_to_open_state() )
#endif /* FEATURE_EHRPD */
      {
        /* Writing the session status to NV will be done in change_state */
        hdrscps_change_state(HDRSCPS_OPEN);
#ifdef FEATURE_LTE_TO_HDR_OH
        hdrsap_set_prereg_status( HDRHAI_PREREG_STATUS_EVENT_EVDO_SESSION_OPENED );
#endif /* FEATURE_LTE_TO_HDR_OH */		
      }
    }
    /* If connection was opened for reasons other than session negotiation, we
       check here if SessionConfigToken was changed through GAUP */
    else if (hdrscp.negotiation_state == HDRSCP_NEG_NONE)
    {
      if (hdrscp_session_config_token_is_changed())
      {
        /* Update SCMDB's inuse instance and return SCM_SESSION_CHANGED indication
           for protocols to get their current configuration parameters; this
           personality switch is activated during the InTraffic/CONNECTED state */
        hdrscmdb_session_config_token_changed( HDRHAI_SCHG_PERSONALITY_SWITCH );
  
        hdrscp.session_cfg_token_changed = FALSE;

#ifdef FEATURE_EHRPD
        /* Check whether eHRPD availability is still satisfied after 
         * personality switch. */
        (void) hdrscp_check_ok_to_open_state();
#endif /* FEATURE_EHRPD */ 
      }
    }
  } /* end if (state is AN_INIT or OPEN */

  /* Since HDRIND_ALMP_CONNECTION_CLOSED is given after ALMP changes state,
   * checking ALMP's state here gives the state after the connection is closed.
   */
  if (hdralmp_system_is_acquired( HDRHAI_SESSION_CONFIGURATION_PROTOCOL ))
  {
    hdrscp_process_almp_back_to_idle();
  }
  else 
  {
    if (curr_state == HDRSCPS_AT_INIT)
    {
      /* Stop the config request timer */
      (void) rex_clr_timer(&hdrscp_config_timer);
      (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);

      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "System Lost during AT_INIT, abandon session." );

      hdrscp_fail_data.reason = HDRHAI_CLOSE_QUICK_FAILURE;
      ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
      hdrind_give_ind(HDRIND_SCP_FAILED, (void *)&ind_data);
    }
    else if (curr_state == HDRSCPS_AN_INIT)
    {
      /* Likely the AT entered LPM during session negotiation.  If it were to
       * exit LPM again within 30s, the AT would be in idle w/session_status == 1
       * which causes the AT to attempt 5.12s sleep without a PCCC.  With this
       * change the session_status will be zero and will just start a new 
       * negotiation.
       */
      (void) rex_clr_timer( &hdrscp_an_init_state_timer );
      (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_AN_INIT_STATE_TIMER_SIG);
  
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "System Lost during AN_INIT, abandon session." );
  
      hdrscp_fail_data.reason = HDRHAI_CLOSE_QUICK_FAILURE;
      ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
      hdrind_give_ind(HDRIND_SCP_FAILED, (void *)&ind_data);
    }
  }
} /* hdrscp_process_almp_connection_closed */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_OVHD_INFO_UPDATED

DESCRIPTION
  This function processes the OVHD.InfoUpdated Indication

  It is possible that we need to start or continue to configure the session
    but have been unable to proceed because we were not acquired on a
    system.  Logically we would use INIT.NetworkAcquired indication to trigger
    us to continue, but we don't know for sure that we reacquired the same
    subnet until we get the overhead information and allow AMP to check this.
    Since AMP uses the OVHD.InfoUpdated Indication, SCP must wait for it as
    well.

  If the is_first_after_acq associated data is set to TRUE, this is the
    first InfoUpdated after the last network acquisition, so behave as if
    the INIT.NetworkAcquired Indication was received.

DEPENDENCIES
  SCP must process this indication after AMP.

PARAMETERS
  ind_data_ptr - points to indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_ovhd_info_updated
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  boolean ind_was_ignored = FALSE;
    /* Flag indicating if indication was ignored */
  hdrscps_state_enum_type curr_scp_state = hdrscps_get_state();
    /* Current SCP state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EHRPD
  /* Need to renegotiate session if eHRPD availability changes. 
   * When SCP receives overhead info update indication, AMP has made a
   * desicion from the following three cases:
   * 1. does not have session and need to negotiate one,
   * 2. close a current session because of subnet change,
   * 3. keep current session or restore prior session.
   * So SCP can verify the session's eHPRD capability at this point.
   */
  if ( !(hdrscp.ehrpd_no_check) && /* Do not close session if ehrpd no check is set. Hotswap */
         hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
         !hdrscp_ehrpd_status_is_valid() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "eHRPD availability of current session is invalid. Close session" );	

    hdrind_give_ind( HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED, NULL );
  }
  else
#endif /* FEATURE_EHRPD */
  {
    if ( hdrsmp_session_is_open( HDRHAI_SESSION_CONFIGURATION_PROTOCOL ) &&
         hdramp_is_session_close_nai_pending() )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Close session because NAI changes" );
      /* Leave it as HDRIND_AMP_FAILED_NEW_NAI for now */
      hdrind_give_ind(HDRIND_AMP_FAILED_NEW_NAI, NULL);
      hdramp_set_session_close_nai_pending(FALSE);

      /* This indication causes the session to close. Hence, there is no need
       * to process the ovhd_update message, as it is not possible to start 
       * or continue to configure the session. */
      return;
    }
  }

  if ( hdramp_is_session_close_nai_pending() )
  {
    hdramp_set_session_close_nai_pending(FALSE);
  }


#ifdef FEATURE_LTE_TO_HDR_OH
  /* Close session if device does not support IRAT while current personality
   * is IRAT. If current personality is non-IRAT, but IRAT is enabled, keep 
   * the current session. 
   */ 
  if ( hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
       hdrscp_is_personality_irat() && 
       !hdrscp_is_irat_enabled() )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "IRAT not enabled, close session(IRAT personality)" );
    hdrind_give_ind( HDRIND_SCP_IRAT_CAPABILITY_CHANGED, NULL );
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

  if (ind_data_ptr->ovhd_updated_is_first_after_acq)
  {

    switch(curr_scp_state)
    {
      case HDRSCPS_OPEN:
        if (hdrscp_check_ok_to_config())
        {
          (void) rex_clr_timer(&hdrscp_config_timer);
          (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);
          hdrscp_config(HDRSCP_CR_AT_INITIATED);
        }
        break;

      case HDRSCPS_AT_INIT:
        (void) rex_clr_timer(&hdrscp_config_timer);
        (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);
        hdrscp_config(HDRSCP_CR_RETRY);
        break; //lint !e527 return before break

      default:
        ind_was_ignored = TRUE;
        /* ignore in any other state */
        break;
    }
  }
  else
  {
    /* Ignore if OVHD update indication is not first after acq */
    ind_was_ignored = TRUE;
  }

  if (ind_was_ignored)
  {
  HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                                HDRIND_OVHD_INFO_UPDATED, curr_scp_state);
  }
} /* hdrscp_process_ovhd_info_updated */
/* <EJECT> */ 

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRSCP_PROCESS_OVHD_PREREG_INFO_UPDATED

DESCRIPTION
  This function processes the PreReg info updated Indication

  It is possible that we need to start or continue to configure the session
    but have been unable to proceed because we were not acquired on a
    system. We don't know for sure that we reacquired the same
    subnet(ColorCode) until we get the pre-preg information and allow AMP to 
    check this. Since AMP uses the Pre-Reg info Indication, SCP must wait for 
    it as well.

  If the is_first_after_acq associated data is set to TRUE, this is the
    first InfoUpdated after the last network acquisition, so behave as if
    the INIT.NetworkAcquired Indication was received.

DEPENDENCIES
  SCP must process this indication after AMP.

PARAMETERS
  ind_data_ptr - points to indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_ovhd_prereg_info_updated
(
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  boolean ind_was_ignored = FALSE;
    /* Flag indicating if indication was ignored */
  hdrscps_state_enum_type curr_scp_state = hdrscps_get_state();
    /* Current SCP state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrscp.awaiting_prereg_allowed && ind_data_ptr->prereg_is_allowed )
  {
    hdrscp.awaiting_prereg_allowed = FALSE;

    HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                    "Rxed PreRegAllowed 1 scp state: %d", curr_scp_state );

    switch( curr_scp_state )
    {
      case HDRSCPS_OPEN:
        if ( hdrscp_check_ok_to_config() )
        {
          (void) rex_clr_timer( &hdrscp_config_timer );
          (void) rex_clr_sigs( HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG );
          hdrscp_config( HDRSCP_CR_AT_INITIATED );
        }
        break;

      case HDRSCPS_AT_INIT:
        (void) rex_clr_timer( &hdrscp_config_timer );
        (void) rex_clr_sigs( HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG );
        hdrscp_config( HDRSCP_CR_RETRY_BACKOFF );
        break;

      default:
        /* ignore in any other state */
        ind_was_ignored = TRUE;
        break;
    }
  }
  else
  {
    /* Ignore if OVHD update indication is not first after acq */
    ind_was_ignored = TRUE;
  }

  if ( ind_was_ignored )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                                HDRIND_OVHD_PREREG_INFO_UPDATED, 
                                curr_scp_state );
  }

} /* hdrscp_process_ovhd_prereg_info_updated() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRSCP_MAP_PID_TO_NEG_FLAG

DESCRIPTION
  This function maps the Protocol ID to its negotiation flag.

DEPENDENCIES
  None.

PARAMETERS
  prot_name - Protocol ID of the protocol to look up

RETURN VALUE
  neg_flag  - Negotiation Flag for this protocol ID

SIDE EFFECTS
  None.

===========================================================================*/
uint32 hdrscp_map_pid_to_neg_flag
(
  hdrhai_protocol_name_enum_type prot_name
)
{
  uint32 neg_flag;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (prot_name)
  {
    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
      neg_flag = HDRSCP_PAC_NEG;
    break;

    default:
      neg_flag = 0x00000000;
    break;
  }

  return neg_flag;

} /* hdrscp_map_pid_to_neg_flag() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_COMPLETE

DESCRIPTION
  This function processes the Config_Complete Indication

DEPENDENCIES
  None.

PARAMETERS
  ind_name  - name of the config complete indication to process
  prot_name - name of the protocol this config complete ind is from 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_complete
(
  hdrind_ind_name_enum_type ind_name,
  hdrhai_protocol_name_enum_type prot_name
)
{
  if (hdrscps_get_state() != HDRSCPS_AT_INIT)
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                          ind_name, hdrscps_get_state());

    return;
  }

  /* turn negotiation flag off */
  switch (ind_name)
  {
    case HDRIND_IDLE_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_IDLE_NEG;
      break;

    case HDRIND_STREAM_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_STREAM_NEG;

      /* If we have a new stream configuration, check to see if the apps 
         bound to those streams need configuration too */
      if (hdrpacc_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL))
      {
        hdrscp.neg_flags |= HDRSCP_PAC_NEG; 
      }

      #ifdef FEATURE_HDR_CSNA
      if (hdrcsna_configure(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) )
      {
        hdrscp.neg_flags |= HDRSCP_CSNA_NEG;      
      }
      #endif /* FEATURE_HDR_CSNA */
      break;

    case HDRIND_SCM_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~hdrscp_map_pid_to_neg_flag(prot_name);
      break;

    case HDRIND_GMMCDP_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_GMMCDP_NEG; 
      break;

    case HDRIND_RUP_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_RUP_NEG;
      break;

    case HDRIND_RMAC_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_RMAC_NEG;
      break;

    case HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_CSNA_NEG;
      break;
      
    case HDRIND_AMAC_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_AMAC_NEG;
      break;

    case HDRIND_FMAC_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_FMAC_NEG;
      break;

    case HDRIND_PHY_CONFIG_COMPLETE:
      hdrscp.neg_flags &= ~HDRSCP_PHY_NEG;
      break;

    default:
      ERR("unknown config complete - ignoring",0,0,0);
      break;
  }

  /* if we are not waiting for any other protocols */
  if (!hdrscp_waiting_for_protocols())
  {
    hdrscp_send_config_complete_msg ();
  }
} /* hdrscp_process_config_complete */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_REQ

DESCRIPTION
  This function processes the Config_Req Indication

DEPENDENCIES
  None.

PARAMETERS
  ind_name - name of the config req indication to process
  prot_name - Protocol requesting configuration
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_req
(
  hdrind_ind_name_enum_type ind_name,
  hdrhai_protocol_name_enum_type prot_name
)
{

  SYS_ARG_NOT_USED(prot_name); /* remove this line when prot_name is used */

  /* state-dependent processing */
  switch (hdrscps_get_state())
  {
    case HDRSCPS_INACTIVE:
      /* ignore indication in inactive state.  When SCP is activated,
         IDLE will eventually be given the Config Command */
      HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                          ind_name, hdrscps_get_state());
      break;

    case HDRSCPS_OPEN:
      hdrscp.config_requested = TRUE;

      if (hdrscp_check_ok_to_config())
      {
        hdrscp_config(HDRSCP_CR_AT_INITIATED);
      }
      break;

    case HDRSCPS_AT_INIT:
    case HDRSCPS_AN_INIT:
    case HDRSCPS_PS_GAUP:
      /* set the flag to wait for the return to Open state */
      hdrscp.config_requested = TRUE;
      break;

    default:
      ERR("unknown state, ignoring config req indication", 0, 0, 0);
  }
} /* hdrscp_process_config_req */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_UNPACK_CONFIG_COMPLETE_MSG

DESCRIPTION
  This function first checks if the AN is allowed to send SCC/CC message 
  based on the negotiated personality count attribute, if this is correct
  then unpacks the config complete message fields into the structure
  passed in. After unpacking this function also validates the message
  based on transaction identifier.

DEPENDENCIES
  None.

PARAMETERS
  msg_id      - Message ID (Soft Config Complete or Config Complete)
  item_ptr    - pointer to the received message
  cc_msg_ptr  - Pointer to ConfigComplete message structure in which to unpack
                the fields of the message.

RETURN VALUE
  E_SUCCESS if the message unpacking was sucessful. Failure reason otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscp_unpack_config_complete_msg
(  
  uint8 msg_id,
  dsm_item_type *item_ptr,
  hdrscp_config_complete_msg_type *cc_msg_ptr
)
{
  /*lint --e{64} suppress type mismatch for err for entire function */
  /* Status of message unpacking */
  hdrerrno_enum_type err = E_SUCCESS;
  /* Number of personalities negotiated in the current session */
  uint8 personality_count = hdrscp.session_params.negotiated.personality_count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(cc_msg_ptr, 0, sizeof(hdrscp_config_complete_msg_type));

  if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION)
  {
    /* If AT and AN are trying to restore prior session, AT would accept both
       ConfigComplete(CC) or SoftConfigComplete(SCC) from the AN. Ideally 
       during prior session restore if AN decides to restore prior session, 
       AN should just accept the prior session attribute and not change any
       other attribute value. So CC is the message that AT should accept 
       because the inconfig personality count value will be 1. However there
       is no harm in AT accepting SCC, AT would just take the SCT value and
       default the continue field to 0 and commit to 1. If prior session 
       attribute is accepted the AT would restore all values from the prior
       session except the SCT which is taken from the CC or SCC message, if
       there is a change in the 4MSB of SCT, commit procedure would be 
       triggered */
  }
  else
  { 
  if ((msg_id == HDRSCP_SOFT_CONFIG_COMPLETE_MSG) && (personality_count < 2))
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_ERROR,
                    "SoftConfigComplete not allowed for %d personalities", 
                    personality_count);
    err = E_FAILURE;
    return err;
  }
  else if ((msg_id == HDRSCP_CONFIG_COMPLETE_MSG) && (personality_count > 1))
  {
    HDR_MSG_PROT_1 (MSG_LEGACY_ERROR,
                    "ConfigComplete not allowed for %d personalities", 
                    personality_count);
    err = E_FAILURE;
    return err;
  }
  }

  /* Unpack the transaction ID */
  err |= (int) HDRSCP_UNPACK_TRANS_ID(item_ptr, &(cc_msg_ptr->trans_id));

  if ((err != E_SUCCESS) || (cc_msg_ptr->trans_id != 
                             hdrscp.config_complete.trans_id))
  { 
    /* If transaction ID does not match the one sent in AT Init State's
      Configuration Complete Message, ignore the message */
    err = E_FAILURE;   
    return err;
  }

  /* Unpack the rest of the fields */
  if (msg_id == HDRSCP_CONFIG_COMPLETE_MSG)
  {
    err |= (int) HDRSCP_UNPACK_CC_SES_TOKEN(item_ptr, &(cc_msg_ptr->sct));
  }
  else /* HDRSCP_SOFT_CONFIG_COMPLETE_MSG */
  {
    err |= (int) HDRSCP_UNPACK_SCC_PERSONALITY_INDEX_STORE(item_ptr, 
                                    &(cc_msg_ptr->personality_index_store));
    err |= (int) HDRSCP_UNPACK_SCC_CONTINUE(item_ptr, 
                                            &(cc_msg_ptr->continue_config));

    if (cc_msg_ptr->continue_config == 0)
    {
      err |= (int) HDRSCP_UNPACK_SCC_COMMIT(item_ptr, &(cc_msg_ptr->commit));
      err |= (int) HDRSCP_UNPACK_SCC_SES_TOKEN(item_ptr, &(cc_msg_ptr->sct));
    }
  }

  return err;
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PERSONALITY_TO_STORE_IS_VALID

DESCRIPTION
  This fuction checks if the personality to store into a specified personality
  index store is valid.

DEPENDENCIES
  None.

PARAMETERS
  personality_index_store - Index at which the last negotiated personality has
                            to be stored.

RETURN VALUE
  TRUE if the personality is valid. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrscp_personality_to_store_is_valid
( 
  uint8 personality_index_store
)
{
  /* Whether hardlink subtype was negotiated for any of the protocol in the 
     personality to be stored */
  boolean hardlink_st_is_negotiated = FALSE;
  /* Whether personality to store is valid or not */
  boolean personality_is_valid = TRUE;
  /* Loop index */
  int index;
  /* Negotiated SN Packet app in personality to be stored */
  hdrhai_app_subtype_enum_type neg_snpktapp = 
      hdrstream_get_negotiated_snpktapp();
  uint16 rmac_subtype = 
      hdrscp_get_config_subtype(HDRHAI_RTC_MAC_PROTOCOL);
  uint16 rup_subtype = 
      hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);
/*-------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------
    If prior session is being restored then there is no need to check,
    as we are just going to restore a already valid session.
  -----------------------------------------------------------------------*/
   if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION)
   {
     return personality_is_valid;
   }

  /*----------------------------------------------------------------------
   Find if any protocol subtype is negotiated as hardlink in the personality 
   to be stored.
  -----------------------------------------------------------------------*/
  for (index = 0; index < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; index++)
  {
    if (hdrscp.session_params.negotiated.subtype[index] == HDRSCP_HARDLINK)
    {
      hardlink_st_is_negotiated = TRUE;
      break;
    }
  }

  /*----------------------------------------------------------------------
   Make sure no protocol is hardlinked in main personality.
  -----------------------------------------------------------------------*/
  if (((hdrscp.session_params.negotiated.personality_count == 1) || 
       (personality_index_store == 0)) && hardlink_st_is_negotiated)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Hardlink subtype negotiated in main personality");
    personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
   Personality index store is not greater than max supported.
  -----------------------------------------------------------------------*/
  else if (personality_index_store >= HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Personality Index store greater than max supported");
    personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
   AT least one packet application bound to SN stream is negotiated.
  -----------------------------------------------------------------------*/
  else if (!hdrstream_stream_is_negotiated(HDRHAI_APP_PACKET_APP_ANY))
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Stream protocol config is invalid, no pktapp negotiated");
    personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
   MMPA/EMPA/MFPA are negotiated as per configuration that AT supports
  -----------------------------------------------------------------------*/
  else if (!hdrmrlpc_flow_configured_to_desired()) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "MMPA/EMPA/MPA: Flow00/01 not negotiated to supported value");
    personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
   If EMPA/MFPA bound to SN stream is negotiated then RTMAC Subtype 3 is 
   negotiated.
  -----------------------------------------------------------------------*/
  else if (((neg_snpktapp == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
            (neg_snpktapp == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN)) &&
           (rmac_subtype != HDRSCP_SUB3_RMAC))
  {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: RMAC subtype must be 3 when MPA/EMPA negotiated ");
      personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
   MMPA bound to SN stream or MC-TAP, RUP1 and RTMAC4 all go together
  -----------------------------------------------------------------------*/
  else if ((neg_snpktapp == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) &&
           (((rup_subtype != HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
             && (rup_subtype != HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */
            ) || 
           (rmac_subtype != HDRSCP_MC_RMAC))
          )
  {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: MMPA negotiated w/o RMAC4 or RUP1");
      personality_is_valid = FALSE;
  }
  else if ((rmac_subtype == HDRSCP_MC_RMAC) &&
           (((neg_snpktapp != HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) &&
             !hdrstream_stream_is_negotiated(HDRHAI_MC_TEST_APP_IS890B)) ||
            ((rup_subtype != HDRSCP_MC_RUP)
#ifdef FEATURE_LTE_TO_HDR_OH
             && (rup_subtype != HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */            
            )))
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: RMAC4 negotiated w/o MMPA or RUP1");
    personality_is_valid = FALSE;
  }
  else if (((rup_subtype == HDRSCP_MC_RUP) 
#ifdef FEATURE_LTE_TO_HDR_OH
            || (rup_subtype == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */           
           ) &&
           (((neg_snpktapp != HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN) &&
             !hdrstream_stream_is_negotiated(HDRHAI_MC_TEST_APP_IS890B)) ||
            (rmac_subtype != HDRSCP_MC_RMAC)))
  {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: RUP1 negotiated w/o MMPA or RMAC4");
      personality_is_valid = FALSE;
  }
  /*----------------------------------------------------------------------
  Check whether RUP negotiated values are valid.
  -----------------------------------------------------------------------*/
  else if (!hdrrupc_config_is_valid())
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
          "Unsupported Config: Negotiated RUP attributes are not valid ");
    personality_is_valid = FALSE;
  }
#ifdef FEATURE_LTE_TO_HDR_OH
  else if ( ( hdrscp_get_config_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL )
                == HDRSCP_IRAT_SAP ) &&
            ( ( hdrscp_get_config_subtype( HDRHAI_INITIALIZATION_STATE_PROTOCOL )
                  != HDRSCP_IRAT_INIT ) ||
              ( hdrscp_get_config_subtype( HDRHAI_OVERHEAD_MESSAGES_PROTOCOL )
                  != HDRSCP_IRAT_OVHD ) ||
              !( ( hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL )
                     == HDRSCP_IRAT_IDLE ) ||
                 ( hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL )
                     == HDRSCP_IRAT_QUICK_IDLE )
               ) || 
              !( ( ( rup_subtype == HDRSCP_IRAT_RUP ) &&
                   ( rmac_subtype == HDRSCP_SUB3_RMAC ) ) ||
                 ( ( rup_subtype == HDRSCP_IRAT_MC_RUP ) &&
                   ( rmac_subtype == HDRSCP_MC_RMAC ) ) )
            )
          )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR,
                  "Unsupported Config: IRAT subtypes are not valid" );
    personality_is_valid = FALSE;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
   
  /* Note this check needs to be separate or the last one in else-if otherwise
     rest of the validation never gets done */ 

  /*----------------------------------------------------------------------
   If MMPA/EMPA/MFPA bound to SN stream is hardlinked to main personality, 
   then RTCMAC should also be hardlinked and vice versa.
  -----------------------------------------------------------------------*/
  if ((neg_snpktapp == HDRHAI_MULTIFLOW_PACKET_APP_SN) ||
      (neg_snpktapp == HDRHAI_ENHANCED_MULTIFLOW_PACKET_APP_SN) ||
      (neg_snpktapp == HDRHAI_MULTILINK_MULTIFLOW_PACKET_APP_SN))
  {
    if (hdrscp_config_subtype_is_hardlink(HDRHAI_STREAM_PROTOCOL) &&
        !hdrscp_config_subtype_is_hardlink(HDRHAI_RTC_MAC_PROTOCOL))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: MPA/EMPA/MMPA hardlinked without RMAC hardlink");
      personality_is_valid = FALSE;
    }
    else if (hdrscp_config_subtype_is_hardlink(HDRHAI_RTC_MAC_PROTOCOL) &&
             !hdrscp_config_subtype_is_hardlink(HDRHAI_STREAM_PROTOCOL))
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,
        "Unsupported Config: RMAC hardlinked without MPA/EMPA/MMPA hardlink");
      personality_is_valid = FALSE;
    }
  }
     
  return personality_is_valid;
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PERSONALITY_TO_COMMIT_IS_VALID

DESCRIPTION
  This function checks if the personality to commit specified in the SCC or 
  CC message is valid or not.

DEPENDENCIES
  None.

PARAMETERS
  sct - Session configuration Token.
  personality_index - Personality index store.

RETURN VALUE
  TRUE if personality index is valid, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrscp_personality_to_commit_is_valid
(
  uint16 sct, 
  uint8 pindex_store
)
{
  /* Personality to store is valid or not */
  boolean personality_is_valid = FALSE;
/*-------------------------------------------------------------------------*/

  if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION)
  {
    /* If AT is negotiating prior session then look at the prior session 
       personalities to check if it is valid or not. */
    if (hdrscmdb_personality_is_valid(FALSE, (sct>>12)))
    {
      personality_is_valid = TRUE;
    }
  }
  else /* Negotiating a new session */
  {
    /* If AT is negotiating new session then look at the current session
       personalities, in this case it's also possible that the last personality
       to store is the personality to commit, which is not yet stored in the
       database */
    if (hdrscmdb_personality_is_valid(TRUE, (sct>>12)) ||
        ((sct>>12) == pindex_store))
    {
      personality_is_valid = TRUE;
    }
  }

  return personality_is_valid;
}

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_COMPLETE_MSG

DESCRIPTION
  This function processes the Configuration Complete Message

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_config_complete_msg
(
  dsm_item_type *item_ptr,
  uint8          msg_id
)
{
  /* Fields of (Soft) Config Complete message are extracted into this struct */
  hdrscp_config_complete_msg_type cc_msg;
  /* Data for Session failed indication */
  static hdrscp_scp_fail_ind_type session_fail_data;
  hdrind_ind_data_union_type ind_data;
  boolean abort_processing = FALSE;
  hdrscp_config_params_type previous_config;
/*---------------------------------------------------------------------------*/

  /* Get SCP's previous configuration parameters from SCMDB */
  hdrscmdb_get_scp_prev_session_info(&previous_config);

  if ((hdrscps_get_state() == HDRSCPS_AN_INIT) ||
      (hdrscps_get_state() == HDRSCPS_AT_INIT &&
        hdrscp.config_complete.awaiting_slp_callback))
  {
    /* Configuration complete message can only be received in AN Init State
       or in AT init if we already sent out config complete but just haven't
       received the ack */
  }
  else
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      msg_id, hdrscps_get_state()); //lint !e506
    return;
  }

  if (hdrscp_unpack_config_complete_msg(msg_id, item_ptr,&cc_msg) != E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Error while unpacking Config Complete Msg, ignoring");
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
       msg_id, hdrscps_get_state() ); //lint !e506
    return;
  }

  if ((!hdrscp_personality_to_store_is_valid(cc_msg.personality_index_store)) ||
      ((cc_msg.commit == 1) && 
       (!hdrscp_personality_to_commit_is_valid(cc_msg.sct, 
                                               cc_msg.personality_index_store))) ||
  /* If we are trying to restore prior session and have more than one personality 
    in previous session, network shouldn’t send Configuration Complete */
      ((hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION) &&
       (previous_config.personality_count > 1) &&
       (msg_id == HDRSCP_CONFIG_COMPLETE_MSG) &&
       ((cc_msg.sct>>12) >= HDRSCMDB_MAX_PERSONALITIES)))
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                 "Personality index or SCT to store or commit is invalid sct=%x pi=%d",
				  cc_msg.sct, cc_msg.personality_index_store);
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
       msg_id, hdrscps_get_state() ); //lint !e506

    /* Send session close message */
    session_fail_data.reason = HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID;
    ind_data.scp_fail_ptr = (void *)&session_fail_data;

    hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
       
    return;
  }

  hdrscp.session_params.negotiated.config_token = cc_msg.sct;

  if (hdrscps_get_state() == HDRSCPS_AT_INIT &&
      hdrscp.config_complete.awaiting_slp_callback)
  {
    hdrscp.config_complete.awaiting_slp_callback = FALSE;
    (void)hdrhmp_abort_msg(hdrscp_complete_tx_outcome_cb, NULL);

    ASSERT( hdrscp.negotiation_state != HDRSCP_NEG_NONE );
    hdrscps_change_state(HDRSCPS_AN_INIT);
  }

  switch(hdrscp.negotiation_state)
  {
    case HDRSCP_NEG_PRIOR_SESSION:
      hdrscp.negotiation_state = HDRSCP_NEG_PRIOR_SESSION_COMPLETE;
      if ((msg_id == HDRSCP_SOFT_CONFIG_COMPLETE_MSG) &&
          ((cc_msg.continue_config != 0) || cc_msg.commit != 1))
      {
        HDR_MSG_PROT (MSG_LEGACY_ERROR,
         "No more personalities allowed once PriorSession is negotiated");
        cc_msg.continue_config = 0;
        cc_msg.commit = 1;
      }
      break;

    case HDRSCP_NEG_AN_MAIN_PERSONALITY:
    case HDRSCP_NEG_AT_MAIN_PERSONALITY:
    case HDRSCP_NEG_AN_SUBSEQUENT_PERSONALITY:
    case HDRSCP_NEG_AT_SUBSEQUENT_PERSONALITY:
      if ((msg_id == HDRSCP_CONFIG_COMPLETE_MSG) ||
          (cc_msg.continue_config == 0))
      {
        hdrscp.negotiation_state = HDRSCP_NEG_COMPLETE;
      }
      break;

    default:
      ERR("received config complete msg, unexpected negotiation status",0,0,0);
      abort_processing = TRUE;
      break;
  }

  /* if hdrscp.negotiation_state is invalid, abort processing */
  if (abort_processing)
  {
      return;
  }

  if (hdrscp.negotiation_state != HDRSCP_NEG_PRIOR_SESSION_COMPLETE)
  {
    /* Store the negotiated set of config parameters as new personality, if
       ConfigComplete was received this will be stored as main personality 
       (personality_index_store = 0), otherwise if SoftCC it will be stored
       into personality_index_store specified in SoftCC message */
    hdrscmdb_process_config_complete(cc_msg.personality_index_store);
  }
  else
  {
    /* If we are negotiating prior session wait for connection to close to 
       restore prior session information */
  }

  if (msg_id == HDRSCP_CONFIG_COMPLETE_MSG)
  {
    hdrscp.session_cfg_token_changed = TRUE;

    /* We are done configuring, sending ConnectionClose... */
    hdralmp_close_connection( HDRHAI_ABORT_NORMAL_CLOSE,
                              HDRHAI_SESSION_CONFIGURATION_PROTOCOL );
  }
  else /* HDRSCP_SOFT_CONFIG_COMPLETE_MSG */
  {
    if (cc_msg.continue_config == 0)
    {
      /* When Soft Configuration Complete with continue field = 0 is 
         received, AT will check if all personalities are negotiated 
         as per allowed configuration, if not AT will send session close */
      if (hdrscmdb_negotiated_session_is_valid())
      {
        if (cc_msg.commit == 1)
        {
          hdrscp.session_cfg_token_changed = TRUE;
    
          /* We are done configuring, sending ConnectionClose... */
          hdralmp_close_connection( HDRHAI_ABORT_NORMAL_CLOSE,
                                    HDRHAI_SESSION_CONFIGURATION_PROTOCOL );
        }
        else if (cc_msg.commit == 0)
        {
          /* If commit is zero then 4 most significant bits of the SCT field shall
             be set to the index of Personality that is currently in use */
          if ( (hdrscp.session_params.negotiated.config_token >> 12) != 
                     (hdrscp.session_params.current.config_token >> 12) )
          {
            HDR_MSG_PROT_2(MSG_LEGACY_ERROR,
                           "Recvd SCT with invalid 4MSB NegSCT = %d CurrSCT = %d",
                           hdrscp.session_params.negotiated.config_token, 
                           hdrscp.session_params.current.config_token);
    
            HDR_MSG_PROT(MSG_LEGACY_ERROR,"Discarding rcvd SCT and using current");
    
            hdrscp.session_params.negotiated.config_token =
                          hdrscp.session_params.current.config_token;
          }
           
          hdrscp_commit_session_to_db();

#ifdef FEATURE_EHRPD
          if ( hdrscp_check_ok_to_open_state() )
#endif /* FEATURE_EHRPD */
          {
            /* Just go back to open state since we didn't change the in-use config */
            hdrscps_change_state( HDRSCPS_OPEN );
          }
        }
      }
      else
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, 
           "Session validity check failed, closing session");

        /* Send session close message */
        session_fail_data.reason = HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID;
        ind_data.scp_fail_ptr = (void *)&session_fail_data;

        hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
      }
    }
    else if (cc_msg.continue_config == 1)
    {
      /* Start another round of negotiation */
      hdrscps_change_state( HDRSCPS_AT_INIT );
      hdrscp_config( HDRSCP_CR_SUBSEQUENT_PERSONALITY );
    }
  }
} /* hdrscp_process_config_complete_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_START_MSG

DESCRIPTION
  This function processes the Configuration Start Message

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_start_msg
(
  dsm_item_type * item_ptr
)
{
  if ((hdrscps_get_state() != HDRSCPS_OPEN) || hdrscp_config_is_locked())
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      HDRSCP_CONFIG_START_MSG, hdrscps_get_state()); //lint !e506

    return;
  }

  /* AN is forcing configuration */
  hdrscp_config(HDRSCP_CR_AN_INITIATED);
} /* hdrscp_process_config_start_msg */ //lint !e715 item_ptr not used

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRSCP_UPDATE_CONFIG_LOCK

DESCRIPTION
  This function sets the config lock to the value specified.
  
DEPENDENCIES
  None.

PARAMETERS
  config_lock_val - ConfigLock value.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_update_config_lock(boolean config_lock_val)
{
  if (hdrscp.session_params.current.config_is_locked != config_lock_val)
  {
    hdrscp.session_params.current.config_is_locked = config_lock_val;

    /* Update negotiated instance also */
    hdrscp.session_params.negotiated.config_is_locked = config_lock_val;

    hdrscmdb_update_config_lock(config_lock_val);
  }
  else
  {
    /* There is no change in ConfigLock state so just return */
  }

} /* hdrscp_update_config_lock() */

/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_LOCK_MSG

DESCRIPTION
  This function processes the ConfigurationLock and ConfigurationUnlock
  messages.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_lock_msg
(
  dsm_item_type * item_ptr,
  uint8           msg_id
)
{
  uint8 transaction_id; 
    /* Transaction ID of the message */
  dsm_item_type *send_msg_ptr; 
    /* DSM buffer for sent message */
  hdrscps_state_enum_type curr_state = hdrscps_get_state(); 
    /* Current state of SCP protocol */
  boolean config_is_locked = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( !hdrscp.session_params.current.support_config_lock || 
       (hdralmps_get_state() != HDRALMPS_CONNECTED) )
  {
    /* If support_config_lock is FALSE then processing of ConfigLock related
     * messages are not supported so just ignore the message and return */
    if ( msg_id == HDRSCP_LOCK_CONFIG_MSG )
    {
      HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_LOCK_CONFIG_MSG, curr_state); //lint !e506
    }
    else
    {
      HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_UNLOCK_CONFIG_MSG, curr_state); //lint !e506
    }

    return;
  }

  /* Extract the Transaction ID from the message */
  if ( HDRSCP_UNPACK_TRANS_ID(item_ptr, &transaction_id) != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from Lock or unLock Msg", 0, 0, 0 );
    return;
  }
  
  /* AT succesfully got all the fields of the message, now process the
     message and send the response */
  send_msg_ptr = hdrutil_new_msg_buffer();

  if( msg_id == HDRSCP_LOCK_CONFIG_MSG )
  {
    /* Set the Configuration to Locked */
    config_is_locked = TRUE;

    /* Pack the LockConfigurationAck messageID in the response */
    HDRSCP_PACK_LOCK_CFG_ACK_MSG_ID( send_msg_ptr, HDRSCP_LOCK_CONFIG_ACK_MSG);
  }
  else /* HDRSCP_UNLOCK_CONFIG_MSG */
  {
    /* Set the Configuration to UnLocked */
    config_is_locked = FALSE;

    /* Pack the UnLockConfigurationAck messageID */
    HDRSCP_PACK_UNLOCK_CFG_ACK_MSG_ID(send_msg_ptr, 
                                      HDRSCP_UNLOCK_CONFIG_ACK_MSG);
  }
    
  HDRSCP_PACK_LOCK_CFG_TRANS_ID( send_msg_ptr, transaction_id );
   
  hdrscp_update_config_lock( config_is_locked );

  /* Send LockConfigurationAck or UnLockConfigurationAck message */
  hdrhmp_send_msg_on_existing_rtc(
                   HDRHAI_SESSION_CONFIGURATION_PROTOCOL, /* protocol */
                   FALSE, /* reliable = TRUE, best effort = FALSE */
                   HDRSCP_MSG_PRIORITY, /* priority */
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,  /* pointer to the message to be delivered */
                   NULL,  /* function to call for outcome */
                   NULL); /* data to deliver to outcome callback */

  if (msg_id == HDRSCP_UNLOCK_CONFIG_MSG)
  {
    hdrind_give_ind(HDRIND_SCP_CONFIG_UNLOCKED, NULL);

    if (hdrscp_check_ok_to_config()) 
    {
      /* Check if idle page slot change was requested when AT's config was
       * locked. If yes then initiate an configrequest to change the idle
       * page slot */
      hdrscp_config(HDRSCP_CR_AT_INITIATED);
    }
  }

} /* hdrscp_process_config_lock_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_DELETE_PERSONALITY_MSG

DESCRIPTION
  This function processes the Delete Personality message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_delete_personality_msg
(
  dsm_item_type * item_ptr
)
{
  dsm_item_type *send_msg_ptr; 
    /* DSM buffer for sent message */
  uint8 transaction_id; 
    /* Transaction ID of the message */
  uint8 personality_count;
    /* Number of personalities included in DeletePersonality message */
  uint8 personalities_to_delete[HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL];
    /* Indexes of personalities that AN included in DeletePersonality message*/
  uint8 cnt;
    /* Loop counter */
  int err = E_SUCCESS;
    /* Error value */
  uint8 offset = 0;
  static hdrscp_scp_fail_ind_type session_fail_data;
    /* Data for Session failed indication */
  hdrind_ind_data_union_type ind_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscps_get_state() != HDRSCPS_OPEN)
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      HDRSCP_DELETE_PERSONALITY_MSG, hdrscps_get_state()); //lint !e506

    return;
  }

  /* Extract the Transaction ID from the message */
  if ( HDRSCP_UNPACK_TRANS_ID(item_ptr, &transaction_id) != E_SUCCESS )
  {
    ERR("Error extracting TransactionID from DeletePersonality Msg", 0, 0, 0 );
    return;
  }

  err |= (int) HDRSCP_UNPACK_DELETE_PERSONALITY_COUNT(item_ptr, 
                                                      &personality_count);

  if ( personality_count >= HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
                    "PersonalityCount: %d, valid range is less than %d.", 
                    personality_count, HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL );
    return;
  }

  for( cnt=0; ((cnt < personality_count) && (err == E_SUCCESS)) ; cnt++ )
  {
    err |= (int) HDRSCP_UNPACK_DELETE_PERSONALITY_INDEX(item_ptr,
                   &personalities_to_delete[cnt], offset);

    if ( (personalities_to_delete[cnt] == 0) || 
         (personalities_to_delete[cnt] > 
            HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL) ||
         (personalities_to_delete[cnt] == 
            (hdrscp.session_params.current.config_token >> 12)) )
    {
      /* AN should not delete the current or main personality, if AN includes
       * these personality indexes in DeletePersonality message, ignore the
       * message and send session close */
      err = E_DATA_INVALID;
      ERR("Invalid pindex in DeletePersonalityMsg %d", 
           personalities_to_delete[cnt], 0, 0);
      break;
    }

    offset += HDRSCP_PERSONALITY_INDEX_SIZE;
  }

  if (!err)
  {
    /* Set the personalitites as deleted */
    hdrscmdb_delete_personalities( personalities_to_delete, personality_count);

    /* Send DeletePersonalityAck */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* Pack the DeletePersonalityAck messageID in the response */
    HDRSCP_PACK_DELETE_PERSONALITY_ACK_MSG_ID( send_msg_ptr, 
                                           HDRSCP_DELETE_PERSONALITY_ACK_MSG );
    HDRSCP_PACK_DELETE_PERSONALITY_ACK_TRANS_ID(send_msg_ptr, transaction_id );

    /* Send DeletePersonalityAck message */
    hdrhmp_send_msg( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, /* protocol */
                     HDRHAI_AC_OR_RTC,
                     TRUE, /* reliable = TRUE, best effort = FALSE */
                     HDRSCP_MSG_PRIORITY, /* priority */
                     HDRHAI_IN_USE, /* Instance of protocol sending message */
                     send_msg_ptr,  /* pointer to the message to be delivered */
                     NULL,  /* function to call for outcome */
                     NULL); /* data to deliver to outcome callback */
  }
  else if (err == E_DATA_INVALID)
  {
    /* AN included invalid personality index. Send session close */
    session_fail_data.reason = HDRHAI_CLOSE_PERSONALITY_INDEX_INVALID;
    ind_data.scp_fail_ptr = (void *)&session_fail_data;

    hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
  }

} /* hdrscp_process_delete_personality_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_PROTOCOL_SUBTYPE

DESCRIPTION
  This function processes the subtype attributes for Configuration
    Request Messages

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  attrib_id - The particular protocol whose subtype is being negotiated.
  protocol_subtype_ptr - Pointer to buffer to write new negotiated value.

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If TRUE is returned, the acceptable value is returned through
  protocol_subtype_ptr

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean hdrscp_process_protocol_subtype
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint16 attrib_id,
  uint16 * protocol_subtype_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  uint16 cur_value; /* unpacked attribute value to be evaluated */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  /* We assume the attribute ID passed in maps directly onto our
     hdrhai_protocol_name_enum_type */
  hdrhai_protocol_name_enum_type protocol_name =
    (hdrhai_protocol_name_enum_type) attrib_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRSCP_ATTRIB_LEN_SIZE + HDRSCP_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRSCP_SIMPLE_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {
    /* there is at least one more attribute value for this attribute */

    if (HDRSCP_UNPACK_CON_REQ_ATTRIBUTE_VALUE( item_ptr, (&cur_value),
        attrib_offset, attrib_val_offset) != E_SUCCESS)
    {
      ERR("Error unpacking configuration request message, ignoring attribute",
           0,0,0);
      /* if we can't get this value, there is no reason to get more values */
      break;
    }

    /* If main personality is not valid AN should not propose hardlink for any
     * protocol. Magic number zero is main personality's index which is always 
     * zero */
    if ((cur_value == HDRSCP_HARDLINK) && 
        !hdrscmdb_personality_is_valid(TRUE, 0))
    {
      val_is_acceptable = FALSE;
    }
    else
    {
      /* Check if this protocol subtype is valid for this protocol */
      switch (protocol_name)
      {
        case HDRHAI_PHYSICAL_LAYER_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_PHYS_LAYER) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ( (cur_value == HDRSCP_SUB2_PHYS_LAYER) && 
                 (hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer) ) ||
               ( (cur_value == HDRSCP_SUB3_PHYS_LAYER) && 
                 (hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer) )
             )
          {
            val_is_acceptable = TRUE;
          }
          break;

        case HDRHAI_SECURITY_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_SCP) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_GENERIC_SECURITY) &&
                ((hdrscp.session_params.current.subtype[HDRHAI_SECURITY_PROTOCOL] 
                  == HDRSCP_GENERIC_SECURITY) ||
                 hdrscmdb_nondefault_security_ps_is_allowed(
                     HDRHAI_SECURITY_PROTOCOL))))
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_AUTHENTICATION_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_AUTH) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_SHA1_AUTH) &&
                ((hdrscp.session_params.current.subtype[HDRHAI_AUTHENTICATION_PROTOCOL] 
                  == HDRSCP_SHA1_AUTH) ||
                 hdrscmdb_nondefault_security_ps_is_allowed(
                     HDRHAI_AUTHENTICATION_PROTOCOL))))
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_KEY_EXCHANGE_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_KEP) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_DH_KEY_EXCHANGE) &&
                ((hdrscp.session_params.current.subtype[HDRHAI_KEY_EXCHANGE_PROTOCOL] 
                  == HDRSCP_DH_KEY_EXCHANGE) ||
                 hdrscmdb_nondefault_security_ps_is_allowed(
                     HDRHAI_KEY_EXCHANGE_PROTOCOL))))
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_BROADCAST_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_BCMCS) ||
               (cur_value == HDRSCP_HARDLINK) ||
               (cur_value == HDRSCP_GENERIC_BCMCS) )
          {
            /* If the broadcast feature is disabled in NV and the AN is trying
               to negotiated the generic broadcast protocol suite, the AT
               will not accept this attribute.
              
               If Default BCMCS was also listed in the ConfigurationRequest, then 
               it will include this in the ConfigurationResponse message.  If 
               Generic BCMCS was the only choice, then the AT will not include 
               this attribute in the ConfigurationResponse.  Default BCMCS is used
               anyway in this case, as dictated by the standard.
            */
            if (!hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled &&
                (cur_value == HDRSCP_GENERIC_BCMCS) )
            {
              val_is_acceptable = FALSE;
            }
            else
            {
              val_is_acceptable = TRUE;
            }
          }
          break;
   
        case HDRHAI_FTC_MAC_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_FMAC) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_ENH_FMAC) && 
                 (hdrscp.permitted_subtypes.prot_subtype_mask.enh_fmac)) ||
               ((cur_value == HDRSCP_MC_FMAC) && 
                 (hdrscp.permitted_subtypes.prot_subtype_mask.mc_fmac)) )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_RTC_MAC_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_RMAC) || 
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_SUB1_RMAC) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1)) ||
               ((cur_value == HDRSCP_SUB3_RMAC) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3)) ||
               ((cur_value == HDRSCP_MC_RMAC) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac)) )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_IDLE_STATE_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_IDLE) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_ENH_IDLE) && 
                (hdrscp.permitted_subtypes.prot_subtype_mask.enh_idle)) || 
               ((cur_value == HDRSCP_QUICK_IDLE) && 
                (hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle))
#ifdef FEATURE_LTE_TO_HDR_OH
               || ((cur_value == HDRSCP_IRAT_IDLE) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap))
               || ((cur_value == HDRSCP_IRAT_QUICK_IDLE) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle))
#endif /* FEATURE_LTE_TO_HDR_OH */               
             )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_MMCDP) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_GENERIC_MMCDP) &&
                (hdrscp.permitted_subtypes.prot_subtype_mask.gmmcdp)) )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
#ifdef FEATURE_LTE_TO_HDR_OH             
        case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_SAP) ||
               ((cur_value == HDRSCP_IRAT_SAP) &&
                (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap)) )
          {
            val_is_acceptable = TRUE;
          }
          break;
#endif /* FEATURE_LTE_TO_HDR_OH */  
                       
        case HDRHAI_CC_MAC_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_CCMAC) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_ENH_CCMAC) &&
                (hdrscp.permitted_subtypes.prot_subtype_mask.enh_cmac)) )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_AC_MAC_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_ACMAC) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ( (cur_value == HDRSCP_ENH_ACMAC) &&
                 (hdrscp.permitted_subtypes.prot_subtype_mask.enh_amac) ) )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_ROUTE_UPDATE_PROTOCOL:
          if ( (cur_value == HDRSCP_DEFAULT_RUP) ||
               (cur_value == HDRSCP_HARDLINK) ||
               ((cur_value == HDRSCP_MC_RUP) &&
                (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rup)) 
#ifdef FEATURE_LTE_TO_HDR_OH
               || ((cur_value == HDRSCP_IRAT_RUP) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap))
               || ((cur_value == HDRSCP_IRAT_MC_RUP) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap) &&
                   (hdrscp.permitted_subtypes.prot_subtype_mask.mc_rup))
#endif /* FEATURE_LTE_TO_HDR_OH */
             )
          {
            val_is_acceptable = TRUE;
          }
          break;
   
        case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
        case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
        case HDRHAI_ENCRYPTION_PROTOCOL:
        case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
#ifdef FEATURE_LTE_TO_HDR_OH
          if ((cur_value == HDRSCP_IRAT_INIT) &&
              (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap))
          {
            val_is_acceptable = TRUE;
          }
#endif /* FEATURE_LTE_TO_HDR_OH */
        case HDRHAI_CONNECTED_STATE_PROTOCOL:
        case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
#ifdef FEATURE_LTE_TO_HDR_OH
          if ((cur_value == HDRSCP_IRAT_OVHD) &&
              (hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap))
          {
            val_is_acceptable = TRUE;
          }
#endif /* FEATURE_LTE_TO_HDR_OH */
        case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
        case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
        case HDRHAI_SIGNALING_LINK_PROTOCOL:
        case HDRHAI_PACKET_CONSOLIDATION_PROTOCOL:
        case HDRHAI_STREAM_PROTOCOL:
          if ((cur_value == HDRSCP_DEFAULT_PROTOCOL) ||
              (cur_value == HDRSCP_HARDLINK))
          {
            val_is_acceptable = TRUE;
          }
          break;

        default:
          ERR("unknown attrib ID in config req msg: %d",
              attrib_id,0,0);
          break;
      } /* switch */
    }

    if (val_is_acceptable)
    {
      /* Once we find an acceptable value we can stop */
      *protocol_subtype_ptr = cur_value;
      break;
    }
    attrib_val_offset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;
  } /* while */

  return (val_is_acceptable);

} /* hdrscp_process_protocol_subtype */

/*===========================================================================

FUNCTION HDRSCP_PROCESS_SIMPLE_ATTRIB

DESCRIPTION
  This function processes and extracts a simple attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr      - The DSM item containing the message.
  attrib_len    - The length of the attribute being processed.
  attrib_offset - The offset into the DSM item of the current attribute.
  attrib_id     - The simple attribute to be processed
  value_ptr     - A reference parameter to be set once the function processes
                  an acceptable attribute.

RETURN VALUE
  TRUE if the attribute was accepted; FALSE if the attribute was rejected.
  If the return value is TRUE, the accepted attribute value is written back
  through the value_ptr reference parameter.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_process_simple_attrib
(
  dsm_item_type *item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint16 attrib_id,
  uint16 *value_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the 
   * current attribute */
  uint16 attrib_val_offset; 
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint16 cur_value = 0; /* Current value being extracted */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRSCP_ATTRIB_LEN_SIZE +
                      HDRSCP_ATTRIB_ID_SIZE;

  while( (attrib_val_offset + 
          HDRSCP_SIMPLE_ATTRIB_VAL_SIZE) <= attrib_len )
  {
    if( HDRBIT_UNPACK16(
          item_ptr, 
          &cur_value,
          (attrib_offset + attrib_val_offset), 
          HDRSCP_SIMPLE_ATTRIB_VAL_SIZE ) != E_SUCCESS )
    {
      ERR( "Error unpacking Configuration Request message, ignoring", 0, 0, 0 );
      break;
    }

    switch (attrib_id)
    {
      case HDRSCP_PERSONALITY_CNT_ATTRIB_ID:
        if ( !hdrscp_phone_is_set_to_rev0_mode() ) 
        {
          /* Rev A */
          if( (cur_value >= HDRSCP_PERSONALITY_COUNT_ATTRIB_MIN_VAL) && 
              (cur_value <= HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL) )
          {
            val_is_acceptable = TRUE;
          }
        }
        else
        {
          /* Rev 0 */
          if( cur_value == HDRSCP_PERSONALITY_COUNT_ATTRIB_DEFAULT )
          {
            val_is_acceptable = TRUE;
          }
        }
        break;

      case HDRSCP_SESS_CFG_TOKEN_ATTRIB_ID:
      case HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_ID:
      case HDRSCP_SUPP_CFG_LOCK_ATTRIB_ID:
        /* All values are supported */
        if ( !hdrscp_phone_is_set_to_rev0_mode() )
        {
          /* All values are supported */
          val_is_acceptable = TRUE;
        }
        else
        {
          val_is_acceptable = FALSE;
        }
        break;
    
      default:
        break;
    }

    attrib_val_offset += HDRSCP_SIMPLE_ATTRIB_VAL_SIZE;

    if (val_is_acceptable)
    {
      break;
    }
  }

  *value_ptr = cur_value;
  return val_is_acceptable;

} /* hdrscp_process_simple_attrib() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request Message and Attribute 
  Update Message

  One principle behind the design processing the Configuration Request Message
  is that we should always be able to respond.  If they give an attribute ID
  that we don't understand, we can still respond with default.  We may not
  know what the attribute is, but the network does and it's up to the network
  to interpret the default value of an attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_req_msg
(
  dsm_item_type * item_ptr,
  hdrscp_creq_msg_enum_type msg_type
)
{

  int total_bits_remaining; /* number of bits remaining in the message */
  uint8  creq_transaction_id; /* Transaction ID of the message */

  uint8 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint16 attrib_id; /* ID field of attribute */

  /* If during processing we end up in a situation where we don't know what
   * attribute ID the AN included, or aren't sure if they meant to include
   * one, we can't send a response because we may be accepting to use the
   * default value for an attribute that we are not actually using the
   * default value for (e.g. if we negotiated a non-default value during
   * AT initiated negotiation).
   */
  boolean send_response = TRUE;

  /* As we parse the message we update the negotiated parameters in a
   * local copy as we agree to accept new values.  That way, if we
   * later encounter an error and decide not to send a response, or if
   * the same attribute is negotiated twice and we accept the first but
   * decline the second, we don't corrupt the real negotiated parameters.
   */

  /* For each simple attribute we need a flag if we are accpeting it
   * and the value we are accepting.  In this case there is a simple
   * attribute for each protocol, so we just use an array.
   */
  hdrscp_protocol_subtype_attrib_type
    protocol_subtype_attrib[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];

  /* For each complex attribute we need a flag if we are accpeting it,
   * the ValueID we are accepting, and a structure holding the accepted value.
   */

  dsm_item_type * send_msg_ptr; /* dsm buffer for sent message */

  int index; /* for looping through the array of protocol subtype attribs */

  struct
  {
    boolean is_accepted;
    uint16 value;
  } sess_cfg_token_attrib;

  struct
  {
    boolean is_accepted;
    uint16 value;
  } supp_gaup_sess_cfg_token_attrib;

  struct
  {
    boolean is_accepted;
    uint16 value;
  } personality_cnt_attrib;

  struct
  {
    boolean is_accepted;
    uint16 value;
  } supp_cfg_lock_attrib;

  /* Current state of SCP protocol */
  hdrscps_state_enum_type curr_state = hdrscps_get_state(); 

  /* This flag is set after processing SCT GAUP if AT's personality need
   * to be switched */
  boolean switch_personality = FALSE;
  /* Flag to indicate if GAUP message from AN is accepted or not */
  boolean gaup_is_accepted = TRUE;
  static hdrscp_scp_fail_ind_type gaup_fail_data;
  /* Data for AN GAUP failed indication */
  hdrind_ind_data_union_type ind_data;
  
  boolean protocol_st_attrib_is_included = FALSE;
  boolean sess_cfg_token_attrib_is_included = FALSE;
  boolean supp_gaup_sct_attrib_is_included = FALSE;
  boolean personality_count_attrib_is_included = FALSE;
  boolean supp_config_lock_attrib_is_included = FALSE;
  boolean unknown_attrib_is_included = FALSE;
  uint16 gaup_failed_attrib_id = 0;
  boolean non_gaupable_attrib_is_included = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sess_cfg_token_attrib.is_accepted = FALSE;
  supp_gaup_sess_cfg_token_attrib.is_accepted = FALSE;
  personality_cnt_attrib.is_accepted = FALSE;
  supp_cfg_lock_attrib.is_accepted = FALSE;

  memset (&sess_cfg_token_attrib, 0, sizeof (sess_cfg_token_attrib));

  if (msg_type == HDRSCP_CONFIG_REQ)
  {
    if ((curr_state == HDRSCPS_AN_INIT) ||
        ((curr_state == HDRSCPS_AT_INIT) && 
          hdrscp.config_complete.awaiting_slp_callback)) 
    {
      /* Process ConfigRequest messages during AN_INIT state or if the AT
       * is in AT_INIT state and has already sent a ConfigComplete message
       * but has just not received the SLP ACK yet.
       */
    }
    else
    {
      HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
         HDRSCP_CONFIG_REQ_MSG, curr_state); //lint !e506
      return;
    }
  }

  if (msg_type == HDRSCP_GAUP_REQ)
  {
    if ((curr_state == HDRSCPS_OPEN) || (curr_state == HDRSCPS_PS_GAUP))
    {
      /* Process GAUP request only in OPEN and PS_GAUP state */
    }
    else 
    {
      HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
         HDRSCP_ATTRIB_UPDATE_REQ_MSG, curr_state); //lint !e506

      return;
    }
  }

  total_bits_remaining = (dsm_length_packet(item_ptr) * HDRSCP_BITS_PER_BYTE);

  /* Extract the Transaction ID */
  if (HDRSCP_UNPACK_TRANS_ID(item_ptr, &creq_transaction_id) !=
      E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigReq Msg",0,0,0);
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* initialize all protocol subtype attributes to be "not accepted" */
  for (index = 0; index < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; index++)
  {
    protocol_subtype_attrib[index].is_accepted = FALSE;
  }

  /* initialize attrib len so the initial attrib_offset will be
     immediately after the transaction ID of the config request */
  attrib_len = HDRSCP_MSG_ID_SIZE + HDRSCP_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* continue through message until there's not enough bits for a minimal
     attribute */
  while ((total_bits_remaining - attrib_len) >= HDRSCP_MIN_ATTRIB_LEN)
  {
    /* move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    if (HDRSCP_UNPACK_CON_REQ_LEN(item_ptr, &attrib_len, attrib_offset) !=
        E_SUCCESS)
    {
      ERR("Error unpacking ConfigReq Msg", 0, 0, 0 );
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in
     * bytes, excluding the length of the length field.  For our purposes,
     * we want attrib_len to be the total length of the attribute in bits,
     * including the length field.
     */
    attrib_len *= HDRSCP_BITS_PER_BYTE;
    attrib_len += HDRSCP_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRSCP_MIN_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      /* If the length is something unexpected, we can't be sure of what
       * the AN intended and it is safest to not send a response
       */
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if (HDRSCP_UNPACK_CON_REQ_ATTRIBUTE_ID(item_ptr,
                 &attrib_id, attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }

    /* perform necessary processing for various attribute IDs */
    switch (attrib_id)
    {
      /* these attribute IDs all have the same processing */
      case HDRHAI_SECURITY_PROTOCOL:
      case HDRHAI_AUTHENTICATION_PROTOCOL:
      case HDRHAI_KEY_EXCHANGE_PROTOCOL:
      case HDRHAI_PHYSICAL_LAYER_PROTOCOL:
      case HDRHAI_CC_MAC_PROTOCOL:
      case HDRHAI_AC_MAC_PROTOCOL:
      case HDRHAI_FTC_MAC_PROTOCOL:
      case HDRHAI_RTC_MAC_PROTOCOL:
      case HDRHAI_ENCRYPTION_PROTOCOL:
      case HDRHAI_PACKET_CONSOLIDATION_PROTOCOL:
      case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      case HDRHAI_IDLE_STATE_PROTOCOL:
      case HDRHAI_CONNECTED_STATE_PROTOCOL:
      case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      case HDRHAI_SIGNALING_LINK_PROTOCOL:
      case HDRHAI_BROADCAST_PROTOCOL:
      case HDRHAI_STREAM_PROTOCOL:
      case HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL:
      case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
        protocol_st_attrib_is_included = TRUE;
        /* the given length must be enough for at least one attribute */
        if (attrib_len >= HDRSCP_SIMPLE_ATTRIB_LEN)
        {
          /* only process this attribute ID if we haven't yet found an
             acceptable value in this configuration request message */
          if (!(protocol_subtype_attrib[attrib_id].is_accepted))
          {
            protocol_subtype_attrib[attrib_id].is_accepted =
              hdrscp_process_protocol_subtype(
                item_ptr, attrib_len, attrib_offset, attrib_id,
                &protocol_subtype_attrib[attrib_id].value);
          }
        }
        else
        {
          ERR("Not enough bits to get attribute %d",attrib_id,0,0);
          send_response = FALSE;
        }
        break;

      /*-----------------------------------------------------------------------
      SessionConfigurationToken
      -----------------------------------------------------------------------*/
      case HDRSCP_SESS_CFG_TOKEN_ATTRIB_ID:
        sess_cfg_token_attrib_is_included = TRUE;
        if( attrib_len >= HDRSCP_SIMPLE_ATTRIB_LEN )
        {
          sess_cfg_token_attrib.is_accepted = 
            hdrscp_process_simple_attrib( 
              item_ptr, attrib_len, attrib_offset, attrib_id,
              &sess_cfg_token_attrib.value );
        }
        else
        {
          ERR( "Not enough bits to get attribute %d", attrib_id, 0, 0 );
          send_response = FALSE;
        }

        break;

      /*-----------------------------------------------------------------------
      SupportGAUPSessionConfigurationToken
      -----------------------------------------------------------------------*/
      case HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_ID:
        supp_gaup_sct_attrib_is_included = TRUE;
        if( attrib_len >= HDRSCP_SIMPLE_ATTRIB_LEN )
        {
          supp_gaup_sess_cfg_token_attrib.is_accepted =
            hdrscp_process_simple_attrib( 
              item_ptr, attrib_len, attrib_offset, attrib_id,
              &supp_gaup_sess_cfg_token_attrib.value );
        }
        else
        {
          ERR( "Not enough bits to get attribute %d", attrib_id, 0, 0 );
          send_response = FALSE;
        }

        break;

      /*-----------------------------------------------------------------------
      PersonalityCount
      -----------------------------------------------------------------------*/
      case HDRSCP_PERSONALITY_CNT_ATTRIB_ID:
        personality_count_attrib_is_included = TRUE;
        if( attrib_len >= HDRSCP_SIMPLE_ATTRIB_LEN )
        {
          personality_cnt_attrib.is_accepted =
            hdrscp_process_simple_attrib( 
              item_ptr, attrib_len, attrib_offset, attrib_id,
              &personality_cnt_attrib.value );
        }
        else
        {
          ERR( "Not enough bits to get attribute %d", attrib_id, 0, 0 );
          send_response = FALSE;
        }

        break;

      /*-----------------------------------------------------------------------
      SupportConfigurationLock
      -----------------------------------------------------------------------*/
      case HDRSCP_SUPP_CFG_LOCK_ATTRIB_ID:
        supp_config_lock_attrib_is_included = TRUE;
        if( attrib_len >= HDRSCP_SIMPLE_ATTRIB_LEN )
        {
          supp_cfg_lock_attrib.is_accepted =
            hdrscp_process_simple_attrib( 
              item_ptr, attrib_len, attrib_offset, attrib_id,
              &supp_cfg_lock_attrib.value );
        }
        else
        {
          ERR( "Not enough bits to get attribute %d", attrib_id, 0, 0 );
          send_response = FALSE;
        }

        break;

      default:
        unknown_attrib_is_included = TRUE;
        ERR("unknown or invalid attrib ID %d",attrib_id,0,0);
        break;
    }

    /* if we are not sending a response, there is no reason to continue
       with this while loop */
    if (!send_response)
    {
      break;
    }
  }

  if (msg_type == HDRSCP_GAUP_REQ)
  {
    if (protocol_st_attrib_is_included || supp_gaup_sct_attrib_is_included ||
        personality_count_attrib_is_included || 
        supp_config_lock_attrib_is_included || unknown_attrib_is_included)
    {
      non_gaupable_attrib_is_included = TRUE;
    }

    if (hdrscp.session_params.current.support_gaup_session_config_token != 0x0001)
    {
      /* When SupportGAUPConfigToken is not set check the GAUP'ed token value*/
      if ( sess_cfg_token_attrib_is_included && (sess_cfg_token_attrib.value == 
            hdrscp.session_params.current.config_token) &&
           !hdrscp_phone_is_set_to_rev0_mode() && 
           !non_gaupable_attrib_is_included )
      {
        /* If GAUP'ed config token matches the current config token and no other
         * attribute is included then send AUA */
      }
      else
      {
        /* If it is not included or if GAUP'ed config token does not match the
         * current config token do not send a response, however we do not want
         * to close the session */
        send_response = FALSE;
      }
    }
    else if (non_gaupable_attrib_is_included || 
             (sess_cfg_token_attrib_is_included &&
           !hdrscmdb_personality_is_valid(TRUE, 
                                          (sess_cfg_token_attrib.value>>12))))
    {
      /* If Support GAUP SCT is set then check if GAUP message is valid, if not
       * send session close */
      send_response = FALSE;
      gaup_is_accepted = FALSE;

      if (sess_cfg_token_attrib_is_included &&
            !hdrscmdb_personality_is_valid(TRUE, 
                                           (sess_cfg_token_attrib.value>>12)))
      {
        gaup_failed_attrib_id = HDRSCP_SESS_CFG_TOKEN_ATTRIB_ID;
      }
      else if (supp_gaup_sct_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_ID;
      }
      else if (personality_count_attrib_is_included) 
      {
        gaup_failed_attrib_id = HDRSCP_PERSONALITY_CNT_ATTRIB_ID;
      }
      else if (supp_config_lock_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRSCP_SUPP_CFG_LOCK_ATTRIB_ID;
      }
    }
  }

  if (send_response)
  {
    if ((msg_type == HDRSCP_CONFIG_REQ) &&
        (curr_state == HDRSCPS_AT_INIT) &&
        hdrscp.config_complete.awaiting_slp_callback)
    {
      /* We assume that we missed the ack to the config complete
         and just start AN initiated negotiation. */
      hdrscp.config_complete.awaiting_slp_callback = FALSE;
      (void)hdrhmp_abort_msg(hdrscp_complete_tx_outcome_cb, NULL);

      ASSERT( hdrscp.negotiation_state != HDRSCP_NEG_NONE );
      hdrscps_change_state(HDRSCPS_AN_INIT);
    }

    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* pack message ID and transaction ID */
    if( msg_type == HDRSCP_GAUP_REQ )
    {
      HDRSCP_PACK_CON_RSP_MSG_ID( send_msg_ptr, 
                                  HDRSCP_ATTRIB_UPDATE_ACCEPT_MSG );
    }
    else
    {
    HDRSCP_PACK_CON_RSP_MSG_ID( send_msg_ptr, HDRSCP_CONFIG_RSP_MSG);
    }
    
    HDRSCP_PACK_CON_RSP_TRANS_ID( send_msg_ptr, creq_transaction_id);

    /* Now pack in the attribute ID and value of all attributes we have
     * flagged that we want to accept.
     */

    /* attrib_offset is now offset for response message */
    attrib_offset = HDRSCP_MSG_ID_SIZE + HDRSCP_TRANSACTION_ID_SIZE;

    /* Go through our array of simple protocol subtype attributes */
    for (index = 0; index < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; index++)
    {
      /* if we accepted this attribute */
      if (protocol_subtype_attrib[index].is_accepted)
      {
        /* pack length, attrib ID, and accepted value */
        HDRSCP_PACK_CON_RSP_LEN(send_msg_ptr,
          HDRSCP_SIMPLE_ATTRIB_LEN_BYTES,
          attrib_offset);

        /* The array index is the attribute ID */
        HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID(send_msg_ptr,
          (uint8) index, attrib_offset);

        HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE(send_msg_ptr,
          protocol_subtype_attrib[index].value, attrib_offset);

        /* Here we compare with "negotiated" instance instead of "current"
           because it is posssible in AN_INIT phase AN could re-propose the
           same subtype that AT had proposed and AN had accepted in AT_INIT
           phase. This should not be considered as subtype change. */
        if (hdrscp.session_params.negotiated.subtype[index] != 
            protocol_subtype_attrib[index].value)
        {
          hdrscmdb_protocol_subtype_changed(
            (hdrhai_protocol_name_enum_type)index, 
            protocol_subtype_attrib[index].value);
        }

        /* save accepted value into negotiated */
        hdrscp.session_params.negotiated.subtype[index] =
          protocol_subtype_attrib[index].value;

        attrib_offset += HDRSCP_SIMPLE_ATTRIB_LEN;

        /* //NMN Currently the key exchange, authentication and security
         * protocols are the only ones that have non-default subtypes,
         * and they call hdrscp_get_current_subtype() and
         * hdrscp_get_config_subtype() when needed to tell what subtype
         * is active.  In the future we may want to command protocols
         * to change their "in-config subtype" at this point.
         */
      }
    } /* for */

    /* Pack the SessionConfigurationToken attribute */
    if( sess_cfg_token_attrib.is_accepted )
    {
      if( msg_type == HDRSCP_GAUP_REQ )
      {
        if ((hdrscp.session_params.current.config_token>>12) !=
            (sess_cfg_token_attrib.value>>12))
        {
          /* Switch personality only if the 4MSB of current and GAUP'ed SCT value
           * are different */
          switch_personality = TRUE;
        }

        hdrscp.session_params.negotiated.config_token = 
            sess_cfg_token_attrib.value;

        hdrscp.session_params.current.config_token = 
            sess_cfg_token_attrib.value;

        hdrscmdb_set_session_config_token( sess_cfg_token_attrib.value );

        if (switch_personality)
        {
          if (hdralmp_connection_is_open(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) ||
		  	  (hdrrup_is_tc_assigned(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) == TRUE))
          {
            /* Flag to indicate SessionConfigurationToken has changed, the
             * actual personality switching will happen when connection is
             * closed (or) connection setup failed */
            hdrscp.session_cfg_token_changed = TRUE;    
			HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                    "switch personality delayed : Current state of RUP : %d", hdrrups_get_state() );
          }
          else
          {
            /* Switch to new personality; this personality switch 
               is activated during the IDLE state */
            hdrscmdb_session_config_token_changed( HDRHAI_SCHG_PERSONALITY_SWITCH );
#ifdef FEATURE_EHRPD
            /* Check whether eHRPD availability is still satisfied after 
             * personality switch. */
            (void) hdrscp_check_ok_to_open_state();
#endif /* FEATURE_EHRPD */ 
          }
        }
      }
      else
      {
        HDRSCP_PACK_CON_RSP_LEN( send_msg_ptr, 
          HDRSCP_SIMPLE_ATTRIB_LEN_BYTES, attrib_offset );
        HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr, 
          HDRSCP_SESS_CFG_TOKEN_ATTRIB_ID, attrib_offset );
        HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE( send_msg_ptr, 
          sess_cfg_token_attrib.value, attrib_offset );

        attrib_offset += HDRSCP_SIMPLE_ATTRIB_LEN;

        hdrscp.session_params.negotiated.config_token = 
          sess_cfg_token_attrib.value;
      }
    }

    /* Pack the SupportGAUPSessionConfigurationToken attribute */
    if( supp_gaup_sess_cfg_token_attrib.is_accepted )
    {
      HDRSCP_PACK_CON_RSP_LEN( send_msg_ptr, 
        HDRSCP_SIMPLE_ATTRIB_LEN_BYTES, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr, 
        HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_ID, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE( send_msg_ptr,
        supp_gaup_sess_cfg_token_attrib.value, attrib_offset );

      attrib_offset += HDRSCP_SIMPLE_ATTRIB_LEN;

      hdrscp.session_params.negotiated.support_gaup_session_config_token =
        supp_gaup_sess_cfg_token_attrib.value;
    }

    /* Pack the PersonalityCount attribute */
    if( personality_cnt_attrib.is_accepted )
    {
      HDRSCP_PACK_CON_RSP_LEN( send_msg_ptr, 
        HDRSCP_SIMPLE_ATTRIB_LEN_BYTES, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRSCP_PERSONALITY_CNT_ATTRIB_ID, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE( send_msg_ptr,
        personality_cnt_attrib.value, attrib_offset );

      attrib_offset += HDRSCP_SIMPLE_ATTRIB_LEN;

      hdrscp.session_params.negotiated.personality_count = 
        personality_cnt_attrib.value;
    }

    /* Pack the SupportConfigurationLock attribute */
    if( supp_cfg_lock_attrib.is_accepted )
    {
      HDRSCP_PACK_CON_RSP_LEN( send_msg_ptr, 
        HDRSCP_SIMPLE_ATTRIB_LEN_BYTES, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRSCP_SUPP_CFG_LOCK_ATTRIB_ID, attrib_offset );
      HDRSCP_PACK_CON_RSP_ATTRIBUTE_VALUE( send_msg_ptr,
        supp_cfg_lock_attrib.value, attrib_offset );

      attrib_offset += HDRSCP_SIMPLE_ATTRIB_LEN;

      hdrscp.session_params.negotiated.support_config_lock =
        supp_cfg_lock_attrib.value;
    }

    /* if (accept_some_other_attribute) ... */

    if( msg_type == HDRSCP_GAUP_REQ )
    {
      /* send GAUP accept Message */
      hdrhmp_send_msg( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, /* protocol */
                       HDRHAI_AC_OR_RTC,
                       TRUE, /* reliable = TRUE, best effort = FALSE */
                       HDRSCP_MSG_PRIORITY, /* priority */
                       HDRHAI_IN_USE, /* Instance of protocol sending message */
                       send_msg_ptr, /* pointer to the message to be delivered */
                       NULL, /* function to call for outcome */
                       NULL); /* data to deliver to outcome callback */
    }
    else
    {
      /* send Configuration Response Message */
      hdrhmp_send_msg( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, /* protocol */
                       HDRHAI_RTC,
                       TRUE, /* reliable = TRUE, best effort = FALSE */
                       HDRSCP_MSG_PRIORITY, /* priority */
                       HDRHAI_IN_USE, /* Instance of protocol sending message */
                       send_msg_ptr, /* pointer to the message to be delivered */
                       NULL, /* function to call for outcome */
                       NULL); /* data to deliver to outcome callback */
    }
  }
  else
  {
    if (!gaup_is_accepted)
    {
      gaup_fail_data.reason = HDRHAI_CLOSE_AN_GAUP_FAIL;
      gaup_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
      gaup_fail_data.subtype = HDRSCP_DEFAULT_SCP;
      gaup_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;
      gaup_fail_data.attribute_id = gaup_failed_attrib_id;

      ind_data.scp_fail_ptr = (void *)&gaup_fail_data;

      hdrind_give_ind(HDRIND_SCP_AN_GAUP_FAILED, (void*)&ind_data);
    }
  }

} /* hdrscp_process_config_req_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_RSP_MSG

DESCRIPTION
  This function processes the Configuration Response Message

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the received message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_rsp_msg
(
  dsm_item_type * item_ptr
)
{

  uint8  crsp_transaction_id; /* Transaction ID of the message */
  uint16 total_bits_remaining; /* Number of bits left to be unpacked */
  uint8 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint16  attrib_id; /* ID field of current attribute being unpacked */
  uint16 attrib_value; /* Value of ptotocol being unpacked */
  int index; /* for looping through the array of protocol subtype attribs */

  /* For each simple attribute we need a flag if it was included in this
     response, and the value that was included. */
  hdrscp_protocol_subtype_attrib_type
    protocol_subtype_attrib[HDRHAI_PROTOCOL_TYPE_ENUM_MAX];

  uint8 value_id; /* value ID of previous session attribute unpacked */

  /* Flag if we should fail negotiation based on this config response */
  boolean negotiation_failed = FALSE;

  /* Flag if there was an error with the message structure */
  boolean message_parsing_error = FALSE;

  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;

  /* Flag indicating whether transaction ID in the recvd Configresp is valid */
  boolean transaction_id_is_valid = FALSE;

  /* Flag indicating whether the recvd Configresp is for PriorSession attrib */
  boolean recvd_resp_is_for_prior_session = FALSE;
  boolean prior_session_rejected = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscps_get_state() != HDRSCPS_AT_INIT)
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
       HDRSCP_CONFIG_RSP_MSG, hdrscps_get_state()); //lint !e506

    return;
  }

  total_bits_remaining = dsm_length_packet(item_ptr) * HDRSCP_BITS_PER_BYTE;

  /* Extract the Transaction ID from the message */
  if (HDRSCP_UNPACK_TRANS_ID(item_ptr, &crsp_transaction_id) != E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigRsp Msg", 0, 0, 0 );
    return;
  }

  /* Loop through and find if the transaction ID matches with one of the 
     request that we sent */ 
  for (index = 0; index < hdrscp.config_req_info.num_cfg_req_sent; index++)
  {
    if (hdrscp.config_req_info.mapping_req_resp[index].transaction_id ==
        crsp_transaction_id)
    {
      transaction_id_is_valid = TRUE;
       
      /* If the transaction id matched than check if the response is for
         Prior Session attribute */
      if (hdrscp.config_req_info.mapping_req_resp[index].attrib_id == 
          HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID )
      {
        recvd_resp_is_for_prior_session = TRUE;
        prior_session_rejected = TRUE;
      }
      break;   
    }
  }

  /* If transaction ID doesn't match, ignore message. */
  if (!transaction_id_is_valid)
  {
     HDRTRACE_RXMSG_IGNORED_SEQ( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      HDRSCP_CONFIG_RSP_MSG, hdrscps_get_state()); //lint !e506
    return;
  }

  /* Initialize all protocol subtype attributes as not included with its value
     same as current.  The current attribute value will be used if that 
     attribute was proposed by the AT but the AN didn't include it in the 
     response.  In this case, the AT uses the current fallback value instead.*/
  for (index = 0; index < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; index++)
  {
    protocol_subtype_attrib[index].is_accepted = FALSE;

    protocol_subtype_attrib[index].value =
      hdrscp.session_params.current.subtype[index];
  }

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config response.
   */
  attrib_len = HDRSCP_MSG_ID_SIZE + HDRSCP_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */
  while ((total_bits_remaining - attrib_len) >=
          (HDRSCP_MIN_ATTRIB_LEN + HDRSCP_VALUE_ID_SIZE))
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    if (HDRSCP_UNPACK_CON_RSP_LEN(item_ptr, &attrib_len,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Response Message, ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in
     * bytes, excluding the length of the length field.  For our purposes,
     * we want attrib_len to be the total length of the attribute in bits,
     * including the length field.
     */
    attrib_len *= HDRSCP_BITS_PER_BYTE;
    attrib_len += HDRSCP_ATTRIB_LEN_SIZE;

    /* In a config response, we can have 8 bit ValueIDs or 16 bit protocol
       subtype values.  For now check for smallest possible, at least */
    if ((attrib_len < (HDRSCP_MIN_ATTRIB_LEN + HDRSCP_VALUE_ID_SIZE)) ||
        (attrib_len > total_bits_remaining))
    {
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (HDRSCP_UNPACK_CON_RSP_ATTRIBUTE_ID(item_ptr, &attrib_id,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking creq_attrib_id",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (attrib_id >= HDRHAI_PROTOCOL_TYPE_ENUM_MAX &&
        attrib_id != HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID &&
        attrib_id != HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_ID &&
        attrib_id != HDRSCP_PERSONALITY_CNT_ATTRIB_ID )
    {
      /* Accepting a different ValueID than we proposed is a failure */
      negotiation_failed = TRUE;
      break;
    }

    switch (attrib_id)
    {
      case HDRSCP_AT_SUPP_APP_SUBTYPES_ATTRIB_ID:
        if( HDRSCP_UNPACK_CON_RSP_VALUE_ID( 
              item_ptr, &value_id, attrib_offset ) != E_SUCCESS )
        {
          ERR( "Error unpacking Configuration Rsp message, ignoring", 0, 0, 0 );
          message_parsing_error = TRUE;
          break;
        }

        /* If the value id was successfully unpacked ... 
           ignore for now, we don't care about the AN's response */

        break;

      case HDRSCP_PERSONALITY_CNT_ATTRIB_ID:
        if( attrib_len < ( HDRSCP_MIN_ATTRIB_LEN + HDRSCP_SIMPLE_ATTRIB_VAL_SIZE ) )
        {
          /* Not enough bits for our 16 bit protocol subtype */
          ERR( "Error, invalid total len vs bits in message. ignoring", 0, 0, 0 );
          message_parsing_error = TRUE;
          break;
        }

        if( HDRSCP_UNPACK_CON_RSP_ATTRIBUTE_VALUE( 
              item_ptr, &attrib_value, attrib_offset ) != E_SUCCESS )
        {
          ERR( "Error unpacking Configuration Rsp message, ignoring", 0, 0, 0 );
          message_parsing_error = TRUE;
          break;
        }

        if ((hdrscp.config_req_info.sent_personality_count_attrib) &&
            (attrib_value == HDRSCP_PERSONALITY_COUNT_ATTRIB_MAX_VAL))
        {
          hdrscp.session_params.negotiated.personality_count = attrib_value;
        }
        else
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
          break;
        }

        break;

      case HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID:
        /* if AN does not include prior session attribute in its ConfigResp,
           (if AT requested it in ConfigReq), it is an explicit rejection,
           and AT will invalidate prior session later */
        prior_session_rejected = FALSE;
        if (HDRSCP_UNPACK_CON_RSP_VALUE_ID(item_ptr,
                         &value_id, attrib_offset) != E_SUCCESS)
        {
          ERR("Error unpacking Configuration Rsp message, ignoring",0,0,0);
          return;
        }

        if ((!hdrscp.config_req_info.sent_previous_session_attrib) ||
            (value_id == hdrscp.config_req_info.value_id))
        {
          hdrscp.config_req_info.prior_session_is_accepted = TRUE;
        }
        else
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
          break;
        }
        break;

      case HDRHAI_KEY_EXCHANGE_PROTOCOL:
      case HDRHAI_SECURITY_PROTOCOL:
      case HDRHAI_AUTHENTICATION_PROTOCOL:
      case HDRHAI_PHYSICAL_LAYER_PROTOCOL:
      case HDRHAI_CC_MAC_PROTOCOL:
      case HDRHAI_AC_MAC_PROTOCOL:
      case HDRHAI_FTC_MAC_PROTOCOL:
      case HDRHAI_RTC_MAC_PROTOCOL:
      case HDRHAI_ENCRYPTION_PROTOCOL:
      case HDRHAI_PACKET_CONSOLIDATION_PROTOCOL:
      case HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL:
      case HDRHAI_INITIALIZATION_STATE_PROTOCOL:
      case HDRHAI_IDLE_STATE_PROTOCOL:
      case HDRHAI_CONNECTED_STATE_PROTOCOL:
      case HDRHAI_ROUTE_UPDATE_PROTOCOL:
      case HDRHAI_OVERHEAD_MESSAGES_PROTOCOL:
      case HDRHAI_SESSION_MANAGEMENT_PROTOCOL:
      case HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL:
      case HDRHAI_SESSION_CONFIGURATION_PROTOCOL:
      case HDRHAI_SIGNALING_LINK_PROTOCOL:
      case HDRHAI_RADIO_LINK_PROTOCOL:
      case HDRHAI_HDR_MESSAGING_PROTOCOL:
      case HDRHAI_LOCATION_UPDATE_PROTOCOL:
      case HDRHAI_BROADCAST_PROTOCOL:
      case HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL:
      case HDRHAI_STREAM_PROTOCOL:
      case HDRHAI_SIGNALING_ADAPTATION_PROTOCOL:
        if (attrib_len < (HDRSCP_MIN_ATTRIB_LEN + HDRSCP_SIMPLE_ATTRIB_VAL_SIZE))
        {
          /* Not enough bits for our 16 bit protocol subtype */
          ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
          message_parsing_error = TRUE;
          break;
        }

        if (HDRSCP_UNPACK_CON_RSP_ATTRIBUTE_VALUE(item_ptr,
                         &attrib_value, attrib_offset) != E_SUCCESS)
        {
          ERR("Error unpacking Configuration Rsp message, ignoring",0,0,0);
          message_parsing_error = TRUE;
          break;
        }

        if (hdrscp_attrib_was_sent( attrib_id, attrib_value ))
        {
          protocol_subtype_attrib[attrib_id].is_accepted = TRUE;
          protocol_subtype_attrib[attrib_id].value = attrib_value;
        }
        else
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
          break;
        }
        break;

      default:
        /* Accepting an attribute we never negotiated is grounds for failure */
        negotiation_failed = TRUE;
        break;
    } /* switch */

    if ((negotiation_failed) || (message_parsing_error))
    {
      break;
    }
  } /* while total bytes unprocessed > 0 */

  if (message_parsing_error)
  {
    /* If we had an unexpected problem parsing the message then it is better
     * to just ignore it.  If the AN wants to send a valid response it still
     * can, or else our config response timer will expire and we will abort.
     */
  }
  else if (negotiation_failed)
  {
    hdrscp_fail_data.reason = HDRHAI_CLOSE_PROT_NEG_FAIL;
    hdrscp_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
    hdrscp_fail_data.subtype = HDRSCP_DEFAULT_SCP;
    hdrscp_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;
    hdrscp_fail_data.attribute_id = attrib_id; //lint !e771 attrib is init
    ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
    hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
  }
  else
  {

    /* after this point, the config rsp message is completely processed, and
       the relevant data is saved.  The data is now processed to determine
       what to do next. */

    if (recvd_resp_is_for_prior_session)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,"Rcvd ConfigRsp for PriorSession ConfigReq");

      /* Set that we are not anymore waiting for ConfigResp message
         for Prior Session attribute */
      hdrscp.config_req_info.awaiting_l3_ack &= 
                            ~HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB;

      /* If we received the ConfigResp for Prior Session attrib AT need not wait
         for SLP Ack, so abort waiting for SLP Ack */
      if ((hdrscp.config_req_info.awaiting_slp_ack & 
             HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB))
      {
        /* The flag must be set first because hdrhmp_abort_msg() causes
         * the callback to be called, and if this flag is not FALSE
         * the callback will set the config response timer, for example.
         */
        hdrscp.config_req_info.awaiting_slp_ack &= 
                               ~HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB;
        (void)hdrhmp_abort_msg(hdrscp_config_tx_outcome_cb, &attrib_id);
      }
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,
                   "Rcvd ConfigRsp for Rest of SCP's Config attributes");

      /* Set that we are not anymore waiting for ConfigResp message
         for rest of the configuration attributes */
      hdrscp.config_req_info.awaiting_l3_ack &= 
                            ~HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB;

      if (hdrscp.config_req_info.awaiting_slp_ack & 
             HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB)
      {
        /* The flag must be set first because hdrhmp_abort_msg() causes
         * the callback to be called, and if this flag is not FALSE
         * the callback will set the config response timer, for example.
         */
        hdrscp.config_req_info.awaiting_slp_ack &= 
                               ~HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB;
        (void)hdrhmp_abort_msg(hdrscp_config_tx_outcome_cb, NULL);
      }
    }

    /* If AT is not waiting for ConfigResp/SLP Ack for both the SCP ConfigReq 
       messages that AT sent then  we can clear the timer and do rest of the 
       processing */
    if (!hdrscp.config_req_info.awaiting_l3_ack && 
        !hdrscp.config_req_info.awaiting_slp_ack)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED,
         "Rcvd ConfigRsp for all of SCP's ConfigReq msgs, clearing timer");

      /* Message is a response to our config request. Stop and clear timer */
      (void) rex_clr_timer( &hdrscp_config_timer );
      (void) rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);

      if (hdrscp.config_req_info.prior_session_is_accepted)
      {
        /* Clear the flag */
        hdrscp.config_req_info.prior_session_is_accepted = FALSE;
        hdrscp_send_config_complete_msg();
      }
      else /* not using previous session - examine configuration */
      {
        if (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION)
        {
          HDR_MSG_PROT (MSG_LEGACY_HIGH,
                        "AN refused attempt to restore previous session ");
          hdrscp.negotiation_state = HDRSCP_NEG_AT_MAIN_PERSONALITY;
          /* if this ConfigResp has the same trans_id as AT's ConfigReq with
             PrirorSession attrib, but AN did not include this attrib_id,
             consider this as an explicit rejection, invalide prior session,
             all the other cases, ie, time out, value_id mismatch, etc,
             still keep prior session around */
          if (recvd_resp_is_for_prior_session && prior_session_rejected)
          {
            hdrscmdb_set_prior_session_validity(FALSE);
          }
          hdrind_give_ind( HDRIND_SCP_NO_PREV_SESSION, NULL );
        }
        else
        {
          ASSERT( hdrscp.negotiation_state != HDRSCP_NEG_NONE );
        }
        
        /* Successful negotiation, copy all accepted attributes into
           negotiated. */
        for (index = 0; index < HDRHAI_PROTOCOL_TYPE_ENUM_MAX; index++)
        {
          if (protocol_subtype_attrib[index].is_accepted)
          {
            hdrscp.session_params.negotiated.subtype[index] = 
              protocol_subtype_attrib[index].value;

            if (hdrscp.session_params.current.subtype[index] != 
                hdrscp.session_params.negotiated.subtype[index])
            {
              hdrscmdb_protocol_subtype_changed(
                (hdrhai_protocol_name_enum_type) index, 
                hdrscp.session_params.negotiated.subtype[index]);
            }
          }

          /* Note that if we proposed a value that was not accepted, we need to
           * use the fall-back value.  In this case, since the AT only proposes
           * subtypes one time per negotiation, and since the AT goes first,
           * the negotiated value is already set to the fall-back value.
           */
        }

        /* Note that it may be OK for the subtype configuration to be
         * inconsistent at the end of AN initiated negotiation.  If the
         * AN doesn't clean it up before we finish, we will fail at that point
         */

        /* AT Initiated SCP config is completed. All In_Config protocol subtypes
           have been agreed upon so SCP can now initiate the configuration of 
           all other protocols. */
        hdrscp_command_protocols_to_configure( FALSE );

        /* No further negotiation is needed, complete AT Initiated round of
           negotiation and transition to AN_INIT. */
        if (!hdrscp_waiting_for_protocols())
        {
          /* This happens for AN initiated negotiation, and possibly if the AT 
           * was interrupted during AT initiated state of an AT initiated
           * negotiation, after all protocols had signaled they were done but
           * SCP had not yet entered HDRSCPS_AT_COMPLETE_SUBSTATE.
           */
          HDR_MSG_PROT (MSG_LEGACY_HIGH, 
                        "No AT initiated attributes at this time");
          hdrscp_send_config_complete_msg();
        }

      } /* else not using previous session */

      /* Config request info is no longer needed */
      hdrscp_reset_config_req_info();
    }

  } /* else (no parsing error or negotiation failure) */
} /* hdrscp_process_config_rsp_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_AN_INIT_STATE_TIMER_EXP

DESCRIPTION
  This function processes the AN Init State Timer expiration

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_an_init_state_timer_exp(void)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* SCP fails because we spent too long in the AN Init State.  This is our
     own timer, not part of the spec. */
  hdrscp_fail_data.reason = HDRHAI_CLOSE_AN_INIT_EXP;
  ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
  hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);

} /* hdrscp_process_an_init_state_timer_exp */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CONFIG_RSP_TIMER_EXP

DESCRIPTION
  This function processes the Configuration Response Timer expiration

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdrscp_process_config_rsp_timer_exp(void)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* SCP fails because AN took too long to send the Config Response msg */
  hdrscp_fail_data.reason = HDRHAI_CLOSE_CFG_RSP_EXP;
  hdrscp_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
  hdrscp_fail_data.subtype = HDRSCP_DEFAULT_SCP;
  hdrscp_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;

  if (hdrscp.config_req_info.sent_previous_session_attrib)
  {
    /* It must have been the prior session attribute*/
    hdrscp_fail_data.attribute_id = HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID;
  }
  else 
  {
    hdrscp_fail_data.attribute_id = 
      hdrscp.config_req_info.ptype_attrib[0].type;
  }

  ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
  hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
} /* hdrscp_process_config_rsp_timer_exp */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_DEACTIVATE

DESCRIPTION
  This function processes the Deactivate Command

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes State.

===========================================================================*/
LOCAL void hdrscp_process_deactivate
(
  hdrscp_cmd_type * scp_cmd_ptr
)
{
  /* We set the prior session flag to false by default.  It will be overwritten
   * as needed if the deactivate command is processed.
   * If we consider the following scenario:
   * We have already closed the session and decided that the prior session will
   * need to be restored next time we open the session. If we close the session
   * again before it is reopened, then we should not try to restore the prior
   * session. The rational is that we always save the current session info as
   * previous session info when closing a session. This will lead to invalid
   * data if we save info from a session that is not opened and negotiated.
   * A better fix would be to only save the previous session info when we have
   * a fully negotiated session.  This way, we would still try to attempt
   * restoring the prior session even if the session was closed again before we
   * could attempt the restoration.  This fix would involve modifications in
   * almost all the protocols.
   */

  if (hdrscps_get_state() == HDRSCPS_INACTIVE)
  {
    HDRTRACE_CMD_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      scp_cmd_ptr->name, scp_cmd_ptr->sender, hdrscps_get_state() );

    return;
  }

  /* if current session is active, it should become the prior session,
     that is done automatically by hdrscmdb. Simply set session_status
     to INACTIVE without updating NV ITEM */

  hdrscp.session_status = HDRSCP_SESSION_INACTIVE;

  /* In case we had an outstanding config request... */
  hdrscp_reset_config_req_info();
  hdrscp.ps_gaup_failed = FALSE;

  hdrscp_reset_con_fail_retry_count();

  hdrscps_change_state(HDRSCPS_INACTIVE);
} /* hdrscp_process_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_ACTIVATE

DESCRIPTION
  This function processes the Activate command

DEPENDENCIES
  None.

PARAMETERS
  scp_cmd_ptr - pointer to the command to process

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state.

===========================================================================*/
LOCAL void hdrscp_process_activate
(
  hdrscp_cmd_type * scp_cmd_ptr
)
{
  if (hdrscps_get_state() != HDRSCPS_INACTIVE)
  {
    HDRTRACE_CMD_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
      scp_cmd_ptr->name, scp_cmd_ptr->sender, hdrscps_get_state() );

    return;
  }
  hdrscps_change_state(HDRSCPS_OPEN);

  hdrscp_config(HDRSCP_CR_AT_INITIATED);

} /* hdrscp_process_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRSCP.

DEPENDENCIES
  None

PARAMETERS
  msg - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscp_msg_cb
(
  dsm_item_type * msg_ptr
)
{
  hdrmc_queue_msg( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
    HDRHAI_IN_USE, msg_ptr );
} /* hdrscp_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRSCP.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr
)
{
  if (rex_self() == HDRSCP_TASK_PTR)
  {
    hdrscp_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, ind_name, ind_data_ptr );
  }
} /* hdrscp_ind_cb */

/* <EJECT> */ 

#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRSCP_READ_EHRPD_CREDENTIALS

DESCRIPTION
  The function reads the NV and checks whether AT is provisioned with eHRPD 
  credentials.

DEPENDENCIES

RETURN VALUE
  TRUE if the AT is provisioned with eHRPD credentials, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrscp_read_ehrpd_credentials( void )
{
  nv_ehrpd_imsi_type  ehrpd_imsi;
    /* eHRPD IMSI */
  nv_eap_shared_secret_type eap_shared_secret;
    /* EAP AKA Shared Secret key */
  nv_ehrpd_milenage_op_type milenage_op;
    /* Operator Variant */
  boolean imsi_mcc_is_valid = FALSE;
  boolean imsi_mnc_is_valid = FALSE;
  boolean imsi_msin_is_valid = FALSE;
  boolean imsi_is_valid = FALSE;
  boolean eap_ss_is_valid = FALSE;
  boolean eap_op_is_valid = FALSE;
  boolean ehrpd_credentials_are_provisioned = FALSE;
    /* Status flags */
  uint8 i = 0;
/*-------------------------------------------------------------------------*/

  /*****************************************************************
   Read the eHRPD credentials from NV 
   ****************************************************************/
  if( hdrutil_read_nv_item( NV_EHRPD_IMSI_I,
     (nv_item_type *) &ehrpd_imsi) != NV_DONE_S)
  {
    return FALSE;
  }

  if( hdrutil_read_nv_item( NV_PPP_EAP_SHARED_SECRET_I,
     (nv_item_type *) &eap_shared_secret) != NV_DONE_S)
  {
    return FALSE;
  }

  if( hdrutil_read_nv_item( NV_EHRPD_MILENAGE_OP_I,
     (nv_item_type *) &milenage_op) != NV_DONE_S)
  {
    return FALSE;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, 
               "eHRPD credentials NV items read successfully");

  /*****************************************************************
   Check whether eHRPD credentials are provisioned 
   1) IMSI/EAP APA key length and value should be non-zero
   2) OP key length is set to max
   *****************************************************************/
  for (i = 0; i < ehrpd_imsi.num_of_mcc_digits; i++)
  {
    if (ehrpd_imsi.ehrpd_mcc[i] != 0)
    {
      imsi_mcc_is_valid = TRUE;
      break;
    }
  }

  for (i = 0; i < ehrpd_imsi.num_of_mnc_digits; i++)
  {
    if (ehrpd_imsi.ehrpd_mnc[i] != 0)
    {
      imsi_mnc_is_valid = TRUE;
      break;
    }
  }

  for (i = 0; i < ehrpd_imsi.num_of_msin_digits; i++)
  {
    if (ehrpd_imsi.ehrpd_msin[i] != 0)
    {
      imsi_msin_is_valid = TRUE;
      break;
    }
  }

  for (i = 0; i < eap_shared_secret.eap_shared_secret_len; i++)
  {
    if (eap_shared_secret.eap_shared_secret[i] != 0)
    {
      eap_ss_is_valid = TRUE;
      break;
    }
  }

  if (milenage_op.ehrpd_milenage_op_length == 
      NV_EHRPD_MILENAGE_MAX_OP_LENGTH)
  {
    eap_op_is_valid = TRUE;
  }

  /* IMSI is valid when atleast MCC or MNC or MSIN is non zero */
  if (!imsi_mcc_is_valid && !imsi_mnc_is_valid && !imsi_msin_is_valid)
  {
    imsi_is_valid = FALSE;
  }
  else
  {
    imsi_is_valid = TRUE;
  }

  /*****************************************************************
   If all the values are true then the eHRPD credentials are 
   provisioned correctly.
   *****************************************************************/
  if (imsi_is_valid && eap_ss_is_valid && eap_op_is_valid)
  {
    ehrpd_credentials_are_provisioned = TRUE;

    HDR_MSG_PROT (MSG_LEGACY_HIGH, "eHRPD credentials are provisioned");
  }
  else
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "eHRPD credentials are not provisioned");
  }

  return ehrpd_credentials_are_provisioned;

} /* hdrscp_read_ehrpd_credentials */
#endif /* FEATURE_EHRPD */

/* <EJECT> */ 
#ifdef FEATURE_HDR_REVB_CUSTOM_CONFIG
/*===========================================================================

FUNCTION HDRSCP_READ_SCP_CUSTOM_CONFIG_NV

DESCRIPTION
  This function reads the NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I NV item and 
  initializes the permitted subtypes accordingly.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  E_SUCCESS - If this NV item is enabled and read successfully.
  E_FAILURE - If this NV item is disabled or read failed.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrscp_read_scp_custom_config_nv()
{
  nv_hdrscp_custom_config_type nvc;
    /* Custom subtype configuration read from NV */
  boolean ps_init_status = E_FAILURE;
    /* Whether permitted subtypes has been initialized or not */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the app/prot subtype custom config from NV. */
  if (hdrutil_read_nv_item ( NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I,
             (nv_item_type*)&nvc) == NV_DONE_S)
  {
    /* The NV item is packed and we don't want to use a packed data
     * type throughout the code.  Also, the NV item has uint32 fields
     * that we want to interpret as bit-masks.  We do the conversion here.
     */
    hdrscp.permitted_subtypes.config_is_custom =
      (boolean) nvc.custom_config_is_active;

    if (hdrscp.permitted_subtypes.config_is_custom)
    {
      /* We need to use memcpy because the compiler is rightfully concerned
       * about bit ordering.  This code is still portable because it only
       * depends that the NV item has the same byte ordering as the hdrscp
       * data types.
       */
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( &hdrscp.permitted_subtypes.prot_subtype_mask, 
                sizeof(hdrscp.permitted_subtypes.prot_subtype_mask),
               (uint8 *) &nvc.prot_subtype_mask, sizeof(uint32) );
      memscpy( &hdrscp.permitted_subtypes.bcmcs_subtype_mask, 
               sizeof(hdrscp.permitted_subtypes.bcmcs_subtype_mask), 
              (uint8 *) &nvc.bcmcs_subtype_mask, sizeof(uint32) );
      memscpy( &hdrscp.permitted_subtypes.app_subtype_mask, 
               sizeof(hdrscp.permitted_subtypes.app_subtype_mask), 
              (uint8 *) &nvc.app_subtype_mask, sizeof(uint32) );
#else
      memcpy( &hdrscp.permitted_subtypes.prot_subtype_mask,
              (uint8 *) &nvc.prot_subtype_mask, sizeof(uint32) );
      memcpy( &hdrscp.permitted_subtypes.bcmcs_subtype_mask,
              (uint8 *) &nvc.bcmcs_subtype_mask, sizeof(uint32) );
      memcpy( &hdrscp.permitted_subtypes.app_subtype_mask,
              (uint8 *) &nvc.app_subtype_mask, sizeof(uint32) );
#endif /* FEATURE_MEMCPY_REMOVAL */

      ps_init_status = E_SUCCESS;
    }
  }
  else
  {
       HDR_MSG_PROT ( MSG_LEGACY_ERROR, "Continue with default values for NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I ");	
  }

  return ps_init_status;
}
#endif /* FEATURE_HDR_REVB_CUSTOM_CONFIG */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_READ_SCP_FORCE_AT_CONFIG_NV
    
DESCRIPTION
  This function reads the NV_HDRSCP_FORCE_AT_CONFIG_I NV item and 
  initializes the permitted subtypes accordingly.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscp_read_scp_force_at_config_nv( void )
{
  boolean force_restricted_cf = FALSE;
    /* Boolean flag that controls session call flow for KDDI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the app/prot subtype config from NV. */
  if (hdrutil_read_nv_item(NV_HDRSCP_FORCE_AT_CONFIG_I,
           (nv_item_type*)&hdrscp.force_at_config) != NV_DONE_S) 
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "NV_HDRSCP_FORCE_AT_CONFIG_I read failed"); 

#ifdef FEATURE_HDR_DEFAULT_TO_REV_A
#ifdef FEATURE_EHRPD
    /* If the read fails we default to Rev A configuration with EMPA when 
       eHRPD feature is enabled */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Default to revA with EHRPD"); 
    hdrscp.force_at_config = HDRSCP_REVA_PROTOCOLS_WITH_EHRPD;
#else
    /* Otherwise default to Rev A configuration with MFPA */
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Default to revA with MFPA"); 
    hdrscp.force_at_config = HDRSCP_REVA_PROTOCOLS_WITH_MFPA;
#endif /* FEATURE_EHRPD */
#else
    /* If the read fails we default it to Rev B configuration */
    hdrscp.force_at_config = HDRSCP_REVB_PROTOCOLS_WITH_MMPA;
#endif /* FEATURE_HDR_DEFAULT_TO_REV_A */
  }

#ifndef FEATURE_HDR_NEGOTIATE_REVB
  if (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_MMPA)
  {
    /* If the AT doesn't suppot negotiating revB, Set default to MFPA, 
     * We do not set default to EMPA is because no customer deploy EMPA yet,
     * however, the AT still supports EMPA if FEATURE_HDR_EMPA is enabled.
     */
    hdrscp.force_at_config = HDRSCP_REVA_PROTOCOLS_WITH_MFPA;
  }

  if (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_EHRPD)
  {
    /* If the AT doesn't suppot negotiating revB,
       Set to HDRSCP_REVA_PROTOCOLS_WITH_EHRPD.
     */
    hdrscp.force_at_config = HDRSCP_REVA_PROTOCOLS_WITH_EHRPD;
  }

  if (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_EHRPD_AND_IRAT)
  {
    /* If the AT doesn't suppot negotiating revB,
       Set to HDRSCP_REVA_PROTOCOLS_WITH_EHRPD_AND_IRAT.
     */
    hdrscp.force_at_config = HDRSCP_REVA_PROTOCOLS_WITH_EHRPD_AND_IRAT;
  }

#endif /* FEATURE_HDR_NEGOTIATE_REVB */

  switch (hdrscp.force_at_config)
  {
    case HDRSCP_REVB_PROTOCOLS_WITH_EHRPD_AND_IRAT:
#ifdef FEATURE_LTE_TO_HDR_OH
#ifdef FEATURE_EHRPD
      hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap = TRUE;
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRSCP_REVB_PROTOCOLS_WITH_EHRPD:
#ifdef FEATURE_HDR_NEGOTIATE_REVB
#ifdef FEATURE_EHRPD
      hdrscp.permitted_subtypes.app_subtype_mask.enable_ammpa = TRUE;
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_HDR_NEGOTIATE_REVB */

    case HDRSCP_REVB_PROTOCOLS_WITH_MMPA:
#ifdef FEATURE_HDR_NEGOTIATE_REVB
#ifdef FEATURE_HDR_REVB
      /* Enable Rev A protocols, Rev B protocols, MMPA, EMPA and MFPA */
      hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.mc_fmac = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.mc_rmac = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.mc_rup = TRUE;
      hdrscp.permitted_subtypes.app_subtype_mask.enable_mmpa = TRUE;
#endif /* FEATURE_HDR_REVB */
#endif /* FEATURE_HDR_NEGOTIATE_REVB */

    case HDRSCP_REVA_PROTOCOLS_WITH_EHRPD_AND_IRAT:
#ifdef FEATURE_LTE_TO_HDR_OH
#ifdef FEATURE_EHRPD
      hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap = TRUE;
#endif /* FEATURE_EHRPD */
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRSCP_REVA_PROTOCOLS_WITH_EHRPD:
#ifdef FEATURE_EHRPD
      hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa = TRUE;
#endif /* FEATURE_EHRPD */

    case HDRSCP_REVA_PROTOCOLS_WITH_MFPA_AND_EMPA:
#ifdef FEATURE_HDR_EMPA
      /* Enable Rev A protocols EMPA and MFPA */
      hdrscp.permitted_subtypes.app_subtype_mask.enable_empa = TRUE;
#endif /* FEATURE_HDR_EMPA */

    case HDRSCP_REVA_PROTOCOLS_WITH_MFPA:
      /* Enable Rev A protocols and MFPA */
      hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_cmac = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_amac = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_fmac = TRUE;
#ifdef FEATURE_HDR_QUICK_IDLE
      if (hdrmc_feature_is_enabled(HDRMC_FEATURE_QUICK_IDLE))
      {
        hdrscp.permitted_subtypes.prot_subtype_mask.quick_idle = TRUE;
      }
#endif /* FEATURE_HDR_QUICK_IDLE */
      hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub3= TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.rmac_sub1= TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_idle = TRUE;
      hdrscp.permitted_subtypes.prot_subtype_mask.gmmcdp = TRUE;
      hdrscp.permitted_subtypes.app_subtype_mask.enable_mfpa = TRUE;
      break;

    case HDRSCP_REV0_PROTOCOLS_ONLY:
      /* All Rev 0 protocols are enabled by default */
      break;

    default:
      HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                   "Invalid entry in NV_HDRSCP_FORCE_AT_CONFIG_I NV item");
      break;
  }

#ifdef FEATURE_HDR_CSNA
  /* Enable CSNA if the feature is defined, there is no NV flag. */
  hdrscp.permitted_subtypes.app_subtype_mask.enable_csna = TRUE;
#endif

  /* The above switch-case statement is based on the assumption that each "case"
     below is subset of the above "case" however when HDRSCP_REVB_PROTOCOLS_WITH_MMPA
     is chosen EHRPD is not enabled so we explictly reset Alt EMPA when 
     HDRSCP_REVB_PROTOCOLS_WITH_MMPA is chosen */
  if (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_MMPA)
  {
    hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa = FALSE;
  }

  if (hdrscp.force_at_config == HDRSCP_REVA_PROTOCOLS_WITH_MFPA ||
      hdrscp.force_at_config == HDRSCP_REVA_PROTOCOLS_WITH_MFPA_AND_EMPA ||
      hdrscp.force_at_config == HDRSCP_REVA_PROTOCOLS_WITH_EHRPD)
  {
    /* Restricted session call flow is active only when RevA is 
       activated */
    if (hdrutil_read_nv_item(NV_HDRSCP_FORCE_RESTRICTED_CF_I,
             (nv_item_type*)&force_restricted_cf) != NV_DONE_S)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "SCP force restricted CF data not active in NV");
    }

    if (force_restricted_cf)
    {
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_cmac = FALSE;
      hdrscp.permitted_subtypes.prot_subtype_mask.enh_amac = FALSE;
      hdrscp.permitted_subtypes.prot_subtype_mask.gmmcdp = FALSE;
    }
  }

  if ( hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_MMPA ||
       hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_EHRPD )
  {
    hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap = FALSE;
  }

}


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRSCP_IS_IRAT_ENABLED

DESCRIPTION
  This function tells whether IRAT subtypes are enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - IRAT subtypes are enabled.
  FALSE - IRAT subtypes are disabled.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrscp_is_irat_enabled( void )
{
  return hdrscp.permitted_subtypes.prot_subtype_mask.irat_sap;

} /* hdrscp_is_irat_enabled() */
#endif /* FEATURE_LTE_TO_HDR_OH */

    
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_POWERUP_INIT

DESCRIPTION
  This function commands SCP to Initialize.  This function is called
    instead of hdrscp_init at powerup.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - session is open
  FALSE - session is not open

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrscp_powerup_init ( void )
{
  boolean session_status = FALSE;
    /* Return value */
#ifdef FEATURE_EHRPD
  boolean ehrpd_auth_in_usim = FALSE;
    /* Whether authentication to be done on USIM or S/W */
#ifdef T_QSC6695
  DalDeviceHandle *phChipInfo;
  DalChipInfoModemType pnModem;
#endif /* T_QSC6695 */
#endif /* FEATURE_EHRPD */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*************************************************************************
   *                 Initialize module variables                           *
   *************************************************************************/
  if ( hdrmc_is_first_init() )
  {
    rex_def_timer( &hdrscp_an_init_state_timer,
                   HDRSCP_TASK_PTR,
                   HDRMC_SCP_AN_INIT_STATE_TIMER_SIG );

    rex_def_timer( &hdrscp_config_timer,
                   HDRSCP_TASK_PTR,
                   HDRMC_SCP_CONFIG_RSP_TIMER_SIG );
  }

  hdrscp.config_complete.trans_id = 0;
  hdrscp.config_complete.awaiting_slp_callback = FALSE;
  hdrscp.ps_gaup_failed = FALSE;
  hdrscp.session_cfg_token_changed = FALSE;
  hdrscp.config_req_info.transaction_id = 0;
  hdrscp.config_req_info.value_id = 0;
  hdrscp.config_requested = FALSE;
#ifdef FEATURE_EHRPD
  hdrscp.ehrpd_no_check = FALSE;
#endif
  hdrscp_reset_config_req_info ();
  memset(&hdrscp.permitted_subtypes, 0,
         sizeof(hdrscp_permitted_subtypes_type));

  /*************************************************************************
   *                Initialize permitted subtypes                          *
   *************************************************************************/
#ifdef FEATURE_HDR_REVB_CUSTOM_CONFIG
  if (hdrscp_read_scp_custom_config_nv() != E_SUCCESS)
  /* The NV_HDRSCP_SUBTYPE_CUSTOM_CONFIG_I NV item is defined only
     for internal builds. This NV item allows more flexibility to 
     choose a set of permitted subtypes. When this NV is enabled it
     overrides NV_HDRSCP_FORCE_AT_CONFIG_I */
#endif /* FEATURE_HDR_REVB_CUSTOM_CONFIG */
  {
    hdrscp_read_scp_force_at_config_nv();

#ifdef FEATURE_HDR_BCMCS

    if (hdrutil_read_nv_item ( 
          NV_HDRSCP_SELECT_BCMCS_CONFIG_I,
          (nv_item_type*) &hdrscp.bcmcs_config_from_nv ) != NV_DONE_S)
    {
      hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled = 
        FALSE;
    }
    else
    {
      hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled = 
        ( (hdrscp.bcmcs_config_from_nv == NV_HDRSCP_ENABLE_GOLD_BCMCS) ||
          (hdrscp.bcmcs_config_from_nv == 
            NV_HDRSCP_ENABLE_GOLD_AND_PLATINUM_BCMCS) );
    }
#else
    /* Always disable BCMCS for now */
    hdrscp.permitted_subtypes.bcmcs_subtype_mask.gen_broadcast_is_enabled = FALSE;
#endif /* FEATURE_HDR_BCMCS */

  }

#ifdef FEATURE_EHRPD
  if( hdrutil_read_nv_item( NV_EHRPD_AUTH_IN_USIM_I,
     (nv_item_type *) &ehrpd_auth_in_usim) != NV_DONE_S)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,
      "EHRPD_AUTH_IN_USIM NV read failed assume that eHRPD credentials are in NV");
  }

  if (ehrpd_auth_in_usim == TRUE)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,
      "EHRPD_AUTH_IN_USIM NV returned TRUE wait for USIMApp activation to propose eHRPD");

    /* Keep a global copy of whether eHRPD auth is in card*/
    hdrscp.ehrpd_auth_in_usim = ehrpd_auth_in_usim;
  
    if(hdrmc_mcfg_refresh_in_progress() != TRUE)
    {
    /* Wait for USIM App activation to set whether credentials are provisioned */
    hdrscp.ehrpd_credentials_are_provisioned = FALSE;
  }
  else
  {
       HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
         "MCFG refresh is in progress. Do not updated ehrpd credentials. current value %d",
         hdrscp.ehrpd_credentials_are_provisioned);
    }
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,
      "EHRPD_AUTH_IN_USIM NV returned FALSE validate eHRPD credentials in NV");

    hdrscp.ehrpd_auth_in_usim = FALSE;

    hdrscp.ehrpd_credentials_are_provisioned = 
        hdrscp_read_ehrpd_credentials();
  }

#ifdef T_QSC6695
  if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "Device capability read failed");
  }
  else 
  {
    DalChipInfo_GetModemSupport(phChipInfo,  &pnModem);

    if( !(pnModem & DALCHIPINFO_MODEM_EHRPD) )
    {
      hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa = FALSE;
      hdrscp.ehrpd_credentials_are_provisioned = FALSE;
    }

    DAL_DeviceDetach(phChipInfo);
  }
#endif /* T_QSC6695 */
  
  /* By default, network setting of eHRPD is True. */
  hdrscp.ehrpd_network_setting = TRUE;

  /* Read eHRPD network setting from EFS NV. */
  if ( (int)efs_get( HDRSCP_EHRPD_NETWORK_SETTING_FILENAME,
                     &hdrscp.ehrpd_network_setting, 
                     1 ) == 1 )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "Read eHRPD network setting value: %d successfully", 
                    hdrscp.ehrpd_network_setting );
  }
  else 
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "Reading eHRPD network setting failed. Use default value: %d", 
                    hdrscp.ehrpd_network_setting );
  }

  /* Whenever AT powers up, Data Service will notify HDR the upper layer's 
   * eHRPD capablity. So the initialization here is actually meaningless. */
  hdrscp.data_ehrpd_capability = TRUE;
#endif /* FEATURE_EHRPD */

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrscp.awaiting_prereg_allowed = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH*/

  /*************************************************************************
   *                Read session status                              *
   *************************************************************************/
  hdrscp.session_status = HDRSCP_SESSION_ACTIVE;

  if (hdrutil_read_nv_item ( NV_HDRSCP_SESSION_STATUS_I,
               (nv_item_type*)&hdrscp.session_status) != NV_DONE_S)
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "SCP session status data not active in NV" );
    hdrscp.session_status = HDRSCP_SESSION_INACTIVE;
  }
  if (hdrmc_feature_is_enabled(
        HDRMC_FEATURE_HDR_SESSION_TIED_TO_PPP) &&
      hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA)) 
  {
    /*************************************************************************
     * Always bring up a new session if the SN PPP was up before power down  *
     *************************************************************************/
    if ( hdrpac_get_sn_ppp_status_from_nv() ) 
    {
      if (hdrscp.session_status == 
          (hdrscp_session_status_enum_type) NV_SCP_SESSION_INACTIVE)
      {
        /* The SN PPP session should not be up before powerup while the 
           HDR session is inactive */
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "SN PPP status was up and HDR session not active" );
      }
      else
      {     
#ifndef FEATURE_DEDICATED_C2K_MODE
        /* Write the current session status to inactive, the AT will open a 
           new session */
        hdrscp.session_status = HDRSCP_SESSION_INACTIVE;
        hdrutil_write_nv_item( NV_HDRSCP_SESSION_STATUS_I,
                              (nv_item_type*)&hdrscp.session_status);
#endif
      }
  
      /* Reset the SN PPP status */
      hdrpacdsif_clear_sn_ppp_status();
    }
  }
  /***************************************************************************
   * If the session is active and the EFS file is valid SCMDB is initialized *
   * to stored session,  otherwise it is intialized to defaults.             *
   **************************************************************************/
  session_status = hdrscmdb_powerup_init();

  /* Get current configuration parameters from SCMDB. */
  hdrscmdb_get_scp_session_info(&hdrscp.session_params.current);

  if (session_status)
  {
    hdrscps_change_state(HDRSCPS_OPEN);
  }
  else
  {
    hdrscp.session_status = HDRSCP_SESSION_INACTIVE;
  }

  return session_status;

} /* hdrscp_powerup_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_COMPLETE_TX_OUTCOME_CB

DESCRIPTION
  This function tells SCP that the given config complete msg was sent.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - Pointer to the dsm_item containing the message that was sent
  err_val - Used to match signature of slp callback function.  Ignored.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrscp_complete_tx_outcome_cb
(
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  /* Allocated out of the heap. It is possible for this indication to be
     queued for a higher level task, so the data must be made available
     to all here. */
  static hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!hdrscp.config_complete.awaiting_slp_callback)
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Ignore config complete tx outcome callback");
    return;
  }

  switch (err_val)
  {
    case E_SUCCESS:
      HDRTRACE_TXMSG_ACKED(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_CONFIG_COMPLETE_MSG, hdrscps_get_state());

      ASSERT( hdrscp.negotiation_state != HDRSCP_NEG_NONE );

      /* we wait until we get the ACK for sending the Config Complete
         in case the channel is lost, in which case we will have to resend
         the config complete message. */
      hdrscps_change_state(HDRSCPS_AN_INIT);
      break;

    case E_FAILURE: /* given when lose acquisition */
      HDRTRACE_TXMSG_FAILED(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_CONFIG_COMPLETE_MSG, hdrscps_get_state());

      hdrscp_fail_data.reason = HDRHAI_CLOSE_CFG_MSG_FAIL;
      hdrscp_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
      hdrscp_fail_data.subtype = HDRSCP_DEFAULT_SCP;
      hdrscp_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;

      if (hdrscp.config_req_info.sent_previous_session_attrib)
      {
        /* It must have been the prior session attribute*/
        hdrscp_fail_data.attribute_id = HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID;
      }
      else 
      {
        hdrscp_fail_data.attribute_id = 
          hdrscp.config_req_info.ptype_attrib[0].type;
      }

      ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
      hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
      break;

    case E_AGAIN:   /* given when lose connection */
      /* ignore, message will be resent in response to other indications */
    case E_CANCELED:
    case E_NOT_ALLOWED:
      break;

    default:
      ERR( "Unexpected err val for Config outcome %d", err_val, 0, 0);
      break;
  }
  
    /* This flag is set to TRUE when the message is sent, and because it is
   * guaranteed that this callback is called regardless of the outcome of
   * the message, the flag will not get out of sync. 
   * Set the flag after AT enters AN_INIT state. 
   */
  hdrscp.config_complete.awaiting_slp_callback = FALSE;

} /* hdrscp_complete_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_CONFIG_TX_OUTCOME_CB

DESCRIPTION
  This function tells SCP that the given config request msg was sent.

DEPENDENCIES
  If hdrhmp_abort_msg() has been called previously to abort this message,
  the flag hdrstream.config_req_info.awaiting_slp_ack should be set to
  FALSE and any "message outcome" related processing done at that point.
  This callback only handles the case where hdrhmp_abort_msg() has not
  been previously called.

PARAMETERS
  data_ptr - Pointer to the dsm_item containing the message that was sent
  err_val - Used to match signature of slp callback function.  Ignored.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrscp_config_tx_outcome_cb
(
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  /* data for SCP.Failed indication */
  hdrind_ind_data_union_type ind_data;
  /* Allocated out of the heap. It is possible for this indication to be
     queued for a higher level task, so the data must be made available
     to all here. */
  static hdrscp_scp_fail_ind_type hdrscp_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (uint16) ((dword) data_ptr) == HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,"Rcvd SLPAck for PriorSession ConfigReq msg");

    if (hdrscp.config_req_info.awaiting_slp_ack & 
        HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB)
    {
      /* This flag is set to TRUE when the message is sent, and because it is
       * guaranteed that this callback is called regardless of the outcome of
       * the message, the flag will not get out of sync.
       */
      hdrscp.config_req_info.awaiting_slp_ack &= 
                             ~HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB;
    
      /* If success mark that we are waiting for L3 ack */
      if (err_val == E_SUCCESS)
      {
        hdrscp.config_req_info.awaiting_l3_ack |= 
                               HDRSCP_CFGREQ_FOR_PRIORSESSION_ATTRIB;
      }
    }
    else
    {
      /* If callback gets called but the flag is cleared then just return, we
         would have received the ConfigRsp before the SLP Ack and hence aborted
         waiting for SLP ack */
      return;  
    }    
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,
       "Rcvd SLPAck for rest of SCP's config attributes ConfigReq msg");

    if (hdrscp.config_req_info.awaiting_slp_ack & 
        HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB)
    {
      hdrscp.config_req_info.awaiting_slp_ack &= 
                             ~HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB; 
 
      /* If success mark that we are waiting for L3 ack */
      if (err_val == E_SUCCESS)
      {
        hdrscp.config_req_info.awaiting_l3_ack |= 
                               HDRSCP_CFGREQ_FOR_RESTOFTHESCP_ATTRIB;
      }
    }
    else
    {
      return;
    }
  }

  /* If we haven't received the SLP Ack for both the messages then do not
     start the timer, but if the SLP Ack failed then fail the negotiation 
     immediately need not wait for the status of the other message */
  if (hdrscp.config_req_info.awaiting_slp_ack && (err_val == E_SUCCESS))
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
         "Waiting for SLPack for both the CfgReq messages to start the timer");
    return;
  }

  switch (err_val)
  {
    case E_SUCCESS:
      HDRTRACE_TXMSG_ACKED(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_CONFIG_REQ_MSG, hdrscps_get_state());
      if (hdrscps_get_state() == HDRSCPS_AT_INIT)
      {
        (void) rex_set_timer( &hdrscp_config_timer,
                              HDRHAI_CONFIG_RSP_TIMER_MSEC );
      }
      else
      {
        ERR("config_tx_outcome_cb in state %d", hdrscps_get_state(),0,0);
      }
      break;

    case E_FAILURE: /* given when lose acquisition */
      HDRTRACE_TXMSG_FAILED(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
        HDRSCP_CONFIG_REQ_MSG, hdrscps_get_state());
      hdrscp_fail_data.reason = HDRHAI_CLOSE_CFG_MSG_FAIL;
      hdrscp_fail_data.type = HDRHAI_SESSION_CONFIGURATION_PROTOCOL;
      hdrscp_fail_data.subtype = HDRSCP_DEFAULT_SCP;
      hdrscp_fail_data.attribute_id_length = HDRSCP_ATTRIBUTE_ID_LENGTH;

      if ((uint16) ((dword) data_ptr) == HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID)
      {
        /* It must have been the prior session attribute*/
        hdrscp_fail_data.attribute_id = HDRSCP_PRIOR_SESSION_ATTRIBUTE_ID;
      }
      else 
      {
        hdrscp_fail_data.attribute_id = 
          hdrscp.config_req_info.ptype_attrib[0].type;
      }

      ind_data.scp_fail_ptr = (void *)&hdrscp_fail_data;
      hdrind_give_ind(HDRIND_SCP_FAILED, (void*)&ind_data);
      break;

    case E_AGAIN:   /* given when lose connection */
    case E_CANCELED:
    case E_NOT_ALLOWED:
      /* ignore, message will be resent in response to other indications */
      hdrscp_reset_config_req_info();
      break;

    default:
      hdrscp_reset_config_req_info();
      ERR( "Unknown err val for Config outcome %d",
           err_val, 0, 0);
  }

} /* hdrscp_config_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_ACTIVATE

DESCRIPTION
  This function commands SCP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscp_activate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrscp_cmd_type hdrscp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRSCP_ACTIVATE_CMD received %d",
                  hdrscps_get_state());

  /* Fill in all members of the hdrscp_cmd_type for the Activate command */
  hdrscp_cmd.name = HDRSCP_ACTIVATE_CMD;
  hdrscp_cmd.sender = sender;

  if (rex_self() == HDRSCP_TASK_PTR)
  {
    hdrscp_process_cmd( &hdrscp_cmd );
  }
  else
  {
    ERR("Activate Command not expected from other task",0,0,0);
    /* This function copies the hdrscp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, &hdrscp_cmd,
                     sizeof (hdrscp_cmd) );
  }
} /* hdrscp_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_DEACTIVATE

DESCRIPTION
  This function commands HDRSCP to deactivate

DEPENDENCIES
  None

PARAMETERS
  deactivate_reason - the reason SCP is being deacitvated
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrscp_deactivate
(
  hdrhai_session_close_reason_enum_type deactivate_reason,
  hdrhai_protocol_name_enum_type sender
)
{
  hdrscp_cmd_type hdrscp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRSCP_DEACTIVATE_CMD received %d",
                  hdrscps_get_state());

  /* Fill in all members of the hdrscp_cmd_type for the Activate command */
  hdrscp_cmd.name = HDRSCP_DEACTIVATE_CMD;
  hdrscp_cmd.sender = sender;
  hdrscp_cmd.params.deactivate.reason = deactivate_reason;

  if (rex_self() == HDRSCP_TASK_PTR)
  {
    hdrscp_process_cmd( &hdrscp_cmd );
  }
  else
  {
    ERR("Deactivate Command not expected from other task",0,0,0);
    /* This function copies the hdrscp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, &hdrscp_cmd,
                     sizeof (hdrscp_cmd) );
  }
} /* hdrscp_deactivate */

/* <EJECT> */ 
#ifdef FEATURE_EHRPD
/*===========================================================================

FUNCTION HDRSCP_PROCESS_UPDATE_EHRPD_CREDENTIALS

DESCRIPTION
  This function process update eHRPD credentials command.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdrscp_process_update_ehrpd_credentials
(
  hdrscp_cmd_type * scp_cmd_ptr
)
{

  if (hdrscp_ehrpd_auth_is_in_usim())
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, 
      "eHRPD credentials are in card do not process update credentials cmd");
    return;
  }

  /* Read and store eHRPD credentials */
  hdrscp.ehrpd_credentials_are_provisioned = 
      hdrscp_read_ehrpd_credentials();

  if ( hdrscp.ehrpd_credentials_are_provisioned == TRUE )
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "eHRPD credentials are provisioned");
  }
  else
  {
    HDR_MSG_PROT (MSG_LEGACY_HIGH, "eHRPD credentials are not provisioned");
  }

  /* Do nothing here.
   * After Phone Status Change, HDR receives deactivation and activation
   * in sequence. eHRPD availability will be checked when HDR is activated 
   * and acquires the network. */

} /* hdrscp_process_update_ehrpd_credentials */

#endif /* FEATURE_EHRPD */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SCP.

DEPENDENCIES
  None

PARAMETERS
  scp_cmd_ptr - Pointer to the command for SCP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_process_cmd
(
  hdrscp_cmd_type * scp_cmd_ptr
)
{
  HDRTRACE_CMD( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, scp_cmd_ptr->name,
    scp_cmd_ptr->sender, hdrscps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( scp_cmd_ptr->name )
  {
    case HDRSCP_ACTIVATE_CMD:        /* Begin Opening a Session*/
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Activate Command");
      hdrscp_process_activate(scp_cmd_ptr);
      break;

    case HDRSCP_DEACTIVATE_CMD:      /* Deactivate the protocol */
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process Deactivate Command");
      hdrscp_process_deactivate(scp_cmd_ptr);
      break;

#ifdef FEATURE_EHRPD
    case HDRSCP_UPDATE_EHRPD_CREDENTIALS_CMD:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Process update eHRPD credentials command");
      hdrscp_process_update_ehrpd_credentials(scp_cmd_ptr);
      break;
#endif /* FEATURE_EHRPD */

    default:
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrscp_process_cmd */

/*===========================================================================

FUNCTION HDRSCP_PROCESS_GAUP_RSP

DESCRIPTION
  This function processes the GAUP Accept/Reject Indication sent by SCM

DEPENDENCIES
  None.

PARAMETERS
   ind_name - ind_name, either GAUP_ACCEPT or GAUP_REJECT

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_gaup_rsp
(
  hdrind_ind_name_enum_type ind_name
)
{
  boolean commited = FALSE;
  hdrscps_state_enum_type scps_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  scps_state = hdrscps_get_state();
  if ((scps_state != HDRSCPS_PS_GAUP))
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
       ind_name, scps_state); //lint !e506
    return;
  }

  if ((ind_name == HDRIND_SCM_SCP_AT_GAUP_ACCEPTED) && 
      hdrscp_restore_prior_session(HDRSCP_PS_RESTORE_GAUP))
  {
    commited = TRUE;
  }
  else if (ind_name == HDRIND_SCM_SCP_AT_GAUP_REJECTED)
  {
    /* if AT rejects PS_GAUP, chances are it will not be able to restore
       prior session, invalidate prior session data on AT */
    hdrscmdb_set_prior_session_validity(FALSE);
    hdrscp.ps_gaup_failed = TRUE;
  }
  else
  {
    /* GAUP failed */
    hdrscp.ps_gaup_failed = TRUE;
  }

  /* always switch to OPEN state whether GAUP failed or not */
  hdrscps_change_state(HDRSCPS_OPEN);
  if (commited)
  {
    if (hdridlec_pccc_attrib_config_is_needed())
    {
      /* GAUP restore successful, see if PCCC config needed */
      hdrscp_config(HDRSCP_CR_AT_INITIATED);    
    }
  }
  else 
  {
    /* PS GAUP failed, switch back to OPEN state and start
       negotiation process */
    hdrscp_config(HDRSCP_CR_AT_INITIATED);
  }
  
} /* hdrscp_process_gaup_rsp */


/*===========================================================================

FUNCTION HDRSCP_PROCESS_IDLE_CONN_FAILED_IND

DESCRIPTION
  This function processes the Connection Failed Indication sent by IDLE

DEPENDENCIES
  None.

PARAMETERS
   ind_name - ind_name, either GAUP_ACCEPT or GAUP_REJECT

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_idle_conn_failed_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  boolean retry_config = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscps_get_state() == HDRSCPS_AT_INIT)
  {
    /* if the connection setup failed because of a denial from the AN,
       flag that we have received at least one connection denial in
       this round of connection setup trials.  This flag will be reset
       when the connection failure retry count is reset */
    if (ind_data_ptr->conn_fail_info.failure_reason & HDRIDLE_CONN_FAIL_CONN_DENY_MASK)
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "Con Denied in ATInit, resent config msgs");
      hdrscp.config_conn_setup.rxed_con_deny = TRUE;
      retry_config = TRUE;
    }
#ifdef FEATURE_LTE_TO_HDR_OH
    else if ( ind_data_ptr->conn_fail_info.failure_reason & 
               HDRIDLE_CONN_FAIL_SAP_L3_FAILED_MASK )
    {
      /* ind from SAP will trigger retry*/
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "SAP L3 FAILED, retry immediately ...");
      retry_config = TRUE;
    }
    else if ( ind_data_ptr->conn_fail_info.failure_reason & 
                HDRIDLE_CONN_FAIL_SAP_L2_FAILED_MASK )
    {
      /* backoff_timer_expired ind from SAP will trigger retry*/
      HDR_MSG_PROT( MSG_LEGACY_HIGH,
                    "SAP L2 FAILED, waiting for backoff timer expires ..." );
      retry_config = FALSE;
    }
#endif /* FEATURE_LTE_TO_HDR_OH */
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_HIGH,
                    "Con Fail in ATInit, resent config msgs");
      retry_config = TRUE;
    }

    /* stop the config request timer */
    (void)rex_clr_timer(&hdrscp_config_timer);
    (void)rex_clr_sigs(HDRSCP_TASK_PTR, HDRMC_SCP_CONFIG_RSP_TIMER_SIG);
    hdrscp_reset_config_req_info();

    if (retry_config)
    {
      hdrscp_config(HDRSCP_CR_RETRY);
    }
  }
  else
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
              HDRIND_IDLE_CONNECTION_FAILED, hdrscps_get_state());
  }
} /* hdrscp_process_idle_conn_failed_ind */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRSCP_PROCESS_BACKOFF_TIMER_EXPIRED_IND

DESCRIPTION
  This function processes the backoff timer expired Indication sent by SAP

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_backoff_timer_expired_ind( void )
{
  if ( hdrscps_get_state() == HDRSCPS_AT_INIT )
  {
    /* keep retry until receiving backoff_timer_stop_ind */
    HDR_MSG_PROT( MSG_LEGACY_MED, "SCP: backoff timer expired" );

    /* In tunnel mode, we do not count L2 failure as real "connection setup
     * failure" because our backoff timer is shorter than MAP avoidance time in
     * native HDR Mode.
     * For above reason, a new close_reason, CR_RETRY_BACKOFF, is used so 
     * hdrscp_config() function will not increase con_fail_retry_count */
    hdrscp_config( HDRSCP_CR_RETRY_BACKOFF );
  }
  else
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                                HDRIND_SAP_BACKOFF_TIMER_EXPIRED, 
                                hdrscps_get_state() );
  }

} /* hdrscp_process_backoff_timer_expired_ind */

/*===========================================================================

FUNCTION HDRSCP_PROCESS_BACKOFF_TIMER_STOPPED_IND

DESCRIPTION
  This function processes the backoff timer stopped Indication sent by SAP

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscp_process_backoff_timer_stopped_ind( void )
{
  if (hdrscps_get_state() == HDRSCPS_AT_INIT)
  {
    /* keep retry until receiving backoff_timer_stop_ind */
    HDR_MSG_PROT( MSG_LEGACY_MED, "PreReg is not allowed" );

    /* Wait for PreRegUpdate ind from ovhd and try to negotiate session
     * again if PreRegAllowed is set to 1*/
    hdrscp.awaiting_prereg_allowed = TRUE;
  }
  else
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
              HDRIND_SAP_BACKOFF_TIMER_STOPPED, hdrscps_get_state());
  }

} /* hdrscp_process_backoff_timer_expired_ind */
#endif  /* FEATURE_LTE_TO_HDR_OH*/

/*===========================================================================

FUNCTION HDRSCP_SILENT_RESTORE_PREV_SESSION

DESCRIPTION
  This function silently restore prior session

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_silent_restore_prior_session( void )
{  
  /* SCP could be in any state when AT reacquired prev network, 
     deactivate SCP first before restoring session */
  hdrscp_deactivate(HDRHAI_CLOSE_NEW_NETWORK, 
      HDRHAI_SESSION_CONFIGURATION_PROTOCOL);

  if (hdrscp_restore_prior_session(HDRSCP_PS_RESTORE_SILENT))
  {
#ifdef FEATURE_EHRPD
    if ( hdrscp_check_ok_to_open_state() )
#endif /* FEATURE_EHRPD */
    {
      hdrscps_change_state(HDRSCPS_OPEN);
    }
  }
} /* hdrscp_silent_restore_prior_session */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SCP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SCP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
)
{
  HDRTRACE_IND( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, ind_name,
    hdrscps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_ALMP_CONNECTION_CLOSED:
      HDR_MSG_PROT (MSG_LEGACY_LOW,
                    "ALMP Connection Closed Indication received");
      /* Always set configlock to unlocked when connection gets closed.
         This prevents AT from accidentally being locked forever */
      hdrscp_update_config_lock( FALSE );
      hdrscp_process_almp_connection_closed();
      break;

    case HDRIND_IDLE_CONFIG_COMPLETE:
      hdrscp_process_config_complete(ind_name,HDRHAI_IDLE_STATE_PROTOCOL);
      break;

    case HDRIND_STREAM_CONFIG_COMPLETE:
      hdrscp_process_config_complete(ind_name,HDRHAI_STREAM_PROTOCOL);
      break;

    case HDRIND_SCM_CONFIG_COMPLETE:
      hdrscp_process_config_complete(ind_name,
                      (hdrhai_protocol_name_enum_type)ind_data_ptr->prot_name);
      break;

    case HDRIND_GMMCDP_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name,
        HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL);
      break;

    case HDRIND_RUP_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name,
        HDRHAI_ROUTE_UPDATE_PROTOCOL);
      break;

    case HDRIND_RMAC_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name, 
        HDRHAI_RTC_MAC_PROTOCOL);
      break;

    case HDRIND_AMAC_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name,
        HDRHAI_AC_MAC_PROTOCOL);
      break;

    case HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name,
        HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL);
      break;

    case HDRIND_FMAC_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name, 
        HDRHAI_FTC_MAC_PROTOCOL);
      break;

    case HDRIND_PHY_CONFIG_COMPLETE:
      hdrscp_process_config_complete(
        ind_name, HDRHAI_PHYSICAL_LAYER_PROTOCOL);
      break;

    case HDRIND_IDLE_CONFIG_REQ:
      hdrscp_process_config_req(ind_name,HDRHAI_IDLE_STATE_PROTOCOL);
      break;

    case HDRIND_OVHD_INFO_UPDATED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "OVHD Info Updated Indication received");
      hdrscp_process_ovhd_info_updated(ind_data_ptr);
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      hdrscp_process_session_changed( ind_data_ptr );
      break;

    case HDRIND_SCM_SCP_AT_GAUP_FAILED:
    case HDRIND_SCM_SCP_AT_GAUP_ACCEPTED:
    case HDRIND_SCM_SCP_AT_GAUP_REJECTED:
      hdrscp_process_gaup_rsp(ind_name);
      break;

    case HDRIND_IDLE_CONNECTION_FAILED:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "IDLE Connection Failed received");
      hdrscp_process_idle_conn_failed_ind(ind_data_ptr);
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_BACKOFF_TIMER_EXPIRED:
      hdrscp_process_backoff_timer_expired_ind();
      break;

    case HDRIND_SAP_BACKOFF_TIMER_STOPPED:
      hdrscp_process_backoff_timer_stopped_ind();
      break;

    case HDRIND_OVHD_PREREG_INFO_UPDATED:
      hdrscp_process_ovhd_prereg_info_updated( ind_data_ptr );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIND_IDLE_CONNECTION_OPENED:
      hdrscp_reset_con_fail_retry_count();
      break;

    default:
      ERR ("Unrecognized Indication %d", ind_name, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrscp_process_ind */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SCP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */
  boolean free_item = TRUE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));

  /* MessageID is the first uint8 */
  if (HDRSCP_UNPACK_MSG_ID(item_ptr, &(msg_id)) != E_SUCCESS)
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages.
   * Only SCP and Key Exchange currently handle the config request/response
   * in their in use instance.
   */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet(&item_ptr);
    return;
  }

  HDRTRACE_MSG( HDRHAI_SESSION_CONFIGURATION_PROTOCOL, msg_id,
    hdrscps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( msg_id )
  {
    case HDRSCP_CONFIG_COMPLETE_MSG:
    case HDRSCP_SOFT_CONFIG_COMPLETE_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Complete Msg received");
      hdrscp_process_config_complete_msg(item_ptr, msg_id);
      break;

    case HDRSCP_CONFIG_START_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Start Msg received");
      hdrscp_process_config_start_msg( item_ptr );
      break;

    case HDRSCP_CONFIG_REQ_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Request Msg received");
      hdrscp_process_config_req_msg(item_ptr, HDRSCP_CONFIG_REQ );
      break;

    case HDRSCP_ATTRIB_UPDATE_REQ_MSG:
      HDR_MSG_PROT( MSG_LEGACY_LOW, "Attrib Update Req Msg received" );
      hdrscp_process_config_req_msg( item_ptr, HDRSCP_GAUP_REQ );
      break;

    case HDRSCP_CONFIG_RSP_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Config Response Msg received");
      hdrscp_process_config_rsp_msg(item_ptr);
      break;

    case HDRSCP_ATTRIB_UPDATE_ACCEPT_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "GAUP_Accept received, route to SCM");
      hdrscm_process_msg(item_ptr, HDRHAI_IN_USE);
      free_item = FALSE;
      break;

    case HDRSCP_ATTRIB_UPDATE_REJECT_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "GAUP_Reject Msg received, route to SCM");
      hdrscm_process_msg(item_ptr, HDRHAI_IN_USE);
      free_item = FALSE;
      break;

    case HDRSCP_LOCK_CONFIG_MSG:
    case HDRSCP_UNLOCK_CONFIG_MSG:
      hdrscp_process_config_lock_msg(item_ptr, msg_id);
      break;

    case HDRSCP_DELETE_PERSONALITY_MSG:
      hdrscp_process_delete_personality_msg(item_ptr);
      break;

    default:
      ERR ("HDRSCP unknown message %d", msg_id, 0, 0);
      break;
  }
  if (free_item)
  {
  dsm_free_packet( &item_ptr );
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrscp_process_msg */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSCP_PROCESS_SIG

DESCRIPTION
  This function processes signals for SCP.

DEPENDENCIES
  Only one signal is sent at a time, so only one bit of the sig input
  parameter is set.  If multiple signals need to be delivered, they must
  be delivered by multiple calls.

PARAMETERS
  sig - The signal that belongs to this protocol and was set.  This is not
        a signal mask.  Only one bit of the value is set

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscp_process_sig
(
  rex_sigs_type sig
)
{
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  /*---------------------------------------------------------------------------
  Determine which signal has been received.
  ---------------------------------------------------------------------------*/
  switch ( sig )
  {
    case HDRMC_SCP_CONFIG_RSP_TIMER_SIG:
      /*-----------------------------------------------------------------------
      Configuration Response timer expired:
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_SCP_CONFIG_RSP_TIMER_SIG( hdrscps_get_state() );
      hdrscp_process_config_rsp_timer_exp();
      break;

    case HDRMC_SCP_AN_INIT_STATE_TIMER_SIG:
      /*-----------------------------------------------------------------------
      AN Init State Timer Expired:
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_AN_INIT_STATE_TIMER_SIG( hdrscps_get_state() );
      hdrscp_process_an_init_state_timer_exp();
      break;

    default:
      /*-----------------------------------------------------------------------
      Default: signal not recognized.
      -----------------------------------------------------------------------*/
      ERR("Invalid overhead protocol signal!", 0, 0, 0);
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdrscp_process_sig() */

/*===========================================================================

FUNCTION HDRSCP_IS_CONFIGURING

DESCRIPTION
  Indicates whether SCP is in session configuration
 
DEPENDENCIES
  Must be called after hdrscp powerup initialization.

PARAMETERS
  None

RETURN VALUE
  TRUE  - SCP is in session configuration
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_is_configuring( void )
{
  return ( (hdrscps_get_state() == HDRSCPS_AN_INIT) ||
           (hdrscps_get_state() == HDRSCPS_AT_INIT) );
} /* hdrscp_is_configuring() */

/*===========================================================================

FUNCTION HDRSCP_SET_HARDLINK_ATTRIBS_TO_DEFAULT

DESCRIPTION
  

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to SCP's current configuration structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_hardlink_attribs_to_default
( 
  hdrscp_hardlink_config_params_type *current_config
)
{
  current_config->config_token = HDRSCP_SESS_CFG_TOKEN_ATTRIB_DEFAULT;
  current_config->support_gaup_session_config_token =
                            HDRSCP_SUPP_GAUP_SESS_CFG_TOKEN_ATTRIB_DEFAULT;
  current_config->personality_count = HDRSCP_PERSONALITY_CNT_ATTRIB_DEFAULT;
  current_config->support_config_lock =  HDRSCP_SUPP_CFG_LOCK_ATTRIB_DEFAULT;
  current_config->config_is_locked = FALSE;
#ifdef FEATURE_EHRPD
  current_config->ehrpd_was_advertised = FALSE;
#endif /* FEATURE_EHRPD */
} /* hdrscp_set_hardlink_attribs_to_default() */

/*===========================================================================

FUNCTION HDRSCP_SET_PER_PERSONALITY_ATTRIBS_TO_DEFAULT

DESCRIPTION
  This function initializes SCP's per personality configuration parameters
  to default.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to SCP's current configuration structure in SCMDB.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_per_personality_attribs_to_default
( 
  hdrscp_per_personality_config_params_type *current_config
)
{
  memset( current_config->subtype, 0,
          sizeof(hdrscp.session_params.current.subtype) );

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "scp set per personality to default" );
#ifdef FEATURE_LTE_TO_HDR_OH
  /* If CDMA is not the active RAT then initialize with IRAT subtypes */
  if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_SESSION_CONFIGURATION_PROTOCOL) )
  {
    current_config->subtype[HDRHAI_INITIALIZATION_STATE_PROTOCOL] = 
        HDRSCP_IRAT_INIT;
    current_config->subtype[HDRHAI_IDLE_STATE_PROTOCOL] = HDRSCP_IRAT_IDLE;
    current_config->subtype[HDRHAI_ROUTE_UPDATE_PROTOCOL] = HDRSCP_IRAT_RUP;
    current_config->subtype[HDRHAI_OVERHEAD_MESSAGES_PROTOCOL] = 
        HDRSCP_IRAT_OVHD;
    current_config->subtype[HDRHAI_SIGNALING_ADAPTATION_PROTOCOL] = 
        HDRSCP_IRAT_SAP;
    current_config->subtype[HDRHAI_RTC_MAC_PROTOCOL] = 
        HDRSCP_SUB3_RMAC;
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrscp_set_per_personality_attribs_to_default() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRSCP_IS_PERSONALITY_IRAT

DESCRIPTION
  This function checks whether SCP's current personality is IRAT.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE - current personality is IRAT
  FALSE - current personality is not IRAT 

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_is_personality_irat( void )
{
  boolean personality_is_irat = FALSE;
  uint16 subtype_sap = 
    hdrscp_get_config_subtype(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL);
  uint16 subtype_init = 
    hdrscp_get_config_subtype(HDRHAI_INITIALIZATION_STATE_PROTOCOL);
  uint16 subtype_idle = 
    hdrscp_get_config_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
  uint16 subtype_ovhd = 
    hdrscp_get_config_subtype(HDRHAI_OVERHEAD_MESSAGES_PROTOCOL);
  uint16 subtype_rup = 
    hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  uint16 subtype_rmac = 
    hdrscp_get_config_subtype(HDRHAI_RTC_MAC_PROTOCOL);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( (subtype_sap == HDRSCP_IRAT_SAP) &&
       (subtype_init == HDRSCP_IRAT_INIT) &&
       ( (subtype_idle == HDRSCP_IRAT_IDLE) ||
         (subtype_idle == HDRSCP_IRAT_QUICK_IDLE) ) &&
       (subtype_ovhd == HDRSCP_IRAT_OVHD) &&
       ( ( (subtype_rup == HDRSCP_IRAT_RUP) &&
           (subtype_rmac == HDRSCP_SUB3_RMAC) ) ||
         ( (subtype_rup == HDRSCP_IRAT_MC_RUP) &&
           (subtype_rmac == HDRSCP_MC_RMAC) ) ) )
  {
    personality_is_irat = TRUE;
  }

  HDR_MSG_PROT_7( MSG_LEGACY_MED, 
                  "IRAT personality: %d, SAP(%d), INIT(%d), IDLE(%d), OVHD(%d), RUP(%d), RMAC(%d)", 
                  personality_is_irat,
                  subtype_sap, subtype_init, subtype_idle, 
                  subtype_ovhd, subtype_rup, subtype_rmac );

  return personality_is_irat;

} /* hdrscp_is_personality_irat() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/*===========================================================================

FUNCTION HDRSCP_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of SCP

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of SCP

SIDE EFFECTS
  None

===========================================================================*/
void* hdrscp_get_inconfig_handle(void)
{
  return (void*) &hdrscp.session_params.negotiated;
} /* hdrscp_get_inconfig_handle() */

/*===========================================================================

FUNCTION HDRSCP_CONFIG_IS_LOCKED

DESCRIPTION
  This function checks if AT is allowed to send and receive configuration
  message.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Returns the value of ConfigLock.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_config_is_locked(void)
{
  return hdrscp.session_params.current.config_is_locked;
} /* hdrscp_config_is_locked() */

/*===========================================================================

FUNCTION HDRSCP_PHONE_IS_SET_TO_REV0_MODE

DESCRIPTION
  This function checks if the phone is provisioned to be in Rev0 mode.
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE if both PhyLayer Subtype 2/Subtpe 3 are disabled. 
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_phone_is_set_to_rev0_mode(void)
{

  return (!hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer &&
          !hdrscp.permitted_subtypes.prot_subtype_mask.sub3_phys_layer);

} /* hdrscp_phone_is_set_to_rev0_mode() */

/*===========================================================================

FUNCTION HDRSCP_PHONE_IS_SET_TO_REVA_MODE

DESCRIPTION
  This function checks if the phone is provisioned to be in RevA mode.
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  Returns TRUE if PhyLayer Subtype 2 is enabled. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_phone_is_set_to_revA_mode(void)
{

  return hdrscp.permitted_subtypes.prot_subtype_mask.sub2_phys_layer;

} /* hdrscp_phone_is_set_to_revA_mode() */

/*===========================================================================

FUNCTION HDRSCP_POWER_DOWN

DESCRIPTION
  This function is called during AT power down and saves the session
  status appropriately
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscp_power_down(void)
{
  nv_hdrscp_session_status_enum_type current_status = NV_SCP_SESSION_INACTIVE;
  nv_hdrscp_session_status_enum_type status = NV_SCP_SESSION_INACTIVE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if session status is inactive in NV, it is set by test script,
     should not overwrite this value. */
  (void) hdrutil_read_nv_item( NV_HDRSCP_SESSION_STATUS_I, 
                               (nv_item_type*)&current_status );

  if ( current_status != NV_SCP_SESSION_INACTIVE )
  {
    if ( hdrscp.session_status != HDRSCP_SESSION_INACTIVE )
    {
      status = (nv_hdrscp_session_status_enum_type)hdrscp.session_status;
    }
    else if ( hdrscmdb_prior_session_is_valid() )
    {
      status = NV_SCP_SESSION_ACTIVE;
    }
    else
    {
      status = NV_SCP_SESSION_INACTIVE;
    }
    
    if ( current_status != status )
    {
      hdrutil_write_nv_item( NV_HDRSCP_SESSION_STATUS_I,
                             (nv_item_type*)&status );
    }
  }
} /* hdrscp_power_down() */


/*===========================================================================

FUNCTION HDRSCP_AT_IS_DORB_CAPABLE

DESCRIPTION
  This function returns whether DORB is enabled in the NV item 
  NV_HDRSCP_FORCE_AT_CONFIG_I. 

  Please note that this function doesn't return whether DORB is negotiated. A
  return value of TRUE doesn't mean that DORB is negotiated. Despite AT's 
  capability, DORB configuration could still be rejected by AN.
  
DEPENDENCIES
  Assumes that the global variable force_at_config is already initialized.
  Must only be called after the NV item NV_HDRSCP_FORCE_AT_CONFIG_I has been
  read.

PARAMETERS
  TRUE - If AT is capable of DORB
  FALSE - Otherwise

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrscp_at_is_dorb_capable ( void )
{

  if ( (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_MMPA) ||
       (hdrscp.force_at_config == HDRSCP_REVB_PROTOCOLS_WITH_EHRPD) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* hdrscp_at_is_dorb_capable */

/* <EJECT> */ 
#ifdef FEATURE_EHRPD
/*=============================================================================
FUNCTION HDRSCP_UPDATE_EHRPD_CREDENTIALS

DESCRIPTION
  This function posts a command to SCP to process update to eHRPD credentials.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscp_update_ehrpd_credentials
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdrscp_cmd_type hdrscp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( !hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa && 
       !hdrscp.permitted_subtypes.app_subtype_mask.enable_ammpa )
  {
     HDR_MSG_PROT(MSG_LEGACY_MED, 
                  "Update eHRPD credentials recvd when eHRPD is not enabled");
     return;
  }

  hdrscp_cmd.name   = HDRSCP_UPDATE_EHRPD_CREDENTIALS_CMD;
  hdrscp_cmd.sender = sender;

  if ( rex_self() == HDRSCP_TASK_PTR )
  {
    hdrscp_process_cmd(&hdrscp_cmd);
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, 
            "Update eHRPD credentials cmd called from outside HDRMC!");
    hdrmc_queue_cmd( HDRHAI_SESSION_CONFIGURATION_PROTOCOL,
                     &hdrscp_cmd,
                     sizeof(hdrscp_cmd_type));
  }

} /* hdrscp_update_ehrpd_credentials() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_EHRPD_CREDENTIALS_ARE_PROVISIONED

DESCRIPTION
  This function returns whether eHRPD credentials are provisioned in the NV
  or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if eHRPD credentials are provisioned. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_credentials_are_provisioned( void )
{
  return hdrscp.ehrpd_credentials_are_provisioned;
}  /* hdrscp_ehrpd_credentials_are_provisioned */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_SET_EHRPD_CREDENTIALS_ARE_VALID

DESCRIPTION
  This function is used to set the flag whether eHRPD credentials are 
  provisioned or not.

DEPENDENCIES
  None.

PARAMETERS
  ehrpd_credentials_available - Flag indicating whether eHRPD credentials 
  are available

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_ehrpd_credentials_are_valid
( 
  boolean ehrpd_credentials_available 
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "Set eHRPD credentials value %d",
                  ehrpd_credentials_available );

  hdrscp.ehrpd_credentials_are_provisioned = ehrpd_credentials_available;

}  /* hdrscp_set_ehrpd_credentials_are_valid */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCP_EHRPD_AUTH_IS_IN_USIM

DESCRIPTION
  This function is used to check the flag whether eHRPD credentials are 
  in USIM.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_auth_is_in_usim ( void )
{
  return hdrscp.ehrpd_auth_in_usim;
}


/*===========================================================================

FUNCTION HDRSCP_SET_DATA_EHRPD_CAPABILITY_INTERNAL

DESCRIPTION
  This function sets the flag which tells eHRPD capability (e.g. APN,
  authentication) are valid or not from upper layer (data service)'s point of
  view.
  This function only sets the value and does not check whether it is needed
  to close session or not.

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
void hdrscp_set_data_ehrpd_capability_internal
( 
  boolean data_ehrpd_capability
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "Set DATA eHRPD capability value %d",
                  data_ehrpd_capability );

  hdrscp.data_ehrpd_capability = data_ehrpd_capability;

} /* hdrscp_set_data_ehrpd_capability_internal() */


/*===========================================================================

FUNCTION HDRSCP_LTE_IS_ATTACHED

DESCRIPTION
  This function resets ehrpd_was_advertised flag after LTE successful attach
  so that session will be renegotiated if current session is HRPD.
 
  This function should be called only after a successful LTE attach.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_lte_is_attached( void ) 
{
  hdrscp_config_params_type scp_cfg;

/*---------------------------------------------------------------------------*/

  hdrscmdb_get_scp_session_info(&scp_cfg);

  /* Normally, if AT has proposed eHPRD before but AN picks HPRD, AT won't close
   * HRPD session even if eHPRD capabiltiy is TRUE. However, after a successful 
   * LTE attach, we would like to force current HPRD session to be closed. 
   * Reset ehrpd_was_advertised flag so that HPRD session will be closed when 
   * hdrscp_ehrpd_status_is_valid() is called. */ 
  if ( !hdrmrlpc_current_personality_is_ehrpd() &&
       scp_cfg.ehrpd_was_advertised )
  {
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "Ready to close HPRD session after LTE attach" );
    scp_cfg.ehrpd_was_advertised = FALSE;
  }

  /* Save changes */
  hdrscmdb_set_scp_session_info(&scp_cfg);

} /* hdrscp_lte_is_attached() */


/*===========================================================================

FUNCTION HDRSCP_SET_DATA_EHRPD_CAPABILITY

DESCRIPTION
  This function sets the flag which tells eHRPD capability (e.g. APN,
  authentication) are valid or not from upper layer (data service)'s point of
  view.

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
void hdrscp_set_data_ehrpd_capability
( 
  boolean data_ehrpd_capability
) 
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "Received DATA eHRPD capability value %d",
                  data_ehrpd_capability );

  hdrscp.data_ehrpd_capability_initialized = TRUE;
  hdrscp.data_ehrpd_capability = data_ehrpd_capability;

  /* Check whether there is need to close current session.
   * If network is not acquired, change the flag but do not close the session.
   * When HDR is acquired, there is another eHRPD capability check. */
  if ( !(hdrscp.ehrpd_no_check) && /* Do not close session if ehrpd no check is set */
  	   hdralmp_system_is_acquired( HDRHAI_SESSION_CONFIGURATION_PROTOCOL ) &&
       hdrstream_stream_is_assigned( HDRHAI_APP_PACKET_APP_ANY ) &&
       !hdrscp_ehrpd_status_is_valid() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
       "eHRPD availability of current session is invalid. Close session" );

    hdrind_give_ind( HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED, NULL );
  }
} /* hdrscp_set_data_ehrpd_capability() */


/*===========================================================================

FUNCTION HDRSCP_EHRPD_IS_ALLOWED

DESCRIPTION
  This function tells whether eHRPD capability should be advertised based on
  all the condition (APNs, eHRPD network setting, eHRPD credentials) when this
  funtion is called.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - eHRPD is allowed. 
  FALSE - eHRPD is not allowed.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_is_allowed( void ) 
{
  boolean ehrpd_is_allowed = TRUE;

/*---------------------------------------------------------------------------*/

  /* eHRPD should be advertised only when
   * 1. at least one of Alternate EMPA and Alternate MMPA is supported; 
   * 1. eHRPD credentials are provisoned;
   * 2. Data eHRPD are initialized and valid; 
   * 3. eHRPD network setting is True. 
   */
  if ( !(hdrscp.permitted_subtypes.app_subtype_mask.enable_aempa || 
         hdrscp.permitted_subtypes.app_subtype_mask.enable_ammpa) )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "neither Alternate EMPA nor Alternate MMPA is supported" );
    ehrpd_is_allowed = FALSE;
  }
  if ( !hdrscp_ehrpd_credentials_are_provisioned() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "eHRPD credentials are not provisioned" );
    ehrpd_is_allowed = FALSE;
  }
  if ( hdrscp.data_ehrpd_capability_initialized && !hdrscp.data_ehrpd_capability )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Data eHRPD are not allowed" );
    ehrpd_is_allowed = FALSE;
  }
  if ( !hdrscp.ehrpd_network_setting )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "eHRPD network setting is False" );
    ehrpd_is_allowed = FALSE;
  }

  HDR_MSG_PROT_1( MSG_LEGACY_MED, "eHRPD availability is %d", ehrpd_is_allowed );
  return ehrpd_is_allowed;
} /* hdrscp_ehrpd_is_allowed() */

/*===========================================================================

FUNCTION HDRSCP_EHRPD_WAS_ADVERTISED

DESCRIPTION
  This function tells whether eHRPD capability was advertised when negotiating
  ATSupportedApplicationSubtypes attribute.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - eHRPD has been advertised. 
  FALSE - eHRPD was not advertised.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_was_advertised( void ) 
{ 
  return hdrscp.session_params.negotiated.ehrpd_was_advertised;
} /* hdrscp_ehrpd_was_advertised() */

/*===========================================================================

FUNCTION HDRSCP_EHRPD_STATUS_IS_VALID

DESCRIPTION
  This function checks whether eHRPD status of current session is valid.
 
  AT's eHRPD   Current active  has eHRPD advertised        Action
  capability	 personality   when session negotiated 
    True          HRPD             True	          Retain the current DO session
    True          HRPD             False          Close the current session
    True          eHRPD            True           Retain the current DO session
    True          eHRPD            False                    NA
    False         HRPD             True	          Retain the current DO session
    False         HRPD             False          Retain the current DO session
    False         eHRPD            True           Close the current session
    False         eHRPD            False                    NA

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - Session is valid.
  FALSE - eHRPD availability of current session is invalid. Session needs to be
          closed.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_ehrpd_status_is_valid( void ) 
{
  /* TBD, currently we assume that if there is an eHRPD personality, it is
   * the current and the only one personality in the session.
   * If this is not the case, we replace 
   * hdrmrlpc_current_personality_is_ehrpd() with other functions. 
   */ 
  hdrscp_config_params_type scp_cfg;

/*---------------------------------------------------------------------------*/

  hdrscmdb_get_scp_session_info(&scp_cfg);

  /* Need to be removed */
  HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                  "eHRPD is allowed: %d; current personality: %d; eHRPD was advertised: %d",
                  hdrscp_ehrpd_is_allowed(), 
                  hdrmrlpc_current_personality_is_ehrpd(), 
                  scp_cfg.ehrpd_was_advertised );
  
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Hotswap: eHRPD no check is %d", hdrscp.ehrpd_no_check );

  /* Current personality is eHRPD, but eHRPD is no longer available for AT. */
  if ( !hdrscp_ehrpd_is_allowed() &&
       hdrmrlpc_current_personality_is_ehrpd() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Session is eHRPD. eHRPD is no longer available" );
    return FALSE;
  }
  /* eHRPD capability should be advertised while current personality is HPRD.
   * We do not close session if in the previous session eHRPD was advertised 
   * but AN did not pick eHRPD. */ 
  else if ( hdrscp_ehrpd_is_allowed() && 
            !hdrmrlpc_current_personality_is_ehrpd() && 
            scp_cfg.ehrpd_was_advertised == FALSE )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Session is HRPD. eHRPD is available and should be advertised" );
    return FALSE;
  }

  return TRUE;
} /* hdrscp_ehrpd_status_is_valid() */

/*===========================================================================

FUNCTION HDRSCP_CHECK_OK_TO_OPEN_STATE

DESCRIPTION
  This function checks whether the current eHPRD availability before SCP 
  goes to OPEN state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - SCP is OK to go to OPEN state.
  FALSE - eHRPD status may have changed and current session has to be closed.
          SCP won't go to OPEN state.
 
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscp_check_ok_to_open_state( void )
{
  if ( !hdrscp_ehrpd_status_is_valid() )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "eHRPD availability of current session is invalid. Close session" );

    hdrind_give_ind( HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED, NULL );
    return FALSE;
  }

  HDR_MSG_PROT( MSG_LEGACY_MED, "SCP is OK to move to OPEN state" );
  return TRUE;
} /* hdrscp_check_ok_to_open_state() */


/*===========================================================================

FUNCTION HDRSCP_SET_EHRPD_NO_CHECK

DESCRIPTION
  This function is used to set/reset the flag, which indicates whether eHRPD 
  validation should be made, when there is a trigger.

DEPENDENCIES
  None.

PARAMETERS
  Value - Flag indicating if ehrpd check needs to be performed or not when
          there is any trigger to validate ehrpd.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscp_set_ehrpd_no_check
( 
  boolean value 
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "eHRPD no check is set to %d", value);

  hdrscp.ehrpd_no_check = value;

}  /* hdrscp_set_ehrpd_changed_session_evt */


#endif /* FEATURE_EHRPD */

/*===========================================================================

FUNCTION HDRSCP_GET_CURRENT_HDR_REV

DESCRIPTION
  This function returns current HDR revision. 
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  SYS_ACTIVE_PROT_NONE - The protocol revision is unknown (no session)
  SYS_ACTIVE_PROT_HDR_REL0 - HDR release 0
  SYS_ACTIVE_PROT_HDR_RELA - HDR release A
  SYS_ACTIVE_PROT_HDR_RELB - HDR release B

SIDE EFFECTS
  None.

===========================================================================*/

sys_active_prot_e_type hdrscp_get_current_hdr_rev(void)
{
  sys_active_prot_e_type hdr_active_prot;
  uint16 phys_subtype = HDRSCP_DEFAULT_PHYS_LAYER;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_subtype = hdrscp_get_config_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

  switch(phys_subtype)
  {
    case HDRSCP_SUB3_PHYS_LAYER:
      hdr_active_prot = SYS_ACTIVE_PROT_HDR_RELB;
      break;

    case HDRSCP_SUB2_PHYS_LAYER:
      hdr_active_prot = SYS_ACTIVE_PROT_HDR_RELA;
      break;

    case HDRSCP_DEFAULT_PHYS_LAYER:
      hdr_active_prot = SYS_ACTIVE_PROT_HDR_REL0;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Invalid phys layer subtype %d", 
                       phys_subtype);
      hdr_active_prot = SYS_ACTIVE_PROT_NONE;
  }

  return hdr_active_prot;

} /* hdrscp_get_current_hdr_rev */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRSCP_UPDATE_SUBTYPE_DEFAULTS

DESCRIPTION
  This function resets the defaults for protocol subtype to subtype0.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void hdrscp_update_subtype_defaults( void )
{
  HDR_MSG_PROT( MSG_LEGACY_HIGH, "Resetting SCP subtypes" );

  memset( &hdrscp.session_params.current.subtype[0], 0, 
          sizeof( hdrscp.session_params.current.subtype ) );

  memset( &hdrscp.session_params.negotiated.subtype[0], 0, 
          sizeof( hdrscp.session_params.negotiated.subtype ) );

  /* Change back default subtypes in native HDR */
  hdrscmdb_set_inuse_to_default();

} /* hdrscp_update_subtype_defaults() */
#endif /* FEATURE_LTE_TO_HDR_OH */



/*===========================================================================

FUNCTION HDRSCP_SESSION_NEGOTIATION_IN_PROGRESS

DESCRIPTION
  This function checks if session negotiation is in progress or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE -if session negotiation is in progress
  FALSE - session negotiation NOT in progress

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscp_session_negotiation_in_progress(void)
{

     if ((hdrscp.negotiation_state == HDRSCP_NEG_COMPLETE)
	 	  || (hdrscp.negotiation_state == HDRSCP_NEG_NONE)
	 	  || (hdrscp.negotiation_state == HDRSCP_NEG_PRIOR_SESSION_COMPLETE))
     {
        if(hdrscp.session_status == HDRSCP_SESSION_ACTIVE)
        {
          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "hdrscp_session_negotiation_in_progress returning FALSE neg_state %d sess_status %d",
                                              hdrscp.negotiation_state, hdrscp.session_status);
			return FALSE; /* session is opened */
        }
		else
		{
          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "hdrscp_session_negotiation_in_progress returning TRUE neg_state %d sess_status %d",
                                              hdrscp.negotiation_state, hdrscp.session_status);

			return TRUE; /* session is not yet open, UE is going to try shortly */
		}	  
     }
	 else
	 {
          HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "hdrscp_session_negotiation_in_progress returning TRUE neg_state %d sess_status %d",
                                              hdrscp.negotiation_state, hdrscp.session_status);

	    return TRUE;
	 }	 	

}/* hdrscp_session_negotiation_in_progress */

