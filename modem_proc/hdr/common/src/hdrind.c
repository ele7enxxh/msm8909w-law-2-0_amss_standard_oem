
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 H D R   I N D I C A T I O N   R O U T E R  

GENERAL DESCRIPTION
  This module routes indications to the protocols that register to 
  receive them.  All protocols that send indications must send them through
  the hdrind_give_ind() function.

EXTERNALIZED FUNCTIONS (Global)
  hdrind_give_ind - Give an indication to other protocols
  
EXTERNALIZED FUNCTIONS (Regional)
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrind.c_v   1.51   17 Mar 2003 12:28:20   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrind.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/16   vko     FR 33944: Inform D2L redir success and failure to DS
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
02/04/15   sjj     HDRIND_ACMAC_TX_STARTED is handled in hdridle for reporting RAT's L3
                   state to CXM.
11/06/14   rmv     Handle FTCMAC supervision in RMAC to stop processing  packets
                   from Tx queues
04/07/14   vko     Fixed IDLE giving SLEEP command to LMAC, when SRCH moves from 3a->3f
01/27/14   sat     Mainlined FEATURE_HYBRID_ACCESS_DURING_OVERHEAD.
10/30/13  kss/rmv  FR3272 - Modem API for Cost of Power estimate changes
01/07/14   vko     HDRIND_RTCMAC_SHUTDOWN_COMPLETE is handled in hdrlmac for clearing 
                   pending queue
08/29/13   vko     FR2812 - Pausing sync timer from HDRCP when HDRSRCH is in 2a state
05/06/13   vko     HDR CP not to go to IDLE unless HDR SRCH moves to MONITOR state
04/18/13   kss     Added notification for CCMAC when connection fails.
01/07/13   kss     Added notification for CCMAC when timesync is done.
01/02/12   cnx     Fixed no IRMI response to DS when HDR is deactivated.
10/02/12   wsh     Fixed data stall issue after active HO
09/20/12   cnx     Save session to EFS when powerdown in tunnel.
09/11/12   cnx     Handle TX and RX messages during active handoff
08/22/12   cnx     Abort sending IRMI when HDR exits online.
04/25/12   cnx     Added HDRIND_SAP_RETURNING_TO_CLOSE.
03/30/12   cnx     Close virtual connection when session is closed.
03/08/12   wsh     Merged NLB support
01/27/11   cnx     Support pre-Registration status update.
01/06/12   sju     Check trans id when SAPTX receives ACK for UL messages.
01/06/12   cnx     Add indication HDRIND_ALMP_RESELECT_EXIT_HDR.
01/06/12   cnx     Add parameter to HDRIND_OVHD_LTE_CELL_CHANGED.
11/28/11   cnx     Support HDRIND_OVHD_COLOR_CODE_CHANGED.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/10/11   cnx     Support HDRIND_OVHD_LTE_CELL_CHANGED; Adjusted the sequence
                   of protocols registered for HDRIND_OVHD_PREREG_INFO_UPDATED.
09/24/11   cnx     Support HDRIND_AMP_PREREG_ZONE_CHANGED.
                   Registered AMP for HDRIND_OVHD_PREREG_INFO_UPDATED 
                   and HDRIND_SAP_IDLE_HANDOFF.
08/26/11   cnx     Deregistered HDRIND_INIT_NETWORK_ACQUIRED for SCP. 
07/27/11   cnx     Merged Optimized Handoff changes.
06/03/11   cnx     Registered HDRIND_INIT_NETWORK_ACQUIRED for SCP. 
                   Renamed HDRIND_SCP_EHRPD_CREDENTIALS_CHANGED. 
05/18/11   wsh     Added network load balancing support
11/08/10   kss     Added Access MAC to rx HDRIND_LMAC_ACCESS_MODE_ENTERED.
07/30/10   kss     Added support for ConnectionFailureReport based on SAR  
07/30/10   cnx     Featurized callbacks related to GAUP MultiATPageMsgSupported 
07/29/10   cnx     Added support for GAUP MultiATPageMessageSupported attribute 
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
06/10/10   pba     IRAT Measurement feature.
05/19/10   pba     Added support for Reverse Time Transfer.
03/02/10   pba     Added support for LTE to HDR reselection
02/22/10   pba     Reset the connection retry counter once connection is opened
                   succesfully.
11/12/09   sju     Added support for SVDO.
10/30/09   pba     Added ehrpd support.
09/18/09   wsh     CMI: support changes from modem DATA
02/03/09   kss     Merged HDRIND_CCMAC_SCC_PKT_RXED support.
01/12/09   etv     Disable DTXMode after connection close or fail.
12/23/08   pba     Support for Quick Idle Protocol
09/03/08   etv     Added HDRIND_SCM_PHY_AN_GAUP.
08/13/08   sju     Registered SMP for ACMAC_TRANSMISSION_SUCCESSFUL indication.
06/18/08   etv     Registered PHY module for SCM_SESSION_CHANGED indication.
06/08/08   wsh     Merged DCM on mainbranch
09/19/07   cc      Replaced HDRIND_RMAC3_... indications by HDRIND_RMAC_....
08/30/07   etv     Added HDRIND_RTC_RESTART_TIMER_EXPIRED.
08/15/07   etv     Added HDRIND_PHY_CONFIG_COMPLETE & HDRIND_PHY_PROT_NEG_FAILED
08/23/07   pba     Do not do DDARF for rude wakeups.
08/21/07   wsh     Made changes to support Silent Redial for sending CR+RoR 
07/23/07   sju     Added HDRIND_LMAC_SCHEDULER_GROUP_CHANGED
06/19/07   cc      Added FMAC AT-Init. config. support.
06/14/07   kss     Added support for AT-initiated AC MAC config.
06/13/07   etv     Added HDRIND_OUTAGE_IMMINENT for Outage Notification.
05/29/07   mpa     Added HDRIND_AMP_FAILED_NEW_NAI.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
03/30/07   etv     Removed ind data from HDRIND_RMAC3_FLOW_MAPPING_CHANGED.
03/27/07   wsh     Added support for new prior session handling
02/20/07   yll     Removed FEATURE_EIDLE_SCI for HDRIND_INIT_NETWORK_ACQUIRED
                   indication.
02/01/07   sju     Added support for QOS HHO
01/08/07   yll     Added support for CSNA
11/16/06   sju     Obsoleted HDRIND_FCP_ALL_STREAMS_CLOSED indication.
09/19/06   pba     Added new indication HDRIND_IDLE_NETWORK_LOST, payload data
                   for HDRIND_LMAC_AWAKE.
09/18/06   etv     Registered pac_ind_cb for init_network_acquired ind.
09/08/06   spn     Added support to give indications to reservation module, 
                   for connection open/close
07/24/06   mpa     Added GMMCDP and RMAC config complete to get_ind_data_size
07/18/06   dna     Major RASM changes to allow reliable QoS call setup.
06/14/06   etv     Added AT Gaup Indications for HDRIDLE.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
05/03/06   ljl     Added support for hybrid access
06/05/06   pba     Removed AMP callback from HDRIND_SCM_SESSION_CHANGED.
05/09/06   yll     Added support for Minimal UATI Mode.
04/26/06   pba     Added HDRIND_SCP_NO_PREV_SESSION indication
04/21/06   hal     Added sizeof information for HDRIND_SCM_SESSION_CHANGE
                   indication data
04/17/06   pba     Removed ovhd call back from HDRIND_IDLE_CONNECTION_OPENED.
04/11/06   yll     Added HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI indication.
04/06/06   kss     Added HDRIND_BCMCS_SLEEP_OK ind; register BCMAC for 
                   HDRIND_LMAC_AWAKE. 
03/22/06   pba     Added new indications for AN GAUP fail.
03/02/06   kss     CC MAC now registers for LMAC_AWAKE.
02/23/06   hal     Added new indication to support AN GAUP of AMP attribs
02/07/06   dna     Added 7 indications missing from hdrind_get_ind_data_size()
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
01/17/06   etv     Return right size for ind data of RMAC3_FLOW_<De>Activated.
01/14/06   dna     Don't use new session parameters until ALMP completely 
                   transitions out of connected state.
12/13/05   pba     Obsoleted SCP_RECONFIGURED indication
12/06/05   etv     Added new indications for AT-init GAUP and Neg support for 
                   AMAC and Generic Multi-mode Capab Disc protocol.
11/15/05   mpa     hdrbccp now registers for HDRIND_ALMP_RETURNING_TO_IDLE and
                   new HDRIND_IDLE_MONITOR_CONNECTION_FAILED
11/15/05   dna     Allow AT to actually use the GAUPed values for RateParams
11/14/05   dna     Phase 0 support for multiple personalities
11/04/05   etv     Changed the order of callbacks for scm session changed ind
                   and added indications HDRIND_RMAC3_FLOW_MAPPING_CHANGED and
                   HDRIND_RMAC3_FLOW_<DE>ACTIVATED
11/01/05   pba     Removed AMP indication callback from HDRIND_SCP_RECONFIGURED,
                   because HDRIND_SCM_SESSION_CHANGED is sufficient.
10/31/05   dna     Include RMAC GAUP attributes in hdrind_get_ind_data_size.
10/28/05   mpa     Close SMP based on ALMP_CONNECTION_CLOSED ind instead of
                   CONNECTED_CONNECTION_CLOSED
10/26/05   dna     Integration updates to GAUP support for RMAC1 rate params
10/26/05   dna     Continue using HDRIND_RTCMAC_SHUTDOWN_COMPLETE, for now.
10/22/05   dna     SMP should close based on CONNECTED_CONNECTION_CLOSED ind,
                   not RMAC_SHUTDOWN_COMPLETE.
10/21/05   pba     Added Broadcast protocol to SCM
10/18/05   pba     Give HDRIND_SMP_SESSION_CLOSED indication to HMP and 
                   Connected state protcool
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/30/05   mpa     Obsolete HDRIND_OVHD_BC_UPDATED.
09/19/05   mpa     Obsolete HDRIND_BCMCS_FREQ_CHANGE_INITIATED.
                   hdrind_ovhd_bc_updated no longer sent to hdrbccp.
09/07/05   etv     Added support for AT-init GAUP for RMAC ST-1.
08/28/05   mpa     Handle hdrovhd_bc_updated indication.
08/22/05   etv     Added support for AN Init GAUP for FMAC.
08/22/05   mpa     Added hdrovhd_bc_updated indication for BCMCS (place holder)
08/07/05   mpa     Added AT Initiated GAUP Indications for MRLP.
07/07/05   dna     Added support for Enhanced Idle State protocol
06/21/05   etv     Registered CMAC for Idle.ConnectionOpened and Connected.
                   ConnectionClosed indications. Added cmac_interrupted 
                   indication.
03/24/05   pba     Lint cleanup
03/07/05   dna     Reformatted hdrind_ind_table because each entry in the 
                   table points to a variable length list of function pointers,
                   not to an array.  This change makes the data types used
                   be correctly defined and fixes lint warnings.
03/07/05   pba     Process all indications to PAC in DS task context. Session 
                   related indications need processing both by PAC in DS task
                   context and PACC in MC task context.
03/07/05   pba     Give HDRIND_RUP_CONNECTION_INITIATED to PAC to allow 
                   initialization of RLP before AT starts receiving data on FTC.
11/29/04   dna     Count AEHO as an AHO to avoid infinite AEHO handoffs.
                   LMAC no longer needs HDRIND_ACMAC_TRANSMISSION_STOPPED.
11/29/04   grl     Added suspend traffic and resume traffic inds.
11/29/04   pba     Changed order in which indication are given in 
                   HDRIND_OVHD_INFO_UPDATED
11/09/04   dna     Added HDRIND_IDLE_CONNECTION_ABORTED to allow 
                   the connection setup to be aborted.
11/08/04   pba     Added HDRIND_RUP_EXIT_HDR_MODE,HDRIND_RUP_CHANGE_HDR_SYSTEMS
11/04/04   mpa     Added HDRIND_SCM_RMAC_GAUP, HDRIND_SCM_MRLP_GAUP.
11/01/04   mpa     Added HDRIND_BCMCS_FREQ_CHANGE_INITIATED.
09/22/04   vas     Added HDRIND_FCP_DATA_READY_RCVD to hdrind_get_data_size
09/09/04   mpa     Added HDRIND_PROT_NEG_FAILED, HDRIND_CONFIG_COMPLETE,
                   HDRIND_CONFIG_REQ (generic AT init negotiation)
08/26/04   mpa     Added HDRIND_AMP_FAILED_NEW_ESN.  
                   Give SCP_RECONFIGURED indication to amp.
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
07/02/04   mpa     Give Session negotiation related indications to HDRBCCP
05/21/04   dna     Abort HMPSA on HDRIND_RUP_CONNECTION_INITIATED instead of 
                   HDRIND_RTCMAC_LINK_ACQUIRED, to avoid getting stuck if we
                   call hdrlmac_access_mode during connection setup but the 
                   connection setup fails.
04/13/04   dna     Allow ALMP to close connection immediately if TX is stopped.
03/16/04   dna     Re-start HMPSA state machine if it is in the middle of
                   starting another access attempt when one ends.
02/23/04   mpa     Added HDRIND_ACMAC_TRANSMISSION_FAILED to hdrind_get_data_size
02/13/04   vas     PAC does not get connection opened/closed indications
11/14/03    vr     Send Session Closed to HDRSTREAM before HDRPAC.
11/10/02    km     Added HDRIND_LMAC_HDR_USABLE support.
10/13/03   vas     Give Connection Close and Connection End to HDRPAC
07/21/03   mpa     Merged in 6500 baseline
06/26/03   mpa     Added HDRIND_IDLE_CONNECTION_FAILED to hdrind_get_data_size
06/18/03   vas     Dont give ConnOpened/ConnClosed to PAC. Give SessionClosed
05/15/03   mpa     Added HDRIND_LUP_ALN_TRIGGER
03/14/03   mpa     Updated hdrind_get_data_size().
03/12/03   mpa     Removed obsolete LUP_PROT_NEG_FAILED indication
02/28/03   mpa     Restored giving CC missed indication to hdridle
01/30/03   mpa     Added support for HDRIND_LMAC_RESELECT
                   Give HDRIND_RTCMAC_SHUTDOWN_COMPLETE ind to smp as well.
01/29/03   kss     Added HDRIND_LMAC_CC_MISSED indication; add hdrlmac to 
                   receive end of sync indication.
01/14/03   vas     Give HDRIND_SMP_SESSION_CLOSED to hdrpac instead of 
                   hdrftap/hdrrtap
12/10/02   dna     Added support for HDRIDLES_SUSPEND_STATE                   
10/03/02   vas     Give CONNECTION_CLOSED ind to hdrpac.
08/21/02   mpa     Add HDRIND_LMAC_ACCESS_ENTRY_HANDOFF and give to slp
08/01/02   dna     Give OVHD_INFO_UPDATED to idle last to not sleep if we xmit.
07/10/02   dna     Support LMAC limiting number of consecutive AHOs
05/13/02   nmw     Added SCP receiving IDLE.ConnectionFailed
03/29/02   dna     Give HDRIND_FCP_ALL_STREAMS_CLOSED to idle for 40s sleep
                   Add HDRIND_CCMAC_SLEEP_CAPSULE_DONE and give to idle.
02/12/02   dna     Re-format the indication routing table so it can be indexed
                   directly by indication rather than a linear search.
01/29/02   vas     Give ACMAC_TRANSMISSION_SUCCESSFUL to RUP.
01/21/02   sh      Give network acquired to IDLE to check if PCCC 
                   configuration need to initialized or not.
01/17/02   dna     Give network acquired to SMP to do some initialization
12/21/01   dna     Tell OVHD of con. setup to avoid making ovhd info invalid.
11/30/01   om      Added security layer support
11/14/01   om      Addded IS-890 specific callbacks
09/04/01   dna     Added HDRIND_RUP_CONNECTION_INITIATED based on DDR-46
                   to reset SLP before reliable RTCACK message comes.
08/27/01   kss     Pass session close, previous session open and scp reconfig
                   to MAC layer protocols; rename DRC supervision indications
08/17/01   vas     Added HDRIND_FCP_DATA_READY_RCVD. Added support for PAC to
                   receive appropriate indications.
08/14/01   dna     Pass session closed and prev session opened to STREAM.
07/10/01   dna     Remove support for X4 and did some other code cleanup
                   Change LUP to not do AT initiated negotiation
                   HDRIND_IDLE_CONNECTION_OPENED now includes an open reason
06/06/01   dna     Give SCP_RECONFIGURED indication to stream protocol
04/05/01   vas     Changed to include new session protocol files for X4 builds
                   too since the IS856 session indications are not featurized
03/29/01   dna     New indications for LUP, RUP and HMP, updated ind_data_size
                   to include sizes for LUP, IDLE, OVHD and SCP indications.
03/19/01   dna     Added support for IS-856 compliant session layer indications
03/10/01   sh      added idle state related indication callback entries
02/09/01   dna     Send HDRIND_ACMAC_TRANSMISSION_FAILED to SLP before ALMP.
                   It was the other way to accommodate protocols re-sending
                   AC messages, but now we don't do that.
02/07/01   dna     Added ALMP_RETURNING_TO_IDLE indication
                   Give ALMP_RETURNING_TO_IDLE to SLP and HMP, give
                   HDRIND_ALMP_REACQUIRING and HDRIND_ALMP_EXIT_HDR_MODE to
                   SLP.
10/13/00   sh      added idle callback for session boot completed ind.
09/07/00   dna     Changed most of the indications as a result of design
                   changes.
03/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrind.h"
#include "hdrerrno.h"

#include "hdrsmp.h"
#include "hdramp.h"
#include "hdrscp.h"
#include "hdrrlp_api.h"
#include "hdrstream.h"

#include "hdralmp.h"
#include "hdrovhd.h"
#include "hdrinit.h"
#include "hdridle.h"
#include "hdrrup.h"
#include "hdramac.h"
#include "hdrcmac.h"
#include "hdrfmac.h"
#include "hdrrmac.h"
#include "hdrslp.h"
#include "hdrhmp.h"
#include "hdrlmac.h"
#include "hdrinit.h"
#include "hdridle.h"
#include "hdrcon.h"
#include "hdrkep.h"
#include "hdrauth.h"
#include "hdrdebug.h"
#include "err.h"
#include "hdrlup.h"
#include "hdrpacc.h"
#include "hdrpac_api.h"

#ifdef FEATURE_HDR_IS890
  #include "hdrftap.h"
  #include "hdrrtap.h"
#endif

/* _mpa_ revisit featurisation? hdrbccp.c is featurized... */
#ifdef FEATURE_HDR_BCMCS
#include "hdrbccp.h"
#include "hdrbcmac.h"
#endif /* FEATURE_HDR_BCMCS */

#ifdef FEATURE_HDR_QOS
  #include "hdrmrlp_api.h"
#endif /* FEATURE_HDR_QOS */ 

#include "hdrcsna.h"
#include "hdrmac.h"
#include "hdrphy.h"
#include "hdrsap.h"

#ifdef FEATURE_HDR_REVC
#include "sys.h"
#include "sys_v.h"  /* for sys_link_type */
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsaptx.h"
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* General signature for indication callback functions registered with HMP */
typedef void ( hdrind_cb_f_type ) ( hdrind_ind_name_enum_type ind_name,
                                    void *ind_data );

/* Each cb_list is an array of pointers to the callback functions */
typedef hdrind_cb_f_type *hdrind_cb_list_type[];

/* Static registration for indications.  The indications are given in the 
 * order they are listed in the table. The general rule is that indications
 * are to be given to the lowest layer first, the highest layer last.  For
 * indications that are given to multiple protocols in the same layer, the
 * order must be carefully chosen to avoid race conditions.  If the race
 * condition can not be avoided, a second indication should be used to
 * synchronize the processing.  Specific comments MUST be included below
 * describing what race conditions are being solved by the chosen order of 
 * indication delivery.
 */

/* 00 HDRIND_SBOOT_BOOT_COMPLETED *** obsolete *** */
/* 01 HDRIND_SBOOT_SESSION_ABORTED *** obsolete *** */
/* 02 HDRIND_SCTL_SESSION_CLOSED *** obsolete *** */

/* 03 HDRIND_ALMP_REACQUIRING */
static const hdrind_cb_list_type hdrind_almp_reacquiring = 
{
  hdrslp_ind_cb, 
  hdrovhd_ind_cb, 
  hdrhmp_ind_cb, 
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif
  NULL 
};

/* 04 HDRIND_ALMP_EXIT_HDR_MODE */
static const hdrind_cb_list_type hdrind_almp_exit_hdr_mode = 
{
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrpac_ind_cb,
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrslp_ind_cb, 
  hdrhmp_ind_cb, 
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif
  hdrsmp_ind_cb,
#ifdef FEATURE_LTE_TO_HDR_OH
  hdramp_ind_cb,
#endif /* FEATURE_LTE_TO_HDR_OH */
  NULL 
};

/* 05 HDRIND_OVHD_INFO_UPDATED */
static const hdrind_cb_list_type hdrind_ovhd_info_updated = 
{
  hdrrup_ind_cb, 
  hdramp_ind_cb,  
  hdrhmp_ind_cb, 
  hdrscp_ind_cb, 
  hdrsmp_ind_cb, 
#ifdef FEATURE_HDR_BCMCS
  hdrbccp_ind_cb,
#endif /* FEATURE_HDR_BCMCS*/  
  hdridle_ind_cb,  /* See comment below */
  NULL
};
/* We give indication to AMP before HMP. In the specific scenario when we
 * are in HDR traffic and make a short voice call with AT crossing the subnet
 * boundary while on 1x. At the end of the voice call, AT needs to send a
 * UATI request since it has crossed subnet boundary and connection request 
 * since there is pending data. Giving indication to AMP before HMP ensures 
 * UATI request will be sent prior or with connection request in the same 
 * ACMAC capsule
 */

/* We give this indication to idle state last as an optimization to avoid 
 * going to sleep in the case where some other protocol may need to transmit
 * a message.  Also, the specific case where we go to sleep and then 
 * immediately wake up to send a UATIRequest is suspected to cause a problem
 * with the HDR sleep controller, so this avoids the suspected scenario.
 */

/* 06 HDRIND_OVHD_AP_UPDATED */
static const hdrind_cb_list_type hdrind_ovhd_ap_updated = 
{
  hdrhmp_ind_cb, 
  hdridle_ind_cb, 
  NULL 
};

/* 07 HDRIND_OVHD_SUPERVISION_FAILED */
static const hdrind_cb_list_type hdrind_ovhd_supervision_failed = 
{
  hdralmp_ind_cb, 
  hdrphy_ind_cb,
  NULL 
};

/* 08 HDRIND_OVHD_CHANGE_HDR_SYSTEMS */
static const hdrind_cb_list_type hdrind_ovhd_change_hdr_systems = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 09 HDRIND_OVHD_EXIT_HDR_MODE */
static const hdrind_cb_list_type hdrind_ovhd_exit_hdr_mode = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 10 HDRIND_INIT_NETWORK_ACQUIRED */
static const hdrind_cb_list_type hdrind_init_network_acquired = 
{
  hdralmp_ind_cb, 
  hdrsmp_ind_cb, 
  hdrhmp_ind_cb,
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif
  /* FEATURE_EIDLE_SCI is removed, because HDRPAC needs 
   * this indication to clean up QoS reservations when 
   * AT has a RASM Queue full error. This feature
   * will be handled in HDRPAC module */
  hdrpac_ind_cb,
  NULL 
};

/* 11 HDRIND_INIT_EXIT_HDR_MODE */
static const hdrind_cb_list_type hdrind_init_exit_hdr_mode = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 12 HDRIND_IDLE_CONNECTION_OPENED */
static const hdrind_cb_list_type hdrind_idle_connection_opened = 
{
  hdrcmac_ind_cb,
  hdralmp_ind_cb, 
  hdrsmp_ind_cb, 
  hdrpac_ind_cb, 
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif 
#ifdef FEATURE_HDR_QOS
  hdrmrlpresv_ind_cb,
#endif
  hdrovhd_ind_cb,
  hdrscp_ind_cb,
#ifdef FEATURE_HDR_MODEM_API
  hdrlmac_ind_cb,
#endif /* FEATURE_HDR_MODEM_API */
  NULL 
};

/* 13 HDRIND_IDLE_CONNECTION_FAILED */
static const hdrind_cb_list_type hdrind_idle_connection_failed = 
{
  hdrovhd_ind_cb, 
  hdrhmp_ind_cb,
  hdrscp_ind_cb,
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrpac_ind_cb,
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrcmac_ind_cb,
  NULL 
};

/* 14 HDRIND_IDLE_CHANGE_HDR_SYSTEMS */
static const hdrind_cb_list_type hdrind_idle_change_hdr_systems = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 15 HDRIND_IDLE_EXIT_HDR_MODE */
static const hdrind_cb_list_type hdrind_idle_exit_hdr_mode = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 16 HDRIND_CONNECTED_CONNECTION_CLOSED */
static const hdrind_cb_list_type hdrind_connected_connection_closed = 
{
  hdrcmac_ind_cb,
  hdridle_ind_cb, 
  hdrovhd_ind_cb, 
  hdrsmp_ind_cb, 
#ifdef FEATURE_HDR_QOS
  hdrmrlpresv_ind_cb,
#endif
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrpac_ind_cb,
#endif /* FEATURE_LTE_TO_HDR_OH */
#ifdef FEATURE_HDR_MODEM_API
  hdrlmac_ind_cb,
#endif /* FEATURE_HDR_MODEM_API */
  NULL 
};
  /* Connection Closed must go to SCP before SMP.  Why? */

/* 17 HDRIND_RTCMAC_SHUTDOWN_COMPLETE, Was HDRIND_CONNECTED_CLOSE_MSG_SENT */
static const hdrind_cb_list_type hdrind_rtcmac_shutdown_complete = 
{
  hdrlmac_ind_cb,
  hdralmp_ind_cb, 
  NULL 
};

/* 18 HDRIND_RUP_CONN_SETUP_FAILED */
static const hdrind_cb_list_type hdrind_rup_conn_setup_failed = 
{
  hdridle_ind_cb, 
  NULL 
};

/* 19 HDRIND_RUP_FREQ_CHANGE_INITIATED */
static const hdrind_cb_list_type hdrind_rup_freq_change_initiated = 
{
  hdrovhd_ind_cb, 
  NULL 
};
  
/* 20 HDRIND_RUP_ASSIGNMENT_REJECTED */
static const hdrind_cb_list_type hdrind_rup_assignment_rejected = 
{
  hdralmp_ind_cb, 
  hdrphy_ind_cb,
  NULL 
};


/* 21 HDRIND_CCMAC_MOBILE_DIRECTED_PKT */
static const hdrind_cb_list_type hdrind_ccmac_mobile_directed_pkt = 
{
  hdrsmp_ind_cb, 
  NULL 
};
  
/* 22 HDRIND_CCMAC_SUPERVISION_FAILED */
static const hdrind_cb_list_type hdrind_ccmac_supervision_failed = 
{
  hdralmp_ind_cb, 
  hdrphy_ind_cb,
  NULL 
};

/* 23 HDRIND_CCMAC_END_OF_SYNC_CAPSULE *** obsolete *** */

/* 24 HDRIND_ACMAC_TX_STARTED */
static const hdrind_cb_list_type hdrind_acmac_tx_started = 
{
  hdridle_ind_cb,
  NULL 
};

/* 25 HDRIND_ACMAC_TX_ENDED */
static const hdrind_cb_list_type hdrind_acmac_tx_ended = 
{
  hdrlmac_ind_cb, 
  hdridle_ind_cb, 
  hdrhmp_ind_cb, 
  NULL 
};

/* 26 HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL */
static const hdrind_cb_list_type hdrind_acmac_transmission_successful = 
{
  hdrlmac_ind_cb, 
  hdrslp_ind_cb,
#ifdef FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
  hdrsmp_ind_cb,
#endif /* FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY*/
  hdrrup_ind_cb, 
  hdridle_ind_cb, 
  NULL 
};

/* 27 HDRIND_ACMAC_TRANSMISSION_FAILED */
static const hdrind_cb_list_type hdrind_acmac_transmission_failed = 
{
  hdrslp_ind_cb, 
  hdralmp_ind_cb, 
  NULL 
};
  /* SLP calls msg callbacks based on this indication.  If ALMP gets
   * it first and gives REACQUIRING, SLP would call callbacks based on 
   * that which would give the wrong failure reason.
   */

/* 28 HDRIND_ACMAC_TRANSMISSION_ABORTED */
static const hdrind_cb_list_type hdrind_acmac_transmission_aborted = 
{
  hdrslp_ind_cb, 
  NULL 
};

/* 29 HDRIND_ACMAC_TRANSMISSION_STOPPED */
static const hdrind_cb_list_type hdrind_acmac_transmission_stopped = 
{
  hdrslp_ind_cb, 
  NULL 
};

/* 30 HDRIND_RTCMAC_DRC_TX_STOPPED, was HDRIND_FTCMAC_STOP_TX */
static const hdrind_cb_list_type hdrind_rtcmac_drc_tx_stopped = 
{
  hdrlmac_ind_cb, 
  hdralmp_ind_cb, 
  NULL 
};
  
/* 31 HDRIND_RTCMAC_DRC_TX_RESTARTED, was HDRIND_FTCMAC_RESTART_TX */
static const hdrind_cb_list_type hdrind_rtcmac_drc_tx_restarted = 
{
  hdrlmac_ind_cb, 
  NULL 
};

/* 32 HDRIND_FTCMAC_SUPERVISION_FAILED */
static const hdrind_cb_list_type hdrind_ftcmac_supervision_failed = 
{
  hdrrmac_ind_cb,
  hdralmp_ind_cb, 
  hdrphy_ind_cb,
  NULL 
};

/* 33 HDRIND_RTCMAC_LINK_ACQUIRED */
static const hdrind_cb_list_type hdrind_rtcmac_link_acquired = 
{
  hdrlmac_ind_cb, 
  hdridle_ind_cb, 
  hdrrup_ind_cb, 
  hdrhmp_ind_cb, 
  NULL 
};
  /* Idle gets LinkAcquired before RUP because otherwise
   * RUP tries to send TC Complete before ALMP is in 
   * connected state.  Idle must change state before 
   * giving connection opened as well.
   */

/* 34 HDRIND_RTCMAC_TX_STOPPED */
static const hdrind_cb_list_type hdrind_rtcmac_tx_stopped = 
{
  hdrlmac_ind_cb, 
  NULL 
};


/* 35 HDRIND_LMAC_ACQUIRED */
static const hdrind_cb_list_type hdrind_lmac_acquired = 
{
  hdrinit_ind_cb, 
  NULL 
};

/* 36 HDRIND_LMAC_ACQUISITION_FAILED */
static const hdrind_cb_list_type hdrind_lmac_acquisition_failed = 
{
  hdrinit_ind_cb, 
  NULL 
};

/* 37 HDRIND_LMAC_NETWORK_LOST */
static const hdrind_cb_list_type hdrind_lmac_network_lost = 
{
  hdralmp_ind_cb, 
#ifdef FEATURE_IRAT_PILOT_MEAS
  hdrrup_ind_cb,
#endif /* FEATURE_IRAT_PILOT_MEAS */
  NULL 
};

/* 38 HDRIND_LMAC_IDLE_HANDOFF */
static const hdrind_cb_list_type hdrind_lmac_idle_handoff = 
{
  hdrovhd_ind_cb, 
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
#endif
  NULL 
};

/* 39 HDRIND_LMAC_AWAKE */
static const hdrind_cb_list_type hdrind_lmac_awake = 
{
#ifdef FEATURE_HDR_BCMCS
  hdrbcmac_ind_cb,
#endif /* FEATURE_HDR_BCMCS */
  hdridle_ind_cb, 
  hdrcmac_ind_cb,  /* CCMAC needs to update its internal configuration
                    * when the physical layer wakes up.
                    */ 
  NULL 
};

/* 40 HDRIND_LMAC_ACCESS_MODE_ENTERED */
static const hdrind_cb_list_type hdrind_lmac_access_mode_entered = 
{
  hdrhmp_ind_cb, 
  hdramac_ind_cb,
  NULL 
};

/* 41 HDRIND_LMAC_AHO_REQUEST */
static const hdrind_cb_list_type hdrind_lmac_aho_request = 
{
  hdrovhd_ind_cb, 
  NULL 
};

/* 42 HDRIND_LMAC_AHO_COMPLETE */
static const hdrind_cb_list_type hdrind_lmac_aho_complete = 
{
  hdrovhd_ind_cb, 
  NULL 
};

/* 43 HDRIND_LMAC_FREQ_CHANGE_COMPLETE */
static const hdrind_cb_list_type hdrind_lmac_freq_change_complete = 
{
  hdrovhd_ind_cb, 
  NULL 
};

/* 44 HDRIND_LMAC_SEND_PILOT_REPORT */
static const hdrind_cb_list_type hdrind_lmac_send_pilot_report = 
{
  hdrrup_ind_cb, 
  NULL 
};

/* 45 HDRIND_LMAC_CHAN_MEAS_COMPLETE */
static const hdrind_cb_list_type hdrind_lmac_chan_meas_complete = 
{
  hdrinit_ind_cb, 
  NULL 
};

/* 46 HDRIND_SMP_SESSION_CLOSED */
static const hdrind_cb_list_type hdrind_smp_session_closed = 
{
  hdrslp_ind_cb, 
  hdridle_ind_cb, 
  hdrkep_ind_cb,
#ifdef FEATURE_LTE_TO_HDR_OH
  hdrsap_ind_cb,
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrcon_ind_cb,
  hdrhmp_ind_cb, 
  NULL 
};
  /* HMP tells ALMP to reacquire, which deactivates some protocols,
   * so it is safest to give SESSION_CLOSED to HMP last, although
   * it currently does not matter what order it is given in.
   */


/* 47 HDRIND_SMP_BOOT_COMPLETED */
static const hdrind_cb_list_type hdrind_smp_boot_completed = 
{
  hdrslp_ind_cb, 
  NULL 
};

/* 48 HDRIND_SMP_SESSION_OPENED */
static const hdrind_cb_list_type hdrind_smp_session_opened = 
{
  hdridle_ind_cb, 
  NULL 
};

/* 49 HDRIND_AMP_OPENED */
static const hdrind_cb_list_type hdrind_amp_opened = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 50 HDRIND_AMP_FAILED_REQUEST_TIMEOUT */
static const hdrind_cb_list_type hdrind_amp_failed_request_timeout = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 51 HDRIND_AMP_FAILED_NEW_NETWORK */
static const hdrind_cb_list_type hdrind_amp_failed_new_network = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 52 HDRIND_AMP_UATI_ASSIGNED */
static const hdrind_cb_list_type hdrind_amp_uati_assigned = 
{
  NULL 
};

/* 53 HDRIND_AMP_UATI_RELEASED */
static const hdrind_cb_list_type hdrind_amp_uati_released = 
{
  NULL 
};

/* 54 HDRIND_SCP_RECONFIGURED *** obsolete *** */

/* 55 HDRIND_SCP_PREV_SESSION_OPENED *** obsolete *** */

/* 56 HDRIND_SCP_FAILED */
static const hdrind_cb_list_type hdrind_scp_failed = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 57 HDRIND_IDLE_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_idle_prot_neg_failed = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 58 HDRIND_IDLE_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_idle_config_complete = 
{
  hdrscp_ind_cb, 
  NULL 
};

/* 59 HDRIND_IDLE_CONFIG_REQ */
static const hdrind_cb_list_type hdrind_idle_config_req = 
{
  hdrscp_ind_cb, 
  NULL 
};

/* 60 HDRIND_ALMP_RETURNING_TO_IDLE */
static const hdrind_cb_list_type hdrind_almp_returning_to_idle = 
{
  hdrslp_ind_cb, 
  hdrhmp_ind_cb, 
#ifdef FEATURE_HDR_BCMCS
  hdrbccp_ind_cb,
#endif /* FEATURE_HDR_BCMCS */
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif
  NULL 
};

/* 61 HDRIND_CDMA_IMSI_CHANGED */
static const hdrind_cb_list_type hdrind_cdma_imsi_changed = 
{
  NULL 
};

/* 62 HDRIND_LUP_PROT_NEG_FAILED *** obsolete *** */
/* 63 HDRIND_LUP_CONFIG_COMPLETE *** obsolete *** */
/* 64 HDRIND_LUP_CONFIG_REQ *** obsolete *** */

/* 65 HDRIND_STREAM_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_stream_prot_neg_failed = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 66 HDRIND_STREAM_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_stream_config_complete = 
{
  hdrpac_ind_cb, /* hdrpac needs the notification first here because
                  * we might need to do further negotiation for the apps.
                  */
  hdrscp_ind_cb, 
  NULL 
};

/* 67 HDRIND_STREAM_CONFIG_REQ *** obsolete *** */

/* 68 HDRIND_FCP_DATA_READY_RCVD */
static const hdrind_cb_list_type hdrind_fcp_data_ready_rcvd = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 69 HDRIND_RUP_CONNECTION_INITIATED */
static const hdrind_cb_list_type hdrind_rup_connection_initiated = 
{
  hdrslp_ind_cb, 
  hdrovhd_ind_cb, 
  hdrhmp_ind_cb,
  hdrpac_ind_cb, 
  NULL 
};


/* 70 HDRIND_KEP_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_kep_prot_neg_failed = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 71 HDRIND_FCP_ALL_STREAMS_CLOSED *** obsolete *** */

/* 72 HDRIND_CCMAC_SLEEP_CAPSULE_DONE *** obsolete *** */

/* 73 HDRIND_LMAC_ACCESS_ENTRY_HANDOFF */
static const hdrind_cb_list_type hdrind_lmac_access_entry_handoff =
{
  hdrslp_ind_cb,
  hdrhmp_ind_cb,
  NULL
};

/* 74 HDRIND_LMAC_IDLE_SUSPEND */
static const hdrind_cb_list_type hdrind_lmac_idle_suspend =
{
  hdridle_ind_cb, 
  NULL
};

/* 75 HDRIND_LMAC_IDLE_RESUME */
static const hdrind_cb_list_type hdrind_lmac_idle_resume =
{
  hdridle_ind_cb, 
  NULL
};

/* 76 HDRIND_LMAC_CC_MISSED */
static const hdrind_cb_list_type hdrind_lmac_cc_missed =
{
  hdrlmac_ind_cb, 
  hdridle_ind_cb,
  NULL
};

/* 77 HDRIND_LMAC_RESELECT */
static const hdrind_cb_list_type hdrind_lmac_reselect = 
{
  hdralmp_ind_cb,
  NULL
};

/* 78 HDRIND_LUP_ALN_TRIGGER */
static const hdrind_cb_list_type hdrind_lup_aln_trigger = 
{
  hdrpac_ind_cb,
  NULL
};

/* 79 HDRIND_LMAC_HDR_USABLE */
static const hdrind_cb_list_type hdrind_lmac_hdr_usable = 
{
  hdralmp_ind_cb,
  NULL
};

/* 80 HDRIND_LMAC_DEACTIVATE_DONE */
static const hdrind_cb_list_type hdrind_lmac_deactivate_done = 
{
  hdralmp_ind_cb,
  NULL
};

/* 81 HDRIND_AMP_FAILED_NEW_ESN */
static const hdrind_cb_list_type hdrind_amp_failed_new_esn = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 82 HDRIND_SCM_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_scm_prot_neg_failed = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 83 HDRIND_SCM_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_scm_config_complete = 
{
  hdrscp_ind_cb, 
  NULL 
};

/* 84 HDRIND_SCM_CONFIG_REQ *** obsolete *** */

/* 85 HDRIND_BCMCS_FREQ_CHANGE_INITIATED - OBSOLETE */

/* 86 HDRIND_SCM_RMAC_GAUP */
static const hdrind_cb_list_type hdrind_scm_rmac_gaup = 
{
  hdrrmac_ind_cb, 
  NULL 
};

/* 87 HDRIND_SCM_MRLP_GAUP */
static const hdrind_cb_list_type hdrind_scm_mrlp_gaup = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 88 HDRIND_RUP_CHANGE_HDR_SYSTEMS */
static const hdrind_cb_list_type hdrind_rup_change_hdr_systems = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 89 HDRIND_RUP_EXIT_HDR_MODE */
static const hdrind_cb_list_type hdrind_rup_exit_hdr_mode = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 90 HDRIND_IDLE_CONNECTION_ABORTED */
static const hdrind_cb_list_type hdrind_idle_connection_aborted = 
{
  hdrslp_ind_cb, 
  hdrhmp_ind_cb,
  hdrcmac_ind_cb,
  NULL 
};

/* 91 HDRIND_LMAC_TC_SUSPEND */
static const hdrind_cb_list_type hdrind_lmac_tc_suspend = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 92 HDRIND_LMAC_TC_RESUME */
static const hdrind_cb_list_type hdrind_lmac_tc_resume = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 93 HDRIND_CCMAC_INTERRUPTED */
static const hdrind_cb_list_type hdrind_ccmac_interrupted =
{
  hdrslp_ind_cb,
  NULL
};

/* 94 HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_mrlp_at_gaup_accepted = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 95 HDRIND_SCM_MRLP_AT_GAUP_REJECTED */
static const hdrind_cb_list_type hdrind_scm_mrlp_at_gaup_rejected = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 96 HDRIND_SCM_MRLP_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_mrlp_at_gaup_failed = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 97 HDRIND_OVHD_BC_UPDATED  *** obsolete *** */

/* 98 HDRIND_SCM_FMAC_GAUP */
static const hdrind_cb_list_type hdrind_scm_fmac_gaup =
{
  hdrfmac_ind_cb,
  NULL
};

/* 99 HDRIND_SCM_SESSION_CHANGED 
   SCP should get the Session Changed indication before every other protocol. 
   The reason being, the indication processing of all protocols are based on 
   the subtype information that SCP gives to each protocol. So it needs to be
   updated first */
static const hdrind_cb_list_type hdrind_scm_session_changed = 
{
  hdrscp_ind_cb, 
  hdrrmac_ind_cb,
  hdrfmac_ind_cb,
  hdramac_ind_cb,
  hdrcmac_ind_cb,
  hdrsmp_ind_cb,
  hdrkep_ind_cb,
  hdrauth_ind_cb,
  hdridle_ind_cb,
  hdrrup_ind_cb,  
  hdrstream_ind_cb,
  hdrpacc_ind_cb,
#ifdef FEATURE_HDR_BCMCS
  hdrbccp_ind_cb,
#endif /* FEATURE_HDR_BCMCS */
  hdrovhd_ind_cb,
  hdrhmp_ind_cb,  
  hdrpac_ind_cb,
  hdrlup_ind_cb, 
  hdrcsna_ind_cb,
  hdrphy_ind_cb,
  NULL
};

/* 100 HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_rmac_at_gaup_accepted =
{
  hdrrmac_ind_cb, 
  hdrpac_ind_cb,
  NULL
};

/* 101 HDRIND_SCM_RMAC_AT_GAUP_REJECTED */
static const hdrind_cb_list_type hdrind_scm_rmac_at_gaup_rejected =
{
  hdrpac_ind_cb,
  NULL
};

/* 102 HDRIND_SCM_RMAC_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_rmac_at_gaup_failed =
{
  hdrpac_ind_cb,
  NULL
};

/* 103 HDRIND_ALMP_CONNECTION_CLOSED */
static const hdrind_cb_list_type hdrind_almp_connection_closed = 
{
  hdrscp_ind_cb,
  hdrsmp_ind_cb, 
  hdrhmp_ind_cb, 
  hdrphy_ind_cb,
  NULL 
};

/* 104 HDRIND_RMAC3_FLOW_ACTIVATED - obsoleted */

/* 105 HDRIND_RMAC3_FLOW_DEACTIVATED - obsoleted */

/* 106 HDRIND_RMAC3_FLOW_MAPPING_CHANGED - obsoleted */

/* 107 HDRIND_IDLE_MONITOR_CONNECTION_FAILED */
static const hdrind_cb_list_type hdrind_idle_monitor_connection_failed = 
{
#ifdef FEATURE_HDR_BCMCS
  hdrbccp_ind_cb,
#endif /* FEATURE_HDR_BCMCS */
  NULL
};

/* 108 HDRIND_GMMCDP_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_gmmcdp_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 109 HDRIND_GMMCD_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_gmmcd_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 110 HDRIND_SCM_AMAC_GAUP */
static const hdrind_cb_list_type hdrind_scm_amac_gaup = 
{
  hdramac_ind_cb,
  NULL
};

/* 111 HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_amac_at_gaup_accepted = 
{
  hdramac_ind_cb,
  NULL
};

/* 112 HDRIND_SCM_AMAC_AT_GAUP_REJECTED */
static const hdrind_cb_list_type hdrind_scm_amac_at_gaup_rejected = 
{ 
  hdramac_ind_cb,
  NULL
};

/* 113 HDRIND_SCM_AMAC_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_amac_at_gaup_failed = 
{
  hdramac_ind_cb,
  NULL
};

/* 114 HDRIND_RUP_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_rup_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 115 HDRIND_RUP_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_rup_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 116 HDRIND_RMAC_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_rmac_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 117 HDRIND_RMAC_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_rmac_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 118 HDRIND_SCM_AMP_GAUP */
static const hdrind_cb_list_type hdrind_scm_amp_gaup = 
{
  hdramp_ind_cb,
  NULL
};

/* 119 HDRIND_SCP_AN_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scp_an_gaup_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 120 HDRIND_IDLE_AN_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_idle_an_gaup_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 121 HDRIND_RUP_AN_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_rup_an_gaup_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 122 HDRIND_PACC_AN_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_pacc_an_gaup_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 123 HDRIND_SCM_AN_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_an_gaup_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 124 HDRIND_SCP_CONFIG_UNLOCKED */
static const hdrind_cb_list_type hdrind_scp_config_unlocked = 
{
  hdrhmp_ind_cb,
  NULL
};

#ifdef FEATURE_HDR_BCMCS
/* 125 HDRIND_BCMCS_SLEEP_OK */
static const hdrind_cb_list_type hdrind_bcmcs_sleep_ok = 
{
  hdridle_ind_cb,
  NULL
};
#endif /* FEATURE_HDR_BCMCS */

/* 126 HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI */
static const hdrind_cb_list_type hdrind_amp_failed_not_maintaining_uati = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 127 HDRIND_SCP_NO_PREV_SESSION */
static const hdrind_cb_list_type hdrind_scp_no_prev_session = 
{
  hdrpac_ind_cb,
  NULL
};

/* 128 HDRIND_ACMAC_READY */
static const hdrind_cb_list_type hdrind_acmac_ready = 
{
  hdrlmac_ind_cb,
  NULL
};

/* 129 HDRIND_ACMAC_TUNE_AWAY_TIME_OK */
static const hdrind_cb_list_type hdrind_acmac_tune_away_time_ok = 
{
  hdrlmac_ind_cb,
  NULL
};

/* 130 HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_idle_at_gaup_accepted = 
{
  hdridle_ind_cb,
  hdrpac_ind_cb,
  NULL
};

/* 131 HDRIND_SCM_IDLE_AT_GAUP_REJECTED */ 
static const hdrind_cb_list_type hdrind_scm_idle_at_gaup_rejected = 
{
#ifdef FEATURE_HDR_REVC
  hdridle_ind_cb,
#endif
  hdrpac_ind_cb,
  NULL
};

/* 132 HDRIND_SCM_IDLE_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_idle_at_gaup_failed = 
{
#ifdef FEATURE_HDR_REVC
  hdridle_ind_cb,
#endif
  hdrpac_ind_cb,
  NULL
};

#ifdef FEATURE_HDR_DDARF
/* 133 HDRIND_IDLE_NETWORK_LOST */
static const hdrind_cb_list_type hdrind_idle_network_lost = 
{
  hdralmp_ind_cb,
  NULL
};
#endif /* FEATURE_HDR_DDARF */

/* 134 HDRIND_SCM_CSNA_AN_GAUPED */
static const hdrind_cb_list_type hdrind_scm_csna_an_gauped = 
{
  hdrcsna_ind_cb,
  NULL
};


/* 135 HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_scm_csna_at_config_complete = 
{
  hdrscp_ind_cb,
  hdrcsna_ind_cb,
  NULL
};

/* 136 HDRIND_CSNA_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_csna_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  hdrcsna_ind_cb,
  NULL
};

/* 137 HDRIND_CSNA_3G1X_PARM_RECEIVED */
static const hdrind_cb_list_type hdrind_csna_3g1x_parm_received = 
{
  NULL
};

/* 138 HDRIND_CSNA_3G1X_PARM_RESET_RECEIVED */
static const hdrind_cb_list_type hdrind_csna_3g1x_parm_reset_received = 
{
  NULL
};

#ifdef FEATURE_HDR_QOS
/* 139 HDRIND_RUP_ASSIGNMENT_FAILED */
static const hdrind_cb_list_type hdrind_rup_assignment_failed = 
{
  hdrmrlpresv_ind_cb,
  NULL
};
#endif /* FEATURE_HDR_QOS */ 

/* 140 HDRIND_SCM_SCP_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_scp_at_gaup_accepted = 
{
  hdrscp_ind_cb,
  NULL
};

/* 141 HDRIND_SCM_SCP_AT_GAUP_REJECTED */ 
static const hdrind_cb_list_type hdrind_scm_scp_at_gaup_rejected = 
{
  hdrscp_ind_cb,
  NULL
};

/* 142 HDRIND_SCM_SCP_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_scp_at_gaup_failed = 
{
  hdrscp_ind_cb,
  NULL
};

/* 143 HDRIND_AMP_OPENED_PREV_SESSION */
static const hdrind_cb_list_type hdrind_amp_opened_prev_session = 
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 144 HDRIND_CSNA_TX_MSG_STATUS_RECEIVED*/
static const hdrind_cb_list_type hdrind_csna_tx_msg_status_received =
{
  hdrcsna_ind_cb,
  NULL
};

/* 145 HDRIND_LMAC_RF_CONDITION_CHANGED */
static const hdrind_cb_list_type hdrind_lmac_rf_condition_changed =
{
  hdrpac_ind_cb,
  NULL
};

/* 146 HDRIND_RUP_PILOT_RPT_RECEIVED */
static const hdrind_cb_list_type hdrind_rup_pilot_rpt_received =
{
  hdrrup_ind_cb,
  NULL
};

#ifdef FEATURE_EIDLE_SCI
/* 147 HDRIND_IDLE_AN_GAUP_SLOTTEDMODE */
static const hdrind_cb_list_type hdrind_idle_an_gaup_slottedmode = 
{
  hdrpac_ind_cb,
  NULL
};
#endif /* FEATURE_EIDLE_SCI */

/* 148 HDRIND_AMP_FAILED_NEW_NAI */
static const hdrind_cb_list_type hdrind_amp_failed_new_nai =
{
  hdrsmp_ind_cb, 
  NULL 
};

/* 149 HDRIND_OUTAGE_IMMINENT */
static const hdrind_cb_list_type hdrind_outage_imminent = 
{
  hdrpac_ind_cb,
  NULL
};

/* 150 HDRIND_AMAC_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_amac_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 151 HDRIND_AMAC_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_amac_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 152 HDRIND_RSV1 - reserved for HDRIND_BCCP_CONFIG_COMPLETE */

/* 153 HDRIND_RSV2 - reserved for HDRIND_BCCP_PROT_NEG_FAILED */

/* 154 HDRIND_CCMAC_SCC_PKT_RXED */
static const hdrind_cb_list_type hdrind_ccmac_scc_pkt_rxed = 
{
  hdrlmac_ind_cb,
  hdridle_ind_cb,
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb,
#endif
  NULL
};

#ifdef FEATURE_IRAT_PILOT_MEAS
/* 155 HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND */
static const hdrind_cb_list_type hdrind_lmac_irat_plt_meas_done_ind = 
{
  hdrrup_ind_cb,
  hdridle_ind_cb,
  NULL
};
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_EHRPD
/* 156 HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED */
static const hdrind_cb_list_type hdrind_scp_ehrpd_availability_changed = 
{
  hdrsmp_ind_cb,
  NULL
};
#endif /* FEATURE_EHRPD */

/* 157 HDRIND_FMAC_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_fmac_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 158 HDRIND_FMAC_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_fmac_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 159 HDRIND_LMAC_SCHEDULER_GROUP_CHANGED */
static const hdrind_cb_list_type hdrind_lmac_scheduler_group_changed = 
{
  hdrrlp_ind_cb,
  NULL
};

/* 160 HDRIND_PHY_CONFIG_COMPLETE */
static const hdrind_cb_list_type hdrind_phy_config_complete = 
{
  hdrscp_ind_cb,
  NULL
};

/* 161 HDRIND_PHY_PROT_NEG_FAILED */
static const hdrind_cb_list_type hdrind_phy_prot_neg_failed = 
{
  hdrsmp_ind_cb,
  NULL
};

/* 162 HDRIND_RTC_RESTART_TIMER_EXPIRED */
static const hdrind_cb_list_type hdrind_rtc_restart_timer_expired = 
{
  hdrrmac_ind_cb,
  NULL
};

/* 163 HDRIND_RMAC_FLOW_ACTIVATED */
static const hdrind_cb_list_type hdrind_rmac_flow_activated = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 164 HDRIND_RMAC_FLOW_DEACTIVATED */
static const hdrind_cb_list_type hdrind_rmac_flow_deactivated = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 165 HDRIND_RMAC_FLOW_MAPPING_CHANGED */
static const hdrind_cb_list_type hdrind_rmac_flow_mapping_changed =
{
  hdrpac_ind_cb,
  NULL
};

/* 166 HDRIND_LMAC_CARRIER_MAP_CHANGED */
static const hdrind_cb_list_type hdrind_lmac_carrier_map_changed =
{
#ifdef FEATURE_HDR_IS890
  hdrftap_ind_cb, 
  hdrrtap_ind_cb, 
#endif
  NULL
};

/* 167 HDRIND_SCM_PHY_AN_GAUP */
static const hdrind_cb_list_type hdrind_scm_phy_an_gaup = 
{
  hdrphy_ind_cb,
  NULL
};

/* 168 HDRIND_LMAC_TCA_FAILED */
static const hdrind_cb_list_type hdrind_lmac_tca_failed = 
{
#if defined (FEATURE_SVDO) || defined (FEATURE_LTE_TO_HDR_OH)
  hdrrup_ind_cb,
#endif /* FEATURE_SVDO || FEATURE_LTE_TO_HDR_OH */
  NULL
};

/* 169 HDRIND_LMAC_CONN_ERROR */
static const hdrind_cb_list_type hdrind_lmac_conn_error = 
{
#ifdef FEATURE_SVDO
  hdralmp_ind_cb,
#endif /* FEATURE_SVDO */
  NULL
};

#ifdef FEATURE_LTE_TO_HDR
/* 170 HDRIND_LMAC_TIME_SYNC_DONE */
static const hdrind_cb_list_type hdrind_lmac_time_sync_done = 
{
  hdrinit_ind_cb,
  hdrcmac_ind_cb,
  NULL
};

/* 171 HDRIND_LMAC_FAST_ACQ_DONE */
static const hdrind_cb_list_type hdrind_lmac_fast_acq_done = 
{
  hdrinit_ind_cb,
  NULL
};
#endif /* FEATURE_LTE_TO_HDR */

/* 172 HDRIND_LMAC_TIMING_INFO */
static const hdrind_cb_list_type hdrind_lmac_timing_info = 
{
  hdrinit_ind_cb,
  NULL
};

#ifdef FEATURE_HDR_REVC
/* 173 HDRIND_SCM_FMAC_AT_GAUP_ACCEPTED */
static const hdrind_cb_list_type hdrind_scm_fmac_at_gaup_accepted = 
{
  hdrfmac_ind_cb,
  NULL
};

/* 174 HDRIND_SCM_FMAC_AT_GAUP_REJECTED */
static const hdrind_cb_list_type hdrind_scm_fmac_at_gaup_rejected = 
{
  hdrfmac_ind_cb,
  NULL
};

/* 175 HDRIND_SCM_FMAC_AT_GAUP_FAILED */
static const hdrind_cb_list_type hdrind_scm_fmac_at_gaup_failed = 
{
  hdrfmac_ind_cb,
  NULL
};
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_TO_LTE
/* 176 HDRIND_LMAC_IRAT_RESELECT */
static const hdrind_cb_list_type hdrind_lmac_irat_reselect = 
{
  hdralmp_ind_cb,
  NULL
};
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REVC
/* 177 HDRIND_LMAC_ASET_CHANGED */
static const hdrind_cb_list_type hdrind_lmac_aset_changed = 
{
  hdrovhd_ind_cb,
  NULL
};

/* 178 HDRIND_OVHD_NLB_CACHE_UPDATED */
static const hdrind_cb_list_type hdrind_ovhd_nlb_cache_updated = 
{
  hdrfmac_ind_cb,
  NULL
};

/* 179 HDRIND_QOS_FLOW_STATUS_CHANGED */
static const hdrind_cb_list_type hdrind_mc_qos_flow_status_changed = 
{
  hdrfmac_ind_cb,
  NULL
};

#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
/* 180 HDRIND_OVHD_PREREG_INFO_UPDATED */
static const hdrind_cb_list_type hdrind_ovhd_prereg_info_updated = 
{
  hdrsmp_ind_cb,
  hdrhmp_ind_cb,
  hdramp_ind_cb,
  hdrscp_ind_cb,
  hdrsap_ind_cb,
  hdrpac_ind_cb,
  NULL
};

/* 181 HDRIND_SAP_CONNECTION_OPENED */
static const hdrind_cb_list_type hdrind_sap_connection_opened = 
{
  hdridle_ind_cb,
  NULL
};

/* 182 HDRIND_SAP_CONNECTION_INITIATED */
static const hdrind_cb_list_type hdrind_sap_connection_initiated = 
{
  hdrrup_ind_cb,
  NULL
};

/* 183 HDRIND_SAP_CONNECTION_CLOSED */
static const hdrind_cb_list_type hdrind_sap_connection_closed = 
{
  hdrcon_ind_cb,
  hdralmp_ind_cb,
  hdrsaptx_ind_cb,
  NULL
};

/* 184 HDRIND_SAP_CONNECTION_FAILED */
static const hdrind_cb_list_type hdrind_sap_connection_failed = 
{
  hdridle_ind_cb,
  NULL
};

/* 185 HDRIND_SAP_IDLE_HANDOFF */
static const hdrind_cb_list_type hdrind_sap_idle_handoff = 
{
  hdramp_ind_cb,
  NULL
};

/* 186 HDRIND_SAP_LINK_ACQUIRED */
static const hdrind_cb_list_type hdrind_sap_link_acquired = 
{
  hdrrmac_ind_cb,
  NULL
};

/* 187 HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED */
static const hdrind_cb_list_type hdrind_sap_alt_link_comp_cb_received = 
{
  hdrhmp_ind_cb,
  NULL
};

/* 188 HDRIND_SAPTX_TRANSMISSION_FAILED_AC */
static const hdrind_cb_list_type hdrind_saptx_transmission_failed_ac = 
{
  hdrslp_ind_cb,
  hdrsap_ind_cb,
  NULL
};

/* 189 HDRIND_SAPTX_TRANSMISSION_CNF_AC */
static const hdrind_cb_list_type hdrind_saptx_transmission_cnf_ac = 
{
  hdrsap_ind_cb,
  NULL
};

/* 190 HDRIND_SAP_BACKOFF_TIMER_EXPIRED */
static const hdrind_cb_list_type hdrind_sap_backoff_timer_expired = 
{
  hdramp_ind_cb,
  hdrscp_ind_cb,
  NULL
};

/* 191 HDRIND_SAP_BACKOFF_TIMER_STOPPED */
static const hdrind_cb_list_type hdrind_sap_backoff_timer_stopped = 
{
  hdramp_ind_cb,
  hdrscp_ind_cb,
  NULL
};

/* 192 HDRIND_AMP_PREREG_ZONE_CHANGED */
static const hdrind_cb_list_type hdrind_amp_prereg_zone_changed = 
{
  hdrsap_ind_cb,
  NULL
};

/* 193 HDRIND_SAP_TUNNELMODE_STATUS_CHANGED */
static const hdrind_cb_list_type hdrind_sap_tunnelmode_status_changed = 
{
  hdrpac_ind_cb,
  NULL
};

/* 194 HDRIND_SAP_CONNECTION_ALREADY_OPENED */
static const hdrind_cb_list_type hdrind_sap_connection_already_opened = 
{
  hdridle_ind_cb,
  NULL
};

/* 195 HDRIND_IDLE_CONNECTION_ALREADY_OPENED */
static const hdrind_cb_list_type hdrind_idle_connection_already_opened = 
{
  hdrpac_ind_cb,
  NULL
};

/* 196 HDRIND_AMP_IRMI_ACCEPTED */
static const hdrind_cb_list_type hdrind_amp_irmi_accepted = 
{
  hdrpac_ind_cb,
  NULL
};

/* 197 HDRIND_AMP_IRMI_FAILED */
static const hdrind_cb_list_type hdrind_amp_irmi_failed = 
{
  hdrpac_ind_cb,
  NULL
};

/* 198 HDRIND_OVHD_LTE_CELL_CHANGED */
static const hdrind_cb_list_type hdrind_ovhd_lte_cell_changed = 
{
  hdramp_ind_cb,
  NULL
};

/* 199 HDRIND_IDLE_CONNECTION_NOT_ALLOWED */
static const hdrind_cb_list_type hdrind_idle_connection_not_allowed = 
{
  hdrpac_ind_cb,
  NULL
};

/* 200 HDRIND_OVHD_COLOR_CODE_CHANGED */
static const hdrind_cb_list_type hdrind_ovhd_color_code_changed = 
{
  hdrpac_ind_cb,
  NULL
};
#endif /* FEATURE_LTE_TO_HDR_OH */

/* 201 HDRIND_ALMP_RESELECT_EXIT_HDR */
static const hdrind_cb_list_type hdrind_almp_reselect_exit_hdr = 
{
  hdrpac_ind_cb,
  NULL
};

#ifdef FEATURE_LTE_TO_HDR_OH
/* 202 HDRIND_SAP_PREREG_STATUS_UPDATED */
static const hdrind_cb_list_type hdrind_sap_prereg_status_updated = 
{
  hdrsap_ind_cb,
  NULL
};

/* 203 HDRIND_SAP_RETURNING_TO_CLOSE */
static const hdrind_cb_list_type hdrind_sap_returning_to_close = 
{
  hdrslp_ind_cb, 
  hdrhmp_ind_cb, 
  NULL 
};

/* 204 HDRIND_SAP_TRANSMISSION_STOPPED */
static const hdrind_cb_list_type hdrind_sap_transmission_stopped = 
{
  hdrslp_ind_cb,
  NULL 
};

/* 205 HDRIND_SCP_IRAT_CAPABILITY_CHANGED */
static const hdrind_cb_list_type hdrind_scp_irat_capability_changed = 
{
  hdrsmp_ind_cb,
  NULL 
};

/* 206 HDRIND_RUP_ACT_HO_COMPLETED */
static const hdrind_cb_list_type hdrind_rup_act_ho_completed = 
{
  hdrpac_ind_cb,
  NULL 
};
#endif /* FEATURE_LTE_TO_HDR_OH */

/* 207 HDRIND_LMAC_SYNC_SUSPEND */
static const hdrind_cb_list_type hdrind_lmac_sync_suspend =
{
  hdrinit_ind_cb, 
  NULL
};

/* 208 HDRIND_LMAC_SYNC_RESUME */
static const hdrind_cb_list_type hdrind_lmac_sync_resume =
{
  hdrinit_ind_cb, 
  NULL
};

/* 209 HDRIND_LMAC_IDLE_SUSPEND_TO_REACQ */
static const hdrind_cb_list_type hdrind_lmac_idle_suspend_to_reacq =
{
  hdridle_ind_cb, 
  NULL
};

/* 210 HDRIND_LMAC_ORAT_MEAS_RPT */
static const hdrind_cb_list_type hdrind_lmac_orat_meas_rpt = 
{
  hdralmp_ind_cb, 
  NULL 
};

/* 211 HDRIND_LMAC_ORAT_MEAS_FAILED */
static const hdrind_cb_list_type hdrind_lmac_orat_meas_failed = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* 212 HDRIND_LMAC_ORAT_REDIR_SUCCESS */
static const hdrind_cb_list_type hdrind_lmac_orat_redir_success = 
{
  hdrpac_ind_cb, 
  NULL 
};

/* Although this table exceeds 80 characters, it is too useful to have 
 * one line per entry to break the lines. (DNA)
 */

/* The ind_table is an array of pointers to cb_lists */
static hdrind_cb_f_type ** const hdrind_ind_table[] = 
{
  NULL,                               /* 00 obsolete */
  NULL,                               /* 01 obsolete */
  NULL,                               /* 02 obsolete */
  (hdrind_cb_f_type **) hdrind_almp_reacquiring,              /* 03 HDRIND_ALMP_REACQUIRING */
  (hdrind_cb_f_type **) hdrind_almp_exit_hdr_mode,            /* 04 HDRIND_ALMP_EXIT_HDR_MODE */
  (hdrind_cb_f_type **) hdrind_ovhd_info_updated,             /* 05 HDRIND_OVHD_INFO_UPDATED */
  (hdrind_cb_f_type **) hdrind_ovhd_ap_updated,               /* 06 HDRIND_OVHD_AP_UPDATED */
  (hdrind_cb_f_type **) hdrind_ovhd_supervision_failed,       /* 07 HDRIND_OVHD_SUPERVISION_FAILED */
  (hdrind_cb_f_type **) hdrind_ovhd_change_hdr_systems,       /* 08 HDRIND_OVHD_CHANGE_HDR_SYSTEMS */
  (hdrind_cb_f_type **) hdrind_ovhd_exit_hdr_mode,            /* 09 HDRIND_OVHD_EXIT_HDR_MODE */
  (hdrind_cb_f_type **) hdrind_init_network_acquired,         /* 10 HDRIND_INIT_NETWORK_ACQUIRED */
  (hdrind_cb_f_type **) hdrind_init_exit_hdr_mode,            /* 11 HDRIND_INIT_EXIT_HDR_MODE */
  (hdrind_cb_f_type **) hdrind_idle_connection_opened,        /* 12 HDRIND_IDLE_CONNECTION_OPENED */
  (hdrind_cb_f_type **) hdrind_idle_connection_failed,        /* 13 HDRIND_IDLE_CONNECTION_FAILED */
  (hdrind_cb_f_type **) hdrind_idle_change_hdr_systems,       /* 14 HDRIND_IDLE_CHANGE_HDR_SYSTEMS */
  (hdrind_cb_f_type **) hdrind_idle_exit_hdr_mode,            /* 15 HDRIND_IDLE_EXIT_HDR_MODE */
  (hdrind_cb_f_type **) hdrind_connected_connection_closed,   /* 16 HDRIND_CONNECTED_CONNECTION_CLOSED */
  (hdrind_cb_f_type **) hdrind_rtcmac_shutdown_complete,      /* 17 HDRIND_RTCMAC_SHUTDOWN_COMPLETE */
  (hdrind_cb_f_type **) hdrind_rup_conn_setup_failed,         /* 18 HDRIND_RUP_CONN_SETUP_FAILED */
  (hdrind_cb_f_type **) hdrind_rup_freq_change_initiated,     /* 19 HDRIND_RUP_FREQ_CHANGE_INITIATED */
  (hdrind_cb_f_type **) hdrind_rup_assignment_rejected,       /* 20 HDRIND_RUP_ASSIGNMENT_REJECTED */
  (hdrind_cb_f_type **) hdrind_ccmac_mobile_directed_pkt,     /* 21 HDRIND_CCMAC_MOBILE_DIRECTED_PKT */
  (hdrind_cb_f_type **) hdrind_ccmac_supervision_failed,      /* 22 HDRIND_CCMAC_SUPERVISION_FAILED */
  NULL,
  (hdrind_cb_f_type **) hdrind_acmac_tx_started,              /* 24 HDRIND_ACMAC_TX_STARTED */
  (hdrind_cb_f_type **) hdrind_acmac_tx_ended,                /* 25 HDRIND_ACMAC_TX_ENDED */
  (hdrind_cb_f_type **) hdrind_acmac_transmission_successful, /* 26 HDRIND_ACMAC_TRANSMISSION_SUCCESSFUL */
  (hdrind_cb_f_type **) hdrind_acmac_transmission_failed,     /* 27 HDRIND_ACMAC_TRANSMISSION_FAILED */
  (hdrind_cb_f_type **) hdrind_acmac_transmission_aborted,    /* 28 HDRIND_ACMAC_TRANSMISSION_ABORTED */
  (hdrind_cb_f_type **) hdrind_acmac_transmission_stopped,    /* 29 HDRIND_ACMAC_TRANSMISSION_STOPPED */
  (hdrind_cb_f_type **) hdrind_rtcmac_drc_tx_stopped,         /* 30 HDRIND_RTCMAC_DRC_TX_STOPPED */
  (hdrind_cb_f_type **) hdrind_rtcmac_drc_tx_restarted,       /* 31 HDRIND_RTCMAC_DRC_TX_RESTARTED */
  (hdrind_cb_f_type **) hdrind_ftcmac_supervision_failed,     /* 32 HDRIND_FTCMAC_SUPERVISION_FAILED */
  (hdrind_cb_f_type **) hdrind_rtcmac_link_acquired,          /* 33 HDRIND_RTCMAC_LINK_ACQUIRED */
  (hdrind_cb_f_type **) hdrind_rtcmac_tx_stopped,             /* 34 HDRIND_RTCMAC_TX_STOPPED */
  (hdrind_cb_f_type **) hdrind_lmac_acquired,                 /* 35 HDRIND_LMAC_ACQUIRED */
  (hdrind_cb_f_type **) hdrind_lmac_acquisition_failed,       /* 36 HDRIND_LMAC_ACQUISITION_FAILED */
  (hdrind_cb_f_type **) hdrind_lmac_network_lost,             /* 37 HDRIND_LMAC_NETWORK_LOST */
  (hdrind_cb_f_type **) hdrind_lmac_idle_handoff,             /* 38 HDRIND_LMAC_IDLE_HANDOFF */
  (hdrind_cb_f_type **) hdrind_lmac_awake,                    /* 39 HDRIND_LMAC_AWAKE */
  (hdrind_cb_f_type **) hdrind_lmac_access_mode_entered,      /* 40 HDRIND_LMAC_ACCESS_MODE_ENTERED */
  (hdrind_cb_f_type **) hdrind_lmac_aho_request,              /* 41 HDRIND_LMAC_AHO_REQUEST */
  (hdrind_cb_f_type **) hdrind_lmac_aho_complete,             /* 42 HDRIND_LMAC_AHO_COMPLETE */
  (hdrind_cb_f_type **) hdrind_lmac_freq_change_complete,     /* 43 HDRIND_LMAC_FREQ_CHANGE_COMPLETE */
  (hdrind_cb_f_type **) hdrind_lmac_send_pilot_report,        /* 44 HDRIND_LMAC_SEND_PILOT_REPORT */
  (hdrind_cb_f_type **) hdrind_lmac_chan_meas_complete,       /* 45 HDRIND_LMAC_CHAN_MEAS_COMPLETE */
  (hdrind_cb_f_type **) hdrind_smp_session_closed,            /* 46 HDRIND_SMP_SESSION_CLOSED */
  (hdrind_cb_f_type **) hdrind_smp_boot_completed,            /* 47 HDRIND_SMP_BOOT_COMPLETED */
  (hdrind_cb_f_type **) hdrind_smp_session_opened,            /* 48 HDRIND_SMP_SESSION_OPENED */
  (hdrind_cb_f_type **) hdrind_amp_opened,                    /* 49 HDRIND_AMP_OPENED */
  (hdrind_cb_f_type **) hdrind_amp_failed_request_timeout,    /* 50 HDRIND_AMP_FAILED_REQUEST_TIMEOUT */
  (hdrind_cb_f_type **) hdrind_amp_failed_new_network,        /* 51 HDRIND_AMP_FAILED_NEW_NETWORK */
  (hdrind_cb_f_type **) hdrind_amp_uati_assigned,             /* 52 HDRIND_AMP_UATI_ASSIGNED */
  (hdrind_cb_f_type **) hdrind_amp_uati_released,             /* 53 HDRIND_AMP_UATI_RELEASED */
  NULL,                                                       /* 54 *** obsolete *** */
  NULL,                                                       /* 55 *** obsolete *** */
  (hdrind_cb_f_type **) hdrind_scp_failed,                    /* 56 HDRIND_SCP_FAILED */
  (hdrind_cb_f_type **) hdrind_idle_prot_neg_failed,          /* 57 HDRIND_IDLE_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_idle_config_complete,          /* 58 HDRIND_IDLE_CONFIG_COMPLETE */
  (hdrind_cb_f_type **) hdrind_idle_config_req,               /* 59 HDRIND_IDLE_CONFIG_REQ */
  (hdrind_cb_f_type **) hdrind_almp_returning_to_idle,        /* 60 HDRIND_ALMP_RETURNING_TO_IDLE */
  (hdrind_cb_f_type **) hdrind_cdma_imsi_changed,             /* 61 HDRIND_CDMA_IMSI_CHANGED */
  NULL,                                                       /* 62 *** obsolete *** */
  NULL,                                                       /* 63 *** obsolete *** */
  NULL,                                                       /* 64 *** obsolete *** */             
  (hdrind_cb_f_type **) hdrind_stream_prot_neg_failed,        /* 65 HDRIND_STREAM_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_stream_config_complete,        /* 66 HDRIND_STREAM_CONFIG_COMPLETE */
  NULL,                                                       /* 67 *** obsolete *** */
  (hdrind_cb_f_type **) hdrind_fcp_data_ready_rcvd,           /* 68 HDRIND_FCP_DATA_READY_RCVD */
  (hdrind_cb_f_type **) hdrind_rup_connection_initiated,      /* 69 HDRIND_RUP_CONNECTION_INITIATED */
  (hdrind_cb_f_type **) hdrind_kep_prot_neg_failed,           /* 70 HDRIND_KEP_PROT_NEG_FAILED */
  NULL,                                                       /* 71 *** obsolete *** */
  NULL,
  (hdrind_cb_f_type **) hdrind_lmac_access_entry_handoff,     /* 73 HDRIND_LMAC_ACCESS_ENTRY_HANDOFF */
  (hdrind_cb_f_type **) hdrind_lmac_idle_suspend,             /* 74 HDRIND_LMAC_IDLE_SUSPEND */
  (hdrind_cb_f_type **) hdrind_lmac_idle_resume,              /* 75 HDRIND_LMAC_IDLE_RESUME */
  (hdrind_cb_f_type **) hdrind_lmac_cc_missed,                /* 76 HDRIND_LMAC_CC_MISSED */
  (hdrind_cb_f_type **) hdrind_lmac_reselect,                 /* 77 HDRIND_LMAC_RESELECT */
  (hdrind_cb_f_type **) hdrind_lup_aln_trigger,               /* 78 HDRIND_LUP_ALN_TRIGGER */
  (hdrind_cb_f_type **) hdrind_lmac_hdr_usable,               /* 79 HDRIND_LMAC_HDR_USABLE */
  (hdrind_cb_f_type **) hdrind_lmac_deactivate_done,          /* 80 HDRIND_LMAC_DEACTIVATE_DONE */
  (hdrind_cb_f_type **) hdrind_amp_failed_new_esn,            /* 81 HDRIND_AMP_FAILED_NEW_ESN */
  (hdrind_cb_f_type **) hdrind_scm_prot_neg_failed,           /* 82 HDRIND_SCM_NEG_FAILED*/
  (hdrind_cb_f_type **) hdrind_scm_config_complete,           /* 83 HDRIND_SCM_CONFIG_COMPLETE */
  NULL,                                                       /* 84 *** obsolete *** */
  NULL,                                                       /* 85 *** obsolete *** */
  (hdrind_cb_f_type **) hdrind_scm_rmac_gaup,                 /* 86 HDRIND_SCM_RMAC_GAUP */
  (hdrind_cb_f_type **) hdrind_scm_mrlp_gaup,                 /* 87 HDRIND_SCM_MRLP_GAUP */
  (hdrind_cb_f_type **) hdrind_rup_change_hdr_systems,        /* 88 HDRIND_RUP_CHANGE_HDR_SYSTEMS */
  (hdrind_cb_f_type **) hdrind_rup_exit_hdr_mode,             /* 89 HDRIND_RUP_EXIT_HDR_MODE */
  (hdrind_cb_f_type **) hdrind_idle_connection_aborted,       /* 90 HDRIND_IDLE_CONNECTION_ABORTED */
  (hdrind_cb_f_type **) hdrind_lmac_tc_suspend,               /* 91 HDRIND_LMAC_TC_SUSPEND */
  (hdrind_cb_f_type **) hdrind_lmac_tc_resume,                /* 92 HDRIND_LMAC_TC_RESUME */
  (hdrind_cb_f_type **) hdrind_ccmac_interrupted,             /* 93 HDRIND_CCMAC_INTERRUPTED */
  (hdrind_cb_f_type **) hdrind_scm_mrlp_at_gaup_accepted,     /* 94 HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED */
  (hdrind_cb_f_type **) hdrind_scm_mrlp_at_gaup_rejected,     /* 95 HDRIND_SCM_MRLP_AT_GAUP_REJECTED */
  (hdrind_cb_f_type **) hdrind_scm_mrlp_at_gaup_failed,       /* 96 HDRIND_SCM_MRLP_AT_GAUP_FAILED */
  NULL,                                                       /* 97 *** obsolete *** */
  (hdrind_cb_f_type **) hdrind_scm_fmac_gaup,                 /* 98 HDRIND_SCM_FMAC_GAUP */
  (hdrind_cb_f_type **) hdrind_scm_session_changed,           /* 99 HDRIND_SCM_SESSION_CHANGED */
  (hdrind_cb_f_type **) hdrind_scm_rmac_at_gaup_accepted,     /* 100 HDRIND_SCM_AT_GAUP_ACCEPTED */
  (hdrind_cb_f_type **) hdrind_scm_rmac_at_gaup_rejected,     /* 101 HDRIND_SCM_AT_GAUP_REJECTED */
  (hdrind_cb_f_type **) hdrind_scm_rmac_at_gaup_failed,       /* 102 HDRIND_SCM_AT_GAUP_FAILED */ 
  (hdrind_cb_f_type **) hdrind_almp_connection_closed,        /* 103 HDRIND_ALMP_CONNECTION_CLOSED */
  NULL,                                                       /* 104 HDRIND_RMAC3_FLOW_ACTIVATED - obsoleted */
  NULL,                                                       /* 105 HDRIND_RMAC3_FLOW_DEACTIVATED - obsoleted */
  NULL,                                                       /* 106 HDRIND_RMAC3_FLOW_MAPPING_CHANGED - obsoleted */
  (hdrind_cb_f_type **) hdrind_idle_monitor_connection_failed,/* 107 HDRIND_IDLE_MONITOR_CONNECTION_FAILED */
  (hdrind_cb_f_type **) hdrind_gmmcdp_config_complete,        /* 108 HDRIND_GMMCDP_CONFIG_COMPLETE */
  (hdrind_cb_f_type **) hdrind_gmmcd_prot_neg_failed,         /* 109 HDRIND_GMMCD_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_scm_amac_gaup,                 /* 110 HDRIND_SCM_AMAC_GAUP */
  (hdrind_cb_f_type **) hdrind_scm_amac_at_gaup_accepted,     /* 111 HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED */
  (hdrind_cb_f_type **) hdrind_scm_amac_at_gaup_rejected,     /* 112 HDRIND_SCM_AMAC_AT_GAUP_REJECTED */
  (hdrind_cb_f_type **) hdrind_scm_amac_at_gaup_failed,       /* 113 HDRIND_SCM_AMAC_AT_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_rup_config_complete,           /* 114 HDRIND_RUP_CONFIG_COMPLETE */
  (hdrind_cb_f_type **) hdrind_rup_prot_neg_failed,           /* 115 HDRIND_RUP_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_rmac_config_complete,          /* 116 HDRIND_RMAC_CONFIG_COMPLETE */
  (hdrind_cb_f_type **) hdrind_rmac_prot_neg_failed,          /* 117 HDRIND_RMAC_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_scm_amp_gaup,                  /* 118 HDRIND_SCM_AMP_GAUP */
  (hdrind_cb_f_type **) hdrind_scp_an_gaup_failed,            /* 119 HDRIND_SCP_AN_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_idle_an_gaup_failed,           /* 120 HDRIND_IDLE_AN_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_rup_an_gaup_failed,            /* 121 HDRIND_RUP_AN_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_pacc_an_gaup_failed,           /* 122 HDRIND_PACC_AN_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_scm_an_gaup_failed,            /* 123 HDRIND_SCM_AN_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_scp_config_unlocked,           /* 124 HDRIND_SCP_CONFIG_UNLOCKED */
#ifdef FEATURE_HDR_BCMCS
  (hdrind_cb_f_type **) hdrind_bcmcs_sleep_ok,                /* 125 HDRIND_BCMCS_SLEEP_OK */
#else
  NULL,                                                       /* 125 Reserved */
#endif /* FEATURE_HDR_BCMCS */
  (hdrind_cb_f_type **) hdrind_amp_failed_not_maintaining_uati, /* 126 HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI */
  (hdrind_cb_f_type **) hdrind_scp_no_prev_session,            /* 127 HDRIND_SCP_NO_PREV_SESSION */
  (hdrind_cb_f_type **) hdrind_acmac_ready,                   /* 128 HDRIND_ACMAC_READY */
  (hdrind_cb_f_type **) hdrind_acmac_tune_away_time_ok,       /* 129 HDRIND_ACMAC_TUNE_AWAY_TIME_OK */
  (hdrind_cb_f_type **) hdrind_scm_idle_at_gaup_accepted,     /* 130 HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED */
  (hdrind_cb_f_type **) hdrind_scm_idle_at_gaup_rejected,     /* 131 HDRIND_SCM_IDLE_AT_GAUP_REJECTED */
  (hdrind_cb_f_type **) hdrind_scm_idle_at_gaup_failed,       /* 132 HDRIND_SCM_IDLE_AT_GAUP_FAILED */
#ifdef FEATURE_HDR_DDARF
  (hdrind_cb_f_type **) hdrind_idle_network_lost,             /* 133 HDRIND_IDLE_NETWORK_LOST */
#else
  NULL,                                                       /* 133 Reserved */
#endif /* FEATURE_HDR_DDARF */
  (hdrind_cb_f_type **) hdrind_scm_csna_an_gauped,            /* 134 HDRIND_SCM_CSNA_AN_GAUPED */
  (hdrind_cb_f_type **) hdrind_scm_csna_at_config_complete,   /* 135 HDRIND_SCM_CSNA_CONFIG_COMPLETE */
  (hdrind_cb_f_type **) hdrind_csna_prot_neg_failed,          /* 136 HDRIND_CSNA_PROT_NEG_FAILED */
  (hdrind_cb_f_type **) hdrind_csna_3g1x_parm_received,       /* 137 HDRIND_CSNA_3G1X_PARM_RECEIVED */
  (hdrind_cb_f_type **) hdrind_csna_3g1x_parm_reset_received, /* 138 HDRIND_CSNA_3G1X_PARM_RESET_RECEIVED */
#ifdef FEATURE_HDR_QOS
  (hdrind_cb_f_type **) hdrind_rup_assignment_failed,         /* 139 HDRIND_RUP_ASSIGNMENT_FAILED */
#else
  NULL,                                                       /* 139 Reserved */
#endif /* FEATURE_HDR_QOS */
  (hdrind_cb_f_type **) hdrind_scm_scp_at_gaup_accepted,      /* 140 HDRIND_SCM_SCP_AT_GAUP_ACCEPTED */
  (hdrind_cb_f_type **) hdrind_scm_scp_at_gaup_rejected,      /* 141 HDRIND_SCM_SCP_AT_GAUP_REJECTED */
  (hdrind_cb_f_type **) hdrind_scm_scp_at_gaup_failed,        /* 142 HDRIND_SCM_SCP_AT_GAUP_FAILED */
  (hdrind_cb_f_type **) hdrind_amp_opened_prev_session,       /* 143 HDRIND_AMP_OPENED_PREV_SESSION */
  (hdrind_cb_f_type **) hdrind_csna_tx_msg_status_received,   /* 144 HDRIND_CSNA_TX_MSG_STATUS_RECEIVED */
  (hdrind_cb_f_type **) hdrind_lmac_rf_condition_changed,     /* 145 HDRIND_LMAC_RF_CONDITION_CHANGED */
  (hdrind_cb_f_type **) hdrind_rup_pilot_rpt_received,        /* 146 HDRIND_RUP_PILOT_RPT_RECEIVED */
#ifdef FEATURE_EIDLE_SCI
  (hdrind_cb_f_type **) hdrind_idle_an_gaup_slottedmode,      /* 147 HDRIND_IDLE_AN_GAUP_SLOTTEDMODE */
#else
  NULL,                                                       /* 147 Reserved */
#endif /* FEATURE_EIDLE_SCI */
 (hdrind_cb_f_type **) hdrind_amp_failed_new_nai,             /* 148 HDRIND_AMP_FAILED_NEW_NAI */
 (hdrind_cb_f_type **) hdrind_outage_imminent,                /* 149 HDRIND_OUTAGE_IMMINENT */
 (hdrind_cb_f_type **) hdrind_amac_config_complete,           /* 150 HDRIND_AMAC_CONFIG_COMPLETE */
 (hdrind_cb_f_type **) hdrind_amac_prot_neg_failed,           /* 151 HDRIND_AMAC_PROT_NEG_FAILED */
  NULL,                                                       /* 152 HDRIND_RSV1 */
  NULL,                                                       /* 153 HDRIND_RSV2 */
 (hdrind_cb_f_type **) hdrind_ccmac_scc_pkt_rxed,             /* 154 HDRIND_CCMAC_SCC_PKT_RXED */
#ifdef FEATURE_IRAT_PILOT_MEAS
  (hdrind_cb_f_type **) hdrind_lmac_irat_plt_meas_done_ind,    /* 155 HDRIND_LMAC_IRAT_PLT_MEAS_DONE_IND */
#else 
  NULL,
#endif /* FEATURE_IRAT_PILOT_MEAS */
#ifdef FEATURE_EHRPD
  (hdrind_cb_f_type **) hdrind_scp_ehrpd_availability_changed, /* 156 HDRIND_SCP_EHRPD_AVAILABILITY_CHANGED */
#else
  NULL,
#endif /* FEATURE_EHRPD */
 (hdrind_cb_f_type **) hdrind_fmac_prot_neg_failed,           /* 157 HDRIND_FMAC_PROT_NEG_FAILED */
 (hdrind_cb_f_type **) hdrind_fmac_config_complete,           /* 158 HDRIND_FMAC_CONFIG_COMPLETE */
 (hdrind_cb_f_type **) hdrind_lmac_scheduler_group_changed,   /* 159 HDRIND_LMAC_SCHEDULER_GROUP_CHANGED */
 (hdrind_cb_f_type **) hdrind_phy_config_complete,            /* 160 HDRIND_PHY_CONFIG_COMPLETE */
 (hdrind_cb_f_type **) hdrind_phy_prot_neg_failed,            /* 161 HDRIND_PHY_PROT_NEG_FAILED */
 (hdrind_cb_f_type **) hdrind_rtc_restart_timer_expired,      /* 162 HDRIND_RTC_RESTART_TIMER_EXPIRED */
 (hdrind_cb_f_type **) hdrind_rmac_flow_activated,            /* 163 HDRIND_RMAC_FLOW_ACTIVATED */
 (hdrind_cb_f_type **) hdrind_rmac_flow_deactivated,          /* 164 HDRIND_RMAC_FLOW_DEACTIVATED */
 (hdrind_cb_f_type **) hdrind_rmac_flow_mapping_changed,      /* 165 HDRIND_RMAC_FLOW_MAPPING_CHANGED */
 (hdrind_cb_f_type **) hdrind_lmac_carrier_map_changed,       /* 166 HDRIND_LMAC_CARRIER_MAP_CHANGED */
 (hdrind_cb_f_type **) hdrind_scm_phy_an_gaup,                /* 167 HDRIND_SCM_PHY_AN_GAUP */
 (hdrind_cb_f_type **) hdrind_lmac_tca_failed,                /* 168 HDRIND_LMAC_TCA_FAILED */
 (hdrind_cb_f_type **) hdrind_lmac_conn_error,                /* 169 HDRIND_LMAC_CONN_ERROR */
#ifdef FEATURE_LTE_TO_HDR
 (hdrind_cb_f_type **) hdrind_lmac_time_sync_done,            /* 170 HDRIND_LMAC_TIME_SYNC_DONE */
 (hdrind_cb_f_type **) hdrind_lmac_fast_acq_done,             /* 171 HDRIND_LMAC_FAST_ACQ_DONE */ 
#else
  NULL, 
  NULL,
#endif /* FEATURE_LTE_TO_HDR */
 (hdrind_cb_f_type **) hdrind_lmac_timing_info,               /* 172 HDRIND_LMAC_TIMING_INFO */
#ifdef FEATURE_HDR_REVC
 (hdrind_cb_f_type **) hdrind_scm_fmac_at_gaup_accepted,       /* 173 HDRIND_SCM_FMAC_AT_GAUP_ACCEPTED */
 (hdrind_cb_f_type **) hdrind_scm_fmac_at_gaup_rejected,       /* 174 HDRIND_SCM_FMAC_AT_GAUP_REJECTED */
 (hdrind_cb_f_type **) hdrind_scm_fmac_at_gaup_failed,         /* 175 HDRIND_SCM_FMAC_AT_GAUP_FAILTED */
#else	  
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_HDR_REVC */
#ifdef FEATURE_HDR_TO_LTE
 (hdrind_cb_f_type **) hdrind_lmac_irat_reselect,              /* 176 HDRIND_LMAC_IRAT_RESELECT */ 
#else	  
  NULL,
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REVC
  (hdrind_cb_f_type **) hdrind_lmac_aset_changed,              /* 177 HDRIND_LMAC_ASET_CHANGED */ 
  (hdrind_cb_f_type **) hdrind_ovhd_nlb_cache_updated,         /* 178 HDRIND_OVHD_NLB_CACHE_UPDATED */
  (hdrind_cb_f_type **) hdrind_mc_qos_flow_status_changed,     /* 179 HDRIND_MC_QOS_FLOW_STATUS_CHANGED */
#else
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_HDR_REVC */
#ifdef FEATURE_LTE_TO_HDR_OH
 (hdrind_cb_f_type **) hdrind_ovhd_prereg_info_updated,        /* 180 HDRIND_OVHD_PREREG_INFO_UPDATED */ 
 (hdrind_cb_f_type **) hdrind_sap_connection_opened,           /* 181 HDRIND_SAP_CONNECTION_OPENED */
 (hdrind_cb_f_type **) hdrind_sap_connection_initiated,        /* 182 HDRIND_SAP_CONNECTION_INITIATED */
 (hdrind_cb_f_type **) hdrind_sap_connection_closed,           /* 183 HDRIND_SAP_CONNECTION_CLOSED */
 (hdrind_cb_f_type **) hdrind_sap_connection_failed,           /* 184 HDRIND_SAP_CONNECTION_FAILED */
 (hdrind_cb_f_type **) hdrind_sap_idle_handoff,                /* 185 HDRIND_SAP_IDLE_HANDOFF */
 (hdrind_cb_f_type **) hdrind_sap_link_acquired,               /* 186 HDRIND_SAP_LINK_ACQUIRED */
 (hdrind_cb_f_type **) hdrind_sap_alt_link_comp_cb_received,   /* 187 HDRIND_SAP_ALT_LINK_COMP_CB_RECEIVED */
 (hdrind_cb_f_type **) hdrind_saptx_transmission_failed_ac,    /* 188 HDRIND_SAPTX_TRANSMISSION_FAILED_AC */
 (hdrind_cb_f_type **) hdrind_saptx_transmission_cnf_ac,       /* 189 HDRIND_SAPTX_TRANSMISSION_CNF_AC */
 (hdrind_cb_f_type **) hdrind_sap_backoff_timer_expired,       /* 190 HDRIND_SAP_BACKOFF_TIMER_EXPIRED */
 (hdrind_cb_f_type **) hdrind_sap_backoff_timer_stopped,       /* 191 HDRIND_SAP_BACKOFF_TIMER_STOPPED */
 (hdrind_cb_f_type **) hdrind_amp_prereg_zone_changed,         /* 192 HDRIND_AMP_PREREG_ZONE_CHANGED */
 (hdrind_cb_f_type **) hdrind_sap_tunnelmode_status_changed,   /* 193 HDRIND_SAP_TUNNELMODE_STATUS_CHANGED */
 (hdrind_cb_f_type **) hdrind_sap_connection_already_opened,   /* 194 HDRIND_SAP_CONNECTION_ALREADY_OPENED */
 (hdrind_cb_f_type **) hdrind_idle_connection_already_opened,  /* 195 HDRIND_IDLE_CONNECTION_ALREADY_OPENED */
 (hdrind_cb_f_type **) hdrind_amp_irmi_accepted,               /* 196 HDRIND_AMP_IRMI_ACCEPTED */
 (hdrind_cb_f_type **) hdrind_amp_irmi_failed,                 /* 197 HDRIND_AMP_IRMI_FAILED */
 (hdrind_cb_f_type **) hdrind_ovhd_lte_cell_changed,           /* 198 HDRIND_OVHD_LTE_CELL_CHANGED */
 (hdrind_cb_f_type **) hdrind_idle_connection_not_allowed,     /* 199 HDRIND_IDLE_CONNECTION_NOT_ALLOWED */
 (hdrind_cb_f_type **) hdrind_ovhd_color_code_changed,         /* 200 HDRIND_OVHD_COLOR_CODE_CHANGED */
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_LTE_TO_HDR_OH */
 (hdrind_cb_f_type **) hdrind_almp_reselect_exit_hdr,          /* 201 HDRIND_ALMP_RESELECT_EXIT_HDR */
#ifdef FEATURE_LTE_TO_HDR_OH
 (hdrind_cb_f_type **) hdrind_sap_prereg_status_updated,       /* 202 HDRIND_SAP_PREREG_STATUS_UPDATED */
 (hdrind_cb_f_type **) hdrind_sap_returning_to_close,          /* 203 HDRIND_SAP_RETURNING_TO_CLOSE */
 (hdrind_cb_f_type **) hdrind_sap_transmission_stopped,        /* 204 HDRIND_SAP_TRANSMISSION_STOPPED */
 (hdrind_cb_f_type **) hdrind_scp_irat_capability_changed,     /* 205 HDRIND_SCP_IRAT_CAPABILITY_CHANGED */
 (hdrind_cb_f_type **) hdrind_rup_act_ho_completed,            /* 206 HDRIND_SAP_ACT_HO_COMPLETED */
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* FEATURE_LTE_TO_HDR_OH */
 (hdrind_cb_f_type **) hdrind_lmac_sync_suspend,              /* 207 HDRIND_LMAC_SYNC_SUSPEND */
 (hdrind_cb_f_type **) hdrind_lmac_sync_resume,               /* 208 HDRIND_LMAC_SYNC_RESUME */
 (hdrind_cb_f_type **) hdrind_lmac_idle_suspend_to_reacq,     /* 209 HDRIND_LMAC_IDLE_SUSPEND_TO_REACQ */
 (hdrind_cb_f_type **) hdrind_lmac_orat_meas_rpt,             /* 210 HDRIND_LMAC_ORAT_MEAS_RPT */
 (hdrind_cb_f_type **) hdrind_lmac_orat_meas_failed, 		 /* 211 HDRIND_LMAC_ORAT_MEAS_FAILED */
 (hdrind_cb_f_type **) hdrind_lmac_orat_redir_success		 /* 212 HDRIND_LMAC_ORAT_REDIR_SUCCESS */
};

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIND_GIVE_IND

DESCRIPTION
  This function gives the specified indication to all the protocols that 
  are registered to receive it.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Name of the indication to be broadcast 
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrind_give_ind( hdrind_ind_name_enum_type ind_name,
                        void *ind_data_ptr )
{
  /* Pointer to an entry in the list of callback function pointers 
   * registered  for this indication */
  hdrind_cb_f_type **cb_f_index_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "hdrind_give_ind %d", ind_name);

  if ((ind_name >= HDRIND_IND_NAME_ENUM_MAX) ||
      (hdrind_ind_table[ind_name] == NULL))
  {
    ERR("Invalid Indication %d",ind_name,0,0);
  }
  else
  {
    cb_f_index_ptr = hdrind_ind_table[ind_name];
    while (*cb_f_index_ptr != NULL)
    {
      (*cb_f_index_ptr)( ind_name, ind_data_ptr );
      cb_f_index_ptr++;
    }
  }
} /* hdrind_give_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIND_GET_IND_DATA_SIZE

DESCRIPTION
  This function returns the data size of the specificed indication name

DEPENDENCIES
  None
  
PARAMETERS
  ind_name - indication name

RETURN VALUE
  Size of the indication data

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrind_get_ind_data_size 
(
  hdrind_ind_name_enum_type ind_name
)
{

   uint32 size=0; /* size of data associated with the given indication */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
   switch(ind_name)
   {
     case HDRIND_SCM_SESSION_CHANGED:
       size = sizeof( hdrhai_session_change_reason_enum_type );
       break;

     case HDRIND_LMAC_IDLE_HANDOFF:
     case HDRIND_LMAC_AHO_COMPLETE:
     case HDRIND_LMAC_FREQ_CHANGE_COMPLETE:
       size = sizeof(hdrsrch_link_id_type);
       break;
      
     case HDRIND_OVHD_INFO_UPDATED:
     case HDRIND_ACMAC_TRANSMISSION_FAILED:
     case HDRIND_ACMAC_TUNE_AWAY_TIME_OK:
     case HDRIND_CONNECTED_CONNECTION_CLOSED:
     case HDRIND_FTCMAC_SUPERVISION_FAILED:
     case HDRIND_OVHD_PREREG_INFO_UPDATED:
     case HDRIND_SAP_IDLE_HANDOFF:
#ifdef FEATURE_LTE_TO_HDR_OH
     case HDRIND_OVHD_LTE_CELL_CHANGED:
#endif /* FEATURE_LTE_TO_HDR_OH */
       size = sizeof(boolean);
       break;

#ifdef FEATURE_HDR_DDARF
     case HDRIND_LMAC_AWAKE:
       size = sizeof(hdrind_lmac_wakeup_status_type);
       break;
#endif /* FEATURE_HDR_DDARF */

     case HDRIND_SCP_FAILED:
     case HDRIND_IDLE_PROT_NEG_FAILED:
     case HDRIND_STREAM_PROT_NEG_FAILED:
     case HDRIND_KEP_PROT_NEG_FAILED:
     case HDRIND_GMMCD_PROT_NEG_FAILED:
     case HDRIND_SCM_PROT_NEG_FAILED:
     case HDRIND_RUP_PROT_NEG_FAILED:
     case HDRIND_RMAC_PROT_NEG_FAILED:
     case HDRIND_AMAC_PROT_NEG_FAILED:
     case HDRIND_SCP_AN_GAUP_FAILED:
     case HDRIND_IDLE_AN_GAUP_FAILED:
     case HDRIND_RUP_AN_GAUP_FAILED:
     case HDRIND_PACC_AN_GAUP_FAILED:
     case HDRIND_SCM_AN_GAUP_FAILED:
     case HDRIND_CSNA_PROT_NEG_FAILED:
     case HDRIND_CSNA_TX_MSG_STATUS_RECEIVED:
#ifdef FEATURE_EIDLE_SCI
     case HDRIND_IDLE_AN_GAUP_SLOTTEDMODE:
#endif /* FEATURE_EIDLE_SCI */
     case HDRIND_FMAC_PROT_NEG_FAILED:  
     case HDRIND_PHY_PROT_NEG_FAILED:
       size = sizeof(void *);
       break;

     case HDRIND_SMP_SESSION_CLOSED:
       size = sizeof( hdrhai_session_close_reason_enum_type );
       break;

     case HDRIND_IDLE_CONNECTION_OPENED:
#ifdef FEATURE_LTE_TO_HDR_OH
     case HDRIND_SAP_CONNECTION_ALREADY_OPENED:
#endif /* FEATURE_LTE_TO_HDR_OH */
       size = sizeof( hdrhai_open_reason_enum_type );
       break;

     case HDRIND_FCP_DATA_READY_RCVD:
       size = sizeof( hdrhai_stream_enum_type );   
       break;

     case HDRIND_SCM_CONFIG_COMPLETE:
     case HDRIND_SCM_CONFIG_REQ:
     case HDRIND_GMMCDP_CONFIG_COMPLETE:
     case HDRIND_RMAC_CONFIG_COMPLETE:
     case HDRIND_AMAC_CONFIG_COMPLETE:
     case HDRIND_FMAC_CONFIG_COMPLETE:
     case HDRIND_PHY_CONFIG_COMPLETE:
       size = sizeof( hdrhai_protocol_name_enum_type);
       break;

     case HDRIND_SCM_MRLP_AT_GAUP_ACCEPTED:
     case HDRIND_SCM_MRLP_AT_GAUP_REJECTED:
     case HDRIND_SCM_MRLP_AT_GAUP_FAILED:
     case HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED:
     case HDRIND_SCM_RMAC_AT_GAUP_REJECTED:
     case HDRIND_SCM_RMAC_AT_GAUP_FAILED:
     case HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED:
     case HDRIND_SCM_AMAC_AT_GAUP_REJECTED:
     case HDRIND_SCM_AMAC_AT_GAUP_FAILED:
     case HDRIND_RMAC_FLOW_DEACTIVATED:
     case HDRIND_RMAC_FLOW_ACTIVATED:
     case HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED:
     case HDRIND_SCM_IDLE_AT_GAUP_REJECTED:
     case HDRIND_SCM_IDLE_AT_GAUP_FAILED:
     case HDRIND_SCM_SCP_AT_GAUP_ACCEPTED:
     case HDRIND_SCM_SCP_AT_GAUP_REJECTED:
     case HDRIND_SCM_SCP_AT_GAUP_FAILED:
     case HDRIND_RTC_RESTART_TIMER_EXPIRED:
     case HDRIND_RUP_CONN_SETUP_FAILED:
#ifdef FEATURE_LTE_TO_HDR_OH
     case HDRIND_SAP_CONNECTION_OPENED:
     case HDRIND_SAP_TUNNELMODE_STATUS_CHANGED:
     case HDRIND_OVHD_COLOR_CODE_CHANGED:
#endif /* FEATURE_LTE_TO_HDR_OH */
       size = sizeof( uint8 );
       break;

     case HDRIND_IDLE_CONNECTION_FAILED:
#ifdef FEATURE_LTE_TO_HDR_OH
     case HDRIND_SAP_CONNECTION_FAILED:
     case HDRIND_IDLE_CONNECTION_NOT_ALLOWED:
#endif /* FEATURE_LTE_TO_HDR_OH */
       size = sizeof(hdrind_idle_conn_fail_info_type);
       break;

     case HDRIND_LMAC_NETWORK_LOST:
       size = sizeof(hdrsrch_system_lost_enum_type);
       break;

     case HDRIND_LMAC_RF_CONDITION_CHANGED:
       size = sizeof(hdrind_lmac_rf_change_enum_type);
       break;

     case HDRIND_OUTAGE_IMMINENT:
       size = sizeof(hdrmac_outage_notification_data_type);
       break;

     case HDRIND_CCMAC_SCC_PKT_RXED:
       size = sizeof(hdrind_cmac_intra_scc_info_type);
       break;

#ifdef FEATURE_LTE_TO_HDR
     case HDRIND_LMAC_TIME_SYNC_DONE:
       size = sizeof(hdrsrch_tran_sys_time_enum_type);
       break;

     case HDRIND_LMAC_FAST_ACQ_DONE:
       size = sizeof(hdrsrch_fast_acq_enum_type);
       break;
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
     case HDRIND_LMAC_ORAT_MEAS_RPT:
	   size = sizeof(hdrind_srchltemeas_rpt_struct_type);
       break;
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

     case HDRIND_LMAC_TIMING_INFO:
       size = sizeof(hdrind_lmac_timing_info_type);
       break;

#ifdef FEATURE_HDR_TO_LTE
     case HDRIND_LMAC_IRAT_RESELECT:
       size = sizeof(hdrind_lmac_lte_resel_info_type);
       break;
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_REVC
     case HDRIND_OVHD_NLB_CACHE_UPDATED:
       size = sizeof(sys_link_type);
       break;
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_LTE_TO_HDR_OH
     case HDRIND_SAPTX_TRANSMISSION_FAILED_AC:
       size = sizeof(hdrind_irat_failure_ind_type);
       break;

     case HDRIND_SAP_PREREG_STATUS_UPDATED:
       size = sizeof(hdrhai_prereg_status_event_enum_type);
       break;
#endif /* FEATURE_LTE_TO_HDR_OH */

     case HDRIND_LMAC_ORAT_MEAS_FAILED:
       size = sizeof(hdrhai_irat_meas_fail_reason_enum_type);
       break;

     default:
       break;
   }

   return size;
} /* hdrind_get_ind_data_size() */

