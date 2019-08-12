#ifndef L1RRCIF_H
#define L1RRCIF_H
/*===========================================================================

             L 1   T A S K   S A P   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the L1's control service access point (SAP).

  Copyright (c) 2000-2014 by QUALCOMM Technologies Incorporated.
  All Rights Reserved.

TABLE OF CONTENTS

    +-Constant Declarations
    | |
    | |__Non-Specification Related Constants
    | |
    | |__Specification Related Constants
    |    |
    |    |__10.3.10  Multiplicity values and type constraint values
    |    |
    |    |__Constants declared directly in IEs
    |
    |
    +-Enumerated Type Declarations For Commands
    | |
    | |__Specification Related Message IEs (Information Elements)
    | |
    | |__Non-Specification Related L1 - RRC Message Information Structures
    |
    |
    +-Structure Type Declarations For Commands
    | |
    | |__Non-Specification Related L1 - RRC Message Information Structures
    | |
    | |__Specification Related Message IEs (Information Elements)
    |
    |
    +-Definition of L1 Commands from RRC
    | |
    | |__CPHY_CAPABILITY_REQ
    | |
    | |__CPHY_IDLE_REQ
    | |
    | |__CPHY_DEACTIVATE_REQ
    | |
    | |__CPHY_STOP_WCDMA_MODE_REQ
    | |
    | |__CPHY_FREQ_SCAN_REQ
    | |
    | |__CPHY_ACQ_REQ
    | |
    | |__CPHY_CELL_PARM_REQ
    | |
    | |__CPHY_IMMEDIATE_CELL_EVAL_REQ
    | |
    | |__CPHY_MEASUREMENT_REQ
    | |
    | |__CPHY_HARD_HANDOFF_REQ
    | |
    | |__CPHY_SET_SFN_REQ
    | |
    | |__CPHY_SET_CFN_REQ
    | |
    | |__CPHY_BCH_SCHEDULE_REQ
    | |
    | |__CPHY_SET_REQ
    | |
    | |__CPHY_ASET_UPDATE_REQ
    | |
    | |__CPHY_CELL_TRANSITION_REQ
    | |
    | |__CPHY_DRX_REQ
    | |
    | |__CPHY_CIPHERING_KEY_REQ
    | |
    | |__CPHY_CELL_SELECTION_REQ
    | |
    | |__CPHY_IMMEDIATE_MEAS_REQ
    | |
    | |__RRC_CPHY_MEAS_CTRL_CNF
    | |
    | |__CPHY_INTEGRITY_KEY_REQ
    | |
    | |__CPHY_INTEGRITY_KEY_MAUC_REQ
    | |
    | |__CPHY_IMMEDIATE_RESELECTION_REQ
    | |
    | |__CPHY_ACT_TIME_CANCEL_REQ
    | |
    | |__CPHY_ACTION_CANCEL_REQ
    |  
    |  
    +-Definition of L1 Indications and Confirmations to RRC
    | |
    | |__CPHY_MEASUREMENT_IND
    | |
    | |__CPHY_CELL_RESELECTION_RSP
    | |
    | |__CPHY_ERROR_IND
    | |
    | |__CPHY_IDLE_IND
    | |
    | |__CPHY_DEACTIVATE_IND
    | |
    | |__CPHY_FREQ_SCAN_CNF
    | |
    | |__L1_ACQ_CNF
    | |
    | |__L1_DRX_CNF
    | |
    | |__L1_SETUP_CNF
    | |
    | |__L1_CELL_SELECT_CNF
    | |
    | |__CPHY_ACT_TIME_CANCEL_CNF
    | |
    | |__L1_CELL_RESELEC_IND
    | |
    | |__L1_IMMEDIATE_CELL_EVAL_CNF
    | |
    | |__L1_SET_SFN_CNF
    | |
    | |__L1_SET_CFN_CNF
    | |
    | |__L1_MEASUREMENT_IND
    | |
    | |__L1_CAPABILITY_CNF
    | |
    | |__L1_INTG_KEY_MAUC_CNF
    | |
    | |__L1_OUT_SYNC_IND
    | |
    | |__L1_IN_SYNC_IND
    | |
    | |__L1_PHYCHAN_EST_IND
    | |
    | |__L1_IMMEDIATE_RESELEC_CNF
    | |
    | |__L1_IDLE_CNF
    | |
    | |__L1_CELL_TRANSITION_CNF
    | |
    | |__RRC_CPHY_COMPRESSED_MODE_IND
    |
    |
    +-RRC Interface Function Prototypes
    | |
    | |__l1_get_rssi()
    | |
    | |__l1_get_eutra_measurement_data()
    | |
    | |__l1_get_interrat_measurement_data()

    
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/l1rrcif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/25/16   sp      Made changes to expose RRC API to check if trch is mapped to voice rab
03/20/16   nga     Do not clear nbr list if reselection failed due to INTERNAL_FAIL_CAUSE from RRC
03/03/16   nr      Exposing RRC API to check if RRC is waiting for L2 ACK from NW
02/19/16   nga     Adding one more resel fail cause L1_RESEL_FAIL_L2_ACK_WAIT
12/02/15   nga     setting M2M DRX cycle length based on scaling factor
11/28/15   svh     Expose new API to RRC to check whether MDSP Recovery is in progress.
10/08/15   nga     FR 23268: Configurable scaling of IDLE DRX for M2M devices (WCDMA)
                   FR 23280: Skipping IDLE mode measurements for M2M devices (WCDMA)
09/09/15   gm      Mods to add an API to reutrn default band to RRC.
04/14/15   sp      Made changes to indicate Multi rab status to L1
03/30/15   sr      Changes to extern is_sleep_drv_not_in_sleep
03/03/15   db      Correction in logging of resel end event
12/21/14   gm      Mods to take care of delay that may be caused after timer expiry for QTA.
12/10/14   sp      Made changes to revert reordering of structures
12/03/14   sp      Made changes to save one byte status in cmd_ptr to crash debug global
11/24/14   sd      wl1_qta_overhead_time_in_ms comments
09/25/14   pv      Support for DRX Page indication for true page also. 
03/09/14   kr      adding new cmd so as to abort for BPLMN
09/16/14   pkj     Optimization: HARQ macro re-defined
09/03/14   dm      DSDS: Increase the QTA overhead time by 2 ms to account for
                   delay in response from LTE during W2L CM.
08/21/14   dm      DSDS: Increase the QTA overhead time by 2 ms to account for
                   lock acquisition and release delays for W2L CM gap.
08/18/14   vr      Cleanup code for HSRACH 'conventional' seamless recfg.
07/31/14   gm      Changes to reduce the text size.
07/31/14   sl      Changes for new API which returns SFN of given cell
07/28/14   pkg     Code Check-in for 16QAM feature.
07/01/14   hk      Header file inclusion cleanup
06/23/14   ar      Changes to support AbsPrio Tresel scaling factor for GSM and LTE
06/25/14   rs      Externed L1 API for blocking/unblocking CTCH
06/24/14   gsk     New enum to unbar the freq 
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
06/20/14   sa      Made the changes to support UL 16QAM
10/14/13   tsk     Slot formats 5, 5a and 5b are removed as per spec 25.211 v8. 
06/12/14   gsk     Adding enum types for resel fail causes
05/30/14   sa      Made changes to support 64x64 single PS RAB detection
05/26/14   ag      R10 periodic measurement support on second carrier.
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/06/14   sad     Fix to disable sec carrier after completion of cphy if post verification
                   failure happens in middle of cphy processing
04/27/14   jkb     Add check for Feature dual sim with feature qta
04/21/14   svh     Fixed KW errors.
04/16/14   bj      Split ACQ code cleanup
04/15/14   as      Fixing Bolt 2.0 DS compilation errors.
04/11/14   as      Mods to support opportunistic SGLTE/SGTDS
04/03/14   ymu     Fix Compilation errors for EDPCCH_BOOST 
02/05/14   sad     L1-RRC interface change and compaction cphy request
08/13/13   ad      Adding support to provide tx div info from sib5 in cell tran req 
10/30/14   gsk     Adding API to let RRC know if cell bar is on a USET cell
10/21/13   kr      Added an API to let WL1 know BPLMN is active in RRC
01/24/14   dm      DSDS: Reduce the overhead time for QTA in DCH.
01/16/14   jk      Decreased the Maximum cell to be reported back to 4, since the spec states only 4.
01/06/14   vn      Added interface variables for Field Test Mode Feature.
12/17/13   yw      Enabling Rel7 E-DPCCH boost feature and Rel7 MPR  
12/11/13   gp      FR:3693 EUL stats API for data services
12/12/13   mk      Included a flag to indicate whether the reselection triggered is on a CSG cell or not
12/09/13   jk      Increased the Maximum cell can be reported to 9.
11/18/13   ag      Added changes to support NMR UTRAN Spec Test Cases.
11/16/13   hk      Reduced CPHY SETUP timeout
11/14/13   as      W2G TA: Adding code for RRC dependency.
11/07/2013 nd      Added support for QMI Extension for Detailed Protocol Stack Information
11/05/13   hk      Removed the dependency on FW interface
10/30/13   bs      Extended TSN support added for 3C
10/29/13   sad     Inital check in for Secondary ASET update changes
10/15/13   ar      Support for FEFACH (FR2759) absolute priority measurements in FACH
10/10/13   mm      Interface for passing UE mode if SGLTE to WL1
09/24/13   vi      Added changes for FE-FACH
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/18/13   vs      HS redesign changes.
09/12/13   jd      Searcher redesign 
08/19/13   vn      Added enum member to indicate RRC whether ACQ failure 
                   is due to band mismatch or not.
07/18/13   rs     Support for W barring cause to be sent to WRRC  
07/11/13   ar      Added max number of scrambling code types in enumeration and number 
                   of words per ciphering key definition
07/05/13   stk/mk  FR 2677 - Initial version. Support for W2LTE CSG.
06/28/13   mk      Introducing flag to indicate whether the UE is in manual CSG mode.
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
06/14/13   jhl     Added extern API declaration from RRC to get CTCH trch id
06/12/13   amj     Added API for returning HS PSC and PN Pos for SIB reading
05/28/13   jk      Increased L1_MAX_MEAS_ID from 16 to 32 under FEATURE_WCDMA_REL9 
05/22/13   oh      Adding DC-HSUPA framework support
05/23/13   geg     WSW decoupling (except for component of QCHAT)
05/20/13   sad     Support for Enhanced Layer-1 Manager
04/29/13   gp      Defining the L1_UL_TFC_MIN for validating the NUM_TFCI
04/10/13   abs     Added start_qta structure to fix compilation error
03/26/13   sad     Changes to handle new RRC-L1 interface.
03/06/13   vr      Code changes for HS RACH opt handling CU CNF
02/12/13   rsr     Declared an API which checks if SRBs are mapped to HSDSCH or no 
02/10/13   pr      Added new constants for L1-RRC Timers
02/05/13   ar      Added prototype for getting OOS timer from RRC
01/21/13   stk     Support for Femto/CSG selection and reselection
01/18/13   vn      Declared additional measurement type to enhance quality 
                   measurement reports.
01/11/13   scm     More partial FEATURE_DUAL_SIM support for DSDA.
12/24/12   vs      Added a new macro to check if this is only a Efach call.
12/10/12   scm     Partial FEATURE_DUAL_SIM support for DSDA.
11/29/12   gsk     Fix for a compiler warning
11/15/12   ag      Added data structure to pass LTE cells info from L1 under 
                   feature FEATURE_MODEM_STATISTICS
09/24/12   nd      Added an element in cell_bar_req interface for RRC to 
                   indicate if a cell is reserved for operator use.
10/19/12   pr      Added elements to l1_rssi_info_struct_type.
10/15/12   jd      Memory Optimization Changes
09/24/12   zr      Adding DC ASET max size macro
09/19/12   zr      Assigning types to anonymous structs used to report EUTRA msmts
09/07/12   jhl     Moved function rrcllc_check_srb_mapped_to_trch here from
                   rrcllcoc.h
08/22/12   gv      Provided an API to check if EDCH rsrc is active or not
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr      Adding feature definitions for Dime
06/19/12   vr      HSRACH code update
06/13/12   kcm     Trigger DL opts for X2W HHO only for R99 config.
06/08/12   gv      Feature HS RACH Initial code check in
03/30/11   sv      Added variable to retain the pn_pos when we are in BPLMN search
03/30/12   zr      Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY.
03/28/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
03/22/12   vs      MIMO related changes for the max harq processes.
03/21/12   ms      Included macl1rrcif.h
03/16/12   ms      Added support for FEATURE_WCDMA_HS_FACH
03/06/12   zr      Removing FEATURE_WTOL_PS_HANDOVER featurization
02/27/12   zr      Adding interface to retrieve timing info for LTE cells during HHO
01/31/12   stk     Changes for E-UTRA detection under "FEATURE_EUTRA_DETECT"
01/25/12   oh      Fixed compilation error.
01/25/12   at      Complete support for Back up and revert of HS SCCH orders 
                   during HHO failure and revert back.
01/24/12   bs      compilation error resolved.
01/20/12   bs      free floating changes.
12/21/11   stk     Added support for Rel9-RSRQ based W2L reselection. 
                   New L1-RRC interface variables added for criteria 4 and 5 calculation.
11/03/11   amj     Removed ifdefs such that file is CMI compliant.
10/26/11   oh/amj  Merged code to support CPC DTX and CPC DRX features.
10/06/11   mc      Changes for CMI compilation
09/29/11   sp      Remove featurization of THERMAL MTPL_BACKOFF in interface file
09/29/11   kcm     Made the MAX_DEFAULT_EFDPCH_OFFSET macro independent of 
                   FEATURE_WCDMA_EFDPCH 
09/21/11   sp      Added NPA Support for Thermal Mitigation under FEATURE_WCDMA_NPA_THERMAL
09/13/11   sa      Removed compilation error by introducing code for 
                   FEATURE_MODEM_STATICS.
09/08/11   srk     Added the missing command types to l1_cmd_enum_type
08/24/11   hk      Added command id parameter while sending the error
                   indication to higher layer
09/06/11   uk      Adding new BPLMN search type to distinguish automatic BPLMN 
                   search from manual 
08/24/11   sa      Removed compilation warnings.
08/16/11   sj      Added CPHY_DC_CFG_CMD data structure used by WPLT 
07/18/11    ks      Updated default FDPCH offset to 306944 according to Rel-8 RRC spec 
07/08/11   mk      Fixed compilation error.
05/16/11   uk      Change return type of RRC nudge API.
05/31/11   geg     Changes for RRC and L2 api files clean-up wcdma dependencies
04/28/11   sj      upmerge from offline/main
04/21/11   gnk     Adding an RRC L1 API to check if we are using AMR call
04/19/11   sv      Added support for FEATURE_WCDMA_GET_RAT_MEAS_INFO.
03/30/11   gnk     Mainlined several features
03/11/11   dp      Ported CPC DRX changes to offline Architecture
03/02/11   ms      Moved extern of hs_get_dc_state and hs_get_mimo_state from
                   hscfg.h to l1rrcif.h for RRC to be able to invoke these API
02/24/11   sb      Featurized changes to l1_rssi_struct under FEATURE_WCDMA_DIAG_QUERY_CELL_INFO
02/24/11   nd      Added some missing definitions for Additonal channels on BC19.
02/18/11   sb      Made changes to RSSI INFO struct to support the new QXDM command for 
                   Querying the Network Meas parameters.
02/03/11   nd      Added support for Band-BC11
01/18/11   dm      Made changes to the API dl_phychan_cctrch_id_in_use to
                   accommodate the case when EFDPCH is enabled.
01/10/11   stk     Removed the dependency on wsrchltedrv.h, which needs files in \src to compile. 
11/24/10   sa      Added code to support feature "FEATURE_WCDMA_CM_LTE_SEARCH".
11/02/10   nd      Added code changes for FEATURE_WCDMA_DEFERRED_SIB11_12_READING
09/02/10   gv      Code Checkin for CPC DTX
08/13/10   stk     Added a new API to find whether a given Freq/PSC is barred and 
                   if yes, return the remaining tbar timer for that cell. 
08/08/10   sj      Added unified PLT support
06/16/10   stk     changed the SIB19 priority to int8 to store the invalid value of -1. 
06/03/10   gkg     Added new i/f for signaling Different Tx Diversity mode for DC calls.
                   Changes are under feature FEATURE_WCDMA_DC_HSDPA_DIFF_TXD_MODE.
05/27/10   sa      Moved reporting of GSM cells for FEATURE_MODEM_STATISTICS under
                   FEATURE_INTERRAT_HANDOVER_WTOG to fix the compilation errors.
05/27/10   sa      Added new API to support feature FEATURE_MODEM_STATISTICS. 
05/21/10   nd      Added support for Band BC11.
03/30/10   mr      Added support for reporting average CQI for UI display
04/19/10   mc      Added support for MIMO codebook restriction
04/19/10   stk     Added WCDMA_TO_LTE / Rel8 interface support for cell reselection.
03/16/10   mr      Changed the data type of s_cpich_po from uint8 to int8 
11/17/09   mr      Fixed MOB errors
11/16/09   mr      Added s_cpich_po in l1_dl_hs_mimo_info_struct_type to resolve MOB errors
11/13/09   sa      Release 8 min Beta_ed support: Added an element in EDPDCH info of 
                   L1 setup command to hold beta_ed,k,reduced,min configured by RRC
11/07/09   hk      Added the support to enable DC on WPLT test mode
11/06/09   mr      Handle MIMO S_CPICH power Imbalance from RRC
11/06/09   hk      Added the support to enable DC on R99
10/28/09   sa      Release 8 min Beta_ed support: Added an element in EDPDCH info of 
                   L1 setup command to hold beta_ed,k,reduced,min configured by RRC
09/23/09   stk     Added new API to clear all barred cells, when UE hit Soft-reset or LPM.
09/02/09   sa      EDPCCH power boost feature support: Added a structure to hold 
                   E-DPCCH boost information in CPHY_SETUP_REQ command structure. 
02/02/09   nd      extend the LFS feature for conventional OOS under FEATURE_LFS_COOS
06/26/09   sa      Added a unsupported MCM failure cause in l1_failure_cause
                   enum type.
05/22/09   oh      Support for Rel-7 EDPDCH power interpolation feature.
03/30/09   rm      Added new featurization for FEATURE_CMI.
03/18/09   yiz     Add debug capability to reset RTR after string of RRC Connection
                   failures
02/18/09   prk     Changed the declaration of Inline functions to pass offline builds.
01/19/09   stk     Added support for R7 spec CR High Mobility Indicator( spec CR 2948(RP-060712))
12/09/08    uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                    FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                    and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
12/09/08   uk      Added non_eq_plmn_barred to l1_cell_bar_cmd_type
10/03/08   hk      Bringing in 7k Mailine fixes
07/30/08   mc      Added support for MIMO configuration.
07/11/08   ms      Replaced __inline with INLINE for Q6.
11/12/07   rc      Added prototype for l1_get_data_for_ui_display() under
                   FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY.
11/11/07   ub      MBMS:inlcuded cfg and add freq and rf chan in l1msetup cmd
11/07/07   stk     Removed FEATURE_WCDMA_L1_DUMP_DEBUG_DATA_TO_EFS and code inside this feature
10/24/07   vsr     Mainlining MBMS and Enhanced DL changes 
10/16/07   m       Added a new RRC ACQ command type to support DCH-FACH
                   ACQ SRCH retry
08/30/07   rc      Added prototype for function
                   srchmeas_set_inter_freq_measdb_status_during_state_transition()
08/14/07   vb      defined prototype l1_cm_query_compressed_mode_active and
                   l1_cm_query_num_of_cm_patterns_active
08/06/07   vsr     Included l1dlphychancfg.h and nv_items.h instead of l1cmmgr.h
                   because cm related variables were moved.
07/16/07   nd      Added interface for inter freq rach reporting support
06/28/07   rc      Defined WCDMA bands used by L1 and RRC for frequency scan.
06/04/07   sk      Added FEATURE_SUPPORT_FOR_PARTIAL_ROAMING or CR151
05/17/07   nd      added prototype for l1_get_rssi_during_wtow_bplmn()
05/16/07   npr     Added FEATURE_FREQ_SCAN featurisation around l1_srchfs_list_scan_direct()
05/02/07   sk      Added extern rrc_sib_can_l1_start_fmo()
03/05/07   sk      Added code for flexible deep sleep and 
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2 features.
01/31/07   nd      report ecio to RRC in l1_get_rssi() under FEATURE_UMTS_REPORT_ECIO
01/22/07   nd      Event 1J,added the enum for event 1J.
12/22/06    ksr     Rel-6 post-verification, skip sync A changes
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_OPTIMIZE_SEARCH_FOR_HHO
                    -FEATURE_BARRED_CELL_DURING_CELL_SELECTION
09/15/06   am      Changed L1_SG_STEP_THRESHOLD_NOT_PRESENT to 0 from 0xFF
09/12/06   scm     Adding some data types to support interface with TRM.
09/08/06   gv      Fixed an issue in the macro CPHY_SETUP_REQ_E_MASK_IS_EUL_NOOP.
09/05/06   ms      Featurized routine 'hs_is_supported' under FEATURE_HSDPA.
08/29/06   ms      Provided API 'hs_is_supported' for RRC to see whether HS is
                   supported on the particular chip.
08/23/06   ms      Added new API to probe if UL physical channels are active
08/22/06   rc      Added meas_cells_list structure to inter-freq cell info list. 
08/22/06   rc      Declared srchmeas_save_inter_freq_measdb_during_state_transition()
                   function to store the inter-freq meas db during dch-fach-dch transition.
08/18/06   ms      Added function prototypes for ul_phychan_is_ul_dpch_active
                   and dl_phychan_is_dl_dpch_active.
08/14/06   yiz     Mainline HSUPA code
07/07/06   rmak    Extern'd l1m_is_connected() and l1m_state_is_fach() API calls
05/11/06   sk      Rel5 CR 136. Add tresel scale factors and non-hcs fast move detect vars.
05/03/06   nd      l1_get_rssi() prototype has been changed and a new 
                   structure has been defined as argument to the function
03/17/06   vb      Added Call type bit mask to l1_setup_cmd_type
03/10/06   ub      Changed Treselection unit from seconds to milliseconds
02/27/06   rc      Added enum  L1_FREQ_WCDMA_BAND_WCDMA_1700_2100 to 
                   l1_freq_scan_band_enum_type.
02/20/06   gv      Added a new enum l1_suspend_resume_cause_enum_type.Modified 
                   the function prototype for rrc_can_l1_go_to_sleep() so that 
                   status whether the W2G BPLMN is ongoing or not.
02/17/06   rc      Added enum  L1_FREQ_WCDMA_BAND_WCDMA_1700 to 
                   l1_freq_scan_band_enum_type.
01/11/06   bd      Lint fixes
01/05/06   scm     Change SIB sleep feature name to FEATURE_INTER_SIB_SLEEP.
11/28/05   rc      Added enum  L1_FREQ_WCDMA_BAND_WCDMA_900 to 
                   l1_freq_scan_band_enum_type.
11/11/05   mg      Added rrcpg1_modify_sibs_for_sfn_update prototype.
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
11/04/05   mc      Added function prototype to query current HS state.
10/07/05   ub      Added cell cell_type and gsm_info to transition request
10/04/05   vk/rc   Added action L1_MEAS_DELETE_ALL in type l1_meas_trans_enum_type
09/25/05   kps     Preliminary SIB sleep optimization support. 
                   Code changes are inside #ifdef FEATURE_SIB_SLEEP_OPTIMIZATION
09/20/05   cpe     Rel5 support for delta rscp from l1 to RRC
09/13/05   nd      added L1_MEAS_EVENT_ASET_MSET_DSET_TRIG for 1d trigg cond
08/23/05   asm     Add support for FEATURE_WCDMA_LOWER_POWER_FOR_AMR.
07/25/05   sk      Added resel ind strcuture to piggy-back TRANS_CNF
08/02/05   rc      Changed L1_WCDMA_ACQ_DET_LIST_MAX to 5 to allow upto 5
                   cells to be reported to RRC when srch acq mode is 
                   L1_WCDMA_ACQ_DET_LIST.
07/29/05   cpe     Replaced FEATURE_HSDPA with FEATURE_REL5
07/06/05   vp      Code changes to support new DL sync parms received in a 
                   CPHY_UPD_DL_SYNC_PARMS message. RRC sends this message
                   when it has received a UTRAN Mobility Information message.
07/05/05   rc      Defined L1_NO_CNF_TRANSACTION_ID which indicates that no 
                   confirmation need to sent to RRC.
07/03/05   rc      Added L1_RPT_INTERVAL_INVALID enum to l1_reporting_interval_enum_type.
06/21/05   cpe     Added support for trying acquisition on the best cell in the active
                   set on DCH->Idle transition. Added prototype for new function 
                   l1_get_best_psc_in_aset().
06/20/05   sk/vm   Defined new function wl1_check_barred_cell() and modified
                   enum l1_cell_bar_info_e_type to include not-barred case.
05/25/05   sk      Added enum L1_WCDMA_INTER_S_RESEL_ACQ to differentiate inter-s 
                   reselection and handover in ACQ_REQ
05/12/05   sk      T-barred Feature: Added new enum for type of cell barring in 
                   cell bar req. Removed intraf_barred flag.
05/11/05   kps     lint error fix.
05/05/05   vb      Added new field in setup cnf to indicate what kind of setup 
                   status out of validation fail or setup fail or setup success
05/04/05   sk/sh   Added T-barred Feature support.New Cell bar req struct and
                   a element unbar_freq in cphy_acq_req
04/05/05   ub      Added hcs_used flag in DCH to FACH transition structure
04/05/05   gv      Removing the feature flag FEATURE_L1_CELL_SELECTION as  
                   it was always defined and also the code under the #else
                   has been removed.
04/14/05   rc      Added a new field bler_present to 
                   l1_transport_channel_bler_struct_type and bler_rpt_requested
                   to l1_qual_meas_ctrl_struct_type.
04/11/05   vb      Added hho_req_mask in setup_req to support resuming to old config upon HHO failure. 
04/07/05   m       Added support for FEATURE_OPTIMIZE_SEARCH_FOR_HHO
04/06/05   ub/ba   HCS based cell reselection feature (FEATURE_WCDMA_HCS)
03/22/05   vk/bbd  Added SDU Time parameter under FEATURE_DETECT_MISSED_ACT_TIME 
                   in l1_cm_status_info_struct_type structure
03/10/05   vk/cpe  Added new Event 1D parameters under FEATURE_HSDPA
03/07/05   rc      Removed the featurization for Quality Measurements support added
                   in previous check-in.
03/07/05   rc      Added support for Quality Measurements feature under
                   feature name FEATURE_WCDMA_QUALITY_MEAS.
03/07/05   cpe     Added a flag measure_in_idle_pch_fach for each of the intra-freq,
                   inter-freq and inter-RAT cell addition sturctures which
                   are sent to L1 via CPHY_MEASUREMENT_REQ command. A cell is
                   measured in non-DCH states only when this flag for the cell is
                   true.
01/04/05   rc      Defined L1_MAX_INTERRAT_EVENTS to indicate the events 3A-3D.
11/29/04   bu      Added l1_dl_weak_ind_type.
11/19/04   yiz     Support DYNAMIC_POWER_CLASS
11/11/04   rc      Defined enum l1_wcdma_srch_enum_type for BPLMN support and  
                   added this to l1_acq_cmd_type and l1_freq_scan_cmd_type 
                   structures. Featurised the changes under 
                   FEATURE_GTOW_BACKGROUND_PLMN_SEARCH.
11/09/04   yiz     Add FEATURE_WCDMA_PRACH_SELECTION support.
09/15/04   cpe     Added a new field apply_agc_thresh to l1_acq_cmd_type structure
08/30/04   gs      Added interface definitions for HSDPA
08/24/04   rc      1. Added l1_freq_scan_band_enum_type enum to the 
                      l1_freq_scan_cmd_type structure.
                   2. Added new enum l1_freq_scan_band_enum_type for additional 
                      channel scan support.
                   3. Added L1_FREQ_ADDITIONAL_CHANNEL_SCAN to enum 
                     l1_freq_scan_step_enum_type 
08/19/04   cpe     Removed L1_RSSI from l1_meas_quan_enum_type
07/21/04   vk      Declared a new function rrc_is_gsm_cell_present_in_sibs()
                   that indicates to L1 if GSM cells are present
06/30/04   scm     Remove trk_lo_adj and trk_lo_adj_present from l1_acq_cmd_type.
06/25/04   scm     Mainline FEATURE_L1_ACT_TIME.
04/14/04   bu      Added is_list_search field in l1_freq_scan_cmd_type and
                   l1_acq_cmd_type. This field will be TRUE for PLMN LIST searches
                   and FALSE otherwise.
04/16/04   asr     Added L1_STUCK_IN_L1M_IDLE to l1_op_err_source_type.
04/15/04   yiz     Rename FACH inter-RAT CR as FEATURE_CELL_FACH_MEAS_OCCASION
04/12/04   src     Support for multiple neighbour finger and triage.
04/08/04   scm     Implement mdsp_is_wcdma_image_loaded(), returns boolean.
03/22/04   sh      Added SRCH_CMD_TIMEOUT to l1_op_err_source_type.
03/16/04   yiz     Add interface to disable UL power TFC selection for AMR.
02/29/04   bu      Added fach_meas_incl and l1_fach_meas_info_struct_type to
                   l1_meas_trans_type.
02/16/03   asr     Added l1_op_error_ind_type
01/28/04   scm     Extern wl1_dump_debug_data_to_efs() function.
01/06/04   vk      Changed reporting of static cells in Inter Freq Measurement
                   report to dynamic reporting
12/18/03   yiz     Add inter_s field to l1_meas_trans_type for INTER RAT cell
                   info list
12/10/03   yiz     Add FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
11/07/03   yus     Added support for GSM Quad-band. 
10/30/03   cpe     Added enum type l1_wcdma_filter_coef_enum_type 
10/29/03   scm     Add l1_get_reference_cell_info(uint16 *freq, uint16 *psc).
10/23/03   scm     Add l1_wcdma_get_trk_lo_adj_pdm() for VCTCXO PDM retrieval.
10/17/03   cpe/vk  Added support for inter frequency measurement interface
10/17/03   cpe/vk  Added support for internal measurement interface.
09/12/03   m       Added missing feature flag around the new code
09/09/03   m       Modifications to support Inter Freq HHO
06/04/03   sh      Interface changes to return list of candidate cells to
                   acquire to CPHY_ACQ_REQ and CPHY_ACQ_CNF:
                   - added enum L1_WCDMA_ACQ_DET_LIST to indicate this mode
                   - added new acq mech type L1_WCDMA_FREQ_SCR_POS
                   - new struct l1_acq_scr_code_det for list of candidate cells
                   - added number and list of scrambling codes detected to
                     l1_acq_cnf_type.
05/15/03   djm     lint corrections for last check-in
05/06/03   djm     Addition of L1_TGPS_TYPE() inline function/macro to be used
                   by l1cmmeas to classify a TGPS during TGPS collision if there
                   are no "correspoinding" MeasIds, all Inter-FREQ or Inter-RAT
                   measurements will be removed.
04/22/03   cpe/vk  Added prototype of l1_get_additional_intra_freq_msmt_results() 
                   used for getting intra freq additional measurement info.
04/14/03   vk      Removed inter_f, inter_freq_meas_info_incl from 
                   l1_meas_sib_parm_struct_type structure to reduce the
                   size of measurement command that goes from RRC to L1.
                   A new command to send Intre-Freq cells from SIB's
                   would be sent from RRC to L1.
04/09/03   scm     Remove FEATURE_WCDMA_COMP_GF.  Feature always supported.
04/02/03   scm     Change pri_scr_code to scr_code in l1_acq_cmd_type.
03/27/03   vk/cpe  Changed the syntax of function l1_get_stored_intra_freq_meas()
03/27/03   cpe     Changed the variable meas_rpt in l1_measurement_ind_type 
                   to a pointer
03/24/03   scm     Add sys_stop_mode_reason_e_type to l1_deactivate_cmd_type.
03/21/03   scm     Adding additional fields to l1_acq_cmd_type from RRC.
02/25/03   vk      Declared function l1_get_stored_intra_freq_meas(..) that
                   gives RRC the saved measurement criteria for an intra freq
                   measurement identity
02/13/03   djm     add an INVALID RPT MODE to the report mode enum type.
                   this is used to identify unconfigured measurement events
02/04/03   scm     Remove #ifdef FEATURE_CELL_RESELECT_INTER_S from acq_mode_type.
01/16/03   yiz     Moved declaration of rrc_ul_tx_pwr_ind() from rrcllc.h into
                   this header to fix a compiler warning against enc.c.
01/02/03   ddh     Added changes for 100 kHz frequency scan, this included
                   increasing the L1_MAX_FINE_SCAN_FREQ from 11 to 21
11/26/02   vk      Removed addl_meas from l1_meas_rpt_struct_type structure
                   as it is not required. Also the removal results in 
                   reducing the size of Measurement Report by 4 times
11/22/02   cpe     Added more types to the structure l1_wcdma_acq_mode_type 
                   to support GSM to WCDMA idle mode cell reselection
11/15/02   ddh     Removed the cell reselection use of #ifdef 
                   FEATURE_L1RRCIF_CHANGE
11/09/02   vk      Added support to handle Inter-Freq Events 2d and 2f
11/07/02   scm     Add STOP cause to support RAT change vs. deep sleep STOPs.
11/07/02   djm     Added support for additional measurements in measurement report msg.
11/05/02   cpe     Added more fields to l1_intra_meas_rpt_cell_info_struct_type
                   to support reporting multiple measurement quantities in one 
                   Report
10/30/02   ddh     Added changes for InterRAT cell Reselection
10/17/02   djm     change type of gsm rssi uint16 to int16 value can be/is negative
10/02/02   djm     change type of fn_offset, qbit_offset to reflect srchcm driver types
09/18/02   scm     Working on activation time support in SUSPEND.
09/11/01   vk      While sending measurement req to L1 on state transition, only
                   one measurement type request would be sent at a time. Hence removed
                   l1_meas_state_transition_struct_type structure
09/09/02   vk      Fixed previous checkin due to merge problems
09/06/02   vk      Modified the enum values in l1_meas_trans_enum_type. Added
                   a union type in l1_meas_trans_type and structure type in
                   l1_intra_freq_new_cell_list_struct_type. These changes
                   have been added to reflect the latest spec change in handling
                   of intra-freq measurements on DCH-->FACH transitions
09/05/02   gs      Added ciphering req parameter to l1_ciphering_key_cmd_type
08/29/02   gs      Added enum type L1_SETUP_OPS_DPCH_DROP_ADD to enum definition
                   l1_setup_ops_enum_type
08/21/02   scm     Add signal_ul_mac_at_act_time and signal_dl_mac_at_act_time
                   to l1_setup_cmd_type.
08/21/02   djm      pull interrat measurement data into FEATURE_INTERRAT_HANDOVER_WTOG
08/14/02   sh      Added mac_ul_tfcs_type to l1_cell_transition_cmd_type.
08/13/02   scm     Fix compile problems if FEATURE_CM_SUPPORTED is defined
                   and FEATURE_CM_MEASUREMENT is not.
08/07/02   scm     Add signal_mac_at_act_time to l1_setup_cmd_type.
08/06/02   scm     Continued work on SUSPEND/RESUME support.
08/05/02   sh      Modified l1_cell_transition_cmd_type for UL phychan add.
08/02/02   djm     Added l1 meas id invalid define
07/31/02   vk      Added changes to reconfigure measurements on FACH<-->DCH 
                   transition of RRC
07/31/02   sh      Fixed L1_MAX_RL to 6 instead of 8.
07/18/02   vk      Removed reference to additional measurement structure as
                   it is not required. A new interface would be developed later
                   once multiple measurements of the same type are supported
07/17/02   ddh     Corrected doc reference number for l1_cell_sel_resel_info
07/11/02   ddh     Renamed inter_sys sib element, added #define for 
                   L1_REMOVE_ALL
07/18/02   djm     Added RRC_CPHY_COMPRESSED_MODE_IND Message definition
06/24/02   djm     Re-Organized File Contents
06/13/02   djm     Addition of Measurement Report IEs
06/06/02   ddh     Added inter-freq and inter-RAT changes for 
                   CPHY_CELL_PARM_REQ and the CPHY_MEASUREMENT_REQ messages.
06/03/02   scm     Put computed gain factor mod of rev 1.44 back in the code.
                   It disappeared in rev 1.46.
05/21/02   sh      Added L1_ASET_UPD_CELL_RESELECTION_FAILURE to 
                   l1_aset_update_enum_type.
05/18/02   sh      Cell Reselection related changes.
                   Modified l1_cell_transition_cmd_type to be able to setup
                   PCH directly.                   
                   Removed l1_aset_struct_type, l1_aset_update_cmd_type,
                   modified l1_aset_update_enum_type,
05/08/02   yiz     Include macl1rrcif.h and add tfcs_ptr to l1_setup_cmd_type
                   for computed GF.
05/06/02   scm     Change FEATURE_WCDMA_L1_START_STOP to
                   FEATURE_CHEETAH_BASELINE, to match RRC.
05/03/02   scm     Implementing support of START/STOP primitives from RRC.
                   Code wrapped in FEATURE_WCDMA_L1_START_STOP define.
03/21/02   sh      Updated l1_cell_selec_cmd_type and l1_cell_select_cnf_type
                   for single cell/freq and wrapped it in flag
                   FEATURE_L1_CELL_SELECTION.
03/12/02   cpe     Removed rpt_mode field from l1_meas_ctrl_parm_struct_type
03/11/02   cpe     Added more comments to indicate the values being returned
                   in intra_frequency measurement reports
02/13/02   vk      Added num_cell_meas in l1_intra_freq_cell_list_struct_type
                   to indicate the number of cells for which meas has
                   to be done. Also added INVALID_CONFIGURATION error in 
                   structure l1_error_cause_enum_type
02/12/02   sh/gs   Added macro for loopback mode for cphy_setup_req.
02/12/02   sh      Added commands l1_act_time_cancel_cmd and
                   l1_act_time_cancel_cnf. Removed l1_action_cancel_cmd_type.
02/06/02   sh      Added fields for Activation Time to l1_setup_cmd_type and
                   l1_setup_cnf_type.
02/08/02   cpe     Added trans_id to l1_meas_ctrl_parm_struct_type and 
                   l1_meas_ctrl_cnf_type structures
                   Also added a boolean indicating whether cell_measured 
                   results are included to l1_intra_freq_meas_rpt_struct
                   structure 
02/07/02   gs      Added the enum for loop back mode setting in l1_setup_ops
                   Added the macro CPHY_DL_SET_LOOPBACK_MODE_INCL.
01/24/02   vk      Removed redundant parameter rpt_mode from structure
                   l1_meas_ctrl_parm_struct_type
01/17/02   vk      Updated l1_intra_freq_cell_list_struct_type structure
                   to use primary scrambling code instead of intra freq
                   cell id's. Also added support for cells for measurement
                   in this structure. Removed INCORRECT_PARAMETER_VALUE value
                   from l1_error_cause_enum_type as RRC does the validation
                   Removed meas_vald from l1_intra_freq_meas_ctrl_struct_type 
                   structure as RRC keeps track of intra freq measurement 
                   validity. Changed measured value while sending measured
                   results to uint16
12/05/01   cpe     Updated the measurement control and report interface to
                   account for cell reporting status
11/02/01   cpe     Lots of changes for measurement control message support
                   Added new structure l1_meas_ctrl_cnf_type to send success
                   or failure response for CPHY_MEASUREMENT_REQ command
10/19/01   sh      Added l1_deactivate_cmd_type and l1_deactivate_cnf_type.
10/15/01   gs      Added the member status in structure l1_phychan_est_ind
                   for DL DPCH physical channel establishment result.
09/17/01   cpe     Changed the measurement quantity to int16 in immediate meas
                   function
                   Added ASET update flag and macro in CPHY_SETUP_REQ
                   Changed measurement reporting interface  to report one
                   event at a time.
09/14/01   asr     Added l1_drx_cnf_type
09/04/01   sh      Changed l1_drx_cmd_type to only include drx_cycle_len.
08/30/01   wjn     Added "l1_freq_scan_cmd_type" for WCDMA carrier frequency
                   scan request for RRC to L1; and added l1_freq_scan_cnf_type
                   for the frequency scan result report from L1 to RRC.
                   Changed the type of "freq" in "l1_acq_cmd_type" from "uint32"
                   to "uint16" for consistency.
                   Moved the function prototype l1_process_immediate_meas_req()
                   to l1task_v.h. 
08/22/01   cpe     Added the type that is reported for SFN-SFN difference 
                   parameter  in measurement report structure and immediate 
                   measurement structure
08/09/01   cpe     Expanded the structure for immediate_meas_req from RRC.
                   Added the function to handle the request.
07/31/01   cpe     added the type of events, which triggered the measurement 
                   report, to the  report structure. 
                   Modified L1_INTRA_FREQ_MAX_NUM_EVENT from 8 to 6
                   Also corrected a comment that describes time-to-trigger
                   parameter
06/22/01   sh      Modified CPHY_SETUP_REQ sub-primitive mask. Moved
                   cphy_setup_cmd_id_enum_type from l1msetup.h to here.
05/29/01   sh      Fixed macro CPHY_SETUP_REQ_IS_DL_TRCH_EN_DIS_INCL.
05/18/01   cpe     Updated several measurement control structures and added
                    the l1_meas_rpt_struct_type structure. Also updated the
                    section numbers for l1_meas_sib_parm_struct_type based
                    on 2000-12 version of spec.
04/26/01   wjn     Updated l1_cell_parm_cmd_type based the spec of 2000-12 
                   release. Beefed up comments on parameters in the command.
03/29/01   sh      Modified l1_aset_struct_type and
                  l1_cell_transition_cmd_type for cell transition.

03/09/01   sh      Added l1_cell_transition_cmd_type and 
                   l1_cell_transition_cnf_type.
                   Removed the ..._MASK bit defines used for the request mask
                   in the CPHY_SETUP_REQ command.

03/08/01   gs      1. Added the definition of macro CPHY_DL_TRCH_EN_DIS_INCL
                      for CPHY_SETUP_REQ sub-primitive mask.
                   2. Added the definition of macro to check the
                      CPHY_DL_TRCH_EN_DIS_INCL bit in the request mask.
                   3. Added the definition of macro CPHY_DL_TRCH_EN_DIS_MASK
                      for the mask of bit field CPHY_DL_TRCH_EN_DIS_INCL.
                   4. Changed the l1_cphy_setup_cnf_ops_type to
                      l1_setup_ops_type to have wider scope. Changed the enum
                      names to L1_SETUP_OPS_xxx from CPHY_SETUP_OPS_xxx.
                      Added the enum L1_SETUP_OPS_EN_DIS_TRCH.

02/21/01   wjn     Added "event-triggered and periodical measurement reporting
                   criteria" to "Measurement Command".
                   Made additional structural hierachies for intra-frequency
                   measurement control parameters for better readability and 
                   maintainability.
02/14/01   wjn     Modified definition of l1_aset_update_cmd_type for unified
                   aset update mechanism cross all states.
01/25/01   sh      Modified l1_error_ind_type to include error types for
                   L1 Hardware and Firmware failures.
01/23/01   wjn     Loaded l1_cell_reselec_ind_type with meaningful data fields.
01/16/01   wjn     Updated cell_info structure type to include different 
                   offsets for Ec/Io and RSCP measurement. Added mapped 
                   quality value, q_map, in l1_cell_select_cnf_type.
01/07/01   wjn     Completely redefined l1_meas_cmd_type for clarity and 
                   maintainability.
12/28/00   wjn     Updated l1_cell_parm_cmd_type, l1_cell_selec_cmd_type,
                   l1_cell_select_cnf_type according to Spec rel 2000-09.
12/19/00   sh      Ported RRC related declarations from l1rrcctrl.h.
12/01/2000 mdl     added support for new CPHY_SETUP_CNF packet, added
                   prototype for l1_cphy_setup_cnf_get_status(), finalized
                   new ASET_UPDATE packet structure for Cell Selection and
                   Camping, removed definition of L1 queue signal, removed
                   some extraneous header files.
11/26/00   wjn     Defined/Modified l1_cell_select_cnf_type, 
                   l1_cell_selec_cmd_type, and l1_cell_parm_cmd_type.
11/22/2000 mdl     First step of changing CPHY_ASET_REQ struture, added
                   aset update type enums, added struture for idle mode,
                   i.e. cell selection and reselection, aset updates.
                   Everything wrapped up in FEATURE_NEW_ASET_UPDATE_REQ
                   flag.
11/09/00   wjn     Added Neighbor cell timing accuracy field in 
                   l1_meas_cmd_type.
11/02/00   yus     Added l1_report_type.
09/06/00   yus     modifications to fit the name convensions.
09/01/00   mdl     modifications to the cphy_setup_req command packet             
08/09/00   yus     Created file.
                                     
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "sys_plmn_selection.h"

#include "rrc_rr_types.h"
#include "nv_items.h"

#ifdef FEATURE_DUAL_SIM
#include "trm.h"

/* RRC starts tuneaway this much in advance.  This overhead time is read
** from NV when UE first starts up. */
extern uint16 RRC_UNLOCK_BY_OVERHEAD_TIMER_IN_MS;
extern uint16 RRC_UNLOCK_BY_OVERHEAD_TIMER_IN_TIMETICKS;

#ifdef FEATURE_QTA
/* RRC starts QTA around 22ms + wl1_qta_wfw_start_to_qta_gap_usec(6ms)  
** to allow for WCDMA to freeze CM, disable EQ. 22 ms should be sufficient
** for freezing L1 activities. Also, as RRC files are zipped by qzip, we may 
** face some delay in processing the commands and releasing the chain for QTA 
** which may lead to missed page on other sub. So assuming a maximum delay of 
** 10 ms, overhead time will be 22+6+10 = 38 ms.*/
#define RRC_UNLOCK_QTA_OVERHEAD_TIMER_IN_MS           38
#define RRC_MIN_GAP_ACROSS_TUNEAWAYS_IN_MS            4
#define MAX_DELAY_AFTER_UNLOCK_TIMER_EXPIRY           10
#endif

#endif  /* #ifdef FEATURE_DUAL_SIM */

#include "wl1api.h"
#ifdef FEATURE_WCDMA_DC_HSUPA
#define MAX_NUM_CARR 2
#else
#define MAX_NUM_CARR 1
#endif

/* Public portion of the API exists in wcdmamvsif.h */
#include "wcdmamvsif.h"
#include "macl1rrcif.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*==========================================================================*/
/*=                                                                        =*/
/*=      CONSTANT DECLARATIONS                                             =*/
/*=                                                                        =*/
/*==========================================================================*/


/***************************************************************************/
/**                                                                       **/
/**     L1 - RRC   Non-Specification Related Information                  **/
/**                                                                       **/
/***************************************************************************/

/*-------------------------------------------------------------------------*/
/* L1 - RRC   Interface Constant Definitions                               */
/*-------------------------------------------------------------------------*/

/*This represent the num of secondary carries excluding primary carrier  */
#define MAX_NUM_OF_SEC_CARRIERS 3

#define L1_RRC_CPHY_SETUP_TIMEOUT                    2560
#define L1_RRC_CPHY_SETUP_TIMEOUT_REMOVE_OVERHEAD    2060
#define L1_RRC_CPHY_IDLE_REQUEST_TIMEOUT             2000
#define L1_RRC_CPHY_ACQ_TIMEOUT                      2000
#define L1_RRC_CPHY_WCDMA_START_REQ_TIMEOUT          2000
#define L1_RRC_CPHY_WCDMA_STOP_REQ_TIMEOUT           2000
#define L1_RRC_CELL_TRANSITION_REQ_TIMEOUT           2000
#define L1_RRC_DSDS_CPHY_SETUP_LOCK_WAIT_TIMER       5000


/*--------------------------------------------------------------------------
                  STRUCT: DL_COMMON_INFO_STRUCT_TYPE

This structure contains downlink information common for all radio links
but really seems to be related only to the DPCH.  At any rate it
corresponds to IE 10.3.6.20.
--------------------------------------------------------------------------*/
#define DL_COMMON_INFO_COMPRESSED_MODE_INFO_PRESENT 0x0001
#define DL_COMMON_INFO_TX_DIV_MODE_PRESENT          0x0002
#define DL_COMMON_INFO_SSDT_INFO_PRESENT            0x0004
#define DL_COMMON_INFO_DEFAULT_DPCH_OFFSET_PRESENT  0x0008

#define MAX_DEFAULT_DPCH_OFFSET 306688
/* According to Rel-8 RRC spec, default FDPCH value can be (306688+256 =306944)*/
#define MAX_DEFAULT_EFDPCH_OFFSET 306944


/*--------------------------------------------------------------------------
                  STRUCTURE: L1_DL_DPCH_COMMON_INFO_STRUCT_TYPE

This structure contains the parameters relating the the Downlink DPCH info
common for all Radio Links and corresponds to IE 10.3.6.14.

--------------------------------------------------------------------------*/
#define DL_DPCH_COMMON_INFO_TARG_FRAME_OFFSET_PRESENT 0x01
#define DL_DPCH_COMMON_INFO_DPCH_PWR_CTRL_INFO_PRESENT 0x02

#define MAX_CFN_TARGET_SFN_FRAME_OFFSET 255


#define L1_MIN_TRCH_ID     1
#define L1_MAX_TRCH_ID     32
#define L1_MIN_RATE_MATCH_ATTRIB 1
#define L1_MAX_RATE_MATCH_ATTRIB 256

/* Max simultaneous transport channels */
#define L1_MAX_TRCH 8
/* Max simultanious transport format */
#define L1_MAX_TF   64
/* Max simultanious transport format combinations */
#define L1_MAX_TFC  128
/* Maximum TF possible per TrCh (From RRC spec 25.331 2000-12 V3.5.0 Page 442,443) */
#define L1_MAX_TF_PER_TRCH 32
/* maximum number of TB in one TF. This is set to limit of maximum number of
   TB that can be received from all TrCh in CCTrCh at end of frame */
#define L1_DEC_MAX_TB_PER_TF 32

/* Maximum number of cells that can be in the active set */
#define L1_ASET_MAX   6

/* Maximum number of cells that can be in the dual carrier active set */
#define L1_DC_A_SET_MAX   2

/* Maximum number of cells that can currently be triaged from neighbour set */

#define L1_NSET_MAX   2 /* Actually we need only one for now but keep two */

/* MACROs needed for L1_FREQ_SCAN_CNF declaration */
#define L1_MAX_RAW_SCAN_FREQ   32

  /* Maximum of the center frequencies scanned through a RAW scan. */
  /* The value "32" is defined for 30MHz frequency band and 2MHz RAW
     scan incremental step. "31" should be enough, but "32" is more
     conservative.
   */  

#define L1_MAX_FINE_SCAN_FREQ  21

#define L1_MAX_SCAN_FREQ  MAX(L1_MAX_RAW_SCAN_FREQ, L1_MAX_FINE_SCAN_FREQ)
  /* Maximum of the center frequencies scanned through any type of scan. */


#define L1_MAX_NUM_KEYS        10
  /* djm - should this be maxCNdomains ? */

#define L1_REMOVE_ALL_CELLS    0xFF
  /* Cell Info List Structure for Remove All Command Value for use in */
  /* num_cell_rmv parameter                                           */

#define L1_TTT_MAX             5000
#define L1_TTT_NOT_SET         (L1_TTT_MAX+5)
  /* 10.3.7.64 - Time to Trigger */

#define L1_MAX_TRANSPORT_CHANNELS  32

/* this is used by RRC to indicate L1 that no confirmation is needed for the MCM sent */
#define L1_NO_CNF_TRANSACTION_ID  100


/* for NMR upto 6 cells need to be sent */
#define L1_MAX_CELLS_FOR_NMR 6

/* Default priority set by L1 when priority is not provided from SIB 19 */
#define L1_LOWEST_PRIORITY 0XFF
/* Max number of inter frequency layers that can be sent in by 3rd_MEAS_REQ */
#define L1_MAX_INTERF_PRIORITY_LAYERS 8
/* Max number of GSM layers that can be sent in by 3rd_MEAS_REQ */
#define L1_MAX_GSM_PRIORITY_LAYERS 32

/* For EUTRA frequency up to 8 frequencies  */
#define L1_MAX_EUTRA_FREQ 8
/* Max number of LTE layers that can be sent in by 3rd_MEAS_REQ */
#define L1_MAX_EUTRA_PRIORITY_LAYERS 8
/* Max cells supported by EUTRA frequency */
#define L1_MAX_EUTRA_CELLS_PER_FREQUENCY 16
#define L1_REMOVE_ALL_EUTRA_FREQ    0xFF
/* MAX EUTRA reporting requencies*/
#define L1_MAX_REP_EUTRA_FREQ 4
/* Max cells reported per EUTRA frequency */
#define L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY 4

/***************************************************************************/
/**                                                                       **/
/**     25.331  Specification Related Information                         **/
/**                                                                       **/
/***************************************************************************/

/*-------------------------------------------------------------------------*/
/* 25.331   10.3.10  Multiplicity Values and Type Contraint Values         */
/*-------------------------------------------------------------------------*/

/* --- CN Information ---               */
/*      maxCNdomains - Maximum number of CN domains                        */
#define L1_MAX_CN_DOMAINS          4

/* --- UTRAN Mobility Information ---   */

/* --- UE Information ---               */
/*      maxOtherRAT - Maximum number of other Radio Access Technologies    */
#define L1_MAX_OTHER_RAT           15
/*      maxRAT - Maximum number of Radio Access Technologies               */
#define L1_MAX_RAT                 (L1_MAX_OTHER_RAT + 1)


/* --- RB Information ---                */
/*      maxRB - Maximum number of RBs (Radio Bearers)                      */
#define L1_MAX_RB            32


/* --- PhyCH Information ---             */

/*     maxRL - Maximum number of Radio Links                               */
#undef L1_MAX_RL  /* also defined in l1sapcommon.h (should pull those here) */
#define L1_MAX_RL                  6


/* --- Measurement Information ---       */
/* maxMeasId */

#ifdef FEATURE_WCDMA_REL9
  #define L1_MAX_MEAS_ID             32
#else
  #define L1_MAX_MEAS_ID             16
#endif

#define L1_MEAS_ID_INVALID         (0)
#define L1_MEAS_ID_VALID(measid)    ( ( ((uint16) (measid)) > 0 ) \
                                      && ((uint16) (measid)) <= L1_MAX_MEAS_ID )
/*     maxTGPS      - Maximum number of transmission gap pattern sequences */
#define L1_MAX_TGPS                6
#define L1_TGPSI_VALID(tgpsi)      ( (((uint16) (tgpsi)) > 0 )  \
                                     &&  (((uint16) (tgpsi)) <= L1_MAX_TGPS) )

/*     maxAdditionalMeas - Maximum number of additional measurements for   *
 *                         a given measurement id                          */
#define L1_MAX_ADDITIONAL_MEAS     4
/*     maxMeasEvent - Maximum number of events that can be listed in       *
 *                    measurement reporting criteria                       */
#define L1_MAX_MEAS_EVENT          8
/*     maxMeasParEvent - Maximum number of measurement parameters          *
 *                       (eg. thresholds) per event                        */
#define L1_MAX_MEAS_PAR_EVENT      2
/*     maxMeasIntervals - Maximum number of intervals that define the      *
 *                        mapping function between the measurements for    *
 *                        the cell quality Q of a cell and representing    *
 *                        quality value                                    */
#define L1_MAX_MEAS_INTERVALS      1
/*     maxCellMeas  - Maximum Number of Cells Measured                     */
#define L1_MAX_CELL_MEAS           32
/*     maxReportedGSMCells - Maximum number of GSM cells to be reported    */
#define L1_MAX_REPORTED_GSM_CELLS  6
#define L1_MAX_REPORTED_LTE_CELLS  4
#define L1_MAX_EUTRA_MEAS_EVENT    8

/*     maxFreq - Maximum number of frequencies to measure                  */
#define L1_MAX_FREQ                3

/*     maxNonUsedFreq - Maximum number of non-used frequencies to measure   */
#define L1_MAX_NON_USED_FREQ       (L1_MAX_FREQ - 1)

/* --- Frequency Information ---         */


/* --- Other Information ---             */
/*     maxNumGSMFreqRanges - Maximum number of GSM Frequency Ranges to store */
#define L1_MAX_NUM_GSM_FREQ_RANGES 32
/*     maxNumFDDFreqs - Maximum number of FDD centre frequencies to store  */
#define L1_MAX_NUM_FDD_FREQS       8

#define PRI_FREQ     0
#define SEC_FREQ     1



/*-------------------------------------------------------------------------*/
/* 25.331   Constants declared directly in IEs                             */
/*-------------------------------------------------------------------------*/

/* first use, 10.3.7.26 -   BCCH ARFCN  (0..1023)     */
#define L1_MAX_BCCH_ARFCN     1023


/* noted in 8.6.7.2 - Required Filter Coefficient Storage */
/* note: specification says "UE shall support 2 filter coefficients per measurement type" */
/*       I am assuming "measurement type" is Intra-Freq,Inter-Freq,Inter-Rat, etc. */
/*       I am assuming "support 2" implies 2 that are non-zero, hence with zero support is 2+1 */
#define L1_MAX_FILTER_COEF_STORAGE     (2+1)

/*-------------------------------------------------------------------------*/
/* Constants for which I do not know where they come from...               */
/*-------------------------------------------------------------------------*/

/*--DJM--TO BE REMOVED... use L1_MAX_MEAS_EVENT */
#define L1_INTRA_FREQ_MAX_NUM_EVENT  L1_MAX_MEAS_EVENT
/*--DJM--TO BE REMOVED (END) */


/*--DJM--TO BE REMOVED... use L1_MAX_CELL_MEAS */
#define L1_MAX_NUM_MEAS     L1_MAX_CELL_MEAS
/*--DJM--TO BE REMOVED (END) */

/*--DJM--Where does this come from? */
/* Second, maximum number of measurement mapping intervals */
#define L1_MAX_MAP_INTERVAL 3
/*--DJM-- (End) */


/*--DJM--Where does this come from? */
#define L1_MAX_RPT_CELLS 12
/*--DJM-- (End) */


/*--DJM--Where does this come from? */
/* 25.331 constant - djm where does this come from... should it be maxFreq = 8? */
#define L1_MAX_NUM_FREQ    3
/*--DJM-- (End) */

#define L1_MAX_INTERRAT_EVENTS 4
/* used as array index (0,1,2,3) for events 3A, B, C, D to see if the rssi value
   has been used atleast once and then start aging */

#define MBMS_MAX_PL_PER_REQ 4

/* Macro for invalid PSC */
#define NO_PSC 0x0FFF
#define l1_MAX_MONITORED_CELLS_PER_FREQ 8
#define L1_MAX_MONITORED_GSM_CELLS 8

/* Maximum number of logical channels that can be mapped
 * onto the same priority queue. */
#define L1_MAX_LOGCHAN_PER_QUEUE 15

#define L1_MAX_SCAN_REQ 50

#ifdef FEATURE_WCDMA_DC_HSUPA
#define SRCHMEAS_MAX_NUM_CARR    2
#else
#define SRCHMEAS_MAX_NUM_CARR    1
#endif

/*==========================================================================*/
/*=                                                                        =*/
/*=      Enumerated Type Declarations                                      =*/
/*=                                                                        =*/
/*==========================================================================*/

/***************************************************************************/
/**                                                                       **/
/**     L1 - RRC   Non-Specification Related Enumerated Types             **/
/**                                                                       **/
/***************************************************************************/

/* defines for the ciphering key updates */
#define L1_NUM_BYTES_CIPHER_KEY  16

/* DPCH scrambling code types */
typedef enum {
  L1_UL_SC_SHORT,
  L1_UL_SC_LONG,
  L1_UL_SC_NUM_SC
} l1_ul_dpch_sc_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                         ENUM: DPC_MODE_ENUM_TYPE

This enumerates the types of Downlink Power Control and corresponds to
IE 10.3.6.19.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum {
  L1_DPC_MODE_SINGLE_TPC,
  L1_DPC_MODE_TPC_TRIPLE_IN_SOFT
} l1_dpc_mode_enum_type;


/* HS-DSCH TB-size Table */
typedef enum
{
  L1_DL_HS_DSCH_TB_TABLE_OCTET_UNALIGNED,
  L1_DL_HS_DSCH_TB_TABLE_OCTET_ALIGNED
} l1_dl_hs_dsch_tb_size_alignment_enum_type;

/* defines for the ciphering key updates */
#define L1_NUM_BYTES_CIPHER_KEY  16
#define L1_NUM_WORDS_CIPHER_KEY  4
#define L1_MAX_CIPHER_KEY        4
#define L1_MAX_CIPHER_KEY_ID_VAL 3

#define L1_UL_CIPHER_KEY_REQ 0x01
#define L1_DL_CIPHER_KEY_REQ 0x02


typedef uecomdef_wcdma_cipher_algo_e_type mcalwcdma_dec_ciphering_algo_enum_type;

/* This enum defines the ciphering algorithm type */
typedef mcalwcdma_dec_ciphering_algo_enum_type l1_cipher_alg_enum_type;

/* This structure defines the structure for the ciphering key information */
typedef struct
{
     /* key Id of the ciphering key in dl */
     uint8 key_id;

     /* ciphering algorithm */
     l1_cipher_alg_enum_type algorithm;
     
     /* ciphering key bit stream.
        MSB byte 0 is CK_bit0. LSB last byte is CK_bit127 */
     uint8 key_val[L1_NUM_BYTES_CIPHER_KEY];
} l1_cipher_key_struct_type;

/* This structure defines the structure for all the the ciphering key
   information */
typedef struct
{
     /* number of ciphering keys */
     uint8 num_keys;

     /* ciphering key info */
     l1_cipher_key_struct_type key_info[L1_MAX_CIPHER_KEY];
} l1_cipher_key_info_struct_type;

/* This structure defines the command packet for the CPHY_CIPHERING_KEY_REQ */
typedef struct
{
     /* ciphering request mask
        Bit 0: UL ciphering param
        Bit 1: DL ciphering param */
     uint8 req_mask;

     /* UL ciphering key info */
     l1_cipher_key_info_struct_type ul_cipher_info;

     /* DL ciphering key info */
     l1_cipher_key_info_struct_type dl_cipher_info;
} l1_ciphering_key_req_type;

/* 10.3.6.33 Compressed mode measurement purpose (TGMP) */
typedef enum
{
  L1_CM_TGMP_NONE,
  L1_CM_TGMP_WCDMA_INTERF,
  L1_CM_TGMP_GSM_RSSI_SCAN,
  L1_CM_TGMP_GSM_BSIC_IDENTIFY,
  L1_CM_TGMP_GSM_BSIC_RECONF,
  L1_CM_TGMP_LTE,
  L1_CM_NUM_TGMP_TYPE
} l1_cm_tg_meas_purpose_enum_type;

/* Initial Transmit Power (ITP) control mode in Uplink */
typedef enum
{
  L1_CM_ITP_MODE_0,
  L1_CM_ITP_MODE_1
} l1_cm_itp_mode_enum_type;

/* Recovery Period Power (RPP) control mode during during the
   frame after the transmission gap in Uplink */
typedef enum
{
  L1_CM_NORMAL_PC_MODE,    /* Normal mode power control */
  L1_CM_COMPRESSED_PC_MODE /* compressed mode power control */
} l1_cm_rpp_ctrl_mode_enum_type;

/* Uplink compressed mode method */
typedef enum
{
  L1_CM_NA,                   /* Compressed mode not used */
  L1_CM_BY_PUNCT,             /* Compressed mode by puncturing */
  L1_CM_BY_SF_RED,            /* spreading factor reduction by 2 */
  L1_CM_BY_HIGHER_LAYER_SCHD  /* Higher layer scheduling */
} l1_cm_method_enum_type;

/* Down link frame type in compressed frame */
typedef enum
{
  L1_CM_DL_FR_TYPE_A, /* Frame type A */
  L1_CM_DL_FR_TYPE_B  /* Frame type B */
} l1_cm_dl_frame_type_enum_type;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
/* Enum for FE-FACH feature*/
typedef enum
{
  FE_FACH_NOT_SUPPORTED,
  FE_FACH_HIGH_PRIORITY_LAYERS, /* High Priority Layers */
  FE_FACH_ALL_LAYERS  /* All Layers*/
} rrc_fach_meas_indicator_enum_type;
#endif

typedef  l1_cm_method_enum_type l1_cm_dl_method_enum_type;
typedef  l1_cm_method_enum_type l1_cm_ul_method_enum_type;

/* This structure defines the power control parameter */
typedef struct
{
  /* recovery period power control mode */
  l1_cm_rpp_ctrl_mode_enum_type rpp;

  /* initial UL transmit power control method */
  l1_cm_itp_mode_enum_type itp;

  /* The following variable ranges from 1..30
     Actual values are x 0.1 (0..3 in steps of 0.1 */
  /* Power control changes at start and after the start of TG */
  uint8 delta_sir_1;
  uint8 delta_sir_after_1;
  uint8 delta_sir_2;
  uint8 delta_sir_after_2;
} l1_cm_pwr_ctrl_info_struct_type;

/* This structure defines the compressed mode state control parameters */
typedef struct
{
  /* Transmission gap repetition count. Valid range 1..511
     If 0 then it is infinite */
  uint16 tgp_rep_count;

  /* starting slot number (0..14) for transmission gap */
  uint8 tg_start_slot_num;

  /* transmission gap length in slots (1..14)
     This may not be specified. RRC must set TGL2 = TGL1 */
  uint8 tg_len_1;
  uint8 tg_len_2;

  /* transmission gap distance (0, 15..269) */
  uint16 tgd;

  /* transmission gap pattern length in frames (1..144)
     This may not be specified. RRC must set TGPL2 = TGPL1 */
  uint8 tgp_len_1;
  uint8 tgp_len_2;
} l1_cm_tgp_state_ctrl_info_struct_type;

/* Pls note that valid TGPSI is from 1 to 6 only
   But for the ease of indexing the TGPSI info in to array
   the index 0 to 6 are maintained hence TGPSI SEQ count 7.
   The TGPSI is validated in validation function */
#define L1_CM_MAX_TGP_SEQ_COUNT 7

/* This structure defines the transmission gap pattern sequence
   parameter for a sequence */
typedef struct
{
  uint8 tgp_seq_id; /* TGPSI (1..6) */
  boolean active;   /* TG activate/de-activate */
  uint8 start_cfn;  /* TG starting CFN */

  /* For use by L1CMMEAS module.
     If activate action CFN is valid then TGPS is activated at
     start_cfn on or after activate_action_cfn. */
  boolean activate_action_cfn_valid;
  uint8 activate_action_cfn;
  /* Initialized by Demod module
     These are number of TGPRC that are advanced for start CFN
     for a pattern going active in case original TGCFN was missed.
     Usually in normal scenario this will be set to 0 */
  uint16 init_tgprc_advance_num_fr;

  /* states if the following info is valid.
     If this is set to FALSE it means that compressed info has
     already been received and this is only meant for enabling or
     disabling the compressed mode for the TGPSI for this info */
  boolean info_valid;

  /* The following members are valid only if 'info_valid'
     is set to TRUE.
     ---------------------------------------------------- */

  /* transmission gap measurement purpose */
  l1_cm_tg_meas_purpose_enum_type tg_meas_purpose;
  /* previoys TGMP *** FOR INTERNAL USE ONLY *** */
  l1_cm_tg_meas_purpose_enum_type prev_meas_purpose;
  /* N identify abort used for TGMP BSIC Identification.
     Specifies the number of patterns used by UE to decode
     the unknown BSIC */
  uint8 n_identify_abort;
  /* T reconfirm abort. Used for TGMP BSIC reconfirmation.
     Range is 1 to 20 actual values 0.5 to 10 in 0.5 units */
  uint8 t_reconf_abort;

  /* UL/DL compressed mode method */
  l1_cm_ul_method_enum_type ul_cm_method;
  l1_cm_dl_method_enum_type dl_cm_method;

  /* down link frame type A or B */
  l1_cm_dl_frame_type_enum_type dl_frame_type;

  /* compressed mode state control info */
  l1_cm_tgp_state_ctrl_info_struct_type tgp_state_ctrl_info;

  /* compesseed mode TG power control info */
  l1_cm_pwr_ctrl_info_struct_type pwr_ctrl_info;
} l1_cm_tgp_seq_info_struct_type;

/* Compressed mode Transmission gap pattern information for all seq. */
typedef struct
{
  uint8 tgp_seq_info_count;
  boolean action_needed[L1_CM_MAX_TGP_SEQ_COUNT * 2];
  l1_cm_tgp_seq_info_struct_type *tgp_seq_info[L1_CM_MAX_TGP_SEQ_COUNT * 2];
} l1_cm_info_struct_type;

/** This structure defines transport format information of TF in a
 * transport channel  */
typedef struct
{
  uint16 tb_size;  /**< Transport block size. */
  uint8  num_tb;   /**< Number of transport block in TBS. */
  boolean allowed; /**< If this TF is allowed */
} mcalwcdma_dec_tf_info_struct_type;

/** This enum defines various loopback moes */
typedef enum
{
  L1_LOOPBACK_NA,     /**< No loopback is required */
  L1_LOOPBACK_MODE_1, /**< loopback mode 1 */
  L1_LOOPBACK_MODE_2  /**< loopback mode 2 */
} mcalwcdma_dec_loop_back_mode_type_enum_type;

/** This enum defines the CRC length for TrCh in a CCTrCh. */
typedef enum
{
  L1_CRC_LENGTH_0,  /**< CRC length 0 bits */
  L1_CRC_LENGTH_8,  /**< CRC length 8 bits */
  L1_CRC_LENGTH_12, /**< CRC length 12 bits */
  L1_CRC_LENGTH_16, /**< CRC length 16 bits */
  L1_CRC_LENGTH_24, /**< CRC length 24 bits */
  L1_NUM_CRC_LENGTH /**< Number CRC length types  */
} mcalwcdma_dec_crc_length_enum_type; 

/** This enum defines the coding type of the TrCh in a CCTrCh. */
typedef enum
{
  L1_DL_CONV_1_BY_2_CODING, /**< Convolutional coding with rate 1/2 */
  L1_DL_CONV_1_BY_3_CODING, /**< Convolutional coding with rate 1/3 */
  L1_DL_TURBO_CODING,       /**< Turbo coding. Rate is always 1/3 */
  L1_DL_UNCODED             /**< Uncoded data */
} mcalwcdma_dec_coding_enum_type;

/** This enum defines the reference type of the TrCh */
typedef enum
{
  L1_DL_TRCH_SBCH_TYPE,    /**< Serving cell BCH transport channel type */
  L1_DL_TRCH_NBCH_TYPE,    /**< Neighbor cell BCH transport channel type */
  L1_DL_TRCH_DCH_TYPE,     /**< Dedicated channel transport channel type */
  L1_DL_TRCH_DSCH_TYPE,    /**< Should never be used, only for MAC/RRC compatibility */
  L1_DL_TRCH_PCH_TYPE,     /**< Paging channel transport channel type */
  L1_DL_TRCH_FACH_TYPE,    /**< Fast access chanel transport channel type */
  L1_DL_TRCH_HS_DSCH_TYPE, /**< HSDPA channel transport channel type */
  L1_DL_TRCH_EHS_DSCH_TYPE, /**< Enhanced HSDPA channel transport channel type */
  L1_DL_NUM_TRCH_REF_TYPE, /**< Total number of transport channels types */
  L1_DL_TRCH_INVALID       /* Needed by rrcllcoc.c */
} mcalwcdma_dec_trch_ref_type_enum_type;

/** This enum defines the rate matching criteria type used in DL data for the
   TrCh defined in a CCTrCh */
typedef enum
{
  L1_DL_RM_FIXED_POS, /**< Fixed position rate matching */
  L1_DL_RM_FLEX_POS,  /**< Flexible position rate matching */
  L1_DL_NO_RM         /**< No rate matching */
} mcalwcdma_dec_rm_method_enum_type;


/** Max transport channels in one CCTrCh */
#define MCALWCDMA_DEC_MAX_TRCH 12
/** Maximum number of transport format per transport channel */
#define MCALWCDMA_DEC_MAX_TF_PER_TRCH 64


/** This structure defines the transport channel information */
typedef struct
{
  /** Transport channel reference type */
  mcalwcdma_dec_trch_ref_type_enum_type ref_type;

  /** Transport channel Id. This is for backward compatibility of structure
   * and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 trch_id;

  /** Rate matching attribute. */
  uint16 rate_match_attrib;

  /** Coding type of transport chanel */
  mcalwcdma_dec_coding_enum_type coding_type;

  /** CRC length (enum or number of bits). */
  mcalwcdma_dec_crc_length_enum_type crc_length;

  /** TTI duration of transport channel */
  mcalwcdma_dec_tti_enum_type tti_value;

  /** DCH BLER Quality Value. If not valid or not available, set it to -127.
   * Valid range -63 to 0. For value x real BLER quality is 10^(x/10).
   * This is for backward compatibility of structure and used by WHL1.
   * NOT USED IN MCAL SOFTWARE */
  int8 bler_quality_val;

  /** Loop back mode for this TrCh. Only Applicable if TrCh ref type is DCH */
  mcalwcdma_dec_loop_back_mode_type_enum_type loopback_mode;

  /* If set to TRUE, this flag indicates that this TrCh is mapped to
     Non-TM bearer.  Else if FALSE it is mapped to TM bearers.
     This flag is only applicable for L1_LOOPBACK_MODE_1 */
  boolean                       non_tm_rb;

  /* FACH TrCh type 
   0x00   R99 FACH TrCh 
   0x01   MCCH FACH TrCh
   0x02   MTCH FACH TrCh
   0x04   MSCH FACH TrCh
   0xFF   Not a FACH transport channel */
  uint8 fach_trch_type;

  /** Number of transport format. */
  uint8 num_tf;

  /** Pointer to the array of transport format. */
  mcalwcdma_dec_tf_info_struct_type *tf_info_array[MCALWCDMA_DEC_MAX_TF_PER_TRCH];
} mcalwcdma_dec_trch_info_struct_type;


/** This structure defines CCTrCh information parameters for each transport
 * channel in it and all transport format of each transport channel */
typedef struct
{
  /** CCTrCh Id for this CCTrCh information. This is for backward compatibility
   * of structure and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 cctrch_id;

  /** Rate matching method (fixed or flexible position). */
  mcalwcdma_dec_rm_method_enum_type rm_method;

  /** TrCh ref type bit field indicating the TrCh ref type present in this CCTrCh */
  uint8 trch_ref_type_bf;

  /** TrCh ref type enable bit field indicating the TrCh ref type data will be
   * passed up to MAC. If the bit for a given TrCh reference type is 0 TBset is
   * dropped after it is received. This is for backward compatibility of
   * structure and used by WHL1. NOT USED IN MCAL SOFTWARE */
  uint8 trch_enable_bf;

  /** Number of transport channel. */
  uint8 num_trch;

  /** Pointer to the TrCh info array. */
  mcalwcdma_dec_trch_info_struct_type *trch_info_array[MCALWCDMA_DEC_MAX_TRCH];
} mcalwcdma_dec_cctrch_info_struct_type;

/* This enum defines the rate matching criteria type used in DL data for the
   TrCh defined in a CCTrCh */
typedef mcalwcdma_dec_rm_method_enum_type  l1_dl_rm_method_enum_type;

/* This enum defines the coding type of the TrCh in a CCTrCh. */
typedef mcalwcdma_dec_coding_enum_type  l1_dl_coding_enum_type;

/* This enum defines the reference type of the TrCh */
typedef mcalwcdma_dec_trch_ref_type_enum_type l1_dl_trch_ref_type_enum_type;

/* This enum defines the CRC length for TrCh in a CCTrCh. The enum values
   are kept same as used in RRC IEs and denotes the number of CRC bits. */
typedef mcalwcdma_dec_crc_length_enum_type  l1_crc_length_enum_type;

/*--------------------------------------------------------------------------
                            ENUM: L1_LOOPBACK_MODE_TYPE_ENUM_TYPE
                            
This enumerates the various test loopback mode types
--------------------------------------------------------------------------*/
typedef mcalwcdma_dec_loop_back_mode_type_enum_type  l1_loop_back_mode_type_enum_type;

/* This structure defines the transport format information member passed from
   RRC to L1. */
typedef mcalwcdma_dec_tf_info_struct_type  l1_dl_tf_info_struct_type;

typedef mcalwcdma_dec_trch_info_struct_type  l1_dl_trch_info_struct_type;

/* This structure defines the CCTrCh information member. */
typedef mcalwcdma_dec_cctrch_info_struct_type  l1_dl_cctrch_info_struct_type;

/*
 * Frequency Scan Step Mode
 */
typedef enum
{
  L1_FREQ_RAW_SCAN,                   /* 2 MHz step for raw scan */
  L1_FREQ_FINE_SCAN,                  /* 200 KHz step for fine scan */
  L1_FREQ_ADDITIONAL_CHANNEL_SCAN     /* PCS, 850, 800 MHz additional channel scan */
  ,L1_FREQ_LIST_SCAN  /* When UE is in deep sleep RRC will peform list agc scan
                                       instead of ACQ DB scan*/
} l1_freq_scan_step_enum_type;

/*
 * The acquisition mode is used as a parameter of 
 * function l1_srch_acq_init().
 */
typedef enum
{
  L1_WCDMA_MICRO,         /* Micro Acquisition Mode */
  L1_WCDMA_FULL,          /* Full Acquisition Mode */
  L1_WCDMA_REACQ,         /* Re-acquisition Mode */
  L1_WCDMA_INTER_F_HO_ACQ, /* Inter freq HO ACQ Mode */
  L1_WCDMA_INTER_S_HO_ACQ,/* Intersystem HO ACQ Mode */
  L1_WCDMA_INTER_S_RESEL_ACQ,/* Intersystem Reselection ACQ Mode */
  L1_WCDMA_INTER_S_RESUME,/* Intersystem Resume Mode */
  L1_WCDMA_ACQ_DET_LIST,  /* Return list of srambling codes detected */
  L1_WCDMA_INTER_F_REDIRECTION_ACQ,
  L1_WCDMA_ACQ_MODE_MAX
} l1_wcdma_acq_mode_type;

/*
 * The acquisition mechanism is used as a parameter of 
 * function l1_acq_cmd_type().
 */
typedef enum
{
  L1_WCDMA_FREQ_ONLY,     /* Use only frequency for acquisition          */
  L1_WCDMA_FREQ_SCR,      /* Use frequency and scrambling code           */
  L1_WCDMA_FREQ_SCR_POS   /* Use frequency, scrambling code, pn position */
} l1_wcdma_acq_mech_type;
/*
 * Identifies whether SIB, or Measurement Control Parameters are
 * Contained within the measurement command
 */
typedef enum
{
  L1_SIB_PARMS,
    /* Measurement parameters from SIB_11/12 */
  L1_MEAS_CTRL_PARMS,
    /* Measurement parameters from Measurement Control Message */
  L1_RRC_STATE_CHANGE_PARMS,
    /* Measurement parameters from RRC on state transition from FACH<-->DCH */
  L1_INTER_FREQ_SIB_PARMS
    /* Measurement parameters from RRC for inter freq SIB */
 /* With Deferred reading feature RRC will defer measurement req to L1*/
   ,L1_DEFERRED_MEASUREMENT_FROM_SIB
    /* Measurement parameters and priority information from SIB_19 */
  ,L1_PRIORITY_EUTRA_CELL_LIST_INFO
  ,L1_CSG_MEAS_INFO_PARAMS

} l1_meas_ctrl_enum_type;

/*
 * Error Indication for use in RRC_CPHY_ERROR_IND message
 */
typedef enum
{
  L1_INVALID_CMD_IND,        /* Invalid command in current state */
  L1_PARM_ERR_IND,           /* Invalid parameters in command    */
  L1_UNABLE_PROC_CMD_IND,    /* Unable to process command        */
  L1_FREQ_ERR_IND,           /* Invalid frequency selection      */
  L1_HARDWARE_FAILURE_IND,   /* Hardware failure in L1           */
  L1_FIRMWARE_FAILURE_IND,   /* Firmware failure in L1           */

  L1_LAST_ERR_IND            /* Last error indication            */
} l1_error_enum_type;

/*
 * Cell Reselection Indication Status
 */
typedef enum
{
  L1_RESEL_FAILURE,
  L1_RESEL_SUCCESS
} l1_cell_resel_status_enum_type;

/*
 * Cell Reselection Indication Cell System Selected
 */
typedef enum
{  
  L1_RESEL_WCDMA,  
  L1_RESEL_GSM,      
  L1_RESEL_IS2000
  ,L1_RESEL_LTE
} l1_cell_resel_cell_enum_type;

/* Carrier index for MC-HSDPA */
typedef enum
{
  L1_HSDPA_CARRIER_0,
  L1_HSDPA_CARRIER_1,
  L1_HSDPA_CARRIER_2,
 #ifdef FEATURE_WCDMA_3C_HSDPA
  L1_HSDPA_MAX_CARRIERS = (L1_HSDPA_CARRIER_2 + 1)
 #else
  L1_HSDPA_MAX_CARRIERS = (L1_HSDPA_CARRIER_1 + 1)
 #endif
} l1_hsdpa_carrier_enum_type;

/*
 * Meas Reconfiguration type defined
 */
typedef enum 
{
  L1_MEAS_STOP_AND_OVERWRITE, /* Stop Rpt & Overwrite on current meas */
  L1_MEAS_STOP_AND_CONFIG_CELL_LIST, /* Stop Rpt & Config the cell info list in measurement identity */
  L1_MEAS_DELETE, /* Delete the measurement */
  L1_MEAS_DELETE_ALL
} l1_meas_trans_enum_type;

/*
 * MM sends RRC_STOP_WCDMA_MODE_REQ to RRC with a cause field.  Then RRC sends
 * CPHY_STOP_WCDMA_MODE_REQ to L1 with cause field below.
 *
 * - RAT Change means we are leaving WCDMA to activate another RAT.
 * - Deep Sleep means we have not found service for some period of time and we now
 *   quit searching and enter a power conservation mode.
 * - TRM (Transceiver Resource Manager) stop means WL1 was either unable to
 *   reserve the primary antenna, or TRM asked WL1 to give up the primary
 *   antenna (and WL1 is giving it up).  These can happen if another RAT has
 *   priority over WCDMA for the antenna at that time.
 */
typedef enum
{
  L1_RAT_CHANGE,
  L1_DEEP_SLEEP,
  L1_TRM_STOP,
  L1_STOP_CAUSE_MAX
} l1_stop_cause_enum_type;

/*
 * When RRC issues the CPHY_START_WCDMA_MODE_REQ command to WL1, it may specify
 * the mDSP firmware image to be loaded in the mDSP.  These are the various
 * mDSP image types available.
 */
typedef enum
{
  WL1_MDSP_IMAGE_WCDMA_ONLY,
  WL1_MDSP_IMAGE_WCDMA_GSM,
  WL1_MDSP_IMAGE_WCDMA_1X,
  WL1_MDSP_IMAGE_MAX
} wl1_mdsp_image_enum_type;

/*--------------------------------------------------------------------------
RRC sends a CPHY_SUSPEND_WCDMA_MODE_REQ to L1 with a specific cause.
--------------------------------------------------------------------------*/
typedef enum
{
  L1_IRAT_REASON_OTHER,
  L1_IRAT_RESELECTION,
  L1_IRAT_REDIRECTION,
  L1_IRAT_OOS,
  L1_IRAT_BPLMN,
  L1_IRAT_IHO,
  L1_IRAT_CCO,
#ifdef FEATURE_DUAL_SIM
  L1_IRAT_DUAL_SIM_TUNEAWAY,
#endif
  L1_IRAT_NONE
}l1_suspend_resume_cause_enum_type;


/*--------------------------------------------------------------------------
                          ENUM: ASET_UPD_ENUM_TYPE
                          
This enum lists the various types of ASET update.

1) Cell Selection - used during BCH state. Note this is not really an ASET 
   update; it is more demod set updated.
2) Cell Reselection - used to demodulate a neighbor BCH for cell reselection 
   during BCH, PCH and FACH state. Note this is not really an ASET update;
   it is more demod set updated. In this mode, the new ASET info in the 
   local command only contains the the neighbor cell and does not contain
   the camp cell. The camp cell will be unchanged.
3) Cell Camp - Used to update actual ASET during BCH, PCH and FACH state. 
4) Macro Diversity - used for soft-handoff during DCH state.
5) Hard Handoff - Used for hard handoff during DCH state.
--------------------------------------------------------------------------*/
typedef enum
{
  L1_ASET_UPD_CELL_RESELECTION,
  L1_ASET_UPD_CELL_CAMP,
  L1_ASET_UPD_CELL_RESELECTION_SUCCESS,
  L1_ASET_UPD_CELL_RESELECTION_FAILURE

} l1_aset_update_enum_type;

/*--------------------------------------------------------------------------
     List of bands for frequency scan
--------------------------------------------------------------------------*/
typedef enum
{
  L1_FREQ_WCDMA_BAND_PCS,
  L1_FREQ_WCDMA_BAND_CELL,
  L1_FREQ_WCDMA_BAND_IMT2K,
  L1_FREQ_WCDMA_BAND_WCDMA_800,
  L1_FREQ_WCDMA_BAND_WCDMA_850,
  L1_FREQ_WCDMA_BAND_WCDMA_900,
  L1_FREQ_WCDMA_BAND_WCDMA_1700,
  L1_FREQ_WCDMA_BAND_WCDMA_1700_2100,
  L1_FREQ_WCDMA_BAND_1800,
  L1_FREQ_WCDMA_BAND_1500,
  L1_FREQ_WCDMA_BAND_EXTENDED_800,
  L1_FREQ_WCDMA_BAND_NONE
} l1_freq_scan_band_enum_type;


/*--------------------------------------------------------------------------
     Type of search requested by RRC 
--------------------------------------------------------------------------*/
typedef enum
{
  L1_SERVICE_SRCH_TYPE,
  L1_MPLMN_LIST_SRCH_TYPE,
  L1_ABPLMN_SRCH_TYPE,
  L1_BPLMN_SRCH_TYPE

} l1_wcdma_srch_enum_type;

/***************************************************************************/
/**                                                                       **/
/**     25.331  Specification Related Enumerated Types                    **/
/**                                                                       **/
/***************************************************************************/

/**** 10.2... ****/

/*
 * derived from 10.2.17  - Measurement Control,  Measurement Type Selector
 */
typedef enum
{
  L1_MEAS_TYPE_INVALID,
  L1_INTRA_FREQ,
  L1_INTER_FREQ,
  L1_INTER_SYS,
  L1_LCS,
  L1_TRAF_VOL,
  L1_QUALITY,
  L1_UE_INTERNAL,
  L1_INTER_SYS_EUTRA,
  L1_MEAS_TYPE_MAX
} l1_meas_type_enum_type;

typedef struct
{
  /*Cell Primary Scrambling Code*/
  uint16 psc;

  /*RSCP*/
  int16 rscp;

  /*EC/Io*/
  int16 ecio;

  /*S_qual */
  int16 squal;

  /*S_rxlev */
  int16 srxlev;
  
  /* Rank of the cell */
  int16 rank;

  /* Set of the cell */
  uint8 set;
} l1_cell_power_info_type;


/* enum type definition for spreading factor */
typedef enum {
  L1_SF_4,
  L1_SF_8,
  L1_SF_16,
  L1_SF_32,
  L1_SF_64,
  L1_SF_128,
  L1_SF_256,
  L1_SF_512,
  L1_NUM_SF
} l1_sf_enum_type;

/*--------------------------------------------------------------------------
                            ENUM: DL_REF_PHYCHAN_ENUM_TYPE
                            
This enumerates the various downlink physical channel reference id's                            
--------------------------------------------------------------------------*/
typedef enum {
  L1_DL_PHYCHAN_PCCPCH_S,
  L1_DL_PHYCHAN_PCCPCH_N,
  L1_DL_PHYCHAN_SCCPCH0,
  L1_DL_PHYCHAN_SCCPCH1,
  L1_DL_PHYCHAN_PICH,
  L1_DL_PHYCHAN_AICH,
  L1_DL_PHYCHAN_HS_RACH_AICH,
  L1_DL_PHYCHAN_DPCH,
  L1_DL_PHYCHAN_HS_RACH_FDPCH,
  L1_DL_PHYCHAN_FDPCH,
  L1_DL_PHYCHAN_SEC_FDPCH,
  L1_DL_PHYCHAN_PDSCH,
  L1_NUM_DL_PHYCHAN,
  L1_DL_PHYCHAN_NOCHAN
} l1_dl_phychan_enum_type;

typedef struct
{
  boolean is_valid;

  /*RxAGC (in dBm)*/
  int16 rx_agc;

  /*TxAGC (in dBm)*/
  int16 tx_agc;

  /* BLER on DL */
  uint16 dl_bler;

  l1_cell_power_info_type cell_info;
}l1_serving_cell_power_info_type;

typedef struct
{

  /*RxAGC (in dBm)*/
  int16 rx_agc;

  /*TxAGC (in dBm)*/
  int16 tx_agc;

  /* BLER on DL */
  uint16 dl_bler;
  
  /*Number of RLs in active set*/
  uint8 num_rls;
  /*Primary Scrambling codes*/
  uint16 psc[L1_ASET_MAX];

  /*RSCP*/
  int16 rscp[L1_ASET_MAX];

  /*EC/Io*/
  int16 ecio[L1_ASET_MAX];

  /* spreading factor of the channel */
  l1_sf_enum_type sf;

  /* Physical channel type FDPCH/DPCH */
  l1_dl_phychan_enum_type phy_chan_type;

  /* Slot format for the channel (DPCH/FDPCH).
     This has to be interpreted based on Physical channel typr */
  uint8 slot_format;
} l1_active_set_info_type;


typedef struct
{
  /*UARFCN*/
  uint16 uarfcn;

  /*Number of cells in the frequency*/
  uint8 num_of_cells;

  /*Cells info*/
  l1_cell_power_info_type cell_info[l1_MAX_MONITORED_CELLS_PER_FREQ];
} l1_monitored_freq_info_type;

typedef struct
{
  /*EARFCN*/
  uint32 earfcn;      
  /* Is this a LTE TDD cell*/
  boolean is_tdd_freq;
  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* Max RSRP comb.value combined across Tx-Rx pairs. In dB  scale. */
  int16 rsrp;
  /* Max RSRQ value combined across Tx-Rx pairs. In dB scale*/
  int16 rsrq;
  /* LTE cell rxlev */
  int16  s_rxlev;
} l1_lte_interrat_measurement_type;

typedef struct
{
  /* number of cells */
  uint8 number_of_cells;

  /* Inter-RAT measurement data */
  l1_lte_interrat_measurement_type interrat_measurements[L1_MAX_EUTRA_FREQ*L1_MAX_EUTRA_CELLS_PER_FREQUENCY];
} l1_lte_interrat_measurement_data_type;

typedef struct
{
  /*Number of frequencies*/
  uint8 num_of_freqs;

  /*Freq info*/
  l1_monitored_freq_info_type freq[L1_MAX_FREQ];
  rrc_rr_interrat_measurement_data_type gsm_cells_info;
  l1_lte_interrat_measurement_data_type lte_cells_info;
}l1_monitored_set_info_type;
extern boolean srch_modem_stat_call;

/* RRC or MAC to L1 Interface Primitives */
/* ************************************** *
 * This list is used by logging!!!        *
 * Send updates to cfa.team               *
 * ************************************** */
typedef enum
{
  /* Commands from RRC */
  CPHY_CAPABILITY_REQ           =   0x0,
  CPHY_IDLE_REQ                 =   0x1,
  CPHY_DEACTIVATE_REQ           =   0x2,
  CPHY_STOP_WCDMA_MODE_REQ      =   0x3,
  CPHY_START_WCDMA_MODE_REQ     =   0x4,
  CPHY_SUSPEND_WCDMA_MODE_REQ   =   0x5,
  CPHY_RESUME_WCDMA_MODE_REQ    =   0x6,
  CPHY_FREQ_SCAN_REQ            =   0x7,
  CPHY_ACQ_REQ                  =   0x8,
  CPHY_CELL_PARM_REQ            =   0x9,
  CPHY_ASET_UPDATE_REQ          =   0xA,
  CPHY_HARD_HANDOFF_REQ         =   0xB,
  CPHY_BCH_SCHEDULE_REQ         =   0xC,
  CPHY_SETUP_REQ                =   0xD,
  CPHY_DRX_REQ                  =   0xE,
  CPHY_CIPHERING_KEY_REQ        =   0xF,
  CPHY_MEASUREMENT_REQ          =   0x10,
  CPHY_CELL_SELECTION_REQ       =   0x11,
  CPHY_IMMEDIATE_MEAS_REQ       =   0x12,
  CPHY_INTEGRITY_KEY_REQ        =   0x13,
  CPHY_INTEGRITY_KEY_MAUC_REQ   =   0x14,
  CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
  CPHY_CELL_RESELECTION_RSP     =   0x16,
  CPHY_CELL_TRANSITION_REQ      =   0x17,
  CPHY_ACT_TIME_CANCEL_REQ      =   0x18,

  /* Commands from MAC */
  CPHY_UL_TX_CONTROL_REQ        =   0x19,
  CPHY_MAC_UL_CONFIG_CNF        =   0x1A,
  CPHY_MAC_DL_CONFIG_CNF        =   0x1B,
  CPHY_MAC_DL_CONFIG_AT_L1_FAIL_CNF =  0x1C,

  /* Commands from LSM (Location Services Manager) */
  CPHY_LSM_TIMETAG_REQ          =   0x1D,
  CPHY_LSM_CELL_MEAS_REQ        =   0x1E,

  /* Commands from BMC (Broadcast Multicast Control) */
  CPHY_BMC_SCHEDULING_REQ       =   0x1F,
  CPHY_BMC_SKIP_REQ             =   0x20,
  CPHY_BMC_DEACTIVATE_REQ       =   0x21,
  CPHY_CELL_BAR_REQ             =   0x22,
  CPHY_UPD_DL_SYNC_PARMS        =   0x23,

  /* Commands for SIBS */
  CPHY_SIB_SCHED_CHANGE_IND     =   0x24,

  CPHY_GO_TO_SLEEP_IND          =   0x25,

  /* MBMS related command and indication */
  /* ----------------------------------- */

  /* MCCH DRX request */
  CPHY_MBMS_MCCH_DRX_REQ         =  0x26,
  /* MBMS IDLE request */
  CPHY_MBMS_IDLE_REQ             =  0x27,
  /* MBMS Preferred layer (FLC) request */
  CPHY_MBMS_PL_PARM_REQ          =  0x28,

  CPHY_UPD_OTA_PARMS_IND         = 0x29,
  /* Commands from QChat */
  CPHY_QCHAT_START_IMM_ORIG      =  0x2A,

  /*Nudging RRC means telling RRC to bring up PCCPCH_S, so we can reestablish
   *timing of the network
   */
  CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP = 0x2B,

  /*RRC request L1 to provide info for modem stat*/
  CPHY_GET_MODEM_STATISTICS_REQ   =   0x2C,

  CPHY_RSSI_REQ                   =   0x2D,

  CPHY_DATA_FOR_UI_DISP_REQ       =   0x2E,

  CPHY_CHECK_SLEEP_RSP            =   0x2F,

  CPHY_CHECK_S_CRIT_REQ           =   0x30,

  CPHY_NMR_INFO_REQ               =   0x31,
  
  CPHY_ADDITIONAL_MEAS_DATA_REQ   =   0x32,

  CPHY_GET_IMM_MEAS_REQ           =   0x33,

#ifdef FEATURE_DUAL_SIM
 /*Dual SIM Commands*/
  CPHY_DS_STATUS_CHANGE_IND       =   0x34,
  
  CPHY_RECOVER_WCDMA_REQ          =   0x35,

#ifdef FEATURE_QTA
  CPHY_START_QTA_REQ              =   0x36,
  
  CPHY_STOP_QTA_REQ               =   0x37,
#endif

  CPHY_DRX_TIMER_EXPIRY_IND       =   0x38,
#endif
/*cmd is added so as to start BPLMN in the immediate 
  sleep cycleit will take WL1 to PCH and then immediately 
  to PCH sleep so that WL1 will call Can l1 function for 
  RRC to start BPMLN
*/
  CPHY_BPLMN_SLEEP_ABORT_IND      =   0x39,

  CPHY_M2M_ENABLE_IND             =   0x3A,

  /* Number of External Commands. Should be the last element.*/
  CPHY_NUM_EXT_REQ_CMDS           =   0x3B
} l1_cmd_enum_type;


/**** 10.3.2... ****/

/*
 * derived from 10.3.2.5  - Mapping Information,   Function Type
 */
typedef enum
{
  L1_MAP_FUN_LINEAR,
    /* Supported initially */
  L1_MAP_FUN_TYPE_2,
    /* Not supported */
  L1_MAP_FUN_TYPE_3,
    /* Not supported */
  L1_MAP_FUN_TYPE_4,
    /* Not supported */
  L1_MAP_FUN_TYPE_MAX
} l1_map_function_type_enum_type;


/*
 * derived from 10.3.2.5  - Mapping Information,   RAT
 */
typedef enum
{
  L1_UTRA_FDD,
    /* RAT is WCDMA FDD.  Supported. */
  L1_UTRA_TDD,
    /* RAT is WCDMA TDD.  Not supported */
  L1_GSM,
    /* RAT is GSM.        Supported (soon) */
  L1_CDMA2000
    /* RAT is CDMA2000.   Not supported */
} l1_rat_enum_type;


/**** 10.3.3... ****/

/*
 * 10.3.3.1  Activation Time (CFN)
 * 10.3.6.75 SFN Time Info, Activation Time (SFN)
 */
typedef enum
{
  L1_ACTIVATION_TIME_NONE,
  L1_ACTIVATION_TIME_SFN,
  L1_ACTIVATION_TIME_CFN
} l1_act_time_enum_type;


/*
 * 10.3.3.13  - Failure cause
 */
typedef enum 
{
  INVALID_CONFIGURATION,
  INCOMPLETE_CONFIGURATION,
  UNSUPPORTED_MEASUREMENT
} l1_failure_cause_enum_type;
typedef l1_failure_cause_enum_type l1_error_cause_enum_type;



/**** 10.3.6... ****/
/*
 * 10.3.6.33 and 10.3.6.34  - TGPS Status
 */
typedef enum
{
  L1_TGPS_INACTIVE,
  L1_TGPS_ACTIVE
} l1_tgps_status_enum_type;
#define L1_TGPS_STATUS_VALID(status) \
                                ( ((status) == L1_TGPS_ACTIVE) \
                                  || ((status) == L1_TGPS_INACTIVE) )

/*
 * 10.3.6.34  - DPCH CM Status Info, TGPS Re-Configuration Information from 
 */
typedef struct
{
  uint16  tgpsi;
    /* Transmission Gap Pattern Identifier */

  l1_tgps_status_enum_type  status; 
    /* Transmission Gap Pattern Status (active/inactive) */

  uint16  tgcfn;
    /* Transmission Gap Starting CFN(active)/Deactivate CFN if inactive */
} l1_tgps_struct_type;

/*
 * 10.3.6.34  - DPCH Compressed Mode Status Info 
 */
typedef struct
{
  uint16  tgps_reconfig_cfn;

  uint16  num_tgps;
  l1_tgps_struct_type tgps_array[L1_MAX_TGPS];

  /* SDU timestamp of received MCM at RRC used to determine
     missed TG reconfig CFN */
  uint16 sdu_time;
} l1_cm_status_info_struct_type;

/*
 * 10.3.6.36  Frequency Info  Choice
 */
typedef enum
{
  L1_FREQ_MODE_FDD
} l1_freq_mode_enum_type;


/**** 10.3.7... ****/

/*
 * 10.3.2.3  RAT Identifier
 */
typedef enum 
{
  L1_GSM_SYSTEM,    /* GSM system */
  L1_IS2000_SYSTEM, /* IS2000 system (currently not supported) */
  L1_EUTRA_SYSTEM,
  L1_SYSTEM_NONE    /* Neither GSM or IS2000 */
} l1_rat_type_enum_type;


/*
 * 10.3.7.5  SFN-SFN observed time difference reporting indicator
 */
typedef enum
{
  L1_NO_REPORT,
  L1_REPORT_TYPE_1,
  L1_REPORT_TYPE_2
} l1_sfn_report_enum_type;


/*
 * 10.3.7.9  - Filter Coefficient
 */
typedef enum 
{
  L1_GSM_FILTER_COEF_0 = 0,  
  L1_GSM_FILTER_COEF_1 = 1,  
  L1_GSM_FILTER_COEF_2 = 2,  
  L1_GSM_FILTER_COEF_3 = 3,  
  L1_GSM_FILTER_COEF_4 = 4,  
  L1_GSM_FILTER_COEF_5 = 5,  
  L1_GSM_FILTER_COEF_6 = 6,  
  L1_GSM_FILTER_COEF_7 = 7,  
  L1_GSM_FILTER_COEF_8 = 8,  
  L1_GSM_FILTER_COEF_9 = 9,  
  L1_GSM_FILTER_COEF_11 = 11,  
  L1_GSM_FILTER_COEF_13 = 13,  
  L1_GSM_FILTER_COEF_15 = 15,  
  L1_GSM_FILTER_COEF_17 = 17,  
  L1_GSM_FILTER_COEF_19 = 19,
  L1_GSM_FILTER_COEF_MAX,
  L1_GSM_FILTER_COEF_INVALID
} l1_gsm_filter_coef_enum_type;

/*
 * 10.3.7.9  - Filter Coefficient
 */
typedef enum 
{
  L1_WCDMA_FILTER_COEF_0 = 0,  
  L1_WCDMA_FILTER_COEF_1 = 1,  
  L1_WCDMA_FILTER_COEF_2 = 2,  
  L1_WCDMA_FILTER_COEF_3 = 3,  
  L1_WCDMA_FILTER_COEF_4 = 4,  
  L1_WCDMA_FILTER_COEF_5 = 5,  
  L1_WCDMA_FILTER_COEF_6 = 6,  
  L1_WCDMA_FILTER_COEF_7 = 7,  
  L1_WCDMA_FILTER_COEF_8 = 8,  
  L1_WCDMA_FILTER_COEF_9 = 9,  
  L1_WCDMA_FILTER_COEF_11 = 11,  
  L1_WCDMA_FILTER_COEF_13 = 13,  
  L1_WCDMA_FILTER_COEF_15 = 15,  
  L1_WCDMA_FILTER_COEF_17 = 17,  
  L1_WCDMA_FILTER_COEF_19 = 19,
  L1_WCDMA_FILTER_COEF_MAX,
  L1_WCDMA_FILTER_COEF_INVALID
} l1_wcdma_filter_coef_enum_type;

typedef enum 
{
  L1_FILTER_COEF_0 = 0,  
  L1_FILTER_COEF_1 = 1,  
  L1_FILTER_COEF_2 = 2,  
  L1_FILTER_COEF_3 = 3,  
  L1_FILTER_COEF_4 = 4,  
  L1_FILTER_COEF_5 = 5,  
  L1_FILTER_COEF_6 = 6,  
  L1_FILTER_COEF_7 = 7,  
  L1_FILTER_COEF_8 = 8,  
  L1_FILTER_COEF_9 = 9,  
  L1_FILTER_COEF_11 = 11,  
  L1_FILTER_COEF_13 = 13,  
  L1_FILTER_COEF_15 = 15,  
  L1_FILTER_COEF_17 = 17,  
  L1_FILTER_COEF_19 = 19,
  L1_FILTER_COEF_MAX,
  L1_FILTER_COEF_INVALID
} l1_common_filter_coef_enum_type;

/*
 * 10.3.7.14  - Inter-frequency event identity
 */
typedef enum
{
  L1_INTER_FREQ_EVENT_2A,
    /* Change of Best Frequency */
  L1_INTER_FREQ_EVENT_2B,
    /* The estimated quality of the currently used frequency is below a threshold *and* the */
    /* estimated quality of a non-used frequency is above a certain threashold              */
  L1_INTER_FREQ_EVENT_2C,
    /* The estimated quality of a non-used frequency is above a certain threshold */
  L1_INTER_FREQ_EVENT_2D,
    /* The estimated quality of the currently used frequency falls below a certain threshold */
  L1_INTER_FREQ_EVENT_2E,
    /* The quality of a non-used frequency is below a certain threshold */
  L1_INTER_FREQ_EVENT_2F
    /* The estimated quality of the currently used frequency is above a certain threshold */
} l1_inter_freq_event_enum_type;


/*
 * 10.3.7.22  - Set update mode
 */
typedef enum
{
  L1_SET_UPDATE_ON,
  L1_SET_UPDATE_ON_NO_RPT,
  L1_SET_UPDATE_OFF
} l1_set_update_mode_enum_type;


/*
 * 10.3.7.23  - Inter-RAT Cell Info List,  Band Indicator
 */
typedef enum 
{
  L1_DCS_1800_BAND,    /* DCS 1800 band used */
  L1_PCS_1900_BAND     /* PCS 1900 band used */
} l1_gsm_band_indicator_enum_type;


/*
 * 10.3.7.26  - CHOICE BSIC verified or non-verified
 */
typedef enum
{
  L1_BSIC_VERIFIED,
  L1_BSIC_NOT_VERIFIED
} l1_bsic_choice_enum_type;



/*
 * 10.3.7.28  Inter-RAT Event Identity
 */
typedef enum
{
  L1_INTER_RAT_EVENT_3A,
    /* The estimated quality of the currently used UTRAN frequency is below a certain         */
    /* threshold *and* the estimated quality of the other system is above a certain threshold */
  L1_INTER_RAT_EVENT_3B,
    /* The estimated quality of the other system is below a certain threshold */
  L1_INTER_RAT_EVENT_3C,
    /* The estimated quality of the other system is above a certain threshold */
  L1_INTER_RAT_EVENT_3D
    /* Change of the best cell in other system */
} l1_inter_sys_event_enum_type;


/*
 * derived from 10.3.7.29  - Inter-RAT measurement quantity,  Measurement Quantitiy
 */
typedef enum
{
    L1_GSM_CARRIER_RSSI
} l1_gsm_meas_quantity_enum_type;


/*
 * 10.3.7.34  - Intra-frequency event identity 
 */
typedef enum
{
  L1_INTRA_FREQ_EVENT_1A,
    /* A primary CPICH enters the reporting range */
  L1_INTRA_FREQ_EVENT_1B,
    /* A primary CPICH leaves the reporting range */
  L1_INTRA_FREQ_EVENT_1C,
    /* A non-active P-CPICH becomes better than an active P-CPICH */
  L1_INTRA_FREQ_EVENT_1D,
    /* Change of the best cell */
  L1_INTRA_FREQ_EVENT_1E,
    /* A P-CPICH becomes better than an absolute threshold */
  L1_INTRA_FREQ_EVENT_1F
    /* A P-CPICH becomes worse than an absolute threshold */
  ,L1_INTRA_FREQ_EVENT_1G,
  /* not supported; for TDD only */
  L1_INTRA_FREQ_EVENT_1H,
  /* not supported; for TDD only */
  L1_INTRA_FREQ_EVENT_1I,
  /* not supported; for TDD only */
  L1_INTRA_FREQ_EVENT_1J
/* A non EDCH aset cell becomes better than EDCH aset cell */
} l1_intra_freq_event_enum_type;


/*
 * 10.3.7.38  - Measurement quantity
 *   For cell selection and reselection, only Ec/No and RSCP are used.
 */
typedef enum
{
  L1_EC_NO,
  L1_RSCP,
  L1_PATHLOSS,
  L1_MEAS_NONE
} l1_meas_quan_enum_type;


/*
 * 10.3.7.39  - Triggering Condition(s) 1 & 2 
 */
typedef enum
{
  L1_MEAS_EVENT_ASET_TRIG,
    /* Aset cell triggers event */
  L1_MEAS_EVENT_MSET_TRIG,
    /* Monitored (neighbor) set cell triggers event */
  L1_MEAS_EVENT_ASET_MSET_TRIG,
    /* Aset and Monitored (Neighbor) cell triggers event */
  L1_MEAS_EVENT_DSET_TRIG,
  /* Detected Set triggers event */
  L1_MEAS_EVENT_DSET_MSET_TRIG,
  /* Detected and Monitored (Neighbor) cell triggers event */
  L1_MEAS_EVENT_ASET_MSET_DSET_TRIG
  /* defined for 1D only as it allows all cells to trigger 
       trigg condn2 is optional in rel5 and not needed in rel99 */
} l1_meas_event_trigger_enum_type;


/*
 * 10.3.7.43  - Maximum number of reported cells on RACH
 */
typedef enum
{
  L1_RACH_RPT_NONE, 
    /* No Report */
  L1_RACH_RPT_0,
    /* Current cell */
  L1_RACH_RPT_1,
    /* Current cell + best neighbor */
  L1_RACH_RPT_2,
    /* Current cell + 2 best neighbors */
  L1_RACH_RPT_3,
    /* Current cell + 3 best neighbors */
  L1_RACH_RPT_4,
    /* Current cell + 4 best neighbors */
  L1_RACH_RPT_5,
    /* Current cell + 5 best neighbors */
  L1_RACH_RPT_6
    /* Current cell + 6 best neighbors */
} l1_intra_freq_rach_rpt_max_enum_type;


/*
 * 10.3.7.46  - Measurement Command for measurement control message
 */
typedef enum
{
  L1_MEAS_SETUP,
  L1_MEAS_MODIFY,
  L1_MEAS_RELEASE
} l1_meas_cmd_enum_type;


/*
 * derived from 10.3.7.49  - Measurement Reporting Mode,
 *                              Measurement Report Transfer Mode
 */
typedef enum 
{
  ACKNOWLEDGED_MODE,
  UNACKNOWLEDGED_MODE
} l1_meas_report_trans_mode_enum_type;


/*
 * derived from 10.3.7.49  - Measurement Reporting Mode,
 *                              Periodical Reporting / Event Trigger Reporting Mode
 */
typedef enum
{
  L1_PERIODIC_RPT,
  L1_EVENT_TRIGGER_RPT,
  L1_NO_RPT,
  L1_NO_CHANGE,
  L1_INVALID_RPT_MODE,
  L1_RPT_MODE_NOT_PRESENT
} l1_meas_report_mode_enum_type;


/*
 * 10.3.7.49  - Measurement reporting mode
 */
typedef struct
{
  l1_meas_report_mode_enum_type        rpt_mode;
    /* event based, or periodic */
  l1_meas_report_trans_mode_enum_type  rpt_trans_mode;
    /* measurement transfer mode */
} l1_meas_reporting_mode_struct_type;


/*
 * 10.3.7.51  - Measurement Validity
 */
typedef enum
{
  L1_STATE_DCH,
  L1_ALL_STATES_BUT_DCH,
  L1_ALL_STATES
} l1_meas_validity_enum_type;


/*
 * 10.3.7.53  - Periodical reporting criteria, Amount of reporting
 *                  The default value is infinity
 */
typedef enum
{
  L1_RPT_AMOUNT_INFINITY = 0,
  L1_RPT_AMOUNT_1  = 1,
  L1_RPT_AMOUNT_2  = 2,
  L1_RPT_AMOUNT_4  = 4,
  L1_RPT_AMOUNT_8  = 8,
  L1_RPT_AMOUNT_16 = 16,
  L1_RPT_AMOUNT_32 = 32,
  L1_RPT_AMOUNT_64 = 64
} l1_amount_of_reporting_enum_type;

/*
 * 10.3.7.53  - Periodical reporting criteria, Reporting Interval
 *                  Indicates the interval of periodical report
 *                  interval in milliseconds.
 */
typedef enum
{
  L1_RPT_INTERVAL_INVALID = 0,
  L1_RPT_INTERVAL_250MS   = 250,
  L1_RPT_INTERVAL_500MS   = 500,
  L1_RPT_INTERVAL_1000MS  = 1000,
  L1_RPT_INTERVAL_2000MS  = 2000,
  L1_RPT_INTERVAL_3000MS  = 3000,
  L1_RPT_INTERVAL_4000MS  = 4000,
  L1_RPT_INTERVAL_5000MS  = 5000,
  L1_RPT_INTERVAL_6000MS  = 6000,
  L1_RPT_INTERVAL_8000MS  = 8000,
  L1_RPT_INTERVAL_12000MS = 12000,
  L1_RPT_INTERVAL_16000MS = 16000,
  L1_RPT_INTERVAL_20000MS = 20000,
  L1_RPT_INTERVAL_24000MS = 24000,
  L1_RPT_INTERVAL_28000MS = 28000,
  L1_RPT_INTERVAL_32000MS = 32000,
  L1_RPT_INTERVAL_64000MS = 64000
} l1_reporting_interval_enum_type;


/*
 * 10.3.7.60  - Time Difference Accuracy enumerate type
 */
typedef enum
{
  L1_CELL_TIME_ACC_40_CHIPS = 40,     /* Cell timing accuracy is 40 chips */
  L1_CELL_TIME_ACC_256_CHIPS = 256,   /* Cell timing accuracy is 256 chips */
  L1_CELL_TIME_ACC_2560_CHIPS = 2560  /* Cell timing accuracy is 2560 chips */
} l1_cell_time_acc_enum_type;

/*
 * 10.3.7.61  - Reporting cell status possible combonations
 */
typedef enum
{
    L1_RPT_ASET,
      /* Report cells within Active Set */
    L1_RPT_MSET_USED_FREQ,
      /* Report cells within Monitored Set on used Freq. */
    L1_RPT_ASET_MSET_USED_FREQ,
      /* Report cells within Aset and MSet on used Freq. */
    L1_RPT_DSET_USED_FREQ,
      /* Report cells within Detected Set */
    L1_RPT_MSET_DSET_USED_FREQ,
      /* Report cells within Detected Set on used Freq. */
    L1_RPT_ALL_ASET_MSET_USED_FREQ,
      /* Report All Aset + MSet on used Freq. */
    L1_RPT_ALL_ASET_DSET_USED_FREQ,
      /* Report All Aset + DSet on used Freq. */
    L1_RPT_ALL_ASET_MSET_DSET_USED_FREQ,
      /* Report All Aset + MSet and/or DSet on used Freq. */
    L1_RPT_VASET,
      /* Report cells within Virtual Active Set */
    L1_RPT_MSET_NUSED_FREQ,
      /* Report cells within Monitored Set on Non-used Freq. */
    L1_RPT_ASET_MSET_NUSED_FREQ,
      /* Report cells within MSet and/or Aset on Non-used Freq. */
    L1_RPT_ALL_VASET_AND_MSET_NUSED_FREQ,
      /* Report cells within Virtual ASet + Mset on Non-used Freq. */
    L1_RPT_ASET_AND_VASET,
    L1_RPT_OTHER_RAT = L1_RPT_ASET_AND_VASET,
      /* Report cells within Active Set + Virtual ASet  or of the Other RAT */
    L1_RPT_ASET_AND_MSET_USED_NUSED
      /* Report cells within MSet and/or Aset on Used and Non-used Freq.*/
} l1_meas_report_cell_status_enum_type;


/*
 * 10.3.7.67  - RLC Buffers Payload, Average and Variance
 */
typedef enum
{
  L1_0_BYTES       = 0,
  L1_4_BYTES       = 4,
  L1_8_BYTES       = 8,
  L1_16_BYTES      = 16,
  L1_32_BYTES      = 32,
  L1_64_BYTES      = 64,
  L1_128_BYTES     = 128,
  L1_256_BYTES     = 256,
  L1_512_BYTES     = 512,
  L1_1K_BYTES      =   1*1024,
  L1_2K_BYTES      =   2*L1_1K_BYTES,
  L1_4K_BYTES      =   4*L1_1K_BYTES,
  L1_8K_BYTES      =   8*L1_1K_BYTES,
  L1_16K_BYTES     =  16*L1_1K_BYTES,
  L1_32K_BYTES     =  32*L1_1K_BYTES,
  L1_64K_BYTES     =  64*L1_1K_BYTES,
  L1_128K_BYTES    = 128*L1_1K_BYTES,
  L1_256K_BYTES    = 256*L1_1K_BYTES,
  L1_512K_BYTES    = 512*L1_1K_BYTES,
  L1_1024K_BYTES   = 1024*L1_1K_BYTES
} l1_num_bytes_enum_type ;


/*
 * 10.3.7.75  - Internal measurement event identity 
 */
typedef enum
{
  L1_INTERNAL_MEAS_EVENT_6A = 0,
    /* The UE Transmitted power becomes larger than an absolute threshold */
  L1_INTERNAL_MEAS_EVENT_6B,
    /* The UE Transmitted power becomes less than an absolute threshold */
  L1_INTERNAL_MEAS_EVENT_6C,
    /* The UE Transmitted power reaches its minimum value */
  L1_INTERNAL_MEAS_EVENT_6D,
    /* The UE Transmitted power reaches its maximum value */
  L1_INTERNAL_MEAS_EVENT_6E,
    /* The UE RSSI reaches the UEs dynamic receiveer range */
  L1_INTERNAL_MEAS_EVENT_6F,
    /* The UE Rx-Tx time difference for a RL included in the active set 
       becomes larger than an absolute threshold */
  L1_INTERNAL_MEAS_EVENT_6G
  /* The UE Rx-Tx time difference for a RL included in the active set 
     becomes less than an absolute threshold */

} l1_internal_meas_event_enum_type;

typedef enum
{
  L1_EUTRA_QUAN_RSRP,
  L1_EUTRA_QUAN_RSRQ,
  L1_EUTRA_QUAN_BOTH,
  L1_EUTRA_QUAN_NONE
} l1_eutra_meas_rpt_quantity_enum_type;

/*==========================================================================*/
/*=                                                                        =*/
/*=      Sub-Structure Type Declarations                                   =*/
/*=                                                                        =*/
/*==========================================================================*/


/***************************************************************************/
/**                                                                       **/
/**     25.331  Specification Related Sub-Structure Types                 **/
/**                                                                       **/
/***************************************************************************/

/**
 ** NOTE: Where Possible These IEs are listed in the order found in 25.331
 **       The message section, 10.2.xxx is dependent on the IEs defined
 **       in 10.3.7.xxx.  A number of IEs within 10.3.7.xxx are dependent
 **       on IEs from higher section numbers within the spec.  These IEs
 **       will be out of order below.
 **/

/**** 10.2... ****/
  /* see below, requires 10.3.7... IEs */

/**** 10.3.2... ****/

/*
 * 10.3.2.3  - The contents of this structure are derived from section
 *             10.3.2.3  RAT List sub-structure
 */
typedef struct
{  
  l1_rat_type_enum_type   rat_type;  

  int16                   s_interrat;  
    /* Inter-RAT (inter-system) measurement threshold. Value (in dB) */
    /* ranges (-32..20) by step of 2. 0xFFFF indicates the parameter */
    /* is not applicable.      */

  boolean                 s_hcsrat_incl;  
    /* Flag indicating whether s_srchhcs is included */  
  int16                   s_hcsrat;   
    /* HCS measurement threshold. Value (in dB) ranges (-105..91)  */
    /* by step of 2. 0xFFFF indicates the parameter is not applicable. */
  int16                   s_limit_srchrat;   
    /* Value (in dB) ranges (-32..20) by step of 2. 0xFFFF indicates */
    /* the parameter is not applicable.   */
} l1_interrat_resel_info_type;

/*
 * 10.3.2.4  - Cell Selection Re-Selection Info Structure Type
 */
typedef struct
{
  int16        qoffset1_s_n;
    /* Offset used for CPICH RSCP. Integer (-50..50), Default 0 */
  int16        qoffset2_s_n;
    /* Offset used for CPICH Ec/No. Integer (-50..50), Default 0 */
    /* Only required for WCDMA-FDD Cells                         */
  int16        max_tx_pwr;
    /* Maximum allowed ULTX power. UE_TXPWR_MAX_RACH dBm */
  int16        qual_min;
    /* Q-qualmin for Ec/Io. Int (-24..0) dBm. */
    /* Default Q-qualmin for serving cell.  */
    /* Only valid for WCDMA - FDD Cells */
  int16        rxlev_min;
    /* For FDD & TDD Cells: Q-rxlevmin for RSCP (dBm). */
    /* For GSM: Q-rxlevmin for GSM RSSI (dBm).         */
    /* Int (-115..-25 by step of 2).                   */
    /* Default value is Q-rxlevmin for serving cell.   */
} l1_cell_sel_resel_info_struct_type;

/*
 * 10.3.7.10 & 10.3.7.11  - HCS neighbor cell information
 */
typedef struct
{
  uint16        hcs_prio;
    /* Hierachical Cell Structrue Priority */
  uint16        q_hcs;
    /* Quality Threshold level for HCS cell reselection */
  uint16        penalty_time;
    /* Penalty_time. 0 means not used. next two not allowed for 0 */
  uint16        tmp_offset1;
    /* Temporary_offset used for CPICH RSCP*/
  uint16        tmp_offset2;
    /* Temporary_offset used for CPICH Ec/No */
} l1_nbr_cell_hcs_info_struct_type;

/*
 * 10.3.2.5  - Parameters pertinent to measurement mapping function
 */
typedef struct
{
  int16 num_rat;
    /* The number of RATs to be measured */
  struct
  {
   /*
    * Parameters for each RAT
    */
    int16  num_intvl;
      /* Number of mapping intervals */
    struct
    {
     /*
      * parameters for each mapping interval
      */
      l1_map_function_type_enum_type   func_type;
        /* Mapping function type, as specified by 10.3.2.5 of TS25.331 */
      int16   map_parm_1;
        /* Parameter used by the mapping function indicated above */
      int16   map_parm_2;
        /* Another parameter used by the mapping function indicated above */
      int16   up_limit;
        /* Upper limit, as specified by 10.3.2.5 of TS25.331 */
    } intvl[L1_MAX_MEAS_INTERVALS];
  } rat[L1_MAX_RAT];
} l1_mapping_info_struct_type;



/**** 10.3.8... ****/

/*
 * 10.3.8.2  BSIC - Base Station Identifier Code
 *           from 3GPP 23.003  section 4.3.2
 */
#define L1_BSIC_COLOR_CODE_MASK               0x07
typedef struct
{
  uint8 ncc;
    /* PLMN Colour Code low 3 bits are valid {b00000111}*/
  uint8 bcc;
    /* Base Station Colour Code low 3 bits are valid {b00000111} */
} l1_gsm_bsic_struct_type;



/**** 10.3.7... ****/

/*
 * 10.3.7.2  - Cell Info Structure Type
 */
typedef struct
{
  int32        cell_offset;
   /* Cell Individual Offset, Real (-10..10 by step of 0.5) dB, default 0  */

  boolean       ref_tm_diff_incl;
    /* Flag indicating whether Ref time diff to the serving cell is included */
  uint16        ref_tm_diff;
    /* Reference Time difference to Serving Cell, integer (0..38400) */
  l1_cell_time_acc_enum_type time_acc;
    /* Neighbor Cell timing accuracy relative to the serving cell */

  boolean      pri_scr_code_incl;
  uint16        pri_scr_code;
    /* Primary Scrambling Code. Not needed if measuring RSSI only. (0..511) */

  boolean       pri_cpich_txpwr_incl;
  int32        pri_cpich_txpwr;
    /*  Primary CPICH Tx power. Needed if calculating patchloss
        Units: Int  -10... +50 dBm */

  boolean       rd_sfn_ind;
    /*  Read SFN Indicator.TRUE means read of SFN requested for target cell*/

  boolean       tx_diver_ind;
    /* TX Diversity Indicator    */

  boolean       cell_sel_resel_info_incl;
    /* Flag to indicate if cell selection and reselection info is     */
    /* specified for this cell or not. If TRUE, the following fields  */
    /* are meaningful; otherwise, ignore the following fields.        */
    /* if HCS is not used and if all of the values are defaults, then */
    /* this field can be absent. */
  l1_cell_sel_resel_info_struct_type  cell_sel_info;

  boolean hcs_nbr_incl;
  l1_nbr_cell_hcs_info_struct_type  hcs_info;
    /* Neighbor cell HCS information */
} l1_cell_info_struct_type;


/*
 * 10.3.7.6  - Cell synchronization information
 */
typedef struct 
{
  l1_freq_mode_enum_type         freq_mode;
    /* choice FDD(supported), TDD(not-supported) */


  /* FDD */
  boolean  count_c_sfn_diff_incl;
  uint16 count_c_sfn_high;
    /* Integer(0..3840 by step of 256) in frames */
  uint16 off ;
    /* 5.1.9 of TS25.215 */
    /* OFF is defined as (SFN-CFNtx) mod 256 given in no. of frames */
    /* Has range:0...255  */

  uint32 tm;
    /* defined in 5.1.9 of TS 25.215  Range:0..38399     */
    /* Given both Tm and OFF , the SFN-CFN observed time */
    /* difference to cell is defined as: OFFx38400 + Tm  */


  /* TDD - unsupported */
  /* uint16 tdd; */

} l1_intra_meas_rpt_cell_sync_info_struct_type ;


/*
 * 10.3.7.3  - measured results for each cell 
 */
typedef struct
{
  l1_sfn_report_enum_type sfn_rpt_ind;
    /* SFN-SFN observed time diff reporting indicator */

  uint32 sfn_sfn_tm_diff ;
    /* Flag indicating if SFN-SFN observed time diff is included */
    /* 10.3.7.63 SFN-SFN observed time difference                 */
    /*            Range: 0....9830399 if Type 1 (chips)           */
    /*                 -20480..0..20480 if type 2 (1/16 of chips) */

    
  boolean cell_sync_info_incl;
    /* Flag indicating if cell synchronization info is included */

  l1_intra_meas_rpt_cell_sync_info_struct_type cell_sync_info;
    /* 10.3.7.6 cell synchronization information */

  uint16 pri_scr_code ;
    /* 10.3.6.60 Primary scriambling code 0...511 */

  l1_meas_quan_enum_type meas_quan_type;
    /* The type of measurement quantity */
  uint16      meas_quan_value;
    /* value of the quantity  measured */

  /*   
   *  The values will be returned as per the mapping specified in 25.133
   *  V3.6.0    
   *  
   *  For RSCP : Table 9-4   (eg., < -115 --> 00
   *                                 -115 -->01
   *                                    ...
   *                               >= -25 -->91
   *                               
   *  For ECNO:  Table 9-9  (eg:   < -24   -->00
   *                                 -24   -->01
   *                                 -23.5 -->02
   *                                   ...
   *                               >= 0   --->49
   */
  /* Flag indicating if the EcNo is included      */ 
  boolean ec_no_included;
  uint16 ec_no_val;

  /* Flag indicating if the RSCP value is included */
  boolean rscp_included;
  uint16 rscp_val;

  /* Flag indicating if the pathloss is included  */
  boolean pathloss_included;
  uint16 pathloss_val;

  boolean delta_rscp_included;
  int16 delta_rscp;

} l1_intra_meas_rpt_cell_info_struct_type ;


/*
 * 10.3.7.5  - Report quantities
 */
typedef struct
{
  l1_sfn_report_enum_type sfn_rpt;
    /* SFN-SFN observed time diff reporting indicator */
  boolean   cfn_sfn_delta_rpt;
    /* Cell synchronization information reporting indicator */
  boolean   cell_id_rpt ;
    /* Cell Identity reporting indicator */
  boolean   ec_no_rpt;
    /* CPICH Ec/No reporting indicator */
  boolean   rscp_rpt;
    /* CPICH RSCP reporting indicator */
  boolean   pathloss_rpt;
    /* Pathloss reporting indicator */
} l1_intra_dch_rpt_quan_struct_type;

/* 
 * 10.3.7.8 FACH measurement occasion info
 */
#define NUM_OTHER_RATS 2
typedef struct 
{  
  boolean k_ind;
    /* Indicates if FACH measurement occasion cycle length coefficient is present */
  uint32        k; 
    /*FACH measurement occasion cycle length coefficient */ 
  boolean       inter_f_ind; 
    /* Inter-frequency FDD Measurement indicator, N_FDD */ 
  uint16        num_inter_sys; 
    /* Number of different RATS to be measured. */ 
  l1_rat_type_enum_type        inter_sys[NUM_OTHER_RATS]; 
    /* What RATs to perform measurments on. N_GSM, and future N_2000 */ 
} l1_fach_meas_info_struct_type;

/* Enum defining all possible values for Tcrmax */
typedef enum
{
  L1_T_CR_MAX_NOT_USED,
  L1_T_CR_MAX_30S  = 30,
  L1_T_CR_MAX_60S  = 60,
  L1_T_CR_MAX_120S = 120,
  L1_T_CR_MAX_180S = 180,
  L1_T_CR_MAX_240S = 240
} l1_t_cr_max_enum_type;

/* Enum defining all possible values for Tcrmaxhyst */
typedef enum
{
  L1_T_CR_MAX_HYST_NOT_USED,
  L1_T_CR_MAX_HYST_10S = 10,
  L1_T_CR_MAX_HYST_20S = 20,
  L1_T_CR_MAX_HYST_30S = 30,
  L1_T_CR_MAX_HYST_40S = 40,
  L1_T_CR_MAX_HYST_50S = 50,
  L1_T_CR_MAX_HYST_60S = 60,
  L1_T_CR_MAX_HYST_70S = 70
} l1_t_cr_max_hyst_enum_type;

/*
 * 10.3.7.11  - HCS serving cell information
 */
typedef struct
{
  uint16   hcs_prio;
    /* Serving cell HCS priority. Value ranges (0..7). Default value is 0. */
  uint16   q_hcs;
    /* Quality threshold level for HCS cell reselection. Value ranges (0..99). */
    /* Default value is 0. */
  uint16   n_cr;
    /* maximum number fo cell reselections. Value ranges (1..16). Default */
    /* value is 8. */
  l1_t_cr_max_enum_type   t_crmax;
    /* Duration for evaluating alowed amount of cell reselections. */
    /* Default value is 0, which means the parameter is not used. When used, */
    /* the values are (0, 30, 60, 120, 180, 240), (in second). */
  l1_t_cr_max_hyst_enum_type   t_crmaxhyst;
    /* Additional time period before UE recert to low-mobility measurement. */
    /* Default value is 0, which means the parameter is not used. When used, */
    /* the value ranges (10..70) by step of 10, (in second). */
} l1_serv_cell_hcs_info_struct_type;

/*
 * 10.3.6.36  Frequency Info
 */
typedef struct
{
      boolean uarfcn_ul_incl;
        /* Flag indicationg UL UARFCN info is included. */
      uint16  uarfcn_ul;
        /* Uplink UARFCN, Nu (0..16383) */
      uint16  uarfcn_dl;
        /* Downlink UARFCN, Nd (0..16383) */
   
} l1_freq_info_struct_type;


/*
 * 10.3.7.13, 10.3.7.23  - Cell Info List Remove Command
 */
typedef enum
{
  L1_CELL_LIST_REMOVE_ALL_CELLS,
  L1_CELL_LIST_REMOVE_SOME_CELLS,
  L1_CELL_LIST_REMOVE_NO_CELLS
} l1_inter_sys_cell_list_remove_enum_type;


/*
 * 10.3.7.13  - Inter-frequency cell list information
 */
typedef struct
{
  /* Removed inter-freqency cells */
  uint16       num_cell_rmv;
    /* Number of inter-freq cells removed 0=none, 1..L1_MAX_CELL_MEAS */
    /* L1_REMOVE_ALL_CELLS(=0xFF)=remove all */
  struct /* for each removed cell   */
  {
    uint16    freq;
     /* Frequency for this cell  */
    uint16    psc;
     /* Primary Scrambling Code  */
  } rmv_cell[ L1_MAX_CELL_MEAS ];

  /* New inter-frequency cells */
  uint16       num_cell_add;
    /* Number of inter-frequency cells Added   */
  struct  /* for each added cell */
  {
    uint16    psc;
      /* Primary Scrambling Code  */
    boolean measure_in_idle_pch_fach;

    l1_freq_info_struct_type freq_info;
    l1_cell_info_struct_type  cell_info;
  } add_cell[ L1_MAX_CELL_MEAS ];

  boolean       hcs_used;
  /* 10.3.7.47  measurement cotrol system information  */
  /* Flag indicating if serving cell belongs to a HCS structure */

  /* If cells_for_meas_incl is set to TRUE, then cells mentioned in this 
     list should only be measured. For SIB's, its always set to FALSE*/
  boolean cells_for_meas_incl;

  /* No of cells to measure */
  uint16 num_cell_meas;

  /* Only cells mentioned in this list should be measured. This list 
     contains the psc and frequency for the cell */
  struct
  { uint16 psc;
    uint16 dl_freq;
  } meas_cells_list[ L1_MAX_CELL_MEAS ]; 
  

} l1_inter_freq_cell_list_struct_type;


/*
 * 10.3.7.15  - Inter-Frequency measured results list
 */
typedef struct 
{
    uint16 num_cells;

    l1_intra_meas_rpt_cell_info_struct_type cell[L1_MAX_CELL_MEAS];

} l1_intra_freq_addl_meas_rpt_struct_type;


/*
 * 10.3.7.15  Inter-Frequency Measured Results List
 */
typedef struct 
{
 /*
  * Inter-Frequency Measurement Results
  */
  uint16 num_msmt_results;
    /* number of inter-frequency measurement results included spec[1..maxFreq], 0=>no info */

  struct
  {
    l1_freq_info_struct_type  freq_info;
      /* frequency information */
    boolean            utra_carrier_rssi_incl;
      /* Flag indicationg UTRA Carrier RSSI info is included. */
    uint16             utra_carrier_rssi;
      /* UTRA_carrier_RSSI_LEV from 25.133 */


    /*
     * Inter-Frequency Cell Measurement Results
     */
     uint16 num_cells;
     /* Number of Cell Measured Results Included[1..maxCellMeas], 0=>no info */
    
     l1_intra_meas_rpt_cell_info_struct_type* cell[L1_MAX_CELL_MEAS];
      /* Cell Measured Results */

  } msmt_results[L1_MAX_FREQ];
  /* Inter-Frequency Measurement Results */


} l1_inter_freq_meas_results_struct_type;

/*
 * 10.3.7.17  Inter-Frequency Measurement Event Results
 */
typedef struct
{
  l1_inter_freq_event_enum_type  evt_id;
    /* event identity, 2a, 2b, etc. */
      
  uint16 num_freq;
    /* Inter-Frequency Cells  spec[1..maxFreq], 0=>no info */

  struct {
    l1_freq_info_struct_type freq_info;
      /* frequency information */

    /* 10.3.7.4  Cell Measurement Event Results */
    /* choice, FDD(supported)/TDD(not supported) */
    /* FDD */

    uint16 num_psc;

    uint16   prim_scr_code[L1_MAX_CELL_MEAS];
      /* Primary Scrambling Code Index */

  } freq [L1_MAX_FREQ] ;
  /* inter-frequency cells */

} l1_inter_freq_event_results;

/*
 * 10.3.7.*15*  - Inter-frequency measured results list *plus* inter-freq event results
 */
typedef struct 
{
  l1_inter_freq_event_results  event_results;
    /* 10.3.7.17 - event results for 10.3.7.7, from msmt report msg 10.2.19 */

  boolean meas_results_included;

  l1_inter_freq_meas_results_struct_type inter_freq_measurements;
    /* Inter-Frequency measurements */

} l1_inter_freq_meas_rpt_struct_type ;


/*
 * 10.3.7.38  - Intra-frequency measurement quantity
 */
typedef struct
{
  uint16    filter_coef;
    /* Filter coefficient as in 10.3.7.9 */
  l1_meas_quan_enum_type   quan_type;
    /* Measurement quantity. Note that, for WCDMA FDD cells, we currently 
       support Ec/Io or RSCP Measurement Quantity for DCH Measurement report.
     */
} l1_intra_freq_meas_quan_struct_type;


/*
 * 10.3.7.18  - Inter-frequency measurement quantity
 */
typedef struct
{
  
  l1_meas_type_enum_type rpt_choice;
  /* Can only be L1_INTRA_FREQ or L1_INTER_FREQ */
  
  l1_intra_freq_meas_quan_struct_type   interf_meas_quan;
    /* Inter-frequency measurment quantity as defined in 10.3.7.18    */
    /* (Currently matches the intra-frequency definition in 10.3.7.18 */
    /* For L1_INTER_FREQ, interf_meas_quan.l1_meas_quan_enum_type can L1_EC_N0 or L1_RSCP only */
    /* For L1_INTRA_FREQ, interf_meas_quan.l1_meas_quan_enum_type can be any value */
} l1_inter_freq_meas_quan_struct_type;


/*
 * 10.3.7.61
 */
typedef enum 
{
  L1_CELL_REPORTING_TYPE1,    /* Max number that can be reported is 6 */
  L1_CELL_REPORTING_TYPE2,     /* Max number that can be reported is 12 */
  L1_CELL_REPORTING_TYPE3      /* Max number that can be reported is 6 */ 
} l1_meas_report_cell_reporting_enum_type;


/*
 * 10.3.7.61  - Reporting cell status
 */
typedef struct
{
   l1_meas_report_cell_status_enum_type rpt_cell_stat;

   l1_meas_report_cell_reporting_enum_type rpt_cell_type;  

   uint16 max_num_rpt_cells ;
     /* Maximum number of reported cells (1..6) */
} l1_meas_rpt_cell_status_struct_type;


/*
 * 10.3.7.19  - Inter-Frequency measurement reporting criteria
 */
typedef struct
{
  l1_inter_freq_event_enum_type  evt_id;
    /* Event ID */
  int16  freq_thresh;
    /* Threshold used for frequency for event 2B, 2D, and 2F. range (-115..0). */
    /* Range used depend on measurement quantity: */
    /*      CPICH RSCP -115..-25 dBm; */
    /*      CPICH Ec/Io -24..0 dB; */
  uint16  w_used;
    /* Weight used for events 2A, 2B, 2D and 2F, value set to 0xFFFF for all */
    /* other events. Range (0..2.0) by step of 0.1, actual value passed is w_used * 10. */
  uint16  hyst;
    /* Hysteresis. range (0..19) dBm  in 0.5 dBm steps */ 

  uint16  time_to_trig;
    /* Time duration for which the event triggering condition needs to */
    /* be fulfilled before the report can be sent, in ms. */

  boolean   rpt_cell_status_incl;
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status;
    /* Reporting cell status */

  uint16    num_non_used_f_parm;
    /* Parameters Required for each non-used Frequency         */
    /* optional parameter, 0 => none, else 1..maxFreq included */
  struct
  {
    int16     non_used_f_thresh;
      /* Threshold for non used frequency. Used for events 2A, 2B, 2C and 2E */
      /* range (-115..0) */
      /* Range used depend on measurement quantity: */
      /*      CPICH RSCP -115..-25 dBm; */
      /*      CPICH Ec/Io -24..0 dB; */
    uint16    w_non_used_f;
      /* Weight used for non-used frequency for events 2A, 2B, 2C and 2E, */
      /* value set to 0xFFFF for all other events.  */
      /* Range (0..2.0) by step of 0.1, actual value passed is w_non_used_f * 10. */
  } non_used_f_parm_array[L1_MAX_NON_USED_FREQ];

} l1_inter_freq_event_crit_struct_type;


/*
 * 10.3.7.22  - Inter-frequency set update info
 */
typedef struct
{
  l1_set_update_mode_enum_type       auto_update_mode;
    /* Autonmous update mode  or 
       Non-Autonmous update mode, only used if auto_update_mode is set to OFF */
  boolean  radio_link_info_incl;
    /* Flag indicationg radio link info is included. */
  uint16   num_add_radio_link;
    /* Number of additional radio links to add. */
  uint16   add_psc[L1_MAX_RL];
    /* Primary Scrambling Code for radio links to add. */
  uint16   num_rmv_radio_link;
    /* Number of additional radio links to remove. */
  uint16   rmv_psc[L1_MAX_RL];
    /* Primary Scrambling Code for radio links to remove. */
} l1_inter_freq_set_update_struct_type;


/*
 * 10.3.7.21  - Inter-frequency reporting quantity
 */
typedef struct
{
  boolean          rpt_rssi;
    /* TRUE means report of the UTRA Carrier RSSI is requested */
  boolean          freq_qual_est;
    /* TRUE means report of the frequency quality estimat is requested */
    /* Currently not used, should be ignored */

  l1_intra_dch_rpt_quan_struct_type  other_rpt_quan;
} l1_inter_freq_rpt_quan_struct_type;


/*
 * 10.3.7.39  - criteria per event as defined in 10.3.7.39 of TS25.331.
 */
typedef struct
{
  l1_intra_freq_event_enum_type  evt_id;
    /* Event ID */
  l1_meas_event_trigger_enum_type  trig_cond;
    /* Event triggering condition (1 or 2, depending on event 1a,1b,etc.) */
  uint16   rpt_range;
    /* 0..29 by step of 0.5dB for event 1A and 1B. Set to 0xFFFF for all */
    /* other events. */
  uint16  num_cell_forbidded;
    /* Number of cells foebidden to affect report range for event 1A and 1B. */
    /* set to 0xFFFF for all other events. */
  uint16  forbidden_cell_scr_code[ L1_MAX_CELL_MEAS ];
    /* Primary Scrambling Code for each forbidden cell */
  uint16  w;
    /* Weight for even 1A and 1B. 0..20 by step of 0.1. set to 0xFFFF */
    /* for all other events. */
  uint16  hyst;
    /* Hysteresis. 0..7.5 by step of 0.5 dB, actual value is hyst*2 */
  int16  freq_thresh;
    /* Threshold used for frequency for event 1E and 1F. -125..165. */
    /* Range used depend on measurement quantity: */
    /*      CPICH RSCP -115..-40 dBm;             */
    /*      CPICH Ec/Io -24..0 dB;                */
    /*      Pathloss   30..165 dB.                */
  uint16  rpt_deact_thresh;
    /* Reporting deactivation threshold for event 1A, indicating        */
    /* the maximum number of cells allowed in the active set in order   */
    /* for event 1A to occur. value range 0..7. 0 means not applicable. */
  uint16 rpt_act_thresh;
    /* Reporting activation thresh for 1c. Indicates min. cells      */
    /* needed in ASET in order for 1C to occur. 0...7 , 0 means none */
  uint16  time_to_trig;
    /* Time duration for which the event triggering condition      */
    /* needs to be fulfilled before the report can be sent, in ms. */
  uint16  num_rpt;
    /* Measurement is "released" after the indicated amount (number) of   */
    /* reporting(s) from the UE itself. 0 means "infinity". For 1A or 1C. */
  uint16  rpt_intvl;
    /* Interval of periodical reporting, in ms, for */
    /* 1A or 1C. 0 means no periodical reporting.   */
  boolean   rpt_cell_status_incl;
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status;
    /* Reporting cell status */

  boolean evt_1d_cio_incl; /* TRUE means use CIO for Event 1D */

  boolean evt_1d_trig_cond_incl; /* TRUE means that trig condition specified */
} l1_intra_freq_event_crit_struct_type;


/*
 * 10.3.7.39  - Intra-frequency measurement reporting criteria
 */
typedef struct
{
  uint16 num_event;
  l1_intra_freq_event_crit_struct_type evt[L1_MAX_MEAS_EVENT];
} l1_intra_freq_report_crit_struct_type;


/*
 * 10.3.7.19  - Inter-frequency report criteria
 */
typedef struct
{
  uint16 num_event;
  l1_inter_freq_event_crit_struct_type evt[L1_MAX_MEAS_EVENT];
} l1_inter_freq_report_crit_struct_type;


/*
 * 10.3.7.53  - Periodical reporting criteria
 */
#define L1_PERIODIC_RPT_AMOUNT_INFINITY      0

typedef struct
{
  l1_amount_of_reporting_enum_type rpt_amount;
    /* Amount of reporting (1,2,4,8,16,32,64, Infinity */
    /* Measurement is "released" after the indicated amount (number) of */
    /* report(s) from the UE itself. 0 means "infinity".   */
  l1_reporting_interval_enum_type  rpt_interval;
    /* Reporting Interval  (250,500,1000,2000, ... ) in ms */
} l1_per_rpt_crit_struct_type;


typedef enum
{
  L1_INTRA_FREQ_EVT_TRIG,
  L1_INTER_FREQ_EVT_TRIG,
  L1_INTER_FREQ_PERIODIC,
  L1_INTER_FREQ_NO_REPORTING,
  L1_INTER_FREQ_NO_CHANGE
} l1_inter_freq_meas_report_mode_enum_type;
/*
 * 10.3.7.16  - Inter-frequency info
 */
typedef struct
{
  boolean cell_list_incl;
  /* flag to indicate if cell list is present  */
  l1_inter_freq_cell_list_struct_type  cell_list;
    /* cell information 10.3.7.13 */

  boolean   meas_quan_incl;
    /* flag to indicate if meas quantity is present  */
  l1_inter_freq_meas_quan_struct_type  meas_quan;
    /* Inter-frequency measurement quantity as in 10.3.7.18 of TS25.331 */

  boolean rpt_quan_incl ;
    /* flag indicating if the reporting quantities are given. If yes, next four 
       fields are valid */
  l1_inter_freq_rpt_quan_struct_type    rpt_quan;
    /* Inter-frequency reporting quantity as defined in 10.3.7.21 of TS25.331 */

  boolean rpt_cell_status_incl;  
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status; 
    /* Reporting cell status as in 10.3.7.61 */
    /* Optional field if reporting criteria is "Periodic reporting" or "No report" */
    /* otherwise not needed */ 


  boolean  inter_freq_set_update_incl;
    /* flag indicating if the inter-frequency set update is included. */
  l1_inter_freq_set_update_struct_type  update;
    /* Inter-frequency set update info as defined in 10.3.7.22 of TS25.331 */

  boolean rpt_mode_incl;
  /* Boolean because this can be optional for MODIFY */
  l1_inter_freq_meas_report_mode_enum_type   rpt_mode;
    /* Measurement reporting mode as in 10.3.7.49  */

  union {
    l1_intra_freq_report_crit_struct_type    intra_rpt_crit;
      /* Event Triggered Measurement reporting criteria as in 10.3.7.39  */
      /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */

    l1_inter_freq_report_crit_struct_type    inter_rpt_crit;
      /* Event Triggered Measurement reporting criteria as in 10.3.7.19  */
      /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */

    l1_per_rpt_crit_struct_type periodic_crit;
      /* Periodical Measurement reporting criteria as in 10.3.7.53  */
      /* Meaningful if "rpt_mode" is L1_PERIODIC_RPT */
  } u;

} l1_inter_freq_meas_ctrl_struct_type;


/*
 * derived from 10.3.7.23  - Inter-RAT Cell Info List,  GSM Information
 */
typedef struct  /* information describing each GSM cell*/
{
    int16                           cio;
      /* Cell Individual Offset value */
      /* Value used to offset the measured quantity in dB, */
      /* range (-50..50) */
    l1_gsm_bsic_struct_type         bsic;
      /* BSIC 10.3.8.2 */
    l1_gsm_band_indicator_enum_type band_indicator;
      /* Band, DCS 1800 or PCS 1900 */
    W_ARFCN_T                          bcch_arfcn;
      /* ARFCN  Integer (0..1023) */
} l1_gsm_cell_info_type;


/*
 * derived from 10.3.7.23  - Inter-RAT cell info list,  GSM Cell Info
 */
typedef struct 
{
  boolean cell_sel_info_incl;
    /* Flag indicationg Cell Re-Selection Info SIB 11/12 info is included. */
  l1_cell_sel_resel_info_struct_type  cell_sel_info;
    /* Cell Selection and Re-Selection Info SIB 11/12 */
  l1_gsm_cell_info_type    gsm_info;
    /* cell identification information */
  l1_nbr_cell_hcs_info_struct_type hcs_info;
    /* Neighbor cell HCS information */
} l1_gsm_cell_sel_info_struct_type;


/*
 * 10.3.7.23  - Inter-RAT (Inter-Stystem) Cell Info List as in 10.3.7.23 of TS25.331
 */
typedef struct
{
 /*
  * Remove inter-RAT cells
  */
  uint16       num_cell_rmv;
    /* Number of inter-RAT cells removed 0=none, 1..L1_MAX_CELL_MEAS */
    /* L1_REMOVE_ALL_CELLS(=0xFF)=remove all */
  struct /* for each removed cell   */
  {
    uint16    intersys_cell_id;
      /* Inter-RAT cell id, unique value in the range (0..<maxCellMeas-1>) */
  } rmv_cell[ L1_MAX_CELL_MEAS ];

 /*
  * Add inter-system cells
  */
  uint16       num_cell_add;
    /* Number of inter-system cells Added  */
  struct
  {
    uint16  intersys_cell_id;
      /* Cell Id, Unique value in the range (0..<maxCellMeas-1>) */
      /* Cell Id is an optional parameter for add cell, but L1 will require */
      /* RRC to select a Cell Id for use, so the choice will be made on */
      /* the UE in one place */

    boolean measure_in_idle_pch_fach;

    l1_rat_type_enum_type  cell_info_type;

    union
    {
      l1_gsm_cell_sel_info_struct_type  gsm_cell;

      uint16  is2000_cell;
        /* IS2000 unsupported */
    } u;

  } add_cell[ L1_MAX_CELL_MEAS ];


  boolean cells_for_meas_incl;
    /* If cells_for_meas_incl is set to TRUE, then cells   */
    /* mentioned in this list should only be measured. For */
    /* SIB's, its always set to FALSE  */

  uint16 num_cell_meas;
    /* Number of cells to measure */
  
  uint16 meas_cells_list[ L1_MAX_CELL_MEAS ]; 
    /* Only cells mentioned in this list should be measured.  */
    /* This list contains the inter-system cell id            */

} l1_inter_sys_cell_list_struct_type;


/*
 * 10.3.7.26  Inter-RAT Measured Results List, Measured Cells
 */
typedef struct
{
  boolean gsm_carrier_rssi_incl;
    /* Flag indicationg GSM Carrier RSSI info is included. */
  int16   gsm_carrier_rssi;
    /* GSM Carrier RSSI */

  l1_bsic_choice_enum_type bsic_choice;
    /* BSIC Choice, Verified or Non-Verified */

  union
  {
    struct
    {
      uint16  inter_sys_cell_id;
        /* Inter-RAT cell id  spec[0..(maxCellMeas-1)] */
    } bsic_verified;
    /* Verified BSIC Information */

    struct
    {
      W_ARFCN_T bcch_arfcn;
       /* BCCH ARFCN  spec[0..1023] */
            } bsic_not_verified;
    /* Non-Verified BSIC Information */
  } u;

  boolean gsm_time_diff_incl;
    /* Flag indicationg UTRA Carrier RSSI info is included. */
  uint16  fn_offset;
  uint16  qbit_offset;
  uint16  gsm_time_diff;
    /* observed time difference to GSM cell, GSM_TIME in 25.133 spec[0..4095]*/
} l1_measured_cells_struct_type;

/*
 * 10.3.7.26  Inter-RAT Measured Results List
 */
typedef struct 
{
  uint16 num_msmt_results;
    /* number of inter-RAT measurement results included spec[1..maxOtherRAT], 0=>no info */

  struct
  {
    l1_rat_enum_type system;
      /* System Type, GSM is only supported choice currently */
    union
    {
      /** GSM **/
      struct
      {
        uint16 measured_gsm_cells;
          /* Number of GSM Cells Measured */
        l1_measured_cells_struct_type measured_cells[L1_MAX_REPORTED_GSM_CELLS];
          /* Measured Cells Information */

      } gsm;

    } u;

  } inter_sys_msmt_results [L1_MAX_OTHER_RAT];

} l1_inter_sys_meas_result_struct_type;


/*
 * 10.3.7.28  - Inter-RAT Measured Results List
 */
typedef struct
{
  l1_inter_sys_event_enum_type  event_id;
    /* Inter-RAT Event Identity 3a,3b,3c,3d */

  uint16 num_cells;
    /* Inter-RAT number of cells to report spec[1..maxCellMeas], 0=>no info */

  struct {
    l1_bsic_choice_enum_type bsic_choice;
      /* BSIC Verified or Non-Verified */

    union {
      struct {
        uint16 intersys_cell_id;
          /* Inter-RAT Cell Id spec[0..(maxCellMeas-1)] */
      } bsic_verified;

      struct {
        W_ARFCN_T bcch_arfcn;
          /* BCCH ARFCH spec[0..1023] */
      } bsic_nonverified;
    } u;
  } cell [L1_MAX_CELL_MEAS] ;

} l1_inter_sys_event_results; 


/*
 * 10.3.7.*26*  - Inter-RAT Measured Results *plus* Measurement Event Results
 */
typedef struct 
{
  l1_inter_sys_event_results event_results;
    /* 10.3.7.28  Inter-rat  Event Results for 10.3.7.7, from msmt report msg 10.2.19 */

  l1_inter_sys_meas_result_struct_type measurements;
    /* 10.3.7.26  Inter RAT Measurements */

} l1_inter_sys_meas_rpt_struct_type ;


/*
 * derived from 10.3.7.32  - Inter-RAT reporting quanity, CHOICE system:GSM
 */
typedef struct
{  
 /*
  * TRUE for either of these Boolean types means that inclusion
  * of the indicated field in the report is requested
  */
  boolean        time_diff;  
    /* Observed time difference to GSM cell Reporting indicator */  
  boolean        rssi_rpt_ind;  
    /* GSM Carrier RSSI Reporting indicator */
} l1_dch_gsm_rpt_quan_struct_type;


/*
 * 10.3.7.32  - Inter-RAT Reporting quantity
 */
typedef struct
{  
  boolean                  utran_est_qual;    
    /* Not used in this release should always be FALSE */  
  l1_rat_type_enum_type             sys_choice;  

  union
  {
    l1_eutra_meas_rpt_quantity_enum_type eutra_rpt_quan;
  }u;

    /* Currently only GSM is defined */  
  l1_dch_gsm_rpt_quan_struct_type   gsm_rpt_quan;
} l1_inter_sys_dch_rpt_quan_struct_type;

typedef struct 
{
    l1_eutra_meas_rpt_quantity_enum_type    meas_quan;
      /* EUTRA measurement quantity*/
    l1_common_filter_coef_enum_type      filter_coef;
      /* Filter Coefficient */
} l1_eutra_meas_quan_struct_type;

/*
 * 10.3.7.29  - Inter-RAT Measurement Quantity
 */
typedef struct 
{
    l1_gsm_meas_quantity_enum_type    meas_quan;
      /* GSM_CARRIER_RSSI */
    l1_gsm_filter_coef_enum_type      filter_coef;
      /* Filter Coefficient */
    boolean   bsic_ver_req;
      /* True indicates BSIC Verification Required */
} l1_gsm_meas_quan_struct_type;


/*
 * 10.3.7.29  - Inter-system measurement quantity
 */
typedef struct
{
  boolean intra_meas_quan_incl;
    /* Flag indicationg Intra-Freq Meas Quantity info is included. */
  l1_intra_freq_meas_quan_struct_type intra_meas_quan;
    /* measurement quantity for UTRAN quality estimate */

  l1_rat_type_enum_type               system_choice; 
    /* Inter-system choice for measurements */
  union  
  {
    l1_gsm_meas_quan_struct_type      gsm_meas_quan;
    uint16   is2000_meas_quan;
    l1_eutra_meas_quan_struct_type     eutra_meas_quan;
  } u;


  /* Measurement quantity. Note that, for WCDMA FDD cells, we currently     */
  /* support Ec/Io or RSCP Measurement Quantity for DCH Measurement report. */
} l1_inter_sys_meas_quan_struct_type;


/*
 * derived from 10.3.7.30  - Inter-RAT Measurement Report Criteria
 */
typedef struct
{
  l1_inter_sys_event_enum_type  evt_id;  
    /* Event ID */  
  int16   own_thresh;   
    /* Threshhold for own system, range (-115..0). Only applies to event 3A. */  
  uint16   w;  
    /* Used for event 3A, the actual value of W is a real number in the range
     * (0.1..2.0) by 0.1 steps, value passed here is W*10  
     */  
  int16   other_thresh;   
    /* Threshhold for other system, range (-115..0). Only applies to events   
     * 3A, 3B and 3C.   
     */  
  uint16  hyst;   
    /* Hysteresis. 0..7.5 by .5 dB steps, value passed is Hysteresis*2 */  
  uint16  time_to_trig;   
    /* Time duration for which the event triggering condition needs to   
     * be fulfilled before the report can be sent, in ms.     
     */  
  boolean   rpt_cell_status_incl;  
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status;  
    /* Reporting cell status */
} l1_inter_sys_event_crit_struct_type;


/*
 * 10.3.7.30  - Inter-Rat Measurement report criteria
 */
typedef struct
{ 
  uint16         num_event;  
  l1_inter_sys_event_crit_struct_type    evt[L1_MAX_MEAS_EVENT];
}  l1_inter_sys_report_crit_struct_type;


/*
 * 10.3.7.27  - Inter-RAT Measurements, Report Criteria
 */
typedef struct
{
  l1_meas_report_mode_enum_type  rpt_mode;  
    /* CHOICE report criteria in 10.3.7.27 */
  union
  {
    l1_inter_sys_report_crit_struct_type    evt_rpt_crit; 
      /* Event Triggered Measurement reporting criteria as in 10.3.7.30  */ 
      /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */  
  
    l1_per_rpt_crit_struct_type periodic_crit;
      /* Periodical Measurement reporting criteria as in 10.3.7.53 */  
      /* Meaningful if "rpt_mode" is L1_PERIODIC_RPT */    
  } u;

} l1_inter_sys_reporting_criteria_struct_type;


/*
 * 10.3.7.27  Inter-RAT Measurement
 */
typedef struct
{
  boolean cell_list_incl;  
    /* flag to indicate if cell info is present  */
  l1_inter_sys_cell_list_struct_type  cell_list;  
    /* cell information 10.3.7.23 */    

  boolean meas_quan_incl;
    /* flag to indicate if meas quantity is present  
     * If true, the next field is valid */  
  l1_inter_sys_meas_quan_struct_type  meas_quan;
    /* Intra-frequency measurement quantity as in 10.3.7.29 of TS25.331 */  

  boolean rpt_quan_incl;
    /* flag indicating if the reporting quantities are given. */
  l1_inter_sys_dch_rpt_quan_struct_type   inter_sys_quan;  
    /* Rpt quantity for Inter-RAT set as defined in 10.3.7.32 of TS25.331 */  

  boolean rpt_cell_status_incl;  
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status; 
    /* Reporting cell status as in 10.3.7.61 */
    /* Optional field if reporting criteria is "Periodic reporting" or "No report" */
    /* otherwise not needed */  

  l1_inter_sys_reporting_criteria_struct_type  rpt_criteria;
    /* reporting criteria, periodic, event, and parameters */

} l1_inter_sys_meas_ctrl_struct_type;


/*
 * 10.3.7.33  - Intra-Frequency Cell Info List
 */
typedef struct
{
  /* Removed intra-freqency cells */
  uint16       num_cell_rmv;
    /* Number of intra-frequency cells removed */
  struct /* for each removed cell   */
  {
    uint16    psc;
     /* Primary Scrambling Code  */
  } rmv_cell[ L1_MAX_CELL_MEAS ];

  /* New intra-frequency cells */
  uint16       num_cell_add;
    /* Number of intra-frequency cells Added   */
  struct  /* for each added cell */
  {
    uint16    psc;
      /* Primary Scrambling Code  */

    boolean measure_in_idle_pch_fach;

    l1_cell_info_struct_type  cell_info;
  } add_cell[ L1_MAX_CELL_MEAS ];

  /* If cells_for_meas_incl is set to TRUE, then cells mentioned in this list should
     only be measured. For SIB's, its always set to FALSE */
  boolean cells_for_meas_incl;

  /* No of cells to measure */
  uint16 num_cell_meas;
  
  /* Only cells mentioned in this list should be measured. This list contains psc */
  uint16 meas_cells_list[ L1_MAX_CELL_MEAS ];
} l1_intra_freq_cell_list_struct_type;

/*
 * 10.3.7.35 Intra-frequency measured results list
 */
typedef struct 
{
/* --change, intra-freq code to use rpt_crit in 10.2.19 struct */
  l1_meas_report_mode_enum_type rpt_crit;
    /*  Describes the criterion of the  report Periodic/event triggered */

  boolean cell_meas_result_incl;
    /* flag indicating whether measured_results are included  */
    /* If FALSE, ignore next two fields  */
  uint16 num_cells ;
    /* num of cells with measured results */
   
  l1_intra_meas_rpt_cell_info_struct_type cell[L1_MAX_CELL_MEAS];
    /* measured results for each cell 10.3.7.3  */
   
  uint16 evt_id ;
    /* The event that triggered this report. Applicable only if */
    /* the rpt_crit is L1_EVENT_TRIGGER_RPT */

  uint16 num_cells_for_evt;

  uint16 cell_psc[L1_MAX_CELL_MEAS];

  boolean last_periodic_rpt_sec;

} l1_intra_freq_meas_rpt_struct_type ;


/*
 * Intra-frequency info 10.3.7.36
 */
typedef struct
{
  boolean cell_list_incl[MAX_NUM_CARR];
    /* flag to indicate if cell info is present  */

  l1_intra_freq_cell_list_struct_type  cell_list[MAX_NUM_CARR];
    /* cell information 10.3.7.33 */

  
  boolean meas_quan_incl ;
    /* flag to indicate if meas quantity is present */
    /* If true, the next two fields are valid       */

  l1_intra_freq_meas_quan_struct_type  meas_quan;
    /* Intra-frequency measurement quantity as in 10.3.7.38 of TS25.331 */

  boolean intra_f_rpt_quan_incl ;
    /* flag indicating if the reporting quantities are given. */
    /* If yes, next four fields are valid */

  /* Intra-frequency reporting quantity as defined in 10.3.7.5 of TS25.331 */
  l1_intra_dch_rpt_quan_struct_type    aset_quan;
    /* Rpt quantity for active set as defined in 10.3.7.41 of TS25.331 */

  l1_intra_dch_rpt_quan_struct_type    mset_quan;
    /* Rpt quantity for monitored set as defined in 10.3.7.41 of TS25.331 */

  boolean       dset_quan_incl;
  l1_intra_dch_rpt_quan_struct_type    dset_quan;
    /* Rpt quantity for detected set as defined in 10.3.7.41 of TS25.331 */

  l1_meas_report_mode_enum_type   rpt_mode[MAX_NUM_CARR];
    /* Measurement reporting mode as in 10.3.7.49  */

  /* Freq info. 10.3.6.36 */
  l1_freq_info_struct_type freq_info[MAX_NUM_CARR];
  /* Valid only for secondary UL freq */
  
  l1_intra_freq_report_crit_struct_type  rpt_crit[MAX_NUM_CARR];
    /* Event Triggered Measurement reporting criteria as in 10.3.7.119  */
    /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */

  boolean  periodic_rpt_crit_incl[MAX_NUM_CARR];
  /* Flag indicating whether priodic report criteria is included */

  l1_per_rpt_crit_struct_type   periodic_rpt_crit[MAX_NUM_CARR]; 
  /* Periodical Measurement reporting criteria as in 10.3.7.53  */
  /* Meaningful if "rpt_mode" is L1_PERIODIC_RPT */

  boolean rpt_cell_status_incl[MAX_NUM_CARR];
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status[MAX_NUM_CARR];
  /* Reporting cell status. Valid only if reporting criteria is */
  /* "Periodic reporting" or "No report"  */

  uint16  num_rpt;
    /* Measurement is "released" after the indicated amount (number) of */
    /* reporting(s) from the UE itself. 0 means "infinity".   */

  uint16  rpt_intvl;
    /* Interval of periodical reporting, in ms. */
  
} l1_intra_freq_meas_ctrl_struct_type;


/*
 * 10.3.7.42  - Intra-frequency reporting quantity for RACH reporting
 */
typedef struct
{
  l1_sfn_report_enum_type    rach_sfn_tm_delta;
    /* SFN-SFN observed time difference */
  l1_meas_quan_enum_type     rach_rpt_quan;
    /* RACH Reporting Quantity. Note that, for WCDMA FDD cells, we currently 
       support Ec/Io or RSCP Measurement Quantity for DCH Measurement report.
     */
} l1_intra_freq_rach_rpt_quan_struct_type;


/*
 * 10.3.7.62  - Reporting information in state CELL_DCH
 */
typedef struct
{
  /* Intra-frequency reporting quantity as defined in 10.3.7.5 of TS25.331 */
  l1_intra_dch_rpt_quan_struct_type    aset_quan;
    /* Rpt quantity for active set as defined in 10.3.7.41 of TS25.331 */
  l1_intra_dch_rpt_quan_struct_type    mset_quan;
    /* Rpt quantity for monitored set as defined in 10.3.7.41 of TS25.331 */
  boolean       dset_quan_incl;
  l1_intra_dch_rpt_quan_struct_type    dset_quan;
    /* Rpt quantity for detected set as defined in 10.3.7.41 of TS25.331 */

  l1_meas_report_mode_enum_type   rpt_mode;
    /* Measurement reporting mode as in 10.3.7.49  */

  l1_meas_report_trans_mode_enum_type  rpt_trans_mode;
    /* Measuremnt Report transfer criteria to UTRAN */

 /*
  * Event Triggered Measurement reporting criteria as in 10.3.7.39
  * Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT
  */
  l1_intra_freq_report_crit_struct_type    rpt_crit;
  
 /* Periodical Measurement reporting criteria as in 10.3.7.53
  * Meaningful if "rpt_mode" is L1_PERIODIC_RPT
  */
  uint16  num_rpt;
    /* Measurement is "released" after the indicated amount (number) of */
    /* reporting(s) from the UE itself. 0 means "infinity".  */
  uint16  rpt_intvl;
    /* Interval of periodical reporting, in ms. 0 means no periodical */
    /* reporting. */
} l1_intra_freq_dch_rpt_info_struct_type;


/*
 * 10.3.7.40  - Intra-frequency measurement system information
 */
typedef struct
{
  uint16       meas_id;
    /* Intra-frequency measurement identity number, default as 1 */

  boolean cell_list_incl;
    /* Flag indicating whether the cell list is included  */
  l1_intra_freq_cell_list_struct_type  cell_list;
    /* Intra-Frequency Cell Info List as in 10.3.7.33 of TS25.331 */
    
  boolean meas_quan_incl;
    /* Flag indicating whether the measurement quantity is included  */
  l1_intra_freq_meas_quan_struct_type  meas_quan;
    /* Intra-frequency measurement quantity as in 10.3.7.38 of TS25.331 */

  boolean rach_rpt_quan_incl;
    /* Flag indicating whether the RACH reporting quantity is included  */
  l1_intra_freq_rach_rpt_quan_struct_type  rach_rpt_quan;
    /* Intra-frequency reporting quantity for RACH reporting as in 10.3.7.42 */

  boolean rach_rpt_max_incl;
    /* Flag indicating whether the max no of reported cells on RACH info is included */
  l1_intra_freq_rach_rpt_max_enum_type    rach_rpt_max;
    /* Maximum number of reported cells on RACH as in 10.3.7.43 */

  boolean   intra_f_dch_rpt_info_incl;
    /* Reporting info for CELL_DCH included  */
  l1_intra_freq_dch_rpt_info_struct_type  dch_rpt_info;
    /* Reporting information in state CELL_DCH, as in 10.3.7.62 */
} l1_intra_freq_sib_struct_type;

/* Bler per transport channel */
typedef struct
{
  /* Tr Channel Identifier 1..32 */
  uint8 transport_ch_identity;

  /* Has bler been measured on this transport channel.
     If false ignore the next field */
  boolean bler_present;

  /* BLER Value 0..63 used only for periodic reports*/
  uint8 bler_value;
} l1_transport_channel_bler_struct_type;

/*
 * 10.3.7.55  - Quality Measured Results List
 */

/* Using the same structure for both periodic and event reporting. For event
   reporting the no_of_tr_ch is set as 1 and only one channel is included in 
   the array */
typedef struct 
{
 /* No of Transport Channels 1..32 for both event triggered and periodic reports*/
  uint8 no_of_tr_ch;
  
  l1_transport_channel_bler_struct_type l1_trch_bler[L1_MAX_TRANSPORT_CHANNELS];
} l1_qual_meas_rpt_struct_type;

/* For qual_meas additional measurements */

/* Event Triggered Cirteria for Quality Measurements */
typedef struct
{
  /* 1 ..32 */
  uint8 no_of_transport_channels;

  struct
  {
    /* Tranpsort Channel Identifier */
    uint8 tr_ch_id;

    /* Total No of CRC's before 5A could be generated 1..512 */
    uint16 total_crc;

    /* BAD CRC's during the window */
    /* 1..512 */
    uint16 bad_crc;

    /* Pending time after trigger CRC's */
    /* 1..512 */
    uint16 pending_time_after_trigger;

  } tr_ch[L1_MAX_TRANSPORT_CHANNELS];

} l1_qual_event_crit_struct_type;

/*
 * 10.3.7.56  - Quality Measurement
 */
typedef struct
{
  /* Meas Quantity is always BLER. No need to indicate explicitly */
  /* Reporting Quantity to be included. */
  boolean rpt_quan_included;

  /* BLER reporting is requested? If this is yes, next two fields are Mandatory */
  boolean bler_rpt_requested;
  
  /* True specifies that BLER to be reported for all DL transport channels. If set to TRUE
     Ignore the next two */
  boolean rpt_all_transport_ch;

  /* No of Transport Channels. 1..32 */
  uint8 no_of_tr_ch;

  uint8 reported_transport_channels[L1_MAX_TRANSPORT_CHANNELS];

  /* Reporting Mode */
  l1_meas_report_mode_enum_type rpt_mode;

  union {
    l1_qual_event_crit_struct_type    evt_trig_crit;
      /* Event Triggered Measurement reporting criteria as in 10.3.7.80  */
      /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */

    l1_per_rpt_crit_struct_type periodic_crit;
      /* Periodical Measurement reporting criteria as in 10.3.7.53  */
      /* Meaningful if "rpt_mode" is L1_PERIODIC_RPT */
  } u;
} l1_qual_meas_ctrl_struct_type;

/*
 * 10.3.7.67  - Traffic Volume Measured Results List
 */
typedef struct 
{
/* djm - to be finished */
    uint16 num_msmt_results;
      /* Traffic Volume Measurement Results spec[1..maxRB], 0=>no info */

    struct {
      uint16 rb_id;
        /* RB Identity 10.3.4.16  spec[1..32] */

      l1_num_bytes_enum_type  rlc_buffers_payload;
        /* op - RLC Bufferes Payload, in bytes */

      l1_num_bytes_enum_type  average_rlc_buffers_payload;
        /* op - Average of RLC Buffer Payload */

      l1_num_bytes_enum_type  variance_rlc_buffers_payload;
        /* op - Variance of RLC Buffer Payload */
    } msmt_info [L1_MAX_RB] ;

} l1_traf_vol_meas_rpt_struct_type ;


/*
 * 10.3.7.67  - Traffic Volume Measured Results List
 */
typedef struct 
{
  /***** to be completed *****/
  uint16 dummy;
} l1_traf_vol_addl_meas_rpt_struct_type;
/* For traf_vol additional measurements */


/*
 * 10.3.7.68  - Traffic Volume Measurement
 */
typedef struct
{
  /********** To Be completed *************/
  uint16 dummy;
} l1_traf_vol_meas_ctrl_struct_type;



typedef struct
{
  /* Primary CPICH info (0...511) */
  uint16 pri_scr_code;

  /* 10.3.7.83 Range (768...1280) */
  uint16 rx_tx_diff_type_1;

} l1_internal_meas_rx_tx_rpt_struct_type;

typedef struct
{
   /* UE Tx power info included ? */
   boolean ue_tx_pwr_included;

   /* Range (0...85) according to 25.133*/
   uint16  ue_tx_pwr;

   /* Rx-Tx diff info included */
   boolean ue_rx_tx_time_diff_included;

   /* Num RL for which Rx-Tx diff info included */
   uint16  num_rl;

   l1_internal_meas_rx_tx_rpt_struct_type rx_tx_rpt[L1_MAX_RL];

} l1_internal_cell_measured_results_struct_type;

/*
 * 10.3.7.76  - UE Internal Measured Results
 */
typedef struct 
{
  /* Indicates whether cell measured results included */
  boolean cell_measured_results_included;

  l1_internal_cell_measured_results_struct_type cell_measured_result_info;

  l1_internal_meas_event_enum_type evt_id ;
    /* The event that triggered this report. Applicable only if */
    /* the rpt_crit is L1_EVENT_TRIGGER_RPT */

  /* Only valid if report is for event 6f/6g */
  uint16 cell_psc;

} l1_ue_internal_meas_rpt_struct_type;
/* For ue_internal additional measurements */


typedef enum
{
  L1_UE_TX_PWR,
  L1_UTRA_RSSI,
  L1_RX_TX_DIFF,
  L1_MEAS_NO_MEAS
} l1_internal_meas_quan_type;


/*  10.3.7.82 */
typedef struct
{
  boolean ue_tx_pwr_rpt;

  boolean ue_rx_tx_diff_rpt;

} l1_internal_meas_rpt_quan_info_struct_type;


/* 10.3.7.80 */
typedef struct
{
  /* event id */
  l1_internal_meas_event_enum_type event_id;

  /*  Time To Trigger */
  uint16 ttt;

  /* Tx power threshold dbm units. ( -50 ... +33) valid for 6A/6B */
  int16 ue_tx_pwr_thresh;

  /* Rx-Tx time diff thresh. in chips ( 768...1280) valid for 6F/6G */
  uint16 rx_tx_diff_thresh;

} l1_internal_meas_evt_crit_struct_type;

typedef struct
{
  /* Num events */
  uint16 num_event;

  l1_internal_meas_evt_crit_struct_type evt[L1_MAX_MEAS_EVENT];

}l1_internal_meas_rpt_crit_struct_type;

/* Internal measurements SIB info */
typedef struct 
{
  /* measurement id */
  uint16 meas_id ;

  /* Measurement quantity 10.3.7.79 */
  l1_internal_meas_quan_type meas_quan;

}l1_internal_meas_sib_struct_type;


/*
 * 10.3.7.77  - UE Internal Measurement
 */
typedef struct
{
  /* If this is yes, next two fields are Mandatory */
  boolean meas_quan_included;

  /* Measurement quantity 10.3.7.79 */
  l1_internal_meas_quan_type meas_quan;

  boolean filter_coeff_incl;

  l1_wcdma_filter_coef_enum_type filt_idx;

  boolean rpt_quan_included;

  /* 10.3.7.82 */
  l1_internal_meas_rpt_quan_info_struct_type rpt_quan;

  l1_meas_report_mode_enum_type  rpt_mode;

  union {
    l1_internal_meas_rpt_crit_struct_type    evt_trig_crit;
      /* Event Triggered Measurement reporting criteria as in 10.3.7.80  */
      /* Meaningful if "rpt_mode" is L1_EVENT_TRIGGER_RPT */

    l1_per_rpt_crit_struct_type periodic_crit;
      /* Periodical Measurement reporting criteria as in 10.3.7.53  */
      /* Meaningful if "rpt_mode" is L1_PERIODIC_RPT */
  } u;

} l1_ue_internal_meas_ctrl_struct_type;


/*
 * 10.3.7.99  - UE Positioning Measured Results
 */
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_ue_positioning_measured_results_type;


/*
 * 10.3.7.100  - UE Positioning Measurement
 */
typedef struct
{
  /********** To Be completed *************/
  uint16 dummy;
} l1_ue_pos_meas_ctrl_struct_type;


/*
 * 10.3.7.44  Measured Results
 */
typedef struct
{
  l1_meas_type_enum_type  meas_type;
    /* Describes the type of measurement  */

 /*
  * Measured Results
  */
  union 
  {
    l1_intra_freq_meas_rpt_struct_type intra_f_rpt ;
      /* Intra-frequency measurement report info 10.3.7.35  */
    l1_inter_freq_meas_rpt_struct_type inter_f_rpt ;
      /* Inter-frequency measurement report info 10.3.7.15  */
    l1_inter_sys_meas_rpt_struct_type inter_sys_rpt ;
      /* Inter-RAT measurement report info       10.3.7.26 */
    l1_traf_vol_meas_rpt_struct_type traf_vol_rpt ;
      /* Traffic volume measurement report info  10.3.7.67 */
    l1_qual_meas_rpt_struct_type qual_meas_rpt ;
      /* Quality measurement report info         10.3.7.55 */
    l1_ue_internal_meas_rpt_struct_type ue_internal_meas_rpt;
      /* UE internal measurement report info     10.3.7.76 */
    l1_ue_positioning_measured_results_type ue_pos_meas_results;
      /* UE positioning measured results list    10.3.7.99 */
  } u;

} l1_measured_results_struct_type;



/**** 10.2... ****/

/*
 * Measurement Parameters from SIB_11/12, as in 10.2.48.8.14 and 
 * 10.2.48.8.15 of TS25.331.
 */
/* DJM - THIS DOESN'T seem correct... should pull Measurement Control System Info Struct
 * from 10.3.7.47...
 * and then need Use of HCS enum, Cell selection and reselection quality measure
 * and then all of the measurement information
 *
 * need SIB 12 indicator
 *      10.3.7.8  FACH measurement occasion info
 *      10.3.7.47 Measurement Control System Information
 *         use of HCS (used, not used)
 *         cell selection and reselection quality measure (CPICH Ec/No, CPICH RSCP)
 *         10.3.7.40  Intra-Freq
 *         10.3.7.20  Inter-Freq
 *         10.3.7.31  Inter-RAT
 *         10.3.7.73  Traffic Volume Measurement System Info
 *         10.3.7.81  UE Internal System Measurement Info
 *
 */
typedef struct
{
  boolean       fach_meas_incl;
    /* FACH Measurement occasion info included if yes,10.3.7.8 */

  l1_fach_meas_info_struct_type   fach;
    /* FACH measurement info */
  
  uint32        k_utra;
    /* UTRAN DRX cycle length coefficient, valid only if fach_meas_incl is true */
  
  boolean       hcs_used;
    /* 10.3.7.47  measurement cotrol system information  */
    /* Flag indicating if serving cell belongs to a HCS structure */

  l1_meas_quan_enum_type  cell_sel_resel_quan_type;
    /* Choice of measurement to use as quality measure Q */

  boolean      intra_freq_meas_info_incl;
    /* Flag indicating if intra-frequency measurement info is included */
  l1_intra_freq_sib_struct_type intra_f;
    /* Intra-frequency measurement system information */
  
  /* Inter freq info is removed from SIB part of MEASUREMENT_REQ.
     A new command CPHY_INTER_FREQ_MEAS_REQ will contain the info */
 
  boolean  inter_sys_meas_info_incl;
    /* Flag indicating if inter-system measurement info is included */
  l1_inter_sys_cell_list_struct_type   inter_s;
    /* Inter-system cell information, as defined in 10.3.7.31 */

  /******** Other measurement information. See 10.3.7.72  ******************/
  /********           To be completed                     ******************/

} l1_meas_sib_parm_struct_type;

/* Ref: 3GPP TS 25.331 version 8.8.0 Release 8 - Section: 10.3.7.115  
  Measurement bandwidth information common for all neighbouring cells on the carrier
  frequency. It is defined by the parameter Transmission Bandwidth Configuration, NRB
  [36.104]. The values indicate the number of resource blocks over which the UE could 
  measure. Default value is 6.   
*/
typedef enum
{
  l1_mbw6,
  l1_mbw15,
  l1_mbw25,
  l1_mbw50,
  l1_mbw75,
  l1_mbw100,
  l1_mbw_invalid
} l1_eutra_meas_bandwidth_type;

/* Struct to store the LTE priority info per EARFCN received from SIB19/3rd MEAS_REQ */
typedef struct
{
  /* EARFCN of the downlink carrier frequency */
  uint32 EARFCN; /*Range: (0..4,294,967,295)*/
  /* Priority of the LTE freq */
  int8 priority; /* Range: (0..7) */
  /* Threshold to be used when calculating suitability criteria for that LTE EARFCN */
  int16 q_rxlevmin_eutra; /* Range: 2*(-70 to -22) */
  /* Max threshold value RSRP, dB */
  uint8 threshx_high; /* Range: 2*(0 to 31) */
  /* Min threshold value RSRP, dB */
  uint8 threshx_low;  /* Range: 2*(0 to 31) */
  /* Threshold to be used when calculating suitability criteria for that LTE EARFCN */
  /* as per spec RSRQ, dB default value is negative infinity, L1 should treat 0xFF as default */
  int16 q_qualmin_eutra;
  /* Max threshold value RSRQ, dB */
  boolean high2_incl;
  uint8 threshx_high2; 
  /* Min threshold value RSRQ, dB */
  boolean low2_incl;
  uint8 threshx_low2;  
} l1_inter_rat_eutra_priority_info_type;

/* struct to store the LTE blacklisted cell id's and Bandwidth per EARFCN */
typedef struct
{
  /* EARFCN of the downlink carrier frequency */
  uint32 EARFCN; /* Range: (0..4,294,967,295) */
  /* See l1_eutra_meas_bandwidth_type comments*/
  l1_eutra_meas_bandwidth_type meas_band_width;
  /* Number of blacklisted cell for that LTE EARFCN */
  uint8 num_blacklisted_cells;
  /* List of cell ids that are blacklised from LTE EARFCN */
  uint16 blacklisted_cells[L1_MAX_EUTRA_CELLS_PER_FREQUENCY];
} l1_eutra_frequency_info_type;

/* Main LTE Freq level struct */
typedef struct
{
  /* Number of LTE frequencies recieved from SIB19 */
  uint8 num_eutra_frequencies;
  /* List of LTE frequencies, look at l1_eutra_frequency_info_type for more details */
  l1_eutra_frequency_info_type eutra_frequency[L1_MAX_EUTRA_FREQ];
} l1_meas_eutra_frequency_list_type;

/* Main LTE cell level struct per Freq */
typedef struct
{
  /* EARFCN of the downlink carrier frequency */
  uint32 EARFCN; /*Range: (0..4,294,967,295)*/
  /* Cell id of a cell that is from LTE EARFCN */
  uint16 physical_cell_identity; /*Range 0-503*/
  /* See l1_eutra_meas_bandwidth_type comments*/
  l1_eutra_meas_bandwidth_type band_width;
} l1_eutra_cell_info_type;

/* LTE information passed to RRC in CEL RESEL IND */
typedef struct
{
  /* E-UTRA Cell Reselection Cell Information */
  l1_eutra_cell_info_type    cell_info;    

  /* Cell selection measurement made by SRCH, to be reported to RRC */  
  int16                    rxlev_meas;  
} l1_eutra_cell_resel_ind_info_type;

typedef struct
{
  uint16       num_freq_rmv;
  /* Number of EUTRA freqs removed 0=none, 1..L1_MAX_EUTRA_FREQ */
  /* L1_REMOVE_ALL_FREQ(=0xFF)=remove all */
  uint16  rmv_freq[ L1_MAX_EUTRA_FREQ ];

  l1_meas_eutra_frequency_list_type add_freq_list;
}l1_inter_sys_eutra_freq_list_struct_type;

typedef struct
{
  boolean eutra_freq_list_incl;
  /* E-UTRA freq list info */
  l1_inter_sys_eutra_freq_list_struct_type eutra_freq_list;

  boolean eutra_meas_quantity_incl;
  /* E-UTRA measurement quantity 8.6.7.5 */
  l1_inter_sys_meas_quan_struct_type  meas_quan;

  boolean eutra_rpt_quantity_incl;
  /* E-UTRA measurement quantity 8.6.7.6 */
  l1_inter_sys_dch_rpt_quan_struct_type rpt_quan;

  boolean rpt_cell_status_incl;  
  l1_meas_rpt_cell_status_struct_type  rpt_cell_status; 

  l1_inter_sys_reporting_criteria_struct_type  rpt_criteria;
}l1_inter_sys_eutra_meas_ctrl_struct_type;

/*
 * 10.2.17  Measurement Control Message, CHOICE Measurement Type
 */
typedef struct
{
  l1_meas_type_enum_type  meas_type;
  union
  {
    l1_intra_freq_meas_ctrl_struct_type  intra_f;
    l1_inter_freq_meas_ctrl_struct_type  inter_f;
    l1_inter_sys_meas_ctrl_struct_type   inter_s;
    l1_inter_sys_eutra_meas_ctrl_struct_type   inter_s_eutra;
    l1_ue_pos_meas_ctrl_struct_type      ue_pos;
    l1_traf_vol_meas_ctrl_struct_type    traf_v;
    l1_qual_meas_ctrl_struct_type        qual;
    l1_ue_internal_meas_ctrl_struct_type ue_internal;
  } u;
} l1_meas_ctrl_union_struct_type;


/*
 * 10.2.17  Measurement Control Message
 */
typedef struct
{
  /* If trans_id is L1_NO_CNF_TRANSACTION_ID, RRC doesn't need a confirmation for the MCM sent */
  uint32  trans_id;
    /* transaction id  */

  uint16  meas_id;
    /* Measurement Identity Number */

  l1_meas_cmd_enum_type  meas_cmd;
    /* Measurement Command 10.3.7.46 */

/* DJM - this block is not needed by L1.  Periodic vs/ Event is found below */
/*       in the meas_object.rpt_criteria */
/*       the AM vs UM RLC is an RRC bit of info, L1 doesn't need */
  boolean  rpt_mode_incl;
    /* flag indicating if reporting mode has been included */
    /* If TRUE, then the following variable is valid       */
  l1_meas_reporting_mode_struct_type  reporting_mode;
    /* periodic or event based, AM or UM RLC transfer */

/* --change, intra-freq/inter freq  to make use of the reporting_mode above, 
    remove use of these */
  l1_meas_report_mode_enum_type        rpt_mode;
    /* event based, or periodic */
  l1_meas_report_trans_mode_enum_type  rpt_trans_mode;
    /* measurement transfer mode */

  uint16  num_add_meas;
    /* Number of additional measurements */
  uint16  add_meas[ L1_MAX_CELL_MEAS ];
    /* 10.3.7.48 Measurement Identity for each addl measurement */
  l1_meas_type_enum_type addl_meas_type[L1_MAX_CELL_MEAS ];

  /*  A boolean to check if the following info is included */  
  /*  It is mandatory if the meas_cmd is "setup", optional */
  /*  if it is "modify" and not needed if it is "release"  */
  boolean  meas_object_info_included;
    /* Flag indicating meas_object is included */
  l1_meas_ctrl_union_struct_type  meas_object;
    /* Measurement object  */

  boolean    dpch_cm_status_incl;
    /* DPCH Compressed Mode Status Info included */
  l1_cm_status_info_struct_type  cm_status_info;
    /* DPCH Compressed Mode Status Information */
} l1_meas_ctrl_parm_struct_type;

#define CLEAR_CELL_INFO_LIST   0x01
#define CLEAR_MEAS_IDENTITY    0x02

/* Empty  Measurement Control Message*/
typedef struct
{
  l1_meas_quan_enum_type meas_quan;
  uint8  l1_deferred_meas_from_sib_action;
} l1_deferred_meas_from_sib_struct_type;

/***************************************************************************/
/**                                                                       **/
/**     L1 - RRC   Non-Specification Related Sub-Structure Types          **/
/**                                                                       **/
/***************************************************************************/
/*
 * Frequency Scan Parameters Structures, Raw & Fine
 */
/* Frequency Scan, raw scan parameters */
typedef struct
{
  uint16 start_ctr_freq;
    /* The first center frequency on the frequency band. The value is  */
    /* specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) */
    /* of downlink carrier frequency. */
  uint16 end_ctr_freq;
    /* The last center frequency on the frequency band. The value is  */
    /* specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) */
    /* of downlink carrier frequency.  */
} l1_freq_raw_scan_parm_struct;

/*
 * Frequency Scan, fine scan parameters
 */
typedef struct
{
  uint16 ctr_freq;
    /* Center frequency for fine (200KHZ incremental) scan. The value is  */
    /* specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) */
    /* of downlink carrier frequency. */
} l1_freq_fine_scan_parm_struct;

typedef struct
{
    uint16  freq[L1_MAX_SCAN_REQ]; 
    uint8   plmn_type[L1_MAX_SCAN_REQ]; 
}l1_freq_list_scan_parm_struct;

/*
 * Cell Reselection WCDMA System Information
 */
typedef struct
{
  uint16    freq;
    /* Frequency of the IHO target cell */
  uint16    scr_code;
    /* Scrambling code of the IHO target cell */
} l1_cell_resel_wcdma_info_type;


/*
 * Cell Reselection GSM System Information
 */
typedef struct
{
  l1_gsm_cell_info_type    cell_info;    
    /* GSM Cell Reselection Cell Information */
   
  int16                    rxlev_meas;  
    /* Cell selection measurement made by SRCH, to be reported to RRC */  
} l1_gsm_cell_resel_ind_info_type;

/*
 * Cell Reselection IS2000 Information
 */
typedef struct
{   
  /***** to be completed *****/
    uint16    dummy;
      /* Needs to be filled in when this option is implemented */     
} l1_cell_resel_is2000_info_type;


/* New Cell List type used in state transition meas*/
typedef struct
{
  boolean cell_list_incl;

  l1_intra_freq_cell_list_struct_type cell_list;

} l1_intra_freq_new_cell_list_struct_type; 


/* State Transition (DCH-->FACH) parameters */ 
typedef struct
{
 /* Measurement Id */
  uint16 meas_id;

  /* What to do with this Meas i.e DELETE, STOP */
  l1_meas_trans_enum_type  meas_action;

  /* Meas Type */
  l1_meas_type_enum_type   meas_type; 

  /* flag to indicate if HCS is used */
  boolean hcs_used;

  /* New Cell List obtained from SIB 11/12 after DCH-->FACH transition */
  
  union 
  {
    /* L1_MEAS_STOP_AND_CONFIG_CELL_LIST */
    l1_intra_freq_new_cell_list_struct_type l1_new_intra_freq_cell_list; 
    
    /* L1_MEAS_STOP_AND_OVERWRITE */
    l1_meas_sib_parm_struct_type l1_new_intra_freq_sib_meas_params;

    /* L1_MEAS_STOP_AND_CONFIG_CELL_LIST */
    l1_inter_freq_cell_list_struct_type l1_inter_freq_cell_list;

    /* INTER RAT cell info list */
    l1_inter_sys_cell_list_struct_type   inter_s;

  } u; 

  /* FACH measurement information */
  boolean       fach_meas_incl;
    /* FACH Measurement occasion info included if yes,10.3.7.8 */

  l1_fach_meas_info_struct_type   fach;
    /* FACH measurement info */

} l1_meas_trans_type;


/* intra, inter-freq, inter-rat are supported */
typedef enum
{
  L1_RRC_UTRAN_NMR_INTRA_FREQ,
  L1_RRC_UTRAN_NMR_INTER_FREQ,
  L1_RRC_UTRAN_NMR_IRAT
} l1_rrc_utran_nmr_report_type;

/* cell info parms for intra and inter-freq */
typedef struct
{
  uint16 psc;
  boolean cpich_ec_no_present;
  uint16 cpich_ec_no;
  boolean cpich_rscp_present;
  uint16 cpich_rscp;
  boolean pathloss_present;
  uint16 pathloss;
}l1_meas_utran_nmr_cell_info_type;

/* cell info parms for inter-rat */
typedef struct
{
    boolean gsm_carrier_rssi_present;
    int16 gsm_carrier_rssi;
    l1_bsic_choice_enum_type bsic_choice;
    union
    {
      struct
      {
        uint16  inter_sys_cell_id;
        /* Inter-RAT cell id  spec[0..(maxCellMeas-1)] */
      } bsic_verified;
      /* Verified BSIC Information */
      struct
      {
        W_ARFCN_T bcch_arfcn;
        /* BCCH ARFCN  spec[0..1023] */
      } bsic_not_verified;
      /* Non-Verified BSIC Information */
    } u;
}l1_meas_utran_nmr_irat_cell_info_type;

/* inter-freq */
typedef struct
{
  uint16 num_cells;
  uint16 freq;
  uint16 utra_carrier_rssi;
  l1_meas_utran_nmr_cell_info_type nmr_cell_info[L1_MAX_CELLS_FOR_NMR];
}l1_meas_utran_nmr_info_type;

/* intra-freq */
typedef struct
{
  uint16 num_cells;
  l1_meas_utran_nmr_cell_info_type nmr_cell_info[L1_MAX_CELLS_FOR_NMR];
}l1_meas_utran_nmr_intra_info_type;

/* inter-freq */
typedef struct
{
  uint16 num_freq;
  l1_meas_utran_nmr_info_type l1_meas_utran_nmr_info[L1_MAX_NON_USED_FREQ];
}l1_meas_utran_nmr_inter_info_type;

/* inter-rat */
typedef struct
{
  uint16 num_cells;
  l1_meas_utran_nmr_irat_cell_info_type irat_cell_info[L1_MAX_CELLS_FOR_NMR];
}l1_meas_utran_nmr_irat_info_type;

typedef union
{
  /* UTRAN NMR information for intra frequency */
  l1_meas_utran_nmr_intra_info_type intra_freq_info;
  /* UTRAN NMR information for inter frequency */
  l1_meas_utran_nmr_inter_info_type inter_freq_info;
  /* UTRAN NMR information for IRAT frequency */
  l1_meas_utran_nmr_irat_info_type irat_info;
}l1_meas_utran_nmr_info_union_type;


/*==========================================================================*/
/*=                                                                        =*/
/*=      Commands                                                          =*/
/*=                                                                        =*/
/*==========================================================================*/

/***************************************************************************/
/**                                                                       **/
/**     DEFINITION OF ALL L1 COMMANDS FROM RRC                            **/
/**                                                                       **/
/***************************************************************************/

/*--------------------------------------------------------------------------
          CPHY_CAPABILITY_REQ

Used by RRC to determine Physical Layer and ASIC capabilities.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} l1_cap_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_IDLE_REQ

Used by RRC to command Physical Layer into a known state of Idle with 
no UL or DL physical channels active.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} l1_idle_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_DEACTIVATE_REQ

Used by RRC to command Physical Layer into a Deactivated state.
--------------------------------------------------------------------------*/
typedef struct
{
  sys_stop_mode_reason_e_type  deact_reason;
} l1_deactivate_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_STOP_WCDMA_MODE_REQ

Used by RRC to command Physical Layer into a Stopped state.  Means WCDMA
is no longer the active RAT.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_stop_cause_enum_type  cause;
} l1_stop_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_START_WCDMA_MODE_REQ

Used by RRC to command Physical Layer out of Stopped state and to start and
go idle.  Means WCDMA is now the active RAT.
--------------------------------------------------------------------------*/
typedef struct
{
  wl1_mdsp_image_enum_type  mdsp_image;
  boolean is_ue_in_sglte_mode;
} l1_start_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_SUSPEND_WCDMA_MODE_REQ

Used by RRC to command Physical Layer into a Suspended state.  Means we're
attempting handover or cell reselection to another RAT.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_suspend_resume_cause_enum_type cause;
} l1_suspend_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_RESUME_WCDMA_MODE_REQ

Used by RRC to command Physical Layer out of Suspended state and to resume
in the state it was in earlier.  Means the attempted handover or cell
reselection to another RAT failed, so we're coming back to WCDMA.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_suspend_resume_cause_enum_type cause;
} l1_resume_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_START_QTA_REQ

Indication from RRC that we are to start W2G quick tuneaway process.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
} l1_start_qta_cmd_type;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*--------------------------------------------------------------------------
          CPHY_STOP_QTA_REQ

Indication from RRC that we are to stop W2G quick tuneaway process,
meaning resume WCDMA mode.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} l1_stop_qta_cmd_type;
#endif  /* #ifdef FEATURE_QTA */

/*--------------------------------------------------------------------------
          CPHY_FREQ_SCAN_REQ

Used by RRC to command Layer 1 to perform WCDMA frequency scan on the
specified frequency band and with the specified incremental step.
--------------------------------------------------------------------------*/
/*
 * The frequency scan Step is used as a parameter of a frequency scan command.
 * The scan Step specifies the incremental step used for frequency band scan.
 * Currently, two incremental steps are used -- 2MHz(Raw) and 200KHz(Fine) steps.
 * 
 *     1. Initially, RRC requests 2MHz incremental frequency scan, with Step 
 *        "L1_FREQ_RAW_SCAN", on WCDMA frequency band covering as much as 
 *        60MHz.
 *        
 *     2. Based on "raw" scan results, RRC requests L1 to perform a 200KHz 
 *        fine scan, with "L1_FREQ_FINE_SCAN", on the +/-1 MHz band around 
 *        the center frequency specified by RRC.
 */
typedef struct
{
  l1_freq_scan_step_enum_type  step;
    /* Indicates which type of scan (RAW or FINE or ADDITIONAL CHANNEL)
      will be performed */
  l1_freq_scan_band_enum_type  band;
    /* Indicates the type of band on which additional channels need 
     * to be scanned */
  l1_wcdma_srch_enum_type  srch;
  /* Indicates the search requested; service search, manual plmn or background plmn */

  uint16 num_freq;
  int16 agc;
  
  union
  {
    l1_freq_raw_scan_parm_struct   raw_scan_parm;
      /* Raw Scan Parameters */
    l1_freq_fine_scan_parm_struct  fine_scan_parm;
      /* Fine Scan Parameters */
    l1_freq_list_scan_parm_struct  list_scan_parm; 
  /* List AGC scan parameters*/
  } u;

} l1_freq_scan_cmd_type;
  /* CPHY_FREQ_SCAN_REQ */

#ifdef FEATURE_DUAL_SIM
/*--------------------------------------------------------------------------
          CPHY_DS_STATUS_CHANGE_IND

Used by RRC to indicate Physical Layer whether the UE is in SS/DSDS/DSDA mode

--------------------------------------------------------------------------*/
#ifdef FEATURE_TRIPLE_SIM

typedef enum 
{
  RRC_DUAL_STANBY_MODE_NO_CHANGE,
  RRC_DUAL_STANBY_MODE_SINGLE,
  RRC_DUAL_STANBY_MODE_DUAL,
  RRC_DUAL_STANBY_MODE_TRIPLE
} rrc_multi_sim_mode_type;
#endif

typedef struct
{
  /* TRUE: UE will be operating in dual SIM mode
  ** FALSE: UE will be operating in single SIM mode*/
  boolean dual_standby_mode;
  /* TRUE: UE will be operating in dual active mode
  ** FALSE: UE will be operating in single active mode*/
  boolean is_dual_active;
#ifdef FEATURE_TRIPLE_SIM
    rrc_multi_sim_mode_type multi_sim_mode;
#endif

  /* bmask to indicate
  ** TRM simultaneous capability*/
  uint8 trm_modes;
} l1_ds_status_change_ind_type;
#endif /*FEATURE_DUAL_SIM*/

/*--------------------------------------------------------------------------
          CPHY_ACQ_REQ

Used by RRC to command Physical Layer into Acquisition state and request 
cell search.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_wcdma_acq_mech_type  acq_type;
    /* Indicates the acquisition mechanism to be used, i.e. frequency
     * only or frequency and scrambling code 
     */
  uint16                  freq;
    /* Indicates which RF channel to be scanned. The value is  */
    /* the UTRA Absolute Radio Frequency Channel Number (UARFCN) */
    /* of downlink carrier frequency, as defined in TS25.101. */

  /* the number of individual radio links */
  uint16                  num_rl;

  /* Requested scrambling code.  Can be 0 to (16*511)+15 = 8191. */
  uint16                  scr_code[L1_MAX_RL];
 

  l1_wcdma_acq_mode_type  acq_mode;
    /* Indicates how deep the reference RF channel shall be scanned.*/

  l1_wcdma_srch_enum_type  srch;
  l1_wcdma_srch_enum_type  actual_srch_type; 
  /* Indicates the search requested; service search, manual plmn or background plmn */
  
  /* Indicates if AGC thresh needs to be compared before proceeding
     with acquisition path.  */
  boolean                 apply_agc_thresh;

  uint32                  pn_pos;
    /* If present, PN position of the cell in chipx1 units. */

  boolean                 sttd_indicator_present;
    /* TRUE means "sttd_indicator" value is present.  FALSE otherwise. */

  boolean                 sttd_indicator;
    /* If present, TRUE means the cell is using Space Time Transmit Diversity.
    ** FALSE means it is not.  Described in TS 25.331, 10.3.6.78. */

  boolean                 unbar_freqs;

  /*  The agc threshold value is set if the apply_agc_thresh (= TRUE)
        Otherwise the value is not set and L1 will not look into this value
        in that case, (= -1) indicates L1 shall decide the value
*/

  boolean                 do_acq_split;

} l1_acq_cmd_type;

/* Default value for invalid priority. Actual value range from 0 to 7 */
#define INVALID_PRIORITY -1

/* Struct to store the Serving Cell priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  int8 priority;          /*Range: (0 to 7) */
  uint8 s_prioritysearch1; /*Range: 2*(0 to 31) */
  uint8 s_prioritysearch2; /*Range: (0 to 7), default is 0*/
  uint8 thresh_servinglow; /*Range: (0 to 31)*/
  uint8 thresh_servinglow2; /*Range: (0 to 31) default is 0 */  
} l1_serving_cell_priority_info_type;

/* Struct to store the inter-f priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  uint16 UARFCN;
  int8 priority;     /*Range: (0 to 7) */

  uint8 threshx_high; /*Range: 2*(0 to 31)*/
  uint8 threshx_low; /*Range: 2*(0 to 31)*/
  int16 q_qualmin_fdd; /*Range: -24 to 0, Default : -24 (Spec 25.331, 13.4.15c)*/
  int16 q_rxlevmin_fdd; /*Range: 2*(-60 to -13)+1, Default : -119 (Spec 25.331, 13.4.15c)*/
} l1_inter_freq_priority_info_type;

/* Struct to store the GSM priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  W_ARFCN_T ARFCN;
  int8 priority;       /*Range: (0 to 7) */
  int16 q_rxlevmin_gsm; /*Range: 2*(-58 to -13)+1 */
  uint8 threshx_high;  /*Range: 2*(0 to 31)*/
  uint8 threshxlow;  /*Range: 2*(0 to 31)*/
} l1_inter_rat_gsm_priority_info_type;

/* Struct to store the priority info for all neighbors carried in 3rd MEAS_REQ */
typedef struct
{
  l1_serving_cell_priority_info_type serving_cell_priority;

  boolean inter_freq_priority_info_incl;
  uint8 num_inter_freq_priorities;
  l1_inter_freq_priority_info_type inter_freq_priority[L1_MAX_INTERF_PRIORITY_LAYERS];

  boolean inter_rat_gsm_priority_info_incl;
  uint8 num_gsm_priorities;
  l1_inter_rat_gsm_priority_info_type gsm_priority[L1_MAX_GSM_PRIORITY_LAYERS];

  boolean inter_rat_eutra_priority_info_incl;

  uint8 num_eutra_priorities;
  l1_inter_rat_eutra_priority_info_type eutra_priority[L1_MAX_EUTRA_PRIORITY_LAYERS];

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  /* Including fach measurement indicator as part of feature FE_FACH*/
  boolean fach_meas_indicator_incl;
  rrc_fach_meas_indicator_enum_type fach_meas_indicator;

  /* Introducing a variable which will say whether UE support FE-FACH or not based on the NV 
  If it supports FE-FACH then whether it supports High layers or all layers*/
  rrc_fach_meas_indicator_enum_type  rrc_fe_fach_support;
#endif
  /* Boolean to check for eutra_treselection scaling factor is present*/
  boolean eutra_treselection_scalingfactor_present;
  /*Range (1-16)--Actual value : (val * 0.25)*/
  uint8   eutra_treselection_scalingfactor;
  /* Boolean to check for gsm_treselection scaling factor is present*/
  boolean gsm_treselection_scalingfactor_present;
  /*Range (1-16)--Actual value : (val * 0.25)*/
  uint8   gsm_treselection_scalingfactor;
} l1_meas_priority_info_type;

/* Main parent struct for 3rd MEAS_REQ that carries all SIB-19 parameters */
typedef struct
{
  l1_meas_eutra_frequency_list_type eutra_cell_list;
  l1_meas_priority_info_type priority_info;
} l1_meas_priority_eutra_cell_list_info_type;

/* Support for FEMTO/CSG selection / reselecction */
/* Considering 20 CSG Freqs. RRC will send (1 cell per inter-f and 6 cells on intra-f). 
So in total it will be (1 cell*19 freq) + (6 cells * 1 freq) = 25 */
#define MAX_ASF_CELLS 25
typedef struct
{
  uint16 freq;
  uint16 psc;
  uint32 csg_id;           /* for INTRA-F MACRO cells, RRC will set it to INVALID CSG ID (0xFFFFFFFF) */
  uint32 pn_pos;
  int16  max_tx_pwr;       /* UE_TXPWR_MAX_RACH (from SIB3) */
  int16  q_qualmin;         /* from SIB3 */
  int16  q_rxlevmin;        /* from SIB3 */
  uint32 acq_tot_eng;      /* might be needed for filtering later to be used as prev_filt_eng  */
  uint32 acq_sfn_extended; /* #define DL_BMC_MAX_EXTENDED_SFN 1048576 */
  int16  acq_rx_agc;       /* RxAGC value read during ASF search */
  int16 ecio_2x;              /* EcIo converted from tot_eng */   
  int16 rscp;              /* RSCP calculated from EcIo and RxAGC */ 
} l1_meas_w_csg_cells_type;

typedef struct
{
  uint16  phy_cell_id;         
  uint32  earfcn;      
  int16   rsrp;        
  int16   rsrq;        
  int16   rssi;        

  int32   q_rxlevmin;  
  int8    p_max;      
  int8    q_qualmin;  

  uint32 csg_id;
} l1_meas_lte_csg_cells_type;

typedef struct
{
  sys_radio_access_tech_e_type rat;

    union {
    l1_meas_w_csg_cells_type wcell;
    l1_meas_lte_csg_cells_type lcell;
  } rat_u;

} l1_meas_csg_list_type;

typedef struct
{
  boolean intraf_csg_cells_included; 
  boolean interf_csg_cells_included; 
    uint8 num_csg_cells;
  l1_meas_csg_list_type csg_info_list[MAX_ASF_CELLS];
} l1_meas_csg_cells_info_type;

/*--------------------------------------------------------------------------
          CPHY_CELL_PARM_REQ

Used by RRC to configure the Cell Selection and Reselection Parameters of 
the Serving cell in either Idle Mode or Connected Mode. The information is 
conveyed via SIB3/4 by WCDMA base station.
--------------------------------------------------------------------------*/
/*
 * 10.3.2.3  - Serving cell parameters, mostly related to Cell selection
 *             and reselection, from SIB_3/4.
 */
typedef struct
{
  boolean  map_info_included;
    /* Flag indicating if mapping functionality is needed. */
    /* FALSE indicates no further mapping functionality is needed, or using */
    /* implicit mapping: Q_map = Q_meas_LEV. See section 7 of TS25.304. */
  l1_mapping_info_struct_type  meas_map_info;
    /* Measurement mapping information. It is used when the above flag is  */
    /* TRUE */

  l1_meas_quan_enum_type   quan_type;
    /* Measurement quantity -- Ec/Io or RSCP. Note that, for Cell Selection      */
    /* and Reselection, we only support Ec/Io Measurement Quantity on FDD cells, */
    /* as required by TS25.304. */

  boolean s_intrasrch_incl;
    /* whether s_intrasrch is included */
  int16   s_intrasrch;
    /* Intra-frequency measurement threshold. Value (in dB) ranges (-32..20) */
    /* by step of 2. 0xFFFF indicates the parameter is not applicable. */

  boolean s_intersrch_incl;
    /* whether s_intersrch is included */
  int16   s_intersrch;
    /* Inter-frequency measurement threshold. Value (in dB) ranges (-32..20) */
    /* by step of 2. 0xFFFF indicates the parameter is not applicable. */

  boolean s_srchhcs_incl;
    /* whether s_srchhcs is included */
  int16   s_srchhcs;
    /* HCS measurement threshold. Value (in dB) ranges (-105..91)      */
    /* by step of 2. 0xFFFF indicates the parameter is not applicable. */

  // DDH THIS NOT NEEDED num_rats should be and is expected to be zero if 
  // there are no other rats.
  boolean s_interrat_incl; 
    /* Flag indicating whether info is included for any interrat */
    /* neighbors  */

  uint16  num_rats;
    /* Number of additional RATs included */
  l1_interrat_resel_info_type     interrat_info[L1_MAX_OTHER_RAT];
    /* Inter-RAT reselection info, as defined in 10.3.2.3 of TS25.331 */

  uint16   qhyst1_s;
    /* Hysteresis value of the serving cell for RSCP (?), It is used for     */
    /* calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2. */
  uint16   qhyst2_s;
    /* Hysteresis value of the serving cell for Ec/Io (?), It is used for     */
    /*  calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2. */
    /*  Default value is qhyst1_s. */
  uint16   trselc_ms;
    /* Cell reselection timer, integer (0..31000) in milli seconds */

  boolean hcs_info_incl;
    /* Flag indicating if HCS info below is applicable */
  l1_serv_cell_hcs_info_struct_type  hcs_info;
    /* Serving cell HCS information */

  int16   max_tx_pwr;
    /* Maximum allowed UL TX power, UE_TXPWR_MAX_RACH (dBm) */
  int16   q_qualmin;
    /* Minimum required quality level for Ec/Io measurement (in dB). */
    /* Value ranges (-24..0) */
  int16   q_rxlevmin;
    /* Minimum required received level for RSCP measurement (in dBm). */
    /* Value ranges (-115..-25) by step of 2. */

  uint16   nhcs_n_cr;
    /* maximum number fo cell reselections. Value ranges (1..16). Default */
    /* value is 8. */
  l1_t_cr_max_enum_type   nhcs_t_crmax;
    /* Duration for evaluating alowed amount of cell reselections. */
    /* Default value is 0, which means the parameter is not used. When used, */
    /* the values are (0, 30, 60, 120, 180, 240), (in second). */
  l1_t_cr_max_hyst_enum_type   nhcs_t_crmaxhyst;
    /* Additional time period before UE recert to low-mobility measurement. */
    /* Default value is 0, which means the parameter is not used. When used, */
    /* the value ranges (10..70) by step of 10, (in second). */

  uint16 tresel_scale;
    /* 0 to 10;  coresponds  speed dependent scale factor/10 */
  uint16 tresel_interf_scale ;
    /* 4 to 19. corresponds to interf scale factor / 4*/
  uint16 tresel_irat_scale; 
    /* 4 to 19. corresponds to inter-rat scalefactor/ 4*/

  /* Store the type of currently camped cell (TRUE: FEMTO, FALSE: MACRO) */
  boolean is_camped_cell_femto;

  boolean is_manual_csg_mode;

} l1_cell_parm_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_IMMEDIATE_CELL_EVAL_REQ

Used by RRC to request the Physical Layer to perform a quick evaluation of 
the quality of the intra-frequency cells prior to RACH transmission.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} l1_immediate_cell_eval_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_MEASUREMENT_REQ

Used by RRC to configure L1 measurement parameters, including neighbor 
information, in Idle Mode or Connected Mode. In Idle Mode, the measurement 
parameters are based on SIB_11. In Connected Mode, the measurement parameters 
are based on either SIB_12 (SIB_11 when SIB_12 is absent) or Measurement 
Control Message. 

All sections adhere to 2002-03 version of 25.331
--------------------------------------------------------------------------*/
/* 
 * 10.3.7.47  Measurement Control System Information
 * Measurement Control Parameter Type for both SIB_11/12 and 
 * Measurement Control Message 
 */
typedef struct
{
  l1_meas_ctrl_enum_type parm_type;
  union
  {
    l1_meas_sib_parm_struct_type sib;
    l1_meas_ctrl_parm_struct_type ctrl;
    /* This cmd comes to L1 only when RRC transitions from FACH<-->DCH */
    l1_meas_trans_type trans_meas;
    l1_inter_freq_cell_list_struct_type inter_freq_sib;
   /*L1 to look at this structure only when parm_type is set 
    to L1_DEFERRED_MEASUREMENT_FROM_SIB */
    l1_deferred_meas_from_sib_struct_type deferred_meas_from_sib;
    l1_meas_priority_eutra_cell_list_info_type priority_eutra_cell_list;
    l1_meas_csg_cells_info_type csg_meas;

  } u;
} l1_meas_parm_type;


/*
 * 10.2.17  Measurement Control Message (RRC command)
 */
typedef struct
{
  l1_meas_parm_type *meas_ptr;
    /* Pointer to the parameter storage memory */ 
} l1_meas_cmd_type;


/*--------------------------------------------------------------------------
         CPHY_HARD_HANDOFF_REQ

Used by RRC to request Hard Handoff.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32       tg_freq;
    /* Target Frequency */
  uint32  tg_prim_code;
    /* Primary Scrambling code of Hand-off cell */
  uint32  tg_ovsf_code;

} l1_hard_handoff_req_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_SET_SFN_REQ

Used by RRC to set SFN based on current BCH frame count.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 cur_sfn;
    /* SFN from received BCH frame */
  uint32 ref_sfn;
    /* SFN on reference L1 BCH frame */
} l1_sfn_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_SET_CFN_REQ

Used by RRC to set current CFN.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 frame_offset;
    /* Frame offset in number of frames */
} l1_cfn_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_BCH_SCHEDULE_REQ

Used by RRC to request BCH frames from a certain SFN period.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 st_sfn;
    /* Start SFN */ 
  uint32 sp_sfn;
    /* Stop SFN */
} l1_bch_schedule_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_SIB_SCHED_CHANGE_IND

Used by RRC to request certain frames to be read from the NBCH
--------------------------------------------------------------------------*/

typedef struct 
{
  /* Pointer to an array of 512 bytes (or 4096 bits) to handle all 4096 SFN's.
  ** On N_BCH setup, this bit array identifies SFN's where RRC wants to receive
  ** SIB data.  A 1 bit tells WL1 to receive that frame.  A 0 means WL1 may skip
  ** it.  WL1 can use this info to sleep between SIB frames and conserve power. */
  uint8  *sib_sfn_bit_mask;
} l1_sib_sched_change_ind_type;


  /*please see 10.3.6.36g   HS-DSCH DRX in CELL_FACH information section in 25.331 for more info*/
typedef struct 
{
  /*Determines the time the UE waits until initiating DRX operation, in ms.
  *valid values 100msec, 200msec, 400msec, 800msec
  */
  uint16 t_321;

  /*Determines the length of the DRX Cycle during DRX operation, in frames.
  *valid values 4, 8, 16, 32 frames
  */
  uint8 drx_cycle_length;

  /*Determines the period within the DRX Cycle that the UE continuously 
  *receives HS-DSCH, in frames.Three spare values are needed
  *valid values 1, 2, 4, 8, 16 frames
  */
  uint8 rx_burst;

  /*TRUE means that the DRX operation can be interrupted by HS-DSCH data.
  *FALSE means that the DRX operation cannot be interrupted by HS-DSCH data
  */
  boolean  can_hs_data_interrupt_drx;
  /*  Dedicated hrnti  */
  uint16 dhrnti;
} l1_hs_fach_drx_info_type;


/*--------------------------------------------------------------------------
          CPHY_SETUP_REQ
Used by RRC to send setup commands to L1.
--------------------------------------------------------------------------*/
/* Operation types for physical channel setup */
typedef enum {
  L1_SETUP_OPS_NOOP,
  L1_SETUP_OPS_DROP,
  L1_SETUP_OPS_CFG,
  L1_SETUP_OPS_ADD,
  L1_SETUP_OPS_DPCH_DROP_ADD,
  L1_SETUP_OPS_EN_DIS_TRCH,
  L1_SETUP_OPS_SET_LOOPBACK_MODE
} l1_setup_ops_enum_type;

/* CPHY_SETUP_REQ Sub-primitive mask: */
#define CPHY_UL_PHYCHAN_DROP_INCL      0x0001
#define CPHY_DL_PHYCHAN_DROP_INCL      0x0002
#define CPHY_DL_CCTRCH_INCL            0x0004
#define CPHY_UL_CCTRCH_INCL            0x0008
#define CPHY_DL_PHYCHAN_ADD_INCL       0x0010
#define CPHY_UL_PHYCHAN_ADD_INCL       0x0020
#define CPHY_DL_PHYCHAN_CFG_INCL       0x0040
#define CPHY_UL_PHYCHAN_CFG_INCL       0x0080
#define CPHY_HS_CHAN_CFG_INCL          0x0100

/* If following bitmask is set, it indicates that L1 shall look into 
 * l1_e_dl_info_struct_type present in l1_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for DL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in EUL(E-DCH) state and config
 *   2. When there is only change of UL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define CPHY_E_DL_CHAN_CFG_INCL        0x0200

/* If following bitmask is set, it indicates that L1 shall look into 
 * l1_e_ul_info_struct_type present in l1_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for UL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in EUL(E-DCH) state and config
 *   2. When there is only change of DL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define CPHY_E_UL_CHAN_CFG_INCL        0x0400

/* The following mask is set when an aset_upd_cmd is incl in the setup_msk
 * We drop all the RL channels using this req_mask in drop_cmd_handler */
#define CPHY_R99_RL_DROP_INCL          0x800

/* Req mask for Dropping RL's on HS channel for aset update Operation */
#define CPHY_PREV_HS_CFG_CELL_DROP_INCL               0x01000


#define CPHY_ASET_UPDATE_INCL          0x2000
#define CPHY_DL_SET_LOOPBACK_MODE_INCL 0x4000

  /*All the info needed for DTX operation is included
   * If following bitmask is set, it indicates that L1 shall look into
   * l1_dtx_drx_timing_info_struct_type & l1_dtx_info_struct_type in setup cmd
   * sent by RRC i.e if this bit is NOT set then it indicates NOOP for DTX configuration.
   */
   
   /* Not putting this constant under the feature intentionally as the check might have to 
    be done especially when CPC_DTX is not defined */
#define CPHY_CPC_DTX_CHAN_CFG_INCL 0x8000

  /*All the info needed for DRX operation is included
   * If following bitmask is set, it indicates that L1 shall look into
   * l1_dtx_drx_timing_info_struct_type & l1_drx_info_struct_type in setup cmd
   * sent by RRC i.e if this bit is NOT set then it indicates NOOP for DRX configuration.
   */
#define CPHY_CPC_DRX_CHAN_CFG_INCL 0x10000

/* This bit mask signifies to L1 that HS RACH configuration is included in the setup command
** This is a global req mask and the L1 implicitly takes this as an indication to setup DL F-DPCH
** UL DPCCH, EUL DL and EUL UL. The params are filled in their respective structures*/
#define CPHY_HS_RACH_CHAN_CFG_INCL 0x20000

/* Request mask to track implicit HS drop */
#define CPHY_HS_IMPLICIT_DROP_INCL     0x40000

/*Request mask to track implicit EDL drop */
#define CPHY_EDL_IMPLICIT_DROP_INCL    0x80000

/* The following bit mask is set to indicate that sec fdpch drop is included */
#define CPHY_SEC_DL_PHYCHAN_DROP_INCL    0x100000

/* The following bit mask is set to indicate that sec fdpch is getting added */
#define CPHY_SEC_DL_PHYCHAN_ADD_INCL     0x200000

/* The following bit mask is set to indicate that sec fdpch is getting reconfigured */
#define CPHY_SEC_DL_PHYCHAN_CFG_INCL      0x400000

/* The following bit mask is set to indicate that sec dpcch drop is included */
#define CPHY_SEC_UL_PHYCHAN_DROP_INCL      0x800000

/* The following bit mask is set to indicate that sec dpcch is getting added */
#define CPHY_SEC_UL_PHYCHAN_ADD_INCL      0x1000000

/* The following bit mask is set to indicate that sec dpcch is getting reconfigured */
#define CPHY_SEC_UL_PHYCHAN_CFG_INCL      0x2000000

/* The following bit mask is set to indicate that sec uplink active set update is received */
#define CPHY_SEC_ASET_UPDATE_INCL         0x4000000

/* If following bitmask is set, it indicates that L1 shall look into 
 * l1_e_sec_dl_info_struct_type present in l1_sec_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for DL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in secondary EUL(E-DCH) state and config
 *   2. When there is only change of sec UL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define CPHY_SEC_EUL_DL_CFG_INCL          0x8000000

/* If following bitmask is set, it indicates that L1 shall look into 
 * l1_e_sec_ul_info_struct_type present in l1_sec_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for UL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in sec EUL(E-DCH) state and config
 *   2. When there is only change of sec DL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define CPHY_SEC_EUL_UL_CFG_INCL         0x10000000

#define CPHY_SEC_R99_RL_DROP_INCL             0x20000000

/* The following enum values correspond to the CPHY setup commands
 * defined in the Request Mask field of the CPHY setup command packet.
 * Enum value 0 corresponds to bit position 0, enum value 1 corresponds
 * to bit position 1, and so on...
 */
typedef enum
{
  CPHY_UL_PHYCHAN_DROP_REQ,
  CPHY_DL_PHYCHAN_DROP_REQ,
  CPHY_DL_CCTRCH_REQ,
  CPHY_UL_CCTRCH_REQ,

  CPHY_DL_PHYCHAN_ADD_REQ,
  CPHY_UL_PHYCHAN_ADD_REQ,
  CPHY_DL_PHYCHAN_CFG_REQ,
  CPHY_UL_PHYCHAN_CFG_REQ,

  CPHY_HS_CHAN_CFG_REQ,
  CPHY_E_DL_CHAN_CFG_REQ,
  CPHY_E_UL_CHAN_CFG_REQ,
  CPHY_R99_RL_DROP_REQ,

  CPHY_PREV_HS_CFG_CELL_DROP_REQ,
  CPHY_ASET_UPD_REQ,
  CPHY_DL_SET_LOOPBACK_MODE_REQ,
  CPHY_DTX_CHAN_CFG_REQ,

  CPHY_DRX_CHAN_CFG_REQ,
  CPHY_HS_RACH_CHAN_CFG_REQ,
  CPHY_HS_IMPLICIT_DROP_REQ,
  CPHY_EDL_IMPLICIT_DROP_REQ,

  CPHY_SEC_DL_PHYCHAN_DROP_REQ,
  CPHY_SEC_DL_PHYCHAN_ADD_REQ,
  CPHY_SEC_DL_PHYCHAN_CFG_REQ,
  CPHY_SEC_UL_PHYCHAN_DROP_REQ,

  CPHY_SEC_UL_PHYCHAN_ADD_REQ,
  CPHY_SEC_UL_PHYCHAN_CFG_REQ,
  CPHY_SEC_ASET_UPDATE_REQ,
  CPHY_SEC_EUL_CHAN_DL_CFG_REQ,


  CPHY_SEC_EUL_CHAN_UL_CFG_REQ,
  CPHY_SEC_R99_RL_DROP_REQ,

  CPHY_SETUP_DONE,
  NO_CMD_TO_ISSUE,

  CPHY_SETUP_NUM_CMDS
} cphy_setup_cmd_id_enum_type;

/* Operation types for physical channel setup */
typedef enum {
  HSDPA_SETUP_OPS_NOOP,
  HSDPA_SETUP_OPS_START,
  HSDPA_SETUP_OPS_RECFG,
  HSDPA_SETUP_OPS_STOP,
  HSDPA_SETUP_OPS_HSDPCCH_START,
  HSDPA_SETUP_OPS_HSDPCCH_STOP
} hsdpa_setup_ops_enum_type;

/*--------------------------------------------------------------------------
MACROS FOR DETERMINING WHICH SUB-PRIMITIVES ARE INCLUDED IN A CPHY_SETUP_REQ

Use these macros to determine whether a given sub-primitive is included
in a CPHY_SETUP_REQ.  Pass in the req_mask field from the command packet.
--------------------------------------------------------------------------*/
#define CPHY_SETUP_REQ_IS_DL_CCTRCH_INCL(mask) \
  (((mask) & CPHY_DL_CCTRCH_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_UL_CCTRCH_INCL(mask) \
  (((mask) & CPHY_UL_CCTRCH_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_UL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & CPHY_UL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)
 
#define CPHY_SETUP_REQ_IS_SEC_UL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & CPHY_SEC_UL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & CPHY_DL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & CPHY_DL_PHYCHAN_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_UL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & CPHY_UL_PHYCHAN_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_SEC_UL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & CPHY_SEC_UL_PHYCHAN_CFG_INCL ) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & CPHY_DL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_UL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & CPHY_UL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_SEC_UL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & CPHY_SEC_UL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_HS_CHAN_CONFIG_INCL(mask) \
  (((mask) & CPHY_HS_CHAN_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_EUL_CHAN_DL_CONFIG_INCL(mask) \
  (((mask) & CPHY_E_DL_CHAN_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_EUL_CHAN_UL_CONFIG_INCL(mask) \
  (((mask) & CPHY_E_UL_CHAN_CFG_INCL) ? TRUE : FALSE)

  #define CPHY_SETUP_REQ_E_MASK_IS_EUL_START_INCL(mask) \
  (((mask) & L1_EUL_START) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_E_MASK_IS_EUL_STOP_INCL(mask) \
  (((mask) & L1_EUL_STOP) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_E_MASK_IS_EUL_NOOP(mask) \
  (((mask) == L1_EUL_NOOP) ? TRUE : FALSE)

  #define CPHY_SETUP_REQ_E_MASK_IS_EDL_CFG_INCL(mask) \
  (((mask) & L1_EUL_RECFG_DL) ? TRUE : FALSE)

  #define CPHY_SETUP_REQ_E_MASK_IS_EUL_CFG_INCL(mask) \
  (((mask) & L1_EUL_RECFG_UL) ? TRUE : FALSE)

/*DC HSUPA specific*/
#define CPHY_SETUP_REQ_E_MASK_IS_SEC_EUL_START_INCL(mask) \
  (((mask) & L1_SEC_EUL_START) ? TRUE : FALSE)


#define CPHY_SETUP_REQ_E_MASK_IS_SEC_EUL_STOP_INCL(mask) \
  (((mask) & L1_SEC_EUL_STOP) ? TRUE : FALSE)


#define CPHY_SETUP_REQ_E_MASK_IS_SEC_EUL_NOOP(mask) \
  (((mask) == L1_SEC_EUL_NOOP) ? TRUE : FALSE)


#define CPHY_SETUP_REQ_E_MASK_IS_SEC_EDL_CFG_INCL(mask) \
  (((mask) & L1_SEC_EUL_RECFG_DL) ? TRUE : FALSE)


#define CPHY_SETUP_REQ_E_MASK_IS_SEC_EUL_CFG_INCL(mask) \
  (((mask) & L1_SEC_EUL_RECFG_UL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_SEC_EUL_CHAN_DL_CONFIG_INCL(mask) \
  (((mask) & CPHY_SEC_EUL_DL_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_SEC_EUL_CHAN_UL_CONFIG_INCL(mask) \
  (((mask) & CPHY_SEC_EUL_UL_CFG_INCL) ? TRUE : FALSE)

#define CPHY_E_DL_CHAN_MASK_REQ_IS_OP_INCL(mask, chan, op) \
  (((mask) & L1_E_##chan##_INFO_##op##) ? TRUE : FALSE)

#define CPHY_E_DL_CHAN_MASK_REQ_IS_ANY_OP_INCL(mask, chan) \
  ( (((mask) & L1_E_##chan##_INFO_ADD) || ((mask) & L1_E_##chan##_INFO_RECFG) || ((mask) & L1_E_##chan##_INFO_DROP)) ? TRUE : FALSE)

#define CPHY_E_DL_CHAN_MASK_IS_HICH_RGCH_ADD_RECFG_INCL(e_dl_ch_mask) \
  (e_dl_ch_mask & (L1_E_HICH_INFO_RECFG|L1_E_HICH_INFO_ADD|L1_E_RGCH_INFO_ADD|L1_E_RGCH_INFO_RECFG))

#define CPHY_SETUP_REQ_IS_DL_LOOPBACK_MODE_INCL(mask) \
  (((mask) & CPHY_DL_SET_LOOPBACK_MODE_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_ASET_UPDATE_INCL(mask) \
  (((mask) & CPHY_ASET_UPDATE_INCL) ? TRUE : FALSE)

 
/* Not putting this check under the feature intentionally as the check might have to 
    be done especially when CPC_DTX is not defined */

#define CPHY_SETUP_REQ_IS_CPC_DTX_CHAN_CFG_INCL(mask) \
  (((mask) & CPHY_CPC_DTX_CHAN_CFG_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_CPC_DTX_RECFG_INCL(mask) \
  (((mask) & L1_CPC_DTX_RECONFIG) ? TRUE : FALSE)


#define CPHY_SETUP_REQ_IS_ACT_TIME_INCL(act_time) \
  ((act_time) != L1_ACTIVATION_TIME_NONE)

/* The rf channel mask.  This denotes that the actual RF channel is specified
   in the 14 LSB of the rf_chan field in the CPHY_SETUP_REQ command packet */
#define CPHY_SETUP_REQ_RF_MASK    0x3FFF

#define CPHY_SETUP_REQ_IS_CPC_DRX_CHAN_CFG_INCL(mask) \
  (((mask) & CPHY_CPC_DRX_CHAN_CFG_INCL) ? TRUE : FALSE)  


#define CPHY_SETUP_REQ_IS_HS_RACH_CFG_INCL(mask) \
  (((mask) & CPHY_HS_RACH_CHAN_CFG_INCL) ? TRUE : FALSE)  

#define CPHY_SETUP_REQ_IS_HS_EUL_CPC_CFG_INCL(mask) \
  (mask & (CPHY_HS_CHAN_CFG_INCL|CPHY_E_DL_CHAN_CFG_INCL|CPHY_E_UL_CHAN_CFG_INCL|CPHY_CPC_DRX_CHAN_CFG_INCL|CPHY_CPC_DTX_CHAN_CFG_INCL))

#define CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & CPHY_SEC_DL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)
  
#define CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & CPHY_SEC_DL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)

#define CPHY_SETUP_REQ_IS_SEC_ASET_UPDATE_INCL(mask) \
  (((mask) & CPHY_SEC_ASET_UPDATE_INCL) ? TRUE : FALSE)

  #define CPHY_SETUP_REQ_IS_SEC_DL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & CPHY_SEC_DL_PHYCHAN_CFG_INCL) ? TRUE : FALSE)
/* --------------------------- */
/* HSDPA related configuration */
/* --------------------------- */

#define CPHY_HS_DSCH_CFG_INCL          0x0001
#define CPHY_HS_PDSCH_RL_CFG_INCL      0x0002
#define CPHY_HS_SCCH_CFG_INCL          0x0004
#define CPHY_HS_DPCCH_CFG_INCL         0x0008
#define CPHY_HS_MIMO_CFG_INCL          0x0020
#define CPHY_HS_SEC_HSDPA_CFG_INCL     0x0040
#define CPHY_HS_E_FACH_CFG_INCL        0x0080
#define CPHY_HS_E_PCH_CFG_INCL         0x0100
#define CPHY_HS_E_FACH_DRX_CFG_INCL    0x0200

#define CPHY_HS_E_FACH_DRX_CFG_ALL_INCL \
  (CPHY_HS_E_FACH_CFG_INCL |            \
   CPHY_HS_DSCH_CFG_INCL |              \
   CPHY_HS_PDSCH_RL_CFG_INCL |          \
   CPHY_HS_SCCH_CFG_INCL |              \
   CPHY_HS_E_FACH_DRX_CFG_INCL)

/* For ENH FACH/PCH state feature we dont have UL Chan HS-DPCCH, 
 * So only valid hs_req_mask are below.
 */
#define CPHY_HS_E_FACH_CFG_ALL_INCL \
  (CPHY_HS_E_FACH_CFG_INCL | \
   CPHY_HS_DSCH_CFG_INCL | \
   CPHY_HS_PDSCH_RL_CFG_INCL | \
   CPHY_HS_SCCH_CFG_INCL)

#define CPHY_HS_CFG_ALL_INCL \
  (CPHY_HS_DSCH_CFG_INCL | \
   CPHY_HS_PDSCH_RL_CFG_INCL | \
   CPHY_HS_SCCH_CFG_INCL | \
   CPHY_HS_DPCCH_CFG_INCL)
   
#define CPHY_HS_CFG_REQ_IS_CFG_INCL(mask, cfg) \
  (((mask) & CPHY_HS_##cfg##_CFG_INCL) ? TRUE : FALSE)

#define CPHY_HS_CFG_REQ_IS_E_FACH_ONLY(mask) \
  ((((mask) & CPHY_HS_E_FACH_CFG_INCL) && !((mask) & CPHY_HS_DPCCH_CFG_INCL)) ? TRUE : FALSE)

/* Action will outline the action related to MIMO Operation */
typedef enum
{
  /* specifies NO operation for MIMO*/
  L1_MIMO_NOOP,                  
  /* specifies START of MIMO operation */
  L1_MIMO_START,
  /* specifies Reconfiguration of MIMO operation */
  L1_MIMO_RECFG,
  /* specifies STOP of MIMO operation */
  L1_MIMO_STOP
}l1_dl_hs_mimo_action_enum_type;

/* MIMO Second cpich patter choice type enum*/
typedef enum
{
  MIMO_NORMAL_PATTERN,
  MIMO_DIVERSITY_PATTERN,
  MIMO_INVALID_PILOT_PATTERN
}l1_dl_hs_mimo_second_cpich_patter_choice_enum_type; 

/* MIMO N to M Ratio type*/
typedef enum
{
  MIMO_N_TO_M_RATIO_1_1 = 0,
  MIMO_N_TO_M_RATIO_1_2,
  MIMO_N_TO_M_RATIO_2_3,
  MIMO_N_TO_M_RATIO_3_4,
  MIMO_N_TO_M_RATIO_4_5,
  MIMO_N_TO_M_RATIO_5_6,
  MIMO_N_TO_M_RATIO_6_7,
  MIMO_N_TO_M_RATIO_7_8,
  MIMO_N_TO_M_RATIO_8_9,
  MIMO_N_TO_M_RATIO_9_10,
  MIMO_N_TO_M_RATIO_INVALID
}l1_dl_hs_mimo_n_m_ratio_enum_type;

/* This structure defines the HSDPA MIMO information */
typedef struct
{
  /* This Action will outline the action related to MIMO */
  l1_dl_hs_mimo_action_enum_type l1_mimo_action;
  
  /* This specifies N to M ratio related to MIMO */
  l1_dl_hs_mimo_n_m_ratio_enum_type mimo_n_m_ratio;
  
  /* MIMO Second cpich patter choice type enum*/
  l1_dl_hs_mimo_second_cpich_patter_choice_enum_type second_cpich_pattern;
  
  /* Channelisation code is valid for second_cpich_pattern if value is MIMO_DIVERSITY_PATTERN 
     its an Integer Value 0..255 */
  uint8 channelisation_code ;
  
  /*Power Offset for S-CPICH for MIMO*/
  int8 s_cpich_po;

  /* Whether to apply Precoding Weight Set Restriction on MIMO or not: Default is FALSE*/
  boolean mimo_codebook_restriction_present;
}l1_dl_hs_mimo_info_struct_type;

/* Enumeration for action type for DC-HSDPA */
typedef enum
{
  /* specifies NO operation for secondary carrier*/
  L1_SEC_HSDPA_NOOP,                  
  /* specifies START of secondary carrier operation */
  L1_SEC_HSDPA_START,
  /* specifies Reconfiguration of secondary carrier operation */
  L1_SEC_HSDPA_RECFG,
  /* specifies STOP of secondary carrier operation */
  L1_SEC_HSDPA_STOP
} l1_hsdpa_sec_hsdpa_action_enum_type;


/* Structure type declaration */
/* -------------------------- */

/* Structures for CCTrCh setup informtion */
/** Total number of CTFC entries per table in mDSP */
#define DEC_TOTAL_CTFC_ENTRIES_PER_TABLE 128

/* This enum defines the PDSCH spreading factor type. The enum values are
   same as in the RRC IEs and denotes the spreading factor values */
typedef enum
{
  L1_DL_SF_4,     /* Spreading factor is 4 */
  L1_DL_SF_8,     /* Spreading factor is 8 */
  L1_DL_SF_16,    /* Spreading factor is 16 */
  L1_DL_SF_32,    /* Spreading factor is 32 */
  L1_DL_SF_64,    /* Spreading factor is 64 */
  L1_DL_SF_128,   /* Spreading factor is 128 */
  L1_DL_SF_256,   /* Spreading factor is 256 */
  L1_DL_SF_512    /* Spreading factor is 512 */
} mcalwcdma_dec_sf_enum_type;

/** This structure defines the CTFC or TFC entry information members */
typedef struct
{
  /** Is this TFC is valid or not */
  boolean valid_ctfc;

  /* PDSCH OVSF code. */
  uint16             pdsch_ovsf_code;

  /* PDSCH spreadind factor. */
  mcalwcdma_dec_sf_enum_type pdsch_spreading_factor;

  /* PDSCH multicode value. */
  uint8              pdsch_multicode_value;

  /** pointer to transport format index of each TrCh in CCTrCh array */
  uint8 *tfi_table;
} mcalwcdma_dec_ctfc_entry_info_struct_type;

/** This structure defines transport format confinition set information */
typedef struct
{
  /** Is any TFCS exists for this CCTrCh */
  boolean tfci_existence;

  /* Is the PDSCH information valid (there is a DSCH TrCh) */
  boolean pdsch_info_valid;

  /** last TFC index */
  int16  last_ctfc_entry_index;

  /** Pointer to the array of TFC information */
  mcalwcdma_dec_ctfc_entry_info_struct_type *ctfc_entry_array[DEC_TOTAL_CTFC_ENTRIES_PER_TABLE];
} mcalwcdma_dec_ctfc_info_struct_type;

typedef mcalwcdma_dec_sf_enum_type    l1_dl_sf_enum_type;

/* This structure defines the CTFC or TFC entry information members. */
typedef mcalwcdma_dec_ctfc_entry_info_struct_type  l1_dl_ctfc_entry_info_struct_type;

/* This structure defines the CTFC set or TFCS information members. */
typedef mcalwcdma_dec_ctfc_info_struct_type  l1_dl_ctfc_info_struct_type;

/* Number of channelisation codes for SCCH in HS PDSCH */
#define L1_DL_HS_SCCH_CODES_MAX 4

/* This structure defines the DL HS DPSCH SCCH information */
typedef struct
{
  /* indicate if secondary scrambling code exists for HS SCCH channel */
  boolean sec_scr_code_valid;
  /* secondary scrambling code value.
     Valid only if sec_scr_code_valid is set to TRUE.
     If not valid then primary CPICH scrambling code is used */
  uint8 sec_scr_code;

  /* number of channelisation codes to SCCH */
  uint8 num_ch_code;
  /* SCCH channelisation codes */
  uint8 ch_code[L1_DL_HS_SCCH_CODES_MAX];
} l1_dl_hs_scch_info_struct_type;

typedef struct
{
  /* DC-HSDPA action type */
  l1_hsdpa_sec_hsdpa_action_enum_type sec_hsdpa_action;


  /* H-RNTI, this is 16 bits in length */
  uint16 h_rnti_sec_cell;

  boolean hs_sec_cell_64_qam_configured;

  /* Indicates if the HS-DSCh TB table to be chosen is octet aligned or not*/
  l1_dl_hs_dsch_tb_size_alignment_enum_type     hs_sec_cell_tb_size_alignment_type;

  /* primary scrambling code of the CPICH */
  uint16 pri_cpich_scr_code;

  /* L1 DL HS SCCH physical channel info */
  l1_dl_hs_scch_info_struct_type hs_sec_cell_scch_info;
    
  /* Power offset between CPICH and HS PDSCH
     Range is from -6 to 13 in units of 0.5 dB
     Values ranges from -12 to 26 */
  int8 po_hs_dsch_sec_cell;

  /* DC-HSDPA secondary cell frequency */
  uint16 hs_sec_cell_dl_freq;

  /* TRUE indicates that the UE uses the Tx diversity mode 
  configuration on secondary carrier different from primary carrier. */
  boolean diff_txd_mode;

  /* SCCH_ORDER_ACTIVATION status, TRUE: ACTIVATE, FALSE: DEACTIVATE */
  /* default value is TRUE */
  boolean activation_status;
}l1_secondary_cell_info_type;


/* MAX common E-DCH resource list. This list has the set of parameters for specific
** dedicated channels based on the common EDCH resource index*/
#define L1_MAX_E_DCH_COMMON_LIST 32

/* used in RRC only */
#define L1_SG_STEP_THRESHOLD_NOT_PRESENT 0

/* 25.331, Section 10.3.10. FFS: To be moved to correct place. */
#define L1_MAX_EDCH_RL                 L1_ASET_MAX

#define L1_SERVING_GRANT_NOT_PRESENT 0xFF

/* e_req_mask bit positions/value */

/* No operation needed for E_DL and E_UL*/
#define  L1_EUL_NOOP                 0x00

/* START: Indicates that its the first time EUL channels are setup. RRC should 
   indicate all the necessary UL/DL configuration. 
   RRC will set START when atleast one of the following is met:
     1. when EUL is setup for the first time
     2. CphyEstablishmentFailure - reverting back to old config (E-DCH->DCH)
     3. CphySetupFailure(functional failure) - reverting back to old config (E-DCH->DCH) */
#define  L1_EUL_START                0x01

/* RECFG_UL: If EUL is up, RRC will set bit L1_HSUPA_RECFG_UL whenever there
   is any change in E-UL config.
   RRC will set RECFG-UL when atleast one of the following is met:
     1. Atleast one of E-UL parameter is modified (parameters include TTI)
     2. CphyEstablishmentFailure - reverting back to old config (E-DCH->E-DCH)
     3. CphySetupFailure(functional failure) - reverting back to old config (E-DCH->E-DCH) */
#define  L1_EUL_RECFG_UL             0x02

/* RECFG_DL: If EUL is up, RRC will set bit L1_HSUPA_RECFG_DL whenever there
   is any change in E-DL config. 
   RRC will set RECFG-DL when atleast one of the following is met:
     1. Whenever any E-DCH active set RL goes out of E-DCH active set,
     2. New RL gets added to E-DCH active set
     3. Simultaneous add and remove of RL to E-DCH active set
     4. Change of E-DCH Serving RL
     5. CphyEstablishmentFailure - reverting back to old config (E-DCH->E-DCH)
     6. CphySetupFailure(functional failure) - reverting to old config (E-DCH->E-DCH), 
        when config message results in deletion of one or more E-DCH active set RL.
     7. Whenever E-DCH active set RL is removed from Active Set 
     8. Whenever E-channel(AGCH/RGCH/HICH) parameters associated with E-DCH
        active set RL is modified
     9. Whenever any of the E-channels associated with E-DCH Active Set RL 
        is added/removed
     10. TTI reconfiguration
     11. (P/S)E-RNTI is added or modified

    NOTE: When E-HICH is removed from any of E-DCH active set RL and/or
          E-DCH active set RL is removed from the active set, then 
          E-DCH RL info for that RL(s) will be removed from e_rl_info[L1_MAX_EDCH_RL] 
          and added in e_rl_release_info[] */
#define  L1_EUL_RECFG_DL             0x04

/* STOP: Layer1 is expected to release/stop all E-DCH Channels. 
   RRC will set STOP whenever one of following is met:
     1. UE moves from E-DCH to non-DCH(FACH/Cell-PCH/URA-PCH) state
     2. NW explicitly indicates UE to stop E-DCH
     3. CphyEstablishmentFailure: reverting back to old config (DCH->E-DCH) */
#define  L1_EUL_STOP                 0x08

/*This macro gives the bits that are valid for EUL check in a uint8*/
#define L1_EUL_MASK                  0x0F

/* For DC-HSUPA the bitmasks used will be the same as for SC case. 
** where understanding is same as  above.*/
#define  L1_SEC_EUL_NOOP                 0x00
#define  L1_SEC_EUL_START                0x01
#define  L1_SEC_EUL_RECFG_UL             0x02
#define  L1_SEC_EUL_RECFG_DL             0x04
#define  L1_SEC_EUL_STOP                 0x08
#define  L1_SEC_EUL_MASK                 0x0F

/* e_dl_channel_mask bit masks - USED BY RRC ONLY,
   L1 should not look into these bits. */
#define L1_E_HICH_INFO_PRESENT           0x8000
#define L1_E_RGCH_INFO_PRESENT           0x4000
#define L1_E_AGCH_INFO_PRESENT           0x2000

/* e_dl_channel_mask bit position definition */
#define L1_E_AGCH_INFO_ADD             0x0001
#define L1_E_RGCH_INFO_ADD             0x0002
#define L1_E_HICH_INFO_ADD             0x0004

#define L1_E_AGCH_INFO_RECFG           0x0008
#define L1_E_RGCH_INFO_RECFG           0x0010
#define L1_E_HICH_INFO_RECFG           0x0020

#define L1_E_AGCH_INFO_DROP            0x0040
#define L1_E_RGCH_INFO_DROP            0x0080
/* HICH-DROP is redundant: Whenever HICH is released then e_rl info will be removed
   from e_rl_add_recfg_info[] and psc will be included in e_rl_release_psc[]  */
#define L1_E_HICH_INFO_DROP            0x0100

#define L1_MAX_REF_E_TFCI_LIST         8

typedef enum
{
  L1_E_GRANT_SELECTOR_NOT_PRESENT,
  L1_E_PRIMARY_GRANT,
  L1_E_SECONDARY_GRANT
} l1_e_pri_sec_grant_selector_enum_type;

/* Enumeration for Beta_ed,k,reduced,min IE in the Release8 OTA */
typedef enum
{
  L1_E_MIN_BED_08, /*  8/15 */
  L1_E_MIN_BED_11, /* 11/15 */
  L1_E_MIN_BED_15, /* 15/15 */
  L1_E_MIN_BED_21, /* 21/15 */
  L1_E_MIN_BED_30, /* 30/15 */
  L1_E_MIN_BED_42, /* 42/15 */
  L1_E_MIN_BED_60, /* 60/15 */
  L1_E_MIN_BED_84, /* 84/15 */
  NUM_L1_E_MIN_BED /* invalid */
} l1_e_min_bed_enum_type;

/* SF_128 and SF_256 is internal to Layer1 only and is not signalled by NW. 
    Ref: 10.3.6.99 in 25.331*/
typedef enum {
  L1_E_MAX_CH_CODES_SF4X2_SF2X2_16QAM,
  L1_E_MAX_CH_CODES_SF4X2_SF2X2,
  L1_E_MAX_CH_CODES_SF2X2,
  L1_E_MAX_CH_CODES_SF4X2,
  L1_E_MAX_CH_CODES_SF_4,
  L1_E_MAX_CH_CODES_SF_8,
  L1_E_MAX_CH_CODES_SF_16,
  L1_E_MAX_CH_CODES_SF_32,
  L1_E_MAX_CH_CODES_SF_64,
  L1_E_MAX_CH_CODES_SF_128,
  L1_E_MAX_CH_CODES_SF_256,
  L1_NUM_L1_E_MAX_CH_CODES_SF
} l1_e_max_ch_codes_enum_type;

/* HARQ RV configuration: Ref: 10.3.5.7d in 25.331*/
typedef enum
{
  L1_RV_NOT_PRESENT, /* initialized with this value */
  L1_RV_TABLE, /* use RSN based RV index */
  L1_RV_0 /* UE shall only use RV index 0 */
} l1_harq_rv_config_enum_type;

typedef struct
{
  /* AGCH channelization code; Ref: 10.3.6.100 in 25.331*/
  uint8 ch_code; /* 0..255 */
} l1_e_agch_info_struct_type;

typedef struct
{
  /* RG combination index; Ref: 10.3.6.100 in 25.331*/
  uint8 rg_idx; /* 0..5 */

  /* Signature Sequence; Ref: 10.3.6.100 in 25.331*/
  uint8 signature_idx; /* 0..39 */
} l1_e_rgch_info_struct_type;

typedef struct
{
  /* HICH channelization code; Ref: 10.3.6.101 in 25.331*/
  uint8 ch_code; /* 0..127 */

  /* Signature Sequence index; Ref: 10.3.6.101 in 25.331*/
  uint8 signature_idx; /* 0..39 */
} l1_e_hich_info_struct_type;

typedef struct
{
/* e_dl_channel_mask bit position definition 
#define L1_E_AGCH_INFO_ADD             0x0001
#define L1_E_RGCH_INFO_ADD             0x0002
#define L1_E_HICH_INFO_ADD             0x0004

#define L1_E_AGCH_INFO_RECFG           0x0008
#define L1_E_RGCH_INFO_RECFG           0x0010
#define L1_E_HICH_INFO_RECFG           0x0020

#define L1_E_AGCH_INFO_DROP            0x0040
#define L1_E_RGCH_INFO_DROP            0x0080
HICH-DROP is redundant: Whenever HICH is released then e_rl info will be removed
from e_rl_add_recfg_info[] and psc will be included in e_rl_release_psc[]
#define L1_E_HICH_INFO_DROP            0x0100*/
  uint16 e_dl_channel_mask;

  /* primary scrambling code of this RL */
  uint16 e_rl_psc;

  uint8 tpc_idx; /* 0..5 */

  l1_e_agch_info_struct_type e_agch_info;
  l1_e_rgch_info_struct_type e_rgch_info;
  l1_e_hich_info_struct_type e_hich_info;
} l1_e_dl_per_rl_info_struct_type;

typedef struct
{
  /* psc of E-DCH active set RL for which E-HICH is released or
     psc of E-DCH active set RL removed from the active set */
  uint16 psc;
} l1_e_rl_release_info_struct_type;

/*serving Grant Info*/
typedef struct
{

  /* 0..37, 38 indicates zero grant.
     L1_SERVING_GRANT_NOT_PRESENT(0xFF) indicates that it is NOT provided by NW 
     Ref: 10.3.6.69a in 25.331*/
  uint8 serving_grant;   /* RRC will not provide this value for secondary carrier. L1 should be able to take this value from primary eul */

  /* Following refers to the index in the SG Table */
  uint8 three_index_step_threshold; /* 0..37 */ /*For secondary eul RRC will not provide this value*/

  /* Following refers to the index in the SG Table */
  uint8 two_index_step_threshold; /* 0..37 */ /*For secondary eul RRC will not provide this value */

  l1_e_pri_sec_grant_selector_enum_type grant_selector;

} l1_e_grant_config_info_struct_type;

typedef struct
{
  boolean primary_e_rnti_present;
  boolean secondary_e_rnti_present; 

  uint16 primary_e_rnti;
  uint16 secondary_e_rnti;

  e_tti_enum_type e_tti;

  l1_e_grant_config_info_struct_type grant_config;

  /* Number of E-RL psc's to be released */
  uint8 num_e_rl_release_info;

  l1_e_rl_release_info_struct_type e_rl_release_info[L1_MAX_EDCH_RL]; /* psc of E-RLs to be released */

  /* Serving RL index in array e_dl_rl_add_recfg_info[L1_MAX_EDCH_RL] */
  uint8  e_serving_rl_idx; /* 0..(L1_MAX_EDCH_RL-1) */

  /* number of enteries valid in array e_rl_info[L1_MAX_EDCH_RL] */
  uint8 num_e_dl_rl_add_recfg_info;
  
  l1_e_dl_per_rl_info_struct_type e_dl_rl_add_recfg_info[L1_MAX_EDCH_RL];

} l1_e_dl_info_struct_type;

typedef struct
{

  /* reference E-TFCI */
  uint8 ref_e_tfci; /* 0..127 */
  uint8 ref_e_tfci_pwr_offset; /* 0..31 */
} l1_ref_e_tfci_struct_type;

typedef struct
{
   /* E-TFCI table index: Indicates which standardised E-TFCI TB size table
  ** shall be used for a given e_tti */
  uint8 e_tfci_table_idx; /* 0..1, 2..3 [16QAM] */

  uint8 num_ref_e_tfci;

  /* Reference E-TFCIs */
  l1_ref_e_tfci_struct_type ref_e_tfci_list[L1_MAX_REF_E_TFCI_LIST];

  /* Beta_ed,k,reduced,min IE in the Release8 OTA - enumerated
  ** If the OTA is Release7/6, or if UE (NV) is Release 7/6, RRC sets an 
  ** invalid value which indicates L1 to use Release7/6 implementation */
  l1_e_min_bed_enum_type min_beta_ed;

  l1_e_max_ch_codes_enum_type e_max_ch_codes;

  /* RRC will convert (11..25) received from NW using (IE value * 4), 
     so range of e_dpdch_pl_non_max is (44..100) */
  uint8 e_dpdch_pl_non_max; /* 44..100 */

} l1_e_dpdch_info_struct_type;

typedef struct
{
  /* Can take values from 0 to 127.
  ** ETFCI_BOOST_VAL to set to 127 to disable BOOST */
  uint8 e_tfci_boost_val;

  /* takes values 0-6. If e_tfci_boost_val is set 127, 
  ** then this variable should not be looked into*/
  uint8 delta_t2tp;
} l1_e_tfc_boost_info_struct_type;

typedef struct
{
  uint8 e_dpcch_pwr_offset; /* 0..8 */

  /* as per OTA message this info is in edpcch_struct.
  TRUE if EDPDCH power interpolation is configured. FALSE 
  by default or if extrapolation is configured */
  boolean e_dpdch_pwr_interpolation; 

  l1_e_tfc_boost_info_struct_type e_tfc_boost_info;
} l1_e_dpcch_info_struct_type;

typedef struct
{
  e_tti_enum_type e_tti;

  /* 25.331, 10.3.5.7d:
     If "rv0" is indicated, the UE shall only use E_DCH RV index 0. 
     Default value is "rvtable", in which case the UE shall use an RSN based
     RV index. */
  l1_harq_rv_config_enum_type harq_rv_config;

  /* Action that indicates if UL 16 QAM needs to be started or stopped */
  boolean e_ul_16qam_action;
  
  /* the value of e_ul_16qam_e_agch_table_idx can be either 0 or 1.
  ** This indicates which AGCH Table needs to be selected.
  ** This variable need to be looked into only when UL 16 QAM is configured */
  uint8 e_ul_16qam_e_agch_table_idx;
  l1_e_dpdch_info_struct_type e_dpdch_info;

  l1_e_dpcch_info_struct_type e_dpcch_info;

} l1_e_ul_info_struct_type;

/* These are the new parameters that are received for sec eul uplink from the OTA. 
     Any other paraneters required to establish secondary eul are common for both primary
     and secondary carrier */

typedef struct
{

l1_ul_dpch_sc_enum_type  scrambling_code_type;
/*  Scrambling Code value (0..16777215) */
uint32                   scrambling_code; 

/* DPCCH power offset in dB (0 .. 7 in steps of 1) */
uint8                    dpcch_pwr_offset;

/* Power Control Preamble length - between 0 and 7 - type to allow transition
   to R'99 2001-06, per sec. 10.3.6.91 in 25.331. */
uint8 pcp_len;

  
}l1_e_sec_dpcch_info_struct_type;


typedef struct
{
  /* Beta_ed,k,reduced,min IE in the Release8 OTA - enumerated
  ** If the OTA is Release7/6, or if UE (NV) is Release 7/6, RRC sets an 
  ** invalid value which indicates L1 to use Release7/6 implementation */
  l1_e_min_bed_enum_type e_min_beta_ed;  

  l1_e_sec_dpcch_info_struct_type dpcch_info;

} l1_e_sec_ul_info_struct_type;

typedef struct
{

  l1_e_ul_info_struct_type e_ul_info;

  l1_e_dl_info_struct_type e_dl_info;

} l1_e_info_struct_type;

typedef struct
{

  l1_e_sec_ul_info_struct_type  e_ul_info;

  l1_e_dl_info_struct_type      e_dl_info;

} l1_sec_e_info_struct_type;

/* This is the structure type in which the RRC would fill in the information 
** for EUL DL information*/
typedef struct
{
  boolean primary_e_rnti_present;
  uint16 primary_e_rnti;

  e_tti_enum_type e_tti;

   /* The grant selector will always be primary grant -- for HS RACH*/
  l1_e_grant_config_info_struct_type grant_config;

  l1_e_agch_info_struct_type e_agch_info;

} l1_e_dl_hs_rach_info_struct_type;

/* This is the structure type in which RRC would be filling the EUL UL and DL
** information for HS RACH. */
typedef struct
{

  l1_e_ul_info_struct_type e_ul_info;

  l1_e_dl_hs_rach_info_struct_type e_dl_info;
  
  /* This is a structure which L1 will be populating from the RRC populated values in
  ** e_dl_info (above). The L1 channel setup code would be used this below information.
  ** this is done to allow the channel setup code to be common for legacy and HS RACH EDL*/
  l1_e_dl_info_struct_type e_dl_info_for_config;
 /* Indicates L1 to flush all HARQs.
    RRC will set this to TRUE under following conditions:
      - NW explicitly indicate in the reconfig message
      - whenever TTI reconfiguration happens 

    NOTE: L1 is expected to look into this variable ir-respective of the value
          of 'e_req_mask'. */
   boolean mac_hsrach_e_reset_indicator;
} l1_e_hs_rach_info_struct_type;


  /* This request mask will outline the action related to DRX*/
  /* bitmask:be set to l1_cpc_drx_req_mask:*/
#define L1_CPC_DRX_NO_OP                      0x00

#define L1_CPC_DRX_START                      0x01

#define L1_CPC_DRX_RECONFIG                   0x02

#define L1_CPC_DRX_STOP                       0x04

  /*It gives the bits that are valid for DRX check in a uint8*/
#define L1_CPC_DRX_MASK                  0x0F


/* req mask for DTX-DRX configuration */
/* This request mask will outline the action related to DTX*/
/* bitmaskwill be set to l1_cpc_dtx_req_mask:*/
#define  L1_CPC_DTX_NO_OP                      0x00

#define  L1_CPC_DTX_START                      0x01

#define  L1_CPC_DTX_RECONFIG                   0x02

#define  L1_CPC_DTX_STOP                       0x04

/*It gives the bits that are valid for DTX check in a uint8*/
#define L1_CPC_DTX_MASK                  0x0F

/* DTX-DRX timing information: Ref: 10.3.6.34b in 25.331*/

/*Enabling Delay, values in radio frames,as per RRC spec 25.311
  *V7.8.0 (2008-03), below are the valid range of values
  */
typedef enum
{
  /* Units in radio frames
   *Valid range Num radio frames: 0, 1, 2, 4, 8, 16, 32, 64, 128
  */
  L1_NUM_RADIO_FRAMES_0,
  L1_NUM_RADIO_FRAMES_1,
  L1_NUM_RADIO_FRAMES_2,
  L1_NUM_RADIO_FRAMES_4,
  L1_NUM_RADIO_FRAMES_8,
  L1_NUM_RADIO_FRAMES_16,
  L1_NUM_RADIO_FRAMES_32,
  L1_NUM_RADIO_FRAMES_64,
  L1_NUM_RADIO_FRAMES_128,
  L1_NUM_RADIO_FRAMES_MAX
}l1_dtx_drx_enabling_delay_info_enum_type;

/*UE_DTX_ cycle1 :- Uplink DPCCH burst pattern length*/
typedef enum
{
  /* Units of subframes.
   sub frames 1, 5, 10, 20 - are valid for 10msec TTI
   sub frames 1, 4, 5, 8, 10, 16, 20 - are valid for 2msec TTI
  */
  L1_UE_DTX_CYCLE1_NUM_SUBFM_1 = 1,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_4 = 4,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_5 = 5,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_8 = 8,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_10 = 10,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_16 = 16,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_20 = 20,
  L1_UE_DTX_CYCLE1_NUM_SUBFM_MAX
}l1_ue_dtx_cycle1_info_enum_type;

/*UE_DTX_ cycle2 :- Uplink DPCCH burst pattern length*/
typedef enum
{
  /* Units of subframes.
   *sub frames 5, 10, 20, 40, 80, 160 - are valid for 10msec TTI
   *sub frames 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160 - are valid for 2msec TTI
  */
  L1_UE_DTX_CYCLE2_NUM_SUBFM_4 = 4,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_5 = 5,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_8 = 8,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_10 = 10,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_16 = 16,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_20 = 20,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_32 = 32,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_40 = 40,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_64 = 64,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_80 = 80,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_128 = 128,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_160 = 160,
  L1_UE_DTX_CYCLE2_NUM_SUBFM_MAX
}l1_ue_dtx_cycle2_info_enum_type;

  /*MAC_DTX_ cycle :- Defines the pattern of time instances where the
  *start uplink E-DCH transmission after inactivity is allowed
  */
typedef enum
{
  /* Units of subframes.
   *sub frames :5, 10, 20 - are valid for 10msec TTI
   *sub frames 1, 4, 5, 8, 10, 16, 20 - are valid for 2msec TTI
  */
  L1_MAC_DTX_CYCLE_NUM_SUBFM_1 = 1,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_4 = 4,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_5 = 5,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_8 = 8,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_10 = 10,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_16 = 16,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_20 = 20,
  L1_MAC_DTX_CYCLE_NUM_SUBFM_MAX
}l1_mac_dtx_cycle_info_enum_type;

  /*Inactivity Threshold for UE DTX cycle 2 :- Defines a number of consecutive E-DCH TTIs
  *without an E-DCH transmission, after which the UE shall immediately move from
  *UE_DTX_cycle_1 to using UE_DTX_cycle_2, Units of E-DCH TTIs..
  */
typedef enum
{
  /* Units of E-DCD TTI's.
  * Valid range Num E-DCh TTI:- 1, 4, 8, 16, 32, 64, 128, 256
  */
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_1 = 1,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_4 = 4,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_8 = 8,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_16 = 16,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_32 = 32,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_64 = 64,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_128 = 128,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_256 = 256,
  L1_INACT_THRSHD_FOR_CYCLE2_NUM_TTI_INVALID
}l1_inactivity_threshold_for_ue_dtx_cycle2_info_enum_type;

  /* UE_DTX_long_ preamble_length:Determines in slots the
   *length of the preamble associated with the UE_DTX_cycle_2
   */
typedef enum
{
  /*Units in slots,Valid range :- 4, 15
  *default value is 2 slots
  */
  L1_LONG_PREAMBLE_LENGTH_NUM_SLOTS_2 = 2,
  L1_LONG_PREAMBLE_LENGTH_NUM_SLOTS_4 = 4,
  L1_LONG_PREAMBLE_LENGTH_NUM_SLOTS_15 = 15,
  L1_LONG_PREAMBLE_LENGTH_NUM_SLOTS_MAX
}l1_dtx_long_preamble_length_info_enum_type;

  /*MAC_Inactivity_ Threshold:E-DCH inactivity time after
   *which the UE can start E-DCH transmission only at given times.
   */
typedef enum
{
  /* Units of E-DCD TTI's.
  * Valid range: 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, Infinity
  */
  L1_MAC_INACT_THRSHD_NUM_TTI_1 = 1,
  L1_MAC_INACT_THRSHD_NUM_TTI_2 = 2,
  L1_MAC_INACT_THRSHD_NUM_TTI_4 = 4,
  L1_MAC_INACT_THRSHD_NUM_TTI_8 = 8,
  L1_MAC_INACT_THRSHD_NUM_TTI_16 = 16,
  L1_MAC_INACT_THRSHD_NUM_TTI_32 = 32,
  L1_MAC_INACT_THRSHD_NUM_TTI_64 = 64,
  L1_MAC_INACT_THRSHD_NUM_TTI_128 = 128,
  L1_MAC_INACT_THRSHD_NUM_TTI_256 = 256,
  L1_MAC_INACT_THRSHD_NUM_TTI_512 = 512,
  L1_MAC_INACT_THRSHD_NUM_TTI_INFINITY = 513,
  L1_MAC_INACT_THRSHD_NUM_TTI_MAX,
}l1_mac_inactivity_threshold_info_enum_type;

  /*CQI_DTX_Timer:Specifies the number of subframes after
   *an HS-DSCH reception during which the CQI reports have
   *higher priority than the DTX pattern. This is the initial value
   *of CQI nominal reporting timer
   */
typedef enum
{
  /*Units of subframes
  * Valid range: 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, Infinity
  */
  L1_CQI_DTX_TIMER_NUM_SUBFM_0,
  L1_CQI_DTX_TIMER_NUM_SUBFM_1,
  L1_CQI_DTX_TIMER_NUM_SUBFM_2,
  L1_CQI_DTX_TIMER_NUM_SUBFM_4,
  L1_CQI_DTX_TIMER_NUM_SUBFM_8,
  L1_CQI_DTX_TIMER_NUM_SUBFM_16,
  L1_CQI_DTX_TIMER_NUM_SUBFM_32,
  L1_CQI_DTX_TIMER_NUM_SUBFM_64,
  L1_CQI_DTX_TIMER_NUM_SUBFM_128,
  L1_CQI_DTX_TIMER_NUM_SUBFM_256,
  L1_CQI_DTX_TIMER_NUM_SUBFM_512,
  L1_CQI_DTX_TIMER_NUM_SUBFM_INFINITY,
  L1_CQI_DTX_TIMER_NUM_SUBFM_MAX
}l1_cqi_dtx_timer_info_enum_type;

  /*UE DPCCH burst_1:Determines the Uplink DPCCH burst
   *length in subframes, when UE_DTX_cycle_1 is applied
   */
typedef enum
{
  /*Units of subframes
  *Valid range:1, 2, 5
  */
  L1_UE_DPCCH_BURST_1_NUM_SUBFM_1 = 1,
  L1_UE_DPCCH_BURST_1_NUM_SUBFM_2 = 2,
  L1_UE_DPCCH_BURST_1_NUM_SUBFM_5 = 5,
  L1_UE_DPCCH_BURST_1_NUM_SUBFM_MAX
}l1_ue_dpcch_burst_1_info_enum_type;

  /*UE DPCCH burst_2:Determines the Uplink DPCCH
   *burst length in subframes, when UE_DTX_cycle_2 is applied
   */
typedef enum
{
  /*Units of subframes
  *Valid range:1, 2, 5
  */
  L1_UE_DPCCH_BURST_2_NUM_SUBFM_1 = 1,
  L1_UE_DPCCH_BURST_2_NUM_SUBFM_2 = 2,
  L1_UE_DPCCH_BURST_2_NUM_SUBFM_5 = 5,
  L1_UE_DPCCH_BURST_2_NUM_SUBFM_MAX
}l1_ue_dpcch_burst_2_info_enum_type;


/*Uplink DPCCH slot format information:Slot format to be used on UL DPCCH*/
typedef enum
{
  /*Units in slots
  *Valid range: 1, 3, 4
  */
  L1_UL_DPCCH_SLOT_FORMAT_INVALID,
  L1_UL_DPCCH_SLOT_FORMAT_1=1,
  L1_UL_DPCCH_SLOT_FORMAT_3=3,
  L1_UL_DPCCH_SLOT_FORMAT_4=4,
  L1_UL_DPCCH_SLOT_FORMAT_MAX
}l1_uplink_dpcch_slot_format_info_enum_type;

/* This enum indicate what action L1 needs to take on received HS-SCCH orders*/
typedef enum
{
  /* L1 to save & continue using any previously received HS-SCCH orders*/
  L1_HS_SCCH_ORDER_ACTION_BACKUP,
  /*L1 to restore any previously received HS-SCCH orders which were Ackd*/
  L1_HS_SCCH_ORDER_ACTION_RESTORE,
  /* L1 to save & act as if HS-SCCH orders were never received*/
  L1_HS_SCCH_ORDER_ACTION_BACKUP_AND_DISCARD,
  /* L1 to clean HS-SCCH orders data base, both current & backup. RRC to set this only for 1st time DTX start case*/
  L1_HS_SCCH_ORDER_ACTION_INIT
}l1_dtx_drx_hs_scch_order_info_enum_type;

/*Contains all the info needed for DTX operation:
*Ref 10.3.6.34a in 25.331*/
typedef struct
{
  /*E-DCH TTI*/
  e_tti_enum_type e_tti;

  l1_ue_dtx_cycle1_info_enum_type ue_dtx_cycle1;

  l1_ue_dtx_cycle2_info_enum_type ue_dtx_cycle2;

  l1_mac_dtx_cycle_info_enum_type l1_mac_dtx_cycle;

  l1_inactivity_threshold_for_ue_dtx_cycle2_info_enum_type inact_thrshd_dtx_cycle2;

  /*Serving Grant value to be used at the transition in DTX-Cycle-2.
  *(0..37,38) indicates E-DCH serving grant index ; index 38 means zero grant.
  * a Value of 0xFF(L1_SERVING_GRANT_NOT_PRESENT) - means default sg not present
  */
  uint8 default_sg_in_dtx_cycle2;

  /* Preamble length to be used for UL DPCCH in cycle 2 SPEC FOLLOWS THIS NAME */
  l1_dtx_long_preamble_length_info_enum_type dtx_long_preamble_length;

  l1_mac_inactivity_threshold_info_enum_type l1_mac_inactivity_threshold;

  l1_cqi_dtx_timer_info_enum_type cqi_dtx_timer;

  l1_ue_dpcch_burst_1_info_enum_type ue_dpcch_burst_1;

  l1_ue_dpcch_burst_2_info_enum_type ue_dpcch_burst_2;

  l1_uplink_dpcch_slot_format_info_enum_type uplink_dpcch_slot_format_info;

}l1_dtx_info_struct_type;

  /*Contains all the info needed for DRX operation:
  *Ref 10.3.6.34a in 25.331*/


  /*Inactivity_threshold_ for_UE_Grant_Monitoring-Determines the number of subframes 
  *after an E-DCH scheduled transmission during which the UE is required to monitor the
  * full E-AGCH transmissions from the serving radio link and the full E-RGCH(s) from all 
  *the cells in the E-DCH active set
  */
  typedef enum
  {
    /* Units of E-DCD TTI's.
    * Valid range Num E-DCh TTI:- 0, 1, 2, 4, 8, 16, 32, 64, 128, 256 
    */
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_0 = 0,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_1 = 1,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_2 = 2,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_4 = 4,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_8 = 8,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_16 = 16,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_32 = 32,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_64 = 64,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_128 = 128,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_256 = 256,
    L1_INACT_THRSHD_FOR_GRANT_MONIT_NUM_TTI_INVALID
  }l1_inact_thrshd_for_ue_grant_monitoring_info_enum_type;

  /*Inactivity_threshold_ for_UE_DRX_cycle :- Defines the number of subframes 
  *after an HS-SCCH reception or after the first slot of an HS-PDSCH reception  
  *during which the UE is required to monitor the HS-SCCHs in the UE's HS-SCCH set continuously
  */
typedef enum
{
  /*Units of sub-frames (0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512)*/
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_0 = 0,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_1 = 1,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_2 = 2,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_4 = 4,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_8 = 8,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_16 = 16,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_32 = 32,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_64 = 64,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_128 = 128,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_256 = 256,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_512 = 512,
  L1_INACT_THRSHD_FOR_UE_DRX_CYCLE_NUM_SUBFM_INVALID
}l1_inact_thrshd_for_ue_drx_cycle_info_enum_type;



/*UE_DRX_cycle :- HS-SCCH reception pattern length in subframes*/
typedef enum
{
  /* Units of subframes.
   sub frames  4, 5, 8, 10, 16, 20 - are valid 
  */
  L1_UE_DRX_CYCLE_NUM_SUBFM_4 = 4,
  L1_UE_DRX_CYCLE_NUM_SUBFM_5 = 5,
  L1_UE_DRX_CYCLE_NUM_SUBFM_8 = 8,
  L1_UE_DRX_CYCLE_NUM_SUBFM_10 = 10,
  L1_UE_DRX_CYCLE_NUM_SUBFM_16 = 16,
  L1_UE_DRX_CYCLE_NUM_SUBFM_20 = 20,
  L1_UE_DRX_CYCLE_NUM_SUBFM_MAX
}l1_ue_drx_cycle_info_enum_type;

typedef struct
{
  /*UE_DRX_cycle :- HS-SCCH reception pattern length in subframes*/
  l1_ue_drx_cycle_info_enum_type ue_drx_cycle;

  /*Inactivity_threshold_ for_UE_DRX_cycle*/
  l1_inact_thrshd_for_ue_drx_cycle_info_enum_type inact_thrshd_drx_cycle;

  /*Inactivity_threshold_ for_UE_Grant_Monitoring*/
  l1_inact_thrshd_for_ue_grant_monitoring_info_enum_type inact_thrshd_grant_monitoring;

  /*A Boolean which determines whether the UE is required to monitor the full E-AGCH transmissions 
  *from the serving E-DCH cell and the full E-RGCH from cells in the serving E-DCH radio link set when
  *they overlap with the start of an HS-SCCH reception as defined in the HS-SCCH reception pattern*/
  boolean l1_ue_drx_grant_monitoring;

}l1_drx_info_struct_type;



/* DTX-DRX timing information: Ref: 10.3.6.34b in 25.331*/
typedef struct
{
  /*Enabling Delay, values in radio frames.
  * a value of L1_NUM_RADIO_FRAMES_MAX - means enabling_delay has invalid
  * value i.e. its not init.RRC should not use this value when DTX operation is ON
  */
  l1_dtx_drx_enabling_delay_info_enum_type enabling_delay;

  /*Offset of the DTX and DRX cycles at the given TTI. Units of
  *subframes, valid range of values are 0 ...159.
  *A value of 0xFF - means dtx_drx_offset has invalid value i.e. its not init.
  *RRC should not use this value when DTX operation is ON
  */
  uint8 dtx_drx_offset;

} l1_dtx_drx_timing_info_struct_type;

/* DTX-DRX information:Ref 10.3.6.34a & 10.3.6.34b in 25.331*/
typedef struct
{
  /*Timing info needed to start DTX/DRX at L1 for CPC*/
  l1_dtx_drx_timing_info_struct_type *dtx_drx_timing_info;

  /*All the info needed for DTX operation at L1 for CPC*/
  l1_dtx_info_struct_type *dtx_info;

  /*All the info needed for DRX operation at L1 for CPC*/
  l1_drx_info_struct_type *drx_info;

} l1_dtx_drx_info_struct_type;

/* =================================================*/
/*                                  HS RACH INFORMATION                                           */
/* =================================================*/


/* This request mask will outline the action related to HS_RACH */
#define  L1_HS_RACH_NO_OP             0x00
/* START is sent when there is no HS RACH active*/
#define  L1_HS_RACH_START             0x01
/* Reconfig is sent for many instances including for cell transition request. This currently
** is being implemented as stop+start in L1*/
#define  L1_HS_RACH_RECFG             0x02
/* To deconfigure HS RACH, STOP is sent. This can happen when the UE reselects to 
** a cell in which there is no HS RACH active, on FACH to DCH transition or on a SIB
** refresh where in new SIB 5 there is no common EDCH information*/
#define  L1_HS_RACH_STOP              0x04

#define L1_HS_RACH_STOP_AND_START     0x05

/* RRC would set this as the action when it receives
** CU CNF message from NW and only RNTIs are present
** in the same*/
#define L1_HS_RACH_ERNTI_UPDATE 0x06

/* The PRACH parameters which are needed for HS RACH other than the params present in the prach_cfg_struct
** for legacy PRACH*/
typedef struct
{
   /*Below values are valid only if cphy_setup has req_mask CPHY_HS_RACH_CHAN_CFG_INCL*/ 
  /*TRUE: E-AICH is in use. FALSE: E-AICH is not in use*/
    boolean                    e_ai_indication;
    
    /*In dB INTEGER   (-5 .  . 10 ). Power offset between the last transmitted preamble and the
    * initial DPCCH transmission, invalid value = L1_HS_RACH_INVALID_POWER_OFFSET_PP_E 11
    */
    int8   powerOffsetPp_e;

}l1_hs_rach_preamble_info_struct_type;

/* The below structure provides the F-DPCH related information. As RRC can provide only 3 parameters
** as part of the CPHY_SETUP_REQ. The rest needed params will be obtained as part of the common
** EDCH resource index and getting these params by indexing into the common EDCH information list*/
typedef struct
{
  /*dl_common_info*/
  
  /*Use "Single TPC" is DPC_Mode=0, please refer to 8.5.45 in 25.331*/
  /* Downlink DPCH power control info */
  l1_dpc_mode_enum_type dpc_info;

  /*SIB5 I.E CommonEDCHSystemInfo
    has dl-FDPCH-TPCcommandErrorRate         INTEGER   (1 .  . 10 )  ,
    only difference is in DCH dl-FDPCH-TPCcommandErrorRate can have a value from 1 to 16
  */
  /* TPC command error rate target. Applicable only for F-DPCH */
  uint8 tpc_cmd_err_rate_target;

  /*NO dl_info_per_rl for HS-RACH*/
  /*8.5.45 in 25.331
    1>  configure F-DPCH in accordance with the F-DPCH slot format #0 [26];
  */
  uint32 fdpch_slot_format;

}l1_hs_rach_f_dpch_info_struct_type;

/* This below structure has the UL DPCCH information that will be present in the common EDCH information
** list*/
typedef struct
{
  /* short or long SC codes */
  l1_ul_dpch_sc_enum_type  scrambling_code_type;
  
  /*  Scrambling Code value (0 .  . 16777215) */
  uint32                   scrambling_code; 
} l1_ul_dpch_codeinfoforcommon_e_dch_info_struct_type;

/* This below structure gives all the parameters that will be present in one entry of the
** common EDCH information list array. There can be max 32*/
typedef struct
{
  /*(0..9) indicates symbol offset*/
  uint8 s_offset;
  
  /*Integer (0..255)*/
  uint8 f_dpch_chan_code_num;
  
  /*RGCH info*/
  boolean  e_rgch_info_present;
  
  l1_e_rgch_info_struct_type e_rgch_info;
  
  /*HICH info*/
  l1_e_hich_info_struct_type e_hich_info;
  
  /*ul dpch code info for common e-dch*/
  l1_ul_dpch_codeinfoforcommon_e_dch_info_struct_type l1_ul_dpch_code_info;
} l1_common_e_dch_info_struct_type;

/* Below structure holds the information regarding all the common EDCH information list and also
** how many common EDCH resources are configured in the SIB 5*/
typedef struct
{
  /*number of e_dch_common_info SIZE   (1 .  . 32 )*/
  uint8 num_e_dch_common_info;

  l1_common_e_dch_info_struct_type e_dch_common_info[L1_MAX_E_DCH_COMMON_LIST];
} l1_e_dch_common_list_info_struct_type;


/* Number of MAC HS queues */
#define L1_DL_MAC_HS_Q_MAX_COUNT    8


/* Maximum number of HARQ processes */
#define L1_DL_HS_DSCH_HARQ_MAX_PROC 8
#define L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO L1_DL_HS_DSCH_HARQ_MAX_PROC
#define L1_DL_HS_DSCH_HARQ_MAX_PROC_MIMO 16

#define HS_DPCCH_BETA_HS_INFO_PRESENT_BITMASK 0x01
#define HS_DPCCH_MEAS_FEEDBACK_INFO_PRESENT_BITMASK 0x02

/* This structure defines the information related to
   BetaHS fields of DPCCH channel */
typedef struct
{
  /* delta CQI (0..8) */
  uint8 delta_cqi;
  /* delta ACK (0..8) */
  uint8 delta_ack;
  /* delta NACK (0..8) */
  uint8 delta_nack;
} l1_ul_hs_dpcch_beta_hs_info_struct_type;

/* This structure defines the information related to
   measurement feedback fields of UL HS DPCCH channel */
typedef struct
{
  /* Power offset between CPICH and HS PDSCH
     Range is from -6 to 13 in units of 0.5 dB
     Values ranges from -12 to 26 */
  int8 po_hs_dsch;
  /* CQI feedback cycle count (0..160 ms) */
  uint8 cqi_feedback_cycle;
  /* CQI repetition factor (1..4) */
  uint8 cqi_rep_factor;
  /* ACK-NACK repetition factor (1..4) */
  uint8 ack_nack_rep_factor;

  /* HARQ Preamble Mode (on/off) */
  boolean harq_pmbl_mode;
} l1_ul_hs_dpcch_meas_feedback_info_struct_type;
 
/* This structure defines the information related to
   UL HS DPCCH channel */
typedef struct
{
  /* Bit mask indicating which information is present */
  uint8 info_present_bitmask;
 
  /* Beta HS information */
  l1_ul_hs_dpcch_beta_hs_info_struct_type beta_hs_info;
 
  /* Measurement fedback related information */
  l1_ul_hs_dpcch_meas_feedback_info_struct_type meas_feedback_info;
} l1_ul_hs_dpcch_info_struct_type;

/* This emum defines the expicit memory partitions among different
   HARQ processes.
   WARNING: THIS ENUM LIST IS OBTAINED FROM SPEC. DO NOT ATTEMPT TO
   CHANGE THE VALUES OF EXISTING ENUMS IN THIS LIST.
 */
typedef enum
{
  L1_DL_HS_DSCH_HMS800, /* Start 800..16000 in step of 800 (enum value 0) */
  L1_DL_HS_DSCH_HMS1600,
  L1_DL_HS_DSCH_HMS2400,
  L1_DL_HS_DSCH_HMS3200,
  L1_DL_HS_DSCH_HMS4000,
  L1_DL_HS_DSCH_HMS4800,
  L1_DL_HS_DSCH_HMS5600,
  L1_DL_HS_DSCH_HMS6400,
  L1_DL_HS_DSCH_HMS7200,
  L1_DL_HS_DSCH_HMS8000,
  L1_DL_HS_DSCH_HMS8800,
  L1_DL_HS_DSCH_HMS9600,
  L1_DL_HS_DSCH_HMS10400,
  L1_DL_HS_DSCH_HMS11200,
  L1_DL_HS_DSCH_HMS12000,
  L1_DL_HS_DSCH_HMS12800,
  L1_DL_HS_DSCH_HMS13600,
  L1_DL_HS_DSCH_HMS14400,
  L1_DL_HS_DSCH_HMS15200,
  L1_DL_HS_DSCH_HMS16000, /* End     800..16000 in step of 800  (enum index 18) */
  L1_DL_HS_DSCH_HMS17600, /* Start 17600..32000 in step of 1600 (enum index 19) */
  L1_DL_HS_DSCH_HMS19200,
  L1_DL_HS_DSCH_HMS20800,
  L1_DL_HS_DSCH_HMS22400,
  L1_DL_HS_DSCH_HMS24000,
  L1_DL_HS_DSCH_HMS25600,
  L1_DL_HS_DSCH_HMS27200,
  L1_DL_HS_DSCH_HMS28800,
  L1_DL_HS_DSCH_HMS30400,
  L1_DL_HS_DSCH_HMS32000, /* End   17600..32000 in step of 1600 (enum index 28) */
  L1_DL_HS_DSCH_HMS36000, /* Start 36000..80000 in step of 4000 (enum index 29) */
  L1_DL_HS_DSCH_HMS40000,
  L1_DL_HS_DSCH_HMS44000,
  L1_DL_HS_DSCH_HMS48000,
  L1_DL_HS_DSCH_HMS52000,
  L1_DL_HS_DSCH_HMS56000,
  L1_DL_HS_DSCH_HMS60000,
  L1_DL_HS_DSCH_HMS64000,
  L1_DL_HS_DSCH_HMS68000,
  L1_DL_HS_DSCH_HMS72000,
  L1_DL_HS_DSCH_HMS76000,
  L1_DL_HS_DSCH_HMS80000, /* End   36000..80000  in step of 4000 (enum index 40) */
  L1_DL_HS_DSCH_HMS88000, /* Start 88000..160000 in step of 8000 (enum index 41) */
  L1_DL_HS_DSCH_HMS96000,
  L1_DL_HS_DSCH_HMS104000,
  L1_DL_HS_DSCH_HMS112000,
  L1_DL_HS_DSCH_HMS120000,
  L1_DL_HS_DSCH_HMS128000,
  L1_DL_HS_DSCH_HMS136000,
  L1_DL_HS_DSCH_HMS144000,
  L1_DL_HS_DSCH_HMS152000,
  L1_DL_HS_DSCH_HMS160000, /* End    88000..160000 in step of 8000  (enum index 50) */
  L1_DL_HS_DSCH_HMS176000, /* Start 176000..304000 in step of 16000 (enum index 51) */
  L1_DL_HS_DSCH_HMS192000,
  L1_DL_HS_DSCH_HMS208000,
  L1_DL_HS_DSCH_HMS224000,
  L1_DL_HS_DSCH_HMS240000,
  L1_DL_HS_DSCH_HMS256000,
  L1_DL_HS_DSCH_HMS272000,
  L1_DL_HS_DSCH_HMS288000,
  L1_DL_HS_DSCH_HMS304000,  /* End 176000..304000 in step of 16000 (enum index 59) */
  L1_DL_HS_DSCH_HMS43200    /* This enum is not in Spec, this is just for internal usage */
} l1_dl_hs_dsch_harq_mem_sz_enum_type;

/* 25.331 Rel-7 Spec - Maximum number of logical channels */
#define L1_DL_MAX_LOGICAL_CHANNEL 16

#define L1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT  MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT

/** Number of MAC-d PDU size information per queue */
#define MCALWCDMA_DEC_HS_MACD_PDU_SZ_PER_Q_MAX_COUNT 8

/* This structure defines HARQ process memort partition information */
typedef struct
{
  /* Number of HARQ processes */
  /* With MIMO, the possible values can be 1..8 or 12 or 14 or 16 */
  /* Without MIMO, the possible values can be 1..8 */
  uint8 num_proc;
  
  /* indicate if memory partitioning is specified explicit by N/W */
  boolean mem_part_info_valid;
  /* HARQ process mem partition information
     Valid only if mem_part_info_valid is valid */
  l1_dl_hs_dsch_harq_mem_sz_enum_type
    mem_size_type[L1_DL_HS_DSCH_HARQ_MAX_PROC_MIMO];

  /* Indicate if additional memory partitioning is specified explicitly by N/W for MIMO */
  boolean mimo_additional_mem_part_info_valid;
  /* HARQ process additional mem partition information
     Valid only if mimo_additional_mem_part_info_valid is valid */
  l1_dl_hs_dsch_harq_mem_sz_enum_type
    mimo_additional_mem_size_type[L1_DL_HS_DSCH_HARQ_MAX_PROC_MIMO];
} l1_dl_hs_dsch_harq_info_struct_type;

/* This structure defines MAC-d PDU size information for given MAC HS queue */
typedef struct
{
  /* PDU size index in MAC HS header */
  uint8 pdu_sz_idx;
  /* PDU size */
  uint16 pdu_sz;
} l1_dl_hs_dsch_mac_d_pdu_sz_info_struct_type;

  /* This structure defines single MAC HS queue information */
  typedef struct
  {
    /* MAC HS queue Id */
    uint8 mac_hs_q_id;
    /* number of MAC-d PDU sizes for this queue */
    uint8 num_mac_d_pdu_sz;
    /* MAC PDU size type information for this queue */
    l1_dl_hs_dsch_mac_d_pdu_sz_info_struct_type
      mac_d_pdu_sz_info[L1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT];
  
  } l1_dl_hs_dsch_mac_hs_q_info_struct_type;


  /* This structure defines MAC HS queue information */
typedef struct
{
  /* Number of MAC HS queues */
  uint8 num_mac_hs_q;
  /* Each MAC HS queue information */
  l1_dl_hs_dsch_mac_hs_q_info_struct_type
    mac_hs_q_info[L1_DL_MAC_HS_Q_MAX_COUNT];
} l1_d1_hs_dsch_mac_d_flow_info_struct_type;



/* This structure defines the HS DSCH TrCh information */
typedef struct
{
  /* CTrCh Id for HS DSCH TrCh. The possible value can be 6 and 7 */
  uint8 cctrch_id;
  /* H-RNTI, this is 16 bits in length */
  uint16 h_rnti;
  /* HARQ information */
  l1_dl_hs_dsch_harq_info_struct_type harq_info;

  /* Bit mask to indicate which of H-RNTI is valid, 
   * Bit mask to indicate Common H-RNTI Valid is L1_COMMON_H_RNTI_VALID 0x01
   * Bit mask to indicate BCCH H-RNTI Valid is L1_BCCH_H_RNTI_VALID 0x02
   * Bit mask to indicate dedicated H-RNTI Valid is L1_DEDICATED_H_RNTI_VALID 0x04
   * Note: At any point of time, either Common or dedicated H-RNTI will be active. Both cant be valid at same time
   * RRC will fill Common or dedicated in uint16 h_rnti.
   */
  uint8 efach_h_rnti_bmsk;
  
  /* BCCH Specific H-RNTI, this is 16 bits in length .Valid only if valid_bit_mask_for_fach_pch_h_rnti
   * has bit mask L1_BCCH_H_RNTI_VALID
   */
  uint16 bcch_h_rnti;

    /*Informs MAC if MAC HS is configured or MAC EHS */
    mac_hs_e_type mac_hs_or_ehs;
  
    /* Indicates if the HS-DSCh TB table to be chosen is octet aligned or not*/
    l1_dl_hs_dsch_tb_size_alignment_enum_type  hsdsch_tb_size_alignment_type;
  
    /* If config is EHS, then only interpret the following EHS info.
     * Otherwise, HS info is valid for this cmd */
  
    /* This field contains the queue id for each logical channel mapped 
     * to MAC-Ehs. If the logical channel is not in use, then the corresponding
     * queue mapping should be set by RRC to INVALID_QUEUE_ID(value 8). */
    uint8 mac_ehs_dlchan_q_info[L1_DL_MAX_LOGICAL_CHANNEL];

    boolean hs_64_qam_configured;
  
    /* MAC-d information. This is used for deciding mac HS header only */
    l1_d1_hs_dsch_mac_d_flow_info_struct_type macd_info;
  } l1_dl_hs_dsch_info_struct_type;

/* Max  number of uplink TrCHs */
#define L1_UL_TRCH_MAX  8

/* Maximum #TFCs allowed - implementation specific */
#define L1_UL_TFC_MAX                   64

/* Minimum #TFCs allowed */
#define L1_UL_TFC_MIN                     1

/* Reference TFC range */
#define L1_UL_REF_TFC_MAX               4

#define MAX_DPCH   1

/* Uplink physical channels */
typedef enum {
  L1_UL_PHYCHAN_DPCH,
  L1_UL_PHYCHAN_PRACH,
  L1_UL_PHYCHAN_PCPCH,
  L1_UL_PHYCHAN_HS_PRACH,
  L1_UL_PHYCHAN_HS_DPCH,
  L1_UL_PHYCHAN_SEC_DPCH,
  L1_UL_NUM_PHYCHAN,
  L1_UL_PHYCHAN_NONE
} l1_ul_phychan_enum_type;

/* Uplink Coding types */
typedef enum {
  L1_UL_CODING_NONE,
  L1_UL_CODING_CONV_ONE_HALF,
  L1_UL_CODING_CONV_ONE_THIRD,
  L1_UL_CODING_TURBO,
  L1_UL_NUM_CODING_TYPE
} l1_ul_coding_enum_type;

/* CRC sizes */
typedef enum {
  L1_UL_CRC_0_BITS,
  L1_UL_CRC_8_BITS,
  L1_UL_CRC_12_BITS,
  L1_UL_CRC_16_BITS,
  L1_UL_CRC_24_BITS,
  L1_UL_NUM_CRC
} l1_ul_crc_enum_type;

/* Gain Factor calculation methods */
typedef enum {
  L1_UL_GF_CALCULATED,
  L1_UL_GF_SIGNALLED
} l1_ul_gain_factor_calc_enum_type;

/* Semi-static Parameters */
typedef struct {
  /* TTI in ms {10, 20, 40, 80} */
  l1_tti_enum_type       tti;
  /* Coding type */
  l1_ul_coding_enum_type coding;
  /* Static Rate Matching attribute 1..256 */
  uint16                 staticRM;
  /* CRC type */
  l1_ul_crc_enum_type    crc;
  /* Transport chanel ID from RRC */
  uint8  trch_id;
} l1_ul_semi_static_parms_struct_type;

/* Gain Factor Parameters - indexed by TFC id.
 * See 25.331 V3.3.0, 2000-06, 10.3.5.8.
 */
typedef struct {
  /* gain factor calculation method - signalled or calculated */
  l1_ul_gain_factor_calc_enum_type algo;
  /* Reference TFC ID.  Mandatory for computed.  Optional for signalled. */
  uint8                            ref_tfc_id;
  /* Used only for signalled.  TRUE if ref_tfc_id above is valid. */
  boolean                          ref_tfc_id_valid;
  /* Gains valid only for signalled TFCs */
  /* control channel gain */
  uint8                            BetaC;
  /* data channel gain */
  uint8                            BetaD;
  /* Do we have a valid PRACH power offset specified ? */
  boolean                          valid_Pp_m;
  /* PRACH power offsets (between preamble and message) -5..10 dB */
  int8                             pwr_offset_Pp_m;
} l1_ul_gain_factor_parms_struct_type;

/* filler polarity */
typedef enum {
  /* pad code segments with 1s */
  L1_UL_RF_EQ_FILLER_0,
  /* pad code segments with 0s */
  L1_UL_RF_EQ_FILLER_1,
  L1_UL_NUM_FILLER_TYPE
} l1_ul_filler_polarity_enum_type;


/* UL CCTrCH items */
typedef struct {
  /* uplink physical channel type */
  l1_ul_phychan_enum_type               chan;
  /* number of transport channels valid for this CCTrCH */
  uint8                                 num_trch;
  /* semi-static parameters for valid transport channels */
  l1_ul_semi_static_parms_struct_type  *semi_static_parms_ptr[L1_UL_TRCH_MAX];
  /* number of TFCIs */
  uint8                                 num_tfci;
  l1_ul_gain_factor_parms_struct_type  *gain_factor_parms_ptr[L1_UL_TFC_MAX];
  /* Maps reference TFC IDs to real TFC IDs for computed gain factors. */
  uint8                                 ref_tfc_to_real_tfc[L1_UL_REF_TFC_MAX];
  /* filler value for code segments after radio frame equalization */
  l1_ul_filler_polarity_enum_type       filler_val;
} l1_ul_cctrch_cfg_struct_type;



/*--------------------------------------------------------------------------
                            ENUM: DL_REF_PHYCHAN_ENUM_TYPE
                            
This enumerates the various downlink physical channel reference id's                            
--------------------------------------------------------------------------*/


#define MAX_SCCPCH 2

/* max number of radio links, corresponds to maxRL */
#define L1_MAX_RL 6   



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                   ENUM: CH_EST_CPICH_USAGE_ENUM_TYPE

This enumerates the types of CPICH usage for channel estimation and
corresponds to IE 10.3.6.53
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* To be on the safer side, start this ENUM from 1 to not pickup any default value */
typedef enum {
  L1_USE_PRI_CPICH = 1,
  L1_DO_NOT_USE_PRI_CPICH
} l1_ch_est_cpich_usage_enum_type;



/******************************************************************************
**                   wl1_cbs_drx_level_1_sched_struct                        **
**                                                                           **
** Cell Broadcast Services (CBS) data comes on the CTCH mapped to FACH on    **
** SCCPCH.  This structure contains the extra info needed if the SCCPCH      **
** being configured is carrying CTCH data.                                   **
**                                                                           **
** ctch_info_present                                                         **
** - TRUE means the following parameters are valid.  FALSE otherwise,        **
**   meaning special CTCH scheduling is not present.                         **
**                                                                           **
** ctch_allocation_period                                                    **
** - (1 - 256) period of CTCH allocation on SCCPCH in frames.                **
**                                                                           **
** cbs_frame_offset                                                          **
** - (0 - 255) number of frames from SFN 0 to first CTCH occasion.           **
**                                                                           **
** ctch_fach_tti                                                             **
** - TTI of the FACH carrying CTCH.  Fixed at 10, 20, 40, or 80 msec         **
**   for a given CTCH.                                                       **
**                                                                           **
** only_ctch_on_sccpch                                                       **
** - TRUE means the only logical channel mapped to this SCCPCH is CTCH.      **
**   FALSE otherwise (like if PCCH is mapped to the same SCCPCH).            **
**                                                                           **
** Described in 25.331, 8.5.16 Configuration of CTCH occasions and           **
**              25.331, 10.3.8.3 CBS DRX Level 1 information                 **
******************************************************************************/
typedef struct
{
  boolean           ctch_info_present;
  uint16            ctch_allocation_period;
  uint16            cbs_frame_offset;
  l1_tti_enum_type  ctch_fach_tti;
  boolean           only_ctch_on_sccpch;
} wl1_cbs_drx_level_1_sched_struct;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                   STRUCT: PCCPCH_PARMS_STRUCT_TYPE

This is just sort of a placeholder for the P-CCPCH parameters.  These are
constant but we have to store them somewhere.  This may change.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct {
  /* scrambling code */
  uint16 scr_code;

  /* 10.3.6.49 - TRUE if the PCCPCH uses STTD */
  boolean tx_div_ind;

  /* Pointer to an array of 512 bytes (or 4096 bits) to handle all 4096 SFN's.
  ** On N_BCH setup, this bit array identifies SFN's where RRC wants to receive
  ** SIB data.  A 1 bit tells WL1 to receive that frame.  A 0 means WL1 may skip
  ** it.  WL1 can use this info to sleep between SIB frames and conserve power. */
  uint8  *sib_sfn_bit_mask;
} l1_pccpch_parms_struct_type;

typedef struct {

  /* primary scrambling code of the CPICH */
  uint16 pri_cpich_scr_code;

  /* optional parameter presence indication bitmask */
  uint8 opt_parm_present_mask;

  /* 10.3.6.61 -> 10.3.6.53 Primary CPICH usage for channel estimation */
  l1_ch_est_cpich_usage_enum_type use_pri_cpich;

  /* 10.3.6.61 -> 10.3.6.63 -> 10.3.6.64
     secondary CPICH secondary scrambling code */
  uint8 sec_cpich_sec_scr_code;

  /* 10.3.6.61 -> 10.3.6.63 secondary CPICH channelisation code */
  uint8 sec_cpich_ch_code;

  /* 10.3.6.61 -> 10.3.6.64 Secondary Scrambling Code */
  uint8 sec_scr_code;

  /* 10.3.6.61 -> 10.3.6.68 TRUE if STTD is used on S-CCPCH */

  boolean tx_div_ind;

  /* 10.3.6.61 spreading factor */
  l1_sf_enum_type sf;

  /* 10.3.6.61 code number, i.e. ovsf channelisation code for S-CCPCH */
  uint16 code_num;

  /* 10.3.6.61 Pilot Symbol Existence */
  boolean pilot_exists;

  /* 10.3.6.61 TFCI Existence */
  boolean tfci_exists;

  /* NOTE: Fixed or Flexible positions is a CCTrCH property so it is not
           included here */

  /* number frames offset useful to compute cfn */           
  uint8 num_frame_offset;
  
  /* 10.3.6.61 Timing Offset */
  uint16 timing_offset;

  /* Describes CTCH (mapped to FACH on SCCPCH) occasions containing CBS
  ** (Cell Broadcast Services) data. */
  wl1_cbs_drx_level_1_sched_struct  cbs_sched;
} l1_sccpch_parms_struct_type;

typedef struct {

  /* 10.3.6.41 Channelisation code */
  uint8 ch_code;

  /* 10.3.6.41 Number of PI per frame */
  uint8 pi_per_frame;

  /* 10.3.6.41 -> 10.3.6.68 TRUE if STTD is used on PICH */

  boolean tx_div_ind;

  /* 10.3.6.62 -> 10.3.6.42 PICH power offset in EcIo relative
     to Primary CPICH */
  int8 pwr_offset;

  /* IMSI divided by 8192, as per 8.3 in 25.304 for R'99 2001-06, used in
     calculating PO. Note: This parameter is new in the transition to R'99
     2001-06. */
  uint64 imsi_div_8192;

  /* IMSI divided by K, as per 8.3 in 25.304, used in calculating PI,
     where K = number of SCCPCH that have a PCH mapped. */
  uint64 imsi_div_pch_cnt;

} l1_pich_parms_struct_type;


/* AICH Transmission Timing values  This defines the position of the PRACH
 * access slot in relation to the PCCPCH.
 */
typedef enum {
  /* AICH transmission timing 0 */
  L1_DL_AICH_TT_0,
  /* AICH transmission timing 1 */
  L1_DL_AICH_TT_1,
  L1_DL_NUM_AICH_TT
} l1_dl_aich_tt_enum_type;


/* AICH configuration */
/* See 25.331 V3.4.1 2000-09 10.3.6.2, 10.3.6.3 */
typedef struct {
  /* channelization code (0-255) */
  uint8                         ovsf;

  /* AICH TT value */
  l1_dl_aich_tt_enum_type       tt;

  /* TRUE if STTD is used on AICH */
  boolean                       tx_div_ind;

  /* pwr per transmitted AICH - CPICH pwr (-22..5) in dB */
  int8                          pwr_offset;
} l1_aich_parms_struct_type;

/*--------------------------------------------------------------------------
                     ENUM: L1_TIMING_IND_ENUM_TYPE

This enumerates whether or not radio link timing should be initialized
or maintained.
--------------------------------------------------------------------------*/
typedef enum {
  L1_TIMING_IND_INIT,
  L1_TIMING_IND_MAINTAIN,
  L1_NUM_TIMING_IND
} l1_timing_ind_enum_type;


typedef struct {
  /* optional parameter absence/presence bitmask.  Use the above #defines */
  uint8 opt_parm_present_mask;

  /* timing indication, init or maintain */
  l1_timing_ind_enum_type timing_ind;

  /* CFN <--> targetSFN frame offset */
  uint8 targ_frame_offset;

  /* Downlink DPCH power control info */
  l1_dpc_mode_enum_type dpc_info;

  /* Downlink power offset between DPCCH pilot and DPDCH */
  uint8 po3;

  /* spreading factor */
  l1_sf_enum_type sf;

  /* set to TRUE if TFCI is used */
  boolean tfci_exists;

  /* The number of pilot bits used in a slot */
  uint8 num_pilot_bits;

  /* Default value is FALSE */
  boolean  skip_sync_a_procedure;

  /* TPC command error rate target. Applicable only for F-DPCH */
  uint8 tpc_cmd_err_rate_target;

} l1_dl_dpch_common_info_struct_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                       ENUM: TX_DIV_MODE_ENUM_TYPE

This enumerates the types of transmit diveresity and corresponds with
IE 10.3.6.74.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum {
  L1_TX_DIV_NONE,
  L1_TX_DIV_STTD,
  L1_TX_DIV_CLOSED_LOOP_1,
  L1_TX_DIV_CLOSED_LOOP_2,
  L1_NUM_TX_DIV_MODES
} l1_tx_div_mode_enum_type;


typedef struct {
  /* Optional parameter presence indication bitmask */
  uint16 opt_parm_presence_bitmask;

  /* 10.3.6.20 -> 10.3.6.14 The downlink DPCH info common for all
     Radio Links */
  l1_dl_dpch_common_info_struct_type dpch_common;

  /* 10.3.6.20 -> 10.3.6.13 the default DPCH offset value */
  uint32 default_dpch_offset;

/* 10.3.6.20 -> 10.3.6.27 Compressed mode information */
  l1_cm_info_struct_type *comp_mode_info;

  /* 10.3.6.20 -> 10.3.6.74 The TX diversity mode */
  l1_tx_div_mode_enum_type tx_div_mode;

  /* Default value is FALSE */
  boolean    post_verification_valid;

} l1_dl_common_info_struct_type;

typedef struct {
  /* bitmask indicating the absence/presence of optional parameters.  Use
     the above defined bit definitions */
  uint16 opt_parms_present_mask;

  /* 10.3.6.17 Primary CPICH usage for channel estimation */
  l1_ch_est_cpich_usage_enum_type use_pri_cpich;

  /* 10.3.6.17 DPCH frame offset */
  uint16 frame_offset;

  /* 10.3.6.17 -> 10.3.6.63 -> 10.3.6.64
     secondary CPICH secondary scrambling code */
  uint16 sec_cpich_sec_scr_code;

  /* 10.3.6.17 -> 10.3.6.63 secondary CPICH channelisation code */
  uint16 sec_cpich_ch_code;

  /* how many DPCH multicode channels are being set up */
  uint16 num_dpch;

  /* array of per DPCH info structures, there will be num_dpch valid
     entries */
  struct
  {
    #define DL_DPCH_CODE_CH_SEC_SCR_CODE_PRESENT 0x01
    #define DL_DPCH_CODE_CH_SCR_CODE_CHG_PRESENT 0x02
    /* optional parameter presence mask */
    uint8 opt_parm_present_mask;

    /* 10.3.6.17 -> 10.3.6.64 DPCH Secondary Scrambling Code */
    uint16 sec_scr_code;

    /* 10.3.6.17 Code Number */
    uint16 code_number;

    /* 10.3.6.17 scrambling code change for compressed mode */
    boolean  scr_code_change;

  } dl_chan_code[MAX_DPCH];

  /* 10.3.6.17 TPC Combination Index */
  uint8 tpc_idx;

  /* 10.3.6.17 Closed loop timing adjustment mode */
  uint8 closed_loop_timing_adj_mode;

  boolean fdpch_sttd_ind;

  uint32 efdpch_slot_format;
} l1_dl_dpch_info_per_rl_struct_type;


/*--------------------------------------------------------------------------
               STRUCT: L1_DL_INFO_PER_RL_STRUCT_TYPE

This structure contains downlink channel information that is per radio link
--------------------------------------------------------------------------*/

typedef struct {
  /* primary scrambling code of the CPICH */
  uint16 pri_cpich_scr_code;

  l1_dl_dpch_info_per_rl_struct_type dpch_per_rl;

} l1_dl_info_per_rl_struct_type;


/*--------------------------------------------------------------------------
                   STRUCTURE: L1_DPCH_SYNC_PARMS_STRUCT_TYPE

This structure contains the counters used for DCH state sync detection.
--------------------------------------------------------------------------*/
typedef struct {
  /* the N312 counter used for physical channel established detection */
  uint16 N312;

  /* the N313 counter used for out of sync detection */
  uint16 N313;

  /* the N315 counter used for in sync detection */
  uint16 N315;

  /* Timer count for physical chanel establishment in seconds */
  uint8 T312;

  /* Timer count for RL failure declaration in seconds */
  uint8 T313;
} l1_dpch_sync_parms_struct_type;

typedef struct
{
  /* req mask for HS configuration */
  uint32 hs_req_mask;

  /* L1 DL HS DSCH TrCh info */
  l1_dl_hs_dsch_info_struct_type hs_dsch_info;

  /* serving HS-PDSCH radio link primary scrambling code */
  uint16 hs_pdsch_rl_psc;

  /* L1 DL HS SCCH physical channel info */
  l1_dl_hs_scch_info_struct_type hs_scch_info;

  /* UL HS DCCH physical chanel info */
  l1_ul_hs_dpcch_info_struct_type hs_dpcch_info;

/* Structure contains all parameters needed for MIMO operation
   L1 to look at below structure only if hs_req_mask indicates
   CPHY_HS_MIMO_CFG_INCL is present. */
  l1_dl_hs_mimo_info_struct_type mimo_info;

  /*num_hs_multi_carrier represents the total num of active secodnary carriers.
        All the other activate secondary carriers will be deactivated by L1 implicitly.
  
        hs_multi_carrier_info - Includes the secondary carrier info in the same order given by NW
     */
    uint8 num_sec_hs_carrier;
    l1_secondary_cell_info_type sec_hs_carrier_info[MAX_NUM_OF_SEC_CARRIERS];

  /* Indicate STTD is used on HS-SCCH & HS-PDSCH in eFACH, 
     valid only when hs_req_mask has CPHY_E_FACH_HS_CFG_INCL */
  boolean use_sttd;

  /*eFACH DRX info structure, valid only if hs_req_mask has CPHY_HS_E_FACH_DRX_CFG_INCL*/
  l1_hs_fach_drx_info_type efach_drx_info;
} l1_hsdpa_info_struct_type;

/*--------------------------------------------------------------------------
                   STRUCTURE: DL_PHYCHAN_DB_STRUCT_TYPE
This contains the information making up a page of the physical channel
configuration database
--------------------------------------------------------------------------*/
typedef struct {
  /* the PCCPCH parameters */
  l1_pccpch_parms_struct_type pccpch;

  /* array of MAX_SCCPCH SCCPCH parameters */
  l1_sccpch_parms_struct_type sccpch[MAX_SCCPCH];

  /* PICH setup flag, the PICH will be added upon adding or configuring
     an SCCPCH if this flag is set to TRUE */
  boolean use_pich;

  /* the PICH parameters */
  l1_pich_parms_struct_type pich;

  /* the AICH parameters */

  l1_aich_parms_struct_type aich;

  /*========= DPCH Parameters ===========*/
  /* Indicate DL DPCH information is for F-DPCH */
  boolean use_fdpch;

  /* the DPCH parameters common to all radio links */
  l1_dl_common_info_struct_type dl_common;

  /* the number of individual radio links */
  uint16 num_rl;

  /* array of DPCH info structures for parameters unique to each
     radio link */
  l1_dl_info_per_rl_struct_type dl_per_rl[L1_MAX_RL];

  /* parameters for DPCCH sync detection and reporting */
  l1_dpch_sync_parms_struct_type sync_det;
  /* HS RACH:: These structures are specific for HS RACH. The DL module would use these structure
  ** for channel setup procedure when the channel type is HS_RACH_FDPCH and HS RACH AICH*/

  l1_hs_rach_f_dpch_info_struct_type hs_rach_dl_fdpch_info;
    /*Aich config*/
  l1_aich_parms_struct_type hs_rach_aich;

/* When cphy req mask is set to either CPHY_SEC_DL_PHYCHAN_ADD_INCL or CPHY_SEC_DL_PHYCHAN_CNFG_INCL
 RRC will fill secondary fdpch info in the following 2 elemenets (sec_num_rl,sec_dl_per_rl ). The rest of all
 elements in this structure will not be filled by RRC for secondary fdpch*/
  /* the number of individual radio links */
  uint16 sec_num_rl;
  /* array of DPCH info structures for parameters unique to each
     radio link */
  l1_dl_info_per_rl_struct_type sec_dl_per_rl[L1_MAX_RL];
} l1_dl_phychan_db_struct_type;

#define L1_UL_ASC_MAX                   8

typedef struct {
  /* starting index into available signature list (0..15) */
  uint8    avail_sig_start_idx;  
  /* ending index into available signature list (0..15) */
  uint8    avail_sig_end_idx;    
  /* Assigned sub-channel number */
  uint8    assigned_sub_chan_num;   
} l1_ul_asc_struct_type;


/* Access Service Class (ASC) configuration.
 * See PRACH partitioning - 25.331 V3.4.1 2000-09 10.3.6.52.
 */
typedef struct {
  /* NUmber of Access Service Classes (1..8) */
  uint8                               num_asc;    
  /* access service class */
  l1_ul_asc_struct_type              *asc_info_ptr[L1_UL_ASC_MAX];
} l1_ul_asc_cfg_struct_type;

/* Power Control Algorithm */
typedef enum {
  /* algorithm 1 */
  L1_UL_PCA_ONE,
  /* algorithm 2 */
  L1_UL_PCA_TWO,
  L1_UL_NUM_PCA
} l1_ul_pca_enum_type;


/* Uplink PRACH configuration. See gain factor table for Pp-m information.
 * See 25.331 V3.4.1 2000-09/10.3.6.54 PRACH System Info List.
 */
typedef struct {
  /* bit mask of available signatures (0..15) */
  uint16                       avail_sig_mask;
  /* PRACH minimum spreading factor for use in transmission */   
  l1_sf_enum_type              sf_min;
  /* scrambling code number (PRACH SC type is always long) 0..15 */
  uint8                        sc_num; 
  /* uplink puncturing limit (.4..1, in steps of .04) * 100 */
  uint8                        punct_lim;
  /* bit mask of available sub-channels (0..11) */
  uint16                       avail_sub_chan_mask;
  /* TrCH id (1..32) */
  uint8                        trch_id;
  /* Access Service Class (PRACH partitioning) */
  /* See 25.331 V3.4.1 2000-09 10.3.6.52 */
  l1_ul_asc_cfg_struct_type    asc_cfg;
  /* See 25.331 V3.4.1 2000-09 10.3.6.53 PRACH Power Offset */
  /* PRACH preamble power ramp step when no ACK (P0 1..8 in dB) */
  int8                         pwr_offset_P0;
  /* Maximum number of preamble transmissions allowed (1..64) */
  uint8                        max_preamble_tx;
   /* The below structure would be populated by RRC only for HS RACH, i.e.
  ** only if the req_mask include HS RACH configuration. For normal RACH
  ** configuration the below structure should not be looked into by L1 in its code
  ** path. -- Exclusively for HS RACH*/

  l1_hs_rach_preamble_info_struct_type hs_rach_info;

} l1_ul_prach_cfg_struct_type;

/* TPC step sizes in dB */
typedef enum {
  /* TPC step size 1db */
  L1_UL_TPC_STEP_ONE,
  /* TPC step size 2db */
  L1_UL_TPC_STEP_TWO,
  L1_UL_NUM_TPC_STEP_SIZE
} l1_ul_tpc_step_enum_type;


/* Uplink DPCH Power Control Info.
 * See 25.331 V3.4.1 2000-09 10.3.6.89.
 */
typedef struct {
  /* SK - Delete this field if not needed */
  boolean                  valid;
  /* DPCCH power offset  */
  int16                    dpcch_pwr_offset;

  /* Power Control Preamble length - between 0 and 7 - type to allow transition
     to R'99 2001-06, per sec. 10.3.6.91 in 25.331. */
  uint8 pcp_len;

  /* SRB Delay - between 0 and 7 - new parameter necessary to allow transition
     to R'99 2001-06, per sec. 10.3.6.91 in 25.331. */
  uint8 srb_delay;

  /* Power Control Algorithm {1,2} */
  l1_ul_pca_enum_type      pca;             
  /* TPC step size {1,2} in dB - optional field, valid only if PCA is 1 */
  l1_ul_tpc_step_enum_type tpc_step_size;   
} l1_ul_dpch_pwr_ctl_info_struct_type;


/* FBI length in bits */
typedef enum {
  /* No FBI bits */
  L1_UL_FBI_LEN_0,
  /* 1 FBI bit */
  L1_UL_FBI_LEN_1,
  L1_UL_NUM_FBI_BITS
} l1_ul_fbi_len_enum_type;


/* Number of TPC Bits */
typedef enum {
  /* Num TPC bits is 2*/
  L1_UL_NUM_TPC_BITS_2,
  /* Num TPC bits is 4*/
  L1_UL_NUM_TPC_BITS_4,
  /* Invalid TPC bits */
  L1_UL_NUM_TPC_BITS_MAX
} l1_ul_num_tpc_bits_enum_type;

/* RAB Configuration types for RRC to inform L1  */
typedef enum 
{
  /* Other types can be added in future */
  RAB_TYPE_NONE,
  /* One PS RAB only of 64 kbps UL and DL */
  RAB_TYPE_UL_DL_64_ONLY,
  /* More than One PS RAB */
  RAB_TYPE_MRAB 
} l1_rrc_config_rab_enum_type;
/* UL DPCH Info. 
 * See 25.331 V3.4.1 2000-09 10.3.6.86.
 */
typedef struct {
  /* DPCH power control information */
  l1_ul_dpch_pwr_ctl_info_struct_type dpch_pwr_ctrl_info;
  /* short or long SC codes */
  l1_ul_dpch_sc_enum_type  scrambling_code_type;
  /*  Scrambling Code value (0..777215) */
  uint32                   scrambling_code; 
  /* number of DPDCHs (1..6) */
  uint8                    num_dpdch;
  /* spreading factor */
  l1_sf_enum_type          sf_min;
  /* TFCI present flag */
  boolean                  tfci_present;
  /* # FBI bits in DPCCH {1,2} - valid only if dpdch_num = 1 */ 
  l1_ul_fbi_len_enum_type  fbi_len; 
  /* uplink puncturing limit (.4..1, in steps of .04) * 100 */
  uint8                    punct_lim;
  
    /* Number Of TPC-Bits, a value of 0xFF indicate NumTPC bits not present */
    l1_ul_num_tpc_bits_enum_type      numtpc;
 /* L1 should be using the below element in the structure only if UL DPCCH is being
** established for HS RACH. Not for DCH state*/

 /*25.331 section 8.5.45 
 *configure Uplink DPCH in accordance with the uplink 
 *DPCCH slot format 1 
 */
 l1_uplink_dpcch_slot_format_info_enum_type dpcch_slot_format;

} l1_ul_dpch_cfg_struct_type;

/* uplink physical channel configuration structure */
typedef struct {
  /* Maximum allowed Tx power in dBm (-50..33) 
   * See 25.331 V3.4.1 2000-09 10.3.6.38.
   */
  int8                        max_tx_pwr;
  /* uplink PRACH configuration */
  l1_ul_prach_cfg_struct_type prach_cfg;
  /* uplink DPCH configuration */
  l1_ul_dpch_cfg_struct_type  dpch_cfg;
  /* HS RACH:: RRC will populate the HS RACH sepcific information for UL DPCCH and
  ** PRACH in the below structures. We are using the legacy structure types with additonal
  ** structures defined within them to capture the HS RACH specific params. Some params
  ** in the legacy structure do not apply for HS RACH, like TFCI, punc limit, tfcs ptr in PRACH
  ** structure. L1 code should not look into those information if HS RACH is not configured*/

  l1_ul_prach_cfg_struct_type hs_prach_cfg;
  l1_ul_dpch_cfg_struct_type hs_ul_dpch_cfg;

} l1_ul_phychan_cfg_struct_type;

/* DC cfg info structure*/
typedef struct {
  uint16 freq;
  uint16 psc;
  l1_hsdpa_sec_hsdpa_action_enum_type dc_ops_type;
} l1_dc_cfg_info_struct_type;

#define CPHY_HHO_NOT_A_HHO_INCL 0x0001
#define CPHY_HHO_INTERF_TI_HHO_INCL 0x0002
#define CPHY_HHO_INTERF_TM_HHO_INCL 0x0004
#define CPHY_HHO_INTRAF_TI_HHO_INCL 0x0008
#define CPHY_HHO_INTRAF_TM_HHO_INCL 0x0010
#define CPHY_HHO_FACH_TO_DCH_INTERF_TI_HHO_INCL 0x0020
#define CPHY_HHO_RESUME_TO_OLD_CONFIG_HHO_INCL 0x0100
  
#define CPHY_VOICE_CALL_ACTIVE 0x0001
#define CPHY_R99_PS_CALL_ACTIVE 0x0002
#define CPHY_HS_CALL_ACTIVE 0x0004
/* To distinguish the cs call type cs(voice/data) */
#define CPHY_CS_DATA_CALL_ACTIVE 0x0008
#define CPHY_CS_PS_0X0_ACTIVE 0x0013 //0x0010|CPHY_VOICE_CALL_ACTIVE|CPHY_R99_PS_CALL_ACTIVE
typedef struct
{
  /* TRUE means L1 should call l1_post_ul_mac_config_sig() and/or
  ** l1_post_dl_mac_config_sig() at the activation time, so MAC can start
  ** its configuration process too. */
  boolean  signal_ul_mac_at_act_time;
  boolean  signal_dl_mac_at_act_time;

  /* a bit mask that indicates which of the sub-primitives are included */
  /* in the CPHY_SETUP_REQ.  See the comments for the mask bit defines */
  /* for more details. */
  uint32 req_mask;

  /* RRC will set the bit based on what type of HHO it is going to be
  bit15(MSB) bit14 bit13 .........bit0(LSB)
  bit0 ...................0 HHO 
                           1 Not a HHO
  bit1 ...................1 for Inter Freq Timing Initialized HHO
  bit2 ...................1 for Inter Freq Timing Maintained HHO
  bit3 ...................1 for Intra Freq Timing Initialized HHO
  bit4 ...................1 for Intra Freq Timing Maintained HHO
  bit5 ...................1 for Inter Freq Timing Initialized FACH-DCH HHO
  bit6, bit 7 ...................reserved
  bit8 ...................1 revert back to old configuration */
  uint16 hho_setup_mask;

 /* This  definitions of legacy HHO bit mask 'hho_setup_mask' holds good for this secondary hho mask as well
       In legacy code (single ul) RRC is setting HHO bit mask only while reverting to old config upon failure. The 
       same will be followed for secondary */
  uint16 sec_hho_setup_mask;

  /* RRC will set the bit based on what type of call is active and 
     what type of call is being setup.
  bit7(MSB) bit6 bit5 .........bit0(LSB)
  bit0 ...................1 CS call  (CS voice if bit3 is 0)
  bit1 ...................1 Release 99 PS data call
  bit2 ...................1 HS call 
  bit3 ...................1 CS Data (along with bit0)
  bit4-7 ...................reserved */
  uint8 call_type_mask;

  /*Indicates if an emergency call is active */
  boolean is_emergency_call;

  /* whether rf frequency information is included or not */
  boolean freq_incl;

  /* the rf channel if included. */
  uint16 rf_chan;

  /* whether sec rf frequency information is included or not */
  boolean sec_freq_incl;
  /* the sec rf channel if included. */
  uint16 sec_rf_chan;

  /* djm - Compressed Mode Information */
  l1_cm_info_struct_type *compressed_mode_info;

  /*
   * data for CPHY_DL_CCTRCH_REQ
   */
  l1_dl_cctrch_info_struct_type   *dl_cctrch_info;
  l1_dl_ctfc_info_struct_type     *dl_ctfc_info;

  /* data for CPHY_UL_CCTRCH_REQ */
  l1_ul_cctrch_cfg_struct_type *ul_cctrch_info;

  /* data for CPHY_UL_PHYCHAN_DROP_REQ */
  l1_ul_phychan_enum_type drop_ul_phychan;

  /* data for CPHY_DL_PHYCHAN_DROP_REQ */
  l1_dl_phychan_enum_type drop_dl_phychan;

  /* data for CPHY_SEC_DL_PHYCHAN_DROP_REQ */
  l1_dl_phychan_enum_type sec_drop_dl_phychan;

  /*
   * data for CPHY_SEC_DL_PHYCHAN_ADD_REQ
   */
  l1_dl_phychan_enum_type sec_dl_phychan;

  /*
   * data for CPHY_DL_PHYCHAN_ADD_REQ /CPHY_DL_PHYCHAN_CFG_REQ.  dl_phychan is the downlink
   * physical channel to be added /reconfigured.  It is assumed that it will
   * be added /reconfigured to use the CCTrCH referred to in the CPHY_DL_CCRTCH_REQ
   * also included in the command packet
   */
  l1_dl_phychan_enum_type dl_phychan;
  l1_dl_phychan_db_struct_type *dl_phychan_db;

  /*
   * data for CPHY_UL_PHYCHAN_ADD_REQ /CPHY_UL_PHYCHAN_CFG_REQ.  ul_phychan is the uplink physical
   * channel to be added /reconfigured.  It is assumed that it will be added/reconfigured
   * to use the CCTrCH referred to in the CPHY_UL_CCTRCH_REQ also included
   * in the command packet
   */
  l1_ul_phychan_enum_type ul_phychan;
  l1_ul_phychan_cfg_struct_type  *ul_phychan_db;

  mac_ul_tfcs_type  *tfcs_ptr;

  /* --------------------------- */
  /* HSDPA related configuration */
  /* --------------------------- */
  
  /* HSDPA operation type for L1 */
  hsdpa_setup_ops_enum_type hsdpa_l1_ops;
  
  /* HSDPA information. If NULL then it is assumed that HSDPA information is
     not present */
  l1_hsdpa_info_struct_type *hsdpa_l1_info;

 /* Indicates L1 to flush all HARQs.
    RRC will set this to TRUE under following conditions:
      - NW explicitly indicate in the reconfig message
      - whenever TTI reconfiguration happens 

    NOTE: L1 is expected to look into this variable ir-respective of the value
          of 'e_req_mask'. */
 boolean mac_e_reset_indicator;

 /* req mask for EUL configuration */
 /* This request mask will outline the action related to HSUPA UL and DL */
 /* bitmask:
      L1_EUL_START                0x01
      L1_EUL_RECFG_UL             0x02
      L1_EUL_RECFG_DL             0x04
      L1_EUL_STOP                 0x08 */
 uint8 e_req_mask;
 
 l1_e_info_struct_type *e_info_ptr;

 /* req mask for EUL configuration */
 /* This request mask will outline the action related to HSUPA UL and DL */
 /* bitmask:
      L1_EUL_START                0x01
      L1_EUL_RECFG_UL             0x02
      L1_EUL_RECFG_DL             0x04
      L1_EUL_STOP                 0x08 */
 uint8 sec_e_req_mask;


 l1_sec_e_info_struct_type *sec_e_info_ptr;

/* The below interface structure is a pointer to the HS RACH EUL UL
** and EUL DL information*/

 l1_e_hs_rach_info_struct_type *hs_rach_e_info_ptr;

  /* req mask for DTX-DRX configuration */
  /* This request mask will outline the action related to DTX*/
  /* bitmask:
    L1_CPC_DTX_NO_OP                      0x00
    L1_CPC_DTX_START                      0x01
    L1_CPC_DTX_RECONFIG                   0x02
    L1_CPC_DTX_STOP                       0x04*/
  uint8 l1_cpc_dtx_req_mask;

  /*All the info needed for DTX/DRX operation at L1 for CPC*/
  l1_dtx_drx_info_struct_type dtx_drx_info;

  l1_dtx_drx_hs_scch_order_info_enum_type l1_hs_scch_order_action;

  /* This request mask will outline the action related to DRX*/
  /* bitmask:
    L1_CPC_DRX_NO_OP                      0x00
    L1_CPC_DRX_START                      0x01
    L1_CPC_DRX_RECONFIG                   0x02
    L1_CPC_DRX_STOP                       0x04*/
  uint8 l1_cpc_drx_req_mask;

 /* req mask for DC CFG cmd*/
 uint16 dc_req_mask;

  l1_dc_cfg_info_struct_type l1_dc_cfg;

/* This request mask will outline the action related to HS_RACH */
  /* bitmask:
       L1_HS_RACH_NO_OP             0x00
       L1_HS_RACH_START             0x01
       L1_HS_RACH_RECFG             0x02
       L1_HS_RACH_STOP              0x04 */
  uint8 hs_rach_req_mask;

  boolean transition_to_dch;
  
   /* e-dch common resource list*/
   l1_e_dch_common_list_info_struct_type *common_e_dch_resource_list;

/* Flag to indicate whether we are reverting back to old config */
  boolean revert_old_config;
  l1_rrc_config_rab_enum_type l1_rrc_config_rab;
  boolean is_mrab_ps_present; /* Indicates presence of multiple PS RABS */
} l1_setup_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_DRX_REQ

Used by RRC to command physical layer to begin DRX operations.  
--------------------------------------------------------------------------*/
typedef struct
{
  uint32       drx_cycle_len;
  /* The 2^K value of the DRX cycle length. */
  /* The UE will sleep for 2^K frames.      */
} l1_drx_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_CIPHERING_KEY_REQ
                       
Parameter structure for the CPHY_CIPHERING_KEY_REQ primitive.                       
--------------------------------------------------------------------------*/
typedef struct {
  l1_ciphering_key_req_type cipher_key_req;
} l1_ciphering_key_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_CELL_SELECTION_REQ

Used by RRC to request Cell Selection values S for specified cells.   
--------------------------------------------------------------------------*/
typedef struct
{
  /* Frequency specification */
  uint16 freq;

  /* Primary Scrambling Code Index */
  uint16 prim_code_inx;

} l1_cell_selec_cmd_type;



/*--------------------------------------------------------------------------
          CPHY_IMMEDIATE_MEAS_REQ

Used by RRC to request an immediate measurement report at FACH state. 
This is implemented as a function.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16  aset_scr_code;
    /* Scrambling code of the serving cell */
  l1_meas_quan_enum_type meas_quan;
    /* measurement quantity type */
  uint16 aset_meas_val;
    /* measurement value for the serving cell */

  uint16 num_ncells;
    /* Number of neighbors being reported  Upto 6 */
  struct
  {
      uint16 scr_code ;
        /* scrambling code for the cell  */
      l1_meas_quan_enum_type meas_quan;
        /* measurement quantity type */
      uint16 meas_val;
        /* measured value for the cell  */
      l1_sfn_report_enum_type sfn_rpt;
        /* SFN-SFN observed time diff reporting indicator */
      uint32 sfn_sfn_diff;
        /*  SFN-SFN difference  */
  } n_cells[ L1_ASET_MAX ];
} l1_immediate_meas_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_MEAS_CTRL_CNF
Used to indicate if meas ctrl message command from RRC is valid in response 
to a CPHY_MEASUREMENT_REQ. Status is true or false
---------------------------------------------------------------------------*/

typedef struct
{
   uint32 trans_id;
     /* Transaction Identifier */
   uint16 meas_id;
     /* Measurement Identity Number */
   boolean status;
     /* Status (Valid?), true or false */
   l1_failure_cause_enum_type error_cause;  
     /* Reason Invalid */

}  l1_meas_ctrl_cnf_type;


/*--------------------------------------------------------------------------
          CPHY_INTEGRITY_KEY_REQ

Used by RRC to configure DL/UL integrity keys. This command provides the 
key database and is set by RRC. The actual usage is also controlled by RRC, 
which will run integrity checks on all signaling messages on DCCH. The Key ID
will be referenced by the RRC when requesting integrity check to be run on 
a signaling message by the Physical Layer. 
--------------------------------------------------------------------------*/
typedef struct
{
  uint32      num_keys;
    /* Number of keys */

  struct
  {
    uint32     id;
      /* Key ID is used by RRC */
    uint32    dir;
      /* UL or DL */
    uint32   algo;
      /* Algorithm used */
    uint32    key;
      /* 128 bit key data */
  } keys[ L1_MAX_NUM_KEYS ];

} l1_intg_key_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_INTEGRITY_KEY_MAUC_REQ

This primitive is used to get the MAC/XMAC (UL/DL Message Authentication Code) 
generated at the receiver for the signaling message received on the DL. RRC 
will provide the parameters needed to generate the XMAC for the signaling 
message. 
--------------------------------------------------------------------------*/
typedef struct
{
  uint32       count_I;
    /* Derived from HFN and RRC sequence number */
  uint32       fresh;
    /* Number chosen by CN, prevents replay */
  uint32       *message;
    /* Pointer to the message over which integrity */
    /* check has to be performed                   */
  uint32       len;
    /* Length of the message */
  uint32       key_id;
    /* Enumerated Key ID. Determines algorithm/direction  */
    /* to use for this message                            */

} l1_intg_key_mauc_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_IMMEDIATE_RESELECTION_REQ

Used by RRC to trigger immediate cell reselection.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean cur_freq_excl;
    /* Indicates if the current frequency              */
    /*  is excluded from the immediate Cell Reselection. */

  boolean cur_cell_excl;
    /* Indicates if the current serving cell is     */
    /* excluded from the immediate cell reselection */
} l1_immediate_reselec_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_CELL_BAR_REQ

Used by RRC to bar a cell.
--------------------------------------------------------------------------*/

/* This is used to inform L1 if the GSM cell is barred, or WCDMA 
 * cell is barred, or WCDMA frequency is barred, or the active
 * set is barred
 */
typedef enum
{
  L1_WCDMA_NOT_BARRED,
  L1_GSM_CELL_BARRED,
  L1_WCDMA_PSC_BARRED,
  L1_WCDMA_FREQ_BARRED,
  L1_WCDMA_ASET_BARRED
  ,L1_EUTRA_CELL_BARRED
  ,L1_EUTRA_FREQ_BARRED
  ,L1_WCDMA_FREQ_UNBARRED
}l1_cell_bar_info_e_type;


typedef struct 
{/* ARFCN and BSIC uniquely identify GSM cell*/
  W_ARFCN_T                   bcch_arfcn;
  l1_gsm_bsic_struct_type     bsic;
}l1_gsm_cell_identity_struct;

typedef struct
{ /* Frequency and scrambling code uniquely identify FDD cell*/
  uint16 freq;
  uint16 scr_code;
}l1_fdd_cell_identity_struct;

typedef struct
{
  /* This is used to inform L1 if the GSM cell is barred, or WCDMA 
   * cell is barred, or WCDMA frequency is barred, or the active
   * set is barred
   */
  l1_cell_bar_info_e_type l1_cell_bar_info;   

  /* GSM cell identity or FDD cell identity*/
  union
  {
    l1_gsm_cell_identity_struct gsm;       
    l1_fdd_cell_identity_struct fdd;    
    l1_eutra_cell_info_type eutra;
  }id;

  /* T barred timer value */
  uint16 t_barred;                     
  /* Bar the frequency because of forbidden lai/non-eplmn*/
  boolean bar_for_resel_only;
  /* set to true to bar/unbar non white csg cells */
  boolean bar_non_white_csg;
  boolean non_eq_plmn_barred;
  /*If the frequency is restricted for operator use*/
  boolean freq_restricted_for_operator_use;
  boolean barred_due_to_band_mismatch;
  /*Set to true if there is a band mismatch, WL1 takes this
   boolean and indicates RRC whether ACQ failure is due to 
   band mismatch or not*/
}l1_cell_bar_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_ACT_TIME_CANCEL_REQ
Activation time cancel primitive command
--------------------------------------------------------------------------*/
typedef struct {
  uint8 seq_num;
    /* sequence number to cancel */
} l1_act_time_cancel_cmd_type;

/*--------------------------------------------------------------------------
          L1_UI_DISPLAY_INFO
This structure will be used by L1 to fill RSSI, RSCP, Ec/Io and its validity
when BER timer is expied
--------------------------------------------------------------------------*/
typedef struct
{
   /* this flag will hold the value false when L1 is doing ACQ (or) Freq scan */
  boolean     is_data_valid;
  /* Rx AGC value */
  int16       rssi_value;
  /* Ec/Io value */
  /* In DCH HS call Ec/Io, RSCP is of the HS cell only */
  int16       ecio_value; 
  /* RSCP value */
  int16       rscp_value;
 #ifdef FEATURE_UMTS_UI_REPORT_BLER 
  #error code not present
#endif
  /* this flag will be set during a data call */
  boolean       use_ecio_for_display;

  /* Scrambling code of the serving cell in the ASET */
  uint16          scr_code;
  /* camped frequency */
  uint16          freq;

  /*Holds the Pathloss value. It is valid only for DCH state*/
  int16 pathloss; 
  /*Holds the Sir value. It is valid only for DCH state*/
  int16 sir;

  /* Periodic Average CQI info */
  uint32 enchs_avg_cqi_val;
  /* Number of samples */
  uint16 enchs_avg_cqi_num_sample;
 
 } l1_ui_display_info_struct_type;

/*--------------------------------------------------------------------------
          L1_RSSI_INFO
This structure will be used by L1 to fill RSSI and its validity when BER timer is expied
--------------------------------------------------------------------------*/

typedef struct
{
 /* this flag will hold the value false when L1 is doing ACQ (or) Freq scan */
  boolean       is_valid_rssi;
  uint8           rssi_value;
  int16           ecio; 
  /* Scrambling code of the serving cell in the ASET */
  uint16          scr_code;
  /* camped frequency */
  uint16          freq;
  /* Periodic Average CQI info */
  uint32 enchs_avg_cqi_val;
  /* Number of samples */
  uint16 enchs_avg_cqi_num_sample;
  
/*Holds the Pathloss value. It is valid only for DCH state*/
  int16 pathloss; 
  /*Holds the Sir value. It is valid only for DCH state*/
  int16 sir;

#ifdef FEATURE_UMTS_UI_REPORT_BLER 
  #error code not present
#endif

 } l1_rssi_info_struct_type;


/*==========================================================================*/
/*=                                                                        =*/
/*=      DEFINITIONS OF ALL L1 INDICATIONS AND CONFIRMATIONS TO RRC        =*/
/*=                                                                        =*/
/*==========================================================================*/


/*--------------------------------------------------------------------------
          CPHY_MEASUREMENT_IND

Used by L1 to send RRC a measurement report message

All sections adhere to 03-2002 version of 25.331
--------------------------------------------------------------------------*/
/* event result structure */
typedef  struct
{
  l1_inter_sys_event_enum_type  event_id;
  uint8 num_freq;
  struct
  {
    uint32 earfcn;
    uint8 num_cell;
    uint16 phy_cell_id[L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
  }freqs[L1_MAX_REP_EUTRA_FREQ];  
}l1_eutra_event_results_struct_type;
 
/* Measurement result structure */
typedef struct
{
      uint16 phy_cell_id;
      boolean rsrp_incl;
      uint8 rsrp;
      boolean rsrq_incl;
      uint8 rsrq;
} l1_eutra_measured_result_cell_struct_type;
/* eutra measured cell structure */

typedef struct
{
  uint32 earfcn;
  uint8 num_cell;
  l1_eutra_measured_result_cell_struct_type cells[L1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
} l1_eutra_measured_result_freq_struct_type;
/* eutra measured frequency structure */    

typedef struct
{
  uint8 measured_eutra_freqs;
  l1_eutra_measured_result_freq_struct_type freqs[L1_MAX_REP_EUTRA_FREQ];
}l1_eutra_measured_results_struct_type;
/* eutra event results structure */

typedef struct
{
  /* event results*/
  l1_eutra_event_results_struct_type  evt_res;
  /* measurement results*/
  l1_eutra_measured_results_struct_type meas_res;
}l1_inter_sys_eutra_rpt_struct_type;

/*
 * 10.2.19  - Measurement Report Message
 */
typedef struct
{
  uint16  meas_id;  
    /* Measurement id: described in 10.3.7.48 range 1...16 */

  /* DJM - this info should be maintained by RRC, not L1??? */
  l1_meas_report_trans_mode_enum_type meas_transfer_mode;
    /* Describes report transfer criteria to UTRAN */

  l1_meas_report_mode_enum_type rpt_crit;
    /*  Describes the criterion of the  report Periodic/event triggered */

  boolean last_periodic_report;
    /* valid if rpt_crit is Periodic, ignored otherwise              */
    /* TRUE - Indicates to RRC that this is the last periodic report */
    /*        for this type of measurement, RRC should delete the    */
    /*        corresponding measurement id                           */

  l1_meas_type_enum_type  meas_type;
    /* Describes the type of measurement  */
 /*
  * Measured Results
  */
  union 
  {
    l1_intra_freq_meas_rpt_struct_type intra_f_rpt[MAX_NUM_CARR] ;
      /* Intra-frequency measurement report info 10.3.7.35  */
    l1_inter_freq_meas_rpt_struct_type inter_f_rpt ;
      /* Inter-frequency measurement report info 10.3.7.15  */
    l1_inter_sys_meas_rpt_struct_type inter_sys_rpt ;
      /* Inter-RAT measurement report info       10.3.7.26 */
    l1_inter_sys_eutra_rpt_struct_type inter_sys_eutra_rpt;
    l1_traf_vol_meas_rpt_struct_type traf_vol_rpt ;
      /* Traffic volume measurement report info  10.3.7.67 */
    l1_qual_meas_rpt_struct_type qual_meas_rpt ;
      /* Quality measurement report info         10.3.7.55 */
    l1_ue_internal_meas_rpt_struct_type ue_internal_meas_rpt;
      /* UE internal measurement report info     10.3.7.76 */
    l1_ue_positioning_measured_results_type ue_pos_meas_results;
      /* UE positioning measured results list    10.3.7.99 */
  } u;

  boolean addl_meas_info_incl;
    /* flag indicating if additional measured results are appended, always false at present */

  uint16 num_addl_meas;
    /* number of additional measurements */

} l1_meas_rpt_struct_type ;


/*--------------------------------------------------------------------------
          CPHY_CELL_RESELECTION_RSP

Used by RRC to respond to CPHY_CELL_RESELECTION_IND.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean camp_tg_cell;
    /* Indicates if the UE is to camps on the target cell */
  boolean camp_old_cell;
    /* Indicates if the UE keeps camping on the original serving cell */
  boolean tg_freq_excl;
    /* Indicates if the target frequency is to be       */
    /* excluded from future cell reselection evaluation */
  boolean tg_cell_excl;
    /* Indicates if the target cell is to be excluded */
    /* from future cell reselection evaluation        */
} l1_cell_reselect_rsp_cmd_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_ERROR_IND

Used by L1 to report errors in configuration, processing or sequencing.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_error_enum_type err;
    /* Error Indication */
  
  uint8              err_type;
    /* Mask indication error type */

  l1_cmd_enum_type   cmd_id;
    /* command id for which failure is being sent */

} l1_error_ind_type;

/* Return true if any of the STMR interrupts have failed status */
#define L1_ERROR_IND_STMR_FAILED( err_type )   err_type

/* Macro to find out the error type in the Error
 * Indication for a particular STMR interrupt.
 */
#define L1_ERROR_IND_STMR_INT_FAILED( err_type, intr ) \
  ( err_type & (1 << intr) )


/*--------------------------------------------------------------------------
          RRC_CPHY_IDLE_CNF 
                         
Used to confirm L1 is in Idle state with no active channels in response to 
a CPHY_IDLE_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  boolean status;
} l1_idle_cnf_type; 


/*--------------------------------------------------------------------------
          RRC_CPHY_DEACTIVATE_CNF
                         
Used to confirm that L1 has transitioned to the deactivated state in
response to the CPHY_DEACTIVATE_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_deactivate_cnf_type; 

typedef enum
{
  WL1_FAILED_NORMAL,
  WL1_FAILED_NO_WRM_LOCK,
  WL1_FAILED_BAND_MISMATCH,
  WL1_FAILED_INVALID_MAX
} wl1_common_fail_enum_type;

/*--------------------------------------------------------------------------
                         l1_stop_cnf_type 
                         
Used to confirm L1 is in STOPPED state in response to
CPHY_STOP_WCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{

#ifdef FEATURE_DUAL_SIM
  boolean status;
  /* Indicates if the acquisition succeeds. */

  wl1_common_fail_enum_type fail_type;
  /* Indicates the type of acq failure. */
#endif /*FEATURE_DUAL_SIM*/

  uint16 dummy;
} l1_stop_cnf_type;

/*--------------------------------------------------------------------------
                         l1_start_cnf_type 
                         
Used to confirm L1 has started and is in IDLE state in response to
CPHY_START_WCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{

#ifdef FEATURE_DUAL_SIM
  boolean              status;
    /* Indicates if the acquisition succeeds. */

  wl1_common_fail_enum_type fail_type;
    /* Indicates the type of acq failure. */
#endif /*FEATURE_DUAL_SIM*/

  uint16 dummy;
} l1_start_cnf_type;

/*--------------------------------------------------------------------------
                         l1_suspend_cnf_type 
                         
Used to confirm L1 is in SUSPENDED state in response to
CPHY_SUSPEND_WCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{

#ifdef FEATURE_DUAL_SIM
  /* Indicates the type of acq failure. */
  wl1_common_fail_enum_type fail_type;
#endif /*FEATURE_DUAL_SIM*/

  /* TRUE indicates suspend was successful. */
  boolean status;
} l1_suspend_cnf_type;

/*--------------------------------------------------------------------------
                         l1_resume_cnf_type 
                         
Used to confirm L1 is in RESUMED state in response to
CPHY_RESUME_WCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{

#ifdef FEATURE_DUAL_SIM
  /* Indicates the type of acq failure. */
  wl1_common_fail_enum_type fail_type;
#endif /*FEATURE_DUAL_SIM*/

  /* TRUE indicates resume was successful. */
  boolean status;
} l1_resume_cnf_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_FREQ_SCAN_CNF

Used by Layer 1 to report frequency scan results back to RRC.
--------------------------------------------------------------------------*/

/* The results of frequency scan reported to RRC for each of the scan types
   are as follows:
   
       1. For a RAW scan, the report consists of all center frequencies
          whose measured AGC RSSI values are above the pre-specified 
          threshold, along theire AGC RSSI's. The frequecy list is sorted 
          on AGC RSSI, with the strongest first.
          
       2. For a FINE scan, the report consists of all center frequencies
          whose measured AGC RSSI values are above the pre-specified 
          threshold, along theire AGC RSSI's. The frequecy list is sorted 
          on AGC RSSI, with the strongest first.
 */

/* First, scan result structure for both raw and fine scan */
typedef struct
{
  uint16  ctr_freq;
  int16  rssi;
  uint8  result_plmn_type;
} l1_freq_scan_result_struct;

/* Finally, Frequency Scan Confirmation Type */
typedef struct
{

#ifdef FEATURE_DUAL_SIM
  boolean              status;
    /* Indicates if the acquisition succeeds. */

  wl1_common_fail_enum_type fail_type;
    /* Indicates the type of acq failure. */
#endif /*FEATURE_DUAL_SIM*/

  l1_freq_scan_step_enum_type  step;
    /* Indicates which type of scan (RAW or FINE) the result is for */

  uint16  num_freq;
    /* Number of center frequencies reported */

  l1_freq_scan_result_struct  freq_result[L1_MAX_SCAN_FREQ];
    /* Center frequencies and their AGC RSSI's */

} l1_freq_scan_cnf_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_ACQ_CNF

Used to indicate acquisition results during initial cell search in response 
to a CPHY_ACQ_REQ.
---------------------------------------------------------------------------*/
 

/* Number of scrambling codes to be returned in the CPHY_ACQ_CNF to RRC
 * in response to acquisition mode type L1_WCDMA_ACQ_DET_LIST.
 * This value is currently set to 5 because 6 step 1 peaks are inputs
   to step 2 search, in the case of non-power up/OOS acquisitions.
 */
#define L1_WCDMA_ACQ_DET_LIST_MAX   5 

/* Structure containing the scrambling code and pn position of
 * the cell that was also detected on the same frequency as the
 * cell that was currently acquired.
 *
 * Used only if acquisition mode type is L1_WCDMA_ACQ_DET_LIST.
 */
typedef struct
{
  /* Scrambling code of detected cell */
  uint16                scr_code;

  /* PN position of the cell in chipx1 units. */
  uint32                  pn_pos;

  uint32 tot_eng;      /* might be needed in ranking (or) for filtering later to be used as prev_filt_eng  */
  int16 ecio_2x;          /* EcIo converted from tot_eng */   
  int16 rscp;          /* RSCP calculated from EcIo and RxAGC */ 

} l1_acq_scr_code_det;
 

typedef struct
{
  boolean              status;
    /* Indicates if the acquisition succeeds. */

  wl1_common_fail_enum_type fail_type;
    /* Indicates the type of acq failure. */

  uint16               freq;
    /* Returns the strongest detected Primary scrambling */
    /* code index, if acquisition succeeds.              */

  uint16                scr_code;
    /* the scrambling code of the acquired cell */

  uint32              pn_pos;
    /* the PN POS of the acquired cell */

  boolean sttd;
    /* the detected CPICH STTD usage of the acquired cell */

  uint8                num_scr_code_det;
    /* Number of scrambling codes detected.
     * Used only if acquisition mode type was L1_WCDMA_ACQ_DET_LIST
     */

  l1_acq_scr_code_det scr_code_det[L1_WCDMA_ACQ_DET_LIST_MAX];
    /* Cells that also passed pullin during acquisition.
     * Used only if acquisition mode type was L1_WCDMA_ACQ_DET_LIST
     */

  uint32 tot_eng;      /* might be needed in ranking (or) for filtering later to be used as prev_filt_eng  */
  uint32 sfn_extended; /* #define DL_BMC_MAX_EXTENDED_SFN 1048576 */
  int16  rx_agc;       /* RxAGC value read during ASF search */
  int16 ecio_2x;          /* EcIo converted from tot_eng */   
  int16 rscp;          /* RSCP calculated from EcIo and RxAGC */ 
} l1_acq_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_DRX_CNF

Used to indicate results from drx setup in response to a CPHY_DRX_REQ
---------------------------------------------------------------------------*/
typedef struct
{
  boolean              status;
    /* Indicates if the drx parameters were valid */

} l1_drx_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_SETUP_CNF

Used to confirm that L1 has completed all of the requested physical channel 
setup in the last CPHY_SETUP_REQ.
--------------------------------------------------------------------------*/
/*
 * RRC_CPHY_SETUP_CNF setup status identifier
 */
typedef enum
{
  L1_CPHY_SETUP_CNF_STAT_TYPE_DL_TRCH,
  L1_CPHY_SETUP_CNF_STAT_TYPE_DL_PHYCHAN,
  L1_CPHY_SETUP_CNF_STAT_TYPE_UL_TRCH,
  L1_CPHY_SETUP_CNF_STAT_TYPE_UL_PHYCHAN,
  L1_CPHY_SETUP_CNF_NUM_STAT_TYPE
} l1_cphy_setup_cnf_stat_type_enum_type;


#define CPHY_SETUP_CNF_OPS_MASK 0x00000003
#define CPHY_SETUP_CNF_OPS_STAT_MASK 0x00000004

#define CPHY_SETUP_CNF_DL_PHYCHAN_OFFSET(phch) \
  l1_cphy_setup_cnf_dl_phychan_offset[phch]
  
#define CPHY_SETUP_CNF_DL_TRCH_OFFSET(trch) \
  l1_cphy_setup_cnf_dl_trch_offset[trch]
  
#define CPHY_SETUP_CNF_UL_PHYCHAN_OFFSET(phch) \
  l1_cphy_setup_cnf_ul_phychan_offset[phch]
  
#define CPHY_SETUP_CNF_UL_TRCH_OFFSET(trch) \
  l1_cphy_setup_cnf_ul_trch_offset[trch]
  
#define CPHY_SETUP_CNF_SET_OP(type, op, offset) \
  (type = (type & ~(CPHY_SETUP_CNF_OPS_MASK << offset)) | \
  (((uint32) (op & CPHY_SETUP_CNF_OPS_MASK)) << offset))
  
#define CPHY_SETUP_CNF_SET_OP_STAT(type, offset, stat) \
  (type = (type & ~(CPHY_SETUP_CNF_OPS_STAT_MASK << offset)

typedef enum
{
  L1_SETUP_STATUS_VALIDATION_FAIL,
  L1_SETUP_STATUS_SETUP_FAIL,
  #ifdef FEATURE_DUAL_SIM
  L1_SETUP_STATUS_WRM_LOCK_FAIL,
  #endif
  L1_SETUP_STATUS_SUCCESS
} l1_setup_cnf_status_enum_type;
typedef struct
{
  uint32 dl_phychan_status;
    /* status of the DL phychan ops and channels */

  uint32 dl_trch_status;
    /* status of the DL TrCH ops and channels */

  uint32 ul_phychan_status;
    /* status of the UL phychan ops and channels */

  boolean sec_ul_phychan_status;
    /* status of the Sec UL phychan ops and channels */

  uint32 ul_trch_status;
    /* status of the UL TrCH ops and channels */

  uint32 compressed_mode_status;
    /* djm - status of the Compressed Mode TGPS Configuration */

  boolean hs_chan_status;

    /* Combined status of the E_DL and E_UL operations;
        Takes a value of FALSE if E_DL or E_UL operation fails; Else, TRUE*/
  boolean eul_chan_status;

  /*Combined status of the Sec EDL and EUL operations; 
    Takes a value of FALSE if Sec E_DL or E_UL operation fails; Else, TRUE*/
    
  boolean sec_eul_chan_status;

  boolean hs_rach_setup_status;

  boolean status;
    /* did the command pass or fail overall */

  boolean send_phy_est_ind;
    /* default value FALSE. 
     * While CPHY_SETUP is not yet completed, if PHY_EST_IND need to be sent, this
     * flag will be set to TRUE. This will help in sending the PHY_EST_IND after
     * CPHY_SETUP_CNF to RRC */
  boolean phy_est_ind_result;
    /* valid only if send_phy_est_ind is TRUE
     * type of result to be sent as part of PHY_EST_IND
     * TRUE for success and FALSE for unsuccessful establishment */

#ifdef FEATURE_WCDMA_DC_HSUPA
  boolean send_phy_est_ind_fail_on_sec;
    /* type of result to be sent as part of PHY_EST_IND
     * FALSE for success and TRUE for unsuccessful establishment 
     * Intialised to FALSE and only used for failure scenario*/
#endif /* FEATURE_WCDMA_DC_HSUPA */

  l1_setup_cnf_status_enum_type status_type;
  /* Indicate the status type validation failures or 
     setup failures or setup success */
} l1_setup_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_CELL_SELECTION_CNF

Used to indicate S values in response to a CPHY_CELL_SELECTION_REQ.
--------------------------------------------------------------------------*/

typedef struct
{
  uint16     freq;
    /* Frequency specification */

  uint16   pri_scr_code;
    /* Primary Scrambling Code Index */

  boolean  evaluated;
    /* Flag indicating if the cell is searched or not.    */
    /* Currently, if the cell timing is unknown, the cell */
    /* will not be searched for cell selection evaluation */

  int16    s_qual;
    /* Cell Selection quality Value */

  int16    s_rxlev;
    /* Cell Selection RX Level Value */

  uint16   q_map;
    /* Quality measured during Cell Selection Evaluation. */
    /* A bigger number means a higher energy meaurement.  */

} l1_cell_select_cnf_type;


/*--------------------------------------------------------------------------
          CPHY_ACT_TIME_CANCEL_CNF

Activation time cancel confirm primitive
--------------------------------------------------------------------------*/
typedef struct {
  uint8 seq_num;
    /* sequence number of the activation time command */
  boolean status;
    /* whether cancellation was successful */
} l1_act_time_cancel_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_CELL_RESELECTION_IND

Used to indicate cell reselection results.
--------------------------------------------------------------------------*/
/*
 * Cell Reselection Indication Information
 */
typedef struct
{
  l1_cell_resel_status_enum_type     status;  
  l1_cell_resel_cell_enum_type       cell;  
  /* flag to indicate whether the reselection triggered is on a CSG cell or not*/
  boolean csg_resel; 

  union
  {
    l1_cell_resel_wcdma_info_type   wcdma_info;  
      /* WCDMA Cell Reselection Cell Information */
    l1_gsm_cell_resel_ind_info_type  gsm_info;    
      /* GSM Cell Reselection Cell Information */
    l1_cell_resel_wcdma_info_type   is2000_info;
      /* IS2000 Cell Reselection Cell Information */
    l1_eutra_cell_resel_ind_info_type eutra_info;
      /*E-UTRA cell reselection information*/
  } u;

} l1_cell_reselec_ind_type;


typedef enum
{
  L1_RESEL_FAIL_CAUSE_NONE,
  L1_RESEL_FAIL_CAUSE_INTERNAL, 
  /*   Frequency and scrambling code returned by SIB in confirm is the different than what was passed to SIB Module
        Failure finding cell access status
        Failure filling acq. Entry
        Duplicate cell reselection ind received.
        Cell System not supported
  */
  L1_RESEL_FAIL_CAUSE_RRC_PROC_ACTIVE,
  /*   GtoW CCO in Progress
        RCE in progress
        BPLMN Search in Progress
   */
  L1_RESEL_FAIL_CAUSE_S_CRITERIA,
  /*   S-criteria not met for nbr cell*/
  L1_RESEL_FAIL_CAUSE_BARRED_CELL,
  /*   During resel no cell access on ncell
        Cell below expected(forbidden/barred cell)
   */
  L1_RESEL_FAIL_CAUSE_SIB_READING_FAILED,
  /*   SIB Cnf failed/SIB timeout/Invalid SIB’s*/
  L1_RESEL_FAIL_CAUSE_DL_WK_IND,
  /*   DL_WK_IND for neighbour cell*/
  L1_RESEL_FAIL_DL_CH_FAILURE,
  /* Channel setup parameters arent as expected */
  L1_RESEL_FAIL_PIGGY_BACK_RESEL,

  L1_RESEL_FAIL_L2_ACK_WAIT,
  /* We didn't get the lock for NBCH setup */
  L1_RESEL_FAIL_NBCH_FAILURE,
  /* 1. if resel is triggered to a cell of diff LAI/RAI while MT call is in progress  
     2. Frequency and scrambling code returned by SIB in confirm is the different than what was passed to SIB Module
     3. Failure finding cell access status  */
  RRC_RESEL_FAIL_CAUSE_INTERNAL 

}l1_resel_fail_cause_enum_type;
/*--------------------------------------------------------------------------
          CPHY_CELL_TRANSITION_REQ

Used by RRC to send a cell transition command
--------------------------------------------------------------------------*/
typedef struct
{
  /* cell type */
  l1_cell_resel_cell_enum_type   cell_type;

  /* GSM cell info -- for cell transition failure case */
  l1_gsm_cell_identity_struct    gsm_info;

  l1_aset_update_enum_type       select_type;
    /* type of ASET update */

  uint16 scr_code;
    /* scrambling code */

  boolean tg_freq_excl;
    /* Only applicable during reselection failure */
    /* Indicates if the target frequency is to be */
    /* excluded from future cell reselection evaluation  */

  boolean tg_cell_excl;
    /* Indicates if the target cell is to be excluded */
    /* from future cell reselection evaluation        */

  boolean pccpch_tx_div_ind;

  /*reselection fail cause*/
  l1_resel_fail_cause_enum_type resel_fail_cause; 

}l1_cell_trans_params_struct_type;

    
typedef struct
{
  l1_cell_trans_params_struct_type cell_trans_params;
  l1_setup_cmd_type               setup_cmd;
} l1_cell_transition_cmd_type;

/*--------------------------------------------------------------------------
           CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP
 
 Used by RRC to send a CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP
--------------------------------------------------------------------------*/
typedef struct
{
  /*TRUE:  RRC accepted RRC_NUDGE_FOR_SFN_MISS_MATCH_REQ from L1
   *FALSE: RRC is not in position to accept RRC_NUDGE_FOR_SFN_MISS_MATCH_REQ from L1
   */
  boolean status;
} l1_rrc_nudge_update_cmd_type;

/*--------------------------------------------------------------------------
           CPHY_GET_MODEM_STATISTICS_REQ
 
 Used by RRC to send a CPHY_GET_MODEM_STATISTICS_REQ
--------------------------------------------------------------------------*/
#define WCDMA_GET_SERVING_CELL_POWER_INFO 0x01
#define WCDMA_GET_ACTIVE_SET_INFO 0x02
#define WCDMA_GET_MONITORED_SET_INFO 0x04


typedef struct
{
  /*GET_SERVING_CELL_POWER_INFO:  RRC request SERVING_CELL_POWER_INFO
   *GET_ACTIVE_SET_INFO: RRC request ACTIVE_SET_INFO
   *GET_MONITORED_SET_INFO: RRC request MONITORED_SET_INFO
   */
  uint8 modem_stat_req_bmsk;
} l1_get_modem_stat_req_cmd_type;

typedef struct
{
  wl1_common_fail_enum_type oos_cause;
} l1_out_of_service_type;

/*--------------------------------------------------------------------------
           RRC_CPHY_OPERATIONAL_ERROR_IND
Used to indicate an error condition within Layer 1 to upper layers

--------------------------------------------------------------------------*/
typedef enum 
{
  START_AGC_MDSP_BAD_STATUS = 0,
  START_AGC_MDSP_TIMEOUT,
  GOTO_SLEEP_MDSP_BAD_STATUS,
  GOTO_SLEEP_MDSP_TIMEOUT,
  MDSP_SYNC_SEND_BUF_OVERFLOW,
  SRCH_CMD_TIMEOUT, 
  L1_STUCK_IN_L1M_IDLE,
  NUM_L1_OP_ERROR_SOURCES
} l1_op_err_source_type;

typedef struct 
{
  l1_op_err_source_type src;
} l1_op_err_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_IMMEDIATE_CELL_EVAL_CNF
Used to confirm results from immediate cell evaluation in response to a 
CPHY_IMMEDIATE_CELL_EVAL_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_immediate_cell_eval_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_SET_SFN_CNF

This confirms that SFN synchronization has been performed by L1 and 
indicates the SFN that frames should show synchronization by. Sent in 
response to a CPHY_SET_SFN_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32  sfn;
} l1_set_sfn_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_SET_CFN_CNF

This confirms that CFN synchronization has been performed by L1 and indicates 
the CFN that frames should show synchronization by. Sent in response to a 
CPHY_SET_CFN_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_set_cfn_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_MEASUREMENT_IND

Used to report Measurement results.
--------------------------------------------------------------------------*/
typedef struct
{
  l1_meas_rpt_struct_type *meas_rpt;
} l1_measurement_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_CAPABILITY_CNF

Used by the Physical Layer to report L1 capability.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_capability_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_INTG_KEY_MAUC_CNF

Used by the Physical Layer to report the Message Authentication Code 
(MAC/XMAC for Uplink/Downlink) of the signaling message for which an 
integrity check was performed upon request from RRC.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_intg_key_mauc_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_OUT_OF_SYNC_IND

Used by L1 to report that the radio link has gone out of sync according to 
the criteria dictated by 25.214 section 4.3.1.2.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_out_sync_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_IN_SYNC_IND

Used by L1 to report that the radio link has gone in sync according to the 
criteria dictated by 25.214 section 4.3.1.2.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_in_sync_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_PHYCHAN_ESTABLISHED_IND

Used by L1 to fulfil criteria of N312 and section 8.5.4 of RRC spec. Should 
only be expected when the DPCCH is first setup.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
    /* physical channel establish indication */
} l1_phychan_est_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_IMMEDIATE_RESELECTION_CNF

Used by L1 to return cell reselection results in response to a 
CPHY_IMMEDIATE_RESELECTION_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_immediate_reselec_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_IMMEDIATE_MEAS_CNF

Used by L1 to respond to RRCs CPHY_IMMEDIATE_MEAS_REQ primitive.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} l1_immediate_meas_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_CELL_TRANSITION_CNF

 Used by L1 to respond to RRCs CPHY_CELL_TRANSITION_REQ primitive
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
  /* Status indicating whether the cell transition was successful or not  */
  
  l1_cell_reselec_ind_type        cell_rsel_ind;
  /* Reselection Indication Piggyback */
} l1_cell_transition_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_COMPRESSED_MODE_IND
Used to indicate if meas ctrl message command from RRC is valid in response 
to a CPHY_MEASUREMENT_REQ. Status is true or false
---------------------------------------------------------------------------*/
typedef struct
{
   uint16 meas_id;
     /* Measurement Identity Number */
   uint16 tgpsi;
    /* Transmission Gap Pattern Identifier */

}  l1_compressed_mode_ind_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_DL_WEAK_IND

Used to indicate a weak DL (for serving or neighbor PCCPCH)
--------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates whether the indication is for serving or neighbor cell */
  boolean     serving_cell;  

#ifdef FEATURE_DUAL_SIM
  /*Set to TRUE if DL WK indication is a result of no lock*/
  boolean     no_resource;
#endif

} l1_dl_weak_ind_type;

#define CPHY_HIGH_MOBILITY_INCL 0x01
typedef struct
{
  uint8 bit_mask;
  boolean high_mobility_ind;
} l1_upd_ota_parms_ind_cmd_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_UPD_DL_SYNC_PARMS_CMD
Used to send the latest DL sync parameters received in a UTRAN Mobility 
Information message.
--------------------------------------------------------------------------*/
typedef struct {
  uint16 N315; 
  uint16 N312; 
  uint16 N313;
  uint8   T312; 
  uint8   T313; 
} l1_upd_dl_sync_parms_cmd_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_MODEM_STATISTICS_RSP

  Confirmation of MODEM_STATISTICS_RSP from L1
--------------------------------------------------------------------------*/

typedef struct {
  l1_serving_cell_power_info_type *l1_serving_cell_power_info;
  l1_active_set_info_type *l1_active_set_info;
  l1_monitored_set_info_type *l1_monitored_set_info;
} l1_modem_stat_rsp_cmd_type;

/*--------------------------------------------------------------------------
          CPHY_CHECK_S_CRIT_REQ

Used to check if S criteria is satisfied
--------------------------------------------------------------------------*/
typedef struct
{
  int16 q_qualmin;
  int16 q_rxlevmin;
  int16 ue_tx_max_pwr;
} l1_s_criteria_req_type;
/*--------------------------------------------------------------------------
          RRC_CPHY_CHECK_S_CRIT_CNF

Used to indicate status if S Criteria is valid in response to CPHY_CHECK_S_CRIT_REQ
Status is TRUE or FALSE.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
} l1_s_criteria_cnf_type;

/*--------------------------------------------------------------------------
          CPHY_CHECK_SLEEP_RSP

Used to indicate if L1 can go to sleep in response to RRC_CPHY_CHECK_SLEEP_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  boolean go_to_sleep; /* TRUE indicates that L1 can go to sleep */
  boolean wtog_bplmn_in_progress;
  boolean wtow_bplmn_in_progress;
} l1_sleep_rsp_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_CHECK_SLEEP_REQ

Used to check if L1 can go to sleep
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 timer_val;
}l1_sleep_req_type;

/*--------------------------------------------------------------------------
           CPHY_ADDITIONAL_MEAS_DATA_REQ
 
 Used by RRC to send a CPHY_ADDITIONAL_MEAS_DATA_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  uint32                  meas_id;
  l1_meas_type_enum_type  meas_type;
  uint8                   carr_idx;
} l1_additional_meas_info_req_type;

/*--------------------------------------------------------------------------
           CPHY_GET_IMM_MEAS_REQ
 
 Used by RRC to send a CPHY_GET_IMM_MEAS_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  l1_meas_quan_enum_type   meas_quan;
  l1_meas_type_enum_type   meas_type;
} l1_process_imm_meas_req_type;

/*--------------------------------------------------------------------------
           RRC_CPHY_GET_IMM_MEAS_RSP
 
 Used by L1 to send a RRC_CPHY_GET_IMM_MEAS_RSP
--------------------------------------------------------------------------*/
typedef struct
{
  l1_immediate_meas_type  *immed_meas;
} l1_process_imm_meas_cnf_type;

/*--------------------------------------------------------------------------
           CPHY_NMR_INFO_REQ
 
 Used by RRC to send a CPHY_NMR_INFO_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  l1_rrc_utran_nmr_report_type report_type;
} l1_utran_nmr_info_req_type;

/*--------------------------------------------------------------------------
           RRC_CPHY_UTRAN_NMR_INFO_CNF
 
 Used by L1 to send a RRC_CPHY_UTRAN_NMR_INFO_CNF
--------------------------------------------------------------------------*/
typedef struct
{
  l1_meas_utran_nmr_info_union_type *l1_meas_utran_nmr_info;
  boolean                            status;
} l1_utran_nmr_info_cnf_type;

/* Support for EUTRA Detection. These enums will be returned to RRC when 
   the API 'rrc_l1_get_eutra_detect_status()' is called */
typedef enum
{
  EUTRA_CELL_DETECT_NOT_STARTED,
  EUTRA_CELLS_DETECTED,
  EUTRA_CELLS_NOT_DETECTED
}l1_eutra_det_state_enum_type;


typedef struct
{
  boolean   scaling_factor_valid;
  uint32    scaling_factor;
  boolean   skip_meas_info_valid;
  boolean   skip_meas_info;
} l1_m2m_cmd_type;


/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION  L1_RRC_IS_COMMON_EDCH_RSRC_ACTIVE

DESCRIPTION:
  This function should return true when a common EDCH resource is allocated to the UE
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -> common EDCH rsrc is allocated
  FALSE -> common EDCH rsrc is not allocated
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_rrc_is_common_edch_resrc_active(void);

/*===========================================================================
FUNCTION  RRC_L1_GET_EUTRA_DETECT_STATUS

DESCRIPTION:
  This function is called by RRC every 3 sec to get the EUTRA detection status.
  8.6.2.5 E-UTRA detection
  If the IE "E-UTRA detection" is included in a received message and set to TRUE and the UE is in CELL_PCH,
  URA_PCH state or idle mode, the UE may detect the presence of a E-UTRA cell on a frequency with a priority lower
  than the current UTRA cell and report the information to the NAS.

DEPENDENCIES
  None.

RETURN VALUE
  EUTRA_CELL_DETECT_NOT_STARTED
  EUTRA_CELLS_DETECTED
  EUTRA_CELLS_NOT_DETECTED

SIDE EFFECTS
  None
===========================================================================*/
extern l1_eutra_det_state_enum_type rrc_l1_get_eutra_detect_status(void) ;

/*===========================================================================
FUNCTION        L1_GET_RSSI

FILE NAME       dldem.c

DESCRIPTION     This function returns the Received Signal Strength Indication
                for use by the UI and Call Manager subsystems

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    The measured signal level shall be sent with no sign

SIDE EFFECTS    None
===========================================================================*/
void l1_get_rssi(l1_rssi_info_struct_type *rssi);

/*===========================================================================

FUNCTION       L1_GET_MOBILITY_INFO

DESCRIPTION
   This function returns the mobility state of UE to RRC.
   
DEPENDENCIES
  None.

RETURN VALUE   
  TRUE if UE is in High mobility state
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_get_mobility_info(void);

/*=========================================================================

FUNCTION     : RRC_GET_HSUPA_CATEGORY

DESCRIPTION  : RRC returns the HSUPA Category to L1. RRC reads NV to determine 
               UE CAT (valid NV value, invalid NV value, NV not present, 
               NV reading failure)  

DEPENDENCIES : None

RETURN VALUE : HSUPA Category value 1..6

SIDE EFFECTS : None

=========================================================================*/

extern uint16 rrc_get_hsupa_category(void);

/*=============================================================================
  FUNCTION     : L1_GET_EUTRA_MEASUREMENT_DATA
 
  DESCRIPTION  : This function will accept an LTE IRAT measurement data
                 structure pointer.  It will fill in the structure with
                 all relevant cell measurement information.

                 This function is used by RRC to retrieve timing information
                 for known LTE cells when performing Hard Handover to LTE.
                 This timing information is given to RRC for use in LTE
                 acquisition.

  DEPENDENCIES : None

  RETURN VALUE : None

  SIDE EFFECTS : None
=============================================================================*/
void l1_get_eutra_measurement_data(lte_irat_timing_tx_info_s* p_msmt_data);

/*=========================================================================

FUNCTION     : L1_GET_INTERRAT_MEASUREMENT_DATA

DESCRIPTION  : This function will accept an inter-rat measurement data
               structure pointer.  It will fill in the structure with
               all relevant inter-rat cell measurement information.

DEPENDENCIES : None

RETURN VALUE : Contents of rrc_rr_interrat_measurement_data_type struct

SIDE EFFECTS : None

=========================================================================*/
void l1_get_interrat_measurement_data( rrc_rr_interrat_measurement_data_type * p_msmt_data);

/*===========================================================================
FUNCTION     L1_GET_PERIODIC_INTER_RAT_MSMT_RESULTS

DESCRIPTION
  This function helps to build a measurement report message when
  periodic reporting triggers that a report should be sent.  This function
  provides the Inter-RAT addl_measurement information.

DEPENDENCIES
  None

RETURN VALUE
  True if information is available, False otherwise

SIDE EFFECTS
  WCDMA_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation, remove int
  blocking when L1 performs the call, in future.
===========================================================================*/
boolean l1_get_periodic_inter_rat_msmt_results
          (
            uint32  meas_id,
            l1_inter_sys_meas_result_struct_type *p_msmtres
          );

/*====================================================================
FUNCTION: rrc_ul_tx_pwr_ind()

DESCRIPTION:
  This function places the values of cpich tx power, ul interference,
  and a constant value defined in PRACH sys info, into the addresses
  passed into this function.

DEPENDENCIES:
  Implemented in RRC

RETURN VALUE:
  Boolean TRUE when UE is campled and not in DCH state
  FALSE otherwise

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrc_ul_tx_pwr_ind
(
  int8    *cpich_tx_pwr,
  int8    *ul_if,
  int8    *const_val
);

/*====================================================================
FUNCTION: l1_get_stored_intra_freq_meas()

DESCRIPTION:
  This function indicates l1 to save intra freq measurement criteria
  based on the value of measurement identity. 

DEPENDENCIES:
  -

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
extern void l1_get_stored_intra_freq_meas
(
 uint16 meas_id
);

/*====================================================================
FUNCTION: srchmeas_set_inter_freq_measdb_status_during_state_transition()

DESCRIPTION:
  Through this function RRC indicates to Layer1 whether to save inter freq 
  measurement database during a state transition. 
  Measurements should only be saved during a (DCH -> FACH -> DCH) 
  transition.
  
DEPENDENCIES:
  -

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
extern void srchmeas_set_inter_freq_measdb_status_during_state_transition(boolean status);

/*===========================================================================
FUNCTION     L1_GET_ADDITIONAL_INTRA_FREQ_MSMT_RESULTS

DESCRIPTION
  This function returns the intra freq measurement results as part
  of additional measurement info.
  
DEPENDENCIES
  None

RETURN VALUE
  False if meas id not present or meas_id is for non-intra freq
        or if the reporting crit is event triggered
  True otherwise.
  
SIDE EFFECTS
  WCDMA_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean l1_get_additional_intra_freq_msmt_results
          (
            uint8 freq_idx,
            uint32  meas_id,
            l1_intra_freq_meas_rpt_struct_type *p_msmtres
          );

/*===========================================================================
FUNCTION     L1_GET_ADDITIONAL_INTERF_MSMT_RESULTS

DESCRIPTION
  This function returns the inter freq measurement results as part
  of additional measurement info.

DEPENDENCIES
  None

RETURN VALUE
  False if meas id not present or meas_id is for non-inter freq
        or if the reporting crit is event triggered
  True otherwise.

SIDE EFFECTS
  WCDMA_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean l1_get_additional_interf_msmt_results(
            uint32  meas_id,
            l1_inter_freq_meas_rpt_struct_type *p_msmtres );


/*===========================================================================
FUNCTION     L1_GET_ADDITIONAL_INTERNAL_MSMT_RESULTS


DESCRIPTION
  This function returns the internal measurement results as part
  of additional measurement info.


DEPENDENCIES
  None


RETURN VALUE
  False if meas id not present or meas_id is for non-internal
        or if the reporting crit is event triggered
  True otherwise.


SIDE EFFECTS
  WCDMA_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean l1_get_additional_internal_msmt_results(
            uint32  meas_id,
            l1_ue_internal_meas_rpt_struct_type *p_msmtres );

/*===========================================================================
FUNCTION     rrc_is_gsm_cell_present_in_sibs


DESCRIPTION
  This function returns TRUE if GSM cell are present in SIB11 else
  return FALSE. Called only by L1 in its context.


DEPENDENCIES
  None


RETURN VALUE
  True/False


SIDE EFFECTS
  WCDMA_INTFREE()/WCDMA_INTLOCK() while L1 performs this operation.
===========================================================================*/
extern boolean rrc_is_gsm_cell_present_in_sibs(void);

/*===========================================================================
FUNCTION        l1_wcdma_get_trk_lo_adj_pdm

DESCRIPTION
  This function may be invoked by RRC to grab the mDSP's current VCTCXO
  adjustment PDM value.  It returns the value from the register
  tcxoAdjAccumMsw.  The tcxoAdjAccumLsw register is not read.

  It may not be able to read the tcxoAdjAccumMsw register because the WCDMA
  mDSP image is not active, or the mDSP clock is not enabled.  In this case
  it returns either a previously read value if there is one or a default
  value if there is not.  It also returns a previously read or default value
  if we do not have WCDMA service.

  This value may then get passed into another RAT, like GSM, so the VCTCXO
  trk_lo_adj PDM can start at this previously good, tracking PDM value.  If
  VCTCXO has never tracked, the return value is unlikely to be of much help.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - Latest VCTCXO adjustment PDM value, read straight from mDSP's
           tcxoAdjAccumMsw register.  If it can.  Otherwise, it returns
           a default or previous PDM value.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 l1_wcdma_get_trk_lo_adj_pdm(void);

/*===========================================================================

FUNCTION     L1_GET_REFERENCE_CELL_INFO

DESCRIPTION
  This function gives subsystems outside L1 (or inside) access to
  information about the current reference cell in DCH or serving cell in
  other states.  It only provides the reference or serving cell's
  frequency and primary scrambling code if the mobile is in a state where
  it actually has a reference or serving cell.

DEPENDENCIES
  None.

PARAMETERS
  uint16 *freq - Reference or serving cell's frequency.
  uint16 *psc  - Reference or serving cell's primary scrambling code.

RETURN VALUE
  TRUE  - Mobile has a reference or serving cell.
          *freq and *psc are valid.
  FALSE - Mobile has no reference or serving cell.
          *freq and *psc are invalid.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean l1_get_reference_cell_info(uint16 *freq, uint16 *psc);

/*===========================================================================
FUNCTION     MDSP_IS_WCDMA_IMAGE_LOADED

DESCRIPTION
  This function returns whether or not WL1 believes the mDSP is running WCDMA
  firmware.  Of course mDSP services should be the subsystem providing this
  information to callers because it knows for sure.  But for some reason it's
  needed here.  One of the purposes is to determine the active RAT at fatal
  error time to get the PA turned off properly.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if WL1 has informed mDSP services to enable WCDMA.
            FALSE if WL1 has informed mDSP services to disable WCDMA.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mdsp_is_wcdma_image_loaded(void);

/*====================================================================
FUNCTION: RRC_QUERY_CRNTI

DESCRIPTION:
  This function queries RRC for C_RNTI existence and value. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  returns TRUE if C_RNTI is valid and the value is *c_rnti

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrc_query_crnti
(
  uint16 *c_rnti
);

/*====================================================================
FUNCTION: L1_DISABLE_UL_TFC_PWR_RESTRICTION

DESCRIPTION:
  This function disables UL TFC power restriction dynamically when
  AMR is setup or added for DCH. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 will not restrict any UL TFC for MAX TX power limitation in DCH.
====================================================================*/
void l1_disable_ul_tfc_pwr_restriction(void);

/*====================================================================
FUNCTION: L1_ENABLE_UL_TFC_PWR_RESTRICTION

DESCRIPTION:
  This function allows UL TFC power restriction dynamically when
  CS/PS data call is setup or AMR is dropped from concurrent services
  for DCH. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 will start to restrict UL TFC for MAX TX power limitation in DCH.
====================================================================*/
void l1_enable_ul_tfc_pwr_restriction(void);


typedef enum
{
  L1_PRACH_INVALID,       /* PRACH configuration is invalid */
  L1_PRACH_10MS_ALLOWED,  /* allows 10ms RACH TTI */
  L1_PRACH_20MS_ALLOWED   /* allows 20ms RACH TTI */
} l1_rach_tti_allowed_enum_type;

typedef struct
{
  uint8   tfci;           /* Interested TFC */
  int8    max_tx_pwr;     /* UE MAX TX power in dBm in SIB */
  int8    pr_cpich_tx_pwr; /* Primary CPICH TX power in dBm in SIB */
  int8    const_val;      /* PRACH constant value in dB in SIB */
  int8    ul_if;           /* UL interference in dB in SIB 7 */
  l1_ul_cctrch_cfg_struct_type*  rach_cctrch_cfg_ptr;
  mac_ul_tfcs_type*              rach_ul_tfcs_ptr;
} l1_prach_selection_info_struct_type;

/*====================================================================
FUNCTION: L1_COMPUTE_RACH_TX_PWR_MARGIN

DESCRIPTION:
  This function computes the RACH TX power margin in PRACH selection
  procedure as in 25.331. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  L1_PRACH_INVALID, L1_PRACH_10MS_ALLOWED, or L1_PRACH_20MS_ALLOWED

SIDE EFFECTS:
  For computed gain factors, L1 will write into rach_cctrch_cfg_ptr
====================================================================*/
l1_rach_tti_allowed_enum_type l1_compute_rach_tx_pwr_margin
(
  l1_prach_selection_info_struct_type* prach_info_ptr
);

/*===========================================================================

FUNCTION L1_GET_BEST_PSC_IN_ASET

DESCRIPTION
  This function returns the best cell in the active set. This function is called
  by RRC on DCH to Idle transition so that acquisition is first attempted on the
  best cell in the active set before RRC tries acquisition on frequencies in the
  acq db.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: at least one cell was found in the active set (which should generally be the case)
  FALSE: no cell was found in the active set (failure case).

SIDE EFFECTS
  None.

===========================================================================*/
boolean l1_get_best_psc_in_aset(uint16 *scr_code, uint32 *pn_pos);

/*===========================================================================

FUNCTION L1_GET_HS_PSC_IN_ASET

DESCRIPTION
  This function returns the HS PSC and PN position for SIB reading. In absence of HS, 
  it will return the best cell in the active set.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: at least one cell was found in the active set (which should generally be the case)
  FALSE: no cell was found in the active set (failure case).

SIDE EFFECTS
  None.

===========================================================================*/
boolean l1_get_hs_psc_sib_read(uint16 *scr_code, uint32 *pn_pos);

/*===========================================================================
FUNCTION hs_get_cur_state

DESCRIPTION
  This function returns the current state of HSDPA channel in L1. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active.
  FALSE if HSDPA channel is inactive.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_get_cur_state(void);

/*===========================================================================
FUNCTION hs_get_num_configured_carriers

DESCRIPTION
  This function returns the number of configured carriers including the primary carrier .

DEPENDENCIES
  None

RETURN VALUE
  # of configured carriers

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_get_num_configured_carriers(void);

/*===========================================================================
FUNCTION        DL_PHYCHAN_CCTRCH_ID_IN_USE

FILE NAME       dl_dem.c

DESCRIPTION     This function retrieves the currently used DL CCTrCH id.

DEPENDENCIES    The physical channel database

RETURN VALUE    CCTrCH that DL is currently working on, if DL DPCH is not associated with 
                FDPCH and DPCH is established.
                0xFF if (DL DPCH is not associated with FDPCH and it is inactive) or FDPCH is active.
                Boolean dpch_active is set to TRUE if DL DPCH is active.

SIDE EFFECTS    None
===========================================================================*/
extern uint8 dl_phychan_cctrch_id_in_use(boolean *dpch_active);

/*===========================================================================
FUNCTION        UL_PHYCHAN_IS_UL_ACTIVE

DESCRIPTION     This function returns the state of UL phy channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if either PRACH or DPCH channels are active
                FALSE if neither PRACH or DPCH channels are active

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_is_ul_active (void);

/*===========================================================================
FUNCTION        UL_PHYCHAN_IS_UL_DPCH_ACTIVE

DESCRIPTION     This function returns the state of UL DPCH channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if UL DPCH channel is active
                FALSE if UL DPCH channel is active

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_is_ul_dpch_active (void);

/*===========================================================================
FUNCTION        UL_PHYCHAN_IS_UL_PRACH_ACTIVE

DESCRIPTION     This function returns the state of UL PRACH channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if UL PRACH channel is active
                FALSE if UL PRACH channel is inactive

SIDE EFFECTS    None
===========================================================================*/
extern boolean ul_phychan_is_ul_prach_active (void);

/*===========================================================================

FUNCTION          RRC_CAN_L1_GO_TO_SLEEP

DESCRIPTION       This function is called by L1 in every DRX cycle to determine
                  if L1 should go to sleep or not. These function call was 
                  introduced due to WTOW BPLMN search. If search needs to be
                  done during the DRX sleep time, then L1 should NOT go to sleep.

DEPENDENCIES      NONE

RETURN VALUE      
                  TRUE:  L1 can go to sleep.
                  FALSE: Otherwise
                                  
SIDE EFFECTS

===========================================================================*/
extern boolean rrc_can_l1_go_to_sleep
(
  uint32 timer_val,
  boolean *wtog_bplmn_in_progress
#ifdef FEATURE_DUAL_SIM
  ,boolean *wtow_bplmn_in_progress
#endif
);

/*===========================================================================

FUNCTION          L1_RRC_IS_BPLMN_ACTIVE

DESCRIPTION       This function is called by L1 during DRX cycle to determine
                  if L1 should call rrc_can_l1_go_to_sleep.

DEPENDENCIES      NONE

RETURN VALUE      
                  TRUE:  BPLMN is in progress
                  FALSE: BPLMN is not in progress
                                  
SIDE EFFECTS

===========================================================================*/
extern boolean l1_rrc_is_bplmn_active(void);


/*===========================================================================

FUNCTION rrciho_is_gtow_ho_active

DESCRIPTION 
    This function returns status of GTOW HO.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
extern boolean rrciho_is_gtow_ho_active(void);

/*===========================================================================

FUNCTION rrcpg1_modify_sibs_for_sfn_update

DESCRIPTION 
   Call back function for L1 to induce S-PCCPCH setup


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
extern boolean rrcpg1_modify_sibs_for_sfn_update(void);

/*===========================================================================

FUNCTION     l1m_state_is_fach

DESCRIPTION
  This function is called to check whether the UE is in L1M_FACH state

DEPENDENCIES
  None

RETURN VALUE
  TRUE if l1m_state is L1M_FACH and FALSE otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean l1m_state_is_fach(void);

/*===========================================================================
FUNCTION hs_is_supported

DESCRIPTION
  This function returns whether HSDPA is supported on this chip.

DEPENDENCIES
  HS cfg init must have been called earlier to set its value.

RETURN VALUE
  TRUE if HSDPA is supported.
  FALSE if HSDPA is not supported.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_is_supported(void);

/*===========================================================================
FUNCTION hs_get_mimo_state

DESCRIPTION
  This function returns the current state of mimo configuration.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_get_mimo_state(void);

/*===========================================================================
FUNCTION hs_get_mc_state

DESCRIPTION
  This function returns the current state of MC configuration.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_get_mc_state(l1_hsdpa_carrier_enum_type carrier);

/*===========================================================================

FUNCTION rrc_sib_can_l1_start_fmo
DESCRIPTION

  This function will check whether the current event is sib-7 event 
  which is needed for starting FMO's at L1 in FACH state.
  FMO should take high priority than SIB-7 event.During the 
  frames where FMO's are going on L1 will through away all
  the sib segments, since the content may be incorrect.
  
DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If event is sib-7 event in FACH, or no sib event present.
  FALSE: If event is other than sib-7 event.

SIDE EFFECTS
  None.
     
===========================================================================*/

extern boolean rrc_sib_can_l1_start_fmo (void);

/*===========================================================================

FUNCTION is_rrc_waiting_for_l2_ack_for_pch_or_smc
DESCRIPTION

  This function checks whether rrc is waiting for the L2 ACK from NW for
  the OTA message it sent.
  
DEPENDENCIES
  None.
RETURN VALUE
  TRUE:  RRC is waiting for L2 ACK
  FALSE: RRC is not waiting for L2 ACK

SIDE EFFECTS
  None.
     
===========================================================================*/

extern boolean is_rrc_waiting_for_l2_ack_for_pch_or_smc (void);

/*===========================================================================

FUNCTION    rrcllc_check_srb_mapped_to_hsdsch_present

DESCRIPTION This function checks if the HSDSCH Trch mapped to SRBs in 
             CELL DCH state exist
 
DEPENDENCIES
  None.

RETURN VALUE
 TRUE or FALSE

===========================================================================*/
extern boolean rrcllc_check_srb_mapped_to_hsdsch_present(void);
/*============================================================================
FUNCTION: rrcllc_get_ctch_trch_id()

DESCRIPTION:
  This function checks if the FACH TRCH id passed is mapped to CTCH or not
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Trch id
      Returns tr_ch_id_type if the FACH Trch is mapped to CTCH
      else   return invalid trch id

SIDE EFFECTS:
=============================================================================*/
extern tr_ch_id_type rrcllc_get_ctch_trch_id(void);
/*===========================================================================
FUNCTION        L1_GET_RSSI_DURING_WTOW_BPLMN

FILE NAME       srchbplmn.c

 DESCRIPTION     This function is used by RRC to get the signal strength of the PLMNs during
                        a BPLMN search.The measured signal level shall be sent with no sign 

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    

SIDE EFFECTS    None
===========================================================================*/
void l1_get_rssi_during_wtow_bplmn(l1_rssi_info_struct_type *rssi);

/*===========================================================================
FUNCTION l1_cm_query_compressed_mode_active

DESCRIPTION
  This function checks if any TGPSI with any TGMP is active.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean l1_cm_query_compressed_mode_active(void);

/*===========================================================================
FUNCTION l1_cm_query_num_of_cm_patterns_active

DESCRIPTION
  This function returns the number of CM patterns currently active
  or pending activation.

DEPENDENCIES
  None

RETURN VALUE
  The number of patterns that are either currently active or going active
  in the future.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 l1_cm_query_num_of_cm_patterns_active(void);

/*===========================================================================
FUNCTION RRC_L1_QUERY_IS_AMR_CALL_ENABLED

DESCRIPTION
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether an AMR voice call is actually being setup or not.
  The initial call_setup_mask was returning VOICE CALL SETUP UP if the
  domain is CS. This function enhances more checks and returns TRUE only
  if it is going to be a CS voice call.

DEPENDENCIES
  None

RETURN VALUE
  Return value is a boolean and gives information whether AMR call is being
  established.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rrc_l1_query_is_amr_call_enabled(void);


/*===========================================================================
FUNCTION rrc_get_predefined_po3_status

DESCRIPTION
  This function returns TRUE if prefined PO3 is configured by NW especially for inter RAT HO cases.
  This function is valid for DCH state only.
 

DEPENDENCIES
  This function has to be called only during DPCH setup/reconfig als when cphy setup is getting processed

RETURN VALUE
  TRUE: Predefined PO3 is configured
  FALSE: If PO3 is signalled from NW OTA

SIDE EFFECTS
  None
===========================================================================*/
boolean rrc_get_predefined_po3_status(void);


#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION     wl1_fill_tuneaway_duration

DESCRIPTION
  This is called by other modules to fill tuneaway duration (in frames)
  into our global variable.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_fill_tuneaway_duration(uint16 duration);

/*===========================================================================
FUNCTION  RRCWRM_RELEASE_RRC_LOCK_ON_L1_REQ

DESCRIPTION
  This function is called to release RRC lock by WRM during split ACQ and inter-SIB sleep

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If RRC lock is released
  FALSE - Otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean rrcwrm_release_rrc_lock_on_l1_req(void);

/*===========================================================================
FUNCTION  RRCWRM_UPDATE_RRC_LOCK_STATUS

DESCRIPTION
  This function is called by WRM to update RRC lock status if needed
  Function is supposed to be called after getting TRM lock if WRM happened to release RRC lock before

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void rrcwrm_update_rrc_lock_status(void);

#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
/*===========================================================================

FUNCTION  rrc_start_unlock_by_timer_for_tune_away

DESCRIPTION

  This function start unlock by timer which triggers tunaway from wcdma
    
DEPENDENCIES
  None.
 
RETURN VALUE

 None
 
SIDE EFFECTS

  None.

===========================================================================*/
extern void rrc_start_unlock_by_timer_for_tune_away
(
  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk
);
#endif  /* #ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY */

#endif  /* #ifdef FEATURE_DUAL_SIM                */
/*====================================================================
FUNCTION: WL1_CHECK_CELL_BARRED

DESCRIPTION:
  This function is used to check if a WCDMA cell is barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
l1_cell_bar_info_e_type wl1_check_cell_barred
(
  uint16 freq, uint16 scr_code
);

/*====================================================================
FUNCTION: wl1_check_cell_barred_and_get_bar_time

DESCRIPTION:
  This function is used to check if a WCDMA cell is barred or not. 
  If its barred, get the remaining barred time, and return the bar_type.

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
l1_cell_bar_info_e_type wl1_check_cell_barred_and_get_bar_time
(
  uint16 freq, uint16 scr_code, uint16 *rem_bar_time_in_secs, 
  boolean *is_non_whte_csg, boolean *is_bar_resel
);

/*============================================================================
FUNCTION: wl1_clear_barred_cells()

DESCRIPTION:
  This function clears all barred cell structures and timers. This function will 
  be called by RRC when the UE is soft-reset, or enters LPM.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void wl1_clear_barred_cells(void);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY)
/*===========================================================================
FUNCTION     wl1_ok_for_tuneaway

DESCRIPTION
  RRC calls this function at inter-RAT tuneaway time to ask WL1 if it is OK
  to SUSPEND WL1 and start the tuneaway process.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern boolean wl1_ok_for_tuneaway(void);

#ifdef FEATURE_QTA
/*===========================================================================
FUNCTION     wl1_ok_for_qta

DESCRIPTION
  RRC calls this function before QTA gap to ask WL1 if it is OK start QTA.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern boolean wl1_ok_for_qta(void);

/*===========================================================================

FUNCTION  RRC_RELEASE_LOCK

DESCRIPTION

  This function is called to release lock if RRC is holding the lock.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
extern void rrc_release_lock(void);

/*===========================================================================

FUNCTION  WL1_QTA_CANCEL_IND

DESCRIPTION
  RRC calls this function to indicate L1 about qta cancellation.
  

DEPENDENCIES
  The indicatin is only for the cases where rrc can't handle QTA. This funciton won't
  be called if QTA cancelled because of GSM unreserv or reservation update. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_qta_cancel_ind(void);

/*===========================================================================
FUNCTION     wl1_qta_overhead_time_in_ms

DESCRIPTION
  This function is called to detect the QTA over head time in ms
    
DEPENDENCIES
  None.
 
PARAMETERS
  

RETURN VALUE
  Uint8 - overhead in ms
===========================================================================*/
extern uint8 wl1_qta_overhead_time_in_ms(void);


#endif

#endif  /* #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_SIM_WCDMA_TUNEAWAY) */


/*===========================================================================
FUNCTION l1_get_utran_nmr_info

DESCRIPTION
  This function sends upto 6 best intra/inter-freq/inter-rat cells
  in response to a NMR request.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
boolean l1_get_utran_nmr_info
(
  l1_meas_utran_nmr_info_union_type *l1_meas_utran_nmr_info, 
  l1_rrc_utran_nmr_report_type report_type
);

/*===========================================================================
FUNCTION L1_GET_SERVING_CELL_INFO

DESCRIPTION
  This function populates the serving cell power info in the arguments
  This API should be used  in non DCH state.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
void l1_get_serving_cell_power(l1_serving_cell_power_info_type* l1_serving_cell_info);

/*===========================================================================
FUNCTION l1_get_active_set_info

DESCRIPTION
  This function populates the active set info in the arguments provided
  This function should be called only in the DCH state.
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
void l1_get_active_set_info(l1_active_set_info_type* l1_active_set_info);

/*===========================================================================
FUNCTION  L1_GET_MONITORED_SET_INFO

DESCRIPTION
  This function populates the monitores set info in the structure provided

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/

void l1_get_monitored_set_info(l1_monitored_set_info_type* l1_monitored_set_info);

/*============================================================================
FUNCTION: hs_get_max_ir_buf_size_supported()

DESCRIPTION:
  This function Will compute total IR buf size supported by UE with the 
indicated HARQ size in SIB5 with explicit memory partition  .
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE:- if the IR buf size provided as input to this function is supported by UE 
  FLASE:- otherwise. 

SIDE EFFECTS:
=============================================================================*/

boolean hs_get_max_ir_buf_size_supported
(
  uint8 hs_cat,
  l1_dl_hs_dsch_harq_info_struct_type *harq_info
);

/*============================================================================
FUNCTION: rrcllc_check_srb_mapped_to_trch()

DESCRIPTION:
  This function checks if the TRCH id passed is mapped to SRB or not
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE
      Returns True if the Trch is mapped to SRB
      else   FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_check_srb_mapped_to_trch(tr_ch_id_type trch_id );

/*===========================================================================
FUNCTION       wl1_drx_page_ind

DESCRIPTION
  This function handles the RRC page ind during PICH demod - for both true and false page.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
PARAMETERS 
  Boolean - TRUE if true page
            FALSE if it is false page.
===========================================================================*/
extern void wl1_drx_page_ind(boolean true_page);

/*===========================================================================

FUNCTION     rrc_get_oos_timer_value

DESCRIPTION
  RRC API to get the OOS timer value. RRC will return an appropriate value based on RRC state

DEPENDENCIES
  None.

RETURN VALUE
  uint32 type timer value in ms

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 rrc_get_oos_timer_value(void);

/* ------------------------------------------------------------------- */
/* These are the channel numbers used by each band class. Note that    */
/* they do match those specified in 3GPP 25.101 v7.2.0.                */
/* ------------------------------------------------------------------- */

/* --------------------------------- */
/*  BC-1 IMT                         */                  
/* --------------------------------- */
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC1_MIN                10562   
#define WCDMA_FS_RX_CHAN_BC1_MAX                10838   


/* --------------------------------- */
/*  BC-2 PCS                         */                  
/* --------------------------------- */
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC2_MIN                9662     
#define WCDMA_FS_RX_CHAN_BC2_MAX                9938


/* ADDITIONAL CHANNELS downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC2_ADDITIONAL_MIN     412     
#define WCDMA_FS_RX_CHAN_BC2_ADDITIONAL_MAX     687                     


/* --------------------------------- */
/*  BC-3                             */                  
/* --------------------------------- */
 /* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC3_MIN                1162     
#define WCDMA_FS_RX_CHAN_BC3_MAX                1513


/* --------------------------------- */
/*  BC-4 1700(uplink)/2100(downlink) */                  
/* --------------------------------- */
 /* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC4_MIN                1537   
#define WCDMA_FS_RX_CHAN_BC4_MAX                1738
 
/* ADDITIONAL CHANNELS downlink channel numbers */                    
#define WCDMA_FS_RX_CHAN_BC4_ADDITIONAL_MIN     1887    
#define WCDMA_FS_RX_CHAN_BC4_ADDITIONAL_MAX     2087


/* --------------------------------- */
/*  BC-5 850                         */                  
/* --------------------------------- */
/* Downlink channel numbers */ 
#define WCDMA_FS_RX_CHAN_BC5_MIN                4357    
#define WCDMA_FS_RX_CHAN_BC5_MAX                4458

/* ADDITIONAL CHANNELS downlink channel numbers */                    
#define WCDMA_FS_RX_CHAN_BC5_ADDITIONAL_MIN     1007    
#define WCDMA_FS_RX_CHAN_BC5_ADDITIONAL_MAX     1087

/* --------------------------------- */
/*  BC-6 800                         */                  
/* --------------------------------- */
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC6_MIN                4387    
#define WCDMA_FS_RX_CHAN_BC6_MAX                4413

/* ADDITIONAL CHANNELS channel numbers */                    
#define WCDMA_FS_RX_CHAN_BC6_ADDITIONAL_MIN     1037    
#define WCDMA_FS_RX_CHAN_BC6_ADDITIONAL_MAX     1062
              
/* --------------------------------- */
/*  BC-7 2600                        */                  
/* --------------------------------- */
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC7_MIN                2237    
#define WCDMA_FS_RX_CHAN_BC7_MAX                2563
 
/* ADDITIONAL CHANNELS downlink channel numbers */                    
#define WCDMA_FS_RX_CHAN_BC7_ADDITIONAL_MIN     2587    
#define WCDMA_FS_RX_CHAN_BC7_ADDITIONAL_MAX     2912

/* --------------------------------- */
/*  BC-8 900                         */                  
/* --------------------------------- */
/* Downlink channel numbers */          
#define WCDMA_FS_RX_CHAN_BC8_MIN                2937    
#define WCDMA_FS_RX_CHAN_BC8_MAX                3088


/* --------------------------------- */
/*  BC-9 1700                         */                  
/* --------------------------------- */  
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC9_MIN                9237    
#define WCDMA_FS_RX_CHAN_BC9_MAX                9387


/* --------------------------------- */
/*  BC-11 1500                         */                  
/* --------------------------------- */  
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC11_MIN                3712
#define WCDMA_FS_RX_CHAN_BC11_MAX                3787

/* --------------------------------- */
/*  BC-19 -extended 800                         */                  
/* --------------------------------- */  
/* Downlink channel numbers */
#define WCDMA_FS_RX_CHAN_BC19_MIN                712
#define WCDMA_FS_RX_CHAN_BC19_MAX                763

/* ADDITIONAL CHANNELS channel numbers */                    
#define WCDMA_FS_RX_CHAN_BC19_ADDITIONAL_MIN     787
#define WCDMA_FS_RX_CHAN_BC19_ADDITIONAL_MAX     837

/* ------------------------------------------------------------------- */
/* The above channel defines are mapped into defines currently being   */
/* used by RRC and L1                                                  */
/* ------------------------------------------------------------------- */

/* DOWNLINK CHANNEL */

/* --------------------------------- */
/*  BC-1 IMT                         */                  
/* --------------------------------- */

/* Downlink channel boundary for IMT band (I)*/
#define WCDMA_FS_RX_CHAN_MIN                   WCDMA_FS_RX_CHAN_BC1_MIN
#define WCDMA_FS_RX_CHAN_MAX                   WCDMA_FS_RX_CHAN_BC1_MAX


/* --------------------------------- */
/*  BC-2 PCS                         */                  
/* --------------------------------- */

/* Downlink channel boundary for PCS band (II)*/
#define WCDMA_FS_RX_CHAN_PCS_MIN               WCDMA_FS_RX_CHAN_BC2_MIN   
#define WCDMA_FS_RX_CHAN_PCS_MAX               WCDMA_FS_RX_CHAN_BC2_MAX

/* Downlink channel boundary for PCS additional channels */
#define WCDMA_FS_RX_CHAN_PCS_ADDITIONAL_MIN    WCDMA_FS_RX_CHAN_BC2_ADDITIONAL_MIN
#define WCDMA_FS_RX_CHAN_PCS_ADDITIONAL_MAX    WCDMA_FS_RX_CHAN_BC2_ADDITIONAL_MAX


/* --------------------------------- */
/*  BC-3                             */                  
/* --------------------------------- */

/* Down link channel boundary for BC3 */
#define WCDMA_FS_RX_CHAN_1800_MIN              WCDMA_FS_RX_CHAN_BC3_MIN   
#define WCDMA_FS_RX_CHAN_1800_MAX              WCDMA_FS_RX_CHAN_BC3_MAX


/* --------------------------------- */
/*  BC-4 1700(uplink)/2100(downlink) */                  
/* --------------------------------- */

/* Down link channel boundary for AWS 1700/2100 band (IV) */
#define WCDMA_FS_RX_CHAN_1700_2100_MIN         WCDMA_FS_RX_CHAN_BC4_MIN   
#define WCDMA_FS_RX_CHAN_1700_2100_MAX         WCDMA_FS_RX_CHAN_BC4_MAX

/* Downlink channel boundary for 1700/2100 band additional channel (BC4)*/
#define WCDMA_FS_RX_CHAN_1700_2100_ADDITIONAL_MIN     WCDMA_FS_RX_CHAN_BC4_ADDITIONAL_MIN   
#define WCDMA_FS_RX_CHAN_1700_2100_ADDITIONAL_MAX     WCDMA_FS_RX_CHAN_BC4_ADDITIONAL_MAX


/* --------------------------------- */
/*  BC-5 850                         */                  
/* --------------------------------- */

/* Downlink channel boundary for 850 band (V)*/
#define WCDMA_FS_RX_CHAN_850_MIN               WCDMA_FS_RX_CHAN_BC5_MIN   
#define WCDMA_FS_RX_CHAN_850_MAX               WCDMA_FS_RX_CHAN_BC5_MAX

/* Downlink channel boundary for 850 band additional channel */
#define WCDMA_FS_RX_CHAN_850_ADDITIONAL_MIN    WCDMA_FS_RX_CHAN_BC5_ADDITIONAL_MIN   
#define WCDMA_FS_RX_CHAN_850_ADDITIONAL_MAX    WCDMA_FS_RX_CHAN_BC5_ADDITIONAL_MAX


/* --------------------------------- */
/*  BC-6 800                         */                  
/* --------------------------------- */

/* Downlink channel boundary for 800 band (VI)*/
#define WCDMA_FS_RX_CHAN_800_MIN               WCDMA_FS_RX_CHAN_BC6_MIN   
#define WCDMA_FS_RX_CHAN_800_MAX               WCDMA_FS_RX_CHAN_BC6_MAX

/* Downlink channel boundary for 800 band additional channel */
#define WCDMA_FS_RX_CHAN_800_ADDITIONAL_MIN    WCDMA_FS_RX_CHAN_BC6_ADDITIONAL_MIN   
#define WCDMA_FS_RX_CHAN_800_ADDITIONAL_MAX    WCDMA_FS_RX_CHAN_BC6_ADDITIONAL_MAX


/* --------------------------------- */
/*  BC-8 900                         */                  
/* --------------------------------- */

/* Down link channel boundary for 900 band (VIII) */
#define WCDMA_FS_RX_CHAN_900_MIN               WCDMA_FS_RX_CHAN_BC8_MIN
#define WCDMA_FS_RX_CHAN_900_MAX               WCDMA_FS_RX_CHAN_BC8_MAX

/* --------------------------------- */
/*  BC-9 1700                         */                  
/* --------------------------------- */ 

/* Downlink channel boundary for BC9 */
#define WCDMA_FS_RX_CHAN_1700_MIN              WCDMA_FS_RX_CHAN_BC9_MIN
#define WCDMA_FS_RX_CHAN_1700_MAX              WCDMA_FS_RX_CHAN_BC9_MAX


/* --------------------------------- */
/*  BC-11 1500                         */                  
/* --------------------------------- */ 

/* Downlink channel boundary for BC11 */
#define WCDMA_FS_RX_CHAN_1500_MIN              WCDMA_FS_RX_CHAN_BC11_MIN
#define WCDMA_FS_RX_CHAN_1500_MAX              WCDMA_FS_RX_CHAN_BC11_MAX

/* --------------------------------- */
/*  BC-19 extended 800                        */                  
/* --------------------------------- */ 

/* Downlink channel boundary for BC19 */
#define WCDMA_FS_RX_CHAN_EXTENDED_800_MIN              WCDMA_FS_RX_CHAN_BC19_MIN
#define WCDMA_FS_RX_CHAN_EXTENDED_800_MAX              WCDMA_FS_RX_CHAN_BC19_MAX
/* Downlink channel boundary for 800 band additional channel */
#define WCDMA_FS_RX_CHAN_EXTENDED_800_ADDITIONAL_MIN    WCDMA_FS_RX_CHAN_BC19_ADDITIONAL_MIN   
#define WCDMA_FS_RX_CHAN_EXTENDED_800_ADDITIONAL_MAX    WCDMA_FS_RX_CHAN_BC19_ADDITIONAL_MAX

/* Needed by RF FTM module also - ftm_hsdpa_bler.c */
#define HARQ_SZ_ENUM_TO_VAL(sz_enum) \
  ((sz_enum == L1_DL_HS_DSCH_HMS43200) ? (43200) : \
   ((sz_enum) <= L1_DL_HS_DSCH_HMS16000)  ? (800    + ((int32) (800   * ((int32)(sz_enum) - (int32)L1_DL_HS_DSCH_HMS800   )))) : \
   ((sz_enum) <= L1_DL_HS_DSCH_HMS32000)  ? (17600  + ((int32) (1600  * ((int32)(sz_enum) - (int32)L1_DL_HS_DSCH_HMS17600 )))) : \
   ((sz_enum) <= L1_DL_HS_DSCH_HMS80000)  ? (36000  + ((int32) (4000  * ((int32)(sz_enum) - (int32)L1_DL_HS_DSCH_HMS36000 )))) : \
   ((sz_enum) <= L1_DL_HS_DSCH_HMS160000) ? (88000  + ((int32) (8000  * ((int32)(sz_enum) - (int32)L1_DL_HS_DSCH_HMS88000 )))) : \
   ((sz_enum) <= L1_DL_HS_DSCH_HMS304000) ? (176000 + ((int32) (16000 * ((int32)(sz_enum) - (int32)L1_DL_HS_DSCH_HMS176000)))) : \
   304000)

/****************************
**  GSM ARFCN_T MACROS     **
****************************/

#define CONVERT_ARFCN_TO_NUM(x) ((x).num)

#define COMPARE_ARFCN(x,y) (((x).num == (y).num) && ((x).band == (y).band))

/*===========================================================================
 FUNCTION:   WCDMA_RRC_GET_TCB

 DESCRIPTION:
   This function returns a pointer to the RRC TCB. 

 DEPENDENCIES:
   None.

 PARAMETERS:
   None.

 RETURN VALUE:
   A pointer to the RRC TCB.

 SIDE EFFECTS:
   An assertion will fail if the lookup fails.
===========================================================================*/
extern rex_tcb_type* wcdma_rrc_get_tcb(void);

/*============================================================================
FUNCTION: WL1_IS_SERV_FREQ_CONSIDERED_BARRED

DESCRIPTION: This function notifies RRC if it can be IN SERVICE. 
If the serving freq bar request is received due to a SIB read on a USET cell, 
L1 treats it as a PSC barred so RRC should stay IN SERVICE 
If the serving freq bar request is received due to a SIB read on a NCL cell, 
L1 bars the serving frequency so RRC should move to NO SERVICE.  

      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - Bar request is received on a NSET cell, serv freq is barred
  FALSE - Bar reqeust is received on a USET cell, serv freq is not barred.

SIDE EFFECTS:
=============================================================================*/
extern boolean wl1_is_serv_freq_considered_barred(uint16 freq, uint16 psc);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION     wl1_fill_tuneaway_duration

DESCRIPTION
  This is called by other modules to fill tuneaway duration (in frames)
  into our global variable.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_fill_tuneaway_duration(uint16 duration);

/*===========================================================================
FUNCTION     wl1_ok_for_tuneaway

DESCRIPTION
  RRC calls this function at inter-RAT tuneaway time to ask WL1 if it is OK
  to SUSPEND WL1 and start the tuneaway process.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern boolean wl1_ok_for_tuneaway(void);
#endif

/*===========================================================================
FUNCTION     rrcllc_update_sec_hsdpa_hs_scch_order_activation_status_to_oc

DESCRIPTION
  L1 will call this API to update the scch order activation

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern void rrcllc_update_sec_hsdpa_hs_scch_order_activation_status_to_oc(uint8 activation_status, boolean oc_update_needed);


/*===========================================================================
FUNCTION     DRX_INFORM_L1_ABT_BPLMN_STATUS

DESCRIPTION
  RRC informs L1 about BPLMN START and STOP by calling this API. Based on the
  parameter passed CTCH is blocked or unblocked accordingly.

DEPENDENCIES
  None.

PARAMETERS
  BPLMN status
  TRUE  --  BPLMN START
  FALSE --  BPLMN STOP

RETURN VALUE
===========================================================================*/
extern void drx_inform_l1_abt_bplmn_status(boolean bplmn_status);
/*===========================================================================
FUNCTION        dlchmgr_is_ext_pccpch_active

DESCRIPTION     This function returns TRUE if S PCCPCH is active

DEPENDENCIES    None

RETURN VALUE
                TRUE: PCCPCH active
                FALSE: No PCCPCH is active

SIDE EFFECTS    None.
===========================================================================*/
extern boolean dlchmgr_is_ext_pccpch_active(void);

/*===========================================================================
FUNCTION               DL_GET_CELL_TIMING
FILE NAME              dlstates.h

DESCRIPTION            This function returns the SFN of cell

INPUT PARAMS           Primary Scrambling Code & Frequency

OUTPUT PARAMS          
EXECUTION CONTEXT    

DEPENDENCIES           None
RETURN VALUE           uint16 SFN of the cell 
                   [0-4095] - if timing is known
                       [0xFFFF] - if cell timing is not known

SIDE EFFECTS           None
===========================================================================*/
extern uint16 dl_get_cell_timing(uint16 psc, uint16 freq);
/*============================================================================
FUNCTION: rrc_check_if_trch_is_configured_for_voice()

DESCRIPTION:
  This function checks if the TRCH id passed is mapped to voice RB
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE
      Returns True if the Trch is mapped to Voice RB
      else   FALSE

SIDE EFFECTS:
=============================================================================*/
extern boolean rrc_check_if_trch_is_configured_for_voice
(
  tr_ch_id_type               trch_id
);
/*===========================================================================

FUNCTION IS_SLEEP_DRV_NOT_IN_SLEEP

DESCRIPTION
    This function returns TRUE or FALSE depending on the last interrupt 
    received from the sleep controller. Sleep driver is supposed to be
    awake if the last interrupt is SLEEP_CTLR_INT_NONE
    
DEPENDENCIES
    None
    
RETURN VALUE
    TRUE : if last interrupt is SLEEP_CTLR_INT_NONE
    FALSE: all other interrupts other than SLEEP_CTLR_INT_NONE

SIDE EFFECTS
    None

===========================================================================*/
extern boolean is_sleep_drv_not_in_sleep(void);

/*===========================================================================
 FUNCTION  wl1_rfm_get_default_band_chan

 DESCRIPTION
   get the default channel info.
 
 DEPENDENCIES
   None

 RETURN VALUE
   default freq. channel

 SIDE EFFECTS
   None
===========================================================================*/
extern uint16 wl1_rfm_get_default_chan();

/*===========================================================================
 FUNCTION  wl1_is_l1_in_mdsp_recovery

 DESCRIPTION
   get whether L1 is in middle of MDSP recovery.
 
 DEPENDENCIES
   None

 RETURN VALUE
   TRUE/FALSE.

 SIDE EFFECTS
   None
===========================================================================*/
extern boolean wl1_is_l1_in_mdsp_recovery(void);
#endif /* L1RRCIF_H */
