/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         A I R   L I N K   M A N A G E M E N T   P R O T O C O L

GENERAL DESCRIPTION
  The Air Link Management Protocol (ALMP) is the main state machine
  controlling the connection layer of the HDR protocol stack.

EXTERNALIZED FUNCTIONS (Global)

  hdralmp_msg_cb() - Callback registered with HMP to deliver  messages to ALMP
  hdralmp_ind_cb() - Callback registered for delivery of indications
  hdralmp_activate() - Command function to activate ALMP
  hdralmp_deactivate() - Command function to deactivate ALMP
  hdralmp_open_connection() - Command function to open a connection
  hdralmp_close_connection() - Command function to open a connection
  hdralmp_reacquire() - Command to return to init state and re-acquire
  hdralmp_reset() - Command to reset due to the session being closed
  hdralmp_connection_is_open() - Query if a connection is open
  hdralmp_system_is_acquired() - Query if a system is currently acquired

EXTERNALIZED FUNCTIONS (Regional)

  hdralmp_process_cmd() - Process a command for ALMP
  hdralmp_process_user_cmd() - Process a command for ALMP from CM (user)
  hdralmp_process_ind() - Process an indication for ALMP
  hdralmp_process_msg() - Process a message for ALMP

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdralmp.c_v   1.69   05 Mar 2003 14:15:10   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdralmp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/16   vko     Fixed race condition between HDR inactive and 
                          HDRMC_STANDBY_PREF_CHGD_CMD_F
10/12/16   vko     Handle MMODE deactivate in middle of D2L measurement
05/09/16   vko     Fix compilation errors
04/28/16   vko     Fixed KW issues
04/05/16   vko     FR 33944: Back port FR26160:3G back to LTE as fast as possible
09/14/15   vko     FR 29020: LTE Power Save Mode w/o coordination with network
07/24/15   vko     Skip earfcn for D2L reselection, if reselection failed with forbidden TA
                   reason
06/11/15   vko     DO should pass LTE Instance ID while sending D2L req/abort req to LTE
05/26/15   vko     DO should wait for D2L Abort response, if deactivate command is received
05/07/15   vko     Inform srch if D2L reselection is aborted. 
12/28/14   vko     Debug info for hdr
11/08/14   vko     DO CP to double check if D2L Reselection CH is supported before sending reselect
05/15/14   arm     TRM API changes for DR-DSDS. 
02/26/14   vko     FR 17879: Extended EARFCN
04/17/14   sat     Fixing the bug in Reporting Connection Failure Reason 
03/14/14   vko/dsp FR19338:Reduce call setup time by skipping RF Tx shutdown
03/31/14   vko     deactivate RMAC, when session is closed and RUP is in connection setup
09/30/13   cnx     Deactivate DO until D2L resel abort is acked by LTE.
10/08/13   sju     Fixed reporting incorrect auto deactivation reason to MMOC.
01/31/14   vko     Fixed SD API not being called, when FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS is enabled
10/07/13   vko     Fixed MMOC not responding to HDR deactivate
11/28/13   vko     Add defensive NULL check for ind_ptr, when HDRIND_IDLE_NETWORK_LOST is recvd
10/31/13   ukl     Keep NSET updated for system avoidance.
09/19/13   ukl     Reduce Code Duplication HDR-CP
06/28/13   cnx     Abort D2L if AdviseUnlock rx'ed during D2L resel.
05/30/13   cnx     Handle more SD actions after D2L resel abort/failure.
04/03/13   wsh     Fixed not releasing RF during ACT HO.
03/01/13   vko     Fix the logging of EVENT_T_HDR_LTE_RESEL_STATUS 
03/20/13   cnx     Return to tunnel if sys loss happens during active handoff. 
02/13/13   cnx     Ensure LTE is presented in RAT pri list of resel cmd.
11/16/12   wsh     Inform SD D2L failure so it will ask HDR to acq same sys
11/06/12   cnx     Delay processing tunnel activation if idle handoff in progress
09/26/12   cnx     Ignore the resel ind received in INACTIVE or INIT state.
08/22/12   cnx     Set deactivation_cmd_is_pending before entering SUSPEND.
08/14/12   cnx     Redir abort to SD when hystr timer expires.
07/24/12   cnx     Handle D2L in non-hybrid mode.
07/16/12   cnx     Handle connection request when SAP subtype is 0.
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
05/22/12   sju     Fixed dropping Close Connection Cmd in tunnel mode.
03/23/12   cnx     Subtract task activation/deactivation time from get CGI timer.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
01/06/12   cnx     Add indication HDRIND_ALMP_RESELECT_EXIT_HDR.
01/05/12   wsh     Added support for SVLTE+SHDR
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
10/23/11   cnx     Fixed a bug when AT handoff to HDR in the middle session 
                   negotiation in the tunnel mode.
10/10/11   cnx     Execute ALMP activate command directly in the tunnel mode.    
10/03/11   cnx     Set AMP reselect flag after successfully reselect to LTE.
07/27/11   cnx     Merged Optimized Handoff changes.
05/10/11   wsh     Fixed unpacking band error in redirect msg
03/22/11   cnx     Set hdralmp.deactivate.deactivation_cmd_is_pending before enter 
                   INACTIVE state to make sure MMoC is informed of deactivation.
08/17/10   sju     Changed fusion feature flag. 
02/16/11   pba     Handled deactivate due to PROT_GEN_CMD during reselection.
08/17/10   sju     Changed fusion feature flag. 
06/25/10   sju     Added support for ConnectionFailureReport based on SAR.
06/21/10   cnx     Fixed compiler warnings.
05/19/10   pba     Added support for Reverse Time Transfer.
04/02/10   sju     Removed temporary changes added for 9k bringup.
03/02/10   pba     Added support for LTE to HDR reselection
12/01/09   grl     Temporarily removed access failure reporting.
09/23/09   sju     Added support for SVDO.
10/27/09   pba     Klockwork fixes.
09/23/09   wsh     Report RX PER data to CM 
09/18/09   wsh     CMI: support changes from modem DATA
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
12/18/08   wsh     Fixed not reporting to MMoC deactivate in some condition
12/10/08   smd     Replaced assert.h with amssassert.h
10/23/08   wsh     Fixed critical/high lint warnings
08/13/08   sju     Added support for SMP KeepAlive enhancement.
05/22/08   pba     Klockwork fixes.
12/12/07   sju     Renamed SystemType enum based on sys.h
11/13/07   sju     Fixed resetting stored connection failure report records.
09/10/07   wsh     Report session or user traffic for KDDI system reselection timer
07/19/07   sju     Added sending TCC+CC(reason0x2) if invalid RUP1 TCA was received    
07/13/07   sju     Changed channel record type to hdr channel record type 
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/06   wsh     Fixed a race condition in handling HDRALMP_DEACTIVATE_CMD
02/12/07   mpa     Fixed RVCT warnings.
11/15/06   sju     Add ABORT_MSG as a new reason to abort connection.
09/22/06   pba     Set QSM timer duration in hdrcon_process_deactivate instead
                   of hdralmp_close_connection_command.
09/19/06   pba     Handled HDRIND_IDLE_NETWORK_LOST indication. (DDARF)
09/05/06   mpa     Added support for FEATURE_HDR_QSM
07/12/06   mpa     Propagate system lost reason down to SD.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
06/06/06   hal     Updated ConnectionFailureReport logging rules
05/18/06   mpa     Handle SD action for sd_ss_ind_hdr_opr_stop_activity().
04/13/06   yll     Mainlined FEATURE_HDR_APERSISTENCE_INHIBIT, to make the 
                   behavior consistent for all operators.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
02/23/06   hal     Changed hdrovhd_get_sp_msg() to hdrovhd_get_sp_msg_location()
02/14/06   hal     Changed ConnectionFailureRecords to return zeroed out 
                   SectorID if OVHD cache entry is invalid
01/16/06   yll     Check if AT has a stream assigned when handling FMAC
                   supervision failure and deactivating the hdridle.
12/13/05   pba     Modified SCMDB calls to use updated API's
                   Added support for handle TCA + ConnectionClose bundling
12/05/05   yll     For FMAC supervision failure, call the SD action from
                   ALMP, and only deactivate the hdridle when SD action is 
                   to continue.
11/18/05   hal     Uses CM API instead of directly accessing cm_mc_rpt_free_q
11/02/05   etv     Changed SCM APIs to take Subtype info.
10/28/05   mpa     Added new HDRIND_ALMP_CONNECTION_CLOSED ind.
10/20/05   hal     Added support for ConnectionFailureReporting
07/12/05   hal     Added support for CloseReason 011 (1x handdown)
                   Added parameter hdrhai_abort_reason_enum_type reason to
                   hdrcon_close_connection()
06/07/05   grl     Moved FTC supervision timeout logging out of file.
04/04/05   pba     Removed Lint errors
11/29/04   dna     Send END_CNF to CM with call_id of user ended call.
                   Send all commands to CM even if there is no session.
11/28/04   grl     Added connected state stats logging for the PS.
11/08/04   pba     Added HDRIND_RUP_EXIT_HDR_MODE, HDRIND_RUP_CHANGE_HDR_SYSTEMS
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
11/01/04   mpa     Removed ASSERT in open connection command.
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
09/20/04   mpa     hdramp_close_connection() may be called outside of HDRMC
06/30/04   dna     Delayed giving mmoc_rpt_prot_auto_deactd_ind() until           
                   HDRIND_LMAC_DEACTIVATE_DONE is received.  This changed     
                   hdralmp_deactivate(), hdralmp_cmd_params_union_type, and 
                   delayed when and where we give HDRIND_ALMP_EXIT_HDR_MODE.  
06/28/04   mpa     Send a ConnectionClose message when redirected by the AN
05/03/04    gr     Added RLP init for scenarios where the traffic channel is 
                   already up. Moved this from DS to HDRMC context.
04/13/04   dna     Allow ALMP to close connection immediately if TX is stopped.
03/23/04   mpa     Give new reason to SD when access probe is denied because
                   apersistence is set to inhibit
12/02/03   mpa     Inform SD of system lost in init state as well.
11/24/03   mpa     Do not call SD again if the protocols are pending to
                   Exit HDR or Reacquire
11/19/03   mpa     Mainlined FEATURE_HDR_RESELECT,FEATURE_HDR_RESELECT_CLOSE_CON
                   Send reports to CM if any of the streams are negotiated
11/03/03    ic     In hdralmp_send_call_release_to_cm() set release code for
                   each call id
10/23/03    km     Send response to CM only if we have valid Call Id
10/21/03    km     Updated to new ds707_extif interfaces
10/09/03    km     Fixed callid deallocation and added to F3 msgs
09/12/03   mpa     Code cleanup + converted F3 msgs to use new MSG2.0 SSID
09/11/03    km     Added support for new CM interface for array of callids
07/23/03   mpa     Merged in 6500 baseline
05/07/03   mpa     Only notify CM of reselect if we have an active data session
03/05/03   dna     Moved "connection_is_closing" to hdrcon.c
01/04/03   mpa     Removed sd_action ASSERTs.
01/30/03   mpa     Notify CM of needed reselection (weak HDR system)
08/01/02   dna     Prevent handling an indication in interrupt context.
07/19/02   vas     Added function hdralmp_connection_is_closing()
05/02/02   dna     Removed assert for FMAC supervision fail in init state
04/09/02   sh      Added change HDR and exit HDR for redirect and PRL not
                   preferred abort reason
03/10/02   sh      Use CM_HDR prefix for CM reports from HDR
01/14/02   dna     Remove assert on HDRIND_ACMAC_TRANSMISSION_FAILED
                   received in a state other than idle state.
01/04/02   sh      Added hybrid operation support
12/11/01   sh      Discard reports to CM if SN stream is not assigned except
                   call orig failed report with no session.
11/20/01   dna     Added enum HDRHAI_SYSTYPE
10/29/01   dna     Reset Idle State Prot. on FMAC supervision fails while idle
                   As per QC-IS-856 BALLOT COMMENT-56
09/26/01   sh      Passed otasp related data from the system changed
                   to sd_ss_ind_user_ss_pref
09/24/01   sh      Added processing for MC_RESET_F
09/05/01   dna     Added separate handling for in-use vs. in-config messages.
08/18/01   sh      Send abort reason to connected state in hdrcon_deactivate
08/06/01   vas     Changed function data_has_stream() to stream_is_assigned()
07/10/01   dna     Send connection open reason to idle state.  Handle the new
                   open reason returned in the HDRIND_IDLE_CONNECTION_OPENED
                   to just close the newly opened connection if necessary.
                   Added hdralmp_reset(), to reset ALMP if a session is closed.
06/19/01   sh      Fixed macros for redirect message field offsets
05/08/01   dna     Re-naming for consistency.
05/01/01   dna     Added pref_reason to pref_sys_change command
03/29/01   dna     Added reacquire command to force ALMP to re-acquire
03/19/01   dna     Added support for IS-856 compliant session layer.
01/26/01   dna     Set the rel_code of the CALL_RELEASE command to CM.
09/07/00   dna     Modifications for SD interface.
02/02/00   dna     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "mc.h"

#ifdef FEATURE_EHRPD
#include "hdrmrlpc.h"
#endif

#ifdef FEATURE_CMI
#include "mc_v.h"
#endif

#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "sd.h"

#ifdef FEATURE_CMI
#include "sd_v.h"
#endif

#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrhai.h"
#include "hdrutil.h"
#include "hdrcallid.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdralmp.h"
#include "hdralmpi.h"
#include "hdralmps.h"
#include "hdrhmp.h"
#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdramp.h"
#endif /* FEATURE_LTE_TO_HDR_OH */  
#include "cmxll.h"
#include "hdrsmp.h"
#include "hdrinit.h"
#include "hdridle.h"
#include "hdrtrace.h"
#include "hdramac.h"
#include "hdrlup.h"
#include "hdrpac_api.h"
#include "hdrplog.h"
#include "hdrrups.h"
#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
#include "lte_rrc_irat_msg.h"
#include "cm_gw.h"
#include "hdridles.h"
#include "hdrlmac.h"
#include "fs_public.h"
#endif /* FEATURE_HDR_TO_LTE */
#include "lte_rrc_ext_api.h"
#include "mcfg_fs.h"

#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

#include "ds707_extif.h"
#include "prot.h"

#ifdef FEATURE_CMI
#include "prot_v.h"
#endif


#include "hdrscmalmp.h" /* Included for configurable attribs */
#include "hdrscmdb.h"   /* Included for configurable attribs */
#include "trm.h"        /* Included to determine ownership of RF chain for 
                           Connection Failure records */

#include "hdrrx.h"
#ifdef FEATURE_HDR_MC_MSGR
#include "hdrcp_msg.h"
#endif /* FEATURE_HDR_MC_MSGR */

#include "hdrsrchrf.h"
#include "policyman.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* The number of Connection Failure records stored by ALMP */
#define HDRALMP_MAX_CONN_FAILURE_RECORDS 4

/* Unknown PER when not Connected. 
   To be replaced with constant provided by CM interface */
#define HDRALMP_FL_PER_UNKNOWN  0xFFFF

#define HDRALMP_FL_PER_TIMER_INTERVAL 5000

/* Estimated time for activating and deactivating HDR stack */
#define HDRALMP_ACTIVATE_AND_DEACTIVATE_HDR_STACK_TIME 30

#define HDRALMP_ORAT_EUTRA_RAT_TYPE 0x01
#define HDRALMP_UE_INITIATED_REDIR_DISABLED_FILENAME "/nv/item_files/modem/hdr/cp/almp/ue_init_d2lredir_disable"
#define HDRALMP_AN_INITIATED_REDIR_ENABLED_FILENAME "/nv/item_files/modem/hdr/cp/almp/an_init_d2lredir"
#define HDRALMP_D2L_REDIR_MAX_RPT_CNT "/nv/item_files/modem/hdr/cp/almp/max_rpt_cnt"
#define HDRALMP_D2L_REDIR_SERVING_THRESH_TRIGGER "/nv/item_files/modem/hdr/cp/almp/serving_thresh_trig"
#define HDRALMP_D2L_REDIR_THRESHXQ_TRIGGER "/nv/item_files/modem/hdr/cp/almp/threshxq_trig"
#define HDRALMP_D2L_REDIR_THRESHX_TRIGGER "/nv/item_files/modem/hdr/cp/almp/threshx_trig"
#define HDRALMP_D2L_REDIR_MIN_RPT_INT "/nv/item_files/modem/hdr/cp/almp/min_rpt_int"
#define HDRALMP_D2L_REDIR_MIN_INIT_LTE_MEAS_TIME "/nv/item_files/modem/hdr/cp/almp/min_init_lte_meas_time"

#define HDRALMP_D2L_REDIR_MAX_RPT_CNT_DEFAULT 3
#define HDRALMP_D2L_REDIR_SERVING_THRESH_TRIGGER_DEFAULT 0
#define HDRALMP_D2L_REDIR_THRESHXQ_TRIGGER_DEFAULT 10
#define HDRALMP_D2L_REDIR_THRESHX_TRIGGER_DEFAULT 13
#define	HDRALMP_D2L_REDIR_MIN_RPT_INT_DEFAULT 6
#define	HDRALMP_D2L_REDIR_MIN_INIT_LTE_MEAS_TIME_DEFAULT 60

#define HDRALMP_CPFL_MAX_PREF_FREQ_LIST_ENTRIES 20
#define HDRALMP_CPFL_FREQ_LIST_EFS_FILENAME "/nv/reg_files/modem/lte/rrc/csp/pref_freq_list"
/* Connection Failure Record Type */
typedef struct
{
  /* 0x0 - General Connection failure
     0x1 - Connection failure due to 1x tune-away
     0x2 - Connection failure due to position determination tune-away */
  uint8 failure_reason;

  /* The time stamp of the CDMA system time in units of 1.28 seconds */
  uint32 time_stamp;

  /* The Sector ID where the Connection failure occurred */
  hdraddr_type sector_id;

  /* The Channel Record for the CDMA channel on which the Connection
     failure occurred */
  sys_channel_type channel_record;

  /* TRUE if the latitude and longitude fields are included in the message */
  boolean lat_long_included;

  /* Latitude of the location where the Connection failure occurred */
  int32 latitude;

  /* Longitude of the location where the Connection failure occurred */
  int32 longitude;

} hdralmp_conn_fail_rec_type;

  /* The information needed to inform MMoC when we deactivate HDR */
typedef struct
{
   boolean          deactivation_cmd_is_pending;
   boolean          trans_id_is_valid;
   prot_trans_type  trans_id;

} hdralmp_deactivate_type;

#ifdef FEATURE_HDR_TO_LTE
typedef struct
{
  lte_earfcn_t     earfcn;
    /* Frequency of the LTE cell to be selected */
  
  uint16           cell_id;
    /* Cell id of the LTE cell to be selected */

  sys_rat_pri_list_info_s_type rat_pri_list;
    /* RAT priority list */
	
  boolean          resel_cmd_is_sent;
    /* Whether resel cmd is sent to LTE */

  boolean          pending_abort_resel;
    /* If HDR is asked to release RF before sending resel cmd to LTE, HDR should abort
     * resel and reacqure. This flag indicates whether abort resel is desired. */

  boolean          resel_abort_cmd_is_sent;
	/* Whether the abort request is sent to LTE */
} hdralmp_lte_resel_info_type;
#endif /* FEATURE_HDR_TO_LTE */

typedef enum
{
  HDRALMP_FL_PER_TIMER = 0
#ifdef FEATURE_LTE_TO_HDR
  ,HDRALMP_GET_CGI_TIMER = 1
#endif /* FEATURE_LTE_TO_HDR */
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  ,HDRALMP_MIN_INIT_LTE_MEAS_TIMER = 2
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

} hdralmp_timer_enum_type;

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
#define HDRALMP_MAX_EARFCN_REDIR_MSG 7
#define HDRALMP_MAX_PLMNIDS_PER_FREQ 7
#define HDRALMP_MAX_PCI_PER_FREQ 32


typedef struct
{
	boolean plmnid_incl;
	    /* Whether PLMNID is included */
	uint8 plmnid_cnt;
	    /* Number of PLMN IDs that are associated with this EARFCN */
	sys_plmn_id_s_type plmnid[HDRALMP_MAX_PLMNIDS_PER_FREQ];
        /* PLMNID of the neighboring EUTRA system */
	boolean plmnid_valid[HDRALMP_MAX_PLMNIDS_PER_FREQ];
		/* If the PLMNID is valid or not */
	boolean pci_incl;
		/* PCI included or not */
        uint8  pci_cnt;
		/* number of PCI that are associated with this EARFCN */
	lte_phy_cell_id_t pci[HDRALMP_MAX_PCI_PER_FREQ];
		/* List of PCI that are associated with this EARFCN */
}hdralmp_earfcn_t;

typedef struct
{
  uint8 rat_type;
    /* Radio Access Technology Type */
  uint8 earfcn_cnt;
    /* EARFCN count for D2L redirection */
  lte_earfcn_t earfcn[HDRALMP_MAX_EARFCN_REDIR_MSG];
    /* List of EARFCN for D2L redirection */
  hdralmp_earfcn_t earfcn_list[HDRALMP_MAX_EARFCN_REDIR_MSG];
    /* List of related info, associated with earfcn  */
  boolean redirect_is_in_progress;
	/* Whether redirection is currently in progress */
  boolean redir_cmd_is_sent;
	/* Whether redirection command is sent to LTE */
  boolean pending_abort_redir;
	/* If there is any abort pending, during redirection */
  uint16 lte_supported_band_index;
	/* Supported lte band. used to determine if the earfcn is supported or not */	
  sys_rat_pri_list_info_s_type rat_pri_list;
	/* RAT priority list */
  boolean plmnid_incl_any_earfcn;
	/* if any of the earfcn contain a PLMNID */  
  boolean redir_abort_cmd_is_sent;
	/* if redirection abort is sent to LTE */
  
} hdralmp_lte_redir_info_type;

typedef struct
{
  boolean ue_initiated_redir;
  boolean an_initiated_redir;
  uint8 max_rpt_cnt;
  uint8 serving_thresh_trig;
  uint8 threshxq_trig;
  uint8 threshx_trig;
  uint8 min_rpt_int;
  uint8 min_init_lte_meas_time;
}hdralmp_lte_redir_info_default_value_type;


typedef struct
{
  boolean lte_is_in_home;
  boolean is_cpfl_valid;
  lte_efs_earfcn_t  efs_earfcns_list[HDRALMP_CPFL_MAX_PREF_FREQ_LIST_ENTRIES];
  uint8 num_pref_earfcns;
}hdralmp_cpfl_info_type;

#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */


/* Control block of the Air Link Management Protocol */

LOCAL struct
{
/* If set, wait for the connection to close to notify CM of the handdown */
  boolean reselect_in_progress;

  /* The information needed to inform MMoC when we deactivate HDR */
  hdralmp_deactivate_type   deactivate;

  /* whether HDR is suspended for a 1x or gps tune away. */
  boolean is_traffic_suspended;

  /* The stored Connection Failure records */
  hdralmp_conn_fail_rec_type conn_fail_recs[HDRALMP_MAX_CONN_FAILURE_RECORDS];

  /* The number of stored Connection Failure records */
  uint8 num_conn_fail_recs;

  /* Transaction ID for ConnectionFailureReport messages */
  uint8 conn_fail_trans_id;

  /* Indicates if ALMP is waiting for a ConnectionFailureReportAck */
  boolean wait_for_conn_failure_report_ack;

  /* Reports PER every HDRALMP_RX_PER_TIMER_INTERVAL ms when connected */
  rex_timer_type fl_per_timer; 

#ifdef FEATURE_LTE_TO_HDR
  rex_timer_type get_cgi_timer; 
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE
  hdralmp_lte_resel_info_type lte_resel_info;
#endif /* FEATURE_HDR_TO_LTE */
  boolean psm_report_ind_pending;

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  hdralmp_lte_redir_info_type lte_redir_info;
  hdralmp_lte_redir_info_default_value_type lte_redir_efs_info; 
  lte_rrc_acq_db_earfcn_rsp_s lte_rrc_acq_db_earfcn;
  hdralmp_cpfl_info_type lte_cpfl_info;
  /* timer which delay the first measurements after going to eHRPD connected state */
  rex_timer_type min_init_lte_meas_timer; 
  boolean redir_meas_in_progress;
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */


}hdralmp;

LOCAL boolean hdralmp_check_lte_in_home(void);
LOCAL void hdralmp_send_call_fade_to_cm( void );
LOCAL void hdralmp_send_call_release_to_cm( void );
LOCAL void hdralmp_send_reselect_to_clients( void );
LOCAL boolean hdralmp_sd_already_consulted( void );
LOCAL sd_ss_sys_lost_e_type hdralmp_map_sys_lost_to_sd
(
  hdrsrch_system_lost_enum_type reason
);
LOCAL void hdralmp_lte_nset_update_redir(void);
LOCAL boolean hdralmp_check_ue_initiated_redir_allowed( void );
/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRALMP_TASK_PTR hdrmc_tcb_ptr

/* Message IDs */
#define HDRALMP_REDIRECT_MSG 0x00     /* Redirect Message ID */
#define HDRALMP_CONN_FAILURE_REPORT_MESSAGE_ID     0x01
#define HDRALMP_CONN_FAILURE_REPORT_ACK_MESSAGE_ID 0x02

/* The transaction ID is 8 bits into the message */
#define HDRALMP_CONN_FAILURE_REPORT_ACK_TRANS_ID_OFFSET 0x08

#define HDRALMP_MESSAGE_ID_LEN 0x08
#define HDRALMP_TRANS_ID_LEN   0x08

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* ConnectionFailureReport Layout - lengths in bits
 */
#define HDRALMP_CONN_FAILURE_REPORT_NUM_RECS_LEN      4

/* The following values are used per failure record */
#define HDRALMP_CONN_FAILURE_REPORT_FAIL_REASON_LEN   4
#define HDRALMP_CONN_FAILURE_REPORT_TIME_STAMP_LEN   32
#define HDRALMP_CONN_FAILURE_REPORT_SYSTEM_TYPE_LEN   8
#define HDRALMP_CONN_FAILURE_REPORT_BAND_CLASS_LEN    5
#define HDRALMP_CONN_FAILURE_REPORT_CHAN_NUM_LEN     11
#define HDRALMP_CONN_FAILURE_REPORT_LAT_LONG_INC_LEN  1
#define HDRALMP_CONN_FAILURE_REPORT_LATITUDE_LEN     22
#define HDRALMP_CONN_FAILURE_REPORT_LONGITUDE_LEN    23

/* The message priority of the ConnectionFailureReport */
#define HDRALMP_CONN_FAILURE_REPORT_MSG_PRIORITY     40

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Common MessageID Field
 */
#define HDRALMP_MESSAGEID( ptr )             ((ptr->data_ptr)[0])

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Message : Redirect
 * Channels: CC, FTC
 */
#define HDRALMP_REDIRECT_NUM_CHANNEL( ptr, retptr ) \
          (HDRBIT_UNPACK8( ptr, retptr, 8, 8 ) )

#define HDRALMP_REDIRECT_BAND( num, ptr, retptr ) \
          (HDRBIT_UNPACK8(ptr, retptr, (uint16)(16 + 24 * num + 8), 5))
#define HDRALMP_REDIRECT_CHANNEL( num, ptr, retptr ) \
          (HDRBIT_UNPACK16(ptr, retptr, (uint16)(16 + 24 * num + 13), 11))
#define HDRALMP_REDIRECT_SYSTEM_TYPE( num, ptr, retptr ) \
          (HDRBIT_UNPACK8(ptr, retptr, (uint16)(16 + 24 * num ), 8))

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRALMP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_msg_cb( dsm_item_type * item_ptr )
{
  /* This protocol runs in the HDRMC task. */
  hdrmc_queue_msg( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
    HDRHAI_IN_USE, item_ptr );
} /* hdralmp_msg_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRALMP.

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

void hdralmp_ind_cb( hdrind_ind_name_enum_type ind_name, void *ind_data )
{
  /* This protocol runs in the HDRMC task. */
  if (( rex_self(  ) == HDRALMP_TASK_PTR ) && !rex_is_in_irq_mode())
  {
    /* Indication is from another protocol in MC task, so consider it
     * an internal indication
     */
    hdralmp_process_ind( ind_name, (hdrind_ind_data_union_type *)ind_data );
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, ind_name, ind_data );
  }
} /* hdralmp_ind_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_ACTIVATE

DESCRIPTION
  Asynchronous command
  This function commands ALMP to activate and enter the initialization state.
  This function must be called when entering HDR mode of operation.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_activate( hdrhai_protocol_name_enum_type sender )
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdralmp_activate");

  /* Fill in all members of the hdralmp_cmd_type for the Activate command */
  hdralmp_cmd.name = HDRALMP_ACTIVATE_CMD;
  hdralmp_cmd.sender = sender;

  /* In the native HDR, ALMP activate command should be considered an  
   * external command because it is not ever given as a result of some other 
   * internal event, or even by another HDR protocol for that matter. 
   * Technically it does not matter because no internal events should ever 
   * be queued before ALMP is activated.
   * However, in the tunnel mode, It is possible that other protocols (e.g. 
   * OVHD) will be processed before ALMP if ALMP is queued. So execute ALMP 
   * directly in this case. 
   */
#ifdef FEATURE_LTE_TO_HDR_OH
  if ( ( sender == HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) &&
       rex_self() == HDRALMP_TASK_PTR)
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
         sizeof( hdralmp_cmd_type ) );
  }
} /* hdralmp_activate() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_DEACTIVATE

DESCRIPTION
  This function commands HDRALMP to deactivate.  Currently there is no
  protocol that would need to call this function, since all events that
  cause the access terminal to leave HDR mode are handled by ALMP.
  This command is intended for use by a higher layer protocol.

DEPENDENCIES
  None

PARAMETERS
  reason - reason for deactivation
  explicit_mmoc_deactivate - Whether MMoC sent the HDRMC_PROT_DEACT_CMD_F
  trans_id - The MMoC Transaction ID of its deactivate command to HDRMC
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_deactivate
(
  hdrhai_abort_reason_enum_type reason,
  boolean                        explicit_mmoc_deactivate,
  prot_trans_type                trans_id,
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdralmp_deactivate");

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdralmp_cmd.name = HDRALMP_DEACTIVATE_CMD;
  hdralmp_cmd.sender = sender;
  hdralmp_cmd.params.deactivate.reason = reason;
  hdralmp_cmd.params.deactivate.explicit_mmoc_deactivate = 
    explicit_mmoc_deactivate;
  hdralmp_cmd.params.deactivate.trans_id = trans_id;


  if ( rex_self(  ) == HDRALMP_TASK_PTR )
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
  {
    /* Currently no other task ever deactivates ALMP, so we should never
     * get here
     */
    ERR( "ALMP deactivated from outside MC!", 0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
         sizeof( hdralmp_cmd_type ) );
  }
} /* hdralmp_deactivate() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_OPEN_CONNECTION

DESCRIPTION
  This function commands ALMP to open a connection.

DEPENDENCIES
  None

PARAMETERS
  open_reason - The reason the connection is being opened
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_open_connection
(
  hdrhai_open_reason_enum_type open_reason,
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW,  "hdralmp_open_connection" );

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdralmp_cmd.name = HDRALMP_OPEN_CONNECTION_CMD;
  hdralmp_cmd.sender = sender;
  hdralmp_cmd.params.open_connection.open_reason = open_reason;

  hdralmp.is_traffic_suspended = FALSE;

  if ( rex_self() == HDRALMP_TASK_PTR )
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
  {
    /* DS should be the only task will call open connection outside HDRMC */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Attempt to open connection from outside MC(%d)", 
                    sender );
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
                     sizeof( hdralmp_cmd_type ) );
  }

} /* hdralmp_open_connection() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_CLOSE_CONNECTION

DESCRIPTION
  This function commands ALMP to close a connection.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_close_connection
(
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_LOW,  "hdralmp_close_connection" );

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdralmp_cmd.name = HDRALMP_CLOSE_CONNECTION_CMD;
  hdralmp_cmd.params.close_connection.reason = reason;
  hdralmp_cmd.sender = sender;

  if ( rex_self() == HDRALMP_TASK_PTR )
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
  {
    /* DS should be the only task will call open connection outside HDRMC */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Attempt to close connection from outside MC(%d)", 
                    sender );
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
                     sizeof( hdralmp_cmd_type ) );
  }

} /* hdralmp_close_connection() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_REACQUIRE

DESCRIPTION
  This function commands ALMP to return to initialization state.  If a
  connection is open and in the process of being closed gracefully, ALMP
  will not transition to initialization state until the connection close is
  complete.  The AT will then proceed to attempt acquisition on whatever
  channel is given by system determination.

DEPENDENCIES
  None

PARAMETERS
  reason - reason to reacquire
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_reacquire
(
  hdrhai_abort_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
)
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH,  "hdralmp_reacquire");

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdralmp_cmd.name = HDRALMP_REACQUIRE_CMD;
  hdralmp_cmd.sender = sender;
  hdralmp_cmd.params.reacquire.reason = reason;

  if ( rex_self(  ) == HDRALMP_TASK_PTR )
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
  {
    /* Currently no other task ever asks ALMP to reacquire, so we should
     * never get here
     */
    ERR( "Request to reacquire from outside MC!", 0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
         sizeof( hdralmp_cmd_type ) );
  }
} /* hdralmp_reacquire() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_RESET

DESCRIPTION
  This function commands ALMP to reset itself.  When the reset is complete,
  ALMP is in idle state, no access attempt is in progress and no connection
  setup is being attempted.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_reset( hdrhai_protocol_name_enum_type sender )
{
  /* Temporary cmd buffer passed to MC to be copied into a queue item */
  hdralmp_cmd_type hdralmp_cmd;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_LOW,  "hdralmp_reset");

  /* Fill in all members of the hdralmp_cmd_type for this command */
  hdralmp_cmd.name = HDRALMP_RESET_CMD;
  hdralmp_cmd.sender = sender;

  if ( rex_self(  ) == HDRALMP_TASK_PTR )
  {
    hdralmp_process_cmd( &hdralmp_cmd );
  }
  else
  {
    /* Currently no other task ever resets ALMP, so we should never
     * get here
     */
    ERR( "Request to reset from outside MC!", 0, 0, 0 );
    hdrmc_queue_cmd( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, &hdralmp_cmd,
         sizeof( hdralmp_cmd_type ) );
  }
} /* hdralmp_reset() */

/*===========================================================================

FUNCTION HDRALMP_GENERATE_CONN_FAIL_RECORD

DESCRIPTION
  This function generates the Connection Failure record that will be sent
  with the ConnectionFailureReport if the ConnectionFailureReportingEnabled
  simple attribute is negotiated.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_generate_conn_fail_record
( 
  hdrhai_abort_reason_enum_type abort_reason
)
{
  hdralmp_conn_fail_rec_type conn_fail_record;

  uint8 subnet_mask;
  sys_link_type current_link;

  time_type ts_val; /* ts_val is a qword */
  uint32 time_stamp;

  /* Location given by the most recent SP message */
  hdrovhd_sp_msg_location_type sp_msg_location;

  /* Traps any error codes */
  hdrerrno_enum_type err_code = E_SUCCESS;

  /* The index of where to store the failure record */
  int conn_fail_rec_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize local variable - connection fail record */
  memset ( &conn_fail_record, 0, sizeof(hdralmp_conn_fail_rec_type) ); 

  ASSERT( !hdralmp.wait_for_conn_failure_report_ack );
  if( hdralmp.wait_for_conn_failure_report_ack )
  {
    return;
  }

  /*----------------------------------------------------------
    Determine ownership of RF chain during Connection Failure 
    ----------------------------------------------------------*/
  if (abort_reason == HDRHAI_ABORT_SAR_LIMIT)
  {
    /* 1x is connected state and DO power is backed off due to SAR limit,
       mark the connection failure reason as 1x tune-away */
    conn_fail_record.failure_reason = HDRHAI_1X_TUNE_AWAY_FAILURE;
  }
  /* Check if 1x has the main transceiver */
  else if( hdrsrchrf_get_rf_device( TRM_1X ) == hdrsrchrf_get_last_valid_rf_dev() )
  {
    /* If 1x has the main transceiever, mark the connection failure
       reason as 1x tune-away */
    conn_fail_record.failure_reason = HDRHAI_1X_TUNE_AWAY_FAILURE;
  }
  /* Check if GPS has the main transceiver */
  else if( hdrsrchrf_get_rf_device( TRM_GPS ) == hdrsrchrf_get_last_valid_rf_dev() ) 
  {
    /* If GPS has the main transceiver, mark the connection failure
       reason as position determination tune-away */
    conn_fail_record.failure_reason = HDRHAI_LOCATION_TUNE_AWAY_FAILURE;
  }
  else /* the main transceiver must belong to HDR */
  {
    /* Mark the connection failure reason as a General Connection Failure */
    conn_fail_record.failure_reason = HDRHAI_GENERAL_FAILURE;
  }

  /* Get the current System Time and convert to a 32 bit quantity in 
     units of 1.28 seconds */
  (void) time_get( ts_val );
  time_stamp = ( qw_lo( ts_val ) >> 26 );
  time_stamp |= ( qw_hi( ts_val ) << 6 );
  conn_fail_record.time_stamp = time_stamp;

  /* Get the current CDMA Channel */
  if( hdrovhd_get_qc_link( &current_link, 
                           HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL ) == E_SUCCESS )
  {
    conn_fail_record.channel_record = current_link.channel;

    /* Get the current Sector ID */
    err_code = hdrovhd_get_sector_id2( conn_fail_record.sector_id, 
      &subnet_mask, &current_link, HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );

    /* If OVHD returns an invalid Sector ID */
    if( err_code == E_NO_DATA )
    {
      /* Zero out invalid Sectord ID */
      memset( conn_fail_record.sector_id, 0, HDRADDR_LENGTH_IN_BYTES );
    }

    /* Get the current latitude and longitude if available */
    if( hdrovhd_get_sp_msg_location( &sp_msg_location,  
          HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL ) == E_SUCCESS )
    {
      conn_fail_record.lat_long_included = TRUE;
  
      /* Latitude is sign-extended 22 bit value */
      conn_fail_record.latitude = sp_msg_location.latitude;
  
      /* Longitude is sign-extended 23 bit value */
      conn_fail_record.longitude = sp_msg_location.longitude;
    }
    else /* not available */
    {
      conn_fail_record.lat_long_included = FALSE;
    }
  }
  else
  {
    /* Should never happen... */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "No current link available" );
  }

  /* Determine index of this failure record, overwriting the 
     oldest record if necessary */
  conn_fail_rec_index = hdralmp.num_conn_fail_recs % 
                        HDRALMP_MAX_CONN_FAILURE_RECORDS;
  
  /* Store record into ALMP struct */
  hdralmp.conn_fail_recs[conn_fail_rec_index] = conn_fail_record;
    
  /* Update the number of saved Connection Failure records */
  hdralmp.num_conn_fail_recs++;
  
} /* hdralmp_generate_conn_fail_record() */

/*===========================================================================

FUNCTION HDRALMP_RESET_CONN_FAIL_RECORDS

DESCRIPTION
  This function is used to reset all the stoerd Connection Failure records.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_reset_conn_fail_records( void )
{
  int const size_of_rec_list = sizeof( hdralmp_conn_fail_rec_type ) * 
                                           HDRALMP_MAX_CONN_FAILURE_RECORDS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( hdralmp.conn_fail_recs, 0, size_of_rec_list );

  /* Reset the waiting for ack flag to FALSE */
  hdralmp.wait_for_conn_failure_report_ack = FALSE;

  /* Set the number of stored records to 0 */
  hdralmp.num_conn_fail_recs = 0;

} /* hdralmp_reset_conn_fail_records() */

/*===========================================================================

FUNCTION HDRALMP_SEND_CONN_FAIL_REPORT_OUTCOME_CB

DESCRIPTION
  This function is called by SLP to return the status of the Connection
  Failure Report transmission.

DEPENDENCIES
  None.

PARAMETERS
  data_ptr - User defined data passed back from SLP.
  status   - The transmission status ( E_SUCCESS, E_FAILURE, etc.. )

RETURN VALUE
  None.

SIDE EFFECTS
  If the transmission is a failure then the stored Connection Failure 
  records will be reset.

===========================================================================*/
void hdralmp_send_conn_fail_report_outcome_cb
(
  void              *data_ptr,
  hdrerrno_enum_type status
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( hdralmp.wait_for_conn_failure_report_ack && 
      hdralmp.conn_fail_trans_id == (uint8) ( (uint32) data_ptr ) )
  {
    if( status != E_SUCCESS )
    {
      HDR_MSG_PROT( MSG_LEGACY_LOW, "ConnectionFailureReport not sent" );
      hdralmp.wait_for_conn_failure_report_ack = FALSE;
      hdralmp_reset_conn_fail_records();
    }
    else
    {
      HDR_MSG_PROT( MSG_LEGACY_LOW, 
                    "ConnectionFailureReport sent successfully" );
    }
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_LOW, 
                  "Ignored invalid ConnectionFailureReport callback" );
  }

} /* hdralmp_send_conn_fail_report_outcome_cb() */

/*===========================================================================

FUNCTION HDRALMP_SEND_CONN_FAIL_REPORT

DESCRIPTION
  If the access terminal has any stored ConnectionFailure records and if the 
  value of the ConnectionFailureReportingEnabled attribute is negotiated then
  the access terminal will send a ConnectionFailureReport message.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_send_conn_fail_report( void )
{
  /* The message being built and sent over the RTC */
  dsm_item_type *msg_ptr;

  /* The offset into the message at which to begin packing */
  uint16 msg_offset = 0;

  /* The transaction ID of this ConnectionFailureReport instance */
  uint8 trans_id = hdralmp.conn_fail_trans_id;

  /* The number of stored Connection Failure records */
  int num_recs = hdralmp.num_conn_fail_recs;

  int i; /* Loop counters */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is at least one record stored then build and send the report */
  if( num_recs > 0 )
  {
    /* Allocate a DSM item for the message */
    msg_ptr = hdrutil_new_msg_buffer();
  
    HDRBIT_PACK8( msg_ptr,
                  HDRALMP_CONN_FAILURE_REPORT_MESSAGE_ID,
                  msg_offset, /* Packing offset */
                  HDRALMP_MESSAGE_ID_LEN );
  
    msg_offset += HDRALMP_MESSAGE_ID_LEN;

    /* Increment transaction ID for this message */
    trans_id++;

    HDRBIT_PACK8( msg_ptr, trans_id, msg_offset, HDRALMP_TRANS_ID_LEN );
    msg_offset += HDRALMP_TRANS_ID_LEN;

    /* Check to ensure that we only send MAX failure records */
    if( num_recs > HDRALMP_MAX_CONN_FAILURE_RECORDS )
    {
      /* num_recs can exceed MAX failure records because num_recs is
         used as a circular index into the failure record array */
      num_recs = HDRALMP_MAX_CONN_FAILURE_RECORDS;
    }

    HDRBIT_PACK8( msg_ptr, num_recs, msg_offset, 
      HDRALMP_CONN_FAILURE_REPORT_NUM_RECS_LEN );

    msg_offset += HDRALMP_CONN_FAILURE_REPORT_NUM_RECS_LEN;
  
    /* For each record ... */
    for( i = 0; i < num_recs; i++ )
    {
      /* Pack the failure reason */
      HDRBIT_PACK8( msg_ptr, hdralmp.conn_fail_recs[i].failure_reason, 
        msg_offset, HDRALMP_CONN_FAILURE_REPORT_FAIL_REASON_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_FAIL_REASON_LEN;

      /* Pack the time stamp */
      HDRBIT_PACK32( msg_ptr, hdralmp.conn_fail_recs[i].time_stamp,
        msg_offset, HDRALMP_CONN_FAILURE_REPORT_TIME_STAMP_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_TIME_STAMP_LEN;

      /* Sector ID is a 16 byte (128 bit) array */
      hdraddr_pack_addr( msg_ptr, hdralmp.conn_fail_recs[i].sector_id, msg_offset );
      msg_offset += 128;
      
      /* Pack Channel Record separated as System Type (8 bits), Band (5 bits) 
         and Channel Number (11 bits) */
      /* TODO: system type maybe 2 */
      HDRBIT_PACK8( msg_ptr, SYS_HDR_CHAN_REC_MODE_IS856, msg_offset, 
        HDRALMP_CONN_FAILURE_REPORT_SYSTEM_TYPE_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_SYSTEM_TYPE_LEN;
      
      HDRBIT_PACK8( msg_ptr, hdralmp.conn_fail_recs[i].channel_record.band, 
        msg_offset, HDRALMP_CONN_FAILURE_REPORT_BAND_CLASS_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_BAND_CLASS_LEN;

      HDRBIT_PACK16( msg_ptr, hdralmp.conn_fail_recs[i].channel_record.chan_num,
        msg_offset, HDRALMP_CONN_FAILURE_REPORT_CHAN_NUM_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_CHAN_NUM_LEN;

      HDRBIT_PACK8( msg_ptr, hdralmp.conn_fail_recs[i].lat_long_included,
        msg_offset, HDRALMP_CONN_FAILURE_REPORT_LAT_LONG_INC_LEN );

      msg_offset += HDRALMP_CONN_FAILURE_REPORT_LAT_LONG_INC_LEN;

      /* If the Latitude and Longitude is included then pack those two fields */
      if( hdralmp.conn_fail_recs[i].lat_long_included )
      {
        HDRBIT_PACK32( msg_ptr, hdralmp.conn_fail_recs[i].latitude, 
          msg_offset, HDRALMP_CONN_FAILURE_REPORT_LATITUDE_LEN );

        msg_offset += HDRALMP_CONN_FAILURE_REPORT_LATITUDE_LEN;

        HDRBIT_PACK32( msg_ptr, hdralmp.conn_fail_recs[i].longitude, 
          msg_offset, HDRALMP_CONN_FAILURE_REPORT_LONGITUDE_LEN );

        msg_offset += HDRALMP_CONN_FAILURE_REPORT_LONGITUDE_LEN;
      }
      
    }

    HDR_MSG_PROT( MSG_LEGACY_MED, "Sending ConnectionFailureReport message" );

    /* The last parameter should be a pointer, which is 4 bytes long on most machines.
       However, trans_id is only 1 byte (uint8), so compiler will give a warning like:
       cast to pointer from integer of different size. Cast the uint8 to uint32 first
       to avoid compiler warning. */
    hdrhmp_send_msg_on_existing_rtc( 
                     HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                     FALSE,
                     HDRALMP_CONN_FAILURE_REPORT_MSG_PRIORITY,
                     HDRHAI_IN_USE, /* should be in use */
                     msg_ptr,
                     hdralmp_send_conn_fail_report_outcome_cb,
                     (void *) ((uint32)trans_id) /* callback data ptr */ );
  
    /* Set the waiting for ack flag to TRUE */
    hdralmp.wait_for_conn_failure_report_ack = TRUE;

    /* Store the transaction ID */
    hdralmp.conn_fail_trans_id = trans_id;

  } /* end if( num_recs > 0 ) */

} /* hdralmp_send_conn_fail_report() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_CM_MC_RPT

DESCRIPTION
  This function sends a report to CM.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - The pointer to the command to be sent to CM.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdralmp_cm_mc_rpt( cm_mc_rpt_type * cmd_ptr )
{
  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT_1 (MSG_LEGACY_LOW,  "CM MC rpt %d", cmd_ptr->hdr.cmd);
  cm_mc_rpt( cmd_ptr );
} /* hdralmp_cm_mc_rpt() */

/*===========================================================================

FUNCTION HDRALMP_SEND_PER_RPT

DESCRIPTION
  This function sends PER info to CM
  
DEPENDENCIES
  None

PARAMETERS
  fl_per - PER rate as reported by MAC (scaled by 10000, 0.01% = 1)
           PER_UNKNOWN = 0xFFFF

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_send_per_rpt( uint16 fl_per )
{
  cm_hdr_per_info_ind_s_type* ind_ptr;

  void* tmp_ptr;
    /* Temporary pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  ind_ptr = (cm_hdr_per_info_ind_s_type*) cm_mc_rpt_get_buf_else_err_fatal();

  if (ind_ptr != NULL)
  {
    ind_ptr->hdr.cmd = CM_HDR_PER_INFO_IND_F;
    ind_ptr->hdr_per_info = fl_per;

    HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "ALMP ==PER(%d)==> CM ", fl_per);

    /* Use void pointer as a bridge to avoid the following compiler warning: 
       "cast increases required alignment of target type". */
    tmp_ptr = (void *)ind_ptr;
    hdralmp_cm_mc_rpt( (cm_mc_rpt_type*) tmp_ptr);
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED,  "No CM buffer to send report");
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_CHANGE_STATE

DESCRIPTION
  This function is called on a state transition where there is common
  processing that is event dependent and thus can not be done as part of
  the state machine.

DEPENDENCIES
  None

PARAMETERS
  abort_reason - The reason why we are leaving the state.  It may not
    actually be due to an abort.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdralmp_change_state
(
  hdrhai_abort_reason_enum_type abort_reason,
  hdralmps_state_enum_type next_state
)
{
  /* Current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );
  cm_call_id_type call_id; /* Call IDs of pending user end */
  hdrind_ind_data_union_type ind_data;

  /* Simple attrib used for ConnectionFailureReport messages;
     0x00 - ConnectionFailureReport messages are disabled
     0x01 - ConnectionFailureReport messages are enabled */
  uint8 conn_failure_reporting_enabled;

  sd_ss_act_e_type sd_action;
    /* SD action - continue, acquire HDR, acquire IS-95, etc. */

  uint16 fl_per = HDRALMP_FL_PER_UNKNOWN;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Currently, an indication to reselect from searcher can only be received
   * and handled while we have a connection opened */
  ASSERT( (!hdralmp.reselect_in_progress) ||
          hdralmps_get_state() == HDRALMPS_CONNECTED );

  if (cur_state == HDRALMPS_IDLE)
  {
    /* The behavior of idle state when it is deactivated is dependent on the
     * reason we are leaving idle state.
     */
    hdridle_deactivate( abort_reason, HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
    if(hdralmp.redir_meas_in_progress == TRUE)
    {
       if (next_state == HDRALMPS_SUSPEND )
        { 
          /* Inform SD, that D2L redirection is triggered */
         HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "HDR DLR: calling SD start activity for DLR with mask %d",SD_SS_HDR_ACT_MASK_D2L_IN_PROG); 
         sd_action = sd_ss_ind_hdr_opr_start_activity( NULL, SD_SS_HDR_ACT_MASK_D2L_IN_PROG );
       }
       else
       {
         hdralmp.lte_redir_info.redirect_is_in_progress = FALSE;
         hdralmp.redir_meas_in_progress = FALSE;
         /* Enable CC msgs */
         hdrhmp_set_ignore_cc_msgs(FALSE);     
         ind_data.irat_meas_fail_reason = HDRIND_IRAT_MEAS_ABORTED;
         /* aborted, give report to DS */
         hdrind_give_ind( HDRIND_LMAC_ORAT_MEAS_FAILED, (void *)&ind_data );
       }
    }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
  }
  else if (cur_state == HDRALMPS_CONNECTED)
  {
    /* Unconditionally reset the wait for ack to FALSE */
    hdralmp.wait_for_conn_failure_report_ack = FALSE;

    /* Retrieve attrib from SCM DB */
    (void) hdrscmdb_get_inuse_attrib( 
                      HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, 
                      HDRSCMALMP_CONN_FAILURE_REPORTING_ENABLED_ATTRIB_ID, 
                      &conn_failure_reporting_enabled,
                      HDRSCP_DEFAULT_ALMP );

    /* If the user ended the call, send an end confirmation to CM */

    if (hdrcallid_deallocate_user_ended_call_id( &call_id ))
    {
      hdrmc_send_end_cnf( call_id );
    }

    /* If we closed using a close message, consider it a call release,
     * otherwise consider it a call fade.
     */
    switch (abort_reason)
    {
      case HDRHAI_ABORT_NORMAL_CLOSE:
        hdralmp_send_call_release_to_cm();
        break;

      case HDRHAI_ABORT_USER_RUDE_CLOSE:
        /* do not send report to CM because user
           initiated this rude close */
        break;

      case HDRHAI_ABORT_SAR_LIMIT:
      default:
        /* Generate a ConnectionFailure record for non-normal closes */
        if( conn_failure_reporting_enabled == 0x01 )
        {
          hdralmp_generate_conn_fail_record(abort_reason);
        }
        
        hdralmp_send_call_fade_to_cm();
        break;
    }

    hdrcon_deactivate(abort_reason,HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

    /* Tell SD that we are not connected so it can proceed with acquisition 
     * attempts for a 1x system at full speed, if no 1x system is acquired.
     * Report whether the previous traffic was user data or session data. */
    if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
    {
      /* if stream is already configured, this is user traffic */
      sd_action = sd_ss_ind_hdr_opr_stop_activity( NULL, 
                                                   SD_SS_HDR_ACT_MASK_TRAFFIC );
    }
    else
    {
      /* otherwise, it's session traffic */
      sd_action = sd_ss_ind_hdr_opr_stop_activity( NULL, 
                                                   SD_SS_HDR_ACT_MASK_SESSION );
    }

    if((next_state == HDRALMPS_INACTIVE)) 
    {
	    HDR_MSG_PROT(MSG_LEGACY_HIGH," Ignore: Do not change ALMP state, when ALMP already deactivating");  
    }
    else if ( sd_action != SD_SS_ACT_CONTINUE )
    {
      if ( (sd_action == SD_SS_ACT_ACQ_HDR) && (next_state != HDRALMPS_INIT) )
      {
        HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
                       "traffic ending cause SD reselect (cur %d, old %d->new %d)",
                       cur_state, next_state, HDRALMPS_INIT);

        next_state = HDRALMPS_INIT;
      }
      else if ( next_state != HDRALMPS_INACTIVE )
      {
        HDR_MSG_PROT_3(MSG_LEGACY_HIGH,
                       "traffic ending cause SD pwr_save (cur %d, old %d->new %d)",
                       cur_state, next_state, HDRALMPS_INACTIVE);

        next_state = HDRALMPS_INACTIVE;
      }
    }
  }

  if (next_state == HDRALMPS_INACTIVE)
  {
    if (!hdralmp.deactivate.deactivation_cmd_is_pending)
    {
      /* catch all "autonomous deactivates" here */
      hdralmp.deactivate.deactivation_cmd_is_pending = TRUE;
      hdralmp.deactivate.trans_id_is_valid = FALSE;
    }
  }


  /* The connection is now closed.  We can notify CM. */
  if (hdralmp.reselect_in_progress)
  {
    hdralmp.reselect_in_progress = FALSE;
    hdralmp_send_reselect_to_clients();
  }

  /* If ALMP is entering the Connected state */
  if( next_state == HDRALMPS_CONNECTED )
  {
    /* Retrieve attrib from SCM DB */
    (void) hdrscmdb_get_inuse_attrib( 
                      HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, 
                      HDRSCMALMP_CONN_FAILURE_REPORTING_ENABLED_ATTRIB_ID, 
                      &conn_failure_reporting_enabled,
                      HDRSCP_DEFAULT_ALMP );

    /* Send a ConnectionFailureReport if ALMP has any failures to report */
    if( conn_failure_reporting_enabled == 0x01 )
    {
      hdralmp_send_conn_fail_report();
    }
  }

  hdralmps_change_state(next_state);

  if ( next_state == HDRALMPS_CONNECTED )
  {
   (void) rex_set_timer(&hdralmp.fl_per_timer,
                        HDRALMP_FL_PER_TIMER_INTERVAL);
  }

  /* Notify clients that ALMP has finished transitioning out of Connected state */
  if (cur_state == HDRALMPS_CONNECTED )
  {
    (void) rex_clr_timer(&hdralmp.fl_per_timer);

    fl_per = hdrrx_get_connected_state_per();

    hdralmp_send_per_rpt(fl_per);
    hdralmp_send_per_rpt(HDRALMP_FL_PER_UNKNOWN);

    hdrind_give_ind( HDRIND_ALMP_CONNECTION_CLOSED, NULL);
  }

  if ((cur_state == HDRALMPS_CONNECTED) &&
      (next_state == HDRALMPS_IDLE ))
  {
    hdrrup_continue_after_connection_close(
      HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 	
    if(hdralmp.redir_meas_in_progress == TRUE)
    {
      /* Trigger DLR redirection measurements */
	  hdralmp_lte_nset_update_redir();
    }
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
  }

} /* hdralmp_change_state() */

/* <EJECT> */
#ifdef FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION HDRALMP_SEND_RESELECT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to reselect to specified LTE
 freq and cell ID.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_send_reselect_req_to_lte( void )
{
  lte_rrc_eHRPD_resel_req_s resel_req;
    /* Message to request reselect to LTE. */
  errno_enum_type estatus;
    /* Error status */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_DUAL_SIM
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "IRAT: Sending reselect request to LTE :%d inst",
                      SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
  msgr_init_hdr_inst(&resel_req.msg_hdr, MSGR_HDR_CP,
                  LTE_RRC_eHRPD_RESEL_REQ, SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
#else
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "IRAT: Sending reselect request to LTE");

  msgr_init_hdr ( &resel_req.msg_hdr, MSGR_HDR_CP,
                  LTE_RRC_eHRPD_RESEL_REQ );
#endif /* FEATURE_HDR_DUAL_SIM */

  resel_req.earfcn = hdralmp.lte_resel_info.earfcn;
  resel_req.lte_cell_id = hdralmp.lte_resel_info.cell_id;
  resel_req.nas_params.network_select_mode = 
      SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
  resel_req.nas_params.rat_pri_list = hdralmp.lte_resel_info.rat_pri_list;
  resel_req.nas_params.num_req_plmn = 
      hdrovhd_get_plmnids_associated_with_earfcn( hdralmp.lte_resel_info.earfcn, 
                                                  resel_req.nas_params.requested_plmn );

  estatus = msgr_send ( &resel_req.msg_hdr, sizeof ( lte_rrc_eHRPD_resel_req_s ) );
  
  hdralmp.lte_resel_info.resel_cmd_is_sent = TRUE;  
  
  #ifdef FEATURE_EHRPD
      if (hdrmrlpc_current_personality_is_ehrpd())
      {
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE(TRUE);
      }
      else
      {
        HDRTRACE_MRLP_EHRPD_PERSONALITY_IS_ACTIVE(FALSE);
      }
  #endif /* FEATURE_EHRPD */

  HDRTRACE_ALMP_LTE_RESELECT ( resel_req.earfcn, resel_req.lte_cell_id );

  ASSERT ( estatus == E_SUCCESS );

#ifdef FEATURE_HDR_DEBUG_INFO
  hdrmc_log_sent_rpt(0, HDR_LTE_RRC_EHRPD_RESEL_REQ);
#endif /* FEATURE_HDR_DEBUG_INFO */
} /* hdralmp_send_reselect_req_to_lte() */
/* <EJECT> */

/*===========================================================================

FUNCTION HDRALMP_SEND_RESELECT_ABORT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to abort reselection.
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_send_reselect_abort_req_to_lte( void )
{
  lte_rrc_eHRPD_abort_resel_req_s abort_resel_req;
    /* Message to request abort reselect to LTE. */
  errno_enum_type estatus;
    /* Error status */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR_DUAL_SIM
  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "IRAT: Sending abort reselect request to LTE:%d inst",
                                  SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));

msgr_init_hdr_inst (&abort_resel_req.msg_hdr, MSGR_HDR_CP,
                    LTE_RRC_eHRPD_ABORT_RESEL_REQ, SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1) );
#else
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "IRAT: Sending abort reselect request to LTE");

  msgr_init_hdr ( &abort_resel_req.msg_hdr, MSGR_HDR_CP,
                  LTE_RRC_eHRPD_ABORT_RESEL_REQ );
#endif /* FEATURE_HDR_DUAL_SIM */

  estatus = msgr_send ( &abort_resel_req.msg_hdr, 
                        sizeof ( lte_rrc_eHRPD_abort_resel_req_s ) );

  ASSERT ( estatus == E_SUCCESS );
#ifdef FEATURE_HDR_DEBUG_INFO
  hdrmc_log_sent_rpt(0, HDR_LTE_RRC_EHRPD_ABORT_RESEL_REQ);
#endif /* FEATURE_HDR_DEBUG_INFO */  
  hdralmp.lte_resel_info.resel_cmd_is_sent = FALSE;
  hdralmp.lte_resel_info.resel_abort_cmd_is_sent = TRUE;
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Set D2L abort is sent TRUE");

} /* hdralmp_send_reselect_abort_req_to_lte() */

/*===========================================================================

FUNCTION hdralmp_hdr_to_lte_resel_abort_cmd_is_sent

DESCRIPTION
 This function returns the abort reselection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  TRUE - if abort reselection command is sent to LTE
  FALSE - if the command is not sent/abort response recieved

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_abort_cmd_is_sent( void )
{
  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "hdralmp_hdr_to_lte_resel_abort_cmd_is_sent is %d",
          hdralmp.lte_resel_info.resel_abort_cmd_is_sent);
  return(hdralmp.lte_resel_info.resel_abort_cmd_is_sent);
}

/*===========================================================================

FUNCTION hdralmp_clr_hdr_to_lte_resel_abort_cmd_is_sent

DESCRIPTION
 This function clears the abort reselection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clr_hdr_to_lte_resel_abort_cmd_is_sent( void )
{
  hdralmp.lte_resel_info.resel_abort_cmd_is_sent = FALSE;
}

/* <EJECT> */
#endif /* FEATURE_HDR_TO_LTE */

#if defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION hdralmp_send_acq_db_earfcn_req_to_lte

DESCRIPTION
 This function sends a request to LTE RRC to redirect to list of LTE
 freq and cell ID.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_send_acq_db_earfcn_req_to_lte(void)
{

 lte_rrc_acq_db_earfcn_req_s lte_rrc_acq_db_earfcn_req;
    /* Message to request redirectt to LTE. */
  errno_enum_type estatus=E_SUCCESS;
    /* Error status */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "HDR DLR: Sending acqdb earfcn request to LTE");
  memset(&lte_rrc_acq_db_earfcn_req,0,sizeof(lte_rrc_acq_db_earfcn_req_s));
  
  msgr_init_hdr ( &lte_rrc_acq_db_earfcn_req.msg_hdr, MSGR_HDR_CP,
                  LTE_RRC_ACQ_DB_EARFCN_REQ );

  estatus = msgr_send ( &lte_rrc_acq_db_earfcn_req.msg_hdr, sizeof ( lte_rrc_acq_db_earfcn_req ) );
  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: acqdb earfcn request sent to LTE");  

  ASSERT ( estatus == E_SUCCESS );
  
}

#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION || defined FEATURE_HDR_TO_LTE */


#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 

/*===========================================================================

FUNCTION HDRALMP_CLEAR_HDR_TO_LTE_REDIR

DESCRIPTION
  This function clears the info related to hdr to lte redirection.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clear_hdr_to_lte_redir( void )
{
  hdralmp.lte_redir_info.redirect_is_in_progress = FALSE;
  hdralmp.redir_meas_in_progress = FALSE;
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DLR: calling SD stop activity for DLR with mask %d",SD_SS_HDR_ACT_MASK_D2L_IN_PROG); 
  (void)sd_ss_ind_hdr_opr_stop_activity( NULL, SD_SS_HDR_ACT_MASK_D2L_IN_PROG );
}/* hdralmp_clear_hdr_to_lte_redir() */



/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE redirection
  is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L redirection is in progress, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_redir_is_in_progress( void )
{
  return hdralmp.lte_redir_info.redirect_is_in_progress;
}/* hdralmp_hdr_to_lte_resel_is_in_progress() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_CMD_IS_SENT

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE redirection
  command is sent to LTE or not.
 
  HDRCP moves to SUSPEND state and deactivates HDRSRCH once D2L redirection
  starts. Once HDRSRCH is deactivated, HDRCP sends D2L redirection command
  to LTE. This functions can tell whether HDRCP is still waiting HDRSRCH to
  be deactivated.

DEPENDENCIES
  Need to call hdralmp_hdr_to_lte_redir_is_in_progress() first to ensure that
  D2L is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L redirection command is sent to LTE, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_redir_cmd_is_sent( void )
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DLR: hdralmp_hdr_to_lte_redir_cmd_is_sent %d",
  	hdralmp.lte_redir_info.redir_cmd_is_sent);  
  return hdralmp.lte_redir_info.redir_cmd_is_sent;
}/* hdralmp_hdr_to_lte_resel_cmd_is_sent() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SET_PENDING_ABORT_REDIR

DESCRIPTION
  This function sets the value of pending_abort_redir flag.

DEPENDENCIES
  DO to LTE redirection should be already in progress. HDR SRCH has been asked
  to be deactivated but reselection command is not sent to LTE yet.

PARAMETERS
  Boolean value of disired pending_abort_redir flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_set_pending_abort_redir
(
  boolean value
)
{
  hdralmp.lte_redir_info.pending_abort_redir = value;
}/* hdralmp_set_pending_abort_resel() */

/*===========================================================================

FUNCTION hdralmp_update_earfcn_plmnid_info

DESCRIPTION
  This function sets the value of pending_abort_redir flag.

DEPENDENCIES
  DO to LTE redirection should be already in progress. HDR SRCH has been asked
  to be deactivated but reselection command is not sent to LTE yet.

PARAMETERS
  lte_rrc_acq_db_earfcn_rsp_s - info from LTE-RRC containing earfcn plmnid info.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_update_earfcn_plmnid_info(lte_rrc_acq_db_earfcn_rsp_s *lte_rrc_acq_db_earfcn_rsp_s)
{
    memset(&hdralmp.lte_rrc_acq_db_earfcn, 0,sizeof(hdralmp.lte_rrc_acq_db_earfcn));
	
	hdralmp.lte_rrc_acq_db_earfcn = *lte_rrc_acq_db_earfcn_rsp_s;
	
 	if(hdralmp.lte_rrc_acq_db_earfcn.num_earfcn_count == 0)
    {
        HDR_MSG_PROT(MSG_LEGACY_HIGH, "HDR DLR:LTE ACQ DB returned 0 EARFCN");
    }
}


/*===========================================================================

FUNCTION HDRALMP_PROCESS_ORAT_MEASUREMENT_REPORT_IND

DESCRIPTION
  This function process ORAT Measurement report indication from L1

DEPENDENCIES
  None.

PARAMETERS
  redirect_rpt - Pointer to the redirect info obtained from L1.
  ue_initiated_redir - boolean value indicating if UE initiated redirection is set.

RETURN VALUE
  E_SUCCESS - if D2L redirection can be triggered
  E_FAILURE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdralmp_process_orat_measurement_report_ind
(
hdrsrchltemeas_rpt_struct_type* redirect_rpt, 
const boolean ue_initiated_redir
)
{
   uint8 i = 0,j = 0;
   hdrerrno_enum_type err;
   sys_plmn_id_s_type pruned_plmn[HDRALMP_MAX_PLMNIDS_PER_FREQ];
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
         HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: Recieved Measurement Report indication from L1. Cell cnt = %d",redirect_rpt->num_eutra_cell);
	 
   	  if(redirect_rpt->num_eutra_cell == 0) /* If report is empty */
	  {
         return E_FAILURE;
	  }
		 
	  if(redirect_rpt->num_eutra_cell > HDRALMP_MAX_EARFCN_REDIR_MSG) /* If report is greater than allowed */
	  {
         HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "HDR DLR: Cell count more than allowed. Limiting to %d",HDRALMP_MAX_EARFCN_REDIR_MSG);
	  }
      /* UE initiated D2L redirection. Measurement report itself is a redirection trigger */
	  hdralmp.lte_redir_info.rat_type = HDRALMP_ORAT_EUTRA_RAT_TYPE;

     
            for(i = 0 ; 
          ((i < redirect_rpt->num_eutra_cell) && 
           (i < HDRALMP_MAX_EARFCN_REDIR_MSG)); 
          i++)
      {
          /* Consider all the earfc/cell present in the report as eligible earfcn's for redirection */
		  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: earfcn = %d",redirect_rpt->cell[i].earfcn);
		  hdralmp.lte_redir_info.earfcn[i] = redirect_rpt->cell[i].earfcn;
		  hdralmp.lte_redir_info.earfcn_cnt++;
          hdralmp.lte_redir_info.earfcn_list[i].pci[0] = redirect_rpt->cell[i].cell_id;
          hdralmp.lte_redir_info.earfcn_list[i].pci_cnt = 1; /* L1 reports only 1 cell id */
          hdralmp.lte_redir_info.earfcn_list[i].pci_incl = TRUE;
		  /* If earfcn is part of ORNL msg. Then retrieve the PLMN ID's for the earfcn */
	  	  hdralmp.lte_redir_info.earfcn_list[i].plmnid_cnt = 
		  	                     hdrovhd_get_plmnids_associated_with_earfcn(redirect_rpt->cell[i].earfcn, pruned_plmn);
		  if(hdralmp.lte_redir_info.earfcn_list[i].plmnid_cnt > 0)
		  {
       		  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: PLMNID count = %d",hdralmp.lte_redir_info.earfcn_list[i].plmnid_cnt);
		      hdralmp.lte_redir_info.earfcn_list[i].plmnid_incl = TRUE;
			  hdralmp.lte_redir_info.plmnid_incl_any_earfcn = 1;
			  for(j = 0; j < hdralmp.lte_redir_info.earfcn_list[i].plmnid_cnt ; j++)
			  {
				  hdralmp.lte_redir_info.earfcn_list[i].plmnid[j] = pruned_plmn[j];
				  hdralmp.lte_redir_info.earfcn_list[i].plmnid_valid[j] = TRUE;
			  }
		  }
      }
	  hdralmp.lte_redir_info.redirect_is_in_progress = TRUE;

   return E_SUCCESS;

}


/*=============================================================================

FUNCTION HDRALMP_HDR_TO_LTE_REDIR_MEAS_IN_PROGRESS

DESCRIPTION
  This function checks if LTE measurements shall be triggered (or) in progress

DEPENDENCIES
  None.

PARAMETERS
None.

RETURN VALUE
  TRUE is LTE meas started. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdralmp_hdr_to_lte_redir_meas_in_progress()
{
  /* HDR recieves call end with reason, to measure LTE means DLR redirection is in progress */
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: redir_meas_in_progress = %d",hdralmp.redir_meas_in_progress);
  return hdralmp.redir_meas_in_progress;
}

/*===========================================================================

FUNCTION HDRALMP_CHECK_UE_INITIATED_REDIR_ALLOWED

DESCRIPTION
  This function checks if UE initiated D2L redirection is current allowed or not.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE - if supported
  FALSE - if not supported

SIDE EFFECTS
  None


===========================================================================*/

boolean hdralmp_check_ue_initiated_redir_allowed( void )
{
   if(hdralmp.lte_redir_efs_info.ue_initiated_redir == FALSE)  /* Check if UE initiated D2L redirection is enabled in NV */
   {
       HDR_MSG_PROT (MSG_LEGACY_MED, "UE initiated D2L redirection is not supported");
       return FALSE;
   }
   return TRUE;
}

/*=============================================================================

FUNCTION HDRALMP_SET_HDR_TO_LTE_REDIR_MEAS_IN_PROGRESS

DESCRIPTION
  This function sets if LTE measurements for redirection are in progress (or) not

DEPENDENCIES
  None.

PARAMETERS
  value - value to be set TRUE (or) FALSE

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdralmp_set_hdr_to_lte_redir_meas_in_progress(boolean value)
{
  /* HDR recieves call end with reason, to measure LTE means DLR redirection is in progress */
  if(hdralmp_check_ue_initiated_redir_allowed() == TRUE)
  {
     hdralmp.redir_meas_in_progress = value;
     HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: redir_meas_in_progress = %d",hdralmp.redir_meas_in_progress);
     hdridle_set_first_hdr_to_lte_redir_meas(TRUE);
  }
  else
  {
      HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: redirection is disabled !!");
  }
}


/*=============================================================================

FUNCTION HDRALMP_CHECK_OK_TO_REDIR_TO_LTE

DESCRIPTION
  This function checks if it is ok to redirect to LTE. 

DEPENDENCIES
  None.

PARAMETERS
  d2l_rat_pri_list - the RAT priority list to be given in redirection command
                           if D2L redirection is allowed.
  supported_band_index - LTE supported index, for retrieving band info 
RETURN VALUE
  TRUE is ok to redirect to LTE. Otherwise FALSE.

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdralmp_check_ok_to_redir_to_lte
(
  sys_rat_pri_list_info_s_type *d2l_rat_pri_list,
  uint16 *supported_band_index
)
{
  sys_rat_pri_list_info_s_type rat_pri_list;
    /* RAT priority list */
  boolean lte_is_supported = FALSE;
   /* Whether LTE is supported or not */
  boolean lte_full_srv_allowed = FALSE;
   /* Whether LTE full srv allowed in SD or not */
  uint8 i = 0;
   /* Loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(hdralmp_check_ue_initiated_redir_allowed() == FALSE)
  {
     HDR_MSG_PROT( MSG_LEGACY_MED, "D2L redirection not enabled ");
     return FALSE;
  }

  rat_pri_list = sd_misc_get_rat_priority_list();
  for (i = 0; i < rat_pri_list.num_items; i++)
  {
    if (rat_pri_list.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      lte_is_supported = TRUE;
      *supported_band_index = i; /* Note the LTE supported index, for retrieving band info later */
      break;
    }
  }

  lte_full_srv_allowed = sd_misc_is_lte_full_srv_allowed();

  if ( !lte_is_supported ||
       !lte_full_srv_allowed )
  {
    HDR_MSG_PROT_2( MSG_LEGACY_MED, 
                    "GWL service not available (%d) or LTE not in RAT pri list (%d)",
                    lte_full_srv_allowed, lte_is_supported );
    return FALSE;
  }

  if ( d2l_rat_pri_list )
  {
    *d2l_rat_pri_list = rat_pri_list;
  }

  return TRUE;
} /* hdralmp_check_ok_to_redir_to_lte() */


/*===========================================================================

FUNCTION HDRALMP_CHECK_EARFCN_SUPPORTED

DESCRIPTION
 This function checks if the passed earfcn is supported in the AT LTE band.
  
DEPENDENCIES

PARAMETERS
  earfcn - earfcn to be checked for supported

RETURN VALUE
  TRUE - if earfcn is supported
  FALSE - earfcn is not supported

SIDE EFFECTS
  None

===========================================================================*/

boolean hdralmp_check_earfcn_supported (lte_earfcn_t earfcn)
{

	boolean keep_earfcn = TRUE;
	  /* Loop index */
	/* Whether a freq is pruned because of its associated PLMNs are forbidden */
	boolean tdd_prune_required = lte_rrc_get_tdd_prune_required();


	if ( lte_rrc_earfcn_is_supported ( 
							earfcn,
#if (LTE_BAND_NUM == 256)
                            hdralmp.lte_redir_info.rat_pri_list.priority_list_info[hdralmp.lte_redir_info.lte_supported_band_index].band_cap.lte_band_cap))
#else
                            hdralmp.lte_redir_info.rat_pri_list.priority_list_info[hdralmp.lte_redir_info.lte_supported_band_index].band_cap))

#endif /* (LTE_BAND_NUM == 256) */
	{
		   /* Remove LTE TDD frequency if required */
		   if ( tdd_prune_required && lte_rrc_cell_is_tdd( 
					  earfcn ))
		   {
			 keep_earfcn = FALSE;
			 HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
							 "Removed LTE TDD freq = %d from redirect list",
							 earfcn);
		   }
		   else
		   {
		   			  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, 
							 "Keep LTE freq = %d",
							 earfcn);
			 keep_earfcn = TRUE;
		   }
	}
	else
	{
		   keep_earfcn = FALSE;
		   HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
						   "Removed unsupported LTE freq = %d from redirect list",
						   earfcn);
	}
	return keep_earfcn;


}


/*===========================================================================

FUNCTION HDRALMP_LTE_NSET_UPDATE_REDIR

DESCRIPTION
  This function gets the needed info to pass measurement request to L1.

DEPENDENCIES
  None.

PARAMETERS
  ue_initiated_redir - boolean value indicating if UE initiated redirection is set.
  item_ptr - Pointer to the DSM item chain holding the message.

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/

void hdralmp_lte_nset_update_redir(void)
{
#if (LTE_BAND_NUM == 256)
  sys_lte_band_mask_e_type lte_band_pref_static = SYS_LTE_BAND_MASK_CONST_NONE; 
#else
  sys_band_mask_type lte_band_pref_static = 0;
#endif /* (LTE_BAND_NUM == 256) */
  hdrsrch_lte_nset_update_params_type lte_nset_update_params;
  boolean earfcn_to_be_included = FALSE;
  sys_plmn_list_s_type plmn_prl;
  uint8 i=0;
  uint8 l=0;
  uint8 j=0;
  uint8 k=0;
  uint8 idx=0;
  struct fs_stat  sbuf;
  int fd = -1;
  int read_errno=0;
  /* EFS file descriptor */
  uint8 err = E_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	
  memset(&hdralmp.lte_redir_info, 0, sizeof(hdralmp_lte_redir_info_type));
  memset(&lte_nset_update_params,0, sizeof(lte_nset_update_params));
  
  /* Trigger measurement only if LTE is supported */
  if (hdralmp_check_ok_to_redir_to_lte(&hdralmp.lte_redir_info.rat_pri_list, 
                                       &hdralmp.lte_redir_info.lte_supported_band_index) != TRUE)

  {
     HDR_MSG_PROT_1 (MSG_LEGACY_MED, 
                     "Measurement request Ignore: Redirection is not supported. ehrpd status %d",
                     hdrmrlpc_current_personality_is_ehrpd());
     return;
  }	

  /* Get higher priority PLMNID's from SD */				 
  if(sd_misc_get_hdr_more_pref_plmn_list(&plmn_prl) == TRUE)
  {
     HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                  "HDR DLR:Retrieved higher priority PLMNID!!");
  }
  else
  {
     HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                  "HDR DLR:Retrieval of higher priority PLMNID FAILED. Ignoring D2L meas!!");
     return;
  }
				 
  /* Filter EARFCN only if the PLMNID associated is in the high priority PLMNID list */				 
  for(i = 0; (i < hdralmp.lte_rrc_acq_db_earfcn.num_earfcn_count) && (lte_nset_update_params.num_eutra_freqs < HDRSRCH_LTE_FREQ_ARR_SIZE); i++)
  {
     HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDR DLR: LTE ACQ DB Earfcn value %d",
                                      hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn);
				 
     earfcn_to_be_included = FALSE;
     for(j = 0; j < hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].plmn_list.num_plmns;j++)
     {
       for(k=0; k < plmn_prl.length; k++)
       {
         if(sys_plmn_match(plmn_prl.plmn[k],hdrovhd_lte_rrc_plmn_to_sys_plmn_id(hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].plmn_list.plmn[j])))
         {
           earfcn_to_be_included = TRUE;
           break;
         }
       }
       if(earfcn_to_be_included == TRUE)
       {
         if(hdralmp_check_earfcn_supported(hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn) != TRUE)
         {
           HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                           "HDR DLR: LTE ACQ DB Earfcn not supported %d",
                            hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn );
           earfcn_to_be_included = FALSE;
         }
         break;
       }
     }
						 
     if(earfcn_to_be_included == TRUE)
     {
         for(idx=0;idx<lte_nset_update_params.num_eutra_freqs;idx++)              
         {    
           if(hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn ==
                  lte_nset_update_params.earfcn_params_arr[idx].earfcn)
           {
              /* earfcn already included in the final list */
              earfcn_to_be_included = FALSE;
              break;
           }
         }
         if(earfcn_to_be_included == TRUE)
         {
		    lte_nset_update_params.earfcn_params_arr[lte_nset_update_params.num_eutra_freqs++].earfcn = 
			                                 hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn;
            HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDR DLR: LTE ACQ DB Earfcn to be included %d",
			                                 hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn);
         }
         else
         {
            HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDR DLR: Skip already included Earfcn %d",
                                             hdralmp.lte_rrc_acq_db_earfcn.earfcn_list[i].earfcn);
         }
     }
   }
				 
   HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                  "HDR DLR:earfcn_cnt %d lte_is_in_home %d",
                   lte_nset_update_params.num_eutra_freqs, 
                   hdralmp.lte_cpfl_info.lte_is_in_home);
				 
   if(hdralmp_check_lte_in_home() != TRUE)
   {
      hdralmp.lte_cpfl_info.lte_is_in_home = FALSE;
   }
				  
   /* hard code this value irrespective of PM. To be removed later */
   hdralmp.lte_cpfl_info.lte_is_in_home = TRUE;
	
				   
   HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
                  "HDR DLR:updated earfcn_cnt %d lte_is_in_home %d CPFL valid %d",
                   lte_nset_update_params.num_eutra_freqs, 
                   hdralmp.lte_cpfl_info.lte_is_in_home,
                   hdralmp.lte_cpfl_info.is_cpfl_valid);
				 
   if((lte_nset_update_params.num_eutra_freqs < HDRSRCH_LTE_FREQ_ARR_SIZE) && 
      (hdralmp.lte_cpfl_info.lte_is_in_home == TRUE))
   {
				 
     if(hdralmp.lte_cpfl_info.is_cpfl_valid != TRUE)
     {
       //Reinitialize
       memset(&sbuf, 0x00, sizeof(struct fs_stat)); 
       memset(hdralmp.lte_cpfl_info.efs_earfcns_list,0x0,sizeof(hdralmp.lte_cpfl_info.efs_earfcns_list));
									   
       /* Get the size of PFL file from EFS */
       if ((mcfg_fs_stat(HDRALMP_CPFL_FREQ_LIST_EFS_FILENAME,
                         &sbuf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0) !=
                         MCFG_FS_STATUS_OK) || (sbuf.st_size == 0))
       {  
         hdralmp.lte_cpfl_info.num_pref_earfcns = 0;
         HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"MCFG_READ: No pref frequency list from EFS size %d",sbuf.st_size);
       }
       else
       {
         /* We will get the first 20 earfcns from the EFS */
         if (sbuf.st_size > sizeof(hdralmp.lte_cpfl_info.efs_earfcns_list))
         {
           HDR_MSG_PROT(MSG_LEGACY_HIGH, "MCFG_READ: Size more than 20 earfcns. limiting it");
           sbuf.st_size = sizeof(hdralmp.lte_cpfl_info.efs_earfcns_list);
         }
         HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MCFG_READ: File size %d", sbuf.st_size);
				 
         /* Read the PFL from EFS */
         if ((read_errno = mcfg_fs_read(HDRALMP_CPFL_FREQ_LIST_EFS_FILENAME,
                                        hdralmp.lte_cpfl_info.efs_earfcns_list,sbuf.st_size, MCFG_FS_TYPE_EFS,
                                        MCFG_FS_SUBID_0)) != MCFG_FS_STATUS_OK)
         { 	
           hdralmp.lte_cpfl_info.num_pref_earfcns = 0;
           HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"MCFG_READ: pref frequency list EFS read failed %d",read_errno);
         }
         else
         {
           /*Get the num of PFL earfcns in EFS */
           hdralmp.lte_cpfl_info.num_pref_earfcns =  (uint32)sbuf.st_size/sizeof(lte_efs_earfcn_t);
				 
           for (l = 0; l < hdralmp.lte_cpfl_info.num_pref_earfcns; l++)
           {
              HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MCFG_READ: earfcn read from cpfl %d",
			  	                               (lte_earfcn_t)hdralmp.lte_cpfl_info.efs_earfcns_list[l]);
           }
				 
           /* We will check the earfcns validation later when we used */
           HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "MCFG_READ: %d pref earfcn list from EFS",hdralmp.lte_cpfl_info.num_pref_earfcns);
				 
         } /* mcfg_fs_read */
       }
       hdralmp.lte_cpfl_info.is_cpfl_valid = TRUE;
	}
				 
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
	               "HDR DLR:CPFL valid %d",hdralmp.lte_cpfl_info.is_cpfl_valid);
    /* Check if CPFL earfcn already included in the to-be measured list */
    for(i = 0 ; ((i < hdralmp.lte_cpfl_info.num_pref_earfcns) && (lte_nset_update_params.num_eutra_freqs < HDRSRCH_LTE_FREQ_ARR_SIZE)) ; i++)
    {
      earfcn_to_be_included = TRUE;
      /* Check if earfcn is already included in the to-be measured list */
      for(j = 0 ; j < lte_nset_update_params.num_eutra_freqs; j++)
      {
        if(hdralmp.lte_cpfl_info.efs_earfcns_list[i] == 
           lte_nset_update_params.earfcn_params_arr[j].earfcn)
        {
          /* earfcn already in the list */
          earfcn_to_be_included = FALSE;
          break;
        }
      }
      if(earfcn_to_be_included == TRUE)
      {
         if(hdralmp_check_earfcn_supported(hdralmp.lte_cpfl_info.efs_earfcns_list[i]) != TRUE)
         {
           HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDR DLR: CPFL Earfcn not supported %d",
                                            hdralmp.lte_cpfl_info.efs_earfcns_list[i]);
         }
         else
         {
           lte_nset_update_params.earfcn_params_arr[lte_nset_update_params.num_eutra_freqs].earfcn = 
           hdralmp.lte_cpfl_info.efs_earfcns_list[i];
				 
           HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDR DLR: CPFL added Earfcn value %d",
                                            lte_nset_update_params.earfcn_params_arr[lte_nset_update_params.num_eutra_freqs].earfcn);
           lte_nset_update_params.num_eutra_freqs++;
         }
      }
    }
   }
				 
   if(lte_nset_update_params.num_eutra_freqs == 0)
   {
     HDR_MSG_PROT(MSG_LEGACY_HIGH, "No EARFCN available to measure");
	 /* Might need to inform DS here via indication ??? */
     return;
   }
	
   /* Inform SRCH the measurement request info */
   HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: hdrlmac_lte_nset_update  Request to L1 with earfcn count %d",
                                    lte_nset_update_params.num_eutra_freqs );

#if (LTE_BAND_NUM == 256)
   lte_band_pref_static = hdralmp.lte_redir_info.rat_pri_list.priority_list_info[hdralmp.lte_redir_info.lte_supported_band_index].band_cap.lte_band_cap;
#else
   lte_band_pref_static = hdralmp.lte_redir_info.rat_pri_list.priority_list_info[hdralmp.lte_redir_info.lte_supported_band_index].band_cap;
#endif /* (LTE_BAND_NUM == 256) */			


  for(i=0 ; i < lte_nset_update_params.num_eutra_freqs ; i++)
  {
     lte_nset_update_params.earfcn_params_arr[i].thresh_x = hdralmp.lte_redir_efs_info.threshx_trig;
     lte_nset_update_params.earfcn_params_arr[i].thresh_xq = hdralmp.lte_redir_efs_info.threshxq_trig;	
	 lte_nset_update_params.earfcn_params_arr[i].earfcn_priority = 1;
  }
   lte_nset_update_params.priority_incl = TRUE;
   lte_nset_update_params.serving_priority = 0;
   	
   lte_nset_update_params.thresh_serving = hdralmp.lte_redir_efs_info.serving_thresh_trig;
   lte_nset_update_params.rsrq_parameters_incl = TRUE;
   lte_nset_update_params.qual_min_eutra_common = 31;
   lte_nset_update_params.max_reselect_timer_incl = TRUE;
   lte_nset_update_params.max_reselect_timer = 5; /* in 100 ms */
   lte_nset_update_params.backoff_timer_incl = TRUE;
   lte_nset_update_params.min_backoff = 1;
   lte_nset_update_params.max_backoff = 20; /* in 100 ms */
   lte_nset_update_params.per_earfcn_params_incl = FALSE;
   lte_nset_update_params.rx_level_min_eutra_common = 96;
   lte_nset_update_params.rx_lev_min_eutra_offset_common_incl = TRUE;
   lte_nset_update_params.rx_lev_min_eutra_offset_common = 0;
   lte_nset_update_params.qual_min_eutra_offset_common_incl = TRUE;
   lte_nset_update_params.qual_min_eutra_offset_common = 0;
   lte_nset_update_params.pe_max_common = 0;

   hdrlmac_lte_nset_update( &lte_nset_update_params, lte_band_pref_static );

}


/*===========================================================================

FUNCTION HDRALMP_SEND_REDIRECT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to redirect to list of LTE
 freq and cell ID.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdralmp_send_redirect_req_to_lte(void)
{

 lte_rrc_eHRPD_redir_req_s redir_req;
    /* Message to request redirectt to LTE. */
  errno_enum_type estatus=E_SUCCESS;
    /* Error status */
  uint8 i=0,j=0;
  sys_plmn_list_s_type plmn_prl;	
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT (MSG_LEGACY_HIGH, "HDR DLR: Sending redirect request to LTE");

#ifdef FEATURE_HDR_DUAL_SIM
	HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDR DLR: Sending redirect request to LTE :%d inst",
						SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
	msgr_init_hdr_inst(&redir_req.msg_hdr, MSGR_HDR_CP,
					LTE_RRC_eHRPD_REDIR_REQ, SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
#else
	msgr_init_hdr ( &redir_req.msg_hdr, MSGR_HDR_CP,
					LTE_RRC_eHRPD_REDIR_REQ );
#endif


  redir_req.redir_earfcn_list.redir_info_list_count = hdralmp.lte_redir_info.earfcn_cnt;
  redir_req.nas_params.num_req_plmn = 0; /* exclusively set to 0 */
  	
  /* Fill the LTE Redirect request structure */
  for(i = 0 ; i < hdralmp.lte_redir_info.earfcn_cnt; i++)
  {
    /* Fill earfcn */
    redir_req.redir_earfcn_list.redir_info[i].earfcn = hdralmp.lte_redir_info.earfcn[i];

    /* Fill associated plmnid */
    if(hdralmp.lte_redir_info.earfcn_list[i].plmnid_incl == TRUE)
    {
       for(j = 0; (j < hdralmp.lte_redir_info.earfcn_list[i].plmnid_cnt)&&(redir_req.nas_params.num_req_plmn <LTE_RRC_IRAT_MAX_ORNL_PLMN); j++)
       {
           if(hdralmp.lte_redir_info.earfcn_list[i].plmnid_valid == TRUE)
           {
              /* only copy valid plmnids */
              redir_req.nas_params.requested_plmn[redir_req.nas_params.num_req_plmn] = 
                                                   hdralmp.lte_redir_info.earfcn_list[i].plmnid[j];
              redir_req.nas_params.num_req_plmn++;
           }
       }
    }

     /* Fill associated pci */
     if (hdralmp.lte_redir_info.earfcn_list[i].pci_incl == TRUE)
     {
        redir_req.redir_earfcn_list.redir_info[i].pci_count = hdralmp.lte_redir_info.earfcn_list[i].pci_cnt;                 
        for ( j = 0; j < hdralmp.lte_redir_info.earfcn_list[i].pci_cnt; j++ )
        {
          redir_req.redir_earfcn_list.redir_info[i].pci_list[j] = hdralmp.lte_redir_info.earfcn_list[i].pci[j];
        }
     }/* if (redir_info_ptr->earfcn_list[i].pci_incl) */
  }

  for(i = 0 ; i < redir_req.redir_earfcn_list.redir_info_list_count ; i++)
  {
	  HDR_MSG_PROT_1( MSG_LEGACY_MED, "HDR DLR: Filled earfcn value %d in the report",
	  	          redir_req.redir_earfcn_list.redir_info[i].earfcn);
  }

  redir_req.nas_params.rat_pri_list = hdralmp.lte_redir_info.rat_pri_list;
  
    HDR_MSG_PROT_2( MSG_LEGACY_MED, "HDR DLR: Filling RAT PRI LIST for items %d %d",
  	               hdralmp.lte_redir_info.rat_pri_list.num_items, hdralmp.lte_redir_info.rat_pri_list.next_acq_sys_index);
  
  /* get additional PLMNID's from SD */
  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: Calling SD API sd_misc_get_hdr_more_pref_plmn_list");
  
  if(sd_misc_get_hdr_more_pref_plmn_list(&plmn_prl) == TRUE)
  {
     for(i = 0; (i < plmn_prl.length)&&((redir_req.nas_params.num_req_plmn + i) <LTE_RRC_IRAT_MAX_ORNL_PLMN); i++)
     {
        redir_req.nas_params.requested_plmn[redir_req.nas_params.num_req_plmn + i]=
        plmn_prl.plmn[i];
     }
     redir_req.nas_params.num_req_plmn += i;
   }

  redir_req.nas_params.network_select_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
  
  estatus = msgr_send ( &redir_req.msg_hdr, sizeof ( lte_rrc_eHRPD_redir_req_s ) );
  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: redirection request sent to LTE");  

  hdralmp.lte_redir_info.redir_cmd_is_sent = TRUE;  
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "HDR DLR: hdralmp_hdr_to_lte_redir_cmd_is_sent %d",
  	hdralmp.lte_redir_info.redir_cmd_is_sent);  
#if 0  
  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: Clear redirection info");  
  hdralmp_clear_hdr_to_lte_redir(); /* Clear all the redirect related info upon abort confirm */

  HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: Reacquire HDR !!!");       
  hdralmp_reacquire( HDRHAI_ABORT_D2L_FAILURE, HDRHAI_HDRMC);
#endif

  ASSERT ( estatus == E_SUCCESS );
}


/*===========================================================================

FUNCTION HDRALMP_SEND_REDIRECT_ABORT_REQ_TO_LTE

DESCRIPTION
 This function sends a request to LTE RRC to abort redirection.
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  hdralmp_send_redirect_abort_req_to_lte(void)
{

	  lte_rrc_eHRPD_abort_redir_req_s abort_redir_req;
		/* Message to request abort reselect to LTE. */
	  errno_enum_type estatus;
		/* Error status */
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	
	  HDR_MSG_PROT (MSG_LEGACY_HIGH, "IRAT: Sending abort redirect request to LTE");
	
#ifdef FEATURE_HDR_DUAL_SIM
	  HDR_MSG_PROT_1 (MSG_LEGACY_HIGH, "HDR DLR: Sending redirect abort request to LTE :%d inst",
					  SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
						  
	  msgr_init_hdr_inst(&abort_redir_req.msg_hdr, MSGR_HDR_CP,
					  LTE_RRC_eHRPD_ABORT_REDIR_REQ, SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
#else
	  msgr_init_hdr ( &abort_redir_req.msg_hdr, MSGR_HDR_CP,
					  LTE_RRC_eHRPD_ABORT_REDIR_REQ );
#endif

	  estatus = msgr_send ( &abort_redir_req.msg_hdr, 
							sizeof ( lte_rrc_eHRPD_abort_redir_req_s ) );
	
	  ASSERT ( estatus == E_SUCCESS );

	  hdralmp.lte_redir_info.redir_cmd_is_sent = FALSE;
	  hdralmp.lte_redir_info.redir_abort_cmd_is_sent = TRUE;
	  
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HDR DLR: abort cmd sent %d redir_cmd_is_sent %d",
  	hdralmp.lte_redir_info.redir_abort_cmd_is_sent,
  	hdralmp.lte_redir_info.redir_cmd_is_sent);  
}

/*===========================================================================

FUNCTION HDRALMP_CLR_HDR_TO_LTE_REDIR_ABORT_CMD_IS_SENT

DESCRIPTION
 This function clears the abort redirection cmd sent flag value
 
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_clr_hdr_to_lte_redir_abort_cmd_is_sent( void )
{
  hdralmp.lte_redir_info.redir_abort_cmd_is_sent = FALSE;
}



/*===========================================================================

FUNCTION HDRALMP_CHECK_LTE_IN_HOME

DESCRIPTION
  This function checks with policyman to determine whether
  LTE service is in home network (or) roam

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - if able to retrive info from policyman
  FALSE - not able to retrieve info from policyman

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_check_lte_in_home(void)
{
/*
  policyman_item_id_t	 id = POLICYMAN_ITEM_FREQ_LIST;
  policyman_item_collection_t const	*pCollection;
  policyman_freq_list_type_t freq_list_type;


	boolean earfcn_to_be_included = FALSE;
    lte_efs_earfcn_t  efs_earfcns_list[HDRALMP_CPFL_MAX_PREF_FREQ_LIST_ENTRIES];
	uint8 num_pref_earfcns = 0;
    struct fs_stat  sbuf;
    int fd = -1;
	int read_errno=0;
	uint8 l=0;
     boolean ret_val = FALSE;
    /* EFS file descriptor 
	

     HDR_MSG_PROT(MSG_LEGACY_HIGH,"hdralmp_check_lte_in_home called. Calling policyman_get_items_block_msim !!");  
  
	 if (POLICYMAN_SUCCEEDED(policyman_get_items_block_msim((policyman_item_id_t const *)&id,
														1,
														&pCollection)))
	 {

	   if (POLICYMAN_SUCCEEDED(policyman_get_freq_list_type(pCollection->pItems[0],&freq_list_type)))
	   {
          HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"PM Update the frequency list type:freq_list_type=%d",freq_list_type);   
					  
		  if(freq_list_type == POLICYMAN_FREQ_LIST_CPFL)
		  {
			  hdralmp.lte_cpfl_info.lte_is_in_home = TRUE;
		  }
		  else
		  {
              hdralmp.lte_cpfl_info.lte_is_in_home = FALSE;
		  }
          HDR_MSG_PROT_1(MSG_LEGACY_HIGH,"PM Update the frequency list type:hdralmp.lte_is_in_home=%d",hdralmp.lte_cpfl_info.lte_is_in_home);   
                  ret_val = TRUE;
	   }
	   else
	   {
		  HDR_MSG_PROT(MSG_LEGACY_HIGH,"policyman_get_freq_list_type : freq list type get failed");
	   }

	 }
		
	policyman_item_collection_release(pCollection); 
	*/
        
        return TRUE;	
}

#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

/*===========================================================================

FUNCTION HDRALMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to ALMP.

DEPENDENCIES
  None

PARAMETERS
  almp_cmd_ptr - Pointer to the command for ALMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_process_cmd( hdralmp_cmd_type * almp_cmd_ptr )
{
  /* Current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );
  /* Next state */
  hdralmps_state_enum_type next_state = cur_state;
  /* Why we are leaving the state (may not be an abort) */
  hdrhai_abort_reason_enum_type abort_reason = HDRHAI_ABORT_UNSPECIFIED;
  /* Where to go when connection close completes */
  hdralmps_state_enum_type pending_state = hdralmps_get_pending_state();

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Current tunnel mode value */
  hdrhai_tunnel_mode_type tunnel_mode_value;
#endif /* FEATURE_LTE_TO_HDR_OH */

#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_LTE_TO_HDR_OH)
  sd_ss_act_e_type sd_act;
#endif /* defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_LTE_TO_HDR_OH) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_CMD( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, almp_cmd_ptr->name,
    almp_cmd_ptr->sender, cur_state );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( almp_cmd_ptr->name )
  {
    case HDRALMP_ACTIVATE_CMD:        /* Starts HDR mode operation */
      ASSERT( cur_state == HDRALMPS_INACTIVE );
      if ( cur_state == HDRALMPS_INACTIVE ) /*lint !e774 always TRUE */
      {
        if (hdralmp.deactivate.deactivation_cmd_is_pending && 
            hdralmp.deactivate.trans_id_is_valid) 
        {
            HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                "ALMP_ACTIVATE_CMD rcv while DEACTIVATE_CNF pending, trans=%d",
                hdralmp.deactivate.trans_id);
        }
        hdralmp.reselect_in_progress = FALSE;
        hdralmp.deactivate.deactivation_cmd_is_pending = FALSE;
        hdralmp.deactivate.trans_id_is_valid = FALSE;
        next_state = HDRALMPS_INIT;
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Wrong State!" );
      }
      break;

    case HDRALMP_DEACTIVATE_CMD:        /* Ends HDR mode operation */

#ifdef FEATURE_LTE_TO_HDR_OH
      if (hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL))
      {
        /* When HDR is in tunnnel mode unless HDRMC receives an explicit MMOC 
           deactivate, it will continue to stay in online state */
        if (almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate)
        {
          HDR_MSG_PROT_1(MSG_LEGACY_MED, "Recv MMoC Deact in TUNNEL mode, send DeactCnf id=%d",
                                          almp_cmd_ptr->params.deactivate.trans_id);

          /* No longer operating in HDR mode. Signal HDRMC mode controller to exit */
          (void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );
          hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );

          mmoc_rpt_prot_deactd_cnf( almp_cmd_ptr->params.deactivate.trans_id );
#ifdef FEATURE_HDR_DEBUG_INFO
		  hdrmc_log_sent_rpt(almp_cmd_ptr->params.deactivate.trans_id, HDR_MMOC_RPT_PROT_DEACTD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
        }
        else
        {
          /* When AT is in tunnel mode ALMP is in INIT state. In this case 
             we just have to deactivate INIT protocol and move ALMP to 
             inactive state */
          HDR_MSG_PROT (MSG_LEGACY_MED, "Leaving tunnel mode");
        }

        /* In ACT HO, ALMP can be in INIT state while IDLE is active.
           Need to deactivate IDLE */
        if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() &&
             ( hdridles_get_state() != HDRIDLES_INACTIVE_STATE ) &&
             ( hdralmps_get_state() != HDRALMPS_IDLE ) )
        {
          hdridle_deactivate(HDRHAI_ABORT_EXIT_TUNNEL,
                             HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
        }
      }
      else 
#endif /* FEATURE_LTE_TO_HDR_OH */
      if (!hdralmp.deactivate.deactivation_cmd_is_pending)
      {  
        if ((almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate) && 
            (cur_state == HDRALMPS_INACTIVE ))
        {
          /* ALMP is already in INACTIVE state and there is no deactivation pending
           * We can report deactivate confirmation immediately to CM
           */

          HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Recv MMoC Deact when INACTIVE, send DeactCnf id=%d",
                          hdralmp.deactivate.trans_id);
          mmoc_rpt_prot_deactd_cnf( almp_cmd_ptr->params.deactivate.trans_id );
#ifdef FEATURE_HDR_DEBUG_INFO
		  hdrmc_log_sent_rpt(almp_cmd_ptr->params.deactivate.trans_id, HDR_MMOC_RPT_PROT_DEACTD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
        }
#ifdef FEATURE_HDR_TO_LTE
        else if (cur_state == HDRALMPS_SUSPEND)
        {
          if (almp_cmd_ptr->params.deactivate.reason != HDRHAI_ABORT_IRAT_TO_GWL)
          {
            HDR_MSG_PROT( MSG_LEGACY_HIGH,
                          "D2L in progress, HDR is deactivated" );
          }
#ifdef FEATURE_LTE_TO_HDR_OH
          else
          {
            HDR_MSG_PROT( MSG_LEGACY_MED, "Successfully reselected/redirected to LTE" );

            /* Successfully reselect to LTE, set reselect flag in AMP so it
             * knows that it need to restore prior sessoin after receiving 
             * PreRegAllowed = 1 */
            if ( hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
                   == HDRSCP_IRAT_SAP )
            {
              hdramp_set_hdr_to_lte_resel( TRUE );
            }
          }
#endif /* FEATURE_LTE_TO_HDR_OH */

          /* No longer operating in HDR mode. Signal HDRMC mode controller to exit */
          (void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );
          hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );
		  hdralmp.lte_resel_info.resel_cmd_is_sent = FALSE;
		  hdralmp.lte_resel_info.resel_abort_cmd_is_sent = FALSE;

          /* ALMP in suspend state and there is no deactivation pending
           * We can report deactivate confirmation immediately to CM
           */
          if (almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate) 
          {
            HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                            "Recv MMoC Deact when SUSPEND, send DeactCnf id=%d",
                            hdralmp.deactivate.trans_id );
            mmoc_rpt_prot_deactd_cnf( almp_cmd_ptr->params.deactivate.trans_id );
#ifdef FEATURE_HDR_DEBUG_INFO
            hdrmc_log_sent_rpt(almp_cmd_ptr->params.deactivate.trans_id, HDR_MMOC_RPT_PROT_DEACTD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
          }
          else
          {
            HDR_MSG_PROT( MSG_LEGACY_MED, "Auto deact in SUSPEND, send DeactCnf" );
            mmoc_rpt_prot_auto_deactd_ind( PROT_DEACT_ACQ_OTHERS, 
                                           PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
            hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
          }

          next_state = HDRALMPS_INACTIVE;
        }
#endif /* FEATURE_HDR_TO_LTE */
        else
        {
          abort_reason = almp_cmd_ptr->params.deactivate.reason;
          hdralmp.deactivate.deactivation_cmd_is_pending = TRUE;
          hdralmp.deactivate.trans_id_is_valid = 
            almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate;
          hdralmp.deactivate.trans_id = almp_cmd_ptr->params.deactivate.trans_id;
        }
      }
      else
      {
        if (hdralmp.deactivate.trans_id_is_valid &&
            almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate ) 
        {
            /* MMoC should not queue another DEACTIVATE while CNF for first
               is not sent yet */
            ERR_FATAL("Double deactivate from MMoC,prev trans=%d,cur trans=%d",
                      hdralmp.deactivate.trans_id,
                      almp_cmd_ptr->params.deactivate.trans_id, 0);
        }
        /* If an MMoC expilit deactivation command is already pending, don’t overwrite the
         * transaction id because we need to use it to report the deactivation confirmation to MMoC.
         * On the other hand, if an MMoC explicit deactivation comes after auto deactivation, we need
         * to save the transaction id.
         */
        else if ((!hdralmp.deactivate.trans_id_is_valid) ||
            almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate)
        {
          abort_reason = almp_cmd_ptr->params.deactivate.reason;
          hdralmp.deactivate.trans_id_is_valid = 
            almp_cmd_ptr->params.deactivate.explicit_mmoc_deactivate;
          hdralmp.deactivate.trans_id = 
            almp_cmd_ptr->params.deactivate.trans_id;		
			
#ifdef FEATURE_HDR_TO_LTE
          /* During D2L reselection, HDRCP is still waiting HDRSRCH to be
           * deactivated, the same time HDR is deactivated by CM. In this 
           * case, reselection command is not sent to LTE yet. HDR goes 
           * offline without D2L. 
           */
          if (cur_state == HDRALMPS_SUSPEND)
          {
            hdralmp_set_pending_abort_resel( FALSE );
          }
#endif /* FEATURE_HDR_TO_LTE */

        }
      }

      if ((cur_state == HDRALMPS_CONNECTED) &&
          (pending_state != HDRALMPS_CONNECTED))
      {
        /* If we have already closing the connection gracefully, just change
         * the pending state to be HDRALMPS_INACTIVE instead of whatever it was
         * before, and change state when the connection close is completed.
         */
        hdralmps_change_pending_state( HDRALMPS_INACTIVE );
      }
      else
      {
        /* If connected this will cause a rude close (no close message) */
        next_state = HDRALMPS_INACTIVE;
      }
      break;

    case HDRALMP_OPEN_CONNECTION_CMD:    /* Open a connection */

#ifdef FEATURE_LTE_TO_HDR_OH
      if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL) )
      {
        tunnel_mode_value = hdrsap_get_current_tunnel_mode_value( 
          HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );

        if ( ( tunnel_mode_value == HDRHAI_TUNNEL_MODE_ACTIVE_HO ) &&
             ( almp_cmd_ptr->params.open_connection.open_reason !=
                 HDRHAI_OPEN_USER_REQ ) )
        {
          hdridle_open_connection(
            almp_cmd_ptr->params.open_connection.open_reason,
            HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
        }
        else
        {
          hdrsap_open_connection(
            almp_cmd_ptr->params.open_connection.open_reason,
            HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
        }
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        // Allow origination command to be received while connected.
        if (almp_cmd_ptr->params.open_connection.open_reason ==
            HDRHAI_OPEN_USER_REQ
            && !hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
        {
          /* We do not want to honor a user request to open a connection
           * until we have negotiated a session, and a stream for data.
           */
          ERR( "User orig req before stream",0,0,0 );

          hdrmc_send_orig_failure( CM_CALL_ORIG_ERR_ABORT );
        }
        else
        {
          if ( cur_state == HDRALMPS_IDLE )
          {
            hdridle_open_connection(
               almp_cmd_ptr->params.open_connection.open_reason,
               HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
          }
          else if ( cur_state == HDRALMPS_CONNECTED )
          {
            /* Need to initialize rlp for scenarios where the traffic
               channel is already up. This was initially done in the ds
               task. Moved it into hdrmc context  */
            hdrpac_mdm_sn_init(); 

            /* Need to spoof the origination, update the call id's
               and return back connected to CM! */
            HDR_MSG_PROT (MSG_LEGACY_HIGH,
                          "Orig Rxed while Connected. Return Connected");
            hdridle_send_call_connected_to_cm();
          }
          else
          {
            /* Reject any attempt to originate before acquiring the system.
             * CM must wait for SD to say OK_TO_ORIG, so this is an error.
            */
            ERR( "Orig Rxed in wrong state %d", cur_state, 0, 0 );
            hdrmc_send_orig_failure( CM_CALL_ORIG_ERR_ABORT );
          }
        }
      }
      break;

    case HDRALMP_CLOSE_CONNECTION_CMD:   /* Close the connection */

      abort_reason = almp_cmd_ptr->params.close_connection.reason;

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        if( abort_reason == HDRHAI_ABORT_RESELECT ||
            abort_reason == HDRHAI_ABORT_ERROR )
        {
          hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                   abort_reason );

          hdralmps_change_pending_state( HDRALMPS_IDLE );
        }
        else if (abort_reason != HDRHAI_ABORT_USER_RUDE_CLOSE)
        {
          /* this is a normal close */
          hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                   HDRHAI_ABORT_NORMAL_CLOSE );

          /* Our intention is to go to idle when the connection is closed. */
          hdralmps_change_pending_state( HDRALMPS_IDLE );
        }
        else
        {
          /* if the pending state is connected then change next state
             to idle otherwise use pening state as the next state because
             we may already in the processing of closing the connection
             and waiting the tx status of the close connection message
          */

          if (pending_state == HDRALMPS_CONNECTED)
          {
            next_state = HDRALMPS_IDLE;
          }
          else
          {
            next_state = pending_state;
          }
        }
      }
      else if (cur_state == HDRALMPS_IDLE)
      {
        if (abort_reason == HDRHAI_ABORT_MSG ) 
        {
          hdridle_abort_open_connection( abort_reason,
              HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
        }
        /* currently we do not need to worry about getting
           rude close while in the processing of setting
           up a call */
        else
        {
          hdridle_abort_open_connection( HDRHAI_ABORT_USER,
            HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
        }
      }
#ifdef FEATURE_LTE_TO_HDR_OH
      else if ( (cur_state == HDRALMPS_INIT) &&
                hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL) &&
                (hdrscp_get_current_subtype(HDRHAI_SIGNALING_ADAPTATION_PROTOCOL) 
                  == HDRSCP_IRAT_SAP) )
      {
        if (hdrscp_is_configuring() && 
           (almp_cmd_ptr->sender == HDRHAI_PACKET_APPLICATION_CONTROLLER) )
        {
          /* This should not happen. DS shouldn't trigger connection close
             when HDR session is still still not established yet */
          HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                "DS should not call connection close during session negotiation");
          HDRTRACE_CMD_IGNORED_STATE( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
            almp_cmd_ptr->name, almp_cmd_ptr->sender, cur_state );
        }
        else
        {
          hdrsap_close_connection(
            abort_reason, HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, FALSE );
        }
      }
#endif /* FEATURE_LTE_TO_HDR_OH */       
      else
      {
        HDRTRACE_CMD_IGNORED_STATE( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
          almp_cmd_ptr->name, almp_cmd_ptr->sender, cur_state );
      }
      break;

    case HDRALMP_RESET_CMD:
      /* This command is used if a session is closed, to make sure
       * any signaling related to the closed session is aborted.
       * By considering this like a system lost abort, CM will perform
       * the correct type of silent redial if a connection attempt
       * is in progress.
       */
      abort_reason = HDRHAI_ABORT_SYS_LOST;

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        /* This causes a rude close (no connection close message).
         * SLP already flushes any remaining signaling as part of the
         * connection close.
         */
         if (pending_state == HDRALMPS_CONNECTED)
         {
           next_state = HDRALMPS_IDLE;
         }
         else
         {
           next_state = pending_state;
         }
      }
      else if (cur_state == HDRALMPS_IDLE)
      {
	if(hdrrups_get_state() == HDRRUPS_CONNECTION_SETUP)
	{
          /* connection setup timeout, will deactivate RMAC immediately
             instead of waiting till RUP connection setup timer expiry */
	     hdrrup_process_conn_setup_timeout();
	}
	else
	{
	  /* If an access attempt is in progress when the session is closed,
	   * the access is no longer necessary.
	   */
          hdramac_deactivate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, FALSE);
	  hdramac_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);

	  /* By deactivating and reactivating the idle state protocol we
	   * ensure that if a connection setup was in progress it is
	   * canceled and the HDRIND_IDLE_CONNECTION_FAILED indication
	   * given so HMP will flush any pending RTC messages.
	   */
	  hdridle_deactivate( abort_reason,
	  HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
	  hdridle_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
	}/* hdrrups_get_state() == HDRRUPS_CONNECTION_SETUP */
      }
      else
      {
        HDRTRACE_CMD_IGNORED_STATE( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
          almp_cmd_ptr->name, almp_cmd_ptr->sender, cur_state );
      }
      break;

    case HDRALMP_REACQUIRE_CMD:    /* Return to init state and reacquire */
      abort_reason = almp_cmd_ptr->params.reacquire.reason;

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        if (pending_state == HDRALMPS_CONNECTED)
        {
          /* This results in a 'rude' closing,
           * with no connection close message being sent.
           */
          next_state = HDRALMPS_INIT;
        }
        else if (pending_state == HDRALMPS_IDLE)
        {
          /* If we were going to return to idle when the connection is closed,
           * go to init instead.
           */
          hdralmps_change_pending_state( HDRALMPS_INIT );
        }
      }
      else if (( cur_state == HDRALMPS_IDLE )
#ifdef FEATURE_HDR_TO_LTE
                || 
               ( cur_state == HDRALMPS_SUSPEND )
#endif /* FEATURE_HDR_TO_LTE */
              )
      {
#ifdef FEATURE_HDR_TO_LTE
        if ( ( abort_reason == HDRHAI_ABORT_D2L_FAILURE ) &&
             ( cur_state == HDRALMPS_SUSPEND ) )
        {
          /* reacquire due to D2L abort/failure */
          sd_act = sd_ss_ind_hdr_opr_sys_lost2( SD_SS_SYS_LOST_LTE_IRAT_FAILURE, 
                                                NULL );

          HDR_MSG_PROT_1( MSG_LEGACY_HIGH,
                          "D2L abort or failure, SD action: %d", sd_act );

          if ( sd_act != SD_SS_ACT_ACQ_HDR )
          {
            /* HDR already in SUSPEND state. Leave HDR online mode. */
            (void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );
            hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );
            HDR_MSG_PROT( MSG_LEGACY_HIGH, "Auto deact in SUSPEND, send DeactCnf" );
            mmoc_rpt_prot_auto_deactd_ind( PROT_DEACT_ACQ_OTHERS, 
                                           PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
            hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
            next_state = HDRALMPS_INACTIVE;
            break;
          }
        }
#endif /* FEATURE_HDR_TO_LTE */
        next_state = HDRALMPS_INIT;
      }
      else if ( cur_state == HDRALMPS_INIT )
      {
        /* Init state is already active, inform it that it must re-acquire */
        hdrinit_reacquire(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
        hdrind_give_ind( HDRIND_ALMP_REACQUIRING , NULL );
      }
      break;

    default:
      ERR_FATAL( "Unknown command", 0, 0, 0 );
      break;
  }

  if ( next_state != cur_state )
  {
    /* There may be some common event dependent processing to be done before
     * we change state.
     */
    hdralmp_change_state( abort_reason, next_state );
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdralmp_process_cmd() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to ALMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for ALMP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_process_ind( hdrind_ind_name_enum_type ind_name,
        hdrind_ind_data_union_type * ind_data_ptr )
{
  /* Current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );

  /* Next state */
  hdralmps_state_enum_type next_state = cur_state;

  /* Continue, acquire HDR, acquire IS-95, etc. */
  sd_ss_act_e_type sd_action = SD_SS_ACT_MAX;

  /* Why we are leaving the state (may not be an abort) */
  hdrhai_abort_reason_enum_type abort_reason = HDRHAI_ABORT_UNSPECIFIED;

  /* Reason connection was opened */
  hdrhai_open_reason_enum_type open_reason = HDRHAI_OPEN_UNSPECIFIED;

  /* The next state of ALMP (after we close the connection) */
  hdralmps_state_enum_type pending_state = hdralmps_get_pending_state();

sd_ss_sys_lost_e_type sys_lost_reason = SD_SS_SYS_LOST_UNKNOWN;

  /* Auto deactivation reason reported to MMOC */
  prot_deact_e_type auto_deact_reason;

#ifdef FEATURE_LTE_TO_HDR_OH
  hdrhai_tunnel_mode_type curr_tunnel_mode = 
      hdrsap_get_current_tunnel_mode_value (HDRHAI_ROUTE_UPDATE_PROTOCOL); 
#endif /* FEATURE_LTE_TO_HDR_OH */
  hdrerrno_enum_type err;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_IND( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, ind_name, cur_state );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( ind_name )
  {
    case HDRIND_INIT_NETWORK_ACQUIRED:
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "Network Acquired");
      ASSERT( cur_state == HDRALMPS_INIT );
      if ( cur_state == HDRALMPS_INIT )    /*lint !e774 always TRUE */
      {
        next_state = HDRALMPS_IDLE;
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
#ifdef FEATURE_LTE_TO_HDR
        if((hdrmc_lte_to_hdr_redir_is_in_progress() != TRUE))
#endif /* FEATURE_LTE_TO_HDR */
        {			
           HDR_MSG_PROT (MSG_LEGACY_HIGH, "Sending acq db earfcn request to LTE");
		   hdralmp_send_acq_db_earfcn_req_to_lte();
        }
#endif /* defined FEATURE_HDR_TO_LTE_REDIRECTION */
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_INIT_EXIT_HDR_MODE:
      ASSERT( cur_state == HDRALMPS_INIT );
      if ( cur_state == HDRALMPS_INIT )    /*lint !e774 always TRUE */
      {
        next_state = HDRALMPS_INACTIVE;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_IDLE_EXIT_HDR_MODE:
      ASSERT( cur_state == HDRALMPS_IDLE );
      abort_reason = HDRHAI_ABORT_RESELECTION;
      if ( cur_state == HDRALMPS_IDLE )    /*lint !e774 always TRUE */
      {
        next_state = HDRALMPS_INACTIVE;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_OVHD_EXIT_HDR_MODE:
    case HDRIND_RUP_EXIT_HDR_MODE:
      
      ASSERT( cur_state == HDRALMPS_IDLE || cur_state == HDRALMPS_CONNECTED );

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        /* Explicitely close the connection first */
        hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                 HDRHAI_ABORT_NORMAL_CLOSE );

        /* We have been redirected away from the HDR system.  Transition to
           INACTIVE state after the connection is done closing */
        hdralmps_change_pending_state( HDRALMPS_INACTIVE );
      }
      else if ( cur_state == HDRALMPS_IDLE )
      {
        next_state = HDRALMPS_INACTIVE;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }

      /* OVHD gives this indication for QC Redirect, initial acquisition SID
       * and SID change on handoff.  For now we treat the QC Redirect the
       * same as the others.  They all abort an origination in progress.
       */
      abort_reason = HDRHAI_ABORT_NOT_PREF_EXIT_HDR;

      break;

    case HDRIND_IDLE_CONNECTION_OPENED:
#ifdef FEATURE_LTE_TO_HDR_OH
        if ( curr_tunnel_mode == HDRHAI_TUNNEL_MODE_ENABLED )
      {
        ASSERT( cur_state == HDRALMPS_INIT );
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "Virtual connection opened");
      }
      else
#endif /* FEATURE_LTE_TO_HDR_OH */
      {
        ASSERT( cur_state == HDRALMPS_IDLE );
        abort_reason = HDRHAI_ABORT_NOT_ABORTING;
        /* Remember why the connection was opened in case we need to close it */
        open_reason = ind_data_ptr->open_reason;
        if ( cur_state == HDRALMPS_IDLE )    /*lint !e774 always TRUE */
        {
          /* Transition to connected state */
          next_state = HDRALMPS_CONNECTED;
        }
        else
        {
          ERR("Wrong State!",0,0,0);
        }
      }
      break;

    case HDRIND_RTCMAC_SHUTDOWN_COMPLETE:
      /* We can get this command while in INIT or INACTIVE if we are
       * re-directed by Quick Config or Redirect message while connected.
       */
      abort_reason = HDRHAI_ABORT_NORMAL_CLOSE;
      if ( cur_state == HDRALMPS_CONNECTED )
      {
        next_state = pending_state;
        if (next_state == HDRALMPS_CONNECTED)
        {
          /* This occurs if connected state protocol autonomously decided to
           * close the connection.  By shutting down the RTCMAC, is implied
           * that we should return to idle state.
           */
          next_state = HDRALMPS_IDLE;
        }
      }
      else
      {
        HDRTRACE_IND_IGNORED_STATE( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
          ind_name, cur_state );
      }
      break;

    case HDRIND_IDLE_CHANGE_HDR_SYSTEMS:
      ASSERT( cur_state == HDRALMPS_IDLE );
      abort_reason = HDRHAI_ABORT_RESELECTION;
      if ( cur_state == HDRALMPS_IDLE )    /*lint !e774 always TRUE */
      {
        next_state = HDRALMPS_INIT;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_OVHD_CHANGE_HDR_SYSTEMS:
    case HDRIND_RUP_CHANGE_HDR_SYSTEMS:
      ASSERT( cur_state == HDRALMPS_IDLE || cur_state == HDRALMPS_CONNECTED );

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        /* Close the connection first */
        hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                 HDRHAI_ABORT_NORMAL_CLOSE );

        /* Return to INIT after the connection is closed, we have been 
           redirected to a different HDR system */
        hdralmps_change_pending_state( HDRALMPS_INIT );
      }
      else if (cur_state == HDRALMPS_IDLE)
      {
        next_state = HDRALMPS_INIT;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }

      /* OVHD gives this indication for QC Redirect, initial acquisition SID
       * and SID change on handoff.  For now we treat the QC Redirect the
       * same as the others.  They all abort an origination in progress.
       */
      abort_reason = HDRHAI_ABORT_NOT_PREF_CHG_HDR;

      break;

    case HDRIND_OVHD_SUPERVISION_FAILED:
    case HDRIND_CCMAC_SUPERVISION_FAILED:

      abort_reason = HDRHAI_ABORT_SYS_LOST;

      if ( cur_state == HDRALMPS_IDLE )
      {
        /* A supervision failure while idle means we lost the system.
         * Report this event to System Determination
         */
#ifdef FEATURE_LTE_TO_HDR_OH
        if (hdrsap_lte_to_hdr_active_ho_is_in_progress())
        {
          next_state = HDRALMPS_INACTIVE;
        }
        else
#endif /* FEATURE_LTE_TO_HDR_OH */
        {
          sd_action = sd_ss_ind_hdr_opr_sys_lost2( SD_SS_SYS_LOST_SUP_FAILURE, 
                                                   NULL );
          HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);

          ASSERT(sd_action != SD_SS_ACT_CONTINUE);
          if ( sd_action == SD_SS_ACT_ACQ_HDR )
          {
            next_state = HDRALMPS_INIT;
          }
          else
          {
            next_state = HDRALMPS_INACTIVE;
          }
        }

        /* Increment the number of supervision failures for logging purposes */
        if ( ind_name == HDRIND_CCMAC_SUPERVISION_FAILED )
        {
          hdrplog_inc_conn_term_stats_counter(
            HDRPLOG_CCMAC_IDLE_SUPERVISION_FAILURE_COUNT, 1 );
        }
      }
      else if ( cur_state == HDRALMPS_CONNECTED )
      {
        /* A supervision failure while connected forces us back to
         * idle state, but we wait for another failure before giving
         * up on the system because our active set while connected
         * may not have included a pilot we can acquire when idle.
         */
        next_state = HDRALMPS_IDLE;

        /* Increment the number of supervision failures for logging purposes */
        if ( ind_name == HDRIND_CCMAC_SUPERVISION_FAILED )
        {
          /* Increment the appropriate supervision failure based on 
           * whether we are in suspended state or not.                 
           */
          if ( hdralmp.is_traffic_suspended )
          {
            hdrplog_inc_conn_term_stats_counter(
              HDRPLOG_CCMAC_TRAFFIC_SUSPENDED_SUPERVISION_FAILURE_COUNT, 1 );
          }
          else
          {
            hdrplog_inc_conn_term_stats_counter(
              HDRPLOG_CCMAC_TRAFFIC_SUPERVISION_FAILURE_COUNT, 1 );
          }
        }
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_ACMAC_TRANSMISSION_FAILED:
      abort_reason = HDRHAI_ABORT_MAX_ACC_PROBE;
      /* If SMP was sending a session close in the probe that failed, ALMP
       * has already returned to init state as a result of HDRALMP_RESET_CMD,
       * so this indication can be ignored if we are already in init state.
       */
      if ( cur_state == HDRALMPS_IDLE )
      {
        /* Max access probes is treated like a fade.  This avoids the need for
         * session layer and protocols like OVHD, RUP and IDLE to have special
         * handling for HDRIND_ACMAC_TRANSMISSION_FAILED.  Instead, it leverages
         * the processing already in place for re-doing whatever processing was
         * interrupted, and re-sending whatever messages were dropped as a result
         * of a fade.
         */

        /* If we performed max access probes, consult SD and see what system
         * we should try next.
         */
        if (ind_data_ptr->access_inhibited)
        {
          /* If the access probe was denied because the Apersistence parameter is
           * set to Inhibit, then SD should avoid the system for a longer duration
           * than a max access probe failure */
          sd_action = sd_ss_ind_hdr_opr_acc_term( SD_SS_ACC_TERM_ACC_DEN, NULL );
        }
        else
        {
          sd_action = sd_ss_ind_hdr_opr_acc_term( SD_SS_ACC_TERM_MAX_ACC, NULL ); 
        }

        HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);

        if (( sd_action == SD_SS_ACT_ACQ_HDR ) ||
            ( sd_action == SD_SS_ACT_CONTINUE ))
        {
          next_state = HDRALMPS_INIT;
        }
        else
        {
          next_state = HDRALMPS_INACTIVE;
        }
      }
      break;

    case HDRIND_FTCMAC_SUPERVISION_FAILED:

#ifdef FEATURE_MDM_FUSION
      if (ind_data_ptr->sar_power_limited)
      {
        /* supervision due to sar limit */
        abort_reason = HDRHAI_ABORT_SAR_LIMIT;
      }
      else
#endif /* FEATURE_MDM_FUSION */
      {
        /* normal supervision */
        abort_reason = HDRHAI_ABORT_SYS_LOST;
      }

      if ( cur_state == HDRALMPS_CONNECTED )
      {
        /* A supervision failure while connected forces us back to
         * idle state, but we wait for another failure before giving
         * up on the system because our active set while connected
         * may not have included a pilot we can acquire when idle.
         */
        next_state = HDRALMPS_IDLE;
      }
      else if ( cur_state == HDRALMPS_IDLE )
      {
        /* As per QC-IS-856 BALLOT COMMENT-56 we are to abort the connection
         * setup immediately upon detecting DRC supervision failure.
         */
        HDR_MSG_PROT (MSG_LEGACY_HIGH, "fmac fail during con setup");

        if (hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY))
        {
          /* Call SD action here instead of HDRIDLE, and based on the SD action,
           * only call hdridle_deactivate when SD action is to continue. */
          sd_action = sd_ss_ind_hdr_opr_acc_term(SD_SS_ACC_TERM_TIMEOUT, NULL); 
        }
        else
        {
          /* If AT is during a session negotiation, always call 
           * hdridle_deactivate */
          sd_action = SD_SS_ACT_CONTINUE;
        }

        if ( sd_action == SD_SS_ACT_CONTINUE )
        {
          /* Abort reason of HDRHAI_ABORT_SYS_LOST does not cause
           * IDLE to call SD actions. */
          hdridle_deactivate( abort_reason,
            HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL );
          hdridle_activate(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
        }
        else if ( sd_action == SD_SS_ACT_ACQ_HDR )
        {
          next_state = HDRALMPS_INIT;
        }
        else
        {
          next_state = HDRALMPS_INACTIVE;
        }
      }
      else
      {
        ERR("FMAC Sup. Fail in state %d",cur_state,0,0);
      }
      break;

    case HDRIND_RUP_ASSIGNMENT_REJECTED:
      ASSERT( cur_state == HDRALMPS_CONNECTED );
      abort_reason = HDRHAI_ABORT_SYS_LOST;
      if ( cur_state == HDRALMPS_CONNECTED )/*lint !e774 always TRUE */
      {
        /* A TC assignment with a different frame offset while
         * connected forces us to close the connection.
         */
        next_state = HDRALMPS_IDLE;
      }
      else
      {
        ERR("Wrong State!",0,0,0);
      }
      break;

    case HDRIND_LMAC_NETWORK_LOST:
#ifdef FEATURE_HDR_DDARF
    case HDRIND_IDLE_NETWORK_LOST:
#endif /* FEATURE_HDR_DDARF */
      abort_reason = HDRHAI_ABORT_SYS_LOST;

      if ( cur_state == HDRALMPS_INACTIVE )
      {
        /* We should not receive a network lost indication while inactive */
        ERR("Wrong state",0,0,0);
      }
#ifdef FEATURE_LTE_TO_HDR_OH
      else if ( hdrsap_lte_to_hdr_active_ho_is_in_progress() )
      {
        /* If system loss happens before active handoff succeeds, AT should
         * go back to tunnel mode instead of reacquring on HDR network.
         */
        next_state = HDRALMPS_INACTIVE;
      }
#endif /* FEATURE_LTE_TO_HDR_OH */
      else if ( !hdralmp_sd_already_consulted() )
      {
#ifdef FEATURE_HDR_DDARF
        if (ind_name == HDRIND_IDLE_NETWORK_LOST)
        {
          /* Since the only reason for which IDP issues network lost indication
             is for losing acquisition after wakeup, directly fill it in */
          sys_lost_reason = SD_SS_SYS_LOST_LOST_ACQ_AFTER_SLEEP;
        }
        else
#endif /* FEATURE_HDR_DDARF */
        {
	  if(ind_data_ptr != NULL)
          {
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
            if(ind_data_ptr->sys_lost_reason != HDRSRCH_SYSLOST_AVOIDED_CH_IN_ASET)
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
            {
               sys_lost_reason = hdralmp_map_sys_lost_to_sd(
                                ind_data_ptr->sys_lost_reason);
            }
	  }/* if(ind_data_ptr != NULL) */
        }/* else ind_name == HDRIND_IDLE_NETWORK_LOST */

        /* Report the system loss to SD if ALMP isn't already waiting to
           reacquire or exit the system */
#ifdef FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS
        /* if the system lost is due to avoided channel present in ASET
                                  we need to call a different SD API */
        if((ind_data_ptr != NULL) && (ind_data_ptr->sys_lost_reason == HDRSRCH_SYSLOST_AVOIDED_CH_IN_ASET))
        {
            sd_action = sd_ss_ind_hdr_opr_bad_sys (SD_SS_BAD_SYS_HDR_AVOIDED_CH_IN_ASET, NULL);
        }
        else
#endif /* FEATURE_HDR_REMOVE_AVOIDED_NSET_CHANNELS */
        {
           sd_action = sd_ss_ind_hdr_opr_sys_lost2( sys_lost_reason, NULL );
        }
          
        HDR_MSG_PROT_2 (MSG_LEGACY_MED,  "NetworkLost (reason %d): SD Action %d", 
                        sys_lost_reason,
                        sd_action);

        ASSERT( sd_action != SD_SS_ACT_CONTINUE);

        switch ( sd_action )
        {
          case SD_SS_ACT_ACQ_HDR:

            if ( cur_state == HDRALMPS_IDLE || cur_state == HDRALMPS_CONNECTED )
            {
              /* We have lost the system.  Re-acquire. */
              next_state = HDRALMPS_INIT;
            }
            else if ( cur_state == HDRALMPS_INIT )
            {
              /* Init state is already active, inform it that it must re-acquire */
              hdrinit_reacquire(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
              hdrind_give_ind( HDRIND_ALMP_REACQUIRING , NULL );
            }
            break;

          case SD_SS_ACT_CONTINUE:

            /* SD should never advice the protocols to continue if ALMP just
               informed it that the network was lost. */
            ERR_FATAL("SD should not continue on a NetworkLost",0,0,0);
            break;

          default:

            /* We need to exit HDR */
            next_state = HDRALMPS_INACTIVE;
            break;
        } /* end switch */
      }

      break;

    case HDRIND_LMAC_RESELECT:

      if ( ( ds707_extif_is_pkt_data_sess_on_hdr() ) &&
           ( !ds707_extif_pkt_is_dormant()  )
         )
      {
        if (cur_state == HDRALMPS_CONNECTED)
        {
          /* Reselection is in progress, we will need to notify CM after
           * we are done closing the connection */
          hdralmp.reselect_in_progress = TRUE;

          /* Increment the number of 1x handdowns for logging purposes. 
           */
          hdrplog_inc_conn_term_stats_counter( HDRPLOG_1X_HANDDOWN_COUNT, 1 );

          /* Pending state will be changed when handling close conn cmd */
          hdralmp_close_connection(HDRHAI_ABORT_RESELECT,
                                   HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
        }
        else
        {
          /* SD should be notified first of the reselection */
          hdralmp_send_reselect_to_clients();
        }
      }
      else
      {
        /* We don't have a data session up and running, so we are ignoring
         * the reselection event.  Let's count the number of these events
         * for logging purposes. 
         */
        hdrplog_inc_conn_term_stats_counter(
          HDRPLOG_1X_HANDDOWN_IGNORED_COUNT, 1 );
      }
      break;

    case HDRIND_LMAC_DEACTIVATE_DONE:
      auto_deact_reason = PROT_DEACT_ACQ_OTHERS;

      if ( cur_state == HDRALMPS_INACTIVE )
      {
        if (hdralmp.deactivate.deactivation_cmd_is_pending)
        {
#ifdef FEATURE_LTE_TO_HDR_OH
          if ( hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL) )
          {     
            if ( hdrmc_lte_to_hdr_ho_is_in_progress() )
            {
              HDR_MSG_PROT (MSG_LEGACY_MED, 
                 "LTE->HDR HO failed in tunnel-mode returning to online tunnel state");
            }
            else if ( hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
            {
              /* 1. CGI procedure is finished successfully or unsuccessfully (timer expires)
               * 2. Time transfer or fast acquisition fails in the middle of CGI procedure */
              HDR_MSG_PROT (MSG_LEGACY_MED, 
                 "Get CGI finishes, returning to online tunnel state");
            }
            else if ( hdrmc_hdr_to_lte_time_transfer_is_in_progress() )
            {
              HDR_MSG_PROT (MSG_LEGACY_MED, 
                 "Reverse time transfer finishes, returning to online tunnel state");
            }
            else 
            {
              (void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );
              hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );
            }

            /* set deact_rason to REDIR_ABORT instead of ACQ_OTHERS. This is because ACQ_OTHERS  
               is used by MMOC only if HDR is in Online steate. For tunnel mode cases, from MMOC 
               point of view, HDR is not online yet */
            auto_deact_reason = PROT_DEACT_REDIR_ABORT;
          }
          else
#endif /* FEATURE_LTE_TO_HDR_OH */
          {
#ifdef FEATURE_LTE_TO_HDR_OH
            if ( hdrmc_tunnel_activation_is_pending() )
            {
              HDR_MSG_PROT( MSG_LEGACY_MED, 
                            "tunnel activation is pending, do not exit HDR" );
            }
            else
#endif /* FEATURE_LTE_TO_HDR_OH */
            {
              /* No longer operating in HDR mode.  
               * Signal HDRMC mode controller to exit */
              (void) rex_set_sigs( rex_self(), HDRMC_EXIT_SIG );
              hdrmc_set_hdr_exiting(TRUE);
              hdrind_give_ind( HDRIND_ALMP_EXIT_HDR_MODE , NULL );
            }
          }

          hdralmp.deactivate.deactivation_cmd_is_pending = FALSE;
          if(hdralmp.deactivate.trans_id_is_valid)
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_MED, "Online send DeactCnf id=%d",
                          hdralmp.deactivate.trans_id);
            mmoc_rpt_prot_deactd_cnf( hdralmp.deactivate.trans_id );
#ifdef FEATURE_HDR_DEBUG_INFO
			hdrmc_log_sent_rpt(hdralmp.deactivate.trans_id, HDR_MMOC_RPT_PROT_DEACTD_CNF);
#endif /* FEATURE_HDR_DEBUG_INFO */
          }
          else
          {
            HDR_MSG_PROT_1 (MSG_LEGACY_MED, 
                            "Online send AutoDeactInd reason=%d",
                            auto_deact_reason );
            mmoc_rpt_prot_auto_deactd_ind( auto_deact_reason, 
                                           PROT_STATE_ONLINE_HDR );
#ifdef FEATURE_HDR_DEBUG_INFO
            hdrmc_log_sent_rpt(0, HDR_MMOC_RPT_PROT_AUTO_DEACTD_IND);
#endif /* FEATURE_HDR_DEBUG_INFO */
          }

#ifdef FEATURE_LTE_TO_HDR
          if (hdrmc_lte_to_hdr_ho_is_in_progress())
          {
            /* If handoff from LTE to HDR was in progress then indicate handoff 
               failure and/or abort done to LTE */

            /* Indicate handoff abort done to SD and LTE*/
            if (hdrmc_lte_to_hdr_ho_is_aborted())
            {
              /* Indicate to LTE that handoff abort operation is done */
              hdrmc_send_abort_done_to_lte(TRUE);
            }
            else
            {
              /* Indicate to LTE failure in redirection operation */
              hdrmc_send_irat_ho_failure_ind_to_lte(TRUE);
            }

            /* SD also needs to clean up it's context */
            if ( hdrmc_lte_to_hdr_ho_is_aborted() || 
                 hdrinit_hystr_timer_is_expired() )
            {
              sd_ss_ind_hdr_acq_redir_abort(NULL);
            }

          }
          else if ( hdrmc_hdr_to_lte_time_transfer_is_in_progress() ||
                    hdrmc_lte_to_hdr_get_cgi_is_in_progress() )
          {
            /* If HDR to LTE timing transfer was in progress when the stack 
               is deactivated send a response to LTE. Response could be 
               success or failure based on whether timing information was 
               obtained from searcher or not */

            /* SD also needs to clean up it's context */
            sd_ss_ind_hdr_acq_redir_abort(NULL);

            /* Indicate handoff abort done to SD and LTE*/
            if (hdrmc_lte_to_hdr_ho_is_aborted())
            {
              /* Indicate to LTE that handoff abort operation is done */
              hdrmc_send_abort_done_to_lte(TRUE);
            }
            else if (hdrmc_lte_to_hdr_get_cgi_is_in_progress())
            {
              /* Send CGI info to LTE */
              hdrmc_send_cgi_to_lte();
            }
            else
            {
              /* Send timing info to LTE */
              hdrmc_send_timing_info_to_lte();
            }
          }

          if ( hdrinit_hystr_timer_is_expired() )
          {
            hdrinit_set_hystr_timer_is_expired(FALSE);
          }

#endif /* FEATURE_LTE_TO_HDR */

        }
        else
        {
          HDR_MSG_PROT (MSG_LEGACY_FATAL, "hdrlmac_deactivate_done_cb() not pending");
        }
      }
#if defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION)
      else if ( cur_state == HDRALMPS_SUSPEND )
      {
        /* deactivation_cmd_is_pending is set when HDR CP enters SUSPEND.
         * Reset. */
        hdralmp.deactivate.deactivation_cmd_is_pending = FALSE;
		  
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
			  if(hdralmp.lte_redir_info.redirect_is_in_progress == TRUE)
			  {
				 if ( hdralmp.lte_redir_info.pending_abort_redir )
				 {
				   hdralmp_set_pending_abort_redir( FALSE );
				   hdralmp_reacquire(HDRHAI_ABORT_D2L_FAILURE, HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
				 }
				 else
				 {
	   HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: deactivate done. Redirection to LTE. Send redirection req to LTE");
				   hdralmp_send_redirect_req_to_lte();
				 }
			  }
			  else
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
			  {
#ifdef FEATURE_HDR_TO_LTE
        if ( hdralmp.lte_resel_info.pending_abort_resel )
        {
          hdralmp_set_pending_abort_resel( FALSE );
          HDRTRACE_LTE_RESEL_STATUS(HDRMC_HDR_LTE_RESELECT_ABORT);
          hdralmp_reacquire(HDRHAI_ABORT_D2L_FAILURE, HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
        }
        else
        {
          hdralmp_send_reselect_req_to_lte();
        }
#endif /* FEATURE_HDR_TO_LTE */
      }
      }/* else if */
#endif /* defined(FEATURE_HDR_TO_LTE) || defined(FEATURE_HDR_TO_LTE_REDIRECTION) */

      else
      {
        /* We should not receive a deactivate done indication while active */
        HDR_MSG_PROT (MSG_LEGACY_FATAL, "hdrlmac_deactivate_done_cb() unexpected");
      }

      break;

    case HDRIND_RTCMAC_DRC_TX_STOPPED:

      /* We are just accelerating the CMAC timer expiry */
      abort_reason = HDRHAI_ABORT_SYS_LOST;

      if (( cur_state == HDRALMPS_CONNECTED ) && 
          ( pending_state != HDRALMPS_CONNECTED))
      {
        /* We are in the middle of closing a connection and we turned off the 
         * transmitter.  Rather than waiting for up to 5s for the  CMAC timer 
         * to expire, just force the connection closed.
         */
        HDR_MSG_PROT (MSG_LEGACY_HIGH,
                      "TX stopped while closing, so close now.");
        next_state = pending_state;
      }
      break;

    /* Maintain whether or not traffic is suspended or not so that we can 
     * properly track how many CCMAC supervision failures happened while
     * traffic was suspended or not.
     */
    case HDRIND_LMAC_TC_SUSPEND:
      hdralmp.is_traffic_suspended = TRUE;
      break;
    case HDRIND_LMAC_TC_RESUME:
      hdralmp.is_traffic_suspended = FALSE;
      break;

#ifdef FEATURE_SVDO
    case HDRIND_LMAC_CONN_ERROR:
      ASSERT( cur_state == HDRALMPS_CONNECTED );
      /* If we have already consulted SD and ALMP is pending to transition
         to INACTIVE or INIT state, then we should not consult SD again */
      if ( !hdralmp_sd_already_consulted() )
      {

        HDR_MSG_PROT (MSG_LEGACY_HIGH, "LMAC_CONN_ERROR ind received");
        sd_action = sd_ss_ind_hdr_opr_svdo_incompat_sys( NULL );
        HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);

        /*---------------------------------------------------------------------
          Redirected to a different HDR system
        ---------------------------------------------------------------------*/
        if ( sd_action == SD_SS_ACT_ACQ_HDR )
        {
          /* Close the connection first before proceeding w/ redirection */
          hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                   HDRHAI_ABORT_ERROR );

          /* Our intention is to go to INIT state after the connection
             is done closing */
          hdralmps_change_pending_state( HDRALMPS_INIT );
        }
        /*---------------------------------------------------------------------
          Exit HDR
        ---------------------------------------------------------------------*/
        else if ( sd_action != SD_SS_ACT_CONTINUE )
        {
          /* Close the connection first before proceeding w/ redirection */
          hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                   HDRHAI_ABORT_ERROR );

          /* Our intention is to Exit HDR after the connection is done 
             closing */
          hdralmps_change_pending_state( HDRALMPS_INACTIVE );
        }
      }
      break;
#endif /* FEATURE_SVDO */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_SAP_CONNECTION_CLOSED:
      /* ALMP is INIT state if virtual connection is closed when AT is in
       * the tunnel mode; ALMP is in IDLE if AT is handoff to native HDR when
       * virtual connection is open, e.g. redirect durring session negotiation.
       */
      ASSERT( (cur_state == HDRALMPS_INIT) || (cur_state == HDRALMPS_IDLE) );
      hdrind_give_ind( HDRIND_ALMP_CONNECTION_CLOSED, NULL );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */
    
#ifdef FEATURE_HDR_TO_LTE
    case HDRIND_LMAC_IRAT_RESELECT:
      /* Ignore the indication when received in INACTIVE or INIT state.
       * This is to handle the following race condition:
       * When HDRCP receives reacquire/deactivate command, the reselection 
       * indication has already been sent by SRCH and queued in HDRMC. */
      if ( cur_state == HDRALMPS_INACTIVE )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "Already deactivated, LMAC_IRAT_RESELECT is ingored" );
      }
      else if ( cur_state == HDRALMPS_INIT
#ifdef FEATURE_LTE_TO_HDR_OH
                &&
                !hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL)
#endif /* FEATURE_LTE_TO_HDR_OH */
              )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                      "Acquring, LMAC_IRAT_RESELECT is ingored" );
      }
      else
      {
        ASSERT (cur_state == HDRALMPS_IDLE );

        memset(&hdralmp.lte_resel_info, 0, sizeof(hdralmp_lte_resel_info_type));
        if ( hdrovhd_check_ok_to_resel_to_lte(&hdralmp.lte_resel_info.rat_pri_list, 
			                                          ind_data_ptr->lte_resel_info.earfcn))
        {
          /*---------------------------------------------------------------------
           Received reselect to LTE indication from SRCH enter suspend state and 
           deactivate searcher. Once searcher is deactivated send command to 
           LTE RRC to attempt acquistion.
          ---------------------------------------------------------------------*/
          HDR_MSG_PROT (MSG_LEGACY_HIGH, "Reselect to LTE");
          hdralmp.lte_resel_info.earfcn = ind_data_ptr->lte_resel_info.earfcn; 
          hdralmp.lte_resel_info.cell_id = ind_data_ptr->lte_resel_info.cell_id;
          next_state = HDRALMPS_SUSPEND;
        }
      }
      break;
#endif /* FEATURE_HDR_TO_LTE */
#ifdef FEATURE_HDR_TO_LTE_REDIRECTION 
      case HDRIND_LMAC_ORAT_MEAS_RPT:

      err = hdralmp_process_orat_measurement_report_ind(&(ind_data_ptr->redirect_rpt),TRUE);
			 
	  if((err == E_SUCCESS))
      {
         /* If UE initiated D2L redirection can be started. Then ALMP move to SUSPEND */
         HDR_MSG_PROT( MSG_LEGACY_MED, "HDR DLR: Trigger redirection to LTE. Move to SUSPEND");
         next_state = HDRALMPS_SUSPEND;
      } 
      break;
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION  */

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }

  if ( next_state != cur_state )
  {
    /* There may be some common event dependent processing to be done before
     * we change state.
     */
    hdralmp_change_state( abort_reason, next_state );
  }

  if ((ind_name == HDRIND_IDLE_CONNECTION_OPENED) &&
      (open_reason == HDRHAI_OPEN_ABORT))
  {
    /* If by the time we opened the connection there was no longer a reason
     * for it to be open, then just close it.
     */
    hdralmp_close_connection(HDRHAI_ABORT_NORMAL_CLOSE,
                             HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL);
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdralmp_process_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_EXTRACT_REDIR_CHAN

DESCRIPTION
  This function extracts a channel record from a redirect message pointed
  to by the DSM item pointer passed in.  A pointer to this function is passed
  to System Determination along with a pointer to the DSM item containing the
  redirect message, and SD passes the pointer back here to extract the
  channels one at a time.

DEPENDENCIES
  None

PARAMETERS
  void_item_ptr - Pointer to the DSM item chain holding the redirect message
  index - Which channel is being requested
  system_ptr - Where to place the extracted system information

RETURN VALUE
  TRUE if a channel is successfully extracted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean hdralmp_extract_redir_chan
(
  void* void_item_ptr,
  int index,
  sd_sys_s_type *system_ptr
)
{
  uint8 num_chans; /* Number of channels in the redirect message */
  uint8 sys; /* System type given in the message */
  int err; /* If there is an error unpacking */
  boolean ret_val = FALSE; /* Value to be returned */
  /* Cast the DSM item poitner passed in to be the correct type */
  dsm_item_type *item_ptr = (dsm_item_type *) void_item_ptr;
  uint8 band;
  uint16 chan;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( HDRALMP_REDIRECT_NUM_CHANNEL( item_ptr, &num_chans ) == E_SUCCESS )
  {
    if (index < num_chans)
    {
      err = HDRALMP_REDIRECT_SYSTEM_TYPE( index, item_ptr, &sys );

      err |= HDRALMP_REDIRECT_BAND( index, item_ptr,
                                    &band );

      system_ptr->band = band;

      err |= HDRALMP_REDIRECT_CHANNEL( index, item_ptr, &chan );

      system_ptr->chan = chan;

      if (err == 0)
      {
        /* TODO: add sys type 2 case */
        if (sys == SYS_HDR_CHAN_REC_MODE_IS856)
        {
          system_ptr->mode = SYS_SYS_MODE_HDR;
          ret_val = TRUE;
        }
        else if (sys == SYS_HDR_CHAN_REC_MODE_IS2000)
        {
          system_ptr->mode = SYS_SYS_MODE_CDMA;
          ret_val = TRUE;
        }
        else
        {
          ERR("Redir not to HDR or CDMA!",0,0,0);
        }
      }
      else
      {
        ERR("Could not extract channel",0,0,0);
      }
    }
    else
    {
      HDR_MSG_PROT (MSG_LEGACY_LOW, "requested index out of range");
    }
  }
  else
  {
    ERR("Could not extract number of items",0,0,0);
  }

  return ret_val;
} /* hdralmp_extract_redir_chan() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for ALMP.

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
void hdralmp_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;            /* Message identifier */

  /* Current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );
  /* Next state */
  hdralmps_state_enum_type next_state = cur_state;
  /* Why we are leaving the state (may not be an abort) */
  hdrhai_abort_reason_enum_type abort_reason = HDRHAI_ABORT_UNSPECIFIED;

  /* Continue, acquire HDR, acquire IS-95, etc. */
  sd_ss_act_e_type sd_action;

  /* Current message's extracted transaction ID */
  uint8 trans_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG));
  /* MessageID is the first uint8 */
  /* b_unpackb (item_ptr->data_ptr, 2); */
  if ( dsm_extract( item_ptr, 0, &msg_id, 1 ) == 0 )
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  HDRTRACE_MSG( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, msg_id, cur_state );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( msg_id )
  {
    case HDRALMP_CONN_FAILURE_REPORT_ACK_MESSAGE_ID:

      /* If we are waiting for the ack message */
      if( hdralmp.wait_for_conn_failure_report_ack )
      {
        if ( HDRBIT_UNPACK8( item_ptr, &trans_id, 
                HDRALMP_CONN_FAILURE_REPORT_ACK_TRANS_ID_OFFSET, 
                HDRALMP_TRANS_ID_LEN ) != E_SUCCESS )
        {
          HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                        "Could not unpack trans_id for conn_failure_report_ack msg");
        }
        else if( trans_id == hdralmp.conn_fail_trans_id )
        {
          HDR_MSG_PROT( MSG_LEGACY_MED, "ConnectionFailureReportAck received" );
  
          /* Reset all the records and the waiting for ack flag */
          hdralmp_reset_conn_fail_records();
        }

      } /* otherwise just break and discard the junk message */
      
      break;

    case HDRALMP_REDIRECT_MSG:
#ifdef FEATURE_HDR_RESET_KA_TIMER_ON_ACACK_ONLY
#ifdef FEATURE_HDR_RESET_KA_TIMER_ON_REDIRECT_MSG
      /* by default, the AT will not restart ka timer when it receives Redirect msg*/
      hdrsmp_restart_ka_timer();
#endif
#endif

      /* If we have already consulted SD and ALMP is pending to transition
         to INACTIVE or INIT state, then we should not consult SD again */
      if ( !hdralmp_sd_already_consulted() )
      {

        HDR_MSG_PROT (MSG_LEGACY_HIGH, "Redirect message received");
        sd_action = sd_ss_ind_hdr_opr_redir( hdralmp_extract_redir_chan,
                                item_ptr, SD_SS_REDIR_HDR_DIRECTED, NULL );
        HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "SD Action %d", sd_action);

        /*---------------------------------------------------------------------
          Redirected to a different HDR system
        ---------------------------------------------------------------------*/
        if ( sd_action == SD_SS_ACT_ACQ_HDR )
        {
          abort_reason = HDRHAI_ABORT_REDIRECT_CHG_HDR;
          HDRTRACE_ALMP_OBEYING_REDIRECTION();

          if ( cur_state == HDRALMPS_CONNECTED )
          {
            /* Close the connection first before proceeding w/ redirection */
            hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                     HDRHAI_ABORT_NORMAL_CLOSE );

            /* Our intention is to go to INIT state after the connection
               is done closing */
            hdralmps_change_pending_state( HDRALMPS_INIT );
          }
          else
          {
            /* If we don't have a connection opened, transition to init 
               immediately */
            next_state = HDRALMPS_INIT;
          }

        }
        /*---------------------------------------------------------------------
          Exit HDR
        ---------------------------------------------------------------------*/
        else if ( sd_action != SD_SS_ACT_CONTINUE )
        {
          abort_reason = HDRHAI_ABORT_REDIRECT_EXIT_HDR;
          HDRTRACE_ALMP_OBEYING_REDIRECTION();

          if ( cur_state == HDRALMPS_CONNECTED )
          {
            /* Close the connection first before proceeding w/ redirection */
            hdrcon_close_connection( HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,
                                     HDRHAI_ABORT_NORMAL_CLOSE );

            /* Our intention is to Exit HDR after the connection is done 
               closing */
            hdralmps_change_pending_state( HDRALMPS_INACTIVE );

          }
          else
          {
            /* If there was no connection opened, transition to inactive 
               immediately */
            next_state = HDRALMPS_INACTIVE;
          }
        }
      }
      break;

    default:
      ERR( "Unknown in use message %d", msg_id, 0, 0 );
      break;
  }

  dsm_free_packet( &item_ptr );

  if ( next_state != cur_state )
  {
    /* There may be some common event dependent processing to be done before
     * we change state.
     */
    hdralmp_change_state( abort_reason, next_state );
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdralmp_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SEND_CALL_FADE_TO_CM

DESCRIPTION
  This function sends a call fade report to the CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdralmp_send_call_fade_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (hdrcallid_get_num_active_call_ids() > 0)
  {

    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->fade.hdr.cmd = CM_HDR_CALL_FADE_F;
      cmd_ptr->fade.num_call_ids = hdrcallid_get_all_call_ids ( cmd_ptr->fade.call_ids );
      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "Sent CM_HDR_CALL_FADE_F for %d callids",
                      cmd_ptr->fade.num_call_ids);
      hdralmp_cm_mc_rpt( cmd_ptr );

    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids();
  }

} /* hdralmp_send_call_fade_to_cm() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SEND_CALL_RELEASE_TO_CM

DESCRIPTION
  This function sends a call release report to the CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdralmp_send_call_release_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (hdrcallid_get_num_active_call_ids() > 0)
  {
    if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
    {
      cmd_ptr->release.hdr.cmd = CM_HDR_CALL_RELEASE_F;
      cmd_ptr->release.num_call_ids = hdrcallid_get_all_call_ids ( cmd_ptr->release.call_ids );
      for(i = 0; i < cmd_ptr->release.num_call_ids; i++) {
        cmd_ptr->release.rel_codes[i] = CM_CALL_REL_NORMAL;
      }

      HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "Sent CM_HDR_CALL_RELEASE_F for %d callids",
                      cmd_ptr->release.num_call_ids);
      hdralmp_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }

    hdrcallid_deallocate_all_call_ids();
  }

} /* hdralmp_send_call_release_to_cm() */
/* <EJECT> */

/*===========================================================================

FUNCTION HDRALMP_SEND_RESELECT_TO_CLIENTS

DESCRIPTION
  This function sends a reselect command to the registered clients.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdralmp_send_reselect_to_clients( void )
{
  cm_mc_rpt_type *cmd_ptr;

#ifdef FEATURE_HDR_MC_MSGR
  hdrcp_msg_rsp_u msg_rsp;
    /* Message to inform client about reselect. */
  errno_enum_type  estatus;
    /* Error status */
#endif /* FEATURE_HDR_MC_MSGR */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /*********** Inform CM ***********/

  if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
  {
    cmd_ptr->hdr.cmd = CM_HDR_RESELECT_EXIT_HDR_F;
    hdralmp_cm_mc_rpt( cmd_ptr );
    HDR_MSG_PROT (MSG_LEGACY_MED,  "Sent CM_HDR_RESELECT_EXIT_HDR_F");
  }
  else
  {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

#ifdef FEATURE_HDR_MC_MSGR
  /*********** Inform MSGR clients ***********/

  /* Init message router message */
  msgr_init_hdr ( &msg_rsp.hdr, 
                  MSGR_HDR_CP, HDR_CP_RESELECT_EXIT_HDR_IND  );

  /* Send the message */
  estatus = msgr_send ( &msg_rsp.hdr, sizeof ( hdrcp_msg_rsp_u ) );

  HDR_MSG_PROT_1 (MSG_LEGACY_ERROR, 
                  "Resel ind MSGR send status %d", estatus);
#endif /* FEATURE_HDR_MC_MSGR */

  /* For subsystem who also wants to know this event but does not have MSG Router */
  hdrind_give_ind( HDRIND_ALMP_RESELECT_EXIT_HDR, NULL );

} /* hdralmp_send_reselect_to_clients() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_CONNECTION_IS_OPEN

DESCRIPTION
  Boolean access function, can be called from any task to check if a
  connection is open.  ALMP thinks a connection is open as long as it
  is in the connected state, which does not correspond directly with
  when the RTCMAC is deactivated.

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a connection is open, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdralmp_connection_is_open( hdrhai_protocol_name_enum_type caller )
{
  /* current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDRTRACE_ALMP_CONNECTION_IS_OPEN( caller, cur_state,
    (cur_state == HDRALMPS_CONNECTED) );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  return( cur_state == HDRALMPS_CONNECTED );
} /* hdralmp_connection_is_open() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SYSTEM_IS_ACQUIRED

DESCRIPTION
  Boolean access function, can be called from any task to check if a
  system is acquired.  ALMP thinks a system is acquired as long as it
  is in the idle or connected states.

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdralmp_system_is_acquired( hdrhai_protocol_name_enum_type caller )
{
  /* current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (caller && caller != HDRHAI_IS890_FTAP)
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL))
    {
      HDRTRACE_ALMP_SYSTEM_IS_ACQUIRED( caller, cur_state, (cur_state == HDRALMPS_INIT) );
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      HDRTRACE_ALMP_SYSTEM_IS_ACQUIRED( caller, cur_state,
        (( cur_state == HDRALMPS_IDLE ) || ( cur_state == HDRALMPS_CONNECTED )) );
    }
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
  if (hdrsap_tunnel_mode_is_enabled(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL))
  {
    return (cur_state == HDRALMPS_INIT);  
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    return(( cur_state == HDRALMPS_IDLE ) ||
           ( cur_state == HDRALMPS_CONNECTED ));
  }
} /* hdralmp_system_is_acquired() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_RESELECT_IS_ALLOWED

DESCRIPTION
  Boolean access function, can be called from any task to check if system
  reselection is allowed

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a system is acquired, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdralmp_reselect_is_allowed( hdrhai_protocol_name_enum_type caller )
{
  /* current state */
  const hdralmps_state_enum_type cur_state = hdralmps_get_state(  );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(caller); /* remove this line when caller is used */

  return( cur_state == HDRALMPS_IDLE ) ;

} /* hdralmp_reselect_is_allowed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SD_ALREADY_CONSULTED

DESCRIPTION
  Boolean access function, should only be called from hdralmp to check if
  system determination has already been consulted and instructed the protocols
  to Exit HDR or Reacquire.  If this is the case, then the protocols should
  not consult system determination again because ALMP is simply delaying its
  transition to INIT or INACTIVE state until some other event completes
  (for instance, finish closing a connection)


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if SD has already been consulted
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdralmp_sd_already_consulted( void )
{
  /* pending state */
  const hdralmps_state_enum_type pending_state = hdralmps_get_pending_state();
  const hdralmps_state_enum_type cur_state = hdralmps_get_state();
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( cur_state == HDRALMPS_CONNECTED &&
         (pending_state == HDRALMPS_INIT || pending_state == HDRALMPS_INACTIVE)
        );

} /* hdralmp_sd_already_consulted() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_MAP_SYS_LOST_TO_SD

DESCRIPTION
  Helper function to map the searcher system lost reason enum type to the 
  extended SD one.

DEPENDENCIES
  None

PARAMETERS
  reason - system lost reason

RETURN VALUE
  Mapped system lost reason in SD format.

SIDE EFFECTS
  None

===========================================================================*/
sd_ss_sys_lost_e_type hdralmp_map_sys_lost_to_sd
(
  hdrsrch_system_lost_enum_type reason
)
{

  switch (reason)
  {
    case HDRSRCH_SYSLOST_LOST_ACQ_AFTER_SLEEP:
      return SD_SS_SYS_LOST_LOST_ACQ_AFTER_SLEEP;

    case HDRSRCH_SYSLOST_RUP_PILOT_SUPERVISION:
      return SD_SS_SYS_LOST_RUP_PILOT_SUPERVISION;

    case HDRSRCH_SYSLOST_SYNC_MSG_TIMEOUT:
      return SD_SS_SYS_LOST_SYNC_MSG_TIMEOUT;

    case HDRSRCH_SYSLOST_NO_RESOURCES:
      return SD_SS_SYS_LOST_NO_RESOURCES;

    case HDRSRCH_SYSLOST_MSTR_SLAM_TIMEOUT:
      return SD_SS_SYS_LOST_MSTR_SLAM_TIMEOUT;

    case HDRSRCH_SYSLOST_RESELECT:
      return SD_SS_SYS_LOST_RESELECT;

    case HDRSRCH_SYSLOST_ADVISE_UNLOCK_RF:
      return SD_SS_SYS_LOST_ADVISE_UNLOCK_RF;

    case HDRSRCH_SYSLOST_SYNC_WEAK_HDR_COVERAGE:
      return SD_SS_SYS_LOST_SYNC_WEAK_HDR_COVERAGE;

    case HDRSRCH_SYSLOST_DSP_NOT_READY:
      return SD_SS_SYS_LOST_DSP_NOT_READY;

    default:
      ERR("Unknown system lost reason",reason,0,0);
      return SD_SS_SYS_LOST_UNKNOWN;
  }

} /* hdralmp_map_sys_lost_to_sd() */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRALMP_TIMER_CB

DESCRIPTION
  This function is the callback for all ALMP protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdralmp_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL, timer_id);
} /* hdralmp_timer_cb() */

/*===========================================================================
FUNCTION HDRALMP_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the ALMP protocol
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_process_timer
(
  uint32 timer_id
)
{
  uint16 fl_per = HDRALMP_FL_PER_UNKNOWN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( (hdralmp_timer_enum_type)timer_id )
  {
    case HDRALMP_FL_PER_TIMER:
      if (hdralmps_get_state() == HDRALMPS_CONNECTED)
      {
        fl_per = hdrrx_get_connected_state_per();
  
        hdralmp_send_per_rpt(fl_per);
       
        (void) rex_set_timer(&hdralmp.fl_per_timer, 
                             HDRALMP_FL_PER_TIMER_INTERVAL);
      }
      break;

#ifdef FEATURE_LTE_TO_HDR
    case HDRALMP_GET_CGI_TIMER:
      if ( (hdralmps_get_state() == HDRALMPS_INIT) ||
           (hdralmps_get_state() == HDRALMPS_IDLE) )
      {
        hdralmp_change_state( HDRHAI_ABORT_UNSPECIFIED, HDRALMPS_INACTIVE );
      }
      break;
#endif /* FEATURE_LTE_TO_HDR */

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Unrecognized timer",
                     timer_id);
      break;
  }
}

/*===========================================================================

FUNCTION HDRALMP_POWERUP_INIT

DESCRIPTION
  This function initializes the ALMP protocol.
  
DEPENDENCIES
  This function must be called before ALMP protocol is used

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_powerup_init( void )
{

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  boolean ue_init_redir_disable=FALSE;
#endif

  /* Initialize RX PER timer */
  rex_def_timer_ex(&hdralmp.fl_per_timer,
                   hdralmp_timer_cb,
                   HDRALMP_FL_PER_TIMER);

#ifdef FEATURE_LTE_TO_HDR
  rex_def_timer_ex(&hdralmp.get_cgi_timer,
                   hdralmp_timer_cb,
                   HDRALMP_GET_CGI_TIMER);
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  rex_def_timer_ex(&hdralmp.min_init_lte_meas_timer,
                   hdralmp_timer_cb,
                   HDRALMP_MIN_INIT_LTE_MEAS_TIMER);
#endif /* FEATURE_HDR_TO_LTE_REDIRECTION */

  hdralmp_send_per_rpt(HDRALMP_FL_PER_UNKNOWN);

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  /* enable UE init redirection by default */
  hdralmp.lte_redir_efs_info.ue_initiated_redir = TRUE;

  /* Read EFS item, for UE initiated redir */
  if ( efs_get( HDRALMP_UE_INITIATED_REDIR_DISABLED_FILENAME,
                &ue_init_redir_disable, 
                sizeof(ue_init_redir_disable) ) == 
                (int)sizeof(ue_init_redir_disable) )
  {
    if(ue_init_redir_disable == TRUE)
  {
      /* redir is disabled. Hence disable ue init redirection */
      hdralmp.lte_redir_efs_info.ue_initiated_redir = FALSE;
    }
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
      "EFS read value %d UE initiated D2L redirection flag = %d",
      ue_init_redir_disable, hdralmp.lte_redir_efs_info.ue_initiated_redir );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
      "UE initiated D2L redirection flag not available in NV, Enabled by default");
  }

  if ( efs_get( HDRALMP_AN_INITIATED_REDIR_ENABLED_FILENAME,
			   &hdralmp.lte_redir_efs_info.an_initiated_redir, 
			   sizeof(hdralmp.lte_redir_efs_info.an_initiated_redir) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.an_initiated_redir) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "AN initiated D2L redirection flag = %d",
	 hdralmp.lte_redir_efs_info.an_initiated_redir );
 }
 else
 {
   HDR_MSG_PROT( MSG_LEGACY_HIGH, 
	 "AN initiated D2L redirection flag not available in NV, disabled by default");
   hdralmp.lte_redir_efs_info.an_initiated_redir = FALSE;
 }

 if ( efs_get( HDRALMP_D2L_REDIR_MAX_RPT_CNT,
			   &hdralmp.lte_redir_efs_info.max_rpt_cnt, 
			   sizeof(hdralmp.lte_redir_efs_info.max_rpt_cnt) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.max_rpt_cnt) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "MAX_RPT_CNT read from EFS = %d",
	 hdralmp.lte_redir_efs_info.max_rpt_cnt );
 }
 else
 {
    hdralmp.lte_redir_efs_info.max_rpt_cnt = HDRALMP_D2L_REDIR_MAX_RPT_CNT_DEFAULT;
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "MAX_RPT_CNT could NOT be read from EFS. Default %d",hdralmp.lte_redir_efs_info.max_rpt_cnt);
 }

 if ( efs_get( HDRALMP_D2L_REDIR_SERVING_THRESH_TRIGGER,
			   &hdralmp.lte_redir_efs_info.serving_thresh_trig, 
			   sizeof(hdralmp.lte_redir_efs_info.serving_thresh_trig) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.serving_thresh_trig) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "SERV_THRESH_TRIGGER read from EFS = %d",
	 hdralmp.lte_redir_efs_info.serving_thresh_trig );
 }
 else
 {
    hdralmp.lte_redir_efs_info.serving_thresh_trig = HDRALMP_D2L_REDIR_SERVING_THRESH_TRIGGER_DEFAULT;
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "SERV_THRESH_TRIGGER could NOT be read from EFS. Default to %d",
       hdralmp.lte_redir_efs_info.serving_thresh_trig);
 }

 if ( efs_get( HDRALMP_D2L_REDIR_THRESHXQ_TRIGGER,
			   &hdralmp.lte_redir_efs_info.threshxq_trig, 
			   sizeof(hdralmp.lte_redir_efs_info.threshxq_trig) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.threshxq_trig) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "THRESHXQ_TRIGGER read from EFS = %d",
	 hdralmp.lte_redir_efs_info.threshxq_trig );
 }
 else
 {
    hdralmp.lte_redir_efs_info.threshxq_trig = HDRALMP_D2L_REDIR_THRESHXQ_TRIGGER_DEFAULT;
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "THRESHXQ_TRIGGER could NOT be read from EFS. Default to %d",
       hdralmp.lte_redir_efs_info.threshxq_trig);
 }

 if ( efs_get( HDRALMP_D2L_REDIR_THRESHX_TRIGGER,
			   &hdralmp.lte_redir_efs_info.threshx_trig, 
			   sizeof(hdralmp.lte_redir_efs_info.threshx_trig) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.threshx_trig) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "THRESHX_TRIGGER read from EFS = %d",
	 hdralmp.lte_redir_efs_info.threshx_trig );
 }
 else
 {
    hdralmp.lte_redir_efs_info.threshx_trig = HDRALMP_D2L_REDIR_THRESHX_TRIGGER_DEFAULT;
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "THRESHX_TRIGGER could NOT be read from EFS. Default to %d",
       hdralmp.lte_redir_efs_info.threshx_trig);
 }

 if ( efs_get( HDRALMP_D2L_REDIR_MIN_RPT_INT,
			   &hdralmp.lte_redir_efs_info.min_rpt_int, 
			   sizeof(hdralmp.lte_redir_efs_info.min_rpt_int) ) == 
			   (int)sizeof(hdralmp.lte_redir_efs_info.min_rpt_int) )
 {
   HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
	 "MIN_RPT_INT read from EFS = %d",
	 hdralmp.lte_redir_efs_info.min_rpt_int );
 }
 else
 {
    hdralmp.lte_redir_efs_info.min_rpt_int = HDRALMP_D2L_REDIR_MIN_RPT_INT_DEFAULT;
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "MIN_RPT_INT could NOT be read from EFS. Default to %d",
       hdralmp.lte_redir_efs_info.min_rpt_int);
 }

  if ( efs_get( HDRALMP_D2L_REDIR_MIN_INIT_LTE_MEAS_TIME,
                &hdralmp.lte_redir_efs_info.min_init_lte_meas_time, 
                sizeof(hdralmp.lte_redir_efs_info.min_init_lte_meas_time) ) == 
                (int)sizeof(hdralmp.lte_redir_efs_info.min_init_lte_meas_time) )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "MIN_INIT_LTE_MEAS_TIMER configured in EFS = %d",
      hdralmp.lte_redir_efs_info.min_init_lte_meas_time );
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
       "MIN_INIT_LTE_MEAS_TIMER not available in NV, default to %d",
       HDRALMP_D2L_REDIR_MIN_INIT_LTE_MEAS_TIME_DEFAULT);
    hdralmp.lte_redir_efs_info.min_init_lte_meas_time = HDRALMP_D2L_REDIR_MIN_INIT_LTE_MEAS_TIME_DEFAULT;
  }

 hdralmp.lte_cpfl_info.lte_is_in_home = FALSE;
 hdralmp.lte_cpfl_info.is_cpfl_valid = FALSE;
 hdralmp.lte_cpfl_info.num_pref_earfcns = 0;

 HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
      "UE-Init redirection enabled %d",hdralmp.lte_redir_efs_info.ue_initiated_redir);
 
  #endif /* FEATURE_HDR_TO_LTE_REDIRECTION */
}

/*===========================================================================

FUNCTION HDRALMP_SET_DEACTIVATE_NOTIFIER

DESCRIPTION
  This function sets the flags to inform MMoC that HDR is deactivated.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_set_deactivate_notifier( void )
{
  if (!hdralmp.deactivate.deactivation_cmd_is_pending)
  {
    /* catch all "autonomous deactivates" here */
    hdralmp.deactivate.deactivation_cmd_is_pending = TRUE;
    hdralmp.deactivate.trans_id_is_valid = FALSE;
  }
}

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRALMP_START_GET_CGI_TIMER

DESCRIPTION
  This function stops the LTE to HDR get CGI timer if it is not 
  active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_start_get_cgi_timer ( void )
{
  uint16 get_cgi_time_allowed = 0;

/*----------------------------------------------------------------------- */

  if (hdrmc_lte_to_hdr_get_cgi_is_in_progress())
  {
    hdrmc_get_duration_allowed_for_time_transfer( &get_cgi_time_allowed );

    /* We need to subtract the HDR activation and deactivation time from
     * the timer value given by LTE. */
    if ( get_cgi_time_allowed > HDRALMP_ACTIVATE_AND_DEACTIVATE_HDR_STACK_TIME )
    {
      get_cgi_time_allowed -= HDRALMP_ACTIVATE_AND_DEACTIVATE_HDR_STACK_TIME;

      HDR_MSG_PROT_1( MSG_LEGACY_MED, "Start LTE to HDR get CGI timer (%dms)", 
                      get_cgi_time_allowed );
    }
    else
    {
      /* This should not happen since LTE won't ask HDR to get CGI if there
       * is not much time left. */
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "get CGI timer (%dms) is too short", 
                      get_cgi_time_allowed );
    }

    (void) rex_set_timer( &hdralmp.get_cgi_timer, get_cgi_time_allowed );
  }

} /* hdralmp_start_get_cgi_timer() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_STOP_GET_CGI_TIMER

DESCRIPTION
  This function stops the LTE to HDR get CGI timer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_stop_get_cgi_timer(void)
{
  if( rex_get_timer( &hdralmp.get_cgi_timer ) !=0 )
  {
    /* stop the timer if it is active */
    HDR_MSG_PROT (MSG_LEGACY_MED, "Stop LTE to HDR get CGI timer");
    ( void ) rex_clr_timer( &hdralmp.get_cgi_timer );
  }
}/* hdralmp_start_get_cgi_timer() */
#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_HDR_TO_LTE
/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_RESEL_IS_IN_PROGRESS

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE reselection
  is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L reselection is in progress, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_is_in_progress( void )
{
  if ( hdralmps_get_state() == HDRALMPS_SUSPEND )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* hdralmp_hdr_to_lte_resel_is_in_progress() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_HDR_TO_LTE_RESEL_CMD_IS_SENT

DESCRIPTION
  This function returns TRUE or FALSE based on whether HDR to LTE reselection
  command is sent to LTE or not.
 
  HDRCP moves to SUSPEND state and deactivates HDRSRCH once D2L reselection
  starts. Once HDRSRCH is deactivated, HDRCP sends D2L reselection command
  to LTE. This functions can tell whether HDRCP is still waiting HDRSRCH to
  be deactivated.

DEPENDENCIES
  Need to call hdralmp_hdr_to_lte_resel_is_in_progress() first to ensure that
  D2L is in progress.

PARAMETERS
  None

RETURN VALUE
  TRUE if D2L reselection command is sent to LTE, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdralmp_hdr_to_lte_resel_cmd_is_sent( void )
{
  return hdralmp.lte_resel_info.resel_cmd_is_sent;
}/* hdralmp_hdr_to_lte_resel_cmd_is_sent() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRALMP_SET_PENDING_ABORT_RESEL

DESCRIPTION
  This function sets the value of pending_abort_resel flag.

DEPENDENCIES
  DO to LTE reselection should be already in progress. HDR SRCH has been asked
  to be deactivated but reselection command is not sent to LTE yet.

PARAMETERS
  Boolean value of disired pending_abort_resel flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdralmp_set_pending_abort_resel
(
  boolean value
)
{
  hdralmp.lte_resel_info.pending_abort_resel = value;
  if(hdralmp.lte_resel_info.pending_abort_resel == TRUE)
  {
     hdrlmac_abort_reselect_to_lte(); /* SRCH is in the process of deactivation. 
                   Inform HDR SRCH that reselect procedure is being aborted */
  }
}/* hdralmp_set_pending_abort_resel() */

/*===========================================================================

FUNCTION HDRALMP_GET_RESELECT_EARFCN

DESCRIPTION
 This function gets the current earfcn on which D2L reseleciton is attempted.
  
DEPENDENCIES

PARAMETERS
  None

RETURN VALUE
  earfcn - on which D2L is in progress

SIDE EFFECTS
  None

===========================================================================*/
lte_earfcn_t hdralmp_get_reselect_earfcn( void )
{
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "current reselected EARFCN %d",hdralmp.lte_resel_info.earfcn);  
  return hdralmp.lte_resel_info.earfcn;
}
#endif /* FEATURE_HDR_TO_LTE */


/*===========================================================================

FUNCTION HDRALMP_PROCESS_PSM_REQ

DESCRIPTION
  This function process the PSM READY REQUEST from CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_process_psm_req(void)
{

 /* current state */
 const hdralmps_state_enum_type cur_state = hdralmps_get_state();
 cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 HDR_MSG_PROT (MSG_LEGACY_HIGH,  "Rcvd CM_HDR_PSM_READY_REQ");
 if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
 {
    cmd_ptr->psm_resp_info.hdr.cmd = CM_HDR_PSM_READY_RSP_F;
 }
 else
 {
    ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    return;
 }

 
  if((( cur_state == HDRALMPS_CONNECTED ) ||  	  /* UE is connected */
      (hdrscp_session_negotiation_in_progress() == TRUE ))&& /* session negotiation is in progress */
     (cur_state != HDRALMPS_INACTIVE)) /* current state not inactive */
  {
     //PSM not ready
     hdralmp.psm_report_ind_pending = TRUE;       
     cmd_ptr->psm_resp_info.status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
  }
  else
  {
    //PSM ready
    cmd_ptr->psm_resp_info.status = SYS_PSM_STATUS_READY;

  }

   HDR_MSG_PROT_1 (MSG_LEGACY_HIGH,  "Sent CM_HDR_PSM_READY_RSP_F with status %d",
                   cmd_ptr->psm_resp_info.status);
   hdralmp_cm_mc_rpt( cmd_ptr );
	

}/* hdralmp_process_psm_req() */



/*===========================================================================

FUNCTION hdralmp_send_psm_ready_ind_to_cm

DESCRIPTION
  This function sends PSM ready indication to CM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdralmp_send_psm_ready_ind_to_cm( void )
{
  cm_mc_rpt_type *cmd_ptr;         /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 if(hdralmp.psm_report_ind_pending == TRUE)
 {
   if ( ( cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal() ) != NULL )
   {
     cmd_ptr->psm_ind_info.hdr.cmd = CM_HDR_PSM_READY_IND_F;
     HDR_MSG_PROT(MSG_LEGACY_HIGH,  "Sent CM_HDR_PSM_READY_IND_F");
     hdralmp_cm_mc_rpt( cmd_ptr );
   }
   else
   {
     ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
   }
  }
  else
  {
     HDR_MSG_PROT(MSG_LEGACY_HIGH,  "Ignored sending CM_HDR_PSM_READY_IND_F to CM !!");
  }
 
  hdralmp.psm_report_ind_pending = FALSE;

} /* hdralmp_send_psm_ready_response_to_cm() */