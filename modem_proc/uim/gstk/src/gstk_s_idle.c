/*===========================================================================


                            G S T K     I D L E


GENERAL DESCRIPTION
  This source file contains function to handle event for GSTK_IDLE_S state.


FUNCTIONS
  gstk_state_idle
    This function handles events/commands/signals for GSTK_IDLE_S


INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2005, 2008-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All  Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_s_idle.c#1 $ $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
05/10/16   gs      F3 Logging improvements
11/19/15   vr      Update client userdata before sending the sw1_sw2
10/28/15   vr      Don't send SW1 report for the clients which didn't send TR
04/27/15   vr      Non UI and non NW proactive command parallel processing
02/06/15   dy      Fix GSTK not blocking Access Tech if MM idle/Mode change
                   received before setup event list
02/06/15   bd      Do not update intermediate 3GPP2 information
01/27/15   gm      Retry call control after recovery
01/08/15   vr      Retry envelope command after fetching the proactive command
11/21/14   gm      Reset rrc cell change indication flag
11/12/14   vr      Cache MM_IDLE in gstk_loci_table even if it mismatchs with RRC info
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
10/06/14   gm      Added support to send failure indication for OTASP
09/25/14   bd      Fixed compiler errors for 1x compiled out build
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/10/14   gm      Added support for SMS_PP data download retry
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/21/14   bd      Do not disable VPE for no service
08/14/14   gm      Free memory allocated for sending command to UIM
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/29/14   bd      Do not disable VPE for no service
07/23/14   vr      Cleanup GSTK_CLIENT_INIT_EXT function
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
06/30/14   gm      Not sending additional info in CSG cell sel event
06/24/14   bd      Fix for IRAT 3GPP2 - 3GPP issue
06/25/14   vr      Set the user data with unique ref id in Cached TR handling
06/23/14   sw      Implemented retry for call-control envelopes
06/17/14   bd      Fill activation_code when originating non-standard OTASP call
05/28/14   gm      Support of CSG Rel-11 feature
05/28/14   dy      Add GSTK_SUBS_FEATURE_MODE_SRLTE
05/23/14   gm      Verifying TR response report
05/12/14   shr     Do not attempt envelope retries when proactive command
                   is in progress
05/12/14   shr     When VPE is enabled, do not check for pending location
                   status envlope before queueing a new one
05/06/14   gs      GSTK Featurization enhancements and clean up
04/29/14   dy      Fix GSTK sending SW rpt to clients without setting slot_id
04/22/14   hn      Clear retry data when received GSTK_MMGSDI_CARD_ERROR_EVT
04/03/14   dy      Replace gstk_malloc() with macro
02/27/14   sw      Extended retry mechanism
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/18/14   hn      Start 1x timer only in SVLTE sub feature mode and LTE is
                   reported on CM stack 1 (hybrid stack)
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/03/14   shr     ESTK BIP clean-up and enhancements
02/20/14   dy      Verify TR response for raw TRs
02/20/14   gm      Clear globals after REFRESH
02/03/14   gs      Fixed infinite IMSI refresh issue
02/04/14   gm      Update gstk_next_curr_rat when MODE_CHANGE_IND received
01/18/14   bd      Fixed MODE CH handler for no service to check nas_rat
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/03/14   bd      Fixed the for-loop that checks all CM stacks for 3GPP service
12/20/13   bd      Fixed memory alignment issue
12/19/13   bd      Do not clear location information when freeing MM IDLE cache
12/16/13   bd      Added support for new CM SS event handling
12/10/13   vr      Clear MM IDLE state flag when MM IDLE cache is freed
12/06/13   shr     Added support for preferred activation over 1x
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
11/29/13   vr      Enable feature GSTK_CFG_FEATURE_ENH_RAT_AND_LOC_EVT in
                   recovery
11/15/13   vr      Fix for UIMDRV Memory De-Couple changes for delay TR
11/13/13   shr     Added support for BIP over WLAN
10/21/13   jd      Added slot parameter to gstkutil_update_plmn_lac_cellId_table()
10/17/13   jd      Clear network status related variables upon refresh reset
10/14/13   vr      Proactive session not closed when the card is removed
                   between fetch and TR
10/04/13   sw      Reduced F3 messages
09/26/13   jd      Free MM IDLE pointer when there is no 3GPP service
09/23/13   jd      Clear all RAT info in MM cache when in No Service
09/06/13   gm      UIMDRV Memory De-Couple changes
09/13/13   gm      Support for Recovery and hot-swap
09/04/13   jd      Start the No Service timer on receiving MM IDLE No Service
                   indication only when in SGLTE mode and both MM are active
08/06/13   vr      Centralized Call Control Changes
07/24/13   sw      Executing location env rsp rcvd confirmation in gstk context
07/23/13   sw      Support for DS callback to obtain RAT information.
07/22/13   hn      Support MM Indications
07/15/13   hn      Assign pointers to NULL after gstk_free()
07/09/13   bd      Invalidate MM IDLE and MODE CHANGE IND cache for required
                   refresh types
07/03/13   kb      Remove write of tp_client when feature is disabled
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
07/01/13   hn      Support SGLTE dual MMs
06/15/13   bd      Invalidate MM IDLE upon No service reported by CM SS event
                   in all it's 3GPP stacks
06/14/13   hh      Do not associate gstk_mm_param_received with setup_evt_list_cnt
06/04/13   gm      Sending location status to UICC when cell change indication
                   MM_IDLE event received
05/23/13   hn      When processing MM IDLE and rat is LTE, get latest Cell ID
                   from RRC table
05/16/13   xz      Fix the issue of MO SMS ctrl failure
05/13/13   vr      gstk_proactive_cmd_in_progress set to flase, for null
                   procactive command bytes.
05/08/13   xz      Set gstk_mm_param_received to true only after SETUP EVT
                   LIST recieved
05/01/13   vr      Fix command processing logic in GSTK
04/26/13   gm      Not allowing fetch before TP download
04/15/13   xz      Fix the issue of unexpected loc status during refresh
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/06/13   av      Move UIM tasks to group4 of RCINIT
04/06/13   bd      Clear CDMA location info timer when MODE CHANGE IND doesn't
                   show up with any of the 3GPP2 radio access technologies
03/29/13   bd      Clean up logic to send VPE from MM IDLE
02/25/13   xz      Process attach ctrl req when envelope is in retry
02/22/13   kb      Fixed to update the local information after refresh.
02/11/13   bd      Support for (e)HRPD access tech
01/22/13   hn      Support 1xRTT CSIM programming retries
11/13/12   bd      Support for Closed Subscriber Group feature
10/18/12   vr      reset the GSTK current instace ptr's apps_enabled, when there
                   is a GSTK_MMGSDI_CARD_ERROR_EVT or a REFRESH_RESET
10/15/12   vr      Fixing compilation error of cdma_location_info_cache
10/04/12   sg      Clean up the TR optimization in GSTK_S_NO_SIM
09/18/12   hk      Support to read IMSI after getting VPE resp
09/04/12   hn      Set nas_rat in MM Idle before switching to Mode Change
08/16/12   sg      Update globals but dont send LS for SVDO scenarios
08/07/12   sg      Handle SVDO scenarios of multiple envelopes
06/06/12   sg      Fix when MM_IDLE does not report No Srv until deep sleep
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
03/09/12   bd      Enhancements for location information in CC envelope
03/28/12   av      Replaced feature flags with NVITEM
02/28/12   xz      Fix the issue of sending VPE in RRC CB
02/06/12   bd      Fixed issue of unable to fetch proactive command after
                   REFRESH  reset
01/09/12   bd      Fixed crash issue when slot in cmd ref table was already
                   freed
12/23/11   shr     Removed ALWAYS OFF feature FEATURE_GSTK_DUAL_SLOTS
12/20/11   nb      GSDI cleanup
12/13/11   dd      Do not overwrite tp_clients if less clients are found
12/05/11   xz      Fix the issue of loc info in CC envelope cmd
11/29/11   dd      Add support for TDSCDMA RAT
11/21/11   sg      Add RRC rat for tracking cell change from LTE vs WCDMA
10/20/11   sg      Changes for GSTK support simultaneous mult-RAT
10/04/11   xz      Fix the issue of memcmp()
10/03/11   sg      Update loc status global from mm idle when not sending ls
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
09/14/11   xz      Fix the issue of duplicated AcT envelope for 3GPP
08/24/11   xz      Fix compiler warnings and LINT/KW errors
07/26/11   js      Change to support AcT event processing in TAL
06/22/11   sg      Fix simultaneous OTASP and BIP on a multi-modem environment
06/20/11   js      Fix issue with uim recovery on fusion
05/14/11   bd      Thread safe changes - timer management
05/11/11   nb      Support for Async TP
04/13/11   xz      Fix issue of unexpected NO SERVICE envelope cmd
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/23/11   sg      Add handling for retry of loc info envelope
03/23/11   bd      Fixed memory leak due to hotswap
03/15/11   sg      Move free of memory from cdma timer cb to gstk context
03/11/11   xz      Reset TP clients content
02/22/11   js      Renaming fusion features
02/18/11   nb      Fix for compilation Error
02/14/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/02/11   dd      Process LTE AcT correctly
02/02/11   dd      Combine Access Tech envelopes in TAL
02/01/11   nb      Fix for compilation error
01/27/11   nb      Fix for compilation warning
01/26/11   xz      Add support of HRPD access tech
01/26/11   xz      Reset OTASP call ID if call orig cmd fails
01/13/11   xz      Fix the issue of CM triggering (E)UTRAN NO SERVICE event
01/12/11   sg      Dual slot scenarios cleanup on GSTK timer expiry
01/11/11   sg      Update location status when cdma loc status is obtained
01/05/11   xz      Ignore LTE event sent by MM
01/04/11   sg      Add code for estk client cleanup on timer expiry
12/17/10   xz      Decouple feature bitmask
12/10/10   sg      CDMA Location status timer
11/29/10   tkl     Removed RRC & MMODE private header file
11/18/10   bd      Fixed compilation error
11/01/10   dd      Set gstk_mm_param_received to false after refresh
10/20/10   xz      Fix the issue of sending LTE access tech event
10/20/10   xz      Fix the issue of not send TR in eHRPD only mode
10/11/10   sg      Port IMSI_M enhancement and multiple OTASP calls
10/05/10   bd      Fixed issue of TP DL
08/19/10   xz      Block OPEN CH until MMGSDI_SUBSCRIPTION_READY_EVT
07/26/10   xz      Add support of MM and EMM idle events simultaneously
07/13/10   bd      Added some important logging code
06/21/10   xz      Use correct feature mask
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/24/10   sg      Send Location information envelope only when it has changed
05/11/10   xz      Fix the issue of mem overwrite when copy cell ID
04/19/10   sg      Send Loc Info Envelope only on PLMN change
04/07/10   yt      Fixed compilation warnings
03/08/10   sg      Location information envelope command
02/22/10   xz      1) Fix a typo of FEATURE_GSTK_IMSI_M_PROVISION_ENH
                   2) Fix compilation error when IMSI_M feature is enabled
02/15/10   nb      Moved Dual Slot Changes
02/09/10   xz      IMSI_M provision enhancement
01/12/10   sg      Added command for getting current access technology
12/16/09   bd      Update gstk_next_curr_location_status with changes received from CM
10/22/09   bd      Set a signal when event GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL
                   is received from GSDI
09/26/09   kp      ZI memory reduction changes
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
02/03/09   gg      Fixed lint error
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
10/22/08   sk      Handle client id reg and proactive command reg
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation issues
09/02/08   sk      Fixed error message
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/21/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/08/08   xz      Added support for proactive command/terminal response
                   timer expiration
05/27/08   sk      Fixed expression evaluation
05/06/08   sk      Added support for event download envelope retry
05/06/08   sk      Fixed access technology updates to the card after a
                   No service.
                   Fixed duplicate access technology updates to the card.
04/04/08   sk      Fix access technology updates to the card
12/19/07   sk      Query for cell id during MM Idle event only for 3G RAT
08/25/06   sk      Featurized uim recovery
06/16/06   jar     Changes to support offtarget VSIM Support
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mod
05/17/06   sk      Added support to send access technology envelope
05/03/06   sk      Removed cell id checks.
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/30/05   sk      Added check for Access technology event bit
                   set in set up event list in GSTK_MODE_CHANGE
10/26/05   tml     compilation fix
10/10/05   tml     Added curr Rat and next curr Rat caching
10/08/05   tml     Used new MM info type for handling MM IDLE event
10/07/05   tml     Added plmn and lac in rrc cell change ind type
09/30/05   sk      Added support for two tables, one for RRC location information
                   and one for MM location information
09/26/05   tml     Updated RRC cell id received variable to ensure location
                   status for normal service is only sent when both MM and RRC
                   info are received
09/22/05   tml     Fixed cell ID change envelope download notification issue
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
08/25/05   sk      Fix for envelope location status for 3G
                   in gstk_state_idle() for case GSTK_MM_IDLE_CMD
07/29/05   sk/tml  Fixed compilation errors.
                   fixed compilation problem
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
05/11/05   sst     Lint fixes
06/18/04   tml     Fixed dual slot not signaling GSDI for OK for TP issue
05/12/04   tml     Clear timer management timer when enter NO SIM state
04/02/04   tml     linted
03/17/04   tml     added handling for Terminal profile download during
                   refresh sim init
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
03/01/04   tml     Fixes for location info env command
02/24/04   tml     Fixed compilation issue with CDMA only target
01/25/04   tml     Dual Slot refresh support
09/04/03   tml     Dual slot support
07/07/03   tml     Made messages more meaningful
06/23/03   tml     removed compiler error for 1x/gsm
05/26/03   tml     linted
03/21/03   tml     Update featurization for WCDMA single build
03/20/03   tml     Properly featurize local info for FEATURE_WCDMA and
                   FEATURE_GSM
03/07/03   tml     Added (FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/28/03   jar     Do not go to GSTK_GSDI_SIG_WAIT when GSDI_REFRESH_SIM_INIT
02/19/03   jar     Modifed MSG_HIGH in gstk_state_idle() from GSDI to GSTK.
                   Added Handling of GSDI_REFRESH_SIM_RESET.
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "gstk_state.h"
#include "gstk_proactive_cmd.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk.h"
#include "mmgsdilib_p.h"

#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

#include "string.h"
#include "err.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#ifdef FEATURE_GSM
#include "rr_usat_if.h"
#endif /*FEATURE_GSM */
#if ((defined FEATURE_GSM) || (defined FEATURE_WCDMA))
#include "mm.h"
#endif /* ((defined FEATURE_GSM) || (defined FEATURE_WCDMA)) */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Queue to hold commands for gstk */
extern q_type gstk_task_cmd_q;

static boolean gstk_check_cm_stacks_for_3gpp(const gstk_mode_change_ind_type*);
static int gstk_mode_ch_compare_func(void* item_ptr, void* compare_val);
static boolean gstk_check_access_tech_for_3gpp2(const gstk_access_technology_type);

static boolean gstk_check_access_tech_for_3gpp(const gstk_access_technology_type);

static boolean gstk_check_send_tp_when_mode_ch(
  gstk_access_technology_type    access_tech,
  gstk_slot_id_enum_type          slot);

gstk_status_enum_type gstk_find_normal_service_rat_when_mode_ch(
  gstk_access_technology_type*     access_tech,
  const gstk_mode_change_ind_type* mode_change_ind);

/*===========================================================================
FUNCTION gstk_state_idle

DESCRIPTION

  This function handles events/commands/signal for GSTK_IDLE_S
  GSTK_IDLE_S may receive any of the following events:
  - sim errors from GSDI -> transition to GSTK_NO_SIM_S
  - terminal response from client -> stay in GSTK_IDLE_S
  - proactive command from uim -> stay in GSTK_IDLE_S
  - envelope command from client -> stay in GSTK_IDLE_S
  - envelope response from uim -> stay in GSTK_IDLE_S
  - timer expiration from GSTK -> stay in GSTK_IDLE_S
  - end proactive session from uim -> stay in GSTK_IDLE_S
  - GSDI_REFRESH_SIM_RESET from GSDI -> transition to GSTK_NO_SIM_S
  - task stop sig from TMC -> transition to GSTK_POWER_DOWN_S

PARAMETERS
  cmd_ptr: [Input] Pointer to the command to be processed in this state

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_state_idle (
  gstk_cmd_type* cmd_ptr)
{
  gstk_status_enum_type           gstk_status       = GSTK_SUCCESS;
  gstk_cell_id_type               temp_cell_id      = {0};
  gstk_location_status_enum_type  location_status   = GSTK_NO_SERVICE;
  boolean                         ls_env            = TRUE;
  gstk_access_technology_type     rat               = GSTK_ACCESS_NONE;
  uint16                          index             = 0;
  int                             i                 = 0;
  uint32                          timer_idx         = 0;
  gstk_slot_id_enum_type          slot              = GSTK_SLOT_1;
  gstk_access_technology_type     access_tech       = GSTK_ACCESS_NONE;
  uint16                          curr_3gpp2_rat    = 0;
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  boolean                         is_1x_available   = FALSE;
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
#ifdef FEATURE_GSTK_FUSION_REMOTE
  gstk_tal_uim_cmd_type           otasp_status_cmd;
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
  sys_modem_as_id_e_type      as_id = SYS_MODEM_AS_ID_1;
  gstk_local_info_rsp_type       *local_info_ptr    = NULL;
  gstk_additional_info_type       additional_info   = {0};
  uint8                           cat_version       = GSTK_CFG_CAT_VER_MAX;
  gstk_nv_items_data_type         nv_data;
  gstk_cdma_loci_ext_cache_type   cdma_ext_temp;
  boolean                         rat_is_different  = FALSE;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);
  cat_version = nv_data.cat_version;

  memset(temp_cell_id.cell_id, 0xFF, (GSTK_MAX_CELL_ID_LEN * sizeof(uint8)));
#ifdef FEATURE_GSTK_FUSION_REMOTE
  memset(&otasp_status_cmd, 0, sizeof(otasp_status_cmd));
#endif /*FEATURE_GSTK_FUSION_REMOTE*/

  switch(cmd_ptr->general_cmd.message_header.command_group )
  {
    case GSTK_TR_REPORT:
      UIM_MSG_HIGH_0("Received GSTK_TR_REPORT cmd");

      if (gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        UIM_MSG_HIGH_0("Ignore TR rsp report as GSTK in recovery sub state");
        break;
      }

      gstk_curr_inst_ptr->tr_unique_ref_id = 0;

      /* cmd_ptr->general_cmd.data[0] is SW1, [1] is SW2
         [2] is proactive_cmd_pending*/
      if (cmd_ptr->general_cmd.length != 3)
      {
        UIM_MSG_HIGH_1("SW1 and SW2 corrupted, stop sending to client 0x%x",
                       cmd_ptr->general_cmd.message_header.user_data);
        break;
      }

      if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2, GSTK_SLOT_ID_MAX) == TRUE)
      {
        gstk_cmd_from_card_type sw1_sw2_rpt;
        gstk_client_id_type     client_id  = GSTK_CMD_REF_TABLE_INDEX(cmd_ptr->general_cmd.message_header.user_data);

        sw1_sw2_rpt.hdr_cmd.command_id = GSTK_TERM_RSP_SW1_SW2_RPT;
        sw1_sw2_rpt.hdr_cmd.sim_slot_id = cmd_ptr->general_cmd.message_header.sim_slot_id;
        sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw1 = (uint8)cmd_ptr->general_cmd.data[0];
        sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw2 = (uint8)cmd_ptr->general_cmd.data[1];

        /* Only for responses to raw TRs */
        if (gstk_util_is_valid_client_id(client_id))
        {
          UIM_MSG_HIGH_1("Send SW1 and SW2 to client 0x%x.", client_id);
          gstk_util_call_client_evt_cb(client_id,
                                       &sw1_sw2_rpt);
        }
        else if(client_id == 0)
        {
          /* Find raw client and send TR report */
          gstk_util_send_sw_to_client(&sw1_sw2_rpt);
        }
        else
        {
          UIM_MSG_ERR_1("Received invalid client id 0x%x", client_id);
        }
      }

      if (((uint8)cmd_ptr->general_cmd.data[0] == SW1_NORMAL_END) &&
          ((uint8)cmd_ptr->general_cmd.data[1] == SW2_NORMAL_END))
      {
         /* check if any pending envelope commands to be sent due to last failure */
         if (gstk_util_has_pending_timer_envelope(
               cmd_ptr->general_cmd.message_header.sim_slot_id))
         {
           UIM_MSG_HIGH_0("GSTK_RETRY_ENVELOPE_CMD in GSTK_TR_REPORT");
           index = gstk_util_find_first_timer_expire_command(
                     cmd_ptr->general_cmd.message_header.sim_slot_id);
           if (index == GSTK_MAX_TIMER_SUPPORTED)
           {
             UIM_MSG_HIGH_0("Invalide Timer Table index");
           }
           else
           {
             gstk_build_timer_expire_envelope(
               cmd_ptr->general_cmd.message_header.sim_slot_id,
               uint16touint8(index));
           }
         }
      }
      /* no more proactive command pending -> build exit proactive command session
      request to clients */
     /* cmd_ptr->general_cmd_data[2] is proactive_cmd_pending */
      if (cmd_ptr->general_cmd.data[2] == 0) {

        /* build the terminal profile response */
        cmd_ptr->proactive_cmd.message_header.command_group = GSTK_PROACTIVE_CMD;
        cmd_ptr->proactive_cmd.message_header.command_id = GSTK_END_PROACTIVE_CMD_REQ; /* doesn't matter */

        /* initialize to 0 */
        memset(cmd_ptr->proactive_cmd.data, 0x00, GSTK_MAX_DATA_SIZE);
        cmd_ptr->proactive_cmd.data[0] = (uint8)GSTK_PROACTIVE_SIM_COMMAND_TAG;
        cmd_ptr->proactive_cmd.data[1] = 0x09;
        cmd_ptr->proactive_cmd.data[2] = (uint8)GSTK_COMMAND_DETAILS_TAG;
        cmd_ptr->proactive_cmd.data[3] = (uint8) GSTK_COMMAND_DETAILS_LEN;
        cmd_ptr->proactive_cmd.data[4] = 0;
        cmd_ptr->proactive_cmd.data[5] = (uint8)GSTK_CMD_STK_END_OF_PROACTIVE_SES;
        cmd_ptr->proactive_cmd.data[GSTK_GENERATED_END_SES_INDEX] =  (uint8)GSTK_GENERATED_END_SES;
        cmd_ptr->proactive_cmd.data[7] = (uint8)GSTK_DEVICE_IDENTITY_TAG;
        cmd_ptr->proactive_cmd.data[8] = (uint8)GSTK_DEVICE_IDENTITY_LEN;
        cmd_ptr->proactive_cmd.data[9] = (uint8)GSTK_UICC_SIM_DEVICE;
        cmd_ptr->proactive_cmd.data[10]= (uint8)GSTK_ME_DEVICE;

        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          UIM_MSG_HIGH_0("Ignoring proactive command as GSTK is in recovery state");
          gstk_status = GSTK_ERROR;
          break;
        }
        UIM_MSG_HIGH_0("Received Proactive cmd in GSTK_TR_REPORT");
        gstk_status = gstk_process_proactive_command(&cmd_ptr->proactive_cmd);
        if (gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("Proactive Cmd ERR, 0x%x", gstk_status);
          GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
              gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress, FALSE);
        }
      }
      break;
    case GSTK_RECOVERY_CMD:
      UIM_MSG_HIGH_2("Received Recovery cmd, %x, gstk_sub_state %x",
                     cmd_ptr->general_cmd.message_header.command_id,
                     gstk_curr_inst_ptr->gstk_sub_state);
      slot = cmd_ptr->general_cmd.message_header.sim_slot_id;
      switch(cmd_ptr->general_cmd.message_header.command_id)
      {
        case GSTK_RECOVERY_START_CMD:
          /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
          gstk_util_find_and_process_pending_responses();

          gstk_curr_inst_ptr->gstk_sub_state = GSTK_RECOVERY_SUB_S;
          /* Cleanup pending envelope commands by sending error envelope response */
          for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
          {
            if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
               !gstk_shared_data.envelope_cmd_ref_table[i].is_free )
            {
              if(gstk_shared_data.envelope_cmd_ref_table[i].expected_cmd_rsp_type == GSTK_CC_IND_RSP)
              {
                gstk_slot_send_envelope_cc_command_ext(slot,
                   gstk_shared_data.envelope_cmd_ref_table[i].user_data,
                   &(gstk_shared_data.envelope_cmd_ref_table[i].gstk_cc_cache_info),
                   gstk_shared_data.envelope_cmd_ref_table[i].env_cb.client_env_cb);
                gstk_util_env_cmd_ref_table_free_slot(i);                   
                continue;                
              }
              if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
              {
                gstk_util_env_cmd_ref_table_free_slot(i);
              }
            }
          }

          /* Clear retry data since we already cleared envelope_cmd_ref_table */
          gstk_util_clear_retry_envelope_data(slot);

          /* Clean up Pending proactive command */
          for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
          {
            if ((!command_details_ref_table_ptr[i].free_slot) &&
                (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
            {
              gstk_util_cmd_details_ref_table_free_slot(i);
              /* Only one proactive command can be processed at a time */
              break;
            }
          }

          if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
          {
            if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2, GSTK_SLOT_ID_MAX) == TRUE)
            {
              if(gstk_curr_inst_ptr->tr_unique_ref_id != 0)
              {
                /* TR already sent to uimdrv, check whether it is sent by GSTK or client */
                gstk_client_id_type client_id  = GSTK_CMD_REF_TABLE_INDEX(gstk_curr_inst_ptr->tr_unique_ref_id);
                if(client_id != gstk_curr_inst_ptr->client_id)
                {
                   /* TR is not generated by GSTK */
                   gstk_util_send_sw_to_client(NULL);
                }
              }
              else
              {
                /* Check already queued RAW TR in GSTK cmd queue */
                gstk_task_cmd_type *cmd_p = gstk_util_find_terminal_rsp_in_queue();
                if(cmd_p != NULL)
                {
                  if(cmd_p->cmd.general_cmd.message_header.command_id == GSTK_RAW_TERMIMAL_RESPONSE)                  
                  {
                    gstk_client_raw_term_resp_type   *raw_term_resp_ptr = &cmd_p->cmd.client_raw_term_resp;
                    gstk_shared_data.gstk_client_table[raw_term_resp_ptr->client_id - 1].user_data =
                                                              raw_term_resp_ptr->message_header.user_data;
                    /* TR is not generated by GSTK */
                    gstk_util_send_sw_to_client(NULL);
                  }
                  gstk_task_free_cmd_buf(cmd_p);
                }
              }
            }
            /* find and send end proactive seesion for pending proactive command */
            gstk_util_find_and_build_proactive_end_session(GSTK_TAG_RECOVERY);
          }

          /* Clean up the GSTK Global data */
          gstk_util_clean_up_gstk_global_data(slot, GSTK_TAG_RECOVERY);
          gstk_curr_inst_ptr->recovery_enable_ls_act = TRUE;
          break;
        case GSTK_RECOVERY_END_CMD:
          gstk_curr_inst_ptr->gstk_sub_state   = GSTK_INVALID_SUB_S;
          break;
        default:
          break;
      }
      break;

    case GSTK_PROACTIVE_CMD:
      if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        UIM_MSG_HIGH_0("Ignoring proactive command as GSTK is in recovery state");
        gstk_status = GSTK_ERROR;
        break;
      }
      UIM_MSG_HIGH_0("Received Proactive cmd");

      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_IN_QUEUE)
      {
        if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
        {
          /* build envelope response back to the client */
          gstk_status = gstk_build_error_envelope_report(
            gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id,
            int32touint32((int32)gstk_shared_data.envelope_cmd_ref_table[gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
            GSTK_ENVELOPE_CMD_FAIL);
          gstk_util_clear_retry_envelope_data(
            gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        }
      }

      gstk_status = gstk_process_proactive_command(&cmd_ptr->proactive_cmd);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("Proactive Cmd ERR, 0x%x", gstk_status);
      }
      break;

    case GSTK_CLIENT_REG_REQ_CMD:
      UIM_MSG_HIGH_0("Received GSTK_CLIENT_REG_REQ_CMD");
      (void)gstk_process_client_reg_req(cmd_ptr);
      break;

    case GSTK_TERMINAL_RSP:
      UIM_MSG_HIGH_0("Received Term rsp");
      gstk_status = gstk_process_terminal_response(cmd_ptr);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("Terminal Rsp ERR, %x", gstk_status);
      }
      break;

    case GSTK_ENVELOPE_CMD:
      UIM_MSG_HIGH_0("Received Envelope cmd");
      if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        UIM_MSG_HIGH_0("Received Envelope cmd in recovery state");
        gstk_status = gstk_process_non_idle_state_envelope_cmd(cmd_ptr);
      }
      else
      {
        gstk_status = gstk_process_envelope_cmd(cmd_ptr);
      }
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("Envelope Cmd ERR, %x", gstk_status);
      }
      break;

    case GSTK_ENVELOPE_RSP:
      UIM_MSG_HIGH_0("Received Envelope rsp");
      gstk_status = gstk_process_envelope_rsp(cmd_ptr);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_1("Envelope rsp ERR, %x", gstk_status);
      }
      if(!(gstk_util_is_envelope_rsp_evt_dl(cmd_ptr->envelope_response.message_header.command_id)) &&
         !(gstk_util_is_envelope_rsp_loc_info(cmd_ptr->envelope_response.message_header.command_id)) &&
         (cmd_ptr->envelope_response.rsp_status != GSTK_ENVELOPE_CMD_CARD_BUSY) &&
         (gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_ENV_IN_QUEUE))
      {
        if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
        {
          /* build envelope response back to the client */
          gstk_status = gstk_build_error_envelope_report(
            gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id,
            int32touint32((int32)gstk_shared_data.envelope_cmd_ref_table[gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
            GSTK_ENVELOPE_CMD_FAIL);
        }
      }
      break;

    case GSTK_MMGSDI_CMD:
      gstk_process_mmgsdi_evt(cmd_ptr);
      break;

    case GSTK_MM_IDLE_CMD:
      UIM_MSG_HIGH_3("Received MM Idle, command_id: 0x%x rat: 0x%x stack_id: 0x%x",
                     cmd_ptr->mm_idle_state.message_header.command_id,
                     cmd_ptr->mm_idle_state.rat,
                     cmd_ptr->mm_idle_state.stack_id);

      /* Process MM Indication */
      if(cmd_ptr->mm_idle_state.message_header.command_id == GSTK_MM_INDICATION &&
         !gstk_util_process_mm_indication(&cmd_ptr->mm_idle_state))
      {
        /* if gstk_util_process_mm_indication() returns FALSE, there is no immediate
           MM IDLE to be processed */
        break;
      }

      /* Clear the gstk_mm_idle_no_service_timer if it's still running and the MM IDLE
         is not from a requeue */
      if((cmd_ptr->mm_idle_state.message_header.command_id != GSTK_MM_IDLE_FROM_REQUEUE) &&
         (rex_get_timer(&(gstk_curr_inst_ptr->gstk_mm_idle_no_service_timer)) > 0))
      {
        UIM_MSG_HIGH_0("Clear gstk_mm_idle_no_service_timer");
        (void)rex_clr_timer(&(gstk_curr_inst_ptr->gstk_mm_idle_no_service_timer));
        gstk_free(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer);
        gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer = NULL;
      }

       /* Check whether we have MM IDLE No Service pending (that was delayed)
          If we do, we need to restore that MM IDLE evt */
      if(cmd_ptr->mm_idle_state.message_header.command_id == GSTK_MM_IDLE_FROM_TIMER)
      {
        if(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer == NULL)
        {
          UIM_MSG_HIGH_0("MM IDLE No Service timer expired - cmd ptr is NULL");
          break;
        }
        (void)gstk_memcpy(cmd_ptr,
                       gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));

        cmd_ptr->mm_idle_state.message_header.command_id =
          GSTK_MM_IDLE_FROM_TIMER;
        gstk_free(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer);
        gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer = NULL;
      }

      /* MM Arbitration logic, if there are MM IDLE from 2 stacks */
      if (gstk_util_requeue_prio_mm(&cmd_ptr->mm_idle_state))
      {
        UIM_MSG_HIGH_0("MM IDLE on another stack has better service, requeue MM IDLE on that stack");
        break;
      }

      gstk_status = gstk_util_map_mm_to_gstk_location_status(&(cmd_ptr->mm_idle_state),
                                                           &location_status);
      UIM_MSG_HIGH_1("Location Status = 0x%x", location_status);

      /* If both MM instances are active and MM IDLE No Service is not
         triggered by timer, delay it */
      if(((gstk_shared_data.mm_ind_cache == GSTK_IO_MM_SGLTE_IND) ||
          (gstk_shared_data.mm_ind_cache == GSTK_IO_MM_SGLTE_SPLIT_IND)) &&
         (location_status == GSTK_NO_SERVICE) &&
         (cmd_ptr->mm_idle_state.message_header.command_id != GSTK_MM_IDLE_FROM_TIMER))
      {
        if(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer == NULL)
        {
          gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer =
            (gstk_cmd_type *)GSTK_CALLOC(sizeof(gstk_cmd_type));
          if(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer == NULL)
          {
            gstk_status = GSTK_MEMORY_ERROR;
            break;
          }
        }
        (void)gstk_memcpy(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer,
                       cmd_ptr,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
        (void)rex_set_timer(&(gstk_curr_inst_ptr->gstk_mm_idle_no_service_timer),
                            GSTK_MM_IDLE_NO_SERVICE_WAIT_TIME);

        gstk_status = GSTK_SUCCESS;
        UIM_MSG_HIGH_1("Delay MM IDLE No Service %dms",
                       GSTK_MM_IDLE_NO_SERVICE_WAIT_TIME);
        break;
      }

      if (!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat, &rat))
      {
        UIM_MSG_ERR_1("Unknown Sys Rat 0x%x", cmd_ptr->mm_idle_state.rat);
      }

      switch(location_status)
      {
        case GSTK_NORMAL_SERVICE:
        case GSTK_LIMITED_SERVICE:
          gstk_curr_inst_ptr->nas_rat = rat;
          break;
        case GSTK_NO_SERVICE:
          gstk_curr_inst_ptr->nas_rat = GSTK_ACCESS_NONE;
          break;
        default:
          UIM_MSG_HIGH_1("Unknown location status 0x%x", location_status);
          break;
      }
      UIM_MSG_HIGH_1("NAS RAT is 0x%x", gstk_curr_inst_ptr->nas_rat);

      if(gstk_curr_inst_ptr->mm_idle_pri_ptr == NULL)
      {
        gstk_curr_inst_ptr->mm_idle_pri_ptr = (gstk_cmd_type *)GSTK_CALLOC(sizeof(gstk_cmd_type));
      }
      if(gstk_curr_inst_ptr->mm_idle_pri_ptr == NULL)
      {
        break;
      }
      else
      {
        memset(gstk_curr_inst_ptr->mm_idle_pri_ptr, 0, sizeof(gstk_cmd_type));
        (void)gstk_memcpy(gstk_curr_inst_ptr->mm_idle_pri_ptr,
                       cmd_ptr,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
        if(!gstk_util_process_mm_or_mode_change(GSTK_MM_IDLE_ORIG))
        {
          UIM_MSG_HIGH_0("Process Mode Change instead");
          break;
        }
        /* Invalidate 3GPP2 location information as MM IDLE is prioritized */
#ifdef FEATURE_CDMA
        UIM_MSG_HIGH_0("Clear 3GPP2 LOCI cache");
        memset(&gstk_curr_inst_ptr->cdma_ext_loci_cache,
               0xFF,
               sizeof(gstk_curr_inst_ptr->cdma_ext_loci_cache));
        gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid = FALSE;
        gstk_curr_inst_ptr->cdma_ext_loci_cache.is_advance_info_valid = FALSE;
        gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid = FALSE;
#endif /* #ifdef FEATURE_CDMA */
      }

      /* gstk_curr_inst_ptr->gstk_mm_state_is_idle flag is used  as an indication to start queuing RRC messages */
      gstk_curr_inst_ptr->gstk_mm_state_is_idle = TRUE;

      /* get the location status gstk enum */
      gstk_status = gstk_util_map_mm_to_gstk_location_status(&(cmd_ptr->mm_idle_state),
                                                             &location_status);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Bad location status");
        break;
      }

#if defined(FEATURE_GSTK_TAL)
      if(GSTK_SUCCESS == gstk_tal_notify_ls(GSTK_TAL_LOCAL_CLIENT, location_status ))
      {
        UIM_MSG_HIGH_1("TAL: Sending Cmd to set LS to 0x%x for LOCAL",
                       location_status);
      }
#elif defined (FEATURE_GSTK_FUSION_REMOTE)
      if(GSTK_SUCCESS == gstk_tal_notify_ls(GSTK_TAL_REMOTE_CLIENT, location_status ))
      {
        UIM_MSG_HIGH_1("TAL: Sending Cmd to set LS to 0x%x for REMOTE",
                       location_status);
      }
#endif /* FEATURE_GSTK_TAL */

      UIM_MSG_HIGH_9("MM_IDLE Evt PLMN Id : 0x%x 0x%x 0x%x LAC Id : 0x%x 0x%x Cell Id : 0x%x 0x%x 0x%x 0x%x",
                     cmd_ptr->mm_idle_state.mcc_mnc[0],
                     cmd_ptr->mm_idle_state.mcc_mnc[1],
                     cmd_ptr->mm_idle_state.mcc_mnc[2],
                     cmd_ptr->mm_idle_state.lac[0],
                     cmd_ptr->mm_idle_state.lac[1],
                     cmd_ptr->mm_idle_state.cell_id.cell_id[0],
                     cmd_ptr->mm_idle_state.cell_id.cell_id[1],
                     cmd_ptr->mm_idle_state.cell_id.cell_id[2],
                     cmd_ptr->mm_idle_state.cell_id.cell_id[3]);
					 
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      UIM_MSG_HIGH_9("MM-GSTK LOCI Table... PLMN Id : 0x%x 0x%x 0x%x LAC Id : 0x%x 0x%x Cell Id : 0x%x 0x%x 0x%x 0x%x",
                     gstk_curr_inst_ptr->gstk_loci_table.plmn_id[0],
                     gstk_curr_inst_ptr->gstk_loci_table.plmn_id[1],
                     gstk_curr_inst_ptr->gstk_loci_table.plmn_id[2],
                     gstk_curr_inst_ptr->gstk_loci_table.lac[0],
                     gstk_curr_inst_ptr->gstk_loci_table.lac[1],
                     gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[0],
                     gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[1],
                     gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[2],
                     gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[3]);
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE) */

      if (!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat, &rat))
      {
        UIM_MSG_ERR_1("Unknown Sys Rat 0x%x", cmd_ptr->mm_idle_state.rat);
        /* Indicate not to send any LS;  However, if we are in No Service,
           the switch statement on location status below will reset the
           boolean to TRUE, so that we will still send down an envelope
           for NO SERVICE regardless of RAT */
        ls_env = FALSE;
      }

      if (gstk_curr_inst_ptr->gstk_next_curr_rat != rat)
      {
        UIM_MSG_HIGH_0("RAT is different");
        /* Since the previous RAT and current RAT is different, just clear the next curr
           PLMN, LAC/TAC and CELL ID. Those can be copied from MM IDLE now. */
        memset(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
               0xFF,
               sizeof(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc));
        memset(gstk_curr_inst_ptr->gstk_next_curr_lac,
               0xFF,
               sizeof(gstk_curr_inst_ptr->gstk_next_curr_lac));
        memset(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
               0xFF,
               sizeof(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id));
        /* Store the current access technology used */
        gstk_curr_inst_ptr->gstk_next_curr_rat = rat;

        rat_is_different = TRUE;
      }

      /* Update MM-GSTK loci table when you receive new srvc notification with change in plmn/lac.
         So that the loci table can remain most updated for RRC cb checking purpose
      */
      switch(location_status)
      {
        case GSTK_NORMAL_SERVICE:
        case GSTK_LIMITED_SERVICE:
          gstk_curr_inst_ptr->nas_rat = rat;
          /* Build access technology envelope if necessary */
          UIM_MSG_HIGH_1("GSTK_EVT_LIST[GSTK_ACCESS_TECH] is 0x%x",
                         gstk_curr_inst_ptr->gstk_evt_list[GSTK_ACCESS_TECH]);

          if(((gstk_curr_inst_ptr->gstk_evt_list[GSTK_ACCESS_TECH] & 0x01) == 0x01) &&
             rat_is_different)
          { /* has registered for this event */
            gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
#if defined(FEATURE_GSTK_TAL)
            UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                           GSTK_TAL_LOCAL_CLIENT,
                           gstk_curr_inst_ptr->gstk_curr_rat);
            (void)gstk_tal_notify_rat(GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#elif defined(FEATURE_GSTK_FUSION_REMOTE)
            UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                           GSTK_TAL_REMOTE_CLIENT,
                           gstk_curr_inst_ptr->gstk_curr_rat);
            (void)gstk_tal_notify_rat(GSTK_TAL_REMOTE_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#endif /* FEATURE_GSTK_TAL */
            gstk_status = gstk_build_access_technology_envelope(gstk_curr_inst_ptr->gstk_next_curr_rat);
            if(gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Err in building Access Tech Envelope");
            }
          }
          /* Initialized cell ID to what MM returned */
          (void)memscpy(temp_cell_id.cell_id, sizeof(temp_cell_id.cell_id),
                        cmd_ptr->mm_idle_state.cell_id.cell_id, GSTK_MAX_CELL_ID_LEN);
          temp_cell_id.cell_len = cmd_ptr->mm_idle_state.cell_id.cell_len;

          /* Attempting to get the latest cell id from RR and RRC. If no information
             is available, cell id from MM will be used */
          /* Tried getting cell ID one more time */
          if(((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
             ((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat == SYS_RAT_TDS_RADIO_ACCESS) ||
             ((sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat == SYS_RAT_LTE_RADIO_ACCESS))
          {
            UIM_MSG_HIGH_0("Getting latest Cell ID from RR/RRC...");
            if (gstk_get_curr_cell_id_info_int((gstk_sys_radio_access_tech_e_type)cmd_ptr->mm_idle_state.rat,
                                           cmd_ptr->mm_idle_state.mcc_mnc,
                                           cmd_ptr->mm_idle_state.lac,
                                           &temp_cell_id,
                                           cmd_ptr->mm_idle_state.message_header.sim_slot_id) == FALSE)
            {
              UIM_MSG_ERR_0("gstk_get_curr_cell_id_info error");
              ls_env = FALSE;
            }
            else
            {
              /* Update the cmd_ptr cell ID data */
              (void)memscpy(cmd_ptr->mm_idle_state.cell_id.cell_id,
                      sizeof(cmd_ptr->mm_idle_state.cell_id.cell_id),
                      temp_cell_id.cell_id, GSTK_MAX_CELL_ID_LEN);
              cmd_ptr->mm_idle_state.cell_id.cell_len = temp_cell_id.cell_len;
            }
          }
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          /* Update the plmn table regardless of whether cell ID is valid or not because
             we might need it later on */
          gstk_status = gstkutil_update_plmn_lac_cellId_table(
                          MM_GSTK_LOCI_UPDATE,
                          cmd_ptr->mm_idle_state.mcc_mnc,
                          cmd_ptr->mm_idle_state.lac,
                          temp_cell_id,
                          location_status,
                          GSTK_ACCESS_NONE,
                          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE) */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          /* Check if RRC Cell ID Has been received, if it hasn't, do not send down LS yet */
          if ((cmd_ptr->mm_idle_state.rat == (int)SYS_RAT_UMTS_RADIO_ACCESS)
              ||
              (cmd_ptr->mm_idle_state.rat == SYS_RAT_LTE_RADIO_ACCESS)
              ||
              (cmd_ptr->mm_idle_state.rat == (int)SYS_RAT_TDS_RADIO_ACCESS))
          {
            if(gstk_curr_inst_ptr->nas_rat == GSTK_ACCESS_TECH_LTE)
            {
              i = GSTK_RAT_LTE_IDX;
            }
            else
            {
              i = GSTK_RAT_UMTS_IDX;
            }
            if (!gstk_curr_inst_ptr->is_rrc_cell_id_received[i])
            {
              gstk_curr_inst_ptr->gstk_next_curr_location_status = location_status;
              UIM_MSG_HIGH_0("RRC Cell ID has not been received yet - dont send LS envelope");
              ls_env = FALSE;
            }
          }
#endif /* FEATURE_WCDMA || FEATURE_LTE */
          break;

        case GSTK_NO_SERVICE:
           /* Reset to TRUE because if ls_env was set to FALSE at this point, it was
              because we have an unknown rat, but we will still want to send down a
              LS */
          ls_env = TRUE;
          gstk_curr_inst_ptr->gstk_curr_rat = GSTK_ACCESS_NONE;
          gstk_curr_inst_ptr->gstk_next_curr_rat = GSTK_ACCESS_NONE;
          gstk_curr_inst_ptr->nas_rat = GSTK_ACCESS_NONE;
#if defined(FEATURE_GSTK_TAL)
          UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                         GSTK_TAL_LOCAL_CLIENT,
                         gstk_curr_inst_ptr->gstk_curr_rat);
          (void)gstk_tal_notify_rat(GSTK_TAL_LOCAL_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#elif defined(FEATURE_GSTK_FUSION_REMOTE)
          UIM_MSG_HIGH_2("Calling gstk_tal_notify_rat with client 0x%x and rat 0x%x",
                         GSTK_TAL_REMOTE_CLIENT,
                         gstk_curr_inst_ptr->gstk_curr_rat);
          (void)gstk_tal_notify_rat(GSTK_TAL_REMOTE_CLIENT, gstk_curr_inst_ptr->gstk_curr_rat);
#endif /* FEATURE_GSTK_TAL */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          /* Update the plmn table by setting everything to default */
          memset(gstk_curr_inst_ptr->gstk_loci_table.plmn_id, 0xFF, GSTK_MAX_PLMN_LEN);
          memset(gstk_curr_inst_ptr->gstk_loci_table.lac, 0xFF, GSTK_MAX_LAC_LEN);
          gstk_curr_inst_ptr->gstk_loci_table.loc_status = GSTK_NO_SERVICE;
          memset(gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
          gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_len = GSTK_MAX_CELL_ID_LEN;
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE) */
          break;
      } /* end of switch(location_status) */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
       UIM_MSG_HIGH_9("After Updation MM-GSTK LOCI Table PLMN Id : 0x%x 0x%x 0x%x LAC Id : 0x%x 0x%x Cell Id : 0x%x 0x%x 0x%x 0x%x",
                      gstk_curr_inst_ptr->gstk_loci_table.plmn_id[0],
                      gstk_curr_inst_ptr->gstk_loci_table.plmn_id[1],
                      gstk_curr_inst_ptr->gstk_loci_table.plmn_id[2],
                      gstk_curr_inst_ptr->gstk_loci_table.lac[0],
                      gstk_curr_inst_ptr->gstk_loci_table.lac[1],
                      gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[0],
                      gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[1],
                      gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[2],
                      gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id[3]);
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE) */

      UIM_MSG_HIGH_2("LS Env: %d, gstk_mm_param_received: %d",
                     ls_env, gstk_curr_inst_ptr->gstk_mm_param_received);
      if (ls_env == FALSE)
      {
        /* Cell ID Not ready yet in Normal or Limited Service, no need to send LS */
        UIM_MSG_HIGH_0("Cell ID is not ready - dont send LS env");
        break;
      }

      /* check for validity of the data */
      if (!gstk_util_is_valid_location_info(cmd_ptr->mm_idle_state.mcc_mnc,
                                            cmd_ptr->mm_idle_state.lac,
                                            cmd_ptr->mm_idle_state.cell_id.cell_id,
                                            location_status))
      {
        break;
      }

    if (!gstk_curr_inst_ptr->gstk_mm_param_received)
      {
        /* set mm_param_received to TRUE since we have determined that the first
           set of data is good */
        gstk_curr_inst_ptr->gstk_mm_param_received = TRUE;
      }
      else
      {
        /* already received the data, need to see if there is any difference */
        if (!gstk_util_check_mm_state_difference(cmd_ptr->mm_idle_state.mcc_mnc,
                                                 cmd_ptr->mm_idle_state.lac,
                                                 cmd_ptr->mm_idle_state.cell_id.cell_id,
                                                 cmd_ptr->mm_idle_state.cell_id.cell_len,
                                                 location_status))
        {
          /* No difference */
          break;
        }
      }

      /* valid data, copy over the data */
      (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
             sizeof(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id),
             cmd_ptr->mm_idle_state.cell_id.cell_id,
             GSTK_MAX_CELL_ID_LEN);
      gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len = cmd_ptr->mm_idle_state.cell_id.cell_len;
      (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
      	       sizeof(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc),
      	       cmd_ptr->mm_idle_state.mcc_mnc, GSTK_MAX_PLMN_LEN);
      (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_lac,
      	       sizeof(gstk_curr_inst_ptr->gstk_next_curr_lac),
      	       cmd_ptr->mm_idle_state.lac, GSTK_MAX_LAC_LEN);
      gstk_curr_inst_ptr->gstk_next_curr_location_status = location_status;

      if ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY,
                                      gstk_curr_inst_ptr->slot_id) == TRUE) &&
          location_status == GSTK_LIMITED_SERVICE)
      {
        if (!gstk_util_compare_mm_to_nw_rej_cache())
        {
          /* MM idle location status different from NW reject cache*/
          /* Do not send LS */
          break;
        }
      }
      UIM_MSG_HIGH_1("GSTK_EVT_LIST[LOC_STATUS] is 0x%x",
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS]);
      if((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01)
      { /* has registered for this event */
        /* When VPE is enabled, Location Status envelope is queued to head of
           GSTK command queue and additional Location Status envelopes are purged
           once the first location status envlope is processed.
           Hence, queue Location Status Envelope even if one is pending */
        if ((gstk_nv_get_feature_status(
               GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
               gstk_curr_inst_ptr->slot_id) == TRUE) ||
            !gstk_curr_inst_ptr->gstk_pending_location_evt_envelope)
        {
          /* there are location info changes that cause the MM to enter idle */
          /* update the gstk static values */
          (void)memscpy(gstk_curr_inst_ptr->gstk_curr_mcc_mnc,
          	       sizeof(gstk_curr_inst_ptr->gstk_curr_mcc_mnc),
          	       gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, GSTK_MAX_PLMN_LEN);
          (void)memscpy(gstk_curr_inst_ptr->gstk_curr_lac,
          	       sizeof(gstk_curr_inst_ptr->gstk_curr_lac),
          	       gstk_curr_inst_ptr->gstk_next_curr_lac, GSTK_MAX_LAC_LEN);
          (void)memscpy(&gstk_curr_inst_ptr->gstk_curr_cell_id,
          	       sizeof(gstk_curr_inst_ptr->gstk_curr_cell_id),
          	       &gstk_curr_inst_ptr->gstk_next_curr_cell_id, sizeof(gstk_cell_id_type));
          gstk_curr_inst_ptr->gstk_curr_location_status = gstk_curr_inst_ptr->gstk_next_curr_location_status;
          gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
          if ((gstk_curr_inst_ptr->nas_rat != GSTK_ACCESS_TECH_GSM) &&
              (gstk_curr_inst_ptr->is_rrc_cell_id_received[i]))
          {
            gstk_curr_inst_ptr->is_rrc_cell_id_received[i] = FALSE;
            UIM_MSG_HIGH_1("is_rrc_cell_id_received[%d] is set to FALSE", i);
          }
#endif /* FEATURE_WCDMA || FEATURE_LTE */
          /* Send an envelope to the card */
          UIM_MSG_HIGH_1(" Location status envelope sent. MM->GSTK->Card -- Location status is 0x%x...... ",
                         gstk_curr_inst_ptr->gstk_curr_location_status );
          UIM_MSG_HIGH_9(" MM->GSTK->Card -- MNC_MCC is 0x%x 0x%x 0x%x LAC is 0x%x 0x%x Cell Id 0x%x 0x%x 0x%x 0x%x",
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[0],
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[1],
                         gstk_curr_inst_ptr->gstk_curr_mcc_mnc[2],
                         gstk_curr_inst_ptr->gstk_curr_lac[0],
                         gstk_curr_inst_ptr->gstk_curr_lac[1],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[0],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[1],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[2],
                         gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[3]);

          /* When VPE feature is enabled, we will let VPE take care of
           enabling/disabling the CDMA location status envelopes  */
          if((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH,
                                         gstk_curr_inst_ptr->slot_id) == TRUE) &&
             (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                                         gstk_curr_inst_ptr->slot_id) == FALSE) &&
             ((gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_NORMAL_SERVICE) ||
              (gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_LIMITED_SERVICE)))
          {
            if (gstk_util_is_vzw_mcc(gstk_curr_inst_ptr->gstk_curr_mcc_mnc))
            {
              gstk_curr_inst_ptr->cdma_loc_st_disabled = FALSE;
            }
            else
            {
               gstk_curr_inst_ptr->cdma_loc_st_disabled = TRUE;
            }
          }

          gstk_status = gstk_build_location_status_envelope(
                          gstk_curr_inst_ptr->gstk_curr_rat,
                          gstk_curr_inst_ptr->gstk_curr_location_status);

          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Have problem sending location status envelope");
          }
          else
          {
            /* Cleanup NW reject cache to avoid using out of date NW reject info */
            gstk_curr_inst_ptr->nw_rej.access_tech = GSTK_ACCESS_NONE;
            memset(gstk_curr_inst_ptr->nw_rej.mcc_mnc, 0xFF, GSTK_MAX_PLMN_LEN);
            memset(gstk_curr_inst_ptr->nw_rej.lac, 0xFF, GSTK_MAX_LAC_LEN);
            gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
          }
        }
        else
        {
          UIM_MSG_HIGH_0("There is pending location evt evp rsp");
        }
      }
      break;

    case GSTK_TIMER_EXP_CMD:
      UIM_MSG_HIGH_1("Received Timer Exp, %x",
                     cmd_ptr->timer_msg.message_header.command_id);
      switch (cmd_ptr->timer_msg.message_header.command_id)
      {
        case GSTK_ATTACH_CTRL_TIMER:
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_TIMER_EXP;
          gstk_util_send_attach_ctrl_res();
          break;

        case GSTK_CLIENTS_REG_TIMER:
          /* save clients that do TP DL to config data */
          if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_DYNAMIC_TP_CLIENTS, GSTK_SLOT_ID_MAX) == TRUE)
          {
            gstk_client_type    client_type       = GSTK_NO_TYPE;
            uint32              num_expected_tp   = 1;
            uint32              tp_clients        = 0;

            for (i = 0; i < GSTK_MAX_CLIENT; ++i)
            {
              client_type = gstk_shared_data.gstk_client_table[i].client_type;
              if (client_type == GSTK_ESTK_TYPE || client_type == GSTK_UI_TYPE)
              {
                tp_clients |= 0x00000001 << (uint8)client_type;
                ++ num_expected_tp;
              }
            }

            UIM_MSG_HIGH_2("num_expected_tp is 0x%x, gstk_shared_data.num_of_expected_tp is 0x%x",
                           num_expected_tp,
                           gstk_shared_data.num_of_expected_tp);
            if(num_expected_tp > gstk_shared_data.num_of_expected_tp)
            {
              memset(&nv_data, 0x00, sizeof(nv_data));

              nv_data.tp_clients = tp_clients;

              (void)gstk_nv_access_write(
                      GSTK_NV_ME_TP_CLIENTS,
                      GSTK_SLOT_ID_MAX,
                      &nv_data);
            }
          }
          break;

        case GSTK_TR_DELAY_TIMER:
          if(gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length <= 0)
          {
            UIM_MSG_HIGH_0("No TR in Cache");
            gstk_status = GSTK_BAD_PARAM;
            break;
          }
          if(gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length > 0)
          {
            uim_return_type  uim_state = UIM_SUCCESS;
            uim_rpt_type     uim_rpt;
            uim_cmd_type*    uim_cmd_ptr = gstk_get_uim_buf();

            if (uim_cmd_ptr != NULL )
            {
              gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,
                                                                &uim_cmd_ptr);
              (void)memscpy(uim_cmd_ptr->terminal_response.data,
              	     sizeof(uim_cmd_ptr->terminal_response.data),
                     gstk_curr_inst_ptr->gstk_cmd_cache.term_resp,
                     gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length);
              uim_cmd_ptr->terminal_response.num_bytes =
                uint32touint8(
                  gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length);

              uim_cmd_ptr->hdr.user_data = gstk_curr_inst_ptr->tr_unique_ref_id;

              uim_state = uim_cmd(uim_cmd_ptr);
              if(uim_state != UIM_SUCCESS)
              {
                uim_rpt.user_data   = uim_cmd_ptr->hdr.user_data;
                uim_rpt.sw1         = SW1_PROBLEM;
                uim_rpt.sw2         = SW2_NORMAL_END;
                uim_rpt.rpt_status  = UIM_FAIL;
                uim_rpt.slot        = uim_cmd_ptr->hdr.slot;
                uim_rpt.rpt_type    = UIM_TERMINAL_RESPONSE_R;
                gstk_uim_terminal_response_report(&uim_rpt);
              }
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              uim_cmd_ptr = NULL;                  
            }
          }
          else
          {
            UIM_MSG_ERR_0("Zero Length TR");
          }
          break;

      case GSTK_TIMER_MGMT_TIMER:
          timer_idx = cmd_ptr->timer_msg.message_header.user_data;
          gstk_curr_inst_ptr->gstk_timer_management[timer_idx].time +=
            GSTK_TIMER_ELAPSED;
          /* restart the timer */
          (void)rex_set_timer(
             &(gstk_curr_inst_ptr->gstk_timer_management[timer_idx].timer_elapsed),
             GSTK_TIMER_ELAPSED);
          break;

      case GSTK_LOC_INFO_TIMER:
        UIM_MSG_HIGH_1("Location information envelope timer expiry0x%x",
                       gstk_curr_inst_ptr->is_tp_dl_done);
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
        /* Loc Info timer is reused for OTASP delay implementation (as Loc Info
           execution does not intersect with Card activation sequence */
        if (gstk_curr_inst_ptr->open_ch_imsi_m_ptr)
        {
          gstk_task_cmd_type* task_cmd_ptr = NULL;

          UIM_MSG_HIGH_0("OTASP delay timer expired");
          (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
          gstk_curr_inst_ptr->otasp_requeued = TRUE;

          /* Queue OTASP command to GSTK in order to process cached OPEN CHANNEL
             (as 1X did not become available in the duration timer was
             running) */
          task_cmd_ptr = gstk_task_get_cmd_buf();
          if (task_cmd_ptr != NULL)
          {
            task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
            task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_ORIG;
            task_cmd_ptr->cmd.general_cmd.message_header.user_data =
              (uint32)gstk_curr_inst_ptr->open_ch_imsi_m_ptr;
            task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id = gstk_curr_inst_ptr->slot_id;
            gstk_task_put_cmd_buf(task_cmd_ptr);
          }
          break;
        }
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */

        if (gstk_nv_get_feature_status(
              GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
              gstk_curr_inst_ptr->slot_id) == TRUE)
        {
          gstk_curr_inst_ptr->is_first_loc_info_envelope_sent = TRUE;
          if (!gstk_curr_inst_ptr->is_tp_dl_done)
          {
            gstk_cmd_from_card_type         *response_ptr = NULL;

            response_ptr = (gstk_cmd_from_card_type *)GSTK_CALLOC(
                                   sizeof(gstk_cmd_from_card_type));
            if(response_ptr != NULL)
            {
              response_ptr->hdr_cmd.command_id           = GSTK_PROFILE_DL_IND_RSP;
              response_ptr->hdr_cmd.sim_slot_id          = cmd_ptr->timer_msg.message_header.sim_slot_id;
              response_ptr->hdr_cmd.cmd_detail_reference = 0;
              response_ptr->hdr_cmd.command_number       = 0;
              response_ptr->hdr_cmd.user_data            = gstk_curr_inst_ptr->tp_info.user_data;
            if(gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb != NULL)
            {
                (gstk_curr_inst_ptr->tp_info.gstk_tp_dl_cb)(response_ptr);
              }
              gstk_free(response_ptr);
              response_ptr = NULL;
            }
            gstk_curr_inst_ptr->is_tp_dl_done = TRUE;
          }
        }
        break;
      case GSTK_TR_TIMER1:
      case GSTK_TR_TIMER2:
      case GSTK_TR_TIMER3:
      case GSTK_TR_TIMER4:
      case GSTK_TR_TIMER5:
      case GSTK_TR_TIMER6:
      case GSTK_TR_TIMER7:
      case GSTK_TR_TIMER8:
        /* user_data in cmd contains command details ref id */
        gstk_status = gstk_util_compare_ref_id(cmd_ptr->timer_msg.message_header.user_data);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }

        gstk_status = gstk_process_term_rsp_timer_expire(
                        cmd_ptr->timer_msg.message_header.user_data);

#ifdef FEATURE_ESTK
        gstk_status = estk_clean_up_on_gstk_timer_expiry(gstk_curr_inst_ptr->slot_id);
        UIM_MSG_HIGH_1("Clean up ESTK returns %d", gstk_status);
#endif /* FEATURE_ESTK */
        break;
      default:
        UIM_MSG_ERR_1("Unhandled timer exp: 0x%x",
                      cmd_ptr->timer_msg.message_header.command_id);
        break;
      }
      break;

    case GSTK_STOP_CMD:
      UIM_MSG_HIGH_0("Received STOP Sig");
      /* flush GSTK queue, clean up and ack tmc */
      gstk_status = gstk_power_down();
      if (gstk_status == GSTK_SUCCESS)
      {
        gstk_curr_inst_ptr->gstk_state = GSTK_POWER_DOWN_S;
      }
      break;

    case GSTK_MODE_CHANGE_IND:
      UIM_MSG_HIGH_2("Mode Change Ind: cmd_id 0x%x, nas_rat 0x%x",
                     cmd_ptr->mode_change_ind.msg_hdr.command_id,
                     gstk_curr_inst_ptr->nas_rat);

      /* As a first step, take required actions depending on the source of command */
      if(cmd_ptr->mode_change_ind.msg_hdr.command_id == GSTK_MODE_CHANGE_FROM_CM)
      {
        gstk_task_cmd_type* task_cmd_ptr = NULL;
        gstk_mode_change_cmd_id_enum_type compare_val = GSTK_MODE_CHANGE_FROM_TIMER;

        /* Update bmsk_3gpp2 with latest information */
        gstk_curr_inst_ptr->bmsk_3gpp2 = 0;
        gstk_process_mode_change_update_3gpp2_bmsk(&(cmd_ptr->mode_change_ind));
        /* Let CDMA location status timer run as long as we have 3GPP2 service.
           This is to avoid sending insufficient CDMA location information to
           card before timer expires in case when MODE CHANGE command gets
           requeued. For e.g: Consider this transition, 1x -> 1x+LTE -> 1x, with
           in the duration of CDMA location timer expiry */
        if(gstk_curr_inst_ptr->bmsk_3gpp2 == 0 &&
           rex_get_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer)) > 0)
        {
          UIM_MSG_HIGH_0 ("Clear gstk_cdma_loc_status_timer");
          (void)rex_clr_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer));
        }
        /* Delete the MODE CH command in GSTK command queue which is added by
           timer callback, if any. */
        rex_enter_crit_sect (&gstk_shared_data.gstk_crit_sect);
        if ((task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new(&gstk_task_cmd_q,
                                                                      gstk_mode_ch_compare_func,
                                                                      &compare_val,
                                                                      NULL,
                                                                      NULL)) != NULL)
        {
          gstk_task_free_cmd_buf (task_cmd_ptr);
        }
        rex_leave_crit_sect (&gstk_shared_data.gstk_crit_sect);
      }
      else if(cmd_ptr->mode_change_ind.msg_hdr.command_id == GSTK_MODE_CHANGE_FROM_REQUEUE)
      {
        gstk_mode_change_cmd_id_enum_type compare_val = GSTK_MODE_CHANGE_FROM_CM;
        /* If CDMA loc status timer is running, wait until it is expired to
           process newer CM SS information */
        if(rex_get_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer)) > 0)
        {
          UIM_MSG_HIGH_0("Wait for CDMA location status timer to expire");
          break;
        }
        /* It's not needed to process this MODE_CHANGE_FROM_REQUEUE if there
           are any MODE_CHANGE_FROM_CM or MODE_CHANGE_FROM_TIMER commands in
           GSTK command queue. That's because those commands are newer */
        if (NULL != q_linear_search(&gstk_task_cmd_q,
                                    gstk_mode_ch_compare_func,
                                    &compare_val))
        {
          UIM_MSG_HIGH_0("Skip processing MODE_CHANGE_FROM_REQUEUE");
          break;
        }
        compare_val = GSTK_MODE_CHANGE_FROM_TIMER;
        if (NULL != q_linear_search(&gstk_task_cmd_q,
                                    gstk_mode_ch_compare_func,
                                    &compare_val))
        {
          UIM_MSG_HIGH_0("Skip processing MODE_CHANGE_FROM_REQUEUE");
          break;
        }
      }
      else if(cmd_ptr->mode_change_ind.msg_hdr.command_id == GSTK_MODE_CHANGE_FROM_TIMER)
      {
        if(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr == NULL)
        {
          UIM_MSG_HIGH_0("CDMA Loc Status timer expired - cmd ptr is NULL");
          break;
        }
       (void)gstk_memcpy(cmd_ptr,
                       gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
        UIM_MSG_HIGH_1("gstk_cdma_loc_status_cmd_ptr cmd id is 0x%x",
                       gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr->mode_change_ind.msg_hdr.command_id);
        cmd_ptr->mode_change_ind.msg_hdr.command_id =
          GSTK_MODE_CHANGE_FROM_TIMER;
        gstk_free(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr);
        gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr = NULL;
      }
      else
      {
        UIM_MSG_ERR_0 ("Mode Change Ind: Incorrect command_id");
        break;
      }

      if(gstk_curr_inst_ptr->mode_change_cmd_ptr == NULL)
      {
        gstk_curr_inst_ptr->mode_change_cmd_ptr =
          (gstk_cmd_type *)GSTK_CALLOC(sizeof(gstk_cmd_type));
      }
      if(gstk_curr_inst_ptr->mode_change_cmd_ptr == NULL)
      {
        break;
      }
      else
      {
        memset(gstk_curr_inst_ptr->mode_change_cmd_ptr, 0, sizeof(gstk_cmd_type));
        /* Cache the command */
        (void)gstk_memcpy(gstk_curr_inst_ptr->mode_change_cmd_ptr,
                       cmd_ptr,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
#ifndef FEATURE_UIM_TEST_FRAMEWORK
        if (gstk_nv_get_feature_status(
              GSTK_CFG_FEATURE_SLOT_MODE_SWITCH_STK_SWITCH,
              gstk_curr_inst_ptr->slot_id) == TRUE)
        {
          gstk_access_technology_type     serv_access_tech  = GSTK_ACCESS_NONE;
          uim_dir_type                    df                = NULL;
          slot = cmd_ptr->mode_change_ind.msg_hdr.sim_slot_id;
          gstk_find_normal_service_rat_when_mode_ch(&serv_access_tech,&(cmd_ptr->mode_change_ind));
       	  if (gstk_check_send_tp_when_mode_ch(serv_access_tech, slot))
       	  {
       	    if (gstk_check_access_tech_for_3gpp2(serv_access_tech))
            {
              gstk_curr_inst_ptr->gstk_curr_tech_group = GSTK_ACCESS_TECH_3GPP2;
              df = UIM_DF_CDMA_TYPE;
            }
            else if (gstk_check_access_tech_for_3gpp(serv_access_tech))
            {
              gstk_curr_inst_ptr->gstk_curr_tech_group = GSTK_ACCESS_TECH_3GPP;
              df = UIM_DF_GSM_TYPE;
            }

            /* Find and process any pending envelope responses and terminal response report in gstk_cmd_q */
            gstk_util_find_and_process_pending_responses();

            /* Cleanup pending envelope commands by sending error envelope response */
            for(i = 0; i <= GSTK_MAX_PROACTIVE_COMMAND; ++i)
            {
              if(gstk_util_get_sim_slot_id(gstk_shared_data.envelope_cmd_ref_table[i].client_id) == slot &&
                 !gstk_shared_data.envelope_cmd_ref_table[i].is_free )
              {
                if(gstk_util_build_error_envelope_response(i) != GSTK_SUCCESS)
                {
                  gstk_util_env_cmd_ref_table_free_slot(i);
                }
              }
            }

            /* Clear retry data since we already cleared envelope_cmd_ref_table */
            gstk_util_clear_retry_envelope_data(slot);

            /* Clean up Pending proactive command */
            for(i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
            {
              if ((!command_details_ref_table_ptr[i].free_slot) &&
                  (command_details_ref_table_ptr[i].sim_slot_id ==  slot) )
              {
                gstk_util_cmd_details_ref_table_free_slot(i);
                /* Only one proactive command can be processed at a time */
                break;
              }
            }

            if(gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress == TRUE)
            {
              /* find and send end proactive seesion for pending proactive command */
              gstk_util_find_and_build_proactive_end_session(GSTK_TAG_TP_DOWNLOAD);
              /* 1. Enable FETCH by resetting all proactive cmd related variables and
                 2. Notify all clients of the end of proactive session */
              GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
              gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress, FALSE);
              if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress ==
                   gstk_curr_inst_ptr->slot_id)
              {
                gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
              }
            }
            /* Send TP with CDMA/GSM DF */
       	    gstk_status = gstk_pack_tp_cmd_to_uim_with_df(df);

       	    if (gstk_status == GSTK_SUCCESS)
            {
       		  /* go to wait for TP response state */
       		  UIM_MSG_HIGH_1("Send TP success, %x", gstk_status);
       		  gstk_curr_inst_ptr->gstk_state = GSTK_TERM_PROFILE_RSP_WAIT_S;
       	    }
       	    else
            {
       		  UIM_MSG_ERR_0("Send TP error");
       	    }
       	  }
          else
          {
            UIM_MSG_HIGH_0("NO need to send TP");
          }
        }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
        /* Change the command_id of the command cached so that it will be
           treated appropriately when requeued */
        gstk_curr_inst_ptr->mode_change_cmd_ptr->mode_change_ind.msg_hdr.command_id =
          GSTK_MODE_CHANGE_FROM_REQUEUE;
        if(gstk_curr_inst_ptr->mm_idle_pri_ptr != NULL)
        {
          if(!gstk_check_cm_stacks_for_3gpp(&(cmd_ptr->mode_change_ind)))
          {
            UIM_MSG_HIGH_0("Free MM_IDLE cache and wait for next MM IDLE evt");
            gstk_free(gstk_curr_inst_ptr->mm_idle_pri_ptr);
            gstk_curr_inst_ptr->mm_idle_pri_ptr = NULL;
          }
        }
        /* Arbitrate between priority MM IDLE and priority MODE CHANGE */
        if(!gstk_util_process_mm_or_mode_change(GSTK_MODE_CHANGE_ORIG))
        {
          UIM_MSG_HIGH_0("Process MM Idle instead");
          break;
        }
      }

      /*** CM MODE CHANGE IND has been prioritized over MM Idle ***/

      /* Run arbitration on all CM stacks for priority 3GPP2 RAT among them */
      if(gstk_util_find_priority_mode_change_rat(
          &access_tech,
          &location_status,
          &(cmd_ptr->mode_change_ind)) != GSTK_SUCCESS)
      {
        break;
      }
#if defined(FEATURE_GSTK_TAL)
      if(GSTK_SUCCESS == gstk_tal_notify_ls(GSTK_TAL_LOCAL_CLIENT,
                                            location_status))
      {
        UIM_MSG_HIGH_1("TAL: Sending Cmd to set LS to 0x%x for LOCAL",
                       location_status);
      }
#elif defined (FEATURE_GSTK_FUSION_REMOTE)
      if(GSTK_SUCCESS == gstk_tal_notify_ls(GSTK_TAL_REMOTE_CLIENT,
                                            location_status))
      {
        UIM_MSG_HIGH_1("TAL: Sending Cmd to set LS to 0x%x for REMOTE",
                       location_status);
      }
#endif /* FEATURE_GSTK_TAL */
      /* If the command is added by CM task and access tech is one of the 3GPP2
         RATs then start the timer to watch out for better information from CM.
         If there is no MODE CHANGE event from CM, this command gets processed
         after timer expires */
      if((cmd_ptr->mode_change_ind.msg_hdr.command_id == GSTK_MODE_CHANGE_FROM_CM) &&
         (access_tech != GSTK_ACCESS_NONE))
      {
        gstk_task_cmd_type* task_cmd_ptr = NULL;
        gstk_mode_change_cmd_id_enum_type compare_val = GSTK_MODE_CHANGE_FROM_TIMER;

        /* Clear the timer if it's still running and then set again */
        if(rex_get_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer)) > 0)
        {
          UIM_MSG_HIGH_0("Clear and restart gstk_cdma_loc_status_timer");
          (void)rex_clr_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer));
        }
        (void)rex_set_timer(&(gstk_curr_inst_ptr->gstk_cdma_loc_status_timer),
                            GSTK_CDMA_LOC_STATUS_ENVELOPE_WAIT_TIME);
        /* Delete the MODE CH command in GSTK command queue which is added by
           timer callback, if any. */
        rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
        if ((task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new(&gstk_task_cmd_q,
                                                                      gstk_mode_ch_compare_func,
                                                                      &compare_val,
                                                                      NULL,
                                                                      NULL)) != NULL)
        {
          gstk_task_free_cmd_buf(task_cmd_ptr);
        }
        rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
        if(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr == NULL)
        {
          gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr =
            (gstk_cmd_type *)GSTK_CALLOC(sizeof(gstk_cmd_type));
          if(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr == NULL)
          {
            gstk_status = GSTK_MEMORY_ERROR;
            break;
          }
        }
        (void)gstk_memcpy(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr,
                       cmd_ptr,
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
        gstk_status = GSTK_SUCCESS;
        break;
      }

      /* Access Technology Envelope */
      if((!gstk_curr_inst_ptr->gstk_mm_state_is_idle) &&
         (access_tech == GSTK_ACCESS_NONE))
      {
        gstk_curr_inst_ptr->gstk_next_curr_rat = GSTK_ACCESS_NONE;
      }
      /* If FEATURE CDMA and FEATURE GSM/WCDMA are turned on, then gstk allows only
       * MM events to update RAT info if RAT == none This is to prevent cm events
       * (where RAT == none) from overwriting MM events due to timing and other issues */

#ifdef FEATURE_CDMA
      /* merge 2 CM stacks into a temp cache */
      if (!gstk_util_merge_cm_stacks(&cdma_ext_temp, &cmd_ptr->mode_change_ind))
      {
        UIM_MSG_ERR_0 ("Fail to merge CM stacks");
        gstk_status = GSTK_ERROR;
        break;
      }

        /* CM updates global gstk rat info only if Access Tech is CDMA */
        if((access_tech != GSTK_ACCESS_NONE))
        {
          if(gstk_curr_inst_ptr->gstk_next_curr_rat != access_tech)
          {
            rat_is_different = TRUE;
          }
          else
          {
            rat_is_different = FALSE;
          }
          gstk_curr_inst_ptr->gstk_next_curr_rat = access_tech;
          if (gstk_curr_inst_ptr->gstk_evt_list[GSTK_ACCESS_TECH] & 0x01)
          {
            /* Send Access Technlogy Envelope only if the current access technology
                is different from the previously reported access technology envelope
            */
            if(rat_is_different)
            {
              /* Build AcT envelope to send down Access Technology change to
                 the card */
              gstk_curr_inst_ptr->gstk_curr_rat = access_tech;
              gstk_status = gstk_build_access_technology_envelope(access_tech);
              if(gstk_status != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("Err in building Access Tech Envelope");
              }
            }
          }
        }

        do
        {
          if (location_status == GSTK_NO_SERVICE &&
              gstk_curr_inst_ptr->nas_rat != GSTK_ACCESS_NONE)
          {
            /* Wait for UE to scan atleast the 3GPP PLMNs before sending
               NO Service to card. Once UE scans and find no service on 3GPP
               PLMNs, MM will report new MM IDLE evt */
            UIM_MSG_HIGH_1("Do not send the no service to card yet - nas_rat: 0x%x",
                           gstk_curr_inst_ptr->nas_rat);
            ls_env = FALSE;
            break;
          }
          /* Check whether calculated location status is same as cached value or not */
          if (location_status != gstk_curr_inst_ptr->gstk_next_curr_location_status)
          {
            UIM_MSG_HIGH_2("NO MATCH LS: NEW: 0x%x OLD: 0x%x",
                           location_status,
                           gstk_curr_inst_ptr->gstk_next_curr_location_status);
            /* Since we found there is a difference either in LS, update the
               loc status */
            gstk_curr_inst_ptr->gstk_next_curr_location_status = location_status;
          }
          else
          {
            /* If the calculated location status is same as cached value, then
               there could be two possibilities
               (1) Old LS and New LS are both no service, in which case we
                   shouldn't send LS to card
               (2) Old LS and New LS are both in service. In this case we need
                   to check whether any information is changed or not. If
                   changed, we need to send the LS to card */
            if(location_status == GSTK_NO_SERVICE ||
               !gstk_util_diff_cdma_ext_loc_info(&gstk_curr_inst_ptr->cdma_ext_loci_cache, &cdma_ext_temp))
            {
              /* There is no difference in LS information. So, we don't need to
                 update our cache and send LS envelope to card */
              UIM_MSG_HIGH_0 ("No change in 3GPP2 location info, no LS");
              ls_env = FALSE;
              break;
            }
          }

          curr_3gpp2_rat = gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_3GPP2_MASK;
          if(location_status == GSTK_NO_SERVICE || access_tech == GSTK_ACCESS_NONE)
          {
            UIM_MSG_HIGH_0("Clear 3GPP2 LOCI cache");
            memset(&gstk_curr_inst_ptr->cdma_ext_loci_cache,
                   0xFF,
                   sizeof(gstk_curr_inst_ptr->cdma_ext_loci_cache));
            gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid = FALSE;
            gstk_curr_inst_ptr->cdma_ext_loci_cache.is_advance_info_valid = FALSE;
            gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid = FALSE;
          }
          else
          {
            /* Update cache with new information */
            UIM_MSG_HIGH_0("Update 3GPP2 LOCI cache");
            gstk_curr_inst_ptr->cdma_ext_loci_cache = cdma_ext_temp;
            if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == FALSE)
            {
              if (!(curr_3gpp2_rat & GSTK_ACCESS_TECH_CDMA_MASK))
              {
                UIM_MSG_ERR_0("3GPP2 service is available but no 1x available - No LS");
                ls_env = FALSE;
                break;
              }
            }
          }
        } while(0);

        if (ls_env == TRUE &&
            ((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01))
        {
        /* When VPE is enabled, Location Status envelope is queued to head of
           GSTK command queue and additional Location Status envelopes are purged
           once the first location status envlope is processed.
           Hence, queue Location Status Envelope even if one is pending */
          if((gstk_nv_get_feature_status(
                GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                gstk_curr_inst_ptr->slot_id) == TRUE) ||
             !gstk_curr_inst_ptr->gstk_pending_location_evt_envelope)
          {
            /* Build an envelope to send down the Location change details to
               the card */
            gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
            gstk_curr_inst_ptr->gstk_curr_location_status =
               gstk_curr_inst_ptr->gstk_next_curr_location_status;
            if ((gstk_curr_inst_ptr->cdma_loc_st_disabled) &&
                (cmd_ptr->mode_change_ind.cm_stack[0].loc_status == GSTK_NORMAL_SERVICE ||
                 cmd_ptr->mode_change_ind.cm_stack[1].loc_status == GSTK_NORMAL_SERVICE))
            {
                UIM_MSG_HIGH_0("Skip CDMA location st event to card");
                break;
            }
            gstk_status = gstk_build_location_status_envelope(
                            access_tech,
                            location_status);
            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Have problem sending location status envelope");
            }
            else
            {
              gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
            }
          }
          else
          {
            UIM_MSG_HIGH_0("There is pending location evt env rsp");
          }
        }
#else  /*#ifdef FEATURE_CDMA */
        UIM_MSG_HIGH_0("GSTK_MODE_CHANGE_IND : Feature CDMA not supported");
#endif /*#ifdef FEATURE_CDMA */
      break;

    case GSTK_CM_CSG_CHANGE_IND:
      if(cat_version >= GSTK_CFG_CAT_VER9)
      {
        /* Check the cached SETUP EVENT LIST to see whether card needs
           CSG Cell Selection changes to be notified */
        if(gstk_curr_inst_ptr->gstk_evt_list[GSTK_CSG_CELL_SELECTION] & 0x01)
        {
          gstk_csg_change_ind_type        csg_evt;
          
          memset(&csg_evt, 0x00, sizeof(gstk_csg_change_ind_type));
          
          UIM_MSG_HIGH_2("CSG: Access Tech 0x%x, CSG_ID: 0x%x",
                         cmd_ptr->csg_change_ind.cmd.access_tech,
                         cmd_ptr->csg_change_ind.cmd.csg_id);
          /* Copy CSG ID */
          csg_evt.cmd.csg_id = cmd_ptr->csg_change_ind.cmd.csg_id;
          /* Copy Home (e)NodeB Name length */
          csg_evt.cmd.len = cmd_ptr->csg_change_ind.cmd.len;
          /* Copy Home (e)NodeB Name */
          if(csg_evt.cmd.len > 0)
          {
            if(gstk_memcpy(csg_evt.cmd.hnb_name,
                           cmd_ptr->csg_change_ind.cmd.hnb_name,
                           csg_evt.cmd.len,
                           GSTK_HNB_NAME_MAX_LEN,
                           GSTK_HNB_NAME_MAX_LEN) <
               csg_evt.cmd.len)
            {
              gstk_status = GSTK_MEMORY_ERROR;
              break;
            }
          }
          /* Copy access tech */
          csg_evt.cmd.access_tech =
            cmd_ptr->csg_change_ind.cmd.access_tech;

          /* Copy PLMN Information */
          (void)gstk_memcpy(csg_evt.cmd.plmn_info,
                            cmd_ptr->csg_change_ind.cmd.plmn_info,
                            3,
                            3,
                            3);

          /* Calculate CSG cell selection status */
          if(!cmd_ptr->csg_change_ind.cmd.selection_status.general_info.is_camped)
          {
            csg_evt.cmd.selection_status.general_info.is_camped = FALSE;
            csg_evt.cmd.selection_status.addi_info.is_addi_info_present = FALSE;
            if(!gstk_curr_inst_ptr->csg_info.selection_status.general_info.is_camped)
            {
              break;
            }
          }
          else
          {
            csg_evt.cmd.selection_status.general_info.is_camped = TRUE;
            csg_evt.cmd.selection_status.addi_info.is_addi_info_present = TRUE;
          }
          if(memcmp(&gstk_curr_inst_ptr->csg_info, &csg_evt.cmd, sizeof(gstk_csg_info_type)) == 0 &&
              gstk_curr_inst_ptr->is_csg_info_sent == TRUE)
          {
            /** same information is already sent **/
            break;
          }
          /** Cache the CSG information for future reference **/
          (void)gstk_memcpy(&gstk_curr_inst_ptr->csg_info,
                            &csg_evt.cmd,
                            sizeof(gstk_curr_inst_ptr->csg_info),
                            sizeof(gstk_csg_info_type),
                            sizeof(csg_evt.cmd));
          
          gstk_curr_inst_ptr->is_csg_info_sent = FALSE;

          /* Build an CSG cell selection envelope to card */
          gstk_status = gstk_build_csg_cell_selection_envelope(&csg_evt);

          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Problem sending CSG cell selection envelope");
          }
        }
      }
      else
      {
        UIM_MSG_HIGH_1("GSTK_CM_CSG_CHANGE_IND: Unsupported CAT ver: 0x%x",
                       cat_version);
      }
      break;

    case GSTK_RRC_CELL_CHANGE_IND:
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      /* We will only come to this state if we received cb from RRC and there is
         no PLMN difference, i.e., only cell ID has been changed and we are not
         expecting a MM_IDLE event; therefore, we will have to send LS from here */
      /* Case   :- When  most recent Service state is either Limited Srvc or Normal Srvc
         Action :- Query MM for location info like mcc, mnc and lac.
                   Check for any difference in location information
                   If any, send envelope to card
      */

      UIM_MSG_HIGH_3("In Case GSTK_RRC_CELL_CHANGE_IND: RRC RAT=0x%x NAS RAT=0x%x GSTK Service=0x%x",
                     cmd_ptr->rrc_cell_change_ind.rrc_rat,
                     gstk_curr_inst_ptr->nas_rat,
                     cmd_ptr->rrc_cell_change_ind.location_state);

      if(cmd_ptr->rrc_cell_change_ind.rrc_rat != gstk_curr_inst_ptr->nas_rat)
      {
        UIM_MSG_HIGH_2("RRC RAT 0x%x does not match with NAS RAT 0x%x - drop this",
                       cmd_ptr->rrc_cell_change_ind.rrc_rat,
                       gstk_curr_inst_ptr->nas_rat);
        break;
      }

      if((cmd_ptr->rrc_cell_change_ind.location_state == (uint8)GSTK_NORMAL_SERVICE) ||
           (cmd_ptr->rrc_cell_change_ind.location_state == (uint8)GSTK_LIMITED_SERVICE))
      {
        if (!gstk_util_is_valid_location_info(cmd_ptr->rrc_cell_change_ind.mcc_mnc,
                                              cmd_ptr->rrc_cell_change_ind.lac,
                                              cmd_ptr->rrc_cell_change_ind.cell_info.cell_id,
                                              (gstk_location_status_enum_type)cmd_ptr->rrc_cell_change_ind.location_state)
            && (gstk_curr_inst_ptr->gstk_curr_location_status == gstk_curr_inst_ptr->gstk_next_curr_location_status))
        {
          UIM_MSG_ERR_0("GSTK received Invalid location info from MM and RRC");
          break;
        }
        /* already received the data, need to see if there is any difference */
        if (!gstk_util_check_mm_state_difference(cmd_ptr->rrc_cell_change_ind.mcc_mnc,
                                                 cmd_ptr->rrc_cell_change_ind.lac,
                                                 cmd_ptr->rrc_cell_change_ind.cell_info.cell_id,
                                                 cmd_ptr->rrc_cell_change_ind.cell_info.cell_len,
                                                 (gstk_location_status_enum_type)cmd_ptr->rrc_cell_change_ind.location_state))
        {
          /* No difference */
          break;
        }
        /* Copy Cell Information -  Cell ID, Cell ID Len */
        (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
               sizeof(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id),
               cmd_ptr->rrc_cell_change_ind.cell_info.cell_id,
               GSTK_MAX_CELL_ID_LEN);
        gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len = cmd_ptr->rrc_cell_change_ind.cell_info.cell_len;

        /* Copy MNC,MCC and LAC Information */
        (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
                      sizeof(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc),
                      cmd_ptr->rrc_cell_change_ind.mcc_mnc, GSTK_MAX_PLMN_LEN);
        (void)memscpy(gstk_curr_inst_ptr->gstk_next_curr_lac,
        	      sizeof(gstk_curr_inst_ptr->gstk_next_curr_lac),
        	      cmd_ptr->rrc_cell_change_ind.lac, GSTK_MAX_LAC_LEN);

        gstk_curr_inst_ptr->gstk_next_curr_location_status =
          (gstk_location_status_enum_type)cmd_ptr->rrc_cell_change_ind.location_state;
        gstk_curr_inst_ptr->gstk_next_curr_rat = cmd_ptr->rrc_cell_change_ind.rrc_rat;
        i = (int)cmd_ptr->rrc_cell_change_ind.message_header.user_data;

        /* No need to copy  srvc type as gstk_curr_inst_ptr->gstk_next_curr_location_status does not change here in this case st */

        UIM_MSG_HIGH_1("gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] = 0x%x",
                       gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS]);
        /* Check evt list to see if card requested for location status envelope */
        if((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01)
        {
          /* When VPE is enabled, Location Status envelope is queued to head of
             GSTK command queue and additional Location Status envelopes are purged
             once the first location status envlope is processed.
             Hence, queue Location Status Envelope even if one is pending */
          if ((gstk_nv_get_feature_status(
                 GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                 gstk_curr_inst_ptr->slot_id) == TRUE) ||
              !gstk_curr_inst_ptr->gstk_pending_location_evt_envelope)
          {
            /* there are location info changes that cause the MM to enter idle */
            /* update the gstk static values */
            (void)memscpy(gstk_curr_inst_ptr->gstk_curr_mcc_mnc,
                       sizeof(gstk_curr_inst_ptr->gstk_curr_mcc_mnc),
                       gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, GSTK_MAX_PLMN_LEN);
            (void)memscpy(gstk_curr_inst_ptr->gstk_curr_lac,
            	       sizeof(gstk_curr_inst_ptr->gstk_curr_lac),
            	       gstk_curr_inst_ptr->gstk_next_curr_lac, GSTK_MAX_LAC_LEN);
            (void)memscpy(&gstk_curr_inst_ptr->gstk_curr_cell_id,
            	       sizeof(gstk_curr_inst_ptr->gstk_curr_cell_id),
            	       &gstk_curr_inst_ptr->gstk_next_curr_cell_id, sizeof(gstk_cell_id_type));
            gstk_curr_inst_ptr->gstk_curr_location_status = gstk_curr_inst_ptr->gstk_next_curr_location_status;
            gstk_curr_inst_ptr->gstk_curr_rat = gstk_curr_inst_ptr->gstk_next_curr_rat;

            /* Send an envelope to the card */

            gstk_curr_inst_ptr->is_rrc_cell_id_received[i] = FALSE;

            UIM_MSG_HIGH_2(" Location status envelope information sent. MM->GSTK->Card -- Location status is 0x%x  Cell Len  : 0x%x",
                           gstk_curr_inst_ptr->gstk_curr_location_status,
                           gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_len);
            UIM_MSG_HIGH_9(" MM->GSTK->Card -- MNC_MCC is 0x%x 0x%x 0x%x LAC is 0x%x 0x%x Cell Id : 0x%x 0x%x 0x%x 0x%x",
                           gstk_curr_inst_ptr->gstk_curr_mcc_mnc[0],
                           gstk_curr_inst_ptr->gstk_curr_mcc_mnc[1],
                           gstk_curr_inst_ptr->gstk_curr_mcc_mnc[2],
                           gstk_curr_inst_ptr->gstk_curr_lac[0],
                           gstk_curr_inst_ptr->gstk_curr_lac[1],
                           gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[0],
                           gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[1],
                           gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[2],
                           gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id[3]);

            gstk_status = gstk_build_location_status_envelope(
                            gstk_curr_inst_ptr->gstk_curr_rat,
                            gstk_curr_inst_ptr->gstk_curr_location_status);

            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Have problem sending location status envelope");
            }
            else
            {
              gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = TRUE;
            }
          }/* if (!gstk_curr_inst_ptr->gstk_pending_location_evt_envelope) */
          else
          {
            UIM_MSG_HIGH_0("There is pending location evt evp rsp");
          }
        }/*  if((gstk_curr_inst_ptr->gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01) */
      }/* if((gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_LIMITED_SERVICE) ||
       (gstk_curr_inst_ptr->gstk_curr_location_status == GSTK_NORMAL_SERVICE)) */
#endif /* FEATURE_WCDMA || FEATURE_LTE */
      break;

    case GSTK_DS_EVT_CMD:
      gstk_process_ds_evt_cmd(cmd_ptr);
      break;

    case GSTK_CM_PH_IND:
      if(cat_version >= GSTK_CFG_CAT_VER6)
      {
        switch (cmd_ptr->cm_ph_ind.cm_ph_event)
        {
          case GSTK_CM_PH_EVENT_SYS_SEL_PREF:
            if(gstk_curr_inst_ptr->gstk_evt_list[GSTK_NW_SEARCH_MODE] & 0x01)
            {
              /* Build an envelope to send down the Access Technology change details to the card */
              gstk_status = gstk_build_nw_search_mode_change_envelope(cmd_ptr->cm_ph_ind.nw_search_mode);
              if(gstk_status != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("Err in building Network Search Mode Change Envelope");
              }
            }
            gstk_curr_inst_ptr->nw_search_mode = cmd_ptr->cm_ph_ind.nw_search_mode;
            break;

          case GSTK_CM_PH_EVENT_INFO:
            (void)gstk_memcpy(&gstk_curr_inst_ptr->available_csg_list,
                              &cmd_ptr->cm_ph_ind.available_networks,
                              sizeof(sys_detailed_plmn_list_s_type),
                              sizeof(sys_detailed_plmn_list_s_type),
                              sizeof(sys_detailed_plmn_list_s_type));
            if(gstk_curr_inst_ptr->gstk_evt_list[GSTK_NW_SEARCH_MODE] & 0x01)
            {
              /* Build an envelope to send down the Access Technology change details to the card */
              gstk_status = gstk_build_nw_search_mode_change_envelope(cmd_ptr->cm_ph_ind.nw_search_mode);
              if(gstk_status != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("Err in building Network Search Mode Change Envelope");
              }
            }
            gstk_curr_inst_ptr->nw_search_mode = cmd_ptr->cm_ph_ind.nw_search_mode;

            local_info_ptr = (gstk_local_info_rsp_type *)GSTK_CALLOC(sizeof(gstk_local_info_rsp_type));

            if(local_info_ptr == NULL)
            {
              break;
            }

            local_info_ptr->info_type = GSTK_INVALID_LOC_INFO;
            if(command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].expected_cmd_rsp_type ==
                 GSTK_PROVIDE_NW_SEARCH_MODE_CNF)
            {
              local_info_ptr->info_type = GSTK_NETWORK_SEARCH_MODE_INFO;
              local_info_ptr->info_data.nw_search_mode = cmd_ptr->cm_ph_ind.nw_search_mode;
               gstk_send_provide_local_info_terminal_rsp(
                 command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].unique_ref_id,
                 command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].command_details.command_number,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result */
                 &additional_info, /* additional result */
                local_info_ptr ); /* local info union pointer */
            }
            if(command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].expected_cmd_rsp_type ==
                 GSTK_PROVIDE_CSG_INFO_CNF)
            {
              local_info_ptr->info_type = GSTK_CSG_CELL_INFO;
              (void)gstk_memcpy(&local_info_ptr->info_data.csg_info,
                                &gstk_curr_inst_ptr->available_csg_list,
                                sizeof(sys_detailed_plmn_list_s_type),
                                sizeof(sys_detailed_plmn_list_s_type),
                                sizeof(sys_detailed_plmn_list_s_type));
               gstk_send_provide_local_info_terminal_rsp(
                 command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].unique_ref_id,
                 command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].command_details.command_number,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result */
                 &additional_info, /* additional result */
                local_info_ptr ); /* local info union pointer */
            }

            gstk_free(local_info_ptr);
            local_info_ptr = NULL;
            break;

          case GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF:
            gstk_shared_data.active_data_subs = cmd_ptr->cm_ph_ind.active_data_subs;
            break;

          case GSTK_CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
            (void)gstk_memcpy(&gstk_curr_inst_ptr->available_csg_list,
                              &cmd_ptr->cm_ph_ind.available_networks,
                              sizeof(sys_detailed_plmn_list_s_type),
                              sizeof(sys_detailed_plmn_list_s_type),
                              sizeof(sys_detailed_plmn_list_s_type));
          default:
            /** Invalid CM_PH event type **/
            break;
        }
      }
      else
      {
        UIM_MSG_HIGH_1("GSTK_CM_PH_IND: CAT version is pre-VER6 : 0x%x",
                       cat_version);
      }
      break;

    case GSTK_RETRY_ENVELOPE_CMD:
      UIM_MSG_HIGH_0("GOT GSTK_RETRY_ENVELOPE_CMD");
      index = gstk_util_find_first_timer_expire_command(
                cmd_ptr->general_cmd.message_header.sim_slot_id);
      if (index == GSTK_MAX_TIMER_SUPPORTED)
      {
        UIM_MSG_HIGH_0("Invalide Timer Table index");
      }
      else
      {
        gstk_build_timer_expire_envelope(
          cmd_ptr->general_cmd.message_header.sim_slot_id,
          uint16touint8(index));
      }
      break;

    case GSTK_TIMER_FOR_ENV_RETRY_CMD:
      UIM_MSG_HIGH_1("GOT GSTK_TIMER_FOR_ENV_RETRY_CMD: gstk_env_state=0x%x",
                     gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state);
      /* retry the envelope */
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state ==
         GSTK_SUB_ST_PENDING_ENV_IN_QUEUE)
      {
        gstk_nv_items_data_type nv_data;

        (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,
                                gstk_curr_inst_ptr->slot_id,
                                &nv_data);

        if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_counter <
           nv_data.env_retry_ext.max_retries)
        {
          if(gstk_util_retry_envelope_cmd() != GSTK_SUCCESS)
          {
            /* build envelope response back to the client */
            gstk_status = gstk_build_error_envelope_report(
              gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id,
              int32touint32((int32)gstk_shared_data.envelope_cmd_ref_table[gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id].expected_cmd_rsp_type),
              GSTK_ENVELOPE_CMD_FAIL);
          }
        }
      }
      break;

    case GSTK_SETUP_OTASP_CALL_CMD:
      switch (cmd_ptr->general_cmd.message_header.command_id)
      {
        case GSTK_OTASP_CALL_ORIG:
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
          /* Since 1X activation is preferred, check if 1X RAT is available */
          if((gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_CDMA_MASK) != 0)
          {
            is_1x_available = TRUE;
          }

          /* If 1X RAT is not availale and we are not here due to an OTASP
            re-queue, start the 30 sec timer to wait for 1X to become
            available */
          if (!gstk_curr_inst_ptr->otasp_requeued &&
              is_1x_available == FALSE)
          {
            /* This is the original OTASP Call origination, if 1x is not yet
               available, device subs feature mode is SVLTE/SRLTE, and LTE is
               reported on CM stack 1 (that means UE mode is also SVLTE/SRLTE),
               set 1x timer */
            if((gstk_curr_inst_ptr->subs_feature_mode == GSTK_SUBS_FEATURE_MODE_NONE ||
                gstk_curr_inst_ptr->subs_feature_mode == GSTK_SUBS_FEATURE_MODE_SVLTE ||
                gstk_curr_inst_ptr->subs_feature_mode == GSTK_SUBS_FEATURE_MODE_SRLTE) &&
               gstk_curr_inst_ptr->mode_change_cmd_ptr->mode_change_ind.cm_stack[1].access_tech == GSTK_ACCESS_TECH_LTE)
            {
              UIM_MSG_HIGH_3("GSTK_OTASP_CALL_ORIG: 1x not yet available, LTE reported on 2nd stack. SUBS mode: 0x%x, set timer: 0x%x, stack 1 RAT: 0x%x",
                             gstk_curr_inst_ptr->subs_feature_mode,
                             GSTK_1X_AVAIL_WAIT_TIMER,
                             gstk_curr_inst_ptr->mode_change_cmd_ptr->mode_change_ind.cm_stack[1].access_tech);

              /* The VPE timer is reused for the 1x availability wait */
              (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
              (void) rex_set_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer),
                                   GSTK_1X_AVAIL_WAIT_TIMER);
               break;
            }
          }

          /* This is the OTASP requeue and 1x is still not available (meaning
             the timer has simply expired) OR the device is non-SVLTE. Go ahead
             with activation over BIP by processing the OPEN CH command */
          if (is_1x_available == FALSE)
          {
            UIM_MSG_HIGH_2("GSTK_OTASP_CALL_ORIG: SUBS mode: 0x%x, 1x available: 0x%x, do activation over BIP",
                           gstk_curr_inst_ptr->subs_feature_mode, is_1x_available);
            gstk_status = gstk_open_ch_req((gstk_proactive_cmd_type *)cmd_ptr->general_cmd.message_header.user_data);
            gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
            gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
            break;
          }
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */

          if(cmd_ptr->general_cmd.message_header.user_data != (uint32)gstk_curr_inst_ptr->open_ch_imsi_m_ptr)
          {
            gstk_status = GSTK_BAD_PARAM;
            break;
          }
          if (gstk_curr_inst_ptr->otasp_call_id < 0)
          {
            cm_num_s_type                   called_number;
            cm_num_s_type                   calling_number;
            cm_cdma_orig_params_s_type      orig_param;
            byte                            num_buff[]        = {'*', '2', '2', '8', '9', '9'};
            cm_call_id_type                 call_id           = 0;
            cm_orig_alpha_s_type            alpha_buff;
            cm_cc_generic_params_s_type cc_generic_params;

            if(gstk_shared_data.gstk_num_of_instances > 1)
            {
               slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
            }

            UIM_MSG_HIGH_0("Initiating OTASP call ...");
            cm_num_init(&calling_number);    /* Setup the numbers to defaults */
            cm_num_fill(&called_number,
                        num_buff,
                        sizeof(num_buff),
                        CM_DIGIT_MODE_4BIT_DTMF);
            called_number.number_plan = (uint8) CM_NUM_PLAN_ISDN;
            cm_util_default_cdma_orig_params(&orig_param);
            /* Fill OTASP activation code */
            orig_param.activate_code = CM_PREFERRED;
            memset(&alpha_buff, 0x00, sizeof(alpha_buff));

            if ( gstk_io_ctrl(
                             GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
                             &slot,
                             &as_id) != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("failed to get as_id for slot 0x%x!");
           }

            gstk_curr_inst_ptr->recv_otasp_commit = FALSE;
            cc_generic_params.is_fdn_to_check = FALSE;
            if (!cm_mm_call_cmd_orig_exten5_per_subs (
                     gstk_cm_cmd_cb,
                     gstk_curr_inst_ptr,
                     gstk_shared_data.gstk_client_id_from_cm,
                     CM_CALL_TYPE_VOICE,
                     CM_SRV_TYPE_AUTOMATIC,
                     &calling_number,
                     &called_number,
                     &alpha_buff,
                     &orig_param,
                     NULL,
                     NULL,
                     NULL,
                     CM_CALL_ID_INVALID,
                     &cc_generic_params,
                     CM_ORIG_CLIENT_CATEG_CARD,
                     TRUE,
                     as_id,
                     &call_id))
            {
              UIM_MSG_ERR_0("failed to start OTASP call!");
              if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_curr_inst_ptr->slot_id) == TRUE)
              {
                gstk_curr_inst_ptr->otasp_failure_cnt++;
                UIM_MSG_HIGH_1("Increment otasp_failure_cnt counter, new counter = %x",
                               gstk_curr_inst_ptr->otasp_failure_cnt);
                if(gstk_curr_inst_ptr->otasp_failure_cnt >= GSTK_MAX_OTASP_RETRY_CNT &&
                   gstk_shared_data.gstk_otasp_status_cb != NULL)
                {
                  (gstk_shared_data.gstk_otasp_status_cb)(gstk_curr_inst_ptr->slot_id, GSTK_OTASP_ACT_STATUS_PERM_FAIL);
                }                               
              }
            }
            else
            {
              UIM_MSG_HIGH_1("OTAS call id: 0x%x", call_id);
              gstk_curr_inst_ptr->otasp_call_id = call_id;
              if(gstk_curr_inst_ptr->otasp_failure_cnt == 0 &&
                 gstk_shared_data.gstk_otasp_status_cb != NULL)
              {
                (gstk_shared_data.gstk_otasp_status_cb)(gstk_curr_inst_ptr->slot_id, GSTK_OTASP_ACT_STATUS_START);         
              }
#ifdef FEATURE_GSTK_FUSION_REMOTE
              otasp_status_cmd.cmd_type = GSTK_SETUP_OTASP_CALL_CMD;
              otasp_status_cmd.user_data = 1;
              (void)gstk_tal_send_cmd(GSTK_TAL_REMOTE_CLIENT,
                                      &otasp_status_cmd);
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
            }
          }
          gstk_shared_data.is_open_ch_ok = FALSE;
          gstk_status = gstk_open_ch_req((gstk_proactive_cmd_type *)cmd_ptr->general_cmd.message_header.user_data);
          if (gstk_curr_inst_ptr->open_ch_imsi_m_ptr)
          {
            gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
            gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
          }
          break;

        case GSTK_OTASP_CALL_CMD_ERROR:
          UIM_MSG_ERR_1("OTASP call orig cmd failed! 0x%x",
                        cmd_ptr->general_cmd.message_header.user_data);
          if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_curr_inst_ptr->slot_id) == TRUE)
          {
            gstk_curr_inst_ptr->otasp_failure_cnt++;
            UIM_MSG_HIGH_1("Increment otasp_failure_cnt counter, new counter = %x",
                           gstk_curr_inst_ptr->otasp_failure_cnt);
            if(gstk_curr_inst_ptr->otasp_failure_cnt >= GSTK_MAX_OTASP_RETRY_CNT &&
               gstk_shared_data.gstk_otasp_status_cb != NULL)
            {
              (gstk_shared_data.gstk_otasp_status_cb)(gstk_curr_inst_ptr->slot_id, GSTK_OTASP_ACT_STATUS_PERM_FAIL);
            }                           
          }
          gstk_curr_inst_ptr->otasp_call_id = -1;
#ifdef FEATURE_GSTK_FUSION_REMOTE
          otasp_status_cmd.cmd_type = GSTK_SETUP_OTASP_CALL_CMD;
          otasp_status_cmd.user_data = 0;
          (void)gstk_tal_send_cmd(GSTK_TAL_REMOTE_CLIENT,
                                  &otasp_status_cmd);
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
          break;
        default:
          UIM_MSG_ERR_1("invalid OTASP call cmd 0x%x!",
                        cmd_ptr->general_cmd.message_header.command_id);
      }
      break;

    case GSTK_TIME_ZONE_CHANGE_IND:
      UIM_MSG_HIGH_0("GSTK_TIME_ZONE_CHANGE_IND");

      for (i = 0;
            i < GSTK_CM_NUM_STACKS &&
            cmd_ptr->time_zone_ind.cm_stack[i].tz_avail != GSTK_TZ_NO_CHANGE;
            ++i)
      {
        gstk_curr_inst_ptr->tz_info[i].access_tech =
          cmd_ptr->time_zone_ind.cm_stack[i].access_tech;
        gstk_curr_inst_ptr->tz_info[i].tz_avail =
          cmd_ptr->time_zone_ind.cm_stack[i].tz_avail;
        switch (cmd_ptr->time_zone_ind.cm_stack[i].access_tech)
        {
          case GSTK_ACCESS_TECH_GSM:
          case GSTK_ACCESS_TECH_UTRAN:
          case GSTK_ACCESS_TECH_LTE:
            gstk_curr_inst_ptr->tz_info[i].tz =
            cmd_ptr->time_zone_ind.cm_stack[i].tz;
            break;
          default:
            /* no action needed */
            break;
        }
      }
      break;

    case GSTK_CM_SUBS_CHANGE_IND:
      gstk_process_cm_subs_change_ind(cmd_ptr);
      break;

    case GSTK_WMS_EVT_IND:
      UIM_MSG_HIGH_0("GSTK_WMS_EVT_IND");
      slot = cmd_ptr->wms_evt_ind.message_header.sim_slot_id;
      switch (cmd_ptr->wms_evt_ind.wms_event)
      {
        case GSTK_WMS_MSG_EVENT_MT_MESSAGE_ERROR:
          if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type == GSTK_SMS_PP_DOWNLOAD_IND)
          {
            gstk_util_clear_retry_envelope_data(slot);
          }
          break;
        default:
          break;        
      }
      break;

    case GSTK_OTASP_REG_CMD:
      gstk_process_otasp_reg_cmd(cmd_ptr);
      break;
      
    default:
      UIM_MSG_HIGH_1("Cmd 0x%x Not handled in GSTK_IDLE_S",
                     cmd_ptr->general_cmd.message_header.command_group);
      gstk_util_send_err_to_client(cmd_ptr);
      break;
  }
  return gstk_status;
} /*gstk_state_idle*/

/*===========================================================================
FUNCTION gstk_check_cm_stacks_for_3gpp

DESCRIPTION
  This function finds the priority access tech based on location status and RAT

PARAMETER
  mode_change_ind_ptr : CM mode change command

DEPENDENCIES
  None

RETURN VALUE
  FALSE : When none of the stacks has 3GPP service
  TRUE  : Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_check_cm_stacks_for_3gpp(
  const gstk_mode_change_ind_type* mode_change_ind_ptr)
{
  int i = 0;

  if(mode_change_ind_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL mode_change_ind_ptr");
    return TRUE;
  }

  UIM_MSG_MED_0("gstk_check_cm_stacks_for_3gpp");

  for (i = 0; i < GSTK_CM_NUM_STACKS; ++i)
  {
    switch(mode_change_ind_ptr->cm_stack[i].access_tech)
    {
      case GSTK_ACCESS_TECH_GSM:
      case GSTK_ACCESS_TECH_UTRAN:
      case GSTK_ACCESS_TECH_LTE:
        /* Found a 3GPP RAT */
        UIM_MSG_MED_2("Found 3GPP RAT 0x%x on cm_stack[%d]",
                      mode_change_ind_ptr->cm_stack[i].access_tech, i);
        return TRUE;
      /* fall through */
      default:
        continue;
    }
  }

  /* No 3GPP RAT found */
  return FALSE;
} /* gstk_check_cm_stacks_for_3gpp */

/*===========================================================================
FUNCTION gstk_mode_ch_compare_func

DESCRIPTION
  This function is used to compare the requested command id with command id of
  the given MODE CHANGE command

PARAMETER
  item_ptr    : Pointer to GSTK command
  compare_val : command id if intrest

DEPENDENCIES
  None

RETURN VALUE
  FALSE : Mode change with command id equal to compare_val
  TRUE  : Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_mode_ch_compare_func(void* item_ptr, void* compare_val)
{
  gstk_task_cmd_type* task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  gstk_mode_change_cmd_id_enum_type* data = (gstk_mode_change_cmd_id_enum_type*)compare_val;

  if(task_cmd_ptr == NULL || data == NULL)
  {
    UIM_MSG_ERR_2("NULL ptr 0x%x 0x%x", task_cmd_ptr, compare_val);
    return 0;
  }

  if((task_cmd_ptr->cmd.general_cmd.message_header.command_group ==
      GSTK_MODE_CHANGE_IND) &&
     (task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id ==
      *data) &&
     (gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id) ==
      task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id))
  {
    UIM_MSG_HIGH_1("gstk_mode_ch_compare_func(): Found MODE CHANGE with command id: 0x%x",
                   *data);
    return 1;
  }

  return 0;
}
/*===========================================================================
FUNCTION gstk_check_access_tech_for_3gpp2

DESCRIPTION
  This function check for 3GPP2 access technology group

PARAMETER
  access_tech : Priority radio access technology

DEPENDENCIES
  None

RETURN VALUE
  TRUE  : When access_tech is one under 3GPP2 group
  FALSE : Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_check_access_tech_for_3gpp2(
  const  gstk_access_technology_type    access_tech)
{
  switch(access_tech)
  {
    case GSTK_ACCESS_TECH_CDMA:
    case GSTK_ACCESS_TECH_HRPD:
    case GSTK_ACCESS_TECH_EHRPD:
      /* Found a 3GPP2 RAT */
      UIM_MSG_MED_1("3GPP2 Access Tech is  0x%x ",access_tech);
      return TRUE;
	default:
	  return FALSE;
  }
  /* No 3GPP2 RAT found */
  return FALSE;
} /* gstk_check_access_tech_for_3gpp2 */

/*===========================================================================
FUNCTION gstk_check_access_tech_for_3gpp

DESCRIPTION
  This function check for 3GPP access technology group

PARAMETER
  access_tech : Priority radio access technology

DEPENDENCIES
  None

RETURN VALUE
  TRUE  : When access_tech is one under 3GPP group
  FALSE : Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_check_access_tech_for_3gpp(
  const  gstk_access_technology_type    access_tech)
{
  switch(access_tech)
  {
    case GSTK_ACCESS_TECH_GSM:
    case GSTK_ACCESS_TECH_UTRAN:
    case GSTK_ACCESS_TECH_LTE:
      /* Found a 3GPP RAT */
      UIM_MSG_MED_1("3GPP Access Tech is  0x%x ",access_tech);
      return TRUE;
	default:
	  return FALSE;
  }
  /* No 3GPP RAT found */
  return FALSE;
} /* gstk_check_access_tech_for_3gpp */

/*===========================================================================
FUNCTION gstk_check_send_tp_when_mode_ch

DESCRIPTION
  This function determine if need to send tp to card when receive mode
  change.

PARAMETER
  access_tech : Priority radio access technology
  slot        : Slot id of the access_tech

DEPENDENCIES
  None

RETURN VALUE
  TRUE : When need to send tp to card
  FALSE: Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_check_send_tp_when_mode_ch(
  gstk_access_technology_type     access_tech,
  gstk_slot_id_enum_type          slot)
{
  UIM_MSG_HIGH_3("In gstk_check_send_tp_when_mode_ch,  slot id is 0x%x, access_tech is 0x%x, gstk_curr_tech_group is 0x%x ",
                                                 slot,
                                                 access_tech,
                                                 gstk_curr_inst_ptr->gstk_curr_tech_group);
  if( slot == GSTK_SLOT_1
     &&(gstk_curr_inst_ptr->card_protocol == MMGSDI_ICC)
     &&((gstk_check_access_tech_for_3gpp2(access_tech)
     && gstk_curr_inst_ptr->gstk_curr_tech_group != GSTK_ACCESS_TECH_3GPP2)
     || (gstk_check_access_tech_for_3gpp(access_tech)
     && gstk_curr_inst_ptr->gstk_curr_tech_group != GSTK_ACCESS_TECH_3GPP))
     )
   {
     return TRUE;
   }
   return FALSE;
} /* gstk_check_send_tp_when_mode_ch */

/*===========================================================================
FUNCTION gstk_find_normal_service_rat_when_mode_ch

DESCRIPTION
  This function normal service rat when receive mode
  change.

PARAMETER
  access_tech     : Priority radio access technology
  mode_change_ind : CM mode change command

DEPENDENCIES
  None

RETURN VALUE
  GSTK_NULL_INPUT_PARAM : When one of the inputs is NULL
  GSTK_SUCCESS          : Otherwise
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_find_normal_service_rat_when_mode_ch(
  gstk_access_technology_type*     access_tech,
  const gstk_mode_change_ind_type* mode_change_ind)
{
  int i = 0;

  UIM_MSG_HIGH_0("In gstk_find_normal_service_rat_when_mode_ch()");

  if(access_tech == NULL  || mode_change_ind == NULL)
  {
    UIM_MSG_HIGH_2("NULL Input Param 0x%x  0x%x",
                   access_tech,  mode_change_ind);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* initialize the input variables to default values */
  *access_tech = GSTK_ACCESS_NONE;

  for (i = 0; i < GSTK_CM_NUM_STACKS; i++)
  {
    if(mode_change_ind->cm_stack[i].loc_status == GSTK_NORMAL_SERVICE)
    {
      *access_tech = mode_change_ind->cm_stack[i].access_tech;
      UIM_MSG_HIGH_1("Find normal service access tech 0x%x",*access_tech);
      break;
    }
  }

  return GSTK_SUCCESS;
} /* gstk_find_normal_service_rat_when_mode_ch */
