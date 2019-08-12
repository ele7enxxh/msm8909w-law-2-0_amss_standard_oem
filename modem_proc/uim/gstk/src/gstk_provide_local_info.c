/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           G S T K  P R O V I D E  L O C A L  I N F O R M A T I O N


GENERAL DESCRIPTION
  This source file contains functions to process provide local information
  proactive command


FUNCTIONS
  gstk_send_provide_local_info_terminal_rsp
    This function populate the gstk_provide_local_info_cnf_type and put the
    terminal response confirm type to the GSTK queue.

  gstk_provide_local_info_req
    This function parses the provide local info Proactive Command from the card
    and sends it to the client.

  gstk_provide_local_info_cnf
    This function packs the provide local info terminal response and send
    the command to UIM server.

  gstk_provide_local_info_cnf_cleanup
    This function cleans up any allocated pointer on the stack.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_provide_local_info.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
01/29/15   nk      Remove FEATURE_GSTK_REMOTE_TIME Feature
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
10/08/14   bd      Changes to control eHRPD loc info feature with NV
09/30/14   gs      Use card_protocol to determine length of cell id
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/09/14   dy      Fix timezone defaulted to 0x00
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/25/14   dy      fix time and date not calculated when in NO SERVICE
08/16/14   lj      avoid overwriting TZ incorrectly and also avoid
                   calculating time and date when UE is in no service
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
07/09/14   bd      LTE NMR interface changes
06/30/14   gm      Remove unnecessary clear of timer
06/26/14   ky      Added test framework define to enter proper stub function
05/28/14   gm      Support of CSG Rel-11 feature
05/06/14   gs      GSTK Featurization enhancements and clean up
04/29/14   vr      GSTK Compilation errors fix for 1x compile out build
04/21/14   vr      IMEI should be linked to slot and not to subscription
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   gm      Added support for IMS communication control
02/19/14   gs      Fixed infinite IMSI refresh issue
01/19/14   bd      Fixed incorrect TR for PLI NMR UTRAN
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   hn      Call wrapper API for segment loading
12/16/13   bd      Added support for new CM SS event handling
11/13/13   shr     Added support for BIP over WLAN
11/01/13   vr      Compilation errors when removing WCDMA feature
11/01/13   shr     Fixed porting error w.r.t. PLI date and time TR expiry
10/29/13   jd      PLI date and time handling w.r.t. TR timer expiry fixed
10/04/13   sw      Reduced F3 messages
09/20/13   shr     LTE multi-SIM support
09/13/13   gm      Support for Recovery and hot-swap
08/19/13   hn      Move lte_nmr_ref_id out of FEATURE_LTE and rename it
08/19/23   hn      Fix compile error
08/06/13   bd      Fixed incorrect usage of fn rrc_gstk_get_utran_nmr_req()
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
06/20/13   vr      Triton TSTS changes
05/17/13   abd     Added support for reading default lang from NV
04/22/13   hh      Replace rr_xx_get_nmr_bcch() with rr_xx_api_get_nmr_bcch()
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/06/13   bd      Check for gstk_next_curr_rat before picking CDMA Loc Info
02/11/13   bd      Support for (e)HRPD access tech
01/17/13   hn      Support multiple sessions per subscription
12/10/12   js      Fixed compiler warnings
12/04/12   hn      Fix compilation error (DUAL_SIM diabled, DUAL_IMEI enabled)
10/18/12   bd      Include time_conv.h
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
07/25/12   sg      Enable FEATURE_PLI_NMR_UTRAN
05/21/12   dd      Make PLI QMI DMS independent of clock defines
03/30/12   av      Replaced feature flags with NVITEM
03/22/12   dd      PLI for QMI DMS
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS ON features
10/11/11   dd      Change padding byte of PLI MEID
08/03/11   xz      Fix compiler warning
07/21/11   sg      Fix PLI for Language
07/20/11   dd      Provide local info for QMI DMS time API
06/06/11   sg      Add support for mandatory TLV for result 0x20
05/18/11   xz      Use MMGSDI evt to track apps enabled
05/03/11   xz      Provide LTE only support
04/28/11   nk      changed uim_get_esn_me to read from NV instead
04/20/11   js      Fixed issue for PLI Location Service crash on non-WCDMA build
04/20/11   nb      Pass default IMEI in case as_id is still not available
12/30/10   ssr     Dynamically allocate command buffer
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   ssr     Fixed compilation warnings
11/29/10   tkl     Removed RRC private header file
11/16/10   yt      Change for compilation with Dual SIM enabled
11/10/10   nb      Dual IMEI support
10/21/10   dd      Set default time zone to 0xFF until it is received from NW
10/20/10   dd      Fix PLI Access Tech TR with no service
10/20/10   xz      LINT Fix
10/20/10   xz      Fix the issue of not send TR in eHRPD only mode
10/07/10   ms      Fixed Lint errors
08/31/10   dd      Changes for compilation for LTE only build
08/30/10   xz      Send error TR if packing TR fails
08/19/10   xz      LINT Fixes
07/30/10   dd      Add time_remote_get_julian API
07/30/10   dd      Removing CMI violation that was added in DS/DS merge
07/26/10   xz      Fix the issue of LTE NMR
06/29/10   nb      Merged changes for DS DS
06/18/10   sg      Fix PLI location status for LTE
05/24/10   bd      Replaced clk_julian_type with time_julian_type
05/23/10   xz      Add support of LTE NMR
05/11/10   xz      Fix the issue of null cell ID
05/20/10   sg      Fix compilation issue on certain branches
05/06/10   sg      MEID corruption and reversal
04/27/10   xz      Add support of EMM-IDLE event
04/07/10   yt      Fixed compilation warnings
03/22/10   dd      CMI complicance, remove clkrtc.h
03/08/10   xz      Use gstk_get_nv to read IMEI and IMEI_SV
03/08/10   dd      Use secure clock if FEATURE_SEC_TIME_V2 is defined
03/01/10   xz      DS/DS NMR Changes
02/24/10   sg      Fixed issue of reading incorrect NV for MEID
02/15/10   nb      Moved Dual Slot Changes
02/10/10   dd      Provide local time and time zone in PLI
02/01/10   dd      Use CMI compliant time_get_apps_user_time function
01/11/10   sg      Fix issue of ESN
11/23/09   dd      Use GMT time zone in PLI
10/27/09   xz      Fix issue of PLI - location status in no service
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
08/31/09   xz      Fix issue of PLI date/time on SC1X
07/21/09   dd      Adding Time Zone information
07/28/09   bd      Fix lint issues
07/08/09   dd      Fix IMEISV length
05/18/09   sg      Added static keyword to remove warnings in gstk_process_pli_utran_nmr_req
04/07/09   xz      Fix issue of DATE/TIME/TIME ZONE failure
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
03/06/09   gg      If in limited service terminal response result for NW
                   measurement must be set to
                   GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE
12/17/08   nb      Common Modem Interface
09/14/08   sk      Fixed lint errors
09/09/08   xz      Fix for compilation warning
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/07/08   sk      Fix for PLI UTRAN NMR Limited Service
03/17/08   sk      Fixed GSTK issue for No payload with Terminal Response for
                   PLI NMR
02/29/08   yb      Fixed malloc failure if BCCH list from network is empty
02/13/08   sk      Fixed compilation error on 1X
02/06/08   sk      Support for PLI UTRAN NMR
01/30/08   yb      Fixed Compiler warning for clk_rtc_get_time()
10/03/07   sk      Fixed support for PLI NMR UTRAN
08/07/07   sk      Fixed compilation problem / warning
05/11/07   sp      Featurize for LTK
05/02/07   nk      Added Featurization
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
03/13/07   sk      Initialized rtc_julian
02/01/07   sk      Added Null ptr checks
12/14/06   sk      Lint fixes
10/23/06   sk      Fixed lint errors.
08/25/06   sk      Fixed typo
08/22/06   sk      Fixed PLI-ESN to obtain info from HW
07/25/06   nk      Fixed potential integer underflow problem
06/30/06   sk      Lint warning
06/24/06   sk      Lint fixes
06/13/06   sk      Fixed compilation problems for 1X build.
06/06/06   sk      Added support for network search mode
05/18/06  jar/nk   Fixed compile error in CDMA build as a result of libprot.h
05/17/06   sk      Lint fixes
05/11/06   sk      Removed warnings
05/03/06   sk      Removed cell id checks
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information.
04/04/06   sk      Added support for pli - Battery status, ESN, MEID, IMEISV
11/21/05   sp      Fixed lint errors
11/17/05   sk      Removed warnings
11/16/05   sk      Changed additional info to conform to the specs.
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
11/03/05   sk      Fixed compilation warning
10/10/05   tml     Used cached rat value for Access Tech.  Add support to use
                   real time info from MM, RR, RRC, and CM if no event has
                   ever received from MM IDle yet
10/08/05   tml     Used most updated cached info for Location Status
10/05/05   sk      Memset cell id field when in Limited service.
10/06/05   tml     Added FEATURE_SEC_TIME support
09/30/05   sk      Added support for two tables, one for RRC location information
                   and one for MM location information
09/27/05   sk      Added check for service status type in
                   gstk_provide_local_info_req()
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/19/05   sk      Fix for IMEI last byte
07/13/05   sk      Added support for Access Technology
05/25/05   sk      Added support for event downloads when changing to cells,
                   all withing limited service.
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/24/04  sk/tml   memset request buffer before assignment
08/03/04   tml     Memory leak fix and changed to use pointer for additional
                   info in terminal response
05/26/04   tml     Added More debug messages
04/16/04   tml     linted
04/02/04   tml     linted
03/01/04   tml     Added language, NMR, BCCH and Date and Time fixes
02/24/04   tml     Fixed compilation error
02/23/04   tml     Fixed compilation error
02/06/04   wli     Featurization for CDMA only environment
09/12/03   tml     Fixed IMEI local info issue
06/23/03   tml     removed compiler error for 1x/gsm
03/25/03   tml     Fixed typo
03/21/03   tml     Featurization fix for WCDMA single mode build
03/07/03   tml     Updated RTC info
03/06/03   tml     Updated enum names, Added FEATURE_GSTK_FOR_NON_LTK_BUILD
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#include "libprot.h"
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#include "err.h"
#include "string.h"
#include <stringl/stringl.h>
#include "qw.h"
#include "uim_msg.h"

#include "time_jul.h"
#include "time_svc.h"

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined (FEATURE_LTE)
#include "ghdi_exp.h"
#include "mm.h"
#ifdef FEATURE_GSM
#include "rr_usat_if.h"
#endif /*FEATURE_GSM */
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_LTE) */

#ifdef FEATURE_LTE
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
#include "rrcintif.h"
#endif /* FEATURE_WCDMA */


#ifdef FEATURE_SEC_TIME
extern void sectime_get_local_time(uint64 *ret_val);
#endif /* #ifdef FEATURE_SEC_TIME */

extern void time_set_julian( time_julian_type *jul_time );
extern void time_get_julian( time_julian_type *jul_time );


#ifdef FEATURE_CDMA
#include "db.h"
#endif /* FEATURE_CDMA */

#ifdef FEATURE_SEC_TIME_V2
#include "time_secure_v2.h"
#endif /* FEATURE_SEC_TIME_V2 */

#include "time.h"
#include "time_genoff.h"
#include "time_conv.h"

/*===========================================================================
FUNCTION gstk_provide_local_info_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  provide_local_info_cnf_ptr: [Input] Pointer to message that is required to
                                      be processed and sent to UIM
  STK_response_ptr          : [Input] Pointer to the STK intermediate message

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
static void gstk_provide_local_info_cnf_cleanup(
  gstk_provide_local_info_cnf_type           *provide_local_info_cnf_ptr,
  provide_local_info_terminal_response_type  *STK_response_ptr)
{
  int i = 0;

  if((STK_response_ptr == NULL)||(provide_local_info_cnf_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR:NULL pointer STK_response_ptr=0x%x provide_local_info_cnf_ptr=0x%x",
                  STK_response_ptr, provide_local_info_cnf_ptr);
    return;
  }

  /* free provide local info cnf and tlv */
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info = NULL;
  }
  if (provide_local_info_cnf_ptr->local_info_rsp.info_type == GSTK_NMR_BCCH_INFO) {
    if (STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list != NULL ) {
      gstk_free(STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list);
      STK_response_ptr->local_info_rsp.nmr_bcch.bcch_ch_list.channel_list= NULL;
    }
  }
  switch (provide_local_info_cnf_ptr->local_info_rsp.info_type)
  {
    case GSTK_CDMA_LOCATION_INFO:
      gstk_util_location_info_tlv_cleanup(&STK_response_ptr->local_info_rsp.cdma_ext_loc_info);
      break;

    case GSTK_NMR_UTRAN_INFO:
      if (STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr)
      {
        for (i = 0; i < STK_response_ptr->local_info_rsp.nmr_utran_info.num_nmr; i++)
        {
          if (STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].measurement_report_ptr != NULL) {
            gstk_free(STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].measurement_report_ptr);
            STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].measurement_report_ptr = NULL;
          }
        }
        gstk_free(STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr);
        STK_response_ptr->local_info_rsp.nmr_utran_info.nmr_utran_ptr = NULL;
      }
      break;
    case GSTK_CSG_CELL_INFO:
      if(STK_response_ptr->local_info_rsp.csg_info.csg_info != NULL)
      {
        gstk_free(STK_response_ptr->local_info_rsp.csg_info.csg_info);
        STK_response_ptr->local_info_rsp.csg_info.csg_info = NULL;
      }
      break;
    default:
      break;
  }
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)provide_local_info_cnf_ptr);/*lint !e826 area too small */
} /* gstk_provide_local_info_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_provide_local_info_terminal_rsp

DESCRIPTION
  This function populate the gstk_provide_local_info_cnf_type and put the
  terminal response confirm type to the GSTK queue.

PARAMETERS
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  cmd_num           : [Input] This is the same value as that was being passed to
                              the client in the corresponding proactive command
                              request
  general_result    : [Input] Specifies the result of the proactive command
                              operation
  additional_result_ptr: [Input] Specifies the optional additional result for the
                             proactive command, the length field in the
                             gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info_ptr: [Input] Pointer to the gstk_local_info_rsp_type

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
gstk_status_enum_type  gstk_send_provide_local_info_terminal_rsp(
  uint32                                  cmd_details_ref_id,
  uint8                                   cmd_num,
  gstk_general_result_enum_type           general_result,
  gstk_additional_info_type               *additional_result_ptr,
  const gstk_local_info_rsp_type          *other_info_ptr)
{

  gstk_task_cmd_type              *task_cmd_ptr = NULL;
  gstk_status_enum_type          gstk_status            = GSTK_SUCCESS;
  gstk_additional_info_ptr_type  additional_result_temp;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if((additional_result_ptr == NULL)||(other_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("ERR:NULL pointer additional_result_ptr=0x%x other_info_ptr=0x%x",
                  additional_result_ptr, other_info_ptr);
    return GSTK_BAD_PARAM;
  }

  memset(&additional_result_temp, 0x00, sizeof(additional_result_temp));

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr != NULL)
  {
    additional_result_temp.length = additional_result_ptr->length;
    additional_result_temp.additional_info_ptr =
      additional_result_ptr->additional_info;

    /* build the terminal response */
    gstk_status = gstk_populate_provide_local_info_terminal_rsp_cnf(
                    task_cmd_ptr,
                    cmd_details_ref_id,
                    cmd_num,
                    general_result,
                    &additional_result_temp,
                    other_info_ptr,
                    0);

    if(gstk_status == GSTK_SUCCESS)
      {
      gstk_task_put_cmd_buf(task_cmd_ptr);
        }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      }
    return gstk_status;
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
} /* gstk_send_provide_local_info_terminal_rsp */

/*===========================================================================
FUNCTION gstk_process_pli_utran_nmr_req

DESCRIPTION
  This function processes the pli utran nmr request sent from the card

PARAMETERS
  cmd_data_ptr:                   [Input] Pointer to command data buffer
  cmd_data_len:                   [Input] Length of cmd_data_ptr buffer
  orig_offset:                    [Input] Original offset in the buffer
  next_tlv_offset_ptr:            [Input/Output] Pointer to next tlv offset
                                  in the buffer
  more_tlvs_ptr:                  [Input/Output] Pointer to more tlv flag
  ref_id_ptr:                     [Input] Pinter to reference id in the
                                  command_details_ref_table
  nmr_utran_qualifier_ptr:        [Input/Output] Pointer to nmr qualifier flag
  need_to_send_immediate_tr_ptr:  [Input/Output] Pointer to flag that indicates the
                                  need to send immediate terminal response
  general_result_ptr:             [Input/Output] Pointer to general result
  additional_info_code_ptr:       [Input/Output] Pointer to additional info

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_BAD_PARAM: Bad Input paramaters passed in
    GSTK_SUCCESS:   Processing was successful

COMMENTS
  None

SIDE EFFECTS
  If the processing is successful, a request to get the UTRAN NMR informatiom
  will be sent to RRC.RRC response for UTRAN NMR will be received
  asycnhronously by GSTK

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_pli_utran_nmr_req (
  uint8                                *cmd_data_ptr,
  int32                                 cmd_data_len,
  int32                                 orig_offset,
  int32                                *next_tlv_offset_ptr,
  boolean                              *more_tlvs_ptr,
  uint32                               *ref_id_ptr,
  gstk_nmr_utran_qualifier_enum_type   *nmr_utran_qualifier_ptr,
  boolean                              *need_to_send_immediate_tr_ptr,
  gstk_general_result_enum_type        *general_result_ptr,
  gstk_additional_info_type            *additional_info_code_ptr
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;
  sys_modem_as_id_e_type as_id        = SYS_MODEM_AS_ID_NONE;
  gstk_slot_id_enum_type slot         = GSTK_SLOT_ID_MAX;
#ifdef FEATURE_LTE
  lte_rrc_nmr_info_req_s lte_nmr_req;
  lte_errno_e            status       = E_SUCCESS;
  /* LTE RRC Instance ID, by default set to 1 (i.e. AS_ID_1) */
  uint8                  inst_id      = 1;
#endif /*FEATURE_LTE*/

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(!cmd_data_ptr || !next_tlv_offset_ptr || !more_tlvs_ptr || !ref_id_ptr ||
     !nmr_utran_qualifier_ptr || !need_to_send_immediate_tr_ptr ||
     !general_result_ptr || !additional_info_code_ptr)
  {
    UIM_MSG_ERR_0("gstk_process_pli_utran_nmr_req ERR:NULL ptr");
    return GSTK_BAD_PARAM;
  }

  *need_to_send_immediate_tr_ptr = TRUE;

  if(gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NO_SERVICE)
  {
    UIM_MSG_ERR_0("NMR UTRAN: No Service");
    *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SERVICE_AVAILABLE);
    return GSTK_SUCCESS;
  }
  else if(gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_LIMITED_SERVICE)
  {
    UIM_MSG_HIGH_0("NMR UTRAN: Limited service");
    *general_result_ptr = GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE;
  }

  /* Normal/Limited Service */
  if(gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_UTRAN &&
     gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_LTE)
  {
    UIM_MSG_ERR_1("UTRAN NMR not supported on RAT 0x%x",
                  gstk_curr_inst_ptr->gstk_next_curr_rat);
    *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SERVICE_AVAILABLE);
    return GSTK_SUCCESS;
  }

  /* process if in UTRAN and tlv is present */
  gstk_status = gstk_parser_pli_nmr_utran_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              orig_offset,
                                              GSTK_UTRAN_MEASUREMENT_TAG,
                                              next_tlv_offset_ptr,
                                              more_tlvs_ptr,
                                              GSTK_MANDATORY,
                                              ref_id_ptr,
                                              nmr_utran_qualifier_ptr);
  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("NMR_INFO UTRAN PARSING ERROR");
    *general_result_ptr  = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
    return GSTK_SUCCESS;
  }

  slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  if (gstk_io_ctrl(GSTK_IO_SLOT_ID_TO_MODEM_AS_ID, &slot, &as_id)
      != GSTK_SUCCESS)
  {
    *general_result_ptr  = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
    return GSTK_SUCCESS;
  }

  gstk_shared_data.nmr_ref_id  = *ref_id_ptr;

  switch (*nmr_utran_qualifier_ptr)
  {
    case GSTK_NMR_INTRA_FREQ_MEAS:
    case GSTK_NMR_INTER_FREQ_MEAS:
    case GSTK_NMR_INTER_RAT_MEAS:
    case GSTK_NMR_INTER_RAT_LTE_MEAS:
#ifdef FEATURE_WCDMA
#if defined(FEATURE_SEGMENT_LOADING) && !defined(FEATURE_UIM_TEST_FRAMEWORK)
      if(gstk_pi_wcdma != NULL)
      {
        UIM_MSG_HIGH_0("IWCDMA_rrc_gstk_get_utran_nmr_req()");
        (void)IWCDMA_rrc_gstk_get_utran_nmr_req(
          gstk_pi_wcdma,
          GSTK_MAX_UTRAN_NMR_NO_OF_CELLS,
          (gstk_nmr_utran_qualifier_enum_type)(*nmr_utran_qualifier_ptr),
          &gstk_shared_data.nmr_ref_id,
          as_id
          );
        *need_to_send_immediate_tr_ptr = FALSE;
        return GSTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_0("WCDMA interface pointer is NULL!");
        *general_result_ptr = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
        return GSTK_SUCCESS;
      }
#else
      /* Send UTRAN NMR info req to RRC */
      rrc_gstk_get_utran_nmr_req(
        GSTK_MAX_UTRAN_NMR_NO_OF_CELLS,
        (gstk_nmr_utran_qualifier_enum_type)(*nmr_utran_qualifier_ptr),
        &gstk_shared_data.nmr_ref_id,
        as_id
        );
      *need_to_send_immediate_tr_ptr = FALSE;
      return GSTK_SUCCESS;
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
    case GSTK_NMR_LTE_INTRA_FREQ_MEAS:
    case GSTK_NMR_LTE_INTER_FREQ_MEAS:
    case GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS:
    case GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS:
      msgr_init_hdr(&lte_nmr_req.msg_hdr, MSGR_UIM_STK, LTE_RRC_NMR_INFO_REQ);
      switch (*nmr_utran_qualifier_ptr)
      {
        case GSTK_NMR_LTE_INTRA_FREQ_MEAS:
          lte_nmr_req.report_type = LTE_RRC_NMR_REPORT_INTRA_FREQ;
          break;
        case GSTK_NMR_LTE_INTER_FREQ_MEAS:
          lte_nmr_req.report_type = LTE_RRC_NMR_REPORT_INTER_FREQ;
          break;
        case GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS:
          lte_nmr_req.report_type = LTE_RRC_NMR_REPORT_IRAT_GERAN;
          break;
        case GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS:
          lte_nmr_req.report_type = LTE_RRC_NMR_REPORT_IRAT_WCDMA;
          break;
        default:
          UIM_MSG_ERR_1("invalid NMR Qualifier 0x%x!", *nmr_utran_qualifier_ptr);
          *general_result_ptr  = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
          return GSTK_SUCCESS;
      }

      if(gstk_shared_data.gstk_num_of_instances > 1)
      {
        /* Convert AS_ID to Instance ID */
        inst_id = SYS_AS_ID_TO_INST_ID(as_id);
      }
      /* Populate Instance ID in the LTE RRC message header */
      msgr_set_hdr_inst(&lte_nmr_req.msg_hdr, inst_id);
      status = msgr_send(&lte_nmr_req.msg_hdr, sizeof(lte_nmr_req));
      if (status == E_SUCCESS)
      {
        *need_to_send_immediate_tr_ptr = FALSE;
        return GSTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("Failed to send LTE NMR req! 0x%x", status);
        *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        return GSTK_SUCCESS;
      }
#endif /*FEATURE_LTE*/

    default:
      UIM_MSG_ERR_0("Invalid NMR qualifiter 0x%x!");
      *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      return GSTK_SUCCESS;
  }
#else /* FEATURE_WCDMA || FEATURE_LTE */
  (void)cmd_data_ptr;
  (void)cmd_data_len;
  (void)orig_offset;
  (void)next_tlv_offset_ptr;
  (void)more_tlvs_ptr;
  (void)ref_id_ptr;
  (void)nmr_utran_qualifier_ptr;
  (void)need_to_send_immediate_tr_ptr;
  UIM_MSG_HIGH_0(" WCDMA undefined. NMR not supported");
  *general_result_ptr = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  gstk_set_additional_info(additional_info_code_ptr, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
  return GSTK_SUCCESS;
#endif /* FEATURE_WCDMA || FEATURE_LTE */
} /* gstk_process_pli_utran_nmr_req */

/*===========================================================================
FUNCTION gstk_provide_local_info_req

DESCRIPTION
  This function parses the Provide Local Info Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_provide_local_info_req (
  gstk_proactive_cmd_type* command_ptr)
{
  uint8                              *cmd_data_ptr                  = NULL;
  int32                               cmd_data_len                  = 0;
  uint32                              cmd_details_ref_id            = 0;
  gstk_status_enum_type               gstk_status                   = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info               = {0};
  gstk_general_result_enum_type       general_result                = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  gstk_local_info_enum_type           local_info_type_req_from_card = GSTK_NO_LOCAL_INFO;
  boolean                             need_to_send_terminal_rsp     = TRUE;
  int32                               curr_offset                   = 0;
  int32                               new_offset                    = 0;
  boolean                             more_tlv                      = TRUE;
  gstk_nmr_utran_qualifier_enum_type  nmr_utran_qualifier           = GSTK_NMR_NONE_MEAS;
  gstk_generic_data_type              gen_data                      = {0};

  /* Cannot do {0x00} in declaration because of compilation warning
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;
  gstk_local_info_rsp_type           *local_info_ptr                = NULL;
  time_genoff_args_struct_type        genoff_args;
  time_type                           ts_val_usr;

  static uint32                       data_blk                      = 0;
  uint8                               i                             = 0;
#if defined(FEATURE_SEC_TIME)
#if defined(FEATURE_SEC_TIME) && !defined(FEATURE_SEC_TIME_V2)
  uint64                              ms                            = 0;
#endif /* FEATURE_SEC_TIME  && !(FEATURE_SEC_TIME_V2)*/
#endif  /* FEATURE_SEC_TIME */
  dword                               secs                          = 0;
  time_julian_type                    rtc_julian                    = {0};
  gstk_slot_id_enum_type slot        = GSTK_SLOT_ID_MAX;
  sys_modem_as_id_e_type as_id       = SYS_MODEM_AS_ID_1;
#if defined(FEATURE_CDMA)
  db_items_value_type                 db;
#endif /* FEATURE_CDMA */
  int32                               tz_seconds                  = 0;
#ifdef FEATURE_SEC_TIME_V2
  time_type                           time_stamp;
#endif /* FEATURE_SEC_TIME_V2 */
  uint8                               cat_version                 = GSTK_CFG_CAT_VER_MAX;
  gstk_nv_items_data_type             nv_data;


  UIM_MSG_HIGH_1(" IN GSTK_PROVIDE_LOCAL_INFO_REQ: command_ptr=0x%x ",
                 command_ptr);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(command_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);
  cat_version = nv_data.cat_version;

  /* initialize request to 0 */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));

  local_info_ptr = (gstk_local_info_rsp_type *)GSTK_CALLOC(sizeof(gstk_local_info_rsp_type));
  if(local_info_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }

  local_info_ptr->info_type = GSTK_INVALID_LOC_INFO;

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  /* GSTK_SUCCESS from preprocess_command */
  if(gstk_status == GSTK_SUCCESS)
  {
    /* No error has been encountered in parsing */
    /* general_result default to success at the beginning of this function */

    /* get info from respective modules */
    local_info_type_req_from_card =
      (gstk_local_info_enum_type)command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.qualifier;

    /* Note: This piece of code is here to determine the NMR type (whether BCCH or UTRAN)
       **if** the PLI type is for Network Measurement Results.
       This is currently determined by checking for extra tlv
       **the utran measurement qual tlv** that is present for UTRAN NMR
       For all others PLI, the cmd_data_ptr will be NULL
    */
    if((cmd_data_len > 0) && (cmd_data_ptr != NULL))
    {
      gen_data.data_len = GSTK_UTRAN_NMR_QUAL_LEN;
      /* For PLI NMR, the first element of cmd_data_ptr will carry the UTRAN
         measurement qualifier tlv tag
      */
      gen_data.data_buffer_ptr = cmd_data_ptr;
    }
    gstk_map_card_local_info_to_gstk_local_info(
        local_info_type_req_from_card,
        &(local_info_ptr->info_type),
        &gen_data);
    UIM_MSG_HIGH_2("Location Info type = %d, Cat version = %d",
                   local_info_ptr->info_type, cat_version);
    switch(local_info_ptr->info_type)
    {
    case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CDMA
      /* initialize to zero */
      memset(&local_info_ptr->info_data.location_info, 0, sizeof(gstk_location_info_rsp_type));

      general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

      need_to_send_terminal_rsp = TRUE;
        if((gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid == FALSE &&
            gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid == FALSE) ||
           (gstk_curr_inst_ptr->cdma_loc_st_disabled == TRUE))
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        UIM_MSG_ERR_1("Invalid CDMA location info, cdma_loc_st_disabled 0x%x",
                 gstk_curr_inst_ptr->cdma_loc_st_disabled);
      }
      else
      {
        /* This is a structure to structure copy */
        local_info_ptr->info_data.cdma_ext_loc_info = gstk_curr_inst_ptr->cdma_ext_loci_cache.data;
      }
#else  /*#ifdef FEATURE_CDMA */
      UIM_MSG_HIGH_0("Loc Info Type : GSTK_CDMA_LOCATION_INFO - Feature CDMA not supported");
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*#ifdef FEATURE_CDMA */
      break;
    case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined (FEATURE_LTE)
      if(gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_LIMITED_SERVICE)
      {
        general_result = GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE;
      }
      local_info_ptr->info_data.location_info.cell_id_len =
        gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len;

      if(gstk_curr_inst_ptr->card_protocol == MMGSDI_ICC)
      {
          local_info_ptr->info_data.location_info.cell_id_len = GSTK_MIN_CELL_ID_LEN;
      }
      (void)gstk_memcpy(local_info_ptr->info_data.location_info.cell_id,
             gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
             local_info_ptr->info_data.location_info.cell_id_len,
             sizeof(local_info_ptr->info_data.location_info.cell_id),
             gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len);
      (void)memscpy(local_info_ptr->info_data.location_info.mcc_and_mnc,
             sizeof(local_info_ptr->info_data.location_info.mcc_and_mnc),
             gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, 3);
      (void)memscpy(local_info_ptr->info_data.location_info.loc_area_code,
             sizeof(local_info_ptr->info_data.location_info.loc_area_code),
             gstk_curr_inst_ptr->gstk_next_curr_lac, 2);
#else
      UIM_MSG_HIGH_0("Loc Info Type: GSTK_GSM_UMTS_LOCATION_INFO - Feature CDMA not supported");
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* iFEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
      break;

    case GSTK_NO_SRVC_LOCATION_INFO:
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
      break;

    case GSTK_IMEI_INFO:
      /* initialize to zero */
      memset(local_info_ptr->info_data.imei, 0, GSTK_IMEI_LEN);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  || defined (FEATURE_LTE)
      gstk_status = gstk_nv_access_read(GSTK_NV_EXT_SLOT_UE_IMEI_I,
                                        gstk_curr_inst_ptr->slot_id,
                                        &nv_data);

      if (gstk_status == GSTK_SUCCESS)
      {
        (void)memscpy(local_info_ptr->info_data.imei, GSTK_IMEI_LEN, nv_data.ue_imei, GSTK_IMEI_LEN);

        /* Zero out the higest nibble of the last byte
           before sending it to the card */
        local_info_ptr->info_data.imei[GSTK_IMEI_LEN-1] &=  GSTK_LOWER_NIBBLE_MASK;
      }
      else
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
#else
      UIM_MSG_HIGH_0("Loc Info Type: GSTK_GSM_UMTS_LOCATION_INFO - Feature GSM/WCDMA not supported");
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /*FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
      break;
    case GSTK_ESN_INFO:
      if(cat_version >= GSTK_CFG_CAT_VER4)
      {
  #if (defined FEATURE_CDMA) && (defined FEATURE_UIM_RUIM)
        /* Get HW ESN information from NV_ESN_ME_I and not from NV_ESN_I as that can have either
           RUIM-ID or actual HW ESN
        */
        gstk_status = gstk_nv_access_read(GSTK_NV_EXT_ME_ESN_ME_I,
                                          GSTK_SLOT_ID_MAX,
                                          &nv_data);

        if(gstk_status != GSTK_SUCCESS)
        {
          /* ESN Not Read Properly from NV */
          UIM_MSG_HIGH_0("HW ESN info not available");
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        }
        else
        {
          /* initialize to zero */
          memset(local_info_ptr->info_data.esn, 0, GSTK_ESN_LEN);
          (void)memscpy(local_info_ptr->info_data.esn,
                        sizeof(local_info_ptr->info_data.esn),
                        nv_data.esn_me, GSTK_ESN_LEN);
        }
        break;
  #endif
      }
      UIM_MSG_HIGH_0("Loc Info Type : GSTK_ESN_INFO - Feature CDMA/CAT_REL6/UIM_RUIM not supported");
      general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      break;
    case GSTK_IMEISV_INFO:
      if(cat_version >= GSTK_CFG_CAT_VER6)
      {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)  || defined (FEATURE_LTE)
        gstk_status = gstk_nv_access_read(GSTK_NV_EXT_SLOT_UE_IMEISVN_I,
                                          gstk_curr_inst_ptr->slot_id,
                                          &nv_data);

        if (gstk_status == GSTK_SUCCESS)
        {
          (void)memscpy(local_info_ptr->info_data.imei_sv, GSTK_IMEI_SV_LEN, nv_data.ue_imeisvn, GSTK_IMEI_SV_LEN);
        }
        else
        {
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        }
#else
        UIM_MSG_HIGH_0("Loc Info Type : GSTK_IMEISV_INFO - Feature GSM/WCDMA not supported");
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || (FEATURE_LTE) */
      }
      else
      {
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      }
      break;
    case GSTK_MEID_INFO:
      if(cat_version >= GSTK_CFG_CAT_VER6)
      {
  #ifdef FEATURE_CDMA
        gstk_status = gstk_nv_access_read(GSTK_NV_EXT_ME_MEID_I,
                                          GSTK_SLOT_ID_MAX,
                                          &nv_data);

        if (gstk_status == GSTK_SUCCESS)
        {
          memset(local_info_ptr->info_data.meid, 0, GSTK_MEID_LEN);
          (void)memscpy(local_info_ptr->info_data.meid,
                        sizeof(local_info_ptr->info_data.meid),
                        nv_data.meid, GSTK_MEID_LEN);
        }
        else
        {
          UIM_MSG_ERR_0("NV_MEID_ME_I ERROR");
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
          break;
        }
  #else  /*#ifdef FEATURE_CDMA */
       UIM_MSG_HIGH_0("Loc Info Type : GSTK_MEID_INFO - Feature CDMA not supported");
       general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
  #endif /*#ifdef FEATURE_CDMA */
      }
      else
      {
        general_result = GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES;
      }
     break;

    case GSTK_NMR_BCCH_INFO:
      slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
      if (gstk_io_ctrl(GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
                       &slot,
                       &as_id) != GSTK_SUCCESS)
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
        break;
      }

#ifdef FEATURE_GSM
      if(gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NO_SERVICE ||
         gstk_curr_inst_ptr->gstk_next_curr_rat != GSTK_ACCESS_TECH_GSM)
      {
        UIM_MSG_ERR_0("NMR BCCH No Service available");
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
        break;
      }
      else if(gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_LIMITED_SERVICE)
      {
        general_result = GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE;
      }
      memset(&local_info_ptr->info_data.nmr_bcch, 0x00, sizeof(gstk_nmr_bcch_rsp_type));
      if(!rr_ds_api_get_nmr_bcch(local_info_ptr->info_data.nmr_bcch.nmr,
                                 sizeof(local_info_ptr->info_data.nmr_bcch.nmr),
                                 (rr_bcch_list_type *)&local_info_ptr->info_data.nmr_bcch.bcch,
                                 as_id))
      {
        UIM_MSG_ERR_0("rr_api_get_nmr_bcch() failed");
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
      else if(local_info_ptr->info_data.nmr_bcch.bcch.length == 0)
      {
        UIM_MSG_HIGH_0(" EMPTY BCCH LIST FROM RR ");
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
#else /* FEATURE_GSM */
      UIM_MSG_HIGH_0(" GSM undefined. NMR not supported");
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
#endif /* FEATURE_GSM */
      break; /* case GSTK_NMR_BCCH_INFO */
    case GSTK_NMR_UTRAN_INFO:
      gstk_status = gstk_process_pli_utran_nmr_req(cmd_data_ptr,
                                                   cmd_data_len,
                                                   curr_offset,
                                                   &new_offset,
                                                   &more_tlv,
                                                   &cmd_details_ref_id,
                                                   &nmr_utran_qualifier,
                                                   &need_to_send_terminal_rsp,
                                                   &general_result,
                                                   &additional_info);
      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_0("gstk_process_pli_utran_nmr_req() did not return success");
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        need_to_send_terminal_rsp = TRUE;
      }
      break; /* case GSTK_NMR_UTRAN_INFO */

    case GSTK_DATE_TIME_ZONE_INFO:
      /* initialize to zero */
      memset(&local_info_ptr->info_data.time, 0, sizeof(gstk_date_time_rsp_type));
      /* initialize TZ */
      local_info_ptr->info_data.time.zone = (uint8)0xFF;

      /*
        Set TZ information if available
      */
      if(gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_NONE)
      {
        UIM_MSG_HIGH_0("No network, time zone defaulted to 0xFF");
      }
      else
      {
        for (i = 0; i < GSTK_CM_NUM_STACKS; ++i)
        {
          if (gstk_curr_inst_ptr->gstk_next_curr_rat ==
              gstk_curr_inst_ptr->tz_info[i].access_tech)
          {
            if (gstk_curr_inst_ptr->tz_info[i].tz_avail == GSTK_TZ_AVAILABLE)
            {
              switch (gstk_curr_inst_ptr->gstk_next_curr_rat)
              {
                case GSTK_ACCESS_TECH_GSM:
                case GSTK_ACCESS_TECH_UTRAN:
                case GSTK_ACCESS_TECH_LTE:
                  local_info_ptr->info_data.time.zone = gstk_curr_inst_ptr->tz_info[i].tz;
                  UIM_MSG_HIGH_1("Setting G/W/L time zone to: 0x%x ",
                                 local_info_ptr->info_data.time.zone);
                  tz_seconds = (int32)(gstk_curr_inst_ptr->tz_info[i].tz * 60 * 15);
                  break;
                 case GSTK_ACCESS_TECH_CDMA:
                 case GSTK_ACCESS_TECH_HRPD:
                 case GSTK_ACCESS_TECH_EHRPD:
                   local_info_ptr->info_data.time.zone = (uint8)0xFF;
#ifdef FEATURE_CDMA
                   /* Time zone from CP is 6 bit 2's compliment number in 30 minute
                      increments */
                   db.ltm_off = 0;
                   db_get(DB_LTM_OFF, &db);
                   tz_seconds = local_info_ptr->info_data.time.zone = (uint8)(db.ltm_off & 0x3F);
                   if (tz_seconds >= 32)
                   {
                     tz_seconds -= 64;
                   }
                   tz_seconds = (int32)(tz_seconds * 60 * 30);
#endif /* FEATURE_CDMA */
                   UIM_MSG_HIGH_1("Setting CDMA time zone to: 0x%x ",
                                  local_info_ptr->info_data.time.zone);
                   break;
                 default:
                   UIM_MSG_ERR_0("Not expected to be here");
                   local_info_ptr->info_data.time.zone = (uint8)0xFF;
                   break;
              }
            }
            else
            {
              UIM_MSG_HIGH_2("Time Zone not available. Access Tech: 0x%x TZ avail 0x%x",
                             gstk_curr_inst_ptr->gstk_next_curr_rat,
                             gstk_curr_inst_ptr->tz_info[i].tz_avail);
              local_info_ptr->info_data.time.zone = (uint8)0xFF;
            }
            break;
          } /* if (stk_next_curr_rat == tz_info[i].access_tech) */
        } /* for (i = 0; i < GSTK_CM_NUM_STACKS; ++i) */
      } /* else */

      /*
        Set Time and Date information
      */
#if defined(FEATURE_SEC_TIME) && !defined(FEATURE_SEC_TIME_V2)
      UIM_MSG_HIGH_0("FEATURE_SEC_TIME && !FEATURE_SEC_TIME_V2");
      memset(&genoff_args, 0, sizeof (genoff_args));
      memset(ts_val_usr, 0, sizeof(ts_val_usr));
      genoff_args.base        = ATS_USER;
      genoff_args.base_source = TIME_SCLK;
      genoff_args.ts_val      = ts_val_usr;
      genoff_args.unit        = TIME_STAMP;
      genoff_args.operation   = T_GET;
      time_genoff_opr(&genoff_args);
      secs = time_conv_to_sec(genoff_args.ts_val) + tz_seconds;
      time_jul_from_secs(secs, &rtc_julian);
#elif (defined(FEATURE_SEC_TIME_V2))
      UIM_MSG_HIGH_0("FEATURE_SEC_TIME_V2 ");
      memset(&genoff_args, 0, sizeof (genoff_args));
      memset(ts_val_usr, 0, sizeof(ts_val_usr));
      genoff_args.base        = ATS_USER;
      genoff_args.base_source = TIME_SCLK;
      genoff_args.ts_val      = ts_val_usr;
      genoff_args.unit        = TIME_STAMP;
      genoff_args.operation   = T_GET;
      time_genoff_opr(&genoff_args);
      secs = time_conv_to_sec(genoff_args.ts_val) + tz_seconds;
      time_jul_from_secs(secs, &rtc_julian);
#else
      memset(&genoff_args, 0, sizeof (genoff_args));
      memset(ts_val_usr, 0, sizeof(ts_val_usr));
      genoff_args.base        = ATS_USER;
      genoff_args.base_source = TIME_SCLK;
      genoff_args.ts_val      = ts_val_usr;
      genoff_args.unit        = TIME_STAMP;
      genoff_args.operation   = T_GET;
      time_genoff_opr(&genoff_args);
      secs = time_conv_to_sec(genoff_args.ts_val) + tz_seconds;
      time_jul_from_secs(secs, &rtc_julian);
#endif

      local_info_ptr->info_data.time.day += ((uint8)rtc_julian.day/10);
      local_info_ptr->info_data.time.day += (((uint8)rtc_julian.day%10) << 4);

      local_info_ptr->info_data.time.hour += ((uint8)rtc_julian.hour/10);
      local_info_ptr->info_data.time.hour += (((uint8)rtc_julian.hour%10) << 4);

      local_info_ptr->info_data.time.minute += ((uint8)rtc_julian.minute/10);
      local_info_ptr->info_data.time.minute += (((uint8)rtc_julian.minute%10) << 4);

      local_info_ptr->info_data.time.month += ((uint8)rtc_julian.month/10);
      local_info_ptr->info_data.time.month += (((uint8)rtc_julian.month%10) << 4);

      local_info_ptr->info_data.time.second += ((uint8)rtc_julian.second/10);
      local_info_ptr->info_data.time.second += (((uint8)rtc_julian.second%10) << 4);

      local_info_ptr->info_data.time.year += (uint8)(((word)rtc_julian.year%100) / 10);
      local_info_ptr->info_data.time.year += (uint8)((((word)rtc_julian.year%100) % 10) << 4);

      break;

    case GSTK_LANGUAGE_SETTING_INFO:
      request.hdr_cmd.command_id = GSTK_PROVIDE_LANG_INFO_REQ;
      request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
      request.hdr_cmd.command_number =
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

      /* initialize to zero */
      memset(local_info_ptr->info_data.lang_code, 0, 2);
      /* Send command to clients */
      gstk_status = gstk_util_send_message_to_clients(&request);
      UIM_MSG_HIGH_1("Client status %d", gstk_status);
      if(gstk_status == GSTK_NO_CLIENT_REG_FOUND)
      {
        general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

        (void)gstk_nv_access_read(
                GSTK_NV_EXT_ME_QMI_CAT_DEFAULT_LANG,
                GSTK_SLOT_ID_MAX,
                &nv_data);

        local_info_ptr->info_data.lang_code[0] = nv_data.qmi_cat_default_lang[0];
        local_info_ptr->info_data.lang_code[1] = nv_data.qmi_cat_default_lang[1];
        need_to_send_terminal_rsp = TRUE;
      }
      else if(gstk_status != GSTK_SUCCESS)
      {
        /* Send Terminal Response to Card */
        /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        /* Send Terminal Response to Card */
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
      else
      {
        /* Start Timer */
        gstk_util_start_client_response_timer(cmd_details_ref_id,
                                              GSTK_TERM_RSP_WAIT_TIME);
        /* reset gstk_status to SUCCESS since there has been no error*/
        /* This is needed since some of the TLVs are optional, which might
        ** cause the various parsing functions to return invalid_command */
        need_to_send_terminal_rsp = FALSE;
        gstk_status = GSTK_SUCCESS;
      }
      break;

    case GSTK_TIMING_ADVANCE_INFO:

      /* initialize to zero */
      memset(&local_info_ptr->info_data.time_adv, 0, sizeof(gstk_time_advance_rsp_type));
      slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
      if (gstk_io_ctrl(GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
                       &slot,
                       &as_id) != GSTK_SUCCESS)
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
        break;
      }

#ifdef FEATURE_GSM
      if(!rr_ds_get_timing_advance(
            &local_info_ptr->info_data.time_adv.me_status,
            &local_info_ptr->info_data.time_adv.timing_advance,
            as_id))
      {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      }
#else
      UIM_MSG_HIGH_0("PROVIDE LOCAL INFO unsupported GSM call");
#endif /* FEATURE_GSM */
      break;

    case GSTK_ACCESS_TECHNOLOGY_INFO:
      if (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_NONE)
        {
        general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
        gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
      }
      else
      {
        general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        local_info_ptr->info_data.gstk_access_tech = gstk_curr_inst_ptr->gstk_next_curr_rat;
      }
      need_to_send_terminal_rsp = TRUE;
      break;
    case GSTK_BATTERY_STATUS_INFO:
      if(cat_version >= GSTK_CFG_CAT_VER6)
      {
        request.hdr_cmd.command_id = GSTK_PROVIDE_BATTERY_STATUS_REQ;
        request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
        request.hdr_cmd.command_number =
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number;

        /* initialize to zero */
        memset(&local_info_ptr->info_data.battery_status, 0, sizeof(uint8));
        /* Send command to clients */
        gstk_status = gstk_util_send_message_to_clients(&request);
        if(gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_HIGH_0("Provide Local Info - Battery Status not able to send to client");
          /* Send Terminal Response to Card */
          /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          /* Send Terminal Response to Card */
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        }
        else
        {
          /* Start Timer */
          gstk_util_start_client_response_timer(cmd_details_ref_id,
                                                GSTK_TERM_RSP_WAIT_TIME);
          /* reset gstk_status to SUCCESS since there has been no error*/
          /* This is needed since some of the TLVs are optional, which might
          ** cause the various parsing functions to return invalid_command */
          need_to_send_terminal_rsp = FALSE;
          gstk_status = GSTK_SUCCESS;
        }
      }
      else
      {
        general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
      }
      break;
    case GSTK_NETWORK_SEARCH_MODE_INFO:
      if(cat_version >= GSTK_CFG_CAT_VER6)
      {
        data_blk = cmd_details_ref_id;
        if(gstk_curr_inst_ptr->nw_search_mode != GSTK_NW_SEARCH_MODE_MAX)
        {
          general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          local_info_ptr->info_data.nw_search_mode =  gstk_curr_inst_ptr->nw_search_mode;
          need_to_send_terminal_rsp = TRUE;
        }
        else if(cm_ph_cmd_get_ph_info(
               gstk_cm_ph_error_event_cb, /* cmd callback */
             (void *)&data_blk,                                                  /* data block ptr */
             gstk_shared_data.gstk_client_id_from_cm) == FALSE)
        {
          need_to_send_terminal_rsp = TRUE;
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        }
        else
        {
          need_to_send_terminal_rsp = FALSE;
        }
      }
      else
      {
        general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
      }
      break;
    case GSTK_CSG_CELL_INFO:
        data_blk = cmd_details_ref_id;
        if(gstk_curr_inst_ptr->csg_info.access_tech == GSTK_ACCESS_NONE)
        {
          need_to_send_terminal_rsp = TRUE;
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SERVICE_AVAILABLE);
          break;
        }

        else if(gstk_curr_inst_ptr->available_csg_list.length > 0)
        {
          (void)gstk_memcpy(&local_info_ptr->info_data.csg_info,
                                &(gstk_curr_inst_ptr->available_csg_list),
                                sizeof(sys_detailed_plmn_list_s_type),
                                sizeof(sys_detailed_plmn_list_s_type),
                                sizeof(sys_detailed_plmn_list_s_type));
          need_to_send_terminal_rsp = TRUE;
          general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        }
        else if(cm_ph_cmd_get_ph_info(
               gstk_cm_ph_error_event_cb, /* cmd callback */
               (void *)&data_blk,                    /* data block ptr */
               gstk_shared_data.gstk_client_id_from_cm) == FALSE)
        {
          need_to_send_terminal_rsp = TRUE;
          general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
        }
        else
        {
          need_to_send_terminal_rsp = FALSE;
        }
      break;
    default:
      UIM_MSG_HIGH_0("PROVIDE LOCAL INFO TLV parsing");
      local_info_ptr->info_type = GSTK_INVALID_LOC_INFO;
      /* Send Terminal Response to Card */
      /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
      break;
    }
  } /* GSTK_SUCCESS from preprocess_command */
  else { /* parsing error */
     UIM_MSG_ERR_0("PROVIDE LOCAL INFO TLV parsing err");
     /* Send Terminal Response to Card */
     /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
     general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if no error from preprocess command */

  /* send the Terminal Response */
  if (need_to_send_terminal_rsp) {
    gstk_status = gstk_send_provide_local_info_terminal_rsp(
                           cmd_details_ref_id, /* command_details_ref */
                           command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].command_details.command_number,
                           /* command_num */
                           general_result, /* general_result */
                           &additional_info, /* additional result */
                           local_info_ptr /* local info union pointer */ );
    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_HIGH_0("Can't send term rsp");
    }
  }

  /* nothing to release in memory location */
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }

  gstk_free(local_info_ptr);
  local_info_ptr = NULL;

  return gstk_status;

} /* gstk_provide_local_info_req */

/*===========================================================================
FUNCTION gstk_provide_local_info_cnf

DESCRIPTION
  This function packs the provide local info terminal response and send
  the command to UIM server.

PARAMETERS
  provide_local_info_cnf_ptr: [Input] Pointer to message that is required to
                                      be processed and sent to UIM

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
gstk_status_enum_type gstk_provide_local_info_cnf(
  gstk_provide_local_info_cnf_type* provide_local_info_cnf_ptr)
{

  gstk_status_enum_type                       gstk_status   = GSTK_SUCCESS;
  provide_local_info_terminal_response_type   STK_response  = {0};
  uim_cmd_type                                *uim_cmd_ptr  = NULL;
  uint8                                       header_len    = 0;
  int                                         offset        = 0;
  uint32                                      ref_id        = 0;
  gstk_nv_items_data_type                     nv_data;

  UIM_MSG_HIGH_1("IN GSTK provide local info cnf: provide_local_info_cnf_ptr=0x%x ",
                 provide_local_info_cnf_ptr);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(provide_local_info_cnf_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* initialize STK_response_ptr */
  memset(&STK_response, 0x00, sizeof(provide_local_info_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      provide_local_info_cnf_ptr->cmd_details_ref_id,
      provide_local_info_cnf_ptr->message_header.command_id,
      provide_local_info_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      provide_local_info_cnf_ptr->command_result,
      provide_local_info_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  header_len = STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;

  /* Local Info */
  UIM_MSG_HIGH_2("GSTK provide local info cnf info type=%d, CAT version=%d",
                 provide_local_info_cnf_ptr->local_info_rsp.info_type,
                 nv_data.cat_version);
  switch(provide_local_info_cnf_ptr->local_info_rsp.info_type) {
    case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined (FEATURE_LTE)
      gstk_status = gstk_packer_location_info_tlv(
          &provide_local_info_cnf_ptr->local_info_rsp.info_data.location_info,
          &STK_response.local_info_rsp.location_information);
      STK_response.num_bytes = header_len +
        STK_response.local_info_rsp.location_information.length +
        2;
#else
      STK_response.num_bytes = header_len;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
      break;
    case GSTK_CDMA_LOCATION_INFO:
#if defined(FEATURE_CDMA)
      gstk_status = gstk_packer_3gpp2_location_info_tlv(
        &gstk_curr_inst_ptr->cdma_ext_loci_cache.data,
        &gstk_curr_inst_ptr->cdma_ext_loci_cache,
        &STK_response.local_info_rsp.cdma_ext_loc_info);
      if (gstk_status != GSTK_SUCCESS)
      {
        return GSTK_ERROR;
      }
      STK_response.num_bytes = header_len +
        STK_response.local_info_rsp.cdma_ext_loc_info.length
        + 2;
#endif
      break;
    case GSTK_NO_SRVC_LOCATION_INFO:
      STK_response.num_bytes = header_len;
      break;
    case GSTK_IMEI_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined (FEATURE_LTE)
      gstk_status = gstk_packer_imei_tlv(
        provide_local_info_cnf_ptr->local_info_rsp.info_data.imei,
        &STK_response.local_info_rsp.imei);
      STK_response.num_bytes = header_len + GSTK_IMEI_LEN + 2;
#else
      STK_response.num_bytes = header_len;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
      break;
    case GSTK_ESN_INFO:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER4)
      {
#if defined(FEATURE_CDMA)
        gstk_status = gstk_packer_esn_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.esn,
          &STK_response.local_info_rsp.esn);
        STK_response.num_bytes = header_len + GSTK_ESN_LEN + 2;
#endif /* FEATURE_CDMA */
      }
      else
      {
        STK_response.num_bytes = header_len;
      }
      break;
    case GSTK_IMEISV_INFO:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
        gstk_status = gstk_packer_imeisv_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.imei_sv,
          &STK_response.local_info_rsp.imeisv);
        STK_response.num_bytes = header_len + GSTK_IMEI_SV_LEN + 2;
#endif /*(FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE)*/
      }
      else
      {
        STK_response.num_bytes = header_len;
      }
      break;
    case GSTK_MEID_INFO:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
#if defined(FEATURE_CDMA)
        gstk_status = gstk_packer_meid_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.meid,
          &STK_response.local_info_rsp.meid);
        STK_response.num_bytes = header_len + GSTK_MEID_LEN + 2;
#endif /* FEATURE_CDMA */
      }
      else
      {
        STK_response.num_bytes = header_len;
      }
      break;
    case GSTK_NMR_BCCH_INFO:
#if defined(FEATURE_GSM)
      /* NMR */
      UIM_MSG_HIGH_1("Provide Local Info cnf: Result = 0x%x",
                     provide_local_info_cnf_ptr->command_result);
      STK_response.num_bytes = header_len;
      if ((provide_local_info_cnf_ptr->command_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ||
          (provide_local_info_cnf_ptr->command_result == GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE))
      {
        gstk_status = gstk_packer_nmr_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.nmr,
          &STK_response.local_info_rsp.nmr_bcch.network_measurement);
        STK_response.num_bytes = STK_response.num_bytes +
                                 GSTK_NMR_LEN +
                                 GSTK_TAG_LENGTH_LEN;
        /* BCCH */
        UIM_MSG_HIGH_1("Provide Local Info cnf: BCCH Length = 0x%x",
                       provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.bcch.length);
        STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.channel_list =
          GSTK_CALLOC(provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.bcch.length);
        if(STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.channel_list == NULL)
        {
          gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
          return GSTK_MEMORY_ERROR;
        }
        gstk_status = gstk_packer_bcch_list_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.bcch.length,
          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.bcch.bcch_list,
          &STK_response.local_info_rsp.nmr_bcch.bcch_ch_list);
        STK_response.num_bytes = STK_response.num_bytes +
                                 STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.length +
                                 GSTK_TAG_LENGTH_LEN;
      }
#else
      STK_response.num_bytes = header_len;
#endif /* FEATURE_GSM */
      break;
    case GSTK_NMR_UTRAN_INFO:
#if (defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
      STK_response.num_bytes = header_len;
      if((provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length > 0)
         &&
         (provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.nmr_ptr))
      {
        switch (provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.nmr_utran_qualifier)
        {
        case GSTK_NMR_LTE_INTER_FREQ_MEAS:
          /* Handle multiple NMR */
          {
            uint8  *nmr_buf_ptr  = NULL;
            int               i  = 0;

            nmr_buf_ptr = provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.nmr_ptr;
            STK_response.local_info_rsp.nmr_utran_info.num_nmr = nmr_buf_ptr[0];

            UIM_MSG_HIGH_1("LTE NMR Inter Freq meas: num of reports = 0x%x", nmr_buf_ptr[0]);

            if (NULL == (STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr =
                         GSTK_CALLOC(nmr_buf_ptr[0] * sizeof(gstk_nmr_utran_tlv_type))))
            {
              gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
              return GSTK_MEMORY_ERROR;
            }

            offset = 1;
            for (i = 0; ((i < STK_response.local_info_rsp.nmr_utran_info.num_nmr)
                         && (offset < provide_local_info_cnf_ptr->
                             local_info_rsp.info_data.nmr_utran.length));
                 i++)
            {
              /*LTE RRC pack each report size as 2 bytes int */
              uint16 rpt_len = 0;
              uint32 freq    = 0;
              uint16 freq_16 = 0;

              if (offset >= (provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length - 1))
              {
                UIM_MSG_ERR_2("Couldn't parse all meas reports: %d, %d", i, offset);
                gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
                return GSTK_BAD_PARAM;
              }

              /*LTE RRC pack each report size as 2 bytes */
              gstk_memcpy(&rpt_len,
                          &nmr_buf_ptr[offset],
                          2,
                          sizeof(rpt_len),
                          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length - offset);
              UIM_MSG_HIGH_3("Meas report: %d, rpt_len: 0x%x, offset: 0x%x", i, rpt_len, offset);

              if (rpt_len > 0x7f) /* spec has report size as 1 byte */
              {
                gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
                return GSTK_PARAM_EXCEED_BOUNDARY;
              }
              if (NULL == (STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].measurement_report_ptr =
                           GSTK_CALLOC(rpt_len)))
              {
                gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
                return GSTK_MEMORY_ERROR;
              }

              /* Copy Freq (2 bytes) + Meas report
                 May need to convert Freq to byte order if LTE does not change
                 the data to byte order */
              offset += 2;

              /* LTE Meas report has 4 byte freq - SIM Toolkit spec currently
                 support only 2 byte frequency */
              /************ Manipulation start ************/
              /* Manipulate the Freq in the original report for gstk_packer_nmr_utran_tlv()
                 to work as is */
              gstk_memcpy(&freq,
                          &nmr_buf_ptr[offset],
                          4,
                          sizeof(freq),
                          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length - offset);
              /* Ensure frequency doesn't cross the range 0 - 65535 */
              if (freq > 65535)
              {
                freq = freq % 65536;
              }
              freq_16 = uint32touint16(freq);

              offset += 2;
              gstk_memcpy(&nmr_buf_ptr[offset],
                          &freq_16,
                          2,
                          provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length - offset, sizeof(freq_16));
              rpt_len -= 2;
              /************ Manipulation end ************/

              STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].length = uint16touint8(rpt_len);
              gstk_status = gstk_packer_nmr_utran_tlv(
                 (uint32)(rpt_len),
                 &nmr_buf_ptr[offset],
                 &STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i]);
              STK_response.num_bytes = STK_response.num_bytes +
                                       uint16touint8(rpt_len) +
                                       GSTK_TAG_LENGTH_LEN;
              offset += rpt_len;
            }
          }
          break;
        default:
          STK_response.local_info_rsp.nmr_utran_info.num_nmr = 1;
          if (NULL == (STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr =
                       GSTK_CALLOC(sizeof(*STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr))))
          {
            gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
            return GSTK_MEMORY_ERROR;
          }
          if (NULL == (STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr->measurement_report_ptr =
                       GSTK_CALLOC(provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length)))
          {
            gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
            return GSTK_MEMORY_ERROR;
          }
          /* No truncation of bytes will occur due to uint32touint8 conversion as check for
           0xFF has already been done by the caller function */
          STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr->length =
            uint32touint8(provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length);

          gstk_status = gstk_packer_nmr_utran_tlv(
            provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length,
            provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.nmr_ptr,
              STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr);
          STK_response.num_bytes = STK_response.num_bytes +
                                   uint32touint8(provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_utran.length) +
                                   GSTK_TAG_LENGTH_LEN;
          break;
        }
      }
#else
      STK_response.num_bytes = header_len;
#endif /*(FEATURE_WCDMA || FEATURE_LTE) */
      break;
    case GSTK_DATE_TIME_ZONE_INFO:
      gstk_status = gstk_packer_date_time_tlv(
          &provide_local_info_cnf_ptr->local_info_rsp.info_data.time,
          &STK_response.local_info_rsp.date_time_zone);
      STK_response.num_bytes = header_len + 7 + 2;
      break;
    case GSTK_LANGUAGE_SETTING_INFO:
      gstk_status = gstk_packer_lang_sel_tlv(
          provide_local_info_cnf_ptr->local_info_rsp.info_data.lang_code,
          &STK_response.local_info_rsp.language);
      STK_response.num_bytes = header_len + 2 + 2;
      break;
    case GSTK_TIMING_ADVANCE_INFO:
      gstk_status = gstk_packer_timing_advance_tlv(
          &provide_local_info_cnf_ptr->local_info_rsp.info_data.time_adv,
          &STK_response.local_info_rsp.timing_advance);
      STK_response.num_bytes = header_len + 2 + 2;
      break;
    case GSTK_ACCESS_TECHNOLOGY_INFO:
      gstk_status = gstk_packer_access_technology_tlv(
          (uint8*)&provide_local_info_cnf_ptr->local_info_rsp.info_data.gstk_access_tech,
          &STK_response.local_info_rsp.access_technology);
      STK_response.num_bytes = header_len + 2 + 1;
      break;
    case GSTK_BATTERY_STATUS_INFO:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
        gstk_status = gstk_packer_battery_status_tlv(
            (uint8*)&provide_local_info_cnf_ptr->local_info_rsp.info_data.battery_status,
            &STK_response.local_info_rsp.battery_status);
        STK_response.num_bytes = header_len + 2 + 1;
      }
      else
      {
        STK_response.num_bytes = header_len;
      }
      break;
    case GSTK_NETWORK_SEARCH_MODE_INFO:
      STK_response.num_bytes = header_len;
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
        gstk_status = gstk_packer_nw_search_mode_tlv(
            provide_local_info_cnf_ptr->local_info_rsp.info_data.nw_search_mode,
            &STK_response.local_info_rsp.nw_search_mode);
        STK_response.num_bytes = STK_response.num_bytes +
                                 STK_response.local_info_rsp.nw_search_mode.length +
                                 GSTK_TAG_LENGTH_LEN;
      }
      break;
    case GSTK_CSG_CELL_INFO:
      STK_response.num_bytes = header_len;
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER9)
      {
        gstk_status = gstk_packer_csg_cell_tlv(
            provide_local_info_cnf_ptr->local_info_rsp.info_data.csg_info,
            &STK_response.local_info_rsp.csg_info);
        STK_response.num_bytes = STK_response.num_bytes +
                                 STK_response.local_info_rsp.csg_info.length +
                                 GSTK_TAG_LENGTH_LEN;
        if (STK_response.local_info_rsp.csg_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
        {
          STK_response.num_bytes ++;
        }
      }
      break;
    default:
      UIM_MSG_HIGH_0("PROVIDE LOCAL INFO TLV packing");
      STK_response.num_bytes = header_len;
      break;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,  &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = provide_local_info_cnf_ptr->cmd_details_ref_id;

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        offset = 0;
        /* copy command details */
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                   &STK_response.header.command_details,
                                   offset,
                                   (size_t)(STK_response.header.command_details.length + 2),
                                   sizeof(uim_cmd_ptr->terminal_response.data),
                                   sizeof(gstk_command_details_tlv_type)) <
           (size_t)(STK_response.header.command_details.length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy device */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.device_id,
                                        offset +=
                                        (int)(STK_response.header.command_details.length + 2),
                                        (size_t)(STK_response.header.device_id.device_tag_length + 2),
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_device_identities_tag_tlv_type)) <
                (size_t)(STK_response.header.device_id.device_tag_length + 2))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        /* copy result */
        else if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.header.result,
                                        offset +=
                                        (int)(STK_response.header.device_id.device_tag_length + 2),
                                        3, /* result tag +  length + general result */
                                        sizeof(uim_cmd_ptr->terminal_response.data),
                                        sizeof(gstk_result_tag_tlv_type)) < 3)
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = offset + 3;
        if(gstk_status == GSTK_SUCCESS &&
           STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
          if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                     STK_response.header.result.additional_info,
                                     offset,
                                     (size_t)(STK_response.header.result.result_tag_length - 1),
                                     sizeof(uim_cmd_ptr->terminal_response.data),
                                     (provide_local_info_cnf_ptr->result_additional_info.length *
                                      sizeof(byte))) <
             (size_t)(STK_response.header.result.result_tag_length - 1))
          {
            UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
            gstk_status = GSTK_MEMORY_ERROR;
          }
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        /* local info */
        if (gstk_status == GSTK_SUCCESS &&
            STK_response.num_bytes > header_len) { /* has local info */
          switch(provide_local_info_cnf_ptr->local_info_rsp.info_type) {
            case GSTK_GSM_UMTS_LOCATION_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.location_information,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_location_information_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
              }
#endif /* (FEATURE_GSM) || (FEATURE_WCDMA) || FEATURE_LTE */
              break;
            case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CDMA
              if ((offset + GSTK_TAG_LENGTH_LEN) < UIM_MAX_TR_BYTES) {
                uim_cmd_ptr->terminal_response.data[offset++] = STK_response.local_info_rsp.cdma_ext_loc_info.tag;
                uim_cmd_ptr->terminal_response.data[offset++] = STK_response.local_info_rsp.cdma_ext_loc_info.length;
              }
              else
              {
                UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): fail to copy CDMA location info");
                gstk_status = GSTK_MEMORY_ERROR;
              }

              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         STK_response.local_info_rsp.cdma_ext_loc_info.loc_info,
                                         offset,
                                         (size_t)(STK_response.local_info_rsp.cdma_ext_loc_info.length),
                                         UIM_MAX_TR_BYTES,
                                         (size_t)(STK_response.local_info_rsp.cdma_ext_loc_info.length)) <
                 (size_t)(STK_response.local_info_rsp.cdma_ext_loc_info.length))
              {
                UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): fail to copy CDMA location info");
                gstk_status = GSTK_MEMORY_ERROR;
              }
#endif /* #ifdef FEATURE_CDMA */
              break;
            case GSTK_NO_SRVC_LOCATION_INFO:
              break;
            case GSTK_IMEI_INFO:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.imei,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_imei_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
#endif /*(FEATURE_GSM) || FEATURE_WCDMA || FEATURE_LTE */
              break;

          case GSTK_IMEISV_INFO:
            if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
            {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.imeisv,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_imeisv_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
#endif /*# FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
            }
            break;
            case GSTK_ESN_INFO:
              if(nv_data.cat_version >= GSTK_CFG_CAT_VER4)
              {
#ifdef FEATURE_CDMA
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           &STK_response.local_info_rsp.esn,
                                           header_len,
                                           (size_t)(STK_response.num_bytes - header_len),
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           sizeof(gstk_esn_tlv_type)) <
                   (size_t)(STK_response.num_bytes - header_len))
                {
                  gstk_status = GSTK_MEMORY_ERROR;
                }
#endif /*#ifdef FEATURE_CDMA */
              }
              break;
            case GSTK_MEID_INFO:
              if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
              {
#ifdef FEATURE_CDMA
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           &STK_response.local_info_rsp.meid,
                                           header_len,
                                           (size_t)(STK_response.num_bytes - header_len),
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           sizeof(gstk_meid_tlv_type)) <
                   (size_t)(STK_response.num_bytes - header_len))
                {
                  gstk_status = GSTK_MEMORY_ERROR;
                }
#endif /*#ifdef FEATURE_CDMA */
              }
              break;
            case GSTK_NMR_BCCH_INFO:
#ifdef FEATURE_GSM
              /* NMR */

              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.nmr_bcch.network_measurement,
                                         header_len,
                                         (size_t)(STK_response.local_info_rsp.nmr_bcch.network_measurement.length + 2),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_nmr_tlv_type)) <
                 (size_t)(STK_response.local_info_rsp.nmr_bcch.network_measurement.length + 2))
              {
                gstk_status = GSTK_MEMORY_ERROR;
                break;
              }
              header_len += STK_response.local_info_rsp.nmr_bcch.network_measurement.length + 2;
              /* BCCH */
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.nmr_bcch.bcch_ch_list,
                                         header_len,
                                         GSTK_TAG_LENGTH_LEN , /* tag , length */
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_bcch_ch_list_tlv_type)) < GSTK_TAG_LENGTH_LEN)
              {
                UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
                gstk_status = GSTK_MEMORY_ERROR;
                break;
              }
              header_len += GSTK_TAG_LENGTH_LEN;

              if(STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.length > 0) {
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.channel_list,
                                           header_len,
                                           STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.length,
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           provide_local_info_cnf_ptr->local_info_rsp.info_data.nmr_bcch.bcch.length) <
                   STK_response.local_info_rsp.nmr_bcch.bcch_ch_list.length)
                {
                  UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                  break;
                }
              }
#else  /* FEATURE_GSM */
              UIM_MSG_HIGH_0("FEATURE GSM Not Defined");
#endif /* FEATURE_GSM */
              break;
            case GSTK_NMR_UTRAN_INFO:
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
              if (STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr)
              {
                int i = 0;
                for (i = 0; i < STK_response.local_info_rsp.nmr_utran_info.num_nmr; i++)
                {
                  if(gstk_byte_offset_memcpy(
                       uim_cmd_ptr->terminal_response.data, /* dest */
                       &STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i],/* src */
                       header_len,                                                /* offset */
                       GSTK_TAG_LENGTH_LEN,                                       /* copy size */
                       UIM_MAX_TR_BYTES,                                            /* dest max size */
                       sizeof(gstk_nmr_utran_tlv_type) +
                       STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].length) /* src max size */
                     <
                     GSTK_TAG_LENGTH_LEN)
                  {
                    gstk_status = GSTK_MEMORY_ERROR;
                    break;
                  }
                  header_len += GSTK_TAG_LENGTH_LEN ;
                  if(STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].length > 0)
                  {
                    if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,     /* dest */
                                               STK_response.local_info_rsp.nmr_utran_info.
                                               nmr_utran_ptr[i].measurement_report_ptr,  /* src */
                                               header_len,              /* offset */
                                               STK_response.local_info_rsp.nmr_utran_info.
                                               nmr_utran_ptr[i].length, /* copy size */
                                               UIM_MAX_TR_BYTES,        /* dest max size */
                                               STK_response.local_info_rsp.nmr_utran_info.
                                               nmr_utran_ptr[i].length) /* src max size */
                       <
                       STK_response.local_info_rsp.nmr_utran_info.nmr_utran_ptr[i].length)
                    {
                      UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
                      gstk_status = GSTK_MEMORY_ERROR;
                      break;
                    }

                    header_len += STK_response.local_info_rsp.nmr_utran_info.
                      nmr_utran_ptr[i].length;
                  }
                }
              }
#endif /* FEATURE_WCDMA || FEATURE_LTE */
              break;
            case GSTK_DATE_TIME_ZONE_INFO:
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.date_time_zone,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_date_time_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
              break;
            case GSTK_LANGUAGE_SETTING_INFO:
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.language,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_lang_tlv_type)) <
                (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
              break;
            case GSTK_TIMING_ADVANCE_INFO:
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.timing_advance,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_timing_advance_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
              break;
            case GSTK_ACCESS_TECHNOLOGY_INFO:
              if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                         &STK_response.local_info_rsp.access_technology,
                                         header_len,
                                         (size_t)(STK_response.num_bytes - header_len),
                                         sizeof(uim_cmd_ptr->terminal_response.data),
                                         sizeof(gstk_access_technology_tlv_type)) <
                 (size_t)(STK_response.num_bytes - header_len))
              {
                gstk_status = GSTK_MEMORY_ERROR;
              }
              break;
            case GSTK_BATTERY_STATUS_INFO:
              if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
              {
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           &STK_response.local_info_rsp.battery_status,
                                           header_len,
                                           (size_t)(STK_response.num_bytes - header_len),
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           sizeof(gstk_battery_status_tlv_type)) <
                   (size_t)(STK_response.num_bytes - header_len))
                {
                  gstk_status = GSTK_MEMORY_ERROR;
                }
              }
              break;
            case GSTK_NETWORK_SEARCH_MODE_INFO:
              if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
              {
                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           &STK_response.local_info_rsp.nw_search_mode,
                                           header_len,
                                           (size_t)(STK_response.local_info_rsp.nw_search_mode.length + 2),
                                           sizeof(uim_cmd_ptr->terminal_response.data),
                                           (size_t)(STK_response.local_info_rsp.nw_search_mode.length + 2)) <
                   (size_t)(STK_response.local_info_rsp.nw_search_mode.length + 2))
                {
                  gstk_status = GSTK_MEMORY_ERROR;
                }
              }
              break;
            case GSTK_CSG_CELL_INFO:
              if(nv_data.cat_version >= GSTK_CFG_CAT_VER9)
              {
                uim_cmd_ptr->terminal_response.data[offset++] = STK_response.local_info_rsp.csg_info.tag;

                if (STK_response.local_info_rsp.csg_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
                {
                  uim_cmd_ptr->terminal_response.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
                }
                uim_cmd_ptr->terminal_response.data[offset++] = STK_response.local_info_rsp.csg_info.length;

                if(gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                           STK_response.local_info_rsp.csg_info.csg_info,
                                           offset,
                                           (size_t)(STK_response.local_info_rsp.csg_info.length),
                                           UIM_MAX_TR_BYTES,
                                           (size_t)(STK_response.local_info_rsp.csg_info.length)) <
                   (size_t)(STK_response.local_info_rsp.csg_info.length))
                {
                  UIM_MSG_ERR_0("gstk_provide_local_info_cnf(): gstk_byte_offset_memcpy failed");
                  gstk_status = GSTK_MEMORY_ERROR;
                }
              }
            default:
              break;
          }
        }

        if (gstk_status == GSTK_SUCCESS)
        {
          gstk_util_dump_byte_array("Provide Local Info TR",
                                    uim_cmd_ptr->terminal_response.data,
                                    uim_cmd_ptr->terminal_response.num_bytes);

          gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
        }
        else
        {
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          uim_cmd_ptr = NULL;
        }
      }

    }
  } /* uim_cmd_ptr != NULL */
  else {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_provide_local_info_cnf_cleanup(provide_local_info_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_provide_local_info_cnf*/

/*===========================================================================
FUNCTION gstk_map_card_local_info_to_gstk_local_info

DESCRIPTION
  This function maps the local info type request from the card to the
  gstk local info type

PARAMETERS
   local_info_type_req_from_card: [Input] info type request from the card
   *info_type_ptr               : [Input]Pointer togstk info type request
   *data_ptr                    : [Input] Pointer to data

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_map_card_local_info_to_gstk_local_info(
  gstk_local_info_enum_type       local_info_type_req_from_card,
  gstk_local_info_gen_enum_type  *info_type_ptr,
  const gstk_generic_data_type   *data_ptr
)
{
#ifdef FEATURE_CDMA
  uint16                  curr_3gpp2_rat = 0;
#endif
  gstk_nv_items_data_type nv_data;

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  if(info_type_ptr == NULL)
  {
    return;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  UIM_MSG_HIGH_2("Local Info req from card: 0x%x, CAT version: %d",
                 local_info_type_req_from_card,
                 nv_data.cat_version);
  switch(local_info_type_req_from_card)
  {
  case GSTK_LOCATION_INFO:
    UIM_MSG_HIGH_2("gstk_next_curr_location_status(0x%x), gstk_next_curr_rat(0x%x)",
                   gstk_curr_inst_ptr->gstk_next_curr_location_status,
                   gstk_curr_inst_ptr->gstk_next_curr_rat);
#ifdef FEATURE_CDMA
    curr_3gpp2_rat = gstk_curr_inst_ptr->bmsk_3gpp2 & GSTK_ACCESS_TECH_3GPP2_MASK;
    UIM_MSG_HIGH_2("bmsk_3gpp2 0x%x curr_3gpp2_rat 0x%x",
                   gstk_curr_inst_ptr->bmsk_3gpp2, curr_3gpp2_rat);
#endif
    if((gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NO_SERVICE ||
        gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_NONE))
    {
      *info_type_ptr = GSTK_NO_SRVC_LOCATION_INFO;
    }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined (FEATURE_LTE)
    else if((gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_GSM)   ||
            (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_UTRAN) ||
            (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_LTE))
    {
      *info_type_ptr = GSTK_GSM_UMTS_LOCATION_INFO;
    }
#endif /* #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined (FEATURE_LTE) */
    else if(gstk_curr_inst_ptr->gstk_next_curr_rat ==  GSTK_ACCESS_TECH_CDMA ||
             gstk_curr_inst_ptr->gstk_next_curr_rat ==  GSTK_ACCESS_TECH_HRPD ||
             gstk_curr_inst_ptr->gstk_next_curr_rat ==  GSTK_ACCESS_TECH_EHRPD)
    {
#ifdef FEATURE_CDMA
      if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO,
                                     gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        *info_type_ptr = GSTK_CDMA_LOCATION_INFO;
      }
      else if (curr_3gpp2_rat == GSTK_ACCESS_TECH_CDMA_MASK ||
               curr_3gpp2_rat == GSTK_ACCESS_TECH_CDMA_HRPD_MASK ||
               curr_3gpp2_rat == GSTK_ACCESS_TECH_CDMA_EHRPD_MASK)
      {
         /* Send CDMA Location Information for (E)HRPD in case of CDMA + HDR */
         /* Send error TR in case of HDR Only */
        *info_type_ptr = GSTK_CDMA_LOCATION_INFO;
      }
#endif /* FEATURE_CDMA */
    }
    break;
  case GSTK_IMEI:
    *info_type_ptr = GSTK_IMEI_INFO;
    break;
  case GSTK_NMR:
    /* check if extra tlv is present */
    if(data_ptr)
    {
      if(data_ptr->data_len > 0 && data_ptr->data_buffer_ptr != NULL)
      {
        /* checking if extra tlv is utran tlv */
        if(gstk_valid_tag(data_ptr->data_buffer_ptr[0],GSTK_UTRAN_MEASUREMENT_TAG))
        {
          *info_type_ptr = GSTK_NMR_UTRAN_INFO;
          break;
        }
      }
    }
    *info_type_ptr = GSTK_NMR_BCCH_INFO;
    break;
  case GSTK_DATE_TIME_ZONE:
    *info_type_ptr = GSTK_DATE_TIME_ZONE_INFO;
    break;
  case GSTK_LANGUAGE_SETTING:
    *info_type_ptr = GSTK_LANGUAGE_SETTING_INFO;
    break;
  case GSTK_TIMING_ADVANCE:
    *info_type_ptr = GSTK_TIMING_ADVANCE_INFO;
    break;
  case GSTK_ACCESS_TECHNOLOGY:
    *info_type_ptr = GSTK_ACCESS_TECHNOLOGY_INFO;
    break;
  case GSTK_ESN:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER4)
    {
#ifdef FEATURE_CDMA
      *info_type_ptr = GSTK_ESN_INFO;
#else /* #ifdef FEATURE_CDMA */
      UIM_MSG_HIGH_0("GSTK_ESN : Feature CDMA not supported");
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CDMA */
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  case GSTK_IMEISV:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
    {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      *info_type_ptr = GSTK_IMEISV_INFO;
#else
      UIM_MSG_HIGH_0("GSTK_IMEISV : Feature GSM/WCDMA/LTE not supported");
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  case GSTK_BATTERY_STATUS:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
    {
      *info_type_ptr = GSTK_BATTERY_STATUS_INFO;
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  case GSTK_MEID:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
    {
#ifdef FEATURE_CDMA
      *info_type_ptr = GSTK_MEID_INFO;
#else /* #ifdef FEATURE_CDMA */
      UIM_MSG_HIGH_0("GSTK_MEID : Feature CDMA not supported");
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
#endif /* #ifdef FEATURE_CDMA */
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  case GSTK_NETWORK_SEARCH_MODE:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
    {
      *info_type_ptr = GSTK_NETWORK_SEARCH_MODE_INFO;
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  case GSTK_CSG_CELL:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER9)
    {
      *info_type_ptr = GSTK_CSG_CELL_INFO;
    }
    else
    {
      *info_type_ptr = GSTK_INVALID_LOC_INFO;
    }
    break;
  default:
    *info_type_ptr = GSTK_INVALID_LOC_INFO;
    break;
  }
}/* gstk_map_card_local_info_to_gstk_local_info */
