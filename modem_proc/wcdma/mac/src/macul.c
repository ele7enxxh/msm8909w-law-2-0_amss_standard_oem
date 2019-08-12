/*================================================================================================

                            U P L I N K     M A C    H A N D L E R

GENERAL DESCRIPTION

  This module contains functions to :
  - process uplink MAC task events
  - process uplink ciphering configuration data
  - process uplink RNTI information
  - process uplink CCTrCh configuration
  - process UL MAC configuration
  - Process UL MAC commands

Copyright (c) 2001,2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2003-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

================================================================================================*/

/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macul.c_v   1.26   10 Jul 2002 10:04:02   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macul.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------------------------------------
05/08/15  aa    Changes for not committing 0x41CE when ciphering config is not changed
06/23/15  aa    Changes for new log packet 0x41CE
05/20/15  as    Enable RxD after back-off timer expired and turn on RxD after persistency passed
02/12/15  aa    Mainlined FEATURE_MAC_QXDM_LOGGING
02/06/15  kc    Cipher related global is properly reset for HO_STEP2 cipher config case
11/16/14  kv    Minimum Set to be updated when TFCC is received and only PS RABs are present
09/04/14  scv   L2 changes for FEATURE_WCDMA_DIME_SW Cleanup
07/30/14  aa/kv Fix compiler warnings on Bolt
07/26/14  rkmk  Reduce image size by using right macro depending upon number of parameters
05/19/14  kv    Fixed KW Errors
05/06/14  geg   Use safer versions of memcpy() and memmove()
03/19/14  kc    Changing FEATURE for heap corruption issue.
03/18/14  ts    Fix for heap memory corruption issue, to allocate and free heap memory fo rlog buffers in l2ultask itself
08/07/13  kc    Log packet support for UL MAC cipher parameters
05/22/13  kc    Removing unused need_srb_status flag from BFI interface
02/19/13  kc    Fix to pend status only if trashed pdu is RLC AM CTRL pdu
11/7/12   ts     Fixed KW errors
10/01/12  pm    New enum RLC_SUSPEND/RLC_RESUME added in CMAC_UL_CONFIG
07/25/12  mrg    New WM reg api() added for trigger base rach
08/13/12  mrg    HS-RACH - Tb timer semaphore added
07/24/12  geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12  zr    Adding feature definitions for Dime
06/18/12  mrg   L2 - MAC-I and HS-RACH code changes
03/06/12  mrg   RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
03/27/12  grk   1. Removed wcdma_intlock() from R99 pdu building.
                2. added additional profiling for pdu building
03/20/12  kc    Fixed concurrency issue between MAC config and RACH procedure
03/06/12  mrg   RACH staus info from L1 to MAC indicaiton with signal based approch enhanced with double buffer.
02/10/12  kc    Moved excessive logging in UL ISR under debug flag
02/03/12  mn    Fix to read amr_mode from rrc api rather than from mvs. 
                Reading from mvs api may return garbage value. 
01/02/12  grk   Feature cleanup
01/23/12  kc    Changes for free floating tasks
01/09/12  sk    MAC changes to support RxD in FACH
12/06/11  ksr   Fix to handle back to back RACH ABORT and RACH NO ACK to MAC from L1
                Moving to command based interface from signal based approach.
10/04/11  kc    Changes to enable blocked Tx only after RBR to DCH is complete
10/06/11  grk   Fixed KW errors
09/05/11  grk   Fix to consider SRBs as highest priority during TFCI selection.
08/26/11  grk   Fix for KW errors
06/02/11  kc    Added support for logical channel to transport channel mapping info
05/02/11  grk   Turn off amr dump flag
04/28/11  vp    Check for CCTrch Type and Order Config TFCS pointer before
                configuring the power state information.
04/25/11  vp    Changes to address the rate change issue for voice call
                Qchat media concurrency scenarios
01/06/10  mrg   compilation error fixed
12/16/10  mrg   In case of RACH abort and persistency failure MAC informs RLC to re-send status report
12/07/10  vp    Added support for amr packet simulation in uplink
01/12/10  mrg   Excessive MAC F3 messages for AMR case are moved under flag mac_enable_amr_debug_dump
11/11/10  mrg   MAC DL HS ZI memory optimization changes
10/18/10  ssg   Added changes for new WCDMA-L2 interface.
09/16/10  mrg   Moved auido(MVS) dependency under the feature flag FEATURE_MVS
07/08/10  mrg   In case of FTM, For ZI memory changes 
                         dynamic memory allcoation done in MAC UL config procedure
06/28/10  mrg   MAC-R99 ZI memory reduction changes
06/21/10  mrg   acquiring UL data path locks added - while EUL reconfig is going on in L2-UL task
06/16/10  mrg   Lint warnings are fixed
06/11/10  mrg   Lint errors fixed
04/29/10  mrg   When TFCC command processing was going on in task context,
                         this fix will make sure TFCC database will not be overwritten by ISR logics.
10/07/09  mrg   Fixed Klocwork warnings
07/06/09   av   R99 & L2 Mutex cleanup
06/18/09  mrg   Find TFCI index for which lowest possible AMR rate allowed and make this TFCI
                 as part of Minimum TFCI Set.
06/08/09  kvk   Fixed compile errors/warnings
05/29/09  ssg   Added support for SNOW_3G algo.
05/15/09  grk   Added changes for R99 mutex.
05/07/09  ssg   Updated Copyright Information
04/29/09   av   Fixed compiler warnings in L2 
04/23/09  mrg   Fix added to log the all allowed AMR rates to EVENT_WCDMA_UL_AMR_RATE.
04/03/09  mrg   Fix to updated the AMR rate with minimum TFCI set
01/22/09  grk   Merged L2 mutex changes from taxis to mainline.
12/18/08  ksr   L2 Mutex changes to avoid concurrency issues
11/21/08  kvk   Replaced INTLOCK on EUL path with critical section
12/01/08  mrg   Made Changes for new MVS-MAC Interface to configure AMR.                   
11/04/08  mrg   Fixed compilation warning,while accessing mac_ul_event_amr_rate() 
10/27/08  ssg   Post EVENT_WCDMA_UL_AMR_RATE event when AMR rate is changed.
22/10/08  mrg   If any MAC internal data struture TFC state change happens, 
                we need to re_compute the AMR Rate.
10/17/08  grk   Merged taxis changes to mainline.
10/10/08  grk   Lint warning fixes.
09/12/08  mrg   NON_HO_STEP2_CIPHER_CFG handler is added to update count_c in two steps.
09/02/08  cnp   Replaced intlock/free with wcdma_intlock/free.
          cnp   Replaced tasklock/free with wcdma_tasklock/free.
          cnp   Replaced intlock/free_sav with wcdma_intlock/free_sav.
08/27/08  mrg   MAC should always update the TFS information sent by RRC.
05/02/08  sk    Added a new enum value MAC_LB_AND_UL_CFG_CHANGED and handling of it
22/04/08  mrg   Added fix CR# 138335 - AMR-WB RESTRICTED_TFCI
03/06/08  ssg   Added fix to store the pdu_size for enhanced buffer status computation
                during MAC re-configuration.
03/13/08   sk   Added an optimization to reset the MAC TSN's right on reaching the activation time
                L1 will call the function L1_edch_reset_tsn at activation time to reset the MAC TSN's
                The switching to the new config will happen after L1 is done with its reconfig
                Added MAC_CHECK_HSUPA_CONFIG_STATUS declaration
03/13/08   sk   Changed the mac_recionfig_in_progress to just before the call to the hsupa handler
02/04/08   sk   Added mac_hs_ul_reconfig_in_progress to indicate the start of CMAC_UL_CONFIG_REQ in MAC
                This is needed to avoid scenarios where the EDCH build frame indication comes to MAC
                before MAC has applied the new config.
11/14/07   rm   Added support for sending RRC_CMAC_UL_TFC_SUBSET_CFG_CNF in case of success as
                well as failure.
                Also made changes to act on tfcs list only when new_tfcs_list_present is TRUE
10/17/07   ps   Logic for deriving AMR rate based on sum of class A,
                Class B and Class C bits.
10/17/07   ps   Added support to make TFCIs from NOT_ALLOWED to ALLOWED in Full TFCI case
                for TFCC
10/01/07   sk   Dont consider the TFC's which are removed for the TFCC restrictions. THis is
                under #ifdef FEATURE_LIMIT_TFCS_TO_ERAM_SIZE. The TFC's would be marked as REMOVED
                only if they dont meet the HW limit for the maximum number of blocks and max tb size
05/21/07   sk   Add 1 the cfn for logging purposes. Also return from bld frame indication when NO
                UL cctrch is detected
04/03/07   sk   Corrected indentation for FEATURE_VOC_AMR_WB
03/28/07   sk   Corrected the indentation
03/20/07   sk   Added mac_get_cnt_c_val under FEATURE_WCDMA_CNT_C_SFN_DIFF
03/08/07   sk   Removed warning
03/03/07   sk   Added support for WB-AMR under FEATURE_VOC_AMR_WB
02/10/07   sk   When the mac config is not changed dont copy the tfcs pointer.
02/04/07   sk   Put the mac_hs_ul_cur_cfn under FEATURE_MAC_HS_UL_QXDM_LOGGING
01/31/07   sk   Added mac_hs_ul_cur_cfn which will export the CFN value to the HSUPA module
11/15/06   sk   Added HHO support
10/16/06   sk   Fixed compiler warning
08/14/06   sk   Added HSUPA support
03/30/06   sk   Call function RLC_PROCESS_STATUS_REPORT_REQ when L1 sends indication that
                L2 ack was tarshed due to HHO, CCO etc under feature
                FEATURE_WCDMA_LAST_UL_SRB_DATA_REC
03/24/06  sk    Added function mac_rrc_get_cipher_pending_status() which will be called by
                RRC before it sends the ciphering config. This function will return TRUE
                when some ciphering config is pending and will return FALSE when there
                is no pending config in MAC for ciphering. This addition is for CR 84591.
03/12/06  sk    Corrected indentation. Changed the array bound from MAC_UL_TFC_MAX to UE_MAX_TFC
09/21/05  gsc   Added support for Rel-5 Spec CR 2272.
09/21/05  gsc   Fixed Lint Errors.
08/16/05  gsc   Removed dependency on rrc_get_gcf_flag_status() in l1_suspend_ind() and
                l1_resume_ind(). Removed unused code.
07/21/05  gsc   Fixed lint error(s).
07/20/05  gsc   Added support for new interface between L1/MAC for CFN reporting to MAC by L1. This
                new mechanism in enabled when FEATURE_WCDMA_HFN_UPDATE is not defined.
07/19/05  sk    Added support for multirate amr rate change dynamically. In function
                mac_ul_compute_amr_rate - compute the lowest amr rate based on the TB sizes.
                In case of multirate AMR call, whenever there are restrictions applied
                removed because of the receipt of TFCC messages, recompute the AMR rate
                and configure vocoder with that rate.
05/02/05  sk    Separated the mac_update_hfn_on_rollover() function as 2 separate functions
                l1_suspend_ind and l1_resume_ind. This functions will be called by L1,
                the ciphering config in case of reverting back is configured based on the
                flag revert_back_to_old_config.
04/21/05  sk    Removed mac_get_nv_items function
04/21/05  sk    Added support for reverting back to old ciphering configuration on failure of HHO
03/21/05  sk    Linted the new changes checked in for ciphering fix.
03/17/05  sk    Removed the compiler warnings .
03/16/05  sk    Fix for CR 58735. Update the count_c's for 2 cfn's prior to the resume cfn so that
                if MAC starts getting PDU's immediately after l1 and mac is setup.
                Moved the code in l1_phy_ul_build_frame_ind to a new function
                called mac-update_ciphering_params.
03/09/05  sk    Fixed all lint errors.
03.03.05  sk    populated the cfn_hfn_table when Step-1 config is received. This is needed
                because DL might be receivign packets even before sync is established.
                Deciphering for those packets will use this config.
02/18/05  sk    When MAC receives a configuration for RACH setup, clear any ciphering config
                stored already in MAC.
02/09/05  sk    Fix for critical lint errors.
01/27/05  sm    Added argument 'l1_mac_suspend_resume_enum_type' to function mac_update_hfn_on_rollover()
                and removed the toggle flag 'wcdma_suspended_state' which was used to indicate
                whether layer 1 is suspended or resumed
12/30/04  sk    Added a new function inline void mac_ul_set_amr_rate(). Also modified the
                function mac_ul_compute_amr_rate to return the amr_mode.
12/22/04  sk    Added processing of SMC_INITIATED_NON_HO_CIPHER_CFG. In this case, MAC would
                start doing ciphering only after the activation time is reached.
12/08/04  sk    Fixed compiler warnings.
12/04/04  sk    config_ul_mac() : mac_cfg_changed is now an enum type.
                MAC_CFG_NOT_CHANGED : Copy only the tfcs ptr.
                MAC_CFG_CHANGED : Normal processing where all the parameters from RRC is copied.
                MAC_LB_CFG_CHANGED : Config is sent only to indicate that loopback is entered.
12/01/04  sk/sm Added inline function mac_handle_activation_cfn_expiry. Optimized the handling
                of ciphering with the additiona of a macro CHECK_FOR_ACT_CFN_EXPIRY.
                Handled the case when ciphering activation time is reached during the time
                when wcdma l1 was in suspended state.
11/22/04  sk    Added function mac_compute_amr_rate() under FEATURE_WCDMA_MULTIRATE_AMR. This function
                is called whenever the TFCC message is recvd and the tfcs table needs to change.
11/10/04  kc    Check for CS_Domain correctly if UE is already suspended or not. Fixed typo due to merging.
10/01/04  kc    Reset WCDMA_SUSPENDED_FLAG to FALSE when mac-tm bearers are released or when ciphering
                is disabled
09/28/04  sk    If MAC is getting reconfigured because of ASU, then copy the control flag field
                to the new tfcs config table. No configuration for MAC actually changes during ASU but
                MAC still gets configured by RRC.
09/21/04  sk    Added mac_increment_hfn_on_rollover flag to determine for the case when another security
                mode command caused new set of keys to be given to MAC when already a ciphering was going
                on for TM bearers. Thsi could happen in the case of call wait for instance.
                In this case MAC will have to increment the hfn on rollvoer without  having to wait
                for the activation time. More importantly the DL will also syncup to the new set
                of keys at the activation time.
09/15/04  sk    Added uint16 ntfc under FEATURE_TFC_WCDMA_PWR_ELMN in function l1_phy_ul_build_frame_ind.
08/18/04  sk    Removed mac_reset_periodic_event_trigger_counters on receving
                everytime CMAC_UL_CONFIG_REQ. Instead this function would now
                be called only on DCH->FACH transition or vice versa
08/03/04  sk    Changed the control duration checking from 512 to 5120. Since RRC would now
                give the duration in ms directly.
07/27/04  sk    Removed multiplying control duration by 10 as RRC would pass the control duration
                in milliseconds.
07/23/04  sk    Handled the restrictedTFCI choice correctly for TFCC msg.
07/23/04  sk    Added mac_ul_update_trch_info() in config_ul_mac(). This will update the transport channel
                list for traffic volume measurements if they are currently active.
07/19/04  sk    proc_ul_mactfcc_msg() : For the choice restricted_tfci_list, allow tfci's corresponding
                to the tfi's specified by RRC for that transport channel.
07/19/04  sk    Fixed a compilation error which was caused because of a merge in function
                config_ul_mac().
07/16/04  sk    Added proc_ul_mac_tfcc_command() - Function to process the TFCC msg and store the
                config
                mac_ul_handle_tfcs_control_timer_expiry().Function to handle the expiry of the
                TFC control duration timer.
                Added some initializations in the int_ul_mac function.
                All additions are done under FEATURE_TFCS_CONTROL_ENABLE
06/04/04  sk    For FEATURE_WCDMA_PWR_TFC_ELMN, the tfc_excess_pwr_state_ptr that L1 passes to MAC
                in build_frame_ind is made a const.
06/03/04  sk    Added some debug messages for ciphering.
04/14/04  sk    Added int_ul_trchtf_info.
03/24/04  sk    Added a flag ul_tx_disabled in l1_phybuild_frame_ind to indicate to mac the cfn value.
                No data will be processed however when this flag is set to TRUE since UL TX is disabled
03/16/04  sk    Based on the mac_tm_bearers_count, the ciphering is enabled. This will ensure that hfn is
                not updated when there are no active tm mode bearers.
03/11/04  sk    Added a ERR_FATAL to catch the problem when oc_ul_tfcs_ptr exceeds UE_MAX value.
03/11/04  sk    Removed the MSG_HIGH for printing prev_ul_cipher_pending flag. This fixed the compilation
                error.
03/09/04  sk    Fix for CR 40529, 39743. Take care of the case when the RB activation time is
                reached before the RB activation time.
12/15/03  sk    mac_update_hfn_on_rollovers(): Set cfn_rolled_over[i] = TRUE in step-2 config
                when the ciphering activation time is given to be 0
12/12/03  sk    Set cfn_rolled_over[i] = TRUE in step-2 config when the ciphering activation
                time is given to be 0.
                Fix for CR 36875 proc_ul_mac_commands(): Remove unnecessary processing for
                (ul_configuration_pending)
12/04/03  sk    Added debug messages to be printed every mac_ul_debug_print_interval
                times for debugging ciphering garbling.
11/24/03  sk    Removed function mac_update_ciphering_parameters function and modified
                mac_update_hfn_on_rollover() to cover corner cases in handling ciphering
                activation time when it is reached in suspend state.
10/28/03  sk    Set the first_time_ciphering[i] flag to true for step2 cfg and non-ho cfg.
10/10/03  sk    In the case of Step-2 cipher cfg, do not get the new HFN from RRC once the activation
                time is reached.
09/26/03  vk    Called function mac_reset_periodic_event_trigger_counters(..) to clear pending timer
08/28/03  sk    config_ul_mac(): Added a flag mac_config_changed
                If no configuration parameters have actually changed in MAC then copy only the
                TFCS ptr and nothing else
08/13/02  sk    Removed memset(cfn_hfn_table) after getting the new start value to transmit from RRC.
07/22/03  sk    Resolved the ciphering activation time problem which was caused due to the merge
                & checkin goof-up.
07/11/03  sk    l1_phy_ul_build_frame_ind(): Handled the case when the ciphering activation time
                is set to 0.
06/26/03  sk    proc_ul_cctrch_info() : Call populate_rach_trch_id() function. to correctly populate
                the RACH id when in RACH state.
05/28/03  sk    Saved the function pointer from L1 under FEATURE_TFCS_CM_BY_HLS.
05/23/03  sk    Added tfc_rlc_size_for_dch_defined under FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH to indiccate
                if RLC size restrictions are done for DCH or not.
05/06/03  sk    Copying the ciphering parameters only if key id has changed in the case of NON_HO_CIPHER_CFG
05/01/03  sk    Ciphering support for 2G<->3G handover.
04/29/03  sk    Support for Constant Delay feature under FEATURE_TC_CONSTANT_DELAY.
04/21/03  sk    Added a flag get_start_value_to_transmit[MAX_CN_DOMAIN] to indicate that MAC
                should get a new HFN from RRC when the configuration is complete. Also made
                first_time_ciphering flag as an array of MAX_CN_DOMAIN.
03/24/03  sk    proc_ul_cipher_info(): Reset the ciphering configuration only if ciphering is not enabled.
12/31/02  sk    Added support for TFCS selection based on UE tX POwer under FEATURE_WCDMA_PWR_TFC_ELMN
12/04/02  sk    proc_ul_cctrch_info(): Not calling rlc_ul_setup_tx_trigger() as PRACH would be always
                enabled and not dependent on the fact that RLC queues have data or not.
11/13/02  sk    Moved updating of hfn to the beginning of the function: l1_phy_ul_build_frame_ind()
11/01/02  sk    proc_ul_cipher_info(): Intialized cfn_hfn_table at the appropriate place.
10/08/02  sk    Removed mac_ul_delete_all_traffic_meas() procedure call from proc_ul_mac_commands().
09/26/02  sk    Added a lookup table cfn_hfn_table. Added boolean first_time_ciphering. Added
                a check to enable RLC size restriction only in RACH.
09/11/02  sk    In RACH handled the case when an interrupt occurs when MAC has not complted
                its earlier build frame indication request.  Also current_tti value is being
                saved immediately when MAC gets the build frame indication from L1.
09/04/02  sk    * init_ul_mac() : Initlisation of cipher configuration structures.
                * proc_ul_cipher_info() : Copy the ciphering configuration sent by RRC
                * Added global variables for ciphering
08/24/02  sk    Removed function proc_dl_mac_cfg_cnf_sig(). Now MAC Ul posts the config cnf
                message wihtout having to wait for MAC DL to finish its reconfig.
08/16/02  sk    l1_ul_phy_build_frame_ind(). Added a check to accept the
                tx_buf_ptr to be NULL when MAC is still reconfiguring
08/02/02  sk    Commented the function void proc_ul_cipher_info (mac_cipher_config_type*cipherptr).
                config_ul_mac(): Commented calling of function proc_ul_cipher_info();
                proc_ul_mac_commands(): Included a check to switch to the new config based
                on the flag wait_for_l1_sig set by RRC.
06/08/02  sk    Removed FEATURE_TRAFFIC_VOLUME_MEAS
05/20/02  sk    proc_ul_mac_commands():Put if(block_transmission) under FEATURE_TRAFFIC_OVLUME_MEAS
05/30/02  sk    Added ul_cfginfo (to copy the configuration paramters recvd from RRC,
                ul_configuration_pending flag to indicate if MAC is waiting for configuration
                signal from L1. proc_ul_mac_cmds(): We copy the configuration parameters.
                Added functions: proc_ul_mac_config_signal(). This function would
                configure UL MAC and send a confirmation message to L1.
05/11/02  sk    Added config_ul_mac_traffic_volume() to process the traffic volume related
                commands received from RRC. Populated the trch_id_to_idx_map_type table.
                Initilised the traffic volume measurements using mac_ul_traffic_volume_init().
                In proc_build_ul_frame_ind(): Added a boolean block_transmission. If this
                flag is false then we transmit else we block.
                All the features added are under "FEATURE_TRAFFIC_VOLUME_MEAS".
03/22/02  ps    proc_ul_cctrch_info() : Request Layer 1 to disable build frame indications when
                UL MAC is not configured for RACH
03/13/02  ps    proc_ul_rnti_info() : Process the UL RNTI config info for BOTH_VALID case
02/12/02  ps    Deleted UL U-RNTI related code
                Added SRB Delay Related Code with FEATURE_L1_SRB_DELAY definition
12/19/01  sh    Code to print current time after mac builds frame.
12/19/01  gs    Removed code that free's dsm items in MAC.
12/18/01  sh    Free dsm items only during DCH before building next frame in
                l1_phy_ul_build_frame_ind(). RACH dsm items will be freed by
                MAC at a different stage.
12/17/01  sh    Changed l1_phy_ul_build_frame_ind() to free dsm items only
                if we have transmitted the data and do nothing if MAC is still
                servicing the previous request.
                Changed proc_build_ul_frame_ind() to reset mac_busy flag after
                building data.
10/19/01  ps    Added FEATURE_MAC_BUILD_UL_FRAME_IN_INTERRUPT
                If this feature is defined, UL frame is built in interrupt
                context.
                Otherwise, the UL Frame is built in UL L2 task context.
10/02/01  ps    l1_phy_ul_build_frame_ind() : UL TBs are built in Build Frame
                Indication interrupt context (instead of task context)
09/18/01  ps    l1_phy_ul_build_frame_ind() : Added code to initialize Transmit buffer
08/21/01  rc    Added support for QXDM logging.
08/07/01  ps    Fixed the leading zero message format problem
07/01/01  ps    Removed the code that sets and clears "mac_busy" flag (not used anymore)
06/19/01  ps    Converted UL TX Status and Build Frame Indications from commands to signals
06/18/00  ps    Addjusted message levels
11/17/00  ps    Initial release

================================================================================================*/
/*================================================================================================
                     INCLUDE FILES
================================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "task.h"
#include  "macrrcif_v.h"
#include  "l1macif.h"
#include  "l1sapcommon.h"
#include  "l1macdata.h"
#include  "macinternal.h"
#include  "l2ultask.h"
#include  "maculdata.h"
#include  "macrach.h"
#include  "maculdata.h"
#include  "macul.h"
#include  "maculdch.h"
#include  "macultfcs.h"
#include  "mactraffic.h"
#include  "macrlcif.h"
#include  <string.h>
#include  "err.h"
#include "rrcnv.h"
#include "stmr.h"
#include "event.h"
#include "event_defs.h"

#ifdef FEATURE_WCDMA_HSUPA
#include "maculhs.h"
#endif
#include "macullog.h"

#include "l1extif.h"

#include "maculdl.h"

#ifdef FEATURE_TC_CONSTANT_DELAY
  #include "maculdl.h"
#endif


#ifdef FEATURE_WCDMA_MULTIRATE_AMR
#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */
#endif

#include "maculsubtask.h"

#include "macdl.h"

#include  "rrccmd_v.h"
extern mac_ul_tfc_control_e_type        mac_ul_full_tfcs_control_table[UE_MAX_TFC];
extern mac_ul_tfc_control_e_type        mac_ul_default_tfcs_control_table[UE_MAX_TFC];
extern mac_ul_tfc_control_e_type        mac_ul_current_tfcs_control_table[UE_MAX_TFC];
mac_ul_tfc_control_e_type               mac_ul_pending_tfcs_control_table[UE_MAX_TFC];
extern mac_ul_tfcs_type                 mac_ul_tfcs_table;
//extern tfcs_control_ptr_in_use_e_type   mac_ul_tfcs_control_ptr_in_use;
boolean                                 mac_ul_tfcc_config_pending=FALSE;
boolean                                 mac_ul_tfcc_config_in_process = FALSE;
uint16                                  mac_ul_tfcc_activation_cfn=0xFFFF;
uint16                                  mac_ul_tfcs_pending_control_duration;
/* Traffic volume timer*/
rex_timer_type                          ul_tfc_ctrl_tmr_blk;

boolean             mac_enable_amr_debug_dump = FALSE;

#ifndef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
/* UL TFCS Info */
extern ul_tfcs_info_type   int_ul_tfcs_info[UE_MAX_TFC];
#else
extern ul_tfcs_info_type   *int_ul_tfcs_info;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

#ifdef FEATURE_MVS_UNDER_GHDI
extern mvs_amr_mode_type ghdi_mvs_get_ul_amr_mode(void);
extern void ghdi_mvs_amr_set_amr_mode( mvs_amr_mode_type amr_ul_mode);

#ifdef FEATURE_VOC_AMR_WB
extern void ghdi_mvs_amr_set_awb_mode(mvs_amr_mode_type amr_ul_mode);
extern mvs_amr_mode_type ghdi_mvs_get_ul_awb_mode(void);
#endif /* FEATURE_VOC_AMR_WB */

#endif /* FEATURE_MVS_UNDER_GHDI */

#ifdef FEATURE_WCDMA_SIMULATE_AMR_PACKETS
extern void rlc_ind_cfn_upd(void);
#endif /*FEATURE_WCDMA_SIMULATE_AMR_PACKETS*/

/*================================================================================================
                         CONSTANT    D E C L A R A T I O N S
================================================================================================*/


#define FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

#define CHECK_FOR_ACT_CFN_EXPIRY(S,A,R)   ((uint32)(((uint16)256+(A)-(S)) & 0xFF) <= ((uint32)((uint16)256+(R)-(S)) & 0xFF))

/*================================================================================================
                         D A T A    D E C L A R A T I O N S
================================================================================================*/
/* Uplink CCTrCh info internally used by MAC */
ul_cctrch_info_type       int_ul_cctrch_info;

boolean mac_ul_config_in_progress = FALSE;
rex_crit_sect_type mac_config_mutex;

#ifdef FEATURE_TC_CONSTANT_DELAY
/* Notes the uplink cfn */
uint8  cur_ul_cfn;
#endif

/* this array contains the indexes where the transport channel information for that */
/* transport channel id is stored. */
trch_id_to_idx_map_type ul_trch_id_to_idx_table[MAX_TRCH];

/* Uplink Logical Channel info table internally used by MAC */
ul_lc_info_type         int_ul_lc_info[UE_MAX_UL_LOGICAL_CHANNEL];

/* UL Transport Channel Info table internally used by MAC */
ul_trch_info_type       int_ul_trch_info[UE_MAX_TRCH];

/* PRACH transmit status */
l1_prach_status_enum_type   rach_tx_status[2]; //Made double buffer to store 2 status info

#ifdef FEATURE_WCDMA_HS_RACH
/* PRACH transmit status */
l1_hsrach_status_enum_type   hs_rach_tx_status[2]; //Made double buffer to store 2 status info
#endif

/* UL Frame Build Info */
mac_ul_build_frame_ind_type   bld_frame_ind_info;

/* TRUE if the C-RNTI is valid. FALSE otherwise */
boolean     ul_mac_crnti_valid = FALSE;

/* C-RNTI value */
uint16      ul_mac_crnti;

/* Flag to denote that MAC is still building previous request */
boolean mac_build_pending = FALSE;

extern l1_tti_enum_type    current_tti;

/* The configuration parameters are stored in here */
mac_ul_config_type    ul_cfginfo;

/* TRUE implies MAC received a new configuration parameters from RRC and waiting    */
/* L1 signal to actually configure. FALSE implies there are no pending configuration*/
/* requests now */
boolean               ul_configuration_pending=FALSE;

extern uint8  mac_tm_bearers_count;

/* The debug messages in UL will be printed every 500 ms. */
uint8 mac_ul_debug_print_interval = 0xF;

/* Poking this flag to TRUE at run time will provide debug messages to see where the
ciphering went wrong */
extern boolean ciphering_garbled;

boolean                   mac_increment_hfn_on_rollover[MAX_CN_DOMAIN];
mac_ciphering_table       cfn_ciphering_table[256];

/* Ordered ciphering config data */
mac_cipher_config_type    cur_ul_cipher_info[MAX_CN_DOMAIN];

/* Before the ciphering activation time, the ciphering configuration that is
received from RRC is stored in this structure */
mac_cipher_config_type    new_ul_cipher_info[MAX_CN_DOMAIN];

/* TRUE indicates that there is a ciphering configuration that is pending*/
boolean                   ul_cipher_activation_pending[MAX_CN_DOMAIN];

#define INVALID_PREV_CFN 0xFFFF

/* Save the prev cfn value sent by physical layer every TTI */
uint16                     prev_cfn= INVALID_PREV_CFN;

/* Count C value for each of the domain */
uint32                    count_c[MAX_CN_DOMAIN];

mac_ul_cipher_log_pkt_type cipher_log[MAC_UL_MAX_CIPHER_LOG_CNT];
uint16                     cipher_log_idx = 0;

/* Table that keeps track of the HFN associated with the cfn */
uint32                    cfn_hfn_table[MAX_CN_DOMAIN][256];

/* This flag will indicate to MAC that it needs to get a new value of HFN from RRC
on reaching the activation time for the first time */
boolean                   get_start_value_to_transmit[MAX_CN_DOMAIN];

boolean                   prev_mac_increment_hfn_on_rollover[MAX_CN_DOMAIN];
mac_ciphering_table       prev_cfn_ciphering_table[256];

/* Ordered ciphering config data */
mac_cipher_config_type    prev_cur_ul_cipher_info[MAX_CN_DOMAIN];

/* Before the ciphering activation time, the ciphering configuration that is
received from RRC is stored in this structure */
mac_cipher_config_type    prev_new_ul_cipher_info[MAX_CN_DOMAIN];

/* TRUE indicates that there is a ciphering configuration that is pending*/
boolean                   prev_ul_cipher_activation_pending[MAX_CN_DOMAIN];

/* Save the prev cfn value sent by physical layer every TTI */
uint16                    old_prev_cfn= INVALID_PREV_CFN;

/* Count C value for each of the domain */
uint32                    prev_count_c[MAX_CN_DOMAIN];

/* Table that keeps track of the HFN associated with the cfn */
uint32                    prev_cfn_hfn_table[MAX_CN_DOMAIN][256];

/* This flag will indicate to MAC that it needs to get a new value of HFN from RRC
on reaching the activation time for the first time */
boolean                   prev_get_start_value_to_transmit[MAX_CN_DOMAIN];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN

boolean       tfc_excess_pwr_state[UE_MAX_TFC];

#endif
#ifdef FEATURE_TC_CONSTANT_DELAY
/* Indicates if the loopback test mode is 1, 2 or invalid*/
mac_loop_back_mode_e_type loopback_test_mode;
#endif

#ifdef FEATURE_VOC_AMR_WB
// This stores the information regarding the codec type for this call.
extern mvs_mode_type    mac_mvs_codec_info;
#endif

boolean       tfc_rlc_size_for_dch_defined=FALSE;
#ifdef FEATURE_TFCS_CM_BY_HLS

L1_UL_QUERY_TFCS_RESTRICT_CB_FUNC_TYPE *ptr_to_l1_fn;

#endif

/* This structre will hold information of the transport channels and the logical
channels mapped to it at various priority levels. */

#ifndef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
maci_ul_trch_priority_info_type int_ul_trch_priority_info[UE_MAX_TRCH][MAC_UL_MAX_MLPS];
#else
maci_ul_trch_priority_info_type     **int_ul_trch_priority_info;
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */

/* Thsi structure holds information as to the number of pdus that can be built
for a particular transport channel having a particular transport format */

maci_ul_trch_tf_type int_ul_trch_tf_info[UE_MAX_TRCH][UE_MAX_TF];

// Indicates if it is PS only, CS only, multirab call.
extern mac_rab_status_e_type   rab_status;

/*-----------------------------------------------------------------------
  Implementation of Rel-5 spec CR 2272.
-----------------------------------------------------------------------*/
mac_saved_cipher_info_type saved_new_ul_cipher_info[MAX_CN_DOMAIN];


#ifdef FEATURE_WCDMA_HSUPA
/* This will be used by the HSUPA module to appropriately
set the CFN value for the log packets and also for using
the value for timers*/
#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
uint8 mac_hs_ul_cur_cfn;
#endif

extern boolean mac_hs_ul_reconfig_in_progress;

#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
rex_crit_sect_type mac_cipher_param_mutex;
#endif

l2_rlc_profile_data_struct_type mac_build_frame_profile_dump;
l2_rlc_profile_data_struct_type phy_build_frame_profile_dump;

extern l1_ul_tx_data_type mac_rach_tx_buf;

/*================================================================================================
 FUNCTION NAME  init_ul_mac_ciphering_params

 DESCRIPTION  This function initialises UL MAC ciphering parameters
================================================================================================*/
void  init_ul_mac_ciphering_params
(
  void
)
{
  uint8 i;

  for (i=0;i<MAX_CN_DOMAIN;i++)
  {
    /* Clear the memory */
    cur_ul_cipher_info[i].cipher_enable = FALSE;
    new_ul_cipher_info[i].cipher_enable = FALSE;
    cur_ul_cipher_info[i].key_idx = 0xFF;
    new_ul_cipher_info[i].key_idx = 0xFF;

    /* Initialize ciphering pending to FALSE for both CS and PS domains */
    ul_cipher_activation_pending[i]=FALSE;

    get_start_value_to_transmit[i]=TRUE;
    mac_increment_hfn_on_rollover[i] = FALSE;

   /*
    * Initialize fields of saved_new_ul_cipher_info here.
    */
    saved_new_ul_cipher_info[i].cipher_info.cipher_enable = FALSE;
    saved_new_ul_cipher_info[i].cipher_info_saved = FALSE;
  }
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  rex_init_crit_sect(&mac_cipher_param_mutex);
#endif
}

/*================================================================================================
 FUNCTION NAME  init_ul_mac

 DESCRIPTION  This function initialises UL MAC
================================================================================================*/
void  init_ul_mac
(
void
)
{
  /* Initialize the traffic volume measurements table */
  mac_ul_traffic_volume_init();
  /* Initialize the mutex */
  rex_init_crit_sect(&mac_config_mutex);

  if (l1_seq_register_get_cfn_cb_func(mac_update_ciphering_params))
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Registered with L1 for CFN update event");
  }
  else
  {
    ERR_FATAL("Unable to register for CFN update event with L1",0,0,0);
  }

  /* Initialize ciphering parameters */
  init_ul_mac_ciphering_params();


  // Define the tfcs control timer.
  rex_def_timer(&ul_tfc_ctrl_tmr_blk, wcdma_l2_ul_get_tcb(), MAC_UL_TFCS_CONTROL_TMR_EXP_SIG);

  // Memset the current config.
  memset(mac_ul_current_tfcs_control_table, (uint8)TFC_NOT_ALLOWED,
          UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

  memset(mac_ul_default_tfcs_control_table, (uint8)TFC_NOT_ALLOWED,
          UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

  memset(mac_ul_full_tfcs_control_table, (uint8)TFC_NOT_ALLOWED,
          UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

  memset(mac_ul_pending_tfcs_control_table, (uint8)TFC_NOT_ALLOWED,
          UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

  memset(tfc_pwr_state_bitmap, 0, TFC_PWR_STATE_BITMAP_SIZE * 4);
  mac_ul_tfcc_config_in_process = FALSE;

  memset(&mac_rach_tx_buf, 0, sizeof(l1_ul_tx_data_type));

  memset(rach_tx_status, L1_PRACH_MAC_INVALID, sizeof(rach_tx_status));

#ifdef FEATURE_WCDMA_HSUPA
  mac_hs_ul_perform_init(TRUE);
  #ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
  mac_eul_allocate_mem_log_records();
  #endif
#endif

#ifdef FEATURE_WCDMA_HS_RACH
  rex_init_crit_sect(&mac_hsrach_tb_timer_sem);
  mac_hs_rach_info.hs_rach_active = FALSE;
  mac_hs_rach_info.data_present_on_srb = FALSE;
  mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause = WL1_HS_RACH_EDCH_REL_INVALID;
  mac_hs_rach_info.mac_hsrach_edch_rsrc_rel_cause_valid = FALSE;
#endif

  cipher_log_idx = 0;
}

/*===============================================================================================
 FUNCTION NAME  proc_ul_cipher_info

 DESCRIPTION  This function processes the Cipher information received from RRC.

===============================================================================================*/
void    proc_ul_cipher_info (mac_cipher_config_type   cipherptr[])
{
  uint8  i;
  uint32 j;
  boolean config_changed = TRUE;

  if ((mac_tm_bearers_count == 0) || (GET_UL_CCTRCH_TYPE == RACH))
  {
    // Entry 0 is for CS domain.
    // Clear the old configuration.
    L2_ACQ_CIPHER_PARAM_LOCK();
    cur_ul_cipher_info[0].cipher_enable = FALSE;
    new_ul_cipher_info[0].cipher_enable = FALSE;
    cur_ul_cipher_info[0].key_idx = 0xFF;
    new_ul_cipher_info[0].key_idx = 0xFF;
    ul_cipher_activation_pending[0]=FALSE;
    get_start_value_to_transmit[0]=TRUE;
    mac_increment_hfn_on_rollover[0] = FALSE;
    L2_ACQ_CIPHER_PARAM_UNLOCK();
    return;
  }

  for (i=0;i<MAX_CN_DOMAIN;i++)
  {
    // if ciphering is enabled
    if (cipherptr[i].cipher_enable)
    {
      L2_ACQ_CIPHER_PARAM_LOCK();
      // Check the type of the config received
      switch (cipherptr[i].cipher_cfg_type)
      {
        case HO_STEP1_CIPHER_CFG:

          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Step-1 Cipher cfg, key: %d, act_time: 0x%x, hfn: 0x%x",
            cipherptr[i].key_idx,
            cipherptr[i].ciph_act_time,
            cipherptr[i].hfn);

          // Copy the new ciphering parametrs received to the current cfg directly
          // as there is no new config to switch to at activation time.
          WCDMA_MEMCPY(&cur_ul_cipher_info[i], 
                       sizeof(mac_cipher_config_type),
                       &cipherptr[i],
                       sizeof(mac_cipher_config_type));

          /* Initialize the cfn hfn table */
          //memset(cfn_hfn_table[i], cur_ul_cipher_info[i].hfn, 256*(sizeof(uint32)));

          for (j=0;j<256;j++)
          {
            cfn_hfn_table[i][j] = cipherptr[i].hfn;
            cfn_ciphering_table[j].cipher_algo = cipherptr[i].cipher_algo;
            cfn_ciphering_table[j].cipher_enable =cipherptr[i].cipher_enable;
            cfn_ciphering_table[j].key_idx = cipherptr[i].key_idx;
          }
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Step-1 Cipher cfg, key: %d, algo: 0x%x, hfn: 0x%x",
            cfn_ciphering_table[0].key_idx,
            cfn_ciphering_table[0].cipher_algo,
            cfn_hfn_table[0][0]);

          // Indicate that there is a new config to which MAC has to switch to at
          // ciphering activation time.

          ul_cipher_activation_pending[i]=FALSE;

          /* Initialize the prev_cfn to 0 */
          prev_cfn = INVALID_PREV_CFN;
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prev cfn 0x%x",
                                                     prev_cfn);

          break;

        case SMC_INITIATED_NON_HO_CIPHER_CFG:
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "SMC Initiated Cipher cfg key %d, act_time: 0x%x, hfn: 0x%x",
            cipherptr[i].key_idx,
            cipherptr[i].ciph_act_time,
            cipherptr[i].hfn);
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "SMC Initiated Cipher cfg algo %d",
                                                     cipherptr[i].cipher_algo);

          /*-----------------------------------------------------------------------
             We have received a new ciphering configuration while the previous one
             is still pending. Save it for future use. Rel-5 Spec CR 2272.
           -----------------------------------------------------------------------*/
          if (ul_cipher_activation_pending[i])
          {
            WCDMA_MEMCPY(&saved_new_ul_cipher_info[i].cipher_info, 
                         sizeof(mac_cipher_config_type),
                         &cipherptr[i],
                         sizeof(mac_cipher_config_type));

            saved_new_ul_cipher_info[i].cipher_info_saved = TRUE;
          }
          else
          {
            /* Store the new configuration recvd and switch to it at the cfn*/
            // Copy the new ciphering parametrs received.
            WCDMA_MEMCPY(&new_ul_cipher_info[i], 
                         sizeof(mac_cipher_config_type),
                         &cipherptr[i],
                         sizeof(mac_cipher_config_type));

            /* Store the new configuration recvd and switch to it at the cfn*/
            // Copy the new ciphering parametrs received.
            WCDMA_MEMCPY(&cur_ul_cipher_info[i], 
                         sizeof(mac_cipher_config_type),
                         &cipherptr[i],
                         sizeof(mac_cipher_config_type));

            // Indicate that there is a new config to which MAC has to switch to at
            // ciphering activation time.
            ul_cipher_activation_pending[i] = TRUE;
            mac_increment_hfn_on_rollover[i] = FALSE;
            /* Initialize the prev_cfn to 0 */
            prev_cfn = INVALID_PREV_CFN;
          }
          break;

        case HO_STEP2_CIPHER_CFG:
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Step-2 Cipher cfg key 0x%x, act_time 0x%x, hfn 0x%x",
            cipherptr[i].key_idx,
            cipherptr[i].ciph_act_time,
            cipherptr[i].hfn);

       case NON_HO_STEP2_CIPHER_CFG: /*lint !e616*/
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "NON HO Step-2 Cipher cfg key 0x%x, act_time 0x%x, hfn 0x%x",
            cipherptr[i].key_idx,
            cipherptr[i].ciph_act_time,
            cipherptr[i].hfn);

        case NON_HO_CIPHER_CFG: /*lint !e616*/
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Non-HO Cipher cfg key 0x%x, act_time 0x%x, hfn 0x%x",
            cipherptr[i].key_idx,
            cipherptr[i].ciph_act_time,
            cipherptr[i].hfn);


          if ((cipherptr[i].cipher_cfg_type==NON_HO_CIPHER_CFG) &&
            (cipherptr[i].key_idx == cur_ul_cipher_info[i].key_idx))
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prev cfn 0x%x",
                                                       prev_cfn);
	    config_changed = FALSE;

            break;
          }

          /* Initialize the prev_cfn to invalid */
          prev_cfn = INVALID_PREV_CFN;
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prev cfn 0x%x prev_key %d, cur_key %d",
                                                     prev_cfn,
                                                     cur_ul_cipher_info[i].key_idx,
            cipherptr[i].key_idx);

          /*--------------------------------------------------------------------------
          If the parameters have changed, then copy them to new_ul_cipher_info
          and switch to the new configuration at ciphering activation time which is
          provided by the CFN number.
          ---------------------------------------------------------------------------*/

          /* Store the new configuration recvd and switch to it at the cfn*/
          // Copy the new ciphering parametrs received.
          WCDMA_MEMCPY(&new_ul_cipher_info[i],
                       sizeof(mac_cipher_config_type), 
                       &cipherptr[i],
                       sizeof(mac_cipher_config_type));

          // Indicate that there is a new config to which MAC has to switch to at
          // ciphering activation time.
          ul_cipher_activation_pending[i]=TRUE;

          /*-------------------------------------------------------------------------
          If previously ciphering was not enabled and now it is enabled, then copy the
          ciphering parameters to the both current & new cipher configuratiosn that
          MAC maintains.
          -------------------------------------------------------------------------*/
          if (!cur_ul_cipher_info[i].cipher_enable)
          {
            /* Store the new configuration recvd and switch to it at the cfn*/
            // Copy the new ciphering parametrs received.
            WCDMA_MEMCPY(&cur_ul_cipher_info[i], 
                         sizeof(mac_cipher_config_type),
                         &cipherptr[i],
                         sizeof(mac_cipher_config_type));

            // Initialize
            get_start_value_to_transmit[i]=TRUE;

          }
          else if (NON_HO_CIPHER_CFG == cipherptr[i].cipher_cfg_type
            && cur_ul_cipher_info[i].cipher_enable)
          {
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Roll-over case for Call-Wait");
            mac_increment_hfn_on_rollover[i] = TRUE;
          }

          if ((mac_increment_hfn_on_rollover[i] == FALSE) &&
            (cipherptr[i].cipher_cfg_type != HO_STEP2_CIPHER_CFG))
          {
            for (j=0;j<256;j++)
            {
              cfn_hfn_table[i][j] = cipherptr[i].hfn;
            }
          }
          break;

        default:
          break;
      } /* end switch */

      if ((config_changed) && (log_status(WCDMA_MAC_UL_CIPHER_CONFIG_LOG_PACKET)))
      {
        mac_ul_send_cipher_config_log_packet(&(cipherptr[i]));
      }
      
      L2_ACQ_CIPHER_PARAM_UNLOCK();
    } /* end if ciphering enabled */
    else if (!cipherptr[i].cipher_enable)
    /* if ciphering is disabled */
    {
      L2_ACQ_CIPHER_PARAM_LOCK();
      // Clear the old configuration.
      cur_ul_cipher_info[i].cipher_enable = FALSE;
      new_ul_cipher_info[i].cipher_enable = FALSE;
      cur_ul_cipher_info[i].key_idx = 0xFF;
      new_ul_cipher_info[i].key_idx = 0xFF;
      ul_cipher_activation_pending[i]=FALSE;
      get_start_value_to_transmit[i]=TRUE;
      mac_increment_hfn_on_rollover[i] = FALSE;
      L2_ACQ_CIPHER_PARAM_UNLOCK();	
    } /* end else */

  } // for number of domains

} /* end function */

/*===============================================================================================
FUNCTION NAME proc_ul_rnti_info

PURPOSE     This function processes UL RNTI information received from RRC
===============================================================================================*/
void    proc_ul_rnti_info
(
mac_rnti_config_type   *rntiptr)
{
  /* Process CRNTI info based on RNTI validity flasg */
  switch (rntiptr->rnti_valid)
  {
    /* CRNTI is valid or BOTH valid */
    case CRNTI_VALID:
    case BOTH_VALID:
      ul_mac_crnti_valid = TRUE;
      ul_mac_crnti = rntiptr->crnti;
      break;

      /* No valid C-RNTI */
    default:
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "No valid C-RNTI");
      ul_mac_crnti_valid = FALSE;
      break;
  }
}


#ifdef FEATURE_WCDMA_SRB_PRIO_READJ
#error code not present
#endif

/*=================================================================================================
 FUNCTION NAME    proc_ul_cctrch_info

 DESCRIPTION    This function processes uplink CCTrCh information
=================================================================================================*/
void    proc_ul_cctrch_info
(
  /* Pointer to ordered config data of UL CCTrCh */
  mac_ul_cctrch_config_type *cctrch_ptr
)
{
  /* Update info based on CCTrCh type */
  switch (cctrch_ptr->cctrch_type)
  {

    /* RACH type CCTrCh */
    case RACH:

      /* Update UL CCTrCh info */
      /* This is internally used by MAC */
      int_ul_cctrch_info.cctrch_type = RACH;
      int_ul_cctrch_info.ntrch = 1;

      /* Process RACH configuration data */
      proc_rach_cctrch_cfg (&cctrch_ptr->rach_info);

      (void)populate_rach_trch_id();

      mac_rach_rxd_enable = FALSE;
      /*------------------------------------------------------------------------------
      PRACH would always be enabled. Hence RLC need not trigger MAC to enable PRACH
      when it sees data in its buffer.
      -------------------------------------------------------------------------------*/
      mac_ul_trigger_tx_req(
                            #ifdef FEATURE_DSM_WM_CB
                              NULL, NULL
                            #else
                            void
                            #endif
                           );
      
      break;

      /* DCH type CCTrCh */
    case DCH:

      /* Update UL CCTrCh info */
      /* This is internally used by MAC */
      int_ul_cctrch_info.cctrch_type = DCH;
      int_ul_cctrch_info.ntrch = cctrch_ptr->dch_info.ndchs;

      /* Process uplink DCH configuration data */
      proc_ul_dch_cctrch_cfg (&cctrch_ptr->dch_info);
      
      #ifdef FEATURE_WCDMA_SRB_PRIO_READJ
      #error code not present
#endif
      
      break;

      /* Invalid CCTrCh type -- Set CCTrCh type to INVALID */
    default:
      int_ul_cctrch_info.cctrch_type = INVALID_UL_CCTRCH;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Uplink CCTrCh released",
                                                  cctrch_ptr->cctrch_type);
      break;
  }
}

/*==============================================================================================
 FUNCTION NAME  config_ul_mac

 PURPOSE    This function processes UL MAC Configuration Data
===============================================================================================*/
void    config_ul_mac
(
  /* Pointer to ordered config data of UL MAC */
  mac_ul_config_type    *cfgptr
)
{
  int i;
  uint8 trch_id[UE_MAX_TRCH];
  int j;

  L2_MUTEX_LOCK(&mac_config_mutex);
  mac_ul_config_in_progress = TRUE;
  L2_MUTEX_UNLOCK(&mac_config_mutex);

#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
  mac_ul_allocate_mem_ZI_opt();
#endif

  /* If no configuration parameters have actually changed in MAC then copy only the
  TFCS ptr and nothing else */
  if (cfgptr->mac_config_changed == MAC_CFG_NOT_CHANGED)
  {
    /*for (i=0;i<oc_ul_tfcs_ptr->ntfc;i++)
    {
      memcpy(&cfgptr->tfcs_ptr->tfc_info[i].control_flag,
             &oc_ul_tfcs_ptr->tfc_info[i].control_flag, sizeof(mac_ul_tfc_control_e_type));
    }
    oc_ul_tfcs_ptr = cfgptr->tfcs_ptr;*/
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC config not changed");
    
    L2_MUTEX_LOCK(&mac_config_mutex);
    mac_ul_config_in_progress = FALSE;
    L2_MUTEX_UNLOCK(&mac_config_mutex);
    return;
  }
#ifdef FEATURE_TC_CONSTANT_DELAY
  if ((cfgptr->mac_config_changed == MAC_LB_CFG_CHANGED) ||
      (cfgptr->mac_config_changed == MAC_LB_AND_UL_CFG_CHANGED))
  {
    // Clear up the loopback mode type.s
    loopback_test_mode = MAC_LOOPBACK_NA;

    /* Store the Transport channel IDs */
    if (cfgptr->cctrch_ptr->cctrch_type == RACH)
    {
      /* Indicated if the TC is in loopback mode 1 or 2 or not applicable */
      if (MAC_LOOPBACK_MODE_1 == cfgptr->cctrch_ptr->rach_info.loopback_test_mode)
      {
        loopback_test_mode = MAC_LOOPBACK_MODE_1;
        // Loopback mode 2 can not be supported in RACH state.
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Loopback mode 1 config recvd in FACH state");
      }
    }
    else if (cfgptr->cctrch_ptr->cctrch_type == DCH)
    {
      loopback_test_mode = cfgptr->cctrch_ptr->dch_info.loopback_test_mode;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Loopback mode %d config recvd in DCH state",
                                                 loopback_test_mode);
    }

    if (cfgptr->mac_config_changed == MAC_LB_CFG_CHANGED)
    {
      L2_MUTEX_LOCK(&mac_config_mutex);
      mac_ul_config_in_progress = FALSE;
      L2_MUTEX_UNLOCK(&mac_config_mutex);
      return;
    }
  }
#endif
  /*else*/
  {

    L2_ACQ_UL_LW_LOCK(UE_MAX_UL_LOGICAL_CHANNEL);


    /* Clear uplink logical channel information table */
    memset (int_ul_lc_info, 0x00, UE_MAX_UL_LOGICAL_CHANNEL * sizeof(ul_lc_info_type));

    /* Clear uplink transport channel information table */
    memset (int_ul_trch_info, 0x00, UE_MAX_TRCH * sizeof(ul_trch_info_type));

#ifdef FEATURE_TC_CONSTANT_DELAY
    // Clear up the loopback mode type.s
    if (cfgptr->mac_config_changed == MAC_CFG_CHANGED)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "No LB cfg changed, only MAC cfg changed");
      loopback_test_mode = MAC_LOOPBACK_NA;
    }
#endif

    /* Initialize the id to idx map table */
    for (i=0;i<MAX_TRCH;i++)
    {
      ul_trch_id_to_idx_table[i].valid= FALSE;
    }

    /* Initialize */
    for (i=0;i<UE_MAX_TRCH;i++)
    {
      for (j=0;j<MAC_UL_MAX_MLPS; j++)
      {
        int_ul_trch_priority_info[i][j].num_lc = 0;
        int_ul_trch_priority_info[i][j].total_bo = 0;
      }
    }

    /* Process UL MAC RNTI information */
    proc_ul_rnti_info (&cfgptr->rnti_info);

    /* Process RACH Transmit Control Parameters */
    proc_rach_tx_parameters (&cfgptr->rach_tx_cntl_info);

#ifdef FEATURE_WCDMA_HS_RACH
    proc_hs_rach_cfg(cfgptr);
#endif

    /* Update uplink logical channel information */
    /* This information is internally used by MAC */
    if (cfgptr->cctrch_ptr != NULL)
    {
      proc_ul_cctrch_info (cfgptr->cctrch_ptr);
    }
    else
    {
       MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Pointer to UL CCTrCh configuration data is NULL");
       return;
    }
    /* Process UL MAC Ciphering information */
    proc_ul_cipher_info (cfgptr->cipher_info);

    /* Process TFCS information */
    if (cfgptr->tfcs_ptr != NULL)
    {
#ifdef FEATURE_TFCC_ENHNC
        if(cfgptr->new_tfcs_list_present == TRUE)
#endif
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCC: Processing UL TFCS info");
          proc_ul_tfcs_info (cfgptr->tfcs_ptr);
        }
#ifdef FEATURE_TFCC_ENHNC
        else if( oc_ul_tfcs_ptr != NULL)
        {
          /* MAC should always update the TFS information sent by RRC */
          /* Clear uplink TFCS information table internally used by MAC */
          memset (mac_ul_tfcs_table.tfs_info, 0, 
                     (UE_MAX_TRCH * sizeof(mac_ul_tfs_type)));
          
          /* Copy config TFS info to the TFS table internally used by MAC */
          WCDMA_MEMCPY (mac_ul_tfcs_table.tfs_info,
                        UE_MAX_TRCH * sizeof(mac_ul_tfs_type), 
                        cfgptr->tfcs_ptr->tfs_info,
                        UE_MAX_TRCH * sizeof(mac_ul_tfs_type));

          /* Save TFS Info into the ordered TFC config data */
          WCDMA_MEMCPY (oc_ul_tfcs_ptr->tfs_info,
                        UE_MAX_TRCH * sizeof(mac_ul_tfs_type), 
                        cfgptr->tfcs_ptr->tfs_info, 
                        UE_MAX_TRCH * sizeof(mac_ul_tfs_type));

          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFS inforamtion is intialized ");
        }
        else
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "oc_ul_tfcs_ptr is NULL");
        }
#endif

      /* Store the PDU size during re-configuration */
        mac_ul_assign_pdu_size();
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Pointer to UL Transport Format Combination Set information is NULL");
      return;
    }
    
    /* Update the transport channel info list for traffic volume measurements */
    mac_ul_update_trch_info();


    L2_ACQ_UL_LW_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
    L2_ACQ_UL_LW_UNLOCK(&e_ul_isr_l2_datapath_mutex);

    

    /* Send DL MAC configuration data to QXDM */
    mac_ul_send_cfg_log_packet(&cfgptr);
    mac_ul_send_rach_param_packet(&cfgptr->rach_tx_cntl_info);

    /* Store the Transport channel IDs */
    if (cfgptr->cctrch_ptr->cctrch_type == RACH)
    {
      /* If CCTRCH type = RACH */
      trch_id[0] = (uint8)cfgptr->cctrch_ptr->rach_info.trch_id;
      mac_ul_send_tfcs_cfg_packet(cfgptr->tfcs_ptr, &trch_id[0]);
    }
    else if (cfgptr->cctrch_ptr->cctrch_type == DCH)
    {
      /* If CCTRCH type = DCH */
      for (j = 0; j < cfgptr->cctrch_ptr->dch_info.ndchs;j++)
      {
        trch_id[j] = (uint8)cfgptr->cctrch_ptr->dch_info.dch_info[j].trch_id;
      }
      mac_ul_send_tfcs_cfg_packet(cfgptr->tfcs_ptr, &trch_id[0]);
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Invalid CCTRCH type ");
    }

  }

#ifdef FEATURE_WCDMA_HSUPA

  L2_ACQ_UL_LW_LOCK(UE_MAX_UL_LOGICAL_CHANNEL);

  mac_hs_ul_reconfig_in_progress = TRUE;

  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Mac reconfig in progress");

  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "RLC action recieved in Re-config(1-Susp, 2-Resume) %d",
                                             cfgptr->ul_rlc_action );
  if(cfgptr->ul_rlc_action == CRLC_SUSPEND)
  {
    rlc_ul_update_config_pending(TRUE);
  }
  else if(cfgptr->ul_rlc_action == CRLC_RESUME)
  {
    rlc_ul_update_config_pending(FALSE);
  }
  
  /* If action is NOOP then check for the reset indicator flag */
  mac_hs_ul_config_req_handler(cfgptr);
  
  L2_ACQ_UL_LW_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
  L2_ACQ_UL_LW_UNLOCK(&e_ul_isr_l2_datapath_mutex);



#endif
  L2_MUTEX_LOCK(&mac_config_mutex);
  mac_ul_config_in_progress = FALSE;
  L2_MUTEX_UNLOCK(&mac_config_mutex);

  /* Enable Tx after re-configuring MAC is complete
        and only if reconfigured CCTrCh is a DCH */
  if (GET_UL_CCTRCH_TYPE == DCH)
  {
    block_transmission = FALSE;
  }
}


#ifdef FEATURE_WCDMA_MULTIRATE_AMR
/*====================================================================
FUNCTION: mac_ul_compute_amr_rate()

DESCRIPTION:
  This function looks at the uplink tfcs table and computes the new
  rate based on what TFCI's are allowed at this point. Based on the
  allowed TFCI's the rate is computed and vocoder is configured with
  this new rate.

  This function is used to change rates for only the AMR. No effect
  on the PS.

DEPENDENCIES:
  VOCODER has to support multiple rates. RRC will have to support
  multiple rates for AMR.

RETURN VALUE:


SIDE EFFECTS:
  There is a worst case delay of 40ms and we can lose upto one frame
  in the process of siwtching the rates

====================================================================*/
void
mac_ul_compute_amr_rate
(
  mac_amr_mode_info_type *mac_amr_mode_info_ptr
)
{
  ul_trch_info_type           *int_tc_ptr;        /* Pointer to Transport channel info */
  ul_lc_info_type             *int_lc_ptr;        /* Pointer to Logical channel info */
  ul_tfcs_info_type           *tfci_ptr;         /* pointer to TFCS information stored internally in MAC */
  uint8                       ix, iy, iz;
  uint8                       tfi;
  uint16                      lowest_amr_rate_tfci = 0;
  boolean                     found_amr_rate = FALSE;
  mvs_amr_mode_type           amr_mode = MVS_AMR_MODE_0475;
  mvs_amr_mode_type           lowest_amr_rate = MVS_AMR_MODE_MAX;

  uint8 index=0;
  uint16 amr_class_size[3];

  mvs_amr_mode_type  current_amr_mode = MVS_AMR_MODE_0475;
  uint16 total_bits;
  boolean found  = TRUE;

  wcdma_ul_amr_rate_event_type start_event_log;

  start_event_log.num_rates =0;
  start_event_log.codec_type =RRC_NO_CODEC;/*In valid*/
  for(ix=0;ix< 9; ix++)
  {
    start_event_log.amr_rate[ix] = MAC_AMR_MODE_UNDEF;
  }

  /* Initialize the values in the structure to max values */
  mac_amr_mode_info_ptr->amr_rate = MVS_AMR_MODE_0475;
  mac_amr_mode_info_ptr->lowest_amr_rate = MVS_AMR_MODE_MAX;
  mac_amr_mode_info_ptr->amr_rate_type = AMR_RATE_UNDEFINED;

  if (mac_enable_amr_debug_dump)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Get AMR Mapping info");
  }

  /*
  * Get the RLC Logical Channel IDs for UL, and use
  * RB IDs extracted here to get Logical Channel IDs
  * for DL
  */
  for (ix=0; ix < oc_ul_tfcs_ptr->ntfc; ix++)
  {
    index =0;
    found = TRUE;
    /* Since for every rate it iterated, reset the values  */
    amr_class_size[0] =0;
    amr_class_size[1]=0;
    amr_class_size[2]=0;
    
    /* Get the pointer to the tfci information stored internally in MAC */
    tfci_ptr = &int_ul_tfcs_info[ix];

    /* Check if the TFC is allowed*/
    if ((oc_ul_tfcs_ptr->tfc_info[ix].control_flag == TFC_MANDATORY) || 
        ((oc_ul_tfcs_ptr->tfc_info[ix].control_flag == TFC_ALLOWED) && 
         (tfci_ptr->control_flag == TFC_ALLOWED))
       )
    {
      /* Go through the number of transport channels configured*/
      for (iy=0; iy <  int_ul_cctrch_info.ntrch; iy++)
      {

        int_tc_ptr = GET_UL_TRCH_INFO_PTR(iy);

        int_lc_ptr = GET_UL_LC_INFO_PTR(int_tc_ptr->rlc_id[0]);

        if ((int_lc_ptr->lc_type == UE_LOGCHAN_DTCH) &&
            (int_lc_ptr->rlc_mode == UE_MODE_TRANSPARENT))
        {
          /* Since we allocated for only 3 RBs for voice */
          if(index >= 3 )
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "only 3 transport channel allowed index> %d so break ",
                                                       index);
            break;
          }
          /* Get the transport format index for TFC "ix" and TFI corresponding to Transoprt channel "iy"*/
          tfi = oc_ul_tfcs_ptr->tfc_info[ix].tfi[iy];
          if (mac_enable_amr_debug_dump)
          {
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "tb size %d, num blks %d tfi %d",
                                                       oc_ul_tfcs_ptr->tfs_info[iy].tf_info[tfi].tb_size,
                                                       oc_ul_tfcs_ptr->tfs_info[iy].tf_info[tfi].ntblks,
                                                       tfi);
          }
          /* If the number of transport blocks are > 0 */
          if (oc_ul_tfcs_ptr->tfs_info[iy].tf_info[tfi].ntblks > 0)
          {
            /* Check the TB_size*/
            amr_class_size[index] = oc_ul_tfcs_ptr->tfs_info[iy].tf_info[tfi].tb_size;
                     /* update the rlc_id, rb_id for Class A*/

            index++;
          }
        }
      }


      total_bits = amr_class_size[0] + amr_class_size[1] +amr_class_size[2];
      if (mac_enable_amr_debug_dump)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Total bits %d in amr mode",
                                                   total_bits);
      }
      switch(total_bits)
      {
#ifdef FEATURE_VOC_AMR_WB
        case AMR_WB_MODE_660_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_0660;
        }
        break;
        case AMR_WB_MODE_885_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_0885;
        }
        break;
        case AMR_WB_MODE_1265_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_1265;
        }
        break;
        case AMR_WB_MODE_1425_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_1425;
        }
        break;
        case AMR_WB_MODE_1585_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_1585;
        }
        break;
        case AMR_WB_MODE_1825_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_1825;
        }
        break;
        case AMR_WB_MODE_1985_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_1985;
        }
        break;
        case AMR_WB_MODE_2305_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_2305;
        }
        break;
        case AMR_WB_MODE_2385_TOTAL_BITS :
        {
          current_amr_mode = MVS_AMR_MODE_2385;
        }
        break;
#endif/* FEATURE_VOC_AMR_WB*/
        case AMR_MODE_122_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_1220;
        }
        break;
        case AMR_MODE_102_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_1020;
        }
        break;
        case AMR_MODE_795_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0795;
        }
        break;
        case AMR_MODE_740_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0740;
        }
        break;
        case AMR_MODE_670_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0670;
        }
        break;
        case AMR_MODE_590_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0590;
        }
        break;
        case AMR_MODE_515_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0515;
        }
        break;
        case AMR_MODE_475_TOTAL_BITS  :
        {
          current_amr_mode = MVS_AMR_MODE_0475;
        }
        break;
        default:
          if((total_bits == AMR_CLASS_NO_DATA_FRAME_SIZE)
#ifdef FEATURE_VOC_AMR_WB
             || (total_bits == AMR_WB_CLASS_A_BIT_SIZE)
#endif/* FEATURE_VOC_AMR_WB*/
             ||(total_bits == AMR_MODE_8_CLASS_A_BIT_SIZE))
          {
            //MSG_HIGH("SID frames found or No dat frames found %d", total_bits,0,0);
          }
          else
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "wrong total size %d for  AMR",
                                                       total_bits);
          }
          found = FALSE;
        break;
      }
      if(found == FALSE)
      {
        if(mac_enable_amr_debug_dump)
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "SID frames found or No dat frames found %d",
                                                     total_bits);
        }
      }     
      else
      {
        boolean rate_present_in_event = FALSE;
        found_amr_rate = TRUE;
        
        if (mac_enable_amr_debug_dump)
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Total bits %d and  amr mode is %d",
                                                     total_bits,
                                                     current_amr_mode);
        }
        
        for(iz =0; (iz<start_event_log.num_rates) && (iz<(sizeof(start_event_log.amr_rate)/sizeof(rrc_event_ul_amr_rate_type)));iz++)
        {
          if(start_event_log.amr_rate[iz] == ((rrc_event_ul_amr_rate_type)current_amr_mode) )
          {
            rate_present_in_event = TRUE;
            break;
          }
        }
        if((rate_present_in_event == FALSE) && 
            (start_event_log.num_rates < (sizeof(start_event_log.amr_rate)/sizeof(rrc_event_ul_amr_rate_type))))
        {
          start_event_log.amr_rate[start_event_log.num_rates] =
              (rrc_event_ul_amr_rate_type) current_amr_mode;
          start_event_log.num_rates++;
        }
        
        if( amr_mode < current_amr_mode)
        {
          if (mac_enable_amr_debug_dump)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Multirate wb-amr lower amr rate %d higher rate %d ",
                                                       amr_mode,
                                                       current_amr_mode);
          }
          amr_mode = current_amr_mode;
        }

        if(current_amr_mode < lowest_amr_rate)
        {
          lowest_amr_rate_tfci = ix;
        }
        lowest_amr_rate = MIN(current_amr_mode,lowest_amr_rate);  
      }

    }
  }
  
  /* EVENT_WCDMA_UL_AMR_RATE - logs the All UL AMR allowed Rates */
  start_event_log.codec_type =RRC_NB_AMR_CODEC;
  event_report_payload(EVENT_WCDMA_UL_AMR_RATE, sizeof(start_event_log),
                       (void *)&start_event_log);
  
  if (found_amr_rate)
  {
    mac_amr_mode_info_ptr->amr_rate = amr_mode;
    mac_amr_mode_info_ptr->lowest_amr_rate = lowest_amr_rate;
    mac_amr_mode_info_ptr->lowest_amr_rate_tfci = lowest_amr_rate_tfci;
    
    tfci_ptr = &int_ul_tfcs_info[lowest_amr_rate_tfci];
    tfci_ptr->control_flag = TFC_MANDATORY;
    oc_ul_tfcs_ptr->tfc_info[lowest_amr_rate_tfci].control_flag = TFC_MANDATORY;

    mac_amr_mode_info_ptr->amr_rate_type =
    ((amr_mode == lowest_amr_rate) ? SINGLE_RATE_AMR : MULTI_RATE_AMR);
    if (mac_enable_amr_debug_dump)
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "AMR rate %d",
                                                 mac_amr_mode_info_ptr->amr_rate);
    }
  }
  else
  {
    mac_amr_mode_info_ptr->amr_rate = MVS_AMR_MODE_MAX;
    mac_amr_mode_info_ptr->lowest_amr_rate = MVS_AMR_MODE_MAX;
  }
} /* end mac_ul_compute_amr_rate() */

/*==============================================================================================
 FUNCTION NAME  void mac_ul_set_amr_rate

 DESCRIPTION : This fn. will set the vocoder rate
===============================================================================================*/
void mac_ul_set_amr_rate(mvs_amr_mode_type amr_mode)
{

#ifndef FEATURE_VOC_AMR_WB

  if (amr_mode < MVS_AMR_MODE_MAX)
  {
  
#ifdef FEATURE_MVS_UNDER_GHDI
    if ( ghdi_mvs_get_ul_amr_mode() != amr_mode)
#else
    if ( rrc_mvssup_get_ul_amr_mode() != amr_mode)
#endif /* FEATURE_MVS_UNDER_GHDI */
    {

#ifdef FEATURE_MVS_UNDER_GHDI
      ghdi_mvs_amr_set_amr_mode(amr_mode);
#else
      rrc_mvssup_amr_set_amr_mode(amr_mode);
#endif /* FEATURE_MVS_UNDER_GHDI */
      
      switch (amr_mode)
      {
        case MVS_AMR_MODE_1220:
        case MVS_AMR_MODE_1020:
        case MVS_AMR_MODE_0795:
        case MVS_AMR_MODE_0740:
        case MVS_AMR_MODE_0670:
        case MVS_AMR_MODE_0590:
        case MVS_AMR_MODE_0515:
        case MVS_AMR_MODE_0475:
          if(mac_enable_amr_debug_dump)
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS mode changed to %d",
                                                       amr_mode);
          }
          break;
        default:
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "No match for MVS AMR mode %d",
                                                      amr_mode);
          break;
      }
    }
    else
    {
      if(mac_enable_amr_debug_dump)
      {  
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS AMR mode not changed");
      }
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MVS AMR mode not SET !!");
  }

  #else

  if (amr_mode < MVS_AMR_MODE_MAX)
  {
    if( mac_mvs_codec_info == MVS_MODE_AMR )
    {
#ifdef FEATURE_MVS_UNDER_GHDI
      if ( ghdi_mvs_get_ul_amr_mode() != amr_mode)
#else
      if ( rrc_mvssup_get_ul_amr_mode() != amr_mode)
#endif /* #ifdef FEATURE_MVS_UNDER_GHDI */
      {
#ifdef FEATURE_MVS_UNDER_GHDI
        ghdi_mvs_amr_set_amr_mode(amr_mode);
#else
        rrc_mvssup_amr_set_amr_mode(amr_mode);
#endif /* FEATURE_MVS_UNDER_GHDI */

        switch (amr_mode)
        {
          case MVS_AMR_MODE_1220:
          case MVS_AMR_MODE_1020:
          case MVS_AMR_MODE_0795:
          case MVS_AMR_MODE_0740:
          case MVS_AMR_MODE_0670:
          case MVS_AMR_MODE_0590:
          case MVS_AMR_MODE_0515:
          case MVS_AMR_MODE_0475:
            if(mac_enable_amr_debug_dump)
            {
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS mode changed to %d",
                                                         amr_mode);
            }
            break;
          default:
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "No match for MVS AMR mode %d",
                                                        amr_mode);
            break;
        }
      }
      else
      {
        if(mac_enable_amr_debug_dump)
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS AMR mode not changed");
        }
      }
    }
    else
    {

#ifdef FEATURE_MVS_UNDER_GHDI
      if ( ghdi_mvs_get_ul_awb_mode() != amr_mode)
#else
      if ( rrc_mvssup_get_ul_awb_mode() != amr_mode)
#endif /* FEATURE_MVS_UNDER_GHDI */
      {
      
#ifdef FEATURE_MVS_UNDER_GHDI
        ghdi_mvs_amr_set_awb_mode(amr_mode);
#else
        rrc_mvssup_amr_set_awb_mode(amr_mode);
#endif /* FEATURE_MVS_UNDER_GHDI */

        switch (amr_mode)
        {
          case MVS_AMR_MODE_0660:
          case MVS_AMR_MODE_0885:
          case MVS_AMR_MODE_1265:
          case MVS_AMR_MODE_1425:
          case MVS_AMR_MODE_1585:
          case MVS_AMR_MODE_1825:
          case MVS_AMR_MODE_1985:
          case MVS_AMR_MODE_2305:
           case MVS_AMR_MODE_2385:
            if(mac_enable_amr_debug_dump)
            {
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS mode changed to %d",
                                                         amr_mode);
            }
            break;
          default:
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "No match for MVS WB_AMR mode %d",
                                                        amr_mode);
            break;
        }
      }
      else
      {
        if(mac_enable_amr_debug_dump)      
        {  
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MVS WB_AMR mode not changed");
        }
      }
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MVS AMR mode not SET !!");
  }

#endif
}

#endif


/*=================================================================================================
 FUNCTION NAME  proc_ul_mac_tfcc_command

 PURPOSE    This function process UL TFCC config Commands
================================================================================================*/

void proc_ul_mac_tfcc_command
(
  mac_tfc_subset_restriction_info_type *tfcc_config_ptr
)
{
  uint8                       tfc_idx, ntrch, ntf, tfi,trch_idx,i;
  uint16                      tfci;
  uint16                      *tfci_list_ptr, ntfc;
  trch_id_to_idx_map_type     *id_to_idx_ptr;
  mac_tfc_subset_restriction_trch_info_type *trch_ptr;
  boolean                     status = TRUE;
  rrc_cmd_type                *rrc_cmd_ptr;
  boolean                     found_tfi = FALSE;

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
  mac_amr_mode_info_type      amr_mode_info;
#endif /* FEATURE_WCDMA_MULTIRATE_AMR */

  // Number of TFC's configured by RRC initially.
  if (oc_ul_tfcs_ptr->ntfc == 0)
  {
    status = FALSE;
  }
  else
  {
    //Initialize all entries to not_allowed
    memset(mac_ul_pending_tfcs_control_table,(uint8) TFC_NOT_ALLOWED,
               UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

    switch (tfcc_config_ptr->tfci_restriction_choice)
    {
      case MIN_ALLOWED_TFCI:
        // Get the minimum not_allowed tfi
        tfci = tfcc_config_ptr->restriction_info.tfci ;

        // Check if this TFCI is within the range
        if ((tfci > UE_MAX_TFC) || (tfci > oc_ul_tfcs_ptr->ntfc))
        {
          // If not, then set the status to FALSE.
          status = FALSE;

          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TFCI %d not among the list of configured tfci's %d",
                                                      tfci,
                                                      oc_ul_tfcs_ptr->ntfc);

          break; // Breaks ffrom the case sattement
        }

        // Starting from the TFCI listed upto the number of
        for (tfc_idx = 0;
            ((tfc_idx <= tfcc_config_ptr->restriction_info.tfci) && (tfc_idx < UE_MAX_TFC));
             tfc_idx++ )
        {
          #ifdef FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
          if (oc_ul_tfcs_ptr->tfc_info[tfc_idx].control_flag != TFC_REMOVED)
          #endif
          {
            // Mark those allowed which are less than the min tfci specified.
            mac_ul_pending_tfcs_control_table[tfc_idx] = TFC_ALLOWED;
          }
        }

        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Recvd MIN_ALLOWED %d tfci",
                                                   tfci);

        break;

      case ALLOWED_TFCI:
        ntfc = tfcc_config_ptr->restriction_info.tfci_restriction_info.ntfc;
        tfci_list_ptr = &tfcc_config_ptr->restriction_info.tfci_restriction_info.tfci_list[0];

        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "ALLOWED TFCI's are ");

        if (ntfc <= oc_ul_tfcs_ptr->ntfc)
        {
          // GO through the list of TFCI's that are allowed
          for (tfc_idx = 0; tfc_idx < ntfc; tfc_idx++)
          {
            // Get the TFCI
            tfci = tfci_list_ptr[tfc_idx];

            if (tfci > oc_ul_tfcs_ptr->ntfc)
            {
              status = FALSE;

              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TFCI %d not among the list of configured tfci's %d",
                                                          tfci,
                                                          oc_ul_tfcs_ptr->ntfc);
              break;
            }
            #ifdef FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
            if (oc_ul_tfcs_ptr->tfc_info[tfci].control_flag != TFC_REMOVED)
            #endif
            {
              mac_ul_pending_tfcs_control_table[tfci] = TFC_ALLOWED;
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCI %d", tfci);
            }
          }
        }
        else
        {
          status = FALSE;
        }

        break;

      case NOT_ALLOWED_TFCI:

        ntfc = tfcc_config_ptr->restriction_info.tfci_restriction_info.ntfc;
        tfci_list_ptr = &tfcc_config_ptr->restriction_info.tfci_restriction_info.tfci_list[0];

        if (ntfc <= oc_ul_tfcs_ptr->ntfc)
        {
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "NOT ALLOWED TFCI's are ");

          //Initialize all entries in the TFCS table to allowed
          memset(mac_ul_pending_tfcs_control_table, (uint8)TFC_ALLOWED,
           oc_ul_tfcs_ptr->ntfc * sizeof(mac_ul_tfc_control_e_type));

          // GO through the list of NOT ALLOWED TFCI's
          for (tfc_idx = 0; tfc_idx < ntfc; tfc_idx++)
          {
            // Get the tfci
            tfci = tfci_list_ptr[tfc_idx];

            // If tfci exceeds the max_number configured then reject the msg
            if (tfci > oc_ul_tfcs_ptr->ntfc)
            {
              status = FALSE;

              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TFCI %d not among the list of configured tfci's %d",
                                                          tfci,
                                                          oc_ul_tfcs_ptr->ntfc);
              break;
            }
            else
            {
              #ifdef FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
              if (oc_ul_tfcs_ptr->tfc_info[tfci].control_flag == TFC_REMOVED)
              {
                mac_ul_pending_tfcs_control_table[tfci] = TFC_NOT_ALLOWED;
                MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCI %d", tfci);
              }
              #endif
              // Mark it as NOT allowed
              mac_ul_pending_tfcs_control_table[tfci] = TFC_NOT_ALLOWED;

              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCI %d", tfci);
            }
          }
        }

        break;

      case RESTRICTED_TFCI:

        ntrch = tfcc_config_ptr->restriction_info.trch_restriction_info_list.ntrch;

        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Restricted TFCI list is ");

        //Initialize all entries to Allowed
        memset(mac_ul_pending_tfcs_control_table,(uint8) TFC_ALLOWED,
               UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        for (trch_idx = 0; trch_idx < ntrch; trch_idx++ )
        {
          trch_ptr = &tfcc_config_ptr->restriction_info.
                     trch_restriction_info_list.trch_restriction_info[trch_idx];

          // Get the number of transport formats that are restricted.
          ntf = (uint8)trch_ptr->ntf;

          // Get the info for this transort channel id
          id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_ptr->trch_id);

          // If the transport channel exists in MAC
          if (id_to_idx_ptr->valid)
          {
            // If the number of transport formats specified in the TFCC
            // msg exceeds the number of transport formats specified in the
            // TFCS table, then discard this message and send failure to RRC
            if (ntf > oc_ul_tfcs_ptr->tfs_info[id_to_idx_ptr->idx].ntf)
            {
              status = FALSE;

              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Mismatch in ntf %d in TFCC msg and ntf %d in TFCS table",
                                                          ntf,
                                                          oc_ul_tfcs_ptr->tfs_info[id_to_idx_ptr->idx].ntf);
              break;
            }

            for (tfc_idx = 0; tfc_idx < oc_ul_tfcs_ptr->ntfc; tfc_idx++ )
            {
              /* Don't check TFCI which are made as Not allowed by the previous TrCh */
              if (mac_ul_pending_tfcs_control_table[tfc_idx] == TFC_NOT_ALLOWED)
              {
                continue;
              }

              found_tfi = FALSE;

              // Go through the number of tranport formats
              for (i=0;i<ntf;i++)
              {
                // Get the transport format index.
                tfi = trch_ptr->tfi[i];
                MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCI %d, Trch %d, TF %d",
                                                           tfc_idx,
                                                           trch_ptr->trch_id,
                                                           tfi);
                if (oc_ul_tfcs_ptr->tfc_info[tfc_idx].tfi[id_to_idx_ptr->idx] == tfi)
                {
                  if ((oc_ul_tfcs_ptr->tfc_info[tfc_idx].control_flag != TFC_REMOVED)
                      &&  (mac_ul_pending_tfcs_control_table[tfc_idx] == TFC_ALLOWED))
                  {
                    mac_ul_pending_tfcs_control_table[tfc_idx] = TFC_ALLOWED;
                    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "AllowedList : TFCI %d, Trch %d, TF %d",
                                                               tfc_idx,
                                                               trch_ptr->trch_id,
                                                               tfi);
                  }
                  found_tfi=TRUE;
                  break;
                }
              }

              if (found_tfi==FALSE)
              {
                mac_ul_pending_tfcs_control_table[tfc_idx] = TFC_NOT_ALLOWED;
              }
            }
          }
          else
          {
            // The config is invalid.
            status = FALSE;
            break;
          }
        }
        break;

      case FULL_TFCI:
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Full TFCI choice selected");
        //Initialize all entries in the TFCS table to allowed
        WCDMA_MEMCPY(mac_ul_pending_tfcs_control_table,
                     sizeof(mac_ul_pending_tfcs_control_table),
                     mac_ul_full_tfcs_control_table,
                     sizeof(mac_ul_full_tfcs_control_table));

        for (tfc_idx = 0;tfc_idx < mac_ul_tfcs_table.ntfc; tfc_idx++)
        {
          if(mac_ul_pending_tfcs_control_table[tfc_idx] == TFC_NOT_ALLOWED)
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Making NOT_ALLOWED TFCI to ALLOWED for TFCI tfc_idx %d",
                                                       tfc_idx);
            mac_ul_pending_tfcs_control_table[tfc_idx] = TFC_ALLOWED;
          }
        }
        break;
      default:
        status = FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Error Not a valid choice");
        break;
      }
  }

  if (tfcc_config_ptr->control_duration > 5120)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Control duration is invalid %d",
                                                tfcc_config_ptr->control_duration);

    status = FALSE;
  }

  // If all parameters are valid
  if (status == TRUE)
  {
    // Always use the current config. Only when the control duration
    // expires use the default.
    //mac_ul_tfcs_control_ptr_in_use = CURRENT_TFCC_TABLE;

    if (tfcc_config_ptr->control_duration == 0)
    {
      if (tfcc_config_ptr->activation_time == 0)
      {

        mac_ul_tfcc_config_in_process = TRUE;
        
        // In this case bothe curren config and default config will be the same.

        /* Clear uplink TFCS information table internally used by MAC */
        WCDMA_MEMCPY(mac_ul_default_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type),
                     mac_ul_pending_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        /* Clear uplink TFCS information table internally used by MAC */
        WCDMA_MEMCPY(mac_ul_current_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type),
                     mac_ul_pending_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        for (tfc_idx = 0;tfc_idx < mac_ul_tfcs_table.ntfc; tfc_idx++)
        {
          // Update the TFCS table with this control flag.
          mac_ul_tfcs_table.tfc_info[tfc_idx].control_flag =
            mac_ul_current_tfcs_control_table[tfc_idx];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
          // If this TFCI has not moved to blocked state then initialize
          if (int_ul_tfcs_info[tfc_idx].power_state != BLOCKED_STATE)
#endif
          {
            int_ul_tfcs_info[tfc_idx].control_flag =
              mac_ul_current_tfcs_control_table[tfc_idx];

            if (int_ul_tfcs_info[tfc_idx].control_flag == TFC_ALLOWED)
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCI %d allowed (int_ul), %d",
                                                         tfc_idx,
                                                         mac_ul_tfcs_table.tfc_info[tfc_idx].control_flag);
            }
          }

        }

        // Indicate that there is no pending config.
        mac_ul_tfcc_config_pending = FALSE;

        // Clear any timers that may be running
        (void)rex_clr_timer(&ul_tfc_ctrl_tmr_blk);

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
       if((rab_status == CS_RABS) ||
          (rab_status == CS_PLUS_PS_RABS))
       {
         mac_ul_compute_amr_rate(&amr_mode_info);
         mac_ul_set_amr_rate(amr_mode_info.amr_rate);
       }
#endif
       mac_compute_minimum_set_of_tfcs(
                                       #ifdef FEATURE_WCDMA_MULTIRATE_AMR
                                       &amr_mode_info
                                       #endif
                                       );

       mac_ul_tfcc_config_in_process = FALSE;
      }
      else
      {
        // Indicate that there is pending config.
        mac_ul_tfcc_config_pending = TRUE;
        mac_ul_tfcc_activation_cfn = tfcc_config_ptr->activation_time;
        mac_ul_tfcs_pending_control_duration = tfcc_config_ptr->control_duration;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCC msg act_cfn %d",
                                                   mac_ul_tfcc_activation_cfn);
      }
    }
    else
    {
      //MSG_HIGH("Control Duration is not supported now", 0, 0,0);

      if (tfcc_config_ptr->activation_time == 0)
      {
        mac_ul_tfcc_config_in_process = TRUE;
        
        /* Clear uplink TFCS information table internally used by MAC */
        WCDMA_MEMCPY(mac_ul_current_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type),
                     mac_ul_pending_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        for (tfc_idx = 0;tfc_idx < mac_ul_tfcs_table.ntfc; tfc_idx++)
        {
          mac_ul_tfcs_table.tfc_info[tfc_idx].control_flag =
            mac_ul_current_tfcs_control_table[tfc_idx];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
          // If this TFCI has not moved to blocked state then initialize
          if (int_ul_tfcs_info[tfc_idx].power_state != BLOCKED_STATE)
#endif
          {
            int_ul_tfcs_info[tfc_idx].control_flag =
              mac_ul_current_tfcs_control_table[tfc_idx];
          }
        }

        mac_ul_tfcc_config_pending = FALSE;

        // Clear any old timers if it is running
        (void)rex_clr_timer(&ul_tfc_ctrl_tmr_blk);

        // Set this new timer.
        (void)rex_set_timer(&ul_tfc_ctrl_tmr_blk,(uint32)(tfcc_config_ptr->control_duration));
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Started timer for duration %d for this TFCC config",
                                                   tfcc_config_ptr->control_duration);

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
        if((rab_status == CS_RABS) ||
          (rab_status == CS_PLUS_PS_RABS))
        {
          mac_ul_compute_amr_rate(&amr_mode_info);
          mac_ul_set_amr_rate(amr_mode_info.amr_rate);
        }
#endif
        mac_compute_minimum_set_of_tfcs(
                                        #ifdef FEATURE_WCDMA_MULTIRATE_AMR
                                        &amr_mode_info
                                        #endif
                                        );

        mac_ul_tfcc_config_in_process = FALSE;
      }
      else
      {
        mac_ul_tfcc_config_pending = TRUE;
        mac_ul_tfcc_activation_cfn = tfcc_config_ptr->activation_time;
        mac_ul_tfcs_pending_control_duration = tfcc_config_ptr->control_duration;
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCC msg act_cfn %d, control_duration %d",
                                                   mac_ul_tfcc_activation_cfn,
                                                   mac_ul_tfcs_pending_control_duration);
      }
    }

    oc_ul_tfcs_ptr = &mac_ul_tfcs_table;
  }
#ifndef FEATURE_TFCC_ENHNC
  else
  {
#endif
    // Post a command to RRC
    /* Failed to allocate memory for RRC command */
    if ((rrc_cmd_ptr = rrc_get_mac_cmd_buf()) == NULL) /*lint !e539*/
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR , "Could not get RRC cmd buffer");
    }
    /* Send RRC_CMAC_UL_TFC_SUBSET_CFG_CNF message to RRC */
    else /*lint !e539*/
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Sending RRC_CMAC_UL_TFC_SUBSET_CFG_CNF to RRC");

      /*Note : This feature definition would be removed only when RRC also removes
      this FEATURE definition */
      /* Fill in command header */
      rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CMAC_UL_TFC_SUBSET_CFG_CNF;

      /* Fill in command data */
      rrc_cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.transaction_id =
        tfcc_config_ptr->transaction_id;
      rrc_cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.rb_id =
        tfcc_config_ptr->rb_id;
#ifdef FEATURE_TFCC_ENHNC
      rrc_cmd_ptr->cmd.mac_tfc_subset_cfg_cnf_info.status = status;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "TFCC:Sending RRC_CMAC_UL_TFC_SUBSET_CFG_CNF to RRC status %d",
                                                  status);
#endif
      /* Send command to RRC */
      rrc_put_mac_cmd(rrc_cmd_ptr);
    }
#ifndef FEATURE_TFCC_ENHNC
  }
#endif
}

/*=================================================================================================
 FUNCTION NAME  mac_ul_handle_tfcs_control_timer_expiry

 PURPOSE    This function process expiry of the control duration
================================================================================================*/

void mac_ul_handle_tfcs_control_timer_expiry(void)
{
  uint16 tfc_idx;

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
  mac_amr_mode_info_type      amr_mode_info;
#endif /* FEATURE_WCDMA_MULTIRATE_AMR */

  for (tfc_idx = 0;tfc_idx < mac_ul_tfcs_table.ntfc; tfc_idx++)
  {
    // Start using the default config from this point.
    mac_ul_tfcs_table.tfc_info[tfc_idx].control_flag =
      mac_ul_default_tfcs_control_table[tfc_idx];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
    // If this TFCI has not moved to blocked state then initialize
    if (int_ul_tfcs_info[tfc_idx].power_state != BLOCKED_STATE)
#endif
    {
      int_ul_tfcs_info[tfc_idx].control_flag =
      mac_ul_default_tfcs_control_table[tfc_idx];
    }

    // Memset the current config.
    memset(mac_ul_current_tfcs_control_table, (uint8)TFC_NOT_ALLOWED,
        UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));
  }
#ifdef FEATURE_WCDMA_MULTIRATE_AMR
  if((rab_status == CS_RABS) ||
    (rab_status == CS_PLUS_PS_RABS))
  {
    mac_ul_compute_amr_rate(&amr_mode_info);
    mac_ul_set_amr_rate(amr_mode_info.amr_rate);
  }
#endif
  mac_compute_minimum_set_of_tfcs(
                                  #ifdef FEATURE_WCDMA_MULTIRATE_AMR
                                  &amr_mode_info
                                  #endif
                                  );

  MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Timer expired for the TFCC msg");
}


/*=================================================================================================
 FUNCTION NAME  proc_ul_mac_commands

 PURPOSE    This function process UL MAC Commands
================================================================================================*/
void  proc_ul_mac_commands
(
  /* Command ID */
  wcdma_l2_ul_cmd_enum_type   cmd_id,

  /* Pointer to command data */
  wcdma_l2_ul_cmd_data_type   *cmd_data_ptr
)
{
  /* Process the command data based on command ID */
  switch (cmd_id)
  {

    /* UL MAC Congiguration Request */
    case CMAC_UL_CONFIG_REQ:

      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Rcvd CMAC_UL_CONFIG_REQ");

      /*-----------------------------------------------------------------------------
      MAC sets ul_configuration to TRUE when RRC asks MAC to wait for the signal
      from L1.
      ------------------------------------------------------------------------------*/
      if (cmd_data_ptr->mac_config_info.wait_for_l1_sig)
      {
        /* Clear the memory */
        memset(&ul_cfginfo,0,sizeof(mac_ul_config_type));

        /* Save the pointer */
        WCDMA_MEMCPY(&ul_cfginfo,
                     sizeof(mac_ul_config_type),
                     &cmd_data_ptr->mac_config_info, 
                     sizeof(mac_ul_config_type));;

        /*Set the configuration pending flag to TRUE to indicate that MAC is */
        /* waiting for the signal from L1 to configure UL MAC */
        ul_configuration_pending=TRUE;

        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_MED, "Waiting for L1 signal to reconfigure");
      }
      /*-----------------------------------------------------------------------------
      If the configuration received from RRC has nothing related to activation time
      then reconfigure MAC now.
      ------------------------------------------------------------------------------*/
      else
      {
        config_ul_mac (&cmd_data_ptr->mac_config_info);
      }
    break;


    /* UL MAC traffic volume vonfiguration request */
  case CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ:
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Rcvd CMAC_UL_TRAFFIC_VOLUME_CONFIG_REQ");
    config_ul_mac_traffic_volume(cmd_data_ptr->ul_traffic_volume_cfg);
    break;

    /* UL MAC block DTCH transmissions on RACH */
  case CMAC_UL_BLOCK_RACH_TRAFFIC_REQ:
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Rcvd CMAC_UL_BLOCK_RACH_TRAFFIC_REQ");
    mac_ul_block_rach_tx(cmd_data_ptr->block_rach);
    break;


      /* RRC sends the TFCC config information */
    case CMAC_UL_TFC_SUBSET_CFG_REQ:
      proc_ul_mac_tfcc_command(&cmd_data_ptr->mac_tfc_subset_config_info);
      break;

    /* Invalid command */
  default:
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Rcvd invalid command %d",
                                                cmd_id);
    break;
  }
}


/*==============================================================================================
 FUNCTION NAME  build_ul_frame

 FUNCTION   This function processes build UL frame event
===============================================================================================*/
void  proc_build_ul_frame_ind
(
  void
)
{
  rlc_profile_dump_start(&mac_build_frame_profile_dump);

  /* Current CCTrCh Type */
  switch (GET_UL_CCTRCH_TYPE)
  {

    /* CCTrCh Type is RACH */
    case RACH:

      /* Build RACH Transport Blocks */
      build_rach_tblks (&bld_frame_ind_info);
      /* Done building RACH data */
      if (mac_build_pending == TRUE)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Current time %d",
                                                   stmr_get_current_time(STMR_INT_2));
      }

      bld_frame_ind_info.tx_buf_ptr->mac_busy = FALSE;
      break;

      /* CCTrCh type is DCH */
    case DCH:

  #ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH
      tfc_rlc_size_for_dch_defined = TRUE;
  #else
      tfc_rlc_size_for_dch_defined = FALSE;
  #endif

      /* Build UL Transport Blocks opeartion was successful */
      if (build_ul_tblks (&bld_frame_ind_info))
      {
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "DCH Frame sent to L1");

      }

      /* Done building DCH data */
      if (mac_build_pending == TRUE)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Current time %d",
                                                   stmr_get_current_time(STMR_INT_2));
      }

      bld_frame_ind_info.tx_buf_ptr->mac_busy = FALSE;

      break;

      /* Invalid CCTrCh type */
    default:
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "build_ul_frame: invalid cctrch type");
      break;
  }
  
  rlc_profile_dump_end(&mac_build_frame_profile_dump);
}

/*==============================================================================================
 FUNCTION NAME  void mac_handle_activation_cfn_expiry

 DESCRIPTION : This fn. is called to figure out if act-cfn has reached during
               Suspension-Resumption handling as well as regular handling of
               build_frame_ind
===============================================================================================*/
#ifdef __GNUC__
static inline void mac_handle_activation_cfn_expiry(uint8 cfn, uint8 i)
#else /* __GNUC__*/
__inline void mac_handle_activation_cfn_expiry(uint8 cfn, uint8 i)
#endif /* __GNUC__ */
{
  /*
   * Flag to see if have to check for saved ciphering configuration info
   */
  boolean check_for_saved_cipher_info = FALSE;

  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Expiry cnf 0x%x, i %d cfg is %d",
                                             cfn,
                                             i,
                                             cur_ul_cipher_info[i].cipher_cfg_type);

  ul_cipher_activation_pending[i] = FALSE;

  if ((new_ul_cipher_info[i].cipher_cfg_type == NON_HO_CIPHER_CFG) &&
      (TRUE == get_start_value_to_transmit[i]))
  {
    new_ul_cipher_info[i].hfn = rrc_get_start_value_to_transmit();

    get_start_value_to_transmit[i]=FALSE;

    /*
     * Indicate that we should check if ciphering configuration was saved.
     */
    check_for_saved_cipher_info = TRUE;

    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "New HFN from RRC 0x%x, cfn 0x%x, prev_cfn 0x%x",
                                               new_ul_cipher_info[i].hfn,
                                               cfn,
                                               prev_cfn);
  }
  else if (new_ul_cipher_info[i].cipher_cfg_type == SMC_INITIATED_NON_HO_CIPHER_CFG)
  {
    new_ul_cipher_info[i].cipher_cfg_type = NON_HO_CIPHER_CFG;
    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "New HFN from RRC 0x%x, cfn 0x%x, prev_cfn 0x%x",
                                               new_ul_cipher_info[i].hfn,
                                               cfn,
                                               prev_cfn);
  }
  else if (new_ul_cipher_info[i].cipher_cfg_type == HO_STEP2_CIPHER_CFG)
  {
    new_ul_cipher_info[i].hfn++;
    new_ul_cipher_info[i].cipher_cfg_type = NON_HO_CIPHER_CFG;

    /*
     * Indicate that we should check if ciphering configuration was saved.
     */
    check_for_saved_cipher_info = TRUE;

    /* For HO_STEP2 MAC need not get HFN to be applied at activation time from RRC. In this       
      * case, MAC would increment already provided HFN by 1 at activation time.
      * Reset get_start_value_to_transmit global, if already set to TRUE, as we no more
      * need HFN to be read from RRC.
      */
    get_start_value_to_transmit[i]=FALSE;

    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Step-2 key %d hfn 0x%x cfn 0x%x",
                                               new_ul_cipher_info[i].key_idx,
                                               new_ul_cipher_info[i].hfn,
                                               cfn);
  }
  else if(new_ul_cipher_info[i].cipher_cfg_type == NON_HO_STEP2_CIPHER_CFG)
  {
    
    /* MAC needs to update activation time from the received cmd and
     * MAC needs to get the HFN that needs to be applied at activation time from RRC. 
     */
    new_ul_cipher_info[i].hfn = rrc_get_start_value_to_transmit();
    
    get_start_value_to_transmit[i]=FALSE;
    
    new_ul_cipher_info[i].cipher_cfg_type = NON_HO_CIPHER_CFG;

    /*
     * Indicate that we should check if ciphering configuration was saved.
     */
    check_for_saved_cipher_info = TRUE;

    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "NON HO Step-2 key %d hfn 0x%x cfn 0x%x",
                                               new_ul_cipher_info[i].key_idx,
                                               new_ul_cipher_info[i].hfn,
                                               cfn);
  }

  // Copy the new configuration parameters
  cur_ul_cipher_info[i] = new_ul_cipher_info[i];

  /*
   * Check to see if we had received a new ciphering configuration before reaching
   * this activation time.
   */
  if (check_for_saved_cipher_info && saved_new_ul_cipher_info[i].cipher_info_saved)
  {
    /*
     * copy over the saved ciphering configuration into new ciphering configuration.
     */
    WCDMA_MEMCPY(&new_ul_cipher_info[i], 
                 sizeof(mac_cipher_config_type),
                 &saved_new_ul_cipher_info[i].cipher_info,
                 sizeof(mac_cipher_config_type));

    /*
     * Indicate that the ciphering configuration is pending and that HFN has to be
     * incremented by one upon CFN rollover.
     */
    ul_cipher_activation_pending[i] = mac_increment_hfn_on_rollover[i] = TRUE;

    /*
     * Reset prev_cfn flag
     */
    prev_cfn = INVALID_PREV_CFN;

    /*
     * Reset the saved ciphering config data structure
     */
    memset(&saved_new_ul_cipher_info[i].cipher_info, 0x0, sizeof(mac_cipher_config_type));

    saved_new_ul_cipher_info[i].cipher_info_saved = FALSE;
  }
  else
  {
    // Clear the memory
    memset(new_ul_cipher_info,0x00, sizeof(mac_cipher_config_type));
  }

  if(cfn < cur_ul_cipher_info[i].ciph_act_time)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Missed act_time, roll-over occured");
    cur_ul_cipher_info[i].hfn++;
  }
}

/*==============================================================================================
 FUNCTION NAME  void mac_update_ciphering_params

 DESCRIPTION : 1. MAC maintains 2 configurations for ciphering. Current Ciphering Parameters (cur_ul_cipher_info)
 and New Ciphering Parameters (new_ul_cipher_info).

 2. If there is an activation time pending, then new_ul_cipher_info is used. The parameters
 are copied into cur_ul_cipher_info at activation time.

 3. cfn_hfn_table is a map of the CFN and the corresponding HFN. CFN serves as the
 index of this array and the value at an index is the HFN. DL MAC reads from this
 table to get the HFN associated with a CFN. UL MAC maintains this table.

 4. When MAC starts ciphering for the first time and an activation time is pending,
   the sequence to increment the HFN is Reach Activation Time, ROLL OVER.
   To illustrate lets consider 2 cases

   Case 1: ciphering_activation_time = 12 and CFN = 30. Now the CFN has to roll over and then reach the
   activation time of 12. HFN would be incremented only after CFN rolls over after the activation time.

   Case 2: ciphering_activation_time = 220 and CFN = 5. Ciphering Activation time is reached before the
   CFN roll over and hence not treated as a special case.

   The flag first_time_rolled_over serves to address the case 1 above for us not to increment HFN
   if there is a CFN roll over and ciphering_activation_time is not reached.

 5. Subsequently every time there is a roll over MAC would increment the HFN.
 roll over is detected by the following piece of code :
 if ( cfn < prev_cfn). MAC maintains the last CFN value. This code is generic and would
 be able to handle even couple of build frame indications get missed for some reason.

 6. In HO_STEP1_CIPHER_CFG : the ciphering_activation_pending flag would be set to FALSE.
 The HFN would not be incremented even if we reach activation time and there are roll overs.
 This is a temporary state when WCDMA has received a request for Handover from GSM and WCDMA
 side has not yet sent the confirm. The same is applicable for hard handovers.

 7. In HO_STEP2_CIPHER_CFG : This configuration is received when WCDMA has sent a confirm.
 On receiving this configuration, MAC would increment the HFN by 1 when it hits activation
 time without waiting for the roll over. After this, the behaviour is as in the case of
 NON_HO_CIPHER_CFG.

===============================================================================================*/

void mac_update_ciphering_params
(
  uint8 cfn
)
{
  uint16 i;

  #ifdef FEATURE_WCDMA_SIMULATE_AMR_PACKETS
  rlc_ind_cfn_upd();
  #endif /*FEATURE_WCDMA_SIMULATE_AMR_PACKETS*/

  L2_ACQ_CIPHER_PARAM_LOCK();
  #ifdef FEATURE_WCDMA_HSUPA
  #ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
  mac_hs_ul_cur_cfn = cfn+1;
  #endif
  #endif

  cfn += 2;

  for (i=0;i<MAX_CN_DOMAIN;i++)
  {
    /* If there is a new config specified to which MAC has to switch to at
    ciphering activation time then ... */
    if (ul_cipher_activation_pending[i])
    {
      if(INVALID_PREV_CFN == prev_cfn )
      {
        prev_cfn = cfn;
      }

      if(CHECK_FOR_ACT_CFN_EXPIRY(prev_cfn, new_ul_cipher_info[i].ciph_act_time, cfn))
      {
        mac_handle_activation_cfn_expiry(cfn, (uint8)i);
      }
      else
      {
        /* If there is another security mode command which is causing the UE to switch to
        new set of parameters at activation time, then at the time of rollover, increment
        the hfn without actuvally having to wait for the activation time to do so. */
        if (mac_increment_hfn_on_rollover[i])
        {
          /* If the rollover happens */
          if (cfn < prev_cfn)
          {
            /* Then increment the current hfn */
            cur_ul_cipher_info[i].hfn++;
            /* Set this flag immediately to false */
            mac_increment_hfn_on_rollover[i] = FALSE;
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "CallWait cfn 0x%x prev_cfn 0x%x act_time 0x%x",
                                                       cfn,
                                                       prev_cfn,
                     new_ul_cipher_info[i].ciph_act_time);
          }
        }
      }

      // Compute the count c value.
      count_c[i] = ((cur_ul_cipher_info[i].hfn << 8) | cfn);

      /* ciphering_grabled boolean can be set to TRUE at run-time
      inorder to debug garbled voice etc */
      if ((!(cfn & mac_ul_debug_print_interval) && ciphering_garbled ))
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prior ciph_act 0x%x count_c 0x%x cfn 0x%x",
                                                   new_ul_cipher_info[i].ciph_act_time,
                                                   count_c[i],
                                                   cfn);

        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prior ciph_key 0x%x",
                                                   cur_ul_cipher_info[i].key_idx);
      }

      /* Save the current value of the cfn */
      prev_cfn = cfn;

    } /* activation time is pending */

    /* Activation time is not pending and if the ciphering is enabled */
    else if (cur_ul_cipher_info[i].cipher_enable)
    {
      if(INVALID_PREV_CFN == prev_cfn )
      {
        prev_cfn = cfn;
      }

      if (cur_ul_cipher_info[i].cipher_cfg_type == NON_HO_CIPHER_CFG)
      {
        // Compute the new hfn when there is a roll over
        // activation time has reached
        if (cfn < prev_cfn)
        {
          cur_ul_cipher_info[i].hfn++;
        }
      }

      /* In the case of HO_STEP1_CIPHER_CFG we do not need to
      increment the hfn, keep using the cfn even if activation time is
      reached. The following operation would take care of that so
      no special processing needs to be done for HO_STEP1_CIPHER_CFG*/

      count_c[i] = ((cur_ul_cipher_info[i].hfn << 8) | cfn);

      /* ciphering_grabled is a boolean. This boolean can be set to TRUE at run-time
      inorder to debug garbled voice etc
      if (ciphering_garbled && (!(cfn & mac_ul_debug_print_interval)))
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "After ciph_act count_c 0x%x cfn 0x%x, key 0x%x",
                                                   count_c[i],
                                                   cfn,
        cur_ul_cipher_info[i].key_idx);
      }*/
      if(cipher_log_idx < MAC_UL_MAX_CIPHER_LOG_CNT)
      {
        cipher_log[cipher_log_idx].countc  = mac_copy_to_word((uint8 *)(&count_c[i]));
        cipher_log[cipher_log_idx].cfn     = cfn;
        cipher_log[cipher_log_idx].key_idx = cur_ul_cipher_info[i].key_idx;
        cipher_log_idx++;
        if(cipher_log_idx == MAC_UL_MAX_CIPHER_LOG_CNT)
        {
          if(log_status(WCDMA_MAC_UL_CIPHER_LOG_PACKET)) 
          {  
            mac_ul_send_cfn_log_packet(cipher_log_idx);
          }
          else
          {
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "MAC UL Cipher Packet not Enabled");
          }
          cipher_log_idx = 0;
        }
      }
      else
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: Cipher Log index exceeded max value: %d",
                                                    cipher_log_idx);
        cipher_log_idx = 0;
      }

      /* Save the current value of the cfn */
      prev_cfn = cfn;
    }

    if (i == 0)
    {
      if (cur_ul_cipher_info[i].cipher_cfg_type != SMC_INITIATED_NON_HO_CIPHER_CFG)
      {
        // Store the hfn corresponding to the CFN. This owuld be used by DL MAC
        cfn_hfn_table[i][cfn] = cur_ul_cipher_info[i].hfn;

        /* This table will be poipulated by Uplink and will be used as a lookup table in the downlink.
        DL will refer to this information to populate thier ciphering keys, count_c etc */
        cfn_ciphering_table[cfn].key_idx = cur_ul_cipher_info[i].key_idx;
        cfn_ciphering_table[cfn].cipher_algo = cur_ul_cipher_info[i].cipher_algo;
        cfn_ciphering_table[cfn].cipher_enable = cur_ul_cipher_info[i].cipher_enable;
      }
      else
      {
        cfn_ciphering_table[cfn].cipher_enable = FALSE;
      }
    }
  }
  L2_ACQ_CIPHER_PARAM_UNLOCK();
}

/*==============================================================================================
 FUNCTION NAME  void l1_suspend_ind

 DESCRIPTION : This function is called 2 times by layer 1. Once at the time when Layer 1 is
 suspended and there is a W->G handover. This function is called again when Layer 1 is
 resumed (G->W this may happen if the reconfiguration fails or...).

 cfn : indicates the current value of the CFN
 num_roll_overs : The number of roll-overs that occureed during the time L1 was suspended
 to the time the Layer 1 resumed .

 If the reconfiguration took
   a) < 2.5 s ==> the variable num_roll_overs = 0
   b) 2.5s <= reconfig_time < 5.12s ===> num_roll_overs = 1
      ....
MAC based on the number of roll overs that ocurred would increment
its hfn count.

===============================================================================================*/
void l1_suspend_ind
(
  uint8                     cfn,
  l1_hho_action_enum_type      hho_action
)
{
  uint8   i=0;
  uint16  j = 0;

  // Adjust the cfn to align with the build frame ind cfn
  cfn += 2;

  L2_ACQ_CIPHER_PARAM_LOCK();
  if (!ul_cipher_activation_pending[0] && (cfn < prev_cfn))
  {
    cur_ul_cipher_info[0].hfn++;
  }

  prev_cfn = cfn;

  if (hho_action == L1_HHO_REV_TI)
  {
    /* No need to save the configuration . On going back to the old cell
    the ciphering parameters will be restored */
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Recvd L1_HHO_REV_TI from L1, No-Op");
  }
  else
  {
    /* for hho_action == HHO_FW_TI, HHO_FW_TM, HHO_REV_TM  the config needs
    to be saved */
    WCDMA_MEMCPY(prev_cfn_ciphering_table, 
                 256 * sizeof(mac_ciphering_table),
                 cfn_ciphering_table, 
                 256 * sizeof(mac_ciphering_table));

    for (i = 0; i < MAX_CN_DOMAIN; i++)
    {
      /* Save the configuration currently being used in prev. In case of reverting
      back to old configuration this will be used */
      prev_mac_increment_hfn_on_rollover[i] = mac_increment_hfn_on_rollover[i];
      prev_new_ul_cipher_info[i] = new_ul_cipher_info[i];
      prev_cur_ul_cipher_info[i] = cur_ul_cipher_info[i];
      prev_ul_cipher_activation_pending[i] = ul_cipher_activation_pending[i];
      old_prev_cfn = prev_cfn;
      prev_count_c[i] = count_c[i];

      /* Initialize the cfn hfn table */
      for (j = 0; j < 256; j++)
      {
        prev_cfn_hfn_table[i][j] = cfn_hfn_table[i][j];
      }

      prev_get_start_value_to_transmit[i] = get_start_value_to_transmit[i];

      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Saving the old config cfn 0x%x, count_c 0x%x, hfn 0x%x",
                                                 prev_cfn,
                                                 count_c[0],
                                                 prev_cur_ul_cipher_info[i].hfn);
    }
  }

  L2_ACQ_CIPHER_PARAM_UNLOCK();
  MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Suspend cfn 0x%x,prev_cfn 0x%x",
                                              cfn, prev_cfn);
}

/*==============================================================================================
 FUNCTION NAME  void l1_resume_ind

 DESCRIPTION : This function is called 2 times by layer 1. Once at the time when Layer 1 is
 suspended and there is a W->G handover. This function is called again when Layer 1 is
 resumed (G->W this may happen if the reconfiguration fails or...).

 cfn : indicates the current value of the CFN
 num_roll_overs : The number of roll-overs that occureed during the time L1 was suspended
 to the time the Layer 1 resumed .

 If the reconfiguration took
   a) < 2.5 s ==> the variable num_roll_overs = 0
   b) 2.5s <= reconfig_time < 5.12s ===> num_roll_overs = 1
      ....
MAC based on the number of roll overs that ocurred would increment
its hfn count.

===============================================================================================*/
void l1_resume_ind
(
  uint8                     cfn,
  uint8                     num_roll_overs,
  l1_hho_action_enum_type   hho_action
)
{
  uint8   i=0;
  uint16  j = 0;
  uint16  num_cfns_to_update=0;
  uint8   t1,t2,t3;
  uint32  hfn;
  boolean ciph_act_cfn_reached_on_resume = FALSE;

  // Adjust the cfn to align with the build frame indication cfn.
  cfn += 2;

  if (hho_action == L1_HHO_FW_TI)
  {
    /* No need to restore the ciphering configuration. In all other cases
    restore the ciphering configuration */
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Recvd L1_HHO_FW_TI from L1, No-Op");
  }
  else /* hho_action == HHO_REV_TM, HHO_FW_TM, HHO_FW_TI */
  {

    L2_ACQ_CIPHER_PARAM_LOCK();
    /* Restore  the saved configuration we have reverted to old configuration
    and this needs to be used again */
    WCDMA_MEMCPY(cfn_ciphering_table,
                 256 * sizeof(mac_ciphering_table), 
                 prev_cfn_ciphering_table, 
                 256 * sizeof(mac_ciphering_table));

    for (i = 0; i < MAX_CN_DOMAIN; i++)
    {
      /* Save the configuration currently being used in prev. In case of reverting
      back to old configuration this will be used */
      mac_increment_hfn_on_rollover[i] = prev_mac_increment_hfn_on_rollover[i];

      cur_ul_cipher_info[i] = prev_cur_ul_cipher_info[i];
      new_ul_cipher_info[i] = prev_new_ul_cipher_info[i];

      ul_cipher_activation_pending[i] = prev_ul_cipher_activation_pending[i];
      prev_cfn = old_prev_cfn;
      count_c[i] = prev_count_c[i];

      /* Initialize the cfn hfn table */
      for (j = 0; j < 256; j++)
      {
        cfn_hfn_table[i][j] = prev_cfn_hfn_table[i][j];
      }

      get_start_value_to_transmit[i] = prev_get_start_value_to_transmit[i];
    }
    L2_ACQ_CIPHER_PARAM_UNLOCK();

    MSG_HIGH("Restoring the old config prev_cfn 0x%x, count_c 0x%x"
             ,prev_cfn,count_c[0],0);
  }

  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Resume cfn 0x%x, roll_over %d, prev_cfn 0x%x",
                                              cfn, num_roll_overs, prev_cfn);

  /* If the time between suspend and resume takes more than 2.5 seconds
  and if there was a pending activation time then we would have reached
  the activation time sometime on between suspend and resume. Hence we
  need to get the new value for the hfn using get_start_value_to_transmit
  and update the hfn accordingly */
  for (i=0;i<MAX_CN_DOMAIN;i++)
  {
    /* Do the following only if ciphering is enabled and wcdma is not in
    suspended state */
    if (cur_ul_cipher_info[i].cipher_enable)
    {
      L2_ACQ_CIPHER_PARAM_LOCK();
      /* If there was a pending activation time during the suspend - resume time frame */
      if (ul_cipher_activation_pending[i])
      {
        if (num_roll_overs >= 1)
        {
          // This indicates that activation time was reached during the time that
          // l1 was suspended.
          // Compute the new hfn when there is a roll over
          // activation time has reached
          ciph_act_cfn_reached_on_resume = TRUE;

          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "ActCFN-reached sus 0x%x res 0x%x roll_ovrs 0x%x",
                                                     prev_cfn,
                                                     cfn,
                                                     num_roll_overs);
        }
        else
        {
          if((t1=(cfn-1)) != prev_cfn)
          {
            if ((t2=(cfn-2)) != prev_cfn)
            {
              if ((t3=(cfn-3)) != prev_cfn)
              {
                num_cfns_to_update = 3;
                cfn = t3;
              }
              else
              {
                num_cfns_to_update = 2;
                cfn = t2;
              }
            }
            else
            {
              num_cfns_to_update = 1;
              cfn = t1;
            }
          }

          /* The G2W took less than 2.5 seconds */
          if (CHECK_FOR_ACT_CFN_EXPIRY(prev_cfn, new_ul_cipher_info[i].ciph_act_time, cfn))
          {
            ciph_act_cfn_reached_on_resume = TRUE;
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "ActCFN-reached sus 0x%x res 0x%x act_time 0x%x",
                                                       prev_cfn,
                                                       cfn,
                                                       new_ul_cipher_info[i].ciph_act_time);
          }
          else
          {
            /* If there is another security mode command which is causing the UE to switch to
            new set of parameters at activation time, then at the time of rollover, increment the hfn
            without actuvally having to wait for the activation time to do so. */
            if (mac_increment_hfn_on_rollover[i])
            {
              /* If the rollover happens */
              if (cfn < prev_cfn)
              {
                /* Then increment the current hfn */
                cur_ul_cipher_info[i].hfn++;
                /* Set this flag immediately to false */
                mac_increment_hfn_on_rollover[i] = FALSE;
                MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "CallWait cfn 0x%x prev_cfn 0x%x act_time 0x%x",
                                                           cfn,
                                                           prev_cfn,
                                                           new_ul_cipher_info[i].ciph_act_time);
              }
            }
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "ActCFN not reached sus 0x%x, res 0x%x, act_time 0x%x",
                                                       prev_cfn,
                                                       cfn,
                                                       new_ul_cipher_info[i].ciph_act_time);
          }
        } // Failed G2W took less than 2.5 seconds

        if (ciph_act_cfn_reached_on_resume)
        {
          mac_handle_activation_cfn_expiry(cfn, i);
        } /* If ciphering activation time was reached during the time L1 was suspended */
      } // If there is no pending activation time
      else
      {
        if ((t1=(cfn-1)) != prev_cfn)
        {
          if ((t2=(cfn-2)) != prev_cfn)
          {
            if ((t3=(cfn-3)) != prev_cfn)
            {
              num_cfns_to_update = 3;
              cfn = t3;
            }
            else
            {
              num_cfns_to_update = 2;
              cfn = t2;
            }
          }
          else
          {
            num_cfns_to_update = 1;
            cfn = t1;
          }
        }

        // Compute the new hfn when there is a roll over
        // activation time has reached
        if (cfn < prev_cfn)
        {
          cur_ul_cipher_info[i].hfn++;
        }
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Non-Pending-Act Sus 0x%x res 0x%x nroll %d",
                                                   prev_cfn,
                                                   cfn,
                                                   num_roll_overs);
      }

      /* Add the number of roll overs that has occurred during the time
      when WCDMA was in suspended state */
      hfn = cur_ul_cipher_info[i].hfn + (uint32)num_roll_overs;

      cfn_hfn_table[i][cfn] = hfn;

      cur_ul_cipher_info[i].hfn += (uint32)num_roll_overs;

      prev_cfn = cfn;

      L2_ACQ_CIPHER_PARAM_UNLOCK();

      for (j=0;j<num_cfns_to_update;j++)
      {
        cfn = cfn+1;
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "cfn 0x%x populated %d",
                                                   cfn, j);
        // Note that the mac_update_ciphering_params function will
        // also adjust the cfn according to the build frame indication
        // Hence before calling the follow. function the cfn needs to be adjusted
        mac_update_ciphering_params(cfn-2);
      }
    }
  } // Go through each domain
}


/*==================================================================================================
 FUNCTION NAME  l1_phy_ul_build_frame_ind

 DESCRIPTION  This function processes UL frame build indication received from PHY layer
      For feature #ifdef FEATURE_UMTS_CIPHERING_ENABLE


 CONTEXT    Called by PHY layer in interrupt context
==================================================================================================*/
boolean   l1_phy_ul_build_frame_ind
(
  /* current tti */
  l1_tti_enum_type     tti,

  /* current CFN */
  uint8               cfn,

  boolean             ul_tx_disabled,

  /* omit SRB data for specified delay frames */
  boolean              SRB_omit,

  #ifdef FEATURE_WCDMA_PWR_TFC_ELMN
  /*  Array of TFCI states for power based TFC elimination
   *  TRUE indicates the TFCI is in excess_pwer_state
   *  FALSE indicates the TFCI is in supported_state
  */
  const boolean *tfc_pwr_state_ptr,
  #endif

  #ifdef FEATURE_TFCS_CM_BY_HLS
  /* Call back function pointer provided by L1. This function
  is called by MAC when it needs to get an updated information
  about the TFCI's that are restricted*/
  L1_UL_QUERY_TFCS_RESTRICT_CB_FUNC_TYPE *l1_cb_func_ptr,

  #endif

  /* pointer to Tx data buffer */
  l1_ul_tx_data_type  *tx_buf_ptr
)

{
  uint8 i;

  /* return valuse */
  boolean   rv = FALSE;

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
  uint16  ntfc;
#endif


  uint16    tfc_idx;

#ifdef FEATURE_WCDMA_MULTIRATE_AMR
  mac_amr_mode_info_type      amr_mode_info;
#endif /* FEATURE_WCDMA_MULTIRATE_AMR */

  rlc_profile_dump_start(&phy_build_frame_profile_dump);

  REX_ISR_LOCK(&r99_ul_isr_l2_datapath_mutex);

  if (GET_UL_CCTRCH_TYPE == INVALID_UL_CCTRCH)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "No Trch configured at MAC!");
    REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
    rlc_profile_dump_end(&phy_build_frame_profile_dump);
    return rv;
  }

  if (mac_ul_tfcc_config_in_process)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TFCC config in process, so skip BFI");
    REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
    rlc_profile_dump_end(&phy_build_frame_profile_dump);
    return rv;
  }

  if (mac_ul_tfcc_config_pending)
  {
    if (mac_ul_tfcc_activation_cfn == cfn)
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Activation time reached cfn %d, act_cfn %d",
                                                 cfn,
                                                 mac_ul_tfcc_activation_cfn);
      mac_ul_tfcc_config_pending = FALSE;
      for (tfc_idx = 0;tfc_idx < mac_ul_tfcs_table.ntfc; tfc_idx++)
      {
        mac_ul_tfcs_table.tfc_info[tfc_idx].control_flag =
          mac_ul_pending_tfcs_control_table[tfc_idx];

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
        // If this TFCI has not moved to blocked state then initialize
        if (int_ul_tfcs_info[tfc_idx].power_state != BLOCKED_STATE)
#endif
        {
          int_ul_tfcs_info[tfc_idx].control_flag =
            mac_ul_pending_tfcs_control_table[tfc_idx];
        }

        mac_ul_tfcc_config_pending = FALSE;

        /* Clear uplink TFCS information table internally used by MAC */
        WCDMA_MEMCPY(mac_ul_current_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type),
                     mac_ul_pending_tfcs_control_table,
                     UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        if (mac_ul_tfcs_pending_control_duration == 0)
        {
          /* Clear uplink TFCS information table internally used by MAC */
          WCDMA_MEMCPY(mac_ul_current_tfcs_control_table,
                       UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type),
                       mac_ul_pending_tfcs_control_table,
                       UE_MAX_TFC * sizeof(mac_ul_tfc_control_e_type));

        }
        else
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Started timer for duration %d for this TFCC config",
                                                     mac_ul_tfcs_pending_control_duration);
          // Clear any old timers if it is running
          (void)rex_clr_timer(&ul_tfc_ctrl_tmr_blk);
          (void)rex_set_timer(&ul_tfc_ctrl_tmr_blk, (uint32)(mac_ul_tfcs_pending_control_duration));
        }
      }
#ifdef FEATURE_WCDMA_MULTIRATE_AMR
     if((rab_status == CS_RABS) ||
          (rab_status == CS_PLUS_PS_RABS))
     {
       mac_ul_compute_amr_rate(&amr_mode_info);
       mac_ul_set_amr_rate(amr_mode_info.amr_rate);
     }
#endif
       mac_compute_minimum_set_of_tfcs(
                                       #ifdef FEATURE_WCDMA_MULTIRATE_AMR
                                       &amr_mode_info
                                       #endif
                                       );
    }
  }

  if (ul_tx_disabled)
  {
    REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
    rlc_profile_dump_end(&phy_build_frame_profile_dump);
    return rv;
  }

  /* Transmit Data Buffer pointer is NULL */
  if (tx_buf_ptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_MED , "Tx Data Buffer pointer is NULL");

    /*-------------------------------------------------------------------
    During the time when L1 has switched to the new config and is
    waiting for MAC to switch to the new configuration. MAC would be
    getting NULL tx_bufptr from L1.
    --------------------------------------------------------------------*/
    if (ul_configuration_pending)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Reconfiguring MAC");
    }

  }
  /* Invalid TTI type */
  else if (tti >= L1_NUM_TTI)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "invalid TTI type %d", tti);
  }
  /* Valid UL MAC Command Buffer Pointer */
  else
  {
    if (!tx_buf_ptr->mac_busy)
    {
      if (mac_build_pending)
      {
        /* MAC has pending build frame */
        if (bld_frame_ind_info.tti <= tti)
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Now at right TTI %d",
                                                     bld_frame_ind_info.tti);
          mac_build_pending = FALSE;
        }
        else
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Waiting for right TTI %d now %d",
                                                     bld_frame_ind_info.tti,
                                                     tti);
          REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
          rlc_profile_dump_end(&phy_build_frame_profile_dump);
          return(rv);
        }
      }
      else
      {
        tx_buf_ptr->mac_busy = TRUE;
      }
    }
    else
    {
      mac_build_pending = TRUE;
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC busy with prev request");
      REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
      rlc_profile_dump_end(&phy_build_frame_profile_dump);
      return(rv);
    }

    /* Save the Build Frame Indication */
    bld_frame_ind_info.cfn = cfn;
    bld_frame_ind_info.tti = tti;

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
    if (oc_ul_tfcs_ptr!= NULL)
    {
      /* Clear off the old values*/
      memset(tfc_excess_pwr_state, 0x0, oc_ul_tfcs_ptr->ntfc*sizeof(boolean));
  
      /* Only for DCH apply the power restriction */
      if (GET_UL_CCTRCH_TYPE == DCH)
      {
        for (ntfc = 0; ntfc < oc_ul_tfcs_ptr->ntfc; ntfc++)
        {
          tfc_excess_pwr_state[ntfc] = tfc_pwr_state_ptr[ntfc];
        }
      }
    }

#endif

#ifdef FEATURE_TC_CONSTANT_DELAY
    /* Save the cfn */
    cur_ul_cfn = cfn;
#endif

#ifdef FEATURE_TFCS_CM_BY_HLS
    /* Store the call back function pointer provided by L1 */
    ptr_to_l1_fn = l1_cb_func_ptr;

#endif

    /* Save the current tti */
    current_tti = tti;

    bld_frame_ind_info.srb_omit = SRB_omit;
    bld_frame_ind_info.tx_buf_ptr = tx_buf_ptr;

    /* Initialize TX Buffer Data */
    tx_buf_ptr->num_trch = 0;
    for (i=0; i <L1_UL_MAX_TRCH; i++)
    {
      tx_buf_ptr->tx_data[i].num_tb = 0;
      tx_buf_ptr->tx_data[i].tb_size = 0;
    }

    if (oc_ul_tfcs_ptr == NULL)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TFCS ptr NULL");
      REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
      rlc_profile_dump_end(&phy_build_frame_profile_dump);
      return rv;
    }
    else if ((oc_ul_tfcs_ptr->ntfc > UE_MAX_TFC) ||
             (oc_ul_tfcs_ptr->nchan > UE_MAX_TRCH))
    {
      ERR_FATAL("Configured values exceed UE_MAX %d %d",
                oc_ul_tfcs_ptr->ntfc,oc_ul_tfcs_ptr->nchan,0);

    }

    /* Process Build UL Frame Indication */
    proc_build_ul_frame_ind();

    rv = TRUE;
  }
  REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);
  rlc_profile_dump_end(&phy_build_frame_profile_dump);
  return(rv);
}

/*===============================================================================================
 FUNCTION NAME  l1_phy_ul_trch_status_ind

 DESCRIPTION  This function processes transmit status indication received from PHY layer

 CONTEXT    Called by PHY layer in interrupt context
===============================================================================================*/
void  l1_phy_ul_trch_status_ind
(
/* PRACH transmit status */
l1_prach_status_enum_type status
)
{

  REX_ISR_LOCK(&r99_ul_isr_l2_datapath_mutex);
  
  /* Save RACH transmit status - using double buffer mechanism*/
  if (rach_tx_status[0] == L1_PRACH_MAC_INVALID)
  {
    rach_tx_status[0] = status;
  }
  else if(rach_tx_status[1] == L1_PRACH_MAC_INVALID)
  {
    rach_tx_status[1] = status;
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "recievd 2nd status status_1 %d, status_2 %d",
                                               rach_tx_status[0],
                                               rach_tx_status[1]);
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Double buffer is full, staus recieved %d is discarded",
                                                status);
  }
  
  REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);

  /* Set signal for the DL L2 Task */
  (void)rex_set_sigs( wcdma_l2_ul_get_tcb(), MAC_UL_TX_STATUS_IND_SIG );
}

#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION

/*-------------------------------------------------------------------
FUNCTION mac_ul_allocate_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC at init time so that MAC can go for 
 dynamic memory allocation for the required varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/
void  mac_ul_allocate_mem_ZI_opt(void)
{

  uint16    num_rows = 0;

  /* Dynamical allocation for the int_ul_tfcs_info */
  if (int_ul_tfcs_info == NULL)
  {
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Allocating %d bytes for int_ul_tfcs_info, client ID %d",
              sizeof(ul_tfcs_info_type) * UE_MAX_TFC,
                                               MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    int_ul_tfcs_info = (ul_tfcs_info_type *)modem_mem_alloc
                         (sizeof(ul_tfcs_info_type) * UE_MAX_TFC, 
                           MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    if (NULL == int_ul_tfcs_info)
    {
      ERR_FATAL("MAC_ERR: failed to allocate %d bytes for int_ul_tfcs_info, client %d ",
                 sizeof(ul_tfcs_info_type) * UE_MAX_TFC, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT, 0);
    }
    memset(int_ul_tfcs_info, 0, (sizeof(ul_tfcs_info_type) * UE_MAX_TFC));
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "int_ul_tfcs_info dynamic allocation already done " );
  }


  /* Dynamical allocation for the rlc_ul_data_info_ptr */
  if(rlc_ul_data_info_ptr == NULL)
  {
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Allocating %d bytes for rlc_ul_data_info, client ID %d",
              sizeof(rlc_ul_frame_data_type),
                                               MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    rlc_ul_data_info_ptr = (rlc_ul_frame_data_type *)modem_mem_alloc
                         (sizeof(rlc_ul_frame_data_type), 
                           MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    if (NULL == rlc_ul_data_info_ptr)
    {
      ERR_FATAL("MAC_ERR: failed to allocate %d bytes for rlc_ul_data_info, client %d",
                 sizeof(rlc_ul_frame_data_type), MODEM_MEM_CLIENT_WCDMA_RLC_CRIT, 0);
    }
    memset(rlc_ul_data_info_ptr, 0, sizeof(rlc_ul_frame_data_type));
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "rlc_ul_data_info_ptr dynamic allocation already done " );
  }


  /* Dynamical allocation for the int_ul_trch_priority_info */
  if (int_ul_trch_priority_info == NULL)
  {
    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Allocating %d bytes for int_ul_trch_priority_info %d pointers, client ID %d",
                                               sizeof(maci_ul_trch_priority_info_type*)*UE_MAX_TRCH,
                                               UE_MAX_TRCH,
              MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    int_ul_trch_priority_info = (maci_ul_trch_priority_info_type **)modem_mem_alloc
                         (sizeof(maci_ul_trch_priority_info_type *) * UE_MAX_TRCH, 
                           MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    
    if (NULL == int_ul_trch_priority_info)
    {
      ERR_FATAL("MAC_ERR: failed to allocate %d bytes for %d pointers, client %d",
                 sizeof(maci_ul_trch_priority_info_type *) * UE_MAX_TRCH, 
                MODEM_MEM_CLIENT_WCDMA_RLC_CRIT, 0);
    }
    
    /* Allocate memory for each Transport Channle */
    for (num_rows = 0; num_rows < UE_MAX_TRCH; num_rows++)
    {
      int_ul_trch_priority_info[num_rows] = (maci_ul_trch_priority_info_type *)modem_mem_alloc
                         (sizeof(maci_ul_trch_priority_info_type) * MAC_UL_MAX_MLPS, 
                           MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
      
      if (NULL == int_ul_trch_priority_info[num_rows])
      {
        ERR_FATAL("MAC_ERR: failed to allocate %d bytes for int_ul_trch_priority_info, client %d",
                   sizeof(maci_ul_trch_priority_info_type) * MAC_UL_MAX_MLPS,
                   MAC_UL_MAX_MLPS, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
      }
    }
  
    /* MemSet to zero for each Transport Channle */
    for (num_rows = 0; num_rows < UE_MAX_TRCH; num_rows++)
    {
      memset(int_ul_trch_priority_info[num_rows], 0, 
             (sizeof(maci_ul_trch_priority_info_type) * MAC_UL_MAX_MLPS));
    }
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "int_ul_trch_priority_info dynamic allocation already done " );
  }
  


}


/*-------------------------------------------------------------------
FUNCTION mac_ul_free_mem_ZI_opt

DESCRIPTION
 This function will be called by RRC release WCDMA time, so that MAC can de-allocate/free
 the dynamic memory allocated varaibles.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
--------------------------------------------------------------------*/
void  mac_ul_free_mem_ZI_opt(void)
{

  uint16    num_rows = 0;

  MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Freeing MAC UL ZI variables, client ID %d",
                                             MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);

  if (int_ul_tfcs_info != NULL)
  {
    modem_mem_free(int_ul_tfcs_info, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    int_ul_tfcs_info = NULL;
  }

  if (rlc_ul_data_info_ptr != NULL)
  {
    modem_mem_free(rlc_ul_data_info_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    rlc_ul_data_info_ptr = NULL;
  }

  if (int_ul_trch_priority_info != NULL)
  {
    for( num_rows = 0; num_rows < UE_MAX_TRCH; num_rows++)
    {
      modem_mem_free(int_ul_trch_priority_info[num_rows], MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    }
    modem_mem_free(int_ul_trch_priority_info, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
    int_ul_trch_priority_info = NULL;
  }
  
}

#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */


/*==============================================================================================
 FUNCTION NAME  proc_ul_mac_config_signal

 FUNCTION   This function processes the config signal received from the physical layer.
===============================================================================================*/
void proc_ul_mac_config_signal ( void )
{
#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
  const boolean *tfc_pwr_state_ptr = NULL;
#endif

  if (ul_configuration_pending)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Recvd L1 sig, reconfig MAC now");

    /* Configure MAC only after this signal is recvd from L1 */
    config_ul_mac(&ul_cfginfo);

#ifdef FEATURE_WCDMA_PWR_TFC_ELMN
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
    MAC_WL1_TFC_EXCESS_PWR_INTLOCK();
#endif
    tfc_pwr_state_ptr = l1_ul_get_tfc_excess_pwr_states();
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
    MAC_WL1_TFC_EXCESS_PWR_INTFREE();
#endif

    if ((tfc_pwr_state_ptr != NULL) && (GET_UL_CCTRCH_TYPE != INVALID_UL_CCTRCH) &&
        (oc_ul_tfcs_ptr != NULL))
    {
      mac_initialize_pwr_state_for_tfcs(tfc_pwr_state_ptr);
    }
    else
    {
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "R99 TFC power config skipped; l1_ptr:0x%X cctrch:%d tfcs_ptr:0x%X",
                                                  tfc_pwr_state_ptr,
                                                  GET_UL_CCTRCH_TYPE,
                                                  oc_ul_tfcs_ptr);
    }
#endif /* FEATURE_WCDMA_PWR_TFC_ELMN */

    /* Post a sucess message to L1 */
    maci_post_l1_config_ind(CPHY_MAC_UL_CONFIG_CNF);

    /* Clear the ul_configuration_pending flag */
    ul_configuration_pending=FALSE;
  }
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Recvd L1 sig, NO pending configurations");
  }
}

/*-------------------------------------------------------------------
FUNCTION mac_rrc_get_cipher_pending_status

DESCRIPTION
 This function will be called by RRC in the case when it receives
 a SMC message

DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC has not reached the ciphering
  activation time for the current config under use. FALSE indicates
  that there is no pending configuration in MAC and ciphering is
  ongoing

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_rrc_get_cipher_pending_status(void)
{
  boolean ul_cipher_pending_stat;

  L2_ACQ_CIPHER_PARAM_LOCK();
  ul_cipher_pending_stat = ul_cipher_activation_pending[0];
  L2_ACQ_CIPHER_PARAM_UNLOCK();

  /* Return the ciphering pending status for the CS domain to RRC */
  return (ul_cipher_pending_stat);
}

#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF

/*-------------------------------------------------------------------
FUNCTION uint32 mac_get_cnt_c_val()

DESCRIPTION
This function will be called by L1 to get the count_c value

DEPENDENCIES
  None

RETURN VALUE
  The count_c value corresponding the current cfn will be returned to L1
  for CS domain only. Ther is no count_c maintained by MAC for the PS
  domain

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
uint32 mac_get_cnt_c_val(uint8 cfn)
{
  /* Return the ciphering pending status for the CS domain to RRC */
  return (cfn_hfn_table[0][cfn] );
}

#endif /* FEATURE_WCDMA_CNT_C_SFN_DIFF */

#ifdef FEATURE_WCDMA_HSUPA
/*-------------------------------------------------------------------
FUNCTION boolean MAC_CHECK_HSUPA_CONFIG_STATUS()

DESCRIPTION
This function will look to see if ul configuration is pending and
if yes then whether mac config has changed or not and whether
mac_reset_indicator is set to true.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicates that mac configuration has chnaged and mac_reset_indicator
  is set to TRUE

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
mac_ul_config_type* mac_check_hsupa_config_status(void)
{
  mac_ul_config_type* cfg_ptr=NULL;

  if (ul_configuration_pending)
  {
    if (ul_cfginfo.mac_e_reset_indicator)
    {
      cfg_ptr = &ul_cfginfo;
      return cfg_ptr;
    }
  }

  return cfg_ptr;
}

#endif //FEATURE_WCDMA_HSUPA

/*===============================================================================================
FUNCTION NAME mac_ul_get_trch_type

DESCRIPTION   This function returns the up-link transport channel type corresponding to given RLC & RB IDs
===============================================================================================*/

log_wcdma_trch_type mac_ul_get_trch_type(rlc_lc_id_type rlc_id, rb_id_type rb_id)
{
  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;

  if(GET_UL_CCTRCH_TYPE == RACH)
  {
    return_trch_val = mac_ul_check_rach_trch(rlc_id, rb_id);
    return return_trch_val;
  }
  else
  {
    return_trch_val = mac_ul_check_dch_trch(rlc_id, rb_id);
    if(return_trch_val != WCDMA_TRCH_INVALID)
    {
      return return_trch_val;
    }

    return_trch_val = mac_ul_check_edch_trch(rlc_id, rb_id);
    return return_trch_val;
  }
}

/*===============================================================================================
FUNCTION NAME l2_get_trch_type_for_given_rb_id

DESCRIPTION   
===============================================================================================*/

log_wcdma_trch_type  l2_get_trch_type_for_given_rb_id(rb_id_type rb_id, boolean direction, rlc_lc_id_type rlc_id)
{

  log_wcdma_trch_type return_trch_val = WCDMA_TRCH_INVALID;

  if(direction == 1)
  {
    return_trch_val = mac_ul_get_trch_type(rlc_id, rb_id);
  }
  else if(direction == 0)
  {
    return_trch_val = mac_dl_get_trch_type(rlc_id, rb_id);
  }
  else
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid direction (%d) in function l2_get_trch_type_for_given_rb_id ",
                                                direction);
  }
  return return_trch_val;
}
