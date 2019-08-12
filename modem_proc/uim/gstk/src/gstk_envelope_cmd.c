/*===========================================================================


            G S T K   E N V E L O P E   C M D

GENERAL DESCRIPTION
  This source file contains functions to handle envelope commands (except
  for the event download commands which are handled in gstk_evt_dl.c


FUNCTIONS
  gstk_menu_selection_ind
    This function builds the menu selection command and sends it to UIM

  gstk_sms_pp_dl_ind
    This function builds the sms pp download command and sends it to UIM

  gstk_cell_broadcast_dl_ind
    This function builds the cell broadcast download command and sends it
    to UIM

  gstk_cc_ind
    This function builds the call control command and sends it to UIM

  gstk_mo_sms_ctrl_ind
    This function builds the MO SMS control command and sends it to UIM

  gstk_timer_expire_ind
    This function builds the timer expiration command and sends it to UIM

  gstk_process_envelope_cmd
    This function switches on the command id and call the corresponding
    envelope command ind function so that GSTK can build the corrresponding
    envelope command tlvs and send it to UIM Server

  gstk_raw_envelope_ind_cleanup
    This function cleans up any allocated pointer on the heap.

  gstk_raw_envelope_ind
    This function packages the raw envelope command and sends it to UIM

  gstk_location_information_ind
    This function packages the location information envelope command and
    sends it to uim

  gstk_build_csg_cell_selection_envelope
    This function queues CSG cell selection envelope command to GSTK task queue

INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_envelope_cmd.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
06/20/16   vr      Set gstk_bip_env_in_progress before de-queue of bip env cmd
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
04/19/16   vr      Send Cached TR only if the delay TR timer is running
01/08/16   vr      Reset usr_acty and idl_scrn for raw envelope rsp evt dl
11/23/15   gs      F3 reduction for the envelope data
10/12/15   vdc     Added support for informing MMGSDI about BIP and SMS-PP
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
01/27/15   gm      Retry call control after recovery
11/11/14   shr     Fixed compilation warnings
10/28/14   gs      Removed support for GSTK_ACCESS_NONE in gstk_cc_ind
10/08/14   bd      Changes to control eHRPD loc info feature with NV
09/30/14   gs      Use card_protocol to determine length of cell id
09/29/14   sw      Support RAW IMS Registration event download
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/19/14   gm      Added support for SMS_PP data download retry
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/14/14   gm      Free memory allocated for sending command to UIM
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
07/31/14   vr      Clear the envelope ref table after sending attch ctrl rsp
07/21/14   vr      Clear cached envelope info in non-idle state
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
07/11/14   bd      Enable retry for call control envelopes by default
06/25/14   vr      Set the user data with unique ref id in cached TR handling
06/03/14   sw      Implemented retry for call-control envelopes
06/02/14   dy      Fixing incorrect cell_id being copied for CC and
                   MO SMS control
05/28/14   gm      Support of CSG Rel-11 feature
05/27/14   gm      Ignoring location info in call control when in WLAN rat
05/08/14   gm      Updating gstk_instances_ptr to GSTK context
05/06/14   gs      GSTK Featurization enhancements and clean up
04/24/14   hn      Add support in GSTK to add location information to Voice
                   call control and MO SMS control envelopes
04/21/14   vr      Fix Location info is copied twice for MO SMS call control
04/07/14   shr     Use non-none RAT passed in by Call Control Client
04/03/14   dy      Replace gstk_malloc() with macro
04/03/14   dy      Fix int32 copying into a 3 byte buffer in
                   gstk_copy_1x_location_info()
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   gm      Added support for IMS communication control
02/26/14   sw      Memory leak fixes
03/04/14   shr     Fix Subscription Ready caching logic
02/11/14   gm      Fix KW critical warning
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/02/13   gm      Remove Lint error
11/28/13   gs      Remove tmc.h inclusion
11/19/13   vr      Fix for UIMDRV Memory De-Couple changes for delay TR
10/04/13   gs      F3 Message reduction
09/25/13   jd      Fix issue of dup loc status env
09/23/13   jd      Fix Log message w.r.t. Access Tech envelope
09/06/13   gm      UIMDRV Memory De-Couple changes
09/13/13   gm      Support for Recovery and hot-swap
08/12/13   sw      Corrected logging of number of envelope bytes
08/06/13   vr      Centralized Call Control Changes
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
06/24/13   xz      Support IMS Registration event download
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
06/19/13   hh      Correctly handle GSTK_LOCATION_INFORMATION_IND in all GSTK states
06/12/13   gm      Setting GSTK_ESTK_CMD_SIG everytime when command is posted
                   to GSTK task
06/15/13   bd      Fixed MO SMS ctrl failure due to LAC change while in a call
05/23/13   hh      Fix KW errors
05/16/13   xz      Fix the issue of MO SMS ctrl failure
05/16/13   hh      Fix KW errors
05/01/13   vr      Fix command processing logic in GSTK
04/11/13   bd      Avoid notifying NAS to ATTACH before IMSI switch
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/25/13   xz      Process attach ctrl req when envelope is in retry
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/14/13   gm      Fix to print call control envelope command buffer completely
02/11/13   bd      Support for (e)HRPD access tech
01/31/13   gm      Fix to store attach control pointer
01/08/13   hn      Fix critical Klocwork warning
11/13/12   bd      Support for Closed Subscriber Group feature
11/21/12   hk      Removing check for gstk_next_curr_location_status to send
                   call control envelope
11/01/12   hk      Allow attach when subs ready received as result of refresh
10/30/12   hn      Send error report to clients when envelope queue is full
10/25/12   hn      Process Call Connected Evt later when there is Set Up Call pending
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
09/25/12   hk      In NO SIM state there are envelopes queued for gstk cell broadcast
                   in no sim state
08/08/12   sg      Set ESTK sig to expedite processing of subsequent GSTK commands
04/25/12   bd      Clean up FEATURE_CAT_REL6 which was moved to NV
03/09/12   bd      Enhancements for location information in CC
03/28/12   av      Replaced feature flags with NVITEM
03/05/12   dd      Fix integer overflow issue
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/04/12   xz      Fix loc infor issue of CC in LTE <-> eHRPD only
12/20/11   nb      GSDI Cleanup
12/08/11   xz      Fix loc infor issue of CC in LTE <-> eHRPD
12/05/11   xz      Fix the issue of loc info in CC envelope cmd
10/31/11   sg      Class L and M support
09/27/11   js      Do not reject CC envelope if GSTK does not have RAT info
09/12/11   js      Add support of CDMA CC envelope
08/24/11   xz      Fix compiler warnings and LINT/KW errors
07/06/11   sg      Fix for not printing in char format for EOF character
06/28/11   js      Fix for potential buffer overflow
05/31/11   js      Fix for potential buffer overflow.
05/14/11   nb      ThreadSafe - Removed Usage of gstk_util_save_env_cmd_info()
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
05/11/11   xz      Fix for Location Envelope
05/04/11   dd      Fix SMS PP DL crash
04/29/11   nk      Fix KW error
04/11/11   js      Fix for envelope memcpy in TAL
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/23/11   sg      Store Location Information envelope for retry
02/03/11   bd      Fixed buffer over flow issue in SMS-PP DL
01/28/10   bd      Changes to use slot specific API
12/30/10   ssr     Dynamically allocate command buffer
12/17/10   ps      UIM memory reduction changes
11/30/10   dd      Fixing read overflow
11/29/10   tkl     Removed RRC private header file
11/22/10   js      Fix for LTE PDN call control
10/20/10   xz      Fix the issue of sending LTE access tech event
10/20/10   xz      Fix the issue of not send TR in eHRPD only mode
08/26/10   xz      Add support of EPS Call Control
08/18/10   dd      Fix KW error
06/21/10   xz      KW fixes
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
06/18/10   sg      Wrong source device id for location info envelope
05/28/10   yt      Lint fixes
05/21/10   xz      Add support of network rejection event
05/17/10   bd      Fix to send location status envelope when UE goes to no service
03/08/10   sg      Location information envelope command
02/26/10   yt      Fixed KW errors
02/15/10   nb      Moved Dual Slot Changes
09/26/09   kp      ZI memory reduction changes
02/03/09   gg      Fixed lint error
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
10/07/08   xz      Fixed Klockwork errors
09/14/08   sk      Fixed lint errors
08/08/08   xz      Added raw envelope command
05/06/08   sk      Added support for event download envelope retry
03/17/08   sk      Fixed klockwork errors
08/01/07   sk      Fixed lint error
07/23/07   sk      Fixed warning
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
07/06/07   sk      Fixed typo
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
12/14/06   sk      Lint fixes
10/23/06   sk      Lint fixes
07/26/06   nk      Added check to prevent buffer overflow
07/25/06   sk      Added support to handle envelope cmds in non idle state
06/07/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mode
05/23/06   tml     GPRS Call Control support and lint
05/18/06   nk      Fixed High Lint Error:  Status defined but not used for
                   CDMA Targets.
05/18/06   sk      Fixed warnings for 1X builds
05/17/06   sk      Lint fixes
04/06/06   sk      Support for informing clients of envelope failure.
04/04/06   sk      Added new function to build location status envelope
03/29/06  sk/tml   Allow all GSM cell ID to go through for Call Control and MO
                   SMS Control
03/13/06   sk      Fixed typo error in gstk_cc_ind()
03/06/06   sk      Fixed location issue for call control and mo sms control
                   location object - obtain cell id from rrc loci table
                   for UTRAN network, mnc mcc lac info from the gstk cache.
                   Added Lint Fixes
11/09/05   sk      Fixed lint errors
09/15/05   tml     Fixed link error
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/14/05   sk      Reomoved warnings
07/13/05   sk      Added support for FEATURE_STOP_ENV_CMD_PROCESSING
                   Added new function gstk_build_access_technology_env()
05/11/05   sst     Lint fixes
11/23/04    sk     Added SAT/USAT data support
08/19/04   tml     Added cell broadcast support
08/10/04   tml     Added gstk_build_postpone_envelope
05/26/04   tml     Added More debug messages
                   Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/02/04   tml     linted
02/24/04   tml     compilation fix for CDMA only target
09/04/03   tml     Dual slot support
07/25/03   tml     ensured only valid address is being passed to the Card in
                   MO SMS Control
06/23/03   tml     removed compiler error for 1x/gsm
05/21/03   tml     linted
04/21/03   tml     Added is_cdma_sms boolean support
04/15/03   tml     Fixed MOSMS control envelope tag
03/20/03   tml     Properly featurize local info for WCDMA and GSM.
                   Fixed offset for envelope responses
03/06/03   tml     Updated enum names, Added FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include <stringl/stringl.h>
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "uim.h"
#include "gstkutil.h"
#include "string.h"
#include "err.h"
#include "gstkutil.h"
#include "uim_msg.h"
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#include "mm.h"
#ifdef FEATURE_GSM
#include "rr_usat_if.h"
#endif /*FEATURE_GSM */
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/

#define GSTK_EVT_LIST_VAL_LEN 1

void gstk_send_cached_terminal_rsp(unsigned long instance);
gstk_status_enum_type gstk_find_cached_cell_id (const uint8        *plmn_ptr,
                                                const uint8        *lac_ptr,
                                                gstk_cell_id_type  *cell_info_ptr);

extern void gstk_uim_terminal_response_report(uim_rpt_type *report);

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  cb_dl_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_cell_broadcast_dl_ind_cleanup(
    gstk_envelope_cell_broadcast_download_ind_type*     cb_dl_cmd,
    gstk_envelope_cell_broadcast_download_command_type* STK_envelope_cmd)
{

  if(STK_envelope_cmd == NULL)
  {
    UIM_MSG_ERR_0("STK_envelope cmd ERR : NULL");
    return;
  }
  /* free envelope page dataind and tlv */
  /* free page tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)cb_dl_cmd));
  if(STK_envelope_cmd->cb_page.cb_page != NULL) {
    gstk_free(STK_envelope_cmd->cb_page.cb_page);
    STK_envelope_cmd->cb_page.cb_page = NULL;
  }
} /* gstk_cell_broadcast_dl_ind_cleanup */


/*===========================================================================
FUNCTION gstk_raw_envelope_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the heap.

PARAMETERS
  raw_envelope_ptr: [Input] Pointer to message that is required to
                            be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_raw_envelope_ind_cleanup(
  gstk_raw_envelope_ind_type* raw_envelope_ptr)
{
  /* free envelope streaming ind and tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)((void*)raw_envelope_ptr));
} /* gstk_raw_envelope_ind_cleanup */


/*===========================================================================
FUNCTION gstk_sms_pp_dl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  sms_pp_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_sms_pp_dl_ind_cleanup(
    gstk_envelope_sms_pp_download_ind_type*           sms_pp_cmd)
{
  /* free envelope sms tpdu ind and tlv */
  /* tpdu tlv is [] so no need to free */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)sms_pp_cmd));
} /* gstk_sms_pp_dl_ind_cleanup */ /*lint !e715 */

/*===========================================================================
FUNCTION gstk_cc_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  cc_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_cc_ind_cleanup(
    gstk_envelope_cc_ind_type*     cc_cmd,
    gstk_envelope_cc_command_type* STK_envelope_cmd)
{
  gstk_access_technology_type rat = GSTK_ACCESS_NONE;

  if(STK_envelope_cmd == NULL || cc_cmd == NULL)
  {
    UIM_MSG_ERR_2("NULL: STK_envelope_cmd 0x%x, cc_cmd 0x%x",
                  STK_envelope_cmd, cc_cmd);
    return;
  }
  /* free envelope cc ind and tlv */
  if(cc_cmd->cc_data.call_enum_type == GSTK_USSD) {
    if(STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text != NULL) {
      gstk_free(STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text);
      STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text = NULL;
    }
  }

  rat = cc_cmd->cc_data.rat;
  if(rat == GSTK_ACCESS_NONE)
  {
    rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
  }
  if(rat == GSTK_ACCESS_TECH_CDMA ||
     rat == GSTK_ACCESS_TECH_HRPD ||
     rat == GSTK_ACCESS_TECH_EHRPD)
  {
    gstk_util_location_info_tlv_cleanup(&STK_envelope_cmd->loc_info.cdma_ext_loc_info);
  }

  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)cc_cmd));
} /* gstk_cc_ind_cleanup */


/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  mo_sms_ctrl_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_mo_sms_ctrl_ind_cleanup(
    gstk_envelope_mo_sms_ctrl_ind_type*           mo_sms_ctrl_cmd,
    gstk_envelope_mo_sms_ctrl_command_type*       STK_envelope_cmd)
{
  gstk_access_technology_type rat = GSTK_ACCESS_NONE;

  if(STK_envelope_cmd == NULL || mo_sms_ctrl_cmd == NULL)
  {
    UIM_MSG_ERR_2("NULL: STK_envelope_cmd 0x%x, cc_cmd 0x%x",
                  STK_envelope_cmd, mo_sms_ctrl_cmd);
    return;
  }

  rat = mo_sms_ctrl_cmd->mo_sms_ctrl_data.rat;
  if(rat == GSTK_ACCESS_NONE)
  {
    rat = gstk_curr_inst_ptr->gstk_next_curr_rat;
  }
  if(rat == GSTK_ACCESS_TECH_CDMA ||
     rat == GSTK_ACCESS_TECH_HRPD ||
     rat == GSTK_ACCESS_TECH_EHRPD)
  {
    gstk_util_location_info_tlv_cleanup(&STK_envelope_cmd->loc_info.cdma_ext_loc_info);
  }

  /* free envelope mo sms ctrl ind and tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)mo_sms_ctrl_cmd));
} /* gstk_mo_sms_ctrl_ind_cleanup */ /*lint !e715 */


/*===========================================================================
FUNCTION gstk_location_information_ind

DESCRIPTION

  This function builds the location information command and sends it to UIM

PARAMETERS
  local_info_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_location_information_ind(
  const gstk_envelope_location_information_ind_type * loc_info_cmd)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  gstk_envelope_location_information_command_type   STK_envelope_cmd;
  uim_cmd_type                                *uim_cmd_ptr;
  int                                         offset      = 0;
  uint32                                      cmd_ref_id  = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(loc_info_cmd == NULL)
  {
    UIM_MSG_ERR_0("loc_info_cmd ERR : NULL");
    return GSTK_BAD_PARAM;
  }

  gstk_curr_inst_ptr->cdma_loc_st_disabled = !(gstk_util_is_vzw_mcc(loc_info_cmd->mcc_mnc));

  if(gstk_curr_inst_ptr->is_first_loc_info_envelope_sent == TRUE
     &&
     memcmp(loc_info_cmd->mcc_mnc,
            gstk_curr_inst_ptr->gstk_last_loc_info_sent,
            sizeof(loc_info_cmd->mcc_mnc)) == 0)
  {
    UIM_MSG_HIGH_3("dup VPE req: 0x%x, 0x%x, 0x%x",
                   loc_info_cmd->mcc_mnc[0],
                   loc_info_cmd->mcc_mnc[1],
                   loc_info_cmd->mcc_mnc[2]);
    gstk_util_env_cmd_ref_table_free_slot(
      loc_info_cmd->message_header.user_data);
    return GSTK_SUCCESS;
  }

  /* initialize STK_envelope_cmd */
  memset(&STK_envelope_cmd, 0,
         sizeof(gstk_envelope_location_information_command_type));

  /* Pack various TLVs */
  STK_envelope_cmd.tag = GSTK_LOC_INFO_ENVELOPE_TAG;

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
    GSTK_ME_DEVICE,
    GSTK_UICC_SIM_DEVICE,
    &STK_envelope_cmd.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    return gstk_status;
  }

  /* Location info */
  gstk_status = gstk_packer_loc_info_tlv(&STK_envelope_cmd.loc_info,
                                         loc_info_cmd->mcc_mnc);

  if(gstk_status != GSTK_SUCCESS) {
     return gstk_status;
  }

  /* total data length: device id data + 2 (2 is for the tag and length field)
                       + location info data + 2
  */
  STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                            STK_envelope_cmd.loc_info.location_info_length +
                            4;
  /* Send command to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F,
                                                      &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
        /* required so that uim will grap the data from index 0 */
        uim_cmd_ptr->envelope.offset = 0;

        /* set user_data */
        uim_cmd_ptr->hdr.user_data = loc_info_cmd->message_header.user_data;

        /* populate the envelope command info */
        uim_cmd_ptr->envelope.num_bytes =
          STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

        /* copy device */
        /*STK_envelope_cmd.device_id.device_tag_length fixed in packer
        function */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                   &STK_envelope_cmd.device_id,
                                   offset,
                                   (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                            GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->envelope.data),
                                   sizeof(gstk_device_identities_tag_tlv_type)) <
           (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy local information */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                        &STK_envelope_cmd.loc_info,
                                        offset +=
                                        (int)(STK_envelope_cmd.device_id.device_tag_length +
                                              GSTK_TAG_LENGTH_LEN),
                                        (size_t)(STK_envelope_cmd.loc_info.location_info_length +
                                                 GSTK_TAG_LENGTH_LEN),
                                        sizeof(uim_cmd_ptr->envelope.data),
                                        sizeof(gstk_location_information_tag_tlv_type)) <
                (size_t)(STK_envelope_cmd.loc_info.location_info_length + GSTK_TAG_LENGTH_LEN))
        {
          UIM_MSG_ERR_0("gstk_location_information_ind(): gstk_byte_offset_memcpy failed");
          gstk_status = GSTK_MEMORY_ERROR;
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          offset = (int)(offset + STK_envelope_cmd.loc_info.location_info_length +
                       GSTK_TAG_LENGTH_LEN);

          gstk_util_dump_byte_array("Env: Location Info Data",
                              &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                              uim_cmd_ptr->envelope.num_bytes);

        /* Only if envelope state is GSTK_NONE, change state to RSP_FROM_CARD
                  and store the env payload */                      
        if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
        {
          if (gstk_nv_get_feature_status(
                GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                gstk_curr_inst_ptr->slot_id) == TRUE)
          {
            /* Cancel any envelope retry in progress, Location Info or Location Status */
            gstk_util_clear_retry_envelope_data(loc_info_cmd->message_header.sim_slot_id);
          }

          gstk_status = gstk_process_and_store_env_cmd_for_retry(
                          uim_cmd_ptr->envelope.data,
                          (uint32)uim_cmd_ptr->envelope.num_bytes,
                          uim_cmd_ptr->hdr.user_data);
          if( gstk_status == GSTK_SUCCESS)
          {
            gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state =
              GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
          }
          else
          {
            UIM_MSG_HIGH_0("FAIL - no retry will be attempted ");
            gstk_status = GSTK_SUCCESS;
          }
        }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
        else
        {
          UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
        }

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear sig,call uim_cmd */
      }
      else
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  if(gstk_status == GSTK_SUCCESS)
  {
    (void)gstk_memcpy(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
                   loc_info_cmd->mcc_mnc,
                   sizeof(loc_info_cmd->mcc_mnc),
                   sizeof(loc_info_cmd->mcc_mnc),
                   sizeof(loc_info_cmd->mcc_mnc));
    gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_INVALID;
  }

  cmd_ref_id = gstk_curr_inst_ptr->in_prog_cmd_ref_id;
  if (gstk_status == GSTK_SUCCESS
      &&
      (gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         gstk_curr_inst_ptr->slot_id) == TRUE))
  {
    gstk_util_remove_pending_env(GSTK_LOCATION_INFORMATION_IND);
    if (gstk_curr_inst_ptr->is_first_loc_info_envelope_sent
        &&
        GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) < GSTK_MAX_PROACTIVE_COMMAND
        &&
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id)].command_details.type_of_command ==
          GSTK_CMD_STK_REFRESH)
    {
      /* For same PLMN, VPE is always sent before LOC STATUS.
       * That's why we can safely remove any pending LOC STATUS.
       * And we should only do this for REFRESH
       */
      gstk_util_remove_pending_env(GSTK_LOCATION_STATUS_EVT_IND);
      gstk_util_remove_pending_env(GSTK_ACCESS_TECH_EVT_IND);
    }
  }

  /* nothing to free for local information envelope */
  return gstk_status;
} /* gstk_location_information_ind */

/*===========================================================================
FUNCTION gstk_menu_selection_ind

DESCRIPTION

  This function builds the menu selection command and sends it to UIM

PARAMETERS
  menu_sel_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_menu_selection_ind(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_menu_selection_command_type   STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;

   if(menu_sel_cmd == NULL)
   {
     UIM_MSG_ERR_0("menu_sel_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_menu_selection_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_MENU_SELECTION_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_KEYPAD_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* item ID tag */
   gstk_status = gstk_packer_item_id_tlv(
       menu_sel_cmd->chosen_item_id,
       &STK_envelope_cmd.item_chosen);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* help request tag */
   if(menu_sel_cmd->request_help) { /* user requested help */
     gstk_status = gstk_packer_help_request_tlv(
         menu_sel_cmd->request_help,
         &STK_envelope_cmd.request_help);

     if (gstk_status != GSTK_SUCCESS) {
         return gstk_status;
     }
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + item chosen data + 2
                        + request help data + 2
   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.item_chosen.length +
                             4;
   if(menu_sel_cmd->request_help) { /* user requested help */
     STK_envelope_cmd.length = STK_envelope_cmd.length +
                               STK_envelope_cmd.request_help.length +
                               2;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
         /* required so that uim will grap the data from index 0 */
         uim_cmd_ptr->envelope.offset = 0;

         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = menu_sel_cmd->message_header.user_data;

         /* populate the envelope command info */
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

         /* copy device */
         /*STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &STK_envelope_cmd.device_id,
                                    offset,
                                    (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(gstk_device_identities_tag_tlv_type)) <
            (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
         {
           gstk_status = GSTK_MEMORY_ERROR;
         }
         /* copy item id */
         /* STK_envelope_cmd.item_chosen.length fixed in packer function */
         else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                         &STK_envelope_cmd.item_chosen,
                                         offset +=
                                         (int)(STK_envelope_cmd.device_id.device_tag_length +
                                               GSTK_TAG_LENGTH_LEN),
                                         (size_t)(STK_envelope_cmd.item_chosen.length +
                                                  GSTK_TAG_LENGTH_LEN),
                                         sizeof(uim_cmd_ptr->envelope.data),
                                         sizeof(gstk_item_identifier_tlv_type)) <
                 (size_t)(STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN))
         {
           gstk_status = GSTK_MEMORY_ERROR;
         }

         offset = (int)(offset + STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN);

         /* copy help requested */
         /* STK_envelope_cmd.request_help.length fixed in packer function */
         if(gstk_status == GSTK_SUCCESS &&
            offset < (STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN) ) { /* has help */

           if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                      &STK_envelope_cmd.request_help,
                                      offset,
                                      (size_t)(STK_envelope_cmd.request_help.length +
                                               GSTK_TAG_LENGTH_LEN),
                                      sizeof(uim_cmd_ptr->envelope.data),
                                      sizeof(gstk_help_request_tlv_type)) <
              (size_t)(STK_envelope_cmd.request_help.length + GSTK_TAG_LENGTH_LEN))
            {
              gstk_status = GSTK_MEMORY_ERROR;
            }
         }

         if (gstk_status == GSTK_SUCCESS)
         {
           gstk_util_dump_byte_array("Env: Menu Sel Data",
                  &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                  uim_cmd_ptr->envelope.num_bytes);
           gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
         }
         else
     {
       gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
       uim_cmd_ptr = NULL;
     }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   /* nothing to free for menu selection ind */
   return gstk_status;

} /* gstk_menu_selection_ind */


/*===========================================================================
FUNCTION gstk_raw_envelope_ind

DESCRIPTION
  This function packages the raw envelope command and sends it to UIM

PARAMETERS
  raw_env_cmd_ptr: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
  GSTK_BAD_PARAM,
  GSTK_MEMORY_ERROR,
  GSTK_SUCCESS

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_raw_envelope_ind(
  const gstk_raw_envelope_ind_type * raw_env_cmd_ptr)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  uim_cmd_type                                *uim_cmd_ptr= NULL;
  int                                         offset = 0;
  int                                         event_list_len = 0;
  uint8                                       event_list_type = 0;

  if(raw_env_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("raw_env_cmd_ptr ERR : NULL");
    return GSTK_BAD_PARAM;
  }

  if (raw_env_cmd_ptr->env_len == 0
      || raw_env_cmd_ptr->env_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("empty raw envelope cmd!");
    return GSTK_BAD_PARAM;
  }

  /* Check the raw envelope type and whether card has registered for such Event */
  /* Download envelope. If card has not registered for such envelope, drop it.  */
  /*    0xD6 = GSTK_EVENT_DOWNLOAD_TAG                                          */
  if (raw_env_cmd_ptr->env_data_ptr[0] == GSTK_EVENT_DOWNLOAD_TAG)
  {
    /* offset for Event List parameter within Event Download envelope, TS 31.111, section 8.25 */
    if (raw_env_cmd_ptr->env_len > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
    {
      offset = 3;
    }
    else
    {
      offset = 2;
    }

    /* Bitmask AND with 0x7F to account for Event List tag with comprehension bit */
    if ((raw_env_cmd_ptr->env_data_ptr[offset] & 0x7F) != GSTK_EVENT_LIST_TAG)
    {
      UIM_MSG_ERR_0("gstk_raw_envelope_ind(): missing mandatory param Event List");
      return GSTK_BAD_PARAM;
    }

    offset++;
    if(offset >= raw_env_cmd_ptr->env_len || offset >= UIM_MAX_ENVELOPE_BYTES)
    {
      UIM_MSG_ERR_0("gstk_raw_envelope_ind(): offset out of bound.");
      return GSTK_BAD_PARAM;
    }

    event_list_len = raw_env_cmd_ptr->env_data_ptr[offset];
    if(event_list_len != 1)
    {
      UIM_MSG_ERR_0("gstk_raw_envelope_ind(): Event List length is greater than 1.");
      return GSTK_BAD_PARAM;
    }

    /* Check each event list type in the raw envelope against Set Up Event List cache */
    offset++;
    if(offset >= raw_env_cmd_ptr->env_len || offset >= UIM_MAX_ENVELOPE_BYTES)
    {
      UIM_MSG_ERR_0("gstk_raw_envelope_ind(): offset out of bound.");
      return GSTK_BAD_PARAM;
    }

    event_list_type = raw_env_cmd_ptr->env_data_ptr[offset];
    if (!(gstk_curr_inst_ptr->gstk_evt_list[event_list_type] & 0x01))
    {
      UIM_MSG_ERR_1("gstk_raw_envelope_ind(): card did not register for Event Download envelope type 0x%x", event_list_type);
      return GSTK_UNSUPPORTED_COMMAND;
    }

    /* Cache the event list type */
    if(raw_env_cmd_ptr->message_header.user_data <= GSTK_MAX_PROACTIVE_COMMAND)
    {
      gstk_shared_data.envelope_cmd_ref_table[raw_env_cmd_ptr->message_header.user_data].evt_list_code = event_list_type;
    }
  }

  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    if (raw_env_cmd_ptr->env_len > sizeof(uim_cmd_ptr->envelope.data))
    {
      UIM_MSG_ERR_0("raw_env_cmd_ptr->env_len is too big!");
      gstk_raw_envelope_ind_cleanup(
        (gstk_raw_envelope_ind_type*)raw_env_cmd_ptr);
      gstk_free(uim_cmd_ptr);
      return GSTK_BAD_PARAM;
    }

    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
    {
      /* required so that uim will grap the data from index 0 */
      uim_cmd_ptr->envelope.offset = 0;

      /* set user_data */
      uim_cmd_ptr->hdr.user_data = raw_env_cmd_ptr->message_header.user_data;

      /* populate the envelope command info */
      uim_cmd_ptr->envelope.num_bytes = uint32touint8(raw_env_cmd_ptr->env_len);

      if(gstk_memcpy(uim_cmd_ptr->envelope.data,
                     raw_env_cmd_ptr->env_data_ptr,
                     raw_env_cmd_ptr->env_len,
                     uim_cmd_ptr->envelope.num_bytes,
                     uim_cmd_ptr->envelope.num_bytes) <
         raw_env_cmd_ptr->env_len)
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }

      if (gstk_status == GSTK_SUCCESS)
      {
        gstk_util_dump_byte_array(
          "Env: Raw Data",
          &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
          uim_cmd_ptr->envelope.num_bytes);

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
    }
  } /* uim_cmd_ptr != NULL */
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_raw_envelope_ind_cleanup(
    (gstk_raw_envelope_ind_type*)raw_env_cmd_ptr);

  return gstk_status;
} /* gstk_raw_envelope_ind */


/*===========================================================================
FUNCTION gstk_sms_pp_dl_ind

DESCRIPTION

  This function builds the sms pp download command and sends it to UIM

PARAMETERS
  sms_pp_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_sms_pp_dl_ind(
          gstk_envelope_sms_pp_download_ind_type * sms_pp_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_sms_pp_download_command_type  STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   uint32                                      addr_length=0;

   if(sms_pp_cmd == NULL)
   {
     UIM_MSG_ERR_0("sms_pp_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_sms_pp_download_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_SMS_PP_DOWNLOAD_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
       return gstk_status;
   }

   /* optional address tag */
   if(sms_pp_cmd->address.length > 0) {
     gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &sms_pp_cmd->address,
       &STK_envelope_cmd.address);

     if (gstk_status != GSTK_SUCCESS) {
         UIM_MSG_ERR_1("In SMS PP: Address Packing Fail 0x%x", gstk_status);
         gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
         return gstk_status;
     }
   }

   /* tpdu tag */
   gstk_status = gstk_packer_sms_tpdu_tlv(
         sms_pp_cmd->is_cdma_sms_pp,
         &sms_pp_cmd->tpdu,
         &STK_envelope_cmd.tpdu);

   if (gstk_status != GSTK_SUCCESS) {
       gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
       return gstk_status;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = sms_pp_cmd->message_header.user_data;

         /* initialized to 3 for value portion of the TLVs
            i.e., the final offset will be either 0 or 1 depends
            on whether the total length is > 0x7F or not */
         offset = 3;

         /* copy device */
         /* No Need to check STK_envelope_cmd.device_id.device_tag_length as is
            fixed length in packer function
         */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &STK_envelope_cmd.device_id,
                                    offset,
                                    (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.device_id)) <
            (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
         {
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_MEMORY_ERROR;
         }

	  offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);

         /* total length: + device tlv */
         STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                   GSTK_TAG_LENGTH_LEN;

         /* copy optional address */
         if(sms_pp_cmd->address.length > 0){
           addr_length = STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN;
           if (gstk_check_envelope_offset(uim_cmd_ptr, offset, addr_length) != GSTK_SUCCESS){
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
             return GSTK_MEMORY_ERROR;
           }
           if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                      &STK_envelope_cmd.address,
                                      offset,
                                      addr_length,
                                      sizeof(uim_cmd_ptr->envelope.data),
                                      sizeof(STK_envelope_cmd.address)) < addr_length)
           {
             UIM_MSG_ERR_0("gstk_sms_pp_dl_ind(): gstk_byte_offset_memcpy failed");
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
             return GSTK_MEMORY_ERROR;
           }

           offset = offset + uint32toint32(addr_length );
           /* total length: + address tlv */
           STK_envelope_cmd.length += uint32touint8(addr_length);
         }

         if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
         {
           UIM_MSG_ERR_0("Length of command exceeding UIM_MAX_ENVELOPE_BYTES");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_MEMORY_ERROR;
         }
         /* copy tpdu */
         /* copy tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tpdu.tag;
         if (STK_envelope_cmd.tpdu.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
           {
             UIM_MSG_ERR_0("Length of command exceeding UIM_MAX_ENVELOPE_BYTES");
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
             return GSTK_MEMORY_ERROR;
           }
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           /* Adding up length of the command to make sure it will not overflow the uint8 */
           if((STK_envelope_cmd.length + 1 + STK_envelope_cmd.tpdu.length + GSTK_TAG_LENGTH_LEN) >
              (0xFF - GSTK_TAG_LENGTH_LEN - 1))
           {
             UIM_MSG_ERR_0("Length of command exceeding 0xFF");
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
             return GSTK_INVALID_LENGTH;
           }
           STK_envelope_cmd.length++;
         }
         if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
         {
           UIM_MSG_ERR_0("Length of command exceeding UIM_MAX_ENVELOPE_BYTES");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_MEMORY_ERROR;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tpdu.length;

         if (gstk_check_envelope_offset(uim_cmd_ptr, offset, STK_envelope_cmd.tpdu.length) !=
                                                                   GSTK_SUCCESS){
           return GSTK_MEMORY_ERROR;
         }

         if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
         {
           UIM_MSG_ERR_1("Length of command exceeding UIM_MAX_ENVELOPE_BYTES offset 0x%x",
                         offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_INVALID_LENGTH;
         }

         (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                sizeof(uim_cmd_ptr->envelope.data) - offset,
                STK_envelope_cmd.tpdu.sms_tpdu,
                int32touint32(STK_envelope_cmd.tpdu.length));
         offset = offset + STK_envelope_cmd.tpdu.length;
         /* total length: + tpdu tlv */
         STK_envelope_cmd.length += STK_envelope_cmd.tpdu.length + GSTK_TAG_LENGTH_LEN;

         /* populate the envelope command info */
         if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
           uim_cmd_ptr->envelope.offset = 0;
           uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           STK_envelope_cmd.length++;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }
         else {
           uim_cmd_ptr->envelope.offset = 1;
           uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }

         gstk_util_dump_byte_array("Env: SMS PP Data",
                  &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                  uim_cmd_ptr->envelope.num_bytes);

         if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
         {
           gstk_status = gstk_process_and_store_env_cmd_for_retry(
                           uim_cmd_ptr->envelope.data,
                           (uint32)uim_cmd_ptr->envelope.num_bytes,
                           uim_cmd_ptr->hdr.user_data);

           UIM_MSG_HIGH_1("gstk_process_and_store_env_cmd_for_retry() gstk_status = %d",
                          gstk_status);
           if(gstk_status == GSTK_SUCCESS)
           {
             gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state =
               GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
           }
           else
           {
             gstk_status = GSTK_SUCCESS;
           }
         }
         else
         {
           UIM_MSG_MED_0("SMS_PP retry is disabled");
         }

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
   return gstk_status;

} /* gstk_sms_pp_dl_ind */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind

DESCRIPTION

  This function builds the cell broadcast download command and sends it to UIM

PARAMETERS
  cb_dl_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_cell_broadcast_dl_ind(
          gstk_envelope_cell_broadcast_download_ind_type * cb_dl_cmd)
{
   gstk_status_enum_type                              gstk_status = GSTK_SUCCESS;
   gstk_envelope_cell_broadcast_download_command_type STK_envelope_cmd;
   uim_cmd_type                                      *uim_cmd_ptr;
   int                                                offset = 0;

   UIM_MSG_HIGH_0("In gstk_cell_broadcast_dl_ind()");
   if(cb_dl_cmd == NULL)
   {
     UIM_MSG_ERR_0("cb_dl_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_cell_broadcast_download_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_CELL_BROADCAST_DOWNLOAD_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   /* cb page tag */
   gstk_status = gstk_packer_cell_broadcast_page_tlv(
         &cb_dl_cmd->cb_page,
         &STK_envelope_cmd.cb_page);

   if (gstk_status != GSTK_SUCCESS) {
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = cb_dl_cmd->message_header.user_data;

         /* initialized to 3 for value portion of the TLVs
            i.e., the final offset will be either 0 or 1 depends
            on whether the total length is > 0x7F or not */
         offset = 3;

         /* copy device */
         /* STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &(STK_envelope_cmd.device_id),
                                    offset,
                                    (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.device_id)) <
            (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
         {
           UIM_MSG_ERR_0("gstk_cell_broadcast_dl_ind(): gstk_byte_offset_memcpy failed");
           gstk_status = GSTK_MEMORY_ERROR;
         }
         if (gstk_status == GSTK_SUCCESS)
         {
         offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);

         /* total length: + device tlv */
         STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                   GSTK_TAG_LENGTH_LEN;

         /* copy cellbroadcast page */
         /* copy tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.tag;
         if (STK_envelope_cmd.cb_page.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.length;

         /* STK_envelope_cmd.cb_page.length fixed in packer function */
         (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                sizeof(uim_cmd_ptr->envelope.data) - offset,
                STK_envelope_cmd.cb_page.cb_page,
                (int)(STK_envelope_cmd.cb_page.length));
         offset = (int)(offset + STK_envelope_cmd.cb_page.length);
         /* total length: + cb page tlv */
         STK_envelope_cmd.length += STK_envelope_cmd.cb_page.length + GSTK_TAG_LENGTH_LEN;

         /* populate the envelope command info */
         if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
           uim_cmd_ptr->envelope.offset = 0;
           uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           STK_envelope_cmd.length++;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }
         else {
           uim_cmd_ptr->envelope.offset = 1;
           uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }

         gstk_util_dump_byte_array("Env: Cell Broadcast DL Data",
                    &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                    uim_cmd_ptr->envelope.num_bytes);

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
   return gstk_status;
} /* gstk_cell_broadcast_dl_ind */

/*===========================================================================
FUNCTION gstk_get_gwl_loc_info

DESCRIPTION
  This function obtains location info based on passed in data:
  - If only valid RAT is provided, then the function attempts to obtain
    the location info (PLMN, LAC and Cell ID) from the GSTK caches.
  - If valid Location info is passed in as well and the PLMN and LAC
    match the cached value for the RAT, Cell ID is obtained from the GSTK cache
    if valid cell ID is not passed in.
  - If valid Location info is passed in, but does not match the cached values
    in GSTK, then the passed in location info is returned back.

PARAMETERS
  loc_info_rsp_ptr : [Output] Pointer to populate location info
  rat              : [Input]  Access tech. for which location info is needed
  gwl_info_ptr     : [Input]  Pointer to location data passed in by client
  slot             : [Input]  Slot ID

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This funciton is to be used for GSM/WCDMA/LTE only.

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
static gstk_status_enum_type gstk_get_gwl_loc_info(
  gstk_location_info_rsp_type   *loc_info_rsp_ptr,
  gstk_access_technology_type    rat,
  gstk_gwl_loc_info             *gwl_info_ptr,
  gstk_slot_id_enum_type         slot
)
{
  gstk_gwl_loc_info           temp_gwl_info;
  gstk_cell_id_type           temp_cell_info   = {0};
  boolean                     loc_info_valid   = FALSE;
  boolean                     cell_id_valid    = FALSE;

  UIM_MSG_HIGH_2("In gstk_get_gwl_loc_info, RAT: 0x%x, slot: 0x%x", rat, slot);

  /* Validate Slot ID */
  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("gstk_get_gwl_loc_info: Invalid Slot: 0x%x", slot);
    return GSTK_BAD_PARAM;
  }

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* Validate input pointers */
  if ((loc_info_rsp_ptr == NULL)||(gwl_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Either loc_info_ptr: 0x%x or gwl_info_ptr: 0x%x is NULL",
                  loc_info_rsp_ptr, gwl_info_ptr);
    return GSTK_BAD_PARAM;
  }

  /* If RAT is WLAN, no location information*/
  if (rat == GSTK_ACCESS_TECH_WLAN)
  {
    UIM_MSG_HIGH_0("RAT is WLAN, set location info to all 0xFF and cell_id_len to 0");
    memset(loc_info_rsp_ptr, 0xFF, sizeof(gstk_location_info_rsp_type));
    loc_info_rsp_ptr->cell_id_len = 0;
    return GSTK_SUCCESS;
  }
  /* Validate RAT - it has to be a GWL RAT */
  if((rat != GSTK_ACCESS_TECH_GSM) &&
     (rat != GSTK_ACCESS_TECH_UTRAN) &&
     (rat != GSTK_ACCESS_TECH_LTE))
  {
    UIM_MSG_ERR_1("Invalid RAT: 0x%x", rat);
    return GSTK_BAD_PARAM;
  }

  /* Initialize temp. variables with invalid info */
  memset(&temp_gwl_info, 0xFF, sizeof (temp_gwl_info));
  temp_gwl_info.cell_id_len = GSTK_MAX_CELL_ID_LEN;

  memset(temp_cell_info.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
  temp_cell_info.cell_len = GSTK_MAX_CELL_ID_LEN;

  /* Check if the entire location info passed in is invalid (all 0xFFs) */
  do
  {
    if((gwl_info_ptr->cell_id_len > 0) &&
       (gwl_info_ptr->cell_id_len <= GSTK_MAX_CELL_ID_LEN) &&
       (memcmp(gwl_info_ptr->cell_id,
               temp_gwl_info.cell_id,
               gwl_info_ptr->cell_id_len)) != 0)
    {
      loc_info_valid = TRUE;
      cell_id_valid = TRUE;

      UIM_MSG_HIGH_0("Valid Cell ID passed in");
      break;
    }
    if(memcmp(gwl_info_ptr->mcc_and_mnc,
              temp_gwl_info.mcc_and_mnc,
              sizeof(temp_gwl_info.mcc_and_mnc)) != 0)
    {
      loc_info_valid = TRUE;
      break;
    }
    if(memcmp(gwl_info_ptr->loc_area_code,
              temp_gwl_info.loc_area_code,
              sizeof(temp_gwl_info.loc_area_code)) != 0)
    {
      loc_info_valid = TRUE;
      break;
    }
  }while(0);

  UIM_MSG_HIGH_1("Validity of location Info provided: 0x%x",
                 loc_info_valid);

  /* GSM RAT*/
  if(rat == GSTK_ACCESS_TECH_GSM)
  {
    /* If location info is invalid, extract the same from the GSTK cache
       using the RAT provided */
    if(loc_info_valid == FALSE)
    {
      /* Copy PLMN information */
      gstk_memcpy(loc_info_rsp_ptr->mcc_and_mnc,
                  gstk_curr_inst_ptr->gstk_loci_table.plmn_id,
                  GSTK_MAX_PLMN_LEN,
                  GSTK_MAX_PLMN_LEN,
                  GSTK_MAX_PLMN_LEN);

      /* Copy LAC information */
      gstk_memcpy(loc_info_rsp_ptr->loc_area_code,
                  gstk_curr_inst_ptr->gstk_loci_table.lac,
                  GSTK_MAX_LAC_LEN,
                  GSTK_MAX_LAC_LEN,
                  GSTK_MAX_LAC_LEN);

      /* Query RR for Cell ID */
      memset(temp_cell_info.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
      (void)gstk_get_rr_cell_id_info(gstk_curr_inst_ptr->gstk_loci_table.plmn_id,
                                     gstk_curr_inst_ptr->gstk_loci_table.lac,
                                     &temp_cell_info,
                                     slot);

      /* 3GPP TS 11.14, 12.19 Location Information: Cell ID length = 2*/
      loc_info_rsp_ptr->cell_id_len = 2;
      gstk_memcpy(loc_info_rsp_ptr->cell_id,
                  temp_cell_info.cell_id,
                  loc_info_rsp_ptr->cell_id_len,
                  sizeof(loc_info_rsp_ptr->cell_id),
                  loc_info_rsp_ptr->cell_id_len);
    }
    /* Valid location info has been passed to the function */
    else
    {
      boolean plmn_lac_match = TRUE;

      /* Check if MCC, MNC and LAC passed in match the RR cache */
      do
      {
        if(memcmp(gwl_info_ptr->mcc_and_mnc,
                  gstk_curr_inst_ptr->gstk_loci_table.plmn_id,
                  sizeof(temp_gwl_info.mcc_and_mnc)) != 0)
        {
          plmn_lac_match = FALSE;
          break;
        }
        if(memcmp(gwl_info_ptr->loc_area_code,
                  gstk_curr_inst_ptr->gstk_loci_table.lac,
                  sizeof(temp_gwl_info.loc_area_code)) != 0)
        {
          plmn_lac_match = FALSE;
          break;
        }
      }while(0);

      UIM_MSG_HIGH_1("PLMN and LAC match status b/w RR cache and info passed in: 0x%x",
                     plmn_lac_match);

      /* 3GPP TS 11.14, 12.19 Location Information: Cell ID length = 2*/
      loc_info_rsp_ptr->cell_id_len = GSTK_MIN_CELL_ID_LEN;
      if (cell_id_valid)
      {
        gstk_memcpy(loc_info_rsp_ptr->cell_id,
                    gwl_info_ptr->cell_id,
                    loc_info_rsp_ptr->cell_id_len,
                    sizeof(loc_info_rsp_ptr->cell_id),
                    sizeof(gwl_info_ptr->cell_id));
      }
      /* Else if PLMN/LAC matches, query RR for Cell ID */
      else if (plmn_lac_match == TRUE)
      {
        /* Query RR for Cell ID */
        memset(temp_cell_info.cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);
        (void)gstk_get_rr_cell_id_info(gstk_curr_inst_ptr->gstk_loci_table.plmn_id,
                                       gstk_curr_inst_ptr->gstk_loci_table.lac,
                                       &temp_cell_info,
                                       slot);

        /* 3GPP TS 11.14, 12.19 Location Information: Cell ID length = 2*/
        gstk_memcpy(loc_info_rsp_ptr->cell_id,
                    temp_cell_info.cell_id,
                    loc_info_rsp_ptr->cell_id_len,
                    sizeof(loc_info_rsp_ptr->cell_id),
                    loc_info_rsp_ptr->cell_id_len);
      }
      /* If PLMN and LAC are different from arbitrated loci cache, then attempt to find
         PLMN/LAC in GSTK MM caches*/
      else
      {
        if (gstk_find_cached_cell_id(gwl_info_ptr->mcc_and_mnc,
                                     gwl_info_ptr->loc_area_code,
                                     &temp_cell_info) == GSTK_SUCCESS)
        {
          loc_info_rsp_ptr->cell_id_len = temp_cell_info.cell_len;
          if (gstk_memcpy(loc_info_rsp_ptr->cell_id,
                          temp_cell_info.cell_id,
                          loc_info_rsp_ptr->cell_id_len,
                          sizeof(loc_info_rsp_ptr->cell_id),
                          temp_cell_info.cell_len) <
              temp_cell_info.cell_len)
          {
            return GSTK_MEMORY_ERROR;
          }
        }
        else
        {
          UIM_MSG_ERR_0("PLMN/LAC from client could not be found in GSTK MM cache");
          memset(loc_info_rsp_ptr->cell_id, 0xFF, sizeof(loc_info_rsp_ptr->cell_id));
        }
      }

      /* Copy PLMN information */
      gstk_memcpy(loc_info_rsp_ptr->mcc_and_mnc,
          gwl_info_ptr->mcc_and_mnc,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN);

      /* Copy LAC information */
      gstk_memcpy(loc_info_rsp_ptr->loc_area_code,
          gwl_info_ptr->loc_area_code,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN);
    }
  }
  /* UTRAN or LTE RAT */
  else
  {
    uint8 rrc_index = 0;

    /* Get the index to the RRC cache based on the access tech */
    switch(rat)
    {
      case GSTK_ACCESS_TECH_UTRAN:
        rrc_index = GSTK_RAT_UMTS_IDX;
        break;
      case GSTK_ACCESS_TECH_LTE:
        rrc_index = GSTK_RAT_LTE_IDX;
        break;
      default:
        UIM_MSG_HIGH_1("Incorrect input for access tech 0x%x", rat);
        return GSTK_BAD_PARAM;
    }

    if (gstk_curr_inst_ptr->card_protocol == MMGSDI_UICC)
    {
      loc_info_rsp_ptr->cell_id_len = GSTK_MAX_CELL_ID_LEN;
    }
    else
    {
      loc_info_rsp_ptr->cell_id_len = GSTK_MIN_CELL_ID_LEN;
    }

    UIM_MSG_HIGH_1("Setting cell_id_len to 0x%x",loc_info_rsp_ptr->cell_id_len);

    /* If location info is invalid, extract the same from the GSTK cache
       using the RAT provided */
    if(loc_info_valid == FALSE)
    {
      /* Copy PLMN information */
      gstk_memcpy(loc_info_rsp_ptr->mcc_and_mnc,
          gstk_curr_inst_ptr->rrc_loci_table[rrc_index].plmn_id,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN);

      /* Copy LAC/TAU information */
      gstk_memcpy(loc_info_rsp_ptr->loc_area_code,
          gstk_curr_inst_ptr->rrc_loci_table[rrc_index].lac,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN);

      /* Copy Cell ID information */
      gstk_memcpy(loc_info_rsp_ptr->cell_id,
                  gstk_curr_inst_ptr->rrc_loci_table[rrc_index].cell_info.cell_id,
                  loc_info_rsp_ptr->cell_id_len,
                  sizeof(loc_info_rsp_ptr->cell_id),
                  sizeof(gstk_curr_inst_ptr->rrc_loci_table[rrc_index].cell_info.cell_id));

      loc_info_rsp_ptr->cell_id_len = gstk_curr_inst_ptr->rrc_loci_table[rrc_index].cell_info.cell_len;
    }
    /* Valid location info has been passed to the function */
    else
    {
      boolean plmn_lac_match = FALSE;

      /* Check if passed in PLMN and LAC are valid and if so retrieve
         corresponding cell ID */
      plmn_lac_match = gstk_get_rrc_cell_id_info(gwl_info_ptr->mcc_and_mnc,
                                                 gwl_info_ptr->loc_area_code,
                                                 &temp_cell_info,
                                                 slot,
                                                 rat);

     UIM_MSG_HIGH_1("PLMN and LAC match status b/w RRC cache and info passed in: 0x%x",
                    plmn_lac_match);

      if (cell_id_valid)
      {
        gstk_memcpy(loc_info_rsp_ptr->cell_id,
                    gwl_info_ptr->cell_id,
                    loc_info_rsp_ptr->cell_id_len,
                    sizeof(loc_info_rsp_ptr->cell_id),
                    sizeof(gwl_info_ptr->cell_id));
      }
      /* if PLMN/LAC matches copy the Cell ID from the RRC cache */
      else if (plmn_lac_match == TRUE)
      {
        gstk_memcpy(loc_info_rsp_ptr->cell_id,
                    temp_cell_info.cell_id,
                    loc_info_rsp_ptr->cell_id_len,
                    sizeof(loc_info_rsp_ptr->cell_id),
                    sizeof(temp_cell_info.cell_id));
      }
      /* If PLMN and LAC are different from rrc table, then attempt to find
         PLMN/LAC in GSTK MM caches*/
      else
      {
        if (gstk_find_cached_cell_id(gwl_info_ptr->mcc_and_mnc,
                                     gwl_info_ptr->loc_area_code,
                                     &temp_cell_info) == GSTK_SUCCESS)
        {
          if (gstk_memcpy(loc_info_rsp_ptr->cell_id,
                          temp_cell_info.cell_id,
                          loc_info_rsp_ptr->cell_id_len,
                          sizeof(loc_info_rsp_ptr->cell_id),
                          temp_cell_info.cell_len) <
              temp_cell_info.cell_len)
          {
            UIM_MSG_ERR_0("Could not copy cell_id into loc_info_rsp_ptr");
            return GSTK_MEMORY_ERROR;
          }
        }
        else
        {
          UIM_MSG_ERR_0("PLMN/LAC from client could not be found in GSTK MM cache");
          memset(loc_info_rsp_ptr->cell_id, 0xFF, sizeof(loc_info_rsp_ptr->cell_id));
        }
      }

      /* Copy PLMN information */
      gstk_memcpy(loc_info_rsp_ptr->mcc_and_mnc,
          gwl_info_ptr->mcc_and_mnc,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN,
          GSTK_MAX_PLMN_LEN);

      /* Copy LAC information */
      gstk_memcpy(loc_info_rsp_ptr->loc_area_code,
          gwl_info_ptr->loc_area_code,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN,
          GSTK_MAX_LAC_LEN);
    }
  }

  return GSTK_SUCCESS;
} /* gstk_get_gwl_loc_info */
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA) || defined(FEATURE_LTE)*/

/*===========================================================================
FUNCTION gstk_copy_location_info

DESCRIPTION
  This function copies location information from one buffer to another. This is
  used for GSM/WCDMA/LTE only.

PARAMETERS
  loc_info_ptr :   pointer of gstk_location_info_rsp_type
  cc_cmd_ptr   :   pointer of gstk_envelope_cc_ind_type

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS

SEE ALSO
  None
===========================================================================*/
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
static gstk_status_enum_type gstk_copy_location_info(
  gstk_location_info_rsp_type*  loc_info_ptr,
  gstk_envelope_cc_ind_type*    cc_cmd_ptr
)
{
  UIM_MSG_HIGH_0("In gstk_copy_location_info()");

  if (loc_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("loc_info_ptr is NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize cell info with invalid cell information */
  memset(loc_info_ptr->cell_id, 0xFF, sizeof (loc_info_ptr->cell_id));

  if (cc_cmd_ptr != NULL)
  {
    return gstk_get_gwl_loc_info(loc_info_ptr,
                                 cc_cmd_ptr->cc_data.rat,
                                 &cc_cmd_ptr->cc_data.loc_info.gwl_info,
                                 cc_cmd_ptr->message_header.sim_slot_id);
  } /* if (cc_cmd_ptr != NULL) */
  else
  {
    UIM_MSG_ERR_0("cmd_ptr is NULL");
    return GSTK_BAD_PARAM;
  } /* else if (cc_cmd_ptr != NULL) */
} /* gstk_copy_location_info */
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA) || defined(FEATURE_LTE)*/

/*===========================================================================
FUNCTION gstk_copy_1x_location_info

DESCRIPTION
  This function copies 1X location information from one buffer to another buffer

PARAMETERS
  cdma_ext_loc_info_ptr :   pointer of gstk_cdma_location_info_type
  cdma_cmd_ptr          :   pointer of gstk_cdma_loc_info

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_CDMA
static gstk_status_enum_type gstk_copy_1x_location_info(
  gstk_cdma_loci_ext_type*        cdma_ext_loc_info_ptr,
  gstk_cdma_loc_info*             cdma_cmd_ptr
)
{
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  UIM_MSG_HIGH_0("In gstk_copy_1x_location_info()");

  if (cdma_ext_loc_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("cdma_ext_loc_info_ptr is NULL");
    return GSTK_NULL_INPUT_PARAM;
  }

  if (cdma_cmd_ptr != NULL)
  {
    /* SID info */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.sid_info.sid,
                   &cdma_cmd_ptr->sid,
                   sizeof(cdma_cmd_ptr->sid),
                   GSTK_MAX_SID_LEN,
                   GSTK_MAX_SID_LEN);
    cdma_ext_loc_info_ptr->cdma_loc_info.sid_info.id_len = GSTK_MAX_SID_LEN;

    /* NID info */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.nid_info.nid,
                   &cdma_cmd_ptr->nid,
                   sizeof(cdma_cmd_ptr->nid),
                   GSTK_MAX_NID_LEN,
                   GSTK_MAX_NID_LEN);
    cdma_ext_loc_info_ptr->cdma_loc_info.nid_info.id_len = GSTK_MAX_NID_LEN;

    /* MCC info */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.mcc,
                   &cdma_cmd_ptr->mcc,
                   sizeof(cdma_cmd_ptr->mcc),
                   GSTK_MCC_LEN,
                   GSTK_MCC_LEN);

    /* IMSI_11_12 */
    cdma_ext_loc_info_ptr->cdma_loc_info.imsi_11_12 = cdma_cmd_ptr->imsi_11_12;

    /* BASE_ID info */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.base_id_info.base_id,
                   &cdma_cmd_ptr->base_id,
                   sizeof(cdma_cmd_ptr->base_id),
                   GSTK_MAX_BASE_ID_LEN,
                   GSTK_MAX_BASE_ID_LEN);
    cdma_ext_loc_info_ptr->cdma_loc_info.base_id_info.id_len = GSTK_MAX_BASE_ID_LEN;

    /* BASE Latitude */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.base_lat,
                   &cdma_cmd_ptr->base_lat,
                   GSTK_BASE_LAT_LEN,
                   GSTK_BASE_LAT_LEN,
                   sizeof(cdma_cmd_ptr->base_lat));

    /* BASE Longitude */
    (void)gstk_memcpy(cdma_ext_loc_info_ptr->cdma_loc_info.base_long,
                   &cdma_cmd_ptr->base_long,
                   GSTK_BASE_LONG_LEN,
                   GSTK_BASE_LONG_LEN,
                   sizeof(cdma_cmd_ptr->base_long));
  }
  else
  {
    /* Client has not passed source of location information.
       Let's try to check if we have it locally */
    if (!gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid)
    {
      UIM_MSG_ERR_0("invalid CDMA loc infor!");
      return GSTK_ERROR;
    }
    else
    {
      *cdma_ext_loc_info_ptr = gstk_curr_inst_ptr->cdma_ext_loci_cache.data;
    }
  }
  return GSTK_SUCCESS;
}
#endif /* FEATURE_CDMA */

/*===========================================================================
FUNCTION gstk_cc_ind

DESCRIPTION

  This function builds the call control command and sends it to UIM.

PARAMETERS
  cc_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_cc_ind(
          gstk_envelope_cc_ind_type* cc_cmd)
{
  gstk_status_enum_type                       gstk_status      = GSTK_SUCCESS;
  gstk_envelope_cc_command_type               STK_envelope_cmd = {0};
  gstk_location_info_rsp_type                 location_info; /* For GSM/WCDMA/LTE */
  uim_cmd_type*                               uim_cmd_ptr      = NULL;
  int                                         offset           = 0;
  uint32                                      addr_length      = 0;
  gstk_access_technology_type                 rat              = GSTK_ACCESS_NONE;
  gstk_cdma_loci_ext_type                     cdma_ext_loc_info; /* For 3GPP2 */
  gstk_nv_items_data_type                     nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cc_cmd == NULL)
  {
    UIM_MSG_ERR_0("cc_cmd ERR : NULL");
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1 ("In gstk_cc_ind (), Rat is 0x%x", cc_cmd->cc_data.rat);

  /* initialize location_info */
  memset(&location_info, 0, sizeof(gstk_location_info_rsp_type));
  memset(&cdma_ext_loc_info, 0xFF, sizeof(gstk_cdma_loci_ext_type));

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  switch(cc_cmd->cc_data.rat)
  {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
    case GSTK_ACCESS_TECH_GSM:
    case GSTK_ACCESS_TECH_UTRAN:
    case GSTK_ACCESS_TECH_LTE:
      rat = cc_cmd->cc_data.rat;
      gstk_status = gstk_copy_location_info(&location_info, cc_cmd);
      if (gstk_status != GSTK_SUCCESS)
      {
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
      }
      break;
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA) || defined(FEATURE_LTE)*/

    case GSTK_ACCESS_TECH_HRPD:
    case GSTK_ACCESS_TECH_EHRPD:
#ifdef FEATURE_CDMA
      if((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE) &&
         (nv_data.cat_version >= GSTK_CFG_CAT_VER6))
      {
        rat = cc_cmd->cc_data.rat;
        if (!gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid)
        {
          UIM_MSG_ERR_0("No valid HDR loc info!");
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return GSTK_ERROR;
        }
        else
        {
          UIM_MSG_HIGH_0("Use HDR loc info cache for CC");
          cdma_ext_loc_info = gstk_curr_inst_ptr->cdma_ext_loci_cache.data;
        }
      }
      else
      {
        UIM_MSG_ERR_1("cat_version: 0x%x", nv_data.cat_version);
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
      }
#else
        UIM_MSG_ERR_0("FEATURE_CDMA is not enabled");
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
#endif
      break;

    case GSTK_ACCESS_TECH_CDMA:
#ifdef FEATURE_CDMA
        rat = cc_cmd->cc_data.rat;
        /* Copy location information from the cmd to cdma_ext_loc_info */
        gstk_status = gstk_copy_1x_location_info(&cdma_ext_loc_info, &cc_cmd->cc_data.loc_info.cdma_info);
        if (gstk_status != GSTK_SUCCESS)
        {
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
#else
        UIM_MSG_ERR_0("FEATURE_CDMA is not enabled");
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
#endif /* FEATURE_CDMA */
       break;
    case GSTK_ACCESS_TECH_WLAN:
      rat = cc_cmd->cc_data.rat;
      break;
    default:
      UIM_MSG_ERR_0 ("Invalid RAT");
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_BAD_PARAM;
  }

  if (cc_cmd->cc_data.call_enum_type == GSTK_PDP_CONTEXT_ACT  ||
      cc_cmd->cc_data.call_enum_type == GSTK_EPS_PDN_CONN_ACT ||
      cc_cmd->cc_data.call_enum_type == GSTK_IMS_CALL)
  {
    gstk_device_id_type          dev_id;
    gstk_location_info_gen_type  gen_location_info;

    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr == NULL)
    {
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_MEMORY_ERROR;
    }
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL))
    {
      UIM_MSG_ERR_2("UIM cmd buf becomes null 0x%x or status != success 0x%x",
                    uim_cmd_ptr, gstk_status);
      if (uim_cmd_ptr != NULL)
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
      }
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_MEMORY_ERROR;
    }

    /* set user_data */
    uim_cmd_ptr->hdr.user_data = cc_cmd->message_header.user_data;

    /* initialized to 3 for value portion of the TLVs
    i.e., the final offset will be either 0 or 1 depends
    on whether the total length is > 0x7F or not */
    offset = 3;

    /* Initialize to the beginning offset for final calculation below */
    uim_cmd_ptr->envelope.num_bytes = uint32touint8(offset);

    /* copy device */
    dev_id.src = GSTK_ME_DEVICE;
    dev_id.dest = GSTK_UICC_SIM_DEVICE;
    gstk_status = gstk_util_populate_tlv(
                    GSTK_DEVICE_IDENTITY_TAG,
                    (int32) offset,
                    (gstk_cmd_to_uim_type*)&dev_id,
                    uim_cmd_ptr->envelope.data,
                    (int32 *) &offset,
                    GSTK_ENVELOPE_CMD);/*lint !e826 dev_id suppress area too small */

    if (gstk_status != GSTK_SUCCESS)
    {
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    if (cc_cmd->cc_data.call_enum_type == GSTK_PDP_CONTEXT_ACT)
    {
      /* copy PDP Param */
      gstk_status = gstk_util_populate_tlv(
                      GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG,
                      (int32) offset,
                      (gstk_cmd_to_uim_type*)&cc_cmd->cc_data.gstk_address_string.pdp_context,
                      uim_cmd_ptr->envelope.data,
                      (int32 *) &offset,
                      GSTK_ENVELOPE_CMD);

    }
    else if (cc_cmd->cc_data.call_enum_type == GSTK_EPS_PDN_CONN_ACT)
    {
      /* copy EPS param */
      gstk_status = gstk_util_populate_tlv(
                      GSTK_EPS_PDN_CONN_PARAM_TAG,
                      (int32) offset,
                      (gstk_cmd_to_uim_type*)&cc_cmd->cc_data.gstk_address_string.eps_conn_act,
                      uim_cmd_ptr->envelope.data,
                      (int32 *) &offset,
                      GSTK_ENVELOPE_CMD);
    }
    else if (cc_cmd->cc_data.call_enum_type == GSTK_IMS_CALL)
    {
      /* copy EPS param */
      gstk_status = gstk_util_populate_tlv(
                      GSTK_IMS_URI_TAG,
                      (int32) offset,
                      (gstk_cmd_to_uim_type*)&cc_cmd->cc_data.gstk_address_string.ims_call,
                      uim_cmd_ptr->envelope.data,
                      (int32 *) &offset,
                      GSTK_ENVELOPE_CMD);
    }


    if (gstk_status != GSTK_SUCCESS)
    {
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    /* copy location info */
    if(cc_cmd->cc_data.rat != GSTK_ACCESS_TECH_WLAN)
    {
      gen_location_info.location_info_tech = GSTK_GSM_UMTS_LOCATION_INFO;
      (void)memscpy(&gen_location_info.location_info.gsm_umts_loc_info,
             sizeof(gstk_location_info_rsp_type),
             &location_info,
             sizeof(gstk_location_info_rsp_type));
      gstk_status = gstk_util_populate_tlv(
                      GSTK_LOCATION_INFORMATION_TAG,
                      (int32) offset,
                      (gstk_cmd_to_uim_type*)&gen_location_info,
                      uim_cmd_ptr->envelope.data,
                      (int32 *) &offset,
                      GSTK_ENVELOPE_CMD);/*lint !e826 gen_location_info suppress area too small */

      if (gstk_status != GSTK_SUCCESS) {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return GSTK_ERROR;
      }
    }

    gstk_status = gstk_util_populate_uim_cmd_total_data_len(
                     GSTK_CALL_CONTROL_TAG,
                     (int32) (offset - uim_cmd_ptr->envelope.num_bytes),
                     uim_cmd_ptr);

    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1 ("status 0x%x for total data len", gstk_status);
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    gstk_util_dump_byte_array("Env: CC Data",
                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                   uim_cmd_ptr->envelope.num_bytes);

    if(gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY, gstk_curr_inst_ptr->slot_id) == FALSE)
    {
      gstk_status = gstk_process_and_store_env_cmd_for_retry(
                     uim_cmd_ptr->envelope.data,
                     (uint32)uim_cmd_ptr->envelope.num_bytes,
                     uim_cmd_ptr->hdr.user_data);
      UIM_MSG_HIGH_1("gstk_process_and_store_env_cmd_for_retry() gstk_status = %d",
                     gstk_status);
      if( gstk_status == GSTK_SUCCESS)
      {
        gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
      }
      else
      {
        gstk_status = GSTK_SUCCESS;
      }
    }/* if(!GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY) */
    else
    {
      UIM_MSG_HIGH_0("Call ctrl envelope retry is disabled");
    }

    gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
  }
  else
  {
    /* Pack various TLVs*/
    STK_envelope_cmd.tag = GSTK_CALL_CONTROL_TAG;
    /* Pack device ID */
    gstk_status = gstk_packer_dev_id_tlv(
        GSTK_ME_DEVICE,
        GSTK_UICC_SIM_DEVICE,
        &STK_envelope_cmd.device_id );
    if (gstk_status != GSTK_SUCCESS)
    {
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return gstk_status;
    }
    /* Pack address */
    switch(cc_cmd->cc_data.call_enum_type)
    {
      case GSTK_VOICE:
        gstk_status = gstk_packer_address_tlv(
          GSTK_ADDRESS_TAG,
          &cc_cmd->cc_data.gstk_address_string.voice.address,
          &STK_envelope_cmd.gstk_address_string_tlv.address);
        if (gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("In CC: Address Packing Fail 0x%x", gstk_status);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.address.length + 2;

        /* Pack optional Capability configuration parameter 1 */
        if(cc_cmd->cc_data.gstk_address_string.voice.ccp1.length > 0)
        {
          gstk_status = gstk_packer_ccp_tlv(
            &cc_cmd->cc_data.gstk_address_string.voice.ccp1,
            &STK_envelope_cmd.ccp1);
          if(gstk_status != GSTK_SUCCESS)
          {
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return gstk_status;
          }
          STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.ccp1.length + 2;
        }

        /* Pack optional Subaddress */
        if(cc_cmd->cc_data.gstk_address_string.voice.subaddress.length > 0)
        {
          gstk_status = gstk_packer_subaddress_tlv(
            &cc_cmd->cc_data.gstk_address_string.voice.subaddress,
            &STK_envelope_cmd.subaddress);
          if(gstk_status != GSTK_SUCCESS)
          {
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return gstk_status;
          }
          STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.subaddress.length + 2;
        }

        /* Pack optional Capability configuration parameter 2 */
        if(cc_cmd->cc_data.gstk_address_string.voice.ccp2.length > 0)
        {
          gstk_status = gstk_packer_ccp_tlv(
            &cc_cmd->cc_data.gstk_address_string.voice.ccp2,
            &STK_envelope_cmd.ccp2);
          if(gstk_status != GSTK_SUCCESS)
          {
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return gstk_status;
          }
          STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.ccp2.length + 2;
        }
        break;

      case GSTK_SS:
        gstk_status = gstk_packer_address_tlv(
          GSTK_SS_STRING_TAG,
          &cc_cmd->cc_data.gstk_address_string.ss_string,
          &STK_envelope_cmd.gstk_address_string_tlv.ss_string);
        if (gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("In CC: SS Packing Fail 0x%x", gstk_status);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.ss_string.length + 2;
        break;

      case GSTK_USSD:
        STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text =
                GSTK_CALLOC(cc_cmd->cc_data.gstk_address_string.ussd_string.length);
        if(STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text == NULL)
        {
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return GSTK_MEMORY_ERROR;
        }
        gstk_status = gstk_packer_text_string_tlv(
          GSTK_USSD_STRING_TAG,
          cc_cmd->cc_data.gstk_address_string.ussd_string.dcs,
          cc_cmd->cc_data.gstk_address_string.ussd_string.length,
          cc_cmd->cc_data.gstk_address_string.ussd_string.text,
          &STK_envelope_cmd.gstk_address_string_tlv.ussd_string);
        if (gstk_status != GSTK_SUCCESS)
        {
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length + 2;
        break;

      default:
        UIM_MSG_ERR_1("In CC: Unknown CC type: 0x%x",
                      cc_cmd->cc_data.call_enum_type);
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return GSTK_ERROR;
    }

    /* Pack location information */
    switch(rat)
    {
      case GSTK_ACCESS_TECH_HRPD:
      case GSTK_ACCESS_TECH_EHRPD:
      case GSTK_ACCESS_TECH_CDMA:
#ifdef FEATURE_CDMA
        gstk_status = gstk_packer_3gpp2_location_info_tlv(
                        &cdma_ext_loc_info,
                        &gstk_curr_inst_ptr->cdma_ext_loci_cache,
                        &STK_envelope_cmd.loc_info.cdma_ext_loc_info);
        if (gstk_status != GSTK_SUCCESS)
        {
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
#else
        UIM_MSG_ERR_0("FEATURE_CDMA is not defined!");
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
#endif /* FEATURE_CDMA */
        break;
      case GSTK_ACCESS_TECH_GSM:
      case GSTK_ACCESS_TECH_UTRAN:
      case GSTK_ACCESS_TECH_LTE:
        gstk_status = gstk_packer_location_info_tlv(
          &location_info, /* comprehension flag = 0! */
          &STK_envelope_cmd.loc_info.gw_loc_info);
        if (gstk_status != GSTK_SUCCESS)
        {
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        break;
      case GSTK_ACCESS_TECH_WLAN:
        UIM_MSG_HIGH_0("RAT type is WLAN");
        break;
      default:
        UIM_MSG_ERR_0("Invalid RAT");
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return GSTK_ERROR;
    }

    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr != NULL )
    {
      gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
      if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
      {
        /* set user_data */
        uim_cmd_ptr->hdr.user_data = cc_cmd->message_header.user_data;
        /* initialized to 3 for value portion of the TLVs
           i.e., the final offset will be either 0 or 1 depends
           on whether the total length is > 0x7F or not */
        offset = 3;

        /* copy device */
        /* STK_envelope_cmd.device_id.device_tag_length fixed in packer function
           no chance of buffer overflow */
        (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                  sizeof(uim_cmd_ptr->envelope.data)-offset,
                  &STK_envelope_cmd.device_id,
                  (int)(STK_envelope_cmd.device_id.device_tag_length
                        + GSTK_TAG_LENGTH_LEN));
        offset = offset + STK_envelope_cmd.device_id.device_tag_length +
                    GSTK_TAG_LENGTH_LEN;
        /* total length: + device tlv */
        STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                  GSTK_TAG_LENGTH_LEN;

        /* copy address */
        switch(cc_cmd->cc_data.call_enum_type)
        {
          case GSTK_VOICE:
            addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.address.length +
                              GSTK_TAG_LENGTH_LEN);
            if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset, addr_length) != GSTK_SUCCESS)
            {
              /* Error Out */
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
              return GSTK_MEMORY_ERROR;
            }
            (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                   &STK_envelope_cmd.gstk_address_string_tlv.address,
                   addr_length);
            offset = offset + addr_length;
            /* total length: + address tlv */
            STK_envelope_cmd.length += uint32touint8(addr_length);

            if(cc_cmd->cc_data.gstk_address_string.voice.ccp1.length > 0)
            {
              if(offset >= sizeof(uim_cmd_ptr->envelope.data))
              {
                UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                uim_cmd_ptr = NULL;
                gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                return GSTK_MEMORY_ERROR;
              }
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp1.tag;
              if (STK_envelope_cmd.ccp1.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
              {
                if(offset >= sizeof(uim_cmd_ptr->envelope.data))
                {
                  UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                  gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                  uim_cmd_ptr = NULL;
                  gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                  return GSTK_MEMORY_ERROR;
                }
                uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                STK_envelope_cmd.length++;
              }
              if(offset >= sizeof(uim_cmd_ptr->envelope.data))
              {
                UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                uim_cmd_ptr = NULL;
                gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                return GSTK_MEMORY_ERROR;
              }
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp1.length;
              if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset, STK_envelope_cmd.ccp1.length) !=
                                                                     GSTK_SUCCESS)
              {
                return GSTK_MEMORY_ERROR;
              }
              if ((offset < UIM_MAX_ENVELOPE_BYTES) &&
                  ((offset + STK_envelope_cmd.ccp1.length -1) < UIM_MAX_ENVELOPE_BYTES))
              {
                (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                       sizeof(uim_cmd_ptr->envelope.data)-offset,
                       STK_envelope_cmd.ccp1.ccp,
                       int32touint32(STK_envelope_cmd.ccp1.length));
              }
              else
              {
                UIM_MSG_ERR_2("buf is too small: 0x%x, 0x%x",
                              offset, STK_envelope_cmd.ccp1.length);
                return GSTK_MEMORY_ERROR;
              }
              offset = offset + STK_envelope_cmd.ccp1.length;
              STK_envelope_cmd.length += STK_envelope_cmd.ccp1.length +
                                            GSTK_TAG_LENGTH_LEN;
            }
            if(cc_cmd->cc_data.gstk_address_string.voice.subaddress.length > 0)
            {
              if(offset >= sizeof(uim_cmd_ptr->envelope.data))
              {
                UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                uim_cmd_ptr = NULL;
                gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                return GSTK_MEMORY_ERROR;
              }
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.subaddress.tag;
              if (STK_envelope_cmd.subaddress.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
              {
                if(offset >= sizeof(uim_cmd_ptr->envelope.data))
                {
                  UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                  gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                  uim_cmd_ptr = NULL;
                  gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                  return GSTK_MEMORY_ERROR;
                }
                uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                STK_envelope_cmd.length++;
              }
              if(offset >= sizeof(uim_cmd_ptr->envelope.data))
              {
                UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
                gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
                uim_cmd_ptr = NULL;
                gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
                return GSTK_MEMORY_ERROR;
              }
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.subaddress.length;
              if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset,
                      STK_envelope_cmd.subaddress.length) != GSTK_SUCCESS)
              {
                  return GSTK_MEMORY_ERROR;
              }
              if(offset < UIM_MAX_ENVELOPE_BYTES)
              {
                (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                      sizeof(uim_cmd_ptr->envelope.data)-offset,
                      STK_envelope_cmd.subaddress.subaddress,
                      int32touint32(STK_envelope_cmd.subaddress.length));
                offset = offset + STK_envelope_cmd.subaddress.length;
                STK_envelope_cmd.length += STK_envelope_cmd.subaddress.length +
                                            GSTK_TAG_LENGTH_LEN;
              }
            }
            break;

          case GSTK_SS:
            addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.ss_string.length
                                    + GSTK_TAG_LENGTH_LEN);
            if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset, addr_length) != GSTK_SUCCESS) {
              /* Error Out */
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
              return GSTK_MEMORY_ERROR;
            }
            (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
            	  sizeof(uim_cmd_ptr->envelope.data)-offset,
                  &STK_envelope_cmd.gstk_address_string_tlv.ss_string,
                  addr_length);
            offset = offset + addr_length;
            /* total length: + ss_string tlv */
            STK_envelope_cmd.length += uint32touint8(addr_length);
            break;

          case GSTK_USSD:
            /* tag */
            uim_cmd_ptr->envelope.data[offset++] =
                STK_envelope_cmd.gstk_address_string_tlv.ussd_string.tag;
            /* length */
            if (STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length >
                    GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
                uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                STK_envelope_cmd.length++;
            }
            uim_cmd_ptr->envelope.data[offset++] =
                STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length;
            /* dcs */
            uim_cmd_ptr->envelope.data[offset++] =
                STK_envelope_cmd.gstk_address_string_tlv.ussd_string.data_coding_scheme;
            /* text */
            /* -1 to account for the TON byte */
            addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length - 1);
            if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset, addr_length) != GSTK_SUCCESS)
            {
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
              return GSTK_MEMORY_ERROR;
            }
            (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
            	  sizeof(uim_cmd_ptr->envelope.data)-offset,
                  STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text,
                  addr_length);
            offset = offset + addr_length;
            STK_envelope_cmd.length += STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length +
                                        GSTK_TAG_LENGTH_LEN;
            break;

          default:
            break;
        }
        /* Copy CDMA Location Info In UIM Cmd Buffer */
        if ((rat == GSTK_ACCESS_TECH_CDMA || rat == GSTK_ACCESS_TECH_HRPD || rat == GSTK_ACCESS_TECH_EHRPD)
            && (offset < UIM_MAX_ENVELOPE_BYTES - 1))
        {
          /* Tag */
          uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.cdma_ext_loc_info.tag;
          /* Length */
          if (STK_envelope_cmd.loc_info.cdma_ext_loc_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
          {
            if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
            { /* Error Out */
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
              return GSTK_MEMORY_ERROR;
            }
            uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
            STK_envelope_cmd.length++;
          }
          if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
          { /* Error Out */
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.cdma_ext_loc_info.length;

          UIM_MSG_HIGH_2("Copy CDMA Location Info Tag 0x%x Len 0x%x",
                         STK_envelope_cmd.loc_info.cdma_ext_loc_info.tag,
                         STK_envelope_cmd.loc_info.cdma_ext_loc_info.length);
          /* Value - CDMA Location Info */
          if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
          { /* Error Out */
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                         STK_envelope_cmd.loc_info.cdma_ext_loc_info.loc_info,
                         (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length,
                         sizeof(uim_cmd_ptr->envelope.data) - offset,
                         (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length) <
             (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length)
          {
            UIM_MSG_ERR_0("gstk_cc_ind(): gstk_memcpy failed");
            gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          offset = offset + STK_envelope_cmd.loc_info.cdma_ext_loc_info.length;
          STK_envelope_cmd.length += STK_envelope_cmd.loc_info.cdma_ext_loc_info.length + GSTK_TAG_LENGTH_LEN;
        }
        else if (rat != GSTK_ACCESS_TECH_WLAN)
        /* copy location info */
        /* tag */
        {
          if(offset < UIM_MAX_ENVELOPE_BYTES)
          {
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.gw_loc_info.tag;
            /* length */
            if (STK_envelope_cmd.loc_info.gw_loc_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            {
              if(offset < UIM_MAX_ENVELOPE_BYTES)
              {
                uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                STK_envelope_cmd.length++;
              }
            }
          }

          if(offset < UIM_MAX_ENVELOPE_BYTES-1)
          {
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.gw_loc_info.length;
            /* location info */
            /* STK_envelope_cmd.location_info.length fixed in packer function no
               chance for buffer overflow */
            if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                           &STK_envelope_cmd.loc_info.gw_loc_info.location_info,
                           (int)STK_envelope_cmd.loc_info.gw_loc_info.length,
                           UIM_MAX_ENVELOPE_BYTES - offset,
                           sizeof(STK_envelope_cmd.loc_info.gw_loc_info.location_info)) <
               (int)STK_envelope_cmd.loc_info.gw_loc_info.length)
            {
              UIM_MSG_ERR_0("gstk_cc_ind(): gstk_memcpy failed");
              gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
              gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
              return GSTK_MEMORY_ERROR;
            }
            offset = offset + STK_envelope_cmd.loc_info.gw_loc_info.length;
            STK_envelope_cmd.length += STK_envelope_cmd.loc_info.gw_loc_info.length + GSTK_TAG_LENGTH_LEN;
          }
        }
        /* copy optional ccp2 */
        if(cc_cmd->cc_data.call_enum_type == GSTK_VOICE)
        {
          if(cc_cmd->cc_data.gstk_address_string.voice.ccp2.length > 0)
          {
            /* tag */
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp2.tag;
            /* length */
            if (STK_envelope_cmd.ccp2.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
            {
              if(offset < UIM_MAX_ENVELOPE_BYTES)
              {
                uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                STK_envelope_cmd.length++;
              }
            }
            if(offset < UIM_MAX_ENVELOPE_BYTES)
            {
              uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp2.length;
            }
            /* ccp2 */
            if (gstk_check_envelope_offset(uim_cmd_ptr, (int) offset,
                                STK_envelope_cmd.ccp2.length) != GSTK_SUCCESS)
            {
              /* Error Out */
              return GSTK_MEMORY_ERROR;
            }
            if(offset < UIM_MAX_ENVELOPE_BYTES)
            {
              (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
              	     sizeof(uim_cmd_ptr->envelope.data)-offset,
                     STK_envelope_cmd.ccp2.ccp,
                     int32touint32(STK_envelope_cmd.ccp2.length));
              offset = offset + STK_envelope_cmd.ccp2.length;
              STK_envelope_cmd.length += STK_envelope_cmd.ccp2.length +
                                        GSTK_TAG_LENGTH_LEN;
            }
          }
        }

        /* populate the envelope command info */
        /* total data length:  device id data + 2 (2 is for the tag and length field)
                      + address/ss/ussd data + 2
                      + location info data + 2
                      + ccp1/ccp2/subaddress
                      address/ss/ussd/ccp1/ccp2/subaddress length has been added ablove
        */
        if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
        {
          /* account for 0x80 */
          uim_cmd_ptr->envelope.offset = 0;
          uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
          uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
          uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
          STK_envelope_cmd.length++;
          uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
        }
        else
        {
          uim_cmd_ptr->envelope.offset = 1;
          uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
          uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
          uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
        }

        gstk_util_dump_byte_array("Env: CC Data",
                                  &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                                  uim_cmd_ptr->envelope.num_bytes);

        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY, gstk_curr_inst_ptr->slot_id) == FALSE)
        {
          gstk_status = gstk_process_and_store_env_cmd_for_retry(
                          uim_cmd_ptr->envelope.data,
                          (uint32)uim_cmd_ptr->envelope.num_bytes,
                          uim_cmd_ptr->hdr.user_data);

          UIM_MSG_HIGH_1("gstk_process_and_store_env_cmd_for_retry() gstk_status = %d",
                         gstk_status);
          if(gstk_status == GSTK_SUCCESS)
          {
            gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state =
              GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
          }
          else
          {
            gstk_status = GSTK_SUCCESS;
          }
        }/* if(!GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY) */
        else
        {
          UIM_MSG_HIGH_0("Call ctrl envelope retry is disabled");
        }
        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
    } /* uim_cmd_ptr != NULL */
    else
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }
  }
  gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);

  return gstk_status;

} /* gstk_cc_ind*/


/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_ind

DESCRIPTION

  This function builds the MO SMS control command and sends it to UIM

PARAMETERS
  mo_sms_ctrl_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_mo_sms_ctrl_ind(
          gstk_envelope_mo_sms_ctrl_ind_type * mo_sms_ctrl_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_mo_sms_ctrl_command_type      STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   uint32                                      offset = 0;
   gstk_location_info_rsp_type                 location_info;
   gstk_slot_id_enum_type                      slot   = GSTK_SLOT_ID_MAX;
   gstk_access_technology_type                 rat    = GSTK_ACCESS_NONE;
#ifdef FEATURE_CDMA
  gstk_cdma_loci_ext_type                      cdma_ext_loc_info; /* For 3GPP2 */
#endif /* FEATURE_CDMA */
  gstk_nv_items_data_type                      nv_data;

   GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

   if(mo_sms_ctrl_cmd == NULL)
   {
     UIM_MSG_ERR_0("mo_sms_ctrl_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   slot = mo_sms_ctrl_cmd->message_header.sim_slot_id;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_mo_sms_ctrl_command_type));
#ifdef FEATURE_CDMA
   memset(&cdma_ext_loc_info, 0xFF, sizeof(gstk_cdma_loci_ext_type));
#endif
   memset(&location_info, 0, sizeof(location_info));

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);
  rat = mo_sms_ctrl_cmd->mo_sms_ctrl_data.rat;

   switch(rat) {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
     case GSTK_ACCESS_TECH_GSM:
     case GSTK_ACCESS_TECH_UTRAN:
     case GSTK_ACCESS_TECH_LTE:
       gstk_status = gstk_get_gwl_loc_info(&location_info,
                                           mo_sms_ctrl_cmd->mo_sms_ctrl_data.rat,
                                           &mo_sms_ctrl_cmd->mo_sms_ctrl_data.loc_info.gwl_info,
                                           slot);

       UIM_MSG_HIGH_1("gstk_mo_sms_ctrl_ind: get_loc_info status=0x%x",
                      gstk_status);
       if(gstk_status != GSTK_SUCCESS)
       {
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
       }
       break;
#endif /*FEATURE_GSM) || FEATURE_WCDMA || FEATURE_LTE*/

     case GSTK_ACCESS_TECH_HRPD:
     case GSTK_ACCESS_TECH_EHRPD:
#ifdef FEATURE_CDMA
       if ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE) &&
           nv_data.cat_version >= GSTK_CFG_CAT_VER6)
       {
         if (!gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid)
         {
           UIM_MSG_ERR_0("No valid HDR loc info!");
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         else
         {
           UIM_MSG_HIGH_0("Use HDR loc info cache for CC");
           cdma_ext_loc_info = gstk_curr_inst_ptr->cdma_ext_loci_cache.data;
         }
       }
       else
       {
         UIM_MSG_ERR_1("cat_version: 0x%x", nv_data.cat_version);
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
       }
#else
       UIM_MSG_ERR_0("FEATURE_CDMA is not defined");
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return gstk_status;
#endif
       break;

     case GSTK_ACCESS_TECH_CDMA:
       if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
       {
#ifdef FEATURE_CDMA
         /* Copy location information from the cmd to cdma_ext_loc_info */
         gstk_status = gstk_copy_1x_location_info(&cdma_ext_loc_info,
                                                  &mo_sms_ctrl_cmd->mo_sms_ctrl_data.loc_info.cdma_info);
         if (gstk_status != GSTK_SUCCESS)
         {
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return gstk_status;
         }
#else
         UIM_MSG_ERR_0("FEATURE_CDMA is not enabled");
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
#endif /* FEATURE_CDMA */
       }
       else
       {
         UIM_MSG_ERR_0("cat_version < GSTK_CFG_CAT_VER6");
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
       }
       break;

     case GSTK_ACCESS_TECH_WLAN:
       break;

     default:
       UIM_MSG_ERR_0 ("Invalid RAT");
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return GSTK_BAD_PARAM;
   }

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_MO_SHORT_MESSAGE_CONTROL_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }


   /* mandatory rp/tp address */

   gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &mo_sms_ctrl_cmd->mo_sms_ctrl_data.rp_dest_address,
       &STK_envelope_cmd.address1);
   if (gstk_status != GSTK_SUCCESS) {
       UIM_MSG_ERR_1("In MO SMS Ctrl: RP Addr Packing Fail 0x%x", gstk_status);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &mo_sms_ctrl_cmd->mo_sms_ctrl_data.tp_dest_address,
       &STK_envelope_cmd.address2);
   if (gstk_status != GSTK_SUCCESS) {
       UIM_MSG_ERR_1("In MO SMS Ctrl: TP Addr Packing Fail 0x%x", gstk_status);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }


   /* Pack location information */
   switch(rat)
   {
     case GSTK_ACCESS_TECH_CDMA:
     case GSTK_ACCESS_TECH_HRPD:
     case GSTK_ACCESS_TECH_EHRPD:
       if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
       {
#ifdef FEATURE_CDMA
         gstk_status = gstk_packer_3gpp2_location_info_tlv(
           &cdma_ext_loc_info,
           &gstk_curr_inst_ptr->cdma_ext_loci_cache,
           &STK_envelope_cmd.loc_info.cdma_ext_loc_info);
         if (gstk_status != GSTK_SUCCESS)
         {
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return gstk_status;
         }
#else
         UIM_MSG_ERR_0("FEATURE_CDMA is not enabled");
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
#endif /* FEATURE_CDMA */
       }
       else
       {
         UIM_MSG_ERR_0("cat_version < GSTK_CFG_CAT_VER6");
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
       }
       break;
     case GSTK_ACCESS_TECH_GSM:
     case GSTK_ACCESS_TECH_UTRAN:
     case GSTK_ACCESS_TECH_LTE:
       gstk_status = gstk_packer_location_info_tlv(
          &location_info, /* comprehension flag = 0! */
          &STK_envelope_cmd.loc_info.gw_loc_info);

       if (gstk_status != GSTK_SUCCESS) {
         UIM_MSG_ERR_1("In MO SMS Ctrl: Location Status Packing Fail 0x%x",
                       gstk_status);
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return gstk_status;
       }
       break;
     case GSTK_ACCESS_TECH_WLAN:
       break;
     default:
       UIM_MSG_ERR_0("Invalid RAT");
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
       return GSTK_ERROR;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = mo_sms_ctrl_cmd->message_header.user_data;

       /* initialized to 3 for value portion of the TLVs
          i.e., the final offset will be either 0 or 1 depends
          on whether the total length is > 0x7F or not */
       offset = 3;

       /* copy device */
       /*STK_envelope_cmd.device_id.device_tag_length is fixed in packer function,
       thus overflow not possible */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &STK_envelope_cmd.device_id,
                                  offset,
                                  (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                           GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.device_id)) <
          (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                           GSTK_TAG_LENGTH_LEN))
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }
       offset = offset + STK_envelope_cmd.device_id.device_tag_length +
                GSTK_TAG_LENGTH_LEN;
       STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                 GSTK_TAG_LENGTH_LEN;

       if(offset >= sizeof(uim_cmd_ptr->envelope.data))
       {
         UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }

       /* copy address1 */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &STK_envelope_cmd.address1,
                                  offset,
                                  GSTK_TAG_LENGTH_LEN,
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.address1)) < GSTK_TAG_LENGTH_LEN)
       {
         UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_byte_offset_memcpy failed");
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }
       STK_envelope_cmd.length += GSTK_TAG_LENGTH_LEN;
       offset = offset + GSTK_TAG_LENGTH_LEN;

       if(offset >= sizeof(uim_cmd_ptr->envelope.data))
       {
         UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }

       /* include length for one extra byte after considering the TON byte. */
       if (STK_envelope_cmd.address1.length - 1 > 0) {
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.address1.ton;
         if(offset >= sizeof(uim_cmd_ptr->envelope.data))
         {
           UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    STK_envelope_cmd.address1.address,
                                    offset,
                                    (size_t)(STK_envelope_cmd.address1.length-1 ),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.address1.address)) <
            (size_t)(STK_envelope_cmd.address1.length-1 ))
         {
           UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_byte_offset_memcpy failed");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         offset = offset + STK_envelope_cmd.address1.length - 1;
         STK_envelope_cmd.length += STK_envelope_cmd.address1.length;
       }

       if(offset >= sizeof(uim_cmd_ptr->envelope.data))
       {
         UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }

       /* copy address2 */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &STK_envelope_cmd.address2,
                                  offset,
                                  GSTK_TAG_LENGTH_LEN,
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.address2)) < GSTK_TAG_LENGTH_LEN)
       {
         UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_byte_offset_memcpy failed");
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }
       STK_envelope_cmd.length += GSTK_TAG_LENGTH_LEN;
       offset = offset + GSTK_TAG_LENGTH_LEN;

       if (STK_envelope_cmd.address2.length > 0)
       {
         if(offset >= sizeof(uim_cmd_ptr->envelope.data))
         {
           UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.address2.ton;

         if(offset >= sizeof(uim_cmd_ptr->envelope.data))
         {
           UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    STK_envelope_cmd.address2.address,
                                    offset,
                                    (size_t)(STK_envelope_cmd.address2.length-1),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.address2.address)) <
            (size_t)(STK_envelope_cmd.address2.length-1))
         {
           UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_byte_offset_memcpy failed");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         offset = offset + STK_envelope_cmd.address2.length - 1;
         STK_envelope_cmd.length += STK_envelope_cmd.address2.length;
       }

       if(offset >= sizeof(uim_cmd_ptr->envelope.data))
       {
         UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
         return GSTK_ERROR;
       }

       /* Copy CDMA Location Info In UIM Cmd Buffer */
       if ((nv_data.cat_version >= GSTK_CFG_CAT_VER6) &&
           (rat == GSTK_ACCESS_TECH_CDMA || rat == GSTK_ACCESS_TECH_HRPD || rat == GSTK_ACCESS_TECH_EHRPD) &&
           (offset < UIM_MAX_ENVELOPE_BYTES - 1))
       {
         /* Tag */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.cdma_ext_loc_info.tag;
         /* Length */
         if (STK_envelope_cmd.loc_info.cdma_ext_loc_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
         {
           if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
           { /* Error Out */
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
             return GSTK_MEMORY_ERROR;
           }
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
         if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
         { /* Error Out */
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_MEMORY_ERROR;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.cdma_ext_loc_info.length;

         UIM_MSG_HIGH_2("Copy CDMA Location Info Tag 0x%x Len 0x%x",
                        STK_envelope_cmd.loc_info.cdma_ext_loc_info.tag,
                        STK_envelope_cmd.loc_info.cdma_ext_loc_info.length);
         /* Value - CDMA Location Info */
         if(offset > UIM_MAX_ENVELOPE_BYTES - 1)
         { /* Error Out */
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_MEMORY_ERROR;
         }
         if(gstk_memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                        STK_envelope_cmd.loc_info.cdma_ext_loc_info.loc_info,
                        (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length,
                        sizeof(uim_cmd_ptr->envelope.data) - offset,
                        (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length) <
            (int)STK_envelope_cmd.loc_info.cdma_ext_loc_info.length)
         {
           UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_memcpy failed");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_MEMORY_ERROR;
         }
         offset = offset + STK_envelope_cmd.loc_info.cdma_ext_loc_info.length;
         STK_envelope_cmd.length += STK_envelope_cmd.loc_info.cdma_ext_loc_info.length + GSTK_TAG_LENGTH_LEN;
       }
       else if(STK_envelope_cmd.loc_info.gw_loc_info.length > 0)
       {
         /* copy location info */
         /* tag */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.gw_loc_info.tag;

         /* length */
         if (STK_envelope_cmd.loc_info.gw_loc_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
         {
           if(offset >= sizeof(uim_cmd_ptr->envelope.data))
           {
             UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
             gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
             uim_cmd_ptr = NULL;
             gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
             return GSTK_ERROR;
           }
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }

         if(offset >= sizeof(uim_cmd_ptr->envelope.data))
         {
           UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.loc_info.gw_loc_info.length;

         if(offset >= sizeof(uim_cmd_ptr->envelope.data))
         {
           UIM_MSG_ERR_1("out of bound offset 0x%x ", offset);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         /* location info */
         /* STK_envelope_cmd.location_info.length is fixed value from packer function
            buffer overflow not possible */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &STK_envelope_cmd.loc_info.gw_loc_info.location_info,
                                    offset,
                                    (size_t)STK_envelope_cmd.loc_info.gw_loc_info.length,
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.loc_info.gw_loc_info.location_info)) <
            (size_t)STK_envelope_cmd.loc_info.gw_loc_info.length)
         {
           UIM_MSG_ERR_0("gstk_mo_sms_ctrl_ind(): gstk_byte_offset_memcpy failed");
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);
           return GSTK_ERROR;
         }
         offset = offset + STK_envelope_cmd.loc_info.gw_loc_info.length;
         STK_envelope_cmd.length += STK_envelope_cmd.loc_info.gw_loc_info.length + GSTK_TAG_LENGTH_LEN;
       }

       /* populate the envelope command info */
       if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
         uim_cmd_ptr->envelope.offset = 0;
         uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
         STK_envelope_cmd.length++;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
       }
       else {
         uim_cmd_ptr->envelope.offset = 1;
         uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
       }

       gstk_util_dump_byte_array("Env: MO SMS control Data",
                                 &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                                 uim_cmd_ptr->envelope.num_bytes);

       if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY,
                                     gstk_curr_inst_ptr->slot_id) == FALSE)
       {
         gstk_status = gstk_process_and_store_env_cmd_for_retry(
                        uim_cmd_ptr->envelope.data,
                        (uint32)uim_cmd_ptr->envelope.num_bytes,
                        uim_cmd_ptr->hdr.user_data);
         if( gstk_status == GSTK_SUCCESS)
         {
           gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
         }
         else
         {
           UIM_MSG_HIGH_0("MO SMS control Env - no retry will be attempted ");
           gstk_status = GSTK_SUCCESS;
         }
       }/* if(!GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY) */
       else
       {
         UIM_MSG_HIGH_0("Call ctrl envelope retry is disabled");
       }

       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd, &STK_envelope_cmd);

   return gstk_status;

} /* gstk_mo_sms_ctrl_ind */


/*===========================================================================
FUNCTION gstk_nw_search_mode_evt_ind

DESCRIPTION

  This function builds the network search mode command and sends it to UIM

PARAMETERS
  nw_search_mode_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_nw_search_mode_evt_ind(
  const gstk_envelope_nw_search_mode_evt_ind_type * nw_search_mode_cmd
)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   uim_cmd_type                                            *uim_cmd_ptr;
   int32                                                   offset = 0;
   gstk_evt_list_code_enum_type                            evt_list_info_data;
   gstk_nw_search_mode_enum_type                           nw_search_mode_data;
   gstk_device_id_type                                     dev_id;

   UIM_MSG_HIGH_0("In gstk_nw_search_mode_evt_ind");

   if(nw_search_mode_cmd == NULL)
   {
     UIM_MSG_ERR_0("nw_search_mode_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   nw_search_mode_data =
     (gstk_nw_search_mode_enum_type)nw_search_mode_cmd->nw_search_mode;

   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr == NULL )
   {
     return GSTK_MEMORY_ERROR;
   }

   gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
   if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL))
   {
     UIM_MSG_HIGH_0("ERR status is 0x%x or uim_cmd_ptr is NULL");
     return gstk_status;
   }
   /* set to 0 so that uim will get the data from index 0 */
   uim_cmd_ptr->envelope.offset = 0x00;
   /* set user_data */
   uim_cmd_ptr->hdr.user_data   = nw_search_mode_cmd->message_header.user_data;

   /* populate the envelope command info */
   uim_cmd_ptr->envelope.num_bytes = GSTK_EVT_LIST_VAL_LEN + /* set up event list */
                                     GSTK_DEVICE_IDENTITY_LEN +
                                     GSTK_NW_SEARCH_MODE_LEN +
                                     (GSTK_TAG_LENGTH_LEN * 3) +
                                     GSTK_TAG_LENGTH_LEN;

   uim_cmd_ptr->envelope.data[offset++] = GSTK_EVENT_DOWNLOAD_TAG;

   /* total data length:  evt id data + evt list tag len+ evt list len len
                          + device id data + dev id tag len + dev id len len
                          + nw search mode data + nw search mode tag len
                          + nw search mode len len
   */
   uim_cmd_ptr->envelope.data[offset++] = GSTK_EVT_LIST_VAL_LEN + /* set up event list */
                                          GSTK_DEVICE_IDENTITY_LEN +
                                          GSTK_NW_SEARCH_MODE_LEN +
                                          (GSTK_TAG_LENGTH_LEN * 3);

   evt_list_info_data = GSTK_NW_SEARCH_MODE;

   gstk_status = gstk_util_populate_tlv(
                       GSTK_EVENT_LIST_TAG,
                       offset,
                       (gstk_cmd_to_uim_type*)&evt_list_info_data,
                       uim_cmd_ptr->envelope.data,
                       &offset,
                       GSTK_ENVELOPE_CMD); /*lint !e826 evt_list_info_data suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }
   /* copy device */
   dev_id.src = GSTK_ME_DEVICE;
   dev_id.dest = GSTK_UICC_SIM_DEVICE;
   gstk_status = gstk_util_populate_tlv(
                   GSTK_DEVICE_IDENTITY_TAG,
                   offset,
                   (gstk_cmd_to_uim_type*)&dev_id,
                   uim_cmd_ptr->envelope.data,
                   &offset,
                   GSTK_ENVELOPE_CMD); /*lint !e826 dev_id suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }

   /* network search mode tag */
   gstk_status = gstk_util_populate_tlv(
                   GSTK_NW_SEARCH_MODE_TAG,
                   offset,
                   (gstk_cmd_to_uim_type*)&nw_search_mode_data,
                   uim_cmd_ptr->envelope.data,
                   &offset,
                   GSTK_ENVELOPE_CMD); /*lint !e826 nw_search_mode_data suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }

   gstk_util_dump_byte_array("Env: Network Search Mode evt",
                             &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                             uim_cmd_ptr->envelope.num_bytes);

   gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
   return gstk_status;
}


/*===========================================================================
FUNCTION gstk_timer_expire_ind

DESCRIPTION

  This function builds the timer expire command and sends it to UIM

PARAMETERS
  timer_expire_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_timer_expire_ind (
  const gstk_envelope_timer_expire_ind_type * timer_expire_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_timer_expiration_command_type STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;

   if(timer_expire_cmd == NULL)
   {
     UIM_MSG_ERR_0("timer_expire_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_timer_expiration_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_TIMER_EXPIRY_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* mandatory timer identifier */
   gstk_status = gstk_packer_timer_id_tlv(
       &timer_expire_cmd->timer_info.timer_id,
       &STK_envelope_cmd.timer_id);
   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* mandatory timer value */
   gstk_status = gstk_packer_timer_value_tlv(
       &timer_expire_cmd->timer_info.time_value,
       &STK_envelope_cmd.timer_value);
   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + timer_id + 2
                        + timer_value + 2

   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.timer_id.length +
                             STK_envelope_cmd.timer_value.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = timer_expire_cmd->message_header.user_data;

       /* required so that uim will grap the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy device */
       /* STK_envelope_cmd.device_id.device_tag_length fixed value from packer function
          overflow not possible */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.device_id),
                                  offset,
                                  (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                           GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.device_id)) <
          (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy timer id */
       /* STK_envelope_cmd.timer_id.length fixed value from packer function,
          buffer overflow not possible */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.timer_id),
                                       offset += STK_envelope_cmd.device_id.device_tag_length +
                                                 GSTK_TAG_LENGTH_LEN,
                                       (size_t)(STK_envelope_cmd.timer_id.length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.timer_id)) <
               (size_t)(STK_envelope_cmd.timer_id.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy timer value */
       /* STK_envelope_cmd.timer_value.length fixed value from packer function,
          buffer overflow not possible */
       offset += (int)(STK_envelope_cmd.timer_id.length + GSTK_TAG_LENGTH_LEN);
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                        &(STK_envelope_cmd.timer_value),
                                        offset,
                                        (size_t)(STK_envelope_cmd.timer_value.length +
                                                 GSTK_TAG_LENGTH_LEN),
                                        sizeof(uim_cmd_ptr->envelope.data) - offset,
                                        sizeof(STK_envelope_cmd.timer_value)) <
                (size_t)(STK_envelope_cmd.timer_value.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       offset = (int)(offset + STK_envelope_cmd.timer_value.length +
                      GSTK_TAG_LENGTH_LEN);

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Env: Timer Expire Data",
                    &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset], 
                    uim_cmd_ptr->envelope.num_bytes);

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   /* no need to free envelope timer expire ind and tlv data */
   return gstk_status;

} /* gstk_timer_expire_ind */


/*===========================================================================
FUNCTION gstk_requeue_envelope_command

DESCRIPTION

  This function requeues the envelope command to the back of the gstk queue

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_requeue_envelope_command(
  const gstk_cmd_type *cmd_ptr)
{
  gstk_status_enum_type     gstk_status       = GSTK_SUCCESS;
  uint32                    prev_env_ref_id   = 0;
  uint32                    user_data         = 0;
  gstk_evt_cb_funct_type    gstk_sms_pp_dl_cb = NULL;
  uint32                    type_of_command   = 0;
  gstk_slot_id_enum_type    slot              = GSTK_SLOT_ID_MAX;

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd NULL ptr");
    return GSTK_BAD_PARAM;
  }

  type_of_command = cmd_ptr->general_cmd.message_header.command_id;
  prev_env_ref_id = cmd_ptr->sms_pp_envelope_ind.message_header.user_data;
  slot            = cmd_ptr->sms_pp_envelope_ind.message_header.sim_slot_id;

  UIM_MSG_HIGH_1(" In gstk_requeue_envelope_command(): Type of Command 0x%x ",
                 type_of_command);

  switch(type_of_command)
  {
    case GSTK_SMS_PP_DOWNLOAD_IND:
      gstk_status = gstk_util_retrieve_env_cmd_info(GSTK_SMS_PP_DOWNLOAD_IND_RSP,
                                                    &user_data,
                                                    &gstk_sms_pp_dl_cb,
                                                    prev_env_ref_id);
      if(gstk_status == GSTK_SUCCESS)
      {
        gstk_status = gstk_slot_send_envelope_sms_pp_dl_command(slot,
                                user_data,
                                cmd_ptr->sms_pp_envelope_ind.is_cdma_sms_pp,
                                &(cmd_ptr->sms_pp_envelope_ind.address),
                                &(cmd_ptr->sms_pp_envelope_ind.tpdu),
                                gstk_sms_pp_dl_cb);
      }
    break;
  default:
    break;
  }
  return gstk_status;
}


/*===========================================================================
FUNCTION gstk_process_envelope_cmd

DESCRIPTION

  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope command tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_process_envelope_cmd(
  gstk_cmd_type* cmd
)
{
  gstk_status_enum_type               gstk_status                  = GSTK_SUCCESS;
  uint32                              type_of_command              = 0;
  boolean                             error_env_flg                = FALSE;
  boolean                             sms_error_env_flg            = FALSE;
  uint32                              env_ref_id                   = GSTK_CMD_DETAILS_REF_TABLE_FULL;
  gstk_nv_items_data_type             nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd == NULL)
  {
    UIM_MSG_ERR_0("cmd ERR : NULL");
    return GSTK_BAD_PARAM;
  }

  /* Get the type of command */
  type_of_command = cmd->general_cmd.message_header.command_id;
  UIM_MSG_HIGH_1("gstk_process_envelope_cmd: command = 0x%x", type_of_command);

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* get the envelope refernce id */
  gstk_status =  gstk_util_save_env_cmd_info(cmd->general_cmd.message_header.client_id,
                                             (gstk_cmd_enum_type)type_of_command,
                                             cmd,
                                             &env_ref_id);
  if((GSTK_MEMORY_ERROR == gstk_status)
     && (GSTK_MAX_PROACTIVE_COMMAND == env_ref_id))
  {
    UIM_MSG_ERR_1("Failed to Save Envelope Info: status = 0x%x", gstk_status);

    /* Send error report to clients so they can do proper memory clean-up */
    gstk_status = gstk_build_error_envelope_report(env_ref_id,
                                                   cmd->general_cmd.message_header.command_id,
                                                   GSTK_ENVELOPE_CMD_FAIL);
    return gstk_status;
  }

  /* user_data is already stored as part of envelope info, use this field
     to store env_ref_id for future use */
  cmd->general_cmd.message_header.user_data = env_ref_id;

  /* Check the proactive command table for proactive command still
   ** being provessed by card
   ** If TRUE, do not send down any envelopes to the card. Inform the client
   ** that the envelope could not be processed
   */

  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_STOP_ENV_CMD_PROCESSING, gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    error_env_flg = gstk_check_for_pending_pro_cmd();
  }

  switch( type_of_command )
  {
    case GSTK_MENU_SELECTION_IND:
      UIM_MSG_HIGH_0(" Rec'd Menu Selection :GSTK_MENU_SELECTION_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_menu_selection_ind(&(cmd->menu_selection_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->menu_selection_envelope_ind.message_header.user_data,
                                                       cmd->menu_selection_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_RAW_ENVELOPE_IND:
      UIM_MSG_HIGH_0(" Rec'd Raw Envelope: GSTK_RAW_ENVELOPE_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */

      if(env_ref_id <= GSTK_MAX_PROACTIVE_COMMAND)
      {
        if(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].evt_list_code != ((0xFF)))
        {
          UIM_MSG_ERR_1("Raw Event list code: 0x%x is not cleared",
                  gstk_shared_data.envelope_cmd_ref_table[env_ref_id].evt_list_code);
        }
      }

      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_raw_envelope_ind(&(cmd->raw_envelope_ind));
      }

      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        if (gstk_status == GSTK_BAD_PARAM || gstk_status == GSTK_UNSUPPORTED_COMMAND)
        {
          gstk_status = gstk_build_error_envelope_report(cmd->raw_envelope_ind.message_header.user_data,
                                                         cmd->raw_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_FAIL);
        }
        else
        {
          gstk_status = gstk_build_error_envelope_report(cmd->raw_envelope_ind.message_header.user_data,
                                                         cmd->raw_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
      }
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND:
      UIM_MSG_HIGH_0(" Rec'd PP DL:GSTK_SMS_PP_DOWNLOAD_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE && sms_error_env_flg ==  FALSE)
      {
        gstk_status = gstk_sms_pp_dl_ind(&(cmd->sms_pp_envelope_ind));
      }
      if (gstk_status == GSTK_SUCCESS)
      {
         mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
         mmgsdi_status = mmgsdi_start_potential_silent_file_change(
                           gstk_shared_data.gstk_mmgsdi_client_id,
                           (mmgsdi_slot_id_enum_type) gstk_curr_inst_ptr->slot_id);
         UIM_MSG_MED_1("mmgsdi_status: 0x%x", mmgsdi_status);
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS) ||
         (sms_error_env_flg == TRUE))
      {
        gstk_status = gstk_requeue_envelope_command(cmd);
        if(gstk_status == GSTK_NULL_INPUT_PARAM)
        {
          UIM_MSG_HIGH_0("Null params, do not requeue");
          gstk_status = gstk_build_error_envelope_report(cmd->sms_pp_envelope_ind.message_header.user_data,
                                                         cmd->sms_pp_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_FAIL);
        }
        else if(gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_HIGH_0("Requeue of sms pp envelope command failed");
          gstk_status = gstk_build_error_envelope_report(cmd->sms_pp_envelope_ind.message_header.user_data,
                                                         cmd->sms_pp_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
        else
        {
          /* Free the reference id in the envelope table for the old command */
          gstk_util_env_cmd_ref_table_free_slot(cmd->sms_pp_envelope_ind.message_header.user_data);
          /* Free the upper layer command */
          gstk_sms_pp_dl_ind_cleanup(&(cmd->sms_pp_envelope_ind));
        }
      }
      break;
    case GSTK_MT_CALL_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd ME Call Evt: GSTK_MT_CALL_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_mt_call_evt_ind(&(cmd->mt_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->mt_evt_envelope_ind.message_header.user_data,
                                                       cmd->mt_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CALL_CONNECTED_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Call connected Evt: GSTK_CALL_CONNECTED_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_call_connected_evt_ind(&(cmd->call_connected_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->call_connected_envelope_ind.message_header.user_data,
                                                       cmd->call_connected_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CALL_DISCONNECTED_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Call disconnected Evt :GSTK_CALL_DISCONNECTED_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_call_disconnected_evt_ind(&(cmd->call_disconnected_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->call_disconnected_envelope_ind.message_header.user_data,
                                                       cmd->call_disconnected_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_IDLE_SCRN_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Idle Scrn Evt: GSTK_IDLE_SCRN_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_idle_scrn_evt_ind(&(cmd->idle_scrn_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->idle_scrn_evt_envelope_ind.message_header.user_data,
                                                       cmd->idle_scrn_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_LOCATION_STATUS_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Location Statuc Evt:GSTK_LOCATION_STATUS_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_location_status_evt_ind(&(cmd->location_st_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->location_st_envelope_ind.message_header.user_data,
                                                       cmd->location_st_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      gstk_curr_inst_ptr->gstk_mm_param_received = TRUE;
      break;
    case GSTK_ACCESS_TECH_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Access Technology Evt:GSTK_ACCESS_TECH_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
         gstk_status = gstk_access_tech_evt_ind(&(cmd->access_tech_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        UIM_MSG_HIGH_0("Building Access Tech error envelope report");
        gstk_status = gstk_build_error_envelope_report(cmd->access_tech_evt_envelope_ind.message_header.user_data,
                                                       cmd->access_tech_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_USER_ACTIVITY_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd User Activity Evt: GSTK_USER_ACTIVITY_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_user_act_evt_ind(&(cmd->user_act_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->user_act_evt_envelope_ind.message_header.user_data,
                                                       cmd->user_act_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_LANG_SELECTION_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Lang Sel Evt: GSTK_LANG_SELECTION_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_lang_selection_evt_ind(&(cmd->lang_sel_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->lang_sel_evt_envelope_ind.message_header.user_data,
                                                       cmd->lang_sel_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_BROWSER_TERMINATION_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd Browser Term Evt: GSTK_BROWSER_TERMINATION_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_browser_term_evt_ind(&(cmd->browser_term_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->browser_term_evt_envelope_ind.message_header.user_data,
                                                       cmd->browser_term_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_DATA_AVAIL_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd DATA AVAIL Evt: GSTK_DATA_AVAIL_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_data_avail_evt_ind(&(cmd->data_avail_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->data_avail_evt_envelope_ind.message_header.user_data,
                                                       cmd->data_avail_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CH_STATUS_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd CHANNEL STATUS Evt: GSTK_CH_STATUS_EVT_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_ch_status_evt_ind(&(cmd->ch_status_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->ch_status_evt_envelope_ind.message_header.user_data,
                                                       cmd->ch_status_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_MO_SMS_CTRL_IND:
      UIM_MSG_HIGH_0(" Rec'd MO SMS Ctrl: GSTK_MO_SMS_CTRL_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_mo_sms_ctrl_ind(&(cmd->mo_sms_ctrl_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        gstk_status = gstk_build_error_envelope_report(cmd->mo_sms_ctrl_envelope_ind.message_header.user_data,
                                                       cmd->mo_sms_ctrl_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CC_IND:
      UIM_MSG_HIGH_0(" Rec'd Call ctrl: GSTK_CC_IND");
      if((gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress) &&
         (gstk_curr_inst_ptr->gstk_cmd_cache.tr_rcvd == TRUE))
      {
         /* Pending Response and TR is received: Send the TR first
            only if terminal_rsp_delay_timer is running so that we can process CC */
         if(rex_get_timer(&(gstk_curr_inst_ptr->gstk_terminal_rsp_delay_timer)) > 0)
         {
           (void)rex_clr_timer(&gstk_curr_inst_ptr->gstk_terminal_rsp_delay_timer);
           gstk_send_cached_terminal_rsp(cmd->cc_envelope_ind.message_header.sim_slot_id);
         }
      }
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        /* cache CC info */
        gstk_memcpy(&(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].gstk_cc_cache_info),
                   &(cmd->cc_envelope_ind.cc_data),
                   sizeof(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].gstk_cc_cache_info),
                   sizeof(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].gstk_cc_cache_info),
                   sizeof(cmd->cc_envelope_ind.cc_data));        
        gstk_status = gstk_cc_ind(&(cmd->cc_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_util_clear_retry_envelope_data(
          gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
        gstk_status = gstk_build_error_envelope_report(cmd->cc_envelope_ind.message_header.user_data,
                                                       cmd->cc_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_TIMER_EXPIRE_IND:
      UIM_MSG_HIGH_0(" Rec'd Timer expire: GSTK_TIMER_EXPIRE_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_timer_expire_ind(&(cmd->timer_expire_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->timer_expire_envelope_ind.message_header.user_data,
                                                       cmd->timer_expire_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      UIM_MSG_HIGH_0(" Rec'd Cell Broadcast DL: GSTK_CELL_BROADCAST_DOWNLOAD_IND");
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_cell_broadcast_dl_ind(&(cmd->cell_broadcast_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->cell_broadcast_envelope_ind.message_header.user_data,
                                                       cmd->cell_broadcast_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_NW_SEARCH_MODE_EVT_IND:
      UIM_MSG_HIGH_0(" Rec'd NW Search Mode evt ind: GSTK_NW_SEARCH_MODE_EVT_IND");
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
        /*lint -e774 error_env_flg always evaluates to FALSE */
        if(error_env_flg == FALSE)
        {
          gstk_status = gstk_nw_search_mode_evt_ind(&(cmd->nw_search_mode_evt_envelope_ind));
        }
        if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
        {
          gstk_status = gstk_build_error_envelope_report(cmd->nw_search_mode_evt_envelope_ind.message_header.user_data,
                                                         cmd->nw_search_mode_evt_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
      }
      else
      {
        UIM_MSG_HIGH_1("GSTK_NW_SEARCH_MODE_EVT_IND : CAT version is pre-VER6 - 0x%x",
                       nv_data.cat_version);
      }
      break;
    case GSTK_LOCATION_INFORMATION_IND:
      UIM_MSG_HIGH_0(" Rec'd envelope GSTK_LOCATION_INFORMATION_IND");
      if (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data !=
          GSTK_MAX_PROACTIVE_COMMAND)
      {
        /*lint -e774 error_env_flg always evaluates to FALSE */
        if(error_env_flg == FALSE)
        {
          gstk_status = gstk_location_information_ind(&(cmd->location_information_envelope_ind));
        }
        if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
        {
          gstk_status = gstk_build_error_envelope_report(cmd->location_information_envelope_ind.message_header.user_data,
                                                         cmd->location_information_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
      }
      else
      {
        gstk_util_env_cmd_ref_table_free_slot(
          cmd->location_information_envelope_ind.message_header.user_data);
        UIM_MSG_HIGH_2("Attach ctrl: sig_to_wait 0x%x,  task_tcb 0x%x",
                       cmd->attach_ctrl_ind.attach_ctrl.sig_to_wait,
                       cmd->attach_ctrl_ind.attach_ctrl.task_tcb);
        UIM_MSG_HIGH_3("MCC/MNC: 0x%x 0x%x 0x%x",
                       cmd->attach_ctrl_ind.attach_ctrl.mcc_mnc[0],
                       cmd->attach_ctrl_ind.attach_ctrl.mcc_mnc[1],
                       cmd->attach_ctrl_ind.attach_ctrl.mcc_mnc[2]);

        gstk_free(gstk_curr_inst_ptr->attach_ctrl_p);
        gstk_curr_inst_ptr->attach_ctrl_p =
          (gstk_attach_ctrl_type *)GSTK_CALLOC(sizeof(gstk_attach_ctrl_type));
        /* Null Pointer check for gstk_curr_inst_ptr->attach_ctrl_p */
        if (gstk_curr_inst_ptr == NULL || gstk_curr_inst_ptr->attach_ctrl_p == NULL)
        {
          (void)rex_set_sigs(
                  (rex_tcb_type *)cmd->attach_ctrl_ind.attach_ctrl.task_tcb,
                  (rex_sigs_type)cmd->attach_ctrl_ind.attach_ctrl.sig_to_wait);
        }
        else
        {
          (void)gstk_memcpy(gstk_curr_inst_ptr->attach_ctrl_p,
                         &cmd->attach_ctrl_ind.attach_ctrl,
                         sizeof(gstk_attach_ctrl_type),
                         sizeof(gstk_attach_ctrl_type),
                         sizeof(gstk_attach_ctrl_type));

          if (gstk_nv_get_feature_status(
                GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
                gstk_curr_inst_ptr->slot_id) == FALSE)
          {
            UIM_MSG_HIGH_0("FEATURE_PROP_LOC_STATUS is disabled");
            gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
            gstk_util_send_attach_ctrl_res();
          }
          else
          {
            env_ref_id = gstk_curr_inst_ptr->in_prog_cmd_ref_id;
            if((env_ref_id != GSTK_MAX_PROACTIVE_COMMAND) /*pro cmd in progress*/
               &&
               (command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(env_ref_id)].command_details.type_of_command ==
                GSTK_CMD_STK_REFRESH)
              )

            {
              UIM_MSG_HIGH_1("REFRESH in progress, subs_ready_gw 0x%x",
                             gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw);

              if (gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw)
              {
                gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
              }
              else
              {
                gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
              }
              gstk_util_send_attach_ctrl_res();
            }
            else
            {
              (void)gstk_nv_access_read(GSTK_NV_SUB_ATTACH_CTRL_TIMER,
                                        gstk_curr_inst_ptr->slot_id,
                                        &nv_data);

              if (memcmp(gstk_curr_inst_ptr->gstk_last_loc_info_sent,
                         gstk_curr_inst_ptr->attach_ctrl_p->mcc_mnc,
                         GSTK_MAX_PLMN_LEN) != 0)
              {
                UIM_MSG_HIGH_0("gstk_last_loc_info_sent and attach_ctrl_p->mcc_mnc doesn't match");
                if (gstk_send_envelope_loc_info_command(
                      gstk_curr_inst_ptr->client_id,
                      gstk_curr_inst_ptr->attach_ctrl_p->mcc_mnc,
                      0) != GSTK_SUCCESS)
                {
                  /* failed to send envelope, let's continue ATTACH */
                  UIM_MSG_ERR_0("Failed to send attach ctrl!");
                  gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
                  gstk_util_send_attach_ctrl_res();
                }
                else
                {
                  (void)rex_clr_timer(&gstk_curr_inst_ptr->attach_ctrl_timer);
                  (void)rex_set_timer(&gstk_curr_inst_ptr->attach_ctrl_timer,
                                      nv_data.attach_ctrl_timer_val);
                }
              }
              else
              {
                UIM_MSG_HIGH_1("same loc info env is already sent loc_info_env_res 0x%x",
                               gstk_curr_inst_ptr->loc_info_env_res);
                /* same loc info env is already sent */
                if(gstk_curr_inst_ptr->loc_info_env_res !=
                   GSTK_ATTACH_CTRL_INVALID)
                {
                  gstk_curr_inst_ptr->attach_ctrl_res =
                    gstk_curr_inst_ptr->loc_info_env_res;
                  gstk_util_send_attach_ctrl_res();
                }
                else
                {
                  (void)rex_clr_timer(&gstk_curr_inst_ptr->attach_ctrl_timer);
                  (void)rex_set_timer(&gstk_curr_inst_ptr->attach_ctrl_timer,
                                      nv_data.attach_ctrl_timer_val);
                }
              }
            }
          }
        }
      }
      break;
    case GSTK_NW_REJ_EVT_IND:
      UIM_MSG_HIGH_3("Receive NW REJ event: 0x%x, 0x%x, 0x%x",
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_NW_REJECTION],
                     cmd->nw_rej_ind.nw_rej_evt.nw_rej_type,
                     cmd->nw_rej_ind.nw_rej_evt.rat);
      /* Cache the RAT */
      if (!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)cmd->nw_rej_ind.nw_rej_evt.rat,
                                  &gstk_curr_inst_ptr->nw_rej.access_tech))
      {
        UIM_MSG_ERR_1("invalid rat 0x%x!", cmd->nw_rej_ind.nw_rej_evt.rat);
        return GSTK_ERROR;
      }
      /* Cache the PLMN*/
      gstk_memcpy(gstk_curr_inst_ptr->nw_rej.mcc_mnc, /* dest_ptr */
                  cmd->nw_rej_ind.nw_rej_evt.mcc_mnc, /* src_ptr */
                  GSTK_MAX_PLMN_LEN, /* copy_size */
                  GSTK_MAX_PLMN_LEN, /* dest_max_size */
                  GSTK_MAX_PLMN_LEN); /* src_max_size */
      /* Cache the LAC/TAC */
      gstk_memcpy(gstk_curr_inst_ptr->nw_rej.lac, /* dest_ptr */
                  cmd->nw_rej_ind.nw_rej_evt.lac, /* src_ptr */
                  GSTK_MAX_LAC_LEN, /* copy_size */
                  GSTK_MAX_LAC_LEN, /* dest_max_size */
                  GSTK_MAX_LAC_LEN); /* src_max_size */

      if (gstk_curr_inst_ptr->gstk_evt_list[GSTK_NW_REJECTION] & 0x01)
      {
        gstk_status = gstk_nw_rej_evt_ind(&(cmd->nw_rej_ind));
      }
      else
      {
        gstk_util_env_cmd_ref_table_free_slot(
          cmd->nw_rej_ind.message_header.user_data);
      }
      break;
    case GSTK_HCI_CONNECTIVITY_IND:
      UIM_MSG_HIGH_1("Receive HCI Connectivity event: 0x%x",
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_HCI_CONNECTIVITY]);
      if (gstk_curr_inst_ptr->gstk_evt_list[GSTK_HCI_CONNECTIVITY] & 0x01)
      {
        gstk_status = gstk_hci_connectivity_evt_ind(&(cmd->hci_evt_envelope_ind));
      }
      else
      {
        gstk_util_env_cmd_ref_table_free_slot(
          cmd->hci_evt_envelope_ind.message_header.user_data);
      }
      break;
    case GSTK_CSG_CELL_SELECTION_EVT_IND:
      UIM_MSG_HIGH_1("Received CSG cell selection evt: 0x%x",
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_CSG_CELL_SELECTION]);
      if(gstk_curr_inst_ptr->gstk_evt_list[GSTK_CSG_CELL_SELECTION] & 0x01)
      {
        gstk_status = gstk_csg_evt_ind(&(cmd->csg_change_ind));
      }
      else
      {
        gstk_util_env_cmd_ref_table_free_slot(
          cmd->csg_change_ind.message_header.user_data);
      }
      break;
    case GSTK_IMS_REG_EVT_IND:
      UIM_MSG_HIGH_1("Receive IMS Registration event: 0x%x",
                     gstk_curr_inst_ptr->gstk_evt_list[GSTK_IMS_REGISTRATION]);

      if(nv_data.cat_version >= GSTK_CFG_CAT_VER8)
      {
        /*lint -e774 error_env_flg always evaluates to FALSE */
        if(error_env_flg == FALSE)
        {
          gstk_status = gstk_ims_registration_evt_ind(&(cmd->ims_reg_evt_envelope_ind));
        }
        if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
        {
          gstk_status = gstk_build_error_envelope_report(cmd->ims_reg_evt_envelope_ind.message_header.user_data,
                                                         cmd->ims_reg_evt_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
      }
      else
      {
        UIM_MSG_HIGH_1("GSTK_IMS_REG_EVT_IND : CAT version is pre-VER8 - 0x%x",
                       nv_data.cat_version);
      }
      break;
    default:
      gstk_util_env_cmd_ref_table_free_slot(
        cmd->location_information_envelope_ind.message_header.user_data);
      UIM_MSG_ERR_0(" Rec'd unknown Envelope");
      break;
    }
    return gstk_status;
} /* gstk_process_envelope_cmd*/


/*===========================================================================
FUNCTION   gstk_build_access_technology_envelope

DESCRIPTION
  This function builds the envelope for access technology event.

PARAMETERS
  access tech :- Type of access technology used

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
gstk_status_enum_type gstk_build_access_technology_envelope(
    gstk_access_technology_type access_tech
)
{
  gstk_task_cmd_type    * task_cmd    = NULL;
  uint32                  user_data   = 0;

  UIM_MSG_HIGH_1("Access Technology event download, Access Tech is 0x%x",
                 access_tech);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if ((access_tech != GSTK_ACCESS_TECH_GSM) &&
      (access_tech != GSTK_ACCESS_TECH_UTRAN) &&
      (access_tech != GSTK_ACCESS_TECH_CDMA) &&
      (access_tech != GSTK_ACCESS_TECH_LTE) &&
      (access_tech != GSTK_ACCESS_TECH_HRPD) &&
      (access_tech != GSTK_ACCESS_TECH_EHRPD)
     )
  {
      return GSTK_ERROR;
  }

  /* set user_data as the access technology value*/
  user_data = (uint32) access_tech;

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd == NULL )
  {
    return GSTK_ERROR;
  }
  /* successfully obtained command buffer - build the envelope command */
  task_cmd->cmd.access_tech_evt_envelope_ind.message_header.client_id = gstk_curr_inst_ptr->client_id;
  task_cmd->cmd.access_tech_evt_envelope_ind.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.access_tech_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.access_tech_evt_envelope_ind.message_header.command_id    = (int)GSTK_ACCESS_TECH_EVT_IND;
  task_cmd->cmd.access_tech_evt_envelope_ind.message_header.user_data     = user_data;
  /* timer info */
  task_cmd->cmd.access_tech_evt_envelope_ind.access_tech = (int)access_tech;

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  UIM_MSG_HIGH_0("Building Access Tech envelope");
  if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    gstk_task_put_cmd_buf_to_head(task_cmd);
  }
  else
  {
    gstk_task_put_cmd_buf(task_cmd);
  }
  return GSTK_SUCCESS;
} /* gstk_build_access_technology_envelope */


/*===========================================================================
FUNCTION   gstk_build_location_status_envelope

DESCRIPTION
  This function builds the envelope for location status event.

PARAMETERS
  access tech :- Type of access technology used
  location status :- GSTK_NORMAL_SERVICE
                     GSTK_NO_SERVICE
                     GSTK_LIMITED_SERVICE

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
gstk_status_enum_type gstk_build_location_status_envelope(
    gstk_access_technology_type    access_tech,
    gstk_location_status_enum_type location_status
)
{
  gstk_task_cmd_type    * task_cmd  = NULL;
  uint32                  user_data   = 0;

  UIM_MSG_HIGH_1("Location status event download, Access Tech is 0x%x",
                 access_tech);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
#ifdef FEATURE_CDMA
  if ((gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == FALSE) &&
      ((access_tech == GSTK_ACCESS_TECH_CDMA) ||
       (access_tech == GSTK_ACCESS_TECH_HRPD) ||
       (access_tech == GSTK_ACCESS_TECH_EHRPD)) &&
      (!gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid))
  {
    UIM_MSG_ERR_0("CDMA Loc Info invalid! Ignore loc status env.");
    return GSTK_ERROR;
  }
#endif
  /* set user_data as the access technology value*/
  user_data = (uint32) access_tech;

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd == NULL )
  {
    return GSTK_ERROR;
  }
  /* successfully obtained command buffer - build the envelope command */
  task_cmd->cmd.location_st_envelope_ind.message_header.client_id = gstk_curr_inst_ptr->client_id;
  task_cmd->cmd.location_st_envelope_ind.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.location_st_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.location_st_envelope_ind.message_header.command_id    = int32touint32((int)GSTK_LOCATION_STATUS_EVT_IND);
  task_cmd->cmd.location_st_envelope_ind.message_header.user_data     = user_data;
  /* location status info */
  task_cmd->cmd.location_st_envelope_ind.location_status              = int32touint8((int)location_status);
  switch(access_tech)
  {
    case GSTK_ACCESS_TECH_GSM:
    case GSTK_ACCESS_TECH_UTRAN:
    case GSTK_ACCESS_TECH_LTE:
      task_cmd->cmd.location_st_envelope_ind.location_info.info_type    = GSTK_GSM_UMTS_LOCATION_INFO;
      break;
    case GSTK_ACCESS_TECH_CDMA:
    case GSTK_ACCESS_TECH_HRPD:
    case GSTK_ACCESS_TECH_EHRPD:
      task_cmd->cmd.location_st_envelope_ind.location_info.info_type    = GSTK_CDMA_LOCATION_INFO;
      break;
    case GSTK_ACCESS_NONE:
      task_cmd->cmd.location_st_envelope_ind.location_info.info_type    = GSTK_NO_SRVC_LOCATION_INFO;
      break;
    default:
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_ERROR;
  }

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
  {
    gstk_task_put_cmd_buf_to_head(task_cmd);
  }
  else
  {
    gstk_task_put_cmd_buf(task_cmd);
  }
  return GSTK_SUCCESS;
} /* gstk_build_location_status_envelope */

/*===========================================================================
FUNCTION   gstk_build_nw_search_mode_change_envelope

DESCRIPTION
  This function builds the envelope for network search mode event.

PARAMETERS
  ph_nw_sel_pref :- Preferred network selection mode used.

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
gstk_status_enum_type gstk_build_nw_search_mode_change_envelope(
    gstk_nw_search_mode_enum_type  ph_nw_sel_pref
)
{

  gstk_task_cmd_type    * task_cmd  = NULL;

  UIM_MSG_HIGH_1("Network Search Mode Change event download, ph_nw_sel_pref 0x%x",
                 ph_nw_sel_pref);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if ((ph_nw_sel_pref != GSTK_NW_SEARCH_MODE_AUTOMATIC) &&
      (ph_nw_sel_pref != GSTK_NW_SEARCH_MODE_MANUAL))
  {
    UIM_MSG_ERR_0("Unknown Neywork Search Mode type");
    return GSTK_ERROR;
  }

  if(gstk_curr_inst_ptr->nw_search_mode == ph_nw_sel_pref &&
     gstk_curr_inst_ptr->is_nw_search_mode_sent == TRUE)
  {
    UIM_MSG_HIGH_0("network search mode pref is same as before");
    return GSTK_ERROR;
  }

  gstk_curr_inst_ptr->nw_search_mode = ph_nw_sel_pref;
  gstk_curr_inst_ptr->is_nw_search_mode_sent = FALSE;

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd == NULL )
  {
    return GSTK_ERROR;
  }

  /* successfully obtained command buffer - build the envelope command */
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.client_id = gstk_curr_inst_ptr->client_id;
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_id = (int)GSTK_NW_SEARCH_MODE_EVT_IND;
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.user_data = 0x00;

  /* network search mode */
  task_cmd->cmd.ph_nw_sel_pref_ind.ph_nw_sel_pref = ph_nw_sel_pref;

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  gstk_task_put_cmd_buf(task_cmd);
  return GSTK_SUCCESS;
}


/*===========================================================================
FUNCTION   gstk_build_csg_cell_selection_envelope

DESCRIPTION
  This function queues CSG cell selection envelope command to GSTK task queue

PARAMETERS


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
gstk_status_enum_type gstk_build_csg_cell_selection_envelope(
  const gstk_csg_change_ind_type* csg_ind
)
{
  gstk_task_cmd_type* task_cmd = NULL;

  UIM_MSG_HIGH_0("CSG cell selection event download");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
  GSTK_RETURN_ERROR_IF_NULL_PTR(csg_ind, GSTK_ERROR);

  task_cmd = gstk_task_get_cmd_buf();
  if (task_cmd == NULL)
  {
    return GSTK_ERROR;
  }

  task_cmd->cmd.csg_change_ind.message_header.client_id =
    gstk_curr_inst_ptr->client_id;
  task_cmd->cmd.csg_change_ind.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.csg_change_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.csg_change_ind.message_header.command_id =
    (int)GSTK_CSG_CELL_SELECTION_EVT_IND;
  task_cmd->cmd.csg_change_ind.message_header.user_data = 0x00;

  /* Copy CSG related data */
  task_cmd->cmd.csg_change_ind.cmd.csg_id = csg_ind->cmd.csg_id;
  task_cmd->cmd.csg_change_ind.cmd.len = csg_ind->cmd.len;
  if (task_cmd->cmd.csg_change_ind.cmd.len > 0)
  {
    if(gstk_memcpy(task_cmd->cmd.csg_change_ind.cmd.hnb_name,
                   csg_ind->cmd.hnb_name,
                   csg_ind->cmd.len,
                   GSTK_HNB_NAME_MAX_LEN,
                   GSTK_HNB_NAME_MAX_LEN) <
       csg_ind->cmd.len)
    {
      UIM_MSG_ERR_0("gstk_build_csg_cell_selection_envelope: gstk_memcpy failed");
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_MEMORY_ERROR;
    }
  }
  task_cmd->cmd.csg_change_ind.cmd.access_tech =
    csg_ind->cmd.access_tech;
  task_cmd->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped =
    csg_ind->cmd.selection_status.general_info.is_camped;
  task_cmd->cmd.csg_change_ind.cmd.selection_status.addi_info.is_addi_info_present =
    csg_ind->cmd.selection_status.addi_info.is_addi_info_present;

  gstk_task_put_cmd_buf(task_cmd);
  return GSTK_SUCCESS;
} /* gstk_build_csg_cell_selection_envelope */


/*===========================================================================
FUNCTION gstk_process_non_idle_state_envelope_cmd

DESCRIPTION
  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope response to the caller

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_process_non_idle_state_envelope_cmd(
  gstk_cmd_type  * cmd_ptr
)
{
  gstk_status_enum_type gstk_status     = GSTK_SUCCESS;
  uint32                type_of_command = 0x00;
  uint32                env_ref_id      = GSTK_CMD_DETAILS_REF_TABLE_FULL;

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Input Pointer");
    return GSTK_BAD_PARAM;
  }

  /* Get the type of command */
  type_of_command = cmd_ptr->general_cmd.message_header.command_id;
  UIM_MSG_HIGH_1("gstk_process_non_idle_state_envelope_cmd: command = 0x%x",
                 type_of_command);

  /* get the envelope refernce id */
  gstk_status =  gstk_util_save_env_cmd_info(cmd_ptr->general_cmd.message_header.client_id,
                                             (gstk_cmd_enum_type)type_of_command,
                                             cmd_ptr,
                                             &env_ref_id);
  if(gstk_status == GSTK_CLIENT_NOT_REGISTERED)
  {
    UIM_MSG_ERR_0("Failed to Save Envelope Info: status = GSTK_CLIENT_NOT_REGISTERED");
    return gstk_status;
  }

  /* user_data is already stored as part of envelope info, use this field
     to store env_ref_id for future use */
  cmd_ptr->general_cmd.message_header.user_data = env_ref_id;

  switch(type_of_command)
  {
    case GSTK_MENU_SELECTION_IND:
      gstk_status = gstk_non_idle_st_menu_selection_err_rsp(&(cmd_ptr->menu_selection_envelope_ind));
      break;

    case GSTK_SMS_PP_DOWNLOAD_IND:
      gstk_status = gstk_non_idle_st_sms_pp_err_rsp(&(cmd_ptr->sms_pp_envelope_ind));
      break;

    case GSTK_MT_CALL_EVT_IND:
    case GSTK_CALL_CONNECTED_EVT_IND:
    case GSTK_CALL_DISCONNECTED_EVT_IND:
    case GSTK_IDLE_SCRN_EVT_IND:
    case GSTK_LOCATION_STATUS_EVT_IND:
    case GSTK_USER_ACTIVITY_EVT_IND:
    case GSTK_LANG_SELECTION_EVT_IND:
    case GSTK_BROWSER_TERMINATION_EVT_IND:
    case GSTK_DATA_AVAIL_EVT_IND:
    case GSTK_CH_STATUS_EVT_IND:
    case GSTK_ACCESS_TECH_EVT_IND:
    case GSTK_NW_SEARCH_MODE_EVT_IND:
    case GSTK_CSG_CELL_SELECTION_EVT_IND:
      gstk_status = gstk_non_idle_st_evt_dl_err_rsp(cmd_ptr);
      break;

    case GSTK_MO_SMS_CTRL_IND:
      gstk_status = gstk_non_idle_st_mo_sms_ctrl_err_rsp(&(cmd_ptr->mo_sms_ctrl_envelope_ind));
      break;

    case GSTK_CC_IND:
      gstk_status = gstk_non_idle_st_cc_err_rsp(&(cmd_ptr->cc_envelope_ind));
      break;

    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      gstk_status = gstk_non_idle_st_cell_change_brcst_err_rsp(&(cmd_ptr->cell_broadcast_envelope_ind));
      break;

    case GSTK_TIMER_EXPIRE_IND:
      gstk_status = gstk_non_idle_st_timer_expire_err_rsp(&(cmd_ptr->timer_expire_envelope_ind));
      break;

    case GSTK_RAW_ENVELOPE_IND:
      gstk_status = gstk_non_idle_st_raw_env_err_rsp(&(cmd_ptr->raw_envelope_ind));
      break;

    case GSTK_LOCATION_INFORMATION_IND:
      gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
      gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;

      /* Orginal attach control command processing */
      if(GSTK_MAX_PROACTIVE_COMMAND == gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data)
      {
        UIM_MSG_HIGH_2("Attach ctrl: sig_to_wait 0x%x, task_tcb 0x%x",
                       cmd_ptr->attach_ctrl_ind.attach_ctrl.sig_to_wait,
                       cmd_ptr->attach_ctrl_ind.attach_ctrl.task_tcb);
        UIM_MSG_HIGH_3("MCC/MNC: 0x%x 0x%x 0x%x",
                       cmd_ptr->attach_ctrl_ind.attach_ctrl.mcc_mnc[0],
                       cmd_ptr->attach_ctrl_ind.attach_ctrl.mcc_mnc[1],
                       cmd_ptr->attach_ctrl_ind.attach_ctrl.mcc_mnc[2]);
        (void)rex_set_sigs(
                (rex_tcb_type *)cmd_ptr->attach_ctrl_ind.attach_ctrl.task_tcb,
                (rex_sigs_type)cmd_ptr->attach_ctrl_ind.attach_ctrl.sig_to_wait);
      }
       /* Internally queued VPE command processing */
      else
      {
        gstk_util_send_attach_ctrl_res();
      }
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break;

    default:
      gstk_util_env_cmd_ref_table_free_slot(
        cmd_ptr->location_information_envelope_ind.message_header.user_data);
      break;
  }
  return gstk_status;
} /* gstk_process_non_idle_state_envelope_cmd*/

/*===========================================================================
FUNCTION gstk_check_envelope_offset

DESCRIPTION
  This function checks the envelope offest to ensure there is no buffer overflow

PARAMETERS
  *cmd_ptr: pointer of uim_cmd_type type
  offset:   offset to be verified
  length:   length of data to be copied, value to be verified against offset

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  cmd_ptr will be freed if validation against offset fails

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_check_envelope_offset(uim_cmd_type *cmd_ptr,
                                           int offset, uint32 length){
  if (!cmd_ptr)
  {
    return GSTK_ERROR;
  }
  if (offset <= uint32toint32(sizeof(cmd_ptr->envelope.data) - length))
  {
    return GSTK_SUCCESS;
  }
  else{
    gstk_free(cmd_ptr);
    return GSTK_ERROR;
  }
}

/*===========================================================================
FUNCTION gstk_env_retry_timer_exp_cb

DESCRIPTION
  Callback function when the timer of retrying envelope command expires

PARAMETERS
  param [Input] user data

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
void gstk_env_retry_timer_exp_cb(
  unsigned long param
)
{
  gstk_task_cmd_type*  task_cmd = NULL;

  UIM_MSG_HIGH_1("In gstk_env_retry_timer_exp_cb() on slot 0x%x", param);

  if (!GSTK_IS_VALID_SLOT_ID(param))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", param);
    return;
  }

  task_cmd = gstk_task_get_cmd_buf();
  if (task_cmd != NULL)
  {
    task_cmd->cmd.general_cmd.message_header.sim_slot_id =
      (gstk_slot_id_enum_type)param;
    task_cmd->cmd.general_cmd.message_header.command_group =
      GSTK_TIMER_FOR_ENV_RETRY_CMD;
    task_cmd->cmd.general_cmd.message_header.command_id = 0;
    task_cmd->cmd.general_cmd.message_header.user_data = 0;
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    gstk_util_clear_retry_envelope_data((gstk_slot_id_enum_type)param);
  }
} /*gstk_env_retry_timer_exp_cb*/

/*===========================================================================
FUNCTION gstk_send_cached_terminal_rsp

DESCRIPTION
  This function posts the cacahed TR to UIM

PARAMETER
  instance: Used to identify GSTK instance

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_send_cached_terminal_rsp(unsigned long instance)
{
  uim_cmd_type*         uim_cmd_ptr = NULL;
  uim_return_type       uim_state   = UIM_SUCCESS;
  uim_rpt_type          uim_rpt;
  gstk_status_enum_type gstk_status;

  UIM_MSG_HIGH_1("gstk_send_cached_terminal_rsp: instance 0x%x", instance);

  if (!GSTK_IS_VALID_SLOT_ID(instance))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", instance);
    return;
  }

  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL )
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);

    if((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
    {
      if(gstk_instances_ptr[(uint8)instance -1]->gstk_cmd_cache.term_resp_length > 0)
      {
        (void)memscpy(uim_cmd_ptr->terminal_response.data,
               gstk_instances_ptr[(uint8)instance -1]->gstk_cmd_cache.term_resp_length,
               gstk_instances_ptr[(uint8)instance -1]->gstk_cmd_cache.term_resp,
               gstk_instances_ptr[(uint8)instance -1]->gstk_cmd_cache.term_resp_length);

        uim_cmd_ptr->terminal_response.num_bytes =
          (byte)gstk_instances_ptr[(uint8)instance -1]->gstk_cmd_cache.term_resp_length;
      }
      else
      {
        UIM_MSG_ERR_0("Zero Length TR");
      }

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
    }
    else
    {
      UIM_MSG_HIGH_2("Populate UIM buf hdr returns %d and uim_cmd_ptr is 0x%x",
                     gstk_status, uim_cmd_ptr);
    }
    gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
    uim_cmd_ptr = NULL;
  }
  else
  {
    /* WHAT NOW - Toolkit Blocked */
    gstk_instances_ptr[(uint8)instance - 1]->gstk_proactive_cmd_in_progress = FALSE;
  }
} /* gstk_send_cached_terminal_rsp */

/*===========================================================================
FUNCTION gstk_find_cached_cell_id

DESCRIPTION
  This function searches the GSTK cached arbitrated MM idles from each MM stack
  by PLMN and LAC to find the corresponding cell id for that stack

PARAMETER
  plmn_p      [Input]  : ptr to PLMN of matching MM stack
  lac_p       [Input]  : ptr to LAC of matching MM stack
  cell_info_p [Output] : ptr to gstk_cell_id_type

DEPENDENCIES
  gstk_curr_inst_ptr is valid

RETURN VALUE
  GSTK_SUCCESS if stack is found; cell_info_p is valid. Otherwise
  cell_info_p is not valid.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_find_cached_cell_id (const uint8        *plmn_ptr,
                                                const uint8        *lac_ptr,
                                                gstk_cell_id_type  *cell_info_ptr)
{
  int   i                                      = 0;
  uint8 invalid_cell_id[GSTK_MAX_CELL_ID_LEN]  = {0};

  /* Check against passed in cell id later*/
  memset(invalid_cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);

  UIM_MSG_HIGH_0("In gstk_find_cached_cell_id()");

  if (!plmn_ptr)
  {
    UIM_MSG_ERR_0("plmn_p is NULL");
    return GSTK_BAD_PARAM;
  }

  if (!lac_ptr)
  {
    UIM_MSG_ERR_0("lac_p is NULL");
    return GSTK_BAD_PARAM;
  }

  if (!cell_info_ptr)
  {
    UIM_MSG_ERR_0("cell_info_p is NULL");
    return GSTK_BAD_PARAM;
  }

  for (i = 0; i < NUM_OF_MM_PER_GSTK_INST; i++)
  {
    if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i] != NULL &&
        !memcmp(plmn_ptr,
                gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.mcc_mnc,
                sizeof(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.mcc_mnc)) &&
        !memcmp(lac_ptr,
                gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.lac,
                sizeof(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.lac)))
    {

      UIM_MSG_HIGH_1("Found matching PLMN/LAC in GSTK arbitrated MM cache with index 0x%x",
                     i);
      UIM_MSG_HIGH_3("PLMN = 0x%x 0x%x 0x%x",
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.mcc_mnc[0],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.mcc_mnc[1],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.mcc_mnc[2]);
      UIM_MSG_HIGH_2("LAC = 0x%x 0x%x",
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.lac[0],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.lac[1]);
      UIM_MSG_HIGH_4("Cell ID is 0x%x 0x%x 0x%x 0x%x",
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_id[0],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_id[1],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_id[2],
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_id[3]);

      if (!memcmp(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_id,
                  invalid_cell_id,
                  gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_len))
      {
        UIM_MSG_ERR_0("MM Cache cell id is invalid");
        return GSTK_ERROR;
      }
      if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_len <= 0 ||
          gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id.cell_len > GSTK_MAX_CELL_ID_LEN)
      {
        UIM_MSG_ERR_0("MM Cache cell id len is invalid");
        return GSTK_ERROR;
      }

      (void)gstk_memcpy(cell_info_ptr,
                      &gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]->mm_idle_state.cell_id,
                      sizeof(gstk_cell_id_type),
                      sizeof(gstk_cell_id_type),
                      sizeof(gstk_cell_id_type));

      return GSTK_SUCCESS;
    }
  }

  return GSTK_ERROR;
}
