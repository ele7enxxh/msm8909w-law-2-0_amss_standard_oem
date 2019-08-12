/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state.c_v   1.4   07 Jun 2002 14:23:22   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/27/12   us      After successful PS Detach, even if CM  sends a CM_SERVICE_REQ to REG, REG should forward this MM instead of 
                   looping back & sending back CM_SERVICE_CNF directly to CM. 
02/13/02   kwa     Initial release.
05/20/02   kwa     Removed strings from calls to MSG_HIGH.
06/05/02   kwa     Filled roaming_ind upon receiving a mmr_reg_cnf.
06/09/02   kwa     Added functions to get the service_status and active_rat
                   from the current service_state.
06/27/02   kwa     Modified function that sends an mmr_plmn_search_req due to
                   interface change.
07/01/02   kwa     Modified function that processes the mmr_reg_cnf due to
                   interface change.
08/06/02   kwa     CR 23776 - Fixed bug in reg_state_process_cm_network_list_req
                   that prevented REG from sending an mmr_plmn_search_req when
                   no service is available.
08/21/02   kwa     Stopped REG timers upon receiving a cm_stop_mode_req.
08/23/02   kwa     Initialized rat in reg_state_process_cm_network_list_req
                   to eliminate lint warning.
08/26/02   kwa     Added call to reg_sim_write_rplmn_rat in
                   reg_state_process_mmr_reg_cnf.
09/19/02   kwa     Added functionality to support cm_act_req.
09/23/02   kwa     Added code to properly handle registration failures due to
                   PLMN forbidden for GPRS services.
10/01/02   kwa     Updated code to properly handle invalid SIM states.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
10/29/02   kwa     Added support for SYS_STOP_MODE_REASON_DEEP_SLEEP in
                   reg_state_process_cm_stop_mode_req.
02/28/03   kwa     Added code to support ps_data_suspend.
03/27/03   kwa     Added function reg_state_process_mmr_service_ind.  Added
                   logic in this function to send a cm_service_cnf or
                   cm_service_ind as appropriate.
05/22/03   kwa     Modified functions to pass const pointers to eliminate
                   lint warnings.  Changed ERR_FATALSs in unsupported message
                   functions to ERRs.
06/04/03   kwa     Removed one remaining lint warning.
06/13/03   kwa     Added code in reg_state_process_mmr_service_ind to handle
                   MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION.  This change was
                   required to pass GPRS GCF test cases where the GPRS Attach
                   is rejected with cause #13 - Roaming not allowed in this
                   location area.
06/17/03   kwa     Added code in reg_state_print_service_state to display
                   the roaming state of the phone.  Modified code in
                   reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to properly set the
                   roaming indication under all scenarios.
07/30/03   kwa     Modified reg_state_process_mmr_service_ind to properly
                   handle MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED.
08/12/03   kwa     Added code to handle MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED.
08/24/03   kwa     Added code in reg_state_process_mmr_reg_cnf to handle
                   MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION.  This change was
                   required to handle field test cases where the IMSI Attach
                   is rejected with cause #13 - Roaming not allowed in this
                   location area.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/12/03   kwa     Removed code in reg_state_process_cm_network_list_req that
                   sent a cm_service_ind message to CM.  MMoC treats this as
                   an unexpected message and raises a warning message.
09/18/03   kwa     Added support for active_band and lac.  Added support for
                   selected_plmn_type in reg_state_process_cm_service_req.
09/19/03   kwa     Added code in reg_state_process_mmr_service_ind to filter
                   out redundant mmr_service_ind messages in limited service.
09/24/03   kwa     Modified code in reg_state_process_mmr_service_ind that
                   filters out redundant mmr_service_ind messages in limited
                   service to resolve GCF test case issues.
09/29/03   kwa     Added code to quickly reacquire normal service if the phone
                   acquired limited service during a routine search for service.
10/02/03   kwa     Removed code to quickly reacquire normal service if the phone
                   acquired limited service during a routine search for service.
                   This functionality is now performed by MM/GMM.
10/02/03   kwa     Added code to handle SYS_STOP_MODE_REASON_MODE_CHANGE_FTM.
10/03/03   kwa     Added code in reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to properly handle
                   MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED during manual PLMN
                   selection.
10/10/03   kwa     Fixed code in reg_state_process_mmr_reg_cnf to remove a PLMN
                   from the list of forbidden PLMNs when it is in PS_ONLY mode.
10/14/03   kwa     Added code in reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to insure that the
                   plmn_forbidden indicator is always set in cm_service_req
                   and cm_service_ind messages.
10/16/03   kwa     Reinserted code to quickly reacquire normal service if the
                   phone acquired limited service on a non-forbidden PLMN
                   during a routine search for service.  MM/GMM only handle the
                   case when the phone acquires service on the RPLMN.
10/17/03   kwa     Modified code that handles MMR_CAUSE_FAILURE_REMAIN_ON_PLMN.
                   SD/MMoC code was modify to properly handle this condition
                   using a new service status - SYS_SRV_STATUS_LIMITED_REGIONAL.
10/23/03   kwa     Modified reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to save the equivalent PLMN
                   list whenever MM sets update_equivalent_plmn_list to TRUE.
11/05/03   kwa     Modified code in reg_state_process_mmr_service_ind to only
                   set the HPLMN search period timer if the phone has acquired
                   service after having lost service.
11/07/03   kwa     Moved calls to reg_mode_save_equivalent_plmn_list so the
                   equivalent PLMN list is updated after PLMN selection has
                   finished using the previous equivalent PLMN list.
11/07/03   kwa     Removed support for REG_CM_MANUALLY_SELECTED_RPLMN.  The
                   capability for CM to request that REG search for the RPLMN
                   at power up in manual network selection mode is provided by
                   sending an undefined PLMN along with
                   REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN.
11/10/03   kwa     Modified the behavior of the state machine upon receiving a
                   cm_stop_mode_req with reason SYS_STOP_MODE_REASON_POWER_OFF
                   to enter reg_state_deregistering rather than
                   reg_state_inactive.  This change will prevent messages
                   from propagating down the stack while the phone is in the
                   process of powering down.
11/16/03   kwa     Modified reg_state_process_cm_stop_mode_req to clear the
                   available PLMN list since the available PLMN list is now
                   used to support manual PLMN selection.
11/17/03   kwa     Modified setting of SYS_SRV_STATUS_LIMITED_REGIONAL to
                   handle scenarios where the phone has successfully acquired
                   CS service but is rejected for PS service.
12/10/03   kwa     Modified handling of MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED
                   so the phone will attempt to acquire GSM_ONLY service on
                   the same PLMN.
01/13/04   kwa     Added code in reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to properly handle
                   MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION during manual PLMN
                   selection.
01/20/04   kwa     Added code in reg_state_process_mmr_service_ind to
                   properly handle a location updating reject with cause #15.
01/22/04   kwa     Added code to avoid initiating a full search for service
                   when the phone is already camped on the desired PLMN.
                   Added function reg_state_service_status_set.
02/04/04   kwa     Added support for PS follow on request.
02/09/04   kwa     Fixed a bug introduced when code was added to avoid
                   initiating a full search for service when the phone is
                   already camped on the desired PLMN.  In this scenario the
                   REG state was moving to REG_STATE_REGISTERING rather than
                   remaining in the previously active state.
02/10/04   kwa     Added code to set reg_state_cm_initiated_registration to
                   FALSE to fix another bug found while testing the code
                   added to avoid initiating a full search for service when
                   the phone is already camped on the desired PLMN.
02/26/04   kwa     Added code to handle an mmr_reg_cnf or mmr_service_ind
                   that contains MMR_CAUSE_FAILURE_REMAIN_ON_PLMN and also
                   indicates no service.  This is required to handle the
                   situation when the phone camps on a cell where all access
                   classes are barred.
03/07/04   kwa     Added call to reg_nv_write_cache in
                   reg_state_process_cm_stop_mode_req.
03/15/04   kwa     Modified reg_state_process_cm_stop_mode_req to only call
                   reg_nv_write_cache when the stop mode reason is
                   SYS_STOP_MODE_REASON_OFFLINE or
                   SYS_STOP_MODE_REASON_POWER_OFF.
03/17/04   kwa     Added call to reg_mode_initialize_equivalent_plmn_list in
                   reg_state_process_cm_sim_available_req.
04/05/04   kwa     Added additional debug messages in
                   reg_state_process_cm_service_req.  Fixed a bug in
                   reg_state_process_mmr_reg_cnf which prevented the MS from
                   searching for limited service on both RATs.
04/06/04   kwa     Added code in reg_state_process_mmr_reg_cnf to allow the
                   phone to remain camped in limited service on a forbidden LAI.
                   This change is featurized using
                   FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI.
04/09/04   kwa     Added code in reg_state_process_cm_service_ind to insure an
                   mmr_reg_req is sent to MM if the phone has not successfully
                   registered for the requested service.
04/12/04   kwa     Modified handling of cm_stop_mode_req with reason
                   SYS_STOP_MODE_REASON_MODE_CHANGE to better handle failed
                   transitions from GSM to 1X on MSM6300.
04/15/04   kwa     Modified debug messages in reg_state_process_cm_service_req.
                   Removed unused function reg_state_update_reg_service_domain.
04/21/04   kwa     Added code in reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to set the service_status
                   to SYS_SRV_STATUS_LIMITED_REGIONAL when the phone is in
                   manual mode and it attempts to camp on a forbidden PLMN.
                   This change should insure that SD will not force multiple
                   attempts to register on a forbidden PLMN.
05/03/04   kwa     Added code in reg_state_process_mmr_service_ind to filter
                   out mmr_service_ind messages indicating limited service
                   when the service_status is SYS_SRV_STATUS_LIMITED_REGIONAL.
05/06/04   kwa     Added code in reg_state_process_cm_service_req to avoid
                   initiating a search for limited service when the phone is
                   already camped on a PLMN.
05/16/04   kwa     Added code to support cs_for_request and the
                   mmr_camped_ind message.
06/01/04   kwa     Removed code in reg_state_process_mmr_service_ind that
                   changes the service status from limited to no service since
                   this caused the phone to ping pong between GSM and WCDMA
                   while camped in limited service.
06/16/04   kwa     Added code to initialize reg_state_service_state.
07/13/04   kwa     Modified handling of MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED in
                   reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind so the UI is informed that
                   the phone only has limited service while attempting to
                   reacquire service on another LAI.
07/16/04   kwa     Added additional check in reg_state_process_mmr_service_ind
                   to only filter out limited service indications if the
                   cause value is MMR_CAUSE_NOT_SPECIFIED.
08/02/04   kwa     Modified handling of SYS_STOP_MODE_REASON_MODE_CHANGE in
                   reg_state_process_cm_stop_mode_req so REG will once again
                   allow user initiated activity while the lower layers of the
                   stack are processing the stop mode request.
08/05/04   kwa     Modified handling of MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED in
                   reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to only change state when
                   the phone is in automatic mode.
08/05/04   kwa     Modified reg_state_process_mmr_reg_cnf to terminate PLMN
                   selection if the SIM has been invalidated for CS service.
08/09/04   kwa     Added code in reg_state_process_mmr_service_ind to check
                   that service was reported on a valid RAT for the requested
                   mode preference.  If service is reported on an invalid RAT
                   then the service status is set to NO_SERVICE so SD will
                   reinitiate PLMN selection.
08/13/04   kwa     Modified reg_state_process_mmr_reg_cnf to terminate PLMN
                   selection upon receiving an mmr_reg_cnf with the cause
                   value set to MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION.
08/17/04   kwa     Removed unnecessary function reg_state_update_sim_state.
08/26/04   kwa     Modified reg_state_process_mmr_reg_cnf to initiate PLMN
                   selection automatically without informing SD when the cause
                   value is MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION.
09/02/04   kwa     Modified handling of MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED in
                   reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind so the stack will move
                   to GSM only mode if the phone is operating in dual-mode
                   and the reject is received on WCDMA.
09/17/04   kwa     Modified reg_state_process_mmr_service_ind to better handle
                   an mmr_service_ind indicating limited service when the
                   current service status is SYS_SRV_STATUS_LIMITED_REGIONAL.
10/04/04   kwa     Added error checking in reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to ignore cases when MM
                   improperly reports that the SIM is not available.
10/16/04   kwa     Modified handling of MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED
                   in reg_state_process_mmr_reg_cnf if this cause is received
                   from the HPLMN while camped on WCDMA.  In this scenario REG
                   will initiate an attempt to acquire service in GSM only mode.
11/10/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH in
                   reg_state_process_cm_network_list_req.
12/01/04   up      Function 'reg_state_current_state_get' is added to get the
                   current state of REG. This function is the interface to
                   outside to get the REG current state, used by REGUTF.
01/11/05   up      Mainlining FEATURE_MANUAL_HPLMN_SELECTION.
01/18/05   up      Updated function reg_state_process_mmr_service_ind to set
                   reacquired_service when moving from HPLMN to VPLMN.
01/24/05   up      HPLMN timer is started when MS is in LIMITED service.
                   i.e. when REG receives reg_cnf(REMAIN_ON_PLMN).
                   Also added Function reg_state_service_plmn_get.
02/17/05   up      Modified reg_state_process_cm_service_req,
                   reg_state_process_mmr_reg_cnf and
                   reg_state_process_mmr_service_ind to handle new mode
                   SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION.
02/22/05   up      Updated for Cingular ENS feature.
                   Flag reg_state_delay_fcn_act_hplmn is added to delay FCN.
                   Added functions reg_state_process_fcn_hplmn,
                   reg_state_process_fcn_act_hplmn,
                   reg_state_process_sim_refresh_fcn_ind,
                   reg_state_unexpected_sim_refresh_fcn_ind.
                   Updated function reg_state_main.
02/25/05   up      Updated reg_state_process_cm_service_req to set
                   the service status to NO_SERVICE when PLMN search starts.
03/01/05   up      ENS Fix - supported flags are replaced with hash defines.
                   Added reg_state_process_sim_refresh_ind for SIM_REFRESH_IND.
04/15/05   up      Updated reg_state_process_mmr_service_ind, do not change
                   service status to NO_SERVICE if the PLMN is forbidden.
05/05/05   up      Updated reg_state_process_mmr_service_ind to start HPLMN
                   timer in LIMITED service.
05/18/05   up      ENS fix - Updated reg_state_process_fcn_act_hplmn to remove
                   Act-HPLMN from FPLMN list.
07/15/05   up      Start HPLMN timer when any service received with cause
                   MMR_CAUSE_FAILURE_REMAIN_ON_PLMN from MM.
07/15/05   up      Updated to send ePLMN list change indication to MM.
07/18/05   up      Supporting for two new ENS EFs UIM_USIM_ACT_HPLMN and 
                   UIM_USIM_SPT_TABLE under USIM directory.
07/27/05   up      Updated reg_state_process_mmr_service_ind to support
                   GCF 12.2.1.5d for OP mode C - Initiate PLMN selection on 
                   EPLMN if GPRS_FORBIDDEN received on the PLMN in OP mode C.
09/09/05   up      Updated reg_state_process_cm_service_req to send
                   network_selection_mode to MM instead of MANUAL_MODE.
09/19/05   up      Added reg_state_ignore_hplmn_search_request and updated
                   to support on demand HPLMN search from user.
09/21/05   up      Updated reg_state_process_mmr_service_ind to remove PLMN
                   from FPLMN list if MS registered on FPLMN in MANUAL mode.
09/28/05   up      Try for LIMITED service when SIM is INVALID for CS/PS in
                   LIMITED_MANUAL_ACQ mode.
09/30/05   up      Updated reg_state_process_mmr_service_ind to not change the
                   service state to LIMITED_REGIONAL if cause is specified.
10/20/05   up      Updated for 3G to 2G BPLMN search. 
                   Kept under FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
10/27/05   up      Send CM_SIM_AVAILABLE_CNF even when SIM access failed.
11/07/05   ajt     Receive MMR_EMERGENCY_NUM_LIST_IND from MM, and
                   pass-through (i.e. no processing) it to CM.
11/09/05   up      Ignore the MMR_REG_CNF if the PLMN is not the requested PLMN
                   OR PLMN is not in equivalent PLMN list.
11/29/05   up      Updated reg_state_process_mmr_service_ind and
                   reg_state_process_mmr_reg_cnf to reacquire service on same 
                   PLMN in different LA when LU/RAU rejected with cause #15.
12/07/05   up      Handling cause FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN
                   to add the PLMN in GPRS FPLMN List and reamin on PLMN.
12/20/05   up      MMR_REG_CNF is not ignored if serving PLMN is in equivalent
                   PLMN list of non-requested PLMN.
01/02/06   up      Abort the manual search when service request is received.
01/05/06   sn      Added new functions reg_state_prev_plmn_get,
                   reg_state_roaming_ind_get. Updated function reg_state_
                   process_cm_network_list_req, start HPLMN Timer 10sec when
                   moving from AUTOMATIC_SEARCH state to MANUAL_SEARCH state.
                   Updated function reg_state_process_mmr_service_ind, to
                   update the value of reg_state_prev_plmn and removed usage
                   of reacquired service and prev_roaming_ind. Updated func
                   reg_state_process_mmr_reg_cnf to update the value of reg_
                   state_prev_plmn.
01/12/06   up      Use original mode-pref while re-initiating plmn-selection.                   
01/18/06   up      Added reg_state_plmn_search_transaction_id_get to get and
                   reg_state_plmn_search_transaction_id_set to generate transID.
02/20/06   sn      EHPLMN Feature
02/23/06   up      Accept MMR_REG_CNF if MM rejects the REG_REQ. 
03/23/06   up      Initiate PLMN selection with HPLMN when LU/RAU rejected
                   with cause #13 (Table 1, 23.122).
04/05/06   sn      Start 10 sec timer when automatic search is aborted by
                   CM_SERVICE_REQ Automatic.
04/06/06   sn      Updated reg_state_process_cm_stop_mode_req to update NV when
                   reason is SYS_STOP_MODE_REASON_MODE_CHANGE.
04/06/06   sn      Added reg_state_process_cell_service_ind. Added
                   support for MMR_CELL_SERVICE_IND
04/11/06   sn      Added reg_state_reg_service_domain_get
05/02/06   up      Abort AUTOMATIC search when service request is received.
05/23/06   sn      Updated process_mmr_service_ind to start timer when network
                   selection mode is LIMITED_ROUTINE_ACQUISITION. Updated
                   process_mmr_reg_cnf to always start timer when having full
                   service or limited service on a VPLMN. 
06/09/06   sn      Added processing of SYS_STOP_MODE_REASON_LPM in
                   process_cm_stop_mode_req
07/03/06   sn      Added function reg_state_active_rat_set
09/14/06   sn      Send LIMITED_REGIONAL if reject #13 in manual mode.       
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
08/28/06   sn      Reject cause 15 handling changes.
09/20/06   sn      Added handling for MMR_CAUSE_MAX_ATTEMPTED.
10/12/06   sn      Added WCDMA_1X mode preference implementation.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
24/11/06   sn      Send LIMITED_REGIONAL in manual mode only if limited service
                   on manually requested plmn.
11/23/06   sn      FEATURE_PRESERVE_MANUAL_PLMN_LIST
02/22/07   sn      Removal from forbidden lists and RPLMN active rat to be 
                   updated only when cause is not specified.
12/15/09   abhi  Removing UOOS code 
01/31/11   rajesh  voice domain preference and UE usage setting related changes
07/11/11   rajesh  NAS will inform NO SERVICE to CM if SIM becomes PS Invalid only
                   if the active rat is LTE.
07/21/11   rajesh  Stopping PLMN selection on LTE when in limited regional service.
08/01/11   rajesh  Edited debug print messages for CM_SERVICE_REQ.
08/09/11   abhi   Sending ph_status_change_req(LPM) to MM/EMM if EFS NV item is set
09/20/11   sla    Make EHPLMN list length to zero when ph status changed req is 
                  received but not when stop mode request is received
09/20/11   sla    Store last gstk sent PLMN.send IDLE event to GSK if camped 
                  PLMN MCC and last gstk sent PLMN MCC are different
09/20/11   sla    Store last gstk service status sent.send IDLE event to GSK 
                  if current service status and last gstk sent service status
                  are different
10/10/11   gps    sending mmr eq plmn indication if LTE feature is enabled
                  while processing sim subscription ready
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
11/29/11  bjanga  update mode preference irrespective of net sel mode  
11/29/11  sla     Sending IDLE event with no service when Scan is complete and no plmn found
05/12/11   gps    new interface change, roam_status added in conn mode service status indication. 
12/06/11  sla     In manual mode, if registration is rejected with cause #11,
                  then send IDLE event to GSTK
27/12/11  gps     processing mmr_service_indication, instead of filtering during interrat.
09/16/11   HC     Added default argument void to void reg_state_log_variables(void); 
09/05/11   ss      Added additional check of oos flag before sending the reg
                   request again
3/26/13   c_mg    Ported CR-447611 
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "err.h"
#include "ms.h"
#include "msg.h"
#include "reg_log.h"
#include "reg_mode.h"
#include "reg_nv.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "reg_timers.h"
#include "sys_eplmn_list_v.h"
#include "sys_plmn_selection_v.h"
#include "sys_cnst.h"
#include "reg_mm_v.h"
#include "mm_v.h"
#include "mmtask_int.h"
#include <stringl/stringl.h>
#include "ds3gpp_api.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_SGLTE
#include "policyman_sglte.h"
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                           EXTERN FUNCTION DECLARATION

==============================================================================*/
extern boolean full_srv_req_in_emerg;

#ifdef FEATURE_LTE
extern boolean reg_bit_mask_check
(
  uint32 value,
  byte bit_mask
);
#endif

extern void mm_get_cell_service_capabilties
(
  mmr_cell_service_ind_s_type *mmr_cell_capability
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)|| defined (FEATURE_SVLTE_DUAL_SIM)
extern void reg_send_mmr_ue_mode_change_ind(sys_ue_mode_e_type  reg_mode_change_indication,boolean ue_submode_srlte);
void reg_state_process_cm_mode_change_ind(sys_ue_mode_e_type  reg_mode_change_indication, boolean ue_submode_srlte);
#endif

#ifdef FEATURE_DUAL_DATA
void reg_state_process_cm_data_priority_ind(sys_data_priority_e_type  reg_data_prio_ind);
#endif

#ifdef FEATURE_SGLTE
/* Ue mode initialized to normal so that, it not crash in DSDS super build*/
sys_ue_mode_e_type   reg_sglte_ue_mode = SYS_UE_MODE_NORMAL  ;
boolean             reg_sglte_mode;
void reg_state_process_service_stat_ind(mmr_service_stat_ind_s_type* msg_p);
void reg_state_process_stack_deact_ind(mmr_stack_deact_ind_s_type* msg_p);
void reg_state_process_stack_act_ind(mmr_stack_act_ind_s_type* msg_p);
void reg_state_process_merge_operation(void);
void reg_state_process_split_operation(void);


#endif

#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
sys_overall_feature_t reg_sub_config_sim[MAX_AS_IDS];
#define reg_sub_config reg_sub_config_sim[reg_sub_id]

extern uint8           reg_sim_max_subscription;
#endif

#if defined FEATURE_DUAL_SIM
extern reg_as_id_e_type reg_multimode_sub;
#endif


/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)

static reg_state_s_type* reg_state_curr_p_sim[MAX_NAS_STACKS] = { &reg_state_inactive,&reg_state_inactive, &reg_state_inactive};
#define reg_state_curr_p reg_state_curr_p_sim[reg_as_id]
static reg_state_s_type* reg_state_next_p_sim[MAX_NAS_STACKS] = { &reg_state_inactive,&reg_state_inactive, &reg_state_inactive};
#define reg_state_next_p reg_state_next_p_sim[reg_as_id]
reg_cm_network_list_req_e_type reg_last_recieved_list_req_type_sim[MAX_NAS_STACKS] = { REG_CM_NETWORK_LIST_NONE, REG_CM_NETWORK_LIST_NONE, REG_CM_NETWORK_LIST_NONE};
#define reg_last_recieved_list_req_type reg_last_recieved_list_req_type_sim[reg_as_id]
static reg_cm_transaction_id_type     reg_state_cm_transaction_id_sim[MAX_NAS_STACKS] = { 0, 0, 0};
#define reg_state_cm_transaction_id reg_state_cm_transaction_id_sim[reg_as_id]
static boolean                        reg_state_cm_initiated_registration_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE};
#define reg_state_cm_initiated_registration reg_state_cm_initiated_registration_sim[reg_as_id]
static sys_plmn_service_state_s_type  reg_state_service_state_sim[MAX_NAS_STACKS] = {
  {
    SYS_SIM_STATE_NOT_AVAILABLE,
    {{0xFF, 0xFF, 0xFF}},
    SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    FALSE,
    SYS_ROAM_STATUS_OFF,
    SYS_SRV_STATUS_NO_SRV,
    SYS_SRV_DOMAIN_NO_SRV,
    SYS_RAT_GSM_RADIO_ACCESS,
#ifdef FEATURE_GSM_BAND_PREF
    {SYS_BAND_MASK_EMPTY},
    0,
#endif
    FALSE
  },
  {
    SYS_SIM_STATE_NOT_AVAILABLE,
    {{0xFF, 0xFF, 0xFF}},
    SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    FALSE,
    SYS_ROAM_STATUS_OFF,
    SYS_SRV_STATUS_NO_SRV,
    SYS_SRV_DOMAIN_NO_SRV,
    SYS_RAT_GSM_RADIO_ACCESS,
#ifdef FEATURE_GSM_BAND_PREF
    {SYS_BAND_MASK_EMPTY},
    0,
#endif
    FALSE
  },{
  SYS_SIM_STATE_NOT_AVAILABLE,
  {{0xFF, 0xFF, 0xFF}},
  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
  FALSE,
  SYS_ROAM_STATUS_OFF,
  SYS_SRV_STATUS_NO_SRV,
  SYS_SRV_DOMAIN_NO_SRV,
  SYS_RAT_GSM_RADIO_ACCESS,
#ifdef FEATURE_GSM_BAND_PREF
  {SYS_BAND_MASK_EMPTY},
  0,
#endif
  FALSE
}};
#define reg_state_service_state reg_state_service_state_sim[reg_as_id]
#ifdef FEATURE_ENHANCED_NW_SELECTION
static boolean reg_state_delay_fcn_act_hplmn_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE};
#define reg_state_delay_fcn_act_hplmn reg_state_delay_fcn_act_hplmn_sim[reg_as_id]
#endif
static boolean reg_last_mm_message_only_for_reg_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define  reg_last_mm_message_only_for_reg reg_last_mm_message_only_for_reg_sim[reg_as_id]
static sys_plmn_id_s_type             reg_state_prev_plmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}, {{0xFF, 0xFF, 0xFF}}};
#define reg_state_prev_plmn reg_state_prev_plmn_sim[reg_as_id]
static byte                           reg_state_plmn_search_transaction_id_sim[MAX_NAS_STACKS] = { 0, 0, 0};
#define reg_state_plmn_search_transaction_id reg_state_plmn_search_transaction_id_sim[reg_as_id]
static reg_cm_service_req_e_type      reg_last_cm_service_req_type_sim[MAX_NAS_STACKS] = { REG_CM_SERVICE_REQ_NONE, REG_CM_SERVICE_REQ_NONE, REG_CM_SERVICE_REQ_NONE};
#define reg_last_cm_service_req_type reg_last_cm_service_req_type_sim[reg_as_id]

static boolean                        reg_state_camped_ind_received_sim[MAX_NAS_STACKS] = {FALSE, FALSE,FALSE};
#define reg_state_camped_ind_received reg_state_camped_ind_received_sim[reg_as_id]

static boolean                        reg_state_ignore_rplmnact_sim[MAX_NAS_STACKS] = {FALSE, FALSE,FALSE};
#define reg_state_ignore_rplmnact     reg_state_ignore_rplmnact_sim[reg_as_id]

uint8 reg_nv_context_id_sim[MAX_NAS_STACKS] = { 0, 1 ,2};
#define reg_nv_context_id reg_nv_context_id_sim[reg_as_id]

/*PLMN to be put at end of available user reselection and foreground plmn search*/
static sys_plmn_id_s_type                   reg_state_user_resel_plmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}} } ;

#define reg_state_user_resel_plmn (reg_state_user_resel_plmn_sim[reg_as_id])

static sys_radio_access_tech_e_type          reg_state_user_resel_plmn_rat_sim[MAX_NAS_STACKS] = {SYS_RAT_NONE, SYS_RAT_NONE, SYS_RAT_NONE};
#define reg_state_user_resel_plmn_rat (reg_state_user_resel_plmn_rat_sim[reg_as_id])

boolean reg_state_is_pwr_on_manual_plmn_selection_sim[MAX_NAS_STACKS] = {FALSE, FALSE, FALSE};
#define reg_state_is_pwr_on_manual_plmn_selection reg_state_is_pwr_on_manual_plmn_selection_sim[reg_as_id]

sys_stop_mode_reason_e_type reg_state_last_stop_mode_reason_sim[MAX_NAS_STACKS] = {SYS_STOP_MODE_REASON_NONE, SYS_STOP_MODE_REASON_NONE, SYS_STOP_MODE_REASON_NONE};
#define reg_state_last_stop_mode_reason reg_state_last_stop_mode_reason_sim[reg_as_id]

#ifdef FEATURE_GSTK
static  boolean   reg_need_to_send_gstk_event_sim[MAX_NAS_STACKS] = {TRUE, TRUE, TRUE};
#define reg_need_to_send_gstk_event reg_need_to_send_gstk_event_sim[reg_as_id]
static  sys_plmn_id_s_type     reg_sate_last_gstk_event_sent_plmn_sim[MAX_NAS_STACKS] =  {{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}} ;
#define reg_sate_last_gstk_event_sent_plmn reg_sate_last_gstk_event_sent_plmn_sim[reg_as_id]
static  sys_srv_status_e_type  reg_state_last_gstk_event_sent_svc_sim[MAX_NAS_STACKS] = {SYS_SRV_STATUS_NONE, SYS_SRV_STATUS_NONE, SYS_SRV_STATUS_NONE};
#define reg_state_last_gstk_event_sent_svc reg_state_last_gstk_event_sent_svc_sim[reg_as_id]
static  sys_radio_access_tech_e_type reg_state_last_gstk_event_sent_rat_sim[MAX_NAS_STACKS]={SYS_RAT_NONE, SYS_RAT_NONE, SYS_RAT_NONE};
#define reg_state_last_gstk_event_sent_rat reg_state_last_gstk_event_sent_rat_sim[reg_as_id]
#endif

static boolean reg_state_rlf_started_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_rlf_started reg_state_rlf_started_sim[reg_as_id]

static boolean reg_state_irat_hplmn_timer_search_ongoing_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_irat_hplmn_timer_search_ongoing reg_state_irat_hplmn_timer_search_ongoing_sim[reg_as_id]

static boolean reg_state_irat_lte_timer_search_ongoing_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_irat_lte_timer_search_ongoing reg_state_irat_lte_timer_search_ongoing_sim[reg_as_id]

static boolean reg_state_force_irat_search_initiated_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_force_irat_search_initiated reg_state_force_irat_search_initiated_sim[reg_as_id]

static word reg_state_cell_id_sim[MAX_NAS_STACKS] = {0xFFFF, 0xFFFF, 0xFFFF};
#define reg_state_cell_id reg_state_cell_id_sim[reg_as_id]

static boolean reg_state_lte_search_requested_sim[MAX_NAS_STACKS] = {FALSE, FALSE, FALSE};
#define reg_state_lte_search_requested reg_state_lte_search_requested_sim[reg_as_id]

static boolean reg_state_power_up_system_selection_sim[MAX_NAS_STACKS] = {TRUE, TRUE, TRUE};
#define reg_state_power_up_system_selection reg_state_power_up_system_selection_sim[reg_as_id]
#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

static reg_state_s_type* reg_state_curr_p_sim[MAX_NAS_STACKS] = { &reg_state_inactive, &reg_state_inactive};
#define reg_state_curr_p reg_state_curr_p_sim[reg_as_id]
static reg_state_s_type* reg_state_next_p_sim[MAX_NAS_STACKS] = { &reg_state_inactive, &reg_state_inactive};
#define reg_state_next_p reg_state_next_p_sim[reg_as_id]

reg_cm_network_list_req_e_type reg_last_recieved_list_req_type_sim[MAX_NAS_STACKS] = { REG_CM_NETWORK_LIST_NONE, REG_CM_NETWORK_LIST_NONE};
#define reg_last_recieved_list_req_type reg_last_recieved_list_req_type_sim[reg_as_id]

static reg_cm_transaction_id_type     reg_state_cm_transaction_id_sim[MAX_NAS_STACKS] = { 0, 0};
#define reg_state_cm_transaction_id reg_state_cm_transaction_id_sim[reg_as_id]
static boolean                        reg_state_cm_initiated_registration_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_state_cm_initiated_registration reg_state_cm_initiated_registration_sim[reg_as_id]
static sys_plmn_service_state_s_type  reg_state_service_state_sim[MAX_NAS_STACKS] = {
  {
  SYS_SIM_STATE_NOT_AVAILABLE,
  {{0xFF, 0xFF, 0xFF}},
  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
  FALSE,
  SYS_ROAM_STATUS_OFF,
  SYS_SRV_STATUS_NO_SRV,
  SYS_SRV_DOMAIN_NO_SRV,
  SYS_RAT_GSM_RADIO_ACCESS,
  {SYS_BAND_MASK_EMPTY},
  0,
  0,  
  FALSE,
  {SYS_CSG_ID_INVALID},
  FALSE,
  SYS_SRV_STATUS_NO_SRV,
  SYS_NO_SVC_CAUSE_NORMAL,
  0,
  0,
  FALSE,
  SYS_LTE_CS_CAPABILITY_NONE,
  SYS_ACQ_STATUS_NO_RESOURCE,
  {0, 0, 0, SYS_CS_LCS_INFO_NOT_AVAILBLE},
  SYS_EXTEND_SRV_INFO_NONE,
  SYS_REG_DOMAIN_NOT_APPLICABLE,
  {{0xFF, 0xFF, 0xFF}}
}, {
  SYS_SIM_STATE_NOT_AVAILABLE,
  {{0xFF, 0xFF, 0xFF}},
  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
  FALSE,
  SYS_ROAM_STATUS_OFF,
  SYS_SRV_STATUS_NO_SRV,
  SYS_SRV_DOMAIN_NO_SRV,
  SYS_RAT_GSM_RADIO_ACCESS,
  {SYS_BAND_MASK_EMPTY},
  0,
  0,  
  FALSE,
  {SYS_CSG_ID_INVALID},
  FALSE,
  SYS_SRV_STATUS_NO_SRV,
  SYS_NO_SVC_CAUSE_NORMAL,
  0,
  0,
  FALSE,
  SYS_LTE_CS_CAPABILITY_NONE,
  SYS_ACQ_STATUS_NO_RESOURCE,
  {0, 0, 0, SYS_CS_LCS_INFO_NOT_AVAILBLE},
  SYS_EXTEND_SRV_INFO_NONE,
  SYS_REG_DOMAIN_NOT_APPLICABLE,
  {{0xFF, 0xFF, 0xFF}}
}};
#define reg_state_service_state reg_state_service_state_sim[reg_as_id]
#ifdef FEATURE_ENHANCED_NW_SELECTION
static boolean reg_state_delay_fcn_act_hplmn_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_state_delay_fcn_act_hplmn reg_state_delay_fcn_act_hplmn_sim[reg_as_id]
#endif
static boolean reg_last_mm_message_only_for_reg_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define  reg_last_mm_message_only_for_reg reg_last_mm_message_only_for_reg_sim[reg_as_id]
static sys_plmn_id_s_type             reg_state_prev_plmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}},  {{0xFF, 0xFF, 0xFF}}};
#define reg_state_prev_plmn reg_state_prev_plmn_sim[reg_as_id]
static byte                           reg_state_plmn_search_transaction_id_sim[MAX_NAS_STACKS] = { 0, 0};
#define reg_state_plmn_search_transaction_id reg_state_plmn_search_transaction_id_sim[reg_as_id]
static reg_cm_service_req_e_type      reg_last_cm_service_req_type_sim[MAX_NAS_STACKS] = { REG_CM_SERVICE_REQ_NONE, REG_CM_SERVICE_REQ_NONE};
#define reg_last_cm_service_req_type reg_last_cm_service_req_type_sim[reg_as_id]

static boolean                        reg_state_camped_ind_received_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_state_camped_ind_received reg_state_camped_ind_received_sim[reg_as_id]

static boolean                        reg_state_ignore_rplmnact_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_state_ignore_rplmnact     reg_state_ignore_rplmnact_sim[reg_as_id]

uint8 reg_nv_context_id_sim[MAX_NAS_STACKS] = { 0, 1 };
#define reg_nv_context_id reg_nv_context_id_sim[reg_as_id]

/*PLMN to be put at end of available user reselection and foreground plmn search*/
static sys_plmn_id_s_type                   reg_state_user_resel_plmn_sim[MAX_NAS_STACKS] = { {{0xFF, 0xFF, 0xFF}}, {{0xFF, 0xFF, 0xFF}} } ;

#define reg_state_user_resel_plmn (reg_state_user_resel_plmn_sim[reg_as_id])

static sys_radio_access_tech_e_type          reg_state_user_resel_plmn_rat_sim[MAX_NAS_STACKS] = {SYS_RAT_NONE,SYS_RAT_NONE};
#define reg_state_user_resel_plmn_rat (reg_state_user_resel_plmn_rat_sim[reg_as_id])

boolean reg_state_is_pwr_on_manual_plmn_selection_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_is_pwr_on_manual_plmn_selection reg_state_is_pwr_on_manual_plmn_selection_sim[reg_as_id]

sys_stop_mode_reason_e_type reg_state_last_stop_mode_reason_sim[MAX_NAS_STACKS] = {SYS_STOP_MODE_REASON_NONE, SYS_STOP_MODE_REASON_NONE};
#define reg_state_last_stop_mode_reason reg_state_last_stop_mode_reason_sim[reg_as_id]

#ifdef FEATURE_GSTK
static  boolean   reg_need_to_send_gstk_event_sim[MAX_NAS_STACKS] = {TRUE,TRUE};
#define reg_need_to_send_gstk_event reg_need_to_send_gstk_event_sim[reg_as_id]
static  sys_plmn_id_s_type     reg_sate_last_gstk_event_sent_plmn_sim[MAX_NAS_STACKS] =  {{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}} ;
#define reg_sate_last_gstk_event_sent_plmn reg_sate_last_gstk_event_sent_plmn_sim[reg_as_id]
static  sys_srv_status_e_type  reg_state_last_gstk_event_sent_svc_sim[MAX_NAS_STACKS] = {SYS_SRV_STATUS_NONE,SYS_SRV_STATUS_NONE};
#define reg_state_last_gstk_event_sent_svc reg_state_last_gstk_event_sent_svc_sim[reg_as_id]
static  sys_radio_access_tech_e_type reg_state_last_gstk_event_sent_rat_sim[MAX_NAS_STACKS]={SYS_RAT_NONE, SYS_RAT_NONE};
#define reg_state_last_gstk_event_sent_rat reg_state_last_gstk_event_sent_rat_sim[reg_as_id]
#endif

static boolean reg_state_rlf_started_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_rlf_started reg_state_rlf_started_sim[reg_as_id]

static boolean reg_state_irat_hplmn_timer_search_ongoing_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_irat_hplmn_timer_search_ongoing reg_state_irat_hplmn_timer_search_ongoing_sim[reg_as_id]

static boolean reg_state_irat_lte_timer_search_ongoing_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_irat_lte_timer_search_ongoing reg_state_irat_lte_timer_search_ongoing_sim[reg_as_id]

static boolean reg_state_force_irat_search_initiated_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_force_irat_search_initiated reg_state_force_irat_search_initiated_sim[reg_as_id]

static boolean reg_state_irat_hplmn_timer_running_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_irat_hplmn_timer_running reg_state_irat_hplmn_timer_running_sim[reg_as_id]

static boolean reg_state_irat_lte_timer_running_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_irat_lte_timer_running reg_state_irat_lte_timer_running_sim[reg_as_id]

static word reg_state_cell_id_sim[MAX_NAS_STACKS] = {0xFFFF, 0xFFFF};
#define reg_state_cell_id reg_state_cell_id_sim[reg_as_id]

static boolean reg_state_lte_search_requested_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_state_lte_search_requested reg_state_lte_search_requested_sim[reg_as_id]

static boolean reg_state_power_up_system_selection_sim[MAX_NAS_STACKS] = {TRUE, TRUE};
#define reg_state_power_up_system_selection reg_state_power_up_system_selection_sim[reg_as_id]

#else
static reg_state_s_type* reg_state_curr_p = &reg_state_inactive;
static reg_state_s_type* reg_state_next_p = &reg_state_inactive;

reg_cm_network_list_req_e_type reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;

static reg_cm_transaction_id_type     reg_state_cm_transaction_id = 0;
static boolean                        reg_state_cm_initiated_registration = FALSE;

static sys_plmn_service_state_s_type  reg_state_service_state =
{
  SYS_SIM_STATE_NOT_AVAILABLE,
  {{0xFF, 0xFF, 0xFF}},
  SYS_SRV_CAPABILITY_NOT_AVAILABLE,
  FALSE,
  SYS_ROAM_STATUS_OFF,
  SYS_SRV_STATUS_NO_SRV,
  SYS_SRV_DOMAIN_NO_SRV,
  SYS_RAT_GSM_RADIO_ACCESS,
  {SYS_BAND_MASK_EMPTY},
  0,
  0,  
  FALSE,
  {SYS_CSG_ID_INVALID},
  FALSE,
  SYS_SRV_STATUS_NO_SRV,
  SYS_NO_SVC_CAUSE_NORMAL,
  0,
  0,
  FALSE,
  SYS_LTE_CS_CAPABILITY_NONE,
  SYS_ACQ_STATUS_NO_RESOURCE,
  {0, 0, 0, SYS_CS_LCS_INFO_NOT_AVAILBLE},
  SYS_EXTEND_SRV_INFO_NONE,
  SYS_REG_DOMAIN_NOT_APPLICABLE,
  {{0xFF, 0xFF, 0xFF}}
};;

#ifdef FEATURE_ENHANCED_NW_SELECTION
static boolean reg_state_delay_fcn_act_hplmn = FALSE;
#endif
static boolean reg_last_mm_message_only_for_reg = FALSE;

static sys_plmn_id_s_type             reg_state_prev_plmn = {{0xFF, 0xFF, 0xFF}};
static byte                           reg_state_plmn_search_transaction_id = 0;
static reg_cm_service_req_e_type      reg_last_cm_service_req_type = REG_CM_SERVICE_REQ_NONE;
static boolean                        reg_state_camped_ind_received = FALSE;
static boolean                        reg_state_ignore_rplmnact = FALSE;

/*PLMN to be put at end of available user reselection and foreground plmn search*/
static sys_plmn_id_s_type                   reg_state_user_resel_plmn = {{0xFF, 0xFF, 0xFF}};
static sys_radio_access_tech_e_type         reg_state_user_resel_plmn_rat = SYS_RAT_NONE;


boolean reg_state_is_pwr_on_manual_plmn_selection = FALSE;

sys_stop_mode_reason_e_type reg_state_last_stop_mode_reason = SYS_STOP_MODE_REASON_NONE;

#ifdef FEATURE_GSTK
static  boolean   reg_need_to_send_gstk_event = TRUE;
static  sys_plmn_id_s_type     reg_sate_last_gstk_event_sent_plmn =  {{0xFF, 0xFF, 0xFF}} ;
static  sys_srv_status_e_type  reg_state_last_gstk_event_sent_svc = SYS_SRV_STATUS_NONE;
static  sys_radio_access_tech_e_type reg_state_last_gstk_event_sent_rat = SYS_RAT_NONE;
#endif

static word reg_state_cell_id = 0xFFFF;

static boolean reg_state_lte_search_requested =  FALSE;

static boolean reg_state_irat_hplmn_timer_search_ongoing = FALSE;

static boolean reg_state_irat_lte_timer_search_ongoing =  FALSE;

static boolean reg_state_force_irat_search_initiated = FALSE;
static boolean reg_state_power_up_system_selection = TRUE;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a, (sys_modem_as_id_e_type)reg_sub_id, (sys_modem_as_id_e_type)reg_as_id) 
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a, (sys_modem_as_id_e_type)reg_as_id)

typedef struct reg_debug_msg_info_s
{
  IMH_T hdr;
  sys_modem_as_id_e_type as_id;
} reg_debug_msg_info_s_type;

#endif
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_sub_id)
#endif
#endif

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
sys_mode_pref_e_type              reg_sim_mode_pref_sim[MAX_NAS_STACKS] = {SYS_MODE_PREF_NONE,SYS_MODE_PREF_NONE,SYS_MODE_PREF_NONE};
#define  reg_sim_mode_pref reg_sim_mode_pref_sim[reg_as_id]

boolean reg_need_to_process_sim_avail_req_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_need_to_process_sim_avail_req reg_need_to_process_sim_avail_req_sim[reg_as_id]

boolean reg_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS] = {FALSE,FALSE, FALSE};
#define reg_waiting_for_open_session_cnf reg_waiting_for_open_session_cnf_sim[reg_as_id]

#ifdef FEATURE_LTE
static boolean   reg_state_cm_initiated_ps_detach_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_cm_initiated_ps_detach reg_state_cm_initiated_ps_detach_sim[reg_as_id]
#endif

reg_mode_foreground_search_status_e_type reg_mode_foreground_search_status_sim[MAX_NAS_STACKS] = {REG_MODE_FOREGROUND_SEARCH_NONE, REG_MODE_FOREGROUND_SEARCH_NONE, REG_MODE_FOREGROUND_SEARCH_NONE};
#define reg_mode_foreground_search_status reg_mode_foreground_search_status_sim[reg_as_id]

boolean reg_state_is_roaming_plmn_to_be_camped_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_is_roaming_plmn_to_be_camped reg_state_is_roaming_plmn_to_be_camped_sim[reg_as_id]
#ifdef FEATURE_LTE
boolean reg_state_force_pref_pending_sim[MAX_NAS_STACKS] = {FALSE,FALSE,FALSE};
#define reg_state_force_pref_pending reg_state_force_pref_pending_sim[reg_as_id]
#endif

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
sys_mode_pref_e_type              reg_sim_mode_pref_sim[MAX_NAS_STACKS] = {SYS_MODE_PREF_NONE,SYS_MODE_PREF_NONE};
#define  reg_sim_mode_pref reg_sim_mode_pref_sim[reg_as_id]

boolean reg_need_to_process_sim_avail_req_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_need_to_process_sim_avail_req reg_need_to_process_sim_avail_req_sim[reg_as_id]

boolean reg_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_waiting_for_open_session_cnf reg_waiting_for_open_session_cnf_sim[reg_as_id]

#ifdef FEATURE_LTE
static boolean   reg_state_cm_initiated_ps_detach_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_cm_initiated_ps_detach reg_state_cm_initiated_ps_detach_sim[reg_as_id]
#endif

reg_mode_foreground_search_status_e_type reg_mode_foreground_search_status_sim[MAX_NAS_STACKS] = {REG_MODE_FOREGROUND_SEARCH_NONE, REG_MODE_FOREGROUND_SEARCH_NONE};
#define reg_mode_foreground_search_status reg_mode_foreground_search_status_sim[reg_as_id]

boolean reg_state_is_roaming_plmn_to_be_camped_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_is_roaming_plmn_to_be_camped reg_state_is_roaming_plmn_to_be_camped_sim[reg_as_id]
#ifdef FEATURE_LTE
boolean reg_state_force_pref_pending_sim[MAX_NAS_STACKS] = {FALSE,FALSE};
#define reg_state_force_pref_pending reg_state_force_pref_pending_sim[reg_as_id]
#endif

#else
boolean reg_need_to_process_sim_avail_req = FALSE;
boolean reg_waiting_for_open_session_cnf = FALSE;
#ifdef FEATURE_LTE
static boolean reg_state_cm_initiated_ps_detach = FALSE;
#endif
reg_mode_foreground_search_status_e_type reg_mode_foreground_search_status     = REG_MODE_FOREGROUND_SEARCH_NONE;
boolean reg_state_is_roaming_plmn_to_be_camped = FALSE;
#ifdef FEATURE_LTE
boolean reg_state_force_pref_pending = FALSE;
#endif

#endif

#ifdef FEATURE_DUAL_SIM
static reg_cm_transaction_id_type     reg_state_cm_non_subid_transaction_id = 0;
static boolean                        reg_state_cm_sim_update_ind_pending_sim[MAX_NAS_STACKS];
#define reg_state_cm_sim_update_ind_pending reg_state_cm_sim_update_ind_pending_sim[reg_sub_id]
#else
static boolean                        reg_state_cm_sim_update_ind_pending;
#endif

  static  boolean reg_state_force_bplmn_search_start = FALSE;
lte_cell_list_type  *reg_state_lte_available_list_ptr;

lte_cell_list_type  *reg_state_lte_non_available_list_ptr;
inter_task_cell_id_type reg_state_lte_available_cell_id;

uint8 reg_state_hplmn_irat_timer_count = 0;

boolean mm_sim_open_session
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_client_id_type client_id
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
static sys_plmn_id_s_type                    reg_state_rlf_plmn = {{0xFF, 0xFF, 0xFF}};
static sys_radio_access_tech_e_type   reg_state_rlf_rat = SYS_RAT_NONE; 
#endif

/*This global indicates that PS detach cnf has been sent to CM.*/
#ifdef FEATURE_LTE
static boolean reg_state_mm_waiting_for_reg_req;
#ifdef FEATURE_LTE_REL9
static reg_mm_lte_cell_info_s_type reg_state_lte_cell_info_ind = {FALSE};
#endif
#endif

reg_debug_buffer_s_type reg_debug_buffer[REG_DEBUG_MESSAGE_BUFFER_SIZE]={{0}};
byte reg_debug_buffer_index = 0;
byte reg_msg_info = 0;
void reg_state_process_validate_sim_timer_expiry(void);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern mmgsdi_app_enum_type reg_sim_card_mode_sim[MAX_NAS_STACKS] ;
#define reg_sim_card_mode reg_sim_card_mode_sim[reg_as_id]
#else
mmgsdi_app_enum_type reg_sim_card_mode;
#endif

#ifdef FEATURE_FEMTO_CSG
static sys_csg_selection_config_e_type  reg_state_csg_selection_config = SYS_CSG_SELECTION_CONFIG_NONE;
boolean reg_state_power_on_csg_search = TRUE;
#endif 

#if defined FEATURE_DUAL_SIM
static boolean reg_state_dds_switch_in_progress_sim[MAX_AS_IDS] = {FALSE, FALSE};
#define reg_state_dds_switch_in_progress reg_state_dds_switch_in_progress_sim[reg_sub_id]
extern void reg_state_send_indication_on_other_stack(void);
#endif

#ifdef FEATURE_SGLTE_DUAL_SIM
static boolean reg_state_trm_failure_sglte_main = FALSE; 
#endif 
/*===========================================================================

FUNCTION REG_ADD_MESSAGE_TO_DEBUG_BUFFER

DESCRIPTION
  This function adds messages that REG received and sent to the reg_debug_buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE


===========================================================================*/
void reg_add_message_to_debug_buffer
(
  byte message_set, 
  byte message_id,
  boolean to_reg,
  sys_mode_pref_e_type                 mode_pref
)
{
  if(reg_debug_buffer_index >= REG_DEBUG_MESSAGE_BUFFER_SIZE)
  {
    reg_debug_buffer_index = 0;
  }
  reg_debug_buffer[reg_debug_buffer_index].message_set = message_set;
  reg_debug_buffer[reg_debug_buffer_index].message_id = message_id;
  reg_debug_buffer[reg_debug_buffer_index].present_reg_state= reg_state_curr_p->number();
  reg_debug_buffer[reg_debug_buffer_index].to_reg = to_reg;
  reg_debug_buffer[reg_debug_buffer_index].mode_pref = mode_pref;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  reg_debug_buffer[reg_debug_buffer_index].as_id = (sys_modem_as_id_e_type)reg_as_id;
  reg_debug_buffer[reg_debug_buffer_index].msg_info = reg_msg_info;
  reg_msg_info =0;
#endif
  reg_debug_buffer_index++;
}



/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

#ifdef REG_DEBUG

/*==============================================================================

FUNCTION NAME

  reg_state_current_state_get

==============================================================================*/

reg_state_e_type reg_state_current_state_get()
{
  return reg_state_curr_p->number();
}

#endif
#ifdef  FEATURE_GSTK
/*==============================================================================

FUNCTION NAME

  reg_state_is_gstk_event_to_be_send

==============================================================================*/
boolean reg_state_is_gstk_event_to_be_send()
{
  return reg_need_to_send_gstk_event;
}
/*==============================================================================

FUNCTION NAME

  reg_state_set_gstk_event_flag

==============================================================================*/
void reg_state_set_gstk_event_flag(boolean flag)
{
  reg_need_to_send_gstk_event = flag;
}
/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_plmn

==============================================================================*/
void reg_state_update_last_gstk_sent_plmn(sys_plmn_id_s_type plmn)
{
  reg_sate_last_gstk_event_sent_plmn = plmn;
}
/*==============================================================================

FUNCTION NAME

 reg_state_reset_last_gstk_event_sent

==============================================================================*/

void reg_state_reset_last_gstk_event_sent(void)
{
  reg_state_last_gstk_event_sent_svc = SYS_SRV_STATUS_NONE;
}
/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_svc

==============================================================================*/

void reg_state_update_last_gstk_sent_svc(sys_srv_status_e_type srv_status)
{
  reg_state_last_gstk_event_sent_svc = srv_status;
}

/*==============================================================================

FUNCTION NAME

  reg_state_get_last_gstk_sent_svc

==============================================================================*/

sys_srv_status_e_type reg_state_get_last_gstk_sent_svc(void)
{
  return reg_state_last_gstk_event_sent_svc;
}
/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_rat

==============================================================================*/

void reg_state_update_last_gstk_sent_rat(sys_radio_access_tech_e_type rat)
{
  reg_state_last_gstk_event_sent_rat = rat;
}

/*==============================================================================

FUNCTION NAME

  reg_state_get_last_gstk_sent_rat

==============================================================================*/

sys_radio_access_tech_e_type reg_state_get_last_gstk_sent_rat(void)
{
  return reg_state_last_gstk_event_sent_rat;
}

void reg_state_reset_last_gstk_rat_sent(void)
{
  reg_state_last_gstk_event_sent_rat = SYS_RAT_NONE;
}
#endif /* FEATURE_IMSI_SWITCH */

#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
void reg_state_reset_lte_cell_info(void)
{
  reg_state_lte_cell_info_ind.emergency_access_barred = FALSE;
  MSG_HIGH_DS_0(REG_SUB, "=REG= Resetting LTE cell info");
}
#endif


/*==============================================================================

FUNCTION NAME

  reg_state_is_no_service_to_be_send_due_to_china_mcc

 

==============================================================================*/

boolean reg_state_is_no_service_to_be_send_due_to_china_mcc(sys_plmn_id_s_type plmn)
{
  boolean result = FALSE;

  if((reg_state_is_roaming_plmn_to_be_camped )&&
     (reg_mode_is_china_mcc(plmn)||
#ifdef FEATURE_EQUIVALENT_HPLMN
        reg_sim_is_ehplmn(plmn)
#else
       (sys_plmn_match(reg_sim_read_hplmn(),plmn))
#endif 
        ))
{
  result = TRUE;
}

return result;

}

#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  reg_state_sglte_ue_mode_change_possible

 

==============================================================================*/

boolean reg_state_sglte_ue_mode_change_possible(sys_plmn_id_s_type plmn)
{

  policyman_status_t out_put    = POLICYMAN_STATUS_SUCCESS;
  policyman_status_t out_put1   = POLICYMAN_STATUS_SUCCESS;
  boolean val = FALSE;
  boolean val1 = FALSE;
  out_put = policyman_sglte_plmn_is_sglte(&plmn,&val);
  out_put1 = policyman_is_sglte_allowed(&val1);
  /* This function will return TRUE, if Policy Manager API Fails*/
  MSG_HIGH_DS_3(REG_SUB, "=REG= Sglte out_put %d, out_put1 %d val %d",out_put,out_put1,val);
  if((out_put != POLICYMAN_STATUS_SUCCESS) || 
      !(( reg_sglte_ue_mode == SYS_UE_MODE_SGLTE_HOME && !val) ||
        ((out_put1 == POLICYMAN_STATUS_SUCCESS) && val1 &&
          reg_sglte_ue_mode == SYS_UE_MODE_SGLTE_ROAM && val )))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

  
}
#endif
/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_network_list_req

==============================================================================*/
reg_state_p_type reg_state_process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  boolean dual_mode = FALSE;
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  sys_network_selection_mode_e_type nw_sel_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
  sys_service_search_e_type  service_search = SYS_SERVICE_SEARCH_NONE;
  sys_plmn_id_s_type plmn = {{0xFF, 0xFF, 0xFF}};
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  uint32 j, i; /* loop counters */
  sys_radio_access_tech_e_type rat_next;
  uint32 hplmn_list_length;
#endif

#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
  sys_plmn_rat_list_s_type *higher_priority_plmn_list = NULL;
#endif

#ifdef FEATURE_SGLTE
  if(REG_SGLTE_DUAL_STACK && reg_as_id == reg_ps_stack_id && reg_state_service_state.sim_state == SYS_SIM_STATE_PS_INVALID &&
     reg_mode_network_selection_mode_get()== SYS_NETWORK_SELECTION_MODE_LIMITED && 
     msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
   {
     sys_found_plmn_list_u_type     *available_list= NULL;
     MSG_HIGH_DS_1(REG_SUB, "=REG= loop back list confirmation to CM, as list request of type %d  sent on PS stack when sim is not PS valid",msg_p->list_search_type);
    /* Immediately respond CM_NETWORK_LIST_CNF with SEARCH FAILED */
#ifdef FEATURE_MODEM_HEAP
     available_list = (sys_found_plmn_list_u_type *)modem_mem_calloc(1,sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
     available_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeofy(sys_found_plmn_list_u_type));
#endif
     if(available_list == NULL)
     {
       MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate available_list Buffer", 0, 0, 0);
     }
     available_list->plmn_list.length = 0;
     reg_send_cm_network_list_cnf
     (
       reg_state_cm_transaction_id_get(),
       reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
       available_list,
       TRUE /* Search completed FALSE */
     );
#ifndef FEATURE_MODEM_HEAP
     gs_free(available_list);
#else
     modem_mem_free(available_list, MODEM_MEM_CLIENT_NAS);
#endif
     available_list = NULL;
     return reg_state_curr_p;
   }
#endif
if((msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND) &&
#ifdef FEATURE_DUAL_SIM 
           (reg_sim_per_subs_int_voice_forbidden( reg_state_service_state.plmn,(sys_modem_as_id_e_type)reg_sub_id ))
#else
           (reg_sim_int_voice_forbidden( reg_state_service_state.plmn ))
#endif
	   )
   {
     sys_found_plmn_list_u_type     *available_list= NULL;
     MSG_HIGH_DS_0(REG_SUB, "=REG= Reject BPLMN Search when Voice Roaming is OFF");
    /* Immediately respond CM_NETWORK_LIST_CNF with SEARCH FAILED */
#ifdef FEATURE_MODEM_HEAP
     available_list = (sys_found_plmn_list_u_type *)modem_mem_calloc(1,sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
     available_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeofy(sys_found_plmn_list_u_type));
#endif
     if(available_list == NULL)
     {
       MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate available_list Buffer", 0, 0, 0);
     }
      available_list->plmn_list.length = 0;
     reg_send_cm_network_list_cnf
     (
       reg_state_cm_transaction_id_get(),
       reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
       available_list,
       TRUE /* Search completed FALSE */
     );
#ifndef FEATURE_MODEM_HEAP
     gs_free(available_list);
#else
     modem_mem_free(available_list, MODEM_MEM_CLIENT_NAS);
#endif
     available_list = NULL;
     return reg_state_curr_p;
   }
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
  
#ifdef FEATURE_MODEM_HEAP
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
  if(higher_priority_plmn_list == NULL)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate higher_priority_plmn_list Buffer", 0, 0, 0);
  }

  higher_priority_plmn_list->length = 0;
#endif
  reg_state_last_stop_mode_reason = SYS_STOP_MODE_REASON_NONE;
  reg_mode_clear_plmn_backgnd_list();  

#ifdef FEATURE_RAT_PRIORITY_LIST 
  reg_mode_bst_status_set(FALSE);
  MSG_HIGH_DS_2(REG_SUB, "=REG= Rat priority list num_items = %d MCC mode band filter num_items = %d",
  msg_p->rat_pri_list_info.num_items,msg_p->mcc_mode_band_filter.num_items);
  for(i = 0; i < msg_p->rat_pri_list_info.num_items; i++)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= sys_mode = %d",
                msg_p->rat_pri_list_info.priority_list_info[i].acq_sys_mode);
    if(msg_p->rat_pri_list_info.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
#if (LTE_BAND_NUM == 256)
     MSG_HIGH_DS_4(REG_SUB, "=REG= : LTE bands 1_64 : %u,LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   msg_p->rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_1_64,
                   msg_p->rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_65_128,
                   msg_p->rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap. bits_129_192,
                   msg_p->rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_193_256);
#else
     MSG_HIGH_DS_1(REG_SUB, "=REG= LTE band_cap = %u ",
                   msg_p->rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap);
#endif
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= band_cap = %u",
                    msg_p->rat_pri_list_info.priority_list_info[i].band_cap.chgwt_band_cap);
    }
  }
  for(i =0; i< msg_p->mcc_mode_band_filter.num_items ;i++)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= sys_mode = %d",
                msg_p->mcc_mode_band_filter.priority_list_info[i].acq_sys_mode);
    if(msg_p->mcc_mode_band_filter.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
#if (LTE_BAND_NUM == 256)
     MSG_HIGH_DS_4(REG_SUB, "=REG= : LTE bands 1_64 : %u,LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.lte_band_cap.bits_1_64,
                   msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.lte_band_cap.bits_65_128,
                   msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.lte_band_cap.bits_129_192,
                   msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.lte_band_cap.bits_193_256);
#else
     MSG_HIGH_DS_1(REG_SUB, "=REG= LTE band_cap = %u ",
                    msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.lte_band_cap);
#endif
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= band_cap = %u",
                    msg_p->mcc_mode_band_filter.priority_list_info[i].band_cap.chgwt_band_cap);
    }
  }
  rat = reg_state_service_state.active_rat;
  dual_mode =((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_GSM_RADIO_ACCESS,&msg_p->rat_pri_list_info, FALSE))&&
                      ((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_UMTS_RADIO_ACCESS,&msg_p->rat_pri_list_info, FALSE)) ||
                        (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_TDS_RADIO_ACCESS,&msg_p->rat_pri_list_info, FALSE))));
#else 
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  /*
  ** Until WCDMA to GSM background PLMN searching is supported all
  ** dual-mode PLMN searches will be initiated on GSM.
  ** WCDMA to GSM background PLMN searching is supported.
  ** Send the PLMN search on current RAT.
  */
  switch (msg_p->mode_pref)
  {
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif
      rat = SYS_RAT_UMTS_RADIO_ACCESS;
      dual_mode = FALSE;
      break;

    case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
      rat = SYS_RAT_GSM_RADIO_ACCESS;
      dual_mode = FALSE;
      break;

    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
    case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
    default:
#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
      rat = reg_state_service_state.active_rat;
#else
      rat = SYS_RAT_GSM_RADIO_ACCESS;
#endif
      dual_mode = TRUE;
      break;
  }
#else
  /*
  ** Initiate the manual search so the preferred RAT is active after the
  ** the search is complete.  This should reduce the time to camp after
  ** selecting a network by eliminating a second power scan.
  */
  switch (msg_p->mode_pref)
  {
    case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
      rat = SYS_RAT_GSM_RADIO_ACCESS;
      break;

    case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif
      rat = SYS_RAT_UMTS_RADIO_ACCESS;
      break;

    default:
      MSG_FATAL_DS(REG_SUB, "=REG= Undefined mode preference (%d)", msg_p->mode_pref, 0, 0);
      break;
  }

  dual_mode = FALSE;
#endif
#endif


  reg_state_manual_search_initiated
  (
#ifndef FEATURE_RAT_PRIORITY_LIST
    msg_p->mode_pref,
#else
    &msg_p->rat_pri_list_info,
#endif /* FEATURE_RAT_PRIORITY_LIST */
    msg_p->list_search_type, 
    rat
  );

#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  MSG_HIGH_DS_1(REG_SUB, "=REG= LIST_REQ TYPE %d",msg_p->list_search_type);

  /*
  ** ABORT ongoing manual/background search if new search request is received.
  */

  if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
      (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
  {
    reg_send_mmr_plmn_search_abort_req();
  }

  reg_last_recieved_list_req_type = msg_p->list_search_type;
  if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
  {
    nw_sel_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;  
    if((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
       (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
    {
      service_search = SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC;
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
      higher_priority_plmn_list->length = 1;
      plmn = reg_sim_read_rplmn(reg_state_reg_service_domain_get()); 
      if((sys_plmn_id_is_undefined(plmn)) ||
#ifdef FEATURE_DUAL_SIM
         (reg_sim_per_subs_plmn_forbidden(plmn, (sys_modem_as_id_e_type)reg_sub_id))
#else
         (reg_sim_plmn_forbidden(plmn))
#endif
         )
      {
        plmn = reg_sim_read_hplmn();
      }
#ifdef FEATURE_LTE
      if (reg_sim_plmn_rat_forbidden(plmn,rat,(sys_modem_as_id_e_type)reg_sub_id))
      {
        sys_plmn_undefine_plmn_id(&(higher_priority_plmn_list->info[0].plmn));
      }
      else
#endif
      {
        higher_priority_plmn_list->info[0].plmn = plmn;
      }
      higher_priority_plmn_list->info[0].rat  = rat;
#ifndef FEATURE_RAT_PRIORITY_LIST
      if(dual_mode)
      {
        higher_priority_plmn_list->info[1].plmn = plmn;
        higher_priority_plmn_list->info[1].rat  = (rat == SYS_RAT_GSM_RADIO_ACCESS)?SYS_RAT_UMTS_RADIO_ACCESS:SYS_RAT_GSM_RADIO_ACCESS;
        higher_priority_plmn_list->length++;
      }
#else
      hplmn_list_length = 1;

      for (i = 0; i < msg_p->rat_pri_list_info.num_items;i++)
      {
        rat_next = reg_mode_convert_acq_mode_to_access_tech(
                               msg_p->rat_pri_list_info.priority_list_info[i].acq_sys_mode); 
        if(rat_next != rat && rat_next != SYS_RAT_NONE)
        {
#ifdef FEATURE_LTE
          if (reg_sim_plmn_rat_forbidden(plmn,rat_next,(sys_modem_as_id_e_type)reg_sub_id))
          {
            sys_plmn_undefine_plmn_id(&(higher_priority_plmn_list->info[hplmn_list_length].plmn));
          }
          else
#endif
          {
            higher_priority_plmn_list->info[hplmn_list_length].plmn = plmn;
          }
          higher_priority_plmn_list->info[hplmn_list_length].rat  = rat_next;
          hplmn_list_length++;
        }
      }
 
      higher_priority_plmn_list->length = hplmn_list_length;
#endif
#endif
    }
    else if ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) || 
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
    {
      service_search = SYS_SERVICE_SEARCH_BACKGROUND_MANUAL;  
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
      higher_priority_plmn_list->length = 1;
      higher_priority_plmn_list->info[0].plmn = reg_mode_req_plmn_get();
      higher_priority_plmn_list->info[0].rat  = rat;
#ifndef FEATURE_RAT_PRIORITY_LIST
      if(dual_mode)
      {
        higher_priority_plmn_list->info[1].plmn = reg_mode_req_plmn_get();
        higher_priority_plmn_list->info[1].rat  = (rat == SYS_RAT_GSM_RADIO_ACCESS)?SYS_RAT_UMTS_RADIO_ACCESS:SYS_RAT_GSM_RADIO_ACCESS;
        higher_priority_plmn_list->length++;
      }
#else
      hplmn_list_length = 1;
      
      for (i = 0; i < msg_p->rat_pri_list_info.num_items;i++)
      {
        rat_next = reg_mode_convert_acq_mode_to_access_tech(
                               msg_p->rat_pri_list_info.priority_list_info[i].acq_sys_mode); 
        if(rat_next != rat && rat_next != SYS_RAT_NONE)
        {
          higher_priority_plmn_list->info[hplmn_list_length].plmn = reg_mode_req_plmn_get();
          higher_priority_plmn_list->info[hplmn_list_length].rat  = rat_next;
          hplmn_list_length++;
        }
      }
      higher_priority_plmn_list->length = hplmn_list_length;
#endif
#endif
    }
  }
#endif
#ifdef FEATURE_FEMTO_CSG
  else if (msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH)
  {
    service_search = SYS_SERVICE_SEARCH_CSG;
    nw_sel_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
    if(msg_p->csg_search_type != SYS_CSG_SEARCH_TYPE_LEGACY)
    {
      reg_state_power_on_csg_search = FALSE;
    }
  }
#endif
  else if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND)
  {

    /****   REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND *******
     *
     *  When REG gets this type in Search request, it sends net sel mode = AUTOMATIC 
     *  service search type = PRIORITY_PLMN to lower layers.
     *  It sends PLMN list with EHPLMNs on all RATS present in RAT priority list 
     * 
     *  SBM Requirement: UE's containing dual IMSI card (EM and SBM profiles) 
     *  with current profile being loaded with  Emobile IMSI try to find SBM PLMN after
     *  certain period of time when a certain timer (say High Priority PLMN search timer) 
     *  expires in PM.
     *  With Emobile IMSI, HPLMN is Emobile PLMN while EHPLMN list contains SBM PLMNs.
     * 
     *  So, when NAS gets Search request with this type, NAS sends a serach request to AS 
     *  on SBM PLMN and AS mimics AUTO BGND search and abort the search when SBM PLMN is found. 
     *  Note that the understanding is that AS return same LIST REQ type in search cinfirmation.
     *  NAS gives search results to CM and CM gives to PM.
     * 
     *  Further, when UE gets registration reject with cause 17 when trying to register on SBM PLMN 
     *  with EM-IMSI, that makes UE to move to Limited service which NAS reports to GSTK card.
     *
     *  Card switches the profile to SBM profile. UE registers in SBM network.
     * 
     *  This makes UE find SBM PLMN automatically while IMSI profile loaded in UE is that of EMobile.
     *
     */

#ifndef FEATURE_MODEM_HEAP
    reg_sim_plmn_list_s_type *temp_plmn_list = (reg_sim_plmn_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_list_s_type));
#else
    reg_sim_plmn_list_s_type *temp_plmn_list = (reg_sim_plmn_list_s_type *)modem_mem_calloc(1,sizeof(reg_sim_plmn_list_s_type), MODEM_MEM_CLIENT_NAS);
#endif

    reg_check_for_null_ptr((void*)temp_plmn_list);
 
#ifdef FEATURE_EQUIVALENT_HPLMN
      *temp_plmn_list = reg_sim_ehplmn_list_get(); /* Get the EHPLMN list */
#else
       temp_plmn_list->plmn[0] = reg_sim_read_hplmn();
       temp_plmn_list->length  = 1;
#endif
  
      hplmn_list_length = 0;
      service_search = SYS_SERVICE_SEARCH_PRIORITY_PLMN; /* List search type for AS*/
      nw_sel_mode    = SYS_NETWORK_SELECTION_MODE_AUTOMATIC; /* net sel mode for AS */
      temp_plmn_list->length = MIN(temp_plmn_list->length, (sizeof(temp_plmn_list->plmn)/sizeof(temp_plmn_list->plmn[0]))); /* truncate length */
  
      /* If EHPLMN list is empty, 
       * or, if EHPLMN list contains only one PLMN that is HPLMN,
       * then send 0xFF 0xFF 0xFF PLMN ID to lower layers
       */
      if((temp_plmn_list->length == 0) || 
          (temp_plmn_list->length == 1 && 
           sys_plmn_match(reg_sim_read_hplmn(), temp_plmn_list->plmn[0])))
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= Processing PRIORITY_PLMN_SEARCH_BGND, EHPLMN list len = %d", temp_plmn_list->length); 
        temp_plmn_list->plmn[0].identity[0] = 0x40;
        temp_plmn_list->plmn[0].identity[1] = 0xF4;
        temp_plmn_list->plmn[0].identity[2] = 0x02;
        temp_plmn_list->length = 1;
    }
  
      /*  Fill PLMNs and RATs to send to MM and lower layers 
       *  Add --> All EHPLMN and RATs (in RAT prio list) combos are 
       *  sent to AS
       */
      for(j = 0; j < temp_plmn_list->length; j++)
      {
         for (i = 0; i < msg_p->rat_pri_list_info.num_items;i++)
         {
            rat_next = reg_mode_convert_acq_mode_to_access_tech(
            msg_p->rat_pri_list_info.priority_list_info[i].acq_sys_mode); 
  
            if( (rat_next != SYS_RAT_NONE) && 
                (hplmn_list_length < (sizeof(higher_priority_plmn_list->info)/sizeof(higher_priority_plmn_list->info[0]))))
            {
               higher_priority_plmn_list->info[hplmn_list_length].plmn = temp_plmn_list->plmn[j];
               higher_priority_plmn_list->info[hplmn_list_length].rat  = rat_next;
  

               MSG_HIGH_DS_3(REG_SUB, "=REG= higher_priority_plmn_list PLMN %x %x %x", 
                                  higher_priority_plmn_list->info[hplmn_list_length].plmn.identity[0], 
                                  higher_priority_plmn_list->info[hplmn_list_length].plmn.identity[1], 
                                  higher_priority_plmn_list->info[hplmn_list_length].plmn.identity[2]); 
               MSG_HIGH_DS_1(REG_SUB, "=REG= higher_priority_plmn_list rat = %d",rat_next); 
               MSG_HIGH_DS_3(REG_SUB, "=REG= temp_plmn_list PLMN %x %x %x", 
                                  temp_plmn_list->plmn[j].identity[0], 
                                  temp_plmn_list->plmn[j].identity[1], 
                                  temp_plmn_list->plmn[j].identity[2]);
               hplmn_list_length++;
            }
  }
      }
  
      higher_priority_plmn_list->length = hplmn_list_length;
  
      MSG_HIGH_DS_1(REG_SUB, "=REG= higher_priority_plmn_list.length = %d",higher_priority_plmn_list->length);

#ifndef FEATURE_MODEM_HEAP
      gs_free(temp_plmn_list);
#else
      modem_mem_free(temp_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
      temp_plmn_list = NULL;

  }
  else if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_PCI_PLMN_LIST_SEARCH)
  {
    service_search = SYS_SERVICE_SEARCH_PCI_PLMN;
    nw_sel_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
  }

  reg_send_mmr_plmn_search_req
  (
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
    nw_sel_mode,
#else
    SYS_NETWORK_SELECTION_MODE_MANUAL, /* network_selection_mode      */
#endif
    dual_mode,                         /* dual_mode                   */
    rat                                /* rat                         */
#ifdef FEATURE_GSM_BAND_PREF
    ,msg_p->band_pref                  /* band_pref                   */
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
    ,higher_priority_plmn_list        /* higher_priority_plmn_list_p */
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
    ,service_search
#endif
#ifdef FEATURE_FEMTO_CSG
    ,msg_p->csg_search_type
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,&msg_p->rat_pri_list_info
  ,&msg_p->mcc_mode_band_filter          
#endif
  ,msg_p->scan_scope                        
  );

#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  if(msg_p->list_search_type != REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
  {
#endif
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    /*
    ** REG needs to remember that the phone has lost service after
    ** initiating a manual search in order to support an optimization to
    ** avoid always searching for service upon receiving a cm_service_req.
    ** This code can be removed once background searching is supported.
    */

    reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  }
#endif

  reg_state_cm_initiated_registration = FALSE;

  if(reg_state_cm_sim_update_ind_pending)
  {
    reg_send_cm_sim_update_ind(SYS_SIM_STATE_AVAILABLE);
    reg_state_cm_sim_update_ind_pending = FALSE;
  }

  /*
  ** If current state is AUTOMATIC SEARCH then one PLMN search is currently
  ** going on. Need to start 10sec timer, to restart the PLMN search(AUTOMATIC)
  ** when timer expires in case MANUAL_SEARCH is aborted.
  */
  if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH)
  {
    /* Determine the kind of PLMN search that has been launched and start 
    ** the appropriate 10sec timer
    */
    if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
    {
       reg_timers_start_hplmn_search_timer_10sec();
    }
    else
    {
      reg_state_start_irat_timers_with_10sec();
    }
  }
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
#ifndef FEATURE_MODEM_HEAP
   gs_free(higher_priority_plmn_list);
#else
   modem_mem_free(higher_priority_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
   higher_priority_plmn_list = NULL;
#endif
  return &reg_state_manual_search;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_service_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_service_req
(
  const cm_service_req_s_type* msg_p
)
{
  sys_plmn_service_state_s_type service_state;
  sys_srv_domain_e_type         allowed_service_domain;
#ifdef FEATURE_EQUIVALENT_HPLMN
  reg_sim_plmn_list_s_type      ehplmn_list;
#else
  sys_plmn_id_s_type            hplmn;
#endif

  mmr_cell_service_ind_s_type cell_capabilties;

  reg_state_p_type              next_state_p = &reg_state_registering;
  boolean                       cm_service_req_same_as_last_req_with_call = FALSE;
  boolean                       system_change_no_rat_order_change = FALSE;

#ifdef FEATURE_LTE
  sys_lte_cs_domain_param_s_type prev_lte_cs_param;
#endif
#ifdef FEATURE_DUAL_DATA
  boolean                        prev_volte_only_ps = FALSE;
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
  uint32 index;

  MSG_HIGH_DS_3(REG_SUB, "=REG= Rat priority list num_items = %d Scan Scope type = %d, additional_info in CM_SERVICE_REQ = %d",
                        msg_p->rat_pri_list_info.num_items,msg_p->scan_scope,msg_p->additional_info);
  for(index = 0; index < msg_p->rat_pri_list_info.num_items; index++)
  {
      MSG_HIGH_DS_2(REG_SUB, "=REG= sys_mode = %d bst_rat_acq_required = %d ",
                msg_p->rat_pri_list_info.priority_list_info[index].acq_sys_mode,
                msg_p->rat_pri_list_info.priority_list_info[index].bst_rat_acq_required);
      if(msg_p->rat_pri_list_info.priority_list_info[index].acq_sys_mode == SYS_SYS_MODE_LTE)
      {
#if (LTE_BAND_NUM == 256)
       MSG_HIGH_DS_4(REG_SUB, "=REG= LTE band_cap : 1_64 %u, LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   msg_p->rat_pri_list_info.priority_list_info[index].band_cap.lte_band_cap.bits_1_64,
                   msg_p->rat_pri_list_info.priority_list_info[index].band_cap.lte_band_cap.bits_65_128,
                   msg_p->rat_pri_list_info.priority_list_info[index].band_cap.lte_band_cap.bits_129_192,
                   msg_p->rat_pri_list_info.priority_list_info[index].band_cap.lte_band_cap.bits_193_256);
        
        MSG_HIGH_DS_4(REG_SUB, "=REG= LTE bst_band_cap : 1_64 %u, LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap.bits_1_64,
                   msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap.bits_65_128,
                   msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap.bits_129_192,
                   msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap.bits_193_256);
#else
        MSG_HIGH_DS_1(REG_SUB, "=REG= LTE band_cap = %u ",
                      msg_p->rat_pri_list_info.priority_list_info[index].band_cap.lte_band_cap);

        MSG_HIGH_DS_1(REG_SUB, "=REG= LTE bst_band_cap = %u ",
                      msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap);
#endif
      }
      else
      {

        MSG_HIGH_DS_1(REG_SUB, "=REG= band_cap = %u",
                     msg_p->rat_pri_list_info.priority_list_info[index].band_cap.chgwt_band_cap);

        MSG_HIGH_DS_1(REG_SUB, "=REG= bst_band_cap = %u",
                     msg_p->rat_pri_list_info.priority_list_info[index].bst_band_cap.chgwt_band_cap);
      }
  }
#endif

if(((msg_p->cs_for_request != reg_mode_cs_for_request_get()) ||
      (msg_p->ps_for_request != reg_mode_ps_for_request_get())) &&
     (reg_state_curr_p->number() == REG_STATE_REGISTERING) &&
     (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_send_last_reg_req_rat_get())) &&
     (reg_mode_is_cm_service_request_same_as_last_except_call_for_request(msg_p) == TRUE) &&
     (reg_sim_allowed_service_domain(msg_p->req_service_domain) == reg_mode_req_service_domain_get())&&
     (reg_mode_foreground_search_status != REG_MODE_FOREGROUND_SEARCH_STARTED))
  {
    cm_service_req_same_as_last_req_with_call = TRUE;
  }
#ifdef FEATURE_LTE
  prev_lte_cs_param = reg_mode_lte_cs_domain_param_get();
  reg_mode_lte_cs_domain_param_set(msg_p->lte_cs_domain_param);
#endif

#ifdef FEATURE_SVLTE_DUAL_SIM
  reg_mode_timed_service_req_set(msg_p->additional_info);
#endif

  memset(&service_state, 0x00, sizeof(sys_plmn_service_state_s_type));
  reg_state_cm_transaction_id         = msg_p->transaction_id;
  reg_state_cm_initiated_registration = TRUE;
  reg_state_cm_sim_update_ind_pending = FALSE;

      /* reset last stop mode reason, In case CM_SERVICE_REQ came before sending CM_STOP_MODE_CNF*/
  reg_state_last_stop_mode_reason = SYS_STOP_MODE_REASON_NONE;
  if(msg_p->parameters.automatic.type != REG_CM_SERVICE_REQ_PERIODIC &&
     msg_p->parameters.manual.type != REG_CM_SERVICE_REQ_PERIODIC )
  {
    reg_mode_clear_plmn_backgnd_list();  
  }
#ifdef FEATURE_SGLTE
  if(REG_SUB_IS_SGLTE)
  {
    if(REG_SGLTE_DUAL_STACK && (reg_as_id == reg_ps_stack_id) &&
       (msg_p->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) && 
       ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) || 
        (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) || 
        (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
       (reg_state_service_state_sim[reg_cs_stack_id].service_status == SYS_SRV_STATUS_SRV) &&
       (reg_state_service_state_sim[reg_cs_stack_id].active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
       (reg_state_service_state_sim[reg_cs_stack_id].reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY))
    {
#ifdef FEATURE_SGLTE_DUAL_SIM
      /* do not start ps timer, as reg would try timed service req */
      if (!reg_state_get_trm_failure_sglte_main()) 
#endif
      {
        reg_timers_start_sglte_ps_timer();
      }
    }
  
    if((reg_sglte_mode) && (reg_as_id == reg_ps_stack_id) && (reg_ps_stack_id != reg_cs_stack_id)&& 
        reg_mode_is_only_req_rat_present_in_rat_pri_list(&msg_p->rat_pri_list_info, SYS_SYS_MODE_GSM))
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= only GSM present in RAT PRIO LIST on main satck in SGLTE mode, so reporting no service to CM");
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      reg_state_service_state.active_rat = SYS_RAT_GSM_RADIO_ACCESS;
      reg_send_cm_service_cnf(reg_state_cm_transaction_id,
                                reg_state_service_state);
      reg_state_cm_initiated_registration = FALSE;
      next_state_p = &reg_state_idle;
      return next_state_p;
    }
  }
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
  if(((reg_nv_use_rplmnact_mode_change_get() == FALSE)
#ifdef FEATURE_DUAL_SIM
       || (reg_state_dds_switch_in_progress == TRUE)
#endif
     ) &&
     (((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) && 
       (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)) ||
      ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
        (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)))
      &&
     (reg_mode_is_rat_pri_list_empty(reg_mode_rat_pri_list_get()) == FALSE)
      &&
     (reg_mode_rat_pri_list_3gpp_sys_mode_match(&(msg_p->rat_pri_list_info),
                                       reg_mode_rat_pri_list_get()) == FALSE))
  {
    reg_state_ignore_rplmnact_set(TRUE);
  }
  else
#endif
  {
    reg_state_ignore_rplmnact_set(FALSE);
  }

#ifdef FEATURE_DUAL_SIM
  reg_state_dds_switch_in_progress = FALSE;
#endif 

  if(msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    reg_last_cm_service_req_type        = msg_p->parameters.limited.type;
    if(reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)
    {
      reg_state_power_up_system_selection = TRUE;
    }
  }
  else if((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
          (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) ||
          (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
  {
    reg_last_cm_service_req_type        = msg_p->parameters.manual.type;
    if((msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
      (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))
    {
       reg_last_cm_service_req_type = REG_CM_SERVICE_REQ_NORMAL;
    }
  }
  else
  {
    reg_last_cm_service_req_type        = msg_p->parameters.automatic.type;
    if((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
      (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))
    {
      reg_last_cm_service_req_type = REG_CM_SERVICE_REQ_NORMAL;
    }
    if(msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)
    {
      reg_state_power_up_system_selection = TRUE;
    }
  }

  reg_mode_scan_scope_set(msg_p->scan_scope);              
#ifdef FEATURE_DUAL_DATA
  prev_volte_only_ps = reg_mode_volte_only_ps_get();
  reg_mode_volte_only_ps_set(msg_p->is_volte_only_ps);

  MSG_HIGH_DS_1(REG_SUB, "=REG= is_volte_only_ps = %d",
                        msg_p->is_volte_only_ps);
#endif

  if(msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
     msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY)
  {
   reg_state_is_pwr_on_manual_plmn_selection = FALSE;
  }

  if(msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;
  }

  if(((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) && 
      (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_ROAMING_PLMN)) ||
     ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)&&
      (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_ROAMING_PLMN)))
  {
    reg_state_is_roaming_plmn_to_be_camped = TRUE;
  }
  else
  {
    reg_state_is_roaming_plmn_to_be_camped = FALSE;
  }
  if((mm_managed_roaming_enabled)&&
  (!(((((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )) &&
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)) ||
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))||
      (mm_managed_roaming_retry_lu_in_manual_mode && 
      (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
      (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION ))&&
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL))))))
  {
    reg_sim_clear_low_priority_plmn_list(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                          (sys_modem_as_id_e_type)reg_as_id
#endif
                                        );
  }

#ifdef FEATURE_EOOS
  #error code not present
#endif /*FEATURE_EOOS*/

#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
  reg_mode_service_req_aborted_set(FALSE);
#endif 

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif

  allowed_service_domain = reg_sim_allowed_service_domain(msg_p->req_service_domain);

#ifdef FEATURE_LTE
  /* If SIM state is PS invalid then CM is not supposed to include LTE rat in the rat priority list */
  if(((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
       (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)  ||
       (msg_p->network_selection_mode ==SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
       (msg_p->network_selection_mode ==SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)) &&
      (reg_sim_state_get() == SYS_SIM_STATE_PS_INVALID)&&
      (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS,
                      &msg_p->rat_pri_list_info, FALSE)))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ -LTE present in rat priority list when ps domain is invalid");
    reg_send_cm_service_cnf
    (
      reg_state_cm_transaction_id,
      reg_state_service_state
    );

    reg_state_cm_initiated_registration = FALSE;
    next_state_p = reg_state_curr_p;
  }
  /*
  ** If UE is camped on LTE cell where access class is barred or bst band cap for LTE changes then dont loop 
  ** back service confirmation to CM in case there is emergency call pending.
  */
#ifdef FEATURE_LTE_REL9
  else if((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED) &&
          ((((reg_state_lte_cell_info_ind.emergency_access_barred == TRUE) ||
           ((reg_mode_rat_pri_list_match(&(msg_p->rat_pri_list_info),reg_mode_rat_pri_list_get())) &&
            (!reg_mode_bst_band_match(&msg_p->rat_pri_list_info,reg_mode_rat_pri_list_get(),SYS_SYS_MODE_LTE))))&&
          (reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS)) ||
            (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_RESTART_PLMN_SELECTION) == TRUE)) &&
          (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE))
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - LIMITED net_sel_mode %d",
                                       msg_p->network_selection_mode);

    reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
    reg_mode_bst_status_set(TRUE);
#endif
    /*
    ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
    */
    if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
        (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
    {
      reg_send_mmr_plmn_search_abort_req();
    }
    reg_mode_start_limited_service
    (
      msg_p->network_selection_mode,
      msg_p->mode_pref,
      msg_p->cs_for_request,
      msg_p->ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
      msg_p->req_service_domain,
      msg_p->band_pref
#else
      msg_p->req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      ,&msg_p->rat_pri_list_info
#endif
      ,msg_p->parameters.limited.type
#ifdef FEATURE_LTE 
     ,msg_p->sms_only,
      msg_p->additional_info
#endif
    );
       
    reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
  }
#endif
  else
#endif
  if ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED) ||
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
      ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) &&
      ((allowed_service_domain == SYS_SRV_DOMAIN_NO_SRV) || (reg_sim_state_get() == SYS_SIM_STATE_NOT_AVAILABLE))))
  {

    /* Clear the LAI reject list as new request came from CM */
    reg_send_mmr_clear_lai_reject_list_req();

         if ((((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV)&&
              ((reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED)  || 
               (msg_p->network_selection_mode         == SYS_NETWORK_SELECTION_MODE_LIMITED))
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
          /* when UE is camped in full service, if current rat is LTE and CM requests for
             emeregency service and if the current cell does not support emeregency service
              then don't loop back the service cnf.
          */
          && !((reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
               (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
               (reg_state_service_state.eps_nw_feature_support.emc_bs_supported == FALSE))
#endif
               ) ||
         ((((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED)
#ifdef FEATURE_LTE
            && ((reg_state_service_state.active_rat != SYS_RAT_LTE_RADIO_ACCESS) 
                 || (reg_mode_is_only_req_rat_present_in_rat_pri_list(&msg_p->rat_pri_list_info, SYS_SYS_MODE_LTE)
#ifdef FEATURE_LTE_REL9
                && (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == FALSE) 
#endif
                 )
#ifdef FEATURE_LTE_REL9
                || 
                ((reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
                 (reg_state_service_state.eps_nw_feature_support.emc_bs_supported == TRUE))
#endif
               )
#endif
         ) ||
         ((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
          /* when UE is camped in limited regional service, if current rat is LTE and CM requests for
             emeregency service and if the current cell does not support emeregency service
              then don't loop back the service cnf.
          */
          && !((reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
               (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
               (reg_state_service_state.eps_nw_feature_support.emc_bs_supported == FALSE))
#endif
            ))))&&
#ifndef FEATURE_RAT_PRIORITY_LIST         
#ifdef FEATURE_GSM_BAND_PREF
         (msg_p->band_pref       == reg_mode_band_pref_get()) &&
#endif
         (msg_p->mode_pref       == reg_mode_mode_pref_get())
#else
         (reg_mode_rat_pri_list_match(&(msg_p->rat_pri_list_info),reg_mode_rat_pri_list_get())) &&
         (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat))
#endif
         &&
         !((reg_state_curr_p->number() == REG_STATE_REGISTERING) 
            ||
           ((reg_nv_is_wcdma_freq_lock_enabled())&&
            (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)) )
#ifdef FEATURE_LTE
          && !(reg_state_mm_waiting_for_reg_req)
#endif
          && (reg_last_mm_message_only_for_reg == FALSE)
#ifdef FEATURE_LTE
          && (!(reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE &&
                         msg_p->cs_for_request == TRUE &&
                         full_srv_req_in_emerg == TRUE))
#endif
         )
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - LIMITED");

      reg_mode_network_selection_mode_set(msg_p->network_selection_mode);

      reg_send_cm_service_cnf
      (
        reg_state_cm_transaction_id,
        reg_state_service_state
      );

      reg_state_cm_initiated_registration = FALSE;
      next_state_p = reg_state_curr_p;
    }
    else
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - LIMITED net_sel_mode %d",
                                               msg_p->network_selection_mode);

      reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_bst_status_set(TRUE);
#endif
      /*
      ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
      */
      if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
          (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
      {
        reg_send_mmr_plmn_search_abort_req();
      }

      if(msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - LIMITED w/ type %d",
                    msg_p->parameters.limited.type);
      }

      reg_mode_start_limited_service
      (
        msg_p->network_selection_mode,
        msg_p->mode_pref,
        msg_p->cs_for_request,
        msg_p->ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
        msg_p->req_service_domain,
        msg_p->band_pref
#else
        msg_p->req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,&msg_p->rat_pri_list_info
#endif
       ,msg_p->parameters.limited.type
#ifdef FEATURE_LTE 
       ,msg_p->sms_only,
        msg_p->additional_info
#endif
      );
  
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
    }
  }
  else if (((reg_sim_state_get()  != SYS_SIM_STATE_NOT_AVAILABLE) &&
            ((reg_sim_card_mode == MMGSDI_APP_SIM) ||
             (reg_sim_card_mode == MMGSDI_APP_USIM))) &&
            (allowed_service_domain != SYS_SRV_DOMAIN_NO_SRV))
  {
    if (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
        msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY)
    {

      /* Clear the LAI reject list as new request came from CM */
      reg_send_mmr_clear_lai_reject_list_req();

      if((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE) &&
         (reg_mode_rat_pri_list_order_match(&msg_p->rat_pri_list_info, reg_mode_rat_pri_list_get())))
      {
        system_change_no_rat_order_change = TRUE;
      }


#ifndef FEATURE_EQUIVALENT_HPLMN
      hplmn = reg_sim_read_hplmn();

      /* Immediately respond back with current service status, If CM_SERVICE_REQ type is SYNC_UP, OR If type is 
      ** USER_SELECTION and UE is on full service on HPLMN with all other system parameters remaining same
      ** OR If type is NORMAL and UE is having full service and all other system parameters remaining same. 
      */
      
      if (((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
           (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))||
          ((((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_NORMAL) ||
              (system_change_no_rat_order_change == TRUE) ||
              (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN) ||
          ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_SELECTION ||
             msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION ||
             msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_HPLMN) && 
          (sys_plmn_match(hplmn, reg_state_service_state.plmn)))) &&
#else
      if (((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
           (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV)) ||
           ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT) && 
            sys_plmn_match(msg_p->parameters.automatic.plmn, reg_state_service_state.plmn) &&
            msg_p->parameters.automatic.rat == reg_state_service_state.active_rat) ||
           ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT) && 
#ifdef FEATURE_DUAL_SIM
           (reg_sim_per_subs_plmn_forbidden(msg_p->parameters.automatic.plmn, (sys_modem_as_id_e_type)reg_as_id)))
#else
           reg_sim_plmn_forbidden(msg_p->parameters.automatic.plmn))
#endif
           ||
          ((((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_NORMAL) ||
             (system_change_no_rat_order_change == TRUE) ||
             (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN) ||
          ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_SELECTION ||
            msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION  ||
            msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_HPLMN) &&
          (reg_sim_is_ehplmn(reg_state_service_state.plmn)))) &&
#endif
           (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
           (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_GSM_BAND_PREF
           (msg_p->band_pref       == reg_mode_band_pref_get()) &&
#endif
           ((msg_p->mode_pref       == reg_mode_mode_pref_get()) ||
            (reg_mode_is_dual_mode(msg_p->mode_pref) && 
             reg_mode_is_dual_mode(reg_mode_mode_pref_get()))
           ) 
#else
          (reg_mode_rat_pri_list_match(&msg_p->rat_pri_list_info, reg_mode_rat_pri_list_get())) &&
          (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat))
#endif

           && (msg_p->cs_for_request == FALSE) &&
           (msg_p->ps_for_request == FALSE) &&
           (reg_state_curr_p->number() != REG_STATE_INACTIVE) &&
           (allowed_service_domain == reg_mode_req_service_domain_get()) &&
           (allowed_service_domain == reg_state_reg_service_domain_get())
#ifdef FEATURE_LTE
           && (reg_mode_lte_cs_domain_param_match(msg_p->lte_cs_domain_param,
                                                  prev_lte_cs_param))
#endif
#ifdef FEATURE_DUAL_DATA
           && (msg_p->is_volte_only_ps == prev_volte_only_ps)
#endif
           )
#ifdef FEATURE_LTE
           && !(reg_state_mm_waiting_for_reg_req)
#endif
           ))
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC type=%d - NAS in service ",
                                                                msg_p->parameters.automatic.type);
        reg_mode_update_service_available_rat(SYS_RAT_NONE);
        reg_state_power_up_system_selection = FALSE;

        if( (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
             (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ))
        {
          reg_send_mmr_net_sel_mode_change_ind(msg_p->network_selection_mode);
        }
        reg_mode_network_selection_mode_set(msg_p->network_selection_mode);

#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_rat_pri_list_set(&(msg_p->rat_pri_list_info)); 
        reg_mode_bst_status_set(FALSE);
         /*Below setting of mode pref to be removed for LTE once mode_pref is removed from LTE*/
#endif /*FEATURE_LTE*/
        reg_mode_req_mode_pref_set(msg_p->mode_pref);


        /* For Manual to Automatic change, if phone registered on VPLMN, 
        ** start HPLMN timer now 
        */
        if(reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON)
        {
          reg_timers_start_hplmn_search_period_timer();
        }

        reg_send_cm_service_cnf
        (
          reg_state_cm_transaction_id,
          reg_state_service_state
        );

        /* Reg loop back for service request, sending cell capabilties again*/
        mm_get_cell_service_capabilties(&cell_capabilties
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                         ,(sys_modem_as_id_e_type)reg_as_id
#endif
                                       );
        reg_state_process_cell_service_ind(&cell_capabilties);
            
        reg_state_cm_initiated_registration = FALSE;

        /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        ** Do not abort ongoing Automatic Search.
        ** Set REG state accordingly
        */
        if (reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH)
        {
          reg_send_mmr_plmn_search_abort_req();
          next_state_p = &reg_state_idle;
        }
        else
        {
          next_state_p = reg_state_curr_p;
        }
        
      }
#ifdef FEATURE_ENHANCED_NW_SELECTION
      else if ((reg_sim_read_ens_flag()) &&
                (reg_mode_foreground_search_status == REG_MODE_FOREGROUND_SEARCH_ATTEMPTED) &&
                (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
                (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_GSM_BAND_PREF
                (msg_p->band_pref       == reg_mode_band_pref_get()) &&
#endif
                ((msg_p->mode_pref       == reg_mode_mode_pref_get()) ||
                 (reg_mode_is_dual_mode(msg_p->mode_pref) && 
                  reg_mode_is_dual_mode(reg_mode_mode_pref_get()))
                ) 
#else
              (reg_mode_rat_pri_list_match(&msg_p->rat_pri_list_info, reg_mode_rat_pri_list_get())) &&
              (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat))
#endif

           && (msg_p->cs_for_request == FALSE) &&
              (msg_p->ps_for_request == FALSE) &&
              (allowed_service_domain == reg_mode_req_service_domain_get()))
      {

        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ -Automatic type=%d - Full band scan already performed ",msg_p->parameters.automatic.type);
        reg_mode_update_service_available_rat(SYS_RAT_NONE);

        reg_mode_network_selection_mode_set(msg_p->network_selection_mode);

#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_rat_pri_list_set(&(msg_p->rat_pri_list_info)); 
        reg_mode_bst_status_set(FALSE);
         /*Below setting of mode pref to be removed for LTE once mode_pref is removed from LTE*/
#endif /*FEATURE_LTE*/
        reg_mode_req_mode_pref_set(msg_p->mode_pref);

        reg_send_cm_service_cnf
        (
          reg_state_cm_transaction_id,
          reg_state_service_state
        );

        reg_state_cm_initiated_registration = FALSE;
        next_state_p = reg_state_curr_p;
      }
#endif
      else if (((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_CAMPED_PLMN) &&
               (reg_state_service_state.plmn_reg_type != SYS_REG_DOMAIN_NOT_APPLICABLE) &&
               (reg_state_service_state.plmn_reg_type != SYS_REG_DOMAIN_NO_SERVICE))
#ifdef FEATURE_LTE 
               ||
               (((reg_bit_mask_check(msg_p->additional_info,
                               (byte)SYS_ADDTNL_INFO_FORCE_RADIO_CAP_UPDATE) == TRUE) ||
                 (reg_bit_mask_check(msg_p->additional_info,
                               (byte)SYS_ADDTNL_INFO_UE_CAP_CHANGE) == TRUE)) &&
                (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
                (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat)))
#endif
              )
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC - PLMN selection from camped PLMN/RAT,\
                         Type %d, Additional Info %d",
                         msg_p->parameters.automatic.type,msg_p->additional_info);

        reg_mode_update_service_available_rat(SYS_RAT_NONE);
        reg_mode_rat_pri_list_set(&(msg_p->rat_pri_list_info));

        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;

        reg_mode_start_plmn_selection_on_specified_plmn
            (
              reg_state_service_state.plmn,
              msg_p->network_selection_mode,
              msg_p->mode_pref,
              msg_p->cs_for_request,
              msg_p->ps_for_request,
              msg_p->req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
              ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,&msg_p->rat_pri_list_info
#endif
              ,reg_state_service_state.active_rat
#ifdef FEATURE_LTE
             ,msg_p->sms_only,
              msg_p->additional_info
#endif 
             ,MMR_REG_REQ_NORMAL
            ); 
      } 
      else if ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_SELECTION ) || 
               ( msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION ||
                  msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_HPLMN))
      {
        reg_mode_automatic_mode_e_type  automatic_mode = REG_MODE_AUTOMATIC_MODE_NO_PREFERENCE;
        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC type=%d - BSR or User Selection",
                                                                 msg_p->parameters.automatic.type);

       reg_mode_update_service_available_rat(SYS_RAT_NONE);

       if(msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_SELECTION )
       {
         automatic_mode = REG_MODE_AUTOMATIC_MODE_USER_RESELECTION;
       }
       else if(msg_p->parameters.automatic.type  == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION)
       {
         automatic_mode = REG_MODE_AUTOMATIC_MODE_BETTER_SYSTEM_RESELECTION;
       }
       else if(msg_p->parameters.automatic.type  == REG_CM_SERVICE_REQ_MMSS_START_HPLMN ||
               msg_p->parameters.automatic.type  == REG_CM_SERVICE_REQ_MMSS_START_RPLMN)
       {
         automatic_mode = REG_MODE_AUTOMATIC_MODE_MMSS_HPLMN;
       }
       else if(msg_p->parameters.automatic.type  == REG_CM_SERVICE_REQ_EF_RAT_REFRESH)
       {
         automatic_mode = REG_MODE_AUTOMATIC_MODE_EF_RAT_REFRESH;
       }
        /*
        ** ABORT ongoing manual/automatic search if new CM_SERVICE_REQ with this type is received.
        ** REG state to be set to registering
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        }

        reg_send_grat_scan_done_set(FALSE); 

        reg_state_user_resel_plmn = 
                           reg_sim_read_rplmn(reg_mode_req_service_domain_get());        
        reg_state_user_resel_plmn_rat = reg_state_service_state.active_rat;
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_bst_status_set(TRUE);
#endif
       reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;

       if((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_NORMAL)||
           (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN) ||
           (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)||
           (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYNC_UP))
       {
         reg_mode_set_hplmn_to_be_selected(TRUE);
       }
        reg_mode_start_automatic_plmn_selection
        (
          msg_p->network_selection_mode,
          automatic_mode,
          msg_p->mode_pref,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
          ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,&(msg_p->rat_pri_list_info)
#endif
#ifdef FEATURE_LTE 
         , msg_p->sms_only,
           msg_p->additional_info
#endif
        );
      
        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
      else if (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH)
      {
        MSG_HIGH_DS_0(REG_SUB,"=REG= CM_SERVICE_REQ - AUTOMATIC - SUITABLE SEARCH");

        reg_mode_update_service_available_rat(SYS_RAT_NONE);

        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        }

        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;

        reg_send_grat_scan_done_set(FALSE);
#ifdef FEATURE_RAT_PRIORITY_LIST
         reg_mode_bst_status_set(TRUE);
#endif

#ifdef FEATURE_SGLTE
        if(REG_SGLTE_DUAL_STACK)
        {
          reg_state_rlf_started = TRUE;
        }
#endif
        reg_mode_start_suitable_cell_selection
        (
          msg_p->network_selection_mode,
          msg_p->mode_pref,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          msg_p->req_service_domain         
        #ifdef FEATURE_GSM_BAND_PREF
          ,msg_p->band_pref
        #endif
          ,&msg_p->rat_pri_list_info
#ifdef FEATURE_LTE
          , msg_p->sms_only,
            msg_p->additional_info
#endif
        );
      }
#endif
#ifdef FEATURE_FEMTO_CSG
      else if (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN)
      {
          MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC  - CSG user selection type");

          reg_mode_update_service_available_rat(SYS_RAT_NONE);
                /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        } 
        if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH) 
        {
            if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
            {          
               reg_timers_start_hplmn_search_timer_10sec();
            }
            else
            { 
                /* Roaming is off if the IRAT HPLMN search is ongoing*/
              reg_state_start_irat_timers_with_10sec();

            }
        }

        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;

        reg_send_grat_scan_done_set(FALSE); 

        reg_mode_bst_status_set(TRUE);

        reg_mode_start_automatic_csg_selection
        (
          msg_p->network_selection_mode,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          msg_p->parameters.automatic.plmn,
          msg_p->parameters.automatic.rat,
          msg_p->parameters.automatic.csg_id,
          allowed_service_domain,
          &msg_p->rat_pri_list_info
#ifdef FEATURE_LTE
         , msg_p->sms_only,
           msg_p->additional_info
#endif 
        );

        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
      else if (msg_p->parameters.automatic.type == 
                                          REG_CM_SERVICE_REQ_CSG_SELECTION_APP)
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC  - CSG selection app type");

        reg_mode_update_service_available_rat(SYS_RAT_NONE);

        /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        } 
        if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH) 
        {
          if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
          {          
            reg_timers_start_hplmn_search_timer_10sec();
          }
          else
          { 
            /* Roaming is off if the IRAT HPLMN search is ongoing*/
            reg_state_start_irat_timers_with_10sec();

          }
        }

        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;

        reg_send_grat_scan_done_set(FALSE); 

        reg_mode_bst_status_set(TRUE);

        reg_mode_start_automatic_csg_selection_app
        (
          msg_p->network_selection_mode,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          allowed_service_domain,
          &msg_p->rat_pri_list_info
#ifdef FEATURE_LTE
         , msg_p->sms_only,
           msg_p->additional_info
#endif
        );

        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
#endif 
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
      else if ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SRV_MODE_UPDATE)
                &&
               (reg_bit_mask_check(msg_p->additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)
                &&
               (reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
                &&
               (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED))
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC - REG_CM_SERVICE_REQ_SRV_MODE_UPDATE");
        
        reg_mode_update_service_available_rat(SYS_RAT_NONE);
        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE;
        reg_mode_rat_pri_list_set(&(msg_p->rat_pri_list_info));

        reg_mode_start_plmn_selection_on_specified_plmn
            (
              reg_state_service_state.plmn,      /* plmn camped on in CAMP_ONLY mode */
              msg_p->network_selection_mode,
              msg_p->mode_pref,
              msg_p->cs_for_request,
              msg_p->ps_for_request,
              msg_p->req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
              ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,&msg_p->rat_pri_list_info
#endif
              ,reg_state_service_state.active_rat
#ifdef FEATURE_LTE
             ,msg_p->sms_only,
              msg_p->additional_info
#endif 
             ,MMR_REG_REQ_SRV_MODE_UPDATE
            ); 
      }
#endif 
      else if(msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_ROAMING_PLMN)
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC - REG_CM_SERVICE_REQ_ROAMING_PLMN ");

        reg_mode_update_service_available_rat(SYS_RAT_NONE);

        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
              (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        }
        if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH) 
        {
          if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
          {
            reg_timers_start_hplmn_search_timer_10sec();
          }
          else
          { 
           /* Roaming is off if the IRAT HPLMN search is ongoing*/
           reg_state_start_irat_timers_with_10sec();
          }
        }
        reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_bst_status_set(TRUE);
#endif
        reg_mode_start_plmn_selection_roaming_rplmn
        (
          msg_p->network_selection_mode,
          msg_p->mode_pref,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
          ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,&msg_p->rat_pri_list_info
#endif
        ,REG_CM_SERVICE_REQ_ROAMING_PLMN

#ifdef FEATURE_LTE 
         , msg_p->sms_only,
           msg_p->additional_info
#endif
        );
        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
      else if(cm_service_req_same_as_last_req_with_call == TRUE)
      {
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_bst_status_set(TRUE);
#endif
        reg_mode_call_for_bit_set(msg_p->cs_for_request, msg_p->ps_for_request);

        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC type=%d, re-sending last reg request with cs_for_req/ps_for_req enabled",
                                         msg_p->parameters.automatic.type);
        reg_send_last_mmr_reg_req_for_call
        (
          msg_p->rat_pri_list_info
        );
      }
      else
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - AUTOMATIC type=%d",
                                         msg_p->parameters.automatic.type);

        reg_mode_update_service_available_rat(SYS_RAT_NONE);
        /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        }
        
        if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH) 
        {
            if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
            {
                reg_timers_start_hplmn_search_timer_10sec();
            }
            else
            { 
                /* Roaming is off if the IRAT HPLMN search is ongoing*/
                reg_state_start_irat_timers_with_10sec();

            }
        }

        if((reg_sim_is_hplmn_to_be_selected()) &&
            ((((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_NORMAL) ||
                (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_MMSS_START_RPLMN)) &&
               (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))||
             (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYSTEM_CHANGE)||
             (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_EF_RAT_REFRESH)||
             ((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
              (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))
            ))
        {
          reg_mode_set_hplmn_to_be_selected(TRUE);
        }

        if(msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_PERIODIC)
        {
          reg_mode_copy_available_plmn_backgnd_list();
          reg_mode_clear_plmn_backgnd_list();  
        }

        reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_bst_status_set(TRUE);
#endif
        /* Store the passed in parameters entered by the user */
        if (msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
        {
            reg_state_user_resel_plmn = msg_p->parameters.automatic.plmn;
            reg_state_user_resel_plmn_rat = msg_p->parameters.automatic.rat;
        }        

        reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_NONE; 
       
        reg_mode_start_plmn_selection
        (
          msg_p->network_selection_mode,
          msg_p->mode_pref,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
          ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,&msg_p->rat_pri_list_info
#endif
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9) 
          ,((msg_p->parameters.automatic.type == REG_CM_SERVICE_REQ_SRV_MODE_UPDATE) ? REG_CM_SERVICE_REQ_NORMAL : msg_p->parameters.automatic.type)
#else
          ,(msg_p->parameters.automatic.type)
#endif
#ifdef FEATURE_LTE 
         , msg_p->sms_only,
           msg_p->additional_info
#endif
        );
        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
    }
#ifdef FEATURE_FEMTO_CSG
   else if (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
   {
     reg_mode_update_service_available_rat(SYS_RAT_NONE);

     MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL - Non RPLMN CSG selection");
     if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
        (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
     {
       reg_send_mmr_plmn_search_abort_req();
     } 
     
     reg_send_grat_scan_done_set(FALSE); 
     
     reg_mode_bst_status_set(TRUE);
     
     reg_mode_start_manual_csg_selection
     (
       msg_p->network_selection_mode,
       msg_p->cs_for_request,
       msg_p->ps_for_request,
       msg_p->parameters.manual.plmn,
       msg_p->parameters.manual.rat,
       msg_p->parameters.manual.csg_id,
       allowed_service_domain,
       &msg_p->rat_pri_list_info
       #ifdef FEATURE_LTE
       , msg_p->sms_only,
         msg_p->additional_info
       #endif 
     );
     
     reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;

   }
#endif 
    else
    {
      reg_mode_update_service_available_rat(SYS_RAT_NONE);

      /* If request type is SYNC_UP then immediately respond back with current service status */ 
      if((((msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_SYNC_UP) &&
         (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))||
         ((msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_ROAMING_PLMN)
             && ((reg_mode_is_china_mcc(msg_p->parameters.manual.plmn)) || 
#ifdef FEATURE_EQUIVALENT_HPLMN
                (reg_sim_is_ehplmn(msg_p->parameters.manual.plmn))
#else
                (sys_plmn_match(msg_p->parameters.manual.plmn,reg_sim_hplmn_get()))
#endif
          )))
#ifdef FEATURE_LTE
         && !(reg_state_mm_waiting_for_reg_req)
#endif
         )
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL");

        reg_mode_network_selection_mode_set(msg_p->network_selection_mode);

        reg_send_cm_service_cnf
        (
          reg_state_cm_transaction_id,
          reg_state_service_state
        );

        /* Reg loop back for service request, sending cell capabilties again*/
        mm_get_cell_service_capabilties(&cell_capabilties
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                         ,(sys_modem_as_id_e_type)reg_as_id
#endif
                                       );
        reg_state_process_cell_service_ind(&cell_capabilties);

        reg_state_cm_initiated_registration = FALSE;
        
        /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ with this type is received.
        ** There must not be an ongoing Automatic search if this request is received.
        ** REG state to be set to IDLE.
        */
        if (reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH)
        {
          reg_send_mmr_plmn_search_abort_req();
          next_state_p = &reg_state_idle;
        }
        else
        {
          next_state_p = reg_state_curr_p;
        }
      }
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
      else if(msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH)
      {

        MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL - SUTIABLE_SEARCH, type=%d",
                                         msg_p->parameters.manual.type);
        
        reg_send_grat_scan_done_set(FALSE); 
   
        reg_mode_bst_status_set(TRUE);
#ifdef FEATURE_SGLTE
        if(REG_SGLTE_DUAL_STACK)
        {
          reg_state_rlf_started = TRUE;
        }
#endif

        reg_mode_start_manual_plmn_selection
        (
          msg_p->network_selection_mode,
          reg_state_rlf_plmn,
          msg_p->mode_pref,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
          ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,&msg_p->rat_pri_list_info
#endif
          ,REG_CM_SERVICE_REQ_SUITABLE_SEARCH
#ifdef FEATURE_LTE 
          , msg_p->sms_only,
            msg_p->additional_info
#endif
        );

        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
#endif
#ifdef FEATURE_FEMTO_CSG
      else if (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN)
      {
         MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL  - CSG user selection type");
                /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        } 

        reg_send_grat_scan_done_set(FALSE); 

        reg_mode_bst_status_set(TRUE);

        reg_mode_start_manual_csg_selection
        (
          msg_p->network_selection_mode,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          msg_p->parameters.manual.plmn,
          msg_p->parameters.manual.rat,
          msg_p->parameters.manual.csg_id,
          allowed_service_domain,
          &msg_p->rat_pri_list_info
#ifdef FEATURE_LTE
         , msg_p->sms_only,
           msg_p->additional_info
#endif 
        );

        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
      else if (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_CSG_SELECTION_APP)
      {
         MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL  - CSG selection app type");
        /*
        ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
        */
        if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
            (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
        {
          reg_send_mmr_plmn_search_abort_req();
        } 

        reg_send_grat_scan_done_set(FALSE); 

        reg_mode_bst_status_set(TRUE);

        reg_mode_start_manual_csg_selection_app
        (
          msg_p->network_selection_mode,
          msg_p->cs_for_request,
          msg_p->ps_for_request,
          msg_p->parameters.manual.plmn,
          allowed_service_domain,
          &msg_p->rat_pri_list_info
#ifdef FEATURE_LTE
         , msg_p->sms_only,
           msg_p->additional_info
#endif 
        );

        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
#endif 

      else 
      {
        if((msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_SELECTION) || 
           (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT))
        {
          /* Clear the LAI reject list as new request came from CM */
          reg_send_mmr_clear_lai_reject_list_req();
        }

        switch (msg_p->parameters.manual.selected_plmn_type)
        {
          case REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN:
            /*
            ** If manually selected specified PLMN, then populate available_plmn_list_manual to  
            ** available_plmn_list 
            */
            if(msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_PERIODIC)
            {
              reg_mode_copy_available_plmn_backgnd_list();
              reg_mode_clear_plmn_backgnd_list();               
            }

            if((msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_SELECTION) ||
                (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT))
            {
              reg_mode_copy_available_plmn_list_manual(TRUE);
#ifdef FEATURE_SGLTE
              if( reg_state_sglte_ue_mode_change_possible(msg_p->parameters.manual.plmn)== TRUE )
#endif 
              {
                reg_mode_clear_available_plmn_list_manual();
              }
            }

            /*
            ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
            */
            if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
                (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
            {
              reg_send_mmr_plmn_search_abort_req();
            }
                    
            /*
            ** Attempt to acquire service on the specified PLMN if it is defined.
            */
            if (!sys_plmn_id_is_undefined(msg_p->parameters.manual.plmn))
            {  
              if ((sys_plmn_match(msg_p->parameters.manual.plmn, reg_state_service_state.plmn)) &&
                   (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
                   (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_GSM_BAND_PREF
                   (msg_p->band_pref       == reg_mode_band_pref_get()) &&
#endif
                   (msg_p->mode_pref       == reg_mode_mode_pref_get()) &&
#else
                   (reg_mode_rat_pri_list_order_match(&msg_p->rat_pri_list_info, reg_mode_rat_pri_list_get())) &&
                   (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat))&&
#endif
                   (allowed_service_domain == reg_mode_req_service_domain_get()) &&
                   (allowed_service_domain == reg_state_service_state.reg_service_domain) &&
                   ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                    (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
#ifdef FEATURE_LTE
                   && (reg_mode_lte_cs_domain_param_match(msg_p->lte_cs_domain_param,
                                                          prev_lte_cs_param))
                   && !(reg_state_mm_waiting_for_reg_req)
#endif
#ifdef FEATURE_DUAL_DATA
                   && (msg_p->is_volte_only_ps == prev_volte_only_ps)
#endif
                   && 
                   (( (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT) &&
                   (msg_p->parameters.manual.rat == reg_state_service_state.active_rat) ) ||
                   (msg_p->parameters.manual.type != REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT))
                   )
              {
                MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL");
                if( (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
                     (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ))
                {
                  reg_send_mmr_net_sel_mode_change_ind(msg_p->network_selection_mode);
                }
                reg_mode_network_selection_mode_set(msg_p->network_selection_mode);

                reg_send_cm_service_cnf
                (
                  reg_state_cm_transaction_id,
                  reg_state_service_state
                );

                /* Reg loop back for service request, sending cell capabilties again*/
                mm_get_cell_service_capabilties(&cell_capabilties
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                                 ,(sys_modem_as_id_e_type)reg_as_id
#endif
                                               );
                reg_state_process_cell_service_ind(&cell_capabilties);
                reg_mode_clear_available_plmn_list();

                reg_state_cm_initiated_registration = FALSE;
                next_state_p = &reg_state_idle;
             
              }
              else if(cm_service_req_same_as_last_req_with_call == TRUE)
              {
#ifdef FEATURE_RAT_PRIORITY_LIST
                reg_mode_bst_status_set(TRUE);
#endif
                reg_mode_call_for_bit_set(msg_p->cs_for_request, msg_p->ps_for_request);

                MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d, re-sending last reg request with cs_for_req/ps_for_req enabled",
                                          msg_p->parameters.manual.type);
                reg_send_last_mmr_reg_req_for_call
                (
                  msg_p->rat_pri_list_info
                );
              }
              else
              {
                MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d",
                                                 msg_p->parameters.manual.type);

               if (msg_p->parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
               {
                   reg_state_user_resel_plmn = msg_p->parameters.manual.plmn;
                   reg_state_user_resel_plmn_rat = msg_p->parameters.manual.rat;                
               }
                reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
                reg_mode_bst_status_set(TRUE);
#endif
                reg_mode_start_manual_plmn_selection
                (
                  msg_p->network_selection_mode,
                  msg_p->parameters.manual.plmn,
                  msg_p->mode_pref,
                  msg_p->cs_for_request,
                  msg_p->ps_for_request,
                  allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
                  ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
                  ,&msg_p->rat_pri_list_info
#endif
                  ,msg_p->parameters.manual.type
#ifdef FEATURE_LTE 
                 , msg_p->sms_only,
                   msg_p->additional_info
#endif                 
                 );

                reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV; 
              }
            }
            else if(cm_service_req_same_as_last_req_with_call == TRUE)
            {
#ifdef FEATURE_RAT_PRIORITY_LIST
              reg_mode_bst_status_set(TRUE);
#endif
              reg_mode_call_for_bit_set(msg_p->cs_for_request, msg_p->ps_for_request);

              MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d, re-sending last reg request with cs_for_req/ps_for_req enabled",
                              msg_p->parameters.manual.type);
              reg_send_last_mmr_reg_req_for_call
              (
                msg_p->rat_pri_list_info
              );
            }
            /*
            ** If the specified PLMN is not defined then attempt to acquire
            ** service on the RPLMN.
            */
            else
            {
              MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d",
                                              msg_p->parameters.manual.type);
  
              reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
              reg_mode_bst_status_set(TRUE);
#endif
              reg_mode_start_plmn_selection
              (
                msg_p->network_selection_mode,
                msg_p->mode_pref,
                msg_p->cs_for_request,
                msg_p->ps_for_request,
                allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
                ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
                ,&msg_p->rat_pri_list_info
#endif
                ,msg_p->parameters.manual.type
#ifdef FEATURE_LTE 
               , msg_p->sms_only,
                 msg_p->additional_info
#endif
              );
              reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
            }
            break;

          case REG_CM_MANUALLY_SELECTED_HPLMN:

            /*
            ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
            */
            if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
                (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
            {
              reg_send_mmr_plmn_search_abort_req();
            }
          
#ifndef FEATURE_EQUIVALENT_HPLMN
            hplmn = reg_sim_read_hplmn();
        
            if ( (sys_plmn_match(hplmn, reg_state_service_state.plmn)) &&
#else
            if ( (reg_sim_is_ehplmn(reg_state_service_state.plmn)) &&
#endif
                 (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
                 (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_GSM_BAND_PREF
                 (msg_p->band_pref       == reg_mode_band_pref_get()) &&
#endif
                 (msg_p->mode_pref       == reg_mode_mode_pref_get()) &&
#else
                 (reg_mode_rat_pri_list_order_match(&msg_p->rat_pri_list_info, reg_mode_rat_pri_list_get())) &&
                 (reg_mode_is_bst_set_for_rat(&msg_p->rat_pri_list_info,reg_state_service_state.active_rat))&&
#endif
                 (allowed_service_domain == reg_mode_req_service_domain_get()) &&
                 (allowed_service_domain == reg_state_service_state.reg_service_domain)
#ifdef FEATURE_LTE
                 && !(reg_state_mm_waiting_for_reg_req)
#endif
#ifdef FEATURE_DUAL_DATA
                 && (msg_p->is_volte_only_ps == prev_volte_only_ps)
#endif
                 )
            {
              MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL");
              if( (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
                   (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ))
              {
                reg_send_mmr_net_sel_mode_change_ind(msg_p->network_selection_mode);
              }
              reg_mode_network_selection_mode_set(msg_p->network_selection_mode);
  
              reg_send_cm_service_cnf
              (
                reg_state_cm_transaction_id,
                reg_state_service_state
              );

              /* Reg loop back for service request, sending cell capabilties again*/
              mm_get_cell_service_capabilties(&cell_capabilties
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                              ,(sys_modem_as_id_e_type)reg_as_id
#endif
                                             );
              reg_state_process_cell_service_ind(&cell_capabilties);

              reg_state_cm_initiated_registration = FALSE;
              next_state_p = &reg_state_idle;
            }
            else if(cm_service_req_same_as_last_req_with_call == TRUE)
            {
#ifdef FEATURE_RAT_PRIORITY_LIST
              reg_mode_bst_status_set(TRUE);
#endif
              reg_mode_call_for_bit_set(msg_p->cs_for_request, msg_p->ps_for_request);

              MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d, re-sending last reg request with cs_for_req/ps_for_req enabled",
                                  msg_p->parameters.manual.type);
              reg_send_last_mmr_reg_req_for_call
              (
                msg_p->rat_pri_list_info
              );
            }
            else
            {
              MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d",
                                               msg_p->parameters.manual.type);
#ifdef FEATURE_EQUIVALENT_HPLMN
              ehplmn_list = reg_sim_ehplmn_list_get();
#endif

              reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
              reg_mode_bst_status_set(TRUE);
#endif
              reg_mode_start_manual_plmn_selection
              (
                msg_p->network_selection_mode,
#ifndef FEATURE_EQUIVALENT_HPLMN
                hplmn,
#else
                ehplmn_list.plmn[0],
#endif
                msg_p->mode_pref,
                msg_p->cs_for_request,
                msg_p->ps_for_request,
                allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
                ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
               ,&msg_p->rat_pri_list_info
#endif
                ,msg_p->parameters.manual.type
#ifdef FEATURE_LTE 
                , msg_p->sms_only,
                  msg_p->additional_info
#endif
              );
              reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
            }
            break;

          default:
            MSG_HIGH_DS_1(REG_SUB, "=REG= CM_SERVICE_REQ - MANUAL type=%d",
                                           msg_p->parameters.manual.type);

            /*
            ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
            */
            if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
                (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
            {
              reg_send_mmr_plmn_search_abort_req();
            }

            reg_send_grat_scan_done_set(FALSE); 
#ifdef FEATURE_RAT_PRIORITY_LIST
            reg_mode_bst_status_set(TRUE);
#endif
            reg_mode_start_plmn_selection
            (
              SYS_NETWORK_SELECTION_MODE_MANUAL,
              msg_p->mode_pref,
              msg_p->cs_for_request,
              msg_p->ps_for_request,
              allowed_service_domain
#ifdef FEATURE_GSM_BAND_PREF
              ,msg_p->band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,&msg_p->rat_pri_list_info
#endif
              ,msg_p->parameters.manual.type
#ifdef FEATURE_LTE 
             , msg_p->sms_only,
               msg_p->additional_info
#endif
            );
            reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
            break;
        }
      }  
    }
  }
  else
  {
    reg_sim_state_print();

    switch (allowed_service_domain)
    {
      case SYS_SRV_DOMAIN_CS_ONLY:
        MSG_HIGH_DS_0(REG_SUB, "=REG= ONLY CS SERVICE ALLOWED");
        break;

      case SYS_SRV_DOMAIN_PS_ONLY:
        MSG_HIGH_DS_0(REG_SUB, "=REG= ONLY PS SERVICE ALLOWED");
        break;

      case SYS_SRV_DOMAIN_CS_PS:
        MSG_HIGH_DS_0(REG_SUB, "=REG= CS AND PS SERVICE ALLOWED");
        break;

      case SYS_SRV_DOMAIN_NO_SRV:
        MSG_HIGH_DS_0(REG_SUB, "=REG= NORMAL SERVICE NOT ALLOWED");
        break;

      default:
        MSG_HIGH_DS_0(REG_SUB, "=REG= NORMAL SERVICE NOT ALLOWED");
        break;
    }

    service_state.sim_state               = reg_sim_state_get();
    service_state.plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
    service_state.plmn_forbidden          = FALSE;
    service_state.roaming_ind             = SYS_ROAM_STATUS_OFF;
    service_state.service_status          = SYS_SRV_STATUS_NO_SRV;
    service_state.reg_service_domain      = SYS_SRV_DOMAIN_NO_SRV;
    service_state.active_rat              = SYS_RAT_UMTS_RADIO_ACCESS;
#ifdef FEATURE_GSM_BAND_PREF
    service_state.active_band.active_band = SYS_BAND_MASK_EMPTY;
    service_state.lac                     = 0xFFFE;
#endif
    service_state.ps_data_suspend         = FALSE;

    sys_plmn_undefine_plmn_id(&service_state.plmn);

    reg_send_cm_service_cnf
    (
      reg_state_cm_transaction_id,
      service_state
    );

    reg_state_cm_initiated_registration = FALSE;
    next_state_p = reg_state_curr_p;
  }

#ifdef FEATURE_LTE
  reg_state_mm_waiting_for_reg_req = FALSE;
#endif

#ifdef FEATURE_TDSCDMA
  if((msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
      (msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) &&
      (msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    reg_nv_reset_pm_chg_pending_plmn_rat();
  }
#endif 

  reg_state_ignore_rplmnact_set(FALSE);
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_sim_available_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;

#ifdef FEATURE_DUAL_SIM
  reg_sim_mode_pref = msg_p->mode_pref;
  reg_nv_context_id = msg_p->nv_context_id;

  rex_enter_crit_sect(&reg_sim_crit_sect);
  if((reg_sim_mmgsdi_info_sim[reg_as_id].session_type != msg_p->session_type) &&
      (reg_sim_session_matching_with_other_sub(msg_p->session_type)))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Swapping MMGSDI info");
    nas_swap_mmgsdi_info((mm_as_id_e_type)reg_as_id, reg_sim_mmgsdi_info_sim,reg_waiting_for_open_session_cnf_sim,msg_p->session_type);
  }
  /* Unexpected but try to recover */ 
  else if(reg_sim_mmgsdi_info_sim[reg_as_id].session_type != msg_p->session_type)
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Unexpected session type",msg_p->session_type);
    reg_sim_mmgsdi_info.session_type = msg_p->session_type;
    reg_sim_mmgsdi_info.session_id = 0;
    reg_sim_reset_card();
  }
  rex_leave_crit_sect(&reg_sim_crit_sect);

/*
  if(reg_nv_context_id != (byte)reg_as_id)
  {
    MSG_ERROR_DS_0( REG_SUB, "=REG= DS: NV Context ID (%d) swap failed!!!", reg_nv_context_id, 0,0 );
    reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id);
    return next_state_p;
  }
*/
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  MSG_HIGH_DS_1(REG_SUB, "=REG= Overall Feature: %d", reg_sub_config);
#endif

  if((reg_sim_mmgsdi_info.client_id == 0) || (reg_waiting_for_open_session_cnf))
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG= Waiting for MMGSDI open session response %d/client reg response, client:H:0x%x L:0x%x",
                 reg_waiting_for_open_session_cnf,(dword)(reg_sim_mmgsdi_info.client_id >> 32), 
                 (dword)(reg_sim_mmgsdi_info.client_id & 0xFFFFFFFF));
    reg_need_to_process_sim_avail_req = TRUE;
    reg_send_mmr_sim_available_req
    (
#ifdef FEATURE_DUAL_SIM
      reg_sim_mode_pref
      ,reg_sim_mmgsdi_info.client_id
      ,reg_sim_mmgsdi_info.session_type
      ,reg_nv_context_id
#else
      reg_sim_mmgsdi_info.client_id
#endif
    );
  }
  else if (reg_sim_mmgsdi_info.session_id == 0)
  {
    if (!reg_sim_open_session())
    {
      MSG_ERROR_DS_0( REG_SUB, "=REG= MMGSDI OPEN SESSION failed!!!");
      reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id);
    }
    else
    {
      reg_need_to_process_sim_avail_req = TRUE;
      reg_send_mmr_sim_available_req
      (
#ifdef FEATURE_DUAL_SIM
        reg_sim_mode_pref
        ,reg_sim_mmgsdi_info.client_id
        ,reg_sim_mmgsdi_info.session_type
        ,reg_nv_context_id
#else
        reg_sim_mmgsdi_info.client_id
#endif
      );
    }
  }
  else
  {
    if (reg_sim_load_card_mode())
    {
      /*
      ** Read the equivalent PLMN list from NVRAM.
      */
#ifdef FEATURE_FEMTO_CSG
      reg_state_power_on_csg_search = TRUE;
#endif 
      reg_mode_initialize_equivalent_plmn_list
      (
  #ifdef FEATURE_ENHANCED_NW_SELECTION
        FALSE
  #endif
      );
  
      reg_send_eq_plmn_change_ind();

      reg_mode_t3245_efnas_config_behavior();
#ifdef FEATURE_TDSCDMA
      reg_timers_stop_validate_sim_timer();
#endif
      /* Populate enhanced HPLMN search tbl */
      reg_mode_enhanced_hplmn_srch_init();

#ifdef FEATURE_LTE 
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
#endif
      reg_send_mmr_sim_available_req
      (
#ifdef FEATURE_DUAL_SIM
        reg_sim_mode_pref
        ,reg_sim_mmgsdi_info.client_id
        ,reg_sim_mmgsdi_info.session_type
        ,reg_nv_context_id
#else
        reg_sim_mmgsdi_info.client_id
#endif
      );
  
      next_state_p = &reg_state_registering;
    }
    else
    {
      reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id);
    }
  }

  if (mm_nv_nas_rel_compliance > NV_NAS_COMPLIANCE_REL6)
  {
    reg_state_is_pwr_on_manual_plmn_selection = TRUE;
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_sim_not_available_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
)
{
  (void) msg_p;

  reg_sim_reset_card();

  reg_send_mmr_sim_not_available_req();

  return &reg_state_deregistering;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_stop_mode_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_stop_mode_req
(
  const cm_stop_mode_req_s_type* msg_p
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;

  reg_last_mm_message_only_for_reg = FALSE;

  reg_mode_clear_plmn_backgnd_list();
  reg_mode_foreground_search_status      = REG_MODE_FOREGROUND_SEARCH_NONE;
  reg_state_is_roaming_plmn_to_be_camped = FALSE;
  
  reg_mode_clear_available_plmn_list();
  if((msg_p->stop_mode_reason != SYS_STOP_MODE_REASON_ALT_SCAN)
#ifdef FEATURE_DUAL_SIM
     && (msg_p->stop_mode_reason != SYS_STOP_MODE_REASON_DUAL_SWITCH)
#endif
#ifdef FEATURE_SGLTE
     && (msg_p->stop_mode_reason != SYS_STOP_MODE_REASON_SUSPEND_STACK)
#endif
     )
  {
    reg_timers_stop_hplmn_search_period_timer();    
    reg_timers_stop_all_irat_search_period_timer();
  }

  reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
  reg_state_reset_lte_cell_info();
#endif

#ifdef FEATURE_SGLTE_DUAL_SIM
  if(REG_SUB_IS_SGLTE)
  {
    reg_state_trm_failure_sglte_main = FALSE;
  }
#endif 
  /*
  ** ABORT ongoing manual search if a CM_STOP_MODE_REQ is received.
  */
  if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
      (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
  {
    reg_send_mmr_plmn_search_abort_req();
  }
  
  reg_state_last_stop_mode_reason = msg_p->stop_mode_reason;
  
  switch (msg_p->stop_mode_reason)
  {
    case SYS_STOP_MODE_REASON_IRAT:
#ifdef FEATURE_LTE
    case SYS_STOP_MODE_REASON_LOCAL_DETACH:
#endif
      next_state_p = &reg_state_inactive;
      reg_state_is_pwr_on_manual_plmn_selection = FALSE;
      break;
    case SYS_STOP_MODE_REASON_NO_SYS:
    case SYS_STOP_MODE_REASON_MODE_CHANGE:
#ifdef FEATURE_DUAL_SIM
    case SYS_STOP_MODE_REASON_MODE_CAP_CHANGE:
#endif
      next_state_p = &reg_state_inactive;
      reg_state_is_pwr_on_manual_plmn_selection = FALSE;
      reg_state_power_up_system_selection = TRUE;
#ifdef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_DUAL_SIM
      if(msg_p->stop_mode_reason != SYS_STOP_MODE_REASON_MODE_CAP_CHANGE)
#endif
      {
        reg_mode_rat_pri_list_reset();
      }
#endif
#ifdef FEATURE_LTE
      reg_sim_clear_backoff_fplmn_list();
      reg_timers_stop_update_lte_cap_timer();
      if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
      }
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif
#ifdef FEATURE_DUAL_SIM
      if(msg_p->stop_mode_reason != SYS_STOP_MODE_REASON_MODE_CAP_CHANGE)
      {
        reg_state_dds_switch_in_progress = FALSE;
      }
	  reg_multimode_sub = REG_AS_ID_1;
#endif
      break;
    case SYS_STOP_MODE_REASON_MODE_CHANGE_FTM:
      next_state_p = &reg_state_inactive;
      reg_state_is_pwr_on_manual_plmn_selection = FALSE;
#ifdef FEATURE_GSTK
      reg_state_set_gstk_event_flag ( TRUE);
      reg_state_reset_last_gstk_event_sent();
      reg_state_reset_last_gstk_rat_sent();
#endif
      break;
    case SYS_STOP_MODE_REASON_DEEP_SLEEP: 
    next_state_p = &reg_state_inactive;
#ifdef FEATURE_GSTK
      reg_state_set_gstk_event_flag (TRUE);
      reg_state_reset_last_gstk_event_sent();
      reg_state_reset_last_gstk_rat_sent();     
#endif
      break;
    case SYS_STOP_MODE_REASON_ALT_SCAN:
#ifdef FEATURE_DUAL_SIM
    case SYS_STOP_MODE_REASON_DSDS_SUSPEND: 
    case SYS_STOP_MODE_REASON_DUAL_SWITCH:
#endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
    case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
    case SYS_STOP_MODE_REASON_SUSPEND_STACK :
#endif
      next_state_p = &reg_state_inactive;
      break;

    case SYS_STOP_MODE_REASON_OFFLINE:
      next_state_p = &reg_state_offline;
      reg_state_is_pwr_on_manual_plmn_selection = FALSE;
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_rat_pri_list_reset();
#endif
#ifdef FEATURE_LTE
      reg_sim_clear_backoff_fplmn_list();
      reg_timers_stop_update_lte_cap_timer();
      if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
      }

      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif
#ifdef FEATURE_GSTK
      reg_state_set_gstk_event_flag (TRUE);
      reg_state_reset_last_gstk_event_sent();
      reg_state_reset_last_gstk_rat_sent();     
#endif
#ifdef FEATURE_DUAL_SIM
      reg_state_dds_switch_in_progress = FALSE;
      reg_multimode_sub = REG_AS_ID_1;
#endif
      break;

    case SYS_STOP_MODE_REASON_POWER_OFF:
    case SYS_STOP_MODE_REASON_LPM:
    default:
      next_state_p = &reg_state_deregistering;
      reg_state_is_pwr_on_manual_plmn_selection = FALSE;
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_rat_pri_list_reset();
#endif
#ifdef FEATURE_DUAL_SIM
      reg_state_dds_switch_in_progress = FALSE;
      reg_multimode_sub = REG_AS_ID_1;
#endif
#ifdef FEATURE_LTE
      reg_sim_clear_backoff_fplmn_list();
      reg_timers_stop_update_lte_cap_timer();
      if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
      }
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif
      reg_state_dump_the_list(reg_state_lte_available_list_ptr);
      reg_state_dump_the_list(reg_state_lte_non_available_list_ptr);


#ifdef FEATURE_GSTK
      reg_state_set_gstk_event_flag (TRUE);
      reg_state_reset_last_gstk_event_sent();
      reg_state_reset_last_gstk_rat_sent();    
#endif
#ifdef FEATURE_FEMTO_CSG
      reg_state_power_on_csg_search = TRUE;
#endif 

      reg_state_power_up_system_selection = TRUE;
#ifdef FEATURE_SGLTE
      if(REG_SUB_IS_SGLTE)
      {
        reg_state_power_up_system_selection_sim[REG_SGLTE_STACK_2] = TRUE;
      }
#endif 
      break;
  }
#ifdef FEATURE_SGLTE
  if(REG_SUB_IS_SGLTE && 
     (reg_sglte_mode) && (reg_as_id == reg_ps_stack_id))
  {
    reg_timers_stop_sglte_ps_timer();
  }
#endif
  reg_send_mmr_stop_mode_req
  (
    msg_p->stop_mode_reason
  );

  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_ph_status_chgd_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_ph_status_chgd_req
(
  const cm_ph_status_chgd_req_s_type* msg_p
)
{
  sys_stop_mode_reason_e_type stop_mode_reason = SYS_STOP_MODE_REASON_NONE;
  reg_state_p_type next_state_p = reg_state_curr_p;


  switch(msg_p->oprt_mode)
  {
    case SYS_OPRT_MODE_RESET:
    case SYS_OPRT_MODE_PWROFF:
      reg_sim_reset_card();
      reg_nv_write_cache();
#ifdef FEATURE_LTE
      if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
       }
#endif

      reg_nv_write_lte_lists_to_efs();
      reg_state_clear_lte_lists();
      reg_timers_stop_backoff_lte_search_period_timer();
 


#ifdef FEATURE_DUAL_SIM
      if(reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_as_id))
#else
      if(reg_is_t3245_timer_active())
#endif
      {
        reg_nv_write_t3245_to_efs(reg_timers_get_rem_t3245_value());
        reg_timers_stop_t3245();
      }
#ifdef FEATURE_TDSCDMA
      reg_timers_stop_validate_sim_timer();
      reg_mode_reset_validate_sim_counter();
#endif
      stop_mode_reason = SYS_STOP_MODE_REASON_POWER_OFF;
      next_state_p = &reg_state_inactive;
      reg_send_mmr_ph_status_chng_req(stop_mode_reason);
      break;

    case SYS_OPRT_MODE_LPM:
      if((!reg_nv_is_imsi_switch_enabled()) &&
         (reg_sim_state_get() != SYS_SIM_STATE_NOT_AVAILABLE)
#ifdef FEATURE_ENHANCED_NW_SELECTION
         && (!reg_sim_read_ens_flag()) 
#endif
        )
      {
        reg_send_mmr_gstk_service( FALSE);
      }
      reg_sim_reset_card();
#ifdef FEATURE_LTE
      if( ds3gpp_is_plmn_blocked_force_lte_pref((sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
      }
      reg_sim_clear_backoff_fplmn_list();
      reg_timers_stop_update_lte_cap_timer();
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif
      if(reg_nv_is_lpm_power_off_enabled())
      {
        reg_nv_write_cache();
#ifdef FEATURE_DUAL_SIM
        if(reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_as_id))
#else
        if(reg_is_t3245_timer_active())
#endif
        {
          reg_nv_write_t3245_to_efs(reg_timers_get_rem_t3245_value());
        }

        reg_nv_write_lte_lists_to_efs();


        stop_mode_reason = SYS_STOP_MODE_REASON_LPM_POWER_OFF;
        reg_send_mmr_ph_status_chng_req(stop_mode_reason);
      }
      else
      {
        reg_send_cm_ph_status_chgd_cnf
        (
          reg_state_cm_transaction_id_get(),
          SYS_PH_STATUS_CHGD_SUCCESS
        );
      }
      break;
    case SYS_OPRT_MODE_OFFLINE:
      /*Fall-Through*/
      reg_sim_reset_card();
    default:
      reg_send_cm_ph_status_chgd_cnf
      (
        reg_state_cm_transaction_id_get(),
        SYS_PH_STATUS_CHGD_SUCCESS
      );
      break;
  }

  return next_state_p;
}


#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_state_update_service_state_if_required

==============================================================================*/



void reg_state_update_service_state_if_required(reg_state_p_type next_state_p )
{
  if((reg_mode_is_only_req_rat_present_in_rat_pri_list(reg_mode_curr_rat_pri_list_get(),SYS_SYS_MODE_LTE) == TRUE )&&
       (next_state_p == &reg_state_registering) &&
      (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION || 
       reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  { 
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    if (reg_state_cm_initiated_registration == TRUE)
    {
      reg_state_cm_initiated_registration =FALSE;
      reg_send_cm_service_cnf
      (
        reg_state_cm_transaction_id,
        reg_state_service_state
      );
     }
     else
     {
       reg_send_cm_service_ind(reg_state_service_state);
     }
   }
}

#endif


/*==============================================================================

FUNCTION NAME

  reg_state_is_sim_invalidated

==============================================================================*/

boolean reg_state_is_sim_invalidated
(
  sys_sim_state_e_type previous_sim_state,
  sys_sim_state_e_type new_sim_state
)
{
  if((previous_sim_state == SYS_SIM_STATE_AVAILABLE) &&
     ((new_sim_state == SYS_SIM_STATE_CS_INVALID)||
      (new_sim_state == SYS_SIM_STATE_PS_INVALID) ||
      (new_sim_state == SYS_SIM_STATE_CS_PS_INVALID)))
  {
    return TRUE;
  }
  return FALSE;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_reg_cnf

==============================================================================*/

reg_state_p_type reg_state_process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
)
{
#if defined(FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT) || !defined(FEATURE_EQUIVALENT_HPLMN)
  sys_plmn_id_s_type hplmn;
#endif
  reg_state_p_type   state_p;
  sys_srv_domain_e_type        allowed_service_domain;
  sys_detailed_plmn_list_info_s_type *managed_roaming_plmn;
  const reg_sim_preferred_plmn_list_s_type* low_priority_list;
  boolean            finished = TRUE;
  boolean            attempting_registration = FALSE;
  uint32               i;
  sys_detailed_plmn_list_s_type      *available_plmn_list = NULL;
  const sys_detailed_plmn_list_s_type      *plmn_list = (reg_mode_available_plmn_list_get());
#ifdef FEATURE_TDSCDMA
  nv_rplmnact_type rplmnact;
#endif
#ifdef FEATURE_GSTK
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  last_gstk_sent_plmn_mcc;
  uint32  last_gstk_sent_plmn_mnc;
  uint32  camped_plmn_mcc, camped_plmn_mnc;
  sys_srv_status_e_type current_service_status;
  sys_srv_status_e_type previous_service_status;
#endif
  
 mmr_cause_e_type local_cause = MMR_CAUSE_NONE; 
#ifdef FEATURE_SGLTE
  reg_state_p_type   state_local;
#endif
#ifdef FEATURE_EOOS
  #error code not present
#endif /*FEATURE_EOOS*/
#ifdef FEATURE_LTE
  boolean update_mm_for_lte_cap_change = FALSE;
#endif
  sys_sim_state_e_type previous_sim_state = reg_sim_state_get();
  int32 file_size = 0;

  reg_nv_imp_variable_prints();
  reg_sim_imp_variable_prints();

  if(reg_state_is_sim_invalidated(previous_sim_state,
                                  msg_p->service_state.sim_state))
  {
    if(reg_mode_revalidate_sim_allowed(msg_p->service_state.plmn))
    {
      reg_timers_start_validate_sim_timer();
      reg_mode_increment_validate_sim_counter();
#ifdef FEATURE_SGLTE
      if(REG_SGLTE_DUAL_STACK)
      {
         sys_sim_state_e_type reg_sim_state_cs = SYS_SIM_STATE_NONE, reg_sim_state_ps = SYS_SIM_STATE_NONE;
         if(reg_as_id == reg_ps_stack_id)
         {
           reg_sim_state_ps = msg_p->service_state.sim_state;
           reg_as_id = reg_cs_stack_id;
           reg_sim_state_cs = reg_sim_state_get();
           reg_as_id = reg_ps_stack_id;
         }
         else if(reg_as_id == reg_cs_stack_id)
         {
           reg_sim_state_cs = msg_p->service_state.sim_state;
           reg_as_id = reg_ps_stack_id;
           reg_sim_state_ps = reg_sim_state_get();
           reg_as_id = reg_cs_stack_id;
         }
         if((reg_sim_state_ps == SYS_SIM_STATE_PS_INVALID  &&
             reg_sim_state_cs == SYS_SIM_STATE_CS_INVALID ))
         {
           (void) mmgsdi_session_manage_illegal_subscription(reg_sim_mmgsdi_info.session_id,
                                                              MMGSDI_SESSION_APP_IS_ILLEGAL);
           MSG_HIGH_DS_2(REG_SUB, "=REG= SIM is marked as illegal and same has been notified to MSGSDI sim state PS = %d SIM state CS = %d",reg_sim_state_ps,reg_sim_state_cs);
         }
      }
#endif
    }
    else
#ifdef FEATURE_DUAL_SIM
    if((reg_sim_check_if_t3245_behavior_enabled((sys_modem_as_id_e_type)reg_sub_id) == TRUE) &&
       (reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_sub_id) == FALSE))
    {
      reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE,(sys_modem_as_id_e_type)reg_sub_id);
    }
#else
    if((reg_sim_check_if_t3245_behavior_enabled() == TRUE) &&
       (reg_is_t3245_timer_active() == FALSE))
    {
      reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE);
    }
#endif

  }
#if defined(FEATURE_LTE) && defined(FEATURE_RAT_PRIORITY_LIST)  
  if(msg_p->service_state.sim_state == SYS_SIM_STATE_PS_INVALID && 
     reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED &&
     reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS,reg_mode_rat_pri_list_get(), FALSE)
     )
  {
    if(!((reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
          (reg_bit_mask_check(reg_mode_additional_info_get(),
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)))
    {
      reg_mode_remove_rat_from_rat_pri_list(reg_mode_rat_pri_list_get(), SYS_SYS_MODE_LTE);
      reg_mode_filter_available_plmn_list_rat(SYS_RAT_LTE_RADIO_ACCESS);
    }
  }
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
#ifdef FEATURE_SGLTE_DUAL_SIM
  if(REG_SGLTE_DUAL_STACK && (reg_as_id == reg_ps_stack_id) && 
     (msg_p->service_state.acq_status == SYS_ACQ_STATUS_NO_RESOURCE) &&
     (msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV))
  {
    reg_state_trm_failure_sglte_main = TRUE;
  }
  else if(REG_SUB_IS_SGLTE && (reg_as_id == reg_ps_stack_id))
  {
    reg_state_trm_failure_sglte_main = FALSE;

    /* Reg sent timed service req, but received no srv with resource */
    if(REG_SGLTE_DUAL_STACK &&
       ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
       (reg_state_service_state_sim[reg_cs_stack_id].service_status == SYS_SRV_STATUS_SRV) &&
       (reg_state_service_state_sim[reg_cs_stack_id].active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
       (reg_state_service_state_sim[reg_cs_stack_id].reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
       (reg_state_service_state_sim[reg_cs_stack_id].plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE) &&
       (msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
       (!reg_is_timer_active(REG_SGLTE_PS_TIMER)))
    {
      reg_timers_start_sglte_ps_timer();
    }
  }
#endif 
  if ((msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
       (msg_p->service_state.acq_status == SYS_ACQ_STATUS_NO_RESOURCE) &&
       (msg_p->ue_oos == TRUE) &&
       ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)))
  {
#ifdef FEATURE_SGLTE_DUAL_SIM
    if((REG_SGLTE_DUAL_STACK) && (reg_as_id == reg_ps_stack_id) &&
       (mm_get_verify_ps_transfer_conditions((sys_modem_as_id_e_type)reg_cs_stack_id)) &&
       (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs((sys_modem_as_id_e_type)reg_cs_stack_id),
                                              (sys_modem_as_id_e_type)reg_sub_id) == FALSE ))
    {
      reg_timers_stop_sglte_ps_timer();
      reg_state_sglte_merge_in_progress = TRUE;
      reg_send_ps_timer_expiry();
      return &reg_state_idle;
    }
    else
#endif
    if((reg_mode_trm_timeout_value_get(msg_p->service_state.active_rat) == REG_TIMERS_MAX_TIMER_VALUE)
#ifdef FEATURE_SGLTE_DUAL_SIM
        ||
        (REG_SGLTE_DUAL_STACK)
#endif
       )
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= Resending service request on TRM failure");
      reg_send_last_mmr_reg_req();
      return &reg_state_registering;
    }
  }
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
  /*
  ** If FCN - Acting HPLMN is delayed, i.e. PLMN rescan should be done.
  ** Ignore MMR_REG_CNF if PLMN rescan is started.
  */
  if (reg_state_delay_fcn_act_hplmn == TRUE)
  {
    reg_state_delay_fcn_act_hplmn = FALSE;
    if (reg_mode_restart_plmn_selection() == TRUE)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_REG_CNF - ACT HPLMN Changed");
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      return &reg_state_registering;
    }
  }
#endif
  /*
  ** Make sure that MM is not improperly reporting that the SIM is not
  ** available.  REG only expects to be informed that the SIM is not
  ** available upon receiving a CM_SIM_NOT_AVAILABLE_REQ.
  */
  if (msg_p->service_state.sim_state != SYS_SIM_STATE_NOT_AVAILABLE)
  {
    reg_sim_state_set(msg_p->service_state.sim_state);
  }

#ifdef FEATURE_GSTK
   sys_plmn_get_mcc_mnc
  (
    reg_sate_last_gstk_event_sent_plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &last_gstk_sent_plmn_mcc,
    &last_gstk_sent_plmn_mnc
  );

  sys_plmn_get_mcc_mnc
  (
    msg_p->service_state.plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &camped_plmn_mcc,
    &camped_plmn_mnc
  );

  if(reg_nv_is_imsi_switch_enabled() )
  {
    if( !sys_plmn_id_is_undefined( msg_p->service_state.plmn) &&
        !sys_plmn_id_is_undefined(reg_sate_last_gstk_event_sent_plmn) &&
        !sys_mcc_same_country(last_gstk_sent_plmn_mcc, camped_plmn_mcc))
    {
      reg_need_to_send_gstk_event = TRUE;
      MSG_HIGH_DS_1(REG_SUB, "=REG= reg_need_to_send_gstk_event %d", reg_need_to_send_gstk_event);
    }
  }
#endif

  if(msg_p->service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS || 
     msg_p->service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS || 
     msg_p->service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)  
  {
    
    if(reg_state_cell_id_changed(msg_p->service_state,msg_p->cell_id))
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Reset the retry counter");
      reg_mode_reset_retry_counter();
    }
    reg_state_cell_id = msg_p->cell_id;
  }
  else
  {
    reg_state_cell_id = 0xFFFF;                         
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : REG Cell Id =%d reg_state_hplmn_irat_timer_count = %d ",reg_state_cell_id, reg_state_hplmn_irat_timer_count);                                  
  reg_state_service_state = msg_p->service_state;
  reg_state_service_state.sim_state = reg_sim_state_get();

#ifdef FEATURE_SGLTE
  if(REG_SGLTE_DUAL_STACK && reg_state_rlf_started == TRUE &&
    (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))
  {
    reg_state_rlf_started = FALSE;
  }
#endif


  if(msg_p->service_state.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    reg_mode_scan_scope_set(SYS_SCAN_SCOPE_FULL_BAND);             
  }

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
 // If no service is due to RLF set rlf plmn and rat
  if(reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV &&
       reg_state_service_state.no_service_cause == SYS_NO_SVC_CAUSE_RLF)
  {
     reg_state_rlf_plmn = reg_state_service_state.plmn;
     sys_plmn_undefine_plmn_id(&reg_state_service_state.plmn);   
     reg_state_rlf_rat  = reg_state_service_state.active_rat;
  }
#endif
#if defined(FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT) || !defined(FEATURE_EQUIVALENT_HPLMN)
  hplmn = reg_sim_read_hplmn();
#endif

  if(((msg_p->ue_oos == FALSE)
#ifdef FEATURE_LTE
      &&
      (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED ||
       msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
#endif
       ) ||
     (msg_p->available_plmn_list.length != 0) ||
     (msg_p->service_state.service_status != SYS_SRV_STATUS_NO_SRV))
  {
    reg_mode_update_service_available_rat(msg_p->service_state.active_rat);
  }

#ifdef FEATURE_EOOS    
  #error code not present
#endif /*FEATURE_EOOS*/
    /*
  ** MMR_REG_CNF is received on a PLMN which is not requested or that is not in equivalent list,
  ** Then REG is expecting that MM is already processing for the last requested PLMN and
  ** shall be sending the another MMR_REG_CNF on that PLMN. So ignoring this MMR_REG_CNF.
  ** If MM rejects the REG_REQ in Connected state, then accept the MMR_REG_CNF.
  */
  if (msg_p->cause == MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN)
  {
    local_cause  = MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN;
    msg_p->cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  }
  else if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
           (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) &&
          !(sys_plmn_match(reg_mode_req_plmn_get(), reg_state_service_state.plmn) ||
            (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED)||
            (msg_p->cause == MMR_CAUSE_REQ_CAMPED_PLMN_MISMATCH) ||
            (sys_eplmn_list_equivalent_plmn(reg_state_service_state.plmn))) &&
           !(reg_mode_get_hplmn_to_selected(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                               (sys_modem_as_id_e_type)reg_as_id
#endif
                                        ) &&
#ifdef FEATURE_EQUIVALENT_HPLMN
              reg_sim_is_ehplmn(reg_state_service_state.plmn)
#else
              sys_plmn_match(reg_sim_read_hplmn(), reg_state_service_state.plmn) 
#endif       
            ))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_REG_CNF - unexpected PLMN");
    return reg_state_curr_p;
  }
#ifdef FEATURE_SGLTE
  else if ( REG_SUB_IS_SGLTE &&  
            reg_sglte_mode && 
           /* split is in progress */
           (IS_SGLTE_SPLIT_IN_PROGRESS || 
           /* merge is in progress at lower layers */
           (REG_SGLTE_DUAL_STACK && MM_IS_SGLTE_ON_PS_STACK_ID && msg_p->service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS)) &&  
           (reg_mode_req_rat_get() != reg_state_service_state.active_rat))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_REG_CNF - since Split/Merge is in progress");
    return reg_state_curr_p;
  }
#endif
  if(msg_p->cause == MMR_CAUSE_REQ_CAMPED_PLMN_MISMATCH)
  {
    msg_p->cause = MMR_CAUSE_NOT_SPECIFIED;
  }
  if ((reg_sim_is_hplmn_to_be_selected()) &&
     (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))
  {
    reg_mode_set_hplmn_to_be_selected(FALSE);
  }

#ifdef FEATURE_TDSCDMA
  /* If MMR_REG_CNF has been received with coverage on WCDMA\TDSCDMA, set 
  ** the TDSCDMA bit in the NV_RPLMNAcT accordingly
  */
  if(reg_mode_get_3GPP_coverage(msg_p->available_plmn_list, SYS_RAT_TDS_RADIO_ACCESS))
  {
      reg_nv_read_rplmnact(&rplmnact);
      rplmnact.act[0] |= TDSCDMA_ACT_BIT;
      reg_nv_write_rplmnact(rplmnact);     
  }
  else if(reg_mode_get_3GPP_coverage(msg_p->available_plmn_list, SYS_RAT_UMTS_RADIO_ACCESS))
  {
      reg_nv_read_rplmnact(&rplmnact);
      rplmnact.act[0] &= ~TDSCDMA_ACT_BIT;
      reg_nv_write_rplmnact(rplmnact);     
  }
#endif

  if ((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) && 
     (msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS))
  {
    /* If forced IRAT is enabled, stop the relevant timers*/
       reg_mode_rat_search_stop(msg_p->service_state.active_rat);
  }

  file_size = sizeof(uint32)+(plmn_list->length*sizeof(sys_detailed_plmn_list_info_s_type));
#ifdef FEATURE_MODEM_HEAP
  available_plmn_list = (sys_detailed_plmn_list_s_type *)modem_mem_alloc(file_size, MODEM_MEM_CLIENT_NAS);
#else
  available_plmn_list = (sys_detailed_plmn_list_s_type *)gs_alloc(file_size);
#endif
  reg_check_for_null_ptr((void*)available_plmn_list );
  memscpy(available_plmn_list,file_size,plmn_list,file_size);
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
  if(!reg_state_camped_ind_received && (msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
      (msg_p->service_state.acq_status == SYS_ACQ_STATUS_NO_RESOURCE))
  {
    reg_mode_service_req_aborted_set(TRUE);
  }
#endif 
  if ((msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) && !reg_state_camped_ind_received
#ifdef FEATURE_FEMTO_CSG
         && (reg_mode_found_attempted_plmn_rat(reg_mode_req_plmn_get(), reg_mode_req_rat_get()))
#endif 
   ) 
  {
    for ( i= 0; i < available_plmn_list->length; i++)
    {
      if ((sys_plmn_match(reg_mode_req_plmn_get(), available_plmn_list->info[i].plmn)) &&
          (available_plmn_list->info[i].rat == reg_mode_req_rat_get()) &&
          (available_plmn_list->info[i].signal_strength == 0))
      {
        reg_mode_remove_from_attempted_plmn_list(reg_mode_req_plmn_get(), reg_mode_req_rat_get()); 
        reg_mode_remove_from_available_plmn_list(reg_mode_req_plmn_get(), reg_mode_req_rat_get()); 
        break;
      }
    }
  }

  /*
  ** Set the roaming indication.
  */
  if ( !sys_plmn_id_is_undefined(msg_p->service_state.plmn) )
  {
#ifndef FEATURE_EQUIVALENT_HPLMN
    if ( sys_plmn_match(hplmn, msg_p->service_state.plmn) )
#else
    if ( reg_sim_is_ehplmn(msg_p->service_state.plmn) ) 
#endif
    {
      reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_OFF;
    }
    else
    {
      reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_ON;
    }
  }

  /*
  ** Combine attach rejected with cause #14, MM trying for LU.
  ** So add remain on PLMN.
  */
  if (msg_p->cause == MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN)
  {
    msg_p->cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  }

  /*
  ** Make sure the service status indicates no service so MMoC/SD 
  ** will initiate PLMN selection.
  */
  if (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED)
  {
    if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
    }
    else if ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    }
  }
  else if ((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
           ((msg_p->cause == MMR_CAUSE_NOT_SPECIFIED) || 
            (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES) || 
            (msg_p->cause == MMR_CAUSE_FAILURE_MAX_ATTEMPTED)))  
  {
    if (reg_sim_is_rplmn(&(msg_p->service_state)))
    {
#ifndef FEATURE_FEMTO_CSG
      reg_sim_write_rplmn_rat
      (
        reg_state_service_state.active_rat
      );
#ifdef FEATURE_NAS_ECALL
      reg_nv_write_rplmn(reg_state_service_state.plmn);
      reg_nv_write_rplmn_dup(FALSE);
#endif 
#else
      if (reg_send_nw_sel_mode_sent_to_mm_get() != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
      {
        reg_sim_write_rplmn_rat
        (
          reg_state_service_state.active_rat
        );
        reg_nv_write_rplmn(reg_state_service_state.plmn);
        reg_nv_write_rplmn_dup(FALSE);
      }
      else
      {
        reg_nv_write_rplmn_dup(TRUE);
      }
#endif 
    }

    if((reg_is_qrd_device == TRUE) && 
          reg_sim_is_plmn_not_preferred_plmn(reg_state_service_state.plmn,reg_state_service_state.active_rat))
    {
       MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN is not higher priority PLMN");
       (void)reg_mode_update_vplmn_plmn_rat(reg_state_service_state.plmn,reg_state_service_state.active_rat);  
  }
  }
  
  /*
  ** Do not continue searching for regular service if the SIM has been invalidated.
  */
  if ((reg_mode_req_service_domain_get() == SYS_SRV_DOMAIN_NO_SRV) &&
      (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
      (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
  {
    if(msg_p->service_state.service_status != SYS_SRV_STATUS_NO_SRV)
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
    }
    finished = TRUE;
  }
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
  /* If RLF happened during service request procedure send service confirm to CM so that
       CM can start RLF */
  else if (msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV &&
                msg_p->service_state.no_service_cause == SYS_NO_SVC_CAUSE_RLF)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= RLF received in MMR_REG_CNF");
    finished = TRUE;
  }
#endif
  else
  {
 
    /*
    ** Save the equivalent PLMN list if required.
    */
    if (msg_p->update_equivalent_plmn_list)
    {

      boolean list_matched = reg_mode_save_equivalent_plmn_list
                             (
                               msg_p->service_state.plmn,
                               &(msg_p->equivalent_plmn_list)
                             );
      boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
      if ((!list_matched) || eplmn_list_validated)
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
        reg_send_eq_plmn_change_ind();
      }
    
    }  
    switch (msg_p->cause)
    {
      case MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED:
#ifdef FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT
        if ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
#ifndef FEATURE_EQUIVALENT_HPLMN
            (sys_plmn_match(hplmn, msg_p->service_state.plmn)) &&
#else
            (reg_sim_is_ehplmn(msg_p->service_state.plmn)) &&
#endif
            (sys_plmn_match(hplmn, reg_sim_read_rplmn(reg_mode_req_service_domain_get()))))
        {
          reg_send_last_mmr_reg_req_invalid_plmn();
          finished = FALSE;
        }
        else
#endif /* FEATURE_HPLMN_NOT_ALLOWED_SELECT_NEXT */
#ifndef FEATURE_LTE
#ifndef FEATURE_EQUIVALENT_HPLMN
        if ((sys_plmn_match(hplmn, msg_p->service_state.plmn)) &&
#else
        if ((reg_sim_is_ehplmn(msg_p->service_state.plmn)) &&
#endif
            (((reg_mode_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
              (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)) &&
              (reg_state_service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)))
        {
          reg_send_last_mmr_reg_req_gsm_only();

          finished = FALSE;

          reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
        else
#endif /*FEATURE_LTE*/
        {
          if((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
                sys_plmn_match(reg_mode_req_plmn_get(), reg_sim_read_rplmn(reg_mode_req_service_domain_get())))
          {
            reg_state_user_resel_plmn = reg_state_service_state.plmn;
            reg_state_user_resel_plmn_rat = reg_state_service_state.active_rat;
            reg_mode_start_foreground_plmn_search();
            finished = FALSE;
            reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
          }
          else
          {
            finished = reg_mode_finished
            (
              reg_state_service_state,
              &(msg_p->available_plmn_list)
            );
          }
        }
        break;

      case MMR_CAUSE_FAILURE_REMAIN_ON_PLMN:
 #ifdef FEATURE_FEMTO_CSG
       if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
       {
         reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
       }
       else
 #endif
        if ((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
            (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))
        {
          reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
        }

        if ((reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION) &&
            (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL))
        {
          reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
        }  

        /*
        **  Start HPLMN timer, so that higher priority PLMN search will happen
        **  when MS is in Limited Service (No suitable PLMN is found).
        */
        if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
             (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON))
        {
          reg_timers_start_hplmn_search_period_timer();
        }
        else
        {
            MSG_HIGH_DS_0(REG_SUB, "=REG= reg_mode_rat_search_start");
            reg_mode_rat_search_start();
        }

        finished = TRUE;
        break;

      case MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED:
#ifdef FEATURE_FEMTO_CSG 
        if(reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_CSG_SELECTION_APP &&
           (reg_state_cm_initiated_registration == TRUE))
        {
           reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
           finished = reg_mode_finished
          (
            reg_state_service_state,
            &(msg_p->available_plmn_list)
          );
        }
        else
#endif 
        {
          finished = TRUE;
  
          reg_mode_set_hplmn_to_be_selected(FALSE);
          if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
          {
            reg_send_last_mmr_reg_req();
  
            attempting_registration = TRUE;
            reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
          }
          else if ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                   (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
          {
            reg_send_last_mmr_reg_req_new_type(MMR_REG_REQ_NORMAL);
            finished = FALSE;
            reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
          }     
        }      
        break;

    case MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION:
#ifdef FEATURE_FEMTO_CSG 
        if(reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_CSG_SELECTION_APP &&
           (reg_state_cm_initiated_registration == TRUE))
        {
           reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
           finished = reg_mode_finished
          (
            reg_state_service_state,
            &(msg_p->available_plmn_list)
          );
        }
        else
#endif 
        {
          finished = TRUE; 
          reg_mode_set_hplmn_to_be_selected(FALSE);
          if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
          {
            reg_send_grat_scan_done_set(FALSE); 
          
            reg_mode_start_automatic_plmn_selection
            (
              SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
              REG_MODE_AUTOMATIC_MODE_NO_PREFERENCE,
              reg_mode_req_mode_pref_get(),
              reg_mode_cs_for_request_get(),
              reg_mode_ps_for_request_get(),
              reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get(),
              0
#endif 
            );

#ifdef FEATURE_ENHANCED_NW_SELECTION
            if(reg_sim_read_ens_flag())
            {
              attempting_registration = TRUE;
              reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
            }
            else
#endif
            {
              finished = FALSE;
            }
          }
          else if(reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
          {
            reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED; 
          }
        }
        break;
      case MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH:
#ifdef FEATURE_MODEM_HEAP
        managed_roaming_plmn = (sys_detailed_plmn_list_info_s_type *)modem_mem_alloc(sizeof(sys_detailed_plmn_list_info_s_type), MODEM_MEM_CLIENT_NAS);
#else
        managed_roaming_plmn = (sys_detailed_plmn_list_info_s_type *)gs_alloc(sizeof(sys_detailed_plmn_list_info_s_type));
#endif
        reg_check_for_null_ptr((void*)managed_roaming_plmn );
        sys_plmn_undefine_plmn_id(&managed_roaming_plmn->plmn); 
        reg_mode_set_hplmn_to_be_selected(FALSE);
        if ( mm_managed_roaming_retry_lu_in_manual_mode && mm_managed_roaming_enabled &&
             (( reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ) ||
              ( reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )))
        {
           low_priority_list = reg_sim_low_priority_plmn_list_get();
           for(i = 0; i<low_priority_list->length; i++)
           {
             if(sys_plmn_match(low_priority_list->info[i].plmn, reg_state_service_state.plmn) &&
                low_priority_list->info[i].rat == reg_state_service_state.active_rat)
             {
               managed_roaming_plmn->plmn = reg_state_service_state.plmn;
               managed_roaming_plmn->rat = reg_state_service_state.active_rat;
               break;
            }
          }
        }
        if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
           reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
        {
          reg_state_user_resel_plmn = reg_state_service_state.plmn;
          reg_state_user_resel_plmn_rat = reg_state_service_state.active_rat;
          reg_mode_start_foreground_plmn_search();
          finished = FALSE;
          reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
        else if(( reg_state_service_state.cs_service_status == SYS_SRV_STATUS_LIMITED ) && 
                ( !sys_plmn_id_is_undefined( managed_roaming_plmn->plmn )))
        {
           allowed_service_domain = reg_mode_allowed_service_domain
           (
             managed_roaming_plmn->plmn,                                 /* plmn                   */
             reg_mode_req_service_domain_get()     /* req_service_domain     */
           );
           
           reg_send_mmr_reg_req
           (
             reg_mode_network_selection_mode_get(), /* network_selection_mode */
             managed_roaming_plmn->plmn,                                 /* plmn                   */
             allowed_service_domain,               /* req_service_domain     */
             reg_mode_mode_pref_get(),             /* mode_pref              */
             reg_mode_cs_for_request_get(),        /* cs_for_request         */
             reg_mode_ps_for_request_get(),        /* ps_for_request         */
             managed_roaming_plmn->rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
             ,reg_mode_band_pref_get()              /* band_pref              */
#endif    
#ifdef FEATURE_RAT_PRIORITY_LIST
             ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
#endif   
             ,MMR_REG_REQ_USER_SELECTION
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get() ,
              0
#endif
#ifdef FEATURE_FEMTO_CSG
             ,SYS_CSG_ID_INVALID
#endif 
            );

            reg_mode_req_rat_set(managed_roaming_plmn->rat);
            reg_mode_req_plmn_set(managed_roaming_plmn->plmn);
            finished = FALSE;
            reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
            state_p = &reg_state_registering;
         }
        else
        {
          finished = reg_mode_finished
          (
            reg_state_service_state,
          &(msg_p->available_plmn_list)   
          );
        }
#ifndef FEATURE_MODEM_HEAP
        gs_free(managed_roaming_plmn);
#else
        modem_mem_free(managed_roaming_plmn, MODEM_MEM_CLIENT_NAS);
#endif
        managed_roaming_plmn = NULL;
        break;

      /* Finish the PLMN selection procedure. CM will send 
      PS detach followed by service request with LTE removed after this */
      case MMR_CAUSE_ABORT_PLMN_SELECTION:
        finished = TRUE;
        break;

      case MMR_CAUSE_FAILURE_MAX_ATTEMPTED:
        finished = reg_mode_finished
        (
          reg_state_service_state,
          &(msg_p->available_plmn_list)
        );
        break;
#if defined(FEATURE_LTE)
      case MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW:
        {
          MSG_HIGH("=REG= Initiate PLMN selection move to GSM/UMTS",0,0,0);
          reg_send_grat_scan_done_set(FALSE);       
          reg_mode_start_plmn_selection_to_gw();
          reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
          state_p = &reg_state_registering;   
        }
        break;
#endif

      case MMR_CAUSE_NOT_SPECIFIED:
      case MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES:
      default:
#ifdef FEATURE_LTE
        if(reg_state_service_state.service_status == SYS_SRV_STATUS_SRV)
        {
          update_mm_for_lte_cap_change = reg_mode_update_curr_rat_pri_list
                 (&(msg_p->service_state.plmn), reg_state_service_state.active_rat);
        }
#endif
        finished = reg_mode_finished
        (
          reg_state_service_state,
          &(msg_p->available_plmn_list)
        );
        break;
    }
  }


  MSG_HIGH_DS_1(REG_SUB, "=REG= MAN_ROAM: CS service status: %d",reg_state_service_state.cs_service_status);
  /*
  ** Modify the Roaming indication flag based on the EPLMN list.
  ** Roaming should be OFF it the current PLMN is HPLMN or equivalent of HPLMN.
  */
  if(reg_nv_is_roaming_on_in_eplmn)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Roaming is not off for EHPLMN,nv_is_roaming_on_in_eplmn: %d",reg_nv_is_roaming_on_in_eplmn);
  }
  else if (sys_eplmn_list_equivalent_plmn(msg_p->service_state.plmn)&&
#ifdef FEATURE_EQUIVALENT_HPLMN
       reg_sim_is_ehplmn_in_eplmn()
#else
       sys_eplmn_list_equivalent_plmn(hplmn)
#endif
     )
  {
    reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_OFF;
  }
  if (finished)
  {
    /*
    ** Set the forbidden PLMN indication.
    */
    
#ifdef FEATURE_DUAL_SIM
    reg_state_service_state.plmn_forbidden = reg_sim_per_subs_plmn_forbidden(reg_state_service_state.plmn, (sys_modem_as_id_e_type)reg_sub_id);
#else 
    reg_state_service_state.plmn_forbidden = reg_sim_plmn_forbidden(reg_state_service_state.plmn);
#endif

    reg_state_print_service_state(reg_state_service_state);

#ifdef FEATURE_GSTK
    /* Send GSTK No service indication if UOOS is ON, and search is completed on both RAT
    *  and no service is found. */
    if (!reg_nv_is_imsi_switch_enabled())
    {
      if(reg_mode_scan_status_get() == REG_CM_SCAN_STATUS_COMPLETE_NO_PLMN) 
      {
        reg_send_mmr_gstk_service( FALSE);
      }
    }
#endif

    if(msg_p->only_for_reg == FALSE)
    {
#ifdef FEATURE_SGLTE
      if((REG_SGLTE_DUAL_STACK) && (reg_as_id == reg_cs_stack_id) &&
         (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
         (reg_state_service_state.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY)&&
         (reg_state_service_state_sim[reg_ps_stack_id].service_status != SYS_SRV_STATUS_SRV)
        )
      {
        reg_timers_start_sglte_ps_timer();
      }

      if(REG_SUB_IS_SGLTE && 
         (reg_sglte_mode) && (reg_as_id == reg_ps_stack_id) &&
         (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) 
        )
      {
        reg_timers_stop_sglte_ps_timer();
      }

      if((REG_SGLTE_DUAL_STACK) && (reg_as_id == reg_ps_stack_id)&&
         ((reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV)&&
         ((reg_mode_scan_scope_get() != SYS_SCAN_SCOPE_ACQ_DB) || 
          (reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH)) &&         
         (msg_p->service_state.no_service_cause != SYS_NO_SVC_CAUSE_RLF))&& 
         (reg_state_service_state_sim[reg_cs_stack_id].service_status == SYS_SRV_STATUS_SRV) &&
         (reg_state_service_state_sim[reg_cs_stack_id].plmn_service_capability == SYS_SRV_CAPABILITY_CS_PS_SERVICE) &&
         (reg_state_service_state_sim[reg_cs_stack_id].active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
         (reg_state_service_state_sim[reg_cs_stack_id].reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
         /*Adding CS stack id to get PLMN , becuase during split process finished or dual stack mode
           CS stack PLMN is to check for forbidden PLMN */
#ifdef FEATURE_DUAL_SIM
         (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs((sys_modem_as_id_e_type)reg_cs_stack_id),(sys_modem_as_id_e_type)reg_sub_id) == FALSE )
#else
         (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs((sys_modem_as_id_e_type)reg_cs_stack_id))==FALSE )
#endif
        )
      {
        state_p = &reg_state_idle;
        reg_timers_stop_sglte_ps_timer();
        reg_state_sglte_merge_in_progress = TRUE;
        reg_send_ps_timer_expiry();
      }
      else
#endif
      if (reg_state_cm_initiated_registration)
      {
        reg_state_cm_initiated_registration = FALSE;

        reg_send_cm_service_cnf
        (
          reg_state_cm_transaction_id,
          reg_state_service_state
        );
        if(reg_state_cm_sim_update_ind_pending)
        {
          reg_send_cm_sim_update_ind(SYS_SIM_STATE_AVAILABLE);
          reg_state_cm_sim_update_ind_pending = FALSE;
        }
      }
      else
      {
        reg_send_cm_service_ind
        (
          reg_state_service_state
        );
      }

#ifdef FEATURE_LTE
     if (local_cause != MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN)
     {
       reg_mode_reset_addtnl_info(SYS_ADDTNL_INFO_EMERG_CALL);
     }
#endif
  
#if defined FEATURE_DUAL_SIM
      reg_state_send_indication_on_other_stack();
#endif
    }

#ifdef FEATURE_LTE
    if(update_mm_for_lte_cap_change)
    {
      if(reg_state_force_pref_pending == TRUE)
      {
        reg_sim_write_rplmn_rat(SYS_RAT_NONE);
        MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to start plmn selection all over again");
        if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
           reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
        {
          reg_mode_start_plmn_selection
          (
             reg_mode_network_selection_mode_get(),
             reg_mode_req_mode_pref_get(),
             reg_mode_cs_for_request_get(),
             reg_mode_ps_for_request_get(),
             reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
             ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
             ,reg_mode_rat_pri_list_get()
#endif
             ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get(),
             0
#endif 
            );
            state_p = &reg_state_registering;
            reg_state_force_pref_pending = FALSE ;
        }
        else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                 reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )

        {
          reg_mode_start_manual_plmn_selection
          (
             reg_mode_network_selection_mode_get(),
             reg_send_last_reg_req_plmn_get(),
             reg_mode_mode_pref_get(),
             FALSE,
             FALSE,
             reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
             reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
             reg_mode_rat_pri_list_get(),
#endif
             REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
             ,reg_mode_sms_only_get(),
             0
#endif                 
          );
          state_p = &reg_state_registering;
          reg_state_force_pref_pending = FALSE ;
        }
        reg_state_update_service_state_if_required(state_p);
      }
      else
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= Sending REG request to update LTE RAT capability");
        reg_send_mmr_reg_req_curr_rat_list();
        state_p = &reg_state_registering;
      }
    }
    else
#endif
    if (attempting_registration)
    {
      state_p = &reg_state_registering;
    }
    else
    {
      /*
      **  When no registration is going on or plmn selection is finished
      **  but no suitable PLMN found, so MS moved to LIMITED service.
      **  In that case Start HPLMN timer if MS in AUTOMATIC or
      **  LIMITED_ROUTINE_ACQUISITION and Roaming is on.
      */
      /*  If full service is received on a VPLMN, start HPLMN search timer
      */
      if (((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
           (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) ||
           (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
          ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
          (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
          (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON))
      {
        reg_timers_start_hplmn_search_period_timer();
        reg_timers_stop_all_irat_search_period_timer();
      }
     else if (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV)
     {
         MSG_HIGH_DS_0(REG_SUB, "=REG= reg_mode_rat_search_start");   
        if (reg_mode_rat_search_start())
        {            
            reg_timers_stop_hplmn_search_period_timer();
        }
     }
      if(!sys_plmn_id_is_undefined(msg_p->service_state.plmn))
      {
        /* Updating previous plmn as current one */
        reg_state_prev_plmn = msg_p->service_state.plmn;
      }

      reg_state_is_pwr_on_manual_plmn_selection = FALSE;

#ifdef FEATURE_FEMTO_CSG
      if((reg_state_power_on_csg_search == TRUE) && 
        (msg_p->only_for_reg == FALSE) && 
        (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
        (reg_sim_get_csg_list_read_complete() == TRUE))
      {
        MSG_HIGH_DS_1(REG_SUB, "=REG= config-%d",reg_state_csg_selection_config);
        if((reg_state_csg_selection_config == SYS_CSG_SELECTION_CONFIG_1) &&
           ((reg_state_service_state.csg_info.csg_id == SYS_CSG_ID_INVALID) || 
           ((reg_state_service_state.csg_info.hybrid_cell == TRUE) && 
             reg_state_service_state.csg_in_white_list == FALSE)))
        {
          reg_send_cm_csg_search_trigger_ind();
        }
        reg_state_power_on_csg_search = FALSE; 
      }
#endif 

      if((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
         (msg_p->only_for_reg == FALSE) &&
         (local_cause != MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN))
      {
        reg_mode_remove_from_attempted_plmn_list(reg_state_service_state.plmn, reg_state_service_state.active_rat); 
      }

      if(reg_state_service_state.service_status == SYS_SRV_STATUS_SRV)
      {
        reg_state_power_up_system_selection = FALSE;
        reg_mode_set_available_plmn_list_populated(FALSE);

        if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
        {
          reg_mode_network_selection_mode_set(SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
        }
        else if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
        {
          reg_mode_network_selection_mode_set(SYS_NETWORK_SELECTION_MODE_MANUAL);
        }
      }

#ifdef FEATURE_GSTK
      if((reg_state_service_state.service_status== SYS_SRV_STATUS_NO_SRV) &&
         (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED ))
      {
        current_service_status = SYS_SRV_STATUS_LIMITED;
      }
      else
      {
       current_service_status = reg_state_service_state.service_status;
      }
      /* Send GSTK  When registration is tried on alla available PLMNs */
      if (reg_nv_is_imsi_switch_enabled())
      {
        previous_service_status = reg_state_get_last_gstk_sent_svc();
        if ((reg_state_is_gstk_event_to_be_send() || 
           (current_service_status != previous_service_status )||
           ((reg_state_service_state.active_rat != reg_state_get_last_gstk_sent_rat()) &&
             (current_service_status == SYS_SRV_STATUS_SRV))
            ) &&
            (reg_sim_state_get() != SYS_SIM_STATE_NOT_AVAILABLE)&&
            (msg_p->cause != MMR_CAUSE_FAILURE_REMAIN_ON_PLMN))
        {
           if ((reg_state_service_state.service_status== SYS_SRV_STATUS_NO_SRV) &&
               (msg_p->cause != MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED ))
           {
             if ((reg_mode_scan_status_get() == REG_CM_SCAN_STATUS_COMPLETE_NO_PLMN) &&
                 (previous_service_status == SYS_SRV_STATUS_NONE))
             {

                reg_send_mmr_gstk_service(FALSE);
             }
           }
           else
           {
             reg_send_mmr_gstk_service( TRUE );
             reg_state_update_last_gstk_sent_plmn(reg_state_service_state.plmn);
             reg_state_update_last_gstk_sent_svc(current_service_status);
             reg_state_update_last_gstk_sent_rat(reg_state_service_state.active_rat);
           }
           reg_state_set_gstk_event_flag(FALSE);
        }
      }
#endif

      if((reg_mode_foreground_search_status == REG_MODE_FOREGROUND_SEARCH_STARTED ) &&
         (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))
      {
        reg_mode_foreground_search_status   = REG_MODE_FOREGROUND_SEARCH_ATTEMPTED;
      }

      state_p = &reg_state_idle;
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_bst_status_set(FALSE);
#endif
      reg_mode_set_hplmn_to_be_selected(FALSE);
    }
  }
  else
  {
    state_p = &reg_state_registering;
  }

  if((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) ||
     (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED))
  {
    reg_mode_foreground_search_status   = REG_MODE_FOREGROUND_SEARCH_NONE;
  }
  
  reg_last_mm_message_only_for_reg = msg_p->only_for_reg;

#ifdef FEATURE_SGLTE

     if(reg_state_service_state_sim[reg_ps_stack_id].service_status != SYS_SRV_STATUS_NO_SRV && reg_as_id == reg_ps_stack_id)
     {
        boolean bplmn_search_not_started = TRUE;
        state_local = state_p;
        if(state_local == &reg_state_idle && reg_state_force_bplmn_search_start)
        {
           state_local = state_local->process_lte_irat_search_timer_expiration();
           if(state_local != &reg_state_idle)
           {
             bplmn_search_not_started = FALSE;
             reg_timers_stop_lte_irat_search_period_timer();
			 reg_state_irat_lte_timer_search_ongoing = TRUE;
             reg_state_force_irat_search_initiated = TRUE;
           }
        }
        if( bplmn_search_not_started)                      
        {
          reg_mode_rat_search_start();
        }
        state_p = state_local;
     }
     reg_state_force_bplmn_search_start = FALSE;
  #endif
        if(reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV &&
			state_p == &reg_state_idle && reg_state_force_bplmn_search_start)
        {
           state_p = state_p->process_lte_irat_search_timer_expiration();
           if(state_p != &reg_state_idle)
           {
             reg_timers_stop_lte_irat_search_period_timer();
			 reg_state_irat_lte_timer_search_ongoing = TRUE;
             reg_state_force_irat_search_initiated = TRUE;
           }
        }
        reg_state_force_bplmn_search_start = FALSE;

#ifndef FEATURE_MODEM_HEAP
  gs_free(available_plmn_list);
#else
  modem_mem_free(available_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
  available_plmn_list = NULL;
  return state_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_service_ind

==============================================================================*/

reg_state_p_type reg_state_process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{
  sys_srv_domain_e_type allowed_service_domain;
  sys_detailed_plmn_list_info_s_type managed_roaming_plmn;
  const reg_sim_preferred_plmn_list_s_type* low_priority_list;
  uint16 i;
  reg_state_p_type   next_state_p = &reg_state_idle;
#ifndef FEATURE_EQUIVALENT_HPLMN
  sys_plmn_id_s_type hplmn;
#endif
  boolean            send_cm_service_ind = TRUE;
  sys_sim_state_e_type previous_sim_state = reg_sim_state_get();
  sys_radio_access_tech_e_type active_rat = reg_state_service_state.active_rat;
  sys_plmn_id_s_type tim_plmn = {{0x22,0xF2,0x10}},hutchit_plmn = {{0x22,0xF2,0x99}};

  reg_nv_imp_variable_prints();
  reg_sim_imp_variable_prints();
  if(reg_state_is_sim_invalidated(previous_sim_state,
                                  msg_p->service_state.sim_state))
  {
    if( reg_mode_revalidate_sim_allowed(msg_p->service_state.plmn))
    {
      reg_timers_start_validate_sim_timer();
      reg_mode_increment_validate_sim_counter();
#ifdef FEATURE_SGLTE
      if(REG_SGLTE_DUAL_STACK)
      {
         sys_sim_state_e_type reg_sim_state_cs = SYS_SIM_STATE_NONE, reg_sim_state_ps = SYS_SIM_STATE_NONE;
         if(reg_as_id == reg_ps_stack_id)
         {
           reg_sim_state_ps = msg_p->service_state.sim_state;
           reg_as_id = reg_cs_stack_id;
           reg_sim_state_cs = reg_sim_state_get();
           reg_as_id = reg_ps_stack_id;
         }
         else if(reg_as_id == reg_cs_stack_id)
         {
           reg_sim_state_cs = msg_p->service_state.sim_state;
           reg_as_id = reg_ps_stack_id;
           reg_sim_state_ps = reg_sim_state_get();
           reg_as_id = reg_cs_stack_id;
         }
         if((reg_sim_state_ps == SYS_SIM_STATE_PS_INVALID  &&
             reg_sim_state_cs == SYS_SIM_STATE_CS_INVALID ))
         {
           (void) mmgsdi_session_manage_illegal_subscription(reg_sim_mmgsdi_info.session_id,
                                                              MMGSDI_SESSION_APP_IS_ILLEGAL);
           MSG_HIGH_DS_2(REG_SUB, "=REG= SIM is marked as illegal and same has been notified to MSGSDI sim state PS = %d SIM state CS = %d",reg_sim_state_ps,reg_sim_state_cs);
         }
      }
#endif
    }
    else
#ifdef FEATURE_DUAL_SIM
    if((reg_sim_check_if_t3245_behavior_enabled((sys_modem_as_id_e_type)reg_sub_id) == TRUE) &&
       (reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_sub_id) == FALSE))
    {
      reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE,(sys_modem_as_id_e_type)reg_sub_id);
    }
#else
    if((reg_sim_check_if_t3245_behavior_enabled() == TRUE) &&
       (reg_is_t3245_timer_active() == FALSE))
    {
      reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE);
    }
#endif
  }
#if defined(FEATURE_LTE) && defined(FEATURE_RAT_PRIORITY_LIST)
   if(msg_p->service_state.sim_state == SYS_SIM_STATE_PS_INVALID && 
      reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED &&
      reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS,reg_mode_rat_pri_list_get(), FALSE)
     )
   {
     
     if((!((reg_state_service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
         (reg_bit_mask_check(reg_mode_additional_info_get(),
                           (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)))&&
        (!reg_mode_is_only_req_rat_present_in_rat_pri_list(reg_mode_rat_pri_list_get(), SYS_SYS_MODE_LTE)))   
     {
       reg_mode_remove_rat_from_rat_pri_list(reg_mode_rat_pri_list_get(), SYS_SYS_MODE_LTE);
       reg_mode_filter_available_plmn_list_rat(SYS_RAT_LTE_RADIO_ACCESS);
     }
   }
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
  /*
  ** If FCN - Acting HPLMN is delayed, i.e. PLMN rescan should be done.
  ** Ignore MMR_REG_CNF if PLMN rescan is started.
  */
  if (reg_state_delay_fcn_act_hplmn == TRUE)
  {
    reg_state_delay_fcn_act_hplmn = FALSE;
    if (reg_mode_restart_plmn_selection() == TRUE)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_SERVICE_IND - ACT HPLMN Changed");
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      return &reg_state_registering;
    }
  }
#endif

  if(msg_p->service_state.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    reg_mode_scan_scope_set(SYS_SCAN_SCOPE_FULL_BAND);             
  }
  reg_mode_foreground_search_status   = REG_MODE_FOREGROUND_SEARCH_NONE;

  if (msg_p->service_state.service_status == SYS_SRV_STATUS_LIMITED)
  {
    if (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED)
    {
      if(reg_last_mm_message_only_for_reg == FALSE)
      {
        /*
             ** Filter out extraneous mmr_service_ind messages so MMoC/SD
             ** will continue to periodically scan for normal service.
             */
        if ((reg_state_service_state.sim_state               == msg_p->service_state.sim_state) &&
            (sys_plmn_match(reg_state_service_state.plmn, msg_p->service_state.plmn)) &&
            (reg_state_service_state.plmn_service_capability == msg_p->service_state.plmn_service_capability) &&
            (reg_state_service_state.reg_service_domain      == msg_p->service_state.reg_service_domain) &&
            (reg_state_service_state.active_rat              == msg_p->service_state.active_rat) &&
            (reg_state_service_state.ps_data_suspend         == msg_p->service_state.ps_data_suspend) &&
            (msg_p->cause                                    == MMR_CAUSE_NOT_SPECIFIED )
#ifdef FEATURE_FEMTO_CSG
            && (reg_state_service_state.csg_info.csg_id == msg_p->service_state.csg_info.csg_id)
#endif 
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
            && ((reg_state_service_state.active_rat != SYS_RAT_LTE_RADIO_ACCESS) ||
                (reg_state_service_state.eps_nw_feature_support.emc_bs_supported == 
                                                 msg_p->service_state.eps_nw_feature_support.emc_bs_supported))
#endif 
            )
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Filtered");
          if(msg_p->service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS || msg_p->service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS ||  msg_p->service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)   
         {
           if(reg_state_cell_id_changed(msg_p->service_state,msg_p->cell_id))
           {
              MSG_HIGH_DS_0(REG_SUB,"=REG= LTE IRAT BPLMN: Reset the retry counter");
             reg_mode_reset_retry_counter();
           }
           reg_state_cell_id = msg_p->cell_id;
         }
         else
         {
            reg_state_cell_id = 0xFFFF;                       
         }
          MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : REG Cell Id =%d ",reg_state_cell_id);        
          return next_state_p;
        }
      }
    }
    /*
    ** Modify the service_status so the phone will remain camped on the current network.
    */
    else if ((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL) &&
             (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED) &&
             (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      msg_p->service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    }
  }

  /*
  ** Make sure that MM is not improperly reporting that the SIM is not
  ** available.  REG only expects to be informed that the SIM is not
  ** available upon receiving a CM_SIM_NOT_AVAILABLE_REQ.
  */
  if (msg_p->service_state.sim_state != SYS_SIM_STATE_NOT_AVAILABLE)
  {
    reg_sim_state_set(msg_p->service_state.sim_state);
  }

  if(msg_p->service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS || 
   msg_p->service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS ||  msg_p->service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)   
  {
    if(reg_state_cell_id_changed(msg_p->service_state,msg_p->cell_id))
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN: Reset the retry counter");                   
      reg_mode_reset_retry_counter();
    }
    reg_state_cell_id = msg_p->cell_id;
  }
  else
  {
    reg_state_cell_id = 0xFFFF;                                                          
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : REG Cell Id =%d reg_state_hplmn_irat_timer_count = %d ",reg_state_cell_id, reg_state_hplmn_irat_timer_count);                                  

  reg_state_service_state = msg_p->service_state;

#ifdef FEATURE_SGLTE
  if(REG_SGLTE_DUAL_STACK && reg_state_rlf_started == TRUE &&
    (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV))
  {
    reg_state_rlf_started = FALSE;
  }
#endif



#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
/*If No service is due to RLF then save PLMN into RLF PLMN and set PLMN in reg service state to undefined*/
  if ((reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
         ( reg_state_service_state.no_service_cause == SYS_NO_SVC_CAUSE_RLF))
  {
     reg_state_rlf_plmn = reg_state_service_state.plmn;     
     sys_plmn_undefine_plmn_id(&reg_state_service_state.plmn);
   
    reg_state_rlf_rat  = reg_state_service_state.active_rat;
  }
#endif
#ifdef FEATURE_SGLTE_DUAL_SIM
  if(REG_SGLTE_DUAL_STACK && (reg_as_id == reg_ps_stack_id) && 
     (reg_state_service_state.acq_status == SYS_ACQ_STATUS_NO_RESOURCE) &&
     (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV))
  {
    reg_state_trm_failure_sglte_main = TRUE;
  }
  else if(REG_SUB_IS_SGLTE && (reg_as_id == reg_ps_stack_id))
  {
    reg_state_trm_failure_sglte_main = FALSE;
  }
#endif 

  reg_state_service_state.sim_state = reg_sim_state_get();
#ifndef FEATURE_EQUIVALENT_HPLMN
  hplmn = reg_sim_read_hplmn();
#endif

   sys_plmn_undefine_plmn_id( &managed_roaming_plmn.plmn ); 
   if ( mm_managed_roaming_retry_lu_in_manual_mode && mm_managed_roaming_enabled &&
        ( reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
          reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION ))
   {
     low_priority_list = reg_sim_low_priority_plmn_list_get();
     for(i = 0; i<low_priority_list->length; i++)
     {
       if(sys_plmn_match(low_priority_list->info[i].plmn, reg_state_service_state.plmn) &&
          low_priority_list->info[i].rat == reg_state_service_state.active_rat)
       {
         managed_roaming_plmn.plmn = reg_state_service_state.plmn;
         managed_roaming_plmn.rat = reg_state_service_state.active_rat;
         break;
       }
     }
   }

  if ((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) && 
      (msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) )
  {
    /* If forced IRAT is enabled, stop the relevant timers*/
    reg_mode_rat_search_stop(msg_p->service_state.active_rat);
  }
  /*
  ** Set the roaming indication.
  */
  if ( !sys_plmn_id_is_undefined(msg_p->service_state.plmn) )
  {
#ifndef FEATURE_EQUIVALENT_HPLMN
    if ( sys_plmn_match(hplmn, msg_p->service_state.plmn) )
#else
    if ( reg_sim_is_ehplmn(msg_p->service_state.plmn) )
#endif
    {
      reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_OFF;
    }
    else
    {
      reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_ON;
    }
  }
  /*
  ** Save the equivalent PLMN list if required.
  */
  if (msg_p->update_equivalent_plmn_list)
  {
    boolean list_matched = reg_mode_save_equivalent_plmn_list
    (
      msg_p->service_state.plmn,
      &(msg_p->equivalent_plmn_list)
    );
    boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);    
    if ((!list_matched) || eplmn_list_validated)
    {
       MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
       reg_send_eq_plmn_change_ind();
    }
  }
  /*
  ** Combine attach rejected with cause #14, MM trying for LU.
  ** So remain on PLMN.
  */
  if (msg_p->cause == MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN)
  {
    msg_p->cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  }
  if(msg_p->service_state.sim_state == SYS_SIM_STATE_CS_PS_INVALID &&
     msg_p->service_state.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
  }
  else if (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED)
  {
    if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
        reg_send_grat_scan_done_set(FALSE); 
        reg_mode_start_plmn_selection
        (
          reg_mode_network_selection_mode_get(),
          reg_mode_req_mode_pref_get(),
          reg_mode_cs_for_request_get(),
          reg_mode_ps_for_request_get(),
          reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,reg_mode_rat_pri_list_get()
#endif
          ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
         0
#endif 
        );
#ifdef FEATURE_ENHANCED_NW_SELECTION
      if(reg_sim_read_ens_flag())
      {
        send_cm_service_ind = TRUE;
        reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
      }
      else
#endif
      {
        send_cm_service_ind = FALSE;
        reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      }
      next_state_p = &reg_state_registering;
    }
    else if ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    }
  }

  /*
  ** If MM sends MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION then make
  ** sure the service status indicates no service so MMoC/SD will initiate
  ** PLMN selection.
  */
  else if (msg_p->cause == MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION)
  {
    if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {

      reg_send_grat_scan_done_set(FALSE);
        
      reg_mode_start_automatic_plmn_selection
      (
        SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
        REG_MODE_AUTOMATIC_MODE_NO_PREFERENCE,
        reg_mode_req_mode_pref_get(),
        reg_mode_cs_for_request_get(),
        reg_mode_ps_for_request_get(),
        reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,reg_mode_rat_pri_list_get()
#endif
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
         0
#endif 
      );

      send_cm_service_ind = TRUE;
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
      next_state_p = &reg_state_registering;
    }
    else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)          
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    }
  }
  /*
  ** If MM sends MMR_CAUSE_FAILURE_REMAIN_ON_PLMN then set the service
  ** status so MMoC/SD will not initiate PLMN selection.
  */
  else if (msg_p->cause == MMR_CAUSE_FAILURE_REMAIN_ON_PLMN)
  {

#ifdef FEATURE_FEMTO_CSG
    if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
    { 
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
    } 
    else
#endif 
    if (((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
        (reg_state_service_state.service_status == SYS_SRV_STATUS_NO_SRV)) &&
        (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED))
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    }

    if ((reg_last_cm_service_req_type == REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION) &&
         (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL))
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
    }  

    /*
    **  Start HPLMN timer, so that higher priority PLMN search will happen
    **  when MS is in Limited Service (No suitable PLMN is found).
    */
    if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
         (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
         (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON))
    {
      reg_timers_start_hplmn_search_period_timer();
      reg_timers_stop_all_irat_search_period_timer();
    }
    else if (reg_mode_rat_search_start())
    {
        reg_timers_stop_hplmn_search_period_timer();
    }
  }
  /*
  ** If MM sends MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED then reinitiate PLMN
  ** selection while setting the service status to LIMITED_REGIONAL so
  ** SD will not trigger PLMN selection.
  */
  else if (msg_p->cause == MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED)
  {
    reg_send_grat_scan_done_set(FALSE); 
    active_rat = reg_state_service_state.active_rat;
    
	if(reg_nv_is_camp_on_2g_if_reg_failure && sys_plmn_match(msg_p->service_state.plmn,tim_plmn) && sys_plmn_match(reg_sim_read_hplmn(),hutchit_plmn))
    {
      active_rat = SYS_RAT_GSM_RADIO_ACCESS;
    }

    if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)
    {
       reg_mode_start_manual_plmn_selection_on_specified_plmn
        (
          msg_p->service_state.plmn,
          SYS_NETWORK_SELECTION_MODE_MANUAL,
          reg_mode_mode_pref_get(),
          FALSE,
          FALSE,
          reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
          reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          reg_mode_rat_pri_list_get(),
#endif
          active_rat
#ifdef FEATURE_LTE
         ,reg_mode_sms_only_get(),
          0
#endif 
         ,MMR_REG_REQ_NORMAL
        );
      next_state_p = &reg_state_registering;
    }
    else
    {
    reg_mode_start_plmn_selection_on_specified_plmn
        (
          msg_p->service_state.plmn,
          reg_mode_network_selection_mode_get(),
          reg_mode_mode_pref_get(),
          FALSE,
          FALSE,
          reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
          reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          reg_mode_rat_pri_list_get(),
#endif
          active_rat
#ifdef FEATURE_LTE
         ,reg_mode_sms_only_get(),
          0
#endif 
         ,MMR_REG_REQ_ACQ_HPLMN_OR_RPLMN_RAT_SEARCH_ORDER
        );
    next_state_p = &reg_state_registering;
    }
    if((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
       (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
    {
      send_cm_service_ind = FALSE;
    }

    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
      
  }
  else if (msg_p->cause == MMR_CAUSE_PLMN_SEL_CALL_END)
  {
    reg_send_grat_scan_done_set(FALSE);

    if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
      reg_mode_start_plmn_selection_on_specified_plmn
      (
        reg_send_last_reg_req_plmn_get(),
        reg_mode_network_selection_mode_get(),
        reg_mode_mode_pref_get(),
        FALSE,
        FALSE,
        reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
        reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_rat_pri_list_get(),
#endif
        reg_send_last_reg_req_rat_get()
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get()
        ,reg_mode_additional_info_get()
#endif 
         ,MMR_REG_REQ_NORMAL
      );
    }
    else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL)
    {
      reg_mode_start_manual_plmn_selection
      (
        reg_mode_network_selection_mode_get(),
        reg_send_last_reg_req_plmn_get(),
        reg_mode_mode_pref_get(),
        FALSE,
        FALSE,
        reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
        reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_rat_pri_list_get(),
#endif
        REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
        ,reg_mode_sms_only_get()
        ,reg_mode_additional_info_get()
#endif                 
      );
    } 
    else
    {
      reg_send_last_mmr_reg_req();
    }

    send_cm_service_ind = FALSE;
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    next_state_p = &reg_state_registering; 
  }
  else if(msg_p->cause == MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH &&
         (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
          reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) )
  {
    reg_state_user_resel_plmn = reg_state_service_state.plmn;
    reg_state_user_resel_plmn_rat = reg_state_service_state.active_rat;
    reg_mode_start_foreground_plmn_search();
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    next_state_p = &reg_state_registering;
  }
  
  else if((msg_p->cause == MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH) && 
          (reg_state_service_state.cs_service_status == SYS_SRV_STATUS_LIMITED) &&
          (!sys_plmn_id_is_undefined(managed_roaming_plmn.plmn)))
  {
    allowed_service_domain = reg_mode_allowed_service_domain
    (
       managed_roaming_plmn.plmn,                                 /* plmn                   */
       reg_mode_req_service_domain_get()     /* req_service_domain     */
    );
           
    reg_send_mmr_reg_req
    (
      reg_mode_network_selection_mode_get(), /* network_selection_mode */
      managed_roaming_plmn.plmn,                                 /* plmn                   */
      allowed_service_domain,               /* req_service_domain     */
      reg_mode_mode_pref_get(),             /* mode_pref              */
      reg_mode_cs_for_request_get(),        /* cs_for_request         */
      reg_mode_ps_for_request_get(),        /* ps_for_request         */
      managed_roaming_plmn.rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
     ,reg_mode_band_pref_get()              /* band_pref              */
#endif    
#ifdef FEATURE_RAT_PRIORITY_LIST
     ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
#endif   
     ,MMR_REG_REQ_USER_SELECTION
#ifdef FEATURE_LTE
     ,reg_mode_sms_only_get() ,
      0
#endif
#ifdef FEATURE_FEMTO_CSG
     ,SYS_CSG_ID_INVALID
#endif 
       );

       reg_mode_req_rat_set(managed_roaming_plmn.rat);
       reg_mode_req_plmn_set(managed_roaming_plmn.plmn);
       reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
       next_state_p = &reg_state_registering;
   }
#if defined(FEATURE_LTE)
  else if(msg_p->cause == MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection move to GSM/UMTS");
    reg_send_grat_scan_done_set(FALSE);         
    reg_mode_start_plmn_selection_to_gw();
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
    next_state_p = &reg_state_registering;    
  }
#endif
  /*
  ** The MS reacquired or changed service.
  */
  else if ( msg_p->service_state.service_status == SYS_SRV_STATUS_SRV)
            
  {
    /*
    ** MM is reporting service on a RAT that is inconsistent with the
    ** requested mode preference.  Set the service status to no service
    ** so MMoC/SD will initiate PLMN selection.
    */
#ifndef FEATURE_RAT_PRIORITY_LIST
    if (((msg_p->service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS) && (
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
          #error code not present
#endif
          (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY))) ||
        ((msg_p->service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) &&
         (reg_mode_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY)))
#else
    if(!reg_mode_is_rat_present_in_rat_pri_list(msg_p->service_state.active_rat,
                                                 reg_mode_rat_pri_list_get(), FALSE))  
#endif 
    {
      reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
    }
    else
    {

      if (reg_sim_is_rplmn(&(msg_p->service_state)))
      {
#ifndef FEATURE_FEMTO_CSG
        reg_sim_write_rplmn_rat
        (
          reg_state_service_state.active_rat
        );
#ifdef FEATURE_NAS_ECALL
        reg_nv_write_rplmn(reg_state_service_state.plmn);
        reg_nv_write_rplmn_dup(FALSE);
#endif
#else
        if(reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
        {
          reg_sim_write_rplmn_rat
          (
            reg_state_service_state.active_rat
          );
          reg_nv_write_rplmn(reg_state_service_state.plmn);
          reg_nv_write_rplmn_dup(FALSE);
        }
        else
        {
          reg_nv_write_rplmn_dup(TRUE);
        }
#endif 
      }
      if( (reg_is_qrd_device ==TRUE) && 
          reg_sim_is_plmn_not_preferred_plmn(reg_state_service_state.plmn,reg_state_service_state.active_rat))
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN is not higher priority PLMN");
        (void)reg_mode_update_vplmn_plmn_rat(reg_state_service_state.plmn,reg_state_service_state.active_rat);   
      }
      if(reg_state_curr_p != &reg_state_automatic_search)
      {
        if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
            (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))&&
           (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON))
        {
           reg_timers_start_hplmn_search_period_timer();
           reg_timers_stop_all_irat_search_period_timer();
        }
        else 
        {    
           if (reg_mode_rat_search_start())
           {
              reg_timers_stop_hplmn_search_period_timer();
           }
        }
      }
#ifdef FEATURE_LTE
      if(reg_mode_update_curr_rat_pri_list(&(msg_p->service_state.plmn),
                                      reg_state_service_state.active_rat))
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to update LTE RAT capability");
        if(reg_state_force_pref_pending == TRUE)
        {
           reg_sim_write_rplmn_rat(SYS_RAT_NONE);
           MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to start plmn selection all over again");
           if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC || 
              reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
           {
             reg_mode_start_plmn_selection
             (
               reg_mode_network_selection_mode_get(),
               reg_mode_req_mode_pref_get(),
               reg_mode_cs_for_request_get(),
               reg_mode_ps_for_request_get(),
               reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
              ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get(),
               0
#endif  
             );
             reg_state_force_pref_pending = FALSE ;
             next_state_p = &reg_state_registering;
          }
          else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                  reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )
          {
            reg_mode_start_manual_plmn_selection
            (
               reg_mode_network_selection_mode_get(),
               reg_send_last_reg_req_plmn_get(),
               reg_mode_mode_pref_get(),
               FALSE,
               FALSE,
               reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
               reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
               reg_mode_rat_pri_list_get(),
#endif
               REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
              ,reg_mode_sms_only_get(),
               0
#endif                 
            );
            reg_state_force_pref_pending = FALSE ;
            next_state_p = &reg_state_registering;
          } 
          reg_state_update_service_state_if_required(next_state_p);
        }
        else
        {
          reg_mode_start_plmn_selection_on_specified_plmn
          (
            msg_p->service_state.plmn,
            reg_mode_network_selection_mode_get(),
            reg_mode_mode_pref_get(),
            FALSE,
            FALSE,
            reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
            reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
            reg_mode_curr_rat_pri_list_get(),
#endif
            reg_state_service_state.active_rat
#ifdef FEATURE_LTE
            ,reg_mode_sms_only_get(),
            0
#endif 
            ,MMR_REG_REQ_NORMAL
          );
          next_state_p = &reg_state_registering;
        }
      }
#endif
      /* Updating previous plmn as current one */
      reg_state_prev_plmn = msg_p->service_state.plmn;
    }
  }
  else if ( ((reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
             (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
            ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
            (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON) )
  {
    reg_timers_start_hplmn_search_period_timer();
    reg_timers_stop_all_irat_search_period_timer();
   }
  else if (msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES)
  {
     if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL 
#if defined(FEATURE_LTE) && defined(FEATURE_RAT_PRIORITY_LIST)
           && (!(msg_p->service_state.sim_state == SYS_SIM_STATE_PS_INVALID &&
            reg_mode_is_only_req_rat_present_in_rat_pri_list(reg_mode_rat_pri_list_get(), SYS_SYS_MODE_LTE)))
#endif
        )
     {
        reg_mode_start_manual_plmn_selection
        (
           SYS_NETWORK_SELECTION_MODE_MANUAL,
           reg_state_service_state.plmn,
           reg_mode_mode_pref_get(),
           reg_mode_cs_for_request_get(),
           reg_mode_ps_for_request_get(),
           reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
           ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
           ,reg_mode_rat_pri_list_get()
#endif
           ,REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
           ,reg_mode_sms_only_get()
           ,reg_mode_additional_info_get()
#endif                 
         );
         next_state_p = &reg_state_registering;
     }
  }
     else if (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV)
     {
        if (reg_mode_rat_search_start())
        {
            reg_timers_stop_hplmn_search_period_timer();
        }
  }



#ifdef FEATURE_SGLTE

     if(reg_state_service_state_sim[reg_ps_stack_id].service_status != SYS_SRV_STATUS_NO_SRV && reg_as_id == reg_ps_stack_id)
     {
        boolean bplmn_search_not_started = TRUE;
        if(reg_state_next_p == &reg_state_idle && reg_state_force_bplmn_search_start)
        {
           reg_state_next_p = reg_state_next_p->process_lte_irat_search_timer_expiration();
           next_state_p = reg_state_next_p; 
           if(reg_state_next_p != &reg_state_idle)
           {
             bplmn_search_not_started = FALSE;
             reg_timers_stop_lte_irat_search_period_timer();
			 reg_state_irat_lte_timer_search_ongoing = TRUE;
             reg_state_force_irat_search_initiated = TRUE;
           }
        }
        if(bplmn_search_not_started)         
        {
          reg_mode_rat_search_start();
        }
     }
     reg_state_force_bplmn_search_start = FALSE;
#endif
        if(reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV && 
			reg_state_next_p == &reg_state_idle && reg_state_force_bplmn_search_start)
        {
           reg_state_next_p = reg_state_next_p->process_lte_irat_search_timer_expiration();
           next_state_p = reg_state_next_p; 
           if(reg_state_next_p != &reg_state_idle)
           {
             reg_timers_stop_lte_irat_search_period_timer();
			 reg_state_irat_lte_timer_search_ongoing = TRUE;
             reg_state_force_irat_search_initiated = TRUE;
           }
        }
        reg_state_force_bplmn_search_start = FALSE;


  /*
  ** Modify the Roaming indication flag based on the EPLMN list.
  ** Roaming should be OFF it the current PLMN is HPLMN or equivalent of HPLMN.
  */
  if(reg_nv_is_roaming_on_in_eplmn)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Roaming is not off for EHPLMN,nv_is_roaming_on_in_eplmn: %d",reg_nv_is_roaming_on_in_eplmn);
  }
  else if (sys_eplmn_list_equivalent_plmn(msg_p->service_state.plmn)&&
#ifdef FEATURE_EQUIVALENT_HPLMN
       reg_sim_is_ehplmn_in_eplmn()
#else
       sys_eplmn_list_equivalent_plmn(hplmn)
#endif
     )
  {
    reg_state_service_state.roaming_ind = SYS_ROAM_STATUS_OFF;
  }


  /*
  ** Set the forbidden PLMN indication.
  */
#ifdef FEATURE_DUAL_SIM
  reg_state_service_state.plmn_forbidden = reg_sim_per_subs_plmn_forbidden(reg_state_service_state.plmn, (sys_modem_as_id_e_type)reg_sub_id);
#else
  reg_state_service_state.plmn_forbidden = reg_sim_plmn_forbidden(reg_state_service_state.plmn);
#endif

  /*
  ** Set the service status so SD will not reinitiate an attempt to
  ** acquire service in manual mode.
  */
  if ((reg_state_service_state.plmn_forbidden) &&
      (sys_plmn_match(reg_state_service_state.plmn, reg_mode_req_plmn_get())) && 
      ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
       (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) &&
      (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED))
  {
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED_REGIONAL;
  }

  /*
  ** Output the service state.
  */
  reg_state_print_service_state(reg_state_service_state);

  MSG_HIGH_DS_1(REG_SUB, "=REG= MAN_ROAM: CS service status: %d",reg_state_service_state.cs_service_status);
  
  if (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV)
  {
    reg_state_power_up_system_selection = FALSE;
    reg_mode_set_available_plmn_list_populated(FALSE);    

    if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
    {
      reg_mode_network_selection_mode_set(SYS_NETWORK_SELECTION_MODE_AUTOMATIC);
    }
    else if (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
    {
      reg_mode_network_selection_mode_set(SYS_NETWORK_SELECTION_MODE_MANUAL);
    } 
  }

  /*
  ** Send a confirmation or indication message to CM.
  */
  if (send_cm_service_ind && (msg_p->only_for_reg == FALSE))
  {
#ifdef FEATURE_GSTK
      if ((reg_nv_is_imsi_switch_enabled())&&
           (reg_state_is_gstk_event_to_be_send() ||
            reg_state_get_last_gstk_sent_svc() != SYS_SRV_STATUS_SRV  ||
            ((reg_state_service_state.active_rat != reg_state_get_last_gstk_sent_rat()) &&
            (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV))
             )&&
            ((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) ||
             ((msg_p->cause == MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED) &&
             ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
              (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)))))
      {

        reg_send_mmr_gstk_service( TRUE);
        reg_state_update_last_gstk_sent_plmn(reg_state_service_state.plmn);
        reg_state_update_last_gstk_sent_svc(reg_state_service_state.service_status);
        reg_state_update_last_gstk_sent_rat(reg_state_service_state.active_rat);
        reg_state_set_gstk_event_flag(FALSE);
      }
#endif

    if(reg_state_service_state.service_status == SYS_SRV_STATUS_SRV)
    {
      reg_mode_remove_from_attempted_plmn_list(reg_state_service_state.plmn, reg_state_service_state.active_rat); 
    }

    if (reg_state_cm_initiated_registration)
    {
      reg_state_cm_initiated_registration = FALSE;

      reg_send_cm_service_cnf
      (
        reg_state_cm_transaction_id,
        reg_state_service_state
      );

      if(reg_state_cm_sim_update_ind_pending)
      {
        reg_send_cm_sim_update_ind(SYS_SIM_STATE_AVAILABLE);
        reg_state_cm_sim_update_ind_pending = FALSE;
      }
    }
    else
    {
      reg_send_cm_service_ind(reg_state_service_state);
    }
#ifdef FEATURE_LTE
    reg_mode_reset_addtnl_info(SYS_ADDTNL_INFO_EMERG_CALL);
#endif

#ifdef FEATURE_FEMTO_CSG
    if((reg_state_power_on_csg_search == TRUE) && 
      (msg_p->only_for_reg == FALSE) && 
      (reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) &&
      (reg_sim_get_csg_list_read_complete() == TRUE))
    {
      MSG_HIGH_DS_1(REG_SUB, "=REG= config-%d",reg_state_csg_selection_config);
      if((reg_state_csg_selection_config == SYS_CSG_SELECTION_CONFIG_1) &&
           ((reg_state_service_state.csg_info.csg_id == SYS_CSG_ID_INVALID) || 
           ((reg_state_service_state.csg_info.hybrid_cell == TRUE) && 
             reg_state_service_state.csg_in_white_list == FALSE)))
      {
        reg_send_cm_csg_search_trigger_ind();
      }
      reg_state_power_on_csg_search = FALSE; 
    }
#endif 

  }

  reg_last_mm_message_only_for_reg = msg_p->only_for_reg;

  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_emergency_num_list_ind

==============================================================================*/

reg_state_p_type reg_state_process_mmr_emergency_num_list_ind
(
  mmr_emergency_num_list_ind_s_type* msg_p
)

{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Received MMR_EMERGENCY_NUM_LIST_IND");
  reg_send_cm_emergency_num_list_ind((cm_emergency_num_list_ind_s_type*)msg_p);
  return reg_state_curr_p;
}

void reg_state_process_sim_refresh_ind
(
  sim_refresh_ind_s_type* msg_p
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Processing SIM_REFRESH_IND");
  reg_sim_process_refresh_ind(
  msg_p
    );
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_sim_refresh_fcn_ind

==============================================================================*/
reg_state_p_type reg_state_process_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;

#ifdef FEATURE_ENHANCED_NW_SELECTION
  uint8            ens_flag;

  ens_flag = reg_sim_read_ens_flag();
  
#endif
    switch(msg_p->file_id)
    {
      case MMGSDI_USIM_HPLMN:
      case MMGSDI_GSM_HPLMN:
        reg_state_process_fcn_hplmn();
        break;
#ifdef FEATURE_ENHANCED_NW_SELECTION
      case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
      case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
        if (ens_flag & GSDI_SUPPORT_ACTING_HPLMN)
        {
          reg_state_process_fcn_act_hplmn(TRUE);
        }
        break;
#endif
      case MMGSDI_USIM_FPLMN:
      case MMGSDI_GSM_FPLMN:
      case MMGSDI_USIM_HPLMNWACT:
      case MMGSDI_GSM_HPLMNACT:
      case MMGSDI_USIM_RPLMNACT:
        MSG_HIGH_DS_1(REG_SUB, "=REG= No need to process FCN (%d) in current state", (int) msg_p->file_id);
        break;
      default:
        MSG_HIGH_DS_1(REG_SUB, "=REG= Wrong FCN Received file - %d", msg_p->file_id);
        break;
    }

  return next_state_p;
}

void reg_state_process_sim_subscription_ready
(
  sim_subs_ready_ind_s_type* msg_p
)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
	  mmgsdi_session_info_query_type	session_info_query;
	  mmgsdi_session_get_info_type		session_get_info_ptr;
#endif

  reg_state_p_type next_state_p = reg_state_curr_p;

  reg_waiting_for_open_session_cnf = FALSE;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
	/* Read the current slot_id */
	session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
	session_info_query.query_key.session_type = reg_sim_mmgsdi_info.session_type;
	if (MMGSDI_SUCCESS != mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr))
	{
	  MSG_ERROR_DS(MM_SUB, "=MM= SIM card mode failed!!!", 0,0,0);
	}
	MSG_HIGH_DS_1(REG_SUB,"slot id received is", session_get_info_ptr.slot_id);
        reg_sim_mmgsdi_info.slot_id = session_get_info_ptr.slot_id;
#endif
  if(reg_need_to_process_sim_avail_req)
  {
    reg_need_to_process_sim_avail_req = FALSE;
    if (reg_sim_load_card_mode())
    {
      /*
      ** Read the equivalent PLMN list from NVRAM.
      */
      reg_mode_initialize_equivalent_plmn_list
      (
#ifdef FEATURE_ENHANCED_NW_SELECTION
        FALSE
#endif
      );
  #ifdef FEATURE_LTE
      reg_send_eq_plmn_change_ind();
  #endif
      reg_mode_t3245_efnas_config_behavior();
#ifdef FEATURE_TDSCDMA
      reg_timers_stop_validate_sim_timer();
#endif

      /* Populate enhanced HPLMN search tbl */
      reg_mode_enhanced_hplmn_srch_init();
#ifdef FEATURE_LTE 
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
#endif

    }
    else
    {
      reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id);
    }
  }

  reg_state_next_p = next_state_p;
}

void reg_state_process_sim_client_id
(
  sim_client_id_ind_s_type* msg_p
)
{
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  uint8 index;
  reg_as_id = REG_AS_ID_1;
  for ( index = 0; index < reg_sim_max_subscription; index++ )
  {
    reg_sub_id = reg_sub_id_stack[reg_as_id];
    reg_sim_mmgsdi_info.client_id = msg_p->cid;
    if (!reg_sim_open_session() ||
  
        !mm_sim_open_session
           (
             reg_sim_mmgsdi_info.session_type,
             reg_sim_mmgsdi_info.client_id,
             (sys_modem_as_id_e_type)reg_as_id
           )
       )
    {
      MSG_FATAL_DS(REG_SUB, "=REG= MMGSDI open session failed!", 0, 0, 0);
    }
    reg_as_id++;
  }
  reg_as_id=REG_AS_ID_1;
  reg_sub_id = reg_sub_id_stack[reg_as_id];
#else
  reg_sim_mmgsdi_info.client_id = msg_p->cid;
  if (!reg_sim_open_session() ||
  
      !mm_sim_open_session
         (
           reg_sim_mmgsdi_info.session_type,
           reg_sim_mmgsdi_info.client_id
         )
     )
  {
    MSG_FATAL_DS(REG_SUB, "=REG= MMGSDI open session failed!", 0, 0, 0);
  }
#endif
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_sim_card_unavailable

==============================================================================*/
void reg_state_process_sim_card_unavailable(void)
{
#ifdef FEATURE_DUAL_SIM
  if(reg_is_t3245_timer_active((sys_modem_as_id_e_type)reg_as_id))
#else
  if(reg_is_t3245_timer_active())
#endif
  {
    reg_nv_write_t3245_to_efs(reg_timers_get_rem_t3245_value());
    reg_timers_stop_t3245();
  }
#ifdef FEATURE_TDSCDMA
  reg_timers_stop_validate_sim_timer();
#endif
}


#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_state_process_fcn_act_hplmn

==============================================================================*/
void reg_state_process_fcn_act_hplmn
(
  boolean fcn_delay
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Processing FCN - ACT HPLMN");
  reg_mode_initialize_equivalent_plmn_list(TRUE);
  reg_send_eq_plmn_change_ind();
  /*
  ** Read Act-HPLMN and remove the Act-HPLMN from FPLMN list, if present.
  */

#ifdef FEATURE_DUAL_SIM
  if (reg_sim_remove_plmn_from_fplmn_list(reg_sim_read_hplmn(), (sys_modem_as_id_e_type)reg_sub_id))
#else
  if (reg_sim_remove_plmn_from_fplmn_list( reg_sim_read_hplmn()))
#endif /*FEATURE_DUAL_SIM*/
  {
  #ifdef FEATURE_LTE
    reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
  #endif /*FEATURE_LTE*/
  }

  /*
  ** Delay the processing of FCN - Acting HPLMN as REG is
  ** now middle of REGISTERING or PLMN SEARCHING.
  */
  reg_state_delay_fcn_act_hplmn = fcn_delay;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_process_fcn_hplmn

==============================================================================*/
void reg_state_process_fcn_hplmn
(
  void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Processing FCN - HPLMN Timer");

  if (!reg_timers_restart_hplmn_search_period_timer()) // if running
  {
    /*
    ** Initially it may not be started when in camped on VPLMN,
    ** because timer was set to 0. Now Timer is changes do start timer.
    */
    if (((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) ||
        (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
        (reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
        ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
        (reg_state_service_state.roaming_ind == SYS_ROAM_STATUS_ON))
    {
      reg_timers_start_hplmn_search_period_timer();
    }
  }
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_reg_reject_ind

==============================================================================*/

reg_state_p_type reg_state_process_mmr_reg_reject_ind
(
  mmr_reg_reject_ind_s_type* msg_p
)

{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Received MMR_REG_REJECT_IND");
  reg_send_cm_reg_reject_ind(
       msg_p->reject_domain,
       msg_p->reject_cause, 
       msg_p->plmn,
       msg_p->tac
   #ifdef FEATURE_FEMTO_CSG
       ,msg_p->csg_id
   #endif
       , msg_p->reject_rat
      );

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_reg_reject_ind

==============================================================================*/
reg_state_p_type reg_state_unexpected_mmr_reg_reject_ind
(
  mmr_reg_reject_ind_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_REJECT_IND - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_handle_mmr_reg_reject_ind

==============================================================================*/
reg_state_p_type reg_state_handle_mmr_reg_reject_ind 
(
  mmr_reg_reject_ind_s_type* msg_p
)
{
  if((reg_last_recieved_list_req_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND) ||
      (reg_last_recieved_list_req_type == REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND))
  {
    return reg_state_process_mmr_reg_reject_ind(msg_p);
  }
  else
  {
    return reg_state_unexpected_mmr_reg_reject_ind(msg_p);
  }
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_sim_refresh_fcn_ind

==============================================================================*/
reg_state_p_type reg_state_unexpected_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= SIM_REFRESH_FCN - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_cell_service_ind

==============================================================================*/
void reg_state_process_cell_service_ind(mmr_cell_service_ind_s_type* mmr_ind)
{
  cm_cell_service_ind_s_type cm_cell_service_ind;

  memset(&cm_cell_service_ind, 0x00, sizeof(cm_cell_service_ind_s_type));
  cm_cell_service_ind.hsdpa_hsupa_support = mmr_ind->hsdpa_hsupa_support;
  cm_cell_service_ind.dtm_support = mmr_ind->dtm_support;
  cm_cell_service_ind.egprs_support = mmr_ind->egprs_support;

  reg_send_cm_cell_service_ind(&cm_cell_service_ind);
}

#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*==============================================================================

FUNCTION NAME

  reg_state_process_cell_service_ind

==============================================================================*/
void reg_state_process_hspa_call_status_ind(const mmr_hspa_call_status_ind_s_type* mmr_ind)
{
  cm_hspa_call_status_ind_s_type cm_hspa_call_status_ind;

  cm_hspa_call_status_ind.hs_call_status_ind = mmr_ind->hs_call_status_ind;

  reg_send_cm_hspa_call_status_ind(&cm_hspa_call_status_ind);
}
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
/*==============================================================================

FUNCTION NAME

  reg_state_process_conn_mode_service_status_ind

==============================================================================*/
void reg_state_process_conn_mode_service_status_ind
(
  const mmr_conn_mode_service_status_ind_s_type* mmr_ind
)
{
  cm_conn_mode_ss_ind_s_type cm_conn_mode_ss_ind;
#ifndef FEATURE_EQUIVALENT_HPLMN
  sys_plmn_id_s_type hplmn;
  hplmn = reg_sim_read_hplmn();
#endif
  reg_mode_update_service_available_rat(SYS_RAT_NONE);   /* First reset the RAT value to NONE */
  if(mmr_ind->conn_mode_ss_ind.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    reg_mode_update_service_available_rat(mmr_ind->conn_mode_ss_ind.rat); 
  }

  cm_conn_mode_ss_ind.conn_mode_ss_ind = mmr_ind->conn_mode_ss_ind;
  if((reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED ||
      reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED_REGIONAL) &&
     mmr_ind->conn_mode_ss_ind.service_status == SYS_SRV_STATUS_SRV)
  {
    cm_conn_mode_ss_ind.conn_mode_ss_ind.service_status = reg_state_service_status_get();
    MSG_HIGH_DS_2(REG_SUB, "=REG= CONN_MODE_SS_IND. SRV status chgd frm %d to %d",
            mmr_ind->conn_mode_ss_ind.service_status,
            cm_conn_mode_ss_ind.conn_mode_ss_ind.service_status);
  }
  else if ((reg_state_service_status_get() == SYS_SRV_STATUS_NO_SRV) &&
           (mmr_ind->conn_mode_ss_ind.service_status == SYS_SRV_STATUS_SRV))
  {
    cm_conn_mode_ss_ind.conn_mode_ss_ind.service_status = SYS_SRV_STATUS_LIMITED;
    MSG_HIGH_DS_2(REG_SUB, "=REG= CONN_MODE_SS_IND. SRV status chgd frm %d to %d",
            mmr_ind->conn_mode_ss_ind.service_status,
            cm_conn_mode_ss_ind.conn_mode_ss_ind.service_status);    
  }
  if ( !sys_plmn_id_is_undefined(cm_conn_mode_ss_ind.conn_mode_ss_ind.selected_plmn) )
  {
#ifndef FEATURE_EQUIVALENT_HPLMN
    if ( sys_plmn_match(hplmn, cm_conn_mode_ss_ind.conn_mode_ss_ind.selected_plmn) )
#else
    if ( reg_sim_is_ehplmn(cm_conn_mode_ss_ind.conn_mode_ss_ind.selected_plmn) ) 
#endif
    {
      cm_conn_mode_ss_ind.conn_mode_ss_ind.roam_status= SYS_ROAM_STATUS_OFF;
    }
    else
    {
      cm_conn_mode_ss_ind.conn_mode_ss_ind.roam_status = SYS_ROAM_STATUS_ON;
    }
    if ( sys_eplmn_list_equivalent_plmn(cm_conn_mode_ss_ind.conn_mode_ss_ind.selected_plmn)&&
#ifdef FEATURE_EQUIVALENT_HPLMN
        reg_sim_is_ehplmn_in_eplmn()
#else
        sys_eplmn_list_equivalent_plmn(hplmn)
#endif
     )
    {
      cm_conn_mode_ss_ind.conn_mode_ss_ind.roam_status= SYS_ROAM_STATUS_OFF;
    }
  }
    

  reg_send_cm_conn_mode_ss_ind(&cm_conn_mode_ss_ind);
}
#endif

#ifdef FEATURE_LTE

boolean reg_state_get_force_pref_pending(void)
{
  MSG_HIGH_DS_1(REG_SUB, "=REG= force pref pending set to %d ", reg_state_force_pref_pending);
  return reg_state_force_pref_pending;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_block_plmn_req

==============================================================================*/
reg_state_p_type reg_state_process_cm_block_plmn_req
(
  const cm_block_plmn_req_s_type *msg_p
)
{
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;  
  sys_plmn_get_mcc_mnc
  (
    msg_p->plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );
#ifdef FEATURE_DUAL_SIM
  if (reg_multimode_sub != msg_p->as_id)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= PLMN blocking ignored on non mutimode SUB, multimode SUB %d", reg_multimode_sub);
    return reg_state_curr_p;
  }
#endif
  
  MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN ID(%d-%d)", mcc, mnc);
  MSG_HIGH_DS_2(REG_SUB, "=REG= type = %d timer count = %d",msg_p->type, msg_p->timer_count);

  switch(msg_p->type)
  {
    case SYS_BLOCK_PLMN_BLOCK:
      if (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH)
      {
        reg_send_mmr_plmn_search_abort_req();
        if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
        {
          reg_timers_start_hplmn_search_timer_10sec();
        }
        else
        {
          /* Roaming is off if the IRAT HPLMN search is ongoing*/
          reg_state_start_irat_timers_with_10sec();
        }
      }
      reg_send_mmr_block_plmn_req(msg_p->plmn, msg_p->timer_count, FALSE,msg_p->cause); 
      break;

    case SYS_BLOCK_PLMN_UNBLOCK:
      if (reg_sim_remove_plmn_from_backoff_fplmn_list(msg_p->plmn,msg_p->cause,(sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
      }
      break;
   case SYS_BLOCK_PLMN_UNBLOCK_FORCE_PREF:
     {
        if(reg_state_curr_p == &reg_state_automatic_search || reg_state_curr_p == &reg_state_registering)
        {
          if (reg_sim_remove_plmn_from_backoff_fplmn_list(msg_p->plmn,SYS_BLOCK_PLMN_CAUSE_PRIORITY,
                                                          (sys_modem_as_id_e_type)reg_sub_id))
          {
             rex_timer_cnt_type timer_count = 0;
             sys_block_plmn_cause_e_type cause = SYS_BLOCK_PLMN_CAUSE_NONE;
             if(reg_mode_check_eplmn_backoff_forbidden(&msg_p->plmn,&timer_count,&cause,(sys_modem_as_id_e_type)reg_sub_id))
             {
               reg_mode_remove_eplmn_rplmn_backoff_forbidden(&msg_p->plmn,cause);
             }
             reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
             if(reg_state_curr_p == &reg_state_automatic_search)
             {
               reg_send_mmr_plmn_search_abort_req();
               reg_state_curr_p = &reg_state_idle;
             }
             reg_sim_write_rplmn_rat(SYS_RAT_NONE);
             if(((reg_mode_is_only_req_rat_present_in_rat_pri_list(reg_mode_curr_rat_pri_list_get(),SYS_SYS_MODE_LTE) == TRUE )&&
                (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION || 
                 reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) ||
                 reg_mode_reenable_lte())
             {
               reg_sim_write_rplmn_rat(SYS_RAT_NONE);
               MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to start plmn selection all over again");
               if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
                  reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION )
               {
                 reg_mode_start_plmn_selection
                (
                  reg_mode_network_selection_mode_get(),
                  reg_mode_req_mode_pref_get(),
                  reg_mode_cs_for_request_get(),
                  reg_mode_ps_for_request_get(),
                  reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
                 ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
                  ,reg_mode_rat_pri_list_get()
#endif
                  ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
                  ,reg_mode_sms_only_get(),
                   0
#endif 
                 );
                 reg_state_force_pref_pending = FALSE;
                 reg_state_curr_p = &reg_state_registering;
               }
               else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                       reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
               {
                 reg_mode_start_manual_plmn_selection
                (
                  reg_mode_network_selection_mode_get(),
                  reg_send_last_reg_req_plmn_get(),
                  reg_mode_mode_pref_get(),
                  FALSE,
                  FALSE,
                  reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
                  reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
                  reg_mode_rat_pri_list_get(),
#endif
                  REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
                  ,reg_mode_sms_only_get(),
                  0
#endif                 
                 );
                 reg_state_force_pref_pending = FALSE;
                 reg_state_curr_p = &reg_state_registering;
               } 
               reg_state_update_service_state_if_required(reg_state_curr_p);
             }
             else
             { 
               reg_state_force_pref_pending = TRUE;
             }
           }
           else
           {
              reg_sim_write_rplmn_rat(SYS_RAT_NONE);
              if (reg_sim_remove_plmn_from_backoff_fplmn_list(msg_p->plmn,SYS_BLOCK_PLMN_CAUSE_PRIORITY,
                                                              (sys_modem_as_id_e_type)reg_sub_id))
              {
                rex_timer_cnt_type timer_count = 0;
                sys_block_plmn_cause_e_type cause = SYS_BLOCK_PLMN_CAUSE_NONE;
                if(reg_mode_check_eplmn_backoff_forbidden(&msg_p->plmn,&timer_count,&cause,(sys_modem_as_id_e_type)reg_sub_id))
                {
                  reg_mode_remove_eplmn_rplmn_backoff_forbidden(&msg_p->plmn,cause);
                }
                reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
              }
              MSG_HIGH_DS_0(REG_SUB, "=REG= RPLMN Act deleated on UNBLOCK FORCE PREF ");
            }
         }
     }
     break;
    case SYS_BLOCK_PLMN_RESET:
      if(reg_sim_backoff_fplmn_list.length != 0)
      {
        reg_sim_clear_backoff_fplmn_list();
        reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
      }
      break;

    default:
      break;
  }
  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_backoff_fplmn_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_process_backoff_fplmn_timer_expiration(void)
{
  reg_sim_handle_backoff_fplmn_search_timer_expiry();
  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_update_lte_cap_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_process_update_lte_cap_timer_expiration(void)
{
  if(reg_mode_update_curr_rat_pri_list(&reg_state_service_state.plmn,
                                    reg_state_service_state.active_rat))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to update LTE RAT capability");
    if(reg_state_force_pref_pending == TRUE)                                            
    {
       reg_sim_write_rplmn_rat(SYS_RAT_NONE);
       MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to start plmn selection all over again from LTE");
       if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC || 
          reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
       {
          reg_mode_start_plmn_selection
          (
             reg_mode_network_selection_mode_get(),
             reg_mode_req_mode_pref_get(),
             reg_mode_cs_for_request_get(),
             reg_mode_ps_for_request_get(),
             reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
             ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
             ,reg_mode_rat_pri_list_get()
#endif
             ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get(),
             0
#endif 
             );
            reg_state_force_pref_pending = FALSE ;
            reg_state_curr_p = &reg_state_registering;
         }
         else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
                 reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )
         {
           reg_mode_start_manual_plmn_selection
          (
             reg_mode_network_selection_mode_get(),
             reg_send_last_reg_req_plmn_get(),
             reg_mode_mode_pref_get(),
             FALSE,
             FALSE,
             reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
             reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
             reg_mode_rat_pri_list_get(),
#endif
             REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
             ,reg_mode_sms_only_get(),
             0
#endif                 
             );
             reg_state_force_pref_pending = FALSE ;
             reg_state_curr_p = &reg_state_registering;
           } 
         reg_state_update_service_state_if_required(reg_state_curr_p);

    }
    else
    {
      reg_mode_start_plmn_selection_on_specified_plmn
      (
        reg_state_service_state.plmn,
        reg_mode_network_selection_mode_get(),
        reg_mode_mode_pref_get(),
        FALSE,
        FALSE,
        reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
        reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        reg_mode_curr_rat_pri_list_get(),
#endif
        reg_state_service_state.active_rat
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
        0
#endif 
        ,MMR_REG_REQ_NORMAL
      );
      return &reg_state_registering;
    }
  }
  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_ignore_update_lte_cap_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_ignore_update_lte_cap_timer_expiration(void)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= UPDATE LTE CAP TIMER expired - Ignored/Unexpected");
  return reg_state_curr_p;
}


#endif


/*==============================================================================

FUNCTION NAME

  reg_state_process_set_hplmn_search_req

==============================================================================*/
void reg_state_process_set_hplmn_search_req
(
  const cm_set_hplmn_search_timer_req_s_type *msg_p
)
{

  MSG_HIGH_DS_1(REG_SUB, "=REG= Set HPLMN timer request: Timer = %d", msg_p->timer_value);

  reg_timers_set_hplmn_search_timer(msg_p->timer_value);


}

/*==============================================================================

FUNCTION NAME

  reg_state_process_set_hplmn_search_req

==============================================================================*/
void reg_state_process_get_hplmn_search_req
(
  const cm_get_hplmn_search_timer_req_s_type *msg_p
)
{
  uint32 timer_value = reg_timers_get_hplmn_search_timer();

  reg_send_cm_get_hplmn_search_timer_cnf(timer_value);
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_network_list_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_NETWORK_LIST_REQ - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_service_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_service_req
(
  const cm_service_req_s_type* msg_p
)
{
  sys_plmn_service_state_s_type service_state;

  (void) msg_p;
  memset(&service_state, 0x00, sizeof(sys_plmn_service_state_s_type));

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - Unexpected");

  service_state.sim_state               = reg_sim_state_get();
  service_state.plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
  service_state.plmn_forbidden          = FALSE;
  service_state.roaming_ind             = SYS_ROAM_STATUS_OFF;
  service_state.service_status          = SYS_SRV_STATUS_NO_SRV;
  service_state.reg_service_domain      = SYS_SRV_DOMAIN_NO_SRV;
  service_state.active_rat              = SYS_RAT_UMTS_RADIO_ACCESS;
#ifdef FEATURE_GSM_BAND_PREF
  service_state.active_band.active_band     = SYS_BAND_MASK_EMPTY;
  service_state.lac                     = 0xFFFE;
#endif
  service_state.ps_data_suspend         = FALSE;

  sys_plmn_undefine_plmn_id(&service_state.plmn);

  reg_send_cm_service_cnf
  (
    reg_state_cm_transaction_id,
    service_state
  );

  reg_state_cm_initiated_registration = FALSE;

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_sim_available_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_sim_not_available_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_SIM_NOT_AVAILABLE_REQ - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_stop_mode_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_stop_mode_req
(
  const cm_stop_mode_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_STOP_MODE_REQ - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_ph_status_chgd_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_ph_status_chgd_req
(
  const cm_ph_status_chgd_req_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= CM_PH_STATUS_CHGD_REQ - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_camped_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_camped_ind
(
  mmr_camped_ind_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_CAMPED_IND - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_attach_failed_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_attach_failed_ind             
(
  void
)
{

  MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_ATTACH_FAILED_IND - Unexpected");

  return reg_state_curr_p;
}
/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_reg_req_ignored_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_reg_req_ignored_ind             
(
  void
)
{

  MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_REQ_IGNORED_IND - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_reg_req_ignored_ind

==============================================================================*/
reg_state_p_type reg_state_process_mmr_reg_req_ignored_ind               
(
  void
)
{
  reg_send_last_mmr_reg_req();
  return reg_state_curr_p;
}


/*==============================================================================

                                                 FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_attach_failed_ind

==============================================================================*/

reg_state_p_type reg_state_process_mmr_attach_failed_ind               
(
  void
)
{
  if( ((reg_state_curr_p != &reg_state_registering) || 
       (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID))
#ifdef FEATURE_SGLTE
        &&(!IS_SGLTE_MERGE_IN_PROGRESS)
#endif    
      
    )
  {
    reg_send_cm_attach_failed_ind();
  }
  else
  {
    reg_state_unexpected_mmr_attach_failed_ind();
  }
  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_mm_information_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_mm_information_ind
(
  const mmr_mm_information_ind_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_MM_INFORMATION_IND - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_plmn_search_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_plmn_search_cnf
(
  const mmr_plmn_search_cnf_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_PLMN_SEARCH_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_reg_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_service_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_sim_available_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_sim_available_cnf
(
  mmr_sim_available_cnf_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SIM_AVAILABLE_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_sim_not_available_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_sim_not_available_cnf
(
  mmr_sim_not_available_cnf_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SIM_NOT_AVAILABLE_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_stop_mode_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_stop_mode_cnf
(
  mmr_stop_mode_cnf_s_type* msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_STOP_MODE_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_hplmn_search_timer_expiration

==============================================================================*/

reg_state_p_type reg_state_unexpected_hplmn_search_timer_expiration
(
  void
)
{
  MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer - Unexpected");

  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_hplmn_irat_search_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_unexpected_hplmn_irat_search_timer_expiration
(
  void
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;
  MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN  IRAT Search Timer expiration - Unexpected");
  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_mmr_emergency_num_list_ind

==============================================================================*/

reg_state_p_type reg_state_unexpected_mmr_emergency_num_list_ind
(
  mmr_emergency_num_list_ind_s_type* msg_p
)
{
  (void) msg_p;
  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_EMERGENCY_NUM_LIST_IND - Unexpected");

  return reg_state_curr_p;
}

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_backoff_fplmn_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_unexpected_backoff_fplmn_timer_expiration(void)
{
  MSG_ERROR_DS_0(REG_SUB, "=REG= Backoff plmn Search Timer - Unexpected");

  return reg_state_curr_p;
}

#endif


#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

FUNCTION NAME

  reg_state_ignore_hplmn_search_request

==============================================================================*/

reg_state_p_type reg_state_ignore_hplmn_search_request
(
  const cm_hplmn_search_req_s_type* msg_p
)
{
  (void) msg_p;
  
  MSG_HIGH_DS_1(REG_SUB, "=REG= Ignore: HPLMN Search Request - REG is in Non-IDLE state %d", reg_state_curr_p->number());

  reg_send_cm_hplmn_search_cnf(FALSE);
  
  return reg_state_curr_p;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_ph_status_change_cnf

==============================================================================*/

reg_state_p_type reg_state_unexpected_ph_status_change_cnf
(
  mmr_ph_status_change_cnf_s_type * msg_p
)
{
  (void) msg_p;

  MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_PH_STATUS_CHANGE_CNF - Unexpected");

  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_hplmn_irat_search_timer_expiration

==============================================================================*/
reg_state_p_type reg_state_unexpected_lte_irat_search_timer_expiration
(
  void
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;
  MSG_ERROR_DS_0(REG_SUB, "=REG= LTE  IRAT Search Timer expiration - Unexpected");
  return next_state_p;
}


#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
void reg_state_process_lte_cell_info_ind(mmr_lte_cell_info_ind_s_type mmr_lte_cell_info_ind)
{
  reg_state_lte_cell_info_ind = mmr_lte_cell_info_ind.lte_cell_info;
  MSG_HIGH_DS_1(REG_SUB, "=REG= MMR_LTE_CELL_INFO_IND emergency_access_barred = %d",
                reg_state_lte_cell_info_ind.emergency_access_barred);
}
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

reg_as_id_e_type  reg_get_stack_id(sys_modem_stack_id_e_type  stack_id)
{
  reg_as_id_e_type as_id;
#ifdef FEATURE_SGLTE
  if(REG_SUB_IS_SGLTE)
  {
    if(stack_id == SYS_MODEM_STACK_ID_1)
    {
      as_id = reg_ps_stack_id;
    }
    else
    {
      as_id = reg_cs_stack_id;
    }
  }
  else
#endif 
  {
    as_id = reg_sub_id;
  }
   return as_id;
}

/*==============================================================================

FUNCTION NAME

  reg_get_sub_id 

  This function validates and sets the sub_id.
==============================================================================*/
boolean reg_get_sub_id
(
  sys_modem_as_id_e_type sub_id,
  IMH_T hdr
)
{
  reg_sub_id = (reg_as_id_e_type)sub_id;
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_3(REG_SUB, "=REG=Invalid SUB ID:%d - Ignoring message MS: %d   MSG_ID: %d", 
                   sub_id+1,hdr.message_set, hdr.message_id);
    return FALSE;
  }
  return TRUE;
}

/*==============================================================================

FUNCTION NAME

  reg_get_as_id 

  This function will extract SIM_ID from all incoming primitives and enables  processing of  the primitive 
  in the context of extracted SIM_ID

==============================================================================*/

boolean  reg_get_as_id(reg_cmd_type* reg_cmd_p)
{
  boolean status = TRUE;

  /*Decode SIM_ID from each incoming primitive */
  switch( reg_cmd_p->cmd.hdr.message_set )
      {

        /*
        ** MESSAGE FROM CM
        */
        case MS_CM_REG:
          switch( reg_cmd_p->cmd.hdr.message_id )
          {
            case CM_NETWORK_LIST_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_network_list_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_network_list_req.stack_id);
                reg_msg_info = (byte)reg_cmd_p->cmd.cm_network_list_req.list_search_type;
              }
              break;
            
            case CM_SERVICE_REQ:
             if(reg_get_sub_id(reg_cmd_p->cmd.cm_service_req.as_id,reg_cmd_p->cmd.hdr))
             {
             
               /* Making crash for any wrong request on hybrid.*/
#ifdef FEATURE_SGLTE
               if ((REG_SUB_IS_SGLTE) &&
                    ((reg_sglte_ue_mode==SYS_UE_MODE_SGLTE_ROAM)||
                     (reg_sglte_ue_mode==SYS_UE_MODE_CSFB_ONLY))&&
                    (reg_cmd_p->cmd.cm_service_req.stack_id == (SYS_MODEM_STACK_ID_2)) && (!IS_SGLTE_SPLIT_IN_PROGRESS))
               {
                  MSG_FATAL_DS(REG_SUB, "=REG= CM send Service Req on Hybrid stack when not in SGLTE_HOME", 0, 0, 0);
               }
               if ((REG_SUB_IS_SGLTE) &&
                   (reg_cmd_p->cmd.cm_service_req.stack_id == (SYS_MODEM_STACK_ID_2)) &&
                   (IS_SGLTE_MERGE_IN_PROGRESS))
               {
                  MSG_HIGH_DS_0(REG_SUB, "=REG= Service request on Hybrid stack in SGLTE mode during merge - ignoring the service to CM");
                  return FALSE;
               }
#endif
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_service_req.stack_id);

           /*reg_msg_info : bits 2-0 : network_selection_mode
                            bits 7-3 : type              */

                reg_msg_info = (byte)((reg_cmd_p->cmd.cm_service_req.network_selection_mode)&0x07);
                if(reg_msg_info == SYS_NETWORK_SELECTION_MODE_LIMITED)
                {
                  reg_msg_info = (reg_msg_info |(((reg_cmd_p->cmd.cm_service_req.parameters.limited.type)&0x1F)<<3));
                }
                else if((reg_msg_info == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                        (reg_msg_info == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) ||
                        (reg_msg_info == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
                {  
                  reg_msg_info = (reg_msg_info |(((reg_cmd_p->cmd.cm_service_req.parameters.manual.type)&0x1F)<<3));
                }
                else
                {
                   reg_msg_info = (reg_msg_info |(((reg_cmd_p->cmd.cm_service_req.parameters.automatic.type)&0x1F)<<3));
                }
              }
              break;

            case CM_SIM_AVAILABLE_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_sim_available_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_sim_available_req.stack_id);
              }
              break;

            case CM_SIM_NOT_AVAILABLE_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_sim_not_available_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_sim_not_available_req.stack_id);
              }
              break;

            case CM_STOP_MODE_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_stop_mode_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_sub_id = (reg_as_id_e_type)reg_cmd_p->cmd.cm_stop_mode_req.as_id;
                /* Making crash for any wrong request on hybrid - if single stack but not if split in progress or if merge in progress */
#ifdef FEATURE_SGLTE
                if ((REG_SUB_IS_SGLTE) && (reg_cmd_p->cmd.cm_stop_mode_req.stack_id == SYS_MODEM_STACK_ID_2) && 
                    (((!REG_SGLTE_DUAL_STACK) && (!IS_SGLTE_SPLIT_IN_PROGRESS)) ||
                     (IS_SGLTE_MERGE_IN_PROGRESS)))
                {
                  MSG_HIGH_DS_1(REG_SUB,"=REG= Looping back stop mode cnf on hybrid stack as current mode is single stack or merge in progress%d",reg_sglte_ue_mode);
                  /* derive stack ID so that we loop back on the same stack */
                  reg_as_id = REG_SGLTE_STACK_2;
                  reg_send_cm_stop_mode_cnf(reg_cmd_p->cmd.cm_stop_mode_req.transaction_id);
                  return FALSE;
                }
                else if((REG_SUB_IS_SGLTE) && (IS_SGLTE_SPLIT_IN_PROGRESS) &&
                        (reg_cmd_p->cmd.cm_stop_mode_req.stack_id == SYS_MODEM_STACK_ID_2))
                {
                  /*While split is in progress,stop mode request received on hybrid is routed to hybrid stack and pended until split completion*/
                  reg_as_id = REG_SGLTE_STACK_2;
                }
                else
#endif
                {
                  reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_stop_mode_req.stack_id);
                }
                reg_msg_info =(byte)(reg_cmd_p->cmd.cm_stop_mode_req.stop_mode_reason);
              }
              break;

            case CM_PH_STATUS_CHGD_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_ph_status_chgd_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_ph_status_chgd_req.stack_id);
                reg_msg_info = (byte)(reg_cmd_p->cmd.cm_ph_status_chgd_req.oprt_mode);
              }
              break;

#ifdef FEATURE_OOSC_USER_ACTION
            case CM_EXIT_FROM_PWR_SAVE:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_exit_from_pwr_save.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_exit_from_pwr_save.as_id);
              }
              break;
#endif
  
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
            case CM_HPLMN_SEARCH_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_hplmn_search_req.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_get_stack_id(reg_cmd_p->cmd.cm_hplmn_search_req.as_id);
              }
              break;
#endif
            case CM_DS_STAT_CHGD_REQ:/*NO SIM ID*/
              reg_msg_info =(byte)(reg_cmd_p->cmd.cm_ds_stat_chgd_req.active_subs);
              break;

#ifdef FEATURE_DUAL_SIM
            case CM_DS_TUNEAWAY_STATUS_CHANGE_IND:/*NO SIM ID*/
              break;

            case CM_MULTIMODE_SUBS_CHGD_REQ:/*NO SIM ID*/
              break;

            case CM_DDS_SWITCH_IND: /*NO SIM ID*/
              break;
#endif

#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)) || defined(FEATURE_SGLTE)
            case CM_PS_DETACH_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_ps_detach_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
                reg_msg_info =(byte)(reg_cmd_p->cmd.cm_ps_detach_req.detach_type);
              }
              break;
            case CM_DATA_ROAMING_IND:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_data_roaming_ind.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_BLOCK_PLMN_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_block_plmn_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_SET_DRX_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_set_drx_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_GET_DRX_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_get_drx_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_set_hplmn_irat_search_timer_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_SET_HPLMN_SEARCH_TIMER_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_set_hplmn_search_timer_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            case CM_GET_HPLMN_SEARCH_TIMER_REQ:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_get_hplmn_search_timer_req.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
             break;
            case CM_UE_MODE_IND:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_ue_mode_ind.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_sub_id;
                reg_msg_info =(byte)(reg_cmd_p->cmd.cm_ue_mode_ind.ue_mode);
              }
              break;
#ifdef FEATURE_DUAL_DATA
            case CM_DATA_PRIORITY_IND:
               if(reg_get_sub_id(reg_cmd_p->cmd.cm_data_prio_ind.as_id,reg_cmd_p->cmd.hdr))
              {
                reg_as_id = reg_sub_id;
              }
              break;
#endif
#endif

#ifdef FEATURE_FEMTO_CSG
            case CM_CSG_SELECT_CONFIG_UPDATE_IND:
              reg_as_id = reg_cmd_p->cmd.cm_csg_select_config_update_ind.as_id;
              break;
#endif

            case CM_PSM_READY_REQ:
              reg_as_id = reg_cmd_p->cmd.cm_psm_ready_req.as_id;
              break;
            case CM_ROAMING_IND:
              if(reg_get_sub_id(reg_cmd_p->cmd.cm_roaming_ind.as_id,reg_cmd_p->cmd.hdr))
              {
#ifdef FEATURE_SGLTE
                if(REG_SUB_IS_SGLTE)
                {
                  reg_as_id = reg_ps_stack_id;
                }
                else
#endif
                {
                  reg_as_id = reg_sub_id;
                }
              }
              break;
            default:
              MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown CM message (%d)", reg_cmd_p->cmd.hdr.message_id);
              break;
          }
          break;

        /*
        ** MESSAGE FROM MM
        */
        case MS_MM_REG:

          switch( reg_cmd_p->cmd.hdr.message_id )
          {
            case MMR_CAMPED_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_camped_ind.as_id;
              break;

            case MMR_MM_INFORMATION_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_mm_information_ind.as_id;
              break;

            case MMR_PLMN_SEARCH_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_plmn_search_cnf.as_id;
              break;

            case MMR_REG_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_reg_cnf.as_id;
              break;

            case MMR_SERVICE_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_service_ind.as_id;
              break;

            case MMR_SIM_AVAILABLE_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_sim_available_cnf.as_id;
              break;

            case MMR_SIM_NOT_AVAILABLE_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_sim_not_available_cnf.as_id;
              break;

            case MMR_STOP_MODE_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_stop_mode_cnf.as_id;
              break;
#if defined(FEATURE_HSDPA) || defined(FEATURE_GSM_DTM)
            case MMR_CELL_SERVICE_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_cell_service_ind.as_id;
              break;
#ifdef FEATURE_HSPA_CALL_STATUS_IND
            case MMR_HSPA_CALL_STATUS_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_hspa_call_status_ind.as_id;
              break;
#endif
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
            case MMR_CONN_MODE_SERVICE_STATUS_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_conn_mode_service_status_ind.as_id;
              break;
#endif
            case MMR_EMERGENCY_NUM_LIST_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_emergency_num_list_ind.as_id;
              break;

            case MMR_PH_STATUS_CHANGE_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_ph_status_change_cnf.as_id;
              break;

            case MMR_REG_REJECT_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_reg_reject_ind.as_id;
              break;  

            case MMR_DS_STAT_CHGD_CNF:/*NO SIM ID*/
              break;

            case MMR_MULTIMODE_SUBS_CHGD_CNF:/*NO SIM ID*/
              break;

            case MMR_CELL_ACCESS_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_cell_access_ind.as_id;
              break;
#ifdef FEATURE_LTE
            case MMR_UPDATE_CURR_RAT_LIST_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_update_curr_rat_list_ind.as_id;
              break;
            case MMR_LTE_CELL_INFO_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_lte_cell_info_ind.as_id;
               break;
            case MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_pseudo_resel_low_priority_ind.as_id;
               break;
            case MMR_RRC_SIB6_INFO_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_rrc_sib6_info_ind.as_id;
               break;
            case MMR_RRC_SIB7_INFO_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_rrc_sib7_info_ind.as_id;
               break;

#endif
#ifdef FEATURE_LTE
            case MMR_PS_DETACH_CNF:
              reg_as_id = reg_cmd_p->cmd.mmr_ps_detach_cnf.as_id;
              break;
            case MMR_SET_DRX_CNF:
               reg_as_id = reg_cmd_p->cmd.mmr_set_drx_cnf.as_id;
              break;
            case MMR_GET_DRX_CNF:
               reg_as_id = reg_cmd_p->cmd.mmr_get_drx_cnf.as_id;
              break;
            case MMR_FPLMN_LIST_CHANGE_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_fplmn_list_change_ind.as_id;
              break;
            case MMR_DISABLE_BPLMN_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_disable_bplmn_ind.as_id;
              break;
            case MMR_SUITABLE_SEARCH_END_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_suitable_search_end_ind.as_id;
              break;
#ifdef FEATURE_FEMTO_CSG
            case MMR_HOME_NODEB_NAME_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_home_nodeb_name_ind.as_id;
              break;
            case MMR_CSG_SEARCH_TRIGGER_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_csg_search_trigger_ind.as_id;
              break;
            case MMR_CSG_LIST_READ_COMPLETE_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_csg_list_read_complete_ind.as_id;
              break;
#endif
            case MMR_SERVICE_STAT_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_service_stat_ind.as_id;
               break;
            case MMR_STACK_DEACT_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_stack_deact_ind.as_id;
               break;
            case MMR_STACK_ACT_IND:
               reg_as_id = reg_cmd_p->cmd.mmr_stack_act_ind.as_id;
               break;
 #endif

            case MMR_ATTACH_FAILED_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_attach_failed_ind.as_id;
              break;

            case MMR_T3346_TIMER_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_t3346_timer_ind.as_id;
              break;

            case MMR_REG_REQ_IGNORED_IND:              
              MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_REQ_IGNORED_IND");
              reg_state_next_p = reg_state_curr_p->process_mmr_reg_req_ignored_ind();
              break;
            case MMR_PSM_READY_RSP:
              reg_as_id = reg_cmd_p->cmd.mmr_psm_ready_rsp.as_id;
              break;

            case MMR_PSM_READY_IND:
              reg_as_id = reg_cmd_p->cmd.mmr_psm_ready_ind.as_id;
              break;

            default:
              MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown MM message (%d)", reg_cmd_p->cmd.hdr.message_id);
              break;
          }
          if(!IS_NOT_VALID_STACK_ID(reg_as_id))
          {
            reg_sub_id = reg_sub_id_stack[reg_as_id];
          }
          break;

        /*
        ** TIMER EXPIRED
        */
        case MS_TIMER:

          switch(reg_cmd_p->cmd.timer_expiry.timer_id)
          {
            case REG_HPLMN_TIMER:
             reg_as_id = REG_AS_ID_1;
                break;

            case REG_HPLMN_TIMER_SIM_ID_2:
              reg_as_id = REG_AS_ID_2;
              break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
            case REG_HPLMN_TIMER_SIM_ID_3:
              reg_as_id = REG_AS_ID_3;
              break;
  
#endif
            case REG_LTE_IRAT_TIMER:
             reg_as_id = REG_AS_ID_1;
                break;
#if defined FEATURE_DUAL_SIM
            case REG_LTE_IRAT_TIMER_SIM_ID_2:
             reg_as_id = REG_AS_ID_2;
                break;
#endif
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM 
            case REG_LTE_IRAT_TIMER_SIM_ID_3:
              reg_as_id = REG_AS_ID_3;
              break;
  
#endif
            case REG_HPLMN_IRAT_TIMER:
             reg_as_id = REG_AS_ID_1;
                break;
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
            case REG_HPLMN_IRAT_TIMER_SIM_ID_2:
             reg_as_id = REG_AS_ID_2;
                break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
             case REG_HPLMN_IRAT_TIMER_SIM_ID_3:
              reg_as_id = REG_AS_ID_3;
                break;
#endif
#endif

            case REG_T3245_TIMER:
             reg_as_id = REG_AS_ID_1;
                break;

#ifdef FEATURE_DUAL_SIM 
            case REG_T3245_TIMER_SIM_ID_2:
              reg_as_id = REG_AS_ID_2;
              break;
#ifdef FEATURE_TRIPLE_SIM
            case REG_T3245_TIMER_SIM_ID_3:
              reg_as_id = REG_AS_ID_3;
              break;
#endif
#endif


            case REG_VALIDATE_SIM_TIMER:
              reg_as_id = REG_AS_ID_1;
              break;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
            case REG_VALIDATE_SIM_TIMER_SIM_ID_2:
              reg_as_id = REG_AS_ID_2;
              reg_sub_id = reg_sub_id_stack[reg_as_id];
#ifdef FEATURE_SGLTE
              if(REG_SUB_IS_SGLTE &&
                 (reg_as_id == REG_SGLTE_STACK_2))
              {
                reg_as_id = reg_cs_stack_id;
              }
#endif
              break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
            case REG_VALIDATE_SIM_TIMER_SIM_ID_3:
              reg_as_id = REG_AS_ID_3;
              reg_sub_id = reg_sub_id_stack[reg_as_id];
#ifdef FEATURE_SGLTE_DUAL_SIM
              if(REG_SUB_IS_SGLTE &&
                 (reg_as_id == REG_SGLTE_STACK_2))
              {
                reg_as_id = reg_cs_stack_id;
              }
#endif
              break;
#endif
#endif


#ifdef FEATURE_SGLTE
            case REG_SGLTE_PS_TIMER:
              reg_as_id = reg_ps_stack_id;
                break;
#endif

#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)) || defined(FEATURE_SGLTE)
            case REG_UPDATE_LTE_CAP_TIMER:
              reg_as_id = REG_AS_ID_1;
              break;

#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            case REG_UPDATE_LTE_CAP_TIMER_SIM_ID_2:
              reg_as_id = REG_AS_ID_2;
              break;
#endif

            case REG_BACKOFF_FPLMN_TIMER:
              reg_as_id = REG_AS_ID_1;
              break;

#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            case REG_BACKOFF_FPLMN_TIMER_SIM_ID_2:
              reg_as_id = REG_AS_ID_2;
              break;
#endif

#endif
            default:
              MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown timer expired id = %d", reg_cmd_p->cmd.timer_expiry.timer_id);
              break;
          }
          if(!IS_NOT_VALID_STACK_ID(reg_as_id))
          {
            reg_sub_id = reg_sub_id_stack[reg_as_id];
          }
          break;
        /*
        ** FCN Message from GSDI
        */
         case MS_GSDI:

          switch(reg_cmd_p->cmd.hdr.message_id)
          {
            case SIM_REFRESH_IND:
              reg_as_id = reg_cmd_p->cmd.sim_refresh_ind.as_id;
              break;

            case SIM_REFRESH_FCN_IND:
              reg_as_id = reg_cmd_p->cmd.sim_refresh_fcn_ind.as_id;
              break;

            case SIM_REG_SUBS_READY_IND:
              reg_as_id = reg_cmd_p->cmd.sim_subs_ready_ind.as_id;
              break;

            case SIM_REG_CLIENT_ID_IND: /*NO SIM ID*/
              reg_as_id = REG_AS_ID_1;
              break;

            case REG_SIM_CARD_UNAVAILABLE_IND:
              reg_as_id = reg_cmd_p->cmd.sim_card_unavailable_ind.as_id;
              break;

            default:
              MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown Message from GSDI = %d", reg_cmd_p->cmd.hdr.message_id);
              break;
          }
          if(!IS_NOT_VALID_STACK_ID(reg_as_id))
          {
            reg_sub_id = reg_sub_id_stack[reg_as_id];
          }
          break;

#ifdef FEATURE_NAS_ECALL
         case MS_ECALL_REG:
            switch(reg_cmd_p->cmd.hdr.message_id )
            {
              case ECALL_REG_MODE_IND:
                reg_sub_id = reg_cmd_p->cmd.ecall_reg_mode_ind.as_id;
                reg_as_id = reg_get_stack_id(SYS_MODEM_STACK_ID_2);
                break;

              default: 
                 break;
            }
#endif

         case MS_REG_REG:
           switch (reg_cmd_p->cmd.hdr.message_id)
           {
#ifdef FEATURE_MODEM_CONFIG_REFRESH
             case REG_MCFG_REFRESH_IND:
               reg_as_id = reg_cmd_p->cmd.mcfg_refresh_ind.as_id;
               break;
#endif
             default:
               MSG_ERROR_DS_1( REG_SUB, "=REG= Unknown message = %d",
                                                 reg_cmd_p->cmd.hdr.message_id);
               break;
           }
           break;

         default:
           MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown message set (%d)",
                  reg_cmd_p->cmd.hdr.message_set);
           break;
       }

  if((IS_NOT_VALID_SUB_ID(reg_sub_id))
       ||(IS_NOT_VALID_STACK_ID(reg_as_id)))
  {
    MSG_ERROR_DS_3(REG_SUB, "=REG= Invalid AS ID:%d - Ignoring message MS: %d   MSG_ID: %d", 
             reg_as_id+1,reg_cmd_p->cmd.hdr.message_set, reg_cmd_p->cmd.hdr.message_id);
    status = FALSE;

    /* leave SUB ID with a valid value */
    reg_as_id = REG_AS_ID_1;
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= MS: %d   MSG_ID: %d ", reg_cmd_p->cmd.hdr.message_set, reg_cmd_p->cmd.hdr.message_id);
  }


  return status;

}

#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION REG_STATE_PROCESS_MCFG_REFRESH_IND

DESCRIPTION
  This function processes the refresh indication from MCFG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void reg_state_process_mcfg_refresh_ind
( 
  reg_cmd_type *cmd_ptr 
)
{
  ASSERT(cmd_ptr != NULL);
  reg_nv_process_mcfg_refresh();
  mcfg_refresh_done(cmd_ptr->cmd.mcfg_refresh_ind.reference_id);
}
#endif
#ifdef FEATURE_LTE
reg_state_p_type reg_state_process_data_roaming_ind
(
  cm_data_roaming_ind_s_type  *data_roaming_ind
)
{
  reg_state_s_type *state_p = reg_state_curr_p;

#ifdef FEATURE_DUAL_SIM
  if(reg_nv_read_voice_roaming((sys_modem_as_id_e_type)reg_sub_id) == FALSE)
#else
  if(reg_nv_read_voice_roaming() == FALSE)
#endif
  {
        MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore Data Roaming When Voice is already Off");
	return state_p;
  }
  if(reg_nv_write_data_roaming(data_roaming_ind->data_roaming))
  {
    if((data_roaming_ind->data_roaming == SYS_DATA_ROAMING_INTERNATIONAL_OFF) &&
       (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) &&
       (reg_sim_check_mcc_same_country(reg_state_service_state.plmn) == FALSE))
    {
      reg_send_mmr_ps_detach_req(SYS_PS_DETACH_TYPE_LOCAL_DETACH);
    }
    if(((reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) &&
         (reg_sim_check_mcc_same_country(reg_state_service_state.plmn) == FALSE) &&
         ((reg_state_curr_p->number() == REG_STATE_IDLE) ||
          (reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
          (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH)))||
        (reg_state_curr_p->number() == REG_STATE_REGISTERING))
    {
      if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
        (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
      {
        reg_send_mmr_plmn_search_abort_req();
      }
      if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reg_mode_start_plmn_selection
        (
           reg_mode_network_selection_mode_get(),
           reg_mode_req_mode_pref_get(),
           reg_mode_cs_for_request_get(),
           reg_mode_ps_for_request_get(),
           reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
           ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
           ,reg_mode_rat_pri_list_get()
#endif
           ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
           ,reg_mode_sms_only_get(),
           0
#endif 
        );
        state_p = &reg_state_registering;
      }
      else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
      {
        reg_mode_start_limited_service
        (
          reg_mode_network_selection_mode_get(),
           reg_mode_req_mode_pref_get(),
          reg_mode_cs_for_request_get(),
          reg_mode_ps_for_request_get(),
          reg_mode_req_service_domain_get()
    #ifdef FEATURE_GSM_BAND_PREF
          
          ,reg_mode_band_pref_get()
    #endif
    #ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()
    #endif
          ,REG_CM_SERVICE_REQ_NORMAL
    #ifdef FEATURE_LTE 
         ,reg_mode_sms_only_get(),
          0
    #endif
        );
      }
      else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
              reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )
      {
        reg_mode_start_manual_plmn_selection
        (
           reg_mode_network_selection_mode_get(),
           reg_send_last_reg_req_plmn_get(),
           reg_mode_mode_pref_get(),
           FALSE,
           FALSE,
           reg_mode_req_service_domain_get(),
     #ifdef FEATURE_GSM_BAND_PREF
           reg_mode_band_pref_get(),
     #endif
     #ifdef FEATURE_RAT_PRIORITY_LIST
           reg_mode_rat_pri_list_get(),
     #endif
           REG_CM_SERVICE_REQ_NORMAL
     #ifdef FEATURE_LTE 
           ,reg_mode_sms_only_get(),
           0
     #endif                 
        );
        state_p = &reg_state_registering;
      } 
    }
  }
  return state_p;
}
#endif


reg_state_p_type reg_state_process_roaming_ind
(
	cm_roaming_ind_s_type	*roaming_ind
)
{
  reg_state_s_type *state_p = reg_state_curr_p;
  MSG_HIGH_DS_1(REG_SUB, "=REG= Roaming Indication Received with %d",roaming_ind->international_voice_roaming_allowed);
  if(reg_nv_write_voice_roaming(roaming_ind->international_voice_roaming_allowed))
  {
	if((roaming_ind->international_voice_roaming_allowed == FALSE) &&
       (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) &&
       (reg_sim_check_mcc_same_country(reg_state_service_state.plmn) == FALSE))
    {
      reg_send_mmr_stop_mode_req(SYS_STOP_MODE_REASON_LOCAL_CS_PS_DETACH);
    }
    if(((reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) &&
		((roaming_ind->international_voice_roaming_allowed == TRUE))&&
         (reg_sim_check_mcc_same_country(reg_state_service_state.plmn) == FALSE) &&
         ((reg_state_curr_p->number() == REG_STATE_IDLE) ||
          (reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
          (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH)))||
        (reg_state_curr_p->number() == REG_STATE_REGISTERING))
    {
      if ((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
        (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
      {
        reg_send_mmr_plmn_search_abort_req();
      }
      if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reg_mode_start_plmn_selection
        (
           reg_mode_network_selection_mode_get(),
           reg_mode_req_mode_pref_get(),
           reg_mode_cs_for_request_get(),
           reg_mode_ps_for_request_get(),
           reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
           ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
           ,reg_mode_rat_pri_list_get()
#endif
           ,REG_CM_SERVICE_REQ_NORMAL 
#ifdef FEATURE_LTE
           ,reg_mode_sms_only_get(),
           0
#endif 
        );
        state_p = &reg_state_registering;
      }
      else if((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) &&
		  (reg_state_curr_p->number() != REG_STATE_REGISTERING))
      {
		reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
		reg_send_cm_service_ind(reg_state_service_state);
      }
      else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL ||
              reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )
      {
        reg_mode_start_manual_plmn_selection
        (
           reg_mode_network_selection_mode_get(),
           reg_send_last_reg_req_plmn_get(),
           reg_mode_mode_pref_get(),
           FALSE,
           FALSE,
           reg_mode_req_service_domain_get(),
     #ifdef FEATURE_GSM_BAND_PREF
           reg_mode_band_pref_get(),
     #endif
     #ifdef FEATURE_RAT_PRIORITY_LIST
           reg_mode_rat_pri_list_get(),
     #endif
           REG_CM_SERVICE_REQ_NORMAL
     #ifdef FEATURE_LTE 
           ,reg_mode_sms_only_get(),
           0
     #endif                 
        );
        state_p = &reg_state_registering;
      } 
    }
  }
  return state_p;

}

/*==============================================================================

FUNCTION NAME

  reg_state_main

==============================================================================*/

void reg_state_main
(
  reg_cmd_type* reg_cmd_p
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if (!reg_get_as_id( reg_cmd_p))
  {
    return;
  }
#endif

  reg_add_message_to_debug_buffer(reg_cmd_p->cmd.hdr.message_set, (byte)reg_cmd_p->cmd.hdr.message_id, TRUE, reg_mode_mode_pref_get());

  reg_state_curr_p->output_name();

  reg_state_next_p = reg_state_curr_p;

  switch( reg_cmd_p->cmd.hdr.message_set )
  {

    /*
    ** MESSAGE FROM CM
    */
    case MS_CM_REG:

      switch( reg_cmd_p->cmd.hdr.message_id )
      {

        case CM_NETWORK_LIST_REQ:
          if ( cm_network_list_req_is_valid(&(reg_cmd_p->cmd.cm_network_list_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_NETWORK_LIST_REQ");
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_network_list_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_network_list_req
            (
              &(reg_cmd_p->cmd.cm_network_list_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_NETWORK_LIST_REQ - Invalid");
          }
          break;
        case CM_SERVICE_REQ:
          if ( cm_service_req_is_valid(&(reg_cmd_p->cmd.cm_service_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ");
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
            reg_sim_mode_pref = reg_cmd_p->cmd.cm_service_req.mode_pref;
#endif
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_service_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_service_req
            (
              &(reg_cmd_p->cmd.cm_service_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SERVICE_REQ - Invalid");
          }
          break;

        case CM_SIM_AVAILABLE_REQ:
          if ( cm_sim_available_req_is_valid(&(reg_cmd_p->cmd.cm_sim_available_req)) )
          {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
            MSG_HIGH_DS_3(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ mode_pref:%d, session_type:%d nv_index:%d",reg_cmd_p->cmd.cm_sim_available_req.mode_pref,
                        reg_cmd_p->cmd.cm_sim_available_req.session_type,reg_cmd_p->cmd.cm_sim_available_req.nv_context_id);
#else
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ");
#endif
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_sim_available_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_sim_available_req
            (
              &(reg_cmd_p->cmd.cm_sim_available_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SIM_AVAILABLE_REQ - Invalid");
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_sim_available_req.transaction_id;
            reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id);
          }
          break;

        case CM_SIM_NOT_AVAILABLE_REQ:
          if ( cm_sim_not_available_req_is_valid(&(reg_cmd_p->cmd.cm_sim_not_available_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SIM_NOT_AVAILABLE_REQ");
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_sim_not_available_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_sim_not_available_req
            (
              &(reg_cmd_p->cmd.cm_sim_not_available_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SIM_NOT_AVAILABLE_REQ - Invalid");
          }
          break;

        case CM_STOP_MODE_REQ:
          if ( cm_stop_mode_req_is_valid(&(reg_cmd_p->cmd.cm_stop_mode_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_STOP_MODE_REQ");
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_stop_mode_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_stop_mode_req
            (
              &(reg_cmd_p->cmd.cm_stop_mode_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_STOP_MODE_REQ - Invalid");
          }
          break;
        case CM_PH_STATUS_CHGD_REQ:
          if(cm_ph_status_chgd_req_is_valid(&(reg_cmd_p->cmd.cm_ph_status_chgd_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PH_STATUS_CHGD_REQ");
            reg_state_cm_transaction_id = reg_cmd_p->cmd.cm_ph_status_chgd_req.transaction_id;
            reg_state_next_p = reg_state_curr_p->process_cm_ph_status_chgd_req
            (
              &(reg_cmd_p->cmd.cm_ph_status_chgd_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PH_STATUS_CHGD_REQ - Invalid");
          }
          break;

#ifdef FEATURE_OOSC_USER_ACTION
        case CM_EXIT_FROM_PWR_SAVE:
          if ( cm_exit_from_pwr_save_is_valid(&(reg_cmd_p->cmd.cm_exit_from_pwr_save)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_EXIT_FROM_PWR_SAVE");
            reg_send_mmr_exit_from_pwr_save();
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_EXIT_FROM_PWR_SAVE - Invalid");
          }
          break;
#endif
  
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
        case CM_HPLMN_SEARCH_REQ:
          if ( cm_hplmn_search_req_is_valid(&(reg_cmd_p->cmd.cm_hplmn_search_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_HPLMN_SEARCH_REQ");
            reg_state_next_p = reg_state_curr_p->process_cm_hplmn_search_req
            (
              &(reg_cmd_p->cmd.cm_hplmn_search_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_HPLMN_SEARCH_REQ - Invalid");
          }
          break;
#endif

#ifdef FEATURE_DUAL_SIM
        case CM_DS_STAT_CHGD_REQ:
          if(cm_ds_stat_chgd_req_is_valid(&(reg_cmd_p->cmd.cm_ds_stat_chgd_req)) )
          {
            uint8 active_subs_count =0;
            sys_modem_as_id_e_type as_id;

            MSG_HIGH_DS_1(REG_SUB, "=REG= CM_DS_STAT_CHGD_REQ, ds stusus %d",reg_cmd_p->cmd.cm_ds_stat_chgd_req.dual_standby_mode);
            
            for(as_id = SYS_MODEM_AS_ID_1; as_id < MAX_NAS_STACKS; as_id++)
            {
              active_subs_count = active_subs_count+(((reg_cmd_p->cmd.cm_ds_stat_chgd_req.active_subs) & BM(as_id)) >> as_id); 
              MSG_HIGH_DS_1(REG_SUB, "=REG= active subs count is %d ",active_subs_count); 
            }
            if(active_subs_count > 1)
            {
              reg_send_mmr_ds_stat_chgd_req(TRUE,reg_cmd_p->cmd.cm_ds_stat_chgd_req.dual_standby_mode,
                                                                          reg_cmd_p->cmd.cm_ds_stat_chgd_req.device_mode);           
            }
            else
            {
              reg_send_mmr_ds_stat_chgd_req(FALSE,reg_cmd_p->cmd.cm_ds_stat_chgd_req.dual_standby_mode,
                                                                          reg_cmd_p->cmd.cm_ds_stat_chgd_req.device_mode);              
            }
            reg_send_cb_ds_stat_chgd_ind(reg_cmd_p->cmd.cm_ds_stat_chgd_req.active_subs);

          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DS_STAT_CHGD_REQ - Invalid");
          }
          break;
#endif

#ifdef FEATURE_DUAL_SIM     
        case CM_MULTIMODE_SUBS_CHGD_REQ:
          MSG_HIGH_DS_2(REG_SUB, "=REG= CM_MULTIMODE_SUBS_CHGD_REQ{\n subs capability[0]= %d,subs capability[1]=%d \n}",
                           reg_cmd_p->cmd.cm_multimode_subs_chgd_req.subs_capability[0],
                           reg_cmd_p->cmd.cm_multimode_subs_chgd_req.subs_capability[1]); 
          if(cm_multimode_subs_chgd_req_is_valid(&reg_cmd_p->cmd.cm_multimode_subs_chgd_req))
          {
            reg_send_mmr_multimode_subs_chgd_req(reg_cmd_p->cmd.cm_multimode_subs_chgd_req.subs_capability);
          }
          else
          {
            MSG_FATAL_DS(REG_SUB, "=REG= CM_MULTIMODE_SUBS_CHGD_REQ - Invalid", 0, 0, 0);
          }
        break;
#endif

#ifdef FEATURE_DUAL_SIM
        case CM_DS_TUNEAWAY_STATUS_CHANGE_IND:
          if(cm_ds_tuneaway_status_change_ind_is_valid(&(reg_cmd_p->cmd.cm_ds_tuneaway_stat_chgd_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DS_TUNEAWAY_STATUS_CHANGE_IND");
            reg_send_mmr_ds_tuneaway_status_change_ind
            (
              reg_cmd_p->cmd.cm_ds_tuneaway_stat_chgd_ind.tuneaway_status
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DS_TUNEAWAY_STATUS_CHANGE_IND - Invalid");
          }
          break;
#endif

#ifdef FEATURE_LTE
        case CM_PS_DETACH_REQ:
          if(cm_ps_detach_req_is_valid(&(reg_cmd_p->cmd.cm_ps_detach_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PS_DETACH_REQ");
            reg_state_cm_initiated_ps_detach = TRUE;
            reg_send_mmr_ps_detach_req
            (
              reg_cmd_p->cmd.cm_ps_detach_req.detach_type
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PS_DETACH_REQ - Invalid");
          }
          break;

        case CM_BLOCK_PLMN_REQ :
          if(cm_block_plmn_req_is_valid(&(reg_cmd_p->cmd.cm_block_plmn_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_BLOCK_PLMN_REQ ");
            reg_state_next_p = reg_state_curr_p->process_cm_block_plmn_req
            (
              &(reg_cmd_p->cmd.cm_block_plmn_req)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_BLOCK_PLMN_REQ  - Invalid");
          }
          break;

        case CM_SET_DRX_REQ:
          if(cm_set_drx_req_is_valid(&(reg_cmd_p->cmd.cm_set_drx_req)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SET_DRX_REQ");
            reg_state_next_p = reg_state_curr_p->process_cm_set_drx_req
            (
              &(reg_cmd_p->cmd.cm_set_drx_req)
            );
          }
          else
          {
            reg_send_cm_set_drx_cnf(FALSE);
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SET_DRX_REQ - Invalid ");
          }
          break;

        case CM_GET_DRX_REQ:
          MSG_HIGH_DS_0(REG_SUB, "=REG= CM_GET_DRX_REQ");
          reg_send_mmr_get_drx_req();
          break;
     
#endif /*FEATURE_LTE*/
        case CM_DATA_ROAMING_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DATA_ROAMING_IND");
#ifdef FEATURE_LTE
          reg_state_next_p = reg_state_process_data_roaming_ind(&(reg_cmd_p->cmd.cm_data_roaming_ind));
#endif
          break;
        case CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ:
           if(cm_set_hplmn_irat_timer_req_is_valid(&(reg_cmd_p->cmd.cm_set_hplmn_irat_search_timer_req)))
           {
             MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ");
              reg_state_next_p = reg_state_curr_p->process_set_hplmn_irat_search_timer
              (
                &(reg_cmd_p->cmd.cm_set_hplmn_irat_search_timer_req)
              );
           }
           else
           {
             MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ - Invalid"); 
           }       
 
          break;


      case CM_SET_HPLMN_SEARCH_TIMER_REQ:
          MSG_HIGH_DS_0(REG_SUB, "=REG= CM_SET_HPLMN_SEARCH_TIMER_REQ");
          reg_state_process_set_hplmn_search_req(&(reg_cmd_p->cmd.cm_set_hplmn_search_timer_req));
          break;

      case CM_GET_HPLMN_SEARCH_TIMER_REQ:
        MSG_HIGH_DS_0(REG_SUB, "=REG= CM_GET_HPLMN_SEARCH_TIMER_REQ");
        reg_state_process_get_hplmn_search_req(&(reg_cmd_p->cmd.cm_get_hplmn_search_timer_req));
        break;
#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)|| defined (FEATURE_SVLTE_DUAL_SIM)
     case CM_UE_MODE_IND:
        if(cm_mode_change_ind_is_valid(&(reg_cmd_p->cmd.cm_ue_mode_ind)))
        {
          MSG_HIGH_DS_2(REG_SUB, "=REG= CM_MODE_CHANGE_INDICATION  Mode is %d and sub mode %d",
                                                              reg_cmd_p->cmd.cm_ue_mode_ind.ue_mode ,reg_cmd_p->cmd.cm_ue_mode_ind.is_ue_mode_substate_srlte);
          reg_state_process_cm_mode_change_ind(reg_cmd_p->cmd.cm_ue_mode_ind.ue_mode,reg_cmd_p->cmd.cm_ue_mode_ind.is_ue_mode_substate_srlte);
        }

       break;
#endif

#ifdef FEATURE_DUAL_DATA
     case CM_DATA_PRIORITY_IND:
        if(cm_data_priority_ind_is_valid(&(reg_cmd_p->cmd.cm_data_prio_ind)))
        {
          MSG_HIGH_DS_1(REG_SUB, "=REG= CM_DATA_PRIORITY_IND  priority %d",
                                                              reg_cmd_p->cmd.cm_data_prio_ind.priority);
          reg_state_process_cm_data_priority_ind(reg_cmd_p->cmd.cm_data_prio_ind.priority);
        }

       break;
#endif

#ifdef FEATURE_FEMTO_CSG
     case CM_CSG_SELECT_CONFIG_UPDATE_IND:
          if(cm_csg_select_config_update_ind_is_valid(
                 &(reg_cmd_p->cmd.cm_csg_select_config_update_ind)))
          {
            MSG_HIGH_DS_1(REG_SUB, "=REG= CM_CSG_SELECT_CONFIG_UPDATE_IND, config-%d",
               reg_cmd_p->cmd.cm_csg_select_config_update_ind.csg_selection_config);
            reg_state_csg_selection_config = 
                   reg_cmd_p->cmd.cm_csg_select_config_update_ind.csg_selection_config;
            reg_send_mmr_csg_select_config_update_ind
            (
              reg_cmd_p->cmd.cm_csg_select_config_update_ind.csg_selection_config
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_CSG_SELECT_CONFIG_UPDATE_IND - Invalid ");
          }
          break;
#endif
#ifdef FEATURE_DUAL_SIM
     case CM_DDS_SWITCH_IND:
          if(cm_dds_switch_ind_is_valid(&(reg_cmd_p->cmd.cm_dds_switch_ind)))
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DDS_SWITCH_IND");
            reg_state_dds_switch_in_progress_sim[REG_AS_ID_1] = TRUE;
            reg_state_dds_switch_in_progress_sim[REG_AS_ID_2] = TRUE;
            reg_send_mmr_dds_switch_ind(reg_cmd_p->cmd.cm_dds_switch_ind.target_dds_sub);
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_DDS_SWITCH_IND  - Invalid");
          }
          break;
#endif
 
        case CM_PSM_READY_REQ:
          if(cm_psm_ready_req_is_valid(&(reg_cmd_p->cmd.cm_psm_ready_req)))
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PSM_READY_REQ");
            reg_send_mmr_psm_ready_req();
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= CM_PSM_READY_REQ - Invalid ");
          }
          break;

	case CM_ROAMING_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= CM_ROAMING_IND");
          reg_state_next_p = reg_state_process_roaming_ind(&(reg_cmd_p->cmd.cm_roaming_ind));

          break;

        default:
          MSG_HIGH_DS_1(REG_SUB, "=REG= Unknown CM message (%d)",
                   reg_cmd_p->cmd.hdr.message_id);
          break;
      }
      break;
#ifdef FEATURE_NAS_ECALL
    /*
    ** MESSAGE FROM ECALL
    */ 
    case MS_ECALL_REG:
      switch(reg_cmd_p->cmd.hdr.message_id )
      {
        case ECALL_REG_MODE_IND:

          if (ecall_reg_mode_ind_is_valid(&(reg_cmd_p->cmd.ecall_reg_mode_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB,"=REG= ECALL_REG_MODE_IND");

            reg_send_mmr_ecall_mode_ind
            (
              ecall_reg_convert_ecall_mode_type_to_mm(reg_cmd_p->cmd.ecall_reg_mode_ind.mode_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB,"=REG= ECALL_REG_MODE_IND - Invalid");
          }

          break;

        default:
          MSG_HIGH_DS_1(REG_SUB,"=REG= Unknown ECALL message (%d)",
                   reg_cmd_p->cmd.hdr.message_id);
         break;
      }
     
      break;
#endif
   /*
    ** MESSAGE FROM MM
    */ 
    case MS_MM_REG:

      switch( reg_cmd_p->cmd.hdr.message_id )
      {
        case MMR_CAMPED_IND:
          if ( mmr_camped_ind_is_valid(&(reg_cmd_p->cmd.mmr_camped_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CAMPED_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_camped_ind
            (
              &(reg_cmd_p->cmd.mmr_camped_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CAMPED_IND - Invalid");
          }
          break;
        case MMR_ATTACH_FAILED_IND:                     
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_ATTACH_FAILED_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_attach_failed_ind();
          break;

        case MMR_MM_INFORMATION_IND:
          if ( mmr_mm_information_ind_is_valid(&(reg_cmd_p->cmd.mmr_mm_information_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_MM_INFORMATION_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_mm_information_ind
            (
              &(reg_cmd_p->cmd.mmr_mm_information_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_MM_INFORMATION_IND - Invalid");
          }
          break;

        case MMR_PLMN_SEARCH_CNF:
          if ( mmr_plmn_search_cnf_is_valid(&(reg_cmd_p->cmd.mmr_plmn_search_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PLMN_SEARCH_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_plmn_search_cnf
            (
              &(reg_cmd_p->cmd.mmr_plmn_search_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PLMN_SEARCH_CNF - Invalid");
          }
          break;

        case MMR_REG_CNF:
          if ( mmr_reg_cnf_is_valid(&(reg_cmd_p->cmd.mmr_reg_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_reg_cnf
            (
              &(reg_cmd_p->cmd.mmr_reg_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_CNF - Invalid");
          }
          break;

        case MMR_SERVICE_IND:
          if ( mmr_service_ind_is_valid(&(reg_cmd_p->cmd.mmr_service_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_service_ind
            (
              &(reg_cmd_p->cmd.mmr_service_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Invalid");
          }
          break;

        case MMR_SIM_AVAILABLE_CNF:
          if ( mmr_sim_available_cnf_is_valid(&(reg_cmd_p->cmd.mmr_sim_available_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SIM_AVAILABLE_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_sim_available_cnf
            (
              &(reg_cmd_p->cmd.mmr_sim_available_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SIM_AVAILABLE_CNF - Invalid");
          }
          break;

        case MMR_SIM_NOT_AVAILABLE_CNF:
          if ( mmr_sim_not_available_cnf_is_valid(&(reg_cmd_p->cmd.mmr_sim_not_available_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SIM_NOT_AVAILABLE_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_sim_not_available_cnf
            (
              &(reg_cmd_p->cmd.mmr_sim_not_available_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SIM_NOT_AVAILABLE_CNF - Invalid");
          }
          break;

        case MMR_STOP_MODE_CNF:
          if ( mmr_stop_mode_cnf_is_valid(&(reg_cmd_p->cmd.mmr_stop_mode_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_STOP_MODE_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_stop_mode_cnf
            (
              &(reg_cmd_p->cmd.mmr_stop_mode_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_STOP_MODE_CNF - Invalid");
          }
          break;
#if defined(FEATURE_HSDPA) || defined(FEATURE_GSM_DTM)
        case MMR_CELL_SERVICE_IND:
          if( mmr_cell_service_ind_is_valid(&(reg_cmd_p->cmd.mmr_cell_service_ind)) )
          {
              MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CELL_SERVICE_IND");
              reg_state_process_cell_service_ind(&(reg_cmd_p->cmd.mmr_cell_service_ind));
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CELL_SERVICE_IND - Invalid");
          }
          break;
#endif /*defined(FEATURE_HSDPA) || defined(FEATURE_GSM_DTM)*/

#ifdef FEATURE_HSPA_CALL_STATUS_IND
        case MMR_HSPA_CALL_STATUS_IND:
          if( mmr_hspa_call_status_ind_is_valid(&(reg_cmd_p->cmd.mmr_hspa_call_status_ind)) )
          {
              MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_HSPA_CALL_STATUS_IND");
              reg_state_process_hspa_call_status_ind(&(reg_cmd_p->cmd.mmr_hspa_call_status_ind));
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_HSPA_CALL_STATUS_IND - Invalid");
          }
          break;
#endif
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
        case MMR_CONN_MODE_SERVICE_STATUS_IND:
          if( mmr_conn_mode_service_status_ind_is_valid(&(reg_cmd_p->cmd.mmr_conn_mode_service_status_ind)) )
          {
              MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CONN_MODE_SERVICE_STATUS_IND");
              reg_state_process_conn_mode_service_status_ind(&(reg_cmd_p->cmd.mmr_conn_mode_service_status_ind));
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CONN_MODE_SERVICE_STATUS_IND - Invalid");
          }
          break;
#endif
        case MMR_EMERGENCY_NUM_LIST_IND:
          if ( mmr_emergency_num_list_is_valid(&(reg_cmd_p->cmd.mmr_emergency_num_list_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_EMERGENCY_NUM_LIST_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_emergency_num_list_ind
            (
              &(reg_cmd_p->cmd.mmr_emergency_num_list_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_EMERGENCY_NUM_LIST_IND - Invalid");
          }
          break;

        case MMR_PH_STATUS_CHANGE_CNF:
          if ( mmr_ph_status_change_cnf_is_valid(&(reg_cmd_p->cmd.mmr_ph_status_change_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PH_STATUS_CHANGE_CNF");
            reg_state_next_p = reg_state_curr_p->process_mmr_ph_status_change_cnf
            (
              &(reg_cmd_p->cmd.mmr_ph_status_change_cnf)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PH_STATUS_CHANGE_CNF - Invalid");
          }
          break;          

        case MMR_REG_REJECT_IND:
          if ( mmr_reg_reject_ind_is_valid(&(reg_cmd_p->cmd.mmr_reg_reject_ind)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_REJECT_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_reg_reject_ind
            (
              &(reg_cmd_p->cmd.mmr_reg_reject_ind)
            );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_REG_REJECT_IND - Invalid");
          }
          break;

#ifdef FEATURE_DUAL_SIM
        case MMR_DS_STAT_CHGD_CNF:
          if ( mmr_ds_stat_chgd_cnf_is_valid(&(reg_cmd_p->cmd.mmr_ds_stat_chgd_cnf)) )
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_DS_STAT_CHGD_CNF");
            reg_send_cm_ds_stat_chgd_cnf( reg_state_cm_non_subid_transaction_id );
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_DS_STAT_CHGD_CNF - Invalid");
          }
          break;
#endif
 
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
         case MMR_SUITABLE_SEARCH_END_IND:         
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SUITABLE_SEARCH_END_IND");
            reg_mode_update_plmn_selection_mode(REG_MODE_AUTOMATIC_MODE_NORMAL);
            reg_send_cm_suitable_search_end_ind();
          break;            
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
         case MMR_DISABLE_BPLMN_IND:         
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_DISABLE_BPLMN_IND");
            reg_send_cm_disable_bplmn_ind(reg_cmd_p->cmd.mmr_disable_bplmn_ind);
          break;            
#endif
         case MMR_CELL_ACCESS_IND:
          if (mmr_cell_access_ind_is_valid(&(reg_cmd_p->cmd.mmr_cell_access_ind)))
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CELL_ACCESS_IND");
            reg_send_cm_cell_access_ind(reg_cmd_p->cmd.mmr_cell_access_ind.cell_access_status);
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CELL_ACCESSIND - Invalid"); 
          }
          break;

#ifdef FEATURE_LTE
         case MMR_PS_DETACH_CNF:
          /* This api shall send PS detach cnf & set reg_mode_mm_waiting_for_reg_req to TRUE.*/
          reg_state_proc_mmr_ps_detach_cnf();
          break;

         case MMR_FPLMN_LIST_CHANGE_IND:
           if(mmr_fplmn_list_change_ind_is_valid(&reg_cmd_p->cmd.mmr_fplmn_list_change_ind))
           {
             MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_FPLMN_LIST_CHANGE_IND");
             reg_send_cm_fplmn_list_ind(reg_cmd_p->cmd.mmr_fplmn_list_change_ind.list_type);
           }
           else
           {
             MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_FPLMN_LIST_CHANGE_IND-Invalid");
           }
           break;

         case MMR_SET_DRX_CNF:
          MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SET_DRX_CNF");
          reg_send_cm_set_drx_cnf(reg_cmd_p->cmd.mmr_set_drx_cnf.result);
          break;

         case MMR_GET_DRX_CNF:
          MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_SET_DRX_CNF");
          reg_send_cm_get_drx_cnf(reg_cmd_p->cmd.mmr_get_drx_cnf.drx_coefficient);
          break;
#ifdef FEATURE_LTE_REL9
         case MMR_LTE_CELL_INFO_IND:
           reg_state_process_lte_cell_info_ind(reg_cmd_p->cmd.mmr_lte_cell_info_ind);
           break;   
#endif

         case MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND:
           reg_state_process_pseudo_resel_low_priority_ind(reg_cmd_p->cmd.mmr_pseudo_resel_low_priority_ind);
           break;   


#ifdef FEATURE_FEMTO_CSG
         case MMR_HOME_NODEB_NAME_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_HOME_NODEB_NAME_IND");
           reg_send_cm_home_nodeb_name_ind(&(reg_cmd_p->cmd.mmr_home_nodeb_name_ind));
           break;
#endif

         case MMR_UPDATE_CURR_RAT_LIST_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_UPDATE_CURR_RAT_LIST_IND");
           reg_mode_curr_rat_pri_list_set(&(reg_cmd_p->cmd.mmr_update_curr_rat_list_ind.rat_pri_list_info));
           break;

         case MMR_RRC_SIB6_INFO_IND:
           MSG_HIGH_DS_0(REG_SUB,"=REG= MMR_RRC_SIB6_INFO_IND");
           reg_mode_process_rrc_sib_info_ind(reg_cmd_p);
           break;

         case MMR_RRC_SIB7_INFO_IND:
           MSG_HIGH_DS_0(REG_SUB,"=REG= MMR_RRC_SIB7_INFO_IND");
           reg_mode_process_rrc_sib_info_ind(reg_cmd_p);
           break;
#endif /*FEATURE_LTE*/

#ifdef FEATURE_DUAL_SIM
         case MMR_MULTIMODE_SUBS_CHGD_CNF:
           MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_MULTIMODE_SUBS_CHGD_CNF");
           reg_send_cm_multimode_subs_chgd_cnf();
          break;
#endif

#ifdef FEATURE_SGLTE
         case MMR_SERVICE_STAT_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= reg received Status change indication form MM");
           reg_state_process_service_stat_ind(&(reg_cmd_p->cmd.mmr_service_stat_ind));
           break;

         case MMR_STACK_DEACT_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= reg received STACK DEACT indication form MM");
           reg_state_process_stack_deact_ind(&(reg_cmd_p->cmd.mmr_stack_deact_ind));
           break;

         case MMR_STACK_ACT_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= reg received STACK ACT indication form MM");
           reg_state_process_stack_act_ind(&(reg_cmd_p->cmd.mmr_stack_act_ind));
           break;

#endif
#ifdef FEATURE_FEMTO_CSG
         case MMR_CSG_SEARCH_TRIGGER_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CSG_SEARCH_TRIGGER_IND");
           reg_send_cm_csg_search_trigger_ind();
           break;
 
         case MMR_CSG_LIST_READ_COMPLETE_IND:
            MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_CSG_LIST_READ_COMPLETE_IND");
            reg_state_next_p = reg_state_curr_p->process_mmr_csg_list_read_complete();
           break;
#endif

         case MMR_T3346_TIMER_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_T3346_TIMER_IND");
           reg_send_cm_t3346_timer_ind(&(reg_cmd_p->cmd.mmr_t3346_timer_ind));
           break;

        case MMR_PSM_READY_RSP:
          MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PSM_READY_RSP");
          reg_send_cm_psm_ready_rsp(reg_cmd_p->cmd.mmr_psm_ready_rsp.status);
          break;

        case MMR_PSM_READY_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PSM_READY_IND");
          reg_send_cm_psm_ready_ind();
          break;

         default:
          MSG_HIGH_DS_1(REG_SUB, "=REG= Unknown MM message (%d)",
                   reg_cmd_p->cmd.hdr.message_id);
          break;
      }
      break;

    /*
    **  TIMER EXPIRED
    */
    case MS_TIMER:

      if (!reg_is_timer_active(reg_cmd_p->cmd.timer_expiry.timer_id))
      switch(reg_cmd_p->cmd.timer_expiry.timer_id)
      {
        case REG_HPLMN_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired");
          reg_state_next_p = reg_state_curr_p->process_hplmn_search_timer_expiration();
          break;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
        case REG_HPLMN_TIMER_SIM_ID_3:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired for SIM ID 3");
          reg_state_next_p = reg_state_curr_p->process_hplmn_search_timer_expiration();
          break;

#endif
        case REG_HPLMN_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired for SIM ID 2");
          reg_state_next_p = reg_state_curr_p->process_hplmn_search_timer_expiration();
          break;
#endif

#ifdef FEATURE_LTE
        case REG_BACKOFF_FPLMN_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= Backoff plmn Search Timer expired");
          reg_state_next_p = reg_state_curr_p->process_backoff_fplmn_timer_expiration();
          break;

#ifdef FEATURE_DUAL_SIM
        case REG_BACKOFF_FPLMN_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= Backoff plmn Search Timer expired for SIM 2");
          reg_state_next_p = reg_state_curr_p->process_backoff_fplmn_timer_expiration();
          break;
#endif

        case REG_UPDATE_LTE_CAP_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= UPDATE LTE CAP Timer expired");
          reg_state_next_p = reg_state_curr_p->process_update_lte_cap_timer_expiration();
          break;
#ifdef FEATURE_DUAL_SIM
        case REG_UPDATE_LTE_CAP_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= UPDATE LTE CAP Timer expired for SIM 2");
          reg_state_next_p = reg_state_curr_p->process_update_lte_cap_timer_expiration();
          break;
#endif
#endif
        case REG_HPLMN_IRAT_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN IRAT Search Timer expired");
          reg_state_next_p = reg_state_curr_p->process_hplmn_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p) &&
                (reg_state_next_p == &reg_state_automatic_search))
          {
             reg_state_irat_hplmn_timer_search_ongoing = TRUE;
          }
          break;
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
        case REG_HPLMN_IRAT_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN IRAT Search Timer expired for SIM 2");
          reg_state_next_p = reg_state_curr_p->process_hplmn_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p))
          {
            reg_state_irat_hplmn_timer_search_ongoing = TRUE;
          }

          break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
        case REG_HPLMN_IRAT_TIMER_SIM_ID_3:
          MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN IRAT Search Timer expired for SIM 3");
          reg_state_next_p = reg_state_curr_p->process_hplmn_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p))
          {
            reg_state_irat_hplmn_timer_search_ongoing = TRUE;
          }

          break;
#endif
#endif
        case REG_T3245_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= T3245 Timer expired");
          reg_state_next_p = reg_state_curr_p->process_t3245_timer_expiration();
          break;


#ifdef FEATURE_DUAL_SIM
        case REG_T3245_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= T3245 Timer expired for SIM ID 2");
          reg_state_next_p = reg_state_curr_p->process_t3245_timer_expiration();
          break;

#ifdef FEATURE_TRIPLE_SIM
        case REG_T3245_TIMER_SIM_ID_3:
          MSG_HIGH_DS_0(REG_SUB, "=REG= T3245 Timer expired for SIM ID 3");
          reg_state_next_p = reg_state_curr_p->process_t3245_timer_expiration();
          break;
#endif
#endif

        case REG_VALIDATE_SIM_TIMER:
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
        case REG_VALIDATE_SIM_TIMER_SIM_ID_2:
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
        case REG_VALIDATE_SIM_TIMER_SIM_ID_3:
#endif
#endif
          MSG_HIGH_DS_0(REG_SUB, "=REG= REG VALIDATE SIM Timer expired");
          reg_state_process_validate_sim_timer_expiry();
          break;

#ifdef FEATURE_SGLTE
        case REG_SGLTE_PS_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= SGLTE PS timer expired");
          reg_state_next_p = reg_state_curr_p->process_sglte_ps_timer_expiry();
          break;
#endif


        case REG_LTE_IRAT_TIMER:
          MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE_IRAT timer expired");
          reg_state_next_p = reg_state_curr_p->process_lte_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p))
          {
            reg_state_irat_lte_timer_search_ongoing = TRUE;
          }
          break;
#if defined FEATURE_DUAL_SIM
        case REG_LTE_IRAT_TIMER_SIM_ID_2:
          MSG_HIGH_DS_0(REG_SUB, "=REG= LTE_IRAT timer expired for SIM 2");
          reg_state_next_p = reg_state_curr_p->process_lte_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p))
          {
            reg_state_irat_lte_timer_search_ongoing = TRUE;
          }
          break;
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
        case REG_LTE_IRAT_TIMER_SIM_ID_3:
          MSG_HIGH_DS_0(REG_SUB, "=REG= LTE_IRAT timer expired for SIM 3");
          reg_state_next_p = reg_state_curr_p->process_lte_irat_search_timer_expiration();
          if ((reg_state_curr_p != reg_state_next_p))
          {
            reg_state_irat_lte_timer_search_ongoing = TRUE;
          }
          break;
#endif
#endif

        case REG_BACK_OFF_LTE_SEARCH_TIMER:
         MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : REG_BACK_OFF_LTE_SEARCH_TIMER timer expired");
         reg_state_next_p = reg_state_curr_p;
         reg_state_lte_non_available_list_ptr = reg_state_process_back_off_lte_timer_expiry(reg_state_lte_non_available_list_ptr);
         break;



        default:
          MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown timer expired id = %d", reg_cmd_p->cmd.timer_expiry.timer_id);
          break;
      }

      break;
    /*
    **  FCN Message from GSDI
    */
    case MS_GSDI:

      switch(reg_cmd_p->cmd.hdr.message_id)
      {
        case SIM_REFRESH_IND:
          reg_state_process_sim_refresh_ind
            (
              &(reg_cmd_p->cmd.sim_refresh_ind)

            );
          break;
        case SIM_REFRESH_FCN_IND:
          if ( reg_sim_refresh_fcn_ind_is_valid(&(reg_cmd_p->cmd.sim_refresh_fcn_ind)) )
          {
 #ifndef FEATURE_SUPRESS_SIM_REFRESH_ACTION
            MSG_HIGH_DS_0(REG_SUB, "=REG= SIM_REFRESH_FCN_IND");
            reg_state_next_p = reg_state_curr_p->process_sim_refresh_fcn_ind
            (
              &(reg_cmd_p->cmd.sim_refresh_fcn_ind)
            );
 #else
            MSG_HIGH_DS_0(REG_SUB, "=REG= SIM_REFRESH_FCN_IND. No Action taken.");
 #endif
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= SIM_REFRESH_FCN_IND - Invalid");
          }
          break;

        case SIM_REG_SUBS_READY_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= SIM_REG_SUBS_READY_IND");
          reg_state_process_sim_subscription_ready
          (
          &(reg_cmd_p->cmd.sim_subs_ready_ind)
          );
          break;
        case SIM_REG_CLIENT_ID_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= SIM_REG_CLIENT_ID_IND");
          reg_state_process_sim_client_id
          (
          &(reg_cmd_p->cmd.sim_client_id_ind)
          );
          break;
        case REG_SIM_CARD_UNAVAILABLE_IND:
          MSG_HIGH_DS_0(REG_SUB, "=REG= REG_SIM_CARD_UNAVAILABLE_IND");
          reg_state_process_sim_card_unavailable();
          break;
        default:
          MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown Message from GSDI = %d", reg_cmd_p->cmd.hdr.message_id);
          break;
      }

      break;

     case MS_REG_REG:
       switch(reg_cmd_p->cmd.hdr.message_id)
       {
#ifdef FEATURE_MODEM_CONFIG_REFRESH
         case REG_MCFG_REFRESH_IND:
           MSG_HIGH_DS_0(REG_SUB, "=REG= REG_MCFG_REFRESH_IND");
           reg_state_process_mcfg_refresh_ind(reg_cmd_p);
           break;
#endif
         default:
          break;
       }
       break;

    default:
      MSG_ERROR_DS_1(REG_SUB, "=REG= Unknown message set (%d)", reg_cmd_p->cmd.hdr.message_set);
      break;
  }

  /*
  ** Log the reg_state if the state changed
  */
  if (reg_state_curr_p != reg_state_next_p)
  {
    reg_log_reg_packet
    (
      reg_state_next_p->number(),
      reg_mode_network_selection_mode_get()
    );
    reg_state_next_p->output_name();
  }

  reg_state_curr_p = reg_state_next_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_cm_transaction_id_get

==============================================================================*/

reg_cm_transaction_id_type reg_state_cm_transaction_id_get
(
  void
)
{
  return reg_state_cm_transaction_id;
}


/*==============================================================================

FUNCTION NAME

  reg_state_print_service_state

==============================================================================*/

void reg_state_print_service_state
(
  sys_plmn_service_state_s_type service_state
)
{
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  char    plmn_type;
  uint32  mcc;
  uint32  mnc;

  if (service_state.roaming_ind == SYS_ROAM_STATUS_OFF)
  {
    plmn_type = 'H';
  }
  else
  {
    plmn_type = 'V';
  }

  sys_plmn_get_mcc_mnc
  (
    service_state.plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

  if (service_state.service_status == SYS_SRV_STATUS_NO_SRV)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= NO_SERVICE");
  }
  else if ((service_state.service_status == SYS_SRV_STATUS_LIMITED) ||
           (service_state.service_status == SYS_SRV_STATUS_LIMITED_REGIONAL))
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG= LIMITED_SERVICE on %cPLMN(%d-%d)", plmn_type, mcc, mnc);
  }
  else if (service_state.service_status == SYS_SRV_STATUS_SRV)
  {
    switch (service_state.reg_service_domain)
    {
      case SYS_SRV_DOMAIN_CS_ONLY:
        MSG_HIGH_DS_3(REG_SUB, "=REG= CS_SERVICE on %cPLMN(%d-%d)", plmn_type, mcc, mnc);
        break;

      case SYS_SRV_DOMAIN_PS_ONLY:
        MSG_HIGH_DS_3(REG_SUB, "=REG= PS_SERVICE on %cPLMN(%d-%d)", plmn_type, mcc, mnc);
        break;

      case SYS_SRV_DOMAIN_CS_PS:
        MSG_HIGH_DS_3(REG_SUB, "=REG= CS_PS_SERVICE on %cPLMN(%d-%d)", plmn_type, mcc, mnc);
        break;

      case SYS_SRV_DOMAIN_NO_SRV:
      default:
        MSG_HIGH_DS_0(REG_SUB, "=REG= NO_SERVICE");
        break;
    }
  }
}


/*==============================================================================

FUNCTION NAME

  reg_state_service_status_get

==============================================================================*/

sys_srv_status_e_type reg_state_service_status_get
(
  void
)
{
  return reg_state_service_state.service_status;
}

#ifdef FEATURE_DUAL_SIM
/*==============================================================================

FUNCTION NAME

  reg_state_acq_status_get

==============================================================================*/
sys_acq_status_e_type reg_state_acq_status_get
(
  void
)
{
  return reg_state_service_state.acq_status;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_service_status_set

==============================================================================*/

void reg_state_service_status_set
(
  sys_srv_status_e_type service_status
)
{
  reg_state_service_state.service_status = service_status;
}


/*==============================================================================  

FUNCTION NAME

  reg_state_service_lac_get

==============================================================================*/

word reg_state_service_lac_get
(
  void
)
{
  return reg_state_service_state.lac;
}

/*==============================================================================

FUNCTION NAME

  reg_state_service_plmn_get

==============================================================================*/

sys_plmn_id_s_type reg_state_service_plmn_get
(
  void
)
{
  return reg_state_service_state.plmn;
}
#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  reg_state_service_plmn_get_per_subs

==============================================================================*/
sys_plmn_id_s_type reg_state_service_plmn_get_per_subs
(
  sys_modem_as_id_e_type as_id
)
{
  return reg_state_service_state_sim[as_id].plmn;
}
#endif
/*==============================================================================

FUNCTION NAME

  reg_state_plmn_reg_type_get

DESCRIPTION

  Function that returns the current reg_plmn domain type of the MS.

RETURN VALUE

  sys_reg_domain_e_type       The current domain type of the reg_plmn.

==============================================================================*/

sys_reg_domain_e_type reg_state_plmn_reg_type_get
(
  void
)
{
  return reg_state_service_state.plmn_reg_type;
}


/*==============================================================================

FUNCTION NAME

  reg_state_active_rat_get

==============================================================================*/

sys_radio_access_tech_e_type reg_state_active_rat_get
(
  void
)
{
  return reg_state_service_state.active_rat;
}

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_state_delay_fcn_act_hplmn_get

==============================================================================*/

boolean reg_state_delay_fcn_act_hplmn_get
(
  void
)
{
  return reg_state_delay_fcn_act_hplmn;
}

/*==============================================================================

FUNCTION NAME

  reg_state_delay_fcn_act_hplmn_set

==============================================================================*/

void reg_state_delay_fcn_act_hplmn_set
(
  boolean delay_fcn
)
{
  reg_state_delay_fcn_act_hplmn = delay_fcn;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_prev_plmn_get

==============================================================================*/

sys_plmn_id_s_type reg_state_prev_plmn_get
(
  void
)
{
  return reg_state_prev_plmn;
}

/*==============================================================================

FUNCTION NAME

  reg_state_roaming_ind_get

==============================================================================*/

sys_roam_status_e_type reg_state_roaming_ind_get 
(
  void
)
{
  return reg_state_service_state.roaming_ind;
}

/*==============================================================================

FUNCTION NAME

  reg_state_plmn_search_transaction_id_get

==============================================================================*/

byte reg_state_plmn_search_transaction_id_get 
(
  void
)
{
  return reg_state_plmn_search_transaction_id;
}

/*==============================================================================

FUNCTION NAME

  reg_state_plmn_search_transaction_id_set

==============================================================================*/

byte reg_state_plmn_search_transaction_id_set
(
  void
)
{
  reg_state_plmn_search_transaction_id = (reg_state_plmn_search_transaction_id + 1) & 0x0F;

  return reg_state_plmn_search_transaction_id;
}

/*==============================================================================

FUNCTION NAME

  reg_state_reg_service_domain_get

==============================================================================*/

sys_srv_domain_e_type reg_state_reg_service_domain_get
(
  void
)
{
  return reg_state_service_state.reg_service_domain;
}

/*==============================================================================

FUNCTION NAME

  reg_state_no_service_cause_set

==============================================================================*/
#ifdef FEATURE_LTE
void reg_state_no_service_cause_set
(
  sys_no_svc_cause_e_type  no_service_cause
)
{
  reg_state_service_state.no_service_cause = no_service_cause;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_active_rat_set

==============================================================================*/
void reg_state_active_rat_set
(
  sys_radio_access_tech_e_type  rat
)
{
  reg_state_service_state.active_rat = rat;
}
/*==============================================================================

FUNCTION NAME

  reg_state_ignore_rplmnact_set

==============================================================================*/
void reg_state_ignore_rplmnact_set
(
  boolean ignore_rplmnact
)
{
  if(reg_state_ignore_rplmnact != ignore_rplmnact)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= reg_state_ignore_rplmnact change from %d to %d\n",
                                           reg_state_ignore_rplmnact,ignore_rplmnact);
    reg_state_ignore_rplmnact = ignore_rplmnact;
  }
}

/*==============================================================================

FUNCTION NAME

  reg_state_ignore_rplmnact_get

==============================================================================*/
boolean reg_state_ignore_rplmnact_get
(
  void
)
{
  return reg_state_ignore_rplmnact;
}

/*==============================================================================

FUNCTION NAME

  reg_state_camped_ind_received_set

==============================================================================*/
void reg_state_camped_ind_received_set
(
  boolean        flag
)
{
  reg_state_camped_ind_received = flag;
}
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
/*==============================================================================

FUNCTION NAME

  reg_state_rlf_plmn_set

==============================================================================*/

void reg_state_rlf_plmn_set
(
  sys_plmn_id_s_type rlf_plmn
)
{
  reg_state_rlf_plmn = rlf_plmn;
}


/*==============================================================================

FUNCTION NAME

  reg_state_rlf_plmn_get

==============================================================================*/
sys_plmn_id_s_type reg_state_rlf_plmn_get
(
  void
)
{
  return reg_state_rlf_plmn;
}

/*==============================================================================

FUNCTION NAME

  reg_state_rlf_rat_get

==============================================================================*/

sys_radio_access_tech_e_type reg_state_rlf_rat_get
(
  void
)
{
  return reg_state_rlf_rat;
}
#endif 

/*==============================================================================

FUNCTION NAME

  reg_state_user_resel_plmn_rat_get

==============================================================================*/

void reg_state_user_resel_plmn_rat_get
(
  sys_plmn_id_s_type *plmn,
  sys_radio_access_tech_e_type *rat
)
{
   *plmn = reg_state_user_resel_plmn;
   *rat = reg_state_user_resel_plmn_rat;
}
/*==============================================================================

FUNCTION NAME

  reg_state_user_resel_get_rat

==============================================================================*/

sys_radio_access_tech_e_type reg_state_user_resel_get_rat(void)
{
   return reg_state_user_resel_plmn_rat;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_steering_of_roaming_refresh

==============================================================================*/
void reg_state_process_steering_of_roaming_refresh(mmgsdi_refresh_source_enum_type source)
{
  reg_cmd_type *reg_cmd_p;
  boolean is_search_possible = FALSE;

  switch (reg_state_curr_p->number())
  {
     case REG_STATE_REGISTERING:
     case REG_STATE_MANUAL_SEARCH:
     case REG_STATE_INACTIVE:
     case REG_STATE_AUTOMATIC_SEARCH:
     case REG_STATE_IDLE:
       if (((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) ||
             (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) &&
            (reg_state_service_status_get() != SYS_SRV_STATUS_NO_SRV))
       {
          is_search_possible = TRUE;
       }
      break;
    case REG_STATE_DEREGISTERING:
    case REG_STATE_OFFLINE:
     default:
      break;
   }
   if (is_search_possible)
   {
      if(reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH)
      {
          if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
          {
             reg_timers_start_hplmn_search_timer_10sec();
          }
          else
          { 
              /* Roaming is off if the IRAT HPLMN search is ongoing*/
              reg_state_start_irat_timers_with_10sec();

          }
        reg_state_next_p = &reg_state_idle;
        reg_state_next_p->output_name();        
        reg_send_mmr_plmn_search_abort_req();
      }  
      else
      {
        reg_timers_stop_hplmn_search_period_timer();
        reg_cmd_p = reg_get_cmd_buf();

        if(reg_cmd_p == NULL)
        {
          MSG_FATAL_DS(REG_SUB,"=REG= ERR:Unable to allocate the memory", 0, 0, 0);
        }

        reg_cmd_p->cmd.timer_expiry.IMH.message_set = MS_TIMER;
        reg_cmd_p->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

        reg_cmd_p->cmd.timer_expiry.IMH.message_len_lsb =
                 (sizeof(timer_expiry_T) - sizeof(IMH_T)) % 256;
        reg_cmd_p->cmd.timer_expiry.IMH.message_len_msb =
                 (sizeof(timer_expiry_T) - sizeof(IMH_T)) / 256;

        reg_cmd_p->cmd.timer_expiry.data     = 0;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
        if (reg_as_id == (reg_as_id_e_type)SYS_MODEM_AS_ID_3)
        {
          reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_3;
        }
#endif
        if (reg_as_id == (reg_as_id_e_type)SYS_MODEM_AS_ID_2)
        {
          reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_2;
        }
        else
#endif
        {
          reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER;
        }

        /*
          ** Put on REG command queue
          */
        reg_put_cmd(reg_cmd_p);
      }
      if( source != MMGSDI_REFRESH_SOURCE_BUILTIN_PLMN) 
      {
        reg_sim_refresh_complete(TRUE);
      }
   }
   else
   {
    if( source != MMGSDI_REFRESH_SOURCE_BUILTIN_PLMN)    
     {
       reg_sim_refresh_complete(FALSE);
     }
   }
}

/*==============================================================================

FUNCTION NAME

  reg_state_log_variables

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
void reg_state_log_variables(void)
{
  uint8 i = 0;
  mmgsdi_session_info_query_type    session_info_query;
  mmgsdi_session_get_info_type      session_get_info;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
  for(i = 0; i < MAX_NAS_STACKS ; i++)
#endif  
  {
    boolean local_sim_avail = reg_need_to_process_sim_avail_req_sim[i];
    boolean local_open_session = reg_waiting_for_open_session_cnf_sim[i];
    MSG_HIGH_DS_1( i, "=REG= REG STATE %d", reg_state_curr_p_sim[i]->number());
    /* get SIM CARD MODE */
    session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
    session_info_query.query_key.session_type = reg_sim_mmgsdi_info_sim[i].session_type;
    if (MMGSDI_SUCCESS != mmgsdi_session_get_info_sync(session_info_query, &session_get_info))
    {
      MSG_ERROR_DS_0(i, "=REG= SIM card mode get failed!!!");
    }

    MSG_HIGH_DS_3( i, "=REG= reg_need_to_process_sim_avail_req/waiting_for_open=%x, session_type = %d, reg_sim_card_mode=%d", 
                              ((local_sim_avail<<4)| local_open_session), 
                              reg_sim_mmgsdi_info_sim[i].session_type,
                              session_get_info.app_data.app_type);
  }
}
#else
void reg_state_log_variables(void)
{
  mmgsdi_session_info_query_type    session_info_query;
  mmgsdi_session_get_info_type      session_get_info;
  mmgsdi_app_enum_type reg_sim_card_mode;

  /* get SIM CARD MODE */
  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
  session_info_query.query_key.session_type = reg_sim_mmgsdi_info.session_type;
  if (MMGSDI_SUCCESS != mmgsdi_session_get_info_sync(session_info_query, &session_get_info))
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card mode get failed!!!");
  }
  reg_sim_card_mode = session_get_info.app_data.app_type;
  MSG_HIGH_DS_2(REG_SUB, "=REG= REG STATE %d, reg_sim_card_mode=%d", reg_state_curr_p->number(),reg_sim_card_mode);

  MSG_HIGH_DS_3(REG_SUB, "=REG= reg_need_to_process_sim_avail_req=%d, reg_waiting_for_open_session_cnf=%d, session_type = %d", 
                reg_need_to_process_sim_avail_req, reg_waiting_for_open_session_cnf, reg_sim_mmgsdi_info.session_type);
}
#endif


#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_set_drx_req

==============================================================================*/

reg_state_p_type reg_state_process_cm_set_drx_req
(
  const cm_set_drx_req_s_type* msg_p
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;
  
  reg_send_mmr_set_drx_req(msg_p->drx_coefficient);

  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_set_drx_req

==============================================================================*/

reg_state_p_type reg_state_unexpected_cm_set_drx_req
(
  const cm_set_drx_req_s_type* msg_p
)
{
  reg_state_p_type next_state_p = reg_state_curr_p;
  
  reg_send_cm_set_drx_cnf(FALSE);

  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_proc_mmr_ps_detach_cnf

==============================================================================*/



void reg_state_proc_mmr_ps_detach_cnf
(
  void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= MMR_PS_DETACH_CNF");
  if(reg_state_cm_initiated_ps_detach == TRUE)
  {
    reg_send_cm_ps_detach_cnf();
    reg_state_mm_waiting_for_reg_req = TRUE;
    reg_state_cm_initiated_ps_detach = FALSE;
  }
}


#endif /*FEATURE_LTE*/



/*==============================================================================

FUNCTION NAME

  reg_state_t3245_exp_clean_up

==============================================================================*/

reg_state_p_type reg_state_t3245_exp_clean_up()
{
  sys_sim_state_e_type sim_state = reg_sim_state_get();
  MSG_HIGH_DS_2(REG_SUB, "=REG= t3245 exp clean up function sim_state %d reg_state_cm_initiated_registration %d",
                                                              sim_state,reg_state_cm_initiated_registration);
  /* clear the forbidden list */
  reg_sim_delete_fplmn_and_gprs_fplmn_list();

  if((sim_state == SYS_SIM_STATE_CS_INVALID) ||
     (sim_state == SYS_SIM_STATE_PS_INVALID))
  {
    /* update MM/EMM with the new sim_state */
    reg_send_mmr_sim_state_update_ind(SYS_SIM_STATE_AVAILABLE);
    reg_sim_reset_sim_state();
    if(reg_state_cm_initiated_registration)
    {
      reg_state_cm_sim_update_ind_pending = TRUE;
    }
    else
    {
     /* update CM with the new sim_state */
      reg_send_cm_sim_update_ind(SYS_SIM_STATE_AVAILABLE);
    }
  }
  else if ((sim_state == SYS_SIM_STATE_NOT_AVAILABLE) ||
           (sim_state == SYS_SIM_STATE_CS_PS_INVALID))
  {
     /* Mark sim as legal */
     mmgsdi_session_manage_illegal_subscription(reg_sim_mmgsdi_info.session_id,
                                                MMGSDI_SESSION_APP_IS_LEGAL);
  }
  else 
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= t3245 timer expired in reg_sim_state %d", 
                                                           sim_state); 
  }
  return reg_state_curr_p;
}


/*==============================================================================

FUNCTION NAME

  reg_state_process_t3245_expiry

==============================================================================*/

reg_state_p_type reg_state_process_t3245_expiry(void)
{
  sys_sim_state_e_type sim_state = reg_sim_state_get();
  MSG_HIGH_DS_2(REG_SUB, "=REG= t3245 expiry handling service status %d sim_state %d",
             reg_state_service_state.service_status,sim_state);
  /*
  ** If camped on an FPLMN in manual mode and UE reported limited regional to CM
  ** then start the PLMN selection
  */
  if((reg_state_service_state.service_status ==  SYS_SRV_STATUS_LIMITED_REGIONAL) &&
       (reg_state_service_state.plmn_forbidden) &&
       ((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
        (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) &&
      (sim_state == SYS_SIM_STATE_AVAILABLE) &&
      (reg_state_cm_initiated_registration == FALSE))
  {
    reg_sim_delete_fplmn_and_gprs_fplmn_list();
    reg_state_service_state.service_status = SYS_SRV_STATUS_LIMITED;
    reg_send_cm_service_ind(reg_state_service_state);
  }
  if(((reg_state_service_state.service_status == SYS_SRV_STATUS_SRV) ||
      (reg_state_service_state.service_status ==  SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
#ifdef FEATURE_DUAL_SIM
     (reg_sim_plmn_gprs_forbidden(reg_state_service_state.plmn, (sys_modem_as_id_e_type)reg_sub_id)) &&
#else
     (reg_sim_plmn_gprs_forbidden(reg_state_service_state.plmn)) &&
#endif
     (sim_state == SYS_SIM_STATE_AVAILABLE))
  {
    reg_sim_delete_fplmn_and_gprs_fplmn_list();
    /*
    ** ABORT ongoing manual search if new CM_SERVICE_REQ is received.
    */
    if((reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH) ||
        (reg_state_curr_p->number() == REG_STATE_AUTOMATIC_SEARCH))
    {
      reg_send_mmr_plmn_search_abort_req();
    }
    if(reg_state_curr_p->number() == REG_STATE_MANUAL_SEARCH)
    {
      sys_found_plmn_list_u_type *available_list = NULL;
#ifdef FEATURE_MODEM_HEAP
      available_list = (sys_found_plmn_list_u_type *)modem_mem_calloc(1, sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
      available_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeof(sys_found_plmn_list_u_type));
#endif
      reg_check_for_null_ptr((void*)available_list );
      
      
	  reg_send_cm_network_list_cnf
      (
       reg_state_cm_transaction_id_get(),
       reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
       available_list,
       FALSE /* Search completed FALSE */
      );
#ifndef FEATURE_MODEM_HEAP
      gs_free(available_list);
#else
      modem_mem_free(available_list, MODEM_MEM_CLIENT_NAS);
#endif
     available_list = NULL; 
    }
    reg_mode_start_plmn_selection_on_specified_plmn
    (
      reg_state_service_state.plmn,
      reg_mode_network_selection_mode_get(),
      reg_mode_mode_pref_get(),
      FALSE,
      FALSE,
      reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
      reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_rat_pri_list_get(),
#endif
      reg_state_service_state.active_rat
#ifdef FEATURE_LTE
     ,reg_mode_sms_only_get()
     ,0
#endif 
     ,MMR_REG_REQ_NORMAL
    );
    return &reg_state_registering;
  }
  else
  {
    reg_state_t3245_exp_clean_up();
  }
  return reg_state_curr_p;
}

/*==============================================================================

FUNCTION NAME

  reg_state_process_validate_sim_timer_expiry

==============================================================================*/

void reg_state_process_validate_sim_timer_expiry(void)
{
  sys_sim_state_e_type sim_state = reg_sim_state_get();
  if((sim_state == SYS_SIM_STATE_CS_INVALID) ||
     (sim_state == SYS_SIM_STATE_PS_INVALID))
  {
    /* update MM/EMM with the new sim_state */
    reg_send_mmr_sim_state_update_ind(SYS_SIM_STATE_AVAILABLE);
    reg_sim_reset_sim_state();
    if(reg_state_cm_initiated_registration)
    {
      reg_state_cm_sim_update_ind_pending = TRUE;
    }
    else
    {
     /* update CM with the new sim_state */
      reg_send_cm_sim_update_ind(SYS_SIM_STATE_AVAILABLE);
    }
  }
  else if ((sim_state == SYS_SIM_STATE_NOT_AVAILABLE) ||
           (sim_state == SYS_SIM_STATE_CS_PS_INVALID))
  {
     /* Mark sim as legal */
     mmgsdi_session_manage_illegal_subscription(reg_sim_mmgsdi_info.session_id,
                                                MMGSDI_SESSION_APP_IS_LEGAL);
  }
  else 
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= Validate Sim timer expired in reg_sim_state %d", 
                                                           sim_state); 
  }
}

#ifdef FEATURE_SGLTE

void reg_state_validate_timer_split_operation(void)
{
   boolean validation_allowed = FALSE;
   uint8 validate_timer_counter ;
   reg_as_id_e_type reg_temp_id = reg_as_id;

   rex_timer_cnt_type validate_sim_timer ;
   sys_sim_state_e_type reg_sim_state;
   reg_as_id = reg_ps_stack_id;
   validate_timer_counter = reg_mode_get_validate_sim_counter();
   validate_sim_timer = reg_timer_get_validate_sim_timer();
   reg_sim_state = reg_sim_state_get();
   MSG_HIGH_DS_3(REG_SUB, "=REG= Validate Sim timer value on mode change %d(sec) validate SIM counter %d sim state %d ", 
                                                          (rex_timer_cnt_type)(validate_sim_timer/1000),validate_timer_counter,reg_sim_state);
  if( reg_mode_check_validate_sim_counter())
  {
    validation_allowed = TRUE;
  }
  reg_timers_stop_validate_sim_timer();
  if( (reg_sim_state == SYS_SIM_STATE_CS_PS_INVALID 
      || reg_sim_state == SYS_SIM_STATE_PS_INVALID) && validation_allowed == TRUE)
  {
     reg_timers_start_validate_sim_timer_per_stack(validate_sim_timer);
     reg_sim_state_set(reg_sim_state);
     if(reg_nv_max_validate_sim_counter_get() != REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER)
     {
       reg_mode_set_validate_sim_counter(validate_timer_counter);
     }
     MSG_HIGH_DS_2(REG_SUB, "=REG= Validate Sim timer counter PS stack value  %d sim state %d ", validate_timer_counter,reg_sim_state);
   }
   reg_as_id = REG_SGLTE_STACK_2;
   reg_timers_stop_validate_sim_timer();
   if( (reg_sim_state == SYS_SIM_STATE_CS_PS_INVALID 
       || reg_sim_state == SYS_SIM_STATE_CS_INVALID) && validation_allowed == TRUE) 
   {
     reg_timers_start_validate_sim_timer_per_stack(validate_sim_timer);
     reg_sim_state_set(reg_sim_state);
     if(reg_nv_max_validate_sim_counter_get() != REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER)
     {
       reg_mode_set_validate_sim_counter(validate_timer_counter);
     }
     MSG_HIGH_DS_2(REG_SUB, "=REG= Validate Sim timer counter CS stack value  %d sim state %d ", validate_timer_counter,reg_sim_state);    
   }
   reg_as_id = reg_temp_id ;
}

void reg_state_validate_timer_merge_operation(void)
{
   rex_timer_cnt_type validate_sim_timer_ps, validate_sim_timer_cs;
   uint8 validate_timer_counter ;
   reg_as_id_e_type reg_temp_id = reg_as_id;
   boolean validation_allowed = FALSE;


   uint8 validate_sim_counter_ps, validate_sim_counter_cs;
   uint8 max_validate_counter_value ;
   sys_sim_state_e_type reg_sim_state_ps_stack, reg_sim_state_cs_stack;

   reg_as_id = REG_SGLTE_STACK_2;
   validate_sim_timer_cs = reg_timer_get_validate_sim_timer();
   validate_sim_counter_cs = reg_mode_get_validate_sim_counter();;
   reg_sim_state_cs_stack = reg_sim_state_get();
   reg_timers_stop_validate_sim_timer();

   reg_as_id = reg_ps_stack_id;
   validate_sim_timer_ps = reg_timer_get_validate_sim_timer();
   max_validate_counter_value = reg_nv_max_validate_sim_counter_get();
   validate_sim_counter_ps = reg_mode_get_validate_sim_counter();
   reg_sim_state_ps_stack = reg_sim_state_get();
   reg_timers_stop_validate_sim_timer();

   MSG_HIGH_DS_3(REG_SUB, "=REG= Validate Sim timer value on mode change PS stack =%d(sec), PS validate counter = %d sim state PS = %d ", 
                                                         (rex_timer_cnt_type)(validate_sim_timer_ps/1000), validate_sim_counter_ps,reg_sim_state_ps_stack);
   MSG_HIGH_DS_3(REG_SUB, "=REG= Validate Sim timer value on mode change CS stack =%d(sec), CS validate counter = %d sim state CS = %d ", 
                                                         (rex_timer_cnt_type)(validate_sim_timer_cs/1000), validate_sim_counter_cs,reg_sim_state_cs_stack);
   if((max_validate_counter_value == REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER) || 
      (validate_sim_counter_ps < max_validate_counter_value) ||
      (validate_sim_counter_cs < max_validate_counter_value))
   { 
     if(max_validate_counter_value != REG_NV_DEFAULT_MAX_VALIDATE_SIM_COUNTER)
     {
        validate_timer_counter = MIN(validate_sim_counter_cs,validate_sim_counter_ps);
        reg_mode_set_validate_sim_counter(validate_timer_counter);
        MSG_HIGH_DS_1(REG_SUB, "=REG= Validate Sim timer counter value  %d", validate_timer_counter);
     }
     validation_allowed = TRUE;
   }
   if((( reg_sim_state_ps_stack == SYS_SIM_STATE_CS_PS_INVALID || reg_sim_state_cs_stack == SYS_SIM_STATE_CS_PS_INVALID)||
      ( reg_sim_state_ps_stack == SYS_SIM_STATE_NOT_AVAILABLE ) ||
      (reg_sim_state_cs_stack == SYS_SIM_STATE_CS_INVALID    || reg_sim_state_ps_stack == SYS_SIM_STATE_PS_INVALID  ))&& validation_allowed == TRUE)
   {
      rex_timer_cnt_type min_t, max_t;
      min_t = MIN(validate_sim_timer_cs,validate_sim_timer_ps);
      max_t = MAX(validate_sim_timer_cs,validate_sim_timer_ps);
      if( reg_sim_state_ps_stack == SYS_SIM_STATE_NOT_AVAILABLE)
      {
        reg_sim_state_set(SYS_SIM_STATE_NOT_AVAILABLE);
      }
      else if( (reg_sim_state_ps_stack == SYS_SIM_STATE_CS_PS_INVALID || reg_sim_state_cs_stack == SYS_SIM_STATE_CS_PS_INVALID)||
             (reg_sim_state_ps_stack == SYS_SIM_STATE_PS_INVALID && reg_sim_state_cs_stack == SYS_SIM_STATE_CS_INVALID))
      {
        reg_sim_state_set(SYS_SIM_STATE_CS_PS_INVALID);
      }
      else if(reg_sim_state_cs_stack == SYS_SIM_STATE_CS_INVALID)
      {
        reg_sim_state_set(SYS_SIM_STATE_CS_INVALID);
      }
      else if(reg_sim_state_ps_stack == SYS_SIM_STATE_PS_INVALID)
      {
        reg_sim_state_set(SYS_SIM_STATE_PS_INVALID);
      }
      MSG_HIGH_DS_2(REG_SUB, "=REG= Validate Sim timer value min = %d, max = %d", min_t,max_t);
      if( min_t != 0)
      {
         reg_timers_start_validate_sim_timer_per_stack(min_t);
      }
      else if(max_t != 0)
      {
        reg_timers_start_validate_sim_timer_per_stack(max_t);
      }
    }
    reg_as_id = reg_temp_id;
}


void reg_state_start_appropriate_validate_timer(sys_ue_mode_e_type sglte_mode)
{
   if(sglte_mode == SYS_UE_MODE_SGLTE_HOME || sglte_mode == SYS_UE_MODE_SGLTE_TEST_ONLY)
   {
     reg_state_validate_timer_split_operation();
   }
   else if(sglte_mode == SYS_UE_MODE_SGLTE_ROAM || sglte_mode == SYS_UE_MODE_CSFB_ONLY || sglte_mode == SYS_UE_MODE_NORMAL) 
   {
     reg_state_validate_timer_merge_operation();
   }          
}
#endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
void reg_state_process_cm_mode_change_ind(sys_ue_mode_e_type  reg_mode_change_indication,boolean ue_submode_srlte)
{
#ifdef FEATURE_SGLTE
  sys_ue_mode_e_type sglte_previous_mode = reg_sglte_ue_mode;
  reg_sub_id_stack[REG_SGLTE_STACK_2] = reg_sub_id;
  reg_ps_stack_id = reg_sub_id;
  reg_sglte_ue_mode = reg_mode_change_indication;
  if( REG_SUB_IS_SGLTE &&((reg_state_curr_p_sim[reg_ps_stack_id] != &reg_state_inactive )|| 
     (reg_state_curr_p_sim[REG_SGLTE_STACK_2] != &reg_state_inactive)))
  {
    MSG_FATAL_DS(REG_SUB, "=REG= CM is sending the UE mode change without stopping the both the stacks", 0, 0, 0);
  }
  if(sglte_previous_mode != reg_sglte_ue_mode)
  {
    reg_state_start_appropriate_validate_timer(reg_sglte_ue_mode);
  }
  if((SYS_UE_MODE_SGLTE_HOME == reg_sglte_ue_mode)||
                                                  (reg_sglte_ue_mode ==SYS_UE_MODE_SGLTE_TEST_ONLY))
  {
    reg_update_dual_stack_variables();
    reg_mode_update_dual_stack_variables();
    reg_sglte_mode = TRUE;
    reg_cs_stack_id = REG_SGLTE_STACK_2;
    reg_sub_config = SYS_OVERALL_FEATURE_MODE_SGLTE;
  }
  else
  {
    reg_sglte_mode = FALSE;
    reg_cs_stack_id = reg_ps_stack_id;
  }
#endif
  reg_send_mmr_ue_mode_change_ind( reg_mode_change_indication,ue_submode_srlte);
}
#endif

#ifdef FEATURE_DUAL_DATA
void reg_state_process_cm_data_priority_ind(sys_data_priority_e_type  reg_data_prio_ind)
{

   reg_send_mmr_data_priority_ind(reg_data_prio_ind);
}
#endif

#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_sglte_ps_timer_expiration

==============================================================================*/

reg_state_p_type reg_state_unexpected_sglte_ps_timer_expiry
(
  void
)
{
  MSG_ERROR_DS_0(REG_SUB, "=REG= SGLTE PS Timer - Unexpected");

  return reg_state_curr_p;
}

sys_srv_status_e_type reg_state_get_service_status_per_subs(sys_modem_as_id_e_type as_id)
{
 return reg_state_service_state_sim[as_id].service_status;
}

sys_srv_domain_e_type reg_state_get_service_domain_per_subs(sys_modem_as_id_e_type as_id)
{
 return reg_state_service_state_sim[as_id].reg_service_domain;
}

sys_srv_capability_e_type reg_state_get_plmn_service_capability(sys_modem_as_id_e_type as_id)
{
 return reg_state_service_state_sim[as_id].plmn_service_capability;
}

sys_radio_access_tech_e_type reg_state_get_active_rat_per_subs(sys_modem_as_id_e_type as_id)
{
 return reg_state_service_state_sim[as_id].active_rat;
}

void reg_state_process_service_stat_ind(mmr_service_stat_ind_s_type* msg_p)
{

  if(IS_SGLTE_MERGE_IN_PROGRESS)
  {
    /*start stack merging*/
    reg_as_id = reg_cs_stack_id;
    reg_state_process_merge_operation();
    reg_mode_perform_merge_operation();
    reg_sim_perform_merge_operation();
    reg_state_validate_timer_merge_operation();

    reg_cs_stack_id = reg_ps_stack_id;
    reg_as_id = reg_ps_stack_id;
  }
  else if (IS_SGLTE_SPLIT_IN_PROGRESS)
  {
    /*start  Split operation*/
    reg_cs_stack_id = REG_SGLTE_STACK_2;
    reg_state_process_split_operation();
    reg_mode_perform_split_operation();
    reg_sim_perform_split_operation();
    reg_state_validate_timer_split_operation();
    reg_state_sglte_split_in_progress = FALSE;
  }
  reg_state_service_state = msg_p->service_state;

  if(IS_SGLTE_MERGE_IN_PROGRESS)
  {
    reg_state_sglte_merge_in_progress = FALSE;

#ifdef FEATURE_SGLTE
    if( REG_SUB_IS_SGLTE && (! reg_state_get_rlf_started()
#ifdef FEATURE_SGLTE_DUAL_SIM
       && (! reg_state_get_trm_failure_sglte_main())
#endif 
       ) )
#endif
    {
      /* The IRAT timer may be started if the reason for
       ** the stack merge is not RLF and not TRM failure. 
       ** In both of these cases, a 10s timer will start 
       ** in other funtions 
      */
      if (reg_mode_rat_search_start())
      {
         reg_timers_stop_hplmn_search_period_timer();
      }
    }
  }

  if (reg_state_cm_initiated_registration)
  {
    reg_state_cm_initiated_registration = FALSE;

    reg_send_cm_service_cnf
    (
      reg_state_cm_transaction_id,
      reg_state_service_state
    );
  }
  else
  {
    reg_send_cm_stat_ind
    (
      reg_state_service_state
     );
  }

  
}

void reg_state_process_stack_deact_ind(mmr_stack_deact_ind_s_type* msg_p)
{
  reg_timers_stop_sglte_ps_timer();
  reg_send_cm_stack_deact_ind();
  reg_state_sglte_merge_in_progress = TRUE;
   
  /* If the HPLMN IRAT is running, stop it*/
  if (reg_is_timer_active(REG_HPLMN_IRAT_TIMER))
  {
      reg_timers_stop_hplmn_irat_search_period_timer();
  }
}

void reg_state_process_stack_act_ind(mmr_stack_act_ind_s_type* msg_p)
{
  reg_send_cm_stack_act_ind();
  reg_state_sglte_split_in_progress = TRUE;
}

void reg_state_process_merge_operation(void)
{
  reg_as_id_e_type reg_local_as_id;
  reg_state_curr_p_sim[reg_cs_stack_id] = &reg_state_inactive;
  reg_state_next_p_sim[reg_ps_stack_id] = &reg_state_idle;
  reg_state_next_p_sim[reg_cs_stack_id] = &reg_state_inactive;
  reg_state_cm_initiated_registration_sim[reg_cs_stack_id] = FALSE;
  reg_state_service_state_sim[reg_cs_stack_id].service_status = SYS_SRV_STATUS_NO_SRV;
  /*  reg_state_service_state_sim[reg_cs_stack_id]= {
    SYS_SIM_STATE_NOT_AVAILABLE,
    0xFF, 0xFF, 0xFF,
    SYS_SRV_CAPABILITY_NOT_AVAILABLE,
    FALSE,
    SYS_ROAM_STATUS_OFF,
    SYS_SRV_STATUS_NO_SRV,
    SYS_SRV_DOMAIN_NO_SRV,
    SYS_RAT_GSM_RADIO_ACCESS,
#ifdef FEATURE_GSM_BAND_PREF
    SYS_BAND_MASK_EMPTY,
    0,
#endif
    FALSE
  };*/
  reg_last_mm_message_only_for_reg_sim[reg_cs_stack_id] =  FALSE;
  reg_state_prev_plmn_sim[reg_ps_stack_id] = reg_state_prev_plmn_sim[reg_cs_stack_id];
  reg_state_prev_plmn_sim[reg_cs_stack_id].identity[0] = 0xFF;
  reg_state_prev_plmn_sim[reg_cs_stack_id].identity[1] = 0xFF;
  reg_state_prev_plmn_sim[reg_cs_stack_id].identity[2] = 0xFF;
  reg_state_camped_ind_received_sim[reg_ps_stack_id] = reg_state_camped_ind_received_sim[reg_cs_stack_id];//Doubt
  reg_state_user_resel_plmn_sim[reg_cs_stack_id].identity[0] = 0xFF;
  reg_state_user_resel_plmn_sim[reg_cs_stack_id].identity[1] = 0xFF;
  reg_state_user_resel_plmn_sim[reg_cs_stack_id].identity[2] = 0xFF;
  reg_state_user_resel_plmn_rat_sim[reg_cs_stack_id] = SYS_RAT_NONE;
//  reg_state_is_pwr_on_manual_plmn_selection_sim[reg_cs_stack_id] = FALSE;
  reg_state_last_stop_mode_reason_sim[reg_cs_stack_id] = SYS_STOP_MODE_REASON_NONE;
  reg_state_cell_id_sim[reg_ps_stack_id] = reg_state_cell_id_sim[reg_cs_stack_id];
#ifdef FEATURE_GSTK
  reg_need_to_send_gstk_event_sim[reg_cs_stack_id] = TRUE;
//  reg_sate_last_gstk_event_sent_plmn_sim[reg_cs_stack_id] = {0xFF, 0xFF, 0xFF};
//  reg_state_last_gstk_event_sent_svc_sim[reg_cs_stack_id] = SYS_SRV_STATUS_NONE;
#endif

#ifdef FEATURE_SGLTE_DUAL_SIM
  if(reg_is_timer_active(REG_HPLMN_TIMER_SIM_ID_3))
#else 
  if(reg_is_timer_active(REG_HPLMN_TIMER_SIM_ID_2))
#endif 
  {
    reg_local_as_id = reg_as_id;
    reg_as_id = REG_SGLTE_STACK_2;
    reg_timers_stop_hplmn_search_period_timer();
    reg_as_id = reg_sub_id;
    reg_timers_start_hplmn_search_period_timer();
    reg_as_id = reg_local_as_id;
  }
  
}

void reg_state_process_split_operation(void)
{
  reg_as_id_e_type reg_local_as_id;
  reg_state_curr_p_sim[reg_cs_stack_id] = &reg_state_idle;
//  reg_state_next_p_sim[reg_ps_stack_id] = &reg_state_inactive;
  reg_state_next_p_sim[reg_cs_stack_id] = &reg_state_idle;
//  reg_state_cm_initiated_registration_sim[reg_cs_stack_id] =  reg_state_cm_initiated_registration_sim[reg_ps_stack_id];
  reg_state_camped_ind_received_sim[reg_cs_stack_id] = reg_state_camped_ind_received_sim[reg_ps_stack_id];//Doubt
  reg_state_service_state_sim[reg_ps_stack_id].cs_service_status = SYS_SRV_STATUS_NO_SRV;
  reg_last_mm_message_only_for_reg_sim[reg_cs_stack_id] = reg_last_mm_message_only_for_reg_sim[reg_ps_stack_id];
  reg_state_prev_plmn_sim[reg_cs_stack_id] = reg_state_prev_plmn_sim[reg_ps_stack_id];
//  reg_state_camped_ind_received_sim[reg_ps_stack_id] = FALSE;  //Doubt
  reg_state_user_resel_plmn_sim[reg_cs_stack_id] =  reg_state_user_resel_plmn_sim[reg_ps_stack_id];
  reg_state_user_resel_plmn_rat_sim[reg_cs_stack_id] = reg_state_user_resel_plmn_rat_sim[reg_ps_stack_id];
//  reg_state_is_pwr_on_manual_plmn_selection_sim[reg_cs_stack_id] = reg_state_is_pwr_on_manual_plmn_selection_sim[reg_ps_stack_id];
//  reg_state_last_stop_mode_reason_sim[reg_cs_stack_id] = SYS_STOP_MODE_REASON_NONE;
if(((reg_sub_id == REG_AS_ID_1) && (reg_is_timer_active(REG_HPLMN_TIMER))) ||
    ((reg_sub_id == REG_AS_ID_2) && (reg_is_timer_active(REG_HPLMN_TIMER_SIM_ID_2)))
   )
  {
    reg_local_as_id = reg_as_id;
    reg_as_id = reg_ps_stack_id;
    reg_timers_stop_hplmn_search_period_timer();
    reg_as_id = reg_local_as_id;
    reg_timers_start_hplmn_search_period_timer();
  }

}

void reg_state_set_rlf_started(boolean value)
{
  reg_state_rlf_started = value;
}

boolean reg_state_get_rlf_started(void)
{
 return reg_state_rlf_started;
}

/*==============================================================================

FUNCTION NAME

  reg_state_sglte_hybrid_stack_inactive

==============================================================================*/
boolean reg_state_sglte_hybrid_stack_inactive(void)
{
  if (((reg_state_curr_p_sim[reg_cs_stack_id]->number() == REG_STATE_INACTIVE) || 
     (reg_state_curr_p_sim[reg_cs_stack_id]->number() == REG_STATE_DEREGISTERING) ||
     (reg_state_curr_p_sim[reg_cs_stack_id]->number() == REG_STATE_OFFLINE)) &&
     (reg_state_last_stop_mode_reason_sim[reg_cs_stack_id] == SYS_STOP_MODE_REASON_NONE))
  {
    return TRUE;
  }
  return FALSE;
}
#endif

void reg_state_process_pseudo_resel_low_priority_ind(mmr_rrc_pseudo_resel_low_priority_ind_s_type mmr_pseudo_resel_low_priority_ind)
{
  boolean found_in_available_list;
  boolean                  plmn_id_is_undefined;
  boolean                  mnc_includes_pcs_digit;
  uint32                   mcc;
  uint32                   mnc;
  uint32  earfcn = mmr_pseudo_resel_low_priority_ind.earfcn;
  MSG_HIGH_DS_1(REG_SUB, "=REG= MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND earfcn = %u",
                mmr_pseudo_resel_low_priority_ind.earfcn);
  sys_plmn_get_mcc_mnc
  (
     reg_state_lte_available_cell_id.plmn,
     &plmn_id_is_undefined,
     &mnc_includes_pcs_digit,
     &mcc,
     &mnc
  );
  if(mnc_includes_pcs_digit)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu-%03lu", mcc, mnc);
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu- %02lu", mcc, mnc);
  }
  MSG_HIGH_DS_3(REG_SUB, "=REG= RAT = %d LAC = %d Cell ID = %d",reg_state_lte_available_cell_id.rat,reg_state_lte_available_cell_id.lac,reg_state_lte_available_cell_id.cell_id); 
  found_in_available_list = reg_is_cell_present_in_list(reg_state_lte_available_list_ptr, reg_state_lte_available_cell_id.plmn,reg_state_lte_available_cell_id.rat,reg_state_lte_available_cell_id.lac, reg_state_lte_available_cell_id.cell_id);             
  if(found_in_available_list && reg_state_lte_available_cell_id.rat ==  SYS_RAT_UMTS_RADIO_ACCESS)
  {
    (void)reg_state_add_or_delete_earfcn_to_lte_available_list(reg_state_lte_available_cell_id.rat,reg_state_lte_available_cell_id.plmn,reg_state_lte_available_cell_id.lac,reg_state_lte_available_cell_id.cell_id,earfcn,FALSE);
     MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE available list"); 
     reg_state_dump_the_list(reg_state_lte_available_list_ptr);

  }
}

word reg_get_cell_id(void )
{
  return reg_state_cell_id;
}
lte_cell_list_type* reg_state_add_cell_id_to_lte_list
(
  lte_cell_list_type* list_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,                                       
  sys_lac_type lac,
  word cell_id,
  rex_timer_cnt_type timer_count
)
{
  lte_cell_list_type* temp_ptr;
  lte_cell_list_type *prev_ptr = NULL;
  cell_list_type    *curr_cell_list_ptr = NULL;
  cell_list_type    *prev_cell_list_ptr = NULL;

  dword               list_length =0;

  if(cell_id == 0xFFFF)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN: Cell ID is not valid, don't add to the list");
    return list_ptr;
  }
  if(list_ptr == NULL)
  {
    list_ptr = reg_state_create_new_plmn_rat_list_entry(plmn,rat,lac,cell_id,timer_count);  
    temp_ptr = list_ptr;
  }
  else
  {
   temp_ptr = list_ptr;

   while(temp_ptr != NULL)
   {
     list_length = list_length + temp_ptr->length;
     temp_ptr = temp_ptr->next_ptr;
   }
   temp_ptr = list_ptr;
   if(list_length == REG_MAX_LTE_AVAILABLE_CELL_NUM)
   {
     MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN: List length reached 1000 so deleting one entry");
     list_ptr = reg_delete_last_cell_entry_from_list(list_ptr);
   }
    //Find the PLMN entry and rat 
    while(list_ptr != NULL)
    {
      if(sys_plmn_match(list_ptr->plmn,plmn) && list_ptr->rat == rat)                      
      {
        break;
      }
      else
      {
        prev_ptr = list_ptr;
        list_ptr = list_ptr->next_ptr;
      }
    }
    //if PLMN entry and rat exists check if lac entry exists as well.                                         
    if(list_ptr != NULL)
    {
      curr_cell_list_ptr = list_ptr->cell_ptr;
      while(curr_cell_list_ptr != NULL)
      {
        if(curr_cell_list_ptr->lac == lac && 
             curr_cell_list_ptr->cell_id == cell_id)
        {
          /* reset the count if cell already present in lte available list*/
          curr_cell_list_ptr->count = 0;
          break;
        }
        else
        {
          prev_cell_list_ptr = curr_cell_list_ptr;
          curr_cell_list_ptr = curr_cell_list_ptr->next_ptr;
        }
      }
    }
     //if PLMN entry is not found create a new entry for PLMN /RAT and corresponding lac list.                   
    if(list_ptr == NULL && prev_ptr!=NULL)
    {
      prev_ptr->next_ptr = reg_state_create_new_plmn_rat_list_entry(plmn,rat,lac,cell_id,timer_count);  
    }
     
    else if(curr_cell_list_ptr == NULL)
    {
      if ( prev_cell_list_ptr == NULL )
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN: List is NULL");
        return NULL;
      }
      list_ptr->length = list_ptr->length+1;
      prev_cell_list_ptr->next_ptr = reg_state_create_new_cell_list_entry(lac,cell_id,timer_count);            
    }
  }
  return temp_ptr;
}

lte_cell_list_type* reg_state_create_new_plmn_rat_list_entry                    
(
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,                                 
  sys_lac_type lac,
  word cell_id,
  rex_timer_cnt_type timer_count
)
{
  lte_cell_list_type *lte_cell_list_entry;
  lte_cell_list_entry = (lte_cell_list_type*) modem_mem_alloc(sizeof(lte_cell_list_type),MODEM_MEM_CLIENT_NAS);
  reg_check_for_null_ptr((void*)lte_cell_list_entry );
  lte_cell_list_entry->plmn = plmn;
  lte_cell_list_entry->rat  = rat;                                  
  lte_cell_list_entry->length = 1;
  lte_cell_list_entry->cell_ptr = reg_state_create_new_cell_list_entry(lac,cell_id,timer_count);    
  lte_cell_list_entry->next_ptr = NULL;
  return lte_cell_list_entry;
}

cell_list_type* reg_state_create_new_cell_list_entry(sys_lac_type lac,word cell_id, rex_timer_cnt_type timer_count )       
{
  cell_list_type * cell_list_entry;
  uint8 i=0;

  cell_list_entry = (cell_list_type*) modem_mem_alloc(sizeof(cell_list_type),MODEM_MEM_CLIENT_NAS);

  reg_check_for_null_ptr((void*)cell_list_entry ) ;
  cell_list_entry->cell_id = cell_id;
  cell_list_entry->lac = lac;
  cell_list_entry->next_ptr = NULL;
  cell_list_entry->count = 0;
  cell_list_entry->earfcn.count = 0;
  for(i = 0 ; i<NAS_MAX_EARFCN_COUNT;i++)
  {
    cell_list_entry->earfcn.earfcn_nas[i]=INVALID_EARFCN;
  }
  cell_list_entry->timer_count = timer_count;

  return cell_list_entry;
}

boolean reg_is_cell_present_in_list   
(

  lte_cell_list_type* list_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,                    
  sys_lac_type lac,
  word cell_id 
)
{
  cell_list_type* cell_list_temp;
  boolean return_value = FALSE;

  if(list_ptr == NULL)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : List is empty, returning value = %d",return_value);
    return return_value;
  }
  else
  {
    while (list_ptr != NULL)
    {
      if(sys_plmn_match(list_ptr ->plmn,plmn ) && list_ptr ->rat == rat)                 
      {
        cell_list_temp = list_ptr->cell_ptr;
        while (cell_list_temp != NULL)
        {
          if(cell_list_temp->cell_id == cell_id &&
              cell_list_temp->lac== lac
            )
          {
            return_value = TRUE;
            MSG_HIGH_DS_1(REG_SUB, "=REG=  LTE IRAT BPLMN : cell entry found in lists, returning value = %d",return_value);
            return return_value;
          }
          else
          {
            cell_list_temp = cell_list_temp->next_ptr;
          }
        }
      }
      list_ptr = list_ptr->next_ptr;
    }
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG=  LTE IRAT BPLMN : is cell entry found in lists (1 found ,0 not found), value = %d",return_value);
  return return_value;
}  

lte_cell_list_type* reg_delete_cell_entry_from_list(lte_cell_list_type* lte_list_ptr,sys_plmn_id_s_type plmn,sys_radio_access_tech_e_type rat, sys_lac_type lac,word cell_id )
{
  lte_cell_list_type* curr_lte_list_ptr;
  lte_cell_list_type* prev_lte_list_ptr = NULL;                          
  cell_list_type* curr_cell_node;
  cell_list_type* prev_cell_node = NULL;

  curr_lte_list_ptr = lte_list_ptr;
  if(curr_lte_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : List is empty ");
    return curr_lte_list_ptr;
  }
  else
  {
    while (curr_lte_list_ptr != NULL)
    {
      if(sys_plmn_match(curr_lte_list_ptr->plmn,plmn) && curr_lte_list_ptr->rat == rat)               
      {
        curr_cell_node = curr_lte_list_ptr->cell_ptr;
        while(curr_cell_node != NULL)
        {
          if(curr_cell_node->cell_id == cell_id &&
                curr_cell_node->lac == lac)
          {

            if(prev_cell_node != NULL)
            {
              prev_cell_node->next_ptr = curr_cell_node->next_ptr;
            }
            else
            {
              curr_lte_list_ptr->cell_ptr = curr_cell_node->next_ptr;
            }
            curr_lte_list_ptr->length = curr_lte_list_ptr->length - 1;
            modem_mem_free(curr_cell_node,MODEM_MEM_CLIENT_NAS);
            if(curr_lte_list_ptr->length == 0)
            {
              if(prev_lte_list_ptr != NULL)                                              
              {
                prev_lte_list_ptr->next_ptr = curr_lte_list_ptr->next_ptr;               
              }
              else
              {
                lte_list_ptr = lte_list_ptr->next_ptr;
              }
              modem_mem_free(curr_lte_list_ptr,MODEM_MEM_CLIENT_NAS);
            }
            break;
          }
          else
          {
            prev_cell_node = curr_cell_node;
            curr_cell_node = curr_cell_node->next_ptr;
          }
        }
        break;
      }
      else
      {
        prev_lte_list_ptr = curr_lte_list_ptr;                                            
       curr_lte_list_ptr = curr_lte_list_ptr->next_ptr;
      }
    }
  }
  return lte_list_ptr;
}

lte_cell_list_type* reg_delete_last_cell_entry_from_list(lte_cell_list_type* list_ptr)
{

  lte_cell_list_type* lte_list_local_ptr = NULL;
  lte_cell_list_type* lte_list_prev_ptr = NULL;
  cell_list_type* cell_list_ptr = NULL;
  cell_list_type* prev_cell_list_ptr = NULL;


  lte_list_local_ptr = list_ptr;
  while(lte_list_local_ptr != NULL)
  {
    lte_list_prev_ptr = lte_list_local_ptr;
    lte_list_local_ptr = lte_list_local_ptr->next_ptr;
  }
  if(lte_list_prev_ptr != NULL)
  {
    lte_list_local_ptr = lte_list_prev_ptr;
  }
  if(lte_list_local_ptr != NULL)
  {
    cell_list_ptr = lte_list_local_ptr->cell_ptr;
  }
  while(cell_list_ptr != NULL)
  {
    prev_cell_list_ptr = cell_list_ptr;
    cell_list_ptr = cell_list_ptr->next_ptr;
  }
    if(prev_cell_list_ptr != NULL)
  {
    cell_list_ptr = prev_cell_list_ptr;
  }
  if(cell_list_ptr != NULL)  
  {
    list_ptr = reg_delete_cell_entry_from_list(list_ptr,lte_list_local_ptr->plmn, lte_list_local_ptr->rat, cell_list_ptr->lac,cell_list_ptr->cell_id );   
  }
  return list_ptr;
}


byte* reg_convert_lte_list_into_array(lte_cell_list_type* lte_list_ptr, uint32 *length,boolean lte_available)  
{
  lte_cell_list_type* list_ptr;
  uint32 total_size_for_array = 0;
  earfcn_nas_type earfcn;               
  byte *byte_array=NULL;
  byte byte_to_write =0;
  uint32 write_start_index=0;
  uint32 earfcn_size;
  
  memset(&earfcn, NULL, sizeof(earfcn_nas_type));

  if(lte_available)
  {
    earfcn_size = sizeof(uint8)+sizeof(earfcn.earfcn_nas);
  }
  else
  {
    earfcn_size = 0;
  }

  list_ptr = lte_list_ptr;
  reg_state_dump_the_list(lte_list_ptr);

  if(list_ptr == NULL)
  {
    return NULL;
  }

  reg_timers_adjust_backoff_lte_search_timer_count(list_ptr);
  while(list_ptr != NULL)
  {
    total_size_for_array = total_size_for_array + (sizeof(sys_plmn_id_s_type));
    total_size_for_array = total_size_for_array + (sizeof(sys_radio_access_tech_e_type));      
    total_size_for_array = total_size_for_array + sizeof(uint32);
    total_size_for_array = total_size_for_array +(list_ptr->length * (LAC_SIZE + sizeof(word)+ earfcn_size +sizeof(int8)+sizeof(rex_timer_cnt_type)));
    list_ptr = list_ptr->next_ptr;
  }

  total_size_for_array = sizeof(uint32) + total_size_for_array;
  byte_array = (byte*)modem_mem_alloc(total_size_for_array,MODEM_MEM_CLIENT_NAS);

  *length = total_size_for_array;
  
  if (byte_array == NULL)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : Memory allocation failed for list array of size %d", total_size_for_array);
    MSG_FATAL_DS(REG_SUB, "=REG= LTE IRAT BPLMN : failed to allocate memory of size %d", total_size_for_array, 0, 0);
    return byte_array;
  }

  byte_to_write = sizeof(uint32);
  memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&total_size_for_array,byte_to_write);
  write_start_index += byte_to_write;

  list_ptr = lte_list_ptr;

  while(list_ptr != NULL)
  {
     cell_list_type  *cell_ptr_temp = list_ptr->cell_ptr;
    /*write plmmn to byte array  */
    byte_to_write = sizeof(sys_plmn_id_s_type);
    memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)list_ptr->plmn.identity,byte_to_write);
    write_start_index += byte_to_write;
    /*write rat to byte array  */
    byte_to_write = sizeof(sys_radio_access_tech_e_type);                  
    memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&list_ptr->rat,byte_to_write);
    write_start_index += byte_to_write;

    /*write length ofcell id list to the byte array*/
    byte_to_write = sizeof(uint32);
    memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&list_ptr->length,byte_to_write);
    write_start_index += byte_to_write;
    /*now write the whole lac and timer value */
    while(cell_ptr_temp!=NULL)
    {
      byte_to_write = sizeof(sys_lac_type);
      memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->lac,byte_to_write);
      write_start_index += byte_to_write; 
      byte_to_write = sizeof(word);
      memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->cell_id,byte_to_write);
      write_start_index += byte_to_write; 
      if(lte_available)                            
      {     
        byte_to_write = sizeof(uint8);
        memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->earfcn.count,byte_to_write);
        write_start_index += byte_to_write; 
        byte_to_write = sizeof(earfcn.earfcn_nas);
        memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->earfcn.earfcn_nas[0],byte_to_write);
        write_start_index += byte_to_write;
      }
      /*count*/
      byte_to_write = sizeof(int8);
      memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->count,byte_to_write);
      write_start_index += byte_to_write; 

      byte_to_write = sizeof(rex_timer_cnt_type);
      memscpy((void *)&byte_array[write_start_index],byte_to_write,(void *)&cell_ptr_temp->timer_count,byte_to_write);
      write_start_index += byte_to_write; 
      cell_ptr_temp = cell_ptr_temp->next_ptr ;
    }
    list_ptr = list_ptr->next_ptr;
  }
  return byte_array;
}

void reg_state_clear_lte_list(lte_cell_list_type *list_ptr)
{
  lte_cell_list_type *next_ptr;
  cell_list_type *next_cell_list_ptr;
  while(list_ptr != NULL)
  {
    next_ptr = list_ptr->next_ptr;
    //clear cell list first
    while(list_ptr->cell_ptr != NULL)
    {
      next_cell_list_ptr = list_ptr->cell_ptr->next_ptr;
      modem_mem_free(list_ptr->cell_ptr, MODEM_MEM_CLIENT_NAS);
      list_ptr->cell_ptr = next_cell_list_ptr;
    }
    //clear the PLMN entry
    modem_mem_free(list_ptr, MODEM_MEM_CLIENT_NAS);
    list_ptr = next_ptr;
  }
  list_ptr = NULL;
}

int8 reg_state_increment_cell_list_counter
(
  lte_cell_list_type* lte_list_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,
  sys_lac_type lac,
  word cell_id, 
  int8 count
)
{
  if(!lte_list_ptr)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : List pointer is empty,");
    return 0; 
  }
  else
  {
    lte_cell_list_type*  list_ptr = NULL;
    cell_list_type*      curr_cell_list_ptr = NULL;
    list_ptr = lte_list_ptr;
    while(list_ptr != NULL)
    {
      if(sys_plmn_match(list_ptr->plmn,plmn) && list_ptr->rat == rat)                      
      {
        break;
      }
      else
      {
        list_ptr = list_ptr->next_ptr;
      }
    }
    //if PLMN entry and rat exists check if lac entry exists as well.                                       
    if(list_ptr != NULL)
    {
      curr_cell_list_ptr = list_ptr->cell_ptr;
      while(curr_cell_list_ptr != NULL)
      {
        if(curr_cell_list_ptr->lac == lac && 
           curr_cell_list_ptr->cell_id == cell_id)
        {
          break;
        }
        else
        { 
          curr_cell_list_ptr = curr_cell_list_ptr->next_ptr;
        }
       }
     }
     if(!list_ptr || !curr_cell_list_ptr)
     {
       MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : either Lte available list is empty or no entry is found to increment the count");
       return 0;
     }
     else
     {
       if(count == -1)
       {
         curr_cell_list_ptr->count = curr_cell_list_ptr->count + 1;
       }
       else
       {
         curr_cell_list_ptr->count = count ;               
       }
       MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : Count has been increased to %d", curr_cell_list_ptr->count);
       return curr_cell_list_ptr->count;
     }
   }
}

lte_cell_list_type* reg_convert_byte_array_to_lte_cell_list( lte_cell_list_type* lte_list_ptr, byte *byte_array, boolean lte_available)
{
  byte byte_to_read =0;
  uint32 read_start_index=0;
  uint32 total_length;
  uint32 index=0;
  uint32 cell_list_length =0;
  uint32 cell_length=0;
  uint32 num_of_cells=0; 
  sys_plmn_id_s_type plmn;
  sys_radio_access_tech_e_type rat;                  
  sys_lac_type lac;
  earfcn_nas_type earfcn;            
  word cell_id;
  rex_timer_cnt_type timer_count;
  int8   count;                    
  uint8 earfcn_index =0;
  lte_cell_list_type*  lte_list_temp_ptr = lte_list_ptr;
  byte_to_read = sizeof(uint32);
  memscpy((void *)&total_length,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
  read_start_index+=byte_to_read;
  
  while((read_start_index + sizeof(sys_plmn_id_s_type)+sizeof(sys_radio_access_tech_e_type)+sizeof(uint32)) <= total_length)
  {
    index =0;     
    byte_to_read = sizeof(sys_plmn_id_s_type);
    memscpy(&plmn,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
    read_start_index+=byte_to_read; 
    byte_to_read = sizeof(sys_radio_access_tech_e_type);                
    memscpy(&rat,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
    read_start_index+=byte_to_read;
    byte_to_read = sizeof(uint32);
    memscpy((void*)&cell_list_length,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
    read_start_index+=byte_to_read; 
    
    cell_length = sizeof(sys_lac_type)+sizeof(word)+sizeof(int8)+sizeof(rex_timer_cnt_type);
    if(lte_available == TRUE)
    {
      cell_length = cell_length + sizeof(uint8)+ sizeof(earfcn.earfcn_nas);
    }
    while ((index<cell_list_length) && (read_start_index + cell_length <= total_length))
    {
      if(num_of_cells >= REG_MAX_LTE_AVAILABLE_CELL_NUM)
      {
        /*Number of cells converted from byte array exceed total number of cells possible..
          Incorrect data in byte stream....Clear lte lists and return NULL */
        MSG_HIGH_DS_0(REG_SUB,"=REG= LTE IRAT BPLMN : LTE cell list conversion failed");
        reg_state_clear_lte_list(lte_list_temp_ptr);
        return NULL;
      }
      else
      {
        //Keeps track of total num of cells converted and added to list
        num_of_cells++;
      }
      byte_to_read = sizeof(sys_lac_type);
      memscpy(&lac,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
      read_start_index+=byte_to_read; 
      byte_to_read = sizeof(word);
      memscpy(&cell_id,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
      read_start_index+=byte_to_read;
      if(lte_available == TRUE)          
      {
        /*size of earfcn_nas_type, count + earfcn_nas */
         byte_to_read = sizeof(uint8);
         memscpy(&earfcn.count,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
         read_start_index+=byte_to_read;
         byte_to_read = sizeof(earfcn.earfcn_nas);
         memscpy(&earfcn.earfcn_nas[0],byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
         read_start_index+=byte_to_read;
      }
      byte_to_read = sizeof(int8);
      memscpy(&count,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
      read_start_index+=byte_to_read;

      byte_to_read = sizeof(rex_timer_cnt_type);
      memscpy(&timer_count,byte_to_read,(void*)&byte_array[read_start_index],byte_to_read);
      read_start_index+=byte_to_read;
      lte_list_temp_ptr = reg_state_add_cell_id_to_lte_list( lte_list_temp_ptr, plmn,rat,lac,cell_id,timer_count); 
      (void)reg_state_increment_cell_list_counter(lte_list_temp_ptr,plmn,rat,lac,cell_id,count);
      if(lte_available == TRUE)
      {
        reg_state_lte_available_list_ptr = lte_list_temp_ptr;
        for(earfcn_index=0;earfcn_index<NAS_MAX_EARFCN_COUNT && earfcn_index < earfcn.count ; earfcn_index++)           
        {
          (void)reg_state_add_or_delete_earfcn_to_lte_available_list(rat,plmn,lac,cell_id,earfcn.earfcn_nas[earfcn_index],TRUE);
        }
      }
      index++;
    }   
                   
  }
  reg_state_dump_the_list(lte_list_temp_ptr);
  return lte_list_temp_ptr;
}

void reg_state_clear_lte_lists(void)
{
  lte_cell_list_type *next_ptr;
  cell_list_type *next_cell_list_ptr;
  while(reg_state_lte_available_list_ptr != NULL)
  {
    next_ptr = reg_state_lte_available_list_ptr->next_ptr;
    //clear cell list first
    while(reg_state_lte_available_list_ptr->cell_ptr != NULL)
    {
      next_cell_list_ptr = reg_state_lte_available_list_ptr->cell_ptr->next_ptr;
      modem_mem_free(reg_state_lte_available_list_ptr->cell_ptr, MODEM_MEM_CLIENT_NAS);
      reg_state_lte_available_list_ptr->cell_ptr = next_cell_list_ptr;
    }
    //clear the PLMN entry
    modem_mem_free(reg_state_lte_available_list_ptr, MODEM_MEM_CLIENT_NAS);
    reg_state_lte_available_list_ptr = next_ptr;
  }
  reg_state_lte_available_list_ptr = NULL;


  while(reg_state_lte_non_available_list_ptr != NULL)
  {
    next_ptr = reg_state_lte_non_available_list_ptr->next_ptr;
    while(reg_state_lte_non_available_list_ptr->cell_ptr != NULL)
    {
      next_cell_list_ptr = reg_state_lte_non_available_list_ptr->cell_ptr->next_ptr;
      modem_mem_free(reg_state_lte_non_available_list_ptr->cell_ptr, MODEM_MEM_CLIENT_NAS);
      reg_state_lte_non_available_list_ptr->cell_ptr = next_cell_list_ptr;
    }
    //clear the PLMN entry
    modem_mem_free(reg_state_lte_non_available_list_ptr, MODEM_MEM_CLIENT_NAS);
    reg_state_lte_non_available_list_ptr = next_ptr;
  }
  reg_state_lte_non_available_list_ptr = NULL;


}


void reg_state_cell_present_in_list( byte *found_in_avaialble_list, byte *found_in_non_avaialble_list)
{

  sys_plmn_id_s_type plmn;
  sys_radio_access_tech_e_type rat = SYS_RAT_NONE;   
  sys_lac_type lac = 0xFFFF;
  word cell_id = 0xFFFF;

  if((reg_state_service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS ||                                       
      reg_state_service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS ||
      reg_state_service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) && 
      (reg_state_service_state.service_status != SYS_SRV_STATUS_NO_SRV) 
     )
  {
    plmn = reg_state_service_state.plmn;
    lac = reg_state_service_state.lac;
    rat = reg_state_service_state.active_rat;                                                                  
    cell_id = reg_state_cell_id;
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Checkin if cell is present in LTE Avaiable list");
    *found_in_avaialble_list = reg_is_cell_present_in_list(reg_state_lte_available_list_ptr, plmn,rat,lac, cell_id);             
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Checkin if cell is present in LTE Non Avaiable list");
    *found_in_non_avaialble_list = reg_is_cell_present_in_list(reg_state_lte_non_available_list_ptr, plmn,rat,lac, cell_id);     
  }
  else
  {
    *found_in_avaialble_list = FALSE;
    *found_in_non_avaialble_list = FALSE;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : found in avaialble_list %d found in non avaialble list %d", *found_in_avaialble_list, *found_in_non_avaialble_list);

  if(*found_in_avaialble_list == TRUE)
  {
    reg_state_lte_available_list_ptr = reg_state_move_cell_up_queue(reg_state_lte_available_list_ptr, plmn,rat,lac, cell_id);           
  }
  else if(*found_in_non_avaialble_list == TRUE)
  {
    reg_state_lte_non_available_list_ptr = reg_state_move_cell_up_queue(reg_state_lte_non_available_list_ptr, plmn,rat,lac, cell_id);        
  }

}

boolean reg_state_cell_not_present_in_lte_lists(void)
{
  boolean found_in_avaialble_list = FALSE;
  boolean   found_in_non_avaialble_list = FALSE;
  reg_state_cell_present_in_list( &found_in_avaialble_list, &found_in_non_avaialble_list);
  MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : found in avaialble_list %d found in non avaialble list %d", found_in_avaialble_list, found_in_non_avaialble_list);
  return !(found_in_avaialble_list || found_in_non_avaialble_list);
}

boolean  is_earfcn_param_valid(inter_task_cell_id_type *earfcn_nas_param)
{
  boolean return_value=FALSE;
  if(!earfcn_nas_param)
  {
    return FALSE;
  }
  else
  {
    return_value = (sys_radio_access_tech_is_valid(earfcn_nas_param->rat)&& sys_plmn_id_is_valid(earfcn_nas_param->plmn));
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : is earfcn param valid , return_value = %d", return_value);
    if(earfcn_nas_param->lac == 0xFFFF || earfcn_nas_param->cell_id == 0xFFFF)
    {
      return_value = FALSE;
    }
  }
  return return_value;
}
boolean  get_lte_earfcn_list_from_nas(inter_task_cell_id_type *earfcn_nas_param, earfcn_nas_type *earfcn_ptr)
{
  lte_cell_list_type *list_ptr = NULL;
  cell_list_type    *curr_cell_list_ptr = NULL;
  boolean                  plmn_id_is_undefined;
  boolean                  mnc_includes_pcs_digit;
  uint32                   mcc;
  uint32                   mnc;
  uint8                    i=0;
  uint8 j=0;
  boolean return_value = FALSE;
  if(!earfcn_nas_param)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : caller calling earfcn list function without allocating memory");
    return FALSE;
  }
  if(!is_earfcn_param_valid(earfcn_nas_param))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : caller calling earfcn list function with invalid parameter");
    return FALSE;
  }
  if(!earfcn_ptr)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : caller calling without allocating memory, Active RAT =%d",earfcn_nas_param->rat);
    return FALSE;
  }
#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
  if(reg_nv_pseudo_reselection_allowed() == FALSE && reg_nv_w_bplmn_pseudo_enabled() == FALSE)
  {
    return FALSE;
  }
  else if(reg_nv_pseudo_reselection_allowed() == FALSE && 
         earfcn_nas_param->rat == SYS_RAT_TDS_RADIO_ACCESS )
  {
    return FALSE;
  }
  else if(reg_nv_w_bplmn_pseudo_enabled() == FALSE && earfcn_nas_param->rat == SYS_RAT_UMTS_RADIO_ACCESS)
  {
    return FALSE;
  }
#else
  if(reg_nv_w_bplmn_pseudo_enabled() == FALSE)
  {
    return FALSE;
  }
#endif
  list_ptr = reg_state_lte_available_list_ptr;
  if(!list_ptr)
  {    
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : No entry for the PLMN and RAT, Active RAT =%d",earfcn_nas_param->rat);
    return return_value;
  }
  MSG_HIGH_DS_0(REG_SUB, "=REG= Requested EARFCN is for :");
  sys_plmn_get_mcc_mnc
  (
     earfcn_nas_param->plmn,
     &plmn_id_is_undefined,
     &mnc_includes_pcs_digit,
     &mcc,
     &mnc
  );
  if(mnc_includes_pcs_digit)
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu-%03lu", mcc, mnc);
  }
  else
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu- %02lu", mcc, mnc);
  }
  MSG_HIGH_DS_3(REG_SUB, "=REG= RAT = %d LAC = %d Cell ID = %d",earfcn_nas_param->rat,earfcn_nas_param->lac,earfcn_nas_param->cell_id); 

    
  while(list_ptr != NULL)
  {
      if(sys_plmn_match(list_ptr->plmn,earfcn_nas_param->plmn) && list_ptr->rat == earfcn_nas_param->rat)
      {
        break;
      }
      else
      {
        list_ptr = list_ptr->next_ptr;
      }
  }  
  if(list_ptr != NULL)
  {
    curr_cell_list_ptr = list_ptr->cell_ptr;
    while(curr_cell_list_ptr != NULL)
    {
      if(curr_cell_list_ptr->lac == earfcn_nas_param->lac && 
         curr_cell_list_ptr->cell_id == earfcn_nas_param->cell_id)
      {
        break;
      }
      else
      {
        curr_cell_list_ptr = curr_cell_list_ptr->next_ptr;
      }
    }
  }
  if(!list_ptr || !curr_cell_list_ptr)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : No entry for the PLMN and RAT, Active RAT =%d",earfcn_nas_param->rat);
    return return_value;  
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= Requested EARFCN default count passed = %d :",earfcn_ptr->count);
  memset(earfcn_ptr, 0x0,sizeof(earfcn_nas_type));
  earfcn_ptr->count = 0;
  for(i=0;((i<NAS_MAX_EARFCN_COUNT) && (j < NAS_MAX_EARFCN_COUNT)); i++)
  {     
    if(curr_cell_list_ptr->earfcn.earfcn_nas[i] != INVALID_EARFCN)
    {
      earfcn_ptr->earfcn_nas[j++] = curr_cell_list_ptr->earfcn.earfcn_nas[i];
      earfcn_ptr->count++;
      MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : count value = %d j = %d:",earfcn_ptr->count,j);
    } 
  }
  if(earfcn_ptr->count !=0)
  {
    return_value = TRUE;
    reg_state_lte_available_cell_id.plmn  = earfcn_nas_param->plmn;
    reg_state_lte_available_cell_id.rat   = earfcn_nas_param->rat;
    reg_state_lte_available_cell_id.lac   = earfcn_nas_param->lac;
    reg_state_lte_available_cell_id.cell_id = earfcn_nas_param->cell_id;
  }
  MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : returning the below entry to caller , Count = %d",earfcn_ptr->count);
  if(earfcn_ptr)
  {
    reg_state_dump_earfcn_list(earfcn_ptr);
  }
  return return_value;
}


boolean reg_state_search_earfcn_in_list(earfcn_nas_type earfcn, uint32 earfcn_curr)                
{
//  earfcn_nas_type *earfcn_temp = earfcn;
  boolean result = FALSE;
  uint8 i = 0;
  while(i < NAS_MAX_EARFCN_COUNT )
  {
    if(earfcn.earfcn_nas[i] == earfcn_curr)
    {
      result = TRUE;
      break;
    }
    else
    {
      i++;
    }
  }
  return result;
}

boolean reg_state_add_or_delete_earfcn_to_lte_available_list(sys_radio_access_tech_e_type rat,sys_plmn_id_s_type plmn, sys_lac_type lac,word cell_id,uint32 earfcn, boolean add)
{
  lte_cell_list_type*  list_ptr = NULL;
  cell_list_type*      curr_cell_list_ptr = NULL;
  boolean earfcn_added = FALSE;
  if(!reg_state_lte_available_list_ptr)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= =REG= LTE IRAT BPLMN : Lte available list is empty");
    return  earfcn_added;
  }
  if(earfcn == INVALID_EARFCN)               
  {
    return earfcn_added;
  }
  list_ptr = reg_state_lte_available_list_ptr;
  while(list_ptr != NULL)
  {
      if(sys_plmn_match(list_ptr->plmn,plmn) && list_ptr->rat == rat)                     
      {
        break;
      }
      else
      {
        list_ptr = list_ptr->next_ptr;
      }
  }
  //if PLMN entry and rat exists check if lac entry exists as well.                                        
  if(list_ptr != NULL)
  {
    curr_cell_list_ptr = list_ptr->cell_ptr;
    while(curr_cell_list_ptr != NULL)
    {
      if(curr_cell_list_ptr->lac == lac && 
         curr_cell_list_ptr->cell_id == cell_id)
      {
        break;
      }
      else
      { 
        curr_cell_list_ptr = curr_cell_list_ptr->next_ptr;
      }
    }
  }
  if(!list_ptr || !curr_cell_list_ptr)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : either Lte available list is empty or no entry is found for LAC to add the earfcn");
    return earfcn_added;
  }
  if(add == TRUE)
  {
  if(curr_cell_list_ptr->earfcn.count == 0)
  {
    uint8 i=1;
    curr_cell_list_ptr->earfcn.count = 1;
    curr_cell_list_ptr->earfcn.earfcn_nas[0]= earfcn;
    for(i=1;i<NAS_MAX_EARFCN_COUNT;i++)
    {
      curr_cell_list_ptr->earfcn.earfcn_nas[i] = INVALID_EARFCN;
    }
    earfcn_added = TRUE;
  }
  else
  {
    if(!reg_state_search_earfcn_in_list(curr_cell_list_ptr->earfcn, earfcn))
    {
      if(curr_cell_list_ptr->earfcn.count < NAS_MAX_EARFCN_COUNT)
      {
        curr_cell_list_ptr->earfcn.count= curr_cell_list_ptr->earfcn.count + 1;
        curr_cell_list_ptr->earfcn.earfcn_nas[curr_cell_list_ptr->earfcn.count-1] = earfcn;
      }
      else
      {
        uint8 i=0;
        for(i=1 ; i<NAS_MAX_EARFCN_COUNT;i++)
        { 
          curr_cell_list_ptr->earfcn.earfcn_nas[i-1] = curr_cell_list_ptr->earfcn.earfcn_nas[i];
        }
        curr_cell_list_ptr->earfcn.count= NAS_MAX_EARFCN_COUNT;
        curr_cell_list_ptr->earfcn.earfcn_nas[i-1] = earfcn;
      }
      earfcn_added = TRUE;
    }  
  }
  }
  else
  {
    if(curr_cell_list_ptr->earfcn.count == 0 || curr_cell_list_ptr->earfcn.count == 1)
    {
      uint8 i=0;
      curr_cell_list_ptr->earfcn.count = 0;
      for(i=0;i<NAS_MAX_EARFCN_COUNT;i++)
      {
        curr_cell_list_ptr->earfcn.earfcn_nas[i] = INVALID_EARFCN;
      }
      earfcn_added = FALSE;
    }
    else if(reg_state_search_earfcn_in_list(curr_cell_list_ptr->earfcn,earfcn))
    {
      uint8 k = 0,i=0;
      uint8 j = 0;
      for(k=0;k<NAS_MAX_EARFCN_COUNT;k++)
      {
        if(curr_cell_list_ptr->earfcn.earfcn_nas[k] == earfcn)
        {
          for(j=k ;j<NAS_MAX_EARFCN_COUNT-1;j++)
          {
           curr_cell_list_ptr->earfcn.earfcn_nas[j] = curr_cell_list_ptr->earfcn.earfcn_nas[j+1];
          }
          for(i=j;i<NAS_MAX_EARFCN_COUNT;i++)
          {
             curr_cell_list_ptr->earfcn.earfcn_nas[i] = INVALID_EARFCN;
          }
          curr_cell_list_ptr->earfcn.count= curr_cell_list_ptr->earfcn.count - 1;
          earfcn_added = FALSE;
          break;
        }
       }
    }
    MSG_HIGH_DS_1(REG_SUB, "=REG= LTE IRAT BPLMN : EARFCN DELETED = %d ",earfcn);
  }
  reg_state_dump_earfcn_list(&curr_cell_list_ptr->earfcn);
  return earfcn_added;
}


boolean reg_state_add_tds_or_gsm_or_w_cell_id_to_lte_list(boolean lte_avaialble, sys_radio_access_tech_e_type rat, uint32 earfcn)
{
  lte_cell_list_type* temp_list_ptr;
  boolean earfcn_added = FALSE;

  if(lte_avaialble)
  {
    temp_list_ptr = reg_state_lte_available_list_ptr;
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Available list will be updated if cell ID is valid");
  }
  else
  {
    temp_list_ptr = reg_state_lte_non_available_list_ptr;
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE not Available list will be updated if cell ID is valid");
  }
    temp_list_ptr = reg_state_add_cell_id_to_lte_list(temp_list_ptr, 
                                                      reg_state_service_state.plmn,
                                                      rat,                                     
                                                      reg_state_service_state.lac,
                                                      reg_state_cell_id,
                                                      MAX_LTE_SEARCH_LIST_DURATION - reg_timers_get_remaining_lte_duration_timer()
                                                      );
    if(lte_avaialble)
    {
      reg_state_lte_available_list_ptr = temp_list_ptr;
      earfcn_added = reg_state_add_or_delete_earfcn_to_lte_available_list(reg_state_service_state.active_rat,reg_state_service_state.plmn,reg_state_service_state.lac,reg_state_cell_id,earfcn, TRUE);                               
      reg_state_dump_the_list(reg_state_lte_available_list_ptr);
    }
    else
    {
      reg_state_lte_non_available_list_ptr = temp_list_ptr;
      if(!reg_timers_get_remaining_lte_duration_timer())
      {
        reg_state_start_backoff_lte_search_timer_count(reg_state_lte_non_available_list_ptr);
      }
    }
  return earfcn_added;
}

void reg_state_set_irat_hplmn_timer_search_ongoing(boolean value)
{
  reg_state_irat_hplmn_timer_search_ongoing = value;
}
void reg_state_set_irat_lte_timer_search_ongoing(boolean value)
{
  reg_state_irat_lte_timer_search_ongoing = value;
}

boolean reg_state_get_irat_hplmn_timer_search_ongoing (void)
{
  return reg_state_irat_hplmn_timer_search_ongoing;
}

boolean reg_state_get_irat_lte_timer_search_ongoing (void)
{
  return reg_state_irat_lte_timer_search_ongoing;
}

lte_cell_list_type* reg_state_delete_tds_or_gsm_or_w_cell_from_list(lte_cell_list_type* list_ptr ,sys_radio_access_tech_e_type rat)
{
  list_ptr = reg_delete_cell_entry_from_list(list_ptr,
                        reg_state_service_state.plmn,
                        rat,
                        reg_state_service_state.lac,
                        reg_state_cell_id);
return list_ptr;
}

void reg_state_set_lte_search_requested(boolean value)
{
  reg_state_lte_search_requested = value;
}
boolean reg_state_get_lte_search_requested(void)
{
  return reg_state_lte_search_requested;
}

boolean reg_state_cell_id_changed(sys_plmn_service_state_s_type service_state_local, word cell_id)
{
  boolean ret_value = FALSE;
  sys_plmn_id_s_type plmn;
  sys_radio_access_tech_e_type rat;   
  sys_lac_type   lac;
  word cell_id_local;

  plmn = reg_state_service_state.plmn;                    
  lac = reg_state_service_state.lac;
  rat = reg_state_service_state.active_rat;
  cell_id_local = reg_state_cell_id;
  if((service_state_local.active_rat == SYS_RAT_GSM_RADIO_ACCESS || service_state_local.active_rat == SYS_RAT_TDS_RADIO_ACCESS || service_state_local.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
  	  && (cell_id != 0xFFFF))   
  {
    if(!(sys_plmn_match(service_state_local.plmn,plmn ) &&
       service_state_local.lac == lac &&
       cell_id == cell_id_local))
    {
      ret_value = TRUE;
    }
  }

  if(ret_value == TRUE)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : Checking long timer status and also if Cell is present in LTE non Available List");  
    if(reg_is_lte_timer_running(REG_LTE_LONG_TIMER) && (! reg_is_cell_present_in_list(reg_state_lte_non_available_list_ptr, service_state_local.plmn,rat,service_state_local.lac, cell_id)))  
    {
      reg_timers_stop_lte_irat_search_period_timer();
      reg_state_force_bplmn_search_start = TRUE;
    }
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : Cell Changed =%d, Force BPLMN search =%d",ret_value,reg_state_force_bplmn_search_start);  
  return ret_value;
}

void reg_state_start_irat_timers_with_10sec(void)
{
  if(reg_state_get_irat_lte_timer_search_ongoing() == TRUE)
  {
    reg_timers_start_lte_irat_search_period_timer_10sec();
  }
  else if(reg_state_get_irat_hplmn_timer_search_ongoing() == TRUE)
  {
   /* Roaming is off if the IRAT HPLMN search is ongoing*/
   reg_timers_start_hplmn_irat_search_period_timer_10sec();
  }
  reg_state_set_irat_hplmn_timer_search_ongoing(FALSE);
  reg_state_set_irat_lte_timer_search_ongoing(FALSE);
}

void reg_state_dump_earfcn_list(earfcn_nas_type* earfcn_curr)       
{
  uint8 i=0;
  if(!earfcn_curr)
  {
    return;
  }
  while(i<NAS_MAX_EARFCN_COUNT && i < earfcn_curr->count)
  {
     MSG_HIGH_DS_2(REG_SUB, "=REG= earfcn[%d] = %d",i,earfcn_curr->earfcn_nas[i]);
     i++;
  }
}
void reg_state_dump_the_list(lte_cell_list_type* temp_list_ptr)
{
  boolean                  plmn_id_is_undefined;
  boolean                  mnc_includes_pcs_digit;
  uint32                   mcc;
  uint32                   mnc;
  cell_list_type*          temp_cell_id_ptr = NULL;
  lte_cell_list_type*      list_ptr = NULL;
  earfcn_nas_type         earfcn_curr;
  list_ptr = temp_list_ptr;
  
  if(list_ptr == NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : List is empty");
  }
  while (list_ptr != NULL)
  {
    sys_plmn_get_mcc_mnc
    (
     list_ptr->plmn,
     &plmn_id_is_undefined,
     &mnc_includes_pcs_digit,
     &mcc,
     &mnc
    );
    if(mnc_includes_pcs_digit)
    {
     MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu-%03lu", mcc, mnc);
    }
    else
    {
     MSG_HIGH_DS_2(REG_SUB, "=REG= %03lu- %02lu", mcc, mnc);
    }
    MSG_HIGH_DS_2(REG_SUB, "=REG= RAT = %d, length = %d",list_ptr->rat,list_ptr->length);
    temp_cell_id_ptr = list_ptr->cell_ptr;

    while(temp_cell_id_ptr != NULL)
    {
      earfcn_curr = temp_cell_id_ptr->earfcn;
      MSG_HIGH_DS_4(REG_SUB, "=REG=  LAC - CELL_ID - COUNTER - EARFCN Count   %d - %d - %d - %d",
                       temp_cell_id_ptr->lac, temp_cell_id_ptr->cell_id,temp_cell_id_ptr->count,earfcn_curr.count);
      reg_state_dump_earfcn_list(&earfcn_curr);
      temp_cell_id_ptr = temp_cell_id_ptr->next_ptr;
    }
    list_ptr = list_ptr->next_ptr;
  }
}

boolean reg_state_get_force_bplmn_search_start(void)
{
 return reg_state_force_bplmn_search_start;
}

lte_cell_list_type* reg_state_move_cell_up_queue(lte_cell_list_type* list_head_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,
  sys_lac_type lac,
  word cell_id 
)
{
  lte_cell_list_type* prev_lte_list_ptr = NULL;
  lte_cell_list_type* lte_local_list_ptr = NULL;
  cell_list_type*     prev_cell_list_ptr = NULL;
  cell_list_type*     cell_head_list_ptr = NULL;
  cell_list_type*     cell_list_local_ptr = NULL;
  
  lte_local_list_ptr = list_head_ptr;
  
  while(lte_local_list_ptr != NULL)
  {
    if(sys_plmn_match(lte_local_list_ptr->plmn ,plmn) && lte_local_list_ptr->rat == rat)
    {
      cell_head_list_ptr = lte_local_list_ptr->cell_ptr;
      cell_list_local_ptr = lte_local_list_ptr->cell_ptr;
      while(cell_list_local_ptr != NULL)
      {
        if(cell_list_local_ptr->lac == lac && cell_list_local_ptr->cell_id == cell_id)
        {
          if(prev_cell_list_ptr != NULL)
          {
            prev_cell_list_ptr->next_ptr = cell_list_local_ptr->next_ptr;
            cell_list_local_ptr->next_ptr = cell_head_list_ptr;
            lte_local_list_ptr->cell_ptr = cell_list_local_ptr;
          }
        }
        prev_cell_list_ptr = cell_list_local_ptr;
        cell_list_local_ptr = cell_list_local_ptr->next_ptr;
      }
      if(prev_lte_list_ptr != NULL)
      {
        prev_lte_list_ptr->next_ptr = lte_local_list_ptr->next_ptr;
        lte_local_list_ptr->next_ptr = list_head_ptr;
        list_head_ptr = lte_local_list_ptr;
      }
    }
    prev_lte_list_ptr = lte_local_list_ptr;
    lte_local_list_ptr = lte_local_list_ptr->next_ptr;
  }
  return list_head_ptr;
}

byte reg_state_start_backoff_lte_search_timer_count(lte_cell_list_type* lte_list_ptr)
{
  lte_cell_list_type * lte_temp_list = NULL;
  cell_list_type* cell_list_ptr = NULL;
  rex_timer_cnt_type min_timer_count = MAX_LTE_SEARCH_LIST_DURATION;

  if(lte_list_ptr== NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Not Available list is empty so not starting the timer");
    return 0;
  }
  lte_temp_list = lte_list_ptr;
  while (lte_temp_list != NULL)
  {
   cell_list_ptr = lte_temp_list->cell_ptr;
   while(cell_list_ptr != NULL)
   {
     if(cell_list_ptr->timer_count < min_timer_count)
     {
       min_timer_count = cell_list_ptr->timer_count;
     }
     cell_list_ptr = cell_list_ptr->next_ptr;
   }
   lte_temp_list = lte_temp_list->next_ptr;
  }
  lte_temp_list = lte_list_ptr;
  while (lte_temp_list != NULL)
  {
   cell_list_ptr = lte_temp_list->cell_ptr;
   while(cell_list_ptr != NULL)
   {
     cell_list_ptr->timer_count =  cell_list_ptr->timer_count - min_timer_count;
     cell_list_ptr = cell_list_ptr->next_ptr;
   }
   lte_temp_list = lte_temp_list->next_ptr;
  }
  reg_timers_restart_backoff_lte_search_period_timer(min_timer_count);
  return TRUE;
 

}

lte_cell_list_type* reg_state_process_back_off_lte_timer_expiry(lte_cell_list_type* lte_list_ptr )
{
  lte_cell_list_type * lte_temp_list=NULL;
  cell_list_type* cell_list_ptr = NULL;
  if(lte_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : LTE Not Available list is empty so not processing the back off timer expiry");
    return NULL;
  }

  lte_temp_list = lte_list_ptr;

  while(lte_temp_list != NULL)
  {
    cell_list_ptr = lte_temp_list->cell_ptr;
    while(cell_list_ptr != NULL)
    {
      if(cell_list_ptr->timer_count == 0)
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN : deleted lac %d and cell_id %d from the LTE Not Available list",cell_list_ptr->lac,cell_list_ptr->cell_id);
        lte_list_ptr = reg_delete_cell_entry_from_list(lte_list_ptr,lte_temp_list->plmn,lte_temp_list->rat,cell_list_ptr->lac,cell_list_ptr->cell_id);  //FR 60646
        reg_state_start_backoff_lte_search_timer_count(lte_list_ptr);
        return lte_list_ptr;
      }
      cell_list_ptr = cell_list_ptr->next_ptr;
    }
    lte_temp_list = lte_temp_list->next_ptr;
  }

  return lte_list_ptr;
}

boolean reg_state_get_force_irat_search_initiated (void)
{
  MSG_HIGH_DS_1(REG_SUB,"=REG= force irat still going on %d",reg_state_force_irat_search_initiated);
  return reg_state_force_irat_search_initiated;
}

void reg_state_set_force_irat_search_initiated(boolean value)
{
  reg_state_force_irat_search_initiated = value;
  MSG_HIGH_DS_1(REG_SUB,"=REG= force irat still going on %d",reg_state_force_irat_search_initiated);
}

#ifdef FEATURE_SGLTE_DUAL_SIM
boolean reg_state_get_trm_failure_sglte_main(void)
{
  return reg_state_trm_failure_sglte_main;
}
#endif 

#ifdef FEATURE_FEMTO_CSG
void reg_state_update_last_cm_service_req_type(reg_cm_service_req_e_type service_req_type)
{
  reg_last_cm_service_req_type = service_req_type;
}

sys_csg_info_s_type  reg_state_csg_info_get(void)
{
  return reg_state_service_state.csg_info;
}

reg_state_p_type reg_state_process_unexpected_mmr_csg_list_read_complete
(
 void
)
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Unexpected MMR_CSG_LIST_READ_COMPLETE_IND");
  reg_sim_set_csg_list_read_complete(TRUE);
  return reg_state_curr_p;
}

#endif 

reg_cm_service_req_e_type reg_state_get_last_cm_service_req_type(void)
{
  return reg_last_cm_service_req_type;
}

void reg_check_for_null_ptr(void* msg_p)
{
  if(msg_p == NULL)
  {
    MSG_FATAL_DS(REG_SUB,"=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
  }
}
#if defined FEATURE_DUAL_SIM
void reg_state_send_indication_on_other_stack(void)
{
  sys_srv_status_e_type local_service_status;
  sys_modem_as_id_e_type  reg_other_as_id = SYS_MODEM_AS_ID_1, buffered_reg_as_id = (sys_modem_as_id_e_type)reg_as_id;

  if(reg_state_service_state.service_status == SYS_SRV_STATUS_SRV && (MMCNM_IS_MODE_DUAL_STANDBY))
  {
    for(reg_other_as_id = SYS_MODEM_AS_ID_1; reg_other_as_id < MAX_NAS_STACKS; reg_other_as_id++)
    {
       if(reg_other_as_id != buffered_reg_as_id)
       {
         reg_as_id = reg_other_as_id;
         reg_sub_id = reg_sub_id_stack[reg_as_id];
         if(reg_state_service_state.service_status == SYS_SRV_STATUS_LIMITED && reg_state_curr_p == &reg_state_idle )
         {
           MSG_HIGH_DS_0(REG_SUB,"=REG= Sending NO service on limited service stack, for BPLMN OOS optimization");
           local_service_status = reg_state_service_state.service_status;
           reg_state_service_state.service_status = SYS_SRV_STATUS_NO_SRV;
           reg_send_cm_service_ind
           (
             reg_state_service_state
           );
           reg_state_service_state.service_status = local_service_status;
         }
       }
    }
    reg_as_id  =  buffered_reg_as_id;
    reg_sub_id = reg_sub_id_stack[reg_as_id];
  }
}
#endif



/*==============================================================================

FUNCTION NAME

  reg_state_get_power_up_system_selection

==============================================================================*/
boolean reg_state_get_power_up_system_selection(void)
{
  return reg_state_power_up_system_selection;
}


#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_state_sim_status_get

==============================================================================*/

sys_sim_state_e_type reg_state_sim_status_get
(
  void
)
{
  return reg_state_service_state.sim_state;
}
#endif
